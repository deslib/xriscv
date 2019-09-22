module rom#(
    parameter DATA_WIDTH = 16,
    parameter DATA_DEPTH = 512,
    parameter ADDR_WIDTH  = $clog2(DATA_DEPTH),
    parameter INIT_FN = ""
)(
    input clk,
    input [ADDR_WIDTH-1:0] addr,
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

    always @(posedge clk) begin
        dout <= ram_table[addr];
    end

endmodule
