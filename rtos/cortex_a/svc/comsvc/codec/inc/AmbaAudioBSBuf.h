/**
 *  @file AmbaAudioBSBuf.h
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
 *  @details audio bitstream buffer
 *
 */

#ifndef AMBA_AUDIO_BSBUF_H
#define AMBA_AUDIO_BSBUF_H

#include "AmbaFS.h"

#define AU_MUX_DESC_NUM                 16U
#define AU_DEMUX_DESC_NUM               128U
#define AMBA_AUDIO_MAX_MUX_NUM          8U
#define AMBA_AUDIO_MAX_DEMUX_NUM        2U
#define AMBA_AUDIO_MUX_STACK_SIZE       0x2000U
#define AMBA_AUDIO_DEMUX_STACK_SIZE     0x2000U
#define AMBA_AUDIO_MUX_BUFFER_SIZE      8192U
#define AMBA_AUDIO_DEMUX_BUFFER_SIZE    8192U
#define AMBA_AUDIO_DEMUX_BS_BUFFER_SIZE (ADEC_FRAME_SIZE * ADEC_CH_NUM * 4U * AU_DEMUX_DESC_NUM)  //8192U*AU_DEMUX_DESC_NUM (AU_DEMUX_DESC_NUM entry and each entry 8192 size)

#define AU_MUXER_NA_ID    0xFFFFFFFFU
#define AU_DEMUXER_NA_ID  0xFFFFFFFFU

#define AMUX_OK         0x0000U
#define AMUX_ERR_0000   0x0001U
#define AMUX_ERR_0001   0x0002U
#define AMUX_ERR_0003   0x0003U
#define AMUX_ERR_0004   0x0004U

#define ADEMUX_OK          0x0000U
#define ADEMUX_ERR_0000    0x0001U
#define ADEMUX_ERR_0001    0x0002U
#define ADEMUX_ERR_0003    0x0003U
#define ADEMUX_ERR_0004    0x0004U

#define AU_MAX_CH           8U

#define AUDIO_MUX_FLG_GET_LOF        0x00000001U

typedef struct {
    char       AudRomFileName[30];
    UINT32     AudROMSize;
    UINT32     AudROMRdCnt;
    UINT32     IsAudROMReload;
} AMBA_AUD_FROM_ROM_s;

typedef struct {
    UINT32                  *pDecHdlr;

    AMBA_KAL_SEMAPHORE_t    RemainSem;          /* counting semaphore for the Audio Data Blocks in the Buffer: init value = 0 */
    AMBA_KAL_SEMAPHORE_t    RoomSem;            /* counting semaphore for the Remained Rooms in the Buffer: init value = Max Buf Size */
    UINT32                  ReadIdx;
    UINT32                  WriteIdx;  /* Read/Write index (pointers) */
    AMBA_ADEC_AUDIO_DESC_s  Desc[AU_DEMUX_DESC_NUM];     /* pointer to the Audio Buffer Descriptor */

    UINT8                   *pBsBuf;
    UINT32                  BsSize;
    UINT32                  BsCurSize;
    AMBA_AUD_FROM_ROM_s     AudFromROMCtrl;
} AMBA_AUDIO_DEMUX_CTRL_s;


typedef struct {
    UINT32                  MuxIndex;
    UINT32                  *pEncHdlr;
    AMBA_FS_FILE            *pFile;
    INT8                    MuxStack[AMBA_AUDIO_MUX_STACK_SIZE];
    AMBA_KAL_TASK_t         MuxTask;
    AMBA_KAL_EVENT_FLAG_t   EventFlag;
    UINT32                  CurrentBufferSize;
    INT8                    WriteBuffer[AMBA_AUDIO_MUX_BUFFER_SIZE];

    AMBA_KAL_SEMAPHORE_t    RemainSem;          /* counting semaphore for the Audio Data Blocks in the Buffer: init value = 0 */
    AMBA_KAL_SEMAPHORE_t    RoomSem;            /* counting semaphore for the Remained Rooms in the Buffer: init value = Max Buf Size */
    UINT32                  ReadIdx;
    UINT32                  WriteIdx;  /* Read/Write index (pointers) */
    AMBA_AENC_AUDIO_DESC_s  Desc[AU_MUX_DESC_NUM];     /* pointer to the Audio Buffer Descriptor */

    UINT32                  *pBsBuf;
    UINT32                  BsSize;
    UINT32                  BsCurSize;
} AMBA_AUDIO_MUX_CTRL_s;

extern AMBA_AUDIO_MUX_CTRL_s   AudioMuxHdlr[AMBA_AUDIO_MAX_MUX_NUM] __attribute__((section(".bss.noinit")));
extern AMBA_AUDIO_DEMUX_CTRL_s AudioDemuxHdlr[AMBA_AUDIO_MAX_DEMUX_NUM] __attribute__((section(".bss.noinit")));

AMBA_AENC_AUDIO_DESC_s *Audio_MuxGetReadPointer(AMBA_AUDIO_MUX_CTRL_s *pHdlr);
UINT32 Audio_MuxUpdateReadPointer(AMBA_AUDIO_MUX_CTRL_s *pHdlr);
AMBA_AENC_AUDIO_DESC_s *Audio_EncGetWritePointer(AMBA_AUDIO_MUX_CTRL_s *pHdlr);
UINT32 Audio_EncUpdateWritePointer(AMBA_AUDIO_MUX_CTRL_s *pHdlr);

void AmbaAudio_MuxIdHdrInit(void);
UINT32 AmbaAudio_MuxCreate(UINT32 Priority, UINT32 *pBsBuf, UINT32 BsSize);
UINT32 AmbaAudio_MuxDelete(UINT32 MuxIndex);
UINT32 AmbaAudio_MuxTaskSetUp(UINT32 MuxIndex, const char *pName);
UINT32 AmbaAudio_MuxTaskStart(UINT32 MuxIndex);
UINT32 AmbaAudio_MuxWaitLof(UINT32 MuxIndex);

void                     AmbaAudio_FeedAudio(AMBA_AUDIO_DEMUX_CTRL_s *pHdlr);
void                     AmbaAudio_FeedROMAudio(AMBA_AUDIO_DEMUX_CTRL_s *pHdlr, UINT32 NvmID);
UINT32                   AmbaAudio_DeMuxCreate(UINT32 Source, ULONG MPly);
UINT32                   AmbaAudio_DeMuxDelete(UINT32 DemuxIndex);

AMBA_AUDIO_DEMUX_CTRL_s* AmbaAudio_BsBufDecInit(UINT32 *pAdecHdlr, UINT8 *pBsBuf, UINT32 BsSize, UINT32 DemuxIndex, UINT32 Source);
AMBA_ADEC_AUDIO_DESC_s*  AmbaAudio_BsBufDecGetRdPtr(AMBA_AUDIO_DEMUX_CTRL_s *pHdlr);
UINT32                   AmbaAudio_BsBufDecUptRdPtr(AMBA_AUDIO_DEMUX_CTRL_s *pHdlr);
AMBA_ADEC_AUDIO_DESC_s*  AmbaAudio_BsBufDemuxGetWrPtr(AMBA_AUDIO_DEMUX_CTRL_s *pHdlr);
UINT32                   AmbaAudio_BsBufDemuxUptWrPtr(AMBA_AUDIO_DEMUX_CTRL_s *pHdlr);
UINT32                   AmbaAudio_GetDecBsBufSize(void);

#endif /* AMBA_AUDIO_BSBUF_H */
