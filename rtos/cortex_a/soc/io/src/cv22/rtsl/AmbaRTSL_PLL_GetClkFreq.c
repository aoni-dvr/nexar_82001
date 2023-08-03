/**
 *  @file AmbaRTSL_PLL_GetClkFreq.c
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

static UINT32 AmbaRTSL_PllGetHdmiPhyClk(void);

/**
 *  AmbaRTSL_PllGetAdcClk - Get ADC clock frequency
 *  @return ADC clock frequency
 */
UINT32 AmbaRTSL_PllGetAdcClk(void)
{
    UINT32 RefFreq;
    UINT32 Divider = AmbaCSL_PllGetAdcDivider();

    switch (AmbaCSL_PllGetAdcRefClkSelect()) {  /* ADC clock source selection (LSB 2-bit) */
    case 0U: /* clk_ref */
        RefFreq = AmbaRTSL_PllGetClkRefFreq();
        break;

    case 1U: /* gclk_vision */
        RefFreq = AmbaRTSL_PllGetVisionClk();
        break;

    case 2U: /* gclk_nand */
        RefFreq = AmbaRTSL_PllGetNandClk();
        break;

    default:
        RefFreq = 0U;
        break;
    }

    if (0x0U != Divider) {
        RefFreq = (RefFreq / Divider) / 2U;
    } else {
        RefFreq = 0U;
    }

    return RefFreq;
}

/**
 *  AmbaRTSL_PllGetAhbClk - Get AHB clock frequency
 *  @return AHB clock frequency
 */
UINT32 AmbaRTSL_PllGetAhbClk(void)
{
    /* gclk_ahb = (pll_out_core / 2) */
    return ((UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_CORE) >> 1U);
}

/**
 *  AmbaRTSL_PllGetApbClk - Get APB clock frequency
 *  @return APB clock frequency
 */
UINT32 AmbaRTSL_PllGetApbClk(void)
{
    /* gclk_apb = (pll_out_core / 4) */
    return ((UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_CORE) >> 2U);
}

/**
 *  AmbaRTSL_PllGetAxiClk - Get AXI clock frequency
 *  @return AXI clock frequency
 */
UINT32 AmbaRTSL_PllGetAxiClk(void)
{
    /* gclk_axi = (pll_out_cortex / 3) */
    return ((UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_CORTEX) / 3U);
}

/**
 *  AmbaRTSL_PllGetAudioClk - Get Audio (gclk_au) clock frequency
 *  @return Audio 0 clock frequency
 */
UINT32 AmbaRTSL_PllGetAudioClk(void)
{
    AMBA_PLL_EXT_CLK_CONFIG_s ExtClkConfig;
    UINT32 Frequency;

    if (0x0U != AmbaCSL_PllGetAudioExtClkCtrl()) {
        /* Use external clock source */
        (void)AmbaRTSL_PllGetExtClkConfig(&ExtClkConfig);
        Frequency = ExtClkConfig.ExtAudioClkFreq;
    } else {
        /* Use Audio PLL */
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_AUDIO);
        Frequency = Frequency / AmbaCSL_PllGetPostScaleRegVal(AMBA_PLL_AUDIO);
    }

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetAudio1Clk - Get Audio1 (gclk_au_1ch) clock frequency
 *  @return Audio 1 clock frequency
 */
UINT32 AmbaRTSL_PllGetAudio1Clk(void)
{
    AMBA_PLL_EXT_CLK_CONFIG_s ExtClkConfig;
    UINT32 Frequency;

    if (0x0U != AmbaCSL_PllGetAudio1ExtClkCtrl()) {
        /* Use external clock source */
        (void)AmbaRTSL_PllGetExtClkConfig(&ExtClkConfig);
        Frequency = ExtClkConfig.ExtAudio1ClkFreq;
    } else {
        /* Use Audio PLL */
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_AUDIO);
        Frequency = Frequency / AmbaCSL_PllGetAudio1Postscaler();
    }

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetCanClk - Get CANC clock frequency
 *  @return CANC clock frequency
 */
UINT32 AmbaRTSL_PllGetCanClk(void)
{
    UINT32 RefFreq;
    UINT32 Divider = AmbaCSL_PllGetCanDivider();

    switch (AmbaCSL_PllGetCanClkSrcSel()) { /* CANC clock source selection (LSB 2-bit) */
    case 0: /* CLK_REF */
        RefFreq = AmbaRTSL_PllGetClkRefFreq();
        break;

    case 1: /* pll_out_enet */
        RefFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_ENET);
        break;

    case 2: /* gclk_ahb */
        RefFreq = AmbaRTSL_PllGetAhbClk();
        break;

    default:
        RefFreq = 0U;
        break;
    }

    if (0x0U != Divider) {
        RefFreq = RefFreq / Divider;
    } else {
        RefFreq = 0U;
    }

    return RefFreq;
}

/**
 *  AmbaRTSL_PllGetCoreClk - Get core clock frequency
 *  @return Core clock frequency
 */
UINT32 AmbaRTSL_PllGetCoreClk(void)
{
    /* gclk_core = pll_out_core */
    return ((UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_CORE));
}

/**
 *  AmbaRTSL_PllGetCortexClk - Get Cortex clock frequency
 *  @return Cortex clock frequency
 */
UINT32 AmbaRTSL_PllGetCortexClk(void)
{
    UINT32 RetVal = 0U;

    if (0x0U != (AmbaCSL_PllGetCortexClkSrcSelect() & 1U)) {
        /* gclk_cortex = ref_clk */
        RetVal = AmbaRTSL_PllGetClkRefFreq();
    } else {
        /* gclk_cortex = pll_out_cortex */
        RetVal = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_CORTEX);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllGetDbgClk - Get debug clock frequency
 *  @return Debug clock frequency
 */
UINT32 AmbaRTSL_PllGetDbgClk(void)
{
    /* gclk_dbg = (pll_out_core / 8) */
    return ((UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_CORE) >> 3U);
}

/**
 *  AmbaRTSL_PllGetDebounceClk - Get Debounce clock frequency
 *  @return Debounce clock frequency
 */
UINT32 AmbaRTSL_PllGetDebounceClk(void)
{
    UINT32 Frequency;
    UINT32 Divider = AmbaCSL_PllGetDebounceDivider();

    if (0x0U != Divider) {
        Frequency = AmbaRTSL_PllGetClkRefFreq();
        Frequency = Frequency / Divider;
    } else {
        Frequency = 0U;
    }

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetDramClk - Get DRAM clock frequency
 *  @return DRAM clock frequency
 */
UINT32 AmbaRTSL_PllGetDramClk(void)
{
    /* gclk_dram = (pll_out_ddr / 2) */
    return ((UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_DDR) >> 1U);
}

/**
 *  AmbaRTSL_PllGetEthernetClk - Get ethernet clock frequency
 *  @return Ethernet clock frequency
 */
UINT32 AmbaRTSL_PllGetEthernetClk(void)
{
    UINT32 Frequency;

    Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_ENET);
    Frequency = Frequency / AmbaCSL_PllGetPostScaleRegVal(AMBA_PLL_ENET);

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetGpioDebounceClk - Get GPIO Debounce clock frequency
 *  @return Debounce clock frequency
 */
UINT32 AmbaRTSL_PllGetGpioDebounceClk(void)
{
    UINT32 Frequency;

    Frequency = AmbaRTSL_PllGetClkRefFreq();
    Frequency = Frequency / AmbaCSL_PllGetGpioDebounceDivider();

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetHdmiPhyClk - Get HDMI PHY clock frequency
 *  @return HDMI PHY clock frequency
 */
static UINT32 AmbaRTSL_PllGetHdmiPhyClk(void)
{
    UINT32 RetVal;
    UINT32 PllOutFreq;
    UINT32 Factor = 1U;

    if (0x0U != AmbaCSL_PllGetHdmiPhyPllSrcSelect()) {
        /* Use HDMI PLL */
        PllOutFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_HDMI);
    } else {
        /* Use HDMI2 PLL */
        PllOutFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_HDMI2);
    }

    if (AmbaCSL_PllGetVoutBClkSrc() == 2U) {
        /* Path for HDMI/CVBS */
        Factor = 10U;
    } else if (AmbaCSL_PllGetVoutBClkSrc() == 0U) {
        /* Path for MIPI CSI */
        Factor = AmbaCSL_PllGetVoutBClkDivider();
    } else if (AmbaCSL_PllGetVoutBClkSrc() == 1U) {
        /* Path for SLVS */
        Factor = 1U;
    } else {
        /* For misraC checking, do nothing */
    }

    RetVal = (PllOutFreq / Factor);

    return RetVal;
}

/**
 *  AmbaRTSL_PllGetIdspClk - Get IDSP clock frequency
 *  @return IDSP clock frequency
 */
UINT32 AmbaRTSL_PllGetIdspClk(void)
{
    UINT32 Frequency;

    Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_IDSP);
    Frequency = Frequency / AmbaCSL_PllGetPostScaleRegVal(AMBA_PLL_IDSP);

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetIrClk - Get IR clock frequency
 *  @return IR clock frequency
 */
UINT32 AmbaRTSL_PllGetIrClk(void)
{
    UINT32 Frequency;

    Frequency = AmbaRTSL_PllGetClkRefFreq();
    Frequency = Frequency / AmbaCSL_PllGetIrDivider();

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetMipiDsiClk - Get MIPI-DSI clock frequency
 *  @return MIPI DSI clock frequency
 */
UINT32 AmbaRTSL_PllGetMipiDsiClk(void)
{
    UINT32 Frequency;

    if (0x0U != AmbaCSL_PllGetVoutComboPhyClkSelect()) {
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_HDMI);
    } else {
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_VIDEO2);
    }

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetMotorClk - Get Motor clock frequency
 *  @return Motor clock frequency
 */
UINT32 AmbaRTSL_PllGetMotorClk(void)
{
    UINT32 Frequency;

    Frequency = AmbaRTSL_PllGetClkRefFreq();
    Frequency = Frequency / AmbaCSL_PllGetMotorDivider();

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetNandClk - Get NAND clock frequency
 *  @return NAND clock frequency
 */
UINT32 AmbaRTSL_PllGetNandClk(void)
{
    return ((UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_NAND));
}

/**
 *  AmbaRTSL_PllGetPwmClk - Get PWM clock frequency
 *  @return PWM clock frequency
 */
UINT32 AmbaRTSL_PllGetPwmClk(void)
{
    UINT32 RefFreq;
    UINT32 Divider = AmbaCSL_PllGetPwmDivider();

    switch (AmbaCSL_PllGetPwmClkSrcSel()) { /* PWM clock source selection (LSB 2-bit) */
    case 0U: /* gclk_apb */
        RefFreq = AmbaRTSL_PllGetApbClk();
        break;

    case 1U: /* gclk_cortex */
        RefFreq = AmbaRTSL_PllGetCortexClk();
        break;

    case 2U: /* pll_out_enet */
        RefFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_ENET);
        break;

    case 3U: /* CLK_REF */
        RefFreq = AmbaRTSL_PllGetClkRefFreq();
        break;

    default:
        RefFreq = 0U;
        break;
    }

    if (0x0U != Divider) {
        RefFreq = RefFreq / Divider;
    } else {
        RefFreq = 0;
    }

    return RefFreq;
}

/**
 *  AmbaRTSL_PllGetSd0Clk - Get SD0 clock frequency
 *  @return SD0 clock frequency
 */
UINT32 AmbaRTSL_PllGetSd0Clk(void)
{
    UINT32 Frequency;
    UINT32 Divider = AmbaCSL_PllGetSd0Postscaler();

    if (0x0U != Divider) {
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SD);
        Frequency = Frequency / Divider;
    } else {
        Frequency = 0;
    }

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetSd1Clk - Get SD1 clock frequency
 *  @return SD1 clock frequency
 */
UINT32 AmbaRTSL_PllGetSd1Clk(void)
{
    UINT32 Frequency;
    UINT32 Divider = AmbaCSL_PllGetSd1Postscaler();

    if (0x0U != Divider) {
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SD);
        Frequency = Frequency / Divider;
    } else {
        Frequency = 0;
    }

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetSensor0Clk - Get Sensor 0 clock frequency
 *  @return Sensor 0 clock frequency
 */
UINT32 AmbaRTSL_PllGetSensor0Clk(void)
{
    UINT32 Frequency;
    UINT32 Divider = AmbaCSL_PllGetPostScaleRegVal(AMBA_PLL_SENSOR0);

    if (0x0U != Divider) {
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SENSOR0);
        Frequency = Frequency / Divider;
    } else {
        Frequency = 0;
    }

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetSensor1Clk - Get Sensor 1 clock frequency
 *  @return Sensor 1 clock frequency
 */
UINT32 AmbaRTSL_PllGetSensor1Clk(void)
{
    return ((UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SENSOR2));
}

/**
 *  AmbaRTSL_PllGetSpiMasterClk - Get SPI master clock frequency
 *  @return SPI master clock frequency
 */
UINT32 AmbaRTSL_PllGetSpiMasterClk(void)
{
    UINT32 RefFreq;
    UINT32 Divider = AmbaCSL_PllGetSpiMasterClkDivider();

    switch (AmbaCSL_PllGetSpiMasterRefClkSelect()) {    /* SPI master clock source selection (LSB 2-bit) */
    case 1: /* gclk_core */
        RefFreq = AmbaRTSL_PllGetCoreClk();
        break;

    default:
        RefFreq = 0U;
        break;
    }

    if (0x0U != Divider) {
        RefFreq = RefFreq / Divider;
    } else {
        RefFreq = 0;
    }

    return RefFreq;
}

/**
 *  AmbaRTSL_PllGetSpiSlaveClk - Get SPI slave clock frequency
 *  @return SPI slave clock frequency
 */
UINT32 AmbaRTSL_PllGetSpiSlaveClk(void)
{
    UINT32 RefFreq;
    UINT32 Divider = AmbaCSL_PllGetSpiSlaveClkDivider();

    switch (AmbaCSL_PllGetSpiSlaveRefClkSelect()) {     /* SPI slave clock source selection (LSB 2-bit) */
    case 1: /* gclk_core */
        RefFreq = (UINT32) AmbaRTSL_PllGetCoreClk();
        break;

    default:
        RefFreq = 0;
        break;
    }

    if (0x0U != Divider) {
        RefFreq = RefFreq / Divider;
    } else {
        RefFreq = 0;
    }

    return RefFreq;
}

/**
 *  AmbaRTSL_PllGetSpiNorClk - Get SPI-NOR clock frequency
 *  @return SPI-NOR clock frequency
 */
UINT32 AmbaRTSL_PllGetSpiNorClk(void)
{
    UINT32 RefFreq;
    UINT32 Divider = AmbaCSL_PllGetSpiNorClkDivider();

    switch (AmbaCSL_PllGetSpiNorRefClkSelect()) {       /* SPI-NOR clock source selection (LSB 2-bit) */
    case 0: /* pll_out_enet */
        RefFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_ENET);
        break;

    case 1: /* gclk_core */
        RefFreq = AmbaRTSL_PllGetCoreClk();
        break;

    case 2: /* gclk_ssi */
        RefFreq = AmbaRTSL_PllGetSpiMasterClk();
        break;

    case 3: /* CLK_REF */
        RefFreq = AmbaRTSL_PllGetClkRefFreq();
        break;

    default:
        RefFreq = 0;
        break;
    }

    if (0x0U != Divider) {
        RefFreq = RefFreq / Divider;
    } else {
        RefFreq = 0;
    }

    return RefFreq;
}

/**
 *  AmbaRTSL_PllGetUartApbClk - Get UART clock frequency
 *  @return UART clock frequency
 */
UINT32 AmbaRTSL_PllGetUartApbClk(void)
{
    UINT32 RefFreq;
    UINT32 Divider = AmbaCSL_PllGetUartApbDivider();

    switch (AmbaCSL_PllGetUartApbClkSrcSel()) { /* UART clock source selection (LSB 2-bit) */
    case 0: /* CLK_REF */
        RefFreq = AmbaRTSL_PllGetClkRefFreq();
        break;

    case 1: /* gclk_core */
        RefFreq = AmbaRTSL_PllGetCortexClk();
        break;

    case 2: /* pll_out_enet */
        RefFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_ENET);
        break;

    case 3: /* pll_out_sd */
        RefFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SD);
        break;

    default:
        RefFreq = 0;
        break;
    }

    if (0x0U != Divider) {
        RefFreq = RefFreq / Divider;
    } else {
        RefFreq = 0;
    }

    return RefFreq;
}

/**
 *  AmbaRTSL_PllGetUart0Clk - Get UART clock frequency
 *  @return UART clock frequency
 */
UINT32 AmbaRTSL_PllGetUart0Clk(void)
{
    UINT32 RefFreq;
    UINT32 Divider = AmbaCSL_PllGetUart0Divider();

    switch (AmbaCSL_PllGetUart0ClkSrcSel()) {   /* UART clock source selection (LSB 2-bit) */
    case 0: /* CLK_REF */
        RefFreq = AmbaRTSL_PllGetClkRefFreq();
        break;

    case 1: /* gclk_core */
        RefFreq = AmbaRTSL_PllGetCortexClk();
        break;

    case 2: /* pll_out_enet */
        RefFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_ENET);
        break;

    case 3: /* pll_out_sd */
        RefFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SD);
        break;

    default:
        RefFreq = 0;
        break;
    }

    if (0x0U != Divider) {
        RefFreq = RefFreq / Divider;
    } else {
        RefFreq = 0;
    }

    return RefFreq;
}


/**
 *  AmbaRTSL_PllGetUart1Clk - Get UART clock frequency
 *  @return UART clock frequency
 */
UINT32 AmbaRTSL_PllGetUart1Clk(void)
{
    UINT32 RefFreq;
    UINT32 Divider = AmbaCSL_PllGetUart1Divider();

    switch (AmbaCSL_PllGetUart1ClkSrcSel()) {   /* UART clock source selection (LSB 2-bit) */
    case 0: /* CLK_REF */
        RefFreq = AmbaRTSL_PllGetClkRefFreq();
        break;

    case 1: /* gclk_core */
        RefFreq = AmbaRTSL_PllGetCortexClk();
        break;

    case 2: /* pll_out_enet */
        RefFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_ENET);
        break;

    case 3: /* pll_out_sd */
        RefFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SD);
        break;

    default:
        RefFreq = 0;
        break;
    }

    if (0x0U != Divider) {
        RefFreq = RefFreq / Divider;
    } else {
        RefFreq = 0;
    }

    return RefFreq;
}

/**
 *  AmbaRTSL_PllGetUart2Clk - Get UART clock frequency
 *  @return UART clock frequency
 */
UINT32 AmbaRTSL_PllGetUart2Clk(void)
{
    UINT32 RefFreq;
    UINT32 Divider = AmbaCSL_PllGetUart2Divider();

    switch (AmbaCSL_PllGetUart2ClkSrcSel()) {   /* UART clock source selection (LSB 2-bit) */
    case 0: /* CLK_REF */
        RefFreq = AmbaRTSL_PllGetClkRefFreq();
        break;

    case 1: /* gclk_core */
        RefFreq = AmbaRTSL_PllGetCortexClk();
        break;

    case 2: /* pll_out_enet */
        RefFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_ENET);
        break;

    case 3: /* pll_out_sd */
        RefFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SD);
        break;

    default:
        RefFreq = 0;
        break;
    }

    if (0x0U != Divider) {
        RefFreq = RefFreq / Divider;
    } else {
        RefFreq = 0;
    }

    return RefFreq;
}

/**
 *  AmbaRTSL_PllGetUart3Clk - Get UART clock frequency
 *  @return UART clock frequency
 */
UINT32 AmbaRTSL_PllGetUart3Clk(void)
{
    UINT32 RefFreq;
    UINT32 Divider = AmbaCSL_PllGetUart3Divider();

    switch (AmbaCSL_PllGetUart3ClkSrcSel()) {   /* UART clock source selection (LSB 2-bit) */
    case 0: /* CLK_REF */
        RefFreq = AmbaRTSL_PllGetClkRefFreq();
        break;

    case 1: /* gclk_core */
        RefFreq = AmbaRTSL_PllGetCortexClk();
        break;

    case 2: /* pll_out_enet */
        RefFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_ENET);
        break;

    case 3: /* pll_out_sd */
        RefFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SD);
        break;

    default:
        RefFreq = 0;
        break;
    }

    if (0x0U != Divider) {
        RefFreq = RefFreq / Divider;
    } else {
        RefFreq = 0;
    }

    return RefFreq;
}

/**
 *  AmbaRTSL_PllGetUsbPhyClk - Get USB-PHY clock frequency
 *  @return USB-PHY clock frequency
 */
UINT32 AmbaRTSL_PllGetUsbPhyClk(void)
{
    return AmbaRTSL_PllGetClkRefFreq();
}

/**
 *  AmbaRTSL_PllGetVin0Clk - Get SoVin clock frequency
 *  @return SoVin clock frequency
 */
UINT32 AmbaRTSL_PllGetVin0Clk(void)
{
    UINT32 Frequency;
    UINT32 Divider = AmbaCSL_PllGetVin0Divider();

    if (0x0U != Divider) {
        /* gclk_so_vin always = pll_out_sensor / scaler_vin */
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SENSOR0);
        Frequency = Frequency / Divider;
    } else {
        Frequency = 0;
    }

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetVin1Clk - Get SoPIP clock frequency
 *  @return SoPIP clock frequency
 */
UINT32 AmbaRTSL_PllGetVin1Clk(void)
{
    UINT32 Frequency;
    UINT32 Divider = AmbaCSL_PllGetVin1Divider();

    if (0x0U != Divider) {
        /* gclk_so_pip always = pll_out_enet / scaler_pip */
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_ENET);
        Frequency = Frequency / Divider;
    } else {
        Frequency = 0;
    }

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetVisionClk - Get Vision clock frequency
 *  @return Vision clock frequency
 */
UINT32 AmbaRTSL_PllGetVisionClk(void)
{
    return ((UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_VISION));
}

/**
 *  AmbaRTSL_PllGetVoutLcdClk - Get VOUT-LCD clock frequency
 *  @return VOUT-LCD clock frequency
 */
UINT32 AmbaRTSL_PllGetVoutLcdClk(void)
{
    UINT32 Frequency;
    UINT32 Divider = AmbaCSL_PllGetPostScaleRegVal(AMBA_PLL_VIDEO2);

    if (pAmbaRCT_Reg->VoutLcdClkSelect.Bits.ClkSrc != 0x0U) {
        /* Use gclk_vo */
        Frequency = AmbaRTSL_PllGetVoutTvClk();
    } else {
        if (0x0U != Divider) {
            /* Use gclk_vo2 */
            Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_VIDEO2);
            Frequency = Frequency / Divider;
        } else {
            Frequency = 0;
        }
    }

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetVoutTvClk - Get VOUT-TV clock frequency
 *  @return VOUT-TV clock frequency
 */
UINT32 AmbaRTSL_PllGetVoutTvClk(void)
{
    UINT32 Frequency;

    if (0x0U != AmbaCSL_PllGetHdmiUsePhyClkVo()) {
        /* Use clk160d */
        Frequency = AmbaRTSL_PllGetHdmiPhyClk();
    } else {
        /* Use pll_out_hdmi */
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_HDMI);
    }

    return Frequency;
}
