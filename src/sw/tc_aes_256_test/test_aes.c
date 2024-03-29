/* test_aes.c - TinyCrypt AES-256 tests (including NIST tests) */

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

/*
 * DESCRIPTION
 * This module tests the following AES routines:
 *
 * Scenarios tested include:
 * - AES256 NIST key schedule test
 * - AES256 NIST encryption test
 */

#include <tinycrypt/aes.h>
#include <tinycrypt/constants.h>
#include <test_utils.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdint.h>
#include <stddef.h>

#define NUM_OF_NIST_KEYS 32
#define NUM_OF_FIXED_KEYS 128


struct kat_table {
	uint8_t in[NUM_OF_NIST_KEYS];
	uint8_t out[NUM_OF_NIST_KEYS];
};

/*
 * NIST test key schedule.
 */
int test_1(void)
{
	int result = TC_PASS;
	// const uint8_t nist_key[NUM_OF_NIST_KEYS] = {
		// 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
		// 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
	// };
	const uint8_t nist_key[NUM_OF_NIST_KEYS] = {
		0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
		0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
		0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
		0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
	};
	// const struct tc_aes_key_sched_struct expected = {
		// {
			// 0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c,
			// 0xa0fafe17, 0x88542cb1, 0x23a33939, 0x2a6c7605,
			// 0xf2c295f2, 0x7a96b943, 0x5935807a, 0x7359f67f,
			// 0x3d80477d, 0x4716fe3e, 0x1e237e44, 0x6d7a883b,
			// 0xef44a541, 0xa8525b7f, 0xb671253b, 0xdb0bad00,
			// 0xd4d1c6f8, 0x7c839d87, 0xcaf2b8bc, 0x11f915bc,
			// 0x6d88a37a, 0x110b3efd, 0xdbf98641, 0xca0093fd,
			// 0x4e54f70e, 0x5f5fc9f3, 0x84a64fb2, 0x4ea6dc4f,
			// 0xead27321, 0xb58dbad2, 0x312bf560, 0x7f8d292f,
			// 0xac7766f3, 0x19fadc21, 0x28d12941, 0x575c006e,
			// 0xd014f9a8, 0xc9ee2589, 0xe13f0cc8, 0xb6630ca6
		// }
	// };
	const struct tc_aes_key_sched_struct expected = {
		{
			0x603deb10, 0x15ca71be, 0x2b73aef0, 0x857d7781,
			0x1f352c07, 0x3b6108d7, 0x2d9810a3, 0x0914dff4,
			0x9ba35411, 0x8e6925af, 0xa51a8b5f, 0x2067fcde,
			0xa8b09c1a, 0x93d194cd, 0xbe49846e, 0xb75d5b9a,
			0xd59aecb8, 0x5bf3c917, 0xfee94248, 0xde8ebe96,
			0xb5a9328a, 0x2678a647, 0x98312229, 0x2f6c79b3,
			0x812c81ad, 0xdadf48ba, 0x24360af2, 0xfab8b464,
			0x98c5bfc9, 0xbebd198e, 0x268c3ba7, 0x09e04214,
			0x68007bac, 0xb2df3316, 0x96e939e4, 0x6c518d80,
			0xc814e204, 0x76a9fb8a, 0x5025c02d, 0x59c58239,
			0xde136967, 0x6ccc5a71, 0xfa256395, 0x9674ee15,
			0x5886ca5d, 0x2e2f31d7, 0x7e0af1fa, 0x27cf73c3,
			0x749c47ab, 0x18501dda, 0xe2757e4f, 0x7401905a,
			0xcafaaae3, 0xe4d59b34, 0x9adf6ace, 0xbd10190d,
			0xfe4890d1, 0xe6188d0b, 0x046df344, 0x706c631e
		}
	};
	struct tc_aes_key_sched_struct s;

	puts("AES256 (NIST key schedule test):\n");

	if (tc_aes256_set_encrypt_key(&s, nist_key) == 0) {
		puts("AES256 test (NIST key schedule test) failed.\n");
		result = TC_FAIL;
		goto exitTest1;
	}

	result = check_result(1, expected.words, sizeof(expected.words), s.words,
			      sizeof(s.words));

exitTest1:
	//TC_END_RESULT(result);
    if (result == TC_PASS)
        puts("PASS\n");
    else
        puts("FAIL\n");
	return result;
}

/*
 * NIST test vectors for encryption.
 */
int test_2(void)
{
	int result = TC_PASS;
    int ret;
	// const uint8_t nist_key[NUM_OF_NIST_KEYS] = {
		// 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
		// 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
	// };
	const uint8_t nist_key[NUM_OF_NIST_KEYS] = {
		0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
		0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
		0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
		0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
	};
	// const uint8_t nist_input[NUM_OF_NIST_KEYS] = {
		// 0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
		// 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34
	// };
	const uint8_t nist_input[NUM_OF_NIST_KEYS/2] = {
		0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
		0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34
	};
	// const uint8_t expected[NUM_OF_NIST_KEYS] = {
		// 0x39, 0x25, 0x84, 0x1d, 0x02, 0xdc, 0x09, 0xfb,
		// 0xdc, 0x11, 0x85, 0x97, 0x19, 0x6a, 0x0b, 0x32
	// };
	const uint8_t expected[NUM_OF_NIST_KEYS/2] = {
		0x30, 0x21, 0x61, 0x3a, 0x97, 0x3e, 0x58, 0x2f,
        0x4a, 0x29, 0x23, 0x41, 0x37, 0xae, 0xc4, 0x94
	};
	struct tc_aes_key_sched_struct s;
	uint8_t ciphertext[NUM_OF_NIST_KEYS/2];

	puts("AES256 (NIST encryption test):\n");

	(void)tc_aes256_set_encrypt_key(&s, nist_key);
	puts("After Key Sched\n");

    pcount_reset();
    pcount_enable(1);
    ret = tc_aes_encrypt(ciphertext, nist_input, &s);
	puts("After Enc\n");
    pcount_reset();
    pcount_enable(1);

	if (ret == 0) {
		puts("AES256 %s (NIST encryption test) failed.\n");
		result = TC_FAIL;
		goto exitTest2;
	}

	result = check_result(2, expected, sizeof(expected), ciphertext,
			      sizeof(ciphertext));

exitTest2:
	//TC_END_RESULT(result);
    if (result == TC_PASS)
        puts("PASS\n");
    else
        puts("FAIL\n");

	return result;
}

/*
 * Main task to test AES
 */
int main(void)
{
	int result = TC_PASS;

	puts("Performing AES256 tests:");

	//result = test_1();
	//if (result == TC_FAIL) { /* terminate test */
	//	puts("AES256 test #1 (NIST key schedule test) failed.\n");
		//goto exitTest;
	//}
	result = test_2();
	if (result == TC_FAIL) { /* terminate test */
		puts("AES256 test #2 (NIST encryption test) failed.\n");
		goto exitTest;
	}
	// result = test_3();
	// if (result == TC_FAIL) { /* terminate test */
		// puts("AES128 test #3 (NIST fixed-key and variable-text) "
			 // "failed.\n");
		// goto exitTest;
	// }
	// result = test_4();
	// if (result == TC_FAIL) { /* terminate test */
		// puts("AES128 test #4 (NIST variable-key and fixed-text) "
			 // "failed.\n");
		// goto exitTest;
	// }

	puts("All AES256 tests succeeded!\n");

 exitTest:
	//TC_END_RESULT(result);
	//TC_END_REPORT(result);

	return result;
}
