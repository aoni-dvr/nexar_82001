/**
 * @file Amba_AwbCalib.h
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *
 */
 #ifndef AMBA_AWB_CAL_H
#define AMBA_AWB_CAL_H

#include "AmbaImg_Proc.h"

#define WB_CAL_LHCT_RESET   0U
#define WB_CAL_LCT_DONE     1U
#define WB_CAL_HCT_DONE     2U

#define WB_CAL_TEST         0U
#define WB_CAL_LCT          1U
#define WB_CAL_HCT          2U

#define WB_CAL_STORE_LCT    5U
#define WB_CAL_STORE_HCT    6U

#define WB_CAL_GET_LCT      9U
#define WB_CAL_GET_HCT      10U

#define WB_CAL_RESET        98U
#define WB_CAL_INVALID      99U

#define WB_CAL_FRAMES       12U

typedef struct  {
    AMBA_IK_WB_GAIN_s   target_gain[2]; // 2 for LCT and HCT
    AMBA_IK_WB_GAIN_s   set_gain[2];    // 2 for LCT and HCT
    AMBA_IK_WB_GAIN_s   delta[2];       // 2 for LCT and HCT
} WB_COMP_INFO_s;


typedef struct {
    UINT32 CurR;            /**< Current red strength */
    UINT32 CurG;            /**< Current green strength */
    UINT32 CurB;            /**< Current blue strength */
    UINT32 RefR;            /**< Reference red strength */
    UINT32 RefG;            /**< Reference red strength */
    UINT32 RefB;            /**< Reference red strength */

}  WB_Detect_Info_s;

typedef struct {
    AMBA_IK_CFA_AWB_s           AWBStatistic[AMBA_IK_3A_AWB_TILE_ROW_COUNT * AMBA_IK_3A_AWB_TILE_COL_COUNT];
    AMBA_IK_3A_HEADER_s         Header;
} WB_CAL_DATA_S;

typedef struct {
    UINT32                  WbCalibEnable;
    WB_CAL_DATA_S           WbCalibData[WB_CAL_FRAMES];

} WB_CALIB_INFO_s;

UINT32 ImgProc_WbCalibCalculateWbCompInfo(UINT32 ViewID);
UINT32 ImgProc_WbCalibTargetToSet(UINT32 ViewID, AMBA_IK_WB_GAIN_s *pWbGain);
UINT32 ImgProc_WBCalibSetToTarget(UINT32 ViewID, AMBA_IK_WB_GAIN_s *pWbGain);
UINT32 ImgProc_WBCal(UINT32 ViewID, UINT32 Index, UINT32 FlickerMode, WB_Detect_Info_s *WbDetectInfo);
UINT32 ImgProc_WbCalibCtrl(UINT32 ViewID);
UINT32 ImgProc_WbCalibSetEnable(UINT32 ViewID, UINT32 Enable);
UINT32 ImgProc_WbCalibGetEnable(UINT32 ViewID, UINT32 *pEnable);

#endif