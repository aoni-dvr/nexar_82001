/**
 *  @file audio_api.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights includiERR_IMPL, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDIERR_IMPL, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRIERR_IMPLEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDIERR_IMPL, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDIERR_IMPL NEGLIGENCE OR OTHERWISE)
 *  ARISIERR_IMPL IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Audio APIs
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "AmbaKAL.h"
#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_AacEnc.h"
#include "AmbaAudio_AacDec.h"

#include <stdio.h>
#include <pthread.h>
#include <inttypes.h>
#include <syslog.h>
#include "ambarella_audio.h"
#include "audio_api.h"
#if 1
static int AudioLnxInitSet = -1;
static amba_audio_ctrl_t g_audio_ctrl;
#endif

#define AUCMD_CREATE_FLG    0x00000001
#define AUCMD_DATA_FLG      0x00000002
#define AUCMD_STOP_FLG      0x00000004
#define AU_PARSER_STK_SZ    0x2000UL
AMBA_KAL_TASK_t         AuParserTask;
AMBA_KAL_EVENT_FLAG_t   Ain0ParserFlag;
AMBA_KAL_EVENT_FLAG_t   Aout0ParserFlag;
AMBA_KAL_EVENT_FLAG_t   Ain1ParserFlag;
AMBA_KAL_EVENT_FLAG_t   Aout1ParserFlag;
INT8                    AuParserStack[AU_PARSER_STK_SZ];

static AMBA_KAL_EVENT_FLAG_t *GetAuParserFlag(UINT32 I2sIndx, UINT32 Dir)
{
    AMBA_KAL_EVENT_FLAG_t *pFlag;

    if ((I2sIndx == 0U) && (Dir == AUDIO_CAPTURE)) {
        pFlag = &Ain0ParserFlag;
    } else if ((I2sIndx == 0U) && (Dir == AUDIO_PLAYBACK)) {
        pFlag = &Aout0ParserFlag;
    } else if((I2sIndx == 1U) && (Dir == AUDIO_CAPTURE)) {
        pFlag = &Ain1ParserFlag;
    } else if((I2sIndx == 1U) && (Dir == AUDIO_PLAYBACK)) {
        pFlag = &Aout1ParserFlag;
    } else {
        pFlag = NULL;
    }

    return pFlag;
}

static void* AudioLnxMsgParser(void *EntryArg)
{
    UINT32 RetVal;
    AMBA_KAL_EVENT_FLAG_t *pFlag;

    amba_audio_msg_t msg;
    ssize_t received_len;
    unsigned int prio = 5;

    (void)EntryArg;

    while (1) {
        memset(&msg, 0, sizeof(amba_audio_msg_t));
        received_len = mq_receive (g_audio_ctrl.mq_rcv, (char*)&msg, sizeof(msg), &prio);
        if (received_len == -1) {
            printf("AudioLnxMsgParser mq_receive() error\n");
        }

        pFlag = GetAuParserFlag(msg.i2s_idx, msg.dir);
        if (pFlag == NULL) {
            printf("wrong audio parser flag\n");
        }

        switch (msg.cmd) {
        case AUDIO_CMD_CREATE_RDY:
            RetVal = AmbaKAL_EventFlagSet(pFlag, AUCMD_CREATE_FLG);
            break;
        case AUDIO_CMD_DATA_RDY:
            RetVal = AmbaKAL_EventFlagSet(pFlag, AUCMD_DATA_FLG);
            break;
        case AUDIO_CMD_STOP_RDY:
            RetVal = AmbaKAL_EventFlagSet(pFlag, AUCMD_STOP_FLG);
            break;
        default:
            RetVal = 0x1U;
            printf("wrong cmd parameter: 0x%x\n",msg.cmd);
            break;
        }
        if (RetVal != OK) {
            printf("wrong audio_msg_parser: 0x%x\n", RetVal);
        }
    }

    return NULL;
}

static void AudioFlagInit(AMBA_KAL_EVENT_FLAG_t *pFlag, char *pEventFlagName)
{
    UINT32 RetVal;

    RetVal = AmbaKAL_EventFlagCreate(pFlag, pEventFlagName);
    if (RetVal == OK) {
        RetVal = AmbaKAL_EventFlagClear(pFlag, 0xffffffffUL);
        if (RetVal != OK) {
            AmbaPrint_PrintUInt5("AudioFlagInit: clear fail 0x%x", RetVal, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("AudioFlagInit: create fail 0x%x", RetVal, 0U, 0U, 0U, 0U);
    }
}

static void AudioLnxInit(void)
{
    int rfd;
    UINT32 RetVal;

    if (AudioLnxInitSet == -1) {
        printf("%s: start\n", __func__);
        // get share memory address
        /* Create shared memory object and set its size */
        rfd = shm_open(SHM_AUDIO_BUF, O_RDWR, S_IRUSR | S_IWUSR);

        if (rfd == -1) {
            /* Handle error */;
             printf("error");
        }

        /* Map shared memory object */
        g_audio_ctrl.p_buf = mmap(NULL, AUDIO_MAX_BUFFER_LENGTH,
                PROT_READ | PROT_WRITE, MAP_SHARED, rfd, 0);
        if (g_audio_ctrl.p_buf == MAP_FAILED) {
             /* Handle error */;
             printf("error");
        }

        /* Message queue init */
        g_audio_ctrl.mq_snd = mq_open( MQ_AUDIO_SND, O_RDWR , S_IRWXU | S_IRWXG, NULL );
        if (g_audio_ctrl.mq_snd == -1) {
            perror ("SND mq_open()");
        }
        g_audio_ctrl.mq_rcv = mq_open( MQ_AUDIO_RCV, O_RDWR , S_IRWXU | S_IRWXG, NULL );
        if (g_audio_ctrl.mq_rcv == -1) {
            perror ("RCV mq_open()");
        }

        AudioFlagInit(&Ain0ParserFlag, "Audio I2S0 Rec Msg Status");
        AudioFlagInit(&Aout0ParserFlag, "Audio I2S0 Playback Msg Status");
        AudioFlagInit(&Ain1ParserFlag, "Audio I2S1 Rec Msg Status");
        AudioFlagInit(&Aout1ParserFlag, "Audio I2S1 Playback Msg Status");

        RetVal = AmbaKAL_TaskCreate(&AuParserTask, "Audio Msg Parser", 20U,
                                    AudioLnxMsgParser, NULL, AuParserStack,
                                    AU_PARSER_STK_SZ, 1U/*auto start*/);
        if (RetVal != OK) {
            AmbaPrint_PrintUInt5("AudioLnxInit: task create fail 0x%x", RetVal, 0U, 0U, 0U, 0U);
        }

        AudioLnxInitSet = 0;
        printf("%s: end\n", __func__);
    } else {
        printf("%s: already init\n", __func__);
    }
}

UINT32 AmbaAudio_LnxCreate(UINT32 Dir, UINT32 Freq, UINT32 HwIndex, UINT32 DmaFrameSize, UINT32 ChannelNum, UINT32 DmicEn)
{
    unsigned int prio = 5;
    amba_audio_msg_t msg;
    UINT32 ActualFlags = 0U, RetVal;
    AMBA_KAL_EVENT_FLAG_t *pFlag;

    AudioLnxInit();

    memset(&msg, 0, sizeof(amba_audio_msg_t));
    msg.i2s_idx = HwIndex;
    msg.dmic_en = DmicEn;
    msg.dir = Dir;
    msg.cmd = AUDIO_CMD_CREATE;
    msg.frames = DmaFrameSize;
    msg.channels = ChannelNum;
    msg.freq = Freq;
    mq_send(g_audio_ctrl.mq_snd, (char*)&msg, sizeof(msg), prio);

    pFlag = GetAuParserFlag(msg.i2s_idx, msg.dir);
    if (pFlag == NULL) {
        printf("wrong audio parser flag\n");
    }

    RetVal = AmbaKAL_EventFlagGet(pFlag, AUCMD_CREATE_FLG, 0U/*or*/, 1U/*auto clear*/,
                                  &ActualFlags, AMBA_KAL_WAIT_FOREVER);

    AmbaPrint_PrintUInt5("AmbaAudio_LnxCreate: %d %d %d %d", msg.i2s_idx, msg.dir, msg.cmd, DmicEn, 0U);

    return RetVal;
}


UINT32 AmbaAudio_LnxData(UINT32 I2sIdx, UINT32 Dir, UINT32 *pData, UINT32 Size)
{
    unsigned int prio = 5;
    amba_audio_msg_t msg;
    UINT32 ActualFlags = 0U, RetVal;
    AMBA_KAL_EVENT_FLAG_t *pFlag;
    UINT8 *p_buf;

    memset(&msg, 0, sizeof(amba_audio_msg_t));

    msg.i2s_idx = I2sIdx;
    msg.dir = Dir;
    msg.cmd = AUDIO_CMD_DATA;
    msg.frames = 1024;
    msg.channels = 2;
    msg.freq = 48000;
    mq_send(g_audio_ctrl.mq_snd, (char*)&msg, sizeof(msg), prio);

    pFlag = GetAuParserFlag(msg.i2s_idx, msg.dir);
    if (pFlag == NULL) {
        printf("wrong audio parser flag\n");
    }

    RetVal = AmbaKAL_EventFlagGet(pFlag, AUCMD_DATA_FLG, 0U/*or*/, 1U/*auto clear*/,
                                  &ActualFlags, AMBA_KAL_WAIT_FOREVER);

    if (RetVal == OK) {
        if (Dir == AUDIO_CAPTURE) {
            p_buf =  g_audio_ctrl.p_buf + (I2sIdx*AUDIO_MAX_BUFFER_LENGTH/2);
            RetVal = AmbaWrap_memcpy(pData, p_buf, Size);
        } else if (Dir == AUDIO_PLAYBACK) {
            p_buf =  g_audio_ctrl.p_buf + (AUDIO_FRAME_SIZE*4*AUDIO_CHANNELS) + (I2sIdx*AUDIO_MAX_BUFFER_LENGTH/2);
            RetVal = AmbaWrap_memcpy(p_buf, pData, Size);
        } else {
            RetVal = AIN_ERR_0006;
        }
    }

    //AmbaPrint_PrintUInt5("AmbaAudio_LnxData: %d %d %d 0x%x 0x%x", msg.i2s_idx, msg.dir, msg.cmd, *g_audio_ctrl.p_buf, RetVal);

    return RetVal;
}

UINT32 AmbaAudio_LnxStop(UINT32 I2sIdx, UINT32 Dir)
{
    unsigned int prio = 5;
    amba_audio_msg_t msg;
    UINT32 ActualFlags = 0U, RetVal;
    AMBA_KAL_EVENT_FLAG_t *pFlag;

    memset(&msg, 0, sizeof(amba_audio_msg_t));

    msg.i2s_idx = I2sIdx;
    msg.dir = Dir;
    msg.cmd = AUDIO_CMD_STOP;
    msg.frames = 1024;
    msg.channels = 2;
    msg.freq = 48000;
    mq_send(g_audio_ctrl.mq_snd, (char*)&msg, sizeof(msg), prio);

    pFlag = GetAuParserFlag(msg.i2s_idx, msg.dir);
    if (pFlag == NULL) {
        printf("wrong audio parser flag\n");
    }

    RetVal = AmbaKAL_EventFlagGet(pFlag, AUCMD_STOP_FLG, 0U/*or*/, 1U/*auto clear*/,
                                  &ActualFlags, AMBA_KAL_WAIT_FOREVER);

    return RetVal;
}
