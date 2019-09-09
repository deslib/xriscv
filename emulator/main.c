#include "glb.h"
#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include "utils.h"

i32 imm_signed = 0;
u32 imm_unsigned = 0;

u8 opcode;
u8 opcode_6t2;
u8 reg_dest_sel;
u8 reg1_src_sel;
u8 reg2_src_sel;
u8 shamt;
u8 funct3;
u8 funct7;

i32 operand1_signed;
i32 operand2_signed;
u32 operand1_unsigned;
u32 operand2_unsigned;

u32 pc;
i32 xreg[32];

u32 rom[2048];
u32 ram[2048];

u32 get_bits(u32 val, u8 bit_start, u8 length){
    return (val >> bit_start) & ((1<<length)-1);
}

void get_operand(){
    operand1_signed = xreg[reg1_src_sel];
    operand2_signed = xreg[reg2_src_sel];
    operand1_unsigned = xreg[reg1_src_sel];
    operand2_unsigned = xreg[reg2_src_sel];
}

void decode(){
    u32 code;
    u8 opcode_4t2;
    u8 opcode_6t5;

    code = rom[pc/4];
    opcode = get_bits(code,0,7);
    opcode_6t2 = get_bits(code,2,5);
    opcode_4t2 = get_bits(code,2,3);
    opcode_6t5 = get_bits(code,5,2);
    reg_dest_sel = get_bits(code,7,5);
    reg1_src_sel = get_bits(code,15,5);
    reg2_src_sel = get_bits(code,20,5);
    funct3 = get_bits(code,12,3);
    funct7 = get_bits(code,25,7);
    printf("PC = %4x \t code=%08x ",pc,code);
    if(opcode_4t2 == 5){
        imm_signed = code & 0xfffff000;
    }else if(opcode_4t2 == 3){
        imm_signed = (get_bits(code,12,8) << 12) | \
                     (get_bits(code,20,1) << 11) | \
                     (get_bits(code,21,10) << 1) | \
                     (get_bits(code,31,1) << 20);
        if(get_bits(code,31,1)){
            imm_signed = 0xffe00000 | imm_signed;
        }
    }else if(opcode_4t2 == 1){
        imm_signed = get_bits(code,20,12) << 20;
    }else if(opcode_4t2 == 0){
        if(opcode_6t5 == 3){
            imm_signed = (get_bits(code,7,1) << 11) | \
                         (get_bits(code,8,4) << 1) | \
                         (get_bits(code,25,6) << 5) | \
                         (get_bits(code,31,1) << 12);
        }else if(opcode_6t5 == 0){
            if(funct3 == 4 || funct3 == 5){
                imm_unsigned = get_bits(code,20,12);
            }else{
                if(code > 0){
                    imm_signed = get_bits(code,20,12);
                }else{
                    imm_signed = 0xfffff000 & get_bits(code,20,12);
                }
            }
        }else{
            imm_signed = (get_bits(code,25,7)<<5) | get_bits(code,7,5);
        }
    }else if(opcode_4t2 == 4){
        imm_signed = get_bits(code,20,12);
        shamt = get_bits(code,20,5);
    }
}

void exec(){
    u8 jmp;
    u8 i;
    i32 ram_data;
    switch(opcode_6t2){
        case OP_LUI:
            xreg[reg_dest_sel] = imm_signed;
            pc += 4;
            log_deep_debug_direct("LUI %04d\n",imm_signed);
            break;
        case OP_AUIPC:
            pc += imm_signed;
            log_deep_debug_direct("AUIPC %04d\n",imm_signed);
            break;
        case OP_JAL:
            xreg[reg_dest_sel] = pc + 4;
            log_deep_debug_direct("JAL to (%08x,%08d)=%d. Store %08x to reg[%d]\n",pc,imm_signed,pc+imm_signed,pc+4,reg_dest_sel);
            pc += imm_signed;
            break;
        case OP_JALR:
            xreg[reg_dest_sel] = pc + 4;
            pc = (xreg[reg1_src_sel] + imm_signed);
            log_deep_debug_direct("JALR (REG[%d]=%08x) to %04x. Store %08x to reg[%d]\n",reg1_src_sel,xreg[reg1_src_sel],pc,xreg[reg_dest_sel],reg_dest_sel);
            break;
        case OP_BRANCH:
            if((funct3 & 6) == 0){
                jmp = ((funct3&1) == 0) ^ (operand1_signed == operand2_signed);
            }else if( (funct3 & 6) == 4){
                jmp = ((funct3&1) == 0) ^ (operand1_signed < operand2_signed);
            }else if( (funct3 & 6) == 6){
                jmp = ((funct3&1) == 0) ^(operand1_unsigned < operand2_unsigned);
            }
            if(jmp){
                pc += imm_signed;
            }else{
                pc += 4;
            }
            log_deep_debug_direct("Branch to %04x\n",pc);
            break;
        case OP_LOAD:
            ram_data = ram[(xreg[reg1_src_sel] + imm_signed)>>2];
            log_deep_debug_direct("Load %08x from %04x\n",ram_data,xreg[reg1_src_sel]+imm_signed);
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
            }else{
                log_error("OP_LOAD funct3 error\n");
            }
            pc += 4;
            break;
        case OP_STORE:
            log_deep_debug_direct("Store %08x to %08x(%08x,%08x)\n",xreg[reg2_src_sel],xreg[reg1_src_sel]+imm_signed,xreg[reg1_src_sel],imm_signed);
            if(funct3 == 0){
                ram[(xreg[reg1_src_sel]+imm_signed)>>2] = xreg[reg2_src_sel] & 0xff;
            }else if(funct3 == 1){
                ram[(xreg[reg1_src_sel]+imm_signed)>>2] = xreg[reg2_src_sel] & 0xffff;
            }else if(funct3 == 2){
                ram[(xreg[reg1_src_sel]+imm_signed)>>2] = xreg[reg2_src_sel];
            }else{
                log_error("OP_STORE funct3 error\n");
            }
            pc += 4;
            break;
        case OP_IMM:
            if(funct3 == 0){
                xreg[reg_dest_sel] = xreg[reg1_src_sel] + imm_signed;
            }else if(funct3 == 1){
                xreg[reg_dest_sel] = xreg[reg1_src_sel] << shamt;
            }else if(funct3 ==2){
                if(xreg[reg1_src_sel] < imm_signed){
                    xreg[reg_dest_sel] = 1;
                }else{
                    xreg[reg_dest_sel] = 0;
                }
            }else if(funct3 == 3){
                if((u32)xreg[reg1_src_sel] < imm_unsigned){
                    xreg[reg_dest_sel] = 1;
                }else{
                    xreg[reg_dest_sel] = 0;
                }
            }else if(funct3 == 4){
                xreg[reg_dest_sel] = (u32)xreg[reg1_src_sel] ^ imm_unsigned;
            }else if(funct3 == 5){
                //if(funct7 == 0){
                    xreg[reg_dest_sel] = xreg[reg1_src_sel] >> shamt;
                //}else{
                //    xreg[reg_dest_sel] = xreg[reg1_src_sel] >>> shamt;
                //}
            }else if(funct3 == 6){
                xreg[reg_dest_sel] = (u32)xreg[reg1_src_sel] | imm_unsigned;
            }else if(funct3 == 7){
                xreg[reg_dest_sel] = (u32)xreg[reg1_src_sel] & imm_unsigned;
            }
            pc += 4;
            log_deep_debug_direct("IMM \n");
            break;
        case OP_REG:
            if(funct3 == 0){
                if(funct7 == 0){
                    xreg[reg_dest_sel] = xreg[reg1_src_sel] + xreg[reg2_src_sel];
                }else{
                    xreg[reg_dest_sel] = xreg[reg1_src_sel] - xreg[reg2_src_sel];
                }
            }else if(funct3 == 1){
                xreg[reg_dest_sel] = xreg[reg1_src_sel] << xreg[reg2_src_sel];
            }else if(funct3 == 2){
                if(xreg[reg1_src_sel] < xreg[reg2_src_sel]){
                    xreg[reg_dest_sel] = 1;
                }else{
                    xreg[reg_dest_sel] = 0;
                }
            }else if(funct3 == 3){
                if((u32)xreg[reg1_src_sel] < (u32)xreg[reg2_src_sel]){
                    xreg[reg_dest_sel] = 1;
                }else{
                    xreg[reg_dest_sel] = 0;
                }
            }else if(funct3 == 4){
                xreg[reg_dest_sel] = (u32)xreg[reg1_src_sel] ^ (u32)xreg[reg2_src_sel];
            }else if(funct3 == 5){
                //if(funct7 == 0){
                    xreg[reg_dest_sel] = xreg[reg1_src_sel] >> xreg[reg2_src_sel];
                //}else{
                //    xreg[reg_dest_sel] = xreg[reg1_src_sel] >>> xreg[reg2_src_sel];
                //}
            }else if(funct3 == 6){
                xreg[reg_dest_sel] = (u32)xreg[reg1_src_sel] | (u32)xreg[reg2_src_sel];
            }else if(funct3 == 7){
                xreg[reg_dest_sel] = (u32)xreg[reg1_src_sel] & (u32)xreg[reg2_src_sel];
            }
            pc += 4;
            log_deep_debug_direct("REG \n");
            break;
    }
    for(i=0;i<32;i++){
        log_deep_debug_direct("%08x ",xreg[i]);
        if((i%8) == 7){
            log_deep_debug_direct("\n");
        }
    }
    log_deep_debug_direct("\n");
}

u32 read2ram(char *ram_file){
    FILE *fp;
    i32 code;
    u32 ram_index = 0;
    int ret_items_num;
    fp = fopen(ram_file,"r");
    if(fp){
        log_info("Read rom file\n");
    }else{
        log_error("Can't file the file: %s\n",ram_file);
        exit(0);
    }
    while(1){
        ret_items_num = fscanf(fp,"%x\n",&code);
        if(ret_items_num == 1){
            rom[ram_index] = (u32)code;
            log_deep_debug("%04d: %08x\n",ram_index,code);
            ram_index += 1;
        }else{
            fclose(fp);
            log_info("\nRead rom file done\n");
            break;
        }
    }
    return ram_index;
}

void init(){
    u8 i;
    for(i=0;i<32;i++){
        xreg[i] = 0;
    }
}

void main(){
    u32 rom_length;
    init();
    log_init();
    rom_length = read2ram("../c/xriscv.rom");
    while(1){
        decode();
        get_operand();
        exec();
        if(pc/4 > rom_length) break;
    }
}

