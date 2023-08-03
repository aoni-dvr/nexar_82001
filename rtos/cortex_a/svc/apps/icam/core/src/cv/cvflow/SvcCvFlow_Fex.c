/**
 *  @file SvcCvFlow_Fex.c
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
 *  @details Implementation of FlexiDAG based Ambarella Feature Extraction
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
#include "SvcCvFlow_Fex.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcImgFIFO.h"
#include "SvcImgFeeder.h"
#include "SvcOsd.h"
#include "SvcWrap.h"

#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_ref_util.h"
#if defined (CONFIG_SOC_CV2FS)
#include "cvapi_flexidag_ambaspufex_cv2fs.h"
#else
#include "cvapi_flexidag_ambaspufex_cv2.h"
#endif

#define SVC_LOG_FEX        "Fex"

#define SVC_FEX_INSTANCE0                   (0U)
#define SVC_FEX_INSTANCE1                   (1U)
#define SVC_FEX_MAX_INSTANCE                (2U)

#define SVC_FEX_SEM_INIT_COUNT              (1U)
#define SVC_FEX_OUTPUT_DEPTH                (SVC_FEX_SEM_INIT_COUNT + 3U)

#define SVC_FEX_RX_TASK_STACK_SIZE          (0x4000)
#define SVC_FEX_PICINFO_QUEUE_SIZE          (8U)
#define SVC_FEX_ALGO_OUT_QUEUE_SIZE         (SVC_FEX_OUTPUT_DEPTH)

#define SRC_RATE_FRACTION_MODE_BIT          (0x10000000U)

typedef UINT32 (*SVC_FEX_DATA_HANDLER_f)(UINT32 Inst);
typedef UINT32 (*SVC_FEX_TX_HANDLER_f)(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

typedef struct {
    flexidag_memblk_t           OutputBuf[SVC_FEX_OUTPUT_DEPTH][FLEXIDAG_MAX_OUTPUTS];
    AMBA_CV_FLEXIDAG_IO_s       InBuf[SVC_FEX_OUTPUT_DEPTH];
    AMBA_CV_FLEXIDAG_IO_s       OutBuf[SVC_FEX_OUTPUT_DEPTH];
} SVC_CV_FLOW_FEX_OP_EX_INFO_s;

typedef struct {
    UINT8                       Used;
    UINT8                       CvfChan;
    UINT8                       Enable;
    UINT8                       Started;
    SVC_CV_FLOW_CFG_s           *pCvFlow;
    SVC_CV_FLOW_CFG_s           CvCfg;

    UINT8                       NumValidAlgo;
    SVC_CV_FLOW_ALGO_OP_INFO_s  AlgoOp[FEX_MAX_ALGO_NUM];
    SVC_CV_FLOW_FEX_OP_EX_INFO_s AlgoOpEx[FEX_MAX_ALGO_NUM];
    void                       *pImgInfoBuf[SVC_FEX_PICINFO_QUEUE_SIZE];
    UINT32                      ImgInfoBufSize;

    /* Data source service */
    SVC_IMG_FEEDER_PORT_s       FeederPort;
    UINT32                      SrcSeqNum;
    SVC_FEX_TX_HANDLER_f      pTxProc;
    UINT32                      TxSeqNum;
    UINT32                      PicInfoIdx;

    /* Data sink service */
    SVC_TASK_CTRL_s             RxTaskCtrl;
    SVC_FEX_DATA_HANDLER_f    pRxProc;
    UINT32                      RxSeqNum;
    UINT32                      OutBufIdx;
    AMBA_KAL_SEMAPHORE_t        OutBufSem;
    UINT32                      SinkInfoIdx;

    /* Algo Callback */
    AMBA_KAL_MSG_QUEUE_t        AlgoOutQueId;


    /* Misc */
    UINT32                      SrcRateCtrl;
#define                         FEX_SRC_RATE_MAX_DENOMINATOR  (16U)
    UINT8                       SrcValidTbl[FEX_SRC_RATE_MAX_DENOMINATOR];
    UINT8                       SrcRateNum;
    UINT8                       SrcRateDen;
    UINT8                       SrcInfoErr;
    UINT8                       SceneMode;
    UINT32                      FlexiIOChan;    /* for AmbaLink CV only */
} SVC_FEX_CTRL_s;

typedef struct {
   UINT32 Event;
   SVC_CV_ALGO_OUTPUT_s AlgoOut;
} FEX_OUTPUT_MSG_s;

static SVC_FEX_CTRL_s g_FexCtrl[SVC_FEX_MAX_INSTANCE] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MUTEX_t FexMutex;
static UINT8 SvcCvDebugEnable = 0;

static UINT32 CvFlow_FexInit(UINT32 CvFlowChan, void *pInit);
static UINT32 CvFlow_FexConfig(UINT32 CvFlowChan, void *pConfig);
static UINT32 CvFlow_FexStart(UINT32 CvFlowChan, void *pInput);
static UINT32 CvFlow_FexControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo);
static UINT32 CvFlow_FexStop(UINT32 CvFlowChan, void *pInput);
static UINT32 Fex_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst);

static UINT32 Fex_TxDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 Fex_TxDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

static UINT32 Fex_AlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);
static UINT32 Fex_AlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData);

static void SVC_CV_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcCvDebugEnable > 0U) {
        SvcLog_DBG(pModule, pFormat, Arg1, Arg2);
    }
}

static void Fex_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&FexMutex, 5000)) {
        SvcLog_NG(SVC_LOG_FEX, "Fex_MutexTake: timeout", 0U, 0U);
    }
}

static void Fex_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&FexMutex)) {
        SvcLog_NG(SVC_LOG_FEX, "Fex_MutexGive: error", 0U, 0U);
    }
}

static UINT32 Fex_SemTake(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreTake(&g_FexCtrl[Inst].OutBufSem, 10000)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_FEX, "Fex_SemTake[%d]: timeout", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 Fex_SemGive(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreGive(&g_FexCtrl[Inst].OutBufSem)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_FEX, "Fex_SemGive[%d]: error", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 Fex_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst)
{
    UINT32 Rval = SVC_NG;
    UINT32 i;

    if (NULL != pInst) {
        for (i = 0; i < SVC_FEX_MAX_INSTANCE; i++) {
            if ((1U == g_FexCtrl[i].Used) && (CvfChan == g_FexCtrl[i].CvfChan)) {
                *pInst = i;
                Rval = SVC_OK;
                break;
            }
        }
    }
    return Rval;
}

static void Fex_DumpPicInfo(const cv_pic_info_t *pPicInfo)
{
    static UINT32 DumpTimes = 0U;
    extern void SvcCvFlow_DumpPicInfo(const char *pModuleName, const cv_pic_info_t *pPicInfo);

    if (DumpTimes > 0U) {
        SvcCvFlow_DumpPicInfo(SVC_LOG_FEX, pPicInfo);
        DumpTimes--;
    }
}

/**
* Pack pyramid information to pic info structure
* @param [in] pPicInfo pic info structure
* @param [in] pYuvInfo pyramid information
* @param [in] MainWidth width of main YUV
* @param [in] MainHeight height of main YUV
* @param [in] Flag Bit[0]: 1-transfer to relative address. Bit[1]: 1-Stereo Mode
* @param [in] LeftRight fill in left or right channel in pic info structure. 1-Left chan only, 2-Right chan only, 0 or 3-Duplicated
* @return 0-OK, 1-NG
*/
#define LEFT_CHAN               (0x1U)
#define RIGHT_CHAN              (0x2U)
#define FLAG_RELATIVE_ADDR      (0x00000001U)
#define FLAG_STEREO_MODE        (0x00000002U)
static UINT32 Fex_PackPicInfo_Pyramid_Stereo(cv_pic_info_t *pPicInfo, const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo, UINT32 MainWidth, UINT32 MainHeight, UINT32 Flag, UINT32 LeftRight)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i, DataChan;
    ULONG  BufPhyAddr = 0U, PicInfoAddr = 0U, PicInfoPhyAddr = 0U;
    UINT32 StereoMode = 0U;

    if ((pPicInfo != NULL) && (pYuvInfo != NULL)) {
        /* Important: The addresses for FlexiDAG are relative to picinfo. */
        if ((Flag & FLAG_RELATIVE_ADDR) > 0U) {
            AmbaMisra_TypeCast(&PicInfoAddr, &pPicInfo);
            if (MMU_ERR_NONE != AmbaMMU_VirtToPhys(PicInfoAddr, &PicInfoPhyAddr)) {
                SvcLog_NG(SVC_LOG_FEX, "PicInfoAddr V2P error", 0U, 0U);
                RetVal |= SVC_NG;
            }
        }

        if ((Flag & FLAG_STEREO_MODE) > 0U) {
            StereoMode = 1U;
        }

        if (LeftRight == 0U) { /* Duplicate the info */
            DataChan = (LEFT_CHAN | RIGHT_CHAN);
        } else {
            DataChan = LeftRight;
        }

        pPicInfo->capture_time = (UINT32) pYuvInfo->CapPts;
        pPicInfo->channel_id   = (UINT8)  pYuvInfo->ViewZoneId;
        pPicInfo->frame_num    = (UINT32) pYuvInfo->CapSequence;

        if ((pYuvInfo->YuvBuf[0].Window.Width != 0U) && (pYuvInfo->YuvBuf[0].Window.Height != 0U)) {
            pPicInfo->pyramid.image_width_m1  = pYuvInfo->YuvBuf[0].Window.Width - 1U;
            pPicInfo->pyramid.image_height_m1 = pYuvInfo->YuvBuf[0].Window.Height - 1U;
            pPicInfo->pyramid.image_pitch_m1  = (UINT32) pYuvInfo->YuvBuf[0].Pitch - 1U;
        } else {
            pPicInfo->pyramid.image_width_m1  = (UINT16) MainWidth - 1U;
            pPicInfo->pyramid.image_height_m1 = (UINT16) MainHeight - 1U;
            pPicInfo->pyramid.image_pitch_m1  = (UINT32) (GetAlignedValU32(MainWidth, 64U) + 64U) - 1U;
        }

        /* Fill layer 0 information for fusion use */
        pPicInfo->pyramid.half_octave[0].ctrl.roi_pitch = (UINT16) pPicInfo->pyramid.image_pitch_m1;
        pPicInfo->pyramid.half_octave[0].roi_start_col  = 0;
        pPicInfo->pyramid.half_octave[0].roi_start_row  = 0;
        pPicInfo->pyramid.half_octave[0].roi_width_m1   = pPicInfo->pyramid.image_width_m1;
        pPicInfo->pyramid.half_octave[0].roi_height_m1  = pPicInfo->pyramid.image_height_m1;

        for (i = 0U; i < AMBA_DSP_MAX_HIER_NUM; i++) {
            if (((pYuvInfo->Config.HierBit >> i) & 0x1U) == 1U) {
                pPicInfo->pyramid.half_octave[i].ctrl.roi_pitch = pYuvInfo->YuvBuf[i].Pitch;
                pPicInfo->pyramid.half_octave[i].roi_start_col  = (INT16) pYuvInfo->YuvBuf[i].Window.OffsetX;
                pPicInfo->pyramid.half_octave[i].roi_start_row  = (INT16) pYuvInfo->YuvBuf[i].Window.OffsetY;
                pPicInfo->pyramid.half_octave[i].roi_width_m1   = pYuvInfo->YuvBuf[i].Window.Width - 1U;
                pPicInfo->pyramid.half_octave[i].roi_height_m1  = pYuvInfo->YuvBuf[i].Window.Height - 1U;

                if ((DataChan & LEFT_CHAN) > 0U) {
                    if (MMU_ERR_NONE != AmbaMMU_VirtToPhys(pYuvInfo->YuvBuf[i].BaseAddrY, &BufPhyAddr)) {
                        SvcLog_NG(SVC_LOG_FEX, "BaseAddrY V2P error0", 0U, 0U);
                        RetVal |= SVC_NG;
                    }
                    pPicInfo->rpLumaLeft[i] = BufPhyAddr - PicInfoPhyAddr;
                    if (MMU_ERR_NONE != AmbaMMU_VirtToPhys(pYuvInfo->YuvBuf[i].BaseAddrUV, &BufPhyAddr)) {
                        SvcLog_NG(SVC_LOG_FEX, "BaseAddrUV V2P error0", 0U, 0U);
                        RetVal |= SVC_NG;
                    }
                    pPicInfo->rpChromaLeft[i] = BufPhyAddr - PicInfoPhyAddr;
                }

                if ((DataChan & RIGHT_CHAN) > 0U) {
                    if (MMU_ERR_NONE != AmbaMMU_VirtToPhys(pYuvInfo->YuvBuf[i].BaseAddrY, &BufPhyAddr)) {
                        SvcLog_NG(SVC_LOG_FEX, "BaseAddrY V2P error1", 0U, 0U);
                        RetVal |= SVC_NG;
                    }
                    pPicInfo->rpLumaRight[i]   = BufPhyAddr - PicInfoPhyAddr;
                    if (MMU_ERR_NONE != AmbaMMU_VirtToPhys(pYuvInfo->YuvBuf[i].BaseAddrUV, &BufPhyAddr)) {
                        SvcLog_NG(SVC_LOG_FEX, "BaseAddrUV V2P error1", 0U, 0U);
                        RetVal |= SVC_NG;
                    }
                    pPicInfo->rpChromaRight[i] = BufPhyAddr - PicInfoPhyAddr;
                }
            }
        }

        for (i = 0; i < AMBA_DSP_MAX_HIER_NUM; i++) {
            if ((pPicInfo->rpLumaRight[i] != 0U) &&
                (pPicInfo->rpLumaLeft[i] != 0U)) {
                pPicInfo->pyramid.half_octave[i].ctrl.disable = 0U;
                if (StereoMode == 1U) {
                    pPicInfo->pyramid.half_octave[i].ctrl.mode = 1U;
                } else {
                    pPicInfo->pyramid.half_octave[i].ctrl.mode = 0U;
                }
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
        SvcLog_NG(SVC_LOG_FEX, "PackPicInfo_Pyramid: invalid info", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Pack data source to pic info structure
*/
static void Fex_PackPicInfo(UINT32 Inst, const SVC_CV_INPUT_IMG_INFO_s *pInputImg, memio_source_recv_picinfo_t *pInfo)
{
    UINT32 Rval = SVC_OK;
    cv_pic_info_t *pPicInfo = &pInfo->pic_info;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    const SVC_FEX_CTRL_s *pCtrl = &g_FexCtrl[Inst];
    UINT32 DataSrc, RelativeAddrOn = 1U, Flag = 0U;

    Rval = AmbaWrap_memset(pInfo, 0, sizeof(memio_source_recv_picinfo_t));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_FEX, "memset picinfo failed", 0U, 0U);
    }

    DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
    if (pInputImg->NumInfo > 0U) {
        if (SvcCvFlow_Fex_ModeInfo[pCtrl->pCvFlow->CvModeID].ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
            RelativeAddrOn = 0U;
        }

        if (DataSrc == SVC_CV_DATA_SRC_PYRAMID) {
            const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo;
            AmbaMisra_TypeCast(&pYuvInfo, &pInputImg->Info[0].pBase);

            if (RelativeAddrOn > 0U) {
                Flag |= FLAG_RELATIVE_ADDR;
            }
            Flag |= FLAG_STEREO_MODE;

            Rval = Fex_PackPicInfo_Pyramid_Stereo(pPicInfo,
                                                  pYuvInfo,
                                                  pCfg->FovCfg[pYuvInfo->ViewZoneId].MainWin.Width,
                                                  pCfg->FovCfg[pYuvInfo->ViewZoneId].MainWin.Height,
                                                  Flag, 0U);
        } else {
            SvcLog_NG(SVC_LOG_FEX, "Fex_PackPicInfo: invalid DataSrc(%d)", DataSrc, 0U);
        }
    } else {
        Rval = SVC_NG;
    }

    Fex_DumpPicInfo(pPicInfo);

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_FEX, "Fex_PackPicInfo error", 0U, 0U);
    }
}

/**
* Send message to Feeder to update port status
*/
static void Fex_SendReceiverReady(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort)
{
    SVC_FEX_CTRL_s *pCtrl = &g_FexCtrl[Inst];
    AMBA_KAL_SEMAPHORE_INFO_s SemInfo;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreQuery(&pCtrl->OutBufSem, &SemInfo)) {
        if (SemInfo.CurrentCount > 0U) {
            if (SVC_OK != SvcImgFeeder_MsgToFeeder(pPort, SVC_IMG_FEEDER_RECEIVER_READY, NULL)) {
                SvcLog_NG(SVC_LOG_FEX, "Send ReceiverReady[%u] error", Inst, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_FEX, "ReceiverReady[%u]: SemQuery error", Inst, 0U);
    }
}

/**
* TX data source handler: process input data
*/
static UINT32 Fex_TxDataSrcHandler(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_FEX_CTRL_s *pCtrl = &g_FexCtrl[Inst];
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
    const SVC_CV_INPUT_IMG_INFO_s *pInputImg;
    memio_source_recv_picinfo_t *pPicInfo;
    UINT32 i, j;
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;
    AMBA_CV_FLEXIDAG_IO_s *pInBuf, *pOutBuf;
    const SVC_CV_FLOW_FEX_MODE_INFO_s *pModeInfo;

    AmbaMisra_TouchUnused(pInfo);
    pModeInfo = &SvcCvFlow_Fex_ModeInfo[pCtrl->pCvFlow->CvModeID];

    SVC_CV_DBG(SVC_LOG_FEX, "Tx[%u]: run %d", Inst, pCtrl->TxSeqNum);

    if (Event == SVC_IMG_FEEDER_IMAGE_DATA_READY) {
        if ((pCtrl->Enable == 1U) &&
            (0U == (pCtrl->SrcSeqNum % pCtrl->SrcRateCtrl))) {
            Rval = Fex_SemTake(Inst);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pInputImg, &pInfo);
                AmbaMisra_TypeCast(&pPicInfo, &pCtrl->pImgInfoBuf[pCtrl->PicInfoIdx]);
                Fex_PackPicInfo(Inst, pInputImg, pPicInfo);

                /* Prepare feed config */
                Rval = AmbaWrap_memset(&FeedCfg, 0, sizeof(SVC_CV_ALGO_FEED_CFG_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_FEX, "memset FeedCfg failed", 0U, 0U);
                }

                for (i = 0; i < pCtrl->NumValidAlgo; i++) {
                    pAlgoOp = &pCtrl->AlgoOp[i];
                    pInBuf = &pCtrl->AlgoOpEx[i].InBuf[pCtrl->OutBufIdx];
                    pOutBuf = &pCtrl->AlgoOpEx[i].OutBuf[pCtrl->OutBufIdx];

                    FeedCfg.pIn  = pInBuf;
                    FeedCfg.pOut = pOutBuf;
                    pInBuf->num_of_buf = 1U;
                    SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_FEX, pPicInfo, pCtrl->ImgInfoBufSize, &pInBuf->buf[0]);
                    pOutBuf->num_of_buf = pAlgoOp->OutputNum;
                    for (j = 0U; j < pAlgoOp->OutputNum; j++) {
                        Rval = AmbaWrap_memcpy(&pOutBuf->buf[j], &pCtrl->AlgoOpEx[i].OutputBuf[pCtrl->OutBufIdx][j], sizeof(flexidag_memblk_t));
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_FEX, "memcpy OutputBufBlk failed", 0U, 0U);
                        }
                    }

                    if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG_LINUX) {
                        /* Do nothing */
                    } else {
                        AmbaMisra_TypeCast(&FeedCfg.pExtFeedCfg, &pPicInfo); /* SCA_OpenOD */
                        AmbaMisra_TypeCast(&FeedCfg.pUserData, &pPicInfo);
                    }

                    Rval |= SvcCvAlgo_Feed(&pAlgoOp->AlgoHandle, &FeedCfg);
                    SVC_CV_DBG(SVC_LOG_FEX, "Tx[%u]: Send(%u)", Inst, pPicInfo->pic_info.frame_num);
                }

                if (SVC_OK == Rval) {
                    pCtrl->TxSeqNum++;
                    pCtrl->PicInfoIdx++;
                    if (pCtrl->PicInfoIdx >= SVC_FEX_PICINFO_QUEUE_SIZE) {
                        pCtrl->PicInfoIdx = 0;
                    }
                    pCtrl->OutBufIdx++;
                    if (pCtrl->OutBufIdx >= SVC_FEX_OUTPUT_DEPTH) {
                        pCtrl->OutBufIdx = 0;
                    }
                } else {
                    SvcLog_NG(SVC_LOG_FEX, "Tx[%u]: send error", Inst, 0U);
                }

                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    Fex_SendReceiverReady(Inst, pPort);
                }
            } else {
                SvcLog_NG(SVC_LOG_FEX, "Tx[%u]: SemTake error", Inst, 0U);
            }
        } else {
            if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                Fex_SendReceiverReady(Inst, pPort);
                Rval = AmbaKAL_TaskSleep(1);
            }
        }

        pCtrl->SrcSeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_FEX, "Tx[%u]: unsupported event(0x%x)", Inst, Event);
        Rval = SVC_NG;
    }

    SVC_CV_DBG(SVC_LOG_FEX, "Tx[%u]: end", Inst, 0U);

    return Rval;
}

static UINT32 Fex_TxDataSrcHandler0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_FEX_CTRL_s *pCtrl = &g_FexCtrl[SVC_FEX_INSTANCE0];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_FEX_INSTANCE0, pPort, Event, pInfo);
    }
    return Rval;
}

static UINT32 Fex_TxDataSrcHandler1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    const SVC_FEX_CTRL_s *pCtrl = &g_FexCtrl[SVC_FEX_INSTANCE1];

    if (NULL != pCtrl->pTxProc) {
        Rval = pCtrl->pTxProc(SVC_FEX_INSTANCE1, pPort, Event, pInfo);
    }
    return Rval;
}

/**
* Algorithm output event callback function
*/
static UINT32 Fex_AlgoOutCallback(UINT32 Inst, UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Rval = SVC_OK;
    SVC_FEX_CTRL_s *pCtrl = &g_FexCtrl[Inst];
    FEX_OUTPUT_MSG_s Msg;
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
                SvcLog_NG(SVC_LOG_FEX, "memcpy AlgoOut failed", 0U, 0U);
            }
        }
        Rval = AmbaKAL_MsgQueueSend(&pCtrl->AlgoOutQueId, &Msg, 5000);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_FEX, "Fex_AlgoOutCallback[%u]: MsgQueueSend error", Inst, 0U);
        }
    }

    return Rval;
}

static UINT32 Fex_AlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Fex_AlgoOutCallback(SVC_FEX_INSTANCE0, Event, pEventData);
}

static UINT32 Fex_AlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Fex_AlgoOutCallback(SVC_FEX_INSTANCE1, Event, pEventData);
}

static void Fex_DumpAmbaFexResult(const AMBA_CV_FEX_DATA_s *pOut, UINT32 ScaleId)
{
#if 0
    ULONG ULFex, KeyPointCntAddr, KeyPointAddr;
    const UINT8 *pKeyPointCnt;
    const AMBA_CV_FEX_KEYPOINT_s *pKeyPoints;
    UINT32 Idex, j;
    const AMBA_CV_FEX_DATA_s *pFex;
    (void)AmbaMisra_TypeCast(&pFex, &pOut);
    (void)AmbaMisra_TypeCast(&ULFex, &pFex);
    if (pFex->PrimaryList[ScaleId].Enable == 1U) {
        KeyPointCntAddr = ULFex + pFex->PrimaryList[ScaleId].KeypointsCountOffset;
        KeyPointAddr = ULFex + pFex->PrimaryList[ScaleId].KeypointsOffset;
        (void)AmbaMisra_TypeCast(&pKeyPointCnt, &KeyPointCntAddr);
        (void)AmbaMisra_TypeCast(&pKeyPoints, &KeyPointAddr);
        for (Idex = 0U; Idex < CV_FEX_MAX_BUCKETS; Idex++) {
            AmbaPrint_PrintUInt5("PrimaryList Bucket[%d]:", Idex, 0U, 0U, 0U, 0U);
            for (j = 0U; j < pKeyPointCnt[Idex]; j++) {
                AmbaPrint_PrintUInt5("KeyPoint.2[%d]: %d, %d", j,
                        pKeyPoints[(Idex * CV_FEX_MAX_KEYPOINTS) + j].X,
                        pKeyPoints[(Idex * CV_FEX_MAX_KEYPOINTS) + j].Y, 0U, 0U);
            }
        }
    } else {
        AmbaPrint_PrintUInt5("FEX PrimaryList[%d] is Disabled", ScaleId, 0U, 0U, 0U, 0U);
    }
    if (pFex->SecondaryList[ScaleId].Enable == 1U) {
        KeyPointCntAddr = ULFex + pFex->SecondaryList[ScaleId].KeypointsCountOffset;
        KeyPointAddr = ULFex + pFex->SecondaryList[ScaleId].KeypointsOffset;
        (void)AmbaMisra_TypeCast(&pKeyPointCnt, &KeyPointCntAddr);
        (void)AmbaMisra_TypeCast(&pKeyPoints, &KeyPointAddr);
        for (Idex = 0U; Idex < CV_FEX_MAX_BUCKETS; Idex++) {
            AmbaPrint_PrintUInt5("SecondaryList Bucket[%d]:", Idex, 0U, 0U, 0U, 0U);
            for (j = 0U; j < pKeyPointCnt[Idex]; j++) {
                AmbaPrint_PrintUInt5("KeyPoint.2[%d]: %d, %d", j,
                        pKeyPoints[(Idex * CV_FEX_MAX_KEYPOINTS) + j].X,
                        pKeyPoints[(Idex * CV_FEX_MAX_KEYPOINTS) + j].Y, 0U, 0U);
            }
        }
    } else {
        AmbaPrint_PrintUInt5("FEX SecondaryList[%d] is Disabled", ScaleId, 0U, 0U, 0U, 0U);
    }
#else
    AmbaMisra_TouchUnused(&pOut);
    AmbaMisra_TouchUnused(&ScaleId);
#endif
}

/**
* RX data sink handler: process detection output
*/
static UINT32 Fex_RxDataSinkHandler(UINT32 Inst)
{
    UINT32 Rval;
    SVC_FEX_CTRL_s *pCtrl = &g_FexCtrl[Inst];
    const memio_source_recv_picinfo_t *pInPicInfo = NULL;
    FEX_OUTPUT_MSG_s Msg;
    const SVC_CV_ALGO_OUTPUT_s *pAlgoOut = &Msg.AlgoOut;

    AMBA_CV_FEX_DATA_s *pFexOut;
    SVC_CV_FLOW_FEX_OUTPUT_s Output;
    const SVC_CV_FLOW_FEX_MODE_INFO_s *pModeInfo = &SvcCvFlow_Fex_ModeInfo[pCtrl->pCvFlow->CvModeID];
    UINT8 OutputBufIdx = pModeInfo->OutputBufIdx;

    SVC_CV_DBG(SVC_LOG_FEX, "Rx[%u]: run %d", Inst, pCtrl->RxSeqNum);

    Rval = AmbaKAL_MsgQueueReceive(&pCtrl->AlgoOutQueId, &Msg, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK == Rval) {
        SVC_CV_DBG(SVC_LOG_FEX, "Rx[%u]: recv out(%u)", Inst, 0);

        if (Msg.Event == CALLBACK_EVENT_OUTPUT) {
            if (pCtrl->Enable == 1U) {
                AmbaMisra_TypeCast(&pFexOut, &pAlgoOut->pOutput->buf[OutputBufIdx].pBuffer);
                Fex_DumpAmbaFexResult(pFexOut, pCtrl->pCvFlow->InputCfg.Input[0U].RoiInfo.Roi[0].Index);

                Output.Msg = SVC_CV_FEX_OUTPUT;
                if (NULL != pAlgoOut->pUserData) {
                    AmbaMisra_TypeCast(&pInPicInfo, &pAlgoOut->pUserData);
                    Output.FrameNum = pInPicInfo->pic_info.frame_num;
                    Output.CaptureTime = pInPicInfo->pic_info.capture_time;
                } else {
                    Output.FrameNum = 0U;
                    Output.CaptureTime = 0U;
                }
                Output.pFexOut = pFexOut;
                Rval = SvcCvFlow_Issue(pCtrl->CvfChan,
                                       SVC_CV_FLOW_OUTPUT_PORT0,
                                       pCtrl->pCvFlow->OutputCfg.OutputTag[SVC_CV_FLOW_OUTPUT_PORT0],
                                       &Output);

                if (SVC_OK == Rval) {
                    pCtrl->SinkInfoIdx++;
                    if (pCtrl->SinkInfoIdx >= SVC_FEX_OUTPUT_DEPTH) {
                        pCtrl->SinkInfoIdx = 0;
                    }
                }
            }

            Rval = Fex_SemGive(Inst);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_FEX, "RxDataSinkHandler[%u]: SemGive error", Inst, 0U);
            }

            if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                Fex_SendReceiverReady(Inst, &pCtrl->FeederPort);
            }
        }

        pCtrl->RxSeqNum++;
    } else {
        SvcLog_NG(SVC_LOG_FEX, "Rx[%u]: recv error", Inst, 0U);
    }

    SVC_CV_DBG(SVC_LOG_FEX, "Rx[%u]: end", Inst, 0U);

    return SVC_OK;
}

/**
* Task entry of RX process
*/
static void* Fex_RxTaskEntry(void* EntryArg)
{
    const ULONG  *pArg;
    UINT32 Rval = SVC_OK, Inst;
    const SVC_FEX_CTRL_s *pCtrl;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Inst = (UINT32)(*pArg);
    pCtrl = &g_FexCtrl[Inst];

    SvcLog_DBG(SVC_LOG_FEX, "RxTaskEntry[%d] start", Inst, 0U);

    while (SVC_OK == Rval) {
        if (NULL != pCtrl->pRxProc) {
            Rval = pCtrl->pRxProc(Inst);
        } else {
            Rval = SVC_NG;
        }
    }

    SvcLog_DBG(SVC_LOG_FEX, "RxTaskEntry[%d] exit", Inst, 0U);

    return NULL;
}

/**
* Set and wait TX and RX task to idle state
*/
static UINT32 Fex_SetTaskIdle(UINT32 Inst)
{
    UINT32 Rval;
    SVC_FEX_CTRL_s *pCtrl = &g_FexCtrl[Inst];
    UINT32 i;

    /* Disable input and output */
    pCtrl->Enable = 0U;

    if (SVC_OK != SvcImgFeeder_Disconnect(&pCtrl->FeederPort)) {
        SvcLog_NG(SVC_LOG_FEX, "Disconn feeder port error", 0U, 0U);
    }

    if (SVC_OK != SvcImgFeeder_ClosePort(&pCtrl->FeederPort)) {
        SvcLog_NG(SVC_LOG_FEX, "Close feeder port error", 0U, 0U);
    }

    /* Wait Rx idle */
    for (i = 0; i < SVC_FEX_SEM_INIT_COUNT; i++) {
        Rval = Fex_SemTake(Inst);
    }

    return Rval;
}

/**
* Send external YUV to driver
*/
static UINT32 Fex_SendExtYUV(UINT32 Inst, const void *pInfo)
{
    UINT32 Rval = SVC_OK;
    SVC_FEX_CTRL_s *pCtrl = &g_FexCtrl[Inst];
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
static void Fex_SrcRateUpdate(SVC_FEX_CTRL_s *pCtrl, UINT32 SrcRate)
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
            (Den <= FEX_SRC_RATE_MAX_DENOMINATOR) && (Num <= Den)) {
            Rval = AmbaWrap_memset(&pCtrl->SrcValidTbl[0], 0, sizeof(pCtrl->SrcValidTbl));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_FEX, "memset SrcValidTbl failed", 0U, 0U);
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
            SvcLog_NG(SVC_LOG_FEX, "Invalid ratio: %u/%u", Num, Den);
        }
    } else {
        /* Divisor */
        if (SrcRate == 0U) {
            pCtrl->SrcRateCtrl = 1U;
        } else {
            pCtrl->SrcRateCtrl = SrcRate;
        }
    }

    SvcLog_OK(SVC_LOG_FEX, "SrcRateCtrl: 0x%x", pCtrl->SrcRateCtrl, 0U);
}


/**
* Entry of configuration function
*/
static UINT32 Fex_Config(UINT32 Inst, UINT32 AlgoIdx)
{
    AmbaMisra_TouchUnused(&Inst);
    AmbaMisra_TouchUnused(&AlgoIdx);

    return SVC_OK;
}

static UINT32 Fex_ConfigBuffer(UINT32 Inst)
{
    UINT32 Rval = SVC_OK;
    SVC_FEX_CTRL_s *pCtrl = &g_FexCtrl[Inst];
    SVC_CV_FLOW_BUF_INFO_s BufInfo;
    UINT32 i;

    /* Img info buffer */
    if (pCtrl->pImgInfoBuf[0] == NULL) {
        for (i = 0; i < SVC_FEX_PICINFO_QUEUE_SIZE; i++) {
            Rval = SvcCvFlow_AllocWorkBuf(sizeof(memio_source_recv_picinfo_t), &BufInfo);
            if (SVC_OK == Rval) {
                AmbaMisra_TypeCast(&pCtrl->pImgInfoBuf[i], &BufInfo.pAddr);
                pCtrl->ImgInfoBufSize = BufInfo.Size;
            } else {
                SvcLog_NG(SVC_LOG_FEX, "Alloc Img info buf failed", 0U, 0U);
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
static UINT32 CvFlow_FexInit(UINT32 CvFlowChan, void *pInit)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    static UINT32 g_FexInit = 0U;

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(pInit);

    if (0U == g_FexInit) {
        g_FexInit = 1U;

        if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&FexMutex, NULL)) {
            SvcLog_NG(SVC_LOG_FEX, "MutexCreate error", 0U, 0U);
            Rval = SVC_NG;
        }
        for (i = 0; i < SVC_FEX_MAX_INSTANCE; i++) {
            g_FexCtrl[i].Used = 0;
        }

        SVC_CV_DBG(SVC_LOG_FEX, "CvFlow_FexInit: first init", 0U, 0U);
    }

    SvcLog_OK(SVC_LOG_FEX, "CvFlow_FexInit: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver configuration function
*/
static UINT32 CvFlow_FexConfig(UINT32 CvFlowChan, void *pConfig)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    SVC_FEX_CTRL_s *pCtrl = NULL;
    const SVC_CV_FLOW_FEX_MODE_INFO_s *pModeInfo;
    const SVC_CV_INPUT_CFG_s *pInputCfg;

    AmbaMisra_TouchUnused(pConfig);

    if (NULL != pConfig) {
        Fex_MutexTake();
        /* Create instance */
        for (i = 0; i < SVC_FEX_MAX_INSTANCE; i++) {
            if (0U == g_FexCtrl[i].Used) {
                Rval = AmbaWrap_memset(&g_FexCtrl[i], 0, sizeof(SVC_FEX_CTRL_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_FEX, "memset g_AmbaODCtrl[%u] failed", i, 0U);
                }
                pCtrl = &g_FexCtrl[i];
                pCtrl->Used = 1;
                SvcLog_DBG(SVC_LOG_FEX, "Create Inst(%u)", i, 0U);
                break;
            }
        }
        Fex_MutexGive();

        /* Configure instance */
        if (NULL != pCtrl) {
            pCtrl->CvfChan     = (UINT8) CvFlowChan;
            pCtrl->pCvFlow     = &pCtrl->CvCfg;
            Rval = AmbaWrap_memcpy(&pCtrl->CvCfg, pConfig, sizeof(SVC_CV_FLOW_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_FEX, "memcpy CvCfg failed", 0U, 0U);
            }
            Fex_SrcRateUpdate(pCtrl, pCtrl->pCvFlow->InputCfg.SrcRate);
            pCtrl->Enable      = 1;

            pModeInfo = &SvcCvFlow_Fex_ModeInfo[pCtrl->pCvFlow->CvModeID];
            pInputCfg = &pCtrl->pCvFlow->InputCfg;
            if (pInputCfg->InputNum <= SVC_CV_FLOW_INPUT_MAX) {
                for (i = 0; i < pInputCfg->InputNum; i++) {
                    if (pInputCfg->Input[i].RoiInfo.NumRoi <= SVC_CV_MAX_ROI) {
                        //for (j = 0; j < pInputCfg->Input[i].RoiInfo.NumRoi; j++) {
                        //    pInputCfg->Input[i].RoiInfo.Roi[j].Width  = pModeInfo->Cfg[0].NetWidth;
                        //    pInputCfg->Input[i].RoiInfo.Roi[j].Height = pModeInfo->Cfg[0].NetHeight;
                        //}
                    } else {
                        Rval = SVC_NG;
                        break;
                    }
                }
            } else {
                Rval = SVC_NG;
            }

            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_FEX, "unsupported InputNum(%u), NumRoi(%u)", pInputCfg->InputNum, pInputCfg->Input[0].RoiInfo.NumRoi);
            } else {
                if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG) {
                    Rval = SVC_OK;
                } else {
                    SvcLog_NG(SVC_LOG_FEX, "unsupported CVFlow ProfID(%u)", pModeInfo->ProfID, 0U);
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
        SvcLog_OK(SVC_LOG_FEX, "CvFlow_FexConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_FEX, "CvFlow_FexConfig: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver load function
*/
static UINT32 CvFlow_FexLoad(UINT32 CvFlowChan, void *pInfo)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_FEX_CTRL_s *pCtrl;
    const SVC_CV_FLOW_LOAD_INFO_s *pLoadInfo;
    const SVC_CV_FLOW_FEX_MODE_INFO_s *pModeInfo;
    UINT32 i;

    AmbaMisra_TouchUnused(pInfo);

    Rval = Fex_CvfChan2Inst(CvFlowChan, &Inst);
    if (Rval == SVC_OK) {
        pCtrl = &g_FexCtrl[Inst];
        if (NULL != pInfo) {
            AmbaMisra_TypeCast(&pLoadInfo, &pInfo);
            if (pLoadInfo->NumBin > 0U) {
                /* Use external path */
                if (pLoadInfo->AlgoIdx < FEX_MAX_ALGO_NUM) {
                    Rval = SvcCvFlow_LoadAlgoBin(SVC_LOG_FEX, pLoadInfo, &pCtrl->AlgoOp[pLoadInfo->AlgoIdx]);
                } else {
                    SvcLog_NG(SVC_LOG_FEX, "CvFlow_FexLoad: CvFlowChan(%d) invalid AlgoIdx(%u)", CvFlowChan, pLoadInfo->AlgoIdx);
                }
            } else {
                /* Use ModeInfo */
                if (pCtrl->pCvFlow->CvModeID < FEX_NUM_MODE) {
                    pModeInfo = &SvcCvFlow_Fex_ModeInfo[pCtrl->pCvFlow->CvModeID];

                    for (i = 0; i < pModeInfo->AlgoGrp.NumAlgo; i++) {
                        Rval |= SvcCvFlow_LoadAlgo(SVC_LOG_FEX, &pModeInfo->AlgoGrp.AlgoInfo[i], &pCtrl->AlgoOp[i]);
                    }
                } else  {
                    SvcLog_NG(SVC_LOG_FEX, "CvFlow_FexLoad: CvFlowChan(%d) invalid CvModeID(%u)", CvFlowChan, pCtrl->pCvFlow->CvModeID);
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_FEX, "CvFlow_FexLoad: invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_FEX, "CvFlow_FexLoad: CvFlowChan(%d) error", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver start function
*/
static UINT32 CvFlow_FexStart(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst, i, j, AlgoIdx, Num;
    SVC_FEX_CTRL_s *pCtrl;
    const SVC_CV_FLOW_FEX_MODE_INFO_s *pModeInfo;
    SVC_CV_FLOW_BUF_INFO_s BufInfo;
    const SVC_CV_FLOW_ALGO_INFO_s *pAlgoInfo;
    SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
    SVC_CV_FLOW_ALGO_QUERY_CFG_s QueryCfg;
    SVC_CV_FLOW_ALGO_CREATE_CFG_s CreateCfg;
    SVC_CV_ALGO_REGCB_CFG_s CbCfg;
    SVC_IMG_FEEDER_PORT_CFG_s PortCfg = {0};
    char Text[2];
    static char CbEvtQueName[] = "EvtQue##_Fex";
    static char SemName[] = "OutBufSem";
    static FEX_OUTPUT_MSG_s AlgoOutQue[SVC_FEX_MAX_INSTANCE][SVC_FEX_ALGO_OUT_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 ODRxTaskStack[SVC_FEX_MAX_INSTANCE][SVC_FEX_RX_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_IMG_FEEDER_EVENT_CALLBACK_f Fex_TxDataSrcHandlerList[SVC_FEX_MAX_INSTANCE] = {
        Fex_TxDataSrcHandler0,
        Fex_TxDataSrcHandler1,
    };
    static SVC_CV_ALGO_CALLBACK_f Fex_AlgoOutCbList[SVC_FEX_MAX_INSTANCE] = {
        Fex_AlgoOutCallback0,
        Fex_AlgoOutCallback1,
    };

    AmbaMisra_TouchUnused(pInput);
    AmbaMisra_TouchUnused(Fex_TxDataSrcHandlerList);
    AmbaMisra_TouchUnused(Fex_AlgoOutCbList);

    Rval = Fex_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl     = &g_FexCtrl[Inst];
        pModeInfo = &SvcCvFlow_Fex_ModeInfo[pCtrl->pCvFlow->CvModeID];

        /* Open Feeder port first to enable frame FIFO earlier */
        if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
            /* The driver supports single input per instance */
            PortCfg.NumPath            = 1U;
            PortCfg.Content[0].DataSrc = pCtrl->pCvFlow->InputCfg.Input[0].DataSrc;
            PortCfg.Content[0].StrmId  = pCtrl->pCvFlow->InputCfg.Input[0].StrmId;
            PortCfg.SendFunc           = Fex_TxDataSrcHandlerList[Inst];
            if (SVC_OK != SvcImgFeeder_OpenPort(pCtrl->pCvFlow->InputCfg.FeederID, &PortCfg, &pCtrl->FeederPort)) {
                SvcLog_NG(SVC_LOG_FEX, "Open feeder port error", 0U, 0U);
            }
        } else {
            if (pCtrl->pCvFlow->InputCfg.Input[0].DataSrc != SVC_CV_DATA_SRC_EXT_DATA) {
                SvcLog_NG(SVC_LOG_FEX, "ImgFeeder is not enabled", 0U, 0U);
            }
        }

        for (AlgoIdx = 0; AlgoIdx < pModeInfo->AlgoGrp.NumAlgo; AlgoIdx++) {
            pAlgoInfo = &pModeInfo->AlgoGrp.AlgoInfo[AlgoIdx];
            pAlgoOp   = &pCtrl->AlgoOp[AlgoIdx];

            Rval = AmbaWrap_memset(&QueryCfg, 0, sizeof(SVC_CV_FLOW_ALGO_QUERY_CFG_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_FEX, "memset QueryCfg failed", 0U, 0U);
            }
            Rval = SvcCvFlow_QueryAlgo(SVC_LOG_FEX, pAlgoInfo, &QueryCfg, pAlgoOp);
            if (SVC_OK == Rval) {
                /* Create algo */
                Rval = AmbaWrap_memset(&CreateCfg, 0, sizeof(SVC_CV_FLOW_ALGO_CREATE_CFG_s));
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_FEX, "memset CreateCfg failed", 0U, 0U);
                }

                if (pModeInfo->ProfID == SVC_CV_FLOW_PROF_FLEXIDAG) {
                    /* Do nothing */
                } else {
                    SvcLog_NG(SVC_LOG_FEX, "unsupported CVFlow ProfID(%u)", pModeInfo->ProfID, 0U);
                    Rval = SVC_NG;
                }

                if (SVC_OK == Rval) {
                    Rval = SvcCvFlow_CreateAlgo(SVC_LOG_FEX, pAlgoInfo, &CreateCfg, pAlgoOp);
                }

                if (SVC_OK == Rval) {
                    /* Create Output buffer */
                    for (i = 0; i < SVC_FEX_OUTPUT_DEPTH; i++) {
                        Num = pAlgoOp->OutputNum;
                        for (j = 0; j < Num; j++) {
                            Rval |= SvcCvFlow_AllocWorkBuf(pAlgoOp->OutputSize[j], &BufInfo);
                            SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_FEX, BufInfo.pAddr, BufInfo.Size, &pCtrl->AlgoOpEx[AlgoIdx].OutputBuf[i][j]);
                        }
                    }

                    if (SVC_OK == Rval) {
                        /* Register Algo callback */
                        Rval = AmbaWrap_memset(&CbCfg, 0, sizeof(SVC_CV_ALGO_REGCB_CFG_s));
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_FEX, "memset CbCfg failed", 0U, 0U);
                        }
                        CbCfg.Mode     = 0U;
                        CbCfg.Callback = Fex_AlgoOutCbList[Inst];
                        Rval = SvcCvAlgo_RegCallback(&pAlgoOp->AlgoHandle, &CbCfg);
                        if (SVC_OK == Rval) {
                            Rval = Fex_Config(Inst, AlgoIdx);
                            if (SVC_OK != Rval) {
                                SvcLog_NG(SVC_LOG_FEX, "FlexiDAG Config error(0x%X)", Rval, 0U);
                            }

                            pCtrl->NumValidAlgo++;
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_FEX, "Alloc Algo[%u] Output Buf failed", AlgoIdx, 0U);
                    }
                } else {
                   SvcLog_NG(SVC_LOG_FEX, "CreateAlgo[%u] error", AlgoIdx, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_FEX, "QueryAlgo[%u] error", AlgoIdx, 0U);
            }
        }

        if (SVC_OK == Rval) {
            Rval = Fex_ConfigBuffer(Inst);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_FEX, "ConfigBuffer error", 0U, 0U);
            }
        }

        if (SVC_OK == Rval) {
            pCtrl->pTxProc = Fex_TxDataSrcHandler;
            pCtrl->pRxProc = Fex_RxDataSinkHandler;

            if (1U != AmbaUtility_UInt32ToStr(&Text[0], sizeof(Text), Inst, 10)) {
                SvcLog_NG(SVC_LOG_FEX, "UInt32ToStr failed", 0U, 0U);
            }
            CbEvtQueName[6] = Text[0];

            Rval = AmbaKAL_MsgQueueCreate(&pCtrl->AlgoOutQueId,
                                           CbEvtQueName,
                                           sizeof(FEX_OUTPUT_MSG_s),
                                           &AlgoOutQue[Inst][0],
                                           SVC_FEX_ALGO_OUT_QUEUE_SIZE * sizeof(FEX_OUTPUT_MSG_s));

            Rval |= AmbaKAL_SemaphoreCreate(&pCtrl->OutBufSem, SemName, SVC_FEX_SEM_INIT_COUNT);

            if (KAL_ERR_NONE == Rval) {
                /* Create RX task */
                pCtrl->RxTaskCtrl.Priority   = 60;
                pCtrl->RxTaskCtrl.EntryFunc  = Fex_RxTaskEntry;
                pCtrl->RxTaskCtrl.EntryArg   = Inst;
                pCtrl->RxTaskCtrl.pStackBase = &ODRxTaskStack[Inst][0];
                pCtrl->RxTaskCtrl.StackSize  = SVC_FEX_RX_TASK_STACK_SIZE;
                pCtrl->RxTaskCtrl.CpuBits    = 0x01U;
                Rval = SvcTask_Create("SvcODRxTask", &pCtrl->RxTaskCtrl);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_FEX, "Create RxTask Inst(%u) error", Inst, 0U);
                }

                /* Start input */
                if (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder) {
                    Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_FEX, "Connect feeder port error", 0U, 0U);
                    }
                }

                if (SVC_OK == Rval) {
                    pCtrl->Started = 1U;
                }
            } else {
                SvcLog_NG(SVC_LOG_FEX, "Create MsgQue/Semaphore error(0x%X)", Rval, 0U);
                Rval =  SVC_NG;
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_FEX, "Invalid CvFlowChan", 0U, 0U);
        Rval =  SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_FEX, "CvFlow_FexStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_FEX, "CvFlow_FexStart: CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver control function
*/
static UINT32 CvFlow_FexControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo)
{
    UINT32 Rval, Inst;
    SVC_FEX_CTRL_s *pCtrl;
    const UINT32 *pValue;

    Fex_MutexTake();

    Rval = Fex_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl = &g_FexCtrl[Inst];
        switch (CtrlType) {
            case SVC_CV_CTRL_ENABLE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->Enable = (UINT8) *pValue;
                if ((1U == pCtrl->Started) &&
                    (1U == pCtrl->pCvFlow->InputCfg.EnableFeeder)) {
                    if (*pValue == 0U) {
                        Rval = SvcImgFeeder_Disconnect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_FEX, "Disconn feeder port error", 0U, 0U);
                        }
                    } else {
                        Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_FEX, "Cnnect feeder port error", 0U, 0U);
                        }
                    }
                }
                break;
            case SVC_CV_CTRL_SET_SRC_RATE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->SrcRateCtrl = *pValue;
                SvcLog_OK(SVC_LOG_FEX, "Control[%d]: SrcRate = %u", Inst, *pValue);
                break;
            case SVC_CV_CTRL_SEND_EXT_YUV:
                Rval = Fex_SendExtYUV(Inst, pInfo);
                break;
            default:
                SvcLog_NG(SVC_LOG_FEX, "Control[%d]: Invalid CtrlType(%d)", Inst, CtrlType);
                break;
        }
    } else {
        SvcLog_NG(SVC_LOG_FEX, "Control: Invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    Fex_MutexGive();

    return Rval;
}

/**
* CvFlow driver stop function
*/
static UINT32 CvFlow_FexStop(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst;
    SVC_FEX_CTRL_s *pCtrl;
    SVC_CV_FLOW_ALGO_DELETE_CFG_s DelCfg;
    UINT32 i;

    AmbaMisra_TouchUnused(pInput);

    Rval = Fex_CvfChan2Inst(CvFlowChan, &Inst);

    if (SVC_OK == Rval) {
        pCtrl = &g_FexCtrl[Inst];

        Rval = Fex_SetTaskIdle(Inst);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_FEX, "SetTaskIdle error", 0U, 0U);
        }

        Rval = SvcTask_Destroy(&pCtrl->RxTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_FEX, "Destroy Tx/Rx Task error", 0U, 0U);
        }

        Rval = AmbaWrap_memset(&DelCfg, 0, sizeof(SVC_CV_FLOW_ALGO_DELETE_CFG_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_FEX, "memset DelCfg failed", 0U, 0U);
        }

        for (i = 0; i < pCtrl->NumValidAlgo; i++) {
            Rval |= SvcCvFlow_DeleteAlgo(SVC_LOG_FEX, &DelCfg, &pCtrl->AlgoOp[i]);
        }
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_FEX, "Delele Algo error", 0U, 0U);
        }

        Rval = AmbaKAL_MsgQueueDelete(&pCtrl->AlgoOutQueId);
        Rval |= AmbaKAL_SemaphoreDelete(&pCtrl->OutBufSem);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_FEX, "Delete MsgQue/Semaphore error(0x%X)", Rval, 0U);
        }

        /* Reset Ctrl Intance */
        Rval = AmbaWrap_memset(&g_FexCtrl[Inst], 0, sizeof(SVC_FEX_CTRL_s));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_FEX, "memset g_FexCtrl failed", 0U, 0U);
        }
    }

    SvcLog_OK(SVC_LOG_FEX, "CvFlow_FexStop: CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver debug interface
*/
static UINT32 CvFlow_FexDebug(UINT32 CvFlowChan, const char *pCmd, UINT32 Param0, UINT32 Param1, UINT32 Param2)
{
    UINT32 i;
    ULONG Addr;
    const SVC_FEX_CTRL_s *pCtrl;
    const AMBA_CV_FLEXIDAG_HANDLE_s *pCvFDHandle;

    if (0 == SvcWrap_strcmp("debug", pCmd)) {
        SvcCvDebugEnable = (UINT8) Param0;
        SvcLog_DBG(SVC_LOG_FEX, "Set debug(%u)", Param0, 0U);
    } else if (0 == SvcWrap_strcmp("info", pCmd)) {
        for (i = 0; i < SVC_FEX_MAX_INSTANCE; i++) {
            pCtrl = &g_FexCtrl[i];
            if (pCtrl->Used == 1U) {
                AmbaPrint_PrintUInt5("Fex[%d]: CvfChan %d", i, pCtrl->CvfChan, 0U, 0U, 0U);
                pCvFDHandle = &pCtrl->AlgoOp[0].AlgoHandle.FDs[0].Handle;
                AmbaMisra_TypeCast(&Addr, &pCvFDHandle);
                SvcCvFlow_PrintULong("  CV_FLEXIDAG_HANDLE: 0x%x", Addr, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        SvcLog_DBG(SVC_LOG_FEX, "Unsupported cmd", 0U, 0U);
    }

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(&Param0);
    AmbaMisra_TouchUnused(&Param1);
    AmbaMisra_TouchUnused(&Param2);

    return SVC_OK;
}

SVC_CV_FLOW_OBJ_s SvcCvFlow_FexObj = {
    .Init         = CvFlow_FexInit,
    .Config       = CvFlow_FexConfig,
    .Load         = CvFlow_FexLoad,
    .Start        = CvFlow_FexStart,
    .Control      = CvFlow_FexControl,
    .Stop         = CvFlow_FexStop,
    .Debug        = CvFlow_FexDebug,
};

