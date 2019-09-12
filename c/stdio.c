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
    char s[12] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
    int i;
    int s_i = 0;
    int md[10] = {1000000000,100000000,10000000,1000000,100000,10000,1000,100,10,1};
    unsigned char headed = 0;
    if(d<0){
        s[s_i] = '-';
        s_i += 1;
        d = -d;
    }
    s[s_i] = d/1000000000 + 48;
    d = d%1000000000;
    if(s[s_i] != 48) {
        s_i += 1;
        headed = 1;
    }
    for(i=2;i<11;i++){
        s[s_i] = (d%md[i-2])/md[i-1] + 48;
        d = d%md[i-1];
        if((s[s_i]!=48)|headed){
            s_i += 1;
            headed = 1;
        }
    }
    xuart_puts(s);
}

void putu(u32 u){
    char s[11] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
    int i;
    u32 md[10] = {1000000000,100000000,10000000,1000000,100000,10000,1000,100,10,1};
    int s_i = 0;
    char char0 = 48;
    u8 headed = 0;
    s[s_i] = u/md[0]+ char0;
    u = u%100000000;
    if(s[s_i] != char0){
        headed = 1;
        s_i += 1;
    }
    for(i=1;i<10;i++){
        s[s_i] = (u%md[i-1])/md[i] + 48;
        u = u%md[i];
        if((s[s_i] != char0)|headed){
            s_i += 1;
            headed = 1;
        }
    }
    xuart_puts(s);
}

int putchar(int c){
    xuart_putchar((char)c);
    return 0;
}

void putx(u32 x){
    char s[9] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0'};
    int i;
    char four_bits;
    int s_i = 0;
    u8 headed = 0;
    for(i=0;i<8;i++){
        four_bits = (x>>(4*(7-i)))&0xf;
        if(four_bits > 9){
            four_bits += 87;
        }else{
            four_bits += 48;
        }
        s[s_i] = four_bits;
        if((four_bits!=48)|headed){
            s_i += 1;
            headed = 1;
        }
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
            else putchar(*fmt);
        }else{
            putchar(*fmt);
        }
    }
    return 0;
}
