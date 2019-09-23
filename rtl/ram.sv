module ram #(
    parameter DATA_WIDTH = 16,
    parameter DATA_DEPTH = 512,
    parameter ADDR_WIDTH  = $clog2(DATA_DEPTH),
    parameter INIT_FN = ""
)(
    input clk,
    input en,
    input [DATA_WIDTH/8-1:0] we,
    input [ADDR_WIDTH-1:0] addr,
    input [DATA_WIDTH-1:0] din,
    output logic signed [DATA_WIDTH-1:0] dout 
);

    bit signed [DATA_WIDTH-1:0] ram_table[DATA_DEPTH];
    int i;

    initial begin
        if(INIT_FN == "") begin
            for(i=0;i<DATA_DEPTH;i++) begin
                ram_table[i] = 0;
            end
        end else begin
            $readmemh(INIT_FN,ram_table);
        end
    end

    genvar byte_i;
    generate
    for(byte_i=0;byte_i<(DATA_WIDTH/8-1);byte_i++) begin
        always @(posedge clk) begin
            if(en) begin
                if(we[byte_i]) begin
                    ram_table[addr][8*byte_i+7:8*byte_i] <= din[8*byte_i+7:8*byte_i];
                end
            end
        end
    end
    endgenerate
    
    always @(posedge clk) begin
        if(en) begin
            dout <= ram_table[addr];
        end
    end

endmodule

module ram_sdp #(
    parameter DATA_WIDTH = 16,
    parameter DATA_DEPTH = 512,
    parameter ADDR_WIDTH  = $clog2(DATA_DEPTH),
    parameter INIT_FN = ""
)(
    input                                   clk,
    input                                   ena,
    input [DATA_WIDTH/8-1:0]                wea,
    input [ADDR_WIDTH-1:0]                  addra,
    input [DATA_WIDTH-1:0]                  dina,
    output logic signed [DATA_WIDTH-1:0]    douta,

    input [ADDR_WIDTH-1:0]                  addrb,
    output logic signed [DATA_WIDTH-1:0]    doutb 
);

    bit signed [DATA_WIDTH-1:0] ram_table[DATA_DEPTH];
    int i;

    initial begin
        if(INIT_FN == "") begin
            for(i=0;i<DATA_DEPTH;i++) begin
                ram_table[i] = 0;
            end
        end else begin
            $readmemh(INIT_FN,ram_table);
        end
    end

    genvar byte_i;
    generate
    for(byte_i=0;byte_i<(DATA_WIDTH/8-1);byte_i++) begin
        always @(posedge clk) begin
            if(ena) begin
                if(wea[byte_i]) begin
                    ram_table[addra][8*byte_i+7:8*byte_i] <= dina[8*byte_i+7:8*byte_i];
                end
            end
        end
    end
    endgenerate
    
    always @(posedge clk) begin
        doutb <= ram_table[addrb];
        douta <= ram_table[addra];
    end

endmodule
