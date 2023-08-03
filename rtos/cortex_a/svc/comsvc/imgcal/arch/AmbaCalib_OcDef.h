/**
 *  @file AmbaCalib_OcDef.h
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
#ifndef AMBA_CALIB_OC_DEF_H
#define AMBA_CALIB_OC_DEF_H

#include "AmbaCalib_Def.h"
#include "AmbaCalib_Camera.h"

#define MAX_OC_ELLIPSE_RESULT (3U)
#define OC_CAL_FILTER2D_WORKSIZE (4)

#define OC_MAX_RAW_WIDTH (4000)
#define OC_MAX_RAW_HEIGHT (3000)
#define OC_MAX_RAW_SIZE (OC_MAX_RAW_WIDTH*OC_MAX_RAW_HEIGHT)
#define OC_MAX_ELLIPSE_SAMPLE (256U * 1024U)

typedef struct {
    UINT16 *pRaw;
    UINT8 Bayer;                                        /**< Bayer ID */
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    UINT32 MinRadius;                                   /**< minimum ellipse radius to keep */
    AMBA_CAL_SIZE_s CenterLimit;            /**< constraint center shift value */
    UINT8 SplitCircle;                                  /**< number of circle to find */
} AMBA_CAL_OC_CFG_s;

typedef struct {
    UINT32 Version;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s CalibSensorGeo;
    AMBA_CAL_POINT_DB_2D_s Center;
    AMBA_CAL_ELLIPSE_INFO_s Ellipse[MAX_OC_ELLIPSE_RESULT];
} AMBA_CAL_OC_CALIB_DATA_s;

#endif
