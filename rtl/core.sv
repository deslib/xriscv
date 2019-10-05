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

    localparam logic [31:0] ALL0 = 32'h0;
    localparam logic [31:0] ALL1 = 32'hFFFF_FFFF; 

    logic [31:0] pc;
    logic [31:0] next_pc;
    logic pipe_flush_pre;
    logic pipe_flush;

    `ifdef DBG
        integer fp;
        integer tick;
        initial begin
            tick = 0;
            fp = $fopen("core.log","w");
        end
        logic [31:0] prev_pc;
        always @(posedge clk) begin
            prev_pc <= pc;
        end
    `endif


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


    logic [31:1][31:0] x_reg;
    wire [31:0][31:0]x = {x_reg,32'h0};

    wire wait_rd_data = d_rd_req & ~d_rd_ready;
    wire wait_wr_data = d_wr_req & ~d_wr_ready;
    wire stalling = wait_rd_data | wait_wr_data;


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
        end else if(~stalling) begin
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
    always @(posedge clk) begin
        if(~stalling) begin //wait load data
            imm_signed   <= (is_op_lui | is_op_auipc)         ? {i_data[31:12],ALL0[11:0]} :
                            (is_op_jal)                       ? {i_data[31] ? ALL1[31:21]:ALL0[31:21], i_data[31],i_data[19:12],i_data[20],i_data[30:21],1'b0} :
                            (is_op_jalr|is_op_imm|is_op_load) ? {i_data[31] ? ALL1[31:12]:ALL0[31:12], i_data[31:20]} :
                            (is_op_branch)                    ? {i_data[31] ? ALL1[31:12]:ALL0[31:12], i_data[31],i_data[7],i_data[30:25],i_data[11:8],1'b0} :
                            (is_op_store)                     ? {i_data[31] ? ALL1[31:12]:ALL0[31:12], i_data[31:25], i_data[11:7]} : 32'h0;

            imm_unsigned <= (is_op_lui | is_op_auipc)         ? {i_data[31:12],ALL0[11:0]} :
                            (is_op_jal)                       ? {ALL0[31:21], i_data[31],i_data[19:12],i_data[20],i_data[30:21],1'b0} :
                            (is_op_jalr|is_op_imm|is_op_load) ? {ALL0[31:12], i_data[31:20]} :
                            (is_op_branch)                    ? {ALL0[31:12], i_data[31],i_data[7],i_data[30:25],i_data[11:8],1'b0} :
                            (is_op_store)                     ? {ALL0[31:12], i_data[31:25], i_data[11:7]} : 32'h0;

            src1 <= i_data[19:15];
            src2 <= i_data[24:20];
            dest <= i_data[11:7];
            funct3 <= i_data[14:12];
            funct7 <= i_data[31:25];
        end
    end

    wire signed [31:0] operand1 = x[src1];
    wire operand2_is_unsigned = (funct3 == 3);
    wire signed [31:0] operand2 = (op_reg|op_branch|op_store) ? x[src2] : (operand2_is_unsigned ? imm_unsigned : imm_signed);

    /****************************************************************************
    *       alu
    ****************************************************************************/
    wire [31:0] operand_rs =(funct7[5] ? operand1 >>> operand2[4:0] : operand1 >> operand2[4:0]);
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            x_reg[31:1] <= 0;
        end else begin
            if(!pipe_flush) begin
                if(d_rd_req & d_rd_ready) begin
                    x_reg[dest] <= funct3 == 3'b000 ? (
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
                    `LOG_CORE($sformatf("PC=%05x OP_LOAD %08x from %08x\n", prev_pc, d_rd_data, d_addr));
                end else if(op_lui) begin
                    x_reg[dest] <= imm_signed;
                    `LOG_CORE($sformatf("PC=%05x LUI\n",prev_pc));
                end else if(op_auipc) begin
                    x_reg[dest] <= pc + $signed(imm_signed&32'hFFFFF000) - 4;
                    `LOG_CORE($sformatf("PC=%05x AUIPC \n",prev_pc));
                end else if(op_jal|op_jalr) begin
                    x_reg[dest] <= pc;
                    `LOG_CORE($sformatf("PC=%05x OP_JAL|OP_JALR\n",prev_pc));
                end else if(op_imm|op_reg) begin
                    x_reg[dest] <= funct3 == 3'b000 ? ( (funct7[5]&op_reg) ? operand1 - operand2 : operand1 + operand2) :
                                   funct3 == 3'b001 ? operand1 << operand2[4:0] :
                                   funct3 == 3'b010 ? (operand1 < operand2 ? 32'h1 : 32'h0) :
                                   funct3 == 3'b011 ? ( ($unsigned(operand1) < $unsigned(operand2)) ? 32'h1 : 32'h0) :
                                   funct3 == 3'b100 ? operand1 ^ operand2 :
                                   funct3 == 3'b101 ? operand_rs :
                                   funct3 == 3'b110 ? operand1 | operand2 :
                                                      operand1 & operand2;
                end
            end
        end
    end

    wire [1:0] laddr = operand1[1:0] + imm_signed[1:0];
    
    assign d_wr_data = funct3 == 0 ? 
                           (laddr == 0 ? {24'h0,operand2[7:0]} :
                            laddr == 1 ? {16'h0,operand2[7:0],8'h0} :
                            laddr == 2 ? {8'h0, operand2[7:0],16'h0} :
                                         {operand2[7:0],24'h0}
                           ) :
                       funct3 == 1 ?
                            (laddr[1] ? {operand2[15:0],16'h0} : {16'h0, operand2[15:0]}) : 
                           operand2;

    assign d_be =  funct3[1:0] == 0 ? 
                        (laddr == 0 ? 4'h1 : 
                        laddr == 1 ? 4'h2 :
                        laddr == 2 ? 4'h4 : 4'h8) :
                   funct3[1:0] == 1 ? 
                           (laddr[1] ? 4'hc : 4'h3) :
                   4'hf;


    assign d_wr_req = op_store & ~pipe_flush;

    assign d_addr = x[src1] + imm_signed;

     
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            d_rd_req <= 0;
        end else begin
            if(is_op_load&~pipe_flush_pre) begin
                d_rd_req <= 1;
            end else if(d_rd_ready) begin
                d_rd_req <= 0;
            end
        end
    end

    `ifdef DBG
        always @(posedge clk) begin
            if(op_store & ~pipe_flush) begin
                `LOG_CORE($sformatf("PC=%05x OP_STORE %08x to %08x\n",prev_pc,operand2,x[src1]+imm_signed));
            end
        end
    `endif

    /****************************************************************************
    *         PC control (Jump or Branch)
    ****************************************************************************/
    wire operand_lt = operand1 < operand2;
    wire operand_ltu = $unsigned(operand1) < $unsigned(operand2);
    wire branch = op_branch & ( ( (funct3 == 3'b000) & (operand1 == operand2)) |
                                ( (funct3 == 3'b001) & (operand1 != operand2)) |
                                ( (funct3 == 3'b100) & operand_lt)  |
                                ( (funct3 == 3'b101) & !operand_lt )  |
                                ( (funct3 == 3'b110) & operand_ltu ) |
                                ( (funct3 == 3'b111) & !operand_ltu ) );

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            pc <= 0;
        end else begin
            pc <= next_pc;
        end
    end

    wire [31:0] jalr_addr = operand1 + imm_signed;
    always @(*) begin
        if(~rstb) begin
            next_pc = 0;
        end else begin
            if(pipe_flush) begin
                next_pc = pc + 4;
            end else if(stalling) begin
                next_pc = pc;
            end else if(op_jal|branch) begin
                next_pc = pc + imm_signed - 4; //-4 because the pipe already added 4
            end else if(op_jalr) begin
                next_pc = {jalr_addr[31:1],1'b0};
            end else begin
                next_pc = pc + 4;
            end
        end
    end 

    assign i_addr = next_pc;

    /****************************************************************************
    *         Pipeline flush 
    ****************************************************************************/
    assign pipe_flush_pre = (op_jal | op_jalr | branch) & ~pipe_flush;
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            pipe_flush <= 0;
        end else begin
            if(~pipe_flush) begin //flush need to go down if the opcode next to the jmp/branch is also jmp/branch
                pipe_flush <= pipe_flush_pre;
            end else begin
                pipe_flush <= 0;
            end
        end
    end
     
    `ifdef DBG

        integer fp_pc;
        initial begin
            fp_pc = $fopen("pc.log","w");
        end
        logic [1:0][31:0] dbg_pc_pipe;
        always @(posedge clk) begin
            dbg_pc_pipe <= {dbg_pc_pipe[0],pc};
            if(pc != 0 && ~pipe_flush && ~(~d_rd_req&d_rd_ready)) begin
                $fwrite(fp_pc,"%0x ",dbg_pc_pipe[0]);
                for(int i=0;i<32;i++) begin
                    $fwrite(fp_pc,"%08x ",x[i]);
                end
                $fwrite(fp_pc,"\n");
            end
        end
    `endif
     

endmodule

