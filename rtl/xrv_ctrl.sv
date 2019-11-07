module xrv_ctrl(
    input               clk,
    input               rstb,

    input               is_ls,
    input               ls_done,       //load/store done
    input               id_jmp,
    input        [31:0] id_jmp_addr,
    input               ex_jmp,
    input        [31:0] ex_jmp_addr,

    output logic        stalling,
    output logic        flush,
    output logic        jmp,
    output logic [31:0] jmp_addr
);

    logic during_ls;
    logic jmp_dly;
    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            during_ls <= 0;
        end else begin
            if(is_ls&~jmp&~jmp_dly) begin
                during_ls <= 1;
            end else if(ls_done|jmp|jmp_dly) begin  //ls follow an jalr. jmp is later than is_ls
                during_ls <= 0;
            end
        end
    end

    assign stalling = (is_ls | during_ls) & ~ls_done & ~jmp_dly;

    //always @(posedge clk or negedge rstb) begin
    //    if(~rstb) begin
    //        stalling_id <= 0;
    //        stalling_ex <= 0;
    //    end else begin
    //        stalling_id <= stalling_if;
    //        stalling_ex <= stalling_id;
    //    end
    //end

    always @(posedge clk or negedge rstb) begin
        if(~rstb) begin
            jmp_dly <= 0;
        end else begin
            jmp_dly <= jmp;
        end
    end 

    assign jmp = id_jmp | ex_jmp;
    assign jmp_addr = ex_jmp ? ex_jmp_addr : id_jmp_addr;

    assign flush = ex_jmp;

endmodule
