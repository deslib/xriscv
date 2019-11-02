module xrv_mult(
    input               clk,
    input               rstb,
    input        [31:0] a,
    input        [31:0] b,
    input        [2:0]  mult_type,
    input               valid,

    output logic [31:0] result,
    output logic        result_valid
);
    logic [15:0] al;
    logic [15:0] ah;
    logic [15:0] bl;
    logic [15:0] bh;

    logic [15:0] dsp_a;
    logic [15:0] dsp_b;
    logic [31:0] dsp_out;
    logic [63:0] result64;

    logic [2:0]  mult_type_reg;

    logic [5:0] valid_dly;
    logic a_sign;
    logic b_sign;
    logic no_calc;

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            valid_dly <= 0;
        end else begin
            valid_dly <= {valid_dly[4:0],valid};
        end
    end 

    always @(posedge clk) begin
        if(valid) begin
            al <= a[15:0];
            ah <= mult_type == 3'h3 ? a[31:16] : {1'b0,a[30:16]};
            bl <= b[15:0];
            bh <= mult_type[1] ? b[31:16] : {1'b0,a[30:16]};
            a_sign <= a[31];
            b_sign <= b[31];
            mult_type_reg <= mult_type;
            no_calc <= (al == a[15:0]) & (bl == b[15:0]) & ({a_sign,ah[14:0]} == a[31:16]) & ({b_sign,bh[14:0]} == b[31:16]);
        end
    end

    always @(posedge clk) begin
        if(valid_dly[0]) begin
            dsp_a <= al;
            dsp_b <= bl;
        end else if(valid_dly[1]) begin
            dsp_a <= al;
            dsp_b <= bh;
        end else if(valid_dly[2]) begin
            dsp_a <= ah;
            dsp_b <= bl;
        end else if(valid_dly[3]) begin
            dsp_a <= ah;
            dsp_b <= bh;
        end
    end

    always @(posedge clk) begin
        if(valid_dly[1]) begin
            result64 <= {32'h0,dsp_out};
        end else if(valid_dly[2]|valid_dly[3]) begin
            result64 <= result64 + {16'h0,dsp_out,16'h0};
        end else if(valid_dly[4]) begin
            result64 <= result64 + {dsp_out,32'h0};
        end
    end

    assign result = mult_type == 3'b000 ? result64[31:0] :
                    mult_type == 3'b001 ? {sign,result64[62:32]} :
                    mult_type == 3'b010 ? {a[31],result64[62:32]} :
                                          result63[63:32];

    logic calc_result_valid;
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            calc_result_valid <= 0;
        end else begin
            if(~no_calc) begin
                if(mult_type == 3'h0) begin
                    calc_result_valid <= valid_dly[3];
                end else begin
                    calc_result_valid <= valid_dly[4];
                end
            end
        end
    end 
    assign result_valid = calc_result_valid | (valid_dly[0] & no_calc);

endmodule

