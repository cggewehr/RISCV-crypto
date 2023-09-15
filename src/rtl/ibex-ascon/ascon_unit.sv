module ascon_unit import ibex_pkg::*;  #(
    parameter LOGIC_GATING = 1  // Gate sub-module inputs to save toggling
)(

  input logic ascon_en_i,
  input ibex_pkg::ascon_op_t ascon_op_i,

  input logic[31:0] op_a_i,
  input logic[31:0] op_b_i,
  output logic[31:0] ascon_result_o

);

  `define GATE_INPUTS(LEN,SEL,SIG) (LOGIC_GATING ? ({LEN{SEL}} & SIG[LEN-1:0]) : SIG[LEN-1:0])
  `define ROTR(x, n) ($bits(x)'(x >> n) | $bits(x)'(x << ($bits(x) - n)))
  
  wire[31:0] op_a_gated = `GATE_INPUTS(32, ascon_en_i, op_a_i);
  wire[31:0] op_b_gated = `GATE_INPUTS(32, ascon_en_i , op_b_i);
  logic[63:0] result_full;
  wire[63:0] op_full = {op_b_gated, op_a_gated};

  always_comb begin

    unique case (ascon_op_i)

      ASCSIG0H, ASCSIG0L: result_full = (op_full ^ `ROTR(op_full, 19) ^ `ROTR(op_full, 28));
      ASCSIG1H, ASCSIG1L: result_full = (op_full ^ `ROTR(op_full, 61) ^ `ROTR(op_full, 39));
      ASCSIG2H, ASCSIG2L: result_full = (op_full ^ `ROTR(op_full, 1) ^ `ROTR(op_full, 6));
      ASCSIG3H, ASCSIG3L: result_full = (op_full ^ `ROTR(op_full, 10) ^ `ROTR(op_full, 17));
      ASCSIG4H, ASCSIG4L: result_full = (op_full ^ `ROTR(op_full, 7) ^ `ROTR(op_full, 41));

    endcase

    unique case (ascon_op_i)

      ASCSIG0H, ASCSIG1H, ASCSIG2H, ASCSIG3H, ASCSIG4H: ascon_result_o = result_full[63:32];
      ASCSIG0L, ASCSIG1L, ASCSIG2L, ASCSIG3L, ASCSIG4L: ascon_result_o = result_full[31:0];
      default: ascon_result_o = ascon_result_o;

    endcase

  end

  `undef GATE_INPUTS
  `undef ROTR
endmodule
