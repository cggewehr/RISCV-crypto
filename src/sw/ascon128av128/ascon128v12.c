#include "ascon.h"

int crypto_aead_encrypt(unsigned char* c, unsigned long long* clen,
                        const unsigned char* m, unsigned long long mlen,
                        const unsigned char* ad, unsigned long long adlen,
                        const unsigned char* nsec, const unsigned char* npub,
                        const unsigned char* k) {
  *clen = mlen + CRYPTO_ABYTES;
  (void)nsec;

  return ascon_core(c, m, mlen, ad, adlen, npub, k, 1);
}


int crypto_aead_decrypt(unsigned char* m, unsigned long long* mlen,
                        unsigned char* nsec, const unsigned char* c,
                        unsigned long long clen, const unsigned char* ad,
                        unsigned long long adlen, const unsigned char* npub,
                        const unsigned char* k) {
  if (clen < CRYPTO_ABYTES) {
    *mlen = 0;
    return -1;
  }

  *mlen = clen - CRYPTO_ABYTES;
  (void)nsec;

  return ascon_core(m, c, *mlen, ad, adlen, npub, k, -1);
}


void print(unsigned char c, unsigned char* x, unsigned long long xlen) {
  unsigned long long i;
  puts("[");
  puthex((int)xlen);
  puts("]=");
  
  for (i = 0; i < xlen; ++i) puthex(x[i]);
  puts("\n");
}

int main() {
  unsigned char n[CRYPTO_NPUBBYTES] = {0, 1, 2,  3,  4,  5,  6,  7,
                                       8, 9, 10, 11, 12, 13, 14, 15};
  unsigned char k[CRYPTO_KEYBYTES] = {0, 1, 2,  3,  4,  5,  6,  7,
                                      8, 9, 10, 11, 12, 13, 14, 15};
  unsigned char a[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  unsigned char m[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  unsigned char c[32], h[32], t[32];
  unsigned long long alen = 16;
  unsigned long long mlen = 16;
  unsigned long long clen = CRYPTO_ABYTES;
  int result = 0;
  puts("k");
  print('k', k, CRYPTO_KEYBYTES);
  puts(" n");
  print('n', n, CRYPTO_NPUBBYTES);
  puts("\na");
  print('a', a, alen);
  puts(" m");
  print('m', m, mlen);
  puts(" -> ");
  result |= crypto_aead_encrypt(c, &clen, m, mlen, a, alen, (void*)0, n, k);
  puts("c");
  print('c', c, clen - CRYPTO_ABYTES);
  puts(" t");
  print('t', c + clen - CRYPTO_ABYTES, CRYPTO_ABYTES);
  puts(" -> ");
  result |= crypto_aead_decrypt(m, &mlen, (void*)0, c, clen, a, alen, n, k);
  puts("a");
  print('a', a, alen);
  puts(" ");
  puts("m");
  print('m', m, mlen);
  puts("\n");
}
