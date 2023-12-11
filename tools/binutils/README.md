Modified `binutils` used for the Xascon extension. [riscv-opcodes](https://github.com/riscv/riscv-opcodes) was used to generate the riscv-opc.h constants using the following description:
 ```
 asconsigma0h     rd rs1 rs2 31..30=1 29..25=0 14..12=7 6..0=0x2B
asconsigma1h     rd rs1 rs2 31..30=1 29..25=1 14..12=7 6..0=0x2B
asconsigma2h     rd rs1 rs2 31..30=1 29..25=2 14..12=7 6..0=0x2B
asconsigma3h     rd rs1 rs2 31..30=1 29..25=3 14..12=7 6..0=0x2B
asconsigma4h     rd rs1 rs2 31..30=1 29..25=4 14..12=7 6..0=0x2B
asconsigma0l     rd rs1 rs2 31..30=0 29..25=0 14..12=7 6..0=0x2B
asconsigma1l     rd rs1 rs2 31..30=0 29..25=1 14..12=7 6..0=0x2B
asconsigma2l     rd rs1 rs2 31..30=0 29..25=2 14..12=7 6..0=0x2B
asconsigma3l     rd rs1 rs2 31..30=0 29..25=3 14..12=7 6..0=0x2B
asconsigma4l     rd rs1 rs2 31..30=0 29..25=4 14..12=7 6..0=0x2B
```

We use the -B flag in gcc to compile code with this version of binutils.


