/**
 *  @file AmbaDDR4_1596Mhz.asm
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

        .global DRAM_DllCalibParam
        .global DRAM_PowerDownParam

        /* DDR4 Initializations */
        .global DDR4_ModeReg0Param
        .global DDR4_ModeReg0Param_DLL_Rst
        .global DDR4_ModeReg1Param
        .global DDR4_ModeReg2Param
        .global DDR4_ModeReg3Param
        .global DDR4_ModeReg4Param
        .global DDR4_ModeReg5Param
        .global DDR4_ModeReg6Param

        .section BstDramParam

/* Cortex, Core, and DDR PLL parameters */
PLL_CortexCtrlParam:            .word   0x29100000
PLL_CortexCtrl2Param:           .word   0x30520000
PLL_CortexCtrl3Param:           .word   0x00088004

PLL_CoreCtrlParam:              .word   0x12111000
PLL_CoreCtrl2Param:             .word   0x30520000
PLL_CoreCtrl3Param:             .word   0x00088004

PLL_DdrCtrlParam:               .word   0x25116000
PLL_DdrCtrl2Param:              .word   0x30528000
PLL_DdrCtrl3Param:              .word   0x00088006

DRAMC_ModeParam:                .word   0x0000000c

/* DLL parameters */
DLL_ResetDelay:                 .word   0x1000

DLL_Setting0Param:              .word   0x251f25
DLL_Setting1Param:              .word   0x251f25
DLL_Setting2Param:              .word   0x251f25
DLL_Setting3Param:              .word   0x251f25

DLL_CtrlSel0Param:              .word   0x03b9b1
DLL_CtrlSel1Param:              .word   0x03b9b1
DLL_CtrlSel2Param:              .word   0x03b9b1
DLL_CtrlSel3Param:              .word   0x03b9b1
DLL_CtrlSelMiscParam:           .word   0x248000
DLL_CtrlSyncTrackParam:         .word   0x0000b2c1 /* leave default value for ddr4 */

/* DDR Controller parameters */
DRAM_PorDelay:                  .word   0x2710

DRAM_CtrlParam:                 .word   0x60007280
DRAM_ConfigParam:               .word   0x681801b4
DRAM_Timing1Param:              .word   0x6890a289
DRAM_Timing2Param:              .word   0x1aa4616a
DRAM_Timing3Param:              .word   0x0000030c
DRAM_Timing4Param:              .word   0x1285ff2e
DRAM_Timing5Lp4TrainingParam:   .word   0x0058a84c
DRAM_ByteMapParam:              .word   0x000000e4

DRAM_DqsSyncPreRttParam:        .word   0x0301ea00
DRAM_DqsSyncParam:              .word   0x0301ea00
DRAM_ZqCalibParam:              .word   0x00000060

DRAM_PadTermParam:              .word   0x0002007E
DRAM_PadTerm2Param:             .word   0x05055705
DRAM_PadTerm3Param:             .word   0x00775000

DRAM_DllCalibParam:             .word   0x001e3204
DRAM_PowerDownParam:            .word   0x00000030

/* LPDDR4 Initializations */
DDR4_ModeReg0Param:           .word   0x01000c51
DDR4_ModeReg0Param_DLL_Rst:   .word   0x01000d51
DDR4_ModeReg1Param:           .word   0x01010503
DDR4_ModeReg2Param:           .word   0x01020028
DDR4_ModeReg3Param:           .word   0x01030400
DDR4_ModeReg4Param:           .word   0x01040000
DDR4_ModeReg5Param:           .word   0x01050400
DDR4_ModeReg6Param:           .word   0x01061014
