#include "regfile.h"
#include "xuart.h"

void xuart_putchar(char c){
    while((regfile.xuart.status & XUART_FULL_BIT));
    regfile.xuart.send_byte = c;
}

char xuart_getchar(){
    while((regfile.xuart.status & XUART_EMPTY_BIT));
    return regfile.xuart.rcvd_byte;
}

void xuart_puts(const char *str){
    while(*str){
        xuart_putchar(*str++);
    }
}

void xuart_gets(char *str, u32 length){
    u32 i;
    for(i=0;i<length;i++){
        *str = xuart_getchar();
        str++;
    }
}

