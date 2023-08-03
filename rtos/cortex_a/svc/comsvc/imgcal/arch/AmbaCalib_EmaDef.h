/**
 *  @file AmbaCalib_EmaDef.h
 *
 * Copyright (c) 2020 Ambarella International LP
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
 */
#ifndef AMBA_CALIB_EMA_DEF_H
#define AMBA_CALIB_EMA_DEF_H

#include "AmbaTypes.h"
#include "AmbaOD_2DBbx.h"
#include "AmbaCalib_Def.h"

enum {
    EMA_LOG_LVL_NONE, // 0
    EMA_LOG_LVL_CRITICAL, // 1
    EMA_LOG_LVL_WARNING, // 2
    EMA_LOG_LVL_NOTICE, // 3
    EMA_LOG_LVL_LOG, // 4
    EMA_LOG_LVL_DEBUG, // 5
    EMA_LOG_LVL_NEVER // 6
};

enum {
    AMBA_CAL_OVERLAP_AREA_LB = 0,
    AMBA_CAL_OVERLAP_AREA_RB,
    AMBA_CAL_OVERLAP_AREA_NUM,
};

enum {
    AMBA_CAL_EMA_BACK_FOV = 0,
    AMBA_CAL_EMA_LEFT_FOV,
    AMBA_CAL_EMA_RIGHT_FOV,
    AMBA_CAL_EMA_FOV_NUM,
};

typedef enum {
    AMBA_CAL_STITCH_LINE_STATUS_KEEP, // keep current stitch line
    AMBA_CAL_STITCH_LINE_STATUS_MUST, // suggested to change to new stitch line
    AMBA_CAL_STITCH_LINE_STATUS_NA,   // cannot find a good one
} AMBA_CAL_STITCH_LINE_STATUS_e;

typedef struct {
    UINT32 X1;
    UINT32 Y1;
    UINT32 X2;
    UINT32 Y2;
    UINT32 VLine;
} AMBA_CAL_EMA_LINE_s;

typedef struct {
    AMBA_CAL_EMA_LINE_s Line[AMBA_CAL_OVERLAP_AREA_NUM];
    UINT32 CloseObjTh;
} AMBA_CAL_EMA_FILTER_RULE_s;

typedef struct {
    AMBA_CAL_EMA_LINE_s Line[AMBA_CAL_OVERLAP_AREA_NUM];
} AMBA_CAL_EMA_FILTER_RULE_V2_s;

typedef struct {
    AMBA_CAL_SIZE_s Size;
    UINT8 *pAddr;
} AMBA_CAL_EMA_ROI_TBL_s;

typedef struct {
    UINT32 StitchX;
    AMBA_CAL_STITCH_LINE_STATUS_e Status;
} AMBA_CAL_EMA_STITCH_LINE_INFO_s;

typedef struct {
    UINT32 X1;
    UINT32 X2;
    UINT8 Alpha;
    UINT8 Margin;
} AMBA_CAL_EMA_BLEND_CFG_s;

typedef struct {
    AMBA_CAL_SIZE_s Size;
    AMBA_CAL_EMA_BLEND_CFG_s Cfg[AMBA_CAL_OVERLAP_AREA_NUM];
    UINT8 *pAddr;
} AMBA_CAL_EMA_BLEND_TBL_s;

#endif
