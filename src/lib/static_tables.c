/*
   Build and output length and distance decoding tables for fixed code
   decoding.
 */
#include <stdio.h>

#include "static_tables.h"

void gen_static_table9(struct inflate_state FAR *state, code *fixed, code *distfix, code *lenfix) {
    unsigned sym, bits;
    static code *next;

    /* literal/length table */
    sym = 0;
    while (sym < 144) state->lens[sym++] = 8;
    while (sym < 256) state->lens[sym++] = 9;
    while (sym < 280) state->lens[sym++] = 7;
    while (sym < 288) state->lens[sym++] = 8;
    next = fixed;
    lenfix = next;
    bits = 9;
    inflate_table9(LENS, state->lens, 288, &(next), &(bits), state->work);

    /* distance table */
    sym = 0;
    while (sym < 32) state->lens[sym++] = 5;
    distfix = next;
    bits = 5;
    inflate_table9(DISTS, state->lens, 32, &(next), &(bits), state->work);
}

#ifdef GEN_TREES_H
/*
   Write out the inflate_tree.h.
   makefixed() writes those tables to stdout, which would be piped to inflate_tree.h.
 */
void make_inflate_tree() {
    unsigned sym, bits, low, size;
    static code *lenfix, *distfix;
    struct inflate_state state;
    static code fixed[544];

    gen_static_table9(&state, fixed, distfix, lenfix);

    FILE *header = fopen("inflate_tree.h", "w");

    /* write tables */
    fprintf(header, "    /* inflate_fixed9.h -- table for decoding deflate64 fixed codes\n");
    fprintf(header, "     * Generated automatically by makefixed9().\n");
    fprintf(header, "     */\n\n");
    fprintf(header, "    /* WARNING: this file should *not* be used by applications.\n");
    fprintf(header, "       It is part of the implementation of this library and is\n");
    fprintf(header, "       subject to change.\n");
    fprintf(header, "     */\n\n");
    size = 1U << 9;
    fprintf(header, "    static const code lenfix[%u] = {", size);
    low = 0;
    for (;;) {
        if ((low % 6) == 0) fprintf(header, "\n        ");
        fprintf(header, "{%u,%u,%d}", lenfix[low].op, lenfix[low].bits,
               lenfix[low].val);
        if (++low == size) break;
        fprintf(header, ",");
    }
    fprintf(header, "\n    };");
    size = 1U << 5;
    fprintf(header, "\n    static const code distfix[%u] = {", size);
    low = 0;
    for (;;) {
        if ((low % 5) == 0) fprintf(header, "\n        ");
        fprintf(header, "{%u,%u,%d}", distfix[low].op, distfix[low].bits,
               distfix[low].val);
        if (++low == size) break;
        fprintf(header, ",");
    }
    fprintf(header, "\n    };\n");
}


int main(int argc, char* argv[]) {
    make_inflate_tree();
    make_deflate_tree();
}
#endif /* GEN_TREES_H */