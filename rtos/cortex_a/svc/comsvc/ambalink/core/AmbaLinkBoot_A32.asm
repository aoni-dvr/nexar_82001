/**
 *  @file AmbaBoot.asm
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
        .globl AmbaLink_EntryPoint

        .align 5                            /* Vector Base Address shall be 32-byte alignment */
AmbaLink_EntryPoint:                    /* Make this a DATA label, so that stack usage */
                                            /* analysis doesn't consider it an uncalled function */
        B       AmbaLink_ResetHandler       /* 0x00: Reset (RESET: Supervisor) */
        B       .
        B       .
        B       .
        B       .
        B       .                           /* 0x14: Reserved */
        B       .             				/* 0x18: IRQ */
        B       .             				/* 0x1c: FIQ */

/*---------------------------------------------------------------------------*\
 *  Initialise the processor
\*---------------------------------------------------------------------------*/
AmbaLink_ResetHandler:
        MOV    R0, #0
        MCR    p15, 0, R0, c1, c0, 0        /* SCTLR */
        LDR    R0, =AmbaLink_EntryPoint
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
        B       AmbaLink_OtherCoreEntry