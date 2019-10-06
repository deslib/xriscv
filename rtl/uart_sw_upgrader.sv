/**********************************************************************************************
* This module use uart to upgrade the content of RAM
**********************************************************************************************/
module uart_sw_upgrader #(
    parameter ADDR_LEN = 14,
    parameter XLEN = 32
)(
    input                       clk,
    input                       rstb,
    input                       sw_uart_upgrade_b,

    input                       uart_rx_valid,
    input [7:0]                 uart_rx_data,
    
    output logic                during_sw_upgrade,
    output logic                uart_ram_wr_en,
    output logic [XLEN-1:0]     uart_ram_wr_data,
    output logic [ADDR_LEN-1:0] uart_ram_addr,
    output logic [XLEN/8-1:0]   uart_ram_we
);

    logic [1:0] be_cnt;

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            during_sw_upgrade <= 0;
        end else begin
            if(~sw_uart_upgrade_b) begin
                during_sw_upgrade <= 1;
            end
        end
    end
     

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            be_cnt <= 0;
        end else begin
            if(uart_rx_valid) begin
                if(be_cnt == XLEN/8) begin
                    be_cnt <= 0;
                end else begin
                    be_cnt <= be_cnt + 1;
                end
            end
        end
    end
     
    always @(posedge clk) begin
        uart_ram_we <= 1 << be_cnt;
    end
     
    always @(posedge clk) begin
        uart_ram_wr_data <= uart_rx_data << (be_cnt * 8);
    end

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            uart_ram_addr <= 0;
        end else begin
            if(uart_ram_wr_en & uart_ram_we[XLEN/8-1]) begin
                uart_ram_addr <= uart_ram_addr + 1;
            end
        end
    end

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            uart_ram_wr_en <= 0;
        end else begin
            uart_ram_wr_en <= uart_rx_valid;
        end
    end

endmodule
