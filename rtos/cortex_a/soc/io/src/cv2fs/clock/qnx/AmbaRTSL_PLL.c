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

//#include "AmbaRTSL_DRAMC.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_TMR.h"
#include "AmbaRTSL_WDT.h"

#include "AmbaCSL_PLL.h"

typedef struct {
    UINT32  ClkRefFreq;                             /* CLK_REF: Clock Frequency (Hz) */
    UINT32  PllRefFreq[AMBA_NUM_PLL];               /* Reference Clock Frequency (Hz) */
    AMBA_PLL_EXT_CLK_CONFIG_s ExtClkConfig;         /* External Clock Configurations */
} AMBA_PLL_CLK_SOURCE_CTRL_s;

static AMBA_PLL_CLK_SOURCE_CTRL_s AmbaPllSrcClks = {
    .ClkRefFreq = 24000000U,                        /* CLK_REF: Clock Frequency (Hz) */

    /* PLL Reference Clock Frequency (Hz) */
    .PllRefFreq = {
        [AMBA_PLL_AUDIO]     = 24000000U,    /* default = CLK_REF */
        [AMBA_PLL_CORE]      = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_CORTEX0]   = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_CORTEX1]   = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_DDR]       = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_ENET]      = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_IDSP]      = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_NAND]      = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_SD]        = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_SENSOR0]   = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_SENSOR2]   = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_VIDEO_A]   = 24000000U,    /* default = CLK_REF */
        [AMBA_PLL_VIDEO_B]   = 24000000U,    /* default = CLK_REF */
        [AMBA_PLL_VISION]    = 24000000U,    /* always = CLK_REF */
        [AMBA_PLL_FEX]       = 24000000U,    /* always = CLK_REF */
    },

    .ExtClkConfig = {                               /* External Clock Configurations */
        .ExtAudioClkFreq = 0U,
        .ExtSensorInputClkFreq = 0U,
        .ExtMuxLvdsSpclkClkFreq = 0U,
        .ExtEthernetClkFreq = 0U,
        .SpclkMuxSelect = 0U
    }
};

static FLOAT AmbaPllOutClkFreq[AMBA_NUM_PLL];       /* pll_out_xxx Frequency (Hz) */

/**
 *  PLL_NotifyCoreFreqChanges - Re-calculate clock frequencies for those clock divided from Core PLL
 *  @return error code
 */
static UINT32 PLL_NotifyCoreFreqChanges(void)
{
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
    NewPllFreq = AmbaRTSL_PllGetCortex1Clk();
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
    //AmbaRTSL_DramcInit();

    /* do not use fractional mode */
    pAmbaRCT_Reg->VisionPllCtrl.PllOpMode = 0U;
    pAmbaRCT_Reg->SdPllCtrl.PllOpMode = 0U;
    pAmbaRCT_Reg->NandPllCtrl.PllOpMode = 0U;
    pAmbaRCT_Reg->IdspPllCtrl.PllOpMode = 0U;
    pAmbaRCT_Reg->DdrPllCtrl.PllOpMode = 0U;
    pAmbaRCT_Reg->CorePllCtrl.PllOpMode = 0U;
    pAmbaRCT_Reg->Cortex0PllCtrl.PllOpMode = 0U;
    pAmbaRCT_Reg->Cortex1PllCtrl.PllOpMode = 0U;

    /* force Idsp clk to be multiple of 24MHz  */
    pAmbaRCT_Reg->IdspPllCtrl.Sdiv = 0U;
    pAmbaRCT_Reg->IdspPllCtrl.Sout = 0U;
    pAmbaRCT_Reg->IdspPllPostscaler.ClkDivider = 0U;

    /* use fractional mode */
    pAmbaRCT_Reg->Vout0PllCtrl.PllOpMode = 1U;
    pAmbaRCT_Reg->Vout1PllCtrl.PllOpMode = 1U;
    pAmbaRCT_Reg->SensorPllCtrl.PllOpMode = 1U;
    pAmbaRCT_Reg->Sensor2PllCtrl.PllOpMode = 1U;
    pAmbaRCT_Reg->AudioPllCtrl.PllOpMode = 1U;

    /* set default external clock inputs as disabled */
    pExtClkConfig->ExtAudioClkFreq = 0U;
    pExtClkConfig->ExtSensorInputClkFreq = 0U;
    pExtClkConfig->ExtMuxLvdsSpclkClkFreq = 0U;
    pExtClkConfig->ExtEthernetClkFreq = 0U;
    pExtClkConfig->SpclkMuxSelect = 0U;

    /* set the frequency of the Reference Clock */
    PLL_SetClkRefFreq(ClkRefFreq);

    if (AmbaCSL_PllGetNumCpuCycleUs() == 0x0U) {
        AmbaCSL_PllSetNumCpuCycleUs(AmbaPllOutClkFreq[AMBA_PLL_CORTEX1]);
    }

    if (AmbaCSL_PllGetNumCpuCycleMs() == 0x0U) {
        AmbaCSL_PllSetNumCpuCycleMs(AmbaPllOutClkFreq[AMBA_PLL_CORTEX1]);
    }
}

/**
 *  AmbaRTSL_PllNotifyFreqChanges - Re-calculate clock frequencies for those clock divided from the specified PLL
 *  @param[in] PllID PLL ID
 *  @return error code
 */
UINT32 AmbaRTSL_PllNotifyFreqChanges(UINT32 PllID)
{
    UINT32 RetVal = PLL_ERR_NONE;

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
        RegVal = pAmbaRCT_Reg->MuxedLvdsSpclk.ClkSrc;
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
        pExtClkConfig->ExtAudio1ClkFreq = AmbaPllSrcClks.ExtClkConfig.ExtAudio1ClkFreq;
        pExtClkConfig->ExtSensorInputClkFreq = AmbaPllSrcClks.ExtClkConfig.ExtSensorInputClkFreq;
        pExtClkConfig->ExtMuxLvdsSpclkClkFreq = AmbaPllSrcClks.ExtClkConfig.ExtMuxLvdsSpclkClkFreq;
        pExtClkConfig->ExtEthernetClkFreq = AmbaPllSrcClks.ExtClkConfig.ExtEthernetClkFreq;
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
    AmbaPllOutClkFreq[AMBA_PLL_CORTEX1] = AmbaRTSL_PllCalcFreq(AMBA_PLL_CORTEX1);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_CORTEX1));
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
 *  PLL_CalcFexPllOutFreq - Get Fex PLL output clock frequency
 *  @return Vision PLL output clock frequency
 */
static FLOAT PLL_CalcFexPllOutFreq(void)
{
    AmbaPllOutClkFreq[AMBA_PLL_FEX] = AmbaRTSL_PllCalcFreq(AMBA_PLL_FEX);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_FEX));
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
 *  PLL_CalcSensor2PllOutFreq - Get Sensor2 PLL out clock frequency
 *  @return Sensor2 clock frequency
 */
static FLOAT PLL_CalcSensor2PllOutFreq(void)
{
    AmbaPllOutClkFreq[AMBA_PLL_SENSOR2] = AmbaRTSL_PllCalcFreq(AMBA_PLL_SENSOR2);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SENSOR2));
}

/**
 *  PLL_CalcVideoAPllOutFreq - Get VideoA PLL output clock frequency
 *  @return VideoA PLL output clock frequency
 */
static FLOAT PLL_CalcVideoAPllOutFreq(void)
{
    if (pAmbaRCT_Reg->Vout0RefClkCtrl0.ClkRefSel != 0x0U) {
        if (pAmbaRCT_Reg->Vout0RefClkCtrl1.ExtClkSrc == 1U) {
            /* PLL input = xx_clk_si */
            AmbaPllSrcClks.PllRefFreq[AMBA_PLL_VIDEO_A] = AmbaPllSrcClks.ExtClkConfig.ExtSensorInputClkFreq;
        } else if (pAmbaRCT_Reg->Vout0RefClkCtrl1.ExtClkSrc == 2U) {
            /* PLL input = MUXED_LVDS_SPCLK */
            AmbaPllSrcClks.PllRefFreq[AMBA_PLL_VIDEO_A] = AmbaPllSrcClks.ExtClkConfig.ExtMuxLvdsSpclkClkFreq;
        } else {
            /* for misraC checking, do nothing */
        }
    } else {
        /* PLL input = CLK_REF */
        AmbaPllSrcClks.PllRefFreq[AMBA_PLL_VIDEO_A] = AmbaPllSrcClks.ClkRefFreq;
    }

    AmbaPllOutClkFreq[AMBA_PLL_VIDEO_A] = AmbaRTSL_PllCalcFreq(AMBA_PLL_VIDEO_A);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_VIDEO_A));
}

/**
 *  PLL_CalcVideoBPllOutFreq - Get VideoB PLL output clock frequency
 *  @return VideoB PLL output clock frequency
 */
static FLOAT PLL_CalcVideoBPllOutFreq(void)
{
    if (pAmbaRCT_Reg->Vout1RefClkCtrl0.ClkRefSel != 0x0U) {
        if (pAmbaRCT_Reg->Vout1RefClkCtrl1.ExtClkSrc == 1U) {
            /* PLL input = xx_clk_si */
            AmbaPllSrcClks.PllRefFreq[AMBA_PLL_VIDEO_B] = AmbaPllSrcClks.ExtClkConfig.ExtSensorInputClkFreq;
        } else if (pAmbaRCT_Reg->Vout1RefClkCtrl1.ExtClkSrc == 2U) {
            /* PLL input = MUXED_LVDS_SPCLK */
            AmbaPllSrcClks.PllRefFreq[AMBA_PLL_VIDEO_B] = AmbaPllSrcClks.ExtClkConfig.ExtMuxLvdsSpclkClkFreq;
        } else {
            /* for misraC checking, do nothing */
        }
    } else {
        /* PLL input = CLK_REF */
        AmbaPllSrcClks.PllRefFreq[AMBA_PLL_VIDEO_B] = AmbaPllSrcClks.ClkRefFreq;
    }

    AmbaPllOutClkFreq[AMBA_PLL_VIDEO_B] = AmbaRTSL_PllCalcFreq(AMBA_PLL_VIDEO_B);

    return (AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_VIDEO_B));
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
    } else if (PllID == AMBA_PLL_CORTEX1) {
        (void)PLL_CalcCortexPllOutFreq();
    } else if (PllID == AMBA_PLL_DDR) {
        (void)PLL_CalcDdrPllOutFreq();
    } else if (PllID == AMBA_PLL_ENET) {
        (void)PLL_CalcEnetPllOutFreq();
    } else if (PllID == AMBA_PLL_FEX) {
        (void)PLL_CalcFexPllOutFreq();
    } else if (PllID == AMBA_PLL_IDSP) {
        (void)PLL_CalcIdspPllOutFreq();
    } else if (PllID == AMBA_PLL_NAND) {
        (void)PLL_CalcNandPllOutFreq();
    } else if (PllID == AMBA_PLL_SD) {
        (void)PLL_CalcSdPllOutFreq();
    } else if (PllID == AMBA_PLL_SENSOR0) {
        (void)PLL_CalcSensorPllOutFreq();
    } else if (PllID == AMBA_PLL_SENSOR2) {
        (void)PLL_CalcSensor2PllOutFreq();
    } else if (PllID == AMBA_PLL_VIDEO_A) {
        (void)PLL_CalcVideoAPllOutFreq();
    } else if (PllID == AMBA_PLL_VIDEO_B) {
        (void)PLL_CalcVideoBPllOutFreq();
    } else if (PllID == AMBA_PLL_VISION) {
        (void)PLL_CalcVisionPllOutFreq();
    } else {
        RetVal = PLL_ERR_ARG;
    }

    return RetVal;
}
