Getting started with Inflate64
==============================

The ``inflate64`` is a python package to provide ``Deflater`` and ``Inflater`` class to compress and
decompress with Enhanced Deflate compression algorithm.

The project is in ``Production/Stable`` status.

How to use
----------

You can install it with ``pip`` command as usual.

.. code-block::

  pip install inflate64


You can extract compressed data by instantiating ``Inflater`` class and call ``inflate`` method.

.. code-block:: python

  import inflate64
  decompressor = inflate64.Inflater()
  extracted = decompressor.inflate(data)


You can also compress data by instantiating ``Deflater`` class and call ``deflate`` method.

.. code-block:: python

  import inflate64
  compressor = inflate64.Deflater()
  compressed = compressor.deflate(data)
  compressed += compressor.flush()

