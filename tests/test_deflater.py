import os
import pathlib
import zipfile

import pytest

import inflate64

testdata_path = pathlib.Path(os.path.dirname(__file__)).joinpath("data")
srcdata = testdata_path.joinpath("src.zip")


@pytest.mark.parametrize("fname",
                         ["test-file.1",
                          "test-file.2",
                          "test-file.3",
                          "test-file.4",
                          "test-file.5",
                          "test-file.6",
                          "test-file.7",
                          "test-file.8",
                          "test-file.9",
                          "test-file.10",
                          "test-file.11",
                          "test-file.12",
                          "test-file.13",
                          "test-file.14",
                          "test-file.15",
                          "test-file.16",
                          "test-file.17",
                          "test-file.18",
                          "test-file.19",
                          "test-file.20",
                          ])
def test_compress_n(tmp_path, fname):
    """
    Compress test with various data.
    :param tmp_path:  fixture.
    :param fname:  target file name.
    """
    with zipfile.ZipFile(srcdata) as f:
        data = f.read(fname)
    expected_len = len(data)
    compressor = inflate64.Deflater()
    compressed = compressor.deflate(data)
    compressed += compressor.flush()
    with tmp_path.joinpath(fname).open("wb") as f:
        f.write(compressed)
    decompressor = inflate64.Inflater()
    extracted = decompressor.inflate(compressed)
    result_len = len(extracted)
    assert result_len == expected_len
    assert extracted == data


def test_compress_larger(tmp_path):
    """
    Compression test with larger size of data.
    :param tmp_path:  fixture.
    """
    fname = "10000SalesRecords.csv"
    BS = 8192
    #
    compressor = inflate64.Deflater()
    with tmp_path.joinpath(fname).open(mode="wb") as target:
        with zipfile.ZipFile(srcdata) as myzip:
            with myzip.open(fname) as myfile:
                data = myfile.read(BS)
                while len(data) > 0:
                    target.write(compressor.deflate(data))
                    data = myfile.read(BS)
                target.write(compressor.flush())

