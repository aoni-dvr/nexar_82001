/**
 *  @file AmbaRdtNmlEvt.c
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
 *  @details svc record destination - Normal event
 */

/* ssp */
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDef.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaGDMA.h"
/* framework*/
#include "AmbaRecFrwk.h"
#include "AmbaRbxInf.h"
#include "AmbaRbxMP4.h"
#include "AmbaMux.h"
#include "AmbaRdtInf.h"
#include "AmbaSvcGdma.h"
#include "AmbaVfs.h"
#include "AmbaSvcWrap.h"
#include "AmbaRdtNmlEvt.h"
#include "AmbaCodecCom.h"

#define RDTNML_SLOT_NUM             (UINT32)(CONFIG_AMBA_REC_NMLEVT_BUF_NUM)
#define RDTNML_MAX_FNAME_LEN        (0x00000080U)
#define RDTNML_MAX_STREAM_NUM       (4U)
#define RDTNML_STACK_SIZE           (0x00008000U)

#define FLG_RDTNML_STR_RUN          (0x00000001U) /* stream is running */
#define FLG_RDTNML_STR_STORE_PREV   (0x00000002U) /* event start */
#define FLG_RDTNML_STR_STORE_NEXT   (0x00000004U) /* event stop */

#define FLG_RDTNML_DATA_VALID       (0x00000001U) /* the data is valid */
#define FLG_RDTNML_DATA_STORE       (0x00000002U) /* the data should be stored */
#define FLG_RDTNML_DATA_WRITING     (0x00000004U) /* the data is being written */

#define FLG_RDTNML_TSK_SAVE         (0x00000001U) /* notify task to save datas to the storage */


typedef struct {
    char    FileName[RDTNML_MAX_FNAME_LEN];
    ULONG   Addr;
    UINT32  DataSize;
    UINT32  Flag;
} AMBA_RDT_NML_DATA_s;

typedef struct {
    AMBA_RDT_STATIS_s  Statis;
}AMBA_RDT_NML_PRIV_s;

typedef struct {
    UINT32         Flag;
    UINT32         WriteSlotId;
    UINT32         SlotSize;
    ULONG          WritePointer;
    ULONG          BufBaseAddr;
    UINT32         BufSize;
    UINT32         NextStoreNum;
    AMBA_REC_DST_s *pNmlEvtRdt;
} AMBA_RDT_NML_STREAM_CTRL_s;

typedef struct {
    UINT32                 IsCreated;
    UINT32                 Priority;
    UINT32                 CpuBits;
    AMBA_KAL_EVENT_FLAG_t  Flag;
    AMBA_KAL_TASK_t        Task;
} AMBA_RDT_NML_TASK_CTRL_s;

static AMBA_RDT_NML_DATA_s         NmlData[RDTNML_MAX_STREAM_NUM][RDTNML_SLOT_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_RDT_NML_STREAM_CTRL_s  StrCtrl[RDTNML_MAX_STREAM_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_RDT_NML_TASK_CTRL_s    TaskCtrl GNU_SECTION_NOZEROINIT;

static void   MarkPrevSlot(UINT16 StreamId, UINT32 FileNum);
static void*  NmlEvtTaskEntry(void* EntryArg);
static UINT32 DataStore(const AMBA_REC_DST_s *pRdtCtrl, const AMBA_REC_FRWK_DESC_s *pDesc);

static inline void RDTNE_NG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
}

static inline void RDTNE_DBG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
}

static inline void SvcUtil_BitsToArr(UINT32 MaxNum, UINT32 Bits, UINT32 *pNum, UINT16 *pArr)
{
    UINT32  i, Num = 0;

    for (i = 0; i < MaxNum; i++) {
        if (0U < (Bits & ((UINT32)0x01U << i))) {
            pArr[Num] = (UINT16)i;

            Num += 1U;
        }
    }
    *pNum = Num;
}

/**
* eval buffer size for normal event buffer
* @param [in]  StreamId record stream index
* @param [in]  SubType record box type
* @param [in]  pEval stream info
* @param [out]  pBufSize pointer of evaluated memory size
* @param [out]  pSlotSize pointer of evaluated memory size
*/
void AmbaRdtNmlEvt_EvalMemSize(UINT32 StreamId, UINT32 SubType, const AMBA_REC_EVAL_s *pEval, UINT32 *pBufSize, UINT32 *pSlotSize)
{
    UINT32 Err;

    if (StreamId < RDTNML_MAX_STREAM_NUM) {
        Err = AmbaMux_EvalFileSize(SubType, pEval, pSlotSize);
        if (Err != RECODER_OK) {
            RDTNE_DBG("AmbaMux_EvalFileSize failed %u", Err, 0U);
        }
        *pBufSize  = (*pSlotSize) * RDTNML_SLOT_NUM;
        RDTNE_DBG("Memory eval(%u): %u byte", StreamId, *pBufSize);
    } else {
        RDTNE_NG("Invalid StreamId(%u)", StreamId, 0U);
    }
}

/**
* create normal event record task
* @param [in]  pNmlEvtRdt record destination control
* @return ErrorCode
*/
UINT32 AmbaRdtNmlEvt_Create(AMBA_REC_DST_s *pNmlEvtRdt)
{
    static UINT8  TaskStack[RDTNML_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char   FlagName[] = "AmbaRdtNmlEvtFlag";
    UINT32        Err, Rval = RECODER_OK, i;
    UINT32        StreamId = pNmlEvtRdt->UsrCfg.StreamID;
    static UINT32 IsInit = 0U;

    if (IsInit == 0U) {
        Err = AmbaWrap_memset(&TaskCtrl, 0, sizeof(AMBA_RDT_NML_TASK_CTRL_s));
        if (RECODER_OK != Err) {
            RDTNE_NG("AmbaWrap_memset failed(%u)", Err, 0U);
        }

        Err = AmbaWrap_memset(StrCtrl, 0, sizeof(StrCtrl));
        if (RECODER_OK != Err) {
            RDTNE_NG("AmbaWrap_memset failed(%u)", Err, 0U);
        }

        Err = AmbaWrap_memset(NmlData, 0, sizeof(NmlData));
        if (RECODER_OK != Err) {
            RDTNE_NG("AmbaWrap_memset failed(%u)", Err, 0U);
        }
        IsInit = 1U;
    }

    if ((pNmlEvtRdt->UsrCfg.DstType == AMBA_RDT_TYPE_EVT) && (pNmlEvtRdt->UsrCfg.SubType == AMBA_RDT_STYPE_NML)) {
        StrCtrl[StreamId].BufBaseAddr = pNmlEvtRdt->UsrCfg.MemBase;
        StrCtrl[StreamId].BufSize     = pNmlEvtRdt->UsrCfg.MemSize;
        StrCtrl[StreamId].SlotSize    = pNmlEvtRdt->UsrCfg.SlotSize;
        StrCtrl[StreamId].Flag        = 0U;
        StrCtrl[StreamId].pNmlEvtRdt  = pNmlEvtRdt;

        /* buffer addr */
        if (StrCtrl[StreamId].BufBaseAddr == 0U) {
            RDTNE_NG("There's no buffer for event record(%u)", StreamId, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        } else {
            for (i = 0U; i < RDTNML_SLOT_NUM; i++) {
                NmlData[StreamId][i].Addr = StrCtrl[StreamId].BufBaseAddr + ((ULONG)i * (ULONG)StrCtrl[StreamId].SlotSize);
            }
        }

        /* init parameters */
        if (Rval == RECODER_OK) {
            pNmlEvtRdt->pfnStore = DataStore;
        }

        /* only create a task for all streams */
        if (Rval == RECODER_OK) {
            if (TaskCtrl.IsCreated == 0U) {

                TaskCtrl.Priority   = pNmlEvtRdt->UsrCfg.TaskPriority;
                TaskCtrl.CpuBits    = pNmlEvtRdt->UsrCfg.TaskCpuBits;

                /* create flag */
                Err = AmbaKAL_EventFlagCreate(&(TaskCtrl.Flag), FlagName);
                if (Err != KAL_ERR_NONE) {
                    RDTNE_NG("create flag failed!(%u)", Err, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
                Err = AmbaKAL_EventFlagClear(&(TaskCtrl.Flag), 0xFFFFFFFFU);
                if (Err != KAL_ERR_NONE) {
                    RDTNE_NG("clear flag failed!(%u)", Err, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }

                /* create task */
                if (Rval == RECODER_OK) {
                    static char Name[] = "RdtNml_Task";
                    Err = AmbaKAL_TaskCreate(&(TaskCtrl.Task),
                                            Name,
                                            TaskCtrl.Priority,
                                            NmlEvtTaskEntry,
                                            NULL,
                                            TaskStack,
                                            RDTNML_STACK_SIZE,
                                            0);
                    if (Err != KAL_ERR_NONE) {
                        RDTNE_NG("AmbaKAL_TaskCreate failed %u ", Err, 0U);
                        Rval = RECODER_ERROR_GENERAL_ERROR;
                    } else {
                        TaskCtrl.IsCreated = 1U;
                    }
                }

                if (Rval == RECODER_OK) {
                    Err = AmbaKAL_TaskSetSmpAffinity(&(TaskCtrl.Task), TaskCtrl.CpuBits);
                    if (Err != KAL_ERR_NONE) {
                        RDTNE_NG("AmbaKAL_TaskSetSmpAffinity failed %u ", Err, 0U);
                        Rval = RECODER_ERROR_GENERAL_ERROR;
                    }
                }

                if (Rval == RECODER_OK) {
                    Err = AmbaKAL_TaskResume(&(TaskCtrl.Task));
                    if (Err != KAL_ERR_NONE) {
                        RDTNE_NG("AmbaKAL_TaskResume failed %u ", Err, 0U);
                        Rval = RECODER_ERROR_GENERAL_ERROR;
                    }
                }
            }
        }
    } else {
        RDTNE_NG("destination type error(%u)", pNmlEvtRdt->UsrCfg.DstType, 0U);
        Rval = RECODER_ERROR_GENERAL_ERROR;
    }

    return Rval;
}

/**
* destroy normal event record task
* @param [in]  pNmlEvtRdt record destination control
* @return ErrorCode
*/
UINT32 AmbaRdtNmlEvt_Destroy(const AMBA_REC_DST_s *pNmlEvtRdt)
{
    UINT32 Err, Rval = RECODER_OK;

    AmbaMisra_TouchUnused(&pNmlEvtRdt);

    if ((pNmlEvtRdt->UsrCfg.DstType == AMBA_RDT_TYPE_EVT) && (pNmlEvtRdt->UsrCfg.SubType == AMBA_RDT_STYPE_NML)) {
        if (TaskCtrl.IsCreated == 1U) {
            {
                Err = AmbaKAL_TaskTerminate(&(TaskCtrl.Task));
                if (Err != KAL_ERR_NONE) {
                    RDTNE_NG("AmbaKAL_TaskTerminate failed %u ", Err, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == RECODER_OK) {
                Err = AmbaKAL_TaskDelete(&(TaskCtrl.Task));
                if (Err != KAL_ERR_NONE) {
                    RDTNE_NG("AmbaKAL_TaskDelete failed %u ", Err, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                } else {
                    TaskCtrl.IsCreated = 0U;
                }
            }

            if (Rval == RECODER_OK) {
                Err = AmbaKAL_EventFlagDelete(&(TaskCtrl.Flag));
                if (Err != KAL_ERR_NONE) {
                    RDTNE_NG("delete flag failed!(%u)", Err, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            }
        }
    } else {
        RDTNE_NG("destination type error(%u)", pNmlEvtRdt->UsrCfg.DstType, 0U);
        Rval = RECODER_ERROR_GENERAL_ERROR;
    }

    return Rval;
}

/**
* normal event record control
* @param [in]  pNmlEvtRdt record destination control
* @param [in]  CtrlType control type
* @param [in]  pParam control parameter
*/
void AmbaRdtNmlEvt_Control(const AMBA_REC_DST_s *pNmlEvtRdt, UINT32 CtrlType, void *pParam)
{
    const UINT8               *pPrivData = pNmlEvtRdt->PrivData;
    UINT32                    Rval, StreamId = pNmlEvtRdt->UsrCfg.StreamID, i;
    const AMBA_RDT_NML_PRIV_s  *pPriv;
    const AMBA_RDT_USR_CFG_s   *pUsrCfg;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    if ((pNmlEvtRdt->UsrCfg.DstType == AMBA_RDT_TYPE_EVT) && (pNmlEvtRdt->UsrCfg.SubType == AMBA_RDT_STYPE_NML)) {
        switch (CtrlType) {
        case AMBA_RDTC_NMLEVT_STATIS_GET:
            Rval = AmbaWrap_memcpy(pParam, &(pPriv->Statis), sizeof(AMBA_RDT_STATIS_s));
            if (Rval != RECODER_OK) {
                RDTNE_NG("fail to AmbaWrap_memcpy %u", Rval, 0U);
            }
            break;
        case AMBA_RDTC_NMLEVT_OPEN:
            if (0U == CheckBits(StrCtrl[StreamId].Flag, FLG_RDTNML_STR_RUN)) {
                if (StrCtrl[StreamId].BufSize > 0U) {
                    pUsrCfg = &(pNmlEvtRdt->UsrCfg);
                    /* reset parameters */
                    StrCtrl[StreamId].WriteSlotId                         = 0U;
                    StrCtrl[StreamId].WritePointer                        = NmlData[StreamId][0U].Addr;
                    for (i = 0U; i < RDTNML_SLOT_NUM; i++) {
                        NmlData[StreamId][i].Flag = 0U;
                    }
                    NmlData[StreamId][StrCtrl[StreamId].WriteSlotId].Flag = SetBits(NmlData[StreamId][StrCtrl[StreamId].WriteSlotId].Flag, FLG_RDTNML_DATA_VALID);
                    NmlData[StreamId][StrCtrl[StreamId].WriteSlotId].Flag = SetBits(NmlData[StreamId][StrCtrl[StreamId].WriteSlotId].Flag, FLG_RDTNML_DATA_WRITING);

                    /* fetch file name */
                    if (pUsrCfg->pfnFetchFName != NULL) {
                        Rval = pUsrCfg->pfnFetchFName(NmlData[StreamId][StrCtrl[StreamId].WriteSlotId].FileName, RDTNML_MAX_FNAME_LEN, StreamId);
                        if (Rval != RECODER_OK) {
                            RDTNE_NG("fail to fetch filename", 0U, 0U);
                        }
                    } else {
                        RDTNE_NG("pfnFetchFName is null", 0U, 0U);
                    }

                    StrCtrl[StreamId].Flag = SetBits(StrCtrl[StreamId].Flag, FLG_RDTNML_STR_RUN);
                } else {
                    RDTNE_NG("stream(%u) has no normal event buffer", StreamId, 0U);
                }
            } else {
                RDTNE_NG("stream(%u) is running", StreamId, 0U);
            }
            break;
        case AMBA_RDTC_NMLEVT_CLOSE:
            if (0U != CheckBits(StrCtrl[StreamId].Flag, FLG_RDTNML_STR_RUN)) {
                pUsrCfg = &(pNmlEvtRdt->UsrCfg);
                /* update file name */
                if (pUsrCfg->pfnUpdateFName != NULL) {
                    Rval = pUsrCfg->pfnUpdateFName(NmlData[StreamId][StrCtrl[StreamId].WriteSlotId].FileName,
                                                   pUsrCfg->StreamID);
                    if (Rval != RECODER_OK) {
                        RDTNE_NG("fail to update filename", 0U, 0U);
                    }
                } else {
                    RDTNE_NG("pfnUpdateFName is null", 0U, 0U);
                }

                NmlData[StreamId][StrCtrl[StreamId].WriteSlotId].Flag = ClearBits(NmlData[StreamId][StrCtrl[StreamId].WriteSlotId].Flag, FLG_RDTNML_DATA_WRITING);

                /* save file to storage if event is started */
                if ((0U != CheckBits(StrCtrl[StreamId].Flag, FLG_RDTNML_STR_STORE_PREV)) ||
                    (0U != CheckBits(StrCtrl[StreamId].Flag, FLG_RDTNML_STR_STORE_NEXT))) {
                    Rval = AmbaKAL_EventFlagSet(&(TaskCtrl.Flag), FLG_RDTNML_TSK_SAVE);
                    if (Rval != KAL_ERR_NONE) {
                        RDTNE_NG("give flag failed(%u)", Rval, 0U);
                    }
                }

                /* clean stream flag */
                StrCtrl[StreamId].Flag = 0U;
            } else {
                RDTNE_NG("stream(%u) is idle", StreamId, 0U);
            }
            break;
        default:
            RDTNE_NG("unknown control type", 0U, 0U);
            break;
        }
    } else {
        RDTNE_NG("destination type error(%u)", pNmlEvtRdt->UsrCfg.DstType, 0U);
    }
}

/**
* normal event start
* @param [in]  StreamBits record stream bits
* @param [in]  FileNum the number of files to save before event start
*/
void AmbaRdtNmlEvt_EvtStart(UINT32 StreamBits, UINT32 FileNum)
{
    UINT32 StreamNum, i, Err;
    UINT16 StreamId, StrmIdxArr[RDTNML_MAX_STREAM_NUM];

    SvcUtil_BitsToArr(RDTNML_MAX_STREAM_NUM, StreamBits, &StreamNum, StrmIdxArr);

    if (FileNum > 0U) {
        for (i = 0U; i < StreamNum; i++) {
            StreamId = StrmIdxArr[i];
            if (StrCtrl[StreamId].pNmlEvtRdt->UsrCfg.pfnCheckStorage(StreamId) == RECODER_OK) {
                if (0U != CheckBits(StrCtrl[StreamId].Flag, FLG_RDTNML_STR_RUN)) {
                    if (0U == CheckBits(StrCtrl[StreamId].Flag, FLG_RDTNML_STR_STORE_PREV | FLG_RDTNML_STR_STORE_NEXT)) {
                        MarkPrevSlot(StreamId, FileNum);
                        StrCtrl[StreamId].Flag = SetBits(StrCtrl[StreamId].Flag, FLG_RDTNML_STR_STORE_PREV);
                        Err = AmbaKAL_EventFlagSet(&(TaskCtrl.Flag), FLG_RDTNML_TSK_SAVE);
                        if (Err != KAL_ERR_NONE) {
                            RDTNE_NG("give flag failed(%u)", Err, 0U);
                        } else {
                            RDTNE_DBG("Normal event start (%u)", StreamId, 0U);
                        }
                    } else {
                        RDTNE_NG("event record(%u) is already be triggered", StreamId, 0U);
                    }
                } else {
                    RDTNE_NG("stream(%u) is idle", StreamId, 0U);
                }
            }
        }
    } else {
        RDTNE_NG("FileNum should not be zero!", 0U, 0U);
    }
}

/**
* normal event stop
* @param [in]  StreamBits record stream bits
* @param [in]  FileNum the number of files to save after event stop
*/
void AmbaRdtNmlEvt_EvtStop(UINT32 StreamBits, UINT32 FileNum)
{
    UINT32 StreamNum, i;
    UINT16 StreamId, StrmIdxArr[RDTNML_MAX_STREAM_NUM];

    SvcUtil_BitsToArr(RDTNML_MAX_STREAM_NUM, StreamBits, &StreamNum, StrmIdxArr);

    if (FileNum > 0U) {
        for (i = 0U; i < StreamNum; i++) {
            StreamId = StrmIdxArr[i];
            if (0U != CheckBits(StrCtrl[StreamId].Flag, FLG_RDTNML_STR_RUN)) {
                if (0U != CheckBits(StrCtrl[StreamId].Flag, FLG_RDTNML_STR_STORE_PREV)) {
                    StrCtrl[StreamId].NextStoreNum = FileNum;
                    StrCtrl[StreamId].Flag = ClearBits(StrCtrl[StreamId].Flag, FLG_RDTNML_STR_STORE_PREV);
                    StrCtrl[StreamId].Flag = SetBits(StrCtrl[StreamId].Flag, FLG_RDTNML_STR_STORE_NEXT);
                } else {
                    RDTNE_NG("stream(%u) event is not started", StreamId, 0U);
                }
            } else {
                RDTNE_NG("stream(%u) is idle", StreamId, 0U);
            }
        }
    } else {
        RDTNE_NG("FileNum should not be zero!", 0U, 0U);
    }
}

static void MarkPrevSlot(UINT16 StreamId, UINT32 FileNum)
{
    UINT32 MarkNum = FileNum, SlotNumber, i;

    if (FileNum > RDTNML_SLOT_NUM) {
        MarkNum = RDTNML_SLOT_NUM;
        RDTNE_DBG("FileNum(%u) exceeds slot number(%u)", FileNum, RDTNML_SLOT_NUM);
    }

    /* calculate start mark slot number */
    if ((StrCtrl[StreamId].WriteSlotId + 1U) < MarkNum) {
        SlotNumber = StrCtrl[StreamId].WriteSlotId + 1U + RDTNML_SLOT_NUM - MarkNum;
    } else {
        SlotNumber = StrCtrl[StreamId].WriteSlotId + 1U - MarkNum;
    }

    for (i = 0U; i < MarkNum; i++) {
        if (0U != CheckBits(NmlData[StreamId][SlotNumber].Flag, FLG_RDTNML_DATA_VALID)) {
            NmlData[StreamId][SlotNumber].Flag = SetBits(NmlData[StreamId][SlotNumber].Flag, FLG_RDTNML_DATA_STORE);
        } else {
            RDTNE_DBG("data(%u) is invalid", SlotNumber, 0U);
        }
        SlotNumber++;
        if (SlotNumber >= RDTNML_SLOT_NUM) {
            SlotNumber -= RDTNML_SLOT_NUM;
        }
    }
}

static void* NmlEvtTaskEntry(void* EntryArg)
{
    UINT32           Err, LoopU = 1U, ActualFlags, i, j, NumSuccess;
    void             *pBuf;
    AMBA_VFS_FILE_s  File;

    AmbaMisra_TouchUnused(EntryArg);
    RDTNE_DBG("normal event task created!", 0U, 0U);

    while (LoopU == 1U) {
        Err = AmbaKAL_EventFlagGet(&(TaskCtrl.Flag), FLG_RDTNML_TSK_SAVE, AMBA_KAL_FLAGS_ALL,
                                   AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, AMBA_KAL_WAIT_FOREVER);

        if (Err != KAL_ERR_NONE) {
            RDTNE_NG("AmbaKAL_EventFlagGet failed(%u)!", Err, 0U);
            LoopU = 0U;
        } else {
            for (i = 0U; i < RDTNML_MAX_STREAM_NUM; i++) {
                for (j = 0U; j < RDTNML_SLOT_NUM; j++) {
                    if ((0U != CheckBits(NmlData[i][j].Flag, FLG_RDTNML_DATA_STORE)) &&
                        (0U != CheckBits(NmlData[i][j].Flag, FLG_RDTNML_DATA_VALID)) &&
                        (0U == CheckBits(NmlData[i][j].Flag, FLG_RDTNML_DATA_WRITING))) {

                        File.pFile = NULL;
                        Err = AmbaVFS_Open(NmlData[i][j].FileName, "w", 1U, &(File));
                        if (Err != RECODER_OK) {
                            RDTNE_NG("AmbaVFS_Open failed(%u)!", Err, 0U);
                        }

                        if (Err == RECODER_OK) {
                            AmbaMisra_TypeCast(&pBuf, &(NmlData[i][j].Addr));
                            Err = AmbaVFS_Write(pBuf, NmlData[i][j].DataSize, 1U, &(File), &NumSuccess);
                            if (Err != RECODER_OK) {
                                RDTNE_NG("AmbaVFS_Write failed(%u)!", Err, 0U);
                            }
                        }

                        if (Err == RECODER_OK) {
                            Err = AmbaVFS_Sync(&(File));
                            if (Err != RECODER_OK) {
                                RDTNE_NG("AmbaVFS_Sync failed(%u)!", Err, 0U);
                            }
                        }

                        if ((Err == RECODER_OK) && (File.pFile != NULL)) {
                            Err = AmbaVFS_Close(&(File));
                            if (Err != RECODER_OK) {
                                RDTNE_NG("AmbaVFS_Close failed(%u)!", Err, 0U);
                            }
                        }

                        NmlData[i][j].Flag = ClearBits(NmlData[i][j].Flag, FLG_RDTNML_DATA_STORE);
                        NmlData[i][j].Flag = ClearBits(NmlData[i][j].Flag, FLG_RDTNML_DATA_VALID);

                        AmbaPrint_PrintStr5("File saved: %s", NmlData[i][j].FileName, NULL, NULL, NULL, NULL);
                    }
                }
            }
        }
    }

    return NULL;
}

static UINT32 DataStore(const AMBA_REC_DST_s *pRdtCtrl, const AMBA_REC_FRWK_DESC_s *pDesc)
{
    const UINT8                   *pPrivData = pRdtCtrl->PrivData;
    UINT32                        Err, Rval = RECODER_OK, WriteSlotId, i;
    AMBA_RDT_NML_PRIV_s            *pPriv;
    const AMBA_RDT_USR_CFG_s       *pUsrCfg = &(pRdtCtrl->UsrCfg);
    UINT32                        StreamId = pUsrCfg->StreamID;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    WriteSlotId = StrCtrl[StreamId].WriteSlotId;

    /* store data to ram */
    if (0U < CheckBits(pRdtCtrl->UsrCfg.InBoxBits, pDesc->BoxBit)) {
        if (0U < pDesc->BoxBufNum) {
            for (i = 0U; i < pDesc->BoxBufNum; i++) {
                UINT8     *pDst, *pSrc;

                AmbaMisra_TypeCast(&pSrc, &(pDesc->BoxBufChain[i].Base));
                AmbaMisra_TypeCast(&pDst, &(StrCtrl[StreamId].WritePointer));
                if (pDesc->BoxBufChain[i].Size > 0UL) {
#if defined(CONFIG_AMBA_REC_GDMA_USED)
                    Err = AmbaSvcGdma_Copy(pDst, pSrc, pDesc->BoxBufChain[i].Size);
                    if (Err != RECODER_OK) {
                        RDTNE_NG("fail to do AmbaSvcGdma_Copy %u", Err, 0U);
                        Rval = RECODER_ERROR_GENERAL_ERROR;
                    }
#else
                    AmbaMisra_TouchUnused(pSrc);

                    Err = AmbaWrap_memcpy(pDst, pSrc, pDesc->BoxBufChain[i].Size);
                    if (Err != RECODER_OK) {
                        RDTNE_NG("fail to do AmbaWrap_memcpy %u", Err, 0U);
                    }
#endif
                }
                StrCtrl[StreamId].WritePointer          += pDesc->BoxBufChain[i].Size;
                NmlData[StreamId][WriteSlotId].DataSize += pDesc->BoxBufChain[i].Size;

                if (StrCtrl[StreamId].WritePointer > (NmlData[StreamId][WriteSlotId].Addr + StrCtrl[StreamId].SlotSize)) {
                    RDTNE_NG("buffer overflow", 0U, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            }
        }
    }
    /* file split */
    if (0U < CheckBits(pDesc->DescFlags, AMBA_REC_DDESC_FLAG_SPLIT)) {
        /* make sure gdma copy done */
        {
            ULONG  AlignStart, AlignSize;

            AlignStart = NmlData[StreamId][WriteSlotId].Addr & AMBA_CACHE_LINE_MASK;
            AlignSize  = StrCtrl[StreamId].SlotSize & AMBA_CACHE_LINE_MASK;

            Err = AmbaSvcWrap_CacheClean(AlignStart, AlignSize);
            if (Err != RECODER_OK) {
                RDTNE_NG("fail to AmbaSvcWrap_CacheClean %u", Err, 0U);
            }
        }
#if defined(CONFIG_AMBA_REC_GDMA_USED)
        Err = AmbaGDMA_WaitAllCompletion(1000U);
#else
        Err = GDMA_ERR_NONE;
        AmbaMisra_TouchUnused(&Err);
#endif
        if (Err == GDMA_ERR_NONE) {
            /* update file name */
            if (pUsrCfg->pfnUpdateFName != NULL) {
                Rval = pUsrCfg->pfnUpdateFName(NmlData[StreamId][WriteSlotId].FileName, pUsrCfg->StreamID);
                if (Rval != RECODER_OK) {
                    RDTNE_NG("fail to update filename", 0U, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            } else {
                RDTNE_NG("pfnUpdateFName is null", 0U, 0U);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            }

            NmlData[StreamId][WriteSlotId].Flag     = ClearBits(NmlData[StreamId][WriteSlotId].Flag, FLG_RDTNML_DATA_WRITING);

            /* update slot number and buffer write pointer */
            StrCtrl[StreamId].WriteSlotId++;
            if (StrCtrl[StreamId].WriteSlotId >= RDTNML_SLOT_NUM) {
                StrCtrl[StreamId].WriteSlotId = 0U;
            }
            WriteSlotId = StrCtrl[StreamId].WriteSlotId;

            StrCtrl[StreamId].WritePointer          = NmlData[StreamId][WriteSlotId].Addr;
            NmlData[StreamId][WriteSlotId].Flag     = SetBits(NmlData[StreamId][WriteSlotId].Flag, FLG_RDTNML_DATA_VALID);
            NmlData[StreamId][WriteSlotId].Flag     = SetBits(NmlData[StreamId][WriteSlotId].Flag, FLG_RDTNML_DATA_WRITING);
            NmlData[StreamId][WriteSlotId].DataSize = 0U;

            if (0U < CheckBits(NmlData[StreamId][WriteSlotId].Flag, FLG_RDTNML_DATA_STORE)) {
                RDTNE_NG("The file is overwritten before save to storage (%u)", StreamId, 0U);
            }

            /* notify task if event starts */
            if (0U < CheckBits(StrCtrl[StreamId].Flag, FLG_RDTNML_STR_STORE_PREV)) {
                NmlData[StreamId][WriteSlotId].Flag = SetBits(NmlData[StreamId][WriteSlotId].Flag, FLG_RDTNML_DATA_STORE);
                Err = AmbaKAL_EventFlagSet(&(TaskCtrl.Flag), FLG_RDTNML_TSK_SAVE);
                if (Err != KAL_ERR_NONE) {
                    RDTNE_NG("give flag failed(%u)", Err, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            }

            if (0U < CheckBits(StrCtrl[StreamId].Flag, FLG_RDTNML_STR_STORE_NEXT)) {
                if (StrCtrl[StreamId].NextStoreNum > 0U) {
                    NmlData[StreamId][WriteSlotId].Flag = SetBits(NmlData[StreamId][WriteSlotId].Flag, FLG_RDTNML_DATA_STORE);
                    StrCtrl[StreamId].NextStoreNum--;
                    Err = AmbaKAL_EventFlagSet(&(TaskCtrl.Flag), FLG_RDTNML_TSK_SAVE);
                    if (Err != KAL_ERR_NONE) {
                        RDTNE_NG("give flag failed(%u)", Err, 0U);
                        Rval = RECODER_ERROR_GENERAL_ERROR;
                    }
                } else {
                    Err = AmbaKAL_EventFlagSet(&(TaskCtrl.Flag), FLG_RDTNML_TSK_SAVE);
                    if (Err != KAL_ERR_NONE) {
                        RDTNE_NG("give flag failed(%u)", Err, 0U);
                        Rval = RECODER_ERROR_GENERAL_ERROR;
                    }
                    StrCtrl[StreamId].Flag = ClearBits(StrCtrl[StreamId].Flag, FLG_RDTNML_STR_STORE_NEXT);
                    RDTNE_DBG("Normal event stop (%u)", StreamId, 0U);
                }
            }

            /* fetch new file name */
            if (pUsrCfg->pfnFetchFName != NULL) {
                Rval = pUsrCfg->pfnFetchFName(NmlData[StreamId][WriteSlotId].FileName, RDTNML_MAX_FNAME_LEN, StreamId);
                if (Rval != RECODER_OK) {
                    RDTNE_NG("fail to fetch filename", 0U, 0U);
                }
            } else {
                RDTNE_NG("pfnFetchFName is null", 0U, 0U);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            }
        } else {
            RDTNE_NG("fail to wait gdma done(%u)", Err, 0U);
        }
    }

    if (0U < pDesc->BoxDataSize) {
        pPriv->Statis.StoreCount += 1U;
        pPriv->Statis.StoreSize  += pDesc->BoxDataSize;
    }

    return Rval;
}
