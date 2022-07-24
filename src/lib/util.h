/* zutil.h -- internal interface and configuration of the compression library
 * Copyright (C) 1995-2022 Jean-loup Gailly, Mark Adler
 */

#ifndef ZUTIL_H
#define ZUTIL_H

#include "inflate64_config.h"

         /* functions */

extern z_const char * const z_errmsg[10]; /* indexed by 2-zlib_error */
/* (size given to avoid silly warnings with Visual C++) */

#define ERR_MSG(err) z_errmsg[Z_NEED_DICT-(err)]

#define ERR_RETURN(strm,err) \
  return (strm->msg = ERR_MSG(err), (err))
/* To be used only when the state is known to be valid */

#if defined(pyr) || defined(Z_SOLO)
#  define NO_MEMCPY
#endif
#if defined(STDC) && !defined(HAVE_MEMCPY) && !defined(NO_MEMCPY)
#  define HAVE_MEMCPY
#endif

#ifdef HAVE_MEMCPY
#    define zmemcpy memcpy
#    define zmemcmp memcmp
#    define zmemzero(dest, len) memset(dest, 0, len)
#else
   void ZLIB_INTERNAL zmemcpy OF((Byte FAR* dest, const Byte FAR* source, uInt len));
   int ZLIB_INTERNAL zmemcmp OF((const Byte FAR* s1, const Byte FAR* s2, uInt len));
   void ZLIB_INTERNAL zmemzero OF((Byte FAR* dest, uInt len));
#endif

/* Diagnostic functions */
#ifdef ZLIB_DEBUG
#  include <stdio.h>
   extern int ZLIB_INTERNAL z_verbose;
   extern void ZLIB_INTERNAL z_error OF((char *m));
#  define Assert(cond,msg) {if(!(cond)) z_error(msg);}
#  define Tracev(x) {if (z_verbose>0) fprintf x ;}
#  define Tracevv(x) {if (z_verbose>1) fprintf x ;}
#  define Tracecv(c,x) {if (z_verbose>1 && (c)) fprintf x ;}
#else
#  define Assert(cond,msg)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracecv(c,x)
#endif

#define ZALLOC(strm, items, size) \
           (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (voidpf)(addr))
#define TRY_FREE(s, p) {if (p) ZFREE(s, p);}

#endif /* ZUTIL_H */
