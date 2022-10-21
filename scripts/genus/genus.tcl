## 1) load synthesis configuration, read description and elaborate design
set SCRIPTS_DIR $env(SCRIPTS_DIR)
source ${SCRIPTS_DIR}/genus/setup.tcl

read_hdl -language sv -define SYNTHESIS "${RISCV_CRYPTO_RTL}/util/prim_ram_1p_pkg.sv ${RISCV_CRYPTO_RTL}/util/prim_ram_2p_pkg.sv"
read_hdl -language sv -define SYNTHESIS "${RISCV_CRYPTO_RTL}/util/prim_lfsr.sv ${RISCV_CRYPTO_RTL}/util/prim_onehot_check.sv"
read_hdl -language sv -define SYNTHESIS "${RISCV_CRYPTO_RTL}/util/prim_secded_inv_39_32_dec.sv ${RISCV_CRYPTO_RTL}/util/prim_secded_inv_39_32_enc.sv"
read_hdl -language sv -define SYNTHESIS -f "${RISCV_CRYPTO_RTL}/syn/syn.f"
read_hdl -language sv -define SYNTHESIS -f "${RISCV_CRYPTO_RTL}/${RISCV_CORE}/${RISCV_CORE}.f"
elaborate;# -parameters [getParams "${SIM_DIR}/param.txt"]
check_design -all

## 2) read constraints
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
write_hdl            > deliverables/ibex_top.v

exit

