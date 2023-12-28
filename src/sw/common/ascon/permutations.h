#ifndef PERMUTATIONS_H_
#define PERMUTATIONS_H_

typedef unsigned char u8;
typedef unsigned long long u64;

typedef struct {
  u64 x0, x1, x2, x3, x4;
} state;

#define EXT_BYTE64(x, n) ((u8)((u64)(x) >> (8 * (7 - (n)))))
#define INS_BYTE64(x, n) ((u64)(x) << (8 * (7 - (n))))
#define ROTR64(x, n) (((x) >> (n)) | ((x) << (64 - (n))))

#define ROUND(C)                    \
  do {                              \
    state t, r;                     \
    sx2 ^= C;                      \
    t.x0 = sx1 ^ sx2; \
    t.x1 = sx0 ^ sx4; \
    t.x2 = sx3 ^ sx4; \
    sx4 = ~sx4; \
    sx4 = sx4 | sx3; \
    r.x2 = sx4 ^ t.x0; \
    sx3 = sx3 ^ sx1; \
    sx3 = sx3 | t.x0; \
    r.x1 = sx3 ^ t.x1; \
    sx2 = sx2 ^ t.x1; \
    sx2 = sx2 | sx1; \
    r.x0 = sx2 ^ t.x2; \
    t.x1 = ~t.x1; \
    sx1 = sx1 & t.x1; \
    r.x4 = sx1 ^ t.x2; \
    sx0 = sx0 | t.x2; \
    r.x3 = sx0 ^ t.x0; \
    sx0 = r.x0 ^ ROTR64(r.x0, 19) ^ ROTR64(r.x0, 28); \
    sx1 = r.x1 ^ ROTR64(r.x1, 61) ^ ROTR64(r.x1, 39); \
    sx2 = r.x2 ^ ROTR64(r.x2,  1) ^ ROTR64(r.x2,  6); \
    sx3 = r.x3 ^ ROTR64(r.x3, 10) ^ ROTR64(r.x3, 17); \
    sx4 = r.x4 ^ ROTR64(r.x4,  7) ^ ROTR64(r.x4, 41); \
  } while (0)

#define P12()    \
  do {           \
    ROUND(0xf0); \
    ROUND(0xe1); \
    ROUND(0xd2); \
    ROUND(0xc3); \
    ROUND(0xb4); \
    ROUND(0xa5); \
    ROUND(0x96); \
    ROUND(0x87); \
    ROUND(0x78); \
    ROUND(0x69); \
    ROUND(0x5a); \
    ROUND(0x4b); \
  } while (0)

#define P6()     \
  do {           \
    ROUND(0x96); \
    ROUND(0x87); \
    ROUND(0x78); \
    ROUND(0x69); \
    ROUND(0x5a); \
    ROUND(0x4b); \
  } while (0)

#endif  // PERMUTATIONS_H_

