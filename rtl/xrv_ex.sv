`include "glb.svh"
/*******************************************************************************
*  Execuate
* *******************************************************************************/ 
module xrv_ex(
    input                       clk,
    input                       rstb,
    //input                       flush,

    output logic                ex_jmp,
    output logic [31:0]         ex_jmp_addr,
    output logic                ncycle_alu_wait,
    output                      ls_done,

    input                       ex_valid,
    input        [31:0]         ex_pc,
    input                       op_lui,              
    input                       op_auipc,
    input                       op_jal,  
    input                       op_jalr,
    input                       op_branch,
    input                       op_load,
    input                       op_store,
    input                       op_imm,
    input                       op_reg,
    input                       op_is_compressed,

    input        [31:0]         imm_signed,
    input        [31:0]         imm_unsigned,
    input        [4:0]          src1,
    input        [4:0]          src2,
    input        [4:0]          dest,
    input        [2:0]          funct3,
    input        [6:0]          funct7,

    output logic [31:0]         d_addr,
    output logic                d_wr_req,
    output logic [3 :0]         d_be,
    output logic [31:0]         d_wr_data,
    input                       d_wr_ready,
    output logic                d_rd_req,
    input                       d_rd_ready,
    input        [31:0]         d_rd_data
);

    localparam logic [31:0] ALL0 = 32'h0;
    localparam logic [31:0] ALL1 = 32'hFFFF_FFFF; 

    logic op_imm_dly;
    logic op_reg_dly;


    (* ram_style = "distributed" *) logic [31:0] x_reg[0:31];

    `ifdef DBG
        integer fp;
        integer tick;
        initial begin
            tick = 0;
            fp = $fopen("core.log","w");
        end
        wire [31:0][31:0] x;
        genvar i;
        generate 
            for(i=0;i<32;i++) begin
                assign x[i] = x_reg[i];
            end
        endgenerate
    `endif


    initial begin
        int i;
        for(i=0;i<32;i++) begin
            x_reg[i] = 0;
        end
    end

    wire [31:0] reg1 = x_reg[src1];
    wire [31:0] reg2 = x_reg[src2];

    wire signed [31:0] operand1 = reg1;
    wire operand2_is_unsigned = (funct3 == 3);
    wire signed [31:0] operand2 = (op_reg|op_branch|op_store) ? reg2 : (operand2_is_unsigned ? imm_unsigned : imm_signed);

    logic [31:0] dest_reg;
    logic dest_reg_wr_en;

    wire ex_en = ex_valid&~ex_jmp;

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            op_imm_dly <= 0;
            op_reg_dly <= 0;
        end else begin
            op_imm_dly <= op_imm;
            op_reg_dly <= op_reg;
        end
    end 

    wire [31:0] operand_rs =(funct7[5] ? operand1 >>> operand2[4:0] : operand1 >> operand2[4:0]);
    logic [31:0] dest_reg_pre;
    always @(posedge clk) begin
        dest_reg_pre <= funct3 == 3'b000 ? ( (funct7[5]&op_reg) ? operand1 - operand2 : operand1 + operand2) :
                        funct3 == 3'b001 ? operand1 << operand2[4:0] :
                        funct3 == 3'b010 ? (operand1 < operand2 ? 32'h1 : 32'h0) :
                        funct3 == 3'b011 ? ( ($unsigned(operand1) < $unsigned(operand2)) ? 32'h1 : 32'h0) :
                        funct3 == 3'b100 ? operand1 ^ operand2 :
                        funct3 == 3'b101 ? operand_rs :
                        funct3 == 3'b110 ? operand1 | operand2 :
                                           operand1 & operand2;
    end 

    logic ncycle_alu_cmp;
    assign ncycle_alu_wait = ex_en & (op_imm|op_reg) & ~ncycle_alu_cmp;

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            ncycle_alu_cmp <= 0;
        end else begin
            if(ex_en & (op_imm | op_reg) ) begin
                ncycle_alu_cmp <= 1;
            end else begin
                ncycle_alu_cmp <= 0;
            end
        end
    end 

    always @(*) begin
            dest_reg_wr_en = 0;
            dest_reg = 0;
            if(d_rd_req&d_rd_ready) begin
                dest_reg = funct3 == 3'b000 ? (
                                    d_addr[1:0] == 0 ? {d_rd_data[7] ? ALL1[31:8] : ALL0[31:8],d_rd_data[7:0]} :
                                    d_addr[1:0] == 1 ? {d_rd_data[15] ? ALL1[31:8] : ALL0[31:8], d_rd_data[15:8]} :
                                    d_addr[1:0] == 2 ? {d_rd_data[23] ? ALL1[31:8] : ALL0[31:8], d_rd_data[23:16]} :
                                                       {d_rd_data[31] ? ALL1[31:8] : ALL0[31:8], d_rd_data[31:24]}
                               ) :
                               funct3 == 3'b001 ? (
                                   d_addr[1] == 0 ? {d_rd_data[15] ? ALL1[31:16] : ALL0[31:16], d_rd_data[15:0]} :
                                                    {d_rd_data[31] ? ALL1[31:16] : ALL0[31:16], d_rd_data[31:16]}
                               ) :
                               funct3 == 3'b010 ? d_rd_data[31:0] :
                               funct3 == 3'b100 ? (
                                    d_addr[1:0] == 0 ? {ALL0[31:8],d_rd_data[7:0]} :
                                    d_addr[1:0] == 1 ? {ALL0[31:8],d_rd_data[15:8]} :
                                    d_addr[1:0] == 2 ? {ALL0[31:8],d_rd_data[23:16]} :
                                                       {ALL0[31:8],d_rd_data[31:24]}
                               ):
                               funct3 == 3'b101 ? (
                                   d_addr[1] == 0 ? {ALL0[31:16],d_rd_data[15:0]} : {ALL0[31:16],d_rd_data[31:16]}
                               ) : 32'h0;
                dest_reg_wr_en = 1;
                `LOG_CORE($sformatf("PC=%05x OP_LOAD %08x from %08x\n", ex_pc, d_rd_data, d_addr));
            end else if(ex_en) begin
                if(op_lui) begin
                    dest_reg = imm_signed;
                    dest_reg_wr_en = 1;
                    `LOG_CORE($sformatf("PC=%05x LUI\n",ex_pc));
                end else if(op_auipc) begin
                    dest_reg = ex_pc + $signed(imm_signed&32'hFFFFF000) + 4;
                    dest_reg_wr_en = 1;
                    `LOG_CORE($sformatf("PC=%05x AUIPC \n",ex_pc));
                end else if(op_jal|op_jalr) begin
                    dest_reg = ex_pc + (op_is_compressed ? 2 : 4);
                    dest_reg_wr_en = 1;
                    `LOG_CORE($sformatf("PC=%05x OP_JAL|OP_JALR\n",ex_pc));
                end
            end else if(ncycle_alu_cmp) begin
                if(op_imm|op_reg) begin
                    dest_reg = dest_reg_pre;
                    dest_reg_wr_en = 1;
                end
            end
    end

    always @(posedge clk) begin
        if(dest_reg_wr_en & (|dest)) begin
            x_reg[dest] <= dest_reg;
        end
    end
     

     
/*******************************************************************************
* JMP
********************************************************************************/ 
    logic operand_eq;
    logic operand_lt;
    logic operand_ltu;
    assign operand_eq  = reg1 == reg2;
    assign operand_lt  = $signed(reg1) < $signed(reg2);
    assign operand_ltu = $unsigned(reg1) < $unsigned(reg2);

    wire branch = op_branch & ( ( (funct3 == 3'b000) & operand_eq) |
                                ( (funct3 == 3'b001) & ~operand_eq) |
                                ( (funct3 == 3'b100) & operand_lt)  |
                                ( (funct3 == 3'b101) & ~operand_lt )  |
                                ( (funct3 == 3'b110) & operand_ltu ) |
                                ( (funct3 == 3'b111) & ~operand_ltu ) );
                            
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            ex_jmp <= 0;
        end else begin
            if(ex_en) begin
                ex_jmp <= branch|op_jalr;
            end else begin
                ex_jmp <= 0;
            end
        end
    end
     
    always @(posedge clk) begin
        ex_jmp_addr <= branch ? ex_pc + imm_signed : reg1 + imm_signed;
    end

/*******************************************************************************
*  Load/Store 
********************************************************************************/ 
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            d_wr_req <= 0;
        end else begin
            if(d_wr_ready) begin
                d_wr_req <= 0;
            end else if(ex_en&op_store)begin
                d_wr_req <= 1;
            end
        end
    end

    wire [1:0] laddr = operand1[1:0] + imm_signed[1:0];

    always @(posedge clk) begin
        if(op_store&ex_en) begin
            d_wr_data <= funct3 == 0 ? 
                            (laddr == 0 ? {24'h0,reg2[7:0]} :
                             laddr == 1 ? {16'h0,reg2[7:0],8'h0} :
                             laddr == 2 ? {8'h0, reg2[7:0],16'h0} :
                                          {reg2[7:0],24'h0}) :
                         funct3 == 1 ?
                               (laddr[1] ? {reg2[15:0],16'h0} : {16'h0, reg2[15:0]}) : 
                         reg2;
        end
    end

    always @(posedge clk) begin
        if((op_store|op_load)&ex_en) begin
            d_be <=  funct3[1:0] == 0 ? 
                            (laddr == 0 ? 4'h1 : 
                            laddr == 1 ? 4'h2 :
                            laddr == 2 ? 4'h4 : 4'h8) :
                     funct3[1:0] == 1 ? 
                               (laddr[1] ? 4'hc : 4'h3) :
                     4'hf;
        end
    end

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            d_rd_req <= 0;
        end else begin
            if(d_rd_ready) begin
                d_rd_req <= 0;
            end else if(ex_en&op_load) begin
                d_rd_req <= 1;
            end
        end
    end

    always @(posedge clk) begin
        if((op_store|op_load)&ex_en) begin
            d_addr <= x_reg[src1] + imm_signed;
        end
    end
     

    assign ls_done = (d_rd_req&d_rd_ready) | (d_wr_req&d_wr_ready);
     
endmodule
