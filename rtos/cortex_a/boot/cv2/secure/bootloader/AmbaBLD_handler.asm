/**
 *  @file AmbaBLD_handler.asm
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
 *  @details Cortex-A53 Boot Code for system
 */

#include "AmbaCortexA53.h"

        .text

        .global AmbaBLD_handler
        .global ExceptionEntry

/*
 * Enter Exception.
 * This will save the processor state that is ELR/X0~X30
 * to the stack frame.
 */
.macro  ExceptionEntry
#if 0
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

        /* Could be running at EL3/EL2/EL1 */
        SwitchEL X11, 3f, 2f, 1f
3:      MRS     X1, ESR_EL3
        MRS     X2, ELR_EL3
        B       0f
2:      MRS     X1, ESR_EL2
        MRS     X2, ELR_EL2
        B       0f
1:      MRS     X1, ESR_EL1
        MRS     X2, ELR_EL1
0:      STP     X2, X1, [SP, #-16]!
#endif
.endm

#if 0
        .section ExceptionTable
#endif
/*-----------------------------------------------------------------------------------------------*\
 *   Default exception handlers
\*-----------------------------------------------------------------------------------------------*/
        .align  11          /* Vector Base Address shall be 2048-byte alignment */
AmbaBLD_handler:
        .align  7           /* 0x000 */
Synchronous_Handler_SP_EL0: /* Current EL Synchronous Exception with SP_EL0 */
        ExceptionEntry
        B       .

        .align  7           /* 0x080 */
IRQ_Handler_SP_EL0:         /* Current EL IRQ Exception with SP_EL0 */
        STP     X29, X30, [SP, #-16]!
#if 1
        b .
#else
        LDR     W30, IRQ_IsrAddr
#endif
        BR      X30

        .align  7           /* 0x100 */
FIQ_Handler_SP_EL0:         /* Current EL FIQ Exception with SP_EL0 */
        STP     X29, X30, [SP, #-16]!
#if 1
        b .
#else
        LDR     W30, FIQ_IsrAddr
#endif
        BR      X30

        .align  7           /* 0x180 */
System_Error_Handler_SP_EL0:/* Current EL System Error Exception with SP_EL0 */
        ExceptionEntry
        B       .

        .align  7           /* 0x200 */
Synchronous_Handler_SP_ELX: /* Current EL Synchronous Exception with SP_ELx, x > 0 */
        ExceptionEntry
        B       .

        .align  7           /* 0x280 */
IRQ_Handler_SP_ELX:         /* Current EL IRQ Exception with SP_ELx, x > 0 */
        STP     X29, X30, [SP, #-16]!
#if 1
        b .
#else
        LDR     W30, IRQ_IsrAddr
#endif
        BR      X30

        .align  7           /* 0x300 */
FIQ_Handler_SP_ELX:         /* Current EL FIQ Exception with SP_ELx, x > 0 */
        STP     X29, X30, [SP, #-16]!
#if 1
        b .
#else
        LDR     W30, FIQ_IsrAddr
#endif
        BR      X30

        .align  7           /* 0x380 */
System_Error_Handler_SP_ELX:/* Current EL System Error Exception with SP_ELx, x > 0 */
        ExceptionEntry
        eret
#if 0
        B       .
#endif

        .align  7           /* 0x400 */

