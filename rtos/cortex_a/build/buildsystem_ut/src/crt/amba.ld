OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_Reset)
SECTIONS
{
    . = 0x60000000;

    .text : {
        *startup.o (.text*)
        *(.text*)
    }

    . = ALIGN(4);
    .rodata : {
        *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*)))
    }

    . = ALIGN(4);
    .data : {
        *(.data*)
    }

    . = ALIGN(4);
    .bss : {
        lk_bss_start = .;
        *(.bss*)
        lk_bss_end = .;
    }

    . = ALIGN(32);
    /* Stack size */
    . = ALIGN(0x1000);
    . = . + 0x2000;
    stack_top = .;
}

