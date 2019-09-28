`timescale 1ns/100ps

module tb_soc;
parameter PEIROD = 20;
logic clk;
logic rstb_in;
wire [7:0] led;

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
    .uart_rx(uart_rx),
    .uart_tx(uart_tx),
    .btn(btn),
    .led(led)
);

endmodule
