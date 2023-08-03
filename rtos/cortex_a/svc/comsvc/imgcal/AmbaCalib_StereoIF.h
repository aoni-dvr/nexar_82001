/**
 *  @file AmbaCalib_StereoIF.h
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
#ifndef AMBA_CALIB_STEREO_API_H
#define AMBA_CALIB_STEREO_API_H

#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV2FS)
#include "AmbaCalib_ErrNo.h"
#include "AmbaCalib_EmirrorDef.h"

#define AMBA_CAL_ST_MAX_CORNER_NUM (7260U)

#define AMBA_CAL_ST_MAX_CORNER_2D_MAP_WIDTH  (200U)
#define AMBA_CAL_ST_MAX_CORNER_2D_MAP_HEIGHT (200U)

typedef struct {
    AMBA_CAL_SIZE_s Corner2dMapSize[AMBA_CAL_ST_CAM_NUM];
    AMBA_CAL_SIZE_s RawSize[AMBA_CAL_ST_CAM_NUM];
    AMBA_CAL_POINT_INT_2D_s BaseCorner2DIdx[AMBA_CAL_ST_CAM_NUM];
    AMBA_CAL_WORLD_RAW_POINT_s *pCornerPairs[AMBA_CAL_ST_CAM_NUM]; // need feed in
    AMBA_CAL_EM_CURVED_SURFACE_CFG_s VirtualCamViewCfg[AMBA_CAL_ST_CAM_NUM];
    AMBA_CAL_ROI_s Vout[AMBA_CAL_ST_CAM_NUM];
    AMBA_CAL_CAM_s Cam[AMBA_CAL_ST_CAM_NUM];
    DOUBLE CalibMatrix[AMBA_CAL_ST_CAM_NUM][9];
    UINT32 HorGridNum;
    UINT32 VerGridNum;
    UINT32 TileWidthExp;
    UINT32 TileHeightExp;
    DOUBLE DistanceToWall; // in mm
    DOUBLE BaseLine; // in mm
} AMBA_CAL_ST_REFINE_CFG_s;

typedef AMBA_CAL_EM_SV_DATA_s AMBA_CAL_ST_DATA_s;

UINT32 AmbaCal_GetStereoRefineWorkSize(SIZE_t *pSize);
UINT32 AmbaCal_StereoRefineTbl(const void *pWorkingBuf,
    const AMBA_CAL_ST_REFINE_CFG_s *pCfg, AMBA_CAL_GRID_POINT_s *pLeftWarpTbl);

#endif
#endif
