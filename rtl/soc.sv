`include "glb.svh"
module soc#(
    parameter XLEN = 32,
    parameter ADDR_LEN = 16,
    parameter RAM_ADDR_LEN = ADDR_LEN-2
)(
    input clk,
    input rstb_in,
    input sw_uart_upgrade_b, //software upgrade
    input uart_rx,
    output uart_tx,
    output [3:0] led
);

`include "reg_decl.vh"

logic [31:0]                i_addr;

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
logic                       io_wr_req;
logic [XLEN/8-1:0]          io_be;
logic                       io_rd_req;
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
wire                        uart_txfifo_full;
wire                        uart_rxfifo_empty;

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
    .io_wr_req(io_wr_req),
    .io_be(io_be),
    .io_wr_data(io_wr_data),
    .io_rd_req(io_rd_req),
    .io_rd_data(io_rd_data),
    .io_rd_ready(io_rd_ready),
    .io_wr_ready(io_wr_ready)
);

xrv_core U_XRV_CORE(
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

`ifdef SIM
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
    .addrb(i_addr[ADDR_LEN-1:2]),
    .doutb(i_data)
);
`else

core_ram U_RAM(
    .clka(clk),
    .ena(d_ram_en|uart_ram_wr_en),
    .wea(ram_we),
    .addra(ram_addr),
    .dina(ram_wr_data),
    .douta(d_ram_rd_data),
    .clkb(clk),
    .enb(1'b0),
    .web('h0),
    .dinb(),
    .addrb(i_addr[ADDR_LEN-1:2]),
    .doutb(i_data)
);
    
`endif

regfile U_REGFILE(
    .clk(clk),
    .rstb(rstb),
    .wr_en(io_wr_req),
    .be(io_be),
    .wr_addr(io_addr),
    .wdata(io_wr_data),

    `include "reg_conn.vh"

    .rd_en(io_rd_req),
    .rd_addr(io_addr),
    .rdata(io_rd_data),
    .rd_rdy(io_rd_ready)
);

always @(posedge clk or negedge rstb) begin
    if(~rstb) begin
        io_wr_ready <= 0;
    end else begin
        if(io_wr_req&io_wr_ready) begin
            io_wr_ready <= 0;
        end else if(io_wr_req)begin
            if(io_addr == 0 && io_be[1]) begin
                io_wr_ready <= uart_wr_ready;
            end else begin
                io_wr_ready <= 1;
            end
        end
    end
end
 
//assign io_wr_ready = 1;

/**************************************************************************************************
*    UART 
**************************************************************************************************/

`ifdef FAST_UART
    assign uart_status[0] = 1'b0;
`else
    assign uart_status[0] = uart_txfifo_full;
`endif

`ifdef UART
assign uart_status[1] = uart_rxfifo_empty;
assign uart_status[7:2] = 'h0;
assign uart_wr_req = io_wr_req & (io_addr == 'h0) & d_be[1];
assign uart_rd_req = io_rd_req & (io_addr == 'h0) & d_be[2];
assign uart_wr_data = d_wr_data[15:8];

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
    
    .uart_wr_en(uart_wr_req & io_wr_ready),
    .uart_wr_data(uart_wr_data),
    .uart_wr_ready(uart_wr_ready),
    .uart_rd_req(uart_rd_req),
    .uart_rd_data(uart_rcvd_byte),
    .uart_rd_ready(uart_rd_ready),
    .uart_txfifo_full(uart_txfifo_full),
    .uart_rxfifo_empty(uart_rxfifo_empty),
    
    .baudrate_cfg(uart_cfg), //clk_en will go to 1 every (50000000/(baudrate_cfg+1)). 216: 9600; 108: 19200; 52: 38400; 36: 57600;  18: 115200; 9: 230400; 
    .rx(uart_rx),
    .tx(uart_tx)
);
`else
assign uart_wr_ready = 1;
assign uart_ram_wr_en = 0;
assign during_sw_upgrade = 0;
`endif
    
`ifdef SIM
    integer fp;
    initial begin
        fp = $fopen("uart.txt","w");
    end
    always @(posedge clk) begin
        if(io_wr_req&io_wr_ready) begin
            if(io_addr == 0 && io_be[1]) begin
                $fwrite(fp,"%c",io_wr_data[15:8]);
            end
        end
    end
`endif

assign led = ~led_b;

endmodule

