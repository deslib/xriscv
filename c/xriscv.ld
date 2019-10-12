MEMORY
{
    RAM (rw!x) : ORIGIN = 0x00000000, LENGTH = 0x8000
    IO  (rw!x) : ORIGIN = 0x00008000, LENGTH = 0x1000
}

SECTIONS
{
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
