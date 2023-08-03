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
 *  @details Cortex-A76 Boot Code for Bootloader
 *
 */

#include "AmbaCortexA76.h"

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
        // 4 NOP for UST header
        NOP
        NOP
        NOP
        NOP
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
/*
DBGPOINTA:
        B       DBGPOINTA
        NOP
*/

        /* Disable all traps, so we do not get any nasty surprise */
        SwitchEL X1, 3f, 2f, 1f
3:      MSR     ELR_EL3, XZR                /* Clear ELR */
        MSR     ESR_EL3, XZR                /* Clear ESR */
        MSR     FAR_EL3, XZR                /* Clear FAR */
        MRS     X0, SCR_EL3
        ORR     X0, X0, #0xF                /* SCR_EL3.EA|IRQ|FIQ|NS */
        //ORR     X0, X0, #0xE                /* SCR_EL3.EA|IRQ|FIQ */
        //BIC     X0, X0, #0x1                /* Force clear NS */
        MSR     SCR_EL3, X0
        MSR     CPTR_EL3, XZR               /* Enable FP/SIMD */
2:      MSR     ELR_EL2, XZR                /* Clear ELR */
        MSR     ESR_EL2, XZR                /* Clear ESR */
        MSR     FAR_EL2, XZR                /* Clear FAR */
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
        UBFX    X10, X10, #8, #8

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

        CMP     X10, #0
        B.EQ    ContinueCore0Init           /* Continue Core-0 initializations */

        /* spin-table start */
        mov	x0, #8
secondary_core_wait:
        wfe
        isb
        dsb	sy
        adr	x1, cpu_release_addr
        ldr	w29, [x1, x0]
        cbz	w29, secondary_core_wait
        ldr	w2, =0xdeadbeaf
        cmp	w29, w2
        beq	secondary_core_wait
        /* spin-table end */
        ldr x0, =0x02FAF080
        msr cntfrq_el0, x0

        bl	__secondary_gic_init

        bl	__switch_to_el2
        bl	__switch_to_el1		/* jump to Kernel, entry is stroed in w29 */

	/* never run here */
	b .

/**
 *  Add more initialization here
 */

ContinueCore0Init:                          /* Continue Core-0 initializations */
/**
 *  Add more initialization here
 */
        /* Configure NIC */
        LDR     X1, =0x20f1000000U
        MOV     W3, #0x1
        STR     W3, [X1, #0x18]
        STR     W3, [X1, #0x1c]
        STR     W3, [X1, #0x20]
        STR     W3, [X1, #0x24]
        STR     W3, [X1, #0x28]

        LDR     X1, =0x20f2000000U
        MOV     W3, #0x0
        STR     W3, [X1, #0x90]
        STR     W3, [X1, #0x94]
        STR     W3, [X1, #0x98]

        /* Power down MIPI DPHY */
        LDR     X1, =0x20ed200000U
        MOV     W3, #0x6
        STR     W3, [X1, #0x020]

        /* Power down SLVS-EC 12 Lanes */
        LDR     X1, =0x20ed200000U
        MOV     W3, #0x5c03
        MOVK    W3, #0x0d2b, LSL #16
        STR     W3, [X1, #0x0e8]
        STR     W3, [X1, #0x124]
        STR     W3, [X1, #0x160]
        STR     W3, [X1, #0x19c]
        STR     W3, [X1, #0x1d8]
        STR     W3, [X1, #0x214]
        STR     W3, [X1, #0x250]
        STR     W3, [X1, #0x28c]
        STR     W3, [X1, #0x2c8]
        STR     W3, [X1, #0x304]
        STR     W3, [X1, #0x340]
        STR     W3, [X1, #0x37c]
        MOV     W3, #0x79a8
        MOVK    W3, #0x0241, LSL #16
        STR     W3, [X1, #0x0ec]
        STR     W3, [X1, #0x128]
        STR     W3, [X1, #0x164]
        STR     W3, [X1, #0x1a0]
        STR     W3, [X1, #0x1dc]
        STR     W3, [X1, #0x218]
        STR     W3, [X1, #0x254]
        STR     W3, [X1, #0x290]
        STR     W3, [X1, #0x2cc]
        STR     W3, [X1, #0x308]
        STR     W3, [X1, #0x344]
        STR     W3, [X1, #0x380]

        /* Power down DAC */
        LDR     X1, =0x20ed080000U
        MOV     W3, #0x0020
        MOVK    W3, #0x1200, LSL #16
        STR     W3, [X1, #0x1a8]
        MOV     W3, #0x0000
        MOVK    W3, #0x4200, LSL #16
        STR     W3, [X1, #0x688]

        /* Power down HDMI */
        LDR     X1, =0x20ed080000U
        MOV     W3, #0x0020
        MOVK    W3, #0x1200, LSL #16
        STR     W3, [X1, #0x1a8]
        MOV     W3, #0x0020
        MOVK    W3, #0x1200, LSL #16
        STR     W3, [X1, #0x404]
        MOV     W3, #0x0
        STR     W3, [X1, #0x7f0]

        /* Power down MIPI DSI A/B */
        LDR     X1, =0x20ed080000U
        MOV     W3, #0x0
        STR     W3, [X1, #0x59c]
        MOV     W3, #0x0010
        MOVK    W3, #0x5000, LSL #16
        STR     W3, [X1, #0x584]
        MOV     W3, #0x0000
        MOVK    W3, #0x3000, LSL #16
        STR     W3, [X1, #0x58c]
        MOV     W3, #0x0
        STR     W3, [X1, #0x5cc]
        MOV     W3, #0x0010
        MOVK    W3, #0x5000, LSL #16
        STR     W3, [X1, #0x5b4]
        MOV     W3, #0x0000
        MOVK    W3, #0x3000, LSL #16
        STR     W3, [X1, #0x5bc]

/**
 *  C initialization here
 */
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
/*
 *  Continue to __cmain for C-level initialization
 */
        BL      main
        B       .

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

#define CPTR_EL2_VAL    (3 << 12 | 0x3FF)

/*---------------------------------------------------------------------------*\
 *  Boot Linux
 *  void AmbaBootLinux(UINT64 p0, UINT64 p1, UINT64 p2, UINT64 p3, void *addr);
\*---------------------------------------------------------------------------*/
.globl AmbaBootLinux
AmbaBootLinux:
#if 1
        /* Disable coprocessor traps to EL3 */
        MSR     CPTR_EL3, XZR
        /* Disable coprocessor traps to EL2 */
        LDR     X5, =CPTR_EL2_VAL
        MSR     CPTR_EL2, X5
        /* Initialize Generic Timers */
        MSR     CNTVOFF_EL2, XZR
        /* Initialize SCTLR_EL2 */
        LDR     X5, =0x0000000030C50830
        MSR     SCTLR_EL2, X5

        MOV     X5, SP
        MSR     SP_EL2, X5

        MRS     X5, VBAR_EL3
        MSR     VBAR_EL2, X5

#if 1
        LDR     X5, =0x00000000000005B1
#else
        LDR     X5, =0x0000000000000531
#endif
        MSR     SCR_EL3, X5

        /* Return to the EL2_SP2 mode from EL3 */
        MSR     ELR_EL3, X4
        MOV     X4, #0x3C9
        MSR     SPSR_EL3, X4
        ERET
#else
#if 0
        MSR     ELR_EL1, X4
        MOV     X4, #0x3C5
        MSR     SPSR_EL1, X4
        ERET
#else
        MOV     X30, X4
        RET
#endif
#endif

.align  3
BSS_START:          .dword   __bss_start
BSS_END:            .dword   __bss_end

.globl cpu_release_addr
cpu_release_addr:
.rept 4
	.quad	0xdeadbeafdeadbeaf
.endr
