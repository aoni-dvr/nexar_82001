/*
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
#ifndef CVAPI_FLEXIDAG_AMBAMVAC_DEF_H
#define CVAPI_FLEXIDAG_AMBAMVAC_DEF_H

#include "cvapi_flexidag.h"
#include "cvapi_flexidag_ambavo_def_cv2.h"

typedef struct {
    uint16_t           HorizontalGridNumber;
    uint16_t           VerticalGridNumber;
    uint32_t           TileWidthQ16;
    uint32_t           TileHeightQ16;
} AMBA_CV_MVAC_WARP_TABLE_INFO_s;

typedef struct {
    AMBA_CV_MVAC_WARP_TABLE_INFO_s WarpTableInfo;
    uint32_t ScaleId; ///< The id of the half octave used for extracting features.
} AMBA_CV_MVAC_VIEW_INFO_s;


#define AMBA_CV_MVAC_MODE_CONFIG   0U
#define AMBA_CV_MVAC_MODE_CONTROL  1U

#define AMBA_CV_MVAC_CONTROL_STOP  0U
#define AMBA_CV_MVAC_CONTROL_START 1U

/**
 * @brief The structure contains the control parameters of the AutoCalib task.
 */
typedef struct {
    uint32_t Mode;                      //0:config mode 1:control mode
    //config parameters
    AMBA_CV_MVAC_VIEW_INFO_s View;
    uint32_t MaxIterations; ///< The maximum number of iterations that the internal minimizator can complete.
    float MinEpipolarScoreThreshold;    //Default: 0. Valid range: [0, 2]
    float MaxFrameSpeedThreshold;       //Default: 0.5. Valid range: [0.1, 0.6]
    uint32_t MinNumDepthThreshold;      //Default: 200. Valid range: [100, 800]
    uint32_t InhibitedFrameSlotSize;    //Default: 300. Valid range: [0, max]
    AMBA_CV_META_BUF_s *pInMetaLeft;
    //control parameters
    uint32_t ControlState;                     //0:Stop 1:Start
} AMBA_CV_MVAC_CFG_s;

/******************************************************************************/
/* OUTPUT ARM MVAC                                                               */
/******************************************************************************/
/**
 * @brief The output of the Multi View Auto Calib task which presents some statistics.
 */
typedef struct {
    uint16_t NumStereoMatch; ///< The number of stereo matches from the FMA task.
    uint16_t NumFlowMatch; ///< The number of flow matches from the FMA task.
    uint32_t NumStereoModelGenerated; ///< The number of models generated from the internal estimator.
    float    VerticalMisalignment; ///< The average vertical misalignment of corresponding features in the stereo image, considering only the last run.
} AMBA_CV_MVAC_STATISTICS_s;

#define AMBA_CV_MVAC_WARP_TABLE_MAXSIZE (128 * 96 * 2)

typedef struct {
    unsigned short              WarpTableValid;
    unsigned short              MetaCalibValid;
    unsigned short              WarpTable[AMBA_CV_MVAC_WARP_TABLE_MAXSIZE];
    AMBA_CV_META_DATA_s         MetaCalib;
} __attribute__((packed)) AMBA_CV_MVAC_WARP_TBL_s;

typedef struct {
    unsigned int                 Reserved;
    AMBA_CV_MVAC_WARP_TBL_s      CalibrationUpdates[2]; // 0 : left ; 1 : right
} __attribute__((packed)) AMBA_CV_MVAC_WARP_INFO_s;

#define AMBA_CV_MVAC_STATE_RESET        0x0U
#define AMBA_CV_MVAC_STATE_WAIT         0x1U
#define AMBA_CV_MVAC_STATE_ACCUMULATION 0x2U
#define AMBA_CV_MVAC_STATE_SOLVE        0x3U
#define AMBA_CV_MVAC_STATE_END          0x4U
#define AMBA_CV_MVAC_STATE_SKIP         0x5U

typedef struct {
    uint32_t State; ///RESET, WAIT, ACCUMULATION, SOLVE, END
    uint32_t NumRun; ///< The number of runs.
    uint32_t NumWarpInfoSent; ///< The number of models sent out for updating the warp tables.
    AMBA_CV_MVAC_STATISTICS_s ViewStatistic;
    AMBA_CV_MVAC_WARP_INFO_s *pWarpInfo; //NULL when no warp table output
} AMBA_CV_MVAC_DATA_s;

typedef struct {
    AMBA_CV_MVAC_DATA_s        *pBuffer;
    ULONG                       BufferDaddr;
    UINT32                      BufferCacheable :1;
    UINT32                      BufferSize      :31;
    ULONG                       BufferCaddr;
    UINT32                      Reserved;
} AMBA_CV_MVAC_BUF_s;

#endif //CVAPI_FLEXIDAG_AMBAMVAC_DEF_H
