# DOC
- [Instruction set manual](https://content.riscv.org/wp-content/uploads/2017/05/riscv-spec-v2.2.pdf)   
- Book:The reader of risc-v. [EN](https://people.eecs.berkeley.edu/~krste/papers/EECS-2016-1.pdf) [CN](http://crva.io/documents/RISC-V-Reader-Chinese-v2p1.pdf)
# XRISCV
A systemveriog implemented risc-v. Including an emulator written with C

# Emulator
Please use [mingw64](http://www.mingw-w64.org/doku.php) to compile the files.

# C
- source code for the test software
- I am using [Eclipse](https://github.com/gnu-mcu-eclipse/riscv-none-gcc/releases/) as the compiling tool. 
- set enviorment parameter RV32I_ILP32 to libgcc.a folder, eg. "C:\work\riscv\tool\riscv_embedded_gcc-8.2.0-2.2-20190521-0004\lib\gcc\riscv-none-embed\8.2.0\rv32i\ilp32"