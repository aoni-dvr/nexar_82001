/**
 *  @file AmbaSRRef_Flow.h
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
 *
 *  @details Inline function definitions
 *
 */
#ifndef AMBASRREF_FLOW_H_
#define AMBASRREF_FLOW_H_

#include <AmbaOD_Filter.h>
#include <AmbaOD_Tracker.h>
#include <AmbaSR_SegPosExt.h>
#include <AmbaSR_3DPosExt.h>

#define AMBA_SRREF_FCWS_HDLR_INTI_CFG_s AMBA_SRREF_HDLR_INTI_CFG_V1_s
#define AMBA_SRREF_FCWS_NN_INFO_s AMBA_SRREF_NN_INFO_s
#define AMBA_SRREF_FCWS_NN_DATA_s AMBA_SRREF_NN_DATA_s
#define AMBA_SRREF_FCWS_PROC_RESULT_s AMBA_SRREF_PROC_RESULT_s
#define AmbaSRRef_FcwsHdlrInit AmbaSRRef_HdlrInitV1
#define AmbaSRRef_FcwsDoPreProcess AmbaSRRef_DoPreProcess
#define AmbaSRRef_FcwsDoProcess AmbaSRRef_DoProcess

#define AMBA_SRREF_BSD_HDLR_INTI_CFG_s AMBA_SRREF_HDLR_INTI_CFG_V1_s
#define AMBA_SRREF_BSD_NN_INFO_s AMBA_SRREF_NN_INFO_s
#define AMBA_SRREF_BSD_NN_DATA_s AMBA_SRREF_NN_DATA_s
#define AMBA_SRREF_BSD_PROC_RESULT_s AMBA_SRREF_PROC_RESULT_s
#define AmbaSRRef_BsdHdlrInit AmbaSRRef_HdlrInitV1
#define AmbaSRRef_BsdDoPreProcess AmbaSRRef_DoPreProcess
#define AmbaSRRef_BsdDoProcess AmbaSRRef_DoProcess

typedef enum {
    AMBA_SRREF_FUNC_DO_TRACKER = 0, /**< Get bbx data after tracker. */
    AMBA_SRREF_FUNC_DO_FILTER,      /**< Get bbx data after tracker and filter. */
    AMBA_SRREF_FUNC_DO_ALL,         /**< Get result of above. */
    AMBA_SRREF_FUNC_MAX,
} AMBA_SRREF_FUNC_e;

typedef struct {
    UINT32 ObjType;                /**< Object type. */
    AMBA_SR_OBJECT_SIZE_s *pSize;  /**< Object 3D size. */
} AMBA_SRREF_OBJ_MODEL_INFO_s;

typedef struct {
    UINT32 ListElementNum;              /**< Element number in list. */
    AMBA_SRREF_OBJ_MODEL_INFO_s *pList; /**< Object model list. */
} AMBA_SRREF_OBJ_MODEL_LIST_s;

typedef struct {
    UINT32 AppType;                                /**< App type. Value define in AmbaSR_ObjType.h */
    UINT32 CamPosID;                               /**< Invalid: 0. Value define in AmbaSR_ObjType.h */
    UINT32 FilterMode;                             /**< Filter, OdRing, ObsrvBuf. None:0 */
    DOUBLE FilterDist;                             /**< 0 for default */
    DOUBLE TrackerSigmaIOU;                        /**< 0~1. Suggest 0.5 */
    AMBA_CAL_EM_CALC_COORD_CFG_s CalibData;        /**< Data for Coordinate transformation. */
    AMBA_SR_NN_CAT_TO_OBJ_TYPE_LUT_s *pObjTypeLUT; /**< Object type look up table. */
    AMBA_SRREF_OBJ_MODEL_LIST_s ObjModelList;      /**< Object model look up list. */
    UINT32 SelfCarWheelBase;                       /**< Wheel base, unit: mm */
    UINT32 TSClock;                                /**< Clock Info used to calculate system time from Time tick */
    UINT32 ErrTolerantNum[1];                      /**< Maximum number of internal error can be ignored */
} AMBA_SRREF_HDLR_INTI_CFG_s;

typedef struct {
    UINT32 AppType;                                /**< App type. Value define in AmbaSR_ObjType.h */
    UINT32 CamPosID;                               /**< Invalid: 0. Value define in AmbaSR_ObjType.h */
    UINT32 FilterMode;                             /**< Filter, OdRing, ObsrvBuf. None:0 */
    DOUBLE FilterDist;                             /**< 0 for default */
    DOUBLE TrackerSigmaIOU;                        /**< 0~1. Suggest 0.5 */
    AMBA_CAL_EM_CALC_COORD_CFG_V1_s CalibData;     /**< Setting for Coordinate transformation. */
    AMBA_SR_NN_CAT_TO_OBJ_TYPE_LUT_s *pObjTypeLUT; /**< Object type look up table. */
    AMBA_SRREF_OBJ_MODEL_LIST_s ObjModelList;      /**< Object model look up list. */
    UINT32 SelfCarWheelBase;                       /**< Wheel base, unit: mm */
    UINT32 TSClock;                                /**< Clock Info used to calculate system time from Time tick */
    UINT32 ErrTolerantNum[1];                      /**< Maximum number of internal error can be ignored */
    AMBA_SR_ROI_s ODRoi;                           /**< OD Roi */
#define SRREF_NN_ALGO_DEFAULT (0x0U)               /**< Run default flow (NN-3D Hor mode) */
#define SRREF_NN_ALGO_3D_VER (0x1U)                /**< Run flow of NN-3D Ver mode */
    UINT32 NNAlgoSelect;                           /**< Configure to run designate NN related SR algorithm flow for requested output */
#define SRREF_IMPROV_ALL_OBJ (0xFFFFFFFFU)         /**< Improve all object */
    AMBA_SR_CFG_PROCESS_OBJ_NUM MaxImprovNum;      /**< Maximum improve target number for NN-SEG and NN-3D. SRREF_IMPROV_ALL_OBJ for default */
    AMBA_CAL_POINT_DB_2D_s FocalLength;            /**< For supporting NN-3D data related work. Unit is pixel. Focal length of lens */
    AMBA_CAL_EM_CALIB_INFO_DATA_V1_s CalibInfo;    /**< For supporting NN-3D data related work. Advance data for calibration. Generated by AmbaCal_EmGenCalibInfoV1() */
} AMBA_SRREF_HDLR_INTI_CFG_V1_s;

/* RealTime Update Data */
typedef struct {
    AMBA_OD_2DBBX_LIST_s *pBbxList;                 /**< Necessary. For Tracker & ObjPos */
    AMBA_SR_ROI_s *pBbxDetROI;                      /**< Necessary. BBX detection ROI setting */
    AMBA_SR_CANBUS_TRANSFER_DATA_s *pSrCanbusTrans; /**< Optional. Pointer to the Buffer that buffers input transferred CANBus */
} AMBA_SRREF_HDLR_DATA_UPDATE_s;

typedef struct {
    struct {
        AMBA_OD_2DBBX_LIST_s *pDoTrackerResult; /**< Address of result data of TRACKER stage if user request TRACKER or it would be invalid */
        AMBA_OD_2DBBX_LIST_s *pDoFilterResult;  /**< Address of result data of FILTER stage if user request FILTER or it would be invalid */
    } ResultBuffer;
} AMBA_SRREF_FUNC_RESULT_s;

typedef struct {
    UINT32 ObjId;                          /**< Object ID */
#define SRREF_NN_SEG_FLAG_ENBALE 0x01U
#define SRREF_NN_3D_FLAG_ENBALE 0x02U
    UINT32 ObjNNDataFlag;                  /**< Valid object data type */
    AMBA_SR_OBJ_SEG_INFO ObjNNSegInfo;     /**< Corresponding object information extract from NN SEG data */
    AMBA_OD_3DBBX_s ObjNN3DInfo;           /**< Corresponding object information extract from NN 3D data */
} AMBA_SRREF_NN_INFO_s;

typedef struct {
    UINT32 ObjNum;                             /**< Object number in pObjNNInfoList */
    AMBA_SRREF_NN_INFO_s *pObjNNInfoList; /**< All object information extract from NN SEG and 3D data of current frame */
} AMBA_SRREF_NN_DATA_s;

typedef struct {
    AMBA_SR_SROBJECT_DATA_s *pSrData;          /**< SR data list */
    AMBA_SR_IMAGE_3D_MODEL_s *pImgSrDataList;  /**< Array for storing coordinate of object model vertices on image. Objects order and number are identical with data in pSrData */
} AMBA_SRREF_PROC_RESULT_s;

/**
* Get working buffer size per SR-Reference handler.
* @param [out] pSize working buffer size.
* @return ErrorCode
*/
UINT32 AmbaSRRef_GetHdlrWorkBufSize(SIZE_t *pSize);

/**
* Initialize SR-Reference handler.
* @param [in]  pCfg initialize setting.
* @param [in]  pWorkingBuf working buffer for running handler. User can get buffer size by AmbaSRRef_GetHdlrWorkBufSize()
* @param [out] pOutHdlr address of request SR-Reference handler
* @return ErrorCode
*/
UINT32 AmbaSRRef_HdlrInit(const AMBA_SRREF_HDLR_INTI_CFG_s *pCfg, const void *pWorkingBuf, void **pOutHdlr);

/**
* Delete SR-Reference handler. Available for all SR-Reference handler
* @param [in]  pHdlr target handler.
* @return ErrorCode
*/
UINT32 AmbaSRRef_HdlrUninit(const void *pHdlr);

/**
* Update object data for running SR algorithm
* @param [in]  pInHandler target SR-Reference handler.
* @param [in]  pInData object data for updating
* @return ErrorCode
*/
UINT32 AmbaSRRef_UpdateObjData(const void *pInHandler, const AMBA_SRREF_HDLR_DATA_UPDATE_s* pInData);

/**
* Running tracker algorithm to processing object data in SR-Reference handler.
* @param [in]  pInHandler target SR-Reference handler.
* @return ErrorCode
*/
UINT32 AmbaSRRef_DoTracker(const void *pInHandler);

/**
* Running filter algorithm to processing object data in SR-Reference handler.
* @param [in]  pInHandler target SR-Reference handler.
* @return ErrorCode
*/
UINT32 AmbaSRRef_DoFilter(const void *pInHandler);

/**
* Calculate world position data of objects.
* @param [in]  pInHandler target SR-Reference handler.
* @param [out] pOut3DObjectList address of 3D information array of all object
* @param [out] pOutObjListElementNum element number of pOut3DObjectList array
* @return ErrorCode
*/
UINT32 AmbaSRRef_CalcObjPos(const void *pInHandler,
                            AMBA_SR_WORLD_3D_OBJECT_s **pOut3DObjectList,
                            UINT32 *pOutObjListElementNum);

/**
* Calculate speed information of objects.
* @param [in]  pInHandler target SR-Reference handler.
* @param [out] pOutSrData address of SR data array of all object
* @return ErrorCode
*/
UINT32 AmbaSRRef_CalcObjSpeed(const void *pInHandler, AMBA_SR_SROBJECT_DATA_s **pOutSrData);

/**
* Get function result of DoTracker or DoFilter.
* @param [in]  pInHandler target SR-Reference handler.
* @param [in]  ReqFunc target function
* @param [out] pFuncResult data address of request API's result
* @return ErrorCode
*/
UINT32 AmbaSRRef_GetFuncResult(const void *pInHandler, AMBA_SRREF_FUNC_e ReqFunc, AMBA_SRREF_FUNC_RESULT_s *pFuncResult);

/**
* Update calibration data for coordinate transformation.
* @param [in]  pInHandler target SR-Reference handler.
* @param [in]  pInCalibData data for updating
* @return ErrorCode
*/
UINT32 AmbaSRRef_UpdateCalibData(const void *pInHandler, const AMBA_CAL_EM_CAM_CALIB_DATA_s *pInCalibData);

/**
* Initialize SR-Reference handler for FCWS project. Support refined algorithm with NN-SEG and NN-3D data
* @param [in]  pCfg initialize setting.
* @param [in]  pWorkingBuf Working buffer for running handler. User can get buffer size by AmbaSRRef_GetHdlrWorkBufSize()
* @param [out] pOutHdlr address of request SR-Reference handler
* @return ErrorCode
*/
UINT32 AmbaSRRef_HdlrInitV1(const AMBA_SRREF_HDLR_INTI_CFG_V1_s *pCfg, const void *pWorkingBuf, void **pOutHdlr);

/**
* Calculate priority of objects.
* @param [in]  pInHandler target SR-Reference handler.
* @param [out] pOutValidData priority rank of object
* @return ErrorCode
*/
UINT32 AmbaSRRef_DoPreProcess(const void *pInHandler, AMBA_SR_OP_VALID_DATA_s **pOutValidData);

/**
* This API would run DoTracker, DoFilter, CalcObjPos then CalcObjSpeed.
* By calling AmbaSRRef_FcwsDoPreProcess() before and feeding NN-Seg or NN-3D data, algorithm can work with better precision.
* @param [in]  pInHandler target SR-Reference handler.
* @param [in]  pObjNNInfoDataArry NN objects data for updating. Assign NULL if no NN-Seg and NN-3D data.
* @param [out] pOutSrData address of SR data array of all object
* @return ErrorCode
*/
UINT32 AmbaSRRef_DoProcess(const void *pInHandler, const AMBA_SRREF_NN_DATA_s *pObjNNInfoDataArry, AMBA_SRREF_PROC_RESULT_s *pOutSrData);

#endif /* AMBASRREF_FLOW_H_ */
