/**
*  @file CvCommFlexi.c
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
*   @details CV communication (The communication between cvtask and app) for flexidag
*
*/

#include "CvCommFlexi.h"

#define ARM_LOG_CCF    "CCF"

/*---------------------------------------------------------------------------*\
 * Static functions
\*---------------------------------------------------------------------------*/
static ArmTaskRet_t CvCommFlexiRunEntry(ArmTaskEntryArg_t EntryArg);
static ArmTaskRet_t CvCommFlexiCBEntry(ArmTaskEntryArg_t EntryArg);

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAX
 *
 *  @Description:: Return maximum value between input a and b
 *
 *  @Input      ::
 *     a:          The input a
 *     b:          The input b
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      The maximum value between a and b
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 MAX(UINT32 a, UINT32 b)
{
    UINT32 tmp;

    if (a > b) {
        tmp = a;
    } else {
        tmp = b;
    }

    return tmp;
}

/*---------------------------------------------------------------------------*\
 * CvCommFlexi resource functions
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: GetAvailableBufIdx
 *
 *  @Description:: Get available buffer index
 *
 *  @Input      ::
 *     pBufCtrl:   The Buffer control
 *
 *  @Output     ::
 *     pIndex:     The buffer index
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 GetAvailableBufIdx(CCF_FLEXI_BUF_CTRL_s *pBufCtrl, UINT32 *pIndex)
{
    UINT32 Rval = ARM_OK;
    UINT32 WaitTime = 10; // 10ms

    /* 1. Sanity check */
    if (pBufCtrl == NULL) {
        ArmLog_ERR(ARM_LOG_CCF, "## pBufCtrl is null", 0U, 0U);
        Rval = ARM_NG;
    }
    if (pIndex == NULL) {
        ArmLog_ERR(ARM_LOG_CCF, "## pIndex is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Get available index */
    while (Rval == ARM_OK) {
        (void) ArmMutex_Take(&pBufCtrl->Mutex);

        if (pBufCtrl->AvblNum > 0U) {
            pBufCtrl->AvblNum--;

            *pIndex = pBufCtrl->Wp;
            pBufCtrl->Wp ++;
            pBufCtrl->Wp %= pBufCtrl->MaxAvblNum;

            (void) ArmMutex_Give(&pBufCtrl->Mutex);
            break;
        } else {
            (void) ArmMutex_Give(&pBufCtrl->Mutex);
            ArmLog_WARN(ARM_LOG_CCF, "Warning!! Cannot get index, wait %ums", WaitTime, 0U);
            (void) ArmTask_Sleep(WaitTime);
        }
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: FreeBufIdx
 *
 *  @Description:: Free buffer index
 *
 *  @Input      ::
 *     pBufCtrl:   The Buffer control
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 FreeBufIdx(CCF_FLEXI_BUF_CTRL_s *pBufCtrl)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check */
    if (pBufCtrl == NULL) {
        ArmLog_ERR(ARM_LOG_CCF, "## pBufCtrl is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Free index */
    if (Rval == ARM_OK) {
        (void) ArmMutex_Take(&pBufCtrl->Mutex);

        if (pBufCtrl->AvblNum >= pBufCtrl->MaxAvblNum) {
            ArmLog_DBG(ARM_LOG_CCF, "## can't free index due to incorrect AvblNum %u", pBufCtrl->AvblNum, 0U);
            Rval = ARM_NG;
        } else {
            pBufCtrl->AvblNum++;
        }

        (void) ArmMutex_Give(&pBufCtrl->Mutex);
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FillTskCtrl
 *
 *  @Description:: Fill the stack size and tsk priority
 *
 *  @Input      ::
 *    pInTskCtrl:  The input task control
 *
 *  @Output     ::
 *    pOutTskCtrl: The output task control
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void FillTskCtrl(CCF_TSK_CTRL_s *pOutTskCtrl, const CCF_TSK_CTRL_s *pInTskCtrl)
{
    UINT32 FinalStackSz = 0U, FinalPrioirty = 0U, FinalCoreSel = 0U;

    if (pInTskCtrl->StackSz != 0U) {
        FinalStackSz = pInTskCtrl->StackSz;
    } else {
        FinalStackSz = DEFAULT_TSK_STACK_SIZE;
    }

    if (pInTskCtrl->Priority != 0U) {
        FinalPrioirty = pInTskCtrl->Priority;
    } else {
        FinalPrioirty = DEFAULT_TSK_PRIORITY;
    }

    if (pInTskCtrl->CoreSel != 0U) {
        FinalCoreSel = pInTskCtrl->CoreSel;
    } else {
        FinalCoreSel = DEFAULT_TSK_CORE_SEL;
    }

    pOutTskCtrl->StackSz  = FinalStackSz;
    pOutTskCtrl->Priority = FinalPrioirty;
    pOutTskCtrl->CoreSel  = FinalCoreSel;
    return;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CreateResourceNames
 *
 *  @Description:: Fill the resource names
 *
 *  @Input      :: None
 *
 *  @Output     ::
 *     pNames:     The resource names
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void CreateResourceNames(CCF_RESOURCE_NAME_s *pNames)
{
    static UINT32 index = 0U;

    // RunTask
    ArmStdC_strcpy(&(pNames->RunTask[0]), MAX_CCF_NAME_LEN, "CCFRunTask_XXX");
    pNames->RunTask[11] = '0' + ((index / 100U) % 10U);
    pNames->RunTask[12] = '0' + ((index % 100U) / 10U);
    pNames->RunTask[13] = '0' + (index % 10U);
    pNames->RunTask[14] = '\0';

    // CBTask
    ArmStdC_strcpy(&(pNames->CBTask[0]), MAX_CCF_NAME_LEN, "CCFCBTask_XXX");
    pNames->CBTask[10] = '0' + ((index / 100U) % 10U);
    pNames->CBTask[11] = '0' + ((index % 100U) / 10U);
    pNames->CBTask[12] = '0' + (index % 10U);
    pNames->CBTask[13] = '\0';

    // RunMsgQ
    ArmStdC_strcpy(&(pNames->RunMsgQ[0]), MAX_CCF_NAME_LEN, "CCFRunMsgQ_XXX");
    pNames->RunMsgQ[11] = '0' + ((index / 100U) % 10U);
    pNames->RunMsgQ[12] = '0' + ((index % 100U) / 10U);
    pNames->RunMsgQ[13] = '0' + (index % 10U);
    pNames->RunMsgQ[14] = '\0';

    // CBMsgQ
    ArmStdC_strcpy(&(pNames->CBMsgQ[0]), MAX_CCF_NAME_LEN, "CCFCBMsgQ_XXX");
    pNames->CBMsgQ[10] = '0' + ((index / 100U) % 10U);
    pNames->CBMsgQ[11] = '0' + ((index % 100U) / 10U);
    pNames->CBMsgQ[12] = '0' + (index % 10U);
    pNames->CBMsgQ[13] = '\0';

    // InBufMtx
    ArmStdC_strcpy(&(pNames->InBufMtx[0]), MAX_CCF_NAME_LEN, "CCFInBufMut_XXX");
    pNames->OutBufMtx[12] = '0' + ((index / 100U) % 10U);
    pNames->OutBufMtx[13] = '0' + ((index % 100U) / 10U);
    pNames->OutBufMtx[14] = '0' + (index % 10U);
    pNames->OutBufMtx[15] = '\0';

    // OutBufMtx
    ArmStdC_strcpy(&(pNames->OutBufMtx[0]), MAX_CCF_NAME_LEN, "CCFOutBufMut_XXX");
    pNames->OutBufMtx[13] = '0' + ((index / 100U) % 10U);
    pNames->OutBufMtx[14] = '0' + ((index % 100U) / 10U);
    pNames->OutBufMtx[15] = '0' + (index % 10U);
    pNames->OutBufMtx[16] = '\0';

    // update index
    index ++;
    if (index > 999U) {
        index = 0U;
    }

    return;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CreateCCFResource
 *
 *  @Description:: Create CvCommFlexi resource
 *                 (allocate buffer -> mutex -> MsgQ -> run/cb tasks)
 *
 *  @Input      ::
 *     pHdlr:      The CvCommFlexi handler
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 CreateCCFResource(CCF_HANDLE_s *pHdlr)
{
    UINT32 Rval = ARM_OK;

    /* Sanity check */
    if (pHdlr == NULL) {
        ArmLog_ERR(ARM_LOG_CCF, "## CreateCCFResource fail (pHdlr is null)", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 1-1. Allocate buffer for task stack */
    if (Rval == ARM_OK) {
        Rval = ArmMemPool_Allocate(pHdlr->MemPoolId, pHdlr->RunTsk.Ctrl.StackSz, &(pHdlr->RunTsk.StackBuf));
    }
    if (Rval == ARM_OK) {
        Rval = ArmMemPool_Allocate(pHdlr->MemPoolId, pHdlr->CBTsk.Ctrl.StackSz, &(pHdlr->CBTsk.StackBuf));
    }

    /* 1-2. Allocate buffer for input buffers */
    if (Rval == ARM_OK) {
        UINT32 InBufSize = MAX(sizeof(memio_source_recv_picinfo_t), sizeof(memio_source_recv_raw_t));

        pHdlr->FlexiBuf.In.Wp         = 0U;
        pHdlr->FlexiBuf.In.AvblNum    = pHdlr->InBufDepth;
        pHdlr->FlexiBuf.In.MaxAvblNum = pHdlr->InBufDepth;

        for (UINT32 i = 0U; i < pHdlr->InBufDepth; i++) {
            for (UINT32 j = 0U; j < (UINT32) FLEXIDAG_MAX_OUTPUTS; j++) {
                if (Rval == ARM_OK) {
                    Rval = ArmMemPool_Allocate(pHdlr->MemPoolId, InBufSize,
                                               &(pHdlr->FlexiBuf.In.Buf[i].buf[j]));
                }
            }
        }
    }

    /* 1-3. Allocate buffer for output buffers */
    if (Rval == ARM_OK) {
        pHdlr->FlexiBuf.Out.Wp         = 0U;
        pHdlr->FlexiBuf.Out.AvblNum    = pHdlr->OutBufDepth;
        pHdlr->FlexiBuf.Out.MaxAvblNum = pHdlr->OutBufDepth;

        for (UINT32 i = 0U; i < pHdlr->OutBufDepth; i++) {
            pHdlr->FlexiBuf.Out.Buf[i].num_of_buf = pHdlr->Handle.mem_req.flexidag_num_outputs;
            for (UINT32 j = 0U; j < pHdlr->Handle.mem_req.flexidag_num_outputs; j++) {
                if (Rval == ARM_OK) {
                    Rval = ArmMemPool_Allocate(pHdlr->MemPoolId, pHdlr->Handle.mem_req.flexidag_output_buffer_size[j],
                                               &(pHdlr->FlexiBuf.Out.Buf[i].buf[j]));
                }
            }
        }
    }

    /* 2. Create resource names */
    if (Rval == ARM_OK) {
        CreateResourceNames(&(pHdlr->ResourceNames));
    }

    /* 3. Create mutex */
    if (Rval == ARM_OK) {
        Rval = ArmMutex_Create(&(pHdlr->FlexiBuf.In.Mutex), &(pHdlr->ResourceNames.InBufMtx[0]));
    }
    if (Rval == ARM_OK) {
        Rval = ArmMutex_Create(&(pHdlr->FlexiBuf.Out.Mutex), &(pHdlr->ResourceNames.OutBufMtx[0]));
    }

    /* 4. Create message queue */
    if (Rval == ARM_OK) {
        Rval = ArmMsgQueue_Create(&pHdlr->RunMsgQ, &(pHdlr->ResourceNames.RunMsgQ[0]), sizeof(CCF_RUN_MQ_ITEM_s),
                                  MAX_CCF_MSG_QUEUE_NUM, &pHdlr->RunMsgQBuf[0]);
    }
    if (Rval == ARM_OK) {
        Rval = ArmMsgQueue_Create(&pHdlr->CBMsgQ, &(pHdlr->ResourceNames.CBMsgQ[0]), sizeof(CCF_CB_MQ_ITEM_s),
                                  MAX_CCF_MSG_QUEUE_NUM, &pHdlr->CBMsgQBuf[0]);
    }

    /* 5. Create run and callback task */
    if (Rval == ARM_OK) {
        ARM_TASK_CREATE_ARG_s  TskArg = {0};
        ArmTaskEntryArg_t EntryArg;
        // EntryArg = (ArmTaskEntryArg_t) pHdlr;
        (void) ArmStdC_memcpy(&EntryArg, &pHdlr, sizeof(void *));

        TskArg.TaskPriority  = pHdlr->RunTsk.Ctrl.Priority;
        TskArg.EntryFunction = CvCommFlexiRunEntry;
        TskArg.EntryArg      = EntryArg;
        TskArg.pStackBase    = pHdlr->RunTsk.StackBuf.pBuffer;
        TskArg.StackByteSize = pHdlr->RunTsk.Ctrl.StackSz;
        TskArg.CoreSel       = pHdlr->RunTsk.Ctrl.CoreSel;

        Rval = ArmTask_Create(&(pHdlr->RunTsk.Tsk), pHdlr->ResourceNames.RunTask, &TskArg);
    }
    if (Rval == ARM_OK) {
        ARM_TASK_CREATE_ARG_s  TskArg = {0};
        ArmTaskEntryArg_t EntryArg;
        // EntryArg = (ArmTaskEntryArg_t) pHdlr;
        (void) ArmStdC_memcpy(&EntryArg, &pHdlr, sizeof(void *));

        TskArg.TaskPriority  = pHdlr->CBTsk.Ctrl.Priority;
        TskArg.EntryFunction = CvCommFlexiCBEntry;
        TskArg.EntryArg      = EntryArg;
        TskArg.pStackBase    = pHdlr->CBTsk.StackBuf.pBuffer;
        TskArg.StackByteSize = pHdlr->CBTsk.Ctrl.StackSz;
        TskArg.CoreSel       = pHdlr->CBTsk.Ctrl.CoreSel;

        Rval = ArmTask_Create(&(pHdlr->CBTsk.Tsk), pHdlr->ResourceNames.CBTask, &TskArg);
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: DeleteCCFResource
 *
 *  @Description:: Delete CvCommFlexi resource
 *                 (run/cb tasks -> MsgQ -> mutex)
 *
 *  @Input      ::
 *     pHdlr:      The CvCommFlexi handler
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 DeleteCCFResource(CCF_HANDLE_s *pHdlr)
{
    UINT32 Rval = ARM_OK;

    /* Sanity check */
    if (pHdlr == NULL) {
        ArmLog_ERR(ARM_LOG_CCF, "## DeleteCCFResource fail (pHdlr is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        /* 1. Clear all callback */
        for (UINT32 Idx = 0U; Idx < MAX_CCF_OUT_CB_NUM; Idx++) {
            pHdlr->OutCallback[Idx] = NULL;
        }

        /* 2. Delete run and callback task */
        (void) ArmTask_Delete(&(pHdlr->RunTsk.Tsk));
        (void) ArmTask_Delete(&(pHdlr->CBTsk.Tsk));

        /* 3. Delete message queue */
        (void) ArmMsgQueue_Delete(&(pHdlr->RunMsgQ), pHdlr->ResourceNames.RunMsgQ);
        (void) ArmMsgQueue_Delete(&(pHdlr->CBMsgQ), pHdlr->ResourceNames.CBMsgQ);

        /* 4. Delete mutex */
        (void) ArmMutex_Delete(&(pHdlr->FlexiBuf.In.Mutex));
        (void) ArmMutex_Delete(&(pHdlr->FlexiBuf.Out.Mutex));

        /* 5. Free memory for stack and out buffer */
        (void) ArmMemPool_Free(pHdlr->MemPoolId, &(pHdlr->RunTsk.StackBuf));
        (void) ArmMemPool_Free(pHdlr->MemPoolId, &(pHdlr->CBTsk.StackBuf));
        {
            const CCF_FLEXI_BUF_s *pBuf = &(pHdlr->FlexiBuf);

            for (UINT32 i = 0U; i < pBuf->Out.MaxAvblNum; i++) {    // depth
                for (UINT32 j = 0U; j < pHdlr->Handle.mem_req.flexidag_num_outputs; j++) {   // output number
                    if (Rval == ARM_OK) {
                        Rval = ArmMemPool_Free(pHdlr->MemPoolId, &(pBuf->Out.Buf[i].buf[j]));
                    }
                }
            }
        }
    }
    return Rval;
}

/*---------------------------------------------------------------------------*\
 * Flexidag related functions
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: OpenAndInitFlexidag
 *
 *  @Description:: Open and init FlexiDAG with memory allocation
 *                 (AmbaCV_FlexidagOpen() -> AmbaCV_FlexidagInit())
 *
 *  @Input      ::
 *    Slot:        The slot id
 *    pBin:        The flexibin path
 *    OutBufDepth: The output buffer depth
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 OpenAndInitFlexidag(CCF_HANDLE_s *pHdlr, flexidag_memblk_t  *pBin)
{
    UINT32 Rval;
    AMBA_CV_FLEXIDAG_HANDLE_s *pHandle = &(pHdlr->Handle);
    CCF_FLEXI_BUF_s           *pFlexiBuf = &(pHdlr->FlexiBuf);

    /* 1. Open flexidag (call AmbaCV_FlexidagOpen())  */
    {
        Rval = AmbaCV_FlexidagOpen(pBin, pHandle);
        if(Rval != 0U) {
            ArmLog_ERR(ARM_LOG_CCF, "## AmbaCV_FlexidagOpen fail (Rval 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    /* 2. Allocate memory for State and Temp buffer */
    if (Rval == ARM_OK) {     // State
        if (pHandle->mem_req.flexidag_state_buffer_size != 0U) {
            Rval = ArmMemPool_Allocate(pHdlr->MemPoolId, pHandle->mem_req.flexidag_state_buffer_size, &(pFlexiBuf->State));
        } else {
            (void) ArmStdC_memset(&(pFlexiBuf->State), 0, sizeof(flexidag_memblk_t));
        }
    }

    if (Rval == ARM_OK) {     // Temp
        if (pHandle->mem_req.flexidag_temp_buffer_size != 0U) {
            Rval = ArmMemPool_Allocate(pHdlr->MemPoolId, pHandle->mem_req.flexidag_temp_buffer_size, &(pFlexiBuf->Temp));
        } else {
            (void) ArmStdC_memset(&(pFlexiBuf->Temp), 0, sizeof(flexidag_memblk_t));
        }
    }

    /* 3. Initialize flexidag (call AmbaCV_FlexidagInit()) */
    if (Rval == ARM_OK) {
        AMBA_CV_FLEXIDAG_INIT_s init;

        (void) ArmStdC_memset(&init, 0, sizeof(AMBA_CV_FLEXIDAG_INIT_s));

        init.state_buf = pFlexiBuf->State;
        init.temp_buf  = pFlexiBuf->Temp;
        Rval = AmbaCV_FlexidagInit(pHandle, &init);
        if(Rval != 0U) {
            ArmLog_ERR(ARM_LOG_CCF, "## AmbaCV_FlexidagInit fail (Rval 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    return Rval;
}


/*---------------------------------------------------------------------------*\
 *  @RoutineName:: RunFlexidag
 *
 *  @Description:: Run flexidag one time
 *                 (AmbaCV_FlexidagRun())
 *
 *  @Input      ::
 *    Slot:        The slot id
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 RunFlexidag(CCF_HANDLE_s *pHdlr, AMBA_CV_FLEXIDAG_IO_s *pIn, AMBA_CV_FLEXIDAG_IO_s *pOut, UINT32 *pToken)
{
    UINT32 Rval = ARM_OK, i = 0U;

    /* Sanity check */
    if ((pHdlr == NULL) || (pIn == NULL) || (pOut == NULL) || (pToken == NULL)) {
        ArmLog_ERR(ARM_LOG_CCF, "## RunFlexidag fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 1. Cache clean for IN buffer (AMBA_CV_FLEXIDAG_IO_s) */
    if (Rval == ARM_OK) {
        for (i = 0U; i < pIn->num_of_buf; i++) {
            if (pIn->buf[i].buffer_cacheable == 1U) {
                (void) ArmMemPool_CacheClean(&(pIn->buf[i]));
            }
        }
    }

    /* 2. Cache clean for OUT buffer (AMBA_CV_FLEXIDAG_IO_s) */
    if (Rval == ARM_OK) {
        for (i = 0U; i < pHdlr->Handle.mem_req.flexidag_num_outputs; i++) {

            // If flexidag output includes some padding bytes, it will be random number. memset to 0.
            (void) ArmStdC_memset(pOut->buf[i].pBuffer, 0, pOut->buf[i].buffer_size);
            if (pOut->buf[i].buffer_cacheable == 1U) {
                (void) ArmMemPool_CacheClean(&(pOut->buf[i]));
            }
        }
    }

    /* 3. Call AmbaCV_FlexidagRunNonBlock() */
    if (Rval == ARM_OK) {
        Rval = AmbaCV_FlexidagRunNonBlock(&(pHdlr->Handle), NULL, NULL, pIn, pOut, pToken);
        if (Rval != 0U) {
            ArmLog_ERR(ARM_LOG_CCF, "## AmbaCV_FlexidagRunNonBlock fail (Rval 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 * CvCommFlexi task entry
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexiRunEntry
 *
 *  @Description:: The task to run flexidag
 *
 *  @Input      ::
 *     EntryArg:   pointer to CvCommFlexi handler (CCF_HANDLE_s *)
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*---------------------------------------------------------------------------*/
static ArmTaskRet_t CvCommFlexiRunEntry(ArmTaskEntryArg_t EntryArg)
{
    UINT32 Rval = ARM_OK;
    UINT32 Token = 0U;
    CCF_HANDLE_s *pHdlr;
    CCF_RUN_MQ_ITEM_s RunItem;
    CCF_CB_MQ_ITEM_s  CBItem;

    (void)EntryArg;
    //pHdlr = (CCF_HANDLE_s *)EntryArg;
    (void)ArmStdC_memcpy(&pHdlr, &EntryArg, sizeof(void *));

    (void)ArmStdC_memset(&RunItem, 0, sizeof(CCF_RUN_MQ_ITEM_s));
    (void)ArmStdC_memset(&CBItem, 0, sizeof(CCF_CB_MQ_ITEM_s));

    if (pHdlr != NULL) {
        ArmLog_STR(ARM_LOG_CCF, "Create CvCommFlexi Run task (%s)", pHdlr->FDName, NULL);

        for(;;) {
            /* 1. Get data from RunMsgQueue */
            Rval = ArmMsgQueue_Recv(&(pHdlr->RunMsgQ), &RunItem);

            /* 2. Run FlexiDag */
            if (Rval == ARM_OK) {
                Rval = RunFlexidag(pHdlr, RunItem.pIn, RunItem.pOut, &Token);
            }

            /* 3. Send message to CBMsgQueue */
            if (Rval == ARM_OK) {
                CBItem.Token            = Token;
                CBItem.pOut             = RunItem.pOut;
                CBItem.pUserData        = RunItem.pUserData;
                CBItem.pCvAlgoHdlr      = RunItem.pCvAlgoHdlr;
                CBItem.pInternal        = RunItem.pInternal;
                CBItem.IsInternalInBuf  = RunItem.IsInternalInBuf;
                CBItem.IsInternalOutBuf = RunItem.IsInternalOutBuf;

                Rval = ArmMsgQueue_Send(&(pHdlr->CBMsgQ), &CBItem);
            }

            /* 4. Exit for loop if there is anything wrong */
            if (Rval != ARM_OK) {
                ArmLog_ERR(ARM_LOG_CCF,"## Exit CvCommFlexi Run task due to errors (Rval = 0x%x)", Rval, 0U);
                break;
            }
        }

        ArmLog_STR(ARM_LOG_CCF, "CvCommFlexi Run task terminate (%s)", pHdlr->FDName, NULL);
    } else {
        ArmLog_ERR(ARM_LOG_CCF,"## CvCommFlexi Run task shutdown because of null pHdlr", 0U, 0U);
    }

    return ARM_TASK_NULL_RET;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexiCBEntry
 *
 *  @Description:: The task to run flexidag output callback
 *
 *  @Input      ::
 *     EntryArg:   pointer to CvCommFlexi handler (CCF_HANDLE_s *)
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*---------------------------------------------------------------------------*/
static ArmTaskRet_t CvCommFlexiCBEntry(ArmTaskEntryArg_t EntryArg)
{
    UINT32 Rval = ARM_OK;
    CCF_HANDLE_s *pHdlr;
    CCF_CB_MQ_ITEM_s  CBItem;
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo = {0};

    (void)EntryArg;
    //pHdlr = (CCF_HANDLE_s *)EntryArg;
    (void)ArmStdC_memcpy(&pHdlr, &EntryArg, sizeof(void *));

    (void)ArmStdC_memset(&CBItem, 0, sizeof(CCF_CB_MQ_ITEM_s));


    if (pHdlr != NULL) {
        ArmLog_STR(ARM_LOG_CCF, "Create CvCommFlexi CB task (%s)", pHdlr->FDName, NULL);

        for(;;) {
            /* 1. Get data from CBMsgQueue */
            Rval = ArmMsgQueue_Recv(&(pHdlr->CBMsgQ), &CBItem);

            /* 2. Call AmbaCV_FlexidagWaitRunFinish to wait flexidag finish */
            if (Rval == ARM_OK) {
                Rval = AmbaCV_FlexidagWaitRunFinish(&(pHdlr->Handle), CBItem.Token, &RunInfo);
                if (Rval != 0U) {
                    ArmLog_ERR(ARM_LOG_CCF, "## AmbaCV_FlexidagWaitRunFinish fail (Rval 0x%x)", Rval, 0U);
                    Rval = ARM_NG;
                } else {
                    Rval = ARM_OK;
                }
            }

            if (Rval == ARM_OK) {
                for (UINT32 i= 0U; i < CBItem.pOut->num_of_buf; i++) {
                    if (CBItem.pOut->buf[i].buffer_cacheable == 1U) {
                        (void) ArmMemPool_Invalid(&(CBItem.pOut->buf[i]));
                    }
                }
            }

            /* 3. Release the internal in buffer if necessary */
            if (Rval == ARM_OK) {
                if (CBItem.IsInternalInBuf == 1U) {
                    Rval = FreeBufIdx(&(pHdlr->FlexiBuf.In));
                }
            }

            /* 4. Print performance log */
            if (Rval == ARM_OK) {
                if (pHdlr->LogCnt != 0U) {
                    UINT32 diff;
                    diff = RunInfo.end_time - RunInfo.start_time;
                    ArmLog_DBG(ARM_LOG_CCF, "", 0U, 0U);
                    ArmLog_DBG(ARM_LOG_CCF, "--------------------------------------", 0U, 0U);
                    ArmLog_STR(ARM_LOG_CCF, "[FlexiDag]Name :             %s", pHdlr->FDName, NULL);
                    ArmLog_DBG(ARM_LOG_CCF, "[FlexiDag]RetCode :          %u", RunInfo.overall_retcode, 0U);
                    ArmLog_DBG(ARM_LOG_CCF, "[FlexiDag]Processing time :  %uus", diff, 0U);
                    ArmLog_DBG(ARM_LOG_CCF, "--------------------------------------", 0U, 0U);

                    pHdlr->LogCnt --;
                }
            }

            /* 5. Fill CCF_OUTPUT_s */
            if (Rval == ARM_OK) {
                pHdlr->OutData.pOut        = CBItem.pOut;
                pHdlr->OutData.pUserData   = CBItem.pUserData;
                pHdlr->OutData.pCvAlgoHdlr = CBItem.pCvAlgoHdlr;
                pHdlr->OutData.pInternal   = CBItem.pInternal;
            }

            /* 6. Call callback function */
            if (Rval == ARM_OK) {
                for (UINT32 Idx = 0U; Idx < MAX_CCF_OUT_CB_NUM; Idx++) {
                    const CCF_OUT_CALLBACK_f *pCCFOutHdlr = &(pHdlr->OutCallback[Idx]);
                    if ((*pCCFOutHdlr) == NULL) {
                        continue;
                    } else {
                        (*pCCFOutHdlr)(&(pHdlr->OutData));
                    }
                }
            }

            /* 7. Release the internal out buffer if necessary */
            if (Rval == ARM_OK) {
                if (CBItem.IsInternalOutBuf == 1U) {
                    Rval = FreeBufIdx(&(pHdlr->FlexiBuf.Out));
                }
            }

            /* 8. Exit for loop if there is anything wrong */
            if (Rval != ARM_OK) {
                ArmLog_ERR(ARM_LOG_CCF,"## Exit CvCommFlexi CB task due to errors (Rval = 0x%x)", Rval, 0U);
                break;
            }
        }

        ArmLog_STR(ARM_LOG_CCF, "CvCommFlexi CB task terminate (%s)", pHdlr->FDName, NULL);
    } else {
        ArmLog_ERR(ARM_LOG_CCF,"## CvCommFlexi CB task shutdown because of null pHdlr", 0U, 0U);
    }

    return ARM_TASK_NULL_RET;
}

/*---------------------------------------------------------------------------*\
 * CvCommFlexi APIs
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexi_Create
 *
 *  @Description:: Create flexidag and resource
 *
 *  @Input      ::
 *     pHdlr:      The CvCommFlexi handler
 *     pCfg:       The create config
 *
 *  @Output     ::
 *     pOutInfo:   The output size
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CvCommFlexi_Create(CCF_HANDLE_s *pHdlr, const CCF_CREATE_CFG_s *pCfg, CCF_OUTPUT_INFO_s *pOutInfo)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL) || (pOutInfo == NULL)) {
        ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_Open fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CCF_STATE_PREOPEN) {
            ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_Open fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        }
        if (pCfg->pBin == NULL) {
            ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_Open fail (pBin is null)", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Open and init flexidag */
    if (Rval == ARM_OK) {
        pHdlr->MemPoolId   = pCfg->MemPoolId;
        Rval = OpenAndInitFlexidag(pHdlr, pCfg->pBin);
        if(Rval != 0U) {
            ArmLog_ERR(ARM_LOG_CCF, "## AmbaCV_FlexidagOpen fail (Rval 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    /* 3. Fill other settings (OutBufDepth, TskCtrl, FDName ...) */
    if (Rval == ARM_OK) {
        FillTskCtrl(&(pHdlr->RunTsk.Ctrl), &(pCfg->RunTskCtrl));
        FillTskCtrl(&(pHdlr->CBTsk.Ctrl), &(pCfg->CBTskCtrl));

        pHdlr->InBufDepth  = pCfg->InBufDepth;
        pHdlr->OutBufDepth = pCfg->OutBufDepth;
        pHdlr->LogCnt      = DEFAULT_CCF_LOG_CNT;
        pHdlr->State       = CCF_STATE_INITED;
        ArmStdC_strcpy(pHdlr->FDName, MAX_CCF_NAME_LEN, pCfg->FDName);
    }

    /* 4. Create other resource (task, mutex, eventflag ...) */
    if (Rval == ARM_OK) {
        Rval = CreateCCFResource(pHdlr);
    }

    /* 5. Fill pOutInfo */
    if (Rval == ARM_OK) {
        pOutInfo->OutputNum = pHdlr->Handle.mem_req.flexidag_num_outputs;
        for (UINT32 i = 0U; i < pHdlr->Handle.mem_req.flexidag_num_outputs; i++) {
            pOutInfo->OutputSz[i] = pHdlr->Handle.mem_req.flexidag_output_buffer_size[i];
        }
    }

    /* 6. Print log */
    if (Rval == ARM_OK) {
        ArmLog_STR(ARM_LOG_CCF, "Flexidag [%s] open successfully", pHdlr->FDName, NULL);
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexi_Delete
 *
 *  @Description:: Delete flexidag and free resource
 *
 *  @Input      ::
 *     pHdlr:      The CvCommFlexi handler
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CvCommFlexi_Delete(CCF_HANDLE_s *pHdlr)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if (pHdlr == NULL) {
        ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_Close fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CCF_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_Close fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Delete controller resource */
    if (Rval == ARM_OK) {
        Rval = DeleteCCFResource(pHdlr);
    }

    /* 3. Delete Flexidag */
    if (Rval == ARM_OK) {
        Rval = AmbaCV_FlexidagClose(&(pHdlr->Handle));
        if(Rval != 0U) {
            ArmLog_ERR(ARM_LOG_CCF, "## AmbaCV_FlexidagClose fail (Rval 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    /* 4. Print log and Reset CCF_HANDLE_s */
    if (Rval == ARM_OK) {
        ArmLog_STR(ARM_LOG_CCF, "Flexidag [%s] close successfully", pHdlr->FDName, NULL);
        (void) ArmStdC_memset(pHdlr, 0, sizeof(CCF_HANDLE_s));
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexi_Feed
 *
 *  @Description:: Feed data to flexidag
 *
 *  @Input      ::
 *     pHdlr:      The CvCommFlexi handler
 *     pCfg:       The feed config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CvCommFlexi_Feed(CCF_HANDLE_s *pHdlr, const CCF_FEED_CFG_s *pCfg)
{
    UINT32            Rval      = ARM_OK;
    UINT32            OutBufIdx = 0U, InBufIdx = 0U;
    CCF_RUN_MQ_ITEM_s RunItem;

    (void)ArmStdC_memset(&RunItem, 0, sizeof(CCF_RUN_MQ_ITEM_s));

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_Feed fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CCF_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_Feed fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Check input buffer */
    if (Rval == ARM_OK) {
        if (pCfg->pIn == NULL) {   // use internal input buffer
            Rval = GetAvailableBufIdx(&(pHdlr->FlexiBuf.In), &InBufIdx);
            if (Rval == ARM_OK) {
                RunItem.pIn              = &(pHdlr->FlexiBuf.In.Buf[InBufIdx]);
                RunItem.IsInternalInBuf  = 1;

                if ((pCfg->pRaw != NULL) && (pCfg->pPic == NULL)) { // RAW mode
                    pHdlr->FlexiBuf.In.Buf[InBufIdx].num_of_buf = pCfg->pRaw->num_io;
                    for (UINT32 i = 0U; i < pCfg->pRaw->num_io; i++) {
                        (void) ArmStdC_memcpy(pHdlr->FlexiBuf.In.Buf[InBufIdx].buf[i].pBuffer, &(pCfg->pRaw->io[i]), sizeof(memio_source_recv_raw_t));
                    }
                } else if ((pCfg->pRaw == NULL) && (pCfg->pPic != NULL)) {  // PICINFO mode
                    pHdlr->FlexiBuf.In.Buf[InBufIdx].num_of_buf = 1U;
                    (void) ArmStdC_memcpy(pHdlr->FlexiBuf.In.Buf[InBufIdx].buf[0].pBuffer, pCfg->pPic, sizeof(memio_source_recv_picinfo_t));
                } else {
                    ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_Feed fail (Incorrect pRaw or pPic)", 0U, 0U);
                    Rval = ARM_NG;
                }
            } else {
                ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_Feed fail (pIn is null and no internal in buffers)", 0U, 0U);
                Rval = ARM_NG;
            }
        } else {
            RunItem.pIn              = pCfg->pIn;
            RunItem.IsInternalInBuf  = 0;
        }
    }

    /* 3. Check outputbuf */
    if (Rval == ARM_OK) {
        if (pCfg->pOut == NULL) {   // use internal out buffer
            Rval = GetAvailableBufIdx(&(pHdlr->FlexiBuf.Out), &OutBufIdx);
            if (Rval == ARM_OK) {
                RunItem.pOut             = &(pHdlr->FlexiBuf.Out.Buf[OutBufIdx]);
                RunItem.IsInternalOutBuf = 1U;
            } else {
                ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_Feed fail (pOut is null and no internal out buffers)", 0U, 0U);
            }
        } else {
            RunItem.pOut             = pCfg->pOut;
            RunItem.IsInternalOutBuf = 0U;
        }
    }

    /* 4. Send message to run task */
    if (Rval == ARM_OK) {
        RunItem.pUserData   = pCfg->pUserData;
        RunItem.pCvAlgoHdlr = pCfg->pCvAlgoHdlr;
        RunItem.pInternal   = pCfg->pInternal;

        Rval = ArmMsgQueue_Send(&(pHdlr->RunMsgQ), &RunItem);
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexi_RegCallback
 *
 *  @Description:: Register/Unregister CvCommFlexi output callback
 *
 *  @Input      ::
 *     pHdlr:      The CvCommFlexi handler
 *     pCfg:       The regcb config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CvCommFlexi_RegCallback(CCF_HANDLE_s *pHdlr, const CCF_REGCB_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;
    UINT32 Idx  = 0U;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_RegCallback fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if ((pHdlr->State != CCF_STATE_OPENED) && (pHdlr->State != CCF_STATE_INITED)) {
            ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_RegCallback fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        }
        if (pCfg->Callback == NULL) {
            ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_RegCallback fail (Callback is null)", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Register or Unregister */
    if (Rval == ARM_OK) {
        if (pCfg->Mode == 0U) {    // Find an empty space to register

            ArmLog_STR(ARM_LOG_CCF, "Flexidag [%s] register callback", pHdlr->FDName, NULL);

            for (Idx = 0U; Idx < MAX_CCF_OUT_CB_NUM; Idx++) {
                if ((pHdlr->OutCallback[Idx] == pCfg->Callback) || (pHdlr->OutCallback[Idx] == NULL)) {
                    pHdlr->OutCallback[Idx] = pCfg->Callback;
                    ArmLog_DBG(ARM_LOG_CCF, "Register callback to idx %u", Idx, 0U);
                    break;
                }
            }

            if (MAX_CCF_OUT_CB_NUM == Idx) {
                ArmLog_ERR(ARM_LOG_CCF, "No more empty space to register callback", 0U, 0U);
                Rval = ARM_NG;
            }
        } else { // Find callback and remove it

            ArmLog_STR(ARM_LOG_CCF, "Flexidag [%s] unregister callback", pHdlr->FDName, NULL);

            for (Idx = 0U; Idx < MAX_CCF_OUT_CB_NUM; Idx++) {
                if (pHdlr->OutCallback[Idx] == pCfg->Callback) {
                    pHdlr->OutCallback[Idx] = NULL;
                    ArmLog_DBG(ARM_LOG_CCF, "Remove callback idx %u", Idx, 0U);
                    break;
                }
            }

        }
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexi_SendMsg
 *
 *  @Description:: Send message to cvtask
 *
 *  @Input      ::
 *     pHdlr:      The CvCommFlexi handler
 *     pCfg:       The send msg config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CvCommFlexi_SendMsg(const CCF_HANDLE_s *pHdlr, const CCF_SEND_MSG_CFG_s *pCfg)
{
    UINT32 Rval   = ARM_OK;
    UINT16 FlowId = 0U;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_SendMsg fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CCF_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_SendMsg fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        }
        if (pCfg->pMsg == NULL) {
            ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_SendMsg fail (pMsg is null)", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Get flow ID by UUID */
    if (Rval == ARM_OK) {
        Rval = AmbaCV_FlexidagGetFlowIdByUUID(&(pHdlr->Handle), pCfg->UUID, &FlowId);
        if (Rval != 0U) {
            ArmLog_ERR(ARM_LOG_CCF,"## AmbaCV_FlexidagGetFlowIdByUUID fail (Rval 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    /* 3. Send message through Flow ID */
    if (Rval == ARM_OK) {
        AMBA_CV_FLEXIDAG_MSG_s Msg = {0};

        Msg.flow_id   = FlowId;
        Msg.vpMessage = pCfg->pMsg;
        Msg.length    = pCfg->MsgSize;
        Rval = AmbaCV_FlexidagSendMsg(&(pHdlr->Handle), &Msg);
        if (Rval != 0U) {
            ArmLog_ERR(ARM_LOG_CCF,"## AmbaCV_FlexidagSendMsg fail (Rval 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexi_SendPrivateMsg
 *
 *  @Description:: Send private message to cvtask
 *
 *  @Input      ::
 *     pHdlr:      The CvCommFlexi handler
 *     pCfg:       The send msg config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CvCommFlexi_SendPrivateMsg(const CCF_HANDLE_s *pHdlr, const CCF_SEND_MSG_CFG_s *pCfg)
{
    UINT32 Rval   = ARM_OK;
    UINT16 FlowId = 0U;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_SendPrivateMsg fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CCF_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_SendPrivateMsg fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        }
        if (pCfg->pMsg == NULL) {
            ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_SendPrivateMsg fail (pMsg is null)", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Get flow ID by UUID */
    if (Rval == ARM_OK) {
        Rval = AmbaCV_FlexidagGetFlowIdByUUID(&(pHdlr->Handle), pCfg->UUID, &FlowId);
        if (Rval != 0U) {
            ArmLog_ERR(ARM_LOG_CCF,"## AmbaCV_FlexidagGetFlowIdByUUID fail (Rval 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    /* 3. Send private message through Flow ID */
    if (Rval == ARM_OK) {
        AMBA_CV_FLEXIDAG_MSG_s Msg = {0};

        Msg.flow_id   = FlowId;
        Msg.vpMessage = pCfg->pMsg;
        Msg.length    = pCfg->MsgSize;
        Rval = AmbaCV_FlexidagSendPrivateMsg(&(pHdlr->Handle), &Msg);
        if (Rval != 0U) {
            ArmLog_ERR(ARM_LOG_CCF,"## AmbaCV_FlexidagSendPrivateMsg fail (Rval 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CvCommFlexi_DumpLog
 *
 *  @Description:: Print logs to console
 *
 *  @Input      ::
 *     pHdlr:      The CvCommFlexi handler
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CvCommFlexi_DumpLog(const CCF_HANDLE_s *pHdlr)
{
    UINT32 Rval   = ARM_OK;

    /* 1. Sanity check for parameters */
    if (pHdlr == NULL) {
        ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_DumpLog fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CCF_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_CCF, "## CvCommFlexi_DumpLog fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Dump log */
    if (Rval == ARM_OK) {
        ArmLog_DBG(ARM_LOG_CCF, "=== Dump Start ===", 0U, 0U);
        Rval = AmbaCV_FlexidagDumpLog(&(pHdlr->Handle), NULL, FLEXILOG_VIS_CVTASK | FLEXILOG_ECHO_TO_CONSOLE);
        ArmLog_DBG(ARM_LOG_CCF, "=== Dump End ===", 0U, 0U);
    }

    return Rval;
}

