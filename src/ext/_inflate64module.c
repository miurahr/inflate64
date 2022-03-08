/* deflate64 module for Python 3.6+
   ---
   Borrows BlocksOutputBuffer, unused data buffer functions
   from pyzstd module - BSD-3 licensed by Ma Lin.
   https://github.com/animalize/pyzstd
 */

#include "Python.h"
#include "pythread.h"   /* For Python 3.6 */
#include "structmember.h"

#if defined(_WIN32) && defined(timezone)
#undef timezone
#endif

#ifndef Py_UNREACHABLE
    #define Py_UNREACHABLE() assert(0)
#endif

#include "zutil.h"
#include "inflate9.h"

#define True 1
#define False 0

typedef struct {
    PyObject_HEAD

    /* Unconsumed input data */
    char *input_buffer;
    size_t input_buffer_size;
    size_t in_begin, in_end;

    z_stream* strm;
    PyObject* window_buffer;

    /* Thread lock for decompressing */
    PyThread_type_lock lock;

    /* 0 if decompressor has (or may has) unconsumed input data, 0 or 1. */
    char needs_input;

    /* 1 when end mark observed */
    char eof;

    /* __init__ has been called, 0 or 1. */
    char inited;
} Inflater;

typedef struct {
    PyTypeObject *Inflate64_type;
    PyObject *Inflate64Error;
} _inflate64_state;

static _inflate64_state static_state;

#define ACQUIRE_LOCK(obj) do {                    \
    if (!PyThread_acquire_lock((obj)->lock, 0)) { \
        Py_BEGIN_ALLOW_THREADS                    \
        PyThread_acquire_lock((obj)->lock, 1);    \
        Py_END_ALLOW_THREADS                      \
    } } while (0)
#define RELEASE_LOCK(obj) PyThread_release_lock((obj)->lock)

static const char init_twice_msg[] = "__init__ method is called twice.";

static PyObject *
Inflater_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Inflater *self;
    self = (Inflater *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    assert(self->inited == 0);

    /* Thread lock */
    self->lock = PyThread_allocate_lock();
    if (self->lock == NULL) {
        goto error;
    }
    return (PyObject*)self;

error:
    Py_XDECREF(self);
    return PyErr_NoMemory();
}

static void
Inflater_dealloc(Inflater *self) {
    if (self->lock) {
        PyThread_free_lock(self->lock);
    }

    if (self->strm != NULL) {
        int err = inflate9End(self->strm);
        switch (err) {
            case Z_OK:
                break;
            case Z_STREAM_ERROR:
            default:
                PyErr_BadInternalCall();
                break;
        }
    }
    Py_XDECREF(self->window_buffer);
    PyMem_Free(self->strm);

    PyTypeObject *tp = Py_TYPE(self);
    tp->tp_free((PyObject*)self);
    Py_DECREF(tp);
}

static voidpf zlib_alloc(voidpf opaque, uInt items, uInt size) {
    // For safety, give zlib a zero-initialized memory block
    // Also, PyMem_Calloc call does an overflow-safe maximum size check
    void* address = PyMem_Calloc(items, size);
    if (address == NULL) {
        // For safety, don't assume Z_NULL is the same as NULL
        return Z_NULL;
    }

    return address;
}

static void zlib_free(voidpf opaque, voidpf address) {
    PyMem_Free(address);
}

PyDoc_STRVAR(Inflater_doc, "A Deflate64 inflater.\n\n"
                                 "Inflater.__init__(self)\n"
                                 );

static int
Inflater_init(Inflater *self, PyObject *args, PyObject *kwargs)
{

    /* Only called once */
    if (self->inited) {
        PyErr_SetString(PyExc_RuntimeError, init_twice_msg);
        goto error;
    }

    self->strm = PyMem_Calloc(1, sizeof(z_stream));
    if (self->strm == NULL) {
        PyErr_NoMemory();
        return -1;
    }
    self->strm->opaque = NULL;
    self->strm->zalloc = zlib_alloc;
    self->strm->zfree = zlib_free;

    self->inited = 1;
    self->needs_input = 1;

    self->window_buffer = PyBytes_FromStringAndSize(NULL, 64 << 10);
    int err = inflate9Init(self->strm, (unsigned char*) PyBytes_AS_STRING(self->window_buffer));
    switch (err) {
        case Z_OK:
            // Success
            goto success;
        case Z_MEM_ERROR:
            // The internal state could not be allocated
            PyErr_NoMemory();
        // Fatal errors
        case Z_STREAM_ERROR:
            // Some parameters are invalid
        case Z_VERSION_ERROR:
            // The version of the library does not match the version of the header file
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
Inflater_inflate(Inflater *self,  PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"data", "length", NULL};
    Py_buffer data;
    int length = -1;
    PyObject *ret = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,
                                     "y*|i:Inflater.inflate", kwlist,
                                     &data, &length)) {
        return NULL;
    }

    ACQUIRE_LOCK(self);

    if (data.len  > 0) {
        int result;
        Py_BEGIN_ALLOW_THREADS
        result = inflate9(self->strm);
        Py_END_ALLOW_THREADS

        if (result == -1) {
            self->eof = True;
        }
        Py_CLEAR(ret);
    }
    goto success;

//error:
    /* Reset variables */
//    self->eof = True;
//    self->needs_input = False;
    Py_CLEAR(ret);

success:
    RELEASE_LOCK(self);
    PyBuffer_Release(&data);
    return ret;
}

static PyMethodDef Inflater_methods[] = {
        {"inflate", (PyCFunction)Inflater_inflate,
                     METH_VARARGS|METH_KEYWORDS, Inflater_inflate_doc},
        {NULL, NULL, 0, NULL}
};

PyDoc_STRVAR(Inflater_eof__doc, "True if the end-of-stream marker has been reached.");
PyDoc_STRVAR(Inflater_needs_input_doc, "True if more input is needed before more decompressed data can be produced.");

static PyMemberDef Inflater_members[] = {
    {"eof", T_BOOL, offsetof(Inflater, eof),
     READONLY, Inflater_eof__doc},

    {"needs_input", T_BOOL, offsetof(Inflater, needs_input),
     READONLY, Inflater_needs_input_doc},

    {NULL}
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

static PyType_Spec Inflater_type_spec = {
        .name = "_inflate64.Inflater",
        .basicsize = sizeof(Inflater),
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
