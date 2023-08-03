/**
*  @file SvcDataCmp.c
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
*  @details svc data comparison
*
*/

#include "AmbaMisraFix.h"
#include "AmbaDef.h"
#include "AmbaUtility.h"
#include "AmbaSvcWrap.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcTask.h"
#include "SvcDataCmp.h"

#define LOG_DTCMP             "DTCMP"
#define DTCMP_MAX_CMP_NUM     (CONFIG_ICAM_DATACMP_MAX_CMP_NUM)
#define DTCMP_MAX_QUE_NUM     (CONFIG_ICAM_DATACMP_MAX_DATAQUE_NUM)
#define DTCMP_TIMEOUT         (5000U)

typedef struct {
    UINT32    CmpId;
} SVC_DATA_CMP_MSG_s;

typedef struct {
    SVC_DATA_CMP_DATA_s    Data[SVC_DATA_CMP_MAX_DATA_NUM];
} SVC_DATA_CMP_DATA_LIST_s;

typedef struct {
    UINT8                       IsCreate;
    UINT8                       CmpNum;
    UINT8                       DataNum;

    UINT64                      WCount[DTCMP_MAX_CMP_NUM];
    UINT64                      RCount[DTCMP_MAX_CMP_NUM];
    SVC_DATA_CMP_DATA_LIST_s    DataList[DTCMP_MAX_CMP_NUM][DTCMP_MAX_QUE_NUM];

    UINT64                      ErrCount;
    UINT64                      CmpCount;

    #define DTCMP_STACK_SIZE    (0x6000UL)
    SVC_TASK_CTRL_s             Task;
    UINT8                       TaskStack[DTCMP_STACK_SIZE];

    #define DTCMP_MAX_MSG_NUM   (8U)
    AMBA_KAL_MSG_QUEUE_t        Msg;
    SVC_DATA_CMP_MSG_s          MsqBuf[DTCMP_MAX_MSG_NUM];

    AMBA_KAL_MUTEX_t            Mtx;
} SVC_DATA_CMP_HDLR_PRIV_s;

static void* DataCmp_TaskEntry(void* EntryArg)
{
    UINT32                            Err, DoCmp, LoopU = 1U, MtxTake, CmpId;
    UINT64                            CmpWCount = 0U, CmpDIdx = 0U;
    UINT8                             i, j;
    UINT32                            CmpResult, Diff;
    ULONG                             Addr = 0U, Arg;
    const ULONG                       *pArg;
    SVC_DATA_CMP_HDLR_PRIV_s          *pPriv;
    SVC_DATA_CMP_MSG_s                Msg = {.CmpId = 0U};
    const SVC_DATA_CMP_DATA_LIST_s    *pListA, *pListB;
    const SVC_DATA_CMP_DATA_s         *pDataA, *pDataB;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Arg = (*pArg);
    AmbaMisra_TypeCast(&pPriv, &Arg);

    SvcLog_OK(LOG_DTCMP, "DataCmp_TaskEntry is created", 0U, 0U);

    while (LoopU == 1U) {
        DoCmp     = 1U;
        CmpResult = 0U;

        Err = AmbaKAL_MsgQueueReceive(&(pPriv->Msg), &Msg, AMBA_KAL_WAIT_FOREVER);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(LOG_DTCMP, "AmbaKAL_MsgQueueReceive failed %u", Err, 0U);
            LoopU  = 0U;
            DoCmp = 0U;
        }

        /* check if all the data is already sent */
        if (DoCmp == 1U) {
            MtxTake = 1U;
            Err = AmbaKAL_MutexTake(&(pPriv->Mtx), DTCMP_TIMEOUT);
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(LOG_DTCMP, "AmbaKAL_MutexTake failed %u", Err, 0U);
                MtxTake = 0U;
                LoopU  = 0U;
                DoCmp = 0U;
            }

            CmpId     = Msg.CmpId;
            CmpWCount = pPriv->RCount[CmpId] + 1ULL;
            CmpDIdx   = pPriv->RCount[CmpId] % (UINT64)DTCMP_MAX_QUE_NUM;

            for (i = 0U; i < pPriv->CmpNum; i++) {
                if (pPriv->WCount[i] < CmpWCount) {
                    DoCmp = 0U;
                }
            }

            if (MtxTake == 1U) {
                Err = AmbaKAL_MutexGive(&(pPriv->Mtx));
                if (Err != KAL_ERR_NONE) {
                    SvcLog_NG(LOG_DTCMP, "AmbaKAL_MutexGive failed %u", Err, 0U);
                    LoopU  = 0U;
                    DoCmp = 0U;
                }
            }
        }

        /* data comparioson */
        if (DoCmp == 1U) {
            for (i = 0U; i < (pPriv->CmpNum - 1U); i++) {
                pListA  = &(pPriv->DataList[i][CmpDIdx]);
                pListB  = &(pPriv->DataList[i + 1U][CmpDIdx]);

                for (j = 0U; j < pPriv->DataNum; j++) {
                    ULONG          AddrA, AddrB, k;
                    ULONG          Len;
                    UINT8          *pA = NULL, *pB = NULL;

                    AmbaMisra_TouchUnused(pA);
                    AmbaMisra_TouchUnused(pB);

                    pDataA = &(pListA->Data[j]);
                    pDataB = &(pListB->Data[j]);

                    AmbaMisra_TypeCast(&AddrA, &pDataA->DataAddr);
                    AmbaMisra_TypeCast(&AddrB, &pDataB->DataAddr);

                    Diff = 0U;
                    Len = (ULONG)pDataA->DataSize;

                    if (pDataA->DataSize == pDataB->DataSize) {
                        {
                            for (k = 0U; k < Len; k++) {
                                AmbaMisra_TypeCast(&pA, &AddrA);
                                AmbaMisra_TypeCast(&pB, &AddrB);

                                if ((*pA) != (*pB)) {
                                    Diff = 1U;
                                    break;
                                }

                                AddrA++;
                                AddrB++;

                                if ((pDataA->BufBase != 0U) && (pDataA->BufSize != 0U)) {
                                    if (AddrA >= (pDataA->BufBase + (ULONG)pDataA->BufSize)) {
                                        AddrA = pDataA->BufBase;
                                    }
                                }

                                if ((pDataB->BufBase != 0U) && (pDataB->BufSize != 0U)) {
                                    if (AddrB >= (pDataB->BufBase + (ULONG)pDataB->BufSize)) {
                                        AddrB = pDataB->BufBase;
                                    }
                                }
                            }
                        }
                    } else {
                        Diff = 1U;
                    }

                    if (Diff != 0U) {
                        CmpResult = 1U;

                        AmbaMisra_TypeCast(&pA, &AddrA);
                        AmbaMisra_TypeCast(&pB, &AddrB);

                        AmbaPrint_PrintUInt5("DataCmp result different, ErrCount %u", (UINT32)(pPriv->ErrCount + 1ULL), 0U, 0U, 0U, 0U);
                        AmbaMisra_TypeCast(&Addr, &(pDataA->DataAddr));
                        AmbaPrint_PrintUInt5("    Data[%u][%u][%u]: Addr 0x%x, Size 0x%x", i, j, (UINT32)(CmpWCount - 1ULL), (UINT32)Addr, pDataA->DataSize);
                        AmbaPrint_PrintUInt5("                      Offset 0x%x, Val 0x%x", (UINT32)(AddrA - Addr), (UINT32)(*pA), 0U, 0U, 0U);
                        AmbaMisra_TypeCast(&Addr, &(pDataB->DataAddr));
                        AmbaPrint_PrintUInt5("    Data[%u][%u][%u]: Addr 0x%x, Size 0x%x", (UINT32)i + (UINT32)1U, j, (UINT32)(CmpWCount - 1ULL), (UINT32)Addr, pDataB->DataSize);
                        AmbaPrint_PrintUInt5("                      Offset 0x%x, Val 0x%x", (UINT32)(AddrB - Addr), (UINT32)(*pB), 0U, 0U, 0U);
                    }
                }
            }
        }

        /* upadte RCount */
        if (DoCmp == 1U) {
            MtxTake = 1U;
            Err = AmbaKAL_MutexTake(&(pPriv->Mtx), DTCMP_TIMEOUT);
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(LOG_DTCMP, "AmbaKAL_MutexTake failed %u", Err, 0U);
                MtxTake = 0U;
                LoopU  = 0U;
            }

            for (i = 0U; i < pPriv->CmpNum; i++) {
                pPriv->RCount[i]++;
            }

            if (CmpResult != 0U) {
                pPriv->ErrCount++;
            }

            pPriv->CmpCount++;

            if (MtxTake == 1U) {
                Err = AmbaKAL_MutexGive(&(pPriv->Mtx));
                if (Err != KAL_ERR_NONE) {
                    SvcLog_NG(LOG_DTCMP, "AmbaKAL_MutexGive failed %u", Err, 0U);
                    LoopU  = 0U;
                }
            }
        }
    }

    return NULL;
}

/**
* create data comparison task
* @param [in]  pHdlr data comparison handler
* @param [in]  pCreate create info
* @return ErrorCode
*/
UINT32 SvcDataCmp_Create(SVC_DATA_CMP_HDLR_s *pHdlr, const SVC_DATA_CMP_TASK_CREATE_s *pCreate)
{
    UINT32                      Rval = SVC_OK, Err, HdlrSize;
    static UINT32               PrivSize = (UINT32)sizeof(SVC_DATA_CMP_HDLR_PRIV_s);
    SVC_DATA_CMP_HDLR_PRIV_s    *pPriv = NULL;

    AmbaMisra_TouchUnused(pHdlr);
    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TypeCast(&pPriv, &pHdlr);

    HdlrSize = (UINT32)sizeof(SVC_DATA_CMP_HDLR_s);

    /* parameters check */
    if (Rval == SVC_OK) {
        if (pCreate->DataNum > (UINT8)SVC_DATA_CMP_MAX_DATA_NUM) {
            SvcLog_NG(LOG_DTCMP, "Invalid DataNum %u/%u", pCreate->DataNum, SVC_DATA_CMP_MAX_DATA_NUM);
            Rval = SVC_NG;
        }
        if (pCreate->CmpNum > (UINT8)DTCMP_MAX_CMP_NUM) {
            SvcLog_NG(LOG_DTCMP, "Invalid DataNum %u/%u", pCreate->DataNum, SVC_DATA_CMP_MAX_DATA_NUM);
            Rval = SVC_NG;
        }
        if (PrivSize >  HdlrSize) {
            SvcLog_NG(LOG_DTCMP, "size of SVC_DATA_CMP_HDLR_s is too small %u/%u ", HdlrSize, PrivSize);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        AmbaSvcWrap_MisraMemset(pPriv, 0, sizeof(SVC_DATA_CMP_HDLR_PRIV_s));

        pPriv->CmpNum    = pCreate->CmpNum;
        pPriv->DataNum   = pCreate->DataNum;
    }

    /* create mutex */
    if (Rval == SVC_OK) {
        static char MtxName[] = "DataCmpMtx";

        Err = AmbaKAL_MutexCreate(&(pPriv->Mtx), MtxName);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(LOG_DTCMP, "AmbaKAL_MutexCreate failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }

    /* create msg queue */
    if (Rval == SVC_OK) {
        static char MsgName[] = "DataCmpQue";

        Err = AmbaKAL_MsgQueueCreate(&(pPriv->Msg), MsgName, (UINT32)sizeof(SVC_DATA_CMP_MSG_s),
                                     &(pPriv->MsqBuf[0U]), (UINT32)sizeof(pPriv->MsqBuf));
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(LOG_DTCMP, "AmbaKAL_MsgQueueCreate failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }

    /* creaet task */
    if (Rval == SVC_OK) {
        pPriv->Task.Priority    = pCreate->TaskPriority;
        pPriv->Task.CpuBits     = pCreate->TaskCpuBits;
        pPriv->Task.EntryFunc   = DataCmp_TaskEntry;
        pPriv->Task.pStackBase  = pPriv->TaskStack;
        pPriv->Task.StackSize   = (UINT32)sizeof(pPriv->TaskStack);
        AmbaMisra_TypeCast(&(pPriv->Task.EntryArg), &pPriv);

        Err = SvcTask_Create("SvcDataCmp", &(pPriv->Task));
        if (Err != SVC_OK) {
            SvcLog_NG(LOG_DTCMP, "SvcTask_Create failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }

    return Rval;
}

/**
* delete data comparison task
* @param [in]  pHdlr data comparison handler
* @return ErrorCode
*/
UINT32 SvcDataCmp_Delete(SVC_DATA_CMP_HDLR_s *pHdlr)
{
    UINT32                      Rval = SVC_OK, Err;
    SVC_DATA_CMP_HDLR_PRIV_s    *pPriv = NULL;

    AmbaMisra_TouchUnused(pHdlr);
    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TypeCast(&pPriv, &pHdlr);

    /* delete task */
    if (Rval == SVC_OK) {
        Err = SvcTask_Destroy(&(pPriv->Task));
        if (Err != SVC_OK) {
            SvcLog_NG(LOG_DTCMP, "SvcTask_Destroy failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }

    /* delete msg que */
    if (Rval == SVC_OK) {
        Err = AmbaKAL_MsgQueueDelete(&(pPriv->Msg));
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(LOG_DTCMP, "AmbaKAL_MsgQueueDelete failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }

    /* delete mutex */
    if (Rval == SVC_OK) {
        Err = AmbaKAL_MutexDelete(&(pPriv->Mtx));
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(LOG_DTCMP, "AmbaKAL_MutexDelete failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }

    return Rval;
}

/**
* send data to compare
* @param [in]  pHdlr data comparison handler
* @param [in]  pSend the data to compare
* @return ErrorCode
*/
UINT32 SvcDataCmp_DataSend(SVC_DATA_CMP_HDLR_s *pHdlr, const SVC_DATA_CMP_SEND_s *pSend)
{
    UINT32                      Rval = SVC_OK, Err, MtxTake = 0U;
    UINT8                       CmpId = 0U, i;
    SVC_DATA_CMP_HDLR_PRIV_s    *pPriv = NULL;

    AmbaMisra_TouchUnused(pHdlr);
    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TypeCast(&pPriv, &pHdlr);

    if (Rval == SVC_OK) {
        if (pSend->CmpId >= (UINT8)DTCMP_MAX_CMP_NUM) {
            SvcLog_NG(LOG_DTCMP, "Invalid CmpId %u/%u", CmpId, DTCMP_MAX_CMP_NUM);
            Rval = SVC_NG;
        } else {
            CmpId = pSend->CmpId;
        }
        if (pSend->NumData != pPriv->DataNum) {
            SvcLog_NG(LOG_DTCMP, "Invalid NumData %u/%u", pSend->NumData, pPriv->DataNum);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        Err = AmbaKAL_MutexTake(&(pPriv->Mtx), DTCMP_TIMEOUT);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(LOG_DTCMP, "AmbaKAL_MutexTake failed %u", Err, 0U);
            Rval = SVC_NG;
        } else {
            MtxTake = 1U;
        }
    }

    if (Rval == SVC_OK) {
        if ((pPriv->WCount[CmpId] - pPriv->RCount[CmpId]) >= (UINT64)DTCMP_MAX_QUE_NUM) {
            SvcLog_NG(LOG_DTCMP, "DataBuf is fulled", 0U, 0U);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        UINT64                     Index = pPriv->WCount[CmpId] % (UINT64)DTCMP_MAX_QUE_NUM;
        SVC_DATA_CMP_DATA_LIST_s   *pList = &(pPriv->DataList[CmpId][Index]);

        for (i = 0U; i < pPriv->DataNum; i++) {
            AmbaSvcWrap_MisraMemcpy(&(pList->Data[i]), &(pSend->Data[i]), sizeof(SVC_DATA_CMP_DATA_s));
        }

        pPriv->WCount[CmpId]++;
    }

    if (MtxTake == 1U) {
        Err = AmbaKAL_MutexGive(&(pPriv->Mtx));
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(LOG_DTCMP, "AmbaKAL_MutexGive failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        SVC_DATA_CMP_MSG_s Msg;

        Msg.CmpId = CmpId;

        Err = AmbaKAL_MsgQueueSend(&(pPriv->Msg), &Msg, DTCMP_TIMEOUT);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(LOG_DTCMP, "AmbaKAL_MsgQueueSend failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }

    return Rval;

}

/**
* get comparison report
* @param [in]  pHdlr data comparison handler
* @param [out]  pReport comparison report
* @return ErrorCode
*/
UINT32 SvcDataCmp_ReportGet(SVC_DATA_CMP_HDLR_s *pHdlr, SVC_DATA_CMP_REPORT_s *pReport)
{
    UINT32                      Rval = SVC_OK, Err, MtxTake = 0U;
    SVC_DATA_CMP_HDLR_PRIV_s    *pPriv = NULL;

    AmbaMisra_TouchUnused(pHdlr);
    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TypeCast(&pPriv, &pHdlr);

    if (Rval == SVC_OK) {
        Err = AmbaKAL_MutexTake(&(pPriv->Mtx), DTCMP_TIMEOUT);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(LOG_DTCMP, "AmbaKAL_MutexTake failed %u", Err, 0U);
            Rval = SVC_NG;
        } else {
            MtxTake = 1U;
        }
    }

    if (Rval == SVC_OK) {
        pReport->ErrCount = pPriv->ErrCount;
        pReport->CmpCount = pPriv->CmpCount;
    }

    if (MtxTake == 1U) {
        Err = AmbaKAL_MutexGive(&(pPriv->Mtx));
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(LOG_DTCMP, "AmbaKAL_MutexGive failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }

    return Rval;
}

