`include "glb.svh"
module core(
    input clk,
    input rstb,

    input [31:0] i_data,
    output [31:0] i_addr,

    output [31:0] d_addr,
    output        d_wr_en,
    output [3:0]  d_be,
    input [31:0]  d_data
);

    localparam logic [31:0] ALL0 = 32'h0;
    localparam logic [31:0] ALL1 = 32'hFFFF_FFFF; 

    logic [31:0] pc;
    logic [31:0] next_pc;
    logic pipe_flush;

    wire [4:0] opcode = i_data[6:2];
    wire is_op_lui    = (opcode == `OP_LUI)     & ~pipe_flush;
    wire is_op_auipc  = (opcode == `OP_AUIPC)   & ~pipe_flush;
    wire is_op_jal    = (opcode == `OP_JAL)     & ~pipe_flush;
    wire is_op_jalr   = (opcode == `OP_JALR)    & ~pipe_flush;
    wire is_op_branch = (opcode == `OP_BRANCH)  & ~pipe_flush;
    wire is_op_load   = (opcode == `OP_LOAD)    & ~pipe_flush;
    wire is_op_store  = (opcode == `OP_STORE)   & ~pipe_flush;
    wire is_op_imm    = (opcode == `OP_IMM)     & ~pipe_flush;
    wire is_op_reg    = (opcode == `OP_REG)     & ~pipe_flush;

    logic op_lui  ;
    logic op_auipc;
    logic op_jal  ;
    logic op_jalr ;
    logic op_branch;
    logic op_load  ;
    logic op_store ;
    logic op_imm   ;
    logic op_reg   ;

    logic [31:0] operand_imm_signed;
    logic [31:0] operand_imm_unsigned;
    logic [4:0] reg_src_sel1;
    logic [4:0] reg_src_sel2;
    logic [4:0] reg_dest_sel;
    logic [2:0] alu_func;
    logic [6:0] alu_func_ext;

    logic [31:0][31:0] reg_table;
    assign reg_table[0] = 32'h0;

    assign d_addr = reg_table[reg_src_sel1] + operand_imm_signed;

    /****************************************************************************
    *       Opcode Decoder
    ****************************************************************************/
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            op_lui       <= 0;
            op_auipc     <= 0;
            op_jal       <= 0;
            op_jalr      <= 0;
            op_branch    <= 0;
            op_load      <= 0;
            op_store     <= 0;
            op_imm       <= 0;
            op_reg       <= 0;
        end else begin
            op_lui       <= is_op_lui;
            op_auipc     <= is_op_auipc;
            op_jal       <= is_op_jal;
            op_jalr      <= is_op_jalr;
            op_branch    <= is_op_branch;
            op_load      <= is_op_load;
            op_store     <= is_op_store;
            op_imm       <= is_op_imm;
            op_reg       <= is_op_reg;
        end
    end

    /****************************************************************************
    *       operand decoder
    ****************************************************************************/
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            operand_imm_signed <= 0;
            operand_imm_unsigned <= 0;
            reg_src_sel1 <= 0;
            reg_src_sel2 <= 0;
            reg_dest_sel <= 0;
            alu_func <= 0;
            alu_func_ext <= 0;
        end else begin
            operand_imm_signed   <= (is_op_lui | is_op_auipc)         ? {i_data[31:12],ALL0[11:0]} :
                                    (is_op_jal)                       ? {i_data[31] ? ALL1[31:21]:ALL0[31:21], i_data[31],i_data[19:12],i_data[20],i_data[30:21],1'b0} :
                                    (is_op_jalr|is_op_imm|is_op_load) ? {i_data[11] ? ALL1[31:12]:ALL0[31:12], i_data[31:20]} :
                                    (is_op_branch)                    ? {i_data[31],i_data[7],i_data[30:25],i_data[11:8],1'b0} :
                                    (is_op_store)                     ? {i_data[31] ? ALL1[31:12]:ALL0[31:12], i_data[31:25], i_data[11:7]} : 32'h0;
            operand_imm_unsigned <= (is_op_lui | is_op_auipc)         ? {i_data[31:12],ALL0[11:0]} :
                                    (is_op_jal)                       ? {{ALL0[31:21], i_data[31],i_data[19:12],i_data[20],i_data[30:21],1'b0} :
                                    (is_op_jalr|is_op_imm|is_op_load) ? {{ALL0[31:12], i_data[31:20]} :
                                    (is_op_branch)                    ? {i_data[31],i_data[7],i_data[30:25],i_data[11:8],1'b0} :
                                    (is_op_store)                     ? {ALL0[31:12], i_data[31:25], i_data[11:7]} : 32'h0;
            reg_src_sel1 <= i_data[19:15];
            reg_src_sel2 <= i_data[24:20];
            reg_dest_sel <= i_data[11:7];
            alu_func <= i_data[14:12];
            alu_func_ext <= i_data[31:25];
        end
    end

    wire signed [31:0] operand1 = reg_table[reg_src_sel1];
    wire signed [31:0] operand2 = op_reg ? reg_table[reg_src_sel2] : operand_imm_signed;

    /****************************************************************************
    *       alu
    ****************************************************************************/
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            reg_table[31:1] <= 0;
        end else begin
            if(op_lui) begin
                reg_table[reg_dest_sel] <= operand_imm_signed;
            end else if(op_auipc) begin
                reg_table[reg_dest_sel] <= pc + operand_imm_signed;
            end else if(op_jal|op_jalr) begin
                reg_table[reg_dest_sel] <= pc + 4;
            end else if(op_load) begin
                reg_table[reg_dest_sel] <= alu_func == 3'b000 ? {d_data[7] ? ALL1[31:7] : ALL0[31:7],d_data[6:0]} :
                                                       3'b001 ? {d_data[15] ? ALL1[31:15] : ALL0[31:15], d_data[14:0]} :
                                                       3'b010 ? d_data[31:0] :
                                                       3'b100 ? {ALL0[31:8],d_data[7:0]} :
                                                       3'b101 ? {ALL0[31:16],d_data[15:0]} : 32'h0;
            end else if(op_imm|op_reg) begin
                reg_table[reg_dest_sel] <= alu_func == 3'b000 ? (alu_func_ext[5] ? operand1 - operand2 : operand1 + operand2) :
                                           alu_func == 3'b001 ? operand1 << operand2[4:0] :
                                           alu_func == 3'b010 ? (operand1 < operand2 ? 32'h1 : 32'h0) :
                                           alu_func == 3'b011 ? ($unsigned(operand1) < $unsigned(operand2)) ? 32'h1 : 32'h0) :
                                           alu_func == 3'b100 ? operand1 ^ operand2 :
                                           alu_func == 3'b101 ? operand1 >> operand2[4:0] :
                                           alu_func == 3'b110 ? operand1 | operand2 :
                                                                operand1 & operand2;
            end
        end
    end

    /****************************************************************************
    *         PC control (Jump or Branch)
    ****************************************************************************/
    wire operand_lt = operand1 < operand2;
    wire operand_ltu = $unsigned(operand1) < $unsigned(operand2);
    wire branch = op_branch & ( ( alu_func == 3'b000 && operand1 == operand2) |
                                ( alu_func == 3'b001 && operand1 != operand2) |
                                ( alu_func == 3'b100 && operand_lt)  |
                                ( alu_func == 3'b101 && !operand_lt )  |
                                ( alu_func == 3'b110 && operand_ltu ) |
                                ( alu_func == 3'b111 && !operand_ltu ) );

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            pc <= PC_INIT;
        end else begin
            pc <= next_pc;
        end
    end

    always @(*) begin
        if(~rstb) begin
            next_pc = PC_INIT;
        end else begin
            if(op_jal|op_jalr|branch) begin
                next_pc = pc + operand_imm_signed;
            end else begin
                next_pc = pc + 4;
            end
        end
    end 

    assign i_addr = next_pc;

    /****************************************************************************
    *         Pipeline flush 
    ****************************************************************************/
    assign pipe_flush = op_jal | op_jalr | branch;

endmodule

