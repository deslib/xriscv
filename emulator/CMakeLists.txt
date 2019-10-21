# mkdir build
# cd build
# cmake -G"Unix Makefiles" ../
# make

cmake_minimum_required(VERSION 3.10)
project(riscv)
add_executable(rv32 main.c logger.c utils.c csr.c rv32i.c)