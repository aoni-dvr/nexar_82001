/**
 *  @file AmbaRTSL_PLL.h
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
 *  @details Definitions & Constants for PLL RTSL APIs
 *
 */

#ifndef AMBA_RTSL_PLL_H
#define AMBA_RTSL_PLL_H

#ifndef AMBA_SYS_DEF_H
#include "AmbaSYS_Def.h"
#endif

#ifndef AMBA_RTSL_PLL_DEF_H
#include "AmbaRTSL_PLL_Def.h"
#endif

/*
 * Defined in AmbaRTSL_PLL.c
 */
void AmbaRTSL_PllInit(UINT32 ClkRefFreq);
UINT32 AmbaRTSL_PllNotifyFreqChanges(UINT32 PllID);
UINT32 AmbaRTSL_PllSetExtClkConfig(const AMBA_PLL_EXT_CLK_CONFIG_s *pExtClkConfig);
UINT32 AmbaRTSL_PllGetExtClkConfig(AMBA_PLL_EXT_CLK_CONFIG_s *pExtClkConfig);
UINT32 AmbaRTSL_PllGetClkRefFreq(void);
UINT32 AmbaRTSL_PllGetPllRefFreq(UINT32 PllID);
UINT32 AmbaRTSL_PllSetPllOutFreq(UINT32 PllID, FLOAT Freq);
FLOAT AmbaRTSL_PllGetPllOutFreq(UINT32 PllID);
UINT32 AmbaRTSL_PllCalcPllOutFreq(UINT32 PllID);
UINT32 AmbaRTSL_PllSetIOClk(UINT32 ClkID, UINT32 DesiredFreq, UINT32 *pActualFreq);
UINT32 AmbaRTSL_PllGetIOClk(UINT32 ClkID, UINT32 *pActualFreq);

/*
 * Defined in AmbaRTSL_PLL_Config.c
 */
UINT32 AmbaRTSL_PllSetAdcClkConfig(UINT32 AdcClkConfig);
UINT32 AmbaRTSL_PllSetAudioClkConfig(UINT32 AudioClkConfig);
UINT32 AmbaRTSL_PllSetGtxClkConfig(UINT32 ClkGtxClkConfig);
UINT32 AmbaRTSL_PllSetPwmClkConfig(UINT32 PwmClkConfig);
UINT32 AmbaRTSL_PllSetSd0ScalerClkSrc(UINT32 SdClkConfig);
UINT32 AmbaRTSL_PllSetSd1ScalerClkSrc(UINT32 SdClkConfig);
UINT32 AmbaRTSL_PllSetSd2ScalerClkSrc(UINT32 SdClkConfig);
UINT32 AmbaRTSL_PllSetSensor0ClkDir(UINT32 ClkSiDir);
UINT32 AmbaRTSL_PllSetSpiClkConfig(UINT32 SpiClkConfig);
UINT32 AmbaRTSL_PllSetUartApbClkConfig(UINT32 UartClkConfig);
UINT32 AmbaRTSL_PllSetUart0ClkConfig(UINT32 UartClkConfig);
UINT32 AmbaRTSL_PllSetUart1ClkConfig(UINT32 UartClkConfig);
UINT32 AmbaRTSL_PllSetUart2ClkConfig(UINT32 UartClkConfig);
UINT32 AmbaRTSL_PllSetUart3ClkConfig(UINT32 UartClkConfig);
UINT32 AmbaRTSL_PllSetVoutLcdClkConfig(UINT32 VoutLcdClkConfig);
UINT32 AmbaRTSL_PllSetVoutTvClkConfig(UINT32 VoutTvClkConfig);
UINT32 AmbaRTSL_PllSetVin1ClkConfig(UINT32 Vin1ClkConfig);

void AmbaRTSL_PllClkEnable(UINT32 SysFeature);
void AmbaRTSL_PllClkDisable(UINT32 SysFeature);
void AmbaRTSL_PllClkCheck(UINT32 SysFeature, UINT32 *pEnable);

void AmbaRTSL_PllVdspClkDisable(void);
void AmbaRTSL_PllIdspClkDisable(void);

UINT32 AmbaRTSL_RctTimerWait(UINT32 TimeOut);

void AmbaRTSL_PllSetNumCpuCycleUs(UINT32 Frequency);
void AmbaRTSL_PllSetNumCpuCycleMs(UINT32 Frequency);
UINT32 AmbaRTSL_PllGetNumCpuCycleUs(void);
UINT32 AmbaRTSL_PllGetNumCpuCycleMs(void);

/*
 * Defined in AmbaRTSL_PLL_GetClkFreq.c
 */
UINT32 AmbaRTSL_PllGetAdcClk(void);         /* clk_adc */
UINT32 AmbaRTSL_PllGetAhbClk(void);         /* gclk_ahb */
UINT32 AmbaRTSL_PllGetApbClk(void);         /* gclk_apb */
UINT32 AmbaRTSL_PllGetAxiClk(void);         /* gclk_axi */
UINT32 AmbaRTSL_PllGetAudioClk(void);       /* gclk_au */
UINT32 AmbaRTSL_PllGetAudio2Clk(void);      /* gclk_au3 */
UINT32 AmbaRTSL_PllGetCoreClk(void);        /* gclk_core */
UINT32 AmbaRTSL_PllGetCortexClk(void);      /* gclk_cortex */
UINT32 AmbaRTSL_PllGetDbgClk(void);         /* gclk_dbg */
UINT32 AmbaRTSL_PllGetDebounceClk(void);    /* clk_debounce */
UINT32 AmbaRTSL_PllGetDramClk(void);        /* gclk_dram (=gclk_ddrio/2) */
UINT32 AmbaRTSL_PllGetEthernetClk(void);    /* gclk_enet_clk_rmii_from_pll */
UINT32 AmbaRTSL_PllGetGpioDebounceClk(void);/* clk_gpio_debounce */
UINT32 AmbaRTSL_PllGetIdspClk(void);        /* gclk_idsp */
UINT32 AmbaRTSL_PllGetIrClk(void);          /* gclk_ir */
UINT32 AmbaRTSL_PllGetNandClk(void);        /* gclk_nand */
UINT32 AmbaRTSL_PllGetPwmClk(void);         /* gclk_pwm */
UINT32 AmbaRTSL_PllGetSd0Clk(void);         /* gclk_sd48 */
UINT32 AmbaRTSL_PllGetSd1Clk(void);         /* gclk_sdio0 */
UINT32 AmbaRTSL_PllGetSd2Clk(void);         /* gclk_sdio1 */
UINT32 AmbaRTSL_PllGetSensor0Clk(void);     /* gclk_so (xx_clk_si) */
UINT32 AmbaRTSL_PllGetSpiMasterClk(void);   /* gclk_ssi */
UINT32 AmbaRTSL_PllGetSpiSlaveClk(void);    /* gclk_ssi2 */
UINT32 AmbaRTSL_PllGetSpiNorClk(void);      /* gclk_ssi3 */
UINT32 AmbaRTSL_PllGetUartApbClk(void);     /* gclk_uart_apb */
UINT32 AmbaRTSL_PllGetUart0Clk(void);       /* gclk_uart0 */
UINT32 AmbaRTSL_PllGetUart1Clk(void);       /* gclk_uart1 */
UINT32 AmbaRTSL_PllGetUart2Clk(void);       /* gclk_uart2 */
UINT32 AmbaRTSL_PllGetUart3Clk(void);       /* gclk_uart3 */
UINT32 AmbaRTSL_PllGetUsbPhyClk(void);      /* gclk_usb_phy */
UINT32 AmbaRTSL_PllGetVin0Clk(void);        /* gclk_so_vin */
UINT32 AmbaRTSL_PllGetVin1Clk(void);        /* gclk_so_pip */
UINT32 AmbaRTSL_PllGetVoutLcdClk(void);     /* gclk_vo2 */
UINT32 AmbaRTSL_PllGetVoutTvClk(void);      /* gclk_vo */

/*
 * Defined in AmbaRTSL_PLL_SetClkFreq.c
 */
UINT32 AmbaRTSL_PllSetFreq(UINT32 PllIdx, UINT32 Frequency);
UINT32 AmbaRTSL_PllFineAdjFreq(UINT32 PllIdx, UINT32 Frequency);
UINT32 AmbaRTSL_PllAdjFreq(UINT32 PllIdx, UINT32 Frequency);
FLOAT AmbaRTSL_PllCalcFreq(UINT32 PllIdx);

UINT32 AmbaRTSL_PllSetAdcClk(UINT32 Frequency);         /* clk_adc */
UINT32 AmbaRTSL_PllSetAudioClk(UINT32 Frequency);       /* gclk_au */
UINT32 AmbaRTSL_PllSetAudio2Clk(UINT32 Frequency);      /* gclk_au3 */
UINT32 AmbaRTSL_PllSetCoreClk(UINT32 Frequency);        /* gclk_core */
UINT32 AmbaRTSL_PllSetDebounceClk(UINT32 Frequency);    /* clk_debounce */
UINT32 AmbaRTSL_PllSetEthernetClk(UINT32 Frequency);    /* gclk_enet_clk_rmii_from_pll */
UINT32 AmbaRTSL_PllSetIdspClk(UINT32 Frequency);        /* gclk_idsp */
UINT32 AmbaRTSL_PllSetIrClk(UINT32 Frequency);          /* gclk_ir */
UINT32 AmbaRTSL_PllSetPwmClk(UINT32 Frequency);         /* gclk_pwm */
UINT32 AmbaRTSL_PllSetSdPllOutClk(UINT32 Frequency);    /* pll_out_sd */
UINT32 AmbaRTSL_PllSetSd0Clk(UINT32 Frequency);         /* gclk_sd48 */
UINT32 AmbaRTSL_PllSetSd1Clk(UINT32 Frequency);         /* gclk_sdio0 */
UINT32 AmbaRTSL_PllSetSd2Clk(UINT32 Frequency);         /* gclk_sdio1 */
UINT32 AmbaRTSL_PllSetSensor0Clk(UINT32 Frequency);     /* gclk_so (xx_clk_si) */
UINT32 AmbaRTSL_PllSetSpiMasterClk(UINT32 Frequency);   /* gclk_ssi */
UINT32 AmbaRTSL_PllSetSpiSlaveClk(UINT32 Frequency);    /* gclk_ssi2 */
UINT32 AmbaRTSL_PllSetSpiNorClk(UINT32 Frequency);      /* gclk_ssi3 */
UINT32 AmbaRTSL_PllSetUartApbClk(UINT32 Frequency);     /* gclk_uart_apb */
UINT32 AmbaRTSL_PllSetUart0Clk(UINT32 Frequency);       /* gclk_uart0 */
UINT32 AmbaRTSL_PllSetUart1Clk(UINT32 Frequency);       /* gclk_uart1 */
UINT32 AmbaRTSL_PllSetUart2Clk(UINT32 Frequency);       /* gclk_uart2 */
UINT32 AmbaRTSL_PllSetUart3Clk(UINT32 Frequency);       /* gclk_uart3 */
UINT32 AmbaRTSL_PllSetVoutLcdClk(UINT32 Frequency);     /* gclk_vo2 */
UINT32 AmbaRTSL_PllSetVoutTvClk(UINT32 Frequency);      /* gclk_vo */
UINT32 AmbaRTSL_PllSetVoutLcdClkHint(UINT32 Type, UINT32 Ratio);
UINT32 AmbaRTSL_PllSetVoutTvClkHint(UINT32 Type, UINT32 Ratio);
UINT32 AmbaRTSL_PllFineAdjAudioClk(UINT32 Frequency);   /* gclk_au */
UINT32 AmbaRTSL_PllFineAdjSensor0Clk(UINT32 Frequency); /* gclk_so (xx_clk_si) */
UINT32 AmbaRTSL_PllFineAdjEthernetClk(UINT32 Frequency);/* gclk_enet_clk_rmii / gclk_so_pip */
UINT32 AmbaRTSL_PllFineAdjVoutLcdClk(UINT32 Frequency); /* gclk_vo2 */
UINT32 AmbaRTSL_PllFineAdjVoutTvClk(UINT32 Frequency);  /* gclk_vo */

#endif /* AMBA_RTSL_PLL_H */
