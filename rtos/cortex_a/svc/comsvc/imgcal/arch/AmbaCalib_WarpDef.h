/**
 *  @file AmbaCalib_WarpDef.h
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
#ifndef AMBA_CALIB_WARP_DEF_H
#define AMBA_CALIB_WARP_DEF_H
#include "AmbaTypes.h"
#include "AmbaCalib_WrapDef.h"

#if defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define MAX_WARP_TBL_H_GRID_NUM (256UL)
#define MAX_WARP_TBL_V_GRID_NUM (192UL)
#else
#define MAX_WARP_TBL_H_GRID_NUM (82UL)
#define MAX_WARP_TBL_V_GRID_NUM (70UL)
#endif

#define MAX_WARP_TBL_LEN (MAX_WARP_TBL_H_GRID_NUM * MAX_WARP_TBL_V_GRID_NUM)

typedef struct {
    UINT32 Version;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s CalibSensorGeo;
    UINT32 HorGridNum;
    UINT32 VerGridNum;
    UINT32 TileWidthExp;
    UINT32 TileHeightExp;
    AMBA_CAL_GRID_POINT_s WarpVector[MAX_WARP_TBL_LEN];
} AMBA_CAL_WARP_CALIB_DATA_s;

#endif
