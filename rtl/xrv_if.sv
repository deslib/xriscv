/*******************************************************************************
*   instruction fetcher 
********************************************************************************/ 
`include "glb.svh"
module xrv_if(
    input               clk,
    input               rstb,

    input               stalling,
    input               jmp,
    input        [31:0] jmp_addr,

    input        [31:0] i_data,
    output logic [31:0] i_addr,

    output logic        is_ls,
    `ifdef EN_MULT_DIV
    output logic        is_mult_div,
    `endif
    output logic [31:0] inst,
    output logic [31:0] inst_pc,
    output logic        inst_is_compressed,
    output logic        inst_valid
);
/*******************************************************************************
*   fetch data into fifo
********************************************************************************/ 
    logic [31:0] i_data_align;
    logic        i_data_req;
    logic        i_data_wr_en;
    logic        i_data_rd_en;
    logic        i_data_full;
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            i_data_req <= 0;
        end else begin
            if(jmp) begin
                i_data_req <= 1;
            end else if(~i_data_full) begin
                i_data_req <= 1;
            end else begin
                i_data_req <= 0;
            end
        end
    end
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            i_addr <= 0;
        end else begin
            if(jmp) begin
                i_addr <= jmp_addr;
            end else if(i_data_req&~i_data_full) begin
                i_addr <= i_addr + 4;
            end
        end
    end
     
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            i_data_wr_en <= 0;
        end else begin
            i_data_wr_en <= i_data_req & ~i_data_full & ~jmp;
        end
    end
     
    if_fifo U_IF_FIFO(
        .clk(clk),
        .rstb(rstb),
        .jmp(jmp),
        .jmp_addr_bit1(jmp_addr[1]),
        .wr_data(i_data),
        .wr_en(i_data_wr_en),
        .rd_en(i_data_rd_en),
        .rd_data(i_data_align),
        .full(i_data_full),
        .empty(i_data_empty)
    );

/*******************************************************************************
*   Fetch i_data from fifo and do inst decompress
********************************************************************************/ 
    logic [31:0] inst_decompress;
    `ifdef OP_IMM_REG_2_STAGE
    logic wait_cycle;
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            wait_cycle <= 0;
        end else begin
            if(i_data_rd_en) begin
                wait_cycle <= ~(inst_decompress[6]|~inst_decompress[4]|inst_decompress[3]|inst_decompress[2]); //op_imm and op_reg need 2cycles
            end else begin
                wait_cycle <= 0;
            end
        end
    end 
    `else
    wire wait_cycle = 0;
    `endif
    //assign i_data_rd_en = ~i_data_empty & ~stalling & ~jmp & ~wait_cycle;
    assign i_data_rd_en = ~i_data_empty & ~stalling & ~wait_cycle;
     
    xrv_i_decompress U_XRV_I_DECOMPRESS(
        .data_in(i_data_align),
        .data_out(inst_decompress)
    );
     
/*******************************************************************************
*   PC
********************************************************************************/  
    logic [31:0] pc;
    logic during_jmp;
    logic [1:0] jmp_dly;
    logic jmp_addr_bit1;
    wire inst_is_compressed_pre = ~&i_data_align[1:0];
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            jmp_dly <= 0;
        end else begin
            jmp_dly <= {jmp_dly[0],jmp};
        end
    end
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            jmp_addr_bit1 <= 0;
        end else begin
            if(jmp) begin
                jmp_addr_bit1 <= jmp_addr[1];
            end
        end
    end 
    wire jmp_settle = jmp_addr_bit1 ? jmp_dly[1] : jmp_dly[0];
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            during_jmp <= 0;
        end else begin
            if(jmp) begin
                during_jmp <= 1;
            end else if(jmp_settle) begin
                during_jmp <= 0;
            end
        end
    end

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            pc <= 0;
        end else begin
            if(jmp) begin
                pc <= jmp_addr;
            end else if(i_data_rd_en) begin
                pc <= pc + (inst_is_compressed_pre ? 32'h2 : 32'h4);
            end
        end
    end

/*******************************************************************************
*   load & store
********************************************************************************/ 
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            is_ls <= 0;
        end else begin
            if(i_data_rd_en) begin
                is_ls <= ~((|inst_decompress[4:2])|inst_decompress[6]);
            end else begin
                is_ls <= 0;
            end
        end
    end

    `ifdef EN_MULT_DIV
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            is_mult_div <= 0;
        end else begin
            if(i_data_rd_en) begin
                is_mult_div <= (inst_decompress[6:2] == `OP_MULT_DIV && inst_decompress[31:25] == 'h1) & ~inst_is_compressed;
            end else begin
                is_mult_div <= 0;
            end
        end
    end 
    `endif

/*******************************************************************************
*   inst valid control
********************************************************************************/ 
    logic inst_en;
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            inst_en <= 0;
        end else begin
            if(jmp) begin
                inst_en <= 0;
            end else if(during_jmp) begin
                inst_en <= 0;
            end else begin
                inst_en <= 1;
            end
        end
    end
     
    assign inst_valid = i_data_rd_en;
    assign inst_pc = pc;
    assign inst = inst_decompress;
    assign inst_is_compressed = inst_is_compressed_pre;

    //always @(posedge clk or negedge rstb) begin
    //    if(~rstb) begin
    //        inst_valid <= 0;
    //    end else begin
    //        inst_valid <= i_data_rd_en;
    //    end
    //end 

    //always @(posedge clk) begin
    //    inst <= inst_decompress;
    //    inst_pc <= pc;
    //    inst_is_compressed <= inst_is_compressed_pre;
    //end 

endmodule
