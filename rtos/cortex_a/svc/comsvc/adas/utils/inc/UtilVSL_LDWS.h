#ifndef UTIL_VSL_LDWS_H
#define UTIL_VSL_LDWS_H


#include "RefFlow_LDWS.h"

UINT32 UtilVSL_DrawLDWSLane(UINT8* pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SLD_CROP_INFO_s* pCropInfo, const AMBA_SLD_LANE_PNT_DATA_s* pSldData, const AMBA_LDWS_DEPARTURE_INFO_s* pLdwsInfo);

UINT32 UtilVSL_DrawLDWSWarningMsg(UINT8* pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SLD_CROP_INFO_s* pCropInfo, const AMBA_LDWS_DEPARTURE_INFO_s* pLdwsInfo, const AMBA_SR_CANBUS_TRANSFER_DATA_s* pCanbusInfo);

#endif
