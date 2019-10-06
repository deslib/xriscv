`timescale 1ns/100ps

//`define SW_UPGRADE

module tb_soc;
localparam PEIROD = 20;
localparam RAM_SIZE = 16'h4000;
logic clk;
logic rstb;
wire [3:0] led;

logic [7:0] uart_wr_data;
wire  [7:0] uart_rx_data;
wire        uart_rx_valid;
logic       uart_wr_en;
logic       sw_uart_upgrade_b;

integer fp;
logic [7:0] ram_data[RAM_SIZE];
logic [7:0] bin_data;

always #(PEIROD/2) clk = ~clk;

initial begin
    clk = 0;
    `ifdef SW_UPGRADE
        rstb = 0;
        sw_uart_upgrade_b = 0;

        fp = $fopen("../c/xriscv_ram.bin","rb");
        $fread(ram_data,fp);
        $fclose(fp);

        fp = $fopen("../c/xriscv_ram.bin","rb");
        repeat(16) begin
            $fread(bin_data,fp);
            $display("%02x",bin_data);
        end

        repeat(10) @(negedge clk);
        rstb = 1;
        repeat(1000000) @(negedge clk);
    `endif

    rstb = 0;
    sw_uart_upgrade_b = 1;
    repeat(10) @(negedge clk);
    rstb = 1;
    repeat(100000) @(negedge clk);
end

soc#(
    .XLEN(32)
)U_SOC(
    .clk(clk),
    .rstb_in(rstb),
    .sw_uart_upgrade_b(sw_uart_upgrade_b),
    .uart_rx(uart_tx),
    .uart_tx(uart_rx),
    .led(led)
);

uart U_UART(
    .clk(clk),
    .rstb(rstb),
    .baudrate_cfg(8'd6),
    .rx(uart_rx),
    .tx(uart_tx),
    .tx_valid(uart_wr_en),
    .tx_data(uart_wr_data),
    .tx_busy(uart_tx_busy),
    .rx_valid(uart_rx_valid),
    .rx_data(uart_rx_data)
);

logic [15:0] ram_addr;
always @(posedge clk or negedge rstb) begin
    if(~rstb) begin
        uart_wr_en <= 0;
    end else begin
        if(~sw_uart_upgrade_b && ram_addr != RAM_SIZE) begin
            if(~uart_tx_busy & ~uart_wr_en) begin
                uart_wr_en <= 1;
            end else begin
                uart_wr_en <= 0;
            end
        end
    end
end

always @(posedge clk or negedge rstb) begin
    if(~rstb) begin
        ram_addr <= 0;
    end else begin
        if(uart_wr_en) begin
            ram_addr <= ram_addr + 1;
        end
    end
end

assign uart_wr_data = ram_data[ram_addr];

endmodule
