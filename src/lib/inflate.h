/* inflate.h -- internal inflate state definition
 * Copyright (C) 1995-2003 Mark Adler
 * Copyright (C) 2022 Hiroshi Miura
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change.
 */

/* Possible inflate modes between inflate() calls */
typedef enum {
    TYPE,       /* i: waiting for type bits, including last-flag bit */
    TYPED0,
    STORED,     /* i: waiting for stored size (length and complement) */
    COPY_,
    COPY,
    TABLE,      /* i: waiting for dynamic block table lengths */
    LENLENS,
    CODELENS,
        LEN_,
        LEN,        /* i: waiting for length/lit code */
        LENEXT,
        DIST,
        DISTEXT,
        MATCH,
        LIT,
    CHECK,
    DONE,       /* finished check, done -- remain here until reset */
    BAD,         /* got a data error -- remain here until reset */
    MEM
} inflate_mode;

/*
    State transitions between above modes -

    (most modes can go to the BAD mode -- not shown for clarity)

    Read deflate blocks:
            TYPE -> STORED or TABLE or LEN or DONE
            STORED -> TYPE
            TABLE -> LENLENS -> CODELENS -> LEN
    Read deflate codes:
                LEN -> LEN or TYPE
 */

/* state maintained between inflate() calls.  Approximately 7K bytes. */
struct inflate_state {
    z_streamp strm;
    inflate_mode mode;          /* current inflate mode */
    int last;                   /* true if processing last block */
    int wrap;                   /* true if the window has wrapped */

    unsigned long check;
    unsigned long total;
        /* sliding window */        /* sliding window */
    unsigned wbits;             /* log base 2 of requested window size */
    unsigned wsize;             /* window size or zero if not using window */
    unsigned whave;             /* valid bytes in the window */
    unsigned wnext;             /* window write index */
    unsigned char FAR *window;  /* allocated sliding window, if needed */
        /* bit accumulator */
    unsigned long hold;         /* input bit accumulator */
    unsigned bits;              /* number of bits in "in" */
        /* for string and stored block copying */
    unsigned length;            /* literal or length of data to copy */
    unsigned offset;            /* distance back to copy string from */
        /* for table and code decoding */
    unsigned extra;             /* extra bits needed */
    /* fixed and dynamic code tables */
    code const FAR *lencode;    /* starting table for length/literal codes */
    code const FAR *distcode;   /* starting table for distance codes */
    unsigned lenbits;           /* index bits for lencode */
    unsigned distbits;          /* index bits for distcode */

    /* dynamic table building */
    unsigned ncode;             /* number of code length code lengths */
    unsigned nlen;              /* number of length code lengths */
    unsigned ndist;             /* number of distance code lengths */
    unsigned have;              /* number of code lengths in lens[] */
    code FAR *next;             /* next available space in codes[] */
    unsigned short lens[320];   /* temporary storage for code lengths */
    unsigned short work[288];   /* work area for code table building */
    code codes[ENOUGH];         /* space for code tables */
    int sane;                   /* if false, allow invalid distance too far */
    int back;                   /* bits back of last unprocessed length/lit */
    unsigned was;               /* initial length of match */
};
