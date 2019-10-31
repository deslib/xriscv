#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "logger.h"
#include "rv32c.h"

u32 rd;
u32 rs1;
u32 rs2;
u32 uimm;
i32 imm;
u32 off;
u32 subcode1;
u32 subcode2;
u32 subcode3;

struct opcode_group c_opcodes_00[8];
struct opcode_group c_opcodes_01[8];
struct opcode_group c_opcodes_10[8];
struct opcode_group c_opcodes_01_sub1[4];
struct opcode_group c_opcodes_01_sub2[4];
struct opcode_group c_opcodes_10_sub1[2];

//x[8+rd’] = x[2] + uimm
void c_addi4spn(u32 code){
    rd = get_bits(code, 2, 3);
    uimm = ((get_bits(code, 5, 1) << 3) | \
           (get_bits(code, 6, 1) << 2) | \
           (get_bits(code, 7, 4) << 6) | \
           (get_bits(code, 11, 2) << 4));
    if (uimm == 0)
        invalid_opcode(code);
    xreg[rd+8] = (xreg[2] + uimm);
    pc += 2;
}

//x[8+rd’] = sext(M[x[8+rs1’] + uimm][31:0])
void c_lw(u32 code){
    rd = get_bits(code, 2, 3);
    rs1 = get_bits(code, 7, 3);
    uimm = ((get_bits(code, 4, 1) << 6) | \
           (get_bits(code, 5, 1) << 2) | \
           (get_bits(code, 10, 3) << 3));
    xreg[rd+8] = (i32)get_mem(xreg[8+rs1]+uimm);
    pc += 2;
}

//M[x[8+rs1’] + uimm][31:0] = x[8+rs2’]
void c_sw(u32 code){
    rs1 = get_bits(code, 7, 3);
    rs2 = get_bits(code, 2, 3);
    uimm = ((get_bits(code, 4, 1) << 6) | \
           (get_bits(code, 5, 1) << 2) | \
           (get_bits(code, 10, 3) << 3));
    set_mem((xreg[8+rs1]+uimm), xreg[8+rs2], DWORD);
    pc += 2;
}

void c_nop(u32 code){
    pc += 2;
}

//x[rd] = x[rd] + sext(imm)
void c_addi(u32 code){
    uimm = (get_bits(code, 2, 5) | \
           (get_bits(code, 12, 1) << 5));
    rd = get_bits(code, 7, 5);
    xreg[rd] = (xreg[rd] + u2i(uimm, 6));
    pc += 2;
}

void c_nop_addi(u32 code){
    if (code == 01) {
        //c.nop
        c_nop(code);
    }
    else if(get_bits(code, 7, 5) == 0) {
        //invalide
        invalid_opcode(code);
    }
    else {
        c_addi(code);
    }
}

//x[1] = pc+2; pc += sext(offset)
void c_jal(u32 code){
    off = ((get_bits(code, 2, 1) << 5) | \
           (get_bits(code, 3, 3) << 1) | \
           (get_bits(code, 6, 1) << 7) | \
           (get_bits(code, 7, 1) << 6) | \
           (get_bits(code, 8, 1) << 10) | \
           (get_bits(code, 9, 2) << 8) | \
           (get_bits(code, 11, 1) << 4) | \
           (get_bits(code, 12, 1) << 11));
    xreg[1] = (pc + 2);
    pc += u2i(off, 12);
}

//x[rd] = sext(imm)
void c_li(u32 code){
    uimm = (get_bits(code, 2, 5) | \
           (get_bits(code, 12, 1) << 5));
    rd = get_bits(code, 7, 5);
    xreg[rd] = u2i(uimm, 6);
    pc += 2;
}

//x[2] = x[2] + sext(imm)
void c_addi16sp(u32 code){
    uimm = ((get_bits(code, 2, 1) << 5) | \
           (get_bits(code, 3, 2) << 7) | \
           (get_bits(code, 5, 1) << 6) | \
           (get_bits(code, 6, 1) << 4) | \
           (get_bits(code, 12, 1) << 9));
    xreg[2] = (xreg[2] + u2i(uimm, 10));
    pc += 2;
}

//x[rd] = sext(imm[17:12] << 12)
void c_lui(u32 code){
    uimm = ((get_bits(code, 2, 5) << 12) | \
           (get_bits(code, 12, 1) << 17));
    rd = get_bits(code, 7, 5);
    xreg[rd] = u2i(uimm, 18);
    pc += 2;
}

void c_addi16sp_lui(u32 code){
    if (get_bits(code, 7, 5) == 2) {
        //c.addi16sp
        c_addi16sp(code);
    }
    else if(get_bits(code, 7, 5) == 0) {
        //invalid
        invalid_opcode(code);
    }
    else {
        //c.lui
        c_lui(code);
    }
}

//x[8+rd’] = x[8+rd’] >>(u)uimm
void c_srli(u32 code){
    uimm = (get_bits(code, 2, 5) | \
           (get_bits(code, 12, 1) << 5));
    rd = get_bits(code, 7, 3);
    xreg[8+rd] = (xreg[8+rd] >> uimm);
    pc += 2;
}

//x[8+rd’] = x[8+rd’] >>(s)uimm
void c_srai(u32 code){
    uimm = (get_bits(code, 2, 5) | \
           (get_bits(code, 12, 1) << 5));
    rd = get_bits(code, 7, 3);
    xreg[8+rd] = (xreg[8+rd] >> uimm);
    pc += 2;
}

//x[8+rd’] = x[8+rd’] & sext(imm)
void c_andi(u32 code){
    uimm = (get_bits(code, 2, 5) | \
           (get_bits(code, 12, 1) << 5));
    rd = get_bits(code, 7, 3);
    xreg[rd] = (xreg[rd] & u2i(uimm, 6));
    pc += 2;
}

//x[8+rd’] = x[8+rd’] - x[8+rs2’]
void c_sub(u32 code){
    rd = get_bits(code, 7, 3);
    rs2 = get_bits(code, 2, 3);
    xreg[8+rd] = (xreg[8+rd] - xreg[8+rs2]);
    pc += 2;
}

//x[8+rd’] = x[8+rd’] ^ x[8+rs2’]
void c_xor(u32 code){
    rd = get_bits(code, 7, 3);
    rs2 = get_bits(code, 2, 3);
    xreg[8+rd] = (xreg[8+rd] ^ xreg[8+rs2]);
    pc += 2;
}

//x[8+rd’] = x[8+rd’] | x[8+rs2’]
void c_or(u32 code){
    rd = get_bits(code, 7, 3);
    rs2 = get_bits(code, 2, 3);
    xreg[8+rd] = (xreg[8+rd] | xreg[8+rs2]);
    pc += 2;
}

//x[8+rd’] = x[8+rd’] & x[8+rs2’]
void c_and(u32 code){
    rd = get_bits(code, 7, 3);
    rs2 = get_bits(code, 2, 3);
    xreg[8+rd] = (xreg[8+rd] & xreg[8+rs2]);
    pc += 2;
}

//pc += sext(offset)
void c_j(u32 code){
    off = ((get_bits(code, 2, 1) << 5) | \
           (get_bits(code, 3, 3) << 1) | \
           (get_bits(code, 6, 1) << 7) | \
           (get_bits(code, 7, 1) << 6) | \
           (get_bits(code, 8, 1) << 10) | \
           (get_bits(code, 9, 2) << 8) | \
           (get_bits(code, 11, 1) << 4) | \
           (get_bits(code, 12, 1) << 11));
    pc += u2i(off, 12);
}

//if (x[8+rs1’] == 0) pc += sext(offset)
void c_beqz(u32 code){
    off = ((get_bits(code, 2, 1) << 5) | \
           (get_bits(code, 3, 2) << 1) | \
           (get_bits(code, 5, 2) << 6) | \
           (get_bits(code, 10, 2) << 3) | \
           (get_bits(code, 12, 1) << 8));
    rs1 = get_bits(code, 7, 3);
    if (xreg[8+rs1] == 0)
      pc += u2i(off, 9);
    else
      pc += 2;
}

//if (x[8+rs1’] != 0) pc += sext(offset)
void c_bnez(u32 code){
    off = ((get_bits(code, 2, 1) << 5) | \
           (get_bits(code, 3, 2) << 1) | \
           (get_bits(code, 5, 2) << 6) | \
           (get_bits(code, 10, 2) << 3) | \
           (get_bits(code, 12, 1) << 8));
    rs1 = get_bits(code, 7, 3);
    if (xreg[8+rs1] != 0)
      pc += u2i(off, 9);
    else
      pc += 2;
}

//x[rd] = x[rd] << uimm
void c_slli(u32 code){
    uimm = (get_bits(code, 2, 5) | \
           (get_bits(code, 12, 1) << 5));
    rd = get_bits(code, 7, 5);
    xreg[rd] = (xreg[rd] << uimm);
    pc += 2;
}

//x[rd] = sext(M[x[2] + uimm][31:0])
void c_lwsp(u32 code){
    uimm = (get_bits(code, 2, 5) | \
           (get_bits(code, 12, 1) << 5));
    rd = get_bits(code, 7, 5);
    xreg[rd] = (i32)get_mem(xreg[2]+uimm);
    pc += 2;
}

//pc = x[rs1]
void c_jr(u32 code){
    rs1 = get_bits(code, 7, 5);
    pc = xreg[rs1];
}

//x[rd] = x[rs2]
void c_mv(u32 code){
    rd = get_bits(code, 7, 5);
    rs2 = get_bits(code, 2, 5);
    xreg[rd] = xreg[rs2];
    pc += 2;
}

//RaiseException(Breakpoint)
void c_ebreak(u32 code){
  //TODO
    pc += 2;  
}

//t = pc+2; pc = x[rs1]; x[1] = t
void c_jalr(u32 code){
  u32 t;
    rs1 = get_bits(code, 7, 5);
  t = (pc + 2);
  pc = xreg[rs1];
  xreg[1] = t;
  pc += 2;
}

//x[rd] = x[rd] + x[rs2]
void c_add(u32 code){
    rd = get_bits(code, 7, 5);
    rs2 = get_bits(code, 2, 5);
  xreg[rd] = (xreg[rd] + xreg[rs2]);
  pc += 2;
}

void c_jr_mv(u32 code){
  u32 i, j;
  i = get_bits(code, 2, 5);
  j = get_bits(code, 7, 5);
  if ((i == 0) && (j != 0)) {
    //c.jr
    c_jr(code);
  }
  else if ((i != 0) && (j != 0)) {
    //c.mv
    c_mv(code);
  }
  else {
    //invalid
    invalid_opcode(code);
  }
}

void c_ebreak_jalr_add(u32 code){
  u32 i, j;
  i = get_bits(code, 2, 5);
  j = get_bits(code, 7, 5);
  if ((i == 0) && (j == 0)) {
    //c.ebreak
    c_ebreak(code);
  }
  else if ((i == 0) && (j != 0)) {
    //c.jalr
    c_jalr(code);
  }
  else if ((i != 0) && (j != 0)) {
    //c.add
    c_add(code);
  }
  else {
    //invalid
    invalid_opcode(code);    
  }
}

//M[x[2] + uimm][31:0] = x[rs2]
void c_swsp(u32 code){
    uimm = ((get_bits(code, 7, 2) << 6) | \
           (get_bits(code, 9, 4) << 2));
    rs2 = get_bits(code, 2, 5);
    set_mem((xreg[2]+uimm), xreg[rs2], DWORD);
    pc += 2;
}

void c_sub_xor_or_and(u32 code){
  subcode3 = get_bits(code, 5, 2);
  c_opcodes_01_sub2[subcode3].exec(code);
  c_opcodes_01_sub2[subcode3].count++;
}

void c_srli_srai_andi_sub_xor_or_and(u32 code){
  subcode2 = get_bits(code, 10, 2);
  c_opcodes_01_sub1[subcode2].exec(code);
  c_opcodes_01_sub1[subcode2].count++;
}

void c_jr_mv_ebreak_jalr_add(u32 code){
  c_opcodes_10_sub1[get_bits(code, 12, 1)].exec(code);
}

void c_exec_00(u32 code){
    //log_info("c_exec_00, pc[0x%x] = 0x%x\n", pc, code&0xffff);
    subcode1 = get_bits(code, 13, 3);
    c_opcodes_00[get_bits(code, 13, 3)].exec(code&0xffff);
    c_opcodes_00[subcode1].count++;
    xreg[0] = 0;// some instruction will set reg[0]. But it is hard wired as 0 in hardware;
    tick += 1;
}

void c_exec_01(u32 code){
    //log_info("c_exec_01, pc[0x%x] = 0x%x\n", pc, code&0xffff);
    subcode1 = get_bits(code, 13, 3);
    c_opcodes_01[get_bits(code, 13, 3)].exec(code&0xffff);
    c_opcodes_01[subcode1].count++;
    xreg[0] = 0;// some instruction will set reg[0]. But it is hard wired as 0 in hardware;
    tick += 1;
}

void c_exec_10(u32 code){
    //log_info("c_exec_10, pc[0x%x] = 0x%x\n", pc, code&0xffff);
    subcode1 = get_bits(code, 13, 3);
    c_opcodes_10[subcode1].exec(code&0xffff);
    c_opcodes_10[subcode1].count++;
    xreg[0] = 0;// some instruction will set reg[0]. But it is hard wired as 0 in hardware;
    tick += 1;
}

void c_result(){
    log_info_direct("RV32C result: \n");
    log_tested_print("OP_C_00", c_opcodes_00, 8);
    log_tested_print("OP_C_01", c_opcodes_01, 8);
    log_tested_print("OP_C_10", c_opcodes_10, 8);
    log_tested_print("OP_C_01 sub1 ", c_opcodes_01_sub1, 4);
    log_tested_print("OP_C_01 sub2 ", c_opcodes_01_sub2, 4);
    log_tested_print("OP_C_01 sub1 ", c_opcodes_10_sub1, 2);
}

void  __attribute__((constructor)) c_opcodes_group_init(){
    u32 i;

    opcodes_group[OP_C_00].exec = c_exec_00;
    opcodes_group[OP_C_00].result = c_result;
    opcodes_group[OP_C_01].exec = c_exec_01;
    opcodes_group[OP_C_01].result = NULL;
    opcodes_group[OP_C_10].exec = c_exec_10;
    opcodes_group[OP_C_10].result = NULL;

    for (i = 0; i < 8; i++) {
        c_opcodes_00[i].exec = invalid_opcode;
        c_opcodes_00[i].count = 0;
    }
    c_opcodes_00[0].exec = c_addi4spn;
    c_opcodes_00[2].exec = c_lw;
    c_opcodes_00[6].exec = c_sw;

    for (i = 0; i < 8; i++) {
        c_opcodes_01[i].exec = invalid_opcode;
        c_opcodes_01[i].count = 0;
    }
    c_opcodes_01[0].exec = c_nop_addi;
    c_opcodes_01[1].exec = c_jal;
    c_opcodes_01[2].exec = c_li;
    c_opcodes_01[3].exec = c_addi16sp_lui;
    c_opcodes_01[4].exec = c_srli_srai_andi_sub_xor_or_and;
    c_opcodes_01[5].exec = c_j;
    c_opcodes_01[6].exec = c_beqz;
    c_opcodes_01[7].exec = c_bnez;

    for (i = 0; i < 8; i++) {
        c_opcodes_10[i].exec = invalid_opcode;
        c_opcodes_10[i].count = 0;
    }
    c_opcodes_10[0].exec = c_slli;
    c_opcodes_10[2].exec = c_lwsp;
    c_opcodes_10[4].exec = c_jr_mv_ebreak_jalr_add;
    c_opcodes_10[6].exec = c_swsp;

    for (i = 0; i < 4; i++) {
        c_opcodes_01_sub1[i].exec = invalid_opcode;
        c_opcodes_01_sub1[i].count = 0;
    }
    c_opcodes_01_sub1[0].exec = c_srli;
    c_opcodes_01_sub1[1].exec = c_srai;
    c_opcodes_01_sub1[2].exec = c_andi;
    c_opcodes_01_sub1[3].exec = c_sub_xor_or_and;

    for (i = 0; i < 4; i++) {
        c_opcodes_01_sub2[i].exec = invalid_opcode;
        c_opcodes_01_sub2[i].count = 0;
    }
    c_opcodes_01_sub2[0].exec = c_sub;
    c_opcodes_01_sub2[0].exec = c_xor;
    c_opcodes_01_sub2[0].exec = c_or;
    c_opcodes_01_sub2[0].exec = c_and;

    for (i = 0; i < 2; i++) {
        c_opcodes_10_sub1[i].exec = invalid_opcode;
        c_opcodes_10_sub1[i].count = 0;
    }
    c_opcodes_10_sub1[0].exec = c_jr_mv;
    c_opcodes_10_sub1[1].exec = c_ebreak_jalr_add;
}

