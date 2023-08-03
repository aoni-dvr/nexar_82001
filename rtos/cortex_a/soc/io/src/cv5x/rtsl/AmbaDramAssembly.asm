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

.global ambarella_dram_clockchang
.global ambarella_standby

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

#if defined(CONFIG_DRAM_TYPE_LPDDR5)
.align  3
ambarella_standby:
    MOV sp, X0

    LDR X20, =(0x1000000000)
    LDR X25, =(0X20ed080000)

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
    MOV W0, #0x9f1c0000
    orr W0, W0, #0x6
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

    LDR X20, =(0X20e002f000)
    MOV W1, #0x4
    STR W1, [X20, #0x58]
    MOV W0, #0x1
    STR W0, [X25, #0X258]
    STR WZR, [X25, #0X258]
    LDR W1, =72000
6:  LDR W0, [X25, #0X254]
    CMP W0, W1
    blo 6b
    MOV W1, #0X0
    STR W1, [X20, #0x40]
    MOV W1, #0x1
    STR W1, [X20, #0x40]
    MOV W0, #0x1
    STR W0, [X25, #0X258]
    STR WZR, [X25, #0X258]
    LDR W1, =14400000
2:  LDR W0, [X25, #0X254]
    CMP W0, W1
    blo 2b

    LDR X0, =0X20e0031000
    blr X0

    MOV W0, #0x1
    STR W0, [X25, #0X258]
    STR WZR, [X25, #0X258]
    LDR W1, =2400000000
7:  LDR W0, [X25, #0X254]
    CMP W0, W1
    blo 7b

    //LDR W0, [X25, #0x68]
    //orr W0, W0, #0x1
    //STR W0, [X25, #0x68]

    wfi
    b .

.align  3
ambarella_dram_clockchang:
    sub X0, X0, #8
    STR x1, [X0]
    sub X0, X0, #8
    MOV x1, sp
    STR x1, [X0]
    MOV sp, X0

    STP     x2, x4, [sp, #-16]!
    STP     X20, X21, [sp, #-16]!
    STP     X22, X23, [sp, #-16]!
    STP     X25, x30, [sp, #-16]!

    LDR X20, =(0x1000000000)
    LDR X25, =(0X20ed080000)

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
    MOV W0, #0x9f1c0000
    orr W0, W0, #0x6
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

    /* change clock */
    LDR X0, =0X20e0031000
    blr X0

    MOV     W0, #0x1
    LDR     W1, =48000
    STR W0, [X25, #0X258]
    STR WZR, [X25, #0X258]
7:  LDR W0, [X25, #0X254]
    CMP W0, W1
    blo 7b

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
    LDR     W1, =48000
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
    LDR     W1, =48000
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
    LDR W0, =0x9f1c0000
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
    LDP     X0, X1, [SP]
    MOV     sp, X0
    RET
#endif

#if defined(CONFIG_DRAM_TYPE_LPDDR4)
.align  3
ambarella_standby:
    sub X0, X0, #8
    STR x1, [X0]
    sub X0, X0, #8
    MOV x1, sp
    STR x1, [X0]
    MOV sp, X0

    STP     x2, x4, [sp, #-16]!
    STP     X20, X21, [sp, #-16]!
    STP     X22, X23, [sp, #-16]!
    STP     X25, x30, [sp, #-16]!

    LDR X20, =(0x1000000000)
    LDR X25, =(0X20ed080000)

    add X21, X20, #0x4000   /* broadcast base register */
    add X22, X20, #0x5000   /* ddrc0 base regsiter */
    add X23, X20, #0x6000   /* ddrc1 base regsiter */

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

    /* issue SRE to enter into SR */
    MOV W1, #0X0000000d
    MOV W2, #0X00000018
    MOV W4, #0X00000303
    trigger_uinst
    /* wait for complete */
5:  LDR W0, [X22, #0xbc]
    LDR W1, [X23, #0xbc]
    wait_complete tbnz, #0, 5b

    wfi
    b .

.align  3
ambarella_dram_clockchang:
    SUB X0, X0, #8
    STR X1, [X0]
    SUB X0, X0, #8
    MOV X1, SP
    STR X1, [X0]
    MOV SP, X0

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
    STP     X25, x30, [sp, #-16]!

    LDR X20, =(0X1000000000)
    LDR X24, =(0X20E0032000)
    LDR X25, =(0X20ED080000)

    ADD X21, X20, #0X4000
    ADD X22, X20, #0X5000
    ADD X23, X20, #0X6000

/* Program MR13 */
    LDR     W1, [X24]
    LDR     W2, =(0x9f0d0008)
    ORR     W1, W1, W2
    STR     W1, [X21, #0x30]
MR13_WR_host0:
    LDR     W1, [X22, #0x30]
    TBNZ    W1, #31, MR13_WR_host0
MR13_WR_host1:
    LDR     W1, [X23, #0x30]
    TBNZ    W1, #31, MR13_WR_host1

    LDR     W1, [X24, #8]
    STR     W1, [X21, #0x30]
MR1_0:
    LDR     W1, [X22, #0x30]
    TBNZ    W1, #31, MR1_0
MR1_1:
    LDR     W1, [X23, #0x30]
    TBNZ    W1, #31, MR1_1

    LDR     W1, [X24, #0xc]
    STR     W1, [X21, #0x30]
MR2_0:
    LDR     W1, [X22, #0x30]
    TBNZ    W1, #31, MR2_0
MR2_1:
    LDR     W1, [X23, #0x30]
    TBNZ    W1, #31, MR2_1

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

    /* issue SRE to enter into SR */
    MOV W1, #0X0000000d
    MOV W2, #0X00000018
    MOV W4, #0X00000303
    trigger_uinst
    /* wait for complete */
5:  LDR W0, [X22, #0xbc]
    LDR W1, [X23, #0xbc]
    wait_complete tbnz, #0, 5b

    /* change ddrc clock */
    LDR     X0, =0X20e0031000
    blr     X0

    LDR X20, =(0x1000000000)
    LDR X24, =(0X20e0032000)
    LDR X25, =(0X20ed080000)

    add X21, X20, #0x4000   /* broadcast base register */
    add X22, X20, #0x5000   /* ddrc0 base regsiter */
    add X23, X20, #0x6000   /* ddrc1 base regsiter */

    MOV     W0, #0x1
    LDR     W1, =48000
    STR     W0, [X25, #0X258]
    STR     WZR, [X25, #0X258]
7:  LDR     W0, [X25, #0X254]
    CMP     W0, W1
    BLO     7b

    /* Program MR13 */
    LDR     W1, [X24, #4]
    LDR     W2, =(0x9f0d0008)
    ORR     W1, W1, W2
    STR     W1, [X21, #0x30]
    MR13_op_host0:
    LDR     W1, [X22, #0x30]
    TBNZ    W1, #31, MR13_op_host0
MR13_op_host1:
    LDR     W1, [X23, #0x30]
    TBNZ    W1, #31, MR13_op_host1

    MOV     W0, #0x1
    LDR     W1, =48000
    STR W0, [X25, #0X258]
    STR WZR, [X25, #0X258]
3:  LDR W0, [X25, #0X254]
    CMP W0, W1
    blo 3b

    /* issue XSR to exit SR */
    MOV W1, #0X0000000d
    MOV W2, #0X00000014
    MOV W4, #0X00000303
    trigger_uinst
    /* wait for complete */
12: LDR W0, [X22, #0xbc]
    LDR W1, [X23, #0xbc]
    wait_complete tbnz, #0, 12b

    /* do auto refresh */
    MOV W1, #0X0000000d
    MOV W2, #0X00000028
    MOV W4, #0X00000303
    trigger_uinst
    /* wait for complete */
13: LDR W0, [X22, #0xbc]
    LDR W1, [X23, #0xbc]
    wait_complete tbnz, #0, 13b

    /* setting DDR to Read preamble mode for die-0 */
    LDR     W1, [X24, #4]
    LDR     W2, =(0x9f0d0008)
    ORR     W1, W1, #2
    BIC     W1, W1, #0X04000000
    STR     W1, [X21, #0x30]
MR13_preamble0:
    LDR     W1, [X22, #0x30]
    TBNZ    W1, #31, MR13_preamble0
MR13_preamble1:
    LDR     W1, [X23, #0x30]
    TBNZ    W1, #31, MR13_preamble1

    /* set DQS gate to 1 */
    LDR     W7, [X21, #0x8]
    MOVZ    W1, #0xc00
    STR     W1, [X21, #0x8]

    /* issue GET_RTT_EN command for die 0 */
    MOVZ    W1, #0x4
    STR     W1, [X21, #0x2c]
    MOVZ    W2, #0x4
DramInitCtrlReg_Wait0:
    LDR     W1, [X22, #0x2c]
    ANDS    W1, W1, W2
    B.NE    DramInitCtrlReg_Wait0
DramInitCtrlReg_Wait1:
    LDR     W1, [X23, #0x2c]
    ANDS    W1, W1, W2
    B.NE    DramInitCtrlReg_Wait1

	/* Restore to original value */
    STR     W7, [X21, #0x8]

    /* unset DDR Read preamble mode for die-0 */
    LDR     W1, [X24, #4]
    LDR     W2, =(0x9f0d0008)
    ORR     W1, W1, W2
    STR     W1, [X21, #0x30]
MR13_unset0:
    LDR     W1, [X22, #0x30]
    TBNZ    W1, #31, MR13_unset0
MR13_unset1:
    LDR     W1, [X23, #0x30]
    TBNZ    W1, #31, MR13_unset1

    /* enable DRAM access */
    LDR W0, [X21]
    orr W0, W0, #(3 << 0)
    STR W0, [X21]

    LDP     X25, X30, [SP], #16
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
    LDR     X0, [X0]
    MOV     SP, X0

    RET
#endif

