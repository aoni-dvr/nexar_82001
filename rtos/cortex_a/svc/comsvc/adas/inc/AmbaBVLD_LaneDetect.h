/**
 *  @file AmbaBVLD_LaneDetect.h
 *
 * Copyright (c) 2019 Ambarella International LP
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
 *  @details Definitions & Constants for Bird's-eye view lane detection API
 *
 */



#ifndef AMBA_BVLD_LANE_DETECT_H
#define AMBA_BVLD_LANE_DETECT_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ADAS_ERROR_CODE_H
#include "AmbaAdasErrorCode.h"
#endif


/** Line type and maximum line number */
#define AMBA_BVLD_LINE_LEFT (0U)
#define AMBA_BVLD_LINE_RIGHT (1U)
#define AMBA_BVLD_LINE_MAX_NUM (2U)

/** Define for LineStat in AMBA_BVLD_LANE_PNT_DATA_s */
#define AMBA_BVLD_LANE_LINE_STAT_NONE (0xFFFFFFFFU)

/** Maximum of line points number */
#define AMBA_BVLD_MAX_PNT_NUM (16U)


typedef struct {
    UINT8* pBufY;       /**< Buffer address of Y data*/
    UINT8* pBufUV;      /**< Buffer address of UV data*/
    UINT16 BufPitch;    /**< Buffer pitch which is the width to change to next image row */
    UINT16 ImgWidth;    /**< Image width */
    UINT16 ImgHeight;   /**< Image height */
} AMBA_BVLD_YUV_DATA_s;


/** Bird's-eye view lane detection process input data */
typedef struct {
    AMBA_BVLD_YUV_DATA_s* pYUVBufInfo;         /**< YUV data buffer information */
    UINT64 TimeStamp;                         /**< Time stamp of yuv data source buffer, invalid at current version */
    INT32 CarSpeed;                           /**< Current car speed in km/hr (set -1 when no speed information) */
} AMBA_BVLD_PROC_INPUT_DATA_s;


typedef struct {
    INT32 X;    /**< Point X at source image domain in pixel */
    INT32 Y;    /**< Point Y at source image domain in pixel */
} AMBA_BVLD_PNT_2D_s;


typedef struct {
    UINT32 PntNum;                                  /**< Total point number to represent a line */
    AMBA_BVLD_PNT_2D_s Pnt2D[AMBA_BVLD_MAX_PNT_NUM]; /**< Points at image domain */
} AMBA_BVLD_LINE_2D_s;


typedef struct {
    DOUBLE X;    /**< Point X at world domain in mm */
    DOUBLE Y;    /**< Point Y at world domain in mm */
    DOUBLE Z;    /**< Point Z at world domain in mm */
} AMBA_BVLD_PNT_3D_s;


typedef struct {
    UINT32 PntNum;                                   /**< Total point number to represent a line */
    AMBA_BVLD_PNT_3D_s Pnt3D[AMBA_BVLD_MAX_PNT_NUM];  /**< Points at world domain */
} AMBA_BVLD_LINE_3D_s;


/** Ego-lane result */
typedef struct {
    UINT64 TimeStamp;                                    /**< Time stamp of lane result, invalid at current version */
    UINT32 LineStat[AMBA_BVLD_LINE_MAX_NUM];              /**< Line status */
    AMBA_BVLD_LINE_2D_s Line2D[AMBA_BVLD_LINE_MAX_NUM];    /**< Line result at bird's-eye view image domain  */
    AMBA_BVLD_LINE_3D_s Line3D[AMBA_BVLD_LINE_MAX_NUM];    /**< Line result at world domain */
} AMBA_BVLD_LANE_PNT_DATA_s;


/** Initial configuration data  */
typedef struct {
    UINT8* pWrkBuf;                           /**< Working buffer address */
    UINT32 WrkBufSize;                        /**< Working buffer size */
    INT32 DashLength;                         /**< Highway dash line length in mm, invalid at current version */
    INT32 DashSpace;                          /**< Highway dash line space in mm */
    UINT32 SrcWidth;                          /**< Source image width */
    UINT32 SrcHeight;                         /**< Source image height */
    INT32 Clock;                              /**< System clock in KHz, invalid at current version */
    DOUBLE Src2WorldMatrix[3][3];             /**< Transformation matrix to convert points from source to world */
} AMBA_BVLD_CFG_DATA_s;



UINT32 AmbaBVLD_Process(const AMBA_BVLD_PROC_INPUT_DATA_s* pInData, AMBA_BVLD_LANE_PNT_DATA_s* pLinePntData);

UINT32 AmbaBVLD_GetDefaultConfig(UINT32 SrcWidth, UINT32 SrcHeight, AMBA_BVLD_CFG_DATA_s* pDefCfgData);

UINT32 AmbaBVLD_SetInitConfig(const AMBA_BVLD_CFG_DATA_s* pCfgData);

UINT32 AmbaBVLD_SetHoughSearchRange(INT32 _tStart, INT32 _tEnd);

UINT32 AmbaBVLD_SetHoughThreshold(INT32 _hfThres);

UINT32 AmbaBVLD_SetLaneMarkerWidth(INT32 _laneMarkerWidth);

UINT32 AmbaBVLD_DisableCheckTwoLine(INT32 disable);

UINT32 AmbaBVLD_DisableHoldSmoothLane(INT32 disable);

#endif
