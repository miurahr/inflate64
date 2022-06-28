import os
import pathlib
import zipfile

import pytest

import inflate64
from inflate64 import dev

testdata_path = pathlib.Path(os.path.dirname(__file__)).joinpath("data")
srcdata = testdata_path.joinpath("src.zip")


@pytest.mark.skip(reason="Implementation is not completed.")
@pytest.mark.parametrize("fname,minsize,maxsize", [("test-file.1", 3000, 3600), ("test-file.2", 3100, 3700)])
def test_compress(fname, minsize, maxsize):
    with zipfile.ZipFile(srcdata) as f:
        data = f.read(fname)
    compressor = dev.Deflater()
    compressed = compressor.deflate(data)
    compressed += compressor.flush()
    #
    decompressor = inflate64.Inflater()
    extracted = decompressor.inflate(compressed)
    assert len(extracted) == len(data)
