#include "kat.h"
#include "endian.h"
#include "permutations.h"

#define RATE (64 / 8)
#define ASCON_HASH_PA_ROUNDS 12
#define ASCON_HASH_PB_ROUNDS 12
#define ASCON_HASH_BYTES 32

#define ASCON_XOF_IV                          \
  (((u64)(RATE * 8) << 48) |  \
   ((u64)(ASCON_HASH_PA_ROUNDS) << 40) | \
   ((u64)(ASCON_HASH_PA_ROUNDS - ASCON_HASH_PB_ROUNDS) << 32))

#define ASCON_HASH_IV                                                \
  (((u64)(RATE * 8) << 48) |                         \
   ((u64)(ASCON_HASH_PA_ROUNDS) << 40) |                        \
   ((u64)(ASCON_HASH_PA_ROUNDS - ASCON_HASH_PB_ROUNDS) << 32) | \
   ((u64)(ASCON_HASH_BYTES * 8) << 0))


int ascon_hash(const unsigned char *m, unsigned long long mlen, unsigned char *c, unsigned long long outlen, int xof) {
    state s;

    ascon_hash_absorb(&s, m, mlen, xof);
    ascon_hash_squeeze(s, c, outlen, xof);

    return 0;
}

void ascon_hash_absorb(state *s_ptr, const unsigned char *m, unsigned long long len, int xof) {
    state s = *s_ptr;

    s.x0 = xof ? ASCON_XOF_IV : ASCON_HASH_IV ;
    s.x1 = 0;
    s.x2 = 0;
    s.x3 = 0;
    s.x4 = 0;       

    P12();

    while (len >= RATE) {
        s.x0 ^= U64BIG(*(u64 *)m);
        P12();
        len -= RATE;
        m += RATE;
    }
    for (int i = 0; i < len; ++i, ++m) {
        s.x0 ^= INS_BYTE64(*m, i);
    }
    s.x0 ^= INS_BYTE64(0x80, len);

    P12();
    *s_ptr = s;
}

void ascon_hash_squeeze(state *s_ptr, unsigned char *out, unsigned long long len, int xof) {
    state s = *s_ptr;
    if (!xof) len = 32;
    while (len > RATE) {
        for (int i = 0; i < 8; ++i, ++out) {
           *out = EXT_BYTE64(s.x0, i);
        }
        P12();
        len -= RATE;
    }

    for (int i = 0; i < 8; ++i, ++out) {
        *out = EXT_BYTE64(s.x0, i);
    }
}