`include "glb.svh"

module d_mux#(
    parameter XLEN = 32,
    parameter ADDR_LEN = 14
)(
    input                       clk,
    input                       rstb,

    input        [ADDR_LEN-1:0] addr,
    input                       rd_req,
    output                      rd_ready,
    input                       wr_req,
    output                      wr_ready,
    input  logic [XLEN/8-1:0]   wr_be,
    input  logic [XLEN-1:0]     wr_data,
    output logic [XLEN-1:0]     rd_data,

    output logic [ADDR_LEN-3:0] ram_addr,
    output logic                ram_en,
    output logic [XLEN/8-1:0]   ram_we,
    output logic [XLEN-1:0]     ram_wr_data,
    input        [XLEN-1:0]     ram_rd_data,
    //input                       ram_rd_ready,

    output logic [15:0]         reg_addr,
    output logic                reg_wr_en,
    output logic [XLEN/8-1:0]   reg_wr_be,
    output logic [XLEN-1:0]     reg_wr_data,
    output logic                reg_rd_en,
    input        [XLEN-1:0]     reg_rd_data,
    input                       reg_rd_ready
);

    
// ready
logic ram_rd_ready;
logic ram_rd_req;
wire d_wr_ready = 1;
always @(posedge clk or negedge rstb) begin
    if(~rstb) begin
        ram_rd_ready <= 0;
    end else begin
        if(ram_rd_req&ram_rd_ready) begin
            ram_rd_ready <= 0;
        end else if(ram_rd_req) begin
            ram_rd_ready <= 1;
        end
    end
end

assign rd_ready = ram_rd_ready | reg_rd_ready;
assign wr_ready = 1;
 

// rd_data
wire d_is_from_reg = addr < `RAM_BASE_ADDR;
logic d_is_from_reg_dly;

always @(posedge clk) begin
    d_is_from_reg_dly <= d_is_from_reg;
end
assign rd_data = d_is_from_reg_dly ? reg_rd_data : ram_rd_data;

// wr_en, wr_be, wr_data
assign reg_wr_en = d_is_from_reg ? wr_req : 0;
assign reg_wr_be = wr_be;

assign ram_en = d_is_from_reg ? 0 : (wr_req | rd_req);
assign ram_rd_req = d_is_from_reg ? 0 : rd_req;
assign ram_we = wr_req ? wr_be : 0;

assign reg_wr_data = wr_data;
assign ram_wr_data = wr_data;

// reg_rd_en
assign reg_rd_en = d_is_from_reg ? rd_req : 0;

// addr, addr
assign ram_addr = (addr - `RAM_BASE_ADDR) >> 2;
assign reg_addr = (16'h0 | (addr - `REG_BASE_ADDR)) & 16'hFFFC;

endmodule
