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
#define STATE_VARS() register u64 s0 asm("s2"), s1 asm("s4"), s2 asm("s6"), s3 asm("s8"), s4 asm("s10"), r0 asm("t5"), t0, t1, t2
#define ASCON_LINEAR() \
  do { \
    __asm__ ("asconsigma0h	 s3, s6, s7"); \
    __asm__ ("asconsigma0l	 s2, s6, s7"); \
    __asm__ ("asconsigma2h	 s7, s10, s11"); \
    __asm__ ("asconsigma2l	 s6, s10, s11"); \
    __asm__ ("asconsigma4h	 s11, s4, s5"); \
    __asm__ ("asconsigma4l	 s10, s4, s5"); \
    __asm__ ("asconsigma1h	 s5, s8, s9"); \
    __asm__ ("asconsigma1l	 s4, s8, s9"); \
    /* It's necessary to use the r0 variable here otherwise gcc will think r0 is unused and will use the registers for something else */ \
    unsigned int hi = r0 >> 32; \
    __asm__ ("asconsigma3h	 s9, %0, %1" : : "r"(r0), "r"(hi)); \
    __asm__ ("asconsigma3l	 s8, %0, %1" : : "r"(r0), "r"(hi)); \
  } while (0)
#else
#define STATE_VARS() u64 s0, s1, s2, s3, s4, r0, t0, t1, t2
#define ASCON_LINEAR() \
  do { \
    s0 = s2 ^ ROTR64(s2, 19) ^ ROTR64(s2, 28); \
    s2 = s4 ^ ROTR64(s4,  1) ^ ROTR64(s4,  6); \
    s4 = s1 ^ ROTR64(s1,  7) ^ ROTR64(s1, 41); \
    s1 = s3 ^ ROTR64(s3, 61) ^ ROTR64(s3, 39); \
    s3 = r0 ^ ROTR64(r0, 10) ^ ROTR64(r0, 17); \
  } while (0)
#endif

#define ROUND(C)                    \
  do {                              \
    s2 ^= C;                      \
    t0 = s1 ^ s2; \
    t1 = s0 ^ s4; \
    t2 = s3 ^ s4; \
    s4 = (~s4 | s3) ^ t0; \
    s3 = ((s3 ^ s1) | t0) ^ t1; \
    s2 = ((s2 ^ t1) | s1) ^ t2; \
    s1 = (~t1 & s1) ^ t2; \
    s0 |= t2; \
    r0 = s0 ^ t0; \
    ASCON_LINEAR(); \
  } while (0)

#define P12_CONST 0xf0
#define P6_CONST 0x96

#define P12()    \
  do {           \
    STATE_VARS(); \
    s0 = s.x0; \
    s1 = s.x1; \
    s2 = s.x2; \
    s3 = s.x3; \
    s4 = s.x4; \
    for (int i = P12_CONST; i >= 0x4b; i -= 0xf) { \
      ROUND(i); \
    } \
    s.x0 = s0; \
    s.x1 = s1; \
    s.x2 = s2; \
    s.x3 = s3; \
    s.x4 = s4; \
  } while (0)

#define P6()     \
  do {           \
    STATE_VARS(); \
    s0 = s.x0; \
    s1 = s.x1; \
    s2 = s.x2; \
    s3 = s.x3; \
    s4 = s.x4; \
    for (int i = P6_CONST; i >= 0x4b; i -= 0xf) { \
      ROUND(i); \
    } \
    s.x0 = s0; \
    s.x1 = s1; \
    s.x2 = s2; \
    s.x3 = s3; \
    s.x4 = s4; \
  } while (0)

#endif  // PERMUTATIONS_H_

