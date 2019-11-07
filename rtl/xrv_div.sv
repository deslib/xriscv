module xrv_div(
    input               clk,
    input               rstb,
    input        [31:0] dividend,
    input        [31:0] divisor,
    input               is_div_sign,
    input               optype, //0: div; 1: rem
    input               valid,
    output logic [31:0] result,
    output logic        result_valid
);

    logic [31:0] quotient;
    logic [63:0] remainder;
    logic [31:0] divisor_reg;
    logic        sign_reg;
    logic        calc_en;
    logic        optype_reg;
    logic [4:0]  calc_cnt;

    wire [31:0] dividend_abs = is_div_sign ? 
                                (dividend[31] ? {1'b0,(~dividend[30:0] + 31'h1)} : 
                                               {1'b0,dividend[30:0]}) :
                                dividend[31:0];
    wire [31:0] divisor_abs  = is_div_sign ? 
                                (divisor[31] ? {1'b0,(~divisor[30:0] + 31'h1)} : 
                                               {1'b0,divisor[30:0]}) :
                                divisor[31:0];
    wire        sign         = is_div_sign ? dividend[31] ^ divisor[31] : 1'b0;
    wire [31:0] remainder_h  = remainder[63:32] - divisor;
    always @(posedge clk) begin
        if(valid) begin
            remainder    <= dividend_abs << 1;
            divisor_reg  <= divisor_abs;
            sign_reg     <= sign;
            quotient     <= 0;
            remainder    <= dividend_abs << 1;
            optype_reg   <= 1;
        end else begin
            if(calc_en) begin
                if(remainder[63:32] >= divisor_reg) begin
                    quotient  <= {quotient[30:0],1'b1};
                    remainder <= {remainder_h[30:0],remainder[31:0],1'b0};
                end else begin
                    quotient  <= {quotient[30:0],1'b0};
                    remainder <= {remainder[62:0],1'b0};
                end
            end
        end
    end 

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            calc_en <= 0;
        end else begin
            if(calc_cnt == 'h1F) begin
                calc_en <= 0;
            end else if(valid) begin
                calc_en <= 1;
            end
        end
    end 

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            calc_cnt <= 0;
        end else begin
            if(valid) begin
                calc_cnt <= 0;
            end else if(calc_en)begin
                calc_cnt <= calc_cnt + 1;
            end
        end
    end 

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            result_valid <= 0;
            result <= 0;
        end else begin
            result_valid <= (calc_cnt == 5'h1f)&calc_en;
            result <= optype_reg ? remainder : quotient;
        end
    end 

endmodule
