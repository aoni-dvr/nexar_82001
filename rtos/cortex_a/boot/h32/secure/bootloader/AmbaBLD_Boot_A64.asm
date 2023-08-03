/**
 *  @file AmbaBLD_Boot_A64.asm
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Cortex-A53 Boot Code for Bootloader
 *
 */

#include "AmbaCortexA53.h"

        .text

        .global __vector
        .global _AmbaResetHandler

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

__vector:                                   /* Make this a DATA label, so that stack usage analysis does not consider it an uncalled function */
/**
 *  Initialise the processor
 */
_AmbaResetHandler:
        /*
         * Could be EL3/EL2/EL1, Initial State:
         * Little Endian, MMU Disabled, i/dCache Disabled
         */
        IC      IALLU                       /* I+BTB cache invalidate */
        TLBI    VMALLE1IS                   /* invalidate I + D TLBs */
        DSB     ISH                         /* Ensure visibility of the data stored */
        MSR     MDSCR_EL1, XZR              /* Reset MDSCR_EL1 */

        /* Disable all traps, so we do not get any nasty surprise */
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
        MSR     IFSR32_EL2, XZR             /* Clear IFSR32 */
        MSR     CNTVOFF_EL2, XZR            /* Clear CNTVOFF_EL2 */
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
/**
 *  X10 = Current Running Core-ID, bit [1:0]
 */
        MRS     X10, MPIDR_EL1              /* Read Multiprocessor ID register */
        ANDS    X10, X10, #0x3              /* The possible values are: 0x0, 0x1, 0x2, 0x3. */

#if defined(CONFIG_BLD_SEQ_SRTOS) && defined(CONFIG_ATF_HAVE_BL2) && defined(CONFIG_ATF_SPD_OPTEE)
        /* If have op-tee, ensure RTOS will NOT run at core-0 */
        CMP     X10, #CONFIG_BOOT_CORE_SRTOS
        B.NE    BootCore0
        /* LDR     X0, =AMBA_SIP_BOOT_RTOS */
        LDR     X0, =0x8200FF04
        LDR     X1, =CONFIG_FWPROG_SYS_LOADADDR
        LDR     X2, =0
        /* LDR     X3, =S_SWITCH_AARCH32 */
        LDR     X3, =2
        LDR     X4, =0
        SMC     #0
        /* Should not return to here */
        B       .

BootCore0:
#endif // #if defined(CONFIG_BLD_SEQ_SRTOS) && defined(CONFIG_ATF_HAVE_BL2)

/**
 *  setup stack
 */
        MOVZ    X0, #1                      /* 1 = To prevent exception at accessing SP_EL0 and SP_ELx. */
        MSR     SPSel, X0                   /* Write to Stack Pointer Select Register */
        LDR     X0, =pAmbaHwStack           /* X0 = pointer to the Stack pointer array */
        LSL     X1, X10, #3                 /* Pointer Offset = 0x0, 0x8, 0x10, 0x18 */
        LDR     X2, [X0, X1]                /* X2 = pointer to the End of Stack */
        MOV     SP, X2                      /* set stack pointer */

        B.EQ    ContinueCore0Init           /* Continue Core-0 initializations */

/**
 *  Add more initialization here
 */

ContinueCore0Init:                          /* Continue Core-0 initializations */
/**
 *  Add more initialization here
 */
        /* Configure NIC */
        LDR     W0, =0xf1000000
        LDR     W1, =0xf2000000
        MOV     W2, #0x3
        MOV     W3, #0x0
        STR     W2, [X0, #0x08]
        STR     W2, [X0, #0x0c]
        STR     W2, [X0, #0x10]
        STR     W2, [X0, #0x14]
        STR     W3, [X1, #0x90]
        STR     W3, [X1, #0x94]
        STR     W3, [X1, #0x98]
/**
 *  C initialization here
 */
        LDR     W0, BSS_START
        LDR     W1, BSS_END
        BIC     X1, X1, #0x3
        ADD     X1, X1, #4

        MOV     X3, #0
        MOV     X4, #0
BssZeroInit:
        STP     X3, X4, [X0], #16
        SUBS    X2, X1, X0
        CMP     X2, #16
        B.GE    BssZeroInit

        CMP     X2, #0
        B.EQ    BssInitDone

BssZeroInitRemain:
        STR     W3, [X0], #4
        SUBS    X2, X2, #4
        B.GT    BssZeroInitRemain

BssInitDone:
/*
 *  Continue to __cmain for C-level initialization
 */
        BL      main
        B       .

/*
 *  BLD stop here
 */
.globl BldStop
BldStop:
        B       BldStop

/*---------------------------------------------------------------------------*\
 *  Secure monitor Call ( SMC )
\*---------------------------------------------------------------------------*/
.globl AmbaSmcCall64
AmbaSmcCall64:
        SMC     #0
        RET

/*---------------------------------------------------------------------------*\
 *  Boot Linux
 *  void AmbaBootLinux(UINT64 p0, UINT64 p1, UINT64 p2, UINT64 p3, void *addr);
\*---------------------------------------------------------------------------*/
.globl AmbaBootLinux
AmbaBootLinux:
        MSR     ELR_EL1, X4
        MOV     X4, #0x3C5
        MSR     SPSR_EL1, X4
        ERET

BSS_START:          .word   __bss_start
BSS_END:            .word   __bss_end
