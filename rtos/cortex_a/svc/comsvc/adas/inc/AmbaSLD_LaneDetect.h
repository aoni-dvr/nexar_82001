/**
 *  @file AmbaSLD_LaneDetect.h
 *
 * Copyright (c) 2018 Ambarella International LP
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
 *
 *  @details Definitions & Constants for Segmentation lane detection API
 *
 */



#ifndef AMBA_SLD_LANE_DETECT_H
#define AMBA_SLD_LANE_DETECT_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ADAS_ERROR_CODE_H
#include "AmbaAdasErrorCode.h"
#endif
#ifndef AMBA_CALIB_EMIRROR_API_H
#include "AmbaCalib_EmirrorDef.h"
#endif


/** Line type and maximum line number */
#define AMBA_SLD_LINE_LEFT    (0U)
#define AMBA_SLD_LINE_RIGHT   (1U)
#define AMBA_SLD_LINE_MAX_NUM (2U)

/** Category for lane detection */
#define AMBA_SLD_TYPE_LANE    (0U)
#define AMBA_SLD_TYPE_CURB    (1U)

/** Define for LineStat in AMBA_SLD_LANE_PNT_DATA_s */
#define AMBA_SLD_LANE_LINE_STAT_NONE (0xFFFFFFFFU)

/** Maximum of line points number */
#define AMBA_SLD_MAX_PNT_NUM (32U)

/** Maximum of idx list number */
#define AMBA_SLD_MAX_IDX_LIST (32U)

/** The label indicating that cannot assure the lane type*/
#define UNCERTAIN_LANETYPE (-1)

typedef struct {
    UINT8* pBuf;     /**< Buffer address */
    UINT32 Pitch;    /**< Buffer pitch which is the width to change to next image row */
    UINT32 Width;    /**< Image width */
    UINT32 Height;   /**< Image height */
} AMBA_SLD_BUF_INFO_s;

typedef struct {
    UINT32 SrcW;    /**< Source image width */
    UINT32 SrcH;    /**< Source image height */
    UINT32 RoiW;    /**< ROI width at source image */
    UINT32 RoiH;    /**< ROI height at source image */
    UINT32 RoiX;    /**< ROI X offset at source image */
    UINT32 RoiY;    /**< ROI Y offset at source image */
} AMBA_SLD_CROP_INFO_s;

/** Segmentation lane detection process input data */
typedef struct {
    AMBA_SLD_BUF_INFO_s* pSegBufInfo;         /**< Segmentation buffer information */
    AMBA_SLD_CROP_INFO_s* pSegBufCropInfo;    /**< Segmentation buffer crop information */
    UINT64 TimeStamp;                         /**< Time stamp of segmentation buffer */
    INT32 CarSpeed;                           /**< Current car speed in km/hr (set -1 when no speed information) */
} AMBA_SLD_PROC_INPUT_DATA_s;


typedef struct {
    INT32 X;    /**< Point X at source image domain in pixel */
    INT32 Y;    /**< Point Y at source image domain in pixel */
} AMBA_SLD_PNT_2D_s;


typedef struct {
    UINT32 PntNum;                                  /**< Total point number to represent a line */
    AMBA_SLD_PNT_2D_s Pnt2D[AMBA_SLD_MAX_PNT_NUM];  /**< Points at image domain */
} AMBA_SLD_LINE_2D_s;


typedef struct {
    DOUBLE X;    /**< Point X at world domain in mm */
    DOUBLE Y;    /**< Point Y at world domain in mm */
    DOUBLE Z;    /**< Point Z at world domain in mm */
} AMBA_SLD_PNT_3D_s;


typedef struct {
    UINT32 PntNum;                                   /**< Total point number to represent a line */
    AMBA_SLD_PNT_3D_s Pnt3D[AMBA_SLD_MAX_PNT_NUM];   /**< Points at world domain */
} AMBA_SLD_LINE_3D_s;


/** Ego-lane result */
typedef struct {
    UINT64 TimeStamp;                                    /**< Time stamp of lane result */
    UINT32 LineStat[AMBA_SLD_LINE_MAX_NUM];              /**< Line status */
    INT16 LaneType[AMBA_SLD_LINE_MAX_NUM];               /**< Lane type */
    AMBA_SLD_LINE_2D_s Line2D[AMBA_SLD_LINE_MAX_NUM];    /**< Line result at segmentation image domain  */
    AMBA_SLD_LINE_3D_s Line3D[AMBA_SLD_LINE_MAX_NUM];    /**< Line result at world domain */
} AMBA_SLD_LANE_PNT_DATA_s;


typedef struct {
    INT32 IdxNum;                            /**< Valid lane mark index number */
    UINT8 IdxList[AMBA_SLD_MAX_IDX_LIST];    /**< Valid lane mark index in segmentation buffer */
    UINT8 TypeList[AMBA_SLD_MAX_IDX_LIST];   /**< Category of the lane mark index */
} AMBA_SLD_SEG_IDX_INFO;

/** Initial configuration data  */
typedef struct {
    UINT8* pWrkBuf;                           /**< Working buffer address */
    UINT32 WrkBufSize;                        /**< Working buffer size */
    INT32 DashLength;                         /**< Highway dash line length in mm, invalid at current version */
    INT32 DashSpace;                          /**< Highway dash line space in mm */
    UINT32 SegBufImgW;                        /**< Segmentation image width. If it doesn't match pSegBufInfo->Width, pSegBufInfo->Width will be downscale to CFG size */
    UINT32 SegBufImgH;                        /**< Segmentation image height. If it doesn't match pSegBufInfo->Height, pSegBufInfo->Height will be downscale to CFG size */
    INT32 Clock;                              /**< System clock in KHz */
    AMBA_SLD_SEG_IDX_INFO LaneMarkIdxInfo;    /**< Lane mark index in segmentation image */
} AMBA_SLD_CFG_DATA_s;



UINT32 AmbaSLD_Process(const AMBA_SLD_PROC_INPUT_DATA_s* pInData, AMBA_SLD_LANE_PNT_DATA_s* pLinePntData);

UINT32 AmbaSLD_GetDefaultConfig(UINT32 SegBufImgW, UINT32 SegBufImgH, AMBA_SLD_CFG_DATA_s* pDefCfgData);

UINT32 AmbaSLD_SetInitConfig(const AMBA_SLD_CFG_DATA_s* pCfgData, const AMBA_CAL_EM_CALC_COORD_CFG_s* pCalCfg);

#endif
