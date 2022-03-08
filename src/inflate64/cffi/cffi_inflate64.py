from threading import Lock

from ._cffi_inflate64 import ffi, lib

__all__ = (
    "Inflate64Error",
)


_BLOCK_SIZE = 16384
_allocated = []

CFFI_INFLATE64 = True

_new_nonzero = ffi.new_allocator(should_clear_after_alloc=False)


class Inflate64Error(Exception):
    "Call to the underlying library failed."
    pass


@ffi.def_extern()
def raw_alloc(size: int) -> object:
    if size == 0:
        return ffi.NULL
    block = ffi.new("char[]", size)
    _allocated.append(block)
    return block


@ffi.def_extern()
def raw_free(o: object) -> None:
    if o in _allocated:
        _allocated.remove(o)


class Deflatter:
    def __init__(self):
        self.lock = Lock()
        self.closed = False
        self.flushed = False

    def deflate(self, data) -> bytes:
        return b""

    def flush(self) -> bytes:
        return b""

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        if not self.flushed:
            self.flush()


class Inflater:
    def __init__(self):
        self.lock = Lock()
        self.closed = False

    def inflate(self, data) -> bytes:
        return b""

    def release(self):
        pass

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.release()