#ifndef __MAIN_H__
#define __MAIN_H__

#include "glb.h"


struct opcode_group {
	//u8 group;
	void (*exec)(u32 code);
	void (*result)();
	u32 count;
};

u32 get_mem(u32 addr);
u32 set_mem(u32 addr,u32 val,u8 type);
u32 get_bits(u32 val, u8 bit_start, u8 length);
i32 u2i(u32 uval, u8 length);
void log_tested_print(char *name, struct opcode_group *op, u32 length);

extern u32 pc;
extern u32 tick;
extern i32 xreg[];
extern struct opcode_group opcodes_group[];

#endif