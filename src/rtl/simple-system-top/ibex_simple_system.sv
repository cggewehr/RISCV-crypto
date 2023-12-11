// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

/**
 * Ibex simple system
 *
 * This is a basic system consisting of an ibex, a 1 MB sram for instruction/data
 * and a small memory mapped control module for outputting ASCII text and
 * controlling/halting the simulation from the software running on the ibex.
 *
 * It is designed to be used with verilator but should work with other
 * simulators, a small amount of work may be required to support the
 * simulator_ctrl module.
 */

module ibex_simple_system import ibex_pkg::*; #(

  // parameter bit                 SecureIbex               = 1'b0,
  parameter bit                 SecureIbex               = 1'b1,
  parameter bit                 ICacheScramble           = 1'b0,
  parameter bit                 PMPEnable                = 1'b0,
  parameter int unsigned        PMPGranularity           = 0,
  parameter int unsigned        PMPNumRegions            = 4,
  parameter int unsigned        MHPMCounterNum           = 0,
  parameter int unsigned        MHPMCounterWidth         = 40,
  parameter bit                 RV32E                    = 1'b0,
  parameter ibex_pkg::rv32m_e   RV32M                    = RV32MFast,
  parameter ibex_pkg::rv32b_e   RV32B                    = RV32BNone,
  parameter ibex_pkg::regfile_e RegFile                  = RegFileLatch,
  parameter bit                 BranchTargetALU          = 1'b0,
  parameter bit                 WritebackStage           = 1'b0,
  parameter bit                 ICache                   = 1'b0,
  parameter bit                 DbgTriggerEn             = 1'b0,
  parameter bit                 ICacheECC                = 1'b0,
  parameter bit                 BranchPredictor          = 1'b0,
  parameter string              SRAMInitFile             = ""

) (

  input IO_CLK,
  input IO_RST_N

);

  logic clk_sys /*= 1'b0*/;
  logic rst_sys_n;

  typedef enum logic {CoreD} bus_host_e;
  typedef enum logic[1:0] {Ram, SimCtrl, Timer} bus_device_e;

  localparam int NrDevices = 3;
  localparam int NrHosts = 1;

  // interrupts
  logic timer_irq;

  // host and device signals
  logic           host_awvalid   [NrHosts];
  logic           host_arvalid   [NrHosts];
  logic           host_rready    [NrHosts];
  logic           host_wvalid    [NrHosts];
  logic [31:0]    host_araddr    [NrHosts];
  logic [ 3:0]    host_wstrb     [NrHosts];
  logic [31:0]    host_wdata     [NrHosts];
  logic           host_rvalid    [NrHosts];
  logic [31:0]    host_rdata     [NrHosts];
  logic           host_awready     [NrHosts];
  logic           host_wready      [NrHosts];
  logic [31:0]    host_awaddr      [NrHosts];
  logic           host_arready      [NrHosts];
  logic           host_bvalid      [NrHosts];
  logic           host_bready      [NrHosts];
  logic [1:0]     host_bresp     [NrHosts];
  logic [1:0]     host_rresp     [NrHosts];
  logic [2:0]     host_arprot    [NrHosts];
  logic [2:0]     host_awprot    [NrHosts];


  logic [6:0]     data_rdata_intg;
  logic [6:0]     instr_rdata_intg;

  // devices (slaves)
  logic           device_awvalid   [NrDevices];
  logic           device_arvalid   [NrDevices];
  logic           device_rready    [NrDevices];
  logic           device_wvalid    [NrDevices];
  logic [31:0]    device_araddr    [NrDevices];
  logic [ 3:0]    device_wstrb     [NrDevices];
  logic [31:0]    device_wdata     [NrDevices];
  logic           device_rvalid    [NrDevices];
  logic [31:0]    device_rdata     [NrDevices];
  logic           device_awready   [NrDevices];
  logic           device_wready    [NrDevices];
  logic [31:0]    device_awaddr    [NrDevices];
  logic           device_arready   [NrDevices];
  logic           device_bvalid    [NrDevices];
  logic           device_bready    [NrDevices];
  logic [1:0]     device_bresp     [NrDevices];
  logic [1:0]     device_rresp     [NrDevices];
  logic [2:0]     device_arprot    [NrDevices];
  logic [2:0]     device_awprot    [NrDevices];

  // Device address mapping
  logic [31:0] cfg_device_addr_base [NrDevices];
  logic [31:0] cfg_device_addr_mask [NrDevices];
  assign cfg_device_addr_base[Ram] = 32'h100000;
  assign cfg_device_addr_mask[Ram] = ~32'hFFFFF; // 1 MB
  assign cfg_device_addr_base[SimCtrl] = 32'h20000;
  assign cfg_device_addr_mask[SimCtrl] = ~32'h3FF; // 1 kB
  assign cfg_device_addr_base[Timer] = 32'h30000;
  assign cfg_device_addr_mask[Timer] = ~32'h3FF; // 1 kB

  // Instruction fetch signals
  logic instr_req;
  logic instr_gnt;
  logic instr_rvalid;
  logic [31:0] instr_addr;
  logic [31:0] instr_rdata;
  logic instr_err;

  assign instr_gnt = instr_req;
  assign instr_err = '0;

  // `ifdef VERILATOR
    assign clk_sys = IO_CLK;
    assign rst_sys_n = IO_RST_N;
  // `else
    // initial begin
      // rst_sys_n = 1'b0;
      // #8
      // rst_sys_n = 1'b1;
    // end
    // always begin
      // #1 clk_sys = 1'b0;
      // #1 clk_sys = 1'b1;
    // end
  // `endif

  // Tie-off unused error signals
  //assign device_err[Ram] = 1'b0;
  //assign device_err[SimCtrl] = 1'b0;

  bus_axi #(
    .NrDevices    ( NrDevices ),
    .NrHosts      ( NrHosts   ),
    .DataWidth    ( 32        ),
    .AddressWidth ( 32        )
  ) u_bus (
    .clk_i               (clk_sys),
    .rst_ni              (rst_sys_n),
    .m_awvalid          (host_awvalid),
    .m_arvalid          (host_arvalid),
    .m_rready           (host_rready),
    .m_awready          (host_awready),
    .m_wready           (host_wready),
    .m_awaddr           (host_awaddr),
    .m_araddr           (host_araddr),
    .m_wstrb            (host_wstrb),
    .m_wdata            (host_wdata),
    .m_wvalid           (host_wvalid),
    .m_arready          (host_arready),
    .m_rvalid           (host_rvalid),
    .m_rdata            (host_rdata),
    .m_bvalid           (host_bvalid),
    .m_bready           (host_bready),
    .m_bresp            (host_bresp),
    .m_rresp            (host_rresp),
    .m_awprot           (host_awprot),
    .m_arprot           (host_arprot),

    .s_awvalid          (device_awvalid),
    .s_arvalid          (device_arvalid),
    .s_rready           (device_rready),
    .s_awready          (device_awready),
    .s_wready           (device_wready),
    .s_awaddr           (device_awaddr),
    .s_araddr           (device_araddr),
    .s_wstrb            (device_wstrb),
    .s_wdata            (device_wdata),
    .s_wvalid           (device_wvalid),
    .s_arready          (device_arready),
    .s_rvalid           (device_rvalid),
    .s_rdata            (device_rdata),
    .s_bvalid           (device_bvalid), 
    .s_bready           (device_bready),
    .s_bresp            (device_bresp),
    .s_rresp            (device_rresp),
    .s_awprot           (device_awprot),
    .s_arprot           (device_arprot),

    .cfg_device_addr_base,
    .cfg_device_addr_mask
  );

  if (SecureIbex) begin : g_mem_rdata_ecc

    logic [31:0] unused_data_rdata;
    logic [31:0] unused_instr_rdata;

    prim_secded_inv_39_32_enc u_data_rdata_intg_gen (
      .data_i (host_rdata[CoreD]),
      .data_o ({data_rdata_intg, unused_data_rdata})
    );

    prim_secded_inv_39_32_enc u_instr_rdata_intg_gen (
      .data_i (instr_rdata),
      .data_o ({instr_rdata_intg, unused_instr_rdata})
    );
  end else begin : g_no_mem_rdata_ecc
    assign data_rdata_intg = '0;
    assign instr_rdata_intg = '0;
  end

  ibex_top_tracing #(
      .SecureIbex      ( SecureIbex       ),
      .ICacheScramble  ( ICacheScramble   ),
      .PMPEnable       ( PMPEnable        ),
      .PMPGranularity  ( PMPGranularity   ),
      .PMPNumRegions   ( PMPNumRegions    ),
      .MHPMCounterNum  ( MHPMCounterNum   ),
      .MHPMCounterWidth( MHPMCounterWidth ),
      .RV32E           ( RV32E            ),
      .RV32M           ( RV32M            ),
      .RV32B           ( RV32B            ),
      .RegFile         ( RegFile          ),
      .BranchTargetALU ( BranchTargetALU  ),
      .ICache          ( ICache           ),
      .ICacheECC       ( ICacheECC        ),
      .WritebackStage  ( WritebackStage   ),
      .BranchPredictor ( BranchPredictor  ),
      .DbgTriggerEn    ( DbgTriggerEn     ),
      .DmHaltAddr      ( 32'h00100000     ),
      .DmExceptionAddr ( 32'h00100000     )
  ) u_top (
    .clk_i                  (clk_sys),
    .rst_ni                 (rst_sys_n),

    .test_en_i              (1'b0),
    .scan_rst_ni            (1'b1),
    .ram_cfg_i              (10'b0),

    .hart_id_i              (32'b0),
    // First instruction executed is at 0x0 + 0x80
    .boot_addr_i            (32'h00100000),

    .instr_req_o            (instr_req),
    .instr_gnt_i            (instr_gnt),
    .instr_rvalid_i         (instr_rvalid),
    .instr_addr_o           (instr_addr),
    .instr_rdata_i          (instr_rdata),
    .instr_rdata_intg_i     (instr_rdata_intg),
    .instr_err_i            (instr_err),

    .data_awvalid_o          (host_awvalid[CoreD]),
    .data_arvalid_o          (host_arvalid[CoreD]),
    .data_rready_o           (host_rready[CoreD]),
    .data_awready_i          (host_awready[CoreD]),
    .data_wready_i           (host_wready[CoreD]),
    .data_awaddr_o           (host_awaddr[CoreD]),
    .data_araddr_o           (host_araddr[CoreD]),
    .data_wstrb_o            (host_wstrb[CoreD]),
    .data_wdata_o            (host_wdata[CoreD]),
    .data_wvalid_o           (host_wvalid[CoreD]),
    .data_arready_i          (host_arready[CoreD]),
    .data_rvalid_i           (host_rvalid[CoreD]),
    .data_rdata_i            (host_rdata[CoreD]),
    .data_bvalid_i           (host_bvalid[CoreD]),
    .data_bready_o           (host_bready[CoreD]),
    .data_bresp_i            (host_bresp[CoreD]),
    .data_rresp_i            (host_rresp[CoreD]),
    .data_arprot_o           (host_arprot[CoreD]),
    .data_awprot_o           (host_awprot[CoreD]),

    .data_wdata_intg_o      (),
    .data_rdata_intg_i      (data_rdata_intg),

    .irq_software_i         (1'b0),
    .irq_timer_i            (timer_irq),
    .irq_external_i         (1'b0),
    .irq_fast_i             (15'b0),
    .irq_nm_i               (1'b0),

    .scramble_key_valid_i   ('0),
    .scramble_key_i         ('0),
    .scramble_nonce_i       ('0),
    .scramble_req_o         (),

    .debug_req_i            (1'b0),
    .crash_dump_o           (),
    .double_fault_seen_o    (),

    .fetch_enable_i         (ibex_pkg::FetchEnableOn),
    .alert_minor_o          (),
    .alert_major_internal_o (),
    .alert_major_bus_o      (),
    .core_sleep_o           ()
  );

  // SRAM block for instruction and data storage
  ram_2p #(
      .Depth(1024*1024/4),
      .MemInitFile(SRAMInitFile)
  ) u_ram (
    .clk_i       (clk_sys),
    .rst_ni      (rst_sys_n),

    .a_awvalid_i (device_awvalid[Ram]),
    .a_arvalid_i (device_arvalid[Ram]),
    .a_rready_i  (device_rready [Ram]),
    .a_wvalid_i  (device_wvalid [Ram]),
    .a_be_i      (device_wstrb  [Ram]),
    .a_araddr_i  (device_araddr [Ram]),
    .a_awaddr_i  (device_awaddr [Ram]),
    .a_wdata_i   (device_wdata  [Ram]),
    .a_rvalid_o  (device_rvalid [Ram]),
    .a_rdata_o   (device_rdata  [Ram]),
    .a_arready_o (device_arready[Ram]),
    .a_wready_o  (device_wready [Ram]),
    .a_awready_o (device_awready[Ram]),
    .a_bvalid_o  (device_bvalid [Ram]),
    .a_bready_i  (device_bready [Ram]),
    .a_bresp_o   (device_bresp  [Ram]),
    .a_rresp_o   (device_rresp  [Ram]),
    .a_arprot_i  (device_arprot [Ram]),
    .a_awprot_i  (device_awprot [Ram]),

    .b_req_i     (instr_req),
    .b_we_i      (1'b0),
    .b_be_i      (4'b0),
    .b_addr_i    (instr_addr),
    .b_wdata_i   (32'b0),
    .b_rvalid_o  (instr_rvalid),
    .b_rdata_o   (instr_rdata)
  );

  timer #(    
    .DataWidth    (32),
    .AddressWidth (32)
  ) u_timer (
    .clk_i       (clk_sys),
    .rst_ni      (rst_sys_n),

    .timer_awvalid_i (device_awvalid[Timer]),
    .timer_arvalid_i (device_arvalid[Timer]),
    .timer_rready_i  (device_rready [Timer]),
    .timer_wvalid_i  (device_wvalid [Timer]),
    .timer_be_i      (device_wstrb  [Timer]),
    .timer_araddr_i  (device_araddr [Timer]),
    .timer_awaddr_i  (device_awaddr [Timer]),
    .timer_wdata_i   (device_wdata  [Timer]),
    .timer_rvalid_o  (device_rvalid [Timer]),
    .timer_rdata_o   (device_rdata  [Timer]),
    .timer_arready_o (device_arready[Timer]),
    .timer_wready_o  (device_wready [Timer]),
    .timer_awready_o (device_awready[Timer]),
    .timer_bvalid_o  (device_bvalid [Timer]),
    .timer_bready_i  (device_bready [Timer]),
    .timer_bresp_o   (device_bresp  [Timer]),
    .timer_rresp_o   (device_rresp  [Timer]),
    .timer_arprot_i  (device_arprot [Timer]),
    .timer_awprot_i  (device_awprot [Timer]),
    .timer_intr_o    (timer_irq)
  );

endmodule
