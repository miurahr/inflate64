import sys

import cffi  # type: ignore  # noqa


def is_64bit() -> bool:
    return sys.maxsize > 1 << 32


ffibuilder = cffi.FFI()

# ----------- python binding API ---------------------
ffibuilder.cdef(
    r"""
extern "Python" void *raw_alloc(size_t);
extern "Python" void raw_free(void *);
"""
)

source = r"""
#include <stdio.h>
#include <stdlib.h>
"""


def set_kwargs(**kwargs):
    ffibuilder.set_source(source=source, **kwargs)


if __name__ == "__main__":  # not when running with setuptools
    ffibuilder.compile(verbose=True)
