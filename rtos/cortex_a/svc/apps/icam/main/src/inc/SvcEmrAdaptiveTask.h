/**
 *  @file SvcEmrAdaptiveTask.h
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
 *  @details svc emr adaptive task header
 *
 */

#ifndef SVC_EMR_ADAPTIVE_TASK_H
#define SVC_EMR_ADAPTIVE_TASK_H

#if defined(CONFIG_BUILD_AMBA_ADAS)
#include "AmbaOD_2DBbx.h"
#include "AmbaSurround.h"
#endif

enum {
    SVC_EMA_BACK_FOV = 0,
    SVC_EMA_LEFT_FOV,
    SVC_EMA_RIGHT_FOV,
    SVC_EMA_FLOW_CHAN_NUM
};

enum {
    SVC_EMA_OVERLAP_AREA_LB = 0,
    SVC_EMA_OVERLAP_AREA_RB,
    SVC_EMA_OVERLAP_AREA_NUM
};

typedef struct {
    UINT32                   Chan;
    UINT32                   FovIdx;
    AMBA_OD_2DBBX_LIST_s     *pBbx;
    const char               **Class_name;
    AMBA_SR_SROBJECT_DATA_s  *pSRData;
} SVC_EMA_DRAW_INFO_s;

void   SvcEmrAdaptiveTask_Init(void);
UINT32 SvcEmrAdaptiveTask_Config(void);
UINT32 SvcEmrAdaptiveTask_Start(void);
UINT32 SvcEmrAdaptiveTask_Stop(void);
UINT32 SvcEmrAdaptiveTask_SetDefaultBldTbl(void);
void   SvcEmrAdaptiveTask_SetPrint(UINT32 Enb);
void   SvcEmrAdaptiveTask_SetOsd(UINT32 Enb);
void   SvcEmrAdaptiveTask_SetFilterWidth(UINT32 Width);
void   SvcEmrAdaptiveTask_SetMaxStitchZone(UINT32 Width);
void   SvcEmrAdaptiveTask_SetBbxRoiMargin(UINT32 Width);

#endif /* SVC_EMR_ADAPTIVE_TASK_H */
