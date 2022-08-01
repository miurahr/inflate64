/* deflate9.h -- internal compression state for deflate.c
 * Copyright (C) 1995-2018 Jean-loup Gailly
 * Copyright (C) 2022 Hiroshi Miura
 */

#ifndef DEFLATE9_H
#define DEFLATE9_H

#include "util.h"

/* ===========================================================================
 * Internal compression state.
 */

#define LENGTH_CODES 30
/* number of length codes, not counting the special END_BLOCK code */
/* extended (+1) for deflate64 from 29 */

#define LITERALS  256
/* number of literal bytes 0..255 */

#define L_CODES (LITERALS+1+LENGTH_CODES)
/* number of Literal or Length codes, including the END_BLOCK code */

#define D_CODES   32
/* number of distance codes */
/* extended +2 for deflate64 from 30 of deflate */

#define BL_CODES  19
/* number of codes used to transfer the bit lengths */

#define HEAP_SIZE (2*L_CODES+1)
/* maximum heap size */

#define Buf_size 16
/* size of bit buffer in bi_buf */

#define INIT_STATE    42    /* deflate -> BUSY_STATE */
#define BUSY_STATE   113    /* finish deflate -> FINISH_STATE */
#define FINISH_STATE 666    /* stream complete */
/* Stream status */


/* Data structure describing a single value and its code string. */
typedef struct ct_data_s {
    union {
        unsigned short  freq;       /* frequency count */
        unsigned short  code;       /* bit string */
    } fc;
    union {
        unsigned short  dad;        /* father node in Huffman tree */
        unsigned short  len;        /* length of bit string */
    } dl;
} FAR ct_data;

#define Freq fc.freq
#define Code fc.code
#define Dad  dl.dad
#define Len  dl.len

typedef struct static_tree_desc_s  static_tree_desc;

typedef struct tree_desc_s {
    ct_data *dyn_tree;           /* the dynamic tree */
    int     max_code;            /* largest code with non zero frequency */
    const static_tree_desc *stat_desc;  /* the corresponding static tree */
} FAR tree_desc;

typedef unsigned short Pos;
typedef Pos FAR Posf;
typedef unsigned IPos;

/* A Pos is an index in the character window. We use short instead of int to
 * save space in the various tables. IPos is used only for parameter passing.
 */

typedef struct internal_state {
    z_streamp strm;      /* pointer back to this zlib stream */
    int   status;        /* as the name implies */
    Byte FAR *pending_buf;  /* output still pending */
    unsigned long   pending_buf_size; /* size of pending_buf */
    Byte FAR *pending_out;  /* next pending byte to output to the stream */
    unsigned long   pending;       /* nb of bytes in the pending buffer */
    int   last_flush;    /* value of flush param for previous deflate call */

                /* used by deflate.c: */

    unsigned long  w_size;        /* LZ77 window size (64K for deflate64) */
    uInt w_bits;                  /* 8..16 */
    unsigned long  w_mask;        /* w_size - 1 */

    Byte FAR *window;
    /* Sliding window. Input bytes are read into the second half of the window,
     * and move to the first half later to keep a dictionary of at least wSize
     * bytes. With this organization, matches are limited to a distance of
     * wSize-MAX_MATCH bytes, but this ensures that IO is always
     * performed with a length multiple of the block size. Also, it limits
     * the window size to 64K, which is quite useful on MSDOS.
     * To do: use the user input buffer as sliding window.
     */

    unsigned long window_size;
    /* Actual size of window: 2*wSize, except when the user input buffer
     * is directly used as sliding window.
     */

    Posf *prev;
    /* Link to older string with same hash index. To limit the size of this
     * array to 64K, this link is maintained only for the last 32K strings.
     * An index in this array is thus a window index modulo 32K.
     */

    Posf *head; /* Heads of the hash chains or NIL. */

    uInt  ins_h;          /* hash index of string to be inserted */
    uInt  hash_size;      /* number of elements in hash table */
    uInt  hash_bits;      /* log2(hash_size) */
    uInt  hash_mask;      /* hash_size-1 */

    uInt  hash_shift;
    /* Number of bits by which ins_h must be shifted at each input
     * step. It must be such that after MIN_MATCH steps, the oldest
     * byte no longer takes part in the hash key, that is:
     *   hash_shift * MIN_MATCH >= hash_bits
     */

    long block_start;
    /* Window position at the beginning of the current output block. Gets
     * negative when the window is moved backwards.
     */

    uInt match_length;           /* length of best match */
    IPos prev_match;             /* previous match */
    int match_available;         /* set if previous match exists */
    uInt strstart;               /* start of string to insert */
    uInt match_start;            /* start of matching string */
    uInt lookahead;              /* number of valid bytes ahead in window */

    uInt prev_length;
    /* Length of the best match at previous step. Matches not greater than this
     * are discarded. This is used in the lazy match evaluation.
     */

    uInt max_chain_length;
    /* To speed up deflation, hash chains are never searched beyond this
     * length.  A higher limit improves compression ratio but degrades the
     * speed.
     */

    uInt max_lazy_match;
    /* Attempt to find a better match only when the current match is strictly
     * smaller than this value. This mechanism is used only for compression
     * levels >= 4.
     */
#   define max_insert_length  max_lazy_match
    /* Insert new strings in the hash table only if the match length is not
     * greater than this length. This saves time but degrades compression.
     * max_insert_length is used only for compression levels <= 3.
     */

    int level;    /* compression level (1..9) */
    int strategy; /* favor or force Huffman coding*/

    uInt good_match;
    /* Use a faster search when the previous match is longer than this */

    int nice_match; /* Stop searching when current match exceeds this */

                /* used by trees.c: */
    /* Didn't use ct_data typedef below to suppress compiler warning */
    struct ct_data_s dyn_ltree[HEAP_SIZE];   /* literal and length tree */
    struct ct_data_s dyn_dtree[2*D_CODES+1]; /* distance tree */
    struct ct_data_s bl_tree[2*BL_CODES+1];  /* Huffman tree for bit lengths */

    struct tree_desc_s l_desc;               /* desc. for literal tree */
    struct tree_desc_s d_desc;               /* desc. for distance tree */
    struct tree_desc_s bl_desc;              /* desc. for bit length tree */

    unsigned short bl_count[MAX_BITS+1];
    /* number of codes at each bit length for an optimal tree */

    int heap[2*L_CODES+1];      /* heap used to build the Huffman trees */
    int heap_len;               /* number of elements in the heap */
    int heap_max;               /* element of largest frequency */
    /* The sons of heap[n] are heap[2*n] and heap[2*n+1]. heap[0] is not used.
     * The same heap array is used to build all trees.
     */

    unsigned char depth[2*L_CODES+1];
    /* Depth of each subtree used as tie breaker for trees of equal frequency
     */

    unsigned char FAR *sym_buf;        /* buffer for distances and literals/lengths */

    uInt  lit_bufsize;
    /* Size of match buffer for literals/lengths.  There are 4 reasons for
     * limiting lit_bufsize to 64K:
     *   - frequencies can be kept in 16 bit counters
     *   - if compression is not successful for the first block, all input
     *     data is still in the window so we can still emit a stored block even
     *     when input comes from standard input.  (This can also be done for
     *     all blocks if lit_bufsize is not greater than 32K.)
     *   - if compression is not successful for a file smaller than 64K, we can
     *     even emit a stored file instead of a stored block (saving 5 bytes).
     *     This is applicable only for zip (not gzip or zlib).
     *   - creating new Huffman trees less frequently may not provide fast
     *     adaptation to changes in the input data statistics. (Take for
     *     example a binary file with poorly compressible code followed by
     *     a highly compressible string table.) Smaller buffer sizes give
     *     fast adaptation but have of course the overhead of transmitting
     *     trees more frequently.
     *   - I can't count above 4
     */

    uInt sym_next;      /* running index in sym_buf */
    uInt sym_end;       /* symbol table full when sym_next reaches this */

    unsigned long opt_len;        /* bit length of current block with optimal trees */
    unsigned long static_len;     /* bit length of current block with static trees */
    uInt matches;       /* number of string matches in current block */
    uInt insert;        /* bytes at end of window left to insert */

#ifdef ZLIB_DEBUG
    ulg compressed_len; /* total bit length of compressed file mod 2^32 */
    ulg bits_sent;      /* bit length of compressed data sent mod 2^32 */
#endif

    unsigned short bi_buf;
    /* Output buffer. bits are inserted starting at the bottom (least
     * significant bits).
     */
    int bi_valid;
    /* Number of valid bits in bi_buf.  All bits above the last valid bit
     * are always zero.
     */

    unsigned long high_water;
    /* High water mark offset in window for initialized bytes -- bytes above
     * this are set to zero in order to avoid memory check warnings when
     * longest match routines access bytes past the input.  This is then
     * updated to the new high water mark.
     */

} FAR deflate_state;

/* Output a byte on the stream.
 * IN assertion: there is enough room in pending_buf.
 */
#define put_byte(s, c) {s->pending_buf[s->pending++] = (Byte FAR)(c);}


#define MIN_LOOKAHEAD (MAX_MATCH+MIN_MATCH+1)
/* Minimum amount of lookahead, except at the end of the input file.
 * See deflate.c for comments about the MIN_MATCH+1.
 */

#define MAX_DIST(s)  ((s)->w_size-MIN_LOOKAHEAD)
/* In order to simplify the code match distances are limited to MAX_DIST instead of WSIZE. */

#define WIN_INIT MAX_MATCH
/* Number of bytes after end of data in window to initialize in order to avoid
   memory checker errors from longest match routines */

        /* in trees.c */
void ZLIB_INTERNAL _tr_init OF((deflate_state *s));
int ZLIB_INTERNAL _tr_tally OF((deflate_state *s, unsigned dist, unsigned lc));
void ZLIB_INTERNAL _tr_flush_block OF((deflate_state *s, char FAR *buf,
                        ulg stored_len, int last));
void ZLIB_INTERNAL _tr_flush_bits OF((deflate_state *s));
void ZLIB_INTERNAL _tr_stored_block OF((deflate_state *s, char FAR *buf,
                        ulg stored_len, int last));

#define d_code(dist) \
   ((dist) < 256 ? _dist_code[dist] : _dist_code[256+((dist)>>7)])
/* Mapping from a distance to a distance code. dist is the distance - 1 and
 * must not have side effects. _dist_code[256] and _dist_code[257] are never
 * used.
 */

#define l_code(length) \
   ((length) <= BASE_MATCH ? length_code[length] : 28)
/* last length code is common (28) for 259-65536 */

#ifndef ZLIB_DEBUG
#if defined(GEN_TREES_H) || !defined(STDC)
  extern uch ZLIB_INTERNAL _dist_code[];
#else
  extern const unsigned char ZLIB_INTERNAL _dist_code[];
#endif
#endif
extern uch ZLIB_INTERNAL length_code[];

#endif /* DEFLATE_H */
