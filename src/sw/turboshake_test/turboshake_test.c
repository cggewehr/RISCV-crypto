//	sha3_test.c
//	2020-03-09	Markku-Juhani O. Saarinen <mjos@pqshield.com>
//	Copyright (c) 2020, PQShield Ltd. All rights reserved.

//	Unit tests for FIPS 202 -- SHA-3 HASH and SHAKE XOF.

#include "riscv_crypto.h"
#include "test_rvkat.h"
#include "sha3_api.h"

#include <string.h>

//	A test for SHAKE128 and SHAKE256.

int test_shake_tv() {
	//	Test vectors have bytes 480..511 of XOF output for given inputs.
	//	From http://csrc.nist.gov/groups/ST/toolkit/examples.html#aHashing

	const char *shake_tv[4][2] = {
		{ "SHAKE128",						// SHAKE128, message of length 0
		"43E41B45A653F2A5C4492C1ADD544512DDA2529833462B71A41A45BE97290B6F" },
		{ "SHAKE256",						// SHAKE256, message of length 0
		 "AB0BAE316339894304E35877B0C28A9B1FD166C796B9CC258A064A8F57E27F2A" },
		{ "SHAKE128",						// SHAKE128, 1600-bit test pattern
		 "44C9FB359FD56AC0A9A75A743CFF6862F17D7259AB075216C0699511643B6439" },
		{ "SHAKE256",						// SHAKE256, 1600-bit test pattern
		 "6A1A9D7846436E4DCA5728B6F760EEF0CA92BF0BE5615E96959D767197A0BEEB" }
	};


	int i, j, fail;
	sha3_ctx_t sha3;
	uint8_t md[64], in[256];

	fail = 0;

	//	simple initial output tests

	// shake128_init(&sha3);
	// shake_update(&sha3, in, rvkat_gethex(in, sizeof(in), "7216A825029DA1"));
	// shake_xof(&sha3);
	turboshake128_absorb(&sha3, in, rvkat_gethex(in, sizeof(in), "000102030405060708090A0B0C0D0E0F10"));

	// shake_out(md, 16, &sha3);
	turboshake128_squeeze(&sha3, md, 32);

	fail += rvkat_chkhex("TurboSHAKE128", md, 32,
		// "9DE6FFACF3E59693A3DE81B02F7DB77A");
		"ACBD4AA57507043BCEE55AD3F48504D815E707FE82EE3DAD6D5852C8920B905E");

	// shake256_init(&sha3);
	// shake_update(&sha3, in, rvkat_gethex(in, sizeof(in),
	// 	"FDBFF9BF3FB910A92D25651EBBFBA365"));
	// shake_xof(&sha3);
	turboshake256_absorb(&sha3, in, rvkat_gethex(in, sizeof(in), "000102030405060708090A0B0C0D0E0F10"));

	// shake_out(md, 32, &sha3);
	turboshake256_squeeze(&sha3, md, 64);

	fail += rvkat_chkhex("TurboSHAKE256", md, 64,
		"66D378DFE4E902AC4EB78F7C2E5A14F0"
        "2BC1C849E621BAE665796FB3346E6C79"
        "75705BB93C00F3CA8F83BCA479F06977"
        "AB3A60F39796B136538AAAE8BCAC8544");

	return fail;
}

//	FIPS 202: algorithm tests

int test_sha3() {
	int fail = 0;

	#ifdef RVKINTRIN_RV32
	rvkat_info("=== TurboSHAKE using sha3_f1600_rvb32() ===");
	fail += test_shake_tv();
	#endif

	return fail;
}

int main(void) {

	int result = 0;

    data_ind_timing_enable(1);

	result = test_sha3();

    if (!result)
        puts("PASS\n");
    else
        puts("FAIL\n");

	return result;
}
