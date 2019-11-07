`include "glb.svh"
module xrv_core(
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
    logic [31:0] ex_pc_auipc;
    logic [31:0] ex_pc_jmp;
    logic [31:0] ex_pc_branch;
    logic [31:0] inst;
    logic [31:0] inst_pc;
    logic ls_done;
    logic id_jmp;
    logic [31:0] id_jmp_addr;
    logic ex_jmp;
    logic [31:0] ex_jmp_addr;
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

    xrv_ctrl U_XRV_CTRL(
        .clk(clk),
        .rstb(rstb),
        .is_ls(is_ls),
        .ls_done(ls_done),
        .id_jmp(id_jmp),
        .id_jmp_addr(id_jmp_addr),
        .ex_jmp(ex_jmp),
        .ex_jmp_addr(ex_jmp_addr),
        .stalling(stalling),
        .flush(flush),
        .jmp(jmp),
        .jmp_addr(jmp_addr)
    );

    xrv_if U_XRV_IF(
        .clk(clk),
        .rstb(rstb),
        .stalling(stalling),
        .jmp(jmp),
        .jmp_addr(jmp_addr),
        .i_addr(i_addr),
        .i_data(i_data),
        .is_ls(is_ls),
        .inst(inst),
        .inst_valid(inst_valid),
        .inst_is_compressed(inst_is_compressed),
        .inst_pc(inst_pc)
    );

    xrv_id U_XRV_ID(
        .clk(clk),
        .rstb(rstb),
        .flush(flush),
    
        .inst(inst),
        .inst_pc(inst_pc),
        .inst_is_compressed(inst_is_compressed),
        .inst_valid(inst_valid),
        .id_jmp(id_jmp),
        .id_jmp_addr(id_jmp_addr),

        .op_lui(op_lui),              
        .op_auipc(op_auipc),
        .op_jal(op_jal),  
        .op_jalr(op_jalr),
        .op_branch(op_branch),
        .op_load(op_load),
        .op_store(op_store),
        .op_imm(op_imm),
        .op_reg(op_reg),
        .op_is_compressed(op_is_compressed),
    
        .imm_signed(imm_signed),
        .imm_unsigned(imm_unsigned),
        .src1(src1),
        .src2(src2),
        .dest(dest),
        .funct3_is_0(funct3_is_0),
        .funct3_is_1(funct3_is_1),
        .funct3_is_2(funct3_is_2),
        .funct3_is_3(funct3_is_3),
        .funct3_is_4(funct3_is_4),
        .funct3_is_5(funct3_is_5),
        .funct3_is_6(funct3_is_6),
        .funct3_is_7(funct3_is_7),
        .funct7_bit5(funct7_bit5),

        .ex_pc(ex_pc),
        .ex_pc_auipc(ex_pc_auipc),
        .ex_pc_jmp(ex_pc_jmp),
        .ex_pc_branch(ex_pc_branch),
        .ex_valid(ex_valid)

    );

    xrv_ex U_XRV_EX(
        .clk(clk),
        .rstb(rstb),
        //.flush(flush),

        .ex_jmp(ex_jmp),
        .ex_jmp_addr(ex_jmp_addr),
        .ls_done(ls_done),
    
        .ex_valid(ex_valid),
        .ex_pc(ex_pc),
        .ex_pc_auipc(ex_pc_auipc),
        .ex_pc_jmp(ex_pc_jmp),
        .ex_pc_branch(ex_pc_branch),
        .op_lui(op_lui),              
        .op_auipc(op_auipc),
        .op_jal(op_jal),  
        .op_jalr(op_jalr),
        .op_branch(op_branch),
        .op_load(op_load),
        .op_store(op_store),
        .op_imm(op_imm),
        .op_reg(op_reg),
        .op_is_compressed(op_is_compressed),
    
        .imm_signed(imm_signed),
        .imm_unsigned(imm_unsigned),
        .src1(src1),
        .src2(src2),
        .dest(dest),
        .funct3_is_0(funct3_is_0),
        .funct3_is_1(funct3_is_1),
        .funct3_is_2(funct3_is_2),
        .funct3_is_3(funct3_is_3),
        .funct3_is_4(funct3_is_4),
        .funct3_is_5(funct3_is_5),
        .funct3_is_6(funct3_is_6),
        .funct3_is_7(funct3_is_7),
        .funct7_bit5(funct7_bit5),
    
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
