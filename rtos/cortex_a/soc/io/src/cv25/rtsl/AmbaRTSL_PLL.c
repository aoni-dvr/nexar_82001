/**
 *  @file AmbaRTSL_PLL.c
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

#include "AmbaRTSL_DRAMC.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_TMR.h"
#include "AmbaRTSL_WDT.h"

#include "AmbaCSL_PLL.h"

typedef struct {
    UINT32  ClkRefFreq;                             /* CLK_REF: Clock Frequency (Hz) */
    UINT32  PllRefFreq[AMBA_NUM_PLL];               /* Reference Clock Frequency (Hz) */
    AMBA_PLL_EXT_CLK_CONFIG_s ExtClkConfig;         /* External Clock Configurations */
} AMBA_PLL_CLK_SOURCE_CTRL_s;

typedef struct {
    ctrl_id_t id;
    UINT32 (*SetFunc)(UINT32 Frequency); /* pointer to the AmbaRTSL_PllSetXXX */
    UINT32 (*GetFunc)(void); /* pointer to the AmbaRTSL_PllGetXXX */
    UINT32 (*ConfigFunc)(UINT32 ClkConfig); /* pointer to the AmbaRTSL_PllSetXXXClkConfig */
} AMBA_PLL_CALLBACK_s;

static AMBA_PLL_CLK_SOURCE_CTRL_s AmbaPllSrcClks = {
    .ClkRefFreq = 24000000U,                        /* CLK_REF: Clock Frequency (Hz) */

    /* PLL Reference Clock Frequency (Hz) */
    .PllRefFreq = {
        [AMBA_PLL_AUDIO]     = 24000000U,    /* default = CLK_REF */
        [AMBA_PLL_CORE]      = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_CORTEX]    = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_DDR]       = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_ENET]      = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_HDMI]      = 24000000U,    /* default = output of HDMI prescaler */
        [AMBA_PLL_IDSP]      = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_NAND]      = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_SD]        = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_SENSOR0]   = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_VIDEO2]    = 24000000U,    /* default = CLK_REF */
        [AMBA_PLL_VISION]    = 24000000U,    /* always = CLK_REF */
    },

    .ExtClkConfig = {                               /* External Clock Configurations */
        .ExtAudioClkFreq = 0U,
        .ExtSensorInputClkFreq = 0U,
        .ExtMuxLvdsSpclkClkFreq = 0U,
        .ExtEthernetClkFreq = 0U,
        .SpclkMuxSelect = 0U
    }
};

static AMBA_PLL_CALLBACK_s PLL_CB[AMBA_CLK_NUM] = {
    { AMBA_CLK_ADC, AmbaRTSL_PllSetAdcClk, AmbaRTSL_PllGetAdcClk, AmbaRTSL_PllSetAdcClkConfig},
    { AMBA_CLK_AHB, NULL, AmbaRTSL_PllGetAhbClk, NULL},
    { AMBA_CLK_APB, NULL, AmbaRTSL_PllGetApbClk, NULL},
    { AMBA_CLK_AXI, NULL, AmbaRTSL_PllGetAxiClk, NULL},
    { AMBA_CLK_AUDIO, AmbaRTSL_PllSetAudioClk, AmbaRTSL_PllGetAudioClk, AmbaRTSL_PllSetAudioClkConfig},
    { AMBA_CLK_CAN, AmbaRTSL_PllSetCanClk, AmbaRTSL_PllGetCanClk, AmbaRTSL_PllSetCanClkConfig},
    { AMBA_CLK_CORE, AmbaRTSL_PllSetCoreClk, AmbaRTSL_PllGetCoreClk, NULL},
    { AMBA_CLK_CORTEX, NULL, AmbaRTSL_PllGetCortexClk, NULL},
    { AMBA_CLK_DEBUGPORT, NULL, AmbaRTSL_PllGetDbgClk, NULL},
    { AMBA_CLK_DEBOUNCE, AmbaRTSL_PllSetDebounceClk, AmbaRTSL_PllGetDebounceClk, NULL},
    { AMBA_CLK_DRAM, NULL, AmbaRTSL_PllGetDramClk, NULL},
    { AMBA_CLK_ENET, AmbaRTSL_PllSetEthernetClk, AmbaRTSL_PllGetEthernetClk, NULL},
    { AMBA_CLK_GPIODEBOUNCE, NULL, AmbaRTSL_PllGetGpioDebounceClk, NULL},
    { AMBA_CLK_IDSP, AmbaRTSL_PllSetIdspClk, AmbaRTSL_PllGetIdspClk, NULL},
    { AMBA_CLK_IR, AmbaRTSL_PllSetIrClk, AmbaRTSL_PllGetIrClk, NULL},
    { AMBA_CLK_MIPIDSI, NULL, AmbaRTSL_PllGetMipiDsiClk, AmbaRTSL_PllSetMipiDsiClkConfig},
    { AMBA_CLK_NAND, AmbaRTSL_PllSetNandClk, AmbaRTSL_PllGetNandClk, NULL},
    { AMBA_CLK_PWM, AmbaRTSL_PllSetPwmClk, AmbaRTSL_PllGetPwmClk, AmbaRTSL_PllSetPwmClkConfig},
    { AMBA_CLK_SD0, AmbaRTSL_PllSetSd0Clk, AmbaRTSL_PllGetSd0Clk, AmbaRTSL_PllSetSd0ScalerClkSrc},
    { AMBA_CLK_SD1, AmbaRTSL_PllSetSd1Clk, AmbaRTSL_PllGetSd1Clk, AmbaRTSL_PllSetSd1ScalerClkSrc},
    { AMBA_CLK_SENSOR0, AmbaRTSL_PllSetSensor0Clk, AmbaRTSL_PllGetSensor0Clk, AmbaRTSL_PllSetSensor0ClkDir},
    { AMBA_CLK_SPIMASTER, AmbaRTSL_PllSetSpiMasterClk, AmbaRTSL_PllGetSpiMasterClk, AmbaRTSL_PllSetSpiClkConfig},
    { AMBA_CLK_SPISLAVE, AmbaRTSL_PllSetSpiSlaveClk, AmbaRTSL_PllGetSpiSlaveClk, NULL},
    { AMBA_CLK_SPINOR, AmbaRTSL_PllSetSpiNorClk, AmbaRTSL_PllGetSpiNorClk, NULL},
    { AMBA_CLK_UARTAPB, AmbaRTSL_PllSetUartApbClk, AmbaRTSL_PllGetUartApbClk, AmbaRTSL_PllSetUartApbClkConfig},
    { AMBA_CLK_UART0, AmbaRTSL_PllSetUart0Clk, AmbaRTSL_PllGetUart0Clk, AmbaRTSL_PllSetUart0ClkConfig},
    { AMBA_CLK_UART1, AmbaRTSL_PllSetUart1Clk, AmbaRTSL_PllGetUart1Clk, AmbaRTSL_PllSetUart1ClkConfig},
    { AMBA_CLK_UART2, AmbaRTSL_PllSetUart2Clk, AmbaRTSL_PllGetUart2Clk, AmbaRTSL_PllSetUart2ClkConfig},
    { AMBA_CLK_UART3, AmbaRTSL_PllSetUart3Clk, AmbaRTSL_PllGetUart3Clk, AmbaRTSL_PllSetUart3ClkConfig},
    { AMBA_CLK_USBPHY, NULL, AmbaRTSL_PllGetUsbPhyClk, NULL},
    { AMBA_CLK_VIN0, AmbaRTSL_PllFineAdjSensor0Clk, AmbaRTSL_PllGetVin0Clk, NULL},
    { AMBA_CLK_VIN1, AmbaRTSL_PllFineAdjEthernetClk, AmbaRTSL_PllGetVin1Clk, NULL},
    { AMBA_CLK_VISION, AmbaRTSL_PllSetVisionClk, AmbaRTSL_PllGetVisionClk, NULL},
    { AMBA_CLK_VOUTLCD, AmbaRTSL_PllSetVoutLcdClk, AmbaRTSL_PllGetVoutLcdClk, AmbaRTSL_PllSetVoutLcdClkConfig},
    { AMBA_CLK_VOUTTV, AmbaRTSL_PllSetVoutTvClk, AmbaRTSL_PllGetVoutTvClk, AmbaRTSL_PllSetVoutTvClkConfig},
};

static FLOAT AmbaPllOutClkFreq[AMBA_NUM_PLL];       /* pll_out_xxx Frequency (Hz) */

/**
 *  PLL_NotifyCoreFreqChanges - Re-calculate clock frequencies for those clock divided from Core PLL
 *  @return error code
 */
static UINT32 PLL_NotifyCoreFreqChanges(void)
{
    AmbaRTSL_TmrCoreFreqChg();
    AmbaRTSL_WdtCoreFreqChg();

    return PLL_ERR_NONE;
}

/**
 *  PLL_SetClkRefFreq - Set reference clock frequency value
 *  @param[in] Frequency Frequency value of the reference clock
 */
static void PLL_SetClkRefFreq(UINT32 Frequency)
{
    UINT32 i, *pPllRefFreq;

    if (AmbaPllSrcClks.ClkRefFreq != Frequency) {
        AmbaPllSrcClks.ClkRefFreq = Frequency;

        /* Reset all PLL reference frequencies */
        pPllRefFreq = (UINT32 *) & (AmbaPllSrcClks.PllRefFreq[0]);
        for (i = 0U; i < AMBA_NUM_PLL; i++) {
            pPllRefFreq[i] = Frequency;
        }
    }

    /* Get all PLL clock frequencies */
    for (i = 0; i < AMBA_NUM_PLL; i++) {
        (void)AmbaRTSL_PllCalcPllOutFreq(i);
    }
}

/**
 *  PLL_NotifyCortexFreqChanges - Re-calculate clock frequencies for those clock divided from Cortex PLL
 *  @return error code
 */
static UINT32 PLL_NotifyCortexFreqChanges(void)
{
    UINT32 NewPllFreq;

    /* Set PWC&RTC delay time*/
    NewPllFreq = AmbaRTSL_PllGetCortexClk();
    AmbaRTSL_PllSetNumCpuCycleUs(NewPllFreq);
    AmbaRTSL_PllSetNumCpuCycleMs(NewPllFreq);

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllInit - PLL RTSL initializations
 *  @param[in] ClkRefFreq Frequency value of the reference clock
 */
void AmbaRTSL_PllInit(UINT32 ClkRefFreq)
{
    AMBA_PLL_EXT_CLK_CONFIG_s *pExtClkConfig = &AmbaPllSrcClks.ExtClkConfig;

    /* DRAM controller initializations */
    AmbaRTSL_DramcInit();

    /* do not use fractional mode */
    pAmbaRCT_Reg->VisionPllCtrl.FractionEnable = 0U;
    pAmbaRCT_Reg->SdPllCtrl.FractionEnable = 0U;
    pAmbaRCT_Reg->NandPllCtrl.FractionEnable = 0U;
    pAmbaRCT_Reg->IdspPllCtrl.FractionEnable = 0U;
    pAmbaRCT_Reg->DdrPllCtrl.FractionEnable = 0U;
    pAmbaRCT_Reg->CorePllCtrl.FractionEnable = 0U;
    pAmbaRCT_Reg->CortexPllCtrl.FractionEnable = 0U;

    /* force Idsp clk to be multiple of 24MHz  */
    pAmbaRCT_Reg->IdspPllCtrl.Sdiv = 0U;
    pAmbaRCT_Reg->IdspPllCtrl.Sout = 0U;
    pAmbaRCT_Reg->IdspPllPostscaler.Divider = 0U;

    /* use fractional mode */
    pAmbaRCT_Reg->VoutLcdPllCtrl.FractionEnable = 1U;
    pAmbaRCT_Reg->HdmiPllCtrl.FractionEnable = 1U;
    pAmbaRCT_Reg->SensorPllCtrl.FractionEnable = 1U;
    pAmbaRCT_Reg->AudioPllCtrl.FractionEnable = 1U;

    /* set default external clock inputs as disabled */
    pExtClkConfig->ExtAudioClkFreq = 0U;
    pExtClkConfig->ExtSensorInputClkFreq = 0U;
    pExtClkConfig->ExtMuxLvdsSpclkClkFreq = 0U;
    pExtClkConfig->ExtEthernetClkFreq = 0U;
    pExtClkConfig->SpclkMuxSelect = 0U;

    /* set the frequency of the Reference Clock */
    PLL_SetClkRefFreq(ClkRefFreq);

    /* Don't change Cortex frequency at runtime because generic timer frequency will be incorrect. */
    (void)AmbaRTSL_PllNotifyFreqChanges(AMBA_PLL_CORTEX);

    /* Disable DAC power by default */
    pAmbaRCT_Reg->DacCtrl.PowerDown = 1;
}

/**
 *  AmbaRTSL_PllNotifyFreqChanges - Re-calculate clock frequencies for those clock divided from the specified PLL
 *  @param[in] PllID PLL ID
 *  @return error code
 */
UINT32 AmbaRTSL_PllNotifyFreqChanges(UINT32 PllID)
{
    UINT32 RetVal = PLL_ERR_NONE;

    if (PllID == AMBA_PLL_CORE) {
        RetVal = PLL_NotifyCoreFreqChanges();
    } else if (PllID == AMBA_PLL_CORTEX) {
        RetVal = PLL_NotifyCortexFreqChanges();
    } else if (PllID < AMBA_NUM_PLL) {
        RetVal = PLL_ERR_IMPL;
    } else {
        RetVal = PLL_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetExtClkConfig - Set external clock configurations
 *  @param[in] pExtClkConfig pointer to the external clock configurations (NULL = None)
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetExtClkConfig(const AMBA_PLL_EXT_CLK_CONFIG_s *pExtClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;

    if (pExtClkConfig == NULL) {
        RetVal = PLL_ERR_ARG;
    } else {
        AmbaPllSrcClks.ExtClkConfig.ExtAudioClkFreq = pExtClkConfig->ExtAudioClkFreq;
        AmbaPllSrcClks.ExtClkConfig.ExtSensorInputClkFreq = pExtClkConfig->ExtSensorInputClkFreq;
        AmbaPllSrcClks.ExtClkConfig.ExtMuxLvdsSpclkClkFreq = pExtClkConfig->ExtMuxLvdsSpclkClkFreq;
        AmbaPllSrcClks.ExtClkConfig.ExtEthernetClkFreq = pExtClkConfig->ExtEthernetClkFreq;

        if (pExtClkConfig->SpclkMuxSelect < (UINT32)AMBA_PLL_NUM_MUX_SPCLK) {
            AmbaPllSrcClks.ExtClkConfig.SpclkMuxSelect = pExtClkConfig->SpclkMuxSelect;
            if (pExtClkConfig->SpclkMuxSelect == (UINT32)AMBA_PLL_MUX_SPCLK_NA) {
                AmbaCSL_PllSetMuxedLvdsSpclkSelect(0U);
            } else {
                AmbaCSL_PllSetMuxedLvdsSpclkSelect((UINT32)1U << (pExtClkConfig->SpclkMuxSelect - 1U));
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllGetExtClkConfig - Get external clock configurations
 *  @param[out] pExtClkConfig pointer to the external clock configurations (NULL = None)
 *  @return error code
 */
UINT32 AmbaRTSL_PllGetExtClkConfig(AMBA_PLL_EXT_CLK_CONFIG_s *pExtClkConfig)
{
    UINT32 RetVal = PLL_ERR_NONE;
    UINT32 i, RegVal;

    if (pExtClkConfig == NULL) {
        RetVal = PLL_ERR_ARG;
    } else {
        /* Get actual register settings */
        RegVal = (UINT32)pAmbaRCT_Reg->MuxedLvdsSpclk.ClkSrc;
        if (RegVal == 0U) {
            AmbaPllSrcClks.ExtClkConfig.SpclkMuxSelect = (UINT32)AMBA_PLL_MUX_SPCLK_NA;
        } else {
            for (i = 0U; i < AMBA_PLL_NUM_MUX_SPCLK; i++) {
                if (((RegVal >> i) & 0x1U) != 0x0U) {
                    AmbaPllSrcClks.ExtClkConfig.SpclkMuxSelect = i;
                    break;
                }
            }
        }

        pExtClkConfig->ExtAudioClkFreq = AmbaPllSrcClks.ExtClkConfig.ExtAudioClkFreq;
        pExtClkConfig->ExtEthernetClkFreq = AmbaPllSrcClks.ExtClkConfig.ExtEthernetClkFreq;
        pExtClkConfig->ExtSensorInputClkFreq = AmbaPllSrcClks.ExtClkConfig.ExtSensorInputClkFreq;
        pExtClkConfig->ExtMuxLvdsSpclkClkFreq = AmbaPllSrcClks.ExtClkConfig.ExtMuxLvdsSpclkClkFreq;
        pExtClkConfig->SpclkMuxSelect = AmbaPllSrcClks.ExtClkConfig.SpclkMuxSelect;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllGetClkRefFreq - Get CLK_REF clock frequency
 *  @return SoC reference clock frequency
 */
UINT32 AmbaRTSL_PllGetClkRefFreq(void)
{
    return AmbaPllSrcClks.ClkRefFreq;
}

/**
 *  AmbaRTSL_PllGetPllRefFreq - Get the reference clock frequency of the speicifed PLL
 *  @param[in] PllID PLL ID
 *  @return Reference clock frequency of the specified PLL
 */
UINT32 AmbaRTSL_PllGetPllRefFreq(UINT32 PllID)
{
    UINT32 ClkFreq = 0U;

    if (PllID < AMBA_NUM_PLL) {
        ClkFreq = AmbaPllSrcClks.PllRefFreq[PllID];
    }

    return ClkFreq;
}

/**
 *  AmbaRTSL_PllSetPllOutFreq - Get the speicifed PLL output frequency
 *  @param[in] PllID PLL ID
 *  @param[in] Freq PLL output clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetPllOutFreq(UINT32 PllID, FLOAT Freq)
{
    UINT32 RetVal = PLL_ERR_NONE;

    if (PllID < AMBA_NUM_PLL) {
        AmbaPllOutClkFreq[PllID] = Freq;
    } else {
        RetVal = PLL_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllGetPllOutFreq - Get the speicifed PLL output frequency
 *  @param[in] PllID PLL ID
 *  @return PLL output clock frequency
 */
FLOAT AmbaRTSL_PllGetPllOutFreq(UINT32 PllID)
{
    FLOAT ClkFreq = 0.0F;

    if (PllID < AMBA_NUM_PLL) {
        ClkFreq = AmbaPllOutClkFreq[PllID];
    }

    return ClkFreq;
}

/**
 *  AmbaRTSL_PllSetIOClk - Set IO frequency
 *  @param[in] ClkID Clock ID
 *  @param[in] DesiredFreq Target IO clock frequency
 *  @param[out] pActualFreq The actual IO clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetIOClk(UINT32 ClkID, UINT32 DesiredFreq, UINT32 *pActualFreq)
{
    AMBA_PLL_CALLBACK_s const *clk = NULL;
    UINT32 RetVal = PLL_ERR_NONE;

    if (ClkID < (UINT32)AMBA_CLK_NUM) {
        clk = &PLL_CB[ClkID];

        if (clk->SetFunc != NULL) {
            (void)clk->SetFunc(DesiredFreq);
            if (pActualFreq != NULL) {
                if (clk->GetFunc != NULL) {
                    *pActualFreq = clk->GetFunc();
                } else {
                    *pActualFreq = DesiredFreq;
                }
            } else {
                /* do nothing */
            }
        } else {
            RetVal = PLL_ERR_ARG;
        }

    } else {
        RetVal = PLL_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllGetIOClk - Get IO frequency
 *  @param[in] ClkID Clock ID
 *  @param[out] pActualFreq The actual IO clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllGetIOClk(UINT32 ClkID, UINT32 *pActualFreq)
{
    AMBA_PLL_CALLBACK_s const *clk = NULL;
    UINT32 RetVal = PLL_ERR_NONE;

    if (ClkID < (UINT32)AMBA_CLK_NUM) {
        clk = &PLL_CB[ClkID];

        if (clk->GetFunc != NULL) {
            *pActualFreq = clk->GetFunc();
        }
    } else {
        RetVal = PLL_ERR_ARG;
    }

    return RetVal;
}

/**
 *  PLL_CalcAudioPllOutFreq - Get Audio PLL output clock frequency
 *  @return Audio PLL output clock frequency
 */
static FLOAT PLL_CalcAudioPllOutFreq(void)
{
    UINT32 Frequency;

    if (0x0U != AmbaCSL_PllGetAudioRefClkCtrl0()) {
        if (AmbaCSL_PllGetAudioRefClkCtrl1() == 1U) {
            /* PLL input = xx_clk_si */
            Frequency = AmbaPllSrcClks.ExtClkConfig.ExtSensorInputClkFreq;
        } else if (AmbaCSL_PllGetAudioRefClkCtrl1() == 2U) {
            /* PLL input = MUXED_LVDS_SPCLK */
            Frequency = AmbaPllSrcClks.ExtClkConfig.ExtMuxLvdsSpclkClkFreq;
        } else {
            Frequency = 0U;
        }
    } else {
        /* PLL input = CLK_REF */
        Frequency = AmbaPllSrcClks.ClkRefFreq;
    }
    AmbaPllSrcClks.PllRefFreq[AMBA_PLL_AUDIO] = Frequency;
    AmbaPllOutClkFreq[AMBA_PLL_AUDIO] = AmbaRTSL_PllCalcFreq(AMBA_PLL_AUDIO);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_AUDIO));
}

/**
 *  PLL_CalcCorePllOutFreq - Get Core PLL output clock frequency
 *  @return Core PLL output clock frequency
 */
static FLOAT PLL_CalcCorePllOutFreq(void)
{
    AmbaPllOutClkFreq[AMBA_PLL_CORE] = AmbaRTSL_PllCalcFreq(AMBA_PLL_CORE);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_CORE));
}

/**
 *  PLL_CalcCortexPllOutFreq - Get Cortex PLL output clock frequency
 *  @return Cortex PLL output clock frequency
 */
static FLOAT PLL_CalcCortexPllOutFreq(void)
{
    AmbaPllOutClkFreq[AMBA_PLL_CORTEX] = AmbaRTSL_PllCalcFreq(AMBA_PLL_CORTEX);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_CORTEX));
}

/**
 *  PLL_CalcDdrPllOutFreq - Get DDR PLL output clock frequency
 *  @return DDR PLL output clock frequency
 */
static FLOAT PLL_CalcDdrPllOutFreq(void)
{
    AmbaPllOutClkFreq[AMBA_PLL_DDR] = AmbaRTSL_PllCalcFreq(AMBA_PLL_DDR);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_DDR));
}

/**
 *  PLL_CalcEnetPllOutFreq - Get ethernet PLL output clock frequency
 *  @return Ethernet PLL output clock frequency
 */
static FLOAT PLL_CalcEnetPllOutFreq(void)
{
    AmbaPllOutClkFreq[AMBA_PLL_ENET] = AmbaRTSL_PllCalcFreq(AMBA_PLL_ENET);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_ENET));
}

/**
 *  PLL_GetOscillatorFreq - Get crystal oscillator frequency
 *  @return Oscillator frequency
 */
static UINT32 PLL_GetOscillatorFreq(void)
{
    UINT32 Freq = AmbaRTSL_PllGetClkRefFreq();

    if (pAmbaRCT_Reg->SysConfig.RefClk48mSelect != 0x0U) {
        Freq *= 2U;
    }

    if (pAmbaRCT_Reg->SysConfig.RefClk72mSelect != 0x0U) {
        Freq *= 3U;
    }

    return Freq;
}

/**
 *  PLL_GetHdmiPhyRefFreq - Get HDMI reference clock frequency
 *  @return HDMI reference clock frequency
 */
static UINT32 PLL_GetHdmiPhyRefFreq(void)
{
    UINT32 Frequency;

    if (AmbaCSL_PllGetHdmiRefClkSelect() == 0U) {
        /* PLL input = xx_xin */
        Frequency = PLL_GetOscillatorFreq();
    } else {
        /* use output of HDMI prescaler */
        if (0x0U != AmbaCSL_PllGetVoutTvRefClkCtrl0()) {
            if (AmbaCSL_PllGetVoutTvRefClkCtrl1() == 1U) {
                /* PLL input = xx_clk_si */
                Frequency = AmbaPllSrcClks.ExtClkConfig.ExtSensorInputClkFreq;
            } else if (pAmbaRCT_Reg->VoutLcdRefClkCtrl1.ExtClkSrc == 2U) {
                /* PLL input = MUXED_LVDS_SPCLK */
                Frequency = AmbaPllSrcClks.ExtClkConfig.ExtMuxLvdsSpclkClkFreq;
            } else {
                Frequency = 0U;
            }
        } else {
            /* PLL input = CLK_REF */
            Frequency = AmbaRTSL_PllGetClkRefFreq();
        }

        Frequency /= AmbaCSL_PllGetHdmiPhyPrescaler();
    }

    (AmbaPllSrcClks.PllRefFreq[AMBA_PLL_HDMI] = Frequency);

    return Frequency;
}

/**
 *  PLL_CalcHdmiPllOutFreq - Get HDMI PLL output clock frequency
 *  @return HDMI PLL output clock frequency
 */
static FLOAT PLL_CalcHdmiPllOutFreq(void)
{
    AmbaPllSrcClks.PllRefFreq[AMBA_PLL_HDMI] = PLL_GetHdmiPhyRefFreq();
    AmbaPllOutClkFreq[AMBA_PLL_HDMI] = AmbaRTSL_PllCalcFreq(AMBA_PLL_HDMI);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_HDMI));
}

/**
 *  PLL_CalcIdspPllOutFreq - Get IDSP PLL output clock frequency
 *  @return IDSP PLL output clock frequency
 */
static FLOAT PLL_CalcIdspPllOutFreq(void)
{
    AmbaPllOutClkFreq[AMBA_PLL_IDSP] = AmbaRTSL_PllCalcFreq(AMBA_PLL_IDSP);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_IDSP));
}

/**
 *  PLL_CalcNandPllOutFreq - Get NAND PLL output clock frequency
 *  @return NAND PLL output clock frequency
 */
static FLOAT PLL_CalcNandPllOutFreq(void)
{
    AmbaPllOutClkFreq[AMBA_PLL_NAND] = AmbaRTSL_PllCalcFreq(AMBA_PLL_NAND);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_NAND));
}

/**
 *  PLL_CalcSdPllOutFreq - Get SD PLL output clock frequency
 *  @return SD PLL output clock frequency
 */
static FLOAT PLL_CalcSdPllOutFreq(void)
{
    AmbaPllOutClkFreq[AMBA_PLL_SD] = AmbaRTSL_PllCalcFreq(AMBA_PLL_SD);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SD));
}

/**
 *  PLL_CalcSensorPllOutFreq - Get Sensor PLL output clock frequency
 *  @return Sensor PLL output clock frequency
 */
static FLOAT PLL_CalcSensorPllOutFreq(void)
{
    AmbaPllOutClkFreq[AMBA_PLL_SENSOR0] = AmbaRTSL_PllCalcFreq(AMBA_PLL_SENSOR0);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SENSOR0));
}

/**
 *  PLL_CalcVideo2PllOutFreq - Get Video2 PLL output clock frequency
 *  @return Video2 PLL output clock frequency
 */
static FLOAT PLL_CalcVideo2PllOutFreq(void)
{
    if (pAmbaRCT_Reg->VoutLcdRefClkCtrl0.ClkRefSel != 0x0U) {
        if (pAmbaRCT_Reg->VoutLcdRefClkCtrl1.ExtClkSrc == 1U) {
            /* PLL input = xx_clk_si */
            AmbaPllSrcClks.PllRefFreq[AMBA_PLL_VIDEO2] = AmbaPllSrcClks.ExtClkConfig.ExtSensorInputClkFreq;
        } else if (pAmbaRCT_Reg->VoutLcdRefClkCtrl1.ExtClkSrc == 2U) {
            /* PLL input = MUXED_LVDS_SPCLK */
            AmbaPllSrcClks.PllRefFreq[AMBA_PLL_VIDEO2] = AmbaPllSrcClks.ExtClkConfig.ExtMuxLvdsSpclkClkFreq;
        } else {
            /* for misraC checking, do nothing */
        }
    } else {
        /* PLL input = CLK_REF */
        AmbaPllSrcClks.PllRefFreq[AMBA_PLL_VIDEO2] = AmbaPllSrcClks.ClkRefFreq;
    }

    AmbaPllOutClkFreq[AMBA_PLL_VIDEO2] = AmbaRTSL_PllCalcFreq(AMBA_PLL_VIDEO2);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_VIDEO2));
}

/**
 *  PLL_CalcVisionPllOutFreq - Get Vision PLL output clock frequency
 *  @return Vision PLL output clock frequency
 */
static FLOAT PLL_CalcVisionPllOutFreq(void)
{
    AmbaPllOutClkFreq[AMBA_PLL_VISION] = AmbaRTSL_PllCalcFreq(AMBA_PLL_VISION);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_VISION));
}

/**
 *  AmbaRTSL_PllCalcPllOutFreq - Calculate the speicifed PLL output frequency
 *  @param[in] PllID PLL ID
 *  @return error code
 */
UINT32 AmbaRTSL_PllCalcPllOutFreq(UINT32 PllID)
{
    UINT32 RetVal = PLL_ERR_NONE;

    if (PllID == AMBA_PLL_AUDIO) {
        (void)PLL_CalcAudioPllOutFreq();
    } else if (PllID == AMBA_PLL_CORE) {
        (void)PLL_CalcCorePllOutFreq();
    } else if (PllID == AMBA_PLL_CORTEX) {
        (void)PLL_CalcCortexPllOutFreq();
    } else if (PllID == AMBA_PLL_DDR) {
        (void)PLL_CalcDdrPllOutFreq();
    } else if (PllID == AMBA_PLL_ENET) {
        (void)PLL_CalcEnetPllOutFreq();
    } else if (PllID == AMBA_PLL_HDMI) {
        (void)PLL_CalcHdmiPllOutFreq();
    } else if (PllID == AMBA_PLL_IDSP) {
        (void)PLL_CalcIdspPllOutFreq();
    } else if (PllID == AMBA_PLL_NAND) {
        (void)PLL_CalcNandPllOutFreq();
    } else if (PllID == AMBA_PLL_SD) {
        (void)PLL_CalcSdPllOutFreq();
    } else if (PllID == AMBA_PLL_SENSOR0) {
        (void)PLL_CalcSensorPllOutFreq();
    } else if (PllID == AMBA_PLL_VIDEO2) {
        (void)PLL_CalcVideo2PllOutFreq();
    } else if (PllID == AMBA_PLL_VISION) {
        (void)PLL_CalcVisionPllOutFreq();
    } else {
        RetVal = PLL_ERR_ARG;
    }

    return RetVal;
}
