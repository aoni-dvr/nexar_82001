/**
*  @file CtCvAlgoWrapper.c
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
*   @details The wrapper of SvcCvAlgo
*
*/

#include "CtCvAlgoWrapper.h"

#define ARM_LOG_CVALGO_WRAPPER    "CtCvAlgoWrapper"

/*---------------------------------------------------------------------------*\
 * Ring buffer to control feeding frame
\*---------------------------------------------------------------------------*/

typedef struct {
    AMBA_CV_FLEXIDAG_IO_s   In;
    AMBA_CV_FLEXIDAG_IO_s   Out;
    void                    *pUserData;
} FRAME_INFO_s;

typedef struct {
    FRAME_INFO_s            FrameInfo[MAX_BUF_DEPTH];
    UINT32                  Wp;               /* Write index*/
    UINT32                  AvblNum;          /* Available buffer number */
    UINT32                  MaxAvblNum;       /* The maximum available buffer number */
    ArmMutex_t              Mutex;            /* Protect AvblNum */
    char                    MutexName[MAX_PATH_LEN];
} FRAME_CTRL_s;

/*---------------------------------------------------------------------------*\
 * The control center for SvcCvAlgo
\*---------------------------------------------------------------------------*/

typedef struct {
    /* SvcCvAlgo */
    SVC_CV_ALGO_OBJ_s         *pCvAlgoObj;
    SVC_CV_ALGO_HANDLE_s      Hdlr;
    UINT32                    OutputNum;
    UINT32                    OutputSz[FLEXIDAG_MAX_OUTPUTS];

    /* Buffer control */
    UINT32                    MemPoolId;
    UINT32                    NumFD;
    flexidag_memblk_t         BinBuf[MAX_ALGO_FD];
    flexidag_memblk_t         CvAlgoBuf;
    FRAME_CTRL_s              FrameCtrl;

    /* Callback */
    SVC_CV_ALGO_CALLBACK_f    OutCallback[MAX_CALLBACK_NUM];
    SVC_CV_ALGO_OUTPUT_s      CallbackResult;

    /* Misc */
    UINT32                    IsCreated;
} CT_CVALGO_WRAPPER_CTRL_s;

static CT_CVALGO_WRAPPER_CTRL_s g_CtCvAlgoWrapperCtrl[MAX_SLOT_NUM];

/*---------------------------------------------------------------------------*\
 * Static functions
\*---------------------------------------------------------------------------*/

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
 *  @RoutineName:: GetAvailableFrameIdx
 *
 *  @Description:: Get available frame index
 *
 *  @Input      ::
 *     pFrameCtrl: The frame control
 *
 *  @Output     ::
 *     pIndex:     The available frame index
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 GetAvailableFrameIdx(FRAME_CTRL_s *pFrameCtrl, UINT32 *pIndex)
{
    UINT32 Rval = ARM_OK;
    UINT32 WaitTime = 10; // 10ms

    /* 1. Sanity check */
    if (pFrameCtrl == NULL) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## pFrameCtrl is null", 0U, 0U);
        Rval = ARM_NG;
    }
    if (pIndex == NULL) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## pIndex is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Get available index */
    if ((pFrameCtrl != NULL) && (pIndex != NULL)) {
        do {
            (void) ArmMutex_Take(&pFrameCtrl->Mutex);

            if (pFrameCtrl->AvblNum > 0U) {
                pFrameCtrl->AvblNum--;

                *pIndex = pFrameCtrl->Wp;
                pFrameCtrl->Wp ++;
                pFrameCtrl->Wp %= pFrameCtrl->MaxAvblNum;

                (void) ArmMutex_Give(&pFrameCtrl->Mutex);
                break;
            } else {
                (void) ArmMutex_Give(&pFrameCtrl->Mutex);
                ArmLog_WARN(ARM_LOG_CVALGO_WRAPPER, "Warning!! Cannot get index, wait %ums", WaitTime, 0U);
                (void) ArmTask_Sleep(WaitTime);
            }
        } while (Rval == ARM_OK);
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: FreeFrameIdx
 *
 *  @Description:: Free frame index
 *
 *  @Input      ::
 *     pFrameCtrl: The frame control
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 FreeFrameIdx(FRAME_CTRL_s *pFrameCtrl)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check */
    if (pFrameCtrl == NULL) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## pFrameCtrl is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Free index */
    if ((Rval == ARM_OK) && (pFrameCtrl != NULL)) {
        (void) ArmMutex_Take(&pFrameCtrl->Mutex);

        if (pFrameCtrl->AvblNum >= pFrameCtrl->MaxAvblNum) {
            ArmLog_DBG(ARM_LOG_CVALGO_WRAPPER, "## can't free index due to incorrect AvblNum %u", pFrameCtrl->AvblNum, 0U);
            Rval = ARM_NG;
        } else {
            pFrameCtrl->AvblNum++;
        }

        (void) ArmMutex_Give(&pFrameCtrl->Mutex);
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CreateResource
 *
 *  @Description:: Create resource and buffers
 *                 (allocate buffers -> mutex )
 *
 *  @Input      ::
 *     pCtrl:      The CvAlgo control
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 CreateResource(CT_CVALGO_WRAPPER_CTRL_s *pCtrl)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check */
    if (pCtrl == NULL) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## pCtrl is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Allocate buffer for in/out buffers */
    if ((Rval == ARM_OK) && (pCtrl != NULL)) {
        UINT32 InBufSize = MAX(sizeof(memio_source_recv_picinfo_t), sizeof(memio_source_recv_raw_t));

        pCtrl->FrameCtrl.Wp         = 0U;
        pCtrl->FrameCtrl.AvblNum    = MAX_BUF_DEPTH;
        pCtrl->FrameCtrl.MaxAvblNum = MAX_BUF_DEPTH;

        // in buf - the size is memio_source_recv_picinfo_t or memio_source_recv_raw_t
        for (UINT32 i = 0U; i < MAX_BUF_DEPTH; i++) {
            for (UINT32 j = 0U; j < (UINT32) FLEXIDAG_MAX_OUTPUTS; j++) {
                if (Rval == ARM_OK) {
                    Rval = ArmMemPool_Allocate(pCtrl->MemPoolId, InBufSize,
                                               &(pCtrl->FrameCtrl.FrameInfo[i].In.buf[j]));
                }
            }
        }

        // out buf - the size is the output data size
        for (UINT32 i = 0U; i < MAX_BUF_DEPTH; i++) {
            pCtrl->FrameCtrl.FrameInfo[i].Out.num_of_buf = pCtrl->OutputNum;
            for (UINT32 j = 0U; j < pCtrl->OutputNum; j++) {
                if (Rval == ARM_OK) {
                    Rval = ArmMemPool_Allocate(pCtrl->MemPoolId, pCtrl->OutputSz[j],
                                               &(pCtrl->FrameCtrl.FrameInfo[i].Out.buf[j]));
                }
            }
        }
    }

    /* 3. Create mutex */
    if ((Rval == ARM_OK) && (pCtrl != NULL)) {
        static UINT32 MutexIndex = 0U;

        ArmStdC_strcpy(&pCtrl->FrameCtrl.MutexName[0], MAX_PATH_LEN, "CvAlgoWrapBufMut_X");
        pCtrl->FrameCtrl.MutexName[17] = '0' + (MutexIndex % 10U);
        pCtrl->FrameCtrl.MutexName[18] = '\0';
        MutexIndex ++;

        Rval = ArmMutex_Create(&(pCtrl->FrameCtrl.Mutex), &(pCtrl->FrameCtrl.MutexName[0]));
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: DeleteResource
 *
 *  @Description:: Delete resource and buffer
 *                 (mutex -> free buffer)
 *
 *  @Input      ::
 *     pCtrl:      The CvAlgo control
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 DeleteResource(CT_CVALGO_WRAPPER_CTRL_s *pCtrl)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check */
    if (pCtrl == NULL) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## pCtrl is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Clear all callback */
    if ((Rval == ARM_OK) && (pCtrl != NULL)) {
        for (UINT32 Idx = 0U; Idx < MAX_CALLBACK_NUM; Idx++) {
            pCtrl->OutCallback[Idx] = NULL;
        }
    }

    /* 3. Delete mutex */
    if ((Rval == ARM_OK) && (pCtrl != NULL)) {
        Rval = ArmMutex_Delete(&(pCtrl->FrameCtrl.Mutex));
    }

    /* 4. Free buffers */
    if ((Rval == ARM_OK) && (pCtrl != NULL)) {   // flexibin buf
        for (UINT32 i = 0U; i < pCtrl->NumFD; i++) {
            if (Rval == ARM_OK) {
                Rval = ArmMemPool_Free(pCtrl->MemPoolId, &(pCtrl->BinBuf[i]));
            }
        }
    }

    if ((Rval == ARM_OK) && (pCtrl != NULL)) {   // CvAlgoBuf
        Rval = ArmMemPool_Free(pCtrl->MemPoolId, &(pCtrl->CvAlgoBuf));
    }

    if ((Rval == ARM_OK) && (pCtrl != NULL)) {

        // in buf
        for (UINT32 i = 0U; i < pCtrl->FrameCtrl.MaxAvblNum; i++) {    // depth
            for (UINT32 j = 0U; j < (UINT32) FLEXIDAG_MAX_OUTPUTS; j++) {
                if (Rval == ARM_OK) {
                    Rval = ArmMemPool_Free(pCtrl->MemPoolId, &(pCtrl->FrameCtrl.FrameInfo[i].In.buf[j]));
                }
            }
        }

        // out buf
        for (UINT32 i = 0U; i < pCtrl->FrameCtrl.MaxAvblNum; i++) {    // depth
            for (UINT32 j = 0U; j < pCtrl->OutputNum; j++) {
                if (Rval == ARM_OK) {
                    Rval = ArmMemPool_Free(pCtrl->MemPoolId, &(pCtrl->FrameCtrl.FrameInfo[i].Out.buf[j]));
                }
            }
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LoadFlexibin
 *
 *  @Description:: Load flexibin to dram
 *
 *  @Input      ::
 *    pBinBuf:     The pointer to flexidag_memblk_t
 *    pFlexiBin:   The path of flexibin
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 LoadFlexibin(UINT32 MemPoolId, flexidag_memblk_t *pBinBuf, const char *pFlexiBin)
{
    UINT32 Rval;
    UINT32 BinSize = 0U;

    ArmLog_STR(ARM_LOG_CVALGO_WRAPPER, "LoadFlexibin: %s", pFlexiBin, NULL);

    /* 1. Allocate memory for flexibin */
    (void) AmbaCV_UtilityFileSize(pFlexiBin, &BinSize);
    Rval = ArmMemPool_Allocate(MemPoolId, BinSize, pBinBuf);

    /* 2. Load flexibin */
    if (Rval == ARM_OK) {
        (void) AmbaCV_UtilityFileLoad(pFlexiBin, pBinBuf);
    }

    return Rval;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CvAlgoWrapperCallback
 *
 *  @Description:: Callback for SvcCvAlgo
 *
 *  @Input      ::
 *    Event:       Flexidag id
 *    pEventData:  Pointer to data
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK or NG
\*-----------------------------------------------------------------------------------------------*/
static UINT32 CvAlgoWrapperCallback(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Rval = ARM_OK;
    CT_CVALGO_WRAPPER_CTRL_s *pCtrl = NULL;

    /* 1. Sanity check for parameters */
    if (pEventData == NULL) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CvAlgoWrapperCallback fail (pEventData is null)", 0U, 0U);
        Rval = ARM_NG;
    }
    if ((Rval == ARM_OK) && (pEventData != NULL)) {
        if (pEventData->pUserData == NULL) {
            ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CvAlgoWrapperCallback fail (pUserData is null)", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Bypass the callback */
    if ((Rval == ARM_OK) && (pEventData != NULL)) {
        uint32_t tmp_buf_size[FLEXIDAG_MAX_OUTPUTS] = {0};
        //pCtrl = (CT_CVALGO_WRAPPER_CTRL_s *)pEventData->pUserData;
        (void) ArmStdC_memcpy(&pCtrl, &pEventData->pUserData, sizeof(void *));

        pCtrl->CallbackResult.pOutput    = pEventData->pOutput;
        pCtrl->CallbackResult.pExtOutput = pEventData->pExtOutput;
        pCtrl->CallbackResult.pUserData  = NULL;

        // Find the pUserData
        for (UINT32 i = 0U; i < MAX_BUF_DEPTH; i++) {
            if (pEventData->pOutput == &(pCtrl->FrameCtrl.FrameInfo[i].Out)) {
                pCtrl->CallbackResult.pUserData = pCtrl->FrameCtrl.FrameInfo[i].pUserData;
                break;
            }
        }

        // Do cache invalid to make sure we can get data in dram
        if ((Event == CALLBACK_EVENT_OUTPUT) && (pCtrl != NULL) && (pCtrl->CallbackResult.pOutput != NULL)) {
            for (UINT32 i = 0U; i < pCtrl->CallbackResult.pOutput->num_of_buf; i++) {
                if (pCtrl->CallbackResult.pOutput->buf[i].buffer_cacheable == 1U) {
                    (void) ArmMemPool_Invalid(&pCtrl->CallbackResult.pOutput->buf[i]);
                }
            }
        }

        // The flexidag memory block size is 128/4k alignment by ArmMemPool
        // We fill the real output size depends on dags.
        if ((Event == CALLBACK_EVENT_OUTPUT) && (pCtrl != NULL) && (pCtrl->CallbackResult.pOutput != NULL)) {
            for (UINT32 i = 0U; i < pCtrl->CallbackResult.pOutput->num_of_buf; i++) {
                tmp_buf_size[i] = pCtrl->CallbackResult.pOutput->buf[i].buffer_size;
                pCtrl->CallbackResult.pOutput->buf[i].buffer_size = pCtrl->OutputSz[i];
            }
        }

        // Pass callback result to top level
        if (pCtrl != NULL) {
            for (UINT32 Idx = 0U; Idx < MAX_CALLBACK_NUM; Idx++) {
                const SVC_CV_ALGO_CALLBACK_f *pCallback = &(pCtrl->OutCallback[Idx]);
                if ((*pCallback) == NULL) {
                    continue;
                } else {
                    (void) (*pCallback)(Event, &(pCtrl->CallbackResult));
                }
            }
        }

        // Rollback the buffer size due to 128/4k alignment restriction
        if ((Event == CALLBACK_EVENT_OUTPUT) && (pCtrl != NULL) && (pCtrl->CallbackResult.pOutput != NULL)) {
            for (UINT32 i = 0U; i < pCtrl->CallbackResult.pOutput->num_of_buf; i++) {
                pCtrl->CallbackResult.pOutput->buf[i].buffer_size = tmp_buf_size[i];
            }
        }
    }

    /* 3. Free frame index */
    if ((Rval == ARM_OK) && (pCtrl != NULL)) {
        if (Event == CALLBACK_EVENT_OUTPUT) {
            Rval = FreeFrameIdx(&(pCtrl->FrameCtrl));
        }
    }

    return Rval;
}


/*---------------------------------------------------------------------------*\
 * CtCvAlgoWrapper APIs
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CtCvAlgoWrapper_Create
 *
 *  @Description:: Create cvalgo and resource
 *
 *  @Input      ::
 *     Slot:       The slot id
 *     pCfg:       The create config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CtCvAlgoWrapper_Create(UINT32 Slot, const CT_CVALGO_WRAPPER_CREATE_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;
    CT_CVALGO_WRAPPER_CTRL_s *pCtrl = NULL;
    SVC_CV_ALGO_QUERY_CFG_s   QueryCfg;
    SVC_CV_ALGO_CREATE_CFG_s  CreateCfg;
    SVC_CV_ALGO_REGCB_CFG_s   RegCbCfg;

    /* 1. Sanity check for parameters */
    if (pCfg == NULL) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CtCvAlgoWrapper_Create fail (pCfg is null)", 0U, 0U);
        Rval = ARM_NG;
    } else if (Slot >= MAX_SLOT_NUM) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CtCvAlgoWrapper_Create fail (invalid Slot %u)", Slot, 0U);
        Rval = ARM_NG;
    } else {
        if (pCfg->pCvAlgoObj == NULL) {
            ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CtCvAlgoWrapper_Create fail (pCvAlgoObj is null)", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Initialize CVALGO_CTRL_s */
    if ((Rval == ARM_OK) && (pCfg != NULL) && (Slot < MAX_SLOT_NUM)) {
        pCtrl = &g_CtCvAlgoWrapperCtrl[Slot];

        pCtrl->MemPoolId  = pCfg->MemPoolId;
        pCtrl->pCvAlgoObj = pCfg->pCvAlgoObj;
        pCtrl->NumFD      = pCfg->NumFD;

        (void) ArmStdC_memset(&(pCtrl->Hdlr), 0, sizeof(SVC_CV_ALGO_HANDLE_s));
    }

    /* 3. Call SvcCvAlgo_Query and allocate buf for cvalgo */
    if ((Rval == ARM_OK) && (pCtrl != NULL) && (Slot < MAX_SLOT_NUM)) {
        QueryCfg.pAlgoObj     = pCtrl->pCvAlgoObj;
        QueryCfg.pExtQueryCfg = NULL;

        Rval = SvcCvAlgo_Query(&(pCtrl->Hdlr), &QueryCfg);
        if (Rval != CVALGO_OK) {
            ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## SvcCvAlgo_Query fail (Rval 0x%x) ", Rval, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ArmMemPool_Allocate(pCtrl->MemPoolId, QueryCfg.TotalReqBufSz,&(pCtrl->CvAlgoBuf));
        }
    }

    /* 4. Allocate buf for flexibin and load it */
    if ((Rval == ARM_OK) && (pCfg != NULL) && (pCtrl != NULL) && (Slot < MAX_SLOT_NUM)) {
        CreateCfg.NumFD = pCtrl->NumFD;
        for (UINT32 i = 0; i < pCtrl->NumFD; i ++) {
            Rval = LoadFlexibin(pCtrl->MemPoolId, &(pCtrl->BinBuf[i]), &(pCfg->Flexibin[i][0]));
            if (Rval != ARM_OK) {
                ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "LoadFlexibin fail", 0U, 0U);
                break;
            }
            CreateCfg.pBin[i] = &(pCtrl->BinBuf[i]);
        }

    }

    /* 5. Call SvcCvAlgo_Create and get the output size */
    if ((Rval == ARM_OK) && (pCtrl != NULL) && (Slot < MAX_SLOT_NUM)) {
        CreateCfg.pAlgoBuf    = &(pCtrl->CvAlgoBuf);
        CreateCfg.pExtCreateCfg = NULL;

        Rval = SvcCvAlgo_Create(&(pCtrl->Hdlr), &CreateCfg);
        if (Rval != CVALGO_OK) {
            ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## SvcCvAlgo_Create fail (Rval 0x%x) ", Rval, 0U);
            Rval = ARM_NG;
        } else {
            // Keep the output information
            pCtrl->OutputNum = CreateCfg.OutputNum;

            for (UINT32 i = 0; i < pCtrl->OutputNum; i ++) {
                pCtrl->OutputSz[i] = CreateCfg.OutputSz[i];
            }
            Rval = ARM_OK;
        }
    }

    /* 6. Create other resource (allocate in/out buf, mutex to control in/out ring buf ....) */
    if ((Rval == ARM_OK) && (pCtrl != NULL) && (Slot < MAX_SLOT_NUM)) {
        Rval = CreateResource(pCtrl);
    }

    /* 7. Register callback */
    if ((Rval == ARM_OK) && (pCtrl != NULL) && (Slot < MAX_SLOT_NUM)) {
        RegCbCfg.Mode         = 0;  // register
        RegCbCfg.Callback     = CvAlgoWrapperCallback;
        RegCbCfg.pExtRegcbCfg = NULL;

        Rval = SvcCvAlgo_RegCallback(&(pCtrl->Hdlr), &RegCbCfg);
        if (Rval != CVALGO_OK) {
            ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## SvcCvAlgo_RegCallback fail (Rval 0x%x) ", Rval, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    /* 8. Print buffer info */
    if ((Rval == ARM_OK) && (pCtrl != NULL) && (Slot < MAX_SLOT_NUM)) {
        UINT32 TotalSize = 0;
        ArmLog_DBG(ARM_LOG_CVALGO_WRAPPER, "------ Buffer Information for Slot %u ------", Slot, 0U);

        ArmLog_DBG(ARM_LOG_CVALGO_WRAPPER, "CvAlgoBuf:  Addr = 0x%x, Size = %d", pCtrl->CvAlgoBuf.buffer_daddr, pCtrl->CvAlgoBuf.buffer_size);
        TotalSize += pCtrl->CvAlgoBuf.buffer_size;

        for (UINT32 i = 0U; i < pCtrl->NumFD ; i ++) {
            ArmLog_DBG(ARM_LOG_CVALGO_WRAPPER, "Bin:        Addr = 0x%x, Size = %d", pCtrl->BinBuf[i].buffer_daddr, pCtrl->BinBuf[i].buffer_size);
            TotalSize += pCtrl->BinBuf[i].buffer_size;
        }

        for (UINT32 i = 0U; i < MAX_BUF_DEPTH; i++) {
            ArmLog_DBG(ARM_LOG_CVALGO_WRAPPER, "--------- Input Buffer[%d] ------------", i, 0U);
            for (UINT32 j = 0U; j < (UINT32) FLEXIDAG_MAX_OUTPUTS; j++) {
                ArmLog_DBG(ARM_LOG_CVALGO_WRAPPER, "Input:  Addr = 0x%x, Size = %d", pCtrl->FrameCtrl.FrameInfo[i].In.buf[j].buffer_daddr, pCtrl->FrameCtrl.FrameInfo[i].In.buf[j].buffer_size);
                TotalSize += pCtrl->FrameCtrl.FrameInfo[i].In.buf[j].buffer_size;
            }
        }

        for (UINT32 i = 0U; i < MAX_BUF_DEPTH; i++) {
            ArmLog_DBG(ARM_LOG_CVALGO_WRAPPER, "--------- Output Buffer[%d] ------------", i, 0U);
            for (UINT32 j = 0U; j < pCtrl->OutputNum; j++) {
                ArmLog_DBG(ARM_LOG_CVALGO_WRAPPER, "Output: Addr = 0x%x, Size = %d", pCtrl->FrameCtrl.FrameInfo[i].Out.buf[j].buffer_daddr, pCtrl->FrameCtrl.FrameInfo[i].Out.buf[j].buffer_size);
                TotalSize += pCtrl->FrameCtrl.FrameInfo[i].Out.buf[j].buffer_size;
            }
        }
        ArmLog_DBG(ARM_LOG_CVALGO_WRAPPER, "------------- Summary -------------", 0U, 0U);
        ArmLog_DBG(ARM_LOG_CVALGO_WRAPPER, "Total Size = %d bytes (%d MB)", TotalSize, (TotalSize / 1024U) / 1024U);
    }

    if ((Rval == ARM_OK) && (Slot < MAX_SLOT_NUM)) {
        ArmLog_OK(ARM_LOG_CVALGO_WRAPPER, "[Slot_%d] Create successfully", 0U, 0U);
        g_CtCvAlgoWrapperCtrl[Slot].IsCreated = 1U;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CtCvAlgoWrapper_Delete
 *
 *  @Description:: Delete cvalgo and resource
 *
 *  @Input      ::
 *     Slot:       The slot id
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CtCvAlgoWrapper_Delete(UINT32 Slot)
{
    UINT32 Rval = ARM_OK;
    CT_CVALGO_WRAPPER_CTRL_s *pCtrl = NULL;
    SVC_CV_ALGO_DELETE_CFG_s DeleteCfg;

    /* 1. Sanity check for parameters */
    if (Slot >= MAX_SLOT_NUM) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CtCvAlgoWrapper_Delete fail (invalid Slot %d)", Slot, 0U);
        Rval = ARM_NG;
    } else if (g_CtCvAlgoWrapperCtrl[Slot].IsCreated != 1U) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CtCvAlgoWrapper_Delete fail (Slot %u is not created)", Slot, 0U);
        Rval = ARM_NG;
    } else {
        Rval = ARM_OK;
    }

    /* 2. Assign pCtrl */
    if ((Rval == ARM_OK) && (Slot < MAX_SLOT_NUM)) {
        pCtrl = &g_CtCvAlgoWrapperCtrl[Slot];
    }

    /* 3. Call SvcCvAlgo_Delete */
    if ((Rval == ARM_OK) && (pCtrl != NULL)) {
        DeleteCfg.pExtDeleteCfg = NULL;

        Rval = SvcCvAlgo_Delete(&(pCtrl->Hdlr), &DeleteCfg);
        if (Rval != CVALGO_OK) {
            ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## SvcCvAlgo_Delete fail (Rval 0x%x) ", Rval, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    /* 4. Delete resource and free buffers (BinBuf, CvAlgoBuf, in/out buf) */
    if ((Rval == ARM_OK) && (pCtrl != NULL)) {
        Rval = DeleteResource(pCtrl);
    }

    if ((Rval == ARM_OK) && (Slot < MAX_SLOT_NUM)) {
        ArmLog_OK(ARM_LOG_CVALGO_WRAPPER, "[Slot_%d] Delete successfully", 0U, 0U);
        g_CtCvAlgoWrapperCtrl[Slot].IsCreated = 0U;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CtCvAlgoWrapper_FeedRaw
 *
 *  @Description:: Feed raw data to cvalgo
 *
 *  @Input      ::
 *     Slot:       The slot id
 *     pRaw:       The feeding raw data
 *     pUserData:  The attached user data
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CtCvAlgoWrapper_FeedRaw(UINT32 Slot, const memio_source_recv_multi_raw_t * pRaw, void * pUserData)
{
    UINT32 Rval     = ARM_OK;
    UINT32 FrameIdx = 0U;
    CT_CVALGO_WRAPPER_CTRL_s *pCtrl = NULL;
    SVC_CV_ALGO_FEED_CFG_s   FeedCfg;

    /* 1. Sanity check for parameters */
    if (pRaw == NULL) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CtCvAlgoWrapper_FeedRaw fail (pRaw is null)", 0U, 0U);
        Rval = ARM_NG;
    } else if (Slot >= MAX_SLOT_NUM) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CtCvAlgoWrapper_FeedRaw fail (invalid Slot %d)", Slot, 0U);
        Rval = ARM_NG;
    } else if (g_CtCvAlgoWrapperCtrl[Slot].IsCreated != 1U) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CtCvAlgoWrapper_FeedRaw fail (Slot %u is not created)", Slot, 0U);
        Rval = ARM_NG;
    } else {
        Rval = ARM_OK;
    }

    /* 2. Assign pCtrl */
    if ((Rval == ARM_OK) && (Slot < MAX_SLOT_NUM)) {
        pCtrl = &g_CtCvAlgoWrapperCtrl[Slot];
    }

    /* 3. Get the ring buf index */
    if ((Rval == ARM_OK) && (pCtrl != NULL)) {
        Rval = GetAvailableFrameIdx(&(pCtrl->FrameCtrl), &FrameIdx);
    }

    /* 4. Prepare data for AMBA_CV_FLEXIDAG_IO_s input */
    if ((Rval == ARM_OK) && (pCtrl != NULL) && (pRaw != NULL)) {
        pCtrl->FrameCtrl.FrameInfo[FrameIdx].In.num_of_buf = pRaw->num_io;
        for (UINT32 i = 0U; i < pRaw->num_io; i++) {
            (void) ArmStdC_memcpy(pCtrl->FrameCtrl.FrameInfo[FrameIdx].In.buf[i].pBuffer, &(pRaw->io[i]), sizeof(memio_source_recv_raw_t));

            if (pCtrl->FrameCtrl.FrameInfo[FrameIdx].In.buf[i].buffer_cacheable == 1U) {
                (void) ArmMemPool_CacheClean(&(pCtrl->FrameCtrl.FrameInfo[FrameIdx].In.buf[i]));
            }
        }
    }

    /* 5. Call SvcCvAlgo_Feed */
    if ((Rval == ARM_OK) && (pCtrl != NULL)) {
        pCtrl->FrameCtrl.FrameInfo[FrameIdx].pUserData = pUserData;

        FeedCfg.pIn  = &(pCtrl->FrameCtrl.FrameInfo[FrameIdx].In);
        FeedCfg.pOut = &(pCtrl->FrameCtrl.FrameInfo[FrameIdx].Out);

        // FeedCfg.pUserData = (void *)pCtrl;
        (void) ArmStdC_memcpy(&FeedCfg.pUserData, &pCtrl, sizeof(void *));  // we do a conversion for user data
        FeedCfg.pExtFeedCfg = NULL;

        Rval = SvcCvAlgo_Feed(&(pCtrl->Hdlr), &FeedCfg);
        if (Rval != CVALGO_OK) {
            ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## SvcCvAlgo_Feed fail (Rval 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CtCvAlgoWrapper_FeedPicinfo
 *
 *  @Description:: Feed picinfo to cvalgo
 *
 *  @Input      ::
 *     Slot:       The slot id
 *     pPic:       The feeding picinfo
 *     pUserData:  The attached user data
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CtCvAlgoWrapper_FeedPicinfo(UINT32 Slot, const memio_source_recv_picinfo_t * pPic, void * pUserData)
{
    UINT32 Rval     = ARM_OK;
    UINT32 FrameIdx = 0U;
    CT_CVALGO_WRAPPER_CTRL_s *pCtrl = NULL;
    SVC_CV_ALGO_FEED_CFG_s   FeedCfg;

    /* 1. Sanity check for parameters */
    if (pPic == NULL) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CtCvAlgoWrapper_FeedPicinfo fail (pPic is null)", 0U, 0U);
        Rval = ARM_NG;
    } else if (Slot >= MAX_SLOT_NUM) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CtCvAlgoWrapper_FeedPicinfo fail (invalid Slot %d)", Slot, 0U);
        Rval = ARM_NG;
    } else if (g_CtCvAlgoWrapperCtrl[Slot].IsCreated != 1U) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CtCvAlgoWrapper_FeedPicinfo fail (Slot %u is not created)", Slot, 0U);
        Rval = ARM_NG;
    } else {
        Rval = ARM_OK;
    }

    /* 2. Assign pCtrl */
    if ((Rval == ARM_OK) && (Slot < MAX_SLOT_NUM)) {
        pCtrl = &g_CtCvAlgoWrapperCtrl[Slot];
    }

    /* 3. Get the ring buf index */
    if ((Rval == ARM_OK) && (pCtrl != NULL)) {
        Rval = GetAvailableFrameIdx(&(pCtrl->FrameCtrl), &FrameIdx);
    }

    /* 4. Prepare data for AMBA_CV_FLEXIDAG_IO_s input */
    if ((Rval == ARM_OK) && (pCtrl != NULL) && (pPic != NULL)) {
        UINT32 U32DataIn;
        memio_source_recv_picinfo_t *pDataIn;

        pCtrl->FrameCtrl.FrameInfo[FrameIdx].In.num_of_buf = 1;
        (void) ArmStdC_memcpy(pCtrl->FrameCtrl.FrameInfo[FrameIdx].In.buf[0].pBuffer, pPic, sizeof(memio_source_recv_picinfo_t));

        // Calculate offset for picinfo
        //pDataIn = (memio_source_recv_picinfo_t *) pCtrl->FrameCtrl.FrameInfo[FrameIdx].In.buf[0].pBuffer;
        (void) ArmStdC_memcpy(&pDataIn, &(pCtrl->FrameCtrl.FrameInfo[FrameIdx].In.buf[0].pBuffer), sizeof(void *));
        U32DataIn = pCtrl->FrameCtrl.FrameInfo[FrameIdx].In.buf[0].buffer_daddr;
        for (UINT32 i = 0U ; i < MAX_HALF_OCTAVES ; i++) {
            pDataIn->pic_info.rpLumaLeft[i] -= U32DataIn;
            pDataIn->pic_info.rpLumaRight[i] -= U32DataIn;
            pDataIn->pic_info.rpChromaLeft[i] -= U32DataIn;
            pDataIn->pic_info.rpChromaRight[i] -= U32DataIn;
        }

        if (pCtrl->FrameCtrl.FrameInfo[FrameIdx].In.buf[0].buffer_cacheable == 1U) {
            (void) ArmMemPool_CacheClean(&(pCtrl->FrameCtrl.FrameInfo[FrameIdx].In.buf[0]));
        }
    }

    /* 5. Call SvcCvAlgo_Feed */
    if ((Rval == ARM_OK) && (pCtrl != NULL)) {
        pCtrl->FrameCtrl.FrameInfo[FrameIdx].pUserData = pUserData;

        FeedCfg.pIn  = &(pCtrl->FrameCtrl.FrameInfo[FrameIdx].In);
        FeedCfg.pOut = &(pCtrl->FrameCtrl.FrameInfo[FrameIdx].Out);

        // FeedCfg.pUserData = (void *)pCtrl;
        (void) ArmStdC_memcpy(&FeedCfg.pUserData, &pCtrl, sizeof(void *));  // we do a conversion for user data
        FeedCfg.pExtFeedCfg = NULL;

        Rval = SvcCvAlgo_Feed(&(pCtrl->Hdlr), &FeedCfg);
        if (Rval != CVALGO_OK) {
            ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## SvcCvAlgo_Feed fail (Rval 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CtCvAlgoWrapper_RegCb
 *
 *  @Description:: Register/Unregister output callback
 *
 *  @Input      ::
 *     Slot:       The slot id
 *     Mode:       0: register, 1: unregister
 *     Callback:   The callback function
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CtCvAlgoWrapper_RegCb(UINT32 Slot, UINT32 Mode, SVC_CV_ALGO_CALLBACK_f Callback)
{
    UINT32 Rval = ARM_OK;
    UINT32 Idx  = 0U;
    UINT32 IsBreak = 0U;
    CT_CVALGO_WRAPPER_CTRL_s *pCtrl = NULL;

    /* 1. Sanity check for parameters */
    if (Callback == NULL) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CtCvAlgoWrapper_RegCb fail (Callback is null)", 0U, 0U);
        Rval = ARM_NG;
    } else if (Slot >= MAX_SLOT_NUM) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CtCvAlgoWrapper_RegCb fail (invalid Slot %d)", Slot, 0U);
        Rval = ARM_NG;
    } else if (g_CtCvAlgoWrapperCtrl[Slot].IsCreated != 1U) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CtCvAlgoWrapper_RegCb fail (Slot %u is not created)", Slot, 0U);
        Rval = ARM_NG;
    } else {
        Rval = ARM_OK;
    }

    /* 2. Register or Unregister */
    if ((Rval == ARM_OK) && (Slot < MAX_SLOT_NUM)) {
        pCtrl = &g_CtCvAlgoWrapperCtrl[Slot];

        if (Mode == 0U) {    // Find an empty space to register
            ArmLog_DBG(ARM_LOG_CVALGO_WRAPPER, "Register callback for Slot %u", Slot, 0U);

            for (Idx = 0U; Idx < MAX_CALLBACK_NUM; Idx++) {
                if (pCtrl->OutCallback[Idx] == Callback) {
                    ArmLog_DBG(ARM_LOG_CVALGO_WRAPPER, "Already register callback to idx %u", Idx, 0U);
                    IsBreak = 1U;
                } else if (pCtrl->OutCallback[Idx] == NULL) {
                    ArmLog_DBG(ARM_LOG_CVALGO_WRAPPER, "Register callback to idx %u", Idx, 0U);
                    pCtrl->OutCallback[Idx] = Callback;
                    IsBreak = 1U;
                } else {
                    IsBreak = 0U;
                }

                if (IsBreak == 1U) {
                    break;
                }
            }

            if (MAX_CALLBACK_NUM == Idx) {
                ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "No more empty space to register callback", 0U, 0U);
                Rval = ARM_NG;
            }
        } else { // Find callback and remove it

            ArmLog_DBG(ARM_LOG_CVALGO_WRAPPER, "Unregister callback for Slot %u", Slot, 0U);

            for (Idx = 0U; Idx < MAX_CALLBACK_NUM; Idx++) {
                if (pCtrl->OutCallback[Idx] == Callback) {
                    pCtrl->OutCallback[Idx] = NULL;
                    ArmLog_DBG(ARM_LOG_CVALGO_WRAPPER, "Remove callback idx %u", Idx, 0U);
                    break;
                }
            }

        }
    }

    return Rval;
}


/*---------------------------------------------------------------------------*\
 *  @RoutineName:: CtCvAlgoWrapper_Control
 *
 *  @Description:: Send control to cvalgo
 *
 *  @Input      ::
 *     Slot:       The slot id
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
UINT32 CtCvAlgoWrapper_Control(UINT32 Slot, UINT32 CtrlType, void *pCtrlParam)
{
    UINT32 Rval = ARM_OK;
    CT_CVALGO_WRAPPER_CTRL_s *pCtrl = NULL;
    SVC_CV_ALGO_CTRL_CFG_s CtrlCfg;

    /* 1. Sanity check for parameters */
    if (Slot >= MAX_SLOT_NUM) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CtCvAlgoWrapper_Control fail (invalid Slot %d)", Slot, 0U);
        Rval = ARM_NG;
    } else if (g_CtCvAlgoWrapperCtrl[Slot].IsCreated != 1U) {
        ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## CtCvAlgoWrapper_Control fail (Slot %u is not created)", Slot, 0U);
        Rval = ARM_NG;
    } else {
        Rval = ARM_OK;
    }

    /* 2. Assign pCtrl */
    if ((Rval == ARM_OK) && (Slot < MAX_SLOT_NUM)) {
        pCtrl = &g_CtCvAlgoWrapperCtrl[Slot];
    }

    /* 3. Call SvcCvAlgo_Control */
    if ((Rval == ARM_OK) && (pCtrl != NULL)) {
        CtrlCfg.CtrlType   = CtrlType;
        CtrlCfg.pCtrlParam = pCtrlParam;

        Rval = SvcCvAlgo_Control(&(pCtrl->Hdlr), &CtrlCfg);
        if (Rval != CVALGO_OK) {
            ArmLog_ERR(ARM_LOG_CVALGO_WRAPPER, "## SvcCvAlgo_Control fail (Rval 0x%x) ", Rval, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    return Rval;
}

