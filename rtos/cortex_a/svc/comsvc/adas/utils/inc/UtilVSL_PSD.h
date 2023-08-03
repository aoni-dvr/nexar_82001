#ifndef UTIL_VSL_PSD_H
#define UTIL_VSL_PSD_H

#include "RefFlow_PSD.h"
#include "AmbaTypes.h"
#include "UtilVSL_Common.h"

UINT32 UtilVSL_PSD_DrawCalibCkeckLine(UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer,
                                      AMBA_PSD_CROP_INFO_s CropInfo, UINT32 Channel, const AMBA_CAL_AVM_CALIB_DATA_s* pCalData, const AMBA_CAL_AVM_PARK_AST_CFG_V1_s* pPACfgV1, UINT8 IsDrawOnRoi);

UINT32 UtilVSL_PSD_DrawPs(UINT32 BufWidth, UINT32 BufHeight, UINT8 *pBuffer,
                          AMBA_PSD_CROP_INFO_s CropInfo, AMBA_PSD_PROC_OUTPUT_DATA_s *pPsdOutData, UINT8 IsDrawOnRoi);

UINT32 UtilVSL_PSD_DrawBevSelf(UINT8 *pBuffer, const UTIL_VSL_BEV_CFG_s *pBevCfg, UINT32 CarWidth, UINT32 CarLength);

UINT32 UtilVSL_PSD_DrawBevPs(UINT8 *pBuffer, const UTIL_VSL_BEV_CFG_s *pBevCfg,
                             AMBA_PSD_PROC_OUTPUT_DATA_s *pPsdOutData, UINT32 IsDrawTk);

#endif
