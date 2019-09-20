.section text
.align 2
.global auipc_test
.global lb_test
.global lh_test
.global lw_test
.global lbu_test
.global lhu_test


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
