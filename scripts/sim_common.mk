export RISCV_CORE := ${RISCV_CORE}
export RISCV_CRYPTO_RTL := ${RISCV_CRYPTO_RTL}
SCRIPTS_DIR=${ROOT_PATH}/scripts
export SCRIPTS_DIR := ${SCRIPTS_DIR}
export SIM_PATH := ${SIM_PATH}

NETLIST ?= 0
CELL_LIB_BASE_PATH = /soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_
CELL_LIB_VERILOG = $(CELL_LIB_BASE_PATH)8_CORE_LL@2.1@20131011.0/behaviour/verilog/C28SOI_SC_8_CORE_LL.v $(CELL_LIB_BASE_PATH)8_CLK_LL@2.2@20131011.0/behaviour/verilog/C28SOI_SC_8_CLK_LL.v
#CELL_LIB_VERILOG = $(CELL_LIB_BASE_PATH)12_CORE_LR@2.0@20130411.0/behaviour/verilog/C28SOI_SC_12_CORE_LR.v $(CELL_LIB_BASE_PATH)12_CLK_LR@2.1@20130621.0/behaviour/verilog/C28SOI_SC_12_CLK_LR.v

# Set tool-specific variables
ifeq (${VENDOR}, Cadence)
	COMP_EXEC=ncvlog

    ifeq ($(NETLIST), 0)
		COMP_OPTS=-logfile log/ncvlog.log -errormax 15 -update -linedebug -status -messages -sv -nowarn NCEXDEP -work worklib +incdir+${RISCV_CRYPTO_RTL}/util +define+RVFI
		ELAB_OPTS=-logfile log/ncelab.log -errormax 15 -update -status -nowarn NCEXDEP -nowarn DSEM2009 -defparam tb_top.SRAMInitFile=\"${SIM_PATH}/MemFile.vmem\" -defparam tb_top.SymbolAddrs=\"${SIM_PATH}/sw_build/symbol_table.txt\" -timescale 1ps/1ps worklib.tb_top
	else
		COMP_OPTS=-logfile log/ncvlog.log -errormax 15 -update -linedebug -status -messages -sv -nowarn NCEXDEP -work worklib +incdir+${RISCV_CRYPTO_RTL}/util +define+NETLIST +define+functional
#		COMP_OPTS=-logfile log/ncvlog.log -errormax 15 -update -linedebug -status -messages -sv -nowarn NCEXDEP -work worklib +incdir+${RISCV_CRYPTO_RTL}/util +define+NETLIST
		ELAB_OPTS=-logfile log/ncelab.log -errormax 15 -update -status -nowarn NCEXDEP -nowarn DSEM2009 -defparam tb_top.SRAMInitFile=\"${SIM_PATH}/MemFile.vmem\" -defparam tb_top.SymbolAddrs=\"${SIM_PATH}/sw_build/symbol_table.txt\" -timescale 1ps/1ps worklib.tb_top
	endif

	ELAB_EXEC=ncelab
	SIM_EXEC=ncsim
	SIM_OPTS=-logfile log/ncsim.log -errormax 15 -nowarn NCEXDEP -nowarn DSEM2009 worklib.tb_top
	SIM_GUI_OPTS=${SIM_OPTS} -gui -input ${SCRIPTS_DIR}/cadence_gui.tcl
	SYN_EXEC=genus
	SYN_OPTS=-f ${SCRIPTS_DIR}/genus/genus.tcl -no_gui -log log/genus -overwrite
	POWER_EXEC=genus
	POWER_OPTS=-f ${SCRIPTS_DIR}/genus/power_analysis.tcl -no_gui -log log/genus_pa -overwrite -lic_startup_options Joules_RTL_Power

else
	@echo Vendor <${VENDOR}> not supported by this makefile
	exit 1
endif


SYN_FILES = $(addprefix ${RISCV_CRYPTO_RTL}/syn/, $(shell cat ${RISCV_CRYPTO_RTL}/syn/syn.f))
UTIL_FILES = $(addprefix ${RISCV_CRYPTO_RTL}/util/, $(shell cat ${RISCV_CRYPTO_RTL}/util/util.f))

ifeq ($(NETLIST), 0)
	CORE_FILES = $(addprefix ${RISCV_CRYPTO_RTL}/${RISCV_CORE}/, $(shell cat ${RISCV_CRYPTO_RTL}/${RISCV_CORE}/${RISCV_CORE}.f))
else
	CORE_FILES = ${RISCV_CRYPTO_RTL}/${RISCV_CORE}/ibex_pkg.sv ${SIM_PATH}/deliverables/ibex_top.v $(CELL_LIB_VERILOG)
endif

TOP_FILES = $(addprefix ${RISCV_CRYPTO_RTL}/simple-system-top/, $(shell cat ${RISCV_CRYPTO_RTL}/simple-system-top/simple-system.f))
TB_FILES = $(addprefix ${RISCV_CRYPTO_TBENCH}/, $(shell cat ${RISCV_CRYPTO_TBENCH}/tb.f))

SYMBOLS_LIST="tc_aes128_set_encrypt_key
SYMBOLS_LIST+=tc_aes192_set_encrypt_key
SYMBOLS_LIST+=tc_aes256_set_encrypt_key
# SYMBOLS_LIST+=tc_aes_encrypt
SYMBOLS_LIST+=tc_sha256_init
SYMBOLS_LIST+=tc_sha256_update
SYMBOLS_LIST+=tc_sha256_final
SYMBOLS_LIST+=sha256_compress
SYMBOLS_LIST+=tc_sha512_init
SYMBOLS_LIST+=tc_sha512_update
SYMBOLS_LIST+=tc_sha512_final
SYMBOLS_LIST+=sha512_compress
#SYMBOLS_LIST+=ascon_permute
SYMBOLS_LIST+=crypto_aead_chacha20poly1305_ietf_encrypt
SYMBOLS_LIST+=crypto_aead_chacha20poly1305_ietf_encrypt.constprop.0
SYMBOLS_LIST+=tc_ccm_generation_encryption
SYMBOLS_LIST+=sha3_f1600_rvb32
SYMBOLS_LIST+=crypto_kem_keypair
SYMBOLS_LIST+=crypto_kem_enc
SYMBOLS_LIST+=crypto_kem_dec
SYMBOLS_LIST+=rvkat_info
SYMBOLS_LIST+=ascon_core
SYMBOLS_LIST+=ascon_aead_encrypt_opt64
SYMBOLS_LIST+=ascon_hash"

START_TIME?=0ns
END_TIME?=0ns

export POWER_ANALYSIS_DB_FILE=${SIM_PATH}/deliverables/genus.mapped.db
export POWER_ANALYSIS_SHM_FILE=${SIM_PATH}/deliverables/shm/netlist_sim.shm
export POWER_ANALYSIS_REPORT_DIR=${SIM_PATH}/deliverables
export POWER_ANALYSIS_REPORT_NAME=${SIM_NAME}
export POWER_ANALYSIS_START_TIME=${START_TIME}
export POWER_ANALYSIS_END_TIME=${END_TIME}

sw:

	@echo -e "\n---- Building VMEM file from <${SW_BUILD_PATH}/${PROG}>"
	make -f ${ROOT_PATH}/src/sw/${PROG}/Makefile PROGRAM=${PROG} SW_BUILD_PATH=${SW_BUILD_PATH} COMMON_DIR=${COMMON_DIR} all
	nm sw_build/${PROG}.elf > sw_build/nm.out
	awk '{split(${SYMBOLS_LIST}, sym_list); for (i in sym_list) if (sym_list[i] == $$3) print $$3, $$1}' sw_build/nm.out > sw_build/symbol_table.txt
	@echo -e "\n---- Stack Usage:"
	python3 ${ROOT_PATH}/tools/stack-usage/checkStackUsage.py ./sw_build/${PROG}.elf ./sw_build $(subst $\",,$(SYMBOLS_LIST)) | tee sw_build/stack-usage.txt

comp:

	@echo -e "\n---- Compiling SYN (Technology Specific)"
	${COMP_EXEC} ${COMP_OPTS} ${SYN_FILES}
	@echo -e "\n---- Compiling UTIL"
	${COMP_EXEC} ${COMP_OPTS} ${UTIL_FILES}
	@echo -e "\n---- Compiling CORE <${RISCV_CORE}>"
	${COMP_EXEC} ${COMP_OPTS} ${CORE_FILES}
	@echo -e "\n---- Compiling TOP"
	${COMP_EXEC} ${COMP_OPTS} ${TOP_FILES}
	@echo -e "\n---- Compiling TB"
	${COMP_EXEC} ${COMP_OPTS} ${TB_FILES}

elab:

	@echo -e "\n---- Elaborating Testbench"
	${ELAB_EXEC} ${ELAB_OPTS}

sim:

	@echo -e "\n---- Beginning Simulation"
	${SIM_EXEC} ${SIM_OPTS}

simgui:

	@echo -e "\n---- Beginning Interactive Simulation"
	${SIM_EXEC} ${SIM_GUI_OPTS}

all: comp elab sim

allgui: comp elab simgui

netlist:

	@echo -e "\n---- Running synthesis"
	${SYN_EXEC} ${SYN_OPTS}

power:

	@echo -e "\n---- Running power analysis"
	${POWER_EXEC} ${POWER_OPTS}

clean:

	rm -rf *.err
	rm -rf *.diag
	rm -rf *.key
	rm -rf xcelium.d
	rm -rf waves.shm
	rm -rf log/*
	rm -rf MemFile.vmem
	rm -rf symbol_table.txt
	rm -rf fv
	rm -rf genus
	rm -rf genus.*
	rm -rf sw_build/nm.out
	rm -rf sw_build/symbol_table.txt
	make -f ${ROOT_PATH}/src/sw/${PROG}/Makefile PROGRAM=${PROG} SW_BUILD_PATH=${SW_BUILD_PATH} COMMON_DIR=${COMMON_DIR} clean

