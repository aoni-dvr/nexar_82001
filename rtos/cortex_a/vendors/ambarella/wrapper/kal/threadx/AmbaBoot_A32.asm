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

/*-----------------------------------------------------------------------------------------------*\
 *   Mode, correspords to bits 0-5 in CPSR
\*-----------------------------------------------------------------------------------------------*/
#define MODE_MASK   0x1F    /* Bit mask for mode bits in CPSR */
#define FIQ_DISABLE 0x40    /* ARM 1= disable FIQ */
#define IRQ_DISABLE 0x80    /* ARM 1= disable IRQ */

#define USR_MODE    0xD0    /* User mode */
#define FIQ_MODE    0xD1    /* Fast Interrupt Request mode */
#define IRQ_MODE    0xD2    /* Interrupt Request mode */
#define SVC_MODE    0xD3    /* Supervisor mode */
#define ABT_MODE    0xD7    /* Abort mode */
#define UND_MODE    0xDB    /* Undefined Instruction mode */
#define SYS_MODE    0xDF    /* System mode */

        .text
        .arm

        .globl __vector
        .globl AmbaProgramStart

#ifdef CONFIG_XEN_SUPPORT
        .align 12
#else
        .align 5                            /* Vector Base Address shall be 32-byte alignment */
#endif
__vector:                                   /* Make this a DATA label, so that stack usage */
                                            /* analysis doesn't consider it an uncalled function */
        B       AmbaProgramStart            /* 0x00: Reset (RESET: Supervisor) */
        B       Undefined_Handler           /* 0x04: Undefined intruction (UNDEF) */
        B       SWI_Handler                 /* 0x08: Software Interrupt (SWI: Supervisor) */
        B       Prefetch_Handler            /* 0x0c: Prefetch Abort (PABT) */
        B       DataAbort_Handler           /* 0x10: Data Abort (DABT) */
        B       .                           /* 0x14: Reserved */
        LDR     PC, IRQ_IsrAddr             /* 0x18: IRQ */
        LDR     PC, FIQ_IsrAddr             /* 0x1c: FIQ */

/*-----------------------------------------------------------------------------------------------*\
 *   Default exception handlers
\*-----------------------------------------------------------------------------------------------*/
.macro DumpRegisters
        CPSID   if                          @ Disable FIQ and IRQ interrupts
        STMDB   SP!, {R0 - R7}              @ Save r0 - r7
        MOV     R7, SP                      @ Use R7 as SP

        MOV     R1, LR                      @ Get preview mode PC
        MRS     R2, SPSR                    @ Read current SPSR
        MRS     R3, CPSR                    @ Read current CPSR
        ORR     R4, R2, #0xc0               @ Set FIQ and IRQ disable bit flags

        MSR     CPSR_c, R4                  @ Switch to previous mode
        STMDB   R7!, {R8 - R14}             @ Save r8 - r14

        MSR     CPSR_cxsf, R3               @ Switch back to current mode
        STMDB   R7!, {R1, R2}               @ Save preview mode PC, CPSR
        MOV     SP, R7                      @ Restore R7 to SP
.endm


Undefined_Handler:
        DumpRegisters
        MOV     R0, #0
        MOV     R1, SP
        LDR     PC, Excp_HndlAddr

SWI_Handler:
        B       SWI_Handler

Prefetch_Handler:
        DumpRegisters
        MOV     R0, #1
        MOV     R1, SP
        LDR     PC, Excp_HndlAddr

DataAbort_Handler:
        DumpRegisters
        MOV     R0, #2
        MOV     R1, SP
        LDR     PC, Excp_HndlAddr


/*-----------------------------------------------------------------------------------------------*\
 *   low-level system initialization code.
 *
 *   After a reset execution starts here, the mode is ARM, supervisor
 *   with interrupts disabled.
\*-----------------------------------------------------------------------------------------------*/
AmbaProgramStart:
/*---------------------------------------------------------------------------*\
 *  Initialise the processor
\*---------------------------------------------------------------------------*/
AmbaResetHandler:
        MOV    R0, #0
        MCR    p15, 0, R0, c1, c0, 0        /* SCTLR */
        LDR    R0, =__vector
        MCR    p15, 0, R0, c12, c0, 0       /* VBAR */

/*---------------------------------------------------------------------------*\
 *  R0 = CPSR, R1 = Current Running Core-ID, bit [1:0]
\*---------------------------------------------------------------------------*/
        MRC     p15, 0, R1, c0, c0, 5       /* Read Multiprocessor ID register */
        AND     R1, R1, #0x03               /* R1 = Current Running ID, bit [1:0] */
        LSL     R3, R1, #2                  /* Pointer Offset = 0x0, 0x4, 0x8, 0xC */
        MRS     R0, CPSR                    /* get current mode bits to R0 */
        BIC     R4, R0, #MODE_MASK          /* clear mode */

/*---------------------------------------------------------------------------*\
 *  enter FIQ mode and setup stack
\*---------------------------------------------------------------------------*/
        ORR     R0, R4, #FIQ_MODE           /* set to FIQ mode, disable FIR/IRQ interrupts */
        MSR     CPSR_c, R0                  /* enter FIQ mode */
        LDR     R2, =AmbaSysStackBaseFIQ    /* R2 = pointer to the End of Stack */
        LDR     SP, [R2, R3]                /* set stack pointer to the End of Stack */

/*---------------------------------------------------------------------------*\
 *  enter IRQ mode and setup stack
\*---------------------------------------------------------------------------*/
        ORR     R0, R4, #IRQ_MODE           /* set to IRQ mode, disable FIR/IRQ interrupts */
        MSR     CPSR_c, R0                  /* enter FIQ mode */
        LDR     R2, =AmbaSysStackBaseIRQ    /* R2 = pointer to the End of Stack */
        LDR     SP, [R2, R3]                /* set stack pointer to the End of Stack */

/*---------------------------------------------------------------------------*\
 *  enter ABT mode and setup stack
\*---------------------------------------------------------------------------*/
        ORR     R0, R4, #ABT_MODE           /* set to ABT mode, disable FIR/IRQ interrupts */
        MSR     CPSR_c, R0                  /* enter ABT mode */
        LDR     R2, =AmbaSysStackBaseABT    /* R2 = pointer to the End of Stack */
        LDR     SP, [R2, R3]                /* set stack pointer to the End of Stack */

/*---------------------------------------------------------------------------*\
 *  enter UND mode and setup stack
\*---------------------------------------------------------------------------*/
        ORR     R0, R4, #UND_MODE           /* set to UND mode, disable FIR/IRQ interrupts */
        MSR     CPSR_c, R0                  /* enter UND mode */
        LDR     R2, =AmbaSysStackBaseUND    /* R2 = pointer to the End of Stack */
        LDR     SP, [R2, R3]                /* set stack pointer to the End of Stack */

/*---------------------------------------------------------------------------*\
 *  enter system (USER) mode and setup stack
\*---------------------------------------------------------------------------*/
        ORR     R0, R4, #SYS_MODE           /* set to SYS mode, disable FIR/IRQ interrupts */
        MSR     CPSR_c, R0                  /* enter SVC mode */
        LDR     R2, =AmbaSysStackBaseSYS    /* R2 = pointer to the End of Stack */
        LDR     SP, [R2, R3]                /* set stack pointer to the End of Stack */

/*---------------------------------------------------------------------------*\
 *  enter SVC mode and setup stack
\*---------------------------------------------------------------------------*/
        ORR     R0, R4, #SVC_MODE           /* set to SVC mode, disable FIR/IRQ interrupts */
        MSR     CPSR_c, R0                  /* enter SVC mode */
        LDR     R2, =AmbaSysStackBaseSVC    /* R2 = pointer to the End of Stack */
        LDR     SP, [R2, R3]                /* set stack pointer to the End of Stack */

/*---------------------------------------------------------------------------*\
 *  Initialize Architectural Features (Floating-point, Advanced SIMD).
\*---------------------------------------------------------------------------*/
        MRC     p15, 0, R0, c1, c0, 2       /* Read CPACR */
        ORR     R0, R0, #0xf00000           /* CPACR[23:22] = Full access CP11, CPACR[21:20] = Full access CP10 */
        MCR     p15, 0, R0, c1, c0, 2
        ISB
        MOV     R0, #0x40000000             /* FPEXC[30] = Enable Advanced SIMD and VFP extensions */
        VMSR    FPEXC, R0
        MOV     R0, #0x3000000              /* FPSCR[26] = IEEE 754, FPSCR[25] = Default NaN, FPSCR[24] = Flush-to-zero, FPSCR[23:22] = Round to nearest */
        VMSR    FPSCR, R0

        LDR     R0, _AmbaRtosCoreId         /* RtosCore initializations */
        LDR     R0, [R0]
        CMP     R1, R0
        BEQ     RtosCoreInit

        CMP     R1, #0
        BEQ     Core0Init

        ldr     r0, =#0x8200FF04            /* AMBA_SIP_BOOT_RTOS */
        mov     r2, #0
        mov     r3, #2
        ldr     r1, =#CONFIG_FWPROG_SYS_LOADADDR
        smc     #0
        B       AmbaOtherCoreEntry          /* other core initilization */

Core0Init:
        B       main

RtosCoreInit:
/*---------------------------------------------------------------------------*\
 *  Add more initialization here
\*---------------------------------------------------------------------------*/
#if defined(CONFIG_XEN_SUPPORT) && !defined(AMBA_FWPROG)
        ldr     r0, =__xen_device_tree      /* Location to store phy. location of device_tree */
        ldr     r0, [r0]                    /* Phy. location of device tree. */
        // Check magic
        ldr     r1, [r0]
        ldr     r2, =0xEDFE0DD0             /* Magic number 0xd00dfeed (big-endian). */
        cmp     r1, r2
        bne     4f
        // Check length
        ldr     r1, [r0, #4]
        rev     r1, r1
        ldr     r2, =__dts_start            /* VM location of device tree */
        ldr     r3, =__dts_end
        sub     r3, r3, r2
        cmp     r1, r3
        bgt     4f
        // Clone dtb to VM
        mov     r3, #0x0
clone_dtb:
        ldr     r4, [r0], #4
        str     r4, [r2], #4
        ldr     r4, [r0], #4
        str     r4, [r2], #4
        ldr     r4, [r0], #4
        str     r4, [r2], #4
        ldr     r4, [r0], #4
        str     r4, [r2], #4
        add     r3, r3, #16
        cmp     r3, r1
        blt     clone_dtb

4:
#endif // defined(CONFIG_XEN_SUPPORT) && !defined(AMBA_FWPROG)

/*---------------------------------------------------------------------------*\
 *  C initialization here
\*---------------------------------------------------------------------------*/
        LDR     R0, BSS_START
        LDR     R1, BSS_END
        BIC     R1, R1, #0x3
        ADD     R1, R1, #4

        MOV     R3, #0
        MOV     R4, #0
        MOV     R5, #0
        MOV     R6, #0
        MOV     R7, #0
        MOV     R8, #0
        MOV     R9, #0
        MOV     R10, #0
        MOV     R11, #0
        MOV     R12, #0

BssZeroInit:
        STMIA   R0!, {R3-R12}
        SUBS    R2, R1, R0
        CMP     R2, #40
        BGE     BssZeroInit

        CMP     R2, #0
        BEQ     BssInitDone

BssZeroInitRemain:
        STMIA   R0!, {R3}
        SUBS    R2, R2, #4
        BGT     BssZeroInitRemain

BssInitDone:
#if defined(CONFIG_XEN_SUPPORT)
        ldr     r2, =__xen_physical_addr_offset
        ldr     r0, [r2]

        BL      amba_xen_setup
#endif /* defined(CONFIG_XEN_SUPPORT) */

/*---------------------------------------------------------------------------*\
 *  Pre-initialize ThreadX before main and after BSS/DATA sections are intialized.
\*---------------------------------------------------------------------------*/
        BL      _tx_initialize_kernel_setup

/*---------------------------------------------------------------------------*\
 *  Continue to __cmain for C-level initialization
\*---------------------------------------------------------------------------*/
        B       main

/*---------------------------------------------------------------------------*\
 *  Get CPSR (Current Program Status Register)
\*---------------------------------------------------------------------------*/
.globl AMBA_get_CPSR
AMBA_get_CPSR:
        MRS     R0, CPSR
        BX      LR

/*---------------------------------------------------------------------------*\
 *  Set CPSR (Current Program Status Register)
\*---------------------------------------------------------------------------*/
.globl AMBA_set_CPSR
AMBA_set_CPSR:
        MSR     CPSR_cxsf, R0
        BX      LR

/*---------------------------------------------------------------------------*\
 *  Hypervisor Call ( HVC )
\*---------------------------------------------------------------------------*/
.globl AmbaHvcCall
AmbaHvcCall:
        HVC     #0
        BX      LR

/*---------------------------------------------------------------------------*\
 *  Secure monitor Call ( SMC )
\*---------------------------------------------------------------------------*/
.globl AmbaSmcCallA32
AmbaSmcCallA32:
        SMC     #0
        BX      LR

BSS_START:      .word   __bss_start
BSS_END:        .word   __bss_end
IRQ_IsrAddr:    .word   __tx_irq_handler
FIQ_IsrAddr:    .word   __tx_fiq_handler
Excp_HndlAddr:  .word   AmbaExceptionHandler
_AmbaRtosCoreId: .word  AmbaRtosCoreId

