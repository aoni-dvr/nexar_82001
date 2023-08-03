/**
 *  @file AmbaSD_DelayCtrl.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details SD Card Control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"

#include "AmbaRTSL_GPIO.h"

#include "AmbaSD.h"
#include "AmbaSD_Ctrl.h"

#include "AmbaRTSL_SD.h"

/**
 *  AmbaSD_SetDriveStrength - Set SD driving strength
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pSdConfig pointer to the SD controller configurations
 *  @return error code
 */
UINT32 AmbaSD_SetDriveStrength(UINT32 SdChanNo, const AMBA_SD_SETTING_s * pSdSetting)
{
    UINT32 RetVal = 0;
    UINT32 DataDriveStrength;

    switch (SdChanNo) {
    case AMBA_SD_CHANNEL0:      /* for SD0 */
        /* Drive Strength for Clock */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_061_SD0_CLK, pSdSetting->ClockDrive);

        /* Drive Strength for Data */
        DataDriveStrength = pSdSetting->DataDrive;
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_069_SD0_DATA0, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_070_SD0_DATA1, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_071_SD0_DATA2, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_072_SD0_DATA3, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_073_SD0_DATA4, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_074_SD0_DATA5, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_075_SD0_DATA6, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_076_SD0_DATA7, DataDriveStrength);

        /* Drive Strength for CMD */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_062_SD0_CMD, pSdSetting->CMDDrive);

        /* Drive Strength for CD */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_063_SD0_CD,  pSdSetting->CDDrive);

        /* Drive Strength for WP */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_064_SD0_WP,  pSdSetting->WPDrive);
        break;

    case AMBA_SD_CHANNEL1:      /* for SD1 */
        /* Drive Strength for Clock */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_077_SD1_CLK, pSdSetting->ClockDrive);

        /* Drive Strength for Data */
        DataDriveStrength = pSdSetting->DataDrive;
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_079_SD1_DATA0, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_080_SD1_DATA1, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_081_SD1_DATA2, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_082_SD1_DATA3, DataDriveStrength);

        /* Drive Strength for CMD */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_078_SD1_CMD, pSdSetting->CMDDrive);

        /* Drive Strength for CD */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_083_SD1_CD,  pSdSetting->CDDrive);

        /* Drive Strength for WP */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_084_SD1_WP,  pSdSetting->WPDrive);
        break;

    default:
        RetVal = AMBA_SD_ERR_INVALID_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaSD_DelayCtrlSet -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] ClkBypass
 *  @param[in] RxClkPol
 *  @param[in] SbcCore
 *  @param[in] SelValue
 *  @param[in] DinClkPol
 *  @param[in] CmdBypass
 */
void AmbaSD_DelayCtrlSet(UINT32 SdChanNo, UINT32 ClkBypass, UINT32 RxClkPol, UINT32 SbcCore, UINT32 SelValue, UINT32 DinClkPol, UINT32 CmdBypass)
{
    AmbaMisra_TouchUnused(&DinClkPol);
    AmbaSD_DelayCtrlReset(SdChanNo);
    AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_RX_CLK_POL, RxClkPol);
    AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_CLK_OUT_BYPASS, ClkBypass);
    AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_DATA_CMD_BYPASS, CmdBypass);
    AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_DLL_BYPASS, 0x0);
    AmbaRTSL_SDPhySetSharedBusCtrl(SdChanNo, AMBA_SD_PHY_SBC_COARSE_DELAY, ((UINT8) SbcCore));
    AmbaRTSL_SDPhySetSharedBusCtrl(SdChanNo, AMBA_SD_PHY_SBC_ENABLEDLL, 0x1U);
    AmbaRTSL_SDPhySetSelect(SdChanNo, AMBA_SD_PHY_SEL0, ((UINT8) SelValue));
    AmbaRTSL_SDPhySetSelect(SdChanNo, AMBA_SD_PHY_SEL1, ((UINT8) SelValue));
    AmbaRTSL_SDPhySetSelect(SdChanNo, AMBA_SD_PHY_SEL2, ((UINT8) SelValue));
    AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_RESET, 0x1U);
    (void) AmbaKAL_TaskSleep(1U); //DLL reset time
    AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_RESET, 0x0U);
    (void) AmbaKAL_TaskSleep(1U); //DLL lock time
    AmbaRTSL_SDSetRdLatencyCtrl(SdChanNo, 0x1U);
}


/**
 *  AmbaSD_DelayCtrlGet -
 *  @param[in] SdChanNo SD Channel Number
 *  @return delay value
 */
UINT32 AmbaSD_DelayCtrlGet(UINT32 SdChanNo)
{
    AMBA_SD_DETAIL_DELAY_s DetailDelay = {0};
    UINT32 DetailDelayVal;

    DetailDelay.RdLatency = (UINT8) (AmbaRTSL_SDGetRdLatencyCtrl(SdChanNo) & 0x3U);
    DetailDelay.RXClkPol = AmbaRTSL_SDPhyGetConfig(SdChanNo, AMBA_SD_PHY_RX_CLK_POL);
    DetailDelay.ClkOutBypass = AmbaRTSL_SDPhyGetConfig(SdChanNo, AMBA_SD_PHY_CLK_OUT_BYPASS);
    DetailDelay.DataCmdBypass = AmbaRTSL_SDPhyGetConfig(SdChanNo, AMBA_SD_PHY_DATA_CMD_BYPASS);
    DetailDelay.SelValue = AmbaRTSL_SDPhyGetSelect(SdChanNo, AMBA_SD_PHY_SEL0);
    DetailDelay.SbcCoreDelay = (UINT8) AmbaRTSL_SDPhyGetSharedBusCtrl(SdChanNo, AMBA_SD_PHY_SBC_COARSE_DELAY);
    DetailDelay.DinClkPol = AmbaRTSL_SDPhyGetConfig(SdChanNo, AMBA_SD_PHY_DIN_CLK_POL);

    AmbaMisra_TypeCast32(&DetailDelayVal, &DetailDelay);

    return DetailDelayVal;
}

/**
 *  AmbaSD_DelayCtrlReset -
 *  @param[in] SdChanNo SD Channel Number
 */
void AmbaSD_DelayCtrlReset(UINT32 SdChanNo)
{
    AmbaRTSL_SDPhyReset(SdChanNo);
    AmbaRTSL_SDPhySelReset(SdChanNo);
    AmbaRTSL_SDSetRdLatencyCtrl(SdChanNo, 0x0);
}

