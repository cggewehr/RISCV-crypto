#include <stdint.h>

int ascon_hash(const unsigned char *m, uint32_t mlen, const unsigned char *c, uint32_t clen,  int xof);