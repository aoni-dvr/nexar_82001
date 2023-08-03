#ifndef UTIL_LOGFUNCTION_H
#define UTIL_LOGFUNCTION_H

#include "Util_Common.h"
#include "AmbaTypes.h"
#include "AmbaOD_2DBbx.h"
#include "AmbaSurround.h"

/**
* Save 2D bounding box list into bin files.
* @param [in] pBbxList: 2D bounding box list.
* @return Errorcode.
*/
UINT32 UtilLog_Save2DBbx(const AMBA_OD_2DBBX_LIST_s* pBbxList);

/**
* Save CANBUS information into bin files.
* @param [in] pCanbus: Transfered CANBUS data.
* @return Errorcode.
*/
UINT32 UtilLog_SaveCanbus(const AMBA_SR_CANBUS_TRANSFER_DATA_s* pCanbus);

#endif // UTIL_LOGFUNCTION_H
