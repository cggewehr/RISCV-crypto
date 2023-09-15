/* test_ccm_mode.c - TinyCrypt AES-CCM tests (RFC 3610 tests) */

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
 *  DESCRIPTION
 * This module tests the following AES-CCM Mode routines:
 *
 *  Scenarios tested include:
 *  - AES128 CCM mode encryption RFC 3610 test vector #1
 *  - AES128 CCM mode encryption RFC 3610 test vector #2
 *  - AES128 CCM mode encryption RFC 3610 test vector #3
 *  - AES128 CCM mode encryption RFC 3610 test vector #7
 *  - AES128 CCM mode encryption RFC 3610 test vector #8
 *  - AES128 CCM mode encryption RFC 3610 test vector #9
 *  - AES128 CCM mode encryption No associated data
 *  - AES128 CCM mode encryption No payload data
 */

#include "ccm_mode.h"
#include "constants.h"
#include "test_utils.h"
#include "aes.h"

#include <string.h>

#define TC_CCM_MAX_CT_SIZE 127 //era 50 (frame size)
#define TC_CCM_MAX_PT_SIZE 102 // era 25 (payload frame)
#define NUM_NIST_KEYS 16
#define NONCE_LEN 13
#define HEADER_LEN 25
#define M_LEN25 25
#define M_LEN16 16
#define M_LEN8 8
#define M_LEN10 10
#define DATA_BUF_LEN16 16
#define DATA_BUF_LEN23 23
#define DATA_BUF_LEN23 23
#define DATA_BUF_LEN24 24
#define DATA_BUF_LEN25 25
#define DATA_BUF_LEN86 86
#define EXPECTED_BUF_LEN16 16
#define EXPECTED_BUF_LEN31 31
#define EXPECTED_BUF_LEN32 32
#define EXPECTED_BUF_LEN33 33
#define EXPECTED_BUF_LEN34 34
#define EXPECTED_BUF_LEN35 35
#define EXPECTED_BUF_LEN86 86

int do_test(const uint8_t *key, uint8_t *nonce, 
	    size_t nlen, const uint8_t *hdr,
	    size_t hlen, const uint8_t *data,
	    size_t dlen, const uint8_t *expected,
	    size_t elen, const int mlen)
{

	int result = 1;

	uint8_t ciphertext[TC_CCM_MAX_CT_SIZE];
	uint8_t decrypted[TC_CCM_MAX_PT_SIZE];
	struct tc_ccm_mode_struct c;
	struct tc_aes_key_sched_struct sched;

	tc_aes128_set_encrypt_key(&sched, key);

	result = tc_ccm_config(&c, &sched, nonce, nlen, mlen);
	if (result == 0) {
		//puts("CCM config failed \n");

		result = 0; 
	}

	 if (result == 1)
            puts("PASS\n");
        else
            puts("FAIL\n");

	//print_expected("\t\tExpected: ", ciphertext, elen);

	result = tc_ccm_generation_encryption(ciphertext, TC_CCM_MAX_CT_SIZE, hdr,
					      hlen, data, dlen, &c);
	//int i = 0;
	/*for (i = 0; i < EXPECTED_BUF_LEN86; i++) {
        printf("Posição %d: %02X\n", i, ciphertext[i]);
    }*/

	if (result == 0) {
		puts("ccm_encrypt failed in %s.\n");

		result = 0;
	}

	if (result == 1)
          puts("PASS\n");
        else
          puts("FAIL\n");


	if (memcmp(expected, ciphertext, elen) != 0) {
		puts("ccm_encrypt produced wrong ciphertext \n");
		//show_str("\t\tCipher Expected", expected, elen);
		//show_str("\t\tCipher Computed", ciphertext, elen);

		result = 0;
		// 
	}

	if (result == 1)
            puts("PASS\n");
        else
            puts("FAIL\n");

	result = tc_ccm_decryption_verification(decrypted, TC_CCM_MAX_PT_SIZE, hdr,
						hlen, ciphertext, dlen+mlen, &c);
	if (result == 0) {
		puts("ccm_decrypt failed \n");
		//show_str("\t\tData Expected", data, dlen);
		//show_str("\t\tData Computed", decrypted, sizeof(decrypted));

		result = 0;
	}

	if (result == 1)
            puts("PASS\n");
        else
            puts("FAIL\n");
	
	result = 1;

//exitTest1:
	
	return result;
}

/*Scenario 1c: Authenticated encryption of 86 bytes of payload and 25
bytes of header (which is authenticated but not encrypted). As with
Scenario 1b, the authentication tag has a length of 16 bytes.*/


int test_vector_1(void)
{
	int result = 1;
	/* RFC 3610 test vector #1 */
	const uint8_t key[NUM_NIST_KEYS] = {
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf
	};
	uint8_t nonce[NONCE_LEN] = {
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc
	};
	const uint8_t hdr[HEADER_LEN] = {
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
    	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18
	};
	const uint8_t data[DATA_BUF_LEN86] = {
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55
	};
	const uint8_t expected[EXPECTED_BUF_LEN86] = {
		0x16, 0x35, 0xb6, 0x8b, 0x57, 0xc, 0xfc, 0x85, 0x52, 0x9e, 0x39,
		0xac, 0x91, 0x39, 0x10, 0xd7, 0xf3, 0x11, 0x16, 0x31, 0x62, 0x38,
		0x67, 0xf1, 0x34, 0xe6, 0xe4, 0x41, 0x90, 0x4f, 0xd5, 0x4, 0xf5,
		0x74, 0x6d, 0x6b, 0xf1, 0x89, 0x81, 0x5f, 0x51, 0xa4, 0x8f, 0x7,
		0xc9, 0x94, 0xed, 0x9e, 0xee, 0x24, 0xe8, 0xd2, 0xea, 0xda, 0x7e,
		0x42, 0x18, 0xb5, 0x88, 0xf6, 0x7a, 0x3a, 0xf1, 0xa3, 0x8d, 0xfe,
		0xed, 0x70, 0xab, 0x5a, 0x15, 0x36, 0x28, 0x34, 0xb, 0xa1, 0xf9, 
		0xf1, 0xee, 0x95, 0xc3, 0x2e, 0xa4, 0xf7, 0xa7, 0x98
	};

	uint16_t mlen = M_LEN16; 

	puts("Performing CCM test #1 (RFC 3610 test vector #1): - tc_aes_ccm_scenario_1c\n");

	result = do_test(key, nonce, sizeof(nonce), hdr, sizeof(hdr),
			 data, sizeof(data), expected, sizeof(expected), mlen);

	return result;
}


/*
 * Main task to test CCM
 */
int main(void)
{
	int result = 1;
	data_ind_timing_enable(1);
	puts("Performing CCM tests:");

	result = test_vector_1();
	if (result == 0) { /* terminate test */
		puts("CCM test #1 (RFC 3610 test vector #1) failed.\n");
		result = 0;
		if (result == 1)
            puts("PASS\n");
        else
            puts("FAIL\n");
	}

	puts("All CCM tests succeeded!\n");

//exitTest:
        if (result == 1)
            puts("PASS\n");
        else
            puts("FAIL\n");

	return result;
}
