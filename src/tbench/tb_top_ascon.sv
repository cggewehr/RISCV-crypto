module tb_top_ascon #(
  parameter                      SRAMInitFile     = "MemFile.vmem"
) ();

  localparam int NrDevices = 1;
  localparam int NrHosts = 1;

  logic [31:0] cfg_device_addr_base [NrDevices];
  logic [31:0] cfg_device_addr_mask [NrDevices];
  assign cfg_device_addr_base[0] = 32'h100000;
  assign cfg_device_addr_mask[0] = ~32'hFFFFF; // 1 MB


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
  logic [1:0]     m_bresp     [NrHosts];
  logic [1:0]     m_rresp     [NrHosts];
  logic [2:0]     m_arprot    [NrHosts];
  logic [2:0]     m_awprot    [NrHosts];


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
  logic [1:0]     s_bresp     [NrDevices];
  logic [1:0]     s_rresp     [NrDevices];
  logic [2:0]     s_arprot    [NrDevices];
  logic [2:0]     s_awprot    [NrDevices];

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

    // key
    @(posedge clk_sys);
    m_rready[0] <= 1'b0;
    m_arvalid[0] <= 1'b0;
    m_awaddr[0] <= 32'h100005;
    m_wdata[0] <= 'h9D79B1A3;
    m_wstrb[0] <= 4'b1111;
    m_rready[0] <= 1'b1;

    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    #8;
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    #8;
    @(posedge clk_sys);
    m_wdata[0] <= 'h7F31801C;
    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    #8;
    @(posedge clk_sys);
    m_wdata[0] <= 'hD11A6706;
    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    #8;
    @(posedge clk_sys);
    m_wdata[0] <= 'hFB40D6BD;
    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    #8;
    // nonce
    @(posedge clk_sys);
    m_wdata[0] <= 'h57526846;
    m_awaddr[0] <= 32'h100000;
    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    #8;
    @(posedge clk_sys);
    m_wdata[0] <= 'h903BB13E;
    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    #8;
    @(posedge clk_sys);
    m_wdata[0] <= 'hDE562439;
    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    #8;
    @(posedge clk_sys);
    m_wdata[0] <= 'hE9C1B823;
    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    @(posedge clk_sys)
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    // ad
    @(posedge clk_sys);
    m_wdata[0] <= 'h1AB3C589;
    m_awaddr[0] <= 32'h100001;
    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    @(posedge m_awready[0]);
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    #8;
    @(posedge clk_sys);
    m_wdata[0] <= 'hE3E64EC6;
    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    @(posedge m_awready[0]);
    @(posedge clk_sys);
    m_wdata[0] <= 'h1F7EC67B;
    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    #8;
    @(posedge clk_sys);
    m_wdata[0] <= 'hF7017780;
    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    @(posedge clk_sys)
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    // plaintext
    @(posedge clk_sys);
    m_wdata[0] <= 'h4FCF816F;
    m_awaddr[0] <= 32'h100002;
    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    @(posedge m_awready[0]);
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    #8;
    @(posedge clk_sys);
    m_wdata[0] <= 'hB65763D3;
    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    #8;
    @(posedge clk_sys);
    m_wdata[0] <= 'hA38824BB;
    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    @(posedge m_awready[0]);
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
    #8;
    @(posedge clk_sys);
    m_wdata[0] <= 'h6AAC9780;
    m_awvalid[0] <= 1'b1;
    m_wvalid[0] <= 1'b1;
    m_bready[0] <= 1'b1;
    @(posedge m_awready[0]);
    @(posedge clk_sys);
    m_awvalid[0] <= 1'b0;
    m_wvalid[0] <= 1'b0;
    m_bready[0] <= 1'b0;
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
    .m_bresp            (m_bresp),
    .m_rresp            (m_rresp),
    .m_awprot           (m_awprot),
    .m_arprot           (m_arprot),

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
    .s_bresp            (s_bresp),
    .s_rresp            (s_rresp),
    .s_awprot           (s_awprot),
    .s_arprot           (s_arprot),

    .cfg_device_addr_base,
    .cfg_device_addr_mask
  );


  ascon_wrapper u_ascon (
    .clk_i       (clk_sys),
    .rst_i      (!rst_sys_n),

    .awvalid_i (s_awvalid[0]),
    .arvalid_i (s_arvalid[0]),
    .rready_i  (s_rready [0]),
    .wvalid_i  (s_wvalid [0]),
    .be_i      (s_wstrb  [0]),
    .araddr_i  (s_araddr [0]),
    .awaddr_i  (s_awaddr [0]),
    .wdata_i   (s_wdata  [0]),
    .rvalid_o  (s_rvalid [0]),
    .rdata_o   (s_rdata  [0]),
    .arready_o (s_arready[0]),
    .wready_o  (s_wready [0]),
    .awready_o (s_awready[0]),
    .bvalid_o  (s_bvalid [0]),
    .bready_i  (s_bready [0]),
    .bresp_o   (s_bresp  [0]),
    .rresp_o   (s_rresp  [0]),
    .arprot_i  (s_arprot [0]),
    .awprot_i  (s_awprot [0])

  );


endmodule