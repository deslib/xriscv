#include <stdio.h> 
#include "glb.h"
#include "logger.h"
#include "utils.h"

char * STR_OP_LUI       = "LUI";
char * STR_OP_AUIPC     = "AUIPC";
char * STR_OP_JAL       = "JAL";
char * STR_OP_JALR      = "JALR";
char * STR_OP_BRANCH    = "BRANCH";
char * STR_OP_LOAD      = "LOAD";
char * STR_OP_STORE     = "STORE";
char * STR_OP_IMM       = "IMM";
char * STR_OP_REG       = "REG";


char * get_op_str(u8 opcode){
    if(opcode == OP_LUI){
        return STR_OP_LUI;
    }else if(opcode == OP_AUIPC){
        return STR_OP_AUIPC;
    }else if(opcode == OP_JAL){
        return STR_OP_JAL;
    }else if(opcode == OP_JALR){
        return STR_OP_JALR;
    }else if(opcode == OP_BRANCH){
        return STR_OP_BRANCH;
    }else if(opcode == OP_LOAD){
        return STR_OP_LOAD;
    }else if(opcode == OP_STORE){
        return STR_OP_STORE;
    }else if(opcode == OP_IMM){
        return STR_OP_IMM;
    }else if(opcode == OP_REG){
        return STR_OP_REG;
    }
}


