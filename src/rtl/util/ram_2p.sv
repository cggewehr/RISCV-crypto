// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

/**
 * Dual-port RAM with 1 cycle read/write delay, 32 bit words.
 */

`include "prim_assert.sv"

module ram_2p #(
    parameter int Depth       = 128,
    parameter     MemInitFile = ""
) (
    input               clk_i,
    input               rst_ni,

    //input               a_req_i,
    input               a_awvalid_i,
    input               a_arvalid_i,
    input               a_rready_i,
    output logic        a_arready_o,

    input               a_wvalid_i,

    input        [ 3:0] a_be_i,
    input        [31:0] a_araddr_i,
    input        [31:0] a_awaddr_i,
    input        [31:0] a_wdata_i,
    output logic        a_rvalid_o,
    output logic [31:0] a_rdata_o,
    
    output logic        a_wready_o,
    output logic        a_awready_o,
    output logic        a_bvalid_o,
    input               a_bready_i,
    output        [1:0] a_bresp_o,
    output        [1:0] a_rresp_o,
    input         [2:0] a_arprot_i,
    input         [2:0] a_awprot_i,

    input               b_req_i,
    input               b_we_i,
    input        [ 3:0] b_be_i,
    input        [31:0] b_addr_i,
    input        [31:0] b_wdata_i,
    output logic        b_rvalid_o,
    output logic [31:0] b_rdata_o
);

  localparam int Aw = $clog2(Depth);

  assign a_rresp_o = 2'b00;
  assign a_bresp_o = 2'b00;


  logic [Aw-1:0] a_araddr_idx;
  assign a_araddr_idx = a_araddr_i[Aw-1+2:2];
  logic [31-Aw:0] unused_a_araddr_parts;
  assign unused_a_araddr_parts = {a_araddr_i[31:Aw+2], a_araddr_i[1:0]};

  logic [Aw-1:0] a_awaddr_idx;
  assign a_awaddr_idx = a_awaddr_i[Aw-1+2:2];
  logic [31-Aw:0] unused_a_awaddr_parts;
  assign unused_a_awaddr_parts = {a_awaddr_i[31:Aw+2], a_awaddr_i[1:0]};

  logic [Aw-1:0] b_addr_idx;
  assign b_addr_idx = b_addr_i[Aw-1+2:2];
  logic [31-Aw:0] unused_b_addr_parts;
  assign unused_b_addr_parts = {b_addr_i[31:Aw+2], b_addr_i[1:0]};

  // Convert byte mask to SRAM bit mask.
  logic [31:0] a_wmask;
  logic [31:0] b_wmask;
  always_comb begin
    for (int i = 0 ; i < 4 ; i++) begin
      // mask for read data
      a_wmask[8*i+:8] = {8{a_be_i[i]}};
      b_wmask[8*i+:8] = {8{b_be_i[i]}};
    end
  end


  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      b_rvalid_o <= '0;
    end else begin
      b_rvalid_o <= b_req_i;
    end
  end

  prim_ram_2p #(
    .Width(32),
    .Depth(Depth),
    .DataBitsPerMask(8),
    .MemInitFile(MemInitFile)
  ) u_ram (
    .clk_a_i     (clk_i),
    .clk_b_i     (clk_i),
    .cfg_i       (20'b0),
    .rst_ni      (rst_ni),
    //.a_req_i   (a_req_i),
    .a_awvalid_i (a_awvalid_i),
    .a_arvalid_i (a_arvalid_i),
    .a_rready_i  (a_rready_i),
    .a_arready_o  (a_arready_o),
    .a_rvalid_o  (a_rvalid_o),

    .a_write_i   (1'b0),
    .a_araddr_i    (a_araddr_idx),
    .a_awaddr_i    (a_awaddr_idx),
    .a_wdata_i   (a_wdata_i),
    .a_wmask_i   (a_wmask),
    .a_rdata_o   (a_rdata_o),

    .a_wvalid_i (a_wvalid_i),
    .a_wready_o (a_wready_o),
    .a_awready_o(a_awready_o),
    .a_bvalid_o(a_bvalid_o),
    .a_bready_i(a_bready_i),

    .b_req_i   (b_req_i),
    .b_write_i (b_we_i),
    .b_wmask_i (b_wmask),
    .b_addr_i  (b_addr_idx),
    .b_wdata_i (b_wdata_i),
    .b_rdata_o (b_rdata_o)
  );

endmodule
