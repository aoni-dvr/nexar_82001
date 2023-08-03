/**
 *  @file AmbaMisra_A32.asm
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
        .arm

/*
 *  Touch unused argument
 */
.globl AmbaMisra_TouchUnused
AmbaMisra_TouchUnused:
        BX      LR

/*
 *  Data Type Casting (32-bit)
 */
.globl AmbaMisra_VolatileCast32
AmbaMisra_VolatileCast32:
.globl AmbaMisra_TypeCast32
AmbaMisra_TypeCast32:
        LDR     R3, [R1]
        STR     R3, [R0]
        BX      LR

/*
 *  Data Type Casting (64-bit)
 */
.globl AmbaMisra_TypeCast64
AmbaMisra_TypeCast64:
        LDRD    R2, R3, [R1]
        STRD    R2, R3, [R0]
        BX      LR

/*
 *  Delay CPU cycles
 */
.globl AmbaDelayCycles
AmbaDelayCycles:
        SUBS    R0, R0, #1
        BNE     AmbaDelayCycles
        BX      LR

/*
 *  Assertion trap
 */
.globl AmbaAssert
AmbaAssert:
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        /* save LR into ScratchPad RAM + 0x1FFC */
        LDR     R1, =#0xE0031FFC
        STR     LR, [R1]
        /* save CORE ID into AMBA_CA53_SRAM_BASE_ADDR + 0x1FF8 */
        SUB     R1, R1, #4
        MRC     15, 0, R0, CR0, CR0, 5 /* read MPIDR */
        AND     R0, R0, #0x03
        ADD     R0, R0, #0x1000
        STR     R0, [R1]
#endif
loop_start:
        CPSID   if
        SEV
        WFE
        WFE
        B       loop_start

