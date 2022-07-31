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

#include "inflate64_config.h"

#ifdef __cplusplus
extern "C" {
#endif


ZEXTERN int ZEXPORT deflate9Reset OF((z_stream FAR *strm));
ZEXTERN int ZEXPORT deflate9ResetKeep OF((z_stream FAR *strm));

ZEXTERN int ZEXPORT deflate9 OF((z_stream FAR *strm, int flush));
ZEXTERN int ZEXPORT deflate9End OF((z_stream FAR *strm));
ZEXTERN int ZEXPORT deflate9Init2 OF((z_stream FAR *strm));

ZEXTERN int ZEXPORT inflate9 OF((z_stream FAR *strm, int flush));
ZEXTERN int ZEXPORT inflate9End OF((z_stream FAR *strm));
ZEXTERN int ZEXPORT inflate9Init2 OF((z_stream FAR *strm));

#ifdef __cplusplus
}
#endif
