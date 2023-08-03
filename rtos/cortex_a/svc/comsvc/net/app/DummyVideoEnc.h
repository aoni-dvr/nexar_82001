/**
 *  @file DummyVideoEnc.h
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Header File of Dummy Video Encoder for Muxer Tests
 *
 */
#ifndef DUMMYVIDEOENC_H
#define DUMMYVIDEOENC_H

#include "RingBuffer.h"

#define DUMMY_VIDEO_ENC_PRIORITY    40

typedef struct {
    AMBA_CFS_FILE_s *IdxFile;
    AMBA_CFS_FILE_s *RawFile;
    UINT64 HdrFlen;
    UINT32 State;
    AMBA_KAL_TASK_t EncTask;
    UINT8 EncStack[0x8000];
    RING_BUFFER_s RingBuf;
    UINT8 *Buffer;
    UINT32 FrameCount;  // need mutex
    UINT32 TimeScale;
    UINT32 TimePerFrame;
    UINT16 SliceNum;
    AMBA_FIFO_HDLR_s *Fifo;
    char BsName[AMBA_CFS_MAX_FILENAME_LENGTH];
    char HdrName[AMBA_CFS_MAX_FILENAME_LENGTH];
} DUMMY_VIDEO_ENC_s;

extern UINT32 DummyVideoEnc_GetRequiredBufSize(UINT32 RawBufSize, UINT32 MaxQueuedFrame);

extern UINT32 DummyVideoEnc_Init(DUMMY_VIDEO_ENC_s *Encoder, const char *BsName, const char *HdrName, UINT8 *Buffer, UINT32 BufferSize, UINT32 MaxQueuedFrame, AMBA_FIFO_HDLR_s *Fifo, UINT8 Type);

extern UINT32 DummyVideoEnc_Deinit(DUMMY_VIDEO_ENC_s *Encoder);

extern UINT32 DummyVideoEnc_Start(DUMMY_VIDEO_ENC_s *Encoder);

extern UINT32 DummyVideoEnc_Stop(DUMMY_VIDEO_ENC_s *Encoder);

extern UINT32 DummyEnc_Sync(UINT64 Time);

#endif

