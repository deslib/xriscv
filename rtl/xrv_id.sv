/*******************************************************************************
* instruction decoder
********************************************************************************/ 
`include "glb.svh"
module xrv_id(
    input                       clk,
    input                       rstb,

    input                       flush,
    input        [31:0]         inst,
    input        [31:0]         inst_pc,
    input                       inst_is_compressed,
    input                       inst_valid,
    output logic                id_jmp,
    output logic [31:0]         id_jmp_addr,

    output logic                op_lui,              
    output logic                op_auipc,
    output logic                op_jal,  
    output logic                op_jalr,
    output logic                op_branch,
    output logic                op_load,
    output logic                op_store,
    output logic                op_imm,
    output logic                op_reg,
    output logic                op_is_compressed,

    output logic [31:0]         imm_signed,
    output logic [31:0]         imm_unsigned,
    output logic [4:0]          src1,
    output logic [4:0]          src2,
    output logic [4:0]          dest,
    output logic [2:0]          funct3,
    output logic                funct7_bit5,

    output logic [31:0]         ex_pc,
    output logic [31:0]         ex_pc_auipc,
    output logic [31:0]         ex_pc_jmp,
    output logic [31:0]         ex_pc_branch,
    output logic                ex_valid

);

    localparam logic [31:0] ALL0 = 32'h0;
    localparam logic [31:0] ALL1 = 32'hFFFF_FFFF; 

    wire [4:0] opcode = inst[6:2];
    wire is_op_lui    = (opcode == `OP_LUI)   ;
    wire is_op_auipc  = (opcode == `OP_AUIPC) ;
    wire is_op_jal    = (opcode == `OP_JAL)   ;
    wire is_op_jalr   = (opcode == `OP_JALR)  ;
    wire is_op_branch = (opcode == `OP_BRANCH);
    wire is_op_load   = (opcode == `OP_LOAD)  ;
    wire is_op_store  = (opcode == `OP_STORE) ;
    wire is_op_imm    = (opcode == `OP_IMM)   ;
    wire is_op_reg    = (opcode == `OP_REG)   ;

    wire  decode_en = inst_valid;


    /****************************************************************************
    *       Opcode Decoder
    ****************************************************************************/
    always @(posedge clk) begin
        if(decode_en) begin
            op_lui           <= is_op_lui;
            op_auipc         <= is_op_auipc;
            op_jal           <= is_op_jal;
            op_jalr          <= is_op_jalr;
            op_branch        <= is_op_branch;
            op_load          <= is_op_load;
            op_store         <= is_op_store;
            op_imm           <= is_op_imm;
            op_reg           <= is_op_reg;
            op_is_compressed <= inst_is_compressed;
        end
    end

    /****************************************************************************
    *       operand decoder
    ****************************************************************************/
    wire is_op_lui_auipc = is_op_lui | is_op_auipc; //for bit[31:21] and bit[10:5]
    wire is_op_lui_auipc_jal = is_op_lui_auipc | is_op_jal; //for bit [19:13]
    wire is_op_lui_auipc_jal_branch = is_op_lui_auipc_jal | is_op_branch; //for bit [19:13]
    wire is_branch_store = is_op_branch | is_op_store; //for bit[4:1]
    wire is_op_jalr_load_imm = is_op_jalr | is_op_load | is_op_imm; //for bit[0]
    wire signed [31:0] bits_signed;
    wire [31:0] bits_unsigned;

    assign bits_signed[31:20] = is_op_lui_auipc ? inst[31:20] : {12{inst[31]}};
    assign bits_signed[19:12] = is_op_lui_auipc_jal ? inst[19:12] : {8{inst[31]}};
    assign bits_signed[11]    = is_op_lui_auipc ? 1'b0 : is_op_jal ? inst[20] : is_op_branch ? inst[7] : inst[31];
    assign bits_signed[10:5]  = is_op_lui_auipc ? ALL0[11:5] : inst[30:25]; 
    assign bits_signed[4:1]   = is_op_lui_auipc ? ALL0[4:1] : is_branch_store ? inst[11:8] : inst[24:21];
    assign bits_signed[0]     = is_op_lui_auipc_jal_branch ? 1'b0 : is_op_store ? inst[7] : inst[20];

    assign bits_unsigned[31:21] = is_op_lui_auipc ? inst[31:21] : ALL0[31:21];      
    assign bits_unsigned[20]    = is_op_lui_auipc ? inst[20] : is_op_jal ? inst[31] : 1'b0;
    assign bits_unsigned[19:12] = is_op_lui_auipc_jal ? inst[19:12] : ALL0[19:12];
    assign bits_unsigned[11:0]  = bits_signed[11:0];

    always @(posedge clk) begin
        if(decode_en) begin 
            imm_signed    <= bits_signed;
            imm_unsigned  <= bits_unsigned;
            src1          <= inst[19:15];
            src2          <= inst[24:20];
            dest          <= inst[11:7];
            funct3        <= inst[14:12];
            funct7_bit5   <= inst[30];
        end
    end

/*******************************************************************************
*     valid control  
********************************************************************************/ 
always @(posedge clk or negedge rstb) begin
    if(~rstb) begin
        ex_valid <= 0;
        ex_pc <= 0;
        ex_pc_auipc <= 0;
        ex_pc_jmp <= 0;
        ex_pc_branch <= 0;
    end else begin
        if(inst_valid&~flush) begin
            ex_valid <= 1;
            ex_pc <= inst_pc;
            ex_pc_auipc <= inst_pc + $signed({bits_signed[31:12],12'h0}) + 4;
            ex_pc_jmp <= inst_pc + (inst_is_compressed ? 2 : 4);
            ex_pc_branch <= inst_pc + bits_signed;
        end else begin
            ex_valid <= 0;
        end
    end
end

//always @(posedge clk or negedge rstb) begin
//    if(~rstb) begin
//        id_jmp <= 0;
//    end else begin
//        if(decode_en&~id_jmp&~flush&is_op_jal) begin
//            id_jmp <= 1;
//        end else begin
//            id_jmp <= 0;
//        end
//    end
//end

assign id_jmp = decode_en & is_op_jal;
 
//always @(posedge clk) begin
//    if(decode_en&~flush&is_op_jal) begin
//        id_jmp_addr <= inst_pc + $signed({inst[31] ? ALL1[31:21]:ALL0[31:21], inst[31],inst[19:12],inst[20],inst[30:21],1'b0});
//    end
//end
assign id_jmp_addr = inst_pc + $signed({inst[31] ? ALL1[31:21]:ALL0[31:21], inst[31],inst[19:12],inst[20],inst[30:21],1'b0});
     
endmodule
