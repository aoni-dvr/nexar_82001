/**
 *  @file AmbaCalib_CaIF.h
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
#ifndef AMBA_CALIB_CA_IF_H
#define AMBA_CALIB_CA_IF_H
#include "AmbaTypes.h"
#include "AmbaCalib_CaDef.h"

#define AMBA_CAL_CA_DATA_VERSION (1U)
#define AMBA_CAL_CA_DATA_SPEARATE_VERSION (2U)

/**
* This API is used to get ca calibration working buffer size. 
* @param [out] pSize CA working buffer size.
* @return ErrorCode
*/
UINT32 AmbaCal_CaGetBufSize(SIZE_t *pSize);

/**
* This API is used to generate CA compensate tables based on Chromatic aberration spec of lens. 
* @param [in] pCfg Ca configuration. Refer to AMBA_CAL_CA_CFG_s for more details. 
* @param [in] pWorkingBuf Working buffer provided by user
* @param [out] pOutput Returned output settings. Refer to AMBA_CAL_CA_DATA_s for more details. 
* @return ErrorCode
*/
UINT32 AmbaCal_CaGenCaTbl(const AMBA_CAL_CA_CFG_s *pCfg,const void *pWorkingBuf, AMBA_CAL_CA_DATA_s *pOutput);

/**
* This API is to separate Ca data to 2 Ca table(R,B).  
* @param [in] pCaTable Ca data. Please refer to AMBA_CAL_CA_DATA_s for more details. 
* @param [out] pCaRedTbl Ca table content. Returned CA R channel output data. Please refer to AMBA_CAL_GRID_POINT_s for more details. 
* @param [out] pCaBlueTbl Ca table content. Returned CA B channel output data. Please refer to AMBA_CAL_GRID_POINT_s for more details. 
* @return ErrorCode
*/
UINT32 AmbaCal_CaSeparateRBTbl(const AMBA_CAL_CA_DATA_s *pCaTable, AMBA_CAL_GRID_POINT_s *pCaRedTbl, AMBA_CAL_GRID_POINT_s *pCaBlueTbl);

/**
* This API is used to get ca calibration working buffer size for ca separated R/B data. 
* @param [out] pSize CA R/B separated working buffer size
* @return ErrorCode
*/
UINT32 AmbaCal_CaSeparatedRBGetBufSize(SIZE_t *pSize);

/**
* This API is used to generate CA R/B separated tables based on Chromatic aberration spec of lens. 
* @param [in] pCfg Ca configuration. Refer to AMBA_CAL_CA_CFG_s for more details. 
* @param [in] pWorkingBuf “AMBA_CAL_CA_SEPARATE_WORK_BUF_s” Working buffer provided by user
* @param [out] pOutput Returned output settings. Refer to AMBA_CAL_CA_SEPARATE_DATA_s for more details. 
* @return ErrorCode
*/
UINT32 AmbaCal_CaGenSeparatedRBTbl(const AMBA_CAL_CA_CFG_s *pCfg, const void *pWorkingBuf, AMBA_CAL_CA_SEPARATE_DATA_s *pOutput);

#endif
