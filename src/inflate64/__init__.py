from importlib.metadata import PackageNotFoundError, version

# try:
from .c.c_inflate64 import (  # noqa
    Deflater,
    Inflater,
    Inflate64Error,
)

# except ImportError:
#    try:
#        from .cffi.cffi_inflate64 import (  # noqa
#            Deflater,
#            Inflater,
#            Inflate64Error,
#        )
#    except ImportError:
#        msg = "inflate64 module: Neither C implementation nor CFFI implementation can be imported."
#        raise ImportError(msg)

__all__ = ()

__doc__ = """\
Python library to inflate data, the API is similar to Python's bz2/lzma/zlib module.
"""

__copyright__ = "Copyright (C) 2022 Hiroshi Miura"

try:
    __version__ = version(__name__)
except PackageNotFoundError:  # pragma: no-cover
    # package is not installed
    __version__ = "unknown"
