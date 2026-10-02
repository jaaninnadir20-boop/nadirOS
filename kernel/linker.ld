OUTPUT_FORMAT(elf64-x86-64)
ENTRY(kmain)

SECTIONS
{
    /* First bring-up uses a low identity-mapped address.
       Higher-half mapping will be added with the virtual memory subsystem. */
    . = 0x00100000;

    .text : ALIGN(4096)
    {
        KEEP(*(.text.entry))
        *(.text*)
    }

    .rodata : ALIGN(4096)
    {
        *(.rodata*)
    }

    .data : ALIGN(4096)
    {
        *(.data*)
    }

    .bss : ALIGN(4096)
    {
        *(COMMON)
        *(.bss*)
    }
}
