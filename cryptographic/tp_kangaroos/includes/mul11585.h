// multiplication in GF(2**115 - 85)
// PK - 2019/12

#ifndef __MUL11585_H
#define __MUL11585_H

#include <stdint.h>
#include <stdio.h>

typedef union
{
    unsigned __int128 s;
    uint64_t t[2];
} num128;

num128 mul11585(num128 a, num128 b);
void print_num128(num128 a);

#endif // __MUL11585_H
