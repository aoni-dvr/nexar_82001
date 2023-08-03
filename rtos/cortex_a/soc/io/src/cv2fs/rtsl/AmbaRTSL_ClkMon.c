/**
 *  @file AmbaRTSL_ClkMon.c
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
 *  @details CLOCK MONITOR RTSL APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaCSL_PLL.h"

#include "AmbaRTSL_ClkMon.h"
#include "AmbaCSL_ClkMon.h"
#include "AmbaClkMon_Def.h"

/**
 *  AmbaRTSL_ClkMonGetCount - Get the monitor clock count
 *  @param[in] ClkMonID CLKMON ID
 *  @return Frequency
 */
UINT32 AmbaRTSL_ClkMonGetCount(UINT32 ClkMonID, UINT32 *pClkCountsVal)
{
    UINT32 RetVal = PLL_ERR_NONE;

    if (ClkMonID < AMBA_NUM_PLL) {
        *pClkCountsVal = AmbaCSL_ClkMonGetCount(ClkMonID);
    } else {
        RetVal = PLL_ERR_ARG;
    }

    return RetVal;
}

UINT32 AmbaRTSL_ClkMonSetCtrlManual(UINT32 ClkMonID, UINT8 Enable, UINT16 UpperBound, UINT16 LowerBound)
{
    UINT32 uret = 0;
    AMBA_CLK_MON_CTRL_REGS_VAL_s ctrl_value;

    if ((ClkMonID < AMBA_CKM_IDX_MAX) && (ClkMonID != 0U)) {
        ctrl_value.ClkMonEnable = Enable;
        ctrl_value.LowerBound = LowerBound;
        ctrl_value.UpperBound = UpperBound;
        AmbaCSL_ClkMonSetCtrl(ClkMonID, &ctrl_value);
    } else {
        uret = PLL_ERR_ARG;
    }

    return uret;
}

/**
 *  AmbaRTSL_ClkMonGetCtrl - Get the monitor clock information
 *  @param [IN] ClkMonID CLKMON ID
 *  @param [OUT] Enable:<br>
 *               - 0: Disabled
 *               - 1: Enabled
 *               - 0xFF: Information not available
 *  @param [OUT] UpperBound Upper bound value of the clock monitor
 *  @param [OUT] LowerBound Lower bound value of the clock monitor
 *  @retval 0 Success
 *  @retval PLL_ERR_ARG wrong clock monitor ID
 */
UINT32 AmbaRTSL_ClkMonGetCtrl(UINT32 ClkMonID, UINT8 *Enable, UINT16 *UpperBound, UINT16 *LowerBound)
{
    UINT32 uret = 0;
    AMBA_CLK_MON_CTRL_REGS_VAL_s ctrl_value = {0, 0, 0xFF};

    if ((ClkMonID < AMBA_CKM_IDX_MAX) && (ClkMonID != 0U)) {
        AmbaCSL_ClkMonGetCtrl(ClkMonID, &ctrl_value);
        *Enable = ctrl_value.ClkMonEnable;
        *LowerBound = ctrl_value.LowerBound;
        *UpperBound = ctrl_value.UpperBound;
    } else {
        uret = PLL_ERR_ARG;
    }

    return uret;
}

