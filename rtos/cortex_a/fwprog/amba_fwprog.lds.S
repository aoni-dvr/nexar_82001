/**
 * fwprog.lds.S
 */

#define DDR_START           0x08000000

#if defined(CONFIG_ARM64)
OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
#else
#error do not support 32bit FW programmer
#endif

ENTRY(__vector_fwprog)
SECTIONS
{

    . = DDR_START;
    .text : {
        __BEGIN_FIRMWARE_IMAGE__ = ABSOLUTE(.);
        *AmbaFlashFwProg_Boot*.o (.text*)
        *.o (.text*)
        * (.rodata*)

        /* set minimal size of text section to be 4KB
        *  to ensure linker will generate 1-segment firmware
        */
        . = ALIGN(0x1000);
    }

    .data : {
        . = ALIGN(0x1000);
        * (.data*)
    }

    .fwdata : {
#if defined(__LINK_BST__)
        . = ALIGN(2048);
        __BEGIN_BST_IMAGE__ = .;
        KEEP (FW_OUT_DIR/bst.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_BST_IMAGE__ = .;
#endif

#if defined(__LINK_BLD__)
        . = ALIGN(2048);
        __BEGIN_BLD_IMAGE__ = .;
        KEEP (FW_OUT_DIR/bld.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_BLD_IMAGE__ = . ;
#endif

#if defined(__LINK_PBA__)
        . = ALIGN(2048);
        __BEGIN_PBA_IMAGE__ = .;
        KEEP (FW_OUT_DIR/pba.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_PBA_IMAGE__ = .;
#endif

#if defined(__LINK_ATF__)
        . = ALIGN(2048);
        __BEGIN_ATF_IMAGE__ = .;
        KEEP (FW_OUT_DIR/atf.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_ATF_IMAGE__ = .;
#endif

#if defined(__LINK_IPL__)
        . = ALIGN(2048);
        __BEGIN_IPL_IMAGE__ = .;
        KEEP (FW_OUT_DIR/ipl.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_IPL_IMAGE__ = .;
#endif

#if defined(__LINK_BLD2__)
        . = ALIGN(2048);
        __BEGIN_BLD2_IMAGE__ = .;
        KEEP (FW_OUT_DIR/bld2.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_BLD2_IMAGE__ = .;
#endif

#if defined(__LINK_SYS__)
        . = ALIGN(2048);
        __BEGIN_SYS_IMAGE__ = .;
        KEEP (FW_OUT_DIR/sys.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_SYS_IMAGE__ = .;
#endif

#if defined(__LINK_R52SYS__)
        . = ALIGN(2048);
        __BEGIN_R52SYS_IMAGE__ = .;
        KEEP (FW_OUT_DIR/r52sys.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_R52SYS_IMAGE__ = .;
#endif

#if defined(__LINK_XEN__)
        . = ALIGN(2048);
        __BEGIN_XEN_IMAGE__ = .;
        KEEP (FW_OUT_DIR/xen.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_XEN_IMAGE__ = .;
#endif

#if defined(__LINK_XTB__)
        . = ALIGN(2048);
        __BEGIN_XTB_IMAGE__ = .;
        KEEP (FW_OUT_DIR/xtb.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_XTB_IMAGE__ = .;
#endif

#if defined(__LINK_X0K__)
        . = ALIGN(2048);
        __BEGIN_X0K_IMAGE__ = .;
        KEEP (FW_OUT_DIR/x0k.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_X0K_IMAGE__ = .;
#endif

#if defined(__LINK_X0D__)
        . = ALIGN(2048);
        __BEGIN_X0D_IMAGE__ = .;
        KEEP (FW_OUT_DIR/x0d.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_X0D_IMAGE__ = .;
#endif

#if defined(__LINK_DSP__)
        . = ALIGN(2048);
        __BEGIN_DSP_IMAGE__ = .;
        KEEP (FW_OUT_DIR/dsp.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_DSP_IMAGE__ = .;
#endif

#if defined(__LINK_ROM__)
        . = ALIGN(2048);
        __BEGIN_ROM_IMAGE__ = .;
        KEEP (FW_OUT_DIR/rom.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_ROM_IMAGE__ = .;
#endif

#if defined(__LINK_LNX__)
        . = ALIGN(2048);
        __BEGIN_LNX_IMAGE__ = .;
        KEEP (FW_OUT_DIR/lnx.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_LNX_IMAGE__ = .;
#endif

#if defined(__LINK_DTB__)
        . = ALIGN(2048);
        __BEGIN_DTB_IMAGE__ = .;
        KEEP (FW_OUT_DIR/dtb.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_DTB_IMAGE__ = .;
#endif

#if defined(__LINK_RFS__)
        . = ALIGN(2048);
        __BEGIN_RFS_IMAGE__ = .;
        KEEP (FW_OUT_DIR/rfs.fw (*))
        . = ALIGN(2048);
        . += 0x100;             /* there is a 0x100 header in the fw. */
        __END_RFS_IMAGE__ = .;
#endif

        . = ALIGN(2048);
        __END_FIRMWARE_IMAGE__ = ABSOLUTE(.);
    }

    .bss.noinit : {
        . = ALIGN(32);
        __noinit_start = .;
        * (.bss.noinit*)
        __noinit_end = .;
    }

    .bss : {
        . = ALIGN(32);
        __bss_start = .;
        * (.bss*)
        * (COMMON)
        __bss_end = .;
    }


#ifdef CONFIG_ARM_UNWIND
    /*
     * Stack unwinding tables
     */
    . = ALIGN(8);
    .ARM.unwind_idx : {
        __start_unwind_idx = .;
        *(.ARM.exidx*)
        __stop_unwind_idx = .;
    }
    .ARM.unwind_tab : {
        __start_unwind_tab = .;
        *(.ARM.extab*)
        __stop_unwind_tab = .;
    }
#endif

    PROVIDE (end = .);

    .memfwprog . (NOLOAD) : {
        . = ALIGN(0x10000);
        __memfwprog_result = .;
        . = __memfwprog_result  + (0x10000);
        . = ALIGN(0x10000);
        __memfwprog_command = .;
        . = __memfwprog_command + (0x10000);
    }

    /DISCARD/ : {
    /* Discard .ARM.exidx for directUSB parsing. */
    * (.ARM.exidx)
/*
#if !defined(__LINK_BST__)
        FW_OUT_DIR/bst.fw (*)
#endif
#if !defined(__LINK_BLD__)
        FW_OUT_DIR/bld.fw (*)
#endif
#if !defined(__LINK_PBA__)
        FW_OUT_DIR/pba.fw (*)
#endif
#if !defined(__LINK_ATF__)
        FW_OUT_DIR/atf.fw (*)
#endif
#if !defined(__LINK_IPL__)
        FW_OUT_DIR/ipl.fw (*)
#endif
#if !defined(__LINK_BLD2__)
        FW_OUT_DIR/bld2.fw (*)
#endif
#if !defined(__LINK_SYS__)
        FW_OUT_DIR/sys.fw (*)
#endif
#if !defined(__LINK_R52SYS__)
        FW_OUT_DIR/r52sys.fw (*)
#endif
#if !defined(__LINK_DSP__)
        FW_OUT_DIR/dsp.fw (*)
#endif
#if !defined(__LINK_XEN__)
        FW_OUT_DIR/xen.fw (*)
#endif
#if !defined(__LINK_XTB__)
        FW_OUT_DIR/xtb.fw (*)
#endif

#if 0
#if !defined(__LINK_X0K__)
        FW_OUT_DIR/x0k.fw (*)
#endif
#if !defined(__LINK_X0D__)
        FW_OUT_DIR/x0d.fw (*)
#endif
#endif

#if !defined(__LINK_ROM__)
        FW_OUT_DIR/rom.fw (*)
#endif
#if !defined(__LINK_LNX__)
        FW_OUT_DIR/lnx.fw (*)
#endif
#if !defined(__LINK_DTB__)
#ifndef CONFIG_LINUX
        FW_OUT_DIR/dtb.fw (*)
#endif
#endif
#if !defined(__LINK_RFS__)
        FW_OUT_DIR/rfs.fw (*)
#endif
*/
    }
}
