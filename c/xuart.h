#include "xglb.h"

#ifndef __XUART__

struct st_xuart {
    u8 cfg;         //0:9600
    u8 send_byte;
    u8 rcvd_byte;
    u8 status;
};

extern struct st_xuart * xuart;

void xuart_putchar(char c);
char xuart_getchar();
void xuart_puts(char *str);
void xuart_gets(char *str, u32 length);

#define __XUART__
#endif
