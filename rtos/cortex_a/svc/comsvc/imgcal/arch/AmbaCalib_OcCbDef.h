/**
 *  @file AmbaCalib_OcCbDef.h
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
#ifndef AMBA_CALIB_OCCB_DEF_H
#define AMBA_CALIB_OCCB_DEF_H
#include "AmbaTypes.h"
#include "AmbaCalib_Def.h"
#define MAX_RECT_RADIUS (200U)
#define CORNERS_2D_MAP_RADIUS (100U)

#define CORNER_2D_STATUS_EXPAND_FAIL (-2)
#define CORNER_2D_STATUS_WITHOUT_EXPAND (-1)


#define AMBA_CAL_OCCB_CHESSBOARD (0U)
#define AMBA_CAL_OCCB_CIRCLE_GRID (1U)
#define AMBA_CAL_OCCB_PTN_MAX_WIDTH (100U)
#define AMBA_CAL_OCCB_PTN_MAX_HEIGHT (100U)

typedef struct {
    DOUBLE EstToleranceRate; // 0.3, 0.1 ~ 0.9
    UINT32 CenterSearchRadius; // 20, 1 ~ MAX_RECT_RADIUS
    UINT32 CenterNeighborsSearchRadius; // MAX_RECT_RADIUS, 1 ~ MAX_RECT_RADIUS
    UINT32 SearchRadius; // 20, 1 ~ MAX_RECT_RADIUS
} AMBA_CAL_OCCB_ORGANIZE_CFG_s;

typedef struct {
    AMBA_CAL_U32_4_AXIS_SIZE_s MinSize;
    AMBA_CAL_U32_4_AXIS_SIZE_s MaxSize;
} AMBA_CAL_OCCB_USED_GRID_SIZE_s;

typedef struct {
    UINT32 RowNum;     //Row number
    UINT32 ColNum;     //Column number
} AMBA_CAL_OCCB_CHESS_BOARD_PATTERN_s;

typedef struct {
    UINT32 IsExist :1;  //0: circle does not exist at the grid, 1: exists
    UINT32 Color :4;    //0: black, 1: red, ...
    UINT32 Diameter :16;    //diameter of the circle in mm
} AMBA_CAL_OCCB_CIRLCE_BITS_INFO_s;

typedef struct {
    UINT32 Info;
} AMBA_CAL_OCCB_CIRLCE_INFO_s;

typedef struct {
    UINT32 RowNum; //Circles Row number
    UINT32 ColNum; //Circles Column number
    UINT32 Center2CenterDistance;
    UINT32 ImgRotate;
    AMBA_CAL_POINT_INT_2D_s StartPos;
    AMBA_CAL_OCCB_CIRLCE_INFO_s CircleInfo[AMBA_CAL_OCCB_PTN_MAX_WIDTH * AMBA_CAL_OCCB_PTN_MAX_HEIGHT];
} AMBA_CAL_OCCB_CIRCLE_GRID_PATTERN_s;

typedef struct {
    UINT8 Data[40024U];
} AMBA_CAL_OCCB_PATTERN_UNION_DATA_s;

typedef struct {
    AMBA_CAL_OCCB_PATTERN_UNION_DATA_s Type;
} AMBA_CAL_OCCB_PATTERN_INFO_s;

typedef struct {
    UINT32 Type; /*!< AMBA_CAL_OCCB_CIRCLE_GRID or AMBA_CAL_OCCB_CHESSBOARD*/
} AMBA_CAL_OCCB_BOARD_INFO_s;

typedef struct {
    AMBA_CAL_OCCB_BOARD_INFO_s BoardInfo;
    AMBA_CAL_OCCB_PATTERN_INFO_s PatternInfo;
} AMBA_CAL_OCCB_CALIB_BOARD_s;

#endif
