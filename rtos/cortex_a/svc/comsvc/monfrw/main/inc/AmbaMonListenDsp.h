/**
 *  @file AmbaMonListenDsp.h
 *
 *  Copyright (c) [2020] Ambarella International LP
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
 *  @details Constants and Definitions for Amba Monitor Listen DSP
 *
 */

#ifndef AMBA_MONITOR_LISTEN_DSP_H
#define AMBA_MONITOR_LISTEN_DSP_H

typedef enum /*_AMBA_MON_LISTEN_DSP_CMD_e_*/ {
    LISTEN_DSP_TASK_CMD_STOP = 0,
    LISTEN_DSP_TASK_CMD_START
} AMBA_MON_LISTEN_DSP_CMD_e;

typedef struct /*_AMBA_MON_LISTEN_DSP_FLAG_s_*/ {
    UINT64 Flag;
    const char *pName;
} AMBA_MON_LISTEN_DSP_FLAG_s;

typedef struct /*_AMBA_MON_LISTEN_DSP_EVENT_s_*/ {
    UINT64                        Flag;
    AMBA_MON_LISTEN_DSP_FLAG_s    Raw;
    AMBA_MON_LISTEN_DSP_FLAG_s    DefRaw;
    AMBA_MON_LISTEN_DSP_FLAG_s    Yuv;
    AMBA_MON_LISTEN_DSP_FLAG_s    YuvAlt;
    AMBA_MON_LISTEN_DSP_FLAG_s    DefYuvAlt;
    AMBA_MON_LISTEN_DSP_FLAG_s    Disp;
    AMBA_MON_LISTEN_DSP_FLAG_s    Vout;
    AMBA_MON_LISTEN_DSP_FLAG_s    VinTimeout;
    AMBA_MON_LISTEN_DSP_FLAG_s    Echo;
} AMBA_MON_LISTEN_DSP_EVENT_s;

typedef enum /*_AMBA_MON_LISTEN_DSP_STATE_e_*/ {
    DSP_STATE_IDLE = 0,
    DSP_STATE_START,
    DSP_STATE_TIMEOUT
} AMBA_MON_LISTEN_DSP_STATE_e;

typedef struct /*_AMBA_MON_LISTEN_VIN_STATE_s_*/ {
    UINT32 Msg;
} AMBA_MON_LISTEN_DSP_STATE_s;

#define AMBA_MON_NUM_RING_CHANNEL    8U

typedef struct /*_AMBA_MON_LISTEN_DSP_MSG_s_*/ {
    AMBA_DSP_RAW_DATA_RDY_s           RawInfo[AMBA_MON_NUM_VIN_CHANNEL][AMBA_MON_NUM_RING_CHANNEL];
    AMBA_DSP_YUV_DATA_RDY_EXTEND_s    YuvInfo[AMBA_MON_NUM_FOV_CHANNEL][AMBA_MON_NUM_RING_CHANNEL];
    AMBA_DSP_YUV_DATA_RDY_EXTEND_s    YuvAltInfo[AMBA_MON_NUM_FOV_CHANNEL][AMBA_MON_NUM_RING_CHANNEL];
    AMBA_DSP_YUV_DATA_RDY_EXTEND_s    DispInfo[AMBA_MON_NUM_VOUT_CHANNEL][AMBA_MON_NUM_RING_CHANNEL];
} AMBA_MON_LISTEN_DSP_MSG_s;

UINT32 AmbaMonListenDsp_Create(UINT32 Priority, UINT32 CoreInclusion);
UINT32 AmbaMonListenDsp_Delete(void);
UINT32 AmbaMonListenDsp_Active(void);
UINT32 AmbaMonListenDsp_Inactive(void);
UINT32 AmbaMonListenDsp_IdleWait(void);

#endif  /* AMBA_MONITOR_LISTEN_DSP_H */
