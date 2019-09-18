riscv-none-embed-gcc.exe -Wall -Os -fomit-frame-pointer -march=rv32i -S xuart.c -o xuart.s
riscv-none-embed-gcc.exe -Wall -Os -fomit-frame-pointer -march=rv32i -S boot.c -o boot.s
riscv-none-embed-gcc.exe -Wall -Os -fomit-frame-pointer -march=rv32i -S regfile.c -o regfile.s
riscv-none-embed-gcc.exe -Wall -Os -fomit-frame-pointer -march=rv32i -S stdio.c -o stdio.s
riscv-none-embed-gcc.exe -Wall -Os -fomit-frame-pointer -march=rv32i -S string.c -o string.s
riscv-none-embed-gcc.exe -Wall -Os -fomit-frame-pointer -march=rv32i -S test.c -o test.s
riscv-none-embed-as.exe -march=rv32i -c xuart.s -o xuart.o
riscv-none-embed-as.exe -march=rv32i -c boot.s -o boot.o
riscv-none-embed-as.exe -march=rv32i -c regfile.s -o regfile.o
riscv-none-embed-as.exe -march=rv32i -c startup.s -o startup.o
riscv-none-embed-as.exe -march=rv32i -c stdio.s -o stdio.o
riscv-none-embed-as.exe -march=rv32i -c string.s -o string.o
riscv-none-embed-as.exe -march=rv32i -c test.s -o test.o
riscv-none-embed-ld.exe -Txriscv.ld -L. -Map=xriscv.map startup.o boot.o stdio.o string.o xuart.o regfile.o test.o -lgcc -o xriscv.o -L$RV32I_ILP32
riscv-none-embed-objcopy -O binary xriscv.o xriscv.bin
riscv-none-embed-objdump -d xriscv.o > xriscv.lst
python ../py/t.py b2r xriscv.bin xriscv.rom xriscv.ram
