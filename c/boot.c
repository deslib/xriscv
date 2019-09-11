#include "xuart.h"
#include <stdio.h>

int boot(){
    u8 a;
    u8 b;
    u8 c;
    a = 4;
    b = 8;
    xuart_puts("Hello XRISCV!\n");
    c = a+b;
    printf("%d+%d=%d\n",a,b,c);
    printf("%x+%x=%x\n",(u32)a,(u32)b,(u32)c);
    printf("%u+%u=%u\n",a,b,c);

    while(1){}
    return 0;
}
