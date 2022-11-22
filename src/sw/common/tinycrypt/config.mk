################################################################################
#
#      Copyright (C) 2017 by Intel Corporation, All Rights Reserved.
#
#            Global configuration Makefile. Included everywhere.
#
################################################################################

# EDIT HERE:
#CC:=gcc
CC:=riscv64-elf-gcc
#CFLAGS:=-Os -std=c99 -Wall -Wextra -D_ISOC99_SOURCE -MMD -I../lib/include/ -I../lib/source/ -I../tests/include/
#CFLAGS:=-Os -std=c99 -Wall -Wextra -D_ISOC99_SOURCE -MMD -I../ -I../../ -I./include/ 
CFLAGS:=-Os -march=rv32imc_zicsr_zkne_zknh -mabi=ilp32 -std=gnu99 -Wall -Wextra -D_ISOC99_SOURCE -DSHA256_RISCV_ASM -MMD -I../ -I../../ -I./include/ 
LINKERFLAGS:=-march=rv32imc -mabi=ilp32
#vpath %.c ../lib/source/
vpath %.c ../
ENABLE_TESTS=true

# override MinGW built-in recipe
%.o: %.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

ifeq ($(OS),Windows_NT)
DOTEXE:=.exe
endif

# DO NOT EDIT AFTER THIS POINT:
ifeq ($(ENABLE_TESTS), true)
CFLAGS += -DENABLE_TESTS
else
CFLAGS += -DDISABLE_TESTS
endif

export CC
export CFLAGS
export LINKERFLAGS
export VPATH
export ENABLE_TESTS

################################################################################
