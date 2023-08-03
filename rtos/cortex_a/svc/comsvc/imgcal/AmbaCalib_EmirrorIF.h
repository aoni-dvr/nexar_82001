/**
 *  @file AmbaCalib_EmirrorIF.h
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
#ifndef AMBA_CALIB_EMIRROR_API_H
#define AMBA_CALIB_EMIRROR_API_H
#include "AmbaCalib_ErrNo.h"
#include "AmbaCalib_EmirrorDef.h"
#define AMBA_CAL_EM_DATA_VERSION (1U)
#define EMIR_PLUGIN_MODE_MSG_RECIVER (1U)

typedef struct {
    UINT32 Version;
    struct {
        AMBA_CAL_WARP_CALIB_DATA_s WarpTbl;
        AMBA_CAL_ROI_s VoutArea;
        AMBA_CAL_ROTATION_e Rotation;
    } Cam[AMBA_CAL_EM_CAM_MAX];
    AMBA_CAL_EM_BLEND_TBL_s BlendTbl;
    AMBA_CAL_EM_OVERLAP_AREA_s OverlapArea;
} AMBA_CAL_EM_3IN1_DATA_s;

/**
* This API is used to get required working buffer size for 3-in-1 eMirror calculation.
* @param [out] pSize Returned required working buffer size of eMirror 3-in-1 mode for AmbaCal_EmGen3in1View() API.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGet3in1BufSize(SIZE_t *pSize);

/**
* This API is used to generate 3-in-1 eMirror warp tables and blending tables for eMirror application based on user specified view settings and pattern positions.
* The message receiver callback function registered in MsgReciver of structure PlugIn will be callbacked when user specified message types happen.
* @param [in] pCfg 3-in-1 eMirror stitching warp calibration related configuration. Please refer to AMBA_CAL_EM_3IN1_CFG_s for more details.
* @param [in] pWorkingBuf Working buffer provided by user. Required buffer size can be queried by AmbaCal_EmGet3in1BufSize().
* @param [out] pOutput Returned output settings. Please refer to AMBA_CAL_EM_3IN1_DATA_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGen3in1View(const AMBA_CAL_EM_3IN1_CFG_s *pCfg, const void *pWorkingBuf, AMBA_CAL_EM_3IN1_DATA_s *pOutput);

/**
* This API is used to get required working buffer size for single view eMirror warp calculation.
* @param [out] pSize Returned required working buffer size of eMirror single view mode for AmbaCal_EmGenSingleView() API.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGetSingleViewBufSize(SIZE_t *pSize);

/**
* This API is used to generate single view eMirror warp tables and blending tables for eMirror application based on user specified view settings and pattern positions.
* The message receiver callback function registered in MsgReciver of structure PlugIn will be callbacked when user specified message types happen.
* @param [in] pCfg Single view warp calibration configuration. Please refer to AMBA_CAL_EM_SV_CFG_s more details.
* @param [in] pWorkingBuf Working buffer provided by user.
* @param [out] pOutput Returned output settings. Please refer to AMBA_CAL_EM_SV_DATA_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGenSingleView(const AMBA_CAL_EM_SV_CFG_s *pCfg, const void *pWorkingBuf, AMBA_CAL_EM_SV_DATA_s *pOutput);


/**
* This API is used to generate single view eMirror warp tables and blending tables for eMirror application based on user specified view settings and pattern positions.
* The message receiver callback function registered in MsgReciver of structure PlugIn will be callbacked when user specified message types happen.
* @param [in] pCfg Single view warp calibration configuration. Please refer to AMBA_CAL_EM_SV_CFG_V1_s more details.
* @param [in] pWorkingBuf Working buffer provided by user.
* @param [out] pOutput Returned output settings. Please refer to AMBA_CAL_EM_SV_DATA_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGenSingleViewV1(const AMBA_CAL_EM_SV_CFG_V1_s *pCfg, const void *pWorkingBuf, AMBA_CAL_EM_SV_DATA_s *pOutput);

/**
* This API is used to get working buffer size for API AmbaCal_EmGenCalibInfo() to calculate e-mirror calibration information.
* @param [out] pSize Emirror calibration information working buffer size
* @return ErrorCode
*/
UINT32 AmbaCal_EmGetCalibInfoBufSize(SIZE_t *pSize);

/**
* This API is used to generate e-mirror calibration information.
* @param [in] pCfg Configure for e-mirror calibration information calculation. Please refer to AMBA_CAL_EM_CALIB_INFO_CFG_s for more details.
* @param [in] pWorkingBuf Working buffer provided by user.
* @param [out] pOutPut Returned output data. Refer to AMBA_CAL_EM_CALIB_INFO_DATA_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGenCalibInfo(const AMBA_CAL_EM_CALIB_INFO_CFG_s *pCfg, const void *pWorkingBuf, AMBA_CAL_EM_CALIB_INFO_DATA_s *pOutPut);

/**
* This API is used to get working buffer size for API AmbaCal_EmGenCalibInfoV1() to calculate e-mirror calibration information.
* @param [out] pSize Emirror calibration information working buffer size
* @return ErrorCode
*/
UINT32 AmbaCal_EmGetCalibInfoBufSizeV1(SIZE_t *pSize);

/**
* This API is used to generate e-mirror calibration information.
* @param [in] pCfg Configure for e-mirror calibration information calculation. Please refer to AMBA_CAL_EM_CALIB_INFO_CFG_s for more details.
* @param [in] pWorkingBuf Working buffer provided by user.
* @param [out] pOutPut Returned output data. Refer to AMBA_CAL_EM_CALIB_INFO_DATA_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGenCalibInfoV1(const AMBA_CAL_EM_CALIB_INFO_CFG_V1_s *pCfg, const void *pWorkingBuf, AMBA_CAL_EM_CALIB_INFO_DATA_V1_s *pOutPut);

/**
* This API is used to get working buffer size for 3-in-1 stitching e-mirror warp calibration calculation.
* @param [out] pSize 3-in-1 e-mirror calibration working buffer size.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGet3in1VBufSize(SIZE_t *pSize);

/**
* @param [in] pCfg Configure for 3-in-1 e-mirror calibration. Refer to AMBA_CAL_EM_3IN1_GEN_TBL_CFG_s for more details.
* @param [in] pWorkingBuf 3-in-1 e-mirror calibration working buffer size.
* @param [out] pOutput Returned output data. Refer to AMBA_CAL_EM_3IN1_DATA_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGen3in1V(const AMBA_CAL_EM_3IN1_GEN_TBL_CFG_s *pCfg, const void *pWorkingBuf, AMBA_CAL_EM_3IN1_DATA_s *pOutput);

/**
* @param [in] pCfg Configure for 3-in-1 e-mirror calibration. Refer to AMBA_CAL_EM_3IN1_GEN_TBL_CFG_V1_s more details.
* @param [in] pWorkingBuf 3-in-1 e-mirror calibration working buffer size.
* @param [out] pOutput Returned output data. Refer to AMBA_CAL_EM_3IN1_DATA_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGen3in1VV1(const AMBA_CAL_EM3IN1_GEN_TBL_CFG_V1_s *pCfg, const void *pWorkingBuf, AMBA_CAL_EM_3IN1_DATA_s *pOutput);

/**
* @param [in] CamId Camera ID. Refer to AMBA_CAL_EM_CAM_ID_e
* @param [in] pView 3-in-1 e-mirror view related setting.
* @param [out] pTargetVoutArea Corresponding VOUT area information of requested camera.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGet3in1VoutAreaInfo(UINT32 CamId, const AMBA_CAL_EM_3IN1_VIEW_CFG_V1_s *pView, const AMBA_CAL_ROI_s **pTargetVoutArea);

/**
* This API is used to get working buffer size for single view e-mirror warp calibration calculation.
* @param [out] pSize Single view e-mirror calibration working buffer size.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGetSVBufSize(SIZE_t *pSize);

/**
* This API is used to generate single view e-mirror warp calibration data.
* @param [in] pCfg Configure for single view calibration. Refer to AMBA_CAL_EM_SV_GEN_TBL_CFG_s for more details.
* @param [in] pWorkingBuf Single view e-mirror calibration working buffer size.
* @param [out] pOutput Warp table information. Refer to AMBA_CAL_EM_SV_DATA_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGenSV(const AMBA_CAL_EM_SV_GEN_TBL_CFG_s *pCfg, const void *pWorkingBuf, AMBA_CAL_EM_SV_DATA_s *pOutput);

/**
* This API is used to generate single view e-mirror warp calibration data.
* @param [in] pCfg Configure for single view calibration. Refer to AMBA_CAL_EM_SV_GEN_TBL_CFG_V1_s more details.
* @param [in] pWorkingBuf Single view e-mirror calibration working buffer size.
* @param [out] pOutput Warp table information. Refer to AMBA_CAL_EM_SV_DATA_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGenSVV1(const AMBA_CAL_EM_SV_GEN_TBL_CFG_V1_s *pCfg, const void *pWorkingBuf, AMBA_CAL_EM_SV_DATA_s *pOutput);

/**
* This API is used to generate blend table.
* @param [in] pCfg Configure for generating blend table. Please refer to AMBA_CAL_EM_BLEND_TBL_CFG_s for more details.
* @param [out] pOutput Returned blending table data. Please refer to AMBA_CAL_EM_BLEND_TBL_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGenBlendTable(const AMBA_CAL_EM_BLEND_TBL_CFG_s *pCfg, AMBA_CAL_EM_BLEND_TBL_s *pOutput);

/**
* This advanced API is used to get focal length value.
* @param [in] pCfg Configure for convert work. Refer to AMBA_CAL_EM_CALC_COORD_CFG_V1_s for more details.
* (Only support 2 type AMBA_CAL_EM_LDC/ AMBA_CAL_EM_CURVED_SURFACE)
* @param [out] pScaledFocalLength Returned focal length value.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGetScaledFocalLengthV1(const AMBA_CAL_EM_CALC_COORD_CFG_V1_s *pCfg, DOUBLE *pScaledFocalLength);

/**
* This API is used to get focal length value.
* @param [in] pCfg Configure for convert work. Refer to AMBA_CAL_EM_CALC_COORD_CFG_s for more details.
* (Only support 2 type AMBA_CAL_EM_LDC/ AMBA_CAL_EM_CURVED_SURFACE)
* @param [out] pScaledFocalLength Returned focal length value.
* @return ErrorCode
*/
UINT32 AmbaCal_EmGetScaledFocalLength(const AMBA_CAL_EM_CALC_COORD_CFG_s *pCfg, DOUBLE *pScaledFocalLength);

/**
* This advanced API is used to search the horizon position of the image.
* @param [in] pCfg Configure for convert work. Refer to AMBA_CAL_EM_CALC_COORD_CFG_V1_s for more details.
* @param [in] ImgPointX Specified horizons position of image x axis that you would like to get.
* @param [in] SearchStartY The search initial position of the horizon
* @param [in] SearchRadius The search range of the horizon.
* @param [out] pHorizonPositionY Output horizon position of y axis
* @return ErrorCode
*/
UINT32 AmbaCal_EmFindHorizonPositionV1(const AMBA_CAL_EM_CALC_COORD_CFG_V1_s *pCfg,
        UINT32 ImgPointX,
        UINT32 SearchStartY,
        UINT32 SearchRadius,
        UINT32 *pHorizonPositionY);
/**
* This API is used to search the horizon position of the image.
* @param [in] pCfg Configure for convert work. Refer to AMBA_CAL_EM_CALC_COORD_CFG_s for more details.
* @param [in] ImgPointX Specified horizons position of image x axis that you would like to get.
* @param [in] SearchStartY The search initial position of the horizon
* @param [in] SearchRadius The search range of the horizon.
* @param [out] pHorizonPositionY Output horizon position of y axis
* @return ErrorCode
*/
UINT32 AmbaCal_EmFindHorizonPosition(const AMBA_CAL_EM_CALC_COORD_CFG_s *pCfg,
        UINT32 ImgPointX,
        UINT32 SearchStartY,
        UINT32 SearchRadius,
        UINT32 *pHorizonPositionY);

/**
* This advanced API is used to convert a 2D coordinate on image to corresponding 3D coordinate on designated plane in real world according to warp setting.
* @param [in] pCfg Configure for convert work. Refer to AMBA_CAL_EM_CALC_COORD_CFG_V1_s more details.
* @param [out] pImgPoint Point coordinate on image of vout domain. Uint is pixel. Refer to AMBA_CAL_POINT_DB_2D_s for more details.
* @param [in] pCrossPlane Configure 3D plane equation which desired real world is located on. Refer to AMBA_CAL_EM_PLANE_EQUATION_s for more details.
* @param [out] pWorldPoint Output data for coordinate in real wolrd. Unit in mm. Please refer to AMBA_CAL_POINT_DB_3D_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_EmConvPtImgToWorldV1(const AMBA_CAL_EM_CALC_COORD_CFG_V1_s *pCfg,
        const AMBA_CAL_POINT_DB_2D_s *pImgPoint,
        const AMBA_CAL_EM_PLANE_EQUATION_s *pCrossPlane,
        AMBA_CAL_POINT_DB_3D_s *pWorldPoint);

/**
* This advanced API is used to convert a 3D coordinate in real world to corresponding 2D coordinate on image/VOUTdomain according to warp setting.
* @param [in] pCfg Configure for converting work. Please refer to AMBA_CAL_EM_CALC_COORD_CFG_V1_s for more details.
* (Only supports 2 types of AMBA_CAL_EM_LDC/ AMBA_CAL_EM_CURVED_SURFACE)
* @param [in] pWorldPoint Input data for coordinate in real world. Unit is mm. Please refer to AMBA_CAL_POINT_DB_3D_s for more details.
* @param [out] pImgPoint Returned mapping coordinate on VOUT domain of input world coordinate. Unit is pixel. Please refer to AMBA_CAL_POINT_DB_2D_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_EmConvPtWorldToImgV1(const AMBA_CAL_EM_CALC_COORD_CFG_V1_s *pCfg,
        const AMBA_CAL_POINT_DB_3D_s *pWorldPoint,
        AMBA_CAL_POINT_DB_2D_s *pImgPoint);

/**
* This API is used to convert a 2D coordinate on image to corresponding 3D coordinate on designated plane in real world according to warp setting.
* @param [in] pCfg Configure for convert work. Refer to AMBA_CAL_EM_CALC_COORD_CFG_s for more details.
* @param [out] pImgPoint Point coordinate on image of vout domain. Uint is pixel. Refer to AMBA_CAL_POINT_DB_2D_s for more details.
* @param [in] pCrossPlane Configure 3D plane equation which desired real world is located on. Refer to AMBA_CAL_EM_PLANE_EQUATION_s for more details.
* @param [out] pWorldPoint Output data for coordinate in real wolrd. Unit in mm. Please refer to AMBA_CAL_POINT_DB_3D_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_EmConvPtImgToWorldPlane(const AMBA_CAL_EM_CALC_COORD_CFG_s *pCfg,
        const AMBA_CAL_POINT_DB_2D_s *pImgPoint,
        const AMBA_CAL_EM_PLANE_EQUATION_s *pCrossPlane,
        AMBA_CAL_POINT_DB_3D_s *pWorldPoint);

/**
* This API is used to convert a 3D coordinate in real world to corresponding 2D coordinate on image/VOUTdomain according to warp setting.
* @param [in] pCfg Configure for converting work. Please refer to AMBA_CAL_EM_CALC_COORD_CFG_s for more details.
* (Only supports 2 types of AMBA_CAL_EM_LDC/ AMBA_CAL_EM_CURVED_SURFACE)
* @param [in] pWorldPoint Input data for coordinate in real world. Unit is mm. Please refer to AMBA_CAL_POINT_DB_3D_s for more details.
* @param [out] pImgPoint Returned mapping coordinate on VOUT domain of input world coordinate. Unit is pixel. Please refer to AMBA_CAL_POINT_DB_2D_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_EmConvPtWorldToImgPlane(const AMBA_CAL_EM_CALC_COORD_CFG_s *pCfg,
        const AMBA_CAL_POINT_DB_3D_s *pWorldPoint,
        AMBA_CAL_POINT_DB_2D_s *pImgPoint);

#endif
