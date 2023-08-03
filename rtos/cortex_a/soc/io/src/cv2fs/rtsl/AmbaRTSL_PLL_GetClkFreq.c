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
    return ((UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_CORTEX1) / 3U);
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
        RefFreq = 0;
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
 *  AmbaRTSL_PllGetCortex0Clk - Get Cortex clock frequency
 *  @return Cortex clock frequency
 */
UINT32 AmbaRTSL_PllGetCortex0Clk(void)
{
    UINT32 RetVal = 0U;

    if (0x0U != (AmbaCSL_PllGetCortex0ClkSrcSelect() & 1U)) {
        /* gclk_cortex = ref_clk */
        RetVal = AmbaRTSL_PllGetClkRefFreq();
    } else {
        /* gclk_cortex = pll_out_cortex */
        RetVal = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_CORTEX0);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllGetCortexClk - Get Cortex clock frequency
 *  @return Cortex clock frequency
 */
UINT32 AmbaRTSL_PllGetCortex1Clk(void)
{
    UINT32 RetVal = 0U;

    if (0x0U != (AmbaCSL_PllGetCortex1ClkSrcSelect() & 1U)) {
        /* gclk_cortex = ref_clk */
        RetVal = AmbaRTSL_PllGetClkRefFreq();
    } else {
        /* gclk_cortex = pll_out_cortex */
        RetVal = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_CORTEX1);
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
        Frequency = 0;
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
    UINT32 Divider = AmbaCSL_PllGetPostScaleRegVal(AMBA_PLL_ENET);

    if (0x0U != Divider) {
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_ENET);
        Frequency = Frequency / Divider;
    } else {
        Frequency = 0;
    }

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetEthernet1Clk - Get ethernet1 clock frequency
 *  @return Ethernet clock frequency
 */
UINT32 AmbaRTSL_PllGetEthernet1Clk(void)
{
    UINT32 Frequency;
    UINT32 Divider = AmbaCSL_PllGetEnet1Postscaler();

    if (0x0U != Divider) {
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_ENET);
        Frequency = Frequency / Divider;
    } else {
        Frequency = 0;
    }

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetFexClk - Get Fex clock frequency
 *  @return Fex clock frequency
 */
UINT32 AmbaRTSL_PllGetFexClk(void)
{
    return ((UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_FEX));
}

/**
 *  AmbaRTSL_PllGetIdspClk - Get IDSP clock frequency
 *  @return IDSP clock frequency
 */
UINT32 AmbaRTSL_PllGetIdspClk(void)
{
    UINT32 Frequency;
    UINT32 Divider = AmbaCSL_PllGetPostScaleRegVal(AMBA_PLL_IDSP);

    if (0x0U != Divider) {
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_IDSP);
        Frequency = Frequency / Divider;
    } else {
        Frequency = 0;
    }

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetMipiDsiClk - Get MIPI-DSI0 clock frequency
 *  @return MIPI DSI0 clock frequency
 */
UINT32 AmbaRTSL_PllGetMipiDsi0Clk(void)
{
    UINT32 Frequency;

    if (0x0U != AmbaCSL_PllGetVoutComboPhy0ClkSel()) {
        Frequency = 0U;
    } else {
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_VIDEO_A);
    }

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetMipiDsiClk - Get MIPI-DSI1 clock frequency
 *  @return MIPI DSI1 clock frequency
 */
UINT32 AmbaRTSL_PllGetMipiDsi1Clk(void)
{
    UINT32 Frequency;

    if (0x0U != AmbaCSL_PllGetVoutComboPhy1ClkSel()) {
        Frequency = 0U;
    } else {
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_VIDEO_B);
    }

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

    case 1U: /* gclk_cortex1 */
        RefFreq = AmbaRTSL_PllGetCortex1Clk();
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
    UINT32 Frequency;
    UINT32 Divider = AmbaCSL_PllGetPostScaleRegVal(AMBA_PLL_SENSOR2);

    if (0x0U != Divider) {
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SENSOR2);
        Frequency = Frequency / Divider;
    } else {
        Frequency = 0;
    }

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetSensor1Clk - Get Sensor 2 clock frequency
 *  @return Sensor 2 clock frequency
 */
UINT32 AmbaRTSL_PllGetSensor2Clk(void)
{
    return AmbaRTSL_PllGetEthernetClk();
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
        RefFreq = AmbaRTSL_PllGetCoreClk();
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
        RefFreq = AmbaRTSL_PllGetCoreClk();
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
        RefFreq = AmbaRTSL_PllGetCoreClk();
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
        RefFreq = AmbaRTSL_PllGetCoreClk();
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
        RefFreq = AmbaRTSL_PllGetCoreClk();
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
    UINT32 Frequency = 0U;
    UINT32 Divider = AmbaCSL_PllGetVin1Divider();

    switch (AmbaCSL_PllGetVin1ClkSelect()) {
    case 0:
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SENSOR2);
        break;
    case 1:
        Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SENSOR0);
        break;
    default:
        /* for misraC check, do nothing */
        break;
    }

    if (0x0U != Divider) {
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
 *  AmbaRTSL_PllGetVout0Clk - Get VOUT-0 clock frequency
 *  @return VOUT-0 clock frequency
 */
UINT32 AmbaRTSL_PllGetVout0Clk(void)
{
    UINT32 Frequency;
    UINT32 Divider = AmbaCSL_PllGetPostScaleRegVal(AMBA_PLL_VIDEO_A);

    if (pAmbaRCT_Reg->VoutClkSelect.Vout0ClkSelect != 0x0U) {
        /* Use gclk_vo_b */
        Frequency = AmbaRTSL_PllGetMipiDsi0Clk();
    } else {
        if (0x0U != Divider) {
            /* Use gclk_vo_a */
            Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_VIDEO_A);
            Frequency = Frequency / Divider;
        } else {
            Frequency = 0;
        }
    }

    return Frequency;
}

/**
 *  AmbaRTSL_PllGetVout1Clk - Get VOUT-1 clock frequency
 *  @return VOUT-1 clock frequency
 */
UINT32 AmbaRTSL_PllGetVout1Clk(void)
{
    UINT32 Frequency;
    UINT32 Divider = AmbaCSL_PllGetPostScaleRegVal(AMBA_PLL_VIDEO_B);

    if (pAmbaRCT_Reg->VoutClkSelect.Vout1ClkSelect != 0x0U) {
        /* Use gclk_vo_a */
        Frequency = AmbaRTSL_PllGetMipiDsi1Clk();
    } else {
        if (0x0U != Divider) {
            /* Use gclk_vo_b */
            Frequency = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_VIDEO_B);
            Frequency = Frequency / Divider;
        } else {
            Frequency = 0;
        }
    }

    return Frequency;
}
