
#ifndef UTIL_VSL_BSD_H
#define UTIL_VSL_BSD_H

#include "AmbaTypes.h"
#include "AmbaOD_2DBbx.h"
#include "AmbaSurround.h"
#include "AmbaWS_BSD.h"
#include "RefFlow_BSD.h"



/**
* Run drawing BSD warning zone.
* @param [in] width of bounding box domain.
* @param [in] height of bounding box domain.
* @param [in] pCalCfg: Calibration configure data.
* @param [in] pCanbusTrData: Canbus data.
* @param [in] pBsdCfg: Self warning zone data.
* @param [in] width of buffer domain.
* @param [in] height of buffer domain.
* @param [in/out] a uint8 buffer.
* @return Errorcode.
*/
UINT32 UtilVSL_DrawBSDWZ(UINT32 Width, UINT32 Height, const AMBA_CAL_EM_CALC_COORD_CFG_s *pCalCfg, const AMBA_SR_CANBUS_TRANSFER_DATA_s* pCanbusTrData, const AMBA_WS_BSD_DATA_s *pBsdCfg, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

/**
* Run drawing BSD bounding box.
* @param [in] width of bounding box domain (source domain).
* @param [in] height of bounding box domain (source domain).
* @param [in] pBbxIn: Input bounding box data.
* @param [in] width of buffer domain.
* @param [in] height of buffer domain.
* @param [in/out] address of uint8 buffer.
* @return Errorcode.
*/
UINT32 UtilVSL_DrawBSDBbox(UINT32 Width, UINT32 Height, const AMBA_OD_2DBBX_LIST_s *pBbxIn, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

/**
* Run drawing BSD surround reconstruction system function.
* @param [in] width of bounding box domain.
* @param [in] height of bounding box domain.
* @param [in] pSRIn: Input surround reconstruction data.
* @param [in] pBbxIn: Input bounding box data.
* @param [in] width of buffer domain.
* @param [in] height of buffer domain.
* @param [in/out] address of uint8 buffer.
* @return Errorcode.
*/
UINT32 UtilVSL_DrawBSDSR(UINT32 Width, UINT32 Height, const AMBA_SR_SROBJECT_DATA_s *pSRIn, const AMBA_OD_2DBBX_LIST_s *pBbxIn, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

/**
* Run drawing BSD warning icon system function.
* @param [in] width of bounding box domain.
* @param [in] height of bounding box domain.
* @param [in] pBbxIn: Input bounding box data.
* @param [in] pBSDWSIn: Input warning situation data for BSD warning system.
* @param [in] width of buffer domain.
* @param [in] height of buffer domain.
* @param [in/out] address of uint8 buffer.
* @return Errorcode.
*/
UINT32 UtilVSL_DrawBSDWS(UINT32 Width, UINT32 Height, const AMBA_WS_BSD_STATUS_s *pBSDWSIn, const AMBA_OD_2DBBX_LIST_s *pBbxIn, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

/**
* Run drawing BSD bird-eye-view(BEV) self car system.
* @param [in] pBSDBevCfg: BEV configure for BSD system.
* @param [in] pCanBusTrData: CanBus transform data.
* @param [in] pBSDWSData: BSD warning system data (Warning zone data & Self car size).
* @param [in/out] a U8 buffer.
* @return Errorcode.
*/
UINT32 UtilVSL_DrawBSDBEVSelf(const UTIL_VSL_BEV_CFG_s *pBSDBevCfg, const AMBA_SR_CANBUS_TRANSFER_DATA_s *pCanBusTrData, const AMBA_WS_BSD_DATA_s *pBSDWSData, UINT8 *pBuffer);

/**
* Run drawing BSD bird-eye-view(BEV) SR system.
* @param [in] pBSDBevCfg: BEV configure for BSD system.
* @param [in] pBbxIn: Bounding box data.
* @param [in] pSRIn: Surround reconstruction data.
* @param [in/out] a U8 buffer.
* @return Errorcode.
*/
UINT32 UtilVSL_DrawBSDBEVSR(const UTIL_VSL_BEV_CFG_s *pBevCfg, const AMBA_OD_2DBBX_LIST_s *pBbxIn, const AMBA_SR_SROBJECT_DATA_s *pSRIn, UINT8 *pBuffer);

/**
* Run drawing BSD BEV warning system.
* @param [in] pBSDBevCfg: BEV configure for BSD system.
* @param [in] pBbxIn: Bounding box data.
* @param [in] pSRIn: Surround reconstruction data.
* @param [in] pBSDWSIn: BSD warning situation data.
* @param [in/out] a U8 buffer.
* @return Errorcode.
*/
UINT32 UtilVSL_DrawBSDBEVWS(const UTIL_VSL_BEV_CFG_s *pBevCfg, const AMBA_OD_2DBBX_LIST_s *pBbxIn, const AMBA_SR_SROBJECT_DATA_s *pSRIn, const AMBA_WS_BSD_STATUS_s *pBSDWSIn, UINT8 *pBuffer);



#endif //UTIL_VSL_BSD_Hs