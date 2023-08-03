/**
*  @file SixToOne.c
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
*   @details The (FlexiDAG A)*6 + (FlexiDAG B)*1 algorithm
*
*/

#include "SixToOne.h"

#define ARM_LOG_STO           "SixToOne"
#define ENABLE_DEBUG_LOG      (1)

/* Index for multiple flexidag */
#define INDEX_A               (0U)
#define INDEX_B               (1U)
#define NUM_FD                (2U)

/* Internal buffer depth */
#define CAMERA_NUM            (6U)
#define DEPTH_STO_FRAME       (4U)

typedef struct {
    UINT32                  CameraId;   // The camera id : 0-5
    AMBA_CV_FLEXIDAG_IO_s   InFioA;     // The AMBA_CV_FLEXIDAG_IO_s for FlexiDAG A's input
    AMBA_CV_FLEXIDAG_IO_s   OutFioA;    // The AMBA_CV_FLEXIDAG_IO_s for FlexiDAG A's output
} CAMERA_CTRL_s;

typedef struct {
    // CameraCtrl for 6 cameras
    CAMERA_CTRL_s           CameraCtrl[CAMERA_NUM];

    // A output buffers
    flexidag_memblk_t       OutBufA[FLEXIDAG_MAX_OUTPUTS];   // size = FlexiDAG A outputs * 6 cameras
    UINT32                  OutStatusA;    /* bit X = camera X
                                            * ex: 0x0003 => camera 0/1 finished VP execution and write to dram
                                            *               camera 2/3/4/5 is under processing
                                            */
    AMBA_CV_FLEXIDAG_IO_s   *pFinalOut;    // Save the pointer of final CvAlgo output

    void                    *pUserData;
    UINT32                  Valid;
} FRAME_CTRL_s;

typedef struct {
    FRAME_CTRL_s       FrameCtrl[DEPTH_STO_FRAME];
    UINT32             WpFeeder;  // write pointer for FrameCtrl[]
    CCF_OUTPUT_INFO_s  OutInfoA;
    ArmMutex_t         Mutex;   // Protect the access of FrameCtrl[]
} STO_CTRL_s;

/*---------------------------------------------------------------------------*\
 * Static functions
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: GetAvailableFrameCtrl
 *
 *  @Description:: Get available FrameCtrl[]
 *
 *  @Input      ::
 *     pSTOCtrl:   SixToOne algo control
 *
 *  @Output     ::
 *
 *  @Return     ::
 *     FRAME_CTRL_s*:  The control of current feeding frame
\*---------------------------------------------------------------------------*/
static FRAME_CTRL_s* GetAvailableFrameCtrl(STO_CTRL_s *pSTOCtrl)
{
    FRAME_CTRL_s* pFrameCtrl = NULL;
    UINT32 WaitTime = 10; // 10ms

    /* 1. Sanity check */
    if (pSTOCtrl == NULL) {
        ArmLog_ERR(ARM_LOG_STO, "## pSTOCtrl is null", 0U, 0U);
    } else {
        /* 2. Get available FeedCtrl */
        while (1) {
            (void) ArmMutex_Take(&pSTOCtrl->Mutex);

            if (pSTOCtrl->FrameCtrl[pSTOCtrl->WpFeeder].Valid == 0U) {
                pFrameCtrl = &pSTOCtrl->FrameCtrl[pSTOCtrl->WpFeeder];
                pFrameCtrl->OutStatusA = 0U;
                pFrameCtrl->Valid      = 1U;

                // Update WpFeeder
                pSTOCtrl->WpFeeder ++;
                pSTOCtrl->WpFeeder %= DEPTH_STO_FRAME;
                (void) ArmMutex_Give(&pSTOCtrl->Mutex);
                break;
            } else {
                (void) ArmMutex_Give(&pSTOCtrl->Mutex);
                ArmLog_WARN(ARM_LOG_STO, "Warning!! Cannot get FrameCtrl, wait %ums", WaitTime, 0U);
                (void) ArmTask_Sleep(WaitTime);

            }
        }
    }

    return pFrameCtrl;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FreeFrameCtrl
 *
 *  @Description:: Free FrameCtrl[]
 *
 *  @Input      ::
 *     pSTOCtrl:   SixToOne algo control
 *     pFrameCtrl: The pointer of freed FrameCtrl
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void FreeFrameCtrl(STO_CTRL_s *pSTOCtrl, FRAME_CTRL_s *pFrameCtrl)
{
    (void) ArmMutex_Take(&pSTOCtrl->Mutex);

    pFrameCtrl->Valid = 0U;

    (void) ArmMutex_Give(&pSTOCtrl->Mutex);
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
 *    STO_CTRL_s*: The pointer to STO_CTRL_s
\*-----------------------------------------------------------------------------------------------*/
static STO_CTRL_s *GetAlgoCtrl(const SVC_CV_ALGO_HANDLE_s* pHdlr)
{
    STO_CTRL_s *pAlgoCtrl = NULL;

    if (pHdlr != NULL) {
        //pAlgoCtrl = (STO_CTRL_s *) pHdlr->pAlgoCtrl;
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
 *  @RoutineName:: CallbackA
 *
 *  @Description:: Callback for FlexiDAG A
 *
 *  @Input      ::
 *    pEventData:  Pointer to data (CCF_OUTPUT_s)
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void CallbackA(const CCF_OUTPUT_s *pEvnetData)
{
    UINT32 Rval = ARM_OK;
    UINT32 AllDataReady = 0U;
    SVC_CV_ALGO_HANDLE_s  *pHdlr          = NULL;
    const STO_CTRL_s      *pSTOCtrl       = NULL;
    FRAME_CTRL_s          *pFrameCtrl     = NULL;
    CAMERA_CTRL_s         *pCameraCtrl    = NULL;

    /* 1. Sanity check for parameters */
    if (pEvnetData == NULL) {
        ArmLog_ERR(ARM_LOG_STO, "## Callback() fail (pEvnetData is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pEvnetData->pCvAlgoHdlr == NULL) {
            ArmLog_ERR(ARM_LOG_STO, "## Callback() fail (pCvAlgoHdlr is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pOut == NULL) {
            ArmLog_ERR(ARM_LOG_STO, "## Callback() fail (pOut is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pUserData == NULL) {   // We use pUserData to pass FRAME_CTRL_s
            ArmLog_ERR(ARM_LOG_STO, "## Callback() fail (pUserData (FRAME_CTRL_s) is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pInternal == NULL) {   // We use pInternal to pass CAMERA_CTRL_s
            ArmLog_ERR(ARM_LOG_STO, "## Callback() fail (pInternal (CAMERA_CTRL_s) is null)", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Assign the following pointers
          pHdlr          : CvAlgo handler
          pSTOCtrl       : SixToOne algo control
          pFrameCtrl     : The control of current feeding frame
          pCameraCtrl    : The control of current camera
     */
    if ((Rval == ARM_OK) && (pEvnetData != NULL)) {
        //pHdlr = (SVC_CV_ALGO_HANDLE_s*) pEvnetData->pCvAlgoHdlr;
        (void) ArmStdC_memcpy(&pHdlr, &(pEvnetData->pCvAlgoHdlr), sizeof(void *));

        pSTOCtrl = GetAlgoCtrl(pHdlr);

        //pFrameCtrl = (FRAME_CTRL_s*) pEvnetData->pUserData;
        (void) ArmStdC_memcpy(&pFrameCtrl, &(pEvnetData->pUserData), sizeof(void *));

        //pCameraCtrl = (CAMERA_CTRL_s*) pEvnetData->pInternal;
        (void) ArmStdC_memcpy(&pCameraCtrl, &(pEvnetData->pInternal), sizeof(void *));
    }

    /* 3. Update OutStatusA */
    if ((Rval == ARM_OK) && (pSTOCtrl != NULL) && (pCameraCtrl != NULL) && (pFrameCtrl != NULL)) {
        (void) ArmMutex_Take(&pSTOCtrl->Mutex);
        pFrameCtrl->OutStatusA |= (1U << pCameraCtrl->CameraId);

        // Check all results for 6 cameras
        if (pFrameCtrl->OutStatusA == ((1U << CAMERA_NUM) - 1)) {
            AllDataReady = 1U;
        }
        (void) ArmMutex_Give(&pSTOCtrl->Mutex);
    }

    /* 4. Run FlexiDAG B if the input of B (6 result from A) are ready. */
    if ((Rval == ARM_OK) && (pSTOCtrl != NULL) && (pCameraCtrl != NULL) && (pFrameCtrl != NULL) && (pHdlr != NULL) && (pEvnetData != NULL)) {

        /* 5. Call CvCommFlexi_Feed() to run B network */
        if (AllDataReady == 1U) {
            memio_source_recv_multi_raw_t CCFRawData = {0};
            CCF_FEED_CFG_s   FeedCfg;

            CCFRawData.num_io = pSTOCtrl->OutInfoA.OutputNum;
            for (UINT32 i = 0U; i < pSTOCtrl->OutInfoA.OutputNum; i++) {
                CCFRawData.io[i].addr  = pFrameCtrl->OutBufA[i].buffer_daddr;  //physical address
                CCFRawData.io[i].size  = pFrameCtrl->OutBufA[i].buffer_size;
                CCFRawData.io[i].pitch = 0;
            }

            FeedCfg.pIn         = NULL;
            FeedCfg.pRaw        = &CCFRawData;
            FeedCfg.pPic        = NULL;
            FeedCfg.pOut        = pFrameCtrl->pFinalOut;  // pFinalOut = output buffers for B network
            FeedCfg.pUserData   = pFrameCtrl;
            FeedCfg.pCvAlgoHdlr = pHdlr;
            FeedCfg.pInternal   = NULL;

            Rval = CvCommFlexi_Feed(&(pHdlr->FDs[INDEX_B]), &FeedCfg);
            if (Rval != ARM_OK) {
                ArmLog_ERR(ARM_LOG_STO, "## fail to feed to B network", 0U, 0U);
            }
        }
    }

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CallbackB
 *
 *  @Description:: Callback for FlexiDAG B
 *
 *  @Input      ::
 *    pEventData:  Pointer to data (CCF_OUTPUT_s)
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void CallbackB(const CCF_OUTPUT_s *pEvnetData)
{
    UINT32 Rval = ARM_OK;
    SVC_CV_ALGO_HANDLE_s  *pHdlr      = NULL;
    const STO_CTRL_s      *pSTOCtrl   = NULL;
    FRAME_CTRL_s          *pFrameCtrl = NULL;
    SVC_CV_ALGO_OUTPUT_s  CvAlgoOut;

    /* 1. Sanity check for parameters */
    if (pEvnetData == NULL) {
        ArmLog_ERR(ARM_LOG_STO, "## Callback() fail (pEvnetData is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pEvnetData->pCvAlgoHdlr == NULL) {
            ArmLog_ERR(ARM_LOG_STO, "## Callback() fail (pCvAlgoHdlr is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pOut == NULL) {
            ArmLog_ERR(ARM_LOG_STO, "## Callback() fail (pOut is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pUserData == NULL) {   // We use pUserData to pass FRAME_CTRL_s
            ArmLog_ERR(ARM_LOG_STO, "## Callback() fail (pUserData (FRAME_CTRL_s) is null)", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Assign the following pointers
          pHdlr          : CvAlgo handler
          pSTOCtrl       : SixToOne algo control
          pFeedCtrl      : The control of current feeding frame
     */
    if ((Rval == ARM_OK) && (pEvnetData != NULL)) {
        //pHdlr = (SVC_CV_ALGO_HANDLE_s*) pEvnetData->pCvAlgoHdlr;
        (void) ArmStdC_memcpy(&pHdlr, &(pEvnetData->pCvAlgoHdlr), sizeof(void *));

        pSTOCtrl = GetAlgoCtrl(pHdlr);

        //pFrameCtrl = (FRAME_CTRL_s*) pEvnetData->pUserData;
        (void) ArmStdC_memcpy(&pFrameCtrl, &(pEvnetData->pUserData), sizeof(void *));
    }

    /* 3. Send CALLBACK_EVENT_OUTPUT event */
    if ((Rval == ARM_OK) && (pSTOCtrl != NULL) && (pFrameCtrl != NULL) && (pHdlr != NULL) && (pEvnetData != NULL)) {
        CvAlgoOut.pOutput    = pEvnetData->pOut;
        CvAlgoOut.pUserData  = pFrameCtrl->pUserData;;
        CvAlgoOut.pExtOutput = NULL;

        SendCallback(CALLBACK_EVENT_FREE_INPUT, pHdlr, &CvAlgoOut);
        SendCallback(CALLBACK_EVENT_OUTPUT, pHdlr, &CvAlgoOut);

        // Free current frame
        FreeFrameCtrl(pSTOCtrl, pFrameCtrl);
    }
}

/*---------------------------------------------------------------------------*\
 * SixToOne APIs
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: SixToOne_Query
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
static UINT32 SixToOne_Query(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_QUERY_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_STO, "## Query() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_PREOPEN) {
            ArmLog_ERR(ARM_LOG_STO, "## Query() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pAlgoObj == NULL) {
            ArmLog_ERR(ARM_LOG_STO, "## Query() fail (pAlgoObj is null)", 0U, 0U);
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
 *  @RoutineName:: SixToOne_Create
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
static UINT32 SixToOne_Create(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_CREATE_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;
    CCF_OUTPUT_INFO_s OutInfoA = {0};
    CCF_OUTPUT_INFO_s OutInfoB = {0};
    CCF_CREATE_CFG_s  CreateCfg;
    CCF_REGCB_CFG_s   RegCbCfg;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_STO, "## Create() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_PREOPEN) {
            ArmLog_ERR(ARM_LOG_STO, "## Create() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pAlgoBuf == NULL) {
            ArmLog_ERR(ARM_LOG_STO, "## Create() fail (pAlgoBuf is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pCfg->NumFD != 2U) {
            ArmLog_ERR(ARM_LOG_STO, "## Create() fail (invalid NumFD %u)", pCfg->NumFD, 0U);
            Rval = ARM_NG;
        } else {
            for (UINT32 i = 0U; i < pCfg->NumFD; i ++) {
                if (pCfg->pBin[i] == NULL) {
                    ArmLog_ERR(ARM_LOG_STO, "## Create() fail (pBin[%u] is null)", i, 0U);
                    Rval = ARM_NG;
                }
            }
        }
    }

    /* 2. Create memory pool */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        Rval = ArmMemPool_Create(pCfg->pAlgoBuf, &pHdlr->MemPoolId);
    }

    /* 3. Allocate buffer for STO_CTRL_s */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        flexidag_memblk_t CtrlBuf;
        Rval = ArmMemPool_Allocate(pHdlr->MemPoolId, sizeof(STO_CTRL_s), &CtrlBuf);
        if (Rval == ARM_OK) {
            pHdlr->pAlgoCtrl = CtrlBuf.pBuffer;
            (void) ArmStdC_memset(pHdlr->pAlgoCtrl, 0x0, sizeof(STO_CTRL_s));
        }
    }

    /* 4-1. Create flexidag[0] - A */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        CreateCfg.pBin        = pCfg->pBin[INDEX_A];
        CreateCfg.MemPoolId   = pHdlr->MemPoolId;
        CreateCfg.InBufDepth  = 0U;   // Allocate/Manage input buffer by SCA
        CreateCfg.OutBufDepth = 0U;   // Allocate/Manage output buffer by SCA

        CreateCfg.RunTskCtrl.StackSz  = DEFAULT_TSK_STACK_SIZE;
        CreateCfg.RunTskCtrl.Priority = DEFAULT_TSK_PRIORITY;
        CreateCfg.RunTskCtrl.CoreSel  = DEFAULT_TSK_CORE_SEL;

        CreateCfg.CBTskCtrl.StackSz  = DEFAULT_TSK_STACK_SIZE;
        CreateCfg.CBTskCtrl.Priority = DEFAULT_TSK_PRIORITY;
        CreateCfg.CBTskCtrl.CoreSel  = DEFAULT_TSK_CORE_SEL;

        ArmStdC_strcpy(CreateCfg.FDName, MAX_CCF_NAME_LEN, "FD_A");

        Rval = CvCommFlexi_Create(&(pHdlr->FDs[INDEX_A]), &CreateCfg, &OutInfoA);
    }
    /* 4-2. Create flexidag[1] - B */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        CreateCfg.pBin        = pCfg->pBin[INDEX_B];
        CreateCfg.MemPoolId   = pHdlr->MemPoolId;
        CreateCfg.InBufDepth  = DEPTH_STO_FRAME;  // Allocate/Manage input buffer container by CvCommFlexi
        CreateCfg.OutBufDepth = 0U;               // Allocate/Manage output buffer by top-level application

        CreateCfg.RunTskCtrl.StackSz  = DEFAULT_TSK_STACK_SIZE;
        CreateCfg.RunTskCtrl.Priority = DEFAULT_TSK_PRIORITY;
        CreateCfg.RunTskCtrl.CoreSel  = DEFAULT_TSK_CORE_SEL;

        CreateCfg.CBTskCtrl.StackSz  = DEFAULT_TSK_STACK_SIZE;
        CreateCfg.CBTskCtrl.Priority = DEFAULT_TSK_PRIORITY;
        CreateCfg.CBTskCtrl.CoreSel  = DEFAULT_TSK_CORE_SEL;

        ArmStdC_strcpy(CreateCfg.FDName, MAX_CCF_NAME_LEN, "FD_B");

        Rval = CvCommFlexi_Create(&(pHdlr->FDs[INDEX_B]), &CreateCfg, &OutInfoB);
    }

    /* 5. Allocate buffer for OutBufA (size = FD_A outputs x 6) and Create mutex */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        STO_CTRL_s *pSTOCtrl;

        pSTOCtrl = GetAlgoCtrl(pHdlr);
        if (pSTOCtrl != NULL) {
            // Keep OutInfoA
            pSTOCtrl->OutInfoA = OutInfoA;

            // Allocate OutBufA buffer
            for (UINT32 i = 0U; i < DEPTH_STO_FRAME; i++) {
                for (UINT32 j = 0U; j < OutInfoA.OutputNum; j++) {
                    if (Rval == ARM_OK) {
                        Rval = ArmMemPool_Allocate(pHdlr->MemPoolId, OutInfoA.OutputSz[j] * CAMERA_NUM, &(pSTOCtrl->FrameCtrl[i].OutBufA[j]));
                    }
                }
            }

            // Create mutex
            if (Rval == ARM_OK) {
                Rval = ArmMutex_Create(&(pSTOCtrl->Mutex), "STOMtx");
            }
        } else {
            ArmLog_ERR(ARM_LOG_STO, "## pSTOCtrl is null", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    /* 6-1. Register Callback for A network */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        RegCbCfg.Mode = 0U;    // register
        RegCbCfg.Callback = CallbackA;

        Rval = CvCommFlexi_RegCallback(&(pHdlr->FDs[INDEX_A]), &RegCbCfg);
    }
    /* 6-2. Register Callback for B network */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        RegCbCfg.Mode = 0U;    // register
        RegCbCfg.Callback = CallbackB;

        Rval = CvCommFlexi_RegCallback(&(pHdlr->FDs[INDEX_B]), &RegCbCfg);
    }

    /* 7. Fill output size and change state (output struct = FD_B output) */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        pCfg->OutputNum = OutInfoB.OutputNum;
        for (UINT32 i = 0U; i < OutInfoB.OutputNum; i ++) {
            pCfg->OutputSz[i] = OutInfoB.OutputSz[i];
        }

        pHdlr->State    = CVALGO_STATE_INITED;
        pHdlr->pAlgoBuf = pCfg->pAlgoBuf;
    }

    /* 8. Print the Max mempool usage */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        UINT32 UsedSize = 0U;
        Rval = ArmMemPool_GetUsage(pHdlr->MemPoolId, &UsedSize);
        if (Rval == ARM_OK) {
            ArmLog_DBG(ARM_LOG_STO, "Total used memory - %u", UsedSize, 0U);
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
 *  @RoutineName:: SixToOne_Delete
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
static UINT32 SixToOne_Delete(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_DELETE_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_STO, "## Delete() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_STO, "## Delete() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2-1. Close flexidag[0] - A */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        Rval = CvCommFlexi_Delete(&(pHdlr->FDs[INDEX_A]));
    }
    /* 2-2. Close flexidag[1] - B */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        Rval = CvCommFlexi_Delete(&(pHdlr->FDs[INDEX_B]));
    }

    /* 3. Delete mutex */
    if ((Rval == ARM_OK) && (pHdlr != NULL)) {
        STO_CTRL_s *pSTOCtrl = GetAlgoCtrl(pHdlr);
        Rval = ArmMutex_Delete(&(pSTOCtrl->Mutex));
    }

    /* 4. Delete buffer pool */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        Rval = ArmMemPool_Delete(pHdlr->MemPoolId);
    }

    /* 5. Set State to CVALGO_STATE_PREOPEN */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        pHdlr->State = CVALGO_STATE_PREOPEN;
    }

    /* 6. Covert the output value to CVALGO_OK or CVALGO_NG */
    if (Rval == ARM_OK) {
        Rval = CVALGO_OK;
    } else {
        Rval = CVALGO_NG;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: SixToOne_Feed
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
static UINT32 SixToOne_Feed(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_FEED_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;
    STO_CTRL_s            *pSTOCtrl       = NULL;
    FRAME_CTRL_s          *pFrameCtrl     = NULL;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_STO, "## Feed() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_STO, "## Feed() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pIn == NULL) {
            ArmLog_ERR(ARM_LOG_STO, "## Feed() fail (pIn is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pOut == NULL) {
            ArmLog_ERR(ARM_LOG_STO, "## Feed() fail (pOut is null)", 0U, 0U);
            Rval = ARM_NG;
        } else {
            UINT32 TotalInputNum = CAMERA_NUM;
            if (pCfg->pIn->num_of_buf != TotalInputNum) {
                ArmLog_ERR(ARM_LOG_STO, "## Feed() fail (SixToOne need %u inputs)", TotalInputNum, 0U);
                Rval = ARM_NG;
            } else {
                Rval = ARM_OK;
            }
        }
    }

    /* 2. Assign STO_CTRL_s */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        pSTOCtrl = GetAlgoCtrl(pHdlr);
    }

    /* 3. Get available FRAME_CTRL_s */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pSTOCtrl != NULL)) {
        pFrameCtrl = GetAvailableFrameCtrl(pSTOCtrl);
    }

    /* 4. Fill FrameCtrl */
    if ((Rval == ARM_OK) && (pSTOCtrl != NULL)  && (pCfg != NULL) && (pFrameCtrl != NULL)) {
        (void) ArmMutex_Take(&pSTOCtrl->Mutex);

        pFrameCtrl->pUserData = pCfg->pUserData;
        pFrameCtrl->pFinalOut = pCfg->pOut;
        for (UINT32 CamIdx = 0U; CamIdx < CAMERA_NUM ; CamIdx ++) {
            CAMERA_CTRL_s *pCameraCtrl = &pFrameCtrl->CameraCtrl[CamIdx];

            pCameraCtrl->CameraId = CamIdx;
            // Use pCfg->pIn->buf[0-5] to be inputs of FlexiDAG A
            pCameraCtrl->InFioA.num_of_buf = 1U;
            pCameraCtrl->InFioA.buf[0]     = pCfg->pIn->buf[CamIdx];

            pCameraCtrl->OutFioA.num_of_buf = pSTOCtrl->OutInfoA.OutputNum;
            for (UINT32 i = 0U; i < pSTOCtrl->OutInfoA.OutputNum; i++) {
                UINT32 offset = pSTOCtrl->OutInfoA.OutputSz[i] * CamIdx;
                pCameraCtrl->OutFioA.buf[i].pBuffer          = pFrameCtrl->OutBufA[i].pBuffer + offset;
                pCameraCtrl->OutFioA.buf[i].buffer_daddr     = pFrameCtrl->OutBufA[i].buffer_daddr + offset;
                pCameraCtrl->OutFioA.buf[i].buffer_cacheable = pFrameCtrl->OutBufA[i].buffer_cacheable;
                pCameraCtrl->OutFioA.buf[i].buffer_size      = pSTOCtrl->OutInfoA.OutputSz[i];
                pCameraCtrl->OutFioA.buf[i].buffer_caddr     = pFrameCtrl->OutBufA[i].buffer_caddr + offset;
            }
        }

        (void) ArmMutex_Give(&pSTOCtrl->Mutex);
    }

    /* 5. Feed data to flexidag[0] - A network */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        for (UINT32 CamIdx = 0U; CamIdx < CAMERA_NUM ; CamIdx ++) {
          CCF_FEED_CFG_s   FeedCfg = {0};

          FeedCfg.pIn         = &pFrameCtrl->CameraCtrl[CamIdx].InFioA;  // Handle A input by SCA
          FeedCfg.pRaw        = NULL;
          FeedCfg.pPic        = NULL;
          FeedCfg.pOut        = &pFrameCtrl->CameraCtrl[CamIdx].OutFioA;  // Handle A output by SCA
          FeedCfg.pUserData   = pFrameCtrl;  // Pass FRAME_CTRL_s by pUserData
          FeedCfg.pCvAlgoHdlr = pHdlr;
          FeedCfg.pInternal   = &pFrameCtrl->CameraCtrl[CamIdx];  // Pass CAMERA_CTRL_s by pInternal

          Rval = CvCommFlexi_Feed(&(pHdlr->FDs[INDEX_A]), &FeedCfg);
        }
    }

    /* 6. Covert the output value to CVALGO_OK or CVALGO_NG */
    if (Rval == ARM_OK) {
        Rval = CVALGO_OK;
    } else {
        Rval = CVALGO_NG;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: SixToOne_Control
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
static UINT32 SixToOne_Control(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_CTRL_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_STO, "## Control() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_STO, "## Control() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Send message to flexidag */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        switch (pCfg->CtrlType) {
        // public control
        case CTRL_TYPE_DMSG: {
            (void) CvCommFlexi_DumpLog(&(pHdlr->FDs[INDEX_A]));
            (void) CvCommFlexi_DumpLog(&(pHdlr->FDs[INDEX_B]));
        }
        break;
        // private control
        default:
            ArmLog_ERR(ARM_LOG_STO, "## Control() fail (unknown control type %u)", pCfg->CtrlType, 0U);
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

SVC_CV_ALGO_OBJ_s CvAlgoObj_SixToOne = {
    .Name          = "SixToOne",
    .Query         = SixToOne_Query,
    .Create        = SixToOne_Create,
    .Delete        = SixToOne_Delete,
    .Feed          = SixToOne_Feed,
    .Control       = SixToOne_Control,
};


