/**
 *  @file SvcCvFlow_RefOD.c
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
#include "AmbaCache.h"

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
#include "SvcCvFlow_RefOD.h"
#include "SvcCvFlow_Rebel_RefOD.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcImgFIFO.h"
#include "SvcImgFeeder.h"
#include "SvcOsd.h"
#include "SvcWrap.h"
#include "SvcPlat.h"

#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_ref_util.h"
#include "cvapi_amba_od_interface.h"
#include "idsp_roi_msg.h"
#include "cvapi_svccvalgo_openod.h"
#include "SSD.h"

#ifdef CONFIG_AMBALINK_BOOT_OS
#include "cvapi_svccvalgo_linux.h"
#include "AmbaIPC_FlexidagIO.h"
#include "SvcCvFlexidagIO.h"
#endif

#define SVC_LOG_REFOD        "RefOD"

#define SVC_REFOD_INSTANCE0                (0U)
#define SVC_REFOD_INSTANCE1                (1U)
#define SVC_REFOD_INSTANCE2                (2U)
#define SVC_REFOD_INSTANCE3                (3U)
#define SVC_REFOD_MAX_INSTANCE             (4U)

#define SVC_REFOD_SEM_INIT_COUNT           (1U)
#define SVC_REFOD_OUTPUT_DEPTH             (SVC_REFOD_SEM_INIT_COUNT + 1U)

#define SVC_REFOD_RX_TASK_STACK_SIZE       (0x4000)
#define SVC_REFOD_PICINFO_QUEUE_SIZE       (8U)
#define SVC_REFOD_ALGO_OUT_QUEUE_SIZE      (SVC_REFOD_OUTPUT_DEPTH)

#define SRC_RATE_FRACTION_MODE_BIT          (0x10000000U)

typedef UINT32 (*SVC_REFOD_DATA_HANDLER_f)(UINT32 Inst);
typedef UINT32 (*SVC_REFOD_TX_HANDLER_f)(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

typedef struct {
    flexidag_memblk_t           OutputBuf[SVC_REFOD_OUTPUT_DEPTH][FLEXIDAG_MAX_OUTPUTS];
    AMBA_CV_FLEXIDAG_IO_s       InBuf[SVC_REFOD_OUTPUT_DEPTH];
    AMBA_CV_FLEXIDAG_IO_s       OutBuf[SVC_REFOD_OUTPUT_DEPTH];
} SVC_CV_FLOW_REFOD_OP_EX_INFO_s;

typedef struct {
    UINT8                       Used;
    UINT8                       CvfChan;
    UINT8                       Enable;
    UINT8                       Started;
    SVC_CV_FLOW_CFG_s           *pCvFlow;
    SVC_CV_FLOW_CFG_s           CvCfg;

    UINT8                       NumValidAlgo;
    SVC_CV_FLOW_ALGO_OP_INFO_s  AlgoOp[REFOD_MAX_ALGO_NUM];
    SVC_CV_FLOW_REFOD_OP_EX_INFO_s AlgoOpEx[REFOD_MAX_ALGO_NUM];
    void                       *pImgInfoBuf[SVC_REFOD_PICINFO_QUEUE_SIZE];
    UINT32                      ImgInfoBufSize;

    /* Data source service */
    SVC_IMG_FEEDER_PORT_s       FeederPort;
    UINT32                      SrcSeqNum;
    SVC_REFOD_TX_HANDLER_f      pTxProc;
    UINT32                      TxSeqNum;
    UINT32                      PicInfoIdx;

    /* Data sink service */
    SVC_TASK_CTRL_s             RxTaskCtrl;
    SVC_REFOD_DATA_HANDLER_f    pRxProc;
    UINT32                      RxSeqNum;
    UINT32                      OutBufIdx;
    AMBA_KAL_SEMAPHORE_t        OutBufSem;
    UINT32                      SinkInfoIdx;

    /* Algo Callback */
    AMBA_KAL_MSG_QUEUE_t        AlgoOutQueId;


    /* Misc */
    UINT32                      SrcRateCtrl;
#define                        REFOD_SRC_RATE_MAX_DENOMINATOR  (16U)
    UINT8                       SrcValidTbl[REFOD_SRC_RATE_MAX_DENOMINATOR];
    UINT8                       SrcRateNum;
    UINT8                       SrcRateDen;
    UINT8                       SrcInfoErr;
    UINT8                       SceneMode;
    UINT32                      FlexiIOChan;    /* for AmbaLink CV only */
} SVC_REFOD_CTRL_s;

typedef struct {
   UINT32 Event;
   SVC_CV_ALGO_OUTPUT_s AlgoOut;
} REFOD_OUTPUT_MSG_s;

typedef struct {
    const char **pDPtr;
} SVC_CHAR_PTR_s;


static SVC_REFOD_CTRL_s g_RefODCtrl[SVC_REFOD_MAX_INSTANCE] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MUTEX_t ODMutex;
static UINT8 SvcCvDebugEnable = 0;

static UINT32 CvFlow_RefODInit(UINT32 CvFlowChan, void *pInit);
static UINT32 CvFlow_RefODConfig(UINT32 CvFlowChan, void *pConfig);
static UINT32 CvFlow_RefODStart(UINT32 CvFlowChan, void *pInput);
static UINT32 CvFlow_RefODControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo);
static UINT32 CvFlow_RefODStop(UINT32 CvFlowChan, void *pInput);
static UINT32 RefOD_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst);

static UINT32 RefOD_TxDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 RefOD_TxDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 RefOD_TxDataSrcHandler2(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 RefOD_TxDataSrcHandler3(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

static UINT32 RefOD_AlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 RefOD_AlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 RefOD_AlgoOutCallback2(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 RefOD_AlgoOutCallback3(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);

static UINT32 RefOD_ConfigRoi(UINT32 Inst, UINT32 AlgoIdx, const SVC_CV_ROI_INFO_s *pRoiCfg);
static UINT32 RefOD_ConfigNms(UINT32 Inst, UINT32 AlgoIdx, const SVC_CV_FLOW_REFOD_NMS_CFG_s *pNmsCfg);
static UINT32 RefOD_ConfigLicense(UINT32 Inst, UINT32 AlgoIdx);
static UINT32 RefOD_ConfigExtBin(UINT32 Inst, UINT32 AlgoIdx);

static void SVC_CV_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcCvDebugEnable > 0U) {
        SvcLog_DBG(pModule, pFormat, Arg1, Arg2);
    }
}

static void RefOD_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&ODMutex, 5000)) {
        SvcLog_NG(SVC_LOG_REFOD, "RefOD_MutexTake: timeout", 0U, 0U);
    }
}

static void RefOD_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&ODMutex)) {
        SvcLog_NG(SVC_LOG_REFOD, "RefOD_MutexGive: error", 0U, 0U);
    }
}

static UINT32 RefOD_SemTake(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreTake(&g_RefODCtrl[Inst].OutBufSem, 10000)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_REFOD, "RefOD_SemTake[%d]: timeout", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 RefOD_SemGive(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreGive(&g_RefODCtrl[Inst].OutBufSem)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_REFOD, "RefOD_SemGive[%d]: error", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 RefOD_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst)
{
    UINT32 Rval = SVC_NG;
    UINT32 i;

    if (NULL != pInst) {
        for (i = 0; i < SVC_REFOD_MAX_INSTANCE; i++) {
            if ((1U == g_RefODCtrl[i].Used) && (CvfChan == g_RefODCtrl[i].CvfChan)) {
                *pInst = i;
                Rval = SVC_OK;
                break;
            }
        }
    }
    return Rval;
}

#ifdef CONFIG_AMBALINK_BOOT_OS
static UINT32 RefOD_Init_Callback(UINT32 Channel, UINT32 OutType)
{
    UINT32 Rval = SVC_OK;

    (void) Channel;
    (void) OutType;
    AmbaPrint_PrintStr5("RefOD_Init_Callback", NULL, NULL, NULL, NULL, NULL);

    return Rval;
}
#endif

static void RefOD_DumpPicInfo(const cv_pic_info_t *pPicInfo)
{
    static UINT32 DumpTimes = 0U;
    extern void SvcCvFlow_DumpPicInfo(const char *pModuleName, const cv_pic_info_t *pPicInfo);

    if (DumpTimes > 0U) {
        SvcCvFlow_DumpPicInfo(SVC_LOG_REFOD, pPicInfo);
        DumpTimes--;
    }
}

/**
* Pack data source to pic info structure
*/
static void RefOD_PackPicInfo(UINT32 Inst, const SVC_CV_INPUT_IMG_INFO_s *pInputImg, memio_source_recv_picinfo_t *pInfo)
{
    UINT32 Rval = SVC_OK;
    cv_pic_info_t *pPicInfo = &pInfo->pic_info;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    const SVC_REFOD_CTRL_s *pCtrl = &g_RefODCtrl[Inst];
    UINT32 DataSrc, RelativeAddrOn = 1U;
    extern UINT32 SvcCvFlow_PackPicInfo_Pyramid(cv_pic_info_t *pPicInfo, const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo, UINT32 MainWidth, UINT32 MainHeight, UINT32 UseRelative, UINT32 LeftRight);
    extern UINT32 SvcCvFlow_PackPicInfo_MainYuv(cv_pic_info_t *pPicInfo, const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo, UINT32 UseRelative, UINT32 LeftRight);
    extern UINT32 SvcCvFlow_PackPicInfo_Lndt(cv_pic_info_t *pPicInfo, const AMBA_DSP_LNDT_DATA_RDY_s *pYuvInfo, UINT32 MainWidth, UINT32 MainHeight, UINT32 UseRelative, UINT32 LeftRight);

    Rval = AmbaWrap_memset(pInfo, 0, sizeof(memio_source_recv_picinfo_t));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_REFOD, "memset picinfo failed", 0U, 0U);
    }

    DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
    if (pInputImg->NumInfo > 0U) {
        if (SvcCvFlow_RefOD_ModeInfo[pCtrl->pCvFlow->CvModeID].ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
            RelativeAddrOn = 0U;
        }

        if ((DataSrc == SVC_CV_DATA_SRC_MAIN_YUV) ||
            (DataSrc == SVC_CV_DATA_SRC_YUV_STRM)) {
            const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo;
            AmbaMisra_TypeCast(&pYuvInfo, &pInputImg->Info[0].pBase);

            Rval = SvcCvFlow_PackPicInfo_MainYuv(pPicInfo, pYuvInfo, RelativeAddrOn, 0U);
        } else if (DataSrc == SVC_CV_DATA_SRC_PYRAMID_EX_SCALE) {
            const AMBA_DSP_LNDT_DATA_RDY_s *pYuvInfo;
            AmbaMisra_TypeCast(&pYuvInfo, &pInputImg->Info[0].pBase);

            Rval = SvcCvFlow_PackPicInfo_Lndt(pPicInfo,
                                              pYuvInfo,
                                              pCfg->FovCfg[pYuvInfo->ViewZoneId].MainWin.Width,
                                              pCfg->FovCfg[pYuvInfo->ViewZoneId].MainWin.Height,
                                              RelativeAddrOn, 0U);
        } else {
            const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo;
            AmbaMisra_TypeCast(&pYuvInfo, &pInputImg->Info[0].pBase);

            Rval = SvcCvFlow_PackPicInfo_Pyramid(pPicInfo,
                                                 pYuvInfo,
                                                 pCfg->FovCfg[pYuvInfo->ViewZoneId].MainWin.Width,
                                                 pCfg->FovCfg[pYuvInfo->ViewZoneId].MainWin.Height,
                                                 RelativeAddrOn, 0U);
        }
    } else {
        Rval = SVC_NG;
    }

    RefOD_DumpPicInfo(pPicInfo);

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_REFOD, "RefOD_PackPicInfo error", 0U, 0U);
    }
}

/**
* Send message to Feeder to update port status
*/
static void RefOD_SendReceiverReady(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort)
{
    SVC_REFOD_CTRL_s *pCtrl = &g_RefODCtrl[Inst];
    AMBA_KAL_SEMAPHORE_INFO_s SemInfo;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreQuery(&pCtrl->OutBufSem, &SemInfo)) {
        if (SemInfo.CurrentCount > 0U) {
            if (SVC_OK != SvcImgFeeder_MsgToFeeder(pPort, SVC_IMG_FEEDER_RECEIVER_READY, NULL)) {
                SvcLog_NG(SVC_LOG_REFOD, "Send ReceiverReady[%u] error", Inst, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_REFOD, "ReceiverReady[%u]: SemQuery error", Inst, 0U);
    }
}

/**
* TX data source handler: process input data
*/
static UINT32 RefOD_TxDataSrcHandler(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_REFOD_CTRL_s *pCtrl = &g_RefODCtrl[Inst];
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
    const SVC_CV_INPUT_IMG_INFO_s *pInputImg;
    memio_source_recv_picinfo_t *pPicInfo;
    UINT32 i, j;
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;
    AMBA_CV_FLEXIDAG_IO_s *pInBuf, *pOutBuf;
    const SVC_CV_FLOW_REFOD_MODE_INFO_s *pModeInfo;
#ifdef CONFIG_AMBALINK_BOOT_OS
    SVC_CV_ALGO_LINUX_EX_CFG_s ExCfg;
#endif

    AmbaMisra_TouchUnused(pInfo);
    pModeInfo = &SvcCvFlow_RefOD_ModeInfo[pCtrl->pCvFlow->CvModeID];

    SVC_CV_DBG(SVC_LOG_REFOD, "Tx[%u]: run %d", Inst, pCtrl->TxSeqNum);

    if (Event == SVC_IMG_FEEDER_IMAGE_DATA_READY) {
        if ((pCtrl->Enable == 1U) &&
            (0U == (pCtrl->SrcSeqNum % pCtrl->SrcRateCtrl))) {
            Rval = RefOD_SemTake(Inst);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pInputImg, &pInfo);
                AmbaMisra_TypeCast(&pPicInfo, &pCtrl->pImgInfoBuf[pCtrl->PicInfoIdx]);
                RefOD_PackPicInfo(Inst, pInputImg, pPicInfo);

                /* Prepare feed config */
                Rval = AmbaWrap_memset(&FeedCfg, 0, sizeof(SVC_CV_ALGO_FEED_CFG_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_REFOD, "memset FeedCfg failed", 0U, 0U);
                }

                for (i = 0; i < pCtrl->NumValidAlgo; i++) {
                    pAlgoOp = &pCtrl->AlgoOp[i];
                    pInBuf = &pCtrl->AlgoOpEx[i].InBuf[pCtrl->OutBufIdx];
                    pOutBuf = &pCtrl->AlgoOpEx[i].OutBuf[pCtrl->OutBufIdx];

                    FeedCfg.pIn  = pInBuf;
                    FeedCfg.pOut = pOutBuf;
                    pInBuf->num_of_buf = 1U;
                    SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_REFOD, pPicInfo, pCtrl->ImgInfoBufSize, &pInBuf->buf[0]);
                    pOutBuf->num_of_buf = pAlgoOp->OutputNum;
                    for (j = 0U; j < pAlgoOp->OutputNum; j++) {
                        Rval = AmbaWrap_memcpy(&pOutBuf->buf[j], &pCtrl->AlgoOpEx[i].OutputBuf[pCtrl->OutBufIdx][j], sizeof(flexidag_memblk_t));
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_REFOD, "memcpy OutputBufBlk failed", 0U, 0U);
                        }
                    }

                    if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
#ifdef CONFIG_AMBALINK_BOOT_OS
                        const SVC_CV_ALGO_LINUX_EX_CFG_s *pExCfg = &ExCfg;

                        ExCfg.AmbaIPCChannel = pCtrl->FlexiIOChan;
                        ExCfg.ValidDataSize  = sizeof(memio_source_recv_picinfo_t);
                        AmbaMisra_TypeCast(&FeedCfg.pExtFeedCfg, &pExCfg);
#else
                        SvcLog_NG(SVC_LOG_REFOD, "CONFIG_AMBALINK_BOOT_OS not defined ", 0U, 0U);
#endif
                    } else {
                        AmbaMisra_TypeCast(&FeedCfg.pExtFeedCfg, &pPicInfo); /* SCA_OpenOD */
                        AmbaMisra_TypeCast(&FeedCfg.pUserData, &pPicInfo);
                    }

                    Rval |= SvcCvAlgo_Feed(&pAlgoOp->AlgoHandle, &FeedCfg);
                    SVC_CV_DBG(SVC_LOG_REFOD, "Tx[%u]: Send(%u)", Inst, pPicInfo->pic_info.frame_num);
                }

                if (SVC_OK == Rval) {
                    pCtrl->TxSeqNum++;
                    pCtrl->PicInfoIdx++;
                    if (pCtrl->PicInfoIdx >= SVC_REFOD_PICINFO_QUEUE_SIZE) {
                        pCtrl->PicInfoIdx = 0;
                    }
                    pCtrl->OutBufIdx++;
                    if (pCtrl->OutBufIdx >= SVC_REFOD_OUTPUT_DEPTH) {
                        pCtrl->OutBufIdx = 0;
                    }
                } else {
                    SvcLog_NG(SVC_LOG_REFOD, "Tx[%u]: send error", Inst, 0U);
                }

                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    RefOD_SendReceiverReady(Inst, pPort);
                }
            } else {
                SvcLog_NG(SVC_LOG_REFOD, "Tx[%u]: SemTake error", Inst, 0U);
            }
        } else {
            if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                RefOD_SendReceiverReady(Inst, pPort);
                Rval = AmbaKAL_TaskSleep(1);
            }
        }

        pCtrl->SrcSeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_REFOD, "Tx[%u]: unsupported event(0x%x)", Inst, Event);
        Rval = SVC_NG;
    }

    SVC_CV_DBG(SVC_LOG_REFOD, "Tx[%u]: end", Inst, 0U);

    return Rval;
}

static UINT32 RefOD_TxDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_REFOD_CTRL_s *pCtrl = &g_RefODCtrl[SVC_REFOD_INSTANCE0];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_REFOD_INSTANCE0, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 RefOD_TxDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_REFOD_CTRL_s *pCtrl = &g_RefODCtrl[SVC_REFOD_INSTANCE1];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_REFOD_INSTANCE1, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 RefOD_TxDataSrcHandler2(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_REFOD_CTRL_s *pCtrl = &g_RefODCtrl[SVC_REFOD_INSTANCE2];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_REFOD_INSTANCE2, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 RefOD_TxDataSrcHandler3(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_REFOD_CTRL_s *pCtrl = &g_RefODCtrl[SVC_REFOD_INSTANCE3];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_REFOD_INSTANCE3, pPort, Event, pInfo);
    }
    return Rval;
}

#define NUM_CLS             7U
/**
* Transfer Linux SSD detection result to SVC structure
*/
static void RefOD_RefODTransSSD(const SVC_REFOD_CTRL_s *pCtrl, const memio_source_recv_picinfo_t *pInPicInfo, const CVALGO_SSD_OUTPUT_s *pCvAlgoSsdOutput, SVC_CV_DETRES_BBX_LIST_s *pBbxList)
{
    UINT32 Rval;
    UINT32 i = 0;
    UINT32 ObjNum;
    ULONG  TmpAddr;
    static char g_cls_name_array[NUM_CLS][16];
    static UINT32 g_pcls_name[NUM_CLS];
    char *g_cls_name = &g_cls_name_array[0][0];
    const UINT32 *ptr = g_pcls_name;
    ULONG clsaddr;
    const char cls_name[NUM_CLS][16] = {
        "background",
        "bicycle",
        "bus",
        "car",
        "motorbike",
        "person",
        "train"
    };
    SVC_CHAR_PTR_s CharPtr;

    AmbaMisra_TouchUnused(&pCtrl);
    AmbaMisra_TouchUnused(&pInPicInfo);


    ObjNum = pCvAlgoSsdOutput->BoxNum;

    //AmbaPrint_PrintUInt5("[RefOD_RefODTrans] amba OD ========= total:%3d =============", ObjNum, 0U, 0U, 0U, 0U);
    if (ObjNum <= MAX_CVALGO_SSD_BOX_NUM) {
        pBbxList->CaptureTime = 0;//pInPicInfo->pic_info.capture_time;
        pBbxList->FrameNum    = 0;//pInPicInfo->pic_info.frame_num;
        pBbxList->BbxAmount   = ObjNum;
        pBbxList->Source      = 0;
        for (i = 0U ; i < ObjNum; i++) {
            pBbxList->Bbx[i].Cat = (UINT16)pCvAlgoSsdOutput->Box[i].Class;
            pBbxList->Bbx[i].Fid = 0U;
            pBbxList->Bbx[i].H = (UINT16)pCvAlgoSsdOutput->Box[i].H;
            pBbxList->Bbx[i].ObjectId = 0U;
            pBbxList->Bbx[i].Score = 0U;
            pBbxList->Bbx[i].W = (UINT16)pCvAlgoSsdOutput->Box[i].W;
            pBbxList->Bbx[i].X = (UINT16)pCvAlgoSsdOutput->Box[i].X;
            pBbxList->Bbx[i].Y = (UINT16)pCvAlgoSsdOutput->Box[i].Y;

#if 0
            AmbaPrint_PrintUInt5("[RefOD_RefODTrans] cat:%d x:%4d y:%4d w:%3d h:%3d",
                    pCvAlgoSsdOutput->Box[i].Class,
                    pCvAlgoSsdOutput->Box[i].X,
                    pCvAlgoSsdOutput->Box[i].Y,
                    pCvAlgoSsdOutput->Box[i].W,
                    pCvAlgoSsdOutput->Box[i].H);
#endif
        }
        AmbaMisra_TypeCast(&clsaddr, &ptr);
        Rval = AmbaWrap_memcpy(g_cls_name, cls_name, sizeof(cls_name));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_REFOD, "memcpy cls_name failed", 0U, 0U);
        }
        for (i = 0; i < NUM_CLS; i++) {
            AmbaMisra_TypeCast(&TmpAddr, &g_cls_name);
            g_pcls_name[i] = TmpAddr + ((UINT32)i * (UINT32)sizeof(cls_name[0]));
        }

        if (SVC_OK == AmbaWrap_memcpy(&CharPtr, &clsaddr, (UINT32)sizeof(void *))) {
            pBbxList->class_name = CharPtr.pDPtr;
        } else {
            SvcLog_NG(SVC_LOG_REFOD, "typecast class_name failed", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_REFOD, "RefOD_RefODTrans: wrong ObjNum = %u", ObjNum, 0U);
    }
}

/**
* Transfer detection result to SVC structure
*/
static void RefOD_RefODTransRebel(const SVC_REFOD_CTRL_s *pCtrl, const memio_source_recv_picinfo_t *pInPicInfo,
                                  const amba_od_out_t *pOdResult, SVC_CV_DETRES_BBX_LIST_REBEL_s *pBbxListRebel)
{
    UINT32 i = 0;
    UINT32 ObjNum;
    ULONG  ObjAddr;
    const amba_od_candidate_rebel_t   *pObjBase, *pObj;
    SVC_CHAR_PTR_s CharPtr;
    UINT32 RetVal;

    AmbaMisra_TouchUnused(&pCtrl);
    AmbaMisra_TouchUnused(&pInPicInfo);

    AmbaMisra_TypeCast(&ObjAddr, &pOdResult);
    ObjAddr = ObjAddr + pOdResult->objects_offset;
    AmbaMisra_TypeCast(&pObjBase, &ObjAddr);
    ObjNum = pOdResult->num_objects;

//    AmbaPrint_PrintUInt5("[RefOD_RefODTransRebel] amba OD ========= total:%3d =============", ObjNum, 0U, 0U, 0U, 0U);
    if (ObjNum <= SVC_CV_DETRES_MAX_BBX_NUM) {
        if (NULL != pInPicInfo) {
            pBbxListRebel->CaptureTime = pInPicInfo->pic_info.capture_time;
            pBbxListRebel->FrameNum    = pInPicInfo->pic_info.frame_num;
        } else {
            pBbxListRebel->CaptureTime = pOdResult->capture_time;
            pBbxListRebel->FrameNum    = pOdResult->frame_num;
        }
        pBbxListRebel->BbxAmount   = ObjNum;
        pBbxListRebel->Source      = 0;
        for (i = 0U ; i < ObjNum; i++) {
            pObj = &pObjBase[i];
            pBbxListRebel->Bbx[i].Cat = (UINT16)pObj->class;
            pBbxListRebel->Bbx[i].Fid = (UINT16)pObj->field;
            pBbxListRebel->Bbx[i].H = (UINT16)pObj->bb_height_m1 + 1U;
            pBbxListRebel->Bbx[i].ObjectId = pObj->track;
            pBbxListRebel->Bbx[i].Score = pObj->score;
            pBbxListRebel->Bbx[i].W = (UINT16)pObj->bb_width_m1 + 1U;
            pBbxListRebel->Bbx[i].X = (UINT16)pObj->bb_start_col;
            pBbxListRebel->Bbx[i].Y = (UINT16)pObj->bb_start_row;
            pBbxListRebel->Bbx[i].ObjectID         = pObj->ObjectID;
            pBbxListRebel->Bbx[i].Confidence       = pObj->Confidence;
            pBbxListRebel->Bbx[i].PositionX        = pObj->PositionX;
            pBbxListRebel->Bbx[i].PositionY        = pObj->PositionY;
            pBbxListRebel->Bbx[i].VelocityX        = pObj->VelocityX;
            pBbxListRebel->Bbx[i].VelocityY        = pObj->VelocityY;
            pBbxListRebel->Bbx[i].DirectionStatus  = pObj->DirectionStatus;
            pBbxListRebel->Bbx[i].MotionStatus     = pObj->MotionStatus;
            pBbxListRebel->Bbx[i].Length           = pObj->Length;
            pBbxListRebel->Bbx[i].Width            = pObj->Width;
            pBbxListRebel->Bbx[i].TurnSignal       = pObj->TurnSignal;
            pBbxListRebel->Bbx[i].BrakeLamp        = pObj->BrakeLamp;
            pBbxListRebel->Bbx[i].Classification   = pObj->Classification;
            pBbxListRebel->Bbx[i].LaneAssignment   = pObj->LaneAssignment;
            AmbaSvcWrap_MisraMemcpy(&pBbxListRebel->Bbx[i].NearestPoint, &pObj->NearestPoint, sizeof(SVC_POINT_2D_s));
            AmbaSvcWrap_MisraMemcpy(&pBbxListRebel->Bbx[i].DepthPointX, &pObj->DepthPointX, sizeof(SVC_POINT_2D_s));
            AmbaSvcWrap_MisraMemcpy(&pBbxListRebel->Bbx[i].DepthPointY, &pObj->DepthPointY, sizeof(SVC_POINT_2D_s));

#if 0
            AmbaPrint_PrintInt5("[RefOD_RefODTransRebel] cat:%d x:%4d y:%4d w:%3d h:%3d",
                    (INT32)pObj->class,
                    pObj->bb_start_col,
                    pObj->bb_start_row,
                    (INT32)pObj->bb_width_m1,
                    (INT32)pObj->bb_height_m1);

            AmbaPrint_PrintInt5("[RefOD_RefODTransRebel] ObjectID:%d PositionX:%d PositionY:%d VelocityX:%d VelocityY:%d",
                    pObj->ObjectID,
                    pObj->PositionX,
                    pObj->PositionY,
                    (INT32)pObj->VelocityX,
                    (INT32)pObj->VelocityY);

            AmbaPrint_PrintInt5("[RefOD_RefODTransRebel] NearestPoint_X:%d NearestPoint_Y:%d "
                    "DepthPointX_X:%d DepthPointX_Y:%d DepthPointY_X:%d",
                    pObj->NearestPoint.x,
                    pObj->NearestPoint.y,
                    pObj->DepthPointX.x,
                    pObj->DepthPointX.y,
                    pObj->DepthPointY.x);
#endif
        }

        if (pOdResult->class_name_p64 != 0U) {
            RetVal = AmbaWrap_memcpy(&CharPtr, &pOdResult->class_name_p64, (UINT32)sizeof(void *));
        } else {
            const void *pVoidTmp;
            ULONG VirtAddr;
            RetVal = AmbaMMU_PhysToVirt((ULONG)pOdResult->class_name, &VirtAddr);
            AmbaMisra_TypeCast(&pVoidTmp, &VirtAddr);
            RetVal = AmbaWrap_memcpy(&CharPtr, &pVoidTmp, (UINT32)sizeof(void *));
        }

        if (RetVal == SVC_OK) {
            pBbxListRebel->class_name = CharPtr.pDPtr;
        } else {
            SvcLog_NG(SVC_LOG_REFOD, "typecast class_name failed", 0U, 0U);
        }
    } else {
        //SvcLog_NG(SVC_LOG_REFOD, "RefOD_RefODTransRebel: wrong ObjNum = %u", ObjNum, 0U);
    }
}

/**
* Transfer detection result to SVC structure
*/
static void RefOD_RefODTrans(const SVC_REFOD_CTRL_s *pCtrl, const memio_source_recv_picinfo_t *pInPicInfo, const amba_od_out_t *pOdResult, SVC_CV_DETRES_BBX_LIST_s *pBbxList)
{
    UINT32 i = 0;
    UINT32 ObjNum;
    ULONG  ObjAddr;
    const amba_od_candidate_t   *pObjBase, *pObj;
    SVC_CHAR_PTR_s CharPtr;
    UINT32 RetVal;

    AmbaMisra_TouchUnused(&pCtrl);
    AmbaMisra_TouchUnused(&pInPicInfo);

    AmbaMisra_TypeCast(&ObjAddr, &pOdResult);
    ObjAddr = ObjAddr + pOdResult->objects_offset;
    AmbaMisra_TypeCast(&pObjBase, &ObjAddr);
    ObjNum = pOdResult->num_objects;

    //AmbaPrint_PrintUInt5("[RefOD_RefODTrans] amba OD ========= total:%3d =============", ObjNum, 0U, 0U, 0U, 0U);
    if (ObjNum <= SVC_CV_DETRES_MAX_BBX_NUM) {
        if (NULL != pInPicInfo) {
            pBbxList->CaptureTime = pInPicInfo->pic_info.capture_time;
            pBbxList->FrameNum    = pInPicInfo->pic_info.frame_num;
        } else {
            pBbxList->CaptureTime = 0U;
            pBbxList->FrameNum    = 0U;
        }
        pBbxList->BbxAmount   = ObjNum;
        pBbxList->Source      = 0;
        for (i = 0U ; i < ObjNum; i++) {
            pObj = &pObjBase[i];
            pBbxList->Bbx[i].Cat = (UINT16)pObj->class;
            pBbxList->Bbx[i].Fid = (UINT16)pObj->field;
            pBbxList->Bbx[i].H = (UINT16)pObj->bb_height_m1 + 1U;
            pBbxList->Bbx[i].ObjectId = pObj->track;
            pBbxList->Bbx[i].Score = pObj->score;
            pBbxList->Bbx[i].W = (UINT16)pObj->bb_width_m1 + 1U;
            pBbxList->Bbx[i].X = (UINT16)pObj->bb_start_col;
            pBbxList->Bbx[i].Y = (UINT16)pObj->bb_start_row;

#if 0
            AmbaPrint_PrintInt5("[RefOD_RefODTrans] cat:%d x:%4d y:%4d w:%3d h:%3d",
                    (INT32)pObj->class,
                    pObj->bb_start_col,
                    pObj->bb_start_row,
                    (INT32)pObj->bb_width_m1,
                    (INT32)pObj->bb_height_m1);
#endif
        }

        if (pOdResult->class_name_p64 != 0U) {
            RetVal = AmbaWrap_memcpy(&CharPtr, &pOdResult->class_name_p64, (UINT32)sizeof(void *));
        } else {
            const void *pVoidTmp;
            ULONG VirtAddr;
            RetVal = AmbaMMU_PhysToVirt((ULONG)pOdResult->class_name, &VirtAddr);
            AmbaMisra_TypeCast(&pVoidTmp, &VirtAddr);
            RetVal = AmbaWrap_memcpy(&CharPtr, &pVoidTmp, (UINT32)sizeof(void *));
        }

        if (RetVal == SVC_OK) {
            pBbxList->class_name = CharPtr.pDPtr;
        } else {
            SvcLog_NG(SVC_LOG_REFOD, "typecast class_name failed", 0U, 0U);
        }
    } else {
        //SvcLog_NG(SVC_LOG_REFOD, "RefOD_RefODTrans: wrong ObjNum = %u", ObjNum, 0U);
    }
}

/**
* Attach class name string to bounding box structure
*/
static void RefOD_AttachClassName(const SVC_REFOD_CTRL_s *pCtrl, SVC_CV_DETRES_BBX_LIST_s *pBbxList)
{
    AmbaMisra_TouchUnused(&pCtrl);
    AmbaMisra_TouchUnused(pBbxList);
}

static void RefOD_AttachClassNameRebel(const SVC_REFOD_CTRL_s *pCtrl, SVC_CV_DETRES_BBX_LIST_REBEL_s *pBbxListRebel)
{
    AmbaMisra_TouchUnused(&pCtrl);
    AmbaMisra_TouchUnused(pBbxListRebel);
}

/**
* Algorithm output event callback function
*/
static UINT32 RefOD_AlgoOutCallback(UINT32 Inst, UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Rval = SVC_OK;
    SVC_REFOD_CTRL_s *pCtrl = &g_RefODCtrl[Inst];
    REFOD_OUTPUT_MSG_s Msg;
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
                SvcLog_NG(SVC_LOG_REFOD, "memcpy AlgoOut failed", 0U, 0U);
            }
        }
        Rval = AmbaKAL_MsgQueueSend(&pCtrl->AlgoOutQueId, &Msg, 5000);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_REFOD, "RefOD_AlgoOutCallback[%u]: MsgQueueSend error", Inst, 0U);
        }
    }

    return Rval;
}

static UINT32 RefOD_AlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return RefOD_AlgoOutCallback(SVC_REFOD_INSTANCE0, Event, pEventData);
}

static UINT32 RefOD_AlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return RefOD_AlgoOutCallback(SVC_REFOD_INSTANCE1, Event, pEventData);
}

static UINT32 RefOD_AlgoOutCallback2(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return RefOD_AlgoOutCallback(SVC_REFOD_INSTANCE2, Event, pEventData);
}

static UINT32 RefOD_AlgoOutCallback3(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return RefOD_AlgoOutCallback(SVC_REFOD_INSTANCE3, Event, pEventData);
}

/**
* RX data sink handler: process detection output
*/
static UINT32 RefOD_RxDataSinkHandler(UINT32 Inst)
{
    UINT32 Rval;
    SVC_REFOD_CTRL_s *pCtrl = &g_RefODCtrl[Inst];
    const memio_source_recv_picinfo_t *pInPicInfo = NULL;
    REFOD_OUTPUT_MSG_s Msg;

    const SVC_CV_ALGO_OUTPUT_s *pAlgoOut = &Msg.AlgoOut;
    const amba_od_out_t *pAmbaOdOut;
    const CVALGO_SSD_OUTPUT_s *pSSD_Out;

    SVC_CV_DETRES_BBX_LIST_s *pBbxList;
    SVC_CV_DETRES_BBX_LIST_REBEL_s *pBbxListRebel;
    SVC_CV_OUT_BUF_s *pOutBuf;
    static SVC_CV_DETRES_BBX_LIST_s Bbxlist[SVC_REFOD_MAX_INSTANCE][SVC_REFOD_OUTPUT_DEPTH] GNU_SECTION_NOZEROINIT;
    static SVC_CV_DETRES_BBX_LIST_REBEL_s BbxlistRebel[SVC_REFOD_MAX_INSTANCE][SVC_REFOD_OUTPUT_DEPTH] GNU_SECTION_NOZEROINIT;
    static SVC_CV_OUT_BUF_s OutBuf[SVC_REFOD_MAX_INSTANCE][SVC_REFOD_OUTPUT_DEPTH] GNU_SECTION_NOZEROINIT;
    const SVC_CV_FLOW_REFOD_MODE_INFO_s *pModeInfo = &SvcCvFlow_RefOD_ModeInfo[pCtrl->pCvFlow->CvModeID];
    UINT8 OutputType = pModeInfo->OutputType;
    UINT8 OutputBufIdx = pModeInfo->OutputBufIdx;
    ULONG BufferAddr;

    SVC_CV_DBG(SVC_LOG_REFOD, "Rx[%u]: run %d", Inst, pCtrl->RxSeqNum);

    Rval = AmbaKAL_MsgQueueReceive(&pCtrl->AlgoOutQueId, &Msg, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK == Rval) {
        SVC_CV_DBG(SVC_LOG_REFOD, "Rx[%u]: recv out(%u)", Inst, 0);

        if (Msg.Event == CALLBACK_EVENT_OUTPUT) {
            if (pCtrl->Enable == 1U) {
                if (OutputType == REFOD_OUTPUT_TYPE_DETBOX) {
                    AmbaMisra_TypeCast(&pAmbaOdOut, &pAlgoOut->pOutput->buf[OutputBufIdx].pBuffer);
#ifndef CONFIG_AMBALINK_BOOT_OS
                    if (NULL != pAlgoOut->pUserData) {
                        AmbaMisra_TypeCast(&pInPicInfo, &pAlgoOut->pUserData);
                    }
#endif
                    pBbxList = &Bbxlist[Inst][pCtrl->SinkInfoIdx];
                    RefOD_RefODTrans(pCtrl, pInPicInfo, pAmbaOdOut, pBbxList);
                    RefOD_AttachClassName(pCtrl, pBbxList);

                    pBbxList->MsgCode = SVC_CV_DETRES_BBX;
                    pBbxList->Source  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
                    Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                           SVC_CV_FLOW_OUTPUT_PORT0,
                                           pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                           pBbxList);

#if defined(CONFIG_ICAM_PROJECT_SHMOO)
                    /* Advanced Shmoo Task */
                    {
                        UINT32 i;
                        SVC_CV_DATA_OUTPUT_s RawCvData;
                        RawCvData.NumData = (UINT8) pAlgoOut->pOutput->num_of_buf;
                        for(i=0U; i<RawCvData.NumData; i++)
                        {
                            AmbaMisra_TypeCast(&RawCvData.Data[i].pBuf, &pAlgoOut->pOutput->buf[i].pBuffer);
                            RawCvData.Data[i].BufSize = pAlgoOut->pOutput->buf[i].buffer_size;

                        }
                        RawCvData.Msg = SVC_CV_RAW_OUTPUT;
                        Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                SVC_CV_FLOW_OUTPUT_PORT1,
                                pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT1],
                                &RawCvData);
                    }
#endif

                } else if (OutputType == REBEL_OUTPUT_TYPE_DETBOX) {
                    AmbaMisra_TypeCast(&pAmbaOdOut, &pAlgoOut->pOutput->buf[OutputBufIdx].pBuffer);
#ifndef CONFIG_AMBALINK_BOOT_OS
                    if (NULL != pAlgoOut->pUserData) {
                        AmbaMisra_TypeCast(&pInPicInfo, &pAlgoOut->pUserData);
                    }
#endif
                    pBbxListRebel = &BbxlistRebel[Inst][pCtrl->SinkInfoIdx];
                    RefOD_RefODTransRebel(pCtrl, pInPicInfo, pAmbaOdOut, pBbxListRebel);
                    RefOD_AttachClassNameRebel(pCtrl, pBbxListRebel);

                    pBbxListRebel->MsgCode = SVC_CV_DETRES_BBX_REBEL;
                    pBbxListRebel->Source  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
                    Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                           SVC_CV_FLOW_OUTPUT_PORT0,
                                           pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                           pBbxListRebel);
                } else if (OutputType == REFOD_OUTPUT_TYPE_OSDBUF) {
                    AmbaMisra_TypeCast(&BufferAddr, &pAlgoOut->pOutput->buf[OutputBufIdx].pBuffer);
#ifndef CONFIG_AMBALINK_BOOT_OS
                    if (NULL != pAlgoOut->pUserData) {
                        AmbaMisra_TypeCast(&pInPicInfo, &pAlgoOut->pUserData);
                    }
#endif
                    pOutBuf = &OutBuf[Inst][pCtrl->SinkInfoIdx];

                    pOutBuf->MsgCode = SVC_CV_OSD_BUFFER;
                    pOutBuf->Source  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
                    pOutBuf->Addr    = BufferAddr;
                    Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                           SVC_CV_FLOW_OUTPUT_PORT0,
                                           pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                           pOutBuf);
                } else if (OutputType == REFOD_OUTPUT_TYPE_SSD_DETBOX) {
                    AmbaMisra_TypeCast(&pSSD_Out, &pAlgoOut->pOutput->buf[OutputBufIdx].pBuffer);
#ifndef CONFIG_AMBALINK_BOOT_OS
                    if (NULL != pAlgoOut->pUserData) {
                        AmbaMisra_TypeCast(&pInPicInfo, &pAlgoOut->pUserData);
                    }
#endif

                    pBbxList = &Bbxlist[Inst][pCtrl->SinkInfoIdx];
                    RefOD_RefODTransSSD(pCtrl, pInPicInfo, pSSD_Out, pBbxList);

                    pBbxList->MsgCode = SVC_CV_DETRES_BBX;
                    pBbxList->Source  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
                    Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                           SVC_CV_FLOW_OUTPUT_PORT0,
                                           pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                           pBbxList);
                } else {
                    /* Do nothing */
                }

                if (SVC_OK == Rval) {
                    pCtrl->SinkInfoIdx++;
                    if (pCtrl->SinkInfoIdx >= SVC_REFOD_OUTPUT_DEPTH) {
                        pCtrl->SinkInfoIdx = 0;
                    }
                }
            }

            Rval = RefOD_SemGive(Inst);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REFOD, "RxDataSinkHandler[%u]: SemGive error", Inst, 0U);
            }

            if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                RefOD_SendReceiverReady(Inst, &pCtrl->FeederPort);
            }
        }

        pCtrl->RxSeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_REFOD, "Rx[%u]: recv error", Inst, 0U);
    }

    SVC_CV_DBG(SVC_LOG_REFOD, "Rx[%u]: end", Inst, 0U);

    return SVC_OK;
}

/**
* Task entry of RX process
*/
static void* RefOD_RxTaskEntry(void* EntryArg)
{
    const ULONG  *pArg;
    UINT32 Rval = SVC_OK, Inst;
    const SVC_REFOD_CTRL_s *pCtrl;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Inst = (UINT32)(*pArg);
    pCtrl = &g_RefODCtrl[Inst];

    SvcLog_DBG(SVC_LOG_REFOD, "RxTaskEntry[%d] start", Inst, 0U);

    while (SVC_OK == Rval) {
        if (NULL != pCtrl->pRxProc) {
            Rval = pCtrl->pRxProc(Inst);
        } else {
            Rval = SVC_NG;
        }
    }

    SvcLog_DBG(SVC_LOG_REFOD, "RxTaskEntry[%d] exit", Inst, 0U);

    return NULL;
}

/**
* Set and wait TX and RX task to idle state
*/
static UINT32 RefOD_SetTaskIdle(UINT32 Inst)
{
    UINT32 Rval;
    SVC_REFOD_CTRL_s *pCtrl = &g_RefODCtrl[Inst];
    UINT32 i;

    /* Disable input and output */
    pCtrl->Enable = 0U;

    if (SVC_OK != SvcImgFeeder_Disconnect(&pCtrl->FeederPort)) {
        SvcLog_NG(SVC_LOG_REFOD, "Disconn feeder port error", 0U, 0U);
    }

    if (SVC_OK != SvcImgFeeder_ClosePort(&pCtrl->FeederPort)) {
        SvcLog_NG(SVC_LOG_REFOD, "Close feeder port error", 0U, 0U);
    }

    /* Wait Rx idle */
    for (i = 0; i < SVC_REFOD_SEM_INIT_COUNT; i++) {
        Rval = RefOD_SemTake(Inst);
    }

    return Rval;
}

/**
* Send external YUV to driver
*/
static UINT32 RefOD_SendExtYUV(UINT32 Inst, const void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_REFOD_CTRL_s *pCtrl = &g_RefODCtrl[Inst];
    void *pInputImg; /* SVC_CV_INPUT_IMG_INFO_s */

    if (NULL != pCtrl->pTxProc) {
        AmbaMisra_TypeCast(&pInputImg, &pInfo);
        Rval = pCtrl->pTxProc(Inst, &pCtrl->FeederPort, SVC_IMG_FEEDER_IMAGE_DATA_READY, pInputImg);
    }

    return Rval;
}

#if 0
/**
* Set scene mode
*/
static UINT32 RefOD_SetSceneMode(UINT32 Inst, const void *pCtrlInfo)
{
    UINT32 Rval = SVC_OK;
    const UINT32 *pSceneMode;
    SVC_REFOD_CTRL_s *pCtrl = &g_RefODCtrl[Inst];
    const SVC_CV_FLOW_REFOD_MODE_INFO_s *pModeInfo = &SvcCvFlow_RefOD_ModeInfo[pCtrl->pCvFlow->CvModeID];
    const SVC_CV_FLOW_REFOD_NMS_CFG_s *pNmsCfg;

    AmbaMisra_TypeCast(&pSceneMode, &pCtrlInfo);
    if (*pSceneMode < REFOD_MAX_SCENE_MODE) {
        if (pCtrl->Started == 1U) {
            pNmsCfg = pModeInfo->pNmsCfg[*pSceneMode];
            if (pNmsCfg != NULL) {
                Rval = RefOD_ConfigNms(pCtrl, pNmsCfg);
                if (Rval == SVC_OK) {
                    pCtrl->SceneMode = (UINT8)*pSceneMode;
                }
            } else {
                SvcLog_DBG(SVC_LOG_REFOD, "Set scene mode(%u) NmsCfg = NULL", *pSceneMode, 0U);
            }
        } else {
            pCtrl->SceneMode = (UINT8)*pSceneMode;
        }
    } else {
        SvcLog_NG(SVC_LOG_REFOD, "Set scene mode(%u) error", *pSceneMode, 0U);
    }

    return Rval;
}
#endif
/**
* CvFlow driver source rate update function
*/
static void RefOD_SrcRateUpdate(SVC_REFOD_CTRL_s *pCtrl, UINT32 SrcRate)
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
            (Den <= REFOD_SRC_RATE_MAX_DENOMINATOR) && (Num <= Den)) {
            Rval = AmbaWrap_memset(&pCtrl->SrcValidTbl[0], 0, sizeof(pCtrl->SrcValidTbl));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REFOD, "memset SrcValidTbl failed", 0U, 0U);
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
            SvcLog_NG(SVC_LOG_REFOD, "Invalid ratio: %u/%u", Num, Den);
        }
    } else {
        /* Divisor */
        if (SrcRate == 0U) {
            pCtrl->SrcRateCtrl = 1U;
        } else {
            pCtrl->SrcRateCtrl = SrcRate;
        }
    }

    SvcLog_OK(SVC_LOG_REFOD, "SrcRateCtrl: 0x%x", pCtrl->SrcRateCtrl, 0U);
}


/**
* Entry of configuration function
*/
static UINT32 RefOD_Config(UINT32 Inst, UINT32 AlgoIdx)
{
    UINT32 Rval = SVC_OK;
    const SVC_REFOD_CTRL_s *pCtrl = &g_RefODCtrl[Inst];
    const SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &pCtrl->AlgoOp[AlgoIdx];
    const SVC_CV_FLOW_REFOD_MODE_INFO_s *pModeInfo = &SvcCvFlow_RefOD_ModeInfo[pCtrl->pCvFlow->CvModeID];
    const SVC_CV_ROI_INFO_s *pRoiCfg = &pCtrl->pCvFlow->InputCfg.Input[AlgoIdx].RoiInfo;
    const SVC_CV_FLOW_REFOD_NMS_CFG_s *pNmsCfg = pModeInfo->Cfg[AlgoIdx].pNmsCfg[pCtrl->SceneMode];

    if (pAlgoOp->ExtBinNum != 0U) {
        Rval |= RefOD_ConfigExtBin(Inst, AlgoIdx);
    }

    if (pRoiCfg->NumRoi != 0U) {
        Rval |= RefOD_ConfigRoi(Inst, AlgoIdx, pRoiCfg);
    }

    if (pNmsCfg != NULL) {
        Rval |= RefOD_ConfigNms(Inst, AlgoIdx, pNmsCfg);
    }

    if (pAlgoOp->LcsBinBuf.pAddr != NULL) {
        Rval |= RefOD_ConfigLicense(Inst, AlgoIdx);
    }

    return Rval;
}

static UINT32 RefOD_ConfigRoi(UINT32 Inst, UINT32 AlgoIdx, const SVC_CV_ROI_INFO_s *pRoiCfg)
{
    UINT32 Rval;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &g_RefODCtrl[Inst].AlgoOp[AlgoIdx];
    amba_roi_config_t RoiMsg = {0};
    amba_roi_config_t *pRoiMsg = &RoiMsg;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;

    if (pRoiCfg != NULL) {
        /* Suppose each Algo has one ROI */
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
    } else {
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        SvcLog_OK(SVC_LOG_REFOD, "RefOD_ConfigRoi[%d]", Inst, 0U);
    } else {
        SvcLog_NG(SVC_LOG_REFOD, "RefOD_ConfigRoi[%d] error(%d)", Inst, Rval);
    }

    return Rval;
}

static UINT32 RefOD_ConfigNms(UINT32 Inst, UINT32 AlgoIdx, const SVC_CV_FLOW_REFOD_NMS_CFG_s *pNmsCfg)
{
    UINT32 Rval;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &g_RefODCtrl[Inst].AlgoOp[AlgoIdx];
    amba_od_nms_configs_t nms = {0};
    amba_od_nms_configs_t *pNms = &nms;
    amba_od_grouping_t group_cfg = {0};
    amba_od_grouping_t *pGroupCfg = &group_cfg;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;
    UINT32 idx;

    if (pNmsCfg != NULL) {
        /* RefOD */
        pNms->msg_type    = AMBANET_MSG_TYPE_OD_NMS;
        pNms->num_configs = pNmsCfg->NumConfig;

        /* 0 */
        Rval = AmbaWrap_memset(&nms.config[0], 0, sizeof(amba_od_nms_config_t));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_REFOD, "memset nmscfg failed", 0U, 0U);
        }

        /* 1 - 12 */
        for (idx = 1U; idx < pNmsCfg->NumConfig; idx++) {
            pNms->config[idx].nms_config_id        = (UINT16) idx;
            pNms->config[idx].max_final_results_m1 = (UINT32) pNmsCfg->Param[idx].MaxResult - 1U;
            pNms->config[idx].iou_enable           = 1;
            pNms->config[idx].iou_threshold        = 8192;        /* .50 */
            pNms->config[idx].iom_enable           = 0;
            pNms->config[idx].iom_threshold        = 0;
            pNms->config[idx].bb_width_threshold   = 8;
            pNms->config[idx].bb_height_threshold  = 8;
            pNms->config[idx].score_threshold      = pNmsCfg->Param[idx].ScoreTh;
        }

        /* Grouping for NMS */
        pGroupCfg->msg_type     = AMBANET_MSG_TYPE_OD_GROUPING;
        pGroupCfg->group_id[0]  = 0;
        pGroupCfg->group_id[1]  = 1;
        pGroupCfg->group_id[2]  = 1;
        pGroupCfg->group_id[3]  = 1;
        pGroupCfg->group_id[4]  = 1;
        pGroupCfg->group_id[5]  = 1;
        pGroupCfg->group_id[6]  = 2;
        pGroupCfg->group_id[7]  = 2;
        pGroupCfg->group_id[8]  = 3;
        pGroupCfg->group_id[9]  = 3;
        pGroupCfg->group_id[10] = 3;
        pGroupCfg->group_id[11] = 4;
        pGroupCfg->group_id[12] = 3;

        MsgCfg.CtrlType = 0;
        MsgCfg.pExtCtrlCfg = NULL;

        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pNms);
        Rval = SvcCvAlgo_Control(&pAlgoOp->AlgoHandle, &MsgCfg);

        AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pGroupCfg);
        Rval |= SvcCvAlgo_Control(&pAlgoOp->AlgoHandle, &MsgCfg);
    } else {
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        SvcLog_OK(SVC_LOG_REFOD, "RefOD_ConfigNms[%d]", Inst, 0U);
    } else {
        SvcLog_NG(SVC_LOG_REFOD, "RefOD_ConfigNms[%d] error(%d)", Inst, Rval);
    }

    return Rval;
}

static UINT32 RefOD_ConfigLicense(UINT32 Inst, UINT32 AlgoIdx)
{
#if 0//defined AMBANET_MSG_TYPE_LICENSE
    #define LICENSE_DATA_SIZE   (1020U)
    typedef struct {
        UINT32  Msg;
        UINT8   Payload[LICENSE_DATA_SIZE];
    } SVC_CV_FLOW_REFOD_LCS_CFG_s;
    UINT32 Rval;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &g_RefODCtrl[Inst].AlgoOp[AlgoIdx];
    SVC_CV_FLOW_REFOD_LCS_CFG_s LcsCfg;
    const SVC_CV_FLOW_REFOD_LCS_CFG_s *pLcsCfg = &LcsCfg;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;

    LcsCfg.Msg = AMBANET_MSG_TYPE_LICENSE;
    Rval = AmbaWrap_memcpy(LcsCfg.Payload, pAlgoOp->LcsBinBuf.pAddr, LICENSE_DATA_SIZE);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_REFOD, "memcpy LcsBinBuf failed", 0U, 0U);
    }

    MsgCfg.CtrlType = 0;
    MsgCfg.pExtCtrlCfg = NULL;
    AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pLcsCfg);
    Rval = SvcCvAlgo_Control(&pAlgoOp->AlgoHandle, &MsgCfg);

    if (Rval == SVC_OK) {
        SvcLog_OK(SVC_LOG_REFOD, "RefOD_ConfigLicense[%d]", Inst, 0U);
    } else {
        SvcLog_NG(SVC_LOG_REFOD, "RefOD_ConfigLicense[%d] error(%d)", Inst, Rval);
    }

    return Rval;
#else
    AmbaMisra_TouchUnused(&Inst);
    AmbaMisra_TouchUnused(&AlgoIdx);
    return SVC_OK;
#endif
}

static UINT32 RefOD_ConfigExtBin(UINT32 Inst, UINT32 AlgoIdx)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    const SVC_REFOD_CTRL_s *pCtrl = &g_RefODCtrl[Inst];
    const SVC_CV_FLOW_ALGO_INFO_s *pAlgoInfo = &SvcCvFlow_RefOD_ModeInfo[pCtrl->pCvFlow->CvModeID].AlgoGrp.AlgoInfo[AlgoIdx];
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp = &g_RefODCtrl[Inst].AlgoOp[AlgoIdx];
    OPENOD_PB_CTRL_s PriorBoxCfg;
    const OPENOD_PB_CTRL_s *pPriorBoxCfg = &PriorBoxCfg;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;

    for (i = 0; i < pAlgoOp->ExtBinNum; i++) {
        if (pAlgoInfo->ExtBinType[i] == REFOD_BIN_TYPE_PRIOR_BOX) {
            if (NULL != pAlgoOp->ExtBinBuf[i].pAddr) {
                if (SVC_OK != AmbaWrap_memset(&PriorBoxCfg, 0, sizeof(OPENOD_PB_CTRL_s))) {
                    SvcLog_NG(SVC_LOG_REFOD, "memset PriorBoxCfg failed", 0U, 0U);
                }

                PriorBoxCfg.msg_type = CTRL_TYPE_PBOX;
                AmbaMisra_TypeCast(&PriorBoxCfg.pPriorBox, &pAlgoOp->ExtBinBuf[i].pAddr);
                PriorBoxCfg.PBLen = pAlgoOp->ExtBinBuf[i].DataSize;

                MsgCfg.CtrlType = 0;
                MsgCfg.pExtCtrlCfg = NULL;
                AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pPriorBoxCfg);
                Rval |= SvcCvAlgo_Control(&pAlgoOp->AlgoHandle, &MsgCfg);
            }
        }
    }

    if (Rval == SVC_OK) {
        SvcLog_OK(SVC_LOG_REFOD, "RefOD_ConfigExtBin[%d]", Inst, 0U);
    } else {
        SvcLog_NG(SVC_LOG_REFOD, "RefOD_ConfigExtBin[%d] error(0x%x)", Inst, Rval);
    }

    return Rval;
}

static UINT32 RefOD_ConfigBuffer(UINT32 Inst)
{
    UINT32 Rval = SVC_OK;
    SVC_REFOD_CTRL_s *pCtrl = &g_RefODCtrl[Inst];
    SVC_CV_FLOW_BUF_INFO_s BufInfo = {0};
    UINT32 i;

    /* Img info buffer */
    if (pCtrl->pImgInfoBuf[0] == NULL) {
        for (i = 0; i < SVC_REFOD_PICINFO_QUEUE_SIZE; i++) {
            Rval = SvcCvFlow_AllocWorkBuf(sizeof(memio_source_recv_picinfo_t), &BufInfo);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pCtrl->pImgInfoBuf[i], &BufInfo.pAddr);
                pCtrl->ImgInfoBufSize = BufInfo.Size;
            } else {
                SvcLog_NG(SVC_LOG_REFOD, "Alloc Img info buf failed", 0U, 0U);
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
static UINT32 CvFlow_RefODInit(UINT32 CvFlowChan, void *pInit)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    static UINT32 g_RefODInit = 0U;

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(pInit);

    if (0U == g_RefODInit) {
        g_RefODInit = 1U;

        if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&ODMutex, NULL)) {
            SvcLog_NG(SVC_LOG_REFOD, "MutexCreate error", 0U, 0U);
            Rval = SVC_NG;
        }
        for (i = 0; i < SVC_REFOD_MAX_INSTANCE; i++) {
            g_RefODCtrl[i].Used = 0;
        }

        SVC_CV_DBG(SVC_LOG_REFOD, "CvFlow_RefODInit: first init", 0U, 0U);
    }

    SvcLog_OK(SVC_LOG_REFOD, "CvFlow_RefODInit: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver configuration function
*/
static UINT32 CvFlow_RefODConfig(UINT32 CvFlowChan, void *pConfig)
{
    UINT32 Rval = SVC_OK;
    UINT32 i, j;
    SVC_REFOD_CTRL_s *pCtrl = NULL;
    const SVC_CV_FLOW_REFOD_MODE_INFO_s *pModeInfo;
    SVC_CV_INPUT_CFG_s *pInputCfg;

    AmbaMisra_TouchUnused(pConfig);

    if (NULL != pConfig) {
        RefOD_MutexTake();
        /* Create instance */
        for (i = 0; i < SVC_REFOD_MAX_INSTANCE; i++) {
            if (0U == g_RefODCtrl[i].Used) {
                Rval = AmbaWrap_memset(&g_RefODCtrl[i], 0, sizeof(SVC_REFOD_CTRL_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_REFOD, "memset g_AmbaODCtrl[%u] failed", i, 0U);
                }
                pCtrl = &g_RefODCtrl[i];
                pCtrl->Used = 1;
                SvcLog_DBG(SVC_LOG_REFOD, "Create Inst(%u)", i, 0U);
                break;
            }
        }
        RefOD_MutexGive();

        /* Configure instance */
        if (NULL != pCtrl) {
            pCtrl->CvfChan     = (UINT8) CvFlowChan;
            pCtrl->pCvFlow     = &pCtrl->CvCfg;
            Rval = AmbaWrap_memcpy(&pCtrl->CvCfg, pConfig, sizeof(SVC_CV_FLOW_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REFOD, "memcpy CvCfg failed", 0U, 0U);
            }
            RefOD_SrcRateUpdate(pCtrl, pCtrl->pCvFlow->InputCfg.SrcRate);
            pCtrl->Enable      = 1;

            pModeInfo = &SvcCvFlow_RefOD_ModeInfo[pCtrl->pCvFlow->CvModeID];
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
                SvcLog_NG(SVC_LOG_REFOD, "unsupported InputNum(%u), NumRoi(%u)", pInputCfg->InputNum, pInputCfg->Input[0].RoiInfo.NumRoi);
            } else {
                if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG) {
                    Rval = SVC_OK;
                } else if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
#ifdef CONFIG_AMBALINK_BOOT_OS
                    /* Init FlexiDAG IO framework */
                    if (pCtrl->pCvFlow->PrivFlag < SVC_CV_FLEXIDAG_IO_MAX_CHANNEL) {
                        pCtrl->FlexiIOChan = pCtrl->pCvFlow->PrivFlag;
                        Rval = SvcCv_FlexiIORegister(pCtrl->FlexiIOChan, RefOD_Init_Callback);
                        if (Rval != SVC_OK) {
                            SvcLog_NG(SVC_LOG_REFOD, "SvcCv_FlexiIORegister(%u) failed", pCtrl->FlexiIOChan, 0U);
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_REFOD, "Invalid FlexidagIO chan(%u)", pCtrl->pCvFlow->PrivFlag, 0U);
                    }
#else
                    Rval = SVC_NG;
#endif
                } else {
                    SvcLog_NG(SVC_LOG_REFOD, "unsupported CVFlow ProfID(%u)", pModeInfo->ProfID, 0U);
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
        SvcLog_OK(SVC_LOG_REFOD, "CvFlow_RefODConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_REFOD, "CvFlow_RefODConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver load function
*/
static UINT32 CvFlow_RefODLoad(UINT32 CvFlowChan, void *pInfo)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_REFOD_CTRL_s *pCtrl;
    const SVC_CV_FLOW_LOAD_INFO_s *pLoadInfo;
    const SVC_CV_FLOW_REFOD_MODE_INFO_s *pModeInfo;
    UINT32 i;

    AmbaMisra_TouchUnused(pInfo);

    Rval = RefOD_CvfChan2Inst(CvFlowChan, &Inst);
    if (Rval == SVC_OK) {
        pCtrl = &g_RefODCtrl[Inst];
        if (NULL != pInfo) {
            AmbaMisra_TypeCast(&pLoadInfo, &pInfo);
            if (pLoadInfo->NumBin > 0U) {
                /* Use external path */
                if (pLoadInfo->AlgoIdx < REFOD_MAX_ALGO_NUM) {
                    Rval = SvcCvFlow_LoadAlgoBin(SVC_LOG_REFOD, pLoadInfo, &pCtrl->AlgoOp[pLoadInfo->AlgoIdx]);
                } else {
                    SvcLog_NG(SVC_LOG_REFOD, "CvFlow_RefODLoad: CvFlowChan(%d) invalid AlgoIdx(%u)", CvFlowChan, pLoadInfo->AlgoIdx);
                }
            } else {
                /* Use ModeInfo */
                if (pCtrl->pCvFlow->CvModeID < REFOD_NUM_MODE) {
                    pModeInfo = &SvcCvFlow_RefOD_ModeInfo[pCtrl->pCvFlow->CvModeID];

                    for (i = 0; i < pModeInfo->AlgoGrp.NumAlgo; i++) {
                        Rval |= SvcCvFlow_LoadAlgo(SVC_LOG_REFOD, &pModeInfo->AlgoGrp.AlgoInfo[i], &pCtrl->AlgoOp[i]);
                    }
                } else  {
                    SvcLog_NG(SVC_LOG_REFOD, "CvFlow_RefODLoad: CvFlowChan(%d) invalid CvModeID(%u)", CvFlowChan, pCtrl->pCvFlow->CvModeID);
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_REFOD, "CvFlow_RefODLoad: invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_REFOD, "CvFlow_RefODLoad: CvFlowChan(%d) error", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver start function
*/
static UINT32 CvFlow_RefODStart(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst, i, j, AlgoIdx, Num;
    SVC_REFOD_CTRL_s *pCtrl;
    const SVC_CV_FLOW_REFOD_MODE_INFO_s *pModeInfo;
    SVC_CV_FLOW_BUF_INFO_s BufInfo = {0};
    const SVC_CV_FLOW_ALGO_INFO_s *pAlgoInfo;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
    SVC_CV_FLOW_ALGO_QUERY_CFG_s QueryCfg;
    SVC_CV_FLOW_ALGO_CREATE_CFG_s CreateCfg;
    SVC_CV_ALGO_REGCB_CFG_s CbCfg;
    SVC_IMG_FEEDER_PORT_CFG_s PortCfg = {0};
#ifdef CONFIG_AMBALINK_BOOT_OS
    SVC_CV_ALGO_LINUX_EX_CFG_s ExCfg;
#endif
    char Text[2];
    static char CbEvtQueName[] = "EvtQue##_RefOD";
    static char SemName[] = "OutBufSem";
    static REFOD_OUTPUT_MSG_s AlgoOutQue[SVC_REFOD_MAX_INSTANCE][SVC_REFOD_ALGO_OUT_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 ODRxTaskStack[SVC_REFOD_MAX_INSTANCE][SVC_REFOD_RX_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_IMG_FEEDER_EVENT_CALLBACK_f RefOD_TxDataSrcHandlerList[SVC_REFOD_MAX_INSTANCE] = {
        RefOD_TxDataSrcHandler0,
        RefOD_TxDataSrcHandler1,
        RefOD_TxDataSrcHandler2,
        RefOD_TxDataSrcHandler3,
    };
    static SVC_CV_ALGO_CALLBACK_f RefOD_AlgoOutCbList[SVC_REFOD_MAX_INSTANCE] = {
        RefOD_AlgoOutCallback0,
        RefOD_AlgoOutCallback1,
        RefOD_AlgoOutCallback2,
        RefOD_AlgoOutCallback3,
    };

    AmbaMisra_TouchUnused(pInput);
    AmbaMisra_TouchUnused(RefOD_TxDataSrcHandlerList);
    AmbaMisra_TouchUnused(RefOD_AlgoOutCbList);

    Rval = RefOD_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl     = &g_RefODCtrl[Inst];
        pModeInfo = &SvcCvFlow_RefOD_ModeInfo[pCtrl->pCvFlow->CvModeID];

        /* Open Feeder port first to enable frame FIFO earlier */
        if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
            /* The driver supports single input per instance */
            PortCfg.NumPath            = 1U;
            PortCfg.Content[0].DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
            PortCfg.Content[0].StrmId  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
            PortCfg.SendFunc           = RefOD_TxDataSrcHandlerList[Inst];
            if (SVC_OK != SvcImgFeeder_OpenPort(pCtrl->pCvFlow->InputCfg.FeederID, &PortCfg, &pCtrl->FeederPort)) {
                SvcLog_NG(SVC_LOG_REFOD, "Open feeder port error", 0U, 0U);
            }
        } else {
            if (pCtrl->pCvFlow->InputCfg.Input[0].DataSrc != SVC_CV_DATA_SRC_EXT_DATA) {
                SvcLog_NG(SVC_LOG_REFOD, "ImgFeeder is not enabled", 0U, 0U);
            }
        }

        for (AlgoIdx = 0; AlgoIdx < pModeInfo->AlgoGrp.NumAlgo; AlgoIdx++) {
            pAlgoInfo = &pModeInfo->AlgoGrp.AlgoInfo[AlgoIdx];
            pAlgoOp   = &pCtrl->AlgoOp[AlgoIdx];

            Rval = AmbaWrap_memset(&QueryCfg, 0, sizeof(SVC_CV_FLOW_ALGO_QUERY_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REFOD, "memset QueryCfg failed", 0U, 0U);
            }
            Rval = SvcCvFlow_QueryAlgo(SVC_LOG_REFOD, pAlgoInfo, &QueryCfg, pAlgoOp);
            if (SVC_OK == Rval) {
                /* Create algo */
                Rval = AmbaWrap_memset(&CreateCfg, 0, sizeof(SVC_CV_FLOW_ALGO_CREATE_CFG_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_REFOD, "memset CreateCfg failed", 0U, 0U);
                }

                if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG) {

                } else if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
#ifdef CONFIG_AMBALINK_BOOT_OS
                   if (1U == SvcCv_FlexiIOStatusGet(pCtrl->FlexiIOChan)) {
                       const SVC_CV_ALGO_LINUX_EX_CFG_s *pExCfg = &ExCfg;

                       ExCfg.AmbaIPCChannel = pCtrl->FlexiIOChan;
                       AmbaMisra_TypeCast(&CreateCfg.pExtCfg, &pExCfg);
                   } else {
                       SvcLog_NG(SVC_LOG_REFOD, "FlexidagIO(%u) has not ready. Please run Linux app first.", pCtrl->FlexiIOChan, 0U);
                       Rval = SVC_NG;
                   }
#else
                    SvcLog_NG(SVC_LOG_REFOD, "CONFIG_AMBALINK_BOOT_OS not defined ", 0U, 0U);
                    Rval = SVC_NG;
#endif
                } else {
                    SvcLog_NG(SVC_LOG_REFOD, "unsupported CVFlow ProfID(%u)", pModeInfo->ProfID, 0U);
                    Rval = SVC_NG;
                }

                if (SVC_OK == Rval) {
                    Rval = SvcCvFlow_CreateAlgo(SVC_LOG_REFOD, pAlgoInfo, &CreateCfg, pAlgoOp);
                }

                if (SVC_OK == Rval) {
                    /* Create Output buffer */
                    for (i = 0; i < SVC_REFOD_OUTPUT_DEPTH; i++) {
                        Num = pAlgoOp->OutputNum;
                        for (j = 0; j < Num; j++) {
                            Rval |= SvcCvFlow_AllocWorkBuf(pAlgoOp->OutputSize[j], &BufInfo);
                            SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_REFOD, BufInfo.pAddr, BufInfo.Size, &pCtrl->AlgoOpEx[AlgoIdx].OutputBuf[i][j]);
                        }
                    }

                    if (SVC_OK == Rval) {
                        /* Register Algo callback */
                        Rval = AmbaWrap_memset(&CbCfg, 0, sizeof(SVC_CV_ALGO_REGCB_CFG_s));
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_REFOD, "memset CbCfg failed", 0U, 0U);
                        }
                        CbCfg.Mode     = 0U;
                        CbCfg.Callback = RefOD_AlgoOutCbList[Inst];
                        Rval = SvcCvAlgo_RegCallback(&pAlgoOp->AlgoHandle, &CbCfg);
                        if (SVC_OK == Rval) {
                            Rval = RefOD_Config(Inst, AlgoIdx);
                            if (SVC_OK != Rval) {
                                SvcLog_NG(SVC_LOG_REFOD, "FlexiDAG Config error(0x%X)", Rval, 0U);
                            }

                            pCtrl->NumValidAlgo++;
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_REFOD, "Alloc Algo[%u] Output Buf failed", AlgoIdx, 0U);
                    }
                } else {
                   SvcLog_NG(SVC_LOG_REFOD, "CreateAlgo[%u] error", AlgoIdx, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_REFOD, "QueryAlgo[%u] error", AlgoIdx, 0U);
            }
        }

        if (SVC_OK == Rval) {
            Rval = RefOD_ConfigBuffer(Inst);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REFOD, "ConfigBuffer error", 0U, 0U);
            }
        }

        if (SVC_OK == Rval) {
            pCtrl->pTxProc = RefOD_TxDataSrcHandler;
            pCtrl->pRxProc = RefOD_RxDataSinkHandler;

            if (1U != AmbaUtility_UInt32ToStr(&Text[0], sizeof(Text), Inst, 10)) {
                SvcLog_NG(SVC_LOG_REFOD, "UInt32ToStr failed", 0U, 0U);
            }
            CbEvtQueName[6] = Text[0];

            Rval = AmbaKAL_MsgQueueCreate(&pCtrl->AlgoOutQueId,
                                           CbEvtQueName,
                                           sizeof(REFOD_OUTPUT_MSG_s),
                                           &AlgoOutQue[Inst][0],
                                           SVC_REFOD_ALGO_OUT_QUEUE_SIZE * sizeof(REFOD_OUTPUT_MSG_s));

            Rval |= AmbaKAL_SemaphoreCreate(&pCtrl->OutBufSem, SemName, SVC_REFOD_SEM_INIT_COUNT);

            if (KAL_ERR_NONE == Rval) {
                /* Create RX task */
                pCtrl->RxTaskCtrl.Priority   = 60;
                pCtrl->RxTaskCtrl.EntryFunc  = RefOD_RxTaskEntry;
                pCtrl->RxTaskCtrl.EntryArg   = Inst;
                pCtrl->RxTaskCtrl.pStackBase = &ODRxTaskStack[Inst][0];
                pCtrl->RxTaskCtrl.StackSize  = SVC_REFOD_RX_TASK_STACK_SIZE;
                pCtrl->RxTaskCtrl.CpuBits    = 0x01U;
                Rval = SvcTask_Create("SvcODRxTask", &pCtrl->RxTaskCtrl);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_REFOD, "Create RxTask Inst(%u) error", Inst, 0U);
                }

                Rval = AmbaKAL_TaskSleep(100); AmbaMisra_TouchUnused(&Rval);
                /* Start input */
                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_REFOD, "Connect feeder port error", 0U, 0U);
                    }
                }

                if (SVC_OK == Rval) {
                    pCtrl->Started = 1U;
                }
            } else {
                SvcLog_NG(SVC_LOG_REFOD, "Create MsgQue/Semaphore error(0x%X)", Rval, 0U);
                Rval =  SVC_NG;
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_REFOD, "Invalid CvFlowChan", 0U, 0U);
        Rval =  SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_REFOD, "CvFlow_RefODStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_REFOD, "CvFlow_RefODStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver control function
*/
static UINT32 CvFlow_RefODControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo)
{
    UINT32 Rval, Inst;
    SVC_REFOD_CTRL_s *pCtrl;
    const UINT32 *pValue;
    SVC_CV_ROI_INFO_s  *pRoiInfo = NULL;
    SVC_CV_INPUT_INFO_s *pInputInfo = NULL;

    RefOD_MutexTake();

    Rval = RefOD_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl = &g_RefODCtrl[Inst];
        switch (CtrlType) {
            case SVC_CV_CTRL_ENABLE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->Enable = (UINT8) *pValue;
                if ((1U == pCtrl->Started) &&
                    (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder)) {
                    if (*pValue == 0U) {
                        Rval = SvcImgFeeder_Disconnect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_REFOD, "Disconn feeder port error", 0U, 0U);
                        }
                    } else {
                        Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_REFOD, "Cnnect feeder port error", 0U, 0U);
                        }
                    }
                }
                break;
            case SVC_CV_CTRL_SET_SRC_RATE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->SrcRateCtrl = *pValue;
                SvcLog_OK(SVC_LOG_REFOD, "Control[%d]: SrcRate = %u", Inst, *pValue);
                break;
            case SVC_CV_CTRL_SEND_EXT_YUV:
                Rval = RefOD_SendExtYUV(Inst, pInfo);
                break;
            case SVC_CV_CTRL_GET_ROI_INFO:
                AmbaMisra_TypeCast(&pRoiInfo, &pInfo);
                pInputInfo = &pCtrl->pCvFlow->InputCfg.Input[0];
                if (pInputInfo->RoiInfo.NumRoi != 0U) {
                    Rval = AmbaWrap_memcpy(pRoiInfo, &pInputInfo->RoiInfo, sizeof(SVC_CV_ROI_INFO_s));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_REFOD, "memcpy pRoiInfo failed", 0U, 0U);
                    }
                } else {
                    Rval = AmbaWrap_memset(pRoiInfo, 0x0, sizeof(SVC_CV_ROI_INFO_s));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_REFOD, "memset pRoiInfo failed", 0U, 0U);
                    }
                }
                break;
            case SVC_CV_CTRL_SET_ROI_INFO:
                AmbaMisra_TypeCast(&pRoiInfo, &pInfo);
                pInputInfo = &pCtrl->pCvFlow->InputCfg.Input[0];
                if (pInputInfo->RoiInfo.NumRoi != 0U) {
                    Rval = AmbaWrap_memcpy(&pInputInfo->RoiInfo, pRoiInfo, sizeof(SVC_CV_ROI_INFO_s));
                    if (SVC_OK == Rval) {
                        Rval = RefOD_ConfigRoi(Inst, 0U, &pInputInfo->RoiInfo);
                    } else {
                        SvcLog_NG(SVC_LOG_REFOD, "memcpy pRoiInfo failed", 0U, 0U);
                    }
                }
                break;
            default:
                SvcLog_NG(SVC_LOG_REFOD, "Control[%d]: Invalid CtrlType(%d)", Inst, CtrlType);
                break;
        }
    } else {
        SvcLog_NG(SVC_LOG_REFOD, "Control: Invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    RefOD_MutexGive();

    return Rval;
}

/**
* CvFlow driver stop function
*/
static UINT32 CvFlow_RefODStop(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_REFOD_CTRL_s *pCtrl;
    SVC_CV_FLOW_ALGO_DELETE_CFG_s DelCfg;
    UINT32 i;

    AmbaMisra_TouchUnused(pInput);

    Rval = RefOD_CvfChan2Inst(CvFlowChan, &Inst);

    if (SVC_OK == Rval) {
        pCtrl = &g_RefODCtrl[Inst];

        Rval = RefOD_SetTaskIdle(Inst);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_REFOD, "SetTaskIdle error", 0U, 0U);
        }

        Rval = SvcTask_Destroy(&pCtrl->RxTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_REFOD, "Destroy Tx/Rx Task error", 0U, 0U);
        }

        Rval = AmbaWrap_memset(&DelCfg, 0, sizeof(SVC_CV_FLOW_ALGO_DELETE_CFG_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_REFOD, "memset DelCfg failed", 0U, 0U);
        }

        for (i = 0; i < pCtrl->NumValidAlgo; i++) {
            Rval |= SvcCvFlow_DeleteAlgo(SVC_LOG_REFOD, &DelCfg, &pCtrl->AlgoOp[i]);
        }
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_REFOD, "Delele Algo error", 0U, 0U);
        }

#ifdef CONFIG_AMBALINK_BOOT_OS
        if (SvcCvFlow_RefOD_ModeInfo[pCtrl->pCvFlow->CvModeID].ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
            Rval = SvcCv_FlexiIORelease(pCtrl->FlexiIOChan);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_REFOD, "SvcCv_FlexiIORelease(%u) failed", pCtrl->FlexiIOChan, 0U);
            }
        }
#endif

        Rval = AmbaKAL_MsgQueueDelete(&pCtrl->AlgoOutQueId);
        Rval |= AmbaKAL_SemaphoreDelete(&pCtrl->OutBufSem);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_REFOD, "Delete MsgQue/Semaphore error(0x%X)", Rval, 0U);
        }

        /* Reset Ctrl Intance */
        Rval = AmbaWrap_memset(&g_RefODCtrl[Inst], 0, sizeof(SVC_REFOD_CTRL_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_REFOD, "memset g_RefODCtrl[%u] failed", Inst, 0U);
        }
    }

    SvcLog_OK(SVC_LOG_REFOD, "CvFlow_RefODStop: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver debug interface
*/
static UINT32 CvFlow_RefODDebug(UINT32 CvFlowChan, const char *pCmd, UINT32 Param0, UINT32 Param1, UINT32 Param2)
{
    UINT32 i;
    ULONG Addr;
    const SVC_REFOD_CTRL_s *pCtrl;
    const AMBA_CV_FLEXIDAG_HANDLE_s *pCvFDHandle;

    if (0 == SvcWrap_strcmp("debug", pCmd)) {
        SvcCvDebugEnable = (UINT8) Param0;
        SvcLog_DBG(SVC_LOG_REFOD, "Set debug(%u)", Param0, 0U);
    } else if (0 == SvcWrap_strcmp("info", pCmd)) {
        for (i = 0; i < SVC_REFOD_MAX_INSTANCE; i++) {
            pCtrl = &g_RefODCtrl[i];
            if (pCtrl->Used == 1U) {
                AmbaPrint_PrintUInt5("RefOD[%d]: CvfChan %d", i, pCtrl->CvfChan, 0U, 0U, 0U);
                pCvFDHandle = &pCtrl->AlgoOp[0].AlgoHandle.FDs[0].Handle;
                AmbaMisra_TypeCast(&Addr, &pCvFDHandle);
                SvcCvFlow_PrintULong("  CV_FLEXIDAG_HANDLE: 0x%x", Addr, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        SvcLog_DBG(SVC_LOG_REFOD, "Unsupported cmd", 0U, 0U);
    }

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(&Param0);
    AmbaMisra_TouchUnused(&Param1);
    AmbaMisra_TouchUnused(&Param2);

    return SVC_OK;
}

SVC_CV_FLOW_OBJ_s SvcCvFlow_RefODObj = {
    .Init         = CvFlow_RefODInit,
    .Config       = CvFlow_RefODConfig,
    .Load         = CvFlow_RefODLoad,
    .Start        = CvFlow_RefODStart,
    .Control      = CvFlow_RefODControl,
    .Stop         = CvFlow_RefODStop,
    .Debug        = CvFlow_RefODDebug,
};

