/**
 *  @file AmbaIPC_FlexidagIO.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Test/Reference code for AmbaIPC FlexidagIO
 *
 */

#include "AmbaIntrinsics.h"
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaINT.h"
#include "AmbaLink.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaShell_Commands.h"
#include "libwrapc.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_List.h"
#include "AmbaIPC_Rpc.h"
#include "AmbaIPC_FlexidagIO.h"
#include "AmbaIPC_RpcProg_RT_FlexidagIO.h"
#include "AmbaIPC_RpcProg_LU_FlexidagIO.h"


static UINT8 RpcFlexidagIOSvcStack[AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM][0x4000U] GNU_SECTION_NOZEROINIT;
static CLIENT_ID_t RpcFlexidagIOClient[AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM];
static UINT32 FlexidagIOResultUsed[AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM] = {0};
static AMBA_KAL_MSG_QUEUE_t FlexidagIOResultIdxQueue[AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM];
static AMBA_KAL_MUTEX_t FlexidagIOResultMutex[AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_RPC_LU_FLEXIDAGIO_RESULT_s FlexidagIOResult[AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM][FLEXIDAGIO_RESULT_QUEUE_NUM];
AMBA_IPC_FLEXIDAGIO_CONFIG_f FlexidagIOConfigFun[AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM] = {NULL};
AMBA_IPC_FLEXIDAGIO_RESULT_f FlexidagIOResultFun[AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM] = {NULL};


static void AmbaIPC_FlexidagIO_Svc_Config(void *pMsg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    AMBA_RPC_LU_FLEXIDAGIO_CONFIG_s *pConfig = pMsg;

    if (FlexidagIOConfigFun[pConfig->Channel] != NULL){
        FlexidagIOConfigFun[pConfig->Channel](pConfig->Channel, pConfig->OutType);
    }

    //send ipc reply
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

static void AmbaIPC_FlexidagIO_Svc_Result(void *pMsg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    UINT32 ret;
    UINT32 *pStatus;
    AMBA_RPC_LU_FLEXIDAGIO_RESULT_s *pResult = pMsg;
    static UINT32 ResultIdx[AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM] = {0};

    pStatus = (UINT32 *)(pRet->pResult);
    if (FlexidagIOResultFun[pResult->Channel] != NULL){
        FlexidagIOResultFun[pResult->Channel](pResult->Channel, pResult->Data, pResult->DataLen);
    } else {
        ret = AmbaKAL_MutexTake(&FlexidagIOResultMutex[pResult->Channel], AMBA_KAL_WAIT_FOREVER);
        if (ret == FLEXIDAGIO_OK) {
            if (FlexidagIOResultUsed[pResult->Channel] < FLEXIDAGIO_RESULT_QUEUE_NUM - 1U){
                memcpy(&FlexidagIOResult[pResult->Channel][(ResultIdx[pResult->Channel])], pMsg, (pResult->DataLen + AMBA_RPC_LU_FLEXIDAGIO_RESULT_HEAD));
                ret = AmbaKAL_MsgQueueSend(&FlexidagIOResultIdxQueue[pResult->Channel], &ResultIdx[pResult->Channel], AMBA_KAL_NO_WAIT);
                if (ret == FLEXIDAGIO_OK) {
                    FlexidagIOResultUsed[pResult->Channel] = FlexidagIOResultUsed[pResult->Channel] + 1U;
                    ResultIdx[pResult->Channel] = ResultIdx[pResult->Channel] + 1U;
                    if(ResultIdx[pResult->Channel] >= FLEXIDAGIO_RESULT_QUEUE_NUM) {
                        ResultIdx[pResult->Channel] = 0U;
                    }
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_Svc_Result AmbaKAL_MsgQueueSend() fail", 0U, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_Svc_Result result queue is full. Lower the feeding rate or enlarge the queue size.", 0U, 0U, 0U, 0U, 0U);
                ret = FLEXIDAGIO_ERR;
            }

            if (AmbaKAL_MutexGive(&FlexidagIOResultMutex[pResult->Channel]) != FLEXIDAGIO_OK) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_Svc_Result AmbaKAL_MutexGive error", 0U, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_Svc_Result AmbaKAL_MutexTake error", 0U, 0U, 0U, 0U, 0U);
        }

        if (ret == FLEXIDAGIO_OK) {
            *pStatus = FLEXIDAGIO_OK;
        } else {
            *pStatus = FLEXIDAGIO_ERR;
        }
    }

    //send ipc reply
    pRet->Length = sizeof(UINT32);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

UINT32 AmbaIPC_FlexidagIO_Init(UINT32 Channel, AMBA_IPC_FLEXIDAGIO_CONFIG_f ConfigFun)
{
    UINT32 ret = FLEXIDAGIO_OK;
    int status;
    char FelxiName[64] = "flexidagio_rpc_rt_svc";
    AMBA_IPC_PROG_INFO_s prog_info[AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM];
    AMBA_IPC_PROC_s ProcInfo[AMBA_RPC_PROG_RT_FLEXIDAGIO_PROC_NUM];
    static UINT32 FlexidagIOResultIdxBlk[AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM*FLEXIDAGIO_RESULT_QUEUE_NUM];

    if (Channel >= AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_Init Channel(%d) is big than %d\n", Channel, AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM, 0U, 0U, 0U);
        ret = FLEXIDAGIO_ERR;
    }

    if(ret == FLEXIDAGIO_OK) {
        if (AmbaKAL_MutexCreate(&FlexidagIOResultMutex[Channel], "FlexidagIOResultMutex") != FLEXIDAGIO_OK){
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_Init AmbaKAL_MutexCreate fail", 0U, 0U, 0U, 0U, 0U);
            ret = FLEXIDAGIO_ERR;
        }

        if (AmbaKAL_MsgQueueCreate(&FlexidagIOResultIdxQueue[Channel], "FlexidagIOResultIdxQueue", sizeof(UINT32), FlexidagIOResultIdxBlk, AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM*FLEXIDAGIO_RESULT_QUEUE_NUM) != FLEXIDAGIO_OK){
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_Init AmbaKAL_MsgQueueCreate fail", 0U, 0U, 0U, 0U, 0U);
            ret = FLEXIDAGIO_ERR;
        }

        prog_info[Channel].ProcNum = AMBA_RPC_PROG_RT_FLEXIDAGIO_PROC_NUM;
        prog_info[Channel].pProcInfo = (AMBA_IPC_PROC_s *)ProcInfo;
        prog_info[Channel].pProcInfo[0].Mode = AMBA_IPC_SYNCHRONOUS;
        prog_info[Channel].pProcInfo[0].Proc = (AMBA_IPC_PROC_f) &AmbaIPC_FlexidagIO_Svc_Config;
        prog_info[Channel].pProcInfo[1].Mode = AMBA_IPC_SYNCHRONOUS;
        prog_info[Channel].pProcInfo[1].Proc = (AMBA_IPC_PROC_f) &AmbaIPC_FlexidagIO_Svc_Result;
        AmbaUtility_StringAppendUInt32(FelxiName, sizeof(FelxiName), Channel, 10U);
        status = AmbaIPC_SvcRegister((AMBA_RPC_PROG_RT_FLEXIDAGIO_PROG_ID + Channel), AMBA_RPC_PROG_RT_FLEXIDAGIO_VER, FelxiName,
                            65U, (void *)&RpcFlexidagIOSvcStack[Channel], 0x4000U, &prog_info[Channel], 1);
        if(status != 0) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_Init AmbaIPC_SvcRegister fail (%d)\n", status, 0U, 0U, 0U, 0U);
            ret = FLEXIDAGIO_ERR;
        }

        FlexidagIOConfigFun[Channel] = ConfigFun;
    }

    return ret;
}

UINT32 AmbaIPC_FlexidagIO_Deinit(UINT32 Channel)
{
    UINT32 ret = FLEXIDAGIO_OK;

    if (Channel >= AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_Deinit Channel(%d) is big than %d\n", Channel, AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM, 0U, 0U, 0U);
        ret = FLEXIDAGIO_ERR;
    }

    if(ret == FLEXIDAGIO_OK) {
        if (AmbaKAL_MsgQueueDelete(&FlexidagIOResultIdxQueue[Channel]) != FLEXIDAGIO_OK){
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_Deinit AmbaKAL_MsgQueueDelete fail", 0U, 0U, 0U, 0U, 0U);
            ret = FLEXIDAGIO_ERR;
        }

        if (RpcFlexidagIOClient[Channel] != NULL){
            if (AmbaIPC_ClientDestroy(RpcFlexidagIOClient[Channel]) != FLEXIDAGIO_OK){
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_Deinit AmbaIPC_ClientDestroy fail", 0U, 0U, 0U, 0U, 0U);
                ret = FLEXIDAGIO_ERR;
            } else {
                RpcFlexidagIOClient[Channel] = NULL;
            }
        }

        if (AmbaKAL_MutexDelete(&FlexidagIOResultMutex[Channel]) != FLEXIDAGIO_OK){
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_DeInit AmbaKAL_MutexDelete fail", 0U, 0U, 0U, 0U, 0U);
            ret = FLEXIDAGIO_ERR;
        }

        if (AmbaIPC_SvcUnregister((AMBA_RPC_PROG_RT_FLEXIDAGIO_PROG_ID + Channel), AMBA_RPC_PROG_RT_FLEXIDAGIO_VER) != FLEXIDAGIO_OK){
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_Deinit AmbaIPC_SvcUnregister fail", 0U, 0U, 0U, 0U, 0U);
            ret = FLEXIDAGIO_ERR;
        }
    }

    return ret;
}

UINT32 AmbaIPC_FlexidagIO_SetInput(UINT32 Channel, void *pData, UINT32 Len)
{
    UINT32 ret = FLEXIDAGIO_OK;
    AMBA_IPC_REPLY_STATUS_e status;
    AMBA_RPC_RT_FLEXIDAGIO_INPUT_s Input;

    if (Channel >= AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_SetInput Channel(%d) is big than %d\n", Channel, AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM, 0U, 0U, 0U);
        ret = FLEXIDAGIO_ERR;
    }

    if (Len > AMBA_RPC_RT_FLEXIDAGIO_INPUT_DATA) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_SetInput Len(%d) is big than %d\n", Len, AMBA_RPC_RT_FLEXIDAGIO_INPUT_DATA, 0U, 0U, 0U);
        ret = FLEXIDAGIO_ERR;
    }

    if(ret == FLEXIDAGIO_OK) {
        if(RpcFlexidagIOClient[Channel] == NULL) {
            RpcFlexidagIOClient[Channel] = AmbaIPC_ClientCreate(AMBA_RPC_PROG_LU_FLEXIDAGIO_HOST,
                                        AMBA_RPC_PROG_LU_FLEXIDAGIO_PROG_ID + Channel,
                                        AMBA_RPC_PROG_LU_FLEXIDAGIO_VER);
            if(RpcFlexidagIOClient[Channel] == NULL) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_SetInput AmbaIPC_ClientCreate fail\n", 0U, 0U, 0U, 0U, 0U);
                ret = FLEXIDAGIO_ERR;
            }
        }
    }

    if(ret == FLEXIDAGIO_OK) {
        if(Len > AMBA_RPC_RT_FLEXIDAGIO_INPUT_DATA) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_SetInput Len (%d) > AMBA_RPC_RT_FLEXIDAGIO_INPUT_DATA (%d)\n", Len, AMBA_RPC_RT_FLEXIDAGIO_INPUT_DATA, 0U, 0U, 0U);
            ret = FLEXIDAGIO_ERR;
        } else {
            Input.Channel = Channel;
            Input.DataLen = Len;
            AmbaWrap_memcpy(&Input.Data[0], pData, Len);
            status = AmbaIPC_ClientCall(RpcFlexidagIOClient[Channel], AMBA_RPC_PROG_LU_FLEXIDAGIO_INPUT, &Input,
                                        (Len+AMBA_RPC_RT_FLEXIDAGIO_INPUT_HEAD), &ret, sizeof(UINT32), 5000);
            if(status != AMBA_IPC_REPLY_SUCCESS) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_SetInput AmbaIPC_ClientCall fail (%d)\n", status, 0U, 0U, 0U, 0U);
                ret = FLEXIDAGIO_ERR;
            }
        }
    }

    return ret;
}

UINT32 AmbaIPC_FlexidagIO_GetResult_SetCB(UINT32 Channel, AMBA_IPC_FLEXIDAGIO_RESULT_f ResultFun)
{
    UINT32 ret = FLEXIDAGIO_OK;

    if (Channel >= AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_GetResult_SetCB Channel(%d) is big than %d\n", Channel, AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM, 0U, 0U, 0U);
        ret = FLEXIDAGIO_ERR;
    } else {
        FlexidagIOResultFun[Channel] = ResultFun;
    }

    return ret;
}


UINT32 AmbaIPC_FlexidagIO_GetResult(UINT32 Channel, void *pData, UINT32 *Len)
{
    UINT32 ret = FLEXIDAGIO_OK;
    UINT32 ResultIdx = 0;

    if (Channel >= AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_GetResult Channel(%d) is big than %d\n", Channel, AMBA_RPC_PROG_RT_FLEXIDAGIO_NUM, 0U, 0U, 0U);
        ret = FLEXIDAGIO_ERR;
    }

    if(ret == FLEXIDAGIO_OK) {
        if (AmbaKAL_MsgQueueReceive(&FlexidagIOResultIdxQueue[Channel], &ResultIdx, AMBA_KAL_WAIT_FOREVER) != FLEXIDAGIO_OK ) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_GetResult AmbaKAL_MsgQueueReceive() fail.", 0U, 0U, 0U, 0U, 0U);
            ret = FLEXIDAGIO_ERR;
        } else {
            ret = AmbaKAL_MutexTake(&FlexidagIOResultMutex[Channel], AMBA_KAL_WAIT_FOREVER);
            if (ret == FLEXIDAGIO_OK) {
                AmbaWrap_memcpy(pData, &FlexidagIOResult[Channel][ResultIdx].Data[0], FlexidagIOResult[Channel][ResultIdx].DataLen);
                *Len = FlexidagIOResult[Channel][ResultIdx].DataLen;
                FlexidagIOResultUsed[Channel] = FlexidagIOResultUsed[Channel] - 1U;
                ret = AmbaKAL_MutexGive(&FlexidagIOResultMutex[Channel]);
                if (ret != FLEXIDAGIO_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_GetResult AmbaKAL_MutexGive error", 0U, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_FlexidagIO_GetResult AmbaKAL_MutexTake error", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }

    return ret;
}

