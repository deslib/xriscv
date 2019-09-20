.section text
.align 2
.global auipc_test
.global lb_test
.global lh_test
.global lw_test
.global lbu_test
.global lhu_test

.global addi_test
.global slti_test
.global sltiu_test
.global xori_test
.global ori_test
.global slli_test
.global srli_test
.global srai_test

.global add_test
.global sub_test
.global sll_test
.global slt_test
.global sltu_test
.global xor_test
.global srl_test
.global sra_test
.global or_test
.global and_test


# input: a0 - 0: get auipc symbol address / 1: get auipc pc
# output: a0 == 0? a0 = pc00 symbol address : a0 = pc 
auipc_test:
    addi sp, sp, -16            # allocate the stack frame
    sw ra, 12(sp)               # save our return address on the stack 

    beqz a0, getaddr
pc00:
    auipc a0, 0                 # get current pc
    j exit

getaddr:
    la a0, pc00                 # get symbol address

exit:
    lw ra, 12(sp)               # restore our return address
    addi sp, sp, 16             # deallocate the stack frame
    ret

# input: a0 - offset
lb_test:
    lb a0, 0(a0)
    ret

# input: a0 - offset
lh_test:
    lh a0, 0(a0)
    ret

# input: a0 - offset
lw_test:
    lw a0, 0(a0)
    ret

# input: a0 - offset
lbu_test:
    lbu a0, 0(a0)
    ret

# input: a0 - offset
lhu_test:
    lhu a0, 0(a0)
    ret

addi_test:
    addi  a0, a0, 3
    ret

slti_test:
    slti a0, a0, 3
    ret

sltiu_test:
    sltiu a0, a0, 3
    ret

xori_test:
    xori a0, a0, 3
    ret

ori_test:
    ori a0, a0, 3
    ret

andi_test:
    andi a0, a0, 3
    ret

slli_test:
    slli a0, a0, 3
    ret

srli_test:
    srli a0, a0, 3
    ret

srai_test:
    srai a0, a0, 3
    ret

add_test:
    add a0, a0, a1
    ret

sub_test:
    sub a0, a0, a1
    ret

sll_test:
    sll a0, a0, a1
    ret

slt_test:
    slt a0, a0, a1
    ret

sltu_test:
    sltu a0, a0, a1
    ret

xor_test:
    xor a0, a0, a1
    ret

srl_test:
    srl a0, a0, a1
    ret

sra_test:
    sra a0, a0, a1
    ret

or_test:
    or a0, a0, a1
    ret

and_test:
    and a0, a0, a1
    ret
