`include "glb.svh"
module xrv_i_decompress(
    input        [31:0] data_in,
    output logic [31:0] data_out
);
    logic illegal_instr_o;

    always @(*) begin
        case(data_in[1:0])
            2'b00: begin
              unique case (data_in[15:13])
                3'b000: begin
                  // c.addi4spn -> addi rd', x2, imm
                  data_out = {2'b0, data_in[10:7], data_in[12:11], data_in[5],
                             data_in[6], 2'b00, 5'h02, 3'b000, 2'b01, data_in[4:2], `OP_IMM, 2'b11};
                  if (data_in[12:5] == 8'b0)  illegal_instr_o = 1'b1;
                end

                3'b010: begin
                  // c.lw -> lw rd', imm(rs1')
                  data_out = {5'b0, data_in[5], data_in[12:10], data_in[6],
                             2'b00, 2'b01, data_in[9:7], 3'b010, 2'b01, data_in[4:2], `OP_LOAD, 2'b11};
                end

                3'b110: begin
                  // c.sw -> sw rs2', imm(rs1')
                  data_out = {5'b0, data_in[5], data_in[12], 2'b01, data_in[4:2],
                             2'b01, data_in[9:7], 3'b010, data_in[11:10], data_in[6],
                             2'b00, `OP_STORE, 2'b11};
                end

                default: begin
                  illegal_instr_o = 1'b1;
                end
              endcase
            end

            // C1
            //
            // Register address checks for RV32E are performed in the regular instruction decoder.
            // If this check fails, an illegal instruction exception is triggered and the controller
            // writes the actual faulting instruction to mtval.
            2'b01: begin
              unique case (data_in[15:13])
                3'b000: begin
                  // c.addi -> addi rd, rd, nzimm
                  // c.nop
                  data_out = {{6 {data_in[12]}}, data_in[12], data_in[6:2],
                             data_in[11:7], 3'b0, data_in[11:7], `OP_IMM, 2'b11};
                end

                3'b001, 3'b101: begin
                  // 001: c.jal -> jal x1, imm
                  // 101: c.j   -> jal x0, imm
                  data_out = {data_in[12], data_in[8], data_in[10:9], data_in[6],
                             data_in[7], data_in[2], data_in[11], data_in[5:3],
                             {9 {data_in[12]}}, 4'b0, ~data_in[15], `OP_JAL, 2'b11};
                end

                3'b010: begin
                  // c.li -> addi rd, x0, nzimm
                  // (c.li hints are translated into an addi hint)
                  data_out = {{6 {data_in[12]}}, data_in[12], data_in[6:2], 5'b0,
                             3'b0, data_in[11:7], `OP_IMM, 2'b11};
                end

                3'b011: begin
                  // c.lui -> lui rd, imm
                  // (c.lui hints are translated into a lui hint)
                  data_out = {{15 {data_in[12]}}, data_in[6:2], data_in[11:7], `OP_LUI, 2'b11};

                  if (data_in[11:7] == 5'h02) begin
                    // c.addi16sp -> addi x2, x2, nzimm
                    data_out = {{3 {data_in[12]}}, data_in[4:3], data_in[5], data_in[2],
                               data_in[6], 4'b0, 5'h02, 3'b000, 5'h02, `OP_IMM, 2'b11};
                  end

                  if ({data_in[12], data_in[6:2]} == 6'b0) illegal_instr_o = 1'b1;
                end

                3'b100: begin
                  unique case (data_in[11:10])
                    2'b00,
                    2'b01: begin
                      // 00: c.srli -> srli rd, rd, shamt
                      // 01: c.srai -> srai rd, rd, shamt
                      // (c.srli/c.srai hints are translated into a srli/srai hint)
                      data_out = {1'b0, data_in[10], 5'b0, data_in[6:2], 2'b01, data_in[9:7],
                                 3'b101, 2'b01, data_in[9:7], `OP_IMM, 2'b11};
                      if (data_in[12] == 1'b1)  illegal_instr_o = 1'b1;
                    end

                    2'b10: begin
                      // c.andi -> andi rd, rd, imm
                      data_out = {{6 {data_in[12]}}, data_in[12], data_in[6:2], 2'b01, data_in[9:7],
                                 3'b111, 2'b01, data_in[9:7], `OP_IMM, 2'b11};
                    end

                    2'b11: begin
                      unique case ({data_in[12], data_in[6:5]})
                        3'b000: begin
                          // c.sub -> sub rd', rd', rs2'
                          data_out = {2'b01, 5'b0, 2'b01, data_in[4:2], 2'b01, data_in[9:7],
                                     3'b000, 2'b01, data_in[9:7], `OP_REG, 2'b11};
                        end

                        3'b001: begin
                          // c.xor -> xor rd', rd', rs2'
                          data_out = {7'b0, 2'b01, data_in[4:2], 2'b01, data_in[9:7], 3'b100,
                                     2'b01, data_in[9:7], `OP_REG, 2'b11};
                        end

                        3'b010: begin
                          // c.or  -> or  rd', rd', rs2'
                          data_out = {7'b0, 2'b01, data_in[4:2], 2'b01, data_in[9:7], 3'b110,
                                     2'b01, data_in[9:7], `OP_REG, 2'b11};
                        end

                        3'b011: begin
                          // c.and -> and rd', rd', rs2'
                          data_out = {7'b0, 2'b01, data_in[4:2], 2'b01, data_in[9:7], 3'b111,
                                     2'b01, data_in[9:7], `OP_REG, 2'b11};
                        end

                        3'b100,
                        3'b101,
                        3'b110,
                        3'b111: begin
                          // 100: c.subw
                          // 101: c.addw
                          illegal_instr_o = 1'b1;
                        end

                        default: begin
                          illegal_instr_o = 1'bX;
                        end
                      endcase
                    end

                    default: begin
                      illegal_instr_o = 1'bX;
                    end
                  endcase
                end

                3'b110, 3'b111: begin
                  // 0: c.beqz -> beq rs1', x0, imm
                  // 1: c.bnez -> bne rs1', x0, imm
                  data_out = {{4 {data_in[12]}}, data_in[6:5], data_in[2], 5'b0, 2'b01,
                             data_in[9:7], 2'b00, data_in[13], data_in[11:10], data_in[4:3],
                             data_in[12], `OP_BRANCH, 2'b11};
                end

                default: begin
                  illegal_instr_o = 1'bX;
                end
              endcase
            end

            // C2
            //
            // Register address checks for RV32E are performed in the regular instruction decoder.
            // If this check fails, an illegal instruction exception is triggered and the controller
            // writes the actual faulting instruction to mtval.
            2'b10: begin
              unique case (data_in[15:13])
                3'b000: begin
                  // c.slli -> slli rd, rd, shamt
                  // (c.ssli hints are translated into a slli hint)
                  data_out = {7'b0, data_in[6:2], data_in[11:7], 3'b001, data_in[11:7], `OP_IMM, 2'b11};
                  if (data_in[12] == 1'b1)  illegal_instr_o = 1'b1; // reserved for custom extensions
                end

                3'b010: begin
                  // c.lwsp -> lw rd, imm(x2)
                  data_out = {4'b0, data_in[3:2], data_in[12], data_in[6:4], 2'b00, 5'h02,
                             3'b010, data_in[11:7], `OP_LOAD, 2'b11};
                  if (data_in[11:7] == 5'b0)  illegal_instr_o = 1'b1;
                end

                3'b100: begin
                  if (data_in[12] == 1'b0) begin
                    if (data_in[6:2] != 5'b0) begin
                      // c.mv -> add rd/rs1, x0, rs2
                      // (c.mv hints are translated into an add hint)
                      data_out = {7'b0, data_in[6:2], 5'b0, 3'b0, data_in[11:7], `OP_REG, 2'b11};
                    end else begin
                      // c.jr -> jalr x0, rd/rs1, 0
                      data_out = {12'b0, data_in[11:7], 3'b0, 5'b0, `OP_JALR, 2'b11};
                      if (data_in[11:7] == 5'b0)  illegal_instr_o = 1'b1;
                    end
                  end else begin
                    if (data_in[6:2] != 5'b0) begin
                      // c.add -> add rd, rd, rs2
                      // (c.add hints are translated into an add hint)
                      data_out = {7'b0, data_in[6:2], data_in[11:7], 3'b0, data_in[11:7], `OP_REG, 2'b11};
                    end else begin
                      if (data_in[11:7] == 5'b0) begin
                        // c.ebreak -> ebreak
                        data_out = {32'h00_10_00_73};
                      end else begin
                        // c.jalr -> jalr x1, rs1, 0
                        data_out = {12'b0, data_in[11:7], 3'b000, 5'b00001, `OP_JALR, 2'b11};
                      end
                    end
                  end
                end

                3'b110: begin
                  // c.swsp -> sw rs2, imm(x2)
                  data_out = {4'b0, data_in[8:7], data_in[12], data_in[6:2], 5'h02, 3'b010,
                             data_in[11:9], 2'b00, `OP_STORE, 2'b11};
                end

                3'b001,
                3'b011,
                3'b101,
                3'b111: begin
                  illegal_instr_o = 1'b1;
                end

                default: begin
                  illegal_instr_o = 1'bX;
                end
              endcase
            end
            default: begin
                // 32 bit (or more) instruction
                data_out = data_in;
            end
        endcase
    end

endmodule
