/**
 *  @file SvcCvFlow_RemoteApp.c
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
 *  @details Implementation of remote CV application
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
//#include "SvcCvAlgo.h"
#include "SvcCvFlowProfile.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowUtil.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvFlow_Shared.h"
#include "SvcCvFlow_RemoteApp.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcImgFIFO.h"
#include "SvcImgFeeder.h"
#include "SvcOsd.h"
#include "SvcWrap.h"
#include "SvcMem.h"
#include "SvcBuffer.h"

#include "schdr_api.h"
#include "cvtask_ossrv.h"

#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_ref_util.h"
#include "cv_ambaod_header.h"
#include "idsp_roi_msg.h"
#include "cvapi_svccvalgo_openod.h"
#include "cvapi_svccvalgo_ambaod.h"
#include "cvapi_amba_mask_interface.h"
#include "SSD.h"

#if (defined(CONFIG_AMBALINK_BOOT_OS) || defined(CONFIG_LINUX))
#include "cvapi_svccvalgo_linux.h"
#include "AmbaIPC_FlexidagIO.h"
#include "SvcCvFlexidagIO.h"
#include "AmbaMAL.h"
#endif

#define SVC_LOG_REMOTE_APP        "RemoteApp"

#define SVC_REMOTE_APP_INSTANCE0                (0U)
#define SVC_REMOTE_APP_INSTANCE1                (1U)
#define SVC_REMOTE_APP_INSTANCE2                (2U)
#define SVC_REMOTE_APP_INSTANCE3                (3U)
#define SVC_REMOTE_APP_MAX_INSTANCE             (4U)

#define SVC_REMOTE_APP_SEM_INIT_COUNT           (1U)
#define SVC_REMOTE_APP_OUTPUT_DEPTH             (8U)

#define SVC_REMOTE_APP_RX_TASK_STACK_SIZE       (0x4000)
#define SVC_REMOTE_APP_PICINFO_QUEUE_SIZE       (16U)
#define SVC_REMOTE_APP_ALGO_OUT_QUEUE_SIZE      (SVC_REMOTE_APP_OUTPUT_DEPTH)

#define SRC_RATE_FRACTION_MODE_BIT          (0x10000000U)

typedef UINT32 (*SVC_REMOTE_APP_DATA_HANDLER_f)(UINT32 Inst);
typedef UINT32 (*SVC_REMOTE_APP_TX_HANDLER_f)(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

typedef struct {
    UINT8                       Used;
    UINT8                       CvfChan;
    UINT8                       Enable;
    UINT8                       Started;
    SVC_CV_FLOW_CFG_s           *pCvFlow;
    SVC_CV_FLOW_CFG_s           CvCfg;

    void                       *pImgInfoBuf[SVC_REMOTE_APP_PICINFO_QUEUE_SIZE];
    UINT32                      ImgInfoBufSize;

    /* Data source service */
    SVC_IMG_FEEDER_PORT_s       FeederPort;
    UINT32                      SrcSeqNum;
    SVC_REMOTE_APP_TX_HANDLER_f pTxProc;
    UINT32                      TxSeqNum;
    UINT32                      PicInfoIdx;
    /* Driver intermediate data buffer */
    void                        *pInterDataBuf[SVC_REMOTE_APP_OUTPUT_DEPTH];
    UINT32                      InterDataIdx;

    /* Data sink service */
    SVC_TASK_CTRL_s             RxTaskCtrl;
    SVC_REMOTE_APP_DATA_HANDLER_f pRxProc;
    UINT32                      RxSeqNum;
    AMBA_KAL_SEMAPHORE_t        OutBufSem;
    UINT32                      SinkInfoIdx;

    /* Algo Callback */
    AMBA_KAL_MSG_QUEUE_t        AlgoOutQueId;

    /* Misc */
    UINT32                      SrcRateCtrl;
#define                         REMOTE_APP_SRC_RATE_MAX_DENOMINATOR  (16U)
    UINT8                       SrcValidTbl[REMOTE_APP_SRC_RATE_MAX_DENOMINATOR];
    UINT8                       SrcRateNum;
    UINT8                       SrcRateDen;
    UINT8                       SrcInfoErr;
    UINT32                      FlexiIOChan;    /* for AmbaLink CV only */
} SVC_REMOTE_APP_CTRL_s;

typedef struct {
    memio_source_recv_picinfo_t *pPicInfo;      /* input pic info */
    memio_sink_send_out_t       SinkOut;
} REMOTE_APP_INTER_DATA_s;

typedef struct {
   UINT32 Event;
   REMOTE_APP_INTER_DATA_s *pInterData;
} REMOTE_APP_OUTPUT_MSG_s;

typedef struct {
    const char **pDPtr;
} SVC_CHAR_PTR_s;


static SVC_REMOTE_APP_CTRL_s g_RemoteAppCtrl[SVC_REMOTE_APP_MAX_INSTANCE] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MUTEX_t ODMutex;
static UINT8 SvcCvDebugEnable = 0;
static UINT8 SvcCvDumpEnable[SVC_REMOTE_APP_MAX_INSTANCE] = {0};

static UINT32 CvFlow_RemoteAppInit(UINT32 CvFlowChan, void *pInit);
static UINT32 CvFlow_RemoteAppConfig(UINT32 CvFlowChan, void *pConfig);
static UINT32 CvFlow_RemoteAppStart(UINT32 CvFlowChan, void *pInput);
static UINT32 CvFlow_RemoteAppControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo);
static UINT32 CvFlow_RemoteAppStop(UINT32 CvFlowChan, void *pInput);
static UINT32 RemoteApp_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst);

static UINT32 RemoteApp_TxDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 RemoteApp_TxDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 RemoteApp_TxDataSrcHandler2(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 RemoteApp_TxDataSrcHandler3(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

static void SVC_CV_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcCvDebugEnable > 0U) {
        SvcLog_DBG(pModule, pFormat, Arg1, Arg2);
    }
}

static void RemoteApp_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&ODMutex, 5000)) {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "RemoteApp_MutexTake: timeout", 0U, 0U);
    }
}

static void RemoteApp_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&ODMutex)) {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "RemoteApp_MutexGive: error", 0U, 0U);
    }
}

static UINT32 RemoteApp_SemTake(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreTake(&g_RemoteAppCtrl[Inst].OutBufSem, 10000)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "RemoteApp_SemTake[%d]: timeout", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 RemoteApp_SemGive(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreGive(&g_RemoteAppCtrl[Inst].OutBufSem)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "RemoteApp_SemGive[%d]: error", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 RemoteApp_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst)
{
    UINT32 Rval = SVC_NG;
    UINT32 i;

    if (NULL != pInst) {
        for (i = 0; i < SVC_REMOTE_APP_MAX_INSTANCE; i++) {
            if ((1U == g_RemoteAppCtrl[i].Used) && (CvfChan == g_RemoteAppCtrl[i].CvfChan)) {
                *pInst = i;
                Rval = SVC_OK;
                break;
            }
        }
    }
    return Rval;
}

static UINT32 RemoteApp_IOChan2Inst(UINT32 IOChan, UINT32 *pInst)
{
    UINT32 Rval = SVC_NG;
    UINT32 i;

    if (NULL != pInst) {
        for (i = 0; i < SVC_REMOTE_APP_MAX_INSTANCE; i++) {
            if ((1U == g_RemoteAppCtrl[i].Used) && (IOChan == g_RemoteAppCtrl[i].FlexiIOChan)) {
                *pInst = i;
                Rval = SVC_OK;
                break;
            }
        }
    }
    return Rval;
}


#if (defined(CONFIG_AMBALINK_BOOT_OS) || defined(CONFIG_LINUX))
static UINT32 RemoteApp_Init_Callback(UINT32 Channel, UINT32 OutType)
{
    UINT32 Rval = SVC_OK;
    UINT32 Inst;
    SVC_REMOTE_APP_CTRL_s *pCtrl;

    AmbaPrint_PrintUInt5("RemoteApp_Init_Callback(%u)", Channel, 0U, 0U, 0U, 0U);

    for (Inst = 0; Inst < SVC_REMOTE_APP_MAX_INSTANCE; Inst++) {
        pCtrl = &g_RemoteAppCtrl[Inst];
        if (pCtrl->Used == 1U) {
            if (Channel == pCtrl->FlexiIOChan) {
                /* Start input */
                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_REMOTE_APP, "Connect feeder port error", 0U, 0U);
                    }
                }

                /* Issue status to application */
                Rval = SvcCvFlow_StatusEventIssue(pCtrl->CvfChan, SVC_CV_EVT_IPC_CHAN_READY, NULL);
            }
        }
    }

    AmbaMisra_TouchUnused(&OutType);

    return Rval;
}
#endif

/**
* Pack data source to pic info structure
*/
static void RemoteApp_PackPicInfo(UINT32 Inst, const SVC_CV_INPUT_IMG_INFO_s *pInputImg, memio_source_recv_picinfo_t *pInfo)
{
    UINT32 Rval = SVC_OK;
    cv_pic_info_t *pPicInfo = &pInfo->pic_info;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    const SVC_REMOTE_APP_CTRL_s *pCtrl = &g_RemoteAppCtrl[Inst];
    UINT32 DataSrc, RelativeAddrOn = 0U;
    extern UINT32 SvcCvFlow_PackPicInfo_Pyramid(cv_pic_info_t *pPicInfo, const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo, UINT32 MainWidth, UINT32 MainHeight, UINT32 UseRelative, UINT32 LeftRight);
    extern UINT32 SvcCvFlow_PackPicInfo_MainYuv(cv_pic_info_t *pPicInfo, const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo, UINT32 UseRelative, UINT32 LeftRight);
    extern UINT32 SvcCvFlow_PackPicInfo_Lndt(cv_pic_info_t *pPicInfo, const AMBA_DSP_LNDT_DATA_RDY_s *pYuvInfo, UINT32 MainWidth, UINT32 MainHeight, UINT32 UseRelative, UINT32 LeftRight);
    extern void   SvcCvFlow_DumpPicInfo(const char *pModuleName, const cv_pic_info_t *pPicInfo);

    Rval = AmbaWrap_memset(pInfo, 0, sizeof(memio_source_recv_picinfo_t));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "memset picinfo failed", 0U, 0U);
    }

    DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
    if (pInputImg->NumInfo > 0U) {
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

    if (SvcCvDumpEnable[Inst] == 1U) {
        SvcCvFlow_DumpPicInfo(SVC_LOG_REMOTE_APP, pPicInfo);
    }

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "RemoteApp_PackPicInfo error", 0U, 0U);
    }
}

/**
* Send message to Feeder to update port status
*/
static void RemoteApp_SendReceiverReady(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort)
{
    SVC_REMOTE_APP_CTRL_s *pCtrl = &g_RemoteAppCtrl[Inst];
    AMBA_KAL_SEMAPHORE_INFO_s SemInfo;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreQuery(&pCtrl->OutBufSem, &SemInfo)) {
        if (SemInfo.CurrentCount > 0U) {
            if (SVC_OK != SvcImgFeeder_MsgToFeeder(pPort, SVC_IMG_FEEDER_RECEIVER_READY, NULL)) {
                SvcLog_NG(SVC_LOG_REMOTE_APP, "Send ReceiverReady[%u] error", Inst, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "ReceiverReady[%u]: SemQuery error", Inst, 0U);
    }
}

/**
* TX data source handler: process input data
*/
static UINT32 RemoteApp_TxDataSrcHandler_FlexiIO(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_REMOTE_APP_CTRL_s *pCtrl = &g_RemoteAppCtrl[Inst];
    const SVC_CV_INPUT_IMG_INFO_s *pInputImg;
    memio_source_recv_picinfo_t *pPicInfo;

    AmbaMisra_TouchUnused(pInfo);

    SVC_CV_DBG(SVC_LOG_REMOTE_APP, "Tx[%u]: run %d", Inst, pCtrl->TxSeqNum);

    if (Event == SVC_IMG_FEEDER_IMAGE_DATA_READY) {
        if ((pCtrl->Enable == 1U) &&
            (0U == (pCtrl->SrcSeqNum % pCtrl->SrcRateCtrl))) {
            Rval = RemoteApp_SemTake(Inst);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pInputImg, &pInfo);
                AmbaMisra_TypeCast(&pPicInfo, &pCtrl->pImgInfoBuf[pCtrl->PicInfoIdx]);
                RemoteApp_PackPicInfo(Inst, pInputImg, pPicInfo);

                Rval = AmbaIPC_FlexidagIO_SetInput(pCtrl->FlexiIOChan, pPicInfo, sizeof(memio_source_recv_picinfo_t));
                if (Rval != FLEXIDAGIO_OK) {
                    SvcLog_NG(SVC_LOG_REMOTE_APP, "AmbaIPC_FlexidagIO_SetInput Chan(%u) failed", pCtrl->FlexiIOChan, 0U);
                }

                if (SVC_OK == Rval) {
                    pCtrl->TxSeqNum++;
                    pCtrl->PicInfoIdx++;
                    if (pCtrl->PicInfoIdx >= SVC_REMOTE_APP_PICINFO_QUEUE_SIZE) {
                        pCtrl->PicInfoIdx = 0;
                    }
                } else {
                    SvcLog_NG(SVC_LOG_REMOTE_APP, "Tx[%u]: send error", Inst, 0U);
                }

                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    RemoteApp_SendReceiverReady(Inst, pPort);
                }
            } else {
                SvcLog_NG(SVC_LOG_REMOTE_APP, "Tx[%u]: SemTake error", Inst, 0U);
            }
        } else {
            if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                RemoteApp_SendReceiverReady(Inst, pPort);
                Rval = AmbaKAL_TaskSleep(1);
            }
        }

        pCtrl->SrcSeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "Tx[%u]: unsupported event(0x%x)", Inst, Event);
        Rval = SVC_NG;
    }

    SVC_CV_DBG(SVC_LOG_REMOTE_APP, "Tx[%u]: end", Inst, 0U);

    return Rval;
}

static UINT32 RemoteApp_TxDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_REMOTE_APP_CTRL_s *pCtrl = &g_RemoteAppCtrl[SVC_REMOTE_APP_INSTANCE0];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_REMOTE_APP_INSTANCE0, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 RemoteApp_TxDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_REMOTE_APP_CTRL_s *pCtrl = &g_RemoteAppCtrl[SVC_REMOTE_APP_INSTANCE1];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_REMOTE_APP_INSTANCE1, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 RemoteApp_TxDataSrcHandler2(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_REMOTE_APP_CTRL_s *pCtrl = &g_RemoteAppCtrl[SVC_REMOTE_APP_INSTANCE2];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_REMOTE_APP_INSTANCE2, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 RemoteApp_TxDataSrcHandler3(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_REMOTE_APP_CTRL_s *pCtrl = &g_RemoteAppCtrl[SVC_REMOTE_APP_INSTANCE3];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_REMOTE_APP_INSTANCE3, pPort, Event, pInfo);
    }
    return Rval;
}

static void *SvcCMA_Remap(UINT64 PhyAddr, UINT64 Size)
{
#if 0
    AmbaMisra_TouchUnused(&PhyAddr);
    AmbaMisra_TouchUnused(&Size);
    return NULL;
#else
    UINT32 Rval = SVC_OK;
    void *Virt = NULL;
    UINT64 Attri = AMBA_MAL_ATTRI_CACHE;
    AMBA_MAL_INFO_s MalInfo = {0};

    Rval = AmbaMAL_GetInfoByPhys(PhyAddr, &MalInfo);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "SvcCMA_Remap: AmbaMAL_GetInfoByPhys() fail! Rval=0x%x", Rval, 0U);
    } else {
        Rval = AmbaMAL_Map(MalInfo.Id, PhyAddr, Size, Attri, &Virt);
        if ((Rval != OK) || (Virt == NULL)) {
            SvcLog_NG(SVC_LOG_REMOTE_APP, "SvcCMA_Remap: AmbaMAL_Map() fail! Rval=0x%x", Rval, 0U);
        }
    }

    return Virt;
#endif
}

/**
* Transfer detection result to SVC structure
*/
static void RemoteApp_RemoteAppTrans(const SVC_REMOTE_APP_CTRL_s *pCtrl, const memio_source_recv_picinfo_t *pInPicInfo, const amba_od_out_t *pOdResult, SVC_CV_DETRES_BBX_LIST_s *pBbxList)
{
    UINT32 i = 0;
    UINT32 ObjNum;
    ULONG  ObjAddr;
    const amba_od_candidate_t   *pObjBase, *pObj;

    AmbaMisra_TouchUnused(&pCtrl);
    AmbaMisra_TouchUnused(&pInPicInfo);

    AmbaMisra_TypeCast(&ObjAddr, &pOdResult);
    ObjAddr = ObjAddr + pOdResult->objects_offset;
    AmbaMisra_TypeCast(&pObjBase, &ObjAddr);
    ObjNum = pOdResult->num_objects;

    //AmbaPrint_PrintUInt5("[RemoteApp_RemoteAppTrans] amba OD ========= total:%3d =============", ObjNum, 0U, 0U, 0U, 0U);
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
            AmbaPrint_PrintInt5("[RemoteApp_RemoteAppTrans] cat:%d x:%4d y:%4d w:%3d h:%3d",
                    (INT32)pObj->clsId,
                    pObj->bb_start_col,
                    pObj->bb_start_row,
                    (INT32)pObj->bb_width_m1,
                    (INT32)pObj->bb_height_m1);
#endif
        }

#ifndef CONFIG_THREADX //for pure Linux case
        {
            uint32_t *clslist;
            static char *ClsListTable[32] = {0};

            //SvcCvFlow_PrintULong("class_name_p64=0x%llx, class_name=0x%x!",
            //    pOdResult->class_name_p64, pOdResult->class_name, 0, 0U, 0U);

            if (pOdResult->class_name_p64 != 0U) {
                //clslist = (uint32_t *)ambacv_p2v(pOdResult->class_name_p64);
                clslist = (uint32_t *)SvcCMA_Remap(pOdResult->class_name_p64, 32*sizeof(uint32_t));
            } else {
                //clslist = (uint32_t *)ambacv_p2v((ULONG)pOdResult->class_name);
                clslist = (uint32_t *)SvcCMA_Remap(pOdResult->class_name, 32*sizeof(uint32_t));
            }

            if (clslist != NULL) {
                for(i=0;i<32;i++) {
                    if (clslist[i] != 0) {
                        //ClsListTable[i] = (char *)ambacv_p2v((ULONG)clslist[i]);
                        ClsListTable[i] = (char *)SvcCMA_Remap((ULONG)clslist[i], 16*sizeof(char));
                        if (ClsListTable[i] == NULL) {
                            SvcLog_NG(SVC_LOG_REMOTE_APP, "RemoteApp_RemoteAppTrans: fail to get class name [%u]=0x%lx!", i, clslist[i]);
                        }
                    } else {
                        break;
                    }
                }
                pBbxList->class_name = (const char **)ClsListTable;
            } else {
                SvcLog_NG(SVC_LOG_REMOTE_APP, "RemoteApp_RemoteAppTrans: fail to get class name list!", 0U, 0U);
            }
        }
#endif
    } else {
        //SvcLog_NG(SVC_LOG_REMOTE_APP, "RemoteApp_RemoteAppTrans: wrong ObjNum = %u", ObjNum, 0U);
    }
}

/**
* Algorithm output event callback function
*/
static UINT32 RemoteApp_AlgoOutCallback(UINT32 Channel, const void *pData, UINT32 Len)
{
    UINT32 Rval = SVC_OK;
    UINT32 Inst = 0;
    SVC_REMOTE_APP_CTRL_s *pCtrl;
    REMOTE_APP_INTER_DATA_s *pInterData;
    REMOTE_APP_OUTPUT_MSG_s Msg;

    Rval = RemoteApp_IOChan2Inst(Channel, &Inst);
    if (Rval == SVC_OK) {
        pCtrl = &g_RemoteAppCtrl[Inst];

        if (NULL != pData) {
            AmbaMisra_TypeCast(&pInterData, &pCtrl->pInterDataBuf[pCtrl->InterDataIdx]);
            Rval = AmbaWrap_memcpy(&pInterData->SinkOut, pData, sizeof(memio_sink_send_out_t));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REMOTE_APP, "memcpy AlgoOut failed", 0U, 0U);
            }

            Msg.Event = CALLBACK_EVENT_OUTPUT;
            Msg.pInterData = pInterData;
            Rval = AmbaKAL_MsgQueueSend(&pCtrl->AlgoOutQueId, &Msg, 5000);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REMOTE_APP, "RemoteApp_AlgoOutCallback[%u]: MsgQueueSend error", Inst, 0U);
            }

            pCtrl->InterDataIdx++;
            if (pCtrl->InterDataIdx >= SVC_REMOTE_APP_OUTPUT_DEPTH) {
                pCtrl->InterDataIdx = 0;
            }
        } else {
            SvcLog_NG(SVC_LOG_REMOTE_APP, "RemoteApp_AlgoOutCallback[%u]: invalid pData", Inst, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "Control: Invalid FlexiIOChan(%d)", Channel, 0U);
    }

    AmbaMisra_TouchUnused(&Len);

    return Rval;
}

/**
* RX data sink handler: process detection output
*/
static UINT32 RemoteApp_RxDataSinkHandler(UINT32 Inst)
{
    UINT32 Rval;
    SVC_REMOTE_APP_CTRL_s *pCtrl = &g_RemoteAppCtrl[Inst];
    const memio_source_recv_picinfo_t *pInPicInfo = NULL;
    REMOTE_APP_OUTPUT_MSG_s Msg;
    const memio_sink_send_out_t *pSinkOut = NULL;

    const amba_od_out_t *pAmbaOdOut = NULL;
    //const CVALGO_SSD_OUTPUT_s *pSSD_Out;

    SVC_CV_DETRES_BBX_LIST_s *pBbxList;
    SVC_CV_OUT_BUF_s *pOutBuf;
    static SVC_CV_DETRES_BBX_LIST_s Bbxlist[SVC_REMOTE_APP_MAX_INSTANCE][SVC_REMOTE_APP_OUTPUT_DEPTH] GNU_SECTION_NOZEROINIT;
    static SVC_CV_OUT_BUF_s OutBuf[SVC_REMOTE_APP_MAX_INSTANCE][SVC_REMOTE_APP_OUTPUT_DEPTH] GNU_SECTION_NOZEROINIT;
    const SVC_CV_FLOW_REMOTE_APP_MODE_INFO_s *pModeInfo = &SvcCvFlow_RemoteApp_ModeInfo[pCtrl->pCvFlow->CvModeID];
    UINT8 OutputType = pModeInfo->OutputType;
    UINT8 OutputBufIdx = pModeInfo->OutputBufIdx;
    ULONG DataAddr;
    UINT32 DataSize;

    SVC_CV_DBG(SVC_LOG_REMOTE_APP, "Rx[%u]: run %d", Inst, pCtrl->RxSeqNum);

    Rval = AmbaKAL_MsgQueueReceive(&pCtrl->AlgoOutQueId, &Msg, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK == Rval) {
        SVC_CV_DBG(SVC_LOG_REMOTE_APP, "Rx[%u]: recv out(%u)", Inst, 0);

        if (Msg.Event == CALLBACK_EVENT_OUTPUT) {
            if (pCtrl->Enable == 1U) {
                pSinkOut = &Msg.pInterData->SinkOut;

                if (OutputType == REMOTE_APP_OUTPUT_TYPE_BBX) {
                    void *pDataPtr = NULL;
                    DataAddr = (ULONG)pSinkOut->io[OutputBufIdx].addr;

                    if (DataAddr != 0U) {
                        pDataPtr = SvcCMA_Remap(DataAddr, sizeof(amba_od_out_t));
                        if ((Rval != SVC_OK) || (pDataPtr == NULL)) {
                            SvcLog_NG(SVC_LOG_REMOTE_APP, "Rx[%u]: SvcCMA_Remap(0x%lx) fail!", Inst, DataAddr);
                        } else {
                            AmbaMisra_TypeCast(&pAmbaOdOut, &pDataPtr);
                        }
                    }

                    if (pAmbaOdOut != NULL) {
                        pBbxList = &Bbxlist[Inst][pCtrl->SinkInfoIdx];
                        RemoteApp_RemoteAppTrans(pCtrl, pInPicInfo, pAmbaOdOut, pBbxList);

                        pBbxList->MsgCode = SVC_CV_DETRES_BBX;
                        pBbxList->Source  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
                        Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                               SVC_CV_FLOW_OUTPUT_PORT0,
                                               pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                               pBbxList);
                    } else {
                        SvcLog_NG(SVC_LOG_REMOTE_APP, "RxDataSinkHandler[%u]: Invalid data", Inst, 0U);
                    }
                } else if (OutputType == REMOTE_APP_OUTPUT_TYPE_OSDBUF) {
                    void *pDataPtr = NULL;

                    DataAddr = (ULONG)pSinkOut->io[OutputBufIdx].addr;
                    DataSize = pSinkOut->io[OutputBufIdx].size;
                    if (DataAddr != 0) {
                        pDataPtr = SvcCMA_Remap(DataAddr, DataSize);
                        if ((Rval != SVC_OK) || (pDataPtr == NULL)) {
                            SvcLog_NG(SVC_LOG_REMOTE_APP, "Rx[%u]: SvcCMA_Remap(0x%lx) fail!", Inst, DataAddr);
                        }
                    }

                    if (pDataPtr != NULL) {
                        pOutBuf = &OutBuf[Inst][pCtrl->SinkInfoIdx];
                        pOutBuf->MsgCode = SVC_CV_OSD_BUFFER;
                        pOutBuf->Source  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
                        AmbaMisra_TypeCast(&pOutBuf->Addr, &pDataPtr);
                        Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                               SVC_CV_FLOW_OUTPUT_PORT0,
                                               pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                               pOutBuf);
                    } else {
                        SvcLog_NG(SVC_LOG_REMOTE_APP, "RxDataSinkHandler[%u]: Invalid data", Inst, 0U);
                    }
                } else {
                    /* Do nothing */
                }

                if (SVC_OK == Rval) {
                    pCtrl->SinkInfoIdx++;
                    if (pCtrl->SinkInfoIdx >= SVC_REMOTE_APP_OUTPUT_DEPTH) {
                        pCtrl->SinkInfoIdx = 0;
                    }
                }
            }

            Rval = RemoteApp_SemGive(Inst);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REMOTE_APP, "RxDataSinkHandler[%u]: SemGive error", Inst, 0U);
            }

            if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                RemoteApp_SendReceiverReady(Inst, &pCtrl->FeederPort);
            }
        }

        pCtrl->RxSeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "Rx[%u]: recv error", Inst, 0U);
    }

    SVC_CV_DBG(SVC_LOG_REMOTE_APP, "Rx[%u]: end", Inst, 0U);

    return SVC_OK;
}

/**
* Task entry of RX process
*/
static void* RemoteApp_RxTaskEntry(void* EntryArg)
{
    const ULONG  *pArg;
    UINT32 Rval = SVC_OK, Inst;
    const SVC_REMOTE_APP_CTRL_s *pCtrl;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Inst = (UINT32)(*pArg);
    pCtrl = &g_RemoteAppCtrl[Inst];

    SvcLog_DBG(SVC_LOG_REMOTE_APP, "RxTaskEntry[%d] start", Inst, 0U);

    while (SVC_OK == Rval) {
        if (NULL != pCtrl->pRxProc) {
            Rval = pCtrl->pRxProc(Inst);
        } else {
            Rval = SVC_NG;
        }
    }

    SvcLog_DBG(SVC_LOG_REMOTE_APP, "RxTaskEntry[%d] exit", Inst, 0U);

    return NULL;
}

/**
* Set and wait TX and RX task to idle state
*/
static UINT32 RemoteApp_SetTaskIdle(UINT32 Inst)
{
    UINT32 Rval;
    SVC_REMOTE_APP_CTRL_s *pCtrl = &g_RemoteAppCtrl[Inst];
    UINT32 i;

    /* Disable input and output */
    pCtrl->Enable = 0U;

    if (SVC_OK != SvcImgFeeder_Disconnect(&pCtrl->FeederPort)) {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "Disconn feeder port error", 0U, 0U);
    }

    if (SVC_OK != SvcImgFeeder_ClosePort(&pCtrl->FeederPort)) {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "Close feeder port error", 0U, 0U);
    }

    /* Wait Rx idle */
    for (i = 0; i < SVC_REMOTE_APP_SEM_INIT_COUNT; i++) {
        Rval = RemoteApp_SemTake(Inst);
    }

    return Rval;
}

/**
* Send external YUV to driver
*/
static UINT32 RemoteApp_SendExtYUV(UINT32 Inst, const void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_REMOTE_APP_CTRL_s *pCtrl = &g_RemoteAppCtrl[Inst];
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
static void RemoteApp_SrcRateUpdate(SVC_REMOTE_APP_CTRL_s *pCtrl, UINT32 SrcRate)
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
            (Den <= REMOTE_APP_SRC_RATE_MAX_DENOMINATOR) && (Num <= Den)) {
            Rval = AmbaWrap_memset(&pCtrl->SrcValidTbl[0], 0, sizeof(pCtrl->SrcValidTbl));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REMOTE_APP, "memset SrcValidTbl failed", 0U, 0U);
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
            SvcLog_NG(SVC_LOG_REMOTE_APP, "Invalid ratio: %u/%u", Num, Den);
        }
    } else {
        /* Divisor */
        if (SrcRate == 0U) {
            pCtrl->SrcRateCtrl = 1U;
        } else {
            pCtrl->SrcRateCtrl = SrcRate;
        }
    }

    SvcLog_OK(SVC_LOG_REMOTE_APP, "SrcRateCtrl: 0x%x", pCtrl->SrcRateCtrl, 0U);
}

static UINT32 RemoteApp_ConfigBuffer(UINT32 Inst)
{
    UINT32 Rval = SVC_OK;
    SVC_REMOTE_APP_CTRL_s *pCtrl = &g_RemoteAppCtrl[Inst];
    SVC_CV_FLOW_BUF_INFO_s BufInfo = {0};
    UINT32 i;

    /* Img info buffer */
    if (pCtrl->pImgInfoBuf[0] == NULL) {
        for (i = 0; i < SVC_REMOTE_APP_PICINFO_QUEUE_SIZE; i++) {
            Rval = SvcCvFlow_AllocWorkBuf((UINT32)sizeof(memio_source_recv_picinfo_t), &BufInfo);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pCtrl->pImgInfoBuf[i], &BufInfo.pAddr);
                pCtrl->ImgInfoBufSize = BufInfo.Size;
            } else {
                SvcLog_NG(SVC_LOG_REMOTE_APP, "Alloc Img info buf failed", 0U, 0U);
                Rval = SVC_NG;
                break;
            }
        }
    }

    /* Inter data buffer */
    if (pCtrl->pInterDataBuf[0] == NULL) {
        for (i = 0; i < SVC_REMOTE_APP_OUTPUT_DEPTH; i++) {
            Rval = SvcCvFlow_AllocWorkBuf((UINT32)sizeof(REMOTE_APP_INTER_DATA_s), &BufInfo);
            if (SVC_OK == Rval) {
                Rval = AmbaWrap_memset(BufInfo.pAddr, 0, BufInfo.Size);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_REMOTE_APP, "memset pInterDataBuf[%u] failed", i, 0U);
                }

                AmbaMisra_TypeCast(&pCtrl->pInterDataBuf[i], &BufInfo.pAddr);
            } else {
                SvcLog_NG(SVC_LOG_REMOTE_APP, "Alloc Inter data buf failed", 0U, 0U);
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
static UINT32 CvFlow_RemoteAppInit(UINT32 CvFlowChan, void *pInit)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    static UINT32 g_RemoteAppInit = 0U;

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(pInit);

    if (0U == g_RemoteAppInit) {
        g_RemoteAppInit = 1U;

        if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&ODMutex, NULL)) {
            SvcLog_NG(SVC_LOG_REMOTE_APP, "MutexCreate error", 0U, 0U);
            Rval = SVC_NG;
        }
        for (i = 0; i < SVC_REMOTE_APP_MAX_INSTANCE; i++) {
            g_RemoteAppCtrl[i].Used = 0;
        }

        SVC_CV_DBG(SVC_LOG_REMOTE_APP, "CvFlow_RemoteAppInit: first init", 0U, 0U);
    }

    SvcLog_OK(SVC_LOG_REMOTE_APP, "CvFlow_RemoteAppInit: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver configuration function
*/
static UINT32 CvFlow_RemoteAppConfig(UINT32 CvFlowChan, void *pConfig)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    SVC_REMOTE_APP_CTRL_s *pCtrl = NULL;

    AmbaMisra_TouchUnused(pConfig);

    if (NULL != pConfig) {
        RemoteApp_MutexTake();
        /* Create instance */
        for (i = 0; i < SVC_REMOTE_APP_MAX_INSTANCE; i++) {
            if (0U == g_RemoteAppCtrl[i].Used) {
                Rval = AmbaWrap_memset(&g_RemoteAppCtrl[i], 0, sizeof(SVC_REMOTE_APP_CTRL_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_REMOTE_APP, "memset g_RemoteAppCtrl[%u] failed", i, 0U);
                }
                pCtrl = &g_RemoteAppCtrl[i];
                pCtrl->Used = 1;
                SvcLog_DBG(SVC_LOG_REMOTE_APP, "Create Inst(%u)", i, 0U);
                break;
            }
        }
        RemoteApp_MutexGive();

        /* Configure instance */
        if (NULL != pCtrl) {
            pCtrl->CvfChan     = (UINT8) CvFlowChan;
            pCtrl->pCvFlow     = &pCtrl->CvCfg;
            Rval = AmbaWrap_memcpy(&pCtrl->CvCfg, pConfig, sizeof(SVC_CV_FLOW_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REMOTE_APP, "memcpy CvCfg failed", 0U, 0U);
            }
            RemoteApp_SrcRateUpdate(pCtrl, pCtrl->pCvFlow->InputCfg.SrcRate);
            pCtrl->Enable      = 1;

            if (pCtrl->pCvFlow->PrivFlag < SVC_CV_FLEXIDAG_IO_MAX_CHANNEL) {
                pCtrl->FlexiIOChan = pCtrl->pCvFlow->PrivFlag;
                Rval = SvcCv_FlexiIORegister(pCtrl->FlexiIOChan, RemoteApp_Init_Callback);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_REMOTE_APP, "SvcCv_FlexiIORegister(%u) failed", pCtrl->FlexiIOChan, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_REMOTE_APP, "Invalid FlexidagIO chan(%u)", pCtrl->pCvFlow->PrivFlag, 0U);
            }
        } else {
            Rval = SVC_NG;
        }
    } else {
        Rval = SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_REMOTE_APP, "CvFlow_RemoteAppConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "CvFlow_RemoteAppConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver load function
*/
static UINT32 CvFlow_RemoteAppLoad(UINT32 CvFlowChan, void *pInfo)
{
    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(pInfo);

    return SVC_OK;
}

/**
* CvFlow driver start function
*/
static UINT32 CvFlow_RemoteAppStart(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_REMOTE_APP_CTRL_s *pCtrl;
    SVC_IMG_FEEDER_PORT_CFG_s PortCfg = {0};
    char Text[2];
    static char CbEvtQueName[] = "EvtQue##_RemoteApp";
    static char SemName[] = "OutBufSem";
    static REMOTE_APP_OUTPUT_MSG_s AlgoOutQue[SVC_REMOTE_APP_MAX_INSTANCE][SVC_REMOTE_APP_ALGO_OUT_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 ODRxTaskStack[SVC_REMOTE_APP_MAX_INSTANCE][SVC_REMOTE_APP_RX_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_IMG_FEEDER_EVENT_CALLBACK_f RemoteApp_TxDataSrcHandlerList[SVC_REMOTE_APP_MAX_INSTANCE] = {
        RemoteApp_TxDataSrcHandler0,
        RemoteApp_TxDataSrcHandler1,
        RemoteApp_TxDataSrcHandler2,
        RemoteApp_TxDataSrcHandler3,
    };

    AmbaMisra_TouchUnused(pInput);
    AmbaMisra_TouchUnused(RemoteApp_TxDataSrcHandlerList);

    Rval = RemoteApp_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl     = &g_RemoteAppCtrl[Inst];

        /* Open Feeder port first to enable frame FIFO earlier */
        if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
            /* The driver supports single input per instance */
#ifdef CONFIG_ICAM_CV_STEREO
            PortCfg.NumPath            = 2U;
            PortCfg.Content[0].DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
            PortCfg.Content[0].StrmId  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
            PortCfg.Content[1].DataSrc = pCtrl->pCvFlow->InputCfg.Input[1].DataSrc;
            PortCfg.Content[1].StrmId  = pCtrl->pCvFlow->InputCfg.Input[1].StrmId;
#else
            PortCfg.NumPath            = 1U;
            PortCfg.Content[0].DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
            PortCfg.Content[0].StrmId  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
#endif
            PortCfg.SendFunc           = RemoteApp_TxDataSrcHandlerList[Inst];
            if (SVC_OK != SvcImgFeeder_OpenPort(pCtrl->pCvFlow->InputCfg.FeederID, &PortCfg, &pCtrl->FeederPort)) {
                SvcLog_NG(SVC_LOG_REMOTE_APP, "Open feeder port error", 0U, 0U);
            }
        } else {
            SvcLog_DBG(SVC_LOG_REMOTE_APP, "ImgFeeder is not enabled", 0U, 0U);
        }

        if (SVC_OK == Rval) {
            Rval = RemoteApp_ConfigBuffer(Inst);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_REMOTE_APP, "ConfigBuffer error", 0U, 0U);
            }
        }

        if (SVC_OK == Rval) {
            pCtrl->pTxProc = RemoteApp_TxDataSrcHandler_FlexiIO;
            pCtrl->pRxProc = RemoteApp_RxDataSinkHandler;

            if (1U != AmbaUtility_UInt32ToStr(&Text[0], (UINT32)sizeof(Text), Inst, 10)) {
                SvcLog_NG(SVC_LOG_REMOTE_APP, "UInt32ToStr failed", 0U, 0U);
            }
            CbEvtQueName[6] = Text[0];

            Rval = AmbaKAL_MsgQueueCreate(&pCtrl->AlgoOutQueId,
                                           CbEvtQueName,
                                           (UINT32)sizeof(REMOTE_APP_OUTPUT_MSG_s),
                                           &AlgoOutQue[Inst][0],
                                           SVC_REMOTE_APP_ALGO_OUT_QUEUE_SIZE * (UINT32)sizeof(REMOTE_APP_OUTPUT_MSG_s));

            Rval |= AmbaKAL_SemaphoreCreate(&pCtrl->OutBufSem, SemName, SVC_REMOTE_APP_SEM_INIT_COUNT);

            if (KAL_ERR_NONE == Rval) {
                /* Create RX task */
                pCtrl->RxTaskCtrl.Priority   = 60;
                pCtrl->RxTaskCtrl.EntryFunc  = RemoteApp_RxTaskEntry;
                pCtrl->RxTaskCtrl.EntryArg   = Inst;
                pCtrl->RxTaskCtrl.pStackBase = &ODRxTaskStack[Inst][0];
                pCtrl->RxTaskCtrl.StackSize  = SVC_REMOTE_APP_RX_TASK_STACK_SIZE;
                pCtrl->RxTaskCtrl.CpuBits    = 0x01U;
                Rval = SvcTask_Create("SvcODRxTask", &pCtrl->RxTaskCtrl);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_REMOTE_APP, "Create RxTask Inst(%u) error", Inst, 0U);
                }

                Rval = AmbaIPC_FlexidagIO_GetResult_SetCB(pCtrl->FlexiIOChan, RemoteApp_AlgoOutCallback);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_REMOTE_APP, "AmbaIPC_FlexidagIO_GetResult_SetCB Chan(%u) failed", pCtrl->FlexiIOChan, 0U);
                }
#if 0
                /* Start input */
                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_REMOTE_APP, "Connect feeder port error", 0U, 0U);
                    }
                }
#endif

                if (SVC_OK == Rval) {
                    pCtrl->Started = 1U;
                }
            } else {
                SvcLog_NG(SVC_LOG_REMOTE_APP, "Create MsgQue/Semaphore error(0x%X)", Rval, 0U);
                Rval =  SVC_NG;
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "Invalid CvFlowChan", 0U, 0U);
        Rval =  SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_REMOTE_APP, "CvFlow_RemoteAppStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "CvFlow_RemoteAppStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver control function
*/
static UINT32 CvFlow_RemoteAppControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo)
{
    UINT32 Rval, Inst;
    SVC_REMOTE_APP_CTRL_s *pCtrl;
    const UINT32 *pValue;
    SVC_CV_ROI_INFO_s  *pRoiInfo = NULL;
    SVC_CV_INPUT_INFO_s *pInputInfo = NULL;

    RemoteApp_MutexTake();

    Rval = RemoteApp_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl = &g_RemoteAppCtrl[Inst];
        switch (CtrlType) {
            case SVC_CV_CTRL_ENABLE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->Enable = (UINT8) *pValue;
                if ((1U == pCtrl->Started) &&
                    (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder)) {
                    if (*pValue == 0U) {
                        Rval = SvcImgFeeder_Disconnect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_REMOTE_APP, "Disconn feeder port error", 0U, 0U);
                        }
                    } else {
                        Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_REMOTE_APP, "Cnnect feeder port error", 0U, 0U);
                        }
                    }
                }
                break;
            case SVC_CV_CTRL_SET_SRC_RATE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->SrcRateCtrl = *pValue;
                SvcLog_OK(SVC_LOG_REMOTE_APP, "Control[%d]: SrcRate = %u", Inst, *pValue);
                break;
            case SVC_CV_CTRL_SEND_EXT_YUV:
                Rval = RemoteApp_SendExtYUV(Inst, pInfo);
                break;
            case SVC_CV_CTRL_GET_ROI_INFO:
                if (pInfo != NULL) {
                    AmbaMisra_TypeCast(&pRoiInfo, &pInfo);
                    pInputInfo = &pCtrl->pCvFlow->InputCfg.Input[0];
                    Rval = AmbaWrap_memcpy(pRoiInfo, &pInputInfo->RoiInfo, sizeof(SVC_CV_ROI_INFO_s));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_REMOTE_APP, "memcpy pRoiInfo failed", 0U, 0U);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_REMOTE_APP, "SVC_CV_CTRL_GET_ROI_INFO failed", 0U, 0U);
                }
                break;
            case SVC_CV_CTRL_SET_ROI_INFO:
                if (pInfo != NULL) {
                    AmbaMisra_TypeCast(&pRoiInfo, &pInfo);
                    pInputInfo = &pCtrl->pCvFlow->InputCfg.Input[0];
                    Rval = AmbaWrap_memcpy(&pInputInfo->RoiInfo, pRoiInfo, sizeof(SVC_CV_ROI_INFO_s));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_REMOTE_APP, "memcpy pRoiInfo failed", 0U, 0U);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_REMOTE_APP, "SVC_CV_CTRL_SET_ROI_INFO failed", 0U, 0U);
                }
                break;
            default:
                SvcLog_NG(SVC_LOG_REMOTE_APP, "Control[%d]: Invalid CtrlType(%d)", Inst, CtrlType);
                break;
        }
    } else {
        SvcLog_NG(SVC_LOG_REMOTE_APP, "Control: Invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    RemoteApp_MutexGive();

    return Rval;
}

/**
* CvFlow driver stop function
*/
static UINT32 CvFlow_RemoteAppStop(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_REMOTE_APP_CTRL_s *pCtrl;

    AmbaMisra_TouchUnused(pInput);

    Rval = RemoteApp_CvfChan2Inst(CvFlowChan, &Inst);

    if (SVC_OK == Rval) {
        pCtrl = &g_RemoteAppCtrl[Inst];

        Rval = RemoteApp_SetTaskIdle(Inst);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_REMOTE_APP, "SetTaskIdle error", 0U, 0U);
        }

        Rval = SvcTask_Destroy(&pCtrl->RxTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_REMOTE_APP, "Destroy Tx/Rx Task error", 0U, 0U);
        }

#if (defined(CONFIG_AMBALINK_BOOT_OS) || defined(CONFIG_LINUX))
        Rval = SvcCv_FlexiIORelease(pCtrl->FlexiIOChan);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_REMOTE_APP, "SvcCv_FlexiIORelease(%u) failed", pCtrl->FlexiIOChan, 0U);
        }
#endif

        Rval = AmbaKAL_MsgQueueDelete(&pCtrl->AlgoOutQueId);
        Rval |= AmbaKAL_SemaphoreDelete(&pCtrl->OutBufSem);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_REMOTE_APP, "Delete MsgQue/Semaphore error(0x%X)", Rval, 0U);
        }

        /* Reset Ctrl Intance */
        Rval = AmbaWrap_memset(&g_RemoteAppCtrl[Inst], 0, sizeof(SVC_REMOTE_APP_CTRL_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_REMOTE_APP, "memset g_RemoteAppCtrl failed", 0U, 0U);
        }
    }

    SvcLog_OK(SVC_LOG_REMOTE_APP, "CvFlow_RemoteAppStop: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver debug interface
*/
static UINT32 CvFlow_RemoteAppDebug(UINT32 CvFlowChan, const char *pCmd, UINT32 Param0, UINT32 Param1, UINT32 Param2)
{
    UINT32 Rval;
    UINT32 Inst;

    if (0 == SvcWrap_strcmp("debug", pCmd)) {
        SvcCvDebugEnable = (UINT8) Param0;
        SvcLog_DBG(SVC_LOG_REMOTE_APP, "Set debug(%u)", Param0, 0U);
    } else if (0 == SvcWrap_strcmp("dump", pCmd)) {
        Rval = RemoteApp_CvfChan2Inst(CvFlowChan, &Inst);
        if (SVC_OK == Rval) {
            SvcCvDumpEnable[Inst] = Param0;
            SvcLog_DBG(SVC_LOG_REMOTE_APP, "Set CvFlowChan(%u) dump(%u)", CvFlowChan, Param0);
        } else {
            SvcLog_NG(SVC_LOG_REMOTE_APP, "Invalid CvFlowChan", 0U, 0U);
        }
    } else {
        SvcLog_DBG(SVC_LOG_REMOTE_APP, "Unsupported cmd", 0U, 0U);
    }

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(&Param0);
    AmbaMisra_TouchUnused(&Param1);
    AmbaMisra_TouchUnused(&Param2);

    return SVC_OK;
}

SVC_CV_FLOW_OBJ_s SvcCvFlow_RemoteAppObj = {
    .Init         = CvFlow_RemoteAppInit,
    .Config       = CvFlow_RemoteAppConfig,
    .Load         = CvFlow_RemoteAppLoad,
    .Start        = CvFlow_RemoteAppStart,
    .Control      = CvFlow_RemoteAppControl,
    .Stop         = CvFlow_RemoteAppStop,
    .Debug        = CvFlow_RemoteAppDebug,
};

