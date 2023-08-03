/**
 *  @file SvcWarningIconTask.h
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
 *  @details svc warning icon control task header file
 *
 */

#ifndef SVC_WARNING_ICON_TASK_H
#define SVC_WARNING_ICON_TASK_H

#include "SvcOsd.h"

#define SVC_WARN_ICON_TYPE_EMR       (0U)
#define SVC_WARN_ICON_TYPE_ADAS      (1U)
#define SVC_WARN_ICON_TYPE_NUM       (2U)

#define SVC_WARN_ICON_0     (0U)
#define SVC_WARN_ICON_1     (1U)
#define SVC_WARN_ICON_2     (2U)
#define SVC_WARN_ICON_3     (3U)
#define SVC_WARN_ICON_4     (4U)
#define SVC_WARN_ICON_5     (5U)
#define SVC_WARN_ICON_6     (6U)
#define SVC_WARN_ICON_7     (7U)
#define SVC_WARN_ICON_8     (8U)
#define SVC_WARN_ICON_9     (9U)
#define SVC_WARN_ICON_10    (10U)
#define SVC_WARN_ICON_11    (11U)
#define SVC_WARN_ICON_NUM   (12U)

/* EMR warning icon */
#define SVC_WARN_ICON_BSD_L     SVC_WARN_ICON_0
#define SVC_WARN_ICON_BSD_R     SVC_WARN_ICON_1
#define SVC_WARN_ICON_BSD_R1_L  SVC_WARN_ICON_2
#define SVC_WARN_ICON_BSD_R1_R  SVC_WARN_ICON_3

/* ADAS warning icon */
#define SVC_WARN_ICON_FCWS_R2    SVC_WARN_ICON_0
#define SVC_WARN_ICON_FCMD       SVC_WARN_ICON_1
#define SVC_WARN_ICON_LDWS       SVC_WARN_ICON_2
#define SVC_WARN_ICON_LDWS_L     SVC_WARN_ICON_3
#define SVC_WARN_ICON_LDWS_R     SVC_WARN_ICON_4
#define SVC_WARN_ICON_FCWS_R1    SVC_WARN_ICON_5
#define SVC_WARN_ICON_FCMD_2x    SVC_WARN_ICON_6
#define SVC_WARN_ICON_FCWS_R2_2x SVC_WARN_ICON_7
#define SVC_WARN_ICON_FCWS_R1_2x SVC_WARN_ICON_8
#define SVC_WARN_ICON_LDWS_2x    SVC_WARN_ICON_9
#define SVC_WARN_ICON_LDWS_L_2x  SVC_WARN_ICON_10
#define SVC_WARN_ICON_LDWS_R_2x  SVC_WARN_ICON_11

#define SVC_WARN_ICON_FLG_ENA     (0x1U)    // Enable warning icon
#define SVC_WANR_ICON_FLG_ANI_ENA (0x2U)    // Enable animation
#define SVC_WARN_ICON_FLG_SHOW    (0x4U)    // show icon
#define SVC_WANR_ICON_FLG_ANI     (0x8U)    // show animation

typedef struct {
    UINT32 StartX;
    UINT32 StartY;
    UINT32 EndX;
    UINT32 EndY;
} SVC_WARN_ICON_AREA_s;

typedef struct {
    UINT32 IsDataRdy;
    char   IconName[32];
    UINT8 *pIcon;
} SVC_WARN_ICON_RESOURCE_s;

typedef struct {
    UINT32 IsDataRdy;
    char   IconName[32];
    //UINT8 *pIcon;
    SVC_OSD_BMP_s IconInfo;
} SVC_WARN_ICON_FRAME_s;

typedef struct {
    UINT32                    Vout;
    SVC_WARN_ICON_AREA_s      IconArea;
    UINT32                    IconDataNum;
    SVC_WARN_ICON_FRAME_s    *pIconData;
    UINT32                    IconFrmSeqNum;
    UINT32                   *pIconFrmSeq;
    UINT32                    IconFrmInterval;
} SVC_WARN_ICON_s;

typedef struct {
    UINT32          WarnIconNum;
    SVC_WARN_ICON_s WarnIcon[SVC_WARN_ICON_NUM];
} SVC_WARN_ICON_CFG_s;

UINT32 SvcWarnIconTask_Init(void);
UINT32 SvcWarnIconTask_Config(UINT32 IconType);
UINT32 SvcWarnIconTask_Update(UINT32 IconID, UINT32 ActFlag);

#endif /* SVC_WARNING_ICON_TASK_H */
