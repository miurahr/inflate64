import os
import pathlib
import zipfile

import pytest

import inflate64

testdata_path = pathlib.Path(os.path.dirname(__file__)).joinpath("data")
testdata = testdata_path.joinpath("test-file.zip")
srcdata = testdata_path.joinpath("src.zip")


@pytest.mark.parametrize(
    "fname,offset,length",
    [
        ("test-file.1", 41, 3096),
        ("test-file.2", 36434, 3112),
        ("test-file.3", 42984, 3125),
        ("test-file.4", 46150, 3143),
        ("test-file.5", 49334, 3156),
        ("test-file.6", 52531, 3169),
        ("test-file.7", 55741, 3186),
        ("test-file.8", 58968, 3198),
        ("test-file.9", 62207, 3210),
        ("test-file.10", 3179, 3227),
        ("test-file.11", 6448, 3237),
        ("test-file.12", 9727, 3249),
        ("test-file.13", 13018, 3266),
        ("test-file.14", 16326, 3277),
        ("test-file.15", 19645, 3289),
        ("test-file.16", 22976, 3304),
        ("test-file.17", 26322, 3316),
        ("test-file.18", 29680, 3328),
        ("test-file.19", 33050, 3343),
        ("test-file.20", 39588, 3355),
    ],
)
def test_decompress(tmp_path, fname, offset, length):
    with testdata.open("rb") as f:
        _ = f.seek(offset, os.SEEK_SET)
        data = f.read(length)
    with zipfile.ZipFile(srcdata) as z:
        expected = z.read(fname)
    decompressor = inflate64.Inflater()
    result = decompressor.inflate(data)
    assert len(result) == len(expected)
    assert result == expected
