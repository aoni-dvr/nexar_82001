
#ifndef UTIL_VSL_AEB_H
#define UTIL_VSL_AEB_H

#include "AmbaTypes.h"
#include "AmbaOD_2DBbx.h"
#include "AmbaSurround.h"
#include "AmbaADAS_AEB.h"

UINT32 UtilVSL_DrawAEB(UINT32 Width, UINT32 Height, const AMBA_OD_2DBBX_LIST_s *pBbxIn, const AMBA_SR_SROBJECT_DATA_s *pSRIn, const AMBA_ADAS_AEB_PROC_OUT_s *pAebIn, UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer);

#endif // UTIL_VSL_AEB_H
