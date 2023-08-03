/*
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _AMBA_AUDIO_SERV
#define _AMBA_AUDIO_SERV


#include <stdint.h>

#define SHM_AUDIO_BUF   "/AudioBuf"
#define MQ_AUDIO_SND    "/AmbaAudioQueueSnd"
#define MQ_AUDIO_RCV    "/AmbaAudioQueueRcv"

#define AUDIO_REQUEST_TIMEOUT     (100U)        /* in nanoseconds */
#define AUDIO_FRAME_SIZE          (2048)
#define AUDIO_CHANNELS            (2)
#define AUDIO_MAX_BUFFER_LENGTH   (AUDIO_FRAME_SIZE*4*AUDIO_CHANNELS*4)  /* 2048 sampels, S32, 2 channels, 2*(AIN+AOUT) */
#define AUDIO_REQUEST_QUEUE_NUM   (200U)

#define AUDIO_CMD_CREATE      0x0000
#define AUDIO_CMD_DATA        0x0001
#define AUDIO_CMD_STOP        0x0002
#define AUDIO_CMD_CREATE_RDY  0x0003
#define AUDIO_CMD_DATA_RDY    0x0004
#define AUDIO_CMD_STOP_RDY    0x0005

#define AUDIO_CAPTURE   0
#define AUDIO_PLAYBACK  1

/* audio message queue contents */
typedef struct _amba_audio_msg {
    INT8  i2s_idx;  /* 0; 1 */
    INT8  dmic_en;
    INT8  dir;      /* 0: capture; 1: playback */
    INT16 cmd;      /* AUDIO_CMD_ */
    INT16 frames;   /* frame size  */
    INT16 channels; /* channel number */
    INT32 freq;     /* sampling frequency */
} amba_audio_msg_t;

typedef struct _amba_audio_param {
    INT16 capture_frames;   /* frame size  */
    INT16 playback_frames;
    INT16 channels; /* channel number */
    INT32 freq;     /* sampling frequency */
} amba_audio_param_t;

/* audio control */
typedef struct _amba_audio_ctrl {
    UINT8               *p_buf;
    mqd_t               mq_snd;
    mqd_t               mq_rcv;
    amba_audio_param_t  param[2];
} amba_audio_ctrl_t;

#endif
