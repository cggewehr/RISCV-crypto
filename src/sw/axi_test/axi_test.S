.section .text
.align 4

.globl main
.type main,@function
main:
    la t1, test
    li t2, 0x88778877
    li t3, 0x44332211
    li t4, 0x11223344

    sh t2, (t1)
    sh t4, 2(t1)
    lw t0, test
    sw t3, 2(t1)
    lw t0, test
    lw t0, test2
    sh t2, 3(t1)
    lh t0, 3(t1)
    lbu t0, 4(t1)

    lh t0, 0(t1)
    lh t0, 1(t1)
    nop
    lh t0, 2(t1)
    nop
    nop
    lh t0, 3(t1)
    nop
    nop
    nop
    lh t0, 4(t1)

    lbu t0, 0(t1)
    lbu t0, 1(t1)
    nop
    lbu t0, 2(t1)
    nop
    nop
    lbu t0, 3(t1)
    nop
    nop
    nop
    lbu t0, 4(t1)
    ret


.section .data
.align 4
test:
    .word 0x90abcdef
test2:
    .word 0x12345678
