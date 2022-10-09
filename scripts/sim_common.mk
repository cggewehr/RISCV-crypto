export RISCV_CORE := ${RISCV_CORE}
export RISCV_CRYPTO_RTL := ${RISCV_CRYPTO_RTL}
export SCRIPTS_DIR := ${SCRIPTS_DIR}

# Set tool-specific variables
ifeq (${VENDOR}, Cadence)
	COMP_EXEC=ncvlog
	COMP_OPTS=-logfile log/ncvlog.log -errormax 15 -update -linedebug -status -sv -work worklib +incdir+${RISCV_CRYPTO_RTL}/util +define+RVFI
#	COMP_OPTS=-logfile log/ncvlog.log -errormax 15 -update -linedebug -status -sv -work worklib +incdir+${RISCV_CRYPTO_RTL}/util
	ELAB_EXEC=ncelab
	ELAB_OPTS=-logfile log/ncelab.log -errormax 15 -update -status -defparam tb_top.SRAMInitFile=\"${SIM_PATH}/MemFile.vmem\" worklib.tb_top
	SIM_EXEC=ncsim
	SIM_OPTS=-logfile log/ncsim.log -errormax 15 worklib.tb_top
	SIM_GUI_OPTS=${SIM_OPTS} -gui -input ${SCRIPTS_DIR}/cadence_gui.tcl

else
	@echo Vendor <${VENDOR}> not supported by this makefile
	exit 1
endif

SYN_FILES = $(addprefix ${RISCV_CRYPTO_RTL}/syn/, $(shell cat ${RISCV_CRYPTO_RTL}/syn/syn.f))
UTIL_FILES = $(addprefix ${RISCV_CRYPTO_RTL}/util/, $(shell cat ${RISCV_CRYPTO_RTL}/util/util.f))
CORE_FILES = $(addprefix ${RISCV_CRYPTO_RTL}/${RISCV_CORE}/, $(shell cat ${RISCV_CRYPTO_RTL}/${RISCV_CORE}/${RISCV_CORE}.f))
TOP_FILES = ${RISCV_CRYPTO_RTL}/simple-system-top/ibex_simple_system.sv
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

	genus -f ${SCRIPTS_DIR}/genus.tcl -no_gui

clean:

	rm -rf *.err
	rm -rf *.diag
	rm -rf *.key
	rm -rf xcelium.d
	rm -rf waves.shm
	rm -rf log/*
	rm -rf sw_build/*
	rm -rf MemFile.vmem
	rm -rf deliverables
