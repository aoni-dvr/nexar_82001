/**
 *  @file AmbaLPDDR4_MT53D512M32D2DS_1824Mhz.asm
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details DRAM device Parameters
 *
 */

        /* Cortex, Core, and DDR PLL parameters */
        .global PLL_CortexCtrlParam
        .global PLL_CortexCtrl2Param
        .global PLL_CortexCtrl3Param

        .global PLL_CoreCtrlParam
        .global PLL_CoreCtrl2Param
        .global PLL_CoreCtrl3Param

        .global PLL_DdrCtrlParam
        .global PLL_DdrCtrl2Param
        .global PLL_DdrCtrl3Param

        .global DLL_ResetDelay

        .global DRAMC_ModeParam
        .global DRAM_WdqsTimingParam
        .global DRAM_IbiasParam
        /* DDR Controller parameters */
        .global DRAM_PorDelay

        .global DRAM_CtrlParam
        .global DRAM_Config1Param
        .global DRAM_Config2Param
        .global DRAM_Timing1Param
        .global DRAM_Timing2Param
        .global DRAM_Timing3Param
        .global DRAM_Timing4Param
        .global DRAM_Timing5Param
        .global DRAM_DeulDieTimingParam
        .global DRAM_RefreshTimingParam
        .global DRAM_Lp5TimingParam
        .global DRAM_DtteTimingParam

        .global DLL_Setting0D0Param_Host0
        .global DLL_Setting0D1Param_Host0
        .global DLL_Setting1D0Param_Host0
        .global DLL_Setting1D1Param_Host0
        .global DLL_Setting2D0Param_Host0
        .global DLL_Setting2D1Param_Host0

        .global DLL_Setting0D0Param_Host1
        .global DLL_Setting0D1Param_Host1
        .global DLL_Setting1D0Param_Host1
        .global DLL_Setting1D1Param_Host1
        .global DLL_Setting2D0Param_Host1
        .global DLL_Setting2D1Param_Host1

        .global LPDDR4_ModeReg11ParamFsp0_Host0

        /* Training */
        .global TRAIN_ReadDelayOffset_Host0
        .global TRAIN_ReadDelayOffset_Host1
        .global TRAIN_WriteDelayOffset_Host0
        .global TRAIN_WriteDelayOffset_Host1
        .global TRAIN_VrefOffset_Host0
        .global TRAIN_VrefOffset_Host1
        .global TRAIN_MR14Offset_Host0
        .global TRAIN_MR14Offset_Host1
        .global training_param

        .section BstDramParam

/* Cortex, Core, and DDR PLL parameters */
PLL_CortexCtrlParam:        .word   0x42100000
PLL_CortexCtrl2Param:       .word   0x30520000
PLL_CortexCtrl3Param:       .word   0x000C8002

PLL_CoreCtrlParam:          .word   0x21110000
PLL_CoreCtrl2Param:         .word   0x30520000
PLL_CoreCtrl3Param:         .word   0x000C8002

PLL_DdrCtrlParam:           .word   0x25001000
PLL_DdrCtrl2Param:          .word   0x20521300
PLL_DdrCtrl3Param:          .word   0x000c8006

DRAMC_ModeParam:            .word   0x00000012

DLL_ResetDelay:             .word   0x1000
DRAM_PorDelay:              .word   0x2710

/* Common DDR Controller parameters */
DRAM_CtrlParam:             .word   0x0
DRAM_Config1Param:          .word   0x000f2890
DRAM_Config2Param:          .word   0x00001400
DRAM_Timing1Param:          .word   0x4b202012
DRAM_Timing2Param:          .word   0x0e200d6c
DRAM_Timing3Param:          .word   0x480f1fde
DRAM_Timing4Param:          .word   0x00251212
DRAM_Timing5Param:          .word   0x1643a288
DRAM_DeulDieTimingParam:    .word   0x1f1f1f1f
DRAM_RefreshTimingParam:    .word   0x0dbbf0fc
DRAM_Lp5TimingParam:        .word   0x000039a1
DRAM_WdqsTimingParam:       .word   0x00002105
DRAM_DtteTimingParam:       .word   0x00640040

/* Common DDR Controller parameters for Each Host */
/* Host0 */
DLL_Setting0D0Param_Host0:        .word   0x20202020
DLL_Setting0D1Param_Host0:        .word   0x20202020
DLL_Setting1D0Param_Host0:        .word   0x0f0f0f0f
DLL_Setting1D1Param_Host0:        .word   0x0f0f0f0f
DLL_Setting2D0Param_Host0:        .word   0x21212121
DLL_Setting2D1Param_Host0:        .word   0x21212121

DLL_Sbc0Param_Host0:              .word   0x0003b811
DLL_Sbc1Param_Host0:              .word   0x0003b811
DLL_Sbc2Param_Host0:              .word   0x0003b811
DLL_Sbc3Param_Host0:              .word   0x0003b811

DRAM_ByteMapParam_Host0:          .word   0x000000e1

DRAM_Die0Byte0Dly0Param_Host0:              .word   0x00090841
DRAM_Die0Byte0Dly1Param_Host0:              .word   0x16a14000
DRAM_Die0Byte0Dly2Param_Host0:              .word   0x0a0a0a0a
DRAM_Die0Byte0Dly3Param_Host0:              .word   0x0a0a0a0a
DRAM_Die0Byte0Dly4Param_Host0:              .word   0x0b0b0b0b
DRAM_Die0Byte0Dly5Param_Host0:              .word   0x0b0b0b0b

DRAM_Die0Byte1Dly0Param_Host0:              .word   0x00090841
DRAM_Die0Byte1Dly1Param_Host0:              .word   0x16a14000
DRAM_Die0Byte1Dly2Param_Host0:              .word   0x0a0a0a0a
DRAM_Die0Byte1Dly3Param_Host0:              .word   0x0a0a0a0a
DRAM_Die0Byte1Dly4Param_Host0:              .word   0x0b0b0b0b
DRAM_Die0Byte1Dly5Param_Host0:              .word   0x0b0b0b0b

DRAM_Die0Byte2Dly0Param_Host0:              .word   0x00090841
DRAM_Die0Byte2Dly1Param_Host0:              .word   0x16a14000
DRAM_Die0Byte2Dly2Param_Host0:              .word   0x0a0a0a0a
DRAM_Die0Byte2Dly3Param_Host0:              .word   0x0a0a0a0a
DRAM_Die0Byte2Dly4Param_Host0:              .word   0x0b0b0b0b
DRAM_Die0Byte2Dly5Param_Host0:              .word   0x0b0b0b0b

DRAM_Die0Byte3Dly0Param_Host0:              .word   0x00090841
DRAM_Die0Byte3Dly1Param_Host0:              .word   0x16a14000
DRAM_Die0Byte3Dly2Param_Host0:              .word   0x0a0a0a0a
DRAM_Die0Byte3Dly3Param_Host0:              .word   0x0a0a0a0a
DRAM_Die0Byte3Dly4Param_Host0:              .word   0x0b0b0b0b
DRAM_Die0Byte3Dly5Param_Host0:              .word   0x0b0b0b0b

DRAM_Die1Byte0Dly0Param_Host0:              .word   0x00090841
DRAM_Die1Byte0Dly1Param_Host0:              .word   0x16a14000
DRAM_Die1Byte0Dly2Param_Host0:              .word   0x0a0a0a0a
DRAM_Die1Byte0Dly3Param_Host0:              .word   0x0a0a0a0a
DRAM_Die1Byte0Dly4Param_Host0:              .word   0x0b0b0b0b
DRAM_Die1Byte0Dly5Param_Host0:              .word   0x0b0b0b0b

DRAM_Die1Byte1Dly0Param_Host0:              .word   0x00090841
DRAM_Die1Byte1Dly1Param_Host0:              .word   0x16a14000
DRAM_Die1Byte1Dly2Param_Host0:              .word   0x0a0a0a0a
DRAM_Die1Byte1Dly3Param_Host0:              .word   0x0a0a0a0a
DRAM_Die1Byte1Dly4Param_Host0:              .word   0x0b0b0b0b
DRAM_Die1Byte1Dly5Param_Host0:              .word   0x0b0b0b0b

DRAM_Die1Byte2Dly0Param_Host0:              .word   0x00090841
DRAM_Die1Byte2Dly1Param_Host0:              .word   0x16a14000
DRAM_Die1Byte2Dly2Param_Host0:              .word   0x0a0a0a0a
DRAM_Die1Byte2Dly3Param_Host0:              .word   0x0a0a0a0a
DRAM_Die1Byte2Dly4Param_Host0:              .word   0x0b0b0b0b
DRAM_Die1Byte2Dly5Param_Host0:              .word   0x0b0b0b0b

DRAM_Die1Byte3Dly0Param_Host0:              .word   0x00090841
DRAM_Die1Byte3Dly1Param_Host0:              .word   0x16a14000
DRAM_Die1Byte3Dly2Param_Host0:              .word   0x0a0a0a0a
DRAM_Die1Byte3Dly3Param_Host0:              .word   0x0a0a0a0a
DRAM_Die1Byte3Dly4Param_Host0:              .word   0x0b0b0b0b
DRAM_Die1Byte3Dly5Param_Host0:              .word   0x0b0b0b0b

DRAM_CkDlyParam_Host0:                      .word   0x00000000
DRAM_CaDelayCoarseParam_Host0:              .word   0x00000000
DRAM_CkeDlyParam_Host0:                     .word   0x00000000
DRAM_Vref0Param_Host0:                      .word   0x25dd25dd
DRAM_Vref1Param_Host0:                      .word   0x25dd25dd

DRAM_WriteVref0Param_Host0:           .word   0x0
DRAM_WriteVref1Param_Host0:           .word   0x0

DRAM_PadTermParam_Host0:              .word   0x00000010
DRAM_DqsPuPdParam_Host0:              .word   0x00000006
DRAM_CaPadCtrlParam_Host0:            .word   0x02C702C7
DRAM_DqPadCtrlParam_Host0:            .word   0x02970297
DRAM_IbiasParam_Host0:                .word   0x04000000

LPDDR4_ModeReg1ParamFsp0_Host0:       .word   0x9f010064
LPDDR4_ModeReg2ParamFsp0_Host0:       .word   0x9f020036
LPDDR4_ModeReg3ParamFsp0_Host0:       .word   0x9f030031
LPDDR4_ModeReg11ParamFsp0_Host0:      .word   0x9f0b0022
LPDDR4_ModeReg12ParamC0D0_Host0:      .word   0x9f0c0019
LPDDR4_ModeReg12ParamC0D1_Host0:      .word   0x9f0c0019
LPDDR4_ModeReg12ParamC1D0_Host0:      .word   0x9f0c0019
LPDDR4_ModeReg12ParamC1D1_Host0:      .word   0x9f0c0019
LPDDR4_ModeReg13ParamFsp0_Host0:      .word   0x9f0d0008
LPDDR4_ModeReg14ParamC0D0_Host0:      .word   0x9f0e0022
LPDDR4_ModeReg14ParamC0D1_Host0:      .word   0x9f0e0022
LPDDR4_ModeReg14ParamC1D0_Host0:      .word   0x9f0e0022
LPDDR4_ModeReg14ParamC1D1_Host0:      .word   0x9f0e0022
LPDDR4_ModeReg22ParamFsp0_Host0:      .word   0x9f160004

/* Host1 */
DLL_Setting0D0Param_Host1:        .word   0x20202020
DLL_Setting0D1Param_Host1:        .word   0x20202020
DLL_Setting1D0Param_Host1:        .word   0x0f0f0f0f
DLL_Setting1D1Param_Host1:        .word   0x0f0f0f0f
DLL_Setting2D0Param_Host1:        .word   0x21212121
DLL_Setting2D1Param_Host1:        .word   0x21212121

DLL_Sbc0Param_Host1:              .word   0x00e1b881
DLL_Sbc1Param_Host1:              .word   0x00e1b881
DLL_Sbc2Param_Host1:              .word   0x00e1b881
DLL_Sbc3Param_Host1:              .word   0x00e1b881

DRAM_ByteMapParam_Host1:          .word   0x000000b4

DRAM_Die0Byte0Dly0Param_Host1:              .word   0x00090841
DRAM_Die0Byte0Dly1Param_Host1:              .word   0x16a14000
DRAM_Die0Byte0Dly2Param_Host1:              .word   0x0a0a0a0a
DRAM_Die0Byte0Dly3Param_Host1:              .word   0x0a0a0a0a
DRAM_Die0Byte0Dly4Param_Host1:              .word   0x0b0b0b0b
DRAM_Die0Byte0Dly5Param_Host1:              .word   0x0b0b0b0b

DRAM_Die0Byte1Dly0Param_Host1:              .word   0x00090841
DRAM_Die0Byte1Dly1Param_Host1:              .word   0x16a14000
DRAM_Die0Byte1Dly2Param_Host1:              .word   0x0a0a0a0a
DRAM_Die0Byte1Dly3Param_Host1:              .word   0x0a0a0a0a
DRAM_Die0Byte1Dly4Param_Host1:              .word   0x0b0b0b0b
DRAM_Die0Byte1Dly5Param_Host1:              .word   0x0b0b0b0b

DRAM_Die0Byte2Dly0Param_Host1:              .word   0x00090841
DRAM_Die0Byte2Dly1Param_Host1:              .word   0x16a14000
DRAM_Die0Byte2Dly2Param_Host1:              .word   0x0a0a0a0a
DRAM_Die0Byte2Dly3Param_Host1:              .word   0x0a0a0a0a
DRAM_Die0Byte2Dly4Param_Host1:              .word   0x0b0b0b0b
DRAM_Die0Byte2Dly5Param_Host1:              .word   0x0b0b0b0b

DRAM_Die0Byte3Dly0Param_Host1:              .word   0x00090841
DRAM_Die0Byte3Dly1Param_Host1:              .word   0x16a14000
DRAM_Die0Byte3Dly2Param_Host1:              .word   0x0a0a0a0a
DRAM_Die0Byte3Dly3Param_Host1:              .word   0x0a0a0a0a
DRAM_Die0Byte3Dly4Param_Host1:              .word   0x0b0b0b0b
DRAM_Die0Byte3Dly5Param_Host1:              .word   0x0b0b0b0b

DRAM_Die1Byte0Dly0Param_Host1:              .word   0x00090841
DRAM_Die1Byte0Dly1Param_Host1:              .word   0x16a14000
DRAM_Die1Byte0Dly2Param_Host1:              .word   0x0a0a0a0a
DRAM_Die1Byte0Dly3Param_Host1:              .word   0x0a0a0a0a
DRAM_Die1Byte0Dly4Param_Host1:              .word   0x0b0b0b0b
DRAM_Die1Byte0Dly5Param_Host1:              .word   0x0b0b0b0b

DRAM_Die1Byte1Dly0Param_Host1:              .word   0x00090841
DRAM_Die1Byte1Dly1Param_Host1:              .word   0x16a14000
DRAM_Die1Byte1Dly2Param_Host1:              .word   0x0a0a0a0a
DRAM_Die1Byte1Dly3Param_Host1:              .word   0x0a0a0a0a
DRAM_Die1Byte1Dly4Param_Host1:              .word   0x0b0b0b0b
DRAM_Die1Byte1Dly5Param_Host1:              .word   0x0b0b0b0b

DRAM_Die1Byte2Dly0Param_Host1:              .word   0x00090841
DRAM_Die1Byte2Dly1Param_Host1:              .word   0x16a14000
DRAM_Die1Byte2Dly2Param_Host1:              .word   0x0a0a0a0a
DRAM_Die1Byte2Dly3Param_Host1:              .word   0x0a0a0a0a
DRAM_Die1Byte2Dly4Param_Host1:              .word   0x0b0b0b0b
DRAM_Die1Byte2Dly5Param_Host1:              .word   0x0b0b0b0b

DRAM_Die1Byte3Dly0Param_Host1:              .word   0x00090841
DRAM_Die1Byte3Dly1Param_Host1:              .word   0x16a14000
DRAM_Die1Byte3Dly2Param_Host1:              .word   0x0a0a0a0a
DRAM_Die1Byte3Dly3Param_Host1:              .word   0x0a0a0a0a
DRAM_Die1Byte3Dly4Param_Host1:              .word   0x0b0b0b0b
DRAM_Die1Byte3Dly5Param_Host1:              .word   0x0b0b0b0b

DRAM_CkDlyParam_Host1:                      .word   0x00000000
DRAM_CaDelayCoarseParam_Host1:              .word   0x00000000
DRAM_CkeDlyParam_Host1:                     .word   0x00000000
DRAM_Vref0Param_Host1:                      .word   0x25dd25dd
DRAM_Vref1Param_Host1:                      .word   0x25dd25dd

DRAM_WriteVref0Param_Host1:           .word   0x0
DRAM_WriteVref1Param_Host1:           .word   0x0

DRAM_PadTermParam_Host1:              .word   0x00000010
DRAM_DqsPuPdParam_Host1:              .word   0x00000006
DRAM_CaPadCtrlParam_Host1:            .word   0x02C702C7
DRAM_DqPadCtrlParam_Host1:            .word   0x02970297
DRAM_IbiasParam_Host1:                .word   0x04000000

LPDDR4_ModeReg1ParamFsp0_Host1:       .word   0x9f010064
LPDDR4_ModeReg2ParamFsp0_Host1:       .word   0x9f020036
LPDDR4_ModeReg3ParamFsp0_Host1:       .word   0x9f030031
LPDDR4_ModeReg11ParamFsp0_Host1:      .word   0x9f0b0022
LPDDR4_ModeReg12ParamC0D0_Host1:      .word   0x9f0c0019
LPDDR4_ModeReg12ParamC0D1_Host1:      .word   0x9f0c0019
LPDDR4_ModeReg12ParamC1D0_Host1:      .word   0x9f0c0019
LPDDR4_ModeReg12ParamC1D1_Host1:      .word   0x9f0c0019
LPDDR4_ModeReg13ParamFsp0_Host1:      .word   0x9f0d0008
LPDDR4_ModeReg14ParamC0D0_Host1:      .word   0x9f0e0022
LPDDR4_ModeReg14ParamC0D1_Host1:      .word   0x9f0e0022
LPDDR4_ModeReg14ParamC1D0_Host1:      .word   0x9f0e0022
LPDDR4_ModeReg14ParamC1D1_Host1:      .word   0x9f0e0022
LPDDR4_ModeReg22ParamFsp0_Host1:      .word   0x9f160004

/* BLD Training parameters */
TRAIN_ReadDelayOffset_Host0:          .word   0x00000000    /* (Signed) B0: byte0, B1: byte1, B2: byte2, B3: byte3 */
TRAIN_ReadDelayOffset_Host1:          .word   0x00000000
TRAIN_WriteDelayOffset_Host0:         .word   0x00000000    /* (Signed) B0: byte0, B1: byte1, B2: byte2, B3: byte3 */
TRAIN_WriteDelayOffset_Host1:         .word   0x00000000
TRAIN_VrefOffset_Host0:               .byte   0x00          /* (Signed) DDRC0: Read */
TRAIN_VrefOffset_Host1:               .byte   0x00          /* (Signed) DDRC1: Read */
TRAIN_MR14Offset_Host0:               .byte   0x00          /* (Signed) DDRC0: Write */
TRAIN_MR14Offset_Host1:               .byte   0x00          /* (Signed) DDRC1: Write */

/* BST Training parameters
 * bit[0]: enable training
 * bit[2]: train read vref
 * bit[3]: train read_delay/read_dll
 * bit[4]: train write_delay/write vref
 * bit[7:5]: number of repetition of check_read_fifo
 * bit[11:8]: write_delay offset
 * bit[15:12]: read_delay offset
 * bit[23:16]: write_vref offset
 * bit[31:24]: read_vref offset
 */
#if defined(CONFIG_BST_DRAM_TRAINING)
training_param:                 .word   0x0000007d
#endif
