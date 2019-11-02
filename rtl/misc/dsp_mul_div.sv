module dsp_mul_add(
    input [15:0] a,
    input [15:0] b,
    input [31:0] c,
    output [31:0] out 
);

    assign out = a*b+c;

endmodule
