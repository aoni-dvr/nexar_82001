/**
*  @file SvcPbkPictDisp.h
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
*  @details svc picture display related APIs
*
*/

#ifndef SVC_PBK_PICT_DISP_H
#define SVC_PBK_PICT_DISP_H

#include "AmbaDCF.h"
#include "AmbaFS.h"
#include "AmbaExif.h"
#include "AmbaVfs.h"

#include "AmbaStreamWrap.h"
#ifdef CONFIG_BUILD_COMMON_SERVICE_CODEC
#include "AmbaPlayer.h"
#endif
#include "SvcWinCalc.h"
#include "SvcResCfg.h"

#define SVC_PICT_DISP_NONE_STATE            0U
#define SVC_PICT_DISP_INIT_STATE            1U
#define SVC_PICT_DISP_SINGLE_VIEW_STATE     2U
#define SVC_PICT_DISP_THM_VIEW_STATE        3U
#define SVC_PICT_DISP_VIDEO_PLAY_STATE      4U
#define SVC_PICT_DISP_TRANSCODE_STATE       5U

#define SVC_PICT_DISP_EVENT_TASK_IDLE       0x00000100U
#define SVC_PICT_DISP_EVENT_VOUT_IDLE       0x00000200U

typedef struct {
    ULONG   BufBase;
    UINT32  BufSize;
} SVC_PICT_DISP_MEM_CTRL_s;

typedef struct {
    SVC_PICT_DISP_MEM_CTRL_s JpegDecBitsBuf;
    SVC_PICT_DISP_MEM_CTRL_s JpegDecOutBuf;
    SVC_PICT_DISP_MEM_CTRL_s DispYuvBuf[AMBA_DSP_MAX_VOUT_NUM];
    SVC_PICT_DISP_MEM_CTRL_s ThmYuvBuf;
    SVC_PICT_DISP_MEM_CTRL_s ScrYuvBuf;
    SVC_PICT_DISP_MEM_CTRL_s Y2YSrcBuf;
    SVC_PICT_DISP_MEM_CTRL_s Y2YDstBuf;
} SVC_PICT_DISP_MEM_s;

typedef struct {
    UINT32 VideoStart;
    UINT32 VideoPause;
    UINT32 VideoSpeed;
    UINT32 VideoDirect;
} SVC_VIDPBK_INFO_s;

static inline UINT32 ALIGN32(UINT32 X) {return ((X + 31U) & 0xFFFFFFE0U);}
static inline UINT32 ALIGN64(UINT32 X) {return ((X + 63U) & 0xFFFFFFC0U);}
static inline UINT32 ALIGN16(UINT32 X) {return ((X + 15U) & 0xFFFFFFF0U);}

#define SVC_PICT_DISP_THM_BUF_NUM      12U

#define SVC_PICT_DISP_THM_BUF_WIDTH         640U
#define SVC_PICT_DISP_THM_BUF_HEIGHT        480U
#define SVC_PICT_DISP_THM_BUF_PITCH         ALIGN64(SVC_PICT_DISP_THM_BUF_WIDTH)

#define SVC_PICT_DISP_THM_BUF_DPX_WIDTH     384U  /* based on SVC_PICT_DISP_SCRNAIL_BUF_WIDTH * SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT */
#define SVC_PICT_DISP_THM_BUF_DPX_HEIGHT    300U  /* based on SVC_PICT_DISP_SCRNAIL_BUF_WIDTH * SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT */
#define SVC_PICT_DISP_THM_BUF_DPX_PITCH     ALIGN64(SVC_PICT_DISP_THM_BUF_DPX_WIDTH)

#define SVC_PICT_DISP_SCRNAIL_BUF_NUM       1U

#define SVC_PICT_DISP_SCRNAIL_BUF_WIDTH     1920U
#define SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT    1080U
#define SVC_PICT_DISP_SCRNAIL_BUF_PITCH     ALIGN64(SVC_PICT_DISP_SCRNAIL_BUF_WIDTH)

#define SVC_PICT_DISP_KEY_UP         1U
#define SVC_PICT_DISP_KEY_DOWN       2U
#define SVC_PICT_DISP_KEY_LEFT       3U
#define SVC_PICT_DISP_KEY_RIGHT      4U
#define SVC_PICT_DISP_KEY_OK         5U
#define SVC_PICT_DISP_KEY_DEL        6U

#define SVC_VIDPBK_PMT_START         1U
#define SVC_VIDPBK_PMT_STOP          2U
#define SVC_VIDPBK_PMT_PAUSE         3U
#define SVC_VIDPBK_PMT_RESUME        4U
#define SVC_VIDPBK_PMT_STEP          5U
#define SVC_VIDPBK_PMT_DIRECT        6U
#define SVC_VIDPBK_PMT_SPEED         7U
#define SVC_VIDPBK_PMT_MODE          8U
#define SVC_VIDPBK_PMT_ZOOM          9U
#define SVC_VIDPBK_PMT_AUDIO         10U
#define SVC_VIDPBK_PMT_GETINFO       11U

#define SVC_VIDPBK_MODE_BEGIN        1U
#define SVC_VIDPBK_MODE_CONT         2U

#define SVC_VIDPBK_ZOOM_IN           1U
#define SVC_VIDPBK_ZOOM_OUT          2U

#define SVC_VIDPBK_STOP_STAY         1U
#define SVC_VIDPBK_STOP_BACK         2U

#define SVC_PICT_DISP_MQUEUE_ENTRY_NUM      2U
#define SVC_PICT_DISP_SCR_BUF_SIZE          ((SVC_PICT_DISP_SCRNAIL_BUF_PITCH) * ALIGN16(SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT))

#define SVC_PICT_DISP_ZOOM_RATIO_MIN        100U
#define SVC_PICT_DISP_ZOOM_RATIO_1X         100U
#define SVC_PICT_DISP_ZOOM_RATIO_MAX        1000U

#define SVC_PICT_DISP_STACK_SIZE            (0xD000U)
#define SVC_PBK_PICT_DISP_TASK_PRI          (62U)
#define SVC_PBK_PICT_DISP_TASK_CPU_BITS     (0x01U)

typedef struct {
    UINT8                   Error;
    AMBA_DCF_FILE_TYPE_e    FileType;
    char                    FName[64];
    struct {
        AMBA_EXIF_IMAGE_INFO_s  Jpeg;
        AMBA_MOVIE_INFO_s       Mov;
    } Info;
} SVC_PICT_DISP_FILE_INFO_s;

typedef struct {
    UINT32                      MsgQueue[SVC_PICT_DISP_MQUEUE_ENTRY_NUM];
    AMBA_KAL_MSG_QUEUE_t        MsgQueueId;

    AMBA_DCF_FILE_TYPE_e        DcfScanType;
    UINT32                      CurFileIndex;
    UINT32                      ZoomRatio;
    UINT32                      ZoomBorder;
    UINT32                      ZoomCenterX;
    UINT32                      ZoomCenterY;
    UINT32                      ImgRotate;
    SVC_PICT_DISP_FILE_INFO_s   FileInfo;

    UINT32                      SCRBufIndex;
} SVC_PICT_DISP_MGR_s;

typedef struct {
    SVC_PICT_DISP_MEM_CTRL_s JpegDecBits;
    SVC_PICT_DISP_MEM_CTRL_s JpegDecOut;
    SVC_PICT_DISP_MEM_CTRL_s PictDisp;
} SVC_PICT_DISP_BUF_INIT_s;

typedef struct {
    SVC_PICT_DISP_BUF_INIT_s BufInit;
    UINT32                   Mode;
    #define SVC_PICT_DISP_MODE_DUPLEX    (1U)
    #define SVC_PICT_DISP_MODE_PLAYBACK  (2U)

    UINT32                   FovId;        /* for duplex mode display*/
    UINT32                   Y2YCtxId;     /* for duplex mode ik config*/
    UINT32                   ShowDbgLog;   /* 0: Off, 1: On */
} SVC_PICT_DISP_INIT_s;

typedef struct {
    UINT32                   VoutId;
    AMBA_DSP_WINDOW_s        DispWin;
} SVC_PICT_DISP_WIN_s;

typedef struct {
    UINT32                   Mode;
    UINT32                   ShowDbgLog;  /* 0: Off, 1: On */
    UINT32                   FovId;       /* for duplex mode display*/
    UINT32                   Y2YCtxId;    /* for duplex mode ik config*/
    UINT32                   DispNum;
    SVC_PICT_DISP_WIN_s      Disp[AMBA_DSP_MAX_VOUT_NUM];
} SVC_PICT_DISP_INFO_s;

extern AMBA_KAL_EVENT_FLAG_t  SvcPictDispEventId;

void   SvcPbkPictDisp_QuerryMem(UINT32 Mode, const SVC_RES_CFG_s* pCfg, UINT32 *pMemSize);
void   SvcPbkPictDisp_Init(const SVC_RES_CFG_s* pCfg, const SVC_PICT_DISP_INIT_s *pInit);
UINT32 SvcPbkPictDisp_TaskCreate(void);
UINT32 SvcPbkPictDisp_TaskDelete(void);
void   SvcPbkPictDisp_KeyEventSend(UINT32 KeyCode);
void   SvcPbkPictDisp_SetState(UINT32 State);
UINT32 SvcPbkPictDisp_GetState(void);
UINT32 SvcPbkPictDisp_WaitTaskIdle(void);
UINT32 SvcPbkPictDisp_Handler(UINT32 KeyCode);
UINT32 SvcPbkPictDisp_SingleViewStart(AMBA_DCF_FILE_TYPE_e DcfScanType, UINT32 FileIndex);
void   SvcPbkPictDisp_SetCurFileIndex(UINT32 Index);
void   SvcPbkPictDisp_GetCurFileIndex(UINT32 *pIndex);
UINT32 SvcPbkPictDisp_GetFileType(const char *FileName, AMBA_DCF_FILE_TYPE_e *FileType);
void   SvcPbkPictDisp_CalcBaseDispSize(UINT32 ActiveWidth, UINT32 ActiveHeight, UINT32 ActiveWinAR,
                                 UINT32 InputImgAR, UINT32 *pResultDispW1X, UINT32 *pResultDispH1X);
void   SvcPbkPictDisp_GetPictDispMem(SVC_PICT_DISP_MEM_s **ppPictDispMem);
void   SvcPbkPictDisp_MemorySetU16(UINT16 *pBuffer, UINT16 Value, UINT32 Size);
void   PictDispVideoHandler(UINT32 ParamType, const void *pNewVal);
void   SvcPbkPictDisp_GetInfo(const SVC_PICT_DISP_INFO_s** ppInfo);
UINT32 SvcPbkPictDisp_BlockCopy(const AMBA_GDMA_BLOCK_s * pBlockBlit, AMBA_GDMA_ISR_f NotifierFunc, UINT32 NotifierFuncArg, UINT32 TimeOut);

#endif  /* SVC_PBK_PICT_DISP_H */
