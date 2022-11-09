package sha2_pkg;

  `define ROTR(x, n) ($bits(x)'(x >> n) | $bits(x)'(x << 32 - n))

  typedef enum {SIG0, SIG1, SUM0, SUM1, SIG0H, SIG0L, SIG1H, SIG1L, SUM0R, SUM1R} sha2_op_t;

endpackage
