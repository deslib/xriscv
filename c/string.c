#include <string.h>


void * memcpy (void *dest, const void *src, size_t len){
    size_t i;
    char *dest_ch;
    char *src_ch;
    dest_ch = (char *) dest;
    src_ch = (char *) src;
    for(i=0;i<len;i++){
        *dest_ch++ = *src_ch++;
    }
    return (void *)dest_ch; 
}
