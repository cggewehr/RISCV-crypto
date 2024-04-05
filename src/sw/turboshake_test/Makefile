# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0
#
# Generate a baremetal application

# Name of the program $(PROGRAM).c will be added as a source file
PROGRAM = turboshake_test
PROGRAM_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
# Any extra source files to include in the build. Use the upper case .S
# extension for assembly files
EXTRA_SRCS :=
# PROGRAM_CFLAGS := -DRVK_ALGTEST_VERBOSE_SIO -DRVKINTRIN_EMULATE
PROGRAM_CFLAGS := -DRVK_ALGTEST_VERBOSE_SIO -DRVKINTRIN_ASSEMBLER -DTURBOSHAKE

include ${PROGRAM_DIR}/../common/common.mk
