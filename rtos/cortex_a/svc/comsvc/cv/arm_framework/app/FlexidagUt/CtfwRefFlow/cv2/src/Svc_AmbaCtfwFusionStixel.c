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
#include "Svc_MvacStixelFlow.h"
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
#include "cam_util.h"


#define UT_NAME "AmbaFusionStixel_UT"

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

#define RefCV_SVC_AMBALINK_CHANNEL  (0U)

#if 1
typedef struct {
    uint32_t Type;
    uint32_t State; ///RESET, WAIT, ACCUMULATION, SOLVE, END
    uint32_t NumRun; ///< The number of runs.
    uint32_t NumWarpInfoSent; ///< The number of models sent out for updating the warp tables.
    AMBA_CV_MVAC_STATISTICS_s ViewStatistic;
    AMBA_CV_MVAC_WARP_INFO_s WarpInfo; //NULL when no warp table output
    uint32_t WarpValid;
} SVC_CV_MVAC_DATA_s;
#endif

#define AMBA_CV_SPU_TABLE_MAXSIZE (1920 * 960 * 2)

typedef struct {
    uint32_t Type;
    AMBA_CV_SPU_DATA_s        SpuData;
    //UINT8                         ImageBuffer[AMBA_CV_SPU_TABLE_MAXSIZE];
    memio_source_recv_picinfo_t PicInfo;
} SVC_CV_SPU_BUF_s;


typedef struct {
    uint32_t Type;
    AMBA_CV_STIXEL_DET_OUT_s StixelInfo;
} SVC_CV_STIXEL_DATA_s;


static UINT32 gLastSentFrameIdx = 0U;
static UINT32 gStixelRcvLastFrame = 0U;
static UINT32 gMvacRcvLastFrame = 0U;
static flexidag_memblk_t ctfw_stixeloutblk[CTFW_MAX_BUFFER_DEPTH] = {0};
static flexidag_memblk_t ctfw_fusionoutblk[CTFW_MAX_BUFFER_DEPTH] = {0};

static memio_source_recv_picinfo_t PicInfoQue[CTFW_MAX_BUFFER_DEPTH];


UINT32 gInFrameCnt = 0U;
UINT32 gFUSIONOutFrameCnt = 0U;
UINT32 gSTIXELOutFrameCnt = 0U;
UINT32 gFrameID = 0U;

void RefCV_SVC_DumpPicInfo(const char *pModuleName, const cv_pic_info_t *pPicInfo)
{
    UINT32 i;

    ArmLog_ERR(UT_NAME,"-------------------------------------------", 0U, 0U);
    ArmLog_ERR(UT_NAME,"frame_num    : %d", pPicInfo->frame_num, 0U);
    ArmLog_ERR(UT_NAME,"token_id     : %d", pPicInfo->token_id, 0U);
    ArmLog_ERR(UT_NAME,"capture time : %d", pPicInfo->capture_time, 0U);
    ArmLog_ERR(UT_NAME,"channel_id   : %d", pPicInfo->channel_id, 0U);
    ArmLog_ERR(UT_NAME,"addi_flags   : %d", pPicInfo->additional_flags, 0U);
    ArmLog_ERR(UT_NAME,"num_tokens   : %d", pPicInfo->num_tokens, 0U);
    ArmLog_ERR(UT_NAME,"pyramid----------------", 0U, 0U);
    ArmLog_ERR(UT_NAME,"image_height_m1  : %d", pPicInfo->pyramid.image_height_m1, 0U);
    ArmLog_ERR(UT_NAME,"image_width_m1   : %d", pPicInfo->pyramid.image_width_m1, 0U);
    ArmLog_ERR(UT_NAME,"image_pitch_m1   : %d", pPicInfo->pyramid.image_pitch_m1, 0U);
    ArmLog_ERR(UT_NAME,"half octave------------", 0U, 0U);
    for (i = 0U; i < (UINT32)MAX_HALF_OCTAVES; i++) {
        ArmLog_ERR(UT_NAME,"[%2d]", i, 0U);
        ArmLog_ERR(UT_NAME,"[ctrl] disable: %d, mode: %d,", pPicInfo->pyramid.half_octave[i].ctrl.disable, pPicInfo->pyramid.half_octave[i].ctrl.mode);
        ArmLog_ERR(UT_NAME,"octave_mode: %d, roi_pitch: %d", pPicInfo->pyramid.half_octave[i].ctrl.octave_mode, pPicInfo->pyramid.half_octave[i].ctrl.roi_pitch);

        ArmLog_ERR(UT_NAME,"[%2d]", i, 0U);
        ArmLog_ERR(UT_NAME,"[roi]  %d X %d", (UINT32)pPicInfo->pyramid.half_octave[i].roi_width_m1, (UINT32)pPicInfo->pyramid.half_octave[i].roi_height_m1);
        ArmLog_ERR(UT_NAME,"(%d,%d)", (UINT32)pPicInfo->pyramid.half_octave[i].roi_start_col, (UINT32)pPicInfo->pyramid.half_octave[i].roi_start_row);
    }
    ArmLog_ERR(UT_NAME,"Luma/Chroma------------", 0U, 0U);
    for (i = 0U; i < (UINT32)MAX_HALF_OCTAVES; i++) {
        ArmLog_ERR(UT_NAME,"[%2d]", i, 0U);
        ArmLog_ERR(UT_NAME,"[RIGHT]Y: 0x%X UV: 0x%X", pPicInfo->rpLumaRight[i], pPicInfo->rpLumaRight[i]);
        ArmLog_ERR(UT_NAME,"[LEFT] Y: 0x%X UV: 0x%X", pPicInfo->rpLumaLeft[i], pPicInfo->rpLumaLeft[i]);
    }
    ArmLog_ERR(UT_NAME,"-------------------------------------------", 0U, 0U);
}


void RefCV_SVC_DumpSpuOutInfo(const char *pModuleName, const AMBA_CV_SPU_DATA_s *pOutSpu)
{
    UINT32 i;
    ULONG Addr;
    const AMBA_CV_SPU_SCALE_OUT_s *pScaleOut;

    AmbaMisra_TypeCast(&Addr, &pOutSpu);
    ArmLog_ERR(UT_NAME,"----[SPU out @ 0x%x]----", Addr, 0U);
    for (i = 0; i < MAX_HALF_OCTAVES; i++) {
        pScaleOut = &pOutSpu->Scales[i];
        ArmLog_ERR(UT_NAME,"[%u]: Status(0x%x)", i, (UINT32)pScaleOut->Status, 0U, 0U, 0U);
        ArmLog_ERR(UT_NAME,"BufSize: %d DisparityMapOffset: 0x%x", pScaleOut->BufSize, pScaleOut->DisparityMapOffset);
        ArmLog_ERR(UT_NAME,"W: %d H: %d", pScaleOut->DisparityWidth, pScaleOut->DisparityHeight);
        ArmLog_ERR(UT_NAME,"P: %d", pScaleOut->DisparityPitch, 0U);
        ArmLog_ERR(UT_NAME,"Bpp: %d RoiStartCol: %d", pScaleOut->DisparityBpp, pScaleOut->RoiStartCol);
        ArmLog_ERR(UT_NAME,"RoiStartRow: %d RoiAbsStartCol: %d", pScaleOut->RoiStartRow,  pScaleOut->RoiAbsoluteStartCol);
        ArmLog_ERR(UT_NAME,"RoiAbsStartRow: %d", pScaleOut->RoiAbsoluteStartRow, 0U);
    }
}

static void RefCV_SVC_Stereo_DumpSPUInfo(const const AMBA_CV_SPU_DATA_s *pOutSpu)
{
    static UINT32 DumpTimes = 1U;
    //extern void SvcCvFlow_DumpPicInfo(const char *pModuleName, const cv_pic_info_t *pPicInfo);

    if (DumpTimes > 0U) {
        RefCV_SVC_DumpSpuOutInfo("RefCV_CtfwFusionStixel", pOutSpu);
        DumpTimes--;
    }
}



static void RefCV_SVC_Stereo_DumpPicInfo(const cv_pic_info_t *pPicInfo)
{
    static UINT32 DumpTimes = 1U;
    //extern void SvcCvFlow_DumpPicInfo(const char *pModuleName, const cv_pic_info_t *pPicInfo);

    if (DumpTimes > 0U) {
        RefCV_SVC_DumpPicInfo("RefCV_CtfwFusionStixel", pPicInfo);
        DumpTimes--;
    }
}

/**
 * Give Result to RTOS.
 * This function will send result to RTOS.
 **/
static uint32_t RefCV_SVC_GiveResult(uint32_t ch, uint32_t frameId, void *result)
{
    uint32_t retcode = 0;
    flexidag_memblk_t *res_blk = NULL;

    /* Retrieve output */
    res_blk = (flexidag_memblk_t *)result;

    /* Send OSD buffer to RTOS */
    if (1) {
        memio_sink_send_out_t out = {0};

        out.cvtask_frameset_id = frameId;
        out.num_of_io = 1;

        AmbaMisra_TypeCast(&out.io[0].addr, &result);
        out.io[0].addr = res_blk->buffer_daddr;
        out.io[0].size = res_blk->buffer_size;

        retcode = CamUtil_SendResult(ch, &out);
    }

    return retcode;
}

static void RefCV_SVC_Process_input_picinfo(memio_source_recv_picinfo_t *pInfo, flexidag_memblk_t *input)
{
    memio_source_recv_picinfo_t *dest;
    int i;

    /* Always copy frame descripter to dag's input buffer. */
    /* This is because the pic_info might not be in the area which Dag can access. */
    dest = (memio_source_recv_picinfo_t *)input->pBuffer;
    memcpy(dest, pInfo, sizeof(memio_source_recv_picinfo_t));

    /* scheduler expect the rpXXXX is the offset to base address of pic_ifo. */
    /* However, the data retrieved from RTOS is physical address. */
    /* Therefore, they have to be adjusted. */
    for (i = 0U ; i < MAX_HALF_OCTAVES ; i++) {
        if (dest->pic_info.rpLumaLeft[i] != 0) { //valid address
            dest->pic_info.rpLumaLeft[i] -= input->buffer_daddr;
            dest->pic_info.rpLumaRight[i] -= input->buffer_daddr;
            dest->pic_info.rpChromaLeft[i] -= input->buffer_daddr;
            dest->pic_info.rpChromaRight[i] -= input->buffer_daddr;
        }
    }

    AmbaCV_UtilityCmaMemClean(input);
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

static void RefCV_SVC_GetStixelResult(const CtfwFrameMetadata_t *pMetadata, const void *pOutData)
{
    CTFW_STIXEL_TASK_OUT_t *pStixelTskOut;
    UINT32 RetVal = ARM_OK;
    UINT32 FrameBuffIdx = gSTIXELOutFrameCnt % CTFW_MAX_BUFFER_DEPTH;
    SVC_CV_STIXEL_DATA_s *pStixleOut;

    AmbaMisra_TypeCast(&pStixelTskOut, &pOutData);

    pStixleOut = (AMBA_CV_STIXEL_DET_OUT_s *)ctfw_stixeloutblk[FrameBuffIdx].pBuffer;
    pStixleOut->Type = 0x1008U; //Stixel
    RetVal |= AmbaWrap_memcpy(&pStixleOut->StixelInfo, &pStixelTskOut->StixelInfo, sizeof(AMBA_CV_STIXEL_DET_OUT_s));

    RefCV_SVC_GiveResult(RefCV_SVC_AMBALINK_CHANNEL, pMetadata->FrameIndex, (void *)&ctfw_stixeloutblk[FrameBuffIdx]);

    (void) RetVal;
    gSTIXELOutFrameCnt++;
    return;
}

static void RefCV_SVC_GetFusionResult(const CtfwFrameMetadata_t *pMetadata, const void *pOutData)
{

    AMBA_CV_SPU_DATA_s *pFusionTskOut;

    UINT32 RetVal = ARM_OK;
    UINT32 i;
    UINT32 FrameBuffIdx = gFUSIONOutFrameCnt % CTFW_MAX_BUFFER_DEPTH;
    SVC_CV_SPU_BUF_s *pSpuOut;
    AMBA_CV_SPU_DATA_s *pSpuData;

    AmbaMisra_TypeCast(&pFusionTskOut, &pOutData);

    pSpuOut = (SVC_CV_SPU_BUF_s *)ctfw_fusionoutblk[FrameBuffIdx].pBuffer;
    pSpuData = &pSpuOut->SpuData;
    RetVal |= AmbaWrap_memcpy(pSpuData, pFusionTskOut, sizeof(AMBA_CV_SPU_DATA_s));

    pSpuOut->Type = 0x1003U;

    for (i = 0; i < CTFW_MAX_BUFFER_DEPTH; i++) {
        if (pMetadata->FrameIndex == PicInfoQue[i].pic_info.frame_num) {
            //ArmLog_OK(UT_NAME, "Find index %d frame num %d in PicInfoQue", i, PicInfoQue[i].pic_info.frame_num);
            RetVal |= AmbaWrap_memcpy(&pSpuOut->PicInfo, &PicInfoQue[i], sizeof(memio_source_recv_picinfo_t));
        }
    }

    RefCV_SVC_GiveResult(RefCV_SVC_AMBALINK_CHANNEL, pMetadata->FrameIndex, (void *)&ctfw_fusionoutblk[FrameBuffIdx]);

    gFUSIONOutFrameCnt++;
    return;
}

static UINT32 RefCV_SetupStixelTaskConfig(CTFW_STIXEL_TASK_CONFIG_t *pStixelTaskConfig,
        UINT32 RoadDetMode, DOUBLE EstiCamHeight, DOUBLE EstiCamPitch, flexidag_memblk_t StixelFxBinBuf[])
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
            //pStixelConfig->AlgCfg.ManualDetCfg.ExtParam.Height = UT_CFG_DEFAULT_CAMERA_HEIGH;
            //pStixelConfig->AlgCfg.ManualDetCfg.ExtParam.Pitch = UT_CFG_DEFAULT_CAMERA_PITCH;
            pStixelConfig->AlgCfg.ManualDetCfg.ExtParam.Height = EstiCamHeight;
            pStixelConfig->AlgCfg.ManualDetCfg.ExtParam.Pitch = EstiCamPitch;
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

static UINT32 RefCV_SetupSpuFusionTaskConfig(CTFW_SPU_FUSION_TASK_CONFIG_t *pSpuFusionTaskConfig)
{
    UINT32 Rval = ARM_OK;

    pSpuFusionTaskConfig->LayerNum = 2U;
    pSpuFusionTaskConfig->ReferenceScaleId = 2U;

    return Rval;
}

void RefCV_CtfwAcStixel_UT(const char *pInputPath, UINT32 StartFrameIdx,
                           UINT32 EndFrameIdx, UINT32 FrameIdxStep, UINT32 StixelRoadDetMode, DOUBLE EstiCamHeight, DOUBLE EstiCamPitch)
{
    UINT32 Rval = ARM_OK;

    static flexidag_memblk_t TotalBuf = {0};
    static UINT32 MemPoolId;
    UINT32 i;
    UINT32 BuffIdx;

    static CtfwCalibInfo_t CalibInfo;
    static AMBA_CV_MVAC_WARP_TABLE_INFO_s WarpTableInfo;

    flexidag_memblk_t PicInfoBuff[CTFW_MAX_BUFFER_DEPTH];
    flexidag_memblk_t StixelFxBinBuf[STIXEL_FX_IDX_NUM];

    CtfwFrame_t InFramePool[CTFW_MAX_BUFFER_DEPTH];
    UINT32 AlignedPicInfoSize = CTFW_CV_MEM_ALIGN((UINT32)sizeof(memio_source_recv_picinfo_t));
    UINT32 MetaSize = 3000U;
    UINT32 AlignedFusionDataSize = CTFW_CV_MEM_ALIGN((UINT32)sizeof(SVC_CV_SPU_BUF_s));
    UINT32 AlignedStixelDataSize = CTFW_CV_MEM_ALIGN((UINT32)sizeof(AMBA_CV_STIXEL_DET_OUT_s));
    UINT32 AllocSingleChSize = CTFW_MAX_BUFFER_DEPTH*(AlignedPicInfoSize);
    UINT32 AllocTotalSize = AllocSingleChSize * 2U + (CTFW_MAX_BUFFER_DEPTH * (AlignedStixelDataSize + AlignedFusionDataSize)); // left and right

    static CTFW_SPU_FEX_TASK_CONFIG_t SpuFexTaskConfig;
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

    for (BuffIdx = 0U; BuffIdx < CTFW_MAX_BUFFER_DEPTH; BuffIdx++) {
        memio_source_recv_picinfo_t *pPicinfo;
        UINT32 FUSIONOutsize =  sizeof(SVC_CV_SPU_BUF_s);
        UINT32 StixelOutsize =  sizeof(AMBA_CV_STIXEL_DET_OUT_s);
        Rval |= Cftw_MemPool_Allocate(MemPoolId, (UINT32)sizeof(memio_source_recv_picinfo_t), &(PicInfoBuff[BuffIdx]));
        AmbaMisra_TypeCast(&pPicinfo, &(PicInfoBuff[BuffIdx].pBuffer));

        Rval |= Cftw_MemPool_Allocate(MemPoolId, FUSIONOutsize, &ctfw_fusionoutblk[BuffIdx]);
        Rval |= Cftw_MemPool_Allocate(MemPoolId, StixelOutsize, &ctfw_stixeloutblk[BuffIdx]);
    }

    if (Rval != OK) {
        ArmLog_DBG(UT_NAME, "##Cftw_MemPool_Allocate fail", 0U, 0U);
    }

    Rval |= RefCV_SetupSpuFexTaskConfig(&SpuFexTaskConfig);
    Rval |= RefCV_SetupSpuFusionTaskConfig(&SpuFusionTaskConfig);
    Rval |= RefCV_SetupStixelTaskConfig(&StixelTaskConfig, StixelRoadDetMode, EstiCamHeight, EstiCamPitch, StixelFxBinBuf);

    Rval |= AmbaCtfw_FusionStixelInit(&SpuFexTaskConfig,
                                      &SpuFusionTaskConfig, &StixelTaskConfig,
                                      &RefCV_SVC_GetFusionResult, &RefCV_SVC_GetStixelResult, StixelFxBinBuf, CTFW_MAX_BUFFER_DEPTH, InFramePool);

    gLastSentFrameIdx = EndFrameIdx;

    CamUtil_Init(RefCV_SVC_AMBALINK_CHANNEL);
    for (;;) {
        CtfwFrame_t *pFrame;
        UINT32 FrameBuffIdx = gInFrameCnt % CTFW_MAX_BUFFER_DEPTH;
        const flexidag_memblk_t *pPicInfoBuff;
        UINT32 ScaleId;
        UINT32 Addr;
        static memio_source_recv_picinfo_t RecvData;
        static memio_source_recv_picinfo_t *pPicinfo;

        pPicInfoBuff = &PicInfoBuff[FrameBuffIdx];
        pFrame = &InFramePool[FrameBuffIdx];
        AmbaMisra_TypeCast(&pPicinfo, &(PicInfoBuff[FrameBuffIdx].pBuffer));
        CamUtil_GetFrame(RefCV_SVC_AMBALINK_CHANNEL, &RecvData);

        RefCV_SVC_Process_input_picinfo(&RecvData, pPicInfoBuff);

        AmbaWrap_memcpy(&PicInfoQue[FrameBuffIdx], &RecvData, sizeof(memio_source_recv_picinfo_t));

        Rval |= AmbaCtfw_FusionStixelProduceFrame(pPicInfoBuff, &CalibInfo, RecvData.pic_info.frame_num, pFrame);
        if (Rval == ARM_OK) {
            Rval |= AmbaCtfw_FusionStixelFeedOneFrame(pFrame);
        }

        gInFrameCnt++;
    }
    CamUtil_Release(RefCV_SVC_AMBALINK_CHANNEL);

    AmbaCtfw_FusionStixelProfilerReport();

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

    if ( argc < 8 ) {
        printf("Usage: %s [Input Folder] [StartFrame Index] [EndFrame Index] [Frame Index Step] [Stixel Road DetMode] [EstiCamHeight] [EstiCamPitch]", argv[0U]);
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
        DOUBLE EstiCamHeight = atof(argv[6U]);
        DOUBLE EstiCamPitch = atof(argv[7U]);
        RefCV_CtfwAcStixel_UT(pInputPath, StartFrameIdx, EndFrameIdx, FrameIdxStep, StixelRoadDetMode, EstiCamHeight, EstiCamPitch);
    }

    return 0;
}
#endif
