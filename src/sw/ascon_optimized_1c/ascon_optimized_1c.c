#include "ascon/crypto_aead.h"
#include "kat.h"

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
  unsigned char a[CRYPTO_ABYTES] = {
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18
	};
  // The + 2 for the size is required because Ascon's encryption result may not be word-aligned
  unsigned char c[CRYPTO_MBYTES+CRYPTO_ABYTES+2] = {0};
  unsigned char m[CRYPTO_MBYTES] = {
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55
  };


  
  unsigned long long alen = CRYPTO_ABYTES;
  unsigned long long mlen = CRYPTO_MBYTES;
  unsigned long long clen = 16;
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
  result |= ascon_aead_encrypt_opt64(c, &clen, m, mlen, a, alen, (void*)0, n, k);

  puts("result");
  puthex(result);
  puts("\n");
  puts("c");
  print('c', c, clen - CRYPTO_ABYTES);
  puts(" t");
  print('t', c + clen - CRYPTO_ABYTES, CRYPTO_ABYTES);
  puts(" -> ");
  result |= ascon_aead_decrypt_opt64(m, &mlen, (void*)0, c, clen, a, alen, n, k);
  puts("result");
  puthex(result);
  puts("\n");
  puts("a");
  print('a', a, alen);
  puts(" ");
  puts("m");
  print('m', m, mlen);
  puts("\n");
}
