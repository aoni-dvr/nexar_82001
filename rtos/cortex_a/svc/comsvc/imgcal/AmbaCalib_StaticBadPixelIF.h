/**
 *  @file AmbaCalib_StaticBadPixelIF.h
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
#ifndef AMBA_CALIB_STATIC_BAD_PIXEL_IF_H
#define AMBA_CALIB_STATIC_BAD_PIXEL_IF_H
#include "AmbaTypes.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_StaticBadPixelDef.h"

#define AMBA_CAL_SBP_DATA_VERSION (1U)
#define AMBA_CAL_BPC_DEF_BLOCK_WIDTH (160U)
#define AMBA_CAL_BPC_DEF_BLOCK_HEIGHT (160U)
/**
* This API is used to generate static bad pixel tables based on input raw data.
* This is an old version API already, please use latest one instead.
* @param [in] pCfg Static bad pixels configuration. Please refer to AMBA_CAL_BPC_CFG_s for more details.
* @param [in] pRawBuf Raw data buffer provided by user. This raw is actually used raw area .It does not contain OB area.
* @param [in] pRawArea Input Raw data information. Please refer to AMBA_CAL_ROI_s for more details. This area is actually used raw area .It does not contain OB area.
* @param [in] RawPitch Raw pitch of raw data
* @param [out] pData Returned output settings. Please refer to AMBA_CAL_BPC_CALIB_DATA_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_BpcRaw2BpcTbl(const AMBA_CAL_BPC_CFG_s *pCfg, const UINT16 *pRawBuf, const AMBA_CAL_ROI_s *pRawArea, UINT32 RawPitch, AMBA_CAL_BPC_CALIB_DATA_s *pData);

/**
* This API is used to generate static bad pixel tables based on input raw data.
* This is an old version API already, please use latest one instead.
* @param [in] pCfg Static bad pixels configuration. Please refer to AMBA_CAL_BPC_CFG_s for more details.
* @param [in] pRawBuf Raw data buffer provided by user. This raw is actually used raw area .It does not contain OB area.
* @param [in] pRawSize Input Raw data information. Please refer to AMBA_CAL_SIZE_s for more details. This area is actually used raw area .It does not contain OB area.
* @param [in] RawPitch Raw pitch of raw data
* @param [out] pData Returned output data. Please refer to AMBA_CAL_BPC_CALIB_DATA_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_BpcRaw2BpcTblV1(const AMBA_CAL_BPC_CFG_V1_s *pCfg, const UINT16 *pRawBuf, const AMBA_CAL_SIZE_s *pRawSize, UINT32 RawPitch, AMBA_CAL_BPC_CALIB_DATA_s *pData);

/**
* This API is used to get necessary working buffer size for static bad pixel calibration .
* @param [in] pImageSize Raw image size, Please refer to AMBA_CAL_SIZE_s for more details.
* @param [out] pBufSize Returned working buffer size.
* @return ErrorCode
*/
UINT32 AmbaCal_BpcGetWorkingBufSize(const AMBA_CAL_SIZE_s *pRawSize, SIZE_t *pBufSize);

/**
* This API is used to generate static bad pixel tables based on input raw data.
* From this version, there is no limitations on raw size.
* @param [in] pCfg Calibration related configuration. Please refer to AMBA_CAL_BPC_CFG_V1_s for more details.
* @param [in] pWorkingBuf Buffer for algorithm calculation and store output result. Please use AmbaCal_BpcGetWorkingBufSize() to get working buffer size.
* @param [in] pRawBuf Raw data buffer information. Raw data should not contain data of OB area. Please refer to AMBA_CAL_RAW_BUF_s for details.
* @param [in] pRawSize Input raw data size (does not contain OB area). Please refer to AMBA_CAL_SIZE_s for more details.
* @param [in] RawPitch Pitch of raw data
* @param [out] pData Returned output data. Please refer to AMBA_CAL_BPC_CALIB_DATA_V1_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_BpcRaw2BpcTblV2(const AMBA_CAL_BPC_CFG_V2_s *pCfg, const void *pWorkingBuf, const void *pRawBuf, const AMBA_CAL_SIZE_s *pRawSize, UINT32 RawPitch, AMBA_CAL_BPC_CALIB_DATA_V1_s *pData);

/**
* This API is used to merge two static bad pixel tables that own identical size.
* @param [in] pSbpTblA Buffer of static bad pixels calib table A that want to merge.
* @param [in] pSbpTblB Buffer of static bad pixels calib table B that want to merge.
* @param [in] SbpTblSize Input static bad pixels calib table size. A and B should be identical.
* @param [out] pSbpTblOut Returned output static bad pixels calib table which table A and B merged into.
* @return ErrorCode
*/
UINT32 AmbaCal_BpcBpcTblMerge(const UINT8* pSbpTblA, const UINT8* pSbpTblB, UINT32 SbpTblSize, UINT8* pSbpTblOut);

#endif
