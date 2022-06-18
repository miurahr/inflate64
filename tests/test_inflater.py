import os
import pathlib

import inflate64

testdata_path = pathlib.Path(os.path.dirname(__file__)).joinpath("data")
expect = "This file is located in the root.This file is located in a folder.".encode("utf-8")


def test_inflater_basic():
    with testdata_path.joinpath("data.bin").open("rb") as f:
        data = f.read()
        inflater = inflate64.Inflater()
        result = inflater.inflate(data)
        assert result == expect
