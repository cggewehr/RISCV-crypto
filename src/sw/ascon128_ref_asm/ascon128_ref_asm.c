#include <test_utils.h>
#include "api.h"

int main(void)
{
  unsigned char n[CRYPTO_NPUBBYTES] = {0, 1, 2,  3,  4,  5,  6,  7,
                                       8, 9, 10, 11, 12, 13, 14, 15};
  unsigned char k[CRYPTO_KEYBYTES] = {0, 1, 2,  3,  4,  5,  6,  7,
                                      8, 9, 10, 11, 12, 13, 14, 15};
  unsigned char a[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  unsigned char m[16] = {2, 5, 2, 5, 2, 5, 2, 5, 1, 2, 1, 2, 1, 2, 1, 2};
  unsigned char c[32], h[32], t[32];
  unsigned long long alen = 16;
  unsigned long long mlen = 16;
  unsigned long long clen = CRYPTO_ABYTES;
  int result = 0;
  int i;
  
#if defined(CRYPTO_AEAD)
  puts("CRYPTO_KEYBYTES:\n");
  for (int i = 0; i < 8; i++)
  {
    puthex(k[i]);
  }  
  puts("\n");

  puts("Start Encryption\n");
  puts(" -> \n");
  result |= crypto_aead_encrypt(c, &clen, m, mlen, a, alen, (void*)0, n, k);

  puts(" C \n");
  for (int i = 0; i < 32; i++)
  {
    puthex(c[i]);
  } 

   puts("\n ");
  puts("Cipher Text: ");
  for(i=0; i<clen;i++){
    puthex(c[i]);
  }
  puts("\n ");

  puts("\n");
  puts("Start Decrypt\n");
  
  puts(" -> \n");
  result |= crypto_aead_decrypt(m, &mlen, (void*)0, c, clen, a, alen, n, k);
  puts("\n");
  puthex(result);
#endif
  return result;
}
