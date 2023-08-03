/**
*  @file SvcPbkStillDec.h
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
*  @details svc still dec related APIs
*
*/

#ifndef SVC_PBK_STILL_DEC_H
#define SVC_PBK_STILL_DEC_H
#include "AmbaExif.h"
#include "AmbaVfs.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"

#include "AmbaStreamWrap.h"
#include "AmbaPlayer.h"

#define SDEC_MAX_PROCESS_FILE_NUM       16U

#define SDEC_EVENT_DECODE_DONE          0x00000001U

#define SDEC_MARKER_OK                  0x0U
#define SDEC_MARKER_LOAD_BITS_ERROR     0xaU
#define SDEC_MARKER_DECODE_ABORT        0xbU
#define SDEC_MARKER_MAIN_BUF_ERROR      0xcU

#define SDEC_MQUEUE_ENTRY_NUM           1U
#define SDEC_EVENT_HANDLER_NUM          1U

#define SDEC_MSG_START_DECODE           0x1U
#define SDEC_MSG_START_BLOCK_DECODE     0x2U
#define SDEC_MSG_STOP_DECODE            0x3U

#define SDEC_FLAG_DECODE_IDLE           0x00000001U
#define SDEC_FLAG_DECODING              0x00000002U
#define SDEC_FLAG_DECODE_DONE           0x00000004U

typedef struct {
    ULONG   BitsBufWp;      /* Write point of the buffer */
    ULONG   BitsBufLimit;   /* Limit of the buffer */
    UINT32  BitsDataSize;   /* Size of bitstream */
    ULONG   pBitsDataBase;  /* Base address of bitstream */
    UINT8   Abort;
} SDEC_RUNTIME_PARAM_s;

typedef struct {
    ULONG  YAddr;
    ULONG  UVAddr;
    UINT32 YuvPitch;
    UINT32 YuvWidth;
    UINT32 YuvHeight;
    UINT32 YuvFormat;
    UINT32 StatusNum;
    UINT32 *pStatus;
} SVC_SDEC_DEC_REPORT_s;

typedef struct {
    UINT32                          MsgQueue[SDEC_MQUEUE_ENTRY_NUM];
    AMBA_KAL_MSG_QUEUE_t            MsgQueueId;
    AMBA_KAL_EVENT_FLAG_t           EventId;

    SDEC_RUNTIME_PARAM_s            Param;
    SVC_SDEC_DEC_REPORT_s           DecReport;
    UINT32                          DecStatus[SDEC_MAX_PROCESS_FILE_NUM];

    UINT8                           JpegTcInited;
} SDEC_MGR_s;

#define SDEC_IMG_TYPE_MAIN         0U
#define SDEC_IMG_TYPE_SCR          1U
#define SDEC_IMG_TYPE_THM          2U
#define SDEC_IMG_TYPE_H264_IFRAME  3U
#define SDEC_IMG_TYPE_HEVC_IFRAME  4U
#define SDEC_IMG_TYPE_MJPEG        5U

typedef struct {
    ULONG  pYBase;
    ULONG  pUVBase;
} SVC_SDEC_YUV_BUF_s;

typedef struct {
    UINT8               Valid;       /* Description is valid or not */
    UINT8               ErrorFlag;
    UINT32              FileIndex;
    char                FileName[64];
    SVC_SDEC_YUV_BUF_s  YuvBuf;
} SVC_SDEC_FILE_s;

typedef struct {
    char FName[64];
    UINT32 Type;
    struct {
        struct {
            UINT16 Width;
            UINT16 Height;
            UINT32 BitsOffset;
            UINT32 BitsSize;
            UINT8  Orientation;

            UINT8  EnableJpegTrans;                     /* Enable JPEG transcode */
            AMBA_EXIF_JPEG_HEADER_INFO_s JpegHeaderInfo;
            AMBA_EXIF_JPEG_INFO_s        JpegInfo;
        } Jpeg;

        AMBA_MOVIE_INFO_s  Mov;
    } Info;

    UINT8  SubEnable;                                   /* Enable rescaling */
    UINT8  SubRotateFlip;
    UINT16 SubYuvOffsetX;
    UINT16 SubYuvOffsetY;
    UINT16 SubYuvWidth;
    UINT16 SubYuvHeight;
    UINT16 SubBufPitch;
    UINT16 SubBufHeight;
    ULONG  SubBufYBase;                                 /* Destination buffer */
    ULONG  SubBufUVBase;                                /* Destination buffer */
} SVC_SDEC_DEC_SETUP_s;

typedef struct {
    UINT16 ImgNum;
    SVC_SDEC_DEC_SETUP_s Config[SDEC_MAX_PROCESS_FILE_NUM];
} SVC_SDEC_CONFIG_s;

typedef struct {
    ULONG                   MainBufBase;
    UINT32                  MainBufSize;
    ULONG                   BitsBufBase;
    UINT32                  BitsBufSize;
    AMBA_KAL_EVENT_FLAG_t*  pFbEventId;
} SVC_SDEC_CTRL_s;

extern SVC_SDEC_CONFIG_s  SvcSDecConfig;

UINT32 SvcPbkStillDec_TaskCreate(void);
UINT32 SvcPbkStillDec_TaskDelete(void);
void   SvcPbkStillDec_Setup(const SVC_SDEC_CTRL_s *pSetup);
UINT32 SvcPbkStillDec_StartDecode(void);
UINT32 SvcPbkStillDec_StopDecode(void);
void   SvcPbkStillDec_QueryDecResult(SVC_SDEC_DEC_REPORT_s **pReport);
void   SvcPbkStillDec_CheckYuv2YuvParam(UINT32 NumImg,
                                      const AMBA_DSP_YUV_IMG_BUF_s *pSrcYuvBufAddr,
                                      AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                                      const AMBA_DSP_STLDEC_YUV2YUV_s *pOperation);
UINT32 SvcPbkStillDec_StartY2Y(const AMBA_DSP_YUV_IMG_BUF_s *pSrcYuvBufAddr,
                               const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                               const AMBA_DSP_STLDEC_YUV2YUV_s *pOperation);

#endif  /* SVC_PBK_STILL_DEC_H */
