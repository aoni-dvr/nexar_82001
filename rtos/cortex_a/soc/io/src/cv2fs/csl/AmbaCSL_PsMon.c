/**
 *  @file AmbaCSL_PsMon.c
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
 *  @details Chip Support Library (CSL) for Power Supply Monitor
 *
 */

#include "AmbaTypes.h"
#include <AmbaMisraFix.h>

#include "AmbaCSL_PsMon.h"
#include "AmbaPsMon_Def.h"

static volatile AMBA_PS_MON_CTRL1_REG_s *reg_ctrl1_array[AMBA_PSM_MON_NUM];
static volatile AMBA_PS_MON_CTRL2_REG_s *reg_ctrl2_array[AMBA_PSM_MON_NUM];
static volatile AMBA_PS_MON_OBSV_REG_s  *reg_obsv_array[AMBA_PSM_MON_NUM];

static void init_once(void)
{
    static UINT8 flag_init = 0;
    if (flag_init == 0U) {
        reg_ctrl1_array[0] = &pAmbaRCT_Reg->PsMon0Ctrl1;
        reg_ctrl1_array[1] = &pAmbaRCT_Reg->PsMon1Ctrl1;
        reg_ctrl1_array[2] = &pAmbaRCT_Reg->PsMon2Ctrl1;
        reg_ctrl1_array[3] = &pAmbaRCT_Reg->PsMon3Ctrl1;
        reg_ctrl1_array[4] = &pAmbaRCT_Reg->PsMon4Ctrl1;
        reg_ctrl1_array[5] = &pAmbaRCT_Reg->PsMon5Ctrl1;
        reg_ctrl1_array[6] = &pAmbaRCT_Reg->PsMon6Ctrl1;
        reg_ctrl1_array[7] = &pAmbaRCT_Reg->PsMon7Ctrl1;
        reg_ctrl1_array[8] = &pAmbaRCT_Reg->PsMon8Ctrl1;
        reg_ctrl1_array[9] = &pAmbaRCT_Reg->PsMon9Ctrl1;
        reg_ctrl1_array[10] = &pAmbaRCT_Reg->PsMon10Ctrl1;
        reg_ctrl1_array[11] = &pAmbaRCT_Reg->PsMon11Ctrl1;
        reg_ctrl1_array[12] = &pAmbaRCT_Reg->PsMon12Ctrl1;
        reg_ctrl1_array[13] = &pAmbaRCT_Reg->PsMon13Ctrl1;
        reg_ctrl1_array[14] = &pAmbaRCT_Reg->PsMon14Ctrl1;
        reg_ctrl1_array[15] = &pAmbaRCT_Reg->PsMon15Ctrl1;
        reg_ctrl1_array[16] = &pAmbaRCT_Reg->PsMonLv0Ctrl1;
        reg_ctrl1_array[17] = &pAmbaRCT_Reg->PsMonLv1Ctrl1;

        reg_ctrl2_array[0] = &pAmbaRCT_Reg->PsMon0Ctrl2;
        reg_ctrl2_array[1] = &pAmbaRCT_Reg->PsMon1Ctrl2;
        reg_ctrl2_array[2] = &pAmbaRCT_Reg->PsMon2Ctrl2;
        reg_ctrl2_array[3] = &pAmbaRCT_Reg->PsMon3Ctrl2;
        reg_ctrl2_array[4] = &pAmbaRCT_Reg->PsMon4Ctrl2;
        reg_ctrl2_array[5] = &pAmbaRCT_Reg->PsMon5Ctrl2;
        reg_ctrl2_array[6] = &pAmbaRCT_Reg->PsMon6Ctrl2;
        reg_ctrl2_array[7] = &pAmbaRCT_Reg->PsMon7Ctrl2;
        reg_ctrl2_array[8] = &pAmbaRCT_Reg->PsMon8Ctrl2;
        reg_ctrl2_array[9] = &pAmbaRCT_Reg->PsMon9Ctrl2;
        reg_ctrl2_array[10] = &pAmbaRCT_Reg->PsMon10Ctrl2;
        reg_ctrl2_array[11] = &pAmbaRCT_Reg->PsMon11Ctrl2;
        reg_ctrl2_array[12] = &pAmbaRCT_Reg->PsMon12Ctrl2;
        reg_ctrl2_array[13] = &pAmbaRCT_Reg->PsMon13Ctrl2;
        reg_ctrl2_array[14] = &pAmbaRCT_Reg->PsMon14Ctrl2;
        reg_ctrl2_array[15] = &pAmbaRCT_Reg->PsMon15Ctrl2;
        reg_ctrl2_array[16] = &pAmbaRCT_Reg->PsMonLv0Ctrl2;
        reg_ctrl2_array[17] = &pAmbaRCT_Reg->PsMonLv1Ctrl2;

        reg_obsv_array[0] = &pAmbaRCT_Reg->PsMon0Obsv;
        reg_obsv_array[1] = &pAmbaRCT_Reg->PsMon1Obsv;
        reg_obsv_array[2] = &pAmbaRCT_Reg->PsMon2Obsv;
        reg_obsv_array[3] = &pAmbaRCT_Reg->PsMon3Obsv;
        reg_obsv_array[4] = &pAmbaRCT_Reg->PsMon4Obsv;
        reg_obsv_array[5] = &pAmbaRCT_Reg->PsMon5Obsv;
        reg_obsv_array[6] = &pAmbaRCT_Reg->PsMon6Obsv;
        reg_obsv_array[7] = &pAmbaRCT_Reg->PsMon7Obsv;
        reg_obsv_array[8] = &pAmbaRCT_Reg->PsMon8Obsv;
        reg_obsv_array[9] = &pAmbaRCT_Reg->PsMon9Obsv;
        reg_obsv_array[10] = &pAmbaRCT_Reg->PsMon10Obsv;
        reg_obsv_array[11] = &pAmbaRCT_Reg->PsMon11Obsv;
        reg_obsv_array[12] = &pAmbaRCT_Reg->PsMon12Obsv;
        reg_obsv_array[13] = &pAmbaRCT_Reg->PsMon13Obsv;
        reg_obsv_array[14] = &pAmbaRCT_Reg->PsMon14Obsv;
        reg_obsv_array[15] = &pAmbaRCT_Reg->PsMon15Obsv;
        reg_obsv_array[16] = &pAmbaRCT_Reg->PsMonLv0Obsv;
        reg_obsv_array[17] = &pAmbaRCT_Reg->PsMonLv1Obsv;

        flag_init = 1;
    }
}

/**
 *  PM_GetPowerMonCtrl1RegAddr - Get power monitor control 1 register address
 *  @param[in] PmIdx PLL ID
 *  @return NULL or a pointer to a register address
 */
static volatile AMBA_PS_MON_CTRL1_REG_s * PM_GetPowerMonCtrl1RegAddr(UINT32 PmIdx)
{
    volatile AMBA_PS_MON_CTRL1_REG_s *pPmCtrlReg = NULL;

    init_once();

    if (PmIdx < AMBA_PSM_MON_NUM) {
        pPmCtrlReg = reg_ctrl1_array[PmIdx];
    }

    return pPmCtrlReg;
}

/**
 *  PM_GetPowerMonCtrl2RegAddr - Get power monitor control 2 register address
 *  @param[in] PmIdx PM ID
 *  @return NULL or a pointer to a register address
 */
static volatile AMBA_PS_MON_CTRL2_REG_s * PM_GetPowerMonCtrl2RegAddr(UINT32 PmIdx)
{
    volatile AMBA_PS_MON_CTRL2_REG_s *pPmCtrlReg = NULL;

    init_once();
    if (PmIdx < AMBA_PSM_MON_NUM) {
        pPmCtrlReg = reg_ctrl2_array[PmIdx];
    }
    return pPmCtrlReg;
}

/**
 *  PM_GetPowerMonObsvRegAddr - Get power monitor Observation register address
 *  @param[in] PmIdx PLL ID
 *  @return NULL or a pointer to a register address
 */
static volatile AMBA_PS_MON_OBSV_REG_s * PM_GetPowerMonObsvRegAddr(UINT32 PmIdx)
{
    volatile AMBA_PS_MON_OBSV_REG_s *pPmObsvReg = NULL;

    init_once();

    if (PmIdx < AMBA_PSM_MON_NUM) {
        pPmObsvReg = reg_obsv_array[PmIdx];
    }

    return pPmObsvReg;
}

/**
 *  AmbaCSL_PmSetCtrlRegsVal - Set Power monitor Control register values
 *  @param[in] PmIdx PLL ID
 *  @param[in] pNewRegsVal A pointer to load PLL control register values
 */
void AmbaCSL_PmSetCtrlRegsVal(UINT32 PmIdx, const AMBA_PM_CTRL_REGS_VAL_s *pNewRegsVal)
{
    volatile AMBA_PS_MON_CTRL1_REG_s *pPllCtrl1Reg = PM_GetPowerMonCtrl1RegAddr(PmIdx);
    volatile AMBA_PS_MON_CTRL2_REG_s *pPllCtrl2Reg = PM_GetPowerMonCtrl2RegAddr(PmIdx);

    if (pPllCtrl1Reg != NULL) {
        /* Gear Detector */
        pPllCtrl1Reg->LowGearDetUVLevSel = pNewRegsVal->PmGearDetSel.LowGearDetUVLevel;
        pPllCtrl1Reg->LowGearDetOVLevSel = pNewRegsVal->PmGearDetSel.LowGearDetOVLevel;
        pPllCtrl1Reg->HighGearDet1UVLevSel = pNewRegsVal->PmGearDetSel.HighGearDet1UVLevel;
        pPllCtrl1Reg->HighGearDet1OVLevSel = pNewRegsVal->PmGearDetSel.HighGearDet1OVLevel;
        pPllCtrl1Reg->HighGearDet2UVLevSel = pNewRegsVal->PmGearDetSel.HighGearDet2UVLevel;
        pPllCtrl1Reg->HighGearDet2OVLevSel = pNewRegsVal->PmGearDetSel.HighGearDet2OVLevel;
        pPllCtrl1Reg->ClearErrorb = pNewRegsVal->PmGearDetSel.FlagClearError;
    }

    if (pPllCtrl2Reg != NULL) {
        /* Gear Vsence */
        pPllCtrl2Reg->vsenselosel = pNewRegsVal->PmVsenceSel.vsenselosel;
        pPllCtrl2Reg->vsensehi1sel = pNewRegsVal->PmVsenceSel.vsensehi1sel;
        pPllCtrl2Reg->vsensehi2sel = pNewRegsVal->PmVsenceSel.vsensehi2sel;

        /* Power Down */
        pPllCtrl2Reg->pd_declo = pNewRegsVal->PmDetPowerDown.PdDecLo;
        pPllCtrl2Reg->pd_dechi1 = pNewRegsVal->PmDetPowerDown.PdDechi1;
        pPllCtrl2Reg->pd_dechi2 = pNewRegsVal->PmDetPowerDown.PdDechi2;
        pPllCtrl2Reg->pd_ref = pNewRegsVal->PmDetPowerDown.PdRef;

        /* ABIST */
        pPllCtrl2Reg->abist_en = pNewRegsVal->PmAbist.AbistEn;
        pPllCtrl2Reg->abist_sel = pNewRegsVal->PmAbist.AbistSel;
        pPllCtrl2Reg->abist_man = pNewRegsVal->PmAbist.AbistMan;
        pPllCtrl2Reg->abist_rstn = pNewRegsVal->PmAbist.AbistRstn;
        pPllCtrl2Reg->abist_window = pNewRegsVal->PmAbist.AbistWindow;
        pPllCtrl2Reg->abist_toggle = pNewRegsVal->PmAbist.AbistToggle;
        pPllCtrl2Reg->pm_disable = pNewRegsVal->PmAbist.PmDisable;
        pPllCtrl2Reg->sf_rstn = 0U;
        pPllCtrl2Reg->pm_rstn = 0U;
        pPllCtrl2Reg->sf_wait = pNewRegsVal->PmAbist.SfWait;
        pPllCtrl2Reg->pd_psmon = pNewRegsVal->PmAbist.PdPsmon;
        pPllCtrl2Reg->sf_enable = pNewRegsVal->PmAbist.SfEnable;
        pPllCtrl2Reg->abist_brek = pNewRegsVal->PmAbist.AbistBrek;
        pPllCtrl2Reg->uvov_swap = pNewRegsVal->PmAbist.UvovSwap;
        pPllCtrl2Reg->sf_rstn = 1U;
        pPllCtrl2Reg->pm_rstn = 1U;
    }
}

/**
 *  AmbaCSL_PmGetErrorStatus - Get Power monitor Status
 *  @param[in] PmIdx PLL ID
 */
UINT32 AmbaCSL_PmGetErrorStatus(UINT32 PmIdx)
{
    const volatile AMBA_PS_MON_OBSV_REG_s *pPmObsvReg = PM_GetPowerMonObsvRegAddr(PmIdx);
    UINT32 Val;
    const UINT32 *uint32_ptr;

    AmbaMisra_TypeCast(&uint32_ptr, &pPmObsvReg);
    Val = *uint32_ptr;
    return Val;
}

