/**
 *  @file SvcCvFlow_Stereo.c
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
 *  @details Implementation of FlexiDAG based Reference Object Detection
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
#include "SvcCvFlow_Stereo.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcImgFIFO.h"
#include "SvcImgFeeder.h"
#include "SvcOsd.h"
#include "SvcWrap.h"

#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_ref_util.h"
#include "cvapi_flexidag_amba_spu_fex_wrap.h"
#include "cvapi_flexidag_amba_spu_fusion_wrap.h"

#include "cvapi_svccvalgo_ambaspufex.h"

#if defined(CONFIG_SOC_CV2)
#include "cvapi_flexidag_ambaspufex_cv2.h"
#include "cvapi_flexidag_ambaspufex_def_cv2.h"
#include "cvapi_flexidag_ambaspufusion_cv2.h"
#include "cvapi_flexidag_ambastereoerr_cv2.h"
#endif
#if defined(CONFIG_SOC_CV2FS)
#include "cvapi_flexidag_ambaspufex_cv2fs.h"
#include "cvapi_flexidag_ambaspufex_def_cv2fs.h"
#include "cvapi_flexidag_ambaspufusion_cv2fs.h"
#include "cvapi_flexidag_ambastereoerr_cv2fs.h"
#include "cvapi_flexidag_ambaoffex_cv2fs.h"
#endif


#ifdef CONFIG_AMBALINK_BOOT_OS
#include "AmbaIPC_FlexidagIO.h"
#include "SvcCvFlexidagIO.h"
#endif


#define SVC_LOG_STEREO        "Stereo"

#define SVC_STEREO_INSTANCE0                (0U)
#define SVC_STEREO_INSTANCE1                (1U)
#define SVC_STEREO_INSTANCE2                (2U)
#define SVC_STEREO_INSTANCE3                (3U)
#define SVC_STEREO_MAX_INSTANCE             (4U)

#define SVC_STEREO_SEM_INIT_COUNT           (1U)
#define SVC_STEREO_OUTPUT_DEPTH             (SVC_STEREO_SEM_INIT_COUNT + 1U)

#define SVC_STEREO_RX_TASK_STACK_SIZE       (0x4000)
#define SVC_STEREO_PICINFO_QUEUE_SIZE       (8U)
#define SVC_STEREO_ALGO_OUT_QUEUE_SIZE      (SVC_STEREO_OUTPUT_DEPTH)
#define SVC_STEREO_FUSIONIN_QUEUE_SIZE      (SVC_STEREO_OUTPUT_DEPTH)


#define STEREO_ALGO_IDX_0              (0U)
#define STEREO_ALGO_IDX_1              (1U)
#define STEREO_MAX_FLEXIDAG_NUM            (2U)


typedef UINT32 (*SVC_STEREO_DATA_HANDLER_f)(UINT32 Inst);
typedef UINT32 (*SVC_STEREO_TX_HANDLER_f)(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
typedef UINT32 (*SVC_STEREO_EVENT_HANDLER_f)(UINT32 Inst, STEREO_RX1_MSG_s *Msg);

typedef struct {
    flexidag_memblk_t           OutputBuf[SVC_STEREO_OUTPUT_DEPTH][FLEXIDAG_MAX_OUTPUTS];
    AMBA_CV_FLEXIDAG_IO_s       InBuf[SVC_STEREO_OUTPUT_DEPTH];
    AMBA_CV_FLEXIDAG_IO_s       OutBuf[SVC_STEREO_OUTPUT_DEPTH];
} SVC_CV_FLOW_STEREO_OP_EX_INFO_s;

typedef struct {
    UINT8                       Used;
    UINT8                       CvfChan;
    UINT8                       Enable;
    UINT8                       Started;
    UINT8                       SendConfig;
    UINT8                       SendFusionConfig;
    SVC_CV_FLOW_CFG_s           *pCvFlow;
    SVC_CV_FLOW_CFG_s           CvCfg;

    UINT8                       NumValidAlgo;
    SVC_CV_FLOW_ALGO_OP_INFO_s  AlgoOp[STEREO_MAX_ALGO_NUM];
    SVC_CV_FLOW_STEREO_OP_EX_INFO_s AlgoOpEx[STEREO_MAX_ALGO_NUM];
    void                       *pImgInfoBuf[SVC_STEREO_PICINFO_QUEUE_SIZE];
    void                       *pImgInfoPhyBuf[SVC_STEREO_PICINFO_QUEUE_SIZE];
    UINT32                      ImgInfoBufSize;

    /* Data source service */
    SVC_IMG_FEEDER_PORT_s       FeederPort;
    UINT32                      SrcSeqNum;
    SVC_STEREO_TX_HANDLER_f      pTxProc;
    UINT32                      TxSeqNum;
    UINT32                      PicInfoIdx;

    /* Data sink service */
    SVC_TASK_CTRL_s             RxTaskCtrl;
    SVC_STEREO_DATA_HANDLER_f    pRxProc;
    UINT32                      RxSeqNum;
    UINT32                      OutBufIdx;
    AMBA_KAL_SEMAPHORE_t        OutBufSem;
    UINT32                      SinkInfoIdx;


    /* Data 1 Event handler*/
    SVC_STEREO_EVENT_HANDLER_f pRx1EvtHandler;

    /* Data source 1 service */
    SVC_TASK_CTRL_s             Tx1TaskCtrl;
    SVC_STEREO_DATA_HANDLER_f      pTx1Proc;
    UINT32                      Tx1SeqNum;

    /* Data sink 1 service */
    SVC_TASK_CTRL_s             Rx1TaskCtrl;
    SVC_STEREO_DATA_HANDLER_f    pRx1Proc;
    UINT32                      Rx1SeqNum;
    UINT32                      Rx1OutBufIdx;
    AMBA_KAL_SEMAPHORE_t        Rx1OutBufSem;
    UINT32                      Rx1SinkInfoIdx;

    /* Fusion in */
    AMBA_KAL_MSG_QUEUE_t        FusionInQueId;

    /* Algo Callback */
    AMBA_KAL_MSG_QUEUE_t        SpuFexOutAlgoQueId;
    AMBA_KAL_MSG_QUEUE_t        FusionOutAlgoQueId;

    /* Misc */
    UINT32                      SrcRateCtrl;
    UINT8                       SrcInfoErr;
    UINT8                       SceneMode;
    UINT32                      FlexiIOChan;    /* for AmbaLink CV only */
} SVC_STEREO_CTRL_s;

typedef struct {
   UINT32 Event;
   SVC_CV_ALGO_OUTPUT_s AlgoOut;
} STEREO_OUTPUT_MSG_s;

static SVC_STEREO_CTRL_s g_StereoCtrl[SVC_STEREO_MAX_INSTANCE] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MUTEX_t StereoMutex;
static UINT8 SvcCvDebugEnable = 0;

static UINT32 CvFlow_StereoInit(UINT32 CvFlowChan, void *pInit);
static UINT32 CvFlow_StereoConfig(UINT32 CvFlowChan, void *pConfig);
static UINT32 CvFlow_StereoStart(UINT32 CvFlowChan, void *pInput);
static UINT32 CvFlow_StereoControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo);
static UINT32 CvFlow_StereoStop(UINT32 CvFlowChan, void *pInput);
static UINT32 Stereo_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst);

static UINT32 Stereo_CvDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 Stereo_CvDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 Stereo_CvDataSrcHandler2(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 Stereo_CvDataSrcHandler3(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

static UINT32 Stereo_SpuFexAlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 Stereo_SpuFexAlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 Stereo_SpuFexAlgoOutCallback2(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 Stereo_SpuFexAlgoOutCallback3(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);

static UINT32 Stereo_FusionAlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 Stereo_FusionAlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 Stereo_FusionAlgoOutCallback2(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 Stereo_FusionAlgoOutCallback3(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);


static UINT32 Stereo_SendConfig(UINT32 Inst, UINT32 AlgoIdx, const idsp_pyramid_t *pPyramidInfo);
static UINT32 Stereo_ConfigSpuFex(const SVC_STEREO_CTRL_s *pCtrl, UINT32 AlgoIdx, const idsp_pyramid_t *pPyramidInfo, const SVC_CV_FLOW_STEREO_SPU_CFG_s *pSpuCfg, const SVC_CV_FLOW_STEREO_FEX_CFG_s *pFexCfg);
#ifdef CONFIG_SOC_CV2FS
static UINT32 Stereo_SendFusionConfig(UINT32 Inst, UINT32 AlgoIdx);
static UINT32 Stereo_ConfigFusion(const SVC_STEREO_CTRL_s *pCtrl, UINT32 AlgoIdx, const SVC_CV_FLOW_STEREO_FUS_CFG_s *pFusionCfg);
#endif

static void SVC_CV_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcCvDebugEnable > 0U) {
        SvcLog_DBG(pModule, pFormat, Arg1, Arg2);
    }
}

static void Stereo_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&StereoMutex, 5000)) {
        SvcLog_NG(SVC_LOG_STEREO, "Stereo_MutexTake: timeout", 0U, 0U);
    }
}

static void Stereo_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&StereoMutex)) {
        SvcLog_NG(SVC_LOG_STEREO, "Stereo_MutexGive: error", 0U, 0U);
    }
}

static UINT32 Stereo_SemTake(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreTake(&g_StereoCtrl[Inst].OutBufSem, 10000)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_STEREO, "Stereo_SemTake[%d]: timeout", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 Stereo_SemGive(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreGive(&g_StereoCtrl[Inst].OutBufSem)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_STEREO, "Stereo_SemGive[%d]: error", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 Stereo_SemTakeRx1(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreTake(&g_StereoCtrl[Inst].Rx1OutBufSem, 10000)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_STEREO, "Stereo_SemTakeRx1[%d]: timeout", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 Stereo_SemGiveRx1(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreGive(&g_StereoCtrl[Inst].Rx1OutBufSem)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_STEREO, "Stereo_SemGiveRx1[%d]: error", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}


static UINT32 Stereo_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst)
{
    UINT32 Rval = SVC_NG;
    UINT32 i;

    if (NULL != pInst) {
        for (i = 0; i < SVC_STEREO_MAX_INSTANCE; i++) {
            if ((1U == g_StereoCtrl[i].Used) && (CvfChan == g_StereoCtrl[i].CvfChan)) {
                *pInst = i;
                Rval = SVC_OK;
                break;
            }
        }
    }
    return Rval;
}

#ifdef CONFIG_AMBALINK_BOOT_OS
static UINT32 Stereo_Init_Callback(UINT32 Channel, UINT32 OutType)
{
    UINT32 Rval = SVC_OK;

    (void) Channel;
    (void) OutType;
    AmbaPrint_PrintStr5("Stereo_Init_Callback", NULL, NULL, NULL, NULL, NULL);

    return Rval;
}
#endif

static void Stereo_DumpPicInfo(const cv_pic_info_t *pPicInfo)
{
    static UINT32 DumpTimes = 1U;
    extern void SvcCvFlow_DumpPicInfo(const char *pModuleName, const cv_pic_info_t *pPicInfo);

    if (DumpTimes > 0U) {
        SvcCvFlow_DumpPicInfo(SVC_LOG_STEREO, pPicInfo);
        DumpTimes--;
    }
}

/**
* Pack data source to pic info structure
*/
static void Stereo_PackPicInfo(UINT32 Inst, const SVC_CV_INPUT_IMG_INFO_s *pInputImg, memio_source_recv_picinfo_t *pInfo, memio_source_recv_picinfo_t *pInfoPhy)
{
    UINT32 Rval = SVC_OK;
    cv_pic_info_t *pPicInfo = &pInfo->pic_info;
    cv_pic_info_t *pPicInfoPhy = &pInfoPhy->pic_info;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    const SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[Inst];
    UINT32 DataSrc, RelativeAddrOn = 1U;
    extern UINT32 SvcCvFlow_PackPicInfo_Pyramid(cv_pic_info_t *pPicInfo, const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo, UINT32 MainWidth, UINT32 MainHeight, UINT32 UseRelative, UINT32 LeftRight);
    extern UINT32 SvcCvFlow_PackPicInfo_MainYuv(cv_pic_info_t *pPicInfo, const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo, UINT32 UseRelative, UINT32 LeftRight);

    Rval = AmbaWrap_memset(pInfo, 0, sizeof(memio_source_recv_picinfo_t));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STEREO, "memset picinfo failed", 0U, 0U);
    }

    Rval = AmbaWrap_memset(pPicInfoPhy, 0, sizeof(memio_source_recv_picinfo_t));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STEREO, "memset picinfo failed", 0U, 0U);
    }

    DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
    if (pInputImg->NumInfo > 0U) {
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
            if (SvcCvFlow_Stereo_ModeInfo[pCtrl->pCvFlow->CvModeID].ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
                RelativeAddrOn = 0U;
            }
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

            Rval |= SvcCvFlow_PackPicInfo_Pyramid(pPicInfoPhy,
                                                 pYuvInfoLeft,
                                                 pCfg->FovCfg[pYuvInfoLeft->ViewZoneId].MainWin.Width,
                                                 pCfg->FovCfg[pYuvInfoLeft->ViewZoneId].MainWin.Height,
                                                 0U, 1U);
            Rval |= SvcCvFlow_PackPicInfo_Pyramid(pPicInfoPhy,
                                                 pYuvInfoRight,
                                                 pCfg->FovCfg[pYuvInfoRight->ViewZoneId].MainWin.Width,
                                                 pCfg->FovCfg[pYuvInfoRight->ViewZoneId].MainWin.Height,
                                                 0U, 2U);
        }
    } else {
        Rval = SVC_NG;
    }

    Stereo_DumpPicInfo(pPicInfo);

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STEREO, "Stereo_PackPicInfo error", 0U, 0U);
    }
}

/**
* Send message to Feeder to update port status
*/
static void Stereo_SendReceiverReady(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort)
{
    SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[Inst];
    AMBA_KAL_SEMAPHORE_INFO_s SemInfo;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreQuery(&pCtrl->OutBufSem, &SemInfo)) {
        if (SemInfo.CurrentCount > 0U) {
            if (SVC_OK != SvcImgFeeder_MsgToFeeder(pPort, SVC_IMG_FEEDER_RECEIVER_READY, NULL)) {
                SvcLog_NG(SVC_LOG_STEREO, "Send ReceiverReady[%u] error", Inst, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO, "ReceiverReady[%u]: SemQuery error", Inst, 0U);
    }
}

/**
* TX data source handler: process input data
*/
//CV Pic input
static UINT32 Stereo_CvDataSrcHandler(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[Inst];
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
    const SVC_CV_INPUT_IMG_INFO_s *pInputImg;
    memio_source_recv_picinfo_t *pPicInfo;
    memio_source_recv_picinfo_t *pPicInfoPhy;
    UINT32 i;
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;
    AMBA_CV_FLEXIDAG_IO_s *pInBuf, *pOutBuf;
    const SVC_CV_FLOW_STEREO_MODE_INFO_s *pModeInfo = &SvcCvFlow_Stereo_ModeInfo[pCtrl->pCvFlow->CvModeID];
    AmbaMisra_TouchUnused(pInfo);

    SVC_CV_DBG(SVC_LOG_STEREO, "Stereo_CvDataSrcHandler[%u]: run %d", Inst, pCtrl->TxSeqNum);
    if ((pCtrl->Enable == 1U) &&
        (0U == (pCtrl->SrcSeqNum % pCtrl->SrcRateCtrl))) {
        Rval = Stereo_SemTake(Inst);
        if (SVC_OK == Rval) {
            AmbaMisra_TypeCast(&pPicInfo, &pCtrl->pImgInfoBuf[pCtrl->PicInfoIdx]);
            AmbaMisra_TypeCast(&pPicInfoPhy, &pCtrl->pImgInfoPhyBuf[pCtrl->PicInfoIdx]);
            if (Event == SVC_IMG_FEEDER_IMAGE_DATA_READY) {
                AmbaMisra_TypeCast(&pInputImg, &pInfo);
                Stereo_PackPicInfo(Inst, pInputImg, pPicInfo, pPicInfoPhy);
            } else if (Event == SVC_IMG_FEEDER_PICINFO_READY) {
                Rval = AmbaWrap_memcpy(pPicInfo, pInfo, sizeof(memio_source_recv_picinfo_t));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STEREO, "memcpy picinfo failed", 0U, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_STEREO, "Stereo_CvDataSrcHandler: Unsupport Event", 0U, 0U);
            }
            /* Prepare feed config */
            Rval = AmbaWrap_memset(&FeedCfg, 0, sizeof(SVC_CV_ALGO_FEED_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STEREO, "memset FeedCfg failed", 0U, 0U);
            }

            pAlgoOp = &pCtrl->AlgoOp[STEREO_ALGO_IDX_0];
            pInBuf = &pCtrl->AlgoOpEx[STEREO_ALGO_IDX_0].InBuf[pCtrl->OutBufIdx];
            pOutBuf = &pCtrl->AlgoOpEx[STEREO_ALGO_IDX_0].OutBuf[pCtrl->OutBufIdx];

            if (pCtrl->SendConfig == 1U) {
                Rval = Stereo_SendConfig(Inst, STEREO_ALGO_IDX_0, &pPicInfo->pic_info.pyramid);
                pCtrl->SendConfig = 0U;
            }

            if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
#ifdef CONFIG_AMBALINK_BOOT_OS
                const UINT32 *pAmbaIPCChannel = &pCtrl->FlexiIOChan;
                AmbaMisra_TypeCast(&FeedCfg.pExtFeedCfg, &pAmbaIPCChannel);
#else
                SvcLog_NG(SVC_LOG_STEREO, "CONFIG_AMBALINK_BOOT_OS not defined ", 0U, 0U);
#endif
            } else {
                AmbaMisra_TypeCast(&FeedCfg.pExtFeedCfg, &pPicInfo); /* SCA_OpenOD */
                AmbaMisra_TypeCast(&FeedCfg.pUserData, &pPicInfoPhy);
            }

            FeedCfg.pIn  = pInBuf;
            FeedCfg.pOut = pOutBuf;
            pInBuf->num_of_buf = 1U;

            SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STEREO, pPicInfo, pCtrl->ImgInfoBufSize, &pInBuf->buf[0]);
            pOutBuf->num_of_buf = pAlgoOp->OutputNum;
            for (i = 0U; i < pAlgoOp->OutputNum; i++) {
                Rval = AmbaWrap_memcpy(&pOutBuf->buf[i], &pCtrl->AlgoOpEx[STEREO_ALGO_IDX_0].OutputBuf[pCtrl->OutBufIdx][i], sizeof(flexidag_memblk_t));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STEREO, "memcpy OutputBufBlk failed", 0U, 0U);
                }
            }

            Rval |= SvcCvAlgo_Feed(&pAlgoOp->AlgoHandle, &FeedCfg);
            SVC_CV_DBG(SVC_LOG_STEREO, "Stereo_CvDataSrcHandler[%u]: Send(%u)", Inst, pPicInfo->pic_info.frame_num);

            if (SVC_OK == Rval) {
                pCtrl->TxSeqNum++;
                pCtrl->PicInfoIdx++;
                if (pCtrl->PicInfoIdx >= SVC_STEREO_PICINFO_QUEUE_SIZE) {
                    pCtrl->PicInfoIdx = 0;
                }
                pCtrl->OutBufIdx++;
                if (pCtrl->OutBufIdx >= SVC_STEREO_OUTPUT_DEPTH) {
                    pCtrl->OutBufIdx = 0;
                }
            } else {
                SvcLog_NG(SVC_LOG_STEREO, "Stereo_CvDataSrcHandler[%u]: send error", Inst, 0U);
            }

            if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                Stereo_SendReceiverReady(Inst, pPort);
            }
        } else {
            SvcLog_NG(SVC_LOG_STEREO, "Stereo_CvDataSrcHandler[%u]: SemTake error", Inst, 0U);
        }
    } else {
        if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
            Stereo_SendReceiverReady(Inst, pPort);
            Rval = AmbaKAL_TaskSleep(1);
        }
    }

        pCtrl->SrcSeqNum++;
    SVC_CV_DBG(SVC_LOG_STEREO, "Stereo_CvDataSrcHandler[%u]: end", Inst, 0U);

    return Rval;
}

static UINT32 Stereo_CvDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[SVC_STEREO_INSTANCE0];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_STEREO_INSTANCE0, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 Stereo_CvDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[SVC_STEREO_INSTANCE1];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_STEREO_INSTANCE1, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 Stereo_CvDataSrcHandler2(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[SVC_STEREO_INSTANCE2];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_STEREO_INSTANCE2, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 Stereo_CvDataSrcHandler3(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[SVC_STEREO_INSTANCE3];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_STEREO_INSTANCE3, pPort, Event, pInfo);
    }
    return Rval;
}

/**
* Algorithm output event callback function
*/
static UINT32 Stereo_SpuFexAlgoOutCallback(UINT32 Inst, UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Rval = SVC_OK;
    SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[Inst];
    STEREO_OUTPUT_MSG_s Msg;
    UINT32 Valid = 1U;

    if (Event == CALLBACK_EVENT_OUTPUT) {
    } else {
        Valid = 0U;
    }

    if (Valid == 1U) {
        Msg.Event = Event;
        if (NULL != pEventData) {
            Rval = AmbaWrap_memcpy(&Msg.AlgoOut, pEventData, sizeof(SVC_CV_ALGO_OUTPUT_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STEREO, "memcpy AlgoOut failed", 0U, 0U);
            }
        }
        Rval = AmbaKAL_MsgQueueSend(&pCtrl->SpuFexOutAlgoQueId, &Msg, 5000);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STEREO, "Stereo_SpuFexAlgoOutCallback[%u]: MsgQueueSend error", Inst, 0U);
        }
    }

    return Rval;
}

static UINT32 Stereo_SpuFexAlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Stereo_SpuFexAlgoOutCallback(SVC_STEREO_INSTANCE0, Event, pEventData);
}

static UINT32 Stereo_SpuFexAlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Stereo_SpuFexAlgoOutCallback(SVC_STEREO_INSTANCE1, Event, pEventData);
}

static UINT32 Stereo_SpuFexAlgoOutCallback2(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Stereo_SpuFexAlgoOutCallback(SVC_STEREO_INSTANCE2, Event, pEventData);
}

static UINT32 Stereo_SpuFexAlgoOutCallback3(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Stereo_SpuFexAlgoOutCallback(SVC_STEREO_INSTANCE3, Event, pEventData);
}

/**
* RX data sink handler: process detection output
*/
//SPU+FEX
static UINT32 Stereo_SpuFexDataSinkHandler(UINT32 Inst)
{
    UINT32 Rval;
    SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[Inst];
    const memio_source_recv_picinfo_t *pInPicInfo = NULL;
    STEREO_OUTPUT_MSG_s Msg;
    STEREO_RX1_MSG_s Rx1Msg;
    const SVC_CV_ALGO_OUTPUT_s *pAlgoOut = &Msg.AlgoOut;
    static REF_CV_SPU_FEX_WRAP_OUTPUT_DATA_s StereoData;
    REF_CV_SPU_FEX_WRAP_OUTPUT_DATA_s *pStereoData = &StereoData;
    static SVC_CV_STEREO_OUTPUT_s Output[SVC_STEREO_MAX_INSTANCE][SVC_STEREO_OUTPUT_DEPTH] GNU_SECTION_NOZEROINIT;
    SVC_CV_STEREO_OUTPUT_s *pOutout;
    ULONG ResultAddr;
    const SVC_CV_FLOW_STEREO_MODE_INFO_s *pModeInfo = &SvcCvFlow_Stereo_ModeInfo[pCtrl->pCvFlow->CvModeID];

    SVC_CV_DBG(SVC_LOG_STEREO, "Rx[%u]: run %d", Inst, pCtrl->RxSeqNum);

    Rval = AmbaKAL_MsgQueueReceive(&pCtrl->SpuFexOutAlgoQueId, &Msg, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK == Rval) {
        SVC_CV_DBG(SVC_LOG_STEREO, "Rx[%u]: recv out(%u)", Inst, 0);

        if (Msg.Event == CALLBACK_EVENT_OUTPUT) {
            {
                AmbaMisra_TypeCast(&pStereoData->pOutSpu, &pAlgoOut->pOutput->buf[0].pBuffer);
                AmbaMisra_TypeCast(&pStereoData->pOutFex, &pAlgoOut->pOutput->buf[1].pBuffer);

                if (NULL != pAlgoOut->pUserData) {
                    AmbaMisra_TypeCast(&pInPicInfo, &pAlgoOut->pUserData);
                }

                AmbaMisra_TypeCast(&ResultAddr, &pStereoData);
                pOutout = &Output[Inst][pCtrl->SinkInfoIdx];
                pOutout->MsgCode    = SVC_CV_STEREO_OUTPUT;
                pOutout->Type       = STEREO_OUT_TYPE_SPU_FEX;
                pOutout->StereoID   = 0;
                pOutout->FrameSetID = 0;
                pOutout->DataAddr   = ResultAddr;   /* REF_CV_SPU_FEX_WRAP_OUTPUT_DATA_s */
                AmbaMisra_TypeCast(&pOutout->pPicInfo, &pInPicInfo);

                if (NULL != pAlgoOut->pUserData) {
                    pOutout->FrameNum = pInPicInfo->pic_info.frame_num;
                }

                if (pCtrl->Enable == 1U) {
                    Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                                SVC_CV_FLOW_OUTPUT_PORT0,
                                                pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                                pOutout);
                }

                /* Send Result to RX1 for Fusion*/
                if (pModeInfo->EnableFusion == 1U){
                    if (pCtrl->pRx1EvtHandler != NULL) {
                        Rx1Msg.pStereoOut = pOutout;
                        AmbaMisra_TypeCast(&Rx1Msg.pPicInfo, &pInPicInfo);
                        Rval = pCtrl->pRx1EvtHandler(Inst, &Rx1Msg);
                    }
                }
                if (SVC_OK == Rval) {
                    pCtrl->SinkInfoIdx++;
                    if (pCtrl->SinkInfoIdx >= SVC_STEREO_OUTPUT_DEPTH) {
                        pCtrl->SinkInfoIdx = 0;
                    }
                }
            }

            Rval = Stereo_SemGive(Inst);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STEREO, "RxDataSinkHandler[%u]: SemGive error", Inst, 0U);
            }

            if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                Stereo_SendReceiverReady(Inst, &pCtrl->FeederPort);
            }
        }

        pCtrl->RxSeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_STEREO, "Rx[%u]: recv error", Inst, 0U);
    }

    SVC_CV_DBG(SVC_LOG_STEREO, "Rx[%u]: end", Inst, 0U);

    return SVC_OK;
}

/**
* Task entry of RX process
*/
static void* Stereo_SpuFexOutTaskEntry(void* EntryArg)
{
    UINT32 Rval = SVC_OK;
    const SVC_STEREO_CTRL_s *pCtrl;
    const ULONG *pArg;
    ULONG Inst;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Inst = (*pArg);
    pCtrl = &g_StereoCtrl[Inst];

    SvcLog_DBG(SVC_LOG_STEREO, "Stereo_SpuFexOutTaskEntry[%d] start", Inst, 0U);

    while (SVC_OK == Rval) {
        if (NULL != pCtrl->pRxProc) {
            Rval = pCtrl->pRxProc(Inst);
        } else {
            Rval = SVC_NG;
        }
    }

    SvcLog_DBG(SVC_LOG_STEREO, "Stereo_SpuFexOutTaskEntry[%d] exit", Inst, 0U);

    return NULL;
}


/**
* TX1 data source handler: process input data
*/
//CV Pic input
static UINT32 Stereo_FusionEventHandler(UINT32 Inst, STEREO_RX1_MSG_s *Msg)
{
    UINT32 Rval = SVC_OK;
    SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[Inst];

    Rval = AmbaKAL_MsgQueueSend(&pCtrl->FusionInQueId, Msg, 5000);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STEREO, "Stereo_FusionDataSrcHandler[%u]: MsgQueueSend error", Inst, 0U);
    }

    return Rval;
}

static inline UINT32 _CacheAligned (UINT32 Val) {return (((Val) + (128U - 1U)) & ~(128U - 1U));}

/**
* TX1 data source handler: process input data
*/
static UINT32 Stereo_FusionDataSrcHandler(UINT32 Inst)
{
    UINT32 Rval, i;
    SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[Inst];
    const REF_CV_SPU_FEX_WRAP_OUTPUT_DATA_s *pStereoOutData;
    STEREO_RX1_MSG_s Rx1Msg = {NULL};
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;
    AMBA_CV_FLEXIDAG_IO_s *pInBuf, *pOutBuf;

    SVC_CV_DBG(SVC_LOG_STEREO, "Stereo_FusionDataSrcHandler[%u]: run %d", Inst, pCtrl->RxSeqNum);

    Rval = AmbaKAL_MsgQueueReceive(&pCtrl->FusionInQueId, &Rx1Msg, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK == Rval) {
        SVC_CV_DBG(SVC_LOG_STEREO, "Stereo_FusionDataSrcHandler[%u]: recv out(%u)", Inst, 0);

        Rval = Stereo_SemTakeRx1(Inst);
        if (SVC_OK == Rval) {

            AmbaMisra_TypeCast(&pStereoOutData, &Rx1Msg.pStereoOut->DataAddr);

            /* Prepare feed config */
            Rval = AmbaWrap_memset(&FeedCfg, 0, sizeof(SVC_CV_ALGO_FEED_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STEREO, "memset FeedCfg failed", 0U, 0U);
            }

            pAlgoOp = &pCtrl->AlgoOp[STEREO_ALGO_IDX_1];
            pInBuf = &pCtrl->AlgoOpEx[STEREO_ALGO_IDX_1].InBuf[pCtrl->OutBufIdx];
            pOutBuf = &pCtrl->AlgoOpEx[STEREO_ALGO_IDX_1].OutBuf[pCtrl->OutBufIdx];
#ifdef CONFIG_SOC_CV2FS
            if (pCtrl->SendFusionConfig == 1U) {
                Rval = Stereo_SendFusionConfig(Inst, STEREO_ALGO_IDX_1);
                pCtrl->SendFusionConfig = 0U;
            }
#endif
            AmbaMisra_TypeCast(&FeedCfg.pUserData, &Rx1Msg.pPicInfo);
            FeedCfg.pIn  = pInBuf;
            FeedCfg.pOut = pOutBuf;
            pInBuf->num_of_buf = 2U;

            SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STEREO, pStereoOutData->pOutSpu, _CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)), &pInBuf->buf[0]);
            SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STEREO, Rx1Msg.pPicInfo, sizeof(memio_source_recv_picinfo_t), &pInBuf->buf[1]);

            pOutBuf->num_of_buf = pAlgoOp->OutputNum;
            for (i = 0U; i < pAlgoOp->OutputNum; i++) {
                Rval = AmbaWrap_memcpy(&pOutBuf->buf[i], &pCtrl->AlgoOpEx[STEREO_ALGO_IDX_1].OutputBuf[pCtrl->OutBufIdx][i], sizeof(flexidag_memblk_t));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STEREO, "memcpy OutputBufBlk failed", 0U, 0U);
                }
            }

            Rval |= SvcCvAlgo_Feed(&pAlgoOp->AlgoHandle, &FeedCfg);
            SVC_CV_DBG(SVC_LOG_STEREO, "Stereo_FusionDataSrcHandler[%u]: Send(%u)", Inst, Rx1Msg.pPicInfo->pic_info.frame_num);

            if (0U == Rval) {
                pCtrl->Rx1OutBufIdx++;
                if (pCtrl->Rx1OutBufIdx >= SVC_STEREO_OUTPUT_DEPTH) {
                    pCtrl->Rx1OutBufIdx = 0;
                }
            } else {
                SvcLog_NG(SVC_LOG_STEREO, "Stereo_FusionDataSrcHandler[%u]: send error", Inst, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_STEREO, "Stereo_FusionDataSrcHandler[%u]: SemTake error", Inst, 0U);
        }
        pCtrl->Rx1SeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_STEREO, "Stereo_FusionDataSinkHandler[%u]: recv error", Inst, 0U);
    }

    SVC_CV_DBG(SVC_LOG_STEREO, "Stereo_FusionDataSinkHandler[%u]: end", Inst, 0U);

    return SVC_OK;
}

/**
* Task entry of TX1 process
*/
static void* Stereo_FusionInTaskEntry(void* EntryArg)
{
    UINT32 Rval = SVC_OK;
    const SVC_STEREO_CTRL_s *pCtrl;
    const ULONG *pArg;
    ULONG Inst;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Inst = (*pArg);
    pCtrl = &g_StereoCtrl[Inst];

    SvcLog_DBG(SVC_LOG_STEREO, "Stereo_FusionInTaskEntry[%d] start", Inst, 0U);

    while (SVC_OK == Rval) {
        if (NULL != pCtrl->pTx1Proc) {
            Rval = pCtrl->pTx1Proc(Inst);
        } else {
            Rval = SVC_NG;
        }
    }

    SvcLog_DBG(SVC_LOG_STEREO, "Stereo_FusionInTaskEntry[%d] exit", Inst, 0U);

    return NULL;
}


/**
* Algorithm output event callback function
*/
static UINT32 Stereo_FusionAlgoOutCallback(UINT32 Inst, UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Rval = SVC_OK;
    SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[Inst];
    STEREO_OUTPUT_MSG_s Msg;
    UINT32 Valid = 1U;

    if (Event == CALLBACK_EVENT_OUTPUT) {
    } else {
        Valid = 0U;
    }

    if (Valid == 1U) {
        Msg.Event = Event;
        if (NULL != pEventData) {
            Rval = AmbaWrap_memcpy(&Msg.AlgoOut, pEventData, sizeof(SVC_CV_ALGO_OUTPUT_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STEREO, "memcpy AlgoOut failed", 0U, 0U);
            }
        }
        Rval = AmbaKAL_MsgQueueSend(&pCtrl->FusionOutAlgoQueId, &Msg, 5000);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STEREO, "Stereo_AlgoOutCallbackRX[%u]: MsgQueueSend error", Inst, 0U);
        }
    }

    return Rval;
}

static UINT32 Stereo_FusionAlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Stereo_FusionAlgoOutCallback(SVC_STEREO_INSTANCE0, Event, pEventData);
}

static UINT32 Stereo_FusionAlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Stereo_FusionAlgoOutCallback(SVC_STEREO_INSTANCE1, Event, pEventData);
}

static UINT32 Stereo_FusionAlgoOutCallback2(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Stereo_FusionAlgoOutCallback(SVC_STEREO_INSTANCE2, Event, pEventData);
}

static UINT32 Stereo_FusionAlgoOutCallback3(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Stereo_FusionAlgoOutCallback(SVC_STEREO_INSTANCE3, Event, pEventData);
}
//Fusion
static UINT32 Stereo_FusionDataSinkHandler(UINT32 Inst)
{
    UINT32 Rval;
    SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[Inst];
    STEREO_OUTPUT_MSG_s Msg;
    const SVC_CV_ALGO_OUTPUT_s *pAlgoOut = &Msg.AlgoOut;
    static AMBA_CV_SPU_BUF_s OutSpu;
    const AMBA_CV_SPU_BUF_s *pOutSpu = &OutSpu;
    const memio_source_recv_picinfo_t *pInPicInfo = NULL;
    static SVC_CV_STEREO_OUTPUT_s Output[SVC_STEREO_MAX_INSTANCE][SVC_STEREO_OUTPUT_DEPTH] GNU_SECTION_NOZEROINIT;
    SVC_CV_STEREO_OUTPUT_s *pOutout;
    ULONG ResultAddr;

    SVC_CV_DBG(SVC_LOG_STEREO, "Stereo_FusionDataSinkHandler[%u]: run %d", Inst, pCtrl->RxSeqNum);

    Rval = AmbaKAL_MsgQueueReceive(&pCtrl->FusionOutAlgoQueId, &Msg, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK == Rval) {
        SVC_CV_DBG(SVC_LOG_STEREO, "Stereo_FusionDataSinkHandler[%u]: recv out(%u)", Inst, 0);

        if (Msg.Event == CALLBACK_EVENT_OUTPUT) {
            if (pCtrl->Enable == 1U) {
                Rval = AmbaWrap_memcpy(&OutSpu, &pAlgoOut->pOutput->buf[0], sizeof(AMBA_CV_SPU_BUF_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STEREO, "memcpy OutSpu failed", 0U, 0U);
                }


                if (NULL != pAlgoOut->pUserData) {
                    AmbaMisra_TypeCast(&pInPicInfo, &pAlgoOut->pUserData);
                }

                AmbaMisra_TypeCast(&ResultAddr, &pOutSpu);
                pOutout = &Output[Inst][pCtrl->Rx1SinkInfoIdx];
                pOutout->MsgCode    = SVC_CV_STEREO_OUTPUT;
                pOutout->Type       = STEREO_OUT_TYPE_FUSION;
                pOutout->StereoID   = 0;
                pOutout->FrameSetID = 0;
                pOutout->DataAddr   = ResultAddr;   /* AMBA_CV_SPU_BUF_s */
                AmbaMisra_TypeCast(&pOutout->pPicInfo, &pInPicInfo);
                if (NULL != pAlgoOut->pUserData) {
                    pOutout->FrameNum = pInPicInfo->pic_info.frame_num;
                }

                Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                             SVC_CV_FLOW_OUTPUT_PORT0,
                                             pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                             pOutout);

                if (SVC_OK == Rval) {
                    pCtrl->Rx1SinkInfoIdx++;
                    if (pCtrl->Rx1SinkInfoIdx >= SVC_STEREO_OUTPUT_DEPTH) {
                        pCtrl->Rx1SinkInfoIdx = 0;
                    }
                }
            }
        }
        Rval = Stereo_SemGiveRx1(Inst);
        pCtrl->Rx1SeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_STEREO, "Stereo_FusionDataSinkHandler[%u]: recv error", Inst, 0U);
    }

    SVC_CV_DBG(SVC_LOG_STEREO, "Stereo_FusionDataSinkHandler[%u]: end", Inst, 0U);

    return Rval;
}

/**
* Task entry of RX1 process
*/
static void* Stereo_FusionOutTaskEntry(void* EntryArg)
{
    UINT32 Rval = SVC_OK;
    const SVC_STEREO_CTRL_s *pCtrl;
    const ULONG *pArg;
    ULONG Inst;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Inst = (*pArg);
    pCtrl = &g_StereoCtrl[Inst];

    SvcLog_DBG(SVC_LOG_STEREO, "Stereo_FusionOutTaskEntry[%d] start", Inst, 0U);

    while (SVC_OK == Rval) {
        if (NULL != pCtrl->pRx1Proc) {
            Rval = pCtrl->pRx1Proc(Inst);
        } else {
            Rval = SVC_NG;
        }
    }

    SvcLog_DBG(SVC_LOG_STEREO, "Stereo_FusionOutTaskEntry[%d] exit", Inst, 0U);

    return NULL;
}

/**
* Set and wait TX and RX task to idle state
*/
static UINT32 Stereo_SetTaskIdle(UINT32 Inst)
{
    UINT32 Rval;
    SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[Inst];
    UINT32 i;

    /* Disable input and output */
    pCtrl->Enable = 0U;

    if (SVC_OK != SvcImgFeeder_Disconnect(&pCtrl->FeederPort)) {
        SvcLog_NG(SVC_LOG_STEREO, "Disconn feeder port error", 0U, 0U);
    }

    if (SVC_OK != SvcImgFeeder_ClosePort(&pCtrl->FeederPort)) {
        SvcLog_NG(SVC_LOG_STEREO, "Close feeder port error", 0U, 0U);
    }

    /* Wait Rx idle */
    for (i = 0; i < SVC_STEREO_SEM_INIT_COUNT; i++) {
        Rval = Stereo_SemTake(Inst);
    }

    /* Wait Rx idle */
    for (i = 0; i < SVC_STEREO_SEM_INIT_COUNT; i++) {
        Rval = Stereo_SemTakeRx1(Inst);
    }

    return Rval;
}

/**
* Send external YUV to driver
*/
static UINT32 Stereo_SendExtYUV(UINT32 Inst, const void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[Inst];
    void *pInputImg; /* SVC_CV_INPUT_IMG_INFO_s */

    if (NULL != pCtrl->pTxProc) {
        AmbaMisra_TypeCast(&pInputImg, &pInfo);
        Rval = pCtrl->pTxProc(Inst, &pCtrl->FeederPort, SVC_IMG_FEEDER_IMAGE_DATA_READY, pInputImg);
    }

    return Rval;
}

/**
* Send external YUV to driver
*/
static UINT32 Stereo_SendExtPicInfo(UINT32 Inst, const void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[Inst];
    void *pInputImg; /* memio_source_recv_picinfo_t */

    if (NULL != pCtrl->pTxProc) {
        AmbaMisra_TypeCast(&pInputImg, &pInfo);
        Rval = pCtrl->pTxProc(Inst, &pCtrl->FeederPort, SVC_IMG_FEEDER_PICINFO_READY, pInputImg);
    }

    return Rval;
}

/**
* Send Fusion to driver
*/
static UINT32 Stereo_SendFusion(UINT32 Inst, const void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[Inst];
    static STEREO_RX1_MSG_s Rx1Msg;
    STEREO_RX1_MSG_s *pRx1Msg = &Rx1Msg;

    if (NULL != pCtrl->pRx1EvtHandler) {
        AmbaMisra_TypeCast(&pRx1Msg, &pInfo);
        Rval = pCtrl->pRx1EvtHandler(Inst, pRx1Msg);
    }

    return Rval;
}

/**
* Entry of run-time configuration function
*/
static UINT32 Stereo_SendConfig(UINT32 Inst, UINT32 AlgoIdx, const idsp_pyramid_t *pPyramidInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[Inst];
    const SVC_CV_FLOW_STEREO_MODE_INFO_s *pModeInfo = &SvcCvFlow_Stereo_ModeInfo[pCtrl->pCvFlow->CvModeID];

    if (pModeInfo->SpuCfg.NumScale > 0U) {
        Rval |= Stereo_ConfigSpuFex(pCtrl, AlgoIdx, pPyramidInfo, &pModeInfo->SpuCfg, &pModeInfo->FexCfg);
    }

    return Rval;
}

static UINT32 Stereo_ConfigSpuFex(const SVC_STEREO_CTRL_s *pCtrl, UINT32 AlgoIdx, const idsp_pyramid_t *pPyramidInfo, const SVC_CV_FLOW_STEREO_SPU_CFG_s *pSpuCfg, const SVC_CV_FLOW_STEREO_FEX_CFG_s *pFexCfg)
{
    UINT32 Rval = SVC_NG;
    UINT32 i = 0;
    AMBA_CV_SPUFEX_CFG_s Cfg;
    const SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s *pSpuScaleInfo;
    const SVC_CV_FLOW_STEREO_FEX_SCALE_INFO_s *pFexScaleInfo;
    AMBA_CV_SPU_SCALE_CFG_s *pSpuScaleCfg;
    AMBA_CV_FEX_SCALE_CFG_s *pFexScaleCfg;
    const AMBA_CV_SPUFEX_CFG_s *pCfg = &Cfg;
    const SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &pCtrl->AlgoOp[AlgoIdx];
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp1;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;
#ifdef CONFIG_SOC_CV2
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg2;
    UINT32 SpuDisplayMode = AMBA_SCA_CV_SPU_DISPLAY_MODE;
    const UINT32 *pSpuDisplayMode = &SpuDisplayMode;
#endif
    (void)pCtrl;

    if ((pSpuCfg != NULL) && (pFexCfg != NULL)) {
        /* SPU Config */
        Rval = AmbaWrap_memset(Cfg.SpuScaleCfg, 0, sizeof(Cfg.SpuScaleCfg));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STEREO, "memset SpuScaleCfg failed", 0U, 0U);
        }

        if (pSpuCfg->pScaleInfo != NULL) {
            for (i = 0; i < pSpuCfg->NumScale; i++) {
                pSpuScaleInfo = &pSpuCfg->pScaleInfo[i];
                if (pSpuScaleInfo->ScaleIdx < STEREO_FDAG_NUM_SPU_FEX_SCALE) {
                    pSpuScaleCfg = &Cfg.SpuScaleCfg[pSpuScaleInfo->ScaleIdx];//
                    pSpuScaleCfg->Enable          = pSpuScaleInfo->Enable;
                    pSpuScaleCfg->RoiEnable       = pSpuScaleInfo->RoiEnable;
                    pSpuScaleCfg->FusionDisparity = pSpuScaleInfo->FusionDisparity;
                    if (pSpuScaleInfo->RoiEnable == 1U) {
                        pSpuScaleCfg->Roi.m_start_col = pSpuScaleInfo->StartCol;
                        pSpuScaleCfg->Roi.m_start_row = pSpuScaleInfo->StartRow;
                        pSpuScaleCfg->Roi.m_width_m1  = pSpuScaleInfo->RoiWidth - 1U;
                        pSpuScaleCfg->Roi.m_height_m1 = pSpuScaleInfo->RoiHeight - 1U;
                    }
                    Rval = SVC_OK;
                }
            }
        }

        /* FEX Config */
        Rval = AmbaWrap_memset(Cfg.FexScaleCfg, 0, sizeof(Cfg.FexScaleCfg));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STEREO, "memset FexScaleCfg failed", 0U, 0U);
        }

        if (pFexCfg->pScaleInfo != NULL) {
            for (i = 0; i < pFexCfg->NumScale; i++) {
                pFexScaleInfo = &pFexCfg->pScaleInfo[i];
                if (pFexScaleInfo->ScaleIdx < STEREO_FDAG_NUM_SPU_FEX_SCALE) {
                    pFexScaleCfg = &Cfg.FexScaleCfg[pFexScaleInfo->ScaleIdx];//
                    pFexScaleCfg->Enable                = pFexScaleInfo->Enable;
                    pFexScaleCfg->RoiEnable             = pFexScaleInfo->RoiEnable;
                    pFexScaleCfg->SecondaryRoiColOffset = (INT16)pFexScaleInfo->SecondaryRoiColOffset;
                    pFexScaleCfg->SecondaryRoiRowOffset = (INT16)pFexScaleInfo->SecondaryRoiRowOffset;
                    if (pFexScaleInfo->RoiEnable == 1U) {
                        pFexScaleCfg->Roi.m_start_col = pFexScaleInfo->StartCol;
                        pFexScaleCfg->Roi.m_start_row = pFexScaleInfo->StartRow;
                        pFexScaleCfg->Roi.m_width_m1  = pFexScaleInfo->RoiWidth - 1U;
                        pFexScaleCfg->Roi.m_height_m1 = pFexScaleInfo->RoiHeight - 1U;
                    }
                    Rval = SVC_OK;
                }
            }
        }

        if (SVC_OK == Rval) {

            MsgCfg.CtrlType = AMBA_SCA_SPUFEX_CTRL_CFG;
            MsgCfg.pExtCtrlCfg = NULL;
#ifdef CONFIG_SOC_CV2
            AmbaSvcWrap_MisraMemcpy(&Cfg.PyramidInfo, pPyramidInfo, sizeof(idsp_pyramid_t));
#else
            (void)pPyramidInfo;
#endif
            AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pCfg);
            AmbaMisra_TypeCast(&pAlgoOp1, &pAlgoOp);
            Rval = SvcCvAlgo_Control(&pAlgoOp1->AlgoHandle, &MsgCfg);

#ifdef CONFIG_SOC_CV2
            MsgCfg2.CtrlType = AMBA_SCA_SPUFEX_CTRL_DISPLAY_MODE;
            MsgCfg2.pExtCtrlCfg = NULL;

            AmbaMisra_TypeCast(&MsgCfg2.pCtrlParam, &pSpuDisplayMode);
            Rval = SvcCvAlgo_Control(&pAlgoOp1->AlgoHandle, &MsgCfg2);
#endif
            if (Rval == SVC_OK) {
                SvcLog_OK(SVC_LOG_STEREO, "Stereo_ConfigSpuFex", 0U, 0U);

#if 1
                for (i = 0; i < STEREO_FDAG_NUM_SPU_FEX_SCALE; i++) {
                    if (Cfg.SpuScaleCfg[i].Enable == 1U) {
                        AmbaPrint_PrintUInt5("[%2d]Spu: Enable %d, RoiEnable %d, FusionDSI %d",
                                              i,
                                              Cfg.SpuScaleCfg[i].Enable,
                                              Cfg.SpuScaleCfg[i].RoiEnable,
                                              Cfg.SpuScaleCfg[i].FusionDisparity,
                                              0U);
                        if (Cfg.SpuScaleCfg[i].RoiEnable == 1U) {
                            AmbaPrint_PrintUInt5("    Roi: %d, %d, %d, %d",
                                                  Cfg.SpuScaleCfg[i].Roi.m_start_col,
                                                  Cfg.SpuScaleCfg[i].Roi.m_start_row,
                                                  Cfg.SpuScaleCfg[i].Roi.m_width_m1,
                                                  Cfg.SpuScaleCfg[i].Roi.m_height_m1,
                                                  0U);
                        }
                    }

                    if (Cfg.FexScaleCfg[i].Enable == 1U) {
                        AmbaPrint_PrintUInt5("[%2d]Fex: Enable %d, RoiEnable %d, RoiColOffset %d, RoiRowOffset %d",
                                              i,
                                              Cfg.FexScaleCfg[i].Enable,
                                              Cfg.FexScaleCfg[i].RoiEnable,
                                              (UINT32)Cfg.FexScaleCfg[i].SecondaryRoiColOffset,
                                              (UINT32)Cfg.FexScaleCfg[i].SecondaryRoiRowOffset);
                        if (Cfg.FexScaleCfg[i].RoiEnable == 1U) {
                            AmbaPrint_PrintUInt5("    Roi: %d, %d, %d, %d",
                                                  Cfg.FexScaleCfg[i].Roi.m_start_col,
                                                  Cfg.FexScaleCfg[i].Roi.m_start_row,
                                                  Cfg.FexScaleCfg[i].Roi.m_width_m1,
                                                  Cfg.FexScaleCfg[i].Roi.m_height_m1,
                                                  0U);
                        }
                    }

                }
#ifdef CONFIG_SOC_CV2
                AmbaPrint_PrintUInt5("Pyramid: image_width_m1: %d, image_height_m1: %d, image_pitch_m1: %d",
                                       Cfg.PyramidInfo.image_width_m1,
                                       Cfg.PyramidInfo.image_height_m1,
                                       Cfg.PyramidInfo.image_pitch_m1,
                                       0U, 0U);
                for (i = 0U; i < (UINT32)MAX_HALF_OCTAVES; i++) {
                    AmbaPrint_PrintUInt5("[%2d][ctrl] disable: %d, mode: %d, octave_mode: %d, roi_pitch: %d",
                        i,
                        Cfg.PyramidInfo.half_octave[i].ctrl.disable,
                        Cfg.PyramidInfo.half_octave[i].ctrl.mode,
                        Cfg.PyramidInfo.half_octave[i].ctrl.octave_mode,
                        Cfg.PyramidInfo.half_octave[i].ctrl.roi_pitch);
                    AmbaPrint_PrintUInt5("[%2d][roi]  %d X %d (%d,%d)",
                        i,
                        (UINT32)(Cfg.PyramidInfo.half_octave[i].roi_width_m1),
                        (UINT32)(Cfg.PyramidInfo.half_octave[i].roi_height_m1),
                        (UINT32)(Cfg.PyramidInfo.half_octave[i].roi_start_col),
                        (UINT32)(Cfg.PyramidInfo.half_octave[i].roi_start_row));
                }
#endif
#endif
            } else {
                SvcLog_NG(SVC_LOG_STEREO, "Stereo_ConfigSpuFex", 0U, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_STEREO, "Stereo_ConfigSpuFex: Invalid param", 0U, 0U);
        }
    } else {
        Rval = SVC_NG;
    }

    return Rval;
}
#ifdef CONFIG_SOC_CV2FS
/**
* Entry of run-time configuration function
*/
static UINT32 Stereo_SendFusionConfig(UINT32 Inst, UINT32 AlgoIdx)
{
    UINT32 Rval = SVC_OK;
    const SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[Inst];
    const SVC_CV_FLOW_STEREO_MODE_INFO_s *pModeInfo = &SvcCvFlow_Stereo_ModeInfo[pCtrl->pCvFlow->CvModeID];

    if (pModeInfo->FusCfg.NumScale> 0U) {
        Rval |= Stereo_ConfigFusion(pCtrl, AlgoIdx, &pModeInfo->FusCfg);
    }

    return Rval;
}

static UINT32 Stereo_ConfigFusion(const SVC_STEREO_CTRL_s *pCtrl, UINT32 AlgoIdx, const SVC_CV_FLOW_STEREO_FUS_CFG_s *pFusionCfg)
{
    UINT32 Rval = SVC_NG;
    const SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &pCtrl->AlgoOp[AlgoIdx];
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp1;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;
    AMBA_CV_SPUFUSION_CFG_s Cfg02 = {CV_FUSION_MERGE_LOWER_SCALE,0};
    const AMBA_CV_SPUFUSION_CFG_s *pCfg02 = &Cfg02;
    AMBA_CV_SPUFUSION_CFG_s Cfg024 = {CV_FUSION_MERGE_LOWER_UPPER_SCALE,2};
    const AMBA_CV_SPUFUSION_CFG_s *pCfg024 = &Cfg024;
    (void)pCtrl;

    AmbaMisra_TypeCast(&pAlgoOp1, &pAlgoOp);
    MsgCfg.CtrlType = 0;
    MsgCfg.pExtCtrlCfg = NULL;
    if (pFusionCfg->NumScale == 2U){
        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pCfg02);
    } else {
        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pCfg024);
    }
    Rval = SvcCvAlgo_Control(&pAlgoOp1->AlgoHandle, &MsgCfg);


    return Rval;
}
#endif

static UINT32 Stereo_ConfigBuffer(UINT32 Inst)
{
    UINT32 Rval = SVC_OK;
    SVC_STEREO_CTRL_s *pCtrl = &g_StereoCtrl[Inst];
    SVC_CV_FLOW_BUF_INFO_s BufInfo, BufPhyInfo;
    UINT32 i;

    /* Img info buffer */
    if ((pCtrl->pImgInfoBuf[0] == NULL) && (pCtrl->pImgInfoPhyBuf[0] == NULL)) {
        for (i = 0; i < SVC_STEREO_PICINFO_QUEUE_SIZE; i++) {
            Rval = SvcCvFlow_AllocWorkBuf(sizeof(memio_source_recv_picinfo_t), &BufInfo);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pCtrl->pImgInfoBuf[i], &BufInfo.pAddr);
                pCtrl->ImgInfoBufSize = BufInfo.Size;
            } else {
                SvcLog_NG(SVC_LOG_STEREO, "Alloc Img info buf failed", 0U, 0U);
                Rval = SVC_NG;
                break;
            }

            Rval = SvcCvFlow_AllocWorkBuf(sizeof(memio_source_recv_picinfo_t), &BufPhyInfo);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pCtrl->pImgInfoPhyBuf[i], &BufPhyInfo.pAddr);
            } else {
                SvcLog_NG(SVC_LOG_STEREO, "Alloc Img info buf failed", 0U, 0U);
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
static UINT32 CvFlow_StereoInit(UINT32 CvFlowChan, void *pInit)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    static UINT32 g_StereoInit = 0U;

    (void) CvFlowChan;
    (void) pInit;

    if (0U == g_StereoInit) {
        g_StereoInit = 1U;

        if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&StereoMutex, NULL)) {
            SvcLog_NG(SVC_LOG_STEREO, "MutexCreate error", 0U, 0U);
            Rval = SVC_NG;
        }
        for (i = 0; i < SVC_STEREO_MAX_INSTANCE; i++) {
            g_StereoCtrl[i].Used = 0;
        }

        SVC_CV_DBG(SVC_LOG_STEREO, "CvFlow_StereoInit: first init", 0U, 0U);
    }

    SvcLog_OK(SVC_LOG_STEREO, "CvFlow_StereoInit: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver configuration function
*/
static UINT32 CvFlow_StereoConfig(UINT32 CvFlowChan, void *pConfig)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    SVC_STEREO_CTRL_s *pCtrl = NULL;
    const SVC_CV_FLOW_STEREO_MODE_INFO_s *pModeInfo;

    (void) pConfig;

    if (NULL != pConfig) {
        Stereo_MutexTake();
        /* Create instance */
        for (i = 0; i < SVC_STEREO_MAX_INSTANCE; i++) {
            if (0U == g_StereoCtrl[i].Used) {
                Rval = AmbaWrap_memset(&g_StereoCtrl[i], 0, sizeof(SVC_STEREO_CTRL_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STEREO, "memset g_StereoCtrl[%u] failed", i, 0U);
                }
                pCtrl = &g_StereoCtrl[i];
                pCtrl->Used = 1;
                SvcLog_DBG(SVC_LOG_STEREO, "Create Inst(%u)", i, 0U);
                break;
            }
        }
        Stereo_MutexGive();

        /* Configure instance */
        if (NULL != pCtrl) {
            pCtrl->CvfChan     = (UINT8) CvFlowChan;
            pCtrl->pCvFlow     = &pCtrl->CvCfg;
            Rval = AmbaWrap_memcpy(&pCtrl->CvCfg, pConfig, sizeof(SVC_CV_FLOW_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STEREO, "memcpy CvCfg failed", 0U, 0U);
            }
            pCtrl->SrcRateCtrl = 1;
            pCtrl->Enable      = 1;
            pCtrl->SendConfig = 1;
            pCtrl->SendFusionConfig = 1;

            pModeInfo = &SvcCvFlow_Stereo_ModeInfo[pCtrl->pCvFlow->CvModeID];

            if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG) {
                Rval = SVC_OK;
            } else if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
#ifdef CONFIG_AMBALINK_BOOT_OS
                /* Init FlexiDAG IO framework */
                if (pCtrl->pCvFlow->PrivFlag < SVC_CV_FLEXIDAG_IO_MAX_CHANNEL) {
                    pCtrl->FlexiIOChan = pCtrl->pCvFlow->PrivFlag;
                    Rval = SvcCv_FlexiIORegister(pCtrl->FlexiIOChan, Stereo_Init_Callback);
                    if (Rval != SVC_OK) {
                        SvcLog_NG(SVC_LOG_STEREO, "SvcCv_FlexiIORegister(%u) failed", pCtrl->FlexiIOChan, 0U);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_STEREO, "Invalid FlexidagIO chan(%u)", pCtrl->pCvFlow->PrivFlag, 0U);
                }
#else
                Rval = SVC_NG;
#endif
            } else {
                SvcLog_NG(SVC_LOG_STEREO, "unsupported CVFlow ProfID(%u)", pModeInfo->ProfID, 0U);
                Rval = SVC_NG;
            }

        } else {
            Rval = SVC_NG;
        }
    } else {
        Rval = SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_STEREO, "CvFlow_StereoConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_STEREO, "CvFlow_StereoConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver load function
*/
static UINT32 CvFlow_StereoLoad(UINT32 CvFlowChan, void *pInfo)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_STEREO_CTRL_s *pCtrl;
    const SVC_CV_FLOW_LOAD_INFO_s *pLoadInfo;
    const SVC_CV_FLOW_STEREO_MODE_INFO_s *pModeInfo;
    UINT32 i;

    (void) pInfo;

    Rval = Stereo_CvfChan2Inst(CvFlowChan, &Inst);
    if (Rval == SVC_OK) {
        pCtrl = &g_StereoCtrl[Inst];
        if (NULL != pInfo) {
            AmbaMisra_TypeCast(&pLoadInfo, &pInfo);
            if (pLoadInfo->NumBin > 0U) {
                /* Use external path */
                if (pLoadInfo->AlgoIdx < STEREO_MAX_ALGO_NUM) {
                    Rval = SvcCvFlow_LoadAlgoBin(SVC_LOG_STEREO, pLoadInfo, &pCtrl->AlgoOp[pLoadInfo->AlgoIdx]);
                } else {
                    SvcLog_NG(SVC_LOG_STEREO, "CvFlow_StereoLoad: CvFlowChan(%d) invalid AlgoIdx(%u)", CvFlowChan, pLoadInfo->AlgoIdx);
                }
            } else {
                /* Use ModeInfo */
                if (pCtrl->pCvFlow->CvModeID < STEREO_NUM_MODE) {
                    pModeInfo = &SvcCvFlow_Stereo_ModeInfo[pCtrl->pCvFlow->CvModeID];

                    for (i = 0; i < pModeInfo->AlgoGrp.NumAlgo; i++) {
                        Rval |= SvcCvFlow_LoadAlgo(SVC_LOG_STEREO, &pModeInfo->AlgoGrp.AlgoInfo[i], &pCtrl->AlgoOp[i]);
                    }
                } else  {
                    SvcLog_NG(SVC_LOG_STEREO, "CvFlow_StereoLoad: CvFlowChan(%d) invalid CvModeID(%u)", CvFlowChan, pCtrl->pCvFlow->CvModeID);
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO, "CvFlow_StereoLoad: invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STEREO, "CvFlow_StereoLoad: CvFlowChan(%d) error", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver start function
*/
static UINT32 CvFlow_StereoStart(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst, i, j, AlgoIdx, Num;
    SVC_STEREO_CTRL_s *pCtrl;
    const SVC_CV_FLOW_STEREO_MODE_INFO_s *pModeInfo;
    SVC_CV_FLOW_BUF_INFO_s BufInfo;
    const SVC_CV_FLOW_ALGO_INFO_s *pAlgoInfo;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
    SVC_CV_FLOW_ALGO_QUERY_CFG_s QueryCfg;
    SVC_CV_FLOW_ALGO_CREATE_CFG_s CreateCfg;
    SVC_CV_ALGO_REGCB_CFG_s CbCfg;
    SVC_IMG_FEEDER_PORT_CFG_s PortCfg = {0};
    static char SpuFexOutQueName[] = "SpuFexOutAlgoQue";
    static char FusionInQueName[] = "FusionInQue";
    static char FusionAlgoOutQueName[] = "FusionAlgoOutQue";
    static char SemName[] = "OutBufSem";
    static char SemNameRx1[] = "OutBufSemRx1";
    static STEREO_OUTPUT_MSG_s SpuFexAlgoOutQue[SVC_STEREO_MAX_INSTANCE][SVC_STEREO_ALGO_OUT_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static STEREO_RX1_MSG_s FusionInQue[SVC_STEREO_MAX_INSTANCE][SVC_STEREO_ALGO_OUT_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static STEREO_OUTPUT_MSG_s FusionAlgoOutQue[SVC_STEREO_MAX_INSTANCE][SVC_STEREO_ALGO_OUT_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;

    static UINT8 StereoRxTaskStack[SVC_STEREO_MAX_INSTANCE][SVC_STEREO_RX_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 StereoTx1TaskStack[SVC_STEREO_MAX_INSTANCE][SVC_STEREO_RX_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 StereoRx1TaskStack[SVC_STEREO_MAX_INSTANCE][SVC_STEREO_RX_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_IMG_FEEDER_EVENT_CALLBACK_f Stereo_CvDataSrcHandlerList[SVC_STEREO_MAX_INSTANCE] = {
        Stereo_CvDataSrcHandler0,
        Stereo_CvDataSrcHandler1,
        Stereo_CvDataSrcHandler2,
        Stereo_CvDataSrcHandler3,
    };
    static SVC_CV_ALGO_CALLBACK_f Stereo_AlgoOutCbList[STEREO_MAX_FLEXIDAG_NUM][SVC_STEREO_MAX_INSTANCE] = {
        {Stereo_SpuFexAlgoOutCallback0, Stereo_SpuFexAlgoOutCallback1, Stereo_SpuFexAlgoOutCallback2, Stereo_SpuFexAlgoOutCallback3},
        {Stereo_FusionAlgoOutCallback0, Stereo_FusionAlgoOutCallback1, Stereo_FusionAlgoOutCallback2, Stereo_FusionAlgoOutCallback3},
    };
#ifdef CONFIG_SOC_CV2
    static UINT32 Fusionlayer;
    static UINT32 *pFusionlayer = &Fusionlayer;
#endif
    (void) pInput;

    Rval = Stereo_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl     = &g_StereoCtrl[Inst];
        pModeInfo = &SvcCvFlow_Stereo_ModeInfo[pCtrl->pCvFlow->CvModeID];

        /* Open Feeder port first to enable frame FIFO earlier */
        if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
            /* The driver supports single input per instance */
            PortCfg.NumPath            = 2U;
            PortCfg.Content[0].DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
            PortCfg.Content[0].StrmId  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
            PortCfg.Content[1].DataSrc = pCtrl->pCvFlow->InputCfg.Input[1].DataSrc;
            PortCfg.Content[1].StrmId  = pCtrl->pCvFlow->InputCfg.Input[1].StrmId;
            PortCfg.SendFunc           = Stereo_CvDataSrcHandlerList[Inst];

            if (SVC_OK != SvcImgFeeder_OpenPort(pCtrl->pCvFlow->InputCfg.FeederID, &PortCfg, &pCtrl->FeederPort)) {
                SvcLog_NG(SVC_LOG_STEREO, "Open feeder port error", 0U, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_STEREO, "ImgFeeder is not enabled", 0U, 0U);
        }

        for (AlgoIdx = 0; AlgoIdx < pModeInfo->AlgoGrp.NumAlgo; AlgoIdx++) {
            pAlgoInfo = &pModeInfo->AlgoGrp.AlgoInfo[AlgoIdx];
            pAlgoOp   = &pCtrl->AlgoOp[AlgoIdx];

            Rval = AmbaWrap_memset(&QueryCfg, 0, sizeof(SVC_CV_FLOW_ALGO_QUERY_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STEREO, "memset QueryCfg failed", 0U, 0U);
            }
            Rval = SvcCvFlow_QueryAlgo(SVC_LOG_STEREO, pAlgoInfo, &QueryCfg, pAlgoOp);
            if (SVC_OK == Rval) {
                /* Create algo */
                Rval = AmbaWrap_memset(&CreateCfg, 0, sizeof(SVC_CV_FLOW_ALGO_CREATE_CFG_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STEREO, "memset CreateCfg failed", 0U, 0U);
                }

                if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG) {

                } else if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
#ifdef CONFIG_AMBALINK_BOOT_OS
                   if (1U == SvcCv_FlexiIOStatusGet(pCtrl->FlexiIOChan)) {
                       const UINT32 *pAmbaIPCChannel = &pCtrl->FlexiIOChan;
                       AmbaMisra_TypeCast(&CreateCfg.pExtCfg, &pAmbaIPCChannel);
                   } else {
                       SvcLog_NG(SVC_LOG_STEREO, "FlexidagIO(%u) has not ready. Please run Linux app first.", pCtrl->FlexiIOChan, 0U);
                       Rval = SVC_NG;
                   }
#else
                    SvcLog_NG(SVC_LOG_STEREO, "CONFIG_AMBALINK_BOOT_OS not defined ", 0U, 0U);
                    Rval = SVC_NG;
#endif
                } else {
                    SvcLog_NG(SVC_LOG_STEREO, "unsupported CVFlow ProfID(%u)", pModeInfo->ProfID, 0U);
                    Rval = SVC_NG;
                }

#ifdef CONFIG_SOC_CV2
                if (pCtrl->SendFusionConfig == 1U) {
                    Fusionlayer = pModeInfo->FusCfg.NumScale;
                    AmbaMisra_TypeCast(&CreateCfg.pExtCfg, &pFusionlayer);
                }
#endif
                Rval = SvcCvFlow_CreateAlgo(SVC_LOG_STEREO, pAlgoInfo, &CreateCfg, pAlgoOp);

                if (SVC_OK == Rval) {
                    /* Create Output buffer */
                    for (i = 0; i < SVC_STEREO_OUTPUT_DEPTH; i++) {
                        Num = pAlgoOp->OutputNum;
                        for (j = 0; j < Num; j++) {
                            Rval |= SvcCvFlow_AllocWorkBuf(pAlgoOp->OutputSize[j], &BufInfo);
                            SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_STEREO, BufInfo.pAddr, BufInfo.Size, &pCtrl->AlgoOpEx[AlgoIdx].OutputBuf[i][j]);
                        }
                    }

                    if (SVC_OK == Rval) {
                        /* Register Algo callback */
                        Rval = AmbaWrap_memset(&CbCfg, 0, sizeof(SVC_CV_ALGO_REGCB_CFG_s));
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_STEREO, "memset CbCfg failed", 0U, 0U);
                        }
                        CbCfg.Mode     = 0U;
                        CbCfg.Callback = Stereo_AlgoOutCbList[AlgoIdx][Inst]; //Register Algo callback
                        Rval = SvcCvAlgo_RegCallback(&pAlgoOp->AlgoHandle, &CbCfg);
                        if (SVC_OK == Rval) {
                            pCtrl->NumValidAlgo++;
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_STEREO, "Alloc Algo[%u] Output Buf failed", AlgoIdx, 0U);
                    }
                } else {
                   SvcLog_NG(SVC_LOG_STEREO, "CreateAlgo[%u] error", AlgoIdx, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_STEREO, "QueryAlgo[%u] error", AlgoIdx, 0U);
            }
        }

        if (SVC_OK == Rval) {
            Rval = Stereo_ConfigBuffer(Inst);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STEREO, "ConfigBuffer error", 0U, 0U);
            }
        }

        if (SVC_OK == Rval) {
            pCtrl->pTxProc = Stereo_CvDataSrcHandler;
            pCtrl->pRxProc = Stereo_SpuFexDataSinkHandler;
            pCtrl->pTx1Proc = Stereo_FusionDataSrcHandler;
            pCtrl->pRx1Proc = Stereo_FusionDataSinkHandler;
            pCtrl->pRx1EvtHandler = Stereo_FusionEventHandler;
            /* Stereo_SpuFexOutCallback */
            Rval = AmbaKAL_MsgQueueCreate(&pCtrl->SpuFexOutAlgoQueId,
                                           SpuFexOutQueName,
                                           sizeof(STEREO_OUTPUT_MSG_s),
                                           &SpuFexAlgoOutQue[Inst][0],
                                           SVC_STEREO_ALGO_OUT_QUEUE_SIZE * sizeof(STEREO_OUTPUT_MSG_s));

            /* Stereo_FusionIn */
            Rval = AmbaKAL_MsgQueueCreate(&pCtrl->FusionInQueId,
                                           FusionInQueName,
                                           sizeof(STEREO_RX1_MSG_s),
                                           &FusionInQue[Inst][0],
                                           SVC_STEREO_FUSIONIN_QUEUE_SIZE * sizeof(STEREO_RX1_MSG_s));

            /* Stereo_FusionOutCallback */
            Rval = AmbaKAL_MsgQueueCreate(&pCtrl->FusionOutAlgoQueId,
                                           FusionAlgoOutQueName,
                                           sizeof(STEREO_OUTPUT_MSG_s),
                                           &FusionAlgoOutQue[Inst][0],
                                           SVC_STEREO_ALGO_OUT_QUEUE_SIZE * sizeof(STEREO_OUTPUT_MSG_s));

            Rval |= AmbaKAL_SemaphoreCreate(&pCtrl->OutBufSem, SemName, SVC_STEREO_SEM_INIT_COUNT);
            Rval |= AmbaKAL_SemaphoreCreate(&pCtrl->Rx1OutBufSem, SemNameRx1, SVC_STEREO_SEM_INIT_COUNT);

            if (KAL_ERR_NONE == Rval) {
                /* Create RX task */
                pCtrl->RxTaskCtrl.Priority   = 60;
                pCtrl->RxTaskCtrl.EntryFunc  = Stereo_SpuFexOutTaskEntry;
                pCtrl->RxTaskCtrl.EntryArg   = Inst;
                pCtrl->RxTaskCtrl.pStackBase = &StereoRxTaskStack[Inst][0];
                pCtrl->RxTaskCtrl.StackSize  = SVC_STEREO_RX_TASK_STACK_SIZE;
                pCtrl->RxTaskCtrl.CpuBits    = 0x01U;
                Rval = SvcTask_Create("Stereo_SpuFexOutTask", &pCtrl->RxTaskCtrl);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STEREO, "Create Stereo_SpuFexOutTask Inst(%u) error", Inst, 0U);
                }

                /* Create TX1 task */
                pCtrl->Tx1TaskCtrl.Priority   = 60;
                pCtrl->Tx1TaskCtrl.EntryFunc  = Stereo_FusionInTaskEntry;
                pCtrl->Tx1TaskCtrl.EntryArg   = Inst;
                pCtrl->Tx1TaskCtrl.pStackBase = &StereoTx1TaskStack[Inst][0];
                pCtrl->Tx1TaskCtrl.StackSize  = SVC_STEREO_RX_TASK_STACK_SIZE;
                pCtrl->Tx1TaskCtrl.CpuBits    = 0x01U;
                Rval = SvcTask_Create("Stereo_FusionInTask", &pCtrl->Tx1TaskCtrl);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STEREO, "Create Stereo_FusionInTaskEntry Inst(%u) error", Inst, 0U);
                }

                /* Create RX1 task */
                pCtrl->Rx1TaskCtrl.Priority   = 60;
                pCtrl->Rx1TaskCtrl.EntryFunc  = Stereo_FusionOutTaskEntry;
                pCtrl->Rx1TaskCtrl.EntryArg   = Inst;
                pCtrl->Rx1TaskCtrl.pStackBase = &StereoRx1TaskStack[Inst][0];
                pCtrl->Rx1TaskCtrl.StackSize  = SVC_STEREO_RX_TASK_STACK_SIZE;
                pCtrl->Rx1TaskCtrl.CpuBits    = 0x01U;
                Rval = SvcTask_Create("Stereo_FusionOutTask", &pCtrl->Rx1TaskCtrl);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STEREO, "Create Stereo_FusionOutTask Inst(%u) error", Inst, 0U);
                }

                /* Start input */
                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_STEREO, "Connect feeder port error", 0U, 0U);
                    }
                }

                if (SVC_OK == Rval) {
                    pCtrl->Started = 1U;
                }
            } else {
                SvcLog_NG(SVC_LOG_STEREO, "Create MsgQue/Semaphore error(0x%X)", Rval, 0U);
                Rval =  SVC_NG;
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO, "Invalid CvFlowChan", 0U, 0U);
        Rval =  SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_STEREO, "CvFlow_StereoStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_STEREO, "CvFlow_StereoStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver control function
*/
static UINT32 CvFlow_StereoControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo)
{
    UINT32 Rval, Inst;
    SVC_STEREO_CTRL_s *pCtrl;
    const UINT32 *pValue;
    UINT32 Index;
    UINT32 *pFusionEnable;

    Stereo_MutexTake();

    Rval = Stereo_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl = &g_StereoCtrl[Inst];
        switch (CtrlType) {
            case SVC_CV_CTRL_ENABLE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->Enable = (UINT8) *pValue;
                if ((1U == pCtrl->Started) &&
                    (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder)) {
                    if (*pValue == 0U) {
                        Rval = SvcImgFeeder_Disconnect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_STEREO, "Disconn feeder port error", 0U, 0U);
                        }
                    } else {
                        Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_STEREO, "Cnnect feeder port error", 0U, 0U);
                        }
                    }
                }
                break;
            case SVC_CV_CTRL_SET_SRC_RATE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->SrcRateCtrl = *pValue;
                SvcLog_OK(SVC_LOG_STEREO, "Control[%d]: SrcRate = %u", Inst, *pValue);
                break;
            case SVC_CV_CTRL_SEND_EXT_YUV:
                Rval = Stereo_SendExtYUV(Inst, pInfo);
                break;
            case SVC_CV_CTRL_SEND_EXT_PIC_INFO:
                Rval = Stereo_SendExtPicInfo(Inst, pInfo);
                break;
            case SVC_CV_CTRL_SEND_FUSION_INFO:
                Rval = Stereo_SendFusion(Inst, pInfo);
                break;
            case SVC_CV_CTRL_GET_FUSION_INFO:
                {
                    AmbaMisra_TypeCast(&pFusionEnable, &pInfo);
                    Index = pCtrl->pCvFlow->CvModeID;
                    *pFusionEnable = SvcCvFlow_Stereo_ModeInfo[Index].EnableFusion;
                }
                break;
            default:
                SvcLog_NG(SVC_LOG_STEREO, "Control[%d]: Invalid CtrlType(%d)", Inst, CtrlType);
                break;
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO, "Control: Invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    Stereo_MutexGive();

    return Rval;
}

/**
* CvFlow driver stop function
*/
static UINT32 CvFlow_StereoStop(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_STEREO_CTRL_s *pCtrl;
    SVC_CV_FLOW_ALGO_DELETE_CFG_s DelCfg;
    UINT32 i;

    (void)pInput;

    Rval = Stereo_CvfChan2Inst(CvFlowChan, &Inst);
    pCtrl = &g_StereoCtrl[Inst];

    if (SVC_OK == Rval) {
        Rval = Stereo_SetTaskIdle(Inst);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_STEREO, "SetTaskIdle error", 0U, 0U);
        }

        Rval = SvcTask_Destroy(&pCtrl->RxTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STEREO, "Destroy Rx Task error", 0U, 0U);
        }

        Rval = SvcTask_Destroy(&pCtrl->Tx1TaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STEREO, "Destroy Tx Task error", 0U, 0U);
        }

        Rval = SvcTask_Destroy(&pCtrl->Rx1TaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STEREO, "Destroy Rx1 Task error", 0U, 0U);
        }

        Rval = AmbaWrap_memset(&DelCfg, 0, sizeof(SVC_CV_FLOW_ALGO_DELETE_CFG_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STEREO, "memset DelCfg failed", 0U, 0U);
        }

        for (i = 0; i < pCtrl->NumValidAlgo; i++) {
            Rval |= SvcCvFlow_DeleteAlgo(SVC_LOG_STEREO, &DelCfg, &pCtrl->AlgoOp[i]);
        }
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STEREO, "Delele Algo error", 0U, 0U);
        }

#ifdef CONFIG_AMBALINK_BOOT_OS
        if (SvcCvFlow_Stereo_ModeInfo[pCtrl->pCvFlow->CvModeID].ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
            Rval = SvcCv_FlexiIORelease(pCtrl->FlexiIOChan);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_STEREO, "SvcCv_FlexiIORelease(%u) failed", pCtrl->FlexiIOChan, 0U);
            }
        }
#endif

        Rval |= AmbaKAL_MsgQueueDelete(&pCtrl->SpuFexOutAlgoQueId);
        Rval |= AmbaKAL_MsgQueueDelete(&pCtrl->FusionInQueId);
        Rval |= AmbaKAL_MsgQueueDelete(&pCtrl->FusionOutAlgoQueId);
        Rval |= AmbaKAL_SemaphoreDelete(&pCtrl->OutBufSem);
        Rval |= AmbaKAL_SemaphoreDelete(&pCtrl->Rx1OutBufSem);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_STEREO, "Delete MsgQue/Semaphore error(0x%X)", Rval, 0U);
        }

        /* Reset Ctrl Intance */
        Rval = AmbaWrap_memset(&g_StereoCtrl[Inst], 0, sizeof(SVC_STEREO_CTRL_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STEREO, "memset g_StereoCtrl[%u] failed", Inst, 0U);
        }
    }

    SvcLog_OK(SVC_LOG_STEREO, "CvFlow_StereoStop: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver debug interface
*/
static UINT32 CvFlow_StereoDebug(UINT32 CvFlowChan, const char *pCmd, UINT32 Param0, UINT32 Param1, UINT32 Param2)
{
    UINT32 i;
    ULONG Addr;
    const SVC_STEREO_CTRL_s *pCtrl;
    const AMBA_CV_FLEXIDAG_HANDLE_s *pCvFDHandle;

    if (0 == SvcWrap_strcmp("debug", pCmd)) {
        SvcCvDebugEnable = (UINT8) Param0;
        SvcLog_DBG(SVC_LOG_STEREO, "Set debug(%u)", Param0, 0U);
    } else if (0 == SvcWrap_strcmp("info", pCmd)) {
        for (i = 0; i < SVC_STEREO_MAX_INSTANCE; i++) {
            pCtrl = &g_StereoCtrl[i];
            if (pCtrl->Used == 1U) {
                AmbaPrint_PrintUInt5("Stereo[%d]: CvfChan %d", i, pCtrl->CvfChan, 0U, 0U, 0U);
                pCvFDHandle = &pCtrl->AlgoOp[0].AlgoHandle.FDs[0].Handle;
                AmbaMisra_TypeCast(&Addr, &pCvFDHandle);
                SvcCvFlow_PrintULong("  CV_FLEXIDAG_HANDLE: 0x%x", Addr, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        SvcLog_DBG(SVC_LOG_STEREO, "Unsupported cmd", 0U, 0U);
    }

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(&Param0);
    AmbaMisra_TouchUnused(&Param1);
    AmbaMisra_TouchUnused(&Param2);

    return SVC_OK;
}

SVC_CV_FLOW_OBJ_s SvcCvFlow_StereoObj = {
    .Init         = CvFlow_StereoInit,
    .Config       = CvFlow_StereoConfig,
    .Load         = CvFlow_StereoLoad,
    .Start        = CvFlow_StereoStart,
    .Control      = CvFlow_StereoControl,
    .Stop         = CvFlow_StereoStop,
    .Debug        = CvFlow_StereoDebug,
};

