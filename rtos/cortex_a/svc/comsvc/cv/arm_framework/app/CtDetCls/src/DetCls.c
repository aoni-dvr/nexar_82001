/**
*  @file DetCls.c
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
*   @details The detection + classification algorithm
*
*/

#include "DetCls.h"

#define ARM_LOG_DETCLS    "DetCls"
#define ENABLE_DEBUG_LOG   (1)

#define DET_INDEX          (0U)
#define CLS_INDEX          (1U)

/* Network size */
#define DET_WIDTH          (300U)
#define DET_HEIGHT         (300U)
#define DET_PITCH          (320U)    // Align32(DET_WIDTH)
#define CLS_WIDTH          (224U)
#define CLS_HEIGHT         (224U)
#define DETCLS_DEPTH       (4U)

/* SsdFusion + NMS settings */
#define DET_CLASS_NUM      (7U)
#define DET_PRIORBOX       "./mbox_priorbox.bin"
#define NMSCFG_TOP_K       (400U)
#define NMSCFG_NMS_TOP_K   (MAX_CVALGO_DETCLS_BOX_NUM)
#define NMSCFG_CONF_THRD   (0.4f)
#define NMSCFG_NMS_THRD    (0.45f)
#define NMSCFG_MODEL_TYPE  (MODEL_TYPE_CAFFE)
#define VISCFG_VIS_THRD    (0.4f)

/* Classifier settings */
#define CLS_CLASS_NUM      (1000U)

typedef struct {
    UINT32    HorizPhaseInc;   //  19.13
    UINT32    VertPhaseInc;    //  19.13
    UINT32    HorizInitPhase;
    UINT32    VertInitPhase;
} VP_RESIZE_CFG_s;

typedef struct {
    UINT32                  BgrBufAddr;
    void                    *pUserData;
    flexidag_memblk_t       ResizeCfgBuf[MAX_CVALGO_DETCLS_BOX_NUM];
    UINT32                  ClsCount;
    UINT32                  Valid;
} FRAME_CTRL_s;

typedef struct {
    UINT32             PriorBoxNum;
    flexidag_memblk_t  PriorBoxBuf;

    FRAME_CTRL_s       FrameCtrl[DETCLS_DEPTH];
    UINT32             WpFeeder;
} DETCLS_CTRL_s;

/*---------------------------------------------------------------------------*\
 * Static functions
\*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: update_wp
 *
 *  @Description:: Update Write pointer
 *
 *  @Input      ::
 *     wp:         WP
 *
 *  @Output     ::
 *     wp:         (WP+1) % MAX_INPUT_DEPTH
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void update_wp(UINT32 *wp)
{
    UINT32 NewValue;

    NewValue = *wp;
    NewValue ++;
    if (NewValue >= DETCLS_DEPTH) {
        NewValue = 0;
    }

    *wp = NewValue;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GetAlgoCtrl
 *
 *  @Description:: Get the AlgoCtrl struct pointer from pHdlr
 *
 *  @Input      ::
 *     pHdlr:      The SvcCvAlgo handler
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    DETCLS_CTRL_s*: The pointer to DETCLS_CTRL_s
\*-----------------------------------------------------------------------------------------------*/
static DETCLS_CTRL_s *GetAlgoCtrl(const SVC_CV_ALGO_HANDLE_s* pHdlr)
{
    DETCLS_CTRL_s *pAlgoCtrl = NULL;

    if (pHdlr != NULL) {
        //pAlgoCtrl = (DETCLS_CTRL_s *) pHdlr->pAlgoCtrl;
        (void) ArmStdC_memcpy(&pAlgoCtrl, &(pHdlr->pAlgoCtrl), sizeof(void *));
    }

    return pAlgoCtrl;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SendCallback
 *
 *  @Description:: Send callback to top level
 *
 *  @Input      ::
 *    Event:       The callback event
 *    pHdlr:       The SvcCvAlgo handler
 *    pOutData:    The callback output data
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void SendCallback(UINT32 Event, const SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_OUTPUT_s *pOutData)
{
    if ((pHdlr != NULL) && (pOutData != NULL)) {
        for (UINT32 Idx = 0U; Idx < MAX_CALLBACK_NUM; Idx++) {
            const SVC_CV_ALGO_CALLBACK_f *pCvAlgoCB = &(pHdlr->Callback[Idx]);
            if ((*pCvAlgoCB) == NULL) {
                continue;
            } else {
                (void) (*pCvAlgoCB)(Event, pOutData);
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LoadPriorbox
 *
 *  @Description:: Load priorbox binary
 *
 *  @Input      ::
 *     pCtrl:      The control pointer
 *     MemPoolId:  The memory pool ID
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 LoadPriorbox(DETCLS_CTRL_s* pCtrl, UINT32 MemPoolId)
{
    UINT32 Rval = ARM_NG;
    UINT32 FileSize = 0U, LoadSize = 0U;

    if (pCtrl != NULL) {
        ArmLog_STR(ARM_LOG_DETCLS, "LoadPriorbox: %s", DET_PRIORBOX, NULL);

        /* 1. Allocate memory for priorbox */
        Rval = ArmFIO_GetSize(DET_PRIORBOX, &FileSize);
        if (Rval == ARM_OK) {
            Rval = ArmMemPool_Allocate(MemPoolId, FileSize, &(pCtrl->PriorBoxBuf));
        }

        /* 2. Load priorbox */
        if (Rval == ARM_OK) {
            Rval = ArmFIO_Load(pCtrl->PriorBoxBuf.pBuffer, FileSize, DET_PRIORBOX, &LoadSize);
            if (LoadSize != FileSize) {
                ArmLog_ERR(ARM_LOG_DETCLS, "## LoadPriorbox fail (LoadSize(%u) != FileSize(%u))", LoadSize, FileSize);
                Rval = ARM_NG;
            }
        }

        /* 3. Calculate PriorBoxNum */
        if (Rval == ARM_OK) {
            // PriorBoxNum = Size / (4 * 4 * 2)
            // 4: sizeof(FLOAT), 4: xmin,ymin,xmax,ymax, 2: loc and var
            pCtrl->PriorBoxNum = FileSize >> 5;
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DetCallback
 *
 *  @Description:: Callback for Detection network
 *
 *  @Input      ::
 *    pEventData:  Pointer to data (CCF_OUTPUT_s)
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void DetCallback(const CCF_OUTPUT_s *pEvnetData)
{
    UINT32 Rval = ARM_OK;
    SVC_CV_ALGO_HANDLE_s   *pHdlr         = NULL;
    const DETCLS_CTRL_s    *pDetClsCtrl   = NULL;
    FRAME_CTRL_s           *pFrameCtrl    = NULL;
    CVALGO_DETCLS_OUTPUT_s *pDetClsOutput = NULL;
    // ArmSsdFusion

    NMS_INPUT_s  NmsInput  = {NULL};
    NMS_CFG_s    NmsCfg    = {0};
    NMS_OUTPUT_s NmsOutput = {0};
    VIS_CFG_s    VisCfg    = {0};
    VIS_OUTPUT_s VisOutput = {0};

    /* 1. Sanity check for parameters */
    if (pEvnetData == NULL) {
        ArmLog_ERR(ARM_LOG_DETCLS, "## Callback() fail (pEvnetData is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pEvnetData->pCvAlgoHdlr == NULL) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## Callback() fail (pCvAlgoHdlr is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pOut == NULL) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## Callback() fail (pOut is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pUserData == NULL) {   // We use pUserData to pass FRAME_CTRL_s
            ArmLog_ERR(ARM_LOG_DETCLS, "## Callback() fail (pUserData (FRAME_CTRL_s) is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pInternal == NULL) {   // We use pInternal to pass final output buffer
            ArmLog_ERR(ARM_LOG_DETCLS, "## Callback() fail (pInternal (final output buffer) is null)", 0U, 0U);
            Rval = ARM_NG;
        } else {
            if (pEvnetData->pOut->num_of_buf != 2U) {
                ArmLog_ERR(ARM_LOG_DETCLS, "## Callback() fail (DET should have 2 output (num_of_buf = %u)", pEvnetData->pOut->num_of_buf, 0U);
                Rval = ARM_NG;
            }
        }
    }

    /* 2. Assign the following pointers
          pHdlr         : CvAlgo handler
          pFrameCtrl    : The control of current frame
          pDetClsOutput : The buffer to put final output result
          pDetClsCtrl   : DetCls algo control
     */
    if ((Rval == ARM_OK) && (pEvnetData != NULL)) {
        //pHdlr = (SVC_CV_ALGO_HANDLE_s*) pEvnetData->pCvAlgoHdlr;
        (void) ArmStdC_memcpy(&pHdlr, &(pEvnetData->pCvAlgoHdlr), sizeof(void *));

        //pFrameCtrl = (FRAME_CTRL_s*) pEvnetData->pUserData;
        (void) ArmStdC_memcpy(&pFrameCtrl, &(pEvnetData->pUserData), sizeof(void *));

        //pDetClsOutput = (CVALGO_DETCLS_OUTPUT_s*) pEvnetData->pInternal->buf[0].pBuffer;
        (void) ArmStdC_memcpy(&pDetClsOutput, &(pEvnetData->pInternal->buf[0].pBuffer), sizeof(void *));

        pDetClsCtrl = GetAlgoCtrl(pHdlr);
    }

    /* 3. Sanity check for output size */
    if ((Rval == ARM_OK) && (pEvnetData != NULL) && (pDetClsCtrl != NULL)) {
        if (pEvnetData->pOut->buf[0].buffer_size < (pDetClsCtrl->PriorBoxNum * sizeof(FLOAT) * 4U)) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## invalid pLoc size (%d), please check if pLoc is fp32 and at 1st output",
                       pEvnetData->pOut->buf[0].buffer_size, 0U);
            Rval = ARM_NG;
        }
        if (pEvnetData->pOut->buf[1].buffer_size < (pDetClsCtrl->PriorBoxNum * sizeof(FLOAT) * DET_CLASS_NUM)) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## invalid pConf size (%d), please check if pConf is fp32 and at 2nd output",
                       pEvnetData->pOut->buf[1].buffer_size, 0U);
            Rval = ARM_NG;
        }
        if (pDetClsCtrl->PriorBoxNum == 0U) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## invalid PriorBoxNum %d", pDetClsCtrl->PriorBoxNum, 0U);
            Rval = ARM_NG;
        }
    }

    /* 4. Do SSD fusion + NMS */
    if ((Rval == ARM_OK) && (pEvnetData != NULL) && (pHdlr != NULL) && (pDetClsCtrl != NULL)) {
        /* Do NMS calculation */
        //NmsInput.pPriorbox  = (FLOAT*) pSSDCtrl->PriorBoxBuf.pBuffer;
        (void) ArmStdC_memcpy(&NmsInput.pPriorbox, &(pDetClsCtrl->PriorBoxBuf.pBuffer), sizeof(void *));
        //NmsInput.pLoc       = (FLOAT*) pEvnetData->pOut->buf[0].pBuffer;
        (void) ArmStdC_memcpy(&NmsInput.pLoc, &(pEvnetData->pOut->buf[0].pBuffer), sizeof(void *));
        //NmsInput.pConf      = (FLOAT*) pEvnetData->pOut->buf[1].pBuffer;
        (void) ArmStdC_memcpy(&NmsInput.pConf, &(pEvnetData->pOut->buf[1].pBuffer), sizeof(void *));

        NmsCfg.PriorBoxNum  = pDetClsCtrl->PriorBoxNum;
        NmsCfg.ClassNum     = DET_CLASS_NUM;
        NmsCfg.TopK         = NMSCFG_TOP_K;
        NmsCfg.NmsTopK      = NMSCFG_NMS_TOP_K;
        NmsCfg.ConfThrd     = NMSCFG_CONF_THRD;
        NmsCfg.NmsThrd      = NMSCFG_NMS_THRD;
        NmsCfg.ModelType    = NMSCFG_MODEL_TYPE;
        Rval = ArmSsdFusion_NmsCalc(NmsInput, NmsCfg, &NmsOutput);

        if (Rval == ARM_OK) {
            /* Do Visual calculation */
            VisCfg.VisThrd        = VISCFG_VIS_THRD;
            VisCfg.RoiStartX      = 0U;
            VisCfg.RoiStartY      = 0U;
            VisCfg.RoiWidth       = DET_WIDTH;
            VisCfg.RoiHeight      = DET_HEIGHT;
            VisCfg.NetworkWidth   = DET_WIDTH;
            VisCfg.NetworkHeight  = DET_HEIGHT;
            VisCfg.VisWidth       = DET_WIDTH;
            VisCfg.VisHeight      = DET_HEIGHT;
            VisCfg.ShowDetRegion  = 0;
            Rval = ArmSsdFusion_VisCalc(NmsOutput, VisCfg, &VisOutput);
        }
    }

    /* 5. Copy bbox info to pDetClsOutput */
    if ((Rval == ARM_OK) && (pDetClsCtrl != NULL) && (pFrameCtrl != NULL) && (pDetClsOutput != NULL)) {
        const VIS_BOX_s *pBox;

        pDetClsOutput->BoxNum = 0U;
        pFrameCtrl->ClsCount  = 0U;
        pBox = VisOutput.pVisBox;
        for (UINT32 i = 0U; i < VisOutput.VisBoxNum; i ++) {
            pDetClsOutput->Box[pDetClsOutput->BoxNum].X = pBox->X;
            pDetClsOutput->Box[pDetClsOutput->BoxNum].Y = pBox->Y;
            pDetClsOutput->Box[pDetClsOutput->BoxNum].W = pBox->W;
            pDetClsOutput->Box[pDetClsOutput->BoxNum].H = pBox->H;
            pDetClsOutput->BoxNum ++;

            pBox ++;
        }

        if (pDetClsOutput->BoxNum == 0U) { // no bbox => clear Valid flag and free input
            pFrameCtrl->Valid = 0U;

            {
                // FREE input here due to no bbox is detected
                SVC_CV_ALGO_OUTPUT_s CvAlgoOut;
                CvAlgoOut.pOutput    = NULL;
                CvAlgoOut.pUserData  = pFrameCtrl->pUserData;
                CvAlgoOut.pExtOutput = NULL;

                SendCallback(CALLBACK_EVENT_FREE_INPUT, pHdlr, &CvAlgoOut);
            }
        }
#if ENABLE_DEBUG_LOG
        ArmLog_DBG(ARM_LOG_DETCLS, "TotalBoxNum = %d (input bgr daddr = 0x%x)", pDetClsOutput->BoxNum, pFrameCtrl->BgrBufAddr);
#endif
    }

    /* 6. Send each bbox to do vp_resize + cls */
    if ((Rval == ARM_OK) && (pDetClsCtrl != NULL) && (pFrameCtrl != NULL) && (pDetClsOutput != NULL) && (pHdlr != NULL) && (pEvnetData != NULL)) {
        memio_source_recv_multi_raw_t CCFRawData = {0};
        VP_RESIZE_CFG_s               *pVpResizeCfg;
        const CVALGO_DETCLS_BOX_s     *pBox;

        pBox    = pDetClsOutput->Box;
        for (UINT32 i = 0U; i < pDetClsOutput->BoxNum; i ++) {
#if ENABLE_DEBUG_LOG
            ArmLog_DBG(ARM_LOG_DETCLS, "[Det|%d] Detect box", i, 0U);
            ArmLog_DBG(ARM_LOG_DETCLS, "         (X,Y) = (%u,%u)", pBox->X, pBox->Y);
            ArmLog_DBG(ARM_LOG_DETCLS, "         (W,H) = (%u,%u)", pBox->W, pBox->H);
#endif

            /* 7. Fill RawData[0-2] for B/G/R data */
            for (UINT32 j = 0U; j < 3U; j++) {
                CCFRawData.io[j].addr  = pFrameCtrl->BgrBufAddr + (DET_PITCH * DET_HEIGHT * j) + (DET_PITCH * pBox->Y) + pBox->X;  //physical address
                CCFRawData.io[j].size  = pBox->W * pBox->H;
                CCFRawData.io[j].pitch = DET_PITCH;
            }

            /* 8. Fill RawData[3] for vp_resize config */
            //pVpResizeCfg = (VP_RESIZE_CFG_s*) pFrameCtrl->ResizeCfgBuf[i].pBuffer;
            (void) ArmStdC_memcpy(&pVpResizeCfg, &(pFrameCtrl->ResizeCfgBuf[i].pBuffer), sizeof(void *));
            pVpResizeCfg->HorizPhaseInc  = (pBox->W << 13) / CLS_WIDTH;
            pVpResizeCfg->VertPhaseInc   = (pBox->H << 13) / CLS_HEIGHT;
            pVpResizeCfg->HorizInitPhase = 0U;
            pVpResizeCfg->VertInitPhase  = 0U;
#if ENABLE_DEBUG_LOG
            ArmLog_DBG(ARM_LOG_DETCLS, "         (Resize_W,Resize_H) = (%u,%u)", CLS_WIDTH, CLS_HEIGHT);
            ArmLog_DBG(ARM_LOG_DETCLS, "         (HorizPhaseInc,VertPhaseInc) = (%u,%u)", pVpResizeCfg->HorizPhaseInc, pVpResizeCfg->VertPhaseInc);
#endif
            CCFRawData.io[3].addr  = pFrameCtrl->ResizeCfgBuf[i].buffer_daddr;   //physical address
            CCFRawData.io[3].size  = sizeof(VP_RESIZE_CFG_s);
            CCFRawData.io[3].pitch = 0;

            CCFRawData.num_io = 4U;

            /* 9. Cache clean if necessary */
            if(pFrameCtrl->ResizeCfgBuf[i].buffer_cacheable != 0U) {
                (void) ArmMemPool_CacheClean(&(pFrameCtrl->ResizeCfgBuf[i]));
            }

            /* 10. Call CvCommFlexi_Feed() to run CLS network */
            {
                CCF_FEED_CFG_s   FeedCfg;

                FeedCfg.pIn         = NULL;
                FeedCfg.pRaw        = &CCFRawData;
                FeedCfg.pPic        = NULL;
                FeedCfg.pOut        = NULL;
                FeedCfg.pUserData   = pFrameCtrl;
                FeedCfg.pCvAlgoHdlr = pHdlr;
                FeedCfg.pInternal   = pEvnetData->pInternal;

                Rval = CvCommFlexi_Feed(&(pHdlr->FDs[CLS_INDEX]), &FeedCfg);
                if (Rval != ARM_OK) {
                    ArmLog_ERR(ARM_LOG_DETCLS, "## fail to feed to CLS network (%u of %u)", i, pDetClsOutput->BoxNum);
                }
            }

            pBox ++;
        }
    }

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ClsCallback
 *
 *  @Description:: Callback for Classification network
 *
 *  @Input      ::
 *    pEventData:  Pointer to data (CCF_OUTPUT_s)
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void ClsCallback(const CCF_OUTPUT_s *pEvnetData)
{
    UINT32 Rval = ARM_OK;
    const SVC_CV_ALGO_HANDLE_s   *pHdlr         = NULL;
    FRAME_CTRL_s                 *pFrameCtrl    = NULL;
    CVALGO_DETCLS_OUTPUT_s       *pDetClsOutput = NULL;

    /* 1. Sanity check for parameters */
    if (pEvnetData == NULL) {
        ArmLog_ERR(ARM_LOG_DETCLS, "## Callback() fail (pEvnetData is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pEvnetData->pCvAlgoHdlr == NULL) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## Callback() fail (pCvAlgoHdlr is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pOut == NULL) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## Callback() fail (pOut is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pUserData == NULL) {   // We use pUserData to pass FRAME_CTRL_s
            ArmLog_ERR(ARM_LOG_DETCLS, "## Callback() fail (pUserData (FRAME_CTRL_s) is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pInternal == NULL) {   // We use pInternal to pass final output buffer
            ArmLog_ERR(ARM_LOG_DETCLS, "## Callback() fail (pInternal (final output buffer) is null)", 0U, 0U);
            Rval = ARM_NG;
        } else {
            if (pEvnetData->pOut->num_of_buf != 1U) {
                ArmLog_ERR(ARM_LOG_DETCLS, "## Callback() fail (CLS should have 1 output (num_of_buf = %u)", pEvnetData->pOut->num_of_buf, 0U);
                Rval = ARM_NG;
            }
        }
    }

    /* 2. Assign the following pointers
          pHdlr         : CvAlgo handler
          pFrameCtrl    : The control of current frame
          pDetClsOutput : The buffer to put final output result
     */
    if ((Rval == ARM_OK) && (pEvnetData != NULL)) {
        //pHdlr = (SVC_CV_ALGO_HANDLE_s*) pEvnetData->pCvAlgoHdlr;
        (void) ArmStdC_memcpy(&pHdlr, &(pEvnetData->pCvAlgoHdlr), sizeof(void *));

        //pFrameCtrl = (FRAME_CTRL_s*) pEvnetData->pUserData;
        (void) ArmStdC_memcpy(&pFrameCtrl, &(pEvnetData->pUserData), sizeof(void *));

        //pDetClsOutput = (CVALGO_DETCLS_OUTPUT_s*) pEvnetData->pInternal->buf[0].pBuffer;
        (void) ArmStdC_memcpy(&pDetClsOutput, &(pEvnetData->pInternal->buf[0].pBuffer), sizeof(void *));
    }

    /* 3. Sanity check for output size */
    if ((Rval == ARM_OK) && (pEvnetData != NULL)) {
        if (pEvnetData->pOut->buf[0].buffer_size < (CLS_CLASS_NUM * sizeof(FLOAT))) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## invalid cls output size (%d), please check if dataformat is fp32",
                       pEvnetData->pOut->buf[0].buffer_size, 0U);
            Rval = ARM_NG;
        }
    }

    /* 4. Calculate Top1 */
    if ((Rval == ARM_OK) && (pEvnetData != NULL) && (pDetClsOutput != NULL) && (pFrameCtrl != NULL)) {
        const FLOAT *pClsData;
        UINT32      Top1Class = 0U;

        //pClsData  = (FLOAT*) pEvnetData->pOut->buf[0].pBuffer;
        (void) ArmStdC_memcpy(&pClsData, &(pEvnetData->pOut->buf[0].pBuffer), sizeof(void *));

        for (UINT32 ClsIdx = 0; ClsIdx < CLS_CLASS_NUM; ClsIdx++) {
            if (pClsData[ClsIdx] > pClsData[Top1Class]) {  // Find a larger one, replace it
                Top1Class = ClsIdx;
            }
        }

        /* 5. Fill Class and Score to CVALGO_DETCLS_OUTPUT_s */
        pDetClsOutput->Box[pFrameCtrl->ClsCount].Class = Top1Class;
        pDetClsOutput->Box[pFrameCtrl->ClsCount].Score = pClsData[Top1Class];

        pFrameCtrl->ClsCount ++;

#if ENABLE_DEBUG_LOG
        {
            UINT32 IntNum, DecNum;
            FLOAT  Tmp;
            Tmp    = pClsData[Top1Class] * 1000.0f;
            IntNum = (UINT32) (pClsData[Top1Class]);
            DecNum = (UINT32) (Tmp);
            DecNum = DecNum % 1000U;
            ArmLog_DBG(ARM_LOG_DETCLS, "[Cls|%d] label = %u", pFrameCtrl->ClsCount, Top1Class);
            ArmLog_DBG(ARM_LOG_DETCLS, "         score = %u.%u", IntNum, DecNum);
        }
#endif
    }

    /* 6. Send CALLBACK_EVENT_OUTPUT if reach last bbox */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pFrameCtrl != NULL) && (pDetClsOutput != NULL) && (pEvnetData != NULL)) {
        if (pFrameCtrl->ClsCount == pDetClsOutput->BoxNum) {
            SVC_CV_ALGO_OUTPUT_s CvAlgoOut;
            CvAlgoOut.pOutput    = pEvnetData->pInternal;
            CvAlgoOut.pUserData  = pFrameCtrl->pUserData;
            CvAlgoOut.pExtOutput = NULL;

            SendCallback(CALLBACK_EVENT_FREE_INPUT, pHdlr, &CvAlgoOut);
            SendCallback(CALLBACK_EVENT_OUTPUT, pHdlr, &CvAlgoOut);

            // clear valid flag
            pFrameCtrl->Valid = 0U;
        }
    }

}

/*---------------------------------------------------------------------------*\
 * DetCls APIs
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: DetCls_Query
 *
 *  @Description:: Query buffer requirement
 *
 *  @Input      ::
 *     pHdlr:      The SvcCvAlgo handler
 *     pCfg:       The query config
 *
 *  @Output     ::
 *     pCfg:       The buffer requirement
 *
 *  @Return     ::
 *     UINT32      CVALGO_OK(0)/CVALGO_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 DetCls_Query(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_QUERY_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_DETCLS, "## Query() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_PREOPEN) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## Query() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pAlgoObj == NULL) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## Query() fail (pAlgoObj is null)", 0U, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    /* 2. Save pAlgoObj and return required size */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        pHdlr->pAlgoObj     = pCfg->pAlgoObj;
        pCfg->TotalReqBufSz = 64 * 1024 * 1024;  // 64 MB
    }

    /* 3. Covert the output value to CVALGO_OK or CVALGO_NG */
    if (Rval == ARM_OK) {
        Rval = CVALGO_OK;
    } else {
        Rval = CVALGO_NG;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: DetCls_Create
 *
 *  @Description:: Create flexidag
 *
 *  @Input      ::
 *     pHdlr:      The SvcCvAlgo handler
 *     pCfg:       The create config
 *
 *  @Output     ::
 *     pCfg:       The output buffer size
 *
 *  @Return     ::
 *     UINT32      CVALGO_OK(0)/CVALGO_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 DetCls_Create(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_CREATE_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;
    CCF_OUTPUT_INFO_s OutInfo = {0};
    CCF_CREATE_CFG_s  CreateCfg;
    CCF_REGCB_CFG_s   RegCbCfg;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_DETCLS, "## Create() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_PREOPEN) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## Create() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pAlgoBuf == NULL) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## Create() fail (pAlgoBuf is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pCfg->NumFD != 2U) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## Create() fail (invalid NumFD %u)", pCfg->NumFD, 0U);
            Rval = ARM_NG;
        } else {
            for (UINT32 i = 0U; i < pCfg->NumFD; i ++) {
                if (pCfg->pBin[i] == NULL) {
                    ArmLog_ERR(ARM_LOG_DETCLS, "## Create() fail (pBin[%u] is null)", i, 0U);
                    Rval = ARM_NG;
                }
            }
        }
    }

    /* 2. Create memory pool */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        Rval = ArmMemPool_Create(pCfg->pAlgoBuf, &pHdlr->MemPoolId);
    }

    /* 3-1. Allocate buffer for DETCLS_CTRL_s */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        flexidag_memblk_t CtrlBuf;
        Rval = ArmMemPool_Allocate(pHdlr->MemPoolId, sizeof(DETCLS_CTRL_s), &CtrlBuf);
        if (Rval == ARM_OK) {
            pHdlr->pAlgoCtrl = CtrlBuf.pBuffer;
            (void) ArmStdC_memset(pHdlr->pAlgoCtrl, 0x0, sizeof(DETCLS_CTRL_s));
        }
    }

    /* 3-2. Allocate buffer for VP_RESIZE_CFG_s */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        DETCLS_CTRL_s *pDetClsCtrl;

        pDetClsCtrl = GetAlgoCtrl(pHdlr);
        if (pDetClsCtrl != NULL) {
            for (UINT32 i = 0U; i < DETCLS_DEPTH; i++) {
                for (UINT32 j = 0U; j < MAX_CVALGO_DETCLS_BOX_NUM; j++) {
                    if (Rval == ARM_OK) {
                        Rval = ArmMemPool_Allocate(pHdlr->MemPoolId, sizeof(VP_RESIZE_CFG_s), &(pDetClsCtrl->FrameCtrl[i].ResizeCfgBuf[j]));
                    }
                }
            }
        } else {
            ArmLog_ERR(ARM_LOG_DETCLS, "## pDetClsCtrl is null", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    /* 4. Load priorbox */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        DETCLS_CTRL_s *pDetClsCtrl;
        pDetClsCtrl = GetAlgoCtrl(pHdlr);
        Rval = LoadPriorbox(pDetClsCtrl, pHdlr->MemPoolId);
    }

    /* 5-1. Create flexidag[0] - Det */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        CreateCfg.pBin        = pCfg->pBin[DET_INDEX];
        CreateCfg.MemPoolId   = pHdlr->MemPoolId;
        CreateCfg.InBufDepth  = 0U;
        CreateCfg.OutBufDepth = DETCLS_DEPTH;   // Automatical allocate output buffer

        CreateCfg.RunTskCtrl.StackSz  = DEFAULT_TSK_STACK_SIZE;
        CreateCfg.RunTskCtrl.Priority = DEFAULT_TSK_PRIORITY;
        CreateCfg.RunTskCtrl.CoreSel  = DEFAULT_TSK_CORE_SEL;

        CreateCfg.CBTskCtrl.StackSz  = DEFAULT_TSK_STACK_SIZE;
        CreateCfg.CBTskCtrl.Priority = DEFAULT_TSK_PRIORITY;
        CreateCfg.CBTskCtrl.CoreSel  = DEFAULT_TSK_CORE_SEL;

        ArmStdC_strcpy(CreateCfg.FDName, MAX_CCF_NAME_LEN, "Det");

        Rval = CvCommFlexi_Create(&(pHdlr->FDs[DET_INDEX]), &CreateCfg, &OutInfo);
    }
    /* 5-2. Create flexidag[1] - Cls */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        CreateCfg.pBin        = pCfg->pBin[CLS_INDEX];
        CreateCfg.MemPoolId   = pHdlr->MemPoolId;
        CreateCfg.InBufDepth  = DETCLS_DEPTH;   // Automatical allocate input buffer
        CreateCfg.OutBufDepth = DETCLS_DEPTH;   // Automatical allocate output buffer

        CreateCfg.RunTskCtrl.StackSz  = DEFAULT_TSK_STACK_SIZE;
        CreateCfg.RunTskCtrl.Priority = DEFAULT_TSK_PRIORITY;
        CreateCfg.RunTskCtrl.CoreSel  = DEFAULT_TSK_CORE_SEL;

        CreateCfg.CBTskCtrl.StackSz  = DEFAULT_TSK_STACK_SIZE;
        CreateCfg.CBTskCtrl.Priority = DEFAULT_TSK_PRIORITY;
        CreateCfg.CBTskCtrl.CoreSel  = DEFAULT_TSK_CORE_SEL;

        ArmStdC_strcpy(CreateCfg.FDName, MAX_CCF_NAME_LEN, "Cls");

        Rval = CvCommFlexi_Create(&(pHdlr->FDs[CLS_INDEX]), &CreateCfg, &OutInfo);
    }

    /* 6-1. Register Callback for Det network */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        RegCbCfg.Mode = 0U;    // register
        RegCbCfg.Callback = DetCallback;

        Rval = CvCommFlexi_RegCallback(&(pHdlr->FDs[DET_INDEX]), &RegCbCfg);
    }
    /* 6-2. Register Callback for Cls network */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        RegCbCfg.Mode = 0U;    // register
        RegCbCfg.Callback = ClsCallback;

        Rval = CvCommFlexi_RegCallback(&(pHdlr->FDs[CLS_INDEX]), &RegCbCfg);
    }

    /* 7. Fill output size and change state (output struct = CVALGO_DETCLS_OUTPUT_s) */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        pCfg->OutputNum = 1U;
        pCfg->OutputSz[0] = sizeof(CVALGO_DETCLS_OUTPUT_s);

        pHdlr->State    = CVALGO_STATE_INITED;
        pHdlr->pAlgoBuf = pCfg->pAlgoBuf;
    }

    /* 8. Print the Max mempool usage */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        UINT32 UsedSize = 0U;
        Rval = ArmMemPool_GetUsage(pHdlr->MemPoolId, &UsedSize);
        if (Rval == ARM_OK) {
            ArmLog_DBG(ARM_LOG_DETCLS, "Total used memory - %u", UsedSize, 0U);
        }
    }

    /* 9. Covert the output value to CVALGO_OK or CVALGO_NG */
    if (Rval == ARM_OK) {
        Rval = CVALGO_OK;
    } else {
        Rval = CVALGO_NG;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: DetCls_Delete
 *
 *  @Description:: Delete all flexidag
 *
 *  @Input      ::
 *     pHdlr:      The SvcCvAlgo handler
 *     pCfg:       The delete config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      CVALGO_OK(0)/CVALGO_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 DetCls_Delete(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_DELETE_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_DETCLS, "## Delete() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## Delete() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2-1. Close flexidag[0] - Det */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        Rval = CvCommFlexi_Delete(&(pHdlr->FDs[DET_INDEX]));
    }
    /* 2-2. Close flexidag[1] - Cls */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        Rval = CvCommFlexi_Delete(&(pHdlr->FDs[CLS_INDEX]));
    }

    /* 3. Delete buffer pool */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        Rval = ArmMemPool_Delete(pHdlr->MemPoolId);
    }

    /* 4. Set State to CVALGO_STATE_PREOPEN */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        pHdlr->State = CVALGO_STATE_PREOPEN;
    }

    /* 5. Covert the output value to CVALGO_OK or CVALGO_NG */
    if (Rval == ARM_OK) {
        Rval = CVALGO_OK;
    } else {
        Rval = CVALGO_NG;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: DetCls_Feed
 *
 *  @Description:: Feed data
 *
 *  @Input      ::
 *     pHdlr:      The SvcCvAlgo handler
 *     pCfg:       The feed config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      CVALGO_OK(0)/CVALGO_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 DetCls_Feed(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_FEED_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;
    DETCLS_CTRL_s *pDetClsCtrl   = NULL;
    FRAME_CTRL_s  *pFrameCtrl    = NULL;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_DETCLS, "## Feed() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## Feed() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pIn == NULL) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## Feed() fail (pIn is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pOut == NULL) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## Feed() fail (pOut is null)", 0U, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    /* 2. Assign DETCLS_CTRL_s and FRAME_CTRL_s */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        pDetClsCtrl = GetAlgoCtrl(pHdlr);
        if (pDetClsCtrl != NULL) {
            pFrameCtrl = &pDetClsCtrl->FrameCtrl[pDetClsCtrl->WpFeeder];
        }
    }

    /* 3. Get available FrameCtrl and fill it */
    if ((Rval == ARM_OK) && (pDetClsCtrl != NULL) && (pFrameCtrl != NULL) && (pCfg != NULL)) {
        while (pFrameCtrl->Valid == 1U) {
            ArmLog_DBG(ARM_LOG_DETCLS, "FrameCtrl is not available. Wait 10ms", 0U, 0U);
            (void) ArmTask_Sleep(10);
        }

        {
            const memio_source_recv_raw_t  *pBgrBuf;
            //pBgrBuf = (memio_source_recv_raw_t  *)pCfg->pIn->buf[0].pBuffer;
            (void) ArmStdC_memcpy(&pBgrBuf, &(pCfg->pIn->buf[0].pBuffer), sizeof(void *));
            pFrameCtrl->BgrBufAddr = pBgrBuf->addr;
        }
        pFrameCtrl->pUserData = pCfg->pUserData;
        pFrameCtrl->Valid     = 1U;

        update_wp(&(pDetClsCtrl->WpFeeder));
    }

    /* 4. Feed data to flexidag[0] - Det network */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        CCF_FEED_CFG_s   FeedCfg;

        FeedCfg.pIn         = pCfg->pIn;
        FeedCfg.pRaw        = NULL;
        FeedCfg.pPic        = NULL;
        FeedCfg.pOut        = NULL;
        FeedCfg.pUserData   = pFrameCtrl;
        FeedCfg.pCvAlgoHdlr = pHdlr;
        FeedCfg.pInternal   = pCfg->pOut;

        Rval = CvCommFlexi_Feed(&(pHdlr->FDs[DET_INDEX]), &FeedCfg);
    }

    /* 5. Covert the output value to CVALGO_OK or CVALGO_NG */
    if (Rval == ARM_OK) {
        Rval = CVALGO_OK;
    } else {
        Rval = CVALGO_NG;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: DetCls_Control
 *
 *  @Description:: Send control to flexidag
 *
 *  @Input      ::
 *     pHdlr:      The SvcCvAlgo handler
 *     pCfg:       The control config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      CVALGO_OK(0)/CVALGO_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 DetCls_Control(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_CTRL_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_DETCLS, "## Control() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_DETCLS, "## Control() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Send message to flexidag */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        switch (pCfg->CtrlType) {
        // public control
        case CTRL_TYPE_DMSG: {
            (void) CvCommFlexi_DumpLog(&(pHdlr->FDs[DET_INDEX]));
            (void) CvCommFlexi_DumpLog(&(pHdlr->FDs[CLS_INDEX]));
        }
        break;
        // private control
        default:
            ArmLog_ERR(ARM_LOG_DETCLS, "## Control() fail (unknown control type %u)", pCfg->CtrlType, 0U);
            Rval = ARM_NG;
            break;
        }

        pHdlr->State = CVALGO_STATE_INITED; // To fix misraC
    }

    /* 3. Covert the output value to CVALGO_OK or CVALGO_NG */
    if (Rval == ARM_OK) {
        Rval = CVALGO_OK;
    } else {
        Rval = CVALGO_NG;
    }

    return Rval;
}

SVC_CV_ALGO_OBJ_s CvAlgoObj_DetCls = {
    .Name          = "DetCls",
    .Query         = DetCls_Query,
    .Create        = DetCls_Create,
    .Delete        = DetCls_Delete,
    .Feed          = DetCls_Feed,
    .Control       = DetCls_Control,
};


