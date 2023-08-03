/**
 *  @file SvcCvFlow_RefSeg.c
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
 *  @details Implementation of FlexiDAG based Ambarella Segmentation
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
#include "SvcCvFlow_Shared.h"
#include "SvcCvFlow_RefSeg.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcImgFIFO.h"
#include "SvcImgFeeder.h"
#include "SvcOsd.h"
#include "SvcWrap.h"

#include "cvapi_amba_mask_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_ref_util.h"
#include "cvapi_svccvalgo_openseg.h"
#include "cvapi_svccvalgo_ambaod.h"

#ifdef CONFIG_AMBALINK_BOOT_OS
#include "cvapi_svccvalgo_linux.h"
#include "AmbaIPC_FlexidagIO.h"
#include "SvcCvFlexidagIO.h"
#endif

#define SVC_LOG_REFSEG        "RefSeg"

#define SVC_REFSEG_INSTANCE0                (0U)
#define SVC_REFSEG_INSTANCE1                (1U)
#define SVC_REFSEG_INSTANCE2                (2U)
#define SVC_REFSEG_INSTANCE3                (3U)
#define SVC_REFSEG_MAX_INSTANCE             (4U)

#define SVC_REFSEG_SEM_INIT_COUNT           (1U)
#define SVC_REFSEG_OUTPUT_DEPTH             (SVC_REFSEG_SEM_INIT_COUNT + 2U)

#define SVC_REFSEG_RX_TASK_STACK_SIZE       (0x4000)
#define SVC_REFSEG_PICINFO_QUEUE_SIZE       (8U)
#define SVC_REFSEG_ALGO_OUT_QUEUE_SIZE      (SVC_REFSEG_OUTPUT_DEPTH)

#define SRC_RATE_FRACTION_MODE_BIT          (0x10000000U)


typedef UINT32 (*SVC_REFSEG_DATA_HANDLER_f)(UINT32 Inst);
typedef UINT32 (*SVC_REFSEG_TX_HANDLER_f)(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

typedef struct {
    flexidag_memblk_t           OutputBuf[SVC_REFSEG_OUTPUT_DEPTH][FLEXIDAG_MAX_OUTPUTS];
    AMBA_CV_FLEXIDAG_IO_s       InBuf[SVC_REFSEG_OUTPUT_DEPTH];
    AMBA_CV_FLEXIDAG_IO_s       OutBuf[SVC_REFSEG_OUTPUT_DEPTH];
} SVC_CV_FLOW_REFSEG_OP_EX_INFO_s;

typedef struct {
    UINT8                       Used;
    UINT8                       CvfChan;
    UINT8                       Enable;
    UINT8                       Started;
    SVC_CV_FLOW_CFG_s           *pCvFlow;
    SVC_CV_FLOW_CFG_s           CvCfg;

    UINT8                       NumValidAlgo;
    SVC_CV_FLOW_ALGO_OP_INFO_s  AlgoOp[REFSEG_MAX_ALGO_NUM];
    SVC_CV_FLOW_REFSEG_OP_EX_INFO_s AlgoOpEx[REFSEG_MAX_ALGO_NUM];
    void                       *pImgInfoBuf[SVC_REFSEG_PICINFO_QUEUE_SIZE];
    UINT32                      ImgInfoBufSize;

    /* Data source service */
    SVC_IMG_FEEDER_PORT_s       FeederPort;
    UINT32                      SrcSeqNum;
    SVC_REFSEG_TX_HANDLER_f     pTxProc;
    UINT32                      TxSeqNum;
    UINT32                      PicInfoIdx;
    /* Driver intermediate data buffer */
    void                        *pInterDataBuf[SVC_REFSEG_OUTPUT_DEPTH];

    /* Data sink service */
    SVC_TASK_CTRL_s             RxTaskCtrl;
    SVC_REFSEG_DATA_HANDLER_f   pRxProc;
    UINT32                      RxSeqNum;
    UINT32                      OutBufIdx;
    AMBA_KAL_SEMAPHORE_t        OutBufSem;
    UINT32                      SinkInfoIdx;

    /* Algo Callback */
    AMBA_KAL_MSG_QUEUE_t        AlgoOutQueId;

    /* Misc */
    UINT32                      SrcRateCtrl;
#define                        REFSEG_SRC_RATE_MAX_DENOMINATOR  (16U)
    UINT8                       SrcValidTbl[REFSEG_SRC_RATE_MAX_DENOMINATOR];
    UINT8                       SrcRateNum;
    UINT8                       SrcRateDen;
    SVC_CV_FRAME_TIME_CALC_s    FTInfo;
    UINT8                       SrcInfoErr;
    UINT8                       SceneMode;
    UINT32                      FlexiIOChan;    /* for AmbaLink CV only */
} SVC_REFSEG_CTRL_s;

typedef struct {
    memio_source_recv_picinfo_t *pPicInfo;      /* input pic info */
    UINT64 TimeStamp[SVC_CV_FLOW_MAX_TIME_STAMP];
} REFSEG_INTER_DATA_s;

typedef struct {
   UINT32 Event;
   SVC_CV_ALGO_OUTPUT_s AlgoOut;
} REFSEG_OUTPUT_MSG_s;

static SVC_REFSEG_CTRL_s g_RefSegCtrl[SVC_REFSEG_MAX_INSTANCE] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MUTEX_t SegMutex;
static UINT8 SvcCvDebugEnable = 0;

static UINT32 CvFlow_RefSegInit(UINT32 CvFlowChan, void *pInit);
static UINT32 CvFlow_RefSegConfig(UINT32 CvFlowChan, void *pConfig);
static UINT32 CvFlow_RefSegStart(UINT32 CvFlowChan, void *pInput);
static UINT32 CvFlow_RefSegControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo);
static UINT32 CvFlow_RefSegStop(UINT32 CvFlowChan, void *pInput);
static UINT32 RefSeg_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst);

static UINT32 RefSeg_TxDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 RefSeg_TxDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 RefSeg_TxDataSrcHandler2(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 RefSeg_TxDataSrcHandler3(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

static UINT32 RefSeg_AlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 RefSeg_AlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 RefSeg_AlgoOutCallback2(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 RefSeg_AlgoOutCallback3(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);

static UINT32 RefSeg_ConfigRoi(UINT32 Inst, UINT32 AlgoIdx, const SVC_CV_ROI_INFO_s *pRoiCfg);
static UINT32 RefSeg_ConfigLicense(UINT32 Inst, UINT32 AlgoIdx);

static void SVC_CV_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcCvDebugEnable > 0U) {
        SvcLog_DBG(pModule, pFormat, Arg1, Arg2);
    }
}

static void RefSeg_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&SegMutex, 5000)) {
        SvcLog_NG(SVC_LOG_REFSEG, "RefSeg_MutexTake: timeout", 0U, 0U);
    }
}

static void RefSeg_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&SegMutex)) {
        SvcLog_NG(SVC_LOG_REFSEG, "RefSeg_MutexGive: error", 0U, 0U);
    }
}

static UINT32 RefSeg_SemTake(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreTake(&g_RefSegCtrl[Inst].OutBufSem, 10000)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_REFSEG, "RefSeg_SemTake[%d]: timeout", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 RefSeg_SemGive(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreGive(&g_RefSegCtrl[Inst].OutBufSem)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_REFSEG, "RefSeg_SemGive[%d]: error", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 RefSeg_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst)
{
    UINT32 Rval = SVC_NG;
    UINT32 i;

    if (NULL != pInst) {
        for (i = 0; i < SVC_REFSEG_MAX_INSTANCE; i++) {
            if ((1U == g_RefSegCtrl[i].Used) && (CvfChan == g_RefSegCtrl[i].CvfChan)) {
                *pInst = i;
                Rval = SVC_OK;
                break;
            }
        }
    }
    return Rval;
}
#ifdef CONFIG_AMBALINK_BOOT_OS
static UINT32 RefSeg_Init_Callback(UINT32 Channel, UINT32 OutType)
{
    UINT32 Rval = SVC_OK;

    AmbaPrint_PrintUInt5("RefSeg_Init_Callback(%u)", Channel, 0U, 0U, 0U, 0U);

    for (UINT32 i = 0; i < SVC_REFSEG_MAX_INSTANCE; i++) {
        if (g_RefSegCtrl[i].Used == 1U) {
            if (Channel == g_RefSegCtrl[i].FlexiIOChan) {
                Rval = SvcCvFlow_StatusEventIssue(g_RefSegCtrl[i].CvfChan, SVC_CV_EVT_IPC_CHAN_READY, NULL);
            }
        }
    }

    AmbaMisra_TouchUnused(&OutType);

    return Rval;
}
#endif

static void RefSeg_DumpPicInfo(const cv_pic_info_t *pPicInfo)
{
    static UINT32 DumpTimes = 0U;
    extern void SvcCvFlow_DumpPicInfo(const char *pModuleName, const cv_pic_info_t *pPicInfo);

    if (DumpTimes > 0U) {
        SvcCvFlow_DumpPicInfo(SVC_LOG_REFSEG, pPicInfo);
        DumpTimes--;
    }
}

/**
* Pack data source to pic info structure
*/
static void RefSeg_PackPicInfo(UINT32 Inst, const SVC_CV_INPUT_IMG_INFO_s *pInputImg, memio_source_recv_picinfo_t *pInfo)
{
    UINT32 Rval = SVC_OK;
    cv_pic_info_t *pPicInfo = &pInfo->pic_info;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    const SVC_REFSEG_CTRL_s *pCtrl = &g_RefSegCtrl[Inst];
    UINT32 DataSrc, RelativeAddrOn = 1U;
    extern UINT32 SvcCvFlow_PackPicInfo_Pyramid(cv_pic_info_t *pPicInfo, const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo, UINT32 MainWidth, UINT32 MainHeight, UINT32 UseRelative, UINT32 LeftRight);
    extern UINT32 SvcCvFlow_PackPicInfo_MainYuv(cv_pic_info_t *pPicInfo, const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo, UINT32 UseRelative, UINT32 LeftRight);

    Rval = AmbaWrap_memset(pInfo, 0, sizeof(memio_source_recv_picinfo_t));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_REFSEG, "memset picinfo failed", 0U, 0U);
    }

    DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
    if (pInputImg->NumInfo > 0U) {
        if ((DataSrc == SVC_CV_DATA_SRC_MAIN_YUV) ||
            (DataSrc == SVC_CV_DATA_SRC_YUV_STRM)) {
            const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo;
            AmbaMisra_TypeCast(&pYuvInfo, &pInputImg->Info[0].pBase);

            Rval = SvcCvFlow_PackPicInfo_MainYuv(pPicInfo, pYuvInfo, RelativeAddrOn, 0U);
        } else {
            const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo;
            AmbaMisra_TypeCast(&pYuvInfo, &pInputImg->Info[0].pBase);

            if (SvcCvFlow_RefSeg_ModeInfo[pCtrl->pCvFlow->CvModeID].ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
                RelativeAddrOn = 0U;
            }
            Rval = SvcCvFlow_PackPicInfo_Pyramid(pPicInfo,
                                                 pYuvInfo,
                                                 pCfg->FovCfg[pYuvInfo->ViewZoneId].MainWin.Width,
                                                 pCfg->FovCfg[pYuvInfo->ViewZoneId].MainWin.Height,
                                                 RelativeAddrOn, 0U);
        }
    } else {
        Rval = SVC_NG;
    }

    RefSeg_DumpPicInfo(pPicInfo);

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_REFSEG, "RefSeg_PackPicInfo error", 0U, 0U);
    }
}

/**
* Send message to Feeder to update port status
*/
static void RefSeg_SendReceiverReady(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort)
{
    SVC_REFSEG_CTRL_s *pCtrl = &g_RefSegCtrl[Inst];
    AMBA_KAL_SEMAPHORE_INFO_s SemInfo;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreQuery(&pCtrl->OutBufSem, &SemInfo)) {
        if (SemInfo.CurrentCount > 0U) {
            if (SVC_OK != SvcImgFeeder_MsgToFeeder(pPort, SVC_IMG_FEEDER_RECEIVER_READY, NULL)) {
                SvcLog_NG(SVC_LOG_REFSEG, "Send ReceiverReady[%u] error", Inst, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_REFSEG, "ReceiverReady[%u]: SemQuery error", Inst, 0U);
    }
}

/**
* TX data source handler: process input data
*/
static UINT32 RefSeg_TxDataSrcHandler(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_REFSEG_CTRL_s *pCtrl = &g_RefSegCtrl[Inst];
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
    const SVC_CV_INPUT_IMG_INFO_s *pInputImg;
    memio_source_recv_picinfo_t *pPicInfo;
    UINT32 i, j;
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;
    AMBA_CV_FLEXIDAG_IO_s *pInBuf, *pOutBuf;
    REFSEG_INTER_DATA_s *pInterData;
    const SVC_CV_FLOW_REFSEG_MODE_INFO_s *pModeInfo;
#ifdef CONFIG_AMBALINK_BOOT_OS
    SVC_CV_ALGO_LINUX_EX_CFG_s ExCfg;
#endif

    AmbaMisra_TouchUnused(pInfo);
    pModeInfo = &SvcCvFlow_RefSeg_ModeInfo[pCtrl->pCvFlow->CvModeID];

    SVC_CV_DBG(SVC_LOG_REFSEG, "Tx[%u]: run %d", Inst, pCtrl->TxSeqNum);

    if (Event == SVC_IMG_FEEDER_IMAGE_DATA_READY) {
        if ((pCtrl->Enable == 1U) &&
            (0U == (pCtrl->SrcSeqNum % pCtrl->SrcRateCtrl))) {
            Rval = RefSeg_SemTake(Inst);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pInputImg, &pInfo);
                AmbaMisra_TypeCast(&pPicInfo, &pCtrl->pImgInfoBuf[pCtrl->PicInfoIdx]);
                RefSeg_PackPicInfo(Inst, pInputImg, pPicInfo);

                /* Prepare feed config */
                Rval = AmbaWrap_memset(&FeedCfg, 0, sizeof(SVC_CV_ALGO_FEED_CFG_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_REFSEG, "memset FeedCfg failed", 0U, 0U);
                }

                for (i = 0; i < pCtrl->NumValidAlgo; i++) {
                    pAlgoOp = &pCtrl->AlgoOp[i];
                    pInBuf = &pCtrl->AlgoOpEx[i].InBuf[pCtrl->OutBufIdx];
                    pOutBuf = &pCtrl->AlgoOpEx[i].OutBuf[pCtrl->OutBufIdx];

                    FeedCfg.pIn  = pInBuf;
                    FeedCfg.pOut = pOutBuf;
                    pInBuf->num_of_buf = 1U;
                    SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_REFSEG, pPicInfo, pCtrl->ImgInfoBufSize, &pInBuf->buf[0]);
                    pOutBuf->num_of_buf = pAlgoOp->OutputNum;
                    for (j = 0U; j < pAlgoOp->OutputNum; j++) {
                        Rval = AmbaWrap_memcpy(&pOutBuf->buf[j], &pCtrl->AlgoOpEx[i].OutputBuf[pCtrl->OutBufIdx][j], sizeof(flexidag_memblk_t));
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_REFSEG, "memcpy OutputBufBlk failed", 0U, 0U);
                        }
                    }

                    if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
#ifdef CONFIG_AMBALINK_BOOT_OS
                        const SVC_CV_ALGO_LINUX_EX_CFG_s *pExCfg = &ExCfg;

                        ExCfg.AmbaIPCChannel = pCtrl->FlexiIOChan;
                        ExCfg.ValidDataSize  = sizeof(memio_source_recv_picinfo_t);
                        AmbaMisra_TypeCast(&FeedCfg.pExtFeedCfg, &pExCfg);
#else
                        SvcLog_NG(SVC_LOG_REFSEG, "CONFIG_AMBALINK_BOOT_OS not defined ", 0U, 0U);
#endif
                    } else {
                        AmbaMisra_TypeCast(&FeedCfg.pExtFeedCfg, &pPicInfo); /* SCA_OpenOD */
                    }

                    /* Prepare inter data and pass pInterData through user data */
                    AmbaMisra_TypeCast(&pInterData, &(pCtrl->pInterDataBuf[pCtrl->OutBufIdx]));
                    pInterData->pPicInfo = pPicInfo;
                    AmbaMisra_TypeCast(&FeedCfg.pUserData, &pInterData);

                    SvcCvFlow_GetTimeStamp(SVC_LOG_REFSEG, &pInterData->TimeStamp[SVC_CV_FLOW_TIME_STAMP0]);

                    Rval |= SvcCvAlgo_Feed(&pAlgoOp->AlgoHandle, &FeedCfg);
                    SVC_CV_DBG(SVC_LOG_REFSEG, "Tx[%u]: Send(%u)", Inst, pPicInfo->pic_info.frame_num);
                }

                if (SVC_OK == Rval) {
                    pCtrl->TxSeqNum++;
                    pCtrl->PicInfoIdx++;
                    if (pCtrl->PicInfoIdx >= SVC_REFSEG_PICINFO_QUEUE_SIZE) {
                        pCtrl->PicInfoIdx = 0;
                    }
                    pCtrl->OutBufIdx++;
                    if (pCtrl->OutBufIdx >= SVC_REFSEG_OUTPUT_DEPTH) {
                        pCtrl->OutBufIdx = 0;
                    }
                } else {
                    SvcLog_NG(SVC_LOG_REFSEG, "Tx[%u]: send error", Inst, 0U);
                }

                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    RefSeg_SendReceiverReady(Inst, pPort);
                }
            } else {
                SvcLog_NG(SVC_LOG_REFSEG, "Tx[%u]: SemTake error", Inst, 0U);
            }
        } else {
            if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                RefSeg_SendReceiverReady(Inst, pPort);
                Rval = AmbaKAL_TaskSleep(1);
            }
        }

        pCtrl->SrcSeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_REFSEG, "Tx[%u]: unsupported event(0x%x)", Inst, Event);
        Rval = SVC_NG;
    }

    SVC_CV_DBG(SVC_LOG_REFSEG, "Tx[%u]: end", Inst, 0U);

    return Rval;

}

static UINT32 RefSeg_TxDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_REFSEG_CTRL_s *pCtrl = &g_RefSegCtrl[SVC_REFSEG_INSTANCE0];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_REFSEG_INSTANCE0, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 RefSeg_TxDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_REFSEG_CTRL_s *pCtrl = &g_RefSegCtrl[SVC_REFSEG_INSTANCE1];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_REFSEG_INSTANCE1, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 RefSeg_TxDataSrcHandler2(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_REFSEG_CTRL_s *pCtrl = &g_RefSegCtrl[SVC_REFSEG_INSTANCE2];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_REFSEG_INSTANCE2, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 RefSeg_TxDataSrcHandler3(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_REFSEG_CTRL_s *pCtrl = &g_RefSegCtrl[SVC_REFSEG_INSTANCE3];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_REFSEG_INSTANCE3, pPort, Event, pInfo);
    }
    return Rval;
}
static void RefSeg_DumpResult(const amba_mask_out_t *pSegResult)
{
    static UINT32 dumptimes = 0;
    ULONG BufAddr;

    if (dumptimes > 0U) {
        AmbaMisra_TypeCast(&BufAddr, &pSegResult->buff_addr);
        AmbaPrint_PrintUInt5("[RefSeg_DumpResult] ======================", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("type %d, len %d, w %d h %d",
            pSegResult->type, pSegResult->length, pSegResult->buff_width, pSegResult->buff_height, 0U);
        SvcCvFlow_PrintULong("bufaddr 0x%x", BufAddr, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("col %d row %d w %d h %d",
            pSegResult->mask_start_col, pSegResult->mask_start_row, pSegResult->mask_width, pSegResult->mask_height, 0U);
        dumptimes--;
    }
}
/**
* Algorithm output event callback function
*/
static UINT32 RefSeg_AlgoOutCallback(UINT32 Inst, UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Rval = SVC_OK;
    SVC_REFSEG_CTRL_s *pCtrl = &g_RefSegCtrl[Inst];
    REFSEG_OUTPUT_MSG_s Msg;
    REFSEG_INTER_DATA_s *pInterData = NULL;
    UINT32 Valid = 1U;

    if (Event == CALLBACK_EVENT_OUTPUT) {
        if (pEventData->pUserData != NULL) {
            AmbaMisra_TypeCast(&pInterData, &(pEventData->pUserData));
        }
        if (pInterData != NULL) {
            SvcCvFlow_GetTimeStamp(SVC_LOG_REFSEG, &pInterData->TimeStamp[SVC_CV_FLOW_TIME_STAMP1]);
            SvcCvFlow_CalcFrameTime(SVC_LOG_REFSEG, 2U, pInterData->TimeStamp, &pCtrl->FTInfo);
        }
    } else {
        Valid = 0U;
    }

    if (Valid == 1U) {
        Msg.Event = Event;
        if (NULL != pEventData) {
            Rval = AmbaWrap_memcpy(&Msg.AlgoOut, pEventData, sizeof(SVC_CV_ALGO_OUTPUT_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REFSEG, "memcpy AlgoOut failed", 0U, 0U);
            }
        }
        Rval = AmbaKAL_MsgQueueSend(&pCtrl->AlgoOutQueId, &Msg, 5000);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_REFSEG, "RefSeg_AlgoOutCallback[%u]: MsgQueueSend error", Inst, 0U);
        }
    }

    return Rval;
}

static UINT32 RefSeg_AlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return RefSeg_AlgoOutCallback(SVC_REFSEG_INSTANCE0, Event, pEventData);
}

static UINT32 RefSeg_AlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return RefSeg_AlgoOutCallback(SVC_REFSEG_INSTANCE1, Event, pEventData);
}

static UINT32 RefSeg_AlgoOutCallback2(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return RefSeg_AlgoOutCallback(SVC_REFSEG_INSTANCE2, Event, pEventData);
}

static UINT32 RefSeg_AlgoOutCallback3(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return RefSeg_AlgoOutCallback(SVC_REFSEG_INSTANCE3, Event, pEventData);
}

/**
* RX data sink handler: process detection output
*/
static UINT32 RefSeg_RxDataSinkHandler(UINT32 Inst)
{
    UINT32 Rval;
    SVC_REFSEG_CTRL_s *pCtrl = &g_RefSegCtrl[Inst];
    REFSEG_OUTPUT_MSG_s Msg;
    const REFSEG_INTER_DATA_s *pInterData = NULL;

    const SVC_CV_ALGO_OUTPUT_s *pAlgoOut = &Msg.AlgoOut;
    const amba_mask_out_t *pMaskOut = NULL;
    SVC_CV_DETRES_SEG_s SegResult;
    const SVC_CV_FLOW_REFSEG_MODE_INFO_s *pModeInfo = &SvcCvFlow_RefSeg_ModeInfo[pCtrl->pCvFlow->CvModeID];
    UINT8 OutputType = pModeInfo->OutputType;
    UINT8 OutputBufIdx = pModeInfo->OutputBufIdx;

    SVC_CV_DBG(SVC_LOG_REFSEG, "Rx[%u]: run %d", Inst, pCtrl->RxSeqNum);

    Rval = AmbaKAL_MsgQueueReceive(&pCtrl->AlgoOutQueId, &Msg, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK == Rval) {
        SVC_CV_DBG(SVC_LOG_REFSEG, "Rx[%u]: recv out(%u)", Inst, 0);

        if (Msg.Event == CALLBACK_EVENT_OUTPUT) {
            if (pCtrl->Enable == 1U) {
                if (OutputType == REFSEG_OUTPUT_TYPE_MASK_OUT){
                    AmbaMisra_TypeCast(&pMaskOut, &pAlgoOut->pOutput->buf[OutputBufIdx].pBuffer);
                    RefSeg_DumpResult(pMaskOut);

                    if (SvcCvFlow_RefSeg_ModeInfo[pCtrl->pCvFlow->CvModeID].ProfID != SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
                        if (Msg.AlgoOut.pUserData != NULL) {
                            AmbaMisra_TypeCast(&pInterData, &(Msg.AlgoOut.pUserData));
                        }
                    }

                    SegResult.MsgCode     = SVC_CV_DETRES_SEG;
                    SegResult.Source      = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
                    SegResult.Width       = pMaskOut->mask_width;
                    SegResult.Height      = pMaskOut->mask_height;
                    SegResult.Pitch       = pMaskOut->mask_width;
                    if (NULL != pInterData) {
                        SegResult.FrameNum    = pInterData->pPicInfo->pic_info.frame_num;
                        SegResult.CaptureTime = pInterData->pPicInfo->pic_info.capture_time;
                    } else {
                        SegResult.FrameNum    = 0U;
                        SegResult.CaptureTime = 0U;
                    }

                    AmbaMisra_TypeCast(&(SegResult.pCvOut), &pMaskOut);
                    AmbaMisra_TypeCast(&(SegResult.pBuf), &pMaskOut->buff_addr);
                } else if (OutputType == REFSEG_OUTPUT_TYPE_ALGO_LINUX){
                    SegResult.MsgCode     = SVC_CV_DETRES_SEG;
                    SegResult.Source      = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
                    SegResult.Width       = pModeInfo->Cfg[0].pMaskCfg->BufWidth;
                    SegResult.Height      = pModeInfo->Cfg[0].pMaskCfg->BufHeight;
                    SegResult.Pitch       = pModeInfo->Cfg[0].pMaskCfg->BufWidth;
                    SegResult.FrameNum    = 0U;//pInPicInfo->pic_info.frame_num;
                    SegResult.CaptureTime = 0U;//pInPicInfo->pic_info.capture_time;
                    AmbaMisra_TypeCast(&(SegResult.pBuf), &pAlgoOut->pOutput->buf[OutputBufIdx].pBuffer);
                } else {
                    SvcLog_NG(SVC_LOG_REFSEG, "Rx[%u]: Unsupport output type", Inst, 0U);
                }

                Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                       SVC_CV_FLOW_OUTPUT_PORT0,
                                       pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                       &SegResult);

                if (SVC_OK == Rval) {
                    pCtrl->SinkInfoIdx++;
                    if (pCtrl->SinkInfoIdx >= SVC_REFSEG_OUTPUT_DEPTH) {
                        pCtrl->SinkInfoIdx = 0;
                    }
                }
            }

            Rval = RefSeg_SemGive(Inst);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REFSEG, "Rx[%u]: SemGive error", Inst, 0U);
            }

            if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                RefSeg_SendReceiverReady(Inst, &pCtrl->FeederPort);
            }
        }

        pCtrl->RxSeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_REFSEG, "Rx[%u]: recv error", Inst, 0U);
    }

    SVC_CV_DBG(SVC_LOG_REFSEG, "Rx[%u]: end", Inst, 0U);

    return SVC_OK;
}

/**
* Task entry of RX process
*/
static void* RefSeg_RxTaskEntry(void* EntryArg)
{
    const ULONG  *pArg;
    UINT32 Rval = SVC_OK, Inst;
    const SVC_REFSEG_CTRL_s *pCtrl;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Inst = (UINT32)(*pArg);
    pCtrl = &g_RefSegCtrl[Inst];

    SvcLog_DBG(SVC_LOG_REFSEG, "RxTaskEntry[%d] start", Inst, 0U);

    while (SVC_OK == Rval) {
        if (NULL != pCtrl->pRxProc) {
            Rval = pCtrl->pRxProc(Inst);
        } else {
            Rval = SVC_NG;
        }
    }

    SvcLog_DBG(SVC_LOG_REFSEG, "RxTaskEntry[%d] exit", Inst, 0U);

    return NULL;
}

/**
* Set and wait TX and RX task to idle state
*/
static UINT32 RefSeg_SetTaskIdle(UINT32 Inst)
{
    UINT32 Rval;
    SVC_REFSEG_CTRL_s *pCtrl = &g_RefSegCtrl[Inst];
    UINT32 i;

    /* Disable input and output */
    pCtrl->Enable = 0U;

    if (SVC_OK != SvcImgFeeder_Disconnect(&pCtrl->FeederPort)) {
        SvcLog_NG(SVC_LOG_REFSEG, "Disconn feeder port error", 0U, 0U);
    }

    if (SVC_OK != SvcImgFeeder_ClosePort(&pCtrl->FeederPort)) {
        SvcLog_NG(SVC_LOG_REFSEG, "Close feeder port error", 0U, 0U);
    }

    /* Wait Rx idle */
    for (i = 0; i < SVC_REFSEG_SEM_INIT_COUNT; i++) {
        Rval = RefSeg_SemTake(Inst);
    }

    return Rval;
}

/**
* Send external YUV to driver
*/
static UINT32 RefSeg_SendExtYUV(UINT32 Inst, const void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_REFSEG_CTRL_s *pCtrl = &g_RefSegCtrl[Inst];
    void *pInputImg; /* SVC_CV_INPUT_IMG_INFO_s */

    if (NULL != pCtrl->pTxProc) {
        AmbaMisra_TypeCast(&pInputImg, &pInfo);
        Rval = pCtrl->pTxProc(Inst, &pCtrl->FeederPort, SVC_IMG_FEEDER_IMAGE_DATA_READY, pInputImg);
    }

    return Rval;
}

/**
* Entry of configuration function
*/
static UINT32 RefSeg_Config(UINT32 Inst, UINT32 AlgoIdx)
{
    UINT32 Rval = SVC_OK;
    const SVC_REFSEG_CTRL_s *pCtrl = &g_RefSegCtrl[Inst];
    const SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &pCtrl->AlgoOp[AlgoIdx];
    const SVC_CV_ROI_INFO_s *pRoiCfg = &pCtrl->pCvFlow->InputCfg.Input[AlgoIdx].RoiInfo;

    if (pRoiCfg->NumRoi != 0U) {
        Rval = RefSeg_ConfigRoi(Inst, AlgoIdx, pRoiCfg);
    }

    if (pAlgoOp->LcsBinBuf.pAddr != NULL) {
        Rval |= RefSeg_ConfigLicense(Inst, AlgoIdx);
    }

    return Rval;
}

static UINT32 RefSeg_ConfigRoi(UINT32 Inst, UINT32 AlgoIdx, const SVC_CV_ROI_INFO_s *pRoiCfg)
{
    UINT32 Rval;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &g_RefSegCtrl[Inst].AlgoOp[AlgoIdx];
    amba_roi_config_t RoiMsg = {0};
    amba_roi_config_t *pRoiMsg = &RoiMsg;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;

    if (pRoiCfg != NULL) {
        pRoiMsg->msg_type            = AMBA_ROI_CONFIG_MSG;
        pRoiMsg->image_pyramid_index = pRoiCfg->Roi[0].Index;
        pRoiMsg->source_vin          = 0;
        pRoiMsg->roi_start_col       = pRoiCfg->Roi[0].StartX;
        pRoiMsg->roi_start_row       = pRoiCfg->Roi[0].StartY;
        pRoiMsg->roi_width           = pRoiCfg->Roi[0].Width;
        pRoiMsg->roi_height          = pRoiCfg->Roi[0].Height;

        MsgCfg.CtrlType = 0;
        MsgCfg.pExtCtrlCfg = NULL;
        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pRoiMsg);
        Rval = SvcCvAlgo_Control(&pAlgoOp->AlgoHandle, &MsgCfg);

        if (Rval == SVC_OK) {
            SvcLog_OK(SVC_LOG_REFSEG, "RefSeg_ConfigMask", 0U, 0U);
        } else {
            SvcLog_NG(SVC_LOG_REFSEG, "RefSeg_ConfigMask", 0U, 0U);
        }
    } else {
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 RefSeg_ConfigLicense(UINT32 Inst, UINT32 AlgoIdx)
{
#if 0//defined AMBANET_MSG_TYPE_LICENSE
    #define LICENSE_DATA_SIZE   (1020U)
    typedef struct {
        UINT32  Msg;
        UINT8   Payload[LICENSE_DATA_SIZE];
    } SVC_CV_FLOW_REFSEG_LCS_CFG_s;
    UINT32 Rval;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &g_RefSegCtrl[Inst].AlgoOp[AlgoIdx];
    SVC_CV_FLOW_REFSEG_LCS_CFG_s LcsCfg;
    const SVC_CV_FLOW_REFSEG_LCS_CFG_s *pLcsCfg = &LcsCfg;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;

    LcsCfg.Msg = AMBANET_MSG_TYPE_LICENSE;
    Rval = AmbaWrap_memcpy(LcsCfg.Payload, pAlgoOp->LcsBinBuf.pAddr, LICENSE_DATA_SIZE);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_REFSEG, "memcpy LcsBinBuf failed", 0U, 0U);
    }

    MsgCfg.CtrlType = 0;
    MsgCfg.pExtCtrlCfg = NULL;
    AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pLcsCfg);
    Rval = SvcCvAlgo_Control(&pAlgoOp->AlgoHandle, &MsgCfg);

    if (Rval == SVC_OK) {
        SvcLog_OK(SVC_LOG_REFSEG, "RefSeg_ConfigLicense[%d]", Inst, 0U);
    } else {
        SvcLog_NG(SVC_LOG_REFSEG, "RefSeg_ConfigLicense[%d] error(%d)", Inst, Rval);
    }

    return Rval;
#else
    AmbaMisra_TouchUnused(&Inst);
    AmbaMisra_TouchUnused(&AlgoIdx);
    return SVC_OK;
#endif
}

static UINT32 RefSeg_ConfigBuffer(UINT32 Inst)
{
    UINT32 Rval = SVC_OK;
    SVC_REFSEG_CTRL_s *pCtrl = &g_RefSegCtrl[Inst];
    SVC_CV_FLOW_BUF_INFO_s BufInfo = {0};
    UINT32 i;

    /* Img info buffer */
    if (pCtrl->pImgInfoBuf[0] == NULL) {
        for (i = 0; i < SVC_REFSEG_PICINFO_QUEUE_SIZE; i++) {
            Rval = SvcCvFlow_AllocWorkBuf((UINT32)sizeof(memio_source_recv_picinfo_t), &BufInfo);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pCtrl->pImgInfoBuf[i], &BufInfo.pAddr);
                pCtrl->ImgInfoBufSize = BufInfo.Size;
            } else {
                SvcLog_NG(SVC_LOG_REFSEG, "Alloc Img info buf failed", 0U, 0U);
                Rval = SVC_NG;
                break;
            }
        }
    }

    /* Inter data buffer */
    if (pCtrl->pInterDataBuf[0] == NULL) {
        for (i = 0; i < SVC_REFSEG_OUTPUT_DEPTH; i++) {
            Rval = SvcCvFlow_AllocWorkBuf((UINT32)sizeof(REFSEG_INTER_DATA_s), &BufInfo);
            if (SVC_OK == Rval) {
                Rval = AmbaWrap_memset(BufInfo.pAddr, 0, BufInfo.Size);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_REFSEG, "memset pInterDataBuf[%u] failed", i, 0U);
                }

                AmbaMisra_TypeCast(&pCtrl->pInterDataBuf[i], &BufInfo.pAddr);
            } else {
                SvcLog_NG(SVC_LOG_REFSEG, "Alloc Inter data buf failed", 0U, 0U);
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
static UINT32 CvFlow_RefSegInit(UINT32 CvFlowChan, void *pInit)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    static UINT32 g_RefSegInit = 0U;

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(pInit);

    if (0U == g_RefSegInit) {
        g_RefSegInit = 1U;

        if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&SegMutex, NULL)) {
            SvcLog_NG(SVC_LOG_REFSEG, "MutexCreate error", 0U, 0U);
            Rval = SVC_NG;
        }
        for (i = 0; i < SVC_REFSEG_MAX_INSTANCE; i++) {
            g_RefSegCtrl[i].Used = 0;
        }

        SVC_CV_DBG(SVC_LOG_REFSEG, "CvFlow_RefSegInit: first init", 0U, 0U);
    }

    SvcLog_OK(SVC_LOG_REFSEG, "CvFlow_RefSegInit: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver source rate update function
*/
static void RefSeg_SrcRateUpdate(SVC_REFSEG_CTRL_s *pCtrl, UINT32 SrcRate)
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
            (Den <= REFSEG_SRC_RATE_MAX_DENOMINATOR) && (Num <= Den)) {
            Rval = AmbaWrap_memset(&pCtrl->SrcValidTbl[0], 0, sizeof(pCtrl->SrcValidTbl));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REFSEG, "memset SrcValidTbl failed", 0U, 0U);
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
            SvcLog_NG(SVC_LOG_REFSEG, "Invalid ratio: %u/%u", Num, Den);
        }
    } else {
        /* Divisor */
        if (SrcRate == 0U) {
            pCtrl->SrcRateCtrl = 1U;
        } else {
            pCtrl->SrcRateCtrl = SrcRate;
        }
    }

    SvcLog_OK(SVC_LOG_REFSEG, "SrcRateCtrl: 0x%x", pCtrl->SrcRateCtrl, 0U);
}


/**
* CvFlow driver configuration function
*/
static UINT32 CvFlow_RefSegConfig(UINT32 CvFlowChan, void *pConfig)
{
    UINT32 Rval = SVC_OK;
    UINT32 i, j;
    SVC_REFSEG_CTRL_s *pCtrl = NULL;
    const SVC_CV_FLOW_REFSEG_MODE_INFO_s *pModeInfo;
    SVC_CV_INPUT_CFG_s *pInputCfg;

    AmbaMisra_TouchUnused(pConfig);

    if (NULL != pConfig) {
        RefSeg_MutexTake();
        /* Create instance */
        for (i = 0; i < SVC_REFSEG_MAX_INSTANCE; i++) {
            if (0U == g_RefSegCtrl[i].Used) {
                Rval = AmbaWrap_memset(&g_RefSegCtrl[i], 0, sizeof(SVC_REFSEG_CTRL_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_REFSEG, "memset g_RefSegCtrl[%u] failed", i, 0U);
                }
                pCtrl = &g_RefSegCtrl[i];
                pCtrl->Used = 1;
                SvcLog_DBG(SVC_LOG_REFSEG, "Create Inst(%u)", i, 0U);
                break;
            }
        }
        RefSeg_MutexGive();

        /* Configure instance */
        if (NULL != pCtrl) {
            pCtrl->CvfChan     = (UINT8) CvFlowChan;
            pCtrl->pCvFlow     = &pCtrl->CvCfg;
            Rval = AmbaWrap_memcpy(&pCtrl->CvCfg, pConfig, sizeof(SVC_CV_FLOW_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REFSEG, "memcpy CvCfg failed", 0U, 0U);
            }
            RefSeg_SrcRateUpdate(pCtrl, pCtrl->pCvFlow->InputCfg.SrcRate);
            pCtrl->Enable      = 1;

            pModeInfo = &SvcCvFlow_RefSeg_ModeInfo[pCtrl->pCvFlow->CvModeID];
            pInputCfg = &pCtrl->pCvFlow->InputCfg;
            if (pInputCfg->InputNum <= SVC_CV_FLOW_INPUT_MAX) {
                for (i = 0; i < pInputCfg->InputNum; i++) {
                    if (pInputCfg->Input[i].RoiInfo.NumRoi <= SVC_CV_MAX_ROI) {
                        for (j = 0; j < pInputCfg->Input[i].RoiInfo.NumRoi; j++) {
                            pInputCfg->Input[i].RoiInfo.Roi[j].Width  = pModeInfo->Cfg[0].NetWidth;
                            pInputCfg->Input[i].RoiInfo.Roi[j].Height = pModeInfo->Cfg[0].NetHeight;
                        }
                    } else {
                        Rval = SVC_NG;
                        break;
                    }
                }
            } else {
                Rval = SVC_NG;
            }

            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REFSEG, "unsupported InputNum(%u), NumRoi(%u)", pInputCfg->InputNum, pInputCfg->Input[0].RoiInfo.NumRoi);
            } else {
                if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG) {
                    Rval = SVC_OK;
                } else if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
#ifdef CONFIG_AMBALINK_BOOT_OS
                    /* Init FlexiDAG IO framework */
                    if (pCtrl->pCvFlow->PrivFlag < SVC_CV_FLEXIDAG_IO_MAX_CHANNEL) {
                        pCtrl->FlexiIOChan = pCtrl->pCvFlow->PrivFlag;
                        Rval = SvcCv_FlexiIORegister(pCtrl->FlexiIOChan, RefSeg_Init_Callback);
                        if (Rval != SVC_OK) {
                            SvcLog_NG(SVC_LOG_REFSEG, "SvcCv_FlexiIORegister(%u) failed", pCtrl->FlexiIOChan, 0U);
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_REFSEG, "Invalid FlexidagIO chan(%u)", pCtrl->pCvFlow->PrivFlag, 0U);
                    }
#else
                    Rval = SVC_NG;
#endif
                } else {
                    SvcLog_NG(SVC_LOG_REFSEG, "unsupported CVFlow ProfID(%u)", pModeInfo->ProfID, 0U);
                    Rval = SVC_NG;
                }
            }
        } else {
            Rval = SVC_NG;
        }
    } else {
        Rval = SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_REFSEG, "CvFlow_RefSegConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_REFSEG, "CvFlow_RefSegConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver load function
*/
static UINT32 CvFlow_RefSegLoad(UINT32 CvFlowChan, void *pInfo)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_REFSEG_CTRL_s *pCtrl;
    const SVC_CV_FLOW_LOAD_INFO_s *pLoadInfo;
    const SVC_CV_FLOW_REFSEG_MODE_INFO_s *pModeInfo;
    UINT32 i;

    AmbaMisra_TouchUnused(pInfo);

    Rval = RefSeg_CvfChan2Inst(CvFlowChan, &Inst);
    if (Rval == SVC_OK) {
        pCtrl = &g_RefSegCtrl[Inst];
        if (NULL != pInfo) {
            AmbaMisra_TypeCast(&pLoadInfo, &pInfo);
            if (pLoadInfo->NumBin > 0U) {
                /* Use external path */
                if (pLoadInfo->AlgoIdx < REFSEG_MAX_ALGO_NUM) {
                    Rval = SvcCvFlow_LoadAlgoBin(SVC_LOG_REFSEG, pLoadInfo, &pCtrl->AlgoOp[pLoadInfo->AlgoIdx]);
                } else {
                    SvcLog_NG(SVC_LOG_REFSEG, "CvFlow_RefSegLoad: CvFlowChan(%d) invalid AlgoIdx(%u)", CvFlowChan, pLoadInfo->AlgoIdx);
                }
            } else {
                /* Use ModeInfo */
                if (pCtrl->pCvFlow->CvModeID < REFSEG_NUM_MODE) {
                    pModeInfo = &SvcCvFlow_RefSeg_ModeInfo[pCtrl->pCvFlow->CvModeID];

                    for (i = 0; i < pModeInfo->AlgoGrp.NumAlgo; i++) {
                        Rval |= SvcCvFlow_LoadAlgo(SVC_LOG_REFSEG, &pModeInfo->AlgoGrp.AlgoInfo[i], &pCtrl->AlgoOp[i]);
                    }
                } else  {
                    SvcLog_NG(SVC_LOG_REFSEG, "CvFlow_RefSegLoad: CvFlowChan(%d) invalid CvModeID(%u)", CvFlowChan, pCtrl->pCvFlow->CvModeID);
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_REFSEG, "CvFlow_RefSegLoad: invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_REFSEG, "CvFlow_RefSegLoad: CvFlowChan(%d) error", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver start function
*/
static UINT32 CvFlow_RefSegStart(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst, i, j, AlgoIdx, Num;
    SVC_REFSEG_CTRL_s *pCtrl;
    const SVC_CV_FLOW_REFSEG_MODE_INFO_s *pModeInfo;
    SVC_CV_FLOW_BUF_INFO_s BufInfo = {0};
    const SVC_CV_FLOW_ALGO_INFO_s *pAlgoInfo;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
    SVC_CV_FLOW_ALGO_QUERY_CFG_s QueryCfg;
    SVC_CV_FLOW_ALGO_CREATE_CFG_s CreateCfg;
    ExtTaskCreateCfg CreateExtCfg;
    ExtTaskCreateCfg *pCreateExtCfg = &CreateExtCfg;
    SVC_CV_ALGO_REGCB_CFG_s CbCfg;
    SVC_IMG_FEEDER_PORT_CFG_s PortCfg = {0};
#ifdef CONFIG_AMBALINK_BOOT_OS
    SVC_CV_ALGO_LINUX_EX_CFG_s ExCfg;
#endif
    char Text[2];
    static char CbEvtQueName[] = "EvtQue##_RefSeg";
    static char SemName[] = "OutBufSem";
    static REFSEG_OUTPUT_MSG_s AlgoOutQue[SVC_REFSEG_MAX_INSTANCE][SVC_REFSEG_ALGO_OUT_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 SegRxTaskStack[SVC_REFSEG_MAX_INSTANCE][SVC_REFSEG_RX_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_IMG_FEEDER_EVENT_CALLBACK_f RefSeg_TxDataSrcHandlerList[SVC_REFSEG_MAX_INSTANCE] = {
        RefSeg_TxDataSrcHandler0,
        RefSeg_TxDataSrcHandler1,
        RefSeg_TxDataSrcHandler2,
        RefSeg_TxDataSrcHandler3,
    };
    static SVC_CV_ALGO_CALLBACK_f RefSeg_AlgoOutCbList[SVC_REFSEG_MAX_INSTANCE] = {
        RefSeg_AlgoOutCallback0,
        RefSeg_AlgoOutCallback1,
        RefSeg_AlgoOutCallback2,
        RefSeg_AlgoOutCallback3,
    };

    AmbaMisra_TouchUnused(pInput);
    AmbaMisra_TouchUnused(RefSeg_TxDataSrcHandlerList);
    AmbaMisra_TouchUnused(RefSeg_AlgoOutCbList);

    Rval = RefSeg_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl     = &g_RefSegCtrl[Inst];
        pModeInfo = &SvcCvFlow_RefSeg_ModeInfo[pCtrl->pCvFlow->CvModeID];

        /* Open Feeder port first to enable frame FIFO earlier */
        if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
            /* The driver supports single input per instance */
            PortCfg.NumPath            = 1U;
            PortCfg.Content[0].DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
            PortCfg.Content[0].StrmId  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
            PortCfg.SendFunc           = RefSeg_TxDataSrcHandlerList[Inst];
            if (SVC_OK != SvcImgFeeder_OpenPort(pCtrl->pCvFlow->InputCfg.FeederID, &PortCfg, &pCtrl->FeederPort)) {
                SvcLog_NG(SVC_LOG_REFSEG, "Open feeder port error", 0U, 0U);
            }
        } else {
            if (pCtrl->pCvFlow->InputCfg.Input[0].DataSrc != SVC_CV_DATA_SRC_EXT_DATA) {
                SvcLog_NG(SVC_LOG_REFSEG, "ImgFeeder is not enabled", 0U, 0U);
            }
        }

        for (AlgoIdx = 0; AlgoIdx < pModeInfo->AlgoGrp.NumAlgo; AlgoIdx++) {
            pAlgoInfo = &pModeInfo->AlgoGrp.AlgoInfo[AlgoIdx];
            pAlgoOp   = &pCtrl->AlgoOp[AlgoIdx];

            Rval = AmbaWrap_memset(&QueryCfg, 0, sizeof(SVC_CV_FLOW_ALGO_QUERY_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REFSEG, "memset QueryCfg failed", 0U, 0U);
            }
            Rval = SvcCvFlow_QueryAlgo(SVC_LOG_REFSEG, pAlgoInfo, &QueryCfg, pAlgoOp);
            if (SVC_OK == Rval) {
                /* Create algo */
                Rval = AmbaWrap_memset(&CreateCfg, 0, sizeof(SVC_CV_FLOW_ALGO_CREATE_CFG_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_REFSEG, "memset CreateCfg failed", 0U, 0U);
                }

                if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG) {
                    pCreateExtCfg->MagicCode = ExtTaskCreateCfgMagic;
                    pCreateExtCfg->RunTskCtrl.StackSz = ((UINT32)16U << 10U);
                    pCreateExtCfg->RunTskCtrl.Priority = 50;
                    pCreateExtCfg->RunTskCtrl.CoreSel = CONFIG_ICAM_SCA_TASK_CORE_SELECT;
                    pCreateExtCfg->CBTskCtrl.StackSz = ((UINT32)16U << 10U);
                    pCreateExtCfg->CBTskCtrl.Priority = 51;
                    pCreateExtCfg->CBTskCtrl.CoreSel = CONFIG_ICAM_SCA_TASK_CORE_SELECT;
                    pCreateExtCfg->ROIs = 1U;
                    AmbaMisra_TypeCast(&CreateCfg.pExtCfg, &pCreateExtCfg);
                } else if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
#ifdef CONFIG_AMBALINK_BOOT_OS
                   if (1U == SvcCv_FlexiIOStatusGet(pCtrl->FlexiIOChan)) {
                       const SVC_CV_ALGO_LINUX_EX_CFG_s *pExCfg = &ExCfg;

                       ExCfg.AmbaIPCChannel = pCtrl->FlexiIOChan;
                       AmbaMisra_TypeCast(&CreateCfg.pExtCfg, &pExCfg);
                   } else {
                       SvcLog_NG(SVC_LOG_REFSEG, "FlexidagIO(%u) has not ready. Please run Linux app first.", pCtrl->FlexiIOChan, 0U);
                       Rval = SVC_NG;
                   }
#else
                    SvcLog_NG(SVC_LOG_REFSEG, "CONFIG_AMBALINK_BOOT_OS not defined ", 0U, 0U);
                    Rval = SVC_NG;
#endif
                } else {
                    SvcLog_NG(SVC_LOG_REFSEG, "unsupported CVFlow ProfID(%u)", pModeInfo->ProfID, 0U);
                    Rval = SVC_NG;
                }

                if (SVC_OK == Rval) {
                    Rval = SvcCvFlow_CreateAlgo(SVC_LOG_REFSEG, pAlgoInfo, &CreateCfg, pAlgoOp);
                }

                if (SVC_OK == Rval) {
                    /* Create Output buffer */
                    for (i = 0; i < SVC_REFSEG_OUTPUT_DEPTH; i++) {
                        Num = pAlgoOp->OutputNum;
                        for (j = 0; j < Num; j++) {
                            Rval |= SvcCvFlow_AllocWorkBuf(pAlgoOp->OutputSize[j], &BufInfo);
                            SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_REFSEG, BufInfo.pAddr, BufInfo.Size, &pCtrl->AlgoOpEx[AlgoIdx].OutputBuf[i][j]);
                        }
                    }

                    if (SVC_OK == Rval) {
                        /* Register Algo callback */
                        Rval = AmbaWrap_memset(&CbCfg, 0, sizeof(SVC_CV_ALGO_REGCB_CFG_s));
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_REFSEG, "memset CbCfg failed", 0U, 0U);
                        }
                        CbCfg.Mode     = 0U;
                        CbCfg.Callback = RefSeg_AlgoOutCbList[Inst];
                        Rval = SvcCvAlgo_RegCallback(&pAlgoOp->AlgoHandle, &CbCfg);
                        if (SVC_OK == Rval) {
                            Rval = RefSeg_Config(Inst, AlgoIdx);
                            if (SVC_OK != Rval) {
                                SvcLog_NG(SVC_LOG_REFSEG, "FlexiDAG Config error(0x%X)", Rval, 0U);
                            }

                            pCtrl->NumValidAlgo++;
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_REFSEG, "Alloc Algo[%u] Output Buf failed", AlgoIdx, 0U);
                    }
                } else {
                   SvcLog_NG(SVC_LOG_REFSEG, "CreateAlgo[%u] error", AlgoIdx, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_REFSEG, "QueryAlgo[%u] error", AlgoIdx, 0U);
            }
        }

        if (SVC_OK == Rval) {
            Rval = RefSeg_ConfigBuffer(Inst);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REFSEG, "ConfigBuffer error", 0U, 0U);
            }
        }

        if (SVC_OK == Rval) {
            pCtrl->pTxProc = RefSeg_TxDataSrcHandler;
            pCtrl->pRxProc = RefSeg_RxDataSinkHandler;

            if (1U != AmbaUtility_UInt32ToStr(&Text[0], (UINT32)sizeof(Text), Inst, 10)) {
                SvcLog_NG(SVC_LOG_REFSEG, "UInt32ToStr failed", 0U, 0U);
            }
            CbEvtQueName[6] = Text[0];

            Rval = AmbaKAL_MsgQueueCreate(&pCtrl->AlgoOutQueId,
                                           CbEvtQueName,
                                           (UINT32)sizeof(REFSEG_OUTPUT_MSG_s),
                                           &AlgoOutQue[Inst][0],
                                           SVC_REFSEG_ALGO_OUT_QUEUE_SIZE * (UINT32)sizeof(REFSEG_OUTPUT_MSG_s));

            Rval |= AmbaKAL_SemaphoreCreate(&pCtrl->OutBufSem, SemName, SVC_REFSEG_SEM_INIT_COUNT);

            if (KAL_ERR_NONE == Rval) {
                pCtrl->RxTaskCtrl.Priority   = 60;
                pCtrl->RxTaskCtrl.EntryFunc  = RefSeg_RxTaskEntry;
                pCtrl->RxTaskCtrl.EntryArg   = Inst;
                pCtrl->RxTaskCtrl.pStackBase = &SegRxTaskStack[Inst][0];
                pCtrl->RxTaskCtrl.StackSize  = SVC_REFSEG_RX_TASK_STACK_SIZE;
                pCtrl->RxTaskCtrl.CpuBits    = 0x01U;
                Rval |= SvcTask_Create("SvcSegRxTask", &pCtrl->RxTaskCtrl);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_REFSEG, "Create RxTask Inst(%u) error", Inst, 0U);
                }

                /* Start input */
                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_REFSEG, "Connect feeder port error", 0U, 0U);
                    }
                }

                if (SVC_OK == Rval) {
                    pCtrl->Started = 1U;
                }
            } else {
                SvcLog_NG(SVC_LOG_REFSEG, "Create MsgQue/Semaphore error(0x%X)", Rval, 0U);
                Rval =  SVC_NG;
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_REFSEG, "Invalid CvFlowChan", 0U, 0U);
        Rval =  SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_REFSEG, "CvFlow_RefSegStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_REFSEG, "CvFlow_RefSegStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver control function
*/
static UINT32 CvFlow_RefSegControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo)
{
    UINT32 Rval, Inst, Index;
    SVC_REFSEG_CTRL_s *pCtrl;
    const UINT32 *pValue;
    SVC_CV_ROI_INFO_s  *pRoiInfo = NULL;
    SVC_CV_INPUT_INFO_s *pInputInfo = NULL;
    SVC_CV_MASK_INFO_s *pMaskInfo = NULL;
    const SVC_CV_FLOW_REFSEG_MASK_CFG_s *pMaskCfg;
    SVC_CV_FRAME_TIME_INFO_s *pFTInfo = NULL;

    RefSeg_MutexTake();

    Rval = RefSeg_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl = &g_RefSegCtrl[Inst];
        switch (CtrlType) {
            case SVC_CV_CTRL_ENABLE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->Enable = (UINT8) *pValue;
                if ((1U == pCtrl->Started) &&
                    (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder)) {
                    if (*pValue == 0U) {
                        Rval = SvcImgFeeder_Disconnect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_REFSEG, "Disconn feeder port error", 0U, 0U);
                        }
                    } else {
                        Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_REFSEG, "Cnnect feeder port error", 0U, 0U);
                        }
                    }
                }
                break;
            case SVC_CV_CTRL_SET_SRC_RATE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->SrcRateCtrl = *pValue;
                SvcLog_OK(SVC_LOG_REFSEG, "Control[%d]: SrcRate = %u", Inst, *pValue);
                break;
            case SVC_CV_CTRL_SEND_EXT_YUV:
                Rval = RefSeg_SendExtYUV(Inst, pInfo);
                break;
            case SVC_CV_CTRL_GET_ROI_INFO:
                AmbaMisra_TypeCast(&pRoiInfo, &pInfo);
                pInputInfo = &pCtrl->pCvFlow->InputCfg.Input[0];
                if (pInputInfo->RoiInfo.NumRoi != 0U) {
                    Rval = AmbaWrap_memcpy(pRoiInfo, &pInputInfo->RoiInfo, sizeof(SVC_CV_ROI_INFO_s));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_REFSEG, "memcpy pRoiInfo failed", 0U, 0U);
                    }
                } else {
                    Rval = AmbaWrap_memset(pRoiInfo, 0x0, sizeof(SVC_CV_ROI_INFO_s));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_REFSEG, "memset pRoiInfo failed", 0U, 0U);
                    }
                }
                break;
            case SVC_CV_CTRL_SET_ROI_INFO:
                AmbaMisra_TypeCast(&pRoiInfo, &pInfo);
                pInputInfo = &pCtrl->pCvFlow->InputCfg.Input[0];
                if (pInputInfo->RoiInfo.NumRoi != 0U) {
                    Rval = AmbaWrap_memcpy(&pInputInfo->RoiInfo, pRoiInfo, sizeof(SVC_CV_ROI_INFO_s));
                    if (SVC_OK == Rval) {
                        Rval = RefSeg_ConfigRoi(Inst, 0U, &pInputInfo->RoiInfo);
                    } else {
                        SvcLog_NG(SVC_LOG_REFSEG, "memcpy pRoiInfo failed", 0U, 0U);
                    }
                }
                break;
            case SVC_CV_CTRL_GET_MASK_INFO:
                Index = pCtrl->pCvFlow->CvModeID;
                AmbaMisra_TypeCast(&pMaskInfo, &pInfo);
                pMaskInfo->NumRoi = 0U;
                pMaskCfg = SvcCvFlow_RefSeg_ModeInfo[Index].Cfg[0].pMaskCfg;
                if (NULL != pMaskCfg) {
                    pMaskInfo->NumRoi = 1U;
                    pMaskInfo->Roi[0].NumCat = pMaskCfg->NumCat;
                    pMaskInfo->Roi[0].Width  = pMaskCfg->BufWidth;
                    pMaskInfo->Roi[0].Height = pMaskCfg->BufHeight;
                } else {
                    Rval = AmbaWrap_memset(&pMaskInfo->Roi[0], 0x0, sizeof(SVC_CV_MASK_s));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_REFSEG, "memset pMaskInfo failed", 0U, 0U);
                    }
                }
                break;
            case SVC_CV_CTRL_GET_OUTPUT_FRAME_TIME:
                if (pInfo != NULL) {
                    AmbaMisra_TypeCast(&pFTInfo, &pInfo);
                    pFTInfo->ProcTimeFD  = pCtrl->FTInfo.ProcTimeMA[0];
                    pFTInfo->ProcTimeAll = pCtrl->FTInfo.ProcTimeMA[0];
                    pFTInfo->FrameTime   = pCtrl->FTInfo.FrameTimeMA;
                } else {
                    Rval = SVC_NG;
                }
                break;
            default:
                SvcLog_NG(SVC_LOG_REFSEG, "Control[%d]: Invalid CtrlType(%d)", Inst, CtrlType);
                break;
        }
    } else {
        SvcLog_NG(SVC_LOG_REFSEG, "Control: Invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    RefSeg_MutexGive();

    return Rval;
}

/**
* CvFlow driver stop function
*/
static UINT32 CvFlow_RefSegStop(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_REFSEG_CTRL_s *pCtrl;
    SVC_CV_FLOW_ALGO_DELETE_CFG_s DelCfg;
    UINT32 i;

    AmbaMisra_TouchUnused(pInput);

    Rval = RefSeg_CvfChan2Inst(CvFlowChan, &Inst);

    if (SVC_OK == Rval) {
        pCtrl = &g_RefSegCtrl[Inst];

        Rval = RefSeg_SetTaskIdle(Inst);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_REFSEG, "SetTaskIdle error", 0U, 0U);
        }

        Rval = SvcTask_Destroy(&pCtrl->RxTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_REFSEG, "Destroy Tx/Rx Task error", 0U, 0U);
        }

        Rval = AmbaWrap_memset(&DelCfg, 0, sizeof(SVC_CV_FLOW_ALGO_DELETE_CFG_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_REFSEG, "memset DelCfg failed", 0U, 0U);
        }

        for (i = 0; i < pCtrl->NumValidAlgo; i++) {
            Rval |= SvcCvFlow_DeleteAlgo(SVC_LOG_REFSEG, &DelCfg, &pCtrl->AlgoOp[i]);
        }
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_REFSEG, "Delele Algo error", 0U, 0U);
        }

#ifdef CONFIG_AMBALINK_BOOT_OS
        if (SvcCvFlow_RefSeg_ModeInfo[pCtrl->pCvFlow->CvModeID].ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
            Rval = SvcCv_FlexiIORelease(pCtrl->FlexiIOChan);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_REFSEG, "SvcCv_FlexiIORelease(%u) failed", pCtrl->FlexiIOChan, 0U);
            }
        }
#endif

        Rval = AmbaKAL_MsgQueueDelete(&pCtrl->AlgoOutQueId);
        Rval |= AmbaKAL_SemaphoreDelete(&pCtrl->OutBufSem);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_REFSEG, "Delete MsgQue/Semaphore error(0x%X)", Rval, 0U);
        }

        /* Reset Ctrl Intance */
        Rval = AmbaWrap_memset(&g_RefSegCtrl[Inst], 0, sizeof(SVC_REFSEG_CTRL_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_REFSEG, "memset g_RefSegCtrl failed", 0U, 0U);
        }
    }

    SvcLog_OK(SVC_LOG_REFSEG, "CvFlow_RefSegStop: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver debug interface
*/
static UINT32 CvFlow_RefSegDebug(UINT32 CvFlowChan, const char *pCmd, UINT32 Param0, UINT32 Param1, UINT32 Param2)
{
    UINT32 i;
    ULONG Addr;
    const SVC_REFSEG_CTRL_s *pCtrl;
    const AMBA_CV_FLEXIDAG_HANDLE_s *pCvFDHandle;

    if (0 == SvcWrap_strcmp("debug", pCmd)) {
        SvcCvDebugEnable = (UINT8) Param0;
        SvcLog_DBG(SVC_LOG_REFSEG, "Set debug(%u)", Param0, 0U);
    } else if (0 == SvcWrap_strcmp("info", pCmd)) {
        for (i = 0; i < SVC_REFSEG_MAX_INSTANCE; i++) {
            pCtrl = &g_RefSegCtrl[i];
            if (pCtrl->Used == 1U) {
                AmbaPrint_PrintUInt5("RefSeg[%d]: CvfChan %d", i, pCtrl->CvfChan, 0U, 0U, 0U);
                pCvFDHandle = &pCtrl->AlgoOp[0].AlgoHandle.FDs[0].Handle;
                AmbaMisra_TypeCast(&Addr, &pCvFDHandle);
                SvcCvFlow_PrintULong("  CV_FLEXIDAG_HANDLE: 0x%x", Addr, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        SvcLog_DBG(SVC_LOG_REFSEG, "Unsupported cmd", 0U, 0U);
    }

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(&Param0);
    AmbaMisra_TouchUnused(&Param1);
    AmbaMisra_TouchUnused(&Param2);

    return SVC_OK;
}

SVC_CV_FLOW_OBJ_s SvcCvFlow_RefSegObj = {
    .Init         = CvFlow_RefSegInit,
    .Config       = CvFlow_RefSegConfig,
    .Load         = CvFlow_RefSegLoad,
    .Start        = CvFlow_RefSegStart,
    .Control      = CvFlow_RefSegControl,
    .Stop         = CvFlow_RefSegStop,
    .Debug        = CvFlow_RefSegDebug,
};

