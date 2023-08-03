/**
 *  @file RefCV_AmbaCtfwStixel_UT.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details SVC COMSVC CV
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaUtility.h"
#include "AmbaFS.h"
#include "AmbaWrap.h"
#if defined(CONFIG_SOC_CV2FS)
#include "cvapi_flexidag_ambaspufex_def_cv2fs.h"
#else
#include "cvapi_flexidag_ambaspufex_def_cv2.h"
#endif
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_stixel_v2.h"
#include "AmbaCtfw_StixelV2Flow.h"
#include "AmbaMisraFix.h"

#if defined(CONFIG_QNX)
#include "cvapi_flexidag_unittest.h"
#define SD_ROOT_PATH "/sd0"
#elif defined(CONFIG_LINUX)
#define SD_ROOT_PATH "/tmp/SD0"
#else // threadX and the others
#include "cvapi_flexidag_unittest.h"
#define SD_ROOT_PATH "c:"
#endif

#define UT_NAME "AmbaStixelV2_UT"

#define SCALE_0_IMG_WIDTH           (1920U)
#define SCALE_0_IMG_HEIGHT          (960U)

#define IN_DISP_SCALE   (0U)
#define IN_LUMA_SCALE   (0U)

// Stixel configuration
//---------------------------------------------
#define UT_CFG_FOCAL_LENGTH_U              (1308.845298)
#define UT_CFG_FOCAL_LENGTH_V              (1308.845298)
#define UT_CFG_OPTICAL_CENTER_X            (998.2284065)
#define UT_CFG_OPTICAL_CENTER_Y            (479.614282)
#define UT_CFG_BASELINE                    (0.3003)

#define UT_CFG_DEFAULT_CAMERA_HEIGHT        (1.545852)
#define UT_CFG_DEFAULT_CAMERA_PITCH         (0.155035)

#define UT_CFG_ROAD_SEARCH_PITCH_STRIDE                 (0.125)
#define UT_CFG_ROAD_SEARCH_HEIGHT_STRIDE                (0.01)
#define UT_CFG_ROAD_SEARCH_PITCH_NUM                    (129U)
#define UT_CFG_ROAD_SEARCH_HEIGHT_NUM                   (41U)
#define CAM_PH_MANUAL_DET_HEIGHT_METER_MAX_DIFF         (0.1)

#define UT_CFG_VDISP_X_THR                 (0.2)
#define UT_CFG_VDISP_Y_THR                 (0.15)

#define UT_CFG_DET_OBJECT_HEIGHT           (40U)
#define UT_CFG_MIN_DISPARITY               (2U)

#define UT_CFG_FREE_SPACE_GROUND_WEIGHTING (2U)
#define UT_CFG_FREE_SPACE_OBJECT_WEIGHTING (1U)
#define UT_CFG_FREE_SPACE_TOLERANCE        (2U)

#define UT_CFG_INVALID_DISP_VALUE          (0xFFF)
//---------------------------------------------

#define UT_STIXEL_OUT_PATH SD_ROOT_PATH"/CTFW_UT/out/stixelv2"

static UINT32 ReadInputFile(const char* path, flexidag_memblk_t* pMemBlk)
{
    UINT32 Ret;
    UINT32 Fsize = 0U, OpRes;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;

    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret != 0U) {
        ArmLog_ERR(UT_NAME, "ReadInputFile AmbaFS_FileOpen fail ", 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "ReadInputFile AmbaFS_FileTell fail ", 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_START);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileRead(pMemBlk->pBuffer, 1U, (UINT32)Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "ReadInputFile AmbaFS_FileRead fail ", 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "ReadInputFile AmbaFS_FileClose fail ", 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaCV_UtilityCmaMemClean(pMemBlk);
        if(Ret !=  0U) {
            ArmLog_ERR(UT_NAME, "ReadInputFile AmbaCV_UtilityCmaMemClean fail ", 0U, 0U);
        }
    }
    return Ret;
}

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

static void CtfwStixel_UT_ResultCbFxn(const CtfwFrameMetadata_t *pMetadata, const void *pOutData)
{
    const char *pOutFileFormat = UT_STIXEL_OUT_PATH "/STIXEL_OUT_%06d.bin";
    char OutFilePath[128U];
    CTFW_STIXEL_GEN_STIXEL_TASK_OUT_t *pStixelTskOut;
    UINT32 RetVal = ARM_OK;

    if ( pMetadata->ErrorState != 0U ) {
        ArmLog_STR(UT_NAME, "## %s, get error", __func__, 0U);
    } else {

        AmbaMisra_TypeCast(&pStixelTskOut, &pOutData);

        {
            UINT32 StixelNumM1;
            const AMBA_CV_STIXEL_V2_s *pStixel;

            ArmLog_DBG(UT_NAME, "## Get Stixel result, FrameIdx(%06d)", pMetadata->FrameIndex, 0U);

            pStixel = &pStixelTskOut->StixelInfo.StixelList[0U];
            ArmLog_DBG(UT_NAME, "1st stixel (%d, %d)", pStixel->Top, pStixel->Bottom);

            StixelNumM1 = pStixelTskOut->StixelInfo.StixelNum - 1U;
            pStixel = &pStixelTskOut->StixelInfo.StixelList[StixelNumM1];
            ArmLog_DBG(UT_NAME, "last stixel (%d, %d)", pStixel->Top, pStixel->Bottom);
        }

        {
            UINT32 WriteSize;
            UINT32 RetStrLen;

            RetStrLen = AmbaUtility_StringPrintUInt32(OutFilePath, (UINT32)sizeof(OutFilePath),
                        pOutFileFormat, 1U, &pMetadata->FrameIndex);
            RetVal |= ArmFIO_Save(&pStixelTskOut->StixelInfo, (UINT32)sizeof(AMBA_CV_STIXEL_V2_DET_OUT_s),
                                  OutFilePath, &WriteSize);

            (void) RetStrLen;
        }
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
                               UINT32 RoiPitch, const char *pLumaLeftAddr, const char *pLumaRightAddr)
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

static void SetupSpuData(const flexidag_memblk_t *pSpuDataBuff, const flexidag_memblk_t *pDisparityBuff,
                         UINT32 DispWidth, UINT32 DispPitch, UINT32 DispHeight)
{
    AMBA_CV_SPU_DATA_s *pSpuData;
    ULONG SpuDataAddr;
    ULONG DispDataAddr;
    ULONG DisparityMapOffset;
    UINT32 RetVal = ARM_OK;

    UINT32 SetupScaleIdx;

    AmbaMisra_TypeCast(&pSpuData, &pSpuDataBuff->pBuffer);
    AmbaMisra_TypeCast(&SpuDataAddr, &pSpuDataBuff->pBuffer);
    AmbaMisra_TypeCast(&DispDataAddr, &pDisparityBuff->pBuffer);
    DisparityMapOffset = DispDataAddr - SpuDataAddr;

    // Setup header
    {
        AmbaMisra_TypeCast(&pSpuData->Reserved_0[0], &pSpuData);
        pSpuData->Reserved_0[0] = MAX_HALF_OCTAVES;
        pSpuData->Reserved_0[1] = DispHeight;
        pSpuData->Reserved_0[2] = DispWidth;
        pSpuData->Reserved_0[3] = 0U;
    }

    // Zero out all scales
    {
        UINT32 ScaleIdx;
        for(ScaleIdx = 0U; ScaleIdx < MAX_HALF_OCTAVES; ScaleIdx++) {
            RetVal |= AmbaWrap_memset(&(pSpuData->Scales[ScaleIdx]), 0, sizeof(AMBA_CV_SPU_SCALE_OUT_s));
            pSpuData->Scales[ScaleIdx].Status = (INT32)0xFFFFFFFEUL; // Not valid
        }
    }

    SetupScaleIdx = IN_DISP_SCALE;
    // Setup scale
    {
        pSpuData->Scales[SetupScaleIdx].Status = 0;
        pSpuData->Scales[SetupScaleIdx].BufSize = (DispPitch*DispHeight);
        pSpuData->Scales[SetupScaleIdx].DisparityHeight = DispHeight;
        pSpuData->Scales[SetupScaleIdx].DisparityWidth = DispWidth;
        pSpuData->Scales[SetupScaleIdx].DisparityPitch = DispPitch;
        pSpuData->Scales[SetupScaleIdx].DisparityBpp = 16U;
        pSpuData->Scales[SetupScaleIdx].DisparityQm = 8U;
        pSpuData->Scales[SetupScaleIdx].DisparityQf = 4U;
        pSpuData->Scales[SetupScaleIdx].Reserved_0 = 0U;
        pSpuData->Scales[SetupScaleIdx].RoiStartRow = 0U;
        pSpuData->Scales[SetupScaleIdx].RoiStartCol = 0U;
        pSpuData->Scales[SetupScaleIdx].Reserved_1 = 1U;
        pSpuData->Scales[SetupScaleIdx].RoiAbsoluteStartCol = 0U;
        pSpuData->Scales[SetupScaleIdx].RoiAbsoluteStartRow = 0U;
        pSpuData->Scales[SetupScaleIdx].InvalidDisparities = 0U; //No use
        pSpuData->Scales[SetupScaleIdx].DisparityMapOffset = (relative_ptr_t)DisparityMapOffset;
        pSpuData->Scales[SetupScaleIdx].Reserved_2 = 0U;
        pSpuData->Scales[SetupScaleIdx].Reserved_3 = 0U;
    }

    (void) RetVal;

    return;
}

static UINT32 RefCV_SetupStixelV2Config(AMBA_CV_STIXEL_V2_CFG_s *pStixelConfig,
                                        UINT32 RoadDetMode)
{
    UINT32 Rval = ARM_OK;

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
    if ( pStixelConfig->AlgCfg.RoadEstiMode == STIXEL_V2_ROAD_ESTI_MODE_MANUAL ) {
        pStixelConfig->AlgCfg.ManualDetCfg.ExtParam.Height = UT_CFG_DEFAULT_CAMERA_HEIGHT;
        pStixelConfig->AlgCfg.ManualDetCfg.ExtParam.Pitch = UT_CFG_DEFAULT_CAMERA_PITCH;
        pStixelConfig->AlgCfg.ManualDetCfg.UseMultiLineFit = 0U;
        pStixelConfig->AlgCfg.ManualDetCfg.CamHeightLowerBnd =
            pStixelConfig->AlgCfg.ManualDetCfg.ExtParam.Height - CAM_PH_MANUAL_DET_HEIGHT_METER_MAX_DIFF;
    }
    pStixelConfig->AlgCfg.ManualDetCfg.VDispParam.XRatioThr = UT_CFG_VDISP_X_THR;
    pStixelConfig->AlgCfg.ManualDetCfg.VDispParam.YRatioThr = UT_CFG_VDISP_Y_THR;

    pStixelConfig->AlgCfg.MinDisparity = UT_CFG_MIN_DISPARITY;
    pStixelConfig->AlgCfg.DetObjectHeight = UT_CFG_DET_OBJECT_HEIGHT;
    pStixelConfig->AlgCfg.GroundWeighting = UT_CFG_FREE_SPACE_GROUND_WEIGHTING;
    pStixelConfig->AlgCfg.ObjectWeighting = UT_CFG_FREE_SPACE_OBJECT_WEIGHTING;
    pStixelConfig->AlgCfg.Tolerance = UT_CFG_FREE_SPACE_TOLERANCE;
    pStixelConfig->AlgCfg.InvalidHWDisparityVal = UT_CFG_INVALID_DISP_VALUE;

    pStixelConfig->RoiCfg.Width = STIXEL_V2_IN_DISPARITY_WIDTH;
    pStixelConfig->RoiCfg.Height = STIXEL_V2_IN_DISPARITY_HEIGHT;
    pStixelConfig->RoiCfg.StartX = 0U;
    pStixelConfig->RoiCfg.StartY = 0U;

    return Rval;
}

static UINT32 RefCV_SetupCamPoseTaskConfig(const AMBA_CV_STIXEL_V2_CFG_s *pStixelConfig,
        CTFW_STIXEL_CAM_POSE_TASK_CONFIG_t *pCamPoseTaskConfig)
{
    UINT32 Rval = ARM_OK;
    pCamPoseTaskConfig->StixelConfig = *pStixelConfig;
    pCamPoseTaskConfig->InDispScaleId = IN_LUMA_SCALE;
    return Rval;
}

static UINT32 RefCV_SetupGenStixelTaskConfig(const AMBA_CV_STIXEL_V2_CFG_s *pStixelConfig,
        CTFW_STIXEL_GEN_STIXEL_TASK_CONFIG_t *pGenStixelTaskConfig, flexidag_memblk_t StixelFxBinBuf[])
{
    UINT32 Rval = ARM_OK;
    pGenStixelTaskConfig->StixelConfig = *pStixelConfig;
    pGenStixelTaskConfig->InImgScaleId = IN_LUMA_SCALE;
    pGenStixelTaskConfig->InDispScaleId = IN_DISP_SCALE;
    {
        Rval |= LoadFlexiDagBin(SD_ROOT_PATH "/stixelv2_gen_esti_road_ag/flexibin/flexibin0.bin", &StixelFxBinBuf[GEN_STIXEL_FX_IDX_GEN_ESTI_ROAD]);
        Rval |= LoadFlexiDagBin(SD_ROOT_PATH "/stixelv2_preprocess_ag/flexibin/flexibin0.bin", &StixelFxBinBuf[GEN_STIXEL_FX_IDX_PREPROCESS]);
        Rval |= LoadFlexiDagBin(SD_ROOT_PATH "/stixelv2_free_space_ag/flexibin/flexibin0.bin", &StixelFxBinBuf[GEN_STIXEL_FX_IDX_FREE_SPACE]);
        Rval |= LoadFlexiDagBin(SD_ROOT_PATH "/stixelv2_free_space_dp_ag/flexibin/flexibin0.bin", &StixelFxBinBuf[GEN_STIXEL_FX_IDX_FREE_SPACE_DP]);
        Rval |= LoadFlexiDagBin(SD_ROOT_PATH "/stixelv2_height_seg_ag/flexibin/flexibin0.bin", &StixelFxBinBuf[GEN_STIXEL_FX_IDX_HEIGHT_SEG]);
        if (Rval != ARM_OK) {
            ArmLog_ERR(UT_NAME, "## LoadFlexiDagBin error", 0U, 0U);
        }
    }
    return Rval;
}

void RefCV_CtfwStixelV2_UT(const char* pDispInputPath, const char* pLumaInputPath,
                           UINT32 StartFrameIndex, UINT32 EndFrameIndex, UINT32 RoadDetMode)
{
    UINT32 Rval = ARM_OK;

    UINT32 FrameCnt = 0U;

    UINT32 DispWidth = SCALE_0_IMG_WIDTH;
    UINT32 DispPitch = DispWidth*2U;
    UINT32 DispHeight = SCALE_0_IMG_HEIGHT;

    UINT32 LumaPitch = SCALE_0_IMG_WIDTH;
    UINT32 LumaHeight = SCALE_0_IMG_HEIGHT;

    UINT32 AllocDisparitySize = DispPitch*DispHeight;
    UINT32 AllocLumaSize = (LumaPitch*LumaHeight) + 1420U;
    UINT32 AllocSpuDataSize = (UINT32)sizeof(AMBA_CV_SPU_DATA_s);
    UINT32 AllocPicInfoSize = (UINT32)sizeof(memio_source_recv_picinfo_t);

    UINT32 AlignedSingleFrameSize;
    UINT32 AllocTotalSize;

    AMBA_CV_STIXEL_V2_CFG_s StixelConfig;

    static flexidag_memblk_t TotalBuf;
    static UINT32 MemPoolId;
    static CtfwFrame_t InSpuFramePool[CTFW_MAX_BUFFER_DEPTH];
    static CtfwFrame_t InImgFramePool[CTFW_MAX_BUFFER_DEPTH];
    static flexidag_memblk_t SpuDataBuff[CTFW_MAX_BUFFER_DEPTH];
    static flexidag_memblk_t DisparityBuff[CTFW_MAX_BUFFER_DEPTH];
    static flexidag_memblk_t PicInfoBuff[CTFW_MAX_BUFFER_DEPTH];
    static flexidag_memblk_t LumaRight[CTFW_MAX_BUFFER_DEPTH];
    static CTFW_STIXEL_CAM_POSE_TASK_CONFIG_t CamPoseTaskConfig;
    static CTFW_STIXEL_GEN_STIXEL_TASK_CONFIG_t GenStixelTaskConfig;
    static flexidag_memblk_t FxBinBuf[GEN_STIXEL_FX_IDX_NUM];
    static UINT32 InitStat = 0U;

    AlignedSingleFrameSize = 0U;
    AlignedSingleFrameSize += CTFW_CV_MEM_ALIGN(AllocSpuDataSize);
    AlignedSingleFrameSize += CTFW_CV_MEM_ALIGN(AllocLumaSize);
    AlignedSingleFrameSize += CTFW_CV_MEM_ALIGN(AllocDisparitySize);
    AlignedSingleFrameSize += CTFW_CV_MEM_ALIGN(AllocPicInfoSize);

    AllocTotalSize = CTFW_MAX_BUFFER_DEPTH*AlignedSingleFrameSize;

    if ( InitStat == 0U ) {

        // Allocate memory
        {
            Rval = AmbaCV_UtilityCmaMemAlloc(AllocTotalSize, 1, &TotalBuf);
            if (Rval != ARM_OK) {
                ArmLog_ERR(UT_NAME, "## Can't allocate %u from CmaMem", AllocTotalSize, 0U);
            }
        }

        // Partitioning allocated memory
        if (Rval == ARM_OK) {
            UINT32 FrameBuffIdx;
            Rval |= ArmMemPool_Create(&TotalBuf, &MemPoolId);
            for (FrameBuffIdx = 0U; FrameBuffIdx < CTFW_MAX_BUFFER_DEPTH; FrameBuffIdx++) {
                // Setup Img
                {
                    memio_source_recv_picinfo_t *pPicinfo;
                    UINT32 ScaleIdx;
                    roi_t ScaleRoi;
                    UINT32 ScaleRoiPitch;

                    // No left luma is feed
                    const char *pFakeLumaLeftPtr;

                    Rval |= Cftw_MemPool_Allocate(MemPoolId, AllocPicInfoSize, &PicInfoBuff[FrameBuffIdx]);
                    Rval |= Cftw_MemPool_Allocate(MemPoolId, AllocLumaSize, &LumaRight[FrameBuffIdx]);
                    AmbaMisra_TypeCast(&pPicinfo, &(PicInfoBuff[FrameBuffIdx].pBuffer));
                    ConfigPicInfoHeader(pPicinfo, SCALE_0_IMG_WIDTH, SCALE_0_IMG_WIDTH, SCALE_0_IMG_HEIGHT);
                    ScaleIdx = IN_LUMA_SCALE;
                    ScaleRoi.m_start_col = 0U;
                    ScaleRoi.m_start_row = 0U;

                    ScaleRoi.m_width_m1 = (UINT16)SCALE_0_IMG_WIDTH;
                    ScaleRoi.m_width_m1 -= 1U;

                    ScaleRoi.m_height_m1 = (UINT16)SCALE_0_IMG_HEIGHT;
                    ScaleRoi.m_height_m1 -= 1U;

                    ScaleRoiPitch = SCALE_0_IMG_WIDTH;
                    pFakeLumaLeftPtr = LumaRight[FrameBuffIdx].pBuffer;
                    ConfigPicInfoScale(pPicinfo, ScaleIdx, &ScaleRoi, ScaleRoiPitch,
                                       pFakeLumaLeftPtr, LumaRight[FrameBuffIdx].pBuffer);
                }
                // Setup Spu
                {
                    Rval |= Cftw_MemPool_Allocate(MemPoolId, AllocSpuDataSize, &SpuDataBuff[FrameBuffIdx]);
                    Rval |= Cftw_MemPool_Allocate(MemPoolId, AllocDisparitySize, &DisparityBuff[FrameBuffIdx]);
                    SetupSpuData(&SpuDataBuff[FrameBuffIdx], &DisparityBuff[FrameBuffIdx], DispWidth, DispPitch, DispHeight);
                }
            }
            if (Rval != ARM_OK) {
                ArmLog_ERR(UT_NAME, "## ArmMemPool error", 0U, 0U);
            }
        }

        Rval |= RefCV_SetupStixelV2Config(&StixelConfig, RoadDetMode);
        Rval |= RefCV_SetupCamPoseTaskConfig(&StixelConfig, &CamPoseTaskConfig);
        Rval |= RefCV_SetupGenStixelTaskConfig(&StixelConfig, &GenStixelTaskConfig, FxBinBuf);

        if ( Rval == ARM_OK ) {
            CtfwGetProcResult_t GetStixelResultCbFxn = CtfwStixel_UT_ResultCbFxn;
            Rval |= AmbaCtfw_StixelV2Init(&CamPoseTaskConfig, &GenStixelTaskConfig,
                                          GetStixelResultCbFxn, FxBinBuf, CTFW_MAX_BUFFER_DEPTH, InSpuFramePool, InImgFramePool);
        }

        if ( Rval == ARM_OK ) {
            InitStat = 1U;
        }
    }

    if ( InitStat == 1U ) {
        UINT32 TargetFrames = (EndFrameIndex - StartFrameIndex) + 1U;

        for (FrameCnt = 0U; FrameCnt < TargetFrames; FrameCnt++) {
            UINT32 FrameBuffIdx;
            CtfwFrame_t *pSpuFrame;
            CtfwFrame_t *pImgFrame;

            // Load disparity file
            {
                char DispPath[128U];
                char LumaPath[128U];
                UINT32 FrameIdx;
                UINT32 RetStrLen;

                FrameIdx = StartFrameIndex + FrameCnt;
                FrameBuffIdx = FrameCnt % CTFW_MAX_BUFFER_DEPTH;

                // Read out disparity
                RetStrLen = AmbaUtility_StringPrintUInt32(DispPath, (UINT32)sizeof(DispPath),
                            pDispInputPath, 1U, &FrameIdx);
                Rval |= ReadInputFile(DispPath, &DisparityBuff[FrameBuffIdx]);
                pSpuFrame = &InSpuFramePool[FrameBuffIdx];

                // Read out luma
                RetStrLen = AmbaUtility_StringPrintUInt32(LumaPath, (UINT32)sizeof(LumaPath),
                            pLumaInputPath, 1U, &FrameIdx);
                Rval |= ReadInputFile(LumaPath, &LumaRight[FrameBuffIdx]);
                pImgFrame = &InImgFramePool[FrameBuffIdx];

                (void) RetStrLen;
            }

            if (Rval == ARM_OK) {
                Rval = AmbaCtfw_StixelV2ProduceSpuFrame(&SpuDataBuff[FrameBuffIdx], FrameCnt, pSpuFrame);
                Rval = AmbaCtfw_StixelV2ProduceImgFrame(&PicInfoBuff[FrameBuffIdx], FrameCnt, pImgFrame);
            }

            if (Rval == ARM_OK) {
                Rval = AmbaCtfw_StixelV2FeedOneSpuFrame(pSpuFrame);
                Rval = AmbaCtfw_StixelV2FeedOneImgFrame(pImgFrame);
            }

            Rval |= AmbaKAL_TaskSleep(100U);
        }
    }

    AmbaCtfw_StixelV2ProfilerReport();

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
        printf("Usage: %s [Input Disparity Folder] [Input Luma Folder] [StartFrame Index] [EndFrame Index] [Road detection mode]\n", argv[0U]);
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
        const char* pInputDispPath = argv[1U];
        const char* pInputLumaPath = argv[2U];
        UINT32 StartFrameIdx = atoi(argv[3U]);
        UINT32 EndFrameIdx = atoi(argv[4U]);
        UINT32 RoadDetMode = atoi(argv[5U]);

        RefCV_CtfwStixelV2_UT(pInputDispPath, pInputLumaPath, StartFrameIdx, EndFrameIdx, RoadDetMode);
    }

    return 0;
}
#endif

