/**
 * @file AmbaSR_BbxConv.h
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
 *  @details AmbaSR_Bbx_Conv
 *
 */
#ifndef AMBA_SR_BBS_CONV_H
#define AMBA_SR_BBS_CONV_H

#include "AmbaTypes.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_EmirrorIF.h"
#include "AmbaOD_2DBbx.h"
#include "AmbaSR_BbxConvDef.h"
#include "AmbaSR_NNPose3DDef.h"

#define SR_BBX_CONV_OK (0)
#define SR_BBX_CONV_ERR_1 (1)


#define AMBA_BBX2WD_ALGO_PASSTHROUGH (0x1U) ///< BBX to World 3D model algorithm: passthrough
#define AMBA_BBX2WD_ALGO_COMBINE_PROJ_FL (0x2U)
#define AMBA_BBX2WD_ALGO_PASSTHROUGH_EXT (0x3U) ///< BBX to World 3D model algorithm: passthrough and refine corner bbx

typedef struct {
    AMBA_OD_2DBBX_s *pPassthrough; /**< 2D bounding box information. It would be used when the AlgoMode is AMBA_BBX2WD_ALGO_PASSTHROUGH*/
    AMBA_SR_COMBINE_PROJ_FOCAL_LEN_s *pCombineProjAndFocalLen;
    AMBA_SR_PASSTHROUGH_EST_s *pPassthroughEst; /**< 2D bounding box information. It would be used when the AlgoMode is AMBA_BBX2WD_ALGO_PASSTHROUGH_EXT*/
    //ooo_s *pDepthInfo;
} AMBA_SR_2D_BBX_CONV_INFO_s;

typedef struct {
    UINT32 AlgoMode; /**< Algorithm mode, please refer to AMBA_BBX2WD_ALGO_ */
    AMBA_SR_2D_BBX_CONV_INFO_s Info; /**< Bounding box information, please refer to AMBA_SR_2D_BBX_CONV_INFO_s*/
} AMBA_SR_2D_BBX_TO_WD_3D_CFG_s;


/**
* Update the object size into the SR Conv data base, that would be use to predict the car position.
* @param [in] ObjCat object catergory.
* @param [in] pSize object size, please refer to AMBA_SR_OBJECT_SIZE_s.
* @return ErrorCode
*/
UINT32 AmbaSR_SetObjectSize(UINT32 ObjCat, const AMBA_SR_OBJECT_SIZE_s *pSize);

/**
* Reset the object size into of the SR Conv data base.
*/
void AmbaSR_ClearObjectSize(void);

/**
* Convert 2D bounding box of the image domain to 3D model of the world domain function.
* @param [in]  pBbxCfg bounding box information.
* @param [in]  pCalibCfg camera calibration information.
* @param [out] p3DObject 3D model information.
* @return ErrorCode
*/
UINT32 AmbaSR_Conv2dBbxToWorld3DObj(const AMBA_SR_2D_BBX_TO_WD_3D_CFG_s *pBbxCfg, const AMBA_CAL_EM_CALC_COORD_CFG_s *pCalibCfg, AMBA_SR_WORLD_3D_OBJECT_s *p3DObject);

/**
* This API is unused any more.
* @param [out] pSize This API is unused.
* @return ErrorCode
*/
UINT32 AmbaSR_GetObjPosEstBufSize(SIZE_t *pSize);

/**
* This API is unused any more.
* @param [in] pWorkingBuf This API is unused.
* @return ErrorCode
*/
UINT32 AmbaSR_InitObjPosEst(const void *pWorkingBuf);

/**
* Estimate the object position, it would be remove soon, please use the AmbaSR_EstObjPosV1 function.
* @param [in]  pObjInfo object info, please refer to AMBA_SR_OBJ_INFO_s.
* @param [in]  pCalibCfg calibration info, please refer to AMBA_CAL_EM_CALC_COORD_CFG_s.
* @param [out] p3DObject 3D model info, please refer to AMBA_SR_WORLD_3D_OBJECT_s.
* @return ErrorCode
*/
UINT32 AmbaSR_EstObjPos(const AMBA_SR_OBJ_INFO_s *pObjInfo, const AMBA_CAL_EM_CALC_COORD_CFG_s *pCalibCfg, AMBA_SR_WORLD_3D_OBJECT_s *p3DObject);

/**
* Estimate the object position.
* @param [in]  pObjInfo object info, please refer to AMBA_SR_OBJ_INFO_s.
* @param [in]  pCalibCfg calibration info, please refer to AMBA_CAL_EM_CALC_COORD_CFG_V1_s.
* @param [out] p3DObject 3D model info, please refer to AMBA_SR_WORLD_3D_OBJECT_s.
* @return ErrorCode
*/
UINT32 AmbaSR_EstObjPosV1(const AMBA_SR_OBJ_INFO_s *pObjInfo, const AMBA_CAL_EM_CALC_COORD_CFG_V1_s *pCalibCfg, AMBA_SR_WORLD_3D_OBJECT_s *p3DObject);

/**
* Get NN 3d Pose converter buffer size.
* @return buffer size.
*/
SIZE_t AmbaSR_GetNNPose3DBufSize(void);

/**
* Init NN 3d pose converter.
* @param [in]  pInitCfg init config, please refer to AMBA_SR_NN_POSE_3D_INIT_s.
* @param [in]  pWorkingBuf working buffer.
* @param [out] pHandler handler of NN 3d pose converter.
* @return ErrorCode
*/
UINT32 AmbaSR_InitNNPose3D(const AMBA_SR_NN_POSE_3D_INIT_s *pInitCfg, const void *pWorkingBuf, void **pHandler);

/**
* Convert 2D bbx to 3D bbx.
* @param [in]  pHandler handler of NN 3d pose converter.
* @param [in]  pObjInfo object info, please refer to AMBA_SR_NN_3D_OBJ_INFO_s.
* @param [out] p3DImgObj project the 3D bbx to the image, please refer to AMBA_SR_IMAGE_3D_MODEL_s.
* @param [out] p3DWorldObj 3D model info, please refer to AMBA_SR_WORLD_3D_OBJECT_s.
* @return ErrorCode
*/
UINT32 AmbaSR_NNRot2Dto3DBbx(const void *pHandler, const AMBA_SR_NN_3D_OBJ_INFO_s *pObjInfo, AMBA_SR_IMAGE_3D_MODEL_s *p3DImgObj, AMBA_SR_WORLD_3D_OBJECT_s *p3DWorldObj);

#endif //AMBA_SR_BBS_CONV_H
