# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

# ------------------------------
# Defines for ISE-specific ASM
# ------------------------------

SHA256_ASM ?= 0
SHA512_ASM ?= 0
AES_ASM ?= 0
ASCON_ISE ?= 0
KYBER_ISE ?= 0
SET_CUSTOM_BINUTILS?=0

ifneq (${SHA256_ASM}, 0)
	DEFS += -DSHA256_RISCV_ASM
endif

ifneq (${SHA512_ASM}, 0)
	DEFS += -DSHA512_RISCV_ASM
endif

ifneq (${AES_ASM}, 0)
	DEFS += -DAES_RISCV_ASM
endif

ifneq (${ASCON_ISE}, 0)
	DEFS += -DASCON_ISE
	SET_CUSTOM_BINUTILS := 1
endif

ifneq (${KYBER_ISE}, 0)
	DEFS += -DKYBER_ISE
	SET_CUSTOM_BINUTILS := 1
endif

ifneq (${SET_CUSTOM_BINUTILS}, 0)
	DEFS += -B/home/${USER}/riscv-gnu-toolchain/bin/riscv64-unknown-linux-gnu-
	OBJDUMP := /home/${USER}/riscv-gnu-toolchain/bin/riscv64-unknown-linux-gnu-objdump
	OBJCOPY := /home/${USER}/riscv-gnu-toolchain/bin/riscv64-unknown-linux-gnu-objcopy
endif

# TODO: If verbose flag print if ISE ASM will or not be used for each extension

# ------------------------------
# AES 128, 192 or 256
# ------------------------------

AES ?= 256

ifeq ($(AES), 128)
	DEFS += -DTC_AES_128
else ifeq ($(AES), 192)
	DEFS += -DTC_AES_192
else ifeq ($(AES), 256)
	DEFS += -DTC_AES_256
else
	$(error Unexpected AES = $(AES))
endif

# TODO: If verbose flag print which AES will be used

# ------------------------------
# Use Zbkb extension instructions
# ------------------------------

ZBKB ?= 0

# Define used in "src/sw/common/riscv_crypto_scalar.h" to control the use of Zk* (including Zbkb) instructions
ifneq (${ZBKB}, 0)
	DEFS += -DRVKINTRIN_ASSEMBLER
else
	DEFS += -DRVKINTRIN_EMULATE
endif

# ------------------------------
# Kyber variant and security levels defines
# Defines are used in "param.h" to set variant (Kyber-90s or Keccak) and security (512, 768 or 1024)
# ------------------------------

KYBER_VARIANT ?= 90s
KYBER_STRENGTH ?= 512

ifeq (${KYBER_VARIANT}, 90s)
	DEFS += -DKYBER_90S
endif

ifeq (${KYBER_STRENGTH}, 512)
	DEFS += -DKYBER_K=2
else ifeq (${KYBER_STRENGTH}, 768)
	DEFS += -DKYBER_K=3
else ifeq (${KYBER_STRENGTH}, 1024)
	DEFS += -DKYBER_K=4
else
	$(error Unexpected KYBER_STRENGTH = $(KYBER_STRENGTH))
endif

# ------------------------------
# Dependencies for each target
# ------------------------------

COMMON_C_SOURCES := $(wildcard $(COMMON_DIR)/*.c)
COMMON_ASM_SOURCES := $(wildcard $(COMMON_DIR)/*.S)
COMMON_HEADERS := $(wildcard $(COMMON_DIR)/*.h)
COMMON_OBJS := $(COMMON_C_SOURCES:%.c=%.o) $(COMMON_ASM_SOURCES:%.S=%.o)
COMMON_OBJS := $(subst ${COMMON_DIR}, ${SW_BUILD_PATH}/obj, ${COMMON_OBJS})

TINYCRYPT_DIR ?= $(COMMON_DIR)/tinycrypt
TINYCRYPT_C_SOURCES := $(wildcard $(TINYCRYPT_DIR)/*.c)
TINYCRYPT_ASM_SOURCES := $(wildcard $(TINYCRYPT_DIR)/*.S)
TINYCRYPT_HEADERS := $(wildcard $(TINYCRYPT_DIR)/*.h)
TINYCRYPT_OBJS := $(TINYCRYPT_C_SOURCES:%.c=%.o) $(TINYCRYPT_ASM_SOURCES:%.S=%.o)
TINYCRYPT_OBJS := $(subst ${TINYCRYPT_DIR}, ${SW_BUILD_PATH}/obj, ${TINYCRYPT_OBJS})

KECCAK_DIR ?= $(COMMON_DIR)/keccak
KECCAK_C_SOURCES := $(wildcard $(KECCAK_DIR)/*.c)
KECCAK_ASM_SOURCES := $(wildcard $(KECCAK_DIR)/*.S)
KECCAK_HEADERS := $(wildcard $(KECCAK_DIR)/*.h)
KECCAK_OBJS := $(KECCAK_C_SOURCES:%.c=%.o) $(KECCAK_ASM_SOURCES:%.S=%.o)
KECCAK_OBJS := $(subst ${KECCAK_DIR}, ${SW_BUILD_PATH}/obj, ${KECCAK_OBJS})

ASCON_DIR ?= $(COMMON_DIR)/ascon
ASCON_C_SOURCES = $(wildcard $(ASCON_DIR)/*.c)
ASCON_ASM_SOURCES = $(wildcard $(ASCON_DIR)/*.S)
ASCON_HEADERS = $(wildcard $(ASCON_DIR)/*.h)
ASCON_OBJS := $(ASCON_C_SOURCES:%.c=%.o) $(ASCON_ASM_SOURCES:%.S=%.o)
ASCON_OBJS := $(subst ${ASCON_DIR}, ${SW_BUILD_PATH}/obj, ${ASCON_OBJS})

KYBER_DIR ?= $(COMMON_DIR)/kyber_round3_ref
KYBER_C_SOURCES = $(wildcard $(KYBER_DIR)/*.c)
KYBER_ASM_SOURCES = $(wildcard $(KYBER_DIR)/*.S)
KYBER_HEADERS = $(wildcard $(KYBER_DIR)/*.h)
KYBER_OBJS = $(KYBER_C_SOURCES:%.c=%.o) $(KYBER_ASM_SOURCES:%.S=%.o)
KYBER_OBJS := $(subst ${KYBER_DIR}, ${SW_BUILD_PATH}/obj, ${KYBER_OBJS})

LOCAL_C_SOURCES = $(wildcard ${PROGRAM_DIR}/*.c)
LOCAL_ASM_SOURCES = $(wildcard ${PROGRAM_DIR}/*.S)
LOCAL_HEADERS = $(wildcard ${PROGRAM_DIR}/*.h)
LOCAL_OBJS := $(LOCAL_C_SOURCES:%.c=%.o) $(LOCAL_ASM_SOURCES:%.S=%.o)
LOCAL_OBJS := $(subst ${PROGRAM_DIR}, ${SW_BUILD_PATH}/obj, ${LOCAL_OBJS})

# Source files will be looked for in the VPATH dir
VPATH = $(PROGRAM_DIR) $(COMMON_DIR) $(TINYCRYPT_DIR) $(KECCAK_DIR) $(ASCON_DIR) $(KYBER_DIR)

# ------------------------------
# Compiler and other tools setup
# ------------------------------

# ARCH ?= rv32imc_zicsr_zkne_zknh
ifneq (${ZBKB}, 0)
	ARCH ?= rv32imc_zicsr_zkne_zknh_zbkb
else
	ARCH ?= rv32imc_zicsr_zkne_zknh
endif

INCS := -I$(PROGRAM_DIR) -I$(COMMON_DIR) -I$(TINYCRYPT_DIR) -I$(KECCAK_DIR) -I$(ASCON_DIR) -I$(KYBER_DIR)
# CFLAGS ?= -march=$(ARCH) -mabi=ilp32 -static -mcmodel=medany -Wall -Os -fvisibility=hidden -ffreestanding -flto -fcallgraph-info=su $(INCS) $(DEFS) $(PROGRAM_CFLAGS)
CFLAGS ?= -march=$(ARCH) -mabi=ilp32 -static -mcmodel=medany -Wall -Os -fvisibility=hidden -ffreestanding -fcallgraph-info=su -fstack-usage $(INCS) $(DEFS) $(PROGRAM_CFLAGS)
CC = riscv64-elf-gcc
CROSS_COMPILE = $(patsubst %-gcc,%-,$(CC))
OBJCOPY ?= $(CROSS_COMPILE)objcopy
OBJDUMP ?= $(CROSS_COMPILE)objdump
OBJDUMPFLAGS ?= -fhSD

LINKER_SCRIPT ?= $(COMMON_DIR)/link.ld

# ------------------------------
# Targets for executable ELF
# NB: Each .o target in here should be in the $OBJS list
# ------------------------------

OBJS := ${LOCAL_OBJS} ${COMMON_OBJS} ${TINYCRYPT_OBJS} ${KECCAK_OBJS} ${ASCON_OBJS} ${KYBER_OBJS}
OUTFILES := $(PROG).elf $(PROG).vmem $(PROG).bin
#OUTFILES := $(PROG).elf $(PROG).vmem
OUTFILES := $(addprefix ${SW_BUILD_PATH}/, ${OUTFILES})

$(SW_BUILD_PATH)/obj/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
	$(OBJDUMP) $(OBJDUMPFLAGS) $@ > $(subst .o,.dis,$@)
	mv $(subst .o,.dis,$@) $(@D)/../dis/$(subst .o,.dis,$(@F))
	mv $(subst .o,.su,$@) $(@D)/../su/$(subst .o,.su,$(@F))
	mv $(subst .o,.ci,$@) $(@D)/../vcg/$(subst .o,.ci,$(@F))

$(SW_BUILD_PATH)/obj/%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<
	$(OBJDUMP) $(OBJDUMPFLAGS) $@ > $(subst .o,.dis,$@)
	mv $(subst .o,.dis,$@) $(@D)/../dis/$(subst .o,.dis,$(@F))
	mv $(subst .o,.su,$@) $(@D)/../su/$(subst .o,.su,$(@F)) || true
	mv $(subst .o,.ci,$@) $(@D)/../vcg/$(subst .o,.ci,$(@F)) || true

${SW_BUILD_PATH}/$(PROG).elf: $(OBJS)
#	$(CC) -march=rv32imc -mabi=ilp32 -nostdlib -nostartfiles -T $(LINKER_SCRIPT) $(wildcard $(SW_BUILD_PATH)/*.o) -o ${SW_BUILD_PATH}/$(PROG).elf
	$(CC) -march=rv32imc -mabi=ilp32 -nostartfiles -T $(LINKER_SCRIPT) $(OBJS) $(PROGRAM_LDFLAGS) -o ${SW_BUILD_PATH}/$(PROG).elf
	$(OBJDUMP) $(OBJDUMPFLAGS) $@ > $(subst .elf,.dis,$@)

${SW_BUILD_PATH}/$(PROG).bin: ${SW_BUILD_PATH}/$(PROG).elf
	$(OBJCOPY) -O binary $^ $@

# Note: this target requires the srecord package to be installed.
# XXX: This could be replaced by objcopy once
# https://sourceware.org/bugzilla/show_bug.cgi?id=19921
# is widely available.
${SW_BUILD_PATH}/$(PROG).vmem: ${SW_BUILD_PATH}/$(PROG).bin
#${SW_BUILD_PATH}/%.vmem: ${SW_BUILD_PATH}/%.elf
#	srec_cat $^ -binary -offset 0x0000 -byte-swap 4 -o ${SW_BUILD_PATH}/../MemFile.vmem -vmem
#   Remove ram offset added in linker script (see $(LINKER_SCRIPT))
#	$(OBJCOPY) -S -O verilog --change-addresses "-0x100000" --verilog-data-width=4 -R .debug_* -R .comment $^ $@
	$(OBJCOPY) -S --verilog-data-width=4 --reverse-bytes=4 -O verilog -I binary $^ $@
	cp $@ ${SW_BUILD_PATH}/../MemFile.vmem

all: $(OUTFILES)

clean:
	$(RM) -f $(wildcard $(SW_BUILD_PATH)/dis/*)
	$(RM) -f $(wildcard $(SW_BUILD_PATH)/obj/*)
	$(RM) -f $(wildcard $(SW_BUILD_PATH)/su/*)
	$(RM) -f $(wildcard $(SW_BUILD_PATH)/vcg/*)
	$(RM) -f $(wildcard $(SW_BUILD_PATH)/$(PROG).*)

.PHONY: all clean
