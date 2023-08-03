/**
 *  @file AmbaCalib_AVMDef.h
 *
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
#ifndef AMBA_CALIB_AVM_DEF_H
#define AMBA_CALIB_AVM_DEF_H
#include "AmbaTypes.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_WarpDef.h"

#define AVM_CALIB_POINT_NUM (4U)
#define MAX_AVM_ASSISTANCE_POINT_NUM (32U)
#define AVM_BLEND_MAX_SIZE (1536U * 1024U)
#define AVM_STATUS_MAX_SIZE MAX_WARP_TBL_LEN

#define AVM_3D_MODEL_MAX_BTM_COR_NUM (15U)
#define AVM_3D_MODEL_MAX_PILLAR_NUM (15U)
#define AVM_3D_MODEL_MAX_VERTICES_NUM ((AVM_3D_MODEL_MAX_BTM_COR_NUM+2U)*(AVM_3D_MODEL_MAX_PILLAR_NUM+1U)*4U)
#define AVM_3D_MODEL_MAX_MESH_NUM (((AVM_3D_MODEL_MAX_BTM_COR_NUM+1U)*(AVM_3D_MODEL_MAX_PILLAR_NUM+1U)*4U) + 1U)

#define AVM_BLEND_SECTOR_MAX_DEGREE (179U)
#define AVM_BLEND_SECTOR_MIN_DEGREE (1U)

#define AMBA_CAL_CAR_OSD_CORNER_MAX (8U)
#define MAX_PRE_CHECK_DATA_LEN ((MAX_WARP_TBL_H_GRID_NUM*2U)+(MAX_WARP_TBL_V_GRID_NUM*2U))

typedef enum {
    AMBA_CAL_AVM_CAM_FRONT = 0,
    AMBA_CAL_AVM_CAM_BACK,
    AMBA_CAL_AVM_CAM_LEFT,
    AMBA_CAL_AVM_CAM_RIGHT,
    AMBA_CAL_AVM_CAM_MAX,
} AMBA_CAL_AVM_CAM_ID_e;

typedef enum {
    AMBA_CAL_AVM_BlEND_CORNER_FL = 0,
    AMBA_CAL_AVM_BlEND_CORNER_FR,
    AMBA_CAL_AVM_BlEND_CORNER_BL,
    AMBA_CAL_AVM_BlEND_CORNER_BR,
    AMBA_CAL_AVM_BlEND_CORNER_MAX,
} AMBA_CAL_AVM_BlEND_CORNER_e;

typedef enum {
    AMBA_AVM_WARP_CAL_TYPE_0 = 0,
    AMBA_AVM_WARP_CAL_TYPE_1,
    AMBA_AVM_WARP_CAL_TYPE_2,
    AMBA_AVM_WARP_CAL_TYPE_3,
    AMBA_AVM_WARP_CAL_TYPE_AUTO,
    AMBA_AVM_WARP_CAL_TYPE_MAX,
} AMBA_CAL_AVM_3D_WARP_CAL_TYPE_e;

typedef enum {
    AMBA_AVM_MSG_GRID_STATUS = 0,
    AMBA_AVM_MSG_PTN_ERR,
    AMBA_AVM_MSG_REPORT,
    AMBA_AVM_MSG_MAX,
} AMBA_CAL_AVM_MSG_TYPE_e;

#define AMBA_AVM_CAR_PAT_REGION (255U)
#define AMBA_AVM_NONE_CAR_PAT_REGION (0U)

typedef struct {
    // Size of Pattern must be equal to Vout size
    UINT8 *Pattern;
    UINT32 VoutStartX;
    UINT32 VoutStartY;
    UINT32 Width;
    UINT32 Height;
} AMBA_CAL_AVM_CAR_PATTERN_s;

typedef struct {
    INT32 StartX;
    INT32 StartY;
    UINT32 Width;
    UINT32 Height;
} AMBA_CAL_AVM_VIEW_LAYOUT_s;

typedef struct {
    UINT32 Angle[AMBA_CAL_AVM_BlEND_CORNER_MAX];
    UINT32 Width[AMBA_CAL_AVM_BlEND_CORNER_MAX];
    UINT32 ReferenceCarVoutPosition[AMBA_CAL_AVM_BlEND_CORNER_MAX];
} AMBA_CAL_AVM_BLEND_CFG_s;

typedef struct {
    UINT32 Angle[AMBA_CAL_AVM_BlEND_CORNER_MAX];
    UINT32 SectorAngle[AMBA_CAL_AVM_BlEND_CORNER_MAX];
    UINT32 Width[AMBA_CAL_AVM_BlEND_CORNER_MAX];
    UINT32 ReferenceCarVoutPosition[AMBA_CAL_AVM_BlEND_CORNER_MAX];
} AMBA_CAL_AVM_BLEND_CFG_V2_s;

typedef struct {
    UINT32 Size;
    UINT32 StatusTbl[AVM_STATUS_MAX_SIZE];
} AMBA_CAL_AVM_STATUS_TBL_s;

typedef struct {
    UINT32 Width;
    UINT32 Height;
    UINT8 Table[AVM_BLEND_MAX_SIZE];
} AMBA_CAL_AVM_BLEND_TBL_s;

typedef struct {
    UINT32 HorGridNum;
    UINT32 VerGridNum;
    UINT32 GridPosX;
    UINT32 GridPosY;
    UINT32 Status;
} AMBA_CAL_AVM_WARP_TBL_STATUS_s;

typedef struct {
    UINT32 AssistancePointNumber;
    AMBA_CAL_POINT_DB_2D_s AssistancePointsErr[MAX_AVM_ASSISTANCE_POINT_NUM];
} AMBA_CAL_AVM_PTN_ERR_INFO_s;

typedef struct {
    UINT32 OutOfRoiGridNum;
    UINT32 VflipGridNum;
} AMBA_CAL_AVM_REPORT_s;

typedef struct {
    AMBA_CAL_AVM_WARP_TBL_STATUS_s *pGridStatus;
    AMBA_CAL_AVM_PTN_ERR_INFO_s *pPatternError;
    AMBA_CAL_AVM_REPORT_s *pReport;
} AMBA_CAL_AVM_MSG_s;

typedef UINT32 (*AMBA_CAL_AVM_CB_MSG_RECIVER_s)(AMBA_CAL_AVM_MSG_TYPE_e Type, AMBA_CAL_AVM_CAM_ID_e CamId,const AMBA_CAL_AVM_MSG_s *pMsg);

typedef enum {
    AMBA_CAL_AVM_3D_VIEW_MODEL_SIM = 1,
    AMBA_CAL_AVM_3D_VIEW_MODEL_ADV,
} AMBA_CAL_AVM_3D_VIEW_MODEL_e;

typedef struct {
    UINT8 UnionkDataBuf[28704U ];
} AMBA_CAL_AVM_3D_PRE_CHECK_V2_UNION_s;


typedef struct {
    UINT32 ChID;
    AMBA_CAL_ROI_s ManualROI;
    UINT16 WallWorldsLength;
    AMBA_CAL_POINT_DB_2D_s WallWorldsPos[MAX_PRE_CHECK_DATA_LEN];
    UINT16 GroundWorldsLength;
    AMBA_CAL_POINT_DB_2D_s GroundWorldsPos[MAX_PRE_CHECK_DATA_LEN];
} AMBA_CAL_AVM_PRE_CHECK_2D_TBL_s;

typedef struct {
    UINT32 ChID;
    AMBA_CAL_ROI_s ManualROI;
    UINT16 WorldsLength;
    AMBA_CAL_POINT_DB_3D_s WorldsPos[MAX_PRE_CHECK_DATA_LEN];
} AMBA_CAL_AVM_PRE_CHECK_3D_TBL_s;

#endif

