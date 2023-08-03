/**
 *  @file AmbaAudio_ADEC.h
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
 *  @details Audio decoder process header.
 *
 */

#ifndef AMBA_AUDIO_ADEC_H
#define AMBA_AUDIO_ADEC_H

#define AMBA_ADEC_FLG_PREFILL  0x00000010U
#define AMBA_ADEC_FLG_STOP     0x00000020U

/* ADEC error definition */
#define ADEC_OK         ERR_NONE
#define ADEC_ERR_0000   (ADEC_ERR_BASE          )   /* Input null error */
#define ADEC_ERR_0001   (ADEC_ERR_BASE + 0X0001U)   /* Invalid argument, Out of range error(memory, index, ...) */
#define ADEC_ERR_0002   (ADEC_ERR_BASE + 0X0002U)   /* Buffer alignment error */
#define ADEC_ERR_0003   (ADEC_ERR_BASE + 0X0003U)   /* Mutex error */
#define ADEC_ERR_0004   (ADEC_ERR_BASE + 0X0004U)   /* OS resource error, Unexpected error */
#define ADEC_ERR_0005   (ADEC_ERR_BASE + 0X0005U)   /* Callback error */

/* print module */
#define ADEC_MODULE_ID              ((UINT16)(ADEC_ERR_BASE >> 16U))

typedef struct {
    UINT32   SampleFreq;        /* Sampling frequency of the decoder */
    UINT32   ChannelNum;        /* Channel number of the audio decoder */
    UINT32   FrameSize;         /* Frame size of the decoder in samples for one channel */
    UINT32   SampleResolution;  /* Resolution of the decoder in bits */
    UINT32   IoNodeNum;         /* Number of I/O nodes */
    UINT32   PlugInLibSelfSize; /* Required library buffer size of the plug-in decoder */
    UINT32   NeededBytes;       /* Needed bytes for decoding one frame */
    UINT32   CachedBufSize;     /* Required cached buffer size */
} AMBA_AUDIO_DEC_CREATE_INFO_s;

typedef struct {
    UINT32  *pHead;              /* Current memory head */
    UINT32  CurrentSize;         /* Current used size */
    UINT32  MaxSize;             /* Max memory size */
} AMBA_ADEC_MEM_INFO_s;

typedef struct {
    UINT32  *pHead;              /* Memory head */
    UINT32   MaxSize;            /* Maximum memory size */
} AMBA_ADEC_BUF_INFO_s;

typedef enum {
    /* ADEC Event */
    AMBA_ADEC_EVENT_ID_DECODE_STOP            = 0U,
    AMBA_ADEC_EVENT_ID_DECODE_USE_ONE_FRAME   = 1U,
    AMBA_ADEC_EVENT_ID_DECODE_READY           = 2U,
    AMBA_ADEC_NUM_EVENT                       = 3U
} AMBA_ADEC_EVENT_ID_e;

typedef enum {
    ADEC_IONODE_PAUSE      = 0,
    ADEC_IONODE_OPERATION  = 1
} AMBA_ADEC_IONODE_STATUS_enum;

typedef enum {
    ADEC_IONODE_BLOCKING    = 0,
    ADEC_IONODE_NONBLOCKING = 1
} AMBA_ADEC_IONODE_CRITERIA_enum;

typedef UINT32 (*AMBA_ADEC_PUT_PCM_f)(const UINT32 *pHdlr, UINT32 *pPcmBuf, UINT32 Size, const UINT32 *pLof);
typedef UINT32 (*AMBA_ADEC_GET_BUF_ROOM_SIZE_f)(const UINT32 *pHdlr, UINT32 *pSize);
typedef UINT32 (*AMBA_ADEC_EVENT_HANDLER_f)(void *pEventData);

typedef struct {
    UINT32 MaxNumHandler;                       /* maximum number of Handlers */
    AMBA_ADEC_EVENT_HANDLER_f *pEventHandler;   /* pointer to the Event Handlers */
} AMBA_ADEC_EVENT_HANDLER_CTRL_s;

typedef struct {
    AMBA_ADEC_PUT_PCM_f             PutPcm;
    AMBA_ADEC_GET_BUF_ROOM_SIZE_f   GetSize;
} AMBA_ADEC_CB_HDLR_s;

typedef struct {
    UINT32  NodeStatus;
    UINT32  NodeCriteria;
    UINT32  UseTickNum;
    AMBA_ADEC_CB_HDLR_s  *pCbHdlr;
} AMBA_ADEC_IO_NODE_s;

typedef struct {
    UINT32  *pHdlr;
    UINT32  Eos;
    UINT32  DataSize;
    UINT8   *pBufAddr;
} AMBA_ADEC_AUDIO_DESC_s;

typedef struct AMBA_ADEC_PLUGIN_DEC_CS {
    UINT8   *pSrc;
    UINT32  *pDst;
    UINT32  FrameSize;
    UINT32  ChNum;
    UINT32  Resolution;
    UINT32  SelfSize;
    void    *pSelf;
    UINT32  PureAudioBufCurSize;
    UINT32  (*pSetUp_f) (const struct AMBA_ADEC_PLUGIN_DEC_CS *pPlugInCs);
    UINT32  (*pProc_f) (struct AMBA_ADEC_PLUGIN_DEC_CS *pPlugInCs);
    UINT32  ConsumedBytes;
    UINT32  Update;
} AMBA_ADEC_PLUGIN_DEC_CS_s;

typedef struct {
    /* Task related */
    UINT32  ErrorCode;
    AMBA_KAL_EVENT_FLAG_t           Flag;
    AMBA_KAL_MUTEX_t                EventMutex;
    AMBA_ADEC_EVENT_HANDLER_CTRL_s  *pEventHandlerCtrl;

    /* Output Chain */
    UINT32              IoNodeNum;  /* Number of I/O nodes */
    AMBA_KAL_MUTEX_t    IoNodeMutex;
    AMBA_ADEC_IO_NODE_s *pIoNode;

    /* ADEC related */
    UINT8                     *pDataBuf;
    UINT8                     *pBitBufRptr;
    UINT32                    *pPcmBufWptr;
    UINT32                    Eos;
    UINT32                    NeededBytes;
    UINT32                    ConsumedBytes;
    AMBA_ADEC_PLUGIN_DEC_CS_s DecCs;
    void                      *pSelf;
    UINT32                    Idling;
    UINT32                    Lof; /* Get BS Lof */
    UINT32                    ScanOutputChainLof;
    UINT32                    DecStop;

    /* Bistream buffer related */
    UINT8   *pBsRptr;
    UINT8   *pBsWptr;
    UINT8  *pBsAddr;
    UINT32  BsBufSize;
    UINT32  CurrentSize;
    UINT32  RptrRemainSize;
    UINT32  PreFillCount;

} AMBA_ADEC_HDLR;

/*-----------------------------------------------------------------------------------------------*\
 * ADEC related APIs Definition
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaADEC_QueryBufSize (AMBA_AUDIO_DEC_CREATE_INFO_s *pInfo);
UINT32 AmbaADEC_CreateResource(const AMBA_AUDIO_DEC_CREATE_INFO_s *pInfo, const AMBA_ADEC_BUF_INFO_s *pCachedInfo, UINT32 **pHdlr);
UINT32 AmbaADEC_DeleteResource(const UINT32 *pHdlr);
UINT32 AmbaADEC_ProcDec(const UINT32 *pHdlr, UINT32 PreFillFrameNum);
UINT32 AmbaADEC_Start(const UINT32 *pHdlr);
UINT32 AmbaADEC_Stop(const UINT32 *pHdlr, UINT32 Eos);
UINT32 AmbaADEC_RegisterCallBackFunc(const UINT32 *pHdlr, AMBA_ADEC_CB_HDLR_s *pCbHdlr);
UINT32 AmbaADEC_DeRegisterCallBackFunc(const UINT32 *pHdlr, const AMBA_ADEC_CB_HDLR_s *pCbHdlr);
UINT32 AmbaADEC_OpenIoNode(const UINT32 *pHdlr, const AMBA_ADEC_CB_HDLR_s *pCbHdlr);
UINT32 AmbaADEC_CloseIoNode(const UINT32 *pHdlr,const  AMBA_ADEC_CB_HDLR_s *pCbHdlr);
UINT32 AmbaADEC_SetUpBsBuffer(const UINT32 *pHdlr, UINT8 *pBsAddr, UINT32 BsBufSize);
UINT32 AmbaADEC_UpdateBsBufWp(const UINT32 *pHdlr, UINT8 *pBsAddr, UINT32 UpdateSize);
UINT32 AmbaADEC_ConfigEventHdlr(const UINT32 *pHdlr, UINT32 EventId, UINT32 MaxNumHandler, AMBA_ADEC_EVENT_HANDLER_f *pEventHandlers);
UINT32 AmbaADEC_RegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId, AMBA_ADEC_EVENT_HANDLER_f EventHandler);
UINT32 AmbaADEC_DeRegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId, AMBA_ADEC_EVENT_HANDLER_f EventHandler);
UINT32 AmbaADEC_InstallPlugInDec(const UINT32 *pHdlr, const AMBA_ADEC_PLUGIN_DEC_CS_s *pPlugInCs);
UINT32 AmbaADEC_GetPlugInDecConfig(const UINT32 *pHdlr, void *pPlugInSelf);
UINT32 AmbaADEC_UpdatePlugInDec(const UINT32 *pHdlr, const void *pPlugInSelf);

#endif /* AMBA_AUDIO_ADEC_H */
