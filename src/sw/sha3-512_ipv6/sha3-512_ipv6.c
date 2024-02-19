#include <test_utils.h>
#include "sha3_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

unsigned int test_1(void)
{
        unsigned int result = TC_PASS;

        puts("SHA3-512 test #1: \n");
        char m[1224];
        uint8_t digest[64];

        for (int i = 0; i < 1224; i++)
            m[i] = i;

        sha3(digest, 64, m, 1224);

        rvkat_hexout("m", m, 1224);
        rvkat_hexout("digest", digest, 64);

        return result;

}

int main(void) {
        unsigned int result = TC_PASS;

        pcount_enable(0);
        pcount_reset();
        pcount_enable(1);

        data_ind_timing_enable(1);

        puts("Performing SHA3-512 tests (IPv6 packet max payload size of 1224 bytes):\n\n");
        result = test_1();
        if (result == TC_FAIL) {
		/* terminate test */
                puts("SHA3-512 test #1 failed.\n");
                goto exitTest;
        }

        puts("All SHA3-512 tests succeeded!\n");

exitTest:
        return 0;
}

