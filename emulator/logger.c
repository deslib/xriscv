#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include "logger.h"

FILE *log_fp;

char new_message[MAX_LOG_SIZE];

void log_init(){
    #ifdef LOG_FILE
    log_fp = fopen(LOG_FILE,"w");
    #endif
}

void log_debug(char *fmt, ...){
    if(LOG_LEVEL >= DEBUG){
        va_list args_list;
        va_start(args_list,fmt);
        log_base("DEBUG",fmt,args_list);
        va_end(args_list);
    }
}

void log_deep_debug(char *fmt, ...){
    if(LOG_LEVEL >= DEEP_DEBUG){
        va_list args_list;
        va_start(args_list,fmt);
        log_base("DDEBUG",fmt,args_list);
        va_end(args_list);
    }
}


void log_info(char *fmt, ...){
    if(LOG_LEVEL >= INFO){
        va_list args_list;
        va_start(args_list,fmt);
        log_base("INFO",fmt,args_list);
        va_end(args_list);
    }
}

void log_info_direct(char *fmt, ...){
    if(LOG_LEVEL >= INFO){
        va_list args_list;
        va_start(args_list,fmt);
        vsnprintf(new_message,MAX_LOG_SIZE,fmt,args_list);
        printf("%s",new_message);
        va_end(args_list);
    }
}

void log_error(char *fmt, ...){
    if(LOG_LEVEL >= ERROR){
        va_list args_list;
        va_start(args_list,fmt);
        log_base("DEBUG",fmt,args_list);
        va_end(args_list);
    }
}

void log_deep_debug_direct(char *fmt,...){
    if(LOG_LEVEL >= DEEP_DEBUG){
        va_list args_list;
        va_start(args_list,fmt);
        vsnprintf(new_message,MAX_LOG_SIZE,fmt,args_list);
        printf("%s",new_message);
        va_end(args_list);
    }
}

void log_debug_direct(char *fmt,...){
    if(LOG_LEVEL >= DEBUG){
        va_list args_list;
        va_start(args_list,fmt);
        vsnprintf(new_message,MAX_LOG_SIZE,fmt,args_list);
        printf("%s",new_message);
        va_end(args_list);
    }
}


void get_cur_time(char *time_str){
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    sprintf(time_str, "%4d-%02d-%02d_%02d:%02d",timeinfo->tm_year+1900,\
            timeinfo->tm_mon,timeinfo->tm_mday,timeinfo->tm_hour,\
            timeinfo->tm_min);
}

void log_base(char *level, char *fmt, va_list args_list){
    char c_time_str[50];
    vsnprintf(new_message,MAX_LOG_SIZE,fmt,args_list);
    get_cur_time(c_time_str);
    printf("[%s][%s] %s",c_time_str,level,new_message);
    #ifdef LOG_FILE
        fprintf(log_fp, "[%s][%s] %s", c_time_str,level,new_message);
    #endif
}
