set RISCV_CORE $env(RISCV_CORE)
set RISCV_CRYPTO_RTL $env(RISCV_CRYPTO_RTL)
set SCRIPTS_DIR $env(SCRIPTS_DIR)
set SIM_DIR $env(SIM_PATH)

set_db script_search_path "${SCRIPTS_DIR} ${SCRIPTS_DIR}/genus"
set_db hdl_search_path "${RISCV_CRYPTO_RTL}/syn/ ${RISCV_CRYPTO_RTL}/${RISCV_CORE}/ ${RISCV_CRYPTO_RTL}/util/"
set_db information_level 9

set_db hdl_track_filename_row_col true
set_db hdl_array_naming_style %s_%d
set_db hdl_instance_array_naming_style %s_%d
set_db hdl_generate_index_style %s_%d
set_db hdl_bus_wire_naming_style %s_%d

# Clock gating
set_db lp_insert_discrete_clock_gating_logic true
set_db lp_clock_gating_exceptions_aware true
set_db lp_insert_clock_gating true

set_db syn_generic_effort high
set_db syn_map_effort high
set_db syn_opt_effort high

#===============================================================================
#  Load STM 28nm libraries
#===============================================================================

# Set liberty
#set_db library "/soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_12_CORE_LR@2.0@20130411.0/libs/C28SOI_SC_12_CORE_LR_ss28_0.75V_125C.lib \
#                /soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_12_PR_LR@2.0@20130412.0/libs/C28SOI_SC_12_PR_LR_ss28_0.75V_125C.lib \
#                /soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_12_CLK_LR@2.1@20130621.0/libs/C28SOI_SC_12_CLK_LR_ss28_0.75V_125C.lib"
set_db library "/soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_8_CORE_LL@2.1@20131011.0/libs/C28SOI_SC_8_CORE_LL_ss28_0.75V_0.00V_0.00V_0.00V_125C.lib \
                /soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_8_PR_LL@2.1@20131028.1/libs/C28SOI_SC_8_PR_LL_ss28_0.75V_0.00V_0.00V_0.00V_125C.lib \
                /soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_8_CLK_LL@2.2@20131011.0/libs/C28SOI_SC_8_CLK_LL_ss28_0.75V_0.00V_0.00V_0.00V_125C.lib"

# Set LEF
#set_db lef_library "/soft64/design-kits/stm/28nm-cmos28fdsoi_25d/SiteDefKit_cmos28@1.4@20120720.0/LEF/sites.lef \
#                    /soft64/design-kits/stm/28nm-cmos28lp_42/CadenceTechnoKit_cmos028_6U1x_2U2x_2T8x_LB@4.2.1/LEF/technology.12T.lef \
#                    /soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_12_CORE_LR@2.0@20130411.0/CADENCE/LEF/C28SOI_SC_12_CORE_LR_soc.lef \
#                    /soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_12_PR_LR@2.0@20130412.0/CADENCE/LEF/C28SOI_SC_12_PR_LR_soc.lef \
#                    /soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_12_CLK_LR@2.1@20130621.0/CADENCE/LEF/C28SOI_SC_12_CLK_LR_soc.lef"
set_db lef_library "/soft64/design-kits/stm/28nm-cmos28fdsoi_25d/SiteDefKit_cmos28@1.4@20120720.0/LEF/sites.lef \
                    /soft64/design-kits/stm/28nm-cmos28lp_42/CadenceTechnoKit_cmos028_6U1x_2U2x_2T8x_LB@4.2.1/LEF/technology.8T.lef \
                    /soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_8_CORE_LL@2.1@20131011.0/CADENCE/LEF/C28SOI_SC_8_CORE_LL_soc.lef \
                    /soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_8_PR_LL@2.1@20131028.1/CADENCE/LEF/C28SOI_SC_8_PR_LL_soc.lef \
                    /soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_8_CLK_LL@2.2@20131011.0/CADENCE/LEF/C28SOI_SC_8_CLK_LL_soc.lef"

# Set captable
set_db cap_table_file "/soft64/design-kits/stm/28nm-cmos28lp_42/CadenceTechnoKit_cmos028_6U1x_2U2x_2T8x_LB@4.2.1/CAP_TABLE/FuncRCmax.captable"
set_db qrc_tech_file "/soft64/design-kits/stm/28nm-cmos28lp_42/CadenceTechnoKit_cmos028_6U1x_2U2x_2T8x_LB@4.2.1/QRC_TECHFILE/FuncRCmax.tech"

# Set PLE
set_db interconnect_mode ple

# Reads parameter file and outputs parameter string in Genus expected format
proc getParams {paramFileName} {

    set params "\{"

    if {[file exists $paramFileName]} {

        set paramFile [open $paramFileName]
        set lines [split [read $paramFile] "\n"]
        close $paramFile;

        foreach line $lines {
            puts $line
            append params " " "\{ [lindex [split $line =] 0] " " [lindex [split $line =] 1] \}"
        }

    }

    append params "" "\}"
    puts "$params"
    return params
}

