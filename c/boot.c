#include "xuart.h"
#include "glb.h"
#include "test.h"
#include <stdio.h>

int boot(){
    //xuart_puts("Hello XRISCV!\n");
    test();

    while(1){}
    return 0;
}
