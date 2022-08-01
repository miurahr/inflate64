Enhanced Deflate note
=====================

Enhanced Deflate is a slightly modified variant of the Deflate procedure.
The fundamental mechanisms remained completely unchanged, only the following features were improved.

Dictionary size to 64 Kbytes
----------------------------

The addressable size of the sliding dictionary is extended from 32 kbytes to 64 kbytes.
Enhanced deflate detects matches from 65536 bytes distance and maximum 65530 bytes length.
A definition is placed in ``inflate64_config.h``.

Length code
------------

Last length code (285) will be extended by 16 extra bits.
It represent length 258 and extension bit width is 0 in deflate.
Otherwise it represent length 258-65536 and extension bit widths are 16 in enhanced deflate
The code defines lengths in a range between 3 and 65.538 byte.
The original limitation to 258 byte sequences is dropped with that.

.. list-table:: Length code table
   :widths: 40, 40, 25
   :header-rows: 1

   * - Length
     - Code
     - Ext bitwidth
   * - 3
     - 257
     - 0
   * - 4
     - 258
     - 0
   * - 5
     - 259
     - 0
   * - 6
     - 260
     - 0
   * - 7
     - 261
     - 0
   * - 8
     - 262
     - 0
   * - 9
     - 263
     - 0
   * - 10
     - 264
     - 0
   * - 11,12
     - 265
     - 1
   * - 13,14
     - 266
     - 1
   * - 15,16
     - 267
     - 1
   * - 17,18
     - 268
     - 1
   * - 19-22
     - 269
     - 2
   * - 23-26
     - 270
     - 2
   * - 27-30
     - 271
     - 2
   * - 31-34
     - 272
     - 2
   * - 35-42
     - 273
     - 3
   * - 43-50
     - 274
     - 3
   * - 51-58
     - 275
     - 3
   * - 59-66
     - 276
     - 3
   * - 67-82
     - 277
     - 4
   * - 83-98
     - 278
     - 4
   * - 99-114
     - 279
     - 4
   * - 115-130
     - 280
     - 4
   * - 131-162
     - 281
     - 5
   * - 163-194
     - 282
     - 5
   * - 195-226
     - 283
     - 5
   * - 227-258
     - 284
     - 5
   * - 259-65536
     - 285
     - 16



Distance code
-------------

The distance codes (30 and 31) not used until now are extended to address a range of 64 kbytes.
According to the conventional Deflate definition these codes were not used.
14 extra bits are assigned to each of them.
Details definition is described following.

.. list-table:: Distance code table
   :widths: 40, 40, 25
   :header-rows: 1

   * - Distance
     - Code
     - Ext bitwidth
   * - 1
     - 0
     - 0
   * - 2
     - 1
     - 0
   * - 3
     - 2
     - 0
   * - 4
     - 3
     - 0
   * - 5,6
     - 4
     - 1
   * - 7,8
     - 5
     - 1
   * - 9-12
     - 6
     - 2
   * - 13-16
     - 7
     - 2
   * - 17-24
     - 8
     - 3
   * - 25-32
     - 9
     - 3
   * - 33-48
     - 10
     - 4
   * - 49-64
     - 11
     - 4
   * - 65-96
     - 12
     - 5
   * - 97-128
     - 13
     - 5
   * - 129-192
     - 14
     - 6
   * - 193-256
     - 15
     - 6
   * - 257-384
     - 16
     - 7
   * - 385-512
     - 17
     - 7
   * - 513-768
     - 18
     - 8
   * - 769-1024
     - 19
     - 8
   * - 1025-1536
     - 20
     - 9
   * - 1537-2048
     - 21
     - 9
   * - 2049-3072
     - 22
     - 10
   * - 3073-4096
     - 23
     - 10
   * - 4097-6144
     - 24
     - 11
   * - 6145-8192
     - 25
     - 11
   * - 8193-12288
     - 26
     - 12
   * - 12289-16384
     - 27
     - 12
   * - 16385-24576
     - 28
     - 13
   * - 24577-32768
     - 29
     - 13
   * - 32769-49152
     - 30
     - 14
   * - 49153-65536
     - 31
     - 14




