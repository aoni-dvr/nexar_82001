#ifndef UTIL_VSL_RCTA_H
#define UTIL_VSL_RCTA_H


#include "RefFlow_RCTA.h"


/**
* Run drawing RCTA surround reconstruction - vehicle information.
* @param [in] BufWidth     width of buffer domain.
* @param [in] BufHeight    height of buffer domain.
* @param [in] pSRIn        Input surround reconstruction data.
* @param [in/out] pBuffer  a U8 buffer.
* @return Errorcode.
*/
UINT32 UtilVSL_DrawRCTASR(UINT32 BufWidth, UINT32 BufHeight, const AMBA_SR_SCV_CROP_INFO_s *CropInfo, const AMBA_SR_SCV_OBJECT_DATA_LIST_s *pSRIn, UINT8 *pBuffer);

/**
* Run drawing RCTA surround reconstruction - vehicle information.
* @param [in] BufWidth     width of buffer domain.
* @param [in] BufHeight    height of buffer domain.
* @param [in] pWSIn        Input warning system data.
* @param [in/out] pBuffer  a U8 buffer.
* @return Errorcode.
*/
UINT32 UtilVSL_DrawRCTAWS(UINT32 BufWidth, UINT32 BufHeight, const AMBA_SR_SCV_CROP_INFO_s *CropInfo, const AMBA_WS_RCTA_WARNING_INFO_s *pWSIn, UINT8 *pBuffer);

UINT32 UtilVSL_DrawWSMsg(UINT32 BufWidth, UINT32 BufHeight, const AMBA_SR_SCV_CROP_INFO_s *CropInfo, UINT32 DrawX, UINT32 DrawY, const AMBA_WS_RCTA_WARNING_INFO_s *pWSIn, UINT8 *pBuffer);

UINT32 UtilVSL_DrawWarningZone(UINT32 Channel, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SR_SCV_CROP_INFO_s *CropInfo, const AMBA_WS_RCTA_CFG_DATA_s *pWSCfg, const AMBA_SR_SCV_CAL_AVM_CFG_s* CalInfo, UINT8 *pBuffer);

#endif