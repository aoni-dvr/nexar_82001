/**
 *  @file AmbaBST_LoadPref.asm
 *
 *  Copyright (c) 2021 Ambarella International LP
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

/* These defines should be the same as defines in AmbaBLD.h */

#include "AmbaBST.h"
#include "AmbaCSL_DDRC.h"

#define DRAM_TRAIN_PARAM_OFST_FLAG             (0x0<<2)
#define DRAM_TRAIN_PARAM_OFST_CK_DELAY         (0x1<<2)
#define DRAM_TRAIN_PARAM_OFST_CA_DELAY_COARSE  (0x2<<2)
#define DRAM_TRAIN_PARAM_OFST_CKE_DELAY_COARSE (0x3<<2)
#define DRAM_TRAIN_PARAM_OFST_RDDLY_DIE0       (0x4<<2)
#define DRAM_TRAIN_PARAM_OFST_RDDLY_DIE1       (0x5<<2)
#define DRAM_TRAIN_PARAM_OFST_RDVREF_0         (0x6<<2)
#define DRAM_TRAIN_PARAM_OFST_RDVREF_1         (0x7<<2)
#define DRAM_TRAIN_PARAM_OFST_WRVREF_0         (0x8<<2)
#define DRAM_TRAIN_PARAM_OFST_WRVREF_1         (0x9<<2)
#define DRAM_TRAIN_PARAM_OFST_B0_DLY0_D0       (0xA<<2)
#define DRAM_TRAIN_PARAM_OFST_B0_DLY1_D0       (0xB<<2)
#define DRAM_TRAIN_PARAM_OFST_B0_DLY2_D0       (0xC<<2)
#define DRAM_TRAIN_PARAM_OFST_B0_DLY3_D0       (0xD<<2)
#define DRAM_TRAIN_PARAM_OFST_B0_DLY4_D0       (0xE<<2)
#define DRAM_TRAIN_PARAM_OFST_B0_DLY5_D0       (0xF<<2)
#define DRAM_TRAIN_PARAM_OFST_B1_DLY0_D0       (0x10<<2)
#define DRAM_TRAIN_PARAM_OFST_B1_DLY1_D0       (0x11<<2)
#define DRAM_TRAIN_PARAM_OFST_B1_DLY2_D0       (0x12<<2)
#define DRAM_TRAIN_PARAM_OFST_B1_DLY3_D0       (0x13<<2)
#define DRAM_TRAIN_PARAM_OFST_B1_DLY4_D0       (0x14<<2)
#define DRAM_TRAIN_PARAM_OFST_B1_DLY5_D0       (0x15<<2)
#define DRAM_TRAIN_PARAM_OFST_B2_DLY0_D0       (0x16<<2)
#define DRAM_TRAIN_PARAM_OFST_B2_DLY1_D0       (0x17<<2)
#define DRAM_TRAIN_PARAM_OFST_B2_DLY2_D0       (0x18<<2)
#define DRAM_TRAIN_PARAM_OFST_B2_DLY3_D0       (0x19<<2)
#define DRAM_TRAIN_PARAM_OFST_B2_DLY4_D0       (0x1A<<2)
#define DRAM_TRAIN_PARAM_OFST_B2_DLY5_D0       (0x1B<<2)
#define DRAM_TRAIN_PARAM_OFST_B3_DLY0_D0       (0x1C<<2)
#define DRAM_TRAIN_PARAM_OFST_B3_DLY1_D0       (0x1D<<2)
#define DRAM_TRAIN_PARAM_OFST_B3_DLY2_D0       (0x1E<<2)
#define DRAM_TRAIN_PARAM_OFST_B3_DLY3_D0       (0x1F<<2)
#define DRAM_TRAIN_PARAM_OFST_B3_DLY4_D0       (0x20<<2)
#define DRAM_TRAIN_PARAM_OFST_B3_DLY5_D0       (0x21<<2)
#define DRAM_TRAIN_PARAM_OFST_B0_DLY0_D1       (0x22<<2)
#define DRAM_TRAIN_PARAM_OFST_B0_DLY1_D1       (0x23<<2)
#define DRAM_TRAIN_PARAM_OFST_B0_DLY2_D1       (0x24<<2)
#define DRAM_TRAIN_PARAM_OFST_B0_DLY3_D1       (0x25<<2)
#define DRAM_TRAIN_PARAM_OFST_B0_DLY4_D1       (0x26<<2)
#define DRAM_TRAIN_PARAM_OFST_B0_DLY5_D1       (0x27<<2)
#define DRAM_TRAIN_PARAM_OFST_B1_DLY0_D1       (0x28<<2)
#define DRAM_TRAIN_PARAM_OFST_B1_DLY1_D1       (0x29<<2)
#define DRAM_TRAIN_PARAM_OFST_B1_DLY2_D1       (0x2A<<2)
#define DRAM_TRAIN_PARAM_OFST_B1_DLY3_D1       (0x2B<<2)
#define DRAM_TRAIN_PARAM_OFST_B1_DLY4_D1       (0x2C<<2)
#define DRAM_TRAIN_PARAM_OFST_B1_DLY5_D1       (0x2D<<2)
#define DRAM_TRAIN_PARAM_OFST_B2_DLY0_D1       (0x2E<<2)
#define DRAM_TRAIN_PARAM_OFST_B2_DLY1_D1       (0x2F<<2)
#define DRAM_TRAIN_PARAM_OFST_B2_DLY2_D1       (0x30<<2)
#define DRAM_TRAIN_PARAM_OFST_B2_DLY3_D1       (0x31<<2)
#define DRAM_TRAIN_PARAM_OFST_B2_DLY4_D1       (0x32<<2)
#define DRAM_TRAIN_PARAM_OFST_B2_DLY5_D1       (0x33<<2)
#define DRAM_TRAIN_PARAM_OFST_B3_DLY0_D1       (0x34<<2)
#define DRAM_TRAIN_PARAM_OFST_B3_DLY1_D1       (0x35<<2)
#define DRAM_TRAIN_PARAM_OFST_B3_DLY2_D1       (0x36<<2)
#define DRAM_TRAIN_PARAM_OFST_B3_DLY3_D1       (0x37<<2)
#define DRAM_TRAIN_PARAM_OFST_B3_DLY4_D1       (0x38<<2)
#define DRAM_TRAIN_PARAM_OFST_B3_DLY5_D1       (0x39<<2)
#define DRAM_TRAIN_PARAM_OFST_MR30C0D0         (0x3A<<2)
#define DRAM_TRAIN_PARAM_OFST_MR30C0D1         (0x3B<<2)
#define DRAM_TRAIN_PARAM_OFST_MR30C1D0         (0x3C<<2)
#define DRAM_TRAIN_PARAM_OFST_MR30C1D1         (0x3D<<2)
#define DRAM_TRAIN_PARAM_OFST_MR12C0D0         (0x3E<<2)
#define DRAM_TRAIN_PARAM_OFST_MR12C0D1         (0x3F<<2)
#define DRAM_TRAIN_PARAM_OFST_MR12C1D0         (0x40<<2)
#define DRAM_TRAIN_PARAM_OFST_MR12C1D1         (0x41<<2)
#define DRAM_TRAIN_PARAM_OFST_MR14C0D0         (0x42<<2)
#define DRAM_TRAIN_PARAM_OFST_MR14C0D1         (0x43<<2)
#define DRAM_TRAIN_PARAM_OFST_MR14C1D0         (0x44<<2)
#define DRAM_TRAIN_PARAM_OFST_MR14C1D1         (0x45<<2)
#define DRAM_TRAIN_PARAM_OFST_MR15C0D0         (0x46<<2)
#define DRAM_TRAIN_PARAM_OFST_MR15C0D1         (0x47<<2)
#define DRAM_TRAIN_PARAM_OFST_MR15C1D0         (0x48<<2)
#define DRAM_TRAIN_PARAM_OFST_MR15C1D1         (0x49<<2)
#define DRAM_TRAIN_PARAM_OFST_WCK2DQC0D0       (0x4A<<2)
#define DRAM_TRAIN_PARAM_OFST_WCK2DQC1D0       (0x4B<<2)
#define DRAM_TRAIN_PARAM_OFST_WCK2DQC0D1       (0x4C<<2)
#define DRAM_TRAIN_PARAM_OFST_WCK2DQC1D1       (0x4D<<2)
#define DRAM_TRAIN_PARAM_OFST_STATE            (0x4E<<2)
#if defined(CONFIG_TEST_DRAM_SAVE_RESTORE)
#define DRAM_TRAIN_PARAM_OFST_RSTCOUNT         (0x4F<<2)
#define DRAM_TRAIN_PARAM_OFST_TOTALFAILCOUNT   (0x50<<2)
#endif

#define MAGIC_CODE_DRAM_TRAIN_COMPLETION       0x61626D41U
#define BST_OFST_DRAM_HIGH_FREQ_PARAM          0x1c00U
#define DRAM_TRAIN_PARAM_OFST                  (0x80<<2)

        .globl __AmbaBstLoadPref
__AmbaBstLoadPref:
/*
 *  Load User Configuration
 */
#if CONFIG_RESTORE_TRAINING_RESULT
        MOV     X29, X30
        LDR     X0, =(AMBA_CORTEX_A76_FLASH_IO_BASE_ADDR + BST_OFST_DRAM_HIGH_FREQ_PARAM)
        LDR     W1, [X0, #DRAM_TRAIN_PARAM_OFST_FLAG]
        LDR     W2, =MAGIC_CODE_DRAM_TRAIN_COMPLETION
        CMP     W1, W2
        BNE     DONE

        MOV     X26, #0x0
DDRC_TRAIN_RESTORE_LOOP:
        CMP     X26, #DRAMC_HOST_MAX_NUM
        BGE     RESTORE_DONE
        ADD     X26, X26, #1
        ADR     X0, DRAMC_ModeParam
        LDR     W1, [X0]
        MOV     W2, #1
        LSL     X2, X2, X26
        ANDS    X1, X1, X2
        BEQ     DDRC_TRAIN_RESTORE_LOOP
        CMP     X26, #1
        BEQ     1f
        B       2f

#if defined(CONFIG_DRAM_TYPE_LPDDR5)
1:      LDR     X0, =(AMBA_CORTEX_A76_FLASH_IO_BASE_ADDR + BST_OFST_DRAM_HIGH_FREQ_PARAM)
        ADR     X1, DLL_Setting0D0Param_Host0
        B       3f

2:      LDR     X0, =(AMBA_CORTEX_A76_FLASH_IO_BASE_ADDR + BST_OFST_DRAM_HIGH_FREQ_PARAM + DRAM_TRAIN_PARAM_OFST)
        ADR     X1, DLL_Setting0D0Param_Host1

3:
        LDR     W2, [X0, #DRAM_TRAIN_PARAM_OFST_CK_DELAY]
        LDR     W3, [X0, #DRAM_TRAIN_PARAM_OFST_CA_DELAY_COARSE]
        LDR     W4, [X0, #DRAM_TRAIN_PARAM_OFST_CKE_DELAY_COARSE]
        STR     W2, [X1, #DRAM_CKDLYPARAM]
        STR     W3, [X1, #DRAM_CADELAYCOARSEPARAM]
        STR     W4, [X1, #DRAM_CKEDLYPARAM]

        LDR     W2, [X0, #DRAM_TRAIN_PARAM_OFST_RDDLY_DIE0]
        LDR     W3, [X0, #DRAM_TRAIN_PARAM_OFST_RDDLY_DIE1]
        STP     W2, W3, [X1, #DLL_SETTING0D0PARAM]

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_B0_DLY0_D0
        ADD     X4, X1, #DRAM_DIE0BYTE0DLY0PARAM
        BL      DdrcByteDelay_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_B1_DLY0_D0
        ADD     X4, X1, #DRAM_DIE0BYTE1DLY0PARAM
        BL      DdrcByteDelay_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_B2_DLY0_D0
        ADD     X4, X1, #DRAM_DIE0BYTE2DLY0PARAM
        BL      DdrcByteDelay_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_B3_DLY0_D0
        ADD     X4, X1, #DRAM_DIE0BYTE3DLY0PARAM
        BL      DdrcByteDelay_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_B0_DLY0_D1
        ADD     X4, X1, #DRAM_DIE1BYTE0DLY0PARAM
        BL      DdrcByteDelay_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_B1_DLY0_D1
        ADD     X4, X1, #DRAM_DIE1BYTE1DLY0PARAM
        BL      DdrcByteDelay_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_B2_DLY0_D1
        ADD     X4, X1, #DRAM_DIE1BYTE2DLY0PARAM
        BL      DdrcByteDelay_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_B3_DLY0_D1
        ADD     X4, X1, #DRAM_DIE1BYTE3DLY0PARAM
        BL      DdrcByteDelay_Copy

        LDR     W2, [X0, #DRAM_TRAIN_PARAM_OFST_RDVREF_0]
        LDR     W3, [X0, #DRAM_TRAIN_PARAM_OFST_RDVREF_1]
        STR     W2, [X1, #DRAM_VREF0PARAM]
        STR     W3, [X1, #DRAM_VREF1PARAM]

        LDR     W2, [X0, #DRAM_TRAIN_PARAM_OFST_WRVREF_0]
        LDR     W3, [X0, #DRAM_TRAIN_PARAM_OFST_WRVREF_1]
        STR     W2, [X1, #DRAM_WRITEVREF0PARAM]
        STR     W3, [X1, #DRAM_WRITEVREF1PARAM]

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_MR30C0D0
        ADD     X4, X1, #LPDDR5_MODEREG30C0D0PARAMFSP0
        BL      DramModeReg_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_MR14C0D0
        ADD     X4, X1, #LPDDR5_MODEREG14C0D0PARAM
        BL      DramModeReg_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_MR15C0D0
        ADD     X4, X1, #LPDDR5_MODEREG15C0D0PARAM
        BL      DramModeReg_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_MR12C0D0
        ADD     X4, X1, #LPDDR5_MODEREG12C0D0PARAM
        BL      DramModeReg_Copy

        LDR     X6, =DDRCT_RESULT_WCK2DQX_ADDR(0, 0, 0)
        LDR     X7, =DDRCT_RESULT_WCK2DQX_ADDR(1, 0, 0)
        CMP     X26, #1
        CSEL    X6, X6, X7, EQ
        LDR     W2, [X0, #DRAM_TRAIN_PARAM_OFST_WCK2DQC0D0]
        STR     W2, [X6, #0]
        LDR     W2, [X0, #DRAM_TRAIN_PARAM_OFST_WCK2DQC1D0]
        STR     W2, [X6, #4]
        LDR     W2, [X0, #DRAM_TRAIN_PARAM_OFST_WCK2DQC0D1]
        STR     W2, [X6, #8]
        LDR     W2, [X0, #DRAM_TRAIN_PARAM_OFST_WCK2DQC1D1]
        STR     W2, [X6, #12]

        LDR     X6, =DDRCT_RESULT_WCK2DQX_VALID
        LDR     W2, =DDRCT_RESULT_VALID_MAGIC_CODE
        STR     W2, [X6]
#else
1:      LDR     X0, =(AMBA_CORTEX_A76_FLASH_IO_BASE_ADDR + BST_OFST_DRAM_HIGH_FREQ_PARAM)
        ADR     X1, DLL_Setting0D0Param_Host0
        B       3f

2:      LDR     X0, =(AMBA_CORTEX_A76_FLASH_IO_BASE_ADDR + BST_OFST_DRAM_HIGH_FREQ_PARAM + DRAM_TRAIN_PARAM_OFST)
        ADR     X1, DLL_Setting0D0Param_Host1

3:
        LDR     W2, [X0, #DRAM_TRAIN_PARAM_OFST_RDDLY_DIE0]
        LDR     W3, [X0, #DRAM_TRAIN_PARAM_OFST_RDDLY_DIE1]
        STP     W2, W3, [X1, #DLL_SETTING0D0PARAM]

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_B0_DLY0_D0
        ADD     X4, X1, #DRAM_DIE0BYTE0DLY0PARAM
        BL      DdrcByteDelay_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_B1_DLY0_D0
        ADD     X4, X1, #DRAM_DIE0BYTE1DLY0PARAM
        BL      DdrcByteDelay_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_B2_DLY0_D0
        ADD     X4, X1, #DRAM_DIE0BYTE2DLY0PARAM
        BL      DdrcByteDelay_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_B3_DLY0_D0
        ADD     X4, X1, #DRAM_DIE0BYTE3DLY0PARAM
        BL      DdrcByteDelay_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_B0_DLY0_D1
        ADD     X4, X1, #DRAM_DIE1BYTE0DLY0PARAM
        BL      DdrcByteDelay_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_B1_DLY0_D1
        ADD     X4, X1, #DRAM_DIE1BYTE1DLY0PARAM
        BL      DdrcByteDelay_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_B2_DLY0_D1
        ADD     X4, X1, #DRAM_DIE1BYTE2DLY0PARAM
        BL      DdrcByteDelay_Copy

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_B3_DLY0_D1
        ADD     X4, X1, #DRAM_DIE1BYTE3DLY0PARAM
        BL      DdrcByteDelay_Copy

        LDR     W2, [X0, #DRAM_TRAIN_PARAM_OFST_RDVREF_0]
        LDR     W3, [X0, #DRAM_TRAIN_PARAM_OFST_RDVREF_1]
        STR     W2, [X1, #DRAM_VREF0PARAM]
        STR     W3, [X1, #DRAM_VREF1PARAM]

        ADD     X3, X0, #DRAM_TRAIN_PARAM_OFST_MR14C0D0
        ADD     X4, X1, #LPDDR4_MODEREG14C0D0PARAM
        BL      DramModeReg_Copy
#endif
        B       DDRC_TRAIN_RESTORE_LOOP

RESTORE_DONE:
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#if defined(CONFIG_ATF_HAVE_BL2)
        /* For CV5x BL2 cannot read from flash correctly,
         * we use system sketchpad memory to keep the state.
         */
        LDR     x0, =0x20E0030000
        LDR     W2, =MAGIC_CODE_DRAM_TRAIN_COMPLETION
        //str     w2, [x0, #DRAM_TRAIN_PARAM_OFST_FLAG]
        str     w2, [x0, #DRAM_TRAIN_PARAM_OFST_STATE]
#endif
#endif
        LDR     x0, =(AMBA_CORTEX_A76_DDR_CTRL0_BASE_ADDR)
        LDR     W2, =MAGIC_CODE_DRAM_TRAIN_COMPLETION
        str     w2, [x0, 0xc4]
        MOV     X30, X29
        MOV     W9, #1                      /* W9 = reload training status. 1:okay */
        RET

/*
 *  DDRC Byte Delay register copy Subroutine
 */
/*
 *  Notice the sequence of is important to let this function succeed
 *  DLY0(0x0) -> DLY1(0x4) -> DLY2(0x8) -> DLY3(0xc) -> DLY4(0x10) -> DLY5(0x14).
 *  X3 = the address of DLY0 saved trained value
 *  X4 = the address of DLY0 parameter
 */
DdrcByteDelay_Copy:
        LDP     W5, W6, [X3, #0]
        STP     W5, W6, [X4, #0]
        LDP     W5, W6, [X3, #8]
        STP     W5, W6, [X4, #8]
        LDP     W5, W6, [X3, #16]
        STP     W5, W6, [X4, #16]
        RET

/*
 *  DRAM mode register set copy Subroutine
 */
/*
 *  Notice the sequence of is important to let this function succeed
 *  C0D0(0x0) -> C0D1(0x4) -> C1D0(0x8) -> C1D1(0xc).
 *  X3 = the address of C0D0 saved trained value
 *  X4 = the address of C0D0 parameter
 */
DramModeReg_Copy:
        LDP     W5, W6, [X3, #0]
        STP     W5, W6, [X4, #0]
        LDP     W5, W6, [X3, #8]
        STP     W5, W6, [X4, #8]
        RET

#endif
DONE:
        MOV     W9, #0                      /* W9 = reload training status. 0:no valid result */
        RET                                 /* Return to caller */
