import pytest
from hypothesis import given, settings
from hypothesis import strategies as st

import inflate64


@given(obj=st.binary(min_size=100))
@pytest.mark.skip(reason="too early dev stage for fuzzer test")
def test_inflate64_fuzzer(obj):
    deflater = inflate64.Deflater(level=6)
    length = len(obj)
    compressed = deflater.deflate(obj)
    inflater = inflate64.Inflater()
    result = inflater.inflate(compressed, length)
    assert result == obj
