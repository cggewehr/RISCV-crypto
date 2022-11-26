module sha2_unit import ibex_pkg::*; (

  input logic sha2_en_i,
  input ibex_pkg::sha2_op_t sha2_op_i,

  input logic[31:0] op_a_i,
  input logic[31:0] op_b_i,
  output logic[31:0] sha2_result_o

);

  //`define ROTR(x, n) ($bits(x)'(x >> n) | $bits(x)'(x << 32 - n))
  `define ROTR(x, n) ($bits(x)'(x >> n) | $bits(x)'(x << ($bits(x) - n)))

  always_comb begin

    if (sha2_en_i) begin

      unique case (sha2_op_i)

        // SHA256 instructions
        SIG0: sha2_result_o = `ROTR(op_a_i, 7) ^ `ROTR(op_a_i, 18) ^ (op_a_i >> 3);
        SIG1: sha2_result_o = `ROTR(op_a_i, 17) ^ `ROTR(op_a_i, 19) ^ (op_a_i >> 10);
        SUM0: sha2_result_o = `ROTR(op_a_i, 2) ^ `ROTR(op_a_i, 13) ^ `ROTR(op_a_i, 22);
        //SUM1: sha2_result_o = `ROTR(op_a_i, 6) ^ `ROTR(op_a_i, 11) ^ `ROTR(op_a_i, 25);
        SUM1: begin
          sha2_result_o = `ROTR(op_a_i, 6) ^ `ROTR(op_a_i, 11) ^ `ROTR(op_a_i, 25);
          $display("%032b", op_a_i);
          $display("%032b", `ROTR(op_a_i, 6));
          $display("%032b", `ROTR(op_a_i, 11));
          $display("%032b", `ROTR(op_a_i, 25));
          //$display("%032b", 32'ha54ff53a);
          //$display("%032b", `ROTR(32'ha54ff53a, 6));
          //$display("%032b", `ROTR(32'ha54ff53a, 11));
          //$display("%032b", `ROTR(32'ha54ff53a, 25));
        end

        // SHA512 instructions
        SIG0H: sha2_result_o = (op_a_i >> 1) ^ (op_a_i >> 7) ^ (op_a_i >> 8) ^ (op_b_i << 31) ^ (op_b_i << 24);
        SIG0L: sha2_result_o = (op_a_i >> 1) ^ (op_a_i >> 7) ^ (op_a_i >> 8) ^ (op_b_i << 31) ^ (op_b_i << 25) ^ (op_b_i << 24);
        SIG1H: sha2_result_o = (op_a_i << 3) ^ (op_a_i >> 6) ^ (op_a_i >> 19) ^ (op_b_i >> 29) ^ (op_b_i << 13);
        SIG1L: sha2_result_o = (op_a_i << 3) ^ (op_a_i >> 6) ^ (op_a_i >> 19) ^ (op_b_i >> 29) ^ (op_b_i << 26) ^ (op_b_i << 13);
        SUM0R: sha2_result_o = (op_a_i << 25) ^ (op_a_i << 30) ^ (op_a_i >> 28) ^ (op_b_i >> 7) ^ (op_b_i >> 2) ^ (op_b_i << 4);
        SUM1R: sha2_result_o = (op_a_i << 23) ^ (op_a_i >> 14) ^ (op_a_i >> 18) ^ (op_b_i >> 9) ^ (op_b_i << 18) ^ (op_b_i << 14);

        default: sha2_result_o = sha2_result_o;

      endcase

    end

  end

  always @ (sha2_result_o)
      $display("SHA256: OP: <%s> A: <%08h> B: <%08h> RESULT: <%08h>", sha2_op_i.name(), op_a_i, op_b_i, sha2_result_o);

endmodule
