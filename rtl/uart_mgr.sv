/**********************************************************************************************
* This module use uart to upgrade the content of RAM
**********************************************************************************************/
module uart_mgr#(
    parameter ADDR_LEN = 14,
    parameter XLEN = 32
)(
    input                       clk,
    input                       rstb,

    //software upgrader
    input                       sw_uart_upgrade_b,
    output logic                during_sw_upgrade,
    output logic                uart_ram_wr_en,
    output logic [XLEN-1:0]     uart_ram_wr_data,
    output logic [ADDR_LEN-1:0] uart_ram_addr,
    output logic [XLEN/8-1:0]   uart_ram_we,

    //interface to core
    input                       uart_wr_en,
    input [7:0]                 uart_wr_data,
    output logic                uart_wr_ready,
    input                       uart_rd_req,
    output logic [7:0]          uart_rd_data,
    output                      uart_rd_ready,
    output                      uart_txfifo_full,
    output                      uart_rxfifo_empty,

    //uart module
    input [7:0]     baudrate_cfg, //clk_en will go to 1 every (50000000/(baudrate_cfg+1)). 216: 9600; 108: 19200; 52: 38400; 36: 57600;  18: 115200; 9: 230400; 
    input           rx,
    output          tx
);

logic       uart_txfifo_empty;
logic       uart_rxfifo_full;
logic       uart_txfifo_rd_en;

logic [7:0] uart_tx_data;
logic [7:0] uart_rx_data;
logic       uart_tx_busy;
logic       uart_tx_valid;
`ifdef FAST_UART
    assign uart_wr_ready = 1'b1;
`else
    assign uart_wr_ready = ~uart_txfifo_full;
`endif
assign uart_rd_ready = 1;

always @(posedge clk or negedge rstb) begin
    if(~rstb) begin
        uart_txfifo_rd_en <= 0;
    end else begin
        if(~uart_txfifo_empty & ~uart_tx_busy & ~uart_txfifo_rd_en) begin
            uart_txfifo_rd_en <= 1;
        end else begin
            uart_txfifo_rd_en <= 0;
        end
    end
end
assign uart_tx_valid = uart_txfifo_rd_en;
 
    
uart U_UART(
    .clk(clk),
    .rstb(rstb),
    .baudrate_cfg(baudrate_cfg), //clk_en will go to 1 every (50000000/(baudrate_cfg+1)). 216: 9600; 108: 19200; 52: 38400; 36: 57600;  18: 115200; 9: 230400; 
    .rx(rx),
    .tx(tx),
    .tx_valid(uart_tx_valid),
    .tx_data(uart_tx_data),
    .tx_busy(uart_tx_busy),
    .rx_valid(uart_rx_valid),
    .rx_data(uart_rx_data)
);

uart_sw_upgrader #(
    .ADDR_LEN(14),
    .XLEN(32)
)U_UART_SW_UPGRADER(
    .clk(clk),
    .rstb(rstb),
    .sw_uart_upgrade_b(sw_uart_upgrade_b),

    .uart_rx_valid(uart_rx_valid),
    .uart_rx_data(uart_rx_data),
    
    .during_sw_upgrade(during_sw_upgrade),
    .uart_ram_wr_en(uart_ram_wr_en),
    .uart_ram_wr_data(uart_ram_wr_data),
    .uart_ram_addr(uart_ram_addr),
    .uart_ram_we(uart_ram_we)
);

fifo#(
	.DATA_WIDTH(8),
    .DATA_DEPTH(8),
    .PFULL_NUM(0),
    .PEMPTY_NUM(0)
)U_TXFIFO(
	.clk(clk),
	.rst(~rstb),
	.din(uart_wr_data),
	.wr_en(uart_wr_en),
	.rd_en(uart_txfifo_rd_en),
	.dout(uart_tx_data),
	.full(uart_txfifo_full),
	.empty(uart_txfifo_empty),
	.pfull(),
	.pempty()
);

fifo#(
	.DATA_WIDTH(8),
    .DATA_DEPTH(8),
    .PFULL_NUM(0),
    .PEMPTY_NUM(0)
)U_RXFIFO(
	.clk(clk),
	.rst(~rstb),
	.din(uart_rx_data),
	.wr_en(uart_rx_valid),
	.rd_en(uart_rd_req&uart_rd_ready),
	.dout(uart_rd_data),
	.full(uart_rxfifo_full),
	.empty(uart_rxfifo_empty),
	.pfull(),
	.pempty()
);

endmodule
