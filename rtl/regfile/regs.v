module regfile(
    input clk,
    input rstb,
    input [7:0] uart_status,
    output reg [7:0] uart_send_byte,
    input [7:0] uart_rcvd_byte,
    output reg [7:0] uart_cfg,
    output reg [3:0] led_b,
    input wr_en,
    input [3:0] be,
    input [15:0] wr_addr,
    input [31:0] wdata,

    input rd_en,
    input [15:0] rd_addr,
    output reg [31:0] rdata,
    output reg rd_rdy
);

//rw registers write
always @(posedge clk or negedge rstb) begin
    if(~rstb) begin
        uart_send_byte <= 0;
        uart_cfg <= 8'd17;
        led_b <= 0;
    end else if(wr_en) begin
        case(wr_addr) 
            'h0: begin
                if(be[0]) begin
                end
                if(be[1]) begin
                    uart_send_byte[7:0] <= wdata[15:8];
                end
                if(be[2]) begin
                end
                if(be[3]) begin
                    uart_cfg[7:0] <= wdata[31:24];
                end
            end
            'h4: begin
                if(be[0]) begin
                    led_b[3:0] <= wdata[3:0];
                end
            end
        endcase
    end
end

//wo registers write
always @(posedge clk or negedge rstb) begin
    if(~rstb) begin
    end else if(wr_en) begin
        case(wr_addr) 
            'h0: begin
                if(be[0]) begin
                end
                if(be[1]) begin
                end
                if(be[2]) begin
                end
                if(be[3]) begin
                end
            end
            'h4: begin
                if(be[0]) begin
                end
            end
        endcase
    end else begin
    end
end

//register read
always @(posedge clk or negedge rstb) begin
    if(~rstb) begin
        rdata <= 0;
    end else if(rd_en)begin
        case(rd_addr)
            'h0: begin
                rdata[7:0] <= uart_status;
                rdata[15:8] <= uart_send_byte;
                rdata[23:16] <= uart_rcvd_byte;
                rdata[31:24] <= uart_cfg;
            end
            'h4: begin
                rdata[3:0] <= led_b;
            end
        endcase
    end else if(~rd_rdy)begin
        rdata <= 0;
    end
end

always @(posedge clk or negedge rstb) begin
    if(~rstb) begin
        rd_rdy <= 0;
    end else if(rd_en) begin
        rd_rdy <= 1;
    end else begin
        rd_rdy <= 0;
    end
end
endmodule
