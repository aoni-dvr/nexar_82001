
#ifndef UTIL_VSL_FC_H
#define UTIL_VSL_FC_H

#include "AmbaTypes.h"
#include "AmbaOD_2DBbx.h"
#include "AmbaSurround.h"
#include "AmbaWS_FCWS.h"
#include "AmbaWS_FCMD.h"
#include "AmbaAP_ACC.h"
#include "RefFlow_FC.h"

UINT32 UtilVSL_DrawFCWSPosWZ(UINT32 Width, UINT32 Height, const AMBA_CAL_EM_CALC_COORD_CFG_s *pCalCfg, const AMBA_WS_FCWS_DATA_s *pFcwsCfg,
                             const AMBA_SR_CANBUS_TRANSFER_DATA_s *pTransData, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);
UINT32 UtilVSL_DrawFcwsWzV2(UINT32 Width, UINT32 Height, const AMBA_SEG_CROP_INFO_s* pCropInfo, const AMBA_CAL_EM_CALC_COORD_CFG_V1_s* pCalCfg, const AMBA_SR_CANBUS_TRANSFER_DATA_s *pTransData,
                            const AMBA_WS_FCWS_DATA_s *pFcwsCfg, const AMBA_SR_LANE_MEMBER_s* pLaneInfo, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

UINT32 UtilVSL_DrawFCMDPosWZ(UINT32 Width, UINT32 Height, const AMBA_CAL_EM_CALC_COORD_CFG_s *pCalCfg, const AMBA_WS_FCMD_DATA_s *pFcmdCfg,
                             const AMBA_SR_CANBUS_TRANSFER_DATA_s *pTransData, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);
UINT32 UtilVSL_DrawFcmdWzV2(UINT32 Width, UINT32 Height, const AMBA_CAL_EM_CALC_COORD_CFG_V1_s *pCalCfg, const AMBA_WS_FCMD_DATA_s *pFcmdCfg,
                            const AMBA_SR_CANBUS_TRANSFER_DATA_s *pTransData, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

UINT32 UtilVSL_DrawFCSR(UINT32 Width, UINT32 Height, const AMBA_SR_SROBJECT_DATA_s *pSRIn, const AMBA_OD_2DBBX_LIST_s *pBbxIn, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);
UINT32 UtilVSL_DrawFCSRV2(UINT32 Width, UINT32 Height, const AMBA_SR_SROBJECT_DATA_s *pSRIn, const AMBA_OD_2DBBX_LIST_s *pBbxIn, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

UINT32 UtilVSL_DrawFCWS(UINT32 Width, UINT32 Height, const AMBA_OD_2DBBX_LIST_s *pBbxIn, const AMBA_SR_SROBJECT_DATA_s *pSRIn, const AMBA_WS_FCWS_STATUS_s *pFCWSIn, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

UINT32 UtilVSL_DrawFCMD(UINT32 Width, UINT32 Height, const AMBA_WS_FCMD_STATUS_s *pFCMDIn, const AMBA_OD_2DBBX_LIST_s *pBbxIn, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);
UINT32 UtilVSL_DrawFcmdV2(UINT32 Width, UINT32 Height, const AMBA_WS_FCMD_DATA_s *pFcmdCfg, const AMBA_SR_CANBUS_TRANSFER_DATA_s *pTransData, const AMBA_WS_FCMD_STATUS_s *pFCMDIn, const AMBA_OD_2DBBX_LIST_s *pBbxIn, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

UINT32 UtilVSL_DrawFCBEVSelf(const UTIL_VSL_BEV_CFG_s *pBevCfg, const AMBA_SR_CANBUS_TRANSFER_DATA_s *pCanBusTrData, const AMBA_WS_FCWS_DATA_s *pFCWSData, UINT8 *pBuffer);

UINT32 UtilVSL_DrawFCBEVWS(const UTIL_VSL_BEV_CFG_s *pBevCfg, const AMBA_OD_2DBBX_LIST_s *pBbxIn, const AMBA_SR_SROBJECT_DATA_s *pSRIn, const AMBA_WS_FCWS_STATUS_s *pFCWSIn, UINT8 *pBuffer);

UINT32 UtilVSL_DrawFcwsBevWz(const UTIL_VSL_BEV_CFG_s *pBevCfg, const AMBA_SR_CANBUS_TRANSFER_DATA_s *pCanBusTrData, const AMBA_WS_FCWS_DATA_s *pFcwsCfg, const AMBA_SR_LANE_MEMBER_s *pLaneData, UINT8 *pBuffer);

UINT32 UtilVSL_DrawFcmdBevWz(const UTIL_VSL_BEV_CFG_s *pBevCfg, const AMBA_SR_CANBUS_TRANSFER_DATA_s *pCanBusTrData, const AMBA_WS_FCMD_DATA_s *pFcmdCfg, UINT8 *pBuffer);

UINT32 UtilVSL_DrawFcwsBev(const UTIL_VSL_BEV_CFG_s *pBevCfg, const AMBA_SR_SROBJECT_DATA_s *pSRIn, const AMBA_WS_FCWS_STATUS_s *pFcwsData, UINT8 *pBuffer);

UINT32 UtilVSL_DrawFcmdBev(const UTIL_VSL_BEV_CFG_s *pBevCfg, const AMBA_SR_SROBJECT_DATA_s *pSRIn, const AMBA_WS_FCMD_STATUS_s *pFcmdData, UINT8 *pBuffer);

UINT32 UtilVSL_DrawACC(UINT32 Width, UINT32 Height, const AMBA_SR_CANBUS_TRANSFER_DATA_s *pCanBusTrData, DOUBLE SetSpeed, const AMBA_AP_ACC_STATUS_s *pACCIn, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

#endif //UTIL_VSL_FC_H