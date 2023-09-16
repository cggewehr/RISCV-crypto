// VCS does not support overriding enum and string parameters via command line. Instead, a `define
// is used that can be set from the command line. If no value has been specified, this gives a
// default. Other simulators don't take the detour via `define and can override the corresponding
// parameters directly.

`ifndef RV32M
  `define RV32M ibex_pkg::RV32MFast
`endif

`ifndef RV32B
  `define RV32B ibex_pkg::RV32BCrypto
`endif

`ifndef RegFile
  `define RegFile ibex_pkg::RegFileLatch
`endif

// Default clock frequency to 500 MHz
`ifndef ClockPeriod
  //`define ClockPeriod 2ns
  `define ClockPeriod 2
`endif

module tb_top #(

  // parameter bit                  SecureIbex       = 1'b0,
  parameter bit                  SecureIbex       = 1'b1,
  parameter bit                  ICacheScramble   = 1'b0,
  parameter bit                  PMPEnable        = 1'b0,
  parameter int unsigned         PMPGranularity   = 0,
  parameter int unsigned         PMPNumRegions    = 4,
  `ifdef SYNTHESIS
  parameter int unsigned         MHPMCounterNum   = 0,
  `else
  parameter int unsigned         MHPMCounterNum   = 18,
  `endif
  parameter int unsigned         MHPMCounterWidth = 40,
  parameter bit                  RV32E            = 1'b0,
  parameter ibex_pkg::rv32m_e    RV32M            = `RV32M,
  parameter ibex_pkg::rv32b_e    RV32B            = `RV32B,
  parameter ibex_pkg::regfile_e  RegFile          = `RegFile,
  parameter bit                  BranchTargetALU  = 1'b0,
  parameter bit                  WritebackStage   = 1'b0,
  parameter bit                  ICache           = 1'b0,
  parameter bit                  DbgTriggerEn     = 1'b0,
  parameter bit                  ICacheECC        = 1'b0,
  parameter bit                  BranchPredictor  = 1'b0,
  parameter                      SRAMInitFile     = "MemFile.vmem",
  parameter                      SymbolAddrs      = "sw_build/symbol_table.txt"

) ( );
  
  bit clk;
  bit rst_n;

  class symbol_info_t;
    string function_name;
    int times_called = 0;
    realtime start_times[$];
    realtime end_times[$];
    int start_addr;
    int end_addr;
  endclass
  
  class counters_info_t;
    string function_name;
    int start_addr;
    int ret_addr;
    longint unsigned counters[$];
  endclass

  symbol_info_t symbol_info[int];  // Indexed by start_addrs
  
  typedef enum logic {CoreD} bus_host_e;
  typedef enum logic[1:0] {Ram, SimCtrl, Timer} bus_device_e;

  initial begin
    
    clk <= 1'b0;
    
    forever
      #(`ClockPeriod/2 * 1ns) clk <= ~clk;

  end
  
  initial begin
  
    rst_n <= 1'b1;
    
    repeat (10)
      #`ClockPeriod
  
    rst_n <= 1'b0;
    
    repeat (10)
      #`ClockPeriod
  
    rst_n <= 1'b1;
    
  end

  ibex_simple_system #(
    .SecureIbex      (SecureIbex      ),           
    .ICacheScramble  (ICacheScramble  ),           
    .PMPEnable       (PMPEnable       ),           
    .PMPGranularity  (PMPGranularity  ),           
    .PMPNumRegions   (PMPNumRegions   ),           
    .MHPMCounterNum  (MHPMCounterNum  ),           
    .MHPMCounterWidth(MHPMCounterWidth),           
    .RV32E           (RV32E           ),           
    .RV32M           (RV32M           ),           
    .RV32B           (RV32B           ),           
    .RegFile         (RegFile         ),           
    .BranchTargetALU (BranchTargetALU ),           
    .WritebackStage  (WritebackStage  ),           
    .ICache          (ICache          ),           
    .DbgTriggerEn    (DbgTriggerEn    ),           
    .ICacheECC       (ICacheECC       ),           
    .BranchPredictor (BranchPredictor ),           
    .SRAMInitFile    (SRAMInitFile    )             
  ) u_ibex_simple_system (
    .IO_CLK(clk),
    .IO_RST_N(rst_n)
  );

  simulator_ctrl #(
    .LogName("log/ibex_simple_system.log")
  ) u_simulator_ctrl (
    .clk_i     (u_ibex_simple_system.clk_sys),
    .rst_ni    (u_ibex_simple_system.rst_sys_n),

    .req_i     (u_ibex_simple_system.device_req[SimCtrl]),
    .we_i      (u_ibex_simple_system.device_we[SimCtrl]),
    .be_i      (u_ibex_simple_system.device_be[SimCtrl]),
    .addr_i    (u_ibex_simple_system.device_addr[SimCtrl]),
    .wdata_i   (u_ibex_simple_system.device_wdata[SimCtrl]),
    .rvalid_o  (u_ibex_simple_system.device_rvalid[SimCtrl]),
    .rdata_o   (u_ibex_simple_system.device_rdata[SimCtrl])
  );

  `ifndef NETLIST
  initial begin

    symbol_info_t current_symbol;
    counters_info_t counters;
    int current_ret_addr;
    int prev_addr;

    counters_info_t saved_counters [$];

    realtime start_time;
    realtime end_time;
    int start_addr, end_addr;

    int fd;
    string line;

    string ctr_reset_exclusion_list[] = {"tc_sha256_update", "tc_sha256_final", "tc_sha512_update", "tc_sha512_final"};

    $timeformat(-9, 2, " ns");

    fd = $fopen(SymbolAddrs, "r");

    // Parse symbol table
    while (!$feof(fd)) begin

      symbol_info_t new_symbol;
      new_symbol = new;

      $fgets(line, fd);

      if (line != "") begin

        $sscanf(line, "%s %h\n", new_symbol.function_name, new_symbol.start_addr);

        symbol_info[new_symbol.start_addr] = new_symbol;

        $display($sformatf("[Profiler] Parsed symbol <%s> start_addr <%0h>", new_symbol.function_name, new_symbol.start_addr));

      end

    end

    foreach (symbol_info[i])
      $display("[Profiler] %0d: %p", i, symbol_info[i]);

    forever begin
      // Monitor PC register for start addresses in symbol table

        @(posedge u_ibex_simple_system.clk_sys);
        if (symbol_info.exists(u_ibex_simple_system.u_top.u_ibex_top.rvfi_pc_wdata)) begin

          current_symbol = symbol_info[u_ibex_simple_system.u_top.u_ibex_top.rvfi_pc_wdata];

          symbol_info[current_symbol.start_addr].start_times.push_back($realtime());

          $display("[Profiler] Fetched start of symbol <%s> addr <%0h> at <%0t> <%0h>", current_symbol.function_name, u_ibex_simple_system.u_top.u_ibex_top.rvfi_pc_rdata, $realtime(), u_ibex_simple_system.u_top.u_ibex_top.rvfi_insn);

          counters = new;
          counters.function_name = current_symbol.function_name;
          counters.counters = u_ibex_simple_system.u_top.u_ibex_top.u_ibex_core.cs_registers_i.mhpmcounter;
          counters.start_addr = current_symbol.start_addr;

          if (u_ibex_simple_system.u_top.u_ibex_top.rvfi_insn[1:0] == 2'b11)
            counters.ret_addr = u_ibex_simple_system.u_top.u_ibex_top.rvfi_pc_rdata + 4;
          else
            counters.ret_addr = u_ibex_simple_system.u_top.u_ibex_top.rvfi_pc_rdata + 2;
            $display("%h", counters.ret_addr);

          if (!(current_symbol.function_name inside {ctr_reset_exclusion_list})) begin
            saved_counters.push_front(counters);
            print_counters();
          end

          continue;

        end

          if (saved_counters[0]) begin
                start_addr = saved_counters[0].start_addr;

                if (saved_counters[0].ret_addr == u_ibex_simple_system.u_top.u_ibex_top.rvfi_pc_wdata) begin

                  symbol_info[start_addr].end_times.push_back($realtime());
                  symbol_info[start_addr].times_called++;

                  $display("[Profiler] Fetched end of symbol <%s> addr <%0h> at <%0t> <%0h>", saved_counters[0].function_name, u_ibex_simple_system.u_top.u_ibex_top.rvfi_pc_rdata, $realtime(), u_ibex_simple_system.u_top.u_ibex_top.rvfi_insn);
                  print_counters_nested(saved_counters.pop_front());
                  continue;

                end

          end

    end

  end
  `endif

  `ifdef NETLIST
  initial begin
    $shm_open("deliverables/shm/netlist_sim.shm");
    $shm_probe(u_ibex_simple_system.u_top.u_ibex_top, "ASM");
  end
  `endif

  `ifndef NETLIST
  function automatic longint unsigned mhpmcounter_get(int index);
    return u_ibex_simple_system.u_top.u_ibex_top.u_ibex_core.cs_registers_i.mhpmcounter[index];
  endfunction

  function void print_counters();
    
    string reg_names[] = {"Cycles", "NONE", "Instructions Retired", "LSU Busy", "Fetch Wait", "Loads", "Stores", "Jumps", "Conditional Branches", "Taken Conditional Branches", "Compressed Instructions", "Multiplier Busy", "Divider Busy", "Memory store word", "Memory store half word", "Memory store byte", "Memory load word", "Memory load half word", "Memory load byte"};

    $display("====================");
    $display("Performance Counters");
    $display("====================");

    foreach (reg_names[reg_index])
      $display("%s: %0d", reg_names[reg_index], mhpmcounter_get(reg_index));
    
    $display("====================");

  endfunction

  
  function void print_counters_nested(counters_info_t count);

    string reg_names[] = {"Cycles", "NONE", "Instructions Retired", "LSU Busy", "Fetch Wait", "Loads", "Stores", "Jumps", "Conditional Branches", "Taken Conditional Branches", "Compressed Instructions", "Multiplier Busy", "Divider Busy", "Memory store word", "Memory store half word", "Memory store byte", "Memory load word", "Memory load half word", "Memory load byte"};

    $display("====================");
    $display("Performance Counters");
    $display("====================");

    foreach (reg_names[reg_index])
      $display("%s: %0d", reg_names[reg_index], mhpmcounter_get(reg_index)-count.counters[reg_index]);
    
    $display("====================");

  endfunction

  task reset_counters();

    string reg_names[] = {"Cycles", "NONE", "Instructions Retired", "LSU Busy", "Fetch Wait", "Loads", "Stores", "Jumps", "Conditional Branches", "Taken Conditional Branches", "Compressed Instructions", "Multiplier Busy", "Divider Busy"};

    $display("Reseting Performance Counters");

    foreach (reg_names[reg_index])
      $xm_deposit($sformatf("tb_top.u_ibex_simple_system.u_top.u_ibex_top.u_ibex_core.cs_registers_i.gen_cntrs[%0d].gen_imp.mcounters_variable_i.counter_d[31:0]", reg_index), "32'd0");

    $xm_deposit("tb_top.u_ibex_simple_system.u_top.u_ibex_top.u_ibex_core.cs_registers_i.mcycle_counter_i.counter_d[31:0]", "32'd0"); 
    $xm_deposit("tb_top.u_ibex_simple_system.u_top.u_ibex_top.u_ibex_core.cs_registers_i.minstret_counter_i.counter_d[31:0]", "32'd0"); 
 
  endtask

  `endif

  final begin

    int fd;

    `ifndef NETLIST
      $display("====================");
      $display("End of Functional Simulation");
      $display("====================");
      print_counters();
    `else
      $display("====================");
      $display("End of Netlist Simulation");
      $display("====================");
    `endif

    `ifndef NETLIST
      $display("====================");
      $display("Profiling Results");
      $display("FunctionName_TimesCalled | StartTime (ns) | EndTime (ns) | ExecutionTime (ns)");
      $display("====================");

      $timeformat(-9, 2, "");

      // Print out profiling file
      fd = $fopen("deliverables/symbol_profiling.txt", "w");
      $fdisplay(fd, "FunctionName_TimesCalled | StartTime (ns) | EndTime (ns) | ExecutionTime (ns)");

      foreach (symbol_info[i]) begin

        $display("%0d: %p", i, symbol_info[i]);

        for (int j = 0; j < symbol_info[i].times_called; j++) begin

          string line;
          line = $sformatf("%s_%0d %t %t %t", symbol_info[i].function_name, j, symbol_info[i].start_times[j], 
            symbol_info[i].end_times[j], symbol_info[i].end_times[j] - symbol_info[i].start_times[j]);
        
          $display(line);
          $fdisplay(fd, line);

        end

      end
      `endif

  end

endmodule
