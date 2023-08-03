#ifndef UTIL_VLS_RMG_H
#define UTIL_VLS_RMG_H

#include "AmbaTypes.h"
#include "AmbaWS_RMG.h"
#include "UtilVSL_Common.h"

UINT32 UtilVSL_DrawRMGZone(const AMBA_WS_RMG_CONFIG_s* pRMGCfg, const AMBA_CAL_EM_CALC_COORD_CFG_s *pCalib, const AMBA_SR_CANBUS_TRANSFER_DATA_s* pCanbusTrData, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);
UINT32 UtilVSL_DrawRMGTargetInfo(const AMBA_WS_RMG_WARN_OBJS_s* pObj, UINT32 ODWidth, UINT32 ODHeight, const AMBA_OD_2DBBX_LIST_s *pBbxList, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);
UINT32 UtilVSL_DrawRMGCanbusInfo(const AMBA_SR_CANBUS_TRANSFER_DATA_s* pCanbusTrData, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

#endif