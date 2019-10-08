/*******************************************************************************
* Load and Store
********************************************************************************/ 
module ls(
    input                       clk,
    input                       rstb,

    input        [31:0]         rw_addr,
    input                       wr_en,
    input        [31:0]         wr_data,
    input                       rd_en,
    output logic [31:0]         rd_data,
    output logic                rd_done,
    output logic                wr_done,

    output logic [31:0]         d_addr,
    output logic [31:0]         d_wr_data,
    output logic                d_wr_req,
    input                       d_wr_ready,
    output logic                d_rd_req,
    input                       d_rd_ready,
    input        [31:0]         d_rd_data
);

endmodule
