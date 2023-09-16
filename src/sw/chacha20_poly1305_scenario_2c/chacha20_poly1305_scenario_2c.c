
#define TEST_NAME "ChaCha20_Poly1305_scenario_2c"
//#include "cmptest.h"

#include <assert.h>
#include "sodium.h"
#include "quirks.h"
#include <test_utils.h>

static int scenario_2c(void) {
#undef  MLEN
#define MLEN 1224
#undef  ADLEN
#define ADLEN 40
#undef  CLEN
#define CLEN (MLEN + crypto_aead_chacha20poly1305_ietf_ABYTES)
    static const unsigned char firstkey[crypto_aead_chacha20poly1305_ietf_KEYBYTES]
        = {
            0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
            0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
            0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
            0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f
        };
// #undef  MESSAGE
// #define MESSAGE "Ladies and Gentlemen of the class of '99: If I could offer you " \
"only one tip for the future, sunscreen would be it."
    //unsigned char *m = (unsigned char *) sodium_malloc(MLEN);
    // unsigned char m[MLEN];
    unsigned char m[MLEN];
    static const unsigned char nonce[crypto_aead_chacha20poly1305_ietf_NPUBBYTES]
        = { 0x07, 0x00, 0x00, 0x00,
            0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47 };
    static const unsigned char ad[ADLEN]
        // = { 0x50, 0x51, 0x52, 0x53, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7 };
        = {
	    	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 
            0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe};
    //unsigned char *c = (unsigned char *) sodium_malloc(CLEN);
    unsigned char c[CLEN];
    //unsigned char *detached_c = (unsigned char *) sodium_malloc(MLEN);
    unsigned char detached_c[MLEN];
    //unsigned char *mac = (unsigned char *) sodium_malloc(crypto_aead_chacha20poly1305_ietf_ABYTES);
    unsigned char mac[crypto_aead_chacha20poly1305_ietf_ABYTES];
    //unsigned char *m2 = (unsigned char *) sodium_malloc(MLEN);
    unsigned char m2[MLEN];
    unsigned long long found_clen;
    unsigned long long found_maclen;
    unsigned long long m2len;
    static unsigned char *guard_page;  // from cmptest.h
    size_t i;

    puts("Start of test " TEST_NAME "\n");

    // assert(sizeof MESSAGE - 1U == MLEN);
    //memcpy(m, MESSAGE, MLEN);
    // memcpy(&m, MESSAGE, MLEN);

	uint8_t value = 0x0;

	for(i=0; i < MLEN; i++){
		
		if( value == 0xa ){
			value = 0x0;
		}
	    value++;
	    m[i] = value;
	}

    puts("Before crypto_aead_chacha20poly1305_ietf_encrypt()\n");

    //crypto_aead_chacha20poly1305_ietf_encrypt(c, &found_clen, m, MLEN, ad, ADLEN, NULL, nonce, firstkey);
    crypto_aead_chacha20poly1305_ietf_encrypt(&c, &found_clen, &m, MLEN, ad, ADLEN, NULL, nonce, firstkey);
    if (found_clen != MLEN + crypto_aead_chacha20poly1305_ietf_abytes()) {
        puts("found_clen is not properly set\n");
    }
    for (i = 0U; i < CLEN; ++i) {
        // puts(",0x%02x", (unsigned int) c[i]);
        puthex(c[i]);
        if (i % 8 == 7) {
            puts("\n");
        }
    }
    puts("\n");

    puts("Before crypto_aead_chacha20poly1305_ietf_encrypt_detached()\n");

    //crypto_aead_chacha20poly1305_ietf_encrypt_detached(detached_c, mac, &found_maclen, m, MLEN, ad, ADLEN, NULL, nonce, firstkey);
    crypto_aead_chacha20poly1305_ietf_encrypt_detached(&detached_c, &mac, &found_maclen, &m, MLEN, ad, ADLEN, NULL, nonce, firstkey);
    puts("After crypto_aead_chacha20poly1305_ietf_encrypt_detached()\n");
    if (found_maclen != crypto_aead_chacha20poly1305_ietf_abytes()) {
        puts("found_maclen is not properly set\n");
    }
    if (memcmp(detached_c, c, MLEN) != 0) {
        puts("detached ciphertext is bogus\n");
    }
    puts("Before crypto_aead_chacha20poly1305_ietf_decrypt()\n");

    //if (crypto_aead_chacha20poly1305_ietf_decrypt(m2, &m2len, NULL, c, CLEN, ad, ADLEN, nonce, firstkey) != 0) {
    if (crypto_aead_chacha20poly1305_ietf_decrypt(&m2, &m2len, NULL, &c, CLEN, ad, ADLEN, nonce, firstkey) != 0) {
        puts("crypto_aead_chacha20poly1305_ietf_decrypt() failed\n");
    }
    if (m2len != MLEN) {
        puts("m2len is not properly set\n");
    }
    //if (memcmp(m, m2, MLEN) != 0) {
    if (memcmp(&m, &m2, MLEN) != 0) {
        puts("m != m2\n");
    }
    //memset(m2, 0, m2len);
    memset(&m2, 0, m2len);
    //assert(crypto_aead_chacha20poly1305_ietf_decrypt_detached(NULL, NULL, c, MLEN, mac, ad, ADLEN, nonce, firstkey) == 0);
    assert(crypto_aead_chacha20poly1305_ietf_decrypt_detached(NULL, NULL, &c, MLEN, &mac, ad, ADLEN, nonce, firstkey) == 0);

    puts("Before crypto_aead_chacha20poly1305_ietf_decrypt_detached()\n");
    //if (crypto_aead_chacha20poly1305_ietf_decrypt_detached(m2, NULL, c, MLEN, mac, ad, ADLEN, nonce, firstkey) != 0) {
    if (crypto_aead_chacha20poly1305_ietf_decrypt_detached(&m2, NULL, &c, MLEN, &mac, ad, ADLEN, nonce, firstkey) != 0) {
        puts("crypto_aead_chacha20poly1305_ietf_decrypt_detached() failed\n");
    }
    //if (memcmp(m, m2, MLEN) != 0) {
    if (memcmp(&m, &m2, MLEN) != 0) {
        puts("detached m != m2\n");
    }

    for (i = 0U; i < CLEN; i++) {
        c[i] ^= (i + 1U);
        //if (crypto_aead_chacha20poly1305_ietf_decrypt(m2, NULL, NULL, c, CLEN, ad, ADLEN, nonce, firstkey) == 0 || memcmp(m, m2, MLEN) == 0) {
        if (crypto_aead_chacha20poly1305_ietf_decrypt(&m2, NULL, NULL, &c, CLEN, ad, ADLEN, nonce, firstkey) == 0 || memcmp(&m, &m2, MLEN) == 0) {
            puts("message can be forged\n");
        }
        c[i] ^= (i + 1U);
    }
    //crypto_aead_chacha20poly1305_ietf_encrypt(c, &found_clen, m, MLEN, NULL, 0U, NULL, nonce, firstkey);
    crypto_aead_chacha20poly1305_ietf_encrypt(&c, &found_clen, &m, MLEN, NULL, 0U, NULL, nonce, firstkey);
    if (found_clen != CLEN) {
        puts("clen is not properly set (adlen=0)\n");
    }
    for (i = 0U; i < CLEN; ++i) {
        //puts(",0x%02x", (unsigned int) c[i]);
        puthex(c[i]);
        if (i % 8 == 7) {
            puts("\n");
        }
    }
    puts("\n");

    //sodium_free(c);
    //sodium_free(detached_c);
    //sodium_free(mac);
    //sodium_free(m2);
    //sodium_free(m);

    assert(crypto_aead_chacha20poly1305_ietf_keybytes() > 0U);
    assert(crypto_aead_chacha20poly1305_ietf_keybytes() == crypto_aead_chacha20poly1305_keybytes());
    assert(crypto_aead_chacha20poly1305_ietf_npubbytes() > 0U);
    assert(crypto_aead_chacha20poly1305_ietf_npubbytes() > crypto_aead_chacha20poly1305_npubbytes());
    assert(crypto_aead_chacha20poly1305_ietf_nsecbytes() == 0U);
    assert(crypto_aead_chacha20poly1305_ietf_nsecbytes() == crypto_aead_chacha20poly1305_nsecbytes());
    assert(crypto_aead_chacha20poly1305_ietf_messagebytes_max() == crypto_aead_chacha20poly1305_ietf_MESSAGEBYTES_MAX);
    assert(crypto_aead_chacha20poly1305_IETF_KEYBYTES  == crypto_aead_chacha20poly1305_ietf_KEYBYTES);
    assert(crypto_aead_chacha20poly1305_IETF_NSECBYTES == crypto_aead_chacha20poly1305_ietf_NSECBYTES);
    assert(crypto_aead_chacha20poly1305_IETF_NPUBBYTES == crypto_aead_chacha20poly1305_ietf_NPUBBYTES);
    assert(crypto_aead_chacha20poly1305_IETF_ABYTES    == crypto_aead_chacha20poly1305_ietf_ABYTES);
    assert(crypto_aead_chacha20poly1305_IETF_MESSAGEBYTES_MAX == crypto_aead_chacha20poly1305_ietf_MESSAGEBYTES_MAX);

    return 0;
}

int main(void) {

    scenario_2c();

    puts("PASS");

    return 0;
}
