typedef union {
  struct {
    u32 resv1             :  3;
    u32 mie               :  1;     // machine mode interrupt enable
    u32 resv2             :  3;
    u32 mpie              :  1;     // machine mode interrupt enable before trap
    u32 spp               :  1;     // mode before trap in supervios mode, hardwired to 0 if supervisor mode is not supported
    u32 resv3             :  2;
    u32 mpp               :  2;     // mode bfore trap in machine mode
    u32 resv4             : 19;
  } bits;
  u32 data;
} mstatus_reg;
#define mstatus_offset              0x300
#define mstatus_default             0x00001800

typedef union {
  struct {
    u32 extensions        : 26;
    u32 resv1             :  4;
    u32 mxl               :  2;
  } bits;
  u32 data;
} misa_reg;
#define misa_offset                 0x301
#define misa_default                0x40000200

typedef union {
  struct {
    u32 resv1             :  3;
    u32 msie              :  1;     // machine software interrupt enable
    u32 resv2             :  3;
    u32 mtie              :  1;     // machine timer interrupt enable
    u32 resv3             :  3;
    u32 meie              :  1;     // machine external interrupt enable
    u32 resv4             : 20;
  } bits;
  u32 data;
} mie_reg;
#define mie_offset                  0x304
#define mie_default                 0x0

typedef union {
  struct {
    u32 mode              :  2;     // asyschronous interrupts set pc to base+4xcause
    u32 base              : 30;     // vector base address
  } bits;
  u32 data;
} mtvec_reg;
#define mtvec_offset                0x305
#define mtvec_default               0x0

typedef union {
  struct {
    u32 addr              : 32;     // asyschronous interrupts set pc to base+4xcause
  } bits;
  u32 data;
} mepc_reg;
#define mepc_offset                 0x341
#define mepc_default                0x0

typedef union {
  struct {
    u32 addr              :  5;     // asyschronous interrupts set pc to base+4xcause
    u32 resv1             : 26;
    u32 interrupt         :  1;
  } bits;
  u32 data;
} mcause_reg;
#define mcause_offset               0x342
#define mcause_default              0x0

typedef union {
  struct {
    u32 value             : 32;
  } bits;
  u32 data;
} mtval_reg;
#define mtval_offset                0x343
#define mtval_default               0x0

typedef union {
  struct {
    u32 resv1             :  3;
    u32 msip              :  1;     // machine software interrupt pending
    u32 resv2             :  3;
    u32 mtip              :  1;     // machine timer interrupt pending
    u32 resv3             :  3;
    u32 meip              :  1;     // machine external interrupt pending
    u32 resv4             : 20;
  } bits;
  u32 data;
} mip_reg;
#define mip_offset                  0x344
#define mip_default                 0x0

typedef union {
  struct {
    u8 read              :  1;
    u8 write             :  1;
    u8 exec              :  1;
    u8 mode              :  2;
    u8 resv1             :  2;
    u8 lock              :  1;
  } bits;
  u8 data;
} pmpcfg_reg;

typedef union {
  struct {
    u32 pmp0cfg           :  8;
    u32 pmp1cfg           :  8;
    u32 pmp2cfg           :  8;
    u32 pmp3cfg           :  8;
  } bits;
  u32 data;
} pmpcfg0_reg;
#define pmpcfg0_offset              0x3a0
#define pmpcfg0_default             0x0

typedef union {
  struct {
    u32 pmp4cfg           :  8;
    u32 pmp5cfg           :  8;
    u32 pmp6cfg           :  8;
    u32 pmp7cfg           :  8;
  } bits;
  u32 data;
} pmpcfg1_reg;
#define pmpcfg1_offset              0x3a1
#define pmpcfg1_default             0x0

typedef union {
  struct {
    u32 pmp8cfg           :  8;
    u32 pmp9cfg           :  8;
    u32 pmp10cfg          :  8;
    u32 pmp11cfg          :  8;
  } bits;
  u32 data;
} pmpcfg2_reg;
#define pmpcfg2_offset              0x3a2
#define pmpcfg2_default             0x0

typedef union {
  struct {
    u32 pmp12cfg          :  8;
    u32 pmp13cfg          :  8;
    u32 pmp14cfg          :  8;
    u32 pmp15cfg          :  8;
  } bits;
  u32 data;
} pmpcfg3_reg;
#define pmpcfg3_offset              0x3a3
#define pmpcfg3_default             0x0

typedef union {
  struct {
    u32 resv1             :  2;
    u32 address           : 30;
  } bits;
  u32 data;
} pmpaddr0_reg;
#define pmpaddr0_offset             0x3b0
#define pmpaddr0_default            0x0

typedef union {
  struct {
    u32 resv1             :  2;
    u32 address           : 30;
  } bits;
  u32 data;
} pmpaddr1_reg;
#define pmpaddr1_offset             0x3b1
#define pmpaddr1_default            0x0

typedef union {
  struct {
    u32 resv1             :  2;
    u32 address           : 30;
  } bits;
  u32 data;
} pmpaddr2_reg;
#define pmpaddr2_offset             0x3b2
#define pmpaddr2_default            0x0

typedef union {
  struct {
    u32 resv1             :  2;
    u32 address           : 30;
  } bits;
  u32 data;
} pmpaddr3_reg;
#define pmpaddr3_offset             0x3b3
#define pmpaddr3_default            0x0

typedef union {
  struct {
    u32 resv1             :  2;
    u32 address           : 30;
  } bits;
  u32 data;
} pmpaddr4_reg;
#define pmpaddr4_offset             0x3b4
#define pmpaddr4_default            0x0

typedef union {
  struct {
    u32 resv1             :  2;
    u32 address           : 30;
  } bits;
  u32 data;
} pmpaddr5_reg;
#define pmpaddr5_offset             0x3b5
#define pmpaddr5_default            0x0

typedef union {
  struct {
    u32 resv1             :  2;
    u32 address           : 30;
  } bits;
  u32 data;
} pmpaddr6_reg;
#define pmpaddr6_offset             0x3b6
#define pmpaddr6_default            0x0

typedef union {
  struct {
    u32 resv1             :  2;
    u32 address           : 30;
  } bits;
  u32 data;
} pmpaddr7_reg;
#define pmpaddr7_offset             0x3b7
#define pmpaddr7_default            0x0

typedef union {
  struct {
    u32 resv1             :  2;
    u32 address           : 30;
  } bits;
  u32 data;
} pmpaddr8_reg;
#define pmpaddr8_offset             0x3b8
#define pmpaddr8_default            0x0

typedef union {
  struct {
    u32 resv1             :  2;
    u32 address           : 30;
  } bits;
  u32 data;
} pmpaddr9_reg;
#define pmpaddr9_offset             0x3b9
#define pmpaddr9_default            0x0

typedef union {
  struct {
    u32 resv1             :  2;
    u32 address           : 30;
  } bits;
  u32 data;
} pmpaddr10_reg;
#define pmpaddr10_offset            0x3ba
#define pmpaddr10_default           0x0

typedef union {
  struct {
    u32 resv1             :  2;
    u32 address           : 30;
  } bits;
  u32 data;
} pmpaddr11_reg;
#define pmpaddr11_offset            0x3bb
#define pmpaddr11_default           0x0

typedef union {
  struct {
    u32 resv1             :  2;
    u32 address           : 30;
  } bits;
  u32 data;
} pmpaddr12_reg;
#define pmpaddr12_offset            0x3bc
#define pmpaddr12_default           0x0

typedef union {
  struct {
    u32 resv1             :  2;
    u32 address           : 30;
  } bits;
  u32 data;
} pmpaddr13_reg;
#define pmpaddr13_offset            0x3bd
#define pmpaddr13_default           0x0

typedef union {
  struct {
    u32 resv1             :  2;
    u32 address           : 30;
  } bits;
  u32 data;
} pmpaddr14_reg;
#define pmpaddr14_offset            0x3be
#define pmpaddr14_default           0x0

typedef union {
  struct {
    u32 resv1             :  2;
    u32 address           : 30;
  } bits;
  u32 data;
} pmpaddr15_reg;
#define pmpaddr15_offset            0x3bf
#define pmpaddr15_default           0x0

typedef union {
  struct {
    u32 offset            :  7;
    u32 bank              : 25;
  } bits;
  u32 data;
} mvendorid_reg;
#define mvendorid_offset            0xf11
#define mvendorid_default           0x0

typedef union {
  struct {
    u32 id                : 32;
  } bits;
  u32 data;
} marchid_reg;
#define marchid_offset              0xf12
#define marchid_default             0x0

typedef union {
  struct {
    u32 id                : 32;
  } bits;
  u32 data;
} mimpid_reg;
#define mimpid_offset               0xf13
#define mimpid_default              0x0

typedef union {
  struct {
    u32 id                : 32;
  } bits;
  u32 data;
} mhartid_reg;
#define mhartid_offset              0xf14
#define mhartid_default             0x0


#define csr_reg_m_reset(csr) \
          csr[mstatus_offset] =  mstatus_default; \
          csr[misa_offset] =  misa_default; \
          csr[mie_offset] =  mie_default; \
          csr[mtvec_offset] =  mtvec_default; \
          csr[mepc_offset] =  mepc_default; \
          csr[mcause_offset] =  mcause_default; \
          csr[mtval_offset] =  mtval_default; \
          csr[mip_offset] =  mip_default; \
          csr[pmpcfg0_offset] =  pmpcfg0_default; \
          csr[pmpcfg1_offset] =  pmpcfg1_default; \
          csr[pmpcfg2_offset] =  pmpcfg2_default; \
          csr[pmpcfg3_offset] =  pmpcfg3_default; \
          csr[pmpaddr0_offset] =  pmpaddr0_default; \
          csr[pmpaddr1_offset] =  pmpaddr1_default; \
          csr[pmpaddr2_offset] =  pmpaddr2_default; \
          csr[pmpaddr3_offset] =  pmpaddr3_default; \
          csr[pmpaddr4_offset] =  pmpaddr4_default; \
          csr[pmpaddr5_offset] =  pmpaddr5_default; \
          csr[pmpaddr6_offset] =  pmpaddr6_default; \
          csr[pmpaddr7_offset] =  pmpaddr7_default; \
          csr[pmpaddr8_offset] =  pmpaddr8_default; \
          csr[pmpaddr9_offset] =  pmpaddr9_default; \
          csr[pmpaddr10_offset] =  pmpaddr10_default; \
          csr[pmpaddr11_offset] =  pmpaddr11_default; \
          csr[pmpaddr12_offset] =  pmpaddr12_default; \
          csr[pmpaddr13_offset] =  pmpaddr13_default; \
          csr[pmpaddr14_offset] =  pmpaddr14_default; \
          csr[pmpaddr15_offset] =  pmpaddr15_default; \
          csr[mvendorid_offset] =  mvendorid_default; \
          csr[marchid_offset] =  marchid_default; \
          csr[mimpid_offset] =  mimpid_default; \
          csr[mhartid_offset] =  mhartid_default; \

