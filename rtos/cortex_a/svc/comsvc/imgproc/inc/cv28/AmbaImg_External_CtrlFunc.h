/**
 *  @file AmbaImg_External_CtrlFunc.h
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
 *  @Description    :: Image Control Function Implementation
 *
 */

#ifndef AMBA_IMG_EXTERNAL_CTRLFUNC_H
#define AMBA_IMG_EXTERNAL_CTRLFUNC_H
#include "AmbaTypes.h"
#include "AmbaDSP.h"
#include "AmbaImg_AaaDef.h"
// #include "AmbaImg_CtrlFunc_Def.h"

 /*Error code definition*/
#define IMGPROC_ERR_0000  IMGPROC_ERR_BASE + 0x00000001U    // 1   Input argument error
#define IMGPROC_ERR_0001  IMGPROC_ERR_BASE + 0x00000002U    // 2   DS error
#define IMGPROC_ERR_0002  IMGPROC_ERR_BASE + 0x00000004U    // 4   AE error
#define IMGPROC_ERR_0003  IMGPROC_ERR_BASE + 0x00000008U    // 8   AWB error
#define IMGPROC_ERR_0004  IMGPROC_ERR_BASE + 0x00000010U    // 16  ADJ error
#define IMGPROC_ERR_0005  IMGPROC_ERR_BASE + 0x00000020U    // 32  MEM error
#define IMGPROC_ERR_0006  IMGPROC_ERR_BASE + 0x00000040U    // 64  Check version error
#define IMGPROC_ERR_0007  IMGPROC_ERR_BASE + 0x00000040U    // 64  Load IQ table error. If the IQ table address is NULL.
#define IMGPROC_ERR_0008  IMGPROC_ERR_BASE + 0x00000100U    // 256 OS service error

/*Print module allow list*/
#define IMGPROC_PRINT_MODULE_ID  ((UINT16)(IMGPROC_ERR_BASE >> 16U))


/*AAA memory management definition*/
#define WORD_SIZE 4U //1 WORD = 4 Byte = sizeof(void *)

/*AAA memory log definition*/
#define AAA_MEM_LOG_LEN 60U // Memorize for 60 frame


/*Structure*/
/*For IDSP Digtal Gain*/
typedef struct
{
    UINT32      GnR;
    UINT32      GnG;
    UINT32      GnB;
} WB_INFO;

typedef struct
{
    UINT32               Sensr[4];
    UINT8                SensrUpd;
    UINT32               FrEnd[4];
    UINT8                FrEndUpd;
    UINT32               BefCe;
    UINT8                BefCeUpd;
    UINT32               AfrCe;
    UINT8                AfrCeUpd;
} DG_CTRL;

typedef struct
{
    WB_INFO              Sensr[4];
    UINT8                SensrUpd;
    WB_INFO              FrEnd[4];
    UINT8                FrEndUpd;
    WB_INFO              BefCe;
    UINT8                BefCeUpd;
    WB_INFO              AfrCe;
    UINT8                AfrCeUpd;
} WB_CTRL;


/*For AAA memory management*/
typedef struct
{
    void        *pAAAMemHead;
    UINT32      *pAAAMemPtrCurr; //Use UINT32 because it jump one word while using array indexing
    SIZE_t      AAAMemSize;
    SIZE_t      AAAFreeMemSize;
    SIZE_t      AAAUsedMemSize;

} AAA_MEM_INFO;
/*For AAA memory dump(AE, AWB, ADJ info)*/
typedef struct
{
    UINT32              CurrIdx;
    AMBA_AE_INFO_s      AEInfoVideo[AAA_MEM_LOG_LEN];
    AMBA_AE_INFO_s      AEInfoStill[AAA_MEM_LOG_LEN];
    HDR_INFO_s          HDRInfo[AAA_MEM_LOG_LEN];
    AMBA_IK_WB_GAIN_s   AWBInfoVideo[AAA_MEM_LOG_LEN];
    AMBA_IK_WB_GAIN_s   AWBInfoStill[AAA_MEM_LOG_LEN];


} AAA_MEM_LOG;

typedef struct {
    void (* ModeCfg)(UINT16 ViewId, UINT8 Mode, AMBA_IK_MODE_CFG_s* pMode);
    void (* GetLiveViewInfo)(UINT16 ViewId, LIVEVIEW_INFO_s *pLiveViewInfo);
} APP_INFO_FUNC_s;

/*For Fault Injection Test */
typedef struct
{
    UINT8    FaultType;
    UINT8    Reserved[3];
    UINT32   Reserved1;
} AMBA_IMGPROC_FAULT;
#define IMGPROC_FAULT_00    0x00U
#define IMGPROC_FAULT_01    0x01U
#define IMGPROC_FAULT_02    0x02U
#define IMGPROC_FAULT_03    0x03U
#define IMGPROC_FAULT_04    0x04U
#define IMGPROC_FAULT_05    0x05U
#define IMGPROC_FAULT_06    0x06U
#define IMGPROC_FAULT_07    0x07U //OS
#define IMGPROC_FAULT_LAST  0x08U

UINT32 AmbaImgProc_FaultInjectionEnb(UINT8 Enable);
UINT32 AmbaImgProc_FaultInjection(const AMBA_IMGPROC_FAULT *pImgFault);
//[SDK7]
//[DATA INIT]
UINT32 AmbaImgProc_QueryMemorySize(UINT32 ViewIDCount, SIZE_t* pSize);
UINT32 AmbaImgProc_SetMemInfo(void *pMemAddr, SIZE_t MemSizeByte);
UINT32 AmbaImgProc_GetMemInfo(void **pMemAddr, SIZE_t *pMemSizeByte);
UINT32 AmbaImgProc_SetAAAMEMLog(UINT32 ViewID);
UINT32 AmbaImgProc_GetAAAMEMLog(UINT32 ViewID, AAA_MEM_LOG *pAAAMEMLog);
UINT32 AmbaImgProc_SetTotalViewCount(UINT32 ViewIDCount);
UINT32 AmbaImgProc_GetTotalViewCount(UINT32 *pViewIDCount);

UINT32 AmbaImgProc_ParamInit(UINT32 ViewIDCount);
UINT32 AmbaImgProc_SetMaxExpGroupNo(UINT32 MaxExpGroupNo);
UINT32 AmbaImgProc_GetMaxExpGroupNo(UINT32 *pMaxExpGroupNo);
UINT32 AmbaImgProc_SetRegFunc(UINT32 ViewID, const IMG_PROC_FUNC_s *pIpFunc);
UINT32 AmbaImgProc_GetRegFunc(UINT32 ViewID, IMG_PROC_FUNC_s *pIpFunc);
UINT32 AmbaImgProc_SetAppInfoFunc(UINT32 ViewID, const APP_INFO_FUNC_s *pAppInfoFunc);
UINT32 AmbaImgProc_GetAppInfoFunc(UINT32 ViewID, APP_INFO_FUNC_s *pAppInfoFunc);
UINT32 AmbaImgProc_SetStillIdx(UINT32 ViewID, const ADJ_STILL_IDX_INFO_s *pStillIdx);
UINT32 AmbaImgProc_GetStillIdx(UINT32 ViewID, ADJ_STILL_IDX_INFO_s *pStillIdx);
UINT32 AmbaImgProc_GetImgParam(UINT32 ViewID, IMG_PARAM_s *pImgParam);
UINT32 AmbaImgProc_SetImgParam(UINT32 ViewID, const IMG_PARAM_s *pImgParam);
UINT32 AmbaImgProc_SetAAAParam(UINT32 ViewID, const AAA_PARAM_s *pAAADefParam);
UINT32 AmbaImgProc_GetAAAParam(UINT32 ViewID,  AAA_PARAM_s *pAAADefParam);
UINT32 AmbaImgProc_ChkIqParamVerNum(UINT32 ParamType, UINT32 StructVer, UINT32 ParamVer);
UINT32 AmbaImgProc_SetCcTableAddr(UINT32 ViewID, UINT8 Mode, UINT8 tableNo, ULONG Addr);
UINT32 AmbaImgProc_GetCcTableAddr(UINT32 ViewID, UINT8 Mode, UINT8 tableNo, ULONG *pAddr);
UINT32 AmbaImgProc_SetCcRegAddr(UINT32 ViewID, ULONG Addr);
UINT32 AmbaImgProc_GetCcRegAddr(UINT32 ViewID, ULONG *pAddr);
UINT32 AmbaImgProc_SetDeParam(UINT32 ViewID, UINT32 Mode, const DE_PARAM_s *pDeParam);
UINT32 AmbaImgProc_GetDeParam(UINT32 ViewID, UINT32 Mode, DE_PARAM_s *pDeParam);

//[AE]
UINT32 AmbaImgProc_AEInit(UINT32 ViewID, UINT32 InitMode);
UINT32 AmbaImgProc_AEControl(UINT32 ViewID, AMBA_AAA_STATUS_s *pAAAVideoStatus, AMBA_AAA_STATUS_s *pAAAStillStatus);
UINT32 AmbaImgProc_AEPostStill(UINT32 ViewID, UINT8 Type);
UINT32 AmbaImgProc_AEResetFull(UINT32 ViewID);
UINT32 AmbaImgProc_AESetExpInfo(UINT32 ViewID, UINT32 ExpNo, UINT32 Mode, const AMBA_AE_INFO_s *pAEInfo);
UINT32 AmbaImgProc_AEGetExpInfo(UINT32 ViewID, UINT32 ExpNo, UINT32 Mode, AMBA_AE_INFO_s *pAEInfo);
UINT32 AmbaImgProc_AESetMInfo(UINT32 ViewID, UINT32 Mode, const AMBA_AE_INFO_s *pAEInfo);
UINT32 AmbaImgProc_AESetGDgain(UINT32 ViewID, UINT32 GDgain);
UINT32 AmbaImgProc_AEGetGDgain(UINT32 ViewID, UINT32 *pGDgain);
UINT32 AmbaImgProc_AESetEnvInfo (UINT32 ViewID, UINT32 Value);
UINT32 AmbaImgProc_AEGetEnvInfo (UINT32 ViewID, UINT32 * pValue);
UINT32 AmbaImgProc_AEGetCurrLvNo(UINT32 ViewID, UINT16 *pLvNo);
UINT32 AmbaImgProc_AEGetCurrLv(UINT32 ViewID, UINT16 *pLvNo);
UINT32 AmbaImgProc_AESetManualAgcShuIris(UINT32 ViewID, UINT32 ManualSetMode, const INT32 *pManualAgcShuIris);
UINT32 AmbaImgProc_AEGetManualAgcShuIris(UINT32 ViewID, UINT32 *pManualSetMode, INT32 *pManualAgcShuIris);
UINT32 AmbaImgProc_AESetIrisInfo(UINT32 ViewID, const IRIS_INFO_s *pIrisInfo);
UINT32 AmbaImgProc_AEGetIrisInfo(UINT32 ViewID, IRIS_INFO_s *pIrisInfo);
UINT32 AmbaImgProc_AEGetAEHisto(UINT32 ViewID, UINT16 * pAEHisto, const UINT32 *pHistoMode);
UINT32 AmbaImgProc_AEGetAEHistoSum (const UINT16 *pSrtEnd, const UINT16 *pAEHisto, UINT16 *pAEHistoSum);
UINT32 AmbaImgProc_AESetAEAlgoInfo(UINT32 ViewID, const AE_ALGO_INFO_s *pAEAlgoInfo);
UINT32 AmbaImgProc_AEGetAEAlgoInfo(UINT32 ViewID, AE_ALGO_INFO_s *pAEAlgoInfo);
UINT32 AmbaImgProc_AESetAEDefSetting(UINT32 ViewID, const AE_DEF_SETTING_s *pDefSetting);
UINT32 AmbaImgProc_AEGetAEDefSetting(UINT32 ViewID, AE_DEF_SETTING_s *pDefSetting);
UINT32 AmbaImgProc_AESetAEEvLut(UINT32 ViewID, const AE_EV_LUT_s *pEvLut);
UINT32 AmbaImgProc_AEGetAEEvLut(UINT32 ViewID, AE_EV_LUT_s *pEvLut);
UINT32 AmbaImgProc_AESetAEControlCap(UINT32 ViewID, const AE_CONTROL_s *pAEControlMode);
UINT32 AmbaImgProc_AEGetAEControlCap(UINT32 ViewID, AE_CONTROL_s *pAEControlMode);
UINT32 AmbaImgProc_AEGetExpToEvIndex(UINT32 ViewID, UINT32 Type, const AMBA_AE_INFO_s *pAEInfo, UINT16 *pEvIndex);
UINT32 AmbaImgProc_AEGetExpToNfIndex(UINT32 ViewID, UINT32 Type, const AMBA_AE_INFO_s *pAEInfo, UINT16 *pNfIndex);
UINT32 AmbaImgProc_AEGetExpToIsoValue(UINT32 ViewID, UINT32 Type, const AMBA_AE_INFO_s *pAEInfo, UINT16 *pIsoValue);
UINT32 AmbaImgProc_AESetAEBInfo(UINT32 ViewID, const AEB_INFO_s *pAEBInfo);
UINT32 AmbaImgProc_AEGetAEBInfo(UINT32 ViewID, AEB_INFO_s *pAEBInfo);
UINT32 AmbaImgProc_AEGetFlickerCMD(UINT32 ViewID, UINT32 *pEnable);
UINT32 AmbaImgProc_AESetFlickerCMD(UINT32 ViewID, UINT32 Enable);
UINT32 AmbaImgProc_AEGetCurrFlickerMode(UINT32 ViewID, UINT8 *pFlickerMode);
UINT32 AmbaImgProc_AESetCurrFlickerMode(UINT32 ViewID, UINT8 FlickerMode);
UINT32 AmbaImgProc_AESetBfoCeGain(UINT32 ViewID, UINT32 BfoCeGain);
UINT32 AmbaImgProc_AEGetBfoCeGain(UINT32 ViewID, UINT32 *BfoCeGain, UINT32 FramNum);
UINT32 AmbaImgProc_AEGetFlashTableNumber(UINT32 ViewID, UINT8 *pFlashTableNumber);

//[AWB]
UINT32 AmbaImgProc_AWBInit(UINT32 ViewID, UINT32 InitMode);
UINT32 AmbaImgProc_AWBControl(UINT32 ViewID, AMBA_AAA_STATUS_s *pAAAVideoStatus, AMBA_AAA_STATUS_s *pAAAStillStatus);
UINT32 AmbaImgProc_AWBPostStill(UINT32 ViewID, INT32 Speed);
UINT32 AmbaImgProc_AWBResetFull(UINT32 ViewID);
UINT32 AmbaImgProc_AWBSetWBGain(UINT32 ViewID, UINT32 ExpNo, UINT32 Mode, const AMBA_IK_WB_GAIN_s *pAWBGain);
UINT32 AmbaImgProc_AWBGetWBGain(UINT32 ViewID, UINT32 ExpNo, UINT32 Mode, AMBA_IK_WB_GAIN_s *pAWBGain);
UINT32 AmbaImgProc_AWBGetCurrWBLutNo(UINT32 ViewID, INT16 *pLutNo);
UINT32 AmbaImgProc_AWBSetAWBAlgoInfo(UINT32 ViewID, const AWB_ALGO_INFO_s *pAWBAlgoInfo);
UINT32 AmbaImgProc_AWBGetAWBAlgoInfo(UINT32 ViewID, AWB_ALGO_INFO_s *pAWBAlgoInfo);
UINT32 AmbaImgProc_AWBSetAWBCtrlCap(UINT32 ViewID, const AWB_CONTROL_s *pAWBControlMode);
UINT32 AmbaImgProc_AWBGetAWBCtrlCap(UINT32 ViewID, AWB_CONTROL_s *pAWBControlMode);
UINT32 AmbaImgProc_AWBSetWBCompSetInfo(UINT32 ViewID, const AMBA_IK_WB_GAIN_s *pWBSetLowRgb, const AMBA_IK_WB_GAIN_s *pWBSetHighRgb);
UINT32 AmbaImgProc_AWBGetWBCompSetInfo(UINT32 ViewID, AMBA_IK_WB_GAIN_s *pWBSetLowRgb, AMBA_IK_WB_GAIN_s *pWBSetHighRgb);
UINT32 AmbaImgProc_AWBSetWBCompTargetInfo(UINT32 ViewID, const AMBA_IK_WB_GAIN_s *pWBTargetLowRgb, const AMBA_IK_WB_GAIN_s *pWBTargetHighRgb);
UINT32 AmbaImgProc_AWBGetWBCompTargetInfo(UINT32 ViewID, AMBA_IK_WB_GAIN_s *pWBTargetLowRgb, AMBA_IK_WB_GAIN_s *pWBTargetHighRgb);
UINT32 AmbaImgProc_AWBGetFinAWBRatio(UINT32 ViewID, UINT32 *pFinAWBRatio);
UINT32 AmbaImgProc_AWBGetManualFinAWBRatio(UINT32 ViewID, UINT32 *pFinAWBRatio);
UINT32 AmbaImgProc_AWBSetManualFinAWBRatio(UINT32 ViewID, const UINT32 *pFinAWBRatio);
UINT32 AmbaImgProc_AWBSetWBPosition(UINT32 ViewID, UINT32 WBPosition);
UINT32 AmbaImgProc_AWBGetWBPosition(UINT32 ViewID, UINT32 *pWBPosition);
UINT32 AmbaImgProc_AWBGetIrGain(UINT32 ViewID, UINT32 *pIrGain);

//[ADJ]
UINT32 AmbaImgProc_ADJCeCtrl(UINT32 ViewID, const FLOAT HdrRatio, const INT32 FogStrength);
UINT32 AmbaImgProc_ADJVideoInit(UINT32 ViewID);
UINT32 AmbaImgProc_ADJVideoCtrl(UINT32 ViewID, ADJ_VIDEO_IQ_INFO_s *pADJVideoCtrl);
UINT32 AmbaImgProc_ADJAEAWBCtrl(UINT32 ViewID, const ADJ_VIDEO_IQ_INFO_s *pADJVideoIqInfo);
UINT32 AmbaImgProc_ADJStillCtrl(UINT32 ViewID, const ADJ_STILL_IQ_INFO_s *pADJStillIqInfo);
UINT32 AmbaImgProc_ADJChkStillIdx(UINT32 ViewID, AMBA_AE_INFO_s *pStillAEInfo);
UINT32 AmbaImgProc_ADJResetFull(UINT32 ViewID);
UINT32 AmbaImgProc_ADJGetVideoAddr(UINT32 ViewID, ULONG *pAddr);
UINT32 AmbaImgProc_ADJSetVideoParam(UINT32 ViewID, const AMBA_IK_MODE_CFG_s *pMode);
UINT32 AmbaImgProc_ADJResetVideoFlags(UINT32 ViewID, UINT8 Mode);
UINT32 AmbaImgProc_ADJResetStillFlags(UINT32 ViewID, UINT8 Mode);
UINT32 AmbaImgProc_ADJGetVideoUpdFlag(UINT32 ViewID, UINT16 *pUpdateFlag);
UINT32 AmbaImgProc_ADJGetStillAddr(UINT32 ViewID, ULONG *pTableAddr);
UINT32 AmbaImgProc_ADJSetStillParam(UINT32 ViewID, UINT32 AEBIndex, const AMBA_IK_MODE_CFG_s *pMode);
UINT32 AmbaImgProc_ADJSetGammaValue(UINT32 ViewID, const GAMMA_INFO_s *pGammaInfo);
UINT32 AmbaImgProc_ADJGetGammaValue(UINT32 ViewID, GAMMA_INFO_s *pGammaInfo);
UINT32 AmbaImgProc_ADJSetADJAEAWBCtrlInfo(UINT32 ViewID, const ADJ_AEAWB_CONTROL_s *pADJAEAWBCtrl);
UINT32 AmbaImgProc_ADJGetADJAEAWBCtrlInfo(UINT32 ViewID, ADJ_AEAWB_CONTROL_s *pADJAEAWBCtrl);
UINT32 AmbaImgProc_ADJGetAETarget(UINT32 ViewID, UINT16 *pADJAETarget);
UINT32 AmbaImgProc_ADJGetAutoKnee(UINT32 ViewID, UINT16 *pADJAutoKnee);
UINT32 AmbaImgProc_ADJGetLERatio(UINT32 ViewID, UINT16 *pADJLERatio);
UINT32 AmbaImgProc_ADJGetGammaRatio (UINT32 ViewID, UINT16 *pADJGammaRatio);
UINT32 AmbaImgProc_ADJSetMSpeed(UINT32 ViewID, INT32 MotionSpeed);
UINT32 AmbaImgProc_ADJGetMSpeed(UINT32 ViewID, INT32 *pMotionSpeed);
UINT32 AmbaImgProc_ADJSetMSpeedThresh(UINT32 ViewID, const ADJ_MOTION_SPEED_THRESHOLD_s *pMSThreshold);
UINT32 AmbaImgProc_ADJGetMSpeedThresh(UINT32 ViewID, ADJ_MOTION_SPEED_THRESHOLD_s *pMSThreshold);
UINT32 AmbaImgProc_ADJSetOBEnable(UINT32 ViewID, const AMBA_OB_MODE_INFO_s *pOBInfo,UINT8 Enable);
UINT32 AmbaImgProc_ADJGetOBEnable(UINT32 ViewID, const AMBA_OB_MODE_INFO_s *pOBInfo,UINT8 *pEnable);
UINT32 AmbaImgProc_ADJGetOBValue(UINT32 ViewID, const AMBA_OB_MODE_INFO_s *pInputOBInfo ,AMBA_BLACK_CORRECTION_s *pOutputOBvalue);
UINT32 AmbaImgProc_ADJSetOBValue(UINT32 ViewID, const AMBA_OB_MODE_INFO_s *pInputOBInfo, const AMBA_BLACK_CORRECTION_s* pInputOBvalue);
UINT32 AmbaImgProc_ADJCalOBLevel(const AMBA_IMG_RAW_INFO_s *pOBRawInfo,AMBA_BLACK_CORRECTION_s *pOBResult);

UINT32 AmbaImgProc_ADJSetBlackLevelBase(UINT32 ViewID, UINT32 Mode, const AMBA_BLACK_CORRECTION_s * pBlackLevelBase);
UINT32 AmbaImgProc_ADJGetBlackLevelBase(UINT32 ViewID, UINT32 Mode, AMBA_BLACK_CORRECTION_s * pBlackLevelBase);


//[Scene Mode]
UINT32 AmbaImgProc_SCGetSceneMode(UINT32 ViewID, UINT32 Mode, INT32 *pSceneMode);
UINT32 AmbaImgProc_SCSetSceneMode(UINT32 ViewID,UINT32 Mode, const INT32* pSceneMode);
UINT32 AmbaImgProc_SCGetSceneModeInfo(UINT32 ViewID, INT32 SceneMode, SCENE_DATA_s* pInfo);
UINT32 AmbaImgProc_SCSetSceneModeInfo(UINT32 ViewID, INT32 SceneMode, const SCENE_DATA_s* pInfo);


//[HDR]
UINT32 AmbaImgProc_HDRSetVideoMode(UINT32 ViewID, UINT32 Mode);
UINT32 AmbaImgProc_HDRGetVideoMode(UINT32 ViewID, UINT32 *pMode);
UINT32 AmbaImgProc_HDRSetLinearCEEnable(UINT32 ViewID, UINT32 Enable);
UINT32 AmbaImgProc_HDRGetLinearCEEnable(UINT32 ViewID, UINT32 *pEnable);
UINT32 AmbaImgProc_HDRVideoInit(UINT32 ViewID, HDR_INFO_s *pHDRInfo);
UINT32 AmbaImgProc_HDRVideoCtrl(UINT32 ViewID, HDR_INFO_s *pHDRInfo);
UINT32 AmbaImgProc_HDRSetBlendRatio(UINT32 ViewID, INT32 BlendRatio);
UINT32 AmbaImgProc_HDRGetBlendRatio(UINT32 ViewID, INT32 *pBlendRatio);
UINT32 AmbaImgProc_HDRVideoDynEnable(UINT32 ViewID, UINT32 Enable, UINT32 Millisecond);
UINT32 AmbaImgProc_AdjHdrSetLongRatio(UINT32 ViewID, DOUBLE value);
UINT32 AmbaImgProc_AdjHdrGetLongRatio(UINT32 ViewID, DOUBLE* pValue);
UINT32 AmbaImgProc_AdjHdrSetMaxBlendRatio(UINT32 ViewID, DOUBLE value);
UINT32 AmbaImgProc_AdjHdrGetMaxBlendRatio(UINT32 ViewID, DOUBLE* pValue);
UINT32 AmbaImgProc_AdjHdrSetSnrHdrDGain(UINT32 ViewID, UINT32 value);
UINT32 AmbaImgProc_AdjHdrGetSnrHdrDGain(UINT32 ViewID, UINT32* pValue);
UINT32 AmbaImgProc_AdjHdrSetShiftAeGain(UINT32 ViewID, DOUBLE value);
UINT32 AmbaImgProc_AdjHdrGetShiftAeGain(UINT32 ViewID, DOUBLE* pValue);
UINT32 AmbaImgProc_AdjHdrGetLongBaseRatio(UINT32 ViewID, DOUBLE* pValue);
//[HDR Still]
UINT32 AmbaImgProc_HDRStillInit(UINT32 ViewID, HDR_INFO_s *pHDRInfo); //*pHDRInfo should point to an array with length equals to MAX_AEB_NUM.
UINT32 AmbaImgProc_HDRStillCtrl(UINT32 ViewID, HDR_INFO_s *pHDRInfo); //*pHDRInfo should point to an array with length equals to MAX_AEB_NUM.
UINT32 AmbaImgProc_AdjHdrSetStillLongRatio(UINT32 ViewID, UINT32 AEBIndex, DOUBLE Value);
UINT32 AmbaImgProc_AdjHdrGetStillLongRatio(UINT32 ViewID, UINT32 AEBIndex, DOUBLE* pValue);
UINT32 AmbaImgProc_AdjHdrSetStillShiftAeGain(UINT32 ViewID, UINT32 AEBIndex, DOUBLE Value);
UINT32 AmbaImgProc_AdjHdrGetStillShiftAeGain(UINT32 ViewID, UINT32 AEBIndex, DOUBLE* pValue);




//[UTILITY]
UINT32 AmbaImgProc_SetCFAVinHisto(UINT32 ViewID, UINT32 ExpNo, const AMBA_IK_CFA_HIST_STAT_s *pCFAStat);
UINT32 AmbaImgProc_GetCFAVinHisto(UINT32 ViewID, UINT32 ExpNo, AMBA_IK_CFA_HIST_STAT_s *pCFAStat);
UINT32 AmbaImgProc_SetCFAAAAStat(UINT32 ViewID, const AMBA_IK_CFA_3A_DATA_s *pCFAStat);
UINT32 AmbaImgProc_GetCFAAAAStat(UINT32 ViewID, AMBA_IK_CFA_3A_DATA_s *pCFAStat);
UINT32 AmbaImgProc_SetPGAAAStat(UINT32 ViewID, const AMBA_IK_PG_3A_DATA_s *pRgbStat);
UINT32 AmbaImgProc_GetPGAAAStat(UINT32 ViewID, AMBA_IK_PG_3A_DATA_s *pRgbStat);
UINT32 AmbaImgProc_SetHDRHisto(UINT32 ViewID, const AMBA_IK_CFA_HISTOGRAM_s *pHDRHisto, UINT32 ExpNo);
UINT32 AmbaImgProc_GetHDRHisto(UINT32 ViewID, AMBA_IK_CFA_HISTOGRAM_s *pHDRHisto, UINT32 ExpNo);
UINT32 AmbaImgProc_GetAETileInfo(UINT32 ViewID, UINT32 Mode, AMBA_AE_TILES_INFO_s * pAETilesInfo);
UINT32 AmbaImgProc_GetAWBTileInfo(UINT32 ViewID, UINT32 Mode, AMBA_AWB_TILES_INFO_s*pAWBTilesInfo);
UINT32 AmbaImgProc_SetAAAOPInfo(UINT32 ViewID, const AMBA_AAA_OP_INFO_s *pAAAOpInfo);
UINT32 AmbaImgProc_GetAAAOPInfo(UINT32 ViewID, AMBA_AAA_OP_INFO_s *pAAAOpInfo);
UINT32 AmbaImgProc_SetAAAStatus(UINT32 ViewID, const AMBA_AAA_STATUS_s* pAAAVideoStatus, const AMBA_AAA_STATUS_s* pAAAStillStatus);
UINT32 AmbaImgProc_GetAAAStatus(UINT32 ViewID, AMBA_AAA_STATUS_s* pAAAVideoStatus, AMBA_AAA_STATUS_s* pAAAStillStatus);
UINT32 AmbaImgProc_SetAEStatus(UINT32 ViewID, UINT8 VideoAeStatus, UINT8 StillAeStatus);
UINT32 AmbaImgProc_SetAWBStatus(UINT32 ViewID, UINT8 VideoAwbStatus, UINT8 StillAwbStatus);
UINT32 AmbaImgProc_SetAFStatus(UINT32 ViewID, UINT8 VideoAfStatus, UINT8 StillAfStatus);
UINT32 AmbaImgProc_GetAAALibVersion(AMBA_IP_VER *pIPVer);
UINT32 AmbaImgProc_GetAAAVerNum(UINT32 ViewID,UINT32 *pStructVersion, UINT32 *pParamVersion);
UINT32 AmbaImgProc_GetCapFormat(UINT32 ViewID, UINT16 *pCapFormat);
UINT32 AmbaImgProc_SetCapFormat(UINT32 ViewID, UINT16 CapFormat);
UINT32 AmbaImgProc_GetContiShotCount(UINT32 ViewID, UINT32 *pShotCount);
UINT32 AmbaImgProc_SetContiShotCount(UINT32 ViewID, UINT32 ShotCount);
UINT32 AmbaImgProc_GetPipelineMode(UINT32 ViewID, UINT16 *pPipeMode);
UINT32 AmbaImgProc_SetPipelineMode(UINT32 ViewID, UINT16 PipeMode);
UINT32 AmbaImgProc_GetMode(UINT32 ViewID, UINT32 *pCurrMode, UINT32 *pNextMode);
UINT32 AmbaImgProc_SetMode(UINT32 ViewID, const UINT32 *pCurrMode, const UINT32 *pNextMode);
UINT32 AmbaImgProc_GetPhotoPreview(UINT32 ViewID, UINT32 *pPhotoPreview);
UINT32 AmbaImgProc_SetPhotoPreview(UINT32 ViewID, const UINT32 *pPhotoPreview);
UINT32 AmbaImgProc_SetMEMWBCommand (UINT32 ViewID, UINT32 Enable);
UINT32 AmbaImgProc_GetMEMWBCommand (UINT32 ViewID, UINT32* pEnable);
UINT32 AmbaImgProc_GetFrameRate(UINT32 ViewID, UINT32 *pFpsNumerator, UINT32 *pFpsDenominator);
UINT32 AmbaImgProc_SetFrameRate(UINT32 ViewID, UINT32 FpsNumerator, UINT32 FpsDenominator);
UINT32 AmbaImgProc_GetCurrFrameRate(UINT32 ViewID, UINT32 *pFpsNumerator, UINT32 *pFpsDenominator);
UINT32 AmbaImgProc_SetCurrFrameRate(UINT32 ViewID, UINT32 FpsNumerator, UINT32 FpsDenominator);
UINT32 AmbaImgProc_GetOfflineAAAInfo(UINT32 ViewID, AMBA_IMGPROC_OFFLINE_AAA_INFO_s *pOfflineAAAInfo);
UINT32 AmbaImgProc_SetOfflineAAAInfo(UINT32 ViewID, AMBA_IMGPROC_OFFLINE_AAA_INFO_s OfflineAAAInfo);
UINT32 AmbaImgProc_OfflineHdrVideoControl(UINT32 viewID, DOUBLE blendRatioCur, UINT32 blendRatioMax, UINT32 blendRatioMin);
UINT32 AmbaImgProc_GetIdspDgInfo(UINT32 ViewID, const AMBA_AE_INFO_s *pAEInfo, AMBA_DGAIN_INFO_s  *pDGInfo); //SVC
UINT32 AmbaImgProc_GetStillIdspDgInfo(UINT32 ViewID, UINT32 AEBIndex, const AMBA_AE_INFO_s *pAEInfo, AMBA_DGAIN_INFO_s  *pDGInfo);  //SVC
UINT32 AmbaImgProc_GetIdspWbInfo(UINT32 ViewID, const AMBA_AE_INFO_s *pAEInfo, const AMBA_IK_WB_GAIN_s *pWBGain, WB_CTRL *pWDCtrl); //SSP
UINT32 AmbaImgProc_Get_AQPInfo(UINT32 ViewID, ADJ_AQP_INFO_s *pAdjAQPInfo);
UINT32 AmbaImgProc_SetMultiCurrSceneMode(UINT32 ViewID,UINT8 Mode, const INT32 *pSceneMode);
UINT32 AmbaImgProc_GetMultiCurrSceneMode(UINT32 ViewID,UINT8 Mode, INT32 *pSceneMode);

UINT32 AmbaImgProc_SetUsrCfaAeStat(UINT32 ViewID, const AMBA_IP_USR_AE_s *pUsrAeStat);
UINT32 AmbaImgProc_GetUsrCfaAeStat(UINT32 ViewID, AMBA_IP_USR_AE_s *pUsrAeStat);
UINT32 AmbaImgProc_SetUsrHisto(UINT32 ViewID, const AMBA_IP_USR_HISTO_s *pUsrHisto);
UINT32 AmbaImgProc_GetUsrHisto(UINT32 ViewID, AMBA_IP_USR_HISTO_s *pUsrHisto);

UINT32 AmbaImgProc_GetOfflinePivAAAInfo(UINT32 ViewID, AMBA_IMGPROC_OFFLINE_AAA_INFO_s *pOfflineAAAInfo);
UINT32 AmbaImgProc_SetOfflinePivAAAInfo(UINT32 ViewID, const AMBA_IMGPROC_OFFLINE_AAA_INFO_s *pOfflineAAAInfo);
UINT32 AmbaImgProc_OfflineHdrPivControl(UINT32 viewID, DOUBLE blendRatioCur, UINT32 blendRatioMax, UINT32 blendRatioMin);


//[Data Sharing]
UINT32 AmbaImgProc_DSSetMode(UINT32 ViewID, const UINT32 DSMode, const UINT32 Type);
UINT32 AmbaImgProc_DSGetMode(UINT32 ViewID, UINT32 *pDSMode, UINT32 *pType);
UINT32 AmbaImgProc_DSSetOutgoingStat(UINT32 ViewID, const AMBA_IP_AAA_DATA_s *pOutgoingStat);
UINT32 AmbaImgProc_DSGetOutgoingStat(UINT32 ViewID, AMBA_IP_AAA_DATA_s *pOutgoingStat);
UINT32 AmbaImgProc_DSSetIncomingStat(UINT32 ViewID, const AMBA_IP_AAA_DATA_s * pIncomingStat, UINT32 ChID);
UINT32 AmbaImgProc_DSGetIncomingStat(UINT32 ViewID, AMBA_IP_AAA_DATA_s * pIncomingStat, UINT32 ChID);
UINT32 AmbaImgProc_DSSetOutgoingResult(UINT32 ViewID, const AMBA_IP_AAA_RESULTS_s *pOutgoingResult);
UINT32 AmbaImgProc_DSGetOutgoingResult(UINT32 ViewID, AMBA_IP_AAA_RESULTS_s *pOutgoingResult);
UINT32 AmbaImgProc_DSComputeAWB(UINT32 ViewID, AMBA_IP_CFA_AWB_s *pFinalAWB);
UINT32 AmbaImgProc_DSComputeFinalHisto(UINT32 ViewID, UINT16 *pFinalHisto, const UINT16 *pVinFinalHisto, const UINT16 *pPgFinalHisto);
UINT32 AmbaImgProc_DSSetFinalHisto(UINT32 ViewID, const UINT16 *pFinalHisto, const UINT16 *pVinFinalHisto, const UINT16 *pPgFinalHisto);
UINT32 AmbaImgProc_DSGetFinalHisto(UINT32 ViewID, UINT16 *pFinalHisto, const UINT16 *pVinFinalHisto, const UINT16 *pPgFinalHisto);
UINT32 AmbaImgProc_DSComputeAEROIMap(UINT32 ViewID);
UINT32 AmbaImgProc_DSSetAEROI(UINT32 ViewID, UINT32 AEROINo, const DS_AE_ROI_INFO_s *pROIInfo);
UINT32 AmbaImgProc_DSGetAEROI(UINT32 ViewID, UINT32 AEROINo, DS_AE_ROI_INFO_s *pROIInfo);
UINT32 AmbaImgProc_DSSetADJMasterChNo(UINT32 ViewID, UINT32 ADJMasterChNo);
UINT32 AmbaImgProc_DSGetADJMasterChNo(UINT32 ViewID, UINT32 *pADJMasterChNo);
//[MVIN]
UINT32 AmbaImgProc_MVINAEControlDGain(UINT32 ViewID, UINT16 Mode);
UINT32 AmbaImgProc_MVINSetAEDgain(UINT32 ViewID, const DOUBLE * pMVINAEDgain, UINT8 UpdateFlg);
UINT32 AmbaImgProc_MVINGetAEDgain(UINT32 ViewID, DOUBLE * pMVINAEDgain, UINT8 *pUpdateFlg);
UINT32 AmbaImgProc_MVINSetBLDgainY(UINT32 ViewID, const DOUBLE * pMVINBLDgainY, UINT8 UpdateFlg);
UINT32 AmbaImgProc_MVINGetBLDgainY(UINT32 ViewID, DOUBLE * pMVINBLDgainY, UINT8 *pUpdateFlg);
UINT32 AmbaImgProc_MVINSetBLDgainRGB(UINT32 ViewID, const AMBA_MVIN_BL_RGB_GAIN_s * pMVINBLDgainRGB);
UINT32 AmbaImgProc_MVINGetBLDgainRGB(UINT32 ViewID, AMBA_MVIN_BL_RGB_GAIN_s * pMVINBLDgainRGB);

UINT32 AmbaImgProc_SetStitchingIdxToViewID(const INT32 *pStitchIdxToViewID);
UINT32 AmbaImgProc_GetStitchingIdxToViewID(INT32 *pStitchIdxToViewID);
UINT32 AmbaImgProc_MVINSetRunChk(const UINT8 *vinViewIDRunChk);
UINT32 AmbaImgProc_MVINGetRunChk(UINT8 *vinViewIDRunChk);
UINT32 AmbaImgProc_MVINSetVinViewChk(const UINT8 *vinViewIDViewChk);
UINT32 AmbaImgProc_MVINGetVinViewChk(UINT8 *vinViewIDViewChk);

UINT32 AmbaImgProc_MVINSetRgbstat(UINT32 idx,const AMBA_DISP_ROI_RGB_STAT_s *pRgbStat);
UINT32 AmbaImgProc_MVINGetRgbstat(UINT32 idx, AMBA_DISP_ROI_RGB_STAT_s *pRgbStat);
UINT32 AmbaImgProc_MVINSetState(UINT8 State);
UINT32 AmbaImgProc_MVINGetState(UINT8 *pState);
UINT32 AmbaImgProc_MVINSetPos(UINT32 idx,const AMBA_VIG_CENTER_ROI_s *pVigPos);
UINT32 AmbaImgProc_MVINGetPos(UINT32 idx, AMBA_VIG_CENTER_ROI_s *pVigPos);
UINT32 AmbaImgProc_MVINSetGainFactor(UINT32 idx,const AMBA_VIG_GAIN_FACTOR_s *pVigGainFactor);
UINT32 AmbaImgProc_MVINGetGainFactor(UINT32 idx, AMBA_VIG_GAIN_FACTOR_s *pVigGainFactor);
UINT32 AmbaImgProc_MVINFunc(const AMBA_RUNTIMEVIG_FUNC_s * pRunTimeVigFunc);
UINT32 AmbaImgProc_UpdateVignette(AMBA_VIG_GAIN_FACTOR_s **vigInfo, UINT8 fovUpdateflag);


//[Digital Effect]
UINT32 AmbaImgProc_DESetDEffect(UINT32 ViewID, const AMBA_IK_MODE_CFG_s *pModeCfg, UINT8 Effect);
UINT32 AmbaImgProc_DEGetDEffect(UINT32 ViewID, UINT8 *pEffect);
UINT32 AmbaImgProc_DESetBrightness(UINT32 ViewID, const AMBA_IK_MODE_CFG_s *pModeCfg, INT16 Brightness);
UINT32 AmbaImgProc_DEGetBrightness(UINT32 ViewID, INT16 *pBrightness);
UINT32 AmbaImgProc_DESetSaturation(UINT32 ViewID, const AMBA_IK_MODE_CFG_s *pModeCfg, UINT16 Saturation);
UINT32 AmbaImgProc_DEGetSaturation(UINT32 ViewID, UINT16 *pSaturation);
UINT32 AmbaImgProc_DESetHue(UINT32 ViewID, const AMBA_IK_MODE_CFG_s *pModeCfg, INT16 Hue);
UINT32 AmbaImgProc_DEGetHue(UINT32 ViewID, INT16 *pHue);


UINT32 AmbaImgProc_GAAASetStat(UINT32 ViewID, const AMBA_IP_GLOBAL_AAA_DATA_s *pGAAAData);
UINT32 AmbaImgProc_GAAAGetStat(UINT32 ViewID, AMBA_IP_GLOBAL_AAA_DATA_s **pGAAAData);
UINT32 AmbaImgProc_GAAASetEnb(UINT32 ViewID, UINT8 Enable);
UINT32 AmbaImgProc_GAAAGetEnb(UINT32 ViewID, UINT8 *pEnable);
UINT32 AmbaImgProc_GAAASetWeighting(UINT32 ViewID, const INT32 *pGAAAWeighting);
UINT32 AmbaImgProc_GAAAGetWeighting(UINT32 ViewID, INT32 *pGAAAWeighting);
UINT32 AmbaImgProc_GAAASetStatCount(UINT32 ViewID, UINT32 statCount);
UINT32 AmbaImgProc_GAAAGetStatCount(UINT32 ViewID, UINT32 *pStatCount);


#endif