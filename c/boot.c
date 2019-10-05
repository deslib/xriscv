#include "xuart.h"
#include "regfile.h"
#include "glb.h"
#include "test.h"
#include <stdio.h>

int boot(){
    //xuart_puts("Hello XRISCV!\n");
    xuart_putchar('H');
    xuart_putchar('e');
    xuart_putchar('l');
    //test();

    while(1){}
    return 0;
}
