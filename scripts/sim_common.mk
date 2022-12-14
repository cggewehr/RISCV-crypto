export RISCV_CORE := ${RISCV_CORE}
export RISCV_CRYPTO_RTL := ${RISCV_CRYPTO_RTL}
SCRIPTS_DIR=${ROOT_PATH}/scripts
export SCRIPTS_DIR := ${SCRIPTS_DIR}
export SIM_PATH := ${SIM_PATH}

NETLIST ?= 0
CELL_LIB_BASE_PATH = /soft64/design-kits/stm/28nm-cmos28fdsoi_24/C28SOI_SC_12
CELL_LIB_VERILOG = $(CELL_LIB_BASE_PATH)_CORE_LR@2.0@20130411.0/behaviour/verilog/C28SOI_SC_12_CORE_LR.v $(CELL_LIB_BASE_PATH)_CLK_LR@2.1@20130621.0/behaviour/verilog/C28SOI_SC_12_CLK_LR.v

# Set tool-specific variables
ifeq (${VENDOR}, Cadence)
	COMP_EXEC=ncvlog

    ifeq ($(NETLIST), 0)
		COMP_OPTS=-logfile log/ncvlog.log -errormax 15 -update -linedebug -status -messages -sv -nowarn NCEXDEP -work worklib +incdir+${RISCV_CRYPTO_RTL}/util +define+RVFI
		ELAB_OPTS=-logfile log/ncelab.log -errormax 15 -update -status -nowarn NCEXDEP -nowarn DSEM2009 -defparam tb_top.SRAMInitFile=\"${SIM_PATH}/MemFile.vmem\" -timescale 1ps/1ps worklib.tb_top
	else
		COMP_OPTS=-logfile log/ncvlog.log -errormax 15 -update -linedebug -status -messages -sv -nowarn NCEXDEP -work worklib +incdir+${RISCV_CRYPTO_RTL}/util +define+NETLIST +define+functional
#		COMP_OPTS=-logfile log/ncvlog.log -errormax 15 -update -linedebug -status -messages -sv -nowarn NCEXDEP -work worklib +incdir+${RISCV_CRYPTO_RTL}/util +define+NETLIST
		ELAB_OPTS=-logfile log/ncelab.log -errormax 15 -update -status -nowarn NCEXDEP -nowarn DSEM2009 -defparam tb_top.SRAMInitFile=\"${SIM_PATH}/MemFile.vmem\" -timescale 1ps/1ps worklib.tb_top
	endif

	ELAB_EXEC=ncelab
	SIM_EXEC=ncsim
	SIM_OPTS=-logfile log/ncsim.log -errormax 15 -nowarn NCEXDEP -nowarn DSEM2009 worklib.tb_top
	SIM_GUI_OPTS=${SIM_OPTS} -gui -input ${SCRIPTS_DIR}/cadence_gui.tcl
	SYN_EXEC=genus
	SYN_OPTS=-f ${SCRIPTS_DIR}/genus/genus.tcl -no_gui -log log/genus -overwrite

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


sw:

	@echo -e "\n---- Building VMEM file from <${SW_BUILD_PATH}/${PROG}>"
	make -f ${ROOT_PATH}/src/sw/${PROG}/Makefile PROGRAM=${PROG} SW_BUILD_PATH=${SW_BUILD_PATH} SW_SRC_PATH=${SW_SRC_PATH} COMMON_DIR=${COMMON_DIR}

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

clean:

	rm -rf *.err
	rm -rf *.diag
	rm -rf *.key
	rm -rf xcelium.d
	rm -rf waves.shm
	rm -rf log/*
	rm -rf sw_build/*
	rm -rf MemFile.vmem
	rm -rf deliverables/*
	rm -rf fv
	rm -rf genus
	rm -rf genus.*
