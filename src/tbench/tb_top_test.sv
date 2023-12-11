module tb_top_test #(
  parameter                      SRAMInitFile     = "MemFile.vmem"
) ();

  localparam int NrDevices = 2;
  localparam int NrHosts = 1;

  logic [31:0] cfg_device_addr_base [NrDevices];
  logic [31:0] cfg_device_addr_mask [NrDevices];
  assign cfg_device_addr_base[0] = 32'h100000;
  assign cfg_device_addr_mask[0] = ~32'hFFFFF; // 1 MB
  assign cfg_device_addr_base[1] = 32'h30000;
  assign cfg_device_addr_mask[1] = ~32'h3FF; // 1 kB

  logic clk_sys, rst_sys_n;
  

  // host and device signals
  logic           m_awvalid   [NrHosts];
  logic           m_arvalid   [NrHosts];
  logic           m_rready    [NrHosts];
  logic           m_wvalid    [NrHosts];

  logic [31:0]    m_araddr    [NrHosts];
  logic [ 3:0]    m_wstrb     [NrHosts];
  logic [31:0]    m_wdata     [NrHosts];

  logic           m_rvalid    [NrHosts];
  logic [31:0]    m_rdata     [NrHosts];

  logic        m_awready     [NrHosts];
  logic        m_wready      [NrHosts];
  logic [31:0] m_awaddr      [NrHosts];
  logic        m_arready      [NrHosts];
  logic        m_bvalid      [NrHosts];
  logic        m_bready      [NrHosts];

  // devices (slaves)
  logic           s_awvalid   [NrDevices];
  logic           s_arvalid   [NrDevices];
  logic           s_rready    [NrDevices];
  logic           s_wvalid    [NrDevices];

  logic [31:0]    s_araddr    [NrDevices];
  logic [ 3:0]    s_wstrb     [NrDevices];
  logic [31:0]    s_wdata     [NrDevices];
  
  logic           s_rvalid    [NrDevices];
  logic [31:0]    s_rdata     [NrDevices];

  logic        s_awready     [NrDevices];
  logic        s_wready      [NrDevices];
  logic [31:0] s_awaddr      [NrDevices];
  logic        s_arready     [NrDevices];
  logic        s_bvalid      [NrDevices];
  logic        s_bready      [NrDevices];

  logic timer_irq;

  initial begin
    
    clk_sys <= 1'b0;
    
    forever
      #2 clk_sys <= ~clk_sys;

  end

  initial begin
    rst_sys_n <= 1'b0;
    #10;
    rst_sys_n <= 1'b1;
    @(posedge clk_sys);
    m_rready[0] <= 1'b0;
    m_arvalid[0] <= 1'b0;
    m_awaddr[0] <= 32'h100008;
    m_araddr[0] <= 32'h30000;
    m_wdata[0] <= 'h12345678;
    m_wstrb[0] <= 4'b1111;

    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    #4;
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;

    #20;
    @(posedge clk_sys);
    m_rready[0] <= 1'b1;
    m_arvalid[0] <= 1'b1;
    #4;
    @(posedge clk_sys);
    m_rready[0] <= 1'b0;
    m_arvalid[0] <= 1'b0;
    #12;
    @(posedge clk_sys);
    m_rready[0] <= 1'b1;
    m_arvalid[0] <= 1'b1;
    m_araddr[0] <= 32'h100008;
    #4;
    @(posedge clk_sys);
    m_rready[0] <= 1'b0;
    m_arvalid[0] <= 1'b0;


  end
  

  bus_axi #(
    .NrDevices    ( NrDevices ),
    .NrHosts      ( NrHosts   ),
    .DataWidth    ( 32        ),
    .AddressWidth ( 32        )
  ) u_bus (
    .clk_i               (clk_sys),
    .rst_ni              (rst_sys_n),

    .m_awvalid          (m_awvalid),
    .m_arvalid          (m_arvalid),
    .m_rready           (m_rready),
    .m_awready          (m_awready),
    .m_wready           (m_wready),
    .m_awaddr           (m_awaddr),
    .m_araddr           (m_araddr),
    .m_wstrb            (m_wstrb),
    .m_wdata            (m_wdata),
    .m_wvalid           (m_wvalid),
    .m_arready          (m_arready),
    .m_rvalid           (m_rvalid),
    .m_rdata            (m_rdata),
    .m_bvalid           (m_bvalid),
    .m_bready           (m_bready),

    .s_awvalid          (s_awvalid),
    .s_arvalid          (s_arvalid),
    .s_rready           (s_rready),
    .s_awready          (s_awready),
    .s_wready           (s_wready),
    .s_awaddr           (s_awaddr),
    .s_araddr           (s_araddr),
    .s_wstrb            (s_wstrb),
    .s_wdata            (s_wdata),
    .s_wvalid           (s_wvalid),
    .s_arready          (s_arready),
    .s_rvalid           (s_rvalid),
    .s_rdata            (s_rdata),
    .s_bvalid           (s_bvalid), 
    .s_bready           (s_bready),

    .cfg_device_addr_base,
    .cfg_device_addr_mask
  );

  logic b_rvalid_o, b_rdata_o;

  ram_2p #(
      .Depth(1024*1024/4),
      .MemInitFile(SRAMInitFile)
  ) u_ram (
    .clk_i       (clk_sys),
    .rst_ni      (rst_sys_n),

    .a_awvalid_i (s_awvalid[0]),
    .a_arvalid_i (s_arvalid[0]),
    .a_rready_i  (s_rready[0]),
    .a_wvalid_i    (s_wvalid[0]),
    .a_be_i      (s_wstrb[0]),
    .a_araddr_i    (s_araddr[0]),
    .a_awaddr_i    (s_awaddr[0]),
    .a_wdata_i   (s_wdata[0]),
    .a_rvalid_o  (s_rvalid[0]),
    .a_rdata_o   (s_rdata[0]),
    .a_arready_o (s_arready[0]),
    .a_wready_o  (s_wready[0]),
    .a_awready_o (s_awready[0]),
    .a_bvalid_o  (s_bvalid[0]),
    .a_bready_i  (s_bready[0]),

    .b_be_i      (4'b0),
    .b_addr_i    (32'b0),
    .b_wdata_i   (32'b0),
    .b_rvalid_o  (),
    .b_rdata_o   ()
  );

  timer #(    
    .DataWidth    (32),
    .AddressWidth (32)
  ) u_timer (
    .clk_i       (clk_sys),
    .rst_ni      (rst_sys_n),

    .timer_awvalid_i (s_awvalid[1]),
    .timer_arvalid_i (s_arvalid[1]),
    .timer_rready_i  (s_rready[1]),
    .timer_wvalid_i    (s_wvalid[1]),
    .timer_be_i      (s_wstrb[1]),
    .timer_araddr_i    (s_araddr[1]),
    .timer_awaddr_i    (s_awaddr[1]),
    .timer_wdata_i   (s_wdata[1]),
    .timer_rvalid_o  (s_rvalid[1]),
    .timer_rdata_o   (s_rdata[1]),
    .timer_arready_o (s_arready[1]),
    .timer_wready_o  (s_wready[1]),
    .timer_awready_o (s_awready[1]),
    .timer_bvalid_o  (s_bvalid[1]),
    .timer_bready_i  (s_bready[1]),
    .timer_intr_o     (timer_irq)
  );



endmodule