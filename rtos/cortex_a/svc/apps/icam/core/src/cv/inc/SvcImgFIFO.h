/**
 *  @file SvcImgFIFO.h
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
 *  @details Header of SvcImgFIFO
 *
 */

#ifndef SVC_IMG_FIFO_H
#define SVC_IMG_FIFO_H

#define SVC_IMG_FIFO_MAX_CHAN                   (16U)
#define SVC_IMG_FIFO_MAX_USER_PER_CHAN          (3U)

typedef struct {
    UINT8  ChanID;
    UINT8  UserID;
    UINT32 UserTag;
} SVC_IMG_FIFO_CHAN_s;

typedef UINT32 (*SVC_IMG_FIFO_CALLBACK_f)(SVC_IMG_FIFO_CHAN_s *pChan, UINT32 Event, void *pInfo);

typedef struct {
    UINT8 Reserved;
} SVC_IMG_FIFO_INIT_CFG_s;

#define SVC_IMG_FIFO_SRC_PYRAMID                (SVC_CV_DATA_SRC_PYRAMID)
#define SVC_IMG_FIFO_SRC_MAIN_YUV               (SVC_CV_DATA_SRC_MAIN_YUV)
#define SVC_IMG_FIFO_SRC_YUV_STRM               (SVC_CV_DATA_SRC_YUV_STRM)
#define SVC_IMG_FIFO_SRC_RAW_DATA               (SVC_CV_DATA_SRC_RAW_DATA)
#define SVC_IMG_FIFO_SRC_PROC_RAW               (SVC_CV_DATA_SRC_PROC_RAW)
#define SVC_IMG_FIFO_SRC_MAIN_Y12               (SVC_CV_DATA_SRC_MAIN_Y12)
#define SVC_IMG_FIFO_SRC_PYRAMID_EX_SCALE       (SVC_CV_DATA_SRC_PYRAMID_EX_SCALE)
typedef struct {
    SVC_CV_INPUT_IMG_CONTENT_s Content;
    SVC_IMG_FIFO_CALLBACK_f pCallback;
    UINT32 UserTag;
} SVC_IMG_FIFO_CFG_s;

typedef struct {
    SVC_CV_INPUT_IMG_CONTENT_s Content;
    UINT8 *pQueueBase;                          /* Base address of data queue */
    UINT32 ElementSize;                         /* Size of each data element */
    UINT32 MaxElement;                          /* Max number of element */
    UINT32 Wp;                                  /* Current write point */
    UINT32 WrCount;                             /* Number of data being writen */
    UINT32 CountLimit;                          /* Maximum of WrCount */
} SVC_IMG_FIFO_DATA_STATUS_s;

#define SVC_IMG_FIFO_DATA_READY                 (1U)

UINT32 SvcImgFIFO_Init(SVC_IMG_FIFO_INIT_CFG_s *pCfg);

UINT32 SvcImgFIFO_Register(SVC_IMG_FIFO_CFG_s *pCfg, SVC_IMG_FIFO_CHAN_s *pChan);
UINT32 SvcImgFIFO_Unregister(SVC_IMG_FIFO_CHAN_s *pChan);

UINT32 SvcImgFIFO_GetFIFOStatus(SVC_IMG_FIFO_CHAN_s *pChan, SVC_IMG_FIFO_DATA_STATUS_s *pStatus);
void   SvcImgFIFO_CountToPointer(UINT32 Count, UINT32 CountLimit, UINT32 MaxElement, UINT32 *pPointer);
void   SvcImgFIFO_CountIncrement(UINT32 *pCount, UINT32 CountLimit, UINT32 Stride);


void   SvcImgFIFO_DebugEnable(UINT32 DebugEnable);

#endif /* SVC_IMG_FIFO_H */
