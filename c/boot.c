#include "xuart.h"

int boot(){
    asm (
        "lui a2,0x2\n\t"
    );
    xuart_puts("Hello XRISCV!");
    while(1){}
    return 0;
}
