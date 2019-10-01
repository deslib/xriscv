`timescale 1ns/100ps

module tb_soc;
parameter PEIROD = 20;
logic clk;
logic rstb_in;
wire [7:0] led;

logic [7:0] uart_wr_data;
wire  [7:0] uart_rx_data;
wire        uart_rx_valid;
logic       uart_wr_en;


always #(PEIROD/2) clk = ~clk;

initial begin
    clk = 0;
    rstb_in = 0;
    repeat(10) @(negedge clk);
    rstb_in = 1;
    repeat(100000) @(negedge clk);
end

soc#(
    .XLEN(32)
)U_SOC(
    .clk(clk),
    .rstb_in(rstb_in),
    .sw_uart_upgrade_b(1'b1),
    .uart_rx(uart_tx),
    .uart_tx(uart_rx),
    .btn(btn),
    .led(led)
);

uart_top U_UART(
    .clk(clk),
    .rstb(rstb_in),
    .baudrate_cfg(8'd18), //clk_en will go to 1 every (50000000/(baudrate_cfg+1)). 216: 9600; 108: 19200; 52: 38400; 36: 57600;  18: 115200; 9: 230400; 
    .rx(uart_rx),
    .tx(uart_tx),
    .wr_en(uart_wr_en),
    .wr_data(uart_wr_data),
    .tx_busy(uart_tx_busy),
    .rx_valid(uart_rx_valid),
    .rx_data(uart_rx_data)
);


endmodule
