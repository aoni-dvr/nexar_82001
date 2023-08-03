/**
 *  @file AmbaCalib_WarpIF.h
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
#ifndef AMBA_CALIB_WARP_IF_H
#define AMBA_CALIB_WARP_IF_H
#include "AmbaTypes.h"
#include "AmbaCalib_WarpDef.h"


typedef struct {
    AMBA_CAL_MODE_CFG_s IKMode;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s CalibSensorGeo;
    UINT32 HorGridNum;
    UINT32 VerGridNum;
    UINT32 TileWidth;
    UINT32 TileHeight;
    UINT32 IsTwoStage;
    AMBA_CAL_GRID_POINT_s *pWarpTbl;
} AMBA_CAL_WARP_FEED_DATA_s;

/**
* This API is used to separate 1 warp table into 2 ones
* Given level is to determine the precision of the 2 output warp tables
* @param [in] pSrcWarpTbl Input warp table. Please refer to  more details. 
* @param [in] TblLength Length of input warp table
* @param [in] Level Range: 0~16.
* Precision of output warp table.
* The lower the level is, the more precise the pDstMainWarpTbl is. The higher the level is, the less precise the pDstSubWarpTbl is. 
* @param [out] pDstMainWarpTbl Main output warp table. Please refer to AMBA_CAL_GRID_POINT_s for more details. 
* @param [out] pDstSubWarpTbl 2nd output warp table. Please refer to AMBA_CAL_GRID_POINT_s for more details. 
* @return ErrorCode
*/
UINT32 AmbaCal_WarpSeparateWarpTbl(const AMBA_CAL_GRID_POINT_s *pSrcWarpTbl,
                                   UINT32 TblLength, UINT32 Level, AMBA_CAL_GRID_POINT_s *pDstMainWarpTbl,
                                   AMBA_CAL_GRID_POINT_s *pDstSubWarpTbl);

/**
* This API is used to merge 2 warp tables into one
* @param [in] pSrcMainWarpTbl Input main output warp table. Please refer to AMBA_CAL_GRID_POINT_s for more details. 
* @param [in] pSrcSubWarpTbl Input 2nd output warp table. Please refer to AMBA_CAL_GRID_POINT_s for more details. 
* @param [in] TblLength Length of input warp table
* @param [out] pDstWarpTbl Output warp table. Please refer to AMBA_CAL_GRID_POINT_s for more details. 
* @return ErrorCode
*/
UINT32 AmbaCal_WarpMergeWarpTbl(const AMBA_CAL_GRID_POINT_s *pSrcMainWarpTbl,
                                const AMBA_CAL_GRID_POINT_s *pSrcSubWarpTbl, UINT32 TblLength,
                                AMBA_CAL_GRID_POINT_s *pDstWarpTbl);

UINT32 AmbaCal_WarpAddTbl(const AMBA_CAL_GRID_POINT_s *pTable1, const AMBA_CAL_GRID_POINT_s *pTable2, UINT32 HorGridNum, UINT32 VerGridNum, AMBA_CAL_GRID_POINT_s *pOutputTable);
#endif
