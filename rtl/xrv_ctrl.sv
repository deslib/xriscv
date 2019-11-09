`include "glb.svh"

module xrv_ctrl(
    input               clk,
    input               rstb,

    input               is_ls,
    input               ls_done,       //load/store done
    `ifdef EN_MULT_DIV
    input               is_mult_div,
    input               mult_div_done,
    `endif
    input               id_jmp,
    input        [31:0] id_jmp_addr,
    input               ex_jmp,
    input        [31:0] ex_jmp_addr,

    output logic        stalling,
    output logic        flush,
    output logic        jmp,
    output logic [31:0] jmp_addr
);

    logic during_stalling;
    logic jmp_dly;
    `ifdef EN_MULT_DIV
    wire stall_start = is_ls | is_mult_div;
    wire stall_end = ls_done | mult_div_done;
    `else
    wire stall_start = is_ls;
    wire stall_end = ls_done;
    `endif
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            during_stalling <= 0;
        end else begin
            if((stall_start)&~jmp&~jmp_dly) begin
                during_stalling <= 1;
            end else if(stall_end|jmp|jmp_dly) begin  //ls follow an jalr. jmp is later than is_ls
                during_stalling <= 0;
            end
        end
    end

    assign stalling = (stall_start| during_stalling) & ~stall_end & ~jmp_dly;

    //always @(posedge clk or negedge rstb) begin
    //    if(~rstb) begin
    //        stalling_id <= 0;
    //        stalling_ex <= 0;
    //    end else begin
    //        stalling_id <= stalling_if;
    //        stalling_ex <= stalling_id;
    //    end
    //end

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            jmp_dly <= 0;
        end else begin
            jmp_dly <= jmp;
        end
    end 

    assign jmp = id_jmp | ex_jmp;
    assign jmp_addr = ex_jmp ? ex_jmp_addr : id_jmp_addr;

    assign flush = ex_jmp;

endmodule
