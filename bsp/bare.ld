MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x00080000
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00020000
}

SECTIONS
{
    .text :
    {
        KEEP(*(.vectors))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        __etext = .;
    } > FLASH

    .data : AT(ADDR(.text) + SIZEOF(.text))
    {
        __data_load__ = LOADADDR (.data);
        __data_start__ = .;
        . = ALIGN(4);
        *(.data*)
        . = ALIGN(4);
        __data_end__ = .;
    } > SRAM

    .bss :
    {
        __bss_start__ = .;
        . = ALIGN(4);
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > SRAM

    .heap :
    {
        __heap_start__ = .;
        . = ALIGN(4);
        *(.heap*)
        . = ALIGN(4);
        __heap_end__ = .;
    }

    .stack :
    {
        __stack_start__ = .;
        . = ALIGN(4);
        __stack_size__ = 8192;
        . = . + __stack_size__;
        . = ALIGN(4);
        __stack_end__ = .;
    }
}
