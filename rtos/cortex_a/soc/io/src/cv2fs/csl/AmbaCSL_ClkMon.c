/**
 *  @file AmbaCSL_ClkMon.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Chip Support Library (CSL) for Clock Monitor
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"

#include "AmbaCSL_RCT.h"
#include "AmbaCSL_ClkMon.h"
#include "AmbaCSL_Scratchpad.h"
#include "AmbaClkMon_Def.h"

static void ClkMon_writel(ULONG Address, UINT32 Value)
{
    volatile UINT32 *uint32_ptr;
    AmbaMisra_TypeCast(&uint32_ptr, &Address);
    *uint32_ptr = Value;
}

static UINT32 ClkMon_readl(ULONG Address)
{
    volatile const UINT32 *uint32_ptr;
    AmbaMisra_TypeCast(&uint32_ptr, &Address);

    return (*uint32_ptr);
}

static void writel_24_12_0_12(UINT32 Address, UINT32 Enable, UINT32 UpperBound, UINT32 LowerBound)
{
    UINT32 value_uint32;

    value_uint32 = (Enable << 24U) | ((UpperBound & 0x0FFFU) << 12U) | (LowerBound & 0x0FFFU);
    ClkMon_writel(Address, value_uint32);
}

static void writel_31_16_0_12(UINT32 Address, UINT32 Enable, UINT32 UpperBound, UINT32 LowerBound)
{
    UINT32 value_uint32;

    value_uint32 = (Enable << 31U) | ((UpperBound & 0x0FFFU) << 16U) | (LowerBound & 0x0FFFU);
    ClkMon_writel(Address, value_uint32);
}

static void writel_31_16_0_15(UINT32 Address, UINT32 Enable, UINT32 UpperBound, UINT32 LowerBound)
{
    UINT32 value_uint32;

    value_uint32 = (Enable << 31U) | ((UpperBound & 0x07FFFU) << 16U) | (LowerBound & 0x07FFFU);
    ClkMon_writel(Address, value_uint32);
}

static void readl_24_12_0_12(UINT32 Address, UINT8 *Enable, UINT16 *UpperBound, UINT16 *LowerBound)
{
    UINT32 value_uint32 = ClkMon_readl(Address);

    *Enable = (UINT8)((value_uint32 >> 24U) & 0x01U);
    *UpperBound = (UINT16)((value_uint32 >> 12U) & 0x0FFFU);
    *LowerBound = (UINT16)(value_uint32 & 0x0FFFU);
}

static void readl_31_16_0_12(UINT32 Address, UINT8 *Enable, UINT16 *UpperBound, UINT16 *LowerBound)
{
    UINT32 value_uint32 = ClkMon_readl(Address);

    *Enable = (UINT8)((value_uint32 >> 31U) & 0x01U);
    *UpperBound = (UINT16)((value_uint32 >> 16U) & 0x0FFFU);
    *LowerBound = (UINT16)(value_uint32 & 0x0FFFU);
}

static void readl_31_16_0_15(UINT32 Address, UINT8 *Enable, UINT16 *UpperBound, UINT16 *LowerBound)
{
    UINT32 value_uint32 = ClkMon_readl(Address);

    *Enable = (UINT8)((value_uint32 >> 31U) & 0x01U);
    *UpperBound = (UINT16)((value_uint32 >> 16U) & 0x07FFFU);
    *LowerBound = (UINT16)(value_uint32 & 0x07FFFU);
}

typedef struct {
    UINT32  LowerBound:                12;      /* [11:0] Lower bound value */
    UINT32  UpperBound:                12;      /* [23:12] Upper bound value */
    UINT32  VpaEnable:                  1;      /* [24]  */
    UINT32  VpbEnable:                  1;      /* [25]  */
    UINT32  VpcEnable:                  1;      /* [26]  */
    UINT32  VpdEnable:                  1;      /* [27]  */
    UINT32  VpfEnable:                  1;      /* [28]  */
    UINT32  Reserved:                   3;      /* [31:29]  */
} AMBA_CKM_VP_VISION_CTRL_REG_s;

#define FLAG_ACT_SET_CFG (0U)
#define FLAG_ACT_GET_CFG (1U)
#define FLAG_ACT_GET_CNT (2U)

typedef void (*CKM_CTRL_SET_FUNC_f)(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count);
typedef struct {
    UINT32 ID;
    CKM_CTRL_SET_FUNC_f FuncAct;
} CKM_CTRL_FUNC_s;

static void ref_clk_ctrl_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_15(0xED0D0040U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_15(0xED0D0040U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void osc_ctrl_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_15(0xED0D0044U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_15(0xED0D0044U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void ssi2_ctrl_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkSsi2Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkSsi2Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkSsi2Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkSsi2Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkSsi2Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkSsi2Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscSsi2Count.CycleCount;
    } else {
        // do nothing
    }
}

static void ssi3_ctrl_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkSsi3Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkSsi3Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkSsi3Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkSsi3Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkSsi3Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkSsi3Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscSsi3Count.CycleCount;
    } else {
        // do nothing
    }
}

static void core_ahb_ctrl_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkCoreAhbCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkCoreAhbCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkCoreAhbCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkCoreAhbCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkCoreAhbCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkCoreAhbCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscCoreAhbCount.CycleCount;
    } else {
        // do nothing
    }
}

static void core_dft_ctrl_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkCoreCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkCoreCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkCoreCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkCoreCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkCoreCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkCoreCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscCoreCount.CycleCount;
    } else {
        // do nothing
    }
}

static void ahb_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkAhbCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkAhbCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkAhbCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkAhbCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkAhbCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkAhbCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscAhbCount.CycleCount;
    } else {
        // do nothing
    }
}

static void apb_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkApbCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkApbCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkApbCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkApbCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkApbCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkApbCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscApbCount.CycleCount;
    } else {
        // do nothing
    }
}

static void dbg_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkDbgCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkDbgCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkDbgCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkDbgCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkDbgCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkDbgCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscDbgCount.CycleCount;
    } else {
        // do nothing
    }
}

static void sdio_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkSdio0Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkSdio0Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkSdio0Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkSdio0Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkSdio0Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkSdio0Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscSdio0Count.CycleCount;
    } else {
        // do nothing
    }
}

static void sd48_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkSd48Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkSd48Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkSd48Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkSd48Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkSd48Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkSd48Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscSd48Count.CycleCount;
    } else {
        // do nothing
    }
}

static void nand_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkNandCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkNandCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkNandCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkNandCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkNandCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkNandCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscNandCount.CycleCount;
    } else {
        // do nothing
    }
}

static void n0_syscnt_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkSysCnt0Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkSysCnt0Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkSysCnt0Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkSysCnt0Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkSysCnt0Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkSysCnt0Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscSysCnt0Count.CycleCount;
    } else {
        // do nothing
    }
}

static void n1_syscnt_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkSysCnt1Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkSysCnt1Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkSysCnt1Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkSysCnt1Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkSysCnt1Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkSysCnt1Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscSysCnt1Count.CycleCount;
    } else {
        // do nothing
    }
}

static void can_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkCanCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkCanCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkCanCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkCanCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkCanCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkCanCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscCanCount.CycleCount;
    } else {
        // do nothing
    }
}

static void au_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkAuCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkAuCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkAuCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkAuCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkAuCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkAuCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscAuCount.CycleCount;
    } else {
        // do nothing
    }
}

static void gtx_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkGtxCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkGtxCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkGtxCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkGtxCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkGtxCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkGtxCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscGtxCount.CycleCount;
    } else {
        // do nothing
    }
}

static void axi1_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkAxi1Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkAxi1Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkAxi1Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkAxi1Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkAxi1Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkAxi1Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscAxi1Count.CycleCount;
    } else {
        // do nothing
    }
}

static void vision_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkVisionCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkVisionCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkVisionCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkVisionCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkVisionCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkVisionCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMisVisionCount.CycleCount;
    } else {
        // do nothing
    }
}

static void dram_div2_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkDramDiv2Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkDramDiv2Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkDramDiv2Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkDramDiv2Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkDramDiv2Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkDramDiv2Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMisDramDiv2Count.CycleCount;
    } else {
        // do nothing
    }
}

static void fex_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkFexCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkFexCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkFexCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkFexCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkFexCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkFexCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMisFexCount.CycleCount;
    } else {
        // do nothing
    }
}

static void amba1_dbg2dbg_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclk1DbgCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclk1DbgCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclk1DbgCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclk1DbgCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclk1DbgCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclk1DbgCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMis1DbgCount.CycleCount;
    } else {
        // do nothing
    }
}

static void amba1_core_ahb_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclk1CoreAhbCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclk1CoreAhbCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclk1CoreAhbCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclk1CoreAhbCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclk1CoreAhbCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclk1CoreAhbCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMis1CoreAhbCount.CycleCount;
    } else {
        // do nothing
    }
}

static void amba1_core_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclk1CoreCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclk1CoreCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclk1CoreCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclk1CoreCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclk1CoreCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclk1CoreCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMis1CoreCount.CycleCount;
    } else {
        // do nothing
    }
}

static void amba1_ahb_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclk1AhbCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclk1AhbCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclk1AhbCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclk1AhbCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclk1AhbCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclk1AhbCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMis1AhbCount.CycleCount;
    } else {
        // do nothing
    }
}

static void amba1_axi1_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclk1Axi1Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclk1Axi1Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclk1Axi1Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclk1Axi1Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclk1Axi1Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclk1Axi1Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMis1Axi1Count.CycleCount;
    } else {
        // do nothing
    }
}

static void smem_core_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED05E140U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED05E140U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void eorc_core_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED020310U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED020310U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void eorc_dbg_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED020318U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED020318U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void corc_core_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED160310U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED160310U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void corc_dbg_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED160318U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED160318U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void vdsp_core_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED020320U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED020320U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void vdsp_dbg_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED020328U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED020328U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void idsp0_idsp_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED1CFFFCU, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED1CFFFCU, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void idsp0_core_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED1CFFF8U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED1CFFF8U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void idsp0_dbg_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED1CFFF4U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED1CFFF4U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void idsp1_idsp_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED1CFFFCU, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED1CFFFCU, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void idsp1_core_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED1CFFF8U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED1CFFF8U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void idsp1_dbg_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED1CFFF4U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED1CFFF4U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void storc_vorc_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED010384U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED010384U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void storc_l2c_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED0A00CCU, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED0A00CCU, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void fex_fex_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED255FF8U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED255FF8U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void vmem_vision_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_31_16_0_12(0xED13FFE0U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_31_16_0_12(0xED13FFE0U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void vpa_vision_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    ULONG RegAddr = 0xED82007CUL;
    volatile AMBA_CKM_VP_VISION_CTRL_REG_s *vp_vision_ctrl;
    if (FlagAct == FLAG_ACT_SET_CFG) {
        AmbaMisra_TypeCast(&vp_vision_ctrl, &RegAddr);
        vp_vision_ctrl->UpperBound = CtrlValue->UpperBound;
        vp_vision_ctrl->LowerBound = CtrlValue->LowerBound;
        vp_vision_ctrl->VpaEnable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        AmbaMisra_TypeCast(&vp_vision_ctrl, &RegAddr);
        CtrlValue->UpperBound = vp_vision_ctrl->UpperBound;
        CtrlValue->LowerBound = vp_vision_ctrl->LowerBound;
        CtrlValue->ClkMonEnable = vp_vision_ctrl->VpaEnable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void vpa_dbg_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        writel_24_12_0_12(0xED820080U, CtrlValue->ClkMonEnable, CtrlValue->UpperBound, CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        readl_24_12_0_12(0xED820080U, &CtrlValue->ClkMonEnable, &CtrlValue->UpperBound, &CtrlValue->LowerBound);
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void vpb_vision_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    ULONG RegAddr = 0xED82007CUL;
    volatile AMBA_CKM_VP_VISION_CTRL_REG_s *vp_vision_ctrl;
    if (FlagAct == FLAG_ACT_SET_CFG) {
        AmbaMisra_TypeCast(&vp_vision_ctrl, &RegAddr);
        vp_vision_ctrl->UpperBound = CtrlValue->UpperBound;
        vp_vision_ctrl->LowerBound = CtrlValue->LowerBound;
        vp_vision_ctrl->VpbEnable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        AmbaMisra_TypeCast(&vp_vision_ctrl, &RegAddr);
        CtrlValue->UpperBound = vp_vision_ctrl->UpperBound;
        CtrlValue->LowerBound = vp_vision_ctrl->LowerBound;
        CtrlValue->ClkMonEnable = vp_vision_ctrl->VpbEnable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void vpc_vision_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    ULONG RegAddr = 0xED82007CUL;
    volatile AMBA_CKM_VP_VISION_CTRL_REG_s *vp_vision_ctrl;
    if (FlagAct == FLAG_ACT_SET_CFG) {
        AmbaMisra_TypeCast(&vp_vision_ctrl, &RegAddr);
        vp_vision_ctrl->UpperBound = CtrlValue->UpperBound;
        vp_vision_ctrl->LowerBound = CtrlValue->LowerBound;
        vp_vision_ctrl->VpcEnable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        AmbaMisra_TypeCast(&vp_vision_ctrl, &RegAddr);
        CtrlValue->UpperBound = vp_vision_ctrl->UpperBound;
        CtrlValue->LowerBound = vp_vision_ctrl->LowerBound;
        CtrlValue->ClkMonEnable = vp_vision_ctrl->VpcEnable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void vpd_vision_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    ULONG RegAddr = 0xED82007CUL;
    volatile AMBA_CKM_VP_VISION_CTRL_REG_s *vp_vision_ctrl;
    if (FlagAct == FLAG_ACT_SET_CFG) {
        AmbaMisra_TypeCast(&vp_vision_ctrl, &RegAddr);
        vp_vision_ctrl->UpperBound = CtrlValue->UpperBound;
        vp_vision_ctrl->LowerBound = CtrlValue->LowerBound;
        vp_vision_ctrl->VpdEnable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        AmbaMisra_TypeCast(&vp_vision_ctrl, &RegAddr);
        CtrlValue->UpperBound = vp_vision_ctrl->UpperBound;
        CtrlValue->LowerBound = vp_vision_ctrl->LowerBound;
        CtrlValue->ClkMonEnable = vp_vision_ctrl->VpdEnable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void vpf_vision_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    ULONG RegAddr = 0xED82007CUL;
    volatile AMBA_CKM_VP_VISION_CTRL_REG_s *vp_vision_ctrl;
    if (FlagAct == FLAG_ACT_SET_CFG) {
        AmbaMisra_TypeCast(&vp_vision_ctrl, &RegAddr);
        vp_vision_ctrl->UpperBound = CtrlValue->UpperBound;
        vp_vision_ctrl->LowerBound = CtrlValue->LowerBound;
        vp_vision_ctrl->VpfEnable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        AmbaMisra_TypeCast(&vp_vision_ctrl, &RegAddr);
        CtrlValue->UpperBound = vp_vision_ctrl->UpperBound;
        CtrlValue->LowerBound = vp_vision_ctrl->LowerBound;
        CtrlValue->ClkMonEnable = vp_vision_ctrl->VpfEnable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = 0;
    } else {
        // do nothing
    }
}

static void ssi_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkSsiCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkSsiCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkSsiCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkSsiCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkSsiCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkSsiCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscSsiCount.CycleCount;
    } else {
        // do nothing
    }
}

static void pllout_core_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonPllOutCoreCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonPllOutCoreCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonPllOutCoreCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonPllOutCoreCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonPllOutCoreCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonPllOutCoreCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaRCT_Reg->ClkMonMiscCoreCount.CycleCount;
    } else {
        // do nothing
    }
}

static void pllout_sd_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonPllOutSDCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonPllOutSDCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonPllOutSDCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonPllOutSDCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonPllOutSDCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonPllOutSDCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaRCT_Reg->ClkMonMiscSDCount.CycleCount;
    } else {
        // do nothing
    }
}

static void pllout_enet_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonPllOutENETCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonPllOutENETCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonPllOutENETCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonPllOutENETCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonPllOutENETCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonPllOutENETCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaRCT_Reg->ClkMonMiscENETCount.CycleCount;
    } else {
        // do nothing
    }
}

static void pllout_audio_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonPllOutAudioCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonPllOutAudioCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonPllOutAudioCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonPllOutAudioCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonPllOutAudioCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonPllOutAudioCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaRCT_Reg->ClkMonMiscAudioCount.CycleCount;
    } else {
        // do nothing
    }
}

static void pllout_ddr_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonPllOutDDRCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonPllOutDDRCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonPllOutDDRCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonPllOutDDRCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonPllOutDDRCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonPllOutDDRCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaRCT_Reg->ClkMonMiscDDRCount.CycleCount;
    } else {
        // do nothing
    }
}

static void pllout_cortex0_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonPllOutCortex0Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonPllOutCortex0Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonPllOutCortex0Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonPllOutCortex0Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonPllOutCortex0Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonPllOutCortex0Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaRCT_Reg->ClkMonMiscCortex0Count.CycleCount;
    } else {
        // do nothing
    }
}

static void pllout_cortex1_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonPllOutCortex1Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonPllOutCortex1Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonPllOutCortex1Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonPllOutCortex1Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonPllOutCortex1Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonPllOutCortex1Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaRCT_Reg->ClkMonMiscCortex1Count.CycleCount;
    } else {
        // do nothing
    }
}

static void pllout_fex_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonPllOutFEXCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonPllOutFEXCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonPllOutFEXCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonPllOutFEXCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonPllOutFEXCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonPllOutFEXCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        // TBD
        *Count = pAmbaRCT_Reg->ClkMonMiscFEXCount.CycleCount;
    } else {
        // do nothing
    }
}

static void pllout_vision_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonPllOutVisionCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonPllOutVisionCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonPllOutVisionCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonPllOutVisionCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonPllOutVisionCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonPllOutVisionCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaRCT_Reg->ClkMonMiscVisionCount.CycleCount;
    } else {
        // do nothing
    }
}

static void pllout_nand_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonPllOutNANDCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonPllOutNANDCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonPllOutNANDCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonPllOutNANDCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonPllOutNANDCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonPllOutNANDCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaRCT_Reg->ClkMonMiscNANDCount.CycleCount;
    } else {
        // do nothing
    }
}

static void pllout_idsp_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonPllOutIDSPCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonPllOutIDSPCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonPllOutIDSPCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonPllOutIDSPCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonPllOutIDSPCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonPllOutIDSPCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaRCT_Reg->ClkMonMiscIDSPCount.CycleCount;
    } else {
        // do nothing
    }
}

static void pllout_ddr_init_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonPllOutDDRInitCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonPllOutDDRInitCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonPllOutDDRInitCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonPllOutDDRInitCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonPllOutDDRInitCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonPllOutDDRInitCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaRCT_Reg->ClkMonMiscDDRInitCount.CycleCount;
    } else {
        // do nothing
    }
}

static void gclk_so_vin0_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonGclkSOVIN0Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonGclkSOVIN0Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonGclkSOVIN0Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonGclkSOVIN0Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonGclkSOVIN0Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonGclkSOVIN0Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaRCT_Reg->ClkMonMiscSOVIN0Count.CycleCount;
    } else {
        // do nothing
    }
}

static void gclk_so_vin1_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonGclkSOVIN1Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonGclkSOVIN1Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonGclkSOVIN1Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonGclkSOVIN1Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonGclkSOVIN1Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonGclkSOVIN1Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaRCT_Reg->ClkMonMiscSOVIN1Count.CycleCount;
    } else {
        // do nothing
    }
}

static void gclk_dbg_out_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonGclkDbgCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonGclkDbgCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonGclkDbgCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonGclkDbgCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonGclkDbgCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonGclkDbgCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaRCT_Reg->ClkMonGclkDbgCount.CycleCount;
    } else {
        // do nothing
    }
}

static void gclk_so_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonGclkSOCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonGclkSOCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonGclkSOCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonGclkSOCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonGclkSOCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonGclkSOCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaRCT_Reg->ClkMonMiscGclkSOCount.CycleCount;
    } else {
        // do nothing
    }
}

static void gclk_so2_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonGclkSO2Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonGclkSO2Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonGclkSO2Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonGclkSO2Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonGclkSO2Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonGclkSO2Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaRCT_Reg->ClkMonMiscGclkSO2Count.CycleCount;
    } else {
        // do nothing
    }
}

static void uart0_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkUart0Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkUart0Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkUart0Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkUart0Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkUart0Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkUart0Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscUart0Count.CycleCount;
    } else {
        // do nothing
    }
}

static void uart1_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkUart1Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkUart1Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkUart1Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkUart1Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkUart1Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkUart1Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscUart1Count.CycleCount;
    } else {
        // do nothing
    }
}

static void uart2_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkUart2Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkUart2Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkUart2Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkUart2Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkUart2Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkUart2Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscUart2Count.CycleCount;
    } else {
        // do nothing
    }
}

static void uart3_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkUart3Ctrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkUart3Ctrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkUart3Ctrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkUart3Ctrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkUart3Ctrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkUart3Ctrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscUart3Count.CycleCount;
    } else {
        // do nothing
    }
}

static void uart_apb_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkUartApbCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkUartApbCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkUartApbCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkUartApbCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkUartApbCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkUartApbCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscUartApbCount.CycleCount;
    } else {
        // do nothing
    }
}

static void pwm_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkPwmCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkPwmCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkPwmCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkPwmCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkPwmCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkPwmCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscPwmCount.CycleCount;
    } else {
        // do nothing
    }
}

static void vo_a_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkVoACtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkVoACtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkVoACtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkVoACtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkVoACtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkVoACtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscVoACount.CycleCount;
    } else {
        // do nothing
    }
}

static void vo_b_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonGclkVoBCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkVoBCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonGclkVoBCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonGclkVoBCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonGclkVoBCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonGclkVoBCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscVoBCount.CycleCount;
    } else {
        // do nothing
    }
}

static void cbphy_a_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonComphyACtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonComphyACtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonComphyACtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonComphyACtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonComphyACtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonComphyACtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscComphyACount.CycleCount;
    } else {
        // do nothing
    }
}

static void cbphy_b_dft_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaScratchpadNS_Reg->ClkMonComphyBCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaScratchpadNS_Reg->ClkMonComphyBCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaScratchpadNS_Reg->ClkMonComphyBCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaScratchpadNS_Reg->ClkMonComphyBCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaScratchpadNS_Reg->ClkMonComphyBCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaScratchpadNS_Reg->ClkMonComphyBCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaScratchpadNS_Reg->ClkMonMiscComphyBCount.CycleCount;
    } else {
        // do nothing
    }
}

static void pllout_video_a_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonPllOutVideoACtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonPllOutVideoACtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonPllOutVideoACtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonPllOutVideoACtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonPllOutVideoACtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonPllOutVideoACtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaRCT_Reg->ClkMonMiscVideoACount.CycleCount;
    } else {
        // do nothing
    }
}

static void pllout_video_b_func(UINT8 FlagAct, AMBA_CLK_MON_CTRL_REGS_VAL_s *CtrlValue, UINT32 *Count)
{
    if (FlagAct == FLAG_ACT_SET_CFG) {
        pAmbaRCT_Reg->ClkMonPllOutVideoBCtrl.LowerBound = CtrlValue->LowerBound;
        pAmbaRCT_Reg->ClkMonPllOutVideoBCtrl.UpperBound = CtrlValue->UpperBound;
        pAmbaRCT_Reg->ClkMonPllOutVideoBCtrl.Enable = CtrlValue->ClkMonEnable;
    } else if (FlagAct == FLAG_ACT_GET_CFG) {
        CtrlValue->LowerBound = pAmbaRCT_Reg->ClkMonPllOutVideoBCtrl.LowerBound;
        CtrlValue->UpperBound = pAmbaRCT_Reg->ClkMonPllOutVideoBCtrl.UpperBound;
        CtrlValue->ClkMonEnable = pAmbaRCT_Reg->ClkMonPllOutVideoBCtrl.Enable;
    } else if (FlagAct == FLAG_ACT_GET_CNT) {
        *Count = pAmbaRCT_Reg->ClkMonMiscVideoBCount.CycleCount;
    } else {
        // do nothing
    }
}

static CKM_CTRL_FUNC_s ckm_ctrl_func_array[] = {
    {0, NULL},
    {AMBA_CKM_IDX_DDRH0_DRAM_DIV2, NULL},            // 1, CortexA can't access DDRC
    {AMBA_CKM_IDX_DDRH0_DRAM,      NULL},            // 2, CortexA can't access DDRC
    {AMBA_CKM_IDX_DDRHE_DRAM_DIV2, NULL},            // 3, CortexA can't access DDRC
    {AMBA_CKM_IDX_DDRHE_DRAM,      NULL},            // 4, CortexA can't access DDRC
    {AMBA_CKM_IDX_REF_CLK,         ref_clk_ctrl_func}, // 5
    {AMBA_CKM_IDX_OSC,             osc_ctrl_func},     // 6
    {AMBA_CKM_IDX_CORTEX0,         NULL},             // 7, CortexA can't access AXI
    {AMBA_CKM_IDX_CORTEX1,         NULL},             // 8, CortexA can't access AXI
    {AMBA_CKM_IDX_AXI1,            NULL},             // 9, CortexA can't access AXI
    {AMBA_CKM_IDX_N0_PCLK_DBG,     NULL},             // 10, CortexA can't access AXI
    {AMBA_CKM_IDX_N1_PCLK_DBG,     NULL},             // 11, CortexA can't access AXI
    {AMBA_CKM_IDX_SSI2_DFT,        ssi2_ctrl_func},    // 12
    {AMBA_CKM_IDX_SSI3_DFT,        ssi3_ctrl_func},    // 13
    {AMBA_CKM_IDX_CORE_AHB,        core_ahb_ctrl_func}, // 14
    {AMBA_CKM_IDX_CORE_DFT,        core_dft_ctrl_func}, // 15
    {AMBA_CKM_IDX_AHB_DFT,         ahb_dft_func},       // 16
    {AMBA_CKM_IDX_APB_DFT,         apb_dft_func},       // 17
    {AMBA_CKM_IDX_DBG_DFT,         dbg_dft_func},       // 18
    {AMBA_CKM_IDX_SDIO_DFT,        sdio_dft_func},      // 19
    {AMBA_CKM_IDX_SD48_DFT,        sd48_dft_func},      // 20
    {AMBA_CKM_IDX_NAND_DFT,        nand_dft_func},      // 21
    {AMBA_CKM_IDX_N0_SYSCNT_DFT,   n0_syscnt_dft_func}, // 22
    {AMBA_CKM_IDX_N1_SYSCNT_DFT,   n1_syscnt_dft_func}, // 23
    {AMBA_CKM_IDX_CAN_DFT,         can_dft_func},       // 24
    {AMBA_CKM_IDX_AU_DFT,          au_dft_func},        // 25
    {AMBA_CKM_IDX_GTX_DFT,         gtx_dft_func},       // 26
    {AMBA_CKM_IDX_AXI1_DFT,        axi1_dft_func},      // 27
    {AMBA_CKM_IDX_VISION_DFT,      vision_dft_func},    // 28
    {AMBA_CKM_IDX_DRAM_DIV2_DFT,   dram_div2_dft_func}, // 29
    {AMBA_CKM_IDX_FEX_DFT,         fex_dft_func},       // 30
    {AMBA_CKM_IDX_AMBA1_DBG2DBG,   amba1_dbg2dbg_func}, // 31
    {AMBA_CKM_IDX_AMBA1_CORE_AHB,  amba1_core_ahb_func}, // 32
    {AMBA_CKM_IDX_AMBA1_CORE_DFT,  amba1_core_dft_func}, // 33
    {AMBA_CKM_IDX_AMBA1_AHB_DFT,   amba1_ahb_dft_func},  // 34
    {AMBA_CKM_IDX_AMBA1_AXI1_DFT,  amba1_axi1_dft_func}, // 35
    {AMBA_CKM_IDX_SMEM_CORE,       smem_core_func},      // 36
    {AMBA_CKM_IDX_EORC_CORE,       eorc_core_func},      // 37
    {AMBA_CKM_IDX_EORC_DBG,        eorc_dbg_func},       // 38
    {AMBA_CKM_IDX_CORC_CORE,       corc_core_func},      // 39
    {AMBA_CKM_IDX_CORC_DBG,        corc_dbg_func},       // 40
    {AMBA_CKM_IDX_VDSP_CORE,       vdsp_core_func},      // 41
    {AMBA_CKM_IDX_VDSP_DBG,        vdsp_dbg_func},       // 42
    {AMBA_CKM_IDX_IDSP0_IDSP,      idsp0_idsp_func},     // 43
    {AMBA_CKM_IDX_IDSP0_CORE,      idsp0_core_func},     // 44
    {AMBA_CKM_IDX_IDSP0_DBG,       idsp0_dbg_func},      // 45
    {AMBA_CKM_IDX_IDSP1_IDSP,      idsp1_idsp_func},     // 46
    {AMBA_CKM_IDX_IDSP1_CORE,      idsp1_core_func},     // 47
    {AMBA_CKM_IDX_IDSP1_DBG,       idsp1_dbg_func},      // 48
    {AMBA_CKM_IDX_STORC_VORC,      storc_vorc_func},     // 49
    {AMBA_CKM_IDX_STORC_L2C,       storc_l2c_func},      // 50
    {AMBA_CKM_IDX_FEX_FEX,         fex_fex_func},        // 51
    {AMBA_CKM_IDX_VMEM_VISION,     vmem_vision_func},    // 52
    {AMBA_CKM_IDX_VPA_VISION,      vpa_vision_func},     // 53
    {AMBA_CKM_IDX_VPA_DBG,         vpa_dbg_func},        // 54
    {AMBA_CKM_IDX_VPB_VISION,      vpb_vision_func},     // 55
    {AMBA_CKM_IDX_VPC_VISION,      vpc_vision_func},     // 56
    {AMBA_CKM_IDX_VPD_VISION,      vpd_vision_func},     // 57
    {AMBA_CKM_IDX_VPF_VISION,      vpf_vision_func},     // 58
    {AMBA_CKM_IDX_PLLOUT_CORE,     pllout_core_func},    // 59
    {AMBA_CKM_IDX_PLLOUT_SD,       pllout_sd_func},      // 60
    {AMBA_CKM_IDX_PLLOUT_ENET,     pllout_enet_func},    // 61
    {AMBA_CKM_IDX_PLLOUT_AUDIO,    pllout_audio_func},   // 62
    {AMBA_CKM_IDX_PLLOUT_DDR,      pllout_ddr_func},     // 63
    {AMBA_CKM_IDX_PLLOUT_CORTEX0,  pllout_cortex0_func}, // 64
    {AMBA_CKM_IDX_PLLOUT_CORTEX1,  pllout_cortex1_func}, // 65
    {AMBA_CKM_IDX_PLLOUT_FEX,      pllout_fex_func},     // 66
    {AMBA_CKM_IDX_PLLOUT_VISION,   pllout_vision_func},  // 67
    {AMBA_CKM_IDX_PLLOUT_NAND,     pllout_nand_func},    // 68
    {AMBA_CKM_IDX_PLLOUT_IDSP,     pllout_idsp_func},    // 69
    {AMBA_CKM_IDX_PLLOUT_DDR_INIT, pllout_ddr_init_func}, // 70
    {AMBA_CKM_IDX_GCLK_SO_VIN0,    gclk_so_vin0_func},    // 71
    {AMBA_CKM_IDX_GCLK_SO_VIN1,    gclk_so_vin1_func},    // 72
    {AMBA_CKM_IDX_GCLK_DBG_OUT,    gclk_dbg_out_func},    // 73
    {AMBA_CKM_IDX_GCLK_SO,         gclk_so_func},         // 74
    {AMBA_CKM_IDX_GCLK_SO2,        gclk_so2_func},        // 75
    {AMBA_CKM_IDX_SSI_DFT,         ssi_dft_func},         // 76
    {AMBA_CKM_IDX_UART0_DFT,       uart0_dft_func},       // 77
    {AMBA_CKM_IDX_UART1_DFT,       uart1_dft_func},       // 78
    {AMBA_CKM_IDX_UART2_DFT,       uart2_dft_func},       // 79
    {AMBA_CKM_IDX_UART3_DFT,       uart3_dft_func},       // 80
    {AMBA_CKM_IDX_UART_APB_DFT,    uart_apb_dft_func},    // 81
    {AMBA_CKM_IDX_PWM_DFT,         pwm_dft_func},         // 82
    {AMBA_CKM_IDX_VO_A_DFT,        vo_a_dft_func},        // 83
    {AMBA_CKM_IDX_VO_B_DFT,        vo_b_dft_func},        // 84
    {AMBA_CKM_IDX_CLKA_CBPHY_DFT,  cbphy_a_dft_func},     // 85
    {AMBA_CKM_IDX_CLKB_CBPHY_DFT,  cbphy_b_dft_func},     // 86
    {AMBA_CKM_IDX_PLLOUT_VIDEO_A,  pllout_video_a_func},  // 87
    {AMBA_CKM_IDX_PLLOUT_VIDEO_B,  pllout_video_b_func}   // 88
};

/**
 *  AmbaCSL_ClkMonGetCtrl - Get PLL Control register values
 *  @param[in] ClkMonID CLKMON ID
 *  @param[in] pNewRegsVal A pointer to load PLL control register values
 */
void AmbaCSL_ClkMonGetCtrl(UINT32 ClkMonID, AMBA_CLK_MON_CTRL_REGS_VAL_s *pNewRegsVal)
{
    UINT32 max_count = (UINT32)(sizeof(ckm_ctrl_func_array) / sizeof(CKM_CTRL_FUNC_s));
    UINT32 count;

    if (ClkMonID < max_count) {
        const CKM_CTRL_FUNC_s *ctrl_set_ptr = &ckm_ctrl_func_array[ClkMonID];
        if ((ctrl_set_ptr->ID == ClkMonID) && (ctrl_set_ptr->FuncAct != NULL)) {
            ctrl_set_ptr->FuncAct(FLAG_ACT_GET_CFG, pNewRegsVal, &count);
        }
    }
}

/**
 *  AmbaCSL_ClkMonSetCtrl - Set PLL Control register values
 *  @param[in] ClkMonID CLKMON ID
 *  @param[in] pNewRegsVal A pointer to load PLL control register values
 */
void AmbaCSL_ClkMonSetCtrl(UINT32 ClkMonID, AMBA_CLK_MON_CTRL_REGS_VAL_s *pNewRegsVal)
{
    UINT32 max_count = (UINT32)(sizeof(ckm_ctrl_func_array) / sizeof(CKM_CTRL_FUNC_s));
    UINT32 count;

    if (ClkMonID < max_count) {
        const CKM_CTRL_FUNC_s *ctrl_set_ptr = &ckm_ctrl_func_array[ClkMonID];
        if ((ctrl_set_ptr->ID == ClkMonID) && (ctrl_set_ptr->FuncAct != NULL)) {
            ctrl_set_ptr->FuncAct(FLAG_ACT_SET_CFG, pNewRegsVal, &count);
        }
    }
}

/**
 *  AmbaCSL_ClkMonGetCount - Get PLL Control register values
 *  @param[in] ClkMonID CLKMON ID
 *  @return frequency
 */
UINT32 AmbaCSL_ClkMonGetCount(UINT32 ClkMonID)
{
    UINT32 max_count = (UINT32)(sizeof(ckm_ctrl_func_array) / sizeof(CKM_CTRL_FUNC_s));
    UINT32 count = 0;

    if (ClkMonID < max_count) {
        const CKM_CTRL_FUNC_s *ctrl_set_ptr = &ckm_ctrl_func_array[ClkMonID];
        AMBA_CLK_MON_CTRL_REGS_VAL_s ctrl_value;
        if ((ctrl_set_ptr->ID == ClkMonID) && (ctrl_set_ptr->FuncAct != NULL)) {
            ctrl_set_ptr->FuncAct(FLAG_ACT_GET_CNT, &ctrl_value, &count);
        }
    }

    return count;
}

