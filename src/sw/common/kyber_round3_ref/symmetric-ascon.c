#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "ascon_prim.h"
#include "symmetric.h"

/*************************************************
* Name:        kyber_ascon_absorb
*
* Description: Absorb step of the Ascon specialized for the Kyber context.
*
* Arguments:   - ascon_state_t *state: pointer to (uninitialized) output
*                                     Ascon state
*              - const uint8_t *seed: pointer to KYBER_SYMBYTES input
*                                     to be absorbed into state
*              - uint8_t i            additional byte of input
*              - uint8_t j            additional byte of input
**************************************************/
void kyber_ascon_absorb(ascon_state_t *state,
                        const uint8_t seed[KYBER_SYMBYTES],
                        uint8_t x,
                        uint8_t y)
{
  unsigned int i;
  uint8_t extseed[KYBER_SYMBYTES+2];

  for(i=0;i<KYBER_SYMBYTES;i++)
    extseed[i] = seed[i];
  extseed[i++] = x;
  extseed[i]   = y;

  // ascon_inithash(state);

  // shake128_absorb(state, extseed, sizeof(extseed));
  ascon_hash_absorb(state, extseed, sizeof(extseed), 1);
  // ascon_absorb(state, extseed, sizeof(extseed));

}

/*************************************************
* Name:        kyber_ascon_prf
*
* Description: Usage of ASCON_XOF as a PRF, concatenates secret and public input
*              and then generates outlen bytes of ASCON_XOF output
*
* Arguments:   - uint8_t *out:       pointer to output
*              - size_t outlen:      number of requested output bytes
*              - const uint8_t *key: pointer to the key
*                                    (of length KYBER_SYMBYTES)
*              - uint8_t nonce:      single-byte nonce (public PRF input)
**************************************************/
void kyber_ascon_prf(uint8_t *out,
                        size_t outlen,
                        const uint8_t key[KYBER_SYMBYTES],
                        uint8_t nonce)
{
  unsigned int i;
  uint8_t extkey[KYBER_SYMBYTES+1];

  for(i=0;i<KYBER_SYMBYTES;i++)
    extkey[i] = key[i];
  extkey[i] = nonce;

  // shake256(out, outlen, extkey, sizeof(extkey));
  // crypto_hash(out, outlen, extkey, sizeof(extkey));
  ascon_hash(extkey, sizeof(extkey), out, outlen, 1);

}

void ascon_squeezeblocks(uint8_t *out, size_t nblocks, ascon_state_t *state)
{
  // keccak_squeezeblocks(out, nblocks, state->s, SHAKE256_RATE);
  // ascon_squeeze(state, out, nblocks*ASCON_HASH_RATE);
  // ascon_hash_squeeze(state *s_ptr, unsigned char *out, unsigned long long len, int xof)
  ascon_hash_squeeze(state, out, nblocks*ASCON_HASH_RATE, 1);
}
