#ifndef __REGFILE__

#include "xglb.h"

struct st_xuart {
    u8 status;      //bit 0: full; bit 1: empty;
    u8 send_byte;
    u8 rcvd_byte;
    u8 cfg;         //0:9600
};

struct st_led_btn {
    u8 led: 4;
    u32 rsvd: 28;
};

struct st_regfile {
    struct st_xuart xuart;
    struct st_led_btn led_btn;
};

extern volatile struct st_regfile regfile;

#else
#define __REGFILE__
#endif
