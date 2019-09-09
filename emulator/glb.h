#ifndef __GLB__

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef char i8;
typedef short i16;
typedef int i32;
typedef long long int i64;

#define OP_LUI       0xd
#define OP_AUIPC     0x5
#define OP_JAL       0x1b
#define OP_JALR      0x19
#define OP_BRANCH    0x18
#define OP_LOAD      0x0
#define OP_STORE     0x8
#define OP_IMM       0x4
#define OP_REG       0xc

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
