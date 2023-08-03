/**
 *  @file AmbaCSL_VIN.c
 *
 *  @copyright Copyright (c) 2021 Ambarella International LP
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
#include "AmbaMisraFix.h"

#define VIN_MIPI_VC_REG_MAX_VALUE     0x03U

AMBA_MIPI_REG_s *pAmbaMIPI_Reg;
AMBA_DBG_PORT_IDSP_CONTROLLER_REG_s *pAmbaIDSP_CtrlReg;
AMBA_DBG_PORT_IDSP_VIN_MAIN_REG_s *pAmbaIDSP_VinMainReg;
AMBA_DBG_PORT_IDSP_VIN_GLOBAL_REG_s *pAmbaIDSP_VinGlobalReg;
AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_REG_s *pAmbaIDSP_VinMasterSyncReg;
AMBA_DCPHY_REG_s *pAmbaDCPHY_Regs[2];

static inline void DelayMs(UINT32 TimeInMs)
{
    AmbaDelayCycles(TimeInMs * 1000000U);
}

static UINT8 VinSection[AMBA_NUM_VIN_CHANNEL] = {
    AMBA_DBG_PORT_IDSP_VIN0,
    AMBA_DBG_PORT_IDSP_VIN1,
    AMBA_DBG_PORT_IDSP_VIN2,
    AMBA_DBG_PORT_IDSP_VIN3,
    AMBA_DBG_PORT_IDSP_VIN4,
    AMBA_DBG_PORT_IDSP_VIN5,
    AMBA_DBG_PORT_IDSP_VIN6,
    AMBA_DBG_PORT_IDSP_VIN7,
    AMBA_DBG_PORT_IDSP_VIN8,
    AMBA_DBG_PORT_IDSP_VIN9,
    AMBA_DBG_PORT_IDSP_VIN10,
    AMBA_DBG_PORT_IDSP_VIN11,
    AMBA_DBG_PORT_IDSP_VIN12,
    AMBA_DBG_PORT_IDSP_VIN13,
};

static inline void WriteWord(volatile UINT32 *Addr, UINT16 d)
{
    (*((volatile UINT32 *) (Addr)) = (d));
}

static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin0MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin1MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin2MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin3MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin4MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin5MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin6MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin7MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin8MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin9MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin10MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin11MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin12MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_CONFIG_DATA_s AmbaCSL_Vin13MainConfigData __attribute__((aligned(128)));
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin0GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin1GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin2GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin3GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin4GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin5GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin6GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin7GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin8GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin9GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin10GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin11GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin12GlobalConfigData;
static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s AmbaCSL_Vin13GlobalConfigData;
static AMBA_CSL_VIN_MASTER_SYNC_CONFIG_DATA_s AmbaCSL_VinMSync0ConfigData;
static AMBA_CSL_VIN_MASTER_SYNC_CONFIG_DATA_s AmbaCSL_VinMSync1ConfigData;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin0Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin1Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin2Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin3Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin4Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin5Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin6Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin7Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin8Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin9Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin10Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin11Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin12Info;
static AMBA_CSL_VIN_INFO_s AmbaCSL_Vin13Info;

static AMBA_CSL_VIN_CONFIG_DATA_s* const pMainConfigData[AMBA_NUM_VIN_CHANNEL] = {
    &AmbaCSL_Vin0MainConfigData,
    &AmbaCSL_Vin1MainConfigData,
    &AmbaCSL_Vin2MainConfigData,
    &AmbaCSL_Vin3MainConfigData,
    &AmbaCSL_Vin4MainConfigData,
    &AmbaCSL_Vin5MainConfigData,
    &AmbaCSL_Vin6MainConfigData,
    &AmbaCSL_Vin7MainConfigData,
    &AmbaCSL_Vin8MainConfigData,
    &AmbaCSL_Vin9MainConfigData,
    &AmbaCSL_Vin10MainConfigData,
    &AmbaCSL_Vin11MainConfigData,
    &AmbaCSL_Vin12MainConfigData,
    &AmbaCSL_Vin13MainConfigData,
};

static AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s* const pGlobalConfigData[AMBA_NUM_VIN_CHANNEL] = {
    &AmbaCSL_Vin0GlobalConfigData,
    &AmbaCSL_Vin1GlobalConfigData,
    &AmbaCSL_Vin2GlobalConfigData,
    &AmbaCSL_Vin3GlobalConfigData,
    &AmbaCSL_Vin4GlobalConfigData,
    &AmbaCSL_Vin5GlobalConfigData,
    &AmbaCSL_Vin6GlobalConfigData,
    &AmbaCSL_Vin7GlobalConfigData,
    &AmbaCSL_Vin8GlobalConfigData,
    &AmbaCSL_Vin9GlobalConfigData,
    &AmbaCSL_Vin10GlobalConfigData,
    &AmbaCSL_Vin11GlobalConfigData,
    &AmbaCSL_Vin12GlobalConfigData,
    &AmbaCSL_Vin13GlobalConfigData,
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
    &AmbaCSL_Vin5Info,
    &AmbaCSL_Vin6Info,
    &AmbaCSL_Vin7Info,
    &AmbaCSL_Vin8Info,
    &AmbaCSL_Vin9Info,
    &AmbaCSL_Vin10Info,
    &AmbaCSL_Vin11Info,
    &AmbaCSL_Vin12Info,
    &AmbaCSL_Vin13Info,
};

void AmbaCSL_VinSetMipiCtrl0(UINT32 d)
{
    UINT32 *pData;
    const volatile AMBA_DPHY_CTRL0_REG_s *pDphyCtrl0;

    pDphyCtrl0 = &pAmbaMIPI_Reg->Dphy0Ctrl0;
    AmbaMisra_TypeCast(&pData, &pDphyCtrl0);

    *((volatile UINT32 *) (pData)) = d;
}

UINT32 AmbaCSL_VinGetMipiCtrl0(void)
{
    const UINT32 *pData;
    const volatile AMBA_DPHY_CTRL0_REG_s *pDphyCtrl0;

    pDphyCtrl0 = &pAmbaMIPI_Reg->Dphy0Ctrl0;
    AmbaMisra_TypeCast(&pData, &pDphyCtrl0);

    return *((const volatile UINT32 *) (pData));
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
    UINT32 SubGrpEnable = 0U;
    UINT32 DataLaneEnableVal = DataLaneEnable;

    switch (PadMode) {
    case AMBA_VIN_PAD_MODE_SLVS:
        if ((DataLaneEnableVal & 0x0fU) != 0U) {
            SubGrpEnable |= 0x1U;
            AmbaCSL_VinSetLvdsBitMode0(0x0U);
        }
        if ((DataLaneEnableVal & 0xf0U) != 0U) {
            SubGrpEnable |= 0x2U;
            AmbaCSL_VinSetLvdsBitMode1(0x0U);
        }

        AmbaCSL_VinSetMipiMode(AmbaCSL_VinGetMipiMode() & ~SubGrpEnable);

        DataLaneEnableVal = DataLaneEnableVal & 0xffU;
        AmbaCSL_VinSetLvCmosMode(AmbaCSL_VinGetLvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetLvCmosModeSpclk(AmbaCSL_VinGetLvCmosModeSpclk() & ~SubGrpEnable);
        break;
    case AMBA_VIN_PAD_MODE_MIPI:
        if ((DataLaneEnableVal & 0xffU) == 0xffU) {
            AmbaCSL_VinSetLvdsBitMode0(0x0U);
            AmbaCSL_VinSetLvdsBitMode1(0x0U);
            AmbaCSL_VinSetMipiMode(0x3U);

            AmbaCSL_VinSetLvCmosMode(0x0U);
            AmbaCSL_VinSetLvCmosModeSpclk(0x0U);
        } else {
            AmbaCSL_VinSetLvdsBitMode0(0x0U);
            AmbaCSL_VinSetMipiMode(AmbaCSL_VinGetMipiMode() | 0x1U);

            DataLaneEnableVal = DataLaneEnableVal & 0x0fU;
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
 *  AmbaCSL_VinSetVin11LvdsPadMode - Configure LVDS pad mode
 *  @param[in] PadMode Indicate pad mode
 *  @param[in] DataLaneEnable Enabled data pins defined in bit-wise
 *  @return error code
 */
UINT32 AmbaCSL_VinSetVin11LvdsPadMode(UINT32 PadMode, UINT32 DataLaneEnable)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 DataLaneEnableVal = DataLaneEnable;

    DataLaneEnableVal = DataLaneEnableVal & 0xf0U;

    switch (PadMode) {
    case AMBA_VIN_PAD_MODE_SLVS:
        if ((DataLaneEnableVal & 0xf0U) != 0U) {
            AmbaCSL_VinSetLvdsBitMode1(0x1U);
            AmbaCSL_VinSetMipiMode(AmbaCSL_VinGetMipiMode() & ~0x2U);
        }

        AmbaCSL_VinSetLvCmosMode(AmbaCSL_VinGetLvCmosMode() & ~DataLaneEnableVal);
        AmbaCSL_VinSetLvCmosModeSpclk(AmbaCSL_VinGetLvCmosModeSpclk() & ~0x2U);
        break;
    case AMBA_VIN_PAD_MODE_MIPI:
        if ((DataLaneEnableVal & 0xf0U) != 0U) {
            AmbaCSL_VinSetLvdsBitMode1(0x1U);
            AmbaCSL_VinSetMipiMode(AmbaCSL_VinGetMipiMode() | 0x2U);
        }

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

    if ((VinID == AMBA_VIN_CHANNEL0) || (VinID == AMBA_VIN_CHANNEL1) ||
        (VinID == AMBA_VIN_CHANNEL2) || (VinID == AMBA_VIN_CHANNEL3)) {
        *pLaneShift = 0U;
    } else if ((VinID == AMBA_VIN_CHANNEL4) || (VinID == AMBA_VIN_CHANNEL5) ||
               (VinID == AMBA_VIN_CHANNEL6) || (VinID == AMBA_VIN_CHANNEL7)) {
        *pLaneShift = 4U;
    } else if ((VinID == AMBA_VIN_CHANNEL8) || (VinID == AMBA_VIN_CHANNEL9) ||
               (VinID == AMBA_VIN_CHANNEL10)) {
        *pLaneShift = 0U;
    } else if ((VinID == AMBA_VIN_CHANNEL11) || (VinID == AMBA_VIN_CHANNEL12) ||
               (VinID == AMBA_VIN_CHANNEL13)) {
        *pLaneShift = 4U;
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

static void DebugBusConfigVinGlobal(UINT32 VinID)
{
    UINT32 BackupAddrExtensionData;
    UINT16 GlobalConfigData[AMBA_CSL_VIN_GLOBAL_CONFIG_SIZE];
    ULONG BaseAddr, Addr, i;
    volatile UINT32 *pAddr;
    const AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s *pVinGlobalCfg;

    pVinGlobalCfg = pGlobalConfigData[VinID];

    if (AmbaWrap_memcpy(GlobalConfigData, pVinGlobalCfg, sizeof(GlobalConfigData)) == ERR_NONE) {

        BackupAddrExtensionData = AmbaCSL_IdspGetAddrExtensionData();
        AmbaCSL_IdspSetAddrExtension(VinSection[VinID], 0U);
        AmbaDelayCycles(0x3fffU);

        AmbaMisra_TypeCast(&BaseAddr, &pAmbaIDSP_VinGlobalReg);

        for (i = 0U; i < AMBA_CSL_VIN_GLOBAL_CONFIG_SIZE; i++) {
            Addr = BaseAddr + (i * 4U);
            AmbaMisra_TypeCast(&pAddr, &Addr);
            WriteWord(pAddr, GlobalConfigData[i]);
        }

        AmbaCSL_IdspSetAddrExtensionData(BackupAddrExtensionData);
        AmbaDelayCycles(0x3fffU);
    }
}

static UINT32 DebugBusConfigVinMain(UINT32 VinID, const AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 BackupAddrExtensionData;
    UINT16 MainConfigData[AMBA_CSL_VIN_MAIN_CONFIG_SIZE];
    ULONG BaseAddr, Addr, i;
    volatile UINT32 *pAddr;

    if (AmbaWrap_memcpy(MainConfigData, pVinCfg, sizeof(MainConfigData)) != ERR_NONE) {
        RetVal = VIN_ERR_UNEXPECTED;
    }

    BackupAddrExtensionData = AmbaCSL_IdspGetAddrExtensionData();
    AmbaCSL_IdspResetVinPipelineSection(VinID);
    AmbaCSL_IdspSetAddrExtension(VinSection[VinID], 0U);
    AmbaDelayCycles(0x3fffU);

    MainConfigData[2] &= ~(UINT16)0x4000U; /* do not disable vin after vsync */
    MainConfigData[8] |= 0x2U;     /* DBG_NON_BLOCKING=1 */

    AmbaMisra_TypeCast(&BaseAddr, &pAmbaIDSP_VinMainReg);

    for (i = 0U; i <= 46U; i++) {
        Addr = BaseAddr + (i * 4U);
        AmbaMisra_TypeCast(&pAddr, &Addr);
        WriteWord(pAddr, MainConfigData[i]);
    }
    MainConfigData[128] = 1U; /* manually activate config (for debug only) */
    Addr = BaseAddr + (128U * 4U);
    AmbaMisra_TypeCast(&pAddr, &Addr);
    WriteWord(pAddr, MainConfigData[128]);

    AmbaCSL_IdspSetAddrExtensionData(BackupAddrExtensionData);
    AmbaDelayCycles(0x3fffU);

    return RetVal;
}

static UINT32 DebugBusConfigVinMSync(UINT32 MSyncID)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 BackupAddrExtensionData;
    UINT16 MSyncConfigData[AMBA_CSL_VIN_MASTER_SYNC_CONFIG_SIZE] = {0};
    ULONG BaseAddr, Addr, i;
    volatile UINT32 *pAddr;

    BackupAddrExtensionData = AmbaCSL_IdspGetAddrExtensionData();
    if (MSyncID == AMBA_VIN_MSYNC0) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN0, 0U);
        AmbaDelayCycles(0x3fffU);
        if (AmbaWrap_memcpy(MSyncConfigData, &AmbaCSL_VinMSync0ConfigData, sizeof(MSyncConfigData)) != ERR_NONE) {
            RetVal = VIN_ERR_UNEXPECTED;
        }
    } else if (MSyncID == AMBA_VIN_MSYNC1) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN4, 0U);
        AmbaDelayCycles(0x3fffU);
        if (AmbaWrap_memcpy(MSyncConfigData, &AmbaCSL_VinMSync1ConfigData, sizeof(MSyncConfigData)) != ERR_NONE) {
            RetVal = VIN_ERR_UNEXPECTED;
        }
    } else {
        if (AmbaWrap_memcpy(MSyncConfigData, &AmbaCSL_VinMSync0ConfigData, sizeof(MSyncConfigData)) != ERR_NONE) {
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
        }
    }

    AmbaCSL_IdspSetAddrExtensionData(BackupAddrExtensionData);
    AmbaDelayCycles(0x3fffU);

    return RetVal;
}

static UINT32 VinSetDcphyCsdParam(UINT32 VinID, const UINT32 CsdGrpNum, const AMBA_VIN_DCPHY_PARAM_s* pDcphyParam)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_DCPHY_CSD_REG_s* pCsdReg;

    if ((VinID == AMBA_VIN_CHANNEL0) || (VinID == AMBA_VIN_CHANNEL1) ||
        (VinID == AMBA_VIN_CHANNEL2) || (VinID == AMBA_VIN_CHANNEL3)) {
        pCsdReg = &pAmbaDCPHY_Regs[0]->DcphyCsd[CsdGrpNum];
    } else if ((VinID == AMBA_VIN_CHANNEL4) || (VinID == AMBA_VIN_CHANNEL5) ||
               (VinID == AMBA_VIN_CHANNEL6) || (VinID == AMBA_VIN_CHANNEL7)) {
        pCsdReg = &pAmbaDCPHY_Regs[1]->DcphyCsd[CsdGrpNum];
    } else {
        RetVal = VIN_ERR_ARG;
    }

    if (RetVal == VIN_ERR_NONE) {
        pCsdReg->GnrCon0.Enable = 0U;
        pCsdReg->GnrCon0.PhyReady = 0U;

        pCsdReg->GnrCon1 = 0x1450;                       /* 0x04 */
        pCsdReg->AnaCon0.HsRxCntRout    = (UINT8)pDcphyParam->HsRxCntRout;                        /* 0x08 */
        pCsdReg->AnaCon0.SelXiClk       = 1U;
        pCsdReg->AnaCon0.RxNcCoef       = (UINT8)pDcphyParam->RxNcCoef;

        pCsdReg->AnaCon1.HsRxCapCnt     = (UINT8)pDcphyParam->TimingParam.HsRxCapCnt;             /* 0x0C */
        pCsdReg->AnaCon1.HsRxRselCnt    = (UINT8)pDcphyParam->TimingParam.HsRxRselCnt;
        pCsdReg->AnaCon1.HsRxFlatband   = 1;
        pCsdReg->AnaCon1.HsRxBiasCon    = (UINT8)pDcphyParam->HsRxBiasConCsd;

        pCsdReg->AnaCon2.RxTermSw           = 5U;  /*pDcphyParam->RxTermSwCsd;*/ /* 0x10 */
        pCsdReg->AnaCon2.HsUntermEn         = 0;
        pCsdReg->AnaCon2.SkewCalExtForce    = 0;
        pCsdReg->AnaCon2.RxDdrClkHsSel      = 0;
        pCsdReg->AnaCon2.SkewDlySel         = (UINT8)pDcphyParam->TimingParam.SkewDlySel;
        pCsdReg->AnaCon2.HsmodeManSel       = 0;
        pCsdReg->AnaCon2.HsTermEnMan        = 0;
        pCsdReg->AnaCon2.UdlySpdcnt         = 0;
        pCsdReg->AnaCon2.SelFdly            = 0;
        pCsdReg->AnaCon2.ResetnCfgSel       = 0;

        pCsdReg->AnaCon3 = 0x8600;                       /* 0x14 */
        pCsdReg->AnaCon4 = 0x4000;                       /* 0x18 */

        pCsdReg->AnaCon5.BsCntSg    = 0;                /* 0x1C */
        pCsdReg->AnaCon5.PbiasCntSg = 0;
        pCsdReg->AnaCon5.EnDesk     = 0;
        pCsdReg->AnaCon5.CksymPol   = 0;
        pCsdReg->AnaCon5.SelDly     = (UINT8)pDcphyParam->SelDly;
        pCsdReg->AnaCon5.SelCrcCode = 0;

        pCsdReg->AnaCon6.HsmodeManSelCphy       = 0;       /* 0x20 */
        pCsdReg->AnaCon6.DataDeserEnCphyMan     = 0;
        pCsdReg->AnaCon6.ClkHsRxEmableCphyMan   = 0;
        pCsdReg->AnaCon6.ExtDlMuxEn             = (UINT8)pDcphyParam->ExtDlMuxEn;
        pCsdReg->AnaCon6.UiMaskCtrl             = (UINT8)pDcphyParam->UiMaskCtrl;
        pCsdReg->AnaCon6.DlDataComp             = 0;
        pCsdReg->AnaCon6.DlCompCtrl             = (UINT8)pDcphyParam->DlCompCtrl;
        pCsdReg->AnaCon6.AfcMuxEn               = 0;
        pCsdReg->AnaCon6.SelEespol              = 0;
        pCsdReg->AnaCon6.HsRxAmpDvCnt           = 0;
        pCsdReg->AnaCon6.HsUntermEnCphy         = 0;
        pCsdReg->AnaCon6.PhyChkCnt              = 0;

        pCsdReg->AnaCon7 = 0x0; /*0x40U;*/                          /* 0x24 */

        pCsdReg->TimeCon0.ClkSettleSel  = (UINT8)pDcphyParam->TimingParam.ClkSettleSel; /* 0x30*/
        pCsdReg->TimeCon0.THsSettle     = (UINT8)pDcphyParam->TimingParam.THsSettle;


        pCsdReg->TimeCon1.TErrSotSync   = (UINT8)pDcphyParam->TErrSotSync;   /* 0x34 */

        pCsdReg->DataCon0   = 0U;                            /* 0x38 */

        pCsdReg->DeskewCon0.SkewCalDesSel = 0U;            /* 0x40 */
        pCsdReg->DeskewCon0.SkewCalEn = (UINT8)pDcphyParam->TimingParam.SkewCalEn;  /* to be updated */

        pCsdReg->DeskewCon1 = 0U;                          /* 0x44 */
        pCsdReg->DeskewCon2 = 0U;                          /* 0x48 */
        pCsdReg->DeskewCon3 = 0U;                          /* 0x4c */
        pCsdReg->DeskewCon4 = 0x81A; /*0U;*/                 /* 0x50 */

        pCsdReg->CrcCon0    = 0U;                                           /* 0x60 */
        pCsdReg->CrcCon1.CrcPostGatingCnt = (UINT8)pDcphyParam->CrcPostGatingCnt;  /* 0x64 */
        pCsdReg->CrcCon1.CrcAvgSel = (UINT8)pDcphyParam->CrcAvgSel;
        pCsdReg->CrcCon2.CrcCodeTol = (UINT8)pDcphyParam->CrcCodeTol;              /* 0x68 */
        pCsdReg->CrcCon2.CrcCodeTune = (UINT8)pDcphyParam->CrcCodeTune;

        //pCsdReg->CrcCon1    = 0x1501U;
        //pCsdReg->CrcCon2    = 0x31U;
        /*pCsdReg->CrcCon1    = 0x0U;*/                        /* 0x64 */
        /*pCsdReg->CrcCon2    = 0x0U;*/                        /* 0x68 */
        pCsdReg->TestCon0   = 0U;                            /* 0x70 */
        pCsdReg->TestCon1   = 0U;                            /* 0x74 */
        pCsdReg->TestCon2   = 0U;                            /* 0x78 */
        pCsdReg->TestCon3   = 0U;                            /* 0x7c */
        pCsdReg->TestCon4   = 0U;                            /* 0x80 */
        pCsdReg->TestCon5   = 0U;                            /* 0x84 */
        pCsdReg->TestCon6   = 0U;                            /* 0x88 */
    }

    return RetVal;
}

static UINT32 VinSetDcphySdParam(UINT32 VinID, const AMBA_VIN_DCPHY_PARAM_s* pDcphyParam)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_DCPHY_REG_s* pDcphyReg;

    if ((VinID == AMBA_VIN_CHANNEL0) || (VinID == AMBA_VIN_CHANNEL1) ||
        (VinID == AMBA_VIN_CHANNEL2) || (VinID == AMBA_VIN_CHANNEL3)) {
        pDcphyReg = pAmbaDCPHY_Regs[0];
    } else if ((VinID == AMBA_VIN_CHANNEL4) || (VinID == AMBA_VIN_CHANNEL5) ||
               (VinID == AMBA_VIN_CHANNEL6) || (VinID == AMBA_VIN_CHANNEL7)) {
        pDcphyReg = pAmbaDCPHY_Regs[1];
    } else {
        RetVal = VIN_ERR_ARG;
    }

    if (RetVal == VIN_ERR_NONE) {
        pDcphyReg->DcphySdGnrCon0.Enable = 0U;
        pDcphyReg->DcphySdGnrCon0.PhyReady = 0U;

        pDcphyReg->DcphySdGnrCon1   = 0x1450;           /* 0xF04 */
        pDcphyReg->DcphySdAnaCon0.HsRxCntRout   = (UINT8)pDcphyParam->HsRxCntRout;               /* 0xF08 */

        pDcphyReg->DcphySdAnaCon1.HsRxCapCnt    = (UINT8)pDcphyParam->TimingParam.HsRxCapCnt;    /* 0xF0C */
        pDcphyReg->DcphySdAnaCon1.HsRxRselCnt   = (UINT8)pDcphyParam->TimingParam.HsRxRselCnt;
        pDcphyReg->DcphySdAnaCon1.HsRxFlatband  = 1;
        pDcphyReg->DcphySdAnaCon1.HsRxBiasCon   = (UINT8)pDcphyParam->HsRxBiasConSc;

        pDcphyReg->DcphySdAnaCon2.RxTermSw          = 5;  /*pDcphyParam->RxTermSwCsd;*/ /* 0xF10 */
        pDcphyReg->DcphySdAnaCon2.HsUntermEn        = 0;
        pDcphyReg->DcphySdAnaCon2.SkewCalExtForce   = 0;
        pDcphyReg->DcphySdAnaCon2.RxDdrClkHsSel     = 0;
        pDcphyReg->DcphySdAnaCon2.SkewDlySel        = (UINT8)pDcphyParam->TimingParam.SkewDlySel;
        pDcphyReg->DcphySdAnaCon2.HsmodeManSel      = 0;
        pDcphyReg->DcphySdAnaCon2.HsTermEnMan       = 0;
        pDcphyReg->DcphySdAnaCon2.UdlySpdcnt        = 0;
        pDcphyReg->DcphySdAnaCon2.SelFdly           = 0;
        pDcphyReg->DcphySdAnaCon2.ResetnCfgSel      = 0;

        pDcphyReg->DcphySdAnaCon3   = 0x8600;    /* 0xF14 */
        pDcphyReg->DcphySdAnaCon4   = 0x4000;   /* 0xF18 */

        pDcphyReg->DcphySdTimeCon0.THsSettle    = (UINT8)pDcphyParam->TimingParam.THsSettle;       /* 0xF30 */
        pDcphyReg->DcphySdTimeCon0.ClkSettleSel = (UINT8)pDcphyParam->TimingParam.ClkSettleSel;

        pDcphyReg->DcphySdTimeCon1.TErrSotSync  = (UINT8)pDcphyParam->TErrSotSync;                 /* 0xF34 */

        pDcphyReg->DcphySdDataCon0  = 0;        /* 0xF38 */

        pDcphyReg->DcphySdDeskewCon0.SkewCalEn      = (UINT8)pDcphyParam->TimingParam.SkewCalEn;   /* 0xF40 */
        pDcphyReg->DcphySdDeskewCon0.SkewCalDesSel  = 0;

        pDcphyReg->DcphySdDeskewCon1    = 0;    /* 0xF44 */
        pDcphyReg->DcphySdDeskewCon2    = 0;    /* 0xF48 */
        pDcphyReg->DcphySdDeskewCon3    = 0;    /* 0xF4C */

        pDcphyReg->DcphySdDeskewCon4    = 0x81a; /*0;*/    /* 0xF50 */
        pDcphyReg->DcphySdTestCon0      = 0;    /* 0xF70 */

        pDcphyReg->DcphySdTestCon1      = 0;    /* 0xF74 */

        pDcphyReg->DcphySdTestCon2      = 0;    /* 0xF7C */
        pDcphyReg->DcphySdTestCon3      = 0;    /* 0xF84 */
        pDcphyReg->DcphySdTestCon4      = 0;    /* 0xF88 */

    }

    return RetVal;
}

UINT32 AmbaCSL_VinSetDcphyParam(UINT32 VinID, const AMBA_VIN_DCPHY_PARAM_s* pDcphyParam, UINT32 EnabledPin)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_DCPHY_REG_s* pDcphyReg;
    UINT32 DataLaneEnable = 0U;

    if ((VinID == AMBA_VIN_CHANNEL0) || (VinID == AMBA_VIN_CHANNEL1) ||
        (VinID == AMBA_VIN_CHANNEL2) || (VinID == AMBA_VIN_CHANNEL3)) {
        pDcphyReg = pAmbaDCPHY_Regs[0];
        DataLaneEnable = EnabledPin;
    } else if ((VinID == AMBA_VIN_CHANNEL4) || (VinID == AMBA_VIN_CHANNEL5) ||
               (VinID == AMBA_VIN_CHANNEL6) || (VinID == AMBA_VIN_CHANNEL7)) {
        pDcphyReg = pAmbaDCPHY_Regs[1];
        DataLaneEnable = EnabledPin >> 4U;
    } else {
        RetVal = VIN_ERR_ARG;
    }

    if (RetVal == VIN_ERR_NONE) {
        /* BIAS with fixed value */
        /*        pDcphyReg->DcphyBiasCon0 = 0x10U;*/               /* 0x000 */
        /* Lynn: increase bias current */
        pDcphyReg->DcphyBiasCon0 = 0x0U;                /* 0x000 */
        pDcphyReg->DcphyBiasCon1 = 0x110U;              /* 0x004 */
        pDcphyReg->DcphyBiasCon2 = 0x3123U;             /* 0x008 */
        pDcphyReg->DcphyBiasCon3 = 0x0U;                /* 0x00c */
        pDcphyReg->DcphyBiasCon4.IloopBackEn    = 0;    /* 0x010 */
        pDcphyReg->DcphyBiasCon4.IqResetnPolCon = 0;
        pDcphyReg->DcphyBiasCon4.ILadderEn      = 0;
        pDcphyReg->DcphyBiasCon4.IAtbSelBg      = 0;
        pDcphyReg->DcphyBiasCon4.IMuxSel        = (UINT8)pDcphyParam->IMuxSel;
        pDcphyReg->DcphyBiasCon4.IMuxSelLp      = 0;
        pDcphyReg->DcphyBiasCon4.IClkSel        = 0;
        pDcphyReg->DcphyBiasCon4.IClkEn         = 0x1U;
        pDcphyReg->DcphyBiasCon4.IBypassEn      = 0;
        pDcphyReg->DcphyBiasCon4.QuadCntMan     = 0;

        if (pDcphyParam->ClkLaneEnable == 1U) {    /*  DPHY case */
            /* Slave Clock Lane */
            pDcphyReg->DcphyScGnrCon0.PhyReady = 0;   /* 0xB00 */
            pDcphyReg->DcphyScGnrCon0.Enable   = 0;

            pDcphyReg->DcphyScGnrCon1 = 0x1450U;            /* 0xB04 */
            pDcphyReg->DcphyScAnaCon0.HsRxCntRout =     (UINT8)pDcphyParam->HsRxCntRout;            /* 0xB08 */
            pDcphyReg->DcphyScAnaCon0.SelXiClk =        1U;

            pDcphyReg->DcphyScAnaCon1.HsRxCapCnt =      (UINT8)pDcphyParam->TimingParam.HsRxCapCnt; /* 0xB0C */
            pDcphyReg->DcphyScAnaCon1.HsRxRselCnt =     (UINT8)pDcphyParam->TimingParam.HsRxRselCnt;
            pDcphyReg->DcphyScAnaCon1.HsRxFlatband =    1U;
            pDcphyReg->DcphyScAnaCon1.HsRxBiasCon =     (UINT8)pDcphyParam->HsRxBiasConSc;

            pDcphyReg->DcphyScAnaCon2.RxTermSw =            5;  /*pDcphyParam->RxTermSwSc;*/    /* 0xB10 */
            pDcphyReg->DcphyScAnaCon2.HsUntermEn =          0;
            pDcphyReg->DcphyScAnaCon2.SkewcalClkSel =       0;
            pDcphyReg->DcphyScAnaCon2.ClkDiv1234Mc =        (UINT8)pDcphyParam->TimingParam.ClkDiv1234Mc;
            pDcphyReg->DcphyScAnaCon2.HsmodeManSel =        0;
            pDcphyReg->DcphyScAnaCon2.ClkTermEnMan =        0;
            pDcphyReg->DcphyScAnaCon2.ClkHsRxEnableMan =    0;

            pDcphyReg->DcphyScAnaCon3 =     0x8600U;        /* 0xB14 */
            pDcphyReg->DcphyScAnaCon4 =     0x4000U;        /* 0xB18 */
            pDcphyReg->DcphyScAnaCon5 =     0U;             /* 0xB1C */
            pDcphyReg->DcphyScTimeCon0 =    0x301U;         /* 0xB30 */
            pDcphyReg->DcphyScDataCon0 =    0;              /* 0xB40 */
            pDcphyReg->DcphyScTestCon0 =    0;              /* 0xB70 */
            pDcphyReg->DcphyScTestCon1 =    0;              /* 0xB74 */
        }

        /* Slave Data0 Lane */
        (void) VinSetDcphyCsdParam(VinID, 0U, pDcphyParam); /* 0xC04-0xC88 */
        /* Slave Data1 Lane */
        (void) VinSetDcphyCsdParam(VinID, 1U, pDcphyParam); /* 0xD04-0xD88 */
        /* Slave Data2 Lane */
        (void) VinSetDcphyCsdParam(VinID, 2U, pDcphyParam); /* 0xE04-0xE88 */

        if ((DataLaneEnable & 0x8U) != 0U) {
            /* Slave Data3 Lane*/
            (void) VinSetDcphySdParam(VinID, pDcphyParam);  /* 0xF04-0xF88 */
        }

    }

    return RetVal;
}

UINT32 AmbaCSL_VinDcphyEnable(UINT32 VinID, UINT32 EnabledPin, UINT32 ClkLaneEnable)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_DCPHY_REG_s* pDcphyReg;
    UINT32 i;
    AmbaMisra_TouchUnused(&EnabledPin);

    if ((VinID == AMBA_VIN_CHANNEL0) || (VinID == AMBA_VIN_CHANNEL1) ||
        (VinID == AMBA_VIN_CHANNEL2) || (VinID == AMBA_VIN_CHANNEL3)) {
        pDcphyReg = pAmbaDCPHY_Regs[0];
    } else if ((VinID == AMBA_VIN_CHANNEL4) || (VinID == AMBA_VIN_CHANNEL5) ||
               (VinID == AMBA_VIN_CHANNEL6) || (VinID == AMBA_VIN_CHANNEL7)) {
        pDcphyReg = pAmbaDCPHY_Regs[1];
    } else {
        RetVal = VIN_ERR_ARG;
    }

    if (RetVal == VIN_ERR_NONE) {
        /* clock lane */
        pDcphyReg->DcphyScGnrCon0.Enable = (UINT8)ClkLaneEnable;

        //Sherry 20211014: lane32 VIN NG when not enable GnrCon0.Enable on lane01
        /* lane 0~2 */
        for (i = 0U; i < 3U; i ++) {
            pDcphyReg->DcphyCsd[i].GnrCon0.Enable = 1;
        }

        /* lane 3 */
        pDcphyReg->DcphySdGnrCon0.Enable = 1;

    }

    return RetVal;
}

static UINT32 Readl(ULONG Address)
{
    volatile const UINT32 *ptr;

    AmbaMisra_TypeCast(&ptr, &Address);

    return *ptr;
}

static void Writel(ULONG Address, UINT32 Value)
{
    volatile UINT32 *ptr;

    AmbaMisra_TypeCast(&ptr, &Address);
    *ptr = Value;
}

UINT32 AmbaCSL_VinSetSlvsEcPhyParam(UINT32 EnabledPin, UINT64 DataRate, UINT32 Stage)
{
    UINT32 RetVal = VIN_ERR_NONE;

    (void)EnabledPin;

    Writel ((ULONG)0x20ed1e8000U, 0x1000U);
    AmbaDelayCycles(0x3fffU);

    if (DataRate <= 2500000000ULL) { // 20210810 phy setting
        if (Stage == 0U) {
            ////==========
            ////Config PHY
            ////==========
            ////==========
            ////Config SerDes
            ////==========
            Writel ((ULONG)0x20ed200474, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed200480, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed20048c, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed200498, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2004a4, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2004b0, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2004bc, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2004c8, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2004d4, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2004e0, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2004ec, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2004f8, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2000f0, 0x00070585U);////ctle_func_en_in = 3
            Writel ((ULONG)0x20ed20012c, 0x00070585U);////ctle_func_en_in = 3
            Writel ((ULONG)0x20ed200168, 0x00070585U);////ctle_func_en_in = 3
            Writel ((ULONG)0x20ed2001a4, 0x00070585U);////ctle_func_en_in = 3
            Writel ((ULONG)0x20ed2001e0, 0x00070585U);////ctle_func_en_in = 3
            Writel ((ULONG)0x20ed20021c, 0x00070585U);////ctle_func_en_in = 3
            Writel ((ULONG)0x20ed200258, 0x00070585U);////ctle_func_en_in = 3
            Writel ((ULONG)0x20ed200294, 0x00070585U);////ctle_func_en_in = 3
            Writel ((ULONG)0x20ed2002d0, 0x00070585U);////ctle_func_en_in = 3
            Writel ((ULONG)0x20ed20030c, 0x00070585U);////ctle_func_en_in = 3
            Writel ((ULONG)0x20ed200348, 0x00070585U);////ctle_func_en_in = 3
            Writel ((ULONG)0x20ed200384, 0x00070585U);////ctle_func_en_in = 3
            Writel ((ULONG)0x20ed2000f0, 0x00030585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed20012c, 0x00030585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed200168, 0x00030585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed2001a4, 0x00030585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed2001e0, 0x00030585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed20021c, 0x00030585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed200258, 0x00030585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed200294, 0x00030585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed2002d0, 0x00030585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed20030c, 0x00030585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed200348, 0x00030585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed200384, 0x00030585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed2000e4, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed200120, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed20015c, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed200198, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed2001d4, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed200210, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed20024c, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed200288, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed2002c4, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed200300, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed20033c, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed200378, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed2000f4, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed200130, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed20016c, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed2001a8, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed2001e4, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed200220, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed20025c, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed200298, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed2002d4, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed200310, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed20034c, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed200388, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed2000e8, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed200124, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed200160, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed20019c, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed2001d8, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed200214, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed200250, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed20028c, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed2002c8, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed200304, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed200340, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed20037c, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed200100, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed20013c, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed200178, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed2001b4, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed2001f0, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed20022c, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed200268, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed2002a4, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed2002e0, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed20031c, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed200358, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed200394, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed2000f8, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed200134, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed200170, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed2001ac, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed2001e8, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed200224, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed200260, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed20029c, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed2002d8, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed200314, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed200350, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed20038c, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed2000e4, 0x08001829U);////slvs_mode = 1
            Writel ((ULONG)0x20ed200120, 0x08001829U);////slvs_mode = 1
            Writel ((ULONG)0x20ed20015c, 0x08001829U);////slvs_mode = 1
            Writel ((ULONG)0x20ed200198, 0x08001829U);////slvs_mode = 1
            Writel ((ULONG)0x20ed2001d4, 0x08001829U);////slvs_mode = 1
            Writel ((ULONG)0x20ed200210, 0x08001829U);////slvs_mode = 1
            Writel ((ULONG)0x20ed20024c, 0x08001829U);////slvs_mode = 1
            Writel ((ULONG)0x20ed200288, 0x08001829U);////slvs_mode = 1
            Writel ((ULONG)0x20ed2002c4, 0x08001829U);////slvs_mode = 1
            Writel ((ULONG)0x20ed200300, 0x08001829U);////slvs_mode = 1
            Writel ((ULONG)0x20ed20033c, 0x08001829U);////slvs_mode = 1
            Writel ((ULONG)0x20ed200378, 0x08001829U);////slvs_mode = 1
            Writel ((ULONG)0x20ed200108, 0x20001f40U);//
            Writel ((ULONG)0x20ed200144, 0x20001f40U);//
            Writel ((ULONG)0x20ed200180, 0x20001f40U);//
            Writel ((ULONG)0x20ed2001bc, 0x20001f40U);//
            Writel ((ULONG)0x20ed2001f8, 0x20001f40U);//
            Writel ((ULONG)0x20ed200234, 0x20001f40U);//
            Writel ((ULONG)0x20ed200270, 0x20001f40U);//
            Writel ((ULONG)0x20ed2002ac, 0x20001f40U);//
            Writel ((ULONG)0x20ed2002e8, 0x20001f40U);//
            Writel ((ULONG)0x20ed200324, 0x20001f40U);//
            Writel ((ULONG)0x20ed200360, 0x20001f40U);//
            Writel ((ULONG)0x20ed20039c, 0x20001f40U);//
            Writel ((ULONG)0x20ed200108, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed200144, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed200180, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed2001bc, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed2001f8, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed200234, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed200270, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed2002ac, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed2002e8, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed200324, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed200360, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed20039c, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed2000f4, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed200130, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed20016c, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed2001a8, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed2001e4, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed200220, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed20025c, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed200298, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed2002d4, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed200310, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed20034c, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed200388, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed2000f4, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed200130, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed20016c, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed2001a8, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed2001e4, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed200220, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed20025c, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed200298, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed2002d4, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed200310, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed20034c, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed200388, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed2000f4, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed200130, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed20016c, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed2001a8, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed2001e4, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed200220, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed20025c, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed200298, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed2002d4, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed200310, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed20034c, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed200388, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed2000f4, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed200130, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed20016c, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed2001a8, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed2001e4, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed200220, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed20025c, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed200298, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed2002d4, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed200310, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed20034c, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed200388, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed2000f8, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed200134, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed200170, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed2001ac, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed2001e8, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed200224, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed200260, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed20029c, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed2002d8, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed200314, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed200350, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed20038c, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed2000f4, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed200130, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed20016c, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed2001a8, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed2001e4, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed200220, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed20025c, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed200298, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed2002d4, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed200310, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed20034c, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed200388, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed2000f4, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed200130, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed20016c, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed2001a8, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed2001e4, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed200220, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed20025c, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed200298, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed2002d4, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed200310, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed20034c, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed200388, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed2000f4, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed200130, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed20016c, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed2001a8, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed2001e4, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed200220, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed20025c, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed200298, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed2002d4, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed200310, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed20034c, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed200388, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed2000ec, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed200128, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed200164, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed2001a0, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed2001dc, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed200218, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed200254, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed200290, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed2002cc, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed200308, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed200344, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed200380, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed2000f0, 0x03030585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed20012c, 0x03030585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed200168, 0x03030585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed2001a4, 0x03030585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed2001e0, 0x03030585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed20021c, 0x03030585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed200258, 0x03030585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed200294, 0x03030585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed2002d0, 0x03030585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed20030c, 0x03030585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed200348, 0x03030585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed200384, 0x03030585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed2000ec, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed200128, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed200164, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed2001a0, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed2001dc, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed200218, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed200254, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed200290, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed2002cc, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed200308, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed200344, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed200380, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed2000ec, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed200128, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed200164, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed2001a0, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed2001dc, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed200218, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed200254, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed200290, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed2002cc, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed200308, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed200344, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed200380, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed2000e8, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed200124, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed200160, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed20019c, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed2001d8, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed200214, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed200250, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed20028c, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed2002c8, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed200304, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed200340, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed20037c, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed20010c, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed200148, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed200184, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed2001c0, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed2001fc, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed200238, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed200274, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed2002b0, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed2002ec, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed200328, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed200364, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed2003a0, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed2000ec, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed200128, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed200164, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed2001a0, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed2001dc, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed200218, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed200254, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed200290, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed2002cc, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed200308, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed200344, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed200380, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed2000e8, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed200124, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed200160, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed20019c, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed2001d8, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed200214, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed200250, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed20028c, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed2002c8, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed200304, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed200340, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed20037c, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed2000e8, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed200124, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed200160, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed20019c, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed2001d8, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed200214, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed200250, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed20028c, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed2002c8, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed200304, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed200340, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed20037c, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed2000e8, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed200124, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed200160, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed20019c, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed2001d8, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed200214, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed200250, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed20028c, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed2002c8, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed200304, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed200340, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed20037c, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed200110, 0x8804f000U);////set vga_rctrl_in
            Writel ((ULONG)0x20ed20014c, 0x8804f000U);////set vga_rctrl_in
            Writel ((ULONG)0x20ed200188, 0x8804f000U);////set vga_rctrl_in
            Writel ((ULONG)0x20ed2001c4, 0x8804f000U);////set vga_rctrl_in
            Writel ((ULONG)0x20ed200200, 0x8804f000U);////set vga_rctrl_in
            Writel ((ULONG)0x20ed20023c, 0x8804f000U);////set vga_rctrl_in
            Writel ((ULONG)0x20ed200278, 0x8804f000U);////set vga_rctrl_in
            Writel ((ULONG)0x20ed2002b4, 0x8804f000U);////set vga_rctrl_in
            Writel ((ULONG)0x20ed2002f0, 0x8804f000U);////set vga_rctrl_in
            Writel ((ULONG)0x20ed20032c, 0x8804f000U);////set vga_rctrl_in
            Writel ((ULONG)0x20ed200368, 0x8804f000U);////set vga_rctrl_in
            Writel ((ULONG)0x20ed2003a4, 0x8804f000U);////set vga_rctrl_in
            Writel ((ULONG)0x20ed2000ec, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed200128, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed200164, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed2001a0, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed2001dc, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed200218, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed200254, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed200290, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed2002cc, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed200308, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed200344, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed200380, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed2000ec, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200128, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200164, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed2001a0, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed2001dc, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200218, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200254, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200290, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed2002cc, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200308, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200344, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200380, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200114, 0x00000000U);////config vga rgain
            Writel ((ULONG)0x20ed200150, 0x00000000U);////config vga rgain
            Writel ((ULONG)0x20ed20018c, 0x00000000U);////config vga rgain
            Writel ((ULONG)0x20ed2001c8, 0x00000000U);////config vga rgain
            Writel ((ULONG)0x20ed200204, 0x00000000U);////config vga rgain
            Writel ((ULONG)0x20ed200240, 0x00000000U);////config vga rgain
            Writel ((ULONG)0x20ed20027c, 0x00000000U);////config vga rgain
            Writel ((ULONG)0x20ed2002b8, 0x00000000U);////config vga rgain
            Writel ((ULONG)0x20ed2002f4, 0x00000000U);////config vga rgain
            Writel ((ULONG)0x20ed200330, 0x00000000U);////config vga rgain
            Writel ((ULONG)0x20ed20036c, 0x00000000U);////config vga rgain
            Writel ((ULONG)0x20ed2003a8, 0x00000000U);////config vga rgain
            Writel ((ULONG)0x20ed2000ec, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed200128, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed200164, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed2001a0, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed2001dc, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed200218, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed200254, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed200290, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed2002cc, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed200308, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed200344, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed200380, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed2000ec, 0xc24121a8U);////sslms_vga_manual = 1
            Writel ((ULONG)0x20ed200128, 0xc24121a8U);////sslms_vga_manual = 1
            Writel ((ULONG)0x20ed200164, 0xc24121a8U);////sslms_vga_manual = 1
            Writel ((ULONG)0x20ed2001a0, 0xc24121a8U);////sslms_vga_manual = 1
            Writel ((ULONG)0x20ed2001dc, 0xc24121a8U);////sslms_vga_manual = 1
            Writel ((ULONG)0x20ed200218, 0xc24121a8U);////sslms_vga_manual = 1
            Writel ((ULONG)0x20ed200254, 0xc24121a8U);////sslms_vga_manual = 1
            Writel ((ULONG)0x20ed200290, 0xc24121a8U);////sslms_vga_manual = 1
            Writel ((ULONG)0x20ed2002cc, 0xc24121a8U);////sslms_vga_manual = 1
            Writel ((ULONG)0x20ed200308, 0xc24121a8U);////sslms_vga_manual = 1
            Writel ((ULONG)0x20ed200344, 0xc24121a8U);////sslms_vga_manual = 1
            Writel ((ULONG)0x20ed200380, 0xc24121a8U);////sslms_vga_manual = 1
            Writel ((ULONG)0x20ed2000ec, 0xc24121a8U);////disable vref_pause
            Writel ((ULONG)0x20ed200128, 0xc24121a8U);////disable vref_pause
            Writel ((ULONG)0x20ed200164, 0xc24121a8U);////disable vref_pause
            Writel ((ULONG)0x20ed2001a0, 0xc24121a8U);////disable vref_pause
            Writel ((ULONG)0x20ed2001dc, 0xc24121a8U);////disable vref_pause
            Writel ((ULONG)0x20ed200218, 0xc24121a8U);////disable vref_pause
            Writel ((ULONG)0x20ed200254, 0xc24121a8U);////disable vref_pause
            Writel ((ULONG)0x20ed200290, 0xc24121a8U);////disable vref_pause
            Writel ((ULONG)0x20ed2002cc, 0xc24121a8U);////disable vref_pause
            Writel ((ULONG)0x20ed200308, 0xc24121a8U);////disable vref_pause
            Writel ((ULONG)0x20ed200344, 0xc24121a8U);////disable vref_pause
            Writel ((ULONG)0x20ed200380, 0xc24121a8U);////disable vref_pause
            Writel ((ULONG)0x20ed200108, 0x20003f58U);////set vref_in
            Writel ((ULONG)0x20ed200144, 0x20003f58U);////set vref_in
            Writel ((ULONG)0x20ed200180, 0x20003f58U);////set vref_in
            Writel ((ULONG)0x20ed2001bc, 0x20003f58U);////set vref_in
            Writel ((ULONG)0x20ed2001f8, 0x20003f58U);////set vref_in
            Writel ((ULONG)0x20ed200234, 0x20003f58U);////set vref_in
            Writel ((ULONG)0x20ed200270, 0x20003f58U);////set vref_in
            Writel ((ULONG)0x20ed2002ac, 0x20003f58U);////set vref_in
            Writel ((ULONG)0x20ed2002e8, 0x20003f58U);////set vref_in
            Writel ((ULONG)0x20ed200324, 0x20003f58U);////set vref_in
            Writel ((ULONG)0x20ed200360, 0x20003f58U);////set vref_in
            Writel ((ULONG)0x20ed20039c, 0x20003f58U);////set vref_in
            Writel ((ULONG)0x20ed20010c, 0x04000000U);////set vref_gain
            Writel ((ULONG)0x20ed200148, 0x04000000U);////set vref_gain
            Writel ((ULONG)0x20ed200184, 0x04000000U);////set vref_gain
            Writel ((ULONG)0x20ed2001c0, 0x04000000U);////set vref_gain
            Writel ((ULONG)0x20ed2001fc, 0x04000000U);////set vref_gain
            Writel ((ULONG)0x20ed200238, 0x04000000U);////set vref_gain
            Writel ((ULONG)0x20ed200274, 0x04000000U);////set vref_gain
            Writel ((ULONG)0x20ed2002b0, 0x04000000U);////set vref_gain
            Writel ((ULONG)0x20ed2002ec, 0x04000000U);////set vref_gain
            Writel ((ULONG)0x20ed200328, 0x04000000U);////set vref_gain
            Writel ((ULONG)0x20ed200364, 0x04000000U);////set vref_gain
            Writel ((ULONG)0x20ed2003a0, 0x04000000U);////set vref_gain
            Writel ((ULONG)0x20ed2000ec, 0xc24121a8U);////enable ctle rpause
            Writel ((ULONG)0x20ed200128, 0xc24121a8U);////enable ctle rpause
            Writel ((ULONG)0x20ed200164, 0xc24121a8U);////enable ctle rpause
            Writel ((ULONG)0x20ed2001a0, 0xc24121a8U);////enable ctle rpause
            Writel ((ULONG)0x20ed2001dc, 0xc24121a8U);////enable ctle rpause
            Writel ((ULONG)0x20ed200218, 0xc24121a8U);////enable ctle rpause
            Writel ((ULONG)0x20ed200254, 0xc24121a8U);////enable ctle rpause
            Writel ((ULONG)0x20ed200290, 0xc24121a8U);////enable ctle rpause
            Writel ((ULONG)0x20ed2002cc, 0xc24121a8U);////enable ctle rpause
            Writel ((ULONG)0x20ed200308, 0xc24121a8U);////enable ctle rpause
            Writel ((ULONG)0x20ed200344, 0xc24121a8U);////enable ctle rpause
            Writel ((ULONG)0x20ed200380, 0xc24121a8U);////enable ctle rpause
            Writel ((ULONG)0x20ed2000ec, 0xc24121a8U);////enable ctle cpause
            Writel ((ULONG)0x20ed200128, 0xc24121a8U);////enable ctle cpause
            Writel ((ULONG)0x20ed200164, 0xc24121a8U);////enable ctle cpause
            Writel ((ULONG)0x20ed2001a0, 0xc24121a8U);////enable ctle cpause
            Writel ((ULONG)0x20ed2001dc, 0xc24121a8U);////enable ctle cpause
            Writel ((ULONG)0x20ed200218, 0xc24121a8U);////enable ctle cpause
            Writel ((ULONG)0x20ed200254, 0xc24121a8U);////enable ctle cpause
            Writel ((ULONG)0x20ed200290, 0xc24121a8U);////enable ctle cpause
            Writel ((ULONG)0x20ed2002cc, 0xc24121a8U);////enable ctle cpause
            Writel ((ULONG)0x20ed200308, 0xc24121a8U);////enable ctle cpause
            Writel ((ULONG)0x20ed200344, 0xc24121a8U);////enable ctle cpause
            Writel ((ULONG)0x20ed200380, 0xc24121a8U);////enable ctle cpause
            Writel ((ULONG)0x20ed200110, 0x8804f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed20014c, 0x8804f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed200188, 0x8804f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed2001c4, 0x8804f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed200200, 0x8804f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed20023c, 0x8804f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed200278, 0x8804f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed2002b4, 0x8804f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed2002f0, 0x8804f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed20032c, 0x8804f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed200368, 0x8804f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed2003a4, 0x8804f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed200110, 0x88047700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed20014c, 0x88047700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed200188, 0x88047700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed2001c4, 0x88047700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed200200, 0x88047700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed20023c, 0x88047700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed200278, 0x88047700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed2002b4, 0x88047700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed2002f0, 0x88047700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed20032c, 0x88047700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed200368, 0x88047700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed2003a4, 0x88047700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed200110, 0x88047700U);////set ctle_cctrl0_in
            Writel ((ULONG)0x20ed20014c, 0x88047700U);////set ctle_cctrl0_in
            Writel ((ULONG)0x20ed200188, 0x88047700U);////set ctle_cctrl0_in
            Writel ((ULONG)0x20ed2001c4, 0x88047700U);////set ctle_cctrl0_in
            Writel ((ULONG)0x20ed200200, 0x88047700U);////set ctle_cctrl0_in
            Writel ((ULONG)0x20ed20023c, 0x88047700U);////set ctle_cctrl0_in
            Writel ((ULONG)0x20ed200278, 0x88047700U);////set ctle_cctrl0_in
            Writel ((ULONG)0x20ed2002b4, 0x88047700U);////set ctle_cctrl0_in
            Writel ((ULONG)0x20ed2002f0, 0x88047700U);////set ctle_cctrl0_in
            Writel ((ULONG)0x20ed20032c, 0x88047700U);////set ctle_cctrl0_in
            Writel ((ULONG)0x20ed200368, 0x88047700U);////set ctle_cctrl0_in
            Writel ((ULONG)0x20ed2003a4, 0x88047700U);////set ctle_cctrl0_in
            Writel ((ULONG)0x20ed200110, 0x88047700U);////set ctle_cctrl1_in 0
            Writel ((ULONG)0x20ed20014c, 0x88047700U);////set ctle_cctrl1_in 0
            Writel ((ULONG)0x20ed200188, 0x88047700U);////set ctle_cctrl1_in 0
            Writel ((ULONG)0x20ed2001c4, 0x88047700U);////set ctle_cctrl1_in 0
            Writel ((ULONG)0x20ed200200, 0x88047700U);////set ctle_cctrl1_in 0
            Writel ((ULONG)0x20ed20023c, 0x88047700U);////set ctle_cctrl1_in 0
            Writel ((ULONG)0x20ed200278, 0x88047700U);////set ctle_cctrl1_in 0
            Writel ((ULONG)0x20ed2002b4, 0x88047700U);////set ctle_cctrl1_in 0
            Writel ((ULONG)0x20ed2002f0, 0x88047700U);////set ctle_cctrl1_in 0
            Writel ((ULONG)0x20ed20032c, 0x88047700U);////set ctle_cctrl1_in 0
            Writel ((ULONG)0x20ed200368, 0x88047700U);////set ctle_cctrl1_in 0
            Writel ((ULONG)0x20ed2003a4, 0x88047700U);////set ctle_cctrl1_in 0
            Writel ((ULONG)0x20ed2000ec, 0xc24121a8U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed200128, 0xc24121a8U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed200164, 0xc24121a8U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed2001a0, 0xc24121a8U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed2001dc, 0xc24121a8U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed200218, 0xc24121a8U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed200254, 0xc24121a8U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed200290, 0xc24121a8U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed2002cc, 0xc24121a8U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed200308, 0xc24121a8U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed200344, 0xc24121a8U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed200380, 0xc24121a8U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed2000ec, 0xc24121a0U);////disable dfe pause
            Writel ((ULONG)0x20ed200128, 0xc24121a0U);////disable dfe pause
            Writel ((ULONG)0x20ed200164, 0xc24121a0U);////disable dfe pause
            Writel ((ULONG)0x20ed2001a0, 0xc24121a0U);////disable dfe pause
            Writel ((ULONG)0x20ed2001dc, 0xc24121a0U);////disable dfe pause
            Writel ((ULONG)0x20ed200218, 0xc24121a0U);////disable dfe pause
            Writel ((ULONG)0x20ed200254, 0xc24121a0U);////disable dfe pause
            Writel ((ULONG)0x20ed200290, 0xc24121a0U);////disable dfe pause
            Writel ((ULONG)0x20ed2002cc, 0xc24121a0U);////disable dfe pause
            Writel ((ULONG)0x20ed200308, 0xc24121a0U);////disable dfe pause
            Writel ((ULONG)0x20ed200344, 0xc24121a0U);////disable dfe pause
            Writel ((ULONG)0x20ed200380, 0xc24121a0U);////disable dfe pause
            Writel ((ULONG)0x20ed200104, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed200140, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed20017c, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed2001b8, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed2001f4, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed200230, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed20026c, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed2002a8, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed2002e4, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed200320, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed20035c, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed200398, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed2000e4, 0x08001829U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed200120, 0x08001829U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed20015c, 0x08001829U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed200198, 0x08001829U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed2001d4, 0x08001829U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed200210, 0x08001829U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed20024c, 0x08001829U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed200288, 0x08001829U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed2002c4, 0x08001829U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed200300, 0x08001829U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed20033c, 0x08001829U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed200378, 0x08001829U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed200114, 0x0000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed200150, 0x0000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed20018c, 0x0000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed2001c8, 0x0000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed200204, 0x0000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed200240, 0x0000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed20027c, 0x0000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed2002b8, 0x0000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed2002f4, 0x0000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed200330, 0x0000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed20036c, 0x0000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed2003a8, 0x0000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed2000f4, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed200130, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed20016c, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed2001a8, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed2001e4, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed200220, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed20025c, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed200298, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed2002d4, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed200310, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed20034c, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed200388, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed2000f0, 0x03030585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed20012c, 0x03030585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed200168, 0x03030585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed2001a4, 0x03030585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed2001e0, 0x03030585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed20021c, 0x03030585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed200258, 0x03030585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed200294, 0x03030585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed2002d0, 0x03030585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed20030c, 0x03030585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed200348, 0x03030585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed200384, 0x03030585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed20010c, 0x0400001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed200148, 0x0400001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed200184, 0x0400001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed2001c0, 0x0400001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed2001fc, 0x0400001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed200238, 0x0400001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed200274, 0x0400001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed2002b0, 0x0400001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed2002ec, 0x0400001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed200328, 0x0400001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed200364, 0x0400001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed2003a0, 0x0400001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed200110, 0x88047700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed20014c, 0x88047700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed200188, 0x88047700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed2001c4, 0x88047700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed200200, 0x88047700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed20023c, 0x88047700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed200278, 0x88047700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed2002b4, 0x88047700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed2002f0, 0x88047700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed20032c, 0x88047700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed200368, 0x88047700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed2003a4, 0x88047700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed200110, 0x88047700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed20014c, 0x88047700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed200188, 0x88047700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed2001c4, 0x88047700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed200200, 0x88047700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed20023c, 0x88047700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed200278, 0x88047700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed2002b4, 0x88047700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed2002f0, 0x88047700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed20032c, 0x88047700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed200368, 0x88047700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed2003a4, 0x88047700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed2000f8, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed200134, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed200170, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed2001ac, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed2001e8, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed200224, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed200260, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed20029c, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed2002d8, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed200314, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed200350, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed20038c, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed2000f8, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed200134, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed200170, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed2001ac, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed2001e8, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed200224, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed200260, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed20029c, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed2002d8, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed200314, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed200350, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed20038c, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed2000e8, 0x050d5201U);//
            Writel ((ULONG)0x20ed200124, 0x050d5201U);//
            Writel ((ULONG)0x20ed200160, 0x050d5201U);//
            Writel ((ULONG)0x20ed20019c, 0x050d5201U);//
            Writel ((ULONG)0x20ed2001d8, 0x050d5201U);//
            Writel ((ULONG)0x20ed200214, 0x050d5201U);//
            Writel ((ULONG)0x20ed200250, 0x050d5201U);//
            Writel ((ULONG)0x20ed20028c, 0x050d5201U);//
            Writel ((ULONG)0x20ed2002c8, 0x050d5201U);//
            Writel ((ULONG)0x20ed200304, 0x050d5201U);//
            Writel ((ULONG)0x20ed200340, 0x050d5201U);//
            Writel ((ULONG)0x20ed20037c, 0x050d5201U);//
            Writel ((ULONG)0x20ed2000e8, 0x050d5201U);//
            Writel ((ULONG)0x20ed200124, 0x050d5201U);//
            Writel ((ULONG)0x20ed200160, 0x050d5201U);//
            Writel ((ULONG)0x20ed20019c, 0x050d5201U);//
            Writel ((ULONG)0x20ed2001d8, 0x050d5201U);//
            Writel ((ULONG)0x20ed200214, 0x050d5201U);//
            Writel ((ULONG)0x20ed200250, 0x050d5201U);//
            Writel ((ULONG)0x20ed20028c, 0x050d5201U);//
            Writel ((ULONG)0x20ed2002c8, 0x050d5201U);//
            Writel ((ULONG)0x20ed200304, 0x050d5201U);//
            Writel ((ULONG)0x20ed200340, 0x050d5201U);//
            Writel ((ULONG)0x20ed20037c, 0x050d5201U);//
            Writel ((ULONG)0x20ed2000e4, 0x08001829U);//
            Writel ((ULONG)0x20ed200120, 0x08001829U);//
            Writel ((ULONG)0x20ed20015c, 0x08001829U);//
            Writel ((ULONG)0x20ed200198, 0x08001829U);//
            Writel ((ULONG)0x20ed2001d4, 0x08001829U);//
            Writel ((ULONG)0x20ed200210, 0x08001829U);//
            Writel ((ULONG)0x20ed20024c, 0x08001829U);//
            Writel ((ULONG)0x20ed200288, 0x08001829U);//
            Writel ((ULONG)0x20ed2002c4, 0x08001829U);//
            Writel ((ULONG)0x20ed200300, 0x08001829U);//
            Writel ((ULONG)0x20ed20033c, 0x08001829U);//
            Writel ((ULONG)0x20ed200378, 0x08001829U);//
            Writel ((ULONG)0x20ed2000e4, 0x08001829U);//
            Writel ((ULONG)0x20ed200120, 0x08001829U);//
            Writel ((ULONG)0x20ed20015c, 0x08001829U);//
            Writel ((ULONG)0x20ed200198, 0x08001829U);//
            Writel ((ULONG)0x20ed2001d4, 0x08001829U);//
            Writel ((ULONG)0x20ed200210, 0x08001829U);//
            Writel ((ULONG)0x20ed20024c, 0x08001829U);//
            Writel ((ULONG)0x20ed200288, 0x08001829U);//
            Writel ((ULONG)0x20ed2002c4, 0x08001829U);//
            Writel ((ULONG)0x20ed200300, 0x08001829U);//
            Writel ((ULONG)0x20ed20033c, 0x08001829U);//
            Writel ((ULONG)0x20ed200378, 0x08001829U);//
            Writel ((ULONG)0x20ed2000e4, 0x08001829U);//
            Writel ((ULONG)0x20ed200120, 0x08001829U);//
            Writel ((ULONG)0x20ed20015c, 0x08001829U);//
            Writel ((ULONG)0x20ed200198, 0x08001829U);//
            Writel ((ULONG)0x20ed2001d4, 0x08001829U);//
            Writel ((ULONG)0x20ed200210, 0x08001829U);//
            Writel ((ULONG)0x20ed20024c, 0x08001829U);//
            Writel ((ULONG)0x20ed200288, 0x08001829U);//
            Writel ((ULONG)0x20ed2002c4, 0x08001829U);//
            Writel ((ULONG)0x20ed200300, 0x08001829U);//
            Writel ((ULONG)0x20ed20033c, 0x08001829U);//
            Writel ((ULONG)0x20ed200378, 0x08001829U);//
            Writel ((ULONG)0x20ed2000e4, 0x08001829U);//
            Writel ((ULONG)0x20ed200120, 0x08001829U);//
            Writel ((ULONG)0x20ed20015c, 0x08001829U);//
            Writel ((ULONG)0x20ed200198, 0x08001829U);//
            Writel ((ULONG)0x20ed2001d4, 0x08001829U);//
            Writel ((ULONG)0x20ed200210, 0x08001829U);//
            Writel ((ULONG)0x20ed20024c, 0x08001829U);//
            Writel ((ULONG)0x20ed200288, 0x08001829U);//
            Writel ((ULONG)0x20ed2002c4, 0x08001829U);//
            Writel ((ULONG)0x20ed200300, 0x08001829U);//
            Writel ((ULONG)0x20ed20033c, 0x08001829U);//
            Writel ((ULONG)0x20ed200378, 0x08001829U);//
            Writel ((ULONG)0x20ed2000e4, 0x08001829U);//
            Writel ((ULONG)0x20ed200120, 0x08001829U);//
            Writel ((ULONG)0x20ed20015c, 0x08001829U);//
            Writel ((ULONG)0x20ed200198, 0x08001829U);//
            Writel ((ULONG)0x20ed2001d4, 0x08001829U);//
            Writel ((ULONG)0x20ed200210, 0x08001829U);//
            Writel ((ULONG)0x20ed20024c, 0x08001829U);//
            Writel ((ULONG)0x20ed200288, 0x08001829U);//
            Writel ((ULONG)0x20ed2002c4, 0x08001829U);//
            Writel ((ULONG)0x20ed200300, 0x08001829U);//
            Writel ((ULONG)0x20ed20033c, 0x08001829U);//
            Writel ((ULONG)0x20ed200378, 0x08001829U);//

            Writel ((ULONG)0x20ed2000f0, 0x03030585U);////set datarate_sel to 1, 2304Mbps base
            Writel ((ULONG)0x20ed20012c, 0x03030585U);////set datarate_sel to 1, 2304Mbps base
            Writel ((ULONG)0x20ed200168, 0x03030585U);////set datarate_sel to 1, 2304Mbps base
            Writel ((ULONG)0x20ed2001a4, 0x03030585U);////set datarate_sel to 1, 2304Mbps base
            Writel ((ULONG)0x20ed2001e0, 0x03030585U);////set datarate_sel to 1, 2304Mbps base
            Writel ((ULONG)0x20ed20021c, 0x03030585U);////set datarate_sel to 1, 2304Mbps base
            Writel ((ULONG)0x20ed200258, 0x03030585U);////set datarate_sel to 1, 2304Mbps base
            Writel ((ULONG)0x20ed200294, 0x03030585U);////set datarate_sel to 1, 2304Mbps base
            Writel ((ULONG)0x20ed2002d0, 0x03030585U);////set datarate_sel to 1, 2304Mbps base
            Writel ((ULONG)0x20ed20030c, 0x03030585U);////set datarate_sel to 1, 2304Mbps base
            Writel ((ULONG)0x20ed200348, 0x03030585U);////set datarate_sel to 1, 2304Mbps base
            Writel ((ULONG)0x20ed200384, 0x03030585U);////set datarate_sel to 1, 2304Mbps base
            Writel ((ULONG)0x20ed2000e8, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed200124, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed200160, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed20019c, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed2001d8, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed200214, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed200250, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed20028c, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed2002c8, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed200304, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed200340, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed20037c, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed200474, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed200480, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed20048c, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed200498, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2004a4, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2004b0, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2004bc, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2004c8, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2004d4, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2004e0, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2004ec, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2004f8, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2000ec, 0xc24125a0U);////halt_dco
            Writel ((ULONG)0x20ed200128, 0xc24125a0U);////halt_dco
            Writel ((ULONG)0x20ed200164, 0xc24125a0U);////halt_dco
            Writel ((ULONG)0x20ed2001a0, 0xc24125a0U);////halt_dco
            Writel ((ULONG)0x20ed2001dc, 0xc24125a0U);////halt_dco
            Writel ((ULONG)0x20ed200218, 0xc24125a0U);////halt_dco
            Writel ((ULONG)0x20ed200254, 0xc24125a0U);////halt_dco
            Writel ((ULONG)0x20ed200290, 0xc24125a0U);////halt_dco
            Writel ((ULONG)0x20ed2002cc, 0xc24125a0U);////halt_dco
            Writel ((ULONG)0x20ed200308, 0xc24125a0U);////halt_dco
            Writel ((ULONG)0x20ed200344, 0xc24125a0U);////halt_dco
            Writel ((ULONG)0x20ed200380, 0xc24125a0U);////halt_dco
            Writel ((ULONG)0x20ed2000e8, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed200124, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed200160, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed20019c, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed2001d8, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed200214, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed200250, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed20028c, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed2002c8, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed200304, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed200340, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed20037c, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            ////# 10000
            Writel ((ULONG)0x20ed2000ec, 0xc24121a0U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed200128, 0xc24121a0U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed200164, 0xc24121a0U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed2001a0, 0xc24121a0U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed2001dc, 0xc24121a0U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed200218, 0xc24121a0U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed200254, 0xc24121a0U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed200290, 0xc24121a0U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed2002cc, 0xc24121a0U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed200308, 0xc24121a0U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed200344, 0xc24121a0U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed200380, 0xc24121a0U);////halt_dco_rx[30] set to 1'h0 release DCO

            /* Reset PHY */
            Writel ((ULONG)0x20ed1e0800U, 0x404dU);
        } else if (Stage == 1U) {
            Writel ((ULONG)0x20ed2000e4, 0x08001829U);////obsv_calib = 0
            Writel ((ULONG)0x20ed200120, 0x08001829U);////obsv_calib = 0
            Writel ((ULONG)0x20ed20015c, 0x08001829U);////obsv_calib = 0
            Writel ((ULONG)0x20ed200198, 0x08001829U);////obsv_calib = 0
            Writel ((ULONG)0x20ed2001d4, 0x08001829U);////obsv_calib = 0
            Writel ((ULONG)0x20ed200210, 0x08001829U);////obsv_calib = 0
            Writel ((ULONG)0x20ed20024c, 0x08001829U);////obsv_calib = 0
            Writel ((ULONG)0x20ed200288, 0x08001829U);////obsv_calib = 0
            Writel ((ULONG)0x20ed2002c4, 0x08001829U);////obsv_calib = 0
            Writel ((ULONG)0x20ed200300, 0x08001829U);////obsv_calib = 0
            Writel ((ULONG)0x20ed20033c, 0x08001829U);////obsv_calib = 0
            Writel ((ULONG)0x20ed200378, 0x08001829U);////obsv_calib = 0
            Writel ((ULONG)0x20ed2000f8, 0x809b4030U);////cdr_out_sel = 0
            Writel ((ULONG)0x20ed200134, 0x809b4030U);////cdr_out_sel = 0
            Writel ((ULONG)0x20ed200170, 0x809b4030U);////cdr_out_sel = 0
            Writel ((ULONG)0x20ed2001ac, 0x809b4030U);////cdr_out_sel = 0
            Writel ((ULONG)0x20ed2001e8, 0x809b4030U);////cdr_out_sel = 0
            Writel ((ULONG)0x20ed200224, 0x809b4030U);////cdr_out_sel = 0
            Writel ((ULONG)0x20ed200260, 0x809b4030U);////cdr_out_sel = 0
            Writel ((ULONG)0x20ed20029c, 0x809b4030U);////cdr_out_sel = 0
            Writel ((ULONG)0x20ed2002d8, 0x809b4030U);////cdr_out_sel = 0
            Writel ((ULONG)0x20ed200314, 0x809b4030U);////cdr_out_sel = 0
            Writel ((ULONG)0x20ed200350, 0x809b4030U);////cdr_out_sel = 0
            Writel ((ULONG)0x20ed20038c, 0x809b4030U);////cdr_out_sel = 0
            // Readl ((ULONG)0x20ed200088);//
            // Readl ((ULONG)0x20ed200090);//
            // Readl ((ULONG)0x20ed200098);//
            // Readl ((ULONG)0x20ed2000a0);//
            // Readl ((ULONG)0x20ed2000a8);//
            // Readl ((ULONG)0x20ed2000b0);//
            // Readl ((ULONG)0x20ed2000b8);//
            // Readl ((ULONG)0x20ed2000c0);//
            // Readl ((ULONG)0x20ed2000c8);//
            // Readl ((ULONG)0x20ed2000d0);//
            // Readl ((ULONG)0x20ed2000d8);//
            // Readl ((ULONG)0x20ed2000e0);//
            // Readl ((ULONG)0x20ed2003c0);// Check PHY status
            // Readl ((ULONG)0x20ed2003d0);// Check PHY status
            // Readl ((ULONG)0x20ed2003e0);// Check PHY status
            // Readl ((ULONG)0x20ed2003f0);// Check PHY status
            // Readl ((ULONG)0x20ed200400);// Check PHY status
            // Readl ((ULONG)0x20ed200410);// Check PHY status
            // Readl ((ULONG)0x20ed200420);// Check PHY status
            // Readl ((ULONG)0x20ed200430);// Check PHY status
            // Readl ((ULONG)0x20ed200440);// Check PHY status
            // Readl ((ULONG)0x20ed200450);// Check PHY status
            // Readl ((ULONG)0x20ed200460);// Check PHY status
            // Readl ((ULONG)0x20ed200470);// Check PHY status

            ////== Slicer Offset calibration
            ////lpgain_vcm      .<0>    .<4'h0> .<//lpgain of calibration, set to 0>
            ////sel_calib       .<0>    .<3'h7> .<//sel_calib = 7, calibre slicer offset>
            ////start_calib     .<0>    .<1'h1> .<//start_calib = 1>
            ////;wait 20.ms
            ////start_calib     .<0>    .<1'h0> .<//start_calib = 0>
            ////;wait 20.ms
            Writel ((ULONG)0x20ed2000e8, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed200124, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed200160, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed20019c, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed2001d8, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed200214, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed200250, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed20028c, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed2002c8, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed200304, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed200340, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed20037c, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed2000ec, 0xc20121a0U);////pd_sf = 0
            Writel ((ULONG)0x20ed200128, 0xc20121a0U);////pd_sf = 0
            Writel ((ULONG)0x20ed200164, 0xc20121a0U);////pd_sf = 0
            Writel ((ULONG)0x20ed2001a0, 0xc20121a0U);////pd_sf = 0
            Writel ((ULONG)0x20ed2001dc, 0xc20121a0U);////pd_sf = 0
            Writel ((ULONG)0x20ed200218, 0xc20121a0U);////pd_sf = 0
            Writel ((ULONG)0x20ed200254, 0xc20121a0U);////pd_sf = 0
            Writel ((ULONG)0x20ed200290, 0xc20121a0U);////pd_sf = 0
            Writel ((ULONG)0x20ed2002cc, 0xc20121a0U);////pd_sf = 0
            Writel ((ULONG)0x20ed200308, 0xc20121a0U);////pd_sf = 0
            Writel ((ULONG)0x20ed200344, 0xc20121a0U);////pd_sf = 0
            Writel ((ULONG)0x20ed200380, 0xc20121a0U);////pd_sf = 0
            Writel ((ULONG)0x20ed2000e8, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed200124, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed200160, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed20019c, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed2001d8, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed200214, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed200250, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed20028c, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed2002c8, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed200304, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed200340, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed20037c, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed2000e4, 0x08001c29U);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed200120, 0x08001c29U);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed20015c, 0x08001c29U);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed200198, 0x08001c29U);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed2001d4, 0x08001c29U);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed200210, 0x08001c29U);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed20024c, 0x08001c29U);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed200288, 0x08001c29U);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed2002c4, 0x08001c29U);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed200300, 0x08001c29U);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed20033c, 0x08001c29U);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed200378, 0x08001c29U);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed2000ec, 0xc20131a0U);////power down calibration circuit
            Writel ((ULONG)0x20ed200128, 0xc20131a0U);////power down calibration circuit
            Writel ((ULONG)0x20ed200164, 0xc20131a0U);////power down calibration circuit
            Writel ((ULONG)0x20ed2001a0, 0xc20131a0U);////power down calibration circuit
            Writel ((ULONG)0x20ed2001dc, 0xc20131a0U);////power down calibration circuit
            Writel ((ULONG)0x20ed200218, 0xc20131a0U);////power down calibration circuit
            Writel ((ULONG)0x20ed200254, 0xc20131a0U);////power down calibration circuit
            Writel ((ULONG)0x20ed200290, 0xc20131a0U);////power down calibration circuit
            Writel ((ULONG)0x20ed2002cc, 0xc20131a0U);////power down calibration circuit
            Writel ((ULONG)0x20ed200308, 0xc20131a0U);////power down calibration circuit
            Writel ((ULONG)0x20ed200344, 0xc20131a0U);////power down calibration circuit
            Writel ((ULONG)0x20ed200380, 0xc20131a0U);////power down calibration circuit
            // Readl ((ULONG)0x20ed2003c0);// Check PHY status
            // Readl ((ULONG)0x20ed2003d0);// Check PHY status
            // Readl ((ULONG)0x20ed2003e0);// Check PHY status
            // Readl ((ULONG)0x20ed2003f0);// Check PHY status
            // Readl ((ULONG)0x20ed200400);// Check PHY status
            // Readl ((ULONG)0x20ed200410);// Check PHY status
            // Readl ((ULONG)0x20ed200420);// Check PHY status
            // Readl ((ULONG)0x20ed200430);// Check PHY status
            // Readl ((ULONG)0x20ed200440);// Check PHY status
            // Readl ((ULONG)0x20ed200450);// Check PHY status
            // Readl ((ULONG)0x20ed200460);// Check PHY status
            // Readl ((ULONG)0x20ed200470);// Check PHY status
            ////=========================================
        } else if (Stage == 2U) {
            Writel ((ULONG)0x20ed2000e8, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed200124, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed200160, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed20019c, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed2001d8, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed200214, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed200250, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed20028c, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed2002c8, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed200304, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed200340, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed20037c, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed2000f0, 0x03070585U);////vga_func_en_in = 1
            Writel ((ULONG)0x20ed20012c, 0x03070585U);////vga_func_en_in = 1
            Writel ((ULONG)0x20ed200168, 0x03070585U);////vga_func_en_in = 1
            Writel ((ULONG)0x20ed2001a4, 0x03070585U);////vga_func_en_in = 1
            Writel ((ULONG)0x20ed2001e0, 0x03070585U);////vga_func_en_in = 1
            Writel ((ULONG)0x20ed20021c, 0x03070585U);////vga_func_en_in = 1
            Writel ((ULONG)0x20ed200258, 0x03070585U);////vga_func_en_in = 1
            Writel ((ULONG)0x20ed200294, 0x03070585U);////vga_func_en_in = 1
            Writel ((ULONG)0x20ed2002d0, 0x03070585U);////vga_func_en_in = 1
            Writel ((ULONG)0x20ed20030c, 0x03070585U);////vga_func_en_in = 1
            Writel ((ULONG)0x20ed200348, 0x03070585U);////vga_func_en_in = 1
            Writel ((ULONG)0x20ed200384, 0x03070585U);////vga_func_en_in = 1
            //DelayMs(20);
            DelayMs(1);
            Writel ((ULONG)0x20ed2000f0, 0x03071585U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed20012c, 0x03071585U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed200168, 0x03071585U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed2001a4, 0x03071585U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed2001e0, 0x03071585U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed20021c, 0x03071585U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed200258, 0x03071585U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed200294, 0x03071585U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed2002d0, 0x03071585U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed20030c, 0x03071585U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed200348, 0x03071585U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed200384, 0x03071585U);////enable vga SSLMS loop
            //DelayMs(20);
            DelayMs(1);
            Writel ((ULONG)0x20ed2000ec, 0xc00131a0U);////disable vga rpause
            Writel ((ULONG)0x20ed200128, 0xc00131a0U);////disable vga rpause
            Writel ((ULONG)0x20ed200164, 0xc00131a0U);////disable vga rpause
            Writel ((ULONG)0x20ed2001a0, 0xc00131a0U);////disable vga rpause
            Writel ((ULONG)0x20ed2001dc, 0xc00131a0U);////disable vga rpause
            Writel ((ULONG)0x20ed200218, 0xc00131a0U);////disable vga rpause
            Writel ((ULONG)0x20ed200254, 0xc00131a0U);////disable vga rpause
            Writel ((ULONG)0x20ed200290, 0xc00131a0U);////disable vga rpause
            Writel ((ULONG)0x20ed2002cc, 0xc00131a0U);////disable vga rpause
            Writel ((ULONG)0x20ed200308, 0xc00131a0U);////disable vga rpause
            Writel ((ULONG)0x20ed200344, 0xc00131a0U);////disable vga rpause
            Writel ((ULONG)0x20ed200380, 0xc00131a0U);////disable vga rpause
            //DelayMs(20);
            DelayMs(1);
            Writel ((ULONG)0x20ed2000ec, 0xc20131a0U);////enable vga rpause
            Writel ((ULONG)0x20ed200128, 0xc20131a0U);////enable vga rpause
            Writel ((ULONG)0x20ed200164, 0xc20131a0U);////enable vga rpause
            Writel ((ULONG)0x20ed2001a0, 0xc20131a0U);////enable vga rpause
            Writel ((ULONG)0x20ed2001dc, 0xc20131a0U);////enable vga rpause
            Writel ((ULONG)0x20ed200218, 0xc20131a0U);////enable vga rpause
            Writel ((ULONG)0x20ed200254, 0xc20131a0U);////enable vga rpause
            Writel ((ULONG)0x20ed200290, 0xc20131a0U);////enable vga rpause
            Writel ((ULONG)0x20ed2002cc, 0xc20131a0U);////enable vga rpause
            Writel ((ULONG)0x20ed200308, 0xc20131a0U);////enable vga rpause
            Writel ((ULONG)0x20ed200344, 0xc20131a0U);////enable vga rpause
            Writel ((ULONG)0x20ed200380, 0xc20131a0U);////enable vga rpause
            //DelayMs(20);
            DelayMs(1);
            Writel ((ULONG)0x20ed2000f0, 0x03072585U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed20012c, 0x03072585U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed200168, 0x03072585U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed2001a4, 0x03072585U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed2001e0, 0x03072585U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed20021c, 0x03072585U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed200258, 0x03072585U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed200294, 0x03072585U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed2002d0, 0x03072585U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed20030c, 0x03072585U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed200348, 0x03072585U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed200384, 0x03072585U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed2000ec, 0xc2013180U);////disable vref_pause
            Writel ((ULONG)0x20ed200128, 0xc2013180U);////disable vref_pause
            Writel ((ULONG)0x20ed200164, 0xc2013180U);////disable vref_pause
            Writel ((ULONG)0x20ed2001a0, 0xc2013180U);////disable vref_pause
            Writel ((ULONG)0x20ed2001dc, 0xc2013180U);////disable vref_pause
            Writel ((ULONG)0x20ed200218, 0xc2013180U);////disable vref_pause
            Writel ((ULONG)0x20ed200254, 0xc2013180U);////disable vref_pause
            Writel ((ULONG)0x20ed200290, 0xc2013180U);////disable vref_pause
            Writel ((ULONG)0x20ed2002cc, 0xc2013180U);////disable vref_pause
            Writel ((ULONG)0x20ed200308, 0xc2013180U);////disable vref_pause
            Writel ((ULONG)0x20ed200344, 0xc2013180U);////disable vref_pause
            Writel ((ULONG)0x20ed200380, 0xc2013180U);////disable vref_pause
            ////wait 40.ms
            Writel ((ULONG)0x20ed2000ec, 0xc20131a0U);////enable vref_pause
            Writel ((ULONG)0x20ed200128, 0xc20131a0U);////enable vref_pause
            Writel ((ULONG)0x20ed200164, 0xc20131a0U);////enable vref_pause
            Writel ((ULONG)0x20ed2001a0, 0xc20131a0U);////enable vref_pause
            Writel ((ULONG)0x20ed2001dc, 0xc20131a0U);////enable vref_pause
            Writel ((ULONG)0x20ed200218, 0xc20131a0U);////enable vref_pause
            Writel ((ULONG)0x20ed200254, 0xc20131a0U);////enable vref_pause
            Writel ((ULONG)0x20ed200290, 0xc20131a0U);////enable vref_pause
            Writel ((ULONG)0x20ed2002cc, 0xc20131a0U);////enable vref_pause
            Writel ((ULONG)0x20ed200308, 0xc20131a0U);////enable vref_pause
            Writel ((ULONG)0x20ed200344, 0xc20131a0U);////enable vref_pause
            Writel ((ULONG)0x20ed200380, 0xc20131a0U);////enable vref_pause
            ////sslms_eq_mode       .<0>    .<3'h3> .<//enable ctle SSLMS loop>
            ////sslms_ctle_rpause   .<0>    .<1'h0> .<//ctle rpause = 0 >
            ////;wait 20.ms
            ////sslms_ctle_rpause   .<0>    .<1'h1> .<//ctle rpause = 1>
            ////sslms_ctle_cpause   .<0>    .<1'h0> .<//ctle cpause = 0>
            ////;wait 20.ms
            ////sslms_ctle_cpause   .<0>    .<1'h1> .<//ctle cpause = 1>
            ////sslms_eq_mode   .<0>    .<3'h4> .<//enable dfe SSLMS loop>
            ////;wait 60.ms
            ////sslms_dfe_pause     .<0>    .<1'h1> .<//enable dfe pause>
            ////sslms_vref_pause    .<0>    .<1'h1> .<//enable vref_pause>
            ////;wait 1.ms
            ////Setup RXPHY
            Writel ((ULONG)0x20ed200474, 0x00001201U);////rct_phy_setup=1
            Writel ((ULONG)0x20ed200480, 0x00001201U);////rct_phy_setup=1
            Writel ((ULONG)0x20ed20048c, 0x00001201U);////rct_phy_setup=1
            Writel ((ULONG)0x20ed200498, 0x00001201U);////rct_phy_setup=1
            Writel ((ULONG)0x20ed2004a4, 0x00001201U);////rct_phy_setup=1
            Writel ((ULONG)0x20ed2004b0, 0x00001201U);////rct_phy_setup=1
            Writel ((ULONG)0x20ed2004bc, 0x00001201U);////rct_phy_setup=1
            Writel ((ULONG)0x20ed2004c8, 0x00001201U);////rct_phy_setup=1
            Writel ((ULONG)0x20ed2004d4, 0x00001201U);////rct_phy_setup=1
            Writel ((ULONG)0x20ed2004e0, 0x00001201U);////rct_phy_setup=1
            Writel ((ULONG)0x20ed2004ec, 0x00001201U);////rct_phy_setup=1
            Writel ((ULONG)0x20ed2004f8, 0x00001201U);////rct_phy_setup=1
            Writel ((ULONG)0x20ed200474, 0x00001200U);////rct_phy_setup=0
            Writel ((ULONG)0x20ed200480, 0x00001200U);////rct_phy_setup=0
            Writel ((ULONG)0x20ed20048c, 0x00001200U);////rct_phy_setup=0
            Writel ((ULONG)0x20ed200498, 0x00001200U);////rct_phy_setup=0
            Writel ((ULONG)0x20ed2004a4, 0x00001200U);////rct_phy_setup=0
            Writel ((ULONG)0x20ed2004b0, 0x00001200U);////rct_phy_setup=0
            Writel ((ULONG)0x20ed2004bc, 0x00001200U);////rct_phy_setup=0
            Writel ((ULONG)0x20ed2004c8, 0x00001200U);////rct_phy_setup=0
            Writel ((ULONG)0x20ed2004d4, 0x00001200U);////rct_phy_setup=0
            Writel ((ULONG)0x20ed2004e0, 0x00001200U);////rct_phy_setup=0
            Writel ((ULONG)0x20ed2004ec, 0x00001200U);////rct_phy_setup=0
            Writel ((ULONG)0x20ed2004f8, 0x00001200U);////rct_phy_setup=0
            DelayMs(1);

            /* Setup PHY */
            Writel ((ULONG)0x20ed1e0800U, 0x408dU);
        } else if (Stage == 3U) {
            Writel ((ULONG)0x20ed1e0800U, 0x402dU);
        } else {
            /* avoid misrac parser error */
        }
    } else { // 20211003 phy setting (non-calibre version)
        if (Stage == 0U) {
            ////==========
            ////Config PHY
            ////==========
            ////==========
            ////Config SerDes
            ////==========
            Writel ((ULONG)0x20ed200474, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed200480, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed20048c, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed200498, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2004a4, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2004b0, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2004bc, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2004c8, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2004d4, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2004e0, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2004ec, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2004f8, 0x00000200U);////sq_mode = 0
            Writel ((ULONG)0x20ed2000ec, 0x020131a8U);////pd_ctle = 0
            Writel ((ULONG)0x20ed200128, 0x020131a8U);////pd_ctle = 0
            Writel ((ULONG)0x20ed200164, 0x020131a8U);////pd_ctle = 0
            Writel ((ULONG)0x20ed2001a0, 0x020131a8U);////pd_ctle = 0
            Writel ((ULONG)0x20ed2001dc, 0x020131a8U);////pd_ctle = 0
            Writel ((ULONG)0x20ed200218, 0x020131a8U);////pd_ctle = 0
            Writel ((ULONG)0x20ed200254, 0x020131a8U);////pd_ctle = 0
            Writel ((ULONG)0x20ed200290, 0x020131a8U);////pd_ctle = 0
            Writel ((ULONG)0x20ed2002cc, 0x020131a8U);////pd_ctle = 0
            Writel ((ULONG)0x20ed200308, 0x020131a8U);////pd_ctle = 0
            Writel ((ULONG)0x20ed200344, 0x020131a8U);////pd_ctle = 0
            Writel ((ULONG)0x20ed200380, 0x020131a8U);////pd_ctle = 0
            Writel ((ULONG)0x20ed2000f0, 0x00040585U);////ctle_func_en_in = 0
            Writel ((ULONG)0x20ed20012c, 0x00040585U);////ctle_func_en_in = 0
            Writel ((ULONG)0x20ed200168, 0x00040585U);////ctle_func_en_in = 0
            Writel ((ULONG)0x20ed2001a4, 0x00040585U);////ctle_func_en_in = 0
            Writel ((ULONG)0x20ed2001e0, 0x00040585U);////ctle_func_en_in = 0
            Writel ((ULONG)0x20ed20021c, 0x00040585U);////ctle_func_en_in = 0
            Writel ((ULONG)0x20ed200258, 0x00040585U);////ctle_func_en_in = 0
            Writel ((ULONG)0x20ed200294, 0x00040585U);////ctle_func_en_in = 0
            Writel ((ULONG)0x20ed2002d0, 0x00040585U);////ctle_func_en_in = 0
            Writel ((ULONG)0x20ed20030c, 0x00040585U);////ctle_func_en_in = 0
            Writel ((ULONG)0x20ed200348, 0x00040585U);////ctle_func_en_in = 0
            Writel ((ULONG)0x20ed200384, 0x00040585U);////ctle_func_en_in = 0
            Writel ((ULONG)0x20ed2000f0, 0x00000585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed20012c, 0x00000585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed200168, 0x00000585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed2001a4, 0x00000585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed2001e0, 0x00000585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed20021c, 0x00000585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed200258, 0x00000585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed200294, 0x00000585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed2002d0, 0x00000585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed20030c, 0x00000585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed200348, 0x00000585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed200384, 0x00000585U);////vga_func_en_in = 0
            Writel ((ULONG)0x20ed2000e4, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed200120, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed20015c, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed200198, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed2001d4, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed200210, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed20024c, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed200288, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed2002c4, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed200300, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed20033c, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed200378, 0x08001029U);////bypasspi = 0
            Writel ((ULONG)0x20ed2000f4, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed200130, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed20016c, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed2001a8, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed2001e4, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed200220, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed20025c, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed200298, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed2002d4, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed200310, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed20034c, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed200388, 0xd233b400U);////pibw = 3
            Writel ((ULONG)0x20ed2000e8, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed200124, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed200160, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed20019c, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed2001d8, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed200214, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed200250, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed20028c, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed2002c8, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed200304, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed200340, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed20037c, 0x05095e01U);////bypassDcc = 1
            Writel ((ULONG)0x20ed200100, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed20013c, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed200178, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed2001b4, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed2001f0, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed20022c, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed200268, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed2002a4, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed2002e0, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed20031c, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed200358, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed200394, 0x00000352U);////div_int = 2
            Writel ((ULONG)0x20ed2000f8, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed200134, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed200170, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed2001ac, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed2001e8, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed200224, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed200260, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed20029c, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed2002d8, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed200314, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed200350, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed20038c, 0xb0934033U);////phased_fr_gain = 9
            Writel ((ULONG)0x20ed2000e4, 0x08001029U);////slvs_mode = 0
            Writel ((ULONG)0x20ed200120, 0x08001029U);////slvs_mode = 0
            Writel ((ULONG)0x20ed20015c, 0x08001029U);////slvs_mode = 0
            Writel ((ULONG)0x20ed200198, 0x08001029U);////slvs_mode = 0
            Writel ((ULONG)0x20ed2001d4, 0x08001029U);////slvs_mode = 0
            Writel ((ULONG)0x20ed200210, 0x08001029U);////slvs_mode = 0
            Writel ((ULONG)0x20ed20024c, 0x08001029U);////slvs_mode = 0
            Writel ((ULONG)0x20ed200288, 0x08001029U);////slvs_mode = 0
            Writel ((ULONG)0x20ed2002c4, 0x08001029U);////slvs_mode = 0
            Writel ((ULONG)0x20ed200300, 0x08001029U);////slvs_mode = 0
            Writel ((ULONG)0x20ed20033c, 0x08001029U);////slvs_mode = 0
            Writel ((ULONG)0x20ed200378, 0x08001029U);////slvs_mode = 0
            Writel ((ULONG)0x20ed200108, 0x20001f40U);////rctrl_in = 5'h1f, set termination value to be smallest due to the termination bug
            Writel ((ULONG)0x20ed200144, 0x20001f40U);////rctrl_in = 5'h1f, set termination value to be smallest due to the termination bug
            Writel ((ULONG)0x20ed200180, 0x20001f40U);////rctrl_in = 5'h1f, set termination value to be smallest due to the termination bug
            Writel ((ULONG)0x20ed2001bc, 0x20001f40U);////rctrl_in = 5'h1f, set termination value to be smallest due to the termination bug
            Writel ((ULONG)0x20ed2001f8, 0x20001f40U);////rctrl_in = 5'h1f, set termination value to be smallest due to the termination bug
            Writel ((ULONG)0x20ed200234, 0x20001f40U);////rctrl_in = 5'h1f, set termination value to be smallest due to the termination bug
            Writel ((ULONG)0x20ed200270, 0x20001f40U);////rctrl_in = 5'h1f, set termination value to be smallest due to the termination bug
            Writel ((ULONG)0x20ed2002ac, 0x20001f40U);////rctrl_in = 5'h1f, set termination value to be smallest due to the termination bug
            Writel ((ULONG)0x20ed2002e8, 0x20001f40U);////rctrl_in = 5'h1f, set termination value to be smallest due to the termination bug
            Writel ((ULONG)0x20ed200324, 0x20001f40U);////rctrl_in = 5'h1f, set termination value to be smallest due to the termination bug
            Writel ((ULONG)0x20ed200360, 0x20001f40U);////rctrl_in = 5'h1f, set termination value to be smallest due to the termination bug
            Writel ((ULONG)0x20ed20039c, 0x20001f40U);////rctrl_in = 5'h1f, set termination value to be smallest due to the termination bug
            Writel ((ULONG)0x20ed200108, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed200144, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed200180, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed2001bc, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed2001f8, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed200234, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed200270, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed2002ac, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed2002e8, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed200324, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed200360, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed20039c, 0x20003f40U);////manual_term_en=1, enable manual termination setting
            Writel ((ULONG)0x20ed2000e8, 0x05095e01U);////bypassctle=1, it's a bug here, =1 means not bypass CTLE
            Writel ((ULONG)0x20ed200124, 0x05095e01U);////bypassctle=1, it's a bug here, =1 means not bypass CTLE
            Writel ((ULONG)0x20ed200160, 0x05095e01U);////bypassctle=1, it's a bug here, =1 means not bypass CTLE
            Writel ((ULONG)0x20ed20019c, 0x05095e01U);////bypassctle=1, it's a bug here, =1 means not bypass CTLE
            Writel ((ULONG)0x20ed2001d8, 0x05095e01U);////bypassctle=1, it's a bug here, =1 means not bypass CTLE
            Writel ((ULONG)0x20ed200214, 0x05095e01U);////bypassctle=1, it's a bug here, =1 means not bypass CTLE
            Writel ((ULONG)0x20ed200250, 0x05095e01U);////bypassctle=1, it's a bug here, =1 means not bypass CTLE
            Writel ((ULONG)0x20ed20028c, 0x05095e01U);////bypassctle=1, it's a bug here, =1 means not bypass CTLE
            Writel ((ULONG)0x20ed2002c8, 0x05095e01U);////bypassctle=1, it's a bug here, =1 means not bypass CTLE
            Writel ((ULONG)0x20ed200304, 0x05095e01U);////bypassctle=1, it's a bug here, =1 means not bypass CTLE
            Writel ((ULONG)0x20ed200340, 0x05095e01U);////bypassctle=1, it's a bug here, =1 means not bypass CTLE
            Writel ((ULONG)0x20ed20037c, 0x05095e01U);////bypassctle=1, it's a bug here, =1 means not bypass CTLE
            Writel ((ULONG)0x20ed2000f4, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed200130, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed20016c, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed2001a8, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed2001e4, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed200220, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed20025c, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed200298, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed2002d4, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed200310, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed20034c, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed200388, 0xd233b400U);////not bypass VGA VCM in calibration controller
            Writel ((ULONG)0x20ed2000f4, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed200130, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed20016c, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed2001a8, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed2001e4, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed200220, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed20025c, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed200298, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed2002d4, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed200310, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed20034c, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed200388, 0xd233b000U);////not bypass CTLE VCM in calibration controller
            Writel ((ULONG)0x20ed2000f4, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed200130, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed20016c, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed2001a8, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed2001e4, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed200220, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed20025c, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed200298, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed2002d4, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed200310, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed20034c, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed200388, 0xd233b000U);////not bypass SlicerBuf VCM in calibration controller
            Writel ((ULONG)0x20ed2000f4, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed200130, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed20016c, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed2001a8, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed2001e4, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed200220, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed20025c, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed200298, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed2002d4, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed200310, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed20034c, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed200388, 0xd233b000U);////not bypass DFE summer VCM in calibration controller
            Writel ((ULONG)0x20ed2000f8, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed200134, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed200170, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed2001ac, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed2001e8, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed200224, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed200260, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed20029c, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed2002d8, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed200314, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed200350, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed20038c, 0xb09b4033U);////bypass VGA offset in calibration controller
            Writel ((ULONG)0x20ed2000f4, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed200130, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed20016c, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed2001a8, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed2001e4, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed200220, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed20025c, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed200298, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed2002d4, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed200310, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed20034c, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed200388, 0xda33b000U);////bypass CTLE1 offset in calibration controller
            Writel ((ULONG)0x20ed2000f4, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed200130, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed20016c, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed2001a8, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed2001e4, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed200220, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed20025c, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed200298, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed2002d4, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed200310, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed20034c, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed200388, 0xda33b000U);////bypass CTLE2 offset in calibration controller
            Writel ((ULONG)0x20ed2000f4, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed200130, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed20016c, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed2001a8, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed2001e4, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed200220, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed20025c, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed200298, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed2002d4, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed200310, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed20034c, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed200388, 0xda3bb000U);////bypass Slicer offset in calibration controller
            Writel ((ULONG)0x20ed2000ec, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed200128, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed200164, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed2001a0, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed2001dc, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed200218, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed200254, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed200290, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed2002cc, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed200308, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed200344, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed200380, 0xc20131a8U);////set slicer BW to be the fastest
            Writel ((ULONG)0x20ed2000f0, 0x03000585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed20012c, 0x03000585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed200168, 0x03000585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed2001a4, 0x03000585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed2001e0, 0x03000585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed20021c, 0x03000585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed200258, 0x03000585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed200294, 0x03000585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed2002d0, 0x03000585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed20030c, 0x03000585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed200348, 0x03000585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed200384, 0x03000585U);////set slicer_buf BW to be the fastest
            Writel ((ULONG)0x20ed2000ec, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed200128, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed200164, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed2001a0, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed2001dc, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed200218, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed200254, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed200290, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed2002cc, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed200308, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed200344, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed200380, 0xc20131a8U);////sf_ictrl = 0
            Writel ((ULONG)0x20ed2000ec, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed200128, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed200164, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed2001a0, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed2001dc, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed200218, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed200254, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed200290, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed2002cc, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed200308, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed200344, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed200380, 0xc24131a8U);////pd_sf = 1
            Writel ((ULONG)0x20ed2000e8, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed200124, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed200160, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed20019c, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed2001d8, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed200214, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed200250, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed20028c, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed2002c8, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed200304, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed200340, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed20037c, 0x05095e01U);////manual_vgaCtle_offset = 0
            Writel ((ULONG)0x20ed20010c, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed200148, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed200184, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed2001c0, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed2001fc, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed200238, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed200274, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed2002b0, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed2002ec, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed200328, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed200364, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed2003a0, 0x00000000U);////offset_code_in = 6'h0
            Writel ((ULONG)0x20ed2000ec, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed200128, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed200164, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed2001a0, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed2001dc, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed200218, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed200254, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed200290, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed2002cc, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed200308, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed200344, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed200380, 0xc24121a8U);////power on calibration circuit
            Writel ((ULONG)0x20ed2000e8, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed200124, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed200160, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed20019c, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed2001d8, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed200214, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed200250, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed20028c, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed2002c8, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed200304, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed200340, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed20037c, 0x05095601U);////enable eye momnitor path for calibration
            Writel ((ULONG)0x20ed2000e8, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed200124, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed200160, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed20019c, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed2001d8, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed200214, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed200250, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed20028c, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed2002c8, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed200304, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed200340, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed20037c, 0x050d5601U);////set sgnvref to be 1'b1
            Writel ((ULONG)0x20ed2000e8, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed200124, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed200160, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed20019c, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed2001d8, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed200214, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed200250, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed20028c, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed2002c8, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed200304, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed200340, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed20037c, 0x050d5201U);////enable err slicer
            Writel ((ULONG)0x20ed200110, 0x8801f000U);////set vga_rctrl_in= 1
            Writel ((ULONG)0x20ed20014c, 0x8801f000U);////set vga_rctrl_in= 1
            Writel ((ULONG)0x20ed200188, 0x8801f000U);////set vga_rctrl_in= 1
            Writel ((ULONG)0x20ed2001c4, 0x8801f000U);////set vga_rctrl_in= 1
            Writel ((ULONG)0x20ed200200, 0x8801f000U);////set vga_rctrl_in= 1
            Writel ((ULONG)0x20ed20023c, 0x8801f000U);////set vga_rctrl_in= 1
            Writel ((ULONG)0x20ed200278, 0x8801f000U);////set vga_rctrl_in= 1
            Writel ((ULONG)0x20ed2002b4, 0x8801f000U);////set vga_rctrl_in= 1
            Writel ((ULONG)0x20ed2002f0, 0x8801f000U);////set vga_rctrl_in= 1
            Writel ((ULONG)0x20ed20032c, 0x8801f000U);////set vga_rctrl_in= 1
            Writel ((ULONG)0x20ed200368, 0x8801f000U);////set vga_rctrl_in= 1
            Writel ((ULONG)0x20ed2003a4, 0x8801f000U);////set vga_rctrl_in= 1
            Writel ((ULONG)0x20ed2000ec, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed200128, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed200164, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed2001a0, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed2001dc, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed200218, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed200254, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed200290, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed2002cc, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed200308, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed200344, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed200380, 0xc24121a8U);////set gainSel_slicerBuf_in
            Writel ((ULONG)0x20ed2000ec, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200128, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200164, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed2001a0, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed2001dc, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200218, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200254, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200290, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed2002cc, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200308, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200344, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200380, 0xc24121a8U);////set gainSel_unitSummer_in
            Writel ((ULONG)0x20ed200114, 0x10000000U);////config vga rgain
            Writel ((ULONG)0x20ed200150, 0x10000000U);////config vga rgain
            Writel ((ULONG)0x20ed20018c, 0x10000000U);////config vga rgain
            Writel ((ULONG)0x20ed2001c8, 0x10000000U);////config vga rgain
            Writel ((ULONG)0x20ed200204, 0x10000000U);////config vga rgain
            Writel ((ULONG)0x20ed200240, 0x10000000U);////config vga rgain
            Writel ((ULONG)0x20ed20027c, 0x10000000U);////config vga rgain
            Writel ((ULONG)0x20ed2002b8, 0x10000000U);////config vga rgain
            Writel ((ULONG)0x20ed2002f4, 0x10000000U);////config vga rgain
            Writel ((ULONG)0x20ed200330, 0x10000000U);////config vga rgain
            Writel ((ULONG)0x20ed20036c, 0x10000000U);////config vga rgain
            Writel ((ULONG)0x20ed2003a8, 0x10000000U);////config vga rgain
            Writel ((ULONG)0x20ed2000ec, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed200128, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed200164, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed2001a0, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed2001dc, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed200218, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed200254, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed200290, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed2002cc, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed200308, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed200344, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed200380, 0xc24121a8U);////enable vga rpause
            Writel ((ULONG)0x20ed2000ec, 0xc24121a8U);////sslms_vga_manual = 0
            Writel ((ULONG)0x20ed200128, 0xc24121a8U);////sslms_vga_manual = 0
            Writel ((ULONG)0x20ed200164, 0xc24121a8U);////sslms_vga_manual = 0
            Writel ((ULONG)0x20ed2001a0, 0xc24121a8U);////sslms_vga_manual = 0
            Writel ((ULONG)0x20ed2001dc, 0xc24121a8U);////sslms_vga_manual = 0
            Writel ((ULONG)0x20ed200218, 0xc24121a8U);////sslms_vga_manual = 0
            Writel ((ULONG)0x20ed200254, 0xc24121a8U);////sslms_vga_manual = 0
            Writel ((ULONG)0x20ed200290, 0xc24121a8U);////sslms_vga_manual = 0
            Writel ((ULONG)0x20ed2002cc, 0xc24121a8U);////sslms_vga_manual = 0
            Writel ((ULONG)0x20ed200308, 0xc24121a8U);////sslms_vga_manual = 0
            Writel ((ULONG)0x20ed200344, 0xc24121a8U);////sslms_vga_manual = 0
            Writel ((ULONG)0x20ed200380, 0xc24121a8U);////sslms_vga_manual = 0
            Writel ((ULONG)0x20ed2000ec, 0xc2412188U);////disable vref_pause
            Writel ((ULONG)0x20ed200128, 0xc2412188U);////disable vref_pause
            Writel ((ULONG)0x20ed200164, 0xc2412188U);////disable vref_pause
            Writel ((ULONG)0x20ed2001a0, 0xc2412188U);////disable vref_pause
            Writel ((ULONG)0x20ed2001dc, 0xc2412188U);////disable vref_pause
            Writel ((ULONG)0x20ed200218, 0xc2412188U);////disable vref_pause
            Writel ((ULONG)0x20ed200254, 0xc2412188U);////disable vref_pause
            Writel ((ULONG)0x20ed200290, 0xc2412188U);////disable vref_pause
            Writel ((ULONG)0x20ed2002cc, 0xc2412188U);////disable vref_pause
            Writel ((ULONG)0x20ed200308, 0xc2412188U);////disable vref_pause
            Writel ((ULONG)0x20ed200344, 0xc2412188U);////disable vref_pause
            Writel ((ULONG)0x20ed200380, 0xc2412188U);////disable vref_pause
            Writel ((ULONG)0x20ed200108, 0x20003f60U);////set vref_in
            Writel ((ULONG)0x20ed200144, 0x20003f60U);////set vref_in
            Writel ((ULONG)0x20ed200180, 0x20003f60U);////set vref_in
            Writel ((ULONG)0x20ed2001bc, 0x20003f60U);////set vref_in
            Writel ((ULONG)0x20ed2001f8, 0x20003f60U);////set vref_in
            Writel ((ULONG)0x20ed200234, 0x20003f60U);////set vref_in
            Writel ((ULONG)0x20ed200270, 0x20003f60U);////set vref_in
            Writel ((ULONG)0x20ed2002ac, 0x20003f60U);////set vref_in
            Writel ((ULONG)0x20ed2002e8, 0x20003f60U);////set vref_in
            Writel ((ULONG)0x20ed200324, 0x20003f60U);////set vref_in
            Writel ((ULONG)0x20ed200360, 0x20003f60U);////set vref_in
            Writel ((ULONG)0x20ed20039c, 0x20003f60U);////set vref_in
            Writel ((ULONG)0x20ed20010c, 0x02000000U);////set vref_gain
            Writel ((ULONG)0x20ed200148, 0x02000000U);////set vref_gain
            Writel ((ULONG)0x20ed200184, 0x02000000U);////set vref_gain
            Writel ((ULONG)0x20ed2001c0, 0x02000000U);////set vref_gain
            Writel ((ULONG)0x20ed2001fc, 0x02000000U);////set vref_gain
            Writel ((ULONG)0x20ed200238, 0x02000000U);////set vref_gain
            Writel ((ULONG)0x20ed200274, 0x02000000U);////set vref_gain
            Writel ((ULONG)0x20ed2002b0, 0x02000000U);////set vref_gain
            Writel ((ULONG)0x20ed2002ec, 0x02000000U);////set vref_gain
            Writel ((ULONG)0x20ed200328, 0x02000000U);////set vref_gain
            Writel ((ULONG)0x20ed200364, 0x02000000U);////set vref_gain
            Writel ((ULONG)0x20ed2003a0, 0x02000000U);////set vref_gain
            Writel ((ULONG)0x20ed2000ec, 0xc2412188U);////enable ctle rpause
            Writel ((ULONG)0x20ed200128, 0xc2412188U);////enable ctle rpause
            Writel ((ULONG)0x20ed200164, 0xc2412188U);////enable ctle rpause
            Writel ((ULONG)0x20ed2001a0, 0xc2412188U);////enable ctle rpause
            Writel ((ULONG)0x20ed2001dc, 0xc2412188U);////enable ctle rpause
            Writel ((ULONG)0x20ed200218, 0xc2412188U);////enable ctle rpause
            Writel ((ULONG)0x20ed200254, 0xc2412188U);////enable ctle rpause
            Writel ((ULONG)0x20ed200290, 0xc2412188U);////enable ctle rpause
            Writel ((ULONG)0x20ed2002cc, 0xc2412188U);////enable ctle rpause
            Writel ((ULONG)0x20ed200308, 0xc2412188U);////enable ctle rpause
            Writel ((ULONG)0x20ed200344, 0xc2412188U);////enable ctle rpause
            Writel ((ULONG)0x20ed200380, 0xc2412188U);////enable ctle rpause
            Writel ((ULONG)0x20ed2000ec, 0xc2412188U);////enable ctle cpause
            Writel ((ULONG)0x20ed200128, 0xc2412188U);////enable ctle cpause
            Writel ((ULONG)0x20ed200164, 0xc2412188U);////enable ctle cpause
            Writel ((ULONG)0x20ed2001a0, 0xc2412188U);////enable ctle cpause
            Writel ((ULONG)0x20ed2001dc, 0xc2412188U);////enable ctle cpause
            Writel ((ULONG)0x20ed200218, 0xc2412188U);////enable ctle cpause
            Writel ((ULONG)0x20ed200254, 0xc2412188U);////enable ctle cpause
            Writel ((ULONG)0x20ed200290, 0xc2412188U);////enable ctle cpause
            Writel ((ULONG)0x20ed2002cc, 0xc2412188U);////enable ctle cpause
            Writel ((ULONG)0x20ed200308, 0xc2412188U);////enable ctle cpause
            Writel ((ULONG)0x20ed200344, 0xc2412188U);////enable ctle cpause
            Writel ((ULONG)0x20ed200380, 0xc2412188U);////enable ctle cpause
            Writel ((ULONG)0x20ed200110, 0x8801f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed20014c, 0x8801f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed200188, 0x8801f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed2001c4, 0x8801f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed200200, 0x8801f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed20023c, 0x8801f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed200278, 0x8801f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed2002b4, 0x8801f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed2002f0, 0x8801f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed20032c, 0x8801f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed200368, 0x8801f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed2003a4, 0x8801f700U);////set ctle_rctrl0_in 7
            Writel ((ULONG)0x20ed200110, 0x88017700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed20014c, 0x88017700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed200188, 0x88017700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed2001c4, 0x88017700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed200200, 0x88017700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed20023c, 0x88017700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed200278, 0x88017700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed2002b4, 0x88017700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed2002f0, 0x88017700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed20032c, 0x88017700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed200368, 0x88017700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed2003a4, 0x88017700U);////set ctle_rctrl1_in 7
            Writel ((ULONG)0x20ed200110, 0x88017700U);////set ctle_cctrl0_in=0
            Writel ((ULONG)0x20ed20014c, 0x88017700U);////set ctle_cctrl0_in=0
            Writel ((ULONG)0x20ed200188, 0x88017700U);////set ctle_cctrl0_in=0
            Writel ((ULONG)0x20ed2001c4, 0x88017700U);////set ctle_cctrl0_in=0
            Writel ((ULONG)0x20ed200200, 0x88017700U);////set ctle_cctrl0_in=0
            Writel ((ULONG)0x20ed20023c, 0x88017700U);////set ctle_cctrl0_in=0
            Writel ((ULONG)0x20ed200278, 0x88017700U);////set ctle_cctrl0_in=0
            Writel ((ULONG)0x20ed2002b4, 0x88017700U);////set ctle_cctrl0_in=0
            Writel ((ULONG)0x20ed2002f0, 0x88017700U);////set ctle_cctrl0_in=0
            Writel ((ULONG)0x20ed20032c, 0x88017700U);////set ctle_cctrl0_in=0
            Writel ((ULONG)0x20ed200368, 0x88017700U);////set ctle_cctrl0_in=0
            Writel ((ULONG)0x20ed2003a4, 0x88017700U);////set ctle_cctrl0_in=0
            Writel ((ULONG)0x20ed200110, 0x88017700U);////set ctle_cctrl1_in=0
            Writel ((ULONG)0x20ed20014c, 0x88017700U);////set ctle_cctrl1_in=0
            Writel ((ULONG)0x20ed200188, 0x88017700U);////set ctle_cctrl1_in=0
            Writel ((ULONG)0x20ed2001c4, 0x88017700U);////set ctle_cctrl1_in=0
            Writel ((ULONG)0x20ed200200, 0x88017700U);////set ctle_cctrl1_in=0
            Writel ((ULONG)0x20ed20023c, 0x88017700U);////set ctle_cctrl1_in=0
            Writel ((ULONG)0x20ed200278, 0x88017700U);////set ctle_cctrl1_in=0
            Writel ((ULONG)0x20ed2002b4, 0x88017700U);////set ctle_cctrl1_in=0
            Writel ((ULONG)0x20ed2002f0, 0x88017700U);////set ctle_cctrl1_in=0
            Writel ((ULONG)0x20ed20032c, 0x88017700U);////set ctle_cctrl1_in=0
            Writel ((ULONG)0x20ed200368, 0x88017700U);////set ctle_cctrl1_in=0
            Writel ((ULONG)0x20ed2003a4, 0x88017700U);////set ctle_cctrl1_in=0
            Writel ((ULONG)0x20ed2000ec, 0xc2412188U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed200128, 0xc2412188U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed200164, 0xc2412188U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed2001a0, 0xc2412188U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed2001dc, 0xc2412188U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed200218, 0xc2412188U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed200254, 0xc2412188U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed200290, 0xc2412188U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed2002cc, 0xc2412188U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed200308, 0xc2412188U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed200344, 0xc2412188U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed200380, 0xc2412188U);////set ctle_maunal_mode= 0
            Writel ((ULONG)0x20ed2000ec, 0xc2412180U);////disable dfe pause
            Writel ((ULONG)0x20ed200128, 0xc2412180U);////disable dfe pause
            Writel ((ULONG)0x20ed200164, 0xc2412180U);////disable dfe pause
            Writel ((ULONG)0x20ed2001a0, 0xc2412180U);////disable dfe pause
            Writel ((ULONG)0x20ed2001dc, 0xc2412180U);////disable dfe pause
            Writel ((ULONG)0x20ed200218, 0xc2412180U);////disable dfe pause
            Writel ((ULONG)0x20ed200254, 0xc2412180U);////disable dfe pause
            Writel ((ULONG)0x20ed200290, 0xc2412180U);////disable dfe pause
            Writel ((ULONG)0x20ed2002cc, 0xc2412180U);////disable dfe pause
            Writel ((ULONG)0x20ed200308, 0xc2412180U);////disable dfe pause
            Writel ((ULONG)0x20ed200344, 0xc2412180U);////disable dfe pause
            Writel ((ULONG)0x20ed200380, 0xc2412180U);////disable dfe pause
            Writel ((ULONG)0x20ed200104, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed200140, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed20017c, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed2001b8, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed2001f4, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed200230, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed20026c, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed2002a8, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed2002e4, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed200320, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed20035c, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed200398, 0x00000000U);////set dfe_rgain
            Writel ((ULONG)0x20ed2000e4, 0x08001029U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed200120, 0x08001029U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed20015c, 0x08001029U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed200198, 0x08001029U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed2001d4, 0x08001029U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed200210, 0x08001029U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed20024c, 0x08001029U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed200288, 0x08001029U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed2002c4, 0x08001029U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed200300, 0x08001029U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed20033c, 0x08001029U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed200378, 0x08001029U);////choose IQ slicer mode = 0
            Writel ((ULONG)0x20ed200114, 0x1000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed200150, 0x1000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed20018c, 0x1000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed2001c8, 0x1000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed200204, 0x1000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed200240, 0x1000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed20027c, 0x1000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed2002b8, 0x1000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed2002f4, 0x1000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed200330, 0x1000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed20036c, 0x1000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed2003a8, 0x1000000fU);////lpgain of calibration, set to f
            Writel ((ULONG)0x20ed2000f4, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed200130, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed20016c, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed2001a8, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed2001e4, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed200220, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed20025c, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed200298, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed2002d4, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed200310, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed20034c, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed200388, 0xda3bb0feU);////calibration lock check duration, set to 80*13
            Writel ((ULONG)0x20ed2000f0, 0x03000585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed20012c, 0x03000585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed200168, 0x03000585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed2001a4, 0x03000585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed2001e0, 0x03000585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed20021c, 0x03000585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed200258, 0x03000585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed200294, 0x03000585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed2002d0, 0x03000585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed20030c, 0x03000585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed200348, 0x03000585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed200384, 0x03000585U);////calibration lock check threash hold, set to 0
            Writel ((ULONG)0x20ed20010c, 0x0200001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed200148, 0x0200001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed200184, 0x0200001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed2001c0, 0x0200001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed2001fc, 0x0200001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed200238, 0x0200001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed200274, 0x0200001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed2002b0, 0x0200001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed2002ec, 0x0200001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed200328, 0x0200001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed200364, 0x0200001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed2003a0, 0x0200001fU);////set reference voltage = 1f for auto calibration controller
            Writel ((ULONG)0x20ed200110, 0x88017700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed20014c, 0x88017700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed200188, 0x88017700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed2001c4, 0x88017700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed200200, 0x88017700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed20023c, 0x88017700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed200278, 0x88017700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed2002b4, 0x88017700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed2002f0, 0x88017700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed20032c, 0x88017700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed200368, 0x88017700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed2003a4, 0x88017700U);////set VGA Ctrl in = 8
            Writel ((ULONG)0x20ed200110, 0x88017700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed20014c, 0x88017700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed200188, 0x88017700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed2001c4, 0x88017700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed200200, 0x88017700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed20023c, 0x88017700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed200278, 0x88017700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed2002b4, 0x88017700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed2002f0, 0x88017700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed20032c, 0x88017700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed200368, 0x88017700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed2003a4, 0x88017700U);////set default CTLE vcm
            Writel ((ULONG)0x20ed2000f8, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed200134, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed200170, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed2001ac, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed2001e8, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed200224, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed200260, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed20029c, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed2002d8, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed200314, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed200350, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed20038c, 0x809b4033U);////set default slicerBuffer vcm
            Writel ((ULONG)0x20ed2000f8, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed200134, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed200170, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed2001ac, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed2001e8, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed200224, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed200260, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed20029c, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed2002d8, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed200314, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed200350, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed20038c, 0x809b4030U);////set default unitSummer vcm
            Writel ((ULONG)0x20ed2000e8, 0x050d5201U);//
            Writel ((ULONG)0x20ed200124, 0x050d5201U);//
            Writel ((ULONG)0x20ed200160, 0x050d5201U);//
            Writel ((ULONG)0x20ed20019c, 0x050d5201U);//
            Writel ((ULONG)0x20ed2001d8, 0x050d5201U);//
            Writel ((ULONG)0x20ed200214, 0x050d5201U);//
            Writel ((ULONG)0x20ed200250, 0x050d5201U);//
            Writel ((ULONG)0x20ed20028c, 0x050d5201U);//
            Writel ((ULONG)0x20ed2002c8, 0x050d5201U);//
            Writel ((ULONG)0x20ed200304, 0x050d5201U);//
            Writel ((ULONG)0x20ed200340, 0x050d5201U);//
            Writel ((ULONG)0x20ed20037c, 0x050d5201U);//
            Writel ((ULONG)0x20ed2000e8, 0x050d5201U);//
            Writel ((ULONG)0x20ed200124, 0x050d5201U);//
            Writel ((ULONG)0x20ed200160, 0x050d5201U);//
            Writel ((ULONG)0x20ed20019c, 0x050d5201U);//
            Writel ((ULONG)0x20ed2001d8, 0x050d5201U);//
            Writel ((ULONG)0x20ed200214, 0x050d5201U);//
            Writel ((ULONG)0x20ed200250, 0x050d5201U);//
            Writel ((ULONG)0x20ed20028c, 0x050d5201U);//
            Writel ((ULONG)0x20ed2002c8, 0x050d5201U);//
            Writel ((ULONG)0x20ed200304, 0x050d5201U);//
            Writel ((ULONG)0x20ed200340, 0x050d5201U);//
            Writel ((ULONG)0x20ed20037c, 0x050d5201U);//
            Writel ((ULONG)0x20ed2000e4, 0x08001029U);//
            Writel ((ULONG)0x20ed200120, 0x08001029U);//
            Writel ((ULONG)0x20ed20015c, 0x08001029U);//
            Writel ((ULONG)0x20ed200198, 0x08001029U);//
            Writel ((ULONG)0x20ed2001d4, 0x08001029U);//
            Writel ((ULONG)0x20ed200210, 0x08001029U);//
            Writel ((ULONG)0x20ed20024c, 0x08001029U);//
            Writel ((ULONG)0x20ed200288, 0x08001029U);//
            Writel ((ULONG)0x20ed2002c4, 0x08001029U);//
            Writel ((ULONG)0x20ed200300, 0x08001029U);//
            Writel ((ULONG)0x20ed20033c, 0x08001029U);//
            Writel ((ULONG)0x20ed200378, 0x08001029U);//
            Writel ((ULONG)0x20ed2000e4, 0x08001029U);//
            Writel ((ULONG)0x20ed200120, 0x08001029U);//
            Writel ((ULONG)0x20ed20015c, 0x08001029U);//
            Writel ((ULONG)0x20ed200198, 0x08001029U);//
            Writel ((ULONG)0x20ed2001d4, 0x08001029U);//
            Writel ((ULONG)0x20ed200210, 0x08001029U);//
            Writel ((ULONG)0x20ed20024c, 0x08001029U);//
            Writel ((ULONG)0x20ed200288, 0x08001029U);//
            Writel ((ULONG)0x20ed2002c4, 0x08001029U);//
            Writel ((ULONG)0x20ed200300, 0x08001029U);//
            Writel ((ULONG)0x20ed20033c, 0x08001029U);//
            Writel ((ULONG)0x20ed200378, 0x08001029U);//
            Writel ((ULONG)0x20ed2000e4, 0x08001029U);//
            Writel ((ULONG)0x20ed200120, 0x08001029U);//
            Writel ((ULONG)0x20ed20015c, 0x08001029U);//
            Writel ((ULONG)0x20ed200198, 0x08001029U);//
            Writel ((ULONG)0x20ed2001d4, 0x08001029U);//
            Writel ((ULONG)0x20ed200210, 0x08001029U);//
            Writel ((ULONG)0x20ed20024c, 0x08001029U);//
            Writel ((ULONG)0x20ed200288, 0x08001029U);//
            Writel ((ULONG)0x20ed2002c4, 0x08001029U);//
            Writel ((ULONG)0x20ed200300, 0x08001029U);//
            Writel ((ULONG)0x20ed20033c, 0x08001029U);//
            Writel ((ULONG)0x20ed200378, 0x08001029U);//
            Writel ((ULONG)0x20ed2000e4, 0x08001029U);//
            Writel ((ULONG)0x20ed200120, 0x08001029U);//
            Writel ((ULONG)0x20ed20015c, 0x08001029U);//
            Writel ((ULONG)0x20ed200198, 0x08001029U);//
            Writel ((ULONG)0x20ed2001d4, 0x08001029U);//
            Writel ((ULONG)0x20ed200210, 0x08001029U);//
            Writel ((ULONG)0x20ed20024c, 0x08001029U);//
            Writel ((ULONG)0x20ed200288, 0x08001029U);//
            Writel ((ULONG)0x20ed2002c4, 0x08001029U);//
            Writel ((ULONG)0x20ed200300, 0x08001029U);//
            Writel ((ULONG)0x20ed20033c, 0x08001029U);//
            Writel ((ULONG)0x20ed200378, 0x08001029U);//
            Writel ((ULONG)0x20ed2000e4, 0x08001029U);//
            Writel ((ULONG)0x20ed200120, 0x08001029U);//
            Writel ((ULONG)0x20ed20015c, 0x08001029U);//
            Writel ((ULONG)0x20ed200198, 0x08001029U);//
            Writel ((ULONG)0x20ed2001d4, 0x08001029U);//
            Writel ((ULONG)0x20ed200210, 0x08001029U);//
            Writel ((ULONG)0x20ed20024c, 0x08001029U);//
            Writel ((ULONG)0x20ed200288, 0x08001029U);//
            Writel ((ULONG)0x20ed2002c4, 0x08001029U);//
            Writel ((ULONG)0x20ed200300, 0x08001029U);//
            Writel ((ULONG)0x20ed20033c, 0x08001029U);//
            Writel ((ULONG)0x20ed200378, 0x08001029U);//

            Writel ((ULONG)0x20ed2000f0, 0x03000685U);////set datarate_sel to 2, 4608Mbps base
            Writel ((ULONG)0x20ed20012c, 0x03000685U);////set datarate_sel to 2, 4608Mbps base
            Writel ((ULONG)0x20ed200168, 0x03000685U);////set datarate_sel to 2, 4608Mbps base
            Writel ((ULONG)0x20ed2001a4, 0x03000685U);////set datarate_sel to 2, 4608Mbps base
            Writel ((ULONG)0x20ed2001e0, 0x03000685U);////set datarate_sel to 2, 4608Mbps base
            Writel ((ULONG)0x20ed20021c, 0x03000685U);////set datarate_sel to 2, 4608Mbps base
            Writel ((ULONG)0x20ed200258, 0x03000685U);////set datarate_sel to 2, 4608Mbps base
            Writel ((ULONG)0x20ed200294, 0x03000685U);////set datarate_sel to 2, 4608Mbps base
            Writel ((ULONG)0x20ed2002d0, 0x03000685U);////set datarate_sel to 2, 4608Mbps base
            Writel ((ULONG)0x20ed20030c, 0x03000685U);////set datarate_sel to 2, 4608Mbps base
            Writel ((ULONG)0x20ed200348, 0x03000685U);////set datarate_sel to 2, 4608Mbps base
            Writel ((ULONG)0x20ed200384, 0x03000685U);////set datarate_sel to 2, 4608Mbps base
            Writel ((ULONG)0x20ed200100, 0x00000352U);////no need to adjust frequency since 4608 is the default rate for gear 2
            Writel ((ULONG)0x20ed20013c, 0x00000352U);////no need to adjust frequency since 4608 is the default rate for gear 2
            Writel ((ULONG)0x20ed200178, 0x00000352U);////no need to adjust frequency since 4608 is the default rate for gear 2
            Writel ((ULONG)0x20ed2001b4, 0x00000352U);////no need to adjust frequency since 4608 is the default rate for gear 2
            Writel ((ULONG)0x20ed2001f0, 0x00000352U);////no need to adjust frequency since 4608 is the default rate for gear 2
            Writel ((ULONG)0x20ed20022c, 0x00000352U);////no need to adjust frequency since 4608 is the default rate for gear 2
            Writel ((ULONG)0x20ed200268, 0x00000352U);////no need to adjust frequency since 4608 is the default rate for gear 2
            Writel ((ULONG)0x20ed2002a4, 0x00000352U);////no need to adjust frequency since 4608 is the default rate for gear 2
            Writel ((ULONG)0x20ed2002e0, 0x00000352U);////no need to adjust frequency since 4608 is the default rate for gear 2
            Writel ((ULONG)0x20ed20031c, 0x00000352U);////no need to adjust frequency since 4608 is the default rate for gear 2
            Writel ((ULONG)0x20ed200358, 0x00000352U);////no need to adjust frequency since 4608 is the default rate for gear 2
            Writel ((ULONG)0x20ed200394, 0x00000352U);////no need to adjust frequency since 4608 is the default rate for gear 2
            Writel ((ULONG)0x20ed2000e8, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed200124, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed200160, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed20019c, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed2001d8, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed200214, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed200250, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed20028c, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed2002c8, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed200304, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed200340, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed20037c, 0x050d5201U);////voscalib_en=1
            Writel ((ULONG)0x20ed200474, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed200480, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed20048c, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed200498, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2004a4, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2004b0, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2004bc, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2004c8, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2004d4, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2004e0, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2004ec, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2004f8, 0x00000200U);////disable rxphy slvsec loopbk mode
            Writel ((ULONG)0x20ed2000ec, 0xc2412580U);////halt_dco
            Writel ((ULONG)0x20ed200128, 0xc2412580U);////halt_dco
            Writel ((ULONG)0x20ed200164, 0xc2412580U);////halt_dco
            Writel ((ULONG)0x20ed2001a0, 0xc2412580U);////halt_dco
            Writel ((ULONG)0x20ed2001dc, 0xc2412580U);////halt_dco
            Writel ((ULONG)0x20ed200218, 0xc2412580U);////halt_dco
            Writel ((ULONG)0x20ed200254, 0xc2412580U);////halt_dco
            Writel ((ULONG)0x20ed200290, 0xc2412580U);////halt_dco
            Writel ((ULONG)0x20ed2002cc, 0xc2412580U);////halt_dco
            Writel ((ULONG)0x20ed200308, 0xc2412580U);////halt_dco
            Writel ((ULONG)0x20ed200344, 0xc2412580U);////halt_dco
            Writel ((ULONG)0x20ed200380, 0xc2412580U);////halt_dco
            Writel ((ULONG)0x20ed2000e8, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed200124, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed200160, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed20019c, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed2001d8, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed200214, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed200250, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed20028c, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed2002c8, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed200304, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed200340, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            Writel ((ULONG)0x20ed20037c, 0x010d5201U);////ref_clk_sel_dco = 1'h0, 24MHz reference clock case
            ////# 10000
            Writel ((ULONG)0x20ed2000ec, 0xc2412180U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed200128, 0xc2412180U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed200164, 0xc2412180U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed2001a0, 0xc2412180U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed2001dc, 0xc2412180U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed200218, 0xc2412180U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed200254, 0xc2412180U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed200290, 0xc2412180U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed2002cc, 0xc2412180U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed200308, 0xc2412180U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed200344, 0xc2412180U);////halt_dco_rx[30] set to 1'h0 release DCO
            Writel ((ULONG)0x20ed200380, 0xc2412180U);////halt_dco_rx[30] set to 1'h0 release DCO

            /* Reset PHY */
            Writel ((ULONG)0x20ed1e0800U, 0x404dU);
        } else if (Stage == 1U) {
            Writel ((ULONG)0x20ed2000e4, 0x0800102dU);////obsv_calib = 1
            Writel ((ULONG)0x20ed200120, 0x0800102dU);////obsv_calib = 1
            Writel ((ULONG)0x20ed20015c, 0x0800102dU);////obsv_calib = 1
            Writel ((ULONG)0x20ed200198, 0x0800102dU);////obsv_calib = 1
            Writel ((ULONG)0x20ed2001d4, 0x0800102dU);////obsv_calib = 1
            Writel ((ULONG)0x20ed200210, 0x0800102dU);////obsv_calib = 1
            Writel ((ULONG)0x20ed20024c, 0x0800102dU);////obsv_calib = 1
            Writel ((ULONG)0x20ed200288, 0x0800102dU);////obsv_calib = 1
            Writel ((ULONG)0x20ed2002c4, 0x0800102dU);////obsv_calib = 1
            Writel ((ULONG)0x20ed200300, 0x0800102dU);////obsv_calib = 1
            Writel ((ULONG)0x20ed20033c, 0x0800102dU);////obsv_calib = 1
            Writel ((ULONG)0x20ed200378, 0x0800102dU);////obsv_calib = 1
            Writel ((ULONG)0x20ed2000f8, 0x809b4430U);////cdr_out_sel = 4
            Writel ((ULONG)0x20ed200134, 0x809b4430U);////cdr_out_sel = 4
            Writel ((ULONG)0x20ed200170, 0x809b4430U);////cdr_out_sel = 4
            Writel ((ULONG)0x20ed2001ac, 0x809b4430U);////cdr_out_sel = 4
            Writel ((ULONG)0x20ed2001e8, 0x809b4430U);////cdr_out_sel = 4
            Writel ((ULONG)0x20ed200224, 0x809b4430U);////cdr_out_sel = 4
            Writel ((ULONG)0x20ed200260, 0x809b4430U);////cdr_out_sel = 4
            Writel ((ULONG)0x20ed20029c, 0x809b4430U);////cdr_out_sel = 4
            Writel ((ULONG)0x20ed2002d8, 0x809b4430U);////cdr_out_sel = 4
            Writel ((ULONG)0x20ed200314, 0x809b4430U);////cdr_out_sel = 4
            Writel ((ULONG)0x20ed200350, 0x809b4430U);////cdr_out_sel = 4
            Writel ((ULONG)0x20ed20038c, 0x809b4430U);////cdr_out_sel = 4
            // Readl ((ULONG)0x20ed200088);//
            // Readl ((ULONG)0x20ed200090);//
            // Readl ((ULONG)0x20ed200098);//
            // Readl ((ULONG)0x20ed2000a0);//
            // Readl ((ULONG)0x20ed2000a8);//
            // Readl ((ULONG)0x20ed2000b0);//
            // Readl ((ULONG)0x20ed2000b8);//
            // Readl ((ULONG)0x20ed2000c0);//
            // Readl ((ULONG)0x20ed2000c8);//
            // Readl ((ULONG)0x20ed2000d0);//
            // Readl ((ULONG)0x20ed2000d8);//
            // Readl ((ULONG)0x20ed2000e0);//
            // Readl ((ULONG)0x20ed2003c0);// Check PHY status
            // Readl ((ULONG)0x20ed2003d0);// Check PHY status
            // Readl ((ULONG)0x20ed2003e0);// Check PHY status
            // Readl ((ULONG)0x20ed2003f0);// Check PHY status
            // Readl ((ULONG)0x20ed200400);// Check PHY status
            // Readl ((ULONG)0x20ed200410);// Check PHY status
            // Readl ((ULONG)0x20ed200420);// Check PHY status
            // Readl ((ULONG)0x20ed200430);// Check PHY status
            // Readl ((ULONG)0x20ed200440);// Check PHY status
            // Readl ((ULONG)0x20ed200450);// Check PHY status
            // Readl ((ULONG)0x20ed200460);// Check PHY status
            // Readl ((ULONG)0x20ed200470);// Check PHY status

            ////== Offset calibration
            Writel ((ULONG)0x20ed200114, 0x10000000U);////lpgain of calibration, set to 0
            Writel ((ULONG)0x20ed200150, 0x10000000U);////lpgain of calibration, set to 0
            Writel ((ULONG)0x20ed20018c, 0x10000000U);////lpgain of calibration, set to 0
            Writel ((ULONG)0x20ed2001c8, 0x10000000U);////lpgain of calibration, set to 0
            Writel ((ULONG)0x20ed200204, 0x10000000U);////lpgain of calibration, set to 0
            Writel ((ULONG)0x20ed200240, 0x10000000U);////lpgain of calibration, set to 0
            Writel ((ULONG)0x20ed20027c, 0x10000000U);////lpgain of calibration, set to 0
            Writel ((ULONG)0x20ed2002b8, 0x10000000U);////lpgain of calibration, set to 0
            Writel ((ULONG)0x20ed2002f4, 0x10000000U);////lpgain of calibration, set to 0
            Writel ((ULONG)0x20ed200330, 0x10000000U);////lpgain of calibration, set to 0
            Writel ((ULONG)0x20ed20036c, 0x10000000U);////lpgain of calibration, set to 0
            Writel ((ULONG)0x20ed2003a8, 0x10000000U);////lpgain of calibration, set to 0
            ////sel_calib       .<0>    .<3'h4> .<//sel_calib = 4, calibre vga offset>
            ////start_calib     .<0>    .<1'h1> .<//start_calib = 1>
            ////;wait 200.ms
            ////start_calib     .<0>    .<1'h0> .<//start_calib = 0>
            ////;wait 20.ms
            ////;
            ////ctle_func_en_in .<0>    .<2'h3> .<//ctle_func_en_in = 3 >
            ////sel_calib       .<0>    .<3'h5> .<//sel_calib = 6, calibre cstle2 offset>
            ////start_calib     .<0>    .<1'h1> .<//start_calib = 1>
            ////;wait 200.ms
            ////start_calib     .<0>    .<1'h0> .<//start_calib = 0>
            ////;wait 20.ms
            ////;
            ////sel_calib       .<0>    .<3'h6> .<//sel_calib = 5, calibre ctle1 offset>
            ////start_calib     .<0>    .<1'h1> .<//start_calib = 1>
            ////;wait 200.ms
            ////start_calib     .<0>    .<1'h0> .<//start_calib = 0>
            ////;wait 20.ms
            ////;
            ////sel_calib       .<0>    .<3'h7> .<//sel_calib = 7, calibre slicer offset>
            ////start_calib     .<0>    .<1'h1> .<//start_calib = 1>
            ////;wait 200.ms
            ////start_calib     .<0>    .<1'h0> .<//start_calib = 0>
            ////;wait 20.ms

            Writel ((ULONG)0x20ed2000e8, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed200124, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed200160, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed20019c, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed2001d8, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed200214, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed200250, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed20028c, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed2002c8, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed200304, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed200340, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed20037c, 0x010d4201U);//voscalib_en=0
            Writel ((ULONG)0x20ed2000ec, 0xc2012180U);////pd_sf = 0
            Writel ((ULONG)0x20ed200128, 0xc2012180U);////pd_sf = 0
            Writel ((ULONG)0x20ed200164, 0xc2012180U);////pd_sf = 0
            Writel ((ULONG)0x20ed2001a0, 0xc2012180U);////pd_sf = 0
            Writel ((ULONG)0x20ed2001dc, 0xc2012180U);////pd_sf = 0
            Writel ((ULONG)0x20ed200218, 0xc2012180U);////pd_sf = 0
            Writel ((ULONG)0x20ed200254, 0xc2012180U);////pd_sf = 0
            Writel ((ULONG)0x20ed200290, 0xc2012180U);////pd_sf = 0
            Writel ((ULONG)0x20ed2002cc, 0xc2012180U);////pd_sf = 0
            Writel ((ULONG)0x20ed200308, 0xc2012180U);////pd_sf = 0
            Writel ((ULONG)0x20ed200344, 0xc2012180U);////pd_sf = 0
            Writel ((ULONG)0x20ed200380, 0xc2012180U);////pd_sf = 0
            Writel ((ULONG)0x20ed2000e8, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed200124, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed200160, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed20019c, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed2001d8, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed200214, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed200250, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed20028c, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed2002c8, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed200304, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed200340, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed20037c, 0x010d4a01U);////disable eye momnitor path
            Writel ((ULONG)0x20ed2000e4, 0x0800142dU);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed200120, 0x0800142dU);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed20015c, 0x0800142dU);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed200198, 0x0800142dU);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed2001d4, 0x0800142dU);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed200210, 0x0800142dU);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed20024c, 0x0800142dU);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed200288, 0x0800142dU);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed2002c4, 0x0800142dU);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed200300, 0x0800142dU);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed20033c, 0x0800142dU);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed200378, 0x0800142dU);////rctSerdesTermResEn = 1
            Writel ((ULONG)0x20ed2000ec, 0xc2013180U);////power down calibration circuit
            Writel ((ULONG)0x20ed200128, 0xc2013180U);////power down calibration circuit
            Writel ((ULONG)0x20ed200164, 0xc2013180U);////power down calibration circuit
            Writel ((ULONG)0x20ed2001a0, 0xc2013180U);////power down calibration circuit
            Writel ((ULONG)0x20ed2001dc, 0xc2013180U);////power down calibration circuit
            Writel ((ULONG)0x20ed200218, 0xc2013180U);////power down calibration circuit
            Writel ((ULONG)0x20ed200254, 0xc2013180U);////power down calibration circuit
            Writel ((ULONG)0x20ed200290, 0xc2013180U);////power down calibration circuit
            Writel ((ULONG)0x20ed2002cc, 0xc2013180U);////power down calibration circuit
            Writel ((ULONG)0x20ed200308, 0xc2013180U);////power down calibration circuit
            Writel ((ULONG)0x20ed200344, 0xc2013180U);////power down calibration circuit
            Writel ((ULONG)0x20ed200380, 0xc2013180U);////power down calibration circuit
            ////sq_mode         .<0>    .<1'h1> .<//sq_mode = 1 >
            Writel ((ULONG)0x20ed2000ec, 0xc2013180U);////calibdone = 1, disalbe dcc detecotor
            Writel ((ULONG)0x20ed200128, 0xc2013180U);////calibdone = 1, disalbe dcc detecotor
            Writel ((ULONG)0x20ed200164, 0xc2013180U);////calibdone = 1, disalbe dcc detecotor
            Writel ((ULONG)0x20ed2001a0, 0xc2013180U);////calibdone = 1, disalbe dcc detecotor
            Writel ((ULONG)0x20ed2001dc, 0xc2013180U);////calibdone = 1, disalbe dcc detecotor
            Writel ((ULONG)0x20ed200218, 0xc2013180U);////calibdone = 1, disalbe dcc detecotor
            Writel ((ULONG)0x20ed200254, 0xc2013180U);////calibdone = 1, disalbe dcc detecotor
            Writel ((ULONG)0x20ed200290, 0xc2013180U);////calibdone = 1, disalbe dcc detecotor
            Writel ((ULONG)0x20ed2002cc, 0xc2013180U);////calibdone = 1, disalbe dcc detecotor
            Writel ((ULONG)0x20ed200308, 0xc2013180U);////calibdone = 1, disalbe dcc detecotor
            Writel ((ULONG)0x20ed200344, 0xc2013180U);////calibdone = 1, disalbe dcc detecotor
            Writel ((ULONG)0x20ed200380, 0xc2013180U);////calibdone = 1, disalbe dcc detecotor
            // Readl ((ULONG)0x20ed2003c0);// Check PHY status
            // Readl ((ULONG)0x20ed2003d0);// Check PHY status
            // Readl ((ULONG)0x20ed2003e0);// Check PHY status
            // Readl ((ULONG)0x20ed2003f0);// Check PHY status
            // Readl ((ULONG)0x20ed200400);// Check PHY status
            // Readl ((ULONG)0x20ed200410);// Check PHY status
            // Readl ((ULONG)0x20ed200420);// Check PHY status
            // Readl ((ULONG)0x20ed200430);// Check PHY status
            // Readl ((ULONG)0x20ed200440);// Check PHY status
            // Readl ((ULONG)0x20ed200450);// Check PHY status
            // Readl ((ULONG)0x20ed200460);// Check PHY status
            // Readl ((ULONG)0x20ed200470);// Check PHY status

        } else if (Stage == 2U) {
            Writel ((ULONG)0x20ed2000e8, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed200124, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed200160, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed20019c, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed2001d8, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed200214, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed200250, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed20028c, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed2002c8, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed200304, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed200340, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            Writel ((ULONG)0x20ed20037c, 0x000d4a01U);////pause_lf_dco_rx[17] set to 1'h0
            //DelayMs(20);
            DelayMs(1);
            Writel ((ULONG)0x20ed2000f0, 0x03001685U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed20012c, 0x03001685U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed200168, 0x03001685U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed2001a4, 0x03001685U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed2001e0, 0x03001685U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed20021c, 0x03001685U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed200258, 0x03001685U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed200294, 0x03001685U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed2002d0, 0x03001685U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed20030c, 0x03001685U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed200348, 0x03001685U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed200384, 0x03001685U);////enable vga SSLMS loop
            Writel ((ULONG)0x20ed2000ec, 0xc0013180U);////disable vga rpause
            Writel ((ULONG)0x20ed200128, 0xc0013180U);////disable vga rpause
            Writel ((ULONG)0x20ed200164, 0xc0013180U);////disable vga rpause
            Writel ((ULONG)0x20ed2001a0, 0xc0013180U);////disable vga rpause
            Writel ((ULONG)0x20ed2001dc, 0xc0013180U);////disable vga rpause
            Writel ((ULONG)0x20ed200218, 0xc0013180U);////disable vga rpause
            Writel ((ULONG)0x20ed200254, 0xc0013180U);////disable vga rpause
            Writel ((ULONG)0x20ed200290, 0xc0013180U);////disable vga rpause
            Writel ((ULONG)0x20ed2002cc, 0xc0013180U);////disable vga rpause
            Writel ((ULONG)0x20ed200308, 0xc0013180U);////disable vga rpause
            Writel ((ULONG)0x20ed200344, 0xc0013180U);////disable vga rpause
            Writel ((ULONG)0x20ed200380, 0xc0013180U);////disable vga rpause
            //DelayMs(20);
            DelayMs(1);
            Writel ((ULONG)0x20ed2000ec, 0xc2013180U);////enable vga rpause
            Writel ((ULONG)0x20ed200128, 0xc2013180U);////enable vga rpause
            Writel ((ULONG)0x20ed200164, 0xc2013180U);////enable vga rpause
            Writel ((ULONG)0x20ed2001a0, 0xc2013180U);////enable vga rpause
            Writel ((ULONG)0x20ed2001dc, 0xc2013180U);////enable vga rpause
            Writel ((ULONG)0x20ed200218, 0xc2013180U);////enable vga rpause
            Writel ((ULONG)0x20ed200254, 0xc2013180U);////enable vga rpause
            Writel ((ULONG)0x20ed200290, 0xc2013180U);////enable vga rpause
            Writel ((ULONG)0x20ed2002cc, 0xc2013180U);////enable vga rpause
            Writel ((ULONG)0x20ed200308, 0xc2013180U);////enable vga rpause
            Writel ((ULONG)0x20ed200344, 0xc2013180U);////enable vga rpause
            Writel ((ULONG)0x20ed200380, 0xc2013180U);////enable vga rpause
            Writel ((ULONG)0x20ed2000f0, 0x03002685U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed20012c, 0x03002685U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed200168, 0x03002685U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed2001a4, 0x03002685U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed2001e0, 0x03002685U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed20021c, 0x03002685U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed200258, 0x03002685U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed200294, 0x03002685U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed2002d0, 0x03002685U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed20030c, 0x03002685U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed200348, 0x03002685U);////enable vref SSLMS loop
            Writel ((ULONG)0x20ed200384, 0x03002685U);////enable vref SSLMS loop
            //DelayMs(40);
            DelayMs(2);
            Writel ((ULONG)0x20ed2000f0, 0x03003685U);////enable ctle SSLMS loop
            Writel ((ULONG)0x20ed20012c, 0x03003685U);////enable ctle SSLMS loop
            Writel ((ULONG)0x20ed200168, 0x03003685U);////enable ctle SSLMS loop
            Writel ((ULONG)0x20ed2001a4, 0x03003685U);////enable ctle SSLMS loop
            Writel ((ULONG)0x20ed2001e0, 0x03003685U);////enable ctle SSLMS loop
            Writel ((ULONG)0x20ed20021c, 0x03003685U);////enable ctle SSLMS loop
            Writel ((ULONG)0x20ed200258, 0x03003685U);////enable ctle SSLMS loop
            Writel ((ULONG)0x20ed200294, 0x03003685U);////enable ctle SSLMS loop
            Writel ((ULONG)0x20ed2002d0, 0x03003685U);////enable ctle SSLMS loop
            Writel ((ULONG)0x20ed20030c, 0x03003685U);////enable ctle SSLMS loop
            Writel ((ULONG)0x20ed200348, 0x03003685U);////enable ctle SSLMS loop
            Writel ((ULONG)0x20ed200384, 0x03003685U);////enable ctle SSLMS loop
            Writel ((ULONG)0x20ed2000ec, 0xc2013100U);////ctle rpause = 0
            Writel ((ULONG)0x20ed200128, 0xc2013100U);////ctle rpause = 0
            Writel ((ULONG)0x20ed200164, 0xc2013100U);////ctle rpause = 0
            Writel ((ULONG)0x20ed2001a0, 0xc2013100U);////ctle rpause = 0
            Writel ((ULONG)0x20ed2001dc, 0xc2013100U);////ctle rpause = 0
            Writel ((ULONG)0x20ed200218, 0xc2013100U);////ctle rpause = 0
            Writel ((ULONG)0x20ed200254, 0xc2013100U);////ctle rpause = 0
            Writel ((ULONG)0x20ed200290, 0xc2013100U);////ctle rpause = 0
            Writel ((ULONG)0x20ed2002cc, 0xc2013100U);////ctle rpause = 0
            Writel ((ULONG)0x20ed200308, 0xc2013100U);////ctle rpause = 0
            Writel ((ULONG)0x20ed200344, 0xc2013100U);////ctle rpause = 0
            Writel ((ULONG)0x20ed200380, 0xc2013100U);////ctle rpause = 0
            //DelayMs(20);
            DelayMs(1);
            Writel ((ULONG)0x20ed2000ec, 0xc2013180U);////ctle rpause = 1
            Writel ((ULONG)0x20ed200128, 0xc2013180U);////ctle rpause = 1
            Writel ((ULONG)0x20ed200164, 0xc2013180U);////ctle rpause = 1
            Writel ((ULONG)0x20ed2001a0, 0xc2013180U);////ctle rpause = 1
            Writel ((ULONG)0x20ed2001dc, 0xc2013180U);////ctle rpause = 1
            Writel ((ULONG)0x20ed200218, 0xc2013180U);////ctle rpause = 1
            Writel ((ULONG)0x20ed200254, 0xc2013180U);////ctle rpause = 1
            Writel ((ULONG)0x20ed200290, 0xc2013180U);////ctle rpause = 1
            Writel ((ULONG)0x20ed2002cc, 0xc2013180U);////ctle rpause = 1
            Writel ((ULONG)0x20ed200308, 0xc2013180U);////ctle rpause = 1
            Writel ((ULONG)0x20ed200344, 0xc2013180U);////ctle rpause = 1
            Writel ((ULONG)0x20ed200380, 0xc2013180U);////ctle rpause = 1
            Writel ((ULONG)0x20ed2000ec, 0xc2013080U);////ctle cpause = 0
            Writel ((ULONG)0x20ed200128, 0xc2013080U);////ctle cpause = 0
            Writel ((ULONG)0x20ed200164, 0xc2013080U);////ctle cpause = 0
            Writel ((ULONG)0x20ed2001a0, 0xc2013080U);////ctle cpause = 0
            Writel ((ULONG)0x20ed2001dc, 0xc2013080U);////ctle cpause = 0
            Writel ((ULONG)0x20ed200218, 0xc2013080U);////ctle cpause = 0
            Writel ((ULONG)0x20ed200254, 0xc2013080U);////ctle cpause = 0
            Writel ((ULONG)0x20ed200290, 0xc2013080U);////ctle cpause = 0
            Writel ((ULONG)0x20ed2002cc, 0xc2013080U);////ctle cpause = 0
            Writel ((ULONG)0x20ed200308, 0xc2013080U);////ctle cpause = 0
            Writel ((ULONG)0x20ed200344, 0xc2013080U);////ctle cpause = 0
            Writel ((ULONG)0x20ed200380, 0xc2013080U);////ctle cpause = 0
            //DelayMs(20);
            DelayMs(1);
            Writel ((ULONG)0x20ed2000ec, 0xc2013180U);////ctle cpause = 1
            Writel ((ULONG)0x20ed200128, 0xc2013180U);////ctle cpause = 1
            Writel ((ULONG)0x20ed200164, 0xc2013180U);////ctle cpause = 1
            Writel ((ULONG)0x20ed2001a0, 0xc2013180U);////ctle cpause = 1
            Writel ((ULONG)0x20ed2001dc, 0xc2013180U);////ctle cpause = 1
            Writel ((ULONG)0x20ed200218, 0xc2013180U);////ctle cpause = 1
            Writel ((ULONG)0x20ed200254, 0xc2013180U);////ctle cpause = 1
            Writel ((ULONG)0x20ed200290, 0xc2013180U);////ctle cpause = 1
            Writel ((ULONG)0x20ed2002cc, 0xc2013180U);////ctle cpause = 1
            Writel ((ULONG)0x20ed200308, 0xc2013180U);////ctle cpause = 1
            Writel ((ULONG)0x20ed200344, 0xc2013180U);////ctle cpause = 1
            Writel ((ULONG)0x20ed200380, 0xc2013180U);////ctle cpause = 1
            Writel ((ULONG)0x20ed2000f0, 0x03004685U);////enable dfe SSLMS loop
            Writel ((ULONG)0x20ed20012c, 0x03004685U);////enable dfe SSLMS loop
            Writel ((ULONG)0x20ed200168, 0x03004685U);////enable dfe SSLMS loop
            Writel ((ULONG)0x20ed2001a4, 0x03004685U);////enable dfe SSLMS loop
            Writel ((ULONG)0x20ed2001e0, 0x03004685U);////enable dfe SSLMS loop
            Writel ((ULONG)0x20ed20021c, 0x03004685U);////enable dfe SSLMS loop
            Writel ((ULONG)0x20ed200258, 0x03004685U);////enable dfe SSLMS loop
            Writel ((ULONG)0x20ed200294, 0x03004685U);////enable dfe SSLMS loop
            Writel ((ULONG)0x20ed2002d0, 0x03004685U);////enable dfe SSLMS loop
            Writel ((ULONG)0x20ed20030c, 0x03004685U);////enable dfe SSLMS loop
            Writel ((ULONG)0x20ed200348, 0x03004685U);////enable dfe SSLMS loop
            Writel ((ULONG)0x20ed200384, 0x03004685U);////enable dfe SSLMS loop
            //DelayMs(60);
            DelayMs(3);
            Writel ((ULONG)0x20ed2000ec, 0xc2013188U);////enable dfe pause
            Writel ((ULONG)0x20ed200128, 0xc2013188U);////enable dfe pause
            Writel ((ULONG)0x20ed200164, 0xc2013188U);////enable dfe pause
            Writel ((ULONG)0x20ed2001a0, 0xc2013188U);////enable dfe pause
            Writel ((ULONG)0x20ed2001dc, 0xc2013188U);////enable dfe pause
            Writel ((ULONG)0x20ed200218, 0xc2013188U);////enable dfe pause
            Writel ((ULONG)0x20ed200254, 0xc2013188U);////enable dfe pause
            Writel ((ULONG)0x20ed200290, 0xc2013188U);////enable dfe pause
            Writel ((ULONG)0x20ed2002cc, 0xc2013188U);////enable dfe pause
            Writel ((ULONG)0x20ed200308, 0xc2013188U);////enable dfe pause
            Writel ((ULONG)0x20ed200344, 0xc2013188U);////enable dfe pause
            Writel ((ULONG)0x20ed200380, 0xc2013188U);////enable dfe pause
            Writel ((ULONG)0x20ed2000ec, 0xc20131a8U);////enable vref_pause
            Writel ((ULONG)0x20ed200128, 0xc20131a8U);////enable vref_pause
            Writel ((ULONG)0x20ed200164, 0xc20131a8U);////enable vref_pause
            Writel ((ULONG)0x20ed2001a0, 0xc20131a8U);////enable vref_pause
            Writel ((ULONG)0x20ed2001dc, 0xc20131a8U);////enable vref_pause
            Writel ((ULONG)0x20ed200218, 0xc20131a8U);////enable vref_pause
            Writel ((ULONG)0x20ed200254, 0xc20131a8U);////enable vref_pause
            Writel ((ULONG)0x20ed200290, 0xc20131a8U);////enable vref_pause
            Writel ((ULONG)0x20ed2002cc, 0xc20131a8U);////enable vref_pause
            Writel ((ULONG)0x20ed200308, 0xc20131a8U);////enable vref_pause
            Writel ((ULONG)0x20ed200344, 0xc20131a8U);////enable vref_pause
            Writel ((ULONG)0x20ed200380, 0xc20131a8U);////enable vref_pause
            DelayMs(1);

            /* Setup PHY */
            Writel ((ULONG)0x20ed1e0800U, 0x408dU);
        } else if (Stage == 3U) {
            Writel ((ULONG)0x20ed1e0800U, 0x402dU);
        } else {
            /* avoid misrac parser error */
        }
    }

    return RetVal;
}

UINT32 AmbaCSL_VinGetSlvsEcPhyRxState(UINT32 RxPortNum)
{
    return (Readl((ULONG)0x20ed2003c0U + ((ULONG)RxPortNum * 16U)) >> 4U) & 0xfU;
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
            RetVal = VIN_ERR_UNEXPECTED;
        }
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinMipiConfigGlobal - Configure VIN global configuration for MIPI
 *  @param[in] VinID Indicate VIN channel
 *  @return error code
 */
UINT32 AmbaCSL_VinMipiConfigGlobal(UINT32 VinID)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s *pVinGlobalCfg = NULL;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinGlobalCfg = pGlobalConfigData[VinID];
        if (AmbaWrap_memset(pVinGlobalCfg, 0, sizeof(AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s)) == ERR_NONE) {

            pVinGlobalCfg->SensorType    = 3U;
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

/**
 *  AmbaCSL_VinMipiCphyConfigGlobal - Configure VIN global configuration for MIPI
 *  @param[in] VinID Indicate VIN channel
 *  @return error code
 */
UINT32 AmbaCSL_VinMipiCphyConfigGlobal(UINT32 VinID)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s *pVinGlobalCfg = NULL;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinGlobalCfg = pGlobalConfigData[VinID];
        if (AmbaWrap_memset(pVinGlobalCfg, 0, sizeof(AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s)) == ERR_NONE) {

            pVinGlobalCfg->SensorType    = 6U;
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

/**
 *  AmbaCSL_VinSlvsEcConfigGlobal - Configure VIN global configuration for SLVS-EC
 *  @param[in] VinID Indicate VIN channel
 *  @return error code
 */
UINT32 AmbaCSL_VinSlvsEcConfigGlobal(UINT32 VinID)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s *pVinGlobalCfg = NULL;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinGlobalCfg = pGlobalConfigData[VinID];
        if (AmbaWrap_memset(pVinGlobalCfg, 0, sizeof(AMBA_CSL_VIN_GLOBAL_CONFIG_DATA_s)) == ERR_NONE) {

            pVinGlobalCfg->SensorType     = 5U;
            pVinGlobalCfg->SlvsClkMode    = 1U;
            pVinGlobalCfg->AFIFOBypass    = 0U;
            pVinGlobalCfg->SlvsEcClkMode  = 1U; /* needs to be set to 1 to use recovered symbol clock after mphy_rx_state=0xa */
            pVinGlobalCfg->SlvsEcPhyPulseDuration = 16U;

            // DebugBusConfigVinGlobal(VinID);
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
        pVinCfg->IntrDelayH_BlackLevelCtrl.IntrDelayH = 0U;
    } else {
        pVinCfg->IntrDelayL = (UINT16)(DelayedVsync & 0xffffU);
        pVinCfg->IntrDelayH_BlackLevelCtrl.IntrDelayH = (UINT16)(DelayedVsync >> 16U);
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
    pVinCfg->SlvsEovPattern = CustomSyncCode->PatternEov;

    pVinCfg->SlvsCtrl.PatMaskAlignment = CustomSyncCode->PatternAlign;
    pVinCfg->SlvsSyncCodeDetect_Watchdog.DetectSol = CustomSyncCode->SolDetectEnable;
    pVinCfg->SlvsSyncCodeDetect_Watchdog.DetectEol = CustomSyncCode->EolDetectEnable;
    pVinCfg->SlvsSyncCodeDetect_Watchdog.DetectSof = CustomSyncCode->SofDetectEnable;
    pVinCfg->SlvsSyncCodeDetect_Watchdog.DetectEof = CustomSyncCode->EofDetectEnable;
    pVinCfg->SlvsSyncCodeDetect_Watchdog.DetectSov = CustomSyncCode->SovDetectEnable;
    pVinCfg->SlvsSyncCodeDetect_Watchdog.DetectEov = CustomSyncCode->EovDetectEnable;
}

static void VinITU656SyncCodeConfig(AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg, UINT32 Interlace)
{
    pVinCfg->SlvsCtrl.SyncType = 1U;

    pVinCfg->SlvsSyncDetectMask = 0x8000U;
    pVinCfg->SlvsSyncDetectPattern = 0x8000U;

    pVinCfg->SlvsSyncCodeDetect_Watchdog.DetectSol    = 1U;
    pVinCfg->SlvsSyncCodeDetect_Watchdog.DetectEol    = 0U;
    pVinCfg->SlvsSyncCodeDetect_Watchdog.DetectSof    = 0U;
    pVinCfg->SlvsSyncCodeDetect_Watchdog.DetectEof    = 0U;
    pVinCfg->SlvsSyncCodeDetect_Watchdog.DetectSov    = 1U;
    pVinCfg->SlvsSyncCodeDetect_Watchdog.DetectEov    = 0U;

    pVinCfg->SlvsCtrl.PatMaskAlignment = 1U;

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
            pVinCfg->DataPinMux0_1.DataLane0PinSelect = DataLanePinSelect[0];
            pVinCfg->DataPinMux0_1.DataLane1PinSelect = DataLanePinSelect[1];
            pVinCfg->DataPinMux0_1.DataLane2PinSelect = DataLanePinSelect[2];
            pVinCfg->DataPinMux0_1.DataLane3PinSelect = DataLanePinSelect[3];
            pVinCfg->DataPinMux0_1.DataLane4PinSelect = DataLanePinSelect[4];
            pVinCfg->DataPinMux0_1.DataLane5PinSelect = DataLanePinSelect[5];
            pVinCfg->DataPinMux0_1.DataLane6PinSelect = DataLanePinSelect[6];
            pVinCfg->DataPinMux0_1.DataLane7PinSelect = DataLanePinSelect[7];
            pVinCfg->DataPinMux2_3.DataLane8PinSelect = 8U;
            pVinCfg->DataPinMux2_3.DataLane9PinSelect = 9U;
            pVinCfg->DataPinMux2_3.DataLane10PinSelect = 10U;
            pVinCfg->DataPinMux2_3.DataLane11PinSelect = 11U;
            pVinCfg->DataPinMux2_3.DataLane12PinSelect = 12U;
            pVinCfg->DataPinMux2_3.DataLane13PinSelect = 13U;
            pVinCfg->DataPinMux2_3.DataLane14PinSelect = 14U;
            pVinCfg->DataPinMux2_3.DataLane15PinSelect = 15U;

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

            pVinCfg->SlvsCtrl.SyncInterleaving = pVinSlvsConfig->SyncDetectCtrl.SyncInterleaving;
            pVinCfg->SlvsCtrl.SyncTolerance = 1U;
            pVinCfg->SlvsCtrl.SyncAllLanes = 0U;

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
    const UINT8 DcphyLanePinSel[4U][8U] = {
        {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U},   /* NumActiveLanes=1 */
        {0U, 2U, 1U, 3U, 4U, 5U, 6U, 7U},   /* NumActiveLanes=2 */
        {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},   /* NumActiveLanes=3(not support) */
        {0U, 2U, 4U, 6U, 1U, 3U, 5U, 7U},   /* NumActiveLanes=4 */
    };
    const UINT8 *pLanePinSel = &DcphyLanePinSel[pVinMipiConfig->NumActiveLanes - 1U][0];
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
            if (VinID <= AMBA_VIN_CHANNEL7) { /* DCPHY */
                /* lane enable = 2x lane-num */
                pVinCfg->Ctrl.DataLaneEnable = (UINT16)(((UINT32)1U << (pVinMipiConfig->NumActiveLanes << 1U)) - 1U);
            } else {
                pVinCfg->Ctrl.DataLaneEnable = (UINT16)(((UINT32)1U << pVinMipiConfig->NumActiveLanes) - 1U);
            }
            /* DVP ctrl */
            if (pVinMipiConfig->Config.ColorSpace == AMBA_VIN_COLOR_SPACE_YUV) {
                pVinCfg->DvpCtrl_SyncPinSelect.YuvEnable   = 1U;
                pVinCfg->DvpCtrl_SyncPinSelect.YuvOrder    = pVinMipiConfig->Config.YuvOrder;
            } else {
                pVinCfg->DvpCtrl_SyncPinSelect.YuvEnable   = 0U;
            }

            /* MIPI control */
            pVinCfg->MipiCtrl0_1.VirtChanMask     = 0x3U;
            pVinCfg->MipiCtrl0_1.VirtChanPattern  = 0x0U;
            pVinCfg->MipiCtrl0_1.DataTypeMask     = pVinMipiConfig->DataTypeMask;
            pVinCfg->MipiCtrl0_1.DataTypePattern  = pVinMipiConfig->DataType;
            pVinCfg->MipiCtrl0_1.ByteSwapEnable   = 0U;
            pVinCfg->MipiCtrl0_1.EccEnable        = 1U;
            pVinCfg->MipiCtrl0_1.ForwardEccEnable = 1U;
            pVinCfg->MipiCtrl2_SlvsEcCtrl0.GclkSoVinPassThr = 0U;
            pVinCfg->MipiCtrl2_SlvsEcCtrl0.EccVCxOverride = 1U;
            pVinCfg->MipiCtrl2_SlvsEcCtrl0.VCxMask = 0x7U;
            pVinCfg->MipiCtrl2_SlvsEcCtrl0.VCxPattern = 0x0U;

            if (VinID <= AMBA_VIN_CHANNEL7) { /* DCPHY */
                pVinCfg->DataPinMux0_1.DataLane0PinSelect = pLanePinSel[0];
                pVinCfg->DataPinMux0_1.DataLane1PinSelect = pLanePinSel[1];
                pVinCfg->DataPinMux0_1.DataLane2PinSelect = pLanePinSel[2];
                pVinCfg->DataPinMux0_1.DataLane3PinSelect = pLanePinSel[3];
                pVinCfg->DataPinMux0_1.DataLane4PinSelect = pLanePinSel[4];
                pVinCfg->DataPinMux0_1.DataLane5PinSelect = pLanePinSel[5];
                pVinCfg->DataPinMux0_1.DataLane6PinSelect = pLanePinSel[6];
                pVinCfg->DataPinMux0_1.DataLane7PinSelect = pLanePinSel[7];
            } else {
                pVinCfg->DataPinMux0_1.DataLane0PinSelect = 0U;
                pVinCfg->DataPinMux0_1.DataLane1PinSelect = 1U;
                pVinCfg->DataPinMux0_1.DataLane2PinSelect = 2U;
                pVinCfg->DataPinMux0_1.DataLane3PinSelect = 3U;
                pVinCfg->DataPinMux0_1.DataLane4PinSelect = 4U;
                pVinCfg->DataPinMux0_1.DataLane5PinSelect = 5U;
                pVinCfg->DataPinMux0_1.DataLane6PinSelect = 6U;
                pVinCfg->DataPinMux0_1.DataLane7PinSelect = 7U;
            }
            pVinCfg->DataPinMux2_3.DataLane8PinSelect = 8U;
            pVinCfg->DataPinMux2_3.DataLane9PinSelect = 9U;
            pVinCfg->DataPinMux2_3.DataLane10PinSelect = 10U;
            pVinCfg->DataPinMux2_3.DataLane11PinSelect = 11U;
            pVinCfg->DataPinMux2_3.DataLane12PinSelect = 12U;
            pVinCfg->DataPinMux2_3.DataLane13PinSelect = 13U;
            pVinCfg->DataPinMux2_3.DataLane14PinSelect = 14U;
            pVinCfg->DataPinMux2_3.DataLane15PinSelect = 15U;

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
 *  AmbaCSL_VinMipiCphyConfigMain - Configure VIN main configuration for MIPI
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinMipiConfig Pointer to SLVS configuration *  @return error code
 */
UINT32 AmbaCSL_VinMipiCphyConfigMain(UINT32 VinID, const AMBA_VIN_MIPI_CONFIG_s *pVinMipiCphyConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg = NULL;
    AMBA_CSL_VIN_INFO_s *pVinInfo = NULL;
    /* Vin internal sensor clock rate = sensor output clock rate / 4 */
    UINT32 DelayedVsync = pVinMipiCphyConfig->Config.DelayedVsync / 4U;

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
            pVinCfg->Ctrl.BitsPerPixel = (UINT8)((pVinMipiCphyConfig->Config.NumDataBits - 8U) >> 1U);
            /* lane enable = 2x lane-num */
            pVinCfg->Ctrl.DataLaneEnable = (UINT16)(((UINT32)1U << (pVinMipiCphyConfig->NumActiveLanes << 1U)) - 1U);

            /* DVP ctrl */
            if (pVinMipiCphyConfig->Config.ColorSpace == AMBA_VIN_COLOR_SPACE_YUV) {
                pVinCfg->DvpCtrl_SyncPinSelect.YuvEnable   = 1U;
                pVinCfg->DvpCtrl_SyncPinSelect.YuvOrder    = pVinMipiCphyConfig->Config.YuvOrder;
            } else {
                pVinCfg->DvpCtrl_SyncPinSelect.YuvEnable   = 0U;
            }

            /* MIPI control */
            pVinCfg->MipiCtrl0_1.VirtChanMask     = 0x3U;
            pVinCfg->MipiCtrl0_1.VirtChanPattern  = 0x0U;
            pVinCfg->MipiCtrl0_1.DataTypeMask     = pVinMipiCphyConfig->DataTypeMask;
            pVinCfg->MipiCtrl0_1.DataTypePattern  = pVinMipiCphyConfig->DataType;
            pVinCfg->MipiCtrl0_1.ByteSwapEnable   = 0U;
            pVinCfg->MipiCtrl0_1.EccEnable        = 1U;
            pVinCfg->MipiCtrl0_1.ForwardEccEnable = 1U;
            pVinCfg->MipiCtrl2_SlvsEcCtrl0.GclkSoVinPassThr = 0U;   /* Only used while C-PHY is working */
            pVinCfg->MipiCtrl2_SlvsEcCtrl0.EccVCxOverride = 1U; /* csi2 1.0 */
            pVinCfg->MipiCtrl2_SlvsEcCtrl0.VCxMask = 0x7U;
            pVinCfg->MipiCtrl2_SlvsEcCtrl0.VCxPattern = 0x0U;

            pVinCfg->DataPinMux0_1.DataLane0PinSelect = 0U;
            pVinCfg->DataPinMux0_1.DataLane1PinSelect = 1U;
            pVinCfg->DataPinMux0_1.DataLane2PinSelect = 2U;
            pVinCfg->DataPinMux0_1.DataLane3PinSelect = 3U;
            pVinCfg->DataPinMux0_1.DataLane4PinSelect = 4U;
            pVinCfg->DataPinMux0_1.DataLane5PinSelect = 5U;
            pVinCfg->DataPinMux0_1.DataLane6PinSelect = 6U;
            pVinCfg->DataPinMux0_1.DataLane7PinSelect = 7U;
            pVinCfg->DataPinMux2_3.DataLane8PinSelect = 8U;
            pVinCfg->DataPinMux2_3.DataLane9PinSelect = 9U;
            pVinCfg->DataPinMux2_3.DataLane10PinSelect = 10U;
            pVinCfg->DataPinMux2_3.DataLane11PinSelect = 11U;
            pVinCfg->DataPinMux2_3.DataLane12PinSelect = 12U;
            pVinCfg->DataPinMux2_3.DataLane13PinSelect = 13U;
            pVinCfg->DataPinMux2_3.DataLane14PinSelect = 14U;
            pVinCfg->DataPinMux2_3.DataLane15PinSelect = 15U;

            /* According to VLSI team's information, 1 byte cycle skew between 0~7 is possible since they are treated as async at
             * the output of PHY (RxByteclk0~7 treated as async from each other). Therefore, we should always set this field with
             * value 2 to handle the potential 1 byte cycle skew caused by PHY async lane routing */
            pVinCfg->Ctrl2_ActiveRegionWidth.MipiMaxSkewCycle = 2; /* unit: mipi_byteclk cycle; or 4*mipi_sensor_clk cycle */

            if (pVinMipiCphyConfig->Config.SplitCtrl.NumSplits == 0U) {
                pVinCfg->SplitWidth = 0U;
            } else {
                pVinCfg->SplitWidth = pVinMipiCphyConfig->Config.SplitCtrl.SplitWidth;
            }

            /* Active region size */
            VinActiveRegionSizeConfig(pVinCfg, &pVinMipiCphyConfig->Config.RxHvSyncCtrl);

            /* Should be larger than 1, otherwise VIN will be dead */
            pVinCfg->SyncDelayL = 1U;

            VinIntrDelayConfig(pVinCfg, DelayedVsync);

            pVinCfg->DvpCtrl_SyncPinSelect.DisableAfterSync = 0U;
            pVinCfg->DvpCtrl_SyncPinSelect.DoubleBufferEnable = 1U;

            /* update vin info */
            if (AmbaWrap_memcpy(&pVinInfo->FrameRate, &pVinMipiCphyConfig->Config.FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
                RetVal = VIN_ERR_UNEXPECTED;
            }
            pVinInfo->ColorSpace = pVinMipiCphyConfig->Config.ColorSpace;
            pVinInfo->BayerPattern = pVinMipiCphyConfig->Config.BayerPattern;
            pVinInfo->YuvOrder = pVinMipiCphyConfig->Config.YuvOrder;
            pVinInfo->NumDataBits = pVinMipiCphyConfig->Config.NumDataBits;
            pVinInfo->NumSkipFrame = pVinMipiCphyConfig->Config.NumSkipFrame;
        } else {
            RetVal = VIN_ERR_UNEXPECTED;
        }
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaCSL_VinSlvsEcConfigMain - Configure VIN main configuration for SLVS-EC
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinSlvsEcConfig Pointer to SLVS-EC configuration
 *  @return error code
 */
UINT32 AmbaCSL_VinSlvsEcConfigMain(UINT32 VinID, const AMBA_VIN_SLVSEC_CONFIG_s *pVinSlvsEcConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg = NULL;
    AMBA_CSL_VIN_INFO_s *pVinInfo = NULL;
    /* Vin internal sensor clock rate = sensor output clock rate / 10 */
    UINT32 DelayedVsync = pVinSlvsEcConfig->Config.DelayedVsync / 10U;
    UINT8 DataLanePinSelect[AMBA_VIN_NUM_SLVSEC_LANE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    UINT8 i, j, k = 0U, DualLink;
    UINT16 DataLaneEnable = 0U;

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
            pVinCfg->Ctrl.BitsPerPixel = (UINT8)((pVinSlvsEcConfig->Config.NumDataBits - 8U) >> 1U);

            /* DVP ctrl */
            if (pVinSlvsEcConfig->Config.ColorSpace == AMBA_VIN_COLOR_SPACE_YUV) {
                pVinCfg->DvpCtrl_SyncPinSelect.YuvEnable   = 1U;
                pVinCfg->DvpCtrl_SyncPinSelect.YuvOrder    = pVinSlvsEcConfig->Config.YuvOrder;
            } else {
                pVinCfg->DvpCtrl_SyncPinSelect.YuvEnable   = 0U;
            }

            if (pVinSlvsEcConfig->LinkType == AMBA_VIN_SLVSEC_LINK_TYPE_D) {
                DualLink = 1U;
                pVinCfg->MipiCtrl2_SlvsEcCtrl0.LinkType = 2U;
            } else if (pVinSlvsEcConfig->LinkType == AMBA_VIN_SLVSEC_LINK_TYPE_DX) {
                DualLink = 1U;
                pVinCfg->MipiCtrl2_SlvsEcCtrl0.LinkType = 3U;
            } else {
                DualLink = 0U;
                pVinCfg->MipiCtrl2_SlvsEcCtrl0.LinkType = 0U;
            }

            for (i = 0U; i < AMBA_VIN_NUM_SLVSEC_LANE; i++) {
                if (i < pVinSlvsEcConfig->NumActiveLanes) {
                    DataLaneEnable |= ((UINT16)((UINT32)1U << DataLanePinSelect[i]));
                } else {
                    for (j = k; j < AMBA_VIN_NUM_SLVSEC_LANE; j++) {
                        if (((DataLaneEnable) & (UINT16)((UINT32)1U << j)) == 0U) {
                            k = j + 1U;
                            break;
                        }
                    }
                    DataLanePinSelect[i] = (UINT8)j;
                }
            }

            pVinCfg->Ctrl.DataLaneEnable = DataLaneEnable;
            if (DualLink == 0U) {
                /* For Link0 */
                pVinCfg->DataPinMux0_1.DataLane0PinSelect = DataLanePinSelect[0];
                pVinCfg->DataPinMux0_1.DataLane1PinSelect = DataLanePinSelect[1];
                pVinCfg->DataPinMux0_1.DataLane2PinSelect = DataLanePinSelect[2];
                pVinCfg->DataPinMux0_1.DataLane3PinSelect = DataLanePinSelect[3];
                pVinCfg->DataPinMux0_1.DataLane4PinSelect = DataLanePinSelect[4];
                pVinCfg->DataPinMux0_1.DataLane5PinSelect = DataLanePinSelect[5];
                pVinCfg->DataPinMux0_1.DataLane6PinSelect = DataLanePinSelect[6];
                pVinCfg->DataPinMux0_1.DataLane7PinSelect = DataLanePinSelect[7];

                /* For Link1 */
                pVinCfg->DataPinMux2_3.DataLane8PinSelect = 8U;
                pVinCfg->DataPinMux2_3.DataLane9PinSelect = 9U;
                pVinCfg->DataPinMux2_3.DataLane10PinSelect = 10U;
                pVinCfg->DataPinMux2_3.DataLane11PinSelect = 11U;
                pVinCfg->DataPinMux2_3.DataLane12PinSelect = 12U;
                pVinCfg->DataPinMux2_3.DataLane13PinSelect = 13U;
                pVinCfg->DataPinMux2_3.DataLane14PinSelect = 14U;
                pVinCfg->DataPinMux2_3.DataLane15PinSelect = 15U;
            } else {
                /* For Link0 */
                pVinCfg->DataPinMux0_1.DataLane0PinSelect = DataLanePinSelect[0];
                pVinCfg->DataPinMux0_1.DataLane1PinSelect = DataLanePinSelect[1];
                pVinCfg->DataPinMux0_1.DataLane2PinSelect = DataLanePinSelect[2];
                pVinCfg->DataPinMux0_1.DataLane3PinSelect = DataLanePinSelect[3];
                pVinCfg->DataPinMux0_1.DataLane4PinSelect = DataLanePinSelect[4];
                pVinCfg->DataPinMux0_1.DataLane5PinSelect = DataLanePinSelect[5];
                pVinCfg->DataPinMux0_1.DataLane6PinSelect = 12;
                pVinCfg->DataPinMux0_1.DataLane7PinSelect = 13;

                /* For Link1 */
                pVinCfg->DataPinMux2_3.DataLane8PinSelect = DataLanePinSelect[6];
                pVinCfg->DataPinMux2_3.DataLane9PinSelect = DataLanePinSelect[7];
                pVinCfg->DataPinMux2_3.DataLane10PinSelect = DataLanePinSelect[8];
                pVinCfg->DataPinMux2_3.DataLane11PinSelect = DataLanePinSelect[9];
                pVinCfg->DataPinMux2_3.DataLane12PinSelect = DataLanePinSelect[10];
                pVinCfg->DataPinMux2_3.DataLane13PinSelect = DataLanePinSelect[11];
                pVinCfg->DataPinMux2_3.DataLane14PinSelect = 14U;
                pVinCfg->DataPinMux2_3.DataLane15PinSelect = 15U;

                if (pVinSlvsEcConfig->NumActiveLanes == 8U) { /* 4 + 4 */
                    pVinCfg->Ctrl.DataLaneEnable = 0x3CFU;
                } else if (pVinSlvsEcConfig->NumActiveLanes == 4U) { /* 2 + 2 */
                    pVinCfg->Ctrl.DataLaneEnable = 0x0C3U;
                } else if (pVinSlvsEcConfig->NumActiveLanes == 2U) { /* 1 + 1 */
                    pVinCfg->Ctrl.DataLaneEnable = 0x041U;
                } else { /* 6 + 6 */
                    pVinCfg->Ctrl.DataLaneEnable = 0xFFFU;
                }
            }

            pVinCfg->MipiCtrl2_SlvsEcCtrl0.BypassMode   = 0U;
            pVinCfg->MipiCtrl2_SlvsEcCtrl0.ValidOnly    = 1U;
            pVinCfg->MipiCtrl2_SlvsEcCtrl0.BypassFifo   = 0U;
            pVinCfg->MipiCtrl2_SlvsEcCtrl0.EccOption    = pVinSlvsEcConfig->EccOption;

            if (pVinSlvsEcConfig->Config.SplitCtrl.NumSplits == 0U) {
                pVinCfg->SplitWidth = 0U;
            } else {
                pVinCfg->SplitWidth = pVinSlvsEcConfig->Config.SplitCtrl.SplitWidth;
            }

            /* Active region size */
            VinActiveRegionSizeConfig(pVinCfg, &pVinSlvsEcConfig->Config.RxHvSyncCtrl);

            /* Should be larger than 1, otherwise VIN will be dead */
            pVinCfg->SyncDelayL = 1U;

            VinIntrDelayConfig(pVinCfg, DelayedVsync);

            pVinCfg->DvpCtrl_SyncPinSelect.DisableAfterSync = 0U;
            pVinCfg->DvpCtrl_SyncPinSelect.DoubleBufferEnable = 1U;

            /* update vin info */
            if (AmbaWrap_memcpy(&pVinInfo->FrameRate, &pVinSlvsEcConfig->Config.FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
                RetVal = VIN_ERR_UNEXPECTED;
            }
            pVinInfo->ColorSpace = pVinSlvsEcConfig->Config.ColorSpace;
            pVinInfo->BayerPattern = pVinSlvsEcConfig->Config.BayerPattern;
            pVinInfo->YuvOrder = pVinSlvsEcConfig->Config.YuvOrder;
            pVinInfo->NumDataBits = pVinSlvsEcConfig->Config.NumDataBits;
            pVinInfo->NumSkipFrame = pVinSlvsEcConfig->Config.NumSkipFrame;
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

static UINT32 CheckMipiLaneRemapCfg(UINT32 VinID, const AMBA_VIN_LANE_REMAP_CONFIG_s *pLaneRemapConfig)
{
    UINT32 RetVal = VIN_ERR_NONE, i;
    UINT8 LaneShift = 0U;
    UINT8 NumActiveLanes = pLaneRemapConfig->NumActiveLanes;

    (void) VinGetLaneShift(VinID, &LaneShift);

    /* only support 1, 2, 3, 4, 8 lane MIPI */
    if (((VinID >= AMBA_VIN_CHANNEL8) && (VinID <= AMBA_VIN_CHANNEL10)) && (NumActiveLanes == 8U)) {
        for (i = 0U; i < NumActiveLanes; i ++) {
            if ((UINT8)(pLaneRemapConfig->pPhyLaneMapping[i] - LaneShift) >= 8U) {
                /* DC-Dphy 4 lane only for physical 8lane(lane 0~7) */
                RetVal = VIN_ERR_ARG;
                break;
            }
        }
    } else if (NumActiveLanes <= 4U) {
        for (i = 0U; i < NumActiveLanes; i ++) {
            if ((UINT8)(pLaneRemapConfig->pPhyLaneMapping[i] - LaneShift) >= 4U) {
                /* 4 lane can only use local data lane(lane 0~3) */
                RetVal = VIN_ERR_ARG;
                break;
            }
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
    UINT8 DataLanePinSelect[AMBA_VIN_NUM_DATA_LANE] = {0U};
    UINT32 i;
    UINT8 NumActiveLanes = pLaneRemapConfig->NumActiveLanes;
    UINT8 UsedLane[AMBA_VIN_NUM_DATA_LANE] = {0U}; //non-use:0, use:1
    UINT8 NonUsedLane = 0U;
    UINT16 DataLaneEnable = 0U;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinGlobalCfg   = pGlobalConfigData[VinID];
        pVinCfg         = pMainConfigData[VinID];

        if (pVinGlobalCfg->SensorType == 2U) { /* DVP */
            RetVal = VIN_ERR_ARG;
        } else if (pVinGlobalCfg->SensorType == 3U) { /* MIPI*/
            RetVal = CheckMipiLaneRemapCfg(VinID, pLaneRemapConfig);
        } else {
            //SLVS
        }

        if (RetVal != VIN_ERR_ARG) {
            i = 0U;
            (void) VinGetLaneShift(VinID, &LaneShift);

            if (VinID <= AMBA_VIN_CHANNEL7) { /* DCPHY */
                while (i < NumActiveLanes) {
                    DataLanePinSelect[i] = (pLaneRemapConfig->pPhyLaneMapping[i] - LaneShift) << 1U;
                    DataLaneEnable |= ((UINT16)((UINT32)1U << DataLanePinSelect[i]));
                    UsedLane[pLaneRemapConfig->pPhyLaneMapping[i] - LaneShift] = 1U;
                    i ++;
                }
                while (i < ((UINT32)NumActiveLanes << 1U)) {
                    DataLanePinSelect[i] = ((pLaneRemapConfig->pPhyLaneMapping[i - NumActiveLanes] - LaneShift) << 1U) + 1U;
                    DataLaneEnable |= ((UINT16)((UINT32)1U << DataLanePinSelect[i]));
                    i ++;
                }
            } else {
                while (i < NumActiveLanes) {
                    DataLanePinSelect[i] = pLaneRemapConfig->pPhyLaneMapping[i] - LaneShift;
                    DataLaneEnable |= ((UINT16)((UINT32)1U << DataLanePinSelect[i]));
                    UsedLane[pLaneRemapConfig->pPhyLaneMapping[i] - LaneShift] = 1U;
                    i ++;
                }
            }

            /* get unused pin */
            pVinCfg->Ctrl.DataLaneEnable = DataLaneEnable;
            for (NonUsedLane = 0U ; NonUsedLane < AMBA_VIN_NUM_DATA_LANE ; NonUsedLane ++) {
                if (UsedLane[NonUsedLane] == 0U) {
                    break;
                }
            }

            while (i < AMBA_VIN_NUM_DATA_LANE) {
                /* the default value of un-used pins cannot be the same as used pin */
                if (VinID <= AMBA_VIN_CHANNEL7) { /* DCPHY */
                    DataLanePinSelect[i] = NonUsedLane << 1U;
                } else {
                    DataLanePinSelect[i] = NonUsedLane;
                }
                i ++;
            }
            pVinCfg->DataPinMux0_1.DataLane0PinSelect = DataLanePinSelect[0];
            pVinCfg->DataPinMux0_1.DataLane1PinSelect = DataLanePinSelect[1];
            pVinCfg->DataPinMux0_1.DataLane2PinSelect = DataLanePinSelect[2];
            pVinCfg->DataPinMux0_1.DataLane3PinSelect = DataLanePinSelect[3];
            pVinCfg->DataPinMux0_1.DataLane4PinSelect = DataLanePinSelect[4];
            pVinCfg->DataPinMux0_1.DataLane5PinSelect = DataLanePinSelect[5];
            pVinCfg->DataPinMux0_1.DataLane6PinSelect = DataLanePinSelect[6];
            pVinCfg->DataPinMux0_1.DataLane7PinSelect = DataLanePinSelect[7];
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

        if ((pVinGlobalCfg->SensorType == 3U) || (pVinGlobalCfg->SensorType == 6U)) {

            if (pMipiVirtChanConfig->VirtChan <= VIN_MIPI_VC_REG_MAX_VALUE) {
                pVinCfg->MipiCtrl0_1.VirtChanMask    = (UINT8)(pMipiVirtChanConfig->VirtChanMask & 0x3U);
                pVinCfg->MipiCtrl0_1.VirtChanPattern = (UINT8)(pMipiVirtChanConfig->VirtChan & 0x3U);
                pVinCfg->MipiCtrl2_SlvsEcCtrl0.VCxMask    = 0x7U;
                pVinCfg->MipiCtrl2_SlvsEcCtrl0.VCxPattern = 0x0U;
                pVinCfg->MipiCtrl2_SlvsEcCtrl0.EccVCxOverride = 1U; //CSI-2 1.0
            } else {
                pVinCfg->MipiCtrl0_1.VirtChanMask    = (UINT8)(pMipiVirtChanConfig->VirtChanMask & 0x3U);
                pVinCfg->MipiCtrl0_1.VirtChanPattern = (UINT8)(pMipiVirtChanConfig->VirtChan & 0x3U);
                pVinCfg->MipiCtrl2_SlvsEcCtrl0.VCxMask    = (UINT8)(pMipiVirtChanConfig->VirtChanMask >> 2U);
                pVinCfg->MipiCtrl2_SlvsEcCtrl0.VCxPattern = (UINT8)(pMipiVirtChanConfig->VirtChan >> 2U);
                pVinCfg->MipiCtrl2_SlvsEcCtrl0.EccVCxOverride = 0U; //CSI-2 2.0: VCX
            }
        }
    } else {
        RetVal = VIN_ERR_ARG;
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

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinGlobalCfg   = pGlobalConfigData[VinID];
        pVinCfg         = pMainConfigData[VinID];

        if (pVinGlobalCfg->SensorType == 3U) {
            pVinCfg->MipiCtrl0_1.EDDataTypeMask    = pMipiEmbDataConfig->DataTypeMask;
            pVinCfg->MipiCtrl0_1.EDDataTypePattern = pMipiEmbDataConfig->DataType;
            pVinCfg->MipiCtrl0_1.MipiEDEnable      = 1U;
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
    AMBA_CSL_VIN_CONFIG_DATA_s *pVinCfg = NULL;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        pVinCfg = pMainConfigData[VinID];

        pVinCfg->CropStartCol   = pCaptureWindow->OffsetX;
        pVinCfg->CropEndCol     = pCaptureWindow->OffsetX + pCaptureWindow->Width - 1U;

        pVinCfg->CropStartRow   = pCaptureWindow->OffsetY;
        pVinCfg->CropEndRow     = pCaptureWindow->OffsetY + pCaptureWindow->Height - 1U;
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

    AmbaCSL_IdspSetAddrExtension(VinSection[VinID], 0U);
    AmbaDelayCycles(0x3fffU); /* wait for section selection */

    Enable = pAmbaIDSP_VinMainReg->Ctrl0.Enable;

    AmbaCSL_IdspSetAddrExtensionData(BackupAddrExtensionData);
    AmbaDelayCycles(0x3fffU);

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
    } else if (Value == 3U) {
        RegVal[0] |=  ((UINT32)3U << (BitPos));
        RegVal[1] |=  ((UINT32)3U << (BitPos));
    } else {
        /* Should not happen!! */
    }

    pAmbaRCT_Reg->MiscDriveStrength[0] = RegVal[0];
    pAmbaRCT_Reg->MiscDriveStrength[1] = RegVal[1];
}

void AmbaCSL_VinSetDcphyRstn0(UINT32 PhyMode, UINT32 Value)
{
    UINT32 Data;

    if (PhyMode == AMBA_VIN_DCPHY_MODE_CPHY) {
        Data = (0x001f0000U | (Value << 24U) | (Value << 25U) | (Value << 26U) | (Value << 27U) | (Value << 28U) | (Value << 29U));
    } else {
        Data = (0x00000000U | (Value << 24U) | (Value << 25U) | (Value << 26U) | (Value << 27U) | (Value << 28U) | (Value << 29U));
    }

    pAmbaMIPI_Reg->DCphy0RctCtrl0  = Data;
}

void AmbaCSL_VinSetDcphyRstn1(UINT32 PhyMode, UINT32 Value)
{
    UINT32 Data;

    if (PhyMode == AMBA_VIN_DCPHY_MODE_CPHY) {
        Data = (0x001f0000U | (Value << 24U) | (Value << 25U) | (Value << 26U) | (Value << 27U) | (Value << 28U) | (Value << 29U));
    } else {
        Data = (0x00000000U | (Value << 24U) | (Value << 25U) | (Value << 26U) | (Value << 27U) | (Value << 28U) | (Value << 29U));
    }

    pAmbaMIPI_Reg->DCphy1RctCtrl0  = Data;
}

