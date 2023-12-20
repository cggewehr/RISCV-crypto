/*
Implementation by the Keccak Team, namely, Guido Bertoni, Joan Daemen,
Michaël Peeters, Gilles Van Assche and Ronny Van Keer,
hereby denoted as "the implementer".

For more information, feedback or questions, please refer to our website:
https://keccak.team/

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "test_rvkat.h"

#define KYBER_Q 3329

int test_cbd();
int test_arith();
int test_compress();

int test_cbd() {

    int fail_count_cbd2 = 0;
    int fail_count_cbd3 = 0;

    // Run through all possible inputs to "kybercbd2" instruction
    // for (int i = 0; i < (1 << 8) - 1; i++) {
    for (int i = 0; i < (1 << 8) - 1; i+=10) {

        puts("kybercbd2 i = ");
        puthex(i);
        puts("\n");

        int asm_val, asm_coeff_low, asm_coeff_high;
        __asm__("kybercbd2 %0, %1" :  "=r"(asm_val) : "r"(i));

        asm_coeff_low = asm_val & 0x0000FFFF;
        asm_coeff_high = (asm_val & 0xFFFF0000) >> 16;

        int ref_coeff_low, ref_coeff_high;

        ref_coeff_low = ((i & 1) + ((i & 2) >> 1)) - (((i & 4) >> 2) + ((i & 8) >> 3));
        ref_coeff_low = (ref_coeff_low < 0) ? ref_coeff_low + KYBER_Q : ref_coeff_low;
        // ref_coeff_low = ref_coeff_low % KYBER_Q;

        ref_coeff_high = (((i & 16) >> 4) + ((i & 32) >> 5)) - (((i & 64) >> 6) + ((i & 128) >> 7));
        ref_coeff_high = (ref_coeff_high < 0) ? ref_coeff_high + KYBER_Q : ref_coeff_high;
        // ref_coeff_high = ref_coeff_high % KYBER_Q;

        fail_count_cbd2 += rvkat_chku32("kybercbd2_coeff_low ", ref_coeff_low, asm_coeff_low);
        fail_count_cbd2 += rvkat_chku32("kybercbd2_coeff_high", ref_coeff_high, asm_coeff_high);

    }

    // Run through all possible inputs to "kybercbd3" instruction
    // for (int i = 0; i < (1 << 12) - 1; i++) {
    for (int i = 0; i < (1 << 12) - 1; i+=100) {

        puts("kybercbd3 i = ");
        puthex(i);
        puts("\n");

        int asm_val, asm_coeff_low, asm_coeff_high;
        __asm__("kybercbd3 %0, %1" :  "=r"(asm_val) : "r"(i));

        asm_coeff_low = asm_val & 0x0000FFFF;
        asm_coeff_high = (asm_val & 0xFFFF0000) >> 16;

        int ref_coeff_low, ref_coeff_high;

        ref_coeff_low = ((i & 1) + ((i & 2) >> 1) + ((i & 4) >> 2)) - (((i & 8) >> 3) + ((i & 16) >> 4) + ((i & 32) >> 5));
        ref_coeff_low = (ref_coeff_low < 0) ? ref_coeff_low + KYBER_Q : ref_coeff_low;
        // ref_coeff_low = ref_coeff_low % KYBER_Q;

        ref_coeff_high = (((i & 64) >> 6) + ((i & 128) >> 7) + ((i & 256) >> 8)) - (((i & 512) >> 9) + ((i & 1024) >> 10) + ((i & 2048) >> 11));
        ref_coeff_high = (ref_coeff_high < 0) ? ref_coeff_high + KYBER_Q : ref_coeff_high;
        // ref_coeff_high = ref_coeff_high % KYBER_Q;

        fail_count_cbd3 += rvkat_chku32("kybercbd3_coeff_low ", ref_coeff_low, asm_coeff_low);
        fail_count_cbd3 += rvkat_chku32("kybercbd3_coeff_high", ref_coeff_high, asm_coeff_high);

    }

    puts("\n");
    puts("test_arith() cbd2 fail count: ");
    puthex(fail_count_cbd2);

    puts("\n");
    puts("test_arith() cbd3 fail count: ");
    puthex(fail_count_cbd3);

    puts("\n");
    puts("\n");

    return fail_count_cbd2 + fail_count_cbd3;

}

int test_arith() {

    int fail_count_add = 0;
    int fail_count_sub = 0;
    int fail_count_mul = 0;

    // Run through all possible inputs to "kyberadd"
    // for (int coeff_1_low = 1; coeff_1_low < KYBER_Q; coeff_1_low = coeff_1_low << 1)
    //     for (int coeff_2_low = 1; coeff_2_low < KYBER_Q; coeff_2_low  = coeff_2_low << 1)
    //         for (int coeff_1_high = 1; coeff_1_high < KYBER_Q; coeff_1_high = coeff_1_high << 1)
    //             for (int coeff_2_high = 1; coeff_2_high < KYBER_Q; coeff_2_high = coeff_2_high << 1) {

    //                 puts("kyberadd coeff_1_low = ");
    //                 puthex(coeff_1_low);
    //                 puts(" | ");
    //                 puts("kyberadd coeff_1_high = ");
    //                 puthex(coeff_1_high);
    //                 puts(" | ");
    //                 puts("kyberadd coeff_2_low = ");
    //                 puthex(coeff_2_low);
    //                 puts(" | ");
    //                 puts("kyberadd coeff_2_high = ");
    //                 puthex(coeff_2_high);
    //                 puts("\n");

    //                 int asm_val, asm_coeff_high, asm_coeff_low;

    //                 __asm__("kyberadd %0, %1, %2" :  "=r"(asm_val) : "r"(coeff_1_low | (coeff_1_high << 16)), "r"(coeff_2_low | (coeff_2_high << 16)));
    //                 asm_coeff_low = (asm_val & 0x0000FFFF);
    //                 asm_coeff_high = (asm_val >> 16);

    //                 int ref_coeff_low = (coeff_1_low + coeff_2_low) % KYBER_Q;
    //                 int ref_coeff_high = (coeff_1_high + coeff_2_high) % KYBER_Q;

    //                 fail_count_add += rvkat_chku32("kyberadd_coeff_low", ref_coeff_low, asm_coeff_low);
    //                 fail_count_add += rvkat_chku32("kyberadd_coeff_high", ref_coeff_high, asm_coeff_high);

    //             }

    // Quick "kyberadd" test
    for (int coeff = 1; coeff < KYBER_Q; coeff+= 100) {

        puts("kyberadd coeff_1_low = ");
        puthex(coeff);
        puts(" | ");
        puts("kyberadd coeff_1_high = ");
        puthex(coeff+1);
        puts(" | ");
        puts("kyberadd coeff_2_low = ");
        puthex(coeff);
        puts(" | ");
        puts("kyberadd coeff_2_high = ");
        puthex(coeff+1);
        puts("\n");

        int asm_val, asm_coeff_high, asm_coeff_low;

        __asm__("kyberadd %0, %1, %2" :  "=r"(asm_val) : "r"(coeff | (coeff << 16)), "r"(coeff | ((coeff+1) << 16)));
        asm_coeff_low = (asm_val & 0x0000FFFF);
        asm_coeff_high = (asm_val >> 16);

        int ref_coeff_low = (coeff + coeff) % KYBER_Q;
        int ref_coeff_high = (coeff + coeff + 1) % KYBER_Q;

        fail_count_add += rvkat_chku32("kyberadd_coeff_low", ref_coeff_low, asm_coeff_low);
        fail_count_add += rvkat_chku32("kyberadd_coeff_high", ref_coeff_high, asm_coeff_high);

    }

    // Quick "kybersub" test
    for (int coeff = 1; coeff < KYBER_Q; coeff+= 100) {

        puts("kybersub coeff_1_low = ");
        puthex(1500);
        puts(" | ");
        puts("kybersub coeff_1_high = ");
        puthex(1500);
        puts(" | ");
        puts("kybersub coeff_2_low = ");
        puthex(coeff);
        puts(" | ");
        puts("kybersub coeff_2_high = ");
        puthex(coeff+1);
        puts("\n");

        int asm_val, asm_coeff_high, asm_coeff_low;

        __asm__("kybersub %0, %1, %2" :  "=r"(asm_val) : "r"(1500 | (1500 << 16)), "r"(coeff | ((coeff+1) << 16)));
        asm_coeff_low = (asm_val & 0x0000FFFF);
        asm_coeff_high = (asm_val >> 16);

        // int ref_coeff_low = (1500 - coeff) % KYBER_Q;
        int ref_coeff_low = (1500 - coeff);
        ref_coeff_low = (ref_coeff_low < 0) ? ref_coeff_low + KYBER_Q : ref_coeff_low;
        // int ref_coeff_high = (1500 - (coeff+1)) % KYBER_Q;
        int ref_coeff_high = (1500 - (coeff+1));
        ref_coeff_high = (ref_coeff_high < 0) ? ref_coeff_high + KYBER_Q : ref_coeff_high;

        fail_count_sub += rvkat_chku32("kybersub_coeff_low ", ref_coeff_low, asm_coeff_low);
        fail_count_sub += rvkat_chku32("kybersub_coeff_high", ref_coeff_high, asm_coeff_high);

    }

    // Quick "kybermul" test
    for (int coeff = 1; coeff < KYBER_Q; coeff+= 100) {

        puts("kybermul coeff_1 = ");
        puthex(coeff);
        puts(" | ");
        puts("kybermul coeff_2 = ");
        puthex(coeff);
        puts("\n");

        int asm_val;
        __asm__("kybermul %0, %1, %2" :  "=r"(asm_val) : "r"(coeff), "r"(coeff));

        int ref_val = (coeff * coeff) % KYBER_Q;

        fail_count_mul += rvkat_chku32("kybermul_coeff", ref_val, asm_val);

    }

    puts("\n");
    puts("test_arith() add fail count: ");
    puthex(fail_count_add);

    puts("\n");
    puts("test_arith() sub fail count: ");
    puthex(fail_count_sub);

    puts("\n");
    puts("test_arith() mul fail count: ");
    puthex(fail_count_mul);

    puts("\n");
    puts("\n");

    return fail_count_add + fail_count_sub + fail_count_mul;

}

int test_compress() {

    int fail_count = 0;
    int compress_d[5] = {1, 4, 5, 10, 11};

    for (int i = 0; i < 5; i++)
        for (int coeff = 0; coeff < KYBER_Q; coeff+=100) {
        // for (int coeff = 0; coeff < KYBER_Q; coeff++) {

            puts("kybercompress d = ");
            puthex(compress_d[i]);
            puts(" | ");
            puts("kybermul coeff = ");
            puthex(coeff);
            puts("\n");

            int asm_val;
            // __asm__("kybercompress %0, %1" :  "=r"(asm_val) : "r"(coeff), "i"(compress_d[i]));
            __asm__("kybercompress %0, %1, %2" :  "=r"(asm_val) : "r"(coeff), "r"(compress_d[i]));

            int ref_val = (((coeff << compress_d[i]) + KYBER_Q/2)/KYBER_Q) & ((1 << compress_d[i]) - 1);

            fail_count += rvkat_chku32("kybercompress_coeff", ref_val, asm_val);

        }

    return fail_count;

}

int main() {

    // data_ind_timing_enable(1);

    int test_cbd_fail_count, test_arith_fail_count, test_compress_fail_count;

    test_cbd_fail_count = test_cbd();
    test_arith_fail_count = test_arith();
    test_compress_fail_count = test_compress();

    puts("\n");
    puts("test_cbd() fail count: ");
    puthex(test_cbd_fail_count);

    puts("\n");
    puts("test_arith() fail count: ");
    puthex(test_arith_fail_count);

    puts("\n");
    puts("test_compress() fail count: ");
    puthex(test_compress_fail_count);

    return test_cbd_fail_count + test_arith_fail_count + test_compress_fail_count;

}