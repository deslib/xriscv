#ifndef __csr__

extern void csr_init();

typedef union {
struct {
    u32 uie      : 1;    //bit[0], user mode interrupt enable, hardwired to 0 if user mode is not supported
    u32 sie      : 1;    //bit[1], supervisor mode interrupt enable, hardwired to 0 if supervisor mode is not supported
    u32 resv1    : 1;    //bit[2]
    u32 mie      : 1;    //bit[3], machine mode interrupt enable
    u32 upie     : 1;    //bit[4], user mode interrupt enable before trap, hardwired to 0 if user mode is not supported
    u32 spie     : 1;    //bit[5], supervisor mode interrupt enable before trap, hardwired to 0 if supervisor mode is not supported
    u32 resv2    : 1;    //bit[6]
    u32 mpie     : 1;    //bit[7], machine mode interrupt enable before trap
    u32 spp      : 1;    //bit[8], mode before trap in supervios mode, hardwired to 0 if supervisor mode is not supported
    u32 resv3    : 2;    //bit[10:9]
    u32 mpp      : 2;    //bit[12:11], mode bfore trap in machine mode
    u32 fs       : 2;    //bit[14:13], WARL, status of floating point unit, hardwired to 0 if supervisor mode and floating point unit is not supported, 
                         // 0, off
                         // 1, initial
                         // 2, clean
                         // 3, dirty
    u32 xs       : 2;    //bit[16:15], RO, status of additional user mode extention and associated state, hardwired to 0 if no additional user extension requiring new state
                         // 0, all off
                         // 1, none dirty or clean, some on
                         // 2, none dirty, some clean
                         // 3, some dirty
    u32 mprv     : 1;    //bit[17], modify privilege, hardwired to 0 if user mode is not supported
                         // 0, loads and stores behave as normal, using translation and protection mechanisms of the current mode
                         // 1, load and store memory address are translated and protected as though current privilege mode were set to MPP
                         //    instruction adress translation and protectoin are unaffected 
    u32 sum      : 1;    //bit[18], permit supervisor user memory access, hardwired to 0 if supervisor mode is not supported
                         // 0, supervisor mode memory access to page that accessible by user mode will fault
                         // 1, supervisor mode memory access to page that accessible by user mode will ok
    u32 mxr      : 1;    //bit[19], make execuatable readable, hardwired to 0 if supervisor mode is not supported
                         // 0, only loads from pages marked readable will successed
                         // 1, loads from pages marked either readable or executable will successed
    u32 tvm      : 1;    //bit[20], trap virtual memory, hardwired to 0 if supervisor mode is not supported
                         // 0, attempts to read or write the satp CSR or execute SFENCE.VMA while execution in supervios mode will ok
                         // 1, attempts to read or write the satp CSR or execute SFENCE.VMA while execution in supervios mode will fault
    u32 tw       : 1;    //bit[21], timeout wait, , hardwired to 0 if no mode less privilege than machine
                         // 0, wfi instruction are supportted in lower privilege modes
                         // 1, wfi instruction are not supportted in lower privilege modes
    u32 tsr      : 1;    //bit[22], trap sret, hardwired to 0 if supervisor mode is not supported
                         // 0, sret instruction are supportted in supervisor modes
                         // 1, sret instruction are not supportted in supervisor modes
    u32 resv4    : 8;    //bit[30:23]
    u32 sd       : 1;    //bit[31], RO, hardwired to 0 if  both xs and fs are hardwired to 0
                         // 0, FS and XS no dirty
                         // 1, FS or XS is dirty
  } bits;
  u32 data;
} mstatus_reg;
#define mstatus_off   0x300

typedef union {
struct {
    u32 extensions : 26;    //bit[25:0], WARL
                            // A,B,C,D,E,F...X,Y,Z
    u32 resv1      : 4;     //bit[29:26], WLRL
    u32 mxl        : 2;     //bit[31:30], WARL
                            // 1, 32bit
                            // 2, 64bit 
                            // 3, 128bit 
  } bits;
  u32 data;
} misa_reg;
#define misa_off   0x301

typedef union {
struct {
    u32 exception  : 32;    //bit[31:0], WARL
  } bits;
  u32 data;
} medeleg_reg;
#define medeleg_off   0x302

typedef union {
struct {
    u32 interupts : 32;    //bit[31:0], WARL
  } bits;
  u32 data;
} mideleg_reg;
#define mideleg_off   0x303

typedef union {
struct {
    u32 usie      : 1;    //bit[0], read write
    u32 ssie      : 1;    //bit[1], read write
    u32 resv1     : 1;    //bit[2]
    u32 msie      : 1;    //bit[3], read only
    u32 utie      : 1;    //bit[4], read write
    u32 stie      : 1;    //bit[5], read write
    u32 resv2     : 1;    //bit[6]
    u32 mtie      : 1;    //bit[7], read only
    u32 ueie      : 1;    //bit[8], read write
    u32 seie      : 1;    //bit[9], read write
    u32 resv3     : 1;    //bit[10]
    u32 meie      : 1;    //bit[11], read only
    u32 resv4     : 20;   //bit[31:12]
  } bits;
  u32 data;
} mie_reg;
#define mie_off   0x304

typedef union {
struct {
    u32 mode      : 2;    //bit[1:0], WARL
                          // 0, direct, all exceptions set pc to base
                          // 1, vectored, asyschronous interrupts set pc to base+4xcause
                          // >=2, reserved
    u32 base      : 30;   // vector base address bit[31:2], 4 byte align
  } bits;
  u32 data;
} mtvec_reg;
#define mtvec_off   0x305

typedef union {
struct {
    u32 cy        : 1;    //bit[0]
    u32 tm        : 1;    //bit[1]
    u32 ir        : 1;    //bit[2]
    u32 hpm3      : 1;    //bit[3]
    u32 hpm4      : 1;    //bit[4]
    u32 hmp5      : 1;    //bit[5]
    u32 hpm6      : 1;    //bit[6]
    u32 hpm7      : 1;    //bit[7]
    u32 hmp8      : 1;    //bit[8]
    u32 hpm9      : 1;    //bit[9]
    u32 hpm10     : 1;    //bit[10]
    u32 hpm11     : 1;    //bit[11]
    u32 hpm12     : 1;    //bit[12]
    u32 hpm13     : 1;    //bit[13]
    u32 hpm14     : 1;    //bit[14]
    u32 hpm15     : 1;    //bit[15]
    u32 hpm16     : 1;    //bit[16]
    u32 hpm17     : 1;    //bit[17]
    u32 hpm18     : 1;    //bit[18]
    u32 hpm19     : 1;    //bit[19]
    u32 hpm20     : 1;    //bit[20]
    u32 hpm21     : 1;    //bit[21]
    u32 hpm22     : 1;    //bit[22]
    u32 hpm23     : 1;    //bit[23]
    u32 hpm24     : 1;    //bit[24]
    u32 hpm25     : 1;    //bit[25]
    u32 hpm26     : 1;    //bit[26]
    u32 hpm27     : 1;    //bit[27]
    u32 hpm28     : 1;    //bit[28]
    u32 hpm29     : 1;    //bit[29]
    u32 hpm31     : 1;    //bit[31]
  } bits;
  u32 data;
} mcounteren_reg;
#define mcounteren_off   0x306


typedef union {
struct {
    u32 cy       : 1;    //bit[0]
  } bits;
  u32 data;
} mcountinhibit_reg;
#define mcountinhibit_off   0x320

#define mhpmevent3_off   0x323
#define mhpmevent4_off   0x324
#define mhpmevent5_off   0x325
#define mhpmevent6_off   0x326
#define mhpmevent7_off   0x327
#define mhpmevent8_off   0x328
#define mhpmevent9_off   0x329
#define mhpmevent10_off   0x32a
#define mhpmevent11_off   0x32b
#define mhpmevent12_off   0x32c
#define mhpmevent13_off   0x32d
#define mhpmevent14_off   0x32e
#define mhpmevent15_off   0x32f
#define mhpmevent16_off   0x330
#define mhpmevent17_off   0x331
#define mhpmevent18_off   0x332
#define mhpmevent19_off   0x333
#define mhpmevent20_off   0x334
#define mhpmevent21_off   0x335
#define mhpmevent22_off   0x336
#define mhpmevent23_off   0x337
#define mhpmevent24_off   0x338
#define mhpmevent25_off   0x339
#define mhpmevent26_off   0x33a
#define mhpmevent27_off   0x33b
#define mhpmevent28_off   0x33c
#define mhpmevent29_off   0x33d
#define mhpmevent30_off   0x33e
#define mhpmevent31_off   0x33f

typedef union {
  u32 data;
} mscratch_reg;
#define mscratch_off   0x340

typedef union {
  u32 data;
} mepc_reg;
#define mepc_off   0x341

typedef union {
struct {
    u32 code      : 31;  //bit[30:0], WLRL
    u32 interrupt : 1;   //bit[31]
  } bits;
  u32 data;
} mcause_reg;
#define mcause_off   0x342

typedef union {
  u32 data;
} mtval_reg;
#define mtval_off   0x343

typedef union {
struct {
    u32 usip      : 1;    //bit[0], read write
    u32 ssip      : 1;    //bit[1]
    u32 resv1     : 1;    //bit[2], read write
    u32 msip      : 1;    //bit[3], read only
    u32 utip      : 1;    //bit[4], read write
    u32 stip      : 1;    //bit[5], read write
    u32 resv2     : 1;    //bit[6]
    u32 mtip      : 1;    //bit[7]
    u32 ueip      : 1;    //bit[8], read write
    u32 seip      : 1;    //bit[9], read write
    u32 resv3     : 1;    //bit[10]
    u32 meip      : 1;    //bit[11], read only
    u32 resv4     : 20;   //bit[31:12]
  } bits;
  u32 data;
} mip_reg;
#define mip_off   0x344

typedef union {
struct {
    u32 pmpcfg0   : 8;    //bit[7:0]
    u32 pmpcfg1   : 8;    //bit[15:7]
    u32 pmpcfg2   : 8;    //bit[23:16]
    u32 pmpcfg3   : 8;    //bit[31:24]
  } bits;
  u32 data;
} pmpcfg0_reg;
#define pmpcfg0_off   0x3a0

typedef union {
struct {
    u32 pmpcfg4   : 8;    //bit[7:0]
    u32 pmpcfg5   : 8;    //bit[15:7]
    u32 pmpcfg6   : 8;    //bit[23:16]
    u32 pmpcfg7   : 8;    //bit[31:24]
  } bits;
  u32 data;
} pmpcfg1_reg;
#define pmpcfg1_off   0x3a1

typedef union {
struct {
    u32 pmpcfg8   : 8;    //bit[7:0]
    u32 pmpcfg9   : 8;    //bit[15:7]
    u32 pmpcfg10  : 8;    //bit[23:16]
    u32 pmpcfg11  : 8;    //bit[31:24]
  } bits;
  u32 data;
} pmpcfg2_reg;
#define pmpcfg2_off   0x3a2

typedef union {
struct {
    u32 pmpcfg12  : 8;    //bit[7:0]
    u32 pmpcfg13  : 8;    //bit[15:7]
    u32 pmpcfg14  : 8;    //bit[23:16]
    u32 pmpcfg15  : 8;    //bit[31:24]
  } bits;
  u32 data;
} pmpcfg3_reg;
#define pmpcfg3_off   0x3a3

typedef union {
  u32 data;
} pmpaddr0_reg;
#define pmpaddr0_off   0x3b0

typedef union {
  u32 data;
} pmpaddr1_reg;
#define pmpaddr1_off   0x3b1

typedef union {
  u32 data;
} pmpaddr2_reg;
#define pmpaddr2_off   0x3b2

typedef union {
  u32 data;
} pmpaddr3_reg;
#define pmpaddr3_off   0x3b3

typedef union {
  u32 data;
} pmpaddr4_reg;
#define pmpaddr4_off   0x3b4

typedef union {
  u32 data;
} pmpaddr5_reg;
#define pmpaddr5_off   0x3b5

typedef union {
  u32 data;
} pmpaddr6_reg;
#define pmpaddr6_off   0x3b6

typedef union {
  u32 data;
} pmpaddr7_reg;
#define pmpaddr7_off   0x3b7

typedef union {
  u32 data;
} pmpaddr8_reg;
#define pmpaddr8_off   0x3b8

typedef union {
  u32 data;
} pmpaddr9_reg;
#define pmpaddr9_off   0x3b9

typedef union {
  u32 data;
} pmpaddr10_reg;
#define pmpaddr10_off   0x3ba

typedef union {
  u32 data;
} pmpaddr11_reg;
#define pmpaddr11_off   0x3bb

typedef union {
  u32 data;
} pmpaddr12_reg;
#define pmpaddr12_off   0x3bc

typedef union {
  u32 data;
} pmpaddr13_reg;
#define pmpaddr13_off   0x3bd

typedef union {
  u32 data;
} pmpaddr14_reg;
#define pmpaddr14_off   0x3be

typedef union {
  u32 data;
} pmpaddr15_reg;
#define pmpaddr15_off   0x3bf

typedef union {
  u32 data;
} mcycle_reg;
#define mcycle_off   0xb00

typedef union {
  u32 data;
} minstret_reg;
#define minstret_off   0xb02

typedef union {
  u32 data;
} mhpmcounter3_reg;
#define mhpmcounter3_off   0xb03

typedef union {
  u32 data;
} mhpmcounter4_reg;
#define mhpmcounter4_off   0xb04

typedef union {
  u32 data;
} mhpmcounter5_reg;
#define mhpmcounter5_off   0xb05

typedef union {
  u32 data;
} mhpmcounter6_reg;
#define mhpmcounter6_off   0xb06

typedef union {
  u32 data;
} mhpmcounter7_reg;
#define mhpmcounter7_off   0xb07

typedef union {
  u32 data;
} mhpmcounter8_reg;
#define mhpmcounter8_off   0xb08

typedef union {
  u32 data;
} mhpmcounter9_reg;
#define mhpmcounter9_off   0xb09

typedef union {
  u32 data;
} mhpmcounter10_reg;
#define mhpmcounter10_off   0xb0a

typedef union {
  u32 data;
} mhpmcounter11_reg;
#define mhpmcounter11_off   0xb0b

typedef union {
  u32 data;
} mhpmcounter12_reg;
#define mhpmcounter12_off   0xb0c

typedef union {
  u32 data;
} mhpmcounter13_reg;
#define mhpmcounter13_off   0xb0d

typedef union {
  u32 data;
} mhpmcounter14_reg;
#define mhpmcounter14_off   0xb0e

typedef union {
  u32 data;
} mhpmcounter15_reg;
#define mhpmcounter15_off   0xb0f

typedef union {
  u32 data;
} mhpmcounter16_reg;
#define mhpmcounter16_off   0xb10

typedef union {
  u32 data;
} mhpmcounter17_reg;
#define mhpmcounter17_off   0xb11

typedef union {
  u32 data;
} mhpmcounter18_reg;
#define mhpmcounter18_off   0xb12

typedef union {
  u32 data;
} mhpmcounter19_reg;
#define mhpmcounter19_off   0xb13

typedef union {
  u32 data;
} mhpmcounter20_reg;
#define mhpmcounter20_off   0xb14

typedef union {
  u32 data;
} mhpmcounter21_reg;
#define mhpmcounter21_off   0xb15

typedef union {
  u32 data;
} mhpmcounter22_reg;
#define mhpmcounter22_off   0xb16

typedef union {
  u32 data;
} mhpmcounter23_reg;
#define mhpmcounter23_off   0xb17

typedef union {
  u32 data;
} mhpmcounter24_reg;
#define mhpmcounter24_off   0xb18

typedef union {
  u32 data;
} mhpmcounter25_reg;
#define mhpmcounter25_off   0xb19

typedef union {
  u32 data;
} mhpmcounter26_reg;
#define mhpmcounter26_off   0xb1a

typedef union {
  u32 data;
} mhpmcounter27_reg;
#define mhpmcounter27_off   0xb1b

typedef union {
  u32 data;
} mhpmcounter28_reg;
#define mhpmcounter28_off   0xb1c

typedef union {
  u32 data;
} mhpmcounter29_reg;
#define mhpmcounter29_off   0xb1d

typedef union {
  u32 data;
} mhpmcounter30_reg;
#define mhpmcounter30_off   0xb1e

typedef union {
  u32 data;
} mhpmcounter31_reg;
#define mhpmcounter31_off   0xb1f

typedef union {
  u32 data;
} mcycleh_reg;
#define mcycleh_off   0xb80

typedef union {
  u32 data;
} minstreth_reg;
#define minstreth_off   0xb82

typedef union {
  u32 data;
} mhpmcounter3h_reg;
#define mhpmcounter3h_off   0xb83

typedef union {
  u32 data;
} mhpmcounter4h_reg;
#define mhpmcounter4h_off   0xb84

typedef union {
  u32 data;
} mhpmcounter5h_reg;
#define mhpmcounter5h_off   0xb85

typedef union {
  u32 data;
} mhpmcounter6h_reg;
#define mhpmcounter6h_off   0xb86

typedef union {
  u32 data;
} mhpmcounter7h_reg;
#define mhpmcounter7h_off   0xb87

typedef union {
  u32 data;
} mhpmcounter8h_reg;
#define mhpmcounter8h_off   0xb88

typedef union {
  u32 data;
} mhpmcounter9h_reg;
#define mhpmcounter9h_off   0xb89

typedef union {
  u32 data;
} mhpmcounter10h_reg;
#define mhpmcounter10h_off   0xb8a

typedef union {
  u32 data;
} mhpmcounter11h_reg;
#define mhpmcounter11h_off   0xb8b

typedef union {
  u32 data;
} mhpmcounter12h_reg;
#define mhpmcounter12h_off   0xb8c

typedef union {
  u32 data;
} mhpmcounter13h_reg;
#define mhpmcounter13h_off   0xb8d

typedef union {
  u32 data;
} mhpmcounter14h_reg;
#define mhpmcounter14h_off   0xb8e

typedef union {
  u32 data;
} mhpmcounter15h_reg;
#define mhpmcounter15h_off   0xb8f

typedef union {
  u32 data;
} mhpmcounter16h_reg;
#define mhpmcounter16h_off   0xb90

typedef union {
  u32 data;
} mhpmcounter17h_reg;
#define mhpmcounter17h_off   0xb91

typedef union {
  u32 data;
} mhpmcounter18h_reg;
#define mhpmcounter18h_off   0xb92

typedef union {
  u32 data;
} mhpmcounter19h_reg;
#define mhpmcounter19h_off   0xb93

typedef union {
  u32 data;
} mhpmcounter20h_reg;
#define mhpmcounter20h_off   0xb94

typedef union {
  u32 data;
} mhpmcounter21h_reg;
#define mhpmcounter21h_off   0xb95

typedef union {
  u32 data;
} mhpmcounter22h_reg;
#define mhpmcounter22h_off   0xb96

typedef union {
  u32 data;
} mhpmcounter23h_reg;
#define mhpmcounter23h_off   0xb97

typedef union {
  u32 data;
} mhpmcounter24h_reg;
#define mhpmcounter24h_off   0xb98

typedef union {
  u32 data;
} mhpmcounter25h_reg;
#define mhpmcounter25h_off   0xb99

typedef union {
  u32 data;
} mhpmcounter26h_reg;
#define mhpmcounter26h_off   0xb9a

typedef union {
  u32 data;
} mhpmcounter27h_reg;
#define mhpmcounter27h_off   0xb9b

typedef union {
  u32 data;
} mhpmcounter28h_reg;
#define mhpmcounter28h_off   0xb9c

typedef union {
  u32 data;
} mhpmcounter29h_reg;
#define mhpmcounter29h_off   0xb9d

typedef union {
  u32 data;
} mhpmcounter30h_reg;
#define mhpmcounter30h_off   0xb9e

typedef union {
  u32 data;
} mhpmcounter31h_reg;
#define mhpmcounter31h_off   0xb9f

typedef union {
struct {
    u32 offset     : 7;    //bit[6:0], RO
    u32 bank       : 25;   //bit[31:7], RO
  } bits;
  u32 data;
} mvendorid_reg;
#define mvendorid_off   0xf11

typedef union {
struct {
    u32 archid     : 32;    //bit[31:0], RO
  } bits;
  u32 data;
} marchid_reg;
#define marchid_off   0xf12

typedef union {
struct {
    u32 impl      : 32;    //bit[31:0], RO
  } bits;
  u32 data;
} mimpid_reg;
#define mimpid_off   0xf13

typedef union {
struct {
    u32 hartid   : 32;    //bit[31:0], RO
  } bits;
  u32 data;
} mhartid_reg;
#define mhartid_off   0xf14

#endif