/*
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

#include "AmbaFS.h"
#include "ArmErrCode.h"
#include "AmbaTypes.h"
#include "SvcCvAlgo.h"
#include "cvapi_flexidag.h"
#include "AmbaCtfw_MvacStixelFlow.h"
#include "AmbaMisraFix.h"
#include "cvapi_flexidag_ambaspufex_def_cv2.h"

#if defined(CONFIG_QNX)
#include "cvapi_flexidag_unittest.h"
#define SD_ROOT_PATH "/sd0"
#elif defined(CONFIG_LINUX)
#define SD_ROOT_PATH "/tmp/SD0"
#else // threadX and the others
#include "cvapi_flexidag_unittest.h"
#define SD_ROOT_PATH "c:"
#endif

#define UT_NAME "AmbaAcStixel_UT"

static const char *gInputPath = NULL;

#define MaxFileNameLen (128U)

#define UT_CFG_MVAC_PROC_SCALE_ID          (2U)
#define UT_CFG_MVAC_VO_PROC_WIDTH          (1920U)
#define UT_CFG_MVAC_VO_PROC_HEIGHT         (960U)

#define SCALE_0_IMG_WIDTH           (3840U)
#define SCALE_0_IMG_HEIGHT          (1920U)

#define IN_SCALE_2_IMG_ROI_WIDTH    (1920U)
#define IN_SCALE_2_IMG_ROI_HEIGHT   (960U)

// Stixel configuration
//---------------------------------------------
#define UT_CFG_FOCAL_LENGTH_U              (1308.845298)
#define UT_CFG_FOCAL_LENGTH_V              (1308.845298)
#define UT_CFG_OPTICAL_CENTER_X            (998.2284065)
#define UT_CFG_OPTICAL_CENTER_Y            (479.614282)
#define UT_CFG_BASELINE                    (0.3003)

#define UT_CFG_DEFAULT_CAMERA_HEIGH        (1.545852)
#define UT_CFG_DEFAULT_CAMERA_PITCH        (0.155035)

#define UT_CFG_ROAD_SEARCH_PITCH_STRIDE    (0.25)
#define UT_CFG_ROAD_SEARCH_HEIGHT_STRIDE   (0.02)
#define UT_CFG_ROAD_SEARCH_PITCH_NUM       (64U)
#define UT_CFG_ROAD_SEARCH_HEIGHT_NUM      (10U)

#define UT_CFG_VDISP_X_THR                   (0.2)
#define UT_CFG_VDISP_Y_THR                   (0.15)

#define UT_CFG_DET_OBJECT_HEIGHT           (1.0)
#define UT_CFG_MIN_DISPARITY               (6U)
//---------------------------------------------


#define UT_STIXEL_OUT_PATH SD_ROOT_PATH"/CTFW_UT/out/ac_stixel_stixel"
#define UT_AC_OUT_PATH SD_ROOT_PATH"/CTFW_UT/out/ac_stixel_ac"

static UINT32 gLastSentFrameIdx = 0U;
static UINT32 gStixelRcvLastFrame = 0U;
static UINT32 gMvacRcvLastFrame = 0U;

static void GetFileSize(const char* path, UINT32* Size)
{
    UINT32 Ret;
    UINT32 Fsize = 0U;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        ArmLog_ERR(UT_NAME, "GetFileSize AmbaFS_FileOpen fail ", 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "GetFileSize AmbaFS_FileSeek fail ", 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "GetFileSize AmbaFS_FileTell fail ", 0U, 0U);
        } else {
            Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        }
    }

    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "GetFileSize AmbaFS_FileClose fail ", 0U, 0U);
        }
    }
    *Size = Fsize;
}

static UINT32 LoadFlexiDagBin(const char* path, flexidag_memblk_t* pMemBlk)
{
    UINT32 BinSize;
    UINT32 RetVal = 0U;

    GetFileSize(path, &BinSize);

    if (BinSize != 0U) {
        RetVal = AmbaCV_UtilityCmaMemAlloc(BinSize, 1, pMemBlk);

        if (RetVal != 0U) {
            ArmLog_ERR(UT_NAME, "RefCV_MemblkAlloc: OOM", 0U, 0U);
        }

        RetVal = AmbaCV_UtilityFileLoad(path, pMemBlk);
        if (RetVal != 0U) {
            ArmLog_ERR(UT_NAME, "LoadFlexiDagBin: AmbaCV_UtilityFileLoad failed:%x", RetVal, 0U);
        }
    } else {
        ArmLog_ERR(UT_NAME, "LoadFlexiDagBin: Open flexibin failed", 0U, 0U);
        RetVal = 1U;
    }
    return RetVal;
}

static void RefCV_GetMvacResult(const CtfwFrameMetadata_t *pMetadata, const void *pResult)
{
    UINT32 Rval = ARM_OK;
    AMBA_CV_MVAC_DATA_s *pAcResult;
    char Filename[MaxFileNameLen];

    if ( pMetadata->ErrorState != 0U ) {
        ArmLog_STR(UT_NAME, "## %s, get error", __func__, 0U);
    } else {
        AmbaMisra_TypeCast(&pAcResult, &(pResult));

        switch (pAcResult->State) {
        case AMBA_CV_MVAC_STATE_RESET:
            ArmLog_DBG(UT_NAME, "## state: RESET", 0U, 0U);
            break;
        case AMBA_CV_MVAC_STATE_WAIT:
            ArmLog_DBG(UT_NAME, "## state: WAIT", 0U, 0U);
            break;
        case AMBA_CV_MVAC_STATE_ACCUMULATION:
            ArmLog_DBG(UT_NAME, "## state: ACCUMULATION", 0U, 0U);
            break;
        case AMBA_CV_MVAC_STATE_SOLVE:
            ArmLog_DBG(UT_NAME, "## state: SOLVE", 0U, 0U);
            if (pAcResult->pWarpInfo != NULL) {
                UINT32 WriteSize;
                if (pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTableValid == 1U) {
                    Rval |= AmbaWrap_memset(Filename, 0, sizeof(Filename));
                    AmbaUtility_StringAppend(Filename, (UINT32)sizeof(Filename), UT_AC_OUT_PATH"/warp_left.bin");
                    ArmLog_DBG(UT_NAME, "[ut] got warp table", 0U, 0U);
                    Rval |= ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                        (UINT32)sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                        Filename, &WriteSize);
                    //AmbaCtfw_MvacProfilerReport();
                }
                if (pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalibValid == 1U) {
                    Rval |= AmbaWrap_memset(Filename, 0, sizeof(Filename));
                    AmbaUtility_StringAppend(Filename, (UINT32)sizeof(Filename), UT_AC_OUT_PATH"/calib_left.bin");
                    ArmLog_DBG(UT_NAME, "[ut] got calib info", 0U, 0U);
                    Rval |= ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                        (UINT32)sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                        Filename, &WriteSize);
                }


            }
            break;
        case AMBA_CV_MVAC_STATE_END:
            ArmLog_DBG(UT_NAME, "## state: END", 0U, 0U);
            if (pAcResult->pWarpInfo != NULL) {
                UINT32 WriteSize;
                if (pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTableValid == 1U) {
                    Rval |= AmbaWrap_memset(Filename, 0, sizeof(Filename));
                    AmbaUtility_StringAppend(Filename, (UINT32)sizeof(Filename), UT_AC_OUT_PATH"/warp_left.bin");
                    ArmLog_DBG(UT_NAME, "[ut] got warp table", 0U, 0U);
                    Rval |= ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                        (UINT32)sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                        Filename, &WriteSize);
                    //AmbaCtfw_MvacProfilerReport();
                }
                if (pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalibValid == 1U) {
                    Rval |= AmbaWrap_memset(Filename, 0, sizeof(Filename));
                    AmbaUtility_StringAppend(Filename, (UINT32)sizeof(Filename), UT_AC_OUT_PATH"/calib_left.bin");
                    ArmLog_DBG(UT_NAME, "[ut] got calib info", 0U, 0U);
                    Rval |= ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                        (UINT32)sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                        Filename, &WriteSize);
                }


            }
            break;
        case AMBA_CV_MVAC_STATE_SKIP:
            ArmLog_DBG(UT_NAME, "## state: SKIP", 0U, 0U);

            if (pAcResult->pWarpInfo != NULL) {
                UINT32 WriteSize;
                if (pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTableValid == 1U) {
                    Rval |= AmbaWrap_memset(Filename, 0, sizeof(Filename));
                    AmbaUtility_StringAppend(Filename, (UINT32)sizeof(Filename), UT_AC_OUT_PATH"/warp_left.bin");
                    ArmLog_DBG(UT_NAME, "[ut] got warp table", 0U, 0U);
                    Rval |= ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                        (UINT32)sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                        Filename, &WriteSize);
                    //AmbaCtfw_MvacProfilerReport();
                }
                if (pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalibValid == 1U) {
                    Rval |= AmbaWrap_memset(Filename, 0, sizeof(Filename));
                    AmbaUtility_StringAppend(Filename, (UINT32)sizeof(Filename), UT_AC_OUT_PATH"/calib_left.bin");
                    ArmLog_DBG(UT_NAME, "[ut] got calib info", 0U, 0U);
                    Rval |= ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                        (UINT32)sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                        Filename, &WriteSize);
                }


            }
            break;
        default:
            ArmLog_ERR(UT_NAME, "## callback at state: ERROR", 0U, 0U);
            break;
        }
        AmbaMisra_TouchUnused(&Rval);
    }

    if ( pMetadata->FrameIndex == gLastSentFrameIdx ) {
        gMvacRcvLastFrame = 1U;
    }

    return;
}

static void RefCV_GetStixelResult(const CtfwFrameMetadata_t *pMetadata, const void *pOutData)
{
    const char *pOutFileFormat = UT_STIXEL_OUT_PATH "/STIXEL_OUT_%06d.bin";
    char OutFilePath[128U];
    CTFW_STIXEL_TASK_OUT_t *pStixelTskOut;
    UINT32 RetVal = ARM_OK;
    UINT32 SaveFrameNums = 6U;

    if ( pMetadata->ErrorState != 0U ) {
        ArmLog_ERR(UT_NAME, "## %s, get error", __func__, 0U);
    } else {

        AmbaMisra_TypeCast(&pStixelTskOut, &pOutData);

        {
            UINT32 StixelNumM1;
            const AMBA_CV_STIXEL_s *pStixel;

            ArmLog_DBG(UT_NAME, "## Get Stixel result, FrameIdx(%06d)", pMetadata->FrameIndex, 0U);

            pStixel = &pStixelTskOut->StixelInfo.StixelList[0U];
            ArmLog_DBG(UT_NAME, "1st stixel (%d, %d)", pStixel->Top, pStixel->Bottom);

            StixelNumM1 = pStixelTskOut->StixelInfo.StixelNum - 1U;
            pStixel = &pStixelTskOut->StixelInfo.StixelList[StixelNumM1];
            ArmLog_DBG(UT_NAME, "last stixel (%d, %d)", pStixel->Top, pStixel->Bottom);
        }

        if (pMetadata->FrameIndex < SaveFrameNums) {
            UINT32 WriteSize;
            UINT32 RetStrLen;
            RetStrLen = AmbaUtility_StringPrintUInt32(OutFilePath, (UINT32)sizeof(OutFilePath),
                        pOutFileFormat, 1U, &pMetadata->FrameIndex);
            RetVal |= ArmFIO_Save(&pStixelTskOut->StixelInfo, (UINT32)sizeof(AMBA_CV_STIXEL_DET_OUT_s),
                                  OutFilePath, &WriteSize);
            (void) RetStrLen;
        }
    }

    if ( pMetadata->FrameIndex == gLastSentFrameIdx ) {
        gStixelRcvLastFrame = 1U;
    }

    (void) RetVal;

    return;
}

static void ConfigPicInfoHeader(memio_source_recv_picinfo_t *pPicInfo, UINT32 Scale0Width, UINT32 Scale0Pitch, UINT32 Scale0Height)
{
    UINT32 Idx;
    UINT32 Rval = ARM_OK;

    Rval |= AmbaWrap_memset(pPicInfo, 0, sizeof(memio_source_recv_picinfo_t));

    pPicInfo->pic_info.pyramid.image_width_m1 = (UINT16) (Scale0Width - 1U);
    pPicInfo->pic_info.pyramid.image_height_m1 = (UINT16) (Scale0Height - 1U);
    pPicInfo->pic_info.pyramid.image_pitch_m1 = (Scale0Pitch - 1U);

    for (Idx = 0U; Idx < MAX_HALF_OCTAVES; Idx++) {
        pPicInfo->pic_info.pyramid.half_octave[Idx].ctrl.disable = 1U;
    }

    (void) Rval;

    return;
}

static void ConfigPicInfoScale(memio_source_recv_picinfo_t *pPicInfo, UINT32 ScaleId, const roi_t *pRoi,
                               UINT32 RoiPitch, char *pLumaLeftAddr, char *pLumaRightAddr)
{
    ULONG PicInfoAddr;
    ULONG LumaAddr;
    UINT32 LumaLeftOffset;
    UINT32 LumaRightOffset;
    ULONG DiffAddr;

    AmbaMisra_TypeCast(&PicInfoAddr, &pPicInfo);

    AmbaMisra_TypeCast(&LumaAddr, &pLumaLeftAddr);
    DiffAddr = LumaAddr - PicInfoAddr;
    LumaLeftOffset =  (UINT32)DiffAddr;

    AmbaMisra_TypeCast(&LumaAddr, &pLumaRightAddr);
    DiffAddr = LumaAddr - PicInfoAddr;
    LumaRightOffset =  (UINT32)DiffAddr;

    pPicInfo->pic_info.pyramid.half_octave[ScaleId].ctrl.mode = 1U;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].ctrl.octave_mode = 0U;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].ctrl.disable = 0U;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].ctrl.roi_pitch = (UINT16) RoiPitch;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].roi_start_row = (INT16) pRoi->m_start_col;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].roi_start_col = (INT16) pRoi->m_start_row;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].roi_height_m1 = pRoi->m_height_m1;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].roi_width_m1 = pRoi->m_width_m1;

    pPicInfo->pic_info.rpLumaLeft[ScaleId] = LumaLeftOffset;
    pPicInfo->pic_info.rpLumaRight[ScaleId] = LumaRightOffset;

    return;
}

static UINT32 RefCV_LoadCalibData(const CtfwCalibInfo_t *pCalibInfo)
{
    UINT32 Rval = ARM_OK;
    char Filename[MaxFileNameLen];
    UINT32 Length, ActualLoadSize;
    const char *pStrRet;

    // read metadata
    {
        pStrRet = ArmStdC_strcpy(Filename, MaxFileNameLen, gInputPath);
        ArmStdC_strcat(Filename, MaxFileNameLen, "/metadata/meta_fov0.bin");
        ArmLog_STR(UT_NAME, "## filename: %s", Filename, NULL);
        Rval |= ArmFIO_GetSize(Filename, &Length);
    }
    if (Rval == ARM_OK) {
        Rval = ArmFIO_Load(pCalibInfo->Left.pBuffer, Length, Filename, &ActualLoadSize);
        //ArmLog_U64(UT_NAME, "## metadata left, pBuffer=%llx", (UINT64)pCalibInfo->Left.pBuffer, (UINT64)0U);
    }

    if (Rval == ARM_OK) {
        pStrRet = ArmStdC_strcpy(Filename, MaxFileNameLen, gInputPath);
        ArmStdC_strcat(Filename, MaxFileNameLen, "/metadata/meta_fov1.bin");
        ArmLog_STR(UT_NAME, "## filename: %s", Filename, NULL);
        Rval |= ArmFIO_GetSize(Filename, &Length);
    }
    if (Rval == ARM_OK) {
        Rval = ArmFIO_Load(pCalibInfo->Right.pBuffer, Length, Filename, &ActualLoadSize);
        //ArmLog_U64(UT_NAME, "## metadata right, pBuffer=%llx", (UINT64)pCalibInfo->Right.pBuffer, (UINT64)0U);
    }

    (void) pStrRet;

    return Rval;
}

static UINT32 RefCV_LoadWarpTable(AMBA_CV_MVAC_WARP_TABLE_INFO_s *pWarpTableInfo)
{
    UINT32 Rval = ARM_OK;
    char Filename[MaxFileNameLen];
    UINT32 Length, ActualLoadSize;
    const char *pStrRet;

    {
        pStrRet = ArmStdC_strcpy(Filename, MaxFileNameLen, gInputPath);
        ArmStdC_strcat(Filename, MaxFileNameLen, "/metadata/warp_info.bin");
        ArmLog_STR(UT_NAME, "## filename: %s", Filename, NULL);
        Rval |= ArmFIO_GetSize(Filename, &Length);
    }

    if (Rval == ARM_OK) {
        Rval = ArmFIO_Load(pWarpTableInfo, Length, Filename, &ActualLoadSize);
    }

    (void) pStrRet;

    return Rval;
}

static UINT32 RefCV_LoadMvacInputLuma(UINT32 FrameCnt, flexidag_memblk_t *pLumaLeft,
                                      flexidag_memblk_t *pLumaRight, UINT32 ScaleId)
{
    UINT32 Rval = ARM_OK;
    char Filename[MaxFileNameLen];
    char FileIndex[MaxFileNameLen] = {0};
    UINT32 Length = 0U;
    UINT32 ActualLoadSize;
    flexidag_memblk_t *pOutBufDesc;
    const char *pStrRet;

    // read input image
    //load left image
    //generate filename
    {
        UINT32 RetStrLen;

        pStrRet = ArmStdC_strcpy(Filename, MaxFileNameLen, gInputPath);
        if ( ScaleId == 4U ) {
            ArmStdC_strcat(Filename, MaxFileNameLen, "/left_4_y/");
        } else {
            ArmStdC_strcat(Filename, MaxFileNameLen, "/left_2_y/");
        }
        RetStrLen = AmbaUtility_StringPrintUInt32(FileIndex, (UINT32)sizeof(FileIndex), "%010d", 1U, &FrameCnt);
        ArmStdC_strcat(Filename, MaxFileNameLen, FileIndex);
        ArmStdC_strcat(Filename, MaxFileNameLen, ".y");
        ArmLog_STR(UT_NAME, "## filename: %s", Filename, NULL);

        //load to outbuf
        Rval |= ArmFIO_GetSize(Filename, &Length);
        (void) RetStrLen;
    }

    pOutBufDesc = pLumaLeft;
    if ((Rval == ARM_OK) && (Length <= pOutBufDesc->buffer_size)) {
        Rval = ArmFIO_Load(pOutBufDesc->pBuffer, Length, Filename, &ActualLoadSize);
        Rval |= AmbaCV_UtilityCmaMemClean(pOutBufDesc);
    } else {
        ArmLog_ERR(UT_NAME, "## Proc2Uprp, ArmFIO_Load failed, Rval=%d, Length=%d", Rval, Length);
    }

    if(Rval != ARM_OK) {
        ArmLog_ERR(UT_NAME, "## Proc2Uprp, ArmFIO_Load failed", 0U, 0U);
    }

    //load right image
    //generate filename
    if (Rval == ARM_OK) {
        UINT32 RetStrLen;

        pStrRet = ArmStdC_strcpy(Filename, MaxFileNameLen, gInputPath);
        if ( ScaleId == 4U ) {
            ArmStdC_strcat(Filename, MaxFileNameLen, "/right_4_y/");
        } else {
            ArmStdC_strcat(Filename, MaxFileNameLen, "/right_2_y/");
        }
        RetStrLen = AmbaUtility_StringPrintUInt32(FileIndex, (UINT32)sizeof(FileIndex), "%010d", 1U, &FrameCnt);
        ArmStdC_strcat(Filename, MaxFileNameLen, FileIndex);
        ArmStdC_strcat(Filename, MaxFileNameLen, ".y");
        ArmLog_STR(UT_NAME, "## filename: %s", Filename, NULL);
        //load to outbuf
        Rval = ArmFIO_GetSize(Filename, &Length);
        (void) RetStrLen;
    }

    pOutBufDesc = pLumaRight;
    if ((Rval == ARM_OK) && (Length <= pOutBufDesc->buffer_size)) {
        Rval = ArmFIO_Load(pOutBufDesc->pBuffer, Length, Filename, &ActualLoadSize);
        Rval |= AmbaCV_UtilityCmaMemClean(pOutBufDesc);
    } else {
        ArmLog_ERR(UT_NAME, "## Proc2Uprp, ArmFIO_Load failed, Rval=%d, Length=%d", Rval, Length);
    }
    if(Rval != ARM_OK) {
        ArmLog_ERR(UT_NAME, "## Proc2Uprp, ArmFIO_Load failed", 0U, 0U);
    }

    (void) pStrRet;

    return Rval;
}

static UINT32 RefCV_SetupStixelTaskConfig(CTFW_STIXEL_TASK_CONFIG_t *pStixelTaskConfig,
        UINT32 RoadDetMode, flexidag_memblk_t StixelFxBinBuf[])
{
    UINT32 Rval = ARM_OK;
    AMBA_CV_STIXEL_CFG_V2_s *pStixelConfig;
    pStixelConfig = &pStixelTaskConfig->StixelCfg;

    pStixelTaskConfig->InDispScaleId = 0U;

    // Setup stixel configuration
    {
        pStixelConfig->AlgCfg.RoadEstiMode = (UINT16)RoadDetMode;
        pStixelConfig->AlgCfg.IntParam.Fu = UT_CFG_FOCAL_LENGTH_U;
        pStixelConfig->AlgCfg.IntParam.Fv = UT_CFG_FOCAL_LENGTH_V;
        pStixelConfig->AlgCfg.IntParam.U0 = UT_CFG_OPTICAL_CENTER_X;
        pStixelConfig->AlgCfg.IntParam.V0 = UT_CFG_OPTICAL_CENTER_Y;
        pStixelConfig->AlgCfg.IntParam.Baseline = UT_CFG_BASELINE;

        pStixelConfig->AlgCfg.ManualDetCfg.PitchStride = UT_CFG_ROAD_SEARCH_PITCH_STRIDE;
        pStixelConfig->AlgCfg.ManualDetCfg.HeightStride = UT_CFG_ROAD_SEARCH_HEIGHT_STRIDE;
        pStixelConfig->AlgCfg.ManualDetCfg.PitchSetNum = UT_CFG_ROAD_SEARCH_PITCH_NUM;
        pStixelConfig->AlgCfg.ManualDetCfg.HeightSetNum = UT_CFG_ROAD_SEARCH_HEIGHT_NUM;
        if ( pStixelConfig->AlgCfg.RoadEstiMode == STIXEL_ROAD_ESTI_MODE_MANUAL ) {
            pStixelConfig->AlgCfg.ManualDetCfg.ExtParam.Height = UT_CFG_DEFAULT_CAMERA_HEIGH;
            pStixelConfig->AlgCfg.ManualDetCfg.ExtParam.Pitch = UT_CFG_DEFAULT_CAMERA_PITCH;
        }
        pStixelConfig->AlgCfg.ManualDetCfg.VDispParam.XRatioThr = UT_CFG_VDISP_X_THR;
        pStixelConfig->AlgCfg.ManualDetCfg.VDispParam.YRatioThr = UT_CFG_VDISP_Y_THR;

        pStixelConfig->AlgCfg.MinDisparity = UT_CFG_MIN_DISPARITY;
        pStixelConfig->AlgCfg.DetObjectHeight = UT_CFG_DET_OBJECT_HEIGHT;

        pStixelConfig->RoiCfg.Width = STIXEL_IN_DISPARITY_WIDTH;
        pStixelConfig->RoiCfg.Height = STIXEL_IN_DISPARITY_HEIGHT;
        pStixelConfig->RoiCfg.StartX = 0U;
        pStixelConfig->RoiCfg.StartY = 0U;
    }

    {
        Rval |= LoadFlexiDagBin(SD_ROOT_PATH "/stixel_v_disparity_ag/flexibin/flexibin0.bin", &StixelFxBinBuf[STIXEL_FX_IDX_VDISP]);
        Rval |= LoadFlexiDagBin(SD_ROOT_PATH "/stixel_gen_esti_road_ag/flexibin/flexibin0.bin", &StixelFxBinBuf[STIXEL_FX_IDX_GEN_ESTI_ROAD]);
        Rval |= LoadFlexiDagBin(SD_ROOT_PATH "/stixel_preprocess_ag/flexibin/flexibin0.bin", &StixelFxBinBuf[STIXEL_FX_IDX_PREPROCESS]);
        Rval |= LoadFlexiDagBin(SD_ROOT_PATH "/stixel_free_space_ag/flexibin/flexibin0.bin", &StixelFxBinBuf[STIXEL_FX_IDX_FREE_SPACE]);
        Rval |= LoadFlexiDagBin(SD_ROOT_PATH "/stixel_height_seg_ag/flexibin/flexibin0.bin", &StixelFxBinBuf[STIXEL_FX_IDX_HEIGHT_SEG]);
        if (Rval != ARM_OK) {
            ArmLog_ERR(UT_NAME, "## LoadFlexiDagBin error", 0U, 0U);
        }
    }
    return Rval;
}

static UINT32 RefCV_SetupSpuFexTaskConfig(CTFW_SPU_FEX_TASK_CONFIG_t *pSpuFexTaskConfig)
{
    UINT32 Rval = ARM_OK;
    UINT32 ScaleIdx;
    roi_t Roi;
    UINT32 FusionDisparity;

    Roi.m_start_col = 0U;
    Roi.m_start_row = 0U;
    Roi.m_width_m1 = 0U;
    Roi.m_height_m1 = 0U;

    FusionDisparity = 0U;

    // spu output scale 2 and 4
    for(ScaleIdx = 0U; ScaleIdx < MAX_HALF_OCTAVES; ScaleIdx++) {
        if ( (ScaleIdx == 2U) || (ScaleIdx == 4U) ) {
            pSpuFexTaskConfig->SpuScaleCfg[ScaleIdx].Enable = 1U;
        } else {
            pSpuFexTaskConfig->SpuScaleCfg[ScaleIdx].Enable = 0U;
        }
        pSpuFexTaskConfig->SpuScaleCfg[ScaleIdx].RoiEnable = 0U;
        pSpuFexTaskConfig->SpuScaleCfg[ScaleIdx].Roi = Roi;
        pSpuFexTaskConfig->SpuScaleCfg[ScaleIdx].FusionDisparity = (UINT8)FusionDisparity;
    }

    // fex output scale 2
    for(ScaleIdx = 0U; ScaleIdx < MAX_HALF_OCTAVES; ScaleIdx++) {
        if ( ScaleIdx == 2U ) {
            pSpuFexTaskConfig->FexScaleCfg[ScaleIdx].Enable = (UINT8)CV_FEX_ENABLE_ALL;
        } else {
            pSpuFexTaskConfig->FexScaleCfg[ScaleIdx].Enable = (UINT8)CV_FEX_DISABLE;
        }
        pSpuFexTaskConfig->FexScaleCfg[ScaleIdx].RoiEnable = 0U;
        pSpuFexTaskConfig->FexScaleCfg[ScaleIdx].Roi = Roi;
    }

    //pSpuFexTaskConfig->SpuDisplayMode = AMBA_CV_SPU_DISPLAY_MODE;
    pSpuFexTaskConfig->SpuDisplayMode = AMBA_CV_SPU_NORMAL_MODE;

    return Rval;
}

static UINT32 RefCV_SetupFmaTaskConfig(CTFW_FMA_TASK_CONFIG_t *pFmaTaskConfig)
{
    UINT32 Rval = ARM_OK;

    pFmaTaskConfig->ScaleID = UT_CFG_MVAC_PROC_SCALE_ID;
    pFmaTaskConfig->FexSrcTaskType = CTFW_TASK_SPU_FEX;

    return Rval;
}

static UINT32 RefCV_SetupSpuFusionTaskConfig(CTFW_SPU_FUSION_TASK_CONFIG_t *pSpuFusionTaskConfig)
{
    UINT32 Rval = ARM_OK;

    pSpuFusionTaskConfig->LayerNum = 2U;
    pSpuFusionTaskConfig->ReferenceScaleId = 2U;

    return Rval;
}

static UINT32 RefCV_SetupVoTaskConfig(const CtfwCalibInfo_t *pCalibInfo,
                                      CTFW_MVAC_VO_TASK_CONFIG_t *pVoTaskConfig)
{
    UINT32 Rval = ARM_OK;
    AMBA_CV_VO_CFG_s *pVoConfig = &pVoTaskConfig->VoCfg;
    const flexidag_memblk_t *pMetaRightBuf = &pCalibInfo->Right;

    AmbaMisra_TypeCast(&pVoTaskConfig->pInitMetaRightBuf,
                       &pMetaRightBuf);
    pVoTaskConfig->FexSrcTaskType = CTFW_TASK_SPU_FEX;

    pVoConfig->View.RoiX = 0U;
    pVoConfig->View.RoiY = 0U;
    pVoConfig->View.RoiWidth = UT_CFG_MVAC_VO_PROC_WIDTH;
    pVoConfig->View.RoiHeight = UT_CFG_MVAC_VO_PROC_HEIGHT;
    pVoConfig->View.Scale = UT_CFG_MVAC_PROC_SCALE_ID;
    pVoConfig->MaxMinimizationIters = 8U;
    pVoConfig->ViewMaxMinimizationFeaturesStereo = 400U;

    return Rval;
}

static UINT32 RefCV_SetupMvacTaskConfig(const CtfwCalibInfo_t *pCalibInfo,
                                        const AMBA_CV_MVAC_WARP_TABLE_INFO_s *pWarpTableInfo, CTFW_MVAC_AC_TASK_CONFIG_t *pAcTaskConfig)
{
    UINT32 Rval = ARM_OK;
    const flexidag_memblk_t *pMetaLeftBuf = &pCalibInfo->Left;

    AmbaMisra_TypeCast(&pAcTaskConfig->pInitMetaLeftBuf,
                       &pMetaLeftBuf);
    pAcTaskConfig->FexSrcTaskType = CTFW_TASK_SPU_FEX;

    pAcTaskConfig->MvacViewInfo.WarpTableInfo = *pWarpTableInfo;
    pAcTaskConfig->MvacViewInfo.ScaleId = UT_CFG_MVAC_PROC_SCALE_ID;

    return Rval;
}

void RefCV_CtfwAcStixel_UT(const char *pInputPath, UINT32 StartFrameIdx,
                           UINT32 EndFrameIdx, UINT32 FrameIdxStep, UINT32 StixelRoadDetMode)
{
    UINT32 Rval = ARM_OK;

    static flexidag_memblk_t TotalBuf = {0};
    static UINT32 MemPoolId;

    UINT32 BuffIdx;

    static CtfwCalibInfo_t CalibInfo;
    static AMBA_CV_MVAC_WARP_TABLE_INFO_s WarpTableInfo;

    flexidag_memblk_t PicInfoBuff[CTFW_MAX_BUFFER_DEPTH];
    flexidag_memblk_t LumaLeft_Scale2[CTFW_MAX_BUFFER_DEPTH];
    flexidag_memblk_t LumaRight_Scale2[CTFW_MAX_BUFFER_DEPTH];
    flexidag_memblk_t LumaLeft_Scale4[CTFW_MAX_BUFFER_DEPTH];
    flexidag_memblk_t LumaRight_Scale4[CTFW_MAX_BUFFER_DEPTH];

    flexidag_memblk_t StixelFxBinBuf[STIXEL_FX_IDX_NUM];

    CtfwFrame_t InFramePool[CTFW_MAX_BUFFER_DEPTH];

    UINT32 Scale2ImageSize = 1844620U;//((1920U*960U) + 1420U);
    UINT32 Scale4ImageSize = 462220U;//((960U*480U) + 1420U);
    UINT32 AlignedImageSize = CTFW_CV_MEM_ALIGN(Scale2ImageSize) + CTFW_CV_MEM_ALIGN(Scale4ImageSize);
    UINT32 AlignedPicInfoSize = CTFW_CV_MEM_ALIGN((UINT32)sizeof(memio_source_recv_picinfo_t));
    UINT32 MetaSize = 3000U;
    UINT32 AllocSingleChSize = (CTFW_MAX_BUFFER_DEPTH*(AlignedImageSize+AlignedPicInfoSize)) + CTFW_CV_MEM_ALIGN(MetaSize);
    UINT32 AllocTotalSize = AllocSingleChSize*2U; // left and right

    static CTFW_SPU_FEX_TASK_CONFIG_t SpuFexTaskConfig;
    static CTFW_FMA_TASK_CONFIG_t FmaTaskConfig;
    static CTFW_MVAC_VO_TASK_CONFIG_t VoTaskConfig;
    static CTFW_MVAC_AC_TASK_CONFIG_t AcTaskConfig;
    static CTFW_SPU_FUSION_TASK_CONFIG_t SpuFusionTaskConfig;
    static CTFW_STIXEL_TASK_CONFIG_t StixelTaskConfig;

    UINT32 FrameCnt = 0U;

    gInputPath = pInputPath;

    // alloc memory
    {
        Rval = AmbaCV_UtilityCmaMemAlloc(AllocTotalSize, 1U, &TotalBuf);
        if (Rval != 0U) {
            ArmLog_ERR(UT_NAME, "## Can't allocate %u from CmaMem", AllocTotalSize, 0U);
        } else {
            Rval = ArmMemPool_Create(&TotalBuf, &MemPoolId);
        }
    }


    Rval |= Cftw_MemPool_Allocate(MemPoolId, MetaSize, &CalibInfo.Left);
    Rval |= Cftw_MemPool_Allocate(MemPoolId, MetaSize, &CalibInfo.Right);
    for (BuffIdx = 0U; BuffIdx < CTFW_MAX_BUFFER_DEPTH; BuffIdx++) {
        memio_source_recv_picinfo_t *pPicinfo;
        UINT32 ScaleIdx;
        roi_t ScaleRoi;
        UINT32 ScaleRoiPitch;

        Rval |= Cftw_MemPool_Allocate(MemPoolId, (UINT32)sizeof(memio_source_recv_picinfo_t), &(PicInfoBuff[BuffIdx]));
        AmbaMisra_TypeCast(&pPicinfo, &(PicInfoBuff[BuffIdx].pBuffer));
        ConfigPicInfoHeader(pPicinfo, SCALE_0_IMG_WIDTH, SCALE_0_IMG_WIDTH, SCALE_0_IMG_HEIGHT);

        Rval |= Cftw_MemPool_Allocate(MemPoolId, Scale2ImageSize, &(LumaLeft_Scale2[BuffIdx]));
        Rval |= Cftw_MemPool_Allocate(MemPoolId, Scale2ImageSize, &(LumaRight_Scale2[BuffIdx]));
        Rval |= Cftw_MemPool_Allocate(MemPoolId, Scale4ImageSize, &(LumaLeft_Scale4[BuffIdx]));
        Rval |= Cftw_MemPool_Allocate(MemPoolId, Scale4ImageSize, &(LumaRight_Scale4[BuffIdx]));

        ScaleIdx = 2U;
        ScaleRoi.m_start_col = 0U;
        ScaleRoi.m_start_row = 0U;
        ScaleRoi.m_width_m1 = (UINT16) IN_SCALE_2_IMG_ROI_WIDTH - 1U;
        ScaleRoi.m_height_m1 = (UINT16) IN_SCALE_2_IMG_ROI_HEIGHT- 1U;
        ScaleRoiPitch = IN_SCALE_2_IMG_ROI_WIDTH;
        ConfigPicInfoScale(pPicinfo, ScaleIdx, &ScaleRoi, ScaleRoiPitch,
                           LumaLeft_Scale2[BuffIdx].pBuffer, LumaRight_Scale2[BuffIdx].pBuffer);

        ScaleIdx = 4U;
        ScaleRoi.m_start_col = 0U;
        ScaleRoi.m_start_row = 0U;
        ScaleRoi.m_width_m1 = (UINT16) (IN_SCALE_2_IMG_ROI_WIDTH/2U) - 1U;
        ScaleRoi.m_height_m1 = (UINT16) (IN_SCALE_2_IMG_ROI_HEIGHT/2U)- 1U;
        ScaleRoiPitch = (IN_SCALE_2_IMG_ROI_WIDTH/2U);
        ConfigPicInfoScale(pPicinfo, ScaleIdx, &ScaleRoi, ScaleRoiPitch,
                           LumaLeft_Scale4[BuffIdx].pBuffer, LumaRight_Scale4[BuffIdx].pBuffer);
    }

    Rval |= RefCV_LoadCalibData(&CalibInfo);
    Rval |= RefCV_LoadWarpTable(&WarpTableInfo);

    Rval |= RefCV_SetupVoTaskConfig(&CalibInfo, &VoTaskConfig);
    Rval |= RefCV_SetupMvacTaskConfig(&CalibInfo, &WarpTableInfo, &AcTaskConfig);
    Rval |= RefCV_SetupSpuFexTaskConfig(&SpuFexTaskConfig);
    Rval |= RefCV_SetupFmaTaskConfig(&FmaTaskConfig);
    Rval |= RefCV_SetupSpuFusionTaskConfig(&SpuFusionTaskConfig);
    Rval |= RefCV_SetupStixelTaskConfig(&StixelTaskConfig, StixelRoadDetMode, StixelFxBinBuf);

    Rval |= AmbaCtfw_MvacStixelInit(&SpuFexTaskConfig,
                                    &FmaTaskConfig, &VoTaskConfig, &AcTaskConfig,
                                    &SpuFusionTaskConfig, &StixelTaskConfig,
                                    &RefCV_GetMvacResult, &RefCV_GetStixelResult, StixelFxBinBuf, CTFW_MAX_BUFFER_DEPTH, InFramePool);

    gLastSentFrameIdx = EndFrameIdx;

    for (FrameCnt = StartFrameIdx; FrameCnt <= EndFrameIdx; FrameCnt+=FrameIdxStep) {
        CtfwFrame_t *pFrame;
        UINT32 FrameBuffIdx = FrameCnt % CTFW_MAX_BUFFER_DEPTH;
        flexidag_memblk_t *pLumaLeft;
        flexidag_memblk_t *pLumaRight;
        const flexidag_memblk_t *pPicInfoBuff;
        UINT32 ScaleId;

        pPicInfoBuff = &PicInfoBuff[FrameBuffIdx];
        pFrame = &InFramePool[FrameBuffIdx];

        {
            pLumaLeft = &LumaLeft_Scale2[FrameBuffIdx];
            pLumaRight = &LumaRight_Scale2[FrameBuffIdx];
            ScaleId = 2U;
            Rval |= RefCV_LoadMvacInputLuma(FrameCnt, pLumaLeft, pLumaRight, ScaleId);

            pLumaLeft = &LumaLeft_Scale4[FrameBuffIdx];
            pLumaRight = &LumaRight_Scale4[FrameBuffIdx];
            ScaleId = 4U;
            Rval |= RefCV_LoadMvacInputLuma(FrameCnt, pLumaLeft, pLumaRight, ScaleId);
        }

        Rval |= AmbaCtfw_MvacStixelProduceFrame(pPicInfoBuff, &CalibInfo, FrameCnt, pFrame);
        if (Rval == ARM_OK) {
            Rval |= AmbaCtfw_MvacStixelFeedOneFrame(pFrame);
        }

        Rval |= AmbaKAL_TaskSleep(200U);
    }

    // Wait for both callback receive last frame
    while( (gMvacRcvLastFrame == 0U) || (gStixelRcvLastFrame == 0U) ) {
        Rval |= AmbaKAL_TaskSleep(100U);
    }

    AmbaCtfw_MvacStixelProfilerReport();

    return;
}

#if defined(CONFIG_LINUX)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "schdr_api.h"
#include "cvtask_api.h"
#include "ambint.h"
#include "cvapi_ambacv_flexidag.h"
//#include "rtos/AmbaRTOSWrapper.h"

static void RefCV_SignalHandlerShutdown(int sig)
{
    printf("%s: Got signal %d, program exits!\n",__FILE__,sig);
    exit(0);
}

int main(int argc, char **argv)
{
    static UINT32 init = 0;
    UINT32 ret = 0U;
    AMBA_CV_FLEXIDAG_SCHDR_CFG_s cfg;

    if ( argc < 6 ) {
        printf("Usage: %s [Input Folder] [StartFrame Index] [EndFrame Index] [Frame Index Step] [Stixel Road DetMode]\n", argv[0U]);
        printf("Ex: %s \n", argv[0U]);
        exit(-1);
    }

    /* Allow ourselves to be shut down gracefully by a signal */
    signal(SIGTERM, RefCV_SignalHandlerShutdown);
    signal(SIGHUP, RefCV_SignalHandlerShutdown);
    signal(SIGUSR1, RefCV_SignalHandlerShutdown);
    signal(SIGQUIT, RefCV_SignalHandlerShutdown);
    signal(SIGINT, RefCV_SignalHandlerShutdown);
    signal(SIGKILL, RefCV_SignalHandlerShutdown);

    /* init scheduler */
    cfg.cpu_map = 0xD;
    cfg.log_level = LVL_DEBUG;
    AmbaCV_FlexidagSchdrStart(&cfg);

    {
        const char* pInputPath = argv[1U];
        UINT32 StartFrameIdx = atoi(argv[2U]);
        UINT32 EndFrameIdx = atoi(argv[3U]);
        UINT32 FrameIdxStep = atoi(argv[4U]);
        UINT32 StixelRoadDetMode = atoi(argv[5U]);

        RefCV_CtfwAcStixel_UT(pInputPath, StartFrameIdx, EndFrameIdx, FrameIdxStep, StixelRoadDetMode);
    }

    return 0;
}
#endif
