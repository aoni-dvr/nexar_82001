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
        .align 8
        .globl  AmbaLink_EntryPoint
        .globl  AmbaLinkProgramStart
#if defined (CONFIG_CPU_CORTEX_A76) && !defined(CONFIG_FWPROG_ATF_ENABLE)
AmbaLink_EntryPoint:
        B       AmbaLinkProgramStart
        B       .
#else
AmbaLink_EntryPoint:
        B       AmbaLink_ResetHandler
        B       .
#endif

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

/* void AmbaLink_BootLnxAsm(UINT64 dtb, UINT64 p1, UINT64 p2, UINT64 p3, void *lnx); */
.globl AmbaLink_BootLnxAsm
AmbaLink_BootLnxAsm:
        MSR     ELR_EL1, X4
        MOV     X4, #0x3C5
        MSR     SPSR_EL1, X4
        ERET

AmbaLinkProgramStart:
/*-----------------------------------------------------------------------------------------------*\
 *   low-level system initialization code.
 *
 *   After a reset execution starts here, the mode is ARM, supervisor
 *   with interrupts disabled.
\*-----------------------------------------------------------------------------------------------*/
        ldr x0, =0x02FAF080
        msr cntfrq_el0, x0
        mov    x0, #0x5B0
        msr    scr_el3, x0
        mov    x0, #0x3c5                  // DAIF + EL1h
        msr    spsr_el3, x0
        ldr    x0, =AmbaLink_ResetHandler
        msr    ELR_EL3, x0
        eret

AmbaLink_ResetHandler:
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
 *  X10 = Current Running Core-ID, bit [1:0]
\*---------------------------------------------------------------------------*/
        MRS     X10, MPIDR_EL1              /* Read Multiprocessor ID register */
        ANDS    X10, X10, #0x3              /* The possible values are: 0x0, 0x1, 0x2, 0x3. */

/*---------------------------------------------------------------------------*\
 *  setup stack
\*---------------------------------------------------------------------------*/
        MOVZ    X0, #1                      /* 1 = To prevent exception at accessing SP_EL0 and SP_ELx. */
        MSR     SPSel, X0                   /* Write to Stack Pointer Select Register */
        LDR     X0, =AmbaSysStackBaseSVC    /* X0 = pointer to the Stack pointer array */
        LSL     X1, X10, #3                 /* Pointer Offset = 0x0, 0x8, 0x10, 0x18 */
        LDR     X2, [X0, X1]                /* X2 = pointer to the End of Stack */
        MOV     SP, X2                      /* set stack pointer */

        B       AmbaLink_OtherCoreEntry
