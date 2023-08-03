/**
 *  @file AmbaAudio_AIN.h
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
 *  @details Audio input process header.
 *
 */

#ifndef AMBA_AUDIO_INPUT_H
#define AMBA_AUDIO_INPUT_H

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
#include <limits.h>
#include <ctype.h>
#include <sys/asoundlib.h>
#endif

#define AMBA_AIN_FLG_STOP 0x00000001U

/* AIN error definition */
#define AIN_OK          (OK)
#define AIN_ERR_0000    (AIN_ERR_BASE          )    /* Invalid argument */
#define AIN_ERR_0001    (AIN_ERR_BASE + 0X0001U)    /* Input null error */
#define AIN_ERR_0002    (AIN_ERR_BASE + 0X0002U)    /* 4 byte alignment fail */
#define AIN_ERR_0003    (AIN_ERR_BASE + 0X0003U)    /* 8 byte alignment fail */
#define AIN_ERR_0004    (AIN_ERR_BASE + 0X0004U)    /* Out of range error(memory, index, ...) */
#define AIN_ERR_0005    (AIN_ERR_BASE + 0X0005U)    /* Mutex error */
#define AIN_ERR_0006    (AIN_ERR_BASE + 0X0006U)    /* Unexpected error */
#define AIN_ERR_0007    (AIN_ERR_BASE + 0X0007U)    /* DMA related error(allocte, release,...) */
#define AIN_ERR_0008    (AIN_ERR_BASE + 0X0008U)    /* Callback error */

/* print module */
#define AIN_MODULE_ID              ((UINT16)(AIN_ERR_BASE >> 16U))

typedef enum {
    AIN_I2S_0   = 0U,
    AIN_I2S_1   = 1U
} AMBA_AIN_IO_IDX_e;

typedef struct {
    UINT32   HwIndex;            /* Hardware Index number (I2S, SPI, and, etc.) */
    UINT32   ChannelNum;         /* Channel number of the audio input data */
    UINT32   DmaFrameSize;       /* DMA size in samples for one channel */
    UINT32   DmaDescNum;         /* DMA descriptor number of the DMA ring buffer */
    UINT32   IoNodeNum;          /* Number of I/O nodes */
    UINT32   CachedBufSize;      /* Required cached buffer size */
    UINT32   NonCachedBufSize;   /* Required non-cached buffer size */
} AMBA_AIN_IO_CREATE_INFO_s;

typedef struct {
    UINT32  *pHead;              /* Current memory head */
    UINT32  CurrentSize;         /* Current used size */
    UINT32  MaxSize;             /* Max memory size */
} AMBA_AIN_MEM_INFO_s;

typedef struct {
    UINT32  *pHead;              /* Memory head */
    UINT32   MaxSize;            /* Maximum memory size */
} AMBA_AIN_BUF_INFO_s;

typedef enum {
    /* Input Event */
    AMBA_AIN_EVENT_ID_DMA_STOP      = 0U,
    AMBA_AIN_EVENT_ID_DMA_OVERFLOW  = 1U,
    AMBA_AIN_EVENT_ID_DMA_OVERTIME  = 2U,
    AMBA_AIN_NUM_EVENT              = 3U
} AMBA_AIN_EVENT_ID_e;

typedef enum {
    AIN_IONODE_PAUSE      = 0,
    AIN_IONODE_OPERATION  = 1,
    AIN_IONODE_DROP       = 2
} AMBA_AIN_IONODE_STATUS_enum;

typedef enum {
    AIN_CB_CUTIN   = 0,
    AIN_CB_NORMAL  = 1
} AMBA_AIN_CB_STATUS_enum;

typedef struct {
    UINT32 *pPcmBuf;
    UINT32 Size;
    UINT64 AudioTicks;
    UINT32 Lof;
} AMBA_AIN_DATA_INFO_s;

typedef UINT32 (*AMBA_AIN_PUT_PCM_f)(const AMBA_AIN_DATA_INFO_s *pInfo);
typedef UINT32 (*AMBA_AIN_GET_BUF_ROOM_SIZE_f)(UINT32 *pSize);
typedef UINT32 (*AMBA_AIN_EVENT_HANDLER_f)(const void *pEventData);

typedef struct {
    UINT32 MaxNumHandler;                       /* maximum number of Handlers */
    AMBA_AIN_EVENT_HANDLER_f *pEventHandler;  /* pointer to the Event Handlers */
} AMBA_AIN_EVENT_HANDLER_CTRL_s;

typedef struct {
    AMBA_AIN_PUT_PCM_f              PutPcm;     /* Memory head */
    AMBA_AIN_GET_BUF_ROOM_SIZE_f    GetSize;    /* Maximum memory size */
} AMBA_AIN_CB_HDLR_s;

typedef struct {
    UINT32   NodeStatus;
    UINT32   BuffStatus;
    UINT32   FrameOut;
    AMBA_AIN_CB_HDLR_s  *pCbHdlr;
} AMBA_AIN_IO_NODE_s;

typedef struct {
    UINT32              *pHdlr;     /* Resource handler */
    AMBA_AIN_CB_HDLR_s  *pCbHdlr;   /* Callback handler */
} AMBA_AIN_EVENT_INFO_s;

typedef struct {
    /* Task related */
    UINT32  ErrorCode;
    UINT32  HwIndex;
    UINT32  FrameIn;
    AMBA_KAL_EVENT_FLAG_t           Flag;
    AMBA_KAL_MUTEX_t                EventMutex;
    AMBA_AIN_EVENT_HANDLER_CTRL_s   *pEventHandlerCtrl;
    UINT32  Msb64;
    UINT32  LastAudioTick;

    /* Output Chain */
    UINT32              IoNodeNum;  /* Number of I/O nodes */
    AMBA_KAL_MUTEX_t    IoNodeMutex;
    AMBA_AIN_IO_NODE_s  *pIoNode;

    /* DMA related */
    UINT32   DmaFrameSize;
    UINT32   ChanNum;
    UINT32   RxDmaChanNo;
    UINT32  *pDmaBuffer;
    UINT32  *pReport;
    UINT64  *pAudioTick;
    UINT32   LastDescr;  /* Record lastest DMA done descriptor number */
    UINT32   NumDescr;     /* Number of descriptors */
#if defined(CONFIG_THREADX)
    AMBA_DMA_DESC_s  *pDesc;
#endif
#ifdef CONFIG_QNX
    snd_pcm_t *pcm_handle;
    snd_pcm_channel_params_t pp;
    INT32 ain_bsize;
#endif
} AMBA_AIN_HDLR;

/*-----------------------------------------------------------------------------------------------*\
 * AIN related APIs Definition
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaAIN_QueryBufSize(AMBA_AIN_IO_CREATE_INFO_s *pInfo);
UINT32 AmbaAIN_CreateResource(const AMBA_AIN_IO_CREATE_INFO_s *pInfo,
                              const AMBA_AIN_BUF_INFO_s *pCachedInfo,
                              const AMBA_AIN_BUF_INFO_s *pNonCachedInfo,
                              UINT32 **pHdlr);
UINT32 AmbaAIN_DeleteResource(const UINT32 *pHdlr);
UINT32 AmbaAIN_ProcDMA(const UINT32 *pHdlr, UINT32 *pLof);
UINT32 AmbaAIN_Prepare(const UINT32 *pHdlr);
UINT32 AmbaAIN_Stop(const UINT32 *pHdlr);
UINT32 AmbaAIN_RegisterCallBackFunc(const UINT32 *pHdlr, AMBA_AIN_CB_HDLR_s *pCbHdlr);
UINT32 AmbaAIN_DeRegisterCallBackFunc(const UINT32 *pHdlr, const AMBA_AIN_CB_HDLR_s *pCbHdlr);
UINT32 AmbaAIN_OpenIoNode(const UINT32 *pHdlr, const AMBA_AIN_CB_HDLR_s *pCbHdlr);
UINT32 AmbaAIN_CloseIoNode(const UINT32 *pHdlr, const AMBA_AIN_CB_HDLR_s *pCbHdlr);
UINT32 AmbaAIN_ConfigEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                               UINT32 MaxNumHandler,
                               AMBA_AIN_EVENT_HANDLER_f *pEventHandlers);
UINT32 AmbaAIN_RegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                                 AMBA_AIN_EVENT_HANDLER_f EventHandler);
UINT32 AmbaAIN_DeRegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                                   AMBA_AIN_EVENT_HANDLER_f EventHandler);

#endif /* AMBA_AUDIO_INPUT_H */
