#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaRTSL_PLL.h"

static UINT32 RetVal_PllGetVin0Clk          = 0;
static UINT32 RetVal_PllGetVin1Clk          = 0;
static UINT32 RetVal_PllGetIdspClk          = 0;
static UINT32 RetVal_PllGetApbClk           = 0;
static UINT32 RetVal_PllGetUartApbClk       = 0;
static UINT32 RetVal_PllGetUart0Clk         = 0;
static UINT32 RetVal_PllGetUart1Clk         = 0;
static UINT32 RetVal_PllGetUart2Clk         = 0;
static UINT32 RetVal_PllGetUart3Clk         = 0;
static UINT32 RetVal_PllGetClkRefFreq       = 0;
static UINT32 RetVal_PllGetSpiMasterClk     = 0;
static UINT32 RetVal_PllGetAdcClk           = 0;
static UINT32 RetVal_PllGetNumCpuCycleUs    = 0;
static UINT32 RetVal_PllGetPwmClk           = 0;
static UINT32 RetVal_PllGetExtClkConfig     = 0;
static UINT32 RetVal_PllGetCoreClk          = 0;
static UINT32 RetVal_PllGetVisionClk        = 0;
static UINT32 RetVal_PllGetSensor0Clk       = 0;
static UINT32 RetVal_PllGetSensor1Clk       = 0;
static UINT32 RetVal_PllGetEthernetClk      = 0;
static UINT32 RetVal_PllGetFexClk           = 0;
static UINT32 RetVal_PllGetCortex1Clk       = 0;
static UINT32 RetVal_PllGetDramClk          = 0;

static UINT32 RetVal_PllSetUartApbClk       = 0;
static UINT32 RetVal_PllSetUart0Clk         = 0;
static UINT32 RetVal_PllSetUart1Clk         = 0;
static UINT32 RetVal_PllSetUart2Clk         = 0;
static UINT32 RetVal_PllSetUart3Clk         = 0;
static UINT32 RetVal_PllSetUartApbClkConfig = 0;
static UINT32 RetVal_PllSetUart0ClkConfig   = 0;
static UINT32 RetVal_PllSetUart1ClkConfig   = 0;
static UINT32 RetVal_PllSetUart2ClkConfig   = 0;
static UINT32 RetVal_PllSetUart3ClkConfig   = 0;
static UINT32 RetVal_PllSetSpiMasterClk     = 0;
static UINT32 RetVal_PllSetSpiSlaveClk      = 0;
static UINT32 RetVal_PllSetAdcClk           = 0;
static UINT32 RetVal_PllSetVoutTvClkHint    = 0;
static UINT32 RetVal_PllSetVoutTvClk        = 0;
static UINT32 RetVal_PllSetPwmClkConfig     = 0;
static UINT32 RetVal_PllSetPwmClk           = 0;
static UINT32 RetVal_PllSetExtClkConfig     = 0;
static UINT32 RetVal_PllSetCoreClk          = 0;
static UINT32 RetVal_PllSetIdspClk          = 0;
static UINT32 RetVal_PllSetVisionClk        = 0;
static UINT32 RetVal_PllFineAdjAudioClk     = 0;
static UINT32 RetVal_PllFineAdjSensor0Clk   = 0;
static UINT32 RetVal_PllFineAdjSensor1Clk   = 0;
static UINT32 RetVal_PllFineAdjEthernetClk  = 0;
static UINT32 RetVal_PllFineAdjVout0Clk     = 0;
static UINT32 RetVal_PllFineAdjVout1Clk     = 0;
static UINT32 RetVal_PllSetFexClk           = 0;
static UINT32 RetVal_PllSetSd0Clk           = 0;

static UINT32 RetVal_PllSetSensor0Clk       = ERR_NONE;
static UINT32 RetVal_PllSetSensor1Clk       = ERR_NONE;
static UINT32 RetVal_PllSetEthernetClk      = ERR_NONE;
static UINT32 RetVal_PllSetVin1ClkConfig    = ERR_NONE;
static UINT32 RetVal_PllSetEnetClkConfig    = ERR_NONE;
static UINT32 RetVal_PllSetSensor0ClkDir    = ERR_NONE;

void Set_RetVal_PllGetVin0Clk(UINT32 RetVal)
{
    RetVal_PllGetVin0Clk = RetVal;
}
void Set_RetVal_PllGetVin1Clk(UINT32 RetVal)
{
    RetVal_PllGetVin1Clk = RetVal;
}
void Set_RetVal_PllGetIdspClk(UINT32 RetVal)
{
    RetVal_PllGetIdspClk = RetVal;
}
void Set_RetVal_PllSetSensor0Clk(UINT32 RetVal)
{
    RetVal_PllSetSensor0Clk = RetVal;
}
void Set_RetVal_PllSetSensor1Clk(UINT32 RetVal)
{
    RetVal_PllSetSensor1Clk = RetVal;
}
void Set_RetVal_PllSetEthernetClk(UINT32 RetVal)
{
    RetVal_PllSetEthernetClk = RetVal;
}
void Set_RetVal_PllSetVin1ClkConfig(UINT32 RetVal)
{
    RetVal_PllSetVin1ClkConfig = RetVal;
}
void Set_RetVal_PllSetEnetClkConfig(UINT32 RetVal)
{
    RetVal_PllSetEnetClkConfig = RetVal;
}
void Set_RetVal_PllSetSensor0ClkDir(UINT32 RetVal)
{
    RetVal_PllSetSensor0ClkDir = RetVal;
}
void Set_RetVal_PllGetApbClk(UINT32 RetVal)
{
    RetVal_PllGetApbClk = RetVal;
}
void Set_RetVal_PllGetAdcClk(UINT32 RetVal)
{
    RetVal_PllGetAdcClk = RetVal;
}
void Set_RetVal_PllGetPwmClk(UINT32 RetVal)
{
    RetVal_PllGetPwmClk = RetVal;
}


UINT32 AmbaRTSL_PllGetVin0Clk(void)
{
    return RetVal_PllGetVin0Clk;
}

UINT32 AmbaRTSL_PllGetVin1Clk(void)
{
    return RetVal_PllGetVin1Clk;
}

UINT32 AmbaRTSL_PllGetIdspClk(void)
{
    return RetVal_PllGetIdspClk;
}

UINT32 AmbaRTSL_PllSetSensor0Clk(UINT32 Frequency)
{
    return RetVal_PllSetSensor0Clk;
}

UINT32 AmbaRTSL_PllSetSensor1Clk(UINT32 Frequency)
{
    return RetVal_PllSetSensor1Clk;
}

UINT32 AmbaRTSL_PllSetEthernetClk(UINT32 Frequency)
{
    return RetVal_PllSetEthernetClk;
}

UINT32 AmbaRTSL_PllSetVin1ClkConfig(UINT32 Vin1ClkConfig)
{
    return RetVal_PllSetVin1ClkConfig;
}

UINT32 AmbaRTSL_PllSetEnetClkConfig(UINT32 EnetClkConfig)
{
    return RetVal_PllSetEnetClkConfig;
}

UINT32 AmbaRTSL_PllSetSensor0ClkDir(UINT32 ClkSiDir)
{
    return RetVal_PllSetSensor0ClkDir;
}

UINT32 AmbaRTSL_PllGetVout0Clk(void)
{
    UINT32 Frequency;

    return Frequency;
}

UINT32 AmbaRTSL_PllGetVout1Clk(void)
{
    UINT32 Frequency;

    return Frequency;
}

UINT32 AmbaRTSL_PllSetVout0ClkHint(UINT32 Type, UINT32 Ratio)
{
    UINT32 RetVal = ERR_NONE;

    return RetVal;
}

UINT32 AmbaRTSL_PllSetVout1ClkHint(UINT32 Type, UINT32 Ratio)
{
    UINT32 RetVal = ERR_NONE;

    return RetVal;
}

UINT32 AmbaRTSL_PllSetVout0Clk(UINT32 Frequency)
{
    return ERR_NONE;
}

UINT32 AmbaRTSL_PllSetVout1Clk(UINT32 Frequency)
{
    return ERR_NONE;
}

UINT32 AmbaRTSL_PllGetNumCpuCycleMs(void)
{
    return ERR_NONE;
}

UINT32 AmbaRTSL_PllSetVoutTvClkHint(UINT32 Type, UINT32 Ratio)
{

    return RetVal_PllSetVoutTvClkHint;
}

UINT32 AmbaRTSL_PllSetVoutTvClk(UINT32 Frequency)
{

    return RetVal_PllSetVoutTvClk;
}

UINT32 AmbaRTSL_PllGetVoutTvClk(void)
{
    UINT32 Frequency;

    return Frequency;
}

UINT32 AmbaRTSL_PllGetAudioClk(void)
{
    UINT32 Frequency;

    return Frequency;
}


UINT32 AmbaRTSL_PllGetAudio1Clk(void)
{
    UINT32 Frequency;

    return Frequency;
}

UINT32 AmbaRTSL_PllGetAhbClk(void)
{
    return 1U;
}

UINT32 AmbaRTSL_PllGetApbClk(void)
{
    return RetVal_PllGetApbClk;
}

UINT32 AmbaRTSL_PllGetUartApbClk(void)
{
    return RetVal_PllGetUartApbClk;
}

UINT32 AmbaRTSL_PllGetUart0Clk(void)
{
    return RetVal_PllGetUart0Clk;
}

UINT32 AmbaRTSL_PllGetUart1Clk(void)
{
    return RetVal_PllGetUart1Clk;
}

UINT32 AmbaRTSL_PllGetUart2Clk(void)
{
    return RetVal_PllGetUart2Clk;
}

UINT32 AmbaRTSL_PllGetUart3Clk(void)
{
    return RetVal_PllGetUart3Clk;
}

UINT32 AmbaRTSL_PllGetClkRefFreq(void)
{
    return RetVal_PllGetClkRefFreq;
}

UINT32 AmbaRTSL_PllSetUartApbClk(UINT32 Frequency)
{
    return RetVal_PllSetUartApbClk;
}

UINT32 AmbaRTSL_PllSetUart0Clk(UINT32 Frequency)
{
    return RetVal_PllSetUart0Clk;
}

UINT32 AmbaRTSL_PllSetUart1Clk(UINT32 Frequency)
{
    return RetVal_PllSetUart1Clk;
}

UINT32 AmbaRTSL_PllSetUart2Clk(UINT32 Frequency)
{
    return RetVal_PllSetUart2Clk;
}

UINT32 AmbaRTSL_PllSetUart3Clk(UINT32 Frequency)
{
    return RetVal_PllSetUart3Clk;
}

UINT32 AmbaRTSL_PllSetUartApbClkConfig(UINT32 UartClkConfig)
{
    return RetVal_PllSetUartApbClkConfig;
}

UINT32 AmbaRTSL_PllSetUart0ClkConfig(UINT32 UartClkConfig)
{
    return RetVal_PllSetUart0ClkConfig;
}

UINT32 AmbaRTSL_PllSetUart1ClkConfig(UINT32 UartClkConfig)
{
    return RetVal_PllSetUart1ClkConfig;
}

UINT32 AmbaRTSL_PllSetUart2ClkConfig(UINT32 UartClkConfig)
{
    return RetVal_PllSetUart2ClkConfig;
}

UINT32 AmbaRTSL_PllSetUart3ClkConfig(UINT32 UartClkConfig)
{
    return RetVal_PllSetUart3ClkConfig;
}

UINT32 AmbaRTSL_PllGetSpiMasterClk(void)
{
    return RetVal_PllGetSpiMasterClk;
}

UINT32 AmbaRTSL_PllSetSpiSlaveClk(UINT32 Frequency)
{
    return RetVal_PllSetSpiSlaveClk;
}

UINT32 AmbaRTSL_PllSetSpiMasterClk(UINT32 Frequency)
{
    return RetVal_PllSetSpiMasterClk;
}

UINT32 AmbaRTSL_PllGetAdcClk(void)
{
    return RetVal_PllGetAdcClk;
}

UINT32 AmbaRTSL_PllSetAdcClk(UINT32 Frequency)
{
    return RetVal_PllSetAdcClk;
}

UINT32 AmbaRTSL_PllGetNumCpuCycleUs(void)
{
    return RetVal_PllGetNumCpuCycleUs;
}

UINT32 AmbaRTSL_PllSetPwmClkConfig(UINT32 PwmClkConfig)
{
    return RetVal_PllSetPwmClkConfig;
}

UINT32 AmbaRTSL_PllSetPwmClk(UINT32 Frequency)
{
    return RetVal_PllSetPwmClk;
}

UINT32 AmbaRTSL_PllGetPwmClk(void)
{
    return RetVal_PllGetPwmClk;
}

UINT32 AmbaRTSL_PllSetExtClkConfig(const AMBA_PLL_EXT_CLK_CONFIG_s *pExtClkConfig)
{
    return RetVal_PllSetExtClkConfig;
}

UINT32 AmbaRTSL_PllGetExtClkConfig(AMBA_PLL_EXT_CLK_CONFIG_s *pExtClkConfig)
{
    return RetVal_PllGetExtClkConfig;
}

UINT32 AmbaRTSL_PllSetCoreClk(UINT32 Frequency)
{
    return RetVal_PllSetCoreClk;
}

UINT32 AmbaRTSL_PllGetCoreClk(void)
{
    return RetVal_PllGetCoreClk;
}

UINT32 AmbaRTSL_PllSetIdspClk(UINT32 Frequency)
{
    return RetVal_PllSetIdspClk;
}

UINT32 AmbaRTSL_PllSetVisionClk(UINT32 Frequency)
{
    return RetVal_PllSetVisionClk;
}

UINT32 AmbaRTSL_PllGetVisionClk(void)
{
    return RetVal_PllGetVisionClk;
}

UINT32 AmbaRTSL_PllFineAdjAudioClk(UINT32 Frequency)
{
    return RetVal_PllFineAdjAudioClk;
}

UINT32 AmbaRTSL_PllFineAdjSensor0Clk(UINT32 Frequency)
{
    return RetVal_PllFineAdjSensor0Clk;
}

UINT32 AmbaRTSL_PllFineAdjSensor1Clk(UINT32 Frequency)
{
    return RetVal_PllFineAdjSensor1Clk;
}

UINT32 AmbaRTSL_PllGetSensor0Clk(void)
{
    return RetVal_PllGetSensor0Clk;
}

UINT32 AmbaRTSL_PllGetSensor1Clk(void)
{
    return RetVal_PllGetSensor1Clk;
}

UINT32 AmbaRTSL_PllFineAdjEthernetClk(UINT32 Frequency)
{
    return RetVal_PllFineAdjEthernetClk;
}

UINT32 AmbaRTSL_PllGetEthernetClk(void)
{
    return RetVal_PllGetEthernetClk;
}

UINT32 AmbaRTSL_PllFineAdjVout0Clk(UINT32 Frequency)
{
    return RetVal_PllFineAdjVout0Clk;
}

UINT32 AmbaRTSL_PllFineAdjVout1Clk(UINT32 Frequency)
{
    return RetVal_PllFineAdjVout1Clk;
}

UINT32 AmbaRTSL_PllSetFexClk(UINT32 Frequency)
{
    return RetVal_PllSetFexClk;
}

UINT32 AmbaRTSL_PllGetFexClk(void)
{
    return RetVal_PllGetFexClk;
}

UINT32 AmbaRTSL_PllGetCortex1Clk(void)
{
    return RetVal_PllGetCortex1Clk;
}

UINT32 AmbaRTSL_PllGetDramClk(void)
{
    return RetVal_PllGetDramClk;
}

UINT32 AmbaRTSL_PllSetSd0Clk(UINT32 Frequency)
{
    return RetVal_PllSetSd0Clk;
}

void AmbaRTSL_PllClkEnable(UINT32 SysFeature)
{
}

void AmbaRTSL_PllClkDisable(UINT32 SysFeature)
{
}

void AmbaRTSL_PllClkCheck(UINT32 SysFeature, UINT32 *pEnable)
{
}

