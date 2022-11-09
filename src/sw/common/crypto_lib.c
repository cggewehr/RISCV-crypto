// CHECK FOR OVERFLOW
// add t0, t1, t2
// slti t3, t2, 0
// slt t4, t0, t1
// bne t3, t4, overflow

// SHA256 initial values
const unsigned int h256[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

// SHA256 round constants
const unsigned int k256[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5
                               0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174
                               0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da
                               0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967
                               0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85
                               0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070
                               0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3
                               0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
                      
// SHA512 initial values
const unsigned long long h512[8] = {0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
                                    0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};

// SHA512 round constants
const unsigned long long k512[80] = {0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
                                     0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
                                     0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
                                     0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
                                     0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
                                     0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
                                     0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
                                     0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
                                     0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
                                     0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
                                     0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
                                     0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
                                     0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
                                     0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
                                     0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
                                     0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
                                     0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
                                     0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
                                     0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
                                     0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817};
               
// Relevant words of message schedule for current "i", statically allocated mem space, shared between SHA256, SHA512 (TODO: possibliy AES)
unsigned int sha2_workspace[32];

// TODO: Maybe a* registers should be saved (and restored in sha256_restore())
void sha256_init(void* state) {
// a0: Pointer to running state (should point to h256 if this is the first message block to be hashed)
// t2: Saves SP, restores at the end (sha2_restore())

    asm (

        // Save S0~S7 to stack
        // TODO: Save a*
        c.addi sp, -8
        c.swsp s0, 0
        c.swsp s1, 1
        c.swsp s2, 2
        c.swsp s3, 3
        c.swsp s4, 4
        c.swsp s5, 5
        c.swsp s6, 6
        c.swsp s7, 7

        // Init S0~S7 from given "state" array (register a0)
        c.mv t2, sp
        c.mv sp, a0
        c.lwsp s0, 0
        c.lwsp s1, 1
        c.lwsp s2, 2
        c.lwsp s3, 3
        c.lwsp s4, 4
        c.lwsp s5, 5
        c.lwsp s6, 6
        c.lwsp s7, 7

    )
}

void sha256_compress(char* state, char* data, int storeStateFlag) {

// REG TABLE
// a0: Pointer to running state
// a1: Pointer to message block
// a2: throwaway temp
// a3: Data from compressed load/store (Used for compressed load/store)
// a4: Pointer to current message schedule word (stored in sha2_workspace) (Used for compressed load/store) 
// a5: throwaway temp

// s0: A state variable
// s1: B state variable
// s2: C state variable
// s3: D state variable
// s4: E state variable
// s5: F state variable
// s6: G state variable
// s7: H state variable

// t0: i counter
// t1: workspace index
// t2: Saves SP, restores at the end (saved in sha256_init(), restored in sha2_restore())
// t3: storeStateFlag argument                                               TODO: MOVE a2 to t3
// t4: workspace index mask (0xF)
// t5: throwaway temp
// t6: throwaway temp

// C.ANDI TO DETERMINE WORKSPACE POINTER

    asm (

        // Init t0 to round counter (0 < i < 64) and t1 to workspace index (0 < i < 16)
        c.mv t0, x0
        c.mv t1, x0
        c.mv t3, a2
        c.li t4, 16

        // Set SP as addr of constants array
        la sp, sha2_workspace
        
        sha256_compress_iter_top:
            
            // Choose if load W[i] from data[] (i < 16) or compute new W (i >= 16)
            bge t0, t4, sha256_compress_compute_new_W
            
            // Fall through to load W[i] from data[], i < 16
            sha256_compress_load_W_from_workspace:
                
                // a3 <= data[i]
                c.lw a3, a1, 0
                
                // TODO: Check if BigEndian conversion is necessary

                // W[i] <= data[i]
                c.sw a3, a4, 0
                
                // Increment pointer to data[i]
                c.addi a1, 1
        
                // Jump to "sha256_compress_compute_state"
                c.j sha256_compress_compute_state

            // Compute new W, i >= 16
            sha256_compress_compute_new_W:

                // Get a2 = W[i+1 & 0xF] (equivalent to W[i-15])
                c.mv t1, a5
                c.addi a5, 1
                c.add a5, a4
                c.andi a5, 0xF
                c.lw a2, a5

                // Compute Sigma0(a2)   W[i+1]
                sha256sig0 a2, a2
                
                // Get a3 = W[i+14 & 0xF] (equivalent to W[i-2])
                c.mv t1, a5
                c.addi a5, 14
                c.add a5, a4
                c.andi a5, 0xF
                c.lw a3, a5
                
                // Compute Sigma1(a3)   W[i+14]
                sha256sig1 a3, a3
                
                // a2 = a2 + a3 (a2 now contains Sig0(W[i-2]) + Sig1(W[i-14]))
                c.add a2, a3
                
                // Get a5 = W[i] (will be replaced in this iteration)  (equivalent to W[i-16])
                c.lw a3, a5
                
                // a2 = a2 + a3  (a2 now contains Sig0(W[i-2]) + Sig1(W[i-14] + W[i-16]))
                c.add a2, a3

                // Get a3 = W[i+9 & 0xF]  (equivalent to W[i-7])
                c.mv t1, a5
                c.addi a5, 14
                c.add a5, a4
                c.andi a5, 0xF
                c.lw a3, a5
                
                // a2 = a2 + a3  (a2 now contains Sig0(W[i-2]) + Sig1(W[i-14] + W[i-16] + W[i-14], new W[i] is finished being computed))
                c.add a2, a3

                // W[i] = a2 (Saves newly computed schedule word to message schedule)
                c.sw a2, a4, 0

                // Increment workspace pointer
                c.addi, t1, 1
                c.mv a5, t1
                c.andi a5, 0xF
                c.add a4, a5
                c.mv t1, a5
            
        sha256_compress_compute_state:    
            
        // a3 now contains W[i] (such that this is reused by both branches)
        
        // TODO: Compute new A~H state variables
        
        // TODO: Shift variables using c.mov
        
        // TODO: Increment counters and loop
        
        c.addi t0, 1
        
        // TODO: Branch back to top_iter if i < 64
        // sltu ...
        ///
        
        c.mv a2, t3

    )
}

// TODO: Save state to mem and restore S0~S7 from stack
// void sha256_restore()
