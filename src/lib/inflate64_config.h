//
// Created by miurahr on 22/06/28.
//

#ifndef DEFLATE9_INFLATE64_CONFIG_H
#define DEFLATE9_INFLATE64_CONFIG_H

/* common constants */

#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
/* The three kinds of block type */

#define MIN_MATCH  3
#define BASE_MATCH 258
#define MAX_MATCH  65530  /* < 1<<WBITS64 - MINMATCH -1 */
/* The minimum and maximum match lengths */

#define MEM_LEVEL64   9  /* maximum compression */
#define WBITS64   16     /* 64K LZ77 window for deflate64 */

#define MAX_BITS 15
/* All codes must not exceed MAX_BITS bits */

#ifdef __STDC__
#ifndef STDC
#define STDC
#endif
#endif

/* Type declarations */

#ifndef OF /* function prototypes */
#  ifdef STDC
#    define OF(args)  args
#  else
#    define OF(args)  ()
#  endif
#endif

#ifndef ZEXTERN
#  define ZEXTERN extern
#endif
#ifndef ZEXPORT
#  define ZEXPORT
#endif
#ifndef ZEXPORTVA
#  define ZEXPORTVA
#endif

#ifndef FAR
#  define FAR
#endif

#define z_const const

typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;

#if !defined(__MACTYPES__)
typedef unsigned char  Byte;  /* 8 bits */
#endif
typedef unsigned int   uInt;  /* 16 bits or more */
typedef unsigned long  uLong; /* 32 bits or more */

#ifdef STDC
   typedef void const *voidpc;
   typedef void FAR   *voidpf;
   typedef void       *voidp;
#else
   typedef Byte const *voidpc;
   typedef Byte FAR   *voidpf;
   typedef Byte       *voidp;
#endif

#ifdef _WIN32
#include <stddef.h>         /* for wchar_t */
#endif

#ifndef z_off_t
#  define z_off_t long
#endif

/* MVS linker does not support external names larger than 8 bytes */
#if defined(__MVS__)
  #pragma map(deflate9Init_,"DEIN")
  #pragma map(deflate9Init2_,"DEIN2")
  #pragma map(deflate9End,"DEEND")
  #pragma map(inflate9Init_,"ININ")
  #pragma map(inflate9Init2_,"ININ2")
  #pragma map(inflate9End,"INEND")
#endif
typedef voidpf (*alloc_func) OF((voidpf opaque, uInt items, uInt size));
typedef void   (*free_func)  OF((voidpf opaque, voidpf address));

struct internal_state;

typedef struct z_stream_s {
    z_const Byte FAR *next_in;     /* next input byte */
    uInt     avail_in;  /* number of bytes available at next_in */
    uLong    total_in;  /* total number of input bytes read so far */

    Byte FAR    *next_out; /* next output byte will go here */
    uInt     avail_out; /* remaining free space at next_out */
    uLong    total_out; /* total number of bytes output so far */

    z_const char *msg;  /* last error message, NULL if no error */
    struct internal_state FAR *state; /* not visible by applications */

    alloc_func zalloc;  /* used to allocate the internal state */
    free_func  zfree;   /* used to free the internal state */
    voidpf     opaque;  /* private data object passed to zalloc and zfree */

    int     data_type;  /* best guess about the data type: binary or text
                           for deflate, or the decoding state for inflate */
    uLong   adler;      /* Adler-32 or CRC-32 value of the uncompressed data */
    uLong   reserved;   /* reserved for future use */
} z_stream;

typedef z_stream FAR *z_streamp;

                        /* constants */

#define Z_NO_FLUSH      0
#define Z_FINISH        4
/* Allowed flush values; see deflate() and inflate() below for details */

#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
                        /* constants */

#define Z_NO_FLUSH      0
#define Z_PARTIAL_FLUSH 1
#define Z_SYNC_FLUSH    2
#define Z_FULL_FLUSH    3
#define Z_FINISH        4
#define Z_BLOCK         5
#define Z_TREES         6
/* Allowed flush values; see deflate() and inflate() below for details */

#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)

#define Z_NULL         0

#define Z_FIXED               4
/* compression strategy */

#define Z_BINARY   0
#define Z_TEXT     1
#define Z_UNKNOWN  2
/* Possible values of the data_type field for deflate() */

#ifdef HAVE_HIDDEN
#  define ZLIB_INTERNAL __attribute__((visibility ("hidden")))
#else
#  define ZLIB_INTERNAL
#endif

#ifndef local
#  define local static
#endif
/* since "static" is used to mean two completely different things in C, we
   define "local" for the non-static meaning of "static", for readability
   (compile with -Dlocal if your debugger can't find static symbols) */

#if defined(STDC) && !defined(Z_SOLO)
#  if !defined(_MSC_VER)
#    include <stddef.h>
#  endif
#  include <string.h>
#  include <stdlib.h>
#endif

#endif //DEFLATE9_INFLATE64_CONFIG_H
