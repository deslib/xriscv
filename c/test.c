#include "glb.h"
#include <stdio.h>

void test_alu_16(){
    i16 ia;
    i16 ib;
    u16 ua;
    u16 ub;
    i16 ic = 0;
    u16 uc = 0;
    i16 i;
    u16 target_sum = 0xf*0x8*0x12;
    printf("Test 16 bit ALU\n");

    for(i=0xf;i>=0;i--){
        ia = i;
        ib = i*0x11;
        ua = i;
        ub = i*0x11;
        ic = ic + ia + ib;
        uc = uc + ua + ub;
        printf("%d %u\n",ic,uc);
        printf("%d>>3=%d\n",ic,ic>>3);
        printf("%d<<1=%d\n",ic,ic<<1);
        printf("%d^%d=%d\n",uc,uc,uc^uc);
    }
    if(ic != target_sum){
        printf("ic error,it is %d\n",ic);
    }else{
        printf(".");
    }
    if(uc != target_sum){
        printf("ic error,it is %d\n",uc);
    }else{
        printf(".");
    }
    printf("\n");

}

void test_alu_32(){
    i32 ia;
    i32 ib;
    u32 ua;
    u32 ub;
    i32 ic = 0;
    u32 uc = 0;
    u32 i;
    u32 target_sum = 0xff*0x80*0x102;

    printf("Test 32 bit ALU\n");
    for(i=0;i<0x100;i++){
        ia = i;
        ib = i*0x101;
        ua = i;
        ub = i*0x101;
        ic = ic + ia + ib;
        uc = uc + ua + ub;
        printf("%d>>3=%d\n",ic,ic>>3);
        printf("%d<<1=%d\n",ic,ic<<1);
        printf("%d^%d=%d\n",uc,uc>>1,uc^(uc>>1));
        printf("%d %u\n",ic,uc);
    }
    if(ic != target_sum){
        printf("ic error,it is %d\n",ic);
    }else{
        printf(".");
    }
    if(uc != target_sum){
        printf("ic error,it is %d\n",uc);
    }else{
        printf(".");
    }
    printf("\n");
}

void test(){
    test_alu_16();
    test_alu_32();
}
