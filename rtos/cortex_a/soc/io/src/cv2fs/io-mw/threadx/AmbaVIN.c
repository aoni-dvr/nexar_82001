/**
 *  @file AmbaVIN.c
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
 *  @details Video Input APIs
 *
 */

#if defined(CONFIG_QNX)
#include "Generic.h"
#include "hw/ambarella_clk.h"
#endif
#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"

#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaVIN.h"
#include "AmbaVIN_Ctrl.h"
#include "AmbaRTSL_VIN.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaCSL_DebugPort.h"
#include "AmbaCSL_Scratchpad.h"
#include "AmbaCSL_VIN.h"
#if !(defined(CONFIG_LINUX) || defined(CONFIG_QNX))
#include "AmbaCSL_DebugPort.h"
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include "AmbaIOUtility.h"
#endif
#endif
#if !defined(CONFIG_QNX)
#include "AmbaCEHU.h"
#endif
#if defined(CONFIG_VIN_ASIL)
#include "AmbaSafety_VIN.h"
#endif

#define MIPI_RX_CLK_ACTIVE_HS_POLLING_TIMEOUT 500U

static UINT32 VIN_DrvInited;

AMBA_KAL_MUTEX_t AmbaVinMutex;  /* Mutex */

static void VIN_CalculateMphyConfig(UINT64 BitRate, AMBA_VIN_MIPI_TIMING_PARAM_s *pVinMipiTiming);
#if defined(CONFIG_VIN_ASIL)
static void AmbaVIN_CalculateMphyConfig_(UINT64 BitRate, AMBA_VIN_MIPI_TIMING_PARAM_s *pVinMipiTiming);
#endif

//Check API
static UINT32 VIN_ParamCheckSlvsEnabledLane(UINT32 VinID, UINT32 EnabledPin)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (((VinID == AMBA_VIN_CHANNEL0) &&
         ((EnabledPin & 0xfffU) == 0U)) ||
        ((VinID == AMBA_VIN_CHANNEL1) &&
         (((EnabledPin & 0xf00U) == 0U) || ((EnabledPin & 0xffU) != 0U))) ||
        ((VinID == AMBA_VIN_CHANNEL8) &&
         (((EnabledPin & 0xf0U) == 0U) || ((EnabledPin & 0xf0fU) != 0U)))) {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

static UINT32 VIN_ParamCheckMipiEnabledLane(UINT32 VinID, UINT32 EnabledPin)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (((VinID == AMBA_VIN_CHANNEL0) &&
         (((EnabledPin & 0xffU) == 0U) || ((EnabledPin & 0xf00U) != 0U))) ||
        ((VinID == AMBA_VIN_CHANNEL1) &&
         (((EnabledPin & 0xf00U) == 0U) || ((EnabledPin & 0xffU) != 0U))) ||
        (((VinID == AMBA_VIN_CHANNEL2) || (VinID == AMBA_VIN_CHANNEL3) || (VinID == AMBA_VIN_CHANNEL4)) &&
         (((EnabledPin & 0xfU) == 0U) || ((EnabledPin & 0xff0U) != 0U))) ||
        ((VinID == AMBA_VIN_CHANNEL8) &&
         (((EnabledPin & 0xf0U) == 0U) || ((EnabledPin & 0xf0fU) != 0U)))) {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

static UINT32 VIN_ParamCheckMainConfig(AMBA_VIN_MAIN_CONFIG_s Config)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (((Config.FrameRate.Interlace != 0U) && (Config.FrameRate.Interlace != 1U)) ||
        (Config.ColorSpace > AMBA_VIN_COLOR_SPACE_RCCC) ||
        (((Config.BayerPattern > AMBA_VIN_BAYER_PATTERN_GB) && (Config.BayerPattern < AMBA_VIN_BAYER_PATTERN_RGGI)) ||
         ((Config.BayerPattern > AMBA_VIN_BAYER_PATTERN_GIBG) && (Config.BayerPattern < AMBA_VIN_BAYER_PATTERN_RCCC)) ||
         (Config.BayerPattern > AMBA_VIN_BAYER_PATTERN_CCRC)) ||
        (Config.YuvOrder > AMBA_VIN_YUV_ORDER_Y0_CR_Y1_CB) ||
        ((Config.NumDataBits != 8U) && (Config.NumDataBits != 10U) &&
         (Config.NumDataBits != 12U) && (Config.NumDataBits != 14U) &&
         (Config.NumDataBits != 16U)) ||
        (Config.RxHvSyncCtrl.NumActivePixels == 0U) || (Config.RxHvSyncCtrl.NumActiveLines == 0U) ||
        (Config.RxHvSyncCtrl.NumTotalPixels < Config.RxHvSyncCtrl.NumActivePixels) ||
        (Config.RxHvSyncCtrl.NumTotalLines < Config.RxHvSyncCtrl.NumActiveLines)) {
        RetVal = VIN_ERR_ARG;
    }
    return RetVal;
}

static UINT32 VIN_ParamCheckLaneMapping(UINT8 NumActiveLanes, const UINT8 *pLaneMapping)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 i = 0U;
    UINT32 MaxLane = 12U;
    UINT8 cnt = 0U;


    if ((pLaneMapping != NULL) &&
        ((NumActiveLanes != 0U) && (NumActiveLanes < MaxLane))) {

        for (i = 0; i < NumActiveLanes; i++) {
            if (pLaneMapping[i] == 0U) {
                cnt++;
            } else if(pLaneMapping[i] >= MaxLane) {
                cnt = 2U;
            } else {
                //nothing
            }

            if (cnt > 1U) {
                RetVal = VIN_ERR_ARG;
                break;
            } else {
                //nothing
            }
        }
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

static UINT32 VIN_ParamCheckSyncDetectCtrl(UINT8 ITU656Type, AMBA_VIN_CUSTOM_SYNC_CODE_s CustomSyncCode)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((ITU656Type > 1U) ||
        (CustomSyncCode.PatternAlign > 1U) ||
        (CustomSyncCode.SolDetectEnable > 1U) ||
        (CustomSyncCode.EolDetectEnable > 1U) ||
        (CustomSyncCode.SofDetectEnable > 1U) ||
        (CustomSyncCode.EofDetectEnable > 1U) ||
        (CustomSyncCode.SovDetectEnable > 1U) ||
        (CustomSyncCode.EovDetectEnable > 1U)) {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

static UINT32 VIN_ParamCheckMasterSync(const AMBA_VIN_MASTER_SYNC_CONFIG_s *pMSyncConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((pMSyncConfig->RefClk == 0U) ||
        (pMSyncConfig->HSync.Period == 0U) ||
        (pMSyncConfig->HSync.PulseWidth > pMSyncConfig->HSync.Period) ||
        (pMSyncConfig->HSync.Polarity > 1U) ||
        (pMSyncConfig->VSync.Period == 0U) ||
        (pMSyncConfig->VSync.PulseWidth > (pMSyncConfig->VSync.Period * pMSyncConfig->HSync.Period)) ||
        (pMSyncConfig->VSync.Polarity > 1U) ||
        (pMSyncConfig->ToggleHsyncInVblank > 1U)) {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

static UINT32 VIN_ParamCheckDelayedVSync(const AMBA_VIN_DELAYED_VSYNC_CONFIG_s *pDelayedVSyncConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 i = 0;

    if ((pDelayedVSyncConfig->DelayedHVsyncSource <= 3U) &&
        (pDelayedVSyncConfig->Polarity.HsPolInput <= 1U) &&
        (pDelayedVSyncConfig->Polarity.VsPolInput <= 1U) &&
        (pDelayedVSyncConfig->Polarity.HsPolOutput <= 1U)) {

        for(i=0; i<AMBA_VIN_DELAYED_VSYNC_NUM; i++) {
            if(pDelayedVSyncConfig->Polarity.VsPolOutput[i] > 1U) {
                RetVal = VIN_ERR_ARG;
                break;
            }
        }
    } else {
        RetVal = VIN_ERR_ARG;
    }
    return RetVal;
}

#if 0
static UINT32 VIN_ParamCheckVirtChanHDR(const AMBA_VIN_MIPI_CONFIG_s *pVinMipiConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((pVinMipiConfig->VirtChanHDREnable != 0U) ||
        (pVinMipiConfig->VirtChanHDRConfig.VirtChanNum != 0U) ||
        (pVinMipiConfig->VirtChanHDRConfig.VCPattern1stExp != 0U) ||
        (pVinMipiConfig->VirtChanHDRConfig.VCPattern2ndExp != 0U) ||
        (pVinMipiConfig->VirtChanHDRConfig.VCPattern3rdExp != 0U) ||
        (pVinMipiConfig->VirtChanHDRConfig.VCPattern4thExp != 0U) ||
        (pVinMipiConfig->VirtChanHDRConfig.Offset2ndExp != 0U) ||
        (pVinMipiConfig->VirtChanHDRConfig.Offset3rdExp != 0U)) {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}
#endif
static UINT32 VIN_ParamCheckDvpConfig(const AMBA_VIN_DVP_CONFIG_s *pVinDvpConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (((pVinDvpConfig->PadType != AMBA_VIN_DVP_PAD_TYPE_LVCMOS) &&
         (pVinDvpConfig->PadType != AMBA_VIN_DVP_PAD_TYPE_LVDS)) ||
        (pVinDvpConfig->DvpWide > 1U) ||
        (pVinDvpConfig->DataLatchEdge > 2U) ||
        (pVinDvpConfig->SyncDetectCtrl.SyncType > 1U) ||
        ((pVinDvpConfig->SyncDetectCtrl.SyncType == 0U) && (pVinDvpConfig->SyncDetectCtrl.SyncPinConfig.HsyncPinSelect > 2U)) ||
        ((pVinDvpConfig->SyncDetectCtrl.SyncType == 0U) && (pVinDvpConfig->SyncDetectCtrl.SyncPinConfig.VsyncPinSelect > 2U)) ||
        ((pVinDvpConfig->SyncDetectCtrl.SyncType == 0U) && (pVinDvpConfig->SyncDetectCtrl.SyncPinConfig.FieldPinSelect > 2U)) ||
        ((pVinDvpConfig->SyncDetectCtrl.SyncType == 0U) && (pVinDvpConfig->SyncDetectCtrl.SyncPinConfig.HsyncPolarity > 1U)) ||
        ((pVinDvpConfig->SyncDetectCtrl.SyncType == 0U) && (pVinDvpConfig->SyncDetectCtrl.SyncPinConfig.VsyncPolarity > 1U)) ||
        ((pVinDvpConfig->SyncDetectCtrl.SyncType == 0U) && (pVinDvpConfig->SyncDetectCtrl.SyncPinConfig.FieldPolarity > 1U)) ||
        (pVinDvpConfig->SyncDetectCtrl.EmbSyncConfig.SyncLocate > 2U)) {

        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaVIN_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVIN_DrvEntry(void)
{
    UINT32 RetVal = VIN_ERR_NONE;
#if !(defined(CONFIG_LINUX) || defined(CONFIG_QNX))
    ULONG BaseAddr;
#endif
#if defined(CONFIG_VIN_ASIL)
    UINT32 VinID;
#endif

    /* Create Mutex */
    if (AmbaKAL_MutexCreate(&AmbaVinMutex, NULL) != OK) {
        RetVal = VIN_ERR_UNEXPECTED;    /* should never happen */
    } else {
        /* get base address */
#if !(defined(CONFIG_LINUX) || defined(CONFIG_QNX))
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
        BaseAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,mipi-phy", "reg", 0U);
        if (BaseAddr == 0ULL) {
            RetVal = VIN_ERR_UNEXPECTED;
        } else {
            AmbaMisra_TypeCast(&pAmbaMIPI_Reg, &BaseAddr);
        }

        BaseAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,idsp-controller", "reg", 0U);
        if (BaseAddr == 0ULL) {
            RetVal = VIN_ERR_UNEXPECTED;
        } else {
            AmbaMisra_TypeCast(&pAmbaIDSP_CtrlReg, &BaseAddr);
        }

        BaseAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,idsp-vin-main", "reg", 0U);
        if (BaseAddr == 0ULL) {
            RetVal = VIN_ERR_UNEXPECTED;
        } else {
            AmbaMisra_TypeCast(&pAmbaIDSP_VinMainReg, &BaseAddr);
        }

        BaseAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,idsp-vin-global", "reg", 0U);
        if (BaseAddr == 0ULL) {
            RetVal = VIN_ERR_UNEXPECTED;
        } else {
            AmbaMisra_TypeCast(&pAmbaIDSP_VinGlobalReg, &BaseAddr);
        }

        BaseAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,idsp-vin-msync", "reg", 0U);
        if (BaseAddr == 0ULL) {
            RetVal = VIN_ERR_UNEXPECTED;
        } else {
            AmbaMisra_TypeCast(&pAmbaIDSP_VinMasterSyncReg, &BaseAddr);
        }
#else
        BaseAddr = AMBA_DBG_PORT_MIPI_BASE_ADDR;
        AmbaMisra_TypeCast(&pAmbaMIPI_Reg, &BaseAddr);

        BaseAddr = AMBA_DBG_PORT_IDSP_CONTROLLER_BASE_ADDR;
        AmbaMisra_TypeCast(&pAmbaIDSP_CtrlReg, &BaseAddr);

        BaseAddr = AMBA_DBG_PORT_IDSP_VIN_MAIN_CONFIG_BASE_ADDR;
        AmbaMisra_TypeCast(&pAmbaIDSP_VinMainReg, &BaseAddr);

        BaseAddr = AMBA_DBG_PORT_IDSP_VIN_GLOBAL_CONFIG_BASE_ADDR;
        AmbaMisra_TypeCast(&pAmbaIDSP_VinGlobalReg, &BaseAddr);

        BaseAddr = AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_CONFIG_BASE_ADDR;
        AmbaMisra_TypeCast(&pAmbaIDSP_VinMasterSyncReg, &BaseAddr);
#endif
#endif

        if (RetVal == VIN_ERR_NONE) {
#if defined(CONFIG_VIN_ASIL)
            for (VinID = 0U; VinID < AMBA_NUM_VIN_CHANNEL; VinID++) {
                (void)AmbaSafety_VinInit(VinID);
            }
#endif
            AmbaRTSL_VinInit();
#if defined(CONFIG_VIN_ASIL)
            AmbaRTSL_VinInit_();
#endif
            VIN_DrvInited = 1U;
        }
    }

    return RetVal;
}

#if !defined(CONFIG_QNX)
#define CEHU_NUM                    2U
#define CEHU_IDSP_SAFETY_ERROR_ID   42U
#endif

static void VIN_ClearSafetyError(UINT32 VinID)
{
    if (VinID == AMBA_VIN_CHANNEL0) {
        AmbaCSL_IdspClearVin0PipelineSafetyError();
    } else if (VinID == AMBA_VIN_CHANNEL1) {
        AmbaCSL_IdspClearVin1PipelineSafetyError();
    } else if (VinID == AMBA_VIN_CHANNEL2) {
        AmbaCSL_IdspClearVin2PipelineSafetyError();
    } else if (VinID == AMBA_VIN_CHANNEL3) {
        AmbaCSL_IdspClearVin3PipelineSafetyError();
    } else if (VinID == AMBA_VIN_CHANNEL4) {
        AmbaCSL_IdspClearVin4PipelineSafetyError();
    } else {
        AmbaCSL_IdspClearVin8PipelineSafetyError();
    }
}

static void VIN_Reset(UINT32 VinID)
{
#if !defined(CONFIG_QNX)
    UINT32 i, ErrorID = CEHU_IDSP_SAFETY_ERROR_ID, Value[CEHU_NUM];

    for (i = 0U; i < CEHU_NUM; i++) {
        (void)AmbaCEHU_MaskGet(i, ErrorID, &Value[i]);
        if (Value[i] == 0U) {
            (void)AmbaCEHU_MaskEnable(i, ErrorID);
        }
    }
#endif

#if defined(CONFIG_VIN_ASIL)
    (void)AmbaRTSL_VinReset_(VinID, 1U);
    (void)AmbaRTSL_VinReset_(VinID, 0U);
#endif
    (void)AmbaRTSL_VinReset(VinID, 1U);
    (void)AmbaRTSL_VinReset(VinID, 0U);

    (void)VIN_ClearSafetyError(VinID);

#if !defined(CONFIG_QNX)
    for (i = 0U; i < CEHU_NUM; i++) {
        if (Value[i] == 0U) {
            (void)AmbaCEHU_MaskDisable(i, ErrorID);
        }
    }
#endif
}


/**
 *  AmbaVIN_SlvsReset - Reset VIN module and reset VIN pad to SLVS mode
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pPadConfig Pointer to VIN pad configuration
 *  @return error code
 */
UINT32 AmbaVIN_SlvsReset(UINT32 VinID, const AMBA_VIN_SLVS_PAD_CONFIG_s *pPadConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (((VinID != AMBA_VIN_CHANNEL0) && (VinID != AMBA_VIN_CHANNEL1) && (VinID != AMBA_VIN_CHANNEL8)) ||
        (pPadConfig == NULL) || (VIN_ParamCheckSlvsEnabledLane(VinID, pPadConfig->EnabledPin) == VIN_ERR_ARG)) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            /* pre-configure pad mode */
            (void)AmbaRTSL_VinSetLvdsPadMode(VinID, AMBA_VIN_PAD_MODE_SLVS, pPadConfig->EnabledPin);
#if defined(CONFIG_VIN_ASIL)
            (void)AmbaRTSL_VinSetLvdsPadMode_(VinID, AMBA_VIN_PAD_MODE_SLVS, pPadConfig->EnabledPin);
#endif

            /* Reset VIN module */
            (void)VIN_Reset(VinID);

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_MipiReset - Reset VIN module and reset VIN pad to MIPI mode
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pPadConfig Pointer to VIN pad configuration
 *  @return error code
 */
UINT32 AmbaVIN_MipiReset(UINT32 VinID, const AMBA_VIN_MIPI_PAD_CONFIG_s *pPadConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_VIN_MIPI_TIMING_PARAM_s VinMipiTiming;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pPadConfig == NULL) ||
        ((pPadConfig->ClkMode != AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS) &&
         (pPadConfig->ClkMode != AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS)) ||
        ((pPadConfig->DateRate < AMBA_VIN_MIPI_DATARATE_MIN) ||
         (pPadConfig->DateRate > AMBA_VIN_MIPI_DATARATE_MAX)) ||
        (VIN_ParamCheckMipiEnabledLane(VinID, pPadConfig->EnabledPin) == VIN_ERR_ARG)) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            /* configure MIPI phy */
            VIN_CalculateMphyConfig(pPadConfig->DateRate, &VinMipiTiming);
            RetVal = AmbaRTSL_VinConfigMipiPhy(VinID, &VinMipiTiming, pPadConfig->ClkMode, pPadConfig->EnabledPin);
#if defined(CONFIG_VIN_ASIL)
            AmbaVIN_CalculateMphyConfig_(pPadConfig->DateRate, &VinMipiTiming);
            (void)AmbaRTSL_VinConfigMipiPhy_(VinID, &VinMipiTiming, pPadConfig->ClkMode, pPadConfig->EnabledPin);
#endif

            /* pre-configure pad mode */
            (void)AmbaRTSL_VinSetLvdsPadMode(VinID, AMBA_VIN_PAD_MODE_MIPI, pPadConfig->EnabledPin);
#if defined(CONFIG_VIN_ASIL)
            (void)AmbaRTSL_VinSetLvdsPadMode_(VinID, AMBA_VIN_PAD_MODE_MIPI, pPadConfig->EnabledPin);
#endif

            /* Reset VIN module */
            (void)VIN_Reset(VinID);

            /* Since the default value of DphyRst0/1/2 is 1(dphy is in reset state) on cv2fs,
               call AmbaRTSL_VinResetMipiLogic to set DphyRst0/1/2 to 0 for MIPI signal detection. */
            (void)AmbaRTSL_VinResetMipiLogic(VinID);
#if defined(CONFIG_VIN_ASIL)
            (void)AmbaRTSL_VinResetMipiLogic_(VinID);
#endif

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_DvpReset - Reset VIN module and reset VIN pad to Parallel mode
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pPadConfig Pointer to VIN pad configuration
 *  @return error code
 */
UINT32 AmbaVIN_DvpReset(UINT32 VinID, const AMBA_VIN_DVP_PAD_CONFIG_s *pPadConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((VinID != AMBA_VIN_CHANNEL0) || (pPadConfig == NULL) ||
        ((pPadConfig->PadType != AMBA_VIN_DVP_PAD_TYPE_LVCMOS) &&
         (pPadConfig->PadType != AMBA_VIN_DVP_PAD_TYPE_LVDS))) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            /* pre-configure pad mode */
            if (pPadConfig->PadType == AMBA_VIN_DVP_PAD_TYPE_LVCMOS) {
                (void)AmbaRTSL_VinSetLvdsPadMode(VinID, AMBA_VIN_PAD_MODE_DVP_LVCMOS, 0xff);
#if defined(CONFIG_VIN_ASIL)
                (void)AmbaRTSL_VinSetLvdsPadMode_(VinID, AMBA_VIN_PAD_MODE_DVP_LVCMOS, 0xff);
#endif
            } else { /* (pPadConfig->PadType == AMBA_VIN_DVP_PAD_TYPE_LVDS) */
                (void)AmbaRTSL_VinSetLvdsPadMode(VinID, AMBA_VIN_PAD_MODE_DVP_LVDS, 0xfff);
#if defined(CONFIG_VIN_ASIL)
                (void)AmbaRTSL_VinSetLvdsPadMode_(VinID, AMBA_VIN_PAD_MODE_DVP_LVDS, 0xfff);
#endif
            }

            /* Reset VIN module */
            (void)VIN_Reset(VinID);

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_SlvsConfig - Configure VIN to receive data via SLVS or Sub-LVDS interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinSlvsConfig Pointer to SLVS configuration
 *  @return error code
 */
UINT32 AmbaVIN_SlvsConfig(UINT32 VinID, const AMBA_VIN_SLVS_CONFIG_s *pVinSlvsConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (((VinID != AMBA_VIN_CHANNEL0) && (VinID != AMBA_VIN_CHANNEL1) && (VinID != AMBA_VIN_CHANNEL8)) ||
        (pVinSlvsConfig == NULL) ||
        (VIN_ParamCheckMainConfig(pVinSlvsConfig->Config) == VIN_ERR_ARG) ||
        (VIN_ParamCheckLaneMapping(pVinSlvsConfig->NumActiveLanes, pVinSlvsConfig->pLaneMapping) == VIN_ERR_ARG) ||
        (pVinSlvsConfig->SyncDetectCtrl.SyncInterleaving > 2U) ||
        (VIN_ParamCheckSyncDetectCtrl(pVinSlvsConfig->SyncDetectCtrl.ITU656Type, pVinSlvsConfig->SyncDetectCtrl.CustomSyncCode) == VIN_ERR_ARG)) {
        RetVal = VIN_ERR_ARG;

    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
#if defined(CONFIG_VIN_ASIL)
    } else if (AmbaSafety_VinGetSafeState(VinID, NULL) != 0U) {
        RetVal = VIN_ERR_OSERR;
#endif
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            /* Reset SLVS PHY when sensor bit clock is present, or PHY may not lock the clock */
            (void)AmbaRTSL_VinResetSlvsPhy(VinID);
#if defined(CONFIG_VIN_ASIL)
            (void)AmbaRTSL_VinResetSlvsPhy_(VinID);
#endif

            RetVal = AmbaRTSL_VinSlvsConfig(VinID, pVinSlvsConfig);
#if defined(CONFIG_VIN_ASIL)
            (void)AmbaRTSL_VinSlvsConfig_(VinID, pVinSlvsConfig);
#endif

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_MipiConfig - Configure VIN to receive data via MIPI interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinMipiConfig Pointer to MIPI configuration
 *  @return error code
 */
UINT32 AmbaVIN_MipiConfig(UINT32 VinID, const AMBA_VIN_MIPI_CONFIG_s *pVinMipiConfig)
{
    UINT32 RetVal = VIN_ERR_NONE, IsTimeOut = 0U;
    UINT32 Count = 0U;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pVinMipiConfig == NULL) ||
        (VIN_ParamCheckMainConfig(pVinMipiConfig->Config) == VIN_ERR_ARG) ||
        (pVinMipiConfig->NumActiveLanes == 0U) ||
        (pVinMipiConfig->NumActiveLanes > 8U)) {
        //(VIN_ParamCheckVirtChanHDR(pVinMipiConfig) == VIN_ERR_ARG)) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
#if defined(CONFIG_VIN_ASIL)
    } else if (AmbaSafety_VinGetSafeState(VinID, NULL) != 0U) {
        RetVal = VIN_ERR_OSERR;
#endif
    } else {
        /* Reset MIPI logic inside VIN after receiving input MIPI clock from the PHY */
        if (VinID == AMBA_VIN_CHANNEL1) {
            while (AmbaCSL_VinGetMipiRxClkActHs2() == 0U) {
                if (Count++ >= MIPI_RX_CLK_ACTIVE_HS_POLLING_TIMEOUT) {
                    IsTimeOut = 1U;
                    break;
                }
                (void)AmbaKAL_TaskSleep(1);
            }
        } else if (VinID == AMBA_VIN_CHANNEL8) {
            while (AmbaCSL_VinGetMipiRxClkActHs1() == 0U) {
                if (Count++ >= MIPI_RX_CLK_ACTIVE_HS_POLLING_TIMEOUT) {
                    IsTimeOut = 1U;
                    break;
                }
                (void)AmbaKAL_TaskSleep(1);
            }
        } else {
            while (AmbaCSL_VinGetMipiRxClkActHs0() == 0U) {
                if (Count++ >= MIPI_RX_CLK_ACTIVE_HS_POLLING_TIMEOUT) {
                    IsTimeOut = 1U;
                    break;
                }
                (void)AmbaKAL_TaskSleep(1);
            }
        }
        (void)AmbaRTSL_VinResetMipiLogic(VinID);
#if defined(CONFIG_VIN_ASIL)
        (void)AmbaRTSL_VinResetMipiLogic_(VinID);
#endif

        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinMipiConfig(VinID, pVinMipiConfig);
#if defined(CONFIG_VIN_ASIL)
            (void)AmbaRTSL_VinMipiConfig_(VinID, pVinMipiConfig);
#endif

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return (IsTimeOut == 0U) ? RetVal : VIN_ERR_TIMEOUT;
}

/**
 *  AmbaVIN_DvpConfig - Configure VIN to receive data via digital video port (DVP) parallel interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinDvpConfig Pointer to DVP configuration
 *  @return error code
 */
UINT32 AmbaVIN_DvpConfig(UINT32 VinID, const AMBA_VIN_DVP_CONFIG_s *pVinDvpConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((VinID != AMBA_VIN_CHANNEL0) ||
        (pVinDvpConfig == NULL) ||
        (VIN_ParamCheckMainConfig(pVinDvpConfig->Config) == VIN_ERR_ARG) ||
        (VIN_ParamCheckDvpConfig(pVinDvpConfig) == VIN_ERR_ARG) ||
        (VIN_ParamCheckSyncDetectCtrl(pVinDvpConfig->SyncDetectCtrl.EmbSyncConfig.ITU656Type, pVinDvpConfig->SyncDetectCtrl.EmbSyncConfig.CustomSyncCode) == VIN_ERR_ARG)) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
#if defined(CONFIG_VIN_ASIL)
    } else if (AmbaSafety_VinGetSafeState(VinID, NULL) != 0U) {
        RetVal = VIN_ERR_OSERR;
#endif
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinDvpConfig(VinID, pVinDvpConfig);
#if defined(CONFIG_VIN_ASIL)
            (void)AmbaRTSL_VinDvpConfig_(VinID, pVinDvpConfig);
#endif

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

#if defined(CONFIG_QNX)
static UINT32 GetVinClk(UINT32 VinID)
{
    int fd;
    clk_freq_t ClkFreq;

    fd = open("/dev/clock", O_RDWR);

    if (VinID == AMBA_VIN_CHANNEL0) {
        ClkFreq.id = AMBA_CLK_VIN0;
    } else {
        ClkFreq.id = AMBA_CLK_VIN1;
    }

    devctl(fd, DCMD_CLOCK_GET_FREQ, &ClkFreq, sizeof(ClkFreq), NULL);

    close(fd);

    return ClkFreq.freq;
}

static UINT32 SetSensorClk(UINT32 ClkID, UINT32 Frequency)
{
    int RetVal = 0;
    int fd;
    clk_freq_t ClkFreq;

    fd = open("/dev/clock", O_RDWR);

    if (fd == -1) {
        RetVal = VIN_ERR_OPEN_FILE;
    } else {
        if (ClkID == AMBA_VIN_SENSOR_CLOCK0) {
            ClkFreq.id = AMBA_CLK_SENSOR0;
        } else if (ClkID == AMBA_VIN_SENSOR_CLOCK1) {
            ClkFreq.id = AMBA_CLK_SENSOR1;
        } else {    /* (ClkID == AMBA_VIN_SENSOR_CLOCK2) */
            ClkFreq.id = AMBA_CLK_SENSOR2;
        }
        ClkFreq.freq = Frequency;
        if (devctl(fd, DCMD_CLOCK_SET_FREQ, &ClkFreq, sizeof(ClkFreq), NULL) != 0) {
            RetVal = VIN_ERR_DEV_CTL;
        }

        close(fd);
    }

    return RetVal;
}

static UINT32 SetMsync1ClkSrc(UINT32 Vin1ClkConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    int fd;
    clk_config_t Config;

    fd = open("/dev/clock", O_RDWR);

    if (fd == -1) {
        RetVal = VIN_ERR_OPEN_FILE;
    } else {
        Config.id = AMBA_CLK_SENSOR1;
        Config.config = Vin1ClkConfig;  /* 0: AMBA_PLL_SENSOR_CLK_0,  1: AMBA_PLL_SENSOR_CLK_1 (default source of gclk_so_vin1) */

        if (devctl(fd, DCMD_CLOCK_CLK_CONFIG, &Config, sizeof(Config), NULL) != 0) {
            RetVal = VIN_ERR_DEV_CTL;
        }

        close(fd);
    }

    return RetVal;
}
#endif

/**
 *  AmbaVIN_MasterSyncEnable - Enable master H/V Sync generator
 *  @param[in] MSyncID Indicate Master sync pin ID
 *  @param[in] pMSyncConfig Pointer to master H/V Sync configuration
 *  @return error code
 */
UINT32 AmbaVIN_MasterSyncEnable(UINT32 MSyncID, const AMBA_VIN_MASTER_SYNC_CONFIG_s *pMSyncConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 RefClk;

    if ((MSyncID >= AMBA_NUM_VIN_MSYNC) || (pMSyncConfig == NULL) ||
        (VIN_ParamCheckMasterSync(pMSyncConfig) == VIN_ERR_ARG)) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
    } else {
        RefClk = pMSyncConfig->RefClk;
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {
#if defined(CONFIG_QNX)
            if (MSyncID == AMBA_VIN_MSYNC0) {
                if (RefClk != GetVinClk(AMBA_VIN_CHANNEL0)) {
                    RetVal = SetSensorClk(AMBA_VIN_SENSOR_CLOCK0, RefClk);
                }
            } else { /* (MSyncID == AMBA_VIN_MSYNC1) */
                if (RefClk != GetVinClk(AMBA_VIN_CHANNEL1)) {
                    RetVal = SetSensorClk(AMBA_VIN_SENSOR_CLOCK1, RefClk);

                    if (RetVal == VIN_ERR_NONE) {
                        RetVal = SetMsync1ClkSrc(1);
                    }
                }
            }
#else
            if (MSyncID == AMBA_VIN_MSYNC0) {
                if (RefClk != AmbaRTSL_PllGetVin0Clk()) {
                    /* The reference clock is from SENSOR PLL */
                    RetVal = AmbaRTSL_PllSetSensor0Clk(RefClk);
                }
            } else {    /* (MSyncID == AMBA_VIN_MSYNC1) */
                    /* The reference clock is from SENSOR2 PLL */
                if (RefClk != AmbaRTSL_PllGetVin1Clk()) {
                    RetVal = AmbaRTSL_PllSetSensor1Clk(RefClk);
                }
                if (RetVal == PLL_ERR_NONE) {
                    (void)AmbaRTSL_PllSetVin1ClkConfig(AMBA_PLL_SENSOR_CLK_1);
                }
            }
#endif

            if (RetVal == PLL_ERR_NONE) {
                RetVal = AmbaRTSL_VinMasterSyncConfig(MSyncID, pMSyncConfig);
            }

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_MasterSyncDisable - Disable master H/V Sync generator
 *  @param[in] MSyncID Indicate Master sync pin ID
 *  @return error code
 */
UINT32 AmbaVIN_MasterSyncDisable(UINT32 MSyncID)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (MSyncID >= AMBA_NUM_VIN_MSYNC) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinResetHvSyncOutput(MSyncID);

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_SensorClkEnable - Enable clock as the sensor clock source
 *  @param[in] ClkID Indicate clock channel
 *  @param[in] Frequency Clock frequency
 *  @return error code
 */
UINT32 AmbaVIN_SensorClkEnable(UINT32 ClkID, UINT32 Frequency)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((ClkID >= AMBA_NUM_VIN_SENSOR_CLOCK) || (Frequency == 0U)) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {
#if defined(CONFIG_QNX)
            int fd;
            clk_freq_t ClkFreq;
            clk_config_t Config;

            fd = open("/dev/clock", O_RDWR);

            if (fd == -1) {
                RetVal = VIN_ERR_OPEN_FILE;
            } else {
                if (ClkID == AMBA_VIN_SENSOR_CLOCK0) {
                    ClkFreq.id = AMBA_CLK_SENSOR0;
                    Config.id = AMBA_CLK_SENSOR0;
                    Config.config = 0U;  /* AMBA_PLL_SENSOR_REF_CLK_OUTPUT */
                } else if (ClkID == AMBA_VIN_SENSOR_CLOCK1) {
                    ClkFreq.id = AMBA_CLK_SENSOR1;
                    Config.id = AMBA_CLK_SENSOR1;
                    Config.config = 1U;  /* AMBA_PLL_SENSOR_CLK_1 (default source of gclk_so_vin1) */
                } else {    /* (ClkID == AMBA_VIN_SENSOR_CLOCK2) */
                    ClkFreq.id = AMBA_CLK_SENSOR2;
                    Config.id = AMBA_CLK_SENSOR2;
                    Config.config = 0U; /* Not used */
                }

                ClkFreq.freq = Frequency;

                if (devctl(fd, DCMD_CLOCK_SET_FREQ, &ClkFreq, sizeof(ClkFreq), NULL) != EOK) {
                    RetVal = VIN_ERR_DEV_CTL;
                } else {
                    if (devctl(fd, DCMD_CLOCK_CLK_CONFIG, &Config, sizeof(Config), NULL) != EOK) {
                        RetVal = VIN_ERR_DEV_CTL;
                    }
                }

                close(fd);
            }
#else
            if (ClkID == AMBA_VIN_SENSOR_CLOCK0) {
                RetVal = AmbaRTSL_PllSetSensor0Clk(Frequency);
                if (RetVal == PLL_ERR_NONE) {
                    (void)AmbaRTSL_PllSetSensor0ClkDir(AMBA_PLL_SENSOR_REF_CLK_OUTPUT);
                }
            } else if (ClkID == AMBA_VIN_SENSOR_CLOCK1) {
                RetVal = AmbaRTSL_PllSetSensor1Clk(Frequency);
            } else {    /* (ClkID == AMBA_VIN_SENSOR_CLOCK2) */
                AmbaCSL_SetEnet2ndRefClk();
                RetVal = AmbaRTSL_PllSetEthernetClk(Frequency);
                if (RetVal == PLL_ERR_NONE) {
                    (void)AmbaRTSL_PllSetEnetClkConfig(AMBA_PLL_ENET_CLK_REF);
                }
            }
#endif

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_SensorClkDisable - Disable the clock
 *  @param[in] ClkID Indicate clock channel
 *  @return error code
 */
UINT32 AmbaVIN_SensorClkDisable(UINT32 ClkID)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (ClkID >= AMBA_NUM_VIN_SENSOR_CLOCK) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {
#if defined(CONFIG_QNX)
            int fd;
            clk_freq_t ClkFreq;
            clk_config_t Config;

            fd = open("/dev/clock", O_RDWR);

            if (fd == -1) {
                RetVal = VIN_ERR_OPEN_FILE;
            } else {
                if (ClkID == AMBA_VIN_SENSOR_CLOCK0) {
                    ClkFreq.id = AMBA_CLK_SENSOR0;
                    Config.id = AMBA_CLK_SENSOR0;
                    Config.config = 1U;  /* AMBA_PLL_SENSOR_REF_CLK_INPUT */
                } else if (ClkID == AMBA_VIN_SENSOR_CLOCK1) {
                    ClkFreq.id = AMBA_CLK_SENSOR1;
                    Config.id = AMBA_CLK_SENSOR1;
                    Config.config = 1U;  /* AMBA_PLL_SENSOR_CLK_1 (default source of gclk_so_vin1) */
                } else {    /* (ClkID == AMBA_VIN_SENSOR_CLOCK2) */
                    ClkFreq.id = AMBA_CLK_SENSOR2;
                    Config.id = AMBA_CLK_SENSOR2;
                    Config.config = 0U; /* Not used */
                }

                ClkFreq.freq = 0U;

                if (devctl(fd, DCMD_CLOCK_CLK_CONFIG, &Config, sizeof(Config), NULL) != EOK) {
                    RetVal = VIN_ERR_DEV_CTL;
                } else {
                    if (devctl(fd, DCMD_CLOCK_SET_FREQ, &ClkFreq, sizeof(ClkFreq), NULL) != EOK) {
                        RetVal = VIN_ERR_DEV_CTL;
                    }
                }

                close(fd);
            }

#else
            if (ClkID == AMBA_VIN_SENSOR_CLOCK0) {
                (void) AmbaRTSL_PllSetSensor0ClkDir(AMBA_PLL_SENSOR_REF_CLK_INPUT);
                RetVal = AmbaRTSL_PllSetSensor0Clk(0U);
            } else if (ClkID == AMBA_VIN_SENSOR_CLOCK1) {
                RetVal = AmbaRTSL_PllSetSensor1Clk(0U);
            } else {    /* (ClkID == AMBA_VIN_SENSOR_CLOCK2) */
                RetVal = AmbaRTSL_PllSetEthernetClk(0U);
            }
#endif

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_SensorClkSetDrvStrength - Adjust the driving strength of sensor clock
 *  @param[in] Value Indicate the strength
 *  @return error code
 */
UINT32 AmbaVIN_SensorClkSetDrvStrength(UINT32 Value)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (Value > AMBA_VIN_DRIVE_STRENGTH_12MA) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinSetSensorClkDrvStr(Value);

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_SetLvdsTermination - Adjust the impendance of LVDS pad
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] Value Indicate the impendance
 *  @return error code
 */
UINT32 AmbaVIN_SetLvdsTermination(UINT32 VinID, UINT32 Value)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) ||
        (Value > AMBA_VIN_TERMINATION_VALUE_MAX) ||
        (Value < AMBA_VIN_TERMINATION_VALUE_MIN)) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinSetLvdsTermination(VinID, Value);
#if defined(CONFIG_VIN_ASIL)
            (void)AmbaRTSL_VinSetLvdsTermination_(VinID, Value);
#endif
            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_DataLaneRemap - Remap data lane. It's used when sensor output data lanes are not connected to Ambarella chip in order
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pLaneRemapConfig Pointer to lane remapping configuration
 *  @return error code
 */
UINT32 AmbaVIN_DataLaneRemap(UINT32 VinID, const AMBA_VIN_LANE_REMAP_CONFIG_s *pLaneRemapConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pLaneRemapConfig == NULL) ||
        (VIN_ParamCheckLaneMapping(pLaneRemapConfig->NumActiveLanes, pLaneRemapConfig->pPhyLaneMapping) == VIN_ERR_ARG)) { //SLVS supports 12lane, mipi supports 8lane
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
#if defined(CONFIG_VIN_ASIL)
    } else if (AmbaSafety_VinGetSafeState(VinID, NULL) != 0U) {
        RetVal = VIN_ERR_OSERR;
#endif
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinDataLaneRemap(VinID, pLaneRemapConfig);
#if defined(CONFIG_VIN_ASIL)
            (void)AmbaRTSL_VinDataLaneRemap_(VinID, pLaneRemapConfig);
#endif

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_MipiVirtChanConfig - It's used to configure MIPI virtual channel receiving
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pMipiVirtChanConfig Pointer to MIPI virtual channel receiving configuration
 *  @return error code
 */
UINT32 AmbaVIN_MipiVirtChanConfig(UINT32 VinID, const AMBA_VIN_MIPI_VC_CONFIG_s *pMipiVirtChanConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pMipiVirtChanConfig == NULL) ||
        (pMipiVirtChanConfig->VirtChan > 3U)) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
#if defined(CONFIG_VIN_ASIL)
    } else if (AmbaSafety_VinGetSafeState(VinID, NULL) != 0U) {
        RetVal = VIN_ERR_OSERR;
#endif
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinMipiVirtChanConfig(VinID, pMipiVirtChanConfig);
#if defined(CONFIG_VIN_ASIL)
            (void)AmbaRTSL_VinMipiVirtChanConfig_(VinID, pMipiVirtChanConfig);
#endif

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_MipiEmbDataConfig - Configure MIPI embedded data capturing
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pMipiEmbDataConfig Pointer to MIPI embedded data configuration
 *  @return error code
 */
UINT32 AmbaVIN_MipiEmbDataConfig(UINT32 VinID, const AMBA_VIN_MIPI_EMB_DATA_CONFIG_s *pMipiEmbDataConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pMipiEmbDataConfig == NULL)) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
#if defined(CONFIG_VIN_ASIL)
    } else if (AmbaSafety_VinGetSafeState(VinID, NULL) != 0U) {
        RetVal = VIN_ERR_OSERR;
#endif
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinMipiEmbDataConfig(VinID, pMipiEmbDataConfig);
#if defined(CONFIG_VIN_ASIL)
            (void)AmbaRTSL_VinMipiEmbDataConfig_(VinID, pMipiEmbDataConfig);
#endif

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  BoundaryCheck - Check the boundary for mipi-phy parameters
 *  @param[in] Width bus width
 *  @param[in] Count count number
 *  @return calculated count number
 */
static UINT8 BoundaryCheck(UINT32 Width, DOUBLE Count)
{
    DOUBLE CalCount = 0.0;
    DOUBLE CountMax = 0.0;

    if (AmbaWrap_pow(2.0, (DOUBLE) Width, &CountMax) == ERR_NONE) {
        if (Count > (CountMax - 1.0)) {
            CalCount = (CountMax - 1.0);
        } else if (Count == 0.0) {
            CalCount = 0.0;
        } else {
            CalCount = Count - 1.0;
        }
    }

    return (UINT8)CalCount;
}

static UINT32 GetIdspClk(void)
{
#if defined(CONFIG_QNX)
    int fd;
    clk_freq_t ClkFreq;

    fd = open("/dev/clock", O_RDWR);

    ClkFreq.id = AMBA_CLK_IDSP;
    devctl(fd, DCMD_CLOCK_GET_FREQ, &ClkFreq, sizeof(ClkFreq), NULL);

    close(fd);

    return ClkFreq.freq;
#else
    return AmbaRTSL_PllGetIdspClk();
#endif
}

/**
 *  VIN_CalculateMphyConfig - Calculate mipi-phy parameters
 *  @param[in] BitRate MIPI data rate (DDR)
 *  @param[out] pVinMipiTiming Pointer to mipi-phy parameters
 */
static void VIN_CalculateMphyConfig(UINT64 BitRate, AMBA_VIN_MIPI_TIMING_PARAM_s *pVinMipiTiming)
{
    UINT32 RetVal;
    DOUBLE MipiBitRate;
    DOUBLE RxRefClk;
    DOUBLE TxByteClkHS, UI;
    DOUBLE ClkPrepare, ClkPrepareSec;
    DOUBLE HsSettle;
    DOUBLE HsPrepareSec;
    DOUBLE HsZeroMinSec;
    DOUBLE HsTermMax, HsTermSecMax;
    DOUBLE ClkZeroMinSec;
    DOUBLE ClkMissMax;
    DOUBLE ClkSettle;
    DOUBLE ClkTermMax;
    DOUBLE RxDDRClkHS;
    DOUBLE InitRxMin;

    UINT8 HsSettleTime;
    UINT8 HsTermTime;
    UINT8 ClkSettleTime;
    UINT8 ClkTermTime;
    UINT8 ClkMissTime;
    UINT8 RxInitTime;

    DOUBLE WorkDouble;
    UINT32 IdspClk = GetIdspClk();

    RetVal = AmbaWrap_ceil(((DOUBLE)BitRate / 1000000.0), &MipiBitRate); /* in MHz */
    RetVal |= AmbaWrap_ceil((((DOUBLE)IdspClk / 2.0) / 1000000.0), &RxRefClk); /* in MHz */

    if (RetVal == ERR_NONE) {
        /********************************************************
        * TX (TxByteClkHS)
        ********************************************************/
        TxByteClkHS = MipiBitRate * 1e6 / 8.0; // in Hz
        UI = 1.0 / (MipiBitRate * 1e6);        // in seccond

        ClkPrepareSec = (38e-9 + 95e-9) / 2.0;
        ClkPrepare = ClkPrepareSec * TxByteClkHS;
        if (AmbaWrap_floor((ClkPrepare + 0.5), &ClkPrepare) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }

        // ClkZero
        ClkZeroMinSec = 300e-9 - (ClkPrepare / TxByteClkHS);

        // HsPrepare
        HsPrepareSec = ((40e-9 + (4.0 * UI)) + (85e-9 + (6.0 * UI))) / 2.0;

        // HsZero
        HsZeroMinSec = 145e-9 + (10.0 * UI) - HsPrepareSec;

        /********************************************************
        * RX
        * (RxRefClk for clock lane)
        * (RxDDRClkHS for data lane)
        *******************************************************/
        RxRefClk = RxRefClk * 1e6;
        RxDDRClkHS = MipiBitRate * 1e6 / 4.0; // in Hz

        // InitRx
        InitRxMin = 100e-6 * RxRefClk / 1024.0;
        if (AmbaWrap_floor((InitRxMin + 0.5), &InitRxMin) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }

        RxInitTime = BoundaryCheck(7, InitRxMin);

        // ClkMiss
        ClkMissMax = 60e-9 * RxRefClk;
        if (AmbaWrap_floor((ClkMissMax + 0.5), &ClkMissMax) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }
        ClkMissTime = BoundaryCheck(5, ClkMissMax);
        /* There's a ratio requirement: RxDDRClkHS > 2.5 RxRefClk
         * 2.5 is a number with margin. When this ratio not meet, it's quit easy
         * for mipi_dphy to think sensor clock is missing (not toggling).
         * Per VLSI's information on 2019/9/12, we can increase the miss_ctrl as
         * large as we can to allow faster IDSP_clk. It just increased the time for PHY
         * to be aware of real clock disappeared, which is no effect currently. */
        WorkDouble = (DOUBLE)RxDDRClkHS / 2.5;
        WorkDouble = WorkDouble / (DOUBLE)RxRefClk;
        if (WorkDouble > 1.0) {
            ClkMissTime = BoundaryCheck(5, ClkMissMax);
        } else {
            ClkMissTime = 0x1f;
        }

        // ClkTerm
        ClkTermMax = 38e-9 * RxRefClk;
        if (AmbaWrap_floor((ClkTermMax + 0.5), &ClkTermMax) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }
        ClkTermTime = BoundaryCheck(5, ClkTermMax);

        // ClkSettle
        ClkSettle = (ClkPrepareSec + ClkZeroMinSec) * RxRefClk;
        ClkSettleTime = BoundaryCheck(6, ClkSettle);

        // HsTerm
        HsTermSecMax = 35e-9 + (4.0 * UI);
        HsTermMax = HsTermSecMax * RxDDRClkHS;
        if (AmbaWrap_floor(HsTermMax, &HsTermMax) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }
        HsTermTime = BoundaryCheck(5, HsTermMax);

        // HsSettle
        HsSettle = ((HsPrepareSec - HsTermSecMax) + HsZeroMinSec) * RxDDRClkHS;
        if (AmbaWrap_floor(HsSettle, &HsSettle) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }
        HsSettleTime = BoundaryCheck(6, HsSettle);

        if (RetVal == ERR_NONE) {
            pVinMipiTiming->HsSettleTime  = HsSettleTime;
            pVinMipiTiming->HsTermTime    = HsTermTime;
            pVinMipiTiming->ClkSettleTime = ClkSettleTime;
            pVinMipiTiming->ClkTermTime   = ClkTermTime;
            pVinMipiTiming->ClkMissTime   = ClkMissTime;
            pVinMipiTiming->RxInitTime    = RxInitTime;
        }
    }
}

#if defined(CONFIG_VIN_ASIL)
#pragma GCC push_options
#pragma GCC optimize ("O0")
/* do lock-step calculation */
static UINT8 BoundaryCheck_(UINT32 Width, DOUBLE Count)
{
    DOUBLE CalCount = 0.0;
    DOUBLE CountMax = 0.0;

    if (AmbaWrap_pow(2.0, (DOUBLE) Width, &CountMax) == ERR_NONE) {
        if (Count > (CountMax - 1.0)) {
            CalCount = (CountMax - 1.0);
        } else if (Count == 0.0) {
            CalCount = 0.0;
        } else {
            CalCount = Count - 1.0;
        }
    }

    return (UINT8)CalCount;
}

static void AmbaVIN_CalculateMphyConfig_(UINT64 BitRate, AMBA_VIN_MIPI_TIMING_PARAM_s *pVinMipiTiming)
{
    UINT32 RetVal;
    DOUBLE MipiBitRate;
    DOUBLE RxRefClk;
    DOUBLE TxByteClkHS, UI;
    DOUBLE ClkPrepare, ClkPrepareSec;
    DOUBLE HsSettle;
    DOUBLE HsPrepareSec;
    DOUBLE HsZeroMinSec;
    DOUBLE HsTermMax, HsTermSecMax;
    DOUBLE ClkZeroMinSec;
    DOUBLE ClkMissMax;
    DOUBLE ClkSettle;
    DOUBLE ClkTermMax;
    DOUBLE RxDDRClkHS;
    DOUBLE InitRxMin;

    UINT8 HsSettleTime;
    UINT8 HsTermTime;
    UINT8 ClkSettleTime;
    UINT8 ClkTermTime;
    UINT8 ClkMissTime;
    UINT8 RxInitTime;

    DOUBLE WorkDouble;

    RetVal = AmbaWrap_ceil(((DOUBLE)BitRate / 1000000.0), &MipiBitRate); /* in MHz */
    RetVal |= AmbaWrap_ceil((((DOUBLE)AmbaRTSL_PllGetIdspClk() / 2.0) / 1000000.0), &RxRefClk); /* in MHz */

    if (RetVal == ERR_NONE) {
        /********************************************************
        * TX (TxByteClkHS)
        ********************************************************/
        TxByteClkHS = MipiBitRate * 1e6 / 8.0; // in Hz
        UI = 1.0 / (MipiBitRate * 1e6);        // in seccond

        ClkPrepareSec = (38e-9 + 95e-9) / 2.0;
        ClkPrepare = ClkPrepareSec * TxByteClkHS;
        if (AmbaWrap_floor((ClkPrepare + 0.5), &ClkPrepare) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }
        // ClkZero
        ClkZeroMinSec = 300e-9 - (ClkPrepare / TxByteClkHS);

        // HsPrepare
        HsPrepareSec = ((40e-9 + (4.0 * UI)) + (85e-9 + (6.0 * UI))) / 2.0;

        // HsZero
        HsZeroMinSec = 145e-9 + (10.0 * UI) - HsPrepareSec;

        /********************************************************
        * RX
        * (RxRefClk for clock lane)
        * (RxDDRClkHS for data lane)
        *******************************************************/
        RxRefClk = RxRefClk * 1e6;
        RxDDRClkHS = MipiBitRate * 1e6 / 4.0; // in Hz

        // InitRx
        InitRxMin = 100e-6 * RxRefClk / 1024.0;
        if (AmbaWrap_floor((InitRxMin + 0.5), &InitRxMin) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }
        RxInitTime = BoundaryCheck_(7, InitRxMin);

        // ClkMiss
        ClkMissMax = 60e-9 * RxRefClk;
        if (AmbaWrap_floor((ClkMissMax + 0.5), &ClkMissMax) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }
        ClkMissTime = BoundaryCheck_(5, ClkMissMax);
        /* There's a ratio requirement: RxDDRClkHS > 2.5 RxRefClk
         * 2.5 is a number with margin. When this ratio not meet, it's quit easy
         * for mipi_dphy to think sensor clock is missing (not toggling).
         * Per VLSI's information on 2019/9/12, we can increase the miss_ctrl as
         * large as we can to allow faster IDSP_clk. It just increased the time for PHY
         * to be aware of real clock disappeared, which is no effect currently. */
        WorkDouble = (DOUBLE)RxDDRClkHS / 2.5;
        WorkDouble = WorkDouble / (DOUBLE)RxRefClk;
        if (WorkDouble > 1.0) {
            ClkMissTime = BoundaryCheck_(5, ClkMissMax);
        } else {
            ClkMissTime = 0x1f;
        }

        // ClkTerm
        ClkTermMax = 38e-9 * RxRefClk;
        if (AmbaWrap_floor((ClkTermMax + 0.5), &ClkTermMax) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }
        ClkTermTime = BoundaryCheck_(5, ClkTermMax);

        // ClkSettle
        ClkSettle = (ClkPrepareSec + ClkZeroMinSec) * RxRefClk;
        ClkSettleTime = BoundaryCheck_(6, ClkSettle);

        // HsTerm
        HsTermSecMax = 35e-9 + (4.0 * UI);
        HsTermMax = HsTermSecMax * RxDDRClkHS;
        if (AmbaWrap_floor(HsTermMax, &HsTermMax) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }
        HsTermTime = BoundaryCheck_(5, HsTermMax);

        // HsSettle
        HsSettle = ((HsPrepareSec - HsTermSecMax) + HsZeroMinSec) * RxDDRClkHS;
        if (AmbaWrap_floor(HsSettle, &HsSettle) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }
        HsSettleTime = BoundaryCheck_(6, HsSettle);

        if ((pVinMipiTiming != NULL) && (RetVal == ERR_NONE)) {
            pVinMipiTiming->HsSettleTime  = HsSettleTime;
            pVinMipiTiming->HsTermTime    = HsTermTime;
            pVinMipiTiming->ClkSettleTime = ClkSettleTime;
            pVinMipiTiming->ClkTermTime   = ClkTermTime;
            pVinMipiTiming->ClkMissTime   = ClkMissTime;
            pVinMipiTiming->RxInitTime    = RxInitTime;
        }
    }
}
#pragma GCC pop_options
#endif

/**
 *  AmbaVIN_DelayedVSyncEnable - Generate Delayed HSYNC/VSYNC output control (Only Delay Period can be changed after first time config)
 *  @param[in] pDelayedVSyncConfig Pointer to DelayedVSync configuration
 *  @return error code
 */
UINT32 AmbaVIN_DelayedVSyncEnable(const AMBA_VIN_DELAYED_VSYNC_CONFIG_s *pDelayedVSyncConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 RefClk;
    UINT32 VsdelaySrc;

    if ((pDelayedVSyncConfig == NULL) || (VIN_ParamCheckDelayedVSync(pDelayedVSyncConfig) == VIN_ERR_ARG)) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
    } else {
        RefClk = pDelayedVSyncConfig->FineAdjust.RefClk;
        VsdelaySrc = pDelayedVSyncConfig->DelayedHVsyncSource;

        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {
            /* If not fine adjust, then no need to config clock */
            if (RefClk != 0U) {
#if defined(CONFIG_QNX)
                if ((VsdelaySrc == AMBA_VIN_VSDLY_SRC_HV_MSYNC0) || (VsdelaySrc == AMBA_VIN_VSDLY_SRC_H_MSYNC0_V_EXT)) {
                    /* Config sensor clock0 */
                    if (RefClk != GetVinClk(AMBA_VIN_CHANNEL0)) {
                        RetVal = SetSensorClk(AMBA_VIN_SENSOR_CLOCK0, RefClk);
                    }
                } else {    /* AMBA_VIN_VSDLY_SRC_HV_MSYNC1 or AMBA_VIN_VSDLY_SRC_H_MSYNC1_V_EXT */
                    /* Config sensor clock1 */
                    if (RefClk != GetVinClk(AMBA_VIN_CHANNEL1)) {
                        RetVal = SetSensorClk(AMBA_VIN_SENSOR_CLOCK1, RefClk);
                    }
                    if (RetVal == 0) {
                        RetVal = SetMsync1ClkSrc(1);
                    }
                }
#else
                if ((VsdelaySrc == AMBA_VIN_VSDLY_SRC_HV_MSYNC0) || (VsdelaySrc == AMBA_VIN_VSDLY_SRC_H_MSYNC0_V_EXT)) {
                    /* Config sensor clock0 */
                    if (RefClk != AmbaRTSL_PllGetVin0Clk()) {
                        RetVal |= AmbaRTSL_PllSetSensor0Clk(RefClk);
                    }

                } else {    /* AMBA_VIN_VSDLY_SRC_HV_MSYNC1 or AMBA_VIN_VSDLY_SRC_H_MSYNC1_V_EXT */
                    /* Config sensor clock1 */
                    if (RefClk != AmbaRTSL_PllGetVin1Clk()) {
                        RetVal |= AmbaRTSL_PllSetSensor1Clk(RefClk);
                    }
                    if (RetVal == PLL_ERR_NONE) {
                        (void)AmbaRTSL_PllSetVin1ClkConfig(AMBA_PLL_SENSOR_CLK_1);
                    }

                }
#endif
            }

            RetVal |= AmbaRTSL_VinDelayedVSyncEnable(pDelayedVSyncConfig);

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_DelayedVSyncDisable - Disable Delayed HSYNC/VSYNC output (Previous state and configurations will be kept)
 *  @return error code
 */
UINT32 AmbaVIN_DelayedVSyncDisable(void)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinDelayedVSyncDisable();

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_GetMainCfgBufInfo - Get VIN main buffer address
 *  @param[in] VinID Indicate VIN channel
 *  @param[out] pAddr Pointer to receive the VIN main buffer address
 *  @param[out] pSize Pointer to receive the VIN main buffer size
 *  @return error code
 */
UINT32 AmbaVIN_GetMainCfgBufInfo(UINT32 VinID, ULONG *pAddr, UINT32 *pSize)
{
    UINT32 RetVal;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pAddr == NULL) || (pSize == NULL)) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
    } else {
        RetVal = AmbaCSL_VinGetMainCfgBufAddr(VinID, pAddr);

        if (RetVal == VIN_ERR_NONE) {
            *pSize = AMBA_VIN_MAIN_BUF_SIZE;
        } else {
            *pSize = 0U;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_CaptureConfig - Configure Vin capture window
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pCaptureWindow Pointer to VIN capture window
 *  @return error code
 */
UINT32 AmbaVIN_CaptureConfig(UINT32 VinID, const AMBA_VIN_WINDOW_s *pCaptureWindow)
{
    UINT32 RetVal = VIN_ERR_NONE;

    const AMBA_CSL_VIN_WINDOW_s *pCslCaptureWindow = NULL;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pCaptureWindow == NULL) ||
        (pCaptureWindow->Width == 0U) || (pCaptureWindow->Height == 0U)) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
#if defined(CONFIG_VIN_ASIL)
    } else if (AmbaSafety_VinGetSafeState(VinID, NULL) != 0U) {
        RetVal = VIN_ERR_OSERR;
#endif
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            AmbaMisra_TypeCast(&pCslCaptureWindow, &pCaptureWindow);

            RetVal = AmbaCSL_VinCaptureConfig(VinID, pCslCaptureWindow);
#if defined(CONFIG_VIN_ASIL)
            (void)AmbaCSL_VinCaptureConfig_(VinID, pCslCaptureWindow);
#endif
            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_GetInfo - Get Vin info
 *  @param[in] VinID Indicate VIN channel
 *  @param[out] pInfo Pointer to VIN info
 *  @return error code
 */
UINT32 AmbaVIN_GetInfo(UINT32 VinID, AMBA_VIN_INFO_s *pInfo)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_INFO_s CslVinInfo;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pInfo == NULL)) {
        RetVal = VIN_ERR_ARG;
    } else if (VIN_DrvInited == 0U) {
        RetVal = VIN_ERR_UNEXPECTED;
    } else {
        RetVal = AmbaCSL_VinGetInfo(VinID, &CslVinInfo);

        if (RetVal == VIN_ERR_NONE) {
            pInfo->BayerPattern  = CslVinInfo.BayerPattern;
            pInfo->ColorSpace    = CslVinInfo.ColorSpace;
            pInfo->FrameRate     = CslVinInfo.FrameRate;
            pInfo->NumDataBits   = CslVinInfo.NumDataBits;
            pInfo->NumSkipFrame  = CslVinInfo.NumSkipFrame;
            pInfo->YuvOrder      = CslVinInfo.YuvOrder;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_IsEnabled - Get Vin enable status
 *  @param[in] VinID Indicate VIN channel
 *  @return vin enable status
 */
UINT32 AmbaVIN_IsEnabled(UINT32 VinID)
{
    UINT32 Enable = 0U;

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        Enable = 0U;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            Enable = AmbaCSL_VinIsEnabled(VinID);
            /*
             * Release the Mutex
             */
            (void)AmbaKAL_MutexGive(&AmbaVinMutex);
        }
    }

    return Enable;
}

