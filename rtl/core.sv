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

endmodule    
