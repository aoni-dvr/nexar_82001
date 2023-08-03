/**
 *  @file RefFlow_FC.h
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
 *  @details header file of FC reference flow
 *
 */

#ifndef REF_FLOW_FC_H
#define REF_FLOW_FC_H

#include "RefFlow_Common.h"
#include "AmbaTypes.h"
#include "AmbaOD_Tracker.h"
#include "AmbaOD_Filter.h"
#include "AmbaSR_ObjType.h"
#include "AmbaSR_BbxConvDef.h"
#include "AmbaSR_SegPosExt.h"
#include "AmbaWS_FCWS.h"
#include "AmbaWS_FCMD.h"
#include "AmbaAP_ACC.h"
#include "AmbaAP_LCS.h"
#include "AmbaADAS_AEB.h"

/** Define for maximum neural network category number */
#define RF_FC_MAX_NN_CAT_NUM             (20U)

/** Define for SR mode of 2nd stage NN algorithm */
#define RF_FC_SR_MODE_HOR   (AMBA_SR_HOR_MODE)
#define RF_FC_SR_MODE_VER   (AMBA_SR_VER_MODE)

/******************************************************************/
/** Data Type for log                                             */
/******************************************************************/
/** Refer to REF_FLOW_FC_CFG_s (config) */
#define RF_FC_DFMT_RF_CFG_V000           (0x00000600U)            /**< 0x00000600 to 0x000006FF: Reserved for REF_FLOW_FC_CFG_s (config) */
#define RF_FC_DFMT_RF_CFG                (RF_FUNC_FC | RF_FC_DFMT_RF_CFG_V000)

/** Refer to AMBA_WS_FCWS_DATA_s (fcws config) */
#define RF_FC_DFMT_FCWS_CFG_V000         (0x00000700U)            /**< 0x00000700 to 0x000007FF: Reserved for AMBA_WS_FCWS_DATA_s (fcws config) */
#define RF_FC_DFMT_FCWS_CFG              (RF_FUNC_FC | RF_FC_DFMT_FCWS_CFG_V000)

/** Refer to AMBA_WS_FCMD_DATA_s (fcmd config) */
#define RF_FC_DFMT_FCMD_CFG_V000         (0x00000800U)            /**< 0x00000800 to 0x000008FF: Reserved for AMBA_WS_FCMD_DATA_s (fcmd config) */
#define RF_FC_DFMT_FCMD_CFG              (RF_FUNC_FC | RF_FC_DFMT_FCMD_CFG_V000)

/** Refer to AMBA_AP_ACC_CONFIG_s (acc config) */
#define RF_FC_DFMT_ACC_CFG_V000          (0x00000900U)            /**< 0x00000900 to 0x000009FF: Reserved for AMBA_AP_ACC_CONFIG_s (acc config) */
#define RF_FC_DFMT_ACC_CFG               (RF_FUNC_FC | RF_FC_DFMT_ACC_CFG_V000)

/** Refer to AMBA_AP_LCS_CONFIG_s (lcs config) */
#define RF_FC_DFMT_LCS_CFG_V000          (0x00000C00U)            /**< 0x00000C00 to 0x00000CFF: Reserved for AMBA_AP_LCS_CONFIG_s (lcs config) */
#define RF_FC_DFMT_LCS_CFG               (RF_FUNC_FC | RF_FC_DFMT_LCS_CFG_V000)

/** Refer to AMBA_OD_2DBBX_LIST_s (input) */
#define RF_FC_DFMT_2DBBX_IN_V000         (0x00000100U)            /**< 0x00000100 to 0x000001FF: Reserved for AMBA_OD_2DBBX_LIST_s (input) */
#define RF_FC_DFMT_2DBBX_IN              (RF_FUNC_FC | RF_FC_DFMT_2DBBX_IN_V000)
/** Refer to AMBA_SR_CANBUS_TRANSFER_DATA_s (input) */
#define RF_FC_DFMT_CANBUS_TR_V000        (0x00000200U)            /**< 0x00000200 to 0x000002FF: Reserved for AMBA_SR_CANBUS_TRANSFER_DATA_s (input) */
#define RF_FC_DFMT_CANBUS_TR             (RF_FUNC_FC | RF_FC_DFMT_CANBUS_TR_V000)

/** Refer to AMBA_OD_2DBBX_LIST_s (output) */
#define RF_FC_DFMT_2DBBX_OUT_V000        (0x00000300U)            /**< 0x00000300 to 0x000003FF: Reserved for RF_FC_DFMT_2DBBX_OUT_VER (output) */
#define RF_FC_DFMT_2DBBX_OUT             (RF_FUNC_FC | RF_FC_DFMT_2DBBX_OUT_V000)
/** Refer to AMBA_SR_SROBJECT_DATA_s (output) */
#define RF_FC_DFMT_SR_V000               (0x00000400U)            /**< 0x00000400 to 0x000004FF: Reserved for AMBA_SR_SROBJECT_DATA_s (output) */
#define RF_FC_DFMT_SR                    (RF_FUNC_FC | RF_FC_DFMT_SR_V000)
/** Refer to REF_FLOW_FC_OUT_DATA_s (output) */
#define RF_FC_DFMT_FC_OUT_V000           (0x00000501U)            /**< 0x00000500 to 0x000005FF: Reserved for REF_FLOW_FC_OUT_DATA_s (output) */
#define RF_FC_DFMT_FC_OUT                (RF_FUNC_FC | RF_FC_DFMT_FC_OUT_V000)
/** Refer to REF_FLOW_FC_OUT_DATA_V2_s (output) */
#define RF_FC_DFMT_FC_OUT_V2_V000        (0x00000A00U)            /**< 0x00000A00 to 0x00000AFF: Reserved for REF_FLOW_FC_OUT_DATA_V2_s (output) */
#define RF_FC_DFMT_FC_OUT_V2             (RF_FUNC_FC | RF_FC_DFMT_FC_OUT_V2_V000)
/** Refer to AMBA_ADAS_AEB_PROC_OUT_s (output) */
#define RF_FC_DFMT_AEB_OUT_V000          (0x00000D00U)            /**< 0x00000D00 to 0x00000DFF: Reserved for AMBA_ADAS_AEB_PROC_OUT_s (output) */
#define RF_FC_DFMT_AEB_OUT               (RF_FUNC_FC | RF_FC_DFMT_AEB_OUT_V000)

/** Refer to REF_FLOW_FC_SR_IMAGE_3D_MODEL_s (output) */
#define RF_FC_DFMT_FC_SR_IMAGE_3D_OUT_V000        (0x00000B00U)            /**< 0x00000B00 to 0x00000BFF: Reserved for REF_FLOW_FC_SR_IMAGE_3D_MODEL_s (output) */
#define RF_FC_DFMT_FC_SR_IMAGE_3D_OUT             (RF_FUNC_FC | RF_FC_DFMT_FC_SR_IMAGE_3D_OUT_V000)



typedef struct {
    AMBA_CAL_EM_CALC_COORD_CFG_s CalibCfg;                        /**< Calibration config, please refer to AmbaCalib_EmirrorDef.h */
    AMBA_SR_ROI_s ODRoi;                                          /**< Object detection network Roi, coordinate should be same as BBox's coordinate, please refer to AmbaSR_BbxConvDef.h */
    UINT32 ClassNum;                                              /**< Object class number of OD (invalid at current version) */
    AMBA_SR_OBJECT_SIZE_s ObjSize[RF_FC_MAX_NN_CAT_NUM];          /**< Object size of every object class form OD (invalid at current version) */
    AMBA_SR_NN_CAT_TO_OBJ_TYPE_LUT_s NNCat2ObjTypeLut;            /**< Neural network catagory to object type */
    UINT32  TSClock;                                              /**< Clock of time stamp */
    AMBA_COMPACT_CAR_SPEC_s SelfCarSpec;                          /**< Self car specification, please refer to Amba_Vehicles.h; Need WheelBase at current version */
    AMBA_WS_FCWS_DATA_s WsFcwsCfg;                                /**< Config for WS_FCWS, please refer to AmbaWS_FCWS.h */
    AMBA_WS_FCMD_DATA_s WsFcmdCfg;                                /**< Config for WS_FCMD, please refer to AmbaWS_FCMD.h */
    AMBA_AP_ACC_CONFIG_s ApAccCfg;                                /**< Config for AP_ACC, please refer to AmbaAP_ACC.h */
    REF_FLOW_LOG_FP LogFp;                                        /**< Regist callback function for logging data */
} REF_FLOW_FC_CFG_s;

typedef struct {
    AMBA_OD_2DBBX_LIST_s BbxList;                                 /**< Bounding box information from object detection neural network */
    AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusTransData;               /**< Canbus transfer information */
} REF_FLOW_FC_IN_DATA_s;

typedef struct {
    AMBA_WS_FCWS_STATUS_s WsFcwsStat;                             /**< Front collision warning system result */
    AMBA_WS_FCMD_STATUS_s WsFcmdStat;                             /**< Front car motion detection result */
    AMBA_AP_ACC_STATUS_s  ApAccStat;                              /**< Adaptive cruise control result */
} REF_FLOW_FC_OUT_DATA_s;

typedef struct {
    AMBA_CAL_EM_CALC_COORD_CFG_V1_s CalibCfg;                     /**< Calibration config, please refer to AmbaCalib_EmirrorDef.h */
    AMBA_CAL_EM_CALIB_INFO_DATA_V1_s CalibInfo;                   /**< For supporting NN-3D data related work. Advance data for calibration. Generated by AmbaCal_EmGenCalibInfoV1() */
    AMBA_CAL_POINT_DB_2D_s FocalLength;                           /**< For supporting NN-3D data related work. Focal length of lens; unit: pixel */
    AMBA_SR_CFG_PROCESS_OBJ_NUM MaxImprovNum;                     /**< Maximum improve target number for NN-SEG and NN-3D; 0xFFFFFFFF for doing all, 0x0 for doing none */
    AMBA_SR_ROI_s ODRoi;                                          /**< Object detection network Roi, coordinate should be same as BBox's coordinate, please refer to AmbaSR_BbxConvDef.h */
    AMBA_SR_NN_CAT_TO_OBJ_TYPE_LUT_s NNCat2ObjTypeLut;            /**< Neural network catagory to object type */
    UINT32  TSClock;                                              /**< Clock of time stamp */
    AMBA_COMPACT_CAR_SPEC_s SelfCarSpec;                          /**< Self car specification, please refer to Amba_Vehicles.h; Need WheelBase at current version */
    UINT32 NNAlgoSelect;                                          /**< Configure to run designate NN related SR algorithm flow for requested output; refer to RF_FC_SR_MODE_XXX */
    AMBA_WS_FCWS_DATA_s WsFcwsCfg;                                /**< Config for WS_FCWS, please refer to AmbaWS_FCWS.h */
    AMBA_WS_FCMD_DATA_s WsFcmdCfg;                                /**< Config for WS_FCMD, please refer to AmbaWS_FCMD.h */
    AMBA_AP_ACC_CONFIG_s ApAccCfg;                                /**< Config for AP_ACC, please refer to AmbaAP_ACC.h */
    AMBA_AP_LCS_CONFIG_s ApLcsCfg;                                /**< Config for AP_LCS, please refer to AmbaAP_LCS.h */
    AMBA_ADAS_AEB_CFG_s AdasAEBCfg;                               /**< Config for ADAS_AEB, please refer to AmbaADAS_AEB.h */
    REF_FLOW_LOG_FP LogFp;                                        /**< Regist callback function for logging data */
} REF_FLOW_FC_CFG_V2_s;

typedef struct {
    AMBA_OD_2DBBX_LIST_s BbxList;                                 /**< Bounding box information */
    AMBA_SR_CANBUS_TRANSFER_DATA_s Canbus;                        /**< Canbus transfer information */
} REF_FLOW_FC_PRE_PROC_IN_V2_s;

typedef struct {
    AMBA_OD_2DBBX_LIST_s BbxList;                                 /**< Bounding box information with ObjectId */
    AMBA_SR_OP_VALID_DATA_s ValidObj;                             /**< The object ID list which are required to calculate 3D box or instance seg data */
} REF_FLOW_FC_PRE_PROC_OUT_V2_s;

typedef struct {
    UINT32 ObjId;                                                 /**< Object ID */
    UINT32 ObjNNDataFlag;                                         /**< Valid object data type */
    AMBA_SR_OBJ_SEG_INFO ObjNNSegInfo;                            /**< Corresponding object information extract from NN SEG data; Valid when ObjNNDataFlag bit 0 equals to 1 */
    AMBA_OD_3DBBX_s ObjNN3DInfo;                                  /**< Corresponding object information extract from NN 3D data; Valid when ObjNNDataFlag bit 1 equals to 1 */
} REF_FLOW_FC_NN_INFO_s;

typedef struct {
    UINT32 ObjNum;                                                /**< Object number in pObjNNInfoList */
    REF_FLOW_FC_NN_INFO_s *pObjNNInfoList;                        /**< NN information; All object information extract from NN SEG and 3D data of current frame */
} REF_FLOW_FC_NN_DATA_s;

typedef struct {
    AMBA_OD_2DBBX_LIST_s BbxList;                                 /**< Bounding box information from object detection neural network */
    AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusTransData;               /**< Canbus transfer information */
    AMBA_SR_LANE_RECONSTRUCT_INFO_s SRLaneInfo;                   /**< SR lane reconstruction information */
    REF_FLOW_FC_NN_DATA_s *pNNData;                               /**< NN Data; if NULL -> run 2D SR */
} REF_FLOW_FC_IN_DATA_V2_s;

typedef struct {
    AMBA_WS_FCWS_STATUS_s WsFcwsStat;                             /**< Front collision warning system result */
    AMBA_WS_FCMD_STATUS_s WsFcmdStat;                             /**< Front car motion detection result */
    AMBA_AP_ACC_STATUS_s  ApAccStat;                              /**< Adaptive cruise control result */
    AMBA_AP_LCS_RESULT_s  ApLcsResult;                            /**< Lane change suggestion result */
} REF_FLOW_FC_OUT_DATA_V2_s;

typedef struct {
    UINT32 TotalNum;                                              /**< Total object number are identical with TotalNum defined in AMBA_SR_SROBJECT_DATA_s (AmbaSurround.h) */
    AMBA_SR_IMAGE_3D_MODEL_s SrImg3dData[AMBA_SR_MAX_OBJECT_NUM]; /**< Coordinate of model vertices on image, object order are identical with SRObject defined in AMBA_SR_SROBJECT_DATA_s (AmbaSurround.h) */
    UINT32 ObjectId[AMBA_SR_MAX_OBJECT_NUM];                      /**< Object id; object order are identical with AMBA_SR_OBJECT_DATA_s (AmbaSurround.h) */
} REF_FLOW_FC_SR_IMAGE_3D_MODEL_s;

UINT32 RefFlowFC_UpdateCalibData(const AMBA_CAL_EM_CAM_CALIB_DATA_s *pInCalibData);

UINT32 RefFlowFC_GetDefaultCfg(UINT32 *WrkBufSize, REF_FLOW_FC_CFG_s *pRfFcCfg);
UINT32 RefFlowFC_Init(const REF_FLOW_FC_CFG_s *pRfFcCfg, UINT8 *pWrkBufAddr);
UINT32 RefFlowFC_DeInit(void);
UINT32 RefFlowFC_Process(const REF_FLOW_FC_IN_DATA_s *pFcInData, REF_FLOW_FC_OUT_DATA_s *pFcOutData);

UINT32 RefFlowFC_GetDefaultCfgV2(UINT32 *WrkBufSize, REF_FLOW_FC_CFG_V2_s *pRfFcCfg);
UINT32 RefFlowFC_InitV2(const REF_FLOW_FC_CFG_V2_s *pRfFcCfg, UINT8 *pWrkBufAddr);
UINT32 RefFlowFC_DeInitV2(void);
UINT32 RefFlowFC_PreProcessV2(const REF_FLOW_FC_PRE_PROC_IN_V2_s* pPreProcIn, REF_FLOW_FC_PRE_PROC_OUT_V2_s* pPreProcOut);
UINT32 RefFlowFC_ProcessV2(const REF_FLOW_FC_IN_DATA_V2_s *pFcInData, REF_FLOW_FC_OUT_DATA_V2_s *pFcOutData);

#endif  //REF_FLOW_FC_H
