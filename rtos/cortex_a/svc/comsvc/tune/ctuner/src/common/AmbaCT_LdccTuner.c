/**
 *  @file AmbaCT_LdccTuner.c
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
#include "AmbaCalib_ErrNo.h"
#include "AmbaCT_LdccTuner.h"
#include "AmbaCT_LdccTunerIF.h"
#include "AmbaCalib_DetectionDef.h"
#include "AmbaCalib_DetectionIF.h"
#include "AmbaCalib_Def.h"
#include "AmbaCT_Logging.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"

#ifndef GNU_SECTION_NOZEROINIT
#define GNU_SECTION_NOZEROINIT
#endif

#if 0
typedef enum {
    CT_LDCC_LENS_TYPE_NONFISHEYE = 0,
    CT_LDCC_LENS_TYPE_FISHEYE,
    CT_LDCC_LENS_TYPE_MAX,
} CT_LDCC_LENS_TYPE_s;
#endif
typedef struct {
    UINT8 YImgBuf[CAL_DET_CB_IMG_MAX_SIZE];
    UINT8 UVImgBuf[CAL_DET_CB_IMG_MAX_SIZE];
    void *pPatDetWorkingBuf;
    void *pOrganizeCornerWorkingBuf;
    void *pOCFinderWorkingBuf;
    void *pPatDetSeparateLineBuf;
    void *pCal1PlaneDistorWorkingBuf;
    void *pCal3planeDistorWorkingBuf;
    UINT32 (*pFeedSinglePlaneSrcFunc)(const AMBA_CAL_SIZE_s *pImgSize, const AMBA_CAL_YUV_INFO_s *pYUVInfo);
    UINT32 (*pDrawSinglePlaneOrgResult)(const AMBA_CAL_POINT_DB_2D_s *pCorners, const AMBA_CAL_OCCB_ORGANIZED_CORNER_s *pOrganizedCorners, UINT8 *pYImg);
    UINT32 (*pFeedMultiPlaneSrcFunc)(const AMBA_CAL_SIZE_s *pImgSize, const AMBA_CAL_YUV_INFO_s *pYUVInfo);
    UINT32 (*pDrawMultiPlaneDetResult)(const AMBA_CAL_DET_CORNER_LIST_s *pCornerList, UINT8 *pYImg);
    UINT32 (*pDrawMultiPlaneDetResultByPlane)(const AMBA_CAL_DET_CORNER_LIST_s *pCornersList0, const AMBA_CAL_DET_CORNER_LIST_s *pCornersList1, const AMBA_CAL_DET_CORNER_LIST_s *pCornersList2, UINT8 *pYImg);
    UINT32 (*pFeedMultiPlanePatInfo)(UINT32 PlaneIdx, AMBA_CAL_OCCB_CIRLCE_INFO_s *pPatInfo);
    UINT32 (*pDrawMultiPlaneOrgResult)(UINT32 PlaneIdx, const AMBA_CAL_POINT_DB_2D_s *pCorners, const AMBA_CAL_OCCB_ORGANIZED_CORNER_s *pOrganizedCorners, UINT8 *pYImg);
    AMBA_CAL_POINT_DB_2D_s SubPixCorners[CAL_DET_CB_MAX_CORNER_NUM];
    INT16 MultiPlaneOrganizedCorners2dMap[3][201*201];
    AMBA_CAL_OCCB_ORGANIZED_CORNER_s OrganizedCorners[3];

    AMBA_CAL_DET_CORNER_TYPE_s TotalCornersTypeList[CAL_DET_CB_MAX_CORNER_NUM];
    AMBA_CAL_DET_CORNER_TYPE_s MultiPlaneCornersTypeList[3][CAL_DET_CB_MAX_CORNER_NUM];
    AMBA_CAL_POINT_DB_2D_s MultiPlaneSubPixCorners[3][CAL_DET_CB_MAX_CORNER_NUM];
    AMBA_CT_LDCC_CALIB_DATA_s CalibData;
    DOUBLE OnePlaneRTblResult[MAX_LENS_DISTO_TBL_LEN];
    DOUBLE OnePlaneETblResult[MAX_LENS_DISTO_TBL_LEN];
    DOUBLE MultiPlaneRTblResult[MAX_LENS_DISTO_TBL_LEN];
    DOUBLE MultiPlaneETblResult[MAX_LENS_DISTO_TBL_LEN];
    DOUBLE ResultRealTbl[MAX_LENS_DISTO_TBL_LEN];
    DOUBLE ResultAngleTbl[MAX_LENS_DISTO_TBL_LEN];
    DOUBLE Corner2OCDistReal[CAL_DET_CB_MAX_CORNER_NUM];
    DOUBLE Corner2OCDistExpect[CAL_DET_CB_MAX_CORNER_NUM];
    AMBA_CAL_OCCB_CALIB_BOARD_s CalibBoard;
} CT_LDCC_WORKING_BUF_s;


static CT_LDCC_WORKING_BUF_s *pCTLdccWorkingBuf;
static AMBA_CT_LDCC_USER_SETTING_s CTUserLdccGroupData;
static UINT8 LdccTunerValidGroup[AMBA_CT_LDCC_TUNER_MAX];
//static AMBA_CAL_LENS_DST_REAL_EXPECT_s OnePlaneLensDisto;
static void CT_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}
static inline void CT_LdccResetGData2Default(void)
{
    CT_CheckRval( AmbaWrap_memset(&CTUserLdccGroupData, 0x0, sizeof(CTUserLdccGroupData)), "AmbaWrap_memset", __func__);
}

static inline void CT_LdccSetGroupValid(AMBA_CT_LDCC_TUNER_GROUP_e GroupId)
{
    if (GroupId < AMBA_CT_LDCC_TUNER_MAX) {
        LdccTunerValidGroup[GroupId] = 1U;
    }
}

UINT8 AmbaCT_LdccGetGroupStatus(UINT8 GroupId)
{
    return (GroupId < (UINT8)AMBA_CT_LDCC_TUNER_MAX) ? LdccTunerValidGroup[GroupId] : 0U;
}

void AmbaCT_LdccGetSystem(AMBA_CT_LDCC_SYSTEM_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdccGroupData.System, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdccSetSystem(const AMBA_CT_LDCC_SYSTEM_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdccGroupData.System, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_LdccSetGroupValid(AMBA_CT_LDCC_TUNER_SYSTEM_INFO);
}

void AmbaCT_LdccGetSrcInfo(AMBA_CT_LDCC_SRC_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdccGroupData.SrcInfo, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdccSetSrcInfo(const AMBA_CT_LDCC_SRC_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdccGroupData.SrcInfo, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_LdccSetGroupValid(AMBA_CT_LDCC_TUNER_SRC_INFO);
}

void AmbaCT_LdccGetSinglePatDetCircleCfg(AMBA_CT_LDCC_PAT_DET_CIR_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdccGroupData.SinglePlaneCircleDetCfg, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdccSetSinglePatDetCircleCfg(const AMBA_CT_LDCC_PAT_DET_CIR_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdccGroupData.SinglePlaneCircleDetCfg, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_LdccSetGroupValid(AMBA_CT_LDCC_TUNER_SP_PAT_CIRCLE_DET);
}

void AmbaCT_LdccGetMultiPatDetCenterROI(AMBA_CAL_ROI_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdccGroupData.MultiPlaneDetCenterROI, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdccSetMultiPatDetCenterROI(const AMBA_CAL_ROI_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdccGroupData.MultiPlaneDetCenterROI, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_LdccSetGroupValid(AMBA_CT_LDCC_TUNER_MP_PAT_DET_CENTER_ROI);
}


void AmbaCT_LdccGetMultiPatDetCircleCfg(AMBA_CT_LDCC_PAT_DET_CIR_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdccGroupData.MultiPlaneCircleDetCfg, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdccSetMultiPatDetCircleCfg(const AMBA_CT_LDCC_PAT_DET_CIR_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdccGroupData.MultiPlaneCircleDetCfg, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_LdccSetGroupValid(AMBA_CT_LDCC_TUNER_MP_PAT_CIRCLE_DET);
}

void AmbaCT_LdccGetSinglePlaneOrganizeCornerCfg(AMBA_CAL_OCCB_ORGANIZE_CFG_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdccGroupData.SinglePlaneOrganizeCornerCfg, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdccSetSinglePlaneOrganizeCornerCfg(const AMBA_CAL_OCCB_ORGANIZE_CFG_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdccGroupData.SinglePlaneOrganizeCornerCfg, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_LdccSetGroupValid(AMBA_CT_LDCC_TUNER_SP_ORGANIZE_CORNER);
}

void AmbaCT_LdccGetMultiPlaneOrganizeCornerCfg(UINT32 ArrayIndex, AMBA_CAL_OCCB_ORGANIZE_CFG_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdccGroupData.MultiPlaneOrganizeCornerCfg[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdccSetMultiPlaneOrganizeCornerCfg(UINT32 ArrayIndex, const AMBA_CAL_OCCB_ORGANIZE_CFG_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdccGroupData.MultiPlaneOrganizeCornerCfg[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_LdccSetGroupValid(AMBA_CT_LDCC_TUNER_MP_ORGANIZE_CORNER);
}

void AmbaCT_LdccGetCamera(AMBA_CT_LDCC_CAMERA_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdccGroupData.Camera, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdccSetCamera(const AMBA_CT_LDCC_CAMERA_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdccGroupData.Camera, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_LdccSetGroupValid(AMBA_CT_LDCC_TUNER_CAMERA);
}

void AmbaCT_LdccGetOpticalCenter(AMBA_CT_LDCC_OPTICAL_CENTER_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdccGroupData.OpticalCenter, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdccSetOpticalCenter(const AMBA_CT_LDCC_OPTICAL_CENTER_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdccGroupData.OpticalCenter, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_LdccSetGroupValid(AMBA_CT_LDCC_TUNER_OPTICAL_CENTER);
}

void AmbaCT_LdccGetPatternLayoutCfg(UINT32 ArrayIndex, AMBA_CT_LDCC_PAT_LAYOUT_CFG_s **pData)
{
    const AMBA_CT_LDCC_PAT_LAYOUT_CFG_s *pCTData = &CTUserLdccGroupData.PatternLayout[ArrayIndex];
    CT_CheckRval( AmbaWrap_memcpy(pData, &pCTData, sizeof(AMBA_CT_LDCC_PAT_LAYOUT_CFG_s *)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdccSetPatternLayoutCfg(UINT32 ArrayIndex, const AMBA_CT_LDCC_PAT_LAYOUT_CFG_s *pData)
{
    AMBA_CT_LDCC_PAT_LAYOUT_CFG_s *pCTData = &CTUserLdccGroupData.PatternLayout[ArrayIndex];
    if (pData != pCTData) {
        CT_CheckRval( AmbaWrap_memcpy(pCTData, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    }
    CT_LdccSetGroupValid(AMBA_CT_LDCC_TUNER_MP_PAT_LAYOUT_CFG);
}


 void AmbaCT_LdccGetSPOCUsedRange(AMBA_CAL_OCCB_USED_GRID_SIZE_s *pData)
 {
     CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdccGroupData.UsedOCGridSize, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
 }

 void AmbaCT_LdccSetSPOCUsedRange(const AMBA_CAL_OCCB_USED_GRID_SIZE_s *pData)
 {
     CT_CheckRval( AmbaWrap_memcpy(&CTUserLdccGroupData.UsedOCGridSize, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
     CT_LdccSetGroupValid(AMBA_CT_LDCC_TUNER_SP_OC_USED_RANGE);
 }

 void AmbaCT_LdccGetSPCalDistor(AMBA_CT_LDCC_SP_CAL_DISTOR_CFG_s *pData)
 {
     CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdccGroupData.SPCalDistorCfg, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
 }

 void AmbaCT_LdccSetSPCalDistor(const AMBA_CT_LDCC_SP_CAL_DISTOR_CFG_s *pData)
 {
     CT_CheckRval( AmbaWrap_memcpy(&CTUserLdccGroupData.SPCalDistorCfg, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
     CT_LdccSetGroupValid(AMBA_CT_LDCC_TUNER_SP_CAL_DISTOR_CFG);
 }

void AmbaCT_LdccGetMPCalDistor(AMBA_CT_LDCC_MP_CAL_DISTOR_CFG_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdccGroupData.MPCalDistorCfg, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdccSetMPCalDistor(const AMBA_CT_LDCC_MP_CAL_DISTOR_CFG_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdccGroupData.MPCalDistorCfg, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_LdccSetGroupValid(AMBA_CT_LDCC_TUNER_MP_CAL_DISTOR_CFG);
}

#if 0
static const char* CT_LdccGetGroupName(UINT8 GroupId)
{
        static const struct {
        const char *const GroupName;
        AMBA_CT_LDCC_TUNER_GROUP_e GroupId;
    } GroupLut[] = {
        {"CT_LDCC_TUNER_SYSTEM_INFO = 0", AMBA_CT_LDCC_TUNER_SYSTEM_INFO},
        {"CT_LDCC_TUNER_SRC_INFO", AMBA_CT_LDCC_TUNER_SRC_INFO},
        {"CT_LDCC_TUNER_SP_PAT_CIRCLE_DET", AMBA_CT_LDCC_TUNER_SP_PAT_CIRCLE_DET},
        {"CT_LDCC_TUNER_MP_PAT_CIRCLE_DET", AMBA_CT_LDCC_TUNER_MP_PAT_CIRCLE_DET},
        {"CT_LDCC_TUNER_SP_ORGANIZE_CORNER", AMBA_CT_LDCC_TUNER_SP_ORGANIZE_CORNER},
        {"CT_LDCC_TUNER_MP_ORGANIZE_CORNER", AMBA_CT_LDCC_TUNER_MP_ORGANIZE_CORNER},
        {"CT_LDCC_TUNER_CAMERA", AMBA_CT_LDCC_TUNER_CAMERA},
        {"CT_LDCC_TUNER_OPTICAL_CENTER", AMBA_CT_LDCC_TUNER_OPTICAL_CENTER},
        {"CT_LDCC_TUNER_MP_PAT_LAYOUT_CFG", AMBA_CT_LDCC_TUNER_MP_PAT_LAYOUT_CFG},
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
#endif
void AmbaCT_LdccGetWorkingBufSize(SIZE_t *pSize)
{
    SIZE_t PatDetWorkBufSize, OrganizeCornerWorkingBufSize, OcFinderWorkingBufSize, SeparateLineBufSize,Cal1PlaneDistorWorkBufSize,Cal3PlaneDistorWorkBufSize;
    *pSize = sizeof(CT_LDCC_WORKING_BUF_s);
    AmbaCal_DetGetPetDetBufSize(&PatDetWorkBufSize);
    *pSize += PatDetWorkBufSize;
    AmbaCal_OcCbGetOrganizeBufSize(&OrganizeCornerWorkingBufSize);
    *pSize += OrganizeCornerWorkingBufSize;
    AmbaCal_OcCbGetOCFinderBufSize(&OcFinderWorkingBufSize);
    *pSize += OcFinderWorkingBufSize;
    AmbaCal_OcCbSeparateLinesBufSize(&SeparateLineBufSize);
    *pSize += SeparateLineBufSize;
    CT_CheckRval(AmbaCal_LdccGetCal1PlaneBufSize(&Cal1PlaneDistorWorkBufSize), "AmbaCal_LdccGetCal1PlaneBufSize", __func__);
    *pSize += Cal1PlaneDistorWorkBufSize;
    CT_CheckRval(AmbaCal_LdccGetCal3PlaneBufSize(&Cal3PlaneDistorWorkBufSize), "AmbaCal_LdccGetCal3PlaneBufSize", __func__);
    *pSize += Cal3PlaneDistorWorkBufSize;
}


UINT32 AmbaCT_LdccTunerInit(const AMBA_CT_LDCC_INIT_CFG_s *pInitCfg)
{
    UINT32 Rval;
    const UINT8 *pWholeBuff;
    const void *pNextBufStartAddr;
    SIZE_t StartIdx = 0U;
    SIZE_t PatDetWorkBufSize, OrganizeCornerWorkingBufSize, OcFinderWorkingBufSize, SeparateLineBufSize,Cal1PlaneDistorWorkBufSize,Cal3PlaneDistorWorkBufSize;
    CT_LdccResetGData2Default();
    AmbaCal_DetGetPetDetBufSize(&PatDetWorkBufSize);
    AmbaCal_OcCbGetOrganizeBufSize(&OrganizeCornerWorkingBufSize);
    AmbaCal_OcCbGetOCFinderBufSize(&OcFinderWorkingBufSize);
    AmbaCal_OcCbSeparateLinesBufSize(&SeparateLineBufSize);
    Rval = AmbaCal_LdccGetCal1PlaneBufSize(&Cal1PlaneDistorWorkBufSize);
    Rval |=AmbaCal_LdccGetCal3PlaneBufSize(&Cal3PlaneDistorWorkBufSize);

    Rval |= AmbaWrap_memcpy(&pWholeBuff, &pInitCfg->pWorkingBuf, sizeof(pWholeBuff));
    pNextBufStartAddr = &pWholeBuff[StartIdx];
    Rval |= AmbaWrap_memcpy(&pCTLdccWorkingBuf, &pNextBufStartAddr, sizeof(pCTLdccWorkingBuf));
    StartIdx += sizeof(CT_LDCC_WORKING_BUF_s);
    pNextBufStartAddr = &pWholeBuff[StartIdx];
    Rval |=  AmbaWrap_memcpy(&pCTLdccWorkingBuf->pPatDetWorkingBuf, &pNextBufStartAddr, sizeof(pCTLdccWorkingBuf->pPatDetWorkingBuf));
    StartIdx += PatDetWorkBufSize;
    pNextBufStartAddr = &pWholeBuff[StartIdx];
    Rval |=  AmbaWrap_memcpy(&pCTLdccWorkingBuf->pOrganizeCornerWorkingBuf, &pNextBufStartAddr, sizeof(pCTLdccWorkingBuf->pOrganizeCornerWorkingBuf));
    StartIdx += OrganizeCornerWorkingBufSize;
    pNextBufStartAddr = &pWholeBuff[StartIdx];
    Rval |=  AmbaWrap_memcpy(&pCTLdccWorkingBuf->pOCFinderWorkingBuf, &pNextBufStartAddr, sizeof(pCTLdccWorkingBuf->pOCFinderWorkingBuf));
    StartIdx += OcFinderWorkingBufSize;
    pNextBufStartAddr = &pWholeBuff[StartIdx];
    Rval |=  AmbaWrap_memcpy(&pCTLdccWorkingBuf->pPatDetSeparateLineBuf, &pNextBufStartAddr, sizeof(pCTLdccWorkingBuf->pPatDetSeparateLineBuf));
    StartIdx += SeparateLineBufSize;
    pNextBufStartAddr = &pWholeBuff[StartIdx];
    Rval |=  AmbaWrap_memcpy(&pCTLdccWorkingBuf->pCal1PlaneDistorWorkingBuf, &pNextBufStartAddr, sizeof(pCTLdccWorkingBuf->pCal1PlaneDistorWorkingBuf));
    StartIdx += Cal1PlaneDistorWorkBufSize;
    pNextBufStartAddr = &pWholeBuff[StartIdx];
    Rval |=  AmbaWrap_memcpy(&pCTLdccWorkingBuf->pCal3planeDistorWorkingBuf, &pNextBufStartAddr, sizeof(pCTLdccWorkingBuf->pCal3planeDistorWorkingBuf));
    if(Rval != CT_OK) {
        Rval = CT_ERR_1;
    }

#if 0
    StartIdx += Cal3PlaneDistorWorkBufSize;
    pNextBufStartAddr = &pWholeBuff[StartIdx];
#endif
    pCTLdccWorkingBuf->pFeedSinglePlaneSrcFunc = pInitCfg->pFeedSinglePlaneSrcFunc;
    pCTLdccWorkingBuf->pDrawSinglePlaneOrgResult = pInitCfg->pDrawSinglePlaneOrgResult;
    pCTLdccWorkingBuf->pFeedMultiPlaneSrcFunc = pInitCfg->pFeedMultiPlaneSrcFunc;
    pCTLdccWorkingBuf->pDrawMultiPlaneDetResult = pInitCfg->pDrawMultiPlaneDetResult;
    pCTLdccWorkingBuf->pFeedMultiPlanePatInfo = pInitCfg->pFeedMultiPlanePatInfo;
    pCTLdccWorkingBuf->pDrawMultiPlaneDetResultByPlane = pInitCfg->pDrawMultiPlaneDetResultByPlane;
    pCTLdccWorkingBuf->pDrawMultiPlaneOrgResult = pInitCfg->pDrawMultiPlaneOrgResult;
    pCTLdccWorkingBuf->CalibData.OnePlaneRETbl.pRealTbl = &pCTLdccWorkingBuf->OnePlaneRTblResult[0];
    pCTLdccWorkingBuf->CalibData.OnePlaneRETbl.pExpectTbl = &pCTLdccWorkingBuf->OnePlaneETblResult[0];
    pCTLdccWorkingBuf->CalibData.MultiPlaneRETbl.pRealTbl = &pCTLdccWorkingBuf->MultiPlaneRTblResult[0];
    pCTLdccWorkingBuf->CalibData.MultiPlaneRETbl.pExpectTbl = &pCTLdccWorkingBuf->MultiPlaneETblResult[0];
    pCTLdccWorkingBuf->CalibData.RealAngleTbl.pRealTbl = &pCTLdccWorkingBuf->ResultRealTbl[0];
    pCTLdccWorkingBuf->CalibData.RealAngleTbl.pAngleTbl = &pCTLdccWorkingBuf->ResultAngleTbl[0];
    return Rval;
}

static UINT32 CT_LdccDumpCfg(const AMBA_CT_LDCC_USER_SETTING_s *pCfg)
{
    UINT32 Rval;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    pNextMsg = CT_LogPutStr(Msg, "System.CalibMode: ");
    pNextMsg = CT_LogPutStr(pNextMsg, pCfg->System.CalibMode);
    Rval = CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "System.ChipRev: ");
    pNextMsg = CT_LogPutStr(pNextMsg, pCfg->System.ChipRev);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "System.LensType: ");
    pNextMsg = CT_LogPutStr(pNextMsg, pCfg->System.LensType);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "System.OutputPrefix: ");
    pNextMsg = CT_LogPutStr(pNextMsg, pCfg->System.OutputPrefix);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SrcInfo.SinglePlanePatternYFilePath: ");
    pNextMsg = CT_LogPutStr(pNextMsg, pCfg->SrcInfo.SinglePlanePatternYFilePath);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SrcInfo.SinglePlanePatternUVFilePath: ");
    pNextMsg = CT_LogPutStr(pNextMsg, pCfg->SrcInfo.SinglePlanePatternUVFilePath);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SrcInfo.MultiPlanePatternYFilePath: ");
    pNextMsg = CT_LogPutStr(pNextMsg, pCfg->SrcInfo.MultiPlanePatternYFilePath);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SrcInfo.MultiPlanePatternUVFilePath: ");
    pNextMsg = CT_LogPutStr(pNextMsg, pCfg->SrcInfo.MultiPlanePatternUVFilePath);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SrcInfo.IsYUV420: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->SrcInfo.IsYUV420);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SrcInfo.ImageSize.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->SrcInfo.ImageSize.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SrcInfo.ImageSize.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->SrcInfo.ImageSize.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneCircleDetCfg.MaxPerimeter: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->SinglePlaneCircleDetCfg.MaxPerimeter);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneCircleDetCfg.MinPerimeter: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->SinglePlaneCircleDetCfg.MinPerimeter);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneCircleDetCfg.MinDistance: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->SinglePlaneCircleDetCfg.MinDistance);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneCircleDetCfg.MaxPerimeter: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->MultiPlaneCircleDetCfg.MaxPerimeter);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneCircleDetCfg.MinPerimeter: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->MultiPlaneCircleDetCfg.MinPerimeter);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneCircleDetCfg.MinDistance: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->MultiPlaneCircleDetCfg.MinDistance);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneOrganizeCornerCfg.EstToleranceRate: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->SinglePlaneOrganizeCornerCfg.EstToleranceRate);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneOrganizeCornerCfg.CenterNeighborsSearchRadius: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->SinglePlaneOrganizeCornerCfg.CenterNeighborsSearchRadius);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneOrganizeCornerCfg.CenterSearchRadius: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->SinglePlaneOrganizeCornerCfg.CenterSearchRadius);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneOrganizeCornerCfg.SearchRadius: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->SinglePlaneOrganizeCornerCfg.SearchRadius);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneUsedOCGridRangeCfg.MinTop: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->UsedOCGridSize.MinSize.Top);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneUsedOCGridRangeCfg.MinBottom: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->UsedOCGridSize.MinSize.Bottom);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneUsedOCGridRangeCfg.MinLeft: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->UsedOCGridSize.MinSize.Left);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneUsedOCGridRangeCfg.MinRight: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->UsedOCGridSize.MinSize.Right);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneUsedOCGridRangeCfg.MaxTop: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->UsedOCGridSize.MaxSize.Top);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneUsedOCGridRangeCfg.MaxBottom: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->UsedOCGridSize.MaxSize.Bottom);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneUsedOCGridRangeCfg.MaxLeft: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->UsedOCGridSize.MaxSize.Left);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneUsedOCGridRangeCfg.MaxRight: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->UsedOCGridSize.MaxSize.Right);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneCalDistortionCfg.RefDotNum: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->SPCalDistorCfg.RefDotNum);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "SinglePlaneCalDistortionCfg.PolyOrder: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->SPCalDistorCfg.PolyOrder);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneOrganizeCornerCfg[0].EstToleranceRate: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->MultiPlaneOrganizeCornerCfg[0].EstToleranceRate);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneOrganizeCornerCfg[0].CenterNeighborsSearchRadius: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->MultiPlaneOrganizeCornerCfg[0].CenterNeighborsSearchRadius);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneOrganizeCornerCfg[0].CenterSearchRadius: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->MultiPlaneOrganizeCornerCfg[0].CenterSearchRadius);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneOrganizeCornerCfg[0].SearchRadius: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->MultiPlaneOrganizeCornerCfg[0].SearchRadius);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneOrganizeCornerCfg[1].EstToleranceRate: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->MultiPlaneOrganizeCornerCfg[1].EstToleranceRate);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneOrganizeCornerCfg[1].CenterNeighborsSearchRadius: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->MultiPlaneOrganizeCornerCfg[1].CenterNeighborsSearchRadius);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneOrganizeCornerCfg[1].CenterSearchRadius: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->MultiPlaneOrganizeCornerCfg[1].CenterSearchRadius);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneOrganizeCornerCfg[1].SearchRadius: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->MultiPlaneOrganizeCornerCfg[1].SearchRadius);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneOrganizeCornerCfg[2].EstToleranceRate: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->MultiPlaneOrganizeCornerCfg[2].EstToleranceRate);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneOrganizeCornerCfg[2].CenterNeighborsSearchRadius: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->MultiPlaneOrganizeCornerCfg[2].CenterNeighborsSearchRadius);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneOrganizeCornerCfg[2].CenterSearchRadius: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->MultiPlaneOrganizeCornerCfg[2].CenterSearchRadius);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneOrganizeCornerCfg[2].SearchRadius: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->MultiPlaneOrganizeCornerCfg[2].SearchRadius);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneCalDistortionCfg.PlaneNum: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->MPCalDistorCfg.TotalPlane);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneCalDistortionCfg.SeparateCalStepNum: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->MPCalDistorCfg.SeparateStepNum);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneCalDistortionCfg.RefMaxAngleDeg: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->MPCalDistorCfg.RefMaxAngleDeg);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "MultiPlaneCalDistortionCfg.PolyOrder: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->MPCalDistorCfg.PolyOrder);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "Camera.CellSize: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Camera.CellSize);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "Camera.DistTableUnit: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Camera.DistTableUnit);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "Camera.TableLen: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Camera.TableLen);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "Camera.RealTable: ");
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "Camera.ExceptTable: ");
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "OpticalCenter.IdealPos.X: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->OpticalCenter.IdealPos.X);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "OpticalCenter.IdealPos.Y: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->OpticalCenter.IdealPos.Y);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "OpticalCenter.SearchRange.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->OpticalCenter.SearchRange.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "OpticalCenter.SearchRange.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->OpticalCenter.SearchRange.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[0].FilePath: ");
    pNextMsg = CT_LogPutStr(pNextMsg, pCfg->PatternLayout[0].FilePath);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[0].LayoutCfg.RowNum: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->PatternLayout[0].LayoutCfg.RowNum);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[0].LayoutCfg.ColNum: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->PatternLayout[0].LayoutCfg.ColNum);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[0].LayoutCfg.Center2CenterDistance: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->PatternLayout[0].LayoutCfg.Center2CenterDistance);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[0].LayoutCfg.ImgRotate: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->PatternLayout[0].LayoutCfg.ImgRotate);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[0].LayoutCfg.StartPos.X: ");
    pNextMsg = CT_LogPutS32(pNextMsg, pCfg->PatternLayout[0].LayoutCfg.StartPos.X);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[0].LayoutCfg.StartPos.Y: ");
    pNextMsg = CT_LogPutS32(pNextMsg, pCfg->PatternLayout[0].LayoutCfg.StartPos.Y);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);


    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[1].FilePath: ");
    pNextMsg = CT_LogPutStr(pNextMsg, pCfg->PatternLayout[1].FilePath);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[1].LayoutCfg.RowNum: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->PatternLayout[1].LayoutCfg.RowNum);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[1].LayoutCfg.ColNum: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->PatternLayout[1].LayoutCfg.ColNum);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[1].LayoutCfg.Center2CenterDistance: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->PatternLayout[1].LayoutCfg.Center2CenterDistance);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[1].LayoutCfg.ImgRotate: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->PatternLayout[1].LayoutCfg.ImgRotate);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[1].LayoutCfg.StartPos.X: ");
    pNextMsg = CT_LogPutS32(pNextMsg, pCfg->PatternLayout[1].LayoutCfg.StartPos.X);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[1].LayoutCfg.StartPos.Y: ");
    pNextMsg = CT_LogPutS32(pNextMsg, pCfg->PatternLayout[1].LayoutCfg.StartPos.Y);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[2].FilePath: ");
    pNextMsg = CT_LogPutStr(pNextMsg, pCfg->PatternLayout[2].FilePath);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[2].LayoutCfg.RowNum: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->PatternLayout[2].LayoutCfg.RowNum);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[2].LayoutCfg.ColNum: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->PatternLayout[2].LayoutCfg.ColNum);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[2].LayoutCfg.Center2CenterDistance: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->PatternLayout[2].LayoutCfg.Center2CenterDistance);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[2].LayoutCfg.ImgRotate: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->PatternLayout[2].LayoutCfg.ImgRotate);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[2].LayoutCfg.StartPos.X: ");
    pNextMsg = CT_LogPutS32(pNextMsg, pCfg->PatternLayout[2].LayoutCfg.StartPos.X);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "PatternLayout[2].LayoutCfg.StartPos.Y: ");
    pNextMsg = CT_LogPutS32(pNextMsg, pCfg->PatternLayout[2].LayoutCfg.StartPos.Y);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    return Rval;
}
#if 0
#include "AmbaFS.h"
static void debug_corner(AMBA_CAL_POINT_DB_2D_s *pCorner, AMBA_CAL_DET_CORNER_TYPE_s *pCornerType, UINT32 CornerNum)
{
    AMBA_FS_FILE *Fd;
    Fd = AmbaFS_fopen("det_corner_list.bin", "wb");
    AmbaFS_fwrite(pCorner, sizeof(AMBA_CAL_POINT_DB_2D_s) * CornerNum, 1, Fd);
    AmbaFS_fclose(Fd);
    for (UINT32 Index = 0; Index < CornerNum; Index++) {
        printf("%f %f\n", pCorner[Index].X, pCorner[Index].Y);
    }
    if (pCornerType != NULL) {
        Fd = AmbaFS_fopen("det_corner_type_list.bin", "wb");
        AmbaFS_fwrite(pCornerType, sizeof(AMBA_CAL_DET_CORNER_TYPE_s) * CornerNum, 1, Fd);
        AmbaFS_fclose(Fd);
    }
}
#endif
static UINT32 CT_LdccFisheyeSinglePlaneExecute(void)
{
    // Note # Single plane process
    UINT32 Rval;
    AMBA_CAL_DET_CIRCLE_DET_CFG_s CircleDetCfg;
    UINT32 CornerNum = 0U;
    AMBA_CAL_YUV_INFO_s YUVInfo;
    Rval = CT_LdccDumpCfg(&CTUserLdccGroupData);

    YUVInfo.Format = (CTUserLdccGroupData.SrcInfo.IsYUV420 == 0U) ? AMBA_CAL_YUV_422 : AMBA_CAL_YUV_420;
    YUVInfo.pY = pCTLdccWorkingBuf->YImgBuf;
    YUVInfo.pUV = NULL;
    Rval |= pCTLdccWorkingBuf->pFeedSinglePlaneSrcFunc(&CTUserLdccGroupData.SrcInfo.ImageSize, &YUVInfo);

    if (Rval == CAL_OK) {
        CircleDetCfg.Src.ImgSize = CTUserLdccGroupData.SrcInfo.ImageSize;
        CircleDetCfg.MinContourLength = CTUserLdccGroupData.SinglePlaneCircleDetCfg.MinPerimeter;
        CircleDetCfg.MaxContourLength = CTUserLdccGroupData.SinglePlaneCircleDetCfg.MaxPerimeter;
        CircleDetCfg.MaskRadius = CTUserLdccGroupData.SinglePlaneCircleDetCfg.MinDistance;
        CircleDetCfg.Src.pSrcAddr = YUVInfo.pY;
        Rval |= AmbaCal_DetCircleCorners(&CircleDetCfg, pCTLdccWorkingBuf->pPatDetWorkingBuf, pCTLdccWorkingBuf->SubPixCorners, &CornerNum);
#if 0
        debug_corner(pCTLdccWorkingBuf->SubPixCorners, NULL, CornerNum);
#endif
    } else {
        AmbaPrint_PrintStr5("%s(), FeedSinglePlanSrcFunc Fail", __func__, NULL, NULL, NULL, NULL);
    }

    if (Rval == CAL_OK) {
        Rval |= AmbaCal_OcCbOrganizeCorners(&CTUserLdccGroupData.SinglePlaneOrganizeCornerCfg,
                pCTLdccWorkingBuf->SubPixCorners,
                CornerNum,
                &CTUserLdccGroupData.SrcInfo.ImageSize,
                pCTLdccWorkingBuf->pOrganizeCornerWorkingBuf,
                &pCTLdccWorkingBuf->OrganizedCorners[0]);
        if (pCTLdccWorkingBuf->pDrawSinglePlaneOrgResult != NULL) {
            Rval |= pCTLdccWorkingBuf->pDrawSinglePlaneOrgResult(pCTLdccWorkingBuf->SubPixCorners, &pCTLdccWorkingBuf->OrganizedCorners[0], YUVInfo.pY);
        }
    } else {
        AmbaPrint_PrintStr5("%s(), AmbaCal_DetCircleCorners Fail", __func__, NULL, NULL, NULL, NULL);
    }

    if (Rval == CAL_OK) {
        AMBA_CAL_CAM_s CamSpec;
        AMBA_CAL_LENS_DST_REAL_EXPECT_s RealExpect;
        CamSpec.Lens.LensDistoType = AMBA_CAL_LD_REAL_EXPECT_TBL;
        CamSpec.Lens.LensDistoUnit = (CTUserLdccGroupData.Camera.DistTableUnit == 0U) ? AMBA_CAL_LD_MM : AMBA_CAL_LD_PIXEL;
        CamSpec.Lens.LensDistoSpec.pRealExpect = &RealExpect;
        CamSpec.Lens.LensDistoSpec.pRealExpect->Length = CTUserLdccGroupData.Camera.TableLen;
        CamSpec.Lens.LensDistoSpec.pRealExpect->pRealTbl = CTUserLdccGroupData.Camera.RealTable;
        CamSpec.Lens.LensDistoSpec.pRealExpect->pExpectTbl = CTUserLdccGroupData.Camera.ExceptTable;

        CamSpec.Sensor.CellSize = CTUserLdccGroupData.Camera.CellSize;
        CamSpec.Sensor.StartX = 0U;
        CamSpec.Sensor.StartY = 0U;
        CamSpec.Sensor.Width = CTUserLdccGroupData.SrcInfo.ImageSize.Width;
        CamSpec.Sensor.Height = CTUserLdccGroupData.SrcInfo.ImageSize.Height;
        CamSpec.OpticalCenter.X = CTUserLdccGroupData.OpticalCenter.IdealPos.X;
        CamSpec.OpticalCenter.Y = CTUserLdccGroupData.OpticalCenter.IdealPos.Y;
        CamSpec.Pos.X = 0.0;
        CamSpec.Pos.Y = 0.0;
        CamSpec.Pos.Z = 0.0;
        CamSpec.Rotation = AMBA_CAL_ROTATE_0;
        Rval |= AmbaCal_OcCbOpticalCenterFindV1(&CamSpec,
                &CTUserLdccGroupData.OpticalCenter.SearchRange,
                pCTLdccWorkingBuf->SubPixCorners,
                pCTLdccWorkingBuf->OrganizedCorners[0].pCorner2dMap,
                &pCTLdccWorkingBuf->OrganizedCorners[0].Corner2dMapValidArea,
                &CTUserLdccGroupData.UsedOCGridSize,
                pCTLdccWorkingBuf->pOCFinderWorkingBuf,
                &pCTLdccWorkingBuf->CalibData.OpticalCenter);
    } else {
        AmbaPrint_PrintStr5("%s(), AmbaCal_OcCbOrganizeCorners Fail", __func__, NULL, NULL, NULL, NULL);
    }

   if (Rval == CAL_OK) {
        AMBA_CAL_CAM_V2_s CamSpecV2;
        AMBA_CAL_LENS_DST_REAL_EXPECT_s RealExpect;
        AMBA_CAL_DET_CORNER_LIST_s CornerList;
        UINT32 RefDotNum;
        UINT32 PolyOrder;
        DOUBLE FocalLength;
        UINT32 DistNumber;

        CamSpecV2.Sensor.CellSize = CTUserLdccGroupData.Camera.CellSize;
        CamSpecV2.Sensor.StartX = 0U;
        CamSpecV2.Sensor.StartY = 0U;
        CamSpecV2.Sensor.Width = CTUserLdccGroupData.SrcInfo.ImageSize.Width;
        CamSpecV2.Sensor.Height = CTUserLdccGroupData.SrcInfo.ImageSize.Height;
        CamSpecV2.OpticalCenter.X = pCTLdccWorkingBuf->CalibData.OpticalCenter.X;
        CamSpecV2.OpticalCenter.Y = pCTLdccWorkingBuf->CalibData.OpticalCenter.Y;
        CamSpecV2.Pos.X = 0.0;
        CamSpecV2.Pos.Y = 0.0;
        CamSpecV2.Pos.Z = 0.0;
        CamSpecV2.Rotation = AMBA_CAL_ROTATE_0;

        CamSpecV2.Lens.LensDistoType = AMBA_CAL_LD_REAL_EXPECT_TBL;
        CamSpecV2.Lens.LensDistoUnit = (CTUserLdccGroupData.Camera.DistTableUnit == 0U) ? AMBA_CAL_LD_MM : AMBA_CAL_LD_PIXEL;
        CamSpecV2.Lens.LensDistoSpec.pRealExpect = &RealExpect;
        CamSpecV2.Lens.LensDistoSpec.pRealExpect->Length = CTUserLdccGroupData.Camera.TableLen;
        CamSpecV2.Lens.LensDistoSpec.pRealExpect->pRealTbl = CTUserLdccGroupData.Camera.RealTable;
        CamSpecV2.Lens.LensDistoSpec.pRealExpect->pExpectTbl = CTUserLdccGroupData.Camera.ExceptTable;

        CornerList.CornerNum = CornerNum;
        CornerList.pCornerPosList = &pCTLdccWorkingBuf->SubPixCorners[0];
        RefDotNum = CTUserLdccGroupData.SPCalDistorCfg.RefDotNum;
        PolyOrder = CTUserLdccGroupData.SPCalDistorCfg.PolyOrder;
        FocalLength = CTUserLdccGroupData.Camera.FocalLength;


        Rval |= AmbaCal_LdccCalib1PlaneDistoTbl( pCTLdccWorkingBuf->pCal1PlaneDistorWorkingBuf,
                                                    &CamSpecV2,
                                                    &CornerList,
                                                    pCTLdccWorkingBuf->OrganizedCorners[0].pCorner2dMap,
                                                    &pCTLdccWorkingBuf->OrganizedCorners[0].Corner2dMapValidArea,
                                                    RefDotNum,
                                                    PolyOrder,
                                                    FocalLength,
                                                    &pCTLdccWorkingBuf->CalibData.OnePlaneRETbl,
                                                    &pCTLdccWorkingBuf->Corner2OCDistReal[0],
                                                    &pCTLdccWorkingBuf->Corner2OCDistExpect[0],
                                                    &DistNumber);

    } else {
        AmbaPrint_PrintStr5("%s(), AmbaCal_LdccCalib1PlaneDistoTbl Fail", __func__, NULL, NULL, NULL, NULL);
    }


    // TODO # LDCC 1st
    return Rval;
}

static UINT32 CT_LdccFisheyeMultiPlaneExecute(void)
{
    // Note # Single plane process
    UINT32 Rval;
    AMBA_CAL_DET_CIRCLE_DET_CFG_s CircleDetCfg;
    UINT32 CornerNum = 0U;
    AMBA_CAL_YUV_INFO_s YUVInfo;
    AMBA_CAL_DET_CORNER_LIST_s TotalCornersPosTypeList, CornersPosTypeList[3];

    UINT32 Index;
    Rval = CT_LdccDumpCfg(&CTUserLdccGroupData);

    YUVInfo.Format = (CTUserLdccGroupData.SrcInfo.IsYUV420 == 0U) ? AMBA_CAL_YUV_422 : AMBA_CAL_YUV_420;
    YUVInfo.pY = pCTLdccWorkingBuf->YImgBuf;
    YUVInfo.pUV = NULL;
    Rval |= pCTLdccWorkingBuf->pFeedMultiPlaneSrcFunc(&CTUserLdccGroupData.SrcInfo.ImageSize, &YUVInfo);
    if (Rval == CAL_OK) {
        CircleDetCfg.Src.ImgSize = CTUserLdccGroupData.SrcInfo.ImageSize;
        CircleDetCfg.MinContourLength = CTUserLdccGroupData.MultiPlaneCircleDetCfg.MinPerimeter;
        CircleDetCfg.MaxContourLength = CTUserLdccGroupData.MultiPlaneCircleDetCfg.MaxPerimeter;
        CircleDetCfg.MaskRadius = CTUserLdccGroupData.MultiPlaneCircleDetCfg.MinDistance;
        CircleDetCfg.Src.pSrcAddr = YUVInfo.pY;
        Rval |= AmbaCal_DetCircleCorners(&CircleDetCfg, pCTLdccWorkingBuf->pPatDetWorkingBuf, pCTLdccWorkingBuf->SubPixCorners, &CornerNum);
    } else {
        AmbaPrint_PrintStr5("%s(), pFeedMultiPlaneSrcFunc Fail", __func__, NULL, NULL, NULL, NULL);
    }

    YUVInfo.pUV = pCTLdccWorkingBuf->UVImgBuf;
    // Note # AmbaCal_DetCircleCorners will overwrite img buf
    Rval |= pCTLdccWorkingBuf->pFeedMultiPlaneSrcFunc(&CTUserLdccGroupData.SrcInfo.ImageSize, &YUVInfo);
    TotalCornersPosTypeList.CornerNum = CornerNum;
    TotalCornersPosTypeList.pCornerPosList = pCTLdccWorkingBuf->SubPixCorners;
    TotalCornersPosTypeList.pCornersTypeList = pCTLdccWorkingBuf->TotalCornersTypeList;
    //Rval |= AmbaCal_DetGetCircleCornersType(&YUVInfo, &CTUserLdccGroupData.SrcInfo.ImageSize, &TotalCornersPosTypeList);




#if 0
    debug_corner(pCTLdccWorkingBuf->SubPixCorners, pCTLdccWorkingBuf->TotalCornersTypeList, CornerNum);
#endif


    if (pCTLdccWorkingBuf->pDrawMultiPlaneDetResult != NULL) {
        Rval |= pCTLdccWorkingBuf->pDrawMultiPlaneDetResult(&TotalCornersPosTypeList, YUVInfo.pY);
        Rval |= pCTLdccWorkingBuf->pFeedMultiPlaneSrcFunc(&CTUserLdccGroupData.SrcInfo.ImageSize, &YUVInfo);
    }

    for (Index = 0U; Index < 3U; Index++) {
        CornersPosTypeList[Index].CornerNum = 0;
        CornersPosTypeList[Index].pCornerPosList = pCTLdccWorkingBuf->MultiPlaneSubPixCorners[Index];
        CornersPosTypeList[Index].pCornersTypeList = pCTLdccWorkingBuf->MultiPlaneCornersTypeList[Index];
    }

    Rval |= AmbaCal_OcCbCornerTypePlaneClass(pCTLdccWorkingBuf->pPatDetSeparateLineBuf, &YUVInfo, &CTUserLdccGroupData.SrcInfo.ImageSize, &CTUserLdccGroupData.MultiPlaneDetCenterROI,
            &TotalCornersPosTypeList, &CornersPosTypeList[0], &CornersPosTypeList[1], &CornersPosTypeList[2]);


    if (pCTLdccWorkingBuf->pDrawMultiPlaneDetResultByPlane != NULL) {
        Rval |= pCTLdccWorkingBuf->pDrawMultiPlaneDetResultByPlane(&CornersPosTypeList[0], &CornersPosTypeList[1], &CornersPosTypeList[2], YUVInfo.pY);
    }

    for (Index = 0U; Index < 3U; Index++) {
        AMBA_CAL_OCCB_CALIB_BOARD_s *pCalibBoard = &pCTLdccWorkingBuf->CalibBoard;
        AMBA_CAL_OCCB_CIRCLE_GRID_PATTERN_s *pCirclePatternInfo;
        const AMBA_CAL_OCCB_PATTERN_UNION_DATA_s *pPatternUnionInfo;
        pCalibBoard->BoardInfo.Type = AMBA_CAL_OCCB_CIRCLE_GRID;
        pPatternUnionInfo = &pCalibBoard->PatternInfo.Type;
        Rval |=  AmbaWrap_memcpy(&pCirclePatternInfo, &pPatternUnionInfo, sizeof(AMBA_CAL_OCCB_CIRCLE_GRID_PATTERN_s *));
        *pCirclePatternInfo = CTUserLdccGroupData.PatternLayout[Index].LayoutCfg;
        if (pCTLdccWorkingBuf->pFeedMultiPlanePatInfo != NULL) {
            Rval |= pCTLdccWorkingBuf->pFeedMultiPlanePatInfo(Index, &pCirclePatternInfo->CircleInfo[0]);
        } else {
            Rval = CT_ERR_1;
        }
        pCTLdccWorkingBuf->OrganizedCorners[Index].pCorner2dMap = &pCTLdccWorkingBuf->MultiPlaneOrganizedCorners2dMap[Index][0];
        Rval |= AmbaCal_OcCbOrganizeCornersNook(&CTUserLdccGroupData.MultiPlaneOrganizeCornerCfg[Index], &CornersPosTypeList[Index], &CTUserLdccGroupData.SrcInfo.ImageSize,
                pCalibBoard, pCTLdccWorkingBuf->pOrganizeCornerWorkingBuf, &pCTLdccWorkingBuf->OrganizedCorners[2]);
        pCTLdccWorkingBuf->OrganizedCorners[Index].Corner2dMapValidArea.StartX = pCTLdccWorkingBuf->OrganizedCorners[2].Corner2dMapValidArea.StartX;
        pCTLdccWorkingBuf->OrganizedCorners[Index].Corner2dMapValidArea.StartY = pCTLdccWorkingBuf->OrganizedCorners[2].Corner2dMapValidArea.StartY;
        pCTLdccWorkingBuf->OrganizedCorners[Index].Corner2dMapValidArea.Width = pCTLdccWorkingBuf->OrganizedCorners[2].Corner2dMapValidArea.Width;
        pCTLdccWorkingBuf->OrganizedCorners[Index].Corner2dMapValidArea.Height = pCTLdccWorkingBuf->OrganizedCorners[2].Corner2dMapValidArea.Height;
        Rval |= AmbaWrap_memcpy(&pCTLdccWorkingBuf->OrganizedCorners[Index].pCorner2dMap[0], &pCTLdccWorkingBuf->OrganizedCorners[2].pCorner2dMap[0], sizeof(pCTLdccWorkingBuf->MultiPlaneOrganizedCorners2dMap[Index]));
        Rval |= AmbaWrap_memcpy(&pCTLdccWorkingBuf->MultiPlaneOrganizedCorners2dMap[Index][0], pCTLdccWorkingBuf->OrganizedCorners[Index].pCorner2dMap, sizeof(pCTLdccWorkingBuf->MultiPlaneOrganizedCorners2dMap[Index]));

        if (pCTLdccWorkingBuf->pDrawMultiPlaneOrgResult != NULL) {
            Rval |= pCTLdccWorkingBuf->pDrawMultiPlaneOrgResult(Index, CornersPosTypeList[Index].pCornerPosList, &pCTLdccWorkingBuf->OrganizedCorners[Index], YUVInfo.pY);
        } else {
            AmbaPrint_PrintStr5("%s(), pDrawMultiPlaneOrgResult is NULL\n", __func__, NULL, NULL, NULL, NULL);
        }
    }

    if (Rval == CAL_OK) {
        AMBA_CAL_CAM_V2_s CamSpecV2;
        const AMBA_CAL_DET_CORNER_LIST_s *pCorners[3];
        const INT16 *pCorner2dMap[3];
        AMBA_CAL_LENS_DST_REAL_EXPECT_s CamRealExpect;

        UINT32 SeparateStepNum;
        DOUBLE RefMaxAngleDeg;
        UINT32 TotalPlane;
        UINT32 PolyOrder;
        DOUBLE FocalLength;



        CamSpecV2.Sensor.CellSize = CTUserLdccGroupData.Camera.CellSize;
        CamSpecV2.Sensor.StartX = 0U;
        CamSpecV2.Sensor.StartY = 0U;
        CamSpecV2.Sensor.Width = CTUserLdccGroupData.SrcInfo.ImageSize.Width;
        CamSpecV2.Sensor.Height = CTUserLdccGroupData.SrcInfo.ImageSize.Height;
        CamSpecV2.OpticalCenter.X = pCTLdccWorkingBuf->CalibData.OpticalCenter.X;
        CamSpecV2.OpticalCenter.Y = pCTLdccWorkingBuf->CalibData.OpticalCenter.Y;
        CamSpecV2.Pos.X = 0.0;
        CamSpecV2.Pos.Y = 0.0;
        CamSpecV2.Pos.Z = 0.0;
        CamSpecV2.Rotation = AMBA_CAL_ROTATE_0;

        CamSpecV2.Lens.LensDistoType = AMBA_CAL_LD_REAL_EXPECT_TBL;
        CamSpecV2.Lens.LensDistoUnit = (CTUserLdccGroupData.Camera.DistTableUnit == 0U) ? AMBA_CAL_LD_MM : AMBA_CAL_LD_PIXEL;
        CamSpecV2.Lens.LensDistoSpec.pRealExpect = &CamRealExpect;
        CamSpecV2.Lens.LensDistoSpec.pRealExpect->Length = CTUserLdccGroupData.Camera.TableLen;
        CamSpecV2.Lens.LensDistoSpec.pRealExpect->pRealTbl = CTUserLdccGroupData.Camera.RealTable;
        CamSpecV2.Lens.LensDistoSpec.pRealExpect->pExpectTbl = CTUserLdccGroupData.Camera.ExceptTable;



        pCorners[0] = &CornersPosTypeList[0];
        pCorners[1] = &CornersPosTypeList[1];
        pCorners[2] = &CornersPosTypeList[2];
        pCorner2dMap[0] = &pCTLdccWorkingBuf->OrganizedCorners[0].pCorner2dMap[0];
        pCorner2dMap[1] = &pCTLdccWorkingBuf->OrganizedCorners[1].pCorner2dMap[0];
        pCorner2dMap[2] = &pCTLdccWorkingBuf->OrganizedCorners[2].pCorner2dMap[0];


        SeparateStepNum = CTUserLdccGroupData.MPCalDistorCfg.SeparateStepNum;
        RefMaxAngleDeg = CTUserLdccGroupData.MPCalDistorCfg.RefMaxAngleDeg;
        TotalPlane = CTUserLdccGroupData.MPCalDistorCfg.TotalPlane;
        PolyOrder = CTUserLdccGroupData.MPCalDistorCfg.PolyOrder;
        FocalLength = CTUserLdccGroupData.Camera.FocalLength;

        Rval |= AmbaCal_LdccCalib3PlaneDistoTbl( pCTLdccWorkingBuf->pCal3planeDistorWorkingBuf,
                                                &CamSpecV2,
                                                &pCTLdccWorkingBuf->CalibData.OnePlaneRETbl,
                                                pCorners,
                                                pCorner2dMap,
                                                TotalPlane,
                                                PolyOrder,
                                                FocalLength,
                                                SeparateStepNum,
                                                RefMaxAngleDeg,
                                                &pCTLdccWorkingBuf->CalibData.MultiPlaneRETbl,
                                                &pCTLdccWorkingBuf->CalibData.RealAngleTbl);

    } else {
            AmbaPrint_PrintStr5("%s(), CT_LdccFisheyeMultiPlaneExecute is NULL\n", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

UINT32 AmbaCT_LdccGetCalibData(AMBA_CT_LDCC_CALIB_DATA_s *pLdccData)
{
    *pLdccData = pCTLdccWorkingBuf->CalibData;
    return CT_OK;
}


UINT32 AmbaCT_LdccExecute(void)
{
    UINT32 Rval;
    Rval = CT_LdccFisheyeSinglePlaneExecute();
    Rval |= CT_LdccFisheyeMultiPlaneExecute();
    return Rval;
}

const AMBA_CT_LDCC_USER_SETTING_s* AmbaCT_LdccGetUserSetting(void)
{
    return (const AMBA_CT_LDCC_USER_SETTING_s*)&CTUserLdccGroupData;
}
