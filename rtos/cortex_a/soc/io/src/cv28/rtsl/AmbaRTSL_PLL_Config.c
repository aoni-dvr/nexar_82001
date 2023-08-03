/**
 *  @file AmbaRTSL_PLL_Config.c
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
 *  @details PLL RTSL APIs
 *
 */
#include "AmbaTypes.h"

#include "AmbaRTSL_PLL.h"
#include "AmbaCSL_PLL.h"
#include "AmbaCSL_RCT.h"

/**
 *  AmbaRTSL_PllSetAdcClkConfig - Set Adc clock configurations
 *  @param[in] AdcClkConfig ADC clock source selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetAdcClkConfig(UINT32 AdcClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    switch (AdcClkConfig) {
    case AMBA_PLL_ADC_CLK_REF:
        AmbaCSL_PllSetAdcRefClkSelect(0U);
        break;

    case AMBA_PLL_ADC_CLK_IDSP:
        AmbaCSL_PllSetAdcRefClkSelect(1U);
        break;

    case AMBA_PLL_ADC_CLK_NAND:
        AmbaCSL_PllSetAdcRefClkSelect(2U);
        break;

    default:
        RetVal = PLL_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetAudioClkConfig - Set Audio clock configuration
 *  @param[in] AudioClkConfig Audio clock source selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetAudioClkConfig(UINT32 AudioClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    if (AudioClkConfig >= AMBA_NUM_PLL_AUDIO_CLK_CONFIG) {
        RetVal = PLL_ERR_ARG;
    } else if (AudioClkConfig == AMBA_PLL_AUDIO_CLK_EXT) {
        AmbaCSL_PllSetAudioExtClkCtrl(1U);  /* external clock */
    } else {
        AmbaCSL_PllSetAudioExtClkCtrl(0U);  /* internal PLL generated */

        if (AudioClkConfig == AMBA_PLL_AUDIO_CLK_REF) {
            AmbaCSL_PllSetAudioRefClkCtrl0(0U);  /* clk_ref */
        } else {
            AmbaCSL_PllSetAudioRefClkCtrl0(1U);

            if (AudioClkConfig == AMBA_PLL_AUDIO_CLK_SENSOR) {
                AmbaCSL_PllSetAudioRefClkCtrl1(1U);  /* xx_clk_si */
            } else {
                AmbaCSL_PllSetAudioRefClkCtrl1(2U);  /* MUXED_LVDS_SPCLK */
            }
        }
        (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_AUDIO);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetGtxClkConfig - Set the GTX Clock Config
 *  @param[in] ClkGtxClkConfig GTX clock source selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetGtxClkConfig(UINT32 ClkGtxClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    switch (ClkGtxClkConfig) {
    case AMBA_PLL_GTX_CLK_EXT:
        pAmbaRCT_Reg->GtxClkSelect.UseInternalClk = 0U;
        break;

    case AMBA_PLL_GTX_CLK_INT:
        pAmbaRCT_Reg->GtxClkSelect.UseInternalClk = 1U;
        break;

    default:
        RetVal = PLL_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetCanClkConfig - Set CAN clock configurations
 *  @param[in] CanClkConfig CAN clock source selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetCanClkConfig(UINT32 CanClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    switch (CanClkConfig) {
    case AMBA_PLL_CAN_CLK_REF:
        AmbaCSL_PllSetCanClkSrcSel(0U);
        break;

    case AMBA_PLL_CAN_CLK_ETHERNET:
        AmbaCSL_PllSetCanClkSrcSel(1U);
        break;

    case AMBA_PLL_CAN_CLK_AHB:
        AmbaCSL_PllSetCanClkSrcSel(2U);
        break;

    default:
        RetVal = PLL_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetPwmClkConfig - Set PWM clock configurations
 *  @param[in] PwmClkConfig PWM clock source selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetPwmClkConfig(UINT32 PwmClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    switch (PwmClkConfig) {
    case AMBA_PLL_PWM_CLK_APB:
        AmbaCSL_PllSetPwmRefClkSel(0U);
        break;

    case AMBA_PLL_PWM_CLK_CORTEX:
        AmbaCSL_PllSetPwmRefClkSel(1U);
        break;

    case AMBA_PLL_PWM_CLK_ENET:
        AmbaCSL_PllSetPwmRefClkSel(2U);
        break;

    case AMBA_PLL_PWM_CLK_REF:
        AmbaCSL_PllSetPwmRefClkSel(3U);
        break;

    default:
        RetVal = PLL_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetSd0ScalerClkSrc - Set SD0/SD post scaler clock source selection
 *  @param[in] SdClkConfig SD clock souce selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetSd0ScalerClkSrc(UINT32 SdClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    switch (SdClkConfig) {
    case AMBA_PLL_SD_CLK_SD:
        AmbaCSL_PLLSetSd0ScalerClkSrc(0U);
        break;

    case AMBA_PLL_SD_CLK_CORE:
        AmbaCSL_PLLSetSd0ScalerClkSrc(1U);
        break;

    default:
        RetVal = PLL_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetSd1ScalerClkSrc - Set SD1(SDIO0) post scaler clock source selection
 *  @param[in] SdClkConfig SD clock souce selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetSd1ScalerClkSrc(UINT32 SdClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    switch (SdClkConfig) {
    case AMBA_PLL_SD_CLK_SD:
        AmbaCSL_PLLSetSd1ScalerClkSrc(0U);
        break;

    case AMBA_PLL_SD_CLK_CORE:
        AmbaCSL_PLLSetSd1ScalerClkSrc(1U);
        break;

    default:
        RetVal = PLL_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetSd2ScalerClkSrc - Set SD2(SDIO1) post scaler clock source selection
 *  @param[in] SdClkConfig SD clock souce selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetSd2ScalerClkSrc(UINT32 SdClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    switch (SdClkConfig) {
    case AMBA_PLL_SD_CLK_SD:
        AmbaCSL_PLLSetSd2ScalerClkSrc(0U);
        break;

    case AMBA_PLL_SD_CLK_CORE:
        AmbaCSL_PLLSetSd2ScalerClkSrc(1U);
        break;

    default:
        RetVal = PLL_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetSensor0ClkDir - Set the direction of Sensor 0 Clock (CLK_SI)
 *  @param[in] ClkSiDir I/O direction of Sensor 0 Clock (CLK_SI)
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetSensor0ClkDir(UINT32 ClkSiDir)
{
    UINT32 RetVal = PLL_ERR_NONE;

    switch (ClkSiDir) {
    case AMBA_PLL_SENSOR_REF_CLK_OUTPUT:
        pAmbaRCT_Reg->SensorClk0PadCtrl.PadMode = 0U;
        break;

    case AMBA_PLL_SENSOR_REF_CLK_INPUT:
        pAmbaRCT_Reg->SensorClk0PadCtrl.PadMode = 1U;
        break;

    default:
        RetVal = PLL_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetSpiClkConfig - Set SPI clock configurations
 *  @param[in] SpiClkConfig SPI clock source selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetSpiClkConfig(UINT32 SpiClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    if (SpiClkConfig >= AMBA_NUM_PLL_SPI_CLK_CONFIG) {
        RetVal = PLL_ERR_ARG;
    } else {
        AmbaCSL_PllSetSpiMasterRefClkSelect(1U); /* gclk_core */
        AmbaCSL_PllSetSpiSlaveRefClkSelect(1U);  /* gclk_core */

        if (SpiClkConfig == AMBA_PLL_SPI_CLK_ETHERNET) {
            AmbaCSL_PllSetSpiNorRefClkSelect(0U);
        } else if (SpiClkConfig == AMBA_PLL_SPI_CLK_CORE) {
            AmbaCSL_PllSetSpiNorRefClkSelect(1U);
        } else if (SpiClkConfig == AMBA_PLL_SPI_CLK_SPI) {
            AmbaCSL_PllSetSpiNorRefClkSelect(2U);
        } else if (SpiClkConfig == AMBA_PLL_SPI_CLK_REF) {
            AmbaCSL_PllSetSpiNorRefClkSelect(3U);
        } else {
            /* For misra checking, do nothing */
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetUartApbClkConfig - Set UART APB clock configurations
 *  @param[in] UartClkConfig UART clock source selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetUartApbClkConfig(UINT32 UartClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    switch (UartClkConfig) {
    case AMBA_PLL_UART_CLK_REF:
        AmbaCSL_PllSetUartApbClkSrcSel(0U);
        break;
    case AMBA_PLL_UART_CLK_CORE:
        AmbaCSL_PllSetUartApbClkSrcSel(1U);
        break;
    case AMBA_PLL_UART_CLK_ENET:
        AmbaCSL_PllSetUartApbClkSrcSel(2U);
        break;
    case AMBA_PLL_UART_CLK_SD:
        AmbaCSL_PllSetUartApbClkSrcSel(3U);
        break;
    default:
        RetVal = PLL_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetUart0ClkConfig - Set UART AHB 0 clock configurations
 *  @param[in] UartClkConfig UART clock source selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetUart0ClkConfig(UINT32 UartClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    switch (UartClkConfig) {
    case AMBA_PLL_UART_CLK_REF:
        AmbaCSL_PllSetUart0ClkSrcSel(0U);
        break;
    case AMBA_PLL_UART_CLK_CORE:
        AmbaCSL_PllSetUart0ClkSrcSel(1U);
        break;
    case AMBA_PLL_UART_CLK_ENET:
        AmbaCSL_PllSetUart0ClkSrcSel(2U);
        break;
    case AMBA_PLL_UART_CLK_SD:
        AmbaCSL_PllSetUart0ClkSrcSel(3U);
        break;
    default:
        RetVal = PLL_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetUart1ClkConfig - Set UART AHB 1 clock configurations
 *  @param[in] UartClkConfig UART clock source selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetUart1ClkConfig(UINT32 UartClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    switch (UartClkConfig) {
    case AMBA_PLL_UART_CLK_REF:
        AmbaCSL_PllSetUart1ClkSrcSel(0U);
        break;
    case AMBA_PLL_UART_CLK_CORE:
        AmbaCSL_PllSetUart1ClkSrcSel(1U);
        break;
    case AMBA_PLL_UART_CLK_ENET:
        AmbaCSL_PllSetUart1ClkSrcSel(2U);
        break;
    case AMBA_PLL_UART_CLK_SD:
        AmbaCSL_PllSetUart1ClkSrcSel(3U);
        break;
    default:
        RetVal = PLL_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetUart2ClkConfig - Set UART AHB 2 clock configurations
 *  @param[in] UartClkConfig UART clock source selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetUart2ClkConfig(UINT32 UartClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    switch (UartClkConfig) {
    case AMBA_PLL_UART_CLK_REF:
        AmbaCSL_PllSetUart2ClkSrcSel(0U);
        break;
    case AMBA_PLL_UART_CLK_CORE:
        AmbaCSL_PllSetUart2ClkSrcSel(1U);
        break;
    case AMBA_PLL_UART_CLK_ENET:
        AmbaCSL_PllSetUart2ClkSrcSel(2U);
        break;
    case AMBA_PLL_UART_CLK_SD:
        AmbaCSL_PllSetUart2ClkSrcSel(3U);
        break;
    default:
        RetVal = PLL_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetUart3ClkConfig - Set UART AHB 3 clock configurations
 *  @param[in] UartClkConfig UART clock source selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetUart3ClkConfig(UINT32 UartClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    switch (UartClkConfig) {
    case AMBA_PLL_UART_CLK_REF:
        AmbaCSL_PllSetUart3ClkSrcSel(0U);
        break;
    case AMBA_PLL_UART_CLK_CORE:
        AmbaCSL_PllSetUart3ClkSrcSel(1U);
        break;
    case AMBA_PLL_UART_CLK_ENET:
        AmbaCSL_PllSetUart3ClkSrcSel(2U);
        break;
    case AMBA_PLL_UART_CLK_SD:
        AmbaCSL_PllSetUart3ClkSrcSel(3U);
        break;
    default:
        RetVal = PLL_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetVoutLcdClkConfig - Set VOUT-LCD clock configuration
 *  @param[in] VoutLcdClkConfig VOUT-LCD clock source selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetVoutLcdClkConfig(UINT32 VoutLcdClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    if (VoutLcdClkConfig >= AMBA_NUM_PLL_VOUT_LCD_CLK_CONFIG) {
        RetVal = PLL_ERR_ARG;
    } else {
        if (VoutLcdClkConfig == AMBA_PLL_VOUT_LCD_CLK_REF) {
            pAmbaRCT_Reg->VoutLcdRefClkCtrl0.ClkRefSel = (UINT8)0U;  /* clk_ref */
        } else {
            pAmbaRCT_Reg->VoutLcdRefClkCtrl0.ClkRefSel = (UINT8)1U;

            if (VoutLcdClkConfig == AMBA_PLL_VOUT_LCD_CLK_SENSOR) {
                pAmbaRCT_Reg->VoutLcdRefClkCtrl1.ExtClkSrc = (UINT8)1U;  /* xx_clk_si */
            } else {
                pAmbaRCT_Reg->VoutLcdRefClkCtrl1.ExtClkSrc = (UINT8)2U;  /* MUXED_LVDS_SPCLK */
            }
        }
        (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_VIDEO2);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetVin1ClkConfig - Set the Vin1 Clock Config
 *  @param[in] Vin1ClkConfig Vin1 clock source selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetVin1ClkConfig(UINT32 Vin1ClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    switch (Vin1ClkConfig) {
    case AMBA_PLL_VIN1_CLK_EXT:
        pAmbaRCT_Reg->EnetClkSrcSelect = 0U;
        break;

    case AMBA_PLL_VIN1_CLK_REF:
        pAmbaRCT_Reg->EnetClkSrcSelect = 1U;
        break;

    default:
        RetVal = PLL_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllClkEnable - The function is used to ebable the power of specified HW module.
 *  @param[in] SysFeature - Features ID
 */
void AmbaRTSL_PllClkEnable(UINT32 SysFeature)
{
    UINT32 CurValue = AmbaCSL_PllGetClkCtrl();
    UINT32 NewValue;

    switch (SysFeature) {
    case AMBA_SYS_FEATURE_DSP:
        NewValue = 0x3144U;
        break;
    case AMBA_SYS_FEATURE_MCTF:
        NewValue = 0x10000U;
        break;
    case AMBA_SYS_FEATURE_CODEC:
        NewValue = 0x10U;
        break;
    case AMBA_SYS_FEATURE_HEVC:
        NewValue = 0xc000U;
        break;
    case AMBA_SYS_FEATURE_CNN:
        NewValue = 0x5140000U;
        break;
    case AMBA_SYS_FEATURE_ALL:
        NewValue = 0xfffffffU;
        break;
    default:
        NewValue = 0U;
        break;
    }

    AmbaCSL_PllSetClkCtrl(CurValue | NewValue);
}

/**
 *  AmbaRTSL_PllClkDisable - The function is used to disable the power of specified HW module.
 *  @param[in] SysFeature - Features ID
 */
void AmbaRTSL_PllClkDisable(UINT32 SysFeature)
{
    UINT32 CurValue = AmbaCSL_PllGetClkCtrl();
    UINT32 NewValue;

    switch (SysFeature) {
    case AMBA_SYS_FEATURE_DSP:
        NewValue = 0x3144U;
        break;
    case AMBA_SYS_FEATURE_MCTF:
        NewValue = 0x10000U;
        break;
    case AMBA_SYS_FEATURE_CODEC:
        NewValue = 0x10U;
        break;
    case AMBA_SYS_FEATURE_HEVC:
        NewValue = 0xc000U;
        break;
    case AMBA_SYS_FEATURE_CNN:
        NewValue = 0x5140000U;
        break;
    case AMBA_SYS_FEATURE_ALL:
        NewValue = 0xfffffffU;
        break;
    default:
        NewValue = 0U;
        break;
    }

    AmbaCSL_PllSetClkCtrl(CurValue & (~(NewValue)));
}

/**
 *  AmbaRTSL_PllClkCheck - The function is used to check the feature of specified HW module is enabled or not.
 *  @param[in] SysFeature - Features ID
 *  @param[out] pEnable - Enable Flag, 0 = not enabled, 1 = enabled
 */
void AmbaRTSL_PllClkCheck(UINT32 SysFeature, UINT32 *pEnable)
{
    UINT32 CurValue = AmbaCSL_PllGetClkCtrl();
    UINT32 FeatureMask = 0U;

    switch (SysFeature) {
    case AMBA_SYS_FEATURE_DSP:
        FeatureMask = 0x3144U;
        break;
    case AMBA_SYS_FEATURE_MCTF:
        FeatureMask = 0x10000U;
        break;
    case AMBA_SYS_FEATURE_CODEC:
        FeatureMask = 0x10U;
        break;
    case AMBA_SYS_FEATURE_HEVC:
        FeatureMask = 0xc000U;
        break;
    case AMBA_SYS_FEATURE_CNN:
        FeatureMask = 0x5140000U;
        break;
    case AMBA_SYS_FEATURE_ALL:
        FeatureMask = 0xfffffffU;
        break;
    default:
        FeatureMask = 0U;
        break;
    }

    if ((CurValue & FeatureMask) == FeatureMask) {
        *pEnable = (UINT32)1U;
    } else {
        *pEnable = (UINT32)0U;
    }
}

/**
 *  AmbaRTSL_PllVdspClkDisable - Disable Vdsp clocks
 */
void AmbaRTSL_PllVdspClkDisable(void)
{
    AmbaRTSL_PllClkDisable(AMBA_SYS_FEATURE_ALL);
}

/**
 *  AmbaRTSL_PllIdspClkDisable - Disable IDSP PLL scalar
 */
void AmbaRTSL_PllIdspClkDisable(void)
{
    /* disable IDSP PLL scalar */
    AmbaCSL_PllDisableIdspScalar();
}

/**
 *  AmbaRTSL_RctTimerWait - wait for a while through RCT Timer
 *  @param[in] TimeOut Time out value in us
 *  @return error code
 */
UINT32 AmbaRTSL_RctTimerWait(UINT32 TimeOut)
{
    UINT32 TimeLimit = TimeOut * (AmbaRTSL_PllGetClkRefFreq() / 1000000U);

    AmbaCSL_RctTimer0Reset();   /* reset RCT Timer */
    AmbaCSL_RctTimer0Enable();  /* enable RCT Timer */

    while (AmbaCSL_RctTimer0GetCounter() < TimeLimit) {
        continue;
    }

    AmbaCSL_RctTimer0Reset();   /* reset RCT Timer */

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetNumCpuCycleUs - Set cpu tick value per micro-second
 *  @param[in] Frequency CPU clock frequency
 */
void AmbaRTSL_PllSetNumCpuCycleUs(UINT32 Frequency)
{
    AmbaCSL_PllSetNumCpuCycleUs(Frequency);
}

/**
 *  AmbaRTSL_PllSetNumCpuCycleMs - Set system tick value per milli-second
 *  @param[in] Frequency CPU clock frequency
 */
void AmbaRTSL_PllSetNumCpuCycleMs(UINT32 Frequency)
{
    AmbaCSL_PllSetNumCpuCycleMs(Frequency);
}

/**
 *  AmbaRTSL_PllGetNumCpuCycleUs - Get cpu cycle value per micro-second
 *  @return Number of CPU cycles per micro-second
 */
UINT32 AmbaRTSL_PllGetNumCpuCycleUs(void)
{
    return AmbaCSL_PllGetNumCpuCycleUs();
}

/**
 *  AmbaRTSL_PllGetNumCpuCycleMs - Get cpu cycle value per milli-second
 *  @return Number of CPU cycles per milli-second
 */
UINT32 AmbaRTSL_PllGetNumCpuCycleMs(void)
{
    return AmbaCSL_PllGetNumCpuCycleMs();
}
