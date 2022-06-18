from ._inflate64 import Deflater, Inflater

__all__ = (
    "Deflater",
    "Inflater",
    "Inflate64Error",
)


class Inflate64Error(Exception):
    "Call to the underlying library failed."
    pass
