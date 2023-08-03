/**
 *  @file AmbaBoot.asm
 *
 *  @copyright Copyright (c) 2017 Ambarella, Inc.
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

#if defined (CONFIG_CPU_CORTEX_A76)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif

        .text
        .globl __vector
        .globl  vectors
        .globl _AmbaResetHandler
        .globl __AmbaProgramStart

/*
 * Branch according to exception level
 */
.macro  SwitchEL, xreg, el3_label, el2_label, el1_label
        MRS     \xreg, CurrentEL
        CMP     \xreg, 0xc
        B.EQ    \el3_label
        CMP     \xreg, 0x8
        B.EQ    \el2_label
        CMP     \xreg, 0x4
        B.EQ    \el1_label
.endm

/*
 * Enter Exception.
 * This will save the processor state that is ELR/X0~X30
 * to the stack frame.
 */
.macro  ExceptionEntry
        STP     X29, X30, [SP, #-16]!
        STP     X27, X28, [SP, #-16]!
        STP     X25, X26, [SP, #-16]!
        STP     X23, X24, [SP, #-16]!
        STP     X21, X22, [SP, #-16]!
        STP     X19, X20, [SP, #-16]!
        STP     X17, X18, [SP, #-16]!
        STP     X15, X16, [SP, #-16]!
        STP     X13, X14, [SP, #-16]!
        STP     X11, X12, [SP, #-16]!
        STP     X9, X10, [SP, #-16]!
        STP     X7, X8, [SP, #-16]!
        STP     X5, X6, [SP, #-16]!
        STP     X3, X4, [SP, #-16]!
        STP     X1, X2, [SP, #-16]!
        ADD     X1, SP, #0xF0
        STP     X0, X1, [SP, #-16]!
        MRS     X1, ESR_EL1
        MRS     X2, ELR_EL1
        STP     X2, X1, [SP, #-16]!
.endm

.macro  SaveCallerContext
        SUB     SP, SP, #0x20
        STP     X28, X29, [SP, #-16]!
        STP     X26, X27, [SP, #-16]!
        STP     X24, X25, [SP, #-16]!
        STP     X22, X23, [SP, #-16]!
        STP     X20, X21, [SP, #-16]!
        STP     X18, X19, [SP, #-16]!
        STP     X16, X17, [SP, #-16]!
        STP     X14, X15, [SP, #-16]!
        STP     X12, X13, [SP, #-16]!
        STP     X10, X11, [SP, #-16]!
        STP     X8, X9, [SP, #-16]!
        STP     X6, X7, [SP, #-16]!
        STP     X4, X5, [SP, #-16]!
        STP     X2, X3, [SP, #-16]!
        STP     X0, X1, [SP, #-16]!
        ADD     X21, SP, #0x110
        MRS     X22, ELR_EL2
        MRS     X23, SPSR_EL2
        STP     LR, X21, [SP, #0xF0]
        STP     X22, X23, [SP, #0x100]
.endm

.macro  RestoreCallerContext
        LDP     X21, X22, [SP, #0x100]
        MSR     ELR_EL2, X21
        MSR     SPSR_EL2, X22
        LDP     X0, X1, [SP], #16
        LDP     X2, X3, [SP], #16
        LDP     X4, X5, [SP], #16
        LDP     X6, X7, [SP], #16
        LDP     X8, X9, [SP], #16
        LDP     X10, X11, [SP], #16
        LDP     X12, X13, [SP], #16
        LDP     X14, X15, [SP], #16
        LDP     X16, X17, [SP], #16
        LDP     X18, X19, [SP], #16
        LDP     X20, X21, [SP], #16
        LDP     X22, X23, [SP], #16
        LDP     X24, X25, [SP], #16
        LDP     X26, X27, [SP], #16
        LDP     X28, X29, [SP], #16
        LDP     LR, X24, [SP]
        MOV     SP, X24
.endm

__vector:                                   /* Make this a DATA label, so that stack usage analysis does not consider it an uncalled function */
        SwitchEL X1, 3f, 2f, 1f
3:      b       __AmbaProgramStart
2:      b       .
1:      b       _AmbaResetHandler
        b       .

/*-----------------------------------------------------------------------------------------------*\
 *   low-level system initialization code.
 *
 *   After a reset execution starts here, the mode is ARM, supervisor
 *   with interrupts disabled.
\*-----------------------------------------------------------------------------------------------*/
__AmbaProgramStart:
        mov    x0, #0x5B0
        msr    scr_el3, x0
        mov    x0, #0x3c5                  // DAIF + EL1h
        msr    spsr_el3, x0
        ldr    x0, =_AmbaResetHandler
        msr    ELR_EL3, x0
        eret

/*---------------------------------------------------------------------------*\
 *  Initialise the processor
\*---------------------------------------------------------------------------*/
_AmbaResetHandler:
/*---------------------------------------------------------------------------*\
 *  Initialize Architectural Features (Floating-point, Advanced SIMD).
\*---------------------------------------------------------------------------*/
        SwitchEL X1, 3f, 2f, 1f
3:      MSR     ELR_EL3, XZR                /* Clear ELR */
        MSR     ESR_EL3, XZR                /* Clear ESR */
        MSR     FAR_EL3, XZR                /* Clear FAR */
        MRS     X0, SCR_EL3
        ORR     X0, X0, #0xF                /* SCR_EL3.EA|IRQ|FIQ|NS */
        MSR     SCR_EL3, X0
        MSR     CPTR_EL3, XZR               /* Enable FP/SIMD */
2:      MSR     ELR_EL2, XZR                /* Clear ELR */
        MSR     ESR_EL2, XZR                /* Clear ESR */
        MSR     FAR_EL2, XZR                /* Clear FAR */
        MRS     X0, HCR_EL2
        ORR     X0, X0, #0x38               /* HCR_EL2.AMO|IMO|FMO */
        MSR     HCR_EL2, X0
        MOV     X0, #0x33FF
        MSR     CPTR_EL2, X0                /* Enable FP/SIMD */
1:      MSR     ELR_EL1, XZR                /* Clear ELR */
        MSR     ESR_EL1, XZR                /* Clear ESR */
        MSR     FAR_EL1, XZR                /* Clear FAR */
        MOV     X0, #(3 << 20)
        MSR     CPACR_EL1, X0               /* Enable FP/SIMD */
/*---------------------------------------------------------------------------*\
 *  X10 = Current Running Core-ID, bit[10:8] for CV5
\*---------------------------------------------------------------------------*/
        /* cache flush flow, reference from ArmV8 TRM */
        MRS X0, CLIDR_EL1
        AND W3, W0, #0x07000000
        LSR W3, W3, #23
        CBZ W3, Finished
        MOV W10, #0
        MOV W8, #1
Loop1:
        ADD W2, W10, W10, LSR #1
        LSR W1, W0, W2
        AND W1, W1, #0x7
        CMP W1, #2
        B.LT Skip
        MSR CSSELR_EL1, X10
        ISB
        MRS X1, CCSIDR_EL1
        AND W2, W1, #7
        ADD W2, W2, #4
        UBFX W4, W1, #3, #10
        CLZ W5, W4
        LSL W9, W4, W5
        LSL W16, W8, W5
Loop2:
        UBFX W7, W1, #13, #15
        LSL W7, W7, W2
        LSL W17, W8, W2
Loop3:
        ORR W11, W10, W9
        ORR W11, W11, W7
        DC CSW, X11
        SUBS W7, W7, W17
        B.GE Loop3
        SUBS X9, X9, X16
        B.GE Loop2
        Skip: ADD W10, W10, #2
        CMP W3, W10
        dsb ish
        B.GT Loop1

Finished:

        MRS     X10, MPIDR_EL1              /* Read Multiprocessor ID register */
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)

        LSR     X10, X10, #8
        ANDS    X10, X10, #0x7              /* The possible values are: 0x0, 0x1. */
#else
        ANDS    X10, X10, #0x3              /* The possible values are: 0x0, 0x1, 0x2, 0x3. */
#endif
/*---------------------------------------------------------------------------*\
 *  setup stack
\*---------------------------------------------------------------------------*/
        MOVZ    X0, #1                      /* 1 = To prevent exception at accessing SP_EL0 and SP_ELx. */
        MSR     SPSel, X0                   /* Write to Stack Pointer Select Register */
        LDR     X0, =AmbaSysStackBaseSVC    /* X0 = pointer to the Stack pointer array */
        LSL     X1, X10, #3                 /* Pointer Offset = 0x0, 0x8, 0x10, 0x18 */
        LDR     X2, [X0, X1]                /* X2 = pointer to the End of Stack */
        MOV     SP, X2                      /* set stack pointer */

        CMP     X10, #0
        B.EQ    ContinueCore0Init           /* Continue Core-0 initializations */

#if !defined(CONFIG_SOC_CV5) &&  !defined (CONFIG_SOC_CV52)
        /* LDR     X0, =AMBA_SIP_BOOT_RTOS */
        LDR     X0, =0x8200FF04
        LDR     X1, =CONFIG_FWPROG_SYS_LOADADDR
        LDR     X2, =0
        /* LDR     X3, =S_SWITCH_AARCH64 */
        LDR     X3, =3
        LDR     X4, =0
        SMC     #0
#endif

        B       AmbaOtherCoreEntry

ContinueCore0Init:                          /* Continue Core-0 initializations */
/*---------------------------------------------------------------------------*\
 *  Add more initialization here
\*---------------------------------------------------------------------------*/
#if defined(CONFIG_XEN_SUPPORT) && !defined(AMBA_FWPROG)
        ldr     x0, =__xen_device_tree      /* Location to store phy. location of device_tree */
        ldr     x0, [x0]                    /* Phy. location of device tree. */
        // Check magic
        ldr     w1, [x0]
        ldr     w2, =0xEDFE0DD0             /* Magic number 0xd00dfeed (big-endian). */
        cmp     w1, w2
        bne     4f
        // Check length
        ldr     w1, [x0, #4]
        rev32   x1, x1
        ldr     x2, =__dts_start            /* VM location of device tree */
        ldr     x3, =__dts_end
        sub     x3, x3, x2
        cmp     x1, x3
        bgt     4f
        // Clone dtb to VM
        mov     x3, #0x0
clone_dtb:
        ldr     x4, [x0], #8
        str     x4, [x2], #8
        ldr     x4, [x0], #8
        str     x4, [x2], #8
        add     x3, x3, #16
        cmp     x3, x1
        blt     clone_dtb

4:
        /* Simple MMU setting */
        bl      AmbaMMU_SetupXenInit

#endif /* defined(CONFIG_XEN_SUPPORT) */

/*---------------------------------------------------------------------------*\
 *  C initialization here
\*---------------------------------------------------------------------------*/
        LDR     x0, BSS_START
        LDR     x1, BSS_END
        BIC     X1, X1, #0x3
        ADD     X1, X1, #4

        LDR     x3, =0x0
        LDR     x4, =0x0
BssZeroInit:
        STP     x3, x4, [X0], #16
        STP     x3, x4, [X0], #16
        STP     x3, x4, [X0], #16
        STP     x3, x4, [X0], #16
        STP     x3, x4, [X0], #16
        STP     x3, x4, [X0], #16
        STP     x3, x4, [X0], #16
        STP     x3, x4, [X0], #16
        SUBS    X2, X1, X0
        CMP     X2, #128
        B.GE    BssZeroInit

        CMP     X2, #0
        B.EQ    BssInitDone

BssZeroInitRemain:
        STR     W3, [X0], #4
        SUBS    X2, X2, #4
        B.GT    BssZeroInitRemain

BssInitDone:
/*---------------------------------------------------------------------------*\
 *  Pre-initialize ThreadX before main and after BSS/DATA sections are intialized.
\*---------------------------------------------------------------------------*/
#if defined(CONFIG_XEN_SUPPORT) && !defined(AMBA_FWPROG)
        /* Xen Setup */
        ldr     x2, =__xen_physical_addr_offset
        ldr     x0, [x2]

        bl      amba_xen_setup
#endif /* defined(CONFIG_XEN_SUPPORT) */

        BL      AmbaMMU_SetupPrimaryPreOSInit

        BL      _tx_initialize_kernel_setup

/*---------------------------------------------------------------------------*\
 *  Continue to __cmain for C-level initialization
\*---------------------------------------------------------------------------*/
        B       main


        .section ExceptionTable
/*-----------------------------------------------------------------------------------------------*\
 *   Default exception handlers
\*-----------------------------------------------------------------------------------------------*/
        .align  11          /* Vector Base Address shall be 2048-byte alignment */
vectors:
        .align  7           /* 0x000 */
Synchronous_Handler_SP_EL0: /* Current EL Synchronous Exception with SP_EL0 */
        ExceptionEntry
        MOV     X0, #0
        MOV     X1, SP
        LDR     X30, Excp_HndlAddr
        BR      X30
        B       .

        .align  7           /* 0x080 */
IRQ_Handler_SP_EL0:         /* Current EL IRQ Exception with SP_EL0 */
        STP     X29, X30, [SP, #-16]!
        LDR     X30, IRQ_IsrAddr
        BR      X30

        .align  7           /* 0x100 */
FIQ_Handler_SP_EL0:         /* Current EL FIQ Exception with SP_EL0 */
        STP     X29, X30, [SP, #-16]!
        LDR     X30, FIQ_IsrAddr
        BR      X30

        .align  7           /* 0x180 */
System_Error_Handler_SP_EL0:/* Current EL System Error Exception with SP_EL0 */
        ExceptionEntry
        MOV     X0, #1
        MOV     X1, SP
        LDR     X30, Excp_HndlAddr
        BR      X30
        B       .

        .align  7           /* 0x200 */
Synchronous_Handler_SP_ELX: /* Current EL Synchronous Exception with SP_ELx, x > 0 */
        ExceptionEntry
        MOV     X0, #2
        MOV     X1, SP
        LDR     X30, Excp_HndlAddr
        BR      X30
        B       .

        .align  7           /* 0x280 */
IRQ_Handler_SP_ELX:         /* Current EL IRQ Exception with SP_ELx, x > 0 */
        STP     X29, X30, [SP, #-16]!
        LDR     X30, IRQ_IsrAddr
        BR      X30

        .align  7           /* 0x300 */
FIQ_Handler_SP_ELX:         /* Current EL FIQ Exception with SP_ELx, x > 0 */
        STP     X29, X30, [SP, #-16]!
        LDR     X30, FIQ_IsrAddr
        BR      X30

        .align  7           /* 0x380 */
System_Error_Handler_SP_ELX:/* Current EL System Error Exception with SP_ELx, x > 0 */
        ExceptionEntry
        MOV     X0, #3
        MOV     X1, SP
        LDR     X30, Excp_HndlAddr
        BR      X30
        B       .

        .align  7           /* 0x400 */

Synchronous_Handler_SP_Lower:
        ExceptionEntry
        MOV     X0, #2
        MOV     X1, SP
        LDR     X30, Excp_HndlAddr
        BR      X30
        B       .

        .align  7           /* 0x480 */
IRQ_Handler_SP_Lower:
        STP     X29, X30, [SP, #-16]!
        LDR     X30, IRQ_IsrAddr
        BR      X30

        .align  7           /* 0x500 */
FIQ_Handler_SP_Lower:
        STP     X29, X30, [SP, #-16]!
        LDR     X30, FIQ_IsrAddr
        BR      X30

        .align  7           /* 0x580 */
System_Error_Handler_SP_Lower:
        ExceptionEntry
        MOV     X0, #3
        MOV     X1, SP
        LDR     X30, Excp_HndlAddr
        BR      X30
        B       .
/*---------------------------------------------------------------------------*\
 *  Secure monitor Call ( SMC )
\*---------------------------------------------------------------------------*/
.globl AmbaSmcCall
AmbaSmcCall:
        SMC     #0
        RET

.align  3
BSS_START:      .dword   __bss_start
BSS_END:        .dword   __bss_end
Excp_HndlAddr:  .dword   AmbaExceptionHandler
IRQ_IsrAddr:    .dword   __tx_irq_handler
FIQ_IsrAddr:    .dword   __tx_fiq_handler

