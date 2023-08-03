#ifndef UTIL_CANBUS_HDLR_H
#define UTIL_CANBUS_HDLR_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_SURROUND_H
#include "AmbaSurround.h"
#endif
#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

UINT32 ReadCanbusDatafromSrt(char *pPath);
UINT32 GetCanbusData(UINT32 frameIdx, AMBA_SR_CANBUS_TRANSFER_DATA_s* pCanbusTrData);

#endif