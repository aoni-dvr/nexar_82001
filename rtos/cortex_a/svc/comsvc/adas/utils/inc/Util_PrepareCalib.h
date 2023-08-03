#include "AmbaTypes.h"
#include "AmbaErrorCode.h"
#include "AmbaCalib_EmirrorIF.h"
#include "AmbaCalib_ErrNo.h"
#include "AmbaCalib_AVMIF.h"
#include "Util_Common.h"

#define UTIL_PC_CHANNEL_FRONT   (0x00U)
#define UTIL_PC_CHANNEL_BACK    (0x01U)
#define UTIL_PC_CHANNEL_LEFT    (0x02U)
#define UTIL_PC_CHANNEL_RIGHT   (0x03U)
#define UTIL_PC_CHANNEL_NUM     (0x04U)



/**
* Load Emirror calibration from file
* @param [in] Channel           According to your camera setup, please refer to UTIL_LC_CHANNEL_XXXX
* @param [in] pCalibFileName    Path of calibration file.
* @param [out] pCalibData       Calibration data
* @return ErrorCode
*/
UINT32 UtilPC_LoadEmirCalib(UINT32 Channel, const char* pCalibFileName, AMBA_CAL_EM_CALC_COORD_CFG_s* pCalibData);

/**
* Load Emirror calibration V1 from file
* @param [in] Channel           According to your camera setup, please refer to UTIL_LC_CHANNEL_XXXX
* @param [in] pCalibFileName    Path of calibration file
* @param [in] FocalLength       Focal length depend on lens (For supporting NN-3D data related work), unit: mm
* @param [out] pCalibData       Calibration data V1
* @param [out] pCalibInfo       Advance data for calibration V1 (For supporting NN-3D data related work)
* @param [out] pFocalLength     Focal length of lens (For supporting NN-3D data related work), unit: cellsize
* @return ErrorCode
*/
UINT32 UtilPC_LoadEmirCalibV1(UINT32 Channel, const char* pCalibFileName, DOUBLE FocalLength, AMBA_CAL_EM_CALC_COORD_CFG_V1_s* pCalibData, AMBA_CAL_EM_CALIB_INFO_DATA_V1_s* pCalibInfo, AMBA_CAL_POINT_DB_2D_s *pFocalLength);

/**
* Load AVM calibration from file
* @param [in] Channel           According to your camera setup, please refer to UTIL_LC_CHANNEL_XXXX
* @param [in] pCalibFileName    Path of calibration file.
* @param [out] pCalibData       Calibration data
* @param [out] pPaCfg           Avm park ast config v1
* @return ErrorCode
*/
UINT32 UtilPC_LoadAvmCalib(UINT32 Channel, const char* pCalibFileName, AMBA_CAL_AVM_CALIB_DATA_s *pCalibData, AMBA_CAL_AVM_PARK_AST_CFG_V1_s *pPaCfg);


