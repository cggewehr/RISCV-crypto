# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0
#
# Generate a baremetal application

# Name of the program $(PROGRAM).c will be added as a source file
PROG = ascon128v12_ISE
PROGRAM_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
# Any extra source files to include in the build. Use the upper case .S
# extension for assembly files
EXTRA_SRCS := ascon.S

PROGRAM_CFLAGS = -B/home/lucas.luza/riscv/bin/riscv64-unknown-linux-gnu-
OBJDUMP = /home/lucas.luza/riscv/bin/riscv64-unknown-linux-gnu-objdump
#OBJCOPY = /home/lucas.luza/riscv/bin/riscv64-unknown-linux-gnu-objcopy

include ${PROGRAM_DIR}/../common/common.mk
