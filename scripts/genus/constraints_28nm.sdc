set sdc_version 2.0
set_units -capacitance pF -time ns

# 500 MHz freq
set CLOCK_PERIOD 2

create_clock -period ${CLOCK_PERIOD} -name main_clock [get_ports clk_i];  # 500 MHz clock

#set_clock_uncertainty [expr $CLOCK_PERIOD * 0.05] [get_clocks]
#set_clock_latency [expr $CLOCK_PERIOD * 0.05] [get_clocks]

set_input_delay -clock main_clock [expr ${CLOCK_PERIOD} * 0.15] [remove_from_collection [all_inputs] [get_ports {clk_i rst_ni}]]
set_output_delay -clock main_clock [expr ${CLOCK_PERIOD} * 0.15] [all_outputs]

set_ideal_network [get_ports clk_i rst_ni]

# Output pins should support to drive a load of an inverter
set_load 0.000570 [all_outputs]
