MEMORY
{
    ROM (x!rw) : ORIGIN = 0x00000000, LENGTH = 0x0800
    RAM (rw!x) : ORIGIN = 0x00001000, LENGTH = 0x1F000
    IO  (rw!x) : ORIGIN = 0x00000800, LENGTH = 0x10
}

SECTIONS
{
    .text :
    {
        startup.o
        boot.o(.text)
        assem.o

    } > ROM

    .data :
    {
        *(.text)
        *(.data)
        *(.bss)
        *(.rela*)
        *(.rodata*)
        *(.sbss)

    } > RAM

    .regfile :
    {
        regfile.o(COMMON)
    } > IO
}
