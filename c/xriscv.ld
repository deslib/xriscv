MEMORY
{
    ROM (x!rw) : ORIGIN = 0x00000000, LENGTH = 0x0800
    RAM (rw!x) : ORIGIN = 0x00000800, LENGTH = 0x0800
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
