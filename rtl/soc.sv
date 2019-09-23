`include "glb.svh"

module soc#(
    parameter XLEN = 32,
    parameter ADDR_LEN = 14,
    parameter ROM_ADDR_LEN = 12,
    parameter RAM_ADDR_LEN = 12
)(
    input clk,
    input rstb_in,
    input uart_rx,
    output uart_tx,
    input btn,
    output [7:0] led
);

`include "reg_decl.vh"

logic [RAM_ADDR_LEN-1:0]    i_ram_addr;
logic [ROM_ADDR_LEN-1:0]    i_rom_addr;
logic [31:0]                i_addr;

logic [XLEN-1:0]            i_rom_data;
logic [XLEN-1:0]            i_ram_data;
logic [XLEN-1:0]            i_data;

logic [31:0]                d_addr;
logic                       d_wr_req;
logic [XLEN/8-1:0]          d_wr_be;
logic                       d_rd_req;
logic [XLEN-1:0]            d_rd_data;
logic [XLEN-1:0]            d_wr_data;
logic                       d_ready;

logic [RAM_ADDR_LEN-1:0]    d_ram_addr;
logic [XLEN-1:0]            d_ram_rd_data;
logic [XLEN-1:0]            d_ram_wr_data;
logic [XLEN/8-1:0]          d_ram_we;
logic                       d_ram_en;

logic [15:0]                reg_addr;
logic                       reg_wr_en;
logic [XLEN/8-1:0]          reg_wr_be;
logic                       reg_rd_en;
logic                       reg_rd_ready;
logic [XLEN-1:0]            reg_rd_data;
logic [XLEN-1:0]            reg_wr_data;


/**************************************************************************************************
*     Reset 
**************************************************************************************************/
logic [1:0] rstb_in_pipe;
always @(posedge clk or negedge rstb_in) begin
    if(~rstb_in) begin
        rstb_in_pipe <= 0;
    end else begin
        rstb_in_pipe[0] <= 1;
        rstb_in_pipe[1] <= rstb_in_pipe[0];
    end
end
wire rstb = rstb_in_pipe[1];

i_mux #(
    .XLEN(32),
    .ADDR_LEN(14)
)U_I_MUX(
    .clk(clk),
    .rstb(rstb),

    .addr(i_addr[ADDR_LEN-1:0]),
    .rd_data(i_data),

    .rom_addr(i_rom_addr),
    .rom_data(i_rom_data),
    .ram_addr(i_ram_addr),
    .ram_data(i_ram_data)
);

d_mux#(
    .XLEN(32),
    .ADDR_LEN(14)
)U_D_MUX(
    .clk(clk),
    .rstb(rstb),

    .addr(d_addr[ADDR_LEN-1:0]),
    .rd_req(d_rd_req),
    .rd_ready(d_rd_ready),
    .wr_req(d_wr_req),
    .wr_ready(d_wr_ready),
    .wr_be(d_wr_be),
    .wr_data(d_wr_data),
    .rd_data(d_rd_data),

    .ram_addr(d_ram_addr),
    .ram_en(d_ram_en),
    .ram_we(d_ram_we),
    .ram_wr_data(d_ram_wr_data),
    .ram_rd_data(d_ram_rd_data),

    .reg_addr(reg_addr),
    .reg_wr_en(reg_wr_en),
    .reg_wr_be(reg_wr_be),
    .reg_wr_data(reg_wr_data),
    .reg_rd_en(reg_rd_en),
    .reg_rd_data(reg_rd_data),
    .reg_rd_ready(reg_rd_ready)
);

core U_CORE(
    .clk(clk),
    .rstb(rstb),

    .i_data(i_data),
    .i_addr(i_addr),

    .d_addr(d_addr),
    .d_wr_req(d_wr_req),
    .d_wr_ready(d_wr_ready),
    .d_rd_req(d_rd_req),
    .d_rd_ready(d_rd_ready),
    .d_wr_be(d_wr_be),
    .d_wr_data(d_wr_data),
    .d_rd_data(d_rd_data)
);

ram_sdp #(
    .DATA_WIDTH(32),
    .DATA_DEPTH(2**(RAM_ADDR_LEN)),
    .INIT_FN("../c/xriscv.ram")
)U_RAM(
    .clk(clk),
    .ena(d_ram_en),
    .wea(d_ram_we),
    .addra(d_ram_addr),
    .dina(d_ram_wr_data),
    .douta(d_ram_rd_data),
    .addrb(i_ram_addr),
    .doutb(i_ram_data)
);

rom #(
    .DATA_WIDTH(32),
    .DATA_DEPTH(2**(ROM_ADDR_LEN)),
    .INIT_FN("../c/xriscv.rom")
)U_ROM(
    .clk(clk),
    .addr(i_rom_addr),
    .dout(i_rom_data)
);

regfile U_REGFILE(
    .clk(clk),
    .rstb(rstb),
    .wr_en(reg_wr_en),
    .be(reg_wr_be),
    .wr_addr(reg_addr),
    .wdata(reg_wr_data),

    `include "reg_conn.vh"

    .rd_en(reg_rd_en),
    .rd_addr(reg_addr),
    .rdata(reg_rd_data),
    .rd_rdy(reg_rd_ready)
);

`ifdef SIM
    integer fp;
    initial begin
        fp = $fopen("uart.txt","w");
    end
    always @(posedge clk) begin
        if(reg_wr_en) begin
            if(reg_addr == 0 && reg_wr_be == 4'b0010) begin
                $fwrite(fp,"%c",reg_wr_data[15:8]);
            end
        end
    end
`endif

endmodule
