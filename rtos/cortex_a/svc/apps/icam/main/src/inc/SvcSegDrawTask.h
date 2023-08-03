/**
 *  @file SvcSegDrawTask.h
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
 *  @details svc segmentation draw task header
 *
 */

#ifndef SVC_SEG_DRAW_TASK_H
#define SVC_SEG_DRAW_TASK_H

#define SVC_LW_DRAW        (0U)

#if defined CONFIG_ICAM_VOUTB_OSD_BUF_FHD
#define SVC_LW_LKA_RADIUS  (90U)
#elif defined CONFIG_ICAM_VOUTB_OSD_BUF_HD
#define SVC_LW_LKA_RADIUS  (60U)
#else
#define SVC_LW_LKA_RADIUS  (45U)
#endif

/* Nee to sync to menu icon size */
#if defined(CONFIG_ICAM_VOUTB_OSD_BUF_FHD)
#define LKA_MINUS_ICON_H  (120U)
#else
#define LKA_MINUS_ICON_H  (60U)
#endif


typedef struct {
    UINT32 Chan;
    UINT32 Xstart;
    UINT32 Ystart;
    UINT32 Width;
    UINT32 Height;
} SVC_SEG_DRAW_OSD_ROI_s;

UINT32 SvcSegDrawTask_Init(void);
UINT32 SvcSegDrawTask_Start(void);
UINT32 SvcSegDrawTask_Stop(void);
SVC_GUI_CANVAS_s* SvcSegDrawTask_GetCanvasROIinfo(void);
#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
UINT32 SvcSegDrawTask_LdMsg(UINT32 MsgCode, const void *pInfo);
#endif
void SvcSegDrawTask_FoVResInit(void);
#if defined(CONFIG_ICAM_CV_LOGGER)
void SvcSegDrawTask_SetSaveBufferNum(UINT32 Number);
#endif

#endif /* SVC_SEG_DRAW_TASK_H */
