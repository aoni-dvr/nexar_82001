/**
 *  @file AmbaRTSL_PsMon.c
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
 *  @details Power Supply Monitor RTSL APIs
 *
 */

#include "AmbaTypes.h"

#include "AmbaRTSL_PsMon.h"
#include "AmbaCSL_PsMon.h"
#include "AmbaPsMon_Def.h"

/**
 *  AmbaRTSL_PmSetCtrl - Calculate the speicifed PLL output frequency
 *  @param[in] PmIdx Power Supply Monitor ID
 *  @param[in] pPsMonConfig Power Supply Monitor Setting
 *  @return error code
 */
UINT32 AmbaRTSL_PmSetCtrl(UINT32 PmIdx, const AMBA_POWER_MON_CONFIG_s *pPsMonConfig)
{
    UINT32 RetVal = POWERMONITOR_ERR_NONE;
    AMBA_PM_CTRL_REGS_VAL_s pNewRegsVal;

    /* Gear Detector Control */
    pNewRegsVal.PmGearDetSel.LowGearDetUVLevel = 3U;
    pNewRegsVal.PmGearDetSel.LowGearDetOVLevel = 3U;
    pNewRegsVal.PmGearDetSel.HighGearDet1UVLevel = 3U;
    pNewRegsVal.PmGearDetSel.HighGearDet1OVLevel = 3U;
    pNewRegsVal.PmGearDetSel.HighGearDet2UVLevel = 3U;
    pNewRegsVal.PmGearDetSel.HighGearDet2OVLevel = 3U;
    pNewRegsVal.PmGearDetSel.FlagClearError = pPsMonConfig->FlagClearError;

    /* Gear Vsence Control */
    pNewRegsVal.PmVsenceSel.vsenselosel = pPsMonConfig->vsenselosel;
    pNewRegsVal.PmVsenceSel.vsensehi1sel = pPsMonConfig->vsensehi1sel;
    pNewRegsVal.PmVsenceSel.vsensehi2sel = pPsMonConfig->vsensehi2sel;

    /* Detector Power Down Control */
    pNewRegsVal.PmDetPowerDown.PdDecLo = pPsMonConfig->PdDecLo;
    pNewRegsVal.PmDetPowerDown.PdDechi1 = pPsMonConfig->PdDechi1;
    pNewRegsVal.PmDetPowerDown.PdDechi2 = pPsMonConfig->PdDechi2;
    pNewRegsVal.PmDetPowerDown.PdRef = 0U;

    /* ABIST control */
    pNewRegsVal.PmAbist.AbistEn = 0U;
    pNewRegsVal.PmAbist.AbistSel = 0U;
    pNewRegsVal.PmAbist.AbistMan = 0U;
    pNewRegsVal.PmAbist.AbistRstn = 0U;
    pNewRegsVal.PmAbist.AbistWindow = 1U;
    pNewRegsVal.PmAbist.AbistToggle = 0U;
    pNewRegsVal.PmAbist.PmDisable = 0U;
    pNewRegsVal.PmAbist.SfWait = 3U;
    pNewRegsVal.PmAbist.PdPsmon = 0U;
    pNewRegsVal.PmAbist.SfEnable = 1U;
    pNewRegsVal.PmAbist.AbistBrek = 1U;
    pNewRegsVal.PmAbist.UvovSwap = 0U;

    /* Set Power monitor config*/
    if (PmIdx < AMBA_PSM_MON_NUM) {
        AmbaCSL_PmSetCtrlRegsVal(PmIdx, &pNewRegsVal);
    } else {
        RetVal = POWERMONITOR_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PmGetErrorStatus - Get the Power Monitor Errror Status
 *  @param[in] PmIdx Power Supply Monitor ID
 *  @param[out] pPmErrorStatus Error Status
 *  @return error code
 */
UINT32 AmbaRTSL_PmGetErrorStatus(UINT32 PmIdx, UINT32 *pPmErrorStatus)
{
    UINT32 RetVal = POWERMONITOR_ERR_NONE;

    if (PmIdx < AMBA_PSM_MON_NUM) {
        *pPmErrorStatus = (UINT32)AmbaCSL_PmGetErrorStatus(PmIdx);
    } else {
        RetVal = POWERMONITOR_ERR_ARG;
    }

    return RetVal;
}
