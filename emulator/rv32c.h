#ifndef __RV32C_H__
#define __RV32C_H__

#define OP_C_00         0x0                              //bit[1:0] = 00b
#define OP_C_ADDI4SPN (0x0<<13) | OP_C_00                //bit[1:0] = 00b, bit[15:13] = 000b
#define OP_C_LW       (0x2<<13) | OP_C_00                //bit[1:0] = 00b, bit[15:13] = 010b
#define OP_C_SW       (0x6<<13) | OP_C_00                //bit[1:0] = 00b, bit[15:13] = 110b

#define OP_C_01         0x1                              //bit[1:0] = 01b
#define OP_C_NOP      (0x0<<13) | (0x0 << 12) | (0x0 << 12) | (0x0 << 7) | (0x0 << 2) | OP_C_01                
                                                         //bit[1:0] = 01b, bit[6:2] = 0, bit[11:7] = 0, bit[12] = 0, bit[15:13] = 000b
#define OP_C_ADDI     (0x0<<13) | OP_C_01                //bit[1:0] = 01b, bit[11:7] != 0, bit[15:13] = 000b
#define OP_C_JAL      (0x1<<13) | OP_C_01                //bit[1:0] = 01b, bit[15:13] = 001b
#define OP_C_LI       (0x2<<13) | OP_C_01                //bit[1:0] = 01b, bit[11:7] != 0, bit[15:13] = 010b
#define OP_C_ADDI16SP (0x3<<13) | (0x2 << 7) | OP_C_01   //bit[1:0] = 01b, bit[11:7] = 2, bit[15:13] = 011b
#define OP_C_LUI      (0x3<<13) | OP_C_01                //bit[1:0] = 01b, bit[11:7] != {0,2}, bit[15:13] = 011b
#define OP_C_SRLI     (0x4<<13) | (0x0 << 10) | OP_C_01  //bit[1:0] = 01b, bit[11:10] = 00b, bit[15:13] = 100b
#define OP_C_SRAI     (0x4<<13) | (0x1 << 10) | OP_C_01  //bit[1:0] = 01b, bit[11:10] = 01b, bit[15:13] = 100b
#define OP_C_ANDI     (0x4<<13) | (0x2 << 10) | OP_C_01  //bit[1:0] = 01b, bit[11:10] = 10b, bit[15:13] = 100b
#define OP_C_SUB      (0x4<<13) | (0x0 << 12) |  (0x3 << 10) | (0x0 << 5) | OP_C_01  
                                                         //bit[1:0] = 01b, bit[6:5] = 00b, bit[11:10] = 11b, bit[12] = 0, bit[15:13] = 100b
#define OP_C_XOR      (0x4<<13) | (0x0 << 12) |  (0x3 << 10) | (0x1 << 5) | OP_C_01  
                                                         //bit[1:0] = 01b, bit[6:5] = 01b, bit[11:10] = 11b, bit[12] = 0, bit[15:13] = 100b
#define OP_C_OR       (0x4<<13) | (0x0 << 12) |  (0x3 << 10) | (0x2 << 5) | OP_C_01  
                                                         //bit[1:0] = 01b, bit[6:5] = 10b, bit[11:10] = 11b, bit[12] = 0, bit[15:13] = 100b
#define OP_C_AND      (0x4<<13) | (0x0 << 12) |  (0x3 << 10) | (0x3 << 5) | OP_C_01  
                                                         //bit[1:0] = 01b, bit[6:5] = 11b, bit[11:10] = 11b, bit[12] = 0, bit[15:13] = 100b
#define OP_C_J        (0x5<<13) | OP_C_01                //bit[1:0] = 01b, bit[15:13] = 101b
#define OP_C_BEQZ     (0x6<<13) | OP_C_01                //bit[1:0] = 01b, bit[15:13] = 110b
#define OP_C_BNEZ     (0x7<<13) | OP_C_01                //bit[1:0] = 01b, bit[15:13] = 111b

#define OP_C_10         0x2                              //bit[1:0] = 01b
#define OP_C_SLLI     (0x0<<13) | OP_C_10                //bit[1:0] = 10b, bit[11:7] != 0, bit[15:13] = 000b
#define OP_C_LWSP     (0x2<<13) | OP_C_10                //bit[1:0] = 10b, bit[11:7] != 0, bit[15:13] = 010b
#define OP_C_JR       (0x4<<13) | (0x0<<12)  | (0x0 << 2) | OP_C_10   
                                                         //bit[1:0] = 10b, bit[6:2] = 0, bit[11:7] != 0, bit[12] = 0, bit[15:13] = 100b
#define OP_C_MV       (0x4<<13) | (0x0<<12)  | OP_C_10   //bit[1:0] = 10b, bit[6:2] != 0, bit[11:7] != 0, bit[12] = 0, bit[15:13] = 100b
#define OP_C_EBREAK   (0x4<<13) | (0x1<<12)  | (0x0 << 7) | (0x0 <<2) | OP_C_10   
                                                         //bit[1:0] = 10b, bit[6:2] = 0, bit[11:7] = 0, bit[12] = 1, bit[15:13] = 100b
#define OP_C_JALR     (0x4<<13) | (0x1<<12)  | (0x0 <<2) | OP_C_10   
                                                         //bit[1:0] = 10b, bit[6:2] = 0, bit[11:7] != 0, bit[12] = 1, bit[15:13] = 100b
#define OP_C_ADD      (0x4<<13) | (0x1<<12)  | OP_C_10   //bit[1:0] = 10b, bit[6:2] != 0, bit[11:7] != 0, bit[12] = 1, bit[15:13] = 100b
#define OP_C_SWSP     (0x6<<13) | OP_C_10                //bit[1:0] = 10b, bit[15:13] = 110b

#endif