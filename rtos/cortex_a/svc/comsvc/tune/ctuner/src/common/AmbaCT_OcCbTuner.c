/**
 *  @file AmbaCT_OcCbTuner.c
 *
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "AmbaCT_OcCbTuner.h"
#include "AmbaFS.h"
#include "AmbaCalib_OcIF.h"
#include "AmbaCalib_LDCCIF.h"
#include "AmbaCalib_ErrNo.h"
#include "AmbaCT_SystemApi.h"
#include "AmbaCalib_DetectionIF.h"
#include "AmbaCT_Logging.h"
#include "AmbaCT_OcCbTunerIF.h"

#ifndef GNU_SECTION_NOZEROINIT
#define GNU_SECTION_NOZEROINIT
#endif

typedef struct {
    UINT8 ImgBuf[CAL_DET_CB_IMG_MAX_SIZE];
    DOUBLE RealTable[MAX_LENS_DISTO_TBL_LEN];
    DOUBLE ExpectTable[MAX_LENS_DISTO_TBL_LEN];
    UINT32 CornerNum;
    AMBA_CAL_POINT_INT_2D_s Corners[CAL_DET_CB_MAX_CORNER_NUM];
    AMBA_CAL_POINT_DB_2D_s SubPixCorners[CAL_DET_CB_MAX_CORNER_NUM];
    AMBA_CAL_OCCB_ORGANIZED_CORNER_s OrganizedCorners;
    AMBA_CAL_POINT_DB_2D_s OpticalCenterResult;
    AMBA_CAL_LENS_DST_REAL_EXPECT_s RefinedLensSpecRealExpect;
    AMBA_CAL_POINT_INT_2D_s BaseCorner2DIdx;
    AMBA_CAL_WORLD_RAW_POINT_s CornerPairs[CAL_DET_CB_MAX_CORNER_NUM];
    AMBA_CAL_SIZE_s CornerPairsImgSize;
} CT_OCCB_WORKING_BUF_s;

typedef enum {
    CT_OCCB_LENS_TYPE_NONFISHEYE = 0,
    CT_OCCB_LENS_TYPE_FISHEYE = 1,
    CT_OCCB_LENS_TYPE_MAX = 2,
} CT_OCCB_LENS_TYPE_e;

static void *pCTPatDetWorkingBuf;
static void *pCTOCFinderWorkingBuf;
static void *pCTOrganizeCBCornerWorkingBuf;
static void *pCTOccbLdccWorkingBuf;
static CT_OCCB_WORKING_BUF_s *pOcCbWorkingBuf;
static UINT8 OcCbTunerValidGroup[AMBA_CT_OCCB_TUNER_MAX];
static AMBA_CT_OCCB_USER_SETTING_s CTUserOccbGroupData GNU_SECTION_NOZEROINIT;
static UINT32 (*pCTFeedSrcFunc)(SIZE_t SrcBufSize, UINT8 *pSrc);
static UINT32 (*pCTPatDetDebugReport)(const char *pOutputPrefix, UINT8 *pSrcImg, const AMBA_CAL_SIZE_s *pImgSize, UINT32 CornerNum, const AMBA_CAL_POINT_DB_2D_s *pSubPixCorners, const INT16 *pCorner2dMap);

static void CT_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}

void AmbaCT_OcCbGetWorkingBufSize(SIZE_t *pSize)
{
    *pSize = sizeof(CT_OCCB_WORKING_BUF_s);
}

static inline void CT_OcCbResetGData2Default(void)
{
    CT_CheckRval(AmbaWrap_memset(&CTUserOccbGroupData, 0x0, sizeof(CTUserOccbGroupData)), "AmbaWrap_memset", __func__);
    CTUserOccbGroupData.Camera.pRealTable = pOcCbWorkingBuf->RealTable;
    CTUserOccbGroupData.Camera.pExceptTable = pOcCbWorkingBuf->ExpectTable;
    //Note # Set checkboard pattern by default
    CTUserOccbGroupData.DetectionCfg.PatternType = 0U;

    //Note # Not to output 3D world map by default
    CTUserOccbGroupData.WorldMapCfg.Output3DWorldMap = 0U;
    CTUserOccbGroupData.OptimizedGridNum.MinSize.Width = 0U;
    CTUserOccbGroupData.OptimizedGridNum.MinSize.Height = 0U;
    AmbaUtility_StringCopy(CTUserOccbGroupData.System.LensType, sizeof(CTUserOccbGroupData.System.LensType), CT_OCCB_LENS_NONFISHEYE);
}

UINT32 AmbaCT_OcCbTunerInit(const AMBA_CT_OCCB_INIT_CFG_s *pInitCfg)
{
    UINT32 Rval = CAL_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    SIZE_t PatDetWorkingBufSize, OrganizeCBCornerWorkingBufSize, OCFinderWorkingBufSize, LdccWorkingBufSize;
    AmbaCal_DetGetPetDetBufSize(&PatDetWorkingBufSize);
    AmbaCal_OcCbGetOrganizeBufSize(&OrganizeCBCornerWorkingBufSize);
    AmbaCal_OcCbGetOCFinderBufSize(&OCFinderWorkingBufSize);
    if (pInitCfg->WorkingBufSize < sizeof(CT_OCCB_WORKING_BUF_s)) {
        pNextMsg = CT_LogPutStr(Msg, "Working buffer size ");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(pInitCfg->WorkingBufSize));
        pNextMsg = CT_LogPutStr(pNextMsg, "incorrect, necessary size:");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(sizeof(CT_OCCB_WORKING_BUF_s)));
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        Rval |= CAL_ERR_1;
    } else if (pInitCfg->PatDetWorkingBufSize < PatDetWorkingBufSize) {
        Rval |= CAL_ERR_1;
    } else if (pInitCfg->OrganizeCBCornerWorkingBufSize < OrganizeCBCornerWorkingBufSize) {
        Rval |= CAL_ERR_1;
    } else if (pInitCfg->OCFinderWorkingBufSize < OCFinderWorkingBufSize) {
        Rval |= CAL_ERR_1;
    } else {
        Rval |= AmbaWrap_memcpy(&pOcCbWorkingBuf, &pInitCfg->pWorkingBuf, sizeof(pOcCbWorkingBuf));
        if(Rval != CT_OK) {
            Rval = CT_ERR_1;
        }
        pCTPatDetWorkingBuf = pInitCfg->pPatDetWorkingBuf;
        pCTOrganizeCBCornerWorkingBuf = pInitCfg->pOrganizeCBCornerWorkingBuf;
        pCTOCFinderWorkingBuf = pInitCfg->pOCFinderWorkingBuf;
        pCTFeedSrcFunc = pInitCfg->FeedSrcFunc;
        pCTPatDetDebugReport = pInitCfg->PatDetDebugReport;
    }

    Rval |= AmbaCal_LdccGetBufSize(&LdccWorkingBufSize);
    if (pInitCfg->LdccWorkingBufSize < LdccWorkingBufSize) {
        pCTOccbLdccWorkingBuf = NULL;
    } else {
        pCTOccbLdccWorkingBuf = pInitCfg->pLdccWorkingBuf;
    }

    CT_OcCbResetGData2Default();

    return Rval;
}

static inline void CT_OcCbSetGroupValid(AMBA_CT_OCCB_TUNER_GROUP_e GroupId)
{
    if (GroupId < AMBA_CT_OCCB_TUNER_MAX) {
        OcCbTunerValidGroup[GroupId] = 1U;
    }
}

UINT8 AmbaCT_OcCbGetGroupStatus(UINT8 GroupId)
{
    return (GroupId < (UINT8)AMBA_CT_OCCB_TUNER_MAX) ? OcCbTunerValidGroup[GroupId] : 0U;
}

void AmbaCT_OcCbGetSystemInfo(AMBA_CT_OCCB_TUNER_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserOccbGroupData.System, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_OcCbSetSystemInfo(const AMBA_CT_OCCB_TUNER_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserOccbGroupData.System, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_OcCbSetGroupValid(AMBA_CT_OCCB_TUNER_SYSTEM_INFO);
}

void AmbaCT_OcCbGetSrcInfo(AMBA_CT_OCCB_TUNER_SRC_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserOccbGroupData.SrcInfo, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_OcCbSetSrcInfo(const AMBA_CT_OCCB_TUNER_SRC_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserOccbGroupData.SrcInfo, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_OcCbSetGroupValid(AMBA_CT_OCCB_TUNER_SRC_INFO);
}

void AmbaCT_OcCbGetPatDetCfg(AMBA_CT_OCCB_TUENR_PAT_DET_CHK_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserOccbGroupData.DetectionCfg.CheckBoardCfg, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_OcCbSetPatDetCfg(const AMBA_CT_OCCB_TUENR_PAT_DET_CHK_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserOccbGroupData.DetectionCfg.CheckBoardCfg, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_OcCbSetGroupValid(AMBA_CT_OCCB_TUNER_PAT_DET);
}

void AmbaCT_OcCbGetPatTypeCfg(UINT32 *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserOccbGroupData.DetectionCfg.PatternType, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_OcCbSetPatTypeCfg(const UINT32 *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserOccbGroupData.DetectionCfg.PatternType, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_OcCbSetGroupValid(AMBA_CT_OCCB_TUNER_PAT_TYPE);
}

void AmbaCT_OcCbGetPatDetCircleCfg(AMBA_CT_OCCB_TUENR_PAT_DET_CIR_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserOccbGroupData.DetectionCfg.CircleCfg, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_OcCbSetPatDetCircleCfg(const AMBA_CT_OCCB_TUENR_PAT_DET_CIR_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserOccbGroupData.DetectionCfg.CircleCfg, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_OcCbSetGroupValid(AMBA_CT_OCCB_TUNER_PAT_CIRCLE_DET);
}

void AmbaCT_OcCbGetOrganizeCornerCfg(AMBA_CAL_OCCB_ORGANIZE_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserOccbGroupData.OrganizeCornerCfg, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_OcCbSetOrganizeCornerCfg(const AMBA_CAL_OCCB_ORGANIZE_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserOccbGroupData.OrganizeCornerCfg, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_OcCbSetGroupValid(AMBA_CT_OCCB_TUNER_ORGANIZE_CORNER);
}

void AmbaCT_OcCbGetCamera(AMBA_CT_OCCB_CAMERA_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserOccbGroupData.Camera, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_OcCbSetCamera(const AMBA_CT_OCCB_CAMERA_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserOccbGroupData.Camera, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_OcCbSetGroupValid(AMBA_CT_OCCB_TUNER_CAMERA);
}

void AmbaCT_OcCbGetOpticalCenter(AMBA_CAL_POINT_DB_2D_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserOccbGroupData.IdealOpticalCenter, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_OcCbSetOpticalCenter(const AMBA_CAL_POINT_DB_2D_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserOccbGroupData.IdealOpticalCenter, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_OcCbSetGroupValid(AMBA_CT_OCCB_TUNER_OPTICAL_CENTER);
}

void AmbaCT_OcCbGetSearchRange(AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserOccbGroupData.SearchRange, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_OcCbSetSearchRange(const AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserOccbGroupData.SearchRange, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_OcCbSetGroupValid(AMBA_CT_OCCB_TUNER_SEARCH_RANGE);
}

void AmbaCT_OcCbGetWorldMapCfg(AMBA_CT_OCCB_TUENR_World_Map_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserOccbGroupData.WorldMapCfg, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_OcCbSetWorldMapCfg(const AMBA_CT_OCCB_TUENR_World_Map_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserOccbGroupData.WorldMapCfg, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_OcCbSetGroupValid(AMBA_CT_OCCB_TUNER_WORLD_MAP);
}

void AmbaCT_OcCbGetOptimizedGridNum(AMBA_CT_OCCB_USED_GRID_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserOccbGroupData.OptimizedGridNum, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_OcCbSetOptimizedGridNum(const AMBA_CT_OCCB_USED_GRID_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserOccbGroupData.OptimizedGridNum, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_OcCbSetGroupValid(AMBA_CT_OCCB_TUNER_OPTIMIZED_GRID_NUM);
}

static CT_OCCB_LENS_TYPE_e CT_OcCbGetLensType(void)
{
    CT_OCCB_LENS_TYPE_e Rval = CT_OCCB_LENS_TYPE_MAX;
    UINT32 Index;
    INT32 CmpResult;
    static const struct {
        CT_OCCB_LENS_TYPE_e TypeId;
        const char *const pName;
    } LensTypeLut[CT_OCCB_LENS_TYPE_MAX] = {
        {CT_OCCB_LENS_TYPE_NONFISHEYE, CT_OCCB_LENS_NONFISHEYE},
        {CT_OCCB_LENS_TYPE_FISHEYE, CT_OCCB_LENS_FISHEYE},
    };

    for (Index = 0U; Index < (sizeof(LensTypeLut) / sizeof(LensTypeLut[0])); Index++) {
        CmpResult = AmbaUtility_StringCompare(CTUserOccbGroupData.System.LensType, LensTypeLut[Index].pName, AmbaUtility_StringLength(CTUserOccbGroupData.System.LensType));
        if (0 == CmpResult) {
            Rval = LensTypeLut[Index].TypeId;
            break;
        }
    }
    return Rval;
}

static const char* CT_OcCbGetGroupName(UINT8 GroupId)
{
    static const struct {
        const char *const GroupName;
        AMBA_CT_OCCB_TUNER_GROUP_e GroupId;
    } GroupLut[] = {
        {"CT_OCCB_TUNER_SYSTEM_INFO", AMBA_CT_OCCB_TUNER_SYSTEM_INFO},
        {"CT_OCCB_TUNER_SRC_INFO", AMBA_CT_OCCB_TUNER_SRC_INFO},
        {"CT_OCCB_TUNER_PAT_TYPE", AMBA_CT_OCCB_TUNER_PAT_TYPE},
        {"CT_OCCB_TUNER_PAT_DET", AMBA_CT_OCCB_TUNER_PAT_DET},
        {"CT_OCCB_TUNER_PAT_CIRCLE_DET", AMBA_CT_OCCB_TUNER_PAT_CIRCLE_DET},
        {"CT_OCCB_TUNER_ORGANIZE_CORNER", AMBA_CT_OCCB_TUNER_ORGANIZE_CORNER},
        {"CT_OCCB_TUNER_CAMERA", AMBA_CT_OCCB_TUNER_CAMERA},
        {"CT_OCCB_TUNER_OPTICAL_CENTER", AMBA_CT_OCCB_TUNER_OPTICAL_CENTER},
        {"CT_OCCB_TUNER_SEARCH_RANGE", AMBA_CT_OCCB_TUNER_SEARCH_RANGE},
        {"CT_OCCB_TUNER_WORLD_MAP", AMBA_CT_OCCB_TUNER_WORLD_MAP},
        {"CT_OCCB_TUNER_OPTIMIZED_GRID_NUM", AMBA_CT_OCCB_TUNER_OPTIMIZED_GRID_NUM},
    };
    const char *Rval = "CALIB_TUNER_UNKNOWN_GROUP";
    UINT32 Index;
    for (Index = 0; Index < (sizeof(GroupLut)/sizeof(GroupLut[0])); Index++) {
        if (GroupId == (UINT8)GroupLut[Index].GroupId) {
            Rval = GroupLut[Index].GroupName;
            break;
        }
    }
    return (const char *const) Rval;
}

static UINT32 CT_OcCbCheckOptionalGroup(UINT8 GroupA, UINT8 GroupB)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;

    if ( (GroupA >= (UINT8)AMBA_CT_OCCB_TUNER_MAX) || (GroupB >= (UINT8)AMBA_CT_OCCB_TUNER_MAX) ) {
        pNextMsg = CT_LogPutStr(Msg, "Invalid Group ID(");
        pNextMsg = CT_LogPutU8(pNextMsg, GroupA);
        pNextMsg = CT_LogPutStr(pNextMsg, ",");
        pNextMsg = CT_LogPutU8(pNextMsg, GroupB);
        pNextMsg = CT_LogPutStr(pNextMsg, ")");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        Rval |= CT_ERR_1;
    } else {
        if ( (OcCbTunerValidGroup[GroupA] == 0U) && (OcCbTunerValidGroup[GroupB] == 0U) ) {
            pNextMsg = CT_LogPutStr(Msg, "Lose Group, either");
            pNextMsg = CT_LogPutStr(pNextMsg, CT_OcCbGetGroupName(GroupA));
            pNextMsg = CT_LogPutStr(pNextMsg, " or ");
            pNextMsg = CT_LogPutStr(pNextMsg, CT_OcCbGetGroupName(GroupB));
            pNextMsg = CT_LogPutStr(pNextMsg, " must be assigned");
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            Rval |= CT_ERR_1;
        }
    }

    return Rval;
}

static UINT32 CT_OcCbCheckGroup(void)
{
    UINT32 Rval = CAL_OK;
    static const UINT8 NecessaryGroup[] = {
        (UINT8)AMBA_CT_OCCB_TUNER_SYSTEM_INFO,
        (UINT8)AMBA_CT_OCCB_TUNER_SRC_INFO,
        (UINT8)AMBA_CT_OCCB_TUNER_ORGANIZE_CORNER,
        (UINT8)AMBA_CT_OCCB_TUNER_CAMERA,
        (UINT8)AMBA_CT_OCCB_TUNER_OPTICAL_CENTER,
    };

    UINT32 Index;
    UINT32 GroupNum = CT_sizeT_to_U32(sizeof(NecessaryGroup)/sizeof(NecessaryGroup[0]));
    for (Index = 0; Index < GroupNum; Index++) {
        if (OcCbTunerValidGroup[NecessaryGroup[Index]] == 0U) {
            AmbaPrint_PrintStr5("Lose Group %s", CT_OcCbGetGroupName(NecessaryGroup[Index]), DC_S, DC_S, DC_S, DC_S);
            Rval |= CAL_ERR_1;
        }
    }
    return Rval;
}

static inline UINT32 CT_OcCbDumpDetectCornerCfg(const AMBA_CAL_DET_CFG_s *pDetectCornerCfg)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;

    if ( pDetectCornerCfg->DetPatternType == DET_PATTERN_MODE_CIRCLE ) {
        pNextMsg = CT_LogPutStr(Msg, "pDetectCornerCfg->Src.ImgSize.Width");
        pNextMsg = CT_LogPutU32(pNextMsg, pDetectCornerCfg->CircleCfg.Src.ImgSize.Width);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "pDetectCornerCfg->Src.ImgSize.Height");
        pNextMsg = CT_LogPutU32(pNextMsg, pDetectCornerCfg->CircleCfg.Src.ImgSize.Height);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "pDetectCornerCfg->CircleCfg.MinContourLength");
        pNextMsg = CT_LogPutU32(pNextMsg, pDetectCornerCfg->CircleCfg.MinContourLength);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "pDetectCornerCfg->CircleCfg.MaxContourLength");
        pNextMsg = CT_LogPutU32(pNextMsg, pDetectCornerCfg->CircleCfg.MaxContourLength);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "pDetectCornerCfg->CircleCfg.MaskRadius");
        pNextMsg = CT_LogPutU32(pNextMsg, pDetectCornerCfg->CircleCfg.MaskRadius);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    } else {
        pNextMsg = CT_LogPutStr(Msg, "pDetectCornerCfg->CheckBoardCfg.Src.ImgSize.Width");
        pNextMsg = CT_LogPutU32(pNextMsg, pDetectCornerCfg->CheckBoardCfg.Src.ImgSize.Width);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "pDetectCornerCfg->CheckBoardCfg.Src.ImgSize.Height");
        pNextMsg = CT_LogPutU32(pNextMsg, pDetectCornerCfg->CheckBoardCfg.Src.ImgSize.Height);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "pDetectCornerCfg->CheckBoardCfg.QualityLevel");
        pNextMsg = CT_LogPutDB(pNextMsg, pDetectCornerCfg->CheckBoardCfg.QualityLevel);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "pDetectCornerCfg->CheckBoardCfg.MinDistance");
        pNextMsg = CT_LogPutU32(pNextMsg, pDetectCornerCfg->CheckBoardCfg.MinDistance);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    }
    return Rval;
}
static inline UINT32 CT_OcCbFeedDetectCornerCfg(AMBA_CAL_DET_CFG_s *pDetectCornerCfg)
{
    UINT32 Rval;

    if ( CTUserOccbGroupData.DetectionCfg.PatternType == 0U ) {
        //Note # Detect Checkboard
        pDetectCornerCfg->DetPatternType = DET_PATTERN_MODE_CHECKBOARD;
        pDetectCornerCfg->CheckBoardCfg.Src.pSrcAddr = pOcCbWorkingBuf->ImgBuf;
        Rval = pCTFeedSrcFunc(sizeof(pOcCbWorkingBuf->ImgBuf), pOcCbWorkingBuf->ImgBuf);
        Rval |= AmbaWrap_memcpy(&pDetectCornerCfg->CheckBoardCfg.Src.ImgSize,
            &CTUserOccbGroupData.SrcInfo.ImgSize, sizeof(pDetectCornerCfg->CheckBoardCfg.Src.ImgSize));
        if(Rval != CT_OK) {
            Rval = CT_ERR_1;
        }

        //FIXME # Remove DetectRegion
        pDetectCornerCfg->CheckBoardCfg.QualityLevel = CTUserOccbGroupData.DetectionCfg.CheckBoardCfg.QualityLevel;
        pDetectCornerCfg->CheckBoardCfg.MinDistance = CTUserOccbGroupData.DetectionCfg.CheckBoardCfg.MinDistance;
    } else {
        //Note # Detect Circle pattern
        pDetectCornerCfg->DetPatternType = DET_PATTERN_MODE_CIRCLE;
        pDetectCornerCfg->CircleCfg.Src.pSrcAddr = pOcCbWorkingBuf->ImgBuf;
        Rval = pCTFeedSrcFunc(sizeof(pOcCbWorkingBuf->ImgBuf), pOcCbWorkingBuf->ImgBuf);
        Rval |= AmbaWrap_memcpy(&pDetectCornerCfg->CircleCfg.Src.ImgSize,
            &CTUserOccbGroupData.SrcInfo.ImgSize, sizeof(pDetectCornerCfg->CircleCfg.Src.ImgSize));
        if(Rval != CT_OK) {
            Rval = CT_ERR_1;
        }
        pDetectCornerCfg->CircleCfg.MinContourLength = (UINT32)CTUserOccbGroupData.DetectionCfg.CircleCfg.MinPerimeter;
        pDetectCornerCfg->CircleCfg.MaxContourLength = (UINT32)CTUserOccbGroupData.DetectionCfg.CircleCfg.MaxPerimeter;
        pDetectCornerCfg->CircleCfg.MaskRadius = (UINT32)CTUserOccbGroupData.DetectionCfg.CircleCfg.MinDistance;
    }

    Rval |= CT_OcCbDumpDetectCornerCfg(pDetectCornerCfg);

    return Rval;
}

static inline UINT32 CT_OcCbDumpOrganizeCfg(const AMBA_CAL_OCCB_ORGANIZE_CFG_s *pOrganizeCornerCfg)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    pNextMsg = CT_LogPutStr(Msg, "pOrganizeCornerCfg->EstToleranceRate");
    pNextMsg = CT_LogPutDB(pNextMsg, pOrganizeCornerCfg->EstToleranceRate);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pOrganizeCornerCfg->CenterSearchRadius");
    pNextMsg = CT_LogPutU32(pNextMsg, pOrganizeCornerCfg->CenterSearchRadius);
    Rval |=  CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pOrganizeCornerCfg->CenterNeighborsSearchRadius");
    pNextMsg = CT_LogPutU32(pNextMsg, pOrganizeCornerCfg->CenterNeighborsSearchRadius);
    Rval |=  CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pOrganizeCornerCfg->SearchRadius");
    pNextMsg = CT_LogPutU32(pNextMsg, pOrganizeCornerCfg->SearchRadius);
    Rval |=  CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    return Rval;
}

static inline void CT_OcCbFeedOrganizeCfg(AMBA_CAL_OCCB_ORGANIZE_CFG_s *pOrganizeCornerCfg)
{
    CT_CheckRval(AmbaWrap_memcpy(pOrganizeCornerCfg, &CTUserOccbGroupData.OrganizeCornerCfg, sizeof(*pOrganizeCornerCfg)), "AmbaWrap_memcpy", __func__);
    CT_CheckRval(CT_OcCbDumpOrganizeCfg(pOrganizeCornerCfg), "CT_OcCbDumpOrganizeCfg", __func__);
}

static inline void CT_OcCbFeedLensSpec(AMBA_CAL_LENS_DST_REAL_EXPECT_s *pRealExpectBuf, AMBA_CAL_LENS_SPEC_s *pOut)
{
    pOut->LensDistoType = AMBA_CAL_LD_REAL_EXPECT_TBL;
    pOut->LensDistoUnit = AMBA_CAL_LD_MM;
    pRealExpectBuf->Length = CTUserOccbGroupData.Camera.TableLen;
    pRealExpectBuf->pRealTbl = CTUserOccbGroupData.Camera.pRealTable;
    pRealExpectBuf->pExpectTbl = CTUserOccbGroupData.Camera.pExceptTable;
    pOut->LensDistoSpec.pRealExpect = pRealExpectBuf;
}

static inline UINT32 CT_OcCbDumpOCFinderCfg(const AMBA_CAL_CAM_s *pCamSpec, const AMBA_CAL_SIZE_s *pSearchRadius)
{
    UINT32 Rval = CT_OK;
    UINT32 Index;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    pNextMsg = CT_LogPutStr(Msg, "pCamSpec->Lens.LensDistoType");
    pNextMsg = CT_LogPutU32(pNextMsg, (UINT32) pCamSpec->Lens.LensDistoType);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCamSpec->Lens.LensDistoUnit");
    pNextMsg = CT_LogPutU32(pNextMsg, (UINT32) pCamSpec->Lens.LensDistoUnit);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCamSpec->Lens.LensDistoSpec.pRealExpect->Length");
    pNextMsg = CT_LogPutU32(pNextMsg, pCamSpec->Lens.LensDistoSpec.pRealExpect->Length);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCamSpec->Lens.LensDistoSpec.pRealExpect->pRealTbl");
    for (Index = 0U; Index < pCamSpec->Lens.LensDistoSpec.pRealExpect->Length; Index++) {
        pNextMsg = CT_LogPutDB(Msg, pCamSpec->Lens.LensDistoSpec.pRealExpect->pRealTbl[Index]);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    }

    pNextMsg = CT_LogPutStr(Msg, "pCamSpec->Lens.LensDistoSpec.pRealExpect->pExpectTbl");
    for (Index = 0U; Index < pCamSpec->Lens.LensDistoSpec.pRealExpect->Length; Index++) {
        pNextMsg = CT_LogPutDB(Msg, pCamSpec->Lens.LensDistoSpec.pRealExpect->pExpectTbl[Index]);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    }

    pNextMsg = CT_LogPutStr(Msg, "pCamSpec->Sensor.CellSize");
    pNextMsg = CT_LogPutDB(pNextMsg, pCamSpec->Sensor.CellSize);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCamSpec->Sensor.StartX");
    pNextMsg = CT_LogPutU32(pNextMsg, pCamSpec->Sensor.StartX);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCamSpec->Sensor.StartY");
    pNextMsg = CT_LogPutU32(pNextMsg, pCamSpec->Sensor.StartY);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCamSpec->Sensor.Width");
    pNextMsg = CT_LogPutU32(pNextMsg, pCamSpec->Sensor.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCamSpec->Sensor.Height");
    pNextMsg = CT_LogPutU32(pNextMsg, pCamSpec->Sensor.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCamSpec->OpticalCenter.X");
    pNextMsg = CT_LogPutDB(pNextMsg, pCamSpec->OpticalCenter.X);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCamSpec->OpticalCenter.Y");
    pNextMsg = CT_LogPutDB(pNextMsg, pCamSpec->OpticalCenter.Y);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCamSpec->Pos.X");
    pNextMsg = CT_LogPutDB(pNextMsg, pCamSpec->Pos.X);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCamSpec->Pos.Y");
    pNextMsg = CT_LogPutDB(pNextMsg, pCamSpec->Pos.Y);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCamSpec->Pos.Z");
    pNextMsg = CT_LogPutDB(pNextMsg, pCamSpec->Pos.Z);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCamSpec->Rotation");
    pNextMsg = CT_LogPutU32(pNextMsg, (UINT32)pCamSpec->Rotation);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pSearchRadius->Width");
    pNextMsg = CT_LogPutU32(pNextMsg, pSearchRadius->Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pSearchRadius->Height");
    pNextMsg = CT_LogPutU32(pNextMsg, pSearchRadius->Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    return Rval;
}

static UINT32 CT_OcCbNonFisheyeExecute(void)
{
    UINT32 Rval = CAL_OK;
    UINT32 CheckGroupRval;
    UINT32 CheckOptionalGroupRval;

    CheckGroupRval = CT_OcCbCheckGroup();
    CheckOptionalGroupRval = CT_OcCbCheckOptionalGroup(
        (UINT8) AMBA_CT_OCCB_TUNER_PAT_DET,
        (UINT8) AMBA_CT_OCCB_TUNER_PAT_CIRCLE_DET);

    if ( (CheckGroupRval != CAL_OK) ||
        (CheckOptionalGroupRval != CAL_OK) ) {
        Rval |= CAL_ERR_1;
    } else {
        AMBA_CAL_DET_CFG_s DetectCornerCfg;
        AMBA_CAL_OCCB_ORGANIZE_CFG_s OrganizeCfg;
        AMBA_CAL_CAM_s CamSpec;
        AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect;

        Rval |= CT_OcCbFeedDetectCornerCfg(&DetectCornerCfg);

        if (Rval == CAL_OK) {
            AMBA_CAL_DET_SRC_s SrcCfg;
            UINT32 DebugReportRval = CAL_OK;

            Rval |= AmbaWrap_memset(&SrcCfg, 0, sizeof(AMBA_CAL_DET_SRC_s));

            CT_OcCbFeedOrganizeCfg(&OrganizeCfg);
            CT_OcCbFeedLensSpec(&LensSpecRealExpect, &CamSpec.Lens);
            CamSpec.Sensor.CellSize = CTUserOccbGroupData.Camera.CellSize;
            CamSpec.Sensor.StartX = 0;
            CamSpec.Sensor.StartY = 0;
            CamSpec.Sensor.Width = CTUserOccbGroupData.SrcInfo.ImgSize.Width;
            CamSpec.Sensor.Height = CTUserOccbGroupData.SrcInfo.ImgSize.Height;
            Rval |= AmbaWrap_memcpy(&CamSpec.OpticalCenter, &CTUserOccbGroupData.IdealOpticalCenter, sizeof(CamSpec.OpticalCenter));
            Rval |= AmbaWrap_memset(&CamSpec.Pos, 0x0, sizeof(CamSpec.Pos));
            CamSpec.Rotation = AMBA_CAL_ROTATE_0;
            Rval |= CT_OcCbDumpOCFinderCfg(&CamSpec, &CTUserOccbGroupData.SearchRange);

            if ( DetectCornerCfg.DetPatternType == DET_PATTERN_MODE_CHECKBOARD ) {
                SrcCfg = DetectCornerCfg.CheckBoardCfg.Src;
                Rval |= AmbaCal_DetCheckboardCorners(&DetectCornerCfg.CheckBoardCfg, pCTPatDetWorkingBuf,
                pOcCbWorkingBuf->Corners, &pOcCbWorkingBuf->CornerNum);
                if (Rval == CAL_OK) {
                    Rval |= AmbaCal_DetFindAllCornersSubPixel(&DetectCornerCfg.CheckBoardCfg.Src, pOcCbWorkingBuf->Corners,
                        pOcCbWorkingBuf->CornerNum, pOcCbWorkingBuf->SubPixCorners);
                }
            } else if ( DetectCornerCfg.DetPatternType == DET_PATTERN_MODE_CIRCLE ) {
                SrcCfg = DetectCornerCfg.CircleCfg.Src;
                Rval |= AmbaCal_DetCircleCorners(&DetectCornerCfg.CircleCfg, pCTPatDetWorkingBuf,
                    pOcCbWorkingBuf->SubPixCorners, &pOcCbWorkingBuf->CornerNum);
            } else {
                CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
                CT_LOG_MSG_s *pNextMsg = Msg;

                pNextMsg = CT_LogPutStr(Msg, "Unsupport PatternType ");
                pNextMsg = CT_LogPutU32(pNextMsg, DetectCornerCfg.DetPatternType);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                Rval |= CAL_ERR_1;
            }

            if (Rval == CAL_OK) {
                Rval |= AmbaCal_OcCbOrganizeCorners(&OrganizeCfg, pOcCbWorkingBuf->SubPixCorners, pOcCbWorkingBuf->CornerNum, &CTUserOccbGroupData.SrcInfo.ImgSize, pCTOrganizeCBCornerWorkingBuf, &pOcCbWorkingBuf->OrganizedCorners);
                if (Rval == CAL_OK) {
                    Rval |= AmbaCal_OcCbOpticalCenterFinder(&CamSpec,
                            &CTUserOccbGroupData.SearchRange,
                            pOcCbWorkingBuf->SubPixCorners,
                            pOcCbWorkingBuf->OrganizedCorners.pCorner2dMap,
                            &pOcCbWorkingBuf->OrganizedCorners.Corner2dMapValidArea,
                            pCTOCFinderWorkingBuf,
                            &pOcCbWorkingBuf->OpticalCenterResult);

                    if ( CTUserOccbGroupData.WorldMapCfg.Output3DWorldMap != 0U ) {
                        Rval |= AmbaCal_OcCbCalculateWorldInfo(
                            CTUserOccbGroupData.WorldMapCfg.BasePtPos,
                            SrcCfg.pSrcAddr,
                            SrcCfg.ImgSize,
                            pOcCbWorkingBuf->SubPixCorners,
                            pOcCbWorkingBuf->CornerNum,
                            pOcCbWorkingBuf->OrganizedCorners.pCorner2dMap,
                            &pOcCbWorkingBuf->OrganizedCorners.Corner2dMapValidArea,
                            CTUserOccbGroupData.WorldMapCfg.HorizontalGridSpace,
                            CTUserOccbGroupData.WorldMapCfg.VerticalGridSpace,
                            &pOcCbWorkingBuf->BaseCorner2DIdx,
                            pOcCbWorkingBuf->CornerPairs,
                            &pOcCbWorkingBuf->CornerPairsImgSize);
                    }

                    if (Rval == CAL_OK) {
                        if (pCTOccbLdccWorkingBuf != NULL) {
                            AMBA_CAL_LDCC_CFG_s LdccCfg;
                            CT_CheckRval(AmbaWrap_memcpy(&LdccCfg.Sensor, &CamSpec.Sensor, sizeof(LdccCfg.Sensor)), "AmbaWrap_memcpy", __func__);
                            LdccCfg.OpticalCenter.X = pOcCbWorkingBuf->OpticalCenterResult.X;
                            LdccCfg.OpticalCenter.Y = pOcCbWorkingBuf->OpticalCenterResult.Y;
                            LdccCfg.LensDistoUnit = AMBA_CAL_LD_MM;

                            CT_CheckRval(AmbaWrap_memcpy(&LdccCfg.RealExpectTbl, &LensSpecRealExpect, sizeof(LdccCfg.RealExpectTbl)), "AmbaWrap_memcpy", __func__);;
                            LdccCfg.pSrcMap = pOcCbWorkingBuf->SubPixCorners;
                            CT_CheckRval(AmbaWrap_memcpy(&LdccCfg.DstMap, &pOcCbWorkingBuf->OrganizedCorners, sizeof(LdccCfg.DstMap)), "AmbaWrap_memcpy", __func__);;
                            LdccCfg.PointCount = pOcCbWorkingBuf->CornerNum;

                            Rval |= AmbaCal_LdccCalibLensDistoCurve(&LdccCfg, pCTOccbLdccWorkingBuf, &pOcCbWorkingBuf->RefinedLensSpecRealExpect);
                        } else {
                            pOcCbWorkingBuf->RefinedLensSpecRealExpect.Length = 0U;
                            pOcCbWorkingBuf->RefinedLensSpecRealExpect.pExpectTbl = NULL;
                            pOcCbWorkingBuf->RefinedLensSpecRealExpect.pRealTbl = NULL;
                        }
                    }
                }
            }

            //Since call-back maybe fail due to request folder/path doesn't exist, ignore return value at present.
            DebugReportRval |= pCTPatDetDebugReport(CTUserOccbGroupData.System.OutputPrefix,
                                                    SrcCfg.pSrcAddr, &SrcCfg.ImgSize,
                                                    pOcCbWorkingBuf->CornerNum, pOcCbWorkingBuf->SubPixCorners, pOcCbWorkingBuf->OrganizedCorners.pCorner2dMap);
            if (DebugReportRval != CAL_OK) {
                CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
                CT_LOG_MSG_s *pNextMsg = Msg;
                pNextMsg = CT_LogPutStr(Msg, "Dump debug data failed");
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            }

        } else {
            CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
            CT_LOG_MSG_s *pNextMsg = Msg;
            pNextMsg = CT_LogPutStr(Msg, "Source image open failed");
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            Rval |= CAL_ERR_1;
        }
    }
    return Rval;
}


static UINT32 CT_OcCbFisheyeExecute(void)
{
    UINT32 Rval = CAL_OK;
    UINT32 CheckGroupRval;
    CheckGroupRval = CT_OcCbCheckGroup();

    if (CheckGroupRval != CAL_OK) {
        Rval |= CAL_ERR_1;
    } else {
        AMBA_CAL_DET_CFG_s DetectCornerCfg;
        AMBA_CAL_OCCB_ORGANIZE_CFG_s OrganizeCfg;
        AMBA_CAL_CAM_s CamSpec;
        AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect;

        Rval |= CT_OcCbFeedDetectCornerCfg(&DetectCornerCfg);
        if (Rval == CAL_OK) {
            AMBA_CAL_DET_SRC_s SrcCfg;

            Rval |= AmbaWrap_memset(&SrcCfg, 0, sizeof(AMBA_CAL_DET_SRC_s));

            CT_OcCbFeedOrganizeCfg(&OrganizeCfg);
            CT_OcCbFeedLensSpec(&LensSpecRealExpect, &CamSpec.Lens);
            CamSpec.Sensor.CellSize = CTUserOccbGroupData.Camera.CellSize;
            CamSpec.Sensor.StartX = 0;
            CamSpec.Sensor.StartY = 0;
            CamSpec.Sensor.Width = CTUserOccbGroupData.SrcInfo.ImgSize.Width;
            CamSpec.Sensor.Height = CTUserOccbGroupData.SrcInfo.ImgSize.Height;
            Rval |= AmbaWrap_memcpy(&CamSpec.OpticalCenter, &CTUserOccbGroupData.IdealOpticalCenter, sizeof(CamSpec.OpticalCenter));
            Rval |= AmbaWrap_memset(&CamSpec.Pos, 0x0, sizeof(CamSpec.Pos));
            CamSpec.Rotation = AMBA_CAL_ROTATE_0;
            Rval |= CT_OcCbDumpOCFinderCfg(&CamSpec, &CTUserOccbGroupData.SearchRange);

            if ( DetectCornerCfg.DetPatternType == DET_PATTERN_MODE_CHECKBOARD ) {
                SrcCfg = DetectCornerCfg.CheckBoardCfg.Src;
                Rval |= AmbaCal_DetCheckboardCorners(&DetectCornerCfg.CheckBoardCfg, pCTPatDetWorkingBuf,
                pOcCbWorkingBuf->Corners, &pOcCbWorkingBuf->CornerNum);
                if (Rval == CAL_OK) {
                    Rval |= AmbaCal_DetFindAllCornersSubPixel(&DetectCornerCfg.CheckBoardCfg.Src, pOcCbWorkingBuf->Corners,
                        pOcCbWorkingBuf->CornerNum, pOcCbWorkingBuf->SubPixCorners);
                }
            } else if ( DetectCornerCfg.DetPatternType == DET_PATTERN_MODE_CIRCLE ) {
                SrcCfg = DetectCornerCfg.CircleCfg.Src;
                Rval |= AmbaCal_DetCircleCorners(&DetectCornerCfg.CircleCfg, pCTPatDetWorkingBuf,
                    pOcCbWorkingBuf->SubPixCorners, &pOcCbWorkingBuf->CornerNum);
            } else {
                CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
                CT_LOG_MSG_s *pNextMsg = Msg;

                pNextMsg = CT_LogPutStr(Msg, "Unsupport PatternType ");
                pNextMsg = CT_LogPutU32(pNextMsg, DetectCornerCfg.DetPatternType);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                Rval |= CAL_ERR_1;
            }

            if (Rval == CAL_OK) {
                AMBA_CAL_OCCB_USED_GRID_SIZE_s UsedOCGridSize;
                UsedOCGridSize.MaxSize.Top = (CTUserOccbGroupData.OptimizedGridNum.MaxSize.Height - 1U) / 2U;
                UsedOCGridSize.MaxSize.Bottom = UsedOCGridSize.MaxSize.Top;
                UsedOCGridSize.MaxSize.Left = (CTUserOccbGroupData.OptimizedGridNum.MaxSize.Width - 1U) / 2U;
                UsedOCGridSize.MaxSize.Right = UsedOCGridSize.MaxSize.Left;
                UsedOCGridSize.MinSize.Top = (CTUserOccbGroupData.OptimizedGridNum.MinSize.Height == 0U) ? 0U : ((CTUserOccbGroupData.OptimizedGridNum.MinSize.Height - 1U) / 2U);
                UsedOCGridSize.MinSize.Bottom = UsedOCGridSize.MinSize.Top;
                UsedOCGridSize.MinSize.Left = (CTUserOccbGroupData.OptimizedGridNum.MinSize.Width == 0U) ? 0U : ((CTUserOccbGroupData.OptimizedGridNum.MinSize.Width - 1U) / 2U);
                UsedOCGridSize.MinSize.Right = UsedOCGridSize.MinSize.Left;
                Rval |= AmbaCal_OcCbOrganizeCorners(&OrganizeCfg, pOcCbWorkingBuf->SubPixCorners, pOcCbWorkingBuf->CornerNum, &CTUserOccbGroupData.SrcInfo.ImgSize, pCTOrganizeCBCornerWorkingBuf, &pOcCbWorkingBuf->OrganizedCorners);
                Rval |= AmbaCal_OcCbOpticalCenterFindV1(&CamSpec,
                        &CTUserOccbGroupData.SearchRange,
                        pOcCbWorkingBuf->SubPixCorners,
                        pOcCbWorkingBuf->OrganizedCorners.pCorner2dMap,
                        &pOcCbWorkingBuf->OrganizedCorners.Corner2dMapValidArea,
                        &UsedOCGridSize,
                        pCTOCFinderWorkingBuf,
                        &pOcCbWorkingBuf->OpticalCenterResult);
            }

            Rval |= pCTPatDetDebugReport(CTUserOccbGroupData.System.OutputPrefix,
                    SrcCfg.pSrcAddr, &SrcCfg.ImgSize,
                    pOcCbWorkingBuf->CornerNum, pOcCbWorkingBuf->SubPixCorners, pOcCbWorkingBuf->OrganizedCorners.pCorner2dMap);

        } else {
            CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
            CT_LOG_MSG_s *pNextMsg = Msg;
            pNextMsg = CT_LogPutStr(Msg, "Source image open failed");
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            Rval |= CAL_ERR_1;
        }
    }
    return Rval;
}


UINT32 AmbaCT_OcCbExecute(void)
{
    UINT32 Rval = CT_OK;
    CT_OCCB_LENS_TYPE_e LensType = CT_OcCbGetLensType();
    switch (LensType) {
    case CT_OCCB_LENS_TYPE_NONFISHEYE:
        Rval |= CT_OcCbNonFisheyeExecute();
        break;
    case CT_OCCB_LENS_TYPE_FISHEYE:
        Rval |= CT_OcCbFisheyeExecute();
        break;
    default:
        Rval = CT_ERR_1;
        break;
    }
    return Rval;
}

UINT32 AmbaCT_OcCbGetCalibData(AMBA_CT_OCCB_CALIB_DATA_s *pData)
{
    UINT32 Rval = CT_OK;
    pData->CornerNum = pOcCbWorkingBuf->CornerNum;
    pData->pSubPixCorners = pOcCbWorkingBuf->SubPixCorners;
    pData->pOrganizedCorners = &pOcCbWorkingBuf->OrganizedCorners;
    pData->pOpticalCenterResult = &pOcCbWorkingBuf->OpticalCenterResult;
    pData->pRefinedLensSpecRealExpect = &pOcCbWorkingBuf->RefinedLensSpecRealExpect;

    pData->BaseCornerIdx = pOcCbWorkingBuf->BaseCorner2DIdx;
    pData->pCornerPairs = pOcCbWorkingBuf->CornerPairs;
    pData->CornerPairsImgSize = pOcCbWorkingBuf->CornerPairsImgSize;

    return Rval;
}

const AMBA_CT_OCCB_USER_SETTING_s* AmbaCT_OcCbGetUserSetting(void)
{
    return (const AMBA_CT_OCCB_USER_SETTING_s*) &CTUserOccbGroupData;
}
