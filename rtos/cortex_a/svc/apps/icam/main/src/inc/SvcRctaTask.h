/**
 *  @file SvcRctaTask.h
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
 *  @details Svc RCTA task header file
 *
 */

#ifndef SVC_RCTA_TASK_H
#define SVC_RCTA_TASK_H


#define RCTA_SEG_WIDTH 1280U
#define RCTA_SEG_HEIGH 512U
#define SVC_RCTA_WRK_BUF_SIZE (1318912U)    /** 795648 for V1.3 */

#define SVC_RCTA_WZ_SLICE_L (12U)
#define SVC_RCTA_WZ_SLICE_D (8U)
#define WZ_POINT_NUM ((SVC_RCTA_WZ_SLICE_L * 2U) + SVC_RCTA_WZ_SLICE_D + 1U)

typedef struct {
    UINT32 WZApcPntNumR;
    UINT32 WZTtcPntNumR;
    UINT32 WZApcPntNumL;
    UINT32 WZTtcPntNumL;
    AMBA_CAL_POINT_INT_2D_s  WarnZoneApcR[WZ_POINT_NUM];
    AMBA_CAL_POINT_INT_2D_s  WarnZoneTtcR[WZ_POINT_NUM];
    AMBA_CAL_POINT_INT_2D_s  WarnZoneApcL[WZ_POINT_NUM];
    AMBA_CAL_POINT_INT_2D_s  WarnZoneTtcL[WZ_POINT_NUM];
} SVC_RCTA_TASK_WARN_ZONE_DATA_s;

typedef struct {
    UINT32 WarnChan;
    #define SUR_RCTA_FRONT 0U
    #define SUR_RCTA_REAR  1U
    AMBA_WS_RCTA_WARNING_INFO_s WarnInfoL;        /**< Warning information of left side(world) >*/
    AMBA_WS_RCTA_WARNING_INFO_s WarnInfoR;        /**< Warning information of right side(world) >*/
    SVC_RCTA_TASK_WARN_ZONE_DATA_s WarningZone;
} SVC_RCTA_TASK_DRAW_DATA_s;

UINT32 SvcRctaTask_Init(void);

UINT32 SvcRctaTask_Start(void);

UINT32 SvcRctaTask_Stop(void);

#endif /* SVC_RCTA_TASK_H */


