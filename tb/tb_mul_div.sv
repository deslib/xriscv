`timescale 1ns/10ps

module tb_mul_div;
    localparam real PERIOD = 1.0;
    logic               clk;
    logic               rstb;
    logic signed [31:0] a;
    logic signed [31:0] b;
    logic               valid;
    logic        [5:0]  cnt;
    logic        [2:0]  optype;
    logic        [31:0] result_mult;
    logic        [31:0] result_div;
    wire                result_valid;

    always #(PERIOD/2) clk = ~clk;

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            cnt <= 0;
        end else begin
            cnt <= cnt + 1;
        end
    end 

    initial begin
        clk = 0;
        rstb = 0;
        #(10); rstb = 1;
    end

    always @(posedge clk) begin
        if(cnt == 'h3F) begin
            valid <= 1;
        end else begin
            valid <= 0;
        end
    end 

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            optype <= 0;
        end else begin
            if(result_valid) begin
                optype <= optype + 1;
            end
        end
    end 

    always @(*) begin
        case(optype)
            0: begin
                a = 32'h25;
                b = 32'h34;
            end
            1,2,3: begin
                a = 32'hf5008456;
                b = 32'hf7239485;
            end
            4,5,6,7: begin
                a = 32'hf0e94047;
                b = 32'h230;
            end
        endcase
    end


xrv_mult U_XRV_MULT(
    .clk(clk),
    .rstb(rstb),
    .a(a),
    .b(b),
    .optype(optype),
    .valid(~optype[2]&valid),

    .result(result_mult),
    .result_valid(result_mult_valid)
);

xrv_div U_XRV_DIV(
    .clk(clk),
    .rstb(rstb),
    .dividend(a),
    .divisor(b),
    .optype(optype),
    .valid(optype[2]&valid),
    .result(result_div),
    .result_valid(result_div_valid)
);

assign result_valid = result_mult_valid | result_div_valid;


endmodule
