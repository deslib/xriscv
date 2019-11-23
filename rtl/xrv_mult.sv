
module xrv_mult(
    input               clk,
    input               rstb,
    input        [31:0] a,
    input        [31:0] b,
    input        [1:0]  optype,
    input               valid,

    output logic [31:0] result,
    output logic        result_valid
);

    logic signed [32:0] mult_a;
    logic signed [32:0] mult_b;
    logic signed [63:0] mult_result;
    wire mult_high = optype[0]|optype[1];
    wire mult_sign_a = ~(optype[1]&optype[0]);
    wire mult_sign_b = ~optype[1];
    logic valid_dly;
    always @(posedge clk) begin
        if(valid) begin
            mult_a <= mult_sign_a ? {a[31],a} : {1'b0,a};
            mult_b <= mult_sign_b ? {b[31],b} : {1'b0,b};
        end
    end
    assign mult_result = mult_a * mult_b;
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            valid_dly <= 0;
            result_valid <= 0;
            result <= 0;
        end else begin
            valid_dly <= valid;
            result_valid <= valid_dly;
            result <= mult_high ? mult_result[63:32] : mult_result[31:0];
        end
    end

endmodule

