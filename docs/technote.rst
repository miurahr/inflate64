Enhanced Deflate note
=====================

Length code
-----------

Length code 285 is different from deflate.
It represent length 258 and extension bit width is 0 in deflate.
Otherwise it represent length 258-65536 and extension bit widths are 16 in enhanced deflate

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

Distance code table is extended to length 32.
Enhanced deflate give code 30, 31 to have extension bit widths 14.
That express distance from 32kB to 64kB.

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




