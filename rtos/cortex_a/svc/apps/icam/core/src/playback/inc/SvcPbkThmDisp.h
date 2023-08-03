/**
*  @file SvcPbkThmDisp.h
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
*  @details svc thm display related APIs
*
*/

#ifndef SVC_PBK_THM_DISP_H
#define SVC_PBK_THM_DISP_H

#include "SvcPbkPictDisp.h"
#include "AmbaStreamWrap.h"
#ifdef CONFIG_BUILD_COMMON_SERVICE_CODEC
#include "AmbaPlayer.h"
#endif

#define THMDISP_MAX_IMG_PER_FRAME           12U
#define THMDISP_ENABLE_IMAGE_ROTATION       1U
#define THMDISP_THM_YUV_SIZE                ((SVC_PICT_DISP_THM_BUF_PITCH) * (SVC_PICT_DISP_THM_BUF_HEIGHT))
#define THMDISP_THM_YUV_LIVE_SIZE           ((SVC_PICT_DISP_THM_BUF_DPX_PITCH) * (SVC_PICT_DISP_THM_BUF_DPX_HEIGHT))
#define THMDISP_PAGE_ROW_NUM                4U
#define THMDISP_PAGE_COLUMN_NUM             3U
#define THMDISP_PAGE_FILE_NUM               (THMDISP_PAGE_ROW_NUM * THMDISP_PAGE_COLUMN_NUM)

static inline UINT32 ReferenceValToRealVal(UINT32 RefValue, UINT32 VoutLength) { return ((RefValue * VoutLength) / 1000U);}
static inline UINT32 FileIdxToFileListEntry(UINT32 index) {return ((index - 1U) % SVC_PICT_DISP_THM_BUF_NUM);}
static inline UINT32 WrapAroundForward(UINT32 A, UINT32 Limit) {return ((A >= Limit) ? (A - Limit) : A);}
static inline UINT32 WrapAroundBackward(UINT32 A, UINT32 Limit) {return ((A == 0U) ? (A + Limit) : A);}
static inline UINT32 FileIdxWrapAroundForward(UINT32 A, UINT32 Limit) {return ((A > Limit) ? (A - Limit) : A);}
static inline UINT32 FileIdxWrapAroundBackward(UINT32 A, UINT32 Limit) {return ((A < 1U) ? (A + Limit) : A);}
static inline UINT32 PageFstFileIdx(UINT32 CurFileIndex) {return ((((CurFileIndex - 1U) / THMDISP_PAGE_FILE_NUM) * THMDISP_PAGE_FILE_NUM) + 1U);}

typedef struct {
    UINT16 OffsetX;
    UINT16 OffsetY;
    UINT16 Width;
    UINT16 Height;
} THMDISP_CONFIG_s;

typedef struct {
    AMBA_KAL_EVENT_FLAG_t   EventId;

    AMBA_DCF_FILE_TYPE_e    DcfScanType;
    UINT32                  PageFirstFile;
    UINT32                  CurFileIndex;
    UINT32                  ImgRotate;
    AMBA_DCF_FILE_TYPE_e    FileType;
    struct {
        AMBA_EXIF_IMAGE_INFO_s      Jpeg;
        AMBA_MOVIE_INFO_s           Mov;
    } ImgInfo;

    THMDISP_CONFIG_s        ThmDispLayout[AMBA_DSP_MAX_VOUT_NUM][THMDISP_PAGE_FILE_NUM];
} THMDISP_MGR_s;

UINT32 SvcThmDisp_Create(void);
UINT32 SvcThmDisp_Delete(void);
UINT32 SvcThmDisp_Handler(UINT32 KeyCode);
void   SvcThmDisp_SetCurFileIndex(UINT32 Index);
void   SvcThmDisp_GetCurFileIndex(UINT32 *pIndex);
UINT32 SvcThmDisp_ThmViewStart(AMBA_DCF_FILE_TYPE_e DcfScanType, UINT32 FileIndex);

#endif  /* SVC_PBK_THM_DISP_H */
