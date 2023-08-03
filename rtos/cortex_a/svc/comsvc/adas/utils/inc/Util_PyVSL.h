#ifndef UTIL_PY_VSL_H
#define UTIL_PY_VSL_H

#include "AmbaTypes.h"

typedef struct {
    UINT32  CapTS;
    UINT8   CANBusTurnLightStatus;
    UINT8   CANBusGearStatus;
    DOUBLE  TransferSpeed;
    UINT32  WheelDir;
    DOUBLE  TransferWheelAngle;
    DOUBLE  TransferWheelAngleSpeed;
} PY_CANBUS_DATA_s;

typedef struct {
    UINT32 ODRoiX;
    UINT32 ODRoiY;
    UINT32 ODRoiW;
    UINT32 ODRoiH;
} PY_GET_3DBBOX_CFG_s;

/**
* Get image 3D bbox
* @param [in] pCalibBuf      Calibration buffer from UtilPyPC_LoadEmirCalib.
* @param [in] pRoi           OD roi information, the coordinary should be the same with 2Dbbox. Binary of RF_LOG_CROP_INFO_s format.
* @param [in] pPayload3D     3D bbox. Binary of AMBA_OD_3DBBX_s format.
* @param [out] pImg3DBbox    Image 3D bbox, Binary of AMBA_SR_IMAGE_3D_MODEL_s format.
* @return ErrorCode
*/
UINT32 UtilPy_GetImg3DBbox(const void* pCalibBuf, const void* pRoi, const void* pPayload3D, void* pImg3DBbox, UINT32 Img3DBboxSz);

/**
* Get calibration buffer size for UtilPyPC_LoadEmirCalib and UtilPyPC_LoadEmirCalibV1
* @return Calib buffer size
*/
UINT32 UtilPyPC_GetCalibBufSize(void);

/**
* Load Emir Calibration, invalid at current version
* @param [in] Channel         Channel, 0: front, 1: rear, 2: left, 3: right
* @param [in] pCalibFileName  calibration file name
* @param [out] pCalibData     calibration data, the buffer size = UtilPyPC_GetCalibBufSize()
* @return ErrorCode
*/
UINT32 UtilPyPC_LoadEmirCalib(UINT32 Channel, const char* pCalibFileName, void* pCalibBuf);

/**
* Load Emir Calibration V1
* @param [in] Channel         Channel, 0: front, 1: rear, 2: left, 3: right
* @param [in] pCalibFileName  calibration file name
* @param [in] FocalLength     focal length in mm
* @param [out] pCalibData     calibration data, the buffer size = UtilPyPC_GetCalibBufSize()
* @return ErrorCode
*/
UINT32 UtilPyPC_LoadEmirCalibV1(UINT32 Channel, const char* pCalibFileName, DOUBLE FocalLength, void* pCalibBuf);

UINT32 UtilPyVSL_DrawADASResult(char* pEventLog, void* pSrcBuf, void* pBuffer, UINT32 BufWidth, UINT32 BufHeight, void* pLoggerData, UINT32 ArgsNum, void* pCalibBuf, UINT8 CanbusValid, DOUBLE CarSpeed, UINT64 displayFlag);

UINT32 UtilPyVSL_DrawADASResultV2(char* pEventLog, void* pSrcBuf, void* pBuffer, UINT32 BufWidth, UINT32 BufHeight, void* pLoggerData, UINT32 ArgsNum, void* pCalibBuf, UINT8 CanbusValid, PY_CANBUS_DATA_s CanbusData, UINT64 displayFlag);

#endif
