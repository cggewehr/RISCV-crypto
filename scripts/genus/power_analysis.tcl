set DB_FILE $::env(POWER_ANALYSIS_DB_FILE)
set SHM_FILE $::env(POWER_ANALYSIS_SHM_FILE)
set REPORT_DIR $::env(POWER_ANALYSIS_REPORT_DIR)
set REPORT_NAME $::env(POWER_ANALYSIS_REPORT_NAME)
set START_TIME $::env(POWER_ANALYSIS_START_TIME)
set END_TIME $::env(POWER_ANALYSIS_END_TIME)

read_db ${DB_FILE}
read_stimulus -dut_instance /tb_top/u_ibex_simple_system/u_top/u_ibex_top -start ${START_TIME} -start ${END_TIME} ${SHM_FILE}
report_power -unit mW > ${REPORT_DIR}/rpt_power_${REPORT_NAME}.txt

exit

