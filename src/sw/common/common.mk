# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

# Now is set within individual makefiles from each sim area
# COMMON_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

COMMON_SRCS = $(wildcard $(COMMON_DIR)/*.c)
COMMON_OBJS = ${COMMON_SRCS:.c=.o}
COMMON_OBJS := $(patsubst $(COMMON_DIR)%, $(SW_BUILD_PATH)%, ${COMMON_OBJS})
INCS := -I$(COMMON_DIR)

# ARCH = rv32im # to disable compressed instructions
#ARCH ?= rv32imc
ARCH ?= rv32imc_zicsr

ifdef PROGRAM
#	PROGRAM := $(addprefix ${SW_SRC_PATH}/, ${PROGRAM})
	PROGRAM_C := $(PROGRAM).c
endif

#SRCS = $(COMMON_SRCS) $(PROGRAM_C) $(EXTRA_SRCS)
SRCS = $(PROGRAM_C) $(EXTRA_SRCS)

C_SRCS = $(filter %.c, $(SRCS))
ASM_SRCS = $(filter %.S, $(SRCS))

#CC = riscv32-unknown-elf-gcc
CC = riscv64-elf-gcc

CROSS_COMPILE = $(patsubst %-gcc,%-,$(CC))
OBJCOPY ?= $(CROSS_COMPILE)objcopy
OBJDUMP ?= $(CROSS_COMPILE)objdump

LINKER_SCRIPT ?= $(COMMON_DIR)/link.ld
#CRT ?= $(COMMON_DIR)/crt0.S
CRT ?= $(SW_BUILD_PATH)/crt0.o
CFLAGS ?= -march=$(ARCH) -mabi=ilp32 -static -mcmodel=medany -Wall -g -Os\
	-fvisibility=hidden -nostdlib -nostartfiles -ffreestanding $(PROGRAM_CFLAGS)

#OBJS := ${C_SRCS:.c=.o} ${ASM_SRCS:.S=.o} ${CRT:.S=.o}
OBJS := ${C_SRCS:.c=.o} ${ASM_SRCS:.S=.o}
# This makes it so that object files are created within sim area (build/sim/$SIM_AREA/sw_build, outside src/sw/*)
OBJS := $(addprefix ${SW_BUILD_PATH}/, ${OBJS})
#OBJS := $(addprefix ${SW_BUILD_PATH}/, ${OBJS}) ${CRT:.S=.o}
DEPS = $(OBJS:%.o=%.d)

ifdef PROGRAM
	OUTFILES := $(PROGRAM).elf $(PROGRAM).vmem $(PROGRAM).bin
	OUTFILES := $(addprefix ${SW_BUILD_PATH}/, ${OUTFILES})
else
	OUTFILES := $(OBJS)
endif

all: $(OUTFILES)

ifdef PROGRAM
${SW_BUILD_PATH}/$(PROGRAM).elf: $(OBJS) $(LINKER_SCRIPT) $(CRT) $(COMMON_OBJS)
#	$(CC) $(CFLAGS) -T $(LINKER_SCRIPT) $(OBJS) -o $@ $(LIBS)
	$(CC) $(CFLAGS) -T $(LINKER_SCRIPT) $(wildcard $(SW_BUILD_PATH)/*.o) -o ${SW_BUILD_PATH}/$(PROGRAM).elf $(LIBS)

.PHONY: disassemble
disassemble: $(PROGRAM).dis
endif

${SW_BUILD_PATH}/%.dis: ${SW_BUILD_PATH}/%.elf
	$(OBJDUMP) -fhSD $^ > $@

# Note: this target requires the srecord package to be installed.
# XXX: This could be replaced by objcopy once
# https://sourceware.org/bugzilla/show_bug.cgi?id=19921
# is widely available.
${SW_BUILD_PATH}/%.vmem: ${SW_BUILD_PATH}/%.bin
	srec_cat $^ -binary -offset 0x0000 -byte-swap 4 -o ${SW_BUILD_PATH}/../MemFile.vmem -vmem
#   TODO: Print to a txt file in sim area which $PROG MemFile.vmem is associated to

${SW_BUILD_PATH}/%.bin: ${SW_BUILD_PATH}/%.elf
	$(OBJCOPY) -O binary $^ $@

#%.o: %.c
$(OBJS): $(SW_BUILD_PATH)/%.o: $(SW_SRC_PATH)/$(PROGRAM)/%.c 
	$(CC) $(CFLAGS) -MMD -c $(INCS) -o $@ $<

$(SW_BUILD_PATH)/%.o: $(COMMON_DIR)/%.c
	$(CC) $(CFLAGS) -MMD -c $(INCS) -o $@ $<

#%.o: %.S
#	$(CC) $(CFLAGS) -MMD -c $(INCS) -o $@ $<

$(SW_BUILD_PATH)/%.o: $(COMMON_DIR)/%.S
	$(CC) $(CFLAGS) -MMD -c $(INCS) -o $@ $<

clean:
	$(RM) -f $(OBJS) $(DEPS)

distclean: clean
	$(RM) -f $(OUTFILES)
