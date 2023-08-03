/**
 *  @file AmbaImgMainEis.h
 *
 *  Copyright (c) [2020] Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Constants and Definitions for Amba Image Main EIS
 *
 */

#ifndef AMBA_IMG_MAIN_EIS_H
#define AMBA_IMG_MAIN_EIS_H

#define AMBA_IMG_NUM_EIS_CHANNEL 8U
#ifndef AMBA_IMG_NUM_EXPOSURE_CHANNEL
#define AMBA_IMG_NUM_EXPOSURE_CHANNEL AMBA_IMG_SENSOR_HAL_HDR_SIZE
#endif

typedef enum /*_AMBA_IMG_MAIN_EIS_CMD_e_*/ {
    IMG_EIS_TASK_CMD_STOP = 0,
    IMG_EIS_TASK_CMD_START
} AMBA_IMG_MAIN_EIS_CMD_e;

typedef struct /*_AMBA_IMG_MAIN_EIS_USER_DATA_s_*/ {
    UINT32                          UpdateFlag;
    FLOAT                           ShutterTime[AMBA_IMG_NUM_EXPOSURE_CHANNEL];
    AMBA_IK_VIN_ACTIVE_WINDOW_s     ActWin;
    AMBA_IK_DUMMY_MARGIN_RANGE_s    DummyRange;
    AMBA_IK_DZOOM_INFO_s            DZoomInfo;
    AMBA_IK_WARP_INFO_s             WarpInfo;
} AMBA_IMG_MAIN_EIS_USER_DATA_s;

extern UINT32 AmbaImgMainEis_DebugCount[AMBA_IMG_NUM_EIS_CHANNEL];
extern UINT32 AmbaImgMainEis_LastIndex;
#ifndef CONFIG_BUILD_IMGFRW_EIS
typedef struct /*_AMBA_IMG_MAIN_EIS_WARP_PARAM_s_*/ {
    UINT32 XFactor;
    UINT32 YFactor;
} AMBA_IMG_MAIN_EIS_WARP_PARAM_s;

typedef struct /*_AMBA_IMG_MAIN_EIS_DUMMY_PARAM_s_*/ {
    UINT32 XFactor;
    UINT32 YFactor;
} AMBA_IMG_MAIN_EIS_DUMMY_PARAM_s;

typedef struct /*_AMBA_IMG_MAIN_EIS_ZOOM_PARAM_s_*/ {
    UINT32 Factor;
    UINT32 YFactor;
    UINT32 ShiftXFactor;
    UINT32 ShiftYFactor;
     INT32 ShiftXDir;
     INT32 ShiftYDir;
} AMBA_IMG_MAIN_EIS_ZOOM_PARAM_s;

typedef struct /*_AMBA_IMG_MAIN_EIS_PARAM_s_*/ {
    UINT32                             Update;
    AMBA_IMG_MAIN_EIS_ZOOM_PARAM_s     DZoom;
    AMBA_IMG_MAIN_EIS_DUMMY_PARAM_s    DummyRange;
    AMBA_IMG_MAIN_EIS_WARP_PARAM_s     WarpShift;
} AMBA_IMG_MAIN_EIS_PARAM_s;

UINT32 AmbaImgMainEis_ParamUpdatePut(UINT32 EisIdx, const AMBA_IMG_MAIN_EIS_PARAM_s *pParam);

typedef struct /*_AMBA_IMG_MAIN_EIS_WARP_POINT_s_*/ {
    INT32 X;
    INT32 Y;
} AMBA_IMG_MAIN_EIS_WARP_POINT_s;

#define IMG_EIS_WARP_SHIFT_RANDOM    0U
#define IMG_EIS_WARP_SHIFT_MANUAL    1U
#define IMG_EIS_WARP_SHIFT_LINE      2U

typedef struct /*_AMBA_IMG_MAIN_EIS_WARP_SHIFT_s_*/ {
    UINT32                            Update;
    UINT32                            Mode;
    AMBA_IMG_MAIN_EIS_WARP_POINT_s    Vector;
     INT32                            StepFactor;
     INT32                            StepDir;
     INT32                            AccumulatedFactor;
    AMBA_IMG_MAIN_EIS_WARP_POINT_s    Point[2];
} AMBA_IMG_MAIN_EIS_WARP_SHIFT_s;

UINT32 AmbaImgMainEis_WarpUpdatePut(UINT32 EisIdx, const AMBA_IMG_MAIN_EIS_WARP_SHIFT_s *pWarpShift);
#endif
UINT32 AmbaImgMainEis_Init(void);

  void AmbaImgMainEis_Dispatch(UINT32 VinId, UINT64 Param);
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_AC
  void AmbaImgMainEis_DispatchEx(UINT32 VinId, UINT64 Param, UINT64 Param2);
#endif
#endif  /* AMBA_IMG_MAIN_EIS_H */
