#ifndef __GLB__

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef char i8;
typedef short i16;
typedef int i32;
typedef long long int i64;

#define bit0  1<<0
#define bit1  1<<1
#define bit2  1<<2
#define bit3  1<<3
#define bit4  1<<4
#define bit5  1<<5
#define bit6  1<<6
#define bit7  1<<7
#define bit8  1<<8
#define bit9  1<<9
#define bit10  1<<10
#define bit11  1<<11
#define bit12  1<<12
#define bit13  1<<13
#define bit14  1<<14
#define bit15  1<<15
#define bit16  1<<16
#define bit17  1<<17
#define bit18  1<<18
#define bit19  1<<19
#define bit20  1<<20
#define bit21  1<<21
#define bit22  1<<22
#define bit23  1<<23
#define bit24  1<<24
#define bit25  1<<25
#define bit26  1<<26
#define bit27  1<<27
#define bit28  1<<28
#define bit29  1<<29
#define bit30  1<<30
#define bit31  1<<31

#define TO_ROM 0
#define TO_RAM 1

#define DBYTE 0
#define DHALF 1
#define DWORD 2

#define ROM_BASE_ADDR   0
#define ROM_SIZE        0x0
#define RAM_BASE_ADDR   0x0
#define RAM_SIZE        0x8000
#define REG_BASE_ADDR   0x8000
#define REG_SIZE        0x1000

// opcode >> 2
#define OP_LUI       0xd
#define OP_AUIPC     0x5
#define OP_JAL       0x1b
#define OP_JALR      0x19
#define OP_BRANCH    0x18
#define OP_LOAD      0x0
#define OP_STORE     0x8
#define OP_IMM       0x4
#define OP_REG       0xc
#define OP_E_CSR     0x1c

#define ALU_AOS      0x0
#define ALU_SLL      0x1
#define ALU_SLT      0x2
#define ALU_SLTU     0x3
#define ALU_XOR      0x4
#define ALU_SR       0x5
#define ALU_OR       0x6
#define ALU_AND      0x7

#define PC_INIT      0


#else
    #define __GLB__
#endif
