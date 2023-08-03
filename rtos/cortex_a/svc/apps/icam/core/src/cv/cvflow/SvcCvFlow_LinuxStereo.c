/**
 *  @file SvcCvFlow_Linux_Stereo.c
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
#include "SvcCvFlow_Stereo.h"
#include "SvcCvFlow_Stixel.h"
#include "SvcCvFlow_LinuxStereo.h"
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
#include "cvapi_svccvalgo_linux.h"
#include "AmbaIPC_FlexidagIO.h"
#include "SvcCvFlexidagIO.h"
#endif


#define SVC_LOG_LINUX_STEREO        "Linux_Stereo"

#define SVC_LINUX_STEREO_INSTANCE0            (0U)
#define SVC_LINUX_STEREO_INSTANCE1            (1U)
#define SVC_LINUX_STEREO_MAX_INSTANCE         (2U)

#define SVC_LINUX_STEREO_SEM_INIT_COUNT       (2U)
#define SVC_LINUX_STEREO_OUTPUT_DEPTH         (4U)

#define SVC_LINUX_STEREO_RX_TASK_STACK_SIZE   (0x80000)
#define SVC_LINUX_STEREO_PICINFO_QUEUE_SIZE   (4U)
#define SVC_LINUX_STEREO_ALGO_OUT_QUEUE_SIZE  (SVC_LINUX_STEREO_OUTPUT_DEPTH)

#define SRC_RATE_FRACTION_MODE_BIT         (0x10000000U)

typedef UINT32 (*SVC_LINUX_STEREO_DATA_HANDLER_f)(UINT32 Inst);
typedef UINT32 (*SVC_LINUX_STEREO_TX_HANDLER_f)(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

typedef struct {
    UINT8                       MetaDataFlag;
    AppAcMetadata_t             MetaData;
    flexidag_memblk_t            OutputBuf[SVC_LINUX_STEREO_OUTPUT_DEPTH][FLEXIDAG_MAX_OUTPUTS];
    AMBA_CV_FLEXIDAG_IO_s       InBuf[SVC_LINUX_STEREO_OUTPUT_DEPTH];
    AMBA_CV_FLEXIDAG_IO_s       OutBuf[SVC_LINUX_STEREO_OUTPUT_DEPTH];
} SVC_CV_FLOW_Linux_Stereo_OP_EX_INFO_s;


typedef struct {
    UINT8                       Used;
    UINT8                       CvfChan;
    UINT8                       Enable;
    UINT8                       Started;
    SVC_CV_FLOW_CFG_s           *pCvFlow;
    SVC_CV_FLOW_CFG_s           CvCfg;

    UINT8                          NumValidAlgo;
    SVC_CV_FLOW_ALGO_OP_INFO_s    AlgoOp[SVC_LINUX_STEREO_MAX_ALGO_NUM];
    SVC_CV_FLOW_Linux_Stereo_OP_EX_INFO_s AlgoOpEx[SVC_LINUX_STEREO_MAX_ALGO_NUM];
    void                       *pImgInfoBuf[SVC_LINUX_STEREO_PICINFO_QUEUE_SIZE];
    UINT32                      ImgInfoBufSize;


    /* Data source service */
    SVC_IMG_FEEDER_PORT_s       FeederPort;
    UINT32                      SrcSeqNum;
    SVC_LINUX_STEREO_TX_HANDLER_f  pTxProc;
    UINT32                      TxSeqNum;
    UINT32                      PicInfoIdx;

    /* Data sink service */
    SVC_TASK_CTRL_s             MVACRxTaskCtrl;
    SVC_LINUX_STEREO_DATA_HANDLER_f pMvacRxProc;
    SVC_TASK_CTRL_s             StixelRxTaskCtrl;
    SVC_LINUX_STEREO_DATA_HANDLER_f pStixelRxProc;
    SVC_TASK_CTRL_s             FusionRxTaskCtrl;
    SVC_LINUX_STEREO_DATA_HANDLER_f pFusionRxProc;
    UINT32                      RxMvacSeqNum;
    UINT32                      RxStixelSeqNum;
    UINT32                      RxFusionSeqNum;
    UINT32                      OutBufIdx;
    AMBA_KAL_SEMAPHORE_t        OutBufSem;
    UINT32                      SinkInfoIdx;

    /* Algo Callback */
    AMBA_KAL_MSG_QUEUE_t        AlgoOutQueMVACId;
    AMBA_KAL_MSG_QUEUE_t        AlgoOutQueStixelId;
    AMBA_KAL_MSG_QUEUE_t        AlgoOutQueFusionId;
    void                       *pMVACOutBuf[SVC_LINUX_STEREO_OUTPUT_DEPTH];
    UINT32                      MVACOutBufSize;
    UINT32                      MVACOutBufIdx;
    UINT32                      StixelOutBufIdx;
    UINT32                      FusionOutBufIdx;
    void                       *pWarpOutBuf[SVC_LINUX_STEREO_OUTPUT_DEPTH];
    UINT32                      WarpOutBufSize;

    /* Misc */
    UINT32                      SrcRateCtrl;
#define                         Linux_Stereo_SRC_RATE_MAX_DENOMINATOR  (16U)
    UINT8                       SrcValidTbl[Linux_Stereo_SRC_RATE_MAX_DENOMINATOR];
    UINT8                       SrcRateNum;
    UINT8                       SrcRateDen;
    UINT8                       SrcInfoErr;
    UINT8                       SceneMode;
    UINT32                      FlexiIOChan;    /* for AmbaLink CV only */
} SVC_Linux_Stereo_CTRL_s;

typedef struct {
   UINT32 Event;
#define SVC_STEREO_AC_FINISH               (0x0U)
   AMBA_CV_MVAC_DATA_s *pAlgoOut;
} Linux_Stereo_OUTPUT_MSG_s;


typedef struct {
   UINT32 Event;
#define SVC_STEREO_STIXEL_FINISH               (0x1U)
   SVC_CV_ALGO_OUTPUT_s AlgoOut;
} STEREO_STIXEL_OUTPUT_MSG_s;


typedef struct {
   UINT32 Event;
#define SVC_STEREO_FUSION_FINISH               (0x2U)
   SVC_CV_ALGO_OUTPUT_s AlgoOut;
} STEREO_FUSION_OUTPUT_MSG_s;

typedef struct {
    uint32_t Type;
    AMBA_CV_SPU_DATA_s SpuData;
    memio_source_recv_picinfo_t PicInfo;
} SVC_CV_SPU_BUF_s;

typedef struct {
    uint32_t Type;
    uint32_t State; ///RESET, WAIT, ACCUMULATION, SOLVE, END
    uint32_t NumRun; ///< The number of runs.
    uint32_t NumWarpInfoSent; ///< The number of models sent out for updating the warp tables.
    AMBA_CV_MVAC_STATISTICS_s ViewStatistic;
    AMBA_CV_MVAC_WARP_INFO_s WarpInfo; //NULL when no warp table output
    uint32_t WarpValid;
} SVC_CV_MVAC_DATA_s;

typedef struct {
    uint32_t Type;
    AMBA_CV_STIXEL_DET_OUT_s StixelInfo;
} SVC_CV_STIXEL_DATA_s;


static SVC_Linux_Stereo_CTRL_s g_LinuxStereoCtrl[SVC_LINUX_STEREO_MAX_INSTANCE] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MUTEX_t Linux_StereoMutex;
static UINT8 SvcCvDebugEnable = 0;


static UINT32 CvFlow_LinuxStereoInit(UINT32 CvFlowChan, void *pInit);
static UINT32 CvFlow_LinuxStereoConfig(UINT32 CvFlowChan, void *pConfig);
static UINT32 CvFlow_LinuxStereoStart(UINT32 CvFlowChan, void *pInput);
static UINT32 CvFlow_LinuxStereoControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo);
static UINT32 CvFlow_LinuxStereoStop(UINT32 CvFlowChan, void *pInput);
static UINT32 LinuxStereo_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst);
static UINT32 LinuxStereo_Config(UINT32 Inst, AmbaAcConfig_t *pAcConfig);

static UINT32 LinuxStereo_TxDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 LinuxStereo_TxDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

static UINT32 LinuxStereo_AlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 LinuxStereo_AlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);


static void SVC_CV_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcCvDebugEnable > 0U) {
        SvcLog_DBG(pModule, pFormat, Arg1, Arg2);
    }
}

static void LinuxStereo_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&Linux_StereoMutex, 5000)) {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "LinuxStereo_MutexTake: timeout", 0U, 0U);
    }
}

static void LinuxStereo_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&Linux_StereoMutex)) {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "LinuxStereo_MutexGive: error", 0U, 0U);
    }
}
#if 0
static UINT32 Linux_Stereo_SemTake(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreTake(&g_LinuxStereoCtrl[Inst].OutBufSem, 10000)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "Linux_Stereo_SemTake[%d]: timeout", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 Linux_Stereo_SemGive(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreGive(&g_LinuxStereoCtrl[Inst].OutBufSem)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "Linux_Stereo_SemGive[%d]: error", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}
#endif
static UINT32 LinuxStereo_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst)
{
    UINT32 Rval = SVC_NG;
    UINT32 i;

    if (NULL != pInst) {
        for (i = 0; i < SVC_LINUX_STEREO_MAX_INSTANCE; i++) {
            if ((1U == g_LinuxStereoCtrl[i].Used) && (CvfChan == g_LinuxStereoCtrl[i].CvfChan)) {
                *pInst = i;
                Rval = SVC_OK;
                break;
            }
        }
    }
    return Rval;
}

static void LinuxStereo_SaveBin(const char *pFileName, void *pBuf, const UINT32 Size)
{
    UINT32 Ret;
    AMBA_FS_FILE *pFile;
    UINT32 OpRes;

    Ret = AmbaFS_FileOpen(pFileName, "wb", &pFile);
    if(Ret != 0U) {
        AmbaPrint_PrintUInt5("_WriteOutputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "AmbaFS_FileOpen failed", 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileWrite(pBuf, 1U, Size, pFile, &OpRes);
        if(Ret != 0U) {
            SvcLog_NG(SVC_LOG_LINUX_STEREO, "AmbaFS_FileWrite failed", 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(pFile);
        if(Ret != 0U) {
            SvcLog_NG(SVC_LOG_LINUX_STEREO, "AmbaFS_FileClose failed", 0U, 0U);
        }
    }
}

#ifdef CONFIG_AMBALINK_BOOT_OS
static UINT32 LinuxStereo_Init_Callback(UINT32 Channel, UINT32 OutType)
{
    UINT32 Rval = SVC_OK;

    (void) Channel;
    (void) OutType;
    AmbaPrint_PrintStr5("LinuxStereo_Init_Callback", NULL, NULL, NULL, NULL, NULL);

    return Rval;
}
#endif

static void LinuxStereo_DumpPicInfo(const cv_pic_info_t *pPicInfo)
{
    static UINT32 DumpTimes = 1U;
    extern void SvcCvFlow_DumpPicInfo(const char *pModuleName, const cv_pic_info_t *pPicInfo);

    if (DumpTimes > 0U) {
        SvcCvFlow_DumpPicInfo(SVC_LOG_LINUX_STEREO, pPicInfo);
        DumpTimes--;
    }
}

/**
* Pack data source to pic info structure
*/
static void LinuxStereo_PackPicInfo(UINT32 Inst, const SVC_CV_INPUT_IMG_INFO_s *pInputImg, memio_source_recv_picinfo_t *pInfo)
{
    UINT32 Rval = SVC_OK;
    cv_pic_info_t *pPicInfo = &pInfo->pic_info;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    const SVC_Linux_Stereo_CTRL_s *pCtrl = &g_LinuxStereoCtrl[Inst];
    UINT32 DataSrc, RelativeAddrOn = 1U;
    extern UINT32 SvcCvFlow_PackPicInfo_MainYuv(cv_pic_info_t *pPicInfo, const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo, UINT32 UseRelative, UINT32 LeftRight);
    extern UINT32 SvcCvFlow_PackPicInfo_Pyramid(cv_pic_info_t *pPicInfo, const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo, UINT32 MainWidth, UINT32 MainHeight, UINT32 UseRelative, UINT32 LeftRight);
    //UINT32 i;

    Rval = AmbaWrap_memset(pInfo, 0, sizeof(memio_source_recv_picinfo_t));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "memset picinfo failed", 0U, 0U);
    }

    DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
    if (pInputImg->NumInfo > 0U) {
        if (SvcCvFlow_LinuxStereo_ModeInfo[pCtrl->pCvFlow->CvModeID].ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
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
    //for (i = 0; i < MAX_HALF_OCTAVES; i++) {
    //    if (i != pCtrl->pCvFlow->InputCfg.Input[0].RoiInfo.Roi[0].Index) {
    //        pPicInfo->pyramid.half_octave[i].ctrl.disable = 1U;
    //    }
    //}

    LinuxStereo_DumpPicInfo(pPicInfo);

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "LinuxStereo_PackPicInfo error", 0U, 0U);
    }
}

/**
* Send message to Feeder to update port status
*/
static void LinuxStereo_SendReceiverReady(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort)
{
    SVC_Linux_Stereo_CTRL_s *pCtrl = &g_LinuxStereoCtrl[Inst];
    AMBA_KAL_SEMAPHORE_INFO_s SemInfo;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreQuery(&pCtrl->OutBufSem, &SemInfo)) {
        if (SemInfo.CurrentCount > 0U) {
            if (SVC_OK != SvcImgFeeder_MsgToFeeder(pPort, SVC_IMG_FEEDER_RECEIVER_READY, NULL)) {
                SvcLog_NG(SVC_LOG_LINUX_STEREO, "Send ReceiverReady[%u] error", Inst, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "ReceiverReady[%u]: SemQuery error", Inst, 0U);
    }
}

/**
* TX data source handler: process input data
*/
static UINT32 LinuxStereo_TxDataSrcHandler(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    UINT32 i, j;
    SVC_Linux_Stereo_CTRL_s *pCtrl = &g_LinuxStereoCtrl[Inst];
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
    const SVC_CV_INPUT_IMG_INFO_s *pInputImg;
    memio_source_recv_picinfo_t *pPicInfo;
    //AppAcPicInfo_t AppAcPicInfo;
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;
    AMBA_CV_FLEXIDAG_IO_s *pInBuf, *pOutBuf;
    const SVC_CV_FLOW_LINUX_STEREO_MODE_INFO_s *pModeInfo = &SvcCvFlow_LinuxStereo_ModeInfo[pCtrl->pCvFlow->CvModeID];
#ifdef CONFIG_AMBALINK_BOOT_OS
    SVC_CV_ALGO_LINUX_EX_CFG_s ExCfg;
#endif

    AmbaMisra_TouchUnused(pInfo);

    SVC_CV_DBG(SVC_LOG_LINUX_STEREO, "Tx[%u]: run %d", Inst, pCtrl->TxSeqNum);

    if (Event == SVC_IMG_FEEDER_IMAGE_DATA_READY) {
        if ((pCtrl->Enable == 1U) &&
            (0U == (pCtrl->SrcSeqNum % pCtrl->SrcRateCtrl))) {
                AmbaMisra_TypeCast(&pInputImg, &pInfo);
                AmbaMisra_TypeCast(&pPicInfo, &pCtrl->pImgInfoBuf[pCtrl->PicInfoIdx]);

                LinuxStereo_PackPicInfo(Inst, pInputImg, pPicInfo);

                /* Prepare feed config */
                Rval = AmbaWrap_memset(&FeedCfg, 0, sizeof(SVC_CV_ALGO_FEED_CFG_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_LINUX_STEREO, "memset FeedCfg failed", 0U, 0U);
                }

                for (i = 0; i < pCtrl->NumValidAlgo; i++) {
                    pAlgoOp = &pCtrl->AlgoOp[i];
                    pInBuf = &pCtrl->AlgoOpEx[i].InBuf[pCtrl->OutBufIdx];
                    pOutBuf = &pCtrl->AlgoOpEx[i].OutBuf[pCtrl->OutBufIdx];

                    FeedCfg.pIn  = pInBuf;
                    FeedCfg.pOut = pOutBuf;
                    pInBuf->num_of_buf = 1U;
                    SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_LINUX_STEREO, pPicInfo, pCtrl->ImgInfoBufSize, &pInBuf->buf[0]);
                    pOutBuf->num_of_buf = pAlgoOp->OutputNum;
                    for (j = 0U; j < pAlgoOp->OutputNum; j++) {
                        Rval = AmbaWrap_memcpy(&pOutBuf->buf[j], &pCtrl->AlgoOpEx[i].OutputBuf[pCtrl->OutBufIdx][j], sizeof(flexidag_memblk_t));
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_LINUX_STEREO, "memcpy OutputBufBlk failed", 0U, 0U);
                        }
                    }

                    if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
#ifdef CONFIG_AMBALINK_BOOT_OS
                        const SVC_CV_ALGO_LINUX_EX_CFG_s *pExCfg = &ExCfg;

                        ExCfg.AmbaIPCChannel = pCtrl->FlexiIOChan;
                        ExCfg.ValidDataSize  = sizeof(memio_source_recv_picinfo_t);
                        AmbaMisra_TypeCast(&FeedCfg.pExtFeedCfg, &pExCfg);
#else
                        SvcLog_NG(SVC_LOG_LINUX_STEREO, "CONFIG_AMBALINK_BOOT_OS not defined ", 0U, 0U);
#endif
                    } else {
                        AmbaMisra_TypeCast(&FeedCfg.pExtFeedCfg, &pPicInfo); /* SCA_OpenOD */
                        AmbaMisra_TypeCast(&FeedCfg.pUserData, &pPicInfo);
                    }

                    Rval |= SvcCvAlgo_Feed(&pAlgoOp->AlgoHandle, &FeedCfg);
                    SVC_CV_DBG(SVC_LOG_LINUX_STEREO, "Tx[%u]: Send(%u)", Inst, pPicInfo->pic_info.frame_num);
                }

                if (SVC_OK == Rval) {
                    pCtrl->TxSeqNum++;
                    pCtrl->PicInfoIdx++;

                    if (pCtrl->PicInfoIdx >= SVC_LINUX_STEREO_PICINFO_QUEUE_SIZE) {
                        pCtrl->PicInfoIdx = 0;
                    }
                    pCtrl->OutBufIdx++;
                    if (pCtrl->OutBufIdx >= SVC_LINUX_STEREO_OUTPUT_DEPTH) {
                        pCtrl->OutBufIdx = 0;
                    }
                } else {
                    SvcLog_NG(SVC_LOG_LINUX_STEREO, "Tx[%u]: send error", Inst, 0U);
                }

                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    LinuxStereo_SendReceiverReady(Inst, pPort);
                }
        } else {
            if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                LinuxStereo_SendReceiverReady(Inst, pPort);
                Rval = AmbaKAL_TaskSleep(1);
            }
        }

        pCtrl->SrcSeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "Tx[%u]: unsupported event(0x%x)", Inst, Event);
        Rval = SVC_NG;
    }

    SVC_CV_DBG(SVC_LOG_LINUX_STEREO, "Tx[%u]: end", Inst, 0U);

    return Rval;
}

static UINT32 LinuxStereo_TxDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_Linux_Stereo_CTRL_s *pCtrl = &g_LinuxStereoCtrl[SVC_LINUX_STEREO_INSTANCE0];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_LINUX_STEREO_INSTANCE0, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 LinuxStereo_TxDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_Linux_Stereo_CTRL_s *pCtrl = &g_LinuxStereoCtrl[SVC_LINUX_STEREO_INSTANCE1];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_LINUX_STEREO_INSTANCE1, pPort, Event, pInfo);
    }
    return Rval;
}


static void LinuxStereo_SendACData(UINT32 Inst, const AMBA_CV_MVAC_DATA_s *pAcResult)
{
    UINT32 Rval;
    SVC_Linux_Stereo_CTRL_s *pCtrl = &g_LinuxStereoCtrl[Inst];
    Linux_Stereo_OUTPUT_MSG_s Msg;

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
            SvcLog_NG(SVC_LOG_LINUX_STEREO, "memcpy Warp info failed", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "memcpy MVAC Data failed", 0U, 0U);
    }

    Msg.Event = SVC_STEREO_AC_FINISH;
    AmbaMisra_TypeCast(&Msg.pAlgoOut, &pCtrl->pMVACOutBuf[pCtrl->MVACOutBufIdx]);
    AmbaMisra_TypeCast(&Msg.pAlgoOut->pWarpInfo, &pCtrl->pWarpOutBuf[pCtrl->MVACOutBufIdx]);

    Rval = AmbaKAL_MsgQueueSend(&pCtrl->AlgoOutQueMVACId, &Msg, 5000);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "Linux_Stereo_AlgoOutCallback[%u]: MsgQueueSend error", Inst, 0U);
    }

    pCtrl->MVACOutBufIdx++;
    if (pCtrl->MVACOutBufIdx >= SVC_LINUX_STEREO_OUTPUT_DEPTH) {
        pCtrl->MVACOutBufIdx = 0;
    }
}
/**
* Algorithm output event callback function
*/
static UINT32 Linux_Stereo_AlgoOutCallback(UINT32 Inst, UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Rval = SVC_OK;
    SVC_Linux_Stereo_CTRL_s *pCtrl = &g_LinuxStereoCtrl[Inst];
    SVC_CV_MVAC_DATA_s *pSvcAcResult;
    AMBA_CV_MVAC_DATA_s AcResult;
    AMBA_CV_MVAC_DATA_s *pAcResult = &AcResult;
    //UINT16 *pWarpTbl;
    static UINT8 PrintCount = 0U;
    static UINT8 SvcCvDebugDump = 1;
    static SVC_CV_ALGO_OUTPUT_s AlgoOut;
    const UINT32 *pType;
    UINT32 Valid = 1U;
    static STEREO_STIXEL_OUTPUT_MSG_s StixelMsg;
    static STEREO_FUSION_OUTPUT_MSG_s FusionMsg;

    AmbaMisra_TouchUnused(&Inst);
    AmbaMisra_TouchUnused(&Event);


    if (Event == CALLBACK_EVENT_OUTPUT) {
    } else {
        Valid = 0U;
    }

    if (Valid == 1U) {
        Rval = AmbaWrap_memcpy(&AlgoOut, pEventData, sizeof(SVC_CV_ALGO_OUTPUT_s));
        AmbaMisra_TypeCast(&pType, &AlgoOut.pOutput->buf[0].pBuffer);

        if (*pType == SVC_CV_STEREO_AUTO_CALIB) {
            SVC_CV_DBG(SVC_LOG_LINUX_STEREO, "Rx[%u]:Receive MVAC callback %d", Inst, pCtrl->RxMvacSeqNum);

            AmbaMisra_TypeCast(&pSvcAcResult, &AlgoOut.pOutput->buf[0].pBuffer);
            pAcResult->State = pSvcAcResult->State;
            pAcResult->NumRun = pSvcAcResult->NumRun;
            pAcResult->NumWarpInfoSent= pSvcAcResult->NumWarpInfoSent;
            Rval = AmbaWrap_memcpy(&pAcResult->ViewStatistic , &pSvcAcResult->ViewStatistic, sizeof(AMBA_CV_MVAC_STATISTICS_s));

            if (Rval == SVC_OK) {
                if (pSvcAcResult->WarpValid == 1U) {
                    pAcResult->pWarpInfo = &pSvcAcResult->WarpInfo;
                } else {
                    pAcResult->pWarpInfo = NULL;
                }
                switch (pAcResult->State) {
                    case AMBA_CV_MVAC_STATE_RESET:
                        SvcLog_OK(SVC_LOG_LINUX_STEREO, "## state: RESET", 0U, 0U);
                        PrintCount = 0U;
                        break;
                    case AMBA_CV_MVAC_STATE_WAIT:
                        SvcLog_OK(SVC_LOG_LINUX_STEREO, "## state: WAIT", 0U, 0U);
                        break;
                    case AMBA_CV_MVAC_STATE_ACCUMULATION:
                        if (PrintCount < 5U) {
                            SvcLog_OK(SVC_LOG_LINUX_STEREO, "## state: ACCUMULATION", 0U, 0U);
                        }
                        PrintCount++;
                        break;
                    case AMBA_CV_MVAC_STATE_SOLVE:
                        SvcLog_OK(SVC_LOG_LINUX_STEREO, "## state: SOLVE", 0U, 0U);
                        if (pAcResult->pWarpInfo != NULL) {
                            SvcLog_DBG(SVC_LOG_LINUX_STEREO, "generate warp table at MVAC_STATE_SOLVE", 0U, 0U);
                            LinuxStereo_SendACData(Inst, pAcResult);
                            if (SvcCvDebugDump == 1U) {
                                if (pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTableValid == 1U) {
                                    LinuxStereo_SaveBin("c:\\ac_stixel_warp_left.bin",
                                        &(pSvcAcResult->WarpInfo.CalibrationUpdates[0].WarpTable[0]),
                                        sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable));
                                }
                            }
                            //RefCV_AmbaAc_ProfilerReport();
                        }
                        break;
                    case AMBA_CV_MVAC_STATE_END:
                        SvcLog_OK(SVC_LOG_LINUX_STEREO, "## state: END", 0U, 0U);
                        if (pAcResult->pWarpInfo != NULL ) {
                            SvcLog_DBG(SVC_LOG_LINUX_STEREO, "generate warp table at MVAC_STATE_END", 0U, 0U);
                            LinuxStereo_SendACData(Inst, pAcResult);
                            if (SvcCvDebugDump == 1U) {
                                if (pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTableValid == 1U) {
                                    LinuxStereo_SaveBin("c:\\ac_stixel_warp_left.bin",
                                        &(pSvcAcResult->WarpInfo.CalibrationUpdates[0].WarpTable[0]),
                                        sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable));
                                }
                            }
                            //RefCV_AmbaAc_ProfilerReport();
                        }
                        break;
                    case AMBA_CV_MVAC_STATE_SKIP:
                        SvcLog_OK(SVC_LOG_LINUX_STEREO, "## state: SKIP", 0U, 0U);
                        break;
                    default:
                        SvcLog_NG(SVC_LOG_LINUX_STEREO, "## callback at state: ERROR", 0U, 0U);
                        break;
                }
            } else {
                SvcLog_NG(SVC_LOG_LINUX_STEREO, "AmbaWrap_memcpy fail", 0U, 0U);
            }
            pCtrl->RxMvacSeqNum++;
        } else if (*pType == SVC_CV_STIXEL_OUTPUT) {
            SVC_CV_DBG(SVC_LOG_LINUX_STEREO, "Rx[%u]:Receive Stixel callback %d", Inst, pCtrl->RxStixelSeqNum);

            Rval = AmbaWrap_memcpy(&StixelMsg.AlgoOut , &AlgoOut, sizeof(SVC_CV_ALGO_OUTPUT_s));

            if (Rval == SVC_OK) {
                StixelMsg.Event = SVC_STEREO_STIXEL_FINISH;

                Rval = AmbaKAL_MsgQueueSend(&pCtrl->AlgoOutQueStixelId, &StixelMsg, 5000);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_LINUX_STEREO, "Linux_Stereo_AlgoOutCallback[%u]: MsgQueueSend error", Inst, 0U);
                }
            }
            pCtrl->RxStixelSeqNum++;
        }  else if (*pType == SVC_CV_STEREO_OUTPUT) {
            SVC_CV_DBG(SVC_LOG_LINUX_STEREO, "Rx[%u]:Receive Fusion callback %d", Inst, pCtrl->RxFusionSeqNum);

            Rval = AmbaWrap_memcpy(&FusionMsg.AlgoOut , &AlgoOut, sizeof(SVC_CV_ALGO_OUTPUT_s));

            if (Rval == SVC_OK) {
                FusionMsg.Event = SVC_STEREO_FUSION_FINISH;

                Rval = AmbaKAL_MsgQueueSend(&pCtrl->AlgoOutQueFusionId, &FusionMsg, 5000);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_LINUX_STEREO, "Linux_Stereo_AlgoOutCallback[%u]: MsgQueueSend error", Inst, 0U);
                }
            }
            pCtrl->RxFusionSeqNum++;
        } else {
            SvcLog_NG(SVC_LOG_LINUX_STEREO, "UnSupport Type %d", *pType, 0U);
        }
    }



    //    Rval = Linux_Stereo_SemGive(Inst);
    //    if (SVC_OK != Rval) {
    //        SvcLog_NG(SVC_LOG_LINUX_STEREO, "Linux_Stereo_AlgoOutCallback[%u]: SemGive error", Inst, 0U);
    //    }

    if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
        LinuxStereo_SendReceiverReady(Inst, &pCtrl->FeederPort);
    }

    return Rval;
}

static UINT32 LinuxStereo_AlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Linux_Stereo_AlgoOutCallback(SVC_LINUX_STEREO_INSTANCE0, Event, pEventData);
}

static UINT32 LinuxStereo_AlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Linux_Stereo_AlgoOutCallback(SVC_LINUX_STEREO_INSTANCE1, Event, pEventData);
}


/**
* RX data sink handler: process detection output
*/
static UINT32 LinuxStereo_MvacRxDataSinkHandler(UINT32 Inst)
{
    UINT32 Rval;
    SVC_Linux_Stereo_CTRL_s *pCtrl = &g_LinuxStereoCtrl[Inst];
    Linux_Stereo_OUTPUT_MSG_s Msg;
    SVC_CV_DATA_OUTPUT_s Output;

    Rval = AmbaKAL_MsgQueueReceive(&pCtrl->AlgoOutQueMVACId, &Msg, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK == Rval) {
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
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "ACRx[%u]: recv error", Inst, 0U);
    }

    return Rval;
}

/**
* RX data sink handler: process detection output
*/
static UINT32 LinuxStereo_StixelRxDataSinkHandler(UINT32 Inst)
{
    UINT32 Rval;
    SVC_Linux_Stereo_CTRL_s *pCtrl = &g_LinuxStereoCtrl[Inst];
    static STEREO_STIXEL_OUTPUT_MSG_s StixelMsg;
    static SVC_CV_STIXEL_DATA_s *pSvcStixelOut;
    static SVC_CV_FLOW_STIXEL_OUTPUT_s StixelOutput[SVC_LINUX_STEREO_MAX_INSTANCE][SVC_LINUX_STEREO_OUTPUT_DEPTH] GNU_SECTION_NOZEROINIT;
    SVC_CV_FLOW_STIXEL_OUTPUT_s *pStixelOut;
    static UINT32 *pFrameNum;

    Rval = AmbaKAL_MsgQueueReceive(&pCtrl->AlgoOutQueStixelId, &StixelMsg, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK == Rval) {
        if (StixelMsg.Event == SVC_STEREO_STIXEL_FINISH) {

            AmbaMisra_TypeCast(&pSvcStixelOut, &StixelMsg.AlgoOut.pOutput->buf[0].pBuffer);
            pStixelOut = &StixelOutput[Inst][pCtrl->StixelOutBufIdx];

            pStixelOut->MsgCode = SVC_CV_STIXEL_OUTPUT;
            Rval = AmbaWrap_memcpy(&pStixelOut->StixelOut, &pSvcStixelOut->StixelInfo, sizeof(AMBA_CV_STIXEL_DET_OUT_s));
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_LINUX_STEREO, "AmbaWrap_memcpy fail", 0U, 0U);
            }

#ifdef CONFIG_AMBALINK_BOOT_OS
            if (NULL != StixelMsg.AlgoOut.pUserData) {
                AmbaMisra_TypeCast(&pFrameNum, &StixelMsg.AlgoOut.pUserData);
                pStixelOut->FrameNum = *pFrameNum;
            }
#endif

            Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                       SVC_CV_FLOW_OUTPUT_PORT0,
                                       pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                       pStixelOut);
            if (SVC_OK == Rval) {
                pCtrl->StixelOutBufIdx++;
                if (pCtrl->StixelOutBufIdx >= SVC_LINUX_STEREO_OUTPUT_DEPTH) {
                   pCtrl->StixelOutBufIdx = 0;
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "StixelRx[%u]: recv error", Inst, 0U);
    }

    return SVC_OK;
}

/**
* RX data sink handler: process detection output
*/
static UINT32 LinuxStereo_FusionRxDataSinkHandler(UINT32 Inst)
{
    UINT32 Rval;
    SVC_Linux_Stereo_CTRL_s *pCtrl = &g_LinuxStereoCtrl[Inst];
    static STEREO_FUSION_OUTPUT_MSG_s FusionMsg;
    SVC_CV_SPU_BUF_s *pSvcSpuOut;
    static SVC_CV_STEREO_OUTPUT_s FusionOutput[SVC_LINUX_STEREO_MAX_INSTANCE][SVC_LINUX_STEREO_OUTPUT_DEPTH] GNU_SECTION_NOZEROINIT;
    static UINT32 *pFrameNum;
    ULONG ResultAddr;
    SVC_CV_STEREO_OUTPUT_s *pFsuionOutout;
    AMBA_CV_SPU_DATA_s *pSpuData;

    Rval = AmbaKAL_MsgQueueReceive(&pCtrl->AlgoOutQueFusionId, &FusionMsg, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK == Rval) {
        if (FusionMsg.Event == SVC_STEREO_FUSION_FINISH) {
            AmbaMisra_TypeCast(&pSvcSpuOut, &FusionMsg.AlgoOut.pOutput->buf[0].pBuffer);
            pSpuData = &pSvcSpuOut->SpuData;

            AmbaMisra_TypeCast(&ResultAddr, &pSpuData);
            pFsuionOutout = &FusionOutput[Inst][pCtrl->FusionOutBufIdx];
            pFsuionOutout->MsgCode = SVC_CV_STEREO_OUTPUT;
            pFsuionOutout->Type = STEREO_OUT_TYPE_FUSION;
            pFsuionOutout->StereoID = 0;
            pFsuionOutout->FrameSetID = 0;
            pFsuionOutout->DataAddr = ResultAddr; /* AMBA_CV_SPU_BUF_s */
            pFsuionOutout->pPicInfo = &pSvcSpuOut->PicInfo;

#ifdef CONFIG_AMBALINK_BOOT_OS
            if (NULL != FusionMsg.AlgoOut.pUserData) {
                AmbaMisra_TypeCast(&pFrameNum, &FusionMsg.AlgoOut.pUserData);
                pFsuionOutout->FrameNum = *pFrameNum;
            }
#endif

            Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                        SVC_CV_FLOW_OUTPUT_PORT0,
                                        pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                        pFsuionOutout);

            if (SVC_OK == Rval) {
                pCtrl->FusionOutBufIdx++;
                if (pCtrl->FusionOutBufIdx >= SVC_LINUX_STEREO_OUTPUT_DEPTH) {
                    pCtrl->FusionOutBufIdx = 0;
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "StixelRx[%u]: recv error", Inst, 0U);
    }

    return SVC_OK;
}


/**
* Task entry of RX process
*/
static void* LinuxStereo_MvacRxTaskEntry(void* EntryArg)
{
    const ULONG  *pArg;
    UINT32 Rval = SVC_OK, Inst;
    const SVC_Linux_Stereo_CTRL_s *pCtrl;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Inst = (UINT32)(*pArg);
    pCtrl = &g_LinuxStereoCtrl[Inst];

    SvcLog_DBG(SVC_LOG_LINUX_STEREO, "MVACRxTaskEntry[%d] start", Inst, 0U);

    while (SVC_OK == Rval) {
        if (NULL != pCtrl->pMvacRxProc) {
            Rval = pCtrl->pMvacRxProc(Inst);
        } else {
            Rval = SVC_NG;
        }
    }

    SvcLog_DBG(SVC_LOG_LINUX_STEREO, "MVACRxTaskEntry[%d] exit", Inst, 0U);

    return NULL;
}

/**
* Task entry of RX process
*/
static void* LinuxStereo_StixelRxTaskEntry(void* EntryArg)
{
    const ULONG  *pArg;
    UINT32 Rval = SVC_OK, Inst;
    const SVC_Linux_Stereo_CTRL_s *pCtrl;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Inst = (UINT32)(*pArg);
    pCtrl = &g_LinuxStereoCtrl[Inst];

    SvcLog_DBG(SVC_LOG_LINUX_STEREO, "StixelRxTaskEntry[%d] start", Inst, 0U);

    while (SVC_OK == Rval) {
        if (NULL != pCtrl->pStixelRxProc) {
            Rval = pCtrl->pStixelRxProc(Inst);
        } else {
            Rval = SVC_NG;
        }
    }

    SvcLog_DBG(SVC_LOG_LINUX_STEREO, "StixelRxTaskEntry[%d] exit", Inst, 0U);

    return NULL;
}

/**
* Task entry of RX process
*/
static void* LinuxStereo_FusionRxTaskEntry(void* EntryArg)
{
    const ULONG  *pArg;
    UINT32 Rval = SVC_OK, Inst;
    const SVC_Linux_Stereo_CTRL_s *pCtrl;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Inst = (UINT32)(*pArg);
    pCtrl = &g_LinuxStereoCtrl[Inst];

    SvcLog_DBG(SVC_LOG_LINUX_STEREO, "FusionRxTaskEntry[%d] start", Inst, 0U);

    while (SVC_OK == Rval) {
        if (NULL != pCtrl->pFusionRxProc) {
            Rval = pCtrl->pFusionRxProc(Inst);
        } else {
            Rval = SVC_NG;
        }
    }

    SvcLog_DBG(SVC_LOG_LINUX_STEREO, "FusionRxTaskEntry[%d] exit", Inst, 0U);

    return NULL;
}


/**
* Set and wait TX and RX task to idle state
*/
static UINT32 LinuxStereo_SetTaskIdle(UINT32 Inst)
{
    UINT32 Rval;
    SVC_Linux_Stereo_CTRL_s *pCtrl = &g_LinuxStereoCtrl[Inst];
    UINT32 i;

    /* Disable input and output */
    pCtrl->Enable = 0U;

    if (SVC_OK != SvcImgFeeder_Disconnect(&pCtrl->FeederPort)) {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "Disconn feeder port error", 0U, 0U);
    }

    if (SVC_OK != SvcImgFeeder_ClosePort(&pCtrl->FeederPort)) {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "Close feeder port error", 0U, 0U);
    }

    /* Wait Rx idle */
    for (i = 0; i < SVC_LINUX_STEREO_SEM_INIT_COUNT; i++) {
        //Rval = Linux_Stereo_SemTake(Inst);
        Rval = 0;
    }

    return Rval;
}

/**
* Send external YUV to driver
*/
static UINT32 LinuxStereo_SendExtYUV(UINT32 Inst, const void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_Linux_Stereo_CTRL_s *pCtrl = &g_LinuxStereoCtrl[Inst];
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
static void LinuxStereo_SrcRateUpdate(SVC_Linux_Stereo_CTRL_s *pCtrl, UINT32 SrcRate)
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
            (Den <= Linux_Stereo_SRC_RATE_MAX_DENOMINATOR) && (Num <= Den)) {
            Rval = AmbaWrap_memset(&pCtrl->SrcValidTbl[0], 0, sizeof(pCtrl->SrcValidTbl));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_LINUX_STEREO, "memset SrcValidTbl failed", 0U, 0U);
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
            SvcLog_NG(SVC_LOG_LINUX_STEREO, "Invalid ratio: %u/%u", Num, Den);
        }
    } else {
        /* Divisor */
        if (SrcRate == 0U) {
            pCtrl->SrcRateCtrl = 1U;
        } else {
            pCtrl->SrcRateCtrl = SrcRate;
        }
    }

    SvcLog_OK(SVC_LOG_LINUX_STEREO, "SrcRateCtrl: 0x%x", pCtrl->SrcRateCtrl, 0U);
}
#if 1
/**
* Send meta data to driver
*/
static UINT32 Linux_Stereo_SetMetaData(UINT32 Inst, const void *pInfo)
{
    UINT32 Rval = SVC_OK;

    AmbaMisra_TouchUnused(&Inst);
    AmbaMisra_TouchUnused(&pInfo);

    return Rval;
}
#endif

/**
* Entry of configuration function
*/
static UINT32 LinuxStereo_Config(UINT32 Inst, AmbaAcConfig_t *pAcConfig)
{
    UINT32 Rval;
    const SVC_Linux_Stereo_CTRL_s *pCtrl = &g_LinuxStereoCtrl[Inst];
    const SVC_CV_ROI_INFO_s *pRoiInfo = &pCtrl->pCvFlow->InputCfg.Input[0].RoiInfo;
    extern AmbaAcConfig_t LinuxStereo_AcConfigDefult;

    Rval = AmbaWrap_memcpy(pAcConfig, &LinuxStereo_AcConfigDefult, sizeof(AmbaAcConfig_t));
    if (SVC_OK == Rval) {
        if (pRoiInfo->NumRoi > 0U) {
            pAcConfig->MvacViewInfo.ScaleId = (UINT32)pRoiInfo->Roi[0].Index;
            pAcConfig->VoCfg.View.Scale = (UINT8)pRoiInfo->Roi[0].Index;
            pAcConfig->VoCfg.View.RoiX = (UINT16)pRoiInfo->Roi[0].StartX;
            pAcConfig->VoCfg.View.RoiY = (UINT16)pRoiInfo->Roi[0].StartY;
            pAcConfig->VoCfg.View.RoiWidth = (UINT16)pRoiInfo->Roi[0].Width;
            pAcConfig->VoCfg.View.RoiHeight = (UINT16)pRoiInfo->Roi[0].Height;
        } else {
            SvcLog_DBG(SVC_LOG_LINUX_STEREO, "Use default AC config. Scale = %d", LinuxStereo_AcConfigDefult.MvacViewInfo.ScaleId, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "memcpy AcConfig failed", 0U, 0U);
    }

    return Rval;
}

static UINT32 LinuxStereo_ConfigBuffer(UINT32 Inst)
{
    UINT32 Rval = SVC_OK;
    SVC_Linux_Stereo_CTRL_s *pCtrl = &g_LinuxStereoCtrl[Inst];
    SVC_CV_FLOW_BUF_INFO_s BufInfo;
    UINT32 i;

    /* Img info buffer */
    if (pCtrl->pImgInfoBuf[0] == NULL) {
        for (i = 0; i < SVC_LINUX_STEREO_PICINFO_QUEUE_SIZE; i++) {
            Rval = SvcCvFlow_AllocWorkBuf(sizeof(memio_source_recv_picinfo_t), &BufInfo);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pCtrl->pImgInfoBuf[i], &BufInfo.pAddr);
                pCtrl->ImgInfoBufSize = BufInfo.Size;
            } else {
                SvcLog_NG(SVC_LOG_LINUX_STEREO, "Alloc Img info buf failed", 0U, 0U);
                Rval = SVC_NG;
                break;
            }
        }
    }

    /* MVAC output buffer */
    if (pCtrl->pMVACOutBuf[0] == NULL) {
        for (i = 0; i < SVC_LINUX_STEREO_OUTPUT_DEPTH; i++) {
            Rval = SvcCvFlow_AllocWorkBuf(sizeof(AMBA_CV_MVAC_DATA_s), &BufInfo);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pCtrl->pMVACOutBuf[i], &BufInfo.pAddr);
                pCtrl->MVACOutBufSize = BufInfo.Size;
            } else {
                SvcLog_NG(SVC_LOG_LINUX_STEREO, "Alloc Warp buf failed", 0U, 0U);
                Rval = SVC_NG;
                break;
            }
        }
    }

    /* Warp output buffer */
    if (pCtrl->pWarpOutBuf[0] == NULL) {
        for (i = 0; i < SVC_LINUX_STEREO_OUTPUT_DEPTH; i++) {
            Rval = SvcCvFlow_AllocWorkBuf(sizeof(AMBA_CV_MVAC_WARP_INFO_s), &BufInfo);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pCtrl->pWarpOutBuf[i], &BufInfo.pAddr);
                pCtrl->WarpOutBufSize = BufInfo.Size;
            } else {
                SvcLog_NG(SVC_LOG_LINUX_STEREO, "Alloc Warp buf failed", 0U, 0U);
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
static UINT32 CvFlow_LinuxStereoInit(UINT32 CvFlowChan, void *pInit)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    static UINT32 g_Linux_StereoInit = 0U;

    (void) CvFlowChan;
    (void) pInit;

    if (0U == g_Linux_StereoInit) {
        g_Linux_StereoInit = 1U;

        if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&Linux_StereoMutex, NULL)) {
            SvcLog_NG(SVC_LOG_LINUX_STEREO, "MutexCreate error", 0U, 0U);
            Rval = SVC_NG;
        }
        for (i = 0; i < SVC_LINUX_STEREO_MAX_INSTANCE; i++) {
            g_LinuxStereoCtrl[i].Used = 0;
        }

        SVC_CV_DBG(SVC_LOG_LINUX_STEREO, "CvFlow_Linux_StereoInit: first init", 0U, 0U);
    }

    SvcLog_OK(SVC_LOG_LINUX_STEREO, "CvFlow_Linux_StereoInit: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver configuration function
*/
static UINT32 CvFlow_LinuxStereoConfig(UINT32 CvFlowChan, void *pConfig)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    SVC_Linux_Stereo_CTRL_s *pCtrl = NULL;
    const SVC_CV_FLOW_LINUX_STEREO_MODE_INFO_s *pModeInfo;

    (void) pConfig;

    if (NULL != pConfig) {
        LinuxStereo_MutexTake();
        /* Create instance */
        for (i = 0; i < SVC_LINUX_STEREO_MAX_INSTANCE; i++) {
            if (0U == g_LinuxStereoCtrl[i].Used) {
                Rval = AmbaWrap_memset(&g_LinuxStereoCtrl[i], 0, sizeof(SVC_Linux_Stereo_CTRL_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_LINUX_STEREO, "memset g_LinuxStereoCtrl[%u] failed", i, 0U);
                }
                pCtrl = &g_LinuxStereoCtrl[i];
                pCtrl->Used = 1;
                SvcLog_DBG(SVC_LOG_LINUX_STEREO, "Create Inst(%u)", i, 0U);
                break;
            }
        }
        LinuxStereo_MutexGive();

        /* Configure instance */
        if (NULL != pCtrl) {
            pCtrl->CvfChan     = (UINT8) CvFlowChan;
            pCtrl->pCvFlow     = &pCtrl->CvCfg;
            Rval = AmbaWrap_memcpy(&pCtrl->CvCfg, pConfig, sizeof(SVC_CV_FLOW_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_LINUX_STEREO, "memcpy CvCfg failed", 0U, 0U);
            }
            LinuxStereo_SrcRateUpdate(pCtrl, pCtrl->pCvFlow->InputCfg.SrcRate);
            pCtrl->Enable = 1;

            pModeInfo = &SvcCvFlow_LinuxStereo_ModeInfo[pCtrl->pCvFlow->CvModeID];
            if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG) {
                Rval = SVC_OK;
            } else if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
#ifdef CONFIG_AMBALINK_BOOT_OS
                /* Init FlexiDAG IO framework */
                if (pCtrl->pCvFlow->PrivFlag < SVC_CV_FLEXIDAG_IO_MAX_CHANNEL) {
                    pCtrl->FlexiIOChan = pCtrl->pCvFlow->PrivFlag;
                    Rval = SvcCv_FlexiIORegister(pCtrl->FlexiIOChan, LinuxStereo_Init_Callback);
                    if (Rval != SVC_OK) {
                        SvcLog_NG(SVC_LOG_LINUX_STEREO, "SvcCv_FlexiIORegister(%u) failed", pCtrl->FlexiIOChan, 0U);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_LINUX_STEREO, "Invalid FlexidagIO chan(%u)", pCtrl->pCvFlow->PrivFlag, 0U);
                }
#else
                Rval = SVC_NG;
#endif
            } else {
                SvcLog_NG(SVC_LOG_LINUX_STEREO, "unsupported CVFlow ProfID(%u)", pModeInfo->ProfID, 0U);
                Rval = SVC_NG;
            }
        } else {
            Rval = SVC_NG;
        }
    } else {
        Rval = SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_LINUX_STEREO, "CvFlow_Linux_StereoConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "CvFlow_Linux_StereoConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver load function
*/
static UINT32 CvFlow_LinuxStereoLoad(UINT32 CvFlowChan, void *pInfo)
{
    UINT32 Rval;
    UINT32 Inst, i;
    SVC_Linux_Stereo_CTRL_s *pCtrl;
    const SVC_CV_FLOW_LOAD_INFO_s *pLoadInfo;
    //const char *pFileName;
    //SVC_CV_FLOW_BUF_INFO_s BufInfo;
    const SVC_CV_FLOW_LINUX_STEREO_MODE_INFO_s *pModeInfo;

    (void) pInfo;

    Rval = LinuxStereo_CvfChan2Inst(CvFlowChan, &Inst);
    if (Rval == SVC_OK) {
        pCtrl = &g_LinuxStereoCtrl[Inst];
        if (NULL != pInfo) {
            AmbaMisra_TypeCast(&pLoadInfo, &pInfo);
            if (pLoadInfo->NumBin > 0U) {
                /* Use external path */
                if (pLoadInfo->AlgoIdx < SVC_LINUX_STEREO_MAX_ALGO_NUM) {
                    Rval = SvcCvFlow_LoadAlgoBin(SVC_LOG_LINUX_STEREO, pLoadInfo, &pCtrl->AlgoOp[pLoadInfo->AlgoIdx]);
                } else {
                    SvcLog_NG(SVC_LOG_LINUX_STEREO, "CvFlow_StereoLoad: CvFlowChan(%d) invalid AlgoIdx(%u)", CvFlowChan, pLoadInfo->AlgoIdx);
                }
            } else {
                /* Use ModeInfo */
                if (pCtrl->pCvFlow->CvModeID < LINUX_STEREO_NUM_MODE) {
                    pModeInfo = &SvcCvFlow_LinuxStereo_ModeInfo[pCtrl->pCvFlow->CvModeID];

                    for (i = 0; i < pModeInfo->AlgoGrp.NumAlgo; i++) {
                        Rval |= SvcCvFlow_LoadAlgo(SVC_LOG_LINUX_STEREO, &pModeInfo->AlgoGrp.AlgoInfo[i], &pCtrl->AlgoOp[i]);
                    }
                } else  {
                    SvcLog_NG(SVC_LOG_LINUX_STEREO, "CvFlow_StereoLoad: CvFlowChan(%d) invalid CvModeID(%u)", CvFlowChan, pCtrl->pCvFlow->CvModeID);
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "CvFlow_Linux_StereoLoad: invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "CvFlow_Linux_StereoLoad: CvFlowChan(%d) error", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver start function
*/
static UINT32 CvFlow_LinuxStereoStart(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst, i, j, AlgoIdx, Num;
    SVC_Linux_Stereo_CTRL_s *pCtrl;
    const SVC_CV_FLOW_LINUX_STEREO_MODE_INFO_s *pModeInfo;
    SVC_CV_FLOW_BUF_INFO_s BufInfo;
    const SVC_CV_FLOW_ALGO_INFO_s *pAlgoInfo;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
    SVC_CV_FLOW_ALGO_QUERY_CFG_s QueryCfg;
    SVC_CV_FLOW_ALGO_CREATE_CFG_s CreateCfg;
    SVC_CV_ALGO_REGCB_CFG_s CbCfg;
    SVC_IMG_FEEDER_PORT_CFG_s PortCfg = {0};
    static char CbMVACQueName[] = "MVACEvtQue";
    static char CbStixelEvtQueName[] = "StixelEvtQue";
    static char CbFusionEvtQueName[] = "FusionEvtQue";
    static char SemName[] = "OutBufSem";
    static Linux_Stereo_OUTPUT_MSG_s AlgoOutQue[SVC_LINUX_STEREO_MAX_INSTANCE][SVC_LINUX_STEREO_ALGO_OUT_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static STEREO_STIXEL_OUTPUT_MSG_s AlgoStixelOutQue[SVC_LINUX_STEREO_MAX_INSTANCE][SVC_LINUX_STEREO_ALGO_OUT_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static STEREO_FUSION_OUTPUT_MSG_s AlgoFusionOutQue[SVC_LINUX_STEREO_MAX_INSTANCE][SVC_LINUX_STEREO_ALGO_OUT_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 Linux_StereoStixelRxTaskStack[SVC_LINUX_STEREO_MAX_INSTANCE][SVC_LINUX_STEREO_RX_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 Linux_StereoACRxTaskStack[SVC_LINUX_STEREO_MAX_INSTANCE][SVC_LINUX_STEREO_RX_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 Linux_StereoFusionRxTaskStack[SVC_LINUX_STEREO_MAX_INSTANCE][SVC_LINUX_STEREO_RX_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_IMG_FEEDER_EVENT_CALLBACK_f LinuxStereo_TxDataSrcHandlerList[SVC_LINUX_STEREO_MAX_INSTANCE] = {
        LinuxStereo_TxDataSrcHandler0,
        LinuxStereo_TxDataSrcHandler1,
    };
    //static AcGetResultCb_t Linux_Stereo_AlgoOutCbList[SVC_LINUX_STEREO_MAX_INSTANCE] = {
    static SVC_CV_ALGO_CALLBACK_f Linux_Stereo_AlgoOutCbList[SVC_LINUX_STEREO_MAX_INSTANCE] = {
        LinuxStereo_AlgoOutCallback0,
        LinuxStereo_AlgoOutCallback1,
    };
    AmbaAcConfig_t AcConfig;
#ifdef CONFIG_AMBALINK_BOOT_OS
    SVC_CV_ALGO_LINUX_EX_CFG_s ExCfg;
#endif

    (void) pInput;

    Rval = LinuxStereo_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl = &g_LinuxStereoCtrl[Inst];
        pModeInfo = &SvcCvFlow_LinuxStereo_ModeInfo[pCtrl->pCvFlow->CvModeID];

        /* Open Feeder port first to enable frame FIFO earlier */
        if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
            /* The driver supports two inputs per instance */
            PortCfg.NumPath            = 2U;
            PortCfg.Content[0].DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
            PortCfg.Content[0].StrmId  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
            PortCfg.Content[1].DataSrc = pCtrl->pCvFlow->InputCfg.Input[1].DataSrc;
            PortCfg.Content[1].StrmId  = pCtrl->pCvFlow->InputCfg.Input[1].StrmId;
            PortCfg.SendFunc           = LinuxStereo_TxDataSrcHandlerList[Inst];
            if (SVC_OK != SvcImgFeeder_OpenPort(pCtrl->pCvFlow->InputCfg.FeederID, &PortCfg, &pCtrl->FeederPort)) {
                SvcLog_NG(SVC_LOG_LINUX_STEREO, "Open feeder port error", 0U, 0U);
            }
        } else {
            SvcLog_DBG(SVC_LOG_LINUX_STEREO, "ImgFeeder is not enabled", 0U, 0U);
        }


        for (AlgoIdx = 0; AlgoIdx < pModeInfo->AlgoGrp.NumAlgo; AlgoIdx++) {
            pAlgoInfo = &pModeInfo->AlgoGrp.AlgoInfo[AlgoIdx];
            pAlgoOp   = &pCtrl->AlgoOp[AlgoIdx];

            Rval = AmbaWrap_memset(&QueryCfg, 0, sizeof(SVC_CV_FLOW_ALGO_QUERY_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_LINUX_STEREO, "memset QueryCfg failed", 0U, 0U);
            }
            Rval = SvcCvFlow_QueryAlgo(SVC_LOG_LINUX_STEREO, pAlgoInfo, &QueryCfg, pAlgoOp);
            if (SVC_OK == Rval) {
                /* Create algo */
                Rval = AmbaWrap_memset(&CreateCfg, 0, sizeof(SVC_CV_FLOW_ALGO_CREATE_CFG_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_LINUX_STEREO, "memset CreateCfg failed", 0U, 0U);
                }

                if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG) {

                } else if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
#ifdef CONFIG_AMBALINK_BOOT_OS
                   if (1U == SvcCv_FlexiIOStatusGet(pCtrl->FlexiIOChan)) {
                       const SVC_CV_ALGO_LINUX_EX_CFG_s *pExCfg = &ExCfg;

                       ExCfg.AmbaIPCChannel = pCtrl->FlexiIOChan;
                       AmbaMisra_TypeCast(&CreateCfg.pExtCfg, &pExCfg);
                   } else {
                       SvcLog_NG(SVC_LOG_LINUX_STEREO, "FlexidagIO(%u) has not ready. Please run Linux_Stereo app first.", pCtrl->FlexiIOChan, 0U);
                       Rval = SVC_NG;
                   }
#else
                    SvcLog_NG(SVC_LOG_LINUX_STEREO, "CONFIG_AMBALINK_BOOT_OS not defined ", 0U, 0U);
                    Rval = SVC_NG;
#endif
                } else {
                    SvcLog_NG(SVC_LOG_LINUX_STEREO, "unsupported CVFlow ProfID(%u)", pModeInfo->ProfID, 0U);
                    Rval = SVC_NG;
                }
                Rval = SvcCvFlow_CreateAlgo(SVC_LOG_LINUX_STEREO, pAlgoInfo, &CreateCfg, pAlgoOp);

                if (SVC_OK == Rval) {
                    /* Create Output buffer */
                    for (i = 0; i < SVC_LINUX_STEREO_OUTPUT_DEPTH; i++) {
                        Num = pAlgoOp->OutputNum;
                        for (j = 0; j < Num; j++) {
                            Rval |= SvcCvFlow_AllocWorkBuf(pAlgoOp->OutputSize[j], &BufInfo);
                            SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_LINUX_STEREO, BufInfo.pAddr, BufInfo.Size, &pCtrl->AlgoOpEx[AlgoIdx].OutputBuf[i][j]);
                        }
                    }

                    if (SVC_OK == Rval) {
                        /* Register Algo callback */
                        Rval = AmbaWrap_memset(&CbCfg, 0, sizeof(SVC_CV_ALGO_REGCB_CFG_s));
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_LINUX_STEREO, "memset CbCfg failed", 0U, 0U);
                        }
                        CbCfg.Mode     = 0U;
                        CbCfg.Callback = Linux_Stereo_AlgoOutCbList[Inst]; //Register Algo callback
                        Rval = SvcCvAlgo_RegCallback(&pAlgoOp->AlgoHandle, &CbCfg);
                        if (SVC_OK == Rval) {
                            pCtrl->NumValidAlgo++;
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_LINUX_STEREO, "Alloc Algo[%u] Output Buf failed", AlgoIdx, 0U);
                    }
                } else {
                   SvcLog_NG(SVC_LOG_LINUX_STEREO, "CreateAlgo[%u] error", AlgoIdx, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_LINUX_STEREO, "QueryAlgo[%u] error", AlgoIdx, 0U);
            }
        }


        Rval = LinuxStereo_Config(Inst, &AcConfig);

        Rval |= LinuxStereo_ConfigBuffer(Inst);

        if (SVC_OK == Rval) {
            pCtrl->pTxProc = LinuxStereo_TxDataSrcHandler;
            pCtrl->pMvacRxProc = LinuxStereo_MvacRxDataSinkHandler;
            pCtrl->pStixelRxProc = LinuxStereo_StixelRxDataSinkHandler;
            pCtrl->pFusionRxProc = LinuxStereo_FusionRxDataSinkHandler;

            Rval |= AmbaKAL_MsgQueueCreate(&pCtrl->AlgoOutQueMVACId,
                                           CbMVACQueName,
                                           sizeof(Linux_Stereo_OUTPUT_MSG_s),
                                           &AlgoOutQue[Inst][0],
                                           SVC_LINUX_STEREO_ALGO_OUT_QUEUE_SIZE * sizeof(Linux_Stereo_OUTPUT_MSG_s));

            Rval |= AmbaKAL_MsgQueueCreate(&pCtrl->AlgoOutQueStixelId,
                                                    CbStixelEvtQueName,
                                                    sizeof(STEREO_STIXEL_OUTPUT_MSG_s),
                                                    &AlgoStixelOutQue[Inst][0],
                                                    SVC_LINUX_STEREO_ALGO_OUT_QUEUE_SIZE * sizeof(STEREO_STIXEL_OUTPUT_MSG_s));

            Rval |= AmbaKAL_MsgQueueCreate(&pCtrl->AlgoOutQueFusionId,
                                                    CbFusionEvtQueName,
                                                    sizeof(STEREO_FUSION_OUTPUT_MSG_s),
                                                    &AlgoFusionOutQue[Inst][0],
                                                    SVC_LINUX_STEREO_ALGO_OUT_QUEUE_SIZE * sizeof(STEREO_FUSION_OUTPUT_MSG_s));

            Rval |= AmbaKAL_SemaphoreCreate(&pCtrl->OutBufSem, SemName, SVC_LINUX_STEREO_SEM_INIT_COUNT);
            if (KAL_ERR_NONE == Rval) {
                /* Create RX task */
                pCtrl->MVACRxTaskCtrl.Priority   = 60;
                pCtrl->MVACRxTaskCtrl.EntryFunc  = LinuxStereo_MvacRxTaskEntry;
                pCtrl->MVACRxTaskCtrl.EntryArg   = Inst;
                pCtrl->MVACRxTaskCtrl.pStackBase = &Linux_StereoACRxTaskStack[Inst][0];
                pCtrl->MVACRxTaskCtrl.StackSize  = SVC_LINUX_STEREO_RX_TASK_STACK_SIZE;
                pCtrl->MVACRxTaskCtrl.CpuBits    = 0x01U;
                Rval |= SvcTask_Create("SvcACRxTask", &pCtrl->MVACRxTaskCtrl);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_LINUX_STEREO, "Create RxTask Inst(%u) error", Inst, 0U);
                }

                pCtrl->StixelRxTaskCtrl.Priority   = 58;
                pCtrl->StixelRxTaskCtrl.EntryFunc  = LinuxStereo_StixelRxTaskEntry;
                pCtrl->StixelRxTaskCtrl.EntryArg   = Inst;
                pCtrl->StixelRxTaskCtrl.pStackBase = &Linux_StereoStixelRxTaskStack[Inst][0];
                pCtrl->StixelRxTaskCtrl.StackSize  = SVC_LINUX_STEREO_RX_TASK_STACK_SIZE;
                pCtrl->StixelRxTaskCtrl.CpuBits    = 0x01U;
                Rval = SvcTask_Create("SvcStixelRxTask", &pCtrl->StixelRxTaskCtrl);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_LINUX_STEREO, "Create RxTask Inst(%u) error", Inst, 0U);
                }

                pCtrl->FusionRxTaskCtrl.Priority   = 58;
                pCtrl->FusionRxTaskCtrl.EntryFunc  = LinuxStereo_FusionRxTaskEntry;
                pCtrl->FusionRxTaskCtrl.EntryArg   = Inst;
                pCtrl->FusionRxTaskCtrl.pStackBase = &Linux_StereoFusionRxTaskStack[Inst][0];
                pCtrl->FusionRxTaskCtrl.StackSize  = SVC_LINUX_STEREO_RX_TASK_STACK_SIZE;
                pCtrl->FusionRxTaskCtrl.CpuBits    = 0x01U;
                Rval = SvcTask_Create("SvcFusionRxTask", &pCtrl->FusionRxTaskCtrl);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_LINUX_STEREO, "Create RxTask Inst(%u) error", Inst, 0U);
                }

                /* Start input */
                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_LINUX_STEREO, "Connect feeder port error", 0U, 0U);
                    }
                }

                if (SVC_OK == Rval) {
                    pCtrl->Started = 1U;
                }
            } else {
                SvcLog_NG(SVC_LOG_LINUX_STEREO, "Create MsgQue/Semaphore error(0x%X)", Rval, 0U);
                Rval =  SVC_NG;
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "Invalid CvFlowChan", 0U, 0U);
        Rval =  SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_LINUX_STEREO, "CvFlow_Linux_StereoStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "CvFlow_Linux_StereoStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver control function
*/
static UINT32 CvFlow_LinuxStereoControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo)
{
    UINT32 Rval, Inst;
    SVC_Linux_Stereo_CTRL_s *pCtrl;
    const UINT32 *pValue;

    LinuxStereo_MutexTake();

    Rval = LinuxStereo_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl = &g_LinuxStereoCtrl[Inst];
        switch (CtrlType) {
            case SVC_CV_CTRL_ENABLE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->Enable = (UINT8) *pValue;
                if ((1U == pCtrl->Started) &&
                    (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder)) {
                    if (*pValue == 0U) {
                        Rval = SvcImgFeeder_Disconnect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_LINUX_STEREO, "Disconn feeder port error", 0U, 0U);
                        }
                    } else {
                        Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_LINUX_STEREO, "Cnnect feeder port error", 0U, 0U);
                        }
                    }
                }
                break;
            case SVC_CV_CTRL_SET_SRC_RATE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->SrcRateCtrl = *pValue;
                SvcLog_OK(SVC_LOG_LINUX_STEREO, "Control[%d]: SrcRate = %u", Inst, *pValue);
                break;
            case SVC_CV_CTRL_SEND_EXT_YUV:
                Rval = LinuxStereo_SendExtYUV(Inst, pInfo);
                break;
            case SVC_CV_CTRL_SET_META_DATA:
                Rval = Linux_Stereo_SetMetaData(Inst, pInfo);
                break;
            default:
                SvcLog_NG(SVC_LOG_LINUX_STEREO, "Control[%d]: Invalid CtrlType(%d)", Inst, CtrlType);
                break;
        }
    } else {
        SvcLog_NG(SVC_LOG_LINUX_STEREO, "Control: Invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    LinuxStereo_MutexGive();

    return Rval;
}

/**
* CvFlow driver stop function
*/
static UINT32 CvFlow_LinuxStereoStop(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_Linux_Stereo_CTRL_s *pCtrl;
    SVC_CV_FLOW_ALGO_DELETE_CFG_s DelCfg;

    (void)pInput;

    Rval = LinuxStereo_CvfChan2Inst(CvFlowChan, &Inst);
    pCtrl = &g_LinuxStereoCtrl[Inst];

    if (SVC_OK == Rval) {
        Rval = LinuxStereo_SetTaskIdle(Inst);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_LINUX_STEREO, "SetTaskIdle error", 0U, 0U);
        }

        Rval = SvcTask_Destroy(&pCtrl->MVACRxTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_LINUX_STEREO, "Destroy Tx/Rx Task error", 0U, 0U);
        }

        Rval = SvcTask_Destroy(&pCtrl->StixelRxTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_LINUX_STEREO, "Destroy Tx/Rx Task error", 0U, 0U);
        }

        Rval = SvcTask_Destroy(&pCtrl->FusionRxTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_LINUX_STEREO, "Destroy Tx/Rx Task error", 0U, 0U);
        }

        Rval = AmbaWrap_memset(&DelCfg, 0, sizeof(SVC_CV_FLOW_ALGO_DELETE_CFG_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_LINUX_STEREO, "memset DelCfg failed", 0U, 0U);
        }

#ifdef CONFIG_AMBALINK_BOOT_OS
        if (SvcCvFlow_LinuxStereo_ModeInfo[pCtrl->pCvFlow->CvModeID].ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
            Rval = SvcCv_FlexiIORelease(pCtrl->FlexiIOChan);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_LINUX_STEREO, "SvcCv_FlexiIORelease(%u) failed", pCtrl->FlexiIOChan, 0U);
            }
        }
#endif

        Rval = AmbaKAL_MsgQueueDelete(&pCtrl->AlgoOutQueMVACId);
        Rval = AmbaKAL_MsgQueueDelete(&pCtrl->AlgoOutQueStixelId);
        Rval = AmbaKAL_MsgQueueDelete(&pCtrl->AlgoOutQueFusionId);
        Rval |= AmbaKAL_SemaphoreDelete(&pCtrl->OutBufSem);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_LINUX_STEREO, "Delete MsgQue/Semaphore error(0x%X)", Rval, 0U);
        }

        /* Reset Ctrl Intance */
        Rval = AmbaWrap_memset(&g_LinuxStereoCtrl[Inst], 0, sizeof(SVC_Linux_Stereo_CTRL_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_LINUX_STEREO, "memset g_LinuxStereoCtrl[%u] failed", Inst, 0U);
        }
    }

    SvcLog_OK(SVC_LOG_LINUX_STEREO, "CvFlow_Linux_StereoStop: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver debug interface
*/
static UINT32 CvFlow_LinuxStereoDebug(UINT32 CvFlowChan, const char *pCmd, UINT32 Param0, UINT32 Param1, UINT32 Param2)
{
    UINT32 i;
    const SVC_Linux_Stereo_CTRL_s *pCtrl;

    if (0 == SvcWrap_strcmp("debug", pCmd)) {
        SvcCvDebugEnable = (UINT8) Param0;
        SvcLog_DBG(SVC_LOG_LINUX_STEREO, "Set debug(%u)", Param0, 0U);
    } else if (0 == SvcWrap_strcmp("info", pCmd)) {
        for (i = 0; i < SVC_LINUX_STEREO_MAX_INSTANCE; i++) {
            pCtrl = &g_LinuxStereoCtrl[i];
            if (pCtrl->Used == 1U) {
                AmbaPrint_PrintUInt5("ST_AC[%d]: CvfChan %d", i, pCtrl->CvfChan, 0U, 0U, 0U);
            }
        }
    } else {
        SvcLog_DBG(SVC_LOG_LINUX_STEREO, "Unsupported cmd", 0U, 0U);
    }

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(&Param0);
    AmbaMisra_TouchUnused(&Param1);
    AmbaMisra_TouchUnused(&Param2);

    return SVC_OK;
}

SVC_CV_FLOW_OBJ_s SvcCvFlow_LinuxStereoObj = {
    .Init         = CvFlow_LinuxStereoInit,
    .Config       = CvFlow_LinuxStereoConfig,
    .Load         = CvFlow_LinuxStereoLoad,
    .Start        = CvFlow_LinuxStereoStart,
    .Control      = CvFlow_LinuxStereoControl,
    .Stop         = CvFlow_LinuxStereoStop,
    .Debug        = CvFlow_LinuxStereoDebug,
};

