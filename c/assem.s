.section text
.align 2
.global auipc_test

# input: a0
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
