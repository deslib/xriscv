riscv-none-embed-gcc.exe -Wall -Os -fomit-frame-pointer -march=rv32i -S xuart.c -o xuart.s
riscv-none-embed-gcc.exe -Wall -Os -fomit-frame-pointer -march=rv32i -S boot.c -o boot.s
riscv-none-embed-as.exe -march=rv32i -c xuart.s -o xuart.o
riscv-none-embed-as.exe -march=rv32i -c boot.s -o boot.o
riscv-none-embed-ld.exe -Txriscv.ld -Map=xriscv.map boot.o xuart.o -o xriscv.o
riscv-none-embed-objcopy -O binary xriscv.o xriscv.bin
riscv-none-embed-objdump -d xriscv.o > xriscv.lst
python ../py/t.py b2r xriscv.bin xriscv.rom
