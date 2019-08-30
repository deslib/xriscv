MEMORY
{
    ROM (x!rw) : ORIGIN = 0x00000000, LENGTH = 0x1000
    RAM (rw!x) : ORIGIN = 0x00001000, LENGTH = 0x1000
}

SECTIONS
{
    .text :
    {
        boot.o(.text)
        *(.text)

    } > ROM

    .data :
    {
        *(.data)
        *(.bss)
        *(.rela*)
        *(.rodata*)
        *(.sbss)

    } > RAM
}
