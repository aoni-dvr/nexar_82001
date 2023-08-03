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
#if defined(CONFIG_VIN_ASIL)
#include "AmbaSafety_VIN.h"
#endif

AMBA_MIPI_REG_s *pAmbaMIPI_Reg;
AMBA_DBG_PORT_IDSP_CONTROLLER_REG_s *pAmbaIDSP_CtrlReg;
AMBA_DBG_PORT_IDSP_VIN_MAIN_REG_s *pAmbaIDSP_VinMainReg;
AMBA_DBG_PORT_IDSP_VIN_GLOBAL_REG_s *pAmbaIDSP_VinGlobalReg;
AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_REG_s *pAmbaIDSP_VinMasterSyncReg;

static inline void WriteWord(UINT32 VinID, volatile UINT32 *pAddr, UINT16 Value)
{
#if defined(CONFIG_VIN_ASIL)
    AmbaSafety_VinShadowWrite(VinID, pAddr, Value);
#else
    (void) VinID;
#endif
    (*((volatile UINT32 *) (pAddr)) = (Value));
}

#ifdef CONFIG_QNX
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin0MainConfigData;
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin1MainConfigData;
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin2MainConfigData;
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin3MainConfigData;
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin4MainConfigData;
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin8MainConfigData;
#else
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin0MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin1MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin2MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin3MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin4MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin8MainConfigData __attribute__((aligned(128)));
#endif
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin0GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin1GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin2GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin3GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin4GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin8GlobalConfigData;
static AMBA_CSL_VIN_MASTER_SYNC_CONFIG_DATA_s AmbaCSL_VinMSync0ConfigData;
static AMBA_CSL_VIN_MASTER_SYNC_CONFIG_DATA_s AmbaCSL_VinMSync1ConfigData;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin0Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin1Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin2Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin3Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin4Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin8Info;

static AMBA_CSL_VIN_CONFIG_DATA_s* const pMainConfigData[AMBA_NUM_VIN_CHANNEL] = {
    &AmbaCSL_Vin0MainConfigData,
    &AmbaCSL_Vin1MainConfigData,
    &AmbaCSL_Vin2MainConfigData,
    &AmbaCSL_Vin3MainConfigData,
    &AmbaCSL_Vin4MainConfigData,
    &AmbaCSL_Vin8MainConfigData,
};

static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s* const pGlobalConfigData[AMBA_NUM_VIN_CHANNEL] = {
    &AmbaCSL_Vin0GlobalConfigData,
    &AmbaCSL_Vin1GlobalConfigData,
    &AmbaCSL_Vin2GlobalConfigData,
    &AmbaCSL_Vin3GlobalConfigData,
    &AmbaCSL_Vin4GlobalConfigData,
    &AmbaCSL_Vin8GlobalConfigData,
};

static AMBA_CSL_VIN_MASTER_SYNC_CONFIG_DATA_s* const pMSyncConfigData[AMBA_NUM_VIN_MSYNC] = {
    &AmbaCSL_VinMSync0ConfigData,
    &AmbaCSL_VinMSync1ConfigData,
};

static AMBA_CSL_VIN_INFO_s* const pVinInformation[AMBA_NUM_VIN_CHANNEL] = {
    &AmbaCSL_Vin0Info,
    &AmbaCSL_Vin1Info,
    &AmbaCSL_Vin2Info,
    &AmbaCSL_Vin3Info,
    &AmbaCSL_Vin4Info,
    &AmbaCSL_Vin8Info,
};

/**
 *  AmbaCSL_VinSetVin0LvdsPadMode - Configure LVDS pad mode
 *  @param[in] PadMode Indicate pad mode
 *  @param[in] DataLaneEnable Enabled data pins defined in bit-wise
 *  @return error code
 */
UINT32 AmbaCSL_VinSetVin0LvdsPadMode(UINT32 PadMode, UINT32 DataLaneEnable)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 DataLaneEnableVal = DataLaneEnable;
    UINT32 SubGrpEnable = 0U;

    switch (PadMode) {
    case AMBA_VIN_PAD_MODE_SLVS:
        if ((DataLaneEnableVal & 0x00fU) != 0U) {
            SubGrpEnable |= 0x1U;
            AmbaCSL_VinSetLvdsBitMode0(0x0U);
        }
        if ((DataLaneEnableVal & 0x0f0U) != 0U) {
            SubGrpEnable |= 0x2U;
            AmbaCSL_VinSetLvdsBitMode1(0x0U);
        }
        if ((DataLaneEnableVal & 0xf00U) != 0U) {
            SubGrpEnable |= 0x4U;
            AmbaCSL_VinSetLvdsBitMode2(0x0U);
        }

        AmbaCSL_VinSetMipiMode(AmbaCSL_VinGetMipiMode() & ~SubGrpEnable);

        DataLaneEnableVal = DataLaneEnableVal & 0xfffU;
        AmbaCSL_VinSetLvCmosMode(AmbaCSL_VinGetLvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetLvCmosModeSpclk(AmbaCSL_VinGetLvCmosModeSpclk() & ~SubGrpEnable);
        break;
    case AMBA_VIN_PAD_MODE_DVP_LVCMOS:
        AmbaCSL_VinSetLvdsBitMode0(0x0U);
        AmbaCSL_VinSetLvdsBitMode1(0x1U);
        AmbaCSL_VinSetLvdsBitMode2(0x1U);

        AmbaCSL_VinSetMipiMode(AmbaCSL_VinGetMipiMode() & ~0x3U);
        AmbaCSL_VinSetLvCmosMode(AmbaCSL_VinGetLvCmosMode() | 0xffU);
        AmbaCSL_VinSetLvCmosModeSpclk(AmbaCSL_VinGetLvCmosModeSpclk() | 0x3U);
        break;
    case AMBA_VIN_PAD_MODE_DVP_LVDS:
        AmbaCSL_VinSetLvdsBitMode0(0x0U);
        AmbaCSL_VinSetLvdsBitMode1(0x0U);
        AmbaCSL_VinSetLvdsBitMode2(0x0U);

        AmbaCSL_VinSetMipiMode(0x0U);
        AmbaCSL_VinSetLvCmosMode(0x0U);
        AmbaCSL_VinSetLvCmosModeSpclk(0x0U);
        break;
    case AMBA_VIN_PAD_MODE_MIPI:
        if ((DataLaneEnableVal & 0x0ffU) == 0x0ffU) {
            AmbaCSL_VinSetLvdsBitMode0(0x0U);
            AmbaCSL_VinSetLvdsBitMode1(0x0U);
            AmbaCSL_VinSetMipiMode(AmbaCSL_VinGetMipiMode() | 0x3U);

            AmbaCSL_VinSetLvCmosMode(AmbaCSL_VinGetLvCmosMode() & ~0x0ffU);
            AmbaCSL_VinSetLvCmosModeSpclk(AmbaCSL_VinGetLvCmosModeSpclk() & ~0x3U);
        } else {
            AmbaCSL_VinSetLvdsBitMode0(0x0U);
            AmbaCSL_VinSetMipiMode(AmbaCSL_VinGetMipiMode() | 0x1U);

            DataLaneEnableVal = DataLaneEnableVal & 0x00fU;
            AmbaCSL_VinSetLvCmosMode(AmbaCSL_VinGetLvCmosMode() & ~DataLaneEnableVal);
            AmbaCSL_VinSetLvCmosModeSpclk(AmbaCSL_VinGetLvCmosModeSpclk() & ~0x1U);
        }
        break;
    default:
        RetVal = VIN_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinSetVin1LvdsPadMode - Configure LVDS pad mode
 *  @param[in] PadMode Indicate pad mode
 *  @param[in] DataLaneEnable Enabled data pins defined in bit-wise
 *  @return error code
 */
UINT32 AmbaCSL_VinSetVin1LvdsPadMode(UINT32 PadMode, UINT32 DataLaneEnable)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 DataLaneEnableVal = DataLaneEnable;

    DataLaneEnableVal = DataLaneEnableVal & 0xf00U;

    switch (PadMode) {
    case AMBA_VIN_PAD_MODE_SLVS:
        AmbaCSL_VinSetLvdsBitMode2(0x1U);

        AmbaCSL_VinSetMipiMode(AmbaCSL_VinGetMipiMode() & ~0x4U);

        AmbaCSL_VinSetLvCmosMode(AmbaCSL_VinGetLvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetLvCmosModeSpclk(AmbaCSL_VinGetLvCmosModeSpclk() & ~0x4U);
        break;
    case AMBA_VIN_PAD_MODE_MIPI:
        AmbaCSL_VinSetLvdsBitMode2(0x1U);

        AmbaCSL_VinSetMipiMode(AmbaCSL_VinGetMipiMode() | 0x4U);

        AmbaCSL_VinSetLvCmosMode(AmbaCSL_VinGetLvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetLvCmosModeSpclk(AmbaCSL_VinGetLvCmosModeSpclk() & ~0x4U);
        break;
    default:
        RetVal = VIN_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinSetVin8LvdsPadMode - Configure LVDS pad mode
 *  @param[in] PadMode Indicate pad mode
 *  @param[in] DataLaneEnable Enabled data pins defined in bit-wise
 *  @return error code
 */
UINT32 AmbaCSL_VinSetVin8LvdsPadMode(UINT32 PadMode, UINT32 DataLaneEnable)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 DataLaneEnableVal = DataLaneEnable;

    DataLaneEnableVal = DataLaneEnableVal & 0x0f0U;

    switch (PadMode) {
    case AMBA_VIN_PAD_MODE_SLVS:
        AmbaCSL_VinSetLvdsBitMode1(0x1U);

        AmbaCSL_VinSetMipiMode(AmbaCSL_VinGetMipiMode() & ~0x2U);

        AmbaCSL_VinSetLvCmosMode(AmbaCSL_VinGetLvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetLvCmosModeSpclk(AmbaCSL_VinGetLvCmosModeSpclk() & ~0x2U);
        break;
    case AMBA_VIN_PAD_MODE_MIPI:
        AmbaCSL_VinSetLvdsBitMode1(0x1U);

        AmbaCSL_VinSetMipiMode(AmbaCSL_VinGetMipiMode() | 0x2U);

        AmbaCSL_VinSetLvCmosMode(AmbaCSL_VinGetLvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetLvCmosModeSpclk(AmbaCSL_VinGetLvCmosModeSpclk() & ~0x2U);
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
        *pLaneShift = 8U;
    } else if (VinID == AMBA_VIN_CHANNEL2) {
        *pLaneShift = 0U;
    } else if (VinID == AMBA_VIN_CHANNEL3) {
        *pLaneShift = 0U;
    } else if (VinID == AMBA_VIN_CHANNEL4) {
        *pLaneShift = 0U;
    } else { /* (VinID == AMBA_VIN_CHANNEL8) */
        *pLaneShift = 4U;
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

#if defined(CONFIG_VIN_ASIL)
        AmbaSafety_VinShadowLock();
#endif
        BackupAddrExtensionData = AmbaCSL_IdspGetAddrExtensionData();
        if (VinID == AMBA_VIN_CHANNEL1) {
            AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN1, 0U);
        } else if (VinID == AMBA_VIN_CHANNEL2) {
            AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN2, 0U);
        } else if (VinID == AMBA_VIN_CHANNEL3) {
            AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN3, 0U);
        } else if (VinID == AMBA_VIN_CHANNEL4) {
            AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN4, 0U);
        } else if (VinID == AMBA_VIN_CHANNEL8) {
            AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN8, 0U);
        } else { /* default section */
            AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN0, 0U);
        }
        AmbaDelayCycles(0x3fffU);

        AmbaMisra_TypeCast(&BaseAddr, &pAmbaIDSP_VinGlobalReg);

        for (i = 0U; i < AMBA_CSL_VIN_GLOBAL_CONFIG_SIZE; i++) {
            Addr = BaseAddr + (i * 4U);
            AmbaMisra_TypeCast(&pAddr, &Addr);
            WriteWord(VinID, pAddr, GlobalConfigData[i]);
        }
        AmbaCSL_IdspSetAddrExtensionData(BackupAddrExtensionData);
        AmbaDelayCycles(0x3fffU);
#if defined(CONFIG_VIN_ASIL)
        AmbaSafety_VinShadowUnLock();
#endif
    }
}

static UINT32 DebugBusConfigVinMain(UINT32 VinID, const AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg)
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

#if defined(CONFIG_VIN_ASIL)
    AmbaSafety_VinShadowLock();
#endif
    BackupAddrExtensionData = AmbaCSL_IdspGetAddrExtensionData();
    if (VinID == AMBA_VIN_CHANNEL1) {
        AmbaCSL_IdspResetVin1PipelineSection();
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN1, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL2) {
        AmbaCSL_IdspResetVin2PipelineSection();
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN2, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL3) {
        AmbaCSL_IdspResetVin3PipelineSection();
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN3, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL4) {
        AmbaCSL_IdspResetVin4PipelineSection();
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN4, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL8) {
        AmbaCSL_IdspResetVin8PipelineSection();
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN8, 0U);
    } else { /* default section */
        AmbaCSL_IdspResetVin0PipelineSection();
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN0, 0U);
    }
    AmbaDelayCycles(0x3fffU);

    MainConfigData[2] &= ~(UINT16)0x4000U; /* do not disable vin after vsync */
    MainConfigData[8] |= 0x2U;     /* DBG_NON_BLOCKING=1 */

    AmbaMisra_TypeCast(&BaseAddr, &pAmbaIDSP_VinMainReg);

    for (i = 0U; i <= 63U; i++) {
        Addr = BaseAddr + (i * 4U);
        AmbaMisra_TypeCast(&pAddr, &Addr);
        WriteWord(VinID, pAddr, MainConfigData[i]);
    }
    MainConfigData[128] = 1U; /* manually activate config (for debug only) */
    Addr = BaseAddr + (128U * 4U);
    AmbaMisra_TypeCast(&pAddr, &Addr);
    WriteWord(VinID, pAddr, MainConfigData[128]); /* debug_command_shift is write-only register */

    AmbaCSL_IdspSetAddrExtensionData(BackupAddrExtensionData);
    AmbaDelayCycles(0x3fffU);
#if defined(CONFIG_VIN_ASIL)
    AmbaSafety_VinShadowUnLock();
#endif

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

#if defined(CONFIG_VIN_ASIL)
    AmbaSafety_VinShadowLock();
#endif
    BackupAddrExtensionData = AmbaCSL_IdspGetAddrExtensionData();
    if (MSyncID == AMBA_VIN_MSYNC0) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN0, 0U);
        AmbaDelayCycles(0x3fffU);
        if (AmbaWrap_memcpy(MSyncConfigData, &AmbaCSL_VinMSync0ConfigData, sizeof(MSyncConfigData)) != ERR_NONE) {
            RetVal = VIN_ERR_UNEXPECTED;
        }
    } else { /* MSyncID == AMBA_VIN_MSYNC1 */
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN1, 0U);
        AmbaDelayCycles(0x3fffU);
        if (AmbaWrap_memcpy(MSyncConfigData, &AmbaCSL_VinMSync1ConfigData, sizeof(MSyncConfigData)) != ERR_NONE) {
            RetVal = VIN_ERR_UNEXPECTED;
        }
    }

    AmbaMisra_TypeCast(&BaseAddr, &pAmbaIDSP_VinMasterSyncReg);

    if (RetVal == VIN_ERR_NONE) {
        for (i = 0; i < AMBA_CSL_VIN_MASTER_SYNC_CONFIG_SIZE; i++) {
            Addr = BaseAddr + (i * 4U);
            AmbaMisra_TypeCast(&pAddr, &Addr);
            WriteWord(MSyncID, pAddr, MSyncConfigData[i]);
        }
    }

    AmbaCSL_IdspSetAddrExtensionData(BackupAddrExtensionData);
    AmbaDelayCycles(0x3fffU);
#if defined(CONFIG_VIN_ASIL)
    AmbaSafety_VinShadowUnLock();
#endif

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

    pVinGlobalCfg = pGlobalConfigData[VinID];
    if (AmbaWrap_memset(pVinGlobalCfg, 0, sizeof(AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s)) == ERR_NONE) {

        pVinGlobalCfg->SensorType    = 0U;
        pVinGlobalCfg->SlvsClkMode   = 1U;
        pVinGlobalCfg->AFIFOBypass   = 0U;

        DebugBusConfigVinGlobal(VinID);
    } else {
        RetVal  = VIN_ERR_UNEXPECTED;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinMipiConfigGlobal - Configure VIN global configuration for MIPI
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] MipiVCEnable Enable(1)/Disable(0) MIPI virtual channel
 *  @return error code
 */
UINT32 AmbaCSL_VinMipiConfigGlobal(UINT32 VinID, UINT32 MipiVCEnable)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s *pVinGlobalCfg = NULL;

    pVinGlobalCfg = pGlobalConfigData[VinID];
    if (AmbaWrap_memset(pVinGlobalCfg, 0, sizeof(AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s)) == ERR_NONE) {

        pVinGlobalCfg->SensorType    = 3U;
        pVinGlobalCfg->SlvsClkMode   = 0U;
        pVinGlobalCfg->AFIFOBypass   = 1U;
        pVinGlobalCfg->MipiVCEnable  = (MipiVCEnable == 0U) ? 0U : 1U;

        DebugBusConfigVinGlobal(VinID);
    } else {
        RetVal = VIN_ERR_UNEXPECTED;
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

    pVinGlobalCfg = pGlobalConfigData[VinID];
    if (AmbaWrap_memset(pVinGlobalCfg, 0, sizeof(AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s)) == ERR_NONE) {

        pVinGlobalCfg->SensorType    = 2U;
        pVinGlobalCfg->SlvsClkMode   = 0U;
        pVinGlobalCfg->AFIFOBypass   = 1U;

        DebugBusConfigVinGlobal(VinID);
    } else {
        RetVal = VIN_ERR_UNEXPECTED;
    }

    return RetVal;
}

static void VinActiveRegionSizeConfig(AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg, const AMBA_VIN_RX_HV_SYNC_s *pRxHvSyncCtrl)
{
    if (pRxHvSyncCtrl->NumActivePixels != 0U) {
        pVinCfg->Ctrl2_ActiveRegionWidth.ActiveRegionWidth = (UINT16)(pRxHvSyncCtrl->NumActivePixels - 1U);
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
    pVinCfg->SlvsCtrl0.SyncType = 1U;

    pVinCfg->SlvsSyncDetectMask = 0x8000U;
    pVinCfg->SlvsSyncDetectPattern = 0x8000U;

    pVinCfg->SlvsSyncCompareMask = ~CustomSyncCode->SyncCodeMask;
    pVinCfg->SlvsSolPattern = CustomSyncCode->PatternSol;
    pVinCfg->SlvsEolPattern = CustomSyncCode->PatternEol;
    pVinCfg->SlvsSofPattern = CustomSyncCode->PatternSof;
    pVinCfg->SlvsEofPattern = CustomSyncCode->PatternEof;
    pVinCfg->SlvsSovPattern = CustomSyncCode->PatternSov;
    pVinCfg->SlvsEovPattern = CustomSyncCode->PatternEov;

    pVinCfg->SlvsCtrl0.PatMaskAlignment = CustomSyncCode->PatternAlign;
    pVinCfg->SlvsCtrl1_WatchdogTimeoutL.DetectSol = CustomSyncCode->SolDetectEnable;
    pVinCfg->SlvsCtrl1_WatchdogTimeoutL.DetectEol = CustomSyncCode->EolDetectEnable;
    pVinCfg->SlvsCtrl1_WatchdogTimeoutL.DetectSof = CustomSyncCode->SofDetectEnable;
    pVinCfg->SlvsCtrl1_WatchdogTimeoutL.DetectEof = CustomSyncCode->EofDetectEnable;
    pVinCfg->SlvsCtrl1_WatchdogTimeoutL.DetectSov = CustomSyncCode->SovDetectEnable;
    pVinCfg->SlvsCtrl1_WatchdogTimeoutL.DetectEov = CustomSyncCode->EovDetectEnable;
}

static void VinITU656SyncCodeConfig(AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg, UINT32 Interlace)
{
    pVinCfg->SlvsCtrl0.SyncType = 1U;
    pVinCfg->SlvsSyncDetectMask = 0x8000U;
    pVinCfg->SlvsSyncDetectPattern = 0x8000U;

    pVinCfg->SlvsCtrl1_WatchdogTimeoutL.DetectSol    = 1U;
    pVinCfg->SlvsCtrl1_WatchdogTimeoutL.DetectEol    = 0U;
    pVinCfg->SlvsCtrl1_WatchdogTimeoutL.DetectSof    = 0U;
    pVinCfg->SlvsCtrl1_WatchdogTimeoutL.DetectEof    = 0U;
    pVinCfg->SlvsCtrl1_WatchdogTimeoutL.DetectSov    = 1U;
    pVinCfg->SlvsCtrl1_WatchdogTimeoutL.DetectEov    = 0U;

    pVinCfg->SlvsCtrl0.PatMaskAlignment = 1U;

    if (Interlace == 1U) {
        pVinCfg->SlvsSyncCompareMask = 0xb800U;
        pVinCfg->SlvsSolPattern = 0x8000U;
        pVinCfg->SlvsEolPattern = 0U;
        pVinCfg->SlvsSofPattern = 0U;
        pVinCfg->SlvsEofPattern = 0U;
        pVinCfg->SlvsSovPattern = 0xa800U;
        pVinCfg->SlvsEovPattern = 0U;
    } else {
        pVinCfg->SlvsSyncCompareMask = 0xff00U;
        pVinCfg->SlvsSolPattern = 0x8000U;
        pVinCfg->SlvsEolPattern = 0x9d00U;
        pVinCfg->SlvsSofPattern = 0U;
        pVinCfg->SlvsEofPattern = 0U;
        pVinCfg->SlvsSovPattern = 0xab00U;
        pVinCfg->SlvsEovPattern = 0xb600U;
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
        pVinCfg->DataLanePinSelect1.DataLane8PinSelect = DataLanePinSelect[8];
        pVinCfg->DataLanePinSelect1.DataLane9PinSelect = DataLanePinSelect[9];
        pVinCfg->DataLanePinSelect1.DataLane10PinSelect = DataLanePinSelect[10];
        pVinCfg->DataLanePinSelect1.DataLane11PinSelect = DataLanePinSelect[11];
        pVinCfg->DataLanePinSelect1.DataLane12PinSelect = 12U; /* Not used. Set to default reset value */
        pVinCfg->DataLanePinSelect1.DataLane13PinSelect = 13U; /* Not used. Set to default reset value */
        pVinCfg->DataLanePinSelect1.DataLane14PinSelect = 14U; /* Not used. Set to default reset value */
        pVinCfg->DataLanePinSelect1.DataLane15PinSelect = 15U; /* Not used. Set to default reset value */

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

        pVinCfg->SlvsCtrl0.SyncInterleaving = pVinSlvsConfig->SyncDetectCtrl.SyncInterleaving;
        pVinCfg->SlvsCtrl0.SyncTolerance = 1U;
        pVinCfg->SlvsCtrl0.SyncAllLanes = 0U;

        pVinCfg->SlvsCtrl0.AllowPartialCode       = 0U;
        pVinCfg->SlvsCtrl0.LockSyncPhase          = 0U;
        pVinCfg->SlvsCtrl0.SyncCorrection         = 1U;
        pVinCfg->SlvsCtrl0.DeskewEnable           = 1U;
        pVinCfg->SlvsCtrl0.Enable656Ecc           = 0U;
        pVinCfg->SlvsCtrl0.UnlockOnTimeout        = 1U;
        pVinCfg->SlvsCtrl0.UnlockOnDeskewError    = 1U;
        pVinCfg->SlvsCtrl0.LineReorder            = 0U;

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
    /* Vin internal sensor clock rate = sensor output clock rate / 4 */
    UINT32 DelayedVsync = pVinMipiConfig->Config.DelayedVsync / 4U;

    pVinCfg     = pMainConfigData[VinID];
    pVinInfo    = pVinInformation[VinID];

    RetVal = AmbaWrap_memset(pVinCfg, 0, sizeof(AMBA_CSL_VIN_CONFIG_DATA_s));
    RetVal |= AmbaWrap_memset(pVinInfo, 0, sizeof(AMBA_CSL_VIN_INFO_s));

    if (RetVal == ERR_NONE) {

        /* VIN control register */
        pVinCfg->Ctrl.SwReset = 0U;
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
        pVinCfg->MipiCtrl0.VirtChanMask     = 0x3U;
        pVinCfg->MipiCtrl0.VirtChanPattern  = 0x0U;
        pVinCfg->MipiCtrl0.DataTypeMask     = pVinMipiConfig->DataTypeMask;
        pVinCfg->MipiCtrl0.DataTypePattern  = pVinMipiConfig->DataType;
        pVinCfg->MipiCtrl0.ByteSwapEnable   = 0U;
        pVinCfg->MipiCtrl0.EccEnable        = 1U;
        pVinCfg->MipiCtrl0.ForwardEccEnable = 1U;

        pVinCfg->DataLanePinSelect0.DataLane0PinSelect = 0U;
        pVinCfg->DataLanePinSelect0.DataLane1PinSelect = 1U;
        pVinCfg->DataLanePinSelect0.DataLane2PinSelect = 2U;
        pVinCfg->DataLanePinSelect0.DataLane3PinSelect = 3U;
        pVinCfg->DataLanePinSelect0.DataLane4PinSelect = 4U;
        pVinCfg->DataLanePinSelect0.DataLane5PinSelect = 5U;
        pVinCfg->DataLanePinSelect0.DataLane6PinSelect = 6U;
        pVinCfg->DataLanePinSelect0.DataLane7PinSelect = 7U;
        pVinCfg->DataLanePinSelect1.DataLane8PinSelect = 8U;
        pVinCfg->DataLanePinSelect1.DataLane9PinSelect = 9U;
        pVinCfg->DataLanePinSelect1.DataLane10PinSelect = 10U;
        pVinCfg->DataLanePinSelect1.DataLane11PinSelect = 11U;
        pVinCfg->DataLanePinSelect1.DataLane12PinSelect = 12U; /* Not used. Set to default reset value */
        pVinCfg->DataLanePinSelect1.DataLane13PinSelect = 13U; /* Not used. Set to default reset value */
        pVinCfg->DataLanePinSelect1.DataLane14PinSelect = 14U; /* Not used. Set to default reset value */
        pVinCfg->DataLanePinSelect1.DataLane15PinSelect = 15U; /* Not used. Set to default reset value */

        /* According to VLSI team's information, 1 byte cycle skew between 0~7 is possible since they are treated as async at
         * the output of PHY (RxByteclk0~7 treated as async from each other). Therefore, we should always set this field with
         * value 2 to handle the potential 1 byte cycle skew caused by PHY async lane routing */
        pVinCfg->Ctrl2_ActiveRegionWidth.MipiMaxSkewCycle = 2; /* unit: mipi_byteclk cycle; or 4*mipi_sensor_clk cycle */

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

        pVinCfg->ErrorMask.WatchdogTimeoutIntr = 0;
        pVinCfg->ErrorMask.ShortFrameIntr = 1;
        pVinCfg->ErrorMask.ShortLineIntr = 1;
        pVinCfg->ErrorMask.SfifoOverFlowIntr = 1;
        pVinCfg->ErrorMask.AfifoOverflowIntr = 1;
        pVinCfg->ErrorMask.LostLockAfterSofIntr = 0;
        pVinCfg->ErrorMask.UnknownSyncCodeIntr = 0;
        pVinCfg->ErrorMask.Uncorrectable656ErrorIntr = 0;
        pVinCfg->ErrorMask.SerialDeskewErrorIntr = 1;
        pVinCfg->ErrorMask.FrameSyncErrorIntr = 1;
        pVinCfg->ErrorMask.CrcErrorIntr = 1;
        pVinCfg->ErrorMask.EccError1BitIntr = 1;
        pVinCfg->ErrorMask.EccError2BitIntr = 1;

        pVinCfg->ErrorMask.WatchdogTimeoutECR = 0;
        pVinCfg->ErrorMask.ShortFrameECR = 1;
        pVinCfg->ErrorMask.ShortLineECR = 1;
        pVinCfg->ErrorMask.SfifoOverFlowECR = 1;
        pVinCfg->ErrorMask.AfifoOverflowECR = 1;
        pVinCfg->ErrorMask.LostLockAfterSofECR = 0;
        pVinCfg->ErrorMask.UnknownSyncCodeECR = 0;
        pVinCfg->ErrorMask.Uncorrectable656ErrorECR = 0;
        pVinCfg->ErrorMask.SerialDeskewErrorECR = 1;
        pVinCfg->ErrorMask.FrameSyncErrorECR = 1;
        pVinCfg->ErrorMask.CrcErrorECR = 1;
        pVinCfg->ErrorMask.EccError1BitECR = 1;
        pVinCfg->ErrorMask.EccError2BitECR = 1;

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
        } else { /* AMBA_VIN_DVP_WIDE_2_PIXELS */
            pVinCfg->DvpCtrl_SyncPinSelect.ParallelWidth = 1U;
        }

        if (pVinDvpConfig->DataLatchEdge == AMBA_VIN_DVP_LATCH_EDGE_RISING) {
            pVinCfg->DvpCtrl_SyncPinSelect.ParallelDataRate = 0U;
            pVinCfg->DvpCtrl_SyncPinSelect.ParallelDataEdge = 0U;
        } else if (pVinDvpConfig->DataLatchEdge == AMBA_VIN_DVP_LATCH_EDGE_FALLING) {
            pVinCfg->DvpCtrl_SyncPinSelect.ParallelDataRate = 0U;
            pVinCfg->DvpCtrl_SyncPinSelect.ParallelDataEdge = 1U;
        } else {    /* AMBA_VIN_DVP_LATCH_EDGE_BOTH */
            pVinCfg->DvpCtrl_SyncPinSelect.ParallelDataRate = 1U;
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

        } else {    /* (pVinDvpConfig->SyncDetectCtrl.SyncType == AMBA_VIN_DVP_SYNC_TYPE_EMB_SYNC) */
            pVinCfg->DvpCtrl_SyncPinSelect.SyncType = 0U;

            pVinCfg->DvpCtrl_SyncPinSelect.ParallelEmbSyncLoc = pVinDvpConfig->SyncDetectCtrl.EmbSyncConfig.SyncLocate;

            if (pVinDvpConfig->SyncDetectCtrl.EmbSyncConfig.ITU656Type == 0U) {
                VinCustomSyncCodeConfig(pVinCfg, &pVinDvpConfig->SyncDetectCtrl.EmbSyncConfig.CustomSyncCode);
            } else {    /* (pVinDvpConfig->SyncDetectCtrl.EmbSyncConfig.ITU656Type == 1U) */
                /* ITU-656 */
                VinITU656SyncCodeConfig(pVinCfg, pVinDvpConfig->Config.FrameRate.Interlace);
            }

            pVinCfg->SlvsCtrl0.AllowPartialCode       = 0U;
            pVinCfg->SlvsCtrl0.LockSyncPhase          = 0U;
            pVinCfg->SlvsCtrl0.SyncCorrection         = 1U;
            pVinCfg->SlvsCtrl0.DeskewEnable           = 1U;
            pVinCfg->SlvsCtrl0.Enable656Ecc           = 0U;
            pVinCfg->SlvsCtrl0.UnlockOnTimeout        = 1U;
            pVinCfg->SlvsCtrl0.UnlockOnDeskewError    = 1U;
            pVinCfg->SlvsCtrl0.LineReorder            = 0U;

            pVinCfg->SlvsCtrl0.SyncInterleaving = 0U;
            pVinCfg->SlvsCtrl0.SyncTolerance = 1U;
            pVinCfg->SlvsCtrl0.SyncAllLanes = 0U;
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
    UINT8 DataLanePinSelect[AMBA_VIN_NUM_DATA_LANE] = {0U};
    UINT32 i;
    UINT16 DataLaneEnable = 0U;

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
            }
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
        pVinCfg->DataLanePinSelect1.DataLane8PinSelect = DataLanePinSelect[8];
        pVinCfg->DataLanePinSelect1.DataLane9PinSelect = DataLanePinSelect[9];
        pVinCfg->DataLanePinSelect1.DataLane10PinSelect = DataLanePinSelect[10];
        pVinCfg->DataLanePinSelect1.DataLane11PinSelect = DataLanePinSelect[11];
        pVinCfg->DataLanePinSelect1.DataLane12PinSelect = 12U; /* Not used. Set to default reset value */
        pVinCfg->DataLanePinSelect1.DataLane13PinSelect = 13U; /* Not used. Set to default reset value */
        pVinCfg->DataLanePinSelect1.DataLane14PinSelect = 14U; /* Not used. Set to default reset value */
        pVinCfg->DataLanePinSelect1.DataLane15PinSelect = 15U; /* Not used. Set to default reset value */

        if (pVinGlobalCfg->SensorType == 3U) { /* MIPI*/
            /* According to VLSI team's information, 1 byte cycle skew between 0~7 is possible since they are treated as async at
             * the output of PHY (RxByteclk0~7 treated as async from each other). Therefore, we should always set this field with
             * value 2 to handle the potential 1 byte cycle skew caused by PHY async lane routing */
            pVinCfg->Ctrl2_ActiveRegionWidth.MipiMaxSkewCycle = 2U; /* unit: mipi_byteclk cycle; or 4*mipi_sensor_clk cycle */
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

    pVinGlobalCfg   = pGlobalConfigData[VinID];
    pVinCfg         = pMainConfigData[VinID];

    if (pVinGlobalCfg->SensorType == 3U) {
        pVinCfg->MipiCtrl0.VirtChanMask    = pMipiVirtChanConfig->VirtChanMask;
        pVinCfg->MipiCtrl0.VirtChanPattern = pMipiVirtChanConfig->VirtChan;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinMipiEmbDataConfig - Configure MIPI embedded data
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pMipiEmbDataConfig Pointer to MIPI embedded data configuration
 *  @return error code
 */
UINT32 AmbaCSL_VinMipiEmbDataConfig(UINT32 VinID, const AMBA_VIN_MIPI_EMB_DATA_CONFIG_s *pMipiEmbDataConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg = NULL;
    const AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s *pVinGlobalCfg = NULL;

    pVinGlobalCfg   = pGlobalConfigData[VinID];
    pVinCfg         = pMainConfigData[VinID];

    if (pVinGlobalCfg->SensorType == 3U) {
        pVinCfg->MipiCtrl0.EDDataTypeMask    = pMipiEmbDataConfig->DataTypeMask;
        pVinCfg->MipiCtrl0.EDDataTypePattern = pMipiEmbDataConfig->DataType;
        pVinCfg->MipiCtrl0.MipiEDEnable      = 1U;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinConfigMainThrDbgPort - Configure VIN main via debugbus
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] MainCfgBufAddr Vin Main buffer address
 *  @return error code
 */
UINT32 AmbaCSL_VinConfigMainThrDbgPort(UINT32 VinID, const ULONG MainCfgBufAddr)
{
    UINT32 RetVal;
    const AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg = NULL;

    if (AmbaWrap_memcpy(&pVinCfg, &MainCfgBufAddr, sizeof(MainCfgBufAddr)) == ERR_NONE) {
        RetVal = DebugBusConfigVinMain(VinID, pVinCfg);
    } else {
        RetVal = VIN_ERR_UNEXPECTED;
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

    pVinCfg = pMainConfigData[VinID];

    pVinCfg->CropStartCol   = pCaptureWindow->OffsetX;
    pVinCfg->CropEndCol     = pCaptureWindow->OffsetX + pCaptureWindow->Width - 1U;

    pVinCfg->CropStartRow   = pCaptureWindow->OffsetY;
    pVinCfg->CropEndRow     = pCaptureWindow->OffsetY + pCaptureWindow->Height - 1U;

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

    pVinConfigAddr = pMainConfigData[VinID];
    AmbaMisra_TypeCast(&Addr, &pVinConfigAddr);
    *pAddr = Addr;

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

    pCslVinInfo = pVinInformation[VinID];
    if (AmbaWrap_memcpy(pInfo, pCslVinInfo, sizeof(AMBA_CSL_VIN_INFO_s)) != ERR_NONE) {
        RetVal = VIN_ERR_UNEXPECTED;
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
    UINT32 BackupAddrExtensionData;

#if defined(CONFIG_VIN_ASIL)
    AmbaSafety_VinShadowLock();
#endif
    BackupAddrExtensionData = AmbaCSL_IdspGetAddrExtensionData();
    if (VinID == AMBA_VIN_CHANNEL1) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN1, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL2) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN2, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL3) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN3, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL4) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN4, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL8) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN8, 0U);
    } else { /* default section to parse */
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN0, 0U);
    }
    AmbaDelayCycles(0x3fffU);

    Enable = pAmbaIDSP_VinMainReg->Ctrl0.Enable;

    AmbaCSL_IdspSetAddrExtensionData(BackupAddrExtensionData);
    AmbaDelayCycles(0x3fffU);

#if defined(CONFIG_VIN_ASIL)
    AmbaSafety_VinShadowUnLock();
#endif

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

    /* bit 3 for clk_si, bit 4 for clk_si1 */
    if (Value == 0U) {
        RegVal[0] &= ~((UINT32)3U << (BitPos));
        RegVal[1] &= ~((UINT32)3U << (BitPos));
    } else if (Value == 1U) {
        RegVal[0] &= ~((UINT32)3U << (BitPos));
        RegVal[1] |=  ((UINT32)3U << (BitPos));
    } else if (Value == 2U) {
        RegVal[0] |=  ((UINT32)3U << (BitPos));
        RegVal[1] &= ~((UINT32)3U << (BitPos));
    } else {    /*(Value == 3U) */
        RegVal[0] |=  ((UINT32)3U << (BitPos));
        RegVal[1] |=  ((UINT32)3U << (BitPos));
    }

    pAmbaRCT_Reg->MiscDriveStrength[0] = RegVal[0];
    pAmbaRCT_Reg->MiscDriveStrength[1] = RegVal[1];
}
