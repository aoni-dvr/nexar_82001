/**
*  @file SvcStillMux.h
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
*
*/

#ifndef SVC_STL_MUX_H
#define SVC_STL_MUX_H

#define SVC_LOG_STL_MUX        "STL_MUX"
#define SVC_STL_MUX_STACK_SIZE (0x5000U)

#define SVC_STL_MUX_NUM_FILES  (8U)

#define SVC_STL_MUX_MAX_FILENAME_LEN  (64UL)

typedef struct {
    UINT32   Priority;      /* scanning task priority */
    UINT32   CpuBits;       /* core selection which scanning task running at */
    ULONG    VideoBufAddr;
    UINT32   VideoBufSize;
    ULONG    StillBufAddr;
    UINT32   StillBufSize;
    ULONG    YuvBufAddr;
    UINT32   YuvBufSize;
    ULONG    RawBufAddr;
    UINT32   RawBufSize;
    ULONG    ExifBufAddr;
    UINT32   ExifBufSize;
    char     StorageDrive;
} SVC_STL_MUX_CONFIG_s;

typedef struct {
    UINT16                 RawSeq;
    UINT16                 PicView;
    UINT16                 Width;
    UINT16                 Height;
    UINT8                  PicType;
    UINT8                  Reserved[3];
    AMBA_DSP_ENC_PIC_RDY_s PicRdy;
} SVC_STL_MUX_INPUT_s;

typedef struct {
    UINT32   MaxWidth;
    UINT32   MaxHeight;
    UINT32   MaxScrW;
    UINT32   MaxScrH;
    UINT32   NumFrameBuf;
} SVC_STL_MUX_SETUP_s;

UINT32 SvcStillMux_QueryMem(const SVC_STL_MUX_SETUP_s *pSetup, UINT32 *pMemSize);
UINT32 SvcStillMux_Create(const SVC_STL_MUX_CONFIG_s *pCfg);
UINT32 SvcStillMux_Delete(void);
void   SvcStillMux_Debug(UINT8 On);
UINT32 SvcStillMux_SetExifByFov(UINT32 FovId, UINT8 StreamId);
UINT32 SvcStillMux_SetExifByVin(UINT32 VinId, UINT32 SensorIdx, UINT32 SensorNum, UINT8 AebNum);
UINT32 SvcStillMux_SetVidThmFileName(UINT8 RecStream, char *pFileName, UINT8 FileNameLength);
UINT32 SvcStillMux_CreatePicFile(UINT8 PicType, UINT8 CapType, UINT16 StreamId, UINT16 SensorIdx, UINT16 CapSeq, UINT16 AebIdx);
UINT32 SvcStillMux_WaitAvail(void);
UINT32 SvcStillMux_InputPic(const SVC_STL_MUX_INPUT_s *pPicInput);

#endif  /* SVC_STL_MUX_H */
