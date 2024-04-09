#include "kat.h"
#include "endian.h"
#include "permutations.h"
#include "stdint.h"

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


// int ascon_hash(const unsigned char *m, unsigned long long mlen, unsigned char *c, unsigned long long outlen, int xof) {
int ascon_hash(const unsigned char *m, uint32_t mlen, unsigned char *c, uint32_t outlen, int xof) {
    // state s;
    ascon_state_t s;
    // puthex(outlen);
    ascon_hash_absorb(&s, m, mlen, xof);
    // ascon_hash_squeeze(s, c, outlen, xof);
    ascon_hash_squeeze(&s, c, outlen, xof);

    return 0;
}

// void ascon_hash_absorb(state *s_ptr, const unsigned char *m, unsigned long long len, int xof) {
void ascon_hash_absorb(ascon_state_t *s_ptr, const unsigned char *m, uint32_t len, int xof) {
    // state s = *s_ptr;
    // ascon_state_t s = *s_ptr;

    // s.x0 = xof ? ASCON_XOF_IV : ASCON_HASH_IV ;
    // s.x1 = 0;
    // s.x2 = 0;
    // s.x3 = 0;
    // s.x4 = 0;
    s_ptr->x0 = xof ? ASCON_XOF_IV : ASCON_HASH_IV ;
    s_ptr->x1 = 0;
    s_ptr->x2 = 0;
    s_ptr->x3 = 0;
    s_ptr->x4 = 0;

    // P12();
    // ascon_p12(s_ptr);
    ascon_permute(s_ptr, P12_CONST);

    // puts("In ascon_hash_absorb \n");

    while (len >= RATE) {
        // s.x0 ^= U64BIG(*(u64 *)m);
        s_ptr->x0 ^= U64BIG(*(u64 *)m);
        // P12();
        // ascon_p12(s_ptr);
        ascon_permute(s_ptr, P12_CONST);
        len -= RATE;
        m += RATE;
    }

    // puts("After ascon_hash_absorb while \n");
    for (int i = 0; i < len; ++i, ++m) {
        s_ptr->x0 ^= INS_BYTE64(*m, i);
    }
    // puts("After ascon_hash_absorb for \n");
    s_ptr->x0 ^= INS_BYTE64(0x80, len);

    // P12();
    // ascon_p12(s_ptr);
    ascon_permute(s_ptr, P12_CONST);

    // *s_ptr = s;
}

// void ascon_hash_squeeze(state *s_ptr, unsigned char *out, unsigned long long len, int xof) {
// void ascon_hash_squeeze(ascon_state_t *s_ptr, unsigned char *out, unsigned long long len, int xof) {
void ascon_hash_squeeze(ascon_state_t *s_ptr, unsigned char *out, uint32_t len, int xof) {
    // state s = *s_ptr;
    ascon_state_t s = *s_ptr;
    if (!xof) len = 32;
    // puts("In ascon_hash_squeeze \n");
    // puthex(len);
    while (len > RATE) {
        for (int i = 0; i < 8; ++i, ++out) {
        //    *out = EXT_BYTE64(s.x0, i);
           *out = EXT_BYTE64(s_ptr->x0, i);
        }
        // P12();
        // ascon_p12(s_ptr);
        ascon_permute(s_ptr, P12_CONST);
        len -= RATE;
    }
    // puts("After ascon_hash_squeeze while \n");

    for (int i = 0; i < 8; ++i, ++out) {
        // *out = EXT_BYTE64(s.x0, i);
        *out = EXT_BYTE64(s_ptr->x0, i);
    }
    // *s_ptr = s;
    // puts("After ascon_hash_squeeze for \n");
}