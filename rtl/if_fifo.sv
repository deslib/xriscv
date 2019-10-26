/************************************************************************
*    instuction fifo. also handle compress instuction
*    bit     95 80  64  48  32  16  0
*    wr_sel          4       2      0
*    rd_sel      5   4   3   2   1  0
************************************************************************/
module if_fifo(
    input               clk,
    input               rstb,
    input               jmp,
    input               jmp_addr_bit1,
    input        [31:0] wr_data,
    input               wr_en,
    output logic        full,

    input               rd_en,
    output              empty,
    output logic [31:0] rd_data
);

    logic [127:0] inst_buf;


    logic [3:0] wr_sel_ext;
    logic [3:0] rd_sel_ext;

    wire wr_loop_bit = wr_sel_ext[3];
    wire rd_loop_bit = rd_sel_ext[3];
    wire [2:0] wr_sel = wr_sel_ext[2:0];
    wire [2:0] rd_sel = rd_sel_ext[2:0];
    

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            wr_sel_ext <= 0;
        end else begin
            if(jmp) begin
                wr_sel_ext <= 0;
            end else if(wr_en) begin
                wr_sel_ext <= wr_sel_ext + 2;
            end
        end
    end

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            inst_buf <= 0;
        end else begin
            if(wr_en) begin
                if(wr_sel == 0) begin
                    inst_buf[31:0] <= wr_data;
                end else if(wr_sel == 2) begin
                    inst_buf[63:32] <= wr_data;
                end else if(wr_sel == 4) begin
                    inst_buf[95:64] <= wr_data;
                end else begin
                    inst_buf[127:96] <= wr_data;
                end
            end
        end
    end

    assign rd_data = rd_sel == 0 ? inst_buf[31:0] :
                     rd_sel == 1 ? inst_buf[47:16] :
                     rd_sel == 2 ? inst_buf[63:32] :
                     rd_sel == 3 ? inst_buf[79:48] : 
                     rd_sel == 4 ? inst_buf[95:64] : 
                     rd_sel == 5 ? inst_buf[111:80] : 
                     rd_sel == 6 ? inst_buf[127:96] : 
                                   {inst_buf[15:0],inst_buf[127:112]} ;
    wire is_compress_inst = ~&rd_data[1:0];

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            rd_sel_ext <= 0;
        end else begin
            if(jmp) begin
                rd_sel_ext <= {3'h0,jmp_addr_bit1};
            end else if(rd_en) begin
                if(is_compress_inst) begin
                    rd_sel_ext <= rd_sel_ext + 1;
                end else begin
                    rd_sel_ext <= rd_sel_ext + 2;
                end
            end
        end
    end

    assign full = rd_loop_bit == wr_loop_bit ? (wr_sel == 6) & (rd_sel < 2) : wr_sel + 3 >= rd_sel;
    assign empty = rd_loop_bit == wr_loop_bit ? (rd_sel + 1 >= wr_sel) : (rd_sel == 7) & (wr_sel == 0);

endmodule

