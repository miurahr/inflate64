import os
import pathlib

import inflate64

BLOCKSIZE = 8192
testdata_path = pathlib.Path(os.path.dirname(__file__)).joinpath("data")
testdata = testdata_path.joinpath("test-file.bin")

def test_deflater():
    deflater = inflate64.Deflater()
    with testdata.open("rb") as f:
        data = f.read(BLOCKSIZE)
        while len(data) > 0:
            compressed = deflater.deflate(data)
            data = f.read(BLOCKSIZE)
        compressed += deflater.flush()
        assert len(compressed) > 3000
        assert len(compressed) < 4000
