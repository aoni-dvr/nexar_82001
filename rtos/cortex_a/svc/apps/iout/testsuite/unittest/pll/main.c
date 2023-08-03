#include <stdio.h>
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaCSL_PLL.h"
#include "AmbaReg_ScratchpadNS.h"

AMBA_SCRATCHPAD_NS_REG_s AmbaScratchpadNS_Reg = {0};
AMBA_SCRATCHPAD_NS_REG_s *const pAmbaScratchpadNS_Reg = &AmbaScratchpadNS_Reg;

AMBA_RCT_REG_s AmbaRCT_Reg = {0};
AMBA_RCT_REG_s *const pAmbaRCT_Reg = &AmbaRCT_Reg;

void TEST_AmbaRTSL_PllInit(void)
{
    UINT32 ClkRefFreq = 0U;
    AmbaRTSL_PllInit(ClkRefFreq);

    pAmbaRCT_Reg->AudioRefClkCtrl0.ClkRefSel = 0x1U;
    AmbaRTSL_PllInit(ClkRefFreq);

    pAmbaRCT_Reg->AudioRefClkCtrl1.ExtClkSrc = 0x1U;
    AmbaRTSL_PllInit(ClkRefFreq);

    pAmbaRCT_Reg->AudioRefClkCtrl1.ExtClkSrc = 0x2U;
    AmbaRTSL_PllInit(ClkRefFreq);


    pAmbaRCT_Reg->Vout0RefClkCtrl0.ClkRefSel = 0x1U;
    AmbaRTSL_PllInit(ClkRefFreq);

    pAmbaRCT_Reg->Vout0RefClkCtrl1.ExtClkSrc = 0x1U;
    AmbaRTSL_PllInit(ClkRefFreq);

    pAmbaRCT_Reg->Vout0RefClkCtrl1.ExtClkSrc = 0x2U;
    AmbaRTSL_PllInit(ClkRefFreq);


    pAmbaRCT_Reg->Vout1RefClkCtrl0.ClkRefSel = 0x1U;
    AmbaRTSL_PllInit(ClkRefFreq);

    pAmbaRCT_Reg->Vout1RefClkCtrl1.ExtClkSrc = 0x1U;
    AmbaRTSL_PllInit(ClkRefFreq);

    pAmbaRCT_Reg->Vout1RefClkCtrl1.ExtClkSrc = 0x2U;
    AmbaRTSL_PllInit(ClkRefFreq);

    printf("TEST_AmbaRTSL_PllInit\n");
}

void TEST_AmbaRTSL_PllNotifyFreqChanges(void)
{
    UINT32 PllID = AMBA_NUM_PLL;
    AmbaRTSL_PllNotifyFreqChanges(PllID);

    PllID = AMBA_PLL_CORE;
    AmbaRTSL_PllNotifyFreqChanges(PllID);

    PllID = AMBA_PLL_CORTEX1;
    AmbaRTSL_PllNotifyFreqChanges(PllID);

    PllID = AMBA_NUM_PLL - 1U;
    AmbaRTSL_PllNotifyFreqChanges(PllID);

    printf("TEST_AmbaRTSL_PllNotifyFreqChanges\n");
}

void TEST_AmbaRTSL_PllSetExtClkConfig(void)
{
    AMBA_PLL_EXT_CLK_CONFIG_s ExtClkConfig = {0};
    AMBA_PLL_EXT_CLK_CONFIG_s *pExtClkConfig = &ExtClkConfig;

    pExtClkConfig->SpclkMuxSelect = AMBA_PLL_MUX_SPCLK_NA;
    AmbaRTSL_PllSetExtClkConfig(pExtClkConfig);

    pExtClkConfig->SpclkMuxSelect = AMBA_PLL_MUX_SPCLK_C0;
    AmbaRTSL_PllSetExtClkConfig(pExtClkConfig);

    pExtClkConfig->SpclkMuxSelect = AMBA_PLL_NUM_MUX_SPCLK;
    AmbaRTSL_PllSetExtClkConfig(pExtClkConfig);

    AmbaRTSL_PllSetExtClkConfig(NULL);

    printf("TEST_AmbaRTSL_PllSetExtClkConfig\n");
}

void TEST_AmbaRTSL_PllGetExtClkConfig(void)
{
    AMBA_PLL_EXT_CLK_CONFIG_s ExtClkConfig = {0};
    AMBA_PLL_EXT_CLK_CONFIG_s *pExtClkConfig = &ExtClkConfig;
    AmbaRTSL_PllGetExtClkConfig(pExtClkConfig);

    pExtClkConfig->SpclkMuxSelect = AMBA_PLL_MUX_SPCLK_NA;
    AmbaRTSL_PllSetExtClkConfig(pExtClkConfig);
    AmbaRTSL_PllGetExtClkConfig(pExtClkConfig);

    AmbaRTSL_PllGetExtClkConfig(NULL);

    printf("TEST_AmbaRTSL_PllGetExtClkConfig\n");
}

void TEST_AmbaRTSL_PllGetClkRefFreq(void)
{
    AmbaRTSL_PllGetClkRefFreq();

    printf("TEST_AmbaRTSL_PllGetClkRefFreq\n");
}

void TEST_AmbaRTSL_PllGetPllRefFreq(void)
{
    UINT32 PllID = AMBA_NUM_PLL - 1U;
    AmbaRTSL_PllGetPllRefFreq(PllID);

    PllID = AMBA_NUM_PLL;
    AmbaRTSL_PllGetPllRefFreq(PllID);

    printf("TEST_AmbaRTSL_PllGetPllRefFreq\n");
}

void TEST_AmbaRTSL_PllSetPllOutFreq(void)
{
    UINT32 PllID = AMBA_NUM_PLL - 1U;
    FLOAT Freq = 0;
    AmbaRTSL_PllSetPllOutFreq(PllID, Freq);

    PllID = AMBA_NUM_PLL;
    AmbaRTSL_PllSetPllOutFreq(PllID, Freq);

    printf("TEST_AmbaRTSL_PllSetPllOutFreq\n");
}

void TEST_AmbaRTSL_PllGetPllOutFreq(void)
{
    UINT32 PllID = AMBA_NUM_PLL - 1U;
    AmbaRTSL_PllGetPllOutFreq(PllID);

    PllID = AMBA_NUM_PLL;
    AmbaRTSL_PllGetPllOutFreq(PllID);

    printf("TEST_AmbaRTSL_PllGetPllOutFreq\n");
}

#if defined(CONFIG_SOC_CV2)
void TEST_AmbaRTSL_PllSetAdcClkConfig(void)
{
    UINT32 AdcClkConfig = 0U;
    AmbaRTSL_PllSetAdcClkConfig(AdcClkConfig);

    printf("TEST_AmbaRTSL_PllSetAdcClkConfig\n");
}
#endif

#if defined(CONFIG_SOC_CV2)
void TEST_AmbaRTSL_PllSetAudioClkConfig(void)
{
    UINT32 AudioClkConfig = AMBA_PLL_AUDIO_CLK_MUXED_LVDS_SPCLK;
    AmbaRTSL_PllSetAudioClkConfig(AudioClkConfig);

    AudioClkConfig = AMBA_PLL_AUDIO_CLK_SENSOR;
    AmbaRTSL_PllSetAudioClkConfig(AudioClkConfig);

    AudioClkConfig = AMBA_PLL_AUDIO_CLK_REF;
    AmbaRTSL_PllSetAudioClkConfig(AudioClkConfig);

    AudioClkConfig = AMBA_PLL_AUDIO_CLK_EXT;
    AmbaRTSL_PllSetAudioClkConfig(AudioClkConfig);

    AudioClkConfig = AMBA_NUM_PLL_AUDIO_CLK_CONFIG;
    AmbaRTSL_PllSetAudioClkConfig(AudioClkConfig);

    printf("TEST_AmbaRTSL_PllSetAudioClkConfig\n");
}
#endif

#if defined(CONFIG_SOC_CV2)
void TEST_AmbaRTSL_PllSetAudio1ClkConfig(void)
{
    UINT32 AudioClkConfig = AMBA_PLL_AUDIO_CLK_MUXED_LVDS_SPCLK;
    AmbaRTSL_PllSetAudio1ClkConfig(AudioClkConfig);

    AudioClkConfig = AMBA_PLL_AUDIO_CLK_SENSOR;
    AmbaRTSL_PllSetAudio1ClkConfig(AudioClkConfig);

    AudioClkConfig = AMBA_PLL_AUDIO_CLK_REF;
    AmbaRTSL_PllSetAudio1ClkConfig(AudioClkConfig);

    AudioClkConfig = AMBA_PLL_AUDIO_CLK_EXT;
    AmbaRTSL_PllSetAudio1ClkConfig(AudioClkConfig);

    AudioClkConfig = AMBA_NUM_PLL_AUDIO_CLK_CONFIG;
    AmbaRTSL_PllSetAudio1ClkConfig(AudioClkConfig);

    printf("TEST_AmbaRTSL_PllSetAudio1ClkConfig\n");
}
#endif

void TEST_AmbaRTSL_PllSetCanClkConfig(void)
{
    UINT32 CanClkConfig = AMBA_NUM_PLL_CAN_CLK_CONFIG;
    AmbaRTSL_PllSetCanClkConfig(CanClkConfig);

    CanClkConfig = AMBA_PLL_CAN_CLK_REF;
    AmbaRTSL_PllSetCanClkConfig(CanClkConfig);

    CanClkConfig = AMBA_PLL_CAN_CLK_ETHERNET;
    AmbaRTSL_PllSetCanClkConfig(CanClkConfig);

    CanClkConfig = AMBA_PLL_CAN_CLK_AHB;
    AmbaRTSL_PllSetCanClkConfig(CanClkConfig);

    printf("TEST_AmbaRTSL_PllSetCanClkConfig\n");
}

#if defined(CONFIG_SOC_CV2)
void TEST_AmbaRTSL_PllSetMipiDsiClkConfig(void)
{
    UINT32 MipiDsiClkConfig = AMBA_NUM_PLL_MIPI_DSI_CLK_CONFIG;
    AmbaRTSL_PllSetMipiDsiClkConfig(MipiDsiClkConfig);

    MipiDsiClkConfig = AMBA_PLL_MIPI_DSI_CLK_VIDEO2;
    AmbaRTSL_PllSetMipiDsiClkConfig(MipiDsiClkConfig);

    MipiDsiClkConfig = AMBA_PLL_MIPI_DSI_CLK_HDMI;
    AmbaRTSL_PllSetMipiDsiClkConfig(MipiDsiClkConfig);

    printf("TEST_AmbaRTSL_PllSetMipiDsiClkConfig\n");
}
#endif

void TEST_AmbaRTSL_PllSetPwmClkConfig(void)
{
    UINT32 PwmClkConfig = AMBA_NUM_PLL_PWM_CLK_CONFIG;
    AmbaRTSL_PllSetPwmClkConfig(PwmClkConfig);

    PwmClkConfig = AMBA_PLL_PWM_CLK_APB;
    AmbaRTSL_PllSetPwmClkConfig(PwmClkConfig);

    PwmClkConfig = AMBA_PLL_PWM_CLK_CORTEX;
    AmbaRTSL_PllSetPwmClkConfig(PwmClkConfig);

    PwmClkConfig = AMBA_PLL_PWM_CLK_ENET;
    AmbaRTSL_PllSetPwmClkConfig(PwmClkConfig);

    PwmClkConfig = AMBA_PLL_PWM_CLK_REF;
    AmbaRTSL_PllSetPwmClkConfig(PwmClkConfig);

    printf("TEST_AmbaRTSL_PllSetPwmClkConfig\n");
}

void TEST_AmbaRTSL_PllSetSd0ScalerClkSrc(void)
{
    UINT32 SdClkConfig = AMBA_NUM_PLL_SD_CLK_CONFIG;
    AmbaRTSL_PllSetSd0ScalerClkSrc(SdClkConfig);

    SdClkConfig = AMBA_PLL_SD_CLK_SD;
    AmbaRTSL_PllSetSd0ScalerClkSrc(SdClkConfig);

    SdClkConfig = AMBA_PLL_SD_CLK_CORE;
    AmbaRTSL_PllSetSd0ScalerClkSrc(SdClkConfig);

    printf("TEST_AmbaRTSL_PllSetSd0ScalerClkSrc\n");
}

void TEST_AmbaRTSL_PllSetSd1ScalerClkSrc(void)
{
    UINT32 SdClkConfig = AMBA_NUM_PLL_SD_CLK_CONFIG;
    AmbaRTSL_PllSetSd1ScalerClkSrc(SdClkConfig);

    SdClkConfig = AMBA_PLL_SD_CLK_SD;
    AmbaRTSL_PllSetSd1ScalerClkSrc(SdClkConfig);

    SdClkConfig = AMBA_PLL_SD_CLK_CORE;
    AmbaRTSL_PllSetSd1ScalerClkSrc(SdClkConfig);

    printf("TEST_AmbaRTSL_PllSetSd1ScalerClkSrc\n");
}

void TEST_AmbaRTSL_PllSetSensor0ClkDir(void)
{
    UINT32 ClkSiDir = 3U;
    AmbaRTSL_PllSetSensor0ClkDir(ClkSiDir);

    ClkSiDir = AMBA_PLL_SENSOR_REF_CLK_OUTPUT;
    AmbaRTSL_PllSetSensor0ClkDir(ClkSiDir);

    ClkSiDir = AMBA_PLL_SENSOR_REF_CLK_INPUT;
    AmbaRTSL_PllSetSensor0ClkDir(ClkSiDir);

    printf("TEST_AmbaRTSL_PllSetSensor0ClkDir\n");
}

#if defined(CONFIG_SOC_CV2)
void TEST_AmbaRTSL_PllSetSensor1ClkDir(void)
{
    UINT32 ClkSiDir = 3U;
    AmbaRTSL_PllSetSensor1ClkDir(ClkSiDir);

    ClkSiDir = AMBA_PLL_SENSOR_REF_CLK_OUTPUT;
    AmbaRTSL_PllSetSensor1ClkDir(ClkSiDir);

    ClkSiDir = AMBA_PLL_SENSOR_REF_CLK_INPUT;
    AmbaRTSL_PllSetSensor1ClkDir(ClkSiDir);

    printf("TEST_AmbaRTSL_PllSetSensor1ClkDir\n");
}
#endif

#if defined(CONFIG_SOC_CV2)
void TEST_AmbaRTSL_PllSetSpiClkConfig(void)
{
    UINT32 SpiClkConfig = AMBA_NUM_PLL_SPI_CLK_CONFIG;
    AmbaRTSL_PllSetSpiClkConfig(SpiClkConfig);

    SpiClkConfig = AMBA_PLL_SPI_CLK_ETHERNET;
    AmbaRTSL_PllSetSpiClkConfig(SpiClkConfig);

    SpiClkConfig = AMBA_PLL_SPI_CLK_CORE;
    AmbaRTSL_PllSetSpiClkConfig(SpiClkConfig);

    SpiClkConfig = AMBA_PLL_SPI_CLK_SPI;
    AmbaRTSL_PllSetSpiClkConfig(SpiClkConfig);

    SpiClkConfig = AMBA_PLL_SPI_CLK_REF;
    AmbaRTSL_PllSetSpiClkConfig(SpiClkConfig);

    printf("TEST_AmbaRTSL_PllSetSpiClkConfig\n");
}
#endif

void TEST_AmbaRTSL_PllSetUartApbClkConfig(void)
{
    UINT32 UartClkConfig = AMBA_NUM_PLL_UART_CLK_CONFIG;
    AmbaRTSL_PllSetUartApbClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_REF;
    AmbaRTSL_PllSetUartApbClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_CORE;
    AmbaRTSL_PllSetUartApbClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_ENET;
    AmbaRTSL_PllSetUartApbClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_SD;
    AmbaRTSL_PllSetUartApbClkConfig(UartClkConfig);

    printf("TEST_AmbaRTSL_PllSetUartApbClkConfig\n");
}

void TEST_AmbaRTSL_PllSetUart0ClkConfig(void)
{
    UINT32 UartClkConfig = AMBA_NUM_PLL_UART_CLK_CONFIG;
    AmbaRTSL_PllSetUart0ClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_REF;
    AmbaRTSL_PllSetUart0ClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_CORE;
    AmbaRTSL_PllSetUart0ClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_ENET;
    AmbaRTSL_PllSetUart0ClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_SD;
    AmbaRTSL_PllSetUart0ClkConfig(UartClkConfig);

    printf("TEST_AmbaRTSL_PllSetUart0ClkConfig\n");
}

void TEST_AmbaRTSL_PllSetUart1ClkConfig(void)
{
    UINT32 UartClkConfig = AMBA_NUM_PLL_UART_CLK_CONFIG;
    AmbaRTSL_PllSetUart1ClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_REF;
    AmbaRTSL_PllSetUart1ClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_CORE;
    AmbaRTSL_PllSetUart1ClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_ENET;
    AmbaRTSL_PllSetUart1ClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_SD;
    AmbaRTSL_PllSetUart1ClkConfig(UartClkConfig);

    printf("TEST_AmbaRTSL_PllSetUart1ClkConfig\n");
}

void TEST_AmbaRTSL_PllSetUart2ClkConfig(void)
{
    UINT32 UartClkConfig = AMBA_NUM_PLL_UART_CLK_CONFIG;
    AmbaRTSL_PllSetUart2ClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_REF;
    AmbaRTSL_PllSetUart2ClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_CORE;
    AmbaRTSL_PllSetUart2ClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_ENET;
    AmbaRTSL_PllSetUart2ClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_SD;
    AmbaRTSL_PllSetUart2ClkConfig(UartClkConfig);

    printf("TEST_AmbaRTSL_PllSetUart2ClkConfig\n");
}

void TEST_AmbaRTSL_PllSetUart3ClkConfig(void)
{
    UINT32 UartClkConfig = AMBA_NUM_PLL_UART_CLK_CONFIG;
    AmbaRTSL_PllSetUart3ClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_REF;
    AmbaRTSL_PllSetUart3ClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_CORE;
    AmbaRTSL_PllSetUart3ClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_ENET;
    AmbaRTSL_PllSetUart3ClkConfig(UartClkConfig);

    UartClkConfig = AMBA_PLL_UART_CLK_SD;
    AmbaRTSL_PllSetUart3ClkConfig(UartClkConfig);

    printf("TEST_AmbaRTSL_PllSetUart3ClkConfig\n");
}

#if defined(CONFIG_SOC_CV2)
void TEST_AmbaRTSL_PllSetVoutLcdClkConfig(void)
{
    UINT32 VoutLcdClkConfig = AMBA_NUM_PLL_VOUT_LCD_CLK_CONFIG;
    AmbaRTSL_PllSetVoutLcdClkConfig(VoutLcdClkConfig);

    printf("TEST_AmbaRTSL_PllSetVoutLcdClkConfig\n");
}
#endif

#if defined(CONFIG_SOC_CV2)
void TEST_AmbaRTSL_PllSetVoutTvClkConfig(void)
{

    printf("TEST_AmbaRTSL_PllSetVoutTvClkConfig\n");
}
#endif

void TEST_AmbaRTSL_PllSetVout0ClkConfig(void)
{
    UINT32 VoutClkConfig = AMBA_NUM_PLL_VOUT_CLK_CONFIG;
    AmbaRTSL_PllSetVout0ClkConfig(VoutClkConfig);

    VoutClkConfig = AMBA_PLL_VOUT_CLK_MUXED_LVDS_SPCLK;
    AmbaRTSL_PllSetVout0ClkConfig(VoutClkConfig);

    VoutClkConfig = AMBA_PLL_VOUT_CLK_SENSOR;
    AmbaRTSL_PllSetVout0ClkConfig(VoutClkConfig);

    VoutClkConfig = AMBA_PLL_VOUT_CLK_REF;
    AmbaRTSL_PllSetVout0ClkConfig(VoutClkConfig);

    VoutClkConfig = AMBA_PLL_VOUT_CLK_VOUT0;
    AmbaRTSL_PllSetVout0ClkConfig(VoutClkConfig);

    printf("TEST_AmbaRTSL_PllSetVout0ClkConfig\n");
}

void TEST_AmbaRTSL_PllSetVout1ClkConfig(void)
{
    UINT32 VoutClkConfig = AMBA_NUM_PLL_VOUT_CLK_CONFIG;
    AmbaRTSL_PllSetVout1ClkConfig(VoutClkConfig);

    VoutClkConfig = AMBA_PLL_VOUT_CLK_MUXED_LVDS_SPCLK;
    AmbaRTSL_PllSetVout1ClkConfig(VoutClkConfig);

    VoutClkConfig = AMBA_PLL_VOUT_CLK_SENSOR;
    AmbaRTSL_PllSetVout1ClkConfig(VoutClkConfig);

    VoutClkConfig = AMBA_PLL_VOUT_CLK_REF;
    AmbaRTSL_PllSetVout1ClkConfig(VoutClkConfig);

    VoutClkConfig = AMBA_PLL_VOUT_CLK_VOUT0;
    AmbaRTSL_PllSetVout1ClkConfig(VoutClkConfig);

    printf("TEST_AmbaRTSL_PllSetVout1ClkConfig\n");
}

void TEST_AmbaRTSL_PllSetVin1ClkConfig(void)
{
    UINT32 Vin1ClkConfig = AMBA_PLL_NUM_SENSOR_CLK;
    AmbaRTSL_PllSetVin1ClkConfig(Vin1ClkConfig);

    Vin1ClkConfig = AMBA_PLL_SENSOR_CLK_0;
    AmbaRTSL_PllSetVin1ClkConfig(Vin1ClkConfig);

    Vin1ClkConfig = AMBA_PLL_SENSOR_CLK_1;
    AmbaRTSL_PllSetVin1ClkConfig(Vin1ClkConfig);

    printf("TEST_AmbaRTSL_PllSetVin1ClkConfig\n");
}

void TEST_AmbaRTSL_PllSetEnetClkConfig(void)
{
    UINT32 EnetClkConfig = AMBA_NUM_PLL_ENET_CLK_CONFIG;
    AmbaRTSL_PllSetEnetClkConfig(EnetClkConfig);

    EnetClkConfig = AMBA_PLL_ENET_CLK_EXT;
    AmbaRTSL_PllSetEnetClkConfig(EnetClkConfig);

    EnetClkConfig = AMBA_PLL_ENET_CLK_REF;
    AmbaRTSL_PllSetEnetClkConfig(EnetClkConfig);

    printf("TEST_AmbaRTSL_PllSetEnetClkConfig\n");
}


void TEST_AmbaRTSL_PllClkEnable(void)
{
    UINT32 SysFeature = AMBA_SYS_FEATURE_DSP;
    AmbaRTSL_PllClkEnable(SysFeature);

    SysFeature = AMBA_SYS_FEATURE_CODEC;
    AmbaRTSL_PllClkEnable(SysFeature);

    SysFeature = AMBA_SYS_FEATURE_CNN;
    AmbaRTSL_PllClkEnable(SysFeature);

    SysFeature = AMBA_SYS_FEATURE_FEX;
    AmbaRTSL_PllClkEnable(SysFeature);

    SysFeature = AMBA_SYS_FEATURE_ALL;
    AmbaRTSL_PllClkEnable(SysFeature);

    SysFeature = AMBA_SYS_FEATURE_MCTF;
    AmbaRTSL_PllClkEnable(SysFeature);

    printf("TEST_AmbaRTSL_PllClkEnable\n");
}

void TEST_AmbaRTSL_PllClkDisable(void)
{
    UINT32 SysFeature = AMBA_SYS_FEATURE_DSP;
    AmbaRTSL_PllClkDisable(SysFeature);

    SysFeature = AMBA_SYS_FEATURE_CODEC;
    AmbaRTSL_PllClkDisable(SysFeature);

    SysFeature = AMBA_SYS_FEATURE_CNN;
    AmbaRTSL_PllClkDisable(SysFeature);

    SysFeature = AMBA_SYS_FEATURE_FEX;
    AmbaRTSL_PllClkDisable(SysFeature);

    SysFeature = AMBA_SYS_FEATURE_ALL;
    AmbaRTSL_PllClkDisable(SysFeature);

    SysFeature = AMBA_SYS_FEATURE_MCTF;
    AmbaRTSL_PllClkDisable(SysFeature);

    printf("TEST_AmbaRTSL_PllClkDisable\n");
}

void TEST_AmbaRTSL_PllClkCheck(void)
{
    UINT32 SysFeature = AMBA_SYS_FEATURE_DSP;
    UINT32 Enable = 0U;
    UINT32 *pEnable = &Enable;
    AmbaRTSL_PllClkCheck(SysFeature, pEnable);

    SysFeature = AMBA_SYS_FEATURE_CODEC;
    AmbaRTSL_PllClkCheck(SysFeature, pEnable);

    SysFeature = AMBA_SYS_FEATURE_CNN;
    AmbaRTSL_PllClkCheck(SysFeature, pEnable);

    SysFeature = AMBA_SYS_FEATURE_FEX;
    AmbaRTSL_PllClkCheck(SysFeature, pEnable);

    SysFeature = AMBA_SYS_FEATURE_ALL;
    AmbaRTSL_PllClkCheck(SysFeature, pEnable);

    SysFeature = AMBA_SYS_FEATURE_MCTF;
    AmbaRTSL_PllClkCheck(SysFeature, pEnable);

    printf("TEST_AmbaRTSL_PllClkCheck\n");
}

void TEST_AmbaRTSL_PllVdspClkDisable(void)
{
    AmbaRTSL_PllVdspClkDisable();

    printf("TEST_AmbaRTSL_PllVdspClkDisable\n");
}

void TEST_AmbaRTSL_PllIdspClkDisable(void)
{
    AmbaRTSL_PllIdspClkDisable();

    printf("TEST_AmbaRTSL_PllIdspClkDisable\n");
}

void TEST_AmbaRTSL_RctTimerWait(void)
{
    UINT32 TimeOut = 0U;
    AmbaRTSL_RctTimerWait(TimeOut);

    printf("TEST_AmbaRTSL_RctTimerWait\n");
}

void TEST_AmbaRTSL_PllSetNumCpuCycleUs(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetNumCpuCycleUs(Frequency);

    printf("TEST_AmbaRTSL_PllSetNumCpuCycleUs\n");
}

void TEST_AmbaRTSL_PllSetNumCpuCycleMs(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetNumCpuCycleMs(Frequency);

    printf("TEST_AmbaRTSL_PllSetNumCpuCycleMs\n");
}

void TEST_AmbaRTSL_PllGetNumCpuCycleUs(void)
{
    AmbaRTSL_PllGetNumCpuCycleUs();

    printf("TEST_AmbaRTSL_PllGetNumCpuCycleUs\n");
}

void TEST_AmbaRTSL_PllGetNumCpuCycleMs(void)
{
    AmbaRTSL_PllGetNumCpuCycleMs();

    printf("TEST_AmbaRTSL_PllGetNumCpuCycleMs\n");
}


void TEST_AmbaRTSL_PllGetAhbClk(void)
{
    AmbaRTSL_PllGetAhbClk();

    printf("TEST_AmbaRTSL_PllGetAhbClk\n");
}

void TEST_AmbaRTSL_PllGetApbClk(void)
{
    AmbaRTSL_PllGetApbClk();

    printf("TEST_AmbaRTSL_PllGetApbClk\n");
}

void TEST_AmbaRTSL_PllGetAxiClk(void)
{
    AmbaRTSL_PllGetAxiClk();

    printf("TEST_AmbaRTSL_PllGetAxiClk\n");
}

void TEST_AmbaRTSL_PllGetAudioClk(void)
{
    AmbaRTSL_PllGetAudioClk();

    pAmbaRCT_Reg->AudioExtClkCtrl.AudioClkSelect = 0x1U;
    AmbaRTSL_PllGetAudioClk();

    printf("TEST_AmbaRTSL_PllGetAudioClk\n");
}

void TEST_AmbaRTSL_PllGetCanClk(void)
{
    pAmbaRCT_Reg->CanClkDivider.ClkDivider = 1;

    pAmbaRCT_Reg->CanClkSrcSelect.ClkSrc = 0;
    AmbaRTSL_PllGetCanClk();

    pAmbaRCT_Reg->CanClkSrcSelect.ClkSrc = 1;
    AmbaRTSL_PllGetCanClk();

    pAmbaRCT_Reg->CanClkSrcSelect.ClkSrc = 2;
    AmbaRTSL_PllGetCanClk();

    pAmbaRCT_Reg->CanClkSrcSelect.ClkSrc = 3;
    AmbaRTSL_PllGetCanClk();

    printf("TEST_AmbaRTSL_PllGetCanClk\n");
}

void TEST_AmbaRTSL_PllGetCoreClk(void)
{
    AmbaRTSL_PllGetCoreClk();

    printf("TEST_AmbaRTSL_PllGetCoreClk\n");
}

void TEST_AmbaRTSL_PllGetCortex0Clk(void)
{
    AmbaRTSL_PllGetCortex0Clk();

    pAmbaRCT_Reg->Cortex0ClkSrcSelect.ClkSrc = 0x1U;
    AmbaRTSL_PllGetCortex0Clk();

    printf("TEST_AmbaRTSL_PllGetCortex0Clk\n");
}

void TEST_AmbaRTSL_PllGetCortex1Clk(void)
{
    AmbaRTSL_PllGetCortex1Clk();

    pAmbaRCT_Reg->Cortex1ClkSrcSelect.ClkSrc = 0x1U;
    AmbaRTSL_PllGetCortex1Clk();

    printf("TEST_AmbaRTSL_PllGetCortex1Clk\n");
}

void TEST_AmbaRTSL_PllGetDbgClk(void)
{
    AmbaRTSL_PllGetDbgClk();

    printf("TEST_AmbaRTSL_PllGetDbgClk\n");
}

void TEST_AmbaRTSL_PllGetDebounceClk(void)
{
    pAmbaRCT_Reg->PlugInDetectClkDivider.ClkDivider = 1;

    AmbaRTSL_PllGetDebounceClk();

    printf("TEST_AmbaRTSL_PllGetDebounceClk\n");
}

void TEST_AmbaRTSL_PllGetDramClk(void)
{
    AmbaRTSL_PllGetDramClk();

    printf("TEST_AmbaRTSL_PllGetDramClk\n");
}

void TEST_AmbaRTSL_PllGetEthernetClk(void)
{
    AmbaRTSL_PllGetEthernetClk();

    printf("TEST_AmbaRTSL_PllGetEthernetClk\n");
}

void TEST_AmbaRTSL_PllGetEthernet1Clk(void)
{
    AmbaRTSL_PllGetEthernet1Clk();

    printf("TEST_AmbaRTSL_PllGetEthernet1Clk\n");
}

void TEST_AmbaRTSL_PllGetFexClk(void)
{
    AmbaRTSL_PllGetFexClk();

    printf("TEST_AmbaRTSL_PllGetFexClk\n");
}

void TEST_AmbaRTSL_PllGetIdspClk(void)
{
    AmbaRTSL_PllGetIdspClk();

    printf("TEST_AmbaRTSL_PllGetIdspClk\n");
}

void TEST_AmbaRTSL_PllGetMipiDsi0Clk(void)
{
    AmbaRTSL_PllGetMipiDsi0Clk();

    pAmbaRCT_Reg->VoutClkSelect.ComboPhy0ClkSelect = 0x1U;
    AmbaRTSL_PllGetMipiDsi0Clk();

    printf("TEST_AmbaRTSL_PllGetMipiDsi0Clk\n");
}

void TEST_AmbaRTSL_PllGetMipiDsi1Clk(void)
{
    AmbaRTSL_PllGetMipiDsi1Clk();

    pAmbaRCT_Reg->VoutClkSelect.ComboPhy1ClkSelect = 0x1U;
    AmbaRTSL_PllGetMipiDsi1Clk();

    printf("TEST_AmbaRTSL_PllGetMipiDsi1Clk\n");
}

void TEST_AmbaRTSL_PllGetNandClk(void)
{
    AmbaRTSL_PllGetNandClk();

    printf("TEST_AmbaRTSL_PllGetNandClk\n");
}

void TEST_AmbaRTSL_PllGetPwmClk(void)
{
    pAmbaRCT_Reg->PwmClkDivider.ClkDivider = 1;

    pAmbaRCT_Reg->PwmClkSrcSelect = 0;
    AmbaRTSL_PllGetPwmClk();

    pAmbaRCT_Reg->PwmClkSrcSelect = 1;
    AmbaRTSL_PllGetPwmClk();

    pAmbaRCT_Reg->PwmClkSrcSelect = 2;
    AmbaRTSL_PllGetPwmClk();

    pAmbaRCT_Reg->PwmClkSrcSelect = 3;
    AmbaRTSL_PllGetPwmClk();

    pAmbaRCT_Reg->PwmClkSrcSelect = 4;
    AmbaRTSL_PllGetPwmClk();

    printf("TEST_AmbaRTSL_PllGetPwmClk\n");
}

void TEST_AmbaRTSL_PllGetSd0Clk(void)
{
    pAmbaRCT_Reg->Sd0ClkDivider.ClkDivider = 1;

    AmbaRTSL_PllGetSd0Clk();

    printf("TEST_AmbaRTSL_PllGetSd0Clk\n");
}

void TEST_AmbaRTSL_PllGetSd1Clk(void)
{
    pAmbaRCT_Reg->Sd1ClkDivider.ClkDivider = 1;

    AmbaRTSL_PllGetSd1Clk();

    printf("TEST_AmbaRTSL_PllGetSd1Clk\n");
}

void TEST_AmbaRTSL_PllGetSensor0Clk(void)
{
    AmbaRTSL_PllGetSensor0Clk();

    printf("TEST_AmbaRTSL_PllGetSensor0Clk\n");
}

void TEST_AmbaRTSL_PllGetSensor1Clk(void)
{
    AmbaRTSL_PllGetSensor1Clk();

    printf("TEST_AmbaRTSL_PllGetSensor0Clk\n");
}

void TEST_AmbaRTSL_PllGetSensor2Clk(void)
{
    AmbaRTSL_PllGetSensor2Clk();

    printf("TEST_AmbaRTSL_PllGetSensor2Clk\n");
}

void TEST_AmbaRTSL_PllGetSpiMasterClk(void)
{
    pAmbaRCT_Reg->SpiMasterClkDivider.ClkDivider = 1;

    pAmbaRCT_Reg->SpiMasterClkSrcSelect = 0;
    AmbaRTSL_PllGetSpiMasterClk();

    pAmbaRCT_Reg->SpiMasterClkSrcSelect = 1;
    AmbaRTSL_PllGetSpiMasterClk();

    printf("TEST_AmbaRTSL_PllGetSpiMasterClk\n");
}

void TEST_AmbaRTSL_PllGetSpiSlaveClk(void)
{
    pAmbaRCT_Reg->SpiSlaveClkDivider.ClkDivider = 1;

    pAmbaRCT_Reg->SpiSlaveRefClkSelect.ClkSrc = 0;
    AmbaRTSL_PllGetSpiSlaveClk();

    pAmbaRCT_Reg->SpiSlaveRefClkSelect.ClkSrc = 1;
    AmbaRTSL_PllGetSpiSlaveClk();

    printf("TEST_AmbaRTSL_PllGetSpiSlaveClk\n");
}

void TEST_AmbaRTSL_PllGetSpiNorClk(void)
{
    pAmbaRCT_Reg->SpiNorClkDivider.ClkDivider = 1;

    pAmbaRCT_Reg->SpiNorRefClkSelect.ClkSrc = 0;
    AmbaRTSL_PllGetSpiNorClk();

    pAmbaRCT_Reg->SpiNorRefClkSelect.ClkSrc = 1;
    AmbaRTSL_PllGetSpiNorClk();

    pAmbaRCT_Reg->SpiNorRefClkSelect.ClkSrc = 2;
    AmbaRTSL_PllGetSpiNorClk();

    pAmbaRCT_Reg->SpiNorRefClkSelect.ClkSrc = 3;
    AmbaRTSL_PllGetSpiNorClk();

    printf("TEST_AmbaRTSL_PllGetSpiNorClk\n");
}

void TEST_AmbaRTSL_PllGetUartApbClk(void)
{
    pAmbaRCT_Reg->UartApbClkDivider.ClkDivider = 1;

    pAmbaRCT_Reg->UartApbClkSrcSelect.ClkSrc = 0;
    AmbaRTSL_PllGetUartApbClk();

    pAmbaRCT_Reg->UartApbClkSrcSelect.ClkSrc = 1;
    AmbaRTSL_PllGetUartApbClk();

    pAmbaRCT_Reg->UartApbClkSrcSelect.ClkSrc = 2;
    AmbaRTSL_PllGetUartApbClk();

    pAmbaRCT_Reg->UartApbClkSrcSelect.ClkSrc = 3;
    AmbaRTSL_PllGetUartApbClk();

    printf("TEST_AmbaRTSL_PllGetUartApbClk\n");
}

void TEST_AmbaRTSL_PllGetUart0Clk(void)
{
    pAmbaRCT_Reg->Uart0ClkDivider.ClkDivider = 1;

    pAmbaRCT_Reg->Uart0ClkSrcSelect.ClkSrc = 0;
    AmbaRTSL_PllGetUart0Clk();

    pAmbaRCT_Reg->Uart0ClkSrcSelect.ClkSrc = 1;
    AmbaRTSL_PllGetUart0Clk();

    pAmbaRCT_Reg->Uart0ClkSrcSelect.ClkSrc = 2;
    AmbaRTSL_PllGetUart0Clk();

    pAmbaRCT_Reg->Uart0ClkSrcSelect.ClkSrc = 3;
    AmbaRTSL_PllGetUart0Clk();

    printf("TEST_AmbaRTSL_PllGetUart0Clk\n");
}

void TEST_AmbaRTSL_PllGetUart1Clk(void)
{
    pAmbaRCT_Reg->Uart1ClkDivider.ClkDivider = 1;

    pAmbaRCT_Reg->Uart1ClkSrcSelect.ClkSrc = 0;
    AmbaRTSL_PllGetUart1Clk();

    pAmbaRCT_Reg->Uart1ClkSrcSelect.ClkSrc = 1;
    AmbaRTSL_PllGetUart1Clk();

    pAmbaRCT_Reg->Uart1ClkSrcSelect.ClkSrc = 2;
    AmbaRTSL_PllGetUart1Clk();

    pAmbaRCT_Reg->Uart1ClkSrcSelect.ClkSrc = 3;
    AmbaRTSL_PllGetUart1Clk();

    printf("TEST_AmbaRTSL_PllGetUart1Clk\n");
}

void TEST_AmbaRTSL_PllGetUart2Clk(void)
{
    pAmbaRCT_Reg->Uart2ClkDivider.ClkDivider = 1;

    pAmbaRCT_Reg->Uart2ClkSrcSelect.ClkSrc = 0;
    AmbaRTSL_PllGetUart2Clk();

    pAmbaRCT_Reg->Uart2ClkSrcSelect.ClkSrc = 1;
    AmbaRTSL_PllGetUart2Clk();

    pAmbaRCT_Reg->Uart2ClkSrcSelect.ClkSrc = 2;
    AmbaRTSL_PllGetUart2Clk();

    pAmbaRCT_Reg->Uart2ClkSrcSelect.ClkSrc = 3;
    AmbaRTSL_PllGetUart2Clk();

    printf("TEST_AmbaRTSL_PllGetUart2Clk\n");
}

void TEST_AmbaRTSL_PllGetUart3Clk(void)
{
    pAmbaRCT_Reg->Uart3ClkDivider.ClkDivider = 1;

    pAmbaRCT_Reg->Uart3ClkSrcSelect.ClkSrc = 0;
    AmbaRTSL_PllGetUart3Clk();

    pAmbaRCT_Reg->Uart3ClkSrcSelect.ClkSrc = 1;
    AmbaRTSL_PllGetUart3Clk();

    pAmbaRCT_Reg->Uart3ClkSrcSelect.ClkSrc = 2;
    AmbaRTSL_PllGetUart3Clk();

    pAmbaRCT_Reg->Uart3ClkSrcSelect.ClkSrc = 3;
    AmbaRTSL_PllGetUart3Clk();

    printf("TEST_AmbaRTSL_PllGetUart3Clk\n");
}

void TEST_AmbaRTSL_PllGetUsbPhyClk(void)
{
    AmbaRTSL_PllGetUsbPhyClk();

    printf("TEST_AmbaRTSL_PllGetUsbPhyClk\n");
}

void TEST_AmbaRTSL_PllGetVin0Clk(void)
{
    AmbaRTSL_PllGetVin0Clk();

    printf("TEST_AmbaRTSL_PllGetVin0Clk\n");
}

void TEST_AmbaRTSL_PllGetVin1Clk(void)
{
    pAmbaRCT_Reg->Vin1ClkSelect = 0;
    AmbaRTSL_PllGetVin1Clk();

    pAmbaRCT_Reg->Vin1ClkSelect = 1;
    AmbaRTSL_PllGetVin1Clk();

    pAmbaRCT_Reg->Vin1ClkSelect = 2;
    AmbaRTSL_PllGetVin1Clk();

    printf("TEST_AmbaRTSL_PllGetVin1Clk\n");
}

void TEST_AmbaRTSL_PllGetVisionClk(void)
{
    AmbaRTSL_PllGetVisionClk();

    printf("TEST_AmbaRTSL_PllGetVisionClk\n");
}

void TEST_AmbaRTSL_PllGetVout0Clk(void)
{
    pAmbaRCT_Reg->Vout0PllPostscaler.ClkDivider = 1;

    pAmbaRCT_Reg->VoutClkSelect.Vout0ClkSelect = 0x0U;
    AmbaRTSL_PllGetVout0Clk();

    pAmbaRCT_Reg->VoutClkSelect.Vout0ClkSelect = 0x1U;
    AmbaRTSL_PllGetVout0Clk();

    printf("TEST_AmbaRTSL_PllGetVout0Clk\n");
}

void TEST_AmbaRTSL_PllGetVout1Clk(void)
{
    pAmbaRCT_Reg->Vout1PllPostscaler.ClkDivider = 1;

    pAmbaRCT_Reg->VoutClkSelect.Vout1ClkSelect = 0x0U;
    AmbaRTSL_PllGetVout1Clk();

    pAmbaRCT_Reg->VoutClkSelect.Vout1ClkSelect = 0x1U;
    AmbaRTSL_PllGetVout1Clk();

    printf("TEST_AmbaRTSL_PllGetVout1Clk\n");
}

void TEST_AmbaRTSL_PllFineAdjFreq(void)
{
    UINT32 PllIdx = 0U;
    UINT32 Frequency = 0U;

    AMBA_PLL_EXT_CLK_CONFIG_s ExtClkConfig = {0};
    AMBA_PLL_EXT_CLK_CONFIG_s *pExtClkConfig = &ExtClkConfig;
    pExtClkConfig->ExtMuxLvdsSpclkClkFreq = 1U;
    AmbaRTSL_PllSetExtClkConfig(pExtClkConfig);
    AmbaRTSL_PllCalcPllOutFreq(PllIdx);

    AmbaRTSL_PllFineAdjFreq(PllIdx, Frequency);

    Frequency = 1U;
    AmbaRTSL_PllFineAdjFreq(PllIdx, Frequency);

    PllIdx = AMBA_PLL_CORE;
    AmbaRTSL_PllFineAdjFreq(PllIdx, Frequency);

    printf("TEST_AmbaRTSL_PllFineAdjFreq\n");
}

void TEST_AmbaRTSL_PllAdjFreq(void)
{
    UINT32 PllIdx = 0U;
    UINT32 Frequency = 1U;
    AmbaRTSL_PllAdjFreq(PllIdx, Frequency);

    Frequency = 48000000U;
    AmbaRTSL_PllAdjFreq(PllIdx, Frequency);

    Frequency = 48000001U;
    AmbaRTSL_PllAdjFreq(PllIdx, Frequency);

    Frequency = 0U;
    AmbaRTSL_PllAdjFreq(PllIdx, Frequency);

    printf("TEST_AmbaRTSL_PllAdjFreq\n");
}

void TEST_AmbaRTSL_PllCalcFreq(void)
{

    printf("TEST_AmbaRTSL_PllCalcFreq\n");
}

void TEST_AmbaRTSL_PllSetFreq(void)
{
    UINT32 PllIdx = AMBA_PLL_AUDIO;
    UINT32 Frequency = 1U;
    AmbaRTSL_PllSetFreq(PllIdx, Frequency);

    PllIdx = AMBA_PLL_VIDEO_A;
    AmbaRTSL_PllSetFreq(PllIdx, Frequency);

    PllIdx = AMBA_PLL_VIDEO_B;
    AmbaRTSL_PllSetFreq(PllIdx, Frequency);

    PllIdx = AMBA_PLL_CORE;
    AmbaRTSL_PllSetFreq(PllIdx, Frequency);

    printf("TEST_AmbaRTSL_PllSetFreq\n");
}

void TEST_AmbaRTSL_PllSetAudioClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetAudioClk(Frequency);

    printf("TEST_AmbaRTSL_PllSetAudioClk\n");
}

void TEST_AmbaRTSL_PllSetCanClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetCanClk(Frequency);

    pAmbaRCT_Reg->CanClkDivider.ClkDivider = 2U;
    AmbaRTSL_PllSetCanClk(Frequency);

    printf("TEST_AmbaRTSL_PllSetCanClk\n");
}

void TEST_AmbaRTSL_PllSetCoreClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetCoreClk(Frequency);

    printf("TEST_AmbaRTSL_PllSetCoreClk\n");
}

void TEST_AmbaRTSL_PllSetDebounceClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetDebounceClk(Frequency);

    pAmbaRCT_Reg->PlugInDetectClkDivider.ClkDivider = 0U;
    AmbaRTSL_PllSetDebounceClk(Frequency);

    printf("TEST_AmbaRTSL_PllSetDebounceClk\n");
}

void TEST_AmbaRTSL_PllSetEthernetClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetEthernetClk(Frequency);

    printf("TEST_AmbaRTSL_PllSetEthernetClk\n");
}

void TEST_AmbaRTSL_PllSetEthernet1Clk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetEthernet1Clk(Frequency);

    printf("TEST_AmbaRTSL_PllSetEthernet1Clk\n");
}

void TEST_AmbaRTSL_PllSetFexClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetFexClk(Frequency);

    Frequency = 1U;
    AmbaRTSL_PllSetFexClk(Frequency);

    printf("TEST_AmbaRTSL_PllSetFexClk\n");
}

void TEST_AmbaRTSL_PllSetNandClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetNandClk(Frequency);

    printf("TEST_AmbaRTSL_PllSetNandClk\n");
}

void TEST_AmbaRTSL_PllSetIdspClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetIdspClk(Frequency);

    printf("TEST_AmbaRTSL_PllSetIdspClk\n");
}

void TEST_AmbaRTSL_PllSetPwmClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetPwmClk(Frequency);

    pAmbaRCT_Reg->PwmClkDivider.ClkDivider = 2U;
    AmbaRTSL_PllSetPwmClk(Frequency);

    printf("TEST_AmbaRTSL_PllSetPwmClk\n");
}

void TEST_AmbaRTSL_PllSetSdPllOutClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetSdPllOutClk(Frequency);

    printf("TEST_AmbaRTSL_PllSetSdPllOutClk\n");
}

void TEST_AmbaRTSL_PllSetSd0Clk(void)
{
    UINT32 Frequency = 1U;
    AmbaRTSL_PllSetSd0Clk(Frequency);

    pAmbaRCT_Reg->Sd0ClkDivider.ClkDivider = 0U;
    AmbaRTSL_PllSetSd0Clk(Frequency);

    printf("TEST_AmbaRTSL_PllSetSd0Clk\n");
}

void TEST_AmbaRTSL_PllSetSd1Clk(void)
{
    UINT32 Frequency = 1U;
    AmbaRTSL_PllSetSd1Clk(Frequency);

    pAmbaRCT_Reg->Sd1ClkDivider.ClkDivider = 0U;
    AmbaRTSL_PllSetSd1Clk(Frequency);

    printf("TEST_AmbaRTSL_PllSetSd1Clk\n");
}

void TEST_AmbaRTSL_PllSetSensor0Clk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetSensor0Clk(Frequency);

    pAmbaRCT_Reg->SensorPllPostscaler.ClkDivider = 0x1U;
    pAmbaRCT_Reg->Vin0ClkDivider.ClkDivider = 0U;
    AmbaRTSL_PllSetSensor0Clk(Frequency);

    printf("TEST_AmbaRTSL_PllSetSensor0Clk\n");
}

void TEST_AmbaRTSL_PllSetSensor1Clk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetSensor1Clk(Frequency);

    pAmbaRCT_Reg->Sensor2PllPostscaler.ClkDivider = 0x1U;
    pAmbaRCT_Reg->Vin1ClkDivider.ClkDivider = 0U;
    AmbaRTSL_PllSetSensor1Clk(Frequency);

    printf("TEST_AmbaRTSL_PllSetSensor1Clk\n");
}

void TEST_AmbaRTSL_PllSetSensor2Clk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetSensor2Clk(Frequency);

    printf("TEST_AmbaRTSL_PllSetSensor2Clk\n");
}

void TEST_AmbaRTSL_PllSetSpiMasterClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetSpiMasterClk(Frequency);

    Frequency = 1U;
    AmbaRTSL_PllSetSpiMasterClk(Frequency);

    printf("TEST_AmbaRTSL_PllSetSpiMasterClk\n");
}

void TEST_AmbaRTSL_PllSetSpiSlaveClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetSpiSlaveClk(Frequency);

    Frequency = 1U;
    AmbaRTSL_PllSetSpiSlaveClk(Frequency);

    Frequency = 312500001U;
    AmbaRTSL_PllSetSpiSlaveClk(Frequency);

    printf("TEST_AmbaRTSL_PllSetSpiSlaveClk\n");
}

void TEST_AmbaRTSL_PllSetSpiNorClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetSpiNorClk(Frequency);

    Frequency = 1U;
    AmbaRTSL_PllSetSpiNorClk(Frequency);

    printf("TEST_AmbaRTSL_PllSetSpiNorClk\n");
}

void TEST_AmbaRTSL_PllSetUartApbClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetUartApbClk(Frequency);

    printf("TEST_AmbaRTSL_PllSetUartApbClk\n");
}

void TEST_AmbaRTSL_PllSetUart0Clk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetUart0Clk(Frequency);

    printf("TEST_AmbaRTSL_PllSetUart0Clk\n");
}

void TEST_AmbaRTSL_PllSetUart1Clk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetUart1Clk(Frequency);

    printf("TEST_AmbaRTSL_PllSetUart1Clk\n");
}

void TEST_AmbaRTSL_PllSetUart2Clk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetUart2Clk(Frequency);

    printf("TEST_AmbaRTSL_PllSetUart2Clk\n");
}

void TEST_AmbaRTSL_PllSetUart3Clk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetUart3Clk(Frequency);

    printf("TEST_AmbaRTSL_PllSetUart3Clk\n");
}

void TEST_AmbaRTSL_PllSetVout0Clk(void)
{
    UINT32 Frequency = 0U;
    UINT32 Type = AMBA_NUM_PLL_VOUT_TYPE - 1U;
    UINT32 Ratio = AMBA_NUM_PLL_VOUT_TYPE;
    AmbaRTSL_PllSetVout0Clk(Frequency);

    Ratio = AMBA_PLL_VOUT_FPD_LINK;
    AmbaRTSL_PllSetVout0ClkHint(Type, Ratio);
    AmbaRTSL_PllSetVout0Clk(Frequency);

    Ratio = AMBA_PLL_VOUT_MIPI_CSI;
    AmbaRTSL_PllSetVout0ClkHint(Type, Ratio);
    AmbaRTSL_PllSetVout0Clk(Frequency);

    Ratio = AMBA_PLL_VOUT_MIPI_DSI;
    AmbaRTSL_PllSetVout0ClkHint(Type, Ratio);
    AmbaRTSL_PllSetVout0Clk(Frequency);

    printf("TEST_AmbaRTSL_PllSetVout0Clk\n");
}

void TEST_AmbaRTSL_PllSetVout1Clk(void)
{
    UINT32 Frequency = 0U;
    UINT32 Type = AMBA_NUM_PLL_VOUT_TYPE - 1U;
    UINT32 Ratio = AMBA_NUM_PLL_VOUT_TYPE;
    AmbaRTSL_PllSetVout1Clk(Frequency);

    Ratio = AMBA_PLL_VOUT_FPD_LINK;
    AmbaRTSL_PllSetVout1ClkHint(Type, Ratio);
    AmbaRTSL_PllSetVout1Clk(Frequency);

    Ratio = AMBA_PLL_VOUT_MIPI_CSI;
    AmbaRTSL_PllSetVout1ClkHint(Type, Ratio);
    AmbaRTSL_PllSetVout1Clk(Frequency);

    Ratio = AMBA_PLL_VOUT_MIPI_DSI;
    AmbaRTSL_PllSetVout1ClkHint(Type, Ratio);
    AmbaRTSL_PllSetVout1Clk(Frequency);

    printf("TEST_AmbaRTSL_PllSetVout1Clk\n");
}

void TEST_AmbaRTSL_PllSetVout0ClkHint(void)
{
    UINT32 Type = AMBA_NUM_PLL_VOUT_TYPE - 1U;
    UINT32 Ratio = 1U;
    AmbaRTSL_PllSetVout0ClkHint(Type, Ratio);

    Ratio = 0U;
    AmbaRTSL_PllSetVout0ClkHint(Type, Ratio);

    Type = AMBA_NUM_PLL_VOUT_TYPE;
    AmbaRTSL_PllSetVout0ClkHint(Type, Ratio);

    printf("TEST_AmbaRTSL_PllSetVout0ClkHint\n");
}

void TEST_AmbaRTSL_PllSetVout1ClkHint(void)
{
    UINT32 Type = AMBA_NUM_PLL_VOUT_TYPE - 1U;
    UINT32 Ratio = 1U;
    AmbaRTSL_PllSetVout1ClkHint(Type, Ratio);

    Ratio = 0U;
    AmbaRTSL_PllSetVout1ClkHint(Type, Ratio);

    Type = AMBA_NUM_PLL_VOUT_TYPE;
    AmbaRTSL_PllSetVout1ClkHint(Type, Ratio);

    printf("TEST_AmbaRTSL_PllSetVout1ClkHint\n");
}

void TEST_AmbaRTSL_PllSetVisionClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllSetVisionClk(Frequency);

    printf("TEST_AmbaRTSL_PllSetVisionClk\n");
}

void TEST_AmbaRTSL_PllFineAdjAudioClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllFineAdjAudioClk(Frequency);

    printf("TEST_AmbaRTSL_PllFineAdjAudioClk\n");
}

void TEST_AmbaRTSL_PllFineAdjSensor0Clk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllFineAdjSensor0Clk(Frequency);

    printf("TEST_AmbaRTSL_PllFineAdjSensor0Clk\n");
}

void TEST_AmbaRTSL_PllFineAdjSensor1Clk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllFineAdjSensor1Clk(Frequency);

    printf("TEST_AmbaRTSL_PllFineAdjSensor1Clk\n");
}

void TEST_AmbaRTSL_PllFineAdjEthernetClk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllFineAdjEthernetClk(Frequency);

    printf("TEST_AmbaRTSL_PllFineAdjEthernetClk\n");
}

void TEST_AmbaRTSL_PllFineAdjVout0Clk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllFineAdjVout0Clk(Frequency);

    printf("TEST_AmbaRTSL_PllFineAdjVout0Clk\n");
}

void TEST_AmbaRTSL_PllFineAdjVout1Clk(void)
{
    UINT32 Frequency = 0U;
    AmbaRTSL_PllFineAdjVout1Clk(Frequency);

    printf("TEST_AmbaRTSL_PllFineAdjVout1Clk\n");
}


int main(void)
{
    /* AmbaRTSL_PLL.c */
    TEST_AmbaRTSL_PllInit();
    TEST_AmbaRTSL_PllNotifyFreqChanges();
    TEST_AmbaRTSL_PllSetExtClkConfig();
    TEST_AmbaRTSL_PllGetExtClkConfig();
    TEST_AmbaRTSL_PllGetClkRefFreq();
    TEST_AmbaRTSL_PllGetPllRefFreq();
    TEST_AmbaRTSL_PllSetPllOutFreq();
    TEST_AmbaRTSL_PllGetPllOutFreq();

    /* AmbaRTSL_PLL_Config.c */
#if defined(CONFIG_SOC_CV2)
    TEST_AmbaRTSL_PllSetAdcClkConfig();
    TEST_AmbaRTSL_PllSetAudioClkConfig();
    TEST_AmbaRTSL_PllSetAudio1ClkConfig();
    TEST_AmbaRTSL_PllSetMipiDsiClkConfig();
    TEST_AmbaRTSL_PllSetSensor1ClkDir();
    TEST_AmbaRTSL_PllSetSpiClkConfig();
    TEST_AmbaRTSL_PllSetVoutLcdClkConfig();
    TEST_AmbaRTSL_PllSetVoutTvClkConfig();
#endif

    TEST_AmbaRTSL_PllSetCanClkConfig();
    TEST_AmbaRTSL_PllSetPwmClkConfig();
    TEST_AmbaRTSL_PllSetSd0ScalerClkSrc();
    TEST_AmbaRTSL_PllSetSd1ScalerClkSrc();
    TEST_AmbaRTSL_PllSetSensor0ClkDir();
    TEST_AmbaRTSL_PllSetUartApbClkConfig();
    TEST_AmbaRTSL_PllSetUart0ClkConfig();
    TEST_AmbaRTSL_PllSetUart1ClkConfig();
    TEST_AmbaRTSL_PllSetUart2ClkConfig();
    TEST_AmbaRTSL_PllSetUart3ClkConfig();
    TEST_AmbaRTSL_PllSetVout0ClkConfig();
    TEST_AmbaRTSL_PllSetVout1ClkConfig();
    TEST_AmbaRTSL_PllSetVin1ClkConfig();
    TEST_AmbaRTSL_PllSetEnetClkConfig();
    TEST_AmbaRTSL_PllClkEnable();
    TEST_AmbaRTSL_PllClkDisable();
    TEST_AmbaRTSL_PllClkCheck();
    TEST_AmbaRTSL_PllVdspClkDisable();
    TEST_AmbaRTSL_PllIdspClkDisable();
    TEST_AmbaRTSL_RctTimerWait();
    TEST_AmbaRTSL_PllSetNumCpuCycleUs();
    TEST_AmbaRTSL_PllSetNumCpuCycleMs();
    TEST_AmbaRTSL_PllGetNumCpuCycleUs();
    TEST_AmbaRTSL_PllGetNumCpuCycleMs();

    /* AmbaRTSL_PLL_GetClkFreq.c */
    TEST_AmbaRTSL_PllGetAhbClk();
    TEST_AmbaRTSL_PllGetApbClk();
    TEST_AmbaRTSL_PllGetAxiClk();
    TEST_AmbaRTSL_PllGetAudioClk();
    TEST_AmbaRTSL_PllGetCanClk();
    TEST_AmbaRTSL_PllGetCoreClk();
    TEST_AmbaRTSL_PllGetCortex0Clk();
    TEST_AmbaRTSL_PllGetCortex1Clk();
    TEST_AmbaRTSL_PllGetDbgClk();
    TEST_AmbaRTSL_PllGetDebounceClk();
    TEST_AmbaRTSL_PllGetDramClk();
    TEST_AmbaRTSL_PllGetEthernetClk();
    TEST_AmbaRTSL_PllGetEthernet1Clk();
    TEST_AmbaRTSL_PllGetFexClk();
    TEST_AmbaRTSL_PllGetIdspClk();
    TEST_AmbaRTSL_PllGetMipiDsi0Clk();
    TEST_AmbaRTSL_PllGetMipiDsi1Clk();
    TEST_AmbaRTSL_PllGetNandClk();
    TEST_AmbaRTSL_PllGetPwmClk();
    TEST_AmbaRTSL_PllGetSd0Clk();
    TEST_AmbaRTSL_PllGetSd1Clk();
    TEST_AmbaRTSL_PllGetSensor0Clk();
    TEST_AmbaRTSL_PllGetSensor1Clk();
    TEST_AmbaRTSL_PllGetSensor2Clk();
    TEST_AmbaRTSL_PllGetSpiMasterClk();
    TEST_AmbaRTSL_PllGetSpiSlaveClk();
    TEST_AmbaRTSL_PllGetSpiNorClk();
    TEST_AmbaRTSL_PllGetUartApbClk();
    TEST_AmbaRTSL_PllGetUart0Clk();
    TEST_AmbaRTSL_PllGetUart1Clk();
    TEST_AmbaRTSL_PllGetUart2Clk();
    TEST_AmbaRTSL_PllGetUart3Clk();
    TEST_AmbaRTSL_PllGetUsbPhyClk();
    TEST_AmbaRTSL_PllGetVin0Clk();
    TEST_AmbaRTSL_PllGetVin1Clk();
    TEST_AmbaRTSL_PllGetVisionClk();
    TEST_AmbaRTSL_PllGetVout0Clk();
    TEST_AmbaRTSL_PllGetVout1Clk();

    /* AmbaRTSL_PLL_SetClkFreq.c */
    TEST_AmbaRTSL_PllFineAdjFreq();
    TEST_AmbaRTSL_PllAdjFreq();
    TEST_AmbaRTSL_PllCalcFreq();
    TEST_AmbaRTSL_PllSetFreq();
    TEST_AmbaRTSL_PllSetAudioClk();
    TEST_AmbaRTSL_PllSetCanClk();
    TEST_AmbaRTSL_PllSetCoreClk();
    TEST_AmbaRTSL_PllSetDebounceClk();
    TEST_AmbaRTSL_PllSetEthernetClk();
    TEST_AmbaRTSL_PllSetEthernet1Clk();
    TEST_AmbaRTSL_PllSetFexClk();
    TEST_AmbaRTSL_PllSetNandClk();
    TEST_AmbaRTSL_PllSetIdspClk();
    TEST_AmbaRTSL_PllSetPwmClk();
    TEST_AmbaRTSL_PllSetSdPllOutClk();
    TEST_AmbaRTSL_PllSetSd0Clk();
    TEST_AmbaRTSL_PllSetSd1Clk();
    TEST_AmbaRTSL_PllSetSensor0Clk();
    TEST_AmbaRTSL_PllSetSensor1Clk();
    TEST_AmbaRTSL_PllSetSensor2Clk();
    TEST_AmbaRTSL_PllSetSpiMasterClk();
    TEST_AmbaRTSL_PllSetSpiSlaveClk();
    TEST_AmbaRTSL_PllSetSpiNorClk();
    TEST_AmbaRTSL_PllSetUartApbClk();
    TEST_AmbaRTSL_PllSetUart0Clk();
    TEST_AmbaRTSL_PllSetUart1Clk();
    TEST_AmbaRTSL_PllSetUart2Clk();
    TEST_AmbaRTSL_PllSetUart3Clk();
    TEST_AmbaRTSL_PllSetVout0Clk();
    TEST_AmbaRTSL_PllSetVout1Clk();
    TEST_AmbaRTSL_PllSetVout0ClkHint();
    TEST_AmbaRTSL_PllSetVout1ClkHint();
    TEST_AmbaRTSL_PllSetVisionClk();
    TEST_AmbaRTSL_PllFineAdjAudioClk();
    TEST_AmbaRTSL_PllFineAdjSensor0Clk();
    TEST_AmbaRTSL_PllFineAdjSensor1Clk();
    TEST_AmbaRTSL_PllFineAdjEthernetClk();
    TEST_AmbaRTSL_PllFineAdjVout0Clk();
    TEST_AmbaRTSL_PllFineAdjVout1Clk();

    return 0;
}


