#include "kat.h"
#include "endian.h"
#include "permutations.h"

#define RATE (64 / 8)
#define PA_ROUNDS 12
#define PB_ROUNDS 6
#define IV                                                                     \
    ((u64)(8 * (CRYPTO_KEYBYTES)) << 56 | (u64)(8 * (RATE)) << 48 |            \
     (u64)(PA_ROUNDS) << 40 | (u64)(PB_ROUNDS) << 32)

int ascon_aead_encrypt_opt64(
    unsigned char *c, unsigned long long *clen, const unsigned char *m,
    unsigned long long mlen, const unsigned char *ad, unsigned long long adlen,
    const unsigned char *nsec, const unsigned char *npub,
    const unsigned char *k) {
    const u64 K0 = U64BIG(*(u64 *)k);
    const u64 K1 = U64BIG(*(u64 *)(k + 8));
    const u64 N0 = U64BIG(*(u64 *)npub);
    const u64 N1 = U64BIG(*(u64 *)(npub + 8));
    // state s;
    ascon_state_t s;
    u64 i;
    (void)nsec;

    // set ciphertext size
    *clen = mlen + CRYPTO_ABYTES;

    // initialization
    s.x0 = IV;
    s.x1 = K0;
    s.x2 = K1;
    s.x3 = N0;
    s.x4 = N1;
    // P12();
    // ascon_p12(&s);
    ascon_permute(&s, P12_CONST);
    s.x3 ^= K0;
    s.x4 ^= K1;

    // process associated data
    if (adlen) {
        while (adlen >= RATE) {
            s.x0 ^= U64BIG(*(u64 *)ad);
            // P6();
            // ascon_p6(&s);
            ascon_permute(&s, P6_CONST);
            adlen -= RATE;
            ad += RATE;
        }
        for (i = 0; i < adlen; ++i, ++ad)
            s.x0 ^= INS_BYTE64(*ad, i);
        s.x0 ^= INS_BYTE64(0x80, adlen);
        // P6();
        // ascon_p6(&s);
        ascon_permute(&s, P6_CONST);
    }
    s.x4 ^= 1;

    // process plaintext
    while (mlen >= RATE) {
        s.x0 ^= U64BIG(*(u64 *)m);
        *(u64 *)c = U64BIG(s.x0);
        // P6();
        // ascon_p6(&s);
        ascon_permute(&s, P6_CONST);
        mlen -= RATE;
        m += RATE;
        c += RATE;
    }
    for (i = 0; i < mlen; ++i, ++m, ++c) {
        s.x0 ^= INS_BYTE64(*m, i);
        *c = EXT_BYTE64(s.x0, i);
    }
    s.x0 ^= INS_BYTE64(0x80, mlen);

    // finalization
    s.x1 ^= K0;
    s.x2 ^= K1;
    // P12();
    // ascon_p12(&s);
    ascon_permute(&s, P12_CONST);
    s.x3 ^= K0;
    s.x4 ^= K1;

    // set tag
    for (i = 0; i < 8; ++i, ++c) {
        *c = EXT_BYTE64(s.x3, i);
    }
    for (i = 0; i < 8; ++i, ++c) {
        *c = EXT_BYTE64(s.x4, i);
    }

    return 0;
}