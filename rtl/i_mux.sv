`include "glb.svh"

module i_mux#(
    parameter XLEN = 32,
    parameter ADDR_LEN = 14
)(
//    input                       clk,
//    input                       rstb,

    input        [ADDR_LEN-1:0] addr,
    output logic [XLEN-1:0]     rd_data,

    output logic [ADDR_LEN-3:0] rom_addr,
    input        [XLEN-1:0]     rom_data,
    output logic [ADDR_LEN-3:0] ram_addr,
    input        [XLEN-1:0]     ram_data
);


logic i_is_from_rom;
//always @(posedge clk) begin
//    i_is_from_rom <= (addr < `RAM_BASE_ADDR);
//end
assign i_is_from_rom = (addr & `RAM_BASE_ADDR_MASK) ? 1'b0 : 1'b1;
assign rd_data = i_is_from_rom ? rom_data : ram_data;

wire [ADDR_LEN-1:0] rom_addr_pre = addr;
wire [ADDR_LEN-1:0] ram_addr_pre = addr & `RAM_BASE_ADDR_UNMASK;

assign rom_addr = rom_addr_pre[ADDR_LEN-1:2];
assign ram_addr = ram_addr_pre[ADDR_LEN-1:2];

endmodule
