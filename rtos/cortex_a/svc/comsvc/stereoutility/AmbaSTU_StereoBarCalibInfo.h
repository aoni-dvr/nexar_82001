/**
 *  @file AmbaSTU_StereoBarCalibInfo.h
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
#ifndef AMBA_STU_STEREO_BAR_CALIB_INFO_H
#define AMBA_STU_STEREO_BAR_CALIB_INFO_H
#include "AmbaDSP_ImageFilter.h"
#include "AmbaCalib_WarpDef.h"
#include "AmbaCalib_StereoIF.h"
typedef struct {
    UINT8 Version[4]; /**< Contains 4 bytes, currently 2.2.0.0 */
    UINT32 HeaderOffset; /**< offset in bytes to the beginning of the header */
    UINT32 TableOffset; /**< offset in bytes to the beginning of the warp table */
    UINT32 ExifOffset; /**< offset in bytes to the beginning of the JSON EXIF data */
} AMBA_STU_WARP_INFO_HEADER_BASE_t;

typedef struct {
    UINT32 Enable2Pass:1;/**< 0 1-pass, 1 2-pass */
    UINT32 Reserved:31;
} AMBA_STU_WARP_MODE_t;

typedef struct {
    UINT32 Enable2Pass:1;/**< 0 1-pass, 1 2-pass */
    UINT32 OriginPosition:1;/** 0: on left-upper corner of calib image; 1: on left-upper corner of whole sensor */
    UINT32 NewCropVsyncMode:1;/** 0: old firmware, vsync delay <= 12; 1: new, diff of start y must be vsync delay, vsync delay <= MAX */
    UINT32 Reserved: 29;
} AMBA_STU_WARP_MODE_V3_t;

typedef struct {
    UINT16 HorizontalTilesNum;         /**< number of horizontal tiles */
    UINT16 VerticalTilesNum;           /**< number of vertical tiles */
    UINT32 TileWidthQ16;               /**< tile width, in 16.16 fixed point format */
    UINT32 TileHeightQ16;              /**< tile height, in 16.16 fixed point format */
    AMBA_STU_WARP_MODE_t WarpMode;                    /**< IDSP warp unit behavior */
    UINT32 Id;
    UINT32 Hash;                         /**< pearson16 hash of the table samples */
    UINT32 TableOriginX;               /**< w.r.t. the image used for calibration */
    UINT32 TableOriginY;               /**< w.r.t. the image used for calibration */
    UINT16 VsyncDelay;                  /**< delay [in rows] to apply to the vsync signal for this sensor */
    UINT16 Reserved;
    UINT32 InputWidth;                  /**< input image width */
    UINT32 InputHeight;                 /**< input image height */
} AMBA_STU_WARP_INFO_HEADER_2100_t;

typedef struct {
    UINT16 HorizontalTilesNum;         /**< number of horizontal tiles */
    UINT16 VerticalTilesNum;           /**< number of vertical tiles */
    UINT32 TileWidthQ16;               /**< tile width, in 16.16 fixed point format */
    UINT32 TileHeightQ16;              /**< tile height, in 16.16 fixed point format */
    AMBA_STU_WARP_MODE_t WarpMode;     /**< IDSP warp unit behavior */
    UINT32 Id;
    UINT32 Hash;                       /**< pearson16 hash of the table samples */
    UINT32 TableOriginX;               /**< w.r.t. the image used for calibration */
    UINT32 TableOriginY;               /**< w.r.t. the image used for calibration */
    UINT16 ReservedVsyncDelay;         /**< delay [in rows] to apply to the vsync signal for this sensor */
    UINT16 Reserved;
    UINT32 InputWidth;                  /**< input image width */
    UINT32 InputHeight;                 /**< input image height */
    UINT16 ReservedAE[4];               /**< auto exposure roi */
} AMBA_STU_WARP_INFO_HEADER_2200_t;

typedef struct {
    UINT16 HorizontalTilesNum;         /**< number of horizontal tiles */
    UINT16 VerticalTilesNum;           /**< number of vertical tiles */
    UINT32 TileWidthQ16;               /**< tile width, in 16.16 fixed point format */
    UINT32 TileHeightQ16;              /**< tile height, in 16.16 fixed point format */
    AMBA_STU_WARP_MODE_t WarpMode;     /**< IDSP warp unit behavior */
    UINT32 Id;
    UINT32 Hash;                       /**< pearson16 hash of the table samples */
    UINT32 TableOriginX;               /**< w.r.t. the image used for calibration */
    UINT32 TableOriginY;               /**< w.r.t. the image used for calibration */
    UINT16 ReservedVsyncDelay;         /**< delay [in rows] to apply to the vsync signal for this sensor */
    UINT16 Reserved;
    UINT32 InputWidth;                  /**< input image width */
    UINT32 InputHeight;                 /**< input image height */
    UINT16 ReservedAE[4];               /**< auto exposure roi */
    UINT32 UuId[8];                     /**< LENS module UUID */
} AMBA_STU_WARP_INFO_HEADER_2300_t;

typedef struct {
    UINT16 HorizontalTilesNum;         /**< number of horizontal tiles */
    UINT16 VerticalTilesNum;           /**< number of vertical tiles */
    UINT32 TileWidthQ16;               /**< tile width, in 16.16 fixed point format */
    UINT32 TileHeightQ16;              /**< tile height, in 16.16 fixed point format */
    AMBA_STU_WARP_MODE_V3_t WarpMode;     /**< IDSP warp unit behavior */
    UINT32 Id;
    UINT32 Hash;                       /**< pearson16 hash of the table samples */
    UINT32 TableOriginX;               /**< w.r.t. the image used for calibration */
    UINT32 TableOriginY;               /**< w.r.t. the image used for calibration */
    UINT16 ReservedVsyncDelay;         /**< delay [in rows] to apply to the vsync signal for this sensor */
    UINT16 Reserved;
    UINT32 InputWidth;                  /**< input image width */
    UINT32 InputHeight;                 /**< input image height */
    UINT16 ReservedAE[4];               /**< auto exposure roi */
    UINT32 UuId[8];                     /**< LENS module UUID */
} AMBA_STU_WARP_INFO_HEADER_2310_t;


typedef struct {
    AMBA_STU_WARP_INFO_HEADER_BASE_t HBase;
    AMBA_STU_WARP_INFO_HEADER_2100_t Header;
    AMBA_IK_GRID_POINT_s WarpTable[MAX_WARP_TBL_LEN];
} AMBA_STU_WARP_INFO_2100_t;

typedef struct {
    AMBA_STU_WARP_INFO_HEADER_BASE_t HBase;
    AMBA_STU_WARP_INFO_HEADER_2200_t Header;
    AMBA_IK_GRID_POINT_s WarpTable[MAX_WARP_TBL_LEN];
} AMBA_STU_WARP_INFO_2200_t;

typedef struct {
    AMBA_STU_WARP_INFO_HEADER_BASE_t HBase;
    AMBA_STU_WARP_INFO_HEADER_2300_t Header;
    AMBA_IK_GRID_POINT_s WarpTable[MAX_WARP_TBL_LEN];
} AMBA_STU_WARP_INFO_2300_t;

typedef struct {
    AMBA_STU_WARP_INFO_HEADER_BASE_t HBase;
    AMBA_STU_WARP_INFO_HEADER_2310_t Header;
    AMBA_IK_GRID_POINT_s WarpTable[MAX_WARP_TBL_LEN];
} AMBA_STU_WARP_INFO_2310_t;

typedef struct {
    union {
        UINT8 Version[4];
        AMBA_STU_WARP_INFO_2100_t V2100;
        AMBA_STU_WARP_INFO_2200_t V2200;
        AMBA_STU_WARP_INFO_2300_t V2300;
        AMBA_STU_WARP_INFO_2310_t V2310;
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV2FS)
        AMBA_CAL_ST_DATA_s VAst;
#endif
    } Method;
} AMBA_STU_STEREO_CAM_WARP_INFO_s;


#endif
