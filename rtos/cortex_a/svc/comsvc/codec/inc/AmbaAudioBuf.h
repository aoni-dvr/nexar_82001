/**
 *  @file AmbaAudioBuf.h
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
 *  @details audio buffer API
 *
 */

#ifndef AMBA_AUDIO_BUFFERS_H
#define AMBA_AUDIO_BUFFERS_H

#if defined(CONFIG_BUILD_COMMON_SERVICE_CODEC)
#else
#define CONFIG_AENC_NUM          (1U)
#endif

#define AMBA_AUDIO_MAX_BUF_NUM   ((UINT32)(CONFIG_AENC_NUM))

#define ABUF_OK         0x0000U
#define ABUF_ERR_0000   0x0001U
#define ABUF_ERR_0001   0x0002U
#define ABUF_ERR_0002   0x0003U
#define ABUF_ERR_0003   0x0004U

#define AU_SSP_BUF_DESC_NUM     32U

#define AU_SSP_BUF_FRAME_SIZE   1024U
#define AU_SSP_BUF_CH_NUM       2U
#define AU_SSP_BUF_CHUNK_SIZE   (AU_SSP_BUF_FRAME_SIZE * AU_SSP_BUF_CH_NUM * 4U)

#define AU_BUF_NA_ID  0xFFFFFFFFU

typedef struct {
    UINT32 *pDataBuf;
    UINT32 Size;
    UINT32 Lof;
} AMBA_ADEC_DATA_INFO_s;

typedef struct {
    UINT32 *pDataBuf;
    UINT32 Size;
    UINT32 Lof;
} AMBA_AOUT_DATA_INFO_s;

typedef struct {
    UINT32                  BufIndex;
    AMBA_KAL_SEMAPHORE_t    RemainSem;          /* counting semaphore for the Audio Data Blocks in the Buffer: init value = 0 */
    AMBA_KAL_SEMAPHORE_t    RoomSem;            /* counting semaphore for the Remained Rooms in the Buffer: init value = Max Buf Size */
    UINT32                  ReadIdx;
    UINT32                  WriteIdx;  /* Read/Write index (pointers) */
    AMBA_AOUT_DATA_INFO_s   Desc[AU_SSP_BUF_DESC_NUM];     /* pointer to the Audio Buffer Descriptor */
    UINT32                  Buf[AU_SSP_BUF_DESC_NUM][AU_SSP_BUF_CHUNK_SIZE / 4U];
} AMBA_AUDIO_DEC_BUF_CTRL_s;

typedef struct {
    UINT32                  BufIndex;
    AMBA_KAL_SEMAPHORE_t    RemainSem;          /* counting semaphore for the Audio Data Blocks in the Buffer: init value = 0 */
    AMBA_KAL_SEMAPHORE_t    RoomSem;            /* counting semaphore for the Remained Rooms in the Buffer: init value = Max Buf Size */
    UINT32                  ReadIdx;
    UINT32                  WriteIdx;  /* Read/Write index (pointers) */
    AMBA_AENC_DATA_INFO_s   Desc[AU_SSP_BUF_DESC_NUM];     /* pointer to the Audio Buffer Descriptor */
    UINT32                  Buf[AU_SSP_BUF_DESC_NUM + 1U][AU_SSP_BUF_CHUNK_SIZE / 4U]; /* +1 for FC temp buffer at ring buffer end */
    AMBA_AENC_FLOW_AU_EFX_INFO_s  EfxIn;
    AMBA_AENC_FLOW_AU_EFX_INFO_s  EfxOut;
} AMBA_AUDIO_ENC_BUF_CTRL_s;

extern AMBA_AUDIO_ENC_BUF_CTRL_s AudioEncBufHdlr[AMBA_AUDIO_MAX_BUF_NUM];
extern AMBA_AUDIO_DEC_BUF_CTRL_s AudioDecBufHdlr[AMBA_AUDIO_MAX_BUF_NUM];

void   AmbaAudio_EncBufIdHdrInit(void);
UINT32 AmbaAudio_EncBufCreate(void);
UINT32 AmbaAudio_EncBufReset(UINT32 BufIndex);
UINT32 AmbaAudio_EncBufDelete(UINT32 BufIndex);
AMBA_AENC_DATA_INFO_s *AmbaAudio_EncBufGetReadPointer(AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr);
UINT32 AmbaAudio_EncBufUpdateReadPointer(AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr);
AMBA_AIN_DATA_INFO_s *AmbaAudio_EncBufGetWritePointer(AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr);
UINT32 AmbaAudio_EncBufUpdateWritePointer(AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr);
UINT32 AmbaAudio_EncBufPeekRoomSize(AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr, UINT32 *pSize);
UINT32 AmbaAudio_EncBufPeekRemainSize(AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr, UINT32 *pSize);

void   AmbaAudio_DecBufIdHdrInit(void);
UINT32 AmbaAudio_DecBufCreate(void);
UINT32 AmbaAudio_DecBufDelete(UINT32 BufIndex);
AMBA_AOUT_DATA_INFO_s *AmbaAudio_DecBufGetReadPointer(AMBA_AUDIO_DEC_BUF_CTRL_s *pHdlr);
UINT32 AmbaAudio_DecBufUpdateReadPointer(AMBA_AUDIO_DEC_BUF_CTRL_s *pHdlr);
AMBA_ADEC_DATA_INFO_s *AmbaAudio_DecBufGetWritePointer(AMBA_AUDIO_DEC_BUF_CTRL_s *pHdlr);
UINT32 AmbaAudio_DecBufUpdateWritePointer(AMBA_AUDIO_DEC_BUF_CTRL_s *pHdlr);
UINT32 AmbaAudio_DecBufPeekRoomSize(AMBA_AUDIO_DEC_BUF_CTRL_s *pHdlr, UINT32 *pSize);
UINT32 AmbaAudio_DecBufPeekRemainSize(AMBA_AUDIO_DEC_BUF_CTRL_s *pHdlr, UINT32 *pSize);

UINT32 AmbaAudio_BufEfxInProcess(AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr, UINT32 *pSrc, UINT32 *pDst, UINT32 Size);
UINT32 AmbaAudio_BufEfxOutProcess(AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr, UINT32 *pSrc, UINT32 *pDst, UINT32 Size);
UINT32 AmbaAudio_BufSetup_Fc(UINT32 BufIndex, UINT32 AbuEfxDirection, UINT32 SrcCh, UINT32 DstCh, UINT32 SrcRes, UINT32 DstRes, UINT32 Shift, UINT32 FrameSize);
UINT32 AmbaAudio_BufSetup_FcCa(UINT32 BufIndex, UINT32 AbuEfxDirection, const INT32 *pCaTable);
UINT32 AmbaAudio_BufSetup_Volume(UINT32 BufIndex, UINT32 AbuEfxDirection, UINT32 Ch, UINT32 Res, UINT32 FrameSize, UINT32 Level);
UINT32 AmbaAudio_BufSetup_Dws(UINT32 BufIndex, UINT32 AbuEfxDirection, UINT32 SrcFreq, UINT32 DstFreq);
UINT32 AmbaAudio_BufSetup_DcBlocker(UINT32 BufIndex, UINT32 AbuEfxDirection, UINT32 Alpha);
#endif /* AMBA_AUDIO_BUFFERS_H */
