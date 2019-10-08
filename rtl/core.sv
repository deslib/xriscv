`include "glb.svh"
module core(
    input                 clk,
    input                 rstb,

    input        [31:0]   i_data,
    output logic [31:0]   i_addr,

    output logic [31:0]   d_addr,
    output logic          d_wr_req,
    input                 d_wr_ready,
    output logic          d_rd_req,
    input                 d_rd_ready,
    output logic [3 :0]   d_be,
    input        [31:0]   d_rd_data,
    output logic [31:0]   d_wr_data
);

    logic [31:0] ex_pc;
    logic ls_done;
    logic jmp;
    logic [31:0] jmp_addr;

    logic ex_valid;

    logic op_lui  ;
    logic op_auipc;
    logic op_jal  ;
    logic op_jalr ;
    logic op_branch;
    logic op_load  ;
    logic op_store ;
    logic op_imm   ;
    logic op_reg   ;

    logic signed [31:0] imm_signed;
    logic [31:0] imm_unsigned;
    logic [4:0] src1;
    logic [4:0] src2;
    logic [4:0] dest;
    logic [2:0] funct3;
    logic [6:0] funct7;


    fd U_FD(
        .clk(clk),
        .rstb(rstb),
    
        .i_data(i_data),
        .i_addr(i_addr),
    
        .ls_done(ls_done),       
        .jmp(jmp),
        .jmp_addr(jmp_addr),
    
        .ex_valid(ex_valid),
    
        .ex_pc(ex_pc),
        .op_lui(op_lui),              
        .op_auipc(op_auipc),
        .op_jal(op_jal),  
        .op_jalr(op_jalr),
        .op_branch(op_branch),
        .op_load(op_load),
        .op_store(op_store),
        .op_imm(op_imm),
        .op_reg(op_reg),
    
        .imm_signed(imm_signed),
        .imm_unsigned(imm_unsigned),
        .src1(src1),
        .src2(src2),
        .dest(dest),
        .funct3(funct3),
        .funct7(funct7)
    );

    ex U_EX(
        .clk(clk),
        .rstb(rstb),

        .jmp(jmp),
        .jmp_addr(jmp_addr),
        .ls_done(ls_done),
    
        .ex_valid(ex_valid),
        .ex_pc(ex_pc),
        .op_lui(op_lui),              
        .op_auipc(op_auipc),
        .op_jal(op_jal),  
        .op_jalr(op_jalr),
        .op_branch(op_branch),
        .op_load(op_load),
        .op_store(op_store),
        .op_imm(op_imm),
        .op_reg(op_reg),
    
        .imm_signed(imm_signed),
        .imm_unsigned(imm_unsigned),
        .src1(src1),
        .src2(src2),
        .dest(dest),
        .funct3(funct3),
        .funct7(funct7),
    
        .d_addr(d_addr),
        .d_wr_req(d_wr_req),
        .d_be(d_be),
        .d_wr_data(d_wr_data),
        .d_wr_ready(d_wr_ready),
        .d_rd_req(d_rd_req),
        .d_rd_ready(d_rd_ready),
        .d_rd_data(d_rd_data)
    );

endmodule    
