
#ifndef UTIL_VSL_PRIVATE_H
#define UTIL_VSL_PRIVATE_H

#include "AmbaTypes.h"
#include "AmbaOD_2DBbx.h"
#include "AmbaSurround.h"
#include "Util_DrawFunction.h"
#include "UtilVSL_Common.h"



typedef struct {
    DOUBLE PntX[4];
    DOUBLE PntY[4];
    UINT32 NumOfPnt;
} Util_VSL_BEV_SR_OBJ_s;



UINT32 UtilVSL_StrPrtUintandStr(UINT32 Event, const char *InStr, char *OutStr);

UINT32 UtilVSL_GetObjLocation(const AMBA_SR_OBJECT_DATA_s *pSRIn, Util_VSL_BEV_SR_OBJ_s *pBEVObj);

UINT32 UtilVSL_OnePntDraw(const UTIL_DRAW_BUFFER_INFO_s *pBufInfo, const UTIL_VSL_BEV_CFG_s *pBevCfg, const AMBA_SR_OBJECT_DATA_s *pSRIn, const Util_VSL_BEV_SR_OBJ_s *pBEVObj, UINT32 LineWidth, UINT32 LineLenght, const UINT8 Color[UTIL_DRAW_MAX_CHANNEL_NUM], UINT8 ArrowFlag);
UINT32 UtilVSL_TwoPntDraw(const UTIL_DRAW_BUFFER_INFO_s *pBufInfo, const UTIL_VSL_BEV_CFG_s *pBevCfg, const AMBA_SR_OBJECT_DATA_s *pSRIn, const Util_VSL_BEV_SR_OBJ_s *pBEVObj, UINT32 LineWidth, UINT32 LineLenght, const UINT8 Color[UTIL_DRAW_MAX_CHANNEL_NUM], UINT8 ArrowFlag);
UINT32 UtilVSL_ThreePntDraw(const UTIL_DRAW_BUFFER_INFO_s *pBufInfo, const UTIL_VSL_BEV_CFG_s *pBevCfg, const AMBA_SR_OBJECT_DATA_s *pSRIn, const Util_VSL_BEV_SR_OBJ_s *pBEVObj, UINT32 LineWidth, UINT32 LineLenght, const UINT8 Color[UTIL_DRAW_MAX_CHANNEL_NUM], UINT8 ArrowFlag);
UINT32 UtilVSL_FourPntDraw(const UTIL_DRAW_BUFFER_INFO_s *pBufInfo, const UTIL_VSL_BEV_CFG_s *pBevCfg, const AMBA_SR_OBJECT_DATA_s *pSRIn, const Util_VSL_BEV_SR_OBJ_s *pBEVObj, UINT32 LineWidth, UINT32 LineLenght, const UINT8 Color[UTIL_DRAW_MAX_CHANNEL_NUM], UINT8 ArrowFlag);

#endif // UTIL_VSL_PRIVATE_H