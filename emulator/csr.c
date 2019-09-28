
#include "glb.h"
#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include "utils.h"
#include "csr.h"

i32 csr[4096];

void csr_init()
{
    u32 i;
    mvendorid_reg     *mvendorid;
    marchid_reg       *marchid;
    mimpid_reg        *mimpid;
    mhartid_reg       *mhartid;
    mstatus_reg       *mstatus;
    misa_reg          *misa;
    mtvec_reg         *mtvec;
    mip_reg           *mip;
    mie_reg           *mie;

    //zero the range
    for (i=0; i<4096; i++){
        csr[i] = 0x00000000;
    }

    //machine vendor id register
    mvendorid = (mvendorid_reg *)(u8 *)&csr[mvendorid_off];
    mvendorid->bits.offset = 0;    // no implement
    mvendorid->bits.bank = 0;      // no implement 

    //machine arch id register
    marchid = (marchid_reg *)(u8 *)&csr[marchid_off];
    marchid->bits.archid = 0;     // no implement

    //machine impliment id register
    mimpid = (mimpid_reg *)(u8 *)&csr[mimpid_off];
    mimpid->bits.impl = 0;       // no implement

    //machine hart id register
    mhartid = (mhartid_reg *)(u8 *)&csr[mhartid_off];
    mhartid->bits.hartid = 0;   // hart 0

    //machine status register
    mstatus = (mstatus_reg *)(u8 *)&csr[mstatus_off];
    mstatus->bits.mie = 0;      // disable machine mode interrupt 
    mstatus->bits.mpie = 0;     // clean machine mode previous interrupt
    mstatus->bits.mpp = 0;      // clean mode before trap in machine mode
    //supervisor mode, hardwired to 0
    mstatus->bits.sie = 0;
    mstatus->bits.spie = 0;
    mstatus->bits.spp = 0;
    mstatus->bits.mxr = 0;
    mstatus->bits.tsr = 0;
    mstatus->bits.tvm = 0;
    mstatus->bits.sum = 0;
    //usr mode, hardwired to 0 
    mstatus->bits.uie = 0;      // disable user mode interrupt 
    mstatus->bits.upie = 0;     // clean user mode previous interrupt 
    mstatus->bits.mprv = 0;     // hardwired to 0 if user mode is not supported
    mstatus->bits.tw = 0;       // hardwired to 0 if no mode less privilege than machine
    //extension context status, hardwired to 0
    mstatus->bits.fs = 0;
    mstatus->bits.xs = 0;
    mstatus->bits.sd = 0;

    //machine isa register
    misa = (misa_reg *)(u8 *)&csr[misa_off];
    misa->bits.mxl = 1;             //32bit
    misa->bits.resv1 = 0;
    misa->bits.extensions |= bit8;  //rv32i

    //machine trap-vector base-address register
    mtvec = (mtvec_reg *)(u8 *)&csr[mtvec_off];
    mtvec->bits.mode = 1;          //direct mode
    mtvec->bits.base = 0;          //vector base address, TODO

    //machine interrupt enable register
    mie = (mie_reg *)(u8 *)&csr[mie_off];
    //hardwired to 0
    mie->bits.usie = 0;
    mie->bits.ssie = 0;
    mie->bits.utie = 0;
    mie->bits.stie = 0;
    mie->bits.ueie = 0;
    mie->bits.seie = 0;
    //enable machine mode interrupt
    mie->bits.msie = 1;
    mie->bits.mtie = 1;
    mie->bits.meie = 1;

    //machine interrupt pending register
    mip = (mip_reg *)(u8 *)&csr[mip_off];
    //hardwired to 0
    mip->bits.usip = 0;
    mip->bits.ssip = 0;
    mip->bits.utip = 0;
    mip->bits.stip = 0;
    mip->bits.ueip = 0;
    mip->bits.seip = 0;
    //clean machine mode interrupt pending status
    mip->bits.msip = 0;
    mip->bits.mtip = 0;
    mip->bits.meip = 0;

}
