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
    s[1] = d/1000000000;
    for(i=2;i<11;i++){
        s[i] = (d%md[i-2])/md[i-1];
    }
    xuart_puts(s);
}

void putu(u32 u){
}

void putx(u32 d){
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
        }
    }
    return 0;
}
