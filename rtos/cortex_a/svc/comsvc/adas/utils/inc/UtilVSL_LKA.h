#ifndef UTIL_VSL_LKA_H
#define UTIL_VSL_LKA_H


#include "AmbaSR_Lane.h"
#include "AmbaAP_LKA.h"

UINT32 UtilVSL_DrawLKABVSRBase(UINT8 *pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_COMPACT_CAR_SPEC_s* pCarSpec);

UINT32 UtilVSL_DrawLKABVSRLaneInfo(UINT8 *pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SR_LANE_RECONSTRUCT_INFO_s* pLaneInfo);

UINT32 UtilVSL_DrawLKAMultiLane( UINT8 *pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_CAL_EM_CALC_COORD_CFG_s* pCalCfg, const AMBA_SEG_CROP_INFO_s* pCropInfo, const AMBA_SR_LANE_RECONSTRUCT_INFO_s* pLaneInfo);

UINT32 UtilVSL_DrawLKASteeringInfo(UINT8 *pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SEG_CROP_INFO_s* pCropInfo, const AMBA_AP_LKA_RESULT_s* pLkaRes);

/**
 * Draw LKA Predictive Path
 * @param [in] pBuffer    pointer of drawing buffer
 * @param [in] BufWidth   Width of drawing buffer
 * @param [in] BufHeight   Height of drawing buffer
 * @param [in] pCalCfg     Calibration data
 * @param [in] pCropInfo   Crop information from calibration vout domain to drawing buffer
 * @param [in] pLkaRes     LKA data
 * @return UTIL_VSL_ERR_OK or UTIL_VSL_ERR_NG
*/
UINT32 UtilVSL_DrawLKAPredictivePath(UINT8 *pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_CAL_EM_CALC_COORD_CFG_s* pCalCfg, const AMBA_SEG_CROP_INFO_s* pCropInfo, const AMBA_AP_LKA_RESULT_s* pLkaRes);

UINT32 UtilVSL_DrawCanbusInfo(UINT8 *pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SEG_CROP_INFO_s* pCropInfo, const AMBA_AP_LKA_CONFIG_s* pLkaCfg, const AMBA_SR_CANBUS_TRANSFER_DATA_s* pCanbusTrData);

#endif