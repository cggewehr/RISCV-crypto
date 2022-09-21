// VCS does not support overriding enum and string parameters via command line. Instead, a `define
// is used that can be set from the command line. If no value has been specified, this gives a
// default. Other simulators don't take the detour via `define and can override the corresponding
// parameters directly.

`ifndef RV32M
  `define RV32M ibex_pkg::RV32MFast
`endif

`ifndef RV32B
  `define RV32B ibex_pkg::RV32BNone
`endif

`ifndef RegFile
  `define RegFile ibex_pkg::RegFileLatch
`endif

// Default clock frequency to 64 MHz
`ifndef ClockPeriod
  `define ClockPeriod 15.625ns
`endif

module tb_top #(

  parameter bit                  SecureIbex       = 1'b0,
  parameter bit                  ICacheScramble   = 1'b0,
  parameter bit                  PMPEnable        = 1'b0,
  parameter int unsigned         PMPGranularity   = 0,
  parameter int unsigned         PMPNumRegions    = 4,
  parameter int unsigned         MHPMCounterNum   = 0,
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
  parameter                      SRAMInitFile     = "../MemFile.vmem"
  
) ( );
  
  bit clk;
  bit rst_n;
  
  typedef enum logic {CoreD} bus_host_e;
  typedef enum logic[1:0] {Ram, SimCtrl, Timer} bus_device_e;

  initial begin
    
    clk <= 1'b0;
    
    forever
      #`ClockPeriod clk <= ~clk;
  
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

  function automatic longint unsigned mhpmcounter_get(int index);
    return u_ibex_simple_system.u_top.u_ibex_top.u_ibex_core.cs_registers_i.mhpmcounter[index];
  endfunction

  final begin

    string reg_names[] = {"Cycles", "NONE", "Instructions Retired", "LSU Busy", "Fetch Wait", "Loads", "Stores", "Jumps", "Conditional Branches", "Taken Conditional Branches", "Compressed Instructions"};

    $display("Performance Counters");
    $display("====================");

    foreach (reg_names[reg_index])
      $display("%s: %0d", reg_names[reg_index], mhpmcounter_get(reg_index));

  end

endmodule
