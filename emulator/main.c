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
u32 pc_prev;
i32 xreg[32];

u32 rom[2048];
u32 ram[2048];
u32 regfile[10];

u32 tick = 0;
u32 dead_loop_cnt = 0;

FILE *fp_uart;

void end_sim(){
    fclose(fp_uart);
    exit(0);
}

void uart_send_byte(char a){
    fprintf(fp_uart,"%c",a);
}

u32 get_bits(u32 val, u8 bit_start, u8 length){
    return (val >> bit_start) & ((1<<length)-1);
}

u32 get_mem(u32 addr){
    if((addr >= RAM_BASE_ADDR) && (addr < RAM_BASE_ADDR + RAM_SIZE)){
        return ram[(addr-RAM_BASE_ADDR)>>2];
    }else if((addr >= REG_BASE_ADDR) && (addr < REG_BASE_ADDR + REG_SIZE)){
        return regfile[(addr-REG_BASE_ADDR)>>2];
    }else{
        log_error("Read invalid memory address %08x\n",addr);
        exit(0);
    }
}

u32 set_mem(u32 addr,u32 val,u8 type){
    u32 raw_value;
    if((addr >= RAM_BASE_ADDR) && (addr < RAM_BASE_ADDR + RAM_SIZE)){
        raw_value = ram[(addr-RAM_BASE_ADDR)>>2];
        if(type == DBYTE){
            ram[(addr-RAM_BASE_ADDR)>>2] = (val<<(8*(addr&3))) | (raw_value & (0xffffffff - (0xff << (8*(addr&3)))));
        }else if(type == DHALF){
            ram[(addr-RAM_BASE_ADDR)>>2] = (val<<(8*(addr&3))) | (raw_value & (0xffffffff - (0xffff << (8*(addr&3)))));
        }else{
            ram[(addr-RAM_BASE_ADDR)>>2] = val;
        }
    }else if((addr >= REG_BASE_ADDR) && (addr < REG_BASE_ADDR + REG_SIZE)){
        raw_value = regfile[(addr-REG_BASE_ADDR)>>2];
        if(type == DBYTE){
            regfile[(addr-REG_BASE_ADDR)>>2] = (val<<(8*(addr&3))) | (raw_value & (0xffffffff - (0xff << (8*(addr&3)))));
        }else if(type == DHALF){
            regfile[(addr-REG_BASE_ADDR)>>2] = (val<<(8*(addr&3))) | (raw_value & (0xffffffff - (0xffff << (8*(addr&3)))));
        }else{
            regfile[(addr-REG_BASE_ADDR)>>2] = val;
        }
        if((addr & 0xfffc) == 0x1000){
            if((type == DBYTE) && ((addr & 3) == 1)){
                uart_send_byte((u8)(val&0xff));
            }else if((type == DHALF) && (addr & 1) == 1){
                uart_send_byte((u8)((val>>8)&0xff));
            }else{
                uart_send_byte((u8)((val>>8)&0xff));
            }
        }
    }else{
        log_error("Write invalid memory address %08x\n",addr);
        exit(0);
    }
}

void get_operand(){
    operand1_signed = xreg[reg1_src_sel];
    operand2_signed = xreg[reg2_src_sel];
    operand1_unsigned = xreg[reg1_src_sel];
    operand2_unsigned = xreg[reg2_src_sel];
}

i32 u2i(u32 uval, u8 length){
    i32 ival;
    if(uval >> (length-1)){
        ival = (0xffffffff-(1<<length)+1) | uval;
    }else{
        ival = uval;
    }
    return ival;
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
    if(opcode_4t2 == 5){
        imm_unsigned = code & 0xfffff000;
        imm_signed = (i32)imm_unsigned;
    }else if(opcode_4t2 == 3){
        imm_unsigned = (get_bits(code,12,8) << 12) | \
                     (get_bits(code,20,1) << 11) | \
                     (get_bits(code,21,10) << 1) | \
                     (get_bits(code,31,1) << 20);
        imm_signed = u2i(imm_unsigned,21);
    }else if(opcode_4t2 == 1){
        imm_unsigned = get_bits(code,20,12);
        imm_signed = u2i(imm_unsigned,12);
    }else if(opcode_4t2 == 0){
        if(opcode_6t5 == 3){
            imm_unsigned = (get_bits(code,7,1) << 11) | \
                         (get_bits(code,8,4) << 1) | \
                         (get_bits(code,25,6) << 5) | \
                         (get_bits(code,31,1) << 12);
            imm_signed = u2i(imm_unsigned,13);
        }else if(opcode_6t5 == 0){
            if(funct3 == 4 || funct3 == 5){
                imm_unsigned = get_bits(code,20,12);
            }else{
                imm_unsigned = get_bits(code,20,12);
            }
            imm_signed = u2i(imm_unsigned,12);
        }else{
            imm_unsigned = (get_bits(code,25,7)<<5) | get_bits(code,7,5);
            imm_signed = u2i(imm_unsigned,12);
        }
    }else if(opcode_4t2 == 4){
        imm_unsigned = get_bits(code,20,12);
        imm_signed = u2i(imm_unsigned,12);
        shamt = get_bits(code,20,5);
    }
    printf("Tick = %d PC = %4x code=%08x rs = (%d,%d) rd = %d imm=(%u,%d) ",tick,pc,code,reg1_src_sel,reg2_src_sel,reg_dest_sel,imm_unsigned,imm_signed);
    tick += 1;
    if(pc_prev == pc){
        dead_loop_cnt += 1;
        if(dead_loop_cnt > 10){
            end_sim();
        }
    }
    pc_prev = pc;
}

void exec(){
    u8 jmp;
    u8 i;
    u32 ram_addr;
    i32 ram_data;
    u8 ram_data_ub;
    i8 ram_data_ib;
    u16 ram_data_uh;
    u16 ram_data_ih;
    u32 ram_data_uw;
    u32 ram_data_iw;
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
                jmp = (funct3&1) ^ (operand1_signed == operand2_signed);
            }else if( (funct3 & 6) == 4){
                jmp = (funct3&1) ^ (operand1_signed < operand2_signed);
            }else if( (funct3 & 6) == 6){
                jmp = (funct3&1) ^(operand1_unsigned < operand2_unsigned);
            }
            if(jmp){
                pc += imm_signed;
            }else{
                pc += 4;
            }
            log_deep_debug_direct("Branch to %04x\n",pc);
            break;
        case OP_LOAD:
            ram_addr =(xreg[reg1_src_sel] + imm_signed); 
            ram_data = get_mem(ram_addr);
            log_deep_debug_direct("Load %08x from %04x\n",ram_data,ram_addr);
            if(funct3 == 0){
                ram_data_ub = (ram_addr >> (8*(ram_addr&3))) & 0xff;
                ram_data_ib = ram_data_ub;
                if(ram_data < 0){
                    xreg[reg_dest_sel] = ram_data_ib | 0xffffff80;
                }else{
                    xreg[reg_dest_sel] = ram_data_ib;
                }
            }else if(funct3 == 1){
                ram_data_uh = (ram_addr >> (8*(ram_addr&3))) & 0xffff;
                ram_data_ih = ram_data_uh;
                if(ram_data < 0){
                    xreg[reg_dest_sel] = ram_data_ih | 0xffff8000;
                }else{
                    xreg[reg_dest_sel] = ram_data_ih;
                }
            }else if(funct3 == 2){
                xreg[reg_dest_sel] = ram_data;
            }else if(funct3 == 4){
                xreg[reg_dest_sel] = (ram_data>>(8*(ram_addr&3))) & 0xff;
            }else if(funct3 == 5){
                xreg[reg_dest_sel] = (ram_data>>(8*(ram_addr&3))) & 0xffff;
            }else{
                log_error("OP_LOAD funct3 error\n");
            }
            pc += 4;
            break;
        case OP_STORE:
            log_deep_debug_direct("Store %08x to %08x(%08x,%08x)\n",xreg[reg2_src_sel],xreg[reg1_src_sel]+imm_signed,xreg[reg1_src_sel],imm_signed);
            if(funct3 == 0){
                set_mem((xreg[reg1_src_sel]+imm_signed), xreg[reg2_src_sel],DBYTE);
            }else if(funct3 == 1){
                set_mem((xreg[reg1_src_sel]+imm_signed), xreg[reg2_src_sel],DHALF);
            }else if(funct3 == 2){
                set_mem((xreg[reg1_src_sel]+imm_signed), xreg[reg2_src_sel],DWORD);
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
    xreg[0] = 0;// some instruction will set reg[0]. But it is hard wired as 0 in hardware;
    for(i=0;i<32;i++){
        log_deep_debug_direct("%08x ",xreg[i]);
        if((i%8) == 7){
            log_deep_debug_direct("\n");
        }
    }
    log_deep_debug_direct("\n");

    for(i=0;i<32;i++){
        log_deep_debug_direct("%08x ",ram[i]);
        if((i%8) == 7){
            log_deep_debug_direct("\n");
        }
    }
    log_deep_debug_direct("\n");
}

u32 read2rxm(char *rxm_file, u8 dest_is_ram){
    FILE *fp;
    i32 code;
    u32 rxm_index = 0;
    int ret_items_num;
    fp = fopen(rxm_file,"r");
    if(fp){
        log_info("Read rom file\n");
    }else{
        log_error("Can't file the file: %s\n",rxm_file);
        exit(0);
    }
    while(1){
        ret_items_num = fscanf(fp,"%x\n",&code);
        if(ret_items_num == 1){
            if(dest_is_ram){
                ram[rxm_index] = (u32)code;
            }else{
                rom[rxm_index] = (u32)code;
            }
            log_deep_debug("%04d: %08x\n",rxm_index,code);
            rxm_index += 1;
        }else{
            fclose(fp);
            log_info("\nRead rom file done\n");
            break;
        }
    }
    return rxm_index;
}

void init(){
    u8 i;
    for(i=0;i<32;i++){
        xreg[i] = 0;
    }
    for(i=0;i<10;i++){
        regfile[0] = 0x0000000; //status
    }
    fp_uart = fopen("uart.txt","w");
}

void main(){
    u32 rom_length;
    init();
    log_init();
    rom_length = read2rxm("../c/xriscv.rom",TO_ROM);
    read2rxm("../c/xriscv.ram",TO_RAM);
    while(1){
        decode();
        get_operand();
        exec();
        if(pc/4 > rom_length) break;
    }
}

