/**
 * @file AmbaCSL_VOUT.c
 *
 * Copyright (c) 2021 Ambarella International LP
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

#include "AmbaDef.h"

#include "AmbaCSL_VOUT.h"

/* Memory Map Registers Definition */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
AMBA_VOUT_REG_s *                   pAmbaVout_Reg;
AMBA_VOUT_TVENC_CONTROL_REG_s *     pAmbaVoutTvEnc_Reg;
AMBA_VOUT_MIXER_REG_s *             pAmbaVoutMixer0_Reg;
AMBA_VOUT_DISPLAY_CONFIG_REG_s *    pAmbaVoutDisplay0_Reg;
AMBA_VOUT_MIXER_REG_s *             pAmbaVoutMixer1_Reg;
AMBA_VOUT_DISPLAY_CONFIG_REG_s *    pAmbaVoutDisplay1_Reg;
AMBA_VOUT_OSD_RESCALE_REG_s *       pAmbaVoutOsdRescale_Reg;
AMBA_VOUT_TOP_REG_s *               pAmbaVoutTop_Reg;
AMBA_VOUT_MIPI_DSI_COMMAND_REG_s *  pAmbaVoutMipiDsiCmd0_Reg;
AMBA_VOUT_DISPLAY_C_CONFIG_REG_s *  pAmbaVoutDisplay2_Reg;
AMBA_VOUT_MIPI_DSI_COMMAND_REG_s *  pAmbaVoutMipiDsiCmd1_Reg;
#else
AMBA_VOUT_REG_s *                   pAmbaVout_Reg;
AMBA_VOUT_TVENC_CONTROL_REG_s *     pAmbaVoutTvEnc_Reg;
AMBA_VOUT_MIXER_REG_s *             pAmbaVoutMixer0_Reg;
AMBA_VOUT_DISPLAY0_CONFIG_REG_s *   pAmbaVoutDisplay0_Reg;
#if !defined(CONFIG_SOC_CV28)
AMBA_VOUT_MIXER_REG_s *             pAmbaVoutMixer1_Reg;
AMBA_VOUT_DISPLAY1_CONFIG_REG_s *   pAmbaVoutDisplay1_Reg;
#endif
AMBA_VOUT_OSD_RESCALE_REG_s *       pAmbaVoutOsdRescale_Reg;
AMBA_VOUT_TOP_REG_s *               pAmbaVoutTop_Reg;
#if !defined(CONFIG_SOC_H32)
AMBA_VOUT_MIPI_DSI_COMMAND_REG_s *  pAmbaVoutMipiDsiCmd_Reg;
#endif
#endif


#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
/**
 *  AmbaCSL_VoutCsiPhyPowerUp - Configure MIPI CSI Phy to power up
 *  @param[in] VoutChannel Target vout channel
 *  @return error code
 */
UINT32 AmbaCSL_VoutCsiPhyPowerUp(UINT32 VoutChannel, UINT32 ParamVal)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (VoutChannel == AMBA_VOUT_CHANNEL0) {
        /* Reset digital phy */
        /* DSI_CTRL_REG2[1]=1 (soft reset) */
        AmbaCSL_Vout0MipiDsiSetCtrl2(0x10000002);
        /* Pre-init for send mipi command */
        AmbaCSL_Vout0MipiDsiSetCtrl0(0x61240313);
        AmbaCSL_Vout0MipiDsiSetCtrl1(0x62007207);
        AmbaCSL_Vout0MipiDsiSetCtrl2(0x9000001fU);
        AmbaCSL_Vout0MipiDsiSetCtrl2(0x1000001f);
        AmbaCSL_Vout0MipiDsiSetAuxEn(1);
        AmbaCSL_Vout0MipiDsiSetPreEn(1);
        /* Default pre-emphasis */
        AmbaCSL_Vout0MipiDsiSetPre(1);
        AmbaCSL_Vout0MipiDsiSetTxMode(MIPI_DSI_PHY_MODE_CSI);

        /* MIPI continuous mode */
        if (ParamVal == VOUT_MIPI_DPHY_CONT_CLK) {
            AmbaCSL_Vout0MipiDsiSetClkMode(1U);
        } else {
            AmbaCSL_Vout0MipiDsiSetClkMode(0U);
        }

    } else if (VoutChannel == AMBA_VOUT_CHANNEL1) {
        /* Reset digital phy */
        /* DSI_CTRL_REG2[1]=1 (soft reset) */
        AmbaCSL_Vout1MipiDsiSetCtrl2(0x10000002);
        /* Pre-init for send mipi command */
        AmbaCSL_Vout1MipiDsiSetCtrl0(0x61240313);
        AmbaCSL_Vout1MipiDsiSetCtrl1(0x62007207);
        AmbaCSL_Vout1MipiDsiSetCtrl2(0x9000001fU);
        AmbaCSL_Vout1MipiDsiSetCtrl2(0x1000001f);
        AmbaCSL_Vout1MipiDsiSetAuxEn(1);
        AmbaCSL_Vout1MipiDsiSetPreEn(1);
        /* Default pre-emphasis */
        AmbaCSL_Vout1MipiDsiSetPre(1);
        AmbaCSL_Vout1MipiDsiSetTxMode(MIPI_DSI_PHY_MODE_CSI);

        /* MIPI continuous mode */
        if (ParamVal == VOUT_MIPI_DPHY_CONT_CLK) {
            AmbaCSL_Vout1MipiDsiSetClkMode(1U);
        } else {
            AmbaCSL_Vout1MipiDsiSetClkMode(0U);
        }
    } else {
        RetVal = VOUT_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VoutCsiPhyPowerDown - Configure MIPI CSI Phy to power down
 *  @param[in] VoutChannel Target vout channel
 *  @return error code
 */
UINT32 AmbaCSL_VoutCsiPhyPowerDown(UINT32 VoutChannel)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (VoutChannel == AMBA_VOUT_CHANNEL0) {
        /* Disable analog phy */
        AmbaCSL_Vout0MipiDsiSetCtrl0(0x61240210);
        AmbaCSL_Vout0MipiDsiSetCtrl2(0x10000000);
        /* Reset digital phy */
        AmbaCSL_Vout0MipiDsiSetCtrl2(0x10000002);

    } else if (VoutChannel == AMBA_VOUT_CHANNEL1) {
        /* Disable analog phy */
        AmbaCSL_Vout1MipiDsiSetCtrl0(0x61240210);
        AmbaCSL_Vout1MipiDsiSetCtrl2(0x10000000);
        /* Reset digital phy */
        AmbaCSL_Vout1MipiDsiSetCtrl2(0x10000002);

    } else {
        RetVal = VOUT_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VoutDsiPhyPowerUp - Configure MIPI DSI Phy to power up
 *  @param[in] VoutChannel Target vout channel
 *  @return error code
 */
UINT32 AmbaCSL_VoutDsiPhyPowerUp(UINT32 VoutChannel, UINT32 ParamVal)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (VoutChannel == AMBA_VOUT_CHANNEL0) {
        /* Reset digital phy */
        /* DSI_CTRL_REG2[1]=1 (soft reset) */
        AmbaCSL_Vout0MipiDsiSetCtrl2(0x10000002);
        /* Pre-init for send mipi dsi command */
        AmbaCSL_Vout0MipiDsiSetCtrl0(0x61200313);
        AmbaCSL_Vout0MipiDsiSetCtrl1(0x60007207);
        AmbaCSL_Vout0MipiDsiSetCtrl2(0x1000001f);
        AmbaCSL_Vout0MipiDsiSetAuxEn(0);
        AmbaCSL_Vout0MipiDsiSetPreEn(0);
        /* Default pre-emphasis */
        AmbaCSL_Vout0MipiDsiSetPre(0);
        AmbaCSL_Vout0MipiDsiSetTxMode(MIPI_DSI_PHY_MODE_DSI);
        /* MIPI DSI clock mode */
        AmbaCSL_Vout0MipiDsiSetClkMode(ParamVal);

    } else if (VoutChannel == AMBA_VOUT_CHANNEL1) {
        /* Reset digital phy */
        AmbaCSL_Vout1MipiDsiSetCtrl2(0x10000002);
        /* Pre-init for send mipi dsi command */
        AmbaCSL_Vout1MipiDsiSetCtrl0(0x61200313);
        AmbaCSL_Vout1MipiDsiSetCtrl1(0x60007207);
        AmbaCSL_Vout1MipiDsiSetCtrl2(0x1000001f);
        AmbaCSL_Vout1MipiDsiSetAuxEn(0);
        AmbaCSL_Vout1MipiDsiSetPreEn(0);
        /* Default pre-emphasis */
        AmbaCSL_Vout1MipiDsiSetPre(0);
        AmbaCSL_Vout1MipiDsiSetTxMode(MIPI_DSI_PHY_MODE_DSI);
        /* MIPI DSI clock mode */
        AmbaCSL_Vout1MipiDsiSetClkMode(ParamVal);

    } else {
        RetVal = VOUT_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VoutDsiPhyPowerDown - Configure MIPI DSI Phy to power down
 *  @param[in] VoutChannel Target vout channel
 *  @return error code
 */
UINT32 AmbaCSL_VoutDsiPhyPowerDown(UINT32 VoutChannel)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (VoutChannel == AMBA_VOUT_CHANNEL0) {
        /* Disable analog phy */
        AmbaCSL_Vout0MipiDsiSetCtrl0(0x61240210);
        AmbaCSL_Vout0MipiDsiSetCtrl2(0x10000000);
        /* Reset digital phy */
        AmbaCSL_Vout0MipiDsiSetCtrl2(0x10000002);

    } else if (VoutChannel == AMBA_VOUT_CHANNEL1) {
        /* Disable analog phy */
        AmbaCSL_Vout1MipiDsiSetCtrl0(0x61240210);
        AmbaCSL_Vout1MipiDsiSetCtrl2(0x10000000);
        /* Reset digital phy */
        AmbaCSL_Vout1MipiDsiSetCtrl2(0x10000002);

    } else {
        RetVal = VOUT_ERR_ARG;
    }

    return RetVal;
}

static UINT32 BtaEnable = MIPI_DSI_BTA_ENABLE_FROM_RCT;
void AmbaCSL_VoutMipiDsiSetBtaSrc(UINT32 BtaEnSrc)
{
    BtaEnable = BtaEnSrc;
}

UINT32 AmbaCSL_VoutMipiDsiGetAckData(UINT32 VoutChannel, UINT32 *pAckData)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    /* Trigger BTA flow, need read commands to be sent before BTA flow. */
    if (VoutChannel == AMBA_VOUT_CHANNEL0) {
        AmbaCSL_Vout0MipiDsiObsvSel(5);

        /* Trigger BTA */
        if (BtaEnable == MIPI_DSI_BTA_ENABLE_FROM_RCT) {
            AmbaCSL_Vout0MipiDsiBtaEnable(1);
            AmbaDelayCycles(0xfffU);
        }

        /* Clear overflow */
        AmbaCSL_Vout0ClearStatus();

        /* Get ack data */
        AmbaCSL_Vout0MipiDsiBtaEnable(0);
        AmbaDelayCycles(0xfffU);
        *pAckData = AmbaCSL_Vout0MipiDsiGetAckData();

        if (*pAckData != 0x84U) {
            /* Trigger dsi rct reset */
            AmbaCSL_Vout0MipiDsiSetCtrl2(0x9000001fU);
            AmbaDelayCycles(0xfffU);
            AmbaCSL_Vout0MipiDsiSetCtrl2(0x1000001f);
        }

        /* Clear received data */
        AmbaCSL_Vout0MipiDsiBtaClrData(1);
        AmbaDelayCycles(0xfffU);
        AmbaCSL_Vout0MipiDsiBtaClrData(0);
    } else if (VoutChannel == AMBA_VOUT_CHANNEL1) {
        AmbaCSL_Vout1MipiDsiObsvSel(5);

        /* Trigger BTA */
        if (BtaEnable == MIPI_DSI_BTA_ENABLE_FROM_RCT) {
            AmbaCSL_Vout1MipiDsiBtaEnable(1);
            AmbaDelayCycles(0xfffU);
        }

        /* Clear overflow */
        AmbaCSL_Vout1ClearStatus();

        /* Get ack data */
        AmbaCSL_Vout1MipiDsiBtaEnable(0);
        AmbaDelayCycles(0xfffU);
        *pAckData = AmbaCSL_Vout1MipiDsiGetAckData();

        if (*pAckData != 0x84U) {
            /* Trigger dsi rct reset */
            AmbaCSL_Vout1MipiDsiSetCtrl2(0x9000001fU);
            AmbaDelayCycles(0xfffU);
            AmbaCSL_Vout1MipiDsiSetCtrl2(0x1000001f);
        }

        /* Clear received data */
        AmbaCSL_Vout1MipiDsiBtaClrData(1);
        AmbaDelayCycles(0xfffU);
        AmbaCSL_Vout1MipiDsiBtaClrData(0);
    } else {
        RetVal = VOUT_ERR_ARG;
    }

    return RetVal;
}
#else
/* cv2x */
#if !defined(CONFIG_SOC_H32)
/**
 *  AmbaCSL_VoutCsiPhyPowerUp - Configure MIPI CSI Phy to power up
 *  @return error code
 */
UINT32 AmbaCSL_VoutCsiPhyPowerUp(UINT32 ParamVal)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    /* Reset digital phy */
    AmbaCSL_VoutMipiDsiSetCtrl2(0x17000002);

    /* Pre-init for send mipi command */
    AmbaCSL_VoutMipiDsiSetCtrl0(0x61300313);
    AmbaCSL_VoutMipiDsiSetCtrl1(0x00000007);
#if defined(CONFIG_SOC_CV28)
    AmbaCSL_VoutMipiDsiSetCtrl2(0x1700001f);
#else
    AmbaCSL_VoutMipiDsiSetCtrl2(0x170000f1);
#endif
    AmbaCSL_VoutMipiDsiSetAuxCtrl(0x00000013);

    /* MIPI continuous mode */
    if (ParamVal == VOUT_MIPI_DPHY_CONT_CLK) {
        AmbaCSL_VoutMipiDsiSetClkMode(1U);
    } else {
        AmbaCSL_VoutMipiDsiSetClkMode(0U);
    }

    return RetVal;
}

/**
 *  AmbaCSL_VoutCsiPhyPowerDown - Configure MIPI CSI Phy to power down
 *  @return error code
 */
UINT32 AmbaCSL_VoutCsiPhyPowerDown(void)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    /* Disable analog phy */
    AmbaCSL_VoutMipiDsiSetCtrl0(0x61300210);
    AmbaCSL_VoutMipiDsiSetCtrl2(0x17000000);

    /* Reset digital phy */
    AmbaCSL_VoutMipiDsiSetCtrl2(0x17000002);

    return RetVal;
}

/**
 *  AmbaCSL_VoutDsiPhyPowerUp - Configure MIPI DSI Phy to power up
 *  @return error code
 */
UINT32 AmbaCSL_VoutDsiPhyPowerUp(UINT32 ParamVal)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    /* Reset digital phy */
    /* DSI_CTRL_REG2[1]=1 (soft reset) */
    AmbaCSL_VoutMipiDsiSetCtrl2(0x17000002);

    /* Pre-init for send mipi dsi command */
    /* Default PHY value */
    AmbaCSL_VoutMipiDsiSetCtrl0(0x61300313);
    AmbaCSL_VoutMipiDsiSetCtrl1(0x00000007);
#if defined(CONFIG_SOC_CV28)
    AmbaCSL_VoutMipiDsiSetCtrl2(0x1700001f);
#else
    AmbaCSL_VoutMipiDsiSetCtrl2(0x170000f1);
#endif
    AmbaCSL_VoutMipiDsiSetAuxCtrl(0x00000013);

    /* MIPI DSI clock mode */
    AmbaCSL_VoutMipiDsiSetClkMode(ParamVal);

    return RetVal;
}

/**
 *  AmbaCSL_VoutDsiPhyPowerDown - Configure MIPI DSI Phy to power down
 *  @return error code
 */
UINT32 AmbaCSL_VoutDsiPhyPowerDown(void)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    /* Disable analog phy */
    AmbaCSL_VoutMipiDsiSetCtrl0(0x61300210);
    AmbaCSL_VoutMipiDsiSetCtrl2(0x17000000);

    /* Reset digital phy */
    AmbaCSL_VoutMipiDsiSetCtrl2(0x17000002);

    return RetVal;
}
#endif
#if defined(CONFIG_SOC_CV28)
static UINT32 BtaEnable = MIPI_DSI_BTA_ENABLE_FROM_RCT;
void AmbaCSL_VoutMipiDsiSetBtaSrc(UINT32 BtaEnSrc)
{
    BtaEnable = BtaEnSrc;
}

UINT32 AmbaCSL_VoutMipiDsiGetAckData(UINT32 *pAckData)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    /* Trigger BTA flow, need read commands to be sent before BTA flow. */
    AmbaCSL_VoutMipiDsiSetAuxCtrl(0x01000013);
    AmbaDelayCycles(0xfffU);

    if (BtaEnable == MIPI_DSI_BTA_ENABLE_FROM_RCT) {
        AmbaCSL_VoutMipiDsiSetAuxCtrl(0x05004013);
        AmbaDelayCycles(0xfffU);
    }

    /* Get ack data */
    AmbaCSL_VoutMipiDsiSetAuxCtrl(0x05000013);
    AmbaDelayCycles(0xfffU);
    *pAckData = AmbaCSL_VoutMipiDsiGetAckReg();

    if (*pAckData != 0x84U) {
        /* Trigger dsi rct reset */
        AmbaCSL_VoutMipiDsiSetCtrl2(0x9700001fU);
        AmbaDelayCycles(0xfffU);
        AmbaCSL_VoutMipiDsiSetCtrl2(0x1700001f);
    }

    /* Clear received data */
    AmbaCSL_VoutMipiDsiSetAuxCtrl(0x13);
    AmbaCSL_VoutMipiDsiSetAuxCtrl(0x30000013);
    AmbaDelayCycles(0xfffU);
    AmbaCSL_VoutMipiDsiSetAuxCtrl(0x13);

    return RetVal;
}
#endif
#endif
