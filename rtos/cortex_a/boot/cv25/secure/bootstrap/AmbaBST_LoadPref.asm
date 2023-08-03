/**
 *  @file AmbaBST_LoadPref.asm
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
 *  @details Load user preferences and trained dram parameters
 *
 */

#include "AmbaCortexA53.h"

/* These defines should be the same as defines in AmbaBLD.h */
#define DRAM_TRAIN_PARAM_OFST_FLAG         (0x0<<2)
#define DRAM_TRAIN_PARAM_OFST_DQ_WDLY   (0x1<<2)
#define DRAM_TRAIN_PARAM_OFST_DQCA         (0x2<<2)
#define DRAM_TRAIN_PARAM_OFST_DLL0         (0x3<<2)
#define DRAM_TRAIN_PARAM_OFST_DLL1         (0x4<<2)
#define DRAM_TRAIN_PARAM_OFST_DLL2         (0x5<<2)
#define DRAM_TRAIN_PARAM_OFST_DLL3         (0x6<<2)
#define DRAM_TRAIN_PARAM_OFST_MR14         (0x7<<2)
#define DRAM_TRAIN_PARAM_OFST_FLAG_V               (0x8<<2)
#define DRAM_TRAIN_PARAM_OFST_DQSDLY       (0x9<<2)
#define DRAM_TRAIN_PARAM_OFST_STATE                (0x10<<2)

        .globl __AmbaBstLoadPref
__AmbaBstLoadPref:
/*
 *  Load User Configuration
 */
        MOV     X29, X30
        BL      __AmbaBstUserConfig                 /* call __AmbaBstUserConfig() */
        MOV     X30, X29

#ifdef CONFIG_DRAM_TYPE_LPDDR4
/*
 * Get well trained LPDDR4 DRAM parameters
 */
        LDR     X0, =(AMBA_CORTEX_A53_FLASH_IO_BASE_ADDR + 0xe00)
        LDR     W1, [X0, #DRAM_TRAIN_PARAM_OFST_FLAG]
        CMP     W1, #0
        BEQ     DONE

        LDR     X2, =0xdffe08b8
        LDR     X3, =0x416D6261
        STR     W3, [X2]
        LDR     W2, =0x80000000
        ANDS    W3, W1, W2
        BNE     DONE
        LDR     W3, [X0, #DRAM_TRAIN_PARAM_OFST_STATE]
        CMP     W3, #0
        BEQ     DONE
        CMP     W3, #2
        BEQ     DONE

        LDR     W1, [X0, #DRAM_TRAIN_PARAM_OFST_DQ_WDLY]
        ADR     X2, DRAM_lpddr4DqWriteDlyParam
        STR     W1, [X2]

        LDR     W1, [X0, #DRAM_TRAIN_PARAM_OFST_DQSDLY]
        ADR     X2, DRAM_lpddr4DqReadDlyParam
        STR     W1, [X2]

        LDR     W1, [X0, #DRAM_TRAIN_PARAM_OFST_DQCA]
        ADR     X2, DRAM_lpddr4DqCaVrefParam
        STR     W1, [X2]

        ADR     X2, DLL_Setting0Param
        LDP     W3, W4, [X0, #DRAM_TRAIN_PARAM_OFST_DLL0]
        STR     W3, [X2]
        STR     W4, [X2, #4]
        ADD     X2, X2, #8
        LDP     W3, W4, [X0, #DRAM_TRAIN_PARAM_OFST_DLL2]
        STR     W3, [X2]
        STR     W4, [X2, #4]

        ADR     X2, LPDDR4_ModeReg14ParamFsp0
        LDR     W3, [X0, #DRAM_TRAIN_PARAM_OFST_MR14]
        STR     W3, [X2]
        ADR     X2, LPDDR4_ModeReg14ParamFsp1
        STR     W3, [X2]

        LDR     W3, [X0, #DRAM_TRAIN_PARAM_OFST_STATE]
        LDR     X0, =0xdffe08b8
        LDR     X1, =0x61626D41
        CMP     W3, #1
        ADD     W3, W3, #1
        LSL     W3, W3, #24
        CSEL    X1, X3, X1, EQ
        STR     W1, [X0]
#endif
DONE:
        RET                                 /* Return to caller */
