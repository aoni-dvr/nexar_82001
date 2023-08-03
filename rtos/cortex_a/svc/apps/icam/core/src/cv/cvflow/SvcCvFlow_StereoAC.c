/**
 *  @file SvcCvFlow_StereoAC.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *
 *  @details Implementation of Stereo Auto Calibration driver
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event.h"
#include "AmbaMMU.h"
#include "AmbaDef.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"
#include "AmbaCache.h"
#include "AmbaFS.h"

/* svc-shared */
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcTask.h"
#include "SvcCvAlgo.h"
#include "SvcCvFlowProfile.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowUtil.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvFlow_StereoAC.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcImgFIFO.h"
#include "SvcImgFeeder.h"
#include "SvcOsd.h"
#include "SvcWrap.h"

#include "cvapi_flexidag_ambamvac_cv2.h"
#include "cvapi_flexidag.h"
#include "AmbaSTU_AlgoTaskUtil.h"
#include "AmbaAcRef_Flow.h"

#ifdef CONFIG_AMBALINK_BOOT_OS
#include "AmbaIPC_FlexidagIO.h"
#include "SvcCvFlexidagIO.h"
#endif

#define SVC_LOG_STEREO_AC        "StAC"

#define SVC_STEREO_AC_INSTANCE0            (0U)
#define SVC_STEREO_AC_INSTANCE1            (1U)
#define SVC_STEREO_AC_MAX_INSTANCE         (2U)

#define SVC_STEREO_AC_SEM_INIT_COUNT       (3U)
#define SVC_STEREO_AC_OUTPUT_DEPTH         (SVC_STEREO_AC_SEM_INIT_COUNT + 1U)

#define SVC_STEREO_AC_RX_TASK_STACK_SIZE   (0x80000)
#define SVC_STEREO_AC_PICINFO_QUEUE_SIZE   (2U)
#define SVC_STEREO_AC_ALGO_OUT_QUEUE_SIZE  (SVC_STEREO_AC_OUTPUT_DEPTH)

#define SRC_RATE_FRACTION_MODE_BIT         (0x10000000U)

typedef UINT32 (*SVC_STEREO_AC_DATA_HANDLER_f)(UINT32 Inst);
typedef UINT32 (*SVC_STEREO_AC_TX_HANDLER_f)(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

typedef struct {
    UINT8                       MetaDataFlag;
    AppAcMetadata_t             MetaData;
} SVC_CV_FLOW_STEREO_AC_OP_EX_INFO_s;

typedef struct {
    UINT8                       Used;
    UINT8                       CvfChan;
    UINT8                       Enable;
    UINT8                       Started;
    SVC_CV_FLOW_CFG_s           *pCvFlow;
    SVC_CV_FLOW_CFG_s           CvCfg;

    SVC_CV_FLOW_ALGO_OP_INFO_s  AlgoOp;
    SVC_CV_FLOW_STEREO_AC_OP_EX_INFO_s AlgoOpEx;
    void                       *pImgInfoBuf[SVC_STEREO_AC_PICINFO_QUEUE_SIZE];
    UINT32                      ImgInfoBufSize;

    /* Data source service */
    SVC_IMG_FEEDER_PORT_s       FeederPort;
    UINT32                      SrcSeqNum;
    SVC_STEREO_AC_TX_HANDLER_f  pTxProc;
    UINT32                      TxSeqNum;
    UINT32                      PicInfoIdx;

    /* Data sink service */
    SVC_TASK_CTRL_s             RxTaskCtrl;
    SVC_STEREO_AC_DATA_HANDLER_f pRxProc;
    UINT32                      RxSeqNum;
    UINT32                      OutBufIdx;
    AMBA_KAL_SEMAPHORE_t        OutBufSem;
    UINT32                      SinkInfoIdx;

    /* Algo Callback */
    AMBA_KAL_MSG_QUEUE_t        AlgoOutQueId;
    void                       *pMVACOutBuf[SVC_STEREO_AC_OUTPUT_DEPTH];
    UINT32                      MVACOutBufSize;
    UINT32                      MVACOutBufIdx;
    void                       *pWarpOutBuf[SVC_STEREO_AC_OUTPUT_DEPTH];
    UINT32                      WarpOutBufSize;

    /* Misc */
    UINT32                      SrcRateCtrl;
#define                         STEREO_AC_SRC_RATE_MAX_DENOMINATOR  (16U)
    UINT8                       SrcValidTbl[STEREO_AC_SRC_RATE_MAX_DENOMINATOR];
    UINT8                       SrcRateNum;
    UINT8                       SrcRateDen;
    UINT8                       SrcInfoErr;
    UINT8                       SceneMode;
    UINT32                      FlexiIOChan;    /* for AmbaLink CV only */
} SVC_STEREO_AC_CTRL_s;

typedef struct {
   UINT32 Event;
#define SVC_STEREO_AC_FINISH               (0x0U)
   AMBA_CV_MVAC_DATA_s *pAlgoOut;
} STEREO_AC_OUTPUT_MSG_s;

static SVC_STEREO_AC_CTRL_s g_StereoACCtrl[SVC_STEREO_AC_MAX_INSTANCE] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MUTEX_t StereoACMutex;
static UINT8 SvcCvDebugEnable = 0;

static UINT32 CvFlow_StereoACInit(UINT32 CvFlowChan, void *pInit);
static UINT32 CvFlow_StereoACConfig(UINT32 CvFlowChan, void *pConfig);
static UINT32 CvFlow_StereoACStart(UINT32 CvFlowChan, void *pInput);
static UINT32 CvFlow_StereoACControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo);
static UINT32 CvFlow_StereoACStop(UINT32 CvFlowChan, void *pInput);
static UINT32 StereoAC_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst);
static UINT32 StereoAC_Config(UINT32 Inst, AmbaAcConfig_t *pAcConfig);

static UINT32 StereoAC_TxDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 StereoAC_TxDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

static void   StereoAC_AlgoOutCallback0(void *pResult);
static void   StereoAC_AlgoOutCallback1(void *pResult);

static void SVC_CV_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcCvDebugEnable > 0U) {
        SvcLog_DBG(pModule, pFormat, Arg1, Arg2);
    }
}

static void StereoAC_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&StereoACMutex, 5000)) {
        SvcLog_NG(SVC_LOG_STEREO_AC, "StereoAC_MutexTake: timeout", 0U, 0U);
    }
}

static void StereoAC_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&StereoACMutex)) {
        SvcLog_NG(SVC_LOG_STEREO_AC, "StereoAC_MutexGive: error", 0U, 0U);
    }
}

static UINT32 StereoAC_SemTake(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreTake(&g_StereoACCtrl[Inst].OutBufSem, 10000)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC, "StereoAC_SemTake[%d]: timeout", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 StereoAC_SemGive(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreGive(&g_StereoACCtrl[Inst].OutBufSem)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC, "StereoAC_SemGive[%d]: error", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 StereoAC_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst)
{
    UINT32 Rval = SVC_NG;
    UINT32 i;

    if (NULL != pInst) {
        for (i = 0; i < SVC_STEREO_AC_MAX_INSTANCE; i++) {
            if ((1U == g_StereoACCtrl[i].Used) && (CvfChan == g_StereoACCtrl[i].CvfChan)) {
                *pInst = i;
                Rval = SVC_OK;
                break;
            }
        }
    }
    return Rval;
}

static void StereoAC_SaveBin(const char *pFileName, void *pBuf, const UINT32 Size)
{
    UINT32 Ret;
    AMBA_FS_FILE *pFile;
    UINT32 OpRes;

    Ret = AmbaFS_FileOpen(pFileName, "wb", &pFile);
    if(Ret != 0U) {
        AmbaPrint_PrintUInt5("_WriteOutputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
        SvcLog_NG(SVC_LOG_STEREO_AC, "AmbaFS_FileOpen failed", 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileWrite(pBuf, 1U, Size, pFile, &OpRes);
        if(Ret != 0U) {
            SvcLog_NG(SVC_LOG_STEREO_AC, "AmbaFS_FileWrite failed", 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(pFile);
        if(Ret != 0U) {
            SvcLog_NG(SVC_LOG_STEREO_AC, "AmbaFS_FileClose failed", 0U, 0U);
        }
    }
}

#ifdef CONFIG_AMBALINK_BOOT_OS
static UINT32 StereoAC_Init_Callback(UINT32 Channel, UINT32 OutType)
{
    UINT32 Rval = SVC_OK;

    (void) Channel;
    (void) OutType;
    AmbaPrint_PrintStr5("StereoAC_Init_Callback", NULL, NULL, NULL, NULL, NULL);

    return Rval;
}
#endif

#if 0
/* Pack image info at scale 0 */
static UINT32 StereoAC_PackPicInfo_PyramidScale0(cv_pic_info_t *pPicInfo, const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo, UINT32 MainWidth, UINT32 MainHeight, UINT32 UseRelative, UINT32 LeftRight)
{
    #define LEFT_CHAN               (0x1U)
    #define RIGHT_CHAN              (0x2U)
    UINT32 RetVal = SVC_OK;
    UINT32 i, DataChan;
    ULONG  BufPhyAddr, PicInfoAddr = 0U, PicInfoPhyAddr = 0U;

    AmbaMisra_TouchUnused(&MainWidth);
    AmbaMisra_TouchUnused(&MainHeight);

    if ((pPicInfo != NULL) && (pYuvInfo != NULL)) {
        /* Important: The addresses for FlexiDAG are relative to picinfo. */
        if (UseRelative == 1U) {
            AmbaMisra_TypeCast(&PicInfoAddr, &pPicInfo);
            if (MMU_ERR_NONE != AmbaMMU_VirtToPhys(PicInfoAddr, &PicInfoPhyAddr)) {
                SvcLog_NG(SVC_LOG_STEREO_AC, "PicInfoAddr V2P error", 0U, 0U);
                RetVal |= SVC_NG;
            }
        }

        if (LeftRight == 0U) { /* Duplicate the info */
            DataChan = (LEFT_CHAN | RIGHT_CHAN);
        } else {
            DataChan = LeftRight;
        }

        pPicInfo->capture_time = (UINT32) pYuvInfo->CapPts;
        pPicInfo->channel_id   = (UINT8)  pYuvInfo->ViewZoneId;
        pPicInfo->frame_num    = (UINT32) pYuvInfo->CapSequence;

        for (i = 0U; i < AMBA_DSP_MAX_HIER_NUM; i++) {
            if (((pYuvInfo->Config.HierBit >> i) & 0x1U) == 1U) {
                pPicInfo->pyramid.image_width_m1  = pYuvInfo->YuvBuf[i].Window.Width - 1U;;
                pPicInfo->pyramid.image_height_m1 = pYuvInfo->YuvBuf[i].Window.Height - 1U;
                pPicInfo->pyramid.image_pitch_m1  = pYuvInfo->YuvBuf[i].Pitch - 1U;

                pPicInfo->pyramid.half_octave[0].ctrl.roi_pitch = pYuvInfo->YuvBuf[i].Pitch;
                pPicInfo->pyramid.half_octave[0].roi_start_col  = (INT16) pYuvInfo->YuvBuf[i].Window.OffsetX;
                pPicInfo->pyramid.half_octave[0].roi_start_row  = (INT16) pYuvInfo->YuvBuf[i].Window.OffsetY;
                pPicInfo->pyramid.half_octave[0].roi_width_m1   = pYuvInfo->YuvBuf[i].Window.Width - 1U;
                pPicInfo->pyramid.half_octave[0].roi_height_m1  = pYuvInfo->YuvBuf[i].Window.Height - 1U;

                if ((DataChan & LEFT_CHAN) > 0U) {
                    if (MMU_ERR_NONE != AmbaMMU_VirtToPhys(pYuvInfo->YuvBuf[i].BaseAddrY, &BufPhyAddr)) {
                        SvcLog_NG(SVC_LOG_STEREO_AC, "BaseAddrY V2P error0", 0U, 0U);
                        RetVal |= SVC_NG;
                    }
                    pPicInfo->rpLumaLeft[0] = BufPhyAddr - PicInfoPhyAddr;
                    if (MMU_ERR_NONE != AmbaMMU_VirtToPhys(pYuvInfo->YuvBuf[i].BaseAddrUV, &BufPhyAddr)) {
                        SvcLog_NG(SVC_LOG_STEREO_AC, "BaseAddrUV V2P error0", 0U, 0U);
                        RetVal |= SVC_NG;
                    }
                    pPicInfo->rpChromaLeft[0] = BufPhyAddr - PicInfoPhyAddr;
                }

                if ((DataChan & RIGHT_CHAN) > 0U) {
                    if (MMU_ERR_NONE != AmbaMMU_VirtToPhys(pYuvInfo->YuvBuf[i].BaseAddrY, &BufPhyAddr)) {
                        SvcLog_NG(SVC_LOG_STEREO_AC, "BaseAddrY V2P error1", 0U, 0U);
                        RetVal |= SVC_NG;
                    }
                    pPicInfo->rpLumaRight[0]   = BufPhyAddr - PicInfoPhyAddr;
                    if (MMU_ERR_NONE != AmbaMMU_VirtToPhys(pYuvInfo->YuvBuf[i].BaseAddrUV, &BufPhyAddr)) {
                        SvcLog_NG(SVC_LOG_STEREO_AC, "BaseAddrUV V2P error1", 0U, 0U);
                        RetVal |= SVC_NG;
                    }
                    pPicInfo->rpChromaRight[0] = BufPhyAddr - PicInfoPhyAddr;
                }

                //AmbaPrint_PrintUInt5("Pyramid: 0x%x 0x%x", pYuvInfo->YuvBuf[i].BaseAddrY, pYuvInfo->YuvBuf[i].BaseAddrUV, 0U, 0U, 0U);
                break;
            }
        }

        for (i = 0; i < AMBA_DSP_MAX_HIER_NUM; i++) {
            if ((pPicInfo->rpLumaRight[i] != 0U) &&
                (pPicInfo->rpLumaLeft[i] != 0U) &&
                (pPicInfo->rpLumaRight[i] != pPicInfo->rpLumaLeft[i])) {
                pPicInfo->pyramid.half_octave[i].ctrl.disable = 0U;
                pPicInfo->pyramid.half_octave[i].ctrl.mode    = 1U;
            } else if ((pPicInfo->rpLumaRight[i] != 0U) || (pPicInfo->rpLumaLeft[i] != 0U)) {
                pPicInfo->pyramid.half_octave[i].ctrl.disable = 0U;
                pPicInfo->pyramid.half_octave[i].ctrl.mode    = 0U;
            } else {
                pPicInfo->pyramid.half_octave[i].ctrl.disable = 1U;
                pPicInfo->pyramid.half_octave[i].ctrl.mode    = 0U;
            }
        }

        for (i = AMBA_DSP_MAX_HIER_NUM; i < MAX_HALF_OCTAVES; i++) {
            pPicInfo->pyramid.half_octave[i].ctrl.disable = 1U;
            pPicInfo->pyramid.half_octave[i].ctrl.mode    = 0U;
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC, "PackPicInfo_Pyramid: invalid info", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}
#endif

static void StereoAC_DumpPicInfo(const cv_pic_info_t *pPicInfo)
{
    static UINT32 DumpTimes = 1U;
    extern void SvcCvFlow_DumpPicInfo(const char *pModuleName, const cv_pic_info_t *pPicInfo);

    if (DumpTimes > 0U) {
        SvcCvFlow_DumpPicInfo(SVC_LOG_STEREO_AC, pPicInfo);
        DumpTimes--;
    }
}

/**
* Pack data source to pic info structure
*/
static void StereoAC_PackPicInfo(UINT32 Inst, const SVC_CV_INPUT_IMG_INFO_s *pInputImg, memio_source_recv_picinfo_t *pInfo)
{
    UINT32 Rval = SVC_OK;
    cv_pic_info_t *pPicInfo = &pInfo->pic_info;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    const SVC_STEREO_AC_CTRL_s *pCtrl = &g_StereoACCtrl[Inst];
    UINT32 DataSrc, RelativeAddrOn = 1U;
    extern UINT32 SvcCvFlow_PackPicInfo_MainYuv(cv_pic_info_t *pPicInfo, const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo, UINT32 UseRelative, UINT32 LeftRight);
    extern UINT32 SvcCvFlow_PackPicInfo_Pyramid(cv_pic_info_t *pPicInfo, const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo, UINT32 MainWidth, UINT32 MainHeight, UINT32 UseRelative, UINT32 LeftRight);
    UINT32 i;

    Rval = AmbaWrap_memset(pInfo, 0, sizeof(memio_source_recv_picinfo_t));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STEREO_AC, "memset picinfo failed", 0U, 0U);
    }

    DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
    if (pInputImg->NumInfo > 0U) {
        if (SvcCvFlow_StereoAC_ModeInfo[pCtrl->pCvFlow->CvModeID].ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
            RelativeAddrOn = 0U;
        }

        if ((DataSrc == SVC_CV_DATA_SRC_MAIN_YUV) ||
            (DataSrc == SVC_CV_DATA_SRC_YUV_STRM)) {
            const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo;
            AmbaMisra_TypeCast(&pYuvInfo, &pInputImg->Info[0].pBase);

            Rval = SvcCvFlow_PackPicInfo_MainYuv(pPicInfo, pYuvInfo, RelativeAddrOn, 0U);
        } else {
            const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfoLeft;
            const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfoRight;
            AmbaMisra_TypeCast(&pYuvInfoLeft, &pInputImg->Info[0].pBase);
            AmbaMisra_TypeCast(&pYuvInfoRight, &pInputImg->Info[1].pBase);

            Rval |= SvcCvFlow_PackPicInfo_Pyramid(pPicInfo,
                                                  pYuvInfoLeft,
                                                  pCfg->FovCfg[pYuvInfoLeft->ViewZoneId].MainWin.Width,
                                                  pCfg->FovCfg[pYuvInfoLeft->ViewZoneId].MainWin.Height,
                                                  RelativeAddrOn, 1U);
            Rval |= SvcCvFlow_PackPicInfo_Pyramid(pPicInfo,
                                                  pYuvInfoRight,
                                                  pCfg->FovCfg[pYuvInfoRight->ViewZoneId].MainWin.Width,
                                                  pCfg->FovCfg[pYuvInfoRight->ViewZoneId].MainWin.Height,
                                                  RelativeAddrOn, 2U);
        }
    } else {
        Rval = SVC_NG;
    }

    /* Pass necessary scale only */
    for (i = 0; i < MAX_HALF_OCTAVES; i++) {
        if (i != pCtrl->pCvFlow->InputCfg.Input[0].RoiInfo.Roi[0].Index) {
            pPicInfo->pyramid.half_octave[i].ctrl.disable = 1U;
        }
    }

    StereoAC_DumpPicInfo(pPicInfo);

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STEREO_AC, "StereoAC_PackPicInfo error", 0U, 0U);
    }
}

/**
* Send message to Feeder to update port status
*/
static void StereoAC_SendReceiverReady(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort)
{
    SVC_STEREO_AC_CTRL_s *pCtrl = &g_StereoACCtrl[Inst];
    AMBA_KAL_SEMAPHORE_INFO_s SemInfo;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreQuery(&pCtrl->OutBufSem, &SemInfo)) {
        if (SemInfo.CurrentCount > 0U) {
            if (SVC_OK != SvcImgFeeder_MsgToFeeder(pPort, SVC_IMG_FEEDER_RECEIVER_READY, NULL)) {
                SvcLog_NG(SVC_LOG_STEREO_AC, "Send ReceiverReady[%u] error", Inst, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC, "ReceiverReady[%u]: SemQuery error", Inst, 0U);
    }
}

/**
* TX data source handler: process input data
*/
static UINT32 StereoAC_TxDataSrcHandler(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_STEREO_AC_CTRL_s *pCtrl = &g_StereoACCtrl[Inst];
    const SVC_CV_INPUT_IMG_INFO_s *pInputImg;
    memio_source_recv_picinfo_t *pPicInfo;
    AppAcPicInfo_t AppAcPicInfo;

    AmbaMisra_TouchUnused(pInfo);

    SVC_CV_DBG(SVC_LOG_STEREO_AC, "Tx[%u]: run %d", Inst, pCtrl->TxSeqNum);

    if (Event == SVC_IMG_FEEDER_IMAGE_DATA_READY) {
        if ((pCtrl->Enable == 1U) &&
            (0U == (pCtrl->SrcSeqNum % pCtrl->SrcRateCtrl))) {
            Rval = StereoAC_SemTake(Inst);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pInputImg, &pInfo);
                AmbaMisra_TypeCast(&pPicInfo, &pCtrl->pImgInfoBuf[pCtrl->PicInfoIdx]);

                StereoAC_PackPicInfo(Inst, pInputImg, pPicInfo);

                SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STEREO_AC, pPicInfo, pCtrl->ImgInfoBufSize, &AppAcPicInfo.PicInfo);
                Rval = RefCV_AmbaAcFeedOneFrame(pPicInfo->pic_info.frame_num, &AppAcPicInfo, &pCtrl->AlgoOpEx.MetaData);
                SVC_CV_DBG(SVC_LOG_STEREO_AC, "Tx[%u]: Send(%u)", Inst, pPicInfo->pic_info.frame_num);

                if (SVC_OK == Rval) {
                    pCtrl->TxSeqNum++;
                    pCtrl->PicInfoIdx++;
                    if (pCtrl->PicInfoIdx >= SVC_STEREO_AC_PICINFO_QUEUE_SIZE) {
                        pCtrl->PicInfoIdx = 0;
                    }
                } else {
                    SvcLog_NG(SVC_LOG_STEREO_AC, "Tx[%u]: send error", Inst, 0U);
                }

                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    StereoAC_SendReceiverReady(Inst, pPort);
                }
            } else {
                SvcLog_NG(SVC_LOG_STEREO_AC, "Tx[%u]: SemTake error", Inst, 0U);
            }
        } else {
            if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                StereoAC_SendReceiverReady(Inst, pPort);
                Rval = AmbaKAL_TaskSleep(1);
            }
        }

        pCtrl->SrcSeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC, "Tx[%u]: unsupported event(0x%x)", Inst, Event);
        Rval = SVC_NG;
    }

    SVC_CV_DBG(SVC_LOG_STEREO_AC, "Tx[%u]: end", Inst, 0U);

    return Rval;
}

static UINT32 StereoAC_TxDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_STEREO_AC_CTRL_s *pCtrl = &g_StereoACCtrl[SVC_STEREO_AC_INSTANCE0];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_STEREO_AC_INSTANCE0, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 StereoAC_TxDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_STEREO_AC_CTRL_s *pCtrl = &g_StereoACCtrl[SVC_STEREO_AC_INSTANCE1];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_STEREO_AC_INSTANCE1, pPort, Event, pInfo);
    }
    return Rval;
}

static void StereoAC_SendACData(UINT32 Inst, const AMBA_CV_MVAC_DATA_s *pAcResult)
{
    UINT32 Rval;
    SVC_STEREO_AC_CTRL_s *pCtrl = &g_StereoACCtrl[Inst];
    STEREO_AC_OUTPUT_MSG_s Msg;

    /* Copy MVAC data */
    Rval = AmbaWrap_memcpy(pCtrl->pMVACOutBuf[pCtrl->MVACOutBufIdx],
                           pAcResult,
                           sizeof(AMBA_CV_MVAC_DATA_s));
    if (SVC_OK == Rval) {
        /* Copy warp info */
        Rval = AmbaWrap_memcpy(pCtrl->pWarpOutBuf[pCtrl->MVACOutBufIdx],
                               pAcResult->pWarpInfo,
                               sizeof(AMBA_CV_MVAC_WARP_INFO_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STEREO_AC, "memcpy Warp info failed", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC, "memcpy MVAC Data failed", 0U, 0U);
    }

    Msg.Event = SVC_STEREO_AC_FINISH;
    AmbaMisra_TypeCast(&Msg.pAlgoOut, &pCtrl->pMVACOutBuf[pCtrl->MVACOutBufIdx]);
    AmbaMisra_TypeCast(&Msg.pAlgoOut->pWarpInfo, &pCtrl->pWarpOutBuf[pCtrl->MVACOutBufIdx]);

    Rval = AmbaKAL_MsgQueueSend(&pCtrl->AlgoOutQueId, &Msg, 5000);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STEREO_AC, "StereoAC_AlgoOutCallback[%u]: MsgQueueSend error", Inst, 0U);
    }

    pCtrl->MVACOutBufIdx++;
    if (pCtrl->MVACOutBufIdx >= SVC_STEREO_AC_OUTPUT_DEPTH) {
        pCtrl->MVACOutBufIdx = 0;
    }
}

/**
* Algorithm output event callback function
*/
static void StereoAC_AlgoOutCallback(UINT32 Inst, void *pResult)
{
    UINT32 Rval = SVC_OK;
    SVC_STEREO_AC_CTRL_s *pCtrl = &g_StereoACCtrl[Inst];
    const AMBA_CV_MVAC_DATA_s *pAcResult;
    //UINT16 *pWarpTbl;
    static UINT8 PrintCount = 0U;
    static UINT8 SvcCvDebugDump = 1;

    AmbaMisra_TouchUnused(&Inst);
    AmbaMisra_TouchUnused(pResult);

    AmbaMisra_TypeCast(&pAcResult, &pResult);

    switch (pAcResult->State) {
        case AMBA_CV_MVAC_STATE_RESET:
            SvcLog_OK(SVC_LOG_STEREO_AC, "## state: RESET", 0U, 0U);
            PrintCount = 0U;
            break;
        case AMBA_CV_MVAC_STATE_WAIT:
            SvcLog_OK(SVC_LOG_STEREO_AC, "## state: WAIT", 0U, 0U);
            break;
        case AMBA_CV_MVAC_STATE_ACCUMULATION:
            if (PrintCount < 5U) {
                SvcLog_OK(SVC_LOG_STEREO_AC, "## state: ACCUMULATION", 0U, 0U);
            }
            PrintCount++;
            break;
        case AMBA_CV_MVAC_STATE_SOLVE:
            SvcLog_OK(SVC_LOG_STEREO_AC, "## state: SOLVE", 0U, 0U);
            if (pAcResult->pWarpInfo != NULL) {
                SvcLog_DBG(SVC_LOG_STEREO_AC, "generate warp table at MVAC_STATE_SOLVE", 0U, 0U);

                StereoAC_SendACData(Inst, pAcResult);

                if (SvcCvDebugDump == 1U) {
                    if (pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTableValid == 1U) {
                        //pWarpTbl = &(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable[0]);
                        StereoAC_SaveBin("c:\\ac_warp_left.bin", &pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable,
                                    sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable));
                    }
                }
                RefCV_AmbaAc_ProfilerReport();
            }
            break;
        case AMBA_CV_MVAC_STATE_END:
            SvcLog_OK(SVC_LOG_STEREO_AC, "## state: END", 0U, 0U);
            if (pAcResult->pWarpInfo != NULL) {
                SvcLog_DBG(SVC_LOG_STEREO_AC, "generate warp table at MVAC_STATE_END", 0U, 0U);

                StereoAC_SendACData(Inst, pAcResult);

                if (SvcCvDebugDump == 1U) {
                    if (pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTableValid == 1U) {
                        //pWarpTbl = &(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable[0]);
                        StereoAC_SaveBin("c:\\ac_warp_left.bin", &pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable,
                                    sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable));
                    }
                }
                RefCV_AmbaAc_ProfilerReport();
            }
            break;
        case AMBA_CV_MVAC_STATE_SKIP:
            SvcLog_OK(SVC_LOG_STEREO_AC, "## state: SKIP", 0U, 0U);
            break;
        default:
            SvcLog_NG(SVC_LOG_STEREO_AC, "## callback at state: ERROR", 0U, 0U);
            break;
    }

    Rval = StereoAC_SemGive(Inst);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STEREO_AC, "StereoAC_AlgoOutCallback[%u]: SemGive error", Inst, 0U);
    }

    if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
        StereoAC_SendReceiverReady(Inst, &pCtrl->FeederPort);
    }
}

static void StereoAC_AlgoOutCallback0(void *pResult)
{
    StereoAC_AlgoOutCallback(SVC_STEREO_AC_INSTANCE0, pResult);
}

static void StereoAC_AlgoOutCallback1(void *pResult)
{
    StereoAC_AlgoOutCallback(SVC_STEREO_AC_INSTANCE1, pResult);
}

/**
* RX data sink handler: process detection output
*/
static UINT32 StereoAC_RxDataSinkHandler(UINT32 Inst)
{
    UINT32 Rval;
    SVC_STEREO_AC_CTRL_s *pCtrl = &g_StereoACCtrl[Inst];
    STEREO_AC_OUTPUT_MSG_s Msg;
    SVC_CV_DATA_OUTPUT_s Output;
    AmbaAcConfig_t AcConfig;

    SVC_CV_DBG(SVC_LOG_STEREO_AC, "Rx[%u]: run %d", Inst, pCtrl->RxSeqNum);

    Rval = AmbaKAL_MsgQueueReceive(&pCtrl->AlgoOutQueId, &Msg, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK == Rval) {
        SVC_CV_DBG(SVC_LOG_STEREO_AC, "Rx[%u]: recv out(%u)", Inst, 0);

        if (Msg.Event == SVC_STEREO_AC_FINISH) {
            if (pCtrl->Enable == 1U) {
                Output.MsgCode = SVC_CV_STEREO_AUTO_CALIB;
                Output.Source = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
                Output.NumData = 1U;
                Output.Data[0U].pBuf = Msg.pAlgoOut;
                Output.Data[0U].BufSize = sizeof(AMBA_CV_MVAC_DATA_s);
                Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                       SVC_CV_FLOW_OUTPUT_PORT0,
                                       pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                       &Output);

                pCtrl->Enable = 0U;
                Rval = SvcImgFeeder_Disconnect(&pCtrl->FeederPort);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STEREO_AC, "Disconn feeder port error", 0U, 0U);
                } else {
                    SvcLog_OK(SVC_LOG_STEREO_AC, "Disconn feeder Done", 0U, 0U);
                    AmbaKAL_TaskSleep(30000);
                    SvcLog_OK(SVC_LOG_STEREO_AC, "StereoAC_Config start", 0U, 0U);
                    Rval = StereoAC_Config(Inst, &AcConfig);
                    SvcLog_OK(SVC_LOG_STEREO_AC, "RefCV_AmbaAc_MvacRestart start", 0U, 0U);
                    RefCV_AmbaAc_MvacRestart(&pCtrl->AlgoOpEx.MetaData, &AcConfig.MvacViewInfo);
                    SvcLog_OK(SVC_LOG_STEREO_AC, "RefCV_AmbaAc_MvacRestart Done!", 0U, 0U);

                    AmbaKAL_TaskSleep(3000);
                    pCtrl->Enable = 1U;
                    SvcLog_OK(SVC_LOG_STEREO_AC, "SvcImgFeeder_Connect start", 0U, 0U);
                    Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                    SvcLog_OK(SVC_LOG_STEREO_AC, "RefCV_AmbaAc_MvacRestart done", 0U, 0U);
                }
            }
        }
        pCtrl->RxSeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC, "Rx[%u]: recv error", Inst, 0U);
    }

    SVC_CV_DBG(SVC_LOG_STEREO_AC, "Rx[%u]: end", Inst, 0U);

    return SVC_OK;
}

/**
* Task entry of RX process
*/
static void* StereoAC_RxTaskEntry(void* EntryArg)
{
    const ULONG  *pArg;
    UINT32 Rval = SVC_OK, Inst;
    const SVC_STEREO_AC_CTRL_s *pCtrl;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Inst = (UINT32)(*pArg);
    pCtrl = &g_StereoACCtrl[Inst];

    SvcLog_DBG(SVC_LOG_STEREO_AC, "RxTaskEntry[%d] start", Inst, 0U);

    while (SVC_OK == Rval) {
        if (NULL != pCtrl->pRxProc) {
            Rval = pCtrl->pRxProc(Inst);
        } else {
            Rval = SVC_NG;
        }
    }

    SvcLog_DBG(SVC_LOG_STEREO_AC, "RxTaskEntry[%d] exit", Inst, 0U);

    return NULL;
}

/**
* Set and wait TX and RX task to idle state
*/
static UINT32 StereoAC_SetTaskIdle(UINT32 Inst)
{
    UINT32 Rval;
    SVC_STEREO_AC_CTRL_s *pCtrl = &g_StereoACCtrl[Inst];
    UINT32 i;

    /* Disable input and output */
    pCtrl->Enable = 0U;

    if (SVC_OK != SvcImgFeeder_Disconnect(&pCtrl->FeederPort)) {
        SvcLog_NG(SVC_LOG_STEREO_AC, "Disconn feeder port error", 0U, 0U);
    }

    if (SVC_OK != SvcImgFeeder_ClosePort(&pCtrl->FeederPort)) {
        SvcLog_NG(SVC_LOG_STEREO_AC, "Close feeder port error", 0U, 0U);
    }

    /* Wait Rx idle */
    for (i = 0; i < SVC_STEREO_AC_SEM_INIT_COUNT; i++) {
        Rval = StereoAC_SemTake(Inst);
    }

    return Rval;
}

/**
* Send external YUV to driver
*/
static UINT32 StereoAC_SendExtYUV(UINT32 Inst, const void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_STEREO_AC_CTRL_s *pCtrl = &g_StereoACCtrl[Inst];
    void *pInputImg; /* SVC_CV_INPUT_IMG_INFO_s */

    if (NULL != pCtrl->pTxProc) {
        AmbaMisra_TypeCast(&pInputImg, &pInfo);
        Rval = pCtrl->pTxProc(Inst, &pCtrl->FeederPort, SVC_IMG_FEEDER_IMAGE_DATA_READY, pInputImg);
    }

    return Rval;
}

/**
* CvFlow driver source rate update function
*/
static void StereoAC_SrcRateUpdate(SVC_STEREO_AC_CTRL_s *pCtrl, UINT32 SrcRate)
{
#define FRACTION_NUM_BIT   0x0000FF00U
#define FRACTION_NUM_SFT   8U
#define FRACTION_DEN_BIT   0x000000FFU
#define FRACTION_DEN_SFT   0U
    UINT32 Rval;
    UINT32 Num, Den;
    UINT32 i, index, Ratio_M100;

    if (0U < (SrcRate & SRC_RATE_FRACTION_MODE_BIT)) {
        /* Fraction mode:  SrcRate[4:8]: numerator, SrcRate[0:4]: denominator */
        Num = ((SrcRate & FRACTION_NUM_BIT) >> FRACTION_NUM_SFT);
        Den = ((SrcRate & FRACTION_DEN_BIT) >> FRACTION_DEN_SFT);
        if ((Den > 0U) && (Num > 0U) &&
            (Den <= STEREO_AC_SRC_RATE_MAX_DENOMINATOR) && (Num <= Den)) {
            Rval = AmbaWrap_memset(&pCtrl->SrcValidTbl[0], 0, sizeof(pCtrl->SrcValidTbl));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STEREO_AC, "memset SrcValidTbl failed", 0U, 0U);
            }

            Ratio_M100 = (Den << 8U) / Num;     //Shift 8 to avoid calc loss
            for (i = 0; i < Num; i++) {
                index = (Ratio_M100 * i) >> 8U;
                pCtrl->SrcValidTbl[index] = 1U;
            }

            pCtrl->SrcRateNum = (UINT8)Num;
            pCtrl->SrcRateDen = (UINT8)Den;
            pCtrl->SrcRateCtrl = SrcRate;

            //for (i = 0; i < Den; i++) {
            //    AmbaPrint_PrintUInt5("[%d] = %d", i, pCtrl->SrcValidTbl[i], 0U, 0U, 0U);
            //}
        } else {
            SvcLog_NG(SVC_LOG_STEREO_AC, "Invalid ratio: %u/%u", Num, Den);
        }
    } else {
        /* Divisor */
        if (SrcRate == 0U) {
            pCtrl->SrcRateCtrl = 1U;
        } else {
            pCtrl->SrcRateCtrl = SrcRate;
        }
    }

    SvcLog_OK(SVC_LOG_STEREO_AC, "SrcRateCtrl: 0x%x", pCtrl->SrcRateCtrl, 0U);
}

/**
* Send meta data to driver
*/
static UINT32 StereoAC_SetMetaData(UINT32 Inst, const void *pInfo)
{
    UINT32 Rval = SVC_NG;
    SVC_STEREO_AC_CTRL_s *pCtrl = &g_StereoACCtrl[Inst];
    const SVC_CV_META_DATA_INFO_s *pMetaDataInfo = NULL;
    SVC_CV_FLOW_BUF_INFO_s BufInfo;

    if (NULL != pInfo) {
        AmbaMisra_TypeCast(&pMetaDataInfo, &pInfo);
        if ((pMetaDataInfo->NumData == 2U) && (pMetaDataInfo->pList != NULL)) {
            /* Left */
            Rval = SvcCvFlow_AllocWorkBuf((pMetaDataInfo->pList)[0].Size, &BufInfo);
            if (SVC_OK == Rval) {
                Rval = AmbaWrap_memcpy(BufInfo.pAddr, (pMetaDataInfo->pList)[0].pAddr, (pMetaDataInfo->pList)[0].Size);
                if (SVC_OK == Rval) {
                    SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STEREO_AC,
                                                 BufInfo.pAddr,
                                                 BufInfo.Size,
                                                 &pCtrl->AlgoOpEx.MetaData.CalibInfo.Left);
                }
            }

            /* Right */
            if (SVC_OK == Rval) {
                Rval = SvcCvFlow_AllocWorkBuf((pMetaDataInfo->pList)[1].Size, &BufInfo);
                if (SVC_OK == Rval) {
                    Rval = AmbaWrap_memcpy(BufInfo.pAddr, (pMetaDataInfo->pList)[1].pAddr, (pMetaDataInfo->pList)[1].Size);
                    if (SVC_OK == Rval) {
                        SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STEREO_AC,
                                                     BufInfo.pAddr,
                                                     BufInfo.Size,
                                                     &pCtrl->AlgoOpEx.MetaData.CalibInfo.Right);
                    }
                }
            }
        }
    }

    if ((Rval == SVC_OK) && (pMetaDataInfo != NULL)) {
        pCtrl->AlgoOpEx.MetaDataFlag = 1U;
        SvcLog_OK(SVC_LOG_STEREO_AC, "StereoAC_SetMetaData: Size = %u, %u", (pMetaDataInfo->pList)[0].Size, (pMetaDataInfo->pList)[1].Size);
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC, "StereoAC_SetMetaData[%d] error(0x%x)", Inst, Rval);
    }

    return Rval;
}

/**
* Entry of configuration function
*/
static UINT32 StereoAC_Config(UINT32 Inst, AmbaAcConfig_t *pAcConfig)
{
    UINT32 Rval;
    const SVC_STEREO_AC_CTRL_s *pCtrl = &g_StereoACCtrl[Inst];
    const SVC_CV_ROI_INFO_s *pRoiInfo = &pCtrl->pCvFlow->InputCfg.Input[0].RoiInfo;
    extern AmbaAcConfig_t StereoAC_AcConfigDefult;

    Rval = AmbaWrap_memcpy(pAcConfig, &StereoAC_AcConfigDefult, sizeof(AmbaAcConfig_t));
    if (SVC_OK == Rval) {
        if (pRoiInfo->NumRoi > 0U) {
            pAcConfig->MvacViewInfo.ScaleId = pRoiInfo->Roi[0].Index;
            pAcConfig->VoCfg.View.Scale = pRoiInfo->Roi[0].Index;
            pAcConfig->VoCfg.View.RoiX = pRoiInfo->Roi[0].StartX;
            pAcConfig->VoCfg.View.RoiY = pRoiInfo->Roi[0].StartY;
            pAcConfig->VoCfg.View.RoiWidth = pRoiInfo->Roi[0].Width;
            pAcConfig->VoCfg.View.RoiHeight = pRoiInfo->Roi[0].Height;
        } else {
            SvcLog_DBG(SVC_LOG_STEREO_AC, "Use default AC config. Scale = %d", StereoAC_AcConfigDefult.MvacViewInfo.ScaleId, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC, "memcpy AcConfig failed", 0U, 0U);
    }

    return Rval;
}

static UINT32 StereoAC_ConfigBuffer(UINT32 Inst)
{
    UINT32 Rval = SVC_OK;
    SVC_STEREO_AC_CTRL_s *pCtrl = &g_StereoACCtrl[Inst];
    SVC_CV_FLOW_BUF_INFO_s BufInfo;
    UINT32 i;

    /* Img info buffer */
    if (pCtrl->pImgInfoBuf[0] == NULL) {
        for (i = 0; i < SVC_STEREO_AC_PICINFO_QUEUE_SIZE; i++) {
            Rval = SvcCvFlow_AllocWorkBuf(sizeof(memio_source_recv_picinfo_t), &BufInfo);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pCtrl->pImgInfoBuf[i], &BufInfo.pAddr);
                pCtrl->ImgInfoBufSize = BufInfo.Size;
            } else {
                SvcLog_NG(SVC_LOG_STEREO_AC, "Alloc Img info buf failed", 0U, 0U);
                Rval = SVC_NG;
                break;
            }
        }
    }

    /* MVAC output buffer */
    if (pCtrl->pMVACOutBuf[0] == NULL) {
        for (i = 0; i < SVC_STEREO_AC_OUTPUT_DEPTH; i++) {
            Rval = SvcCvFlow_AllocWorkBuf(sizeof(AMBA_CV_MVAC_DATA_s), &BufInfo);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pCtrl->pMVACOutBuf[i], &BufInfo.pAddr);
                pCtrl->MVACOutBufSize = BufInfo.Size;
            } else {
                SvcLog_NG(SVC_LOG_STEREO_AC, "Alloc Warp buf failed", 0U, 0U);
                Rval = SVC_NG;
                break;
            }
        }
    }

    /* Warp output buffer */
    if (pCtrl->pWarpOutBuf[0] == NULL) {
        for (i = 0; i < SVC_STEREO_AC_OUTPUT_DEPTH; i++) {
            Rval = SvcCvFlow_AllocWorkBuf(sizeof(AMBA_CV_MVAC_WARP_INFO_s), &BufInfo);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pCtrl->pWarpOutBuf[i], &BufInfo.pAddr);
                pCtrl->WarpOutBufSize = BufInfo.Size;
            } else {
                SvcLog_NG(SVC_LOG_STEREO_AC, "Alloc Warp buf failed", 0U, 0U);
                Rval = SVC_NG;
                break;
            }
        }
    }

    return Rval;
}

/**
* CvFlow driver initialization function
*/
static UINT32 CvFlow_StereoACInit(UINT32 CvFlowChan, void *pInit)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    static UINT32 g_StereoACInit = 0U;

    (void) CvFlowChan;
    (void) pInit;

    if (0U == g_StereoACInit) {
        g_StereoACInit = 1U;

        if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&StereoACMutex, NULL)) {
            SvcLog_NG(SVC_LOG_STEREO_AC, "MutexCreate error", 0U, 0U);
            Rval = SVC_NG;
        }
        for (i = 0; i < SVC_STEREO_AC_MAX_INSTANCE; i++) {
            g_StereoACCtrl[i].Used = 0;
        }

        SVC_CV_DBG(SVC_LOG_STEREO_AC, "CvFlow_StereoACInit: first init", 0U, 0U);
    }

    SvcLog_OK(SVC_LOG_STEREO_AC, "CvFlow_StereoACInit: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver configuration function
*/
static UINT32 CvFlow_StereoACConfig(UINT32 CvFlowChan, void *pConfig)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    SVC_STEREO_AC_CTRL_s *pCtrl = NULL;
    const SVC_CV_FLOW_STEREO_AC_MODE_INFO_s *pModeInfo;

    (void) pConfig;

    if (NULL != pConfig) {
        StereoAC_MutexTake();
        /* Create instance */
        for (i = 0; i < SVC_STEREO_AC_MAX_INSTANCE; i++) {
            if (0U == g_StereoACCtrl[i].Used) {
                Rval = AmbaWrap_memset(&g_StereoACCtrl[i], 0, sizeof(SVC_STEREO_AC_CTRL_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STEREO_AC, "memset g_StereoACCtrl[%u] failed", i, 0U);
                }
                pCtrl = &g_StereoACCtrl[i];
                pCtrl->Used = 1;
                SvcLog_DBG(SVC_LOG_STEREO_AC, "Create Inst(%u)", i, 0U);
                break;
            }
        }
        StereoAC_MutexGive();

        /* Configure instance */
        if (NULL != pCtrl) {
            pCtrl->CvfChan     = (UINT8) CvFlowChan;
            pCtrl->pCvFlow     = &pCtrl->CvCfg;
            Rval = AmbaWrap_memcpy(&pCtrl->CvCfg, pConfig, sizeof(SVC_CV_FLOW_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STEREO_AC, "memcpy CvCfg failed", 0U, 0U);
            }
            StereoAC_SrcRateUpdate(pCtrl, pCtrl->pCvFlow->InputCfg.SrcRate);
            pCtrl->Enable = 1;

            pModeInfo = &SvcCvFlow_StereoAC_ModeInfo[pCtrl->pCvFlow->CvModeID];
            if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG) {
                Rval = SVC_OK;
            } else if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
#ifdef CONFIG_AMBALINK_BOOT_OS
                /* Init FlexiDAG IO framework */
                if (pCtrl->pCvFlow->PrivFlag < SVC_CV_FLEXIDAG_IO_MAX_CHANNEL) {
                    pCtrl->FlexiIOChan = pCtrl->pCvFlow->PrivFlag;
                    Rval = SvcCv_FlexiIORegister(pCtrl->FlexiIOChan, StereoAC_Init_Callback);
                    if (Rval != SVC_OK) {
                        SvcLog_NG(SVC_LOG_STEREO_AC, "SvcCv_FlexiIORegister(%u) failed", pCtrl->FlexiIOChan, 0U);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_STEREO_AC, "Invalid FlexidagIO chan(%u)", pCtrl->pCvFlow->PrivFlag, 0U);
                }
#else
                Rval = SVC_NG;
#endif
            } else {
                SvcLog_NG(SVC_LOG_STEREO_AC, "unsupported CVFlow ProfID(%u)", pModeInfo->ProfID, 0U);
                Rval = SVC_NG;
            }
        } else {
            Rval = SVC_NG;
        }
    } else {
        Rval = SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_STEREO_AC, "CvFlow_StereoACConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC, "CvFlow_StereoACConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver load function
*/
static UINT32 CvFlow_StereoACLoad(UINT32 CvFlowChan, void *pInfo)
{
    //#define LOAD_FROM_DEFAULT_PATH
#if defined LOAD_FROM_DEFAULT_PATH
    #define FILE_PATH_LEFT "c:/201120_mvac_input/metadata/fov0_only_meta.bin"
    #define FILE_PATH_RIGHT "c:/201120_mvac_input/metadata/fov1_only_meta.bin"
#endif
    UINT32 Rval;
    UINT32 Inst;
    SVC_STEREO_AC_CTRL_s *pCtrl;
    const SVC_CV_FLOW_LOAD_INFO_s *pLoadInfo;
    const char *pFileName;
    SVC_CV_FLOW_BUF_INFO_s BufInfo;

    (void) pInfo;

    Rval = StereoAC_CvfChan2Inst(CvFlowChan, &Inst);
    if (Rval == SVC_OK) {
        pCtrl = &g_StereoACCtrl[Inst];
        if (NULL != pInfo) {
            AmbaMisra_TypeCast(&pLoadInfo, &pInfo);
            if (pLoadInfo->NumBin > 0U) {
                /* Read metadata for Left */
#if defined LOAD_FROM_DEFAULT_PATH
                pFileName = FILE_PATH_LEFT;
#else
                pFileName = pLoadInfo->pFileName[0];
#endif
                if (pFileName != NULL) {
                    Rval = SvcCvFlow_LoadBinFile(pFileName, SVC_CV_FLOW_STORAGE_TYPE_SD, &BufInfo);
                    if (SVC_OK == Rval) {
                        SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STEREO_AC,
                                                     BufInfo.pAddr,
                                                     BufInfo.Size,
                                                     &pCtrl->AlgoOpEx.MetaData.CalibInfo.Left);
                    } else {
                        SvcLog_NG(SVC_LOG_STEREO_AC, "Load metadata0 failed", 0U, 0U);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_STEREO_AC, "FileName = NULL", 0U, 0U);
                }

                /* Read metadata for Right */
#if defined LOAD_FROM_DEFAULT_PATH
                pFileName = FILE_PATH_RIGHT;
#else
                pFileName = pLoadInfo->pFileName[0];
#endif
                if (pFileName != NULL) {
                    Rval = SvcCvFlow_LoadBinFile(pFileName, SVC_CV_FLOW_STORAGE_TYPE_SD, &BufInfo);
                    if (SVC_OK == Rval) {
                        SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STEREO_AC,
                                                     BufInfo.pAddr,
                                                     BufInfo.Size,
                                                     &pCtrl->AlgoOpEx.MetaData.CalibInfo.Right);
                    } else {
                        SvcLog_NG(SVC_LOG_STEREO_AC, "Load metadata0 failed", 0U, 0U);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_STEREO_AC, "FileName = NULL", 0U, 0U);
                }
            } else {
                /* Do nothing */
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC, "CvFlow_StereoACLoad: invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STEREO_AC, "CvFlow_StereoACLoad: CvFlowChan(%d) error", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver start function
*/
static UINT32 CvFlow_StereoACStart(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_STEREO_AC_CTRL_s *pCtrl;
    SVC_IMG_FEEDER_PORT_CFG_s PortCfg = {0};
    static char CbEvtQueName[] = "StereoACCbEvtQue";
    static char SemName[] = "OutBufSem";
    static STEREO_AC_OUTPUT_MSG_s AlgoOutQue[SVC_STEREO_AC_MAX_INSTANCE][SVC_STEREO_AC_ALGO_OUT_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 StereoACRxTaskStack[SVC_STEREO_AC_MAX_INSTANCE][SVC_STEREO_AC_RX_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_IMG_FEEDER_EVENT_CALLBACK_f StereoAC_TxDataSrcHandlerList[SVC_STEREO_AC_MAX_INSTANCE] = {
        StereoAC_TxDataSrcHandler0,
        StereoAC_TxDataSrcHandler1,
    };
    static AcGetResultCb_t StereoAC_AlgoOutCbList[SVC_STEREO_AC_MAX_INSTANCE] = {
        StereoAC_AlgoOutCallback0,
        StereoAC_AlgoOutCallback1,
    };
    AmbaAcConfig_t AcConfig;

    (void) pInput;

    Rval = StereoAC_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl = &g_StereoACCtrl[Inst];

        /* Open Feeder port first to enable frame FIFO earlier */
        if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
            /* The driver supports two inputs per instance */
            PortCfg.NumPath            = 2U;
            PortCfg.Content[0].DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
            PortCfg.Content[0].StrmId  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
            PortCfg.Content[1].DataSrc = pCtrl->pCvFlow->InputCfg.Input[1].DataSrc;
            PortCfg.Content[1].StrmId  = pCtrl->pCvFlow->InputCfg.Input[1].StrmId;
            PortCfg.SendFunc           = StereoAC_TxDataSrcHandlerList[Inst];
            if (SVC_OK != SvcImgFeeder_OpenPort(pCtrl->pCvFlow->InputCfg.FeederID, &PortCfg, &pCtrl->FeederPort)) {
                SvcLog_NG(SVC_LOG_STEREO_AC, "Open feeder port error", 0U, 0U);
            }
        } else {
            SvcLog_DBG(SVC_LOG_STEREO_AC, "ImgFeeder is not enabled", 0U, 0U);
        }

        Rval = StereoAC_Config(Inst, &AcConfig);

        Rval |= StereoAC_ConfigBuffer(Inst);

        if (SVC_OK == Rval) {
            if (pCtrl->AlgoOpEx.MetaDataFlag != 1U) {
                SvcLog_NG(SVC_LOG_STEREO_AC, "Please exec SVC_CV_CTRL_SET_META_DATA", 0U, 0U);
            }

            /* Activate Auto Calibration */
            Rval = RefCV_AmbaAcInit(&pCtrl->AlgoOpEx.MetaData, &AcConfig, StereoAC_AlgoOutCbList[Inst]);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STEREO_AC, "RefCV_AmbaAc_Liveview error(0x%x)", Rval, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_STEREO_AC, "Config error", 0U, 0U);
        }

        if (SVC_OK == Rval) {
            pCtrl->pTxProc = StereoAC_TxDataSrcHandler;
            pCtrl->pRxProc = StereoAC_RxDataSinkHandler;

            Rval = AmbaKAL_MsgQueueCreate(&pCtrl->AlgoOutQueId,
                                           CbEvtQueName,
                                           sizeof(STEREO_AC_OUTPUT_MSG_s),
                                           &AlgoOutQue[Inst][0],
                                           SVC_STEREO_AC_ALGO_OUT_QUEUE_SIZE * sizeof(STEREO_AC_OUTPUT_MSG_s));

            Rval |= AmbaKAL_SemaphoreCreate(&pCtrl->OutBufSem, SemName, SVC_STEREO_AC_SEM_INIT_COUNT);

            if (KAL_ERR_NONE == Rval) {
                /* Create RX task */
                pCtrl->RxTaskCtrl.Priority   = 60;
                pCtrl->RxTaskCtrl.EntryFunc  = StereoAC_RxTaskEntry;
                pCtrl->RxTaskCtrl.EntryArg   = Inst;
                pCtrl->RxTaskCtrl.pStackBase = &StereoACRxTaskStack[Inst][0];
                pCtrl->RxTaskCtrl.StackSize  = SVC_STEREO_AC_RX_TASK_STACK_SIZE;
                pCtrl->RxTaskCtrl.CpuBits    = 0x01U;
                Rval = SvcTask_Create("SvcStACRxTask", &pCtrl->RxTaskCtrl);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STEREO_AC, "Create RxTask Inst(%u) error", Inst, 0U);
                }

                /* Start input */
                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_STEREO_AC, "Connect feeder port error", 0U, 0U);
                    }
                }

                if (SVC_OK == Rval) {
                    pCtrl->Started = 1U;
                }
            } else {
                SvcLog_NG(SVC_LOG_STEREO_AC, "Create MsgQue/Semaphore error(0x%X)", Rval, 0U);
                Rval =  SVC_NG;
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC, "Invalid CvFlowChan", 0U, 0U);
        Rval =  SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_STEREO_AC, "CvFlow_StereoACStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC, "CvFlow_StereoACStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver control function
*/
static UINT32 CvFlow_StereoACControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo)
{
    UINT32 Rval, Inst;
    SVC_STEREO_AC_CTRL_s *pCtrl;
    const UINT32 *pValue;

    StereoAC_MutexTake();

    Rval = StereoAC_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl = &g_StereoACCtrl[Inst];
        switch (CtrlType) {
            case SVC_CV_CTRL_ENABLE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->Enable = (UINT8) *pValue;
                if ((1U == pCtrl->Started) &&
                    (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder)) {
                    if (*pValue == 0U) {
                        Rval = SvcImgFeeder_Disconnect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_STEREO_AC, "Disconn feeder port error", 0U, 0U);
                        }
                    } else {
                        Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_STEREO_AC, "Cnnect feeder port error", 0U, 0U);
                        }
                    }
                }
                break;
            case SVC_CV_CTRL_SET_SRC_RATE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->SrcRateCtrl = *pValue;
                SvcLog_OK(SVC_LOG_STEREO_AC, "Control[%d]: SrcRate = %u", Inst, *pValue);
                break;
            case SVC_CV_CTRL_SEND_EXT_YUV:
                Rval = StereoAC_SendExtYUV(Inst, pInfo);
                break;
            case SVC_CV_CTRL_SET_META_DATA:
                Rval = StereoAC_SetMetaData(Inst, pInfo);
                break;
            default:
                SvcLog_NG(SVC_LOG_STEREO_AC, "Control[%d]: Invalid CtrlType(%d)", Inst, CtrlType);
                break;
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_AC, "Control: Invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    StereoAC_MutexGive();

    return Rval;
}

/**
* CvFlow driver stop function
*/
static UINT32 CvFlow_StereoACStop(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_STEREO_AC_CTRL_s *pCtrl;
    SVC_CV_FLOW_ALGO_DELETE_CFG_s DelCfg;

    (void)pInput;

    Rval = StereoAC_CvfChan2Inst(CvFlowChan, &Inst);
    pCtrl = &g_StereoACCtrl[Inst];

    if (SVC_OK == Rval) {
        Rval = StereoAC_SetTaskIdle(Inst);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_STEREO_AC, "SetTaskIdle error", 0U, 0U);
        }

        Rval = SvcTask_Destroy(&pCtrl->RxTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STEREO_AC, "Destroy Tx/Rx Task error", 0U, 0U);
        }

        Rval = AmbaWrap_memset(&DelCfg, 0, sizeof(SVC_CV_FLOW_ALGO_DELETE_CFG_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STEREO_AC, "memset DelCfg failed", 0U, 0U);
        }

#ifdef CONFIG_AMBALINK_BOOT_OS
        if (SvcCvFlow_StereoAC_ModeInfo[pCtrl->pCvFlow->CvModeID].ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
            Rval = SvcCv_FlexiIORelease(pCtrl->FlexiIOChan);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_STEREO_AC, "SvcCv_FlexiIORelease(%u) failed", pCtrl->FlexiIOChan, 0U);
            }
        }
#endif

        Rval = AmbaKAL_MsgQueueDelete(&pCtrl->AlgoOutQueId);
        Rval |= AmbaKAL_SemaphoreDelete(&pCtrl->OutBufSem);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_STEREO_AC, "Delete MsgQue/Semaphore error(0x%X)", Rval, 0U);
        }

        /* Reset Ctrl Intance */
        Rval = AmbaWrap_memset(&g_StereoACCtrl[Inst], 0, sizeof(SVC_STEREO_AC_CTRL_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STEREO_AC, "memset g_StereoACCtrl[%u] failed", Inst, 0U);
        }
    }

    SvcLog_OK(SVC_LOG_STEREO_AC, "CvFlow_StereoACStop: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver debug interface
*/
static UINT32 CvFlow_StereoACDebug(UINT32 CvFlowChan, const char *pCmd, UINT32 Param0, UINT32 Param1, UINT32 Param2)
{
    UINT32 i;
    const SVC_STEREO_AC_CTRL_s *pCtrl;

    if (0 == SvcWrap_strcmp("debug", pCmd)) {
        SvcCvDebugEnable = (UINT8) Param0;
        SvcLog_DBG(SVC_LOG_STEREO_AC, "Set debug(%u)", Param0, 0U);
    } else if (0 == SvcWrap_strcmp("info", pCmd)) {
        for (i = 0; i < SVC_STEREO_AC_MAX_INSTANCE; i++) {
            pCtrl = &g_StereoACCtrl[i];
            if (pCtrl->Used == 1U) {
                AmbaPrint_PrintUInt5("ST_AC[%d]: CvfChan %d", i, pCtrl->CvfChan, 0U, 0U, 0U);
            }
        }
    } else {
        SvcLog_DBG(SVC_LOG_STEREO_AC, "Unsupported cmd", 0U, 0U);
    }

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(&Param0);
    AmbaMisra_TouchUnused(&Param1);
    AmbaMisra_TouchUnused(&Param2);

    return SVC_OK;
}

SVC_CV_FLOW_OBJ_s SvcCvFlow_StereoACObj = {
    .Init         = CvFlow_StereoACInit,
    .Config       = CvFlow_StereoACConfig,
    .Load         = CvFlow_StereoACLoad,
    .Start        = CvFlow_StereoACStart,
    .Control      = CvFlow_StereoACControl,
    .Stop         = CvFlow_StereoACStop,
    .Debug        = CvFlow_StereoACDebug,
};

