/*
 *  @file SvcAmageTask.c
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
 *  @details  SvcAmageTask functions
 *
 */
#if defined(CONFIG_QNX)

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"
#include "AmbaUtility.h"
#include "AmbaCache.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcMem.h"
#include "SvcTask.h"
#include "SvcTaskList.h"
#include "SvcAmageTask.h"

#include "AmbaTUNE_USBCtrl.h"


#define SVC_LOG_AMAGE_TASK     "AMAGE_TSK"
#define TCP_SERVER_STACK_SIZE  (0x8000U)
#define TCP_DATA_RBUF_SIZE     (300000U)
#define TCP_DATA_TBUF_SIZE     (300000U)
#define TCP_MAX_QUENUM         (10)

#define EOS_MSG                "AMAGEEOS"

#define DIR_CLI_TO_SRV               48U // ASCII 0
#define DIR_SRV_TO_CLI               49U // ASCII 1
#define TAG_LENGTH                    3U

typedef struct {
    UINT32                  IsCreate;
    int                     SocketFd;
    SVC_TASK_CTRL_s         Task;
    UINT8                   TaskStack[TCP_SERVER_STACK_SIZE];
    UINT8                   RDataBuf[TCP_DATA_RBUF_SIZE];
    UINT8                   TDataBuf[TCP_DATA_TBUF_SIZE];
} SVC_AMAGE_TASK_CTRL_s;


static SVC_AMAGE_TASK_CTRL_s TaskCtrl GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;


static UINT32 Wrap_to_USB_Amage(UINT32 *dir, const UINT32 *item, const UINT32 *loadBufferLength, UINT8 *ObjectBuffer, UINT32 *saveBufferLength){
    UINT32 Rval;
    UINT32 Parameter1 = *dir;
    UINT32 Parameter2 = *loadBufferLength - TAG_LENGTH;
    UINT32 Parameter3 = *item;
    UINT32 Parameter4 = 0U;
    UINT32 Parameter5 = 0U;
    UINT32 ObjectActualLength = 0U;
    UINT32 ObjectOffset = 0U;
    INT32  AmbaItn_USBCtrl_Rval = 0;


    if(Parameter1 == DIR_CLI_TO_SRV){
        AmbaItn_USBCtrl_Rval = AmbaItn_USBCtrl_Communication(0U, Parameter2, Parameter3, Parameter4, Parameter5, saveBufferLength, dir);
        AmbaItn_USBCtrl_Rval = AmbaItn_USBCtrl_Load(ObjectBuffer, ObjectOffset, Parameter2);
        *saveBufferLength = 0U;
    }
    else if(Parameter1 == DIR_SRV_TO_CLI){
        AmbaItn_USBCtrl_Rval = AmbaItn_USBCtrl_Communication(1U, Parameter2, Parameter3, Parameter4, Parameter5, saveBufferLength, dir);
        AmbaItn_USBCtrl_Rval = AmbaItn_USBCtrl_Save(ObjectBuffer, ObjectOffset, *saveBufferLength, &ObjectActualLength);
    } else {
        // misra-c
    }

    if(AmbaItn_USBCtrl_Rval != 0) {
        Rval = SVC_NG;
    } else {
        Rval = SVC_OK;
    }
    return Rval;
}


static UINT32 Amage_DataHandler(void* RAddr, UINT32 RSize, void* TAddr, UINT32 *pTSize)
{
    UINT32 Rval = SVC_OK;
    UINT32 dir = 0U;
    UINT32 item;
    UINT8 *ObjectBuffer;
    UINT8 *RAddrAll;
    UINT8 *TData;

    AmbaMisra_TypeCast(&RAddrAll, &RAddr);

    dir = (UINT32)RAddrAll[0];
    item = (UINT32)RAddrAll[2];
    ObjectBuffer = &RAddrAll[3];

    Rval = Wrap_to_USB_Amage(&dir, &item, &RSize, ObjectBuffer, pTSize);

    /* send message to client */
    AmbaMisra_TypeCast(&TData, &TAddr);
    if (*pTSize != 0U){
        Rval = AmbaWrap_memcpy(TData, ObjectBuffer, *pTSize);
        if (0U != Rval) {
            SvcLog_DBG(SVC_LOG_AMAGE_TASK, "Amage_DataHandler(), fail to AmbaWrap_memcpy(), Rval: %d", Rval, 0U);
        }
    }

    return Rval;
}


static void* TcpServer(void* EntryArg)
{
    UINT32              Rval = SVC_OK;
    UINT32              LoopU = 1U;
    UINT32              IsEos= 0U;
    UINT32              BufOffset = 0U;
    UINT32              TSize;
    int                 RSize;
    int                 NewSocketFd;
    socklen_t           ClientAddrLen;
    struct sockaddr     ClientAddr;

    AmbaMisra_TouchUnused(&EntryArg);

    SvcLog_OK(SVC_LOG_AMAGE_TASK, "TcpServer start", 0U, 0U);

    while (LoopU == 1U) {
        Rval = SVC_OK;

        /* accept */
        NewSocketFd = accept(TaskCtrl.SocketFd, &ClientAddr, &ClientAddrLen);
        if (NewSocketFd < 0) {
            if (0U != AmbaKAL_TaskSleep(1000U)) {
                // misra-c
            }
            continue;
        }

        // SvcLog_DBG(SVC_LOG_AMAGE_TASK, "connection accept!", 0U, 0U);

        /* keep read the data until receive EOS */
        while (LoopU == 1U) {
            /* read data */
            RSize = (int)read(NewSocketFd, &TaskCtrl.RDataBuf[BufOffset], TCP_DATA_RBUF_SIZE);
            BufOffset += (UINT32)RSize;
            if (RSize == 0) {
                // SvcLog_DBG(SVC_LOG_AMAGE_TASK, "RSize = 0", 0U, 0U);
            } else if (RSize < 0) {
                SvcLog_NG(SVC_LOG_AMAGE_TASK, "read failed", 0U, 0U);
                Rval = SVC_NG;
            } else {
                // SvcLog_DBG(SVC_LOG_AMAGE_TASK, "Receive %d bytes", RSize, 0U);
            }

            /* if EOS is received */
            if ((RSize != 0) && (Rval == SVC_OK)) {
                IsEos = 1U;

                if ((SIZE_t)RSize != SvcWrap_strlen(EOS_MSG)) {
                    IsEos = 0U;
                }

                if (IsEos == 1U) {
                    TaskCtrl.RDataBuf[BufOffset] = 0;   // set ASCII '\0'
                    if (0 != SvcWrap_strcmp((char *)&TaskCtrl.RDataBuf[BufOffset-(UINT32)SvcWrap_strlen(EOS_MSG)], EOS_MSG)) {
                        IsEos = 0U;
                    }
                    else{
                        // SvcLog_DBG(SVC_LOG_AMAGE_TASK, "EOS is received", 0U, 0U);
                    }
                }
            }

            /* data process */
            if ((IsEos == 1U) || (RSize == 0)) {
                if (IsEos == 1U) {
                    BufOffset -= (UINT32)SvcWrap_strlen(EOS_MSG);
                }
                TSize = 0U;

                Rval = Amage_DataHandler(TaskCtrl.RDataBuf, BufOffset, TaskCtrl.TDataBuf, &TSize);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_AMAGE_TASK, "Amage_DataHandler failed %u", Rval, 0U);
                    Rval = SVC_NG;
                }
            }

            /* write data */
            if ((IsEos == 1U) && (Rval == SVC_OK)) {
                int ActTSize;

                if (TSize > 0U) {
                    ActTSize = (int)write(NewSocketFd, TaskCtrl.TDataBuf, (size_t)TSize);
                    if ((UINT32)ActTSize != TSize) {
                        SvcLog_NG(SVC_LOG_AMAGE_TASK, "write failed %u/%u", (UINT32)ActTSize, TSize);
                    }
                    else{
                        // SvcLog_DBG(SVC_LOG_AMAGE_TASK, "Write %d bytes", TSize, 0U);
                    }
                }
            }

            /* close socket */
            if ((RSize == 0) || (IsEos == 1U) || (Rval == SVC_NG)) {
                if (Rval == SVC_NG) {
                    SvcLog_NG(SVC_LOG_AMAGE_TASK, "Read failed or Amage_DataHandler failed, close socket", 0U, 0U);
                }
                else{
                    // SvcLog_DBG(SVC_LOG_AMAGE_TASK, "EOS received, close socket", 0U, 0U);
                }
                /* back to accept step */
                BufOffset = 0U;
                break;
            }

            AmbaMisra_TouchUnused(&LoopU);
        }
        if (close(NewSocketFd) < 0) {
            SvcLog_NG(SVC_LOG_AMAGE_TASK, "close failed", 0U, 0U);
        }
    }

    return NULL;
}

/**
 * create amage server (TCP) task
 * @param [in] Ip ip address
 * @param [in] Port port number
 * @return 0-OK, 1-NG
 */
UINT32 SvcAmageTask_ServerCreate(const char *Ip, UINT16 Port)
{
    UINT32             Rval = SVC_OK, Err;

    if (TaskCtrl.IsCreate > 0U) {
        SvcLog_DBG(SVC_LOG_AMAGE_TASK, "TcpServer is already created", 0U, 0U);
        Rval = SVC_NG;
    }

    /* create socket */
    if (Rval == SVC_OK) {
        TaskCtrl.SocketFd = socket(AF_INET, SOCK_STREAM, 0);
        if (TaskCtrl.SocketFd < 0) {
            SvcLog_NG(SVC_LOG_AMAGE_TASK, "socket failed %u", (UINT32)TaskCtrl.SocketFd, 0U);
            Rval = SVC_NG;
        }
    }

    /* bind: set ip and port */
    if (Rval == SVC_OK) {
        struct sockaddr_in         ServerAddr;
        const struct sockaddr_in *pServerAddr;
        struct sockaddr       *ServerAddr_tmp;
        struct in_addr                 IpAddr;

        Err = AmbaWrap_memset(&ServerAddr, 0, sizeof(ServerAddr));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_AMAGE_TASK, "AmbaWrap_memset failed(%u)", Err, 0U);
        }

        if (inet_aton(Ip, &IpAddr) == 0) {
            SvcLog_NG(SVC_LOG_AMAGE_TASK, "inet_aton failed", 0U, 0U);
            Rval = SVC_NG;
        }

        ServerAddr.sin_family        = AF_INET;
        ServerAddr.sin_addr.s_addr   = IpAddr.s_addr;
        ServerAddr.sin_port          = htons(Port);

        pServerAddr = &ServerAddr;
        AmbaMisra_TypeCast(&ServerAddr_tmp, &pServerAddr);
        if (bind(TaskCtrl.SocketFd, ServerAddr_tmp, (UINT32)sizeof(ServerAddr)) < 0) {
            SvcLog_NG(SVC_LOG_AMAGE_TASK, "bind failed", 0U, 0U);
            Rval = SVC_NG;
        }
    }

    /* listen: wait the incoming connection */
    if (Rval == SVC_OK) {
        if (listen(TaskCtrl.SocketFd, TCP_MAX_QUENUM) < 0) {
            SvcLog_NG(SVC_LOG_AMAGE_TASK, "listen failed", 0U, 0U);
            Rval = SVC_NG;
        }
    }

    /* create the server task */
    if (Rval == SVC_OK) {
        TaskCtrl.Task.Priority   = SVC_AMAGE_TCP_SERVER_TASK_PRI;
        TaskCtrl.Task.EntryFunc  = TcpServer;
        TaskCtrl.Task.EntryArg   = 0U;
        TaskCtrl.Task.pStackBase = TaskCtrl.TaskStack;
        TaskCtrl.Task.StackSize  = TCP_SERVER_STACK_SIZE;
        TaskCtrl.Task.CpuBits    = SVC_AMAGE_TCP_SERVER_TASK_CPU_BITS;

        Err = SvcTask_Create("AmageTcpServer", &(TaskCtrl.Task));
        if (SVC_OK != Err) {
            SvcLog_NG(SVC_LOG_AMAGE_TASK, "SvcTask_Create failed : AmageTcpServer %u", Err, 0U);
            Rval = SVC_NG;
        } else {
            TaskCtrl.IsCreate = 1U;
        }
    }

    return Rval;
}


/**
 * delete amage server task
 * @return 0-OK, 1-NG
 */
UINT32 SvcAmageTask_ServerDelete(void)
{
    UINT32 Rval = SVC_OK, Err;

    if (TaskCtrl.IsCreate == 0U) {
        SvcLog_DBG(SVC_LOG_AMAGE_TASK, "the task is not existed", 0U, 0U);
        Rval = SVC_NG;
    }

    /* close socket */
    if (Rval == SVC_OK) {
        if (close(TaskCtrl.SocketFd) < 0) {
            SvcLog_NG(SVC_LOG_AMAGE_TASK, "close failed", 0U, 0U);
        }
    }

    /* delete task */
    if (Rval == SVC_OK) {
        Err = SvcTask_Destroy(&(TaskCtrl.Task));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_AMAGE_TASK, "SvcTask_Destroy failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        TaskCtrl.IsCreate = 0U;
    }


    return Rval;

}

#endif
