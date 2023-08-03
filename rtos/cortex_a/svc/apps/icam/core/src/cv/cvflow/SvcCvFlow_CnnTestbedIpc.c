/**
 *  @file SvcCvFlow_CnnTestbedIpc.c
 *
 * Copyright (c) [2021] Ambarella International LP
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
 *  @details Implementation of CvFlow Driver to hook up cnn_testbed IPC examples
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event.h"
#include "AmbaUtility.h"
#include "AmbaMMU.h"
#include "AmbaIPC_FlexidagIO.h"

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
#include "SvcCvFlow_CnnTestbedIpc.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcImgFeeder.h"
#include "SvcOsd.h"
#include "SvcWrap.h"

#include "cvapi_ambacv_flexidag.h"

#if defined(CONFIG_QNX)
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#endif

#define SVC_LOG_TESTBED_IPC        "TestbedIPC"

#define CFD_TESTBED_IPC_INSTANCE0                (0U)
#define CFD_TESTBED_IPC_INSTANCE1                (1U)
#define CFD_TESTBED_IPC_INSTANCE2                (2U)
#define CFD_TESTBED_IPC_INSTANCE3                (3U)
#define CFD_TESTBED_IPC_MAX_INSTANCE             (4U)

#define CFD_TESTBED_IPC_SEM_INIT_COUNT           (1U)

#define CFD_TESTBED_IPC_TX_QUEUE_SIZE            (8U)
#define CFD_TESTBED_IPC_RX_TASK_STACK_SIZE       (0x6000)
#if !defined (CONFIG_SOC_CV28)
#define CFD_TESTBED_IPC_OSD_CHANNEL              (1U)     // VOUT_IDX_B (LCD)
#else
#define CFD_TESTBED_IPC_OSD_CHANNEL              (0U)     // VOUT_IDX_A (LT9611UXC for CV28)
#endif
#define CFD_TESTBED_IPC_OSD_THICKNESS            (3U)

#define CFD_TESTBED_IPC_DEFAULT_FEEDER_ID        (0U)

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
    flexidag_memblk_t           ImgInfoBuf[CFD_TESTBED_IPC_TX_QUEUE_SIZE];  // Store the memio_source_recv_picinfo_t
    AMBA_KAL_SEMAPHORE_t        OutBufSem;    // Semaphore to control the ring buffer access

    // TX
    SVC_IMG_FEEDER_PORT_s       FeederPort;
    UINT32                      TxSeqNum;
    UINT32                      TxPicInfoBufIdx;

    // RX
    SVC_TASK_CTRL_s             RxTaskCtrl;

    // IPC
    UINT32                      IpcCh;
} CFD_TESTBED_IPC_CTRL_s;

static CFD_TESTBED_IPC_CTRL_s g_CfdTestbedIpcCtrl[CFD_TESTBED_IPC_MAX_INSTANCE] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MUTEX_t g_CfdTestbedIpcMutex;  // Mutex to protect the modification to g_CfdTestbedIpcCtrl

static UINT32 FeedData(UINT32 Inst, void *pInfo);
static UINT32 TestbedIpc_PymdDataRdyHdlr0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 TestbedIpc_PymdDataRdyHdlr1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 TestbedIpc_PymdDataRdyHdlr2(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);
static UINT32 TestbedIpc_PymdDataRdyHdlr3(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo);

static void TestbedIpc_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&g_CfdTestbedIpcMutex, 5000)) {
        SvcLog_NG(SVC_LOG_TESTBED_IPC, "TestbedIpc_MutexTake: timeout", 0U, 0U);
    }
}

static void TestbedIpc_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&g_CfdTestbedIpcMutex)) {
        SvcLog_NG(SVC_LOG_TESTBED_IPC, "TestbedIpc_MutexTake: error", 0U, 0U);
    }
}

static UINT32 TestbedIpc_SemTake(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreTake(&g_CfdTestbedIpcCtrl[Inst].OutBufSem, 10000)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_TESTBED_IPC, "TestbedIpc_SemTake[%d]: timeout", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 TestbedIpc_SemGive(UINT32 Inst)
{
    UINT32 Rval;

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreGive(&g_CfdTestbedIpcCtrl[Inst].OutBufSem)) {
        Rval = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_TESTBED_IPC, "TestbedIpc_SemTake[%d]: error", Inst, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 TestbedIpc_CvfChan2Inst(UINT32 CvfChan, UINT32 *pInst)
{
    UINT32 Rval = SVC_NG;
    UINT32 i;

    if (NULL != pInst) {
        for (i = 0; i < CFD_TESTBED_IPC_MAX_INSTANCE; i++) {
            if ((1U == g_CfdTestbedIpcCtrl[i].Used) && (CvfChan == g_CfdTestbedIpcCtrl[i].CvfChan)) {
                *pInst = i;
                Rval = SVC_OK;
                break;
            }
        }
    }
    return Rval;
}

static UINT32 TestbedIpc_PymdDataRdyHdlr(UINT32 Inst, SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    UINT32 Rval = SVC_OK;
    CFD_TESTBED_IPC_CTRL_s *pCtrl = &g_CfdTestbedIpcCtrl[Inst];

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

static UINT32 TestbedIpc_PymdDataRdyHdlr0(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    return TestbedIpc_PymdDataRdyHdlr(CFD_TESTBED_IPC_INSTANCE0, pPort, Event, pInfo);
}

static UINT32 TestbedIpc_PymdDataRdyHdlr1(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    return TestbedIpc_PymdDataRdyHdlr(CFD_TESTBED_IPC_INSTANCE1, pPort, Event, pInfo);
}

static UINT32 TestbedIpc_PymdDataRdyHdlr2(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    return TestbedIpc_PymdDataRdyHdlr(CFD_TESTBED_IPC_INSTANCE2, pPort, Event, pInfo);
}

static UINT32 TestbedIpc_PymdDataRdyHdlr3(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    return TestbedIpc_PymdDataRdyHdlr(CFD_TESTBED_IPC_INSTANCE3, pPort, Event, pInfo);
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
        SvcLog_NG(SVC_LOG_TESTBED_IPC, "PackToPicInfo error for null input", 0U, 0U);
    }
}


/**
* TX data source handler: process input data
*/
static UINT32 FeedData(UINT32 Inst, void *pInfo)
{
    UINT32 Rval;
    CFD_TESTBED_IPC_CTRL_s *pCtrl = &g_CfdTestbedIpcCtrl[Inst];
    const SVC_CV_INPUT_IMG_INFO_s *pInputImg;
    const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo;
    memio_source_recv_picinfo_t *pPicInfo;

    (void) pInfo;

    /* 1. Take Semaphore (Make sure we have freed buffers) */
    Rval = TestbedIpc_SemTake(Inst);

    if (SVC_OK == Rval) {
        /* 2. Get pyramid data from pInfo */
        AmbaMisra_TypeCast(&pInputImg, &pInfo);
        AmbaMisra_TypeCast(&pYuvInfo, &pInputImg->Info[0].pBase);
        /* 3. Pack the pyramid data to memio_source_recv_picinfo_t format */
        AmbaMisra_TypeCast(&pPicInfo, &pCtrl->ImgInfoBuf[pCtrl->TxPicInfoBufIdx].pBuffer);
        PackToPicInfo(pPicInfo, pYuvInfo);

        /* 4. Send memio_source_recv_picinfo_t to Unix-like OS via AmbaIPC */
        Rval = AmbaIPC_FlexidagIO_SetInput(pCtrl->IpcCh, pPicInfo, sizeof(memio_source_recv_picinfo_t));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_TESTBED_IPC, "Tx[%u] AmbaIPC_FlexidagIO_SetInput() error(0x%x)", Inst, Rval);
        }

        /* 5. Update index */
        pCtrl->TxPicInfoBufIdx ++;
        if (pCtrl->TxPicInfoBufIdx >= CFD_TESTBED_IPC_TX_QUEUE_SIZE) {
            pCtrl->TxPicInfoBufIdx = 0;
        }

    } else {
        SvcLog_NG(SVC_LOG_TESTBED_IPC, "Tx[%u]: SemTake error(0x%x)", Inst, Rval);
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
        SvcLog_NG(SVC_LOG_TESTBED_IPC, "SsdDisplay: pOutput is null", 0U, 0U);
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        /* 2. Clear OSD */
        if (OsdClear != 0U) {
          (void) SvcOsd_Clear(CFD_TESTBED_IPC_OSD_CHANNEL);
        }

        /* 3. Draw Rectangle */
        //pCvAlgoSsdOutput = (CVALGO_SSD_OUTPUT_s *)pEventData->pOutput->buf[0].pBuffer;
        AmbaMisra_TypeCast(&pCvAlgoSsdOutput, &pEventData->pOutput->buf[0].pBuffer);
        for (UINT32 i = 0; i < pCvAlgoSsdOutput->BoxNum; i ++) {
#if 0   // DEBUG LOG
            SvcLog_DBG(SVC_LOG_TESTBED_IPC, "[%u] Detect Class %u ", i, pCvAlgoSsdOutput->Box[i].Class);
            SvcLog_DBG(SVC_LOG_TESTBED_IPC, "    (X,Y) = (%u,%u)", pCvAlgoSsdOutput->Box[i].X, pCvAlgoSsdOutput->Box[i].Y);
            SvcLog_DBG(SVC_LOG_TESTBED_IPC, "    (W,H) = (%u,%u)", pCvAlgoSsdOutput->Box[i].W, pCvAlgoSsdOutput->Box[i].H);
#endif
            pCurBox = &pCvAlgoSsdOutput->Box[i];
            (void) SvcOsd_DrawRect(CFD_TESTBED_IPC_OSD_CHANNEL, pCurBox->X, pCurBox->Y, pCurBox->X + pCurBox->W, pCurBox->Y + pCurBox->H,
                pCurBox->Class, CFD_TESTBED_IPC_OSD_THICKNESS);

        }

        /* 4. Flush OSD */
        if (OsdFlush != 0U) {
          (void) SvcOsd_Flush(CFD_TESTBED_IPC_OSD_CHANNEL);
        }
    }

    return;
}

static void SegDisplay(UINT32 Inst, const SVC_CV_ALGO_OUTPUT_s *pEventData, UINT32 OsdClear, UINT32 OsdFlush)
{
    UINT32 Rval = SVC_OK;
    const CFD_TESTBED_IPC_CTRL_s *pCtrl = &g_CfdTestbedIpcCtrl[Inst];
    const UINT8 *pArgMaxOut;

    /* 1. Sanity check for parameters */
    if (pEventData->pOutput == NULL) {
        SvcLog_NG(SVC_LOG_TESTBED_IPC, "SegDisplay: pOutput is null", 0U, 0U);
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        /* 2. Clear OSD */
        if (OsdClear != 0U) {
          (void) SvcOsd_Clear(CFD_TESTBED_IPC_OSD_CHANNEL);
        }

        /* 3. Draw segmentation */
        //pArgMaxOut    = (UINT8 *) pEventData->pOutput->buf[0].pBuffer;
        AmbaMisra_TypeCast(&pArgMaxOut, &pEventData->pOutput->buf[0].pBuffer);
        (void) SvcOsd_DrawSeg(
            CFD_TESTBED_IPC_OSD_CHANNEL,
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
          (void) SvcOsd_Flush(CFD_TESTBED_IPC_OSD_CHANNEL);
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
    CFD_TESTBED_IPC_CTRL_s *pCtrl = &g_CfdTestbedIpcCtrl[Inst];
    SVC_CV_ALGO_OUTPUT_s CvAlgoOut;

    {
        /* 1. Get CvAlgo result from AmbaIPC */
        UINT32 GetLen;
        memio_sink_send_out_t SinkOut = {0};
        static AMBA_CV_FLEXIDAG_IO_s Out = {0};

        Rval = AmbaIPC_FlexidagIO_GetResult(pCtrl->IpcCh, (void *)&SinkOut, &GetLen);
        if ((Rval == 0U) && (GetLen == sizeof(memio_sink_send_out_t))) {
            Out.num_of_buf = SinkOut.num_of_io;
            for (UINT32 i = 0; i < Out.num_of_buf; i++) {
#if defined(CONFIG_QNX)
              void *pVirtAddr = NULL;
              pVirtAddr = mmap_device_memory(NULL, SinkOut.io[i].size, PROT_READ | PROT_WRITE | PROT_NOCACHE, 0, SinkOut.io[i].addr);
              Out.buf[i].pBuffer          = (char *) pVirtAddr;
#else
              ULONG VirtAddr;
              (void) AmbaMMU_PhysToVirt((ULONG)SinkOut.io[i].addr, &VirtAddr);
              Out.buf[i].pBuffer          = (char *) VirtAddr;
#endif
              Out.buf[i].buffer_daddr     = SinkOut.io[i].addr;
              Out.buf[i].buffer_size      = SinkOut.io[i].size;
              Out.buf[i].buffer_cacheable = 1;
            }

            CvAlgoOut.pOutput    = &Out;
            CvAlgoOut.pUserData  = NULL;
            CvAlgoOut.pExtOutput = NULL;
        } else {
            SvcLog_NG(SVC_LOG_TESTBED_IPC, "Rx[%u]: AmbaIPC_FlexidagIO_GetResult error(GetLen = %u)", Inst, GetLen);
            Rval = SVC_NG;
        }
    }

    if (SVC_OK == Rval) {
        /* 2. Parse the result */

        UINT32 OsdClear = 1U, OsdFlush = 1U;

        // Only do OsdClear for lowest OsdPriority and OsdFlush for highest OsdPriority
        for (UINT32 i = 0; i < CFD_TESTBED_IPC_MAX_INSTANCE; i++) {
            if ( (i != Inst ) && (1U == g_CfdTestbedIpcCtrl[i].Used)) {
                if (pCtrl->ModeInfo.OsdPriority < g_CfdTestbedIpcCtrl[i].ModeInfo.OsdPriority) {
                    OsdFlush = 0U;
                }
                if (pCtrl->ModeInfo.OsdPriority > g_CfdTestbedIpcCtrl[i].ModeInfo.OsdPriority) {
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
                SvcLog_NG(SVC_LOG_TESTBED_IPC, "Rx[%u]: Unsupported CvModeID %u", Inst, pCtrl->CvCfg.CvModeID);
                break;
        }

#if defined(CONFIG_QNX)
        /* unmap device memory */
        for (UINT32 i = 0; i  < CvAlgoOut.pOutput->num_of_buf; i++) {
          INT32 ResultCode = 0;
          ResultCode = munmap_device_memory((void *)CvAlgoOut.pOutput->buf[i].pBuffer, CvAlgoOut.pOutput->buf[i].buffer_size);
          if (-1 == ResultCode) {
            SvcLog_NG(SVC_LOG_TESTBED_IPC, "Rx[%u]: munmap_device_memory fail", Inst, 0U);
            AmbaPrint_PrintStr5("## Error: %s", strerror(errno), NULL, NULL, NULL, NULL);
          }
        }
#endif
        /* 3. Give Semaphore */
        Rval = TestbedIpc_SemGive(Inst);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_TESTBED_IPC, "Rx[%u]: SemGive error(0x%x)", Inst, Rval);
        }

    } else {
        SvcLog_NG(SVC_LOG_TESTBED_IPC, "Rx[%u]: Recieve RxMsgQ error(0x%x)", Inst, Rval);
    }

    return Rval;
}

/**
* Task entry of RX process
*/
static void* IpcRxTaskEntry(void* EntryArg)
{
    const ULONG  *pArg;
    UINT32 Rval = SVC_OK, Inst;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Inst = (UINT32)(*pArg);

    SvcLog_DBG(SVC_LOG_TESTBED_IPC, "RxTaskEntry[%d] start", Inst, 0U);

    while (SVC_OK == Rval) {
        Rval = ProcessResult(Inst);
    }

    SvcLog_DBG(SVC_LOG_TESTBED_IPC, "RxTaskEntry[%d] exit", Inst, 0U);

    return NULL;
}

/**
* CvFlow driver initialization function (pInit : Not used)
*/
static UINT32 CvFlow_CnnTestbedIpcInit(UINT32 CvFlowChan, void *pInit)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    static UINT32 g_IpcInit = 0U;

    (void) CvFlowChan;
    (void) pInit;

    if (0U == g_IpcInit) {
        g_IpcInit = 1U;

        /* 1. Create mutex */
        if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&g_CfdTestbedIpcMutex, NULL)) {
            SvcLog_NG(SVC_LOG_TESTBED_IPC, "Init(): MutexCreate error", 0U, 0U);
            Rval = SVC_NG;
        }

        /* 2. Init IPC_CFD_CTRL_s */
        for (i = 0; i < CFD_TESTBED_IPC_MAX_INSTANCE; i++) {
            g_CfdTestbedIpcCtrl[i].Used = 0;
        }

        SvcLog_DBG(SVC_LOG_TESTBED_IPC, "Init(): first init", 0U, 0U);
    }

    SvcLog_OK(SVC_LOG_TESTBED_IPC, "Init(): CvFlowChan(%u) done", CvFlowChan, 0U);

    return Rval;
}

/**
* CvFlow driver configuration function (pConfig : SVC_CV_FLOW_CFG_s)
*/
static UINT32 CvFlow_CnnTestbedIpcConfig(UINT32 CvFlowChan, void *pConfig)
{
    UINT32 Rval = SVC_OK;
    UINT32 i;
    CFD_TESTBED_IPC_CTRL_s *pCtrl = NULL;

    (void) pConfig;

    if (NULL != pConfig) {
        TestbedIpc_MutexTake();
        /* 1. Create instance */
        for (i = 0; i < CFD_TESTBED_IPC_MAX_INSTANCE; i++) {
            if (0U == g_CfdTestbedIpcCtrl[i].Used) {
                AmbaSvcWrap_MisraMemset(&g_CfdTestbedIpcCtrl[i], 0, sizeof(CFD_TESTBED_IPC_CTRL_s));
                pCtrl = &g_CfdTestbedIpcCtrl[i];
                pCtrl->Used = 1;
                SvcLog_DBG(SVC_LOG_TESTBED_IPC, "Config(): CvFlowChan(%u) Create Inst(%u)", CvFlowChan, i);
                break;
            }
        }
        TestbedIpc_MutexGive();

        /* 2. Configure IPC_CFD_CTRL_s */
        if (NULL != pCtrl) {
            pCtrl->CvfChan     = (UINT8) CvFlowChan;
            AmbaSvcWrap_MisraMemcpy(&pCtrl->CvCfg, pConfig, sizeof(SVC_CV_FLOW_CFG_s));
            AmbaSvcWrap_MisraMemcpy(&pCtrl->ModeInfo, &SvcCvFlow_CnnTestbedIpc_ModeInfo[pCtrl->CvCfg.CvModeID], sizeof(SVC_CV_FLOW_CNN_TESTBED_MODE_INFO_s));

            // Reset index
            pCtrl->TxSeqNum        = 0;
            pCtrl->TxPicInfoBufIdx = 0;
            pCtrl->IpcCh = CvFlowChan;
        } else {
            Rval = SVC_NG;
        }
    } else {
        Rval = SVC_NG;
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_TESTBED_IPC, "Config(): CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_TESTBED_IPC, "Config(): CvFlowChan(%u) done", CvFlowChan, 0U);
    }
    return Rval;
}

/**
* CvFlow driver load function (pInfo : SVC_CV_FLOW_LOAD_INFO_s)
*/
static UINT32 CvFlow_CnnTestbedIpcLoad(UINT32 CvFlowChan, void *pInfo)
{
    UINT32 Rval;
    UINT32 Inst;
    CFD_TESTBED_IPC_CTRL_s *pCtrl;
    const SVC_CV_FLOW_LOAD_INFO_s *pLoadInfo;

    (void) pInfo;

    /* 1. Get Instance index from CvFlowChan */
    Rval = TestbedIpc_CvfChan2Inst(CvFlowChan, &Inst);
    if (Rval == SVC_OK) {
        pCtrl = &g_CfdTestbedIpcCtrl[Inst];

        /* 2. Load flexbin by given SVC_CV_FLOW_LOAD_INFO_s */
        if (NULL != pInfo) {
            AmbaMisra_TypeCast(&pLoadInfo, &pInfo);
            if (pLoadInfo->NumBin > 0U) {
                Rval = SvcCvFlow_LoadAlgoBin(SVC_LOG_TESTBED_IPC, pLoadInfo, &pCtrl->AlgoOp);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_TESTBED_IPC, "Load[%u]: LoadAlgoBin error", Inst, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_TESTBED_IPC, "Load[%u]: NumBin is 0", Inst, 0U);
                Rval = SVC_NG;
            }
        } else {
            SvcLog_NG(SVC_LOG_TESTBED_IPC, "Load[%u]: pInfo is null", Inst, 0U);
            Rval = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_TESTBED_IPC, "Load(): invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_TESTBED_IPC, "Load(): CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_TESTBED_IPC, "Load(): CvFlowChan(%u) done", CvFlowChan, 0U);
    }
    return Rval;
}

/**
* CvFlow driver start function (pInput : Not used)
*/
static UINT32 CvFlow_CnnTestbedIpcStart(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst;
    CFD_TESTBED_IPC_CTRL_s *pCtrl;

    // MsgQ, Task and Semaphore related variables
    static char SemName[] = "TestbedIpcOutBufSem";
    static UINT8 RxTaskStack[CFD_TESTBED_IPC_MAX_INSTANCE][CFD_TESTBED_IPC_RX_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_IMG_FEEDER_EVENT_CALLBACK_f g_CfdIpcDataRdyHdlrList[CFD_TESTBED_IPC_MAX_INSTANCE] = {
        TestbedIpc_PymdDataRdyHdlr0,
        TestbedIpc_PymdDataRdyHdlr1,
        TestbedIpc_PymdDataRdyHdlr2,
        TestbedIpc_PymdDataRdyHdlr3
    };

    (void) pInput;

    /* 1. Get Instance index from CvFlowChan */
    Rval = TestbedIpc_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl = &g_CfdTestbedIpcCtrl[Inst];

        /* Allocate buffer for pImgInfoBuf */
        {
            UINT32 i;
            SVC_CV_FLOW_BUF_INFO_s BufInfo;
            for (i = 0; i < CFD_TESTBED_IPC_TX_QUEUE_SIZE; i++) {
                Rval = SvcCvFlow_AllocWorkBuf(sizeof(memio_source_recv_picinfo_t), &BufInfo);
                if (SVC_OK == Rval) {
                    SvcCvFlow_PackFdagMemBlkInfo(SVC_LOG_TESTBED_IPC, BufInfo.pAddr, BufInfo.Size, &pCtrl->ImgInfoBuf[i]);
                } else {
                    SvcLog_NG(SVC_LOG_TESTBED_IPC, "Alloc Img info buf failed", 0U, 0U);
                    Rval = SVC_NG;
                    break;
                }
            }
        }

        // Create IPC channel
        /* 2. Do AmbaIPC_FlexidagIO_Init if running flexidag on Unix-like OS */
        Rval = AmbaIPC_FlexidagIO_Init(pCtrl->IpcCh, NULL);
        if (FLEXIDAGIO_OK != Rval) {
            SvcLog_NG(SVC_LOG_TESTBED_IPC, "Start[%u]: AmbaIPC_FlexidagIO_Init error (IPC_CH = %u)", Inst, pCtrl->IpcCh);
        } else {
            SvcLog_OK(SVC_LOG_TESTBED_IPC, "Start[%u]: AmbaIPC_FlexidagIO_Init done (IPC_CH = %u)", Inst, pCtrl->IpcCh);
        }

        if (SVC_OK == Rval) {
            /* 6. Create Semaphore */
            Rval = AmbaKAL_SemaphoreCreate(&pCtrl->OutBufSem, SemName, CFD_TESTBED_IPC_SEM_INIT_COUNT);

            /* 7. Create Rx Task */
            if (KAL_ERR_NONE == Rval) {
                pCtrl->RxTaskCtrl.Priority   = 125;
                pCtrl->RxTaskCtrl.EntryFunc  = IpcRxTaskEntry;
                pCtrl->RxTaskCtrl.EntryArg   = Inst;
                pCtrl->RxTaskCtrl.pStackBase = &RxTaskStack[Inst][0];
                pCtrl->RxTaskCtrl.StackSize  = CFD_TESTBED_IPC_RX_TASK_STACK_SIZE;
                pCtrl->RxTaskCtrl.CpuBits    = 0x01U;
                Rval = SvcTask_Create("IpcRxTask", &pCtrl->RxTaskCtrl);
            } else {
                SvcLog_NG(SVC_LOG_TESTBED_IPC, "Start[%u]: Create Semaphore error(0x%X)", Inst, Rval);
                Rval =  SVC_NG;
            }

            /* 8. Register Image Feeder Callback(pyramid event handler) */
            if (SVC_OK == Rval) {
                SVC_IMG_FEEDER_CREATE_CFG_s CreateCfg = {0};

                CreateCfg.TaskPriority = 80U;
                Rval = SvcImgFeeder_Create(CFD_TESTBED_IPC_DEFAULT_FEEDER_ID, &CreateCfg);
                if (SVC_NG == Rval) {
                    SvcLog_NG(SVC_LOG_TESTBED_IPC, "Start[%u]: ImgFeederCreate error", Inst, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_TESTBED_IPC, "Start[%u]: TaskCreate error", Inst, 0U);
            }

            if (SVC_OK == Rval) {
                SVC_IMG_FEEDER_PORT_CFG_s PortCfg = {0};

                PortCfg.NumPath            = 1U;
                PortCfg.Content[0].DataSrc = SVC_CV_DATA_SRC_PYRAMID;
                PortCfg.Content[0].StrmId  = 0U;  // Fov 0
                PortCfg.SendFunc           = g_CfdIpcDataRdyHdlrList[Inst];
                Rval = SvcImgFeeder_OpenPort(CFD_TESTBED_IPC_DEFAULT_FEEDER_ID, &PortCfg, &pCtrl->FeederPort);
                if (SVC_NG == Rval) {
                    SvcLog_NG(SVC_LOG_TESTBED_IPC, "Start[%u]: ImgFeederOpen error", Inst, 0U);
                }
            }

            if (SVC_OK == Rval) {
                Rval = SvcImgFeeder_Connect(&pCtrl->FeederPort);
                if (SVC_NG == Rval) {
                    SvcLog_NG(SVC_LOG_TESTBED_IPC, "Start[%u]: ImgFeederConnect error", Inst, 0U);
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_TESTBED_IPC, "Start(): Invalid CvFlowChan (%u)", CvFlowChan, 0U);
    }

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_TESTBED_IPC, "Start(): CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_TESTBED_IPC, "Start(): CvFlowChan(%u) done", CvFlowChan, 0U);
    }

    return Rval;
}

/**
* CvFlow driver control function
*/
static UINT32 CvFlow_CnnTestbedIpcControl(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInfo)
{
    UINT32 Rval, Inst;
    CFD_TESTBED_IPC_CTRL_s *pCtrl;
    const UINT32 *pValue;
    const SSD_NMS_CTRL_s *pSsdNms;
    const SEG_WIN_CTRL_s *pSegWin;

    TestbedIpc_MutexTake();

    /* 1. Get Instance index from CvFlowChan */
    Rval = TestbedIpc_CvfChan2Inst(CvFlowChan, &Inst);
    if (SVC_OK == Rval) {
        pCtrl = &g_CfdTestbedIpcCtrl[Inst];

        /* 2. Execute the control */
        switch (CtrlType) {
            case SVC_CV_CTRL_SET_SRC_RATE:
                AmbaMisra_TypeCast(&pValue, &pInfo);
                pCtrl->ModeInfo.TxRateCtrl = *pValue;
                SvcLog_OK(SVC_LOG_TESTBED_IPC, "Control[%d]: SrcRate = %u", Inst, *pValue);
                break;
            case TESTBED_CVALGO_CTRL_SET_SSD_NMS:
                AmbaSvcWrap_MisraMemcpy(&pCtrl->ModeInfo.SsdNmsCtrl, pInfo, sizeof(SSD_NMS_CTRL_s));
                pSsdNms = &pCtrl->ModeInfo.SsdNmsCtrl;
                SvcLog_OK(SVC_LOG_TESTBED_IPC, "Control[%d]:", Inst, 0U);
                SvcLog_OK(SVC_LOG_TESTBED_IPC, "    ClassNum = %u, ModelType = %u", pSsdNms->ClassNum, pSsdNms->ModelType);
                SvcLog_OK(SVC_LOG_TESTBED_IPC, "    (RoiW, RoiH) = (%u, %u)", pSsdNms->Win.RoiWidth, pSsdNms->Win.RoiHeight);
                SvcLog_OK(SVC_LOG_TESTBED_IPC, "    (RoiX, RoiY) = (%u, %u)", pSsdNms->Win.RoiStartX, pSsdNms->Win.RoiStartY);
                SvcLog_OK(SVC_LOG_TESTBED_IPC, "    (NetW, NetH) = (%u, %u)", pSsdNms->Win.NetworkWidth, pSsdNms->Win.NetworkHeight);
                SvcLog_OK(SVC_LOG_TESTBED_IPC, "    (VisW, VisH) = (%u, %u)", pSsdNms->Win.VisWidth, pSsdNms->Win.VisHeight);
                break;
            case TESTBED_CVALGO_CTRL_SET_SEG_WIN:
                AmbaSvcWrap_MisraMemcpy(&pCtrl->ModeInfo.SegWinCtrl, pInfo, sizeof(SEG_WIN_CTRL_s));
                pSegWin = &pCtrl->ModeInfo.SegWinCtrl;
                SvcLog_OK(SVC_LOG_TESTBED_IPC, "Control[%d]:", Inst, 0U);
                SvcLog_OK(SVC_LOG_TESTBED_IPC, "    (RoiW, RoiH) = (%u, %u)", pSegWin->RoiWidth, pSegWin->RoiHeight);
                SvcLog_OK(SVC_LOG_TESTBED_IPC, "    (RoiX, RoiY) = (%u, %u)", pSegWin->RoiStartX, pSegWin->RoiStartY);
                SvcLog_OK(SVC_LOG_TESTBED_IPC, "    (NetW, NetH) = (%u, %u)", pSegWin->NetworkWidth, pSegWin->NetworkHeight);
                break;
            default:
                SvcLog_NG(SVC_LOG_TESTBED_IPC, "Control[%d]: Invalid CtrlType(%d)", Inst, CtrlType);
                break;
        }
    } else {
        SvcLog_NG(SVC_LOG_TESTBED_IPC, "Control(): Invalid CvFlowChan(%d)", CvFlowChan, 0U);
    }

    TestbedIpc_MutexGive();

    if (SVC_OK == Rval) {
        SvcLog_OK(SVC_LOG_TESTBED_IPC, "Control(): CvFlowChan(%u) done", CvFlowChan, 0U);
    } else {
        SvcLog_NG(SVC_LOG_TESTBED_IPC, "Control(): CvFlowChan(%u) done", CvFlowChan, 0U);
    }
    return Rval;
}

/**
* CvFlow driver stop function
*/
static UINT32 CvFlow_CnnTestbedIpcStop(UINT32 CvFlowChan, void *pInput)
{
    UINT32 Rval;
    UINT32 Inst;
    CFD_TESTBED_IPC_CTRL_s *pCtrl;
    UINT32 IsExit = 0;

    (void)pInput;

    /* 1. Get Instance index from CvFlowChan */
    Rval = TestbedIpc_CvfChan2Inst(CvFlowChan, &Inst);

    if (SVC_OK == Rval) {
        pCtrl = &g_CfdTestbedIpcCtrl[Inst];

        /* 2. Disable ImgFeeder */
        Rval = SvcImgFeeder_Disconnect(&pCtrl->FeederPort);
        Rval |= SvcImgFeeder_ClosePort(&pCtrl->FeederPort);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_TESTBED_IPC, "Stop[%u]: ImgFeeder Disconnect/ClosePort error(0x%X)", Inst, Rval);
        }

        /* 3. Wait until all Semaphore are freed (a.k.a Tx/RX are idle) */
        while (IsExit == 0U) {
            AMBA_KAL_SEMAPHORE_INFO_s SemaphoreInfo;

            Rval = AmbaKAL_SemaphoreQuery(&pCtrl->OutBufSem, &SemaphoreInfo);
            if (SVC_OK == Rval) {
                if (SemaphoreInfo.CurrentCount == CFD_TESTBED_IPC_SEM_INIT_COUNT) {
                    IsExit = 1;
                } else {
                    (void) AmbaKAL_TaskSleep(10);
                }
            } else {
                SvcLog_NG(SVC_LOG_TESTBED_IPC, "Stop[%u]: SemaphoreQuery error(0x%X)", Inst, Rval);
                IsExit = 1;
            }
        }

        /* 4. Destroy Tx/Rx Task */
        Rval = SvcTask_Destroy(&pCtrl->RxTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_TESTBED_IPC, "Stop[%u]: Destroy Tx/Rx Task error(0x%X)", Inst, Rval);
        }

        /* 5. Delete MsgQ and Semaphore */
        Rval = AmbaKAL_SemaphoreDelete(&pCtrl->OutBufSem);
        if (KAL_ERR_NONE != Rval) {
            SvcLog_NG(SVC_LOG_TESTBED_IPC, "Stop[%u]: Delete Semaphore error(0x%X)", Inst, Rval);
        }

        /* 6. Reset Ctrl Intance */
        AmbaSvcWrap_MisraMemset(&g_CfdTestbedIpcCtrl[Inst], 0, sizeof(CFD_TESTBED_IPC_CTRL_s));
    }

    SvcLog_OK(SVC_LOG_TESTBED_IPC, "Stop(): CvFlowChan(%u) done", CvFlowChan, 0U);

    return SVC_OK;
}

/**
* CvFlow driver debug interface
*/
static UINT32 CvFlow_CnnTestbedIpcDebug(UINT32 CvFlowChan, const char *pCmd, UINT32 Param0, UINT32 Param1, UINT32 Param2)
{
    if (0 == SvcWrap_strcmp("debug", pCmd)) {
        SvcLog_DBG(SVC_LOG_TESTBED_IPC, "Set debug(%u)", Param0, 0U);
    } else {
        SvcLog_DBG(SVC_LOG_TESTBED_IPC, "Unsupported cmd", 0U, 0U);
    }

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(&Param0);
    AmbaMisra_TouchUnused(&Param1);
    AmbaMisra_TouchUnused(&Param2);

    return SVC_OK;
}

SVC_CV_FLOW_OBJ_s SvcCvFlow_CnnTestbedIpcObj = {
    .Init         = CvFlow_CnnTestbedIpcInit,
    .Config       = CvFlow_CnnTestbedIpcConfig,
    .Load         = CvFlow_CnnTestbedIpcLoad,
    .Start        = CvFlow_CnnTestbedIpcStart,
    .Control      = CvFlow_CnnTestbedIpcControl,
    .Stop         = CvFlow_CnnTestbedIpcStop,
    .Debug        = CvFlow_CnnTestbedIpcDebug,
};

