/**
 *  @file AmbaCSL_VIN.c
 *
 *  @copyright Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details VIN CSL
 *
 */

#include "AmbaWrap.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"

#include "AmbaCSL_RCT.h"
#include "AmbaCSL_VIN.h"
#include "AmbaCSL_DebugPort.h"
#include "AmbaCSL_PLL.h"

AMBA_MIPI_REG_s *pAmbaMIPI_Reg;
AMBA_DBG_PORT_IDSP_CONTROLLER_REG_s *pAmbaIDSP_CtrlReg;
AMBA_DBG_PORT_IDSP_VIN_MAIN_REG_s *pAmbaIDSP_VinMainReg;
AMBA_DBG_PORT_IDSP_VIN_GLOBAL_REG_s *pAmbaIDSP_VinGlobalReg;
AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_REG_s *pAmbaIDSP_VinMasterSyncReg;

static inline void WriteWord(volatile UINT32 *Addr, UINT16 d)
{
    (*((volatile UINT32 *) (Addr)) = (d));
}

#ifdef CONFIG_QNX
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_VinMainConfigData;
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin2ndMainConfigData;
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin3rdMainConfigData;
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin4thMainConfigData;
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin5thMainConfigData;
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin6thMainConfigData;
#else
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_VinMainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin2ndMainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin3rdMainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin4thMainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin5thMainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin6thMainConfigData __attribute__((aligned(128)));
#endif

static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_VinGlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin2ndGlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin3rdGlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin4thGlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin5thGlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin6thGlobalConfigData;
static AMBA_CSL_VIN_MASTER_SYNC_CONFIG_DATA_s AmbaCSL_VinMSync0ConfigData;
static AMBA_CSL_VIN_MASTER_SYNC_CONFIG_DATA_s AmbaCSL_VinMSync1ConfigData;

static AMBA_CSL_VIN_INFO_s AmbaCSL_VinInfo;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin2ndInfo;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin3rdInfo;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin4thInfo;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin5thInfo;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin6thInfo;

static AMBA_CSL_VIN_CONFIG_DATA_s* const pMainConfigData[AMBA_NUM_VIN_CHANNEL] = {
    &AmbaCSL_VinMainConfigData,
    &AmbaCSL_Vin2ndMainConfigData,
    &AmbaCSL_Vin3rdMainConfigData,
    &AmbaCSL_Vin4thMainConfigData,
    &AmbaCSL_Vin5thMainConfigData,
    &AmbaCSL_Vin6thMainConfigData,
};

static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s* const pGlobalConfigData[AMBA_NUM_VIN_CHANNEL] = {
    &AmbaCSL_VinGlobalConfigData,
    &AmbaCSL_Vin2ndGlobalConfigData,
    &AmbaCSL_Vin3rdGlobalConfigData,
    &AmbaCSL_Vin4thGlobalConfigData,
    &AmbaCSL_Vin5thGlobalConfigData,
    &AmbaCSL_Vin6thGlobalConfigData,
};

static AMBA_CSL_VIN_MASTER_SYNC_CONFIG_DATA_s* const pMSyncConfigData[AMBA_NUM_VIN_MSYNC] = {
    &AmbaCSL_VinMSync0ConfigData,
    &AmbaCSL_VinMSync1ConfigData,
};

static AMBA_CSL_VIN_INFO_s* const pVinInformation[AMBA_NUM_VIN_CHANNEL] = {
    &AmbaCSL_VinInfo,
    &AmbaCSL_Vin2ndInfo,
    &AmbaCSL_Vin3rdInfo,
    &AmbaCSL_Vin4thInfo,
    &AmbaCSL_Vin5thInfo,
    &AmbaCSL_Vin6thInfo,
};

/**
 *  AmbaCSL_VinSetVinLvdsPadMode - Configure LVDS pad mode
 *  @param[in] PadMode Indicate pad mode
 *  @param[in] DataLaneEnable Enabled data pins defined in bit-wise
 *  @return error code
 */
UINT32 AmbaCSL_VinSetVinLvdsPadMode(UINT32 PadMode, UINT32 DataLaneEnable)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 DataLaneEnableVal = DataLaneEnable;
    UINT32 SubGrpEnable = 0U;
    UINT32 SubGrpEnable_SLVS = 0U;

    switch (PadMode) {
    case AMBA_VIN_PAD_MODE_SLVS:
        if ((DataLaneEnableVal & 0x00fU) != 0U) {
            SubGrpEnable |= 0x1U;
        }
        if ((DataLaneEnableVal & 0x0f0U) != 0U) {
            SubGrpEnable |= 0x2U;
        }
        if ((DataLaneEnableVal & 0xf00U) != 0U) {
            SubGrpEnable |= 0x4U;
        }

        /* bit mode=0: latch data with local clock(spclk1/2/3), bit mode=1: latch data with spclk0 */
        /* bit 0 has to be 0 in cv22/cv2 */
        SubGrpEnable_SLVS = SubGrpEnable & ~0x1U;
        AmbaCSL_VinSetDphy0LvdsBitMode(AmbaCSL_VinGetDphy0LvdsBitMode() | SubGrpEnable_SLVS);

        DataLaneEnableVal = DataLaneEnableVal & 0xfffU;
        AmbaCSL_VinSetDphy0LvCmosMode(AmbaCSL_VinGetDphy0LvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetDphy0LvCmosModeSpclk(AmbaCSL_VinGetDphy0LvCmosModeSpclk() & ~SubGrpEnable);
        AmbaCSL_VinSetDphy0MipiMode(AmbaCSL_VinGetDphy0MipiMode() & ~SubGrpEnable);
        break;
    case AMBA_VIN_PAD_MODE_DVP_LVCMOS:
        AmbaCSL_VinSetDphy0LvdsBitMode(0x0U);

        AmbaCSL_VinSetDphy0LvCmosMode(AmbaCSL_VinGetDphy0LvCmosMode() | 0xffU);
        AmbaCSL_VinSetDphy0LvCmosModeSpclk(AmbaCSL_VinGetDphy0LvCmosModeSpclk() | 0x3U);
        AmbaCSL_VinSetDphy0MipiMode(0x0U);
        break;
    case AMBA_VIN_PAD_MODE_DVP_LVDS:
        AmbaCSL_VinSetDphy0LvdsBitMode(0x6U);

        AmbaCSL_VinSetDphy0LvCmosMode(0x0U);
        AmbaCSL_VinSetDphy0LvCmosModeSpclk(0x6U);
        AmbaCSL_VinSetDphy0MipiMode(0x0U);
        break;
    case AMBA_VIN_PAD_MODE_MIPI:
        if ((DataLaneEnableVal & 0x00ffU) == 0x00ffU) {
            AmbaCSL_VinSetDphy0LvdsBitMode(0x2U);
            AmbaCSL_VinSetDphy0MipiMode(0x3U);
            AmbaCSL_VinSetDphy0EightLaneMipiMode(0x1U);

            AmbaCSL_VinSetDphy0LvCmosMode(AmbaCSL_VinGetDphy0LvCmosMode() & ~0x0ffU);
            AmbaCSL_VinSetDphy0LvCmosModeSpclk(AmbaCSL_VinGetDphy0LvCmosModeSpclk() & ~0x3U);
        } else {
            AmbaCSL_VinSetDphy0LvdsBitMode(AmbaCSL_VinGetDphy0LvdsBitMode() & ~0x1U);
            AmbaCSL_VinSetDphy0MipiMode(AmbaCSL_VinGetDphy0MipiMode() | 0x1U);
            AmbaCSL_VinSetDphy0EightLaneMipiMode(0x0U);

            DataLaneEnableVal = DataLaneEnableVal & 0x00fU;
            AmbaCSL_VinSetDphy0LvCmosMode(AmbaCSL_VinGetDphy0LvCmosMode() & ~DataLaneEnableVal);
            AmbaCSL_VinSetDphy0LvCmosModeSpclk(AmbaCSL_VinGetDphy0LvCmosModeSpclk() & ~0x1U);
        }
        break;
    default:
        RetVal = VIN_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinSetPipLvdsPadMode - Configure LVDS pad mode
 *  @param[in] PadMode Indicate pad mode
 *  @param[in] DataLaneEnable Enabled data pins defined in bit-wise
 *  @return error code
 */
UINT32 AmbaCSL_VinSetPipLvdsPadMode(UINT32 PadMode, UINT32 DataLaneEnable)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 DataLaneEnableVal = (DataLaneEnable >> 12U);
    UINT32 SubGrpEnable = 0U;
    UINT32 SubGrpEnable_SLVS = 0U;

    switch (PadMode) {
    case AMBA_VIN_PAD_MODE_SLVS:
        if ((DataLaneEnableVal & 0x00fU) != 0U) {
            SubGrpEnable |= 0x1U;
        }
        if ((DataLaneEnableVal & 0x0f0U) != 0U) {
            SubGrpEnable |= 0x2U;
        }
        if ((DataLaneEnableVal & 0xf00U) != 0U) {
            SubGrpEnable |= 0x4U;
        }

        /* bit mode=0: latch data with local clock(spclk1/2/3), bit mode=1: latch data with spclk0 */
        /* bit 0 has to be 0 in cv22/cv2 */
        SubGrpEnable_SLVS = SubGrpEnable & ~0x1U;
        AmbaCSL_VinSetDphy1LvdsBitMode(AmbaCSL_VinGetDphy1LvdsBitMode() | SubGrpEnable_SLVS);

        DataLaneEnableVal = DataLaneEnableVal & 0xfffU;
        AmbaCSL_VinSetDphy1LvCmosMode(AmbaCSL_VinGetDphy1LvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetDphy1LvCmosModeSpclk(AmbaCSL_VinGetDphy1LvCmosModeSpclk() & ~SubGrpEnable);
        AmbaCSL_VinSetDphy1MipiMode(AmbaCSL_VinGetDphy1MipiMode() & ~SubGrpEnable);
        break;
    case AMBA_VIN_PAD_MODE_DVP_LVCMOS:
        AmbaCSL_VinSetDphy1LvdsBitMode(0x0U);

        AmbaCSL_VinSetDphy1LvCmosMode(AmbaCSL_VinGetDphy1LvCmosMode() | 0xffU);
        AmbaCSL_VinSetDphy1LvCmosModeSpclk(AmbaCSL_VinGetDphy1LvCmosModeSpclk() | 0x3U);
        AmbaCSL_VinSetDphy1MipiMode(0x0U);

        AmbaCSL_VinSetDphy1MipiCtrl0(AmbaCSL_VinGetDphy1MipiCtrl0() | 0x00000330U);
        AmbaDelayCycles(0xfffU);
        AmbaCSL_VinSetDphy1MipiCtrl0(AmbaCSL_VinGetDphy1MipiCtrl0() & ~0x00000330U);
        break;
    case AMBA_VIN_PAD_MODE_DVP_LVDS:
        AmbaCSL_VinSetDphy1LvdsBitMode(0x6U);

        AmbaCSL_VinSetDphy1LvCmosMode(0x0U);
        AmbaCSL_VinSetDphy1LvCmosModeSpclk(0x6U);
        AmbaCSL_VinSetDphy1MipiMode(0x0U);

        AmbaCSL_VinSetDphy1MipiCtrl0(AmbaCSL_VinGetDphy1MipiCtrl0() | 0x00000770U);
        AmbaDelayCycles(0xfffU);
        AmbaCSL_VinSetDphy1MipiCtrl0(AmbaCSL_VinGetDphy1MipiCtrl0() & ~0x00000770U);
        break;
    case AMBA_VIN_PAD_MODE_MIPI:
        if ((DataLaneEnableVal & 0x00ffU) == 0x00ffU) {
            AmbaCSL_VinSetDphy1LvdsBitMode(0x2U);
            AmbaCSL_VinSetDphy1MipiMode(0x3U);
            AmbaCSL_VinSetDphy1EightLaneMipiMode(0x1U);

            AmbaCSL_VinSetDphy1LvCmosMode(AmbaCSL_VinGetDphy1LvCmosMode() & ~0x0ffU);
            AmbaCSL_VinSetDphy1LvCmosModeSpclk(AmbaCSL_VinGetDphy1LvCmosModeSpclk() & ~0x3U);
        } else {
            AmbaCSL_VinSetDphy1LvdsBitMode(AmbaCSL_VinGetDphy1LvdsBitMode() & ~0x1U);
            AmbaCSL_VinSetDphy1MipiMode(AmbaCSL_VinGetDphy1MipiMode() | 0x1U);
            AmbaCSL_VinSetDphy1EightLaneMipiMode(0x0U);

            DataLaneEnableVal = DataLaneEnableVal & 0x00fU;
            AmbaCSL_VinSetDphy1LvCmosMode(AmbaCSL_VinGetDphy1LvCmosMode() & ~DataLaneEnableVal);
            AmbaCSL_VinSetDphy1LvCmosModeSpclk(AmbaCSL_VinGetDphy1LvCmosModeSpclk() & ~0x1U);
        }
        break;
    default:
        RetVal = VIN_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinSetPip2LvdsPadMode - Configure LVDS pad mode
 *  @param[in] PadMode Indicate pad mode
 *  @param[in] DataLaneEnable Enabled data pins defined in bit-wise
 *  @return error code
 */
UINT32 AmbaCSL_VinSetPip2LvdsPadMode(UINT32 PadMode, UINT32 DataLaneEnable)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 DataLaneEnableVal = DataLaneEnable;

    switch (PadMode) {
    case AMBA_VIN_PAD_MODE_SLVS:
        AmbaCSL_VinSetDphy0LvdsBitMode(AmbaCSL_VinGetDphy0LvdsBitMode() & ~0x2U);

        AmbaCSL_VinSetDphy0MipiMode(AmbaCSL_VinGetDphy0MipiMode() & ~0x2U);

        DataLaneEnableVal = DataLaneEnableVal & 0x0f0U;
        AmbaCSL_VinSetDphy0LvCmosMode(AmbaCSL_VinGetDphy0LvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetDphy0LvCmosModeSpclk(AmbaCSL_VinGetDphy0LvCmosModeSpclk() & ~0x2U);
        break;
    case AMBA_VIN_PAD_MODE_MIPI:
        AmbaCSL_VinSetDphy0LvdsBitMode(AmbaCSL_VinGetDphy0LvdsBitMode() & ~0x2U);

        AmbaCSL_VinSetDphy0MipiMode(AmbaCSL_VinGetDphy0MipiMode() | 0x2U);

        DataLaneEnableVal = DataLaneEnableVal & 0x0f0U;
        AmbaCSL_VinSetDphy0LvCmosMode(AmbaCSL_VinGetDphy0LvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetDphy0LvCmosModeSpclk(AmbaCSL_VinGetDphy0LvCmosModeSpclk() & ~0x2U);
        break;
    default:
        RetVal = VIN_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinSetPip3LvdsPadMode - Configure LVDS pad mode
 *  @param[in] PadMode Indicate pad mode
 *  @param[in] DataLaneEnable Enabled data pins defined in bit-wise
 *  @return error code
 */
UINT32 AmbaCSL_VinSetPip3LvdsPadMode(UINT32 PadMode, UINT32 DataLaneEnable)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 DataLaneEnableVal = DataLaneEnable;

    switch (PadMode) {
    case AMBA_VIN_PAD_MODE_SLVS:
        AmbaCSL_VinSetDphy0LvdsBitMode(AmbaCSL_VinGetDphy0LvdsBitMode() & ~0x4U);

        AmbaCSL_VinSetDphy0MipiMode(AmbaCSL_VinGetDphy0MipiMode() & ~0x4U);

        DataLaneEnableVal = DataLaneEnableVal & 0xf00U;
        AmbaCSL_VinSetDphy0LvCmosMode(AmbaCSL_VinGetDphy0LvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetDphy0LvCmosModeSpclk(AmbaCSL_VinGetDphy0LvCmosModeSpclk() & ~0x4U);
        break;
    case AMBA_VIN_PAD_MODE_MIPI:
        AmbaCSL_VinSetDphy0LvdsBitMode(AmbaCSL_VinGetDphy0LvdsBitMode() & ~0x4U);

        AmbaCSL_VinSetDphy0MipiMode(AmbaCSL_VinGetDphy0MipiMode() | 0x4U);

        DataLaneEnableVal = DataLaneEnableVal & 0xf00U;
        AmbaCSL_VinSetDphy0LvCmosMode(AmbaCSL_VinGetDphy0LvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetDphy0LvCmosModeSpclk(AmbaCSL_VinGetDphy0LvCmosModeSpclk() & ~0x4U);
        break;
    default:
        RetVal = VIN_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinSetPip4LvdsPadMode - Configure LVDS pad mode
 *  @param[in] PadMode Indicate pad mode
 *  @param[in] DataLaneEnable Enabled data pins defined in bit-wise
 *  @return error code
 */
UINT32 AmbaCSL_VinSetPip4LvdsPadMode(UINT32 PadMode, UINT32 DataLaneEnable)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 DataLaneEnableVal = (DataLaneEnable >> 12U);

    switch (PadMode) {
    case AMBA_VIN_PAD_MODE_SLVS:
        AmbaCSL_VinSetDphy1LvdsBitMode(AmbaCSL_VinGetDphy1LvdsBitMode() & ~0x2U);

        AmbaCSL_VinSetDphy1MipiMode(AmbaCSL_VinGetDphy1MipiMode() & ~0x2U);

        DataLaneEnableVal = DataLaneEnableVal & 0x0f0U;
        AmbaCSL_VinSetDphy1LvCmosMode(AmbaCSL_VinGetDphy1LvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetDphy1LvCmosModeSpclk(AmbaCSL_VinGetDphy1LvCmosModeSpclk() & ~0x2U);
        break;
    case AMBA_VIN_PAD_MODE_MIPI:
        AmbaCSL_VinSetDphy1LvdsBitMode(AmbaCSL_VinGetDphy1LvdsBitMode() & ~0x2U);

        AmbaCSL_VinSetDphy1MipiMode(AmbaCSL_VinGetDphy1MipiMode() | 0x2U);

        DataLaneEnableVal = DataLaneEnableVal & 0x0f0U;
        AmbaCSL_VinSetDphy1LvCmosMode(AmbaCSL_VinGetDphy1LvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetDphy1LvCmosModeSpclk(AmbaCSL_VinGetDphy1LvCmosModeSpclk() & ~0x2U);
        break;
    default:
        RetVal = VIN_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinSetPip5LvdsPadMode - Configure LVDS pad mode
 *  @param[in] PadMode Indicate pad mode
 *  @param[in] DataLaneEnable Enabled data pins defined in bit-wise
 *  @return error code
 */
UINT32 AmbaCSL_VinSetPip5LvdsPadMode(UINT32 PadMode, UINT32 DataLaneEnable)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 DataLaneEnableVal = (DataLaneEnable >> 12U);

    switch (PadMode) {
    case AMBA_VIN_PAD_MODE_SLVS:
        AmbaCSL_VinSetDphy1LvdsBitMode(AmbaCSL_VinGetDphy1LvdsBitMode() & ~0x4U);

        AmbaCSL_VinSetDphy1MipiMode(AmbaCSL_VinGetDphy1MipiMode() & ~0x4U);

        DataLaneEnableVal = DataLaneEnableVal & 0xf00U;
        AmbaCSL_VinSetDphy1LvCmosMode(AmbaCSL_VinGetDphy1LvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetDphy1LvCmosModeSpclk(AmbaCSL_VinGetDphy1LvCmosModeSpclk() & ~0x4U);
        break;
    case AMBA_VIN_PAD_MODE_MIPI:
        AmbaCSL_VinSetDphy1LvdsBitMode(AmbaCSL_VinGetDphy1LvdsBitMode() & ~0x4U);

        AmbaCSL_VinSetDphy1MipiMode(AmbaCSL_VinGetDphy1MipiMode() | 0x4U);

        DataLaneEnableVal = DataLaneEnableVal & 0xf00U;
        AmbaCSL_VinSetDphy1LvCmosMode(AmbaCSL_VinGetDphy1LvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetDphy1LvCmosModeSpclk(AmbaCSL_VinGetDphy1LvCmosModeSpclk() & ~0x4U);
        break;
    default:
        RetVal = VIN_ERR_ARG;
        break;
    }

    return RetVal;
}

static UINT32 VinGetLaneShift(UINT32 VinID, UINT8 *pLaneShift)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (VinID == AMBA_VIN_CHANNEL0) {
        *pLaneShift = 0U;
    } else if (VinID == AMBA_VIN_CHANNEL1) {
        *pLaneShift = 12U;
    } else if (VinID == AMBA_VIN_CHANNEL2) {
        *pLaneShift = 4U;
    } else if (VinID == AMBA_VIN_CHANNEL3) {
        *pLaneShift = 8U;
    } else if (VinID == AMBA_VIN_CHANNEL4) {
        *pLaneShift = 16U;
    } else if (VinID == AMBA_VIN_CHANNEL5) {
        *pLaneShift = 20U;
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

static void DebugBusConfigVinGlobal(UINT32 VinID)
{
    UINT32 BackupAddrExtensionData;
    UINT16 GlobalConfigData[AMBA_CSL_VIN_GLOBAL_CONFIG_SIZE];
    ULONG BaseAddr;
    ULONG Addr;
    ULONG i;
    volatile UINT32 *pAddr;
    const AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s *pVinGlobalCfg;

    pVinGlobalCfg = pGlobalConfigData[VinID];

    if (AmbaWrap_memcpy(GlobalConfigData, pVinGlobalCfg, sizeof(GlobalConfigData)) == ERR_NONE) {

        BackupAddrExtensionData = AmbaCSL_IdspGetAddrExtensionData();
        if (VinID == AMBA_VIN_CHANNEL1) {
            AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_PIP_VIN, 0U);
        } else if (VinID == AMBA_VIN_CHANNEL2) {
            AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_PIP2_VIN, 0U);
        } else if (VinID == AMBA_VIN_CHANNEL3) {
            AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_PIP3_VIN, 0U);
        } else if (VinID == AMBA_VIN_CHANNEL4) {
            AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_PIP4_VIN, 0U);
        } else if (VinID == AMBA_VIN_CHANNEL5) {
            AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_PIP5_VIN, 0U);
        } else { /* default section */
            AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN, 0U);
        }
        AmbaDelayCycles(0xfffU);

        AmbaMisra_TypeCast(&BaseAddr, &pAmbaIDSP_VinGlobalReg);

        for (i = 0U; i < AMBA_CSL_VIN_GLOBAL_CONFIG_SIZE; i++) {
            Addr = BaseAddr + (i * 4U);
            AmbaMisra_TypeCast(&pAddr, &Addr);
            WriteWord(pAddr, GlobalConfigData[i]);
            // DSP_PRINT("WriteWord 0x%08x 0x%04x", BaseAddr + i * 4, GlobalConfigData[i]);
        }

        AmbaCSL_IdspSetAddrExtensionData(BackupAddrExtensionData);
        AmbaDelayCycles(0xfffU);
    }
}

static UINT32 DebugBusConfigVinMain(UINT32 VinID, const AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg, UINT32 ToggleFrameID)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 BackupAddrExtensionData;
    UINT16 MainConfigData[AMBA_CSL_VIN_MAIN_CONFIG_SIZE];
    ULONG BaseAddr;
    ULONG i;
    ULONG Addr;
    volatile UINT32 *pAddr;

    if (AmbaWrap_memcpy(MainConfigData, pVinCfg, sizeof(MainConfigData)) != ERR_NONE) {
        RetVal = VIN_ERR_UNEXPECTED;
    }

    BackupAddrExtensionData = AmbaCSL_IdspGetAddrExtensionData();
    if (VinID == AMBA_VIN_CHANNEL1) {
        AmbaCSL_IdspResetPipPipelineSection();
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_PIP_VIN, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL2) {
        AmbaCSL_IdspResetPip2PipelineSection();
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_PIP2_VIN, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL3) {
        AmbaCSL_IdspResetPip3PipelineSection();
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_PIP3_VIN, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL4) {
        AmbaCSL_IdspResetPip4PipelineSection();
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_PIP4_VIN, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL5) {
        AmbaCSL_IdspResetPip5PipelineSection();
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_PIP5_VIN, 0U);
    } else { /* default section */
        AmbaCSL_IdspResetVinPipelineSection();
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN, 0U);
    }
    AmbaDelayCycles(0xfffU);

    MainConfigData[2] &= ~(UINT16)0x4000U; /* do not disable vin after vsync */
    MainConfigData[7] |= 0x2U;     /* DBG_NON_BLOCKING=1 */

    AmbaMisra_TypeCast(&BaseAddr, &pAmbaIDSP_VinMainReg);

    for (i = 0U; i <= 74U; i++) {
        Addr = BaseAddr + (i * 4U);
        AmbaMisra_TypeCast(&pAddr, &Addr);
        WriteWord(pAddr, MainConfigData[i]);
        // DSP_PRINT("WriteWord 0x%08x 0x%04x", BaseAddr + i * 4, MainConfigData[i]);
    }
    MainConfigData[128] = 1U; /* manually activate config (for debug only) */
    Addr = BaseAddr + (128U * 4U);
    AmbaMisra_TypeCast(&pAddr, &Addr);
    if (ToggleFrameID == 0U) {
        WriteWord(pAddr, MainConfigData[128]);
    } else {
        WriteWord(pAddr, MainConfigData[128]);
        AmbaDelayCycles(0xfffU);
        WriteWord(pAddr, MainConfigData[128] | 0x2U);
    }

    AmbaCSL_IdspSetAddrExtensionData(BackupAddrExtensionData);
    AmbaDelayCycles(0xfffU);

    return RetVal;
}

static UINT32 DebugBusConfigVinMSync(UINT32 MSyncID)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 BackupAddrExtensionData;
    UINT16 MSyncConfigData[AMBA_CSL_VIN_MASTER_SYNC_CONFIG_SIZE] = {0};
    ULONG BaseAddr;
    ULONG Addr;
    ULONG i;
    volatile UINT32 *pAddr;

    BackupAddrExtensionData = AmbaCSL_IdspGetAddrExtensionData();
    if (MSyncID == AMBA_VIN_MSYNC0) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN, 0U);
        AmbaDelayCycles(0xfffU);
        if (AmbaWrap_memcpy(MSyncConfigData, &AmbaCSL_VinMSync0ConfigData, sizeof(MSyncConfigData)) != ERR_NONE) {
            RetVal = VIN_ERR_UNEXPECTED;
        }
    } else if (MSyncID == AMBA_VIN_MSYNC1) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_PIP_VIN, 0U);
        AmbaDelayCycles(0xfffU);
        if (AmbaWrap_memcpy(MSyncConfigData, &AmbaCSL_VinMSync1ConfigData, sizeof(MSyncConfigData)) != ERR_NONE) {
            RetVal = VIN_ERR_UNEXPECTED;
        }
    } else {
        if (AmbaWrap_memcpy(MSyncConfigData, &AmbaCSL_VinMSync1ConfigData, sizeof(MSyncConfigData)) != ERR_NONE) {
            /* misra */
        }
        RetVal = VIN_ERR_ARG;
    }

    AmbaMisra_TypeCast(&BaseAddr, &pAmbaIDSP_VinMasterSyncReg);

    if (RetVal != VIN_ERR_ARG) {
        for (i = 0; i < AMBA_CSL_VIN_MASTER_SYNC_CONFIG_SIZE; i++) {
            Addr = BaseAddr + (i * 4U);
            AmbaMisra_TypeCast(&pAddr, &Addr);
            WriteWord(pAddr, MSyncConfigData[i]);
            // DSP_PRINT("WriteWord 0x%08x 0x%04x", BaseAddr + i * 4, MSyncConfigData[i]);
        }
    }

    AmbaCSL_IdspSetAddrExtensionData(BackupAddrExtensionData);
    AmbaDelayCycles(0xfffU);

    return RetVal;
}

/**
 *  AmbaCSL_VinSlvsConfigGlobal - Configure VIN global configuration for SLVS
 *  @param[in] VinID Indicate VIN channel
 *  @return error code
 */
UINT32 AmbaCSL_VinSlvsConfigGlobal(UINT32 VinID)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s *pVinGlobalCfg = NULL;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinGlobalCfg = pGlobalConfigData[VinID];
        if (AmbaWrap_memset(pVinGlobalCfg, 0, sizeof(AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s)) == ERR_NONE) {

            pVinGlobalCfg->SensorType    = 0U;
            pVinGlobalCfg->SlvsClkMode   = 1U;
            pVinGlobalCfg->AFIFOBypass   = 0U;

            DebugBusConfigVinGlobal(VinID);
        } else {
            RetVal  = VIN_ERR_UNEXPECTED;
        }
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinMipiConfigGlobal - Configure VIN global configuration for MIPI
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] VirtChanHDREnable Enable(1)/Disable(0) MIPI virtual channel HDR
 *  @return error code
 */
UINT32 AmbaCSL_VinMipiConfigGlobal(UINT32 VinID, UINT32 VirtChanHDREnable)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s *pVinGlobalCfg = NULL;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinGlobalCfg = pGlobalConfigData[VinID];
        if (AmbaWrap_memset(pVinGlobalCfg, 0, sizeof(AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s)) == ERR_NONE) {

            pVinGlobalCfg->SensorType    = 3U;
            pVinGlobalCfg->SlvsClkMode   = 0U;
            pVinGlobalCfg->AFIFOBypass   = 1U;
            pVinGlobalCfg->MipiVCHdr     = (VirtChanHDREnable == 0U) ? 0U : 3U;

            DebugBusConfigVinGlobal(VinID);
        } else {
            RetVal = VIN_ERR_UNEXPECTED;
        }
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinDvpConfigGlobal - Configure VIN global/static configuration for DVP
 *  @param[in] VinID Indicate VIN channel
 *  @return error code
 */
UINT32 AmbaCSL_VinDvpConfigGlobal(UINT32 VinID)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s *pVinGlobalCfg = NULL;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinGlobalCfg = pGlobalConfigData[VinID];
        if (AmbaWrap_memset(pVinGlobalCfg, 0, sizeof(AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s)) == ERR_NONE) {

            pVinGlobalCfg->SensorType    = 2U;
            pVinGlobalCfg->SlvsClkMode   = 0U;
            pVinGlobalCfg->AFIFOBypass   = 1U;

            DebugBusConfigVinGlobal(VinID);
        } else {
            RetVal = VIN_ERR_UNEXPECTED;
        }
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

static void VinActiveRegionSizeConfig(AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg, const AMBA_VIN_RX_HV_SYNC_s *pRxHvSyncCtrl)
{
    if (pRxHvSyncCtrl->NumActivePixels != 0U) {
        pVinCfg->ActiveRegionWidth = (UINT16)(pRxHvSyncCtrl->NumActivePixels - 1U);
    }
    if (pRxHvSyncCtrl->NumActiveLines != 0U) {
        pVinCfg->ActiveRegionHeight = (UINT16)(pRxHvSyncCtrl->NumActiveLines - 1U);
    }
}

static void VinIntrDelayConfig(AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg, UINT32 DelayedVsync)
{
    if (DelayedVsync < 1U) {
        pVinCfg->IntrDelayL = 1U;
        pVinCfg->IntrDelayH = 0U;
    } else {
        pVinCfg->IntrDelayL = (UINT16)(DelayedVsync & 0xffffU);
        pVinCfg->IntrDelayH = (UINT16)(DelayedVsync >> 16U);
    }
}

static void VinCustomSyncCodeConfig(AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg, const AMBA_VIN_CUSTOM_SYNC_CODE_s *CustomSyncCode)
{
    pVinCfg->SlvsCtrl.SyncType = 1U;

    pVinCfg->SlvsSyncDetectMask = 0x8000U;
    pVinCfg->SlvsSyncDetectPattern = 0x8000U;

    pVinCfg->SlvsSyncCompareMask = ~CustomSyncCode->SyncCodeMask;
    pVinCfg->SlvsSolPattern = CustomSyncCode->PatternSol;
    pVinCfg->SlvsEolPattern = CustomSyncCode->PatternEol;
    pVinCfg->SlvsSofPattern = CustomSyncCode->PatternSof;
    pVinCfg->SlvsEofPattern = CustomSyncCode->PatternEof;
    pVinCfg->SlvsSovPattern = CustomSyncCode->PatternSov;
    pVinCfg->SlvsEovPattern_MipiCtrl0.SlvsEovPattern = CustomSyncCode->PatternEov;

    pVinCfg->SlvsCtrl.PatMaskAlignment = CustomSyncCode->PatternAlign;
    pVinCfg->SlvsCtrl.DetectSol = CustomSyncCode->SolDetectEnable;
    pVinCfg->SlvsCtrl.DetectEol = CustomSyncCode->EolDetectEnable;
    pVinCfg->SlvsCtrl.DetectSof = CustomSyncCode->SofDetectEnable;
    pVinCfg->SlvsCtrl.DetectEof = CustomSyncCode->EofDetectEnable;
    pVinCfg->SlvsCtrl.DetectSov = CustomSyncCode->SovDetectEnable;
    pVinCfg->SlvsCtrl.DetectEov = CustomSyncCode->EovDetectEnable;
}

static void VinITU656SyncCodeConfig(AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg, UINT32 Interlace)
{
    pVinCfg->SlvsCtrl.SyncType = 1U;
    pVinCfg->SlvsSyncDetectMask = 0x8000U;
    pVinCfg->SlvsSyncDetectPattern = 0x8000U;

    pVinCfg->SlvsCtrl.DetectSol    = 1U;
    pVinCfg->SlvsCtrl.DetectEol    = 0U;
    pVinCfg->SlvsCtrl.DetectSof    = 0U;
    pVinCfg->SlvsCtrl.DetectEof    = 0U;
    pVinCfg->SlvsCtrl.DetectSov    = 1U;
    pVinCfg->SlvsCtrl.DetectEov    = 0U;

    pVinCfg->SlvsCtrl.PatMaskAlignment = 1U;

    if (Interlace == 1U) {
        pVinCfg->SlvsSyncCompareMask = 0xb800U;
        pVinCfg->SlvsSolPattern = 0x8000U;
        pVinCfg->SlvsEolPattern = 0U;
        pVinCfg->SlvsSofPattern = 0U;
        pVinCfg->SlvsEofPattern = 0U;
        pVinCfg->SlvsSovPattern = 0xa800U;
        pVinCfg->SlvsEovPattern_MipiCtrl0.SlvsEovPattern = 0U;
    } else {
        pVinCfg->SlvsSyncCompareMask = 0xff00U;
        pVinCfg->SlvsSolPattern = 0x8000U;
        pVinCfg->SlvsEolPattern = 0x9d00U;
        pVinCfg->SlvsSofPattern = 0U;
        pVinCfg->SlvsEofPattern = 0U;
        pVinCfg->SlvsSovPattern = 0xab00U;
        pVinCfg->SlvsEovPattern_MipiCtrl0.SlvsEovPattern = 0xb600U;
    }
}

/**
 *  AmbaCSL_VinSlvsConfigMain - Configure VIN main configuration for SLVS
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinSlvsConfig Pointer to SLVS configuration
 *  @return error code
 */
UINT32 AmbaCSL_VinSlvsConfigMain(UINT32 VinID, const AMBA_VIN_SLVS_CONFIG_s *pVinSlvsConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg = NULL;
    AMBA_CSL_VIN_INFO_s *pVinInfo = NULL;
    /* Vin internal sensor clock rate = sensor output clock rate / 2 */
    UINT32 DelayedVsync = pVinSlvsConfig->Config.DelayedVsync / 2U;
    UINT8 LaneShift = 0U;
    UINT8 DataLanePinSelect[AMBA_VIN_NUM_DATA_LANE] = {0U};
    UINT8 i;
    UINT16 DataLaneEnable = 0U;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinCfg     = pMainConfigData[VinID];
        pVinInfo    = pVinInformation[VinID];

        RetVal = AmbaWrap_memset(pVinCfg, 0, sizeof(AMBA_CSL_VIN_CONFIG_DATA_s));
        RetVal |= AmbaWrap_memset(pVinInfo, 0, sizeof(AMBA_CSL_VIN_INFO_s));
        if (RetVal == ERR_NONE) {

            (void) VinGetLaneShift(VinID, &LaneShift);

            /* VIN control register */
            pVinCfg->Ctrl.SwReset = 1U;
            pVinCfg->Ctrl.Enable = 1U;
            pVinCfg->Ctrl.OutputEnable = 1U;
            pVinCfg->Ctrl.BitsPerPixel = (UINT8)((pVinSlvsConfig->Config.NumDataBits - 8U) >> 1U);

            /* DVP ctrl */
            if (pVinSlvsConfig->Config.ColorSpace == AMBA_VIN_COLOR_SPACE_YUV) {
                pVinCfg->DvpCtrl_SyncPinSelect.YuvEnable   = 1U;
                pVinCfg->DvpCtrl_SyncPinSelect.YuvOrder    = pVinSlvsConfig->Config.YuvOrder;
            } else {
                pVinCfg->DvpCtrl_SyncPinSelect.YuvEnable   = 0U;
            }

            for (i = 0U; i < AMBA_VIN_NUM_DATA_LANE; i ++) {
                if (i < pVinSlvsConfig->NumActiveLanes) {
                    DataLanePinSelect[i] = (pVinSlvsConfig->pLaneMapping[i] - LaneShift);
                    DataLaneEnable |= ((UINT16)((UINT32)1U << DataLanePinSelect[i]));
                } else {
                    /* the default value of un-used pins */
                    DataLanePinSelect[i] = i;
                }
            }

            pVinCfg->Ctrl.DataLaneEnable = DataLaneEnable;
            pVinCfg->DataLanePinSelect0.DataLane0PinSelect = DataLanePinSelect[0];
            pVinCfg->DataLanePinSelect0.DataLane1PinSelect = DataLanePinSelect[1];
            pVinCfg->DataLanePinSelect0.DataLane2PinSelect = DataLanePinSelect[2];
            pVinCfg->DataLanePinSelect0.DataLane3PinSelect = DataLanePinSelect[3];
            pVinCfg->DataLanePinSelect0.DataLane4PinSelect = DataLanePinSelect[4];
            pVinCfg->DataLanePinSelect0.DataLane5PinSelect = DataLanePinSelect[5];
            pVinCfg->DataLanePinSelect0.DataLane6PinSelect = DataLanePinSelect[6];
            pVinCfg->DataLanePinSelect0.DataLane7PinSelect = DataLanePinSelect[7];
            pVinCfg->DataLanePinSelect1_Ctrl2.DataLane8PinSelect = DataLanePinSelect[8];
            pVinCfg->DataLanePinSelect1_Ctrl2.DataLane9PinSelect = DataLanePinSelect[9];
            pVinCfg->DataLanePinSelect1_Ctrl2.DataLane10PinSelect = DataLanePinSelect[10];
            pVinCfg->DataLanePinSelect1_Ctrl2.DataLane11PinSelect = DataLanePinSelect[11];

            if (pVinSlvsConfig->Config.SplitCtrl.NumSplits == 0U) {
                pVinCfg->SplitWidth = 0U;
            } else {
                pVinCfg->SplitWidth = pVinSlvsConfig->Config.SplitCtrl.SplitWidth;
            }

            if (pVinSlvsConfig->SyncDetectCtrl.ITU656Type == 1U) {
                /* ITU-656 */
                VinITU656SyncCodeConfig(pVinCfg, pVinSlvsConfig->Config.FrameRate.Interlace);
            } else {
                /* Customer sync code */
                VinCustomSyncCodeConfig(pVinCfg, &pVinSlvsConfig->SyncDetectCtrl.CustomSyncCode);
            }

            pVinCfg->CropEndRow_SlvsSyncCodeLocation.SyncInterleaving = pVinSlvsConfig->SyncDetectCtrl.SyncInterleaving;
            pVinCfg->CropEndRow_SlvsSyncCodeLocation.SyncTolerance = 1U;
            pVinCfg->CropEndRow_SlvsSyncCodeLocation.SyncAllLanes = 0U;

            pVinCfg->SlvsCtrl.AllowPartialCode       = 0U;
            pVinCfg->SlvsCtrl.LockSyncPhase          = 0U;
            pVinCfg->SlvsCtrl.SyncCorrection         = 1U;
            pVinCfg->SlvsCtrl.DeskewEnable           = 1U;
            pVinCfg->SlvsCtrl.Enable656Ecc           = 0U;
            pVinCfg->SlvsCtrl.UnlockOnTimeout        = 1U;
            pVinCfg->SlvsCtrl.UnlockOnDeskewError    = 1U;
            pVinCfg->SlvsCtrl.LineReorder            = 0U;

            /* Active region size */
            VinActiveRegionSizeConfig(pVinCfg, &pVinSlvsConfig->Config.RxHvSyncCtrl);

            /* Should be larger than 1, otherwise VIN will be dead */
            pVinCfg->SyncDelayL = 1U;

            VinIntrDelayConfig(pVinCfg, DelayedVsync);

            pVinCfg->DvpCtrl_SyncPinSelect.DisableAfterSync = 0U;
            pVinCfg->DvpCtrl_SyncPinSelect.DoubleBufferEnable = 1U;

            /* update vin info */
            if (AmbaWrap_memcpy(&pVinInfo->FrameRate, &pVinSlvsConfig->Config.FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
                RetVal = VIN_ERR_UNEXPECTED;
            }
            pVinInfo->ColorSpace = pVinSlvsConfig->Config.ColorSpace;
            pVinInfo->BayerPattern = pVinSlvsConfig->Config.BayerPattern;
            pVinInfo->YuvOrder = pVinSlvsConfig->Config.YuvOrder;
            pVinInfo->NumDataBits = pVinSlvsConfig->Config.NumDataBits;
            pVinInfo->NumSkipFrame = pVinSlvsConfig->Config.NumSkipFrame;
        } else {
            RetVal = VIN_ERR_UNEXPECTED;
        }
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinMipiConfigMain - Configure VIN main configuration for MIPI
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinMipiConfig Pointer to MIPI configuration
 *  @return error code
 */
UINT32 AmbaCSL_VinMipiConfigMain(UINT32 VinID, const AMBA_VIN_MIPI_CONFIG_s *pVinMipiConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg = NULL;
    AMBA_CSL_VIN_INFO_s *pVinInfo = NULL;
    UINT8 VirtChanNumMinus1;
    /* Vin internal sensor clock rate = sensor output clock rate / 4 */
    UINT32 DelayedVsync = pVinMipiConfig->Config.DelayedVsync / 4U;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinCfg     = pMainConfigData[VinID];
        pVinInfo    = pVinInformation[VinID];

        RetVal = AmbaWrap_memset(pVinCfg, 0, sizeof(AMBA_CSL_VIN_CONFIG_DATA_s));
        RetVal |= AmbaWrap_memset(pVinInfo, 0, sizeof(AMBA_CSL_VIN_INFO_s));
        if (RetVal == ERR_NONE) {

            /* VIN control register */
            pVinCfg->Ctrl.SwReset = 1U;
            pVinCfg->Ctrl.Enable = 1U;
            pVinCfg->Ctrl.OutputEnable = 1U;
            pVinCfg->Ctrl.BitsPerPixel = (UINT8)((pVinMipiConfig->Config.NumDataBits - 8U) >> 1U);
            pVinCfg->Ctrl.DataLaneEnable = (UINT16)(((UINT32)1U << pVinMipiConfig->NumActiveLanes) - 1U);

            /* DVP ctrl */
            if (pVinMipiConfig->Config.ColorSpace == AMBA_VIN_COLOR_SPACE_YUV) {
                pVinCfg->DvpCtrl_SyncPinSelect.YuvEnable   = 1U;
                pVinCfg->DvpCtrl_SyncPinSelect.YuvOrder    = pVinMipiConfig->Config.YuvOrder;
            } else {
                pVinCfg->DvpCtrl_SyncPinSelect.YuvEnable   = 0U;
            }

            /* MIPI control */
            if (pVinMipiConfig->VirtChanHDREnable == 0U) {
                pVinCfg->DataLanePinSelect1_Ctrl2.DisableCropRegion   = 0U;
                pVinCfg->SlvsEovPattern_MipiCtrl0.VirtChanNumMinus1   = 0U;
                pVinCfg->SlvsEovPattern_MipiCtrl0.VirtChanMask        = 0x3U;
                pVinCfg->SlvsEovPattern_MipiCtrl0.FSVirtChanPattern   = 0U;
                pVinCfg->SlvsEovPattern_MipiCtrl0.FEVirtChanPattern   = 0U;
                pVinCfg->SlvsEovPattern_MipiCtrl0.Exp0VirtChanPattern = 0U;
                pVinCfg->SlvsEovPattern_MipiCtrl0.Exp1VirtChanPattern = 1U;
                pVinCfg->SlvsEovPattern_MipiCtrl0.Exp2VirtChanPattern = 2U;
                pVinCfg->SlvsEovPattern_MipiCtrl0.Exp3VirtChanPattern = 3U;
                pVinCfg->MipiCtrl1_MipiExp1Offset.MipiCropEnable      = 0U;
                pVinCfg->MipiCtrl1_MipiExp1Offset.MipiExp1Offset      = 0U;
                pVinCfg->MipiExp2Offset                               = 0U;
                pVinCfg->MipiExp3Offset                               = 0U;
            } else {
                pVinCfg->Ctrl.SwReset                                 = 0U; /* must be 0 when using VC HDR */
                pVinCfg->DataLanePinSelect1_Ctrl2.DisableCropRegion   = 1U;
                VirtChanNumMinus1 = pVinMipiConfig->VirtChanHDRConfig.VirtChanNum - (UINT8)1U;
                pVinCfg->SlvsEovPattern_MipiCtrl0.VirtChanNumMinus1   = VirtChanNumMinus1;
                pVinCfg->SlvsEovPattern_MipiCtrl0.VirtChanMask        = 0x0U;
                pVinCfg->SlvsEovPattern_MipiCtrl0.FSVirtChanPattern   = pVinMipiConfig->VirtChanHDRConfig.VCPattern1stExp;
                pVinCfg->SlvsEovPattern_MipiCtrl0.Exp0VirtChanPattern = pVinMipiConfig->VirtChanHDRConfig.VCPattern1stExp;
                pVinCfg->SlvsEovPattern_MipiCtrl0.Exp1VirtChanPattern = pVinMipiConfig->VirtChanHDRConfig.VCPattern2ndExp;
                pVinCfg->SlvsEovPattern_MipiCtrl0.Exp2VirtChanPattern = pVinMipiConfig->VirtChanHDRConfig.VCPattern3rdExp;
                pVinCfg->SlvsEovPattern_MipiCtrl0.Exp3VirtChanPattern = pVinMipiConfig->VirtChanHDRConfig.VCPattern4thExp;

                if (pVinMipiConfig->VirtChanHDRConfig.VirtChanNum == 2U) {
                    pVinCfg->SlvsEovPattern_MipiCtrl0.FEVirtChanPattern = pVinCfg->SlvsEovPattern_MipiCtrl0.Exp1VirtChanPattern;
                } else if (pVinMipiConfig->VirtChanHDRConfig.VirtChanNum == 3U) {
                    pVinCfg->SlvsEovPattern_MipiCtrl0.FEVirtChanPattern = pVinCfg->SlvsEovPattern_MipiCtrl0.Exp2VirtChanPattern;
                } else if (pVinMipiConfig->VirtChanHDRConfig.VirtChanNum == 4U) {
                    pVinCfg->SlvsEovPattern_MipiCtrl0.FEVirtChanPattern = pVinCfg->SlvsEovPattern_MipiCtrl0.Exp3VirtChanPattern;
                } else {
                    pVinCfg->SlvsEovPattern_MipiCtrl0.FEVirtChanPattern = 0U;
                }

                pVinCfg->MipiCtrl1_MipiExp1Offset.MipiCropEnable      = 1U;
                pVinCfg->MipiCtrl1_MipiExp1Offset.MipiExp1Offset      = pVinMipiConfig->VirtChanHDRConfig.Offset2ndExp;
                pVinCfg->MipiExp2Offset                               = pVinMipiConfig->VirtChanHDRConfig.Offset3rdExp;
                pVinCfg->MipiExp3Offset                               = pVinMipiConfig->VirtChanHDRConfig.Offset4thExp;
            }

            pVinCfg->MipiCtrl1_MipiExp1Offset.DataTypeMask        = pVinMipiConfig->DataTypeMask;
            pVinCfg->MipiCtrl1_MipiExp1Offset.DataTypePattern     = pVinMipiConfig->DataType;
            pVinCfg->MipiCtrl1_MipiExp1Offset.ByteSwapEnable      = 0U;
            pVinCfg->MipiCtrl1_MipiExp1Offset.EccEnable           = 1U;
            pVinCfg->MipiCtrl1_MipiExp1Offset.ForwardEccEnable    = 1U;

            pVinCfg->DataLanePinSelect0.DataLane0PinSelect = 0U;
            pVinCfg->DataLanePinSelect0.DataLane1PinSelect = 1U;
            pVinCfg->DataLanePinSelect0.DataLane2PinSelect = 2U;
            pVinCfg->DataLanePinSelect0.DataLane3PinSelect = 3U;
            pVinCfg->DataLanePinSelect0.DataLane4PinSelect = 4U;
            pVinCfg->DataLanePinSelect0.DataLane5PinSelect = 5U;
            pVinCfg->DataLanePinSelect0.DataLane6PinSelect = 6U;
            pVinCfg->DataLanePinSelect0.DataLane7PinSelect = 7U;

            /* According to VLSI team's information, 1 byte cycle skew between 0~7 is possible since they are treated as async at
             * the output of PHY (RxByteclk0~7 treated as async from each other). Therefore, we should always set this field with
             * value 2 to handle the potential 1 byte cycle skew caused by PHY async lane routing */
            pVinCfg->DataLanePinSelect1_Ctrl2.MipiMaxSkewCycle = 2; /* unit: mipi_byteclk cycle; or 4*mipi_sensor_clk cycle */

            if (pVinMipiConfig->Config.SplitCtrl.NumSplits == 0U) {
                pVinCfg->SplitWidth = 0U;
            } else {
                pVinCfg->SplitWidth = pVinMipiConfig->Config.SplitCtrl.SplitWidth;
            }

            /* Active region size */
            VinActiveRegionSizeConfig(pVinCfg, &pVinMipiConfig->Config.RxHvSyncCtrl);

            /* Should be larger than 1, otherwise VIN will be dead */
            pVinCfg->SyncDelayL = 1U;

            VinIntrDelayConfig(pVinCfg, DelayedVsync);

            pVinCfg->DvpCtrl_SyncPinSelect.DisableAfterSync = 0U;
            pVinCfg->DvpCtrl_SyncPinSelect.DoubleBufferEnable = 1U;

            /* update vin info */
            if (AmbaWrap_memcpy(&pVinInfo->FrameRate, &pVinMipiConfig->Config.FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
                RetVal = VIN_ERR_UNEXPECTED;
            }
            pVinInfo->ColorSpace = pVinMipiConfig->Config.ColorSpace;
            pVinInfo->BayerPattern = pVinMipiConfig->Config.BayerPattern;
            pVinInfo->YuvOrder = pVinMipiConfig->Config.YuvOrder;
            pVinInfo->NumDataBits = pVinMipiConfig->Config.NumDataBits;
            pVinInfo->NumSkipFrame = pVinMipiConfig->Config.NumSkipFrame;
        } else {
            RetVal = VIN_ERR_UNEXPECTED;
        }
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinDvpConfigMain - Configure VIN main configuration for DVP
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinDvpConfig Pointer to DVP configuration
 *  @return error code
 */
UINT32 AmbaCSL_VinDvpConfigMain(UINT32 VinID, const AMBA_VIN_DVP_CONFIG_s *pVinDvpConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg = NULL;
    AMBA_CSL_VIN_INFO_s *pVinInfo = NULL;
    /* Vin internal sensor clock rate = sensor output clock rate */
    UINT32 DelayedVsync = pVinDvpConfig->Config.DelayedVsync;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinCfg     = pMainConfigData[VinID];
        pVinInfo    = pVinInformation[VinID];

        RetVal = AmbaWrap_memset(pVinCfg, 0, sizeof(AMBA_CSL_VIN_CONFIG_DATA_s));
        RetVal |= AmbaWrap_memset(pVinInfo, 0, sizeof(AMBA_CSL_VIN_INFO_s));
        if (RetVal == ERR_NONE) {

            /* VIN control register */
            pVinCfg->Ctrl.SwReset = 1U;
            pVinCfg->Ctrl.Enable = 1U;
            pVinCfg->Ctrl.OutputEnable = 1U;
            pVinCfg->Ctrl.BitsPerPixel = (UINT8)((pVinDvpConfig->Config.NumDataBits - 8U) >> 1U);
            pVinCfg->Ctrl.DataLaneEnable = 0U;

            /* DVP ctrl */
            if (pVinDvpConfig->PadType == AMBA_VIN_DVP_PAD_TYPE_LVCMOS) {
                pVinCfg->DvpCtrl_SyncPinSelect.PadType = 0U;
                /* ====== TBD s ======
                        AmbaDSP_VinPhySetDVP(VinID);
                */// ==== TBD e ======
            } else {
                pVinCfg->DvpCtrl_SyncPinSelect.PadType = 1U;
                /* ====== TBD s ======
                        DSP_VinPhySetParallelLVDS(VinID);
                */// ==== TBD e ======
            }

            if (pVinDvpConfig->Config.ColorSpace == AMBA_VIN_COLOR_SPACE_YUV) {
                pVinCfg->DvpCtrl_SyncPinSelect.YuvEnable   = 1U;
                pVinCfg->DvpCtrl_SyncPinSelect.YuvOrder    = pVinDvpConfig->Config.YuvOrder;
            } else {
                pVinCfg->DvpCtrl_SyncPinSelect.YuvEnable   = 0U;
            }

            if (pVinDvpConfig->DvpWide == AMBA_VIN_DVP_WIDE_1_PIXEL) {
                pVinCfg->DvpCtrl_SyncPinSelect.ParallelWidth = 0U;
            } else if (pVinDvpConfig->DvpWide == AMBA_VIN_DVP_WIDE_2_PIXELS) {
                pVinCfg->DvpCtrl_SyncPinSelect.ParallelWidth = 1U;
            } else {
                RetVal = VIN_ERR_ARG;
            }

            if (pVinDvpConfig->DataLatchEdge == AMBA_VIN_DVP_LATCH_EDGE_RISING) {
                pVinCfg->DvpCtrl_SyncPinSelect.ParallelDataRate = 0U;
                pVinCfg->DvpCtrl_SyncPinSelect.ParallelDataEdge = 0U;
            } else if (pVinDvpConfig->DataLatchEdge == AMBA_VIN_DVP_LATCH_EDGE_FALLING) {
                pVinCfg->DvpCtrl_SyncPinSelect.ParallelDataRate = 0U;
                pVinCfg->DvpCtrl_SyncPinSelect.ParallelDataEdge = 1U;
            } else if (pVinDvpConfig->DataLatchEdge == AMBA_VIN_DVP_LATCH_EDGE_BOTH) {
                pVinCfg->DvpCtrl_SyncPinSelect.ParallelDataRate = 1U;
            } else {
                RetVal = VIN_ERR_ARG;
            }

            pVinCfg->DvpCtrl_SyncPinSelect.FieldMode = 0U;

            if (pVinDvpConfig->SyncDetectCtrl.SyncType == AMBA_VIN_DVP_SYNC_TYPE_BT601) {
                pVinCfg->DvpCtrl_SyncPinSelect.SyncType = 1U;

                pVinCfg->DvpCtrl_SyncPinSelect.HsyncPolarity = pVinDvpConfig->SyncDetectCtrl.SyncPinConfig.HsyncPolarity;
                pVinCfg->DvpCtrl_SyncPinSelect.VsyncPolarity = pVinDvpConfig->SyncDetectCtrl.SyncPinConfig.VsyncPolarity;
                pVinCfg->DvpCtrl_SyncPinSelect.FieldPolarity = pVinDvpConfig->SyncDetectCtrl.SyncPinConfig.FieldPolarity;

                /* Signal PIN Selection */
                pVinCfg->DvpCtrl_SyncPinSelect.FieldPinSelect = pVinDvpConfig->SyncDetectCtrl.SyncPinConfig.FieldPinSelect;
                pVinCfg->DvpCtrl_SyncPinSelect.VsyncPinSelect = pVinDvpConfig->SyncDetectCtrl.SyncPinConfig.VsyncPinSelect;
                pVinCfg->DvpCtrl_SyncPinSelect.HsyncPinSelect = pVinDvpConfig->SyncDetectCtrl.SyncPinConfig.HsyncPinSelect;

            } else if (pVinDvpConfig->SyncDetectCtrl.SyncType == AMBA_VIN_DVP_SYNC_TYPE_EMB_SYNC) {
                pVinCfg->DvpCtrl_SyncPinSelect.SyncType = 0U;

                pVinCfg->DvpCtrl_SyncPinSelect.ParallelEmbSyncLoc = pVinDvpConfig->SyncDetectCtrl.EmbSyncConfig.SyncLocate;

                if (pVinDvpConfig->SyncDetectCtrl.EmbSyncConfig.ITU656Type == 0U) {
                    VinCustomSyncCodeConfig(pVinCfg, &pVinDvpConfig->SyncDetectCtrl.EmbSyncConfig.CustomSyncCode);
                } else if (pVinDvpConfig->SyncDetectCtrl.EmbSyncConfig.ITU656Type == 1U) {
                    /* ITU-656 */
                    VinITU656SyncCodeConfig(pVinCfg, pVinDvpConfig->Config.FrameRate.Interlace);
                } else {
                    RetVal = VIN_ERR_ARG;
                }

                pVinCfg->SlvsCtrl.AllowPartialCode       = 0U;
                pVinCfg->SlvsCtrl.LockSyncPhase          = 0U;
                pVinCfg->SlvsCtrl.SyncCorrection         = 1U;
                pVinCfg->SlvsCtrl.DeskewEnable           = 1U;
                pVinCfg->SlvsCtrl.Enable656Ecc           = 0U;
                pVinCfg->SlvsCtrl.UnlockOnTimeout        = 1U;
                pVinCfg->SlvsCtrl.UnlockOnDeskewError    = 1U;
                pVinCfg->SlvsCtrl.LineReorder            = 0U;

                pVinCfg->CropEndRow_SlvsSyncCodeLocation.SyncInterleaving = 0U;
                pVinCfg->CropEndRow_SlvsSyncCodeLocation.SyncTolerance = 1U;
                pVinCfg->CropEndRow_SlvsSyncCodeLocation.SyncAllLanes = 0U;
            } else {
                RetVal = VIN_ERR_ARG;
            }

            /* Active region size */
            VinActiveRegionSizeConfig(pVinCfg, &pVinDvpConfig->Config.RxHvSyncCtrl);

            /* Should be larger than 1, otherwise VIN will be dead */
            pVinCfg->SyncDelayL = 1U;

            VinIntrDelayConfig(pVinCfg, DelayedVsync);

            pVinCfg->DvpCtrl_SyncPinSelect.DisableAfterSync = 0U;
            pVinCfg->DvpCtrl_SyncPinSelect.DoubleBufferEnable = 1U;

            /* update vin info */
            if (AmbaWrap_memcpy(&pVinInfo->FrameRate, &pVinDvpConfig->Config.FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
                RetVal = VIN_ERR_UNEXPECTED;
            }
            pVinInfo->ColorSpace = pVinDvpConfig->Config.ColorSpace;
            pVinInfo->BayerPattern = pVinDvpConfig->Config.BayerPattern;
            pVinInfo->YuvOrder = pVinDvpConfig->Config.YuvOrder;
            pVinInfo->NumDataBits = pVinDvpConfig->Config.NumDataBits;
            pVinInfo->NumSkipFrame = pVinDvpConfig->Config.NumSkipFrame;
        } else {
            RetVal = VIN_ERR_UNEXPECTED;
        }
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinMasterSyncConfig - Configure master H/V Sync generator
 *  @param[in] MSyncID Indicate Master sync pin ID
 *  @param[in] pMSyncConfig Pointer to H/V Sync configuration
 *  @return error code
 */
UINT32 AmbaCSL_VinMasterSyncConfig(UINT32 MSyncID, const AMBA_VIN_MASTER_SYNC_CONFIG_s *pMSyncConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_MASTER_SYNC_CONFIG_DATA_s *pMasterSyncConfig;

    if (MSyncID < AMBA_NUM_VIN_MSYNC) {
        pMasterSyncConfig = pMSyncConfigData[MSyncID];
        /* clear VIN Master Sync configuration data */
        if (AmbaWrap_memset(pMasterSyncConfig, 0, sizeof(AMBA_CSL_VIN_MASTER_SYNC_CONFIG_DATA_s)) == ERR_NONE) {

            pMasterSyncConfig->HSyncPeriodL = (UINT16)(pMSyncConfig->HSync.Period & 0xffffU);
            pMasterSyncConfig->HSyncPeriodH = (UINT16)((pMSyncConfig->HSync.Period >> 16U) & 0xffffU);
            pMasterSyncConfig->HSyncWidth   = (UINT16)(pMSyncConfig->HSync.PulseWidth);
            pMasterSyncConfig->VSyncPeriodL = (UINT16)(pMSyncConfig->VSync.Period & 0xffffU);
            pMasterSyncConfig->VSyncPeriodH = (UINT16)((pMSyncConfig->VSync.Period >> 16U) & 0xffffU);
            pMasterSyncConfig->VSyncWidth   = (UINT16)(pMSyncConfig->VSync.PulseWidth);
            pMasterSyncConfig->HSyncOffset  = pMSyncConfig->HSyncDelayCycles;
            pMasterSyncConfig->VSyncOffset  = pMSyncConfig->VSyncDelayCycles;

            pMasterSyncConfig->MasterSyncCtrl.HsyncPolarity  = pMSyncConfig->HSync.Polarity;
            pMasterSyncConfig->MasterSyncCtrl.VsyncPolarity  = pMSyncConfig->VSync.Polarity;
            if (pMSyncConfig->ToggleHsyncInVblank == 0U) {
                pMasterSyncConfig->MasterSyncCtrl.NoVblankHsync = 1U;
            } else {
                pMasterSyncConfig->MasterSyncCtrl.NoVblankHsync = 0U;
            }
            pMasterSyncConfig->MasterSyncCtrl.InterruptMode  = 0U;
            pMasterSyncConfig->MasterSyncCtrl.VSyncWidthUnit = 0U; /* unit: CYCLES */
            pMasterSyncConfig->MasterSyncCtrl.NumVsyncs      = 1U;
            pMasterSyncConfig->MasterSyncCtrl.Continuous     = 1U; /* continuous generate master sync */
            pMasterSyncConfig->MasterSyncCtrl.Preempt        = 1U;

            RetVal = DebugBusConfigVinMSync(MSyncID);
        } else {
            RetVal = VIN_ERR_UNEXPECTED;
        }
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinDataLaneRemap - Remap data lane
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pLaneRemapConfig Pointer to lane remapping configuration
 *  @return error code
 */
UINT32 AmbaCSL_VinDataLaneRemap(UINT32 VinID, const AMBA_VIN_LANE_REMAP_CONFIG_s *pLaneRemapConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg = NULL;
    const AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s *pVinGlobalCfg = NULL;
    UINT8 LaneShift = 0U;
    UINT8 EnableDeskew  = 0U;
    UINT8 DataLanePinSelect[AMBA_VIN_NUM_DATA_LANE] = {0U};
    UINT32 i;
    UINT16 DataLaneEnable = 0U;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinGlobalCfg   = pGlobalConfigData[VinID];
        pVinCfg         = pMainConfigData[VinID];

        (void) VinGetLaneShift(VinID, &LaneShift);

        if (pVinGlobalCfg->SensorType == 2U) { /* DVP */
            RetVal = VIN_ERR_ARG;
        } else if (pVinGlobalCfg->SensorType == 3U) { /* MIPI*/
            /* only support 1, 2, 3, 4, 8 lane MIPI */
            if (pLaneRemapConfig->NumActiveLanes <= 4U) {
                for (i = 0U; i < pLaneRemapConfig->NumActiveLanes; i ++) {
                    if ((UINT8)(pLaneRemapConfig->pPhyLaneMapping[i] - LaneShift) >= 4U) {
                        /* 4 lane can only use local data lane(lane 0~3) */
                        RetVal = VIN_ERR_ARG;
                        break;
                    }
                    if ((pLaneRemapConfig->pPhyLaneMapping[i] - LaneShift) == 0U) { /* only enable deskew when lane0 has data connected */
                        EnableDeskew = 1U;
                    }
                }
            } else {
                EnableDeskew = 1U;
            }
        } else {
            /* avoid misrac parser error */
        }

        if (RetVal != VIN_ERR_ARG) {
            for (i = 0U; i < AMBA_VIN_NUM_DATA_LANE; i ++) {
                if (i < pLaneRemapConfig->NumActiveLanes) {
                    DataLanePinSelect[i] = (pLaneRemapConfig->pPhyLaneMapping[i] - LaneShift);
                    DataLaneEnable |= ((UINT16)((UINT32)1U << DataLanePinSelect[i]));
                } else {
                    /* the default value of un-used pins cannot be the same as used pin */
                    DataLanePinSelect[i] = 7U;
                }
            }
            pVinCfg->Ctrl.DataLaneEnable = DataLaneEnable;
            pVinCfg->DataLanePinSelect0.DataLane0PinSelect = DataLanePinSelect[0];
            pVinCfg->DataLanePinSelect0.DataLane1PinSelect = DataLanePinSelect[1];
            pVinCfg->DataLanePinSelect0.DataLane2PinSelect = DataLanePinSelect[2];
            pVinCfg->DataLanePinSelect0.DataLane3PinSelect = DataLanePinSelect[3];
            pVinCfg->DataLanePinSelect0.DataLane4PinSelect = DataLanePinSelect[4];
            pVinCfg->DataLanePinSelect0.DataLane5PinSelect = DataLanePinSelect[5];
            pVinCfg->DataLanePinSelect0.DataLane6PinSelect = DataLanePinSelect[6];
            pVinCfg->DataLanePinSelect0.DataLane7PinSelect = DataLanePinSelect[7];
            pVinCfg->DataLanePinSelect1_Ctrl2.DataLane8PinSelect = DataLanePinSelect[8];
            pVinCfg->DataLanePinSelect1_Ctrl2.DataLane9PinSelect = DataLanePinSelect[9];
            pVinCfg->DataLanePinSelect1_Ctrl2.DataLane10PinSelect = DataLanePinSelect[10];
            pVinCfg->DataLanePinSelect1_Ctrl2.DataLane11PinSelect = DataLanePinSelect[11];

            if (pVinGlobalCfg->SensorType == 3U) { /* MIPI*/
                /* According to VLSI team's information, 1 byte cycle skew between 0~7 is possible since they are treated as async at
                 * the output of PHY (RxByteclk0~7 treated as async from each other). Therefore, we should always set this field with
                 * value 2 to handle the potential 1 byte cycle skew caused by PHY async lane routing */
                if (EnableDeskew == 1U) {
                    pVinCfg->DataLanePinSelect1_Ctrl2.MipiMaxSkewCycle = 2; /* unit: mipi_byteclk cycle; or 4*mipi_sensor_clk cycle */
                } else {
                    pVinCfg->DataLanePinSelect1_Ctrl2.MipiMaxSkewCycle = 0;
                }
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinMipiVirtChanConfig - Configure MIPI virtual channel receiving
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pMipiVirtChanConfig Pointer to MIPI virtual channel receiving configuration
 *  @return error code
 */
UINT32 AmbaCSL_VinMipiVirtChanConfig(UINT32 VinID, const AMBA_VIN_MIPI_VC_CONFIG_s *pMipiVirtChanConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg = NULL;
    const AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s *pVinGlobalCfg = NULL;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinGlobalCfg   = pGlobalConfigData[VinID];
        pVinCfg         = pMainConfigData[VinID];

        if (pVinGlobalCfg->SensorType == 3U) {
            if (pVinCfg->SlvsEovPattern_MipiCtrl0.VirtChanNumMinus1 == 0U) {
                pVinCfg->SlvsEovPattern_MipiCtrl0.VirtChanMask        = pMipiVirtChanConfig->VirtChanMask;
                pVinCfg->SlvsEovPattern_MipiCtrl0.FSVirtChanPattern   = pMipiVirtChanConfig->VirtChan;
                pVinCfg->SlvsEovPattern_MipiCtrl0.FEVirtChanPattern   = pMipiVirtChanConfig->VirtChan;
                pVinCfg->SlvsEovPattern_MipiCtrl0.Exp0VirtChanPattern = pMipiVirtChanConfig->VirtChan;
            } else {
                RetVal = VIN_ERR_ARG;
            }
        }
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinConfigMainThrDbgPort - Configure VIN main via debugbus
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] MainCfgBufAddr Vin Main buffer address
 *  @param[in] ToggleFrameID Toggle Frame IDs
 *  @return error code
 */
UINT32 AmbaCSL_VinConfigMainThrDbgPort(UINT32 VinID, const ULONG MainCfgBufAddr, UINT32 ToggleFrameID)
{
    UINT32 RetVal;
    const AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg = NULL;

    if (AmbaWrap_memcpy(&pVinCfg, &MainCfgBufAddr, sizeof(MainCfgBufAddr)) != ERR_NONE) {
        RetVal = VIN_ERR_UNEXPECTED;
    } else {
        RetVal = DebugBusConfigVinMain(VinID, pVinCfg, ToggleFrameID);
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinCaptureConfig - Configure Vin capture window
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pCaptureWindow Pointer to VIN capture window
 *  @return error code
 */
UINT32 AmbaCSL_VinCaptureConfig(UINT32 VinID, const AMBA_CSL_VIN_WINDOW_s *pCaptureWindow)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg;
    UINT16 CropEndRow;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinCfg = pMainConfigData[VinID];

        if ((pVinCfg->DataLanePinSelect1_Ctrl2.DisableCropRegion == 0U) && (pVinCfg->MipiCtrl1_MipiExp1Offset.MipiCropEnable == 0U)) {
            pVinCfg->CropStartCol   = pCaptureWindow->OffsetX;
            pVinCfg->CropEndCol     = pCaptureWindow->OffsetX + pCaptureWindow->Width - 1U;

            pVinCfg->CropStartRow   = pCaptureWindow->OffsetY;
            CropEndRow              = pCaptureWindow->OffsetY + pCaptureWindow->Height - (UINT16)1U;
            pVinCfg->CropEndRow_SlvsSyncCodeLocation.CropEndRow = CropEndRow;
        } else {
            pVinCfg->MipiCropRowStart = pCaptureWindow->OffsetY;
            pVinCfg->MipiCropColStart = pCaptureWindow->OffsetX;

            pVinCfg->MipiCropWidthMinus1 = pCaptureWindow->Width - 1U;
            pVinCfg->MipiCropHeightMinus1 = pCaptureWindow->Height - 1U;
        }
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinGetMainCfgBufAddr - Get Vin main configuration buffer address
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pAddr Pointer to VIN main configuration buffer
 *  @return error code
 */
UINT32 AmbaCSL_VinGetMainCfgBufAddr(UINT32 VinID, ULONG *pAddr)
{
    UINT32 RetVal = VIN_ERR_NONE;
    ULONG Addr;
    const AMBA_CSL_VIN_CONFIG_DATA_s *pVinConfigAddr;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinConfigAddr = pMainConfigData[VinID];
        AmbaMisra_TypeCast(&Addr, &pVinConfigAddr);
        *pAddr = Addr;
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinGetGlobalCfgBufAddr - Get Vin global configuration buffer address
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pAddr Pointer to VIN global configuration buffer
 *  @return error code
 */
UINT32 AmbaCSL_VinGetGlobalCfgBufAddr(UINT32 VinID, ULONG *pAddr)
{
    UINT32 RetVal = VIN_ERR_NONE;
    ULONG Addr;
    const AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s *pVinGlobalCfgAddr;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinGlobalCfgAddr = pGlobalConfigData[VinID];
        AmbaMisra_TypeCast(&Addr, &pVinGlobalCfgAddr);
        *pAddr = Addr;
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinGetMSyncCfgBufAddr - Get Vin master H/V Sync configuration buffer address
 *  @param[in] MSyncID Indicate Master sync pin ID
 *  @param[in] pAddr Pointer to VIN master H/V Sync configuration buffer
 *  @return error code
 */
UINT32 AmbaCSL_VinGetMSyncCfgBufAddr(UINT32 MSyncID, ULONG *pAddr)
{
    UINT32 RetVal = VIN_ERR_NONE;
    ULONG Addr;
    const AMBA_CSL_VIN_MASTER_SYNC_CONFIG_DATA_s *pVinMsyncCfgAddr;

    if (MSyncID < AMBA_NUM_VIN_MSYNC) {
        pVinMsyncCfgAddr = pMSyncConfigData[MSyncID];
        AmbaMisra_TypeCast(&Addr, &pVinMsyncCfgAddr);
        *pAddr = Addr;
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinGetInfo - Get Vin info
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pInfo Pointer to VIN info
 *  @return error code
 */
UINT32 AmbaCSL_VinGetInfo(UINT32 VinID, AMBA_CSL_VIN_INFO_s *pInfo)
{
    UINT32 RetVal = VIN_ERR_NONE;
    const AMBA_CSL_VIN_INFO_s *pCslVinInfo;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pCslVinInfo = pVinInformation[VinID];
        if (AmbaWrap_memcpy(pInfo, pCslVinInfo, sizeof(AMBA_CSL_VIN_INFO_s)) != ERR_NONE) {
            RetVal = VIN_ERR_UNEXPECTED;
        }
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinIsEnabled - Get Vin enable status
 *  @param[in] VinID Indicate VIN channel
 *  @return vin enable status
 */
UINT32 AmbaCSL_VinIsEnabled(UINT32 VinID)
{
    UINT32 Enable;
    UINT32 BackupAddrExtensionData = AmbaCSL_IdspGetAddrExtensionData();

    if (VinID == AMBA_VIN_CHANNEL1) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_PIP_VIN, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL2) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_PIP2_VIN, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL3) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_PIP3_VIN, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL4) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_PIP4_VIN, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL5) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_PIP5_VIN, 0U);
    } else { /* default section to parse */
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN, 0U);
    }
    AmbaDelayCycles(0xfffU); /* wait for section selection */

    Enable = pAmbaIDSP_VinMainReg->Ctrl0.Enable;

    AmbaCSL_IdspSetAddrExtensionData(BackupAddrExtensionData);
    AmbaDelayCycles(0xfffU);

    return Enable;
}

/**
 *  AmbaCSL_VinSetSensorClkDriveStr - Set the driving strength of sensor clock
 *  @param[in] Value Indicate the strength
 */
void AmbaCSL_VinSetSensorClkDriveStr(UINT32 Value)
{
    UINT32 BitPos = 3U;
    UINT32 RegVal[2];

    RegVal[0] = pAmbaRCT_Reg->MiscDriveStrength[0];
    RegVal[1] = pAmbaRCT_Reg->MiscDriveStrength[1];

    if (Value == 0U) {
        RegVal[0] &= ~((UINT32)1U << (BitPos));
        RegVal[1] &= ~((UINT32)1U << (BitPos));
    } else if (Value == 1U) {
        RegVal[0] &= ~((UINT32)1U << (BitPos));
        RegVal[1] |= ((UINT32)1U << (BitPos));
    } else if (Value == 2U) {
        RegVal[0] |= ((UINT32)1U << (BitPos));
        RegVal[1] &= ~((UINT32)1U << (BitPos));
    } else if (Value == 3U) {
        RegVal[0] |= ((UINT32)1U << (BitPos));
        RegVal[1] |= ((UINT32)1U << (BitPos));
    } else {
        /* Should not happen!! */
    }

    pAmbaRCT_Reg->MiscDriveStrength[0] = RegVal[0];
    pAmbaRCT_Reg->MiscDriveStrength[1] = RegVal[1];
}
