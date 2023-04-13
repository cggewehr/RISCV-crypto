# RISCV-crypto
Ibex RISC-V core extended with hardware acceleration of cryptography functions 

1° Run MakeSImArea.py Passing a name for you build environmet, this is helpfull to split each setup and results in diferent folders
    python3 MakeSimArea.py -d AESTest

2° Do module load for:
    - module load riscv64-elf/12.2.0
    - module load xcelium/1903
    - module load genus/191

3° Do make sw PROG=hello_test (or pass the algritmh to be compiled eg. tc_sha512_test)
    - make sw PROG=tc_sha256_test

4° Do make all
    - make all RISCV_CORE=ibex-crypto (to call an specific riscv version)

