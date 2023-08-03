/**
 *  @file AmbaBLD_PsciOnEntry_A64.asm
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
 *  @details Entry function for PSCI on.
 *
 */

#include "AmbaCortexA53.h"


        .text

        .global AmbaBLD_PsciOnEntry_SRTOS
        .global AmbaBLD_PsciOnEntry_Linux

/* X0: parameter passed from ATF; pointer of parameters. */
AmbaBLD_PsciOnEntry_SRTOS:
        /* Waiting parameters. */
        MOV     X9, X0
1:
        LDR     X10, [X9]
        CMP     X10, #0
        BEQ     1b
        /* Prepare SMC call */
        LDR     X0, [X9]
        LDR     X1, [X9, #8]
        LDR     X2, [X9, #16]
        LDR     X3, [X9, #24]
        SMC     #0
        /* Should not be here */
        B       .

/* X0: parameter passed from ATF; pointer of parameters. */
AmbaBLD_PsciOnEntry_Linux:
        /* Waiting parameters. */
        MOV     X9, X0
1:
        LDR     X10, [X9]
        CMP     X10, #0
        BEQ     1b
        /* Linux AArch64 parameters */
        LDR     X0, [X9, #8]
        LDR     X1, [X9, #16]
        LDR     X2, [X9, #24]
        LDR     X3, [X9, #32]

        /* Boot Entry */
        BR      X10
        /* Should not be here */
        B       .

