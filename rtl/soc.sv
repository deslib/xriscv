`include "glb.svh"

module soc#(
    parameter XLEN = 32,
    parameter ADDR_LEN = 16,
    parameter ROM_ADDR_LEN = 12,
    parameter RAM_ADDR_LEN = ADDR_LEN-2
)(
    input clk,
    input rstb_in,
    input sw_uart_upgrade_b, //software upgrade
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
logic [XLEN/8-1:0]          d_be;
logic                       d_rd_req;
logic [XLEN-1:0]            d_rd_data;
logic [XLEN-1:0]            d_wr_data;

logic [RAM_ADDR_LEN-1:0]    d_ram_addr;
logic [XLEN-1:0]            d_ram_rd_data;
logic [XLEN-1:0]            d_ram_wr_data;
logic [XLEN/8-1:0]          d_ram_we;
logic                       d_ram_en;

logic [15:0]                io_addr;
logic                       io_wr_en;
logic [XLEN/8-1:0]          io_be;
logic                       io_rd_en;
logic                       io_rd_ready;
logic                       io_wr_ready;
logic [XLEN-1:0]            io_rd_data;
logic [XLEN-1:0]            io_wr_data;

// The signals used for software ram upgrade
logic                       uart_ram_wr_en;
logic [XLEN-1:0]            uart_ram_wr_data;
logic [XLEN/8-1:0]          uart_ram_we;
logic [RAM_ADDR_LEN-1:0]    uart_ram_addr;

wire [RAM_ADDR_LEN-1:0]     ram_addr    = uart_ram_wr_en ? uart_ram_addr : d_ram_addr;
wire [XLEN-1:0]             ram_wr_data = uart_ram_wr_en ? uart_ram_wr_data : d_ram_wr_data;
wire [XLEN/8-1:0]           ram_we      = uart_ram_wr_en ? uart_ram_we : d_ram_we;

wire                        during_sw_upgrade;

// uart-core handshake signals
logic                       uart_wr_ready;
logic                       uart_rd_ready;
logic                       uart_wr_req;
logic                       uart_rd_req;
logic [7:0]                 uart_wr_data;

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
    .ADDR_LEN(ADDR_LEN)
)U_I_MUX(
    .addr(i_addr[ADDR_LEN-1:0]),
    .rd_data(i_data),

    .rom_addr(i_rom_addr),
    .rom_data(i_rom_data),
    .ram_addr(i_ram_addr),
    .ram_data(i_ram_data)
);

d_mux#(
    .XLEN(32),
    .ADDR_LEN(ADDR_LEN)
)U_D_MUX(
    .clk(clk),
    .rstb(rstb),

    .addr(d_addr[ADDR_LEN-1:0]),
    .rd_req(d_rd_req),
    .rd_ready(d_rd_ready),
    .wr_req(d_wr_req),
    .wr_ready(d_wr_ready),
    .be(d_be),
    .wr_data(d_wr_data),
    .rd_data(d_rd_data),

    .ram_addr(d_ram_addr),
    .ram_en(d_ram_en),
    .ram_we(d_ram_we),
    .ram_wr_data(d_ram_wr_data),
    .ram_rd_data(d_ram_rd_data),

    .io_addr(io_addr),
    .io_wr_en(io_wr_en),
    .io_be(io_be),
    .io_wr_data(io_wr_data),
    .io_rd_en(io_rd_en),
    .io_rd_data(io_rd_data),
    .io_rd_ready(io_rd_ready),
    .io_wr_ready(io_wr_ready)
);

core U_CORE(
    .clk(clk),
    .rstb(rstb&~during_sw_upgrade),

    .i_data(i_data),
    .i_addr(i_addr),

    .d_addr(d_addr),
    .d_wr_req(d_wr_req),
    .d_wr_ready(d_wr_ready),
    .d_rd_req(d_rd_req),
    .d_rd_ready(d_rd_ready),
    .d_be(d_be),
    .d_wr_data(d_wr_data),
    .d_rd_data(d_rd_data)
);

ram_sdp #(
    .DATA_WIDTH(32),
    .DATA_DEPTH(2**(RAM_ADDR_LEN)),
    .INIT_FN("../c/xriscv.ram")
)U_RAM(
    .clk(clk),
    .ena(d_ram_en|uart_ram_wr_en),
    .wea(ram_we),
    .addra(ram_addr),
    .dina(ram_wr_data),
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
    .wr_en(io_wr_en),
    .be(io_be),
    .wr_addr(io_addr),
    .wdata(io_wr_data),

    `include "reg_conn.vh"

    .rd_en(io_rd_en),
    .rd_addr(io_addr),
    .rdata(io_rd_data),
    .rd_rdy(io_rd_ready)
);

//assign io_wr_ready = d_wr_req & (d_addr[11:2] == 'h200) & d_be[1] & uart_wr_ready;
assign io_wr_ready = 1;

/**************************************************************************************************
*    UART 
**************************************************************************************************/
wire uart_txfifo_full;
wire uart_rxfifo_empty;

assign uart_status[0] = uart_txfifo_full;
assign uart_status[1] = uart_rxfifo_empty;
assign uart_status[7:2] = 'h0;
assign uart_wr_req = d_wr_req & (d_addr[11:2] == 'h200) & d_be[1];
assign uart_rd_req = d_rd_req & (d_addr[11:2] == 'h200) & d_be[2];
assign uart_wr_data = d_wr_data;

uart_mgr#(
    .ADDR_LEN(14),
    .XLEN(32)
)U_UART_MGR(
    .clk(clk),
    .rstb(rstb),
    
    .sw_uart_upgrade_b(sw_uart_upgrade_b),
    .during_sw_upgrade(during_sw_upgrade),
    .uart_ram_wr_en(uart_ram_wr_en),
    .uart_ram_wr_data(uart_ram_wr_data),
    .uart_ram_addr(uart_ram_addr),
    .uart_ram_we(uart_ram_we),
    
    .uart_wr_req(uart_wr_req),
    .uart_wr_data(uart_wr_data),
    .uart_wr_ready(uart_wr_ready),
    .uart_rd_req(uart_rd_req),
    .uart_rd_data(uart_rcvd_byte),
    .uart_rd_ready(uart_rd_ready),
    .uart_txfifo_full(uart_txfifo_full),
    .uart_rxfifo_empty(uart_rxfifo_empty),
    
    .baudrate_cfg(uart_cfg), //clk_en will go to 1 every (50000000/(baudrate_cfg+1)). 216: 9600; 108: 19200; 52: 38400; 36: 57600;  18: 115200; 9: 230400; 
    .rx(rx),
    .tx(tx)
);
    
`ifdef SIM
    integer fp;
    initial begin
        fp = $fopen("uart.txt","w");
    end
    always @(posedge clk) begin
        if(io_wr_en) begin
            if(io_addr == 0 && io_be[1]) begin
                $fwrite(fp,"%c",io_wr_data[15:8]);
            end
        end
    end
`endif

endmodule
