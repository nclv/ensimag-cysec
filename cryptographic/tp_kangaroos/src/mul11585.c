#include "mul11585.h"

static const num128 mod  = {.t = {18446744073709551531ULL, 2251799813685247ULL}};
static const num128 m115 = {.t = {18446744073709551615ULL, 2251799813685247ULL}};

// inputs must be reduced, actually with some margin (about 6 bits)
num128 mul11585(num128 a, num128 b)
{
    num128 a0 = {.t = {0,0}};
    num128 a1 = {.t = {0,0}};
    num128 b0 = {.t = {0,0}};
    num128 b1 = {.t = {0,0}};

    unsigned __int128 a1b1, a0b1, a1b0, a0b0;

    num128 mid;
    num128 mid_q = {.t = {0,0}};
    num128 mid_r = {.t = {0,0}};

    num128 res;

    a0.t[0] = a.t[0];
    a1.t[0] = a.t[1];

    b0.t[0] = b.t[0];
    b1.t[0] = b.t[1];

    a1b1 = a1.s * b1.s * 696320;
    a0b1 = a0.s * b1.s;
    a1b0 = a1.s * b0.s;
    a0b0 = a0.s * b0.s;

    a0b0 = ((a0b0 >> 115) * 85) + (a0b0 & m115.s);
    mid.s = a0b1 + a1b0;

    mid_q.t[0] = mid.t[1];
    mid_q.s   *= 696320;

    mid_r.t[1] = mid.t[0];
    mid_r.s    = ((mid_r.s >> 115) * 85) + (mid_r.s & m115.s);

    res.s = a1b1 + a0b0 + mid_q.s + mid_r.s;
    res.s = (res.s >> 115) * 85 + (res.s & m115.s);

    res.s = res.s > mod.s ? res.s - mod.s : res.s;

    return res;
}

void print_num128(num128 a)
{
    printf("%lX%016lX", a.t[1], a.t[0]);
}