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

#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaIPC_FlexidagIO.h"

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
    uint32_t Channel;
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
    uint32_t Channel;
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

uint32_t AmbaIPC_FlexidagIO_Init(uint32_t Channel, AMBA_IPC_FLEXIDAGIO_CONFIG_f ConfigFun)
{
    uint32_t ret = FLEXIDAGIO_OK;
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

        FlexidagIOConfigFun[Channel] = ConfigFun;
        if (ConfigFun != NULL) {
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

uint32_t AmbaIPC_FlexidagIO_Deinit(uint32_t Channel)
{
    uint32_t ret = FLEXIDAGIO_OK;

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

uint32_t AmbaIPC_FlexidagIO_Delete(uint32_t Channel)
{
    uint32_t ret = FLEXIDAGIO_OK;
    char FlexidagIOQueue[64];

    if (Channel >= AMBA_IPC_FLEXIDAGIO_NUM) {
        printf("AmbaIPC_FlexidagIO_Delete() : Channel(%d) is big than %d\n", Channel, AMBA_IPC_FLEXIDAGIO_NUM);
        ret = FLEXIDAGIO_ERR;
    }

    if(ret == FLEXIDAGIO_OK) {
        snprintf(FlexidagIOQueue, sizeof(FlexidagIOQueue), "/FlexidagIOConfigQueue_%d", Channel);
        if (mq_unlink(FlexidagIOQueue) == -1) {
            printf("AmbaIPC_FlexidagIO_Delete() : mq_unlink FlexidagIOConfigQueue_%d fail errno=%d\n", Channel, errno);
            ret = FLEXIDAGIO_ERR;
        }

        snprintf(FlexidagIOQueue, sizeof(FlexidagIOQueue), "/FlexidagIOInputQueue_%d", Channel);
        if (mq_unlink(FlexidagIOQueue) == -1) {
            printf("AmbaIPC_FlexidagIO_Deinit() : mq_unlink FlexidagIOInputQueue_%d fail errno=%d\n", Channel, errno);
            ret = FLEXIDAGIO_ERR;
        }

        snprintf(FlexidagIOQueue, sizeof(FlexidagIOQueue), "/FlexidagIOResultQueue_%d", Channel);
        if (mq_unlink(FlexidagIOQueue) == -1) {
            printf("AmbaIPC_FlexidagIO_Deinit() : mq_unlink FlexidagIOResultQueue_%d fail errno=%d\n", Channel, errno);
            ret = FLEXIDAGIO_ERR;
        }
    }

    return ret;
}

uint32_t AmbaIPC_FlexidagIO_Config(uint32_t Channel, uint32_t OutType)
{
    uint32_t ret = FLEXIDAGIO_OK;
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

uint32_t AmbaIPC_FlexidagIO_SetInput(uint32_t Channel, void *pData, uint32_t Len)
{
    uint32_t ret = FLEXIDAGIO_OK;
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

uint32_t AmbaIPC_FlexidagIO_GetInput(uint32_t Channel, void *pData, uint32_t *Len)
{
    uint32_t ret = FLEXIDAGIO_OK;
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


uint32_t AmbaIPC_FlexidagIO_GetResult_SetCB(uint32_t Channel, AMBA_IPC_FLEXIDAGIO_RESULT_f ResultFun)
{
    uint32_t ret = FLEXIDAGIO_OK;
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
        } else {
            if (FlexidagIOResultPthreadFlag[Channel] == 1) {
                pthread_cancel(FlexidagIOResultPthread[Channel]);
                pthread_join(FlexidagIOResultPthread[Channel], NULL);
                FlexidagIOResultPthreadFlag[Channel] = 0;
            }
        }
    }

    return ret;
}

uint32_t AmbaIPC_FlexidagIO_SetResult(uint32_t Channel, void *pData, uint32_t Len)
{
    uint32_t ret = FLEXIDAGIO_OK;
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

uint32_t AmbaIPC_FlexidagIO_GetResult(uint32_t Channel, void *pData, uint32_t *Len)
{
    uint32_t ret = FLEXIDAGIO_OK;
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


