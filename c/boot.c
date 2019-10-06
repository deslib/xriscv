#include "xuart.h"
#include "regfile.h"
#include "glb.h"
#include "test.h"
#include <stdio.h>

int boot(){
    //xuart_putchar('>');
    //xuart_puts("Hello XRISCV!\n");
    test();

    while(1){}
    return 0;
}
