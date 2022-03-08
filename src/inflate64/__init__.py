from typing import Union

try:
    from importlib.metadata import PackageNotFoundError, version
except ImportError:
    from importlib_metadata import PackageNotFoundError, version  # type: ignore  # noqa

try:
    from .c.c_deflate64 import (  # noqa
        Deflate64Error,
    )
except ImportError:
    try:
        from .cffi.cffi_deflate64 import (  # noqa
            Deflate64Error,
        )
    except ImportError:
        msg = "pyppmd module: Neither C implementation nor CFFI " "implementation can be imported."
        raise ImportError(msg)

__all__ = ()

__doc__ = """\
Python bindings to Deflate64 compression library, the API is similar to
Python's bz2/lzma/zlib module.

Documentation: https://pyppmd.readthedocs.io
Github: https://github.com/miurahr/pyppmd
PyPI: https://pypi.org/project/pyppmd"""

__copyright__ = "Copyright (C) 2022 Hiroshi Miura"

try:
    __version__ = version(__name__)
except PackageNotFoundError:  # pragma: no-cover
    # package is not installed
    __version__ = "unknown"

