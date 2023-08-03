#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "AmbaFlexidagIO.h"
#ifdef AMBAIPC
#include "aipc_user.h"
#include "AmbaIPC_RpcProg_RT_FlexidagIO.h"
#include "AmbaIPC_RpcProg_LU_FlexidagIO.h"


typedef struct {
    long mtype;
    char mtext[1024];
} MSGQUEUE_BUF_s;

static CLIENT_ID_t RpcFlexidagIOClient[AMBA_RPC_PROG_LU_FLEXIDAGIO_NUM];
static int FlexidagIOHandleQueue[AMBA_RPC_PROG_LU_FLEXIDAGIO_NUM];


static void AmbaIPC_FlexidagIO_Svc_Input(void *pMsg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    unsigned int *pStatus;
    AMBA_RPC_RT_FLEXIDAGIO_INPUT_s *pInput = pMsg;
    MSGQUEUE_BUF_s FlexidagIOMbuf;

    pStatus = (unsigned int *)(pRet->pResult);
    FlexidagIOMbuf.mtype = 1;
    memcpy(&FlexidagIOMbuf.mtext, pMsg, (pInput->DataLen + AMBA_RPC_RT_FLEXIDAGIO_INPUT_HEAD));
    if (msgsnd(FlexidagIOHandleQueue[pInput->Channel], &FlexidagIOMbuf, sizeof(FlexidagIOMbuf.mtext), 0) != FLEXIDAGIO_OK) {
        printf("AmbaIPC_FlexidagIO_Svc_Input msgsnd() fail\n");
        *pStatus = FLEXIDAGIO_ERR;
    } else {
        *pStatus = FLEXIDAGIO_OK;
    }
    //send ipc reply
    pRet->Length = sizeof(unsigned int);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

unsigned int AmbaIPC_FlexidagIO_Init(unsigned int Channel)
{
    unsigned int ret = FLEXIDAGIO_OK;
    int status;
    char FelxiName[64] = {0};
    AMBA_IPC_PROG_INFO_s prog_info[AMBA_RPC_PROG_LU_FLEXIDAGIO_NUM];
    AMBA_IPC_PROC_s ProcInfo[AMBA_RPC_PROG_LU_FLEXIDAGIO_PROC_NUM];

    if(Channel >= AMBA_RPC_PROG_LU_FLEXIDAGIO_NUM) {
        printf("AmbaIPC_FlexidagIO_Init Channel(%d) is big than %d\n", Channel, AMBA_RPC_PROG_LU_FLEXIDAGIO_NUM);
        ret = FLEXIDAGIO_ERR;
    }

    if(ret == FLEXIDAGIO_OK) {
        FlexidagIOHandleQueue[Channel] = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
        if(FlexidagIOHandleQueue[Channel] < FLEXIDAGIO_OK) {
            printf("AmbaIPC_FlexidagIO_Init msgget fail(%d)\n", FlexidagIOHandleQueue[Channel]);
            ret = FLEXIDAGIO_ERR;
        }

        prog_info[Channel].ProcNum = AMBA_RPC_PROG_LU_FLEXIDAGIO_PROC_NUM;
        prog_info[Channel].pProcInfo = (AMBA_IPC_PROC_s *)ProcInfo;
        prog_info[Channel].pProcInfo[0].Mode = AMBA_IPC_SYNCHRONOUS;
        prog_info[Channel].pProcInfo[0].Proc = (AMBA_IPC_PROC_f) &AmbaIPC_FlexidagIO_Svc_Input;
        snprintf(FelxiName, sizeof(FelxiName), "flexidagio_rpc_lu_svc%d", Channel);
        status = ambaipc_svc_register(AMBA_RPC_PROG_LU_FLEXIDAGIO_PROG_ID + Channel, AMBA_RPC_PROG_LU_FLEXIDAGIO_VER, FelxiName, &prog_info[Channel], 1);
        if(status != 0) {
            printf("AmbaIPC_FlexidagIO_Init AmbaIPC_SvcRegister fail (%d)\n", status);
            ret = FLEXIDAGIO_ERR;
        }
    }

    return ret;
}

unsigned int AmbaIPC_FlexidagIO_Deinit(unsigned int Channel)
{
    unsigned int ret = FLEXIDAGIO_OK;

    if(Channel >= AMBA_RPC_PROG_LU_FLEXIDAGIO_NUM) {
        printf("AmbaIPC_FlexidagIO_Deinit Channel(%d) is big than %d\n", Channel, AMBA_RPC_PROG_LU_FLEXIDAGIO_NUM);
        ret = FLEXIDAGIO_ERR;
    }

    if(ret == FLEXIDAGIO_OK) {
        if(RpcFlexidagIOClient[Channel] != NULL) {
            ambaipc_clnt_destroy(RpcFlexidagIOClient[Channel]);
        }
        ambaipc_svc_unregister(AMBA_RPC_PROG_LU_FLEXIDAGIO_PROG_ID + Channel, AMBA_RPC_PROG_LU_FLEXIDAGIO_VER);
    }

    return ret;
}

unsigned int AmbaIPC_FlexidagIO_Config(unsigned int Channel, unsigned int OutType)
{
    unsigned int ret = FLEXIDAGIO_OK;
    AMBA_IPC_REPLY_STATUS_e status;
    AMBA_RPC_LU_FLEXIDAGIO_CONFIG_s Config;

    if(Channel >= AMBA_RPC_PROG_LU_FLEXIDAGIO_NUM) {
        printf("AmbaIPC_FlexidagIO_Config Channel(%d) is big than %d\n", Channel, AMBA_RPC_PROG_LU_FLEXIDAGIO_NUM);
        ret = FLEXIDAGIO_ERR;
    }

    if(ret == FLEXIDAGIO_OK) {
        if(RpcFlexidagIOClient[Channel] == NULL) {
            RpcFlexidagIOClient[Channel] = ambaipc_clnt_create(AMBA_RPC_PROG_RT_FLEXIDAGIO_HOST,
                                           AMBA_RPC_PROG_RT_FLEXIDAGIO_PROG_ID + Channel,
                                           AMBA_RPC_PROG_RT_FLEXIDAGIO_VER);
            if(RpcFlexidagIOClient[Channel] == NULL) {
                printf("AmbaIPC_FlexidagIO_Config ambaipc_clnt_create fail\n");
                ret = FLEXIDAGIO_ERR;
            }
        }
    }

    if(ret == FLEXIDAGIO_OK) {
        Config.Channel = Channel;
        Config.OutType = OutType;
        status = ambaipc_clnt_call(RpcFlexidagIOClient[Channel], AMBA_RPC_PROG_RT_FLEXIDAGIO_CONFIG,
                                   (void *) &Config, sizeof(AMBA_RPC_LU_FLEXIDAGIO_CONFIG_s), NULL, 0, 5000);
        if(status != AMBA_IPC_REPLY_SUCCESS) {
            printf("AmbaIPC_FlexidagIO_Config AmbaIPC_ClientCall fail (%d)\n", status);
            ret = FLEXIDAGIO_ERR;
        }
    }

    return ret;
}


unsigned int AmbaIPC_FlexidagIO_GetInput(unsigned int Channel, void *pData, unsigned int *Len)
{
    unsigned int ret = FLEXIDAGIO_OK;
    AMBA_RPC_RT_FLEXIDAGIO_INPUT_s *pInput;
    MSGQUEUE_BUF_s FlexidagIOMbuf;

    if(Channel >= AMBA_RPC_PROG_LU_FLEXIDAGIO_NUM) {
        printf("AmbaIPC_FlexidagIO_GetInput Channel(%d) is big than %d\n", Channel, AMBA_RPC_PROG_LU_FLEXIDAGIO_NUM);
        ret = FLEXIDAGIO_ERR;
    }

    if(ret == FLEXIDAGIO_OK) {
        if (msgrcv(FlexidagIOHandleQueue[Channel], &FlexidagIOMbuf, sizeof(FlexidagIOMbuf.mtext), 0, 0) < FLEXIDAGIO_OK) {
            printf("AmbaIPC_FlexidagIO_GetInput msgrcv() fail.\n");
            ret = FLEXIDAGIO_ERR;
        } else {
            pInput = (AMBA_RPC_RT_FLEXIDAGIO_INPUT_s *)&FlexidagIOMbuf.mtext;
            memcpy(pData, pInput->Data, pInput->DataLen);
            *Len = pInput->DataLen;
        }
    }

    return ret;
}

unsigned int AmbaIPC_FlexidagIO_SetResult(unsigned int Channel, void *pData, unsigned int Len)
{
    unsigned int ret = FLEXIDAGIO_OK;
    AMBA_IPC_REPLY_STATUS_e status;
    AMBA_RPC_LU_FLEXIDAGIO_RESULT_s Result;

    if(Channel >= AMBA_RPC_PROG_LU_FLEXIDAGIO_NUM) {
        printf("AmbaIPC_FlexidagIO_SetResult Channel(%d) is big than %d\n", Channel, AMBA_RPC_PROG_LU_FLEXIDAGIO_NUM);
        ret = FLEXIDAGIO_ERR;
    }

    if(Len > AMBA_RPC_LU_FLEXIDAGIO_RESULT_DATA) {
        printf("AmbaIPC_FlexidagIO_SetResult Len(%d) is big than %d\n", Len, AMBA_RPC_LU_FLEXIDAGIO_RESULT_DATA);
        ret = FLEXIDAGIO_ERR;
    }

    if(ret == FLEXIDAGIO_OK) {
        if(RpcFlexidagIOClient[Channel] == NULL) {
            RpcFlexidagIOClient[Channel] = ambaipc_clnt_create(AMBA_RPC_PROG_RT_FLEXIDAGIO_HOST,
                                           AMBA_RPC_PROG_RT_FLEXIDAGIO_PROG_ID + Channel,
                                           AMBA_RPC_PROG_RT_FLEXIDAGIO_VER);
            if(RpcFlexidagIOClient[Channel] == NULL) {
                printf("AmbaIPC_FlexidagIO_SetResult ambaipc_clnt_create fail\n");
                ret = FLEXIDAGIO_ERR;
            }
        }
    }

    if(ret == FLEXIDAGIO_OK) {
        Result.Channel = Channel;
        Result.DataLen = Len;
        memcpy(&Result.Data[0], pData, Len);
        status = ambaipc_clnt_call(RpcFlexidagIOClient[Channel], AMBA_RPC_PROG_RT_FLEXIDAGIO_RESULT,
                                   (void *) &Result, (Len+AMBA_RPC_LU_FLEXIDAGIO_RESULT_HEAD), &ret, sizeof(unsigned int), 5000);
        if(status != AMBA_IPC_REPLY_SUCCESS) {
            printf("AmbaIPC_FlexidagIO_Result AmbaIPC_ClientCall fail (%d)\n", status);
            ret = FLEXIDAGIO_ERR;
        }
    }

    return ret;
}

#else
#include <pthread.h>
#include <mqueue.h>

static unsigned int FlexidagIOConfigPthreadFlag[AMBA_IPC_FLEXIDAGIO_NUM] = {0};
static unsigned int FlexidagIOResultPthreadFlag[AMBA_IPC_FLEXIDAGIO_NUM] = {0};
static pthread_t FlexidagIOConfigPthread[AMBA_IPC_FLEXIDAGIO_NUM];
static pthread_t FlexidagIOResultPthread[AMBA_IPC_FLEXIDAGIO_NUM];
static mqd_t FlexidagIOConfigQueue[AMBA_IPC_FLEXIDAGIO_NUM];
static mqd_t FlexidagIOInputQueue[AMBA_IPC_FLEXIDAGIO_NUM];
static mqd_t FlexidagIOResultQueue[AMBA_IPC_FLEXIDAGIO_NUM];
AMBA_IPC_FLEXIDAGIO_CONFIG_f FlexidagIOConfigFun[AMBA_IPC_FLEXIDAGIO_NUM] = {NULL};
AMBA_IPC_FLEXIDAGIO_RESULT_f FlexidagIOResultFun[AMBA_IPC_FLEXIDAGIO_NUM] = {NULL};


static void* FlexidagIO_GetConfig_Task(void *arg)
{
    unsigned int Channel;
    AMBA_IPC_FLEXIDAGIO_CONFIG_s Config;
    struct mq_attr attr;

    memcpy(&Channel, &arg, sizeof(Channel));
    while (1) {
        mq_getattr(FlexidagIOConfigQueue[Channel], &attr);
        if (mq_receive(FlexidagIOConfigQueue[Channel], (char *)&Config, attr.mq_msgsize, NULL) != -1) {
            FlexidagIOConfigFun[Channel](Config.Channel, Config.OutType);
        } else {
            printf("FlexidagIO_GetConfig_Task() : mq_receive fail Channel=%d errno=%d\n", Channel, errno);
        }
    }

    return NULL;
}

static void* FlexidagIO_GetResult_Task(void *arg)
{
    unsigned int Channel;
    AMBA_IPC_FLEXIDAGIO_RESULT_s Result;
    struct mq_attr attr;

    memcpy(&Channel, &arg, sizeof(Channel));
    while (1) {
        mq_getattr(FlexidagIOResultQueue[Channel], &attr);
        if (mq_receive(FlexidagIOResultQueue[Channel], (char *)&Result, attr.mq_msgsize, NULL) != -1) {
            FlexidagIOResultFun[Channel](Result.Channel, Result.Data, Result.DataLen);
        } else {
            printf("FlexidagIO_GetResult_Task() : mq_receive fail Channel=%d errno=%d\n", Channel, errno);
        }
    }

    return NULL;
}

unsigned int AmbaIPC_FlexidagIO_Init(unsigned int Channel)
{
    unsigned int ret = FLEXIDAGIO_OK;
    char FlexidagIOQueue[64];
    struct mq_attr attr;
    void *pChannel;

    if (Channel >= AMBA_IPC_FLEXIDAGIO_NUM) {
        printf("AmbaIPC_FlexidagIO_Init() : Channel(%d) is big than %d\n", Channel, AMBA_IPC_FLEXIDAGIO_NUM);
        ret = FLEXIDAGIO_ERR;
    }

    if(ret == FLEXIDAGIO_OK) {
        snprintf(FlexidagIOQueue, sizeof(FlexidagIOQueue), "/FlexidagIOConfigQueue_%d", Channel);
        attr.mq_maxmsg = AMBA_IPC_FLEXIDAGIO_MAXMSGS;
        attr.mq_msgsize = sizeof(AMBA_IPC_FLEXIDAGIO_CONFIG_s);
        FlexidagIOConfigQueue[Channel] = mq_open(FlexidagIOQueue, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG, &attr);
        if (FlexidagIOConfigQueue[Channel] == -1) {
            printf("AmbaIPC_FlexidagIO_Init() : mq_open %s fail errno=%d\n", FlexidagIOQueue, errno);
            ret = FLEXIDAGIO_ERR;
        }

        snprintf(FlexidagIOQueue, sizeof(FlexidagIOQueue), "/FlexidagIOInputQueue_%d", Channel);
        attr.mq_maxmsg = AMBA_IPC_FLEXIDAGIO_MAXMSGS;
        attr.mq_msgsize = sizeof(AMBA_IPC_FLEXIDAGIO_INPUT_s);
        FlexidagIOInputQueue[Channel] = mq_open(FlexidagIOQueue, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG, &attr);
        if (FlexidagIOInputQueue[Channel] == -1) {
            printf("AmbaIPC_FlexidagIO_Init() : mq_open %s fail errno=%d\n", FlexidagIOQueue, errno);
            ret = FLEXIDAGIO_ERR;
        }

        snprintf(FlexidagIOQueue, sizeof(FlexidagIOQueue), "/FlexidagIOResultQueue_%d", Channel);
        attr.mq_maxmsg = AMBA_IPC_FLEXIDAGIO_MAXMSGS;
        attr.mq_msgsize = sizeof(AMBA_IPC_FLEXIDAGIO_RESULT_s);
        FlexidagIOResultQueue[Channel] = mq_open(FlexidagIOQueue, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG, &attr);
        if (FlexidagIOResultQueue[Channel] == -1) {
            printf("AmbaIPC_FlexidagIO_Init() : mq_open %s fail errno=%d\n", FlexidagIOQueue, errno);
            ret = FLEXIDAGIO_ERR;
        }

        FlexidagIOConfigFun[Channel] = NULL;//ConfigFun;
        if (FlexidagIOConfigFun[Channel] != NULL) {
            memcpy(&pChannel, &Channel, sizeof(Channel));
            if (pthread_create(&FlexidagIOConfigPthread[Channel], NULL, FlexidagIO_GetConfig_Task, pChannel) != 0) {
                printf("AmbaIPC_FlexidagIO_Init() : create FlexidagIOConfigPthread[%d] fail\n", Channel);
            } else {
                FlexidagIOConfigPthreadFlag[Channel] = 1;
            }
        }
    }

    return ret;
}

unsigned int AmbaIPC_FlexidagIO_Deinit(unsigned int Channel)
{
    unsigned int ret = FLEXIDAGIO_OK;

    if (Channel >= AMBA_IPC_FLEXIDAGIO_NUM) {
        printf("AmbaIPC_FlexidagIO_Deinit() : Channel(%d) is big than %d\n", Channel, AMBA_IPC_FLEXIDAGIO_NUM);
        ret = FLEXIDAGIO_ERR;
    }

    if(ret == FLEXIDAGIO_OK) {
        if (FlexidagIOConfigPthreadFlag[Channel] == 1) {
            pthread_cancel(FlexidagIOConfigPthread[Channel]);
            pthread_join(FlexidagIOConfigPthread[Channel], NULL);
            FlexidagIOConfigPthreadFlag[Channel] = 0;
        }

        if (FlexidagIOResultPthreadFlag[Channel] == 1) {
            pthread_cancel(FlexidagIOResultPthread[Channel]);
            pthread_join(FlexidagIOResultPthread[Channel], NULL);
            FlexidagIOResultPthreadFlag[Channel] = 0;
        }

        if (mq_close(FlexidagIOConfigQueue[Channel]) == -1) {
            printf("AmbaIPC_FlexidagIO_Deinit() : mq_close FlexidagIOConfigQueue_%d fail errno=%d\n", Channel, errno);
            ret = FLEXIDAGIO_ERR;
        }

        if (mq_close(FlexidagIOInputQueue[Channel]) == -1) {
            printf("AmbaIPC_FlexidagIO_Deinit() : mq_close FlexidagIOInputQueue_%d fail errno=%d\n", Channel, errno);
            ret = FLEXIDAGIO_ERR;
        }

        if (mq_close(FlexidagIOResultQueue[Channel]) == -1) {
            printf("AmbaIPC_FlexidagIO_Deinit() : mq_close FlexidagIOResultQueue_%d fail errno=%d\n", Channel, errno);
            ret = FLEXIDAGIO_ERR;
        }
    }

    return ret;
}

unsigned int AmbaIPC_FlexidagIO_Config(unsigned int Channel, unsigned int OutType)
{
    unsigned int ret = FLEXIDAGIO_OK;
    AMBA_IPC_FLEXIDAGIO_CONFIG_s Config;
    struct mq_attr attr;

    if (Channel >= AMBA_IPC_FLEXIDAGIO_NUM) {
        printf("AmbaIPC_FlexidagIO_Config() : Channel(%d) is big than %d\n", Channel, AMBA_IPC_FLEXIDAGIO_NUM);
        ret = FLEXIDAGIO_ERR;
    }

    if(ret == FLEXIDAGIO_OK) {
        Config.Channel = Channel;
        Config.OutType = OutType;
        mq_getattr(FlexidagIOConfigQueue[Channel], &attr);
        if( mq_send(FlexidagIOConfigQueue[Channel], (char *)&Config, attr.mq_msgsize, 0) == -1) {
            printf("AmbaIPC_FlexidagIO_Config() : mq_send fail Channel=%d errno=%d\n", Channel, errno);
            ret = FLEXIDAGIO_ERR;
        }
    }

    return ret;
}

unsigned int AmbaIPC_FlexidagIO_SetInput(unsigned int Channel, void *pData, unsigned int Len)
{
    unsigned int ret = FLEXIDAGIO_OK;
    AMBA_IPC_FLEXIDAGIO_INPUT_s Input;
    struct mq_attr attr;

    if (Channel >= AMBA_IPC_FLEXIDAGIO_NUM) {
        printf("AmbaIPC_FlexidagIO_SetInput() : Channel(%d) is big than %d\n", Channel, AMBA_IPC_FLEXIDAGIO_NUM);
        ret = FLEXIDAGIO_ERR;
    }

    if (Len > AMBA_IPC_FLEXIDAGIO_INPUT_DATA) {
        printf("AmbaIPC_FlexidagIO_SetInput() : Len(%d) is big than %d\n", Len, AMBA_IPC_FLEXIDAGIO_INPUT_DATA);
        ret = FLEXIDAGIO_ERR;
    }

    if(ret == FLEXIDAGIO_OK) {
        Input.Channel = Channel;
        Input.DataLen = Len;
        memcpy(&Input.Data[0], pData, Len);
        mq_getattr(FlexidagIOInputQueue[Channel], &attr);
        if( mq_send(FlexidagIOInputQueue[Channel], (char *)&Input, attr.mq_msgsize, 0) == -1) {
            printf("AmbaIPC_FlexidagIO_SetInput() : mq_send fail Channel=%d errno=%d\n", Channel, errno);
            ret = FLEXIDAGIO_ERR;
        }
    }

    return ret;
}

unsigned int AmbaIPC_FlexidagIO_GetInput(unsigned int Channel, void *pData, unsigned int *Len)
{
    unsigned int ret = FLEXIDAGIO_OK;
    AMBA_IPC_FLEXIDAGIO_INPUT_s Input;
    struct mq_attr attr;

    if (Channel >= AMBA_IPC_FLEXIDAGIO_NUM) {
        printf("AmbaIPC_FlexidagIO_GetInput() : Channel(%d) is big than %d\n", Channel, AMBA_IPC_FLEXIDAGIO_NUM);
        ret = FLEXIDAGIO_ERR;
    }

    if(ret == FLEXIDAGIO_OK) {
        mq_getattr(FlexidagIOInputQueue[Channel], &attr);
        if (mq_receive(FlexidagIOInputQueue[Channel], (char *)&Input, attr.mq_msgsize, NULL) == -1) {
            printf("AmbaIPC_FlexidagIO_GetInput() : mq_receive fail Channel=%d errno=%d\n", Channel, errno);
            ret = FLEXIDAGIO_ERR;
        } else {
            memcpy(pData, &Input.Data[0], Input.DataLen);
            *Len = Input.DataLen;
        }
    }

    return ret;
}


unsigned int AmbaIPC_FlexidagIO_GetResult_SetCB(unsigned int Channel, AMBA_IPC_FLEXIDAGIO_RESULT_f ResultFun)
{
    unsigned int ret = FLEXIDAGIO_OK;
    void *pChannel;

    if (Channel >= AMBA_IPC_FLEXIDAGIO_NUM) {
        printf("AmbaIPC_FlexidagIO_GetResult_SetCB() : Channel(%d) is big than %d\n", Channel, AMBA_IPC_FLEXIDAGIO_NUM);
        ret = FLEXIDAGIO_ERR;
    } else {
        FlexidagIOResultFun[Channel] = ResultFun;
        if (ResultFun != NULL) {
            memcpy(&pChannel, &Channel, sizeof(Channel));
            if (pthread_create(&FlexidagIOResultPthread[Channel], NULL, FlexidagIO_GetResult_Task, pChannel) != 0) {
                printf("AmbaIPC_FlexidagIO_GetResult_SetCB() : create FlexidagIOResultPthread[%d] fail\n", Channel);
            } else {
                FlexidagIOResultPthreadFlag[Channel] = 1;
            }
        }
    }

    return ret;
}

unsigned int AmbaIPC_FlexidagIO_SetResult(unsigned int Channel, void *pData, unsigned int Len)
{
    unsigned int ret = FLEXIDAGIO_OK;
    AMBA_IPC_FLEXIDAGIO_RESULT_s Result;
    struct mq_attr attr;

    if (Channel >= AMBA_IPC_FLEXIDAGIO_NUM) {
        printf("AmbaIPC_FlexidagIO_SetResult() : Channel(%d) is big than %d\n", Channel, AMBA_IPC_FLEXIDAGIO_NUM);
        ret = FLEXIDAGIO_ERR;
    }

    if (Len > AMBA_IPC_FLEXIDAGIO_RESULT_DATA) {
        printf("AmbaIPC_FlexidagIO_SetResult() : Len(%d) is big than %d\n", Len, AMBA_IPC_FLEXIDAGIO_RESULT_DATA);
        ret = FLEXIDAGIO_ERR;
    }

    if(ret == FLEXIDAGIO_OK) {
        Result.Channel = Channel;
        Result.DataLen = Len;
        memcpy(&Result.Data[0], pData, Len);
        mq_getattr(FlexidagIOResultQueue[Channel], &attr);
        if( mq_send(FlexidagIOResultQueue[Channel], (char *)&Result, attr.mq_msgsize, 0) == -1) {
            printf("AmbaIPC_FlexidagIO_SetResult() : mq_send fail Channel=%d errno=%d\n", Channel, errno);
            ret = FLEXIDAGIO_ERR;
        }
    }

    return ret;
}

unsigned int AmbaIPC_FlexidagIO_GetResult(unsigned int Channel, void *pData, unsigned int *Len)
{
    unsigned int ret = FLEXIDAGIO_OK;
    AMBA_IPC_FLEXIDAGIO_RESULT_s Result;
    struct mq_attr attr;

    if (Channel >= AMBA_IPC_FLEXIDAGIO_NUM) {
        printf("AmbaIPC_FlexidagIO_GetResult() : Channel(%d) is big than %d\n", Channel, AMBA_IPC_FLEXIDAGIO_NUM);
        ret = FLEXIDAGIO_ERR;
    }

    if((ret == FLEXIDAGIO_OK) && (FlexidagIOResultFun[Channel] == NULL)) {
        mq_getattr(FlexidagIOResultQueue[Channel], &attr);
        if (mq_receive(FlexidagIOResultQueue[Channel], (char *)&Result, attr.mq_msgsize, NULL) == -1) {
            printf("AmbaIPC_FlexidagIO_GetResult() : mq_receive fail Channel=%d errno=%d\n", Channel, errno);
            ret = FLEXIDAGIO_ERR;
        } else {
            memcpy(pData, &Result.Data[0], Result.DataLen);
            *Len = Result.DataLen;
        }
    }

    return ret;
}

#endif


