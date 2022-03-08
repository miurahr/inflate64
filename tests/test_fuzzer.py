import sys
from datetime import timedelta

from hypothesis import given, settings
from hypothesis import strategies as st

import deflate64

MAX_SIZE = 1 << 30


@given(
    obj=st.binary(min_size=1),
    max_order=st.integers(min_value=2, max_value=64),
    mem_size=st.integers(min_value=1 << 11, max_value=MAX_SIZE),
)
@settings(deadline=timedelta(milliseconds=300))
def test_deflate64_fuzzer(obj, max_order, mem_size):
    enc = deflate64.Deflate64Encoder()
    length = len(obj)
    compressed = enc.encode(obj)
    compressed += enc.flush()
    dec = deflate64.Deflate64Decoder()
    result = dec.decode(compressed, length)
    result += dec.flush(length - len(result))
    assert result == obj


