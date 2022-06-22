import os
import pathlib
import subprocess

import inflate64
import pytest

testdata_path = pathlib.Path(os.path.dirname(__file__)).joinpath("data")
expect = "This file is located in the root.This file is located in a folder.".encode("utf-8")


def test_inflater_basic():
    with testdata_path.joinpath("data.bin").open("rb") as f:
        data = f.read()
        inflater = inflate64.Inflater()
        result = inflater.inflate(data)
        assert result == expect

@pytest.mark.parametrize("offset,length,fname", [
    (41, 3096, "test-file.1"),
    (42, 3227, "test-file.10"),
    (42, 3237, "test-file.11"),
    (42, 3249, "test-file.12"),
])
def test_inflater_from_zip(tmp_path, offset, length, fname):
    testdata = testdata_path.joinpath("test-file.zip")
    with testdata.open("rb") as f:
        _ = f.read(offset)
        data = f.read(length)
    cmd = ["unzip", str(testdata), fname, "-d", str(tmp_path)]
    subprocess.run(cmd)
    with tmp_path.joinpath(fname).open("rb") as r:
        expected = r.read()
    inflater = inflate64.Inflater()
    result = inflater.inflate(data)
    assert len(result) == len(expected)
    assert result == expected
