/**
 *  @file AmbaLPDDR4_MT53D512M32D2DS_1800Mhz.asm
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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

        /* DLL parameters */
        .global Ddrc0_DLL_Setting0Param
        .global Ddrc0_DLL_Setting1Param
        .global Ddrc0_DLL_Setting2Param
        .global Ddrc0_DLL_Setting3Param

        .global Ddrc1_DLL_Setting0Param
        .global Ddrc1_DLL_Setting1Param
        .global Ddrc1_DLL_Setting2Param
        .global Ddrc1_DLL_Setting3Param

        .global DLL_CtrlSel0Param
        .global DLL_CtrlSel1Param
        .global DLL_CtrlSel2Param
        .global DLL_CtrlSel3Param
        .global DLL_CtrlSelMiscParam
        .global DLL_CtrlSyncTrackParam

        .global DRAMC_ModeParam

        /* DDR Controller parameters */
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

        .global Ddrc0_DRAM_PadTermParam
        .global Ddrc0_DRAM_PadTerm2Param
        .global Ddrc0_DRAM_PadTerm3Param

        .global Ddrc1_DRAM_PadTermParam
        .global Ddrc1_DRAM_PadTerm2Param
        .global Ddrc1_DRAM_PadTerm3Param

        .global Ddrc0_DRAM_lpddr4DqCaVrefParam
        .global Ddrc1_DRAM_lpddr4DqCaVrefParam
        .global Ddrc0_DRAM_lpddr4DqWriteDlyParam
        .global Ddrc1_DRAM_lpddr4DqWriteDlyParam
        .global Ddrc0_DRAM_lpddr4DqsWriteDlyParam
        .global Ddrc1_DRAM_lpddr4DqsWriteDlyParam
        .global Ddrc0_DRAM_lpddr4DqReadDlyParam
        .global Ddrc1_DRAM_lpddr4DqReadDlyParam
        .global Ddrc0_DRAM_lpddr4TrainMpcRdlyParam
        .global Ddrc1_DRAM_lpddr4TrainMpcRdlyParam
        .global DRAM_DllCalibParam

        /* LPDDR4 Initializations */
        .global LPDDR4_ModeReg1ParamFsp0
        .global LPDDR4_ModeReg2ParamFsp0
        .global LPDDR4_ModeReg13ParamFsp0

        .global Ddrc0_LPDDR4_ModeReg3ParamFsp0
        .global Ddrc1_LPDDR4_ModeReg3ParamFsp0
        .global Ddrc0_LPDDR4_ModeReg11ParamFsp0
        .global Ddrc1_LPDDR4_ModeReg11ParamFsp0
        .global Ddrc0_LPDDR4_ModeReg12ParamFsp0
        .global Ddrc1_LPDDR4_ModeReg12ParamFsp0
        .global Ddrc0_LPDDR4_ModeReg14ParamFsp0
        .global Ddrc1_LPDDR4_ModeReg14ParamFsp0
        .global Ddrc0_LPDDR4_ModeReg22ParamFsp0
        .global Ddrc1_LPDDR4_ModeReg22ParamFsp0

        /* Training */
        .global training_param

        .section BstDramParam

/* Cortex, Core, and DDR PLL parameters */
PLL_CortexCtrlParam:                  .word   0x29100000
PLL_CortexCtrl2Param:                 .word   0x30520000
PLL_CortexCtrl3Param:                 .word   0x00088004

PLL_CoreCtrlParam:                    .word   0x1f111000
PLL_CoreCtrl2Param:                   .word   0x30520000
PLL_CoreCtrl3Param:                   .word   0x00088006

PLL_DdrCtrlParam:                     .word   0x31102000
PLL_DdrCtrl2Param:                    .word   0x30520000
PLL_DdrCtrl3Param:                    .word   0x00088002

DRAMC_ModeParam:                      .word   0x00000011

/* DLL parameters */
Ddrc0_DLL_Setting0Param:              .word   0x00200030
Ddrc0_DLL_Setting1Param:              .word   0x00200030
Ddrc0_DLL_Setting2Param:              .word   0x00200030
Ddrc0_DLL_Setting3Param:              .word   0x00200030

Ddrc1_DLL_Setting0Param:              .word   0x00200030
Ddrc1_DLL_Setting1Param:              .word   0x00200030
Ddrc1_DLL_Setting2Param:              .word   0x00200030
Ddrc1_DLL_Setting3Param:              .word   0x00200030

DLL_CtrlSel0Param:                    .word   0x0042b9b1
DLL_CtrlSel1Param:                    .word   0x0042b9b1
DLL_CtrlSel2Param:                    .word   0x0042b9b1
DLL_CtrlSel3Param:                    .word   0x0042b9b1
DLL_CtrlSelMiscParam:                 .word   0x00188000
DLL_CtrlSyncTrackParam:               .word   0x0000b2c1

/* DDR Controller parameters */
DRAM_CtrlParam:                       .word   0x6ec01a80
DRAM_ConfigParam:                     .word   0x702842d8
DRAM_Timing1Param:                    .word   0x98e123f1
DRAM_Timing2Param:                    .word   0x210289fc
DRAM_Timing3Param:                    .word   0x000001B7
DRAM_Timing4Param:                    .word   0x33a2a847
DRAM_Timing5Lp4TrainingParam:         .word   0x90588000
DRAM_ByteMapParam:                    .word   0x000000e4

DRAM_DqsSyncPreRttParam:              .word   0x00001ea0
DRAM_DqsSyncParam:                    .word   0x00001ea0
DRAM_ZqCalibParam:                    .word   0x00000c60

Ddrc0_DRAM_PadTermParam:              .word   0x000200bc
Ddrc0_DRAM_PadTerm2Param:             .word   0x0a059a03
Ddrc0_DRAM_PadTerm3Param:             .word   0x00aa7000

Ddrc1_DRAM_PadTermParam:              .word   0x000200bc
Ddrc1_DRAM_PadTerm2Param:             .word   0x0a059a03
Ddrc1_DRAM_PadTerm3Param:             .word   0x00aa7000

Ddrc0_DRAM_lpddr4DqCaVrefParam:       .word   0x03A74E9D
Ddrc1_DRAM_lpddr4DqCaVrefParam:       .word   0x03A74E9D
Ddrc0_DRAM_lpddr4DqWriteDlyParam:     .word   0xC56AD62D
Ddrc1_DRAM_lpddr4DqWriteDlyParam:     .word   0xC56AD62D
Ddrc0_DRAM_lpddr4DqReadDlyParam:      .word   0x000294E7
Ddrc1_DRAM_lpddr4DqReadDlyParam:      .word   0x000294E7
Ddrc0_DRAM_lpddr4TrainMpcRdlyParam:   .word   0x00000034
Ddrc1_DRAM_lpddr4TrainMpcRdlyParam:   .word   0x00000034

DRAM_DllCalibParam:                   .word   0x001e3204

/* LPDDR4 Initializations */
LPDDR4_ModeReg1ParamFsp0:             .word   0x01010064
LPDDR4_ModeReg2ParamFsp0:             .word   0x01020036
LPDDR4_ModeReg13ParamFsp0:            .word   0x010d0000
Ddrc0_LPDDR4_ModeReg3ParamFsp0:       .word   0x01030033
Ddrc1_LPDDR4_ModeReg3ParamFsp0:       .word   0x01030033
Ddrc0_LPDDR4_ModeReg11ParamFsp0:      .word   0x010b0024
Ddrc1_LPDDR4_ModeReg11ParamFsp0:      .word   0x010b0024
Ddrc0_LPDDR4_ModeReg12ParamFsp0:      .word   0x010c0008
Ddrc1_LPDDR4_ModeReg12ParamFsp0:      .word   0x010c0008
Ddrc0_LPDDR4_ModeReg14ParamFsp0:      .word   0x010e0008
Ddrc1_LPDDR4_ModeReg14ParamFsp0:      .word   0x010e0008
Ddrc0_LPDDR4_ModeReg22ParamFsp0:      .word   0x01160000
Ddrc1_LPDDR4_ModeReg22ParamFsp0:      .word   0x01160000

/* BST Training
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



/*
 * BLD Training
 * We separated this section to save the size of BST
 */
        .global Ddrc1_DRAM_lpddr4DqCaVrefOffsetParam
        .global Ddrc0_DRAM_lpddr4DqCaVrefOffsetParam
        .global Ddrc1_DRAM_lpddr4Mr12OffsetParam
        .global Ddrc0_DRAM_lpddr4Mr12OffsetParam
        .global Ddrc0_DRAM_lpddr4Mr14OffsetParam;
        .global Ddrc1_DRAM_lpddr4Mr14OffsetParam;
        .global Ddrc0_DRAM_lpddr4DqWriteOffsetParam;
        .global Ddrc1_DRAM_lpddr4DqWriteOffsetParam;
        .global Ddrc0_DRAM_lpddr4DqReadOffsetParam;
        .global Ddrc1_DRAM_lpddr4DqReadOffsetParam;
        .global Ddrc0_DRAM_lpddr4DllOffsetParam;
        .global Ddrc1_DRAM_lpddr4DllOffsetParam;

    .section BstDramParam.bld

Ddrc0_DRAM_lpddr4DqCaVrefOffsetParam: .word   0x0
Ddrc1_DRAM_lpddr4DqCaVrefOffsetParam: .word   0x0
Ddrc0_DRAM_lpddr4Mr12OffsetParam:     .word   0x0
Ddrc1_DRAM_lpddr4Mr12OffsetParam:     .word   0x0
Ddrc0_DRAM_lpddr4Mr14OffsetParam:     .word   0x00        /* 8-bit for ALL bytes. bit[7]:sign bit. ..., 0x82=-2, 0x81=-1, 0x80=0x0=0, 0x1=1, 0x2=2, ... */
Ddrc1_DRAM_lpddr4Mr14OffsetParam:     .word   0x00        /* 8-bit for ALL bytes. bit[7]:sign bit. ..., 0x82=-2, 0x81=-1, 0x80=0x0=0, 0x1=1, 0x2=2, ... */
Ddrc0_DRAM_lpddr4DqWriteOffsetParam:  .word   0x00000000  /* 8-bit for each byte. bit[7]:sign bit. ..., 0x82=-2, 0x81=-1, 0x80=0x0=0, 0x1=1, 0x2=2, ... */
Ddrc1_DRAM_lpddr4DqWriteOffsetParam:  .word   0x00000000  /* 8-bit for each byte. bit[7]:sign bit. ..., 0x82=-2, 0x81=-1, 0x80=0x0=0, 0x1=1, 0x2=2, ... */
Ddrc0_DRAM_lpddr4DqReadOffsetParam:   .word   0x00000000  /* 8-bit for each byte. bit[7]:sign bit. ..., 0x82=-2, 0x81=-1, 0x80=0x0=0, 0x1=1, 0x2=2, ... */
Ddrc1_DRAM_lpddr4DqReadOffsetParam:   .word   0x00000000  /* 8-bit for each byte. bit[7]:sign bit. ..., 0x82=-2, 0x81=-1, 0x80=0x0=0, 0x1=1, 0x2=2, ... */
Ddrc0_DRAM_lpddr4DllOffsetParam:      .word   0x00000000  /* 8-bit for each byte. bit[7]:sign bit. ..., 0x82=-2, 0x81=-1, 0x80=0x0=0, 0x1=1, 0x2=2, ... */
Ddrc1_DRAM_lpddr4DllOffsetParam:      .word   0x00000000  /* 8-bit for each byte. bit[7]:sign bit. ..., 0x82=-2, 0x81=-1, 0x80=0x0=0, 0x1=1, 0x2=2, ... */

