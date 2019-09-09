#include "xuart.h"

struct st_xuart * xuart = XUART_BASE_ADDR;

void xuart_putchar(char c){
    while(xuart->status & XUART_FULL_BIT);
    xuart->send_byte = c;
}

char xuart_getchar(){
    while(xuart->status & XUART_EMPTY_BIT);
    return xuart->rcvd_byte;
}

void xuart_puts(char *str){
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

