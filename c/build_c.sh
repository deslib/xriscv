riscv-none-embed-gcc.exe -Wall -Os -fomit-frame-pointer -march=rv32ic -S xuart.c -o xuart.s
riscv-none-embed-gcc.exe -Wall -Os -fomit-frame-pointer -march=rv32ic -S boot.c -o boot.s
riscv-none-embed-gcc.exe -Wall -Os -fomit-frame-pointer -march=rv32ic -S regfile.c -o regfile.s
riscv-none-embed-gcc.exe -Wall -Os -fomit-frame-pointer -march=rv32ic -S stdio.c -o stdio.s
riscv-none-embed-gcc.exe -Wall -Os -fomit-frame-pointer -march=rv32ic -S string.c -o string.s
riscv-none-embed-gcc.exe -Wall -Os -fomit-frame-pointer -march=rv32ic -S test.c -o test.s
riscv-none-embed-as.exe -march=rv32ic -c xuart.s -o xuart.o
riscv-none-embed-as.exe -march=rv32ic -c boot.s -o boot.o
riscv-none-embed-as.exe -march=rv32ic -c regfile.s -o regfile.o
riscv-none-embed-as.exe -march=rv32ic -c startup.s -o startup.o
riscv-none-embed-as.exe -march=rv32ic -c stdio.s -o stdio.o
riscv-none-embed-as.exe -march=rv32ic -c string.s -o string.o
riscv-none-embed-as.exe -march=rv32ic -c test.s -o test.o
riscv-none-embed-as.exe -march=rv32ic -c assem.s -o assem.o
riscv-none-embed-ld.exe -Txriscv.ld -Map=xriscv.map startup.o boot.o stdio.o string.o xuart.o regfile.o  assem.o test.o -L$RV32IC_ILP32 -lgcc -o xriscv.o 
riscv-none-embed-objcopy -O binary xriscv.o xriscv.bin
riscv-none-embed-objdump -d xriscv.o > xriscv_c.lst
python ../py/t.py b2r xriscv.bin xriscv.rom xriscv.ram xriscv.coe xriscv_ram.bin
