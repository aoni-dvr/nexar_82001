/**
 *  @file SvcCvLogger.h
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
 *  @details Header of SvcCvLogger
 *
 */

#ifndef SVC_CV_LOGGER_H
#define SVC_CV_LOGGER_H
#include "RefFlow_Common.h"

/* Note */
/* Trunk header: RF_LOG_HEADER_V0_s. Defined in RefFlow_Common.h */

#define SVC_CV_LOGGER_MAX_HDLR                  (6U)
#define SVC_CV_LOGGER_FILE_NAME_LENGTH          (64U)

#define SVC_CV_LOGGER_HEADER_SIZE               (sizeof(RF_LOG_HEADER_V0_s) - 4U) /* Exclude RF_LOG_HEADER_V0_s.pDataAddr */

/* Internal buffer */
#define SVC_CV_LOGGER_BUFFER_TYPES              (4U)
#define SVC_CV_LOGGER_FHEAD_INDEX_BUFFER        (0U)    /* File head index buffer */
#define SVC_CV_LOGGER_FHEAD_TRUNK_BUFFER        (1U)    /* File head trunk buffer. It will be repeated at the beginning of each file */
#define SVC_CV_LOGGER_LOG_INDEX_BUFFER          (2U)    /* Log index buffer */
#define SVC_CV_LOGGER_LOG_TRUNK_BUFFER          (3U)    /* Log trunk buffer. Store run-time log. */

typedef struct {
    UINT8                   MaxLogger;
    ULONG                   MemBase;
    UINT32                  MemSize;
} SVC_CV_LOGGER_INIT_CFG_s;

typedef struct {
    const char              *pFileName;
} SVC_CV_LOGGER_CFG_s;

typedef struct {
    ULONG                   Base;
    UINT32                  Size;
    ULONG                   End;      /* (End of buffer + 1) */
    ULONG                   Wp;
    ULONG                   Rp;
} SVC_CV_LOGGER_BUFFER_s;

typedef struct {
    ULONG                   Addr;
    UINT32                  Size;
    UINT32                  Flag;     /* Internal use */
} SVC_CV_LOGGER_WRITE_BUFFER_s;

typedef struct {
    AMBA_VFS_FILE_s         VfsFile;
} SVC_CV_LOGGER_FILE_DESC_s;

typedef struct {
    UINT32                  Started :1;
    UINT32                  HeadFlushed :1;
    UINT32                  Reserved :30;
    UINT8                   ID;
    UINT8                   FlushReq;
    AMBA_KAL_MUTEX_t        ApiMutex;
    AMBA_KAL_EVENT_FLAG_t   EventFlag;
    char                    FileName[SVC_CV_LOGGER_FILE_NAME_LENGTH];
    SVC_CV_LOGGER_FILE_DESC_s FileDesc;
    SVC_CV_LOGGER_BUFFER_s  Buf[SVC_CV_LOGGER_BUFFER_TYPES];
    UINT32                  LogTrunkBufTh;
} SVC_CV_LOGGER_HDLR_s;

typedef struct{
    RF_LOG_SEG_BUF_HEADER_s SegHeader;
    void *pBitmap;
} SVC_CV_LOGGER_SEG_s;


UINT32 SvcCvLogger_Init(SVC_CV_LOGGER_INIT_CFG_s *pCfg);

UINT32 SvcCvLogger_Create(SVC_CV_LOGGER_CFG_s *pCfg, SVC_CV_LOGGER_HDLR_s *pHdlr);
UINT32 SvcCvLogger_Delete(SVC_CV_LOGGER_HDLR_s *pHdlr);

/* Flag */
#define SVC_CV_LOGGER_FLAG_LOG_TRUNK            (0U)    /* Write to log trunk */
#define SVC_CV_LOGGER_FLAG_FILE_HEAD            (1U)    /* Write to file head trunk */

UINT32 SvcCvLogger_TrunkBufferGet(SVC_CV_LOGGER_HDLR_s *pHdlr, UINT32 ReqSize, UINT32 Flag, SVC_CV_LOGGER_WRITE_BUFFER_s *pWrBuf);
UINT32 SvcCvLogger_TrunkBufferUpdate(SVC_CV_LOGGER_HDLR_s *pHdlr, SVC_CV_LOGGER_WRITE_BUFFER_s *pWrBuf, UINT32 WroteSize);

/* CtrlType */
#define SVC_CV_LOGGER_CTRL_VIDEO_START          (0U)
#define SVC_CV_LOGGER_CTRL_VIDEO_CLOSE          (1U)

typedef struct {
    const char              *pFileName;
    UINT32                  IsLastFile;
    UINT64                  VidCapPts;
} SVC_CV_LOGGER_VIDEO_STATUS_s;

UINT32 SvcCvLogger_Ctrl(SVC_CV_LOGGER_HDLR_s *pHdlr, UINT32 CtrlType, void *pInfo);

void SvcCvLogger_DebugEnable(UINT32 DebugEnable);
UINT32 SvcCvLogger_RunLengthEncode(const UINT8* pBufIn, UINT32 Width, UINT32 Height, UINT32 Pitch, UINT8* pBufOut, UINT32 BufOutSize, UINT32* UsedLength);

#endif /* SVC_CV_LOGGER_H */
