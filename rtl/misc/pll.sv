`timescale 1ns/10ps
module pll #(
    parameter PERIOD = 4.5
)(
    input        clk_in,
    input        resetn,
    output logic clk,
    output logic locked
);

    always #(PERIOD/2) clk = ~clk;

    initial begin
        clk = 0;
    end
    
    logic [1:0] rstb_in_pipe;
    always @(posedge clk or negedge resetn) begin
        if(~resetn) begin
            rstb_in_pipe <= 0;
        end else begin
            rstb_in_pipe[0] <= 1;
            rstb_in_pipe[1] <= rstb_in_pipe[0];
        end
    end
    assign locked = rstb_in_pipe[1];

endmodule
