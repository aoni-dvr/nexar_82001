/**
 *  @file RingBuffer.h
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
 *  @details Header File of Ring Buffer Module for DummyXxxEnc/DummyXxxDec
 *
 */
#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <AmbaFIFO.h>
#include <AmbaCFS.h>
#include "FormatLib.h"

#ifndef MP4_NHNT_S
#pragma pack(1)
typedef struct{
    char Signature[4];
    UINT32 version:8;
    UINT32 streamType:8;
    UINT32 objectTypeIndication:8;
    UINT32 reserved1:8;
    UINT32 reserved2:8;
    UINT32 bufferSizeDB:24;
    UINT32 avgBitRate;
    UINT32 maxBitRate;
    UINT32 timeStampResolution;
}MP4_NHNT_HEADER_s;
#pragma pack()

#pragma pack(1)
typedef struct{
    UINT32 dataSize:24;
    UINT32 randomAccessPoint:1;
    UINT32 auStartFlag:1;
    UINT32 auEndFlag:1;
    UINT32 reserved:1;//3->1
    UINT32 frameType:4;// 2->4
//    UINT32 mixFields;
    UINT32 fileOffset;
    UINT32 compositionTimeStamp;
    UINT32 decodingTimeStamp;
}MP4_NHNT_SAMPLE_HEADER_s;
#pragma pack()
#define MP4_NHNT_S
#endif

typedef struct {
    UINT8 *RingBufWp;
    UINT32 RingDescCount;
    UINT32 RingBufFreeSize;
    UINT32 RingBufSize;
} RING_BUFFER_INFO_s;

typedef struct {
    UINT8 *DescWp;
    UINT32 WriteSize;
    UINT64 Pts; /**< time stamp in ticks */
    UINT32 Type; /**< data type of the entry */
    UINT32 FileOffset;
} RING_BUFFER_DESC_s;

typedef UINT32 (*RING_BUF_CALLBACK_f)(const void *Hdlr, UINT32 Event, const void *Info);

typedef struct {
    AMBA_KAL_MUTEX_t Mutex;
    UINT8 *RingBufBase;
    UINT8 *RingBufLimit;
    UINT8 *RingBufWp;
    UINT8 *RingBufRp;
    UINT32 RingWDescNum;
    UINT32 RingRDescNum;
    UINT32 RingDescCount;
    UINT32 RingDescNum;
    UINT32 RingBufSize;
    UINT32 RingBufFreeSize;
    RING_BUFFER_DESC_s *Desc;
    RING_BUF_CALLBACK_f CbRingBuf;
} RING_BUFFER_s;

extern UINT32 RingBuf_GetRequiredBufSize(UINT32 RawBufSize, UINT32 MaxDesc);
extern UINT32 RingBuf_Init(RING_BUFFER_s *Buffer, UINT8 *BufferBase, UINT32 BufferSize, UINT32 MaxDesc, RING_BUF_CALLBACK_f CbRingBuf);
extern UINT32 RingBuf_Deinit(RING_BUFFER_s *Buffer);
extern UINT32 RingBuf_Reset(RING_BUFFER_s *Buffer);
extern UINT32 RingBuf_GetInfo(RING_BUFFER_s *Buffer, RING_BUFFER_INFO_s *Info);
extern UINT32 RingBuf_UpdateWp(RING_BUFFER_s *Buffer, const AMBA_FIFO_BITS_DESC_s *TmpDesc);
extern UINT32 RingBuf_CreateFrame(RING_BUFFER_s *Buffer, AMBA_CFS_FILE_s *File, UINT32 Size);
extern UINT32 RingBuf_DeleteFrame(RING_BUFFER_s *Buffer);
extern UINT32 RingBuf_MoveFrameToFile(RING_BUFFER_s *Buffer, AMBA_CFS_FILE_s *File);
extern UINT32 RingBuf_GetFrame(RING_BUFFER_s *Buffer, RING_BUFFER_DESC_s **Desc);

extern UINT32 SSPUT_TaskCreate(AMBA_KAL_TASK_t *pTask, char *pTaskName, UINT32 TaskPriority,
                              AMBA_KAL_TASK_ENTRY_f EntryFunction, UINT32 EntryArg,
                              void *pStackBase, UINT32 StackByteSize, UINT32 AutoStart);

#endif

