`timescale 1ns / 1ps

module fifo#(
	parameter DATA_WIDTH = 8,
    parameter DATA_DEPTH = 1024,
    parameter PFULL_NUM = 2,
    parameter PEMPTY_NUM = 2
)(
	input	                 clk,
	input	                 rst,
	input	[DATA_WIDTH-1:0] din,
	input	                 wr_en,
	input	                 rd_en,
	output	[DATA_WIDTH-1:0] dout,
	output	                 full,
	output	                 empty,
	output	                 pfull,
	output	                 pempty
);

    localparam ADDR_WIDTH = $clog2(DATA_DEPTH);

	logic	[ADDR_WIDTH:0] wr_addr;
	logic	[ADDR_WIDTH:0] rd_addr;
	bit signed [DATA_WIDTH-1:0] ram_table [DATA_DEPTH];
	int i;
	logic	[DATA_WIDTH-1:0] reg_out;
	assign dout = reg_out;

	initial begin
		for(i=0;i<DATA_DEPTH;i++)begin
			ram_table[i] = 0;
		end
	end

	always @(posedge clk or posedge rst)begin
		if(rst)begin
			wr_addr <= 0;
		end else if(wr_en)begin
			wr_addr <= wr_addr + 1;
		end
	end

	always @(posedge clk or posedge rst)begin
		if(rst)begin
			rd_addr <= 0;
		end else if(rd_en)begin
			rd_addr <= rd_addr + 1;
		end
	end

	always @(posedge clk)begin
		if(wr_en)begin
			ram_table[wr_addr[ADDR_WIDTH-1:0]] <= din;
		end else if(rd_en)begin
			ram_table[rd_addr[ADDR_WIDTH-1:0]] <=0;
		end
	end

	always @(posedge clk)begin
		//if(rd_en)begin
			reg_out <= ram_table[rd_addr[ADDR_WIDTH-1:0]];
		//end
	end

    wire [ADDR_WIDTH:0] rd_addr_minus_1 = rd_addr - 1;


    assign full =  wr_addr[ADDR_WIDTH-1:0] == rd_addr_minus_1[ADDR_WIDTH-1:0];
    assign empty = wr_addr[ADDR_WIDTH-1:0] == rd_addr[ADDR_WIDTH-1:0];

    generate
        if(PFULL_NUM) begin
            wire [ADDR_WIDTH:0] rd_addr_minus_n = rd_addr - PFULL_NUM - 1;
            assign pfull = wr_addr[ADDR_WIDTH] != rd_addr_minus_n[ADDR_WIDTH] ? 
                           wr_addr[ADDR_WIDTH-1:0] > rd_addr_minus_n[ADDR_WIDTH-1:0] : 0;
        end

        if(PEMPTY_NUM) begin
            wire [ADDR_WIDTH:0] rd_addr_plus_n = rd_addr + PEMPTY_NUM - 1;
            assign pempty = wr_addr[ADDR_WIDTH] == rd_addr_plus_n[ADDR_WIDTH] ? 
                           wr_addr < rd_addr_plus_n : 0;
        end
    endgenerate

endmodule
