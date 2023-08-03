/**
 *  @file SvcCvFlow_AmbaOD.c
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
 *  @details Implementation of FlexiDAG based Ambarella Object Detection
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event.h"
#include "AmbaMMU.h"
#include "AmbaDef.h"
#include "AmbaUtility.h"
#include "AmbaFS.h"
#include "AmbaPrint.h"
#include "AmbaDef.h"

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
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcImgFIFO.h"
#include "SvcImgFeeder.h"
#include "SvcOsd.h"
#include "SvcWrap.h"

#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_ref_util.h"
#include "cv_ambaod_header.h"
#include "idsp_roi_msg.h"
#include "cvapi_svccvalgo_ambaod.h"
#include "cvapi_svccvalgo_ambaperception.h"
#include "cvapi_amba_od37_fc_category.h"

//#define AMBAOD_POST_FILTER
#if defined AMBAOD_POST_FILTER
#include "AmbaODFilter.c"
#endif

#define SVC_LOG_AMBAOD        "AmbaOD"

#define SVC_AMBAOD_INSTANCE0                (0U)
#define SVC_AMBAOD_INSTANCE1                (1U)
#define SVC_AMBAOD_INSTANCE2                (2U)
#define SVC_AMBAOD_INSTANCE3                (3U)
#define SVC_AMBAOD_MAX_INSTANCE             (4U)

#define SVC_AMBAOD_SEM_INIT_COUNT           (1U)
#define SVC_AMBAOD_OUTPUT_DEPTH             (SVC_AMBAOD_SEM_INIT_COUNT + 4U)

#define SVC_AMBAOD_RX_TASK_STACK_SIZE       (0x4000)
#define SVC_AMBAOD_PICINFO_QUEUE_SIZE       (8U)
#define SVC_AMBAOD_ALGO_OUT_QUEUE_SIZE      (SVC_AMBAOD_OUTPUT_DEPTH)

#define SRC_RATE_FRACTION_MODE_BIT          (0x10000000U)

typedef UINT32 (*SVC_AMBAOD_DATA_HANDLER_f)(UINT32 Inst);
typedef UINT32 (*SVC_AMBAOD_TX_HANDLER_f)(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

typedef struct {
    flexidag_memblk_t           OutputBuf[SVC_AMBAOD_OUTPUT_DEPTH][FLEXIDAG_MAX_OUTPUTS];
    AMBA_CV_FLEXIDAG_IO_s       InBuf[SVC_AMBAOD_OUTPUT_DEPTH];
    AMBA_CV_FLEXIDAG_IO_s       OutBuf[SVC_AMBAOD_OUTPUT_DEPTH];
} SVC_CV_FLOW_AMBAOD_OP_EX_INFO_s;

typedef struct {
    UINT8                       Used;
    UINT8                       CvfChan;
    UINT8                       Enable;
    UINT8                       Started;
    SVC_CV_FLOW_CFG_s           *pCvFlow;
    SVC_CV_FLOW_CFG_s           CvCfg;

    UINT8                       NumValidAlgo;
    SVC_CV_FLOW_ALGO_OP_INFO_s  AlgoOp[AMBAOD_MAX_ALGO_NUM];
    SVC_CV_FLOW_AMBAOD_OP_EX_INFO_s AlgoOpEx[AMBAOD_MAX_ALGO_NUM];
    void                       *pImgInfoBuf[SVC_AMBAOD_PICINFO_QUEUE_SIZE];
    UINT32                      ImgInfoBufSize;
    UINT32                      FlexiBinBits;
    UINT32                      NumODScale;
    UINT8                       NumStage;

    /* Data source service */
    SVC_IMG_FEEDER_PORT_s       FeederPort;
    UINT32                      SrcSeqNum;
    SVC_AMBAOD_TX_HANDLER_f     pTxProc;
    UINT32                      TxSeqNum;
    UINT32                      PicInfoIdx;
    /* Driver intermediate data buffer */
    void                        *pInterDataBuf[SVC_AMBAOD_OUTPUT_DEPTH];

    /* Data sink service */
    SVC_TASK_CTRL_s             RxTaskCtrl;
    SVC_AMBAOD_DATA_HANDLER_f   pRxProc;
    UINT32                      RxSeqNum;
    UINT32                      OutBufIdx;
    #define AMBAOD_PIPE_STAGE_1                             (0U)
    #define AMBAOD_PIPE_STAGE_2                             (1U)
    #define AMBAOD_PIPE_STAGE_NUM                           (2U)
    AMBA_KAL_SEMAPHORE_t        OutBufSem[AMBAOD_PIPE_STAGE_NUM];
    UINT32                      SinkInfoIdx;

    /* Algo Callback */
    AMBA_KAL_MSG_QUEUE_t        AlgoOutQueId[AMBAOD_PIPE_STAGE_NUM];

    /* Misc */
    UINT32                      SrcRateCtrl;
#define                         AMBAOD_SRC_RATE_MAX_DENOMINATOR  (16U)
    UINT8                       SrcValidTbl[AMBAOD_SRC_RATE_MAX_DENOMINATOR];
    UINT8                       SrcRateNum;
    UINT8                       SrcRateDen;
    SVC_CV_FRAME_TIME_CALC_s    FTInfo;

    UINT8                       SrcInfoErr;
    UINT8                       SceneMode;
    UINT8                       TuningModeOn;
    //#define                   AMBAOD_TUNING_MODE_NORMAL     (0U)
    //#define                   AMBAOD_TUNING_MODE_BENCHMARK  (1U)
    UINT8                       TuningMode;

    #define                     AMBAOD_VER_UNKNOWN          (0U)
    #define                     AMBAOD_VER_36               (3600U)
    #define                     AMBAOD_VER_37               (3700U)
    #define                     AMBAOD_VER_37_ADD_PERCEPTION  (1U)
    UINT32                      AmbaODVer;

    #define                     AMBAOD_TYPE_FC              (0U)
    #define                     AMBAOD_TYPE_EMR             (1U)
    UINT32                      AmbaODType;

    SVC_CV_FLOW_AMBAOD_PCPT_FEATURE_s PcptFeature;
} SVC_AMBAOD_CTRL_s;

typedef struct {
    memio_source_recv_picinfo_t *pPicInfo;      /* input pic info */
    SVC_CV_DETRES_BBX_LIST_s     BbxList;       /* BBX output */
    AmbaPcptCfgNext              NextStepCfg;   /* Perception cfg */
    SVC_CV_PERCEPTION_OUTPUT_s   PcptOut;       /* Perception output */
    UINT64 TimeStamp[SVC_CV_FLOW_MAX_TIME_STAMP];
} AMBAOD_INTER_DATA_s;

typedef struct {
   UINT32 Event;
   SVC_CV_ALGO_OUTPUT_s AlgoOut;
} AMBAOD_OUTPUT_MSG_s;

typedef struct { const char **pDPtr; } CHAR_PTR_s;


static SVC_AMBAOD_CTRL_s g_AmbaODCtrl[SVC_AMBAOD_MAX_INSTANCE] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MUTEX_t ODMutex;
static UINT8 SvcCvDebugEnable = 0;

static UINT32 CvFlow_AmbaODInit(UINT32 CvFlowChan, void *pInit);
static UINT32 CvFlow_AmbaODConfig(UINT32 CvFlowChan, void *pConfig);
static UINT32 CvFlow_AmbaODStart(UINT32 CvFlowChan, void *pInput);
static UINT32 CvFlow_AmbaODControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo);
static UINT32 CvFlow_AmbaODStop(UINT32 CvFlowChan, void *pInput);
static UINT32 AmbaOD_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst);

static UINT32 AmbaOD_TxDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 AmbaOD_TxDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 AmbaOD_TxDataSrcHandler2(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 AmbaOD_TxDataSrcHandler3(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

static UINT32 AmbaOD_AlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 AmbaOD_AlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 AmbaOD_AlgoOutCallback2(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 AmbaOD_AlgoOutCallback3(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);

static void   AmbaOD_BbxDataPrepare(const SVC_AMBAOD_CTRL_s *pCtrl, AMBAOD_INTER_DATA_s *pInterData, const SVC_CV_ALGO_OUTPUT_s *pAlgoOut);
static void   AmbaOD_PcptDataPrepare(SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut, AMBAOD_INTER_DATA_s *pInterData, const SVC_CV_ALGO_OUTPUT_s *pAlgoOut);
static void   AmbaOD_PcptDataResolve(SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut, const SVC_CV_ALGO_OUTPUT_s *pAlgoOut);
static void   AmbaOD_DumpPcptKpResult(const SVC_CV_ALGO_OUTPUT_s *pEventData);
static void   AmbaOD_DumpPcpt3DResult(const SVC_CV_ALGO_OUTPUT_s *pEventData);
static void   AmbaOD_DumpPcptMaskResult(const SVC_CV_ALGO_OUTPUT_s *pEventData);
#if defined AMBAOD_POST_FILTER
static void   AmbaOD_BbxPostFilter(const SVC_AMBAOD_CTRL_s *pCtrl, const SVC_CV_ALGO_OUTPUT_s *pAlgoOut);
#endif

static UINT32 AmbaOD_ConfigRoi(UINT32 Inst, UINT32 AlgoIdx, const SVC_CV_INPUT_INFO_s *pInputInfo);
static UINT32 AmbaOD_ConfigNms(UINT32 Inst, UINT32 AlgoIdx, const SVC_CV_FLOW_AMBAOD_NMS_CFG_s *pNmsCfg);
static UINT32 AmbaOD_ConfigPcptFeature(UINT32 Inst, UINT32 AlgoIdx, const void *pFeature);
static void   AmbaOD_UpdatePcptNextStepCfg(AmbaPcptCfgNext *pNextCfg, const SVC_CV_FLOW_AMBAOD_PCPT_FEATURE_s *pFeature);
static UINT32 AmbaOD_ConfigLicense(UINT32 Inst, UINT32 AlgoIdx, UINT32 NumScale);
static UINT32 AmbaOD_ConfigTuningModeV2(UINT32 Inst, UINT32 AlgoIdx, UINT32 TuningMode);

static void SVC_CV_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcCvDebugEnable > 0U) {
        SvcLog_DBG(pModule, pFormat, Arg1, Arg2);
    }
}

static void AmbaOD_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&ODMutex, 5000)) {
        SvcLog_NG(SVC_LOG_AMBAOD, "AmbaOD_MutexTake: timeout", 0U, 0U);
    }
}

static void AmbaOD_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&ODMutex)) {
        SvcLog_NG(SVC_LOG_AMBAOD, "AmbaOD_MutexGive: error", 0U, 0U);
    }
}

static UINT32 AmbaOD_SemTake(UINT32 Inst, AMBA_KAL_SEMAPHORE_t *pSemaphore)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreTake(pSemaphore, 10000)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_AMBAOD, "AmbaOD_SemTake[%d]: timeout", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 AmbaOD_SemGive(UINT32 Inst, AMBA_KAL_SEMAPHORE_t *pSemaphore)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreGive(pSemaphore)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_AMBAOD, "AmbaOD_SemGive[%d]: error", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 AmbaOD_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst)
{
    UINT32 Rval = SVC_NG;
    UINT32 i;

    if (NULL != pInst) {
        for (i = 0; i < SVC_AMBAOD_MAX_INSTANCE; i++) {
            if ((1U == g_AmbaODCtrl[i].Used) && (CvfChan == g_AmbaODCtrl[i].CvfChan)) {
                *pInst = i;
                Rval = SVC_OK;
                break;
            }
        }
    }
    return Rval;
}

static void AmbaOD_DumpPicInfo(const cv_pic_info_t *pPicInfo)
{
    static UINT32 DumpTimes = 0U;
    extern void SvcCvFlow_DumpPicInfo(const char *pModuleName, const cv_pic_info_t *pPicInfo);

    if (DumpTimes > 0U) {
        SvcCvFlow_DumpPicInfo(SVC_LOG_AMBAOD, pPicInfo);
        DumpTimes--;
    }
}

/**
* Pack data source to pic info structure
*/
static void AmbaOD_PackPicInfo(UINT32 Inst, const SVC_CV_INPUT_IMG_INFO_s *pInputImg, memio_source_recv_picinfo_t *pInfo)
{
    UINT32 Rval = SVC_OK;
    cv_pic_info_t *pPicInfo = &pInfo->pic_info;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    const SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[Inst];
    UINT32 DataSrc, RelativeAddrOn = 1U;
    extern UINT32 SvcCvFlow_PackPicInfo_Pyramid(cv_pic_info_t *pPicInfo, const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo, UINT32 MainWidth, UINT32 MainHeight, UINT32 UseRelative, UINT32 LeftRight);
    extern UINT32 SvcCvFlow_PackPicInfo_MainYuv(cv_pic_info_t *pPicInfo, const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo, UINT32 UseRelative, UINT32 LeftRight);

    Rval = AmbaWrap_memset(pInfo, 0, sizeof(memio_source_recv_picinfo_t));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_AMBAOD, "memset picinfo failed", 0U, 0U);
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

            if (SvcCvFlow_AmbaOD_ModeInfo[pCtrl->pCvFlow->CvModeID].ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
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

    AmbaOD_DumpPicInfo(pPicInfo);

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_AMBAOD, "AmbaOD_PackPicInfo error", 0U, 0U);
    }
}

/**
* Update source rate control rule
*/
static void AmbaOD_SrcRateUpdate(SVC_AMBAOD_CTRL_s *pCtrl, UINT32 SrcRate)
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
            (Den <= AMBAOD_SRC_RATE_MAX_DENOMINATOR) && (Num <= Den)) {
            Rval = AmbaWrap_memset(&pCtrl->SrcValidTbl[0], 0, sizeof(pCtrl->SrcValidTbl));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_AMBAOD, "memset SrcValidTbl failed", 0U, 0U);
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
            SvcLog_NG(SVC_LOG_AMBAOD, "Invalid ratio: %u/%u", Num, Den);
        }
    } else {
        /* Divisor */
        if (SrcRate == 0U) {
            pCtrl->SrcRateCtrl = 1U;
        } else {
            pCtrl->SrcRateCtrl = SrcRate;
        }
    }

    SvcLog_OK(SVC_LOG_AMBAOD, "SrcRateCtrl: 0x%x", pCtrl->SrcRateCtrl, 0U);
}

/**
* Check if the data source is desired
*/
static UINT32 AmbaOD_SrcFrameCheck(const SVC_AMBAOD_CTRL_s *pCtrl, UINT32 SeqNum)
{
    UINT32 Rval = 0U;
    UINT32 Remainder;

    if (0U < (pCtrl->SrcRateCtrl & SRC_RATE_FRACTION_MODE_BIT)) {
        Remainder = SeqNum % (UINT32)pCtrl->SrcRateDen;
        Rval = pCtrl->SrcValidTbl[Remainder];
    } else {
        Remainder = SeqNum % pCtrl->SrcRateCtrl;
        if (Remainder == 0U) {
            Rval = 1U;
        }
    }

    return Rval;
}

/**
* Send message to Feeder to update port status
*/
static void AmbaOD_SendReceiverReady(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort)
{
    SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[Inst];
    AMBA_KAL_SEMAPHORE_INFO_s SemInfo;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreQuery(&pCtrl->OutBufSem[AMBAOD_PIPE_STAGE_1], &SemInfo)) {
        if (SemInfo.CurrentCount > 0U) {
            if (SVC_OK != SvcImgFeeder_MsgToFeeder(pPort, SVC_IMG_FEEDER_RECEIVER_READY, NULL)) {
                SvcLog_NG(SVC_LOG_AMBAOD, "Send ReceiverReady[%u] error", Inst, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_AMBAOD, "ReceiverReady[%u]: SemQuery error", Inst, 0U);
    }
}

/**
* TX data source handler: process input data
*/
static UINT32 AmbaOD_TxDataSrcHandler(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[Inst];
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
    const SVC_CV_INPUT_IMG_INFO_s *pInputImg;
    memio_source_recv_picinfo_t *pPicInfo;
    UINT32 i, j;
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;
    AMBA_CV_FLEXIDAG_IO_s *pInBuf, *pOutBuf;
    AMBAOD_INTER_DATA_s *pInterData;

    AmbaMisra_TouchUnused(pInfo);

    SVC_CV_DBG(SVC_LOG_AMBAOD, "Tx[%u]: run %d", Inst, pCtrl->TxSeqNum);

    if (Event == SVC_IMG_FEEDER_IMAGE_DATA_READY) {
        if ((pCtrl->Enable == 1U) &&
            (1U == AmbaOD_SrcFrameCheck(pCtrl, pCtrl->SrcSeqNum))) {
            Rval = AmbaOD_SemTake(Inst, &pCtrl->OutBufSem[AMBAOD_PIPE_STAGE_1]);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pInputImg, &pInfo);
                AmbaMisra_TypeCast(&pPicInfo, &pCtrl->pImgInfoBuf[pCtrl->PicInfoIdx]);
                AmbaOD_PackPicInfo(Inst, pInputImg, pPicInfo);

                /* Prepare feed config */
                Rval = AmbaWrap_memset(&FeedCfg, 0, sizeof(SVC_CV_ALGO_FEED_CFG_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_AMBAOD, "memset FeedCfg failed", 0U, 0U);
                }

                for (i = 0; i < pCtrl->NumValidAlgo; i++) {
                    pAlgoOp = &pCtrl->AlgoOp[i];
                    pInBuf = &pCtrl->AlgoOpEx[i].InBuf[pCtrl->OutBufIdx];
                    pOutBuf = &pCtrl->AlgoOpEx[i].OutBuf[pCtrl->OutBufIdx];

                    FeedCfg.pIn  = pInBuf;
                    FeedCfg.pOut = pOutBuf;
                    pInBuf->num_of_buf = 1U;
                    SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_AMBAOD, pPicInfo, pCtrl->ImgInfoBufSize, &pInBuf->buf[0]);
                    pOutBuf->num_of_buf = pAlgoOp->OutputNum;
                    for (j = 0U; j < pAlgoOp->OutputNum; j++) {
                        Rval = AmbaWrap_memcpy(&pOutBuf->buf[j], &pCtrl->AlgoOpEx[i].OutputBuf[pCtrl->OutBufIdx][j], sizeof(flexidag_memblk_t));
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_AMBAOD, "memcpy OutputBufBlk failed", 0U, 0U);
                        }
                    }

                    /* Prepare inter data and pass pInterData through user data */
                    AmbaMisra_TypeCast(&pInterData, &(pCtrl->pInterDataBuf[pCtrl->OutBufIdx]));
                    pInterData->pPicInfo = pPicInfo;
                    AmbaMisra_TypeCast(&FeedCfg.pUserData, &pInterData);

                    if ((pCtrl->AmbaODVer == AMBAOD_VER_37) ||
                        (pCtrl->AmbaODVer == (AMBAOD_VER_37|AMBAOD_VER_37_ADD_PERCEPTION))) {
                        AmbaOD_UpdatePcptNextStepCfg(&pInterData->NextStepCfg, &pCtrl->PcptFeature);
                        FeedCfg.pExtFeedCfg = &pInterData->NextStepCfg;
                    }

                    SvcCvFlow_GetTimeStamp(SVC_LOG_AMBAOD, &pInterData->TimeStamp[SVC_CV_FLOW_TIME_STAMP0]);

                    Rval |= SvcCvAlgo_Feed(&pAlgoOp->AlgoHandle, &FeedCfg);
                    SVC_CV_DBG(SVC_LOG_AMBAOD, "Tx[%u]: Send(%u)", Inst, pPicInfo->pic_info.frame_num);
                }

                if (SVC_OK == Rval) {
                    pCtrl->TxSeqNum++;
                    pCtrl->PicInfoIdx++;
                    if (pCtrl->PicInfoIdx >= SVC_AMBAOD_PICINFO_QUEUE_SIZE) {
                        pCtrl->PicInfoIdx = 0;
                    }
                    pCtrl->OutBufIdx++;
                    if (pCtrl->OutBufIdx >= SVC_AMBAOD_OUTPUT_DEPTH) {
                        pCtrl->OutBufIdx = 0;
                    }
                } else {
                    SvcLog_NG(SVC_LOG_AMBAOD, "Tx[%u]: send error", Inst, 0U);
                }

                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    AmbaOD_SendReceiverReady(Inst, pPort);
                }
            } else {
                SvcLog_NG(SVC_LOG_AMBAOD, "Tx[%u]: SemTake error", Inst, 0U);
            }
        } else {
            if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                AmbaOD_SendReceiverReady(Inst, pPort);
                Rval = AmbaKAL_TaskSleep(1);
            }
        }

        pCtrl->SrcSeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_AMBAOD, "Tx[%u]: unsupported event(0x%x)", Inst, Event);
        Rval = SVC_NG;
    }

    SVC_CV_DBG(SVC_LOG_AMBAOD, "Tx[%u]: end", Inst, 0U);

    return Rval;
}

static UINT32 AmbaOD_TxDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[SVC_AMBAOD_INSTANCE0];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_AMBAOD_INSTANCE0, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 AmbaOD_TxDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[SVC_AMBAOD_INSTANCE1];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_AMBAOD_INSTANCE1, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 AmbaOD_TxDataSrcHandler2(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[SVC_AMBAOD_INSTANCE2];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_AMBAOD_INSTANCE2, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 AmbaOD_TxDataSrcHandler3(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[SVC_AMBAOD_INSTANCE3];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_AMBAOD_INSTANCE3, pPort, Event, pInfo);
    }
    return Rval;
}

/**
* Transfer detection result to SVC structure
*/
static void AmbaOD_AmbaODTrans(const SVC_AMBAOD_CTRL_s *pCtrl, const memio_source_recv_picinfo_t *pInPicInfo, const amba_od_out_t *pOdResult, SVC_CV_DETRES_BBX_LIST_s *pBbxList)
{
    UINT32 i = 0;
    UINT32 ObjNum;
    ULONG ObjAddr;
    const amba_od_candidate_t   *pObjBase, *pObj;
    CHAR_PTR_s CharPtr;

    AmbaMisra_TouchUnused(&pCtrl);
    AmbaMisra_TouchUnused(&pInPicInfo);

    AmbaMisra_TypeCast(&ObjAddr, &pOdResult);
    ObjAddr = ObjAddr + pOdResult->objects_offset;
    AmbaMisra_TypeCast(&pObjBase, &ObjAddr);
    ObjNum = pOdResult->num_objects;

    //AmbaPrint_PrintUInt5("[AmbaOD_AmbaODTrans] amba OD ========= total:%3d =============", ObjNum, 0U, 0U, 0U, 0U);
    AmbaMisra_TypeCast(&(pBbxList->pCvOut), &pOdResult);
    if (ObjNum <= SVC_CV_DETRES_MAX_BBX_NUM) {
        if (NULL != pInPicInfo) {
            pBbxList->CaptureTime = pInPicInfo->pic_info.capture_time;
            pBbxList->FrameNum    = pInPicInfo->pic_info.frame_num;
        } else {
            pBbxList->CaptureTime = pOdResult->capture_time;
            pBbxList->FrameNum    = pOdResult->frame_num;
        }
        pBbxList->BbxAmount   = ObjNum;
        pBbxList->Source      = 0;
        for (i = 0U ; i < ObjNum; i++) {
            pObj = &pObjBase[i];
            pBbxList->Bbx[i].Cat = (UINT16)pObj->clsId;
            pBbxList->Bbx[i].Fid = (UINT16)pObj->field;
            pBbxList->Bbx[i].H = (UINT16)pObj->bb_height_m1 + 1U;
            pBbxList->Bbx[i].ObjectId = pObj->track;
            pBbxList->Bbx[i].Score = pObj->score;
            pBbxList->Bbx[i].W = (UINT16)pObj->bb_width_m1 + 1U;
            pBbxList->Bbx[i].X = (UINT16)pObj->bb_start_col;
            pBbxList->Bbx[i].Y = (UINT16)pObj->bb_start_row;

#if 0
            AmbaPrint_PrintInt5("[AmbaOD_AmbaODTrans] cat:%d x:%4d y:%4d w:%3d h:%3d",
                    (INT32)pObj->clsId,
                    pObj->bb_start_col,
                    pObj->bb_start_row,
                    (INT32)pObj->bb_width_m1,
                    (INT32)pObj->bb_height_m1);
#endif
        }

        if (SVC_OK == AmbaWrap_memcpy(&CharPtr, &pOdResult->class_name, (UINT32)sizeof(void *))) {
            pBbxList->class_name = CharPtr.pDPtr;
        } else {
            SvcLog_NG(SVC_LOG_AMBAOD, "typecast class_name failed", 0U, 0U);
        }
    } else {
        //SvcLog_NG(SVC_LOG_AMBAOD, "AmbaOD_AmbaODTrans: wrong ObjNum = %u", ObjNum, 0U);
    }
}

/**
* Attach class name string to bounding box structure
*/
static void AmbaOD_AttachClassName(const SVC_AMBAOD_CTRL_s *pCtrl, SVC_CV_DETRES_BBX_LIST_s *pBbxList)
{
    static const char *AdasClassNameV36[] = {
        "Background",
        "Car",
        "Truck",
        "Bus",
        "SUV",
        "Train",
        "Person",
        "Cyclist",
        "Green",
        "Yellow",
        "Red",
        "Sign",
        "Others",
    };
    static const char *AdasClassNameV37[] = {
        "Person",
        "Rider",
        "Bicycle",
        "Motorcycle",
        "Car",
        "Truck",
        "Red",
        "Green",
        "Sign",
        "Other",
        "Arrow",
        "SpeedBump",
        "Crosswalk",
        "StopLine",
        "YieldLine",
        "YieldMark"
    };
    static const char *EmrClassName[] = {
        "Background",
        "Vehicle",
        "Person",
        "Cyclist",
        "Others",
    };

    if ((pCtrl->AmbaODVer == AMBAOD_VER_37) || (pCtrl->AmbaODVer == (AMBAOD_VER_37|AMBAOD_VER_37_ADD_PERCEPTION))) {
        pBbxList->class_name = AdasClassNameV37;
    } else if (pCtrl->AmbaODType == AMBAOD_TYPE_FC) {
        pBbxList->class_name = AdasClassNameV36;
    } else if (pCtrl->AmbaODType == AMBAOD_TYPE_EMR) {
        pBbxList->class_name = EmrClassName;
    } else {
        pBbxList->class_name = AdasClassNameV36;
    }
}

/**
* Attach class name string to perception data structure
*/
static void AmbaOD_AttachClfClassName(const SVC_AMBAOD_CTRL_s *pCtrl, SVC_CV_PERCEPTION_DATA_s *pPcptData)
{
    static const char *TSClassName[] = {
        "TS-SpeedLimit",
        "TS-Stop",
        "TS-Yield",
        "TS-NoTurns",
        "TS-Exclusionary",
        "TS-Construction",
        "TS-Backside",
        "TS-other", /* other */
        "TS-BG",    /* background */
    };
    static const char *SLClassName[] = {
        "SL<5>",
        "SL<10>",
        "SL<15>",
        "SL<20>",
        "SL<25>",
        "SL<30>",
        "SL<35>",
        "SL<40>",
        "SL<45>",
        "SL<50>",
        "SL<55>",
        "SL<60>",
        "SL<65>",
        "SL<70>",
        "SL<80>",
        "SL<90>",
        "SL<100>",
        "SL<110>",
        "SL<120>",
    };
    static const char *TLSClassName[] = {
        "Circle",
        "Left",
        "Right",
        "Up",
        "Down",
        "UpRight",
        "NorthEast",
        "NorthWest",
        "Pedestrian",
        "Bicycle",
        "TLS-other", /* other */
        "TLS-BG",    /* background */
    };
    static const char *ARClassName[] = {
        "Stright",
        "Left",
        "Right",
        "StrightLeft",
        "StrightRight",
        "LeftRight",
        "StrightLeftRight",
        "AR-other", /* other */
        "AR-BG", /* background */
    };

    #define NUM_CLASS_TBL_ENTRY     5U
    static void *ClsMap[NUM_CLASS_TBL_ENTRY] = {
        TSClassName,    /* AMBANET_OD37_TS */
        SLClassName,    /* AMBANET_OD37_SL */
        TLSClassName,   /* AMBANET_OD37_TLC */
        TLSClassName,   /* AMBANET_OD37_TLS */
        ARClassName,    /* AMBANET_OD37_AR */
    };

    const void *pVoidPtr;
    CHAR_PTR_s CharPtr;
    UINT8 Index;

    if ((pPcptData->PcptType >= AMBANET_OD37_TS) && (pPcptData->PcptType <= AMBANET_OD37_AR)) {
        Index = pPcptData->PcptType - AMBANET_OD37_TS;
        pVoidPtr = ClsMap[Index];
        if (SVC_OK == AmbaWrap_memcpy(&CharPtr, &pVoidPtr, (UINT32)sizeof(void *))) {
            pPcptData->class_name = CharPtr.pDPtr;
        } else {
            SvcLog_NG(SVC_LOG_AMBAOD, "typecast class_name failed", 0U, 0U);
        }
    }  else {
        SvcLog_NG(SVC_LOG_AMBAOD, "AmbaOD_AttachClfClassName failed. PcptType = %u", pPcptData->PcptType, 0U);
    }

    AmbaMisra_TouchUnused(&pCtrl);
    AmbaMisra_TouchUnused(&TSClassName);
    AmbaMisra_TouchUnused(&SLClassName);
    AmbaMisra_TouchUnused(&TLSClassName);
    AmbaMisra_TouchUnused(&ARClassName);
    AmbaMisra_TouchUnused(&ClsMap);
}

/**
* Algorithm output event callback function
*/
static UINT32 AmbaOD_AlgoOutCallback(UINT32 Inst, UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Rval = SVC_OK;
    SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[Inst];
    AMBAOD_OUTPUT_MSG_s Msg;
    UINT32 Send = 0U;
    UINT32 TargetQue = 0U;
    AMBAOD_INTER_DATA_s *pInterData = NULL;

    /*  @
        CALLBACK_EVENT_OUTPUT: 1st stage done
        CALLBACK_EVENT_FREE_INPUT: Feed to 2nd stage done
        CALLBACK_EVENT_XX_OUTPUT: 2nd stage done
    */
    if (Event == CALLBACK_EVENT_OUTPUT) {
        if (pEventData->pUserData != NULL) {
            AmbaMisra_TypeCast(&pInterData, &(pEventData->pUserData));
        }
        if (pInterData != NULL) {
            SvcCvFlow_GetTimeStamp(SVC_LOG_AMBAOD, &pInterData->TimeStamp[SVC_CV_FLOW_TIME_STAMP1]);
            SvcCvFlow_CalcFrameTime(SVC_LOG_AMBAOD, 2U, pInterData->TimeStamp, &pCtrl->FTInfo);

#if defined AMBAOD_POST_FILTER
            AmbaOD_BbxPostFilter(pCtrl, pEventData);
#endif
            AmbaOD_BbxDataPrepare(pCtrl, pInterData, pEventData);

            if (pCtrl->AmbaODVer == (AMBAOD_VER_37 | AMBAOD_VER_37_ADD_PERCEPTION)) {
                AmbaOD_PcptDataPrepare(&pInterData->PcptOut, pInterData, pEventData);

                /* Get semaphore for stage 2. Wait stage 2 available. */
                Rval = AmbaOD_SemTake(Inst, &pCtrl->OutBufSem[AMBAOD_PIPE_STAGE_2]);

                /* Pass to application for decision */
                SVC_CV_DBG(SVC_LOG_AMBAOD, "CB[%u]: IssueS(%d)", Inst, pInterData->PcptOut.pBbxList->FrameNum);
                Rval = SvcCvFlow_StatusEventIssue(pCtrl->CvfChan, SVC_CV_STATUS_AMBAOD_PCPT_PREPROC, &pInterData->PcptOut);

                /* Decide the run list */
                AmbaOD_PcptDataResolve(&pInterData->PcptOut, pEventData);
            }
        }
        TargetQue = AMBAOD_PIPE_STAGE_1;
        Send = 1U;
    } else if ((Event == CALLBACK_EVENT_KP_OUTPUT) ||
               (Event == CALLBACK_EVENT_3D_OUTPUT) ||
               (Event == CALLBACK_EVENT_MKL_OUTPUT) ||
               (Event == CALLBACK_EVENT_MKS_OUTPUT) ||
               (Event == CALLBACK_EVENT_TS_OUTPUT) ||
               (Event == CALLBACK_EVENT_SL_OUTPUT) ||
               (Event == CALLBACK_EVENT_TLC_OUTPUT) ||
               (Event == CALLBACK_EVENT_TLS_OUTPUT) ||
               (Event == CALLBACK_EVENT_AR_OUTPUT)) {
        TargetQue = AMBAOD_PIPE_STAGE_2;
        Send = 1U;
    } else if (Event == CALLBACK_EVENT_FREE_INPUT) {
        /* Release stage 1 */
        Rval = AmbaOD_SemGive(Inst, &pCtrl->OutBufSem[AMBAOD_PIPE_STAGE_1]);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_AMBAOD, "Rx[%u]: SemGive error", Inst, 0U);
        }
        if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
            AmbaOD_SendReceiverReady(Inst, &pCtrl->FeederPort);
        }
    } else {
        /* Do nothing */
    }

    if (Send == 1U) {
        Msg.Event = Event;
        if (NULL != pEventData) {
            Rval = AmbaWrap_memcpy(&Msg.AlgoOut, pEventData, sizeof(SVC_CV_ALGO_OUTPUT_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_AMBAOD, "memcpy AlgoOut failed", 0U, 0U);
            }
        }
        Rval = AmbaKAL_MsgQueueSend(&pCtrl->AlgoOutQueId[TargetQue], &Msg, 5000);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_AMBAOD, "AmbaOD_RxAlgoOutCallback[%u]: MsgQueueSend[%u] error", Inst, TargetQue);
        }
    }

    return Rval;
}

static UINT32 AmbaOD_AlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return AmbaOD_AlgoOutCallback(SVC_AMBAOD_INSTANCE0, Event, pEventData);
}

static UINT32 AmbaOD_AlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return AmbaOD_AlgoOutCallback(SVC_AMBAOD_INSTANCE1, Event, pEventData);
}

static UINT32 AmbaOD_AlgoOutCallback2(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return AmbaOD_AlgoOutCallback(SVC_AMBAOD_INSTANCE2, Event, pEventData);
}

static UINT32 AmbaOD_AlgoOutCallback3(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return AmbaOD_AlgoOutCallback(SVC_AMBAOD_INSTANCE3, Event, pEventData);
}

#if defined AMBAOD_POST_FILTER
/* Supress double detected object for 2 scale */
static void AmbaOD_BbxPostFilter(const SVC_AMBAOD_CTRL_s *pCtrl, const SVC_CV_ALGO_OUTPUT_s *pAlgoOut)
{
    amba_od_out_t *pAmbaOdOut;

    if (pCtrl->NumODScale == 2U) {
        if ((pCtrl->AmbaODVer == AMBAOD_VER_37) ||
            (pCtrl->AmbaODVer == (AMBAOD_VER_37 | AMBAOD_VER_37_ADD_PERCEPTION))) {
            #define NUM_FILTER_CLASS  3U
            uint32_t ClassArr[NUM_FILTER_CLASS] = {
                AMBANET_OD37_FC_CAT4_CAR,
                AMBANET_OD37_FC_CAT5_TRUCK,
                AMBANET_OD37_FC_CAT14_YIELDLINE,
            };
            UINT32 i;
            extern void AmbaOD_FilterDoubleDetect(amba_od_out_t *od_out, uint32_t num_class, uint32_t *class, uint32_t th, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

            for (i = 0U; i < pCtrl->NumODScale; i++) {
                if (pCtrl->pCvFlow->InputCfg.Input[0U].RoiInfo.Roi[i].Index == 0U) {
                    AmbaMisra_TypeCast(&pAmbaOdOut, &pAlgoOut->pOutput->buf[0].pBuffer);
                    AmbaOD_FilterDoubleDetect(pAmbaOdOut,
                                              NUM_FILTER_CLASS, ClassArr,
                                              32,
                                              pCtrl->pCvFlow->InputCfg.Input[0U].RoiInfo.Roi[i].StartX,
                                              pCtrl->pCvFlow->InputCfg.Input[0U].RoiInfo.Roi[i].StartY,
                                              1280, 640);
                    break;
                }
            }

            if (i == pCtrl->NumODScale) {
                /* No scale 0 found */
                SvcLog_DBG(SVC_LOG_AMBAOD, "Bypass AmbaOD_FilterDoubleDetect", 0U, 0U);
            }
        } else {
            /* Do nothing */
        }
    }
}
#endif

static void AmbaOD_BbxDataPrepare(const SVC_AMBAOD_CTRL_s *pCtrl, AMBAOD_INTER_DATA_s *pInterData, const SVC_CV_ALGO_OUTPUT_s *pAlgoOut)
{
    const amba_od_out_t *pAmbaOdOut;

    AmbaMisra_TypeCast(&pAmbaOdOut, &pAlgoOut->pOutput->buf[0].pBuffer);

    AmbaOD_AmbaODTrans(pCtrl, pInterData->pPicInfo, pAmbaOdOut, &pInterData->BbxList);
    AmbaOD_AttachClassName(pCtrl, &pInterData->BbxList);

    pInterData->BbxList.MsgCode = SVC_CV_DETRES_BBX;
    pInterData->BbxList.Source  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
    AmbaMisra_TypeCast(&(pInterData->BbxList.pCvOut), &pAmbaOdOut);
}

/* Initialize the SVC_CV_PERCEPTION_OUTPUT_s */
static void AmbaOD_PcptDataPrepare(SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut, AMBAOD_INTER_DATA_s *pInterData, const SVC_CV_ALGO_OUTPUT_s *pAlgoOut)
{
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
    UINT32 Rval = SVC_OK;
    AmbaPcptCfgNext *pNext;

    AmbaMisra_TouchUnused(&pPcptOut);
    AmbaMisra_TouchUnused(&pInterData);

    Rval = AmbaWrap_memset(pPcptOut, 0, sizeof(SVC_CV_PERCEPTION_OUTPUT_s));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_AMBAOD, "memset pPcptOut failed", 0U, 0U);
    }

    /* Fill in PcptOut */
    pPcptOut->pBbxList = &pInterData->BbxList;
    pPcptOut->PcptBits = 0U;
    pPcptOut->NumPcpt = 0U;
    AmbaMisra_TypeCast(&pNext, &(pAlgoOut->pExtOutput));

    if ((pNext->EnableKP & AMBAOD_PERCEPTION_RUN) > 0U) {
        pPcptOut->PcptBits |= ((UINT32)1U << AMBANET_OD37_KP);
        pPcptOut->PcptData[AMBANET_OD37_KP].PcptType = AMBANET_OD37_KP;
        pPcptOut->PcptData[AMBANET_OD37_KP].Enable = 1U;
        pPcptOut->PcptData[AMBANET_OD37_KP].MaxObj = AMBANET_ODV37_KP_BATCH_SZ;
        pPcptOut->PcptData[AMBANET_OD37_KP].pObjIdxList = pNext->RunObjsKP;
    }

    if ((pNext->Enable3D & AMBAOD_PERCEPTION_RUN) > 0U) {
        pPcptOut->PcptBits |= ((UINT32)1U << AMBANET_OD37_3D);
        pPcptOut->PcptData[AMBANET_OD37_3D].PcptType = AMBANET_OD37_3D;
        pPcptOut->PcptData[AMBANET_OD37_3D].Enable = 1U;
        pPcptOut->PcptData[AMBANET_OD37_3D].MaxObj = AMBANET_ODV37_3D_BATCH_SZ;
        pPcptOut->PcptData[AMBANET_OD37_3D].pObjIdxList = pNext->RunObjs3D;
    }

    if ((pNext->EnableMask & AMBAOD_PERCEPTION_RUN) > 0U) {
        pPcptOut->PcptBits |= ((UINT32)1U << AMBANET_OD37_MKL);
        pPcptOut->PcptData[AMBANET_OD37_MKL].PcptType = AMBANET_OD37_MKL;
        pPcptOut->PcptData[AMBANET_OD37_MKL].Enable = 1U;
        pPcptOut->PcptData[AMBANET_OD37_MKL].MaxObj = AMBANET_ODV37_MK_BATCH_SZ;
        pPcptOut->PcptData[AMBANET_OD37_MKL].pObjIdxList = pNext->RunObjsMK;

        pPcptOut->PcptBits |= ((UINT32)1U << AMBANET_OD37_MKS);
        pPcptOut->PcptData[AMBANET_OD37_MKS].PcptType = AMBANET_OD37_MKS;
        pPcptOut->PcptData[AMBANET_OD37_MKS].Enable = 1U;
        pPcptOut->PcptData[AMBANET_OD37_MKS].MaxObj = AMBANET_ODV37_MK_BATCH_SZ;
        pPcptOut->PcptData[AMBANET_OD37_MKS].pObjIdxList = pNext->RunObjsMK;
    }

    if ((pNext->EnableTS & AMBAOD_PERCEPTION_RUN) > 0U) {
        pPcptOut->PcptBits |= ((UINT32)1U << AMBANET_OD37_TS);
        pPcptOut->PcptData[AMBANET_OD37_TS].PcptType = AMBANET_OD37_TS;
        pPcptOut->PcptData[AMBANET_OD37_TS].Enable = 1U;
        pPcptOut->PcptData[AMBANET_OD37_TS].MaxObj = AMBANET_ODV37_TS_BATCH_SZ;
        pPcptOut->PcptData[AMBANET_OD37_TS].pObjIdxList = pNext->RunObjsTS;

        pPcptOut->PcptBits |= ((UINT32)1U << AMBANET_OD37_SL);
        pPcptOut->PcptData[AMBANET_OD37_SL].PcptType = AMBANET_OD37_SL;
        pPcptOut->PcptData[AMBANET_OD37_SL].Enable = 1U;
        pPcptOut->PcptData[AMBANET_OD37_SL].MaxObj = AMBANET_ODV37_TS_BATCH_SZ;
        pPcptOut->PcptData[AMBANET_OD37_SL].pObjIdxList = pNext->RunObjsTS;
    }

    if ((pNext->EnableTLC & AMBAOD_PERCEPTION_RUN) > 0U) {
        pPcptOut->PcptBits |= ((UINT32)1U << AMBANET_OD37_TLC);
        pPcptOut->PcptData[AMBANET_OD37_TLC].PcptType = AMBANET_OD37_TLC;
        pPcptOut->PcptData[AMBANET_OD37_TLC].Enable = 1U;
        pPcptOut->PcptData[AMBANET_OD37_TLC].MaxObj = AMBANET_ODV37_TL_BATCH_SZ;
        pPcptOut->PcptData[AMBANET_OD37_TLC].pObjIdxList = pNext->RunObjsTL;
    }

    if ((pNext->EnableTLS & AMBAOD_PERCEPTION_RUN) > 0U) {
        pPcptOut->PcptBits |= ((UINT32)1U << AMBANET_OD37_TLS);
        pPcptOut->PcptData[AMBANET_OD37_TLS].PcptType = AMBANET_OD37_TLS;
        pPcptOut->PcptData[AMBANET_OD37_TLS].Enable = 1U;
        pPcptOut->PcptData[AMBANET_OD37_TLS].MaxObj = AMBANET_ODV37_TL_BATCH_SZ;
        pPcptOut->PcptData[AMBANET_OD37_TLS].pObjIdxList = pNext->RunObjsTL;
    }

    if ((pNext->EnableAR & AMBAOD_PERCEPTION_RUN) > 0U) {
        pPcptOut->PcptBits |= ((UINT32)1U << AMBANET_OD37_AR);
        pPcptOut->PcptData[AMBANET_OD37_AR].PcptType = AMBANET_OD37_AR;
        pPcptOut->PcptData[AMBANET_OD37_AR].Enable = 1U;
        pPcptOut->PcptData[AMBANET_OD37_AR].MaxObj = AMBANET_ODV37_AR_BATCH_SZ;
        pPcptOut->PcptData[AMBANET_OD37_AR].pObjIdxList = pNext->RunObjsAR;
    }

#else
    AmbaMisra_TouchUnused(pPcptOut);
    AmbaMisra_TouchUnused(pInterData);
    AmbaMisra_TouchUnused(&pAlgoOut);
    SvcLog_OK(SVC_LOG_AMBAOD, "AmbaOD_PcptDataPrepare do nothing", 0U, 0U);
#endif
}

/* Decide the object index list for the next stage. */
/* If any object lists are updated already, they will not be touched. */
static void AmbaOD_PcptDataResolve(SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut, const SVC_CV_ALGO_OUTPUT_s *pAlgoOut)
{
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
    UINT32 Rval;
    UINT32 i;
    UINT32 Count[SVC_AMBAOD_MAX_PCPT_PROC] = {0};
    UINT32 ObjNum;
    ULONG  ObjAddr;
    const amba_od_out_t *pOdResult;
    const amba_od_candidate_t *pObjBase, *pObj;
    AmbaPcptCfgNext *pNext;
    SVC_CV_PERCEPTION_DATA_s *pPcptData;
    UINT32 ProcCount = 0;

    AmbaMisra_TouchUnused(&pPcptOut);
    AmbaMisra_TypeCast(&pOdResult, &(pAlgoOut->pOutput->buf[0].pBuffer));
    AmbaMisra_TypeCast(&pNext, &(pAlgoOut->pExtOutput));

    AmbaMisra_TypeCast(&ObjAddr, &pOdResult);
    ObjAddr = ObjAddr + pOdResult->objects_offset;
    AmbaMisra_TypeCast(&pObjBase, &ObjAddr);
    ObjNum = pOdResult->num_objects;

    //AmbaPrint_PrintUInt5("[PcptBbx] total:%d", ObjNum, 0U, 0U, 0U, 0U);

    Rval = AmbaWrap_memset(Count, 0, sizeof(Count));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_AMBAOD, "memset Count failed", 0U, 0U);
    }

    /* Scan all objects and fill in the run list */
    for (i = 0U ; i < ObjNum; i++) {
        pObj = &pObjBase[i];

        /*AmbaPrint_PrintUInt5("[PcptBbx] C:%d W:%d H:%d X:%d Y:%d",
            pObj->clsId,
            pObj->bb_width_m1,
            pObj->bb_height_m1,
            pObj->bb_start_col,
            pObj->bb_start_row);*/

        if (pObj->score == 0U) {
            continue;
        }

        /* Prepare the list for sub networks (KP, 3D, MK). */
        if ((pNext->EnableKP & AMBAOD_PERCEPTION_RUN) > 0U) {
            #define SVC_AMBAOD_ALGO_MAX_KP_OBJ      (12U)
            pPcptData = &pPcptOut->PcptData[AMBANET_OD37_KP];
            if (pPcptData->ListUpdate == 0U) {
                if (Count[AMBANET_OD37_KP] < SVC_AMBAOD_ALGO_MAX_KP_OBJ) {
                    if ((pObj->clsId == AMBANET_OD37_FC_CAT0_PERSON) ||
                        (pObj->clsId == AMBANET_OD37_FC_CAT1_RIDER)) {
                        pNext->RunObjsKP[Count[AMBANET_OD37_KP]] = (UINT8) i;
                        Count[AMBANET_OD37_KP]++;
                    }
                }
            }
        }

        if ((pNext->Enable3D & AMBAOD_PERCEPTION_RUN) > 0U) {
            pPcptData = &pPcptOut->PcptData[AMBANET_OD37_3D];
            if (pPcptData->ListUpdate == 0U) {
                if (Count[AMBANET_OD37_3D] < pPcptData->MaxObj) {
                    if ((pObj->clsId == AMBANET_OD37_FC_CAT2_BICYCLE) ||
                        (pObj->clsId == AMBANET_OD37_FC_CAT3_MOTORCYCLE) ||
                        (pObj->clsId == AMBANET_OD37_FC_CAT4_CAR)||
                        (pObj->clsId == AMBANET_OD37_FC_CAT5_TRUCK)) {
                        pNext->RunObjs3D[Count[AMBANET_OD37_3D]] = (UINT8) i;
                        Count[AMBANET_OD37_3D]++;
                    }
                }
            }
        }

        if ((pNext->EnableMask & AMBAOD_PERCEPTION_RUN) > 0U) {
            pPcptData = &pPcptOut->PcptData[AMBANET_OD37_MKL];
            if (pPcptData->ListUpdate == 0U) {
                if (Count[AMBANET_OD37_MKL] < pPcptData->MaxObj) {
                    if (pObj->clsId <= AMBANET_OD37_FC_CAT5_TRUCK) {
                        pNext->RunObjsMK[Count[AMBANET_OD37_MKL]] = (UINT8) i;
                        Count[AMBANET_OD37_MKL]++;
                        Count[AMBANET_OD37_MKS]++;
                    }
                }
            }
        }

        if ((pNext->EnableTS & AMBAOD_PERCEPTION_RUN) > 0U) {
            pPcptData = &pPcptOut->PcptData[AMBANET_OD37_TS];
            if (pPcptData->ListUpdate == 0U) {
                if (Count[AMBANET_OD37_TS] < pPcptData->MaxObj) {
                    if (pObj->clsId == AMBANET_OD37_FC_CAT8_SIGN) {
                        pNext->RunObjsTS[Count[AMBANET_OD37_TS]] = (UINT8) i;
                        Count[AMBANET_OD37_TS]++;
                        Count[AMBANET_OD37_SL]++;
                    }
                }
            }
        }

        if ((pNext->EnableTLS & AMBAOD_PERCEPTION_RUN) > 0U) {
            pPcptData = &pPcptOut->PcptData[AMBANET_OD37_TLS];
            if (pPcptData->ListUpdate == 0U) {
                if (Count[AMBANET_OD37_TLS] < pPcptData->MaxObj) {
                    if ((pObj->clsId == AMBANET_OD37_FC_CAT6_RED) ||
                        (pObj->clsId == AMBANET_OD37_FC_CAT7_GREEN)) {
                        pNext->RunObjsTL[Count[AMBANET_OD37_TLS]] = (UINT8) i;
                        Count[AMBANET_OD37_TLS]++;
                    }
                }
            }
        }

        if ((pNext->EnableAR & AMBAOD_PERCEPTION_RUN) > 0U) {
            pPcptData = &pPcptOut->PcptData[AMBANET_OD37_AR];
            if (pPcptData->ListUpdate == 0U) {
                if (Count[AMBANET_OD37_AR] < pPcptData->MaxObj) {
                    if (pObj->clsId == AMBANET_OD37_FC_CAT10_ARROW) {
                        pNext->RunObjsAR[Count[AMBANET_OD37_AR]] = (UINT8) i;
                        Count[AMBANET_OD37_AR]++;
                    }
                }
            }
        }

    }

    /* Check number of 2nd stage process */
    for (i = 0U ; i < SVC_CV_MAX_PCPT_PROC; i++) {
        pPcptData = &pPcptOut->PcptData[i];
        if (pPcptData->Enable == 1U) {
            if (pPcptData->ListUpdate == 0U) {
                if (Count[i] < pPcptData->MaxObj) {
                    /* Use 0xFF to terminate if the list is not full. */
                    /* pObjIdxList pointer to RunObjsXX list */
                    pPcptData->pObjIdxList[Count[i]] = 0xFF;
                }
                pPcptData->NumObj = (UINT8)Count[i];
                pPcptData->ListUpdate = 1U;
            }

            if (pPcptData->NumObj > 0U) {
                ProcCount++;
            }
        }
    }

    pPcptOut->NumPcpt = ProcCount;

    if (SvcCvDebugEnable > 1U) {
        AmbaPrint_PrintUInt5("[AmbaOD] PcptBits:0x%x, NumPcpt:%u",
                              pPcptOut->PcptBits, pPcptOut->NumPcpt, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("[AmbaOD] Enable: KP(%u) 3D(%u) Mask(%u)",
                              pNext->EnableKP, pNext->Enable3D, pNext->EnableMask, 0U, 0U);
        AmbaPrint_PrintUInt5("[AmbaOD] Enable: TS(%u) SL(%u) TLC(%u) TLS(%u) AR(%u)",
                              pNext->EnableTS, pNext->EnableSL, pNext->EnableTLC, pNext->EnableTLS, pNext->EnableAR);
        AmbaPrint_PrintUInt5("[AmbaOD] Count: Kp(%u) 3D(%u) Ms(%u)",
                              Count[AMBANET_OD37_KP], Count[AMBANET_OD37_3D], Count[AMBANET_OD37_MKL], 0U, 0U);
        AmbaPrint_PrintUInt5("[AmbaOD] Count:  Ts(%u) SL(%u) TLC(%u) TLS(%u) AR(%u)",
                              Count[AMBANET_OD37_TS], Count[AMBANET_OD37_SL], Count[AMBANET_OD37_TLC], Count[AMBANET_OD37_TLS], Count[AMBANET_OD37_AR]);

        if ((pNext->EnableKP & AMBAOD_PERCEPTION_RUN) > 0U) {
            for (i = 0; i < Count[AMBANET_OD37_KP]; i++) {
                AmbaPrint_PrintUInt5("RunObjsKP[%d] = %u", i, pNext->RunObjsKP[i], 0U, 0U, 0U);
                if (pNext->RunObjsKP[i] == 0xffU) {
                    break;
                }
            }
        }

        if ((pNext->Enable3D & AMBAOD_PERCEPTION_RUN) > 0U) {
            for (i = 0; i < Count[AMBANET_OD37_3D]; i++) {
                AmbaPrint_PrintUInt5("RunObjs3D[%d] = %u", i, pNext->RunObjs3D[i], 0U, 0U, 0U);
                if (pNext->RunObjs3D[i] == 0xffU) {
                    break;
                }
            }
        }

        if ((pNext->EnableMask & AMBAOD_PERCEPTION_RUN) > 0U) {
            for (i = 0; i < Count[AMBANET_OD37_MKL]; i++) {
                AmbaPrint_PrintUInt5("RunObjsMK[%u] = %u", i, pNext->RunObjsMK[i], 0U, 0U, 0U);
                if (pNext->RunObjsMK[i] == 0xffU) {
                    break;
                }
            }
        }

        if ((pNext->EnableTS & AMBAOD_PERCEPTION_RUN) > 0U) {
            for (i = 0; i < Count[AMBANET_OD37_TS]; i++) {
                AmbaPrint_PrintUInt5("RunObjsTS[%u] = %u", i, pNext->RunObjsTS[i], 0U, 0U, 0U);
                if (pNext->RunObjsTS[i] == 0xffU) {
                    break;
                }
            }
        }

        if ((pNext->EnableAR & AMBAOD_PERCEPTION_RUN) > 0U) {
            for (i = 0; i < Count[AMBANET_OD37_AR]; i++) {
                AmbaPrint_PrintUInt5("RunObjsAR[%u] = %u", i, pNext->RunObjsAR[i], 0U, 0U, 0U);
                if (pNext->RunObjsAR[i] == 0xffU) {
                    break;
                }
            }
        }
    }

#else
    AmbaMisra_TouchUnused(pPcptOut);
    AmbaMisra_TouchUnused(&pAlgoOut);
    SvcLog_OK(SVC_LOG_AMBAOD, "AmbaOD_PcptDataResolve do nothing", 0U, 0U);
#endif
}

#if 0
static void AmbaOD_DrawLine(UINT32 X1, UINT32 Y1, UINT32 X2, UINT32 Y2, UINT32 Color)
{
    #define X_NOW   1920
    #define Y_NOW   1080
    #define X_NEW   960
    #define Y_NEW   540

    X1 = (X1 * X_NEW) / X_NOW;
    X2 = (X2 * X_NEW) / X_NOW;
    Y1 = (Y1 * Y_NEW) / Y_NOW;
    Y2 = (Y2 * Y_NEW) / Y_NOW;
    SvcOsd_DrawLine(1U, X1, Y1, X2, Y2, 2U, Color);
}
#endif

static void AmbaOD_DumpPcptKpResult(const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    const amba_kp_out_t *pKPOut;
    const amba_kp_candidate_t *pKPObj;
    ULONG ObjAddr;

    AmbaMisra_TypeCast(&pKPOut, &pEventData->pExtOutput);
    AmbaMisra_TypeCast(&ObjAddr, &pKPOut);
    ObjAddr = ObjAddr + pKPOut->objects_offset;
    AmbaMisra_TypeCast(&pKPObj, &ObjAddr);

  //  AmbaPrint_PrintUInt5("Receive KP total: %d", pKPOut->num_objects, 0U, 0U, 0U, 0U);

#if 0
    {
        UINT32 i;
        const UINT16 *pX, *pY;

        for (i = 0; i < pKPOut->num_objects; i++) {
            pX = &pKPObj[i].coord[0];
            pY = &pKPObj[i].coord[32];

            AmbaPrint_PrintUInt5("KP[%d]: (%d, %d) (%d, %d)", i, pX[0], pY[0], pX[1], pX[1]);
        }
    }
#endif

#if 0
    {
        UINT16 *pX, *pY;
        UINT32 i;
        UINT32 Color = 1;
        UINT32 MidX, MidY;
        UINT32 MidX2, MidY2;

        SvcOsd_Clear(1U);

        for (i = 0; i < pKPOut->num_objects; i++) {
            pX = &pKPObj[i].coord[0];
            pY = &pKPObj[i].coord[32];

            MidX = (pX[5] + pX[6]) >> 1U;
            MidY = (pY[5] + pY[6]) >> 1U;
            AmbaOD_DrawLine(pX[0], pY[0], MidX, MidY, Color);

            MidX2 = (pX[11] + pX[12]) >> 1U;
            MidY2 = (pY[11] + pY[12]) >> 1U;
            AmbaOD_DrawLine(MidX,  MidY, MidX2, MidY2, Color);

            MidX = (pX[1] + pX[2]) >> 1U;
            MidY = (pY[1] + pY[2]) >> 1U;
            AmbaOD_DrawLine(pX[0], pY[0], MidX, MidY, Color);

            MidX = (pX[2] + pX[3]) >> 1U;
            MidY = (pY[2] + pY[3]) >> 1U;
            AmbaOD_DrawLine(pX[1], pY[1], MidX, MidY, Color);

            AmbaOD_DrawLine(pX[2], pY[2], pX[4], pY[4], Color);

            MidX = (pX[6] + pX[7]) >> 1U;
            MidY = (pY[6] + pY[7]) >> 1U;
            AmbaOD_DrawLine(pX[5], pY[5], MidX, MidY, Color);

            AmbaOD_DrawLine(pX[6], pY[6], pX[8], pY[8], Color);

            AmbaOD_DrawLine(pX[7], pY[7], pX[9], pY[9], Color);

            AmbaOD_DrawLine(pX[8], pY[8], pX[10], pY[10], Color);

            MidX = (pX[12] + pX[13]) >> 1U;
            MidY = (pY[12] + pY[13]) >> 1U;
            AmbaOD_DrawLine(pX[11], pY[11], MidX, MidY, Color);

            AmbaOD_DrawLine(pX[12], pY[12], pX[14], pY[14], Color);

            AmbaOD_DrawLine(pX[13], pY[13], pX[15], pY[15], Color);

            AmbaOD_DrawLine(pX[14], pY[14], pX[16], pY[16], Color);

            Color++;
        }

        SvcOsd_Flush(1U);
    }
#endif
}

static void AmbaOD_DumpPcpt3DResult(const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    const amba_odv37_step8_out_t *p3DOut;
    const obj_t *p3DObj;
    ULONG ObjAddr;

    AmbaMisra_TypeCast(&p3DOut, &pEventData->pExtOutput);
    AmbaMisra_TypeCast(&ObjAddr, &p3DOut);
    ObjAddr = ObjAddr + p3DOut->objects_offset;
    AmbaMisra_TypeCast(&p3DObj, &ObjAddr);

  //  AmbaPrint_PrintUInt5("Receive 3D total: %d", p3DOut->num_objects, 0U, 0U, 0U, 0U);
}

static void AmbaOD_DumpPcptMaskResult(const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    const amba_is_out_t *pMKOut;
    const UINT32 *pAddr;
    const UINT16 *pW, *pH;
    ULONG TmpAddr, Addr;
    static UINT8 SvcCvDebugDump = 0;

    AmbaMisra_TypeCast(&pMKOut, &pEventData->pExtOutput);
    AmbaMisra_TypeCast(&Addr, &pMKOut);
    TmpAddr = Addr + pMKOut->list_offset_width;
    AmbaMisra_TypeCast(&pW, &TmpAddr);
    TmpAddr = Addr + pMKOut->list_offset_height;
    AmbaMisra_TypeCast(&pH, &TmpAddr);
    TmpAddr = Addr + pMKOut->list_offset_address;
    AmbaMisra_TypeCast(&pAddr, &TmpAddr);

    //AmbaPrint_PrintUInt5("Receive MK total: %d", pMKOut->num_objects, 0U, 0U, 0U, 0U);
    //for (UINT32 i = 0; i < pMKOut->num_objects; i++) {
    //    AmbaPrint_PrintUInt5("MK[%d]: 0x%x (%d, %d, P: %d)", i, (UINT32)pAddr[i], pW[i], pH[i], GetAlignedValU32(pW[i], 32));
    //}

    if (SvcCvDebugDump != 0U) {
        extern void SvcCvFlow_SaveBin(const char *pModuleName, const char *pFileName, void *pBuf, const UINT32 Size);
        #define NUM_DUMP_BUF     4U
        static UINT8 Count = 0;
        SVC_CV_FLOW_BUF_INFO_s BufInfo = {0};
        char pDumpFile[] = "c:\\mask_dump_n.bin";
        UINT32 Picth, DataSize;
        const UINT8 *pMapData;
        UINT8 *pDstMem;
        UINT8 Idx = 0U;

        if (Count < NUM_DUMP_BUF) {
            Picth = GetAlignedValU32(pW[Idx], 32U);

            AmbaPrint_PrintUInt5("Receive MK total: %d", pMKOut->num_objects, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("MK[%u]: 0x%x (%u, %u, P: %u)",
                Idx, (UINT32)pAddr[Idx], pW[Idx], pH[Idx], Picth);

            DataSize = Picth * pH[Idx];
            if (SVC_OK != SvcCvFlow_AllocWorkBuf(DataSize, &BufInfo)) {
                SvcLog_NG(SVC_LOG_AMBAOD, "Alloc mem for mask failed", 0U, 0U);
            }

            AmbaMisra_TypeCast(&pMapData, &pAddr[Idx]);
            AmbaMisra_TypeCast(&pDstMem, &BufInfo.pAddr);

            /* Multiply by 32 for visualization */
            for (UINT32 i = 0; i < DataSize; i++) {
                pDstMem[i] = (UINT8)(pMapData[i] * 32U);
            }

            pDumpFile[13] = '0' + Count;
            SvcCvFlow_SaveBin(SVC_LOG_AMBAOD, pDumpFile, pDstMem, DataSize);

            AmbaPrint_PrintStr5("Dump %s done", pDumpFile, NULL, NULL, NULL, NULL);

            Count++;
        }
    }
}

static void AmbaOD_DumpPcptClfResult(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData, SVC_CV_PERCEPTION_DATA_s *pPcptData)
{
    const UINT8 *pTS = NULL;
    const UINT8 *pTop1 = NULL;
    const AmbaPcptCfgNext *pNext;
    ULONG Addr;

    AmbaMisra_TypeCast(&pNext, &(pEventData->pExtOutput));

    switch (Event) {
    case CALLBACK_EVENT_TS_OUTPUT:
        AmbaMisra_TypeCast(&pTop1, &(pNext->pTSOut->buf[0].pBuffer));
        break;
    case CALLBACK_EVENT_SL_OUTPUT:
        AmbaMisra_TypeCast(&pTS, &(pNext->pTSOut->buf[0].pBuffer));
        AmbaMisra_TypeCast(&Addr, &pTS);
        Addr = Addr + GetAlignedValU32(AMBANET_ODV37_TS_BATCH_SZ, 32U);
        AmbaMisra_TypeCast(&pTop1, &Addr);
        break;
    case CALLBACK_EVENT_TLC_OUTPUT:
        AmbaMisra_TypeCast(&pTop1, &(pNext->pTLCOut->buf[0].pBuffer));
        break;
    case CALLBACK_EVENT_TLS_OUTPUT:
        AmbaMisra_TypeCast(&pTop1, &(pNext->pTLSOut->buf[0].pBuffer));
        break;
    case CALLBACK_EVENT_AR_OUTPUT:
        AmbaMisra_TypeCast(&pTop1, &(pNext->pAROut->buf[0].pBuffer));
        break;
    default:
        AmbaPrint_PrintUInt5("Unknown type: 0x%x", Event, 0U, 0U, 0U, 0U);
        break;
    }

    AmbaMisra_TouchUnused(pPcptData);

#if 0
    {
        AmbaPrint_PrintUInt5("Clf type[%u]", pPcptData->PcptType, 0U, 0U, 0U, 0U);
        for (UINT32 i = 0U; i < pPcptData->NumObj; i++) {
            AmbaPrint_PrintUInt5("Clf obj[%u] = %u", pPcptData->pObjIdxList[i], (UINT32)(pTop1[i]), 0U, 0U, 0U);
        }
    }
#endif
}

/**
* RX data sink handler: process detection output
*/
static UINT32 AmbaOD_RxDataSinkHandler(UINT32 Inst)
{
    UINT32 Rval;
    SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[Inst];
    AMBAOD_OUTPUT_MSG_s Msg;
    AMBAOD_INTER_DATA_s *pInterData = NULL;

    SVC_CV_DBG(SVC_LOG_AMBAOD, "Rx[%u]: run %d", Inst, pCtrl->RxSeqNum);

    Rval = AmbaKAL_MsgQueueReceive(&pCtrl->AlgoOutQueId[AMBAOD_PIPE_STAGE_1], &Msg, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK == Rval) {
        SVC_CV_DBG(SVC_LOG_AMBAOD, "Rx[%u]: recv out(%u)", Inst, 0);

        if (Msg.Event == CALLBACK_EVENT_OUTPUT) {
            if (Msg.AlgoOut.pUserData != NULL) {
                AmbaMisra_TypeCast(&pInterData, &(Msg.AlgoOut.pUserData));
            }

            if (pInterData != NULL) {
                if (pCtrl->Enable == 1U) {
                    Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                           SVC_CV_FLOW_OUTPUT_PORT0,
                                           pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                           &pInterData->BbxList);

                    if (SVC_OK == Rval) {
                        pCtrl->SinkInfoIdx++;
                        if (pCtrl->SinkInfoIdx >= SVC_AMBAOD_OUTPUT_DEPTH) {
                            pCtrl->SinkInfoIdx = 0;
                        }
                    }
                }
            } else {
                SvcLog_NG(SVC_LOG_AMBAOD, "Rx[%u]: Invalid pInterData", Inst, 0U);
            }

            pCtrl->RxSeqNum++;
        }
    } else {
        SvcLog_NG(SVC_LOG_AMBAOD, "Rx[%u]: recv error", Inst, 0U);
    }

    SVC_CV_DBG(SVC_LOG_AMBAOD, "Rx[%u]: end", Inst, 0U);

    return SVC_OK;
}

/**
* RX data sink handler: process detection output
*/
static UINT32 AmbaOD_RxDataSinkHandler2Stages(UINT32 Inst)
{
    UINT32 Rval, i;
    SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[Inst];
    AMBAOD_OUTPUT_MSG_s Msg, PcptMsg;
    const SVC_CV_ALGO_OUTPUT_s *pPcptOut = &PcptMsg.AlgoOut;
    AMBAOD_INTER_DATA_s *pInterData = NULL;
    SVC_CV_PERCEPTION_OUTPUT_s *pPcptList = NULL;
    UINT8 EnableIssue = pCtrl->Enable;
    UINT8 Valid, PcptType = 0;
    UINT32 PcptRecvBits;
    const AmbaPcptCfgNext *pNext;
    void *pCvOut = NULL;

    SVC_CV_DBG(SVC_LOG_AMBAOD, "Rx[%u]: run %d", Inst, pCtrl->RxSeqNum);

    Rval = AmbaKAL_MsgQueueReceive(&pCtrl->AlgoOutQueId[AMBAOD_PIPE_STAGE_1], &Msg, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK == Rval) {
        SVC_CV_DBG(SVC_LOG_AMBAOD, "Rx[%u]: recv out(%u)", Inst, 0);

        if (Msg.Event == CALLBACK_EVENT_OUTPUT) {
            if (Msg.AlgoOut.pUserData != NULL) {
                AmbaMisra_TypeCast(&pInterData, &(Msg.AlgoOut.pUserData));
            }

            if (pInterData != NULL) {
                /* Perception result */
                pPcptList = &pInterData->PcptOut;
                SVC_CV_DBG(SVC_LOG_AMBAOD, "Rx[%u]: NumPcpt = %d", Inst, pPcptList->NumPcpt);

                PcptRecvBits = 0U;
                Valid = 1U;
                for (i = 0; i < pPcptList->NumPcpt; i++) {
                    Rval = AmbaKAL_MsgQueueReceive(&pCtrl->AlgoOutQueId[AMBAOD_PIPE_STAGE_2], &PcptMsg, 1000);
                    if (SVC_OK == Rval) {
                        switch (PcptMsg.Event) {
                            case CALLBACK_EVENT_KP_OUTPUT:
                                PcptType = AMBANET_OD37_KP;
                                AmbaOD_DumpPcptKpResult(pPcptOut);
                                break;
                            case CALLBACK_EVENT_3D_OUTPUT:
                                PcptType = AMBANET_OD37_3D;
                                AmbaOD_DumpPcpt3DResult(pPcptOut);
                                break;
                            case CALLBACK_EVENT_MKL_OUTPUT:
                                PcptType = AMBANET_OD37_MKL;
                                AmbaOD_DumpPcptMaskResult(pPcptOut);
                                break;
                            case CALLBACK_EVENT_MKS_OUTPUT:
                                PcptType = AMBANET_OD37_MKS;
                                AmbaOD_DumpPcptMaskResult(pPcptOut);
                                break;
                            case CALLBACK_EVENT_TS_OUTPUT:
                                PcptType = AMBANET_OD37_TS;
                                AmbaMisra_TypeCast(&pNext, &(pPcptOut->pExtOutput));
                                pCvOut = pNext->pTSOut->buf[0].pBuffer;
                                AmbaOD_DumpPcptClfResult(CALLBACK_EVENT_TS_OUTPUT, pPcptOut, &pPcptList->PcptData[PcptType]);
                                break;
                            case CALLBACK_EVENT_SL_OUTPUT:
                                PcptType = AMBANET_OD37_SL;
                                {
                                    ULONG Addr;
                                    AmbaMisra_TypeCast(&pNext, &(pPcptOut->pExtOutput));
                                    AmbaMisra_TypeCast(&Addr, &(pNext->pTSOut->buf[0].pBuffer));
                                    Addr = Addr + GetAlignedValU32(AMBANET_ODV37_TS_BATCH_SZ, 32U);
                                    AmbaMisra_TypeCast(&pCvOut, &Addr);
                                }
                                AmbaOD_DumpPcptClfResult(CALLBACK_EVENT_SL_OUTPUT, pPcptOut, &pPcptList->PcptData[PcptType]);
                                break;
                            case CALLBACK_EVENT_TLC_OUTPUT:
                                PcptType = AMBANET_OD37_TLC;
                                AmbaMisra_TypeCast(&pNext, &(pPcptOut->pExtOutput));
                                pCvOut = pNext->pTLCOut->buf[0].pBuffer;
                                AmbaOD_DumpPcptClfResult(CALLBACK_EVENT_TLC_OUTPUT, pPcptOut, &pPcptList->PcptData[PcptType]);
                                break;
                            case CALLBACK_EVENT_TLS_OUTPUT:
                                PcptType = AMBANET_OD37_TLS;
                                AmbaMisra_TypeCast(&pNext, &(pPcptOut->pExtOutput));
                                pCvOut = pNext->pTLSOut->buf[0].pBuffer;
                                AmbaOD_DumpPcptClfResult(CALLBACK_EVENT_TLS_OUTPUT, pPcptOut, &pPcptList->PcptData[PcptType]);
                                break;
                            case CALLBACK_EVENT_AR_OUTPUT:
                                PcptType = AMBANET_OD37_AR;
                                AmbaMisra_TypeCast(&pNext, &(pPcptOut->pExtOutput));
                                pCvOut = pNext->pAROut->buf[0].pBuffer;
                                AmbaOD_DumpPcptClfResult(CALLBACK_EVENT_AR_OUTPUT, pPcptOut, &pPcptList->PcptData[PcptType]);
                                break;
                            default:
                                Valid = 0U;
                                SvcLog_NG(SVC_LOG_AMBAOD, "Rx[%u]: Invalid PCPT event(0x%x)", Inst, PcptMsg.Event);
                                break;

                        }

                        if (Valid == 1U) {
                            PcptRecvBits |= ((UINT32) 1U << PcptType);

                            if (PcptType < AMBANET_OD37_TS) {
                                pPcptList->PcptData[PcptType].pCvOut = pPcptOut->pExtOutput;
                            } else if (pCvOut != NULL) {
                                pPcptList->PcptData[PcptType].pCvOut = pCvOut;
                                AmbaOD_AttachClfClassName(pCtrl, &pPcptList->PcptData[PcptType]);
                            } else {
                                SvcLog_NG(SVC_LOG_AMBAOD, "Rx[%u]: Invalid PcptType(0x%x)", Inst, PcptType);
                            }
                        }
                    } else {
                        Valid = 0U;
                        SvcLog_NG(SVC_LOG_AMBAOD, "Rx[%u]: recv Pcpt error. PcptRecvBits(0x%x)", Inst, PcptRecvBits);
                    }
                }

                if ((EnableIssue == 1U) && (Valid != 0U)) {
                    SVC_CV_DBG(SVC_LOG_AMBAOD, "Rx[%u]: Issue(%d)", Inst, pPcptList->pBbxList->FrameNum);

                    pPcptList->MsgCode = SVC_CV_PERCEPTION_OUTPUT;
                    Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                           SVC_CV_FLOW_OUTPUT_PORT0,
                                           pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                           pPcptList);

                    if (SVC_OK == Rval) {
                        pCtrl->SinkInfoIdx++;
                        if (pCtrl->SinkInfoIdx >= SVC_AMBAOD_OUTPUT_DEPTH) {
                            pCtrl->SinkInfoIdx = 0;
                        }
                    }
                }

                /* Release stage 2 */
                Rval = AmbaOD_SemGive(Inst, &pCtrl->OutBufSem[AMBAOD_PIPE_STAGE_2]);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_AMBAOD, "Rx[%u]: SemGive error", Inst, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_AMBAOD, "Rx[%u]: Invalid pInterData", Inst, 0U);
            }

            pCtrl->RxSeqNum++;
        }
    } else {
        SvcLog_NG(SVC_LOG_AMBAOD, "Rx[%u]: recv error", Inst, 0U);
    }

    SVC_CV_DBG(SVC_LOG_AMBAOD, "Rx[%u]: end", Inst, 0U);

    return SVC_OK;
}

/**
* Task entry of RX process
*/
static void* AmbaOD_RxTaskEntry(void* EntryArg)
{
    const ULONG  *pArg;
    UINT32 Rval = SVC_OK, Inst;
    const SVC_AMBAOD_CTRL_s *pCtrl;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Inst = (UINT32)(*pArg);
    pCtrl = &g_AmbaODCtrl[Inst];

    SvcLog_DBG(SVC_LOG_AMBAOD, "RxTaskEntry[%d] start", Inst, 0U);

    while (SVC_OK == Rval) {
        if (NULL != pCtrl->pRxProc) {
            Rval = pCtrl->pRxProc(Inst);
        } else {
            Rval = SVC_NG;
        }
    }

    SvcLog_DBG(SVC_LOG_AMBAOD, "RxTaskEntry[%d] exit", Inst, 0U);

    return NULL;
}

/**
* Set and wait TX and RX task to idle state
*/
static UINT32 AmbaOD_SetTaskIdle(UINT32 Inst)
{
    UINT32 Rval;
    SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[Inst];

    Rval = SvcImgFeeder_Disconnect(&pCtrl->FeederPort);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_AMBAOD, "Disconn feeder port error", 0U, 0U);
    }

    /* Disable input and output */
    pCtrl->Enable = 0U;

    return Rval;
}

/**
* Send external YUV to driver
*/
static UINT32 AmbaOD_SendExtYUV(UINT32 Inst, const void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[Inst];
    void *pInputImg; /* SVC_CV_INPUT_IMG_INFO_s */

    if (NULL != pCtrl->pTxProc) {
        AmbaMisra_TypeCast(&pInputImg, &pInfo);
        Rval = pCtrl->pTxProc(Inst, &pCtrl->FeederPort, SVC_IMG_FEEDER_IMAGE_DATA_READY, pInputImg);
    }

    return Rval;
}

/**
* Set scene mode
*/
static UINT32 AmbaOD_SetSceneMode(UINT32 Inst, UINT32 AlgoIdx, const void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const UINT32 *pSceneMode;
    SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[Inst];
    const SVC_CV_FLOW_AMBAOD_MODE_INFO_s *pModeInfo = &SvcCvFlow_AmbaOD_ModeInfo[pCtrl->pCvFlow->CvModeID];
    const SVC_CV_FLOW_AMBAOD_NMS_CFG_s *pNmsCfg;

    AmbaMisra_TypeCast(&pSceneMode, &pInfo);
    if (*pSceneMode < AMBAOD_MAX_SCENE_MODE) {
        if (pCtrl->Started == 1U) {
            pNmsCfg = pModeInfo->Cfg[AlgoIdx].pNmsCfg[*pSceneMode];
            if (pNmsCfg != NULL) {
                Rval = AmbaOD_ConfigNms(Inst, AlgoIdx, pNmsCfg);
                if (Rval == SVC_OK) {
                    pCtrl->SceneMode = (UINT8)*pSceneMode;
                }
            } else {
                SvcLog_DBG(SVC_LOG_AMBAOD, "Set scene mode(%u) NmsCfg = NULL", *pSceneMode, 0U);
            }
        } else {
            pCtrl->SceneMode = (UINT8)*pSceneMode;
        }
    } else {
        SvcLog_NG(SVC_LOG_AMBAOD, "Set scene mode(%u) error", *pSceneMode, 0U);
    }

    return Rval;
}

/**
* Entry of configuration function
*/
static UINT32 AmbaOD_Config(UINT32 Inst, UINT32 AlgoIdx)
{
    #define NUM_CVTASK_NAME     (5U)
    UINT32 Rval = SVC_OK;
    UINT32 Idx;
    SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[Inst];
    const SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &pCtrl->AlgoOp[AlgoIdx];
    const SVC_CV_FLOW_AMBAOD_MODE_INFO_s *pModeInfo = &SvcCvFlow_AmbaOD_ModeInfo[pCtrl->pCvFlow->CvModeID];
    const SVC_CV_ROI_INFO_s *pRoiCfg = &pCtrl->pCvFlow->InputCfg.Input[AlgoIdx].RoiInfo;
    const SVC_CV_FLOW_AMBAOD_NMS_CFG_s *pNmsCfg = pModeInfo->Cfg[AlgoIdx].pNmsCfg[pCtrl->SceneMode];
    const void *pFeature = pModeInfo->Cfg[AlgoIdx].pFeatureCtrl;
    AMBA_CV_FLEXIDAG_NAME_s CvTask;
    uint32_t NumFound = 0;
    uint16_t FlowIdArray[4];
    const char *TaskName[NUM_CVTASK_NAME] = { "AMBA_ODV37_STEP5", "od367a10_fc_d26_ag", "od367e2_d26_ag", "od367a10_fc_ms_d26_ag","AMBA_ODV4_STEP5" };

    CvTask.instance  = NULL;
    CvTask.algorithm = NULL;
    CvTask.step      = NULL;
    for (Idx = 0; Idx < NUM_CVTASK_NAME; Idx++) {
        AmbaMisra_TypeCast(&CvTask.cvtask, &TaskName[Idx]);
        Rval |= AmbaCV_FlexidagGetFlowIdByName(&pAlgoOp->AlgoHandle.FDs[0].Handle, CvTask, (UINT32)sizeof(FlowIdArray), FlowIdArray, &NumFound);
        if (NumFound > 0U) {
            break;
        }
    }

    if (Rval == 0U) {
        pCtrl->NumStage = 1U;
        if (Idx == 0U) {
            pCtrl->AmbaODVer = AMBAOD_VER_37;
            if (pModeInfo->AlgoGrp.AlgoInfo[AlgoIdx].NumFlexiBin > 1U) {
                pCtrl->AmbaODVer |= AMBAOD_VER_37_ADD_PERCEPTION;
            }

            if ((pModeInfo->FlowCtrl & AMBAOD_FLOW_2STAGE) > 0U) {
                 pCtrl->NumStage = 2U;
            }
        } else if ((Idx == 1U) || (Idx == 2U) || (Idx == 3U)) {
            pCtrl->AmbaODVer = AMBAOD_VER_36;
        } else {
            pCtrl->AmbaODVer = AMBAOD_VER_UNKNOWN;
            Rval = SVC_NG;
        }

        if (Idx == 2U) {
            pCtrl->AmbaODType = AMBAOD_TYPE_EMR;
        } else {
            pCtrl->AmbaODType = AMBAOD_TYPE_FC;
        }

        if (Rval == 0U) {
            SvcLog_OK(SVC_LOG_AMBAOD, "AmbaOD_Config: AmbaODVer = %d", pCtrl->AmbaODVer, 0U);
        } else {
            SvcLog_NG(SVC_LOG_AMBAOD, "AmbaOD_Config: AmbaODVer = %d", pCtrl->AmbaODVer, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_AMBAOD, "AmbaOD_Config: GetFlowIdByName error", 0U, 0U);
    }

    if (pCtrl->TuningModeOn != 0U) {
        Rval |= AmbaOD_ConfigTuningModeV2(Inst, AlgoIdx, pCtrl->TuningMode);
    } else {
        if (pAlgoOp->LcsBinBuf.pAddr != NULL) {
            Rval |= AmbaOD_ConfigLicense(Inst, AlgoIdx, pRoiCfg->NumRoi);
        }

        if (pRoiCfg->NumRoi != 0U) {
            Rval |= AmbaOD_ConfigRoi(Inst, AlgoIdx, &pCtrl->pCvFlow->InputCfg.Input[0]);
        }

        if (pNmsCfg != NULL) {
            Rval |= AmbaOD_ConfigNms(Inst, AlgoIdx, pNmsCfg);
        }

        if (pFeature != NULL) {
            Rval |= AmbaOD_ConfigPcptFeature(Inst, AlgoIdx, pFeature);
        }
    }

    return Rval;
}

static UINT32 AmbaOD_ConfigRoi(UINT32 Inst, UINT32 AlgoIdx, const SVC_CV_INPUT_INFO_s *pInputInfo)
{
    #define SQRT2       (1414213U)
    #define BASE_VAL    (1000000U)
    UINT32 Rval = SVC_OK;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &g_AmbaODCtrl[Inst].AlgoOp[AlgoIdx];
    const SVC_CV_ROI_INFO_s *pRoiCfg = &pInputInfo->RoiInfo;
    amba_roi_config_t RoiMsg = {0};
    ambanet_roi_config_t cfg5 = {0};
    amba_roi_config_t *pRoiMsg = &RoiMsg;
    ambanet_roi_config_t *pCfg5 = &cfg5;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;
    UINT64 Scale[MAX_HALF_OCTAVES];
    //UINT64 pw[MAX_HALF_OCTAVES];
    //UINT64 ph[MAX_HALF_OCTAVES];
    UINT32 i;

    if (pInputInfo != NULL) {
        /* Pyramid size */
        Scale[0] = BASE_VAL;
        //pw[0]    = pRoiCfg->MainWidth * BASE_VAL;
        //ph[0]    = pRoiCfg->MainHeight * BASE_VAL;
        for (i = 1U; i < MAX_HALF_OCTAVES; i++) {
            Scale[i] = (Scale[i - 1U] * SQRT2) / BASE_VAL;
            //pw[i] = (pw[i - 1U] * BASE_VAL) / SQRT2;
            //ph[i] = (ph[i - 1U] * BASE_VAL) / SQRT2;
        }

        for (i = 0; i < pRoiCfg->NumRoi; i++) {
            pRoiMsg->msg_type            = AMBA_ROI_CONFIG_MSG;
            pRoiMsg->image_pyramid_index = pRoiCfg->Roi[i].Index;
            pRoiMsg->source_vin          = 0;
            pRoiMsg->roi_start_col       = pRoiCfg->Roi[i].StartX;
            pRoiMsg->roi_start_row       = pRoiCfg->Roi[i].StartY;
            pRoiMsg->roi_width           = pRoiCfg->Roi[i].Width;
            pRoiMsg->roi_height          = pRoiCfg->Roi[i].Height;

            MsgCfg.CtrlType = SCA_CT_AMBAOD_ROI_F0 + i; /* ROI index */
            MsgCfg.pExtCtrlCfg = NULL;
            AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pRoiMsg);
            Rval = SvcCvAlgo_Control(&pAlgoOp->AlgoHandle, &MsgCfg);

            /* config step 5 */
            pCfg5->msg_type      = AMBANET_MSG_TYPE_ROI;
            pCfg5->roi_index     = (uint16_t)i;
            pCfg5->frame_width   = (uint16_t)(((UINT64)pInputInfo->FrameWidth * BASE_VAL) / Scale[pRoiCfg->Roi[i].Index]);
            pCfg5->frame_height  = (uint16_t)(((UINT64)pInputInfo->FrameHeight * BASE_VAL) / Scale[pRoiCfg->Roi[i].Index]);
            //pCfg5->frame_width   = pInputInfo->FrameWidth;
            //pCfg5->frame_height  = pInputInfo->FrameHeight;
            pCfg5->pyramid_index = (UINT16)pRoiCfg->Roi[i].Index;

            /* global offset x of each image pyramid input */
            /* in pre-hierarchical resampler space */
            pCfg5->pyramid_start_col = 0;

            /* global offset y of each image pyramid input */
            /* in pre-hierarchical resampler space */
            pCfg5->pyramid_start_row = 0;

            pCfg5->pyramid_ratio_col = (UINT16)((Scale[pRoiCfg->Roi[i].Index] * 256U) / BASE_VAL);
                                                /* floor((s[cfg1.image_pyramid_index]*256)+0.5);*/
                                                /* u8.8 phase increment x of each image pyramid input */
                                                /* with respect to pre-hierarchical resampler space */

            pCfg5->pyramid_ratio_row = (UINT16)((Scale[pRoiCfg->Roi[i].Index] * 256U) / BASE_VAL);
                                                /* floor((s[cfg1.image_pyramid_index]*256)+0.5); */
                                                /* u8.8 phase increment y of each image pyramid input */
                                                /* with respect to pre-hierarchical resampler space */

            pCfg5->roi_start_col     = (UINT16)pRoiCfg->Roi[i].StartX;    /* start x of each roi within its image */                                             /* pyramid input */
            pCfg5->roi_start_row     = (UINT16)pRoiCfg->Roi[i].StartY;    /* start y of each roi within its image */
            pCfg5->roi_width         = (UINT16)pRoiCfg->Roi[i].Width;
            pCfg5->roi_height        = (UINT16)pRoiCfg->Roi[i].Height;

            MsgCfg.CtrlType = SCA_CT_AMBAOD_ROI_F0 + i; /* ROI index */;
            MsgCfg.pExtCtrlCfg = NULL;
            AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pCfg5);
            Rval |= SvcCvAlgo_Control(&pAlgoOp->AlgoHandle, &MsgCfg);
        }
    } else {
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        SvcLog_OK(SVC_LOG_AMBAOD, "AmbaOD_ConfigRoi[%d]", Inst, 0U);
    } else {
        SvcLog_NG(SVC_LOG_AMBAOD, "AmbaOD_ConfigRoi[%d] error(%d)", Inst, Rval);
    }

    return Rval;
}

static UINT32 AmbaOD_ConfigNms(UINT32 Inst, UINT32 AlgoIdx, const SVC_CV_FLOW_AMBAOD_NMS_CFG_s *pNmsCfg)
{
    UINT32 Rval;
    const SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[Inst];
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &g_AmbaODCtrl[Inst].AlgoOp[AlgoIdx];
    amba_od_nms_configs_t nms = {0};
    amba_od_nms_configs_t *pNms = &nms;
    amba_od_grouping_t group_cfg = {0};
    amba_od_grouping_t *pGroupCfg = &group_cfg;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;
    UINT32 idx;

    if (pNmsCfg != NULL) {
        /* AmbaOD36 */
        pNms->msg_type    = AMBANET_MSG_TYPE_OD_NMS;
        pNms->num_configs = pNmsCfg->NumConfig;

        /* 0 */
        Rval = AmbaWrap_memset(&nms.config[0], 0, sizeof(amba_od_nms_config_t));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_AMBAOD, "memset nmscfg failed", 0U, 0U);
        }


        if ((pCtrl->AmbaODVer == ((AMBAOD_VER_37))) || (pCtrl->AmbaODVer == (AMBAOD_VER_37|AMBAOD_VER_37_ADD_PERCEPTION))) {
            /* 0 - 15 */
            for (idx = 0U; idx < pNmsCfg->NumConfig; idx++) {
                pNms->config[idx].nms_config_id        = (UINT16) idx;
                pNms->config[idx].max_final_results_m1 = (UINT32) pNmsCfg->Param[idx].MaxResult - 1U;
                if (pNmsCfg->Param[idx].IOUTh != 0U) {
                    pNms->config[idx].iou_enable       = 1;
                    pNms->config[idx].iou_threshold    = pNmsCfg->Param[idx].IOUTh;
                } else {
                    pNms->config[idx].iou_enable       = 1;
                    pNms->config[idx].iou_threshold    = 128;        /* .50 */
                }
                if (pNmsCfg->Param[idx].IOMTh != 0U) {
                    pNms->config[idx].iom_enable       = 1;
                    pNms->config[idx].iom_threshold    = pNmsCfg->Param[idx].IOMTh;
                } else {
                    pNms->config[idx].iom_enable       = 0;
                    pNms->config[idx].iom_threshold    = 128;
                }
                pNms->config[idx].bb_width_threshold   = 16;
                pNms->config[idx].bb_height_threshold  = 15;
                pNms->config[idx].score_threshold[0U]  = pNmsCfg->Param[idx].ScoreTh;
                pNms->config[idx].score_threshold[1U]  = pNmsCfg->Param[idx].ScoreTh;
                pNms->config[idx].score_threshold[2U]  = pNmsCfg->Param[idx].ScoreTh;
                pNms->config[idx].score_threshold[3U]  = pNmsCfg->Param[idx].ScoreTh;
            }

            /* Grouping for NMS */
            pGroupCfg->msg_type     = AMBANET_MSG_TYPE_OD_GROUPING;
            pGroupCfg->group_id[0]  = 0;
            pGroupCfg->group_id[1]  = 1;
            pGroupCfg->group_id[2]  = 2;
            pGroupCfg->group_id[3]  = 2;
            pGroupCfg->group_id[4]  = 4;
            pGroupCfg->group_id[5]  = 4;
            pGroupCfg->group_id[6]  = 6;
            pGroupCfg->group_id[7]  = 6;
            pGroupCfg->group_id[8]  = 8;
            pGroupCfg->group_id[9]  = 9;
            pGroupCfg->group_id[10] = 10;
            pGroupCfg->group_id[11] = 11;
            pGroupCfg->group_id[12] = 11;
            pGroupCfg->group_id[13] = 13;
            pGroupCfg->group_id[14] = 11;
            pGroupCfg->group_id[15] = 15;
        } else {
            Rval = SVC_NG;
        }

        MsgCfg.CtrlType = 0;
        MsgCfg.pExtCtrlCfg = NULL;

        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pNms);
        Rval |= SvcCvAlgo_Control(&pAlgoOp->AlgoHandle, &MsgCfg);

        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pGroupCfg);
        Rval |= SvcCvAlgo_Control(&pAlgoOp->AlgoHandle, &MsgCfg);
    } else {
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        SvcLog_OK(SVC_LOG_AMBAOD, "AmbaOD_ConfigNms[%d]", Inst, 0U);
    } else {
        SvcLog_NG(SVC_LOG_AMBAOD, "AmbaOD_ConfigNms[%d] error(%d)", Inst, Rval);
    }

    return Rval;
}

static UINT32 AmbaOD_ConfigLicense(UINT32 Inst, UINT32 AlgoIdx, UINT32 NumScale)
{
#if defined AMBANET_MSG_TYPE_LICENSE
    #define LICENSE_DATA_SIZE   (1020U)
    typedef struct {
        UINT32  Msg;
        UINT8   Payload[LICENSE_DATA_SIZE];
    } SVC_CV_FLOW_AMBAOD_LCS_CFG_s;
    UINT32 Rval;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &g_AmbaODCtrl[Inst].AlgoOp[AlgoIdx];
    SVC_CV_FLOW_AMBAOD_LCS_CFG_s LcsCfg;
    SVC_CV_FLOW_AMBAOD_LCS_CFG_s *pLcsCfg = &LcsCfg;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;
    UINT32 CtrlType;

    if (NumScale > 0U) {
        pLcsCfg->Msg = AMBANET_MSG_TYPE_LICENSE;

        Rval = AmbaWrap_memcpy(LcsCfg.Payload, pAlgoOp->LcsBinBuf.pAddr, LICENSE_DATA_SIZE);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_AMBAOD, "memcpy LcsBinBuf failed", 0U, 0U);
        }

        if (NumScale == 2U) {
            CtrlType = SCA_CT_AMBAOD_LCS_2SCALE;
        } else {
            CtrlType = 0U;
        }
        MsgCfg.CtrlType = CtrlType; /* Number of ROI */
        MsgCfg.pExtCtrlCfg = NULL;
        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pLcsCfg);
        Rval = SvcCvAlgo_Control(&pAlgoOp->AlgoHandle, &MsgCfg);
    } else {
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        SvcLog_OK(SVC_LOG_AMBAOD, "AmbaOD_ConfigLicense[%d]", Inst, 0U);
    } else {
        SvcLog_NG(SVC_LOG_AMBAOD, "AmbaOD_ConfigLicense[%d] error(%d)", Inst, Rval);
    }

    return Rval;
#else
    AmbaMisra_TouchUnused(&Inst);
    AmbaMisra_TouchUnused(&AlgoIdx);
    return SVC_OK;
#endif
}

static UINT32 AmbaOD_ConfigPcptFeature(UINT32 Inst, UINT32 AlgoIdx, const void *pFeature)
{
    UINT32 Rval = SVC_OK;
    SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[Inst];
    UINT32 Param;

    if (pCtrl->AmbaODVer == (AMBAOD_VER_37|AMBAOD_VER_37_ADD_PERCEPTION)) {
        Rval = AmbaWrap_memcpy(&pCtrl->PcptFeature, pFeature, sizeof(SVC_CV_FLOW_AMBAOD_PCPT_FEATURE_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_AMBAOD, "memcpy PcptFeature failed", 0U, 0U);
        }

        Param = pCtrl->pCvFlow->Config[0U];
        if (Param != 0U) {
            if ((Param & ((UINT32)0x1 << AMBANET_OD37_KP)) != 0U) {
                pCtrl->PcptFeature.EnableKP = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN);
            } else {
                pCtrl->PcptFeature.EnableKP = 0U;
            }
            if ((Param & ((UINT32)0x1 << AMBANET_OD37_3D)) != 0U) {
                pCtrl->PcptFeature.Enable3D = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN);
            } else {
                pCtrl->PcptFeature.Enable3D = 0U;
            }
            if (((Param & ((UINT32)0x1 << AMBANET_OD37_MKL)) != 0U) ||
                ((Param & ((UINT32)0x1 << AMBANET_OD37_MKS)) != 0U)) {
                pCtrl->PcptFeature.EnableMask = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN);
            } else {
                pCtrl->PcptFeature.EnableMask = 0U;
            }
            if ((Param & ((UINT32)0x1 << AMBANET_OD37_TS)) != 0U) {
                pCtrl->PcptFeature.EnableTS = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN);
            } else {
                pCtrl->PcptFeature.EnableTS = 0U;
            }
            if ((Param & ((UINT32)0x1 << AMBANET_OD37_SL)) != 0U) {
                pCtrl->PcptFeature.EnableSL = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN);
            } else {
                pCtrl->PcptFeature.EnableSL = 0U;
            }
            if ((Param & ((UINT32)0x1 << AMBANET_OD37_TLC)) != 0U) {
                pCtrl->PcptFeature.EnableTLC = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN);
            } else {
                pCtrl->PcptFeature.EnableTLC = 0U;
            }
            if ((Param & ((UINT32)0x1 << AMBANET_OD37_TLS)) != 0U) {
                pCtrl->PcptFeature.EnableTLS = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN);
            } else {
                pCtrl->PcptFeature.EnableTLS = 0U;
            }
            if ((Param & ((UINT32)0x1 << AMBANET_OD37_AR)) != 0U) {
                pCtrl->PcptFeature.EnableAR = (UINT8)(AMBAOD_PERCEPTION_INIT | AMBAOD_PERCEPTION_RUN);
            } else {
                pCtrl->PcptFeature.EnableAR = 0U;
            }
        }

        Param = pCtrl->pCvFlow->Config[1U];
        if (Param != 0U) {
            pCtrl->PcptFeature.OutBaseWidth = (UINT16)((Param >> 16U) & 0x0000FFFFU);
            pCtrl->PcptFeature.OutBaseHeight = (UINT16)(Param & 0x0000FFFFU);
        }

        AmbaPrint_PrintUInt5("AmbaOD_ConfigPcptFeature: KP(0x%x) 3D(0x%x) Mask(0x%x) OSD(%dx%d)",
            pCtrl->PcptFeature.EnableKP,
            pCtrl->PcptFeature.Enable3D,
            pCtrl->PcptFeature.EnableMask,
            pCtrl->PcptFeature.OutBaseWidth,
            pCtrl->PcptFeature.OutBaseHeight);
        AmbaPrint_PrintUInt5("AmbaOD_ConfigPcptFeature: TS(0x%x) SL(0x%x) TLC(0x%x) TLS(0x%x) AR(0x%x)",
            pCtrl->PcptFeature.EnableTS,
            pCtrl->PcptFeature.EnableSL,
            pCtrl->PcptFeature.EnableTLC,
            pCtrl->PcptFeature.EnableTLS,
            pCtrl->PcptFeature.EnableAR);
    } else {
        /* Do nothing */
    }

    AmbaMisra_TouchUnused(&AlgoIdx);

    return Rval;
}

static void AmbaOD_UpdatePcptNextStepCfg(AmbaPcptCfgNext *pNextCfg, const SVC_CV_FLOW_AMBAOD_PCPT_FEATURE_s *pFeature)
{
    pNextCfg->RoiIdx     = 0U;
    pNextCfg->EnableKP   = pFeature->EnableKP;
    pNextCfg->Enable3D   = pFeature->Enable3D;
    pNextCfg->EnableMask = pFeature->EnableMask;
    pNextCfg->EnableTS   = pFeature->EnableTS;
    pNextCfg->EnableSL   = pFeature->EnableSL;
    pNextCfg->EnableTLC  = pFeature->EnableTLC;
    pNextCfg->EnableTLS  = pFeature->EnableTLS;
    pNextCfg->EnableAR   = pFeature->EnableAR;
    pNextCfg->OSDWidth   = pFeature->OutBaseWidth;
    pNextCfg->OSDHeight  = pFeature->OutBaseHeight;
}

static UINT32 AmbaOD_ConfigTuningModeV2(UINT32 Inst, UINT32 AlgoIdx, UINT32 TuningMode)
{
    UINT32 Rval;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &g_AmbaODCtrl[Inst].AlgoOp[AlgoIdx];
    AMBA_FS_FILE *pFp;
    const char *pCfgFile = "ambaod37_cfg.bin";
    uint32_t BytesRead, FileSize;
    uint8_t Buf[sizeof(amba_od_nms_configs_t) + \
                sizeof(amba_od_grouping_t) + \
                sizeof(ambanet_roi_config_t) + sizeof(ambanet_roi_config_t)];
    uint8_t *pBuf = Buf;
    const amba_od_nms_configs_t *pNms;
    const amba_od_grouping_t *pGroup;
    const ambanet_roi_config_t *RoiCfg[2];
    amba_roi_config_t RoiMsg;
    const amba_roi_config_t *pRoiMsg;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;

    Rval = AmbaFS_FileOpen(pCfgFile, "rb", &pFp);
    if (Rval != 0U) {
        AmbaPrint_PrintUInt5("can't read config file!!",
                0U, 0U, 0U, 0U, 0U);
    } else {
        Rval = AmbaCV_UtilityFileSize(pCfgFile, &FileSize);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_AMBAOD, "AmbaCV_UtilityFileSize failed(0x%x)", Rval, 0U);
        }

        Rval = AmbaFS_FileRead(pBuf, 1U, (UINT32)FileSize, pFp, &BytesRead);
        if ((BytesRead == 0U) || (Rval != 0U)) {
            SvcLog_NG(SVC_LOG_AMBAOD, "Error while reading file!! (%d) (%d)", Rval, 0U);
        }

        Rval = AmbaFS_FileClose(pFp);
        if(Rval != 0U) {
            SvcLog_NG(SVC_LOG_AMBAOD, "Error while closing file!! (%d) (%d)", Rval, 0U);
        }

        AmbaMisra_TypeCast(&pNms, &pBuf);

        pBuf     = &Buf[sizeof(amba_od_nms_configs_t)];
        AmbaMisra_TypeCast(&pGroup, &pBuf);

        pBuf     = &Buf[sizeof(amba_od_nms_configs_t) + \
                        sizeof(amba_od_grouping_t)];
        AmbaMisra_TypeCast(&RoiCfg[0], &pBuf);

        pBuf     = &Buf[sizeof(amba_od_nms_configs_t) + \
                        sizeof(amba_od_grouping_t) + \
                        sizeof(ambanet_roi_config_t)];
        AmbaMisra_TypeCast(&RoiCfg[1], &pBuf);

        RoiMsg.msg_type                = AMBA_ROI_CONFIG_MSG;
        RoiMsg.image_pyramid_index     = RoiCfg[0]->pyramid_index;
        RoiMsg.source_vin              = 0;
        RoiMsg.roi_start_col           = RoiCfg[0]->roi_start_col;
        RoiMsg.roi_start_row           = RoiCfg[0]->roi_start_row;
        RoiMsg.roi_width               = RoiCfg[0]->roi_width;
        RoiMsg.roi_height              = RoiCfg[0]->roi_height;
        pRoiMsg = &RoiMsg;
        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pRoiMsg);

        MsgCfg.pExtCtrlCfg = NULL;
        MsgCfg.CtrlType = 0;
        Rval = SvcCvAlgo_Control(&pAlgoOp->AlgoHandle, &MsgCfg);
        if (Rval != 0U) {
            SvcLog_NG(SVC_LOG_AMBAOD, "SvcCvAlgo_Control fail", 0U, 0U);
        }

        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pNms);
        Rval = SvcCvAlgo_Control(&pAlgoOp->AlgoHandle, &MsgCfg);
        if (Rval != 0U) {
            SvcLog_NG(SVC_LOG_AMBAOD, "SvcCvAlgo_Control fail", 0U, 0U);
        }

        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pGroup);
        Rval = SvcCvAlgo_Control(&pAlgoOp->AlgoHandle, &MsgCfg);
        if (Rval != 0U) {
            SvcLog_NG(SVC_LOG_AMBAOD, "SvcCvAlgo_Control fail", 0U, 0U);
        }

        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &RoiCfg[0]);
        Rval = SvcCvAlgo_Control(&pAlgoOp->AlgoHandle, &MsgCfg);
        if (Rval != 0U) {
            SvcLog_NG(SVC_LOG_AMBAOD, "SvcCvAlgo_Control fail", 0U, 0U);
        }
    }

    AmbaMisra_TouchUnused(&TuningMode);

    return Rval;
}

static UINT32 AmbaOD_ConfigBuffer(UINT32 Inst)
{
    UINT32 Rval = SVC_OK;
    SVC_AMBAOD_CTRL_s *pCtrl = &g_AmbaODCtrl[Inst];
    SVC_CV_FLOW_BUF_INFO_s BufInfo = {NULL, 0U, 0U};
    UINT32 i;

    /* Img info buffer */
    if (pCtrl->pImgInfoBuf[0] == NULL) {
        for (i = 0; i < SVC_AMBAOD_PICINFO_QUEUE_SIZE; i++) {
            Rval = SvcCvFlow_AllocWorkBuf((UINT32)sizeof(memio_source_recv_picinfo_t), &BufInfo);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pCtrl->pImgInfoBuf[i], &BufInfo.pAddr);
                pCtrl->ImgInfoBufSize = BufInfo.Size;
            } else {
                SvcLog_NG(SVC_LOG_AMBAOD, "Alloc Img info buf failed", 0U, 0U);
                Rval = SVC_NG;
                break;
            }
        }
    }

    /* Inter data buffer */
    if (pCtrl->pInterDataBuf[0] == NULL) {
        for (i = 0; i < SVC_AMBAOD_OUTPUT_DEPTH; i++) {
            Rval = SvcCvFlow_AllocWorkBuf((UINT32)sizeof(AMBAOD_INTER_DATA_s), &BufInfo);
            if (SVC_OK == Rval) {
                Rval = AmbaWrap_memset(BufInfo.pAddr, 0, BufInfo.Size);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_AMBAOD, "memset pInterDataBuf[%u] failed", i, 0U);
                }

                AmbaMisra_TypeCast(&pCtrl->pInterDataBuf[i], &BufInfo.pAddr);
            } else {
                SvcLog_NG(SVC_LOG_AMBAOD, "Alloc Inter data buf failed", 0U, 0U);
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
static UINT32 CvFlow_AmbaODInit(UINT32 CvFlowChan, void *pInit)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    static UINT32 g_AmbaODInit = 0U;

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(pInit);

    if (0U == g_AmbaODInit) {
        g_AmbaODInit = 1U;

        if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&ODMutex, NULL)) {
            SvcLog_NG(SVC_LOG_AMBAOD, "MutexCreate error", 0U, 0U);
            Rval = SVC_NG;
        }
        for (i = 0; i < SVC_AMBAOD_MAX_INSTANCE; i++) {
            g_AmbaODCtrl[i].Used = 0;
        }

        SVC_CV_DBG(SVC_LOG_AMBAOD, "CvFlow_AmbaODInit: first init", 0U, 0U);
    }

    SvcLog_OK(SVC_LOG_AMBAOD, "CvFlow_AmbaODInit: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver configuration function
*/
static UINT32 CvFlow_AmbaODConfig(UINT32 CvFlowChan, void *pConfig)
{
    UINT32 Rval = SVC_OK;
    UINT32 i, j;
    SVC_AMBAOD_CTRL_s *pCtrl = NULL;
    const SVC_CV_FLOW_AMBAOD_MODE_INFO_s *pModeInfo;
    SVC_CV_INPUT_CFG_s *pInputCfg;

    AmbaMisra_TouchUnused(pConfig);

    if (NULL != pConfig) {
        AmbaOD_MutexTake();
        /* Create instance */
        for (i = 0; i < SVC_AMBAOD_MAX_INSTANCE; i++) {
            if (0U == g_AmbaODCtrl[i].Used) {
                Rval = AmbaWrap_memset(&g_AmbaODCtrl[i], 0, sizeof(SVC_AMBAOD_CTRL_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_AMBAOD, "memset g_AmbaODCtrl[%u] failed", i, 0U);
                }
                pCtrl = &g_AmbaODCtrl[i];
                pCtrl->Used = 1;
                SvcLog_DBG(SVC_LOG_AMBAOD, "Create Inst(%u)", i, 0U);
                break;
            }
        }
        AmbaOD_MutexGive();

        /* Configure instance */
        if (NULL != pCtrl) {
            pCtrl->CvfChan     = (UINT8) CvFlowChan;
            pCtrl->pCvFlow     = &pCtrl->CvCfg;
            Rval = AmbaWrap_memcpy(&pCtrl->CvCfg, pConfig, sizeof(SVC_CV_FLOW_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_AMBAOD, "memcpy CvCfg failed", 0U, 0U);
            }
            pCtrl->Enable      = 1;
            AmbaOD_SrcRateUpdate(pCtrl, pCtrl->pCvFlow->InputCfg.SrcRate);

            pModeInfo = &SvcCvFlow_AmbaOD_ModeInfo[pCtrl->pCvFlow->CvModeID];
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

                pCtrl->NumODScale = (UINT32)pInputCfg->Input[0].RoiInfo.NumRoi;
                pCtrl->FlexiBinBits = (UINT32)pCtrl->pCvFlow->Config[0U];
            } else {
                Rval = SVC_NG;
            }

            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_AMBAOD, "unsupported InputNum(%u), NumRoi(%u)", pInputCfg->InputNum, pInputCfg->Input[0].RoiInfo.NumRoi);
            } else {
                if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG) {
                    Rval = SVC_OK;
                } else {
                    SvcLog_NG(SVC_LOG_AMBAOD, "unsupported CVFlow ProfID(%u)", pModeInfo->ProfID, 0U);
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
        SvcLog_OK(SVC_LOG_AMBAOD, "CvFlow_AmbaODConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_AMBAOD, "CvFlow_AmbaODConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver load function
*/
static UINT32 CvFlow_AmbaODLoad(UINT32 CvFlowChan, void *pInfo)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_AMBAOD_CTRL_s *pCtrl;
    const SVC_CV_FLOW_LOAD_INFO_s *pLoadInfo;
    const SVC_CV_FLOW_AMBAOD_MODE_INFO_s *pModeInfo;
    UINT32 i;
    SVC_CV_FLOW_ALGO_INFO_s AlgoInfo;

    (void) pInfo;

    Rval = AmbaOD_CvfChan2Inst(CvFlowChan, &Inst);
    if (Rval == SVC_OK) {
        pCtrl = &g_AmbaODCtrl[Inst];
        if (NULL != pInfo) {
            AmbaMisra_TypeCast(&pLoadInfo, &pInfo);
            if (pLoadInfo->NumBin > 0U) {
                /* Use external path */
                if (pLoadInfo->AlgoIdx < AMBAOD_MAX_ALGO_NUM) {
                    Rval = SvcCvFlow_LoadAlgoBin(SVC_LOG_AMBAOD, pLoadInfo, &pCtrl->AlgoOp[pLoadInfo->AlgoIdx]);
                } else {
                    SvcLog_NG(SVC_LOG_AMBAOD, "CvFlow_AmbaODLoad: CvFlowChan(%d) invalid AlgoIdx(%u)", CvFlowChan, pLoadInfo->AlgoIdx);
                }
            } else {
                /* Use ModeInfo */
                if (pCtrl->pCvFlow->CvModeID < AMBAOD_NUM_MODE) {
                    pModeInfo = &SvcCvFlow_AmbaOD_ModeInfo[pCtrl->pCvFlow->CvModeID];

                    for (i = 0; i < pModeInfo->AlgoGrp.NumAlgo; i++) {
                        AmbaSvcWrap_MisraMemcpy(&AlgoInfo, &pModeInfo->AlgoGrp.AlgoInfo[i], sizeof(SVC_CV_FLOW_ALGO_INFO_s));
                        if (pCtrl->FlexiBinBits != 0U) {
                            AlgoInfo.FlexiBinBits = pCtrl->FlexiBinBits;
                        }
                        Rval |= SvcCvFlow_LoadAlgo(SVC_LOG_AMBAOD, &AlgoInfo, &pCtrl->AlgoOp[i]);
                    }
                } else  {
                    SvcLog_NG(SVC_LOG_AMBAOD, "CvFlow_AmbaODLoad: CvFlowChan(%d) invalid CvModeID(%u)", CvFlowChan, pCtrl->pCvFlow->CvModeID);
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_AMBAOD, "CvFlow_AmbaODLoad: invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_AMBAOD, "CvFlow_AmbaODLoad: CvFlowChan(%d) error", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver start function
*/
static UINT32 CvFlow_AmbaODStart(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst, i, j, AlgoIdx, Num;
    SVC_AMBAOD_CTRL_s *pCtrl;
    const SVC_CV_FLOW_AMBAOD_MODE_INFO_s *pModeInfo;
    SVC_CV_FLOW_BUF_INFO_s BufInfo = {0};
    const SVC_CV_FLOW_ALGO_INFO_s *pAlgoInfo;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
    SVC_CV_FLOW_ALGO_QUERY_CFG_s QueryCfg;
    UINT32 QueryExtCfg;
    const UINT32 *pQueryExtCfg;
    SVC_CV_FLOW_ALGO_CREATE_CFG_s CreateCfg;
    ExtTaskCreateCfg CreateExtCfg;
    ExtTaskCreateCfg *pCreateExtCfg = &CreateExtCfg;
    SVC_CV_ALGO_REGCB_CFG_s CbCfg;
    SVC_IMG_FEEDER_PORT_CFG_s PortCfg = {0};
    char Text[2];
    static char CbEvtQueName[] = "EvtQue##_AmbaOD";
    static char SemName[] = "OutBufSem";
    static AMBAOD_OUTPUT_MSG_s AlgoOutQue1[SVC_AMBAOD_MAX_INSTANCE][SVC_AMBAOD_ALGO_OUT_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static AMBAOD_OUTPUT_MSG_s AlgoOutQue2[SVC_AMBAOD_MAX_INSTANCE][SVC_AMBAOD_ALGO_OUT_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 ODRxTaskStack[SVC_AMBAOD_MAX_INSTANCE][SVC_AMBAOD_RX_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_IMG_FEEDER_EVENT_CALLBACK_f AmbaOD_TxDataSrcHandlerList[SVC_AMBAOD_MAX_INSTANCE] = {
        AmbaOD_TxDataSrcHandler0,
        AmbaOD_TxDataSrcHandler1,
        AmbaOD_TxDataSrcHandler2,
        AmbaOD_TxDataSrcHandler3,
    };
    static SVC_CV_ALGO_CALLBACK_f AmbaOD_AlgoOutCbList[SVC_AMBAOD_MAX_INSTANCE] = {
        AmbaOD_AlgoOutCallback0,
        AmbaOD_AlgoOutCallback1,
        AmbaOD_AlgoOutCallback2,
        AmbaOD_AlgoOutCallback3,
    };
    AMBAOD_OUTPUT_MSG_s *pQueList[AMBAOD_PIPE_STAGE_NUM];

    AmbaMisra_TouchUnused(pInput);
    AmbaMisra_TouchUnused(AmbaOD_TxDataSrcHandlerList);
    AmbaMisra_TouchUnused(AmbaOD_AlgoOutCbList);
    AmbaMisra_TouchUnused(AlgoOutQue1);
    AmbaMisra_TouchUnused(AlgoOutQue2);

    Rval = AmbaOD_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl     = &g_AmbaODCtrl[Inst];
        pModeInfo = &SvcCvFlow_AmbaOD_ModeInfo[pCtrl->pCvFlow->CvModeID];

        /* Open Feeder port first to enable frame FIFO earlier */
        if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
            /* The driver supports single input per instance */
            PortCfg.NumPath            = 1U;
            PortCfg.Content[0].DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
            PortCfg.Content[0].StrmId  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
            PortCfg.SendFunc           = AmbaOD_TxDataSrcHandlerList[Inst];
            if (SVC_OK != SvcImgFeeder_OpenPort(pCtrl->pCvFlow->InputCfg.FeederID, &PortCfg, &pCtrl->FeederPort)) {
                SvcLog_NG(SVC_LOG_AMBAOD, "Open feeder port error", 0U, 0U);
            }
        } else {
            if (pCtrl->pCvFlow->InputCfg.Input[0].DataSrc != SVC_CV_DATA_SRC_EXT_DATA) {
                SvcLog_NG(SVC_LOG_AMBAOD, "ImgFeeder is not enabled", 0U, 0U);
            }
        }

        for (AlgoIdx = 0; AlgoIdx < pModeInfo->AlgoGrp.NumAlgo; AlgoIdx++) {
            pAlgoInfo = &pModeInfo->AlgoGrp.AlgoInfo[AlgoIdx];
            pAlgoOp   = &pCtrl->AlgoOp[AlgoIdx];

            Rval = AmbaWrap_memset(&QueryCfg, 0, sizeof(SVC_CV_FLOW_ALGO_QUERY_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_AMBAOD, "memset QueryCfg failed", 0U, 0U);
            }

            /* Pass number of OD scales for memory query */
            QueryExtCfg = pCtrl->NumODScale;
            QueryExtCfg |= SvcCvFlow_GetChipID();
            pQueryExtCfg = &QueryExtCfg;
            AmbaMisra_TypeCast(&QueryCfg.pExtCfg, &pQueryExtCfg);

            Rval = SvcCvFlow_QueryAlgo(SVC_LOG_AMBAOD, pAlgoInfo, &QueryCfg, pAlgoOp);
            if (SVC_OK == Rval) {
                /* Create algo */
                Rval = AmbaWrap_memset(&CreateCfg, 0, sizeof(SVC_CV_FLOW_ALGO_CREATE_CFG_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_AMBAOD, "memset CreateCfg failed", 0U, 0U);
                }

                pCreateExtCfg->MagicCode = ExtTaskCreateCfgMagic;
                pCreateExtCfg->RunTskCtrl.StackSz = ((UINT32)16U << 10U);
                pCreateExtCfg->RunTskCtrl.Priority = 50;
                pCreateExtCfg->RunTskCtrl.CoreSel = CONFIG_ICAM_SCA_TASK_CORE_SELECT;
                pCreateExtCfg->CBTskCtrl.StackSz = ((UINT32)16U << 10U);
                pCreateExtCfg->CBTskCtrl.Priority = 51;
                pCreateExtCfg->CBTskCtrl.CoreSel = CONFIG_ICAM_SCA_TASK_CORE_SELECT;
                pCreateExtCfg->ROIs = (UINT16)pCtrl->NumODScale;
                pCreateExtCfg->ODSize = (pModeInfo->Cfg[0].NetWidth == AMBAOD_FC_NET_WIDTH_RES1) ? (UINT16)SCA_CT_AMBAOD_SIZE1 : (UINT16)SCA_CT_AMBAOD_SIZE0;
                AmbaMisra_TypeCast(&CreateCfg.pExtCfg, &pCreateExtCfg);
                AmbaPrint_PrintUInt5("SvcCvFlow_CreateAlgo: ROIs: %u ODSize: %u",
                    pCreateExtCfg->ROIs, pCreateExtCfg->ODSize, 0U, 0U, 0U);

                Rval = SvcCvFlow_CreateAlgo(SVC_LOG_AMBAOD, pAlgoInfo, &CreateCfg, pAlgoOp);

                if (SVC_OK == Rval) {
                    /* Create Output buffer */
                    for (i = 0; i < SVC_AMBAOD_OUTPUT_DEPTH; i++) {
                        Num = pAlgoOp->OutputNum;
                        for (j = 0; j < Num; j++) {
                            Rval |= SvcCvFlow_AllocWorkBuf(pAlgoOp->OutputSize[j], &BufInfo);
                            SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_AMBAOD, BufInfo.pAddr, BufInfo.Size, &pCtrl->AlgoOpEx[AlgoIdx].OutputBuf[i][j]);
                        }
                    }

                    if (SVC_OK == Rval) {
                        /* Register Algo callback */
                        Rval = AmbaWrap_memset(&CbCfg, 0, sizeof(SVC_CV_ALGO_REGCB_CFG_s));
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_AMBAOD, "memset CbCfg failed", 0U, 0U);
                        }
                        CbCfg.Mode     = 0U;
                        CbCfg.Callback = AmbaOD_AlgoOutCbList[Inst];
                        Rval = SvcCvAlgo_RegCallback(&pAlgoOp->AlgoHandle, &CbCfg);
                        if (SVC_OK == Rval) {
                            Rval = AmbaOD_Config(Inst, AlgoIdx);
                            if (SVC_OK != Rval) {
                                SvcLog_NG(SVC_LOG_AMBAOD, "FlexiDAG Config error(0x%X)", Rval, 0U);
                            }

                            pCtrl->NumValidAlgo++;
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_AMBAOD, "Alloc Algo[%u] Output Buf failed", AlgoIdx, 0U);
                    }
                } else {
                   SvcLog_NG(SVC_LOG_AMBAOD, "CreateAlgo[%u] error", AlgoIdx, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_AMBAOD, "QueryAlgo[%u] error", AlgoIdx, 0U);
            }
        }

        if (SVC_OK == Rval) {
            Rval = AmbaOD_ConfigBuffer(Inst);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_AMBAOD, "ConfigBuffer error", 0U, 0U);
            }
        }

        if (SVC_OK == Rval) {
            pCtrl->pTxProc = AmbaOD_TxDataSrcHandler;
            if ((pModeInfo->FlowCtrl & AMBAOD_FLOW_2STAGE) > 0U) {
                pCtrl->pRxProc = AmbaOD_RxDataSinkHandler2Stages;
            } else {
                pCtrl->pRxProc = AmbaOD_RxDataSinkHandler;
            }

            pQueList[AMBAOD_PIPE_STAGE_1] = &AlgoOutQue1[Inst][0];
            pQueList[AMBAOD_PIPE_STAGE_2] = &AlgoOutQue2[Inst][0];

            if (1U != AmbaUtility_UInt32ToStr(&Text[0], (UINT32)sizeof(Text), Inst, 10)) {
                SvcLog_NG(SVC_LOG_AMBAOD, "UInt32ToStr failed", 0U, 0U);
            }
            CbEvtQueName[6] = Text[0];

            for (i = 0; i < pCtrl->NumStage; i++) {
                if (1U != AmbaUtility_UInt32ToStr(&Text[0], (UINT32)sizeof(Text), i, 10)) {
                    SvcLog_NG(SVC_LOG_AMBAOD, "UInt32ToStr failed", 0U, 0U);
                }
                CbEvtQueName[7] = Text[0];

                Rval = AmbaKAL_MsgQueueCreate(&pCtrl->AlgoOutQueId[i],
                                              CbEvtQueName,
                                              (UINT32)sizeof(AMBAOD_OUTPUT_MSG_s),
                                              pQueList[i],
                                              SVC_AMBAOD_ALGO_OUT_QUEUE_SIZE * (UINT32)sizeof(AMBAOD_OUTPUT_MSG_s));
                Rval |= AmbaKAL_SemaphoreCreate(&pCtrl->OutBufSem[i], SemName, SVC_AMBAOD_SEM_INIT_COUNT);
                if (KAL_ERR_NONE != Rval) {
                    SvcLog_NG(SVC_LOG_AMBAOD, "Create MsgQue/Semaphore error(0x%X)", Rval, 0U);
                    break;
                }
            }

            if (KAL_ERR_NONE == Rval) {
                pCtrl->RxTaskCtrl.Priority   = 60;
                pCtrl->RxTaskCtrl.EntryFunc  = AmbaOD_RxTaskEntry;
                pCtrl->RxTaskCtrl.EntryArg   = Inst;
                pCtrl->RxTaskCtrl.pStackBase = &ODRxTaskStack[Inst][0];
                pCtrl->RxTaskCtrl.StackSize  = SVC_AMBAOD_RX_TASK_STACK_SIZE;
                pCtrl->RxTaskCtrl.CpuBits    = 0x01U;
                Rval |= SvcTask_Create("SvcODRxTask", &pCtrl->RxTaskCtrl);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_AMBAOD, "Create RxTask Inst(%u) error", Inst, 0U);
                }

                /* Start input */
                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
#if defined (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE) && (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE == 0x2)
                    /* Queue more frames in FIFO */
                    Rval = AmbaKAL_TaskSleep(100);
                    if (Rval != SVC_OK) {
                        SvcLog_NG(SVC_LOG_AMBAOD, "AmbaKAL_TaskSleep failed %u", Rval, 0U);
                    }
#endif
                    Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_AMBAOD, "Connect feeder port error", 0U, 0U);
                    }
                }

                if (SVC_OK == Rval) {
                    pCtrl->Started = 1U;
                }
            } else {
                Rval =  SVC_NG;
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_AMBAOD, "Invalid CvFlowChan", 0U, 0U);
        Rval =  SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_AMBAOD, "CvFlow_AmbaODStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_AMBAOD, "CvFlow_AmbaODStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver control function
*/
static UINT32 CvFlow_AmbaODControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo)
{
    UINT32 Rval, Inst;
    SVC_AMBAOD_CTRL_s *pCtrl;
    const UINT32 *pValue;
    SVC_CV_ROI_INFO_s *pRoiInfo = NULL;
    SVC_CV_INPUT_INFO_s *pInputInfo = NULL;
    SVC_CV_FRAME_TIME_INFO_s *pFTInfo = NULL;

    AmbaOD_MutexTake();

    Rval = AmbaOD_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl = &g_AmbaODCtrl[Inst];
        switch (CtrlType) {
            case SVC_CV_CTRL_ENABLE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->Enable = (UINT8) *pValue;
                if ((1U == pCtrl->Started) &&
                    (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder)) {
                    if (*pValue == 0U) {
                        Rval = SvcImgFeeder_Disconnect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_AMBAOD, "Disconn feeder port error", 0U, 0U);
                        }
                    } else {
                        Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_AMBAOD, "Cnnect feeder port error", 0U, 0U);
                        }
                    }
                }
                break;
            case SVC_CV_CTRL_SET_SRC_RATE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                AmbaOD_SrcRateUpdate(pCtrl, *pValue);
                break;
            case SVC_CV_CTRL_SEND_EXT_YUV:
                Rval = AmbaOD_SendExtYUV(Inst, pInfo);
                break;
            case SVC_CV_CTRL_GET_ROI_INFO:
                AmbaMisra_TypeCast(&pRoiInfo, &pInfo);
                pInputInfo = &pCtrl->pCvFlow->InputCfg.Input[0];
                if (pInputInfo->RoiInfo.NumRoi != 0U) {
                    Rval = AmbaWrap_memcpy(pRoiInfo, &pInputInfo->RoiInfo, sizeof(SVC_CV_ROI_INFO_s));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_AMBAOD, "memcpy pRoiInfo failed", 0U, 0U);
                    }
                } else {
                    Rval = AmbaWrap_memset(pRoiInfo, 0x0, sizeof(SVC_CV_ROI_INFO_s));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_AMBAOD, "memset pRoiInfo failed", 0U, 0U);
                    }
                }
                break;
            case SVC_CV_CTRL_SET_ROI_INFO:
                AmbaMisra_TypeCast(&pRoiInfo, &pInfo);
                pInputInfo = &pCtrl->pCvFlow->InputCfg.Input[0];
                if (pInputInfo->RoiInfo.NumRoi != 0U) {
                    Rval = AmbaWrap_memcpy(&pInputInfo->RoiInfo, pRoiInfo, sizeof(SVC_CV_ROI_INFO_s));
                    if (SVC_OK == Rval) {
                        Rval = AmbaOD_ConfigRoi(Inst, 0U, pInputInfo);
                    } else {
                        SvcLog_NG(SVC_LOG_AMBAOD, "memcpy pRoiInfo failed", 0U, 0U);
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
            case SVC_CV_CTRL_SET_SCECNE_MODE:
                Rval = AmbaOD_SetSceneMode(Inst, 0U, pInfo);
                break;
            case SVC_CV_CTRL_ENABLE_TUNING_MODE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->TuningModeOn = 1U;
                pCtrl->TuningMode = (UINT8) *pValue;
                SvcLog_OK(SVC_LOG_AMBAOD, "Control[%d]: TuningMode = %u", Inst, *pValue);
                if (pCtrl->Started == 1U) {
                    Rval |= AmbaOD_ConfigTuningModeV2(Inst, 0U, pCtrl->TuningMode);
                }
                break;
            default:
                SvcLog_NG(SVC_LOG_AMBAOD, "Control[%d]: Invalid CtrlType(%d)", Inst, CtrlType);
                break;
        }
    } else {
        SvcLog_NG(SVC_LOG_AMBAOD, "Control: Invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    AmbaOD_MutexGive();

    return Rval;
}

/**
* CvFlow driver stop function
*/
static UINT32 CvFlow_AmbaODStop(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_AMBAOD_CTRL_s *pCtrl;
    SVC_CV_FLOW_ALGO_DELETE_CFG_s DelCfg;
    UINT32 i;

    AmbaMisra_TouchUnused(pInput);

    Rval = AmbaOD_CvfChan2Inst(CvFlowChan, &Inst);

    if (SVC_OK == Rval) {
        pCtrl = &g_AmbaODCtrl[Inst];

        Rval = AmbaOD_SetTaskIdle(Inst);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_AMBAOD, "SetTaskIdle error", 0U, 0U);
        }

        Rval = SvcTask_Destroy(&pCtrl->RxTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_AMBAOD, "Destroy Tx/Rx Task error", 0U, 0U);
        }

        Rval = AmbaWrap_memset(&DelCfg, 0, sizeof(SVC_CV_FLOW_ALGO_DELETE_CFG_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_AMBAOD, "memset DelCfg failed", 0U, 0U);
        }

        for (i = 0; i < pCtrl->NumValidAlgo; i++) {
            Rval |= SvcCvFlow_DeleteAlgo(SVC_LOG_AMBAOD, &DelCfg, &pCtrl->AlgoOp[i]);
        }
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_AMBAOD, "Delele Algo error", 0U, 0U);
        }

        for (i = 0; i < pCtrl->NumStage; i++) {
            Rval = AmbaKAL_MsgQueueDelete(&pCtrl->AlgoOutQueId[i]);
            Rval |= AmbaKAL_SemaphoreDelete(&pCtrl->OutBufSem[i]);
            if (KAL_ERR_NONE != Rval) {
                SvcLog_NG(SVC_LOG_AMBAOD, "Delete MsgQue/Semaphore error(0x%X)", Rval, 0U);
            }
        }

        /* Reset Ctrl Intance */
        Rval = AmbaWrap_memset(&g_AmbaODCtrl[Inst], 0, sizeof(SVC_AMBAOD_CTRL_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_AMBAOD, "memset g_AmbaODCtrl[%u] failed", Inst, 0U);
        }
    }

    SvcLog_OK(SVC_LOG_AMBAOD, "CvFlow_AmbaODStop: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver debug interface
*/
static UINT32 CvFlow_AmbaODDebug(UINT32 CvFlowChan, const char *pCmd, UINT32 Param0, UINT32 Param1, UINT32 Param2)
{
    UINT32 i;
    ULONG Addr;
    const SVC_AMBAOD_CTRL_s *pCtrl;
    const AMBA_CV_FLEXIDAG_HANDLE_s *pCvFDHandle;

    if (0 == SvcWrap_strcmp("debug", pCmd)) {
        SvcCvDebugEnable = (UINT8) Param0;
        SvcLog_DBG(SVC_LOG_AMBAOD, "Set debug(%u)", Param0, 0U);
    } else if (0 == SvcWrap_strcmp("info", pCmd)) {
        for (i = 0; i < SVC_AMBAOD_MAX_INSTANCE; i++) {
            pCtrl = &g_AmbaODCtrl[i];
            if (pCtrl->Used == 1U) {
                AmbaPrint_PrintUInt5("AmbaOD[%d]: CvfChan %d", i, pCtrl->CvfChan, 0U, 0U, 0U);
                pCvFDHandle = &pCtrl->AlgoOp[0].AlgoHandle.FDs[0].Handle;
                AmbaMisra_TypeCast(&Addr, &pCvFDHandle);
                SvcCvFlow_PrintULong("  CV_FLEXIDAG_HANDLE: 0x%x", Addr, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        SvcLog_DBG(SVC_LOG_AMBAOD, "Unsupported cmd", 0U, 0U);
    }

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(&Param0);
    AmbaMisra_TouchUnused(&Param1);
    AmbaMisra_TouchUnused(&Param2);

    return SVC_OK;
}

SVC_CV_FLOW_OBJ_s SvcCvFlow_AmbaODObj = {
    .Init         = CvFlow_AmbaODInit,
    .Config       = CvFlow_AmbaODConfig,
    .Load         = CvFlow_AmbaODLoad,
    .Start        = CvFlow_AmbaODStart,
    .Control      = CvFlow_AmbaODControl,
    .Stop         = CvFlow_AmbaODStop,
    .Debug        = CvFlow_AmbaODDebug,
};

