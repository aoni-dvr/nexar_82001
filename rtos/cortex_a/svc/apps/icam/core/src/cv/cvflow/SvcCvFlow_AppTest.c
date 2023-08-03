/**
 *  @file SvcCvFlow_AppTest.c
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
 *  @details Implementation of FlexiDAG based Application test
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
#include "AmbaVIN_Priv.h"

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
#include "SvcCvFlow_AppTest.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcImgFIFO.h"
#include "SvcImgFeeder.h"
#include "SvcOsd.h"
#include "SvcWrap.h"
#if defined(CONFIG_BUILD_IMGFRW_AAA) && defined(CONFIG_SOC_CV2FS)
#include "SvcImg.h"
#include "AmbaImg_Proc.h"
#include "AmbaImg_AaaAsil.h"
#include "Amba_IPMonitor.h"
#include "Amba_IPMonitor_Def.h"
#include "AmbaImg_External_CtrlFunc.h"
#endif

#include "cvapi_amba_mask_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_ref_util.h"
#include "cvapi_svccvalgo_openseg.h"
#include "cvapi_svccvalgo_apptest.h"


#define SVC_LOG_APPTEST        "AppTest"

#define SVC_APPTEST_INSTANCE0                (0U)
#define SVC_APPTEST_INSTANCE1                (1U)
#define SVC_APPTEST_MAX_INSTANCE             (2U)

#define SVC_APPTEST_SEM_INIT_COUNT           (1U)
#define SVC_APPTEST_OUTPUT_DEPTH             (SVC_APPTEST_SEM_INIT_COUNT + 2U)

#define SVC_APPTEST_RX_TASK_STACK_SIZE       (0x4000)
#define SVC_APPTEST_PICINFO_QUEUE_SIZE       (8U)
#define SVC_APPTEST_ALGO_OUT_QUEUE_SIZE      (SVC_APPTEST_OUTPUT_DEPTH)

#define SVC_APPTEST_AAA_ALGO_INPUT           (0x1001U)

typedef UINT32 (*SVC_APPTEST_DATA_HANDLER_f)(UINT32 Inst);
typedef UINT32 (*SVC_APPTEST_TX_HANDLER_f)(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

typedef struct {
    flexidag_memblk_t           OutputBuf[SVC_APPTEST_OUTPUT_DEPTH][FLEXIDAG_MAX_OUTPUTS];
    AMBA_CV_FLEXIDAG_IO_s       InBuf[SVC_APPTEST_OUTPUT_DEPTH];
    AMBA_CV_FLEXIDAG_IO_s       OutBuf[SVC_APPTEST_OUTPUT_DEPTH];
} SVC_CV_FLOW_APPTEST_OP_EX_INFO_s;

typedef struct {
    UINT8                       Used;
    UINT8                       CvfChan;
    UINT8                       Enable;
    UINT8                       Started;
    SVC_CV_FLOW_CFG_s           *pCvFlow;
    SVC_CV_FLOW_CFG_s           CvCfg;

    UINT8                       NumValidAlgo;
    SVC_CV_FLOW_ALGO_OP_INFO_s  AlgoOp[APPTEST_MAX_ALGO_NUM];
    SVC_CV_FLOW_APPTEST_OP_EX_INFO_s AlgoOpEx[APPTEST_MAX_ALGO_NUM];
    void                       *pImgInfoBuf[SVC_APPTEST_PICINFO_QUEUE_SIZE];
    UINT32                      ImgInfoBufSize;

    /* Data source service */
    SVC_IMG_FEEDER_PORT_s       FeederPort;
    UINT32                      SrcSeqNum;
    SVC_APPTEST_TX_HANDLER_f     pTxProc;
    UINT32                      TxSeqNum;
    UINT32                      PicInfoIdx;

    /* Data sink service */
    SVC_TASK_CTRL_s             RxTaskCtrl;
    SVC_APPTEST_DATA_HANDLER_f   pRxProc;
    UINT32                      RxSeqNum;
    UINT32                      OutBufIdx;
    AMBA_KAL_SEMAPHORE_t        OutBufSem;
    UINT32                      SinkInfoIdx;

    /* Algo Callback */
    AMBA_KAL_MSG_QUEUE_t        AlgoOutQueId;

    /* Misc */
    UINT32                      SrcRateCtrl;
} SVC_APPTEST_CTRL_s;

typedef struct {
   UINT32 Event;
   SVC_CV_ALGO_OUTPUT_s AlgoOut;
} APPTEST_OUTPUT_MSG_s;

static SVC_APPTEST_CTRL_s g_AppTestCtrl[SVC_APPTEST_MAX_INSTANCE] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MUTEX_t AppTestMutex;
static UINT8 SvcCvDebugEnable = 0;

static UINT32 CvFlow_AppTestInit(UINT32 CvFlowChan, void *pInit);
static UINT32 CvFlow_AppTestConfig(UINT32 CvFlowChan, void *pConfig);
static UINT32 CvFlow_AppTestStart(UINT32 CvFlowChan, void *pInput);
static UINT32 CvFlow_AppTestControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo);
static UINT32 CvFlow_AppTestStop(UINT32 CvFlowChan, void *pInput);
static UINT32 AppTest_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst);

static UINT32 AppTest_TxDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 AppTest_TxDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

static UINT32 AppTest_AlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 AppTest_AlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);

static void SVC_CV_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcCvDebugEnable > 0U) {
        SvcLog_DBG(pModule, pFormat, Arg1, Arg2);
    }
}

static void AppTest_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&AppTestMutex, 5000)) {
        SvcLog_NG(SVC_LOG_APPTEST, "AppTest_MutexTake: timeout", 0U, 0U);
    }
}

static void AppTest_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&AppTestMutex)) {
        SvcLog_NG(SVC_LOG_APPTEST, "AppTest_MutexGive: error", 0U, 0U);
    }
}

static UINT32 AppTest_SemTake(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreTake(&g_AppTestCtrl[Inst].OutBufSem, 10000)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_APPTEST, "AppTest_SemTake[%d]: timeout", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 AppTest_SemGive(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreGive(&g_AppTestCtrl[Inst].OutBufSem)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_APPTEST, "AppTest_SemGive[%d]: error", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 AppTest_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst)
{
    UINT32 Rval = SVC_NG;
    UINT32 i;

    if (NULL != pInst) {
        for (i = 0; i < SVC_APPTEST_MAX_INSTANCE; i++) {
            if ((1U == g_AppTestCtrl[i].Used) && (CvfChan == g_AppTestCtrl[i].CvfChan)) {
                *pInst = i;
                Rval = SVC_OK;
                break;
            }
        }
    }
    return Rval;
}

static void AppTest_DumpPicInfo(const cv_pic_info_t *pPicInfo)
{
    static UINT32 DumpTimes = 0U;
    extern void SvcCvFlow_DumpPicInfo(const char *pModuleName, const cv_pic_info_t *pPicInfo);

    if (DumpTimes > 0U) {
        SvcCvFlow_DumpPicInfo(SVC_LOG_APPTEST, pPicInfo);
        DumpTimes--;
    }
}

/**
* Pack data source to pic info structure
*/
static void AppTest_PackPicInfo(UINT32 Inst, const SVC_CV_INPUT_IMG_INFO_s *pInputImg, memio_source_recv_picinfo_t *pInfo)
{
    UINT32 Rval = SVC_OK;
    cv_pic_info_t *pPicInfo = &pInfo->pic_info;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    const SVC_APPTEST_CTRL_s *pCtrl = &g_AppTestCtrl[Inst];
    UINT32 DataSrc, RelativeAddrOn = 1U;
    extern UINT32 SvcCvFlow_PackPicInfo_Pyramid(cv_pic_info_t *pPicInfo, const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo, UINT32 MainWidth, UINT32 MainHeight, UINT32 UseRelative, UINT32 LeftRight);
    extern UINT32 SvcCvFlow_PackPicInfo_MainYuv(cv_pic_info_t *pPicInfo, const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo, UINT32 UseRelative, UINT32 LeftRight);

    Rval = AmbaWrap_memset(pInfo, 0, sizeof(memio_source_recv_picinfo_t));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_APPTEST, "memset picinfo failed", 0U, 0U);
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

            if (SvcCvFlow_AppTest_ModeInfo[pCtrl->pCvFlow->CvModeID].ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
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

    AppTest_DumpPicInfo(pPicInfo);

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_APPTEST, "AppTest_PackPicInfo error", 0U, 0U);
    }
}

/**
* Send message to Feeder to update port status
*/
static void AppTest_SendReceiverReady(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort)
{
    SVC_APPTEST_CTRL_s *pCtrl = &g_AppTestCtrl[Inst];
    AMBA_KAL_SEMAPHORE_INFO_s SemInfo;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreQuery(&pCtrl->OutBufSem, &SemInfo)) {
        if (SemInfo.CurrentCount > 0U) {
            if (SVC_OK != SvcImgFeeder_MsgToFeeder(pPort, SVC_IMG_FEEDER_RECEIVER_READY, NULL)) {
                SvcLog_NG(SVC_LOG_APPTEST, "Send ReceiverReady[%u] error", Inst, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_APPTEST, "ReceiverReady[%u]: SemQuery error", Inst, 0U);
    }
}

/**
* TX data source handler: process input data
*/
static UINT32 AppTest_TxDataSrcHandler(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_APPTEST_CTRL_s *pCtrl = &g_AppTestCtrl[Inst];
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
    const SVC_CV_INPUT_IMG_INFO_s *pInputImg;
    memio_source_recv_picinfo_t *pPicInfo;
    UINT32 i, j;
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;
    AMBA_CV_FLEXIDAG_IO_s *pInBuf, *pOutBuf;
    const SVC_CV_FLOW_APPTEST_MODE_INFO_s *pModeInfo;

    AmbaMisra_TouchUnused(pInfo);
    pModeInfo = &SvcCvFlow_AppTest_ModeInfo[pCtrl->pCvFlow->CvModeID];

    SVC_CV_DBG(SVC_LOG_APPTEST, "Tx[%u]: run %d", Inst, pCtrl->TxSeqNum);

    if (Event == SVC_IMG_FEEDER_IMAGE_DATA_READY) {
        if ((pCtrl->Enable == 1U) &&
            (0U == (pCtrl->SrcSeqNum % pCtrl->SrcRateCtrl))) {
            Rval = AppTest_SemTake(Inst);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pInputImg, &pInfo);
                AmbaMisra_TypeCast(&pPicInfo, &pCtrl->pImgInfoBuf[pCtrl->PicInfoIdx]);
                AppTest_PackPicInfo(Inst, pInputImg, pPicInfo);

                /* Prepare feed config */
                Rval = AmbaWrap_memset(&FeedCfg, 0, sizeof(SVC_CV_ALGO_FEED_CFG_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_APPTEST, "memset FeedCfg failed", 0U, 0U);
                }

                for (i = 0; i < pCtrl->NumValidAlgo; i++) {
                    pAlgoOp = &pCtrl->AlgoOp[i];
                    pInBuf = &pCtrl->AlgoOpEx[i].InBuf[pCtrl->OutBufIdx];
                    pOutBuf = &pCtrl->AlgoOpEx[i].OutBuf[pCtrl->OutBufIdx];

                    FeedCfg.pIn  = pInBuf;
                    FeedCfg.pOut = pOutBuf;
                    pInBuf->num_of_buf = 1U;
                    SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_APPTEST, pPicInfo, pCtrl->ImgInfoBufSize, &pInBuf->buf[0]);
                    pOutBuf->num_of_buf = pAlgoOp->OutputNum;
                    for (j = 0U; j < pAlgoOp->OutputNum; j++) {
                        Rval = AmbaWrap_memcpy(&pOutBuf->buf[j], &pCtrl->AlgoOpEx[i].OutputBuf[pCtrl->OutBufIdx][j], sizeof(flexidag_memblk_t));
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_APPTEST, "memcpy OutputBufBlk failed", 0U, 0U);
                        }
                    }

                    if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
                        SvcLog_NG(SVC_LOG_APPTEST, "CONFIG_AMBALINK_BOOT_OS not defined ", 0U, 0U);
                    } else {
                        AmbaMisra_TypeCast(&FeedCfg.pExtFeedCfg, &pPicInfo); /* SCA_OpenOD */
                        AmbaMisra_TypeCast(&FeedCfg.pUserData, &pPicInfo);
                    }

                    Rval |= SvcCvAlgo_Feed(&pAlgoOp->AlgoHandle, &FeedCfg);
                    SVC_CV_DBG(SVC_LOG_APPTEST, "Tx[%u]: Send(%u)", Inst, pPicInfo->pic_info.frame_num);
                }

                if (SVC_OK == Rval) {
                    pCtrl->TxSeqNum++;
                    pCtrl->PicInfoIdx++;
                    if (pCtrl->PicInfoIdx >= SVC_APPTEST_PICINFO_QUEUE_SIZE) {
                        pCtrl->PicInfoIdx = 0;
                    }
                    pCtrl->OutBufIdx++;
                    if (pCtrl->OutBufIdx >= SVC_APPTEST_OUTPUT_DEPTH) {
                        pCtrl->OutBufIdx = 0;
                    }
                } else {
                    SvcLog_NG(SVC_LOG_APPTEST, "Tx[%u]: send error", Inst, 0U);
                }

                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    AppTest_SendReceiverReady(Inst, pPort);
                }
            } else {
                SvcLog_NG(SVC_LOG_APPTEST, "Tx[%u]: SemTake error", Inst, 0U);
            }
        } else {
            if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                AppTest_SendReceiverReady(Inst, pPort);
                Rval = AmbaKAL_TaskSleep(1);
            }
        }

        pCtrl->SrcSeqNum++;
    } else if (Event == SVC_APPTEST_AAA_ALGO_INPUT){
        if (pCtrl->Enable == 1U) {
            Rval = AppTest_SemTake(Inst);
            if (SVC_OK == Rval) {
                /* Prepare feed config */
                Rval = AmbaWrap_memset(&FeedCfg, 0, sizeof(SVC_CV_ALGO_FEED_CFG_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_APPTEST, "memset FeedCfg failed", 0U, 0U);
                }

                for (i = 0; i < pCtrl->NumValidAlgo; i++) {
                    pAlgoOp = &pCtrl->AlgoOp[i];
                    pInBuf = &pCtrl->AlgoOpEx[i].InBuf[pCtrl->OutBufIdx];
                    pOutBuf = &pCtrl->AlgoOpEx[i].OutBuf[pCtrl->OutBufIdx];

                    FeedCfg.pIn  = pInBuf;
                    FeedCfg.pOut = pOutBuf;
                    pInBuf->num_of_buf = 1U;
                    Rval = AmbaWrap_memcpy(pCtrl->pImgInfoBuf[pCtrl->PicInfoIdx], pInfo, pCtrl->ImgInfoBufSize);

                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_APPTEST, "memcpy pImgInfo failed", 0U, 0U);
                    }

                    SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_APPTEST, pCtrl->pImgInfoBuf[pCtrl->PicInfoIdx], pCtrl->ImgInfoBufSize, &pInBuf->buf[0]);
                    pOutBuf->num_of_buf = pAlgoOp->OutputNum;
                    for (j = 0U; j < pAlgoOp->OutputNum; j++) {
                        Rval = AmbaWrap_memcpy(&pOutBuf->buf[j], &pCtrl->AlgoOpEx[i].OutputBuf[pCtrl->OutBufIdx][j], sizeof(flexidag_memblk_t));
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_APPTEST, "memcpy OutputBufBlk failed", 0U, 0U);
                        }
                    }

                    if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
                        SvcLog_NG(SVC_LOG_APPTEST, "CONFIG_AMBALINK_BOOT_OS not defined ", 0U, 0U);
                    } else {
                        AmbaMisra_TypeCast(&FeedCfg.pUserData, &pInfo);
                    }

                    Rval |= SvcCvAlgo_Feed(&pAlgoOp->AlgoHandle, &FeedCfg);
                    SVC_CV_DBG(SVC_LOG_APPTEST, "Tx[%u]: Send(%u)", Inst, 0U);
                }

                if (SVC_OK == Rval) {
                    pCtrl->TxSeqNum++;
                    pCtrl->PicInfoIdx++;
                    if (pCtrl->PicInfoIdx >= SVC_APPTEST_PICINFO_QUEUE_SIZE) {
                        pCtrl->PicInfoIdx = 0;
                    }
                    pCtrl->OutBufIdx++;
                    if (pCtrl->OutBufIdx >= SVC_APPTEST_OUTPUT_DEPTH) {
                        pCtrl->OutBufIdx = 0;
                    }
                } else {
                    SvcLog_NG(SVC_LOG_APPTEST, "Tx[%u]: send error", Inst, 0U);
                }

                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    AppTest_SendReceiverReady(Inst, pPort);
                }
            } else {
                SvcLog_NG(SVC_LOG_APPTEST, "Tx[%u]: SemTake error", Inst, 0U);
            }
        } else {
            if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                AppTest_SendReceiverReady(Inst, pPort);
                Rval = AmbaKAL_TaskSleep(1);
            }
        }

        pCtrl->SrcSeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_APPTEST, "Tx[%u]: unsupported event(0x%x)", Inst, Event);
        Rval = SVC_NG;
    }

    SVC_CV_DBG(SVC_LOG_APPTEST, "Tx[%u]: end", Inst, 0U);

    return Rval;

}

static UINT32 AppTest_TxDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_APPTEST_CTRL_s *pCtrl = &g_AppTestCtrl[SVC_APPTEST_INSTANCE0];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_APPTEST_INSTANCE0, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 AppTest_TxDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_APPTEST_CTRL_s *pCtrl = &g_AppTestCtrl[SVC_APPTEST_INSTANCE1];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_APPTEST_INSTANCE1, pPort, Event, pInfo);
    }
    return Rval;
}

static void AppTest_DumpResult(const amba_mask_out_t *pSegResult)
{
    static UINT32 dumptimes = 0;
    ULONG BufAddr;

    if (dumptimes > 0U) {
        AmbaMisra_TypeCast(&BufAddr, &pSegResult->buff_addr);
        AmbaPrint_PrintUInt5("[AppTest_DumpResult] ======================", 0U, 0U, 0U, 0U, 0U);
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
static UINT32 AppTest_AlgoOutCallback(UINT32 Inst, UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Rval = SVC_OK;
    SVC_APPTEST_CTRL_s *pCtrl = &g_AppTestCtrl[Inst];
    APPTEST_OUTPUT_MSG_s Msg;
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
                SvcLog_NG(SVC_LOG_APPTEST, "memcpy AlgoOut failed", 0U, 0U);
            }
        }
        Rval = AmbaKAL_MsgQueueSend(&pCtrl->AlgoOutQueId, &Msg, 5000);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_APPTEST, "AppTest_AlgoOutCallback[%u]: MsgQueueSend error", Inst, 0U);
        }
    }

    return Rval;
}

static UINT32 AppTest_AlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return AppTest_AlgoOutCallback(SVC_APPTEST_INSTANCE0, Event, pEventData);
}

static UINT32 AppTest_AlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return AppTest_AlgoOutCallback(SVC_APPTEST_INSTANCE1, Event, pEventData);
}

/**
* RX data sink handler: process detection output
*/
static UINT32 AppTest_RxDataSinkHandler(UINT32 Inst)
{
    UINT32 Rval;
    SVC_APPTEST_CTRL_s *pCtrl = &g_AppTestCtrl[Inst];
    APPTEST_OUTPUT_MSG_s Msg;

    const SVC_CV_ALGO_OUTPUT_s *pAlgoOut = &Msg.AlgoOut;
    const amba_mask_out_t *pMaskOut = NULL;
    SVC_CV_DETRES_SEG_s SegResult;
    const SVC_CV_FLOW_APPTEST_MODE_INFO_s *pModeInfo = &SvcCvFlow_AppTest_ModeInfo[pCtrl->pCvFlow->CvModeID];
    UINT8 OutputType = pModeInfo->OutputType;
    UINT8 OutputBufIdx = pModeInfo->OutputBufIdx;

    SVC_CV_DBG(SVC_LOG_APPTEST, "Rx[%u]: run %d", Inst, pCtrl->RxSeqNum);

    Rval = AmbaKAL_MsgQueueReceive(&pCtrl->AlgoOutQueId, &Msg, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK == Rval) {
        SVC_CV_DBG(SVC_LOG_APPTEST, "Rx[%u]: recv out(%u)", Inst, 0);

        if (Msg.Event == CALLBACK_EVENT_OUTPUT) {
            if (pCtrl->Enable == 1U) {
                if (OutputType == APPTEST_OUTPUT_TYPE_MASK_OUT){
                    AmbaMisra_TypeCast(&pMaskOut, &pAlgoOut->pOutput->buf[OutputBufIdx].pBuffer);
                    AppTest_DumpResult(pMaskOut);

                    SegResult.MsgCode     = SVC_CV_DETRES_SEG;
                    SegResult.Source      = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
                    SegResult.Width       = pMaskOut->mask_width;
                    SegResult.Height      = pMaskOut->mask_height;
                    SegResult.Pitch       = pMaskOut->mask_width;
                    if (NULL != pAlgoOut->pUserData) {
                        const memio_source_recv_picinfo_t *pInPicInfo;
                        AmbaMisra_TypeCast(&pInPicInfo, &pAlgoOut->pUserData);
                        SegResult.FrameNum    = pInPicInfo->pic_info.frame_num;
                        SegResult.CaptureTime = pInPicInfo->pic_info.capture_time;
                    } else {
                        SegResult.FrameNum    = 0U;
                        SegResult.CaptureTime = 0U;
                    }

                    AmbaMisra_TypeCast(&(SegResult.pCvOut), &pMaskOut);
                    AmbaMisra_TypeCast(&(SegResult.pBuf), &pMaskOut->buff_addr);

                    Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                           SVC_CV_FLOW_OUTPUT_PORT0,
                                           pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                           &SegResult);

                    if (SVC_OK == Rval) {
                        pCtrl->SinkInfoIdx++;
                        if (pCtrl->SinkInfoIdx >= SVC_APPTEST_OUTPUT_DEPTH) {
                            pCtrl->SinkInfoIdx = 0;
                        }
                    }
                } else if (OutputType == APPTEST_OUTPUT_TYPE_AAA_ALGO){
                    if ((pModeInfo->FlowCtrl & APPTEST_FLOW_AAA_ALGO) > 0U) {
 #if defined(CONFIG_BUILD_IMGFRW_AAA) && defined(CONFIG_SOC_CV2FS) && defined(CONFIG_BUILD_IP_MONITOR)
                    UINT32 VinID, SensorIdx, AlgoNum;
                    UINT32 AlgoId[AMBA_DSP_MAX_VIEWZONE_NUM]={0U};
                    /* Send result to AAA algo */
                    AMBA_IMGPROC_FlEXIDAG_RESULT_t *pAAATmp;
                    AMBA_IP_MONITOR_AE_CMPR_s AeCmpr={0};
                    AMBA_IP_MONITOR_AWB_CMPR_s AwbCmpr={0};
                    AMBA_IP_MONITOR_ADJ_CMPR_s AdjCmpr={0};    
                                  
                    AmbaMisra_TypeCast(&(pAAATmp), &pAlgoOut->pOutput->buf[OutputBufIdx].pBuffer);

                    Rval = SvcResCfg_GetSensorIdxOfFovIdx(pCtrl->pCvFlow->InputCfg.Input[0].StrmId, &VinID, &SensorIdx);
                    // SvcLog_NG(SVC_LOG_APPTEST, "###Rval:  %d, ", Rval, 0U);
                    if (SVC_OK == Rval) {                     
                        Rval = SvcImg_AlgoIdGetEx(VinID, SensorIdx, AlgoId, &AlgoNum);               
                        if (SVC_OK == Rval) {                          
                            Rval = AmbaImgProc_SetAsilProcessState(AlgoId[0], 0U);
                        }
                    }
                        Rval = AmbaImgProc_SetAsilProcessState(AlgoId[0], 0U);
                        AeCmpr.ModuleId = IPMONAE_ERR_BASE;
                        AeCmpr.Result = pAAATmp->AeResult.Result;
                        // AeCmpr.UUID = UUID;
                        if(pAAATmp->AeResult.Result != 0){
                            SvcLog_NG(SVC_LOG_APPTEST, "AeResult:  %d, ", (UINT32)pAAATmp->AeResult.Result, 0U);
                        }
                        // SvcLog_NG(SVC_LOG_APPTEST, "AeResult:  %d, ", (UINT32)pAAATmp->AeResult.Result, 0U);
                        (void)AmbaIP_MonitorAeCmpr(&AeCmpr);

                        AwbCmpr.ModuleId = IPMONAWB_ERR_BASE;
                        AwbCmpr.Result = pAAATmp->AwbResult.Result;
                        // AeCmpr.UUID = UUID;
                        if(pAAATmp->AwbResult.Result != 0){
                            SvcLog_NG(SVC_LOG_APPTEST, "AwbResult:  %d, ", (UINT32)pAAATmp->AwbResult.Result, 0U);
                        }   
                        // SvcLog_NG(SVC_LOG_APPTEST, "AwbResult:  %d, ", (UINT32)pAAATmp->AwbResult.Result, 0U);                     
                        (void)AmbaIP_MonitorAwbCmpr(&AwbCmpr);

                        AdjCmpr.ModuleId = IPMONADJ_ERR_BASE;
                        AdjCmpr.Result = pAAATmp->AdjResult.Result;
                        // AeCmpr.UUID = UUID;
                        if(pAAATmp->AdjResult.Result != 0){
                            SvcLog_NG(SVC_LOG_APPTEST, "AdjResult:  %d, ", (UINT32)pAAATmp->AdjResult.Result, 0U);
                        }   
                        // SvcLog_NG(SVC_LOG_APPTEST, "AdjResult:  %d, ", (UINT32)pAAATmp->AdjResult.Result, 0U);                     
                        (void)AmbaIP_MonitorAdjCmpr(&AdjCmpr);                        
#endif
                    } else {
                        SegResult.MsgCode     = SVC_CV_DETRES_SEG;
                        SegResult.Source      = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
                        SegResult.Width       = 320U;
                        SegResult.Height      = 128U;
                        SegResult.Pitch       = 320U;
                        SegResult.FrameNum    = 0U;
                        SegResult.CaptureTime = 0U;
                        SegResult.pCvOut      = NULL;
                        AmbaMisra_TypeCast(&(SegResult.pBuf), &pAlgoOut->pOutput->buf[OutputBufIdx].pBuffer);

                        Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                               SVC_CV_FLOW_OUTPUT_PORT0,
                                               pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                               &SegResult);

                        if (SVC_OK == Rval) {
                            pCtrl->SinkInfoIdx++;
                            if (pCtrl->SinkInfoIdx >= SVC_APPTEST_OUTPUT_DEPTH) {
                                pCtrl->SinkInfoIdx = 0;
                            }
                        }
                    }
                } else {
                    SvcLog_NG(SVC_LOG_APPTEST, "Rx[%u]: Unsupport output type", Inst, 0U);
                }
            }

            Rval = AppTest_SemGive(Inst);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_APPTEST, "Rx[%u]: SemGive error", Inst, 0U);
            }

            if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                AppTest_SendReceiverReady(Inst, &pCtrl->FeederPort);
            }
        }

        pCtrl->RxSeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_APPTEST, "Rx[%u]: recv error", Inst, 0U);
    }

    SVC_CV_DBG(SVC_LOG_APPTEST, "Rx[%u]: end", Inst, 0U);

    return SVC_OK;
}

/**
* Task entry of RX process
*/
static void* AppTest_RxTaskEntry(void* EntryArg)
{
    const ULONG  *pArg;
    UINT32 Rval = SVC_OK, Inst;
    const SVC_APPTEST_CTRL_s *pCtrl;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Inst = (UINT32)(*pArg);
    pCtrl = &g_AppTestCtrl[Inst];

    SvcLog_DBG(SVC_LOG_APPTEST, "RxTaskEntry[%d] start", Inst, 0U);

    while (SVC_OK == Rval) {
        if (NULL != pCtrl->pRxProc) {
            Rval = pCtrl->pRxProc(Inst);
        } else {
            Rval = SVC_NG;
        }
    }

    SvcLog_DBG(SVC_LOG_APPTEST, "RxTaskEntry[%d] exit", Inst, 0U);

    return NULL;
}

/**
* Set and wait TX and RX task to idle state
*/
static UINT32 AppTest_SetTaskIdle(UINT32 Inst)
{
    UINT32 Rval;
    SVC_APPTEST_CTRL_s *pCtrl = &g_AppTestCtrl[Inst];
    UINT32 i;

    /* Disable input and output */
    pCtrl->Enable = 0U;

    if (SVC_OK != SvcImgFeeder_Disconnect(&pCtrl->FeederPort)) {
        SvcLog_NG(SVC_LOG_APPTEST, "Disconn feeder port error", 0U, 0U);
    }

    if (SVC_OK != SvcImgFeeder_ClosePort(&pCtrl->FeederPort)) {
        SvcLog_NG(SVC_LOG_APPTEST, "Close feeder port error", 0U, 0U);
    }

    /* Wait Rx idle */
    for (i = 0; i < SVC_APPTEST_SEM_INIT_COUNT; i++) {
        Rval = AppTest_SemTake(Inst);
    }

    return Rval;
}

/**
* Send external Input to driver
*/
 #if defined(CONFIG_BUILD_IMGFRW_AAA) && defined(CONFIG_SOC_CV2FS) && defined(CONFIG_BUILD_IP_MONITOR)
static UINT32 AppTest_AAAFlexidagInputCb(void *pInput, UINT32 DataSize)
{
    UINT32 Rval = SVC_OK;
    SVC_APPTEST_CTRL_s *pCtrl = &g_AppTestCtrl[0U];
    UINT32 VinID, SensorIdx, AlgoNum;
    UINT32 AlgoId[AMBA_DSP_MAX_VIEWZONE_NUM]={0U};
    Rval = SvcResCfg_GetSensorIdxOfFovIdx(pCtrl->pCvFlow->InputCfg.Input[0].StrmId, &VinID, &SensorIdx);
    if (SVC_OK == Rval) {
        Rval = SvcImg_AlgoIdGetEx(VinID, SensorIdx, AlgoId, &AlgoNum);
    }

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(0U, &pCtrl->FeederPort, SVC_APPTEST_AAA_ALGO_INPUT, pInput);
        if(SVC_OK == Rval){
            Rval = AmbaImgProc_SetAsilProcessState(AlgoId[0], 1U);
        }
    }

    AmbaMisra_TouchUnused(&DataSize);
    return Rval;
}
#endif
/**
* Entry of configuration function
*/
static UINT32 AppTest_Config(UINT32 Inst, UINT32 AlgoIdx)
{
    UINT32 Rval = SVC_OK;

    AmbaMisra_TouchUnused(&Inst);
    AmbaMisra_TouchUnused(&AlgoIdx);

    return Rval;
}

static UINT32 AppTest_ConfigBuffer(UINT32 Inst)
{
    UINT32 Rval = SVC_OK;
    SVC_APPTEST_CTRL_s *pCtrl = &g_AppTestCtrl[Inst];
    SVC_CV_FLOW_BUF_INFO_s BufInfo = {0};
    UINT32 i, DataSzie;
    const SVC_CV_FLOW_APPTEST_MODE_INFO_s *pModeInfo = &SvcCvFlow_AppTest_ModeInfo[pCtrl->pCvFlow->CvModeID];

    /* Img info buffer */
    if (pCtrl->pImgInfoBuf[0] == NULL) {
        if ((pModeInfo->FlowCtrl & APPTEST_FLOW_AAA_ALGO) > 0U) {
#if defined(CONFIG_BUILD_IMGFRW_AAA) && defined(CONFIG_SOC_CV2FS)
            DataSzie = sizeof(AMBA_AAA_FLEXIDAG_INFO_s);
#else
        {
            #define AAA_ALGO_MAX_INPUT_SIZE     (512U)
            DataSzie = AAA_ALGO_MAX_INPUT_SIZE;
        }
#endif
        } else {
            DataSzie = (UINT32)sizeof(memio_source_recv_picinfo_t);
        }

        for (i = 0; i < SVC_APPTEST_PICINFO_QUEUE_SIZE; i++) {
            Rval = SvcCvFlow_AllocWorkBuf(DataSzie, &BufInfo);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pCtrl->pImgInfoBuf[i], &BufInfo.pAddr);
                pCtrl->ImgInfoBufSize = BufInfo.Size;
            } else {
                SvcLog_NG(SVC_LOG_APPTEST, "Alloc Img info buf failed", 0U, 0U);
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
static UINT32 CvFlow_AppTestInit(UINT32 CvFlowChan, void *pInit)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    static UINT32 g_AppTestInit = 0U;

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(pInit);

    if (0U == g_AppTestInit) {
        g_AppTestInit = 1U;

        if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&AppTestMutex, NULL)) {
            SvcLog_NG(SVC_LOG_APPTEST, "MutexCreate error", 0U, 0U);
            Rval = SVC_NG;
        }
        for (i = 0; i < SVC_APPTEST_MAX_INSTANCE; i++) {
            g_AppTestCtrl[i].Used = 0;
        }

        SVC_CV_DBG(SVC_LOG_APPTEST, "CvFlow_AppTestInit: first init", 0U, 0U);
    }

    SvcLog_OK(SVC_LOG_APPTEST, "CvFlow_AppTestInit: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver configuration function
*/
static UINT32 CvFlow_AppTestConfig(UINT32 CvFlowChan, void *pConfig)
{
    UINT32 Rval = SVC_OK;
    UINT32 i, j;
    SVC_APPTEST_CTRL_s *pCtrl = NULL;
    const SVC_CV_FLOW_APPTEST_MODE_INFO_s *pModeInfo;
    SVC_CV_INPUT_CFG_s *pInputCfg;

    AmbaMisra_TouchUnused(pConfig);

    if (NULL != pConfig) {
        AppTest_MutexTake();
        /* Create instance */
        for (i = 0; i < SVC_APPTEST_MAX_INSTANCE; i++) {
            if (0U == g_AppTestCtrl[i].Used) {
                Rval = AmbaWrap_memset(&g_AppTestCtrl[i], 0, sizeof(SVC_APPTEST_CTRL_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_APPTEST, "memset g_AppTestCtrl[%u] failed", i, 0U);
                }
                pCtrl = &g_AppTestCtrl[i];
                pCtrl->Used = 1;
                SvcLog_DBG(SVC_LOG_APPTEST, "Create Inst(%u)", i, 0U);
                break;
            }
        }
        AppTest_MutexGive();

        /* Configure instance */
        if (NULL != pCtrl) {
            pCtrl->CvfChan     = (UINT8) CvFlowChan;
            pCtrl->pCvFlow     = &pCtrl->CvCfg;
            Rval = AmbaWrap_memcpy(&pCtrl->CvCfg, pConfig, sizeof(SVC_CV_FLOW_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_APPTEST, "memcpy CvCfg failed", 0U, 0U);
            }
            pCtrl->SrcRateCtrl = 1;
            pCtrl->Enable      = 1;

            pModeInfo = &SvcCvFlow_AppTest_ModeInfo[pCtrl->pCvFlow->CvModeID];
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
                SvcLog_NG(SVC_LOG_APPTEST, "unsupported InputNum(%u), NumRoi(%u)", pInputCfg->InputNum, pInputCfg->Input[0].RoiInfo.NumRoi);
            } else {
                if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG) {
                    Rval = SVC_OK;
                } else if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
                    Rval = SVC_NG;
                } else {
                    SvcLog_NG(SVC_LOG_APPTEST, "unsupported CVFlow ProfID(%u)", pModeInfo->ProfID, 0U);
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
        SvcLog_OK(SVC_LOG_APPTEST, "CvFlow_AppTestConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_APPTEST, "CvFlow_AppTestConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver load function
*/
static UINT32 CvFlow_AppTestLoad(UINT32 CvFlowChan, void *pInfo)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_APPTEST_CTRL_s *pCtrl;
    const SVC_CV_FLOW_LOAD_INFO_s *pLoadInfo;
    const SVC_CV_FLOW_APPTEST_MODE_INFO_s *pModeInfo;
    UINT32 i;

    AmbaMisra_TouchUnused(pInfo);

    Rval = AppTest_CvfChan2Inst(CvFlowChan, &Inst);
    if (Rval == SVC_OK) {
        pCtrl = &g_AppTestCtrl[Inst];
        if (NULL != pInfo) {
            AmbaMisra_TypeCast(&pLoadInfo, &pInfo);
            if (pLoadInfo->NumBin > 0U) {
                /* Use external path */
                if (pLoadInfo->AlgoIdx < APPTEST_MAX_ALGO_NUM) {
                    Rval = SvcCvFlow_LoadAlgoBin(SVC_LOG_APPTEST, pLoadInfo, &pCtrl->AlgoOp[pLoadInfo->AlgoIdx]);
                } else {
                    SvcLog_NG(SVC_LOG_APPTEST, "CvFlow_AppTestLoad: CvFlowChan(%d) invalid AlgoIdx(%u)", CvFlowChan, pLoadInfo->AlgoIdx);
                }
            } else {
                /* Use ModeInfo */
                if (pCtrl->pCvFlow->CvModeID < APPTEST_NUM_MODE) {
                    pModeInfo = &SvcCvFlow_AppTest_ModeInfo[pCtrl->pCvFlow->CvModeID];

                    for (i = 0; i < pModeInfo->AlgoGrp.NumAlgo; i++) {
                        Rval |= SvcCvFlow_LoadAlgo(SVC_LOG_APPTEST, &pModeInfo->AlgoGrp.AlgoInfo[i], &pCtrl->AlgoOp[i]);
                    }
                } else  {
                    SvcLog_NG(SVC_LOG_APPTEST, "CvFlow_AppTestLoad: CvFlowChan(%d) invalid CvModeID(%u)", CvFlowChan, pCtrl->pCvFlow->CvModeID);
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_APPTEST, "CvFlow_AppTestLoad: invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_APPTEST, "CvFlow_AppTestLoad: CvFlowChan(%d) error", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver start function
*/
static UINT32 CvFlow_AppTestStart(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst, i, j, AlgoIdx, Num;
    SVC_APPTEST_CTRL_s *pCtrl;
    const SVC_CV_FLOW_APPTEST_MODE_INFO_s *pModeInfo;
    SVC_CV_FLOW_BUF_INFO_s BufInfo = {0};
    const SVC_CV_FLOW_ALGO_INFO_s *pAlgoInfo;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
    SVC_CV_FLOW_ALGO_QUERY_CFG_s QueryCfg;
    SVC_CV_FLOW_ALGO_CREATE_CFG_s CreateCfg;
    SVC_CV_ALGO_REGCB_CFG_s CbCfg;
    SVC_IMG_FEEDER_PORT_CFG_s PortCfg = {0};
    static char CbEvtQueName[] = "AppTestCbEvtQue";
    static char SemName[] = "OutBufSem";
    static APPTEST_OUTPUT_MSG_s AlgoOutQue[SVC_APPTEST_MAX_INSTANCE][SVC_APPTEST_ALGO_OUT_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 SegRxTaskStack[SVC_APPTEST_MAX_INSTANCE][SVC_APPTEST_RX_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_IMG_FEEDER_EVENT_CALLBACK_f AppTest_TxDataSrcHandlerList[SVC_APPTEST_MAX_INSTANCE] = {
        AppTest_TxDataSrcHandler0,
        AppTest_TxDataSrcHandler1,
    };
    static SVC_CV_ALGO_CALLBACK_f AppTest_AlgoOutCbList[SVC_APPTEST_MAX_INSTANCE] = {
        AppTest_AlgoOutCallback0,
        AppTest_AlgoOutCallback1,
    };

    AmbaMisra_TouchUnused(pInput);
    AmbaMisra_TouchUnused(AppTest_TxDataSrcHandlerList);
    AmbaMisra_TouchUnused(AppTest_AlgoOutCbList);

    Rval = AppTest_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl     = &g_AppTestCtrl[Inst];
        pModeInfo = &SvcCvFlow_AppTest_ModeInfo[pCtrl->pCvFlow->CvModeID];

        /* Open Feeder port first to enable frame FIFO earlier */
        if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
            /* The driver supports single input per instance */
            PortCfg.NumPath            = 1U;
            PortCfg.Content[0].DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
            PortCfg.Content[0].StrmId  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
            PortCfg.SendFunc           = AppTest_TxDataSrcHandlerList[Inst];
            if (SVC_OK != SvcImgFeeder_OpenPort(pCtrl->pCvFlow->InputCfg.FeederID, &PortCfg, &pCtrl->FeederPort)) {
                SvcLog_NG(SVC_LOG_APPTEST, "Open feeder port error", 0U, 0U);
            }
        } else {
            if (pCtrl->pCvFlow->InputCfg.Input[0].DataSrc != SVC_CV_DATA_SRC_EXT_DATA) {
                SvcLog_DBG(SVC_LOG_APPTEST, "ImgFeeder is not enabled", 0U, 0U);
            }
        }

        for (AlgoIdx = 0; AlgoIdx < pModeInfo->AlgoGrp.NumAlgo; AlgoIdx++) {
            pAlgoInfo = &pModeInfo->AlgoGrp.AlgoInfo[AlgoIdx];
            pAlgoOp   = &pCtrl->AlgoOp[AlgoIdx];

            Rval = AmbaWrap_memset(&QueryCfg, 0, sizeof(SVC_CV_FLOW_ALGO_QUERY_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_APPTEST, "memset QueryCfg failed", 0U, 0U);
            }
            Rval = SvcCvFlow_QueryAlgo(SVC_LOG_APPTEST, pAlgoInfo, &QueryCfg, pAlgoOp);
            if (SVC_OK == Rval) {
                /* Create algo */
                Rval = AmbaWrap_memset(&CreateCfg, 0, sizeof(SVC_CV_FLOW_ALGO_CREATE_CFG_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_APPTEST, "memset CreateCfg failed", 0U, 0U);
                }

                if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG) {
                    /* Do nothing */
                } else if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
                    SvcLog_NG(SVC_LOG_APPTEST, "CONFIG_AMBALINK_BOOT_OS not defined ", 0U, 0U);
                    Rval = SVC_NG;
                } else {
                    SvcLog_NG(SVC_LOG_APPTEST, "unsupported CVFlow ProfID(%u)", pModeInfo->ProfID, 0U);
                    Rval = SVC_NG;
                }

                if (SVC_OK == Rval) {
                    Rval = SvcCvFlow_CreateAlgo(SVC_LOG_APPTEST, pAlgoInfo, &CreateCfg, pAlgoOp);
                }

                if (SVC_OK == Rval) {
                    /* Create Output buffer */
                    for (i = 0; i < SVC_APPTEST_OUTPUT_DEPTH; i++) {
                        Num = pAlgoOp->OutputNum;
                        for (j = 0; j < Num; j++) {
                            Rval |= SvcCvFlow_AllocWorkBuf(pAlgoOp->OutputSize[j], &BufInfo);
                            SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_APPTEST, BufInfo.pAddr, BufInfo.Size, &pCtrl->AlgoOpEx[AlgoIdx].OutputBuf[i][j]);
                        }
                    }

                    if (SVC_OK == Rval) {
                        /* Register Algo callback */
                        Rval = AmbaWrap_memset(&CbCfg, 0, sizeof(SVC_CV_ALGO_REGCB_CFG_s));
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_APPTEST, "memset CbCfg failed", 0U, 0U);
                        }
                        CbCfg.Mode     = 0U;
                        CbCfg.Callback = AppTest_AlgoOutCbList[Inst];
                        Rval = SvcCvAlgo_RegCallback(&pAlgoOp->AlgoHandle, &CbCfg);
                        if (SVC_OK == Rval) {
                            Rval = AppTest_Config(Inst, AlgoIdx);
                            if (SVC_OK != Rval) {
                                SvcLog_NG(SVC_LOG_APPTEST, "FlexiDAG Config error(0x%X)", Rval, 0U);
                            }

                            pCtrl->NumValidAlgo++;
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_APPTEST, "Alloc Algo[%u] Output Buf failed", AlgoIdx, 0U);
                    }
                } else {
                   SvcLog_NG(SVC_LOG_APPTEST, "CreateAlgo[%u] error", AlgoIdx, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_APPTEST, "QueryAlgo[%u] error", AlgoIdx, 0U);
            }
        }

        if (SVC_OK == Rval) {
            Rval = AppTest_ConfigBuffer(Inst);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_APPTEST, "ConfigBuffer error", 0U, 0U);
            }
        }

        if (SVC_OK == Rval) {
            pCtrl->pTxProc = AppTest_TxDataSrcHandler;
            pCtrl->pRxProc = AppTest_RxDataSinkHandler;

            Rval = AmbaKAL_MsgQueueCreate(&pCtrl->AlgoOutQueId,
                                           CbEvtQueName,
                                            (UINT32)sizeof(APPTEST_OUTPUT_MSG_s),
                                           &AlgoOutQue[Inst][0],
                                           SVC_APPTEST_ALGO_OUT_QUEUE_SIZE * (UINT32)sizeof(APPTEST_OUTPUT_MSG_s));

            Rval |= AmbaKAL_SemaphoreCreate(&pCtrl->OutBufSem, SemName, SVC_APPTEST_SEM_INIT_COUNT);

            if (KAL_ERR_NONE == Rval) {
                pCtrl->RxTaskCtrl.Priority   = 60;
                pCtrl->RxTaskCtrl.EntryFunc  = AppTest_RxTaskEntry;
                pCtrl->RxTaskCtrl.EntryArg   = Inst;
                pCtrl->RxTaskCtrl.pStackBase = &SegRxTaskStack[Inst][0];
                pCtrl->RxTaskCtrl.StackSize  = SVC_APPTEST_RX_TASK_STACK_SIZE;
                pCtrl->RxTaskCtrl.CpuBits    = 0x01U;
                Rval |= SvcTask_Create("SvcSegRxTask", &pCtrl->RxTaskCtrl);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_APPTEST, "Create RxTask Inst(%u) error", Inst, 0U);
                }

                /* Start input */
                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_APPTEST, "Connect feeder port error", 0U, 0U);
                    }
                } else if ((pModeInfo->FlowCtrl & APPTEST_FLOW_AAA_ALGO) > 0U) {
 #if defined(CONFIG_BUILD_IMGFRW_AAA) && defined(CONFIG_SOC_CV2FS) && defined(CONFIG_BUILD_IP_MONITOR)

                    UINT32 VinID, SensorIdx, AlgoNum;
                    UINT32 AlgoId[AMBA_DSP_MAX_VIEWZONE_NUM];
                    IMG_PROC_ASIL_FUNC_s IpFuncTmp = {NULL};
                    AMBA_IP_MONITOR_CONFIG_s    MonCfg;                    
                    AMBA_AAA_ASIL_OP_INFO_s  AsilOpInfo;

                    Rval = SvcResCfg_GetSensorIdxOfFovIdx(pCtrl->pCvFlow->InputCfg.Input[0].StrmId, &VinID, &SensorIdx);
                    if (SVC_OK == Rval) {
                        Rval = SvcImg_AlgoIdGetEx(VinID, SensorIdx, AlgoId, &AlgoNum);
                        if (SVC_OK == Rval) {
                            Rval = AmbaImgProc_GetAsilRegFunc(AlgoId[0], &IpFuncTmp);
                            if (SVC_OK == Rval) {
                                IpFuncTmp.AAAFlexidagInputCb = AppTest_AAAFlexidagInputCb;
                                IpFuncTmp.AmbaIP_GetMonitorConfig = AmbaIP_GetMonitorCfg;
                                if (0U != AmbaImgProc_SetAsilRegFunc(AlgoId[0], &IpFuncTmp)) {
                                    SvcLog_NG(SVC_LOG_APPTEST, "AmbaImgProc_SetAsilRegFunc failed", 0U, 0U);
                                    Rval = SVC_NG;
                                }

                                if(SVC_OK == Rval){
                                    Rval = AmbaImgProc_GetAAAAsilOPInfo(AlgoId[0], &AsilOpInfo);
                                    if(SVC_OK == Rval){
                                        Rval = IpFuncTmp.AmbaIP_GetMonitorConfig((IPMONAE_ERR_BASE >> 16U),&MonCfg);
                                        if(SVC_OK == Rval){
                                            if((MonCfg.BitMask & 0x01U)==0x01U){
                                                AsilOpInfo.Ae = 1U;
                                                Rval = AmbaImgProc_SetAAAAsilOPInfo(AlgoId[0], &AsilOpInfo);
                                            }
                                        }
                                        if(SVC_OK == Rval){
                                            Rval = IpFuncTmp.AmbaIP_GetMonitorConfig((IPMONAWB_ERR_BASE >> 16U),&MonCfg);
                                            if(SVC_OK == Rval){
                                                if((MonCfg.BitMask & 0x01U)==0x01U){
                                                    AsilOpInfo.Awb = 1U;
                                                    Rval = AmbaImgProc_SetAAAAsilOPInfo(AlgoId[0], &AsilOpInfo);
                                                }
                                            }
                                        }
                                        if(SVC_OK == Rval){
                                            Rval = IpFuncTmp.AmbaIP_GetMonitorConfig((IPMONADJ_ERR_BASE >> 16U),&MonCfg);
                                            if(SVC_OK == Rval){
                                                if((MonCfg.BitMask & 0x01U)==0x01U){
                                                    AsilOpInfo.Adj = 1U;
                                                    Rval = AmbaImgProc_SetAAAAsilOPInfo(AlgoId[0], &AsilOpInfo);
                                                }
                                            }                                    
                                        }
                                    }
                                }

                            }else{
                                SvcLog_NG(SVC_LOG_APPTEST, "AmbaImgProc_GetAsilRegFunc failed", 0U, 0U);
                                Rval = SVC_NG;
                            }
                        } else {
                            SvcLog_NG(SVC_LOG_APPTEST, "SvcImg_AlgoIdGetEx failed", 0U, 0U);
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_APPTEST, "SvcResCfg_GetSensorIdxOfFovIdx failed", 0U, 0U);
                    }
 #endif
                } else {
                    /* Do nothing */
                }

                if (SVC_OK == Rval) {
                    pCtrl->Started = 1U;
                }
            } else {
                SvcLog_NG(SVC_LOG_APPTEST, "Create MsgQue/Semaphore error(0x%X)", Rval, 0U);
                Rval =  SVC_NG;
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_APPTEST, "Invalid CvFlowChan", 0U, 0U);
        Rval =  SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_APPTEST, "CvFlow_AppTestStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_APPTEST, "CvFlow_AppTestStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver control function
*/
static UINT32 CvFlow_AppTestControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo)
{
    UINT32 Rval, Inst, Index;
    SVC_APPTEST_CTRL_s *pCtrl;
    const UINT32 *pValue;
    SVC_CV_ROI_INFO_s  *pRoiInfo = NULL;
    SVC_CV_MASK_INFO_s *pMaskInfo = NULL;
    const SVC_CV_FLOW_APPTEST_MASK_CFG_s *pMaskCfg;

    AppTest_MutexTake();

    Rval = AppTest_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl = &g_AppTestCtrl[Inst];
        switch (CtrlType) {
            case SVC_CV_CTRL_ENABLE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->Enable = (UINT8) *pValue;
                if ((1U == pCtrl->Started) &&
                    (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder)) {
                    if (*pValue == 0U) {
                        Rval = SvcImgFeeder_Disconnect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_APPTEST, "Disconn feeder port error", 0U, 0U);
                        }
                    } else {
                        Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_APPTEST, "Cnnect feeder port error", 0U, 0U);
                        }
                    }
                }
                break;
            case SVC_CV_CTRL_SET_SRC_RATE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->SrcRateCtrl = *pValue;
                SvcLog_OK(SVC_LOG_APPTEST, "Control[%d]: SrcRate = %u", Inst, *pValue);
                break;
            case SVC_CV_CTRL_GET_ROI_INFO:
                AmbaMisra_TypeCast(&pRoiInfo, &pInfo);
                if (pCtrl->pCvFlow->InputCfg.Input[0].RoiInfo.NumRoi > 0U) {
                    Rval = AmbaWrap_memcpy(pRoiInfo, &pCtrl->pCvFlow->InputCfg.Input[0].RoiInfo, sizeof(SVC_CV_ROI_INFO_s));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_APPTEST, "memcpy pRoiInfo failed", 0U, 0U);
                    }
                } else {
                    Rval = AmbaWrap_memset(pRoiInfo, 0x0, sizeof(SVC_CV_ROI_INFO_s));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_APPTEST, "memset pRoiInfo failed", 0U, 0U);
                    }
                }
                break;
            case SVC_CV_CTRL_GET_MASK_INFO:
                Index = pCtrl->pCvFlow->CvModeID;
                AmbaMisra_TypeCast(&pMaskInfo, &pInfo);
                pMaskInfo->NumRoi = 0U;
                pMaskCfg = SvcCvFlow_AppTest_ModeInfo[Index].Cfg[0].pMaskCfg;
                if (NULL != pMaskCfg) {
                    pMaskInfo->NumRoi = 1U;
                    pMaskInfo->Roi[0].NumCat = pMaskCfg->NumCat;
                    pMaskInfo->Roi[0].Width  = pMaskCfg->BufWidth;
                    pMaskInfo->Roi[0].Height = pMaskCfg->BufHeight;
                } else {
                    Rval = AmbaWrap_memset(&pMaskInfo->Roi[0], 0x0, sizeof(SVC_CV_MASK_s));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_APPTEST, "memset pMaskInfo failed", 0U, 0U);
                    }
                }
                break;
            default:
                SvcLog_NG(SVC_LOG_APPTEST, "Control[%d]: Invalid CtrlType(%d)", Inst, CtrlType);
                break;
        }
    } else {
        SvcLog_NG(SVC_LOG_APPTEST, "Control: Invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    AppTest_MutexGive();

    return Rval;
}

/**
* CvFlow driver stop function
*/
static UINT32 CvFlow_AppTestStop(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_APPTEST_CTRL_s *pCtrl;
    SVC_CV_FLOW_ALGO_DELETE_CFG_s DelCfg;
    UINT32 i;

    AmbaMisra_TouchUnused(pInput);

    Rval = AppTest_CvfChan2Inst(CvFlowChan, &Inst);

    if (SVC_OK == Rval) {
        pCtrl = &g_AppTestCtrl[Inst];

        Rval = AppTest_SetTaskIdle(Inst);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_APPTEST, "SetTaskIdle error", 0U, 0U);
        }

        Rval = SvcTask_Destroy(&pCtrl->RxTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_APPTEST, "Destroy Tx/Rx Task error", 0U, 0U);
        }

        Rval = AmbaWrap_memset(&DelCfg, 0, sizeof(SVC_CV_FLOW_ALGO_DELETE_CFG_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_APPTEST, "memset DelCfg failed", 0U, 0U);
        }

        for (i = 0; i < pCtrl->NumValidAlgo; i++) {
            Rval |= SvcCvFlow_DeleteAlgo(SVC_LOG_APPTEST, &DelCfg, &pCtrl->AlgoOp[i]);
        }
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_APPTEST, "Delele Algo error", 0U, 0U);
        }

        Rval = AmbaKAL_MsgQueueDelete(&pCtrl->AlgoOutQueId);
        Rval |= AmbaKAL_SemaphoreDelete(&pCtrl->OutBufSem);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_APPTEST, "Delete MsgQue/Semaphore error(0x%X)", Rval, 0U);
        }

        /* Reset Ctrl Intance */
        Rval = AmbaWrap_memset(&g_AppTestCtrl[Inst], 0, sizeof(SVC_APPTEST_CTRL_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_APPTEST, "memset g_AppTestCtrl failed", 0U, 0U);
        }
    }

    SvcLog_OK(SVC_LOG_APPTEST, "CvFlow_AppTestStop: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver debug interface
*/
static UINT32 CvFlow_AppTestDebug(UINT32 CvFlowChan, const char *pCmd, UINT32 Param0, UINT32 Param1, UINT32 Param2)
{
    UINT32 i;
    ULONG Addr;
    const SVC_APPTEST_CTRL_s *pCtrl;
    const AMBA_CV_FLEXIDAG_HANDLE_s *pCvFDHandle;

    if (0 == SvcWrap_strcmp("debug", pCmd)) {
        SvcCvDebugEnable = (UINT8) Param0;
        SvcLog_DBG(SVC_LOG_APPTEST, "Set debug(%u)", Param0, 0U);
    } else if (0 == SvcWrap_strcmp("info", pCmd)) {
        for (i = 0; i < SVC_APPTEST_MAX_INSTANCE; i++) {
            pCtrl = &g_AppTestCtrl[i];
            if (pCtrl->Used == 1U) {
                AmbaPrint_PrintUInt5("AppTest[%d]: CvfChan %d", i, pCtrl->CvfChan, 0U, 0U, 0U);
                pCvFDHandle = &pCtrl->AlgoOp[0].AlgoHandle.FDs[0].Handle;
                AmbaMisra_TypeCast(&Addr, &pCvFDHandle);
                SvcCvFlow_PrintULong("  CV_FLEXIDAG_HANDLE: 0x%x", Addr, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        SvcLog_DBG(SVC_LOG_APPTEST, "Unsupported cmd", 0U, 0U);
    }

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(&Param0);
    AmbaMisra_TouchUnused(&Param1);
    AmbaMisra_TouchUnused(&Param2);

    return SVC_OK;
}

SVC_CV_FLOW_OBJ_s SvcCvFlow_AppTestObj = {
    .Init         = CvFlow_AppTestInit,
    .Config       = CvFlow_AppTestConfig,
    .Load         = CvFlow_AppTestLoad,
    .Start        = CvFlow_AppTestStart,
    .Control      = CvFlow_AppTestControl,
    .Stop         = CvFlow_AppTestStop,
    .Debug        = CvFlow_AppTestDebug,
};

