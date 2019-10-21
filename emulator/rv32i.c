#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "logger.h"
#include "csr.h"
#include "rv32i.h"
u32 rd;
u32 rs1;
u32 rs2;
i32 imm_signed;
i32 imm_unsigned;
i32 op1_signed;
u32 op1_unsigned;
i32 op2_signed;
u32 op2_unsigned;
u32 zimm;
u8  shamt;
u32 csr_off;
u32 opcode;
u32 ram_addr;
i32 ram_data;
u32 subcode1;
u32 subcode2;
u32 subcode3;

struct opcode_group i_opcodes_group[32];
struct opcode_group i_opcodes_b[8];
struct opcode_group i_opcodes_l[8];
struct opcode_group i_opcodes_s[8];
struct opcode_group i_opcodes_i[8];
struct opcode_group i_opcodes_i_1[128];
struct opcode_group i_opcodes_r[8];
struct opcode_group i_opcodes_r_1[128];
struct opcode_group i_opcodes_r_2[128];
struct opcode_group i_opcodes_c[8];

//lui rd, immediate: x[rd] = sext(immediate[31:12] << 12)
void i_lui(u32 code){
    rd = get_bits(code, 7, 5);
    imm_signed = (i32)(get_bits(code, 12, 20) << 12);
    log_deep_debug_direct("Tick = %d PC = %4x code=%08x rs = (%d,%d) rd = %d imm=(%u,%d) \n",tick,pc,code,rs1,rs2,rd,imm_unsigned,imm_signed);
    xreg[rd] = imm_signed;
    pc += 4;
    log_deep_debug_direct("LUI %04d\n",imm_signed);
}

void i_auipc(u32 code){
    //auipc rd, immediate: x[rd] = pc + sext(immediate[31:12] << 12)
    rd = get_bits(code, 7, 5);
    imm_signed = (i32)(get_bits(code, 12, 20) << 12);
    log_deep_debug_direct("Tick = %d PC = %4x code=%08x rs = (%d,%d) rd = %d imm=(%u,%d) \n",tick,pc,code,rs1,rs2,rd,imm_unsigned,imm_signed);
    xreg[rd] = pc + imm_signed;
    pc += 4;
    log_deep_debug_direct("AUIPC %04d\n",imm_signed);
}

void i_jal(u32 code){
    //jal rd, offset: x[rd] = pc+4; pc += sext(offset)
    rd = get_bits(code, 7, 5);
    imm_unsigned = (get_bits(code,12,8) << 12) | \
                 (get_bits(code,20,1) << 11) | \
                 (get_bits(code,21,10) << 1) | \
                 (get_bits(code,31,1) << 20);
    imm_signed = u2i(imm_unsigned, 21);
    log_deep_debug_direct("Tick = %d PC = %4x code=%08x rs = (%d,%d) rd = %d imm=(%u,%d) \n",tick,pc,code,rs1,rs2,rd,imm_unsigned,imm_signed);
    xreg[rd] = pc + 4;
    log_deep_debug_direct("JAL to (%08x,%08d)=%d. Store %08x to reg[%d]\n",pc,imm_signed,pc+imm_signed,pc+4,rd);
    pc += imm_signed;
}

void i_jalr(u32 code){
    //jalr rd, offset(rs1): t =pc+4; pc=(x[rs1]+sext(offset))&~1; x[rd]=t
    rd = get_bits(code, 7, 5);
    rs1 = get_bits(code, 15, 5);
    op1_signed = xreg[rs1];
    imm_unsigned = get_bits(code,20,12);
    imm_signed = u2i(imm_unsigned,12);
    log_deep_debug_direct("Tick = %d PC = %4x code=%08x rs = (%d,%d) rd = %d imm=(%u,%d) \n",tick,pc,code,rs1,rs2,rd,imm_unsigned,imm_signed);
    xreg[rd] = pc + 4;
    pc = (op1_signed+ imm_signed) & -1;
    log_deep_debug_direct("JALR (REG[%d]=%08x) to %04x. Store %08x to reg[%d]\n",rs1,op1_signed,pc,xreg[rd],rd);
}

void i_beq(u32 code){
    //beq rs1, rs2, offset: if (rs1 == rs2) pc += sext(offset)
    pc += (op1_signed== op2_signed)? imm_signed : 4;
}

void i_bne(u32 code){
    //bne rs1, rs2, offset: if (rs1 != rs2) pc += sext(offset)
   pc += (op1_signed!= op2_signed)? imm_signed : 4;
}

void i_blt(u32 code){
    //blt rs1, rs2, offset: if (rs1 <s rs2) pc += sext(offset)
    pc += (op1_signed< op2_signed)? imm_signed : 4;
}

void i_bge(u32 code){
    //bge rs1, rs2, offset: if (rs1 ≥s rs2) pc += sext(offset)
    pc += (op1_signed>= op2_signed)? imm_signed : 4;
}

void i_bltu(u32 code){
    //bltu rs1, rs2, offset: if (rs1 <u rs2) pc += sext(offset)
    pc += ((u32)op1_signed< (u32)xreg[rs2])? imm_signed : 4;
}

void i_bgeu(u32 code){
    //bgeu rs1, rs2, offset: if (rs1 ≥u rs2) pc += sext(offset)
    pc += ((u32)op1_signed>= (u32)xreg[rs2])? imm_signed : 4;
}

void i_lb(u32 code){
    //lb rd, offset(rs1): x[rd] = sext(M[x[rs1] + sext(offset)][7:0])
    i8 ram_data_ib;
    ram_data_ib = (i8)((ram_data >> (8*(ram_addr&3))) & 0xff);
    xreg[rd] = ram_data < 0? (ram_data_ib | 0xffffff80) : ram_data_ib;
}

void i_lh(u32 code){
    //lh rd, offset(rs1): x[rd] = sext(M[x[rs1] + sext(offset)][15:0])
    u16 ram_data_ih;
    ram_data_ih = (i16)((ram_data >> (8*(ram_addr&3))) & 0xffff);
    xreg[rd] = ram_data < 0? (ram_data_ih | 0xffff8000) : ram_data_ih;
}

void i_lw(u32 code){
    //lw rd, offset(rs1): x[rd] = sext(M[x[rs1] + sext(offset)][31:0])
    xreg[rd] = ram_data;
}

void i_lbu(u32 code){
    //lbu rd, offset(rs1): x[rd] = M[x[rs1] + sext(offset)][7:0]
    xreg[rd] = (u8)((ram_data>>(8*(ram_addr&3))) & 0xff);
}

void i_lhu(u32 code){
    //lhu rd, offset(rs1): x[rd] = M[x[rs1] + sext(offset)][15:0]
    xreg[rd] = (u16)((ram_data>>(8*(ram_addr&3))) & 0xffff);
}

void i_sb(u32 code){
    //sb rs2, offset(rs1): M[x[rs1]+sext(offset)=x[rs2][7:0]
    set_mem((op1_signed+imm_signed), op2_signed, DBYTE);
}

void i_sh(u32 code){
    //sh rs2, offset(rs1): M[x[rs1]+sext(offset)=x[rs2][15:0]
    set_mem((op1_signed+imm_signed), op2_signed, DWORD);
}

void i_sw(u32 code){
    //sw rs2, offset(rs1): M[x[rs1]+sext(offset)=x[rs2][31:0]  
    set_mem((op1_signed+imm_signed), op2_signed, DWORD);
}

void i_addi(u32 code){
    //addi rd, rs1, immediate: x[rd] = x[rs1] + sext(immediate)
    xreg[rd] = op1_signed + imm_signed;
}

void i_slti(u32 code){  
    //slti rd, rs1, immediate: x[rd]=(x[rs1]<(s)sext(immediate))
    xreg[rd] = op1_signed < imm_signed? 1 : 0; 
}

void i_sltiu(u32 code){
    //sltiu rd, rs1, immediate: x[rd]=(x[rs1]<(u)sext(immediate))
    xreg[rd] = op1_unsigned < imm_unsigned? 1 : 0;
}

void i_xori(u32 code){
    //xori rd, rs1, immediate: x[rd]=x[rs1] ^ sext(immediate)
    xreg[rd] = op1_signed ^ imm_signed;
}

void i_ori(u32 code){
    //ori: x[rd]=x[rs1] | sext(immediate)
    xreg[rd] = op1_signed | imm_signed;
}

void i_andi(u32 code){
    //andi: x[rd] = x[rs1] & sext(immediate)
    xreg[rd] = op1_signed & imm_signed;
}

void i_slli(u32 code){
    //slli: x[rd]=x[rs1]<<shamt  
    xreg[rd] = op1_signed << shamt;
}

void i_srli(u32 code){
    //srli rd, rs1, shamt: x[rd]=(x[rs1]>>(u)shamt)
    xreg[rd] = ((u32)op1_signed) >> shamt;
}

void i_srai(u32 code){
    //srai rd, rs1, shamt: x[rd]=(x[rs1]>>(s)shamt)
    xreg[rd] = op1_signed >> shamt; 
}

void i_add(u32 code){
    //add rd, rs1, rs2: x[rd] = x[rs1] + x[rs2]
    xreg[rd] = op1_signed + op2_signed;
}

void i_sub(u32 code){
    //sub rd, rs1, rs2: x[rd]=x[rs1]−x[rs2]  
    xreg[rd] = op1_signed - op2_signed;
}

void i_sll(u32 code){
    //sll rd, rs1, rs2: x[rd]=x[rs1]<<x[rs2]
    xreg[rd] = op1_signed << (op2_signed &0x1f);
}

void i_slt(u32 code){
    //slt rd, rs1, rs2: x[rd]=(x[rs1]<(s)x[rs2])
    xreg[rd] = op1_signed < op2_signed? 1: 0;
}

void i_sltu(u32 code){
    //sltu rd, rs1, rs2: x[rd]=(x[rs1]<(u)x[rs2])
    xreg[rd] = ((u32)op1_signed) < ((u32)op2_signed)? 1: 0;
}

void i_xor(u32 code){
    //xor rd, rs1, rs2: x[rd]=x[rs1] ^ x[rs2]
    xreg[rd] = ((u32)op1_signed) ^ ((u32)op2_signed);
}

void i_srl(u32 code){
    //srl rd, rs1, rs2: x[rd]=(x[rs1]>>(u)x[rs2])
    xreg[rd] = ((u32)op1_signed) >> (((u32)op2_signed) & 0x1f);
}

void i_sra(u32 code){
    //sra rd, rs1, rs2: x[rd]=(x[rs1]>>(s)x[rs2])
    xreg[rd] = op1_signed >> (op2_signed & 0x1f);
}

void i_or(u32 code){
    //or rd, rs1, rs2: x[rd]=x[rs1] | x[rs2]
    xreg[rd] = ((u32)op1_signed) | ((u32)op2_signed);
}

void i_and(u32 code){
    //and rd, rs1, rs2: x[rd] = x[rs1] & x[rs2]
    xreg[rd] = ((u32)op1_signed) & ((u32)op2_signed);
}

void i_csrrw(u32 code){
    //csrrw rd, csr, zimm[4:0]: t=CSRs[csr]; CSRs[csr]=x[rs1]; x[rd]=t
    u32 temp;
    temp = csr[csr_off];
    csr[csr_off] = (u32)op1_signed;
    xreg[rd] = temp;
}

void i_csrrs(u32 code){
    //csrrs rd, csr, rs1: t=CSRs[csr]; CSRs[csr]= t | x[rs1]; x[rd]=t
    u32 temp;
    temp = csr[csr_off];
    csr[csr_off] = temp | (u32)op1_signed;
    xreg[rd] = temp;
}

void i_csrrc(u32 code){
    //csrrc rd, csr, rs1: t=CSRs[csr]; CSRs[csr]= t & ~x[rs1]; x[rd]=t
    u32 temp;
    temp = csr[csr_off];
    csr[csr_off] = temp & (~((u32)op1_signed));
    xreg[rd] = temp;
}

void i_csrrwi(u32 code){
    //csrrwi rd, csr, zimm[4:0]: x[rd]=CSRs[csr]; CSRs[csr]= zimm
    xreg[rd] = csr[csr_off];
    csr[csr_off] = zimm;
}

void i_csrrsi(u32 code){
    //csrrsi rd, csr, zimm[4:0]: t=CSRs[csr]; CSRs[csr]= t | zimm; x[rd]=t
    u32 temp;
    temp = csr[csr_off];
    csr[csr_off] = temp | zimm;
    xreg[rd] = temp;
}

void i_csrrci(u32 code){
    //csrrci rd, csr, rs1: t=CSRs[csr]; CSRs[csr]= t & ~zimm; x[rd]=t

    u32 temp;
    temp = csr[csr_off];
    csr[csr_off] = temp & (~zimm);
    xreg[rd] = temp;
}

void i_b(u32 code){
    rs1 = get_bits(code, 15, 5);
    rs2 = get_bits(code, 20, 5);
    op1_signed = xreg[rs1];
    op2_signed = xreg[rs2];
    imm_unsigned = (get_bits(code,7,1) << 11) | \
                 (get_bits(code,8,4) << 1) | \
                 (get_bits(code,25,6) << 5) | \
                 (get_bits(code,31,1) << 12);
    imm_signed = u2i(imm_unsigned,13);
    log_deep_debug_direct("Tick = %d PC = %4x code=%08x rs = (%d,%d) rd = %d imm=(%u,%d) \n",tick,pc,code,rs1,rs2,rd,imm_unsigned,imm_signed);
    i_opcodes_b[subcode2].exec(code);
    i_opcodes_b[subcode2].count++;
    log_deep_debug_direct("Branch to %04x\n",pc);
}

void i_l(u32 code){
    rd = get_bits(code, 7, 5);
    rs1 = get_bits(code, 15, 5);
    op1_signed = xreg[rs1];
    imm_unsigned = get_bits(code,20,12);
    imm_signed = u2i(imm_unsigned,12);
    ram_addr =(op1_signed + imm_signed); 
    ram_data = (i32) get_mem(ram_addr);
    log_deep_debug_direct("Tick = %d PC = %4x code=%08x rs = (%d,%d) rd = %d imm=(%u,%d) \n",tick,pc,code,rs1,rs2,rd,imm_unsigned,imm_signed);
    i_opcodes_l[subcode2].exec(code);
    i_opcodes_l[subcode2].count++;
    pc += 4;
    log_debug_direct("Load %08x from %08x\n",ram_data,ram_addr);
}

void i_s(u32 code){
    rs1 = get_bits(code, 15, 5);
    rs2 = get_bits(code, 20, 5);
    op1_signed = xreg[rs1];
    op2_signed = xreg[rs2];
    imm_unsigned = (get_bits(code,25,7)<<5) | get_bits(code,7,5);
    imm_signed = u2i(imm_unsigned,12);
    log_deep_debug_direct("Tick = %d PC = %4x code=%08x rs = (%d,%d) rd = %d imm=(%u,%d) \n",tick,pc,code,rs1,rs2,rd,imm_unsigned,imm_signed);
    i_opcodes_s[subcode2].exec(code);
    i_opcodes_s[subcode2].count++;
    pc += 4;
    log_debug_direct("Store %08x to %08x(%08x,%08x)\n",op2_signed,op1_signed+imm_signed,op1_signed,imm_signed);
}

void i_i(u32 code){
    rd = get_bits(code, 7, 5);
    rs1 = get_bits(code, 15, 5);
    op1_signed = xreg[rs1];
    op1_unsigned = (u32)xreg[rs1];
    imm_unsigned = get_bits(code,20,12);
    imm_signed = u2i(imm_unsigned,12);
    shamt = get_bits(code,20,5);
    log_deep_debug_direct("Tick = %d PC = %4x code=%08x rs = (%d,%d) rd = %d imm=(%u,%d) \n",tick,pc,code,rs1,rs2,rd,imm_unsigned,imm_signed);
    i_opcodes_i[subcode2].exec(code);
    i_opcodes_i[subcode2].count++;
    pc += 4;
    log_deep_debug_direct("IMM \n");
}

void i_srli_srai(u32 code){
    subcode3 = get_bits(code, 25, 7);
    i_opcodes_i_1[subcode3].exec(code);
    i_opcodes_i_1[subcode3].count++;
}

void i_r(u32 code){
    rd = get_bits(code, 7, 5);
    rs1 = get_bits(code, 15, 5);
    rs2 = get_bits(code, 20, 5);
    op1_signed = xreg[rs1];
    op2_signed = xreg[rs2];
    log_deep_debug_direct("Tick = %d PC = %4x code=%08x rs = (%d,%d) rd = %d imm=(%u,%d) \n",tick,pc,code,rs1,rs2,rd,imm_unsigned,imm_signed);
    i_opcodes_r[subcode2].exec(code);
    i_opcodes_r[subcode2].count++;
    pc += 4;
    log_deep_debug_direct("REG \n");
}

void i_add_sub(u32 code){
    subcode3 = get_bits(code, 25, 7);
    i_opcodes_r_1[subcode3].exec(code);
    i_opcodes_r_1[subcode3].count++;
}

void i_srl_sra(u32 code){
    subcode3 = get_bits(code, 25, 7);
    i_opcodes_r_2[subcode3].exec(code);
    i_opcodes_r_2[subcode3].count++;
}

void i_c(u32 code){
    rd = get_bits(code, 7, 5);
    rs1 = get_bits(code, 15, 5);
    op1_signed = xreg[rs1];
    zimm = (u32)op1_signed;
    csr_off = get_bits(code,20,12);
    log_deep_debug_direct("Tick = %d PC = %4x code=%08x rs = (%d,%d) rd = %d imm=(%u,%d) \n",tick,pc,code,rs1,rs2,rd,imm_unsigned,imm_signed);
    i_opcodes_c[subcode2].exec(code);
    i_opcodes_c[subcode2].count++;
    pc += 4;
    log_deep_debug_direct("CSR \n");
}

void i_exec(u32 code){
    subcode1 = get_bits(code, 2, 5);
    subcode2 = get_bits(code, 12, 3);
    i_opcodes_group[subcode1].exec(code);
    i_opcodes_group[subcode1].count++;
    xreg[0] = 0;// some instruction will set reg[0]. But it is hard wired as 0 in hardware;
    tick += 1;
}

void i_result(){
    log_deep_debug_direct("RV32I result: \n");
    log_tested_print("Opcode", i_opcodes_group, 32);
    log_tested_print("Branch", i_opcodes_b, 8);
    log_tested_print("Load  ", i_opcodes_l, 8);
    log_tested_print("Store ", i_opcodes_s, 8);
    log_tested_print("Imm   ", i_opcodes_i, 8);
    log_tested_print("Reg   ", i_opcodes_r, 8);
    log_tested_print("Imm101", i_opcodes_i_1, 128);
    log_tested_print("Reg000", i_opcodes_r_1, 128);
    log_tested_print("Reg101", i_opcodes_r_2, 128);
    log_tested_print("CSR   ", i_opcodes_c, 8);
}

void  __attribute__((constructor)) i_opcodes_group_init(){
    u32 i;

    opcodes_group[OP_I_CODE].exec = i_exec;
    opcodes_group[OP_I_CODE].result = i_result;

    for (i = 0; i < 32; i++) {
        i_opcodes_group[i].exec = NULL;
    }
    i_opcodes_group[OP_I_LUI>>2].exec = i_lui;
    i_opcodes_group[OP_I_AUIPC>>2].exec = i_auipc;
    i_opcodes_group[OP_I_JAL>>2].exec = i_jal;
    i_opcodes_group[OP_I_JALR>>2].exec = i_jalr;
    i_opcodes_group[OP_I_B>>2].exec = i_b;
    i_opcodes_group[OP_I_L>>2].exec = i_l;
    i_opcodes_group[OP_I_S>>2].exec = i_s;
    i_opcodes_group[OP_I_I>>2].exec = i_i;
    i_opcodes_group[OP_I_R>>2].exec = i_r;
    i_opcodes_group[OP_I_C>>2].exec = i_c;

    for (i = 0; i < 8; i++) {
        i_opcodes_b[i].exec = NULL;
        i_opcodes_b[i].count = 0;
    }
    i_opcodes_b[0].exec = i_beq;
    i_opcodes_b[1].exec = i_bne;
    i_opcodes_b[4].exec = i_blt;
    i_opcodes_b[5].exec = i_bge;
    i_opcodes_b[6].exec = i_bltu;
    i_opcodes_b[7].exec = i_bgeu;

    for (i = 0; i < 8; i++) {
        i_opcodes_l[i].exec = NULL;
        i_opcodes_l[i].count = 0;
    }
    i_opcodes_l[0].exec = i_lb;
    i_opcodes_l[1].exec = i_lh;
    i_opcodes_l[2].exec = i_lw;
    i_opcodes_l[4].exec = i_lbu;
    i_opcodes_l[5].exec = i_lhu;

    for (i = 0; i < 8; i++) {
        i_opcodes_s[i].exec = NULL;
        i_opcodes_s[i].count = 0;
    }
    i_opcodes_s[0].exec = i_sb;
    i_opcodes_s[1].exec = i_sh;
    i_opcodes_s[2].exec = i_sw;

    for (i = 0; i < 8; i++) {
        i_opcodes_i[i].exec = NULL;
        i_opcodes_i[i].count = 0;
    }
    i_opcodes_i[0].exec = i_addi;
    i_opcodes_i[2].exec = i_slti;
    i_opcodes_i[3].exec = i_sltiu;
    i_opcodes_i[4].exec = i_xori;
    i_opcodes_i[6].exec = i_ori;
    i_opcodes_i[7].exec = i_andi;
    i_opcodes_i[1].exec = i_slli;
    i_opcodes_i[5].exec = i_srli_srai;

    for (i = 0; i < 128; i++) {
        i_opcodes_i_1[i].exec = NULL;
        i_opcodes_i_1[i].count = 0;
    }
    i_opcodes_i_1[0].exec = i_srli;
    i_opcodes_i_1[0x20].exec = i_srai;

    for (i = 0; i < 8; i++) {
        i_opcodes_r[i].exec = NULL;
        i_opcodes_r[i].count = 0;
    }
    i_opcodes_r[0].exec = i_add_sub;
    i_opcodes_r[1].exec = i_sll;
    i_opcodes_r[2].exec = i_slt;
    i_opcodes_r[3].exec = i_sltu;
    i_opcodes_r[4].exec = i_xor;
    i_opcodes_r[5].exec = i_srl_sra;
    i_opcodes_r[6].exec = i_or;
    i_opcodes_r[7].exec = i_and;

    for (i = 0; i < 128; i++) {
        i_opcodes_r_1[i].exec = NULL;
        i_opcodes_r_1[i].count = 0;
    }
    i_opcodes_r_1[0].exec = i_add;
    i_opcodes_r_1[0x20].exec = i_sub;

    for (i = 0; i < 128; i++) {
        i_opcodes_r_2[i].exec = NULL;
        i_opcodes_r_2[i].count = 0;
    }
    i_opcodes_r_2[0].exec = i_srl;
    i_opcodes_r_2[0x20].exec = i_sra;


    for (i = 0; i < 8; i++) {
        i_opcodes_c[i].exec = NULL;
        i_opcodes_c[i].count = 0;
    }
    i_opcodes_c[1].exec = i_csrrw;
    i_opcodes_c[2].exec = i_csrrs;
    i_opcodes_c[3].exec = i_csrrc;
    i_opcodes_c[5].exec = i_csrrwi;
    i_opcodes_c[6].exec = i_csrrsi;
    i_opcodes_c[7].exec = i_csrrci;
}
