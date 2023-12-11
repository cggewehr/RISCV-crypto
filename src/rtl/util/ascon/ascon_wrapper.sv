module ascon_wrapper(
    input               clk_i,
    input               rst_i,

    input               awvalid_i,
    input               arvalid_i,
    input               rready_i,
    output logic        arready_o,
    input               wvalid_i,
    input        [ 3:0] be_i,
    input        [31:0] araddr_i,
    input        [31:0] awaddr_i,
    input        [31:0] wdata_i,
    output logic        rvalid_o,
    output logic [31:0] rdata_o,
    output logic        wready_o,
    output logic        awready_o,
    output logic        bvalid_o,
    input               bready_i,
    output        [1:0] bresp_o,
    output        [1:0] rresp_o,
    input         [2:0] arprot_i,
    input         [2:0] awprot_i
);


  // Interface signals
  logic  [CCSW-1:0] key;
  logic             key_valid;
  logic             key_ready;
  logic  [ CCW-1:0] bdi;
  logic             bdi_valid;
  logic             bdi_ready;
  logic  [     3:0] bdi_type;
  logic             bdi_eot;
  logic             bdi_eoi;
  logic             decrypt;
  logic             hash;
  logic  [ CCW-1:0] bdo;
  logic             bdo_valid;
  logic             bdo_ready;
  logic  [     3:0] bdo_type;
  logic             bdo_eot;
  logic             auth;
  logic             auth_valid;
  logic             auth_ready;

  logic [1:0] write_count;
  logic awready;

  assign bresp_o = 2'b00;
  assign rresp_o = 2'b00;

  assign hash = 1'b0;
  assign decrypt = 1'b0;

  always_comb begin
    case (awaddr_i[3:0])
        4'b0000: bdi_type = D_NONCE;
        4'b0001: bdi_type = D_AD;
        4'b0010: bdi_type = D_PTCT;
        4'b0011: bdi_type = D_TAG;
        4'b0100: bdi_type = D_HASH;
        default: bdi_type = D_NULL;
    endcase
  end

  always @(posedge clk_i, posedge rst_i) begin
    if (rst_i) begin
      write_count <= 2'b00;
    end else begin
      if(awready && awvalid_i && bdi_type != D_NULL) begin
        write_count <= write_count + 1;
      end
    end
  end

  assign bdi_eot = write_count == 2'b11 && awvalid_i == 1'b1;
  assign bdi_eoi = bdi_eot && bdi_type == D_PTCT;

  assign bdi_valid = awvalid_i && bdi_type != D_NULL;
  assign rdata_o = auth_valid && araddr_i[3:0] == 4'b0110 ? auth : bdo;
  assign rvalid_o = bdo_valid || auth_valid;
  assign bdo_ready = rready_i; 
  assign auth_ready = rready_i;

  assign awready = bdi_type != D_NULL ? bdi_ready : key_ready;
  assign awready_o = awready;
  assign bdi = wdata_i;

  assign key = wdata_i;
  assign key_valid = wvalid_i && awvalid_i && awaddr_i[3:0] == 4'b0101;

  //assign bdi_eot = (wdata_i[1] && awaddr_i[3:0] == 4'b0110 && awvalid_i && wvalid_i) || bdi_eoi;

  //assign decrypt = wdata_i[2] && awaddr_i[3:0] == 4'b0110 && awvalid_i && wvalid_i;
  //assign hash = wdata_i[3] && awaddr_i[3:0] == 4'b0110 && awvalid_i && wvalid_i;


  // Instatiate Ascon core
  ascon_core ascon_core_i (
      .clk(clk_i),
      .rst(rst_i),
      .key(key),
      .key_valid(key_valid),
      .key_ready(key_ready),
      .bdi(bdi),
      .bdi_valid(bdi_valid),
      .bdi_ready(bdi_ready),
      .bdi_type(bdi_type),
      .bdi_eot(bdi_eot),
      .bdi_eoi(bdi_eoi),
      .decrypt(decrypt),
      .hash(hash),
      .bdo(bdo),
      .bdo_valid(bdo_valid),
      .bdo_ready(bdo_ready),
      .bdo_type(bdo_type),
      .bdo_eot(bdo_eot),
      .auth(auth),
      .auth_valid(auth_valid),
      .auth_ready(auth_ready)
  );



endmodule