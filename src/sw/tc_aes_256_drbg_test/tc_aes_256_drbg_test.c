/*  test_ctr_prng.c - TinyCrypt implementation of some CTR-PRNG tests */

/*
 * Copyright (c) 2016, Chris Morrison, All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
  DESCRIPTION
  This module tests the CTR-PRNG routines
*/

#include <tinycrypt/ctr_prng.h>
#include <tinycrypt/aes.h>
#include <tinycrypt/constants.h>
#include <test_utils.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* utility function to convert hex character representation to their nibble (4 bit) values */
static uint8_t nibbleFromChar(char c)
{
	if(c >= '0' && c <= '9') return c - '0';
	if(c >= 'a' && c <= 'f') return c - 'a' + 10U;
	if(c >= 'A' && c <= 'F') return c - 'A' + 10U;
	return 255U;
}

/* 
 * Convert a string of characters representing a hex buffer into a series of 
 * bytes of that real value 
 */
// uint8_t *hexStringToBytes(char *inhex)
void *hexStringToBytes(char *inhex, char *outbytes)
{
	// uint8_t *retval;
	uint8_t *p;
	int len, i;
	
	len = strlen(inhex) / 2;
	// retval = (uint8_t *)malloc(len+1);
	for(i=0, p = (uint8_t *) inhex; i<len; i++) {
		// retval[i] = (nibbleFromChar(*p) << 4) | nibbleFromChar(*(p+1));
		outbytes[i] = (nibbleFromChar(*p) << 4) | nibbleFromChar(*(p+1));
		// puthex(outbytes[i]);
		p += 2;
	}
	// puts("\n");
	// retval[len] = 0;
	// outbytes[len] = 0;
	// return retval;
}

typedef struct {
	char * entropyString;
	char * personalizationString;  /* may be null */
	char * additionalInputString1; /* may be null */
	char * additionalInputString2; /* may be null */
	char * expectedString;
} PRNG_Vector;

/* vectors taken from NIST CAVS 14.3 CTR_DRBG.rsp */
PRNG_Vector vectors[] = {
	/*
	 * AES-256 no df, PredictionResistance = False, EntropyInputLen = 384,
	 * NonceLen = 0, PersonalizationStringLen = 0, AdditionalInputLen = 0,
	 * ReturnedBitsLen = 512
	 */
	{  /* Count 0 */
		"df5d73faa468649edda33b5cca79b0b05600419ccb7a879ddfec9db32ee494e5531b51de16a30f769262474c73bec010",
		0,
		0,
		0,
		"d1c07cd95af8a7f11012c84ce48bb8cb87189e99d40fccb1771c619bdf82ab2280b1dc2f2581f39164f7ac0c510494b3a43c41b7db17514c87b107ae793e01c5",
	},

	{ /* Count 1 */
		"3b6fb634d35bb386927374f991c1cbc9fafba3a43c432dc411b7b2fa96cfcce8d305e135ff9bc460dbc7ba3990bf8060",
		0,
		0,
		0,
		"083a836fe1cde053164555529409337dc4fec6844594fdf15083ba9d1001eb945c3b96a1bcee3990e1e51f85c80e9f4e04de34e57b640f6cae8ed68e99624712",
	},

	/*
	 * AES-256 no df, PredictionResistance = False, EntropyInputLen = 384,
	 * NonceLen = 0, PersonalizationStringLen = 0, AdditionalInputLen = 384,
	 * ReturnedBitsLen = 512
	 */
	{ /* Count 0 */
		"f45e9d040c1456f1c7f26e7f146469fbe3973007fe037239ad57623046e7ec52221b22eec208b22ac4cf4ca8d6253874",
		0,
		"28819bc79b92fc8790ebdc99812cdcea5c96e6feab32801ec1851b9f46e80eb6800028e61fbccb6ccbe42b06bf5a0864",
		"418ca848027e1b3c84d66717e6f31bf89684d5db94cd2d579233f716ac70ab66cc7b01a6f9ab8c7665fcc37dba4af1ad",
		"4f11406bd303c104243441a8f828bf0293cb20ac39392061429c3f56c1f426239f8f0c687b69897a2c7c8c2b4fb520b62741ffdd29f038b7c82a9d00a890a3ed",
	},

	{ /* Count 1 */
		"933015be052c117ad3d38dd2d1d52bda42d7f36946418b006c67aad49d8130e5ec3f0c1d6ffb0b6da00270f77ae18362",
		0,
		"0e5eccdf748549f94cab63d649145d4c3b84c74a2276d5c188cdebf417bcc9f5f19d4857e76823e00b8f08f8d583a65d",
		"12a0ed9afc1a7456f8430d5aca4cab30f75e39ad7012566c32d8c753ae6a9c59e8ee87832faac3d126056bc9554793db",
		"0615803d2aa28823445786a7ac9951b14619f2072e8de44acfe00674a3d40feaec07aaeeee947b71c7531c3a93737f3415fcce87353c85258e2301d2842b408e",
	},

	/*
	 * AES-256 no df, PredictionResistance = False, EntropyInputLen = 384,
	 * NonceLen = 0, PersonalizationStringLen = 384, AdditionalInputLen = 0,
	 * ReturnedBitsLen = 512
	 */
	{  /* Count 0 */
		"22a89ee0e37b54ea636863d9fed10821f1952a428488d528eceb9d2ec69d573ec6216216fb3e8f72a148a5ada9d620b1",
		"953c10badcbcd45fb4e5475826477fc137ac96a49ad5005fb14bdaf6468ae7f46c5d0de22d304afc67989615adc2e983",
		0,
		0,
		"f7fab6a6fcf445f0a0434b2aa0c610bdef5489ecd95414634623add18a9f888bca6be151312d1b9e8f83bd0acad6234d3bccc11b63a40d6fbff448f67db0b91f",
	},

	{ /* Count 1 */
		"a5ca32ff18305555d32e270f170529232c458779eaace221ac4958b4226df8189e42b0844fc765751a6291a60a35d8b4",
		"f42a3a32dc92a3eeff658c349eb2e181564458c202aa922ec4364e3a93b2ebdfb58ef78fc7237b70d8a261bcf30bd1b6",
		0,
		0,
		"00851cac3204326d97b5f26cd0bc05feafc34f56b5b7def2640bf5a12da0090d85320f3132fe7212c86d65f3b938366eae25cd9233c0f9941a70f99e795cde4c",
	},

	/*
	 * AES-256 no df, PredictionResistance = False, EntropyInputLen = 384,
	 * NonceLen = 0, PersonalizationStringLen = 384, AdditionalInputLen = 384,
	 * ReturnedBitsLen = 512
	 */
	{  /* Count 0 */
		"0dd4d80062ecc0f359efbe7723020be9b88b550fe74088094069e74428395856f63eed4f5b0e7d1e006f0eaff74f638c",
		"d2aa2ccd4bc6537e51f6550ab6d6294547bef3e971a7f128e4436f957de9982c93ee22110b0e40ab33a7d3dfa22f599d",
		"0b081bab6c74d86b4a010e2ded99d14e0c9838f7c3d69afd64f1b66377d95cdcb7f6ec5358e3516034c3339ced7e1638",
		"ca818f938ae0c7f4f507e4cfec10e7baf51fe34b89a502f754d2d2be7395120fe1fb013c67ac2500b3d17b735da09a6e",
		"6808268b13e236f642c06deba2494496e7003c937ebf6f7cb7c92104ea090f18484aa075560d7844a06eb559948c93b26ae40f2db98ecb53ad593eb4c78f82b1",
	},

	{ /* Count 1 */
		"ca0ab9b22d0df4e680daa8dbab562c594bd079c394647af39dc1c616a6bd85c58f2d52a02f4b02435bbde80b33d405ed",
		"1695f83ec7f4f1742b7f13eb62cbbf17804965ed0acd3f0fba0cafc3cd55f306800339baf2567bb84fc37ca30ae2205d",
		"6f88faa9304a915b2b1988d4089bf0da10bf9f4dfa2fe3ccb1cd21202c06919c142d41324e51a5aefeefd05a664f701b",
		"135ead8f090344d1e967e720cbd6756d3b11e390cf2078bcfa5344944685a7590faa52242a207d0b9f33e2fc14c5a61e",
		"7f7e6f089722a06b741b385cdff7902f04f2e72dca7cbb64d6a4f373a9f693ec42fff76d11488ba86a3acc0395c02b7301249d02257da94e60f8ef9a3d844307",
	},

	/*
	 * AES-256 no df, PredictionResistance = False, EntropyInputLen = 384,
	 * NonceLen = 0, PersonalizationStringLen = 0, AdditionalInputLen = 0,
	 * ReturnedBitsLen = 512
	 */
	{  /* Count 0 */
		"fc7f2629c9d88672f81229bbcc0c7e75c4b7d8e5d9380702ea52dc495600a56e4ae5f0a5c25fb5d7e31f5aef4712bc19",
		0,
		0,
		0,
		"c6775c9c64137c018418c4f001d0e4d1f2dc4411d379a678f1d71eee0bdc28c66eacbd38f76be45bf992a709af14d146c35f91702d27a1f4176332ebd903fff9",
	},

	{ /* Count 1 */
		"c32cba5ee64291ed1d7dc51f8ad3ad40ae24c8fd2f78943593c8f0a0b5a5380798242ee88ceeba0d875b35a2d4fd8d19",
		0,
		0,
		0,
		"157366fc8777f6f0b76a12f7be0cf0599c7a2a399d54acba606426026e1c0a11b43801fa9f0f470648f7cd2f83d258c7ed7234ed3ee9c9f61d93da578a070a4b",
	},

	/*
	 * AES-256 no df, PredictionResistance = False, EntropyInputLen = 384,
	 * NonceLen = 0, PersonalizationStringLen = 0, AdditionalInputLen = 384,
	 * ReturnedBitsLen = 512
	 */
	{ /* Count 0 */
		"e9cde3f9ce0366deb161c40b1621641e583bb55114d912c40b7a7e7e2ea53e50665ca133a50e934cc1b4dbbf89c072e0",
		0,
		"be3dfbe5c9079f161b21b7a0781b607363a653350af68d82e21ce149c2fc3b98cac39e72751a61da13a0616e31118e96",
		"7e1627e98faa462ef6314d45a231b7f1a14a54ce8615500d144474a92d259fcc230ffb909853c168bd93531631de25d1",
		"2f265fbe1462014843a16548e172464060c41591b9adf1b76a79ae51c9a45223e6ef39a1eca2613ce9d682545d967d88b34808eb4d9a8e42cfed82eafc334d62",
	},

	{ /* Count 1 */
		"06ce9461e3cefcda7a9da3c0f040bac08257dd4135848a0aa052f6276a3e18f0ca8817070a63ccf2d9bfc1565c99fb10",
		0,
		"fdf895d506966621a01e96720f9e21b7eac2758f06872839366d97fae1cdfb4d01902128fe8c618e9c7274cb5c47ed54",
		"797d1d08cac4b4d10a999016c8b148c58b1dacca0a8e2111a7ee5ac2d1125620f60ee55af09b920e8716feef7c057285",
		"fd10da7096bb831e968a5dd4b551da387405c3fcb9a3298c626644b66c19fc6de8608c81a7f79971da390302b21f34c9cd3adeede871469a4aa557a4eda54e74",
	},

	/*
	 * AES-256 no df, PredictionResistance = False, EntropyInputLen = 384,
	 * NonceLen = 0, PersonalizationStringLen = 384, AdditionalInputLen = 0,
	 * ReturnedBitsLen = 512
	 */
	{  /* Count 0 */
		"d8078f99d5da1b312e4512acbccc198930453975d3d50fb5a13f25fdb11a5fed1a246e6bc153952a16623c233e13e241",
		"df1b8b21725ff886af4c647af1a587b1339e0973782e95c93f3b40bf421d5d03cec2b0b41f9058d730eb0fb53568d00a",
		0,
		0,
		"5edb9b25287f2b5b1e5fd81f69771ccab3b9876bf2ecdd644c9c6c5fe8dd988e0d7622dee902366449f2063e3f826df99743806b825fe8c85946af3b4781d6aa",
	},

	{ /* Count 1 */
		"e5b00f721bbbf081d350433592f6d2dab362217f0c0c49515d1f7f45999cf5acd52b0e816d102d60634a2461f4300103",
		"654a935d0c43affca9280ac152a34242fb6400d20836aabd13917719025c1e0d65a96af75614e05867d5194aa8e71c72",
		0,
		0,
		"464796a7728ebc3b14da925ac9073e2819b64930b9ce62954cf9a04c3b7dd2a3c35780a575d7b92e4023086aba0b4dcc267197dceb1481e43edf4cc030d545e5",
	},

	/*
	 * AES-128 no df, PredictionResistance = False, EntropyInputLen = 256,
	 * NonceLen = 0, PersonalizationStringLen = 256, AdditionalInputLen = 256,
	 * ReturnedBitsLen = 512
	 */
	{  /* Count 0 */
		"b91280bab68e2827d9e151a48e4b6a0812fe297dcb40c5fb91956f326cf998f5e6144886700541b0b6c26ad7a7aaacae",
		"244041d62b3ae7097190e1e43a40cbbb2d102ba204be6ba352e787b1ef508cd197486b5ea7cb17ecb000bdc976b3e20a",
		"015f53d7b4e64156469944566a219579d54b498d5e3fbe42001ddb133e1a9137b84f57dea8b915bc5fe4f66e8e71b13f",
		"ed1f74c1dbbb5b3094fe5d01f105d412d57e5cd17eebcf5cfc4379720fdd269cd93947ad1ecdc8d88a8d4d3ed387138e",
		"54bb9c7df65ed95dfff1178bdf098fca7b5559bfe8fe976c1c83864093583a1a181ef55a0d31711197b8235f8f799c3c1ec4a0194c7259f80ca1500821d2953f",
	},

	{ /* Count 1 */
		"5a0478e786d9472505196a46d2d8b9144b71ca6c4b9fffe1c1ea5fb7533f720d84c672e6511d53dfd37eee6114ca4172",
		"c37d595b7c286ae50ea538b8ab5f6de9147988103b24ce94615e153014afbf9b5e025cf8ec00f1307dcce00c6ed97d39",
		"bf19c0ee9a8be1a50f71dc209e327bb882f2c45ef1b7f9afdea950272d11223f3ebfc7716dcf4ca26d9af83338bb9f4e",
		"66f668a0ec0788e77277938883d946f871c8fbfdc8255799b734621e92cdac2a205bdc7d9e1800e5da4831e1d92d667e",
		"63c914b731706e6c61f86d0b69703f4821e1e4e9f9a978956818fe5f3b49a2a4e8170af0831d6867976c03a7d9a8d8d05f120e0c95a5bda7d505d83949dd1fa1",
	}
};

static unsigned int executePRNG_TestVector(PRNG_Vector vector, unsigned int idx)
{
	unsigned int result = TC_PASS;
	// uint8_t * entropy    = hexStringToBytes(vector.entropyString);
	uint8_t entropy[48];
	hexStringToBytes(vector.entropyString, entropy);
	show_str("entropy", entropy, 48);
	unsigned int  entropylen = strlen(vector.entropyString) / 2U;
	puthex(entropylen);
	puts("\n");

	// uint8_t * expected    = hexStringToBytes(vector.expectedString);
	uint8_t expected[64];
	hexStringToBytes(vector.expectedString, expected);
	show_str("expected", expected, 64);
	unsigned int  expectedlen = strlen(vector.expectedString) / 2U;
	puthex(expectedlen);
	puts("\n");


	// uint8_t * personalization   = 0;
	uint8_t personalization[48];
	unsigned int  plen              = 0U;

	// uint8_t * additional_input1 = 0;
	uint8_t additional_input1[48];
	unsigned int  additionallen1    = 0U;

	// uint8_t * additional_input2 = 0;
	uint8_t additional_input2[48];
	unsigned int  additionallen2    = 0U;

	// uint8_t * output = (uint8_t *)malloc(expectedlen);
	uint8_t output[64];

	unsigned int i;
	TCCtrPrng_t ctx;

	if (0 != vector.personalizationString) {
		// personalization = hexStringToBytes(vector.personalizationString);
		hexStringToBytes(vector.personalizationString, personalization);
		plen = strlen(vector.personalizationString) / 2U;
		show_str("personalization", personalization, plen);
	}

	if (0 != vector.additionalInputString1) {
		// additional_input1 = hexStringToBytes(vector.additionalInputString1);
		hexStringToBytes(vector.additionalInputString1, additional_input1);
		additionallen1 = strlen(vector.additionalInputString1) / 2U;
		show_str("additional_input1", additional_input1, additionallen1);
	}

	if (0 != vector.additionalInputString2) {
		// additional_input2 = hexStringToBytes(vector.additionalInputString2);
		hexStringToBytes(vector.additionalInputString2, additional_input2);
		additionallen2 = strlen(vector.additionalInputString2) / 2U;
		show_str("additional_input2", additional_input2, additionallen2);
	}

	(void)tc_ctr_prng_init(&ctx, entropy, entropylen, personalization, plen);

	show_str("ctx.V", ctx.V, TC_AES_BLOCK_SIZE);
	show_str("ctx.key", ctx.key.words, Nb*(Nr+1)*4);
	show_str("ctx.reseedCount", &ctx.reseedCount, 8);

	(void)tc_ctr_prng_generate(&ctx, additional_input1, additionallen1, output, expectedlen);
	show_str("output1", output, expectedlen);
	(void)tc_ctr_prng_generate(&ctx, additional_input2, additionallen2, output, expectedlen);
	show_str("output2", output, expectedlen);

	// for (i = 0U; i < expectedlen; i++) {
	// 	puts("Comparing test vector ");
	// 	puthex(i);
	// 	puts(":\n");
	// 	if (output[i] != expected[i]) {
	// 		puts("FAIL");
	// 		result = TC_FAIL;
	// 		break;
	// 	}
	// }
	result = check_result(idx, expected, expectedlen, output, expectedlen);

	// free(entropy);
	// free(expected);
	// free(personalization);
	// free(additional_input1);
	// free(additional_input2);
	// free(output);

	return result;
}

static int test_reseed(void)
{
	int result = TC_PASS;
	uint8_t entropy[48U] = {0U}; /* value not important */
	uint8_t additional_input[48] = {0U};
	uint8_t output[64];
	TCCtrPrng_t ctx;
	int ret;
	unsigned int i;

	(void)tc_ctr_prng_init(&ctx, entropy, sizeof entropy, 0, 0U);

	/* force internal state to max allowed count */
	ctx.reseedCount = 0x1000000000000ULL;

	ret = tc_ctr_prng_generate(&ctx, 0, 0, output, sizeof output);
	if (1 != ret) {
		result = TC_FAIL;
		goto exitTest;
	}

	/* expect further attempts to fail due to reaching reseed threshold */
	ret = tc_ctr_prng_generate(&ctx, 0, 0, output, sizeof output);
	if (-1 != ret) {
		result = TC_FAIL;
		goto exitTest;
	}

	/* reseed and confirm generate works again */
	/* make entropy different from original value - not really important for the purpose of this test */
	memset(entropy, 0xFF, sizeof entropy);
	ret = tc_ctr_prng_reseed(&ctx, entropy, sizeof entropy, additional_input, sizeof additional_input);
	if (1 != ret) {
		result = TC_FAIL;
		goto exitTest;
	}

	ret = tc_ctr_prng_generate(&ctx, 0, 0, output, sizeof output);
	if (1 != ret) {
		result = TC_FAIL;
		goto exitTest;
	}

	/* confirm entropy and additional_input are being used correctly */
	/* first, entropy only */
	// memset(&ctx, 0x0, sizeof ctx);
	// for (i = 0U; i < sizeof entropy; i++) {
	// 	entropy[i] = i;
	// }
	// ret = tc_ctr_prng_reseed(&ctx, entropy, sizeof entropy, 0, 0U);
	// if (1 != ret) {
	// 	result = TC_FAIL;
	// 	goto exitTest;
	// }
	// {
	// 	uint8_t expectedV[] =
	// 		{0x7EU, 0xE3U, 0xA0U, 0xCBU, 0x6DU, 0x5CU, 0x4BU, 0xC2U,
	// 		 0x4BU, 0x7EU, 0x3CU, 0x48U, 0x88U, 0xC3U, 0x69U, 0x70U};
	// 	for (i = 0U; i < sizeof expectedV; i++) {
	// 		if (ctx.V[i] != expectedV[i]) {
	// 			result = TC_FAIL;
	// 			goto exitTest;
	// 		}
	// 	}
	// }

	/* now, entropy and additional_input */
	// memset(&ctx, 0x0, sizeof ctx);
	// for (i = 0U; i < sizeof additional_input; i++) {
	// 	additional_input[i] = i * 2U;
	// }
	// ret = tc_ctr_prng_reseed(&ctx, entropy, sizeof entropy, additional_input, sizeof additional_input);
	// if (1 != ret) {
	// 	result = TC_FAIL;
	// 	goto exitTest;
	// }
	// {
	// 	uint8_t expectedV[] =
	// 		{0x5EU, 0xC1U, 0x84U, 0xEDU, 0x45U, 0x76U, 0x67U, 0xECU,
	// 		 0x7BU, 0x4CU, 0x08U, 0x7EU, 0xB0U, 0xF9U, 0x55U, 0x4EU};
	// 	for (i = 0U; i < sizeof expectedV; i++) {
	// 		if (ctx.V[i] != expectedV[i]) {
	// 			result = TC_FAIL;
	// 			goto exitTest;
	// 		}
	// 	}
	// }

	exitTest:
	if (TC_FAIL == result) {
		puts("CTR PRNG reseed tests failed\n");
	}
	return result;
}

static int test_uninstantiate(void)
{
	unsigned int i;
	int result = TC_PASS;
	uint8_t entropy[32U] = {0U}; /* value not important */
	TCCtrPrng_t ctx;

	(void)tc_ctr_prng_init(&ctx, entropy, sizeof entropy, 0, 0U);

	tc_ctr_prng_uninstantiate(&ctx);
	/* show that state has been zeroised */
	for (i = 0U; i < sizeof ctx.V; i++) {
		if (0U != ctx.V[i]) {
			puts("CTR PRNG uninstantiate tests failed\n");
			result = TC_FAIL;
			break;
		}
	}

	for (i = 0U; i < sizeof ctx.key.words / sizeof ctx.key.words[0]; i++) {
		if (0U != ctx.key.words[i]) {
			puts("CTR PRNG uninstantiate tests failed\n");
			result = TC_FAIL;
			break;
		}
	}

	if (0U != ctx.reseedCount) {
		puts("CTR PRNG uninstantiate tests failed\n");
		result = TC_FAIL;
	}

	return result;
}

static int test_robustness(void)
{
	int result = TC_PASS;
	int ret;
	uint8_t entropy[32U] = {0U}; /* value not important */
	uint8_t output[32];
	TCCtrPrng_t ctx;


	/* show that the CTR PRNG is robust to invalid inputs */
	tc_ctr_prng_uninstantiate(0);

	ret = tc_ctr_prng_generate(&ctx, 0, 0, 0, 0);
	if (0 != ret) {
		result = TC_FAIL;
		goto exitTest;
	}

	ret = tc_ctr_prng_generate(0, 0, 0, output, sizeof output);
	if (0 != ret) {
		result = TC_FAIL;
		goto exitTest;
	}

	ret = tc_ctr_prng_generate(0, 0, 0, 0, 0);
	if (0 != ret) {
		result = TC_FAIL;
		goto exitTest;
	}

	ret = tc_ctr_prng_reseed(&ctx, 0, 0, 0, 0);
	if (0 != ret) {
		result = TC_FAIL;
		goto exitTest;
	}

	/* too little entropy */
	ret = tc_ctr_prng_reseed(&ctx, entropy, (sizeof entropy) - 1U, 0, 0);
	if (0 != ret) {
		result = TC_FAIL;
		goto exitTest;
	}

	ret = tc_ctr_prng_reseed(0, entropy, sizeof entropy, 0, 0);
	if (0 != ret) {
		result = TC_FAIL;
		goto exitTest;
	}

	ret = tc_ctr_prng_reseed(0, 0, 0, 0, 0);
	if (0 != ret) {
		result = TC_FAIL;
		goto exitTest;
	}

	ret = tc_ctr_prng_init(&ctx, 0, 0, 0, 0);
	if (0 != ret) {
		result = TC_FAIL;
		goto exitTest;
	}

	/* too little entropy */
	ret = tc_ctr_prng_init(&ctx, entropy, (sizeof entropy) - 1U, 0, 0);
	if (0 != ret) {
		result = TC_FAIL;
		goto exitTest;
	}

	ret = tc_ctr_prng_init(0, entropy, sizeof entropy, 0, 0);
	if (0 != ret) {
		result = TC_FAIL;
		goto exitTest;
	}

	ret = tc_ctr_prng_init(0, 0, 0, 0, 0);
	if (0 != ret) {
		result = TC_FAIL;
		goto exitTest;
	}

	exitTest:
	if (TC_FAIL == result) {
		puts("CTR PRNG reseed tests failed\n");
	}


	return result;
}

/*
 * Main task to test CTR PRNG
 */
int main(void)
{
	int result = TC_PASS;
	unsigned int i;

	puts("Performing AES-256 CTR-PRNG tests:\n");

	for (i = 0U; i < sizeof vectors / sizeof vectors[0]; i++) {
		result = executePRNG_TestVector(vectors[i], i);
		puts("Comparing test vector ");
		puthex(i);
		puts(":\n");
		if (TC_PASS != result) {
			goto exitTest;
		}
    	puts("PASS\n");
	}

	if (TC_PASS != test_reseed()) {
		puts("test_reseed() failed\n");
		goto exitTest;
	}
	puts("test_reseed() passed\n");

	if (TC_PASS != test_uninstantiate()) {
		puts("test_uninstantiate() failed\n");
		goto exitTest;
	}
	puts("test_uninstantiate() passed\n");

	if (TC_PASS != test_robustness()) {
		puts("test_robustness() failed\n");
		goto exitTest;
	}
	puts("test_robustness() passed\n");

	puts("All AES-256 CTR PRNG tests succeeded!\n");

    return 0;

	exitTest:
    	puts("FAIL\n");

    return 0;

}
