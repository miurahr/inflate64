/*
   Build and output length and distance decoding tables for fixed code
   decoding.
 */
#include <stdio.h>
#include "inflate64.h"

#include "inflate_tree.h"
#include "inflate.h"

/*
   Write out the inflate_tree.h.
   makefixed() writes those tables to stdout, which would be piped to inflate_tree.h.
 */
void makefixed9() {
    unsigned sym, bits, low, size;
    code *next, *lenfix, *distfix;
    struct inflate_state state;
    code fixed[544];

    /* literal/length table */
    sym = 0;
    while (sym < 144) state.lens[sym++] = 8;
    while (sym < 256) state.lens[sym++] = 9;
    while (sym < 280) state.lens[sym++] = 7;
    while (sym < 288) state.lens[sym++] = 8;
    next = fixed;
    lenfix = next;
    bits = 9;
    inflate_table9(LENS, state.lens, 288, &(next), &(bits), state.work);

    /* distance table */
    sym = 0;
    while (sym < 32) state.lens[sym++] = 5;
    distfix = next;
    bits = 5;
    inflate_table9(DISTS, state.lens, 32, &(next), &(bits), state.work);

    /* write tables */
    puts("    /* inffix9.h -- table for decoding deflate64 fixed codes");
    puts("     * Generated automatically by makefixed9().");
    puts("     */");
    puts("");
    puts("    /* WARNING: this file should *not* be used by applications.");
    puts("       It is part of the implementation of this library and is");
    puts("       subject to change.");
    puts("     */");
    puts("");
    size = 1U << 9;
    printf("    static const code lenfix[%u] = {", size);
    low = 0;
    for (;;) {
        if ((low % 6) == 0) printf("\n        ");
        printf("{%u,%u,%d}", lenfix[low].op, lenfix[low].bits,
               lenfix[low].val);
        if (++low == size) break;
        putchar(',');
    }
    puts("\n    };");
    size = 1U << 5;
    printf("\n    static const code distfix[%u] = {", size);
    low = 0;
    for (;;) {
        if ((low % 5) == 0) printf("\n        ");
        printf("{%u,%u,%d}", distfix[low].op, distfix[low].bits,
               distfix[low].val);
        if (++low == size) break;
        putchar(',');
    }
    puts("\n    };");
}


int main(int argc, char* argv[]) {
    makefixed9();
}