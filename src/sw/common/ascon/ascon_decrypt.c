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

int crypto_aead_decrypt_opt64_C2_unrolled(
    unsigned char *m, unsigned long long *mlen, unsigned char *nsec,
    const unsigned char *c, unsigned long long clen, const unsigned char *ad,
    unsigned long long adlen, const unsigned char *npub,
    const unsigned char *k) {
    if (clen < adlen) {
        *mlen = 0;
        return -1;
    }

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

    // set plaintext size
    *mlen = clen - adlen;

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
    clen -= CRYPTO_ABYTES;
    while (clen >= RATE) {
        *(u64 *)m = U64BIG(sx0) ^ *(u64 *)c;
        sx0 = U64BIG(*((u64 *)c));
        P6();
        clen -= RATE;
        m += RATE;
        c += RATE;
    }
    for (i = 0; i < clen; ++i, ++m, ++c) {
        *m = EXT_BYTE64(sx0, i) ^ *c;
        sx0 &= ~INS_BYTE64(0xff, i);
        sx0 |= INS_BYTE64(*c, i);
    }
    sx0 ^= INS_BYTE64(0x80, clen);

    // finalization
    sx1 ^= K0;
    sx2 ^= K1;
    P12();
    sx3 ^= K0;
    sx4 ^= K1;

    for (i = 0; i < 8; ++i, ++c) {
        sx3 ^= INS_BYTE64(*c, i);
    }
    for (i = 0; i < 8; ++i, ++c) {
        sx4 ^= INS_BYTE64(*c, i);
    }

    // verify tag (should be constant time, check compiler output)
    if ((sx3 | sx4) != 0) {
        *mlen = 0;
        return -1;
    }

    return 0;
}
