/* inflate64.h -- header for using inflate64 library functions
 * Copyright (C) 2003 Mark Adler
 * Copyright (C) 2022 Hiroshi Miura
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/*
 * This header file and associated patches provide a decoder for PKWare's
 * undocumented deflate64 compression method (method 9).
 * This code has not yet been tested on 16-bit architectures.
 * These functions are used identically, except that there is no windowBits parameter,
 * and a 64K window must be provided. Also if int's are 16 bits, then a zero for
 * the third parameter of the "out" function actually means 65536UL.
 */

#include "zlib.h"

#define WBITS64   16 /* 64K LZ77 window for deflate64 */
#define MEM_LEVEL64   8  /* maximum compression */

#ifdef __cplusplus
extern "C" {
#endif

ZEXTERN int ZEXPORT deflate9 OF((z_stream FAR *strm, int flush));
ZEXTERN int ZEXPORT deflate9End OF((z_stream FAR *strm));
ZEXTERN int ZEXPORT deflate9Init2_ OF((z_stream FAR *strm,
                                       int  level,
                                       int  memLevel,
                                       int  strategy));

#define deflate9Init2(strm, level) \
        deflate9Init2_((strm), (level), MEM_LEVEL64, Z_DEFAULT_STRATEGY)

ZEXTERN int ZEXPORT inflate9 OF((z_stream FAR *strm, int flush));
ZEXTERN int ZEXPORT inflate9End OF((z_stream FAR *strm));
ZEXTERN int ZEXPORT inflate9Init2_ OF((z_stream FAR *strm));

#define inflate9Init2(strm) \
        inflate9Init2_((strm))

#ifdef __cplusplus
}
#endif