`include "glb.svh"

module d_mux#(
    parameter XLEN = 32,
    parameter ADDR_LEN = 14
)(
    input                       clk,
    input                       rstb,

    input        [ADDR_LEN-1:0] addr,
    input                       rd_req,
    output logic                rd_ready,
    input                       wr_req,
    output logic                wr_ready,
    input  logic [XLEN/8-1:0]   be,
    input  logic [XLEN-1:0]     wr_data,
    output logic [XLEN-1:0]     rd_data,

    output logic [ADDR_LEN-3:0] ram_addr,
    output logic                ram_en,
    output logic [XLEN/8-1:0]   ram_we,
    output logic [XLEN-1:0]     ram_wr_data,
    input        [XLEN-1:0]     ram_rd_data,
    //input                       ram_rd_ready,

    output logic [15:0]         io_addr,
    output logic                io_wr_req,
    output logic [XLEN/8-1:0]   io_be,
    output logic [XLEN-1:0]     io_wr_data,
    output logic                io_rd_req,
    input        [XLEN-1:0]     io_rd_data,
    input                       io_rd_ready,
    input                       io_wr_ready
);

    
// ready
logic ram_rd_req;
logic ram_rd_ready_pre;
logic ram_rd_ready;
logic [31:0] ram_rd_data_dly;
logic ram_wr_req;
logic ram_wr_ready;
//wire  ram_wr_ready = 1;
wire d_is_from_io = (addr & `RAM_BASE_ADDR_MASK) ? 1'b1 : 1'b0;
logic d_is_from_io_dly;

always @(posedge clk or negedge rstb) begin
    if(~rstb) begin
        ram_wr_ready <= 0;
    end else begin
        if(ram_wr_req&ram_wr_ready) begin
            ram_wr_ready <= 0;
        end else if(ram_wr_req) begin
            ram_wr_ready <= 1;
        end
    end
end
 
always @(posedge clk or negedge rstb) begin
    if(~rstb) begin
        ram_rd_ready_pre <= 0;
    end else begin
        if(ram_rd_req&ram_rd_ready) begin
            ram_rd_ready_pre <= 0;
        end else if(ram_rd_req) begin
            ram_rd_ready_pre <= 1;
        end
    end
end 

always @(posedge clk or negedge rstb) begin
    if(~rstb) begin
        ram_rd_ready <= 0;
    end else begin
        ram_rd_ready <= ram_rd_ready_pre;
    end
end

assign rd_ready = d_is_from_io_dly ? io_rd_ready&rd_req : ram_rd_ready;
assign wr_ready = d_is_from_io_dly ? io_wr_ready&wr_req : ram_wr_ready;

//always @(posedge clk or negedge rstb) begin
//    if(~rstb) begin
//        rd_ready <= 0;
//        wr_ready <= 0;
//    end else begin
//        rd_ready <= d_is_from_io ? io_rd_ready & io_rd_req : ram_rd_ready;
//        wr_ready <= d_is_from_io ? io_wr_ready : ram_wr_ready;
//    end
//end

// rd_data

always @(posedge clk) begin
    ram_rd_data_dly <= ram_rd_data;
end 
always @(posedge clk) begin
    d_is_from_io_dly <= d_is_from_io;
end
assign rd_data = d_is_from_io_dly ? io_rd_data : ram_rd_data_dly;

// wr_en, be, wr_data
assign io_wr_req = d_is_from_io ? wr_req : 1'b0;
assign io_be = be;

assign ram_en = d_is_from_io ? 1'b0 : (wr_req | rd_req);
assign ram_rd_req = d_is_from_io ? 1'b0 : rd_req;
assign ram_wr_req = d_is_from_io ? 1'b0 : wr_req;
assign ram_we = wr_req ? be : 4'h0;

assign io_wr_data = wr_data;
assign ram_wr_data = wr_data;

// io_rd_req
assign io_rd_req = d_is_from_io ? rd_req : 1'b0;

// addr, addr
assign ram_addr = (addr & `RAM_BASE_ADDR_UNMASK) >> 2;
assign io_addr = addr & `IO_BASE_ADDR_UNMASK;

endmodule
