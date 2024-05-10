set DB_FILE $env(POWER_ANALYSIS_DB_FILE)
set SHM_FILE $env(POWER_ANALYSIS_SHM_FILE)
set REPORT_DIR $env(POWER_ANALYSIS_REPORT_DIR)
set REPORT_NAME $env(POWER_ANALYSIS_REPORT_NAME)
set START_TIME $env(POWER_ANALYSIS_START_TIME)
set END_TIME $env(POWER_ANALYSIS_END_TIME)

read_db ${DB_FILE}

# Override liberty read from db above with typical PVT .libs
#set_db library "/soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_12_CORE_LR@2.0@20130411.0/libs/C28SOI_SC_12_CORE_LR_tt28_0.90V_25C.lib \
#                /soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_12_PR_LR@2.0@20130412.0/libs/C28SOI_SC_12_PR_LR_tt28_0.90V_25C.lib \
#                /soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_12_CLK_LR@2.1@20130621.0/libs/C28SOI_SC_12_CLK_LR_tt28_0.90V_25C.lib"

set_db library "/soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_8_CORE_LL@2.1@20131011.0/libs/C28SOI_SC_8_CORE_LL_tt28_0.90V_0.00V_0.00V_0.00V_25C.lib \
                /soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_8_PR_LL@2.1@20131028.1/libs/C28SOI_SC_8_PR_LL_tt28_0.90V_0.00V_0.00V_0.00V_25C.lib \
                /soft64/design-kits/stm/28nm-cmos28fdsoi_25d/C28SOI_SC_8_CLK_LL@2.2@20131011.0/libs/C28SOI_SC_8_CLK_LL_tt28_0.90V_0.00V_0.00V_0.00V_25C.lib"

set_db qrc_tech_file "/soft64/design-kits/stm/28nm-cmos28lp_42/CadenceTechnoKit_cmos028_6U1x_2U2x_2T8x_LB@4.2.1/QRC_TECHFILE/nominal.tech"

set_db interconnect_mode ple

read_stimulus ${SHM_FILE} -dut_instance /tb_top/u_ibex_simple_system/u_top/u_ibex_top -start ${START_TIME} -end ${END_TIME}
compute_power -mode average
report_power -header -unit mW > ${REPORT_DIR}/rpt_power_${REPORT_NAME}.txt

exit

