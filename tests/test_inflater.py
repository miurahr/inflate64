import hashlib
import os
import pathlib
import zipfile

import pytest

import inflate64

testdata_path = pathlib.Path(os.path.dirname(__file__)).joinpath("data")
testdata = testdata_path.joinpath("test-file.zip")
largedata = testdata_path.joinpath("large-file.zip")
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
def test_decompress(fname, offset, length):
    """
    Test with various size of data.
    :param fname:  file name of test file.
    :param offset:  data offset in target zip file.
    :param length:  compressed size of data.
    """
    with testdata.open("rb") as f:
        _ = f.seek(offset, os.SEEK_SET)
        data = f.read(length)
    with zipfile.ZipFile(srcdata) as z:
        expected = z.read(fname)
    decompressor = inflate64.Inflater()
    result = decompressor.inflate(data)
    assert len(result) == len(expected)
    assert result == expected


def test_decompress_larger():
    """
    Test with larger file with looping inflate() method.
    """
    fname = "10000SalesRecords.csv"
    offset = 51
    remaining = 351298
    BS = 8192
    #
    m = hashlib.sha256()
    with zipfile.ZipFile(srcdata) as z:
        m.update(z.read(fname))
    expected = m.digest()
    #
    m = hashlib.sha256()
    decompressor = inflate64.Inflater()
    assert not decompressor.eof
    with largedata.open("rb") as f:
        _ = f.seek(offset, os.SEEK_SET)
        while remaining > 0:
            length = min(BS, remaining)
            data = f.read(length)
            m.update(decompressor.inflate(data))
            remaining -= len(data)
    assert decompressor.eof
    assert m.digest() == expected
