import sys
from datetime import timedelta

from hypothesis import given, settings
from hypothesis import strategies as st

import inflate64


@given(
    obj=st.binary(min_size=1),
)
def test_inflate64_fuzzer(obj):
    deflater = inflate64.Deflater()
    length = len(obj)
    compressed = deflater.deflate(obj)
    compressed += deflater.flush()
    inflater = inflate64.Inflater()
    result = inflater.inflate(compressed)
    assert result == obj


