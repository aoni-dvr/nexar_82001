/**
 *  @file AmbaLPDDR4_MT53D512M32D2DS_1392Mhz.asm
 *
 *  @copyright Copyright (c) 2017 Ambarella, Inc.
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

        .global PLL_DdrCtrlParam
        .global PLL_DdrCtrl2Param
        .global PLL_DdrCtrl3Param

        /* DLL parameters */
        .global DLL_ResetDelay

        .global DLL_Setting0Param
        .global DLL_Setting1Param
        .global DLL_Setting2Param
        .global DLL_Setting3Param

        .global DLL_CtrlSel0Param
        .global DLL_CtrlSel1Param
        .global DLL_CtrlSel2Param
        .global DLL_CtrlSel3Param
        .global DLL_CtrlSelMiscParam
        .global DLL_CtrlSyncTrackParam

        .global DRAMC_ModeParam

        /* DDR Controller parameters */
        .global DRAM_PorDelay

        .global DRAM_CtrlParam
        .global DRAM_ConfigParam
        .global DRAM_Timing1Param
        .global DRAM_Timing2Param
        .global DRAM_Timing3Param
        .global DRAM_Timing4Param
        .global DRAM_Timing5Lp4TrainingParam
        .global DRAM_ByteMapParam

        .global DRAM_DqsSyncPreRttParam
        .global DRAM_DqsSyncParam
        .global DRAM_ZqCalibParam

        .global DRAM_PadTermParam
        .global DRAM_PadTerm2Param
        .global DRAM_PadTerm3Param

        .global DRAM_lpddr4DqCaVrefParam
        .global DRAM_lpddr4DqCaVrefOffsetParam
        .global DRAM_lpddr4Mr12OffsetParam
        .global DRAM_lpddr4Mr14OffsetParam
        .global DRAM_lpddr4DqWriteDlyParam
        .global DRAM_lpddr4DqWriteOffsetParam
        .global DRAM_lpddr4DqsWriteDlyParam
        .global DRAM_lpddr4DqReadDlyParam
        .global DRAM_lpddr4DqReadOffsetParam
        .global DRAM_lpddr4DllOffsetParam
        .global DRAM_lpddr4TrainMpcRdlyParam

        .global DRAM_DllCalibParam
        .global DRAM_PowerDownParam

        /* LPDDR4 Initializations */
        .global LPDDR4_ModeReg1ParamFsp0
        .global LPDDR4_ModeReg1ParamFsp1
        .global LPDDR4_ModeReg2ParamFsp0
        .global LPDDR4_ModeReg2ParamFsp1
        .global LPDDR4_ModeReg3ParamFsp0
        .global LPDDR4_ModeReg3ParamFsp1
        .global LPDDR4_ModeReg11ParamFsp0
        .global LPDDR4_ModeReg11ParamFsp1
        .global LPDDR4_ModeReg12ParamFsp0
        .global LPDDR4_ModeReg12ParamFsp1
        .global LPDDR4_ModeReg13ParamFsp0
        .global LPDDR4_ModeReg14ParamFsp0
        .global LPDDR4_ModeReg14ParamFsp1
        .global LPDDR4_ModeReg22ParamFsp0
        .global LPDDR4_ModeReg22ParamFsp1

        /* Training */
        .global training_param

        .section BstDramParam

/* Cortex, Core, and DDR PLL parameters */
PLL_CortexCtrlParam:            .word   0x14112000
PLL_CortexCtrl2Param:           .word   0x30520000
PLL_CortexCtrl3Param:           .word   0x00088006

PLL_DdrCtrlParam:               .word   0x1c117000
PLL_DdrCtrl2Param:              .word   0x30520000
PLL_DdrCtrl3Param:              .word   0x00088006

DRAMC_ModeParam:                .word   0x00000010

/* DLL parameters */
DLL_ResetDelay:                 .word   0x1000

DLL_Setting0Param:              .word   0x00202020
DLL_Setting1Param:              .word   0x00202020
DLL_Setting2Param:              .word   0x00202020
DLL_Setting3Param:              .word   0x00202020

DLL_CtrlSel0Param:              .word   0x0001b881
DLL_CtrlSel1Param:              .word   0x0001b881
DLL_CtrlSel2Param:              .word   0x0001b881
DLL_CtrlSel3Param:              .word   0x0001b881
DLL_CtrlSelMiscParam:           .word   0x00188000
DLL_CtrlSyncTrackParam:         .word   0x0000b2c1

/* DDR Controller parameters */
DRAM_PorDelay:                  .word   0x2710

DRAM_CtrlParam:                 .word   0x6c801a80
DRAM_ConfigParam:               .word   0x702842d8
DRAM_Timing1Param:              .word   0x76A4C30C
DRAM_Timing2Param:              .word   0x1CE1F589
DRAM_Timing3Param:              .word   0x00000154
DRAM_Timing4Param:              .word   0x12D88236
DRAM_Timing5Lp4TrainingParam:   .word   0x6c588000
DRAM_ByteMapParam:              .word   0x000000e4

DRAM_DqsSyncPreRttParam:        .word   0x00001ea0
DRAM_DqsSyncParam:              .word   0x00001ea0
DRAM_ZqCalibParam:              .word   0x00000c60

DRAM_PadTermParam:              .word   0x000200bc
DRAM_PadTerm2Param:             .word   0x0a07aa07
DRAM_PadTerm3Param:             .word   0x00Aa7000

DRAM_lpddr4DqCaVrefParam:       .word   0x02c58b16
DRAM_lpddr4DqCaVrefOffsetParam: .word   0x85
DRAM_lpddr4Mr12OffsetParam:     .word   0x0
DRAM_lpddr4Mr14OffsetParam:     .word   0x00        /* 8-bit for ALL bytes. bit[7]:sign bit. ..., 0x82=-2, 0x81=-1, 0x80=0x0=0, 0x1=1, 0x2=2, ... */
DRAM_lpddr4DqWriteDlyParam:     .word   0xc5ab56ad
DRAM_lpddr4DqWriteOffsetParam:  .word   0x00000000  /* 8-bit for each byte. bit[7]:sign bit. ..., 0x82=-2, 0x81=-1, 0x80=0x0=0, 0x1=1, 0x2=2, ... */
DRAM_lpddr4DqsWriteDlyParam:    .word   0x00000000
DRAM_lpddr4DqReadDlyParam:      .word   0x00018c63
DRAM_lpddr4DqReadOffsetParam:   .word   0x00000000  /* 8-bit for each byte. bit[7]:sign bit. ..., 0x82=-2, 0x81=-1, 0x80=0x0=0, 0x1=1, 0x2=2, ... */
DRAM_lpddr4DllOffsetParam:      .word   0x00000000  /* 8-bit for each byte. bit[7]:sign bit. ..., 0x82=-2, 0x81=-1, 0x80=0x0=0, 0x1=1, 0x2=2, ... */
DRAM_lpddr4TrainMpcRdlyParam:   .word   0x00000034

DRAM_DllCalibParam:             .word   0x001e3204
DRAM_PowerDownParam:            .word   0x00000030

/* LPDDR4 Initializations */
LPDDR4_ModeReg1ParamFsp0:       .word   0x01010054
LPDDR4_ModeReg1ParamFsp1:       .word   0x01010054
LPDDR4_ModeReg2ParamFsp0:       .word   0x0102002d
LPDDR4_ModeReg2ParamFsp1:       .word   0x0102002d
LPDDR4_ModeReg3ParamFsp0:       .word   0x01030033
LPDDR4_ModeReg3ParamFsp1:       .word   0x01030033
LPDDR4_ModeReg11ParamFsp0:      .word   0x010b0022
LPDDR4_ModeReg11ParamFsp1:      .word   0x010b0022
LPDDR4_ModeReg12ParamFsp0:      .word   0x010c0010
LPDDR4_ModeReg12ParamFsp1:      .word   0x010c0010
LPDDR4_ModeReg13ParamFsp0:      .word   0x010d0000
LPDDR4_ModeReg14ParamFsp0:      .word   0x010e0012
LPDDR4_ModeReg14ParamFsp1:      .word   0x010e0012
LPDDR4_ModeReg22ParamFsp0:      .word   0x01160000
LPDDR4_ModeReg22ParamFsp1:      .word   0x01160000

/* Training
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
training_param:                 .word   0xfb00007d
#endif
