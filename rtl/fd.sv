/*******************************************************************************
* fetcher & decoder
********************************************************************************/ 
`include "glb.svh"
module fd(
    input                       clk,
    input                       rstb,

    input        [31:0]         i_data,
    output logic [31:0]         i_addr,

    input                       ls_done,       //load/store done
    input                       jmp,
    input        [31:0]         jmp_addr,

    output logic                ex_valid,
    output logic [31:0]         ex_pc,

    output logic                op_lui,              
    output logic                op_auipc,
    output logic                op_jal,  
    output logic                op_jalr,
    output logic                op_branch,
    output logic                op_load,
    output logic                op_store,
    output logic                op_imm,
    output logic                op_reg,

    output logic signed [31:0]  imm_signed,
    output logic [31:0]         imm_unsigned,
    output logic [4:0]          src1,
    output logic [4:0]          src2,
    output logic [4:0]          dest,
    output logic [2:0]          funct3,
    output logic [6:0]          funct7
);

    localparam logic [31:0] ALL0 = 32'h0;
    localparam logic [31:0] ALL1 = 32'hFFFF_FFFF; 

    logic [31:0]         next_pc;
    wire [4:0] opcode = i_data[6:2];
    wire is_op_lui    = (opcode == `OP_LUI)   ;
    wire is_op_auipc  = (opcode == `OP_AUIPC) ;
    wire is_op_jal    = (opcode == `OP_JAL)   ;
    wire is_op_jalr   = (opcode == `OP_JALR)  ;
    wire is_op_branch = (opcode == `OP_BRANCH);
    wire is_op_load   = (opcode == `OP_LOAD)  ;
    wire is_op_store  = (opcode == `OP_STORE) ;
    wire is_op_imm    = (opcode == `OP_IMM)   ;
    wire is_op_reg    = (opcode == `OP_REG)   ;

    logic during_ls_pending;
    wire  is_ls = is_op_load | is_op_store;
    logic jmp_dly; //need to delay one more cycle for jmp i_data in
    wire  decode_en = ls_done|(~during_ls_pending&~jmp&~jmp_dly);

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
            if(decode_en) begin
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
    end

    /****************************************************************************
    *       operand decoder
    ****************************************************************************/
    always @(posedge clk) begin
        if(decode_en) begin 
            imm_signed   <= (is_op_lui | is_op_auipc)         ? {i_data[31:12],ALL0[11:0]} :
                            (is_op_jal)                       ? {i_data[31] ? ALL1[31:21]:ALL0[31:21], i_data[31],i_data[19:12],i_data[20],i_data[30:21],1'b0} :
                            (is_op_jalr|is_op_imm|is_op_load) ? {i_data[31] ? ALL1[31:12]:ALL0[31:12], i_data[31:20]} :
                            (is_op_branch)                    ? {i_data[31] ? ALL1[31:13]:ALL0[31:13], i_data[31],i_data[7],i_data[30:25],i_data[11:8],1'b0} :
                            (is_op_store)                     ? {i_data[31] ? ALL1[31:12]:ALL0[31:12], i_data[31:25], i_data[11:7]} : 32'h0;

            imm_unsigned <= (is_op_lui | is_op_auipc)         ? {i_data[31:12],ALL0[11:0]} :
                            (is_op_jal)                       ? {ALL0[31:21], i_data[31],i_data[19:12],i_data[20],i_data[30:21],1'b0} :
                            (is_op_jalr|is_op_imm|is_op_load) ? {ALL0[31:12], i_data[31:20]} :
                            (is_op_branch)                    ? {ALL0[31:13], i_data[31],i_data[7],i_data[30:25],i_data[11:8],1'b0} :
                            (is_op_store)                     ? {ALL0[31:12], i_data[31:25], i_data[11:7]} : 32'h0;

            src1 <= i_data[19:15];
            src2 <= i_data[24:20];
            dest <= i_data[11:7];
            funct3 <= i_data[14:12];
            funct7 <= i_data[31:25];
        end
    end

/*******************************************************************************
*   pending control
********************************************************************************/ 
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            jmp_dly <= 0;
        end else begin
            jmp_dly <= jmp;
        end
    end
     
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            during_ls_pending <= 0;
        end else begin
            if(ls_done) begin
                during_ls_pending <= 0;
            end else if(jmp|jmp_dly) begin
                during_ls_pending <= 0;
            end else if(is_ls) begin
                during_ls_pending <= 1;
            end
        end
    end

/*******************************************************************************
*   PC control
********************************************************************************/ 
    logic [31:0] pc;
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            next_pc <= 0;
        end else begin
            if(ls_done|jmp_dly) begin
                next_pc <= next_pc + 4;
            end else if(jmp) begin
                next_pc <= jmp_addr;
            end else if(~during_ls_pending&~is_ls) begin
                next_pc <= next_pc + 4;
            end
        end
    end
    assign i_addr = next_pc;

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            pc <= 0;
            ex_pc <= 0;
        end else begin
            pc <= next_pc;
            ex_pc <= pc;
        end
    end
     

/*******************************************************************************
*   execuate valid control
********************************************************************************/ 
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            ex_valid <= 0;
        end else begin
            ex_valid <= decode_en;
        end
    end
     
     
endmodule
