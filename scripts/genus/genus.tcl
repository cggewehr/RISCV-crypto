## 1) load synthesis configuration, read description and elaborate design
set SCRIPTS_DIR $env(SCRIPTS_DIR)
source ${SCRIPTS_DIR}/setup.tcl

read_hdl -language sv -define SYNTHESIS "syn/syn.f ${RISCV_CORE}/${RISCV_CORE}.f"
elaborate -parameters
check_design -all 
# TODO: Set top as ibex_core

## 2) read constraints
# create_clock -period ${CLK_PERIOD} -name clock [get_ports clock]
read_sdc constraints_28nm.sdc
check_timing_intent -verbose
# set_dont_use *SDF*

## 3) synthesize
# set_db auto_ungroup none
syn_generic
syn_map
syn_opt

## 4) write reports & netlist
report area          > deliverables/rpt_area.txt
report timing        > deliverables/rpt_timing.txt
report power         > deliverables/rpt_power.txt
report_gates         > deliverables/rpt_gates.txt
report_clock_gating  > deliverables/rpt_clock_gating.txt
write_hdl            > deliverables/ibex_core.v

exit
