

# Set tool-specific variables
ifeq (${VENDOR}, Cadence)
	COMP_EXEC=ncvlog
	COMP_OPTS=-logfile log/ncvlog.log -errormax 15 -update -linedebug -status -sv -work worklib -f
	ELAB_EXEC=ncelab
	ELAB_OPTS=-logfile log/ncelab.log -errormax 15 -update -status -defparam tb_top.SRAMInitFile=${SIM_PATH}/MemFile.vmem worklib.tb_top
	SIM_EXEC=ncsim
	SIM_OPTS=-logfile log/ncsim.log -errormax 15
	SIM_GUI_OPTS=${SIM_OPTS} -gui

else
	@echo Vendor <${VENDOR}> not supported by this makefile
	exit 1
endif
	
sw:

	@echo "---- Building VMEM file from <${SW_BUILD_PATH}/${PROG}>"
	make -f ${ROOT_PATH}/src/sw/${PROG}/Makefile PROGRAM=${PROG} SW_BUILD_PATH=${SW_BUILD_PATH} SW_SRC_PATH=${SW_SRC_PATH} COMMON_DIR=${COMMON_DIR}

compile:

	@echo "---- Compiling RTL"
	${COMP_EXEC} ${COMP_OPTS} ${RISCV_CRYPTO_RTL}/${RISCV_CORE}/${RISCV_CORE}.f
	${COMP_EXEC} ${COMP_OPTS} ${RISCV_CRYPTO_RTL}/util/util.f
	${COMP_EXEC} ${COMP_OPTS} ${RISCV_CRYPTO_RTL}/simple-system-top/ibex_simple_system.sv
	${COMP_EXEC} ${COMP_OPTS} ${RISCV_CRYPTO_TBENCH}/tb.f

elab: compile

	@echo "---- Elaborating Testbench"
	${ELAB_EXEC} ${ELAB_OPTS}

sim: elab

	@echo "---- Beginning Simulation"
	${SIM_EXEC} ${SIM_OPTS}

simgui: elab

	@echo "---- Beginning Interactive Simulation"
	${SIM_EXEC} ${SIM_GUI_OPTS}

all: sim
	
allgui: simgui
	
clean:

	rm -rf xcelium.d
	rm -rf log/*
	rm -rf sw_build/*
	rm -rf MemFile.vmem

