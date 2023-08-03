/**
 *  @file AmbaAudio_AOUT.h
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
 *  @details Audio output process header.
 *
 */

#ifndef AMBA_AUDIO_OUTPUT_H
#define AMBA_AUDIO_OUTPUT_H

#ifdef CONFIG_QNX
#include <errno.h>
#include <fcntl.h>
#include <gulliver.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/termio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/slogcodes.h>
#include <sys/slog2.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <signal.h>
#include <pthread.h>
#include <sys/asoundlib.h>
#endif

#define AMBA_AOUT_FLG_STOP          0x00000001U
#define AMBA_AOUT_FLG_EOCSET        0x00000002U
#define AMBA_AOUT_FLG_NOWOKIOSET    0x00000004U
#define AMBA_AOUT_FLG_PAUSE         0x00000008U
#define AMBA_AOUT_FLG_STOPWITHLOF   0x00000010U

/* AOUT error definition */
#define AOUT_OK         (OK)
#define AOUT_ERR_0000   (AOUT_ERR_BASE          )   /* Invalid argument */
#define AOUT_ERR_0001   (AOUT_ERR_BASE + 0X0001U)   /* Input null error */
#define AOUT_ERR_0002   (AOUT_ERR_BASE + 0X0002U)   /* 4 byte alignment fail */
#define AOUT_ERR_0003   (AOUT_ERR_BASE + 0X0003U)   /* 8 byte alignment fail */
#define AOUT_ERR_0004   (AOUT_ERR_BASE + 0X0004U)   /* Out of range error(memory, index, ...) */
#define AOUT_ERR_0005   (AOUT_ERR_BASE + 0X0005U)   /* Mutex error */
#define AOUT_ERR_0006   (AOUT_ERR_BASE + 0X0006U)   /* Unexpected error */
#define AOUT_ERR_0007   (AOUT_ERR_BASE + 0X0007U)   /* DMA related error(allocte, release,...) */
#define AOUT_ERR_0008   (AOUT_ERR_BASE + 0X0008U)   /* Callback error */

/* print module */
#define AOUT_MODULE_ID              ((UINT16)(AOUT_ERR_BASE >> 16U))

typedef enum {
    AOUT_I2S_0   = 0U,
    AOUT_I2S_1   = 1U
} AMBA_AOUT_IO_IDX_e;

typedef struct {
    UINT32   HwIndex;            /* Hardware Index number (I2S, SPI, and, etc.) */
    UINT32   ChannelNum;         /* Channel number of the audio output data */
    UINT32   DmaFrameSize;       /* DMA size in samples for one channel */
    UINT32   DmaDescNum;         /* DMA descriptor number of the DMA ring buffer */
    UINT32   IoNodeNum;          /* Number of I/O nodes */
    UINT32   CachedBufSize;      /* Required cached buffer size */
    UINT32   NonCachedBufSize;   /* Required non-cached buffer size */
} AMBA_AOUT_IO_CREATE_INFO_s;

typedef struct {
    UINT32  *pHead;              /* Current memory head */
    UINT32  CurrentSize;         /* Current used size */
    UINT32  MaxSize;             /* Max memory size */
} AMBA_AOUT_MEM_INFO_s;

typedef struct {
    UINT32  *pHead;              /* Memory head */
    UINT32   MaxSize;            /* Maximum memory size */
} AMBA_AOUT_BUF_INFO_s;

typedef enum {
    /* Output Event */
    AMBA_AOUT_EVENT_ID_DMA_STOP        = 0U,
    AMBA_AOUT_EVENT_ID_GET_LOF         = 1U,
    AMBA_AOUT_EVENT_ID_NO_WORK_IO      = 2U,
    AMBA_AOUT_EVENT_ID_DMA_UNDERFLOW   = 3U,
    AMBA_AOUT_EVENT_ID_DMA_OVERTIME    = 4U,
    AMBA_AOUT_NUM_EVENT                = 5U
} AMBA_AOUT_EVENT_ID_e;

typedef enum {
    AOUT_IONODE_PAUSE      = 0,
    AOUT_IONODE_OPERATION  = 1,
    AOUT_IONODE_DROP       = 2
} AMBA_AOUT_IONODE_STATUS_enum;

typedef enum {
    AOUT_CB_CUTIN   = 0,
    AOUT_CB_NORMAL  = 1
} AMBA_AOUT_CB_STATUS_enum;

/*typedef struct {
    UINT32 *pDataBuf;
    UINT32 Size;
    UINT32 Lof;
} AMBA_AOUT_DATA_INFO_s;*/

typedef UINT32 (*AMBA_AOUT_GET_PCM_f)(UINT32 **pPcmBuf, UINT32 Size, UINT32 *pLof);
typedef UINT32 (*AMBA_AOUT_GET_BUF_REMAIN_SIZE_f)(UINT32 *pSize);
typedef UINT32 (*AMBA_AOUT_EVENT_HANDLER_f)(const void *pEventData);

typedef struct {
    UINT32 MaxNumHandler;                       /* maximum number of Handlers */
    AMBA_AOUT_EVENT_HANDLER_f *pEventHandler;  /* pointer to the Event Handlers */
} AMBA_AOUT_EVENT_HANDLER_CTRL_s;

typedef struct {
    AMBA_AOUT_GET_PCM_f                GetPcm;     /* Memory head */
    AMBA_AOUT_GET_BUF_REMAIN_SIZE_f    GetSize;    /* Maximum memory size */
} AMBA_AOUT_CB_HDLR_s;

typedef struct {
    UINT32   NodeStatus;
    UINT32   BuffStatus;
    AMBA_AOUT_CB_HDLR_s  *pCbHdlr;
} AMBA_AOUT_IO_NODE_s;

typedef struct {
    UINT32               *pHdlr;     /* Resource handler */
    AMBA_AOUT_CB_HDLR_s  *pCbHdlr;   /* Callback handler */
} AMBA_AOUT_EVENT_INFO_s;

typedef struct {
    /* Task related */
    UINT32  ErrorCode;
    UINT32  HwIndex;
    UINT32  FrameIn;
    UINT32  FrameOut;
    AMBA_KAL_EVENT_FLAG_t            Flag;
    AMBA_KAL_MUTEX_t                 EventMutex;
    AMBA_AOUT_EVENT_HANDLER_CTRL_s   *pEventHandlerCtrl;
    UINT32  *pMixPcmBuf;

    /* Input Chain */
    UINT32               IoNodeNum;  /* Number of I/O nodes */
    AMBA_KAL_MUTEX_t     IoNodeMutex;
    AMBA_AOUT_IO_NODE_s  *pIoNode;

    /* DMA related */
    UINT32   DmaFrameSize;
    UINT32   ChanNum;
    UINT32   TxDmaChanNo;
    UINT32  *pDmaBuffer;
    UINT32  *pReport;
    UINT32   LastDescr;  /* Record lastest DMA done descriptor number */
    UINT32   NumDescr;     /* Number of descriptors */
    UINT32   ScanInputChainLof;
    UINT32   DmaStop;
#if defined(CONFIG_THREADX)
    AMBA_DMA_DESC_s  *pDesc;
#endif
#ifdef CONFIG_QNX
    snd_pcm_t *pcm_handle;
    snd_pcm_channel_params_t pp;
    INT32 aout_bsize;
#endif
} AMBA_AOUT_HDLR;

/*-----------------------------------------------------------------------------------------------*\
 * AOUT related APIs Definition
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaAOUT_QueryBufSize(AMBA_AOUT_IO_CREATE_INFO_s *pInfo);
UINT32 AmbaAOUT_CreateResource(const AMBA_AOUT_IO_CREATE_INFO_s *pInfo,
                               const AMBA_AOUT_BUF_INFO_s *pCachedInfo,
                               const AMBA_AOUT_BUF_INFO_s *pNonCachedInfo,
                               UINT32 **pHdlr);
UINT32 AmbaAOUT_DeleteResource(const UINT32 *pHdlr);
UINT32 AmbaAOUT_Prepare(const UINT32 *pHdlr);
UINT32 AmbaAOUT_ProcDMA(const UINT32 *pHdlr, UINT32 *pLof);
UINT32 AmbaAOUT_Stop(const UINT32 *pHdlr);
UINT32 AmbaAOUT_StopWithLof(const UINT32 *pHdlr);
UINT32 AmbaAOUT_Pause(const UINT32 *pHdlr);
UINT32 AmbaAOUT_RegisterCallBackFunc(const UINT32 *pHdlr, AMBA_AOUT_CB_HDLR_s *pCbHdlr);
UINT32 AmbaAOUT_DeRegisterCallBackFunc(const UINT32 *pHdlr, const AMBA_AOUT_CB_HDLR_s *pCbHdlr);
UINT32 AmbaAOUT_OpenIoNode(const UINT32 *pHdlr, const AMBA_AOUT_CB_HDLR_s *pCbHdlr);
UINT32 AmbaAOUT_CloseIoNode(const UINT32 *pHdlr, const AMBA_AOUT_CB_HDLR_s *pCbHdlr);
UINT32 AmbaAOUT_ConfigEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                                UINT32 MaxNumHandler,
                                AMBA_AOUT_EVENT_HANDLER_f *pEventHandlers);
UINT32 AmbaAOUT_RegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                                  AMBA_AOUT_EVENT_HANDLER_f EventHandler);
UINT32 AmbaAOUT_DeRegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                                    AMBA_AOUT_EVENT_HANDLER_f EventHandler);


#endif /* AMBA_AUDIO_OUTPUT_H */
