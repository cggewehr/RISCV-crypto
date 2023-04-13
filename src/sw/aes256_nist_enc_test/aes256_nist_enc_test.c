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

    pcount_reset();
    pcount_enable(1);
	(void)tc_aes256_set_encrypt_key(&s, nist_key);
    pcount_reset();
    pcount_enable(1);

    pcount_reset();
    pcount_enable(1);
    ret = tc_aes_encrypt(ciphertext, nist_input, &s);
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
	result = test_2();
	if (result == TC_FAIL) { /* terminate test */
		puts("AES256 test #2 (NIST encryption test) failed.\n");
		//goto exitTest;
	}

 //exitTest:
    if (result == TC_PASS)
        puts("PASS\n");
    else
        puts("FAIL\n");

	//return result;
}
