/**
 *  @file SvcCvFlow_Stixel.c
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
 *  @details Implementation of FlexiDAG based Stixel application
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event.h"
#include "AmbaMMU.h"
#include "AmbaDef.h"
#include "AmbaUtility.h"
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
#include "SvcCvFlow_Stixel.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcOsd.h"
#include "SvcWrap.h"
#include "SvcPlat.h"

#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_ref_util.h"
#include "cvapi_svccvalgo_stixel.h"
#include "CameraPitchDetection.h"

#define SVC_LOG_STIXEL        "Stixel"

#define SVC_STIXEL_INSTANCE0               (0U)
#define SVC_STIXEL_INSTANCE1               (1U)
#define SVC_STIXEL_MAX_INSTANCE            (2U)

#define SVC_STIXEL_SEM_INIT_COUNT          (1U)
#define SVC_STIXEL_OUTPUT_DEPTH            (SVC_STIXEL_SEM_INIT_COUNT + 1U)

#define SVC_STIXEL_ALGO_MAX_INPUT_NUM      (4U)
#define SVC_STIXEL_ALGO_OUT_QUEUE_SIZE     (SVC_STIXEL_OUTPUT_DEPTH)

typedef UINT32 (*SVC_STIXEL_TX_HANDLER_f)(UINT32 Inst, UINT32 Event, void *pInfo);

#define SVC_STIXEL_INPUT_DSI               (0U)

#define SVC_STIXEL_INDEX_VDISP             (0U)
#define SVC_STIXEL_INDEX_GEN_ESTI_ROAD     (1U)
#define SVC_STIXEL_INDEX_PREPROCESS        (2U)
#define SVC_STIXEL_INDEX_FREE_SPACE        (3U)
#define SVC_STIXEL_INDEX_HEIGHT_SEG        (4U)
#define SVC_STIXEL_INDEX_MAX               (5U)

//#define UT_CFG_DEFAULT_CAMERA_HEIGH        (1.520587)
//#define UT_CFG_DEFAULT_CAMERA_PITCH        (0.073589)


typedef struct {
    flexidag_memblk_t           OutputBuf[SVC_STIXEL_OUTPUT_DEPTH][FLEXIDAG_MAX_OUTPUTS];
    AMBA_CV_FLEXIDAG_IO_s       InBuf[SVC_STIXEL_OUTPUT_DEPTH];
    AMBA_CV_FLEXIDAG_IO_s       OutBuf[SVC_STIXEL_OUTPUT_DEPTH];

    SVC_CV_FLOW_BUF_INFO_s      InputBuf;
} SVC_CV_FLOW_STIXEL_OP_EX_INFO_s;

typedef struct {
    UINT8                       Used;
    UINT8                       CvfChan;
    UINT8                       Enable;
    UINT8                       Started;
    SVC_CV_FLOW_CFG_s           *pCvFlow;
    SVC_CV_FLOW_CFG_s           CvCfg;

    UINT8                       NumValidAlgo;
    SVC_CV_FLOW_ALGO_OP_INFO_s  AlgoOp[STIXEL_MAX_ALGO_NUM];
    SVC_CV_FLOW_STIXEL_OP_EX_INFO_s AlgoOpEx[STIXEL_MAX_ALGO_NUM];
    memio_source_recv_raw_t     *pRawInfoBuf[SVC_STIXEL_ALGO_MAX_INPUT_NUM];
    UINT32                      RawInfoBufSize;

    /* Data source service */
    UINT32                      SrcSeqNum;
    SVC_STIXEL_TX_HANDLER_f     pTxProc;
    UINT32                      TxSeqNum;

    /* Data sink service */
    UINT32                      OutBufIdx;
    AMBA_KAL_SEMAPHORE_t        OutBufSem;

    /* Algo Callback */
    AMBA_KAL_MSG_QUEUE_t        AlgoOutQueId;

    /* Misc */
    UINT32                      SrcRateCtrl;

    CAM_PITCH_DET_V_DISP_s      VDisp;
    SVC_CV_FLOW_BUF_INFO_s      AutoDetWorkBuf;     /* CamPitchDet */
    SVC_CV_FLOW_BUF_INFO_s      ManualDetWorkBuf;   /* CamPitchDet */
} SVC_STIXEL_CTRL_s;

typedef struct {
   UINT32 AlgoIndex;
   SVC_CV_ALGO_OUTPUT_s AlgoOut;
} STIXEL_OUTPUT_MSG_s;

typedef struct {
   UINT32 AlgoIndex;
} STIXEL_USER_DATA_s;

static SVC_STIXEL_CTRL_s g_StixelCtrl[SVC_STIXEL_MAX_INSTANCE] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MUTEX_t StixelMutex;
static UINT8 SvcCvDebugEnable = 0;
static UINT8 SvcCvDebugDump = 0;

static UINT32 CvFlow_StixelInit(UINT32 CvFlowChan, void *pInit);
static UINT32 CvFlow_StixelConfig(UINT32 CvFlowChan, void *pConfig);
static UINT32 CvFlow_StixelStart(UINT32 CvFlowChan, void *pInput);
static UINT32 CvFlow_StixelControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo);
static UINT32 CvFlow_StixelStop(UINT32 CvFlowChan, void *pInput);
static UINT32 Stixel_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst);

static UINT32 Stixel_MsgQueueSend(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, UINT32 AlgoIndex, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 Stixel_MsgQueueRecv(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, STIXEL_OUTPUT_MSG_s *pMsg, UINT32 WaitAlgoIndex);

static SVC_CV_FLOW_STIXEL_PREF_STIXEL_CFG_s Stixel_Pref_Cfg;
static CAM_PITCH_DET_MANUAL_PRIOR_ESTI_s PriorEstimatedGround[SVC_STIXEL_MAX_INSTANCE];
static DOUBLE StixelEstiCamHeight = 0.0;
static DOUBLE StixelEstiCamPitch = 0.0;

static void SVC_CV_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcCvDebugEnable > 0U) {
        SvcLog_DBG(pModule, pFormat, Arg1, Arg2);
    }
}

static void SVC_CV_DBG_INFO(const char *pFormat, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    if (SvcCvDebugEnable > 1U) {
        AmbaPrint_PrintUInt5(pFormat, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}

static void Stixel_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&StixelMutex, 5000)) {
        SvcLog_NG(SVC_LOG_STIXEL, "Stixel_MutexTake: timeout", 0U, 0U);
    }
}

static void Stixel_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&StixelMutex)) {
        SvcLog_NG(SVC_LOG_STIXEL, "Stixel_MutexGive: error", 0U, 0U);
    }
}

static UINT32 Stixel_SemTake(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreTake(&g_StixelCtrl[Inst].OutBufSem, 10000)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_STIXEL, "Stixel_SemTake[%d]: timeout", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 Stixel_SemGive(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreGive(&g_StixelCtrl[Inst].OutBufSem)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_STIXEL, "Stixel_SemGive[%d]: error", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 Stixel_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst)
{
    UINT32 Rval = SVC_NG;
    UINT32 i;

    if (NULL != pInst) {
        for (i = 0; i < SVC_STIXEL_MAX_INSTANCE; i++) {
            if ((1U == g_StixelCtrl[i].Used) && (CvfChan == g_StixelCtrl[i].CvfChan)) {
                *pInst = i;
                Rval = SVC_OK;
                break;
            }
        }
    }
    return Rval;
}

static void Stixel_SaveBin(const char *pFileName, UINT8 *pBuf, const UINT32 Size)
{
    UINT32 Ret;
    AMBA_FS_FILE *pFile;
    UINT32 OpRes;

    Ret = AmbaFS_FileOpen(pFileName, "wb", &pFile);
    if(Ret != 0U) {
        AmbaPrint_PrintUInt5("_WriteOutputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
        SvcLog_NG(SVC_LOG_STIXEL, "AmbaFS_FileOpen failed", 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileWrite(pBuf, 1U, Size, pFile, &OpRes);
        if(Ret != 0U) {
            SvcLog_NG(SVC_LOG_STIXEL, "AmbaFS_FileWrite failed", 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(pFile);
        if(Ret != 0U) {
            SvcLog_NG(SVC_LOG_STIXEL, "AmbaFS_FileClose failed", 0U, 0U);
        }
    }
}

static void Stixel_DebugSave(UINT32 FileNum, const char* const SaveNameSet[],
    const ULONG AddrSet[], const UINT32 FileSizeSet[])
{
    UINT32 FileIdx;
    UINT8 *pU8;

    for (FileIdx = 0U; FileIdx < FileNum; FileIdx++) {
        if (SVC_OK != AmbaWrap_memcpy(&pU8, &AddrSet[FileIdx], (UINT32)sizeof(void *))) {
            SvcLog_NG(SVC_LOG_STIXEL, "typecast addr failed", 0U, 0U);
        }
        Stixel_SaveBin(SaveNameSet[FileIdx], pU8, FileSizeSet[FileIdx]);
    }

    return;
}

#if 0
static void Stixel_DumpPicInfo(const cv_pic_info_t *pPicInfo)
{

}
#endif

/**
* Pack data source to pic info structure
*/
#if 0
static void Stixel_PackPicInfo(UINT32 Inst, const SVC_CV_INPUT_IMG_INFO_s *pInputImg, memio_source_recv_picinfo_t *pInfo)
{

}
#endif

static UINT32 Stixel_RunVDisp(UINT32 Inst,
                              const SVC_CV_FLOW_STIXEL_DSI_INPUT_s *pStixelInput,
                              memio_source_recv_raw_t *pRawInfoBuf[],
                              STIXEL_USER_DATA_s *pUserDataBuf)
{
    UINT32 Rval = SVC_OK;
    SVC_STIXEL_CTRL_s *pCtrl = &g_StixelCtrl[Inst];
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &pCtrl->AlgoOp[SVC_STIXEL_INDEX_VDISP];
    const SVC_CV_FLOW_STIXEL_OP_EX_INFO_s *pAlgoOpEx = &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_VDISP];
    const SVC_CV_FLOW_STIXEL_CFG_s *pModeCfg = &SvcCvFlow_Stixel_ModeInfo[pCtrl->pCvFlow->CvModeID].Cfg;
    AMBA_CV_FLEXIDAG_IO_s *pInBuf = &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_VDISP].InBuf[pCtrl->OutBufIdx];
    AMBA_CV_FLEXIDAG_IO_s *pOutBuf = &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_VDISP].OutBuf[pCtrl->OutBufIdx];
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;
    UINT32 i;
    ULONG  DataAddr;
    UINT32 DataSize;
    STIXEL_OUTPUT_MSG_s Msg;
    extern UINT32 SvcCvFlow_PackRawInfo(memio_source_recv_raw_t *pRawInfo, ULONG Addr, UINT32 Size, UINT32 Pitch);

    AmbaMisra_TouchUnused(pRawInfoBuf);

    Rval = AmbaWrap_memset(&FeedCfg, 0, sizeof(SVC_CV_ALGO_FEED_CFG_s));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STIXEL, "memset FeedCfg failed", 0U, 0U);
    }
    FeedCfg.pIn  = pInBuf;
    FeedCfg.pOut = pOutBuf;

    /* Input: Disparity map */
    pInBuf->num_of_buf = 1U;
    AmbaMisra_TypeCast(&DataAddr, &pStixelInput->pAddr);
    DataAddr = DataAddr + (pModeCfg->VDispOffsetY * pStixelInput->Pitch);
    DataSize = pStixelInput->Pitch * (pStixelInput->Height - pModeCfg->VDispOffsetY);
    Rval = SvcCvFlow_PackRawInfo(pRawInfoBuf[0], DataAddr, DataSize, pStixelInput->Pitch);

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STIXEL, "SvcCvFlow_PackRawInfo failed", 0U, 0U);
    } else {
        SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STIXEL, pRawInfoBuf[0], pCtrl->RawInfoBufSize, &pInBuf->buf[0]);

        /* Output */
        pOutBuf->num_of_buf = pAlgoOp->OutputNum;
        for (i = 0U; i < pAlgoOp->OutputNum; i++) {
            Rval = AmbaWrap_memcpy(&pOutBuf->buf[i], &pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][i], sizeof(flexidag_memblk_t));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STIXEL, "memcpy OutputBufBlk failed", 0U, 0U);
            }
        }
        AmbaMisra_TypeCast(&FeedCfg.pUserData, &pUserDataBuf);
        pUserDataBuf->AlgoIndex = SVC_STIXEL_INDEX_VDISP;

        Rval = SvcCvAlgo_Feed(&pAlgoOp->AlgoHandle, &FeedCfg);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STIXEL, "SvcCvAlgo_Feed AlgoIndex(%u) error", SVC_STIXEL_INDEX_VDISP, 0U);
        } else {
            /* Wait complete */
            Rval = Stixel_MsgQueueRecv(&pCtrl->AlgoOutQueId, &Msg, SVC_STIXEL_INDEX_VDISP);

            SVC_CV_DBG_INFO("Tx[%u]: VDisp done", Inst, 0U, 0U, 0U, 0U);
        }

        if (SvcCvDebugDump == 1U) {
            const char* SaveNameSet[] = {"C:/Stixel_VDisp_dump0.raw"};
            ULONG  SaveAddrSet[1];
            UINT32 SaveFileSizeSet[1];

            if (SVC_OK != AmbaWrap_memcpy(&SaveAddrSet[0], &pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][0].pBuffer, (UINT32)sizeof(void *))) {
                SvcLog_NG(SVC_LOG_STIXEL, "typecast addr failed", 0U, 0U);
            }
            SaveFileSizeSet[0] = pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][0].buffer_size;
            Stixel_DebugSave(1U, SaveNameSet, SaveAddrSet, SaveFileSizeSet);
        }
    }

    return Rval;
}

static UINT32 Stixel_CalcCamPitch(UINT32 Inst,
                                  const SVC_CV_FLOW_STIXEL_DSI_INPUT_s *pStixelInput,
                                  CAM_PITCH_DET_LINE_s *pEstiGCLine,
                                  DOUBLE *pEstiCamPitch)
{
    UINT32 Rval = SVC_OK;
    SVC_STIXEL_CTRL_s *pCtrl = &g_StixelCtrl[Inst];
    const AMBA_CV_STIXEL_CFG_s *pStixelCfg;
    ULONG  DataAddr;

    const flexidag_memblk_t *pVDispOutput0;
    CAM_PITCH_DET_AUTO_PARAM_s AutoDetCfg;
    const CAM_PITCH_DET_MANUAL_HDLR_s *pManualDetHdlr;
    DOUBLE EstiCamHeight;
    DOUBLE EstiCamPitch;
    UINT16 RoadEstiMode;

    SVC_CV_DBG_INFO("CalcCamPitch[%u]: StartY(%u) W(%u) H(%u) Pc(%u)",
                  Inst, pCtrl->VDisp.StartY, pCtrl->VDisp.Width, pCtrl->VDisp.Height, pCtrl->VDisp.Precision);

    AmbaMisra_TypeCast(&pStixelCfg, &SvcCvFlow_Stixel_ModeInfo[pCtrl->pCvFlow->CvModeID].Cfg.pStixelCfg);

    /* VDisp output[0] */
    pVDispOutput0 = &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_VDISP].OutputBuf[pCtrl->OutBufIdx][0];
    AmbaMisra_TypeCast(&DataAddr, &pVDispOutput0->pBuffer);
    if (SVC_OK != SvcPlat_CacheInvalidate(DataAddr, pVDispOutput0->buffer_size)) {
        SvcLog_NG(SVC_LOG_STIXEL, "CalcCamPitch[%u]: Invalidate cache error", Inst, 0U);
    }
    AmbaMisra_TypeCast(&pCtrl->VDisp.pBuff, &pVDispOutput0->pBuffer);

    if (Stixel_Pref_Cfg.Used == 1U) {
        //SvcLog_NG(SVC_LOG_STIXEL, "Read from binary", 0U, 0U);
        RoadEstiMode = Stixel_Pref_Cfg.RoadEstiMode;
    } else {
        //SvcLog_NG(SVC_LOG_STIXEL, "Read from modeinfo", 0U, 0U);
        RoadEstiMode = pStixelCfg->RoadEstiMode;
    }


    if (RoadEstiMode == STIXEL_ROAD_ESTI_MODE_AUTO) {
        AutoDetCfg.IntCam.BaseLineMeter = pStixelCfg->IntParam.Baseline;
        AutoDetCfg.IntCam.FocalLengthPixel = pStixelCfg->IntParam.Fu;
        AutoDetCfg.IntCam.V0 = pStixelCfg->IntParam.V0;

        Rval = CamPhDet_AutoDetection((const UINT8 *)&pCtrl->AutoDetWorkBuf.pAddr,
                                      (const CAM_PITCH_DET_V_DISP_s *)&pCtrl->VDisp,
                                      (const CAM_PITCH_DET_AUTO_PARAM_s *)&AutoDetCfg,
                                      &EstiCamHeight, &EstiCamPitch, pEstiGCLine);

        StixelEstiCamHeight = EstiCamHeight;
        StixelEstiCamPitch = EstiCamPitch;

        if (SvcCvDebugEnable > 0U) {
            AmbaPrint_PrintInt5("(AutoDetection) Estimated Ground Disparity: %d ~ %d",
                (INT32)CamPhDet_GetGroundDispByV(0U, pEstiGCLine->m, pEstiGCLine->c),
                (INT32)CamPhDet_GetGroundDispByV(pStixelInput->Height - 1U, pEstiGCLine->m, pEstiGCLine->c), 0, 0, 0);
        }
    } else {
        CAM_PITCH_DET_MANUAL_THR_s VdispThr;
        DOUBLE XPixelCountThrDB;
        UINT32 QualifiedRoadEsti;

        XPixelCountThrDB = (DOUBLE)pCtrl->VDisp.Width;
        XPixelCountThrDB *= pStixelCfg->ManualDetCfg.VDispParam.XRatioThr;
        VdispThr.XPixelCountThr = (UINT32)XPixelCountThrDB;
        VdispThr.YRatioThr = pStixelCfg->ManualDetCfg.VDispParam.YRatioThr;
        AmbaMisra_TypeCast(&pManualDetHdlr, &pCtrl->ManualDetWorkBuf.pAddr);
        Rval = CamPhDet_ManualDetection((const CAM_PITCH_DET_V_DISP_s *)&pCtrl->VDisp,
                                        pManualDetHdlr, &VdispThr, &PriorEstimatedGround[Inst],
                                        &EstiCamHeight, &EstiCamPitch, pEstiGCLine, &QualifiedRoadEsti);
        if (SvcCvDebugEnable > 0U) {
            AmbaPrint_PrintInt5("(ManualDetection) Estimated Ground Disparity: %d ~ %d",
                (INT32)CamPhDet_GetGroundDispByV(0U, pEstiGCLine->m, pEstiGCLine->c),
                (INT32)CamPhDet_GetGroundDispByV(pStixelInput->Height - 1U, pEstiGCLine->m, pEstiGCLine->c), 0, 0, 0);
        }
    }

    *pEstiCamPitch = EstiCamPitch;

    {
        char EstiCamHeightStrBuff[50U];
        char EstiCamPitchStrBuff[50U];

        (void)AmbaUtility_DoubleToStr(&EstiCamHeightStrBuff[0U],
            sizeof(EstiCamHeightStrBuff), EstiCamHeight, 5U);
        (void)AmbaUtility_DoubleToStr(&EstiCamPitchStrBuff[0U],
            sizeof(EstiCamPitchStrBuff), EstiCamPitch, 5U);
        if (SvcCvDebugEnable > 1U) {
            AmbaPrint_PrintStr5("EstiCamHeight = %s, EstiCamPitch = %s\n",
                EstiCamHeightStrBuff, EstiCamPitchStrBuff, NULL, NULL, NULL);
        }
    }

    SVC_CV_DBG_INFO("Tx[%u]: CamPitch done", Inst, 0U, 0U, 0U, 0U);

    return Rval;
}

static UINT32 Stixel_GenEstiRoad(UINT32 Inst,
                                 const SVC_CV_FLOW_STIXEL_DSI_INPUT_s *pStixelInput,
                                 CAM_PITCH_DET_LINE_s *pEstiGCLine, DOUBLE EstiCamPitch,
                                 memio_source_recv_raw_t *pRawInfoBuf[],
                                 STIXEL_USER_DATA_s *pUserDataBuf)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    SVC_STIXEL_CTRL_s *pCtrl = &g_StixelCtrl[Inst];
    const AMBA_CV_STIXEL_CFG_s *pStixelCfg;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &pCtrl->AlgoOp[SVC_STIXEL_INDEX_GEN_ESTI_ROAD];
    const SVC_CV_FLOW_STIXEL_OP_EX_INFO_s *pAlgoOpEx = &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_GEN_ESTI_ROAD];
    AMBA_CV_FLEXIDAG_IO_s *pInBuf = &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_GEN_ESTI_ROAD].InBuf[pCtrl->OutBufIdx];
    AMBA_CV_FLEXIDAG_IO_s *pOutBuf = &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_GEN_ESTI_ROAD].OutBuf[pCtrl->OutBufIdx];
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;
    ULONG  DataAddr;
    STIXEL_OUTPUT_MSG_s Msg;
    extern UINT32 SvcCvFlow_PackRawInfo(memio_source_recv_raw_t *pRawInfo, ULONG Addr, UINT32 Size, UINT32 Pitch);

    static AMBA_CV_STIXEL_GEN_ESTI_ROAD_CFG_s *pVPStixelEstiRoadCfg = NULL;
    UINT32 BufSize;
    DOUBLE EstiSinCamPitch = 0.0;
    DOUBLE EstiCosCamPitch = 0.0;
    UINT32 RoadSectionIdx;

    AmbaMisra_TouchUnused(pEstiGCLine);
    AmbaMisra_TouchUnused(pRawInfoBuf);
    AmbaMisra_TypeCast(&pStixelCfg, &SvcCvFlow_Stixel_ModeInfo[pCtrl->pCvFlow->CvModeID].Cfg.pStixelCfg);

    (void) AmbaWrap_sin(EstiCamPitch, &EstiSinCamPitch);
    (void) AmbaWrap_cos(EstiCamPitch, &EstiCosCamPitch);

    AmbaMisra_TypeCast(&pVPStixelEstiRoadCfg, &pAlgoOpEx->InputBuf.pAddr);
    BufSize = pAlgoOpEx->InputBuf.Size;

    pVPStixelEstiRoadCfg->ImageV0 = (FLOAT)pStixelCfg->IntParam.V0;
    pVPStixelEstiRoadCfg->BaseLine = (FLOAT)pStixelCfg->IntParam.Baseline;
    pVPStixelEstiRoadCfg->FocalLength = (FLOAT)pStixelCfg->IntParam.Fu;
    pVPStixelEstiRoadCfg->DetObjectHeight = (FLOAT)pStixelCfg->DetObjectHeight;
    for (RoadSectionIdx = 0U; RoadSectionIdx < STIXEL_ROAD_SECTION_NUM; RoadSectionIdx++) {
        pVPStixelEstiRoadCfg->RoadSection[RoadSectionIdx].Bottom = (FLOAT)pStixelInput->Height - 1.f;
        pVPStixelEstiRoadCfg->RoadSection[RoadSectionIdx].M = (FLOAT)pEstiGCLine->m;
        pVPStixelEstiRoadCfg->RoadSection[RoadSectionIdx].C = (FLOAT)pEstiGCLine->c;
        pVPStixelEstiRoadCfg->RoadSection[RoadSectionIdx].SinCamPitch = (FLOAT)EstiSinCamPitch;
        pVPStixelEstiRoadCfg->RoadSection[RoadSectionIdx].CosCamPitch = (FLOAT)EstiCosCamPitch;
    }

    AmbaMisra_TypeCast(&DataAddr, &pVPStixelEstiRoadCfg);
    if (SVC_OK != SvcPlat_CacheClean(DataAddr, BufSize)) {
        SvcLog_NG(SVC_LOG_STIXEL, "EstiRoad[%u]: Clean cache error", Inst, 0U);
    }

    Rval = AmbaWrap_memset(&FeedCfg, 0, sizeof(SVC_CV_ALGO_FEED_CFG_s));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STIXEL, "memset FeedCfg failed", 0U, 0U);
    }
    FeedCfg.pIn  = pInBuf;
    FeedCfg.pOut = pOutBuf;

    /* Input: AMBA_CV_STIXEL_GEN_ESTI_ROAD_CFG_s */
    pInBuf->num_of_buf = 1U;
    Rval = SvcCvFlow_PackRawInfo(pRawInfoBuf[0], DataAddr, BufSize, BufSize);

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STIXEL, "SvcCvFlow_PackRawInfo failed", 0U, 0U);
    } else {
        SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STIXEL, pRawInfoBuf[0], pCtrl->RawInfoBufSize, &pInBuf->buf[0]);

        /* Output */
        pOutBuf->num_of_buf = pAlgoOp->OutputNum;
        for (i = 0U; i < pAlgoOp->OutputNum; i++) {
            Rval = AmbaWrap_memcpy(&pOutBuf->buf[i], &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_GEN_ESTI_ROAD].OutputBuf[pCtrl->OutBufIdx][i], sizeof(flexidag_memblk_t));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STIXEL, "memcpy OutputBufBlk failed", 0U, 0U);
            }
        }
        AmbaMisra_TypeCast(&FeedCfg.pUserData, &pUserDataBuf);
        pUserDataBuf->AlgoIndex = SVC_STIXEL_INDEX_GEN_ESTI_ROAD;

        Rval = SvcCvAlgo_Feed(&pAlgoOp->AlgoHandle, &FeedCfg);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STIXEL, "SvcCvAlgo_Feed AlgoIndex(%u) error", SVC_STIXEL_INDEX_GEN_ESTI_ROAD, 0U);
        } else {
            /* Wait complete */
            Rval = Stixel_MsgQueueRecv(&pCtrl->AlgoOutQueId, &Msg, SVC_STIXEL_INDEX_GEN_ESTI_ROAD);

            SVC_CV_DBG_INFO("Tx[%u]: EstiRoad done", Inst, 0U, 0U, 0U, 0U);
        }

        if (SvcCvDebugDump == 1U) {
            const char* SaveNameSet[] = {
                "C:/Stixel_GenEstiRoad_dump0.raw",
                "C:/Stixel_GenEstiRoad_dump1.raw",
                "C:/Stixel_GenEstiRoad_dump2.raw",
                "C:/Stixel_GenEstiRoad_dump3.raw"
            };
            ULONG  SaveAddrSet[4];
            UINT32 SaveFileSizeSet[4];

            if (SVC_OK != AmbaWrap_memcpy(&SaveAddrSet[0], &pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][0].pBuffer, (UINT32)sizeof(void *))) {
                SvcLog_NG(SVC_LOG_STIXEL, "typecast addr failed", 0U, 0U);
            }
            if (SVC_OK != AmbaWrap_memcpy(&SaveAddrSet[1], &pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][1].pBuffer, (UINT32)sizeof(void *))) {
                SvcLog_NG(SVC_LOG_STIXEL, "typecast addr failed", 0U, 0U);
            }
            if (SVC_OK != AmbaWrap_memcpy(&SaveAddrSet[2], &pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][2].pBuffer, (UINT32)sizeof(void *))) {
                SvcLog_NG(SVC_LOG_STIXEL, "typecast addr failed", 0U, 0U);
            }
            if (SVC_OK != AmbaWrap_memcpy(&SaveAddrSet[3], &pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][3].pBuffer, (UINT32)sizeof(void *))) {
                SvcLog_NG(SVC_LOG_STIXEL, "typecast addr failed", 0U, 0U);
            }
            SaveFileSizeSet[0] = pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][0].buffer_size;
            SaveFileSizeSet[1] = pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][1].buffer_size;
            SaveFileSizeSet[2] = pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][2].buffer_size;
            SaveFileSizeSet[3] = pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][3].buffer_size;
            Stixel_DebugSave(4U, SaveNameSet, SaveAddrSet, SaveFileSizeSet);
        }
    }

    return Rval;
}

static UINT32 Stixel_Preprocess(UINT32 Inst,
                                const SVC_CV_FLOW_STIXEL_DSI_INPUT_s *pStixelInput,
                                memio_source_recv_raw_t *pRawInfoBuf[],
                                STIXEL_USER_DATA_s *pUserDataBuf)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    SVC_STIXEL_CTRL_s *pCtrl = &g_StixelCtrl[Inst];
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &pCtrl->AlgoOp[SVC_STIXEL_INDEX_PREPROCESS];
    const SVC_CV_FLOW_STIXEL_OP_EX_INFO_s *pAlgoOpEx = &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_PREPROCESS];
    AMBA_CV_FLEXIDAG_IO_s *pInBuf = &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_PREPROCESS].InBuf[pCtrl->OutBufIdx];
    AMBA_CV_FLEXIDAG_IO_s *pOutBuf = &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_PREPROCESS].OutBuf[pCtrl->OutBufIdx];
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;
    ULONG  DataAddr;
    UINT32 DataSize;
    STIXEL_OUTPUT_MSG_s Msg;
    extern UINT32 SvcCvFlow_PackRawInfo(memio_source_recv_raw_t *pRawInfo, ULONG Addr, UINT32 Size, UINT32 Pitch);

    AmbaMisra_TouchUnused(pRawInfoBuf);

    Rval = AmbaWrap_memset(&FeedCfg, 0, sizeof(SVC_CV_ALGO_FEED_CFG_s));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STIXEL, "memset FeedCfg failed", 0U, 0U);
    }
    FeedCfg.pIn  = pInBuf;
    FeedCfg.pOut = pOutBuf;

    /* Input: Disparity map */
    pInBuf->num_of_buf = 1U;
    AmbaMisra_TypeCast(&DataAddr, &pStixelInput->pAddr);
    DataSize = pStixelInput->Pitch * pStixelInput->Height;
    Rval = SvcCvFlow_PackRawInfo(pRawInfoBuf[0], DataAddr, DataSize, pStixelInput->Pitch);

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STIXEL, "SvcCvFlow_PackRawInfo failed", 0U, 0U);
    } else {
        SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STIXEL, pRawInfoBuf[0], pCtrl->RawInfoBufSize, &pInBuf->buf[0]);

        /* Output */
        pOutBuf->num_of_buf = pAlgoOp->OutputNum;
        for (i = 0U; i < pAlgoOp->OutputNum; i++) {
            Rval = AmbaWrap_memcpy(&pOutBuf->buf[i], &pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][i], sizeof(flexidag_memblk_t));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STIXEL, "memcpy OutputBufBlk failed", 0U, 0U);
            }
        }
        AmbaMisra_TypeCast(&FeedCfg.pUserData, &pUserDataBuf);
        pUserDataBuf->AlgoIndex = SVC_STIXEL_INDEX_PREPROCESS;

        Rval = SvcCvAlgo_Feed(&pAlgoOp->AlgoHandle, &FeedCfg);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STIXEL, "SvcCvAlgo_Feed AlgoIndex(%u) error", SVC_STIXEL_INDEX_PREPROCESS, 0U);
        } else {
            /* Wait complete */
            Rval = Stixel_MsgQueueRecv(&pCtrl->AlgoOutQueId, &Msg, SVC_STIXEL_INDEX_PREPROCESS);

            SVC_CV_DBG_INFO("Tx[%u]: Preproc done", Inst, 0U, 0U, 0U, 0U);
        }

        if (SvcCvDebugDump == 1U) {
            const char* SaveNameSet[] = {"C:/Stixel_preprocess_dump0.raw"};
            ULONG SaveAddrSet[1];
            UINT32 SaveFileSizeSet[1];

            if (SVC_OK != AmbaWrap_memcpy(&SaveAddrSet[0], &pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][0].pBuffer, (UINT32)sizeof(void *))) {
                SvcLog_NG(SVC_LOG_STIXEL, "typecast addr failed", 0U, 0U);
            }
            SaveFileSizeSet[0] = pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][0].buffer_size;
            Stixel_DebugSave(1U, SaveNameSet, SaveAddrSet, SaveFileSizeSet);
        }
    }

    return Rval;
}

static UINT32 Stixel_RunFreeSpace(UINT32 Inst,
                                  const SVC_CV_FLOW_STIXEL_DSI_INPUT_s *pStixelInput,
                                  memio_source_recv_raw_t *pRawInfoBuf[],
                                  STIXEL_USER_DATA_s *pUserDataBuf)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    SVC_STIXEL_CTRL_s *pCtrl = &g_StixelCtrl[Inst];
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &pCtrl->AlgoOp[SVC_STIXEL_INDEX_FREE_SPACE];
    const SVC_CV_FLOW_STIXEL_OP_EX_INFO_s *pAlgoOpEx= &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_FREE_SPACE];
    AMBA_CV_FLEXIDAG_IO_s *pInBuf = &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_FREE_SPACE].InBuf[pCtrl->OutBufIdx];
    AMBA_CV_FLEXIDAG_IO_s *pOutBuf = &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_FREE_SPACE].OutBuf[pCtrl->OutBufIdx];
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;
    ULONG  DataAddr;
    STIXEL_OUTPUT_MSG_s Msg;
    extern UINT32 SvcCvFlow_PackRawInfo(memio_source_recv_raw_t *pRawInfo, ULONG Addr, UINT32 Size, UINT32 Pitch);

    AmbaMisra_TouchUnused(&pStixelInput);
    AmbaMisra_TouchUnused(pRawInfoBuf);

    Rval = AmbaWrap_memset(&FeedCfg, 0, sizeof(SVC_CV_ALGO_FEED_CFG_s));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STIXEL, "memset FeedCfg failed", 0U, 0U);
    }
    FeedCfg.pIn  = pInBuf;
    FeedCfg.pOut = pOutBuf;

    pInBuf->num_of_buf = 4U;
    /* Input[0]: preprocess output[0] */
    AmbaMisra_TypeCast(&DataAddr, &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_PREPROCESS].OutBuf[pCtrl->OutBufIdx].buf[0].pBuffer);
    Rval = SvcCvFlow_PackRawInfo(pRawInfoBuf[0],
                                 DataAddr,
                                 pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_PREPROCESS].OutBuf[pCtrl->OutBufIdx].buf[0].buffer_size,
                                 STIXEL_TRANS_DISPARITY_WIDTH * sizeof(UINT16));

    /* Input[1]: GenEstiRoad output[0] */
    AmbaMisra_TypeCast(&DataAddr, &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_GEN_ESTI_ROAD].OutBuf[pCtrl->OutBufIdx].buf[0].pBuffer);
    Rval |= SvcCvFlow_PackRawInfo(pRawInfoBuf[1],
                                 DataAddr,
                                 pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_GEN_ESTI_ROAD].OutBuf[pCtrl->OutBufIdx].buf[0].buffer_size,
                                 STIXEL_TRANS_DISPARITY_WIDTH * sizeof(UINT16));

    /* Input[2]: GenEstiRoad output[1] */
    AmbaMisra_TypeCast(&DataAddr, &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_GEN_ESTI_ROAD].OutBuf[pCtrl->OutBufIdx].buf[1].pBuffer);
    Rval |= SvcCvFlow_PackRawInfo(pRawInfoBuf[2],
                                 DataAddr,
                                 pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_GEN_ESTI_ROAD].OutBuf[pCtrl->OutBufIdx].buf[1].buffer_size,
                                 STIXEL_TRANS_DISPARITY_WIDTH * sizeof(UINT16));

    /* Input[3]: GenEstiRoad output[2] */
    AmbaMisra_TypeCast(&DataAddr, &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_GEN_ESTI_ROAD].OutBuf[pCtrl->OutBufIdx].buf[2].pBuffer);
    Rval |= SvcCvFlow_PackRawInfo(pRawInfoBuf[3],
                                 DataAddr,
                                 pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_GEN_ESTI_ROAD].OutBuf[pCtrl->OutBufIdx].buf[2].buffer_size,
                                 STIXEL_TRANS_DISPARITY_WIDTH * sizeof(UINT16));

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STIXEL, "SvcCvFlow_PackRawInfo failed", 0U, 0U);
    } else {
        SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STIXEL, pRawInfoBuf[0], pCtrl->RawInfoBufSize, &pInBuf->buf[0]);
        SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STIXEL, pRawInfoBuf[1], pCtrl->RawInfoBufSize, &pInBuf->buf[1]);
        SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STIXEL, pRawInfoBuf[2], pCtrl->RawInfoBufSize, &pInBuf->buf[2]);
        SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STIXEL, pRawInfoBuf[3], pCtrl->RawInfoBufSize, &pInBuf->buf[3]);

        /* Output */
        pOutBuf->num_of_buf = pAlgoOp->OutputNum;
        for (i = 0U; i < pAlgoOp->OutputNum; i++) {
            Rval = AmbaWrap_memcpy(&pOutBuf->buf[i], &pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][i], sizeof(flexidag_memblk_t));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STIXEL, "memcpy OutputBufBlk failed", 0U, 0U);
            }
        }
        AmbaMisra_TypeCast(&FeedCfg.pUserData, &pUserDataBuf);
        pUserDataBuf->AlgoIndex = SVC_STIXEL_INDEX_FREE_SPACE;

        Rval = SvcCvAlgo_Feed(&pAlgoOp->AlgoHandle, &FeedCfg);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STIXEL, "SvcCvAlgo_Feed AlgoIndex(%u) error", SVC_STIXEL_INDEX_FREE_SPACE, 0U);
        } else {
            /* Wait complete */
            Rval = Stixel_MsgQueueRecv(&pCtrl->AlgoOutQueId, &Msg, SVC_STIXEL_INDEX_FREE_SPACE);

            SVC_CV_DBG_INFO("Tx[%u]: FreeSpace done", Inst, 0U, 0U, 0U, 0U);
        }

        if (SvcCvDebugDump == 1U) {
            const char* SaveNameSet[] = {
                "C:/Stixel_free_space_dump0.raw",
                "C:/Stixel_free_space_dump1.raw"
            };
            ULONG  SaveAddrSet[2];
            UINT32 SaveFileSizeSet[2];

            if (SVC_OK != AmbaWrap_memcpy(&SaveAddrSet[0], &pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][0].pBuffer, (UINT32)sizeof(void *))) {
                SvcLog_NG(SVC_LOG_STIXEL, "typecast addr failed", 0U, 0U);
            }
            if (SVC_OK != AmbaWrap_memcpy(&SaveAddrSet[1], &pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][1].pBuffer, (UINT32)sizeof(void *))) {
                SvcLog_NG(SVC_LOG_STIXEL, "typecast addr failed", 0U, 0U);
            }

            SaveFileSizeSet[0] = pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][0].buffer_size;
            SaveFileSizeSet[1] = pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][1].buffer_size;
            Stixel_DebugSave(2U, SaveNameSet, SaveAddrSet, SaveFileSizeSet);
        }
    }

    return Rval;
}

static UINT32 Stixel_RunHeightSegment(UINT32 Inst,
                                      const SVC_CV_FLOW_STIXEL_DSI_INPUT_s *pStixelInput,
                                      memio_source_recv_raw_t *pRawInfoBuf[],
                                      STIXEL_USER_DATA_s *pUserDataBuf)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    SVC_STIXEL_CTRL_s *pCtrl = &g_StixelCtrl[Inst];
    const AMBA_CV_STIXEL_CFG_s *pStixelCfg;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &pCtrl->AlgoOp[SVC_STIXEL_INDEX_HEIGHT_SEG];
    const SVC_CV_FLOW_STIXEL_OP_EX_INFO_s *pAlgoOpEx = &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_HEIGHT_SEG];
    AMBA_CV_FLEXIDAG_IO_s *pInBuf = &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_HEIGHT_SEG].InBuf[pCtrl->OutBufIdx];
    AMBA_CV_FLEXIDAG_IO_s *pOutBuf = &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_HEIGHT_SEG].OutBuf[pCtrl->OutBufIdx];
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;
    ULONG  DataAddr;
    STIXEL_OUTPUT_MSG_s Msg;
    extern UINT32 SvcCvFlow_PackRawInfo(memio_source_recv_raw_t *pRawInfo, ULONG Addr, UINT32 Size, UINT32 Pitch);

    AMBA_CV_STIXEL_HEIGHT_SEG_CFG_s *pVPStixelHeightSegCfg = NULL;

    AmbaMisra_TouchUnused(&pStixelInput);
    AmbaMisra_TouchUnused(pRawInfoBuf);
    AmbaMisra_TypeCast(&pStixelCfg, &SvcCvFlow_Stixel_ModeInfo[pCtrl->pCvFlow->CvModeID].Cfg.pStixelCfg);

    AmbaMisra_TypeCast(&pVPStixelHeightSegCfg, &pAlgoOpEx->InputBuf.pAddr);
    pVPStixelHeightSegCfg->MinDisparity = pStixelCfg->MinDisparity;
    AmbaMisra_TypeCast(&DataAddr, &pVPStixelHeightSegCfg);
    if (SVC_OK != SvcPlat_CacheClean(DataAddr, pAlgoOpEx->InputBuf.Size)) {
        SvcLog_NG(SVC_LOG_STIXEL, "HeightSeg[%u]: Clean cache error", Inst, 0U);
    }

    Rval = AmbaWrap_memset(&FeedCfg, 0, sizeof(SVC_CV_ALGO_FEED_CFG_s));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STIXEL, "memset FeedCfg failed", 0U, 0U);
    }
    FeedCfg.pIn  = pInBuf;
    FeedCfg.pOut = pOutBuf;

    pInBuf->num_of_buf = 4U;
    /* Input[0]: preprocess output[0] */
    AmbaMisra_TypeCast(&DataAddr, &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_PREPROCESS].OutBuf[pCtrl->OutBufIdx].buf[0].pBuffer);
    Rval = SvcCvFlow_PackRawInfo(pRawInfoBuf[0],
                                  DataAddr,
                                  pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_PREPROCESS].OutBuf[pCtrl->OutBufIdx].buf[0].buffer_size,
                                  STIXEL_TRANS_DISPARITY_WIDTH * sizeof(UINT16));

    /* Input[1]: free space output[0] */
    AmbaMisra_TypeCast(&DataAddr, &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_FREE_SPACE].OutBuf[pCtrl->OutBufIdx].buf[0].pBuffer);
    Rval |= SvcCvFlow_PackRawInfo(pRawInfoBuf[1],
                                  DataAddr,
                                  pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_FREE_SPACE].OutBuf[pCtrl->OutBufIdx].buf[0].buffer_size,
                                  STIXEL_TRANS_DISPARITY_HEIGHT * sizeof(UINT16));

    /* Input[2]: gen esti Road output[3] */
    AmbaMisra_TypeCast(&DataAddr, &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_GEN_ESTI_ROAD].OutBuf[pCtrl->OutBufIdx].buf[3].pBuffer);
    Rval |= SvcCvFlow_PackRawInfo(pRawInfoBuf[2],
                                  DataAddr,
                                  pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_GEN_ESTI_ROAD].OutBuf[pCtrl->OutBufIdx].buf[3].buffer_size,
                                  STIXEL_IN_MAX_DISPARITY * sizeof(UINT16));

    /* Input[3]: hight seg config */
    AmbaMisra_TypeCast(&DataAddr, &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_HEIGHT_SEG].InputBuf.pAddr);
    Rval |= SvcCvFlow_PackRawInfo(pRawInfoBuf[3],
                                  DataAddr,
                                  pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_HEIGHT_SEG].InputBuf.Size,
                                  pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_HEIGHT_SEG].InputBuf.Size);

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STIXEL, "SvcCvFlow_PackRawInfo failed", 0U, 0U);
    } else {
        SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STIXEL, pRawInfoBuf[0], pCtrl->RawInfoBufSize, &pInBuf->buf[0]);
        SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STIXEL, pRawInfoBuf[1], pCtrl->RawInfoBufSize, &pInBuf->buf[1]);
        SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STIXEL, pRawInfoBuf[2], pCtrl->RawInfoBufSize, &pInBuf->buf[2]);
        SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STIXEL, pRawInfoBuf[3], pCtrl->RawInfoBufSize, &pInBuf->buf[3]);

        /* Output */
        pOutBuf->num_of_buf = pAlgoOp->OutputNum;
        for (i = 0U; i < pAlgoOp->OutputNum; i++) {
            Rval = AmbaWrap_memcpy(&pOutBuf->buf[i], &pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][i], sizeof(flexidag_memblk_t));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STIXEL, "memcpy OutputBufBlk failed", 0U, 0U);
            }
        }
        AmbaMisra_TypeCast(&FeedCfg.pUserData, &pUserDataBuf);
        pUserDataBuf->AlgoIndex = SVC_STIXEL_INDEX_HEIGHT_SEG;

        Rval = SvcCvAlgo_Feed(&pAlgoOp->AlgoHandle, &FeedCfg);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STIXEL, "SvcCvAlgo_Feed AlgoIndex(%u) error", SVC_STIXEL_INDEX_HEIGHT_SEG, 0U);
        } else {
            /* Wait complete */
            Rval = Stixel_MsgQueueRecv(&pCtrl->AlgoOutQueId, &Msg, SVC_STIXEL_INDEX_HEIGHT_SEG);

            SVC_CV_DBG_INFO("Tx[%u]: HeightSeg done", Inst, 0U, 0U, 0U, 0U);
        }

        if (SvcCvDebugDump == 1U) {
            const char* SaveNameSet[] = {
                "C:/Stixel_height_seg_dump0.raw",
                "C:/Stixel_height_seg_dump1.raw"
            };
            ULONG SaveAddrSet[2];
            UINT32 SaveFileSizeSet[2];

            if (SVC_OK != AmbaWrap_memcpy(&SaveAddrSet[0], &pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][0].pBuffer, (UINT32)sizeof(void *))) {
                SvcLog_NG(SVC_LOG_STIXEL, "typecast addr failed", 0U, 0U);
            }
            if (SVC_OK != AmbaWrap_memcpy(&SaveAddrSet[1], &pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][1].pBuffer, (UINT32)sizeof(void *))) {
                SvcLog_NG(SVC_LOG_STIXEL, "typecast addr failed", 0U, 0U);
            }

            SaveFileSizeSet[0] = pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][0].buffer_size;
            SaveFileSizeSet[1] = pAlgoOpEx->OutputBuf[pCtrl->OutBufIdx][1].buffer_size;
            Stixel_DebugSave(2U, SaveNameSet, SaveAddrSet, SaveFileSizeSet);
        }
    }

    return Rval;
}

/**
* TX data source handler: process input data
*/
static UINT32 Stixel_TxDataSrcHandler(UINT32 Inst, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_STIXEL_CTRL_s *pCtrl = &g_StixelCtrl[Inst];
    static STIXEL_USER_DATA_s UserData[SVC_STIXEL_MAX_INSTANCE];
    STIXEL_USER_DATA_s *pUserDataBuf = &UserData[Inst];

    const SVC_CV_FLOW_STIXEL_DSI_INPUT_s *pStixelInput;
    ULONG Addr;
    CAM_PITCH_DET_LINE_s EstiGCLine;
    DOUBLE EstiCamPitch = 0.0;

    AmbaMisra_TouchUnused(pInfo);

    SVC_CV_DBG(SVC_LOG_STIXEL, "Tx[%u]: run %d", Inst, pCtrl->TxSeqNum);

    if (Event == SVC_STIXEL_INPUT_DSI) {
        if ((pCtrl->Enable == 1U) &&
            (0U == (pCtrl->SrcSeqNum % pCtrl->SrcRateCtrl))) {
            Rval = Stixel_SemTake(Inst);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pStixelInput, &pInfo);
                AmbaMisra_TypeCast(&Addr, &pStixelInput->pAddr);
                SVC_CV_DBG_INFO("Tx[%u]: Recv DSI Addr(0x%x) W(%u) H(%u) P(%u)",
                              Inst, (UINT32)Addr, pStixelInput->Width, pStixelInput->Height, pStixelInput->Pitch);

                Rval = Stixel_RunVDisp(Inst, pStixelInput, pCtrl->pRawInfoBuf, pUserDataBuf);

                if (SVC_OK == Rval) {
                    Rval = Stixel_CalcCamPitch(Inst, pStixelInput, &EstiGCLine/*output*/, &EstiCamPitch/*output*/);
                }

                if (SVC_OK == Rval) {
                    Rval = Stixel_GenEstiRoad(Inst, pStixelInput, &EstiGCLine, EstiCamPitch, pCtrl->pRawInfoBuf, pUserDataBuf);
                }

                if (SVC_OK == Rval) {
                    Rval = Stixel_Preprocess(Inst, pStixelInput, pCtrl->pRawInfoBuf, pUserDataBuf);
                }

                if (SVC_OK == Rval) {
                    Rval = Stixel_RunFreeSpace(Inst, pStixelInput, pCtrl->pRawInfoBuf, pUserDataBuf);
                }

                if (SVC_OK == Rval) {
                    Rval = Stixel_RunHeightSegment(Inst, pStixelInput, pCtrl->pRawInfoBuf, pUserDataBuf);
                }

                /* To stixel */
                if (SVC_OK == Rval) {
                    static SVC_CV_FLOW_STIXEL_OUTPUT_s SvcStixelOut[SVC_STIXEL_MAX_INSTANCE][SVC_STIXEL_OUTPUT_DEPTH];
                    SVC_CV_FLOW_STIXEL_OUTPUT_s *pSvcStixelOut;
                    const UINT16 *pStixelBottomPos;
                    const UINT16 *pStixelConfidence;
                    const UINT16 *pStixelTopPos;
                    const UINT16 *pStixelAvgDisparity;
                    UINT32 StixelIdx;

                    /* free space output[0] */
                    AmbaMisra_TypeCast(&pStixelBottomPos,
                                         &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_FREE_SPACE].OutputBuf[pCtrl->OutBufIdx][0].pBuffer);
                    /* free space output[1] */
                    AmbaMisra_TypeCast(&pStixelConfidence,
                                         &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_FREE_SPACE].OutputBuf[pCtrl->OutBufIdx][1].pBuffer);
                    /* Height segmentation output[0] */
                    AmbaMisra_TypeCast(&pStixelTopPos,
                                         &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_HEIGHT_SEG].OutputBuf[pCtrl->OutBufIdx][0].pBuffer);
                    /* Height segmentation output[1] */
                    AmbaMisra_TypeCast(&pStixelAvgDisparity,
                                         &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_HEIGHT_SEG].OutputBuf[pCtrl->OutBufIdx][1].pBuffer);

                    pSvcStixelOut = &SvcStixelOut[Inst][pCtrl->OutBufIdx];
                    Rval = AmbaWrap_memset(pSvcStixelOut, 0, sizeof(SVC_CV_FLOW_STIXEL_OUTPUT_s));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_STIXEL, "memset pStixelOut failed", 0U, 0U);
                    }
                    pSvcStixelOut->MsgCode = SVC_CV_STIXEL_OUTPUT;
                    pSvcStixelOut->StixelOut.StixelNum = STIXEL_OUT_STIXEL_NUM;
                    pSvcStixelOut->StixelOut.Width = STIXEL_COLUMN_WIDTH;
                    for (StixelIdx = 0U; StixelIdx < pSvcStixelOut->StixelOut.StixelNum; StixelIdx++)
                    {
                        AMBA_CV_STIXEL_s *pStixel = &pSvcStixelOut->StixelOut.StixelList[StixelIdx];
                        pStixel->Top = pStixelTopPos[StixelIdx];
                        pStixel->Bottom = pStixelBottomPos[StixelIdx];
                        pStixel->AvgDisparity = pStixelAvgDisparity[StixelIdx];
                        pStixel->Confidence = pStixelConfidence[StixelIdx];
                    }

                    pSvcStixelOut->FrameNum = pStixelInput->FrameNum;
                    if (SvcCvDebugDump == 1U) {
                        UINT8 *pU8;
                        AMBA_CV_STIXEL_DET_OUT_s *pOut = &pSvcStixelOut->StixelOut;
                        AmbaMisra_TypeCast(&pU8, &pOut);
                        Stixel_SaveBin("c:/stixel_out.bin", pU8, sizeof(AMBA_CV_STIXEL_DET_OUT_s));
                    }


                    Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                           SVC_CV_FLOW_OUTPUT_PORT0,
                                           pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                           pSvcStixelOut);

                }

                Rval = Stixel_SemGive(Inst);

                if (SVC_OK == Rval) {
                    pCtrl->TxSeqNum++;
                    pCtrl->OutBufIdx++;
                    if (pCtrl->OutBufIdx >= SVC_STIXEL_OUTPUT_DEPTH) {
                        pCtrl->OutBufIdx = 0;
                    }
                } else {
                    SvcLog_NG(SVC_LOG_STIXEL, "Tx[%u]: send error", Inst, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_STIXEL, "Tx[%u]: SemTake error", Inst, 0U);
            }
        } else {
            /* Do nothing */
        }

        pCtrl->SrcSeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_STIXEL, "Tx[%u]: unsupported event(0x%x)", Inst, Event);
        Rval = SVC_NG;
    }

    SVC_CV_DBG(SVC_LOG_STIXEL, "Tx[%u]: end", Inst, 0U);

    return Rval;
}

static UINT32 Stixel_MsgQueueSend(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, UINT32 AlgoIndex, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Rval;
    STIXEL_OUTPUT_MSG_s Msg;

    Msg.AlgoIndex = AlgoIndex;
    if (NULL != pEventData) {
        Rval = AmbaWrap_memcpy(&Msg.AlgoOut, pEventData, sizeof(SVC_CV_ALGO_OUTPUT_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STIXEL, "memcpy AlgoOut failed", 0U, 0U);
        }
    }
    Rval = AmbaKAL_MsgQueueSend(pMsgQueue, &Msg, 5000);

    return Rval;
}

static UINT32 Stixel_MsgQueueRecv(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, STIXEL_OUTPUT_MSG_s *pMsg, UINT32 WaitAlgoIndex)
{
    UINT32 Rval;

    Rval = AmbaKAL_MsgQueueReceive(pMsgQueue, pMsg, 1000);
    if (SVC_OK == Rval) {
        if (pMsg->AlgoIndex != WaitAlgoIndex) {
            SvcLog_NG(SVC_LOG_STIXEL, "Wait AlgoIndex(0x%x) msg failed. Recv AlgoIndex(%u)", pMsg->AlgoIndex, WaitAlgoIndex);
            Rval = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_STIXEL, "Wait AlgoIndex(0x%x) msg failed", WaitAlgoIndex, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

/**
* Algorithm output event callback function
*/
static UINT32 Stixel_AlgoOutCallback(UINT32 Inst, UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Rval = SVC_OK;
    SVC_STIXEL_CTRL_s *pCtrl = &g_StixelCtrl[Inst];
    const STIXEL_USER_DATA_s *pUserData;

    if (Event == CALLBACK_EVENT_OUTPUT) {
        if (NULL != pEventData->pUserData) {
            AmbaMisra_TypeCast(&pUserData, &pEventData->pUserData);

            if (pUserData->AlgoIndex < SVC_STIXEL_INDEX_MAX) {
                Rval = Stixel_MsgQueueSend(&pCtrl->AlgoOutQueId, pUserData->AlgoIndex, pEventData);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STIXEL, "AlgoOutCallback[%u]: MsgQueueSend error", Inst, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_STIXEL, "AlgoOutCallback[%u]: Invalid algo index(%u)", Inst, pUserData->AlgoIndex);
            }
        } else {
            SvcLog_NG(SVC_LOG_STIXEL, "AlgoOutCallback[%u]: Invalid user data", Inst, 0U);
        }
    }

    return Rval;
}

static UINT32 Stixel_AlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Stixel_AlgoOutCallback(SVC_STIXEL_INSTANCE0, Event, pEventData);
}

static UINT32 Stixel_AlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Stixel_AlgoOutCallback(SVC_STIXEL_INSTANCE1, Event, pEventData);
}

/**
* Set and wait TX and RX task to idle state
*/
static UINT32 Stixel_SetTaskIdle(UINT32 Inst)
{
    UINT32 Rval;
    SVC_STIXEL_CTRL_s *pCtrl = &g_StixelCtrl[Inst];
    UINT32 i;

    /* Disable input and output */
    pCtrl->Enable = 0U;

    /* Wait Rx idle */
    for (i = 0; i < SVC_STIXEL_SEM_INIT_COUNT; i++) {
        Rval = Stixel_SemTake(Inst);
    }

    return Rval;
}

/**
* Send Disparity image to driver
*/
static UINT32 Stixel_SendDsiInput(UINT32 Inst, const void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_STIXEL_CTRL_s *pCtrl = &g_StixelCtrl[Inst];
    void *pInputImg; /* SVC_CV_FLOW_STIXEL_DSI_INPUT_s */

    if (NULL != pCtrl->pTxProc) {
        AmbaMisra_TypeCast(&pInputImg, &pInfo);
        Rval = pCtrl->pTxProc(Inst, SVC_STIXEL_INPUT_DSI, pInputImg);
    }

    return Rval;
}

/**
* Send Disparity image to driver
*/
static UINT32 Stixel_Get_RoadEstimate(UINT32 Inst, const void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_CV_FLOW_STIXEL_PREF_STIXEL_CFG_s *pcfg;
    static char EstiCamHeightStrBuff[50U];
    static char EstiCamPitchStrBuff[50U];

    AmbaMisra_TypeCast(&pcfg, &pInfo);

    (void)Inst;

    pcfg->EstiCamHeight = StixelEstiCamHeight;
    pcfg->EstiCamPitch = StixelEstiCamPitch;

    (void)AmbaUtility_DoubleToStr(&EstiCamHeightStrBuff[0U], sizeof(EstiCamHeightStrBuff), pcfg->EstiCamHeight, 5U);
    (void)AmbaUtility_DoubleToStr(&EstiCamPitchStrBuff[0U], sizeof(EstiCamPitchStrBuff), pcfg->EstiCamPitch, 5U);

    AmbaPrint_PrintStr5("[Stixel_Get_RoadEstimate] EstiCamHeight = %s, EstiCamPitch = %s\n", EstiCamHeightStrBuff, EstiCamPitchStrBuff, NULL, NULL, NULL);

    return Rval;
}

/**
* Send Disparity image to driver
*/
static UINT32 Stixel_Set_RoadEstimate(UINT32 Inst, const void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_CV_FLOW_STIXEL_PREF_STIXEL_CFG_s *pcfg = NULL;

    AmbaMisra_TypeCast(&pcfg, &pInfo);

    (void)Inst;

    Rval = AmbaWrap_memset(&Stixel_Pref_Cfg, 0, sizeof(Stixel_Pref_Cfg));
    Stixel_Pref_Cfg.Used = pcfg->Used;
    Stixel_Pref_Cfg.RoadEstiMode = pcfg->RoadEstiMode;
    Stixel_Pref_Cfg.EstiCamHeight = pcfg->EstiCamHeight;
    Stixel_Pref_Cfg.EstiCamPitch = pcfg->EstiCamPitch;

    return Rval;
}

/**
* Entry of configuration function
*/
static UINT32 Stixel_Config(UINT32 Inst, UINT32 AlgoIdx)
{
    AmbaMisra_TouchUnused(&Inst);
    AmbaMisra_TouchUnused(&AlgoIdx);
    return SVC_OK;
}

static UINT32 Stixel_ConfigBuffer(UINT32 Inst)
{
    UINT32 Rval = SVC_OK;
    SVC_STIXEL_CTRL_s *pCtrl = &g_StixelCtrl[Inst];
    const AMBA_CV_STIXEL_CFG_s *pStixelCfg;
    UINT32 BufSize, i;
    CAM_PITCH_DET_MANUAL_HDLR_s *pManualDetHdlr;
    SVC_CV_FLOW_BUF_INFO_s BufInfo;
    char EstiCamHeightStrBuff[50U];
    char EstiCamPitchStrBuff[50U];

    AmbaMisra_TypeCast(&pStixelCfg, &SvcCvFlow_Stixel_ModeInfo[pCtrl->pCvFlow->CvModeID].Cfg.pStixelCfg);

    /* Raw info buffer */
    if (pCtrl->pRawInfoBuf[0] == NULL) {
        for (i = 0; i < SVC_STIXEL_ALGO_MAX_INPUT_NUM; i++) {
            Rval = SvcCvFlow_AllocWorkBuf(sizeof(memio_source_recv_raw_t), &BufInfo);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pCtrl->pRawInfoBuf[i], &BufInfo.pAddr);
                pCtrl->RawInfoBufSize = BufInfo.Size;
            } else {
                SvcLog_NG(SVC_LOG_STIXEL, "Alloc Raw info buf failed", 0U, 0U);
                break;
            }
        }
    }

    /* GenEstiRoad */
    if (SVC_OK == Rval) {
        Rval = SvcCvFlow_AllocWorkBuf(sizeof(AMBA_CV_STIXEL_GEN_ESTI_ROAD_CFG_s),
                                      &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_GEN_ESTI_ROAD].InputBuf);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STIXEL, "Alloc GenEstiRoad buf failed", 0U, 0U);
        }
    }

    /* Alloc auto detection work buffer */
    if (SVC_OK == Rval) {
        Rval = CamPhDet_GetAutoDetWorkSize(&BufSize);
        if (SVC_OK == Rval) {
            Rval = SvcCvFlow_AllocWorkBuf(BufSize,
                                          &pCtrl->AutoDetWorkBuf);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STIXEL, "Alloc AutoDet buf failed", 0U, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_STIXEL, "GetAutoDetWorkSize failed", 0U, 0U);
        }
    }

    /* Alloc manual detection work buffer */
    if (SVC_OK == Rval) {
        Rval = SvcCvFlow_AllocWorkBuf(sizeof(CAM_PITCH_DET_MANUAL_HDLR_s), &pCtrl->ManualDetWorkBuf);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STIXEL, "Alloc ManualDet buf failed", 0U, 0U);
        } else {
            pCtrl->VDisp.Precision = STIXEL_OUT_VDISPARITY_PRECISION;
            pCtrl->VDisp.StartY = STIXEL_IN_VDISPARITY_START_Y;
            pCtrl->VDisp.Height = STIXEL_IN_VDISPARITY_HEIGHT;
            pCtrl->VDisp.Width = STIXEL_OUT_VDISPARITY_WIDTH;
            AmbaMisra_TypeCast(&pManualDetHdlr, &pCtrl->ManualDetWorkBuf.pAddr);

            if (Stixel_Pref_Cfg.Used == 1U) {
                SvcLog_OK(SVC_LOG_STIXEL, "Load setting from binary ", 0U, 0U);
                if (Stixel_Pref_Cfg.RoadEstiMode == STIXEL_ROAD_ESTI_MODE_MANUAL) {
                    SvcLog_OK(SVC_LOG_STIXEL, "STIXEL_ROAD_ESTI_MODE_MANUAL", 0U, 0U);
                    (void)AmbaUtility_DoubleToStr(&EstiCamHeightStrBuff[0U], sizeof(EstiCamHeightStrBuff), Stixel_Pref_Cfg.EstiCamHeight, 5U);
                    (void)AmbaUtility_DoubleToStr(&EstiCamPitchStrBuff[0U], sizeof(EstiCamPitchStrBuff), Stixel_Pref_Cfg.EstiCamPitch, 5U);
                    AmbaPrint_PrintStr5("[Stixel_ConfigBuffer] EstiCamHeight = %s, EstiCamPitch = %s\n", EstiCamHeightStrBuff, EstiCamPitchStrBuff, NULL, NULL, NULL);
                    Rval = CamPhDet_InitManualDetection(Stixel_Pref_Cfg.EstiCamHeight,
                                                        Stixel_Pref_Cfg.EstiCamPitch,
                                                        pStixelCfg->IntParam.V0,
                                                        pStixelCfg->IntParam.Baseline,
                                                        pStixelCfg->IntParam.Fu,
                                                        (DOUBLE)pCtrl->VDisp.Precision,
                                                        pStixelCfg->ManualDetCfg.PitchStride,
                                                        pStixelCfg->ManualDetCfg.HeightStride,
                                                        pStixelCfg->ManualDetCfg.PitchSetNum,
                                                        pStixelCfg->ManualDetCfg.HeightSetNum,
                                                        &PriorEstimatedGround[Inst],
                                                        pManualDetHdlr);
                } else {
                    SvcLog_OK(SVC_LOG_STIXEL, "STIXEL_ROAD_ESTI_MODE_AUTO", 0U, 0U);
                }
            } else {
                SvcLog_OK(SVC_LOG_STIXEL, "Load setting from Modeinfo", 0U, 0U);
                if (pStixelCfg->RoadEstiMode == STIXEL_ROAD_ESTI_MODE_MANUAL) {
                    SvcLog_OK(SVC_LOG_STIXEL, "STIXEL_ROAD_ESTI_MODE_MANUAL", 0U, 0U);
                    (void)AmbaUtility_DoubleToStr(&EstiCamHeightStrBuff[0U], sizeof(EstiCamHeightStrBuff), pStixelCfg->ManualDetCfg.ExtParam.Height, 5U);
                    (void)AmbaUtility_DoubleToStr(&EstiCamPitchStrBuff[0U], sizeof(EstiCamPitchStrBuff), pStixelCfg->ManualDetCfg.ExtParam.Pitch, 5U);
                    AmbaPrint_PrintStr5("[Stixel_ConfigBuffer] EstiCamHeight = %s, EstiCamPitch = %s\n", EstiCamHeightStrBuff, EstiCamPitchStrBuff, NULL, NULL, NULL);
                    Rval = CamPhDet_InitManualDetection(pStixelCfg->ManualDetCfg.ExtParam.Height,
                                                        pStixelCfg->ManualDetCfg.ExtParam.Pitch,
                                                        pStixelCfg->IntParam.V0,
                                                        pStixelCfg->IntParam.Baseline,
                                                        pStixelCfg->IntParam.Fu,
                                                        (DOUBLE)pCtrl->VDisp.Precision,
                                                        pStixelCfg->ManualDetCfg.PitchStride,
                                                        pStixelCfg->ManualDetCfg.HeightStride,
                                                        pStixelCfg->ManualDetCfg.PitchSetNum,
                                                        pStixelCfg->ManualDetCfg.HeightSetNum,
                                                        &PriorEstimatedGround[Inst],
                                                        pManualDetHdlr);
                } else {
                    SvcLog_OK(SVC_LOG_STIXEL, "STIXEL_ROAD_ESTI_MODE_AUTO", 0U, 0U);
                }
            }
        }
    }

    /* Height segmentation */
    if (SVC_OK == Rval) {
        Rval = SvcCvFlow_AllocWorkBuf(sizeof(AMBA_CV_STIXEL_HEIGHT_SEG_CFG_s),
                                      &pCtrl->AlgoOpEx[SVC_STIXEL_INDEX_HEIGHT_SEG].InputBuf);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STIXEL, "Alloc GenEstiRoad buf failed", 0U, 0U);
        }
    }

    return Rval;
}

/**
* CvFlow driver initialization function
*/
static UINT32 CvFlow_StixelInit(UINT32 CvFlowChan, void *pInit)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    static UINT32 g_StixelInit = 0U;

    (void) CvFlowChan;
    (void) pInit;

    if (0U == g_StixelInit) {
        g_StixelInit = 1U;

        if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&StixelMutex, NULL)) {
            SvcLog_NG(SVC_LOG_STIXEL, "MutexCreate error", 0U, 0U);
            Rval = SVC_NG;
        }
        for (i = 0; i < SVC_STIXEL_MAX_INSTANCE; i++) {
            g_StixelCtrl[i].Used = 0;
        }

        SVC_CV_DBG(SVC_LOG_STIXEL, "CvFlow_StixelInit: first init", 0U, 0U);
    }

    SvcLog_OK(SVC_LOG_STIXEL, "CvFlow_StixelInit: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver configuration function
*/
static UINT32 CvFlow_StixelConfig(UINT32 CvFlowChan, void *pConfig)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    SVC_STIXEL_CTRL_s *pCtrl = NULL;
    const SVC_CV_FLOW_STIXEL_MODE_INFO_s *pModeInfo;

    (void) pConfig;

    if (NULL != pConfig) {
        Stixel_MutexTake();
        /* Create instance */
        for (i = 0; i < SVC_STIXEL_MAX_INSTANCE; i++) {
            if (0U == g_StixelCtrl[i].Used) {
                Rval = AmbaWrap_memset(&g_StixelCtrl[i], 0, sizeof(SVC_STIXEL_CTRL_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STIXEL, "memset g_StixelCtrl[%u] failed", i, 0U);
                }
                pCtrl = &g_StixelCtrl[i];
                pCtrl->Used = 1;
                SvcLog_DBG(SVC_LOG_STIXEL, "Create Inst(%u)", i, 0U);
                break;
            }
        }
        Stixel_MutexGive();

        /* Configure instance */
        if (NULL != pCtrl) {
            pCtrl->CvfChan     = (UINT8) CvFlowChan;
            pCtrl->pCvFlow     = &pCtrl->CvCfg;
            Rval = AmbaWrap_memcpy(&pCtrl->CvCfg, pConfig, sizeof(SVC_CV_FLOW_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STIXEL, "memcpy CvCfg failed", 0U, 0U);
            }
            pCtrl->SrcRateCtrl = 1;
            pCtrl->Enable      = 1;

            pModeInfo = &SvcCvFlow_Stixel_ModeInfo[pCtrl->pCvFlow->CvModeID];
            if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG) {
                Rval = SVC_OK;
            } else {
                SvcLog_NG(SVC_LOG_STIXEL, "unsupported CVFlow ProfID(%u)", pModeInfo->ProfID, 0U);
                Rval = SVC_NG;
            }
        } else {
            Rval = SVC_NG;
        }
    } else {
        Rval = SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_STIXEL, "CvFlow_StixelConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_STIXEL, "CvFlow_StixelConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver load function
*/
static UINT32 CvFlow_StixelLoad(UINT32 CvFlowChan, void *pInfo)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_STIXEL_CTRL_s *pCtrl;
    const SVC_CV_FLOW_LOAD_INFO_s *pLoadInfo;
    const SVC_CV_FLOW_STIXEL_MODE_INFO_s *pModeInfo;
    UINT32 i;

    (void) pInfo;

    Rval = Stixel_CvfChan2Inst(CvFlowChan, &Inst);
    if (Rval == SVC_OK) {
        pCtrl = &g_StixelCtrl[Inst];
        if (NULL != pInfo) {
            AmbaMisra_TypeCast(&pLoadInfo, &pInfo);
            if (pLoadInfo->NumBin > 0U) {
                /* Use external path */
                if (pLoadInfo->AlgoIdx < STIXEL_MAX_ALGO_NUM) {
                    Rval = SvcCvFlow_LoadAlgoBin(SVC_LOG_STIXEL, pLoadInfo, &pCtrl->AlgoOp[pLoadInfo->AlgoIdx]);
                } else {
                    SvcLog_NG(SVC_LOG_STIXEL, "CvFlow_StixelLoad: CvFlowChan(%d) invalid AlgoIdx(%u)", CvFlowChan, pLoadInfo->AlgoIdx);
                }
            } else {
                /* Use ModeInfo */
                if (pCtrl->pCvFlow->CvModeID < STIXEL_NUM_MODE) {
                    pModeInfo = &SvcCvFlow_Stixel_ModeInfo[pCtrl->pCvFlow->CvModeID];

                    for (i = 0; i < pModeInfo->AlgoGrp.NumAlgo; i++) {
                        Rval |= SvcCvFlow_LoadAlgo(SVC_LOG_STIXEL, &pModeInfo->AlgoGrp.AlgoInfo[i], &pCtrl->AlgoOp[i]);
                    }
                } else  {
                    SvcLog_NG(SVC_LOG_STIXEL, "CvFlow_StixelLoad: CvFlowChan(%d) invalid CvModeID(%u)", CvFlowChan, pCtrl->pCvFlow->CvModeID);
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STIXEL, "CvFlow_StixelLoad: invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STIXEL, "CvFlow_StixelLoad: CvFlowChan(%d) error", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver start function
*/
static UINT32 CvFlow_StixelStart(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst, i, j, AlgoIdx, Num;
    SVC_STIXEL_CTRL_s *pCtrl;
    const SVC_CV_FLOW_STIXEL_MODE_INFO_s *pModeInfo;
    SVC_CV_FLOW_BUF_INFO_s BufInfo;
    const SVC_CV_FLOW_ALGO_INFO_s *pAlgoInfo;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
    SVC_CV_FLOW_ALGO_QUERY_CFG_s QueryCfg;
    SVC_CV_FLOW_ALGO_CREATE_CFG_s CreateCfg;
    SVC_CV_ALGO_REGCB_CFG_s CbCfg;
    char Text[2];
    static char CbEvtQueName[] = "EvtQue##_Stixel";
    static char SemName[] = "OutBufSem";
    static STIXEL_OUTPUT_MSG_s AlgoOutQue[SVC_STIXEL_MAX_INSTANCE][SVC_STIXEL_ALGO_OUT_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_CV_ALGO_CALLBACK_f Stixel_AlgoOutCbList[SVC_STIXEL_MAX_INSTANCE] = {
        Stixel_AlgoOutCallback0,
        Stixel_AlgoOutCallback1,
    };

    (void) pInput;

    Rval = Stixel_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl     = &g_StixelCtrl[Inst];
        pModeInfo = &SvcCvFlow_Stixel_ModeInfo[pCtrl->pCvFlow->CvModeID];

        for (AlgoIdx = 0; AlgoIdx < pModeInfo->AlgoGrp.NumAlgo; AlgoIdx++) {
            pAlgoInfo = &pModeInfo->AlgoGrp.AlgoInfo[AlgoIdx];
            pAlgoOp   = &pCtrl->AlgoOp[AlgoIdx];

            Rval = AmbaWrap_memset(&QueryCfg, 0, sizeof(SVC_CV_FLOW_ALGO_QUERY_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STIXEL, "memset QueryCfg failed", 0U, 0U);
            }
            Rval = SvcCvFlow_QueryAlgo(SVC_LOG_STIXEL, pAlgoInfo, &QueryCfg, pAlgoOp);
            if (SVC_OK == Rval) {
                /* Create algo */
                Rval = AmbaWrap_memset(&CreateCfg, 0, sizeof(SVC_CV_FLOW_ALGO_CREATE_CFG_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STIXEL, "memset CreateCfg failed", 0U, 0U);
                }
                Rval = SvcCvFlow_CreateAlgo(SVC_LOG_STIXEL, pAlgoInfo, &CreateCfg, pAlgoOp);

                if (SVC_OK == Rval) {
                    /* Create Output buffer */
                    for (i = 0; i < SVC_STIXEL_OUTPUT_DEPTH; i++) {
                        Num = pAlgoOp->OutputNum;
                        for (j = 0; j < Num; j++) {
                            Rval |= SvcCvFlow_AllocWorkBuf(pAlgoOp->OutputSize[j], &BufInfo);
                            SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STIXEL, BufInfo.pAddr, BufInfo.Size, &pCtrl->AlgoOpEx[AlgoIdx].OutputBuf[i][j]);
                        }
                    }

                    if (SVC_OK == Rval) {
                        /* Register Algo callback */
                        Rval = AmbaWrap_memset(&CbCfg, 0, sizeof(SVC_CV_ALGO_REGCB_CFG_s));
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_STIXEL, "memset CbCfg failed", 0U, 0U);
                        }
                        CbCfg.Mode     = 0U;
                        CbCfg.Callback = Stixel_AlgoOutCbList[Inst];
                        Rval = SvcCvAlgo_RegCallback(&pAlgoOp->AlgoHandle, &CbCfg);
                        if (SVC_OK == Rval) {
                            Rval = Stixel_Config(Inst, AlgoIdx);
                            if (SVC_OK != Rval) {
                                SvcLog_NG(SVC_LOG_STIXEL, "FlexiDAG Config error(0x%X)", Rval, 0U);
                            }

                            pCtrl->NumValidAlgo++;
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_STIXEL, "Alloc Algo[%u] Output Buf failed", AlgoIdx, 0U);
                    }
                } else {
                   SvcLog_NG(SVC_LOG_STIXEL, "CreateAlgo[%u] error", AlgoIdx, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_STIXEL, "QueryAlgo[%u] error", AlgoIdx, 0U);
            }
        }

        if (SVC_OK == Rval) {
            Rval = Stixel_ConfigBuffer(Inst);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STIXEL, "ConfigBuffer error", 0U, 0U);
            }
        }

        if (SVC_OK == Rval) {
            pCtrl->pTxProc = Stixel_TxDataSrcHandler;

            if (1U != AmbaUtility_UInt32ToStr(&Text[0], sizeof(Text), Inst, 10)) {
                SvcLog_NG(SVC_LOG_STIXEL, "UInt32ToStr failed", 0U, 0U);
            }
            CbEvtQueName[6] = Text[0];

            Rval = AmbaKAL_MsgQueueCreate(&pCtrl->AlgoOutQueId,
                                           CbEvtQueName,
                                           sizeof(STIXEL_OUTPUT_MSG_s),
                                           &AlgoOutQue[Inst][0],
                                           SVC_STIXEL_ALGO_OUT_QUEUE_SIZE * sizeof(STIXEL_OUTPUT_MSG_s));

            Rval |= AmbaKAL_SemaphoreCreate(&pCtrl->OutBufSem, SemName, SVC_STIXEL_SEM_INIT_COUNT);

            if (KAL_ERR_NONE == Rval) {
                pCtrl->Started = 1U;
            } else {
                SvcLog_NG(SVC_LOG_STIXEL, "Create MsgQue/Semaphore error(0x%X)", Rval, 0U);
                Rval =  SVC_NG;
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STIXEL, "Invalid CvFlowChan", 0U, 0U);
        Rval =  SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_STIXEL, "CvFlow_StixelStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_STIXEL, "CvFlow_StixelStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver control function
*/
static UINT32 CvFlow_StixelControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo)
{
    UINT32 Rval, Inst;
    SVC_STIXEL_CTRL_s *pCtrl;
    const UINT32 *pValue;

    Stixel_MutexTake();

    Rval = Stixel_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl = &g_StixelCtrl[Inst];
        switch (CtrlType) {
            case SVC_CV_CTRL_ENABLE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->Enable = (UINT8) *pValue;
                break;
            case SVC_CV_CTRL_SET_SRC_RATE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->SrcRateCtrl = *pValue;
                SvcLog_OK(SVC_LOG_STIXEL, "Control[%d]: SrcRate = %u", Inst, *pValue);
                break;
            case SVC_CV_CTRL_SEND_DSI_INPUT:
                Rval = Stixel_SendDsiInput(Inst, pInfo);
                break;
            case SVC_CV_CTRL_GET_ROAD_ESTIMATE:
                Rval = Stixel_Get_RoadEstimate(Inst, pInfo);
                break;
            case SVC_CV_CTRL_SET_ROAD_ESTIMATE:
                Rval = Stixel_Set_RoadEstimate(Inst, pInfo);
                break;
            default:
                SvcLog_NG(SVC_LOG_STIXEL, "Control[%d]: Invalid CtrlType(%d)", Inst, CtrlType);
                break;
        }
    } else {
        SvcLog_NG(SVC_LOG_STIXEL, "Control: Invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    Stixel_MutexGive();

    return Rval;
}

/**
* CvFlow driver stop function
*/
static UINT32 CvFlow_StixelStop(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_STIXEL_CTRL_s *pCtrl;
    SVC_CV_FLOW_ALGO_DELETE_CFG_s DelCfg;
    UINT32 i;

    (void)pInput;

    Rval = Stixel_CvfChan2Inst(CvFlowChan, &Inst);
    pCtrl = &g_StixelCtrl[Inst];

    if (SVC_OK == Rval) {
        Rval = Stixel_SetTaskIdle(Inst);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_STIXEL, "SetTaskIdle error", 0U, 0U);
        }

        Rval = AmbaWrap_memset(&DelCfg, 0, sizeof(SVC_CV_FLOW_ALGO_DELETE_CFG_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STIXEL, "memset DelCfg failed", 0U, 0U);
        }

        for (i = 0; i < pCtrl->NumValidAlgo; i++) {
            Rval |= SvcCvFlow_DeleteAlgo(SVC_LOG_STIXEL, &DelCfg, &pCtrl->AlgoOp[i]);
        }
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STIXEL, "Delele Algo error", 0U, 0U);
        }

        Rval = AmbaKAL_MsgQueueDelete(&pCtrl->AlgoOutQueId);
        Rval |= AmbaKAL_SemaphoreDelete(&pCtrl->OutBufSem);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_STIXEL, "Delete MsgQue/Semaphore error(0x%X)", Rval, 0U);
        }

        /* Reset Ctrl Intance */
        Rval = AmbaWrap_memset(&g_StixelCtrl[Inst], 0, sizeof(SVC_STIXEL_CTRL_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STIXEL, "memset g_StixelCtrl[%u] failed", Inst, 0U);
        }
    }

    SvcLog_OK(SVC_LOG_STIXEL, "CvFlow_StixelStop: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver debug interface
*/
static UINT32 CvFlow_StixelDebug(UINT32 CvFlowChan, const char *pCmd, UINT32 Param0, UINT32 Param1, UINT32 Param2)
{
    UINT32 i;
    ULONG Addr;
    const SVC_STIXEL_CTRL_s *pCtrl;
    const AMBA_CV_FLEXIDAG_HANDLE_s *pCvFDHandle;

    if (0 == SvcWrap_strcmp("debug", pCmd)) {
        SvcCvDebugEnable = (UINT8) Param0;
        SvcLog_DBG(SVC_LOG_STIXEL, "Set debug(%u)", Param0, 0U);
    } else if (0 == SvcWrap_strcmp("info", pCmd)) {
        for (i = 0; i < SVC_STIXEL_MAX_INSTANCE; i++) {
            pCtrl = &g_StixelCtrl[i];
            if (pCtrl->Used == 1U) {
                AmbaPrint_PrintUInt5("Stixel[%d]: CvfChan %d", i, pCtrl->CvfChan, 0U, 0U, 0U);
                pCvFDHandle = &pCtrl->AlgoOp[0].AlgoHandle.FDs[0].Handle;
                AmbaMisra_TypeCast(&Addr, &pCvFDHandle);
                SvcCvFlow_PrintULong("  CV_FLEXIDAG_HANDLE: 0x%x", Addr, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        SvcLog_DBG(SVC_LOG_STIXEL, "Unsupported cmd", 0U, 0U);
    }

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(&Param0);
    AmbaMisra_TouchUnused(&Param1);
    AmbaMisra_TouchUnused(&Param2);

    return SVC_OK;
}

SVC_CV_FLOW_OBJ_s SvcCvFlow_StixelObj = {
    .Init         = CvFlow_StixelInit,
    .Config       = CvFlow_StixelConfig,
    .Load         = CvFlow_StixelLoad,
    .Start        = CvFlow_StixelStart,
    .Control      = CvFlow_StixelControl,
    .Stop         = CvFlow_StixelStop,
    .Debug        = CvFlow_StixelDebug,
};

