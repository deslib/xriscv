#include "xglb.h"

#ifndef __XUART__

extern volatile struct st_xuart * xuart;

void xuart_putchar(char c);
char xuart_getchar();
void xuart_puts(const char *str);
void xuart_gets(char *str, u32 length);

#define __XUART__
#endif
