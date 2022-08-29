

# Set tool-specific variables
ifeq VENDOR Cadence
    COMP_EXEC=ncvlog
    COMP_OPTS=-logfile log/cadence/ncvlog.log -errormax 15 -update -linedebug -status -sv -work worklib
    ELAB_EXEC=ncelab
    ELAB_OPTS=-logfile log/cadence/ncelab.log -errormax 15 -update -status -defparam tb_top.SRAMInitFile=${SIM_PATH}/MemFile.vmem worklib.tb_top
    SIM_EXEC=ncsim
    SIM_OPTS=-logfile log/cadence/ncsim.log -errormax 15
    SIM_GUI_OPTS=${SIM_OPTS} -gui

else
    @echo Vendor <${VENDOR}> not supported by this makefile
    exit 1
    
sw:

    @echo ---- Building VMEM file from ${PROGRAM} dir
    @export
    make -f ${ROOT_PATH}/sw/${PROGRAM}/Makefile
    @echo ---- Done building VMEM file from ${PROGRAM} dir

compile:

    ${COMP_EXEC} ${COMP_OPTS} ${RISCV_CRYPTO_RTL}/${RISCV_CORE}/${RISCV_CORE}.f
    ${COMP_EXEC} ${COMP_OPTS} ${RISCV_CRYPTO_RTL}/util/util.f
    ${COMP_EXEC} ${COMP_OPTS} ${RISCV_CRYPTO_RTL}/simple-system-top/ibex_simple_system.sv
    ${COMP_EXEC} ${COMP_OPTS} ${RISCV_CRYPTO_TBENCH}/tb.f

elab:

    ${ELAB_EXEC} ${ELAB_OPTS}

sim:

    ${SIM_EXEC} ${SIM_OPTS}

simgui:

    ${SIM_EXEC} ${SIM_GUI_OPTS}

all: compile elab sim
    
allgui: compile elab simgui
    
clean:

    rm xcelium.d
    rm log
