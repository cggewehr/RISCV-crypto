#include <stdio.h>
#include <string.h>

#include "api.h"
#if defined(AVR_UART)
#include "avr_uart.h"
#endif
#if defined(CRYPTO_AEAD)
#include "crypto_aead.h"
#elif defined(CRYPTO_HASH)
#include "crypto_hash.h"
#elif defined(CRYPTO_AUTH)
#include "crypto_auth.h"
#endif



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
  int i;
#if defined(AVR_UART)
  avr_uart_init();
  stdout = &avr_uart_output;
  stdin = &avr_uart_input_echo;
#endif
#if defined(CRYPTO_AEAD)

  puts("key: ");
  for(i=0; i<CRYPTO_KEYBYTES;i++){
    puthex(k[i]);
  }
  
  puts("\n");
  puts("Nonce: ");
  for(i=0; i<CRYPTO_NPUBBYTES;i++){
    puthex(n[i]);
  }
  puts("\n");
  puts("Associated Data: ");
  for(i=0; i<alen;i++){
    puthex(a[i]);
  }
  puts("\n");
  puts("m: ");
  for(i=0; i<mlen;i++){
    puthex(m[i]);
  }
  puts("\n -> \n");
  result |= crypto_aead_encrypt(c, &clen, m, mlen, a, alen, (void*)0, n, k);

  puts("Cipher Text: ");
  for(i=0; i<clen;i++){
    puthex(c[i]);
  }
  puts("\n ");

  puts("t: ");
  for(i=0; i<CRYPTO_ABYTES;i++){
    puthex(t[i]);
  }
  puts("\n -> \n");
  result |= crypto_aead_decrypt(m, &mlen, (void*)0, c, clen, a, alen, n, k);

  puts("a: ");
  for(i=0; i<alen;i++){
   puthex(a[i]);
  }

  puts("m: ");
  for(i=0; i<mlen;i++){
   puthex(m[i]);
  }  
  puts("\n");

#endif
  return result;
}