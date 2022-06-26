import os
import pathlib
import zipfile

import pytest

import inflate64

BLOCKSIZE = 8192
testdata_path = pathlib.Path(os.path.dirname(__file__)).joinpath("data")
srcdata = testdata_path.joinpath("src.zip")


@pytest.mark.parametrize( "fname,minsize,maxsize", [
    ("test-file.1", 3000, 3600),
    ("test-file.2", 3100, 3700),
])
def test_compress(fname, minsize, maxsize):
    with zipfile.ZipFile(srcdata) as f:
        data = f.read(fname)
    compressor = inflate64.Deflater()
    compressed = compressor.deflate(data)
    compressed += compressor.flush()
    assert len(compressed) > minsize
    assert len(compressed) < maxsize
