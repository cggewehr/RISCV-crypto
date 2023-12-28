#include "kat.h"
#include "endian.h"
#include "permutations.h"
#include "crypto_aead.h"

#define RATE (64 / 8)
#define PA_ROUNDS 12
#define PB_ROUNDS 6
#define IV                                                                     \
    ((u64)(8 * (CRYPTO_KEYBYTES)) << 56 | (u64)(8 * (RATE)) << 48 |            \
     (u64)(PA_ROUNDS) << 40 | (u64)(PB_ROUNDS) << 32)

int crypto_aead_encrypt_opt64_C2_unrolled(
    unsigned char *c, unsigned long long *clen, const unsigned char *m,
    unsigned long long mlen, const unsigned char *ad, unsigned long long adlen,
    const unsigned char *nsec, const unsigned char *npub,
    const unsigned char *k) {
    const u64 K0 = U64BIG(*(u64 *)k);
    const u64 K1 = U64BIG(*(u64 *)(k + 8));
    const u64 N0 = U64BIG(*(u64 *)npub);
    const u64 N1 = U64BIG(*(u64 *)(npub + 8));
    unsigned long long sx0 asm("s2");
    unsigned long long sx1 asm("s4");
    unsigned long long sx2 asm("s6");
    unsigned long long sx3 asm("s8");
    unsigned long long sx4 asm("s10");
    state t,r;
    u64 i;
    (void)nsec;

    // set ciphertext size
    *clen = mlen + adlen;

    // initialization
    sx0 = IV;
    sx1 = K0;
    sx2 = K1;
    sx3 = N0;
    sx4 = N1;
    P12();
    sx3 ^= K0;
    sx4 ^= K1;

    // process associated data
    if (adlen) {
        while (adlen >= RATE) {
            sx0 ^= U64BIG(*(u64 *)ad);
            P6();
            adlen -= RATE;
            ad += RATE;
        }
        for (i = 0; i < adlen; ++i, ++ad)
            sx0 ^= INS_BYTE64(*ad, i);
        sx0 ^= INS_BYTE64(0x80, adlen);
        P6();
    }
    sx4 ^= 1;

    // process plaintext
    while (mlen >= RATE) {
        sx0 ^= U64BIG(*(u64 *)m);
        *(u64 *)c = U64BIG(sx0);
        P6();
        mlen -= RATE;
        m += RATE;
        c += RATE;
    }
    for (i = 0; i < mlen; ++i, ++m, ++c) {
        sx0 ^= INS_BYTE64(*m, i);
        *c = EXT_BYTE64(sx0, i);
    }
    sx0 ^= INS_BYTE64(0x80, mlen);

    // finalization
    sx1 ^= K0;
    sx2 ^= K1;
    P12();
    sx3 ^= K0;
    sx4 ^= K1;

    // set tag
    for (i = 0; i < 8; ++i, ++c) {
        *c = EXT_BYTE64(sx3, i);
    }
    for (i = 0; i < 8; ++i, ++c) {
        *c = EXT_BYTE64(sx4, i);
    }

    return 0;
}
