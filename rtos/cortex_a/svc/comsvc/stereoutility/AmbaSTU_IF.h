/**
 *  @file AmbaSTU_IF.h
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
#ifndef AMBA_STU_IF_H
#define AMBA_STU_IF_H
#include "AmbaTypes.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_EmirrorDef.h"
#include "AmbaSTU_StereoBarCalibInfo.h"
#include "AmbaSTU_ErrNo.h"

typedef struct {
    UINT32 Width;
    UINT32 Height;
} AMBA_STU_SIZE_s;

typedef struct {
    AMBA_STU_SIZE_s Size;
    UINT32 Shift;
    UINT16 *pDisparityTbl;
} AMBA_STU_DSI_INFO_s;

typedef struct {
    INT32 X;
    INT32 Y;
} AMBA_STU_POINT_INT_2D_s;

typedef struct {
    UINT8 ColorLUT[4 * 256];
} AMBA_STU_8BITS_COLOR_PALETTE_s;

typedef struct {
   UINT32 DisparityValShift;
   DOUBLE FocalLength;
   DOUBLE BaseLine;
} AMBA_STU_DSI_TO_DIST_CFG_s;

typedef enum {
    AMBA_STU_OSD_RED = 0,
    AMBA_STU_OSD_GREEN,
    AMBA_STU_OSD_BLUE,
    AMBA_STU_OSD_ALPHA,
    AMBA_STU_OSD_MAX,
} AMBA_STU_OSD_PALETTE_COLOR_s;

typedef struct {
    AMBA_STU_OSD_PALETTE_COLOR_s Color[AMBA_STU_OSD_MAX];
} AMBA_STU_OSD_PALETTE_ORDER_s;

typedef struct {
    UINT32 Width;
    UINT32 Height;
    UINT32 IntegerBitNum;
    UINT32 DecimalBitNum;
    DOUBLE Baseline;
    DOUBLE PixelFocalLength[2];
    AMBA_CAL_POINT_DB_2D_s OpticalCenterOfRightCam;
    AMBA_CAL_EM_PROJECTION_MODEL_e ProjectionModel;
    UINT32 Scale;
} AMBA_STU_PGM_METADATA_S;

typedef struct {
    UINT32 Width;
    UINT32 Height;
    UINT32 Pitch;
    void *pData;
} AMBA_STU_DOF_S;

typedef struct {
    DOUBLE X;
    DOUBLE Y;
    DOUBLE Z;
    DOUBLE Yaw;
    DOUBLE Pitch;
    DOUBLE Roll;
} AMBA_STU_EXTRINSIC_PARAM_S;

typedef struct {
    DOUBLE U0;
    DOUBLE V0;
    DOUBLE Ku;
    DOUBLE Kv;
} AMBA_STU_INTRINSIC_PARAM_S;

/**
* This API is used to convert disparity to distance. 
* @param [in] pCfg The stereo bar and disparity config. Please refer to AMBA_STU_TO_DIST_CFG_s for more details.
* @param [in] DisparityVal Disparity value.
* @param [out] pDistance Distance.
* @return ErrorCode
*/
UINT32 AmbaSTU_Disparity2Distance(const AMBA_STU_DSI_TO_DIST_CFG_s *pCfg, UINT16 DisparityVal, DOUBLE *pDistance);

/**
* This API is used get the default color palette of the disparity map. 
* @param [in] pColorOrder The OSD color palette order. Please refer to AMBA_STU_OSD_PALETTE_ORDER_s for more details.
* @param [out] pColorPalette OSD color palette.
* @return ErrorCode
*/
UINT32 AmbaSTU_GetDefaultColorPalette(const AMBA_STU_OSD_PALETTE_ORDER_s *pColorOrder, AMBA_STU_8BITS_COLOR_PALETTE_s *pColorPalette);

/**
* This API is used to convert 16 bits unpacked disparity data to OSD index. 
* @param [in] pDsiInfo The unpacked disparity data. Please refer to AMBA_STU_DSI_INFO_s for more details.
* @param [in] pColor8BitsIdxLut The look up table that used to convert disparity to the OSD index.
* @param [in] pDsiFeedStartPos The start position of the disparity map. Please refer to AMBA_STU_POINT_INT_2D_s for more details.
* @param [in] pColorTblSize The size of the disparity map. Please refer to AMBA_STU_SIZE_s for more details.
* @param [out] pColorTbl OSD color index map.
* @return ErrorCode
*/
UINT32 AmbaSTU_ConvDsi16BitsToColorIdx(const AMBA_STU_DSI_INFO_s *pDsiInfo,
        const UINT32 *pColor8BitsIdxLut,
        const AMBA_STU_POINT_INT_2D_s *pDsiFeedStartPos,
        const AMBA_STU_SIZE_s *pColorTblSize,
        UINT8 *pColorTbl);

/**
* This API is used to convert 10 bits packed disparity data to 16 bits unpacked disparity data. 
* @param [in] pIn10BitsTbl The packed disparity data.
* @param [in] Width The disparity map width.
* @param [out] pOut16BitsTbl The unpacked disparity data.
* @return ErrorCode
*/
UINT32 AmbaSTU_Unpack10BitsTo16Bits(const UINT8 *pIn10BitsTbl, UINT32 Width, UINT16 *pOut16BitsTbl);

/**
* This API is used get the warp table. 
* @param [in] pStereoCamWarpInfo The stereo calibration data.
* @param [out] pWarpTbl The warp table.
* @return ErrorCode
*/
UINT32 AmbaSTU_GetWarpTblAddr(const AMBA_STU_STEREO_CAM_WARP_INFO_s *pStereoCamWarpInfo, AMBA_IK_GRID_POINT_s **pWarpTbl);

/**
* This API is used to convert disparity map to PGM file format. 
* @param [in] pMetaData The meta data. Please refer to AMBA_STU_PGM_METADATA_S for more details.
* @param [in] pUnpackedDsiTbl The unpacked disparity map.
* @param [in] DsiTblSize The disparity map size.
* @param [out] pPGMDataBuf The PGM file format data.
* @param [out] pPGMDataSize The PGM file format data size.
* @return ErrorCode
*/
UINT32 AmbaSTU_DsiToPGM(const AMBA_STU_PGM_METADATA_S *pMetaData, const UINT16 *pUnpackedDsiTbl, UINT32 DsiTblSize, UINT8 *pPGMDataBuf, UINT32 *pPGMDataSize);

/**
* This API is used to get the PGM header size. 
* @param [out] pMaxHeaderSize The PGM header size.
* @return ErrorCode
*/
UINT32 AmbaSTU_GetPGMMaxHeaderSize(UINT32 *pMaxHeaderSize);

/**
* This API is used to convert dense optical flow map to ARGB buffer. 
* @param [in] pOf Input dense optical flow. Please refer to AMBA_STU_DOF_S for more details.
* @param [out] pArgbDataBuf Address of ARGB buffer. The content is arranged as [A0][R0][G0][B0], [A1][R1][G1][B1] ..., 8 bits per element. Value of alpha channel is remained the same during processing.
* @return ErrorCode
*/
UINT32 AmbaSTU_DofToRgb(const AMBA_STU_DOF_S *pOf, UINT8 *pArgbDataBuf);

/**
* This API is used to get optical center of target camera from stereo warp table. 
* @param [in] pStereoCamWarpInfo The stereo warp table.
* @param [out] pU0 U0.
* @param [out] pV0 V0.
* @return ErrorCode
*/
UINT32 AmbaSTU_GetTargetOpticalCenter(const AMBA_STU_STEREO_CAM_WARP_INFO_s *pStereoCamWarpInfo, DOUBLE *pU0, DOUBLE *pV0);

/**
* This API is used to get intrinsic parameter of target camera from stereo warp table. 
* @param [in] pStereoCamWarpInfo The stereo warp table.
* @param [out] pIntParam Intrinsic parameter.
* @return ErrorCode
*/
UINT32 AmbaSTU_GetTargetIntrinsicParam(const AMBA_STU_STEREO_CAM_WARP_INFO_s *pStereoCamWarpInfo, AMBA_STU_INTRINSIC_PARAM_S *pIntParam);

/**
* This API is used to get baseline of stereo camera from stereo warp table. 
* @param [in] pStereoCamWarpInfo The stereo warp table.
* @param [out] pBaseline Stereo baseline.
* @return ErrorCode
*/
UINT32 AmbaSTU_GetStereoBaseline(const AMBA_STU_STEREO_CAM_WARP_INFO_s *pStereoCamWarpInfo, DOUBLE *pBaseline);

/**
* This API is used to get extrinsic parameter of source camera from stereo warp table. 
* @param [in] pStereoCamWarpInfo The stereo warp table.
* @param [out] pExtParam Extrinsic parameter.
* @return ErrorCode
*/
UINT32 AmbaSTU_GetSourceExtrinsicParam(const AMBA_STU_STEREO_CAM_WARP_INFO_s *pStereoCamWarpInfo, AMBA_STU_EXTRINSIC_PARAM_S *pExtParam);


#endif
