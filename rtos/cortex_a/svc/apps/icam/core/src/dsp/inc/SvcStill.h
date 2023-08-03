/**
 *  @file SvcStill.h
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
 *  @details svc application command functions
 *
 */

#ifndef SVC_STILL_H
#define SVC_STILL_H

/* PicType */
#define SVC_STL_TYPE_JPEG       (0U)
#define SVC_STL_TYPE_THM        (1U) /* video thumbnail jpeg */
#define SVC_STL_TYPE_YUV        (2U) /* main yuv */
#define SVC_STL_TYPE_LUMA       (3U)
#define SVC_STL_TYPE_CHROMA     (4U)
#define SVC_STL_TYPE_THMYUV     (5U) /* video thumbnail yuv for CV2FS */
#define SVC_STL_TYPE_THMYUV_PIV (6U) /* PIV thumbnail for CV2FS */
#define SVC_STL_TYPE_RAWONLY    (7U) /* without SVC_STL_TYPE_CE */
#define SVC_STL_TYPE_RAW        (8U) /* with SVC_STL_TYPE_CE */
#define SVC_STL_TYPE_CE         (9U)
#define SVC_STL_TYPE_NULL_WR   (10U) /* do y2j but null write */
#define SVC_STL_TYPE_CLEAN_RAW (11U) /* R2R rescale + BPC */
#define SVC_STL_TYPE_IDSP      (12U) /* still capture ituner, IK CR and uCode CR */

#define SVC_MAX_PIV_STREAMS    ((UINT32)CONFIG_ICAM_MAX_REC_STRM + AMBA_DSP_MAX_VOUT_NUM)
#define SVC_STL_MAX_AEB_NUM    (5U)

#define SVC_STL_IDSP_ITUNER     (0U)
#define SVC_STL_IDSP_IK_CR      (1U)
#define SVC_STL_IDSP_UCODE_CR   (2U)

/* PicView */
#define SVC_STL_MAIN_VIEW (0U)
#define SVC_STL_SCRN_VIEW (1U)
#define SVC_STL_THMB_VIEW (2U)
#define SVC_STL_QVIEW_VIEW (3U)
#define SVC_STL_NUM_VIEW  (4U)

#define SIZE_JPEG_Q_TABLE (128U)

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define SVC_STL_SCRN_W    (896U) // CV5/2FS AmbaIK_SetWindowSizeInfo VinSensor.Width need 128/64 aligned, and VinSensor.Height need 8 aligned.
#else
#define SVC_STL_SCRN_W    (960U)
#endif
#define SVC_STL_THMB_W    (160U)

#define SVC_STL_MAX_CAP_NUM    (5U)
#define SVC_STL_MAX_W          (10000U)
#define SVC_STL_MAX_H          (10000U)

typedef struct {
    ULONG               BaseAddrY;      /* Luma (Y) data buffer address, or Raw buffer address*/
    ULONG               BaseAddrUV;     /* Chroma (UV) buffer address, or CE buffer address */
    UINT16              Pitch;          /* YUV/RAW data buffer pitch */
    UINT16              Width;
    UINT16              Height;
    UINT16              OffsetX;        /* Valid for RAW */
    UINT16              OffsetY;        /* Valid for RAW */
    UINT16              CapInstance;
    UINT8               DataFmt;        /* YUV Data format */
    UINT8               CapType;        /* SVC_FRAME_TYPE_YUV - yuv capture, SVC_FRAME_TYPE_RAW - raw capture */
    UINT8               RawCmpr;
    UINT8               CeAvail;
    UINT16              Index;          /* VinID for raw capture */
    UINT8               IsHiso;
    UINT8               SensorIdx;      /* Sensor Idx for calibration */
} SVC_CAP_MSG_s;

typedef struct {
    ULONG               BaseAddrY;      /* Luma (Y) data buffer address */
    ULONG               BaseAddrUV;     /* Chroma (UV) buffer address */
    UINT16              Pitch;          /* YUV data buffer pitch */
    UINT16              Width;
    UINT16              Height;
    UINT16              CapInstance;
    UINT8               DataFmt;        /* YUV Data format */
    UINT8               Reserved[3];
} SVC_YUV_IMG_BUF_s;

typedef struct {
    ULONG               BufBase;
    UINT32              BufSize;
    ULONG               DataStart;
    UINT32              DataSize;
    UINT8               PicType;
} SVC_STILL_DATAREADY_s;

typedef UINT32 (*SVC_STILL_CALLBACK_f)(void);
typedef void (*SVC_STILL_DATAREADY_f)(SVC_STILL_DATAREADY_s *pInfo);

typedef struct {
    SVC_STILL_CALLBACK_f pLivStop;
    SVC_STILL_CALLBACK_f pLivStart;
} SVC_STILL_CALLBACK_s;

extern SVC_STILL_CALLBACK_f pSvcLivStop;
extern SVC_STILL_CALLBACK_f pSvcLivStart;
extern SVC_STILL_DATAREADY_f pSvDataReady;

void SvcStill_CalJpegDqt(UINT8 *pQTable, INT32 Quality);
void SvcStill_SvcYuv2SspYuv(const SVC_YUV_IMG_BUF_s *pSvcYuv, AMBA_DSP_YUV_IMG_BUF_s *pSspYuv);

void SvcStill_RegisterCallback(const SVC_STILL_CALLBACK_s *pCallback);
void SvcStill_RegisterDataReady(SVC_STILL_DATAREADY_f pHandler);

UINT32 SvcStillBufCopy(UINT8 *pDst, UINT8 *pSrc, UINT32 Size);
void   SvcStillLog(const char *pFmt, ULONG x1, ULONG x2, ULONG x3, ULONG x4, ULONG x5);

#endif /* SVC_STILL_H */
