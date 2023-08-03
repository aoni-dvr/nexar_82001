/**
 *  @file AmbaBoot.asm
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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

#if defined(CONFIG_XEN_SUPPORT)
        .global __xen_zimage_header
        .global _libxenplat_start
__xen_zimage_header:
_libxenplat_start:
        /*
         * Linux Image header, cf. arch/arm64/kernel/head.S
         * Documentation/arm64/booting.rst
         */
        b       __xen_guest_prologue
        nop
#if 0
        .quad   0                               /* Image load offset from start of RAM, little-endian (0 = relocatable) */
        .quad   _edata - __xen_zimage_header    /* Effective size of kernel image, little-endian */
#else
#ifdef CONFIG_XEN_DOMU_TX
        .quad   0x00000000                      /* Image load offset, little endian */
        .quad   (0x40000000 + (_edata - __xen_zimage_header))
#else // dom0
        .quad   0x00000000                      /* Image load offset, little endian */
        .quad   (0x10000000 + (_edata - __xen_zimage_header))
#endif // CONFIG_XEN_DOMU_TX
#endif // 0/1
        .quad   0                       /* Informative flags, little-endian */
        .quad   0                       /* reserved 2 */
        .quad   0                       /* reserved 3 */
        .quad   0                       /* reserved 4 */
        .ascii  "ARM\x64"               /* Magic number */
        .word   0                       /* Offset to the PE header. */
        /* End of zImage header */

__xen_guest_prologue:
        /*
         * Linux Booting arguments, cf.
         * - linux/Documentation/arm/booting.rst
         * - linux/Documentation/arm64/booting.rst
         * For ARM32, only r2 (dtb) is used.
         * For ARM64, only x0 (dtb) is used.
         */
        /*
         * We don't know where Xen will load us, and since the executable
         * must run at a known location, we need to set up the MMU
         * just enugh to start running the program in virtual address space.
         */
        adr     x1, __xen_zimage_header /* x1 = physical address */
        ldr     x3, =__xen_zimage_header/* x3 = virtual address in .lds */
#if 1
        /* Ensure PA > VA */
        sub     x9, x1, x3              /* x9 = (physical - virtual) offset */
#else
        /* Ensure VA > PA */
        sub     x9, x3, x1
#endif

        /* Save DTB pointer (x0 -> x10) and address offset (x9 -> x11) */
        mov     x10, x0
        mov     x11, x9

        /* x7 = virtual addr of ttbl */
        ldr     x7, =__mmu_ptbl_start
        add     x1, x7, x9              /* x1 = physical addr of ttbl */

        /* Clear MMU table */
        adrp    x20, __mmu_ptbl_start
        adrp    x21, __mmu_ptbl_end
        sub     x21, x21, x20
1:      stp     xzr, xzr, [x20], #16
        stp     xzr, xzr, [x20], #16
        stp     xzr, xzr, [x20], #16
        stp     xzr, xzr, [x20], #16
        subs    x21, x21, #64
        b.ne    1b

#if 0
        /* TODO: implementation of relocation. */
        /* Set the page table base address register, cf. include/public/arch-arm.h */
#endif

        /*
         * Store the address offset in a global variable
         */
        ldr     x0, =__xen_physical_addr_offset
        str     x11, [x0]

        /*
         * Store the device tree physical address
         */
        ldr     x0, =__xen_device_tree
        str     x10, [x0]

        /*
         * Go to original ThreadX boot, but skip first two instructions
         * that touches SCTLR unnecessarily in this boot scheme.
         */
        ldr     x0, =__AmbaProgramStart
        mov     x1, #1          // EL1
        br      x0
        b       .


        .data

        .align 3
        .global __xen_physical_addr_offset
__xen_physical_addr_offset:
        .quad 0x0

        .align 3
        .global __xen_device_tree
__xen_device_tree:
        .quad 0x0

#endif // CONFIG_XEN_SUPPORT

