#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glb.h"
#include "logger.h"
#include "utils.h"
#include "csr.h"
#include "rv32i.h"
#include "main.h"

struct opcode_group opcodes_group[4] = {
    {.exec = NULL, .result = NULL},
    {.exec = NULL, .result = NULL},
    {.exec = NULL, .result = NULL},
    {.exec = NULL, .result = NULL}
};

u32 pc;
u32 pc_prev;
i32 xreg[32];
u32 rom[ROM_SIZE/4];
u32 ram[RAM_SIZE/4];
u32 regfile[10];
u32 tick = 0;
u32 dead_loop_cnt = 0;
FILE *fp_uart;
FILE *fp_pc; //for rtl debug

void log_tested_print(char *name, struct opcode_group *op, u32 length) {
    u32 i;
    log_info_direct("  Tested %s: ", name);
    for (i = 0; i < length; i++){
        if (op[i].exec != NULL) {
            if(op[i].count != 0)
                log_info_direct("1 ");
            else
                log_info_direct("0 ");
       }
    }
    log_info_direct("\n");
}

void log_tested(){
    u32 i;
    for (i = 0; i < 4; i++)
        if (opcodes_group[i].result != NULL)
            opcodes_group[i].result();
}

void end_sim(){
    u8 i;
    log_info("Stop at tick=%d, pc=0x%x\n",tick,pc);
    fclose(fp_uart);
    fclose(fp_pc);
    log_tested();
    exit(0);
}

void uart_send_byte(char a){
    fprintf(fp_uart,"%c",a);
}

u32 get_bits(u32 val, u8 bit_start, u8 length){
    return (val >> bit_start) & ((1<<length)-1);
}

u32 get_mem(u32 addr){
    if((addr >= ROM_BASE_ADDR) && (addr < ROM_BASE_ADDR + ROM_SIZE)){
        return rom[(addr-ROM_BASE_ADDR)>>2];
    }else if((addr >= RAM_BASE_ADDR) && (addr < RAM_BASE_ADDR + RAM_SIZE)){
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
        if((addr & 0xfffc) == REG_BASE_ADDR){ //uart data register
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

i32 u2i(u32 uval, u8 length){
    i32 ival;
    if(uval >> (length-1)){
        ival = (0xffffffff-(1<<length)+1) | uval;
    }else{
        ival = uval;
    }
    return ival;
}

u32 read2rxm(char *rxm_file, u8 dest_is_ram){
    FILE *fp;
    i32 code;
    u32 rxm_index = 0;
    int ret_items_num;
    fp = fopen(rxm_file,"r");
    if(fp){
        log_info("Read rxm file\n");
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
            log_info("Read rxm file done\n");
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
    fp_pc = fopen("pc.txt","w");
}

void print_reg_to_file(FILE *f){
    int i;
    for(i=0;i<32;i++){
        fprintf(f,"%08x ",xreg[i]);
    }
}

void main(){
    u32 rom_length;
    u32 code;
    u32 i;
    init();
    log_init();
    rom_length = read2rxm("../c/xriscv.rom",TO_ROM);
    read2rxm("../c/xriscv.ram",TO_RAM);
    while(1){
        fprintf(fp_pc,"%x ",pc);
        code = get_mem(pc);
        opcodes_group[code&3].exec(code);

        log_debug_direct("XREG\n");
        for(i=0;i<32;i++){
            log_debug_direct("%08x ",xreg[i]);
            if((i%8) == 7){
                log_debug_direct("\n");
            }
        }
        log_deep_debug_direct("\n");    

        log_deep_debug_direct("RAM_HEAD\n");
        for(i=0;i<32;i++){
            log_deep_debug_direct("%08x ",ram[i]);
            if((i%8) == 7){
                log_deep_debug_direct("\n");
            }
        }
        log_deep_debug_direct("\n");    

        log_deep_debug_direct("RAM_TAIL\n");
        for(i=0;i<32;i++){
            log_deep_debug_direct("%08x ",ram[RAM_SIZE/4-i]);
            if((i%8) == 7){
                log_deep_debug_direct("\n");
            }
        }
        log_deep_debug_direct("\n");

        if(pc_prev == pc){
            dead_loop_cnt += 1;
            if(dead_loop_cnt > 10){
                end_sim();
            }
        }
        pc_prev = pc;

        print_reg_to_file(fp_pc);
        fprintf(fp_pc,"\n");
    }
}
