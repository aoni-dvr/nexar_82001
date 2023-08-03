/**
 *  @file AmbaDramAssembly.asm
 *
 *  Copyright (c) 2022 Ambarella International LP
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
 *  @details Dram Enter/Exit SR Assembly Functions
 *
 */

.global ambarella_dram_enter_sr
.global ambarella_dram_exit_sr

.macro trigger_uinst
    STR W1, [X21, #0xac]    /* DRAM_UINST1 */
    STR W2, [X21, #0xb0]    /* DRAM_UINST2 */
    STR W4, [X21, #0xb8]    /* DRAM_UINST4 */
    MOV W0, #1
    STR W0, [X21, #0xbc]    /* DRAM_UINST5 */
.endm

.macro wait_complete, op, bit, label
    tbz W10, #1, 100f
    \op W0, \bit, \label
100:    tbz W10, #2, 101f
    \op W1, \bit, \label
101:
.endm

.align  3
ambarella_dram_enter_sr:
    STP     x2, x4, [sp, #-16]!
    STP     X20, X21, [sp, #-16]!
    STP     X22, X23, [sp, #-16]!
    STP     X25, x30, [sp, #-16]!

    MOVZ X20, #0x0000
    MOVK X20, #0x0000,lsl #0x10
    MOVK X20, #0x10,lsl #0x20
    /*LDR X20, =(0x1000000000)*/
    MOVZ X25, #0x0000
    MOVK X25, #0xed08,lsl #0x10
    MOVK X25, #0x20,lsl #0x20
    /*LDR X25, =(0X20ed080000)*/

    add X21, X20, #0x4000   /* broadcast base register */
    add X22, X20, #0x5000   /* ddrc0 base regsiter */
    add X23, X20, #0x6000   /* ddrc1 base regsiter */

    LDR W10, [X20]      /* used for checking host num */

    /* disable DRAM access */
    LDR W0, [X21]
    BIC W0, W0, #(1 << 0)
    STR W0, [X21]

    /* wait for DDRC idle */
1:  LDR W0, [X22, #0x3a4]
    LDR W1, [X23, #0x3a4]
    wait_complete tbz, #2, 1b

    /* wait for 8*tRFC to allow refresh credit to be maxed out */
    MOV W0, #0x1
    STR W0, [X25, #0X258]
    STR WZR, [X25, #0X258]
2:  LDR W0, [X25, #0X254]
    CMP W0, #240    /* 10us */
    blo 2b

    /* disable auto-refresh */
    LDR W0, [X21]
    BIC W0, W0, #(1 << 1)
    STR W0, [X21]

    /* disable LP5 background ZQ calibration */
    MOVZ W0, #0x6
    MOVK W0, #0x9f1c,lsl #0x10
    STR W0, [X21, #0x30]
    dsb sy
    isb
    /* wait for complete */
3:  LDR W0, [X22, #0x30]
    LDR W1, [X23, #0x30]
    wait_complete tbnz, #31, 3b

    /* turnoff WCK Buffer in LP5 device */
    MOV W1, #0X0000000d
    MOV W2, #0X0000007c
    MOV W4, #0X00000303
    trigger_uinst
    /* wait for complete */
4:  LDR W0, [X22, #0xbc]
    LDR W1, [X23, #0xbc]
    wait_complete tbnz, #0, 4b

    /* issue SRE to enter into SR */
    MOV W1, #0X0000000d
    MOV W2, #0X00004068
    MOV W4, #0X00000303
    trigger_uinst
    /* wait for complete */
5:  LDR W0, [X22, #0xbc]
    LDR W1, [X23, #0xbc]
    wait_complete tbnz, #0, 5b

    LDP     X25, x30, [sp], #16
    LDP     X22, X23, [sp], #16
    LDP     X20, X21, [sp], #16
    LDP     x2, x4, [sp], #16
    RET

.align  3
ambarella_dram_exit_sr:
    STP     x2, x4, [sp, #-16]!
    STP     X20, X21, [sp, #-16]!
    STP     X22, X23, [sp, #-16]!
    STP     X25, x30, [sp, #-16]!

    MOVZ X20, #0x0000
    MOVK X20, #0x0000,lsl #0x10
    MOVK X20, #0x10,lsl #0x20
    /*LDR X20, =(0x1000000000)*/
    MOVZ X25, #0x0000
    MOVK X25, #0xed08,lsl #0x10
    MOVK X25, #0x20,lsl #0x20
    /*LDR X25, =(0X20ed080000)*/

    add X21, X20, #0x4000   /* broadcast base register */
    add X22, X20, #0x5000   /* ddrc0 base regsiter */
    add X23, X20, #0x6000   /* ddrc1 base regsiter */

    LDR W10, [X20]      /* used for checking host num */

    /* reset CAS_EFFECT by WCLK CTRL*/
    // Reset cas effect register
    //ddrc_uinst(DDRC_UINST_WCK_CTL, 8, 0)
    MOV W1, #0X0000080e
    MOV W2, #0X00000000
    MOV W4, #0X00000303
    trigger_uinst
    /* wait for complete */
6:  LDR W0, [X22, #0xbc]
    LDR W1, [X23, #0xbc]
    wait_complete tbnz, #0, 6b

    //driver cs pin
    MOV W1, #0X00000001
    MOV W2, #0X00000000
    MOV W4, #0X00000303
    trigger_uinst
    /* wait for complete */
10: LDR W0, [X22, #0xbc]
    LDR W1, [X23, #0xbc]
    wait_complete tbnz, #0, 10b

    MOV W0, #0x1
    MOV W1, #48000
    STR W0, [X25, #0X258]
    STR WZR, [X25, #0X258]
11: LDR W0, [X25, #0X254]
    CMP W0, W1
    blo 11b

    // Now release the CS
    MOV W1, #0X00000002
    MOV W2, #0X00000000
    MOV W4, #0X00000303
    trigger_uinst
    /* wait for complete */
15: LDR W0, [X22, #0xbc]
    LDR W1, [X23, #0xbc]
    wait_complete tbnz, #0, 15b

    MOV W0, #0x1
    MOV W1, #48000
    STR W0, [X25, #0X258]
    STR WZR, [X25, #0X258]
16: LDR W0, [X25, #0X254]
    CMP W0, W1
    blo 16b

    /* issue XSR to exit SR */
    MOV W1, #0X0000000d
    MOV W2, #0X00000028
    MOV W4, #0X00000303
    trigger_uinst
    /* wait for complete */
12: LDR W0, [X22, #0xbc]
    LDR W1, [X23, #0xbc]
    wait_complete tbnz, #0, 12b

    /* do auto refresh */
    MOV W1, #0X0000000d
    MOV W2, #0X00004038
    MOV W4, #0X00000303
    trigger_uinst
    /* wait for complete */
13: LDR W0, [X22, #0xbc]
    LDR W1, [X23, #0xbc]
    wait_complete tbnz, #0, 13b

    /* enable LP5 background ZQ calibration */
    MOVZ W0, #0x0
    MOVK W0, #0x9f1c,lsl #0x10
    STR W0, [X21, #0x30]
    dsb sy
    isb
    /* wait for complete */
14: LDR W0, [X22, #0x30]
    LDR W1, [X23, #0x30]
    wait_complete tbnz, #31, 14b

    /* enable DRAM access */
    LDR W0, [X21]
    orr W0, W0, #(3 << 0)
    STR W0, [X21]

    LDP     X25, x30, [sp], #16
    LDP     X22, X23, [sp], #16
    LDP     X20, X21, [sp], #16
    LDP     x2, x4, [sp], #16
    RET
