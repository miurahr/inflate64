//
// Created by miurahr on 22/08/02.
//

#ifndef INFLATE64_MAKEFIXED_H
#define INFLATE64_MAKEFIXED_H

#include "inflate64.h"
#include "inflate_tree.h"
#include "inflate.h"

void gen_static_table9(struct inflate_state FAR *state, code *fixed, code *distfix, code *lenfix);
extern void make_deflate_tree();

#endif //INFLATE64_MAKEFIXED_H
