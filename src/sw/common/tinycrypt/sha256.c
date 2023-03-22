/* sha256.c - TinyCrypt SHA-256 crypto hash algorithm implementation */

/*
 *  Copyright (C) 2017 by Intel Corporation, All Rights Reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *    - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *    - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    - Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include <tinycrypt/sha256.h>
#include <tinycrypt/constants.h>
#include <tinycrypt/utils.h>

static void compress(unsigned int *iv, const uint8_t *data);

int tc_sha256_init(TCSha256State_t s)
{
	/* input sanity check: */
	if (s == (TCSha256State_t) 0) {
		return TC_CRYPTO_FAIL;
	}

	/*
	 * Setting the initial state values.
	 * These values correspond to the first 32 bits of the fractional parts
	 * of the square roots of the first 8 primes: 2, 3, 5, 7, 11, 13, 17
	 * and 19.
	 */
	_set((uint8_t *) s, 0x00, sizeof(*s));
	s->iv[0] = 0x6a09e667;
	s->iv[1] = 0xbb67ae85;
	s->iv[2] = 0x3c6ef372;
	s->iv[3] = 0xa54ff53a;
	s->iv[4] = 0x510e527f;
	s->iv[5] = 0x9b05688c;
	s->iv[6] = 0x1f83d9ab;
	s->iv[7] = 0x5be0cd19;

	return TC_CRYPTO_SUCCESS;
}

int tc_sha256_update(TCSha256State_t s, const uint8_t *data, size_t datalen)
{

	/* input sanity check: */
    if (s == (TCSha256State_t) 0 || data == (void *) 0) {
        return TC_CRYPTO_FAIL;
    } else if (datalen == 0) {
        return TC_CRYPTO_SUCCESS;
    }

    
    while (datalen-- > 0) {
        s->leftover[s->leftover_offset++] = *(data++);
        if (s->leftover_offset >= TC_SHA256_BLOCK_SIZE) {
            compress(s->iv, s->leftover);
            s->leftover_offset = 0;
            s->bits_hashed += (TC_SHA256_BLOCK_SIZE << 3);
        }
    }

	return TC_CRYPTO_SUCCESS;
}

int tc_sha256_final(uint8_t *digest, TCSha256State_t s)
{
	unsigned int i;

	/* input sanity check: */
	if (digest == (uint8_t *) 0 ||
	    s == (TCSha256State_t) 0) {
		return TC_CRYPTO_FAIL;
	}

	s->bits_hashed += (s->leftover_offset << 3);

	s->leftover[s->leftover_offset++] = 0x80; /* always room for one byte */
	if (s->leftover_offset > (sizeof(s->leftover) - 8)) {
		/* there is not room for all the padding in this block */
		_set(s->leftover + s->leftover_offset, 0x00,
		     sizeof(s->leftover) - s->leftover_offset);
		compress(s->iv, s->leftover);
		s->leftover_offset = 0;
	}

	/* add the padding and the length in big-Endian format */
	_set(s->leftover + s->leftover_offset, 0x00,
	     sizeof(s->leftover) - 8 - s->leftover_offset);
	s->leftover[sizeof(s->leftover) - 1] = (uint8_t)(s->bits_hashed);
	s->leftover[sizeof(s->leftover) - 2] = (uint8_t)(s->bits_hashed >> 8);
	s->leftover[sizeof(s->leftover) - 3] = (uint8_t)(s->bits_hashed >> 16);
	s->leftover[sizeof(s->leftover) - 4] = (uint8_t)(s->bits_hashed >> 24);
	s->leftover[sizeof(s->leftover) - 5] = (uint8_t)(s->bits_hashed >> 32);
	s->leftover[sizeof(s->leftover) - 6] = (uint8_t)(s->bits_hashed >> 40);
	s->leftover[sizeof(s->leftover) - 7] = (uint8_t)(s->bits_hashed >> 48);
	s->leftover[sizeof(s->leftover) - 8] = (uint8_t)(s->bits_hashed >> 56);

	/* hash the padding and length */
	compress(s->iv, s->leftover);

	/* copy the iv out to digest */
	for (i = 0; i < TC_SHA256_STATE_BLOCKS; ++i) {
		unsigned int t = *((unsigned int *) &s->iv[i]);
		*digest++ = (uint8_t)(t >> 24);
		*digest++ = (uint8_t)(t >> 16);
		*digest++ = (uint8_t)(t >> 8);
		*digest++ = (uint8_t)(t);
	}

	/* destroy the current state */
	_set(s, 0, sizeof(*s));

	return TC_CRYPTO_SUCCESS;
}

/*
 * Initializing SHA-256 Hash constant words K.
 * These values correspond to the first 32 bits of the fractional parts of the
 * cube roots of the first 64 primes between 2 and 311.
 */
static const unsigned int k256[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
	0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
	0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
	0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
	0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
	0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

#ifndef SHA256_RISCV_ASM
static inline unsigned int ROTR(unsigned int a, unsigned int n)
{
	return (((a) >> n) | ((a) << (32 - n)));
}

#define Sigma0(a)(ROTR((a), 2) ^ ROTR((a), 13) ^ ROTR((a), 22))
#define Sigma1(a)(ROTR((a), 6) ^ ROTR((a), 11) ^ ROTR((a), 25))
#define sigma0(a)(ROTR((a), 7) ^ ROTR((a), 18) ^ ((a) >> 3))
#define sigma1(a)(ROTR((a), 17) ^ ROTR((a), 19) ^ ((a) >> 10))

#define Ch(a, b, c)(((a) & (b)) ^ ((~(a)) & (c)))
#define Maj(a, b, c)(((a) & (b)) ^ ((a) & (c)) ^ ((b) & (c)))

static inline unsigned int BigEndian(const uint8_t **c)
{
	unsigned int n = 0;

	n = (((unsigned int)(*((*c)++))) << 24);
	n |= ((unsigned int)(*((*c)++)) << 16);
	n |= ((unsigned int)(*((*c)++)) << 8);
	n |= ((unsigned int)(*((*c)++)));
	return n;
}
#endif

static void compress(unsigned int *iv, const uint8_t *data)
{
    #ifndef SHA256_RISCV_ASM
	unsigned int a, b, c, d, e, f, g, h;
	unsigned int s0, s1;
	unsigned int t1, t2;
	unsigned int work_space[16];
	unsigned int n;
	unsigned int i;

	a = iv[0]; b = iv[1]; c = iv[2]; d = iv[3];
	e = iv[4]; f = iv[5]; g = iv[6]; h = iv[7];

	for (i = 0; i < 16; ++i) {
		n = BigEndian(&data);
		t1 = work_space[i] = n;
		t1 += h + Sigma1(e) + Ch(e, f, g) + k256[i];
		t2 = Sigma0(a) + Maj(a, b, c);
		h = g; g = f; f = e; e = d + t1;
		d = c; c = b; b = a; a = t1 + t2;
	}

	for ( ; i < 64; ++i) {
		s0 = work_space[(i+1)&0x0f];
		s0 = sigma0(s0);
		s1 = work_space[(i+14)&0x0f];
		s1 = sigma1(s1);

		t1 = work_space[i&0xf] += s0 + s1 + work_space[(i+9)&0xf];
		t1 += h + Sigma1(e) + Ch(e, f, g) + k256[i];
		t2 = Sigma0(a) + Maj(a, b, c);
		h = g; g = f; f = e; e = d + t1;
		d = c; c = b; b = a; a = t1 + t2;
	}

	iv[0] += a; iv[1] += b; iv[2] += c; iv[3] += d;
	iv[4] += e; iv[5] += f; iv[6] += g; iv[7] += h;

    #else

    // Relevant words of message schedule for current "i"
    unsigned int workspace[16];

    register uint32_t* iv_ptr asm ("a0") = iv;
    register uint8_t* leftover_ptr asm ("a1") = data;
    register unsigned int* k256_ptr asm("a3") = k256;
    register unsigned int* workspace_ptr asm("a4") = workspace;

    // REG TABLE

    // a0: Pointer to running state (not used, but necessary to caller control logic, shouldn't be modified)
    // a1: Pointer to message block (s.leftover)
    // a2: throwaway temp
    // a3: Data from compressed load/store & throwaway temp
    // a4: Pointer to workspace[] (Used for compressed load/store)
    // a5: throwaway temp

    // s0: H state variable
    // s1: G state variable
    // s2: F state variable
    // s3: E state variable
    // s4: D state variable
    // s5: C state variable
    // s6: B state variable
    // s7: A state variable

    // t3: H running state
    // t1: G running state
    // s8: F running state
    // s9: E running state
    // s10: D running state
    // s11: C running state
    // a6: B running state
    // a7: A running state

    // t0: i counter
    // t2: Saves SP, restores at the end
    // t4: workspace index mask (0x3F = 0xF <<< 2)
    // t5: Pointer to sha2_workspace[] (Used for compressed load/store)
    // t6: (A and B) from previous iteration, used as (B and C) in next iteration

    asm volatile (

        // Save S0-S7 registers to stack
        "addi sp, sp, -32 \n"
        "sw s0, 0(sp)  \n"
        "sw s1, 4(sp)  \n"
        "sw s2, 8(sp)  \n"
        "sw s3, 12(sp)  \n"
        "sw s4, 16(sp)  \n"
        "sw s5, 20(sp)  \n"
        "sw s6, 24(sp)  \n"
        "sw s7, 28(sp)  \n"
        "mv t2, sp  \n"

        // Init SHA-256 state from IV
        "mv sp, a0  \n"  //  Allows for compact encoding when assembling with C extension
        "lw s7, 0(sp)  \n"
        "lw s6, 4(sp)  \n"
        "lw s5, 8(sp)  \n"
        "lw s4, 12(sp)  \n"
        "lw s3, 16(sp)  \n"
        "lw s2, 20(sp)  \n"
        "lw s1, 24(sp)  \n"
        "lw s0, 28(sp)  \n"

        // Init t0 to round counter (0 < i < 64) and inits constants
        "mv t0, x0  \n"
        "li t4, 0x3f  \n"
        "mv t5, a4  \n"
        "mv sp, a3  \n"

        // t6 <= (B and C) from first iteration, used in computing MAJ(A, B, C)
        "and t6, s5, s6  \n"

        "sha256_compress_iter_top:  \n"

            // Choose if load W[i] from data[] (i < 16) or compute new W (i >= 16)
            "li a3, 64  \n"  // (16 << 2)
            "bge t0, a3, sha256_compress_compute_new_W  \n"

            // Fall through to load W[i] from data[], i < 16
            "sha256_compress_load_W_from_workspace:  \n"

                // a2 <= data[i]
                "lw a2, 0(a1)  \n"

                // Convert data[i] to big-endian
                "li a3, 0xFF  \n"

                //"and a5, a3, a2  \n"
                "and a4, a3, a2  \n"
                "c.slli a4, 24  \n"
                "c.slli a3, 8  \n"
                //"c.or a4, a5  \n"

                "and a5, a3, a2  \n"
                "c.slli a5, 8  \n"
                "c.slli a3, 8  \n"
                "c.or a4, a5  \n"

                "and a5, a3, a2  \n"
                "c.srli a5, 8  \n"
                "c.slli a3, 8  \n"
                "c.or a4, a5  \n"

                "and a5, a3, a2  \n"
                "c.srli a5, 24  \n"
                "c.or a4, a5  \n"
                "c.mv a2, a4  \n"

                // W[i] <= data[i]
                "c.mv a5, t5  \n"
                "c.add a5, t0  \n"
                "c.sw a2, 0(a5)  \n"
                "c.mv a4, t5  \n"

                // Increment pointer to data[i]
                "c.addi a1, 4  \n"

                // Jump to "sha256_compress_compute_state"
                "c.j sha256_compress_compute_state  \n"

            // Compute new W, i >= 16
            "sha256_compress_compute_new_W:  \n"

                // Get a2 = W[i+1 & 0xF] (equivalent to W[i-15])
                "c.mv a5, t0  \n"
                "c.addi a5, 4  \n"  // (1 << 2)
                "and a5, a5, t4  \n"
                "c.add a5, a4  \n"
                "c.lw a2, 0(a5)  \n"

                // Compute Sigma0(a2)   W[i+1]
                "sha256sig0 a2, a2  \n"

                // Get a3 = W[i+14 & 0xF] (equivalent to W[i-2])
                "c.mv a5, t0  \n"
                "addi a5, a5, 56  \n"  // (14 << 2)
                "and a5, a5, t4  \n"
                "c.add a5, a4  \n"
                "c.lw a3, 0(a5)  \n"

                // Compute Sigma1(a3)   W[i+14]
                "sha256sig1 a3, a3  \n"

                // a2 = a2 + a3 (a2 now contains Sig0(W[i-2]) + Sig1(W[i-14]))
                "c.add a2, a3  \n"

                // Get a5 = W[i] (will be replaced in this iteration)  (equivalent to W[i-16])
                "c.mv a5, t0  \n"
                "and a5, a5, t4  \n"
                "c.add a5, a4  \n"
                "c.lw a3, 0(a5)  \n"

                // a2 = a2 + a3  (a2 now contains Sig0(W[i-2]) + Sig1(W[i-14] + W[i-16]))
                "c.add a2, a3  \n"

                // Get a3 = W[i+9 & 0xF]  (equivalent to W[i-7])  \n"
                "c.mv a5, t0  \n"
                "addi a5, a5, 36  \n"  // (9 << 2)
                "and a5, a5, t4  \n"
                "c.add a5, a4  \n"
                "c.lw a3, 0(a5)  \n"

                // a2 = a2 + a3  (a2 now contains Sig0(W[i-2]) + Sig1(W[i-14] + W[i-16] + W[i-14], new W[i] is finished being computed))
                "c.add a2, a3  \n"

                // W[i] = a2 (Saves newly computed schedule word to message schedule)
                "and a3, t4, t0  \n"
                "c.add a3, a4  \n"
                "c.sw a2, 0(a3)  \n"

            // a2 now contains W[i] (such that this is reused by both branches)
            "sha256_compress_compute_state:  \n"

                "sha256sum1 a5, s3  \n"  // a5 <= Sum1(E)
                "c.add a2, a5  \n"  // a2 not contains W[i] + Sum1(E)
                
                "c.lwsp a5, 0(sp)  \n"
                "c.add a2, a5  \n"  // a2 not contains W[i]) + Sum1(E) + K[i]
                "c.add a2, s0  \n"  // a2 not contains W[i]) + Sum1(E) + K[i] + H
                
                // Updates H, G, and F variables, freeing up s4 (current E) register for temp use
                "c.mv s0, s1  \n"  // s0 now contains G
                "c.mv s1, s2  \n"  // s1 now contains F
                "c.mv s2, s3  \n"  // s2 now contains E
                
                // Compute CH(E, F, G)
                "and a5, s1, s2  \n"   // a5 <= E and F
                "xori s3, s3, -1  \n"  // s3 <= not E
                "and a3, s0, s3  \n"   // s3 <= (not E) and G
                "c.xor a5, a3  \n"  // a5 now contains CH(E, F, G)
                "c.add a2, a5  \n"  // a2 now contains W[i] + Sum1(E) + K[i] + CH(E, F, G) + H (<t1> SHA-256 variable)
                
                "add s3, s4, a2  \n"  // s3 now contains new E. At this point state variables H, G, F and E have been updated
                
                // Updates D, C, and B variables, freeing up s7 (current A) register for temp use
                "c.mv s4, s5  \n"  // s4 now contains C
                "c.mv s5, s6  \n"  // s5 now contains B
                "c.mv s6, s7  \n"  // s6 now contains A
                
                "sha256sum0 s7, s7  \n"  // s7 <= Sum0(A)
                
                // Compute MAJ(A, B, C)
                "and a5, s4, s6  \n"  // a5 <= A and C
                "xor a5, a5, t6  \n"  // a5 <= (A and C) xor (B and C) (Reuses A and B from previous iteration)
                "and t6, s5, s6  \n"  // t6 <= (A and B) (Equivalent to B and C in next iteration)
                "xor a5, a5, t6  \n"  // a5 <= (A and C) xor (B and C) xor (A and B)

                // Finish computing variable <t2>
                "c.add a5, s7  \n"
                
                "add s7, a2, a5  \n"  // A <= <t1> + <t2>, this finishes updating all state vars for this iteration

            // Increment counters and loop back to top_iter
            "c.addi t0, 4  \n"
            "c.addi sp, 4  \n"
            "slti a3, t0, 256  \n"  // (64 << 2)
            "c.bnez a3, sha256_compress_iter_top  \n"

        // Add state vars to IV, yielding next IV
        "mv sp, a0   \n"  //  Allows for compact encoding when assembling with C extension
        "lw a3, 0(sp)  \n"
        "add s7, s7, a3  \n"
        "lw a3, 4(sp)  \n"
        "add s6, s6, a3  \n"
        "lw a3, 8(sp)  \n"
        "add s5, s5, a3  \n"
        "lw a3, 12(sp)  \n"
        "add s4, s4, a3  \n"
        "lw a3, 16(sp)  \n"
        "add s3, s3, a3  \n"
        "lw a3, 20(sp)  \n"
        "add s2, s2, a3  \n"
        "lw a3, 24(sp)  \n"
        "add s1, s1, a3  \n"
        "lw a3, 28(sp)  \n"
        "add s0, s0, a3  \n"

        // Commit next IV to memory
        "sw s7, 0(sp)   \n"
        "sw s6, 4(sp)   \n"
        "sw s5, 8(sp)   \n"
        "sw s4, 12(sp)   \n"
        "sw s3, 16(sp)  \n"
        "sw s2, 20(sp)  \n"
        "sw s1, 24(sp)   \n"
        "sw s0, 28(sp)   \n"

        // Restore saved registers from stack
        "mv sp, t2  \n"
        "lw s0, 0(sp)  \n"
        "lw s1, 4(sp)  \n"
        "lw s2, 8(sp)  \n"
        "lw s3, 12(sp)  \n"
        "lw s4, 16(sp)  \n"
        "lw s5, 20(sp)  \n"
        "lw s6, 24(sp)  \n"
        "lw s7, 28(sp)  \n"
        "addi sp, sp, 32  \n"

    ::"r" (iv_ptr), "r" (leftover_ptr), "r" (k256_ptr), "r" (workspace_ptr):);

    #endif

	return TC_CRYPTO_SUCCESS;

}
