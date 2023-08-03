/**
 *  @file RefCmptDemuxerImpl.h
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
 *  @details Header file of reference compatible demuxer
 *
 */

#ifndef REF_CMPT_DEMUXER_H
#define REF_CMPT_DEMUXER_H

#include "AmbaKAL.h"
#include "SvcStream.h"

#define REF_CMPT_NHNT_DESC_ARRAY_SIZE (576000U)          ///< Size of NHNT array in a demuxer.

typedef struct {
    UINT32 nIdrInterval;
    UINT32 nTimeScale;
    UINT32 nTimePerFrame;
    UINT32 nN;
    UINT32 nM;
    UINT32 nVideoWidth;
    UINT32 nVideoHeight;
    UINT32 nInterlaced;
} REF_CMPT_FORMAT_USER_DATA_s;

typedef struct {
    UINT8 IsInit;                       ///< Whether the demux is initialized
    UINT8 IsOpened;                     ///< Whether the file is opened
    AMBA_KAL_MUTEX_t VideoDecFrmMutex;  ///< Mutex for reading and writing frames to fifo
    UINT8* AvcRawBuffer;                ///< Aligned buffer address of video raw data
    UINT8* AvcRawLimit;                 ///< Maximum address in raw buffer
    UINT8* AvcRawReadPoint;             ///< Read point in raw buffer
    UINT8* AvcRawWritePoint;            ///< Write point in raw buffer
    UINT32 VideoDecFrmCnt;              ///< How many frames to feed. A value of 0xFFFFFFFF indicates EOS.
    SVC_STREAM_HDLR_s *Mp4File;         ///< File system of h264 raw file (.h264)
    REF_CMPT_FORMAT_USER_DATA_s AvcUserData;     ///< Data from ".udta" file
    UINT32 AvcFileWidth;                ///< The width of frame
    UINT32 AvcFileHeight;               ///< The height of frame
    UINT8 NeedGOPHeader;                ///< If 1, demux need to put GOP header in raw buffer.
    UINT8 PutHeader;                    ///< If 1 and NeedGOPHeader is 1, notify demux to put GOP header in raw buffer.
    UINT8 IsWaitingSpace;               ///< Whether the demux is waiting for enough space in raw buffer to feed data
    UINT32 PreviousPts;                 ///< The last fed PTS
    UINT32 FrameSeqNum;                 ///< Sequential number of the next frame
    UINT8 IsFirstGop;                   ///< 1: No GOP header has been put. 0: More than 1 GOP header has been put.
    UINT8 CodingType;                   ///< 0: H264, 1: H265
    UINT8 IsPreFeeding;                 ///< 1: prefeeding, do not send bits fifo update 0: send bits fifo update
} REF_CMPT_DEMUX_HDLR_s;

typedef struct {
    UINT8* AvcRawBuffer;                ///< Aligned buffer address of video raw data
    UINT8* AvcRawLimit;                 ///< Maximum address in raw buffer
} REF_CMPT_DEMUX_CREATE_CFG_s;

/**
 * Video playback coding type
 */
#define REF_CMPT_DEMUX_PB_H264 0U
#define REF_CMPT_DEMUX_PB_H265 1U


extern UINT32 RefCmptDemuxer_Lock(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr, UINT32 Timeout);

extern UINT32 RefCmptDemuxer_UnLock(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr);

extern UINT32 RefCmptDemuxer_Init(void);

extern UINT32 RefCmptDemuxer_Create(const REF_CMPT_DEMUX_CREATE_CFG_s* DemuxCfg, REF_CMPT_DEMUX_HDLR_s** DemuxHdlr, SVC_STREAM_HDLR_s **pMp4File);

extern UINT32 RefCmptDemuxer_Open(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr,
                                         const char* pRawFn);

extern UINT32 RefCmptDemuxer_Start(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr,
                                      UINT32* startTime,
                                      UINT8 direction,
                                      UINT32 speed);

extern UINT32 RefCmptDemuxer_Stop(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr);

extern UINT32 RefCmptDemuxer_Close(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr);

extern UINT32 RefCmptDemuxer_Delete(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr);

extern UINT32 RefCmptDemuxer_Exit(void);

extern UINT32 RefCmptDemuxer_Feed(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr,
                                     UINT32 Req);

extern UINT32 RefCmptDemuxer_GetFreeSpace(const REF_CMPT_DEMUX_HDLR_s* DemuxHdlr, UINT32* Space);

extern UINT32 RefCmptDemuxer_SetGopConfig(REF_CMPT_DEMUX_HDLR_s* DemuxHdlr, UINT32 IdrInterval, UINT32 TimeScale, UINT32 TimePerFrame, UINT32 N, UINT32 M);

#endif /* REF_CMPT_DEMUXER_H */
