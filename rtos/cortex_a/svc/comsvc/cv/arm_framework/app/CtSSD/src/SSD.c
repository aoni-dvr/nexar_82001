/**
*  @file SSD.c
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
*   @details The SSD-like algorithm for flexidag
*
*/

#include "SSD.h"

#define ARM_LOG_SSD    "SSD"

#define SSD_UUID       (2U)

#define NMSCFG_TOP_K            (400)
#define NMSCFG_NMS_TOP_K        (MAX_CVALGO_SSD_BOX_NUM)
#define NMSCFG_CONF_THRD        (0.4f)
#define NMSCFG_NMS_THRD         (0.45f)
#define VISCFG_VIS_THRD         (0.4f)

typedef struct {
    SSD_NMS_CTRL_s     NmsCtrl;
    UINT32             PriorBoxNum;
    flexidag_memblk_t  PriorBoxBuf;
} SSD_CTRL_s;

/*---------------------------------------------------------------------------*\
 * Static functions
\*---------------------------------------------------------------------------*/

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
 *    SSD_CTRL_s*: The pointer to SSD_CTRL_s
\*-----------------------------------------------------------------------------------------------*/
static SSD_CTRL_s *GetAlgoCtrl(const SVC_CV_ALGO_HANDLE_s* pHdlr)
{
    SSD_CTRL_s *pAlgoCtrl = NULL;

    if (pHdlr != NULL) {
        //pAlgoCtrl = (SSD_CTRL_s *) pHdlr->pAlgoCtrl;
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
 *     pSsdCtrl:   The SSD control
 *     MemPoolId:  The memory pool ID
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 LoadPriorbox(SSD_CTRL_s* pSsdCtrl, UINT32 MemPoolId)
{
    UINT32 Rval = ARM_NG;
    UINT32 FileSize = 0U, LoadSize = 0U;

    if (pSsdCtrl != NULL) {
        ArmLog_STR(ARM_LOG_SSD, "LoadPriorbox: %s", pSsdCtrl->NmsCtrl.PriorBox, NULL);

        /* 1. Allocate memory for priorbox */
        Rval = ArmFIO_GetSize(pSsdCtrl->NmsCtrl.PriorBox, &FileSize);
        if (Rval == ARM_OK) {
            Rval = ArmMemPool_Allocate(MemPoolId, FileSize, &(pSsdCtrl->PriorBoxBuf));
        }

        /* 2. Load priorbox */
        if (Rval == ARM_OK) {
            Rval = ArmFIO_Load(pSsdCtrl->PriorBoxBuf.pBuffer, FileSize, pSsdCtrl->NmsCtrl.PriorBox, &LoadSize);
            if (LoadSize != FileSize) {
                ArmLog_ERR(ARM_LOG_SSD, "## LoadPriorbox fail (LoadSize(%u) != FileSize(%u))", LoadSize, FileSize);
                Rval = ARM_NG;
            }
        }

        /* 3. Calculate PriorBoxNum */
        if (Rval == ARM_OK) {
            // PriorBoxNum = Size / (4 * 4 * 2)
            // 4: sizeof(FLOAT), 4: xmin,ymin,xmax,ymax, 2: loc and var
            pSsdCtrl->PriorBoxNum = FileSize >> 5;
            ArmLog_DBG(ARM_LOG_SSD, "PriorBoxNum = %d", pSsdCtrl->PriorBoxNum, 0U);
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SSDCallback
 *
 *  @Description:: Callback for SSD
 *
 *  @Input      ::
 *    pEventData:  Pointer to data (CCF_OUTPUT_s)
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void SSDCallback(const CCF_OUTPUT_s *pEvnetData)
{
    UINT32 Rval = ARM_OK;
    const SVC_CV_ALGO_HANDLE_s *pHdlr      = NULL;
    const SSD_CTRL_s           *pSSDCtrl   = NULL;
    CVALGO_SSD_OUTPUT_s        *pSSDOutput = NULL;
    SVC_CV_ALGO_OUTPUT_s       CvAlgoOut;

    // ArmSsdFusion
    NMS_INPUT_s  NmsInput  = {NULL};
    NMS_CFG_s    NmsCfg    = {0};
    NMS_OUTPUT_s NmsOutput = {0};
    VIS_CFG_s    VisCfg    = {0};
    VIS_OUTPUT_s VisOutput = {0};

    /* 1. Sanity check for parameters */
    if (pEvnetData == NULL) {
        ArmLog_ERR(ARM_LOG_SSD, "## Callback() fail (pEvnetData is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pEvnetData->pCvAlgoHdlr == NULL) {
            ArmLog_ERR(ARM_LOG_SSD, "## Callback() fail (pCvAlgoHdlr is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pOut == NULL) {
            ArmLog_ERR(ARM_LOG_SSD, "## Callback() fail (pOut is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pInternal == NULL) {   // We use pInternal to pass final output buffer
            ArmLog_ERR(ARM_LOG_SSD, "## Callback() fail (pInternal (final output buffer) is null)", 0U, 0U);
            Rval = ARM_NG;
        } else {
            if (pEvnetData->pOut->num_of_buf != 2U) {
                ArmLog_ERR(ARM_LOG_SSD, "## Callback() fail (SSD should have 2 output (num_of_buf = %u)", pEvnetData->pOut->num_of_buf, 0U);
                Rval = ARM_NG;
            }
        }
    }

    /* 2. Assign the following pointers
          pHdlr         : CvAlgo handler
          pSSDOutput    : The buffer to put final output result
          pSSDCtrl      : SSD algo control
     */
    if ((Rval == ARM_OK) && (pEvnetData != NULL)) {
        //pHdlr = (SVC_CV_ALGO_HANDLE_s*) pEvnetData->pCvAlgoHdlr;
        (void) ArmStdC_memcpy(&pHdlr, &(pEvnetData->pCvAlgoHdlr), sizeof(void *));

        //pSSDOutput = (CVALGO_SSD_OUTPUT_s*) pEvnetData->pInternal->buf[0].pBuffer;
        (void) ArmStdC_memcpy(&pSSDOutput, &(pEvnetData->pInternal->buf[0].pBuffer), sizeof(void *));

        pSSDCtrl = GetAlgoCtrl(pHdlr);
    }

    /* 3. Sanity check for output size */
    if ((Rval == ARM_OK) && (pEvnetData != NULL) && (pSSDCtrl != NULL)) {
        if (pEvnetData->pOut->buf[0].buffer_size < (pSSDCtrl->PriorBoxNum * sizeof(FLOAT) * 4U)) {
            ArmLog_ERR(ARM_LOG_SSD, "## invalid pLoc size (%d), please check if pLoc is fp32 and at 1st output",
                       pEvnetData->pOut->buf[0].buffer_size, 0U);
            Rval = ARM_NG;
        }
        if (pEvnetData->pOut->buf[1].buffer_size < (pSSDCtrl->PriorBoxNum * sizeof(FLOAT) * pSSDCtrl->NmsCtrl.ClassNum)) {
            ArmLog_ERR(ARM_LOG_SSD, "## invalid pConf size (%d), please check if pConf is fp32 and at 2nd output",
                       pEvnetData->pOut->buf[1].buffer_size, 0U);
            Rval = ARM_NG;
        }
        if (pSSDCtrl->PriorBoxNum == 0U) {
            ArmLog_ERR(ARM_LOG_SSD, "## invalid PriorBoxNum %d", pSSDCtrl->PriorBoxNum, 0U);
            Rval = ARM_NG;
        }
    }

    /* 4. Send CALLBACK_EVENT_FREE_INPUT*/
    if ((Rval == ARM_OK) && (pEvnetData != NULL) && (pHdlr != NULL)) {
        CvAlgoOut.pOutput    = NULL;
        CvAlgoOut.pUserData  = pEvnetData->pUserData;
        CvAlgoOut.pExtOutput = NULL;

        SendCallback(CALLBACK_EVENT_FREE_INPUT, pHdlr, &CvAlgoOut);
    }

    /* 5. Do SSD fusion + NMS */
    if ((Rval == ARM_OK) && (pEvnetData != NULL) && (pSSDCtrl != NULL)) {
        /* Do NMS calculation */
        //NmsInput.pPriorbox  = (FLOAT*) pSSDCtrl->PriorBoxBuf.pBuffer;
        (void) ArmStdC_memcpy(&NmsInput.pPriorbox, &(pSSDCtrl->PriorBoxBuf.pBuffer), sizeof(void *));
        //NmsInput.pLoc       = (FLOAT*) pEvnetData->pOut->buf[0].pBuffer;
        (void) ArmStdC_memcpy(&NmsInput.pLoc, &(pEvnetData->pOut->buf[0].pBuffer), sizeof(void *));
        //NmsInput.pConf      = (FLOAT*) pEvnetData->pOut->buf[1].pBuffer;
        (void) ArmStdC_memcpy(&NmsInput.pConf, &(pEvnetData->pOut->buf[1].pBuffer), sizeof(void *));

        NmsCfg.PriorBoxNum  = pSSDCtrl->PriorBoxNum;
        NmsCfg.ClassNum     = pSSDCtrl->NmsCtrl.ClassNum;
        NmsCfg.TopK         = NMSCFG_TOP_K;
        NmsCfg.NmsTopK      = NMSCFG_NMS_TOP_K;
        NmsCfg.ConfThrd     = NMSCFG_CONF_THRD;
        NmsCfg.NmsThrd      = NMSCFG_NMS_THRD;
        NmsCfg.ModelType    = pSSDCtrl->NmsCtrl.ModelType;
        Rval = ArmSsdFusion_NmsCalc(NmsInput, NmsCfg, &NmsOutput);

        if (Rval == ARM_OK) {
            /* Do Visual calculation */
            VisCfg.VisThrd        = VISCFG_VIS_THRD;
            VisCfg.RoiWidth       = pSSDCtrl->NmsCtrl.Win.RoiWidth;
            VisCfg.RoiHeight      = pSSDCtrl->NmsCtrl.Win.RoiHeight;
            VisCfg.RoiStartX      = pSSDCtrl->NmsCtrl.Win.RoiStartX;
            VisCfg.RoiStartY      = pSSDCtrl->NmsCtrl.Win.RoiStartY;
            VisCfg.NetworkWidth   = pSSDCtrl->NmsCtrl.Win.NetworkWidth;
            VisCfg.NetworkHeight  = pSSDCtrl->NmsCtrl.Win.NetworkHeight;
            VisCfg.VisWidth       = pSSDCtrl->NmsCtrl.Win.VisWidth;     // OSD size
            VisCfg.VisHeight      = pSSDCtrl->NmsCtrl.Win.VisHeight;
            VisCfg.ShowDetRegion  = 1;
            Rval = ArmSsdFusion_VisCalc(NmsOutput, VisCfg, &VisOutput);
        }

    }

    /* 6. Fill CVALGO_SSD_OUTPUT_s */
    if ((Rval == ARM_OK) && (pEvnetData != NULL) && (pHdlr != NULL) && (pSSDOutput != NULL)) {
        const VIS_BOX_s *pBox;

        pSSDOutput->BoxNum = VisOutput.VisBoxNum;
        pBox = VisOutput.pVisBox;
        for (UINT32 i = 0; i < VisOutput.VisBoxNum; i ++) {
#if 0   // DEBUG LOG
            ArmLog_DBG(ARM_LOG_SSD, "[%u] Detect Class %u ", i, pBox->Class);
            ArmLog_DBG(ARM_LOG_SSD, "    (X,Y) = (%u,%u)", pBox->X, pBox->Y);
            ArmLog_DBG(ARM_LOG_SSD, "    (W,H) = (%u,%u)", pBox->W, pBox->H);
#endif
            pSSDOutput->Box[i].Class = pBox->Class;
            pSSDOutput->Box[i].Score = pBox->Score;
            pSSDOutput->Box[i].X     = pBox->X;
            pSSDOutput->Box[i].Y     = pBox->Y;
            pSSDOutput->Box[i].W     = pBox->W;
            pSSDOutput->Box[i].H     = pBox->H;

            pBox ++;
        }

    }

    /* 7. Send CALLBACK_EVENT_OUTPUT */
    if ((Rval == ARM_OK) && (pEvnetData != NULL) && (pHdlr != NULL)) {
        CvAlgoOut.pOutput    = pEvnetData->pInternal;
        CvAlgoOut.pUserData  = pEvnetData->pUserData;
        CvAlgoOut.pExtOutput = NULL;

        SendCallback(CALLBACK_EVENT_OUTPUT, pHdlr, &CvAlgoOut);
    }
}

/*---------------------------------------------------------------------------*\
 * SSD APIs
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: SSD_Query
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
static UINT32 SSD_Query(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_QUERY_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_SSD, "## Query() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_PREOPEN) {
            ArmLog_ERR(ARM_LOG_SSD, "## Query() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pAlgoObj == NULL) {
            ArmLog_ERR(ARM_LOG_SSD, "## Query() fail (pAlgoObj is null)", 0U, 0U);
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
 *  @RoutineName:: SSD_Create
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
static UINT32 SSD_Create(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_CREATE_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;
    static UINT32     Idx     = 0U;
    CCF_OUTPUT_INFO_s OutInfo = {0};
    CCF_CREATE_CFG_s  CreateCfg;
    CCF_REGCB_CFG_s   RegCbCfg;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_SSD, "## Create() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_PREOPEN) {
            ArmLog_ERR(ARM_LOG_SSD, "## Create() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pAlgoBuf == NULL) {
            ArmLog_ERR(ARM_LOG_SSD, "## Create() fail (pAlgoBuf is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pCfg->NumFD != 1U) {
            ArmLog_ERR(ARM_LOG_SSD, "## Create() fail (invalid NumFD %u)", pCfg->NumFD, 0U);
            Rval = ARM_NG;
        } else {
            for (UINT32 i = 0U; i < pCfg->NumFD; i ++) {
                if (pCfg->pBin[i] == NULL) {
                    ArmLog_ERR(ARM_LOG_SSD, "## Create() fail (pBin[%u] is null)", i, 0U);
                    Rval = ARM_NG;
                }
            }
        }
    }

    /* 2. Create memory pool */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        Rval = ArmMemPool_Create(pCfg->pAlgoBuf, &pHdlr->MemPoolId);
    }

    /* 3. Allocate buffer for SSD_CTRL_s */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        flexidag_memblk_t CtrlBuf;
        Rval = ArmMemPool_Allocate(pHdlr->MemPoolId, sizeof(SSD_CTRL_s), &CtrlBuf);
        if (Rval == ARM_OK) {
            pHdlr->pAlgoCtrl = CtrlBuf.pBuffer;
            (void) ArmStdC_memset(pHdlr->pAlgoCtrl, 0x0, sizeof(SSD_CTRL_s));
        }
    }

    /* 4. Create flexidag */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        CreateCfg.pBin        = pCfg->pBin[0];
        CreateCfg.MemPoolId   = pHdlr->MemPoolId;
        CreateCfg.InBufDepth  = 0U;
        CreateCfg.OutBufDepth = 4U;   // Automatical allocate SSD output buffer

        CreateCfg.RunTskCtrl.StackSz  = DEFAULT_TSK_STACK_SIZE;
        CreateCfg.RunTskCtrl.Priority = DEFAULT_TSK_PRIORITY;
        CreateCfg.RunTskCtrl.CoreSel  = DEFAULT_TSK_CORE_SEL;

        CreateCfg.CBTskCtrl.StackSz  = DEFAULT_TSK_STACK_SIZE;
        CreateCfg.CBTskCtrl.Priority = DEFAULT_TSK_PRIORITY;
        CreateCfg.CBTskCtrl.CoreSel  = DEFAULT_TSK_CORE_SEL;

        ArmStdC_strcpy(CreateCfg.FDName, MAX_CCF_NAME_LEN, "SSD_X");
        CreateCfg.FDName[4] = '0' + (Idx % 10U);

        Idx ++;

        Rval = CvCommFlexi_Create(&(pHdlr->FDs[0]), &CreateCfg, &OutInfo);
    }

    /* 5. Register Callback */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        RegCbCfg.Mode = 0;    // register
        RegCbCfg.Callback = SSDCallback;

        Rval = CvCommFlexi_RegCallback(&(pHdlr->FDs[0]), &RegCbCfg);
    }

    /* 6. Fill output size and change state (output struct = CVALGO_SSD_OUTPUT_s) */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        pCfg->OutputNum = 1U;
        pCfg->OutputSz[0] = sizeof(CVALGO_SSD_OUTPUT_s);

        pHdlr->State    = CVALGO_STATE_INITED;
        pHdlr->pAlgoBuf = pCfg->pAlgoBuf;
    }

    /* 7. Print the Max mempool usage */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        UINT32 UsedSize = 0U;
        Rval = ArmMemPool_GetUsage(pHdlr->MemPoolId, &UsedSize);
        if (Rval == ARM_OK) {
            ArmLog_DBG(ARM_LOG_SSD, "Total used memory - %u", UsedSize, 0U);
        }
    }

    /* 8. Covert the output value to CVALGO_OK or CVALGO_NG */
    if (Rval == ARM_OK) {
        Rval = CVALGO_OK;
    } else {
        Rval = CVALGO_NG;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: SSD_Delete
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
static UINT32 SSD_Delete(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_DELETE_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_SSD, "## Delete() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_SSD, "## Delete() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Close flexidag */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        Rval = CvCommFlexi_Delete(&(pHdlr->FDs[0]));
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
 *  @RoutineName:: SSD_Feed
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
static UINT32 SSD_Feed(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_FEED_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_SSD, "## Feed() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_SSD, "## Feed() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pIn == NULL) {
            ArmLog_ERR(ARM_LOG_SSD, "## Feed() fail (pIn is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pOut == NULL) {
            ArmLog_ERR(ARM_LOG_SSD, "## Feed() fail (pOut is null)", 0U, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    /* 2. Feed flexidag */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        CCF_FEED_CFG_s   FeedCfg;

        FeedCfg.pIn         = pCfg->pIn;
        FeedCfg.pRaw        = NULL;
        FeedCfg.pPic        = NULL;
        FeedCfg.pOut        = NULL;
        FeedCfg.pUserData   = pCfg->pUserData;
        FeedCfg.pCvAlgoHdlr = pHdlr;
        FeedCfg.pInternal   = pCfg->pOut;

        Rval = CvCommFlexi_Feed(&(pHdlr->FDs[0]), &FeedCfg);
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
 *  @RoutineName:: SSD_Control
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
static UINT32 SSD_Control(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_CTRL_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;
    SSD_CTRL_s *pSSDCtrl = NULL;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_SSD, "## Control() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_SSD, "## Control() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Assign AlgoCtrl */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        pSSDCtrl = GetAlgoCtrl(pHdlr);
    }

    /* 3. Send message to flexidag */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL) && (pSSDCtrl != NULL)) {
        switch (pCfg->CtrlType) {
        // public control
        case CTRL_TYPE_ROI: {
            CCF_SEND_MSG_CFG_s SendMsgCfg;

            SendMsgCfg.UUID    = SSD_UUID;
            SendMsgCfg.pMsg    = pCfg->pCtrlParam;
            SendMsgCfg.MsgSize = sizeof(amba_roi_config_t);

            Rval = CvCommFlexi_SendMsg(&(pHdlr->FDs[0]), &SendMsgCfg);
        }
        break;
        case CTRL_TYPE_DMSG: {
            Rval = CvCommFlexi_DumpLog(&(pHdlr->FDs[0]));
        }
        break;
        // private control
        case CTRL_TYPE_SSD_NMS: {
            (void) ArmStdC_memcpy(&pSSDCtrl->NmsCtrl, pCfg->pCtrlParam, sizeof(SSD_NMS_CTRL_s));
            Rval = LoadPriorbox(pSSDCtrl, pHdlr->MemPoolId);
        }
        break;
        default:
            ArmLog_ERR(ARM_LOG_SSD, "## Control() fail (unknown control type %u)", pCfg->CtrlType, 0U);
            Rval = ARM_NG;
            break;
        }

        pHdlr->State = CVALGO_STATE_INITED; // To fix misraC
    }

    /* 4. Covert the output value to CVALGO_OK or CVALGO_NG */
    if (Rval == ARM_OK) {
        Rval = CVALGO_OK;
    } else {
        Rval = CVALGO_NG;
    }

    return Rval;
}

SVC_CV_ALGO_OBJ_s CvAlgoObj_SSD = {
    .Name          = "SSD",
    .Query         = SSD_Query,
    .Create        = SSD_Create,
    .Delete        = SSD_Delete,
    .Feed          = SSD_Feed,
    .Control       = SSD_Control,
};


