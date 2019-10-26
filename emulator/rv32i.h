#ifndef __RV32I_H__
#define __RV32I_H__
#include "glb.h"

#define OP_I_CODE      0x3                              //bit[1:0] = 11b
#define OP_I_LUI      0x37                              //bit[6:0] = 0110111b
#define OP_I_AUIPC    0x17                              //bit[6:0] = 0010111b
#define OP_I_JAL      0x6f                              //bit[6:0] = 1101111b
#define OP_I_JALR     (0x0<<12) | 0x67                  //bit[6:0] = 1100111b, bit[14:12] = 0
#define OP_I_B        0x63
#define OP_I_BEQ      (0x0<<12) | OP_I_B                //bit[6:0] = 1100011b, bit[14:12] = 0
#define OP_I_BNE      (0x1<<12) | OP_I_B                //bit[6:0] = 1100011b, bit[14:12] = 1
#define OP_I_BLT      (0x4<<12) | OP_I_B                //bit[6:0] = 1100011b, bit[14:12] = 4
#define OP_I_BGE      (0x5<<12) | OP_I_B                //bit[6:0] = 1100011b, bit[14:12] = 5
#define OP_I_BLTU     (0x6<<12) | OP_I_B                //bit[6:0] = 1100011b, bit[14:12] = 6
#define OP_I_BGEU     (0x7<<12) | OP_I_B                //bit[6:0] = 1100011b, bit[14:12] = 7
#define OP_I_L        0x03
#define OP_I_LB       (0x0<<12) | OP_I_L               //bit[6:0] = 0000011b, bit[14:12] = 0
#define OP_I_LH       (0x1<<12) | OP_I_L               //bit[6:0] = 0000011b, bit[14:12] = 1
#define OP_I_LW       (0x2<<12) | OP_I_L               //bit[6:0] = 0000011b, bit[14:12] = 2
#define OP_I_LBU      (0x4<<12) | OP_I_L               //bit[6:0] = 0000011b, bit[14:12] = 4
#define OP_I_LHU      (0x5<<12) | OP_I_L               //bit[6:0] = 0000011b, bit[14:12] = 5
#define OP_I_S        0x23
#define OP_I_SB       (0x0<<12) | OP_I_S                //bit[6:0] = 0100011b, bit[14:12] = 0
#define OP_I_SH       (0x1<<12) | OP_I_S                //bit[6:0] = 0100011b, bit[14:12] = 1
#define OP_I_SW       (0x2<<12) | OP_I_S                //bit[6:0] = 0100011b, bit[14:12] = 2
#define OP_I_I        0x13
#define OP_I_ADDI     (0x0<<12) | OP_I_I                //bit[6:0] = 0010011b, bit[14:12] = 0
#define OP_I_SLTI     (0x2<<12) | OP_I_I                //bit[6:0] = 0010011b, bit[14:12] = 2
#define OP_I_SLTIU    (0x3<<12) | OP_I_I                //bit[6:0] = 0010011b, bit[14:12] = 3
#define OP_I_XORI     (0x4<<12) | OP_I_I                //bit[6:0] = 0010011b, bit[14:12] = 4
#define OP_I_ORI      (0x6<<12) | OP_I_I                //bit[6:0] = 0010011b, bit[14:12] = 6
#define OP_I_ANDI     (0x7<<12) | OP_I_I                //bit[6:0] = 0010011b, bit[14:12] = 7
#define OP_I_SLLI     (0x0<<25)|(0x1<<12) | OP_I_I      //bit[6:0] = 0010011b, bit[14:12] = 1, bit[31:25] = 0
#define OP_I_SRLI     (0x0<<25)|(0x5<<12) | OP_I_I      //bit[6:0] = 0010011b, bit[14:12] = 5, bit[31:25] = 0
#define OP_I_SRAI     (0x1<<25)|(0x5<<12) | OP_I_I      //bit[6:0] = 0010011b, bit[14:12] = 5, bit[31:25] = 1
#define OP_I_R        0x33
#define OP_I_ADD      (0x0<<25)|(0x0<<12) | OP_I_R      //bit[6:0] = 0110011b, bit[14:12] = 0, bit[31:25] = 0
#define OP_I_SUB      (0x1<<25)|(0x0<<12) | OP_I_R      //bit[6:0] = 0110011b, bit[14:12] = 0, bit[31:25] = 0
#define OP_I_SLL      (0x0<<25)|(0x1<<12) | OP_I_R      //bit[6:0] = 0110011b, bit[14:12] = 1, bit[31:25] = 0
#define OP_I_SLT      (0x0<<25)|(0x2<<12) | OP_I_R      //bit[6:0] = 0110011b, bit[14:12] = 2, bit[31:25] = 0
#define OP_I_SLTU     (0x0<<25)|(0x3<<12) | OP_I_R      //bit[6:0] = 0110011b, bit[14:12] = 3, bit[31:25] = 0
#define OP_I_XOR      (0x0<<25)|(0x4<<12) | OP_I_R      //bit[6:0] = 0110011b, bit[14:12] = 4, bit[31:25] = 0
#define OP_I_SRL      (0x0<<25)|(0x5<<12) | OP_I_R      //bit[6:0] = 0110011b, bit[14:12] = 5, bit[31:25] = 0
#define OP_I_SRA      (0x1<<25)|(0x5<<12) | OP_I_R      //bit[6:0] = 0110011b, bit[14:12] = 5, bit[31:25] = 1
#define OP_I_OR       (0x0<<25)|(0x6<<12) | OP_I_R      //bit[6:0] = 0110011b, bit[14:12] = 6, bit[31:25] = 0
#define OP_I_AND      (0x0<<25)|(0x7<<12) | OP_I_R      //bit[6:0] = 0110011b, bit[14:12] = 7, bit[31:25] = 0
//#define OP_I_FENCE    (0x0<<12) | 0x0f                  //bit[6:0] = 0001111b, bit[14:12] = 0
//#define OP_I_FENCEI   (0x1<<12) | 0x0f                  //bit[6:0] = 0001111b, bit[14:12] = 1
//#define OP_I_ECALL    (0x0<<12) | OP_I_C                //bit[6:0] = 1110011b, bit[14:12] = 0
//#define OP_I_EBREAK   (0x0<<12) | OP_I_C                //bit[6:0] = 1110011b, bit[14:12] = 0
#define OP_I_C        0x73
#define OP_I_CSRRW    (0x1<<12) | OP_I_C                //bit[6:0] = 1110011b, bit[14:12] = 1
#define OP_I_CSRRS    (0x2<<12) | OP_I_C                //bit[6:0] = 1110011b, bit[14:12] = 2
#define OP_I_CSRRC    (0x3<<12) | OP_I_C                //bit[6:0] = 1110011b, bit[14:12] = 3
#define OP_I_CSRRWI   (0x5<<12) | OP_I_C                //bit[6:0] = 1110011b, bit[14:12] = 5
#define OP_I_CSRRSI   (0x6<<12) | OP_I_C                //bit[6:0] = 1110011b, bit[14:12] = 6
#define OP_I_CSRRCI   (0x7<<12) | OP_I_C                //bit[6:0] = 1110011b, bit[14:12] = 7


#endif //#ifndef __RV32I__