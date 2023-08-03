/**
 *  @file AmbaCalib_UtilityIF.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Toolbox containing calibration-related utility
 *
 */
#ifndef AMBA_CALIB_UTILITY_IF_H
#define AMBA_CALIB_UTILITY_IF_H

#include "AmbaCalib_Def.h"

#define AMBA_CAL_UTIL_MAX_PLANE_CFG_PT_NUM  (2000U)
#define AMBA_CAL_UTIL_PLANE_COEF_NUM        (3U)

typedef struct {
    DOUBLE C[AMBA_CAL_UTIL_PLANE_COEF_NUM]; // Z = C[0]X + C[1]Y+ C[2]
} AMBA_CAL_UTIL_PLANE_COEF_s;

typedef struct {
    AMBA_CAL_POINT_DB_3D_s Points[AMBA_CAL_UTIL_MAX_PLANE_CFG_PT_NUM];
    DOUBLE PointsWeighting[AMBA_CAL_UTIL_MAX_PLANE_CFG_PT_NUM];
    UINT32 PointsNum;
} AMBA_CAL_UTIL_FIT_PLANE_CFG_s;


#ifdef __cplusplus
extern "C" {
#endif
UINT32 AmbaCal_UtilGetFitPlaneWorkSize(UINT32 *pWorkBuffSize);
UINT32 AmbaCal_UtilFitPlane(const void *pWorkBuf, const AMBA_CAL_UTIL_FIT_PLANE_CFG_s *pCfg,
    AMBA_CAL_UTIL_PLANE_COEF_s *pCoef);
#ifdef __cplusplus
}
#endif

#endif /* AMBA_CALIB_UTILITY_IF_H */
