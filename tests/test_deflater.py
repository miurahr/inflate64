import os
import pathlib

import inflate64
import pytest

testdata_path = pathlib.Path(os.path.dirname(__file__)).joinpath("data")
data = "This file is located in the root.This file is located in a folder.".encode("utf-8")


def test_deflater():
    deflater = inflate64.Deflater()
    compressed = deflater.deflate(data)
    assert len(compressed) > 150
