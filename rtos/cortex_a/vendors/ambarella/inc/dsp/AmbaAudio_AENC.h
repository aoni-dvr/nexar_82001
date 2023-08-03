/**
 *  @file AmbaAudio_AENC.h
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
 *  @details Audio encoder process header.
 *
 */

#ifndef AMBA_AUDIO_AENC_H
#define AMBA_AUDIO_AENC_H

#define AENC_OK         ERR_NONE
#define AENC_ERR_0000   (AENC_ERR_BASE          )   /* Input null error */
#define AENC_ERR_0001   (AENC_ERR_BASE + 0X0001U)   /* Invalid argument, Out of range error(memory, index, ...) */
#define AENC_ERR_0002   (AENC_ERR_BASE + 0X0002U)   /* Buffer alignment error */
#define AENC_ERR_0003   (AENC_ERR_BASE + 0X0003U)   /* Mutex error */
#define AENC_ERR_0004   (AENC_ERR_BASE + 0X0004U)   /* OS resource error, Unexpected error */
#define AENC_ERR_0005   (AENC_ERR_BASE + 0X0005U)   /* Callback error */

/* print module */
#define AENC_MODULE_ID        ((UINT16)(AENC_ERR_BASE >> 16U))

typedef struct {
    UINT32   ChannelNum;        /* Channel number of the audio encoder */
    UINT32   FrameSize;         /* Frame size of the encoder in samples for one channel */
    UINT32   SampleResolution;  /* Resolution of the encoder in bits */
    UINT32   SampleFreq;        /* Sample Frequency */
    UINT32   IoNodeNum;         /* Number of I/O nodes */
    UINT32   PlugInLibSelfSize; /* Required library buffer size of the plug-in encoder */
    UINT32   CachedBufSize;     /* Required cached buffer size */
} AMBA_AUDIO_ENC_CREATE_INFO_s;

typedef struct {
    UINT32  *pHead;              /* Current memory head */
    UINT32  CurrentSize;         /* Current used size */
    UINT32  MaxSize;             /* Max memory size */
} AMBA_AENC_MEM_INFO_s;

typedef struct {
    UINT32  *pHead;              /* Memory head */
    UINT32   MaxSize;            /* Maximum memory size */
} AMBA_AENC_BUF_INFO_s;

typedef enum {
    /* AENC Event */
    AMBA_AENC_EVENT_ID_ENCODE_STOP      = 0U,
    AMBA_AENC_EVENT_ID_ENCODE_ONE_FRAME = 1U,
    AMBA_AENC_NUM_EVENT                 = 2U
} AMBA_AENC_EVENT_ID_e;

#define AMBA_AUDIO_ENC_FLG_STOP         0x00000001U
#define AMBA_AUDIO_ENC_FLG_FADEOUT_DONE 0x00000002U

#define AENC_FADE_GAIN_0dB  0x7FFFFFFFUL
#define AENC_FADE_GAIN_MIN  0x00000000UL
#define AENC_FADE_STATE_FADEIN      0U
#define AENC_FADE_STATE_FADEOUT     1U
#define AENC_FADE_STATE_FADEIDLE    2U

typedef enum {
    AENC_IONODE_PAUSE      = 0,
    AENC_IONODE_OPERATION  = 1
} AMBA_AENC_IONODE_STATUS_enum;

typedef enum {
    AENC_IONODE_BLOCKING    = 0,
    AENC_IONODE_NONBLOCKING = 1
} AMBA_AENC_IONODE_CRITERIA_enum;

typedef struct {
    UINT32 *pPcmBuf;
    UINT32 Size;
    UINT64 AudioTicks;
    UINT32 Lof;
} AMBA_AENC_DATA_INFO_s;

typedef UINT32 (*AMBA_AENC_GET_PCM_f)(AMBA_AENC_DATA_INFO_s *pInfo);
typedef UINT32 (*AMBA_AENC_GET_BUF_REMAIN_SIZE_f)(UINT32 *pSize);
typedef UINT32 (*AMBA_AENC_EVENT_HANDLER_f)(const void *pEventData);

typedef struct {
    UINT32  *pHdlr;
    UINT32  Eos;
    UINT32  DataSize;
    UINT8   *pBufAddr;
    UINT64  AudioTicks;
    UINT64  EncodedSamples;
} AMBA_AENC_AUDIO_DESC_s;

typedef struct {
    UINT32 MaxNumHandler;                       /* maximum number of Handlers */
    AMBA_AENC_EVENT_HANDLER_f *pEventHandler;   /* pointer to the Event Handlers */
} AMBA_AENC_EVENT_HANDLER_CTRL_s;

typedef struct {
    AMBA_AENC_GET_PCM_f             GetPcm;
    AMBA_AENC_GET_BUF_REMAIN_SIZE_f GetSize;
} AMBA_AENC_CB_HDLR_s;

typedef struct {
    UINT32  NodeStatus;
    UINT32  NodeCriteria;
    UINT32  UseTickNum;
    AMBA_AENC_CB_HDLR_s  *pCbHdlr;
} AMBA_AENC_IO_NODE_s;

typedef struct AMBA_AENC_PLUGIN_ENC_CS {
    UINT32  *pSrc;
    UINT8   *pDst;
    UINT32  FrameSize;
    UINT32  ChNum;
    UINT32  Resolution;
    UINT32  MaxBsSize;
    UINT32  SelfSize;
    void    *pSelf;
    UINT32  (*pSetUp_f) (const struct AMBA_AENC_PLUGIN_ENC_CS *pPlugInCs);
    UINT32  (*pProc_f) (struct AMBA_AENC_PLUGIN_ENC_CS *pPlugInCs);
    UINT32  EncodedBytes;
    UINT32  Update;
} AMBA_AENC_PLUGIN_ENC_CS_s;

typedef struct {
    UINT32 FadeInTime;
    UINT32 FadeOutTime;
    UINT32 SmpCnt;
    UINT32 Pad;
    UINT32 Step;
    UINT32 CurrGain;
    UINT32 DestGain;
    UINT32 StartGain;
    UINT32 State;
    AMBA_KAL_MUTEX_t Mutex;
} AMBA_AENC_FADE_s;

typedef struct {
    /* Task related */
    UINT32  ErrorCode;
    AMBA_KAL_EVENT_FLAG_t           Flag;
    AMBA_KAL_MUTEX_t                EventMutex;
    AMBA_AENC_EVENT_HANDLER_CTRL_s  *pEventHandlerCtrl;

    /* Input Chain */
    UINT32              IoNodeNum;  /* Number of I/O nodes */
    AMBA_KAL_MUTEX_t    IoNodeMutex;
    AMBA_AENC_IO_NODE_s *pIoNode;

    /* AENC related */
    UINT8   *pBsAddr;
    UINT32  BsBufSize;
    AMBA_AENC_PLUGIN_ENC_CS_s EncCs;
    void    *pSelf;
    UINT32  *pPcmBufRptr;
    UINT32  *pMixPcmBuf;
    UINT32  SrcPcmBufSize;
    UINT32  PlugInPcmSize;
    UINT32  PlugInPcmRes;
    UINT32  PlugInPcmCh;
    UINT64  AudioTicks;

    UINT8   *pBitBufWptr;
    UINT8   *pBitTempBuf;
    UINT32  EncodedByte;
    UINT32  CurrentByte;
    UINT32  InputCpSize;

    UINT64  FrameCount;
    AMBA_AENC_AUDIO_DESC_s Desc;

    UINT32  Stop;

    UINT32  SampleFreq;
    UINT32  SampleResolution;
    UINT32  ChannelNum;
    UINT32  FrameSize;
    AMBA_AENC_FADE_s FadeCtrl;

} AMBA_AENC_HDLR;

/*-----------------------------------------------------------------------------------------------*\
 * AENC related APIs Definition
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaAENC_QueryBufSize (AMBA_AUDIO_ENC_CREATE_INFO_s *pInfo);
UINT32 AmbaAENC_CreateResource(const AMBA_AUDIO_ENC_CREATE_INFO_s *pInfo, const AMBA_AENC_BUF_INFO_s *pCachedInfo, UINT32 **pHdlr);
UINT32 AmbaAENC_DeleteResource(const UINT32 *pHdlr);
UINT32 AmbaAENC_ProcEnc(UINT32 *pHdlr);
UINT32 AmbaAENC_Start(const UINT32 *pHdlr, UINT32 FadeInTime);
UINT32 AmbaAENC_Stop(const UINT32 *pHdlr, UINT32 FadeOutTime);
UINT32 AmbaAENC_RegisterCallBackFunc(const UINT32 *pHdlr, AMBA_AENC_CB_HDLR_s *pCbHdlr, UINT32 UseTickNum, UINT32 Criteria);
UINT32 AmbaAENC_DeRegisterCallBackFunc(const UINT32 *pHdlr, const AMBA_AENC_CB_HDLR_s *pCbHdlr);
UINT32 AmbaAENC_OpenIoNode(const UINT32 *pHdlr, const AMBA_AENC_CB_HDLR_s *pCbHdlr);
UINT32 AmbaAENC_CloseIoNode(const UINT32 *pHdlr, const AMBA_AENC_CB_HDLR_s *pCbHdlr);
UINT32 AmbaAENC_SetUpBsBuffer(const UINT32 *pHdlr, UINT32 *pBsAddr, UINT32 BsBufSize);
UINT32 AmbaAENC_ConfigEventHdlr(const UINT32 *pHdlr, UINT32 EventId, UINT32 MaxNumHandler, AMBA_AENC_EVENT_HANDLER_f *pEventHandlers);
UINT32 AmbaAENC_RegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId, AMBA_AENC_EVENT_HANDLER_f EventHandler);
UINT32 AmbaAENC_DeRegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId, const AMBA_AENC_EVENT_HANDLER_f EventHandler);
UINT32 AmbaAENC_GetPlugInEncConfig(const UINT32 *pHdlr, void *pPlugInConfig);
UINT32 AmbaAENC_InstallPlugInEnc(const UINT32 *pHdlr, const AMBA_AENC_PLUGIN_ENC_CS_s *pPlugInCs);
UINT32 AmbaAENC_UpdatePlugInEnc(const UINT32 *pHdlr, const void *pPlugInConfig);

#endif /* AMBA_AUDIO_AENC_H */
