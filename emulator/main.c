#include "glb.h"

i32 imm_signed = 0;
u32 imm_unsigned = 0;

u8 opcode;
u8 opcode_6t2;
u8 reg_dest_sel;
u8 reg1_src_sel;
u8 reg2_src_sel;
u8 shamt;

i32 operand1_signed;
i32 operand2_signed;
u32 operand1_unsigned;
u32 operand2_unsigned;

u32 pc;
i32 xreg[32];

u32 ram[2048];

void get_bits(u32 val, u8 bit_start, u8 length){
    return (val >> bit_start) & ((1<<length)-1)
}

void get_operand(){
    operand1_signed = xreg[reg1_src_sel];
    operand2_signed = xreg[reg2_src_sel];
    operand1_unsigned = xreg[reg1_src_sel];
    operand2_unsigned = xreg[reg2_src_sel];
}

void decode(u32 code){
    u8 opcode_4t2;
    u8 opcode_6t5;
    opcode = get_bits(code,0,7);
    opcode_6t2 = get_bits(code,2,5);
    opcode_4t2 = get_bits(code,2,3);
    opcode_6t5 = get_bits(code,5,2);
    reg_dest_sel = get_bits(code,7,5);
    reg1_src_sel = get_bits(code,15,5);
    reg2_src_sel = get_bits(code,20,5);
    funct3 = get_bits(code,12,3);
    funct7 = get_bits(code,25,7);
    if(opcode_4t2 == 5){
        imm_signed = code & 0xfffff000;
    }else if(opcode_4t2 == 3){
        imm_signed = (get_bits(code,12,8) << 12) | \
                     (get_bits(code,20,1) << 11) | \
                     (get_bits(code,21,10) << 1) | \
                     (get_bits(code,31,1) << 20);
    }else if(opcode_4t2 == 1){
        imm_signed = get_bits(code,20,12) << 20;
    }else if(opcode_4t2 == 0){
        if(opcode_6t5 == 3){
            imm_signed = (get_bits(code,7,1) << 11) | \
                         (get_bits(code,8,4) << 1) | \
                         (get_bits(code,25,6) << 5) | \
                         (get_bits(code,31,1) << 12);
        }else{
            if(funct3 == 4 || funct3 == 5){
                imm_unsigned = get_bits(code,20,12);
            }else{
                if(code > 0){
                    imm_signed = get_bits(code,20,12);
                }else{
                    imm_signed = 0xfffff000 & get_bits(code,20,12);
                }
            }
        }
    }else if(opcode_4t2 == 4){
        imm_signed = get_bits(code,20,12);
        shamt = get_bits(code,20,5);
    }
}

void exec(){
    u8 jmp;
    switch(opcode_6t2){
        case OP_LUI:
            xreg[reg_dest_sel] = imm_signed;
            op += 4;
            break;
        case OP_AUIPC:
            pc += imm_signed;
            break;
        case OP_JAL:
            xreg[reg_dest_sel] = pc + 4;
            pc += imm_signed;
            break;
        case OP_JALR:
            xreg[reg_dest_sel] = pc + 4;
            pc = xreg[reg1_src_sel] + imm_signed;
            break;
        case OP_BRANCH:
            if( ((funct3 & 6) == 0) && operand1_signed == operand2_signed){
                jmp = (funct3&1) == 0;
            }else if( ( (funct3 & 6) == 4) && operand1_signed < operand2_signed){
                jmp = (funct3&1) == 0;
            }else if( ( (funct3 & 6) == 6) && operand1_unsigned < operand2_unsigned){
                jmp = (funct3&1) == 0;
            }
            if(jmp){
                pc += imm_signed;
            }else{
                pc += 4;
            }
            break;
        case OP_LOAD:
            ram_data = ram[xreg[reg1_src_sel] + imm_signed];
            if(funct3 == 0){
                if(ram_data < 0){
                    xreg[reg_dest_sel] = ram_data | 0xffffff80;
                }else{
                    xreg[reg_dest_sel] = ram_data & 0xff;
                }
            }else if(funct3 == 1){
                if(ram_data < 0){
                    xreg[reg_dest_sel] = ram_data | 0xffff8000;
                }else{
                    xreg[reg_dest_sel] = ram_data & 0xffff;
                }
            }else if(funct3 == 2){
                xreg[reg_dest_sel] = ram_data;
            }else if(funct3 == 3){
                xreg[reg_dest_sel] = ram_data & 0xff;
            }else if(funct3 == 4){
                xreg[reg_dest_sel] = ram_data & 0xffff;
            }
            break;
        case OP_STORE:

    }
}

void main(){
}

