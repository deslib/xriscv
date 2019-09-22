#ifndef __REGFILE__

#include "xglb.h"

struct st_xuart {
    u8 status;      //bit 1: full; bit 2: empty;
    u8 send_byte;
    u8 rcvd_byte;
    u8 cfg;         //0:9600
};

struct st_regfile {
    struct st_xuart xuart;
};

extern volatile struct st_regfile regfile;

#else
#define __REGFILE__
#endif
