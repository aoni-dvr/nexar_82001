/**
 *  @file AmbaBoot.asm
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Cortex-A53 Boot Code for system
 *
 */

#include "AmbaCortexA53.h"

        .text
        .arm

#if defined(CONFIG_XEN_SUPPORT)
        .global __xen_zimage_header
        .global _libxenplat_start
__xen_zimage_header:
_libxenplat_start:
        /*
         * Linux zImage header, cf. linux/arch/arm/boot/compressed/head.S
         */
        .rept   8
        mov     r0, r0
        .endr
        b       __xen_guest_prologue
        .word   0x016F2818      /* Magic numbers to help the loader */
#if 0
        .word   0               /* zImage start address (0 = relocatable) */
        .wod   _edata - __xen_zimage_header     /* zImage end address */
#else
#ifdef CONFIG_XEN_DOMU_TX
        .word  0x40000000
        .word  (0x40000000 + (_edata - __xen_zimage_header))    /* zImage end address */
#else // dom0
        .word  0x10000000
        .word  (0x10000000 + (_edata - __xen_zimage_header))    /* Image end address */
#endif
#endif
        /* End of zImage header */

__xen_guest_prologue:
        /*
         * Linux Booting arguments, cf.
         * - linux/Documentation/arm/booting.rst
         * - linux/Documentation/arm64/booting.rst
         * For ARM32, only r2 (dtb) is used.
         */
        /*
         * We don't know where Xen will load us, and since the executable
         * must run at a known location, we need to set up the MMU
         * just enugh to start running the program in virtual address space.
         */
        adr     r1, __xen_zimage_header /* r1 = physical address */
        ldr     r3, =__xen_zimage_header/* r3 = virtual address in .lds */
#if 1
        /* Ensure PA > VA */
        sub     r9, r1, r3              /* r9 = (physical - virtual) offset */
#else
        /* Ensure VA > PA */
        sub     r9, r3, r1
#endif

        /* r7 = virtual addr of ttbl */
        ldr     r7, =__mmu_ptbl_start
        add     r1, r7, r9              /* r1 = physical addr of ttbl */

        /* Clear MMU table */
        ldr     r4, =__mmu_ptbl_start
        ldr     r5, =__mmu_ptbl_end
        sub     r5, r5, r4
        mov     r6, #0
1:      str     r6, [r4], #4
        str     r6, [r4], #4
        str     r6, [r4], #4
        str     r6, [r4], #4
        subs    r5, r5, #16
        bne     1b

        /* Save DTB pointer (r2 -> r10) and address offset (r9 -> r11) */
        mov     r10, r2
        mov     r11, r9

        /* Set the page table base address register, cf. include/public/arch-arm.h */
        orr     r0, r1, #11             /* Shareable, inner/outer write-back write-allocate cacheable */
        mcr     p15, 0, r0, c2, c0, 0   /* Set TTBR0 */

        /*
         * Set access permission for domains.
         * Domains are deprecated, but we have to configure them anyway.
         * We mark every page as being domain 0 and set domain 0 to "client
         * mode" (client mode = use access flags in page table).
         */
        mov     r0, #1                  /* 1 = client */
        mcr     p15, 0, r0, c3, c0, 0   /* DACR */

        /*
         * Template (flags) for a 1 MB page-table entry.
         * C B = 1 1 (outer and inner write-back, write-allocate)
         * After this, r8 = template page table entry
         */
#if 1
        ldr     r8,   =(0x2 +           /* Section entry */     \
                        0xc +           /* C B */               \
                        (3 << 10) +     /* Read/write */        \
                        (1 << 12) +     /* TEX */               \
                        (1 << 16) +     /* Sharable */          \
                        (1 << 19))      /* Non-secure */
#else
        ldr     r8,   =(0x2 +           /* Section entry */     \
                        0xc +           /* C B */               \
                        (3 << 10) +     /* Read/write */        \
                        (1 << 19))      /* Non-secure */
#endif

        /*
         * Store the virtual end address of the kernel so we can map
         * enough 1MB sections to cover all of it. We map up to the
         * presumed memory size as specified in the linker script.
         */
//      ldr     r5, =__ddr_end
//      ldr     r5, =__bss_end
//      ldr     r5, =_edata
#ifdef CONFIG_XEN_DOMU_TX
        ldr     r5, =0x48000000
#else
        ldr     r5, =0x20000000
#endif

        /*
         * Load the physical start MB for the code segment
         */
        mov     r0, pc, lsr #20

        /*
         * Map the desired virtual address (from r4) to this physical
         * location.
         * r4 = desired virtual address of this section.
         */
        ldr     r4, =__xen_zimage_header

1:
        /*
         * Set up the descriptor for this page table entry: combine the
         * current megabyte counter (r0) with the descriptor template (r8)
         * into r3
         */
        mov     r3, r0, lsl #20
        orr     r3, r3, r8

        /*
         * Store the descriptor (r3) using the current virtual address (r4)
         * as the page table index into the page table at (r1)
         */
        str     r3, [r1, r4, lsr #18]

        /* Advance the virtual mapping by 1MB */
        add     r4, r4, #1<<20

        /* Advance the physical address by 1MB */
        add     r0, r0, #1

        /*
         * Cmpare the virtual address mapped to the virtual end. If the address
         * we just mapped is before the end of the kernel, we have more mappings
         * to create
         */
        cmp     r4, r5
        blt     1b

        /* Done witht the virtual->physical map */

        /*
         * Now, map the current physical address, too, so we can keep
         * executing after the MMU is turned on
         */

        /* Load the physical start MB address for the code segment */
        mov     r0, pc, lsr #20
        lsl     r0, r0, #20

        /*
         * First, adjust r5 so that it refers to the physical _end instead
         * of the virtual __ddr_end using the precomputed virtual/physical
         * offset (r9).
         */
        add     r5, r5, r9

2:
        /*
         * Set up the descriptor for this page table entry: combine the
         * current physical address (r0) with the descriptor template (r8)
         * into r3
         */
        orr     r3, r0, r8

        /*
         * Store the physical->physical mapping into the page table at (r1)
         */
        str     r3, [r1, r0, lsr #18]

        /*
         * Advance the physical address (and virtual, since they are the
         * same) by 1MB
         */
        add     r0, r0, #1<<20

        /*
         * If the address we just mapped is before the end of the kernel,
         * we have more mappings to create.
         */
        cmp     r0, r5
        blt     2b

        /*
         * Invalidate TLB
         */
        dsb     /* Caching is off, but must still prevent reordering */
        mcr     p15, 0, r1, c8, c7, 0   /* TLBIALL */

        /*
         * Enable MMU/SCTLR
        */
        mrc     p15, 0, r1, c1, c0, 0   /* SCTLR */
        orr     r1, r1, #3 << 11        /* Enable icache, branch prediction */
        orr     r1, r1, #4 + 1          /* Enable dcache, MMU */
        mcr     p15, 0, r1, c1, c0, 0   /* SCTLR */
        isb

        /*
         * Branch to virtual address of stage2 now that the MMU is on
         */
        ldr     r1, =stage2
        bx      r1

stage2:
        /*
         * Store the address offset in a global variable
         */
        ldr     r0, =__xen_physical_addr_offset
        str     r11, [r0]

        /*
         * Store the device tree physical address
         */
        ldr     r0, =__xen_device_tree
        str     r10, [r0]

        /*
         * Go to original ThreadX boot, but skip first two instructions
         * that touches SCTLR unnecessarily in this boot scheme.
         */
        ldr     r1, =AmbaProgramStart
        add     r1, r1, #8
        bx      r1
        b       .


        .data

        .align 2
        .global __xen_physical_addr_offset
__xen_physical_addr_offset:
        .word 0x0

        .align 2
        .global __xen_device_tree
__xen_device_tree:
        .word 0x0

#endif // CONFIG_XEN_SUPPORT

