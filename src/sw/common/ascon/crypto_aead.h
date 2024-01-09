#define params_encr                                                            \
    unsigned char *c, unsigned long long *clen, const unsigned char *m,        \
        unsigned long long mlen, const unsigned char *ad,                      \
        unsigned long long adlen, const unsigned char *nsec,                   \
        const unsigned char *npub, const unsigned char *k

#define params_decr                                                            \
    unsigned char *m, unsigned long long *mlen, unsigned char *nsec,           \
        const unsigned char *c, unsigned long long clen,                       \
        const unsigned char *ad, unsigned long long adlen,                     \
        const unsigned char *npub, const unsigned char *k


int ascon_aead_encrypt_opt64(params_encr);
int ascon_aead_decrypt_opt64(params_decr);
