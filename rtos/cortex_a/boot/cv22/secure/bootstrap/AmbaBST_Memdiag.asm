/**
 *  @file AmbaBST_Memdiag.asm
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
 *  @details memory diagnosis
 *
 */

        .globl __AmbaMemDiag
__AmbaMemDiag:
        MOV     X29, X30

        MOV     W0, #0x5555 /* Test pattern 0x55555555 */
        MOVK    W0, #0x5555, lsl 16
        MOV     W1, #0xaaaa /* Test pattern 0xAAAAAAAA */
        MOVK    W1, #0xaaaa, lsl 16
        MOV     W10, #0         /* Target Memory Address */
        MOV     W2, #0          /* Loop Index */
        MOV     W6, #0          /* Test Result */
        BL      LoopCheck
MemTest:
        LSL     W3, W2, #2  /* Row */
        LSL     W4, W2, #12 /* COL */
        ADD     W10, W3, W4 /* Address */
        STR     W0, [X10]   /* Write X0 to Target Memory Address */
        LDR     W5, [X10]
        CMP     W5, W0
        BEQ     MemTest2
        EOR     W6, W5, W0
MemTest2:
        STR     W1, [X10]   /* Write X0 to Target Memory Address */
        LDR     W5, [X10]
        CMP     W5, W1
        BEQ     NextLoop
        EOR     W1, W5, W1
        ORR     W6, W6, W1
        BL      MemTestFail
NextLoop:
        CMP     W6, #0
        BNE     MemTestFail
        ADD     W2, W2, #1
LoopCheck:
        CMP     W2, #9
        BLE     MemTest
        RET     X29
MemTestFail:
        B . /* Check W6 to know which bit of DQ may be unstable */
