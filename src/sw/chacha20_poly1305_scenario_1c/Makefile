# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0
#
# Generate a baremetal application

# Name of the program $(PROGRAM).c will be added as a source file
PROG = chacha20_poly1305_scenario_1c
PROGRAM_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
# Any extra source files to include in the build. Use the upper case .S
# extension for assembly files
EXTRA_SRCS :=

LIBSODIUM_INSTALL_DIR ?= /home/$(shell whoami)/libsodium-install
PROGRAM_LDFLAGS = -L$(LIBSODIUM_INSTALL_DIR)/lib -lsodium
PROGRAM_CFLAGS = -I$(LIBSODIUM_INSTALL_DIR)/include 

include ${PROGRAM_DIR}/../common/common.mk
