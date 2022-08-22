compile:

    # TODO: Create worklib

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

all:

    make compile
    make elab
    make sim
    
allgui:

    make compile
    make elab
    make simgui
    
clean:

    rm work
