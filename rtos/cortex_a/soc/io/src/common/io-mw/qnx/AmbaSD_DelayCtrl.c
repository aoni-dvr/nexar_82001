/*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"

#include "AmbaRTSL_GPIO.h"
#include "AmbaGPIO.h"

#include "AmbaSD.h"
#include "AmbaSD_Ctrl.h"

#include "AmbaRTSL_SD.h"
#include "AmbaCSL_SD.h"
#include "AmbaCortexA53.h"
#include "AmbaGPIO.h"

AMBA_SD_SETTING_s SdSetting = {0};

void AmbaSD_SetDetailDelay(UINT32 Value)
{
    SdSetting.DetailDelay = Value;
}

UINT32 AmbaSD_GetDetailDelay(void)
{
    return SdSetting.DetailDelay;
}

void AmbaSD_SetHsRdLatency(UINT32 SdChanNo, UINT8 HsRdLatency)
{
    (void)SdChanNo;
    SdSetting.HsRdLatency = HsRdLatency;
}

UINT8 AmbaSD_GetHsRdLatency(UINT32 SdChanNo)
{
    (void)SdChanNo;
    return SdSetting.HsRdLatency;
}

/**
 *  AmbaSD_SetDriveStrength - Set SD driving strength
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pSdConfig pointer to the SD controller configurations
 *  @return error code
 */
#if defined (CONFIG_SOC_CV2FS)|| defined (CONFIG_SOC_CV22FS)
UINT32 AmbaSD_SetDriveStrength(UINT32 SdChanNo, const AMBA_SD_SETTING_s * pSdSetting)
{
    UINT32 RetVal = 0;
    UINT32 DataDriveStrength;

    switch (SdChanNo) {
    case AMBA_SD_CHANNEL0:      /* for SD0 */
        /* Drive Strength for Clock */
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_061_SD0_CLK, pSdSetting->ClockDrive);

        /* Drive Strength for Data */
        DataDriveStrength = pSdSetting->DataDrive;
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_069_SD0_DATA0, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_070_SD0_DATA1, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_071_SD0_DATA2, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_072_SD0_DATA3, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_073_SD0_DATA4, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_074_SD0_DATA5, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_075_SD0_DATA6, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_076_SD0_DATA7, DataDriveStrength);

        /* Drive Strength for CMD */
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_062_SD0_CMD, pSdSetting->CMDDrive);

        /* Drive Strength for CD */
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_063_SD0_CD,  pSdSetting->CDDrive);

        /* Drive Strength for WP */
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_064_SD0_WP,  pSdSetting->WPDrive);
        break;

    case AMBA_SD_CHANNEL1:      /* for SD1 */
        /* Drive Strength for Clock */
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_077_SD1_CLK, pSdSetting->ClockDrive);

        /* Drive Strength for Data */
        DataDriveStrength = pSdSetting->DataDrive;
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_079_SD1_DATA0, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_080_SD1_DATA1, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_081_SD1_DATA2, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_082_SD1_DATA3, DataDriveStrength);

        /* Drive Strength for CMD */
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_078_SD1_CMD, pSdSetting->CMDDrive);

        /* Drive Strength for CD */
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_083_SD1_CD,  pSdSetting->CDDrive);

        /* Drive Strength for WP */
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_084_SD1_WP,  pSdSetting->WPDrive);
        break;

    default:
        RetVal = AMBA_SD_ERR_INVALID_ARG;
        break;
    }

    return RetVal;
}
#else
UINT32 AmbaSD_SetDriveStrength(UINT32 SdChanNo, const AMBA_SD_SETTING_s * pSdSetting)
{
    UINT32 RetVal = 0;
    UINT32 DataDriveStrength;

    switch (SdChanNo) {
    case AMBA_SD_CHANNEL0:      /* for SD0 */
        /* Drive Strength for Clock */
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_73_SD0_CLK, pSdSetting->ClockDrive);

        /* Drive Strength for Data */
        DataDriveStrength = pSdSetting->DataDrive;
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_89_SD0_DATA0, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_90_SD0_DATA1, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_91_SD0_DATA2, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_92_SD0_DATA3, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_93_SD0_DATA4, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_94_SD0_DATA5, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_95_SD0_DATA6, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_96_SD0_DATA7, DataDriveStrength);

        /* Drive Strength for CMD */
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_74_SD0_CMD, pSdSetting->CMDDrive);

        /* Drive Strength for CD */
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_75_SD0_CD,  pSdSetting->CDDrive);

        /* Drive Strength for WP */
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_76_SD0_WP,  pSdSetting->WPDrive);
        break;

    case AMBA_SD_CHANNEL1:      /* for SD1 */
        /* Drive Strength for Clock */
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_97_SD1_CLK, pSdSetting->ClockDrive);

        /* Drive Strength for Data */
        DataDriveStrength = pSdSetting->DataDrive;
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_99_SD1_DATA0, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_100_SD1_DATA1, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_101_SD1_DATA2, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_102_SD1_DATA3, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_105_SD1_DATA4, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_106_SD1_DATA5, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_107_SD1_DATA6, DataDriveStrength);
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_108_SD1_DATA7, DataDriveStrength);

        /* Drive Strength for CMD */
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_98_SD1_CMD, pSdSetting->CMDDrive);

        /* Drive Strength for CD */
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_103_SD1_CD,  pSdSetting->CDDrive);

        /* Drive Strength for WP */
        (void) AmbaGPIO_SetDriveStrength(GPIO_PIN_104_SD1_WP,  pSdSetting->WPDrive);
        break;

    default:
        RetVal = AMBA_SD_ERR_INVALID_ARG;
        break;
    }

    return RetVal;
}
#endif

void AmbaSD_DelayCtrlSetValue(UINT32 SdChanNo, UINT32 Value)
{
    AMBA_SD_DETAIL_DELAY_s DetailDelay = {0};

    if (Value != 0U) {
        AmbaMisra_TypeCast32(&DetailDelay, &Value);
        AmbaSD_DelayCtrlSet(SdChanNo, DetailDelay.ClkOutBypass, DetailDelay.RXClkPol, DetailDelay.SbcCoreDelay, DetailDelay.SelValue,
                            DetailDelay.DinClkPol, DetailDelay.DataCmdBypass);
    }
}

void AmbaSD_SetRdLatencyCtrl(UINT32 SdChanNo, UINT32 Value)
{
    AmbaRTSL_SDSetRdLatencyCtrl(SdChanNo, Value);
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

#define AMBA_RCT_BASE            0xED080000U
#define AMBA_RCT_SIZE            0x1000

#if defined (CONFIG_SOC_CV2FS)|| defined (CONFIG_SOC_CV22FS)
#define AMBA_CORTEX_A53_SD0_BASE_ADDR   AMBA_CA53_SD_BASE_ADDR
#define AMBA_CORTEX_A53_SD1_BASE_ADDR   AMBA_CA53_SDIO0_BASE_ADDR
#endif

INT32 AmbaSD_GetChannel(UINT32 RegBase, UINT32 *pSdChanNo);

INT32 AmbaSD_DelayCtrlInit(void)
{
    int Rval = 0;
    void* virtual_addr;

    if ((virtual_addr = mmap_device_memory( NULL, AMBA_RCT_SIZE,
                                            PROT_READ | PROT_WRITE | PROT_NOCACHE, 0,
                                            AMBA_RCT_BASE)) == MAP_FAILED) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot access to RCT register region",
              __FUNCTION__);
        return -1;
    } else {
        pAmbaRCT_Reg =  (AMBA_RCT_REG_s *) virtual_addr;
    }

    if ((virtual_addr = mmap_device_memory( NULL, 0x100,
                                            PROT_READ | PROT_WRITE | PROT_NOCACHE, 0,
                                            AMBA_CORTEX_A53_SD0_BASE_ADDR)) == MAP_FAILED) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot access to SD0 register region",
              __FUNCTION__);
        return -1;
    } else {
        pAmbaSD_Reg[0] =  (AMBA_SD_REG_s *) virtual_addr;
    }

    if ((virtual_addr = mmap_device_memory( NULL, 0x100,
                                            PROT_READ | PROT_WRITE | PROT_NOCACHE, 0,
                                            AMBA_CORTEX_A53_SD1_BASE_ADDR)) == MAP_FAILED) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot access to SD1 register region",
              __FUNCTION__);
        return -1;
    } else {
        pAmbaSD_Reg[1] =  (AMBA_SD_REG_s *) virtual_addr;
    }

    return Rval;
}

