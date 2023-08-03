/**
 *  @file SvcAnimCalib.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 *
 *  @details Calib mode
 *
 */
#include "AmbaShell.h"
#include <AmbaCalib_AVMIF.h>
#include <AmbaCT_AvmTunerIF.h>
#include <AmbaCT_AvmTuner.h>
#include <AmbaCT_TextHdlr.h>
#include <AmbaYuv.h>
#include <AmbaYuv_LT6911.h>
#include <AmbaNAND_FTL.h>
#include <AmbaNAND_Ctrl.h>
#include <AmbaSD_Def.h>
#include <AmbaDSP_VOUT_Def.h>
#include "AmbaUtility.h"
#include "AmbaFS.h"
#include "AmbaCalib_EmirrorDef.h"

#include "SvcWrap.h"
#include "SvcErrCode.h"
#include "SvcAnimCalib.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcCmd.h"
#include "SvcCalibAdas.h"

#define SVC_CALIB_VERSION                      (0x20200214U)

#define SVC_CALIB_ITEM_ID_WARP                 (0U)
#define SVC_CALIB_ITEM_ID_BLEND                (1U)
#define SVC_CALIB_ITEM_ID_OSD                  (2U)

#define SVC_CALIB_3D_ITEM_COUNT                (3U)

#define SVC_CALIB_DISP_ID_3D                   (0U)
#define SVC_CALIB_DISP_COUNT                   (1U)

#define SVC_CALIB_VIEW_COUNT                   (1U)


typedef struct {
    UINT8 *CmprData;
    UINT32 CmprSize;
    UINT32 DecmprDataSize;
    AMBA_DSP_WINDOW_s Window;
} SVC_CALIB_OSD_INFO_s;

typedef struct {
    UINT32 GridStatus[MAX_WARP_TBL_LEN];
    UINT32 HorTileNum;
    UINT32 VerTileNum;
} AMBA_SVC_CALIB_AVM_GRID_STATUS_s;

static AMBA_SVC_CALIB_AVM_GRID_STATUS_s g_AvmGridStatus[AMBA_CAL_AVM_CAM_MAX];
static UINT8 g_DbgFlag = 0U;
static UINT32 g_CalibDataHdlr;
static UINT32 InitBufferSize;

static inline UINT16 U16MAX(UINT16 a, UINT16 b) {return ((a > b) ? a : b);}

static inline void SvcAnimCalib_Perror(const char *Func, UINT32 Line, const char *Message, UINT32 Rval)
{
    char RvalStr[SVC_ANIM_MAX_NAME_LENGTH];
    char LineStr[SVC_ANIM_MAX_NAME_LENGTH];
    (void)AmbaUtility_UInt32ToStr(RvalStr, sizeof(RvalStr), Rval, 10);
    (void)AmbaUtility_UInt32ToStr(LineStr, sizeof(LineStr), Line, 10);
    AmbaPrint_PrintStr5("%s(%s): %s() failed! Rval = %s", Func, LineStr, Message, RvalStr, NULL);
}

static UINT32 SvcAnimCalib_Tuner(const char *filename)
{
    static UINT32 BufSize = 0U;
    static ULONG BufBase = 0U;
    UINT32 Rval = SVC_OK;
    if (BufBase == 0U) {
        Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CAL_SUR_CT, &BufBase, &BufSize);
    }
    if (Rval == SVC_OK) {
        AMBA_CT_INITIAL_CONFIG_s InitCfg;
        AmbaSvcWrap_MisraMemset(&InitCfg, 0x0, sizeof(AMBA_CT_INITIAL_CONFIG_s));
        InitCfg.TunerWorkingBufSize = BufSize;
        AmbaMisra_TypeCast(&(InitCfg.pTunerWorkingBuf), &(BufBase));
        Rval = AmbaCT_Init(AMBA_CT_TYPE_AVM, &InitCfg);
        if (Rval == SVC_OK) {
            Rval = AmbaCT_Load(filename);
            if (Rval != SVC_OK) {
                SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCT_Load", Rval);
            }
        } else {
            SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCT_Init", Rval);
        }
    }
    return Rval;
}

static UINT32 SvcAnimCalib_AvmString2View3DModelType(const char *pViewModelType, AMBA_CAL_AVM_3D_VIEW_MODEL_e *ViewModel)
{
    UINT32 Rval = SVC_OK;
    if (pViewModelType == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: pViewModelType is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else if (ViewModel == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: ViewModel is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        if (AmbaUtility_StringCompare(pViewModelType, "ADVANCE", AmbaUtility_StringLength("ADVANCE")) == 0) {
            *ViewModel = AMBA_CAL_AVM_3D_VIEW_MODEL_ADV;
        } else if (AmbaUtility_StringCompare(pViewModelType, "SIMPLE", AmbaUtility_StringLength("SIMPLE")) == 0) {
            *ViewModel = AMBA_CAL_AVM_3D_VIEW_MODEL_SIM;
        } else {
            *ViewModel = AMBA_CAL_AVM_3D_VIEW_MODEL_SIM;
        }
    }
    return Rval;
}

static UINT32 SvcAnimCalib_SaveVoutOsdInfo(const AMBA_CAL_AVM_3D_DATA_s *CalOutput)
{
    SVC_CALIB_ADAS_NAND_TABLE_s Cfg;
    SVC_CALIB_ADAS_NAND_TABLE_s *pCfg = &Cfg;
    UINT32 Chan, RetVal, s;
    UINT32 TmpX = 0U, TmpY = 0U, TmpH = 0U, TmpW = 0U;

    Chan = SVC_CALIB_ADAS_TYPE_AVM_F;
    AmbaSvcWrap_MisraMemset(&Cfg, 0, sizeof(SVC_CALIB_ADAS_NAND_TABLE_s));
    RetVal = SvcCalib_AdasCfgGet(Chan, pCfg);
    if (RetVal == SVC_OK) {
        for (s = 0U; s < CalOutput->VoutCarOSDCornerNum;s++) {
            if (TmpX < (UINT32)CalOutput->VoutCarOSDCorner[s].X) {
                TmpW = (UINT32)CalOutput->VoutCarOSDCorner[s].X - TmpX;
                TmpX = (UINT32)CalOutput->VoutCarOSDCorner[s].X;
            }

            if (TmpY < (UINT32)CalOutput->VoutCarOSDCorner[s].Y) {
                TmpH = (UINT32)CalOutput->VoutCarOSDCorner[s].Y - TmpY;
                TmpY = (UINT32)CalOutput->VoutCarOSDCorner[s].Y;
            }
        }
        Cfg.AdasAvm2DCfg.CarVoutOSD.Width = TmpW;
        Cfg.AdasAvm2DCfg.CarVoutOSD.Height= TmpH;
        Cfg.AdasAvmPaCfg.Cfg.pPerspective = NULL; // Do Not Rewrite value
        Cfg.AdasAvmPaCfg.Cfg.pEqualDistance = NULL;// Do Not Rewrite value
        Cfg.AdasAvmPaCfg.Cfg.pLDC = NULL;// Do Not Rewrite value
        Cfg.AdasAvmPaCfg.Cfg.pFloor = NULL;// Do Not Rewrite value
        Cfg.AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect = NULL;// Do Not Rewrite value
        (void)SvcCalib_AdasCfgSet(SVC_CALIB_ADAS_TYPE_AVM_F, pCfg);
    }
    return RetVal;
}

UINT32 SvcAnimCalib_AvmFeedLensSpec(const AMBA_CT_AVM_LENS_s *pIn,
    AMBA_CAL_LENS_SPEC_s *pOut,
    AMBA_CAL_LENS_DST_REAL_EXPECT_s *pRealExpectBuf,
    AMBA_CAL_LENS_DST_ANGLE_s *pAngleBuf,
    AMBA_CAL_LENS_DST_FORMULA_s *pFormulaBuf)
{
    UINT32 Rval = SVC_OK;
    if (pIn == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: pIn is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else if (pOut == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: pOut is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else if (pRealExpectBuf == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: pRealExpectBuf is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else if (pAngleBuf == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: pAngleBuf is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else if (pFormulaBuf == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: pFormulaBuf is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        pOut->LensDistoType = pIn->LensDistoType;
        switch(pIn->LensDistoType) {
        case AMBA_CAL_LD_REAL_EXPECT_TBL:
            pRealExpectBuf->Length = pIn->TableLen;
            pRealExpectBuf->pRealTbl = pIn->pRealTable;
            pRealExpectBuf->pExpectTbl = pIn->pExceptTable;
            pOut->LensDistoSpec.pRealExpect = pRealExpectBuf;
            break;
        case AMBA_CAL_LD_ANGLE_TBL:
            pAngleBuf->Length = pIn->TableLen;
            pAngleBuf->pRealTbl = pIn->pRealTable;
            pAngleBuf->pAngleTbl = pIn->pExceptAngleTable;
            pOut->LensDistoSpec.pAngle = pAngleBuf;
            break;
        case AMBA_CAL_LD_REAL_EXPECT_FORMULA:
            pFormulaBuf->X1 = pIn->RealExpectFormula[0];
            pFormulaBuf->X3 = pIn->RealExpectFormula[1];
            pFormulaBuf->X5 = pIn->RealExpectFormula[2];
            pFormulaBuf->X7 = pIn->RealExpectFormula[3];
            pFormulaBuf->X9 = pIn->RealExpectFormula[4];
            pOut->LensDistoSpec.pRealExpectFormula = pFormulaBuf;
            break;
        case AMBA_CAL_LD_ANGLE_FORMULA:
            pFormulaBuf->X1 = pIn->AngleFormula[0];
            pFormulaBuf->X3 = pIn->AngleFormula[1];
            pFormulaBuf->X5 = pIn->AngleFormula[2];
            pFormulaBuf->X7 = pIn->AngleFormula[3];
            pFormulaBuf->X9 = pIn->AngleFormula[4];
            pOut->LensDistoSpec.pAngleFormula = pFormulaBuf;
            break;
        case AMBA_CAL_LD_USER_PINHO_DEF:
        case AMBA_CAL_LD_USER_ANGLE_DEF:
        default:
            AmbaPrint_PrintUInt5("[ERROR] SvcAnimCalib_AvmFeedLensSpec: undefined lens distortion type(%u)!", (UINT32)pIn->LensDistoType, 0U, 0U, 0U, 0U);
            Rval = SVC_NG;
            break;
        }
    }
    return Rval;
}

static UINT32 SvcAnimCalib_AvmFeedLensSpecV2(const AMBA_CT_AVM_LENS_s *pIn,
    AMBA_CAL_LENS_SPEC_V2_s *pOut,
    AMBA_CAL_LENS_DST_REAL_EXPECT_s *pRealExpectBuf,
    AMBA_CAL_LENS_DST_ANGLE_s *pAngleBuf,
    AMBA_CAL_LENS_DST_FORMULA_s *pFormulaBuf)
{
    UINT32 Rval = SVC_OK;
    if (pIn == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: pIn is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else if (pOut == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: pOut is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else if (pRealExpectBuf == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: pRealExpectBuf is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else if (pAngleBuf == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: pAngleBuf is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else if (pFormulaBuf == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: pFormulaBuf is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        pOut->LensDistoType = pIn->LensDistoType;
        switch(pIn->LensDistoType) {
        case AMBA_CAL_LD_REAL_EXPECT_TBL:
            pRealExpectBuf->Length = pIn->TableLen;
            pRealExpectBuf->pRealTbl = pIn->pRealTable;
            pRealExpectBuf->pExpectTbl = pIn->pExceptTable;
            pOut->LensDistoSpec.pRealExpect = pRealExpectBuf;
            break;
        case AMBA_CAL_LD_ANGLE_TBL:
            pAngleBuf->Length = pIn->TableLen;
            pAngleBuf->pRealTbl = pIn->pRealTable;
            pAngleBuf->pAngleTbl = pIn->pExceptAngleTable;
            pOut->LensDistoSpec.pAngle = pAngleBuf;
            break;
        case AMBA_CAL_LD_REAL_EXPECT_FORMULA:
            pFormulaBuf->X1 = pIn->RealExpectFormula[0];
            pFormulaBuf->X3 = pIn->RealExpectFormula[1];
            pFormulaBuf->X5 = pIn->RealExpectFormula[2];
            pFormulaBuf->X7 = pIn->RealExpectFormula[3];
            pFormulaBuf->X9 = pIn->RealExpectFormula[4];
            pOut->LensDistoSpec.pRealExpectFormula = pFormulaBuf;
            break;
        case AMBA_CAL_LD_ANGLE_FORMULA:
            pFormulaBuf->X1 = pIn->AngleFormula[0];
            pFormulaBuf->X3 = pIn->AngleFormula[1];
            pFormulaBuf->X5 = pIn->AngleFormula[2];
            pFormulaBuf->X7 = pIn->AngleFormula[3];
            pFormulaBuf->X9 = pIn->AngleFormula[4];
            pOut->LensDistoSpec.pAngleFormula = pFormulaBuf;
            break;
        case AMBA_CAL_LD_USER_PINHO_DEF:
        case AMBA_CAL_LD_USER_ANGLE_DEF:
        default:
            AmbaPrint_PrintUInt5("[ERROR] SvcAnimCalib_AvmFeedLensSpecV2: undefined lens distortion type(%u)!", (UINT32)pIn->LensDistoType, 0U, 0U, 0U, 0U);
            Rval = SVC_NG;
            break;
        }
    }
    return Rval;
}


UINT32 SvcAnimCalib_AvmFeedPointMap(const AMBA_CT_AVM_CALIB_POINTS_s *pInCalibPoints,
    const AMBA_CT_AVM_ASSISTANCE_POINTS_s *pInAssistancePoints,
    AMBA_CAL_AVM_POINT_MAP_s *pOut)
{
    UINT32 Rval = SVC_OK;
    if (pInCalibPoints == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: pInCalibPoints is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else if (pInAssistancePoints == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: pInAssistancePoints is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else if (pOut == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: pOut is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        UINT32 i;
        for (i = 0; i < AVM_CALIB_POINT_NUM; i++) {
            pOut->CalibPoints[i].WorldPos.X = pInCalibPoints->WorldPositionX[i];
            pOut->CalibPoints[i].WorldPos.Y = pInCalibPoints->WorldPositionY[i];
            pOut->CalibPoints[i].WorldPos.Z = pInCalibPoints->WorldPositionZ[i];
            pOut->CalibPoints[i].RawPos.X = pInCalibPoints->RawPositionX[i];
            pOut->CalibPoints[i].RawPos.Y = pInCalibPoints->RawPositionY[i];
        }
        for (i = 0; i < pInAssistancePoints->Number; i++) {
            pOut->AssistancePoints[i].WorldPos.X = pInCalibPoints->WorldPositionX[i];
            pOut->AssistancePoints[i].WorldPos.Y = pInCalibPoints->WorldPositionY[i];
            pOut->AssistancePoints[i].WorldPos.Z = pInCalibPoints->WorldPositionZ[i];
            pOut->AssistancePoints[i].RawPos.X = pInCalibPoints->RawPositionX[i];
            pOut->AssistancePoints[i].RawPos.Y = pInCalibPoints->RawPositionY[i];
        }
        pOut->AssistancePointNumber = pInAssistancePoints->Number;
    }
    return Rval;
}

static UINT32 SvcAnimCalib_AvmString2CalibMode(const char *pCalibMode, AMBA_CT_AVM_CALIB_MODE_e *CalibMode)
{
    UINT32 Rval = SVC_OK;
    if (pCalibMode == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: pCalibMode is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        if (AmbaUtility_StringCompare(pCalibMode, "AVM_PRE_CALC", AmbaUtility_StringLength("AVM_PRE_CALC")) == 0) {
            *CalibMode = AMBA_CT_AVM_CALIB_MODE_PRE_CALC;
        } else if (AmbaUtility_StringCompare(pCalibMode, "AVM_FAST", AmbaUtility_StringLength("AVM_FAST")) == 0) {
            *CalibMode = AMBA_CT_AVM_CALIB_MODE_FAST;
        } else if (AmbaUtility_StringCompare(pCalibMode, "AVM_NORMAL", AmbaUtility_StringLength("AVM_NORMAL")) == 0) {
            *CalibMode = AMBA_CT_AVM_CALIB_MODE_NORMAL;
        } else if (AmbaUtility_StringCompare(pCalibMode, "AVM_PRE_CHECK", AmbaUtility_StringLength("AVM_PRE_CHECK")) == 0) {
            *CalibMode = AMBA_CT_AVM_CALIB_MODE_PRE_CHECK;
        } else {
            *CalibMode = AMBA_CT_AVM_CALIB_MODE_MAX;
        }
    }
    return Rval;
}

static UINT32 SvcAnimCalib_AvmCbMsgReciver(AMBA_CAL_AVM_MSG_TYPE_e Type, AMBA_CAL_AVM_CAM_ID_e CamId, const AMBA_CAL_AVM_MSG_s *pMsg)
{
    UINT32 Rval = 0U;
    switch (Type) {
    case AMBA_AVM_MSG_GRID_STATUS:
    {
        UINT32 *pStatusTbl = g_AvmGridStatus[CamId].GridStatus;
        const AMBA_CAL_AVM_WARP_TBL_STATUS_s *pStatus = pMsg->pGridStatus;
        if ((pStatus->HorGridNum * pStatus->VerGridNum) > MAX_WARP_TBL_LEN) {
            Rval = 1U;
        } else {
            g_AvmGridStatus[CamId].HorTileNum = pStatus->HorGridNum;
            g_AvmGridStatus[CamId].VerTileNum = pStatus->VerGridNum;
            pStatusTbl[pStatus->GridPosX + (pStatus->GridPosY * pStatus->HorGridNum)] = pStatus->Status;
        }
        break;
    }
    case AMBA_AVM_MSG_PTN_ERR:
    case AMBA_AVM_MSG_REPORT:
        break;
    default:
        Rval = 1U;
        break;
    }
    return Rval;
}

static inline UINT32 SvcAnimCalib_Avm3dFeedPlugIn(const char *CalibModeStr, INT32 UseCustomCalib, AMBA_CAL_AVM_3D_PLUG_IN_V2_s *pPlugIn)
{
    UINT32 Rval = SVC_OK;
    if (CalibModeStr == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: CalibModeStr is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else if (pPlugIn == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: pPlugIn is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        AMBA_CT_AVM_CALIB_MODE_e CalibMode;
        Rval = SvcAnimCalib_AvmString2CalibMode(CalibModeStr, &CalibMode);
        if (Rval == SVC_OK) {
            pPlugIn->MsgReciverCfg.GridStatus = 1U;
            pPlugIn->MsgReciverCfg.PatternError = 1U;
            pPlugIn->MsgReciverCfg.Report = 1U;
            pPlugIn->MsgReciver = SvcAnimCalib_AvmCbMsgReciver;

            pPlugIn->PlugInMode = 0U;
            pPlugIn->PlugInMode |= (UINT32)AVM_3D_PLUGIN_MODE_MSG_RECEIVER;

            switch (CalibMode) {
                case AMBA_CT_AVM_CALIB_MODE_FAST:
                    pPlugIn->PlugInMode |= (UINT32)AVM_3D_PLUGIN_MODE_FEED_INT_DATA;
                    pPlugIn->pFeedPreCalData = NULL;
                    break;
                case AMBA_CT_AVM_CALIB_MODE_PRE_CALC:
                    pPlugIn->PlugInMode |= (UINT32)AVM_3D_PLUGIN_MODE_SAVE_INT_DATA;
                    pPlugIn->pSavePreCalData = NULL;
                    break;
                case AMBA_CT_AVM_CALIB_MODE_NORMAL:
                default:
                    pPlugIn->pSavePreCalData = NULL;
                    break;
            }

            if (UseCustomCalib != 0) {
                pPlugIn->PlugInMode |= (UINT32)AVM_3D_PLUGIN_MODE_CUSTOM_CALIB;
                pPlugIn->pInsertCustomCalibMat = NULL;
            }
        } else {
            SvcAnimCalib_Perror(__func__, __LINE__, "SvcAnimCalib_AvmString2CalibMode", Rval);
        }
    }
    return Rval;
}

static UINT32 SvcAnimCalib_Cal3dView(AMBA_CAL_AVM_3D_DATA_s *CalOutput)
{
    UINT32 Rval = SVC_OK;
    if (CalOutput == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: CalOutput is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        static void *CalWorkBuffer = NULL;
        static void *CalInfoWorkBuffer = NULL;
        if (CalWorkBuffer == NULL) {
            ULONG BufBase = 0U;
            UINT32 BufSize = 0U;

            Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CAL_SUR_3D_VIEW, &BufBase, &BufSize);
            if (Rval == SVC_OK) {
                AmbaMisra_TypeCast(&(CalWorkBuffer), &(BufBase));
            } else {
                SvcAnimCalib_Perror(__func__, __LINE__, "SvcBuffer_Request fail", Rval);
            }
        }
        if (CalInfoWorkBuffer == NULL) {
            ULONG BufBase = 0U;
            UINT32 BufSize = 0U;
            Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CAL_INFO_SUR, &BufBase, &BufSize);
            if (Rval == SVC_OK) {
                AmbaMisra_TypeCast(&(CalInfoWorkBuffer), &(BufBase));
            } else {
                SvcAnimCalib_Perror(__func__, __LINE__, "SvcBuffer_Request fail", Rval);
            }
        }
        if (Rval == SVC_OK) {
            UINT8 i;
            AMBA_CAL_AVM_3D_CFG_V2_s Avm3DWarpConfig;
            AMBA_CT_AVM_SYSTEM_s SystemData;
            AMBA_CAL_AVM_CALIB_DATA_V2_s CalibrationData[AMBA_CAL_AVM_CAM_MAX];
            const AMBA_CT_AVM_USER_SETTING_s *CalibUserSetting;

            AmbaSvcWrap_MisraMemset(&Avm3DWarpConfig, 0x0, sizeof(AMBA_CAL_AVM_3D_CFG_V2_s));
            AmbaSvcWrap_MisraMemset(&SystemData, 0x0, sizeof(AMBA_CT_AVM_SYSTEM_s));
            AmbaSvcWrap_MisraMemset(&CalibrationData[0], 0x0, (sizeof(AMBA_CAL_AVM_CALIB_DATA_V2_s)*4U));

            CalibUserSetting = AmbaCT_AvmGetUserSetting();

            /* Fill avm 3d config for calibration function */
            AmbaSvcWrap_MisraMemcpy(&Avm3DWarpConfig.Car, &CalibUserSetting->Car, sizeof(AMBA_CAL_SIZE_s));
            Rval = SvcAnimCalib_AvmString2View3DModelType(&CalibUserSetting->View3DModelType.ModelType[0], &Avm3DWarpConfig.View.ModelCfg.Type);
            if (Rval == SVC_OK) {
                static const AMBA_CAL_ROTATION_e CAL_ROTATION_ENUM_LUT[4] =
                    {   AMBA_CAL_ROTATE_0,
                        AMBA_CAL_ROTATE_90,
                        AMBA_CAL_ROTATE_180,
                        AMBA_CAL_ROTATE_270,
                    };
                if (Avm3DWarpConfig.View.ModelCfg.Type == AMBA_CAL_AVM_3D_VIEW_MODEL_SIM) {
                    Avm3DWarpConfig.View.ModelCfg.SimpleModelCfg.FloorViewRangeX = CalibUserSetting->View3D.FloorRangeX;
                    Avm3DWarpConfig.View.ModelCfg.SimpleModelCfg.FloorViewRangeY = CalibUserSetting->View3D.FloorRangeY;
                    Avm3DWarpConfig.View.ModelCfg.SimpleModelCfg.ViewCenter.X = CalibUserSetting->View3D.CenterPositionX;
                    Avm3DWarpConfig.View.ModelCfg.SimpleModelCfg.ViewCenter.Y = CalibUserSetting->View3D.CenterPositionY;
                    Avm3DWarpConfig.View.ModelCfg.SimpleModelCfg.ViewDistance = CalibUserSetting->View3D.Distance;
                    Avm3DWarpConfig.View.ModelCfg.SimpleModelCfg.ViewHeight = CalibUserSetting->View3D.Height;

                    Avm3DWarpConfig.View.DisplayWidth = CalibUserSetting->View3D.DisplayWidth;
                    Avm3DWarpConfig.View.DisplayHeight = CalibUserSetting->View3D.DisplayHeight;
                    Avm3DWarpConfig.View.AutoVoutOrderEnable = CalibUserSetting->View3D.AutoVoutOrderEnable;
                } else if (Avm3DWarpConfig.View.ModelCfg.Type == AMBA_CAL_AVM_3D_VIEW_MODEL_ADV) {
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.TopFrontRange = CalibUserSetting->AdvView3D.TopFrontRange;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.TopBackRange = CalibUserSetting->AdvView3D.TopBackRange;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.TopLeftRange = CalibUserSetting->AdvView3D.TopLeftRange;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.TopRightRange = CalibUserSetting->AdvView3D.TopRightRange;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.TopFrontLeftRadiusX = CalibUserSetting->AdvView3D.TopFrontLeftRadiusX;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.TopFrontLeftRadiusY = CalibUserSetting->AdvView3D.TopFrontLeftRadiusY;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.TopFrontRightRadiusX = CalibUserSetting->AdvView3D.TopFrontRightRadiusX;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.TopFrontRightRadiusY = CalibUserSetting->AdvView3D.TopFrontRightRadiusY;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.TopBackLeftRadiusX = CalibUserSetting->AdvView3D.TopBackLeftRadiusX;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.TopBackLeftRadiusY = CalibUserSetting->AdvView3D.TopBackLeftRadiusY;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.TopBackRightRadiusX = CalibUserSetting->AdvView3D.TopBackRightRadiusX;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.TopBackRightRadiusY = CalibUserSetting->AdvView3D.TopBackRightRadiusY;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.TopHeight = CalibUserSetting->AdvView3D.TopHeight;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.BottomFrontRange = CalibUserSetting->AdvView3D.BottomFrontRange;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.BottomBackRange = CalibUserSetting->AdvView3D.BottomBackRange;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.BottomLeftRange = CalibUserSetting->AdvView3D.BottomLeftRange;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.BottomRightRange = CalibUserSetting->AdvView3D.BottomRightRange;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.BottomFrontLeftRadiusX = CalibUserSetting->AdvView3D.BottomFrontLeftRadiusX;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.BottomFrontLeftRadiusY = CalibUserSetting->AdvView3D.BottomFrontLeftRadiusY;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.BottomFrontRightRadiusX = CalibUserSetting->AdvView3D.BottomFrontRightRadiusX;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.BottomFrontRightRadiusY = CalibUserSetting->AdvView3D.BottomFrontRightRadiusY;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.BottomBackLeftRadiusX = CalibUserSetting->AdvView3D.BottomBackLeftRadiusX;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.BottomBackLeftRadiusY = CalibUserSetting->AdvView3D.BottomBackLeftRadiusY;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.BottomBackRightRadiusX = CalibUserSetting->AdvView3D.BottomBackRightRadiusX;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.BottomBackRightRadiusY = CalibUserSetting->AdvView3D.BottomBackRightRadiusY;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.TopToBottomRadiusH = CalibUserSetting->AdvView3D.TopToBottomRadiusH;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.TopToBottomRadiusV = CalibUserSetting->AdvView3D.TopToBottomRadiusV;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.BottomCornerIntervalNum = CalibUserSetting->AdvView3D.BottomCornerIntervalNum;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.PillarIntervalNum = CalibUserSetting->AdvView3D.PillarIntervalNum;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.ViewCenter.X = CalibUserSetting->AdvView3D.CenterPositionX;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.ViewCenter.Y = CalibUserSetting->AdvView3D.CenterPositionY;
                    Avm3DWarpConfig.View.ModelCfg.AdvanceModelCfg.RotationAngle = CalibUserSetting->AdvView3D.RotationAngle;

                    Avm3DWarpConfig.View.DisplayWidth = CalibUserSetting->AdvView3D.DisplayWidth;
                    Avm3DWarpConfig.View.DisplayHeight = CalibUserSetting->AdvView3D.DisplayHeight;
                    Avm3DWarpConfig.View.DisplayRotation = CAL_ROTATION_ENUM_LUT[CalibUserSetting->AdvView3D.DisplayRotation];
                    Avm3DWarpConfig.View.AutoVoutOrderEnable = CalibUserSetting->AdvView3D.AutoVoutOrderEnable;
                } else {
                    /**do nothing for misraC*/
                }

                AmbaSvcWrap_MisraMemcpy(&Avm3DWarpConfig.View.VirtualCam, &CalibUserSetting->VirtualCam3D, sizeof(AMBA_CAL_VIRTUAL_CAM_s));
                AmbaSvcWrap_MisraMemcpy(&Avm3DWarpConfig.View.Blend, &CalibUserSetting->Blend, sizeof(AMBA_CAL_AVM_BLEND_CFG_V2_s));

                AmbaCT_AvmGetSystem(&SystemData);
                for (i = 0U; i < (UINT8)AMBA_CAL_AVM_CAM_MAX; i++) {
                    if (Rval == SVC_OK) {
                        AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect[AMBA_CAL_AVM_CAM_MAX];
                        AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle[AMBA_CAL_AVM_CAM_MAX];
                        AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula[AMBA_CAL_AVM_CAM_MAX];

                        Rval = SvcAnimCalib_AvmFeedLensSpecV2(&CalibUserSetting->Lens[i], &Avm3DWarpConfig.Cam[i].Cam.Lens, &LensSpecRealExpect[i], &LensSpecAngle[i], &LensSpecFormula[i]);
                        if (Rval == SVC_OK) {
                            AMBA_CT_AVM_2D_CUSTOM_CALIB_s CustomCalib;

                            AmbaSvcWrap_MisraMemset(&CustomCalib, 0x0, sizeof(AMBA_CT_AVM_2D_CUSTOM_CALIB_s));

                            AmbaSvcWrap_MisraMemcpy(&Avm3DWarpConfig.Cam[i].Cam.Sensor, &CalibUserSetting->Sensor[i], sizeof(AMBA_CAL_SENSOR_s));
                            AmbaSvcWrap_MisraMemcpy(&Avm3DWarpConfig.Cam[i].Cam.OpticalCenter, &CalibUserSetting->OpticalCenter[i], sizeof(AMBA_CAL_POINT_DB_2D_s));
                            AmbaSvcWrap_MisraMemcpy(&Avm3DWarpConfig.Cam[i].VinSensorGeo, &CalibUserSetting->Vin[i], sizeof(AMBA_IK_VIN_SENSOR_GEOMETRY_s));

                            Avm3DWarpConfig.Cam[i].Cam.Pos.X = CalibUserSetting->Camera[i].PositionX;
                            Avm3DWarpConfig.Cam[i].Cam.Pos.Y = CalibUserSetting->Camera[i].PositionY;
                            Avm3DWarpConfig.Cam[i].Cam.Pos.Z = CalibUserSetting->Camera[i].PositionZ;
                            Avm3DWarpConfig.Cam[i].Cam.Rotation = CalibUserSetting->Camera[i].RotateType;

                            Rval = SvcAnimCalib_AvmFeedPointMap(&CalibUserSetting->CalibPointMap[i], &CalibUserSetting->AssistancePointMap[i], &Avm3DWarpConfig.Cam[i].PointMap);
                            if (Rval == SVC_OK) {
                                AmbaSvcWrap_MisraMemcpy(&Avm3DWarpConfig.Cam[i].ROI, &CalibUserSetting->Roi[i], sizeof(AMBA_CAL_ROI_s));
                                AmbaSvcWrap_MisraMemcpy(&Avm3DWarpConfig.Cam[i].Tile, &CalibUserSetting->TileSize[i], sizeof(AMBA_CAL_SIZE_s));
                                AmbaSvcWrap_MisraMemcpy(&Avm3DWarpConfig.Cam[i].Main, &CalibUserSetting->MainSize[i], sizeof(AMBA_CAL_SIZE_s));
                                AmbaSvcWrap_MisraMemcpy(&Avm3DWarpConfig.Cam[i].MinVout, &CalibUserSetting->MinVout[i], sizeof(AMBA_CAL_SIZE_s));
                                AmbaSvcWrap_MisraMemcpy(&Avm3DWarpConfig.Cam[i].InternalCfg, &CalibUserSetting->Internal[i], sizeof(AMBA_CAL_AVM_INTERNAL_CFG_s));

                                Avm3DWarpConfig.Cam[i].AutoROI = CalibUserSetting->Auto[i].RoiEnable;
                                Avm3DWarpConfig.Cam[i].AutoFrontEndRotation = CalibUserSetting->Auto[i].FrontEndRotationEnable;
                                AmbaSvcWrap_MisraMemcpy(&Avm3DWarpConfig.Cam[i].OptimizeLevel, &CalibUserSetting->OptimizeLevel[i], sizeof(Avm3DWarpConfig.Cam[i].OptimizeLevel));
                                Avm3DWarpConfig.Cam[i].WarpCalType = CalibUserSetting->WarpCalType[i];

                                Rval = SvcAnimCalib_Avm3dFeedPlugIn(SystemData.CalibMode, (INT32)CalibUserSetting->CustomCalib2D[i].Enable, &Avm3DWarpConfig.Cam[i].PlugIn);
                                if (Rval == SVC_OK) {
                                    // Calibration process
                                    AMBA_CAL_AVM_CALIB_CFG_V2_s AvmCalibConfig;
                                    AMBA_CAL_AVM_CALIB_PLUG_IN_s CalibPlugIn;

                                    AmbaSvcWrap_MisraMemset(&AvmCalibConfig, 0x0, sizeof(AMBA_CAL_AVM_CALIB_CFG_V2_s));
                                    AmbaSvcWrap_MisraMemset(&CalibPlugIn, 0x0, sizeof(AMBA_CAL_AVM_CALIB_PLUG_IN_s)); /**Set as 0 because no customer calib*/

                                    Rval = AmbaCal_DistortionTblMmToPixelV2(Avm3DWarpConfig.Cam[i].Cam.Sensor.CellSize, &Avm3DWarpConfig.Cam[i].Cam.Lens);
                                    if (Rval == SVC_OK) {
                                        AmbaSvcWrap_MisraMemcpy(&AvmCalibConfig.LenSpec, &Avm3DWarpConfig.Cam[i].Cam.Lens, sizeof(AMBA_CAL_LENS_SPEC_V2_s));
                                        AmbaSvcWrap_MisraMemcpy(&AvmCalibConfig.LenPos, &Avm3DWarpConfig.Cam[i].Cam.Pos, sizeof(AMBA_CAL_POINT_DB_3D_s));
                                        AmbaSvcWrap_MisraMemcpy(&AvmCalibConfig.OpticalCenter, &Avm3DWarpConfig.Cam[i].Cam.OpticalCenter, sizeof(AMBA_CAL_POINT_DB_2D_s));
                                        AvmCalibConfig.pPointMap = &Avm3DWarpConfig.Cam[i].PointMap;
                                        AvmCalibConfig.CellSize = CalibUserSetting->Sensor[i].CellSize;
                                        AvmCalibConfig.FocalLength = AMBA_CAL_FOCAL_LENGTH_UNKNOWN;

                                        Rval = AmbaCal_GenV2CalibrationInfo(i, CalInfoWorkBuffer, &AvmCalibConfig, &CalibrationData[i]);
                                        if (Rval != SVC_OK) {
                                            SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCal_GenCalibrationInfo", Rval);
                                        }
                                    } else {
                                        SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCal_DistortionTblMmToPixel", Rval);
                                    }
                                } else {
                                    SvcAnimCalib_Perror(__func__, __LINE__, "SvcAnimCalib_Avm3dFeedPlugIn", Rval);
                                }
                            } else {
                                SvcAnimCalib_Perror(__func__, __LINE__, "SvcAnimCalib_AvmFeedPointMap", Rval);
                            }
                        } else {
                            SvcAnimCalib_Perror(__func__, __LINE__, "SvcAnimCalib_AvmFeedLensSpecV2", Rval);
                        }
                    }
                }
                if (Rval == SVC_OK) {
                    Rval = AmbaCal_AvmGen3DViewTblV2(CalibrationData, &Avm3DWarpConfig, CalWorkBuffer, CalOutput);
                    if (Rval == SVC_OK) {
                        (void)SvcAnimCalib_SaveVoutOsdInfo(CalOutput);
                    } else {
                        SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCal_AvmGen3DViewTbl", Rval);
                    }
                }
            } else {
                SvcAnimCalib_Perror(__func__, __LINE__, "SvcAnimCalib_AvmString2View3DModelType", Rval);
            }
        }
    }
    return Rval;
}

static void SvcAnimCalib_ShowCalibOutput3D(const AMBA_CAL_AVM_3D_DATA_s *CalOutput)
{
    UINT8 i, j;
    AmbaPrint_PrintUInt5("Version = %u", CalOutput->Version, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintInt5("VoutCarPosition[0].X = %d", CalOutput->VoutCarOSDCorner[0].X, 0, 0, 0, 0);
    AmbaPrint_PrintInt5("VoutCarPosition[0].Y = %d", CalOutput->VoutCarOSDCorner[0].Y, 0, 0, 0, 0);
    AmbaPrint_PrintInt5("VoutCarPosition[1].X = %d", CalOutput->VoutCarOSDCorner[1].X, 0, 0, 0, 0);
    AmbaPrint_PrintInt5("VoutCarPosition[1].Y = %d", CalOutput->VoutCarOSDCorner[1].Y, 0, 0, 0, 0);
    AmbaPrint_PrintInt5("VoutCarPosition[2].X = %d", CalOutput->VoutCarOSDCorner[2].X, 0, 0, 0, 0);
    AmbaPrint_PrintInt5("VoutCarPosition[2].Y = %d", CalOutput->VoutCarOSDCorner[2].Y, 0, 0, 0, 0);
    AmbaPrint_PrintInt5("VoutCarPosition[3].X = %d", CalOutput->VoutCarOSDCorner[3].X, 0, 0, 0, 0);
    AmbaPrint_PrintInt5("VoutCarPosition[3].Y = %d", CalOutput->VoutCarOSDCorner[3].Y, 0, 0, 0, 0);
    for (i = 0U; i < (UINT8)AMBA_CAL_AVM_CAM_MAX; i++) {
        AmbaPrint_PrintUInt5("Cam[%u].WarpTbl.Version = %u", i, CalOutput->Cam[i].WarpTbl.Version, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].WarpTbl.CalibSensorGeo.StartX = %u", i, CalOutput->Cam[i].WarpTbl.CalibSensorGeo.StartX, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].WarpTbl.CalibSensorGeo.StartY = %u", i, CalOutput->Cam[i].WarpTbl.CalibSensorGeo.StartY, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].WarpTbl.CalibSensorGeo.Width = %u", i, CalOutput->Cam[i].WarpTbl.CalibSensorGeo.Width, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].WarpTbl.CalibSensorGeo.Height = %u", i, CalOutput->Cam[i].WarpTbl.CalibSensorGeo.Height, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].WarpTbl.CalibSensorGeo.HSubSample.FactorNum = %u", i, CalOutput->Cam[i].WarpTbl.CalibSensorGeo.HSubSample.FactorNum, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].WarpTbl.CalibSensorGeo.HSubSample.FactorDen = %u", i, CalOutput->Cam[i].WarpTbl.CalibSensorGeo.HSubSample.FactorDen, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].WarpTbl.CalibSensorGeo.VSubSample.FactorNum = %u", i, CalOutput->Cam[i].WarpTbl.CalibSensorGeo.VSubSample.FactorNum, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].WarpTbl.CalibSensorGeo.VSubSample.FactorDen = %u", i, CalOutput->Cam[i].WarpTbl.CalibSensorGeo.VSubSample.FactorDen, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].WarpTbl.HorGridNum = %u", i, CalOutput->Cam[i].WarpTbl.HorGridNum, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].WarpTbl.VerGridNum = %u", i, CalOutput->Cam[i].WarpTbl.VerGridNum, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].WarpTbl.TileWidthExp = %u", i, CalOutput->Cam[i].WarpTbl.TileWidthExp, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].WarpTbl.TileHeightExp = %u", i, CalOutput->Cam[i].WarpTbl.TileHeightExp, 0U, 0U, 0U);
        /* Check first ten warp data */
        for (j = 0U; j < 10U; j++) {
            AmbaPrint_PrintInt5("Cam[%u].WarpTbl.WarpVector[%u].X = %d", (INT32)i, (INT32)j, CalOutput->Cam[i].WarpTbl.WarpVector[j].X, 0, 0);
        }
        for (j = 0U; j < 10U; j++) {
            AmbaPrint_PrintInt5("Cam[%u].WarpTbl.WarpVector[%u].Y = %d", (INT32)i, (INT32)j, CalOutput->Cam[i].WarpTbl.WarpVector[j].Y, 0, 0);
        }

        AmbaPrint_PrintUInt5("Cam[%u].BlendTbl.Width = %u", i, CalOutput->Cam[i].BlendTbl.Width, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].BlendTbl.Height = %u", i, CalOutput->Cam[i].BlendTbl.Height, 0U, 0U, 0U);
        /* Check first ten blend data */
        for (j = 0U; j < 10U; j++) {
            AmbaPrint_PrintUInt5("Cam[%u].BlendTbl.Table[%u] = %u", i, j, CalOutput->Cam[i].BlendTbl.Table[j], 0U, 0U);
        }

        AmbaPrint_PrintUInt5("Cam[%u].WarpTblStatus = %u", i, (UINT32)CalOutput->Cam[i].WarpTblStatus, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].BlendTblStatus = %u", i, (UINT32)CalOutput->Cam[i].BlendTblStatus, 0U, 0U, 0U);

        AmbaPrint_PrintUInt5("Cam[%u].VoutOrder = %u", i, CalOutput->Cam[i].VoutOrder, 0U, 0U, 0U);

        AmbaPrint_PrintUInt5("Cam[%u].VoutArea.StartX = %u", i, CalOutput->Cam[i].VoutArea.StartX, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].VoutArea.StartY = %u", i, CalOutput->Cam[i].VoutArea.StartY, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].VoutArea.Width = %u", i, CalOutput->Cam[i].VoutArea.Width, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].VoutArea.Height = %u", i, CalOutput->Cam[i].VoutArea.Height, 0U, 0U, 0U);

        AmbaPrint_PrintUInt5("Cam[%u].FrontendRotation = %u", i, (UINT32)CalOutput->Cam[i].FrontendRotation, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Cam[%u].VoutRotation = %u", i, (UINT32)CalOutput->Cam[i].VoutRotation, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("---------------------------------------", 0U, 0U, 0U, 0U, 0U);
    }
}

static UINT32 SvcAnimCalib_GetOsdInfo(SVC_CALIB_OSD_INFO_s *OsdInfo)
{
    UINT32 Rval = SVC_OK;
    if (OsdInfo == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: OsdInfo is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        UINT32 NumSuccess, Ret;
        UINT64 FileLen;
        AMBA_FS_FILE *InputFile;
        char ReadMode[3] = "r";
        char InputFilename[SVC_ANIM_MAX_NAME_LENGTH] = "C:\\ANIM\\OSD\\OsdCarBmp000.bin";
        ULONG BufBase = 0U, BufBaseDst = 0U;
        UINT32 BufSize = 0U, BufSizeDst = 0U;

        Rval = AmbaFS_FileOpen(InputFilename, ReadMode, &InputFile);
        if (Rval == SVC_OK) {
            /* Get file length */
            Rval = AmbaFS_FileSeek(InputFile, 0, AMBA_FS_SEEK_END);
            if (Rval == SVC_OK) {
                Rval = AmbaFS_FileTell(InputFile, &FileLen);
                if (Rval == SVC_OK) {
                    if (FileLen != 0U) {
                        void *SrcOsdBuf;
                        OsdInfo->DecmprDataSize = (UINT32)FileLen;
                        Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CAL_SUR_CAR, &BufBase, &BufSize);
                        if (Rval == SVC_OK) {
                            AmbaMisra_TypeCast(&SrcOsdBuf, &BufBase);
                            /* Set the position indicator as the beginning of the file */
                            Rval = AmbaFS_FileSeek(InputFile, 0, AMBA_FS_SEEK_START);
                            if (Rval == SVC_OK) {
                                /* Read input file */
                                Rval = AmbaFS_FileRead(SrcOsdBuf, (UINT32)FileLen, (UINT32)1U, InputFile, &NumSuccess);
                                if (Rval == SVC_OK) {
                                    if (NumSuccess == 0U) {
                                        AmbaPrint_PrintUInt5("[ERROR] SvcAnimCalib_GetOsdInfo: fread failed", 0U, 0U, 0U, 0U, 0U);
                                        Rval = SVC_NG;
                                    }
                                    Ret = AmbaFS_FileClose(InputFile);
                                    if (Ret != SVC_OK) {
                                        if ((Rval == SVC_OK) /*|| (Rval == REFCODE_IO_ERR)*/) {
                                            Rval = Ret;
                                        }
                                        SvcAnimCalib_Perror(__func__, __LINE__, "AmbaFS_FileClose", Rval);
                                    }
                                    if (Rval == SVC_OK) {
                                        /* Compress osd data by LZ4 */
                                        Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CAL_SUR_CAR_CMPR, &BufBaseDst, &BufSizeDst);
                                        if (Rval == SVC_OK) {
                                            UINT8 *DstBuf;
                                            const UINT8 *SrcAddr;
                                            UINT32 OutputSize;
                                            AMBA_CALIB_DATA_CMPR_INFO_s CmprInfo;
                                            AmbaMisra_TypeCast(&DstBuf, &BufBaseDst);
                                            AmbaMisra_TypeCast(&SrcAddr, &SrcOsdBuf);
                                            /* TODO */
                                            CmprInfo.Lz4Info.SegmentSize = (UINT16)(16U * 1024U);
                                            Rval = AmbaCalibData_Compress(AMBA_CALIB_DATA_CMPR_ALGO_LZ4, &CmprInfo, SrcAddr, (UINT32)FileLen, DstBuf, BufSizeDst, &OutputSize);
                                            if (Rval == SVC_OK) {
                                                OsdInfo->CmprData = DstBuf;
                                                OsdInfo->CmprSize = OutputSize;
                                                OsdInfo->Window.OffsetX = 0U;
                                                OsdInfo->Window.OffsetY = 0U;
                                                OsdInfo->Window.Width = SVC_OSD_WIDTH;
                                                OsdInfo->Window.Height = SVC_OSD_HEIGH;
                                                AmbaPrint_PrintUInt5("SvcAnimCalib_GetOsdInfo: AmbaCalibData_Compress src %d dst %d", (UINT32)FileLen, OutputSize, 0U, 0U, 0U);
                                            } else {
                                                SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCalibData_Compress", Rval);
                                            }
                                        } else {
                                            SvcAnimCalib_Perror(__func__, __LINE__, "SvcBuffer_Request", Rval);
                                        }
                                    }
                                } else {
                                    SvcAnimCalib_Perror(__func__, __LINE__, "AmbaFS_FileRead", Rval);
                                }
                            } else {
                                SvcAnimCalib_Perror(__func__, __LINE__, "AmbaFS_FileSeek", Rval);
                            }
                        } else {
                            SvcAnimCalib_Perror(__func__, __LINE__, "SvcBuffer_Request", Rval);
                        }
                    } else {
                        AmbaPrint_PrintUInt5("[ERROR] SvcAnimCalib_GetOsdInfo: File length is zero!", 0U, 0U, 0U, 0U, 0U);
                        if (AmbaFS_FileClose(InputFile) != SVC_OK) {
                            SvcAnimCalib_Perror(__func__, __LINE__, "AmbaFS_FileClose", Rval);
                        }
                        Rval = SVC_NG;
                    }
                } else {
                    SvcAnimCalib_Perror(__func__, __LINE__, "AmbaFS_FileTell", Rval);
                    if (AmbaFS_FileClose(InputFile) != SVC_OK) {
                        SvcAnimCalib_Perror(__func__, __LINE__, "AmbaFS_FileClose", Rval);
                    }
                }
            } else {
                SvcAnimCalib_Perror(__func__, __LINE__, "AmbaFS_FileSeek", Rval);
            }
        } else {
            SvcAnimCalib_Perror(__func__, __LINE__, "AmbaFS_FileOpen", Rval);
        }
    }
    return Rval;
}

#if 0 //check later
static UINT32 SvcAnimCalib_CalibToNand(AMBA_CAL_AVM_3D_DATA_s *Calib3dData, AMBA_CAL_AVM_MV_DATA_s *CalibMainviewData, SVC_CALIB_OSD_INFO_s *Osd3DInfo)
{
    UINT32 Rval = SVC_OK;
    if (Calib3dData == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: Calib3dData is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else if (CalibMainviewData == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: CalibMainviewData is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        UINT8 i, j, k, ViewWp[SVC_CALIB_DISP_COUNT] = {0U};
        SVC_ANIM_INDEX_s *AnimIndex;
        UINT32 Data3DBufferSize = 0U;
        UINT32 RawDataBufferSize = 2 * sizeof(SVC_ANIM_INDEX_s); // there're 2 disp window (0: 3D, 1: main view) and 1 view for each disp window
        void *RawBuffer, *RawDataBuffer, *CalibDataBuffer;
        AMBA_CALIB_DATA_GROUP_DESC_s GroupDesc;
        AMBA_CALIB_DATA_GROUP_DESC_s *pGroupDesc = &GroupDesc;
        SVC_ANIM_HEADER_s AnimHeader;
        SVC_ANIM_HEADER_s *pAnimHeader = &AnimHeader;
        void *Ptr;
        AmbaMisra_TypeCast(&Ptr, &pAnimHeader);
        AmbaSvcWrap_MisraMemset(Ptr, 0, sizeof(SVC_ANIM_HEADER_s));
        Rval = RefMemPool_Allocate(REF_MEM_POOL_REGION_CACHE, AMBA_CACHE_LINE_SIZE, &RawDataBuffer, &RawBuffer, RawDataBufferSize);
        if (Rval == SVC_OK) {
            UINT32 CalibDataBufSize = 0U;
            void *BlendDataBuffer;
            AmbaMisra_TypeCast(&Ptr, &pGroupDesc);
            AmbaSvcWrap_MisraMemset(Ptr, 0, sizeof(AMBA_CALIB_DATA_GROUP_DESC_s));
            GroupDesc.StorageId = AMBA_CALIB_DATA_STORAGE_NAND;
            GroupDesc.PartId = AMBA_USER_PARTITION_RESERVED0;
            GroupDesc.Count = REF_CALIB_3D_ITEM_COUNT;
            /* Raw data (user-defined) */
            /* Fill for (disp 0, view 0) */
            AmbaMisra_TypeCast(&AnimIndex, &RawDataBuffer);
            AnimIndex->Version = SVC_CALIB_VERSION;
            AnimIndex->VoutCount = 1U;
            AnimIndex->ChanCount = AMBA_CAL_AVM_CAM_MAX; // Front, back, left, right
            AnimIndex->DataInfo.OsdCount = 1U;
            AnimIndex->DataInfo.GroupCount = 1U;
            for (i = 0U; i < AnimIndex->ChanCount; i++) {
                AnimIndex->WarpEnable[i] = 1U;
                AnimIndex->BlendType[i] = 1U;
            }
            /* 3D data */
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].Compressed = AMBA_CALIB_DATA_CMPR_ALGO_NONE;
            for (i = 0U; i < AMBA_CAL_AVM_CAM_MAX; i++) {
                /* Warp */
                UINT8 WarpElementWp = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].Count;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Type = AMBA_CALIB_DATA_TYPE_WARP;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Version = IK_WARP_VER;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.HorGridNum = Calib3dData->Cam[i].WarpTbl.HorGridNum;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.VerGridNum = Calib3dData->Cam[i].WarpTbl.VerGridNum;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.TileWidth = Calib3dData->Cam[i].WarpTbl.TileWidthExp;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.TileHeight = Calib3dData->Cam[i].WarpTbl.TileHeightExp;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.CalibSensorGeo.StartX = Calib3dData->Cam[i].WarpTbl.CalibSensorGeo.StartX;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.CalibSensorGeo.StartY = Calib3dData->Cam[i].WarpTbl.CalibSensorGeo.StartY;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.CalibSensorGeo.Width = Calib3dData->Cam[i].WarpTbl.CalibSensorGeo.Width;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.CalibSensorGeo.Height = Calib3dData->Cam[i].WarpTbl.CalibSensorGeo.Height;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.CalibSensorGeo.HSubSampleFactorDen = Calib3dData->Cam[i].WarpTbl.CalibSensorGeo.HSubSample.FactorDen;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.CalibSensorGeo.HSubSampleFactorNum = Calib3dData->Cam[i].WarpTbl.CalibSensorGeo.HSubSample.FactorNum;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.CalibSensorGeo.VSubSampleFactorDen = Calib3dData->Cam[i].WarpTbl.CalibSensorGeo.VSubSample.FactorDen;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.CalibSensorGeo.VSubSampleFactorNum = Calib3dData->Cam[i].WarpTbl.CalibSensorGeo.VSubSample.FactorNum;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Size = Calib3dData->Cam[i].WarpTbl.HorGridNum * Calib3dData->Cam[i].WarpTbl.VerGridNum * sizeof(AMBA_CAL_GRID_POINT_s);
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].Size += GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Size;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].Count++;
                CalibDataBufSize += GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Size;
                {
                    /* Blend */
                    UINT8 BlendElementWp = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Count;
                    GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[BlendElementWp].Type = AMBA_CALIB_DATA_TYPE_BLEND;
                    GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[BlendElementWp].Blend.Version = SVC_CALIB_VERSION;
                    GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[BlendElementWp].Blend.Width = Calib3dData->Cam[i].BlendTbl.Width;
                    GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[BlendElementWp].Blend.Height = Calib3dData->Cam[i].BlendTbl.Height;
                    GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[BlendElementWp].Size = Calib3dData->Cam[i].BlendTbl.Width * Calib3dData->Cam[i].BlendTbl.Height;
                    GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Count++;
                    CalibDataBufSize += GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[BlendElementWp].Size;
                }
            }
            /* Osd */
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_OSD].Count = 1U;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_OSD].ElementInfo[0].Type = AMBA_CALIB_DATA_TYPE_OSD;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_OSD].ElementInfo[0].Size = Osd3DInfo->DecmprDataSize;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_OSD].ElementInfo[0].Osd.Version = SVC_CALIB_VERSION;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_OSD].ElementInfo[0].Osd.OsdWindow = Osd3DInfo->Window;
            CalibDataBufSize += Osd3DInfo->CmprSize;
            Rval = RefMemPool_Allocate(REF_MEM_POOL_REGION_CACHE, AMBA_CACHE_LINE_SIZE, &CalibDataBuffer, &RawBuffer, CalibDataBufSize);
            if (Rval == SVC_OK) {
                /* Fill calib data buf */
                void *CalibDataPtr = CalibDataBuffer;
                UINT32 TmpAddr, TotalBlendSize = 0U;
                /* Warp data */
                for (i = 0U; i < GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].Count; i++) {
                    AmbaSvcWrap_MisraMemcpy(CalibDataPtr, Calib3dData->Cam[i].WarpTbl.WarpVector, GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[i].Size);
                    AmbaMisra_TypeCast(&TmpAddr, &CalibDataPtr);
                    TmpAddr += GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[i].Size;
                    AmbaMisra_TypeCast(&CalibDataPtr, &TmpAddr);
                }
                for (i = 0U; i < GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Count; i++) {
                    TotalBlendSize += GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[i].Size;
                }
                Rval = RefMemPool_Allocate(REF_MEM_POOL_REGION_CACHE, AMBA_CACHE_LINE_SIZE, &BlendDataBuffer, &RawBuffer, TotalBlendSize);
                if (Rval == SVC_OK) {
                    /* Blend data */
                    void *BlendDataPtr = BlendDataBuffer;
                    for (i = 0U; i < GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Count; i++) {
                        AmbaSvcWrap_MisraMemcpy(BlendDataPtr, Calib3dData->Cam[i].BlendTbl.Table, GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[i].Size);
                        AmbaMisra_TypeCast(&TmpAddr, &BlendDataPtr);
                        TmpAddr += GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[i].Size;
                        AmbaMisra_TypeCast(&BlendDataPtr, &TmpAddr);
                    }
                    {
                        /* Compress blend data by LZ4 */
                        UINT8 *SrcBuf, *DstBuf;
                        UINT32 OutputSize;
                        AmbaMisra_TypeCast(&SrcBuf, &BlendDataBuffer);
                        AmbaMisra_TypeCast(&DstBuf, &CalibDataPtr);
                        Rval = AmbaCalibData_Compress(AMBA_CALIB_DATA_CMPR_ALGO_LZ4, SrcBuf, TotalBlendSize, DstBuf, TotalBlendSize, &OutputSize);
                        if (Rval == SVC_OK) {
                            void *SrcOsdData;
                            UINT8 *Data3DBuffer;
                            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Compressed = AMBA_CALIB_DATA_CMPR_ALGO_LZ4;
                            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Size = OutputSize;
                            AmbaMisra_TypeCast(&TmpAddr, &CalibDataPtr);
                            TmpAddr += GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Size;
                            AmbaMisra_TypeCast(&CalibDataPtr, &TmpAddr);
                            /* Osd data */
                            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_OSD].Compressed = AMBA_CALIB_DATA_CMPR_ALGO_LZ4;
                            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_OSD].Size = Osd3DInfo->CmprSize;
                            AmbaMisra_TypeCast(&SrcOsdData, &Osd3DInfo->CmprData);
                            AmbaSvcWrap_MisraMemcpy(CalibDataPtr, SrcOsdData, GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_OSD].Size);
                            AnimIndex->DataInfo.OsdDataDesc[0].GroupId = g_CalibDataHdlr->GroupNum;
                            AnimIndex->DataInfo.OsdDataDesc[0].ItemId = SVC_CALIB_ITEM_ID_OSD;
                            AnimIndex->DataInfo.OsdDataDesc[0].ElementId = 0U;
                            /* Write 3D avm liveview data */
                            AmbaMisra_TypeCast(&Data3DBuffer, &CalibDataBuffer);
                            for (i = 0U; i < GroupDesc.Count; i++) {
                                Data3DBufferSize += GroupDesc.ItemInfo[i].Size;
                            }
                            Rval = AmbaCalibData_Write(g_CalibDataHdlr->HdlrId, &GroupDesc, Data3DBuffer, Data3DBufferSize);
                            if (Rval == SVC_OK) {
                                /* Fill user data */
                                UINT8 Wp = ViewWp[REF_CALIB_DISP_ID_3D];
                                for (i = 0U; i < GroupDesc.Count; i++) {
                                    if (GroupDesc.ItemInfo[i].Compressed != AMBA_CALIB_DATA_CMPR_ALGO_NONE) {
                                        AnimHeader.CompSize[REF_CALIB_DISP_ID_3D][Wp] += GroupDesc.ItemInfo[i].Size;
                                    }
                                }
                                AnimHeader.WarpHeader[REF_CALIB_DISP_ID_3D][Wp].WarpCount = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].Count;
                                for (i = 0U; i < AnimHeader.WarpHeader[REF_CALIB_DISP_ID_3D][Wp].WarpCount; i++) {
                                    AnimHeader.WarpHeader[REF_CALIB_DISP_ID_3D][Wp].HorGridNum[i] = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[i].Warp.Data.HorGridNum;
                                    AnimHeader.WarpHeader[REF_CALIB_DISP_ID_3D][Wp].VerGridNum[i] = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[i].Warp.Data.VerGridNum;
                                }
                                AnimHeader.BlendHeader[REF_CALIB_DISP_ID_3D][Wp].BlendCount = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Count;
                                for (i = 0U; i < AnimHeader.BlendHeader[REF_CALIB_DISP_ID_3D][Wp].BlendCount; i++) {
                                    AnimHeader.BlendHeader[REF_CALIB_DISP_ID_3D][Wp].Width[i] = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[i].Blend.Width;
                                    AnimHeader.BlendHeader[REF_CALIB_DISP_ID_3D][Wp].Height[i] = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[i].Blend.Height;
                                }
                                ViewWp[REF_CALIB_DISP_ID_3D]++;
                                /* Fill raw data */
                                for (i = 0U; i < AnimIndex->DataInfo.GroupCount; i++) {
                                    AnimIndex->DataInfo.GroupDesc[i].Id = g_CalibDataHdlr->GroupNum - 1U;
                                    AnimIndex->DataInfo.GroupDesc[i].ItemCount = GroupDesc.Count;
                                    for (j = 0U; j < AnimIndex->DataInfo.GroupDesc[i].ItemCount; j++) {
                                        AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementCount = GroupDesc.ItemInfo[j].Count;
                                        for (k = 0U; k < AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementCount; k++) {
                                            if (j == SVC_CALIB_ITEM_ID_WARP) {
                                                AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementInfo[k].DataId = k;
                                            } else if (j == SVC_CALIB_ITEM_ID_BLEND) {
                                                AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementInfo[k].DataId = k;
                                                AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementInfo[k].VoutId = VOUT_IDX_B;
                                            } else {
                                                AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementInfo[k].VoutId = VOUT_IDX_B;
                                            }
                                        }
                                    }
                                }
                                for (i = 0U; i < AnimIndex->VoutCount; i++) {
                                    for (j = 0U; j < AnimIndex->ChanCount; j++) {
                                        AnimIndex->RenderCfg[i][j].ChanInfo.BlendNum = 1U;
                                        AnimIndex->RenderCfg[i][j].ChanInfo.RotateFlip = Calib3dData->Cam[j].VoutRotation * 2U; // transfer from definition of calib to dsp
                                        AnimIndex->RenderCfg[i][j].ChanInfo.Window.OffsetX = Calib3dData->Cam[j].VoutArea.StartX;
                                        AnimIndex->RenderCfg[i][j].ChanInfo.Window.OffsetY = Calib3dData->Cam[j].VoutArea.StartY;
                                        AnimIndex->RenderCfg[i][j].ChanInfo.Window.Width = Calib3dData->Cam[j].VoutArea.Width;
                                        AnimIndex->RenderCfg[i][j].ChanInfo.Window.Height = Calib3dData->Cam[j].VoutArea.Height;
                                    }
                                }
                                /* Move AnimIndex after (disp 0, view 0) is finished */
                                AmbaMisra_TypeCast(&TmpAddr, &AnimIndex);
                                TmpAddr += sizeof(SVC_ANIM_INDEX_s);
                                AmbaMisra_TypeCast(&AnimIndex, &TmpAddr);
                            } else {
                                SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCalibData_Write", Rval);
                            }
                        } else {
                            SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCalibData_Compress", Rval);
                        }
                    }
                } else {
                    SvcAnimCalib_Perror(__func__, __LINE__, "RefMemPool_Allocate", Rval);
                }
            } else {
                SvcAnimCalib_Perror(__func__, __LINE__, "RefMemPool_Allocate", Rval);
            }
        } else {
            SvcAnimCalib_Perror(__func__, __LINE__, "RefMemPool_Allocate", Rval);
        }
        if (Rval == SVC_OK) {
            /* Main view data */
            UINT32 CalibDataBufSize;
            AmbaMisra_TypeCast(&Ptr, &pGroupDesc);
            AmbaSvcWrap_MisraMemset(Ptr, 0, sizeof(AMBA_CALIB_DATA_GROUP_DESC_s));
            GroupDesc.StorageId = AMBA_CALIB_DATA_STORAGE_NAND;
            GroupDesc.PartId = AMBA_USER_PARTITION_RESERVED0;
            GroupDesc.Count = REF_CALIB_MAINVIEW_ITEM_COUNT;
            /* Raw data (user-defined) */
            /* Fill for (disp 1, view 0) */
            AnimIndex->Version = SVC_CALIB_VERSION;
            AnimIndex->VoutCount = 1U;
            AnimIndex->ChanCount = 1U;
            AnimIndex->DataInfo.OsdCount = 0U;
            AnimIndex->DataInfo.GroupCount = 1U;
            for (i = 0U; i < AnimIndex->ChanCount; i++) {
                AnimIndex->WarpEnable[i] = 1U;
                AnimIndex->BlendType[i] = 0U;
            }
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].Count = 1U;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].Compressed = AMBA_CALIB_DATA_CMPR_ALGO_NONE;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Type = AMBA_CALIB_DATA_TYPE_WARP;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Warp.Version = IK_WARP_VER;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Warp.Data.HorGridNum = CalibMainviewData->WarpTbl.HorGridNum;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Warp.Data.VerGridNum = CalibMainviewData->WarpTbl.VerGridNum;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Warp.Data.TileWidth = CalibMainviewData->WarpTbl.TileWidthExp;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Warp.Data.TileHeight = CalibMainviewData->WarpTbl.TileHeightExp;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Warp.Data.CalibSensorGeo.StartX = CalibMainviewData->WarpTbl.CalibSensorGeo.StartX;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Warp.Data.CalibSensorGeo.StartY = CalibMainviewData->WarpTbl.CalibSensorGeo.StartY;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Warp.Data.CalibSensorGeo.Width = CalibMainviewData->WarpTbl.CalibSensorGeo.Width;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Warp.Data.CalibSensorGeo.Height = CalibMainviewData->WarpTbl.CalibSensorGeo.Height;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Warp.Data.CalibSensorGeo.HSubSampleFactorDen = CalibMainviewData->WarpTbl.CalibSensorGeo.HSubSample.FactorDen;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Warp.Data.CalibSensorGeo.HSubSampleFactorNum = CalibMainviewData->WarpTbl.CalibSensorGeo.HSubSample.FactorNum;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Warp.Data.CalibSensorGeo.VSubSampleFactorDen = CalibMainviewData->WarpTbl.CalibSensorGeo.VSubSample.FactorDen;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Warp.Data.CalibSensorGeo.VSubSampleFactorNum = CalibMainviewData->WarpTbl.CalibSensorGeo.VSubSample.FactorNum;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Size = CalibMainviewData->WarpTbl.HorGridNum * CalibMainviewData->WarpTbl.VerGridNum * sizeof(AMBA_CAL_GRID_POINT_s);
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].Size = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Size;
            CalibDataBufSize = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Size;
            Rval = RefMemPool_Allocate(REF_MEM_POOL_REGION_CACHE, AMBA_CACHE_LINE_SIZE, &CalibDataBuffer, &RawBuffer, CalibDataBufSize);
            if (Rval == SVC_OK) {
                UINT8 *DataMainviewBuffer;
                AmbaMisra_TypeCast(&DataMainviewBuffer, &CalibDataBuffer);
                AmbaSvcWrap_MisraMemcpy(CalibDataBuffer, CalibMainviewData->WarpTbl.WarpVector, GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[0].Size);
                Rval = AmbaCalibData_Write(g_CalibDataHdlr->HdlrId, &GroupDesc, DataMainviewBuffer, CalibDataBufSize);
                if (Rval == SVC_OK) {
                    /* Fill user data */
                    UINT8 Wp = ViewWp[REF_CALIB_DISP_ID_MAINVIEW];
                    for (i = 0U; i < GroupDesc.Count; i++) {
                        if (GroupDesc.ItemInfo[i].Compressed != AMBA_CALIB_DATA_CMPR_ALGO_NONE) {
                            AnimHeader.CompSize[REF_CALIB_DISP_ID_MAINVIEW][Wp] += GroupDesc.ItemInfo[i].Size;
                        }
                    }
                    AnimHeader.WarpHeader[REF_CALIB_DISP_ID_MAINVIEW][Wp].WarpCount = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].Count;
                    for (i = 0U; i < AnimHeader.WarpHeader[REF_CALIB_DISP_ID_MAINVIEW][Wp].WarpCount; i++) {
                        AnimHeader.WarpHeader[REF_CALIB_DISP_ID_MAINVIEW][Wp].HorGridNum[i] = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[i].Warp.Data.HorGridNum;
                        AnimHeader.WarpHeader[REF_CALIB_DISP_ID_MAINVIEW][Wp].VerGridNum[i] = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[i].Warp.Data.VerGridNum;
                    }
                    AnimHeader.BlendHeader[REF_CALIB_DISP_ID_MAINVIEW][Wp].BlendCount = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Count;
                    for (i = 0U; i < AnimHeader.BlendHeader[REF_CALIB_DISP_ID_MAINVIEW][Wp].BlendCount; i++) {
                        AnimHeader.BlendHeader[REF_CALIB_DISP_ID_MAINVIEW][Wp].Width[i] = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[i].Blend.Width;
                        AnimHeader.BlendHeader[REF_CALIB_DISP_ID_MAINVIEW][Wp].Height[i] = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[i].Blend.Height;
                    }
                    ViewWp[REF_CALIB_DISP_ID_MAINVIEW]++;
                    /* Fill raw data */
                    for (i = 0U; i < AnimIndex->DataInfo.GroupCount; i++) {
                        AnimIndex->DataInfo.GroupDesc[i].Id = g_CalibDataHdlr->GroupNum - 1U;
                        AnimIndex->DataInfo.GroupDesc[i].ItemCount = GroupDesc.Count;
                        for (j = 0U; j < AnimIndex->DataInfo.GroupDesc[i].ItemCount; j++) {
                            AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementCount = GroupDesc.ItemInfo[j].Count;
                            for (k = 0U; k < AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementCount; k++) {
                                if (j == SVC_CALIB_ITEM_ID_WARP) {
                                    AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementInfo[k].DataId = k;
                                } else if (j == SVC_CALIB_ITEM_ID_BLEND) {
                                    AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementInfo[k].DataId = k;
                                    AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementInfo[k].VoutId = VOUT_IDX_B;
                                } else {
                                    AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementInfo[k].VoutId = VOUT_IDX_B;
                                }
                            }
                        }
                    }
                    for (i = 0U; i < AnimIndex->VoutCount; i++) {
                        for (j = 0U; j < AnimIndex->ChanCount; j++) {
                            AnimIndex->RenderCfg[i][j].ChanInfo.BlendNum = 0U;
                            AnimIndex->RenderCfg[i][j].ChanInfo.RotateFlip = AMBA_CAL_ROTATE_90 * 2U; // transfer from definition of calib (AMBA_CAL_ROTATE_0) to dsp (AMBA_DSP_ROTATE_0)
                            AnimIndex->RenderCfg[i][j].ChanInfo.Window.OffsetX = CalibMainviewData->VoutArea.StartX;
                            AnimIndex->RenderCfg[i][j].ChanInfo.Window.OffsetY = CalibMainviewData->VoutArea.StartY;
                            /* Window size of chan is based on after-rotate window */
                            if ((AnimIndex->RenderCfg[i][j].ChanInfo.RotateFlip == AMBA_DSP_ROTATE_90) || (AnimIndex->RenderCfg[i][j].ChanInfo.RotateFlip == AMBA_DSP_ROTATE_270)) {
                                AnimIndex->RenderCfg[i][j].ChanInfo.Window.Width = CalibMainviewData->VoutArea.Height;
                                AnimIndex->RenderCfg[i][j].ChanInfo.Window.Height = CalibMainviewData->VoutArea.Width;
                            } else {
                                AnimIndex->RenderCfg[i][j].ChanInfo.Window.Width = CalibMainviewData->VoutArea.Width;
                                AnimIndex->RenderCfg[i][j].ChanInfo.Window.Height = CalibMainviewData->VoutArea.Height;
                            }
                        }
                    }
                } else {
                    SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCalibData_Write", Rval);
                }
            } else {
                SvcAnimCalib_Perror(__func__, __LINE__, "RefMemPool_Allocate", Rval);
            }
        }
        if (Rval == SVC_OK) {
            /* Write raw data */
            UINT8 *U8Ptr;
            AmbaMisra_TypeCast(&Ptr, &pGroupDesc);
            AmbaSvcWrap_MisraMemset(Ptr, 0, sizeof(AMBA_CALIB_DATA_GROUP_DESC_s));
            GroupDesc.StorageId = AMBA_CALIB_DATA_STORAGE_NAND;
            GroupDesc.PartId = AMBA_USER_PARTITION_RESERVED0;
            GroupDesc.Count = 1U;
            GroupDesc.ItemInfo[0].Count = 1U;
            GroupDesc.ItemInfo[0].Compressed = AMBA_CALIB_DATA_CMPR_ALGO_NONE;
            GroupDesc.ItemInfo[0].ElementInfo[0].Type = AMBA_CALIB_DATA_TYPE_RAW;
            GroupDesc.ItemInfo[0].ElementInfo[0].Raw.Version = SVC_CALIB_VERSION;
            GroupDesc.ItemInfo[0].ElementInfo[0].Size = RawDataBufferSize;
            GroupDesc.ItemInfo[0].Size = GroupDesc.ItemInfo[0].ElementInfo[0].Size;
            AmbaMisra_TypeCast(&U8Ptr, &RawDataBuffer);
            Rval = AmbaCalibData_Write(g_CalibDataHdlr->HdlrId, &GroupDesc, U8Ptr, RawDataBufferSize);
            if (Rval != SVC_OK) {
                SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCalibData_Write", Rval);
            }
        }
        if (Rval == SVC_OK) {
            /* User data (user-defined) */
            UINT8 *U8Ptr;
            AmbaMisra_TypeCast(&U8Ptr, &pAnimHeader);
            AnimHeader.DispCount = SVC_CALIB_DISP_COUNT;
            AnimHeader.GroupCount = g_CalibDataHdlr->GroupNum;
            for (i = 0U; i < AnimHeader.DispCount; i++) {
                AnimHeader.ViewCount[i] = 1U;
            }
            Rval = AmbaCalibData_AddUdta(g_CalibDataHdlr->HdlrId, 0U, U8Ptr, sizeof(SVC_ANIM_HEADER_s));
            if (Rval != SVC_OK) {
                SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCalibData_SetUdta", Rval);
            }
        }
        if (Rval == SVC_OK) {
            REF_PREF_s RefPref;
            REF_PREF_s *pRefPref = &RefPref;
            UINT32 NumSector = (sizeof(RefPref) + 511U) / 512U;
            AMBA_NVM_SECTOR_CONFIG_s SecConfig;
            /* Delete then writing data to the main storage */
            Rval = AmbaCalibData_Delete(g_CalibDataHdlr);
            if (Rval == SVC_OK) {
                Rval = RefPref_GetPref(&RefPref);
                if (Rval == SVC_OK) {
                    RefPref.CalibDataInfo.CalibDataValid = 1U;
                    Rval = RefPref_SetPref(&RefPref);
                    if (Rval == SVC_OK) {
                        SecConfig.NumSector = NumSector;
                        SecConfig.StartSector = 0U;
                        AmbaMisra_TypeCast(&SecConfig.pDataBuf, &pRefPref);
                        Rval = NAND2A(AmbaNAND_WriteSector(AMBA_USER_PARTITION_USER_SETTING, &SecConfig, AMBA_KAL_WAIT_FOREVER));
                        if (Rval == SVC_OK) {
                            AmbaPrint_PrintUInt5("SvcAnimCalib_CalibToNand: AmbaNAND_WriteSector() done!", 0U, 0U, 0U, 0U, 0U);
                        } else {
                            SvcAnimCalib_Perror(__func__, __LINE__, "AmbaNAND_WriteSector", Rval);
                        }
                    } else {
                        SvcAnimCalib_Perror(__func__, __LINE__, "RefPref_SetPref", Rval);
                    }
                } else {
                    SvcAnimCalib_Perror(__func__, __LINE__, "RefPref_GetPref", Rval);
                }
            } else {
                SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCalibData_Delete", Rval);
            }
        }
    }
    return Rval;
}
#endif

static UINT32 SvcAnimCalib_CalibToSDCard(const AMBA_CAL_AVM_3D_DATA_s *Calib3dData, const SVC_CALIB_OSD_INFO_s *Osd3DInfo)
{
    UINT32 Rval = SVC_OK;
    if (Calib3dData == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: Calib3dData is NULL!!!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        UINT8 i, j, k, ViewWp[SVC_CALIB_DISP_COUNT] = {0U};
        SVC_ANIM_INDEX_s *AnimIndex;
        UINT32 Data3DBufferSize = 0U;
        UINT32 RawDataBufferSize = sizeof(SVC_ANIM_INDEX_s);
        UINT32 BufBaseTmp, BufSize = 0U;
        ULONG BufBase = 0U;
        AMBA_CALIB_DATA_GROUP_DESC_s GroupDesc;
        UINT32 GroupCnt = 0U;
        const AMBA_CALIB_DATA_GROUP_DESC_s *pGroupDesc = &GroupDesc;
        SVC_ANIM_HEADER_s AnimHeader;
        const SVC_ANIM_HEADER_s *pAnimHeader = &AnimHeader;
        void *Ptr;
        /* Fill calib data buf */
        void *CalibDataPtr, *BlendDataPtr;
        const void *BlendDataBuf, *CalibDataBuf;
        UINT32 TotalBlendSize = 0U;

        AmbaMisra_TypeCast(&Ptr, &pAnimHeader);
        AmbaSvcWrap_MisraMemset(&AnimHeader, 0, sizeof(SVC_ANIM_HEADER_s));
        Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CAL_SUR_LV_CAL, &BufBase, &BufSize);
        BufBase += InitBufferSize;//skip inital buffer
        if (Rval == SVC_OK) {
            BufBaseTmp = BufBase;
            AmbaMisra_TypeCast(&Ptr, &pGroupDesc);
            AmbaSvcWrap_MisraMemset(Ptr, 0, sizeof(AMBA_CALIB_DATA_GROUP_DESC_s));
            GroupDesc.StorageId = AMBA_CALIB_DATA_STORAGE_SD;
            GroupDesc.PartId = (UINT32)'C';
            GroupDesc.Count = SVC_CALIB_3D_ITEM_COUNT;
            /* Raw data (user-defined) */
            /* Fill for (disp 0, view 0) */
            AmbaMisra_TypeCast(&AnimIndex, &BufBase);
            AnimIndex->Version = SVC_CALIB_VERSION;
            AnimIndex->VoutCount = 1U;
            AnimIndex->ChanCount = (UINT8)AMBA_CAL_AVM_CAM_MAX; // Front, back, left, right
            AnimIndex->DataInfo.OsdCount = 1U;
            AnimIndex->DataInfo.GroupCount = 1U;
            for (i = 0U; i < AnimIndex->ChanCount; i++) {
                AnimIndex->WarpEnable[i] = 1U;
                AnimIndex->BlendType[i] = 1U;
            }
            /* 3D data */
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].Compressed = AMBA_CALIB_DATA_CMPR_ALGO_NONE;
            for (i = 0U; i < (UINT8)AMBA_CAL_AVM_CAM_MAX; i++) {
                /* Warp */
                UINT8 WarpElementWp = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].Count;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Type = AMBA_CALIB_DATA_TYPE_WARP;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Version = IK_WARP_VER;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.HorGridNum = Calib3dData->Cam[i].WarpTbl.HorGridNum;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.VerGridNum = Calib3dData->Cam[i].WarpTbl.VerGridNum;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.TileWidth = Calib3dData->Cam[i].WarpTbl.TileWidthExp;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.TileHeight = Calib3dData->Cam[i].WarpTbl.TileHeightExp;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.CalibSensorGeo.StartX = Calib3dData->Cam[i].WarpTbl.CalibSensorGeo.StartX;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.CalibSensorGeo.StartY = Calib3dData->Cam[i].WarpTbl.CalibSensorGeo.StartY;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.CalibSensorGeo.Width = Calib3dData->Cam[i].WarpTbl.CalibSensorGeo.Width;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.CalibSensorGeo.Height = Calib3dData->Cam[i].WarpTbl.CalibSensorGeo.Height;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.CalibSensorGeo.HSubSampleFactorDen = Calib3dData->Cam[i].WarpTbl.CalibSensorGeo.HSubSample.FactorDen;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.CalibSensorGeo.HSubSampleFactorNum = Calib3dData->Cam[i].WarpTbl.CalibSensorGeo.HSubSample.FactorNum;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.CalibSensorGeo.VSubSampleFactorDen = Calib3dData->Cam[i].WarpTbl.CalibSensorGeo.VSubSample.FactorDen;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Warp.Data.CalibSensorGeo.VSubSampleFactorNum = Calib3dData->Cam[i].WarpTbl.CalibSensorGeo.VSubSample.FactorNum;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Size = Calib3dData->Cam[i].WarpTbl.HorGridNum * Calib3dData->Cam[i].WarpTbl.VerGridNum * sizeof(AMBA_CAL_GRID_POINT_s);
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].Size += GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[WarpElementWp].Size;
                GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].Count++;
                {
                    /* Blend */
                    UINT8 BlendElementWp = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Count;
                    GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[BlendElementWp].Type = AMBA_CALIB_DATA_TYPE_BLEND;
                    GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[BlendElementWp].Blend.Version = SVC_CALIB_VERSION;
                    GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[BlendElementWp].Blend.Width = Calib3dData->Cam[i].BlendTbl.Width;
                    GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[BlendElementWp].Blend.Height = Calib3dData->Cam[i].BlendTbl.Height;
                    GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[BlendElementWp].Size = Calib3dData->Cam[i].BlendTbl.Width * Calib3dData->Cam[i].BlendTbl.Height;
                    GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Count++;
                }
            }
            /* Osd */
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_OSD].Count = 1U;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_OSD].ElementInfo[0].Type = AMBA_CALIB_DATA_TYPE_OSD;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_OSD].ElementInfo[0].Size = Osd3DInfo->DecmprDataSize;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_OSD].ElementInfo[0].Osd.Version = SVC_CALIB_VERSION;
            GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_OSD].ElementInfo[0].Osd.OsdWindow = Osd3DInfo->Window;

            BufBaseTmp += RawDataBufferSize;

            AmbaMisra_TypeCast(&CalibDataBuf, &BufBaseTmp);
            AmbaMisra_TypeCast(&CalibDataPtr, &BufBaseTmp);
            /* Warp data */
            for (i = 0U; i < GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].Count; i++) {
                AmbaSvcWrap_MisraMemcpy(CalibDataPtr, Calib3dData->Cam[i].WarpTbl.WarpVector, GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[i].Size);
                BufBaseTmp += GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[i].Size;
                AmbaMisra_TypeCast(&CalibDataPtr, &BufBaseTmp);
            }
            for (i = 0U; i < GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Count; i++) {
                TotalBlendSize += GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[i].Size;
            }

            {
                UINT32 BlendTempBuf = ((BufBase + BufSize) - TotalBlendSize) - 1U;
                /* Blend data */
                AmbaMisra_TypeCast(&BlendDataBuf, &BlendTempBuf);
                AmbaMisra_TypeCast(&BlendDataPtr, &BlendTempBuf);
                for (i = 0U; i < GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Count; i++) {
                    AmbaSvcWrap_MisraMemcpy(BlendDataPtr, Calib3dData->Cam[i].BlendTbl.Table, GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[i].Size);
                    BlendTempBuf += GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[i].Size;
                    AmbaMisra_TypeCast(&BlendDataPtr, &BlendTempBuf);
                }
            }
            {
                /* Compress blend data by LZ4 */
                const UINT8 *SrcBuf;
                UINT8 *DstBuf;
                UINT32 OutputSize;
                AMBA_CALIB_DATA_CMPR_INFO_s CmprInfo;
                AmbaMisra_TypeCast(&SrcBuf, &BlendDataBuf);
                AmbaMisra_TypeCast(&DstBuf, &BufBaseTmp);
                /* TODO */
                CmprInfo.Lz4Info.SegmentSize = (UINT16)(16U * 1024U);
                Rval = AmbaCalibData_Compress(AMBA_CALIB_DATA_CMPR_ALGO_LZ4, &CmprInfo, SrcBuf, TotalBlendSize, DstBuf, TotalBlendSize, &OutputSize);
                if (Rval == SVC_OK) {
                    const void *SrcOsdData;
                    UINT8 *Data3DBuffer;
                    GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Compressed = AMBA_CALIB_DATA_CMPR_ALGO_LZ4;
                    GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Size = OutputSize;
                    BufBaseTmp += GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Size;
                    AmbaMisra_TypeCast(&CalibDataPtr, &BufBaseTmp);
                    /* Osd data */
                    GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_OSD].Compressed = AMBA_CALIB_DATA_CMPR_ALGO_LZ4;
                    GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_OSD].Size = Osd3DInfo->CmprSize;
                    AmbaMisra_TypeCast(&SrcOsdData, &Osd3DInfo->CmprData);
                    AmbaSvcWrap_MisraMemcpy(CalibDataPtr, SrcOsdData, GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_OSD].Size);
                    AnimIndex->DataInfo.OsdDataDesc[0].GroupId = GroupCnt;
                    AnimIndex->DataInfo.OsdDataDesc[0].ItemId = SVC_CALIB_ITEM_ID_OSD;
                    AnimIndex->DataInfo.OsdDataDesc[0].ElementId = 0U;
                    /* Write 3D avm liveview data */
                    AmbaMisra_TypeCast(&Data3DBuffer, &CalibDataBuf);
                    for (i = 0U; i < GroupDesc.Count; i++) {
                        Data3DBufferSize += GroupDesc.ItemInfo[i].Size;
                    }
                    Rval = AmbaCalibData_Write(g_CalibDataHdlr, &GroupDesc, Data3DBuffer, Data3DBufferSize);
                    if (Rval == SVC_OK) {
                        /* Fill user data */
                        UINT8 Wp = ViewWp[SVC_CALIB_DISP_ID_3D];
                        for (i = 0U; i < GroupDesc.Count; i++) {
                            if (GroupDesc.ItemInfo[i].Compressed != AMBA_CALIB_DATA_CMPR_ALGO_NONE) {
                                AnimHeader.CompSize[SVC_CALIB_DISP_ID_3D][Wp] += GroupDesc.ItemInfo[i].Size;
                            }
                        }
                        AnimHeader.WarpHeader[SVC_CALIB_DISP_ID_3D][Wp].WarpCount = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].Count;
                        for (i = 0U; i < AnimHeader.WarpHeader[SVC_CALIB_DISP_ID_3D][Wp].WarpCount; i++) {
                            AnimHeader.WarpHeader[SVC_CALIB_DISP_ID_3D][Wp].HorGridNum[i] = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[i].Warp.Data.HorGridNum;
                            AnimHeader.WarpHeader[SVC_CALIB_DISP_ID_3D][Wp].VerGridNum[i] = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_WARP].ElementInfo[i].Warp.Data.VerGridNum;
                        }
                        AnimHeader.BlendHeader[SVC_CALIB_DISP_ID_3D][Wp].BlendCount = GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].Count;
                        for (i = 0U; i < AnimHeader.BlendHeader[SVC_CALIB_DISP_ID_3D][Wp].BlendCount; i++) {
                            AnimHeader.BlendHeader[SVC_CALIB_DISP_ID_3D][Wp].Width[i] = (UINT16)GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[i].Blend.Width;
                            AnimHeader.BlendHeader[SVC_CALIB_DISP_ID_3D][Wp].Height[i] = (UINT16)GroupDesc.ItemInfo[SVC_CALIB_ITEM_ID_BLEND].ElementInfo[i].Blend.Height;
                        }
                        ViewWp[SVC_CALIB_DISP_ID_3D]++;
                        /* Fill raw data */
                        for (i = 0U; i < AnimIndex->DataInfo.GroupCount; i++) {
                            AnimIndex->DataInfo.GroupDesc[i].Id = GroupCnt;
                            AnimIndex->DataInfo.GroupDesc[i].ItemCount = GroupDesc.Count;
                            for (j = 0U; j < AnimIndex->DataInfo.GroupDesc[i].ItemCount; j++) {
                                AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementCount = GroupDesc.ItemInfo[j].Count;
                                for (k = 0U; k < AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementCount; k++) {
                                    if (j == SVC_CALIB_ITEM_ID_WARP) {
                                        AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementInfo[k].DataId = k;
                                    } else if (j == SVC_CALIB_ITEM_ID_BLEND) {
                                        AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementInfo[k].DataId = k;
                                        AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementInfo[k].VoutId = VOUT_IDX_B;
                                    } else {
                                        AnimIndex->DataInfo.GroupDesc[i].ItemInfo[j].ElementInfo[k].VoutId = VOUT_IDX_B;
                                    }
                                }
                            }
                        }
                        for (i = 0U; i < AnimIndex->VoutCount; i++) {
                            for (j = 0U; j < AnimIndex->ChanCount; j++) {
                                AnimIndex->RenderCfg[i][j].ChanInfo.BlendNum = 1U;
                                AnimIndex->RenderCfg[i][j].ChanInfo.RotateFlip = (UINT8)Calib3dData->Cam[j].VoutRotation * 2U; // transfer from definition of calib to dsp
                                AnimIndex->RenderCfg[i][j].ChanInfo.Window.OffsetX = (UINT16)Calib3dData->Cam[j].VoutArea.StartX;
                                AnimIndex->RenderCfg[i][j].ChanInfo.Window.OffsetY = (UINT16)Calib3dData->Cam[j].VoutArea.StartY;
                                AnimIndex->RenderCfg[i][j].ChanInfo.Window.Width = (UINT16)Calib3dData->Cam[j].VoutArea.Width;
                                AnimIndex->RenderCfg[i][j].ChanInfo.Window.Height = (UINT16)Calib3dData->Cam[j].VoutArea.Height;
                            }
                        }
                        GroupCnt++;
                    } else {
                        SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCalibData_Write", Rval);
                    }
                } else {
                    SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCalibData_Compress", Rval);
                }
            }
        } else {
            SvcAnimCalib_Perror(__func__, __LINE__, "SvcBuffer_Request", Rval);
        }
        if (Rval == SVC_OK) {
            /* Write raw data */
            UINT8 *U8Ptr;
            AmbaMisra_TypeCast(&Ptr, &pGroupDesc);
            AmbaSvcWrap_MisraMemset(Ptr, 0, sizeof(AMBA_CALIB_DATA_GROUP_DESC_s));
            GroupDesc.StorageId = AMBA_CALIB_DATA_STORAGE_SD;
            GroupDesc.PartId = (UINT32)'C';
            GroupDesc.Count = 1U;
            GroupDesc.ItemInfo[0].Count = 1U;
            GroupDesc.ItemInfo[0].Compressed = AMBA_CALIB_DATA_CMPR_ALGO_NONE;
            GroupDesc.ItemInfo[0].ElementInfo[0].Type = AMBA_CALIB_DATA_TYPE_RAW;
            GroupDesc.ItemInfo[0].ElementInfo[0].Raw.Version = SVC_CALIB_VERSION;
            GroupDesc.ItemInfo[0].ElementInfo[0].Size = RawDataBufferSize;
            GroupDesc.ItemInfo[0].Size = GroupDesc.ItemInfo[0].ElementInfo[0].Size;
            AmbaMisra_TypeCast(&U8Ptr, &BufBase);
            Rval = AmbaCalibData_Write(g_CalibDataHdlr, &GroupDesc, U8Ptr, RawDataBufferSize);
            if (Rval != SVC_OK) {
                SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCalibData_Write", Rval);
            } else {
                GroupCnt++;
            }
        }
        if (Rval == SVC_OK) {
            /* User data (user-defined) */
            const UINT8 *U8Ptr;
            AmbaMisra_TypeCast(&U8Ptr, &pAnimHeader);
            AnimHeader.DispCount = SVC_CALIB_DISP_COUNT;
            AnimHeader.GroupCount = GroupCnt;
            for (i = 0U; i < AnimHeader.DispCount; i++) {
                AnimHeader.ViewCount[i] = 1U;
            }
            Rval = AmbaCalibData_AddUdta(g_CalibDataHdlr, U8Ptr, sizeof(SVC_ANIM_HEADER_s));
            if (Rval != SVC_OK) {
                SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCalibData_SetUdta", Rval);
            }
        }
        if (Rval == SVC_OK) {
            /* Delete then writing data to the main storage */
            Rval = AmbaCalibData_Delete(g_CalibDataHdlr);
            if (Rval != SVC_OK) {
                SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCalibData_Delete", Rval);
            }
        }
    }
    return Rval;
}

static void SvcAnimCalib_Init(void)
{
    UINT32 Rval;
    UINT32 BufSize = 0U;
    ULONG BufBase = 0U;
    AMBA_CALIB_DATA_INIT_CFG_s CalibDataInitCfg;
    CalibDataInitCfg.MaxHdlr = 1U;
    CalibDataInitCfg.StorageId = AMBA_CALIB_DATA_STORAGE_SD;
    CalibDataInitCfg.PartId = (UINT32)'C';
    CalibDataInitCfg.MaxGroup = 3U;
    CalibDataInitCfg.MaxWarp = 5U;
    CalibDataInitCfg.MaxVig = 0U;
    CalibDataInitCfg.MaxBlend = 4U;
    CalibDataInitCfg.MaxOsd = 1U;
    CalibDataInitCfg.MaxRaw = 1U;
    CalibDataInitCfg.MaxUdta = 1U;
    CalibDataInitCfg.UdtaSize[0] = sizeof(SVC_ANIM_HEADER_s);
    CalibDataInitCfg.BufferSize = InitBufferSize;
    Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CAL_SUR_LV_CAL, &BufBase, &BufSize);
    if (Rval == SVC_OK) {
        AmbaMisra_TypeCast(&CalibDataInitCfg.Buffer, &BufBase);
        AmbaSvcWrap_MisraMemset(CalibDataInitCfg.Buffer, 0, (SIZE_t)InitBufferSize);
        Rval = AmbaCalibData_Init(&CalibDataInitCfg);
        if (Rval == SVC_OK) {
            Rval = AmbaCalibData_Create(AMBA_CALIB_DATA_MODE_WRITE, &g_CalibDataHdlr);
            if (Rval != SVC_OK) {
                SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCalibData_Create", Rval);
            }
        } else {
            SvcAnimCalib_Perror(__func__, __LINE__, "AmbaCalibData_Init", Rval);
        }
    } else {
        SvcAnimCalib_Perror(__func__, __LINE__, "SvcBuffer_Request", Rval);
    }
    AmbaMisra_TouchUnused(&g_AvmGridStatus[0]);
    AmbaMisra_TouchUnused(&g_DbgFlag);
}

UINT32 SvcAnimCalib_QueryBufSize(SIZE_t *Size)
{
    UINT32 AnimIndexSize = sizeof(SVC_ANIM_INDEX_s)*SVC_CALIB_VIEW_COUNT;
    SIZE_t WarpDataSize;
    UINT32 OSDSize = 354033;
    AMBA_CALIB_DATA_INIT_CFG_s CalibDataInitCfg;
    UINT32 Rval;
    CalibDataInitCfg.MaxHdlr = 1U;
    CalibDataInitCfg.StorageId = AMBA_CALIB_DATA_STORAGE_SD;
    CalibDataInitCfg.PartId = (UINT32)'C';
    CalibDataInitCfg.MaxGroup = 3U;
    CalibDataInitCfg.MaxWarp = 5U;
    CalibDataInitCfg.MaxVig = 0U;
    CalibDataInitCfg.MaxBlend = 4U;
    CalibDataInitCfg.MaxOsd = 1U;
    CalibDataInitCfg.MaxRaw = 1U;
    CalibDataInitCfg.MaxUdta = 1U;
    CalibDataInitCfg.UdtaSize[0] = sizeof(SVC_ANIM_HEADER_s);
    Rval = AmbaCalibData_GetInitBufferSize(&CalibDataInitCfg, &InitBufferSize);
    (void)AmbaCal_AvmGet3DWorkSize(&WarpDataSize);
    *Size = AnimIndexSize + WarpDataSize + OSDSize + InitBufferSize;

    return Rval;
}

void SvcAnimCalib_CmdEntry(UINT32 ArgCount, char * const * ArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Rval;
    static AMBA_CAL_AVM_3D_DATA_s Cal3dData;
    if (AmbaUtility_StringCompare(ArgVector[2U], "cal", AmbaUtility_StringLength("cal")) == 0) {
        SvcAnimCalib_Init();
        if (AmbaUtility_StringCompare(ArgVector[3U], "dbg", AmbaUtility_StringLength("dbg")) == 0) {
            if (AmbaUtility_StringCompare(ArgVector[4U], "on", AmbaUtility_StringLength("on")) == 0) {
                g_DbgFlag = 1U;
            } else if (AmbaUtility_StringCompare(ArgVector[4U], "off", AmbaUtility_StringLength("off")) == 0) {
                g_DbgFlag = 0U;
            } else {
                PrintFunc("Help:\n");
                PrintFunc("svc_anim cal dbg on\n");
                PrintFunc("svc_anim cal dbg off\n");
            }
        } else if (AmbaUtility_StringCompare(ArgVector[3U], "lv", AmbaUtility_StringLength("lv")) == 0) {
            if (ArgCount == 4U) {
                /* Calculate 3D view calibration data */
                Rval = SvcAnimCalib_Tuner("C:\\AvmScript\\3D_t0\\avm_tuner.txt");
                if (Rval == SVC_OK) {
                    Rval = SvcAnimCalib_Cal3dView(&Cal3dData);
                    if (Rval == SVC_OK) {
                        SVC_CALIB_OSD_INFO_s OsdInfo;
                        if (g_DbgFlag == 1U) {
                            SvcAnimCalib_ShowCalibOutput3D(&Cal3dData);
                        }
                        Rval = SvcAnimCalib_GetOsdInfo(&OsdInfo);
                        if (Rval == SVC_OK) {
                            /* Save to SDCard */ //TODO
                            Rval = SvcAnimCalib_CalibToSDCard(&Cal3dData, &OsdInfo);
                            if (Rval != SVC_OK) {
                                SvcAnimCalib_Perror(__func__, __LINE__, "SvcAnimCalib_CalibToSDCard", Rval);
                            }
                        } else {
                            SvcAnimCalib_Perror(__func__, __LINE__, "SvcAnimCalib_FillOsdInfo", Rval);
                        }
                    } else {
                        SvcAnimCalib_Perror(__func__, __LINE__, "SvcAnimCalib_Cal3dView", Rval);
                    }
                } else {
                    SvcAnimCalib_Perror(__func__, __LINE__, "SvcAnimCalib_Tuner", Rval);
                }
            } else {
                AmbaPrint_PrintUInt5("SvcAnimCalib_Command: argument count should be 3!", 0U, 0U, 0U, 0U, 0U);
            }
        } else if (AmbaUtility_StringCompare(ArgVector[3U], "anim", AmbaUtility_StringLength("anim")) == 0) {
        } else {
            PrintFunc("Help:\n");
            PrintFunc("svc_app anim cal dbg\n");
            PrintFunc("svc_app anim cal lv\n");
            PrintFunc("svc_app anim cal anim\n");
        }
    } else {
        PrintFunc("Help:\n");
        PrintFunc("svc_app anim\n");
    }
}

