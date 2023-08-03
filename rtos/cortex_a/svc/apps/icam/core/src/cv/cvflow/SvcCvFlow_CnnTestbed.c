/**
 *  @file SvcCvFlow_CnnTestbed.c
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
 *  @details Implementation of CvFlow Driver to hook up cnn_testbed SvcCvAlgo examples
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event.h"
#include "AmbaUtility.h"
#include "AmbaMMU.h"

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
#include "SvcCvFlow_CnnTestbed.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcImgFeeder.h"
#include "SvcOsd.h"
#include "SvcWrap.h"

#include "cvapi_ambacv_flexidag.h"

#define SVC_LOG_TESTBED        "Testbed"

#define CFD_TESTBED_INSTANCE0                (0U)
#define CFD_TESTBED_INSTANCE1                (1U)
#define CFD_TESTBED_INSTANCE2                (2U)
#define CFD_TESTBED_INSTANCE3                (3U)
#define CFD_TESTBED_MAX_INSTANCE             (4U)

#define CFD_TESTBED_SEM_INIT_COUNT           (1U)
#define CFD_TESTBED_OUTPUT_DEPTH             (CFD_TESTBED_SEM_INIT_COUNT)

#define CFD_TESTBED_TX_QUEUE_SIZE            (8U)
#define CFD_TESTBED_RX_MSGQ_SIZE             (8U)
#define CFD_TESTBED_RX_TASK_STACK_SIZE       (0x6000)
#if !defined (CONFIG_SOC_CV28)
#define CFD_TESTBED_OSD_CHANNEL              (1U)     // VOUT_IDX_B (LCD)
#else
#define CFD_TESTBED_OSD_CHANNEL              (0U)     // VOUT_IDX_A (LT9611UXC for CV28)
#endif
#define CFD_TESTBED_OSD_THICKNESS            (3U)

#define CFD_TESTBED_DEFAULT_FEEDER_ID        (0U)

typedef struct {
   SVC_CV_ALGO_OUTPUT_s  CvAlgoOut;
} RX_MSGQ_ITEM_s;

// CvFlow Driver Cnn_testbed Control
typedef struct {
    UINT8                       Used;
    UINT8                       CvfChan;
    SVC_CV_FLOW_CFG_s           CvCfg;

    // CvAlgo
    SVC_CV_FLOW_ALGO_OP_INFO_s  AlgoOp;

    // CnnTestbed Driver Mode Info
    SVC_CV_FLOW_CNN_TESTBED_MODE_INFO_s ModeInfo;

    // Buffer control
    flexidag_memblk_t           ImgInfoBuf[CFD_TESTBED_TX_QUEUE_SIZE];  // Store the memio_source_recv_picinfo_t
    flexidag_memblk_t           OutputBuf[CFD_TESTBED_OUTPUT_DEPTH][FLEXIDAG_MAX_OUTPUTS];
    AMBA_CV_FLEXIDAG_IO_s       InBuf[CFD_TESTBED_OUTPUT_DEPTH];
    AMBA_CV_FLEXIDAG_IO_s       OutBuf[CFD_TESTBED_OUTPUT_DEPTH];
    UINT32                      OutBufIdx;
    AMBA_KAL_SEMAPHORE_t        OutBufSem;    // Semaphore to control the ring buffer access

    // TX
    SVC_IMG_FEEDER_PORT_s       FeederPort;
    UINT32                      TxSeqNum;
    UINT32                      TxPicInfoBufIdx;

    // RX
    SVC_TASK_CTRL_s             RxTaskCtrl;
    AMBA_KAL_MSG_QUEUE_t        RxMsgQId;     // CvAlgo callback -> RxTask
} CFD_TESTBED_CTRL_s;

static CFD_TESTBED_CTRL_s g_CfdTestbedCtrl[CFD_TESTBED_MAX_INSTANCE] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MUTEX_t g_CfdTestbedMutex;  // Mutex to protect the modification to g_CfdTestbedCtrl

static UINT32 FeedData(UINT32 Inst, void *pInfo);
static UINT32 Testbed_PymdDataRdyHdlr0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 Testbed_PymdDataRdyHdlr1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 Testbed_PymdDataRdyHdlr2(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 Testbed_PymdDataRdyHdlr3(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

static void Testbed_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&g_CfdTestbedMutex, 5000)) {
        SvcLog_NG(SVC_LOG_TESTBED, "Testbed_MutexTake: timeout", 0U, 0U);
    }
}

static void Testbed_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&g_CfdTestbedMutex)) {
        SvcLog_NG(SVC_LOG_TESTBED, "Testbed_MutexTake: error", 0U, 0U);
    }
}

static UINT32 Testbed_SemTake(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreTake(&g_CfdTestbedCtrl[Inst].OutBufSem, 10000)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_TESTBED, "Testbed_SemTake[%d]: timeout", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 Testbed_SemGive(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreGive(&g_CfdTestbedCtrl[Inst].OutBufSem)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_TESTBED, "Testbed_SemTake[%d]: error", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 Testbed_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst)
{
    UINT32 Rval = SVC_NG;
    UINT32 i;

    if (NULL != pInst) {
        for (i = 0; i < CFD_TESTBED_MAX_INSTANCE; i++) {
            if ((1U == g_CfdTestbedCtrl[i].Used) && (CvfChan == g_CfdTestbedCtrl[i].CvfChan)) {
                *pInst = i;
                Rval = SVC_OK;
                break;
            }
        }
    }
    return Rval;
}

static UINT32 Testbed_PymdDataRdyHdlr(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    CFD_TESTBED_CTRL_s *pCtrl = &g_CfdTestbedCtrl[Inst];

    AmbaMisra_TouchUnused(&Event);
    /* Feed data by user-specified framerate (feed rate = actual framerate / TxRateCtrl) */
    if(0U == (pCtrl->TxSeqNum % pCtrl->ModeInfo.TxRateCtrl)) {
        Rval = FeedData(Inst, pInfo);
    }
    (void)SvcImgFeeder_MsgToFeeder(pPort, SVC_IMG_FEEDER_RECEIVER_READY, NULL);
    /* Update TxSeqNum for each frame */
    pCtrl->TxSeqNum ++;
    return Rval;
}

static UINT32 Testbed_PymdDataRdyHdlr0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    return Testbed_PymdDataRdyHdlr(CFD_TESTBED_INSTANCE0, pPort, Event, pInfo);
}

static UINT32 Testbed_PymdDataRdyHdlr1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    return Testbed_PymdDataRdyHdlr(CFD_TESTBED_INSTANCE1, pPort, Event, pInfo);
}

static UINT32 Testbed_PymdDataRdyHdlr2(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    return Testbed_PymdDataRdyHdlr(CFD_TESTBED_INSTANCE2, pPort, Event, pInfo);
}

static UINT32 Testbed_PymdDataRdyHdlr3(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    return Testbed_PymdDataRdyHdlr(CFD_TESTBED_INSTANCE3, pPort, Event, pInfo);
}

/**
* Algorithm output event callback function
*/
static UINT32 Testbed_AlgoOutCallbackMain(UINT32 Inst, UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Rval = SVC_OK;
    CFD_TESTBED_CTRL_s *pCtrl = &g_CfdTestbedCtrl[Inst];

    if (Event == CALLBACK_EVENT_OUTPUT) {
        /* 1. Send to Rx MsgQ */
        RX_MSGQ_ITEM_s Msg;

        AmbaSvcWrap_MisraMemcpy(&Msg.CvAlgoOut, pEventData, sizeof(SVC_CV_ALGO_OUTPUT_s));
        Rval = AmbaKAL_MsgQueueSend(&pCtrl->RxMsgQId, &Msg, 5000);
        if (Rval != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_TESTBED, "Inst[%u] Send RxMsgQ error(0x%x)", Inst, Rval);
        }
    }
    return Rval;
}

static UINT32 Testbed_AlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Testbed_AlgoOutCallbackMain(CFD_TESTBED_INSTANCE0, Event, pEventData);
}

static UINT32 Testbed_AlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Testbed_AlgoOutCallbackMain(CFD_TESTBED_INSTANCE1, Event, pEventData);
}

static UINT32 Testbed_AlgoOutCallback2(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Testbed_AlgoOutCallbackMain(CFD_TESTBED_INSTANCE2, Event, pEventData);
}

static UINT32 Testbed_AlgoOutCallback3(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Testbed_AlgoOutCallbackMain(CFD_TESTBED_INSTANCE3, Event, pEventData);
}

/**
* Pack dsp YUV info to pic info structure
*/
static void PackToPicInfo(memio_source_recv_picinfo_t *pInfo, const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo)
{
    if ((pInfo != NULL) && (pYuvInfo != NULL)) {
        AmbaSvcWrap_MisraMemset(pInfo, 0, sizeof(memio_source_recv_picinfo_t));
        pInfo->pic_info.frame_num               = (UINT32) pYuvInfo->CapSequence;
        pInfo->pic_info.capture_time            = (UINT32) pYuvInfo->CapPts;
        pInfo->pic_info.pyramid.image_width_m1  = pYuvInfo->YuvBuf[0U].Window.Width - 1U;
        pInfo->pic_info.pyramid.image_height_m1 = pYuvInfo->YuvBuf[0U].Window.Height - 1U;
        pInfo->pic_info.pyramid.image_pitch_m1  = (UINT32) pYuvInfo->YuvBuf[0U].Pitch - 1U;

        for (UINT32 i = 0U; i < AMBA_DSP_MAX_HIER_NUM; i++) {
            ULONG TempPhyAddr;
            pInfo->pic_info.pyramid.half_octave[i].ctrl.disable   = 0U;
            pInfo->pic_info.pyramid.half_octave[i].ctrl.roi_pitch = pYuvInfo->YuvBuf[i].Pitch;
            pInfo->pic_info.pyramid.half_octave[i].roi_width_m1   = pYuvInfo->YuvBuf[i].Window.Width - 1U;
            pInfo->pic_info.pyramid.half_octave[i].roi_height_m1  = pYuvInfo->YuvBuf[i].Window.Height - 1U;
            pInfo->pic_info.pyramid.half_octave[i].roi_start_col  = 0;
            pInfo->pic_info.pyramid.half_octave[i].roi_start_row  = 0;
            (void) AmbaMMU_VirtToPhys(pYuvInfo->YuvBuf[i].BaseAddrY, &TempPhyAddr);
            pInfo->pic_info.rpLumaLeft[i]                         = (UINT32)TempPhyAddr;
            pInfo->pic_info.rpLumaRight[i]                        = (UINT32)TempPhyAddr;
            (void) AmbaMMU_VirtToPhys(pYuvInfo->YuvBuf[i].BaseAddrUV, &TempPhyAddr);
            pInfo->pic_info.rpChromaLeft[i]                       = (UINT32)TempPhyAddr;
            pInfo->pic_info.rpChromaRight[i]                      = (UINT32)TempPhyAddr;
        }
    } else {
        SvcLog_NG(SVC_LOG_TESTBED, "PackToPicInfo error for null input", 0U, 0U);
    }
}


/**
* TX data source handler: process input data
*/
static UINT32 FeedData(UINT32 Inst, void *pInfo)
{
    UINT32 Rval;
    CFD_TESTBED_CTRL_s *pCtrl = &g_CfdTestbedCtrl[Inst];
    const SVC_CV_INPUT_IMG_INFO_s *pInputImg;
    const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo;
    memio_source_recv_picinfo_t *pPicInfo;

    (void) pInfo;

    /* 1. Take Semaphore (Make sure we have freed buffers) */
    Rval = Testbed_SemTake(Inst);

    if (SVC_OK == Rval) {
        /* 2. Get pyramid data from pInfo */
        AmbaMisra_TypeCast(&pInputImg, &pInfo);
        AmbaMisra_TypeCast(&pYuvInfo, &pInputImg->Info[0].pBase);
        /* 3. Pack the pyramid data to memio_source_recv_picinfo_t format */
        AmbaMisra_TypeCast(&pPicInfo, &pCtrl->ImgInfoBuf[pCtrl->TxPicInfoBufIdx].pBuffer);
        PackToPicInfo(pPicInfo, pYuvInfo);

        {
            AMBA_CV_FLEXIDAG_IO_s *pInBuf, *pOutBuf;
            SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
            SVC_CV_ALGO_FEED_CFG_s FeedCfg;
            UINT32 i;

            /* 4-1. Pack to AMBA_CV_FLEXIDAG_IO_s format for inputs */
            pAlgoOp = &pCtrl->AlgoOp;
            pInBuf = &pCtrl->InBuf[pCtrl->OutBufIdx];
            pOutBuf = &pCtrl->OutBuf[pCtrl->OutBufIdx];
            SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_TESTBED, pPicInfo, sizeof(memio_source_recv_picinfo_t), &pInBuf->buf[0]);
            pInBuf->num_of_buf = 1U;  // For picinfo mode, the input number is always 1.

            /* 4-2. Calculate offset for picinfo
               (The linux app "cvflow_comm" also do the calculation so we only need to do this on rtos side) */
            {
                UINT32 U32DataIn;
                memio_source_recv_picinfo_t *pDataIn;

                //pDataIn = (memio_source_recv_picinfo_t *) pInBuf->buf[0].pBuffer;
                AmbaMisra_TypeCast(&pDataIn, &pInBuf->buf[0].pBuffer);

                U32DataIn = pInBuf->buf[0].buffer_daddr;
                for (i = 0U ; i < MAX_HALF_OCTAVES ; i++) {
                    pDataIn->pic_info.rpLumaLeft[i] -= U32DataIn;
                    pDataIn->pic_info.rpLumaRight[i] -= U32DataIn;
                    pDataIn->pic_info.rpChromaLeft[i] -= U32DataIn;
                    pDataIn->pic_info.rpChromaRight[i] -= U32DataIn;
                }
            }

            /* 4-3. Pack to AMBA_CV_FLEXIDAG_IO_s format for outputs */
            for (i = 0U; i < pAlgoOp->OutputNum; i++) {
                AmbaSvcWrap_MisraMemcpy(&pOutBuf->buf[i], &pCtrl->OutputBuf[pCtrl->OutBufIdx][i], sizeof(flexidag_memblk_t));
            }
            pOutBuf->num_of_buf = pAlgoOp->OutputNum;

            /* 4-4. Call SvcCvAlgo_Feed() to feed data */
            FeedCfg.pIn  = pInBuf;
            FeedCfg.pOut = pOutBuf;
            FeedCfg.pUserData = NULL;
            FeedCfg.pExtFeedCfg = NULL;

            Rval = SvcCvAlgo_Feed(&pAlgoOp->AlgoHandle, &FeedCfg);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_TESTBED, "Tx[%u] SvcCvAlgo_Feed() error(0x%x)", Inst, Rval);
            }
        }

        /* 5. Update index */
        pCtrl->TxPicInfoBufIdx ++;
        if (pCtrl->TxPicInfoBufIdx >= CFD_TESTBED_TX_QUEUE_SIZE) {
            pCtrl->TxPicInfoBufIdx = 0;
        }
        pCtrl->OutBufIdx++;
        if (pCtrl->OutBufIdx >= CFD_TESTBED_OUTPUT_DEPTH) {
            pCtrl->OutBufIdx = 0;
        }

    } else {
        SvcLog_NG(SVC_LOG_TESTBED, "Tx[%u]: SemTake error(0x%x)", Inst, Rval);
    }

    return Rval;
}

static void SsdDisplay(const SVC_CV_ALGO_OUTPUT_s *pEventData, UINT32 OsdClear, UINT32 OsdFlush)
{
    UINT32 Rval = SVC_OK;
    const CVALGO_SSD_OUTPUT_s *pCvAlgoSsdOutput;
    const CVALGO_SSD_BOX_s *pCurBox;

    /* 1. Sanity check for parameters */
    if (pEventData->pOutput == NULL) {
        SvcLog_NG(SVC_LOG_TESTBED, "SsdDisplay: pOutput is null", 0U, 0U);
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        /* 2. Clear OSD */
        if (OsdClear != 0U) {
          (void) SvcOsd_Clear(CFD_TESTBED_OSD_CHANNEL);
        }

        /* 3. Draw Rectangle */
        //pCvAlgoSsdOutput = (CVALGO_SSD_OUTPUT_s *)pEventData->pOutput->buf[0].pBuffer;
        AmbaMisra_TypeCast(&pCvAlgoSsdOutput, &pEventData->pOutput->buf[0].pBuffer);
        for (UINT32 i = 0; i < pCvAlgoSsdOutput->BoxNum; i ++) {
#if 0   // DEBUG LOG
            SvcLog_DBG(SVC_LOG_TESTBED, "[%u] Detect Class %u ", i, pCvAlgoSsdOutput->Box[i].Class);
            SvcLog_DBG(SVC_LOG_TESTBED, "    (X,Y) = (%u,%u)", pCvAlgoSsdOutput->Box[i].X, pCvAlgoSsdOutput->Box[i].Y);
            SvcLog_DBG(SVC_LOG_TESTBED, "    (W,H) = (%u,%u)", pCvAlgoSsdOutput->Box[i].W, pCvAlgoSsdOutput->Box[i].H);
#endif
            pCurBox = &pCvAlgoSsdOutput->Box[i];
            (void) SvcOsd_DrawRect(CFD_TESTBED_OSD_CHANNEL, pCurBox->X, pCurBox->Y, pCurBox->X + pCurBox->W, pCurBox->Y + pCurBox->H,
                pCurBox->Class, CFD_TESTBED_OSD_THICKNESS);

        }

        /* 4. Flush OSD */
        if (OsdFlush != 0U) {
          (void) SvcOsd_Flush(CFD_TESTBED_OSD_CHANNEL);
        }
    }

    return;
}

static void SegDisplay(UINT32 Inst, const SVC_CV_ALGO_OUTPUT_s *pEventData, UINT32 OsdClear, UINT32 OsdFlush)
{
    UINT32 Rval = SVC_OK;
    const CFD_TESTBED_CTRL_s *pCtrl = &g_CfdTestbedCtrl[Inst];
    const UINT8 *pArgMaxOut;

    /* 1. Sanity check for parameters */
    if (pEventData->pOutput == NULL) {
        SvcLog_NG(SVC_LOG_TESTBED, "SegDisplay: pOutput is null", 0U, 0U);
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        /* 2. Clear OSD */
        if (OsdClear != 0U) {
          (void) SvcOsd_Clear(CFD_TESTBED_OSD_CHANNEL);
        }

        /* 3. Draw segmentation */
        //pArgMaxOut    = (UINT8 *) pEventData->pOutput->buf[0].pBuffer;
        AmbaMisra_TypeCast(&pArgMaxOut, &pEventData->pOutput->buf[0].pBuffer);
        (void) SvcOsd_DrawSeg(
            CFD_TESTBED_OSD_CHANNEL,
            pArgMaxOut,
            pCtrl->ModeInfo.SegWinCtrl.RoiWidth,
            pCtrl->ModeInfo.SegWinCtrl.RoiHeight,
            pCtrl->ModeInfo.SegWinCtrl.RoiStartX,
            pCtrl->ModeInfo.SegWinCtrl.RoiStartY,
            pCtrl->ModeInfo.SegWinCtrl.NetworkWidth,
            pCtrl->ModeInfo.SegWinCtrl.NetworkHeight
        );

        /* 4. Flush OSD */
        if (OsdFlush != 0U) {
          (void) SvcOsd_Flush(CFD_TESTBED_OSD_CHANNEL);
        }
    }

    return;
}

/**
* RX data sink handler: process detection output
*/
static UINT32 ProcessResult(UINT32 Inst)
{
    UINT32 Rval;
    CFD_TESTBED_CTRL_s *pCtrl = &g_CfdTestbedCtrl[Inst];
    SVC_CV_ALGO_OUTPUT_s CvAlgoOut;

    {
        /* 1. Get CvAlgo result from MessageQ */
        RX_MSGQ_ITEM_s Msg;

        Rval = AmbaKAL_MsgQueueReceive(&pCtrl->RxMsgQId, &Msg, AMBA_KAL_WAIT_FOREVER);
        CvAlgoOut = Msg.CvAlgoOut;
    }


    if (SVC_OK == Rval) {
        /* 2. Parse the result */

        UINT32 OsdClear = 1U, OsdFlush = 1U;

        // Only do OsdClear for lowest OsdPriority and OsdFlush for highest OsdPriority
        for (UINT32 i = 0; i < CFD_TESTBED_MAX_INSTANCE; i++) {
            if ( (i != Inst ) && (1U == g_CfdTestbedCtrl[i].Used)) {
                if (pCtrl->ModeInfo.OsdPriority < g_CfdTestbedCtrl[i].ModeInfo.OsdPriority) {
                    OsdFlush = 0U;
                }
                if (pCtrl->ModeInfo.OsdPriority > g_CfdTestbedCtrl[i].ModeInfo.OsdPriority) {
                    OsdClear = 0U;
                }
            }
        }

        switch (pCtrl->CvCfg.CvModeID) {
            case TESTBED_CVALGO_SSD:
                SsdDisplay(&CvAlgoOut, OsdClear, OsdFlush);
                break;
            case TESTBED_CVALGO_SEG:
                SegDisplay(Inst, &CvAlgoOut, OsdClear, OsdFlush);
                break;
            default:
                SvcLog_NG(SVC_LOG_TESTBED, "Rx[%u]: Unsupported CvModeID %u", Inst, pCtrl->CvCfg.CvModeID);
                break;
        }

        /* 3. Give Semaphore */
        Rval = Testbed_SemGive(Inst);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_TESTBED, "Rx[%u]: SemGive error(0x%x)", Inst, Rval);
        }

    } else {
        SvcLog_NG(SVC_LOG_TESTBED, "Rx[%u]: Recieve RxMsgQ error(0x%x)", Inst, Rval);
    }

    return Rval;
}

/**
* Task entry of RX process
*/
static void* TestbedRxTaskEntry(void* EntryArg)
{
    const ULONG  *pArg;
    UINT32 Rval = SVC_OK, Inst;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Inst = (UINT32)(*pArg);

    SvcLog_DBG(SVC_LOG_TESTBED, "RxTaskEntry[%d] start", Inst, 0U);

    while (SVC_OK == Rval) {
        Rval = ProcessResult(Inst);
    }

    SvcLog_DBG(SVC_LOG_TESTBED, "RxTaskEntry[%d] exit", Inst, 0U);

    return NULL;
}

/**
* CvFlow driver initialization function (pInit : Not used)
*/
static UINT32 CvFlow_TestbedInit(UINT32 CvFlowChan, void *pInit)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    static UINT32 g_TestbedInit = 0U;

    (void) CvFlowChan;
    (void) pInit;

    if (0U == g_TestbedInit) {
        g_TestbedInit = 1U;

        /* 1. Create mutex */
        if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&g_CfdTestbedMutex, NULL)) {
            SvcLog_NG(SVC_LOG_TESTBED, "Init(): MutexCreate error", 0U, 0U);
            Rval = SVC_NG;
        }

        /* 2. Init TESTBED_CFD_CTRL_s */
        for (i = 0; i < CFD_TESTBED_MAX_INSTANCE; i++) {
            g_CfdTestbedCtrl[i].Used = 0;
        }

        SvcLog_DBG(SVC_LOG_TESTBED, "Init(): first init", 0U, 0U);
    }

    SvcLog_OK(SVC_LOG_TESTBED, "Init(): CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver configuration function (pConfig : SVC_CV_FLOW_CFG_s)
*/
static UINT32 CvFlow_TestbedConfig(UINT32 CvFlowChan, void *pConfig)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    CFD_TESTBED_CTRL_s *pCtrl = NULL;

    (void) pConfig;

    if (NULL != pConfig) {
        Testbed_MutexTake();
        /* 1. Create instance */
        for (i = 0; i < CFD_TESTBED_MAX_INSTANCE; i++) {
            if (0U == g_CfdTestbedCtrl[i].Used) {
                AmbaSvcWrap_MisraMemset(&g_CfdTestbedCtrl[i], 0, sizeof(CFD_TESTBED_CTRL_s));
                pCtrl = &g_CfdTestbedCtrl[i];
                pCtrl->Used = 1;
                SvcLog_DBG(SVC_LOG_TESTBED, "Config(): CvFlowChan(%u) Create Inst(%u)", CvFlowChan, i);
                break;
            }
        }
        Testbed_MutexGive();

        /* 2. Configure TESTBED_CFD_CTRL_s */
        if (NULL != pCtrl) {
            pCtrl->CvfChan     = (UINT8) CvFlowChan;
            AmbaSvcWrap_MisraMemcpy(&pCtrl->CvCfg, pConfig, sizeof(SVC_CV_FLOW_CFG_s));
            AmbaSvcWrap_MisraMemcpy(&pCtrl->ModeInfo, &SvcCvFlow_CnnTestbed_ModeInfo[pCtrl->CvCfg.CvModeID], sizeof(SVC_CV_FLOW_CNN_TESTBED_MODE_INFO_s));

            // Reset index
            pCtrl->OutBufIdx       = 0;
            pCtrl->TxSeqNum        = 0;
            pCtrl->TxPicInfoBufIdx = 0;
        } else {
            Rval = SVC_NG;
        }
    } else {
        Rval = SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_TESTBED, "Config(): CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_TESTBED, "Config(): CvFlowChan(%u) done", CvFlowChan, 0U);
    }
    return Rval;
}

/**
* CvFlow driver load function (pInfo : SVC_CV_FLOW_LOAD_INFO_s)
*/
static UINT32 CvFlow_TestbedLoad(UINT32 CvFlowChan, void *pInfo)
{
    UINT32 Rval;
    UINT32 Inst;
    CFD_TESTBED_CTRL_s *pCtrl;
    const SVC_CV_FLOW_LOAD_INFO_s *pLoadInfo;

    (void) pInfo;

    /* 1. Get Instance index from CvFlowChan */
    Rval = Testbed_CvfChan2Inst(CvFlowChan, &Inst);
    if (Rval == SVC_OK) {
        pCtrl = &g_CfdTestbedCtrl[Inst];

        /* 2. Load flexbin by given SVC_CV_FLOW_LOAD_INFO_s */
        if (NULL != pInfo) {
            AmbaMisra_TypeCast(&pLoadInfo, &pInfo);
            if (pLoadInfo->NumBin > 0U) {
                Rval = SvcCvFlow_LoadAlgoBin(SVC_LOG_TESTBED, pLoadInfo, &pCtrl->AlgoOp);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_TESTBED, "Load[%u]: LoadAlgoBin error", Inst, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_TESTBED, "Load[%u]: NumBin is 0", Inst, 0U);
                Rval = SVC_NG;
            }
        } else {
            SvcLog_NG(SVC_LOG_TESTBED, "Load[%u]: pInfo is null", Inst, 0U);
            Rval = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_TESTBED, "Load(): invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_TESTBED, "Load(): CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_TESTBED, "Load(): CvFlowChan(%u) done", CvFlowChan, 0U);
    }
    return Rval;
}

/**
* CvFlow driver start function (pInput : Not used)
*/
static UINT32 CvFlow_TestbedStart(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst;
    CFD_TESTBED_CTRL_s *pCtrl;

    // MsgQ, Task and Semaphore related variables
    static char RxMsgQName[CFD_TESTBED_MAX_INSTANCE][16] = {"RxMsgQ0", "RxMsgQ1", "RxMsgQ2", "RxMsgQ3"};
    static char SemName[] = "OutBufSem";
    static RX_MSGQ_ITEM_s RxMsgQBuf[CFD_TESTBED_MAX_INSTANCE][CFD_TESTBED_RX_MSGQ_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 RxTaskStack[CFD_TESTBED_MAX_INSTANCE][CFD_TESTBED_RX_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_CV_ALGO_CALLBACK_f g_CfdTestbedAlgoOutCbList[CFD_TESTBED_MAX_INSTANCE] = {
        Testbed_AlgoOutCallback0,
        Testbed_AlgoOutCallback1,
        Testbed_AlgoOutCallback2,
        Testbed_AlgoOutCallback3
    };
    static SVC_IMG_FEEDER_EVENT_CALLBACK_f g_CfdTestbedDataRdyHdlrList[CFD_TESTBED_MAX_INSTANCE] = {
        Testbed_PymdDataRdyHdlr0,
        Testbed_PymdDataRdyHdlr1,
        Testbed_PymdDataRdyHdlr2,
        Testbed_PymdDataRdyHdlr3
    };

    (void) pInput;

    /* 1. Get Instance index from CvFlowChan */
    Rval = Testbed_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl = &g_CfdTestbedCtrl[Inst];

        /* Allocate buffer for pImgInfoBuf */
        {
            UINT32 i;
            SVC_CV_FLOW_BUF_INFO_s BufInfo;
            for (i = 0; i < CFD_TESTBED_TX_QUEUE_SIZE; i++) {
                Rval = SvcCvFlow_AllocWorkBuf(sizeof(memio_source_recv_picinfo_t), &BufInfo);
                if (SVC_OK == Rval) {
                    SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_TESTBED, BufInfo.pAddr, BufInfo.Size, &pCtrl->ImgInfoBuf[i]);
                } else {
                    SvcLog_NG(SVC_LOG_TESTBED, "Alloc Img info buf failed", 0U, 0U);
                    Rval = SVC_NG;
                    break;
                }
            }
        }

        // Create SvcCvAlgo
        {
            SVC_CV_FLOW_CNN_TESTBED_MODE_INFO_s *pModeInfo;
            SVC_CV_FLOW_ALGO_INFO_s AlgoInfo;
            SVC_CV_ALGO_REGCB_CFG_s CbCfg;
            SVC_CV_ALGO_CTRL_CFG_s CtrlCfg;
            SVC_CV_FLOW_BUF_INFO_s BufInfo;
            SVC_CV_FLOW_ALGO_OP_INFO_s *pAlgoOp;
            SVC_CV_FLOW_ALGO_QUERY_CFG_s QueryCfg;
            SVC_CV_FLOW_ALGO_CREATE_CFG_s CreateCfg;
            UINT32 i, j, Num;

            /* 2. Create CvAlgo */
            pModeInfo = &pCtrl->ModeInfo;

            AmbaSvcWrap_MisraMemset(&AlgoInfo, 0, sizeof(SVC_CV_FLOW_ALGO_INFO_s));
            AlgoInfo.pAlgoObj = pModeInfo->pAlgoObj;

            AmbaSvcWrap_MisraMemset(&QueryCfg, 0, sizeof(SVC_CV_FLOW_ALGO_QUERY_CFG_s));
            Rval = SvcCvFlow_QueryAlgo(SVC_LOG_TESTBED, &AlgoInfo, &QueryCfg, &pCtrl->AlgoOp);
            if (SVC_OK == Rval) {
                AmbaSvcWrap_MisraMemset(&CreateCfg, 0, sizeof(SVC_CV_FLOW_ALGO_CREATE_CFG_s));
                Rval = SvcCvFlow_CreateAlgo(SVC_LOG_TESTBED, &AlgoInfo, &CreateCfg, &pCtrl->AlgoOp);

                if (SVC_OK == Rval) {
                    /* 3. Allocate output buffers */
                    pAlgoOp = &pCtrl->AlgoOp;
                    for (i = 0; i < CFD_TESTBED_OUTPUT_DEPTH; i++) {
                        Num = pAlgoOp->OutputNum;
                        for (j = 0; j < Num; j++) {
                            Rval |= SvcCvFlow_AllocWorkBuf(pAlgoOp->OutputSize[j], &BufInfo);
                            SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_TESTBED, BufInfo.pAddr, BufInfo.Size, &pCtrl->OutputBuf[i][j]);
                        }
                    }

                    /* 4. Register cvalgo callback */
                    if (SVC_OK == Rval) {
                        AmbaSvcWrap_MisraMemset(&CbCfg, 0, sizeof(SVC_CV_ALGO_REGCB_CFG_s));
                        CbCfg.Mode     = 0U;
                        CbCfg.Callback = g_CfdTestbedAlgoOutCbList[Inst];
                        Rval = SvcCvAlgo_RegCallback(&pAlgoOp->AlgoHandle, &CbCfg);
                    } else {
                        SvcLog_NG(SVC_LOG_TESTBED, "Start[%u]: Alloc Algo Output Buf failed", Inst, 0U);
                    }

                    /* 5. Send Extra CvAlgo Control */
                    if (SVC_OK == Rval) {
                        switch (pCtrl->CvCfg.CvModeID) {
                            case TESTBED_CVALGO_SSD:
                                AmbaSvcWrap_MisraMemset(&CtrlCfg, 0, sizeof(SVC_CV_ALGO_CTRL_CFG_s));
                                CtrlCfg.CtrlType   = CTRL_TYPE_SSD_NMS;
                                CtrlCfg.pCtrlParam = &pModeInfo->SsdNmsCtrl;
                                Rval = SvcCvAlgo_Control(&pAlgoOp->AlgoHandle, &CtrlCfg);
                                break;
                            case TESTBED_CVALGO_SEG:
                            default:
                                // No extra control
                                break;
                        }

                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_TESTBED, "Start[%u]: Send CvAlgo Control failed", Inst, 0U);
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_TESTBED, "Start[%u]: RegCallback failed", Inst, 0U);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_TESTBED, "Start[%u]: CreateAlgo error", Inst, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_TESTBED, "Start[%u]: QueryAlgo error", Inst, 0U);
            }
        }

        if (SVC_OK == Rval) {
            /* 6. Create MsgQ, Semaphore */
            Rval |= AmbaKAL_MsgQueueCreate(&pCtrl->RxMsgQId,
                                          RxMsgQName[Inst],
                                          sizeof(RX_MSGQ_ITEM_s),
                                          &RxMsgQBuf[Inst][0],
                                          CFD_TESTBED_RX_MSGQ_SIZE * sizeof(RX_MSGQ_ITEM_s));

            Rval |= AmbaKAL_SemaphoreCreate(&pCtrl->OutBufSem, SemName, CFD_TESTBED_SEM_INIT_COUNT);

            /* 7. Create Rx Task */
            if (KAL_ERR_NONE == Rval) {
                pCtrl->RxTaskCtrl.Priority   = 125;
                pCtrl->RxTaskCtrl.EntryFunc  = TestbedRxTaskEntry;
                pCtrl->RxTaskCtrl.EntryArg   = Inst;
                pCtrl->RxTaskCtrl.pStackBase = &RxTaskStack[Inst][0];
                pCtrl->RxTaskCtrl.StackSize  = CFD_TESTBED_RX_TASK_STACK_SIZE;
                pCtrl->RxTaskCtrl.CpuBits    = 0x01U;
                Rval = SvcTask_Create("TestbedRxTask", &pCtrl->RxTaskCtrl);
            } else {
                SvcLog_NG(SVC_LOG_TESTBED, "Start[%u]: Create MsgQue/Semaphore error(0x%X)", Inst, Rval);
                Rval =  SVC_NG;
            }

            /* 8. Register Image Feeder Callback(pyramid event handler) */
            if (SVC_OK == Rval) {
                SVC_IMG_FEEDER_CREATE_CFG_s CreateCfg = {0};

                CreateCfg.TaskPriority = 80U;
                Rval = SvcImgFeeder_Create(CFD_TESTBED_DEFAULT_FEEDER_ID, &CreateCfg);
                if (SVC_NG == Rval) {
                    SvcLog_NG(SVC_LOG_TESTBED, "Start[%u]: ImgFeederCreate error", Inst, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_TESTBED, "Start[%u]: TaskCreate error", Inst, 0U);
            }

            if (SVC_OK == Rval) {
                SVC_IMG_FEEDER_PORT_CFG_s PortCfg = {0};

                PortCfg.NumPath            = 1U;
                PortCfg.Content[0].DataSrc = SVC_CV_DATA_SRC_PYRAMID;
                PortCfg.Content[0].StrmId  = 0U;  // Fov 0
                PortCfg.SendFunc           = g_CfdTestbedDataRdyHdlrList[Inst];
                Rval = SvcImgFeeder_OpenPort(CFD_TESTBED_DEFAULT_FEEDER_ID, &PortCfg, &pCtrl->FeederPort);
                if (SVC_NG == Rval) {
                    SvcLog_NG(SVC_LOG_TESTBED, "Start[%u]: ImgFeederOpen error", Inst, 0U);
                }
            }

            if (SVC_OK == Rval) {
                Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                if (SVC_NG == Rval) {
                    SvcLog_NG(SVC_LOG_TESTBED, "Start[%u]: ImgFeederConnect error", Inst, 0U);
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_TESTBED, "Start(): Invalid CvFlowChan (%u)", CvFlowChan, 0U);
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_TESTBED, "Start(): CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_TESTBED, "Start(): CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver control function
*/
static UINT32 CvFlow_TestbedControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo)
{
    UINT32 Rval, Inst;
    CFD_TESTBED_CTRL_s *pCtrl;
    const UINT32 *pValue;
    const SSD_NMS_CTRL_s *pSsdNms;
    const SEG_WIN_CTRL_s *pSegWin;

    Testbed_MutexTake();

    /* 1. Get Instance index from CvFlowChan */
    Rval = Testbed_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl = &g_CfdTestbedCtrl[Inst];

        /* 2. Execute the control */
        switch (CtrlType) {
            case SVC_CV_CTRL_SET_SRC_RATE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->ModeInfo.TxRateCtrl = *pValue;
                SvcLog_OK(SVC_LOG_TESTBED, "Control[%d]: SrcRate = %u", Inst, *pValue);
                break;
            case TESTBED_CVALGO_CTRL_SET_SSD_NMS:
                AmbaSvcWrap_MisraMemcpy(&pCtrl->ModeInfo.SsdNmsCtrl, pInfo, sizeof(SSD_NMS_CTRL_s));
                pSsdNms = &pCtrl->ModeInfo.SsdNmsCtrl;
                SvcLog_OK(SVC_LOG_TESTBED, "Control[%d]:", Inst, 0U);
                SvcLog_OK(SVC_LOG_TESTBED, "    ClassNum = %u, ModelType = %u", pSsdNms->ClassNum, pSsdNms->ModelType);
                SvcLog_OK(SVC_LOG_TESTBED, "    (RoiW, RoiH) = (%u, %u)", pSsdNms->Win.RoiWidth, pSsdNms->Win.RoiHeight);
                SvcLog_OK(SVC_LOG_TESTBED, "    (RoiX, RoiY) = (%u, %u)", pSsdNms->Win.RoiStartX, pSsdNms->Win.RoiStartY);
                SvcLog_OK(SVC_LOG_TESTBED, "    (NetW, NetH) = (%u, %u)", pSsdNms->Win.NetworkWidth, pSsdNms->Win.NetworkHeight);
                SvcLog_OK(SVC_LOG_TESTBED, "    (VisW, VisH) = (%u, %u)", pSsdNms->Win.VisWidth, pSsdNms->Win.VisHeight);
                break;
            case TESTBED_CVALGO_CTRL_SET_SEG_WIN:
                AmbaSvcWrap_MisraMemcpy(&pCtrl->ModeInfo.SegWinCtrl, pInfo, sizeof(SEG_WIN_CTRL_s));
                pSegWin = &pCtrl->ModeInfo.SegWinCtrl;
                SvcLog_OK(SVC_LOG_TESTBED, "Control[%d]:", Inst, 0U);
                SvcLog_OK(SVC_LOG_TESTBED, "    (RoiW, RoiH) = (%u, %u)", pSegWin->RoiWidth, pSegWin->RoiHeight);
                SvcLog_OK(SVC_LOG_TESTBED, "    (RoiX, RoiY) = (%u, %u)", pSegWin->RoiStartX, pSegWin->RoiStartY);
                SvcLog_OK(SVC_LOG_TESTBED, "    (NetW, NetH) = (%u, %u)", pSegWin->NetworkWidth, pSegWin->NetworkHeight);
                break;
            default:
                SvcLog_NG(SVC_LOG_TESTBED, "Control[%d]: Invalid CtrlType(%d)", Inst, CtrlType);
                break;
        }
    } else {
        SvcLog_NG(SVC_LOG_TESTBED, "Control(): Invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    Testbed_MutexGive();

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_TESTBED, "Control(): CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_TESTBED, "Control(): CvFlowChan(%u) done", CvFlowChan, 0U);
    }
    return Rval;
}

/**
* CvFlow driver stop function
*/
static UINT32 CvFlow_TestbedStop(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst;
    CFD_TESTBED_CTRL_s *pCtrl;
    UINT32 IsExit = 0;
    SVC_CV_FLOW_ALGO_DELETE_CFG_s DelCfg;

    (void)pInput;

    /* 1. Get Instance index from CvFlowChan */
    Rval = Testbed_CvfChan2Inst(CvFlowChan, &Inst);

    if (SVC_OK == Rval) {
        pCtrl = &g_CfdTestbedCtrl[Inst];

        /* 2. Disable ImgFeeder */
        Rval = SvcImgFeeder_Disconnect(&pCtrl->FeederPort);
        Rval |= SvcImgFeeder_ClosePort(&pCtrl->FeederPort);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_TESTBED, "Stop[%u]: ImgFeeder Disconnect/ClosePort error(0x%X)", Inst, Rval);
        }

        /* 3. Wait until all Semaphore are freed (a.k.a Tx/RX are idle) */
        while (IsExit == 0U) {
            AMBA_KAL_SEMAPHORE_INFO_s SemaphoreInfo;

            Rval = AmbaKAL_SemaphoreQuery(&pCtrl->OutBufSem, &SemaphoreInfo);
            if (SVC_OK == Rval) {
                if (SemaphoreInfo.CurrentCount == CFD_TESTBED_SEM_INIT_COUNT) {
                    IsExit = 1;
                } else {
                    (void) AmbaKAL_TaskSleep(10);
                }
            } else {
                SvcLog_NG(SVC_LOG_TESTBED, "Stop[%u]: SemaphoreQuery error(0x%X)", Inst, Rval);
                IsExit = 1;
            }
        }

        /* 4. Destroy Tx/Rx Task */
        Rval = SvcTask_Destroy(&pCtrl->RxTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_TESTBED, "Stop[%u]: Destroy Tx/Rx Task error(0x%X)", Inst, Rval);
        }

        /* 5. Delete MsgQ and Semaphore */
        Rval = AmbaKAL_MsgQueueDelete(&pCtrl->RxMsgQId);
        Rval |= AmbaKAL_SemaphoreDelete(&pCtrl->OutBufSem);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_TESTBED, "Stop[%u]: Delete MsgQue/Semaphore error(0x%X)", Inst, Rval);
        }

        /* 6. Delete CvAlgo */
        AmbaSvcWrap_MisraMemset(&DelCfg, 0, sizeof(SVC_CV_FLOW_ALGO_DELETE_CFG_s));
        Rval = SvcCvFlow_DeleteAlgo(SVC_LOG_TESTBED, &DelCfg, &pCtrl->AlgoOp);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_TESTBED, "Stop[%u]: DeleteAlgo error ", Inst, 0U);
        }

        /* 7. Reset Ctrl Intance */
        AmbaSvcWrap_MisraMemset(&g_CfdTestbedCtrl[Inst], 0, sizeof(CFD_TESTBED_CTRL_s));
    }

    SvcLog_OK(SVC_LOG_TESTBED, "Stop(): CvFlowChan(%u) done", CvFlowChan, 0U);

    return SVC_OK;
}

/**
* CvFlow driver debug interface
*/
static UINT32 CvFlow_TestbedDebug(UINT32 CvFlowChan, const char *pCmd, UINT32 Param0, UINT32 Param1, UINT32 Param2)
{
    if (0 == SvcWrap_strcmp("debug", pCmd)) {
        SvcLog_DBG(SVC_LOG_TESTBED, "Set debug(%u)", Param0, 0U);
    } else {
        SvcLog_DBG(SVC_LOG_TESTBED, "Unsupported cmd", 0U, 0U);
    }

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(&Param0);
    AmbaMisra_TouchUnused(&Param1);
    AmbaMisra_TouchUnused(&Param2);

    return SVC_OK;
}

SVC_CV_FLOW_OBJ_s SvcCvFlow_CnnTestbedObj = {
    .Init         = CvFlow_TestbedInit,
    .Config       = CvFlow_TestbedConfig,
    .Load         = CvFlow_TestbedLoad,
    .Start        = CvFlow_TestbedStart,
    .Control      = CvFlow_TestbedControl,
    .Stop         = CvFlow_TestbedStop,
    .Debug        = CvFlow_TestbedDebug,
};

