#include "xuart.h"
#include <stdio.h>

int boot(){
    u8 a;
    u8 b;
    u8 c;
    a = 1;
    b = 2;
    xuart_puts("Hello XRISCV!\n");
    c = a+b;
    printf("%d+%d=%d",a,b,c);

    while(1){}
    return 0;
}
