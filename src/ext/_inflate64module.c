/* deflate64 module for Python 3.6+
   ---
   Borrows BlocksOutputBuffer, unused data buffer functions
   from pyzstd module - BSD-3 licensed by Ma Lin.
   https://github.com/animalize/pyzstd
 */

#include "Python.h"
#include "pythread.h"   /* For Python 3.6 */
#include "structmember.h"

#include "inflate64.h"

#if defined(_WIN32) && defined(timezone)
#undef timezone
#endif

#ifndef Py_UNREACHABLE
    #define Py_UNREACHABLE() assert(0)
#endif

#define True 1
#define False 0

#include "_blocks_output_buffer.h"

#if OUTPUT_BUFFER_MAX_BLOCK_SIZE > UINT32_MAX
#error "The maximum block size accepted by zlib is UINT32_MAX."
#endif

/* On success, return value >= 0
   On failure, return -1 */
static inline Py_ssize_t
OutputBuffer_InitAndGrow(_BlocksOutputBuffer *buffer, Py_ssize_t max_length,
                         Byte FAR **next_out, uint32_t *avail_out)
{
    Py_ssize_t allocated;

    allocated = _BlocksOutputBuffer_InitAndGrow(
            buffer, max_length, (void**) next_out);
    *avail_out = (uint32_t) allocated;
    return allocated;
}

/* On success, return value >= 0
   On failure, return -1 */
static inline Py_ssize_t
OutputBuffer_Grow(_BlocksOutputBuffer *buffer,
                  Byte FAR **next_out, uint32_t *avail_out)
{
    Py_ssize_t allocated;

    allocated = _BlocksOutputBuffer_Grow(
            buffer, (void**) next_out, (Py_ssize_t) *avail_out);
    *avail_out = (uint32_t) allocated;
    return allocated;
}

static inline Py_ssize_t
OutputBuffer_GetDataSize(_BlocksOutputBuffer *buffer, uint32_t avail_out)
{
    return _BlocksOutputBuffer_GetDataSize(buffer, (Py_ssize_t) avail_out);
}

static inline PyObject *
OutputBuffer_Finish(_BlocksOutputBuffer *buffer, uint32_t avail_out)
{
    return _BlocksOutputBuffer_Finish(buffer, (Py_ssize_t) avail_out);
}

static inline void
OutputBuffer_OnError(_BlocksOutputBuffer *buffer)
{
    _BlocksOutputBuffer_OnError(buffer);
}

/* The max buffer size accepted by zlib is UINT32_MAX, the initial buffer size
   `init_size` may > it in 64-bit build. These wrapper functions maintain an
   UINT32_MAX sliding window for the first block:
    1. OutputBuffer_WindowInitWithSize()
    2. OutputBuffer_WindowGrow()
    3. OutputBuffer_WindowFinish()
    4. OutputBuffer_WindowOnError()

   ==== is the sliding window:
    1. ====------
           ^ next_posi, left_bytes is 6
    2. ----====--
               ^ next_posi, left_bytes is 2
    3. --------==
                 ^ next_posi, left_bytes is 0  */
typedef struct {
    Py_ssize_t left_bytes;
    Byte FAR *next_posi;
} _Uint32Window;

/* Initialize the buffer with an initial buffer size.

   On success, return value >= 0
   On failure, return value < 0 */
static inline Py_ssize_t
OutputBuffer_WindowInitWithSize(_BlocksOutputBuffer *buffer, _Uint32Window *window,
                                Py_ssize_t init_size,
                                Byte FAR **next_out, uint32_t *avail_out)
{
    Py_ssize_t allocated = _BlocksOutputBuffer_InitWithSize(
            buffer, init_size, (void**) next_out);

    if (allocated >= 0) {
        // the UINT32_MAX sliding window
        Py_ssize_t window_size = Py_MIN((size_t)allocated, UINT32_MAX);
        *avail_out = (uint32_t) window_size;

        window->left_bytes = allocated - window_size;
        window->next_posi = *next_out + window_size;
    }
    return allocated;
}

/* Grow the buffer.

   On success, return value >= 0
   On failure, return value < 0 */
static inline Py_ssize_t
OutputBuffer_WindowGrow(_BlocksOutputBuffer *buffer, _Uint32Window *window,
                        Byte FAR **next_out, uint32_t *avail_out)
{
    Py_ssize_t allocated;

    /* ensure no gaps in the data.
       if inlined, this check could be optimized away.*/
    if (*avail_out != 0) {
        PyErr_SetString(PyExc_SystemError,
                        "*avail_out != 0 in OutputBuffer_WindowGrow().");
        return -1;
    }

    // slide the UINT32_MAX sliding window
    if (window->left_bytes > 0) {
        Py_ssize_t window_size = Py_MIN((size_t)window->left_bytes, UINT32_MAX);

        *next_out = window->next_posi;
        *avail_out = (uint32_t) window_size;

        window->left_bytes -= window_size;
        window->next_posi += window_size;

        return window_size;
    }
    assert(window->left_bytes == 0);

    // only the first block may > UINT32_MAX
    allocated = _BlocksOutputBuffer_Grow(
            buffer, (void**) next_out, (Py_ssize_t) *avail_out);
    *avail_out = (uint32_t) allocated;
    return allocated;
}

/* Finish the buffer.

   On success, return a bytes object
   On failure, return NULL */
static inline PyObject *
OutputBuffer_WindowFinish(_BlocksOutputBuffer *buffer, _Uint32Window *window,
                          uint32_t avail_out)
{
    Py_ssize_t real_avail_out = (Py_ssize_t) avail_out + window->left_bytes;
    return _BlocksOutputBuffer_Finish(buffer, real_avail_out);
}

static inline void
OutputBuffer_WindowOnError(_BlocksOutputBuffer *buffer, _Uint32Window *window)
{
    _BlocksOutputBuffer_OnError(buffer);
}

typedef struct
{
    PyObject_HEAD
    z_stream zst;
    PyObject *unused_data;
    PyObject *unconsumed_tail;
    char eof;
    int is_initialised;
    PyObject *zdict;
    PyThread_type_lock lock;
} compobject;

typedef struct {
    PyTypeObject *Deflate64_type;
    PyTypeObject *Inflate64_type;
    PyObject *Inflate64Error;
} _inflate64_state;

static _inflate64_state static_state;

static compobject *
newcompobject(PyTypeObject *type)
{
    compobject *self;
    self = (compobject *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->eof = 0;
    self->is_initialised = 0;
    self->zdict = NULL;
    self->unused_data = PyBytes_FromStringAndSize("", 0);
    if (self->unused_data == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    self->unconsumed_tail = PyBytes_FromStringAndSize("", 0);
    if (self->unconsumed_tail == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    self->lock = PyThread_allocate_lock();
    if (self->lock == NULL) {
        Py_DECREF(self);
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate lock");
        return NULL;
    }
    return self;
}

#define ACQUIRE_LOCK(obj) do {                    \
    if (!PyThread_acquire_lock((obj)->lock, 0)) { \
        Py_BEGIN_ALLOW_THREADS                    \
        PyThread_acquire_lock((obj)->lock, 1);    \
        Py_END_ALLOW_THREADS                      \
    } } while (0)
#define RELEASE_LOCK(obj) PyThread_release_lock((obj)->lock)

static const char init_twice_msg[] = "__init__ method is called twice.";

static voidpf zlib_alloc(voidpf opaque, uInt items, uInt size) {
    // For safety, give zlib a zero-initialized memory block
    // Also, PyMem_Calloc call does an overflow-safe maximum size check
    void* address = PyMem_RawCalloc(items, size);
    if (address == NULL) {
        // For safety, don't assume Z_NULL is the same as NULL
        return Z_NULL;
    }

    return address;
}

static void zlib_free(voidpf opaque, voidpf address) {
    PyMem_RawFree(address);
}

static void
arrange_input_buffer(z_stream *zst, Py_ssize_t *remains)
{
    zst->avail_in = (uInt)Py_MIN((size_t)*remains, UINT_MAX);
    *remains -= zst->avail_in;
}

/* Helper for objdecompress() and flush(). Saves any unconsumed input data in
   self->unused_data or self->unconsumed_tail, as appropriate. */
static int
save_unconsumed_input(compobject *self, Py_buffer *data, int err)
{
    if (err == Z_STREAM_END) {
        /* The end of the compressed data has been reached. Store the leftover
           input data in self->unused_data. */
        if (self->zst.avail_in > 0) {
            Py_ssize_t old_size = PyBytes_GET_SIZE(self->unused_data);
            Py_ssize_t new_size, left_size;
            PyObject *new_data;
            left_size = (Byte *)data->buf + data->len - self->zst.next_in;
            if (left_size > (PY_SSIZE_T_MAX - old_size)) {
                PyErr_NoMemory();
                return -1;
            }
            new_size = old_size + left_size;
            new_data = PyBytes_FromStringAndSize(NULL, new_size);
            if (new_data == NULL)
                return -1;
            memcpy(PyBytes_AS_STRING(new_data),
                      PyBytes_AS_STRING(self->unused_data), old_size);
            memcpy(PyBytes_AS_STRING(new_data) + old_size,
                      self->zst.next_in, left_size);
            Py_SETREF(self->unused_data, new_data);
            self->zst.avail_in = 0;
        }
    }

    if (self->zst.avail_in > 0 || PyBytes_GET_SIZE(self->unconsumed_tail)) {
        /* This code handles two distinct cases:
           1. Output limit was reached. Save leftover input in unconsumed_tail.
           2. All input data was consumed. Clear unconsumed_tail. */
        Py_ssize_t left_size = (Byte *)data->buf + data->len - self->zst.next_in;
        PyObject *new_data = PyBytes_FromStringAndSize(
                (char *)self->zst.next_in, left_size);
        if (new_data == NULL)
            return -1;
        Py_SETREF(self->unconsumed_tail, new_data);
    }

    return 0;
}

static PyObject *
Deflater_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    compobject *self;
    self = newcompobject(type);
    return (PyObject*)self;
}

static void
Deflater_dealloc(compobject *self)
{
    if (self->lock) {
        PyThread_free_lock(self->lock);
    }
    if (self->is_initialised) {
        int err = deflate9End(&self->zst);
        switch (err) {
            case Z_OK:
                break;
            case Z_DATA_ERROR:
                PyErr_SetString(PyExc_IOError,
                                "The stream was freed prematurely (some input or output was discarded).");
                break;
            case Z_STREAM_ERROR:
                PyErr_SetString(PyExc_IOError, "The stream state was inconsistent.");
                break;
            default:
                PyErr_BadInternalCall();
                break;
        }
    }
    PyTypeObject *tp = Py_TYPE(self);
    tp->tp_free((PyObject*)self);
    Py_DECREF(tp);
}

PyDoc_STRVAR(Deflater_doc, "A Deflate64 deflater.\n\n"
                           "Deflater.__init__(self)\n");

static int
Deflater_init(compobject *self, PyObject *args, PyObject *kwargs)
{
    /* only called once */
    if (self->is_initialised) {
        PyErr_SetString(PyExc_RuntimeError, init_twice_msg);
        goto error;
    }

    self->zst.zalloc = zlib_alloc;
    self->zst.zfree = zlib_free;

    self->is_initialised = 1;

    int err = deflate9Init2(&self->zst);
    switch (err) {
        case Z_OK:
            goto success;
        case Z_MEM_ERROR:
            PyErr_NoMemory();
            break;
        case Z_STREAM_ERROR:
        default:
            PyErr_BadInternalCall();
    }

error:
    return -1;

success:
    return 0;
}

PyDoc_STRVAR(Deflater_deflate_doc, "deflate()\n"
                                   "----\n"
                                   "Deflate data with a Deflate64 compression.");

static PyObject *
Deflater_deflate(compobject *self, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"data", NULL};
    Py_buffer data;
    PyObject *RetVal = NULL;
    int err;
    _BlocksOutputBuffer buffer = {.list = NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,
                                     "y*:Deflater.deflate",
                                     kwlist, &data)) {
        PyErr_Format(PyExc_ValueError, "Argument error");
        return NULL;
    }

    ACQUIRE_LOCK(self);

    self->zst.next_in = data.buf;
    Py_ssize_t ibuflen = data.len;

    if (OutputBuffer_InitAndGrow(&buffer, -1, &self->zst.next_out, &self->zst.avail_out) < 0) {
        goto error;
    }

    do {
        arrange_input_buffer(&self->zst, &ibuflen);

        do {
            if (self->zst.avail_out == 0) {
                if (OutputBuffer_Grow(&buffer, &self->zst.next_out, &self->zst.avail_out) < 0) {
                    goto error;
                }
            }

            Py_BEGIN_ALLOW_THREADS
            err = deflate9(&self->zst, Z_NO_FLUSH);
            Py_END_ALLOW_THREADS

            if (err == Z_STREAM_ERROR) {
                goto error;
            }

        } while (self->zst.avail_out == 0);
        assert(self->zst.avail_in == 0);

    } while (ibuflen != 0);

    RetVal = OutputBuffer_Finish(&buffer, self->zst.avail_out);
    if (RetVal != NULL) {
        goto success;
    }

error:
    OutputBuffer_OnError(&buffer);
    RetVal = NULL;
 success:
    RELEASE_LOCK(self);
    return RetVal;
}

PyDoc_STRVAR(Deflater_flush_doc, "flush()\n"
                                   "----\n"
                                   "Flush compressed data.");

static PyObject *
Deflater_flush(compobject *self, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"mode", NULL};
    int err;
    int mode = Z_FINISH;
    PyObject *RetVal = NULL;
    _BlocksOutputBuffer buffer = {.list = NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,
                                     "|i:Deflater.flush",
                                     kwlist, &mode)) {
        PyErr_Format(PyExc_ValueError, "Argument error");
        return NULL;
    }

    /*
     * flushing with Z_NO_FLUSH is a non-op, so there's no point in
     * doing any work at all; just return with empty data.
     */
    if (mode == Z_NO_FLUSH) {
        return PyBytes_FromStringAndSize(NULL, 0);
    }

    ACQUIRE_LOCK(self);

    self->zst.next_in = NULL;
    self->zst.avail_in = 0;

    if (OutputBuffer_InitAndGrow(&buffer, -1, &self->zst.next_out, &self->zst.avail_out) < 0) {
        PyErr_NoMemory();
        goto error;
    }

    do {
        if (self->zst.avail_out == 0) {
            if (OutputBuffer_Grow(&buffer, &self->zst.next_out, &self->zst.avail_out) < 0) {
                PyErr_NoMemory();
                goto error;
            }
        }
        Py_BEGIN_ALLOW_THREADS
            err = deflate9(&self->zst, mode);
        Py_END_ALLOW_THREADS

        if (err == Z_STREAM_ERROR) {
            PyErr_Format(PyExc_RuntimeError, "deflater9 return an unexpected return code %d\n", err);
            goto error;
        }
    } while (self->zst.avail_out == 0);

    if (err == Z_STREAM_END && mode == Z_FINISH) {
        err = deflate9End(&self->zst);
        self->eof = 1;
        if (err != Z_OK) {
            PyErr_Format(PyExc_RuntimeError, "deflater9End return an unexpected return code %d\n", err);
            goto error;
        } else {
            self->is_initialised = 0;
        }
    } else if (err != Z_OK && err != Z_BUF_ERROR){
        PyErr_Format(PyExc_RuntimeError, "Deflater.flush got unexpected return code %d\n", err);
        goto error;
    }

    RetVal = OutputBuffer_Finish(&buffer, self->zst.avail_out);
    if (RetVal != NULL) {
        goto success;
    }

error:
    OutputBuffer_OnError(&buffer);
    RetVal = NULL;

success:
    RELEASE_LOCK(self);
    return RetVal;
}

static PyObject *
Inflater_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    compobject *self;
    self = newcompobject(type);
    return (PyObject*)self;
}

static void
Inflater_dealloc(compobject *self) {
    if (self->lock) {
        PyThread_free_lock(self->lock);
    }

    int err = inflate9End(&self->zst);
    switch (err) {
        case Z_OK:
            break;
        case Z_STREAM_ERROR:
        default:
            PyErr_BadInternalCall();
            break;
    }

    PyTypeObject *tp = Py_TYPE(self);
    tp->tp_free((PyObject*)self);
    Py_DECREF(tp);
}

PyDoc_STRVAR(Inflater_doc, "A Deflate64 inflater.\n\n"
                                 "Inflater.__init__(self)\n"
                                 );

static int
Inflater_init(compobject *self, PyObject *args, PyObject *kwargs)
{
    /* Only called once */
    if (self->is_initialised) {
        PyErr_SetString(PyExc_RuntimeError, init_twice_msg);
        goto error;
    }
    self->zst.opaque = NULL;
    self->zst.zalloc = zlib_alloc;
    self->zst.zfree = zlib_free;
    self->zst.next_in = NULL;
    self->zst.avail_in = 0;
    int err = inflate9Init2(&self->zst);
    switch (err) {
        case Z_OK:
            self->is_initialised = 1;
            goto success;
        case Z_STREAM_ERROR:
        case Z_MEM_ERROR:
            PyErr_NoMemory();
            goto error;
        default:
            PyErr_BadInternalCall();
    }

error:
    return -1;

success:
    return 0;
}

PyDoc_STRVAR(Inflater_inflate_doc, "inflate()\n"
             "----\n"
             "Inflate a Deflate64 compressed data.");

static PyObject *
Inflater_inflate(compobject *self,  PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"data", "max_length", NULL};
    Py_buffer data;
    int max_length = 0;
    PyObject *RetVal = NULL;
    _BlocksOutputBuffer buffer = {.list = NULL};
    Py_ssize_t ibuflen;
    int err = Z_OK;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,
                                     "y*|i:Inflater.inflate", kwlist,
                                     &data, &max_length)) {
        return NULL;
    }

    if (max_length < 0) {
        PyErr_SetString(PyExc_ValueError, "max_length must be non-negative");
        return NULL;
    } else if (max_length == 0) {
        max_length = -1;
    }

    self->zst.next_in = data.buf;
    ibuflen = data.len;

    if (OutputBuffer_InitAndGrow(&buffer, max_length, &self->zst.next_out, &self->zst.avail_out) < 0) {
        goto abort;
    }

    ACQUIRE_LOCK(self);

    do {
        arrange_input_buffer(&self->zst, &ibuflen);

        do {
            if (self->zst.avail_out == 0) {
                if (OutputBuffer_GetDataSize(&buffer, self->zst.avail_out) == max_length) {
                    goto save;
                }
                if (OutputBuffer_Grow(&buffer, &self->zst.next_out, &self->zst.avail_out) < 0) {
                    goto abort;
                }
            }

            Py_BEGIN_ALLOW_THREADS
            err = inflate9(&self->zst, Z_NO_FLUSH);
            Py_END_ALLOW_THREADS

            switch (err) {
                case Z_OK:            /* fall through */
                case Z_BUF_ERROR:     /* fall through */
                case Z_STREAM_END:
                    break;
                default:
                    goto save;
            }
        } while (self->zst.avail_out == 0);
    } while (err != Z_STREAM_END && ibuflen != 0);

save:
    if (save_unconsumed_input(self, &data, err) < 0)
        goto abort;

    if (err == Z_STREAM_END) {
        self->eof = 1;
    } else if (err != Z_OK && err != Z_BUF_ERROR) {
        // Z_STREAM_ERROR (-2)
        // Z_DATA_ERROR   (-3): case BAD
        // Z_MEM_ERROR    (-4): case MEM
        PyErr_Format(PyExc_ValueError, "while decompressing data: error code is %d", err);
        goto abort;
    }
    RetVal = OutputBuffer_Finish(&buffer, self->zst.avail_out);
    if (RetVal != NULL) {
        goto success;
    }

 abort:
    OutputBuffer_OnError(&buffer);
    RetVal = NULL;
 success:
    RELEASE_LOCK(self);
    return RetVal;
}


static PyMethodDef Deflater_methods[] = {
        {"deflate", (PyCFunction)Deflater_deflate,
                METH_VARARGS|METH_KEYWORDS, Deflater_deflate_doc},
        {"flush", (PyCFunction)Deflater_flush,
                METH_VARARGS|METH_KEYWORDS, Deflater_flush_doc},
        {NULL, NULL, 0, NULL}
};

PyDoc_STRVAR(Deflater_eof__doc, "True if the end-of-stream marker has been reached.");

static PyMemberDef Deflater_members[] = {
        {"eof", T_BOOL, offsetof(compobject, eof),
                READONLY, Deflater_eof__doc},
        {NULL}
};

static PyMethodDef Inflater_methods[] = {
        {"inflate", (PyCFunction)Inflater_inflate,
                     METH_VARARGS|METH_KEYWORDS, Inflater_inflate_doc},
        {NULL, NULL, 0, NULL}
};

PyDoc_STRVAR(Inflater_eof__doc, "True if the end-of-stream marker has been reached.");

static PyMemberDef Inflater_members[] = {
    {"eof", T_BOOL, offsetof(compobject, eof),
     READONLY, Inflater_eof__doc},
    {NULL}
};


static PyType_Slot Deflater_slots[] = {
        {Py_tp_new, Deflater_new},
        {Py_tp_dealloc, Deflater_dealloc},
        {Py_tp_init, Deflater_init},
        {Py_tp_methods, Deflater_methods},
        {Py_tp_members, Deflater_members},
        {Py_tp_doc, (char *)Deflater_doc},
        {0, 0}
};

static PyType_Slot Inflater_slots[] = {
    {Py_tp_new, Inflater_new},
    {Py_tp_dealloc, Inflater_dealloc},
    {Py_tp_init, Inflater_init},
    {Py_tp_methods, Inflater_methods},
    {Py_tp_members, Inflater_members},
    {Py_tp_doc, (char *)Inflater_doc},
    {0, 0}
};

static PyType_Spec Deflater_type_spec = {
        .name = "_inflate64.Deflater",
        .basicsize = sizeof(compobject),
        .flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .slots = Deflater_slots,
};

static PyType_Spec Inflater_type_spec = {
        .name = "_inflate64.Inflater",
        .basicsize = sizeof(compobject),
        .flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .slots = Inflater_slots,
};

/* --------------------
     Initialize code
   -------------------- */

static int
_inflate64_traverse(PyObject *module, visitproc visit, void *arg)
{
    Py_VISIT(static_state.Inflate64Error);
    Py_VISIT(static_state.Inflate64_type);
    return 0;
}

static int
_inflate64_clear(PyObject *module)
{
    Py_CLEAR(static_state.Inflate64Error);
    Py_CLEAR(static_state.Inflate64_type);
    return 0;
}

static void
_inflate64_free(void *module) {
    _inflate64_clear((PyObject *)module);
}

static PyModuleDef _inflate64module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "_inflate64",
    .m_size = -1,
    .m_traverse = _inflate64_traverse,
    .m_clear = _inflate64_clear,
    .m_free = _inflate64_free
};


static inline int
add_type_to_module(PyObject *module, const char *name,
                   PyType_Spec *type_spec, PyTypeObject **dest)
{
    PyObject *temp;

    temp = PyType_FromSpec(type_spec);
    if (PyModule_AddObject(module, name, temp) < 0) {
        Py_XDECREF(temp);
        return -1;
    }

    Py_INCREF(temp);
    *dest = (PyTypeObject*) temp;

    return 0;
}

PyMODINIT_FUNC
PyInit__inflate64(void) {
    PyObject *module;

    module = PyModule_Create(&_inflate64module);
    if (!module) {
        goto error;
    }
    if (add_type_to_module(module,
                           "Deflater",
                           &Deflater_type_spec,
                           &static_state.Inflate64_type) < 0) {
        goto error;
    }
    if (add_type_to_module(module,
                           "Inflater",
                           &Inflater_type_spec,
                           &static_state.Inflate64_type) < 0) {
        goto error;
    }
    return module;

error:
     _inflate64_clear(NULL);
     Py_XDECREF(module);

     return NULL;
}
