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

#if ASCON_ISE == 1
#define ASCON_LINEAR() \
  do { \
    unsigned int hi = r.x0 >> 32; \
    unsigned int rd_lo, rd_hi; \
    __asm__ ("asconsigma0h	 %0, %1, %2" : "=&r"(rd_hi) : "r"(r.x0), "r"(hi)); \
    __asm__ ("asconsigma0l	 %0, %1, %2" : "=&r"(rd_lo) : "r"(r.x0), "r"(hi)); \
    s.x0 = rd_lo | (u64) rd_hi << 32; \
    hi = r.x1 >> 32; \
    __asm__ ("asconsigma1h	 %0, %1, %2" : "=&r"(rd_hi) : "r"(r.x1), "r"(hi)); \
    __asm__ ("asconsigma1l	 %0, %1, %2" : "=&r"(rd_lo) : "r"(r.x1), "r"(hi)); \
    s.x1 = rd_lo | (u64) rd_hi << 32; \
    hi = r.x2 >> 32; \
    __asm__ ("asconsigma2h	 %0, %1, %2" : "=&r"(rd_hi) : "r"(r.x2), "r"(hi)); \
    __asm__ ("asconsigma2l	 %0, %1, %2" : "=&r"(rd_lo) : "r"(r.x2), "r"(hi)); \
    s.x2 = rd_lo | (u64) rd_hi << 32; \
    hi = r.x3 >> 32; \
    __asm__ ("asconsigma3h	 %0, %1, %2" : "=&r"(rd_hi) : "r"(r.x3), "r"(hi)); \
    __asm__ ("asconsigma3l	 %0, %1, %2" : "=&r"(rd_lo) : "r"(r.x3), "r"(hi)); \
    s.x3 = rd_lo | (u64) rd_hi << 32; \
    hi = r.x4 >> 32; \
    __asm__ ("asconsigma4h	 %0, %1, %2" : "=&r"(rd_hi) : "r"(r.x4), "r"(hi)); \
    __asm__ ("asconsigma4l	 %0, %1, %2" : "=&r"(rd_lo) : "r"(r.x4), "r"(hi)); \
    s.x4 = rd_lo | (u64) rd_hi << 32; \
  } while (0)
#else
#define ASCON_LINEAR() \
  do { \
    s.x0 = r.x0 ^ ROTR64(r.x0, 19) ^ ROTR64(r.x0, 28); \
    s.x1 = r.x1 ^ ROTR64(r.x1, 61) ^ ROTR64(r.x1, 39); \
    s.x2 = r.x2 ^ ROTR64(r.x2,  1) ^ ROTR64(r.x2,  6); \
    s.x3 = r.x3 ^ ROTR64(r.x3, 10) ^ ROTR64(r.x3, 17); \
    s.x4 = r.x4 ^ ROTR64(r.x4,  7) ^ ROTR64(r.x4, 41); \  
  } while (0)
#endif

#define ROUND(C)                    \
  do {                              \
    state t, r;                     \
    s.x2 ^= C;                      \
    t.x0 = s.x1 ^ s.x2; \
    t.x1 = s.x0 ^ s.x4; \
    t.x2 = s.x3 ^ s.x4; \
    s.x4 = ~s.x4; \
    s.x4 = s.x4 | s.x3; \
    r.x2 = s.x4 ^ t.x0; \
    s.x3 = s.x3 ^ s.x1; \
    s.x3 = s.x3 | t.x0; \
    r.x1 = s.x3 ^ t.x1; \
    s.x2 = s.x2 ^ t.x1; \
    s.x2 = s.x2 | s.x1; \
    r.x0 = s.x2 ^ t.x2; \
    t.x1 = ~t.x1; \
    s.x1 = s.x1 & t.x1; \
    r.x4 = s.x1 ^ t.x2; \
    s.x0 = s.x0 | t.x2; \
    r.x3 = s.x0 ^ t.x0; \
    ASCON_LINEAR(); \
  } while (0)

#define P12_CONST 0xf0
#define P6_CONST 0x96

#define P12()    \
  do {           \
    for (int i = P12_CONST; i >= 0x4b; i -= 0xf) { \
      ROUND(i); \
    } \
  } while (0)

#define P6()     \
  do {           \
    for (int i = P6_CONST; i >= 0x4b; i -= 0xf) { \
      ROUND(i); \
    } \
  } while (0)

#endif  // PERMUTATIONS_H_

