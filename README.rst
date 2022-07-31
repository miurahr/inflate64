inflate64
=========

.. image:: https://dev.azure.com/miurahr/CodeBerg/_apis/build/status/CodeBerg-inflate64-CI?branchName=main
   :alt: Build status

.. image:: https://badge.fury.io/py/inflate64.svg
    :target: https://badge.fury.io/py/inflate64

The inflate64 is a python package to provide an ``Inflater`` class to decompress with Enhanced Deflate algorithm.

The project status is in ``Alpha`` stage.

API
---

You can use inflate64 by instantiating ``Inflater`` class and call ``inflate`` method to extract compressed data.

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


License
-------

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

.. note::
   Please note that Enhanced Deflate algorithm is also known as `DEFLATE64` :sup:`TM`
   that is a registered trademark of `PKWARE, Inc.`
