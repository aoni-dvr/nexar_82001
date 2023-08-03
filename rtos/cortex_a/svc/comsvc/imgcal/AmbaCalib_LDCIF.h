/**
 *  @file AmbaCalib_LDCIF.h
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
#ifndef AMBA_CALIB_LDC_IF_H
#define AMBA_CALIB_LDC_IF_H
#include "AmbaTypes.h"
#include "AmbaCalib_WarpDef.h"
#include "AmbaCalib_LDCDef.h"
#define AMBA_CAL_LDC_DATA_VERSION (1U)

/**
* This API is used to get IDC working buffer size. 
* @param [out] pSize LDC working buffer size.
* @return ErrorCode
*/
UINT32 AmbaCal_LdcGetBufSize(SIZE_t *pSize);
/**
* This API is used to generate IDC warp tables based on user specified settings. 
* @param [in] pCfg Ldc configuration. Please refer to AMBA_CAL_LDC_CFG_s for more details. 
* @param [in] pWorkingBuf Working buffer provided by user.
* @param [out] pOutput Returned output settings. Please refer to AMBA_CAL_LDC_DATA_s for more details. 
* @return ErrorCode
*/
UINT32 AmbaCal_LdcGenWarp(const AMBA_CAL_LDC_CFG_s *pCfg, const void *pWorkingBuf, AMBA_CAL_LDC_DATA_s *pOutput);

/**
* This API is used to change lens distortion table unit from mm to pixel. 
* @param [in] SensorCellSize Cell size on sensor.
* @param [in] pLenSpec Lens infomation. Please refer to AMBA_CAL_LENS_SPEC_s more details.
* @return ErrorCode
*/
UINT32 AmbaCal_DistortionTblMmToPixel(const DOUBLE SensorCellSize, const AMBA_CAL_LENS_SPEC_s *pLenSpec);

/**
* This API is used to change lens distortion table unit from mm to pixel. 
* @param [in] SensorCellSize Cell size on sensor.
* @param [in] pLenSpec Lens infomation. Please refer to AMBA_CAL_LENS_SPEC_V2_s more details.
* @return ErrorCode
*/
UINT32 AmbaCal_DistortionTblMmToPixelV2(const DOUBLE SensorCellSize, const AMBA_CAL_LENS_SPEC_V2_s *pLenSpec);

#endif
