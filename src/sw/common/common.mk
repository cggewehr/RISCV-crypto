# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

# Now is set within individual makefiles from each sim area
# COMMON_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

#COMMON_SRCS = $(wildcard $(COMMON_DIR)/*.c) $(wildcard $(COMMON_DIR)/tinycrypt/*.c)
#COMMON_OBJS = ${COMMON_SRCS:.c=.o}
#COMMON_OBJS := $(patsubst $(COMMON_DIR)%, $(SW_BUILD_PATH)%, ${COMMON_OBJS})


INCS := -I$(COMMON_DIR) -I$(COMMON_DIR)/tinycrypt
DEFS := -DSHA256_RISCV_ASM -DTC_AES_128 -DAES_RISCV_ASM
#DEFS := 
ARCH ?= rv32imc_zicsr_zkne_zknh
CFLAGS ?= -march=$(ARCH) -mabi=ilp32 -static -mcmodel=medany -Wall -g -Os -fvisibility=hidden -ffreestanding $(INCS) $(DEFS) $(PROGRAM_CFLAGS)
CC = riscv64-elf-gcc
CROSS_COMPILE = $(patsubst %-gcc,%-,$(CC))
OBJCOPY ?= $(CROSS_COMPILE)objcopy
OBJDUMP ?= $(CROSS_COMPILE)objdump

LINKER_SCRIPT ?= $(COMMON_DIR)/link.ld

VPATH := $(COMMON_DIR) $(COMMON_DIR)/tinycrypt $(SW_SRC_PATH)/$(PROG)
SRCS := $(foreach SRC_DIR, $(VPATH), $(wildcard $(SRC_DIR)/*))
C_SRCS := $(filter %.c, $(SRCS))
ASM_SRCS := $(filter %.S, $(SRCS))

$(info C_SRCS=$(C_SRCS))
$(info PROG=$(PROG))

OBJS := $(notdir ${C_SRCS:.c=.o} ${ASM_SRCS:.S=.o})
# This makes it so that object files are created within sim area (build/sim/$SIM_AREA/sw_build, outside src/sw/)
OBJS := $(addprefix ${SW_BUILD_PATH}/, ${OBJS})
DEPS = $(OBJS:%.o=%.d)

ifdef PROG
	OUTFILES := $(PROG).elf $(PROG).vmem $(PROG).bin $(PROG).dis
	OUTFILES := $(addprefix ${SW_BUILD_PATH}/, ${OUTFILES})
else
	OUTFILES := $(OBJS)
endif

all: $(OUTFILES)

ifdef PROG

${SW_BUILD_PATH}/$(PROG).elf: $(OBJS)
#	$(CC) -march=rv32imc -mabi=ilp32 -nostdlib -nostartfiles -T $(LINKER_SCRIPT) $(wildcard $(SW_BUILD_PATH)/*.o) -o ${SW_BUILD_PATH}/$(PROG).elf
	$(CC) -march=rv32imc -mabi=ilp32 -nostartfiles -T $(LINKER_SCRIPT) $(wildcard $(SW_BUILD_PATH)/*.o) -o ${SW_BUILD_PATH}/$(PROG).elf

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

endif

#$(OBJS): $(SW_BUILD_PATH)/%.o: $(SW_SRC_PATH)/$(PROGRAM)/%.c 
$(SW_BUILD_PATH)/%.o: %.c 
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

#$(SW_BUILD_PATH)/%.o: $(COMMON_DIR)/%.c
$(SW_BUILD_PATH)/%.o: %.S
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

clean:
	$(RM) -f $(OBJS) $(DEPS)

distclean: clean
	$(RM) -f $(OUTFILES)
