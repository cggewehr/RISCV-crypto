#include <tinycrypt/sha256.h>
#include <tinycrypt/constants.h>
#include <test_utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

unsigned int test_1(void)
{
        unsigned int result = TC_PASS;

        puts("Ascon-Hash test #1: ");
        char m[1224];
        uint8_t digest[32];

        for (int i = 0; i < 1224; i++)
            m[i] = i;

        // sha256(digest, m, 86);
        result = ascon_hash(m, 1224, digest, 32, 0);

        rvkat_hexout("m", m, 1224);
        rvkat_hexout("digest", digest, 32);

        return result;

}

int main(void) {
        unsigned int result = TC_PASS;

        pcount_enable(0);
        pcount_reset();
        pcount_enable(1);

        data_ind_timing_enable(1);

        // puts("Performing SHA256 tests (NIST tests vectors):\n\n");
        result = test_1();
        if (result == TC_FAIL) {
		/* terminate test */
                puts("Ascon-Hash test #1 failed.\n");
                goto exitTest;
        }

        puts("All Ascon-Hash tests succeeded!\n");

exitTest:
        return 0;
}
