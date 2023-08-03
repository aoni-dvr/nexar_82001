/**
 *  @file AmbaMisra_A64.asm
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
 *  @details for MISRA Compliance
 *
 */

        .text

/*
 *  Touch unused argument
 */
.globl AmbaMisra_TouchUnused
AmbaMisra_TouchUnused:
        RET

/*
 *  Data Type Casting (32-bit)
 */
.globl AmbaMisra_VolatileCast32
AmbaMisra_VolatileCast32:
.globl AmbaMisra_TypeCast32
AmbaMisra_TypeCast32:
        LDR     W1, [X1]
        STR     W1, [X0]
        RET

/*
 *  Data Type Casting (64-bit)
 */
.globl AmbaMisra_TypeCast64
AmbaMisra_TypeCast64:
        LDR     X1, [X1]
        STR     X1, [X0]
        RET

/*
 *  Delay CPU cycles
 */
.globl AmbaDelayCycles
AmbaDelayCycles:
        SUBS    W0, W0, #1
        B.NE    AmbaDelayCycles
        RET

/*
 *  Assertion trap
 */
.globl AmbaAssert
AmbaAssert:
        MSR     DAIFSET, #3
        SEV
        WFE
        WFE
        B       AmbaAssert

