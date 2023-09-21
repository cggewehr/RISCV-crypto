# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0
#
# Generate a baremetal application

# Name of the program $(PROGRAM).c will be added as a source file
PROG = ascon128_scenario_1c_ref
PROGRAM_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
# Any extra source files to include in the build. Use the upper case .S
# extension for assembly files
#EXTRA_SRCS := ascon.S

# These point to a custom binutils to support the following implemented instructions:
# asconsigma0h     rd rs1 rs2 31..30=1 29..25=0 14..12=7 6..0=0x2B
# asconsigma1h     rd rs1 rs2 31..30=1 29..25=1 14..12=7 6..0=0x2B
# asconsigma2h     rd rs1 rs2 31..30=1 29..25=2 14..12=7 6..0=0x2B
# asconsigma3h     rd rs1 rs2 31..30=1 29..25=3 14..12=7 6..0=0x2B
# asconsigma4h     rd rs1 rs2 31..30=1 29..25=4 14..12=7 6..0=0x2B
# asconsigma0l     rd rs1 rs2 31..30=0 29..25=0 14..12=7 6..0=0x2B
# asconsigma1l     rd rs1 rs2 31..30=0 29..25=1 14..12=7 6..0=0x2B
# asconsigma2l     rd rs1 rs2 31..30=0 29..25=2 14..12=7 6..0=0x2B
# asconsigma3l     rd rs1 rs2 31..30=0 29..25=3 14..12=7 6..0=0x2B
# asconsigma4l     rd rs1 rs2 31..30=0 29..25=4 14..12=7 6..0=0x2B

#PROGRAM_CFLAGS = -B/home/${USER}/riscv/bin/riscv64-unknown-linux-gnu-
#OBJDUMP = /home/${USER}/riscv/bin/riscv64-unknown-linux-gnu-objdump
#OBJCOPY = /home/${USER}/riscv/bin/riscv64-unknown-linux-gnu-objcopy

include ${PROGRAM_DIR}/../common/common.mk
