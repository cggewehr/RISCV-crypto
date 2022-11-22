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

#ifndef SHA256_RISCV_ASM
    static void compress(unsigned int *iv, const uint8_t *data);
    #define SHA256_COMPRESS(a, b) compress(a, b)
#else
    void sha256_init_asm(unsigned int *iv);
    void sha256_compress_asm(unsigned int *dummy, uint8_t* data);
    void sha256_finish_asm(void);
    #define SHA256_COMPRESS(a, b) \
        sha256_init_asm(a); \
        sha256_compress_asm(a, b); \
        sha256_finish_asm();
#endif

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
	if (s == (TCSha256State_t) 0 ||
	    data == (void *) 0) {
		return TC_CRYPTO_FAIL;
	} else if (datalen == 0) {
		return TC_CRYPTO_SUCCESS;
	}

    #ifndef SHA256_RISCV_ASM
    
        while (datalen-- > 0) {
            s->leftover[s->leftover_offset++] = *(data++);
            if (s->leftover_offset >= TC_SHA256_BLOCK_SIZE) {
                compress(s->iv, s->leftover);
                s->leftover_offset = 0;
                s->bits_hashed += (TC_SHA256_BLOCK_SIZE << 3);
            }
        }
        
    #else

        asm volatile (
        
            // REG TABLE
            // a0: Base pointer to "s" struct
            // a1: Pointer to data to be hashed (unaltered)
            // a2: Length of data array in bytes (is changed inside this function)
            // a3: Pointer to s.leftover[]
            // a4: Temp for load/store and branches
            // a5: s.leftover_offset
    
            // a0 already points to s.iv, no additional arguments are required
            "jal sha256_init_asm  \n"

            "lw a5, 104(a0)  \n"  // a5 now contains s.leftover_offset
            "addi a3, a0, 40  \n"

            "sha256_update_top_loop:  \n"
            
                "c.beqz a2, sha256_update_break  \n"
                "c.addi a2, -1  \n"
            
                // Copies data[] into s.leftover[]
                // s->leftover[s->leftover_offset++] = *(data++);
                "c.lw a4, 0(a1)  \n"
                "c.sw a4, 0(a3)  \n"
                "c.addi a1, 4  \n"
                "c.addi a3, 4  \n"
                
                // if (s->leftover_offset >= TC_SHA256_BLOCK_SIZE)
                "slti a4, a4, 512  \n"
                "bne a4, x0, sha256_update_top_loop  \n"
                
                // Increment s.bits_hashed by (TC_SHA256_BLOCK_SIZE << 3) and reset s.leftover_offset
                "c.lw a4, 8(a0)  \n"  // Compressed L/S shifts offset to the left by 2
                "addi a4, a4, 512  \n"
                "c.sw a4, 8(a0)  \n"  // Compressed L/S shifts offset to the left by 2
                "c.xor a5, a5  \n"

                // Save to stack a1
                "sw a1, -4(sp)  \n"
                
                // Compress current message block
                "jal sha256_compress_asm  \n"
                
                "lw a1, -4(sp)  \n"
                
                "j sha256_update_top_loop  \n"

            // Restores registers to ABI-compliant state and returns to caller (state was previously saved in sha256_init_asm())
            "sha256_update_break:  \n"
            
                // Commit leftover offset to memory and return
                "sw a5, 104(a0)  \n"
                "jal sha256_finish_asm  \n"

        :: "r" (s), "r" (data):);
        
    #endif

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
		// compress(s->iv, s->leftover);
        SHA256_COMPRESS(s->iv, s->leftover);
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
	// compress(s->iv, s->leftover);
    SHA256_COMPRESS(s->iv, s->leftover);

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

static void compress(unsigned int *iv, const uint8_t *data)
{
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
}
#else
    
void sha256_init_asm(unsigned int *iv) {
// a0: Pointer to running state
// t2: Saves SP, restores at the end (sha256_finish())

    asm volatile (

        // Save regs to stack
        "addi sp, sp, -76  \n"
        "c.swsp s0, 0(sp)  \n"
        "c.swsp s1, 4(sp)  \n"
        "c.swsp s2, 8(sp)  \n"
        "c.swsp s3, 12(sp)  \n"
        "c.swsp s4, 16(sp)  \n"
        "c.swsp s5, 20(sp)  \n"
        "c.swsp s6, 24(sp)  \n"
        "c.swsp s7, 28(sp)  \n"
        "c.swsp s8, 32(sp)  \n"
        "c.swsp s9, 36(sp)  \n"
        "c.swsp s10, 40(sp)  \n"
        "c.swsp s11, 44(sp)  \n"
        "c.swsp a0, 48(sp)  \n"
        "c.swsp a1, 52(sp)  \n"
        "c.swsp a2, 56(sp)  \n"
        "c.swsp a3, 60(sp)  \n"
        "c.swsp a4, 64(sp)  \n"
        "c.swsp a5, 68(sp)  \n"
        "c.swsp a6, 72(sp)  \n"
        "c.swsp a7, 76(sp)  \n"

        // Init running hash value from given "state" array (register a0)
        "c.mv t2, sp  \n"
        "c.mv sp, a0  \n"
        "c.lwsp a7, 0(sp)  \n"
        "c.lwsp a6, 4(sp)  \n"
        "c.lwsp s11, 8(sp)  \n"
        "c.lwsp s10, 12(sp)  \n"
        "c.lwsp s9, 16(sp)  \n"
        "c.lwsp s8, 20(sp)  \n"
        "c.lwsp t1, 24(sp)  \n"
        "c.lwsp t3, 28(sp)  \n"
        "c.mv sp, t2  \n"

    ::"r" (iv):);
}

void sha256_compress_asm(unsigned int *dummy, uint8_t* data) {

    // Relevant words of message schedule for current "i"
    unsigned int workspace[16];

    register unsigned int* k256_ptr asm("a3") = k256;
    register unsigned int* workspace_ptr asm("a4") = workspace;

    // REG TABLE

    // a0: Pointer to running state (not used, but necessary to caller control logic, shouldn't be modified)
    // a1: Pointer to message block
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

        // Init t0 to round counter (0 < i < 64) and inits constants
        "mv t0, x0  \n"
        "li t4, 0x3f  \n"
        //"la sp, k256  \n"
        "mv sp, a3  \n"
        //"la a4, workspace  \n"
        "c.mv t5, a4  \n"

        // Init state variables from running hash value
        "c.mv s0, t3  \n"
        "c.mv s1, t1  \n"
        "c.mv s2, s8  \n"
        "c.mv s3, s9  \n"
        "c.mv s4, s10  \n"
        "c.mv s5, s11  \n"
        "c.mv s6, a6  \n"
        "c.mv s7, a7  \n"

        // t6 <= (A and B) from first iteration, used in computing MAJ(A, B, C)
        "and t6, a6, a7  \n"

        "sha256_compress_iter_top:  \n"

            // Choose if load W[i] from data[] (i < 16) or compute new W (i >= 16)
            "li a3, 64  \n"  // (16 << 2)
            "bge t0, a3, sha256_compress_compute_new_W  \n"

            // Fall through to load W[i] from data[], i < 16
            "sha256_compress_load_W_from_workspace:  \n"

                // a2 <= data[i]
                "c.lw a2, 0(a1)  \n"

                // Convert data[i] to big-endian
                "li a3, 0xFF  \n"

                "and a5, a3, a2  \n"
                "c.slli a5, 24  \n"
                "c.slli a3, 8  \n"
                "c.or a4, a5  \n"

                "and a5, a3, a2  \n"
                "c.slli a5, 8  \n"
                "c.slli a3, 8  \n"
                "c.or a4, a5  \n"

                "and a5, a3, a2  \n"
                "c.srli a5, 8  \n"
                "c.slli a3, 8  \n"
                "c.or a4, a5  \n"

                "and a5, a3, a2  \n"
                "c.srli a5, 25  \n"
                "c.or a4, a5  \n"

                // W[i] <= data[i]
                "c.mv a4, t5  \n"
                "c.sw a2, 0(a4)  \n"

                // Increment pointer to data[i]
                "c.addi a1, 4  \n"

                // Jump to "sha256_compress_compute_state"
                "c.j sha256_compress_compute_state  \n"

            // Compute new W, i >= 16
            "sha256_compress_compute_new_W:  \n"

                // Get a2 = W[i+1 & 0xF] (equivalent to W[i-15])
                "c.mv t0, a5  \n"
                "c.addi a5, 4  \n"  // (1 << 2)
                "c.andi a5, 0xF  \n"
                "c.add a5, a4  \n"
                "c.lw a2, 0(a5)  \n"

                // Compute Sigma0(a2)   W[i+1]
                "sha256sig0 a2, a2  \n"

                // Get a3 = W[i+14 & 0xF] (equivalent to W[i-2])
                "c.mv t0, a5  \n"
                "addi a5, a5, 56  \n"  // (14 << 2)
                "and a5, a5, t4  \n"
                "c.add a5, a4  \n"
                "c.lw a3, 0(a5)  \n"

                // Compute Sigma1(a3)   W[i+14]
                "sha256sig1 a3, a3  \n"

                // a2 = a2 + a3 (a2 now contains Sig0(W[i-2]) + Sig1(W[i-14]))
                "c.add a2, a3  \n"

                // Get a5 = W[i] (will be replaced in this iteration)  (equivalent to W[i-16])
                "c.lw a3, 0(a4)  \n"

                // a2 = a2 + a3  (a2 now contains Sig0(W[i-2]) + Sig1(W[i-14] + W[i-16]))
                "c.add a2, a3  \n"

                // Get a3 = W[i+9 & 0xF]  (equivalent to W[i-7])  \n"
                "c.mv t0, a5  \n"
                "addi a5, a5, 36  \n"  // (9 << 2)
                "and a5, a5, t4  \n"
                "c.add a5, a4  \n"
                "c.lw a3, 0(a5)  \n"

                // a2 = a2 + a3  (a2 now contains Sig0(W[i-2]) + Sig1(W[i-14] + W[i-16] + W[i-14], new W[i] is finished being computed))
                "c.add a2, a3  \n"

                // W[i] = a2 (Saves newly computed schedule word to message schedule)
                "c.sw a2, 0(a4)  \n"

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
                "and a5, s1, s2  \n"
                "xori s4, s4, -1  \n"
                "and a3, s0, s4  \n"
                "c.xor a5, a3  \n"  // a5 now contains CH(E, F, G)
                "c.add a2, a5  \n"  // a2 not contains W[i] + Sum1(E) + K[i] + CH(E, F, G) + H (contains <t1> SHA-256 variable)
                
                "add s3, s4, a5  \n"  // s3 now contains new E. At this point state variables H, G, F and E have been updated
                
                // Updates D, C, and B variables, freeing up s7 (current A) register for temp use
                "c.mv s4, s5  \n"  // s4 now contains C
                "c.mv s5, s6  \n"  // s5 now contains B
                "c.mv s6, s7  \n"  // s6 now contains A
                
                "sha256sum0 s7, s7  \n"  // s7 <= Sum0(A)
                
                // Compute MAJ(A, B, C)
                "and a5, s4, s6  \n"
                "xor a5, a5, t6  \n"
                "and t6, s5, s6  \n"
                "xor a5, a5, t6  \n"  // At this point a5 contains SHA-256 variable <t2>
                
                "add s7, a2, a5  \n"  // A <= <t1> + <t2>, this finishes updating all state vars for this iteration

            // Increment counters and loop back to top_iter
            "c.addi t0, 4  \n"
            "c.addi sp, 4  \n"
            "slti a3, t0, 64  \n"
            "c.bnez a3, sha256_compress_iter_top  \n"

        // Add state vars to running state
        "c.add t3, s0   \n"
        "c.add t1, s1   \n"
        "c.add s8, s2   \n"
        "c.add s9, s3   \n"
        "c.add s10, s4  \n"
        "c.add s11, s5  \n"
        "c.add a6, s6   \n"
        "c.add a7, s7   \n"

        "c.mv sp, t2  \n"

    :: "r" (k256_ptr), "r" (workspace_ptr), "r" (data), "r" (dummy):);
}

// Save final hash value to mem (given state[] array), restore S0~S11 and A0~A7 from stack
void sha256_finish_asm(void) {

    asm volatile (

        // Commit running hash value to state array
        "c.mv sp, a0    \n"
        "c.swsp t3, 0(sp)   \n"
        "c.swsp t1, 4(sp)   \n"
        "c.swsp s8, 8(sp)   \n"
        "c.swsp s9, 12(sp)   \n"
        "c.swsp s10, 16(sp)  \n"
        "c.swsp s11, 20(sp)  \n"
        "c.swsp a6, 24(sp)   \n"
        "c.swsp a7, 28(sp)   \n"

        // Restore register state from stack
        "c.lwsp s0, 0(sp)    \n"
        "c.lwsp s1, 4(sp)    \n"
        "c.lwsp s2, 8(sp)    \n"
        "c.lwsp s3, 12(sp)    \n"
        "c.lwsp s4, 16(sp)    \n"
        "c.lwsp s5, 20(sp)    \n"
        "c.lwsp s6, 24(sp)    \n"
        "c.lwsp s7, 28(sp)    \n"
        "c.lwsp s8, 32(sp)    \n"
        "c.lwsp s9, 36(sp)    \n"
        "c.lwsp s10, 40(sp)  \n"
        "c.lwsp s11, 44(sp)  \n"
        "c.lwsp a0, 48(sp)   \n"
        "c.lwsp a1, 52(sp)   \n"
        "c.lwsp a2, 56(sp)   \n"
        "c.lwsp a3, 60(sp)   \n"
        "c.lwsp a4, 64(sp)   \n"
        "c.lwsp a5, 68(sp)   \n"
        "c.lwsp a6, 72(sp)   \n"
        "c.lwsp a7, 76(sp)   \n"
        "addi sp, sp, 76   \n"

    :::);
}
#endif
