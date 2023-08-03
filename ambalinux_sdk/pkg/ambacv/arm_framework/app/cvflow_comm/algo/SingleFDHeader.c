/**
*  @file SingleFDHeader.c
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
*   @details The generic algorithm for single flexidag with header
*
*/

#include "SingleFDHeader.h"

#define ARM_LOG_SINGLE_HEADER    "SingleFDHeader"
#define SINGLE_FD_HEADER_UUID    (2U)


/*---------------------------------------------------------------------------*\
 * Header information struct
\*---------------------------------------------------------------------------*/
typedef struct {
    UINT32 DataFormat[4U];
    UINT32 Dimension[4U];
    UINT32 NumOfByte;
} OUTPUT_DESCRIPTOR_s;

typedef struct {
    UINT32 FDIdx;
    UINT32 OutputIdx;
} FLEXIDAG_IDX_s;

typedef struct {
    UINT32 num_packed;
    UINT32 buffer_offset[13U];
    OUTPUT_DESCRIPTOR_s OutputDescriptor[13U];
    FLEXIDAG_IDX_s OutputIndex[13U];
} HEADER_INFO_s;


/*---------------------------------------------------------------------------*\
 * Static functions
\*---------------------------------------------------------------------------*/

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
 *  @RoutineName:: SingleFDHeaderCallback
 *
 *  @Description:: Callback for SingleFDHeader
 *
 *  @Input      ::
 *    pEventData:  Pointer to data (CCF_OUTPUT_s)
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void SingleFDHeaderCallback(const CCF_OUTPUT_s *pEventData)
{
    UINT32 Rval = ARM_OK;
    const SVC_CV_ALGO_HANDLE_s* pHdlr = NULL;
    HEADER_INFO_s HeaderInfo[8U] = {0U};
    SVC_CV_ALGO_OUTPUT_s CvAlgoOut;

    /* 1. Sanity check for parameters */
    if (pEventData == NULL) {
        ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Callback() fail (pEvnetData is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pEventData->pCvAlgoHdlr == NULL) {
            ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Callback() fail (pCvAlgoHdlr is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEventData->pOut == NULL) {
            ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Callback() fail (pOut is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEventData->pUserData == NULL) {
            ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Callback() fail (pUserData is null)", 0U, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    /* 2. Assign the following pointers
          pHdlr         : CvAlgo handler
          pRawOutput    : The buffer to put final raw output after removing header
          pCtrl         : The buffer to put pUserData
    */
    if ((Rval == ARM_OK) && (pEventData != NULL)) {
        //pHdlr = (SVC_CV_ALGO_HANDLE_s*) pEvnetData->pCvAlgoHdlr;
        (void) ArmStdC_memcpy(&pHdlr, &(pEventData->pCvAlgoHdlr), sizeof(void *));
    }

    /* 3. Send CALLBACK_EVENT_FREE_INPUT event */
    if ((Rval == ARM_OK) && (pEventData != NULL) && (pHdlr != NULL)) {
        CvAlgoOut.pOutput    = pEventData->pOut;
        CvAlgoOut.pUserData  = pEventData->pUserData;
        CvAlgoOut.pExtOutput = &HeaderInfo;

        SendCallback(CALLBACK_EVENT_FREE_INPUT, pHdlr, &CvAlgoOut);
    }

    /* 4. Get raw output one by one and send CALLBACK_EVENT_OUTPUT */
    if ((Rval == ARM_OK) && (pEventData != NULL) && (pHdlr != NULL)) {
        if (Rval == ARM_OK) {
            for (UINT32 FDIdx = 0U; FDIdx < CvAlgoOut.pOutput->num_of_buf; FDIdx++) {
                cvflow_port_header_t FdgOutHeader = {0U};

                Rval = ArmHeader_GetFdgHeader(CvAlgoOut.pOutput, FDIdx, &FdgOutHeader);

                if (Rval == ARM_OK) {
                    /* Get number of packed outputs in this flexidag */
                    HeaderInfo[FDIdx].num_packed = FdgOutHeader.num_packed;

                    for (UINT32 OutIdx = 0U; OutIdx < FdgOutHeader.num_packed; OutIdx++) {
                        cvflow_buffer_desc_t FdgOutDescriptor = {0U};

                        Rval = ArmHeader_GetFdgDesc(CvAlgoOut.pOutput, FDIdx, OutIdx, &FdgOutDescriptor);
                        if (Rval == ARM_OK) {
                            /* Get the size of a specific output */
                            HeaderInfo[FDIdx].OutputDescriptor[OutIdx].NumOfByte = FdgOutDescriptor.num_bytes;
                            HeaderInfo[FDIdx].buffer_offset[OutIdx] = FdgOutHeader.buffer_offset[OutIdx];
                            HeaderInfo[FDIdx].OutputIndex[OutIdx].FDIdx= FDIdx;
                            HeaderInfo[FDIdx].OutputIndex[OutIdx].OutputIdx = OutIdx;
                        }
                    }
                } else {
                    ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## ArmHeader_GetFdgDesc failed", 0U, 0U);
                }
            }

            SendCallback(CALLBACK_EVENT_OUTPUT, pHdlr, &CvAlgoOut);
        }
    }
}


/*---------------------------------------------------------------------------*\
 * SingleFDHeader APIs
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: SingleFDHeader_Query
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
static UINT32 SingleFDHeader_Query(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_QUERY_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Query() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_PREOPEN) {
            ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Query() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pAlgoObj == NULL) {
            ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Query() fail (pAlgoObj is null)", 0U, 0U);
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
 *  @RoutineName:: SingleFDHeader_Create
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
static UINT32 SingleFDHeader_Create(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_CREATE_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;
    static UINT32     Idx     = 0U;
    CCF_OUTPUT_INFO_s OutInfo = {0};
    CCF_CREATE_CFG_s  CreateCfg;
    CCF_REGCB_CFG_s   RegCbCfg;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Create() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_PREOPEN) {
            ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Create() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pAlgoBuf == NULL) {
            ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Create() fail (pAlgoBuf is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pCfg->NumFD != 1U) {
            ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Create() fail (invalid NumFD %u)", pCfg->NumFD, 0U);
            Rval = ARM_NG;
        } else {
            for (UINT32 i = 0U; i < pCfg->NumFD; i ++) {
                if (pCfg->pBin[i] == NULL) {
                    ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Create() fail (pBin[%u] is null)", i, 0U);
                    Rval = ARM_NG;
                }
            }
        }
    }

    /* 2. Create memory pool */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        Rval = ArmMemPool_Create(pCfg->pAlgoBuf, &pHdlr->MemPoolId);
    }

    /* 3. memset CCF_HANDLE_s */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        void *pVoid;
        const CCF_HANDLE_s *pCCFHandle;
        for (UINT32 i = 0U; i < pCfg->NumFD; i ++) {
            pCCFHandle = &(pHdlr->FDs[i]);
            //pVoid = (void *) pCCFHandle
            (void) ArmStdC_memcpy(&pVoid, &pCCFHandle, sizeof(void *));
            (void) ArmStdC_memset(pVoid, 0x0, sizeof(CCF_HANDLE_s));
        }
    }

    /* 4. Create flexidag */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        CreateCfg.pBin        = pCfg->pBin[0];
        CreateCfg.MemPoolId   = pHdlr->MemPoolId;
        CreateCfg.InBufDepth  = 0U;
        CreateCfg.OutBufDepth = 0U;

        CreateCfg.RunTskCtrl.StackSz  = DEFAULT_TSK_STACK_SIZE;
        CreateCfg.RunTskCtrl.Priority = DEFAULT_TSK_PRIORITY;
        CreateCfg.RunTskCtrl.CoreSel  = DEFAULT_TSK_CORE_SEL;

        CreateCfg.CBTskCtrl.StackSz  = DEFAULT_TSK_STACK_SIZE;
        CreateCfg.CBTskCtrl.Priority = DEFAULT_TSK_PRIORITY;
        CreateCfg.CBTskCtrl.CoreSel  = DEFAULT_TSK_CORE_SEL;

        ArmStdC_strcpy(CreateCfg.FDName, MAX_CCF_NAME_LEN, "SingleNN_X");
        CreateCfg.FDName[9] = '0' + (Idx % 10U);

        Idx ++;

        Rval = CvCommFlexi_Create(&(pHdlr->FDs[0]), &CreateCfg, &OutInfo);
    }

    /* 5. Register Callback */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        RegCbCfg.Mode = 0;    // register
        RegCbCfg.Callback = SingleFDHeaderCallback;

        Rval = CvCommFlexi_RegCallback(&(pHdlr->FDs[0]), &RegCbCfg);
    }

    /* 6. Fill output size and change state */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        pCfg->OutputNum = OutInfo.OutputNum;
        for (UINT32 i = 0U; i < OutInfo.OutputNum; i ++) {
            pCfg->OutputSz[i] = OutInfo.OutputSz[i];
        }
        pHdlr->State    = CVALGO_STATE_INITED;
        pHdlr->pAlgoBuf = pCfg->pAlgoBuf;
    }

    /* 7. Print the Max mempool usage */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        UINT32 UsedSize = 0U;
        Rval = ArmMemPool_GetUsage(pHdlr->MemPoolId, &UsedSize);
        if (Rval == ARM_OK) {
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
 *  @RoutineName:: SingleFDHeader_Delete
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
static UINT32 SingleFDHeader_Delete(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_DELETE_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Delete() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Delete() fail (invalid State %u)", pHdlr->State, 0U);
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
 *  @RoutineName:: SingleFDHeader_Feed
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
static UINT32 SingleFDHeader_Feed(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_FEED_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;
    //HEADER_INFO_s HeaderInfo;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Feed() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Feed() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pIn == NULL) {
            ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Feed() fail (pIn is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pOut == NULL) {
            ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Feed() fail (pOut is null)", 0U, 0U);
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
        FeedCfg.pOut        = pCfg->pOut;
        FeedCfg.pUserData   = pCfg->pUserData;
        FeedCfg.pCvAlgoHdlr = pHdlr;
        FeedCfg.pInternal   = NULL;

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
 *  @RoutineName:: SingleFDHeader_Control
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
static UINT32 SingleFDHeader_Control(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_CTRL_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Control() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Control() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Send message to flexidag */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        switch (pCfg->CtrlType) {
        case CTRL_TYPE_ROI: {
            CCF_SEND_MSG_CFG_s SendMsgCfg;

            SendMsgCfg.UUID    = SINGLE_FD_HEADER_UUID;
            SendMsgCfg.pMsg    = pCfg->pCtrlParam;
            SendMsgCfg.MsgSize = sizeof(amba_roi_config_t);

            Rval = CvCommFlexi_SendMsg(&(pHdlr->FDs[0]), &SendMsgCfg);
        }
        break;
        case CTRL_TYPE_DMSG: {
            Rval = CvCommFlexi_DumpLog(&(pHdlr->FDs[0]));
        }
        break;
        case CTRL_TYPE_SEND_SECRET: {
            CCF_SEND_MSG_CFG_s SendMsgCfg;

            SendMsgCfg.UUID    = SINGLE_FD_HEADER_UUID;
            SendMsgCfg.pMsg    = pCfg->pCtrlParam;
            SendMsgCfg.MsgSize = 16U;  // 16 bytes secret data

            Rval = CvCommFlexi_SendPrivateMsg(&(pHdlr->FDs[0]), &SendMsgCfg);
        }

        break;
        default:
            ArmLog_ERR(ARM_LOG_SINGLE_HEADER, "## Control() fail (unknown control type %u)", pCfg->CtrlType, 0U);
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

SVC_CV_ALGO_OBJ_s CvAlgoObj_SingleFDHeader = {
    .Name          = "SingleFDHeader",
    .Query         = SingleFDHeader_Query,
    .Create        = SingleFDHeader_Create,
    .Delete        = SingleFDHeader_Delete,
    .Feed          = SingleFDHeader_Feed,
    .Control       = SingleFDHeader_Control,
};

