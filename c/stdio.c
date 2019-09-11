#include "glb.h"
#include "xuart.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

int puts(const char * s){
    xuart_puts(s);
    return 0;
}

void putd(i32 d){
    char s[11];
    int i;
    int md[10] = {1000000000,100000000,10000000,1000000,100000,10000,1000,100,10,1};
    if(d<0){
        s[0] = '-';
    }else{
        s[0] = ' ';
    }
    s[1] = d/1000000000 + 48;
    for(i=2;i<11;i++){
        s[i] = (d%md[i-2])/md[i-1] + 48;
    }
    xuart_puts(s);
}

void putu(u32 u){
    char s[10];
    int i;
    int md[10] = {1000000000,100000000,10000000,1000000,100000,10000,1000,100,10,1};
    s[0] = u/1000000000 + 48;
    for(i=1;i<10;i++){
        s[i] = (u%md[i-1])/md[i] + 48;
    }
    xuart_puts(s);
}

void put_char(char c){
    xuart_putchar(c);
}

void putx(u32 x){
    char s[8];
    int i;
    char four_bits;
    for(i=0;i<8;i++){
        four_bits = (x>>(4*(7-i)))&0xf;
        if(four_bits > 9){
            four_bits += 87;
        }else{
            four_bits += 48;
        }
        s[i] = four_bits;
    }
    xuart_puts(s);
}

int printf(const char *fmt, ...){
    va_list ap;
    for(va_start(ap,fmt);*fmt;fmt++){
        if(*fmt == '%'){
            fmt++;
            if(*fmt == 's')     
                puts(va_arg(ap,char*));
            else if(*fmt == 'x')
                putx(va_arg(ap,u32));
            else if(*fmt == 'u')
                putu(va_arg(ap,u32));
            else if(*fmt == 'd')
                putd(va_arg(ap,i32));
            else put_char(*fmt);
        }else{
            put_char(*fmt);
        }
    }
    return 0;
}
