from ._inflate64 import (
   Inflate64Error,
)

__all__ = (
    "Inflate64Error",
)


class Inflate64Error(Exception):
    "Call to the underlying library failed."
    pass
