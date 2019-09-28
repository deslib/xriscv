#ifndef __LOGGER__

#define ERROR 1
#define INFO  5
#define DEBUG 10
#define DEEP_DEBUG 100
#define MAX_LOG_SIZE  1024

//#define LOG_LEVEL DEEP_DEBUG 
//#define LOG_LEVEL DEBUG 
#define LOG_LEVEL INFO 


void log_init();
void log_debug(char *fmt, ...);
void log_deep_debug(char *fmt, ...);
void log_info(char *fmt, ...);
void log_info_direct(char *fmt, ...);
void log_error(char *fmt, ...);
void log_base(char *level, char *fmt, va_list args_list);
void log_debug_direct(char *fmt,...);
void log_deep_debug_direct(char *fmt,...);

#else
#define __LOGGER__
#endif
