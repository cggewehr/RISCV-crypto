// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0
//
// Synchronous dual-port SRAM register model
//   This module is for simulation and small size SRAM.
//   Implementing ECC should be done inside wrapper not this model.
`include "prim_assert.sv"
// module prim_generic_ram_2p import prim_ram_2p_pkg::*; #(
module prim_ram_2p import prim_ram_2p_pkg::*; #(
  parameter  int Width           = 32, // bit
  parameter  int Depth           = 128,
  parameter  int DataBitsPerMask = 1, // Number of data bits per bit of write mask
  parameter      MemInitFile     = "", // VMEM file to initialize the memory with

  localparam int Aw              = $clog2(Depth)  // derived parameter
) (
  input clk_a_i,
  input clk_b_i,
  input rst_ni,

  input                    a_awvalid_i,
  input                    a_arvalid_i,
  input                    a_rready_i,
  input                    a_wvalid_i,

  input                    a_write_i,
  input        [Aw-1:0]    a_araddr_i,
  input        [Aw-1:0]    a_awaddr_i,
  input        [Width-1:0] a_wdata_i,
  input        [Width-1:0] a_wmask_i,
  output logic [Width-1:0] a_rdata_o,
  output logic             a_rvalid_o,
  output logic             a_arready_o,

  output logic             a_wready_o,
  output logic             a_awready_o,
  output logic             a_bvalid_o,
  input                    a_bready_i,


  input                    b_req_i,
  input                    b_write_i,
  input        [Aw-1:0]    b_addr_i,
  input        [Width-1:0] b_wdata_i,
  input  logic [Width-1:0] b_wmask_i,
  output logic [Width-1:0] b_rdata_o,


  input ram_2p_cfg_t       cfg_i
);

// For certain synthesis experiments we compile the design with generic models to get an unmapped
// netlist (GTECH). In these synthesis experiments, we typically black-box the memory models since
// these are going to be simulated using plain RTL models in netlist simulations. This can be done
// by analyzing and elaborating the design, and then removing the memory submodules before writing
// out the verilog netlist. However, memory arrays can take a long time to elaborate, and in case
// of dual port rams they can even trigger elab errors due to multiple processes writing to the
// same memory variable concurrently. To this end, we exclude the entire logic in this module in
// these runs with the following macro.
`ifndef SYNTHESIS_MEMORY_BLACK_BOXING

  logic unused_cfg;
  assign unused_cfg = ^cfg_i;

  // Width of internal write mask. Note *_wmask_i input into the module is always assumed
  // to be the full bit mask.
  localparam int MaskWidth = Width / DataBitsPerMask;

  logic [Width-1:0]     mem [Depth];
  logic [MaskWidth-1:0] a_wmask;
  logic [MaskWidth-1:0] b_wmask;

  for (genvar k = 0; k < MaskWidth; k++) begin : gen_wmask
    assign a_wmask[k] = &a_wmask_i[k*DataBitsPerMask +: DataBitsPerMask];
    assign b_wmask[k] = &b_wmask_i[k*DataBitsPerMask +: DataBitsPerMask];

    // Ensure that all mask bits within a group have the same value for a write
    `ASSERT(MaskCheckPortA_A, a_awvalid_i && a_write_i |-> 
        a_wmask_i[k*DataBitsPerMask +: DataBitsPerMask] inside {{DataBitsPerMask{1'b1}}, '0},
        clk_a_i, '0)
    `ASSERT(MaskCheckPortB_A, b_req_i && b_write_i |->
        b_wmask_i[k*DataBitsPerMask +: DataBitsPerMask] inside {{DataBitsPerMask{1'b1}}, '0},
        clk_b_i, '0)
  end

  enum logic[1:0] {IDLE = 0, READ = 1, WRITE = 2} a_state, a_next_state;

  always_comb begin
    a_next_state = IDLE;
    case (a_state)
      IDLE: begin
        if(a_arvalid_i) a_next_state = READ;
        else if(a_wvalid_i) a_next_state = WRITE;
      end
      READ: begin
        if (a_rready_i) begin
          if(a_arvalid_i) a_next_state = READ;
          else if(a_wvalid_i) a_next_state = WRITE;
          else a_next_state = IDLE;
        end
      end
      WRITE: begin
        if (a_bready_i) begin
          if(a_arvalid_i) a_next_state = READ;
          else if(a_wvalid_i) a_next_state = WRITE;
          else a_next_state = IDLE;
        end
      end
      default: a_next_state = IDLE;
    endcase
  end

  always_ff@(posedge clk_a_i, negedge rst_ni) begin
    if (~rst_ni) begin
      a_state <= IDLE;
    end
    else begin
      a_state <= a_next_state;
      case (a_state)

        WRITE: begin
          for (int i=0; i < MaskWidth; i = i + 1) begin
            if (a_wmask[i]) begin
              mem[a_awaddr_i][i*DataBitsPerMask +: DataBitsPerMask] <=
                a_wdata_i[i*DataBitsPerMask +: DataBitsPerMask];
            end
          end
        end
        READ: a_rdata_o <= mem[a_araddr_i];
        IDLE: begin
          if (a_arvalid_i) a_rdata_o <= mem[a_araddr_i];
        end
      endcase
    end
  end
  

  always_comb begin
    a_wready_o = 1'b1;
    a_awready_o = 1'b1;
    a_arready_o = 1'b1;
    
    case (a_state)
      IDLE: begin
        // read
        a_rvalid_o = 1'b0;
        // write
        a_bvalid_o = 1'b0;
      end
      READ: begin
        a_rvalid_o = 1'b1;
      end
      WRITE: begin
        a_bvalid_o = 1'b1;
      end
    endcase
  end

  always @(posedge clk_b_i) begin
    if (b_req_i) begin
      if (b_write_i) begin
        for (int i=0; i < MaskWidth; i = i + 1) begin
          if (b_wmask[i]) begin
            mem[b_addr_i][i*DataBitsPerMask +: DataBitsPerMask] <=
              b_wdata_i[i*DataBitsPerMask +: DataBitsPerMask];
          end
        end
      end else begin
        b_rdata_o <= mem[b_addr_i];
      end
    end
  end

  `include "prim_util_memload.svh"
`endif
endmodule
