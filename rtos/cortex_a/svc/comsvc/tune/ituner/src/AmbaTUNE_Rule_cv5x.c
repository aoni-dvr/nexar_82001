/*
*  @file AmbaTUNE_Rule_cv5x.c
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

#include "AmbaDSP.h" // Note: AMBA_DSP_VIDEO_FILTER need
#include "AmbaTUNE_Parser_cv5x.h"
#include "AmbaTUNE_Rule_cv5x.h"
#include "AmbaTUNE_ImgCalibItuner_cv5x.h" //Note: AMBA_ITN_SYSTEM_s need
#include "AmbaDSP_ImageFilter.h" // Note: IK_NUM_EXPOSURE_CURVE need
#include "AmbaMisraFix.h"
#include "AmbaDSP_ImageHisoFilter.h" // Note: IK_NUM_EXPOSURE_CURVE need

/*
#define RULE_DEBF(fmt, arg...) AmbaPrint("[TEXT RULE][DEBUG]"fmt, ##arg)
#define RULE_WARF(fmt, arg...) AmbaPrint("[TEXT RULE][WARNING]"fmt, ##arg)
#define RULE_ERRF(fmt, arg...) AmbaPrint("[TEXT RULE][ERROR]"fmt, ##arg)
#define OVER_FLOW_CHECK(INDEX, MAX) \
    if ((INDEX) > (MAX)) {\
        RULE_ERRF("%s() %d Index = %d, Max = %d", __func__, __LINE__, (INDEX), (MAX));\
    }
*/
#define GetArraySize(x) ((INT32)(sizeof(x) / sizeof((x)[0])))

void ituner_param_proc(const PARAM_s *pParam, const void *pField);
static void System_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void SdkInfo_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Internal_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void AaaFunctionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void AeInfoProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void WbSimInfoProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void StaticBadPixelCorrectionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void StaticBadPixelCorrectionInternalProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void VignetteCompensationProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void WarpCompensationProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void WarpCompensation2ndProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void DzoomProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void WarpCompensationDzoomInternalProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void SensorInputProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void DeferredBlackLevelProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void DynamicBadPixelCorrectionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void CfaLeakageFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void CfaNoiseFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void AntiAliasingProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void BeforeCeWbGainProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void AfterCeWbGainProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void StoredIrProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void ColorCorrectionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void ToneCurveProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void FrontEndToneCurveProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void RgbToYuvMatrixProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void RgbIrProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void ChromaScaleProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void ChromaMedianFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void DemosaicFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void RgbToY12Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void SharpenBothProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void FinalSharpenBothProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void SharpenNoiseProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void FinalSharpenNoiseProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void SharpenFirProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void SharpenCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void FinalSharpenCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void FinalSharpenFirProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void CoringIndexScaleProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void MinCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void MaxCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void ScaleCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void FinalCoringIndexScaleProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void FinalScaleCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void FinalMinCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void FinalMaxCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void VideoMctfProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void VideoMctfLevelProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void VideoMctfTemporalAdjustProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void VideoMctfAndFinalSharpenProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void ShpAOrSpatialFilterSelectproc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void AdvanceSpatialFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void LumaNoiseReductionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiLumaNoiseReductionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2LumaNoiseReductionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void ChromaFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void GbGrMismatchProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void ChromaAberrationProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void ChromaAberrationInternalProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void ContrastEnhance_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);

static void HdrFrontEndWbgainExp0_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HdrFrontEndWbgainExp1_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HdrFrontEndWbgainExp2_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HdrBlend_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HdrRawInfo_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HdrStaticBlackLevelExp0_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HdrStaticBlackLevelExp1_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HdrStaticBlackLevelExp2_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void WideChromaFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void WideChromaFilterCombineProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void DummyProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void ActiveProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);

// hiso
static void LiChromaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiChromaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiLumaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiCfaNoiseFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiCfaLeakageFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiAntiAliasingProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiAutoBadPixelCorrectionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiChromaFilterHighProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiChromaFilterLowProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiChromaFilterLowCombineProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiChromaFilterMedianProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiChromaFilterMedianCombineProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiChromaFilterPreProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiChromaFilterVeryLowProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiChromaFilterVeryLowCombineProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiChromaMedianFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiDemosaicFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiGbGrMismatchProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiHigh2LumaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiHighLumaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiHighSharpenBothProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiHighSharpenNoiseProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiHighSharpenFirProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiHighSharpenCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiHighCoringIndexScaleProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiHighMaxCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiHighMinCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiHighScaleCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiLowLumaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiLowLumaASFCombineProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiLumaNoiseCombineProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiMedian1LumaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiMedian2LumaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiMedianSharpenBothProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiMedianSharpenNoiseProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiMedianSharpenFirProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiMedianSharpenCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiMedianCoringIndexScaleProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiMedianMaxCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiMedianMinCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiMedianScaleCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiSelectProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiLumaBlendProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiLiCombineProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiLiSharpenBothProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiLiSharpenNoiseProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiLiSharpenFirProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiLiSharpenCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiLiCoringIndexScaleProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiLiMaxCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiLiMinCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiLiScaleCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2LumaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2AntiAliasingProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2AutoBadPixelCorrectionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2CfaLeakageFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2CfaNoiseFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2DemosaicFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2GbGrMismatchProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2SharpenBothProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2SharpenNoiseProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2SharpenFirProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2SharpenCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2CoringIndexScaleProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2MaxCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2MinCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2ScaleCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void HiNonsmoothDetectProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2WideChromaFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);
static void Li2WideChromaFilterCombineProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam);

static OpMode_e Opmode = ITUNER_ENC;

static const char str_tuning_mode[IMG_MODE_NUMBER][32] = {
    "IMG_MODE_VIDEO",
    "IMG_MODE_PREVIEW",
    "IMG_MODE_LOW_ISO_STILL",
    "IMG_MODE_HIGH_ISO_STILL",
};

static const char str_tuning_mode_ext[TUNING_MODE_EXT_NUMBER][32] = {
    "SINGLE_SHOT",
    "SINGLE_SHOT_MULTI_EXPOSURE_HDR",
};

#define SdkInfo_Params_Count 8
static PARAM_s SdkInfo_Params[SdkInfo_Params_Count] = {
    { 0, "burst_tile_enable", U_32, 1, NULL, 2},
    { 1, "stitch_enable", U_32, 1, NULL, 2},
    { 2, "stitch_tile_num_x", U_32, 1, NULL, 2},
    { 3, "stitch_tile_num_y", U_32, 1, NULL, 2},
    { 4, "dram_efficiency", U_32, 1, NULL, 2},
    { 5, "luma_dma_size", U_32, 1, NULL, 2},
    { 6, "luma_wait_lines", U_32, 1, NULL, 2},
    { 7, "overlap_x", U_32, 1, NULL, 2},
};

#define System_Params_Count 39
static PARAM_s System_Params[System_Params_Count] = {
    { 0, "ituner_rev", U_32, 1, NULL, 0},
    { 1, "chip_rev", STRING, 1, NULL, 0 },
    { 2, "sensor_id", STRING, 1, NULL, 0 },
    { 3, "user_mode", STRING, 1, NULL, 0 },
    { 4, "tuning_mode", STRING, 1, NULL, 0 },
    { 5, "raw_path", STRING, 1, NULL, 0 },
    { 6, "raw_width", U_32, 1, NULL, 0 },
    { 7, "raw_height", U_32, 1, NULL, 0 },
    { 8, "raw_pitch", U_32, 1, NULL, 0 },
    { 9, "raw_resolution", U_32, 1, NULL, 0 },
    { 10, "raw_bayer", U_32, 1, NULL, 0 },
    { 11, "main_width", U_32, 1, NULL, 0 },
    { 12, "main_height", U_32, 1, NULL, 0 },
    { 13, "tuning_mode_ext", STRING, 1, NULL, 0 },
    { 14, "extended_cfg_path", STRING, 1, NULL, 0 },
    { 15, "input_pic_cnt", U_32, 1, NULL, 0 },
    { 16, "compressed_raw", U_32, 1, NULL, 0 },
    { 17, "sensor_readout_mode", U_32, 1, NULL, 0 },
    { 18, "raw_start_x", U_32, 1, NULL, 0 },
    { 19, "raw_start_y", U_32, 1, NULL, 0 },
    { 20, "h_subsample_factor_num", U_32, 1, NULL, 0 },
    { 21, "h_subsample_factor_den", U_32, 1, NULL, 0 },
    { 22, "v_subsample_factor_num", U_32, 1, NULL, 0 },
    { 23, "v_subsample_factor_den", U_32, 1, NULL, 0 },
    { 24, "output_filename", STRING, 1, NULL, 0 },
    { 25, "enable_raw2raw", U_32, 1, NULL, 0 },
    { 26, "jpg_quality", U_32, 1, NULL, 0 },
    { 27, "number_of_exposures", U_32, 1, NULL, 0 },
    { 28, "sensor_mode", U_32, 1, NULL, 0 },
    { 29, "compression_offset", S_32, 1, NULL, 0 },
    { 30, "cfa_width", U_32, 1, NULL, 0 },
    { 31, "cfa_height", U_32, 1, NULL, 0 },
    { 32, "ability", U_32, 1, NULL, 0},
    { 33, "yuv_mode", U_32, 1, NULL, 0},
    { 34, "store_ir", U_32, 1, NULL, 0},
    { 35, "flip_h", U_32, 1, NULL, 0},
    { 36, "flip_v", U_32, 1, NULL, 0},
    { 37, "frame_number", U_32, 1, NULL, 0},
    { 38, "number_of_frames", U_32, 1, NULL, 0},
};

#define Internal_Params_Count 4
static PARAM_s Internal_Params[Internal_Params_Count] = {
    { 0, "fpn_highlight", S_8, 1, NULL, 0},
    { 1, "diag_mode", U_8, 1, NULL, 0},
    { 2, "video_mctf_y_10", U_8, 1, NULL, 0},
    { 3, "video_mctf_fast_start", U_8, 1, NULL, 0},
};

#define AaaFunctionParams_Count 4
static PARAM_s AaaFunctionParams[AaaFunctionParams_Count] = {
    { 0, "ae_op", U_8, 1, NULL, 0},
    { 1, "awb_op", U_8, 1, NULL, 0},
    { 2, "af_op", U_8, 1, NULL, 0},
    { 3, "adj_op", U_8, 1, NULL, 0},
};

#define AeInfoParams_Count 15
static PARAM_s AeInfoParams[AeInfoParams_Count] = {
    { 0, "ev_index", U_16, 1, NULL, 0},
    { 1, "nf_index", U_16, 1, NULL, 0},
    { 2, "shutter_index", S_16, 1, NULL, 0},
    { 3, "agc_index", S_16, 1, NULL, 0},
    { 4, "iris_index", S_16, 1, NULL, 0},
    { 5, "dgain", S_32, 1, NULL, 0},
    { 6, "iso_value", U_16, 1, NULL, 0},
    { 7, "flash", U_16, 1, NULL, 0},
    { 8, "mode", U_16, 1, NULL, 0},
    { 9, "shutter_time", S_32, 1, NULL, 0},
    { 10, "agc_gain", S_32, 1, NULL, 0},
    { 11, "target", U_16, 1, NULL, 0},
    { 12, "luma_stat", U_16, 1, NULL, 0},
    { 13, "limit_status", S_16, 1, NULL, 0},
    { 14, "multiplier", U_32, 1, NULL, 0},
};

#define WbSimInfoParams_Count 5
static PARAM_s WbSimInfoParams[WbSimInfoParams_Count] = {
    { 0, "luma_index", U_16, 1, NULL, 0},
    { 1, "outdoor_index", U_16, 1, NULL, 0},
    { 2, "highlight_index", U_16, 1, NULL, 0},
    { 3, "low_index", U_16, 1, NULL, 0},
    { 4, "awb_ratio", U_32, 2, NULL, 0},
};

#define StaticBadPixelCorrectionParams_Count 11
static PARAM_s StaticBadPixelCorrectionParams[StaticBadPixelCorrectionParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "calib_version", U_32, 1, NULL, 0},
    {2, "calib_vin_start_x", U_32, 1, NULL, 0},
    {3, "calib_vin_start_y", U_32, 1, NULL, 0},
    {4, "calib_vin_width", U_32, 1, NULL, 0},
    {5, "calib_vin_height", U_32, 1, NULL, 0},
    {6, "calib_vin_h_subsample_factor_num", U_32, 1, NULL, 0},
    {7, "calib_vin_h_subsample_factor_den", U_32, 1, NULL, 0},
    {8, "calib_vin_v_subsample_factor_num", U_32, 1, NULL, 0},
    {9, "calib_vin_v_subsample_factor_den", U_32, 1, NULL, 0},
    {10, "map_path", EXT_PATH, 1, NULL, 0},
};

#define StaticBadPixelCorrectionInternalParams_Count 5
static PARAM_s StaticBadPixelCorrectionInternalParams[StaticBadPixelCorrectionInternalParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "pixel_map_width", U_32, 1, NULL, 0},
    {2, "pixel_map_height", U_32, 1, NULL, 0},
    {3, "pixel_map_pitch", U_32, 1, NULL, 0},
    {4, "map_path", EXT_PATH, 1, NULL, 0},
};

#define VignetteCompensationParams_Count 18
static PARAM_s VignetteCompensationParams[VignetteCompensationParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "coarse_radial_bins_count", U_32, 1, NULL, 0},
    {2, "coarse_radial_bins_logsize", U_32, 1, NULL, 0},
    {3, "fine_radial_bins_count", U_32, 1, NULL, 0},
    {4, "fine_radial_bins_logsize", U_32, 1, NULL, 0},
    {5, "model_center_x", U_32, 4, NULL, 0},
    {6, "model_center_y", U_32, 4, NULL, 0},
    {7, "vig_gain_table_path", EXT_PATH, 1, NULL, 0},
    {8, "calib_version", U_32, 1, NULL, 0},
    {9, "calib_vin_start_x", U_32, 1, NULL, 0},
    {10, "calib_vin_start_y", U_32, 1, NULL, 0},
    {11, "calib_vin_width", U_32, 1, NULL, 0},
    {12, "calib_vin_height", U_32, 1, NULL, 0},
    {13, "calib_vin_h_subsample_factor_num", U_32, 1, NULL, 0},
    {14, "calib_vin_h_subsample_factor_den", U_32, 1, NULL, 0},
    {15, "calib_vin_v_subsample_factor_num", U_32, 1, NULL, 0},
    {16, "calib_vin_v_subsample_factor_den", U_32, 1, NULL, 0},
    {17, "calib_mode_enable", U_32, 1, NULL, 0},
};

#define WarpCompensationParams_Count 16
static PARAM_s WarpCompensationParams[WarpCompensationParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "calib_version", U_32, 1, NULL, 0},
    {2, "horizontal_grid_number", U_32, 1, NULL, 0},
    {3, "vertical_grid_number", U_32, 1, NULL, 0},
    {4, "tile_width_exponent", U_32, 1, NULL, 0},
    {5, "tile_height_exponent", U_32, 1, NULL, 0},
    {6, "vin_sensor_start_x", U_32, 1, NULL, 0},
    {7, "vin_sensor_start_y", U_32, 1, NULL, 0},
    {8, "vin_sensor_width", U_32, 1, NULL, 0},
    {9, "vin_sensor_height", U_32, 1, NULL, 0},
    {10, "vin_sensor_h_subsample_factor_num", U_32, 1, NULL, 0},
    {11, "vin_sensor_h_subsample_factor_den", U_32, 1, NULL, 0},
    {12, "vin_sensor_v_subsample_factor_num", U_32, 1, NULL, 0},
    {13, "vin_sensor_v_subsample_factor_den", U_32, 1, NULL, 0},
    {14, "enb_2stage_compensation", U_32, 1, NULL, 0},
    {15, "warp_grid_table_path", EXT_PATH, 1, NULL, 0},
};

#define WarpCompensationDzoomInternalParams_Count 11
static PARAM_s WarpCompensationDzoomInternalParams[WarpCompensationDzoomInternalParams_Count] = { //FIX ME
    // Warp Part
    {0, "enable", U_32, 1, NULL, 0},
    {1, "grid_array_width", U_8, 1, NULL, 0},
    {2, "grid_array_height", U_8, 1, NULL, 0},
    {3, "horz_grid_spacing_exponent", U_8, 1, NULL, 0},
    {4, "vert_grid_spacing_exponent", U_8, 1, NULL, 0},
    {5, "vert_warp_grid_array_width", U_8, 1, NULL, 0},
    {6, "vert_warp_grid_array_height", U_8, 1, NULL, 0},
    {7, "vert_warp_horz_grid_spacing_exponent", U_8, 1, NULL, 0},
    {8, "vert_warp_vert_grid_spacing_exponent", U_8, 1, NULL, 0},
    {9, "warp_horizontal_table_path", EXT_PATH, 1, NULL, 0},
    {10, "warp_vertical_table_path", EXT_PATH, 1, NULL, 0},
};

#define DzoomParams_Count 5
static PARAM_s DzoomParams[DzoomParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "shiftx", S_32, 1, NULL, 0},
    {2, "shifty", S_32, 1, NULL, 0},
    {3, "zoomx", U_32, 1, NULL, 0},
    {4, "zoomy", U_32, 1, NULL, 0},
};

#define DummyParams_Count 5
static PARAM_s DummyParams[DummyParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "left", U_32, 1, NULL, 0},
    {2, "right", U_32, 1, NULL, 0},
    {3, "top", U_32, 1, NULL, 0},
    {4, "bottom", U_32, 1, NULL, 0},
};

#define ActiveParams_Count 5
static PARAM_s ActiveParams[ActiveParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "startx", U_32, 1, NULL, 0},
    {2, "starty", U_32, 1, NULL, 0},
    {3, "width",  U_32, 1, NULL, 0},
    {4, "height", U_32, 1, NULL, 0},
};

#define ChromaAberrationParams_Count 16
static PARAM_s ChromaAberrationParams[ChromaAberrationParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "calib_version", U_32, 1, NULL, 0},
    {2, "horizontal_grid_number", S_32, 1, NULL, 0},
    {3, "vertical_grid_number", S_32, 1, NULL, 0},
    {4, "tile_width_exponent", S_32, 1, NULL, 0},
    {5, "tile_height_exponent", S_32, 1, NULL, 0},
    {6, "vin_sensor_start_x", U_32, 1, NULL, 0},
    {7, "vin_sensor_start_y", U_32, 1, NULL, 0},
    {8, "vin_sensor_width", U_32, 1, NULL, 0},
    {9, "vin_sensor_height", U_32, 1, NULL, 0},
    {10, "vin_sensor_h_subsample_factor_num", U_32, 1, NULL, 0},
    {11, "vin_sensor_h_subsample_factor_den", U_32, 1, NULL, 0},
    {12, "vin_sensor_v_subsample_factor_num", U_32, 1, NULL, 0},
    {13, "vin_sensor_v_subsample_factor_den", U_32, 1, NULL, 0},
    {14, "red_ca_grid_table_path", EXT_PATH, 1, NULL, 0},
    {15, "blue_ca_grid_table_path", EXT_PATH, 1, NULL, 0},
};

#define ChromaAberrationInternalParams_Count 14
static PARAM_s ChromaAberrationInternalParams[ChromaAberrationInternalParams_Count] = {
    {0, "horz_warp_enable", U_32, 1, NULL, 0},
    {1, "vert_warp_enable", U_32, 1, NULL, 0},
    {2, "horz_pass_grid_array_width", U_32, 1, NULL, 0},
    {3, "horz_pass_grid_array_height", U_32, 1, NULL, 0},
    {4, "horz_pass_horz_grid_spacing_exponent", U_32, 1, NULL, 0},
    {5, "horz_pass_vert_grid_spacing_exponent", U_32, 1, NULL, 0},
    {6, "vert_pass_grid_array_width", U_32, 1, NULL, 0},
    {7, "vert_pass_grid_array_height", U_32, 1, NULL, 0},
    {8, "vert_pass_horz_grid_spacing_exponent", U_32, 1, NULL, 0},
    {9, "vert_pass_vert_grid_spacing_exponent", U_32, 1, NULL, 0},
    {10, "warp_horz_table_path_red", EXT_PATH, 1, NULL, 0},
    {11, "warp_vert_table_path_red", EXT_PATH, 1, NULL, 0},
    {12, "warp_horz_table_path_blue", EXT_PATH, 1, NULL, 0},
    {13, "warp_vert_table_path_blue", EXT_PATH, 1, NULL, 0},
};

#define SensorInputParams_Count 4
static PARAM_s SensorInputParams[SensorInputParams_Count] = {
    {0, "sensor_id", U_32, 1, NULL, 0},
    {1, "bayer_pattern", U_32, 1, NULL, 0},
    {2, "sensor_resolution", U_32, 1, NULL, 0},
    {3, "readout_mode", U_32, 1, NULL, 0},
};

#define StaticBlackLevelParams_Count 6
static PARAM_s StaticBlackLevelParams[StaticBlackLevelParams_Count] = {
    {0, "r_black", S_32, 1, NULL, 0},
    {1, "g_r_black", S_32, 1, NULL, 0},
    {2, "g_b_black", S_32, 1, NULL, 0},
    {3, "b_black", S_32, 1, NULL, 0},
    {4, "g_black", S_32, 1, NULL, 0},
    {5, "ir_black", S_32, 1, NULL, 0},
};

#define DeferredBlackLevelParams_Count 1
static PARAM_s DeferredBlackLevelParams[DeferredBlackLevelParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
};

#define DynamicBadPixelCorrectionParams_Count 6
static PARAM_s DynamicBadPixelCorrectionParams[DynamicBadPixelCorrectionParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "hot_pixel_detection_strength", U_32, 1, NULL, 0},
    {2, "dark_pixel_detection_strength", U_32, 1, NULL, 0},
    {3, "correction_method", U_32, 1, NULL, 0},
    {4, "hot_pixel_detection_strength_irmode_redblue", U_32, 1, NULL, 0},
    {5, "dark_pixel_detection_strength_irmode_redblue", U_32, 1, NULL, 0},
};

#define CfaLeakageFilterParams_Count 6
static PARAM_s CfaLeakageFilterParams[CfaLeakageFilterParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "alpha_rr", S_32, 1, NULL, 0},
    {2, "alpha_rb", S_32, 1, NULL, 0},
    {3, "alpha_br", S_32, 1, NULL, 0},
    {4, "alpha_bb", S_32, 1, NULL, 0},
    {5, "saturation_level", U_32, 1, NULL, 0},
};

#define CfaNoiseFilterParams_Count 53
static PARAM_s CfaNoiseFilterParams[CfaNoiseFilterParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "min_noise_level_red", U_32, 1, NULL, 0},
    {2, "min_noise_level_green", U_32, 1, NULL, 0},
    {3, "min_noise_level_blue", U_32, 1, NULL, 0},
    {4, "max_noise_level_red", U_32, 1, NULL, 0},
    {5, "max_noise_level_green", U_32, 1, NULL, 0},
    {6, "max_noise_level_blue", U_32, 1, NULL, 0},
    {7, "original_blend_strength_red", U_32, 1, NULL, 0},
    {8, "original_blend_strength_green", U_32, 1, NULL, 0},
    {9, "original_blend_strength_blue", U_32, 1, NULL, 0},
    {10, "extent_regular_red", U_32, 1, NULL, 0},
    {11, "extent_regular_green", U_32, 1, NULL, 0},
    {12, "extent_regular_blue", U_32, 1, NULL, 0},
    {13, "extent_fine_red", U_32, 1, NULL, 0},
    {14, "extent_fine_green", U_32, 1, NULL, 0},
    {15, "extent_fine_blue", U_32, 1, NULL, 0},
    {16, "strength_fine_red", U_32, 1, NULL, 0},
    {17, "strength_fine_green", U_32, 1, NULL, 0},
    {18, "strength_fine_blue", U_32, 1, NULL, 0},
    {19, "selectivity_regular", U_32, 1, NULL, 0},
    {20, "selectivity_fine", U_32, 1, NULL, 0},
    {21, "directional_enable", U_32, 1, NULL, 0},
    {22, "directional_horvert_edge_strength", U_32, 1, NULL, 0},
    {23, "directional_horvert_edge_strength_bias", U_32, 1, NULL, 0},
    {24, "directional_horvert_original_blend_strength", U_32, 1, NULL, 0},
    {25, "dir_lev_max_red", U_32, 1, NULL, 0},
    {26, "dir_lev_max_green", U_32, 1, NULL, 0},
    {27, "dir_lev_max_blue", U_32, 1, NULL, 0},
    {28, "dir_lev_min_red", U_32, 1, NULL, 0},
    {29, "dir_lev_min_green", U_32, 1, NULL, 0},
    {30, "dir_lev_min_blue", U_32, 1, NULL, 0},
    {31, "dir_lev_mul_red", U_32, 1, NULL, 0},
    {32, "dir_lev_mul_green", U_32, 1, NULL, 0},
    {33, "dir_lev_mul_blue", U_32, 1, NULL, 0},
    {34, "dir_lev_offset_red", S_32, 1, NULL, 0},
    {35, "dir_lev_offset_green", S_32, 1, NULL, 0},
    {36, "dir_lev_offset_blue", S_32, 1, NULL, 0},
    {37, "dir_lev_shift_red", U_32, 1, NULL, 0},
    {38, "dir_lev_shift_green", U_32, 1, NULL, 0},
    {39, "dir_lev_shift_blue", U_32, 1, NULL, 0},
    {40, "lev_mul_red", U_32, 1, NULL, 0},
    {41, "lev_mul_green", U_32, 1, NULL, 0},
    {42, "lev_mul_blue", U_32, 1, NULL, 0},
    {43, "lev_offset_red", S_32, 1, NULL, 0},
    {44, "lev_offset_green", S_32, 1, NULL, 0},
    {45, "lev_offset_blue", S_32, 1, NULL, 0},
    {46, "lev_shift_red", U_32, 1, NULL, 0},
    {47, "lev_shift_green", U_32, 1, NULL, 0},
    {48, "lev_shift_blue", U_32, 1, NULL, 0},
    {49, "apply_to_color_diff_red", U_32, 1, NULL, 0},
    {50, "apply_to_color_diff_blue", U_32, 1, NULL, 0},
    {51, "opt_for_rccb_red", U_32, 1, NULL, 0},
    {52, "opt_for_rccb_blue", U_32, 1, NULL, 0},
};

#define AntiAliasingParams_Count 3
static PARAM_s AntiAliasingParams[AntiAliasingParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "thresh", U_32, 1, NULL, 0},
    {2, "log_fractional_correct", U_32, 1, NULL, 0},
};

#define WbGainParams_Count 3
static PARAM_s WbGainParams[WbGainParams_Count] = {
    {0, "r_gain", U_32, 1, NULL, 0},
    {1, "g_gain", U_32, 1, NULL, 0},
    {2, "b_gain", U_32, 1, NULL, 0},
};

#define StoredIrParams_Count 2
static PARAM_s StoredIrParams[StoredIrParams_Count] = {
    {0, "ir_shift", U_32, 1, NULL, 0},
    {1, "ir_tone_curve", U_32, IK_NUM_TONE_CURVE, NULL, 0},
};

#define ColorCorrectionParams_Count 4
static PARAM_s ColorCorrectionParams[ColorCorrectionParams_Count] = {
    {0, "three_d_path", EXT_PATH, 1, NULL, 0},
    {1, "matrix_enable", U_8, 1, NULL, 0},
    {2, "matrix", S_32, 6, NULL, 0},
    {3, "matrix_shift_minus_8", U_8, 1, NULL, 0},

};

#define ToneCurveParams_Count 3
static PARAM_s ToneCurveParams[ToneCurveParams_Count] = {
    {0, "curve_red", U_32, IK_NUM_TONE_CURVE, NULL, 0},
    {1, "curve_green", U_32, IK_NUM_TONE_CURVE, NULL, 0},
    {2, "curve_blue", U_32, IK_NUM_TONE_CURVE, NULL, 0},
};

#define FrontEndToneCurveParams_Count 4
static PARAM_s FrontEndToneCurveParams[FrontEndToneCurveParams_Count] = {
    {0, "compand_enable", U_32, 1, NULL, 0},
    {1, "compand_table", U_16, 45, NULL, 0},
    {2, "decompand_enable", U_32, 1, NULL, 0},
    {3, "decompand_table", U_32, 145, NULL, 0},
};

#define RgbToYuvMatrixParams_Count 12
static PARAM_s RgbToYuvMatrixParams[RgbToYuvMatrixParams_Count] = {
    {0, "m11", S_32, 1, NULL, 0},
    {1, "m12", S_32, 1, NULL, 0},
    {2, "m13", S_32, 1, NULL, 0},
    {3, "m21", S_32, 1, NULL, 0},
    {4, "m22", S_32, 1, NULL, 0},
    {5, "m23", S_32, 1, NULL, 0},
    {6, "m31", S_32, 1, NULL, 0},
    {7, "m32", S_32, 1, NULL, 0},
    {8, "m33", S_32, 1, NULL, 0},
    {9, "y_offset", S_32, 1, NULL, 0},
    {10, "u_offset", S_32, 1, NULL, 0},
    {11, "v_offset", S_32, 1, NULL, 0},
};

#define RgbIrParams_Count 10
static PARAM_s RgbIrParams[RgbIrParams_Count] = {
    {0, "mode", U_32, 1, NULL, 0},
    {1, "ircorrect_offset_b", S_32, 1, NULL, 0},
    {2, "ircorrect_offset_gb", S_32, 1, NULL, 0},
    {3, "ircorrect_offset_gr", S_32, 1, NULL, 0},
    {4, "ircorrect_offset_r", S_32, 1, NULL, 0},
    {5, "mul_base_val", U_32, 1, NULL, 0},
    {6, "mul_delta_high", U_32, 1, NULL, 0},
    {7, "mul_high", U_32, 1, NULL, 0},
    {8, "mul_high_val", U_32, 1, NULL, 0},
    {9, "ir_only", U_32, 1, NULL, 0}
};

#define ChromaScaleParams_Count 2
static PARAM_s ChromaScaleParams[ChromaScaleParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "gain_curve", U_32, IK_NUM_CHROMA_GAIN_CURVE, NULL, 0},
};

#define ChromaMedianFilterParams_Count 7
static PARAM_s ChromaMedianFilterParams[ChromaMedianFilterParams_Count] = {
    {0, "enable", S_32, 1, NULL, 0},
    {1, "cb_adaptive_strength", U_32, 1, NULL, 0},
    {2, "cr_adaptive_strength", U_32, 1, NULL, 0},
    {3, "cb_non_adaptive_strength", U_32, 1, NULL, 0},
    {4, "cr_non_adaptive_strength", U_32, 1, NULL, 0},
    {5, "cb_adaptive_amount", U_32, 1, NULL, 0},
    {6, "cr_adaptive_amount", U_32, 1, NULL, 0},
};

#define DemosaicFilterParams_Count 7
static PARAM_s DemosaicFilterParams[DemosaicFilterParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "activity_thresh", U_32, 1, NULL, 0},
    {2, "activity_difference_thresh", U_32, 1, NULL, 0},
    {3, "grad_clip_thresh", U_32, 1, NULL, 0},
    {4, "grad_noise_thresh", U_32, 1, NULL, 0},
    {5, "alias_interpolation_strength", U_32, 1, NULL, 0},
    {6, "alias_interpolation_thresh", U_32, 1, NULL, 0},
};

#define RgbToY12Params_Count 5
static PARAM_s RgbToY12Params[RgbToY12Params_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "m11", S_32, 1, NULL, 0},
    {2, "m12", S_32, 1, NULL, 0},
    {3, "m13", S_32, 1, NULL, 0},
    {4, "y_offset", S_32, 1, NULL, 0},
};

#define GbGrMismatchParams_Count 4
static PARAM_s GbGrMismatchParams[] = {
    {0, "narrow_enable", U_32, 1, NULL, 0},
    {1, "wide_enable", U_32, 1, NULL, 0},
    {2, "wide_safety", U_32, 1, NULL, 0},
    {3, "wide_thresh", U_32, 1, NULL, 0},
};

#define SharpeningBothParams_Count 6
static PARAM_s SharpeningBothParams[SharpeningBothParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "mode", U_32, 1, NULL, 0},
    {2, "edge_thresh", U_32, 1, NULL, 0},
    {3, "wide_edge_detect", U_32, 1, NULL, 0},
    {4, "max_change_up5x5", U_32, 1, NULL, 0},
    {5, "max_change_down5x5", U_32, 1, NULL, 0},
};

#define FinalSharpeningBothParams_Count 19
static PARAM_s FinalSharpeningBothParams[FinalSharpeningBothParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "mode", U_32, 1, NULL, 0},
    {2, "edge_thresh", U_32, 1, NULL, 0},
    {3, "wide_edge_detect", U_32, 1, NULL, 0},
    {4, "max_change_up5x5", U_32, 1, NULL, 0},
    {5, "max_change_down5x5", U_32, 1, NULL, 0},
    {6, "three_d_table", U_8, 8192, NULL, 0},
    {7, "three_d_u_bits", U_8, 1, NULL, 0},
    {8, "three_d_u_offset", U_8, 1, NULL, 0},
    {9, "three_d_u_shift", U_8, 1, NULL, 0},
    {10, "three_d_v_bits", U_8, 1, NULL, 0},
    {11, "three_d_v_offset", U_8, 1, NULL, 0},
    {12, "three_d_v_shift", U_8, 1, NULL, 0},
    {13, "three_d_y_offset", U_8, 1, NULL, 0},
    {14, "three_d_y_shift", U_8, 1, NULL, 0},
    {15, "max_change_up", U_32, 1, NULL, 0},
    {16, "max_change_down", U_32, 1, NULL, 0},
    {17, "narrow_nonsmooth_detect_sub", U_32, 17, NULL, 0},
    {18, "narrow_nonsmooth_detect_shift", U_32, 1, NULL, 0},
};

#define SharpeningNoiseParams_Count 21
static PARAM_s SharpeningNoiseParams[SharpeningNoiseParams_Count] = {
    { 0, "fir_specify", U_32, 1, NULL, 0},
    { 1, "fir_strength_iso", U_32, 1, NULL, 0},
    { 2, "fir_strength_dir", U_32, 1, NULL, 0},
    { 3, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    { 4, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    { 5, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    { 6, "fir_coefs", S_32, 225, NULL, 0},
    { 7, "max_change_down", U_32, 1, NULL, 0},
    { 8, "max_change_up", U_32, 1, NULL, 0},
    { 9, "level_str_adjust_high", U_32, 1, NULL, 0},
    { 10, "level_str_adjust_high_delta", U_32, 1, NULL, 0},
    { 11, "level_str_adjust_high_strength", U_32, 1, NULL, 0},
    { 12, "level_str_adjust_mid_strength", U_32, 1, NULL, 0},
    { 13, "level_str_adjust_low", U_32, 1, NULL, 0},
    { 14, "level_str_adjust_low_delta", U_32, 1, NULL, 0},
    { 15, "level_str_adjust_low_strength", U_32, 1, NULL, 0},
    { 16, "level_str_adjust_not_T0T1_level_based", U_32, 1, NULL, 0},
    { 17, "T0", U_32, 1, NULL, 0},
    { 18, "T1", U_32, 1, NULL, 0},
    { 19, "alpha_min", U_32, 1, NULL, 0},
    { 20, "alpha_max", U_32, 1, NULL, 0},
};

#define FinalSharpeningNoiseParams_Count 34
static PARAM_s FinalSharpeningNoiseParams[FinalSharpeningNoiseParams_Count] = {//FIX ME
    { 0, "fir_specify", U_32, 1, NULL, 0},
    { 1, "fir_strength_iso", U_32, 1, NULL, 0},
    { 2, "fir_strength_dir", U_32, 1, NULL, 0},
    { 3, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    { 4, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    { 5, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    { 6, "fir_coefs", S_32, 225, NULL, 0},
    { 7, "max_change_down", U_32, 1, NULL, 0},
    { 8, "max_change_up", U_32, 1, NULL, 0},
    { 9, "level_str_adjust_high", U_32, 1, NULL, 0},
    { 10, "level_str_adjust_high_delta", U_32, 1, NULL, 0},
    { 11, "level_str_adjust_high_strength", U_32, 1, NULL, 0},
    { 12, "level_str_adjust_mid_strength", U_32, 1, NULL, 0},
    { 13, "level_str_adjust_low", U_32, 1, NULL, 0},
    { 14, "level_str_adjust_low_delta", U_32, 1, NULL, 0},
    { 15, "level_str_adjust_low_strength", U_32, 1, NULL, 0},
    { 16, "level_str_adjust_not_T0T1_level_based", U_32, 1, NULL, 0},
    { 17, "T0", U_32, 1, NULL, 0},
    { 18, "T1", U_32, 1, NULL, 0},
    { 19, "alpha_min", U_32, 1, NULL, 0},
    { 20, "alpha_max", U_32, 1, NULL, 0},
    { 21, "level_str_adjust_method", U_32, 1, NULL, 0},
    { 22, "directional_decide_t0", U_32, 1, NULL, 0},
    { 23, "directional_decide_t1", U_32, 1, NULL, 0},
    { 24, "advanced_enable", U_32, 1, NULL, 0},
    { 25, "advanced_iso_max_change_method", U_32, 1, NULL, 0},
    { 26, "advanced_iso_max_change_table", U_32, 32, NULL, 0},
    { 27, "advanced_iso_noise_level_method", U_32, 1, NULL, 0},
    { 28, "advanced_iso_noise_level_table", U_32, 32, NULL, 0},
    { 29, "advanced_iso_size", U_32, 1, NULL, 0},
    { 30, "advanced_iso_str_a_method", U_32, 1, NULL, 0},
    { 31, "advanced_iso_str_a_table", U_32, 32, NULL, 0},
    { 32, "advanced_iso_str_b_method", U_32, 1, NULL, 0},
    { 33, "advanced_iso_str_b_table", U_32, 32, NULL, 0},
};

#define FirstSharpeningFirParams_Count 7
static PARAM_s FirstSharpeningFirParams[FirstSharpeningFirParams_Count] = {
    {0, "fir_specify", U_32, 1, NULL, 0},
    {1, "fir_strength_iso", U_32, 1, NULL, 0},
    {2, "fir_strength_dir", U_32, 1, NULL, 0},
    {3, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    {4, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    {5, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    {6, "fir_coefs", S_32, 225, NULL, 0},
};

#define FinalSharpeningFirParams_Count 13
static PARAM_s FinalSharpeningFirParams[FinalSharpeningFirParams_Count] = {
    {0, "fir_specify", U_32, 1, NULL, 0},
    {1, "fir_strength_iso", U_32, 1, NULL, 0},
    {2, "fir_strength_dir", U_32, 1, NULL, 0},
    {3, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    {4, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    {5, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    {6, "fir_coefs", S_32, 225, NULL, 0},
    {7, "pos_fir_artifact_reduce_enable", U_32, 1, NULL, 0},
    {8, "pos_fir_artifact_reduce_strength", U_32, 1, NULL, 0},
    {9, "over_shoot_reduce_allow", U_32, 1, NULL, 0},
    {10, "over_shoot_reduce_strength", U_32, 1, NULL, 0},
    {11, "under_shoot_reduce_allow", U_32, 1, NULL, 0},
    {12, "under_shoot_reduce_strength", U_32, 1, NULL, 0},
};

#define SharpeningCoringParams_Count 2
static PARAM_s SharpeningCoringParams[SharpeningCoringParams_Count] = {
    {0, "coring_table", U_32, IK_NUM_CORING_TABLE_INDEX, NULL, 0},
    {1, "fractional_bits", U_32, 1, NULL, 0},
};

#define FianlSharpeningCoringParams_Count 2
static PARAM_s FianlSharpeningCoringParams[FianlSharpeningCoringParams_Count] = {
    {0, "coring_table", U_32, IK_NUM_CORING_TABLE_INDEX, NULL, 0},
    {1, "fractional_bits", U_32, 1, NULL, 0},
};

#define SharpeningLevelParams_Count 7
static PARAM_s SharpeningLevelParams[SharpeningLevelParams_Count] = {
    {0, "high", U_32, 1, NULL, 0},
    {1, "high_delta", U_32, 1, NULL, 0},
    {2, "high_strength", U_32, 1, NULL, 0},
    {3, "mid_strength", U_32, 1, NULL, 0},
    {4, "low", U_32, 1, NULL, 0},
    {5, "low_delta", U_32, 1, NULL, 0},
    {6, "low_strength", U_32, 1, NULL, 0},
};

#define FinalSharpeningLevelParams_Count 8
static PARAM_s FinalSharpeningLevelParams[FinalSharpeningLevelParams_Count] = {
    {0, "high", U_32, 1, NULL, 0},
    {1, "high_delta", U_32, 1, NULL, 0},
    {2, "high_strength", U_32, 1, NULL, 0},
    {3, "mid_strength", U_32, 1, NULL, 0},
    {4, "low", U_32, 1, NULL, 0},
    {5, "low_delta", U_32, 1, NULL, 0},
    {6, "low_strength", U_32, 1, NULL, 0},
    {7, "method", U_32, 1, NULL, 0},
};

#define VideoMctfParams_Count 295
static PARAM_s VideoMctfParams[VideoMctfParams_Count] = {
    {0,  "enable", U_32, 1, NULL, 0},
    {1,  "y_narrow_nonsmooth_detect_shift", U_32, 1, NULL, 0},
    {2,  "y_narrow_nonsmooth_detect_sub", U_32, 17, NULL, 0},
    {3,  "use_level_based_ta", U_32, 1, NULL, 0},
    /*spat_blend*/
    {4,  "y_spat_blend_high", U_32, 1, NULL, 0},
    {5,  "y_spat_blend_high_delta", U_32, 1, NULL, 0},
    {6,  "y_spat_blend_high_strength", U_32, 1, NULL, 0},
    {7,  "y_spat_blend_low", U_32, 1, NULL, 0},
    {8,  "y_spat_blend_low_delta", U_32, 1, NULL, 0},
    {9,  "y_spat_blend_low_strength", U_32, 1, NULL, 0},
    {10, "y_spat_blend_method", U_32, 1, NULL, 0},
    {11, "y_spat_blend_mid_strength", U_32, 1, NULL, 0},
    {12, "cb_spat_blend_high", U_32, 1, NULL, 0},
    {13, "cb_spat_blend_high_delta", U_32, 1, NULL, 0},
    {14, "cb_spat_blend_high_strength", U_32, 1, NULL, 0},
    {15, "cb_spat_blend_low", U_32, 1, NULL, 0},
    {16, "cb_spat_blend_low_delta", U_32, 1, NULL, 0},
    {17, "cb_spat_blend_low_strength", U_32, 1, NULL, 0},
    {18, "cb_spat_blend_method", U_32, 1, NULL, 0},
    {19, "cb_spat_blend_mid_strength", U_32, 1, NULL, 0},
    {20, "cr_spat_blend_high", U_32, 1, NULL, 0},
    {21, "cr_spat_blend_high_delta", U_32, 1, NULL, 0},
    {22, "cr_spat_blend_high_strength", U_32, 1, NULL, 0},
    {23, "cr_spat_blend_low", U_32, 1, NULL, 0},
    {24, "cr_spat_blend_low_delta", U_32, 1, NULL, 0},
    {25, "cr_spat_blend_low_strength", U_32, 1, NULL, 0},
    {26, "cr_spat_blend_method", U_32, 1, NULL, 0},
    {27, "cr_spat_blend_mid_strength", U_32, 1, NULL, 0},
    /*advanced iso*/
    {28, "y_advanced_iso_max_change_table", U_32, 32, NULL, 0},
    {29, "y_advanced_iso_max_change_method", U_32, 1, NULL, 0},
    {30, "y_advanced_iso_size", U_32, 1, NULL, 0},
    {31, "y_advanced_iso_noise_level", U_32, 1, NULL, 0},
    {32, "y_advanced_iso_enable", U_32, 1, NULL, 0},
    {33, "cb_advanced_iso_max_change_table", U_32, 32, NULL, 0},
    {34, "cb_advanced_iso_max_change_method", U_32, 1, NULL, 0},
    {35, "cb_advanced_iso_size", U_32, 1, NULL, 0},
    {36, "cb_advanced_iso_noise_level", U_32, 1, NULL, 0},
    {37, "cb_advanced_iso_enable", U_32, 1, NULL, 0},
    {38, "cr_advanced_iso_max_change_table", U_32, 32, NULL, 0},
    {39, "cr_advanced_iso_max_change_method", U_32, 1, NULL, 0},
    {40, "cr_advanced_iso_size", U_32, 1, NULL, 0},
    {41, "cr_advanced_iso_noise_level", U_32, 1, NULL, 0},
    {42, "cr_advanced_iso_enable", U_32, 1, NULL, 0},
    /*3d maxchange*/
    {43, "y_overall_max_change_high", U_32, 1, NULL, 0},
    {44, "y_overall_max_change_high_delta", U_32, 1, NULL, 0},
    {45, "y_overall_max_change_high_strength", U_32, 1, NULL, 0},
    {46, "y_overall_max_change_low", U_32, 1, NULL, 0},
    {47, "y_overall_max_change_low_delta", U_32, 1, NULL, 0},
    {48, "y_overall_max_change_low_strength", U_32, 1, NULL, 0},
    {49, "y_overall_max_change_method", U_32, 1, NULL, 0},
    {50, "y_overall_max_change_mid_strength", U_32, 1, NULL, 0},
    {51, "cb_overall_max_change_high", U_32, 1, NULL, 0},
    {52, "cb_overall_max_change_high_delta", U_32, 1, NULL, 0},
    {53, "cb_overall_max_change_high_strength", U_32, 1, NULL, 0},
    {54, "cb_overall_max_change_low", U_32, 1, NULL, 0},
    {55, "cb_overall_max_change_low_delta", U_32, 1, NULL, 0},
    {56, "cb_overall_max_change_low_strength", U_32, 1, NULL, 0},
    {57, "cb_overall_max_change_method", U_32, 1, NULL, 0},
    {58, "cb_overall_max_change_mid_strength", U_32, 1, NULL, 0},
    {59, "cr_overall_max_change_high", U_32, 1, NULL, 0},
    {60, "cr_overall_max_change_high_delta", U_32, 1, NULL, 0},
    {61, "cr_overall_max_change_high_strength", U_32, 1, NULL, 0},
    {62, "cr_overall_max_change_low", U_32, 1, NULL, 0},
    {63, "cr_overall_max_change_low_delta", U_32, 1, NULL, 0},
    {64, "cr_overall_max_change_low_strength", U_32, 1, NULL, 0},
    {65, "cr_overall_max_change_method", U_32, 1, NULL, 0},
    {66, "cr_overall_max_change_mid_strength", U_32, 1, NULL, 0},
    /*overall maxchange*/
    {67, "y_3d_maxchange_high", U_32, 1, NULL, 0},
    {68, "y_3d_maxchange_high_delta", U_32, 1, NULL, 0},
    {69, "y_3d_maxchange_high_strength", U_32, 1, NULL, 0},
    {70, "y_3d_maxchange_low", U_32, 1, NULL, 0},
    {71, "y_3d_maxchange_low_delta", U_32, 1, NULL, 0},
    {72, "y_3d_maxchange_low_strength", U_32, 1, NULL, 0},
    {73, "y_3d_maxchange_method", U_32, 1, NULL, 0},
    {74, "y_3d_maxchange_mid_strength", U_32, 1, NULL, 0},
    {75, "cb_3d_maxchange_high", U_32, 1, NULL, 0},
    {76, "cb_3d_maxchange_high_delta", U_32, 1, NULL, 0},
    {77, "cb_3d_maxchange_high_strength", U_32, 1, NULL, 0},
    {78, "cb_3d_maxchange_low", U_32, 1, NULL, 0},
    {79, "cb_3d_maxchange_low_delta", U_32, 1, NULL, 0},
    {80, "cb_3d_maxchange_low_strength", U_32, 1, NULL, 0},
    {81, "cb_3d_maxchange_method", U_32, 1, NULL, 0},
    {82, "cb_3d_maxchange_mid_strength", U_32, 1, NULL, 0},
    {83, "cr_3d_maxchange_high", U_32, 1, NULL, 0},
    {84, "cr_3d_maxchange_high_delta", U_32, 1, NULL, 0},
    {85, "cr_3d_maxchange_high_strength", U_32, 1, NULL, 0},
    {86, "cr_3d_maxchange_low", U_32, 1, NULL, 0},
    {87, "cr_3d_maxchange_low_delta", U_32, 1, NULL, 0},
    {88, "cr_3d_maxchange_low_strength", U_32, 1, NULL, 0},
    {89, "cr_3d_maxchange_method", U_32, 1, NULL, 0},
    {90, "cr_3d_maxchange_mid_strength", U_32, 1, NULL, 0},
    /*spat_filt_max_smth_change*/
    {91, "y_spat_filt_max_smth_change_high", U_32, 1, NULL, 0},
    {92, "y_spat_filt_max_smth_change_high_delta", U_32, 1, NULL, 0},
    {93, "y_spat_filt_max_smth_change_high_strength", U_32, 1, NULL, 0},
    {94, "y_spat_filt_max_smth_change_low", U_32, 1, NULL, 0},
    {95, "y_spat_filt_max_smth_change_low_delta", U_32, 1, NULL, 0},
    {96, "y_spat_filt_max_smth_change_low_strength", U_32, 1, NULL, 0},
    {97, "y_spat_filt_max_smth_change_method", U_32, 1, NULL, 0},
    {98, "y_spat_filt_max_smth_change_mid_strength", U_32, 1, NULL, 0},
    {99, "cb_spat_filt_max_smth_change_high", U_32, 1, NULL, 0},
    {100, "cb_spat_filt_max_smth_change_high_delta", U_32, 1, NULL, 0},
    {101, "cb_spat_filt_max_smth_change_high_strength", U_32, 1, NULL, 0},
    {102, "cb_spat_filt_max_smth_change_low", U_32, 1, NULL, 0},
    {103, "cb_spat_filt_max_smth_change_low_delta", U_32, 1, NULL, 0},
    {104, "cb_spat_filt_max_smth_change_low_strength", U_32, 1, NULL, 0},
    {105, "cb_spat_filt_max_smth_change_method", U_32, 1, NULL, 0},
    {106, "cb_spat_filt_max_smth_change_mid_strength", U_32, 1, NULL, 0},
    {107, "cr_spat_filt_max_smth_change_high", U_32, 1, NULL, 0},
    {108, "cr_spat_filt_max_smth_change_high_delta", U_32, 1, NULL, 0},
    {109, "cr_spat_filt_max_smth_change_high_strength", U_32, 1, NULL, 0},
    {110, "cr_spat_filt_max_smth_change_low", U_32, 1, NULL, 0},
    {111, "cr_spat_filt_max_smth_change_low_delta", U_32, 1, NULL, 0},
    {112, "cr_spat_filt_max_smth_change_low_strength", U_32, 1, NULL, 0},
    {113, "cr_spat_filt_max_smth_change_method", U_32, 1, NULL, 0},
    {114, "cr_spat_filt_max_smth_change_mid_strength", U_32, 1, NULL, 0},
    /*spat_smth_dir*/
    {115, "y_spat_smth_wide_edge_detect", U_32, 1, NULL, 0},
    {116, "y_spat_smth_edge_thresh", U_32, 1, NULL, 0},
    {117, "y_spat_smth_dir_high", U_32, 1, NULL, 0},
    {118, "y_spat_smth_dir_high_delta", U_32, 1, NULL, 0},
    {119, "y_spat_smth_dir_high_strength", U_32, 1, NULL, 0},
    {120, "y_spat_smth_dir_low", U_32, 1, NULL, 0},
    {121, "y_spat_smth_dir_low_delta", U_32, 1, NULL, 0},
    {122, "y_spat_smth_dir_low_strength", U_32, 1, NULL, 0},
    {123, "y_spat_smth_dir_method", U_32, 1, NULL, 0},
    {124, "y_spat_smth_dir_mid_strength", U_32, 1, NULL, 0},
    {125, "cb_spat_smth_wide_edge_detect", U_32, 1, NULL, 0},
    {126, "cb_spat_smth_edge_thresh", U_32, 1, NULL, 0},
    {127, "cb_spat_smth_dir_high", U_32, 1, NULL, 0},
    {128, "cb_spat_smth_dir_high_delta", U_32, 1, NULL, 0},
    {129, "cb_spat_smth_dir_high_strength", U_32, 1, NULL, 0},
    {130, "cb_spat_smth_dir_low", U_32, 1, NULL, 0},
    {131, "cb_spat_smth_dir_low_delta", U_32, 1, NULL, 0},
    {132, "cb_spat_smth_dir_low_strength", U_32, 1, NULL, 0},
    {133, "cb_spat_smth_dir_method", U_32, 1, NULL, 0},
    {134, "cb_spat_smth_dir_mid_strength", U_32, 1, NULL, 0},
    {135, "cr_spat_smth_wide_edge_detect", U_32, 1, NULL, 0},
    {136, "cr_spat_smth_edge_thresh", U_32, 1, NULL, 0},
    {137, "cr_spat_smth_dir_high", U_32, 1, NULL, 0},
    {138, "cr_spat_smth_dir_high_delta", U_32, 1, NULL, 0},
    {139, "cr_spat_smth_dir_high_strength", U_32, 1, NULL, 0},
    {140, "cr_spat_smth_dir_low", U_32, 1, NULL, 0},
    {141, "cr_spat_smth_dir_low_delta", U_32, 1, NULL, 0},
    {142, "cr_spat_smth_dir_low_strength", U_32, 1, NULL, 0},
    {143, "cr_spat_smth_dir_method", U_32, 1, NULL, 0},
    {144, "cr_spat_smth_dir_mid_strength", U_32, 1, NULL, 0},
    /*spat_smth_iso*/
    {145, "y_spat_smth_iso_high", U_32, 1, NULL, 0},
    {146, "y_spat_smth_iso_high_delta", U_32, 1, NULL, 0},
    {147, "y_spat_smth_iso_high_strength", U_32, 1, NULL, 0},
    {148, "y_spat_smth_iso_low", U_32, 1, NULL, 0},
    {149, "y_spat_smth_iso_low_delta", U_32, 1, NULL, 0},
    {150, "y_spat_smth_iso_low_strength", U_32, 1, NULL, 0},
    {151, "y_spat_smth_iso_method", U_32, 1, NULL, 0},
    {152, "y_spat_smth_iso_mid_strength", U_32, 1, NULL, 0},
    {153, "cb_spat_smth_iso_high", U_32, 1, NULL, 0},
    {154, "cb_spat_smth_iso_high_delta", U_32, 1, NULL, 0},
    {155, "cb_spat_smth_iso_high_strength", U_32, 1, NULL, 0},
    {156, "cb_spat_smth_iso_low", U_32, 1, NULL, 0},
    {157, "cb_spat_smth_iso_low_delta", U_32, 1, NULL, 0},
    {158, "cb_spat_smth_iso_low_strength", U_32, 1, NULL, 0},
    {159, "cb_spat_smth_iso_method", U_32, 1, NULL, 0},
    {160, "cb_spat_smth_iso_mid_strength", U_32, 1, NULL, 0},
    {161, "cr_spat_smth_iso_high", U_32, 1, NULL, 0},
    {162, "cr_spat_smth_iso_high_delta", U_32, 1, NULL, 0},
    {163, "cr_spat_smth_iso_high_strength", U_32, 1, NULL, 0},
    {164, "cr_spat_smth_iso_low", U_32, 1, NULL, 0},
    {165, "cr_spat_smth_iso_low_delta", U_32, 1, NULL, 0},
    {166, "cr_spat_smth_iso_low_strength", U_32, 1, NULL, 0},
    {167, "cr_spat_smth_iso_method", U_32, 1, NULL, 0},
    {168, "cr_spat_smth_iso_mid_strength", U_32, 1, NULL, 0},
    /*level_based_ta*/
    {169, "y_level_based_ta_high", U_32, 1, NULL, 0},
    {170, "y_level_based_ta_high_delta", U_32, 1, NULL, 0},
    {171, "y_level_based_ta_high_strength", U_32, 1, NULL, 0},
    {172, "y_level_based_ta_low", U_32, 1, NULL, 0},
    {173, "y_level_based_ta_low_delta", U_32, 1, NULL, 0},
    {174, "y_level_based_ta_low_strength", U_32, 1, NULL, 0},
    {175, "y_level_based_ta_method", U_32, 1, NULL, 0},
    {176, "y_level_based_ta_mid_strength", U_32, 1, NULL, 0},
    {177, "cb_level_based_ta_high", U_32, 1, NULL, 0},
    {178, "cb_level_based_ta_high_delta", U_32, 1, NULL, 0},
    {179, "cb_level_based_ta_high_strength", U_32, 1, NULL, 0},
    {180, "cb_level_based_ta_low", U_32, 1, NULL, 0},
    {181, "cb_level_based_ta_low_delta", U_32, 1, NULL, 0},
    {182, "cb_level_based_ta_low_strength", U_32, 1, NULL, 0},
    {183, "cb_level_based_ta_method", U_32, 1, NULL, 0},
    {184, "cb_level_based_ta_mid_strength", U_32, 1, NULL, 0},
    {185, "cr_level_based_ta_high", U_32, 1, NULL, 0},
    {186, "cr_level_based_ta_high_delta", U_32, 1, NULL, 0},
    {187, "cr_level_based_ta_high_strength", U_32, 1, NULL, 0},
    {188, "cr_level_based_ta_low", U_32, 1, NULL, 0},
    {189, "cr_level_based_ta_low_delta", U_32, 1, NULL, 0},
    {190, "cr_level_based_ta_low_strength", U_32, 1, NULL, 0},
    {191, "cr_level_based_ta_method", U_32, 1, NULL, 0},
    {192, "cr_level_based_ta_mid_strength", U_32, 1, NULL, 0},
    /*93mporal_min_target*/
    {193, "y_temporal_alpha0", U_32, 1, NULL, 0},
    {194, "y_temporal_alpha1", U_32, 1, NULL, 0},
    {195, "y_temporal_alpha2", U_32, 1, NULL, 0},
    {196, "y_temporal_alpha3", U_32, 1, NULL, 0},
    {197, "y_temporal_T0", U_32, 1, NULL, 0},
    {198, "y_temporal_T1", U_32, 1, NULL, 0},
    {199, "y_temporal_T2", U_32, 1, NULL, 0},
    {200, "y_temporal_T3", U_32, 1, NULL, 0},
    {201, "y_temporal_maxchange", U_32, 1, NULL, 0},
    {202, "y_temporal_min_target_high", U_32, 1, NULL, 0},
    {203, "y_temporal_min_target_high_delta", U_32, 1, NULL, 0},
    {204, "y_temporal_min_target_high_strength", U_32, 1, NULL, 0},
    {205, "y_temporal_min_target_low", U_32, 1, NULL, 0},
    {206, "y_temporal_min_target_low_delta", U_32, 1, NULL, 0},
    {207, "y_temporal_min_target_low_strength", U_32, 1, NULL, 0},
    {208, "y_temporal_min_target_method", U_32, 1, NULL, 0},
    {209, "y_temporal_min_target_mid_strength", U_32, 1, NULL, 0},
    {210, "cb_temporal_alpha0", U_32, 1, NULL, 0},
    {211, "cb_temporal_alpha1", U_32, 1, NULL, 0},
    {212, "cb_temporal_alpha2", U_32, 1, NULL, 0},
    {213, "cb_temporal_alpha3", U_32, 1, NULL, 0},
    {214, "cb_temporal_T0", U_32, 1, NULL, 0},
    {215, "cb_temporal_T1", U_32, 1, NULL, 0},
    {216, "cb_temporal_T2", U_32, 1, NULL, 0},
    {217, "cb_temporal_T3", U_32, 1, NULL, 0},
    {218, "cb_temporal_maxchange", U_32, 1, NULL, 0},
    {219, "cb_temporal_min_target_high", U_32, 1, NULL, 0},
    {220, "cb_temporal_min_target_high_delta", U_32, 1, NULL, 0},
    {221, "cb_temporal_min_target_high_strength", U_32, 1, NULL, 0},
    {222, "cb_temporal_min_target_low", U_32, 1, NULL, 0},
    {223, "cb_temporal_min_target_low_delta", U_32, 1, NULL, 0},
    {224, "cb_temporal_min_target_low_strength", U_32, 1, NULL, 0},
    {225, "cb_temporal_min_target_method", U_32, 1, NULL, 0},
    {226, "cb_temporal_min_target_mid_strength", U_32, 1, NULL, 0},
    {227, "cr_temporal_alpha0", U_32, 1, NULL, 0},
    {228, "cr_temporal_alpha1", U_32, 1, NULL, 0},
    {229, "cr_temporal_alpha2", U_32, 1, NULL, 0},
    {230, "cr_temporal_alpha3", U_32, 1, NULL, 0},
    {231, "cr_temporal_T0", U_32, 1, NULL, 0},
    {232, "cr_temporal_T1", U_32, 1, NULL, 0},
    {233, "cr_temporal_T2", U_32, 1, NULL, 0},
    {234, "cr_temporal_T3", U_32, 1, NULL, 0},
    {235, "cr_temporal_maxchange", U_32, 1, NULL, 0},
    {236, "cr_temporal_min_target_high", U_32, 1, NULL, 0},
    {237, "cr_temporal_min_target_high_delta", U_32, 1, NULL, 0},
    {238, "cr_temporal_min_target_high_strength", U_32, 1, NULL, 0},
    {239, "cr_temporal_min_target_low", U_32, 1, NULL, 0},
    {240, "cr_temporal_min_target_low_delta", U_32, 1, NULL, 0},
    {241, "cr_temporal_min_target_low_strength", U_32, 1, NULL, 0},
    {242, "cr_temporal_min_target_method", U_32, 1, NULL, 0},
    {243, "cr_temporal_min_target_mid_strength", U_32, 1, NULL, 0},
    /*spatial_max_temporal*/
    {244, "y_spatial_max_change", U_32, 1, NULL, 0},
    {245, "y_spatial_max_temporal_high", U_32, 1, NULL, 0},
    {246, "y_spatial_max_temporal_high_delta", U_32, 1, NULL, 0},
    {247, "y_spatial_max_temporal_high_strength", U_32, 1, NULL, 0},
    {248, "y_spatial_max_temporal_low", U_32, 1, NULL, 0},
    {249, "y_spatial_max_temporal_low_delta", U_32, 1, NULL, 0},
    {250, "y_spatial_max_temporal_low_strength", U_32, 1, NULL, 0},
    {251, "y_spatial_max_temporal_method", U_32, 1, NULL, 0},
    {252, "y_spatial_max_temporal_mid_strength", U_32, 1, NULL, 0},
    {253, "cb_spatial_max_change", U_32, 1, NULL, 0},
    {254, "cb_spatial_max_temporal_high", U_32, 1, NULL, 0},
    {255, "cb_spatial_max_temporal_high_delta", U_32, 1, NULL, 0},
    {256, "cb_spatial_max_temporal_high_strength", U_32, 1, NULL, 0},
    {257, "cb_spatial_max_temporal_low", U_32, 1, NULL, 0},
    {258, "cb_spatial_max_temporal_low_delta", U_32, 1, NULL, 0},
    {259, "cb_spatial_max_temporal_low_strength", U_32, 1, NULL, 0},
    {260, "cb_spatial_max_temporal_method", U_32, 1, NULL, 0},
    {261, "cb_spatial_max_temporal_mid_strength", U_32, 1, NULL, 0},
    {262, "cr_spatial_max_change", U_32, 1, NULL, 0},
    {263, "cr_spatial_max_temporal_high", U_32, 1, NULL, 0},
    {264, "cr_spatial_max_temporal_high_delta", U_32, 1, NULL, 0},
    {265, "cr_spatial_max_temporal_high_strength", U_32, 1, NULL, 0},
    {266, "cr_spatial_max_temporal_low", U_32, 1, NULL, 0},
    {267, "cr_spatial_max_temporal_low_delta", U_32, 1, NULL, 0},
    {268, "cr_spatial_max_temporal_low_strength", U_32, 1, NULL, 0},
    {269, "cr_spatial_max_temporal_method", U_32, 1, NULL, 0},
    {270, "cr_spatial_max_temporal_mid_strength", U_32, 1, NULL, 0},
    {271, "y_spat_smth_directional_decide_t0", U_32, 1, NULL, 0},
    {272, "y_spat_smth_directional_decide_t1", U_32, 1, NULL, 0},
    {273, "cb_spat_smth_directional_decide_t0", U_32, 1, NULL, 0},
    {274, "cb_spat_smth_directional_decide_t1", U_32, 1, NULL, 0},
    {275, "cr_spat_smth_directional_decide_t0", U_32, 1, NULL, 0},
    {276, "cr_spat_smth_directional_decide_t1", U_32, 1, NULL, 0},
    {277, "y_spatial_weighting", U_32, 64, NULL, 0},
    {278, "cb_spatial_weighting", U_32, 64, NULL, 0},
    {279, "cr_spatial_weighting", U_32, 64, NULL, 0},
    {280, "y_strength_3d", U_32, 1, NULL, 0},
    {281, "cb_strength_3d", U_32, 1, NULL, 0},
    {282, "cr_strength_3d", U_32, 1, NULL, 0},
    {283, "y_combine_strength", U_32, 1, NULL, 0},
    {284, "local_lighting", U_32, 1, NULL, 0},
    {285, "compression_enable", U_32, 1, NULL, 0},
    {286, "y_temporal_ghost_prevent", U_32, 1, NULL, 0},
    {287, "cb_temporal_ghost_prevent", U_32, 1, NULL, 0},
    {288, "cr_temporal_ghost_prevent", U_32, 1, NULL, 0},
    {289, "y_temporal_artifact_guard", U_32, 1, NULL, 0},
    {290, "cb_temporal_artifact_guard", U_32, 1, NULL, 0},
    {291, "cr_temporal_artifact_guard", U_32, 1, NULL, 0},
    {292, "compression_bit_rate_luma", U_32, 1, NULL, 0},
    {293, "compression_bit_rate_chroma", U_32, 1, NULL, 0},
    {294, "compression_dither_disable", U_32, 1, NULL, 0},
};

#define VideoMctfLevelParams_Count 27
static PARAM_s VideoMctfLevelParams[VideoMctfLevelParams_Count] = {
    {0, "y_temporal_max_change_not_T0T1_level_based", U_32, 1, NULL, 0},
    {1, "y_temporal_either_max_change_or_T0T1_add_high", U_32, 1, NULL, 0},
    {2, "y_temporal_either_max_change_or_T0T1_add_high_delta", U_32, 1, NULL, 0},
    {3, "y_temporal_either_max_change_or_T0T1_add_high_strength", U_32, 1, NULL, 0},
    {4, "y_temporal_either_max_change_or_T0T1_add_low", U_32, 1, NULL, 0},
    {5, "y_temporal_either_max_change_or_T0T1_add_low_delta", U_32, 1, NULL, 0},
    {6, "y_temporal_either_max_change_or_T0T1_add_low_strength", U_32, 1, NULL, 0},
    {7, "y_temporal_either_max_change_or_T0T1_add_method", U_32, 1, NULL, 0},
    {8, "y_temporal_either_max_change_or_T0T1_add_mid_strength", U_32, 1, NULL, 0},
    {9, "cb_temporal_max_change_not_T0T1_level_based", U_32, 1, NULL, 0},
    {10, "cb_temporal_either_max_change_or_T0T1_add_high", U_32, 1, NULL, 0},
    {11, "cb_temporal_either_max_change_or_T0T1_add_high_delta", U_32, 1, NULL, 0},
    {12, "cb_temporal_either_max_change_or_T0T1_add_high_strength", U_32, 1, NULL, 0},
    {13, "cb_temporal_either_max_change_or_T0T1_add_low", U_32, 1, NULL, 0},
    {14, "cb_temporal_either_max_change_or_T0T1_add_low_delta", U_32, 1, NULL, 0},
    {15, "cb_temporal_either_max_change_or_T0T1_add_low_strength", U_32, 1, NULL, 0},
    {16, "cb_temporal_either_max_change_or_T0T1_add_method", U_32, 1, NULL, 0},
    {17, "cb_temporal_either_max_change_or_T0T1_add_mid_strength", U_32, 1, NULL, 0},
    {18, "cr_temporal_max_change_not_T0T1_level_based", U_32, 1, NULL, 0},
    {19, "cr_temporal_either_max_change_or_T0T1_add_high", U_32, 1, NULL, 0},
    {20, "cr_temporal_either_max_change_or_T0T1_add_high_delta", U_32, 1, NULL, 0},
    {21, "cr_temporal_either_max_change_or_T0T1_add_high_strength", U_32, 1, NULL, 0},
    {22, "cr_temporal_either_max_change_or_T0T1_add_low", U_32, 1, NULL, 0},
    {23, "cr_temporal_either_max_change_or_T0T1_add_low_delta", U_32, 1, NULL, 0},
    {24, "cr_temporal_either_max_change_or_T0T1_add_low_strength", U_32, 1, NULL, 0},
    {25, "cr_temporal_either_max_change_or_T0T1_add_method", U_32, 1, NULL, 0},
    {26, "cr_temporal_either_max_change_or_T0T1_add_mid_strength", U_32, 1, NULL, 0},
};

#define VideoMctfTemporalAdjustParams_Count 28
static PARAM_s VideoMctfTemporalAdjustParams[VideoMctfTemporalAdjustParams_Count] = {
    {0, "edge_adjust_b", U_32, 1, NULL, 0},
    {1, "edge_adjust_l", U_32, 1, NULL, 0},
    {2, "edge_adjust_r", U_32, 1, NULL, 0},
    {3, "edge_adjust_t", U_32, 1, NULL, 0},
    {4, "slow_mo_sensitivity", U_32, 1, NULL, 0},
    {5, "score_noise_robust", U_32, 1, NULL, 0},
    {6, "lev_adjust_low", U_32, 1, NULL, 0},
    {7, "lev_adjust_low_delta", U_32, 1, NULL, 0},
    {8, "lev_adjust_low_strength", U_32, 1, NULL, 0},
    {9, "lev_adjust_mid_strength", U_32, 1, NULL, 0},
    {10, "lev_adjust_high", U_32, 1, NULL, 0},
    {11, "lev_adjust_high_delta", U_32, 1, NULL, 0},
    {12, "lev_adjust_high_strength", U_32, 1, NULL, 0},
    {13, "y_max", U_32, 1, NULL, 0},
    {14, "y_min", U_32, 1, NULL, 0},
    {15, "y_motion_response", U_32, 1, NULL, 0},
    {16, "y_noise_base", U_32, 1, NULL, 0},
    {17, "y_still_thresh", U_32, 1, NULL, 0},
    {18, "cr_max", U_32, 1, NULL, 0},
    {19, "cr_min", U_32, 1, NULL, 0},
    {20, "cr_motion_response", U_32, 1, NULL, 0},
    {21, "cr_noise_base", U_32, 1, NULL, 0},
    {22, "cr_still_thresh", U_32, 1, NULL, 0},
    {23, "cb_max", U_32, 1, NULL, 0},
    {24, "cb_min", U_32, 1, NULL, 0},
    {25, "cb_motion_response", U_32, 1, NULL, 0},
    {26, "cb_noise_base", U_32, 1, NULL, 0},
    {27, "cb_still_thresh", U_32, 1, NULL, 0},
};

#define ShpAOrSpatialFilterSelectParams_Count 1
static PARAM_s ShpAOrSpatialFilterSelectParams[ShpAOrSpatialFilterSelectParams_Count] = {
    {0, "use_1st_sharpen_not_asf", U_32, 1, NULL, 0},
};

#define VideoMctfAndFinalSharpenParams_Count 1
static PARAM_s VideoMctfAndFinalSharpenParams[VideoMctfAndFinalSharpenParams_Count] = {
    {0, "pos_dep", U_8, 1089, NULL, 0},
};

#define AdvanceSpatialFilterParams_Count 36
static PARAM_s AdvanceSpatialFilterParams[AdvanceSpatialFilterParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "fir_specify", U_32, 1, NULL, 0},
    { 2, "fir_strength_iso", U_32, 1, NULL, 0},
    { 3, "fir_strength_dir", U_32, 1, NULL, 0},
    { 4, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    { 5, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    { 6, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    { 7, "fir_coefs", S_32, 225, NULL, 0},
    { 8, "fir_wide_edge_detect", U_32, 1, NULL, 0},
    { 9, "directional_decide_t0", U_32, 1, NULL, 0},
    { 10, "directional_decide_t1", U_32, 1, NULL, 0},
    { 11, "alpha_max_down", U_32, 1, NULL, 0},
    { 12, "alpha_max_up", U_32, 1, NULL, 0},
    { 13, "alpha_min_down", U_32, 1, NULL, 0},
    { 14, "alpha_min_up", U_32, 1, NULL, 0},
    { 15, "T0_down", U_32, 1, NULL, 0},
    { 16, "T0_up", U_32, 1, NULL, 0},
    { 17, "T1_down", U_32, 1, NULL, 0},
    { 18, "T1_up", U_32, 1, NULL, 0},
    { 19, "level_str_adjust_high", U_32, 1, NULL, 0},
    { 20, "level_str_adjust_high_delta", U_32, 1, NULL, 0},
    { 21, "level_str_adjust_high_strength", U_32, 1, NULL, 0},
    { 22, "level_str_adjust_mid_strength", U_32, 1, NULL, 0},
    { 23, "level_str_adjust_low", U_32, 1, NULL, 0},
    { 24, "level_str_adjust_low_delta", U_32, 1, NULL, 0},
    { 25, "level_str_adjust_low_strength", U_32, 1, NULL, 0},
    { 26, "T0T1_div_high", U_32, 1, NULL, 0},
    { 27, "T0T1_div_high_delta", U_32, 1, NULL, 0},
    { 28, "T0T1_div_high_strength", U_32, 1, NULL, 0},
    { 29, "T0T1_div_low", U_32, 1, NULL, 0},
    { 30, "T0T1_div_low_delta", U_32, 1, NULL, 0},
    { 31, "T0T1_div_low_strength", U_32, 1, NULL, 0},
    { 32, "T0T1_div_mid_strength", U_32, 1, NULL, 0},
    { 33, "max_change_down", U_32, 1, NULL, 0},
    { 34, "max_change_up", U_32, 1, NULL, 0},
    { 35, "max_change_not_T0T1_alpha", U_32, 1, NULL, 0}, // H22 coring
};

#define LumaNoiseReductionParams_Count 8
static PARAM_s LumaNoiseReductionParams[LumaNoiseReductionParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0 },
    { 1, "sensor_wb_b", U_32, 1, NULL, 0 },
    { 2, "sensor_wb_g", U_32, 1, NULL, 0 },
    { 3, "sensor_wb_r", U_32, 1, NULL, 0 },
    { 4, "strength0", U_32, 1, NULL, 0 },
    { 5, "strength1", U_32, 1, NULL, 0 },
    { 6, "strength2", U_32, 1, NULL, 0 },
    { 7, "strength2_max_change", U_32, 1, NULL, 0 },
};

#define HiLumaNoiseReductionParams_Count 8
static PARAM_s HiLumaNoiseReductionParams[] = {
    { 0, "enable", U_32, 1, 0, 0 },
    { 1, "sensor_wb_b", U_32, 1, 0, 0 },
    { 2, "sensor_wb_g", U_32, 1, 0, 0 },
    { 3, "sensor_wb_r", U_32, 1, 0, 0 },
    { 4, "strength0", U_32, 1, 0, 0 },
    { 5, "strength1", U_32, 1, 0, 0 },
    { 6, "strength2", U_32, 1, 0, 0 },
    { 7, "strength2_max_change", U_32, 1, 0, 0 },
};

#define Li2LumaNoiseReductionParams_Count 8
static PARAM_s Li2LumaNoiseReductionParams[] = {
    { 0, "enable", U_32, 1, 0, 0 },
    { 1, "sensor_wb_b", U_32, 1, 0, 0 },
    { 2, "sensor_wb_g", U_32, 1, 0, 0 },
    { 3, "sensor_wb_r", U_32, 1, 0, 0 },
    { 4, "strength0", U_32, 1, 0, 0 },
    { 5, "strength1", U_32, 1, 0, 0 },
    { 6, "strength2", U_32, 1, 0, 0 },
    { 7, "strength2_max_change", U_32, 1, 0, 0 },
};

#define ChromaFilterParams_Count 6
static PARAM_s ChromaFilterParams[ChromaFilterParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "noise_level_cb", U_32, 1, NULL, 0},
    { 2, "noise_level_cr", U_32, 1, NULL, 0},
    { 3, "original_blend_strength_cb", U_32, 1, NULL, 0},
    { 4, "original_blend_strength_cr", U_32, 1, NULL, 0},
    { 5, "radius", U_32, 1, NULL, 0}
};

#define WideChromaFilterParams_Count 3
static PARAM_s WideChromaFilterParams[WideChromaFilterParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "noise_level_cb", U_32, 1, NULL, 0},
    { 2, "noise_level_cr", U_32, 1, NULL, 0}
};

#define ContrastEnhance_Count 35
static PARAM_s ContrastEnhance[ContrastEnhance_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "luma_avg_method", U_32, 1, NULL, 0},
    {2, "luma_avg_weight_r", S_32, 1, NULL, 0},
    {3, "luma_avg_weight_gr", S_32, 1, NULL, 0},
    {4, "luma_avg_weight_gb", S_32, 1, NULL, 0},
    {5, "luma_avg_weight_b", S_32, 1, NULL, 0},
    {6, "fir_enable", U_32, 1, NULL, 0},
    {7, "fir_coeff", U_32, 3, NULL, 0},
    {8, "coring_index_scale_shift", U_32, 1, NULL, 0},
    {9, "coring_index_scale_low_strength", U_32, 1, NULL, 0},
    {10, "coring_index_scale_mid_strength", U_32, 1, NULL, 0},
    {11, "coring_index_scale_high_strength", U_32, 1, NULL, 0},
    {12, "coring_index_scale_low", U_32, 1, NULL, 0},
    {13, "coring_index_scale_high", U_32, 1, NULL, 0},
    {14, "coring_index_scale_low_delta", U_32, 1, NULL, 0},
    {15, "coring_index_scale_high_delta", U_32, 1, NULL, 0},
    {16, "coring_gain_shift", U_32, 1, NULL, 0},
    {17, "coring_gain_low_strength", U_32, 1, NULL, 0},
    {18, "coring_gain_mid_strength", U_32, 1, NULL, 0},
    {19, "coring_gain_high_strength", U_32, 1, NULL, 0},
    {20, "coring_gain_low", U_32, 1, NULL, 0},
    {21, "coring_gain_high", U_32, 1, NULL, 0},
    {22, "coring_gain_low_delta", U_32, 1, NULL, 0},
    {23, "coring_gain_high_delta", U_32, 1, NULL, 0},
    {24, "coring_table", U_32, 64, NULL, 0},
    {25, "output_enable", U_32, 1, NULL, 0},
    {26, "output_shift", U_32, 1, NULL, 0},
    {27, "output_table", U_32, 256, NULL, 0},
    {28, "input_table_enable", U_32, 1, NULL, 0},
    {29, "input_table", U_32, 449, NULL, 0},
    {30, "boost_gain_shift", U_32, 1, NULL, 0},
    {31, "boost_table", S_32, 64, NULL, 0},
    {32, "boost_table_size_exp", U_32, 1, NULL, 0},
    {33, "radius", U_32, 1, NULL, 0},
    {34, "epsilon", U_32, 1, NULL, 0},
};

#define HdrFrontEndWbgain_Count 5
static PARAM_s HdrFrontEndWbgain[HdrFrontEndWbgain_Count] = {
    {0, "r_gain", U_32, 1, NULL, 0},
    {1, "g_gain", U_32, 1, NULL, 0},
    {2, "b_gain", U_32, 1, NULL, 0},
    {3, "ir_gain", U_32, 1, NULL, 0},
    {4, "shutter_ratio", U_32, 1, NULL, 0},
};

#define HdrRawInfo_Count 12
static PARAM_s HdrRawInfo[HdrRawInfo_Count] = {
    {0, "raw_area_width[0]", U_32, 1, NULL, 0},
    {1, "raw_area_width[1]", U_32, 1, NULL, 0},
    {2, "raw_area_width[2]", U_32, 1, NULL, 0},
    {3, "raw_area_height[0]", U_32, 1, NULL, 0},
    {4, "raw_area_height[1]", U_32, 1, NULL, 0},
    {5, "raw_area_height[2]", U_32, 1, NULL, 0},
    {6, "raw_start_x[0]", U_32, 1, NULL, 0},
    {7, "raw_start_x[1]", U_32, 1, NULL, 0},
    {8, "raw_start_x[2]", U_32, 1, NULL, 0},
    {9, "raw_start_y[0]", U_32, 1, NULL, 0},
    {10, "raw_start_y[1]", U_32, 1, NULL, 0},
    {11, "raw_start_y[2]", U_32, 1, NULL, 0},
};

#define HdrBlendParams_Count 10
static PARAM_s HdrBlendParams[HdrBlendParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "delta_t0", U_32, 4, NULL, 0},
    {2, "delta_t1", U_32, 4, NULL, 0},
    {3, "flicker_threshold", U_32, 1, NULL, 0},
    {4, "t0_offset", U_32, 4, NULL, 0},
    {5, "t1_offset", U_32, 4, NULL, 0},
    {6, "A0", U_32, 4, NULL, 0},
    {7, "A1", U_32, 4, NULL, 0},
    {8, "B0", U_32, 4, NULL, 0},
    {9, "B1", U_32, 4, NULL, 0},
};

#define WideChromaFilterCombineParams_Count 10
static PARAM_s WideChromaFilterCombineParams[WideChromaFilterCombineParams_Count] = {
    {0, "T0_cb", U_32, 1, NULL, 0},
    {1, "T0_cr", U_32, 1, NULL, 0},
    {2, "T1_cb", U_32, 1, NULL, 0},
    {3, "T1_cr", U_32, 1, NULL, 0},
    {4, "alpha_max_cb", U_32, 1, NULL, 0},
    {5, "alpha_max_cr", U_32, 1, NULL, 0},
    {6, "alpha_min_cb", U_32, 1, NULL, 0},
    {7, "alpha_min_cr", U_32, 1, NULL, 0},
    {8, "max_change_cb", U_32, 1, NULL, 0},
    {9, "max_change_cr", U_32, 1, NULL, 0},
};


#define HiChromaAdvanceSpatialFilterParams_Count 31
static PARAM_s HiChromaAdvanceSpatialFilterParams[HiChromaAdvanceSpatialFilterParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "fir_specify", U_32, 1, NULL, 0},
    { 2, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    { 3, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    { 4, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    { 5, "fir_coefs", S_32, 225, NULL, 0},
    { 6, "fir_strength_iso", U_32, 1, NULL, 0},
    { 7, "fir_strength_dir", U_32, 1, NULL, 0},
    { 8, "fir_wide_edge_detect", U_32, 1, NULL, 0},
    { 9, "directional_decide_t0", U_32, 1, NULL, 0},
    { 10, "directional_decide_t1", U_32, 1, NULL, 0},
    { 11, "alpha_min", U_32, 1, NULL, 0},
    { 12, "alpha_max", U_32, 1, NULL, 0},
    { 13, "T0", U_32, 1, NULL, 0},
    { 14, "T1", U_32, 1, NULL, 0},
    { 15, "level_str_adjust_low", U_32, 1, NULL, 0},
    { 16, "level_str_adjust_low_delta", U_32, 1, NULL, 0},
    { 17, "level_str_adjust_low_strength", U_32, 1, NULL, 0},
    { 18, "level_str_adjust_mid_strength", U_32, 1, NULL, 0},
    { 19, "level_str_adjust_high", U_32, 1, NULL, 0},
    { 20, "level_str_adjust_high_delta", U_32, 1, NULL, 0},
    { 21, "level_str_adjust_high_strength", U_32, 1, NULL, 0},
    { 22, "T0T1_div_low", U_32, 1, NULL, 0},
    { 23, "T0T1_div_low_delta", U_32, 1, NULL, 0},
    { 24, "T0T1_div_low_strength", U_32, 1, NULL, 0},
    { 25, "T0T1_div_mid_strength", U_32, 1, NULL, 0},
    { 26, "T0T1_div_high", U_32, 1, NULL, 0},
    { 27, "T0T1_div_high_delta", U_32, 1, NULL, 0},
    { 28, "T0T1_div_high_strength", U_32, 1, NULL, 0},
    { 29, "max_change_not_T0T1_alpha", U_32, 1, NULL, 0}, // H22 coring
    { 30, "max_change", U_32, 1, NULL, 0},
};

#define HiLumaAdvanceSpatialFilterParams_Count 36
static PARAM_s HiLumaAdvanceSpatialFilterParams[HiLumaAdvanceSpatialFilterParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "fir_specify", U_32, 1, NULL, 0},
    { 2, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    { 3, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    { 4, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    { 5, "fir_coefs", S_32, 225, NULL, 0},
    { 6, "fir_strength_iso", U_32, 1, NULL, 0},
    { 7, "fir_strength_dir", U_32, 1, NULL, 0},
    { 8, "fir_wide_edge_detect", U_32, 1, NULL, 0},
    { 9, "directional_decide_t0", U_32, 1, NULL, 0},
    { 10, "directional_decide_t1", U_32, 1, NULL, 0},
    { 11, "alpha_min_up", U_32, 1, NULL, 0},
    { 12, "alpha_max_up", U_32, 1, NULL, 0},
    { 13, "T0_up", U_32, 1, NULL, 0},
    { 14, "T1_up", U_32, 1, NULL, 0},
    { 15, "alpha_min_down", U_32, 1, NULL, 0},
    { 16, "alpha_max_down", U_32, 1, NULL, 0},
    { 17, "T0_down", U_32, 1, NULL, 0},
    { 18, "T1_down", U_32, 1, NULL, 0},
    { 19, "level_str_adjust_low", U_32, 1, NULL, 0},
    { 20, "level_str_adjust_low_delta", U_32, 1, NULL, 0},
    { 21, "level_str_adjust_low_strength", U_32, 1, NULL, 0},
    { 22, "level_str_adjust_mid_strength", U_32, 1, NULL, 0},
    { 23, "level_str_adjust_high", U_32, 1, NULL, 0},
    { 24, "level_str_adjust_high_delta", U_32, 1, NULL, 0},
    { 25, "level_str_adjust_high_strength", U_32, 1, NULL, 0},
    { 26, "T0T1_div_low", U_32, 1, NULL, 0},
    { 27, "T0T1_div_low_delta", U_32, 1, NULL, 0},
    { 28, "T0T1_div_low_strength", U_32, 1, NULL, 0},
    { 29, "T0T1_div_mid_strength", U_32, 1, NULL, 0},
    { 30, "T0T1_div_high", U_32, 1, NULL, 0},
    { 31, "T0T1_div_high_delta", U_32, 1, NULL, 0},
    { 32, "T0T1_div_high_strength", U_32, 1, NULL, 0},
    { 33, "max_change_not_T0T1_alpha", U_32, 1, NULL, 0}, // H22 coring
    { 34, "max_change_up", U_32, 1, NULL, 0},
    { 35, "max_change_down", U_32, 1, NULL, 0},
};

#define HiCfaNoiseFilterParams_Count 53
static PARAM_s HiCfaNoiseFilterParams[HiCfaNoiseFilterParams_Count] = {
    {0, "enable", U_32, 1, 0, 0},
    {1, "min_noise_level_red", U_32, 1, 0, 0},
    {2, "min_noise_level_green", U_32, 1, 0, 0},
    {3, "min_noise_level_blue", U_32, 1, 0, 0},
    {4, "max_noise_level_red", U_32, 1, 0, 0},
    {5, "max_noise_level_green", U_32, 1, 0, 0},
    {6, "max_noise_level_blue", U_32, 1, 0, 0},
    {7, "original_blend_strength_red", U_32, 1, 0, 0},
    {8, "original_blend_strength_green", U_32, 1, 0, 0},
    {9, "original_blend_strength_blue", U_32, 1, 0, 0},
    {10, "extent_regular_red", U_32, 1, 0, 0},
    {11, "extent_regular_green", U_32, 1, 0, 0},
    {12, "extent_regular_blue", U_32, 1, 0, 0},
    {13, "extent_fine_red", U_32, 1, 0, 0},
    {14, "extent_fine_green", U_32, 1, 0, 0},
    {15, "extent_fine_blue", U_32, 1, 0, 0},
    {16, "strength_fine_red", U_32, 1, 0, 0},
    {17, "strength_fine_green", U_32, 1, 0, 0},
    {18, "strength_fine_blue", U_32, 1, 0, 0},
    {19, "selectivity_regular", U_32, 1, 0, 0},
    {20, "selectivity_fine", U_32, 1, 0, 0},
    {21, "directional_enable", U_32, 1, 0, 0},
    {22, "directional_horvert_edge_strength", U_32, 1, 0, 0},
    {23, "directional_horvert_edge_strength_bias", U_32, 1, 0, 0},
    {24, "directional_horvert_original_blend_strength", U_32, 1, 0, 0},
    {25, "dir_lev_max_red", U_32, 1, 0, 0},
    {26, "dir_lev_max_green", U_32, 1, 0, 0},
    {27, "dir_lev_max_blue", U_32, 1, 0, 0},
    {28, "dir_lev_min_red", U_32, 1, 0, 0},
    {29, "dir_lev_min_green", U_32, 1, 0, 0},
    {30, "dir_lev_min_blue", U_32, 1, 0, 0},
    {31, "dir_lev_mul_red", U_32, 1, 0, 0},
    {32, "dir_lev_mul_green", U_32, 1, 0, 0},
    {33, "dir_lev_mul_blue", U_32, 1, 0, 0},
    {34, "dir_lev_offset_red", S_32, 1, 0, 0},
    {35, "dir_lev_offset_green", S_32, 1, 0, 0},
    {36, "dir_lev_offset_blue", S_32, 1, 0, 0},
    {37, "dir_lev_shift_red", U_32, 1, 0, 0},
    {38, "dir_lev_shift_green", U_32, 1, 0, 0},
    {39, "dir_lev_shift_blue", U_32, 1, 0, 0},
    {40, "lev_mul_red", U_32, 1, 0, 0},
    {41, "lev_mul_green", U_32, 1, 0, 0},
    {42, "lev_mul_blue", U_32, 1, 0, 0},
    {43, "lev_offset_red", S_32, 1, 0, 0},
    {44, "lev_offset_green", S_32, 1, 0, 0},
    {45, "lev_offset_blue", S_32, 1, 0, 0},
    {46, "lev_shift_red", U_32, 1, 0, 0},
    {47, "lev_shift_green", U_32, 1, 0, 0},
    {48, "lev_shift_blue", U_32, 1, 0, 0},
    {49, "apply_to_color_diff_red", U_32, 1, 0, 0},
    {50, "apply_to_color_diff_blue", U_32, 1, 0, 0},
    {51, "opt_for_rccb_red", U_32, 1, 0, 0},
    {52, "opt_for_rccb_blue", U_32, 1, 0, 0},
};

#define HiCfaLeakageFilterParams_Count 6
static PARAM_s HiCfaLeakageFilterParams[HiCfaLeakageFilterParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "alpha_rr", S_32, 1, NULL, 0},
    {2, "alpha_rb", S_32, 1, NULL, 0},
    {3, "alpha_br", S_32, 1, NULL, 0},
    {4, "alpha_bb", S_32, 1, NULL, 0},
    {5, "saturation_level", U_32, 1, NULL, 0},
};

#define HiAntiAliasingParams_Count 3
static PARAM_s HiAntiAliasingParams[HiAntiAliasingParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "thresh", U_32, 1, NULL, 0},
    {2, "log_fractional_correct", U_32, 1, NULL, 0},
};

#define HiAutoBadPixelCorrectionParams_Count 4
static PARAM_s HiAutoBadPixelCorrectionParams[HiAutoBadPixelCorrectionParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "hot_pixel_detection_strength", U_32, 1, NULL, 0},
    {2, "dark_pixel_detection_strength", U_32, 1, NULL, 0},
    {3, "correction_method", U_32, 1, NULL, 0},
};

#define HiChromaFilterHighParams_Count 6
static PARAM_s HiChromaFilterHighParams[HiChromaFilterHighParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "noise_level_cb", U_32, 1, NULL, 0},
    { 2, "noise_level_cr", U_32, 1, NULL, 0},
    { 3, "original_blend_strength_cb", U_32, 1, NULL, 0},
    { 4, "original_blend_strength_cr", U_32, 1, NULL, 0},
    { 5, "radius", U_32, 1, NULL, 0}
};

#define HiChromaFilterLowParams_Count 5
static PARAM_s HiChromaFilterLowParams[HiChromaFilterLowParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "noise_level_cb", U_32, 1, NULL, 0},
    { 2, "noise_level_cr", U_32, 1, NULL, 0},
    { 3, "original_blend_strength_cb", U_32, 1, NULL, 0},
    { 4, "original_blend_strength_cr", U_32, 1, NULL, 0},
};

#define HiChromaFilterLowCombineParams_Count 38
static PARAM_s HiChromaFilterLowCombineParams[HiChromaFilterLowCombineParams_Count] = {
    { 0, "T0_cb", U_32, 1, NULL, 0},
    { 1, "T0_cr", U_32, 1, NULL, 0},
    { 2, "T1_cb", U_32, 1, NULL, 0},
    { 3, "T1_cr", U_32, 1, NULL, 0},
    { 4, "alpha_max_cb", U_32, 1, NULL, 0},
    { 5, "alpha_max_cr", U_32, 1, NULL, 0},
    { 6, "alpha_min_cb", U_32, 1, NULL, 0},
    { 7, "alpha_min_cr", U_32, 1, NULL, 0},
    { 8, "max_change_low_cb", U_32, 1, NULL, 0},
    { 9, "max_change_low_delta_cb", U_32, 1, NULL, 0},
    { 10, "max_change_low_strength_cb", U_32, 1, NULL, 0},
    { 11, "max_change_mid_strength_cb", U_32, 1, NULL, 0},
    { 12, "max_change_high_cb", U_32, 1, NULL, 0},
    { 13, "max_change_high_delta_cb", U_32, 1, NULL, 0},
    { 14, "max_change_high_strength_cb", U_32, 1, NULL, 0},
    { 15, "max_change_low_cr", U_32, 1, NULL, 0},
    { 16, "max_change_low_delta_cr", U_32, 1, NULL, 0},
    { 17, "max_change_low_strength_cr", U_32, 1, NULL, 0},
    { 18, "max_change_mid_strength_cr", U_32, 1, NULL, 0},
    { 19, "max_change_high_cr", U_32, 1, NULL, 0},
    { 20, "max_change_high_delta_cr", U_32, 1, NULL, 0},
    { 21, "max_change_high_strength_cr", U_32, 1, NULL, 0},
    { 22, "T0T1_add_low_cb", U_32, 1, NULL, 0},
    { 23, "T0T1_add_low_delta_cb", U_32, 1, NULL, 0},
    { 24, "T0T1_add_low_strength_cb", U_32, 1, NULL, 0},
    { 25, "T0T1_add_mid_strength_cb", U_32, 1, NULL, 0},
    { 26, "T0T1_add_high_cb", U_32, 1, NULL, 0},
    { 27, "T0T1_add_high_delta_cb", U_32, 1, NULL, 0},
    { 28, "T0T1_add_high_strength_cb", U_32, 1, NULL, 0},
    { 29, "T0T1_add_low_cr", U_32, 1, NULL, 0},
    { 30, "T0T1_add_low_delta_cr", U_32, 1, NULL, 0},
    { 31, "T0T1_add_low_strength_cr", U_32, 1, NULL, 0},
    { 32, "T0T1_add_mid_strength_cr", U_32, 1, NULL, 0},
    { 33, "T0T1_add_high_cr", U_32, 1, NULL, 0},
    { 34, "T0T1_add_high_delta_cr", U_32, 1, NULL, 0},
    { 35, "T0T1_add_high_strength_cr", U_32, 1, NULL, 0},
    { 36, "signal_preserve_cb", U_32, 1, NULL, 0},
    { 37, "signal_preserve_cr", U_32, 1, NULL, 0},
};

#define HiChromaFilterMedianParams_Count 5
static PARAM_s HiChromaFilterMedianParams[HiChromaFilterMedianParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "noise_level_cb", U_32, 1, NULL, 0},
    { 2, "noise_level_cr", U_32, 1, NULL, 0},
    { 3, "original_blend_strength_cb", U_32, 1, NULL, 0},
    { 4, "original_blend_strength_cr", U_32, 1, NULL, 0},
};

#define HiChromaFilterMedianCombineParams_Count 38
static PARAM_s HiChromaFilterMedianCombineParams[HiChromaFilterMedianCombineParams_Count] = {
    { 0, "T0_cb", U_32, 1, NULL, 0},
    { 1, "T0_cr", U_32, 1, NULL, 0},
    { 2, "T1_cb", U_32, 1, NULL, 0},
    { 3, "T1_cr", U_32, 1, NULL, 0},
    { 4, "alpha_max_cb", U_32, 1, NULL, 0},
    { 5, "alpha_max_cr", U_32, 1, NULL, 0},
    { 6, "alpha_min_cb", U_32, 1, NULL, 0},
    { 7, "alpha_min_cr", U_32, 1, NULL, 0},
    { 8, "max_change_low_cb", U_32, 1, NULL, 0},
    { 9, "max_change_low_delta_cb", U_32, 1, NULL, 0},
    { 10, "max_change_low_strength_cb", U_32, 1, NULL, 0},
    { 11, "max_change_mid_strength_cb", U_32, 1, NULL, 0},
    { 12, "max_change_high_cb", U_32, 1, NULL, 0},
    { 13, "max_change_high_delta_cb", U_32, 1, NULL, 0},
    { 14, "max_change_high_strength_cb", U_32, 1, NULL, 0},
    { 15, "max_change_low_cr", U_32, 1, NULL, 0},
    { 16, "max_change_low_delta_cr", U_32, 1, NULL, 0},
    { 17, "max_change_low_strength_cr", U_32, 1, NULL, 0},
    { 18, "max_change_mid_strength_cr", U_32, 1, NULL, 0},
    { 19, "max_change_high_cr", U_32, 1, NULL, 0},
    { 20, "max_change_high_delta_cr", U_32, 1, NULL, 0},
    { 21, "max_change_high_strength_cr", U_32, 1, NULL, 0},
    { 22, "T0T1_add_low_cb", U_32, 1, NULL, 0},
    { 23, "T0T1_add_low_delta_cb", U_32, 1, NULL, 0},
    { 24, "T0T1_add_low_strength_cb", U_32, 1, NULL, 0},
    { 25, "T0T1_add_mid_strength_cb", U_32, 1, NULL, 0},
    { 26, "T0T1_add_high_cb", U_32, 1, NULL, 0},
    { 27, "T0T1_add_high_delta_cb", U_32, 1, NULL, 0},
    { 28, "T0T1_add_high_strength_cb", U_32, 1, NULL, 0},
    { 29, "T0T1_add_low_cr", U_32, 1, NULL, 0},
    { 30, "T0T1_add_low_delta_cr", U_32, 1, NULL, 0},
    { 31, "T0T1_add_low_strength_cr", U_32, 1, NULL, 0},
    { 32, "T0T1_add_mid_strength_cr", U_32, 1, NULL, 0},
    { 33, "T0T1_add_high_cr", U_32, 1, NULL, 0},
    { 34, "T0T1_add_high_delta_cr", U_32, 1, NULL, 0},
    { 35, "T0T1_add_high_strength_cr", U_32, 1, NULL, 0},
    { 36, "signal_preserve_cb", U_32, 1, NULL, 0},
    { 37, "signal_preserve_cr", U_32, 1, NULL, 0},
};

#define HiChromaFilterPreParams_Count 5
static PARAM_s HiChromaFilterPreParams[HiChromaFilterPreParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "noise_level_cb", U_32, 1, NULL, 0},
    { 2, "noise_level_cr", U_32, 1, NULL, 0},
    { 3, "original_blend_strength_cb", U_32, 1, NULL, 0},
    { 4, "original_blend_strength_cr", U_32, 1, NULL, 0},
};

#define HiChromaFilterVeryLowParams_Count 5
static PARAM_s HiChromaFilterVeryLowParams[HiChromaFilterVeryLowParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "noise_level_cb", U_32, 1, NULL, 0},
    { 2, "noise_level_cr", U_32, 1, NULL, 0},
    { 3, "original_blend_strength_cb", U_32, 1, NULL, 0},
    { 4, "original_blend_strength_cr", U_32, 1, NULL, 0},
};

#define HiChromaFilterVeryLowCombineParams_Count 38
static PARAM_s HiChromaFilterVeryLowCombineParams[HiChromaFilterVeryLowCombineParams_Count] = {
    { 0, "T0_cb", U_32, 1, NULL, 0},
    { 1, "T0_cr", U_32, 1, NULL, 0},
    { 2, "T1_cb", U_32, 1, NULL, 0},
    { 3, "T1_cr", U_32, 1, NULL, 0},
    { 4, "alpha_max_cb", U_32, 1, NULL, 0},
    { 5, "alpha_max_cr", U_32, 1, NULL, 0},
    { 6, "alpha_min_cb", U_32, 1, NULL, 0},
    { 7, "alpha_min_cr", U_32, 1, NULL, 0},
    { 8, "max_change_low_cb", U_32, 1, NULL, 0},
    { 9, "max_change_low_delta_cb", U_32, 1, NULL, 0},
    { 10, "max_change_low_strength_cb", U_32, 1, NULL, 0},
    { 11, "max_change_mid_strength_cb", U_32, 1, NULL, 0},
    { 12, "max_change_high_cb", U_32, 1, NULL, 0},
    { 13, "max_change_high_delta_cb", U_32, 1, NULL, 0},
    { 14, "max_change_high_strength_cb", U_32, 1, NULL, 0},
    { 15, "max_change_low_cr", U_32, 1, NULL, 0},
    { 16, "max_change_low_delta_cr", U_32, 1, NULL, 0},
    { 17, "max_change_low_strength_cr", U_32, 1, NULL, 0},
    { 18, "max_change_mid_strength_cr", U_32, 1, NULL, 0},
    { 19, "max_change_high_cr", U_32, 1, NULL, 0},
    { 20, "max_change_high_delta_cr", U_32, 1, NULL, 0},
    { 21, "max_change_high_strength_cr", U_32, 1, NULL, 0},
    { 22, "T0T1_add_low_cb", U_32, 1, NULL, 0},
    { 23, "T0T1_add_low_delta_cb", U_32, 1, NULL, 0},
    { 24, "T0T1_add_low_strength_cb", U_32, 1, NULL, 0},
    { 25, "T0T1_add_mid_strength_cb", U_32, 1, NULL, 0},
    { 26, "T0T1_add_high_cb", U_32, 1, NULL, 0},
    { 27, "T0T1_add_high_delta_cb", U_32, 1, NULL, 0},
    { 28, "T0T1_add_high_strength_cb", U_32, 1, NULL, 0},
    { 29, "T0T1_add_low_cr", U_32, 1, NULL, 0},
    { 30, "T0T1_add_low_delta_cr", U_32, 1, NULL, 0},
    { 31, "T0T1_add_low_strength_cr", U_32, 1, NULL, 0},
    { 32, "T0T1_add_mid_strength_cr", U_32, 1, NULL, 0},
    { 33, "T0T1_add_high_cr", U_32, 1, NULL, 0},
    { 34, "T0T1_add_high_delta_cr", U_32, 1, NULL, 0},
    { 35, "T0T1_add_high_strength_cr", U_32, 1, NULL, 0},
    { 36, "signal_preserve_cb", U_32, 1, NULL, 0},
    { 37, "signal_preserve_cr", U_32, 1, NULL, 0},
};

#define HiChromaMedianFilterParams_Count 7
static PARAM_s HiChromaMedianFilterParams[HiChromaMedianFilterParams_Count] = {
    {0, "enable", S_32, 1, NULL, 0},
    {1, "cb_adaptive_strength", U_32, 1, NULL, 0},
    {2, "cr_adaptive_strength", U_32, 1, NULL, 0},
    {3, "cb_non_adaptive_strength", U_32, 1, NULL, 0},
    {4, "cr_non_adaptive_strength", U_32, 1, NULL, 0},
    {5, "cb_adaptive_amount", U_32, 1, NULL, 0},
    {6, "cr_adaptive_amount", U_32, 1, NULL, 0},
};

#define HiDemosaicFilterParams_Count 7
static PARAM_s HiDemosaicFilterParams[HiDemosaicFilterParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "activity_thresh", U_32, 1, NULL, 0},
    {2, "activity_difference_thresh", U_32, 1, NULL, 0},
    {3, "grad_clip_thresh", U_32, 1, NULL, 0},
    {4, "grad_noise_thresh", U_32, 1, NULL, 0},
    {5, "alias_interpolation_strength", U_32, 1, NULL, 0},
    {6, "alias_interpolation_thresh", U_32, 1, NULL, 0},
};

#define HiGbGrMismatchParams_Count 4
static PARAM_s HiGbGrMismatchParams[HiGbGrMismatchParams_Count] = {
    {0, "narrow_enable", U_32, 1, NULL, 0},
    {1, "wide_enable", U_32, 1, NULL, 0},
    {2, "wide_safety", U_32, 1, NULL, 0},
    {3, "wide_thresh", U_32, 1, NULL, 0},
};

#define HiHigh2LumaAdvanceSpatialFilterParams_Count 38
static PARAM_s HiHigh2LumaAdvanceSpatialFilterParams[HiHigh2LumaAdvanceSpatialFilterParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "fir_specify", U_32, 1, NULL, 0},
    { 2, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    { 3, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    { 4, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    { 5, "fir_coefs", S_32, 225, NULL, 0},
    { 6, "fir_strength_iso", U_32, 1, NULL, 0},
    { 7, "fir_strength_dir", U_32, 1, NULL, 0},
    { 8, "fir_wide_edge_detect", U_32, 1, NULL, 0},
    { 9, "directional_decide_t0", U_32, 1, NULL, 0},
    { 10, "directional_decide_t1", U_32, 1, NULL, 0},
    { 11, "alpha_min_up", U_32, 1, NULL, 0},
    { 12, "alpha_max_up", U_32, 1, NULL, 0},
    { 13, "T0_up", U_32, 1, NULL, 0},
    { 14, "T1_up", U_32, 1, NULL, 0},
    { 15, "alpha_min_down", U_32, 1, NULL, 0},
    { 16, "alpha_max_down", U_32, 1, NULL, 0},
    { 17, "T0_down", U_32, 1, NULL, 0},
    { 18, "T1_down", U_32, 1, NULL, 0},
    { 19, "level_str_adjust_low", U_32, 1, NULL, 0},
    { 20, "level_str_adjust_low_delta", U_32, 1, NULL, 0},
    { 21, "level_str_adjust_low_strength", U_32, 1, NULL, 0},
    { 22, "level_str_adjust_mid_strength", U_32, 1, NULL, 0},
    { 23, "level_str_adjust_high", U_32, 1, NULL, 0},
    { 24, "level_str_adjust_high_delta", U_32, 1, NULL, 0},
    { 25, "level_str_adjust_high_strength", U_32, 1, NULL, 0},
    { 26, "level_str_adjust_method", U_32, 1, NULL, 0},
    { 27, "T0T1_div_low", U_32, 1, NULL, 0},
    { 28, "T0T1_div_low_delta", U_32, 1, NULL, 0},
    { 29, "T0T1_div_low_strength", U_32, 1, NULL, 0},
    { 30, "T0T1_div_mid_strength", U_32, 1, NULL, 0},
    { 31, "T0T1_div_high", U_32, 1, NULL, 0},
    { 32, "T0T1_div_high_delta", U_32, 1, NULL, 0},
    { 33, "T0T1_div_high_strength", U_32, 1, NULL, 0},
    { 34, "T0T1_div_method", U_32, 1, NULL, 0},
    { 35, "max_change_not_T0T1_alpha", U_32, 1, NULL, 0}, // H22 coring
    { 36, "max_change_up", U_32, 1, NULL, 0},
    { 37, "max_change_down", U_32, 1, NULL, 0},
};

#define HiHighLumaAdvanceSpatialFilterParams_Count 37
static PARAM_s HiHighLumaAdvanceSpatialFilterParams[HiHighLumaAdvanceSpatialFilterParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "fir_specify", U_32, 1, NULL, 0},
    { 2, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    { 3, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    { 4, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    { 5, "fir_coefs", S_32, 225, NULL, 0},
    { 6, "fir_strength_iso", U_32, 1, NULL, 0},
    { 7, "fir_strength_dir", U_32, 1, NULL, 0},
    { 8, "fir_wide_edge_detect", U_32, 1, NULL, 0},
    { 9, "directional_decide_t0", U_32, 1, NULL, 0},
    { 10, "directional_decide_t1", U_32, 1, NULL, 0},
    { 11, "alpha_min_up", U_32, 1, NULL, 0},
    { 12, "alpha_max_up", U_32, 1, NULL, 0},
    { 13, "T0_up", U_32, 1, NULL, 0},
    { 14, "T1_up", U_32, 1, NULL, 0},
    { 15, "alpha_min_down", U_32, 1, NULL, 0},
    { 16, "alpha_max_down", U_32, 1, NULL, 0},
    { 17, "T0_down", U_32, 1, NULL, 0},
    { 18, "T1_down", U_32, 1, NULL, 0},
    { 19, "level_str_adjust_low", U_32, 1, NULL, 0},
    { 20, "level_str_adjust_low_delta", U_32, 1, NULL, 0},
    { 21, "level_str_adjust_low_strength", U_32, 1, NULL, 0},
    { 22, "level_str_adjust_mid_strength", U_32, 1, NULL, 0},
    { 23, "level_str_adjust_high", U_32, 1, NULL, 0},
    { 24, "level_str_adjust_high_delta", U_32, 1, NULL, 0},
    { 25, "level_str_adjust_high_strength", U_32, 1, NULL, 0},
    { 26, "level_str_adjust_method", U_32, 1, NULL, 0},
    { 27, "T0T1_div_low", U_32, 1, NULL, 0},
    { 28, "T0T1_div_low_delta", U_32, 1, NULL, 0},
    { 29, "T0T1_div_low_strength", U_32, 1, NULL, 0},
    { 30, "T0T1_div_mid_strength", U_32, 1, NULL, 0},
    { 31, "T0T1_div_high", U_32, 1, NULL, 0},
    { 32, "T0T1_div_high_delta", U_32, 1, NULL, 0},
    { 33, "T0T1_div_high_strength", U_32, 1, NULL, 0},
    { 34, "T0T1_div_method", U_32, 1, NULL, 0},
    { 35, "max_change_up", U_32, 1, NULL, 0},
    { 36, "max_change_down", U_32, 1, NULL, 0},
};

#define HiHighSharpeningBothParams_Count 6
static PARAM_s HiHighSharpeningBothParams[HiHighSharpeningBothParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "mode", U_32, 1, NULL, 0},
    {2, "edge_thresh", U_32, 1, NULL, 0},
    {3, "wide_edge_detect", U_32, 1, NULL, 0},
    {4, "max_change_up5x5", U_32, 1, NULL, 0},
    {5, "max_change_down5x5", U_32, 1, NULL, 0},
};

#define HiHighSharpeningNoiseParams_Count 22
static PARAM_s HiHighSharpeningNoiseParams[HiHighSharpeningNoiseParams_Count] = {
    { 0, "max_change_up", U_32, 1, NULL, 0},
    { 1, "max_change_down", U_32, 1, NULL, 0},
    { 2, "fir_specify", U_32, 1, NULL, 0},
    { 3, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    { 4, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    { 5, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    { 6, "fir_coefs", S_32, 225, NULL, 0},
    { 7, "fir_strength_iso", U_32, 1, NULL, 0},
    { 8, "fir_strength_dir", U_32, 1, NULL, 0},
    { 9, "level_str_adjust_low", U_32, 1, NULL, 0},
    { 10, "level_str_adjust_low_delta", U_32, 1, NULL, 0},
    { 11, "level_str_adjust_low_strength", U_32, 1, NULL, 0},
    { 12, "level_str_adjust_mid_strength", U_32, 1, NULL, 0},
    { 13, "level_str_adjust_high", U_32, 1, NULL, 0},
    { 14, "level_str_adjust_high_delta", U_32, 1, NULL, 0},
    { 15, "level_str_adjust_high_strength", U_32, 1, NULL, 0},
    { 16, "level_str_adjust_method", U_32, 1, NULL, 0},
    { 17, "level_str_adjust_not_T0T1_level_based", U_32, 1, NULL, 0},
    { 18, "T0", U_32, 1, NULL, 0},
    { 19, "T1", U_32, 1, NULL, 0},
    { 20, "alpha_min", U_32, 1, NULL, 0},
    { 21, "alpha_max", U_32, 1, NULL, 0},
};

#define HiHighFirstSharpeningFirParams_Count 7
static PARAM_s HiHighFirstSharpeningFirParams[HiHighFirstSharpeningFirParams_Count] = {
    {0, "fir_specify", U_32, 1, NULL, 0},
    {1, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    {2, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    {3, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    {4, "fir_coefs", S_32, 225, NULL, 0},
    {5, "fir_strength_iso", U_32, 1, NULL, 0},
    {6, "fir_strength_dir", U_32, 1, NULL, 0},
};

#define HiHighSharpeningCoringParams_Count 2
static PARAM_s HiHighSharpeningCoringParams[HiHighSharpeningCoringParams_Count] = {
    {0, "coring_table", U_32, IK_NUM_CORING_TABLE_INDEX, NULL, 0},
    {1, "fractional_bits", U_32, 1, NULL, 0},
};

#define HiHighSharpeningLevelParams_Count 8
static PARAM_s HiHighSharpeningLevelParams[HiHighSharpeningLevelParams_Count] = {
    {0, "low", U_32, 1, NULL, 0},
    {1, "low_delta", U_32, 1, NULL, 0},
    {2, "low_strength", U_32, 1, NULL, 0},
    {3, "mid_strength", U_32, 1, NULL, 0},
    {4, "high", U_32, 1, NULL, 0},
    {5, "high_delta", U_32, 1, NULL, 0},
    {6, "high_strength", U_32, 1, NULL, 0},
    {7, "method", U_32, 1, NULL, 0},
};

#define HiLowLumaAdvanceSpatialFilterParams_Count 35
static PARAM_s HiLowLumaAdvanceSpatialFilterParams[HiLowLumaAdvanceSpatialFilterParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "fir_specify", U_32, 1, NULL, 0},
    { 2, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    { 3, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    { 4, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    { 5, "fir_coefs", S_32, 225, NULL, 0},
    { 6, "fir_strength_iso", U_32, 1, NULL, 0},
    { 7, "fir_strength_dir", U_32, 1, NULL, 0},
    { 8, "fir_wide_edge_detect", U_32, 1, NULL, 0},
    { 9, "directional_decide_t0", U_32, 1, NULL, 0},
    { 10, "directional_decide_t1", U_32, 1, NULL, 0},
    { 11, "alpha_min_up", U_32, 1, NULL, 0},
    { 12, "alpha_max_up", U_32, 1, NULL, 0},
    { 13, "T0_up", U_32, 1, NULL, 0},
    { 14, "T1_up", U_32, 1, NULL, 0},
    { 15, "alpha_min_down", U_32, 1, NULL, 0},
    { 16, "alpha_max_down", U_32, 1, NULL, 0},
    { 17, "T0_down", U_32, 1, NULL, 0},
    { 18, "T1_down", U_32, 1, NULL, 0},
    { 19, "level_str_adjust_low", U_32, 1, NULL, 0},
    { 20, "level_str_adjust_low_delta", U_32, 1, NULL, 0},
    { 21, "level_str_adjust_low_strength", U_32, 1, NULL, 0},
    { 22, "level_str_adjust_mid_strength", U_32, 1, NULL, 0},
    { 23, "level_str_adjust_high", U_32, 1, NULL, 0},
    { 24, "level_str_adjust_high_delta", U_32, 1, NULL, 0},
    { 25, "level_str_adjust_high_strength", U_32, 1, NULL, 0},
    { 26, "T0T1_div_low", U_32, 1, NULL, 0},
    { 27, "T0T1_div_low_delta", U_32, 1, NULL, 0},
    { 28, "T0T1_div_low_strength", U_32, 1, NULL, 0},
    { 29, "T0T1_div_mid_strength", U_32, 1, NULL, 0},
    { 30, "T0T1_div_high", U_32, 1, NULL, 0},
    { 31, "T0T1_div_high_delta", U_32, 1, NULL, 0},
    { 32, "T0T1_div_high_strength", U_32, 1, NULL, 0},
    { 33, "max_change_up", U_32, 1, NULL, 0},
    { 34, "max_change_down", U_32, 1, NULL, 0},
};

#define HiLowLumaAdvanceSpatialFilterCombineParams_Count 21
static PARAM_s HiLowLumaAdvanceSpatialFilterCombineParams[HiLowLumaAdvanceSpatialFilterCombineParams_Count] = {
    { 0, "T0", U_32, 1, NULL, 0},
    { 1, "T1", U_32, 1, NULL, 0},
    { 2, "alpha_max", U_32, 1, NULL, 0},
    { 3, "alpha_min", U_32, 1, NULL, 0},
    { 4, "signal_preserve", U_32, 1, NULL, 0},
    { 5, "T0T1_add_low", U_32, 1, NULL, 0},
    { 6, "T0T1_add_low_delta", U_32, 1, NULL, 0},
    { 7, "T0T1_add_low_strength", U_32, 1, NULL, 0},
    { 8, "T0T1_add_mid_strength", U_32, 1, NULL, 0},
    { 9, "T0T1_add_high", U_32, 1, NULL, 0},
    { 10, "T0T1_add_high_delta", U_32, 1, NULL, 0},
    { 11, "T0T1_add_high_strength", U_32, 1, NULL, 0},
    { 12, "T0T1_add_method", U_32, 1, NULL, 0},
    { 13, "max_change_low", U_32, 1, NULL, 0},
    { 14, "max_change_low_delta", U_32, 1, NULL, 0},
    { 15, "max_change_low_strength", U_32, 1, NULL, 0},
    { 16, "max_change_mid_strength", U_32, 1, NULL, 0},
    { 17, "max_change_high", U_32, 1, NULL, 0},
    { 18, "max_change_high_delta", U_32, 1, NULL, 0},
    { 19, "max_change_high_strength", U_32, 1, NULL, 0},
    { 20, "max_change_method", U_32, 1, NULL, 0},
};

#define HiLumaNoiseCombineParams_Count 21
static PARAM_s HiLumaNoiseCombineParams[HiLumaNoiseCombineParams_Count] = {
    { 0, "T0", U_32, 1, NULL, 0},
    { 1, "T1", U_32, 1, NULL, 0},
    { 2, "alpha_max", U_32, 1, NULL, 0},
    { 3, "alpha_min", U_32, 1, NULL, 0},
    { 4, "signal_preserve", U_32, 1, NULL, 0},
    { 5, "T0T1_add_low", U_32, 1, NULL, 0},
    { 6, "T0T1_add_low_delta", U_32, 1, NULL, 0},
    { 7, "T0T1_add_low_strength", U_32, 1, NULL, 0},
    { 8, "T0T1_add_mid_strength", U_32, 1, NULL, 0},
    { 9, "T0T1_add_high", U_32, 1, NULL, 0},
    { 10, "T0T1_add_high_delta", U_32, 1, NULL, 0},
    { 11, "T0T1_add_high_strength", U_32, 1, NULL, 0},
    { 12, "T0T1_add_method", U_32, 1, NULL, 0},
    { 13, "max_change_low", U_32, 1, NULL, 0},
    { 14, "max_change_low_delta", U_32, 1, NULL, 0},
    { 15, "max_change_low_strength", U_32, 1, NULL, 0},
    { 16, "max_change_mid_strength", U_32, 1, NULL, 0},
    { 17, "max_change_high", U_32, 1, NULL, 0},
    { 18, "max_change_high_delta", U_32, 1, NULL, 0},
    { 19, "max_change_high_strength", U_32, 1, NULL, 0},
    { 20, "max_change_method", U_32, 1, NULL, 0},
};

#define HiMedian1LumaAdvanceSpatialFilterParams_Count 38
static PARAM_s HiMedian1LumaAdvanceSpatialFilterParams[HiMedian1LumaAdvanceSpatialFilterParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "fir_specify", U_32, 1, NULL, 0},
    { 2, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    { 3, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    { 4, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    { 5, "fir_coefs", S_32, 225, NULL, 0},
    { 6, "fir_strength_iso", U_32, 1, NULL, 0},
    { 7, "fir_strength_dir", U_32, 1, NULL, 0},
    { 8, "fir_wide_edge_detect", U_32, 1, NULL, 0},
    { 9, "directional_decide_t0", U_32, 1, NULL, 0},
    { 10, "directional_decide_t1", U_32, 1, NULL, 0},
    { 11, "alpha_min_up", U_32, 1, NULL, 0},
    { 12, "alpha_max_up", U_32, 1, NULL, 0},
    { 13, "T0_up", U_32, 1, NULL, 0},
    { 14, "T1_up", U_32, 1, NULL, 0},
    { 15, "alpha_min_down", U_32, 1, NULL, 0},
    { 16, "alpha_max_down", U_32, 1, NULL, 0},
    { 17, "T0_down", U_32, 1, NULL, 0},
    { 18, "T1_down", U_32, 1, NULL, 0},
    { 19, "level_str_adjust_low", U_32, 1, NULL, 0},
    { 20, "level_str_adjust_low_delta", U_32, 1, NULL, 0},
    { 21, "level_str_adjust_low_strength", U_32, 1, NULL, 0},
    { 22, "level_str_adjust_mid_strength", U_32, 1, NULL, 0},
    { 23, "level_str_adjust_high", U_32, 1, NULL, 0},
    { 24, "level_str_adjust_high_delta", U_32, 1, NULL, 0},
    { 25, "level_str_adjust_high_strength", U_32, 1, NULL, 0},
    { 26, "level_str_adjust_method", U_32, 1, NULL, 0},
    { 27, "T0T1_div_low", U_32, 1, NULL, 0},
    { 28, "T0T1_div_low_delta", U_32, 1, NULL, 0},
    { 29, "T0T1_div_low_strength", U_32, 1, NULL, 0},
    { 30, "T0T1_div_mid_strength", U_32, 1, NULL, 0},
    { 31, "T0T1_div_high", U_32, 1, NULL, 0},
    { 32, "T0T1_div_high_delta", U_32, 1, NULL, 0},
    { 33, "T0T1_div_high_strength", U_32, 1, NULL, 0},
    { 34, "T0T1_div_method", U_32, 1, NULL, 0},
    { 35, "max_change_not_T0T1_alpha", U_32, 1, NULL, 0}, // H22 coring
    { 36, "max_change_up", U_32, 1, NULL, 0},
    { 37, "max_change_down", U_32, 1, NULL, 0},
};

#define HiMedian2LumaAdvanceSpatialFilterParams_Count 37
static PARAM_s HiMedian2LumaAdvanceSpatialFilterParams[HiMedian2LumaAdvanceSpatialFilterParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "fir_specify", U_32, 1, NULL, 0},
    { 2, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    { 3, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    { 4, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    { 5, "fir_coefs", S_32, 225, NULL, 0},
    { 6, "fir_strength_iso", U_32, 1, NULL, 0},
    { 7, "fir_strength_dir", U_32, 1, NULL, 0},
    { 8, "fir_wide_edge_detect", U_32, 1, NULL, 0},
    { 9, "directional_decide_t0", U_32, 1, NULL, 0},
    { 10, "directional_decide_t1", U_32, 1, NULL, 0},
    { 11, "alpha_min_up", U_32, 1, NULL, 0},
    { 12, "alpha_max_up", U_32, 1, NULL, 0},
    { 13, "T0_up", U_32, 1, NULL, 0},
    { 14, "T1_up", U_32, 1, NULL, 0},
    { 15, "alpha_min_down", U_32, 1, NULL, 0},
    { 16, "alpha_max_down", U_32, 1, NULL, 0},
    { 17, "T0_down", U_32, 1, NULL, 0},
    { 18, "T1_down", U_32, 1, NULL, 0},
    { 19, "level_str_adjust_low", U_32, 1, NULL, 0},
    { 20, "level_str_adjust_low_delta", U_32, 1, NULL, 0},
    { 21, "level_str_adjust_low_strength", U_32, 1, NULL, 0},
    { 22, "level_str_adjust_mid_strength", U_32, 1, NULL, 0},
    { 23, "level_str_adjust_high", U_32, 1, NULL, 0},
    { 24, "level_str_adjust_high_delta", U_32, 1, NULL, 0},
    { 25, "level_str_adjust_high_strength", U_32, 1, NULL, 0},
    { 26, "level_str_adjust_method", U_32, 1, NULL, 0},
    { 27, "T0T1_div_low", U_32, 1, NULL, 0},
    { 28, "T0T1_div_low_delta", U_32, 1, NULL, 0},
    { 29, "T0T1_div_low_strength", U_32, 1, NULL, 0},
    { 30, "T0T1_div_mid_strength", U_32, 1, NULL, 0},
    { 31, "T0T1_div_high", U_32, 1, NULL, 0},
    { 32, "T0T1_div_high_delta", U_32, 1, NULL, 0},
    { 33, "T0T1_div_high_strength", U_32, 1, NULL, 0},
    { 34, "T0T1_div_method", U_32, 1, NULL, 0},
    { 35, "max_change_up", U_32, 1, NULL, 0},
    { 36, "max_change_down", U_32, 1, NULL, 0},
};

#define HiMedianSharpeningBothParams_Count 6
static PARAM_s HiMedianSharpeningBothParams[HiMedianSharpeningBothParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "mode", U_32, 1, NULL, 0},
    {2, "edge_thresh", U_32, 1, NULL, 0},
    {3, "wide_edge_detect", U_32, 1, NULL, 0},
    {4, "max_change_up5x5", U_32, 1, NULL, 0},
    {5, "max_change_down5x5", U_32, 1, NULL, 0},
};

#define HiMedianSharpeningNoiseParams_Count 22
static PARAM_s HiMedianSharpeningNoiseParams[HiMedianSharpeningNoiseParams_Count] = {
    { 0, "max_change_up", U_32, 1, NULL, 0},
    { 1, "max_change_down", U_32, 1, NULL, 0},
    { 2, "fir_specify", U_32, 1, NULL, 0},
    { 3, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    { 4, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    { 5, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    { 6, "fir_coefs", S_32, 225, NULL, 0},
    { 7, "fir_strength_iso", U_32, 1, NULL, 0},
    { 8, "fir_strength_dir", U_32, 1, NULL, 0},
    { 9, "level_str_adjust_low", U_32, 1, NULL, 0},
    { 10, "level_str_adjust_low_delta", U_32, 1, NULL, 0},
    { 11, "level_str_adjust_low_strength", U_32, 1, NULL, 0},
    { 12, "level_str_adjust_mid_strength", U_32, 1, NULL, 0},
    { 13, "level_str_adjust_high", U_32, 1, NULL, 0},
    { 14, "level_str_adjust_high_delta", U_32, 1, NULL, 0},
    { 15, "level_str_adjust_high_strength", U_32, 1, NULL, 0},
    { 16, "level_str_adjust_method", U_32, 1, NULL, 0},
    { 17, "level_str_adjust_not_T0T1_level_based", U_32, 1, NULL, 0},
    { 18, "T0", U_32, 1, NULL, 0},
    { 19, "T1", U_32, 1, NULL, 0},
    { 20, "alpha_min", U_32, 1, NULL, 0},
    { 21, "alpha_max", U_32, 1, NULL, 0},
};

#define HiMedianFirstSharpeningFirParams_Count 7
static PARAM_s HiMedianFirstSharpeningFirParams[HiMedianFirstSharpeningFirParams_Count] = {
    {0, "fir_specify", U_32, 1, NULL, 0},
    {1, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    {2, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    {3, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    {4, "fir_coefs", S_32, 225, NULL, 0},
    {5, "fir_strength_iso", U_32, 1, NULL, 0},
    {6, "fir_strength_dir", U_32, 1, NULL, 0},
};

#define HiMedianSharpeningCoringParams_Count 2
static PARAM_s HiMedianSharpeningCoringParams[HiMedianSharpeningCoringParams_Count] = {
    {0, "coring_table", U_32, IK_NUM_CORING_TABLE_INDEX, NULL, 0},
    {1, "fractional_bits", U_32, 1, NULL, 0},
};

#define HiMedianSharpeningLevelParams_Count 8
static PARAM_s HiMedianSharpeningLevelParams[HiMedianSharpeningLevelParams_Count] = {
    {0, "low", U_32, 1, NULL, 0},
    {1, "low_delta", U_32, 1, NULL, 0},
    {2, "low_strength", U_32, 1, NULL, 0},
    {3, "mid_strength", U_32, 1, NULL, 0},
    {4, "high", U_32, 1, NULL, 0},
    {5, "high_delta", U_32, 1, NULL, 0},
    {6, "high_strength", U_32, 1, NULL, 0},
    {7, "method", U_32, 1, NULL, 0},
};

#define HiSelectParams_Count 1
static PARAM_s HiSelectParams[HiSelectParams_Count] = {
    {0, "use_high_shp_not_high2_asf", U_32, 1, NULL, 0},
};

#define HiLumaBlendParams_Count 8
static PARAM_s HiLumaBlendParams[HiLumaBlendParams_Count] = {
    {0, "lev_based_low", U_32, 1, NULL, 0},
    {1, "lev_based_low_delta", U_32, 1, NULL, 0},
    {2, "lev_based_low_strength", U_32, 1, NULL, 0},
    {3, "lev_based_mid_strength", U_32, 1, NULL, 0},
    {4, "lev_based_high", U_32, 1, NULL, 0},
    {5, "lev_based_high_delta", U_32, 1, NULL, 0},
    {6, "lev_based_high_strength", U_32, 1, NULL, 0},
    {7, "lev_based_method", U_32, 1, NULL, 0},
};

#define HiLiCombineParams_Count 67
static PARAM_s HiLiCombineParams[HiLiCombineParams_Count] = {
    {0, "T0_cb", U_32, 1, NULL, 0},
    {1, "T0_cr", U_32, 1, NULL, 0},
    {2, "T0_y", U_32, 1, NULL, 0},
    {3, "T1_cb", U_32, 1, NULL, 0},
    {4, "T1_cr", U_32, 1, NULL, 0},
    {5, "T1_y", U_32, 1, NULL, 0},
    {6, "alpha_max_y", U_32, 1, NULL, 0},
    {7, "alpha_max_cb", U_32, 1, NULL, 0},
    {8, "alpha_max_cr", U_32, 1, NULL, 0},
    {9, "alpha_min_y", U_32, 1, NULL, 0},
    {10, "alpha_min_cb", U_32, 1, NULL, 0},
    {11, "alpha_min_cr", U_32, 1, NULL, 0},
    {12, "max_change_low_y", U_32, 1, NULL, 0},
    {13, "max_change_low_delta_y", U_32, 1, NULL, 0},
    {14, "max_change_low_strength_y", U_32, 1, NULL, 0},
    {15, "max_change_mid_strength_y", U_32, 1, NULL, 0},
    {16, "max_change_high_y", U_32, 1, NULL, 0},
    {17, "max_change_high_delta_y", U_32, 1, NULL, 0},
    {18, "max_change_high_strength_y", U_32, 1, NULL, 0},
    {19, "max_change_method_y", U_32, 1, NULL, 0},
    {20, "max_change_low_cb", U_32, 1, NULL, 0},
    {21, "max_change_low_delta_cb", U_32, 1, NULL, 0},
    {22, "max_change_low_strength_cb", U_32, 1, NULL, 0},
    {23, "max_change_mid_strength_cb", U_32, 1, NULL, 0},
    {24, "max_change_high_cb", U_32, 1, NULL, 0},
    {25, "max_change_high_delta_cb", U_32, 1, NULL, 0},
    {26, "max_change_high_strength_cb", U_32, 1, NULL, 0},
    {27, "max_change_low_cr", U_32, 1, NULL, 0},
    {28, "max_change_low_delta_cr", U_32, 1, NULL, 0},
    {29, "max_change_low_strength_cr", U_32, 1, NULL, 0},
    {30, "max_change_mid_strength_cr", U_32, 1, NULL, 0},
    {31, "max_change_high_cr", U_32, 1, NULL, 0},
    {32, "max_change_high_delta_cr", U_32, 1, NULL, 0},
    {33, "max_change_high_strength_cr", U_32, 1, NULL, 0},
    {34, "T0T1_add_low_y", U_32, 1, NULL, 0},
    {35, "T0T1_add_low_delta_y", U_32, 1, NULL, 0},
    {36, "T0T1_add_low_strength_y", U_32, 1, NULL, 0},
    {37, "T0T1_add_mid_strength_y", U_32, 1, NULL, 0},
    {38, "T0T1_add_high_y", U_32, 1, NULL, 0},
    {39, "T0T1_add_high_delta_y", U_32, 1, NULL, 0},
    {40, "T0T1_add_high_strength_y", U_32, 1, NULL, 0},
    {41, "T0T1_add_method_y", U_32, 1, NULL, 0},
    {42, "T0T1_add_low_cb", U_32, 1, NULL, 0},
    {43, "T0T1_add_low_delta_cb", U_32, 1, NULL, 0},
    {44, "T0T1_add_low_strength_cb", U_32, 1, NULL, 0},
    {45, "T0T1_add_mid_strength_cb", U_32, 1, NULL, 0},
    {46, "T0T1_add_high_cb", U_32, 1, NULL, 0},
    {47, "T0T1_add_high_delta_cb", U_32, 1, NULL, 0},
    {48, "T0T1_add_high_strength_cb", U_32, 1, NULL, 0},
    {49, "T0T1_add_low_cr", U_32, 1, NULL, 0},
    {50, "T0T1_add_low_delta_cr", U_32, 1, NULL, 0},
    {51, "T0T1_add_low_strength_cr", U_32, 1, NULL, 0},
    {52, "T0T1_add_mid_strength_cr", U_32, 1, NULL, 0},
    {53, "T0T1_add_high_cr", U_32, 1, NULL, 0},
    {54, "T0T1_add_high_delta_cr", U_32, 1, NULL, 0},
    {55, "T0T1_add_high_strength_cr", U_32, 1, NULL, 0},
    {56, "signal_preserve_cb", U_32, 1, NULL, 0},
    {57, "signal_preserve_cr", U_32, 1, NULL, 0},
    {58, "signal_preserve_y", U_32, 1, NULL, 0},
    {59, "min_tar_low_y", U_32, 1, NULL, 0},
    {60, "min_tar_low_delta_y", U_32, 1, NULL, 0},
    {61, "min_tar_low_strength_y", U_32, 1, NULL, 0},
    {62, "min_tar_mid_strength_y", U_32, 1, NULL, 0},
    {63, "min_tar_high_y", U_32, 1, NULL, 0},
    {64, "min_tar_high_delta_y", U_32, 1, NULL, 0},
    {65, "min_tar_high_strength_y", U_32, 1, NULL, 0},
    {66, "min_tar_method_y", U_32, 1, NULL, 0},

};

#define HiLiSharpeningBothParams_Count 8
static PARAM_s HiLiSharpeningBothParams[HiLiSharpeningBothParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "mode", U_32, 1, NULL, 0},
    {2, "edge_thresh", U_32, 1, NULL, 0},
    {3, "wide_edge_detect", U_32, 1, NULL, 0},
    {4, "max_change_up5x5", U_32, 1, NULL, 0},
    {5, "max_change_down5x5", U_32, 1, NULL, 0},
    {6, "max_change_up", U_32, 1, NULL, 0},
    {7, "max_change_down", U_32, 1, NULL, 0},
};

#define HiLiSharpeningNoiseParams_Count 22
static PARAM_s HiLiSharpeningNoiseParams[HiLiSharpeningNoiseParams_Count] = {
    { 0, "max_change_up", U_32, 1, NULL, 0},
    { 1, "max_change_down", U_32, 1, NULL, 0},
    { 2, "fir_specify", U_32, 1, NULL, 0},
    { 3, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    { 4, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    { 5, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    { 6, "fir_coefs", S_32, 225, NULL, 0},
    { 7, "fir_strength_iso", U_32, 1, NULL, 0},
    { 8, "fir_strength_dir", U_32, 1, NULL, 0},
    { 9, "level_str_adjust_low", U_32, 1, NULL, 0},
    { 10, "level_str_adjust_low_delta", U_32, 1, NULL, 0},
    { 11, "level_str_adjust_low_strength", U_32, 1, NULL, 0},
    { 12, "level_str_adjust_mid_strength", U_32, 1, NULL, 0},
    { 13, "level_str_adjust_high", U_32, 1, NULL, 0},
    { 14, "level_str_adjust_high_delta", U_32, 1, NULL, 0},
    { 15, "level_str_adjust_high_strength", U_32, 1, NULL, 0},
    { 16, "level_str_adjust_method", U_32, 1, NULL, 0},
    { 17, "level_str_adjust_not_T0T1_level_based", U_32, 1, NULL, 0},
    { 18, "T0", U_32, 1, NULL, 0},
    { 19, "T1", U_32, 1, NULL, 0},
    { 20, "alpha_min", U_32, 1, NULL, 0},
    { 21, "alpha_max", U_32, 1, NULL, 0},
};

#define HiLiFirstSharpeningFirParams_Count 7
static PARAM_s HiLiFirstSharpeningFirParams[HiLiFirstSharpeningFirParams_Count] = {
    {0, "fir_specify", U_32, 1, NULL, 0},
    {1, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    {2, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    {3, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    {4, "fir_coefs", S_32, 225, NULL, 0},
    {5, "fir_strength_iso", U_32, 1, NULL, 0},
    {6, "fir_strength_dir", U_32, 1, NULL, 0},
};

#define HiLiSharpeningCoringParams_Count 1
static PARAM_s HiLiSharpeningCoringParams[HiLiSharpeningCoringParams_Count] = {
    {0, "coring_table", U_32, IK_NUM_CORING_TABLE_INDEX, NULL, 0},
};

#define HiLiSharpeningLevelParams_Count 8
static PARAM_s HiLiSharpeningLevelParams[HiLiSharpeningLevelParams_Count] = {
    {0, "low", U_32, 1, NULL, 0},
    {1, "low_delta", U_32, 1, NULL, 0},
    {2, "low_strength", U_32, 1, NULL, 0},
    {3, "mid_strength", U_32, 1, NULL, 0},
    {4, "high", U_32, 1, NULL, 0},
    {5, "high_delta", U_32, 1, NULL, 0},
    {6, "high_strength", U_32, 1, NULL, 0},
    {7, "method", U_32, 1, NULL, 0},
};

#define Li2LumaAdvanceSpatialFilterParams_Count 36
static PARAM_s Li2LumaAdvanceSpatialFilterParams[Li2LumaAdvanceSpatialFilterParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "fir_specify", U_32, 1, NULL, 0},
    { 2, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    { 3, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    { 4, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    { 5, "fir_coefs", S_32, 225, NULL, 0},
    { 6, "fir_strength_iso", U_32, 1, NULL, 0},
    { 7, "fir_strength_dir", U_32, 1, NULL, 0},
    { 8, "fir_wide_edge_detect", U_32, 1, NULL, 0},
    { 9, "directional_decide_t0", U_32, 1, NULL, 0},
    { 10, "directional_decide_t1", U_32, 1, NULL, 0},
    { 11, "alpha_min_up", U_32, 1, NULL, 0},
    { 12, "alpha_max_up", U_32, 1, NULL, 0},
    { 13, "T0_up", U_32, 1, NULL, 0},
    { 14, "T1_up", U_32, 1, NULL, 0},
    { 15, "alpha_min_down", U_32, 1, NULL, 0},
    { 16, "alpha_max_down", U_32, 1, NULL, 0},
    { 17, "T0_down", U_32, 1, NULL, 0},
    { 18, "T1_down", U_32, 1, NULL, 0},
    { 19, "level_str_adjust_low", U_32, 1, NULL, 0},
    { 20, "level_str_adjust_low_delta", U_32, 1, NULL, 0},
    { 21, "level_str_adjust_low_strength", U_32, 1, NULL, 0},
    { 22, "level_str_adjust_mid_strength", U_32, 1, NULL, 0},
    { 23, "level_str_adjust_high", U_32, 1, NULL, 0},
    { 24, "level_str_adjust_high_delta", U_32, 1, NULL, 0},
    { 25, "level_str_adjust_high_strength", U_32, 1, NULL, 0},
    { 26, "T0T1_div_low", U_32, 1, NULL, 0},
    { 27, "T0T1_div_low_delta", U_32, 1, NULL, 0},
    { 28, "T0T1_div_low_strength", U_32, 1, NULL, 0},
    { 29, "T0T1_div_mid_strength", U_32, 1, NULL, 0},
    { 30, "T0T1_div_high", U_32, 1, NULL, 0},
    { 31, "T0T1_div_high_delta", U_32, 1, NULL, 0},
    { 32, "T0T1_div_high_strength", U_32, 1, NULL, 0},
    { 33, "max_change_not_T0T1_alpha", U_32, 1, NULL, 0}, // H22 coring
    { 34, "max_change_up", U_32, 1, NULL, 0},
    { 35, "max_change_down", U_32, 1, NULL, 0},
};

#define Li2AntiAliasingParams_Count 3
static PARAM_s Li2AntiAliasingParams[Li2AntiAliasingParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "thresh", U_32, 1, NULL, 0},
    {2, "log_fractional_correct", U_32, 1, NULL, 0},
};

#define Li2AutoBadPixelCorrectionParams_Count 4
static PARAM_s Li2AutoBadPixelCorrectionParams[Li2AutoBadPixelCorrectionParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "hot_pixel_detection_strength", U_32, 1, NULL, 0},
    {2, "dark_pixel_detection_strength", U_32, 1, NULL, 0},
    {3, "correction_method", U_32, 1, NULL, 0},
};

#define Li2CfaLeakageFilterParams_Count 6
static PARAM_s Li2CfaLeakageFilterParams[Li2CfaLeakageFilterParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "alpha_rr", S_32, 1, NULL, 0},
    {2, "alpha_rb", S_32, 1, NULL, 0},
    {3, "alpha_br", S_32, 1, NULL, 0},
    {4, "alpha_bb", S_32, 1, NULL, 0},
    {5, "saturation_level", U_32, 1, NULL, 0},
};

#define Li2CfaNoiseFilterParams_Count 53
static PARAM_s Li2CfaNoiseFilterParams[Li2CfaNoiseFilterParams_Count] = {
    {0, "enable", U_32, 1, 0, 0},
    {1, "min_noise_level_red", U_32, 1, 0, 0},
    {2, "min_noise_level_green", U_32, 1, 0, 0},
    {3, "min_noise_level_blue", U_32, 1, 0, 0},
    {4, "max_noise_level_red", U_32, 1, 0, 0},
    {5, "max_noise_level_green", U_32, 1, 0, 0},
    {6, "max_noise_level_blue", U_32, 1, 0, 0},
    {7, "original_blend_strength_red", U_32, 1, 0, 0},
    {8, "original_blend_strength_green", U_32, 1, 0, 0},
    {9, "original_blend_strength_blue", U_32, 1, 0, 0},
    {10, "extent_regular_red", U_32, 1, 0, 0},
    {11, "extent_regular_green", U_32, 1, 0, 0},
    {12, "extent_regular_blue", U_32, 1, 0, 0},
    {13, "extent_fine_red", U_32, 1, 0, 0},
    {14, "extent_fine_green", U_32, 1, 0, 0},
    {15, "extent_fine_blue", U_32, 1, 0, 0},
    {16, "strength_fine_red", U_32, 1, 0, 0},
    {17, "strength_fine_green", U_32, 1, 0, 0},
    {18, "strength_fine_blue", U_32, 1, 0, 0},
    {19, "selectivity_regular", U_32, 1, 0, 0},
    {20, "selectivity_fine", U_32, 1, 0, 0},
    {21, "directional_enable", U_32, 1, 0, 0},
    {22, "directional_horvert_edge_strength", U_32, 1, 0, 0},
    {23, "directional_horvert_edge_strength_bias", U_32, 1, 0, 0},
    {24, "directional_horvert_original_blend_strength", U_32, 1, 0, 0},
    {25, "dir_lev_max_red", U_32, 1, 0, 0},
    {26, "dir_lev_max_green", U_32, 1, 0, 0},
    {27, "dir_lev_max_blue", U_32, 1, 0, 0},
    {28, "dir_lev_min_red", U_32, 1, 0, 0},
    {29, "dir_lev_min_green", U_32, 1, 0, 0},
    {30, "dir_lev_min_blue", U_32, 1, 0, 0},
    {31, "dir_lev_mul_red", U_32, 1, 0, 0},
    {32, "dir_lev_mul_green", U_32, 1, 0, 0},
    {33, "dir_lev_mul_blue", U_32, 1, 0, 0},
    {34, "dir_lev_offset_red", S_32, 1, 0, 0},
    {35, "dir_lev_offset_green", S_32, 1, 0, 0},
    {36, "dir_lev_offset_blue", S_32, 1, 0, 0},
    {37, "dir_lev_shift_red", U_32, 1, 0, 0},
    {38, "dir_lev_shift_green", U_32, 1, 0, 0},
    {39, "dir_lev_shift_blue", U_32, 1, 0, 0},
    {40, "lev_mul_red", U_32, 1, 0, 0},
    {41, "lev_mul_green", U_32, 1, 0, 0},
    {42, "lev_mul_blue", U_32, 1, 0, 0},
    {43, "lev_offset_red", S_32, 1, 0, 0},
    {44, "lev_offset_green", S_32, 1, 0, 0},
    {45, "lev_offset_blue", S_32, 1, 0, 0},
    {46, "lev_shift_red", U_32, 1, 0, 0},
    {47, "lev_shift_green", U_32, 1, 0, 0},
    {48, "lev_shift_blue", U_32, 1, 0, 0},
    {49, "apply_to_color_diff_red", U_32, 1, 0, 0},
    {50, "apply_to_color_diff_blue", U_32, 1, 0, 0},
    {51, "opt_for_rccb_red", U_32, 1, 0, 0},
    {52, "opt_for_rccb_blue", U_32, 1, 0, 0},
};

#define Li2DemosaicFilterParams_Count 7
static PARAM_s Li2DemosaicFilterParams[Li2DemosaicFilterParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "activity_thresh", U_32, 1, NULL, 0},
    {2, "activity_difference_thresh", U_32, 1, NULL, 0},
    {3, "grad_clip_thresh", U_32, 1, NULL, 0},
    {4, "grad_noise_thresh", U_32, 1, NULL, 0},
    {5, "alias_interpolation_strength", U_32, 1, NULL, 0},
    {6, "alias_interpolation_thresh", U_32, 1, NULL, 0},
};

#define Li2GbGrMismatchParams_Count 4
static PARAM_s Li2GbGrMismatchParams[Li2GbGrMismatchParams_Count] = {
    {0, "narrow_enable", U_32, 1, NULL, 0},
    {1, "wide_enable", U_32, 1, NULL, 0},
    {2, "wide_safety", U_32, 1, NULL, 0},
    {3, "wide_thresh", U_32, 1, NULL, 0},
};

#define Li2SharpeningBothParams_Count 8
static PARAM_s Li2SharpeningBothParams[Li2SharpeningBothParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "mode", U_32, 1, NULL, 0},
    {2, "edge_thresh", U_32, 1, NULL, 0},
    {3, "wide_edge_detect", U_32, 1, NULL, 0},
    {4, "max_change_up5x5", U_32, 1, NULL, 0},
    {5, "max_change_down5x5", U_32, 1, NULL, 0},
    {6, "max_change_up", U_32, 1, NULL, 0},
    {7, "max_change_down", U_32, 1, NULL, 0},
};

#define Li2SharpeningNoiseParams_Count 22
static PARAM_s Li2SharpeningNoiseParams[Li2SharpeningNoiseParams_Count] = {
    { 0, "max_change_up", U_32, 1, NULL, 0},
    { 1, "max_change_down", U_32, 1, NULL, 0},
    { 2, "fir_specify", U_32, 1, NULL, 0},
    { 3, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    { 4, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    { 5, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    { 6, "fir_coefs", S_32, 225, NULL, 0},
    { 7, "fir_strength_iso", U_32, 1, NULL, 0},
    { 8, "fir_strength_dir", U_32, 1, NULL, 0},
    { 9, "level_str_adjust_low", U_32, 1, NULL, 0},
    { 10, "level_str_adjust_low_delta", U_32, 1, NULL, 0},
    { 11, "level_str_adjust_low_strength", U_32, 1, NULL, 0},
    { 12, "level_str_adjust_mid_strength", U_32, 1, NULL, 0},
    { 13, "level_str_adjust_high", U_32, 1, NULL, 0},
    { 14, "level_str_adjust_high_delta", U_32, 1, NULL, 0},
    { 15, "level_str_adjust_high_strength", U_32, 1, NULL, 0},
    { 16, "level_str_adjust_method", U_32, 1, NULL, 0},
    { 17, "level_str_adjust_not_T0T1_level_based", U_32, 1, NULL, 0},
    { 18, "T0", U_32, 1, NULL, 0},
    { 19, "T1", U_32, 1, NULL, 0},
    { 20, "alpha_min", U_32, 1, NULL, 0},
    { 21, "alpha_max", U_32, 1, NULL, 0},
};

#define Li2FirstSharpeningFirParams_Count 7
static PARAM_s Li2FirstSharpeningFirParams[Li2FirstSharpeningFirParams_Count] = {
    {0, "fir_specify", U_32, 1, NULL, 0},
    {1, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    {2, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    {3, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    {4, "fir_coefs", S_32, 225, NULL, 0},
    {5, "fir_strength_iso", U_32, 1, NULL, 0},
    {6, "fir_strength_dir", U_32, 1, NULL, 0},
};

#define Li2SharpeningCoringParams_Count 1
static PARAM_s Li2SharpeningCoringParams[Li2SharpeningCoringParams_Count] = {
    {0, "coring_table", U_32, IK_NUM_CORING_TABLE_INDEX, NULL, 0},
};

#define Li2SharpeningLevelParams_Count 8
static PARAM_s Li2SharpeningLevelParams[Li2SharpeningLevelParams_Count] = {
    {0, "low", U_32, 1, NULL, 0},
    {1, "low_delta", U_32, 1, NULL, 0},
    {2, "low_strength", U_32, 1, NULL, 0},
    {3, "mid_strength", U_32, 1, NULL, 0},
    {4, "high", U_32, 1, NULL, 0},
    {5, "high_delta", U_32, 1, NULL, 0},
    {6, "high_strength", U_32, 1, NULL, 0},
    {7, "method", U_32, 1, NULL, 0},
};

#define HiNonsmoothDetectParams_Count 2
static PARAM_s HiNonsmoothDetectParams[HiNonsmoothDetectParams_Count] = {
    {0, "smooth", U_32, 1, NULL, 0},
    {1, "lev_adjust", S_32, 16, NULL, 0},
};

#define ImgRegs_Count 143

static REG_s ImgRegs[ImgRegs_Count] = {
    { ITUNER_SDK_INFO, "sdk_info", 1, 0, SdkInfo_Params_Count, PARAM_STATUS_DEFAULT, SdkInfo_Params, SdkInfo_Proc, 2 },
    { ITUNER_SYSTEM_INFO, "system", 1, 0, System_Params_Count, PARAM_STATUS_DEFAULT, System_Params, System_Proc, 0 },
    { ITUNER_INTERNAL_INFO, "internal", 1, 0, Internal_Params_Count, PARAM_STATUS_DEFAULT, Internal_Params, Internal_Proc, ATTRIBUTE_HIDE },
    { ITUNER_AAA_FUNCTION, "aaa_function", 1, 0, AaaFunctionParams_Count, PARAM_STATUS_DEFAULT, AaaFunctionParams, AaaFunctionProc, 0 },
    { ITUNER_AE_INFO, "ae_info", 1, 0, AeInfoParams_Count, PARAM_STATUS_DEFAULT, AeInfoParams, AeInfoProc, 0 },
    { ITUNER_WB_SIM_INFO, "wb_sim_info", 1, 0, WbSimInfoParams_Count, PARAM_STATUS_DEFAULT, WbSimInfoParams, WbSimInfoProc, 0 },
    { ITUNER_STATIC_BAD_PIXEL_CORRECTION, "static_bad_pixel_correction", 1, 0, StaticBadPixelCorrectionParams_Count, PARAM_STATUS_DEFAULT, StaticBadPixelCorrectionParams, StaticBadPixelCorrectionProc, 0 },
    { ITUNER_STATIC_BAD_PIXEL_INTERNAL, "static_bad_pixel_correction_internal", 1, 0, StaticBadPixelCorrectionInternalParams_Count, PARAM_STATUS_DEFAULT, StaticBadPixelCorrectionInternalParams, StaticBadPixelCorrectionInternalProc, ATTRIBUTE_HIDE },
    { ITUNER_VIGNETTE_COMPENSATION, "vignette_compensation", 1, 0, VignetteCompensationParams_Count, PARAM_STATUS_DEFAULT, VignetteCompensationParams, VignetteCompensationProc, 0 },
    { ITUNER_CHROMA_ABERRATION_INFO, "chromatic_aberration_correction", 1, 0, ChromaAberrationParams_Count, PARAM_STATUS_DEFAULT, ChromaAberrationParams, ChromaAberrationProc, 0 },
    { ITUNER_CHROMA_ABERRATION_INFO_INTERNAL, "chromatic_aberration_correction_internal", 1, 0, ChromaAberrationInternalParams_Count, PARAM_STATUS_DEFAULT, ChromaAberrationInternalParams, ChromaAberrationInternalProc, ATTRIBUTE_HIDE },
    { ITUNER_WARP_COMPENSATION, "warp_compensation", 1, 0, WarpCompensationParams_Count, PARAM_STATUS_DEFAULT, WarpCompensationParams, WarpCompensationProc, 0 },
    { ITUNER_WARP_COMPENSATION_2ND, "warp_compensation_2nd", 1, 0, WarpCompensationParams_Count, PARAM_STATUS_DEFAULT, WarpCompensationParams, WarpCompensation2ndProc, ATTRIBUTE_HIDE },
    { ITUNER_WARP_COMPENSATION_DZOOM_INTERNAL, "warp_compensation_internal", 1, 0, WarpCompensationDzoomInternalParams_Count, PARAM_STATUS_DEFAULT, WarpCompensationDzoomInternalParams, WarpCompensationDzoomInternalProc, ATTRIBUTE_HIDE },
    { ITUNER_DZOOM, "dzoom", 1, 0, DzoomParams_Count, PARAM_STATUS_DEFAULT, DzoomParams, DzoomProc, 0 },
    { ITUNER_DEFERRED_BLACK_LEVEL, "deferred_black_level", 1, 0, DeferredBlackLevelParams_Count, PARAM_STATUS_DEFAULT, DeferredBlackLevelParams, DeferredBlackLevelProc, 0 },
    { ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION, "li_auto_bad_pixel_correction", 1, 0, DynamicBadPixelCorrectionParams_Count, PARAM_STATUS_DEFAULT, DynamicBadPixelCorrectionParams, DynamicBadPixelCorrectionProc, 0 },
    { ITUNER_CFA_LEAKAGE_FILTER, "li_cfa_leakage_filter", 1, 0, CfaLeakageFilterParams_Count, PARAM_STATUS_DEFAULT, CfaLeakageFilterParams, CfaLeakageFilterProc, 0 },
    { ITUNER_CFA_NOISE_FILTER, "li_cfa_noise_filter", 1, 0, CfaNoiseFilterParams_Count, PARAM_STATUS_DEFAULT, CfaNoiseFilterParams, CfaNoiseFilterProc, 0 },
    { ITUNER_ANTI_ALIASING_STRENGTH, "li_anti_aliasing", 1, 0, AntiAliasingParams_Count, PARAM_STATUS_DEFAULT, AntiAliasingParams, AntiAliasingProc, 0 },
    { ITUNER_BEFORE_CE_WB_GAIN, "before_ce", 1, 0, WbGainParams_Count, PARAM_STATUS_DEFAULT, WbGainParams, BeforeCeWbGainProc, 0 },
    { ITUNER_AFTER_CE_WB_GAIN, "after_ce", 1, 0, WbGainParams_Count, PARAM_STATUS_DEFAULT, WbGainParams, AfterCeWbGainProc, 0 },
    { ITUNER_STORED_IR, "stored_ir", 1, 0, StoredIrParams_Count, PARAM_STATUS_DEFAULT, StoredIrParams, StoredIrProc, 0 },
    { ITUNER_COLOR_CORRECTION, "color_correction", 1, 0, ColorCorrectionParams_Count, PARAM_STATUS_DEFAULT, ColorCorrectionParams, ColorCorrectionProc, 0 },
    { ITUNER_TONE_CURVE, "tone_curve", 1, 0, ToneCurveParams_Count, PARAM_STATUS_DEFAULT, ToneCurveParams, ToneCurveProc, 0 },
    { ITUNER_FRONT_END_TONE_CURVE, "hdr_input", 1, 0, FrontEndToneCurveParams_Count, PARAM_STATUS_DEFAULT, FrontEndToneCurveParams, FrontEndToneCurveProc, 0 },
    { ITUNER_RGB_TO_YUV_MATRIX, "rgb_to_yuv_matrix", 1, 0, RgbToYuvMatrixParams_Count, PARAM_STATUS_DEFAULT, RgbToYuvMatrixParams, RgbToYuvMatrixProc, 0 },
    { ITUNER_RGB_IR, "rgb_ir", 1, 0, RgbIrParams_Count, PARAM_STATUS_DEFAULT, RgbIrParams, RgbIrProc, 0},
    { ITUNER_CHROMA_SCALE, "chroma_scale", 1, 0, ChromaScaleParams_Count, PARAM_STATUS_DEFAULT, ChromaScaleParams, ChromaScaleProc, 0},
    { ITUNER_CHROMA_MEDIAN_FILTER, "li_chroma_median_filter", 1, 0, ChromaMedianFilterParams_Count, PARAM_STATUS_DEFAULT, ChromaMedianFilterParams, ChromaMedianFilterProc, 0 },
    { ITUNER_DEMOSAIC_FILTER, "li_demosaic", 1, 0, DemosaicFilterParams_Count, PARAM_STATUS_DEFAULT, DemosaicFilterParams, DemosaicFilterProc, 0 },
    { ITUNER_RGB_TO_Y12, "rgb_to_y12_matrix", 1, 0, RgbToY12Params_Count, PARAM_STATUS_DEFAULT, RgbToY12Params, RgbToY12Proc, 0 },
    { ITUNER_GB_GR_MISMATCH, "li_gb_gr_mismatch_correct", 1, 0, GbGrMismatchParams_Count, PARAM_STATUS_DEFAULT, GbGrMismatchParams, GbGrMismatchProc, 0 },
    { ITUNER_VIDEO_MCTF, "video_mctf", 1, 0, VideoMctfParams_Count, PARAM_STATUS_DEFAULT, VideoMctfParams, VideoMctfProc, 0 },
    { ITUNER_VIDEO_MCTF_LEVEL, "video_mctf_level", 1, 0, VideoMctfLevelParams_Count, PARAM_STATUS_DEFAULT, VideoMctfLevelParams, VideoMctfLevelProc, 0 },
    { ITUNER_VIDEO_MCTF_TEMPORAL_ADJUST, "video_mctf_temporal_adjust", 1, 0, VideoMctfTemporalAdjustParams_Count, PARAM_STATUS_DEFAULT, VideoMctfTemporalAdjustParams, VideoMctfTemporalAdjustProc, 0 },
    { ITUNER_VIDEO_MCTF_AND_FINAL_SHARPEN, "video_mctf_and_final_sharpen", 1, 0, VideoMctfAndFinalSharpenParams_Count, PARAM_STATUS_DEFAULT, VideoMctfAndFinalSharpenParams, VideoMctfAndFinalSharpenProc, 0 },
    { ITUNER_SENSOR_INPUT_INFO, "vin_sensor_info", 1, 0, SensorInputParams_Count, PARAM_STATUS_DEFAULT, SensorInputParams, SensorInputProc, 0 },
    { ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT, "li_processing_select", 1, 0, ShpAOrSpatialFilterSelectParams_Count, PARAM_STATUS_DEFAULT, ShpAOrSpatialFilterSelectParams, ShpAOrSpatialFilterSelectproc, 0 },
    { ITUNER_ASF_INFO, "li_advanced_spatial_filter", 1, 0, AdvanceSpatialFilterParams_Count, PARAM_STATUS_DEFAULT, AdvanceSpatialFilterParams, AdvanceSpatialFilterProc, 0 },
    { ITUNER_LUMA_NOISE_REDUCTION, "li_color_dependent_luma_noise_reduction", 1, 0, LumaNoiseReductionParams_Count, PARAM_STATUS_DEFAULT, LumaNoiseReductionParams, LumaNoiseReductionProc, 0 },
    { ITUNER_HI_LUMA_NOISE_REDUCTION, "hi_color_dependent_luma_noise_reduction", 1, 0, HiLumaNoiseReductionParams_Count, PARAM_STATUS_DEFAULT, HiLumaNoiseReductionParams, HiLumaNoiseReductionProc, 0 },
    { ITUNER_LI2_LUMA_NOISE_REDUCTION, "li2_color_dependent_luma_noise_reduction", 1, 0, Li2LumaNoiseReductionParams_Count, PARAM_STATUS_DEFAULT, Li2LumaNoiseReductionParams, Li2LumaNoiseReductionProc, 0 },
    { ITUNER_CHROMA_FILTER, "li_chroma_filter", 1, 0, ChromaFilterParams_Count, PARAM_STATUS_DEFAULT, ChromaFilterParams, ChromaFilterProc, 0 },
    { ITUNER_SHARPEN_BOTH, "li_sharpen_noise_filter_both", 1, 0, SharpeningBothParams_Count, PARAM_STATUS_DEFAULT, SharpeningBothParams, SharpenBothProc, 0 },
    { ITUNER_SHARPEN_NOISE, "li_sharpen_noise_filter_noise", 1, 0, SharpeningNoiseParams_Count, PARAM_STATUS_DEFAULT, SharpeningNoiseParams, SharpenNoiseProc, 0 },
    { ITUNER_FIR, "li_sharpen_noise_filter_sharpen", 1, 0, FirstSharpeningFirParams_Count, PARAM_STATUS_DEFAULT, FirstSharpeningFirParams, SharpenFirProc, 0 },
    { ITUNER_CORING, "li_sharpen_noise_filter_sharpen_coring", 1, 0, SharpeningCoringParams_Count, PARAM_STATUS_DEFAULT, SharpeningCoringParams, SharpenCoringProc, 0 },
    { ITUNER_CORING_INDEX_SCALE, "li_sharpen_noise_filter_sharpen_coring_index_scale", 1, 0, SharpeningLevelParams_Count, PARAM_STATUS_DEFAULT, SharpeningLevelParams, CoringIndexScaleProc, 0 },
    { ITUNER_MIN_CORING_RESULT, "li_sharpen_noise_filter_sharpen_min_coring_result", 1, 0, SharpeningLevelParams_Count, PARAM_STATUS_DEFAULT, SharpeningLevelParams, MinCoringResultProc, 0 },
    { ITUNER_MAX_CORING_RESULT, "li_sharpen_noise_filter_sharpen_max_coring_result", 1, 0, SharpeningLevelParams_Count, PARAM_STATUS_DEFAULT, SharpeningLevelParams, MaxCoringResultProc, 0 },
    { ITUNER_SCALE_CORING, "li_sharpen_noise_filter_sharpen_scale_coring", 1, 0, SharpeningLevelParams_Count, PARAM_STATUS_DEFAULT, SharpeningLevelParams, ScaleCoringProc, 0 },
    { ITUNER_FINAL_SHARPEN_BOTH, "final_sharpen_noise_filter_both", 1, 0, FinalSharpeningBothParams_Count, PARAM_STATUS_DEFAULT, FinalSharpeningBothParams, FinalSharpenBothProc, 0 },
    { ITUNER_FINAL_SHARPEN_NOISE, "final_sharpen_noise_filter_noise", 1, 0, FinalSharpeningNoiseParams_Count, PARAM_STATUS_DEFAULT, FinalSharpeningNoiseParams, FinalSharpenNoiseProc, 0 },
    { ITUNER_FINAL_FIR, "final_sharpen_noise_filter_sharpen", 1, 0, FinalSharpeningFirParams_Count, PARAM_STATUS_DEFAULT, FinalSharpeningFirParams, FinalSharpenFirProc, 0 },
    { ITUNER_FINAL_CORING, "final_sharpen_noise_filter_sharpen_coring", 1, 0, FianlSharpeningCoringParams_Count, PARAM_STATUS_DEFAULT, FianlSharpeningCoringParams, FinalSharpenCoringProc, 0 },
    { ITUNER_FINAL_CORING_INDEX_SCALE, "final_sharpen_noise_filter_sharpen_coring_index_scale", 1, 0, FinalSharpeningLevelParams_Count, PARAM_STATUS_DEFAULT, FinalSharpeningLevelParams, FinalCoringIndexScaleProc, 0 },
    { ITUNER_FINAL_MIN_CORING_RESULT, "final_sharpen_noise_filter_sharpen_min_coring_result", 1, 0, FinalSharpeningLevelParams_Count, PARAM_STATUS_DEFAULT, FinalSharpeningLevelParams, FinalMinCoringResultProc, 0 },
    { ITUNER_FINAL_MAX_CORING_RESULT, "final_sharpen_noise_filter_sharpen_max_coring_result", 1, 0, FinalSharpeningLevelParams_Count, PARAM_STATUS_DEFAULT, FinalSharpeningLevelParams, FinalMaxCoringResultProc, 0 },
    { ITUNER_FINAL_SCALE_CORING, "final_sharpen_noise_filter_sharpen_scale_coring", 1, 0, FinalSharpeningLevelParams_Count, PARAM_STATUS_DEFAULT, FinalSharpeningLevelParams, FinalScaleCoringProc, 0 },
    { ITUNER_CONTRAST_ENHANCE, "contrast_enhance", 1, 0, ContrastEnhance_Count, PARAM_STATUS_DEFAULT, ContrastEnhance, ContrastEnhance_Proc, 0 },
    { ITUNER_VHDR_BLEND, "hdr_blend", 1, 0, HdrBlendParams_Count, PARAM_STATUS_DEFAULT, HdrBlendParams, HdrBlend_Proc, 0},
    { ITUNER_FRONT_END_WB_GAIN_EXP0, "front_end_exp0", 1, 0, HdrFrontEndWbgain_Count, PARAM_STATUS_DEFAULT, HdrFrontEndWbgain, HdrFrontEndWbgainExp0_Proc, 0},
    { ITUNER_FRONT_END_WB_GAIN_EXP1, "front_end_exp1", 1, 0, HdrFrontEndWbgain_Count, PARAM_STATUS_DEFAULT, HdrFrontEndWbgain, HdrFrontEndWbgainExp1_Proc, 0},
    { ITUNER_FRONT_END_WB_GAIN_EXP2, "front_end_exp2", 1, 0, HdrFrontEndWbgain_Count, PARAM_STATUS_DEFAULT, HdrFrontEndWbgain, HdrFrontEndWbgainExp2_Proc, 0},
    { ITUNER_VHDR_RAW_INFO, "raw_info", 1, 0, HdrRawInfo_Count, PARAM_STATUS_DEFAULT, HdrRawInfo, HdrRawInfo_Proc, 0},
    { ITUNER_STATIC_BLACK_LEVEL_EXP0, "static_black_level_exp0", 1, 0, StaticBlackLevelParams_Count, PARAM_STATUS_DEFAULT, StaticBlackLevelParams, HdrStaticBlackLevelExp0_Proc, 0},
    { ITUNER_STATIC_BLACK_LEVEL_EXP1, "static_black_level_exp1", 1, 0, StaticBlackLevelParams_Count, PARAM_STATUS_DEFAULT, StaticBlackLevelParams, HdrStaticBlackLevelExp1_Proc, 0},
    { ITUNER_STATIC_BLACK_LEVEL_EXP2, "static_black_level_exp2", 1, 0, StaticBlackLevelParams_Count, PARAM_STATUS_DEFAULT, StaticBlackLevelParams, HdrStaticBlackLevelExp2_Proc, 0},
    { ITUNER_WIDE_CHROMA_FILTER, "li_wide_chroma_filter", 1, 0, WideChromaFilterParams_Count, PARAM_STATUS_DEFAULT, WideChromaFilterParams, WideChromaFilterProc, 0 },
    { ITUNER_WIDE_CHROMA_FILTER_COMBINE, "li_wide_chroma_filter_combine", 1, 0, WideChromaFilterCombineParams_Count, PARAM_STATUS_DEFAULT, WideChromaFilterCombineParams, WideChromaFilterCombineProc, 0 },
    { ITUNER_DUMMY, "dummy", 1, 0, DummyParams_Count, PARAM_STATUS_DEFAULT, DummyParams, DummyProc, 0 },
    { ITUNER_ACTIVE, "active", 1, 0, ActiveParams_Count, PARAM_STATUS_DEFAULT, ActiveParams, ActiveProc, 0 },
    // hiso
    { ITUNER_LI_CHROMA_ASF, "li_chroma_advanced_spatial_filter", 1, 0, GetArraySize(HiChromaAdvanceSpatialFilterParams), PARAM_STATUS_DEFAULT, HiChromaAdvanceSpatialFilterParams, LiChromaASFProc, 0 },
    { ITUNER_HI_CHROMA_ASF, "hi_chroma_advanced_spatial_filter", 1, 0, GetArraySize(HiChromaAdvanceSpatialFilterParams), PARAM_STATUS_DEFAULT, HiChromaAdvanceSpatialFilterParams, HiChromaASFProc, 0 },
    { ITUNER_HI_LUMA_ASF, "hi_advanced_spatial_filter", 1, 0, GetArraySize(HiLumaAdvanceSpatialFilterParams), PARAM_STATUS_DEFAULT, HiLumaAdvanceSpatialFilterParams, HiLumaASFProc, 0 },
    { ITUNER_HI_CFA_NOISE_FILTER, "hi_cfa_noise_filter", 1, 0, GetArraySize(HiCfaNoiseFilterParams), PARAM_STATUS_DEFAULT, HiCfaNoiseFilterParams, HiCfaNoiseFilterProc, 0 },
    { ITUNER_HI_CFA_LEAKAGE_FILTER, "hi_cfa_leakage_filter", 1, 0, GetArraySize(HiCfaLeakageFilterParams), PARAM_STATUS_DEFAULT, HiCfaLeakageFilterParams, HiCfaLeakageFilterProc, 0 },
    { ITUNER_HI_ANTI_ALIASING, "hi_anti_aliasing", 1, 0, GetArraySize(HiAntiAliasingParams), PARAM_STATUS_DEFAULT, HiAntiAliasingParams, HiAntiAliasingProc, 0 },
    { ITUNER_HI_AUTO_BAD_PIXEL_CORRECTION, "hi_auto_bad_pixel_correction", 1, 0, GetArraySize(HiAutoBadPixelCorrectionParams), PARAM_STATUS_DEFAULT, HiAutoBadPixelCorrectionParams, HiAutoBadPixelCorrectionProc, 0 },
    { ITUNER_HI_CHROMA_FLTR_HIGH, "hi_chroma_filter_high", 1, 0, GetArraySize(HiChromaFilterHighParams), PARAM_STATUS_DEFAULT, HiChromaFilterHighParams, HiChromaFilterHighProc, 0 },
    { ITUNER_HI_CHROMA_FLTR_LOW, "hi_chroma_filter_low", 1, 0, GetArraySize(HiChromaFilterLowParams), PARAM_STATUS_DEFAULT, HiChromaFilterLowParams, HiChromaFilterLowProc, 0 },
    { ITUNER_HI_CHROMA_FLTR_LOW_COMBINE, "hi_chroma_filter_low_combine", 1, 0, GetArraySize(HiChromaFilterLowCombineParams), PARAM_STATUS_DEFAULT, HiChromaFilterLowCombineParams, HiChromaFilterLowCombineProc, 0 },
    { ITUNER_HI_CHROMA_FLTR_MEDIAN, "hi_chroma_filter_med", 1, 0, GetArraySize(HiChromaFilterMedianParams), PARAM_STATUS_DEFAULT, HiChromaFilterMedianParams, HiChromaFilterMedianProc, 0 },
    { ITUNER_HI_CHROMA_FLTR_MEDIAN_COMBINE, "hi_chroma_filter_med_combine", 1, 0, GetArraySize(HiChromaFilterMedianCombineParams), PARAM_STATUS_DEFAULT, HiChromaFilterMedianCombineParams, HiChromaFilterMedianCombineProc, 0 },
    { ITUNER_HI_CHROMA_FLTR_PRE, "hi_chroma_filter_pre", 1, 0, GetArraySize(HiChromaFilterPreParams), PARAM_STATUS_DEFAULT, HiChromaFilterPreParams, HiChromaFilterPreProc, 0 },
    { ITUNER_HI_CHROMA_FLTR_VERY_LOW, "hi_chroma_filter_very_low", 1, 0, GetArraySize(HiChromaFilterVeryLowParams), PARAM_STATUS_DEFAULT, HiChromaFilterVeryLowParams, HiChromaFilterVeryLowProc, 0 },
    { ITUNER_HI_CHROMA_FLTR_VERY_LOW_COMBINE, "hi_chroma_filter_very_low_combine", 1, 0, GetArraySize(HiChromaFilterVeryLowCombineParams), PARAM_STATUS_DEFAULT, HiChromaFilterVeryLowCombineParams, HiChromaFilterVeryLowCombineProc, 0 },
    { ITUNER_HI_CHROMA_MEDIAN_FILTER, "hi_chroma_median_filter", 1, 0, GetArraySize(HiChromaMedianFilterParams), PARAM_STATUS_DEFAULT, HiChromaMedianFilterParams, HiChromaMedianFilterProc, 0 },
    { ITUNRE_HI_DEMOSAIC_FILTER, "hi_demosaic", 1, 0, GetArraySize(HiDemosaicFilterParams), PARAM_STATUS_DEFAULT, HiDemosaicFilterParams, HiDemosaicFilterProc, 0 },
    { ITUNER_HI_GB_GR_MISMATCH, "hi_gb_gr_mismatch_correct", 1, 0, GetArraySize(HiGbGrMismatchParams), PARAM_STATUS_DEFAULT, HiGbGrMismatchParams, HiGbGrMismatchProc, 0 },
    { ITUNER_HI_HIGH2_LUMA_ASF, "hi_high2_advanced_spatial_filter", 1, 0, GetArraySize(HiHigh2LumaAdvanceSpatialFilterParams), PARAM_STATUS_DEFAULT, HiHigh2LumaAdvanceSpatialFilterParams, HiHigh2LumaASFProc, 0 },
    { ITUNER_HI_HIGH_LUMA_ASF, "hi_high_advanced_spatial_filter", 1, 0, GetArraySize(HiHighLumaAdvanceSpatialFilterParams), PARAM_STATUS_DEFAULT, HiHighLumaAdvanceSpatialFilterParams, HiHighLumaASFProc, 0 },
    { ITUNER_HI_HIGH_SHARPEN_BOTH, "hi_high_sharpen_noise_filter_both", 1, 0, GetArraySize(HiHighSharpeningBothParams), PARAM_STATUS_DEFAULT, HiHighSharpeningBothParams, HiHighSharpenBothProc, 0 },
    { ITUNER_HI_HIGH_SHARPEN_NOISE, "hi_high_sharpen_noise_filter_noise", 1, 0, GetArraySize(HiHighSharpeningNoiseParams), PARAM_STATUS_DEFAULT, HiHighSharpeningNoiseParams, HiHighSharpenNoiseProc, 0 },
    { ITUNER_HI_HIGH_FIR, "hi_high_sharpen_noise_filter_sharpen", 1, 0, GetArraySize(HiHighFirstSharpeningFirParams), PARAM_STATUS_DEFAULT, HiHighFirstSharpeningFirParams, HiHighSharpenFirProc, 0 },
    { ITUNER_HI_HIGH_CORING, "hi_high_sharpen_noise_filter_sharpen_coring", 1, 0, GetArraySize(HiHighSharpeningCoringParams), PARAM_STATUS_DEFAULT, HiHighSharpeningCoringParams, HiHighSharpenCoringProc, 0 },
    { ITUNER_HI_HIGH_CORING_INDEX_SCALE, "hi_high_sharpen_noise_filter_sharpen_coring_index_scale", 1, 0, GetArraySize(HiHighSharpeningLevelParams), PARAM_STATUS_DEFAULT, HiHighSharpeningLevelParams, HiHighCoringIndexScaleProc, 0 },
    { ITUNER_HI_HIGH_MAX_CORING_RESULT, "hi_high_sharpen_noise_filter_sharpen_max_coring_result", 1, 0, GetArraySize(HiHighSharpeningLevelParams), PARAM_STATUS_DEFAULT, HiHighSharpeningLevelParams, HiHighMaxCoringResultProc, 0 },
    { ITUNER_HI_HIGH_MIN_CORING_RESULT, "hi_high_sharpen_noise_filter_sharpen_min_coring_result", 1, 0, GetArraySize(HiHighSharpeningLevelParams), PARAM_STATUS_DEFAULT, HiHighSharpeningLevelParams, HiHighMinCoringResultProc, 0 },
    { ITUNER_HI_HIGH_SCALE_CORING, "hi_high_sharpen_noise_filter_sharpen_scale_coring", 1, 0, GetArraySize(HiHighSharpeningLevelParams), PARAM_STATUS_DEFAULT, HiHighSharpeningLevelParams, HiHighScaleCoringProc, 0 },
    { ITUNER_HI_LOW_LUMA_ASF, "hi_low_advanced_spatial_filter", 1, 0, GetArraySize(HiLowLumaAdvanceSpatialFilterParams), PARAM_STATUS_DEFAULT, HiLowLumaAdvanceSpatialFilterParams, HiLowLumaASFProc, 0 },
    { ITUNER_HI_LOW_LUMA_ASF_COMBINE, "hi_low_advanced_spatial_filter_combine", 1, 0, GetArraySize(HiLowLumaAdvanceSpatialFilterCombineParams), PARAM_STATUS_DEFAULT, HiLowLumaAdvanceSpatialFilterCombineParams, HiLowLumaASFCombineProc, 0 },
    { ITUNER_HI_LUMA_NOISE_COMBINE, "hi_luma_noise_combine", 1, 0, GetArraySize(HiLumaNoiseCombineParams), PARAM_STATUS_DEFAULT, HiLumaNoiseCombineParams, HiLumaNoiseCombineProc, 0 },
    { ITUNER_HI_MED1_LUMA_ASF, "hi_med1_advanced_spatial_filter", 1, 0, GetArraySize(HiMedian1LumaAdvanceSpatialFilterParams), PARAM_STATUS_DEFAULT, HiMedian1LumaAdvanceSpatialFilterParams, HiMedian1LumaASFProc, 0 },
    { ITUNER_HI_MED2_LUMA_ASF, "hi_med2_advanced_spatial_filter", 1, 0, GetArraySize(HiMedian2LumaAdvanceSpatialFilterParams), PARAM_STATUS_DEFAULT, HiMedian2LumaAdvanceSpatialFilterParams, HiMedian2LumaASFProc, 0 },
    { ITUNER_HI_MED_SHARPEN_BOTH, "hi_med_sharpen_noise_filter_both", 1, 0, GetArraySize(HiMedianSharpeningBothParams), PARAM_STATUS_DEFAULT, HiMedianSharpeningBothParams, HiMedianSharpenBothProc, 0 },
    { ITUNER_HI_MED_SHARPEN_NOISE, "hi_med_sharpen_noise_filter_noise", 1, 0, GetArraySize(HiMedianSharpeningNoiseParams), PARAM_STATUS_DEFAULT, HiMedianSharpeningNoiseParams, HiMedianSharpenNoiseProc, 0 },
    { ITUNER_HI_MED_FIR, "hi_med_sharpen_noise_filter_sharpen", 1, 0, GetArraySize(HiMedianFirstSharpeningFirParams), PARAM_STATUS_DEFAULT, HiMedianFirstSharpeningFirParams, HiMedianSharpenFirProc, 0 },
    { ITUNER_HI_MED_CORING, "hi_med_sharpen_noise_filter_sharpen_coring", 1, 0, GetArraySize(HiMedianSharpeningCoringParams), PARAM_STATUS_DEFAULT, HiMedianSharpeningCoringParams, HiMedianSharpenCoringProc, 0 },
    { ITUNER_HI_MED_CORING_INDEX_SCALE, "hi_med_sharpen_noise_filter_sharpen_coring_index_scale", 1, 0, GetArraySize(HiMedianSharpeningLevelParams), PARAM_STATUS_DEFAULT, HiMedianSharpeningLevelParams, HiMedianCoringIndexScaleProc, 0 },
    { ITUNER_HI_MED_MAX_CORING_RESULT, "hi_med_sharpen_noise_filter_sharpen_max_coring_result", 1, 0, GetArraySize(HiMedianSharpeningLevelParams), PARAM_STATUS_DEFAULT, HiMedianSharpeningLevelParams, HiMedianMaxCoringResultProc, 0 },
    { ITUNER_HI_MED_MIN_CORING_RESULT, "hi_med_sharpen_noise_filter_sharpen_min_coring_result", 1, 0, GetArraySize(HiMedianSharpeningLevelParams), PARAM_STATUS_DEFAULT, HiMedianSharpeningLevelParams, HiMedianMinCoringResultProc, 0 },
    { ITUNER_HI_MED_SCALE_CORING, "hi_med_sharpen_noise_filter_sharpen_scale_coring", 1, 0, GetArraySize(HiMedianSharpeningLevelParams), PARAM_STATUS_DEFAULT, HiMedianSharpeningLevelParams, HiMedianScaleCoringProc, 0 },
    { ITUNER_HI_SELECT, "hi_select", 1, 0, GetArraySize(HiSelectParams), PARAM_STATUS_DEFAULT, HiSelectParams, HiSelectProc, 0 },
    { ITUNER_HI_LUMA_BLEND, "hili2_blend_luma", 1, 0, GetArraySize(HiLumaBlendParams), PARAM_STATUS_DEFAULT, HiLumaBlendParams, HiLumaBlendProc, 0 },
    { ITUNER_HILI_COMBINE, "hili_combine", 1, 0, GetArraySize(HiLiCombineParams), PARAM_STATUS_DEFAULT, HiLiCombineParams, HiLiCombineProc, 0 },
    { ITUNER_HILI_SHARPEN_BOTH, "hili_sharpen_noise_filter_both", 1, 0, GetArraySize(HiLiSharpeningBothParams), PARAM_STATUS_DEFAULT, HiLiSharpeningBothParams, HiLiSharpenBothProc, 0 },
    { ITUNER_HILi_SHARPEN_NOISE, "hili_sharpen_noise_filter_noise", 1, 0, GetArraySize(HiLiSharpeningNoiseParams), PARAM_STATUS_DEFAULT, HiLiSharpeningNoiseParams, HiLiSharpenNoiseProc, 0 },
    { ITUNER_HILI_FIR, "hili_sharpen_noise_filter_sharpen", 1, 0, GetArraySize(HiLiFirstSharpeningFirParams), PARAM_STATUS_DEFAULT, HiLiFirstSharpeningFirParams, HiLiSharpenFirProc, 0 },
    { ITUNER_HILI_CORING, "hili_sharpen_noise_filter_sharpen_coring", 1, 0, GetArraySize(HiLiSharpeningCoringParams), PARAM_STATUS_DEFAULT, HiLiSharpeningCoringParams, HiLiSharpenCoringProc, 0 },
    { ITUNER_HILI_CORING_INDEX_SCALE, "hili_sharpen_noise_filter_sharpen_coring_index_scale", 1, 0, GetArraySize(HiLiSharpeningLevelParams), PARAM_STATUS_DEFAULT, HiLiSharpeningLevelParams, HiLiCoringIndexScaleProc, 0 },
    { ITUNER_HILI_MAX_CORING_RESULT, "hili_sharpen_noise_filter_sharpen_max_coring_result", 1, 0, GetArraySize(HiLiSharpeningLevelParams), PARAM_STATUS_DEFAULT, HiLiSharpeningLevelParams, HiLiMaxCoringResultProc, 0 },
    { ITUNER_HILI_MIN_CORING_RESULT, "hili_sharpen_noise_filter_sharpen_min_coring_result", 1, 0, GetArraySize(HiLiSharpeningLevelParams), PARAM_STATUS_DEFAULT, HiLiSharpeningLevelParams, HiLiMinCoringResultProc, 0 },
    { ITUNER_HILI_SCALE_CORING, "hili_sharpen_noise_filter_sharpen_scale_coring", 1, 0, GetArraySize(HiLiSharpeningLevelParams), PARAM_STATUS_DEFAULT, HiLiSharpeningLevelParams, HiLiScaleCoringProc, 0 },
    { ITUNER_LI2_LUMA_ASF, "li2_advanced_spatial_filter", 1, 0, GetArraySize(Li2LumaAdvanceSpatialFilterParams), PARAM_STATUS_DEFAULT, Li2LumaAdvanceSpatialFilterParams, Li2LumaASFProc, 0 },
    { ITUNER_LI2_ANTI_ALIASING, "li2_anti_aliasing", 1, 0, GetArraySize(Li2AntiAliasingParams), PARAM_STATUS_DEFAULT, Li2AntiAliasingParams, Li2AntiAliasingProc, 0 },
    { ITUNER_LI2_AUTO_BAD_PIXEL_CORRECTION, "li2_auto_bad_pixel_correction", 1, 0, GetArraySize(Li2AutoBadPixelCorrectionParams), PARAM_STATUS_DEFAULT, Li2AutoBadPixelCorrectionParams, Li2AutoBadPixelCorrectionProc, 0 },
    { ITUNER_LI2_CFA_LEAKAGE_FILTER, "li2_cfa_leakage_filter", 1, 0, GetArraySize(Li2CfaLeakageFilterParams), PARAM_STATUS_DEFAULT, Li2CfaLeakageFilterParams, Li2CfaLeakageFilterProc, 0 },
    { ITUNER_LI2_CFA_NOISE_FILTER, "li2_cfa_noise_filter", 1, 0, GetArraySize(Li2CfaNoiseFilterParams), PARAM_STATUS_DEFAULT, Li2CfaNoiseFilterParams, Li2CfaNoiseFilterProc, 0 },
    { ITUNRE_LI2_DEMOSAIC_FILTER, "li2_demosaic", 1, 0, GetArraySize(Li2DemosaicFilterParams), PARAM_STATUS_DEFAULT, Li2DemosaicFilterParams, Li2DemosaicFilterProc, 0 },
    { ITUNER_LI2_GB_GR_MISMATCH, "li2_gb_gr_mismatch_correct", 1, 0, GetArraySize(Li2GbGrMismatchParams), PARAM_STATUS_DEFAULT, Li2GbGrMismatchParams, Li2GbGrMismatchProc, 0 },
    { ITUNER_LI2_SHARPEN_BOTH, "li2_sharpen_noise_filter_both", 1, 0, GetArraySize(Li2SharpeningBothParams), PARAM_STATUS_DEFAULT, Li2SharpeningBothParams, Li2SharpenBothProc, 0 },
    { ITUNER_LI2_SHARPEN_NOISE, "li2_sharpen_noise_filter_noise", 1, 0, GetArraySize(Li2SharpeningNoiseParams), PARAM_STATUS_DEFAULT, Li2SharpeningNoiseParams, Li2SharpenNoiseProc, 0 },
    { ITUNER_LI2_FIR, "li2_sharpen_noise_filter_sharpen", 1, 0, GetArraySize(Li2FirstSharpeningFirParams), PARAM_STATUS_DEFAULT, Li2FirstSharpeningFirParams, Li2SharpenFirProc, 0 },
    { ITUNER_LI2_CORING, "li2_sharpen_noise_filter_sharpen_coring", 1, 0, GetArraySize(Li2SharpeningCoringParams), PARAM_STATUS_DEFAULT, Li2SharpeningCoringParams, Li2SharpenCoringProc, 0 },
    { ITUNER_LI2_CORING_INDEX_SCALE, "li2_sharpen_noise_filter_sharpen_coring_index_scale", 1, 0, GetArraySize(Li2SharpeningLevelParams), PARAM_STATUS_DEFAULT, Li2SharpeningLevelParams, Li2CoringIndexScaleProc, 0 },
    { ITUNER_LI2_MAX_CORING_RESULT, "li2_sharpen_noise_filter_sharpen_max_coring_result", 1, 0, GetArraySize(Li2SharpeningLevelParams), PARAM_STATUS_DEFAULT, Li2SharpeningLevelParams, Li2MaxCoringResultProc, 0 },
    { ITUNER_LI2_MIN_CORING_RESULT, "li2_sharpen_noise_filter_sharpen_min_coring_result", 1, 0, GetArraySize(Li2SharpeningLevelParams), PARAM_STATUS_DEFAULT, Li2SharpeningLevelParams, Li2MinCoringResultProc, 0 },
    { ITUNER_LI2_SCALE_CORING, "li2_sharpen_noise_filter_sharpen_scale_coring", 1, 0, GetArraySize(Li2SharpeningLevelParams), PARAM_STATUS_DEFAULT, Li2SharpeningLevelParams, Li2ScaleCoringProc, 0 },
    { ITUNER_HI_NONSMOOTH_DETECT, "nonsmooth_detect", 1, 0, GetArraySize(HiNonsmoothDetectParams), PARAM_STATUS_DEFAULT, HiNonsmoothDetectParams, HiNonsmoothDetectProc, 0 },
    { ITUNER_LI2_WIDE_CHROMA_FILTER, "li2_wide_chroma_filter", 1, 0, GetArraySize(WideChromaFilterParams), PARAM_STATUS_DEFAULT, WideChromaFilterParams, Li2WideChromaFilterProc, 0 },
    { ITUNER_LI2_WIDE_CHROMA_FILTER_COMBINE, "li2_wide_chroma_filter_combine", 1, 0, GetArraySize(WideChromaFilterCombineParams), PARAM_STATUS_DEFAULT, WideChromaFilterCombineParams, Li2WideChromaFilterCombineProc, 0 },
};

INT32 TUNE_Rule_Get_Info(Rule_Info_t *RuleInfo)
{

    RuleInfo->RegList = &ImgRegs[0];
    RuleInfo->RegCount = ImgRegs_Count;
    return 0;
}

const char *TUNE_Rule_LU_Tuning_Mode_Str(TUNING_MODE_e Tuning_Mode)
{
    return str_tuning_mode[Tuning_Mode];
}

const char *TUNE_Rule_LU_Tuning_Mode_Ext_Str(AMBA_ITN_TUNING_MODE_EXT_e Tuning_Mode_Ext)
{
    return str_tuning_mode_ext[Tuning_Mode_Ext];
}

TUNING_MODE_e TUNE_Rule_LU_Tuning_Mode(const char *Key)
{
    INT32 Mode = 0, MisraI32;
    TUNING_MODE_e Rval = IMG_MODE_VIDEO, MisraMode;
    if (Key==NULL) {
        // FIXME
    }
    MisraMode = IMG_MODE_NUMBER;
    AmbaMisra_TypeCast32(&MisraI32, &MisraMode);
    for (Mode = 0; Mode < MisraI32; Mode++) {
        if (ituner_strncmp(Key, str_tuning_mode[Mode], (INT32)ituner_strlen(str_tuning_mode[Mode])) == 0) {
            break;
        }
    }
    MisraMode = IMG_MODE_NUMBER;
    AmbaMisra_TypeCast32(&MisraI32, &MisraMode);
    if (Mode == MisraI32) {
        ituner_print_str_5("%s() Unknown Tuning Mode: %s", __func__, Key, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d %d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        Mode = 0;
    }
    AmbaMisra_TypeCast32(&Rval, &Mode);
    return Rval;
}

AMBA_ITN_TUNING_MODE_EXT_e TUNE_Rule_LU_Tuning_Mode_Ext(const char *Key)
{
    INT32 Mode = 0, MisraI32;
    AMBA_ITN_TUNING_MODE_EXT_e Rval = SINGLE_SHOT, MisraModeExt;
    if (Key==NULL) {
        // FIXME
    }
    MisraModeExt = TUNING_MODE_EXT_NUMBER;
    AmbaMisra_TypeCast32(&MisraI32, &MisraModeExt);
    for (Mode = 0; Mode < MisraI32; Mode++) {
        if (ituner_strcmp(Key, str_tuning_mode_ext[Mode]) == 0) {
            break;
        }
    }
    MisraModeExt = TUNING_MODE_EXT_NUMBER;
    AmbaMisra_TypeCast32(&MisraI32, &MisraModeExt);
    if (Mode == MisraI32) {
        ituner_print_str_5("%s() Unknown Tuning Mode EXT : %s", __func__, Key, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d %d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        Mode = 0;
    }
    AmbaMisra_TypeCast32(&Rval, &Mode);
    return Rval;
}

void ituner_param_proc(const PARAM_s *pParam, const void *pField)
{
    OpMode_e OpMode = ituner_Opmode_Get();
    if (OpMode == ITUNER_DEC) {
        TUNE_Parser_Dec_Proc(pParam, pField);
    } else {
        TUNE_Parser_Enc_Proc(pParam, pField);
    }
}

void ituner_Opmode_Set(OpMode_e OpMode)
{
    Opmode = OpMode ;
}

OpMode_e ituner_Opmode_Get(void)
{
    return Opmode ;
}

static UINT32 ituner_CheckIndex(INT32 Index, INT32 Max)
{
    INT32 Idx;
    AmbaMisra_TouchUnused(ImgRegs);
    if (Index > Max) {
        ituner_print_str_5("%s() fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line: %d Index = %d, Max = %d", __LINE__, (UINT32)Index, (UINT32)Max, DC_U, DC_U);
        Idx = Max;
    } else {
        Idx = Index;
    }
    return (UINT32)Idx;
}

static void SdkInfo_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_ITN_SDK_INFO_s Data;
    void *pField[SdkInfo_Params_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_SdkInfo(&Data);
    pField[0] = &Data.BurstTileEnable;
    pField[1] = &Data.StitchEnable;
    pField[2] = &Data.StitchTileNumX;
    pField[3] = &Data.StitchTileNumY;
    pField[4] = &Data.DramEfficiency;
    pField[5] = &Data.LumaDmaSize;
    pField[6] = &Data.LumaWaitLines;
    pField[7] = &Data.OverlapX;
    Max = SdkInfo_Params_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_SdkInfo(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus = SKIP_VALID_CHECK;
    }
}

static void System_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_ITN_SYSTEM_s Data;
    void *pField[System_Params_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_SystemInfo(&Data);
    pField[0] = &Data.ItunerRev;
    pField[1] = &Data.ChipRev;
    pField[2] = &Data.SensorId;
    pField[3] = &Data.UserMode;
    pField[4] = &Data.TuningMode;
    pField[5] = &Data.RawPath;
    pField[6] = &Data.RawWidth;
    pField[7] = &Data.RawHeight;
    pField[8] = &Data.RawPitch;
    pField[9] = &Data.RawResolution;
    pField[10] = &Data.RawBayer;
    pField[11] = &Data.MainWidth;
    pField[12] = &Data.MainHeight;
    pField[13] = &Data.TuningModeExt;
    pField[14] = &Data.ExtConfigPath;
    pField[15] = &Data.InputPicCnt;
    pField[16] = &Data.CompressedRaw;
    pField[17] = &Data.SensorReadoutMode;
    pField[18] = &Data.RawStartX;
    pField[19] = &Data.RawStartY;
    pField[20] = &Data.HSubSampleFactorNum;
    pField[21] = &Data.HSubSampleFactorDen;
    pField[22] = &Data.VSubSampleFactorNum;
    pField[23] = &Data.VSubSampleFactorDen;
    pField[24] = &Data.OutputFilename;
    pField[25] = &Data.EnableRaw2Raw;
    pField[26] = &Data.JpegQuality;
    pField[27] = &Data.NumberOfExposures;
    pField[28] = &Data.SensorMode;
    pField[29] = &Data.CompressionOffset;
    pField[30] = &Data.CfaWidth;
    pField[31] = &Data.CfaHeight;
    pField[32] = &Data.Ability;
    pField[33] = &Data.YuvMode;
    pField[34] = &Data.StoreIr;
    pField[35] = &Data.FlipH;
    pField[36] = &Data.FlipV;
    pField[37] = &Data.FrameNumber;
    pField[38] = &Data.NumberOfFrames;
    Max = System_Params_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_SystemInfo(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus = SKIP_VALID_CHECK;
    }
}


static void Internal_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    Ituner_Internal_s Data;
    void *pField[Internal_Params_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_InternalInfo(&Data);
    pField[0] = &Data.sbp_highlight;
    pField[1] = &Data.diag_mode;
    pField[2] = &Data.video_mctf_y_10;
    pField[3] = &Data.video_mctf_fast_start;
    Max = Internal_Params_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_InternalInfo(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus = SKIP_VALID_CHECK;
    }
}

static void AaaFunctionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AAA_FUNC_s Data;
    void *pField[AaaFunctionParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_AAAFunction(&Data);
    pField[0] = &Data.AeOp;
    pField[1] = &Data.AwbOp;
    pField[2] = &Data.AfOp;
    pField[3] = &Data.AdjOp;
    Max = AaaFunctionParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_AAAFunction(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}


static void AeInfoProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_AE_INFO_s Data;
    void *pField[AeInfoParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_AeInfo(&Data);
    pField[0] = &Data.EvIndex;
    pField[1] = &Data.NfIndex;
    pField[2] = &Data.ShutterIndex;
    pField[3] = &Data.AgcIndex;
    pField[4] = &Data.IrisIndex;
    pField[5] = &Data.Dgain;
    pField[6] = &Data.IsoValue;
    pField[7] = &Data.Flash;
    pField[8] = &Data.Mode;
    pField[9] = &Data.ShutterTime;
    pField[10] = &Data.AgcGain;
    pField[11] = &Data.Target;
    pField[12] = &Data.LumaStat;
    pField[13] = &Data.LimitStatus;
    pField[14] = &Data.Multiplier;
    Max = AeInfoParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_AeInfo(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus = SKIP_VALID_CHECK;
    }
}

static void WbSimInfoProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_WB_SIM_INFO_s Data;
    void *pField[WbSimInfoParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_WbSimInfo(&Data);
    pField[0] = &Data.LumaIdx;
    pField[1] = &Data.OutDoorIdx;
    pField[2] = &Data.HighLightIdx;
    pField[3] = &Data.LowLightIdx;
    pField[4] = Data.AwbRatio;
    Max = WbSimInfoParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_WbSimInfo(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus = SKIP_VALID_CHECK;
    }
}


static void StaticBadPixelCorrectionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_FPN_s Data;
    void *pField[StaticBadPixelCorrectionParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_StaticBadPixelCorrection(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.CalibVersion;
    pField[2] = &Data.CalibVinStartX;
    pField[3] = &Data.CalibVinStartY;
    pField[4] = &Data.CalibVinWidth;
    pField[5] = &Data.CalibVinHeight;
    pField[6] = &Data.CalibVinHSubSampleFactorNum;
    pField[7] = &Data.CalibVinHSubSampleFactorDen;
    pField[8] = &Data.CalibVinVSubSampleFactorNum;
    pField[9] = &Data.CalibVinVSubSampleFactorDen;
    pField[10] = Data.MapPath;
    Max = StaticBadPixelCorrectionParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_StaticBadPixelCorrection(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0U) && (Data.Enable == 0U)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}

static void StaticBadPixelCorrectionInternalProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_FPN_INTERNAL_s Data;
    void *pField[StaticBadPixelCorrectionInternalParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_SBP_CorrectionInternal(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.PixelMapWidth;
    pField[2] = &Data.PixelMapHeight;
    pField[3] = &Data.PixelMapPitch;
    pField[4] = Data.MapPath;
    Max = StaticBadPixelCorrectionInternalParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_SBP_CorrectionInternal(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0U) && (Data.Enable == 0U)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}

static void VignetteCompensationProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_VIGNETTE_s Data;
    void *pField[VignetteCompensationParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_VignetteCompensation(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.VignetteInfo.CalibVignetteInfo.NumRadialBinsCoarse;
    pField[2] = &Data.VignetteInfo.CalibVignetteInfo.SizeRadialBinsCoarseLog;
    pField[3] = &Data.VignetteInfo.CalibVignetteInfo.NumRadialBinsFine;
    pField[4] = &Data.VignetteInfo.CalibVignetteInfo.SizeRadialBinsFineLog;
    pField[5] = &Data.VignetteInfo.CalibVignetteInfo.ModelCenterXR;
    pField[6] = &Data.VignetteInfo.CalibVignetteInfo.ModelCenterYR;
    pField[7] = Data.VigGainTblPath;
    pField[8] = &Data.VignetteInfo.CalibVignetteInfo.Version;
    pField[9] = &Data.VignetteInfo.VinSensorGeo.StartX;
    pField[10] = &Data.VignetteInfo.VinSensorGeo.StartY;
    pField[11] = &Data.VignetteInfo.VinSensorGeo.Width;
    pField[12] = &Data.VignetteInfo.VinSensorGeo.Height;
    pField[13] = &Data.VignetteInfo.VinSensorGeo.HSubSample.FactorNum;
    pField[14] = &Data.VignetteInfo.VinSensorGeo.HSubSample.FactorDen;
    pField[15] = &Data.VignetteInfo.VinSensorGeo.VSubSample.FactorNum;
    pField[16] = &Data.VignetteInfo.VinSensorGeo.VSubSample.FactorDen;
    pField[17] = &Data.VignetteInfo.CalibModeEnable;
    Max = VignetteCompensationParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_VignetteCompensation(&Data);
    if (ParamStatus != NULL) {
        if ((Index == 17) && (Data.VignetteInfo.CalibModeEnable== 0U)) {
            *ParamStatus |=((0x1ULL << 8) | //version
                            (0x1ULL << 9) | //current_vin_sensor_geo.start_x
                            (0x1ULL << 10) | //current_vin_sensor_geo.start_y
                            (0x1ULL << 11) | //current_vin_sensor_geo.width
                            (0x1ULL << 12) | //current_vin_sensor_geo.height
                            (0x1ULL << 13) | //current_vin_sensor_geo.h_sub_sample.factor_num
                            (0x1ULL << 14) | //current_vin_sensor_geo.h_sub_sample.factor_den
                            (0x1ULL << 15) | //current_vin_sensor_geo.v_sub_sample.factor_num
                            (0x1ULL << 16) | //current_vin_sensor_geo.v_sub_sample.factor_den
                            (0x1ULL << 17)); //calib_mode_enable
        } else {
            *ParamStatus |= (((Idx == 0U) && (Data.Enable== 0U)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
        }
    }
}

static void WarpCompensationProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_WARP_s Data;
    void *pField[WarpCompensationParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_GetWarpInfo(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.CalibVersion;
    pField[2] = &Data.HorizontalGridNumber;
    pField[3] = &Data.VerticalGridNumber;
    pField[4] = &Data.TileWidthExponent;
    pField[5] = &Data.TileHeightExponent;
    pField[6] = &Data.VinSensorStartX;
    pField[7] = &Data.VinSensorStartY;
    pField[8] = &Data.VinSensorWidth;
    pField[9] = &Data.VinSensorHeight;
    pField[10] = &Data.VinSensorHSubSampleFactorNum;
    pField[11] = &Data.VinSensorHSubSampleFactorDen;
    pField[12] = &Data.VinSensorVSubSampleFactorNum;
    pField[13] = &Data.VinSensorVSubSampleFactorDen;
    pField[14] = &Data.Enb2StageCompensation;
    pField[15] = Data.WarpGridTablePath;
    Max = WarpCompensationParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_SetWarpInfo(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0U) && (Data.Enable == 0U)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}

static void WarpCompensation2ndProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_WARP_s Data;
    void *pField[WarpCompensationParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_GetWarp2ndInfo(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.CalibVersion;
    pField[2] = &Data.HorizontalGridNumber;
    pField[3] = &Data.VerticalGridNumber;
    pField[4] = &Data.TileWidthExponent;
    pField[5] = &Data.TileHeightExponent;
    pField[6] = &Data.VinSensorStartX;
    pField[7] = &Data.VinSensorStartY;
    pField[8] = &Data.VinSensorWidth;
    pField[9] = &Data.VinSensorHeight;
    pField[10] = &Data.VinSensorHSubSampleFactorNum;
    pField[11] = &Data.VinSensorHSubSampleFactorDen;
    pField[12] = &Data.VinSensorVSubSampleFactorNum;
    pField[13] = &Data.VinSensorVSubSampleFactorDen;
    pField[14] = &Data.Enb2StageCompensation;
    pField[15] = Data.WarpGridTablePath;
    Max = WarpCompensationParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_SetWarp2ndInfo(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0U) && (Data.Enable == 0U)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}

static void DzoomProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_DZOOM_INFO_s Data;
    void *pField[DzoomParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Dzoom(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.ShiftX;
    pField[2] = &Data.ShiftY;
    pField[3] = &Data.ZoomX;
    pField[4] = &Data.ZoomY;
    Max = DzoomParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Dzoom(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0U) && (Data.Enable== 0U)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}

static void WarpCompensationDzoomInternalProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_WARP_DZOOM_INTERNAL_s Data;
    void *pField[WarpCompensationDzoomInternalParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_WarpCompensationDzoomInternal(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.GridArrayWidth;
    pField[2] = &Data.GridArrayHeight;
    pField[3] = &Data.HorzGridSpacingExponent;
    pField[4] = &Data.VertGridSpacingExponent;
    pField[5] = &Data.VertWarpGridArrayWidth;
    pField[6] = &Data.VertWarpGridArrayHeight;
    pField[7] = &Data.VertWarpHorzGridSpacingExponent;
    pField[8] = &Data.VertWarpVertGridSpacingExponent;
    pField[9] = Data.WarpHorizontalTablePath;
    pField[10] = Data.WarpVerticalTablePath;
    Max = WarpCompensationDzoomInternalParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_WarpCompensationDzoomInternal(&Data);
    if (ParamStatus != NULL) {
        if ((Idx == 0U) && (Data.Enable == 0U)) {
            *ParamStatus |= ((0x1ULL) | //WarpControl
                             (0x1ULL << 1) | //GridArrayWidth
                             (0x1ULL << 2) | //GridArrayHeight
                             (0x1ULL << 3) | //HorzGridSpacingExponent
                             (0x1ULL << 4) | //VertGridSpacingExponent
                             (0x1ULL << 5) | //VertWarpGridArrayWidth
                             (0x1ULL << 6) | //VertWarpGridArrayHeight
                             (0x1ULL << 7) | //VertWarpHorzGridSpacingExponent
                             (0x1ULL << 8) | //VertWarpVertGridSpacingExponent
                             (0x1ULL << 9) | //WarpHorizontalTablePath
                             (0x1ULL << 10)); //WarpVerticalTablePath
        } else {
            *ParamStatus |= (0x1ULL << Idx);
        }
    }
}

static void SensorInputProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_SENSOR_INPUT_s Data;
    void *pField[SensorInputParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_SensorInputInfo(&Data);
    pField[0] = &Data.SensorId;
    pField[1] = &Data.BayerPattern;
    pField[2] = &Data.SensorResolution;
    pField[3] = &Data.ReadoutMode;
    Max = SensorInputParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_SensorInputInfo(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void DeferredBlackLevelProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_DEFERRED_BLC_s Data;
    void *pField[DeferredBlackLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_DeferredBlackLevel(&Data);
    pField[0] = &Data.Enable;
    Max = DeferredBlackLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_DeferredBlackLevel(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void DynamicBadPixelCorrectionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_DYNAMIC_BAD_PXL_COR_s Data;
    void *pField[DynamicBadPixelCorrectionParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_DynamicBadPixelCorrection(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.HotPixelStrength;
    pField[2] = &Data.DarkPixelStrength;
    pField[3] = &Data.CorrectionMethod;
    pField[4] = &Data.HotPixelStrengthIRModeRedBlue;
    pField[5] = &Data.DarkPixelStrengthIRModeRedBlue;
    Max = DynamicBadPixelCorrectionParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_DynamicBadPixelCorrection(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0U) && (Data.Enable== 0U)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}

static void CfaLeakageFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_CFA_LEAKAGE_FILTER_s Data;
    void *pField[CfaLeakageFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_CfaLeakageFilter(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.AlphaRr;
    pField[2] = &Data.AlphaRb;
    pField[3] = &Data.AlphaBr;
    pField[4] = &Data.AlphaBb;
    pField[5] = &Data.SaturationLevel;
    Max = CfaLeakageFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_CfaLeakageFilter(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0U) && (Data.Enable== 0U)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}

static void AntiAliasingProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_ANTI_ALIASING_s Data;
    void *pField[AntiAliasingParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_AntiAliasing(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Thresh;
    pField[2] = &Data.LogFractionalCorrect;
    Max = AntiAliasingParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_AntiAliasing(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void CfaNoiseFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_CFA_NOISE_FILTER_s Data;
    void *pField[CfaNoiseFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_CfaNoiseFilter(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.MinNoiseLevel[0];
    pField[2] = &Data.MinNoiseLevel[1];
    pField[3] = &Data.MinNoiseLevel[2];
    pField[4] = &Data.MaxNoiseLevel[0];
    pField[5] = &Data.MaxNoiseLevel[1];
    pField[6] = &Data.MaxNoiseLevel[2];
    pField[7] = &Data.OriginalBlendStr[0];
    pField[8] = &Data.OriginalBlendStr[1];
    pField[9] = &Data.OriginalBlendStr[2];
    pField[10] = &Data.ExtentRegular[0];
    pField[11] = &Data.ExtentRegular[1];
    pField[12] = &Data.ExtentRegular[2];
    pField[13] = &Data.ExtentFine[0];
    pField[14] = &Data.ExtentFine[1];
    pField[15] = &Data.ExtentFine[2];
    pField[16] = &Data.StrengthFine[0];
    pField[17] = &Data.StrengthFine[1];
    pField[18] = &Data.StrengthFine[2];
    pField[19] = &Data.SelectivityRegular;
    pField[20] = &Data.SelectivityFine;
    pField[21] = &Data.DirectionalEnable;
    pField[22] = &Data.DirectionalHorvertEdgeStrength;
    pField[23] = &Data.DirectionalHorvertStrengthBias;
    pField[24] = &Data.DirectionalHorvertOriginalBlendStrength;
    pField[25] = &Data.DirLevMax[0];
    pField[26] = &Data.DirLevMax[1];
    pField[27] = &Data.DirLevMax[2];
    pField[28] = &Data.DirLevMin[0];
    pField[29] = &Data.DirLevMin[1];
    pField[30] = &Data.DirLevMin[2];
    pField[31] = &Data.DirLevMul[0];
    pField[32] = &Data.DirLevMul[1];
    pField[33] = &Data.DirLevMul[2];
    pField[34] = &Data.DirLevOffset[0];
    pField[35] = &Data.DirLevOffset[1];
    pField[36] = &Data.DirLevOffset[2];
    pField[37] = &Data.DirLevShift[0];
    pField[38] = &Data.DirLevShift[1];
    pField[39] = &Data.DirLevShift[2];
    pField[40] = &Data.LevMul[0];
    pField[41] = &Data.LevMul[1];
    pField[42] = &Data.LevMul[2];
    pField[43] = &Data.LevOffset[0];
    pField[44] = &Data.LevOffset[1];
    pField[45] = &Data.LevOffset[2];
    pField[46] = &Data.LevShift[0];
    pField[47] = &Data.LevShift[1];
    pField[48] = &Data.LevShift[2];
    pField[49] = &Data.ApplyToColorDiffRed;
    pField[50] = &Data.ApplyToColorDiffBlue;
    pField[51] = &Data.OptForRccbRed;
    pField[52] = &Data.OptForRccbBlue;
    Max = CfaNoiseFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_CfaNoiseFilter(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
        if(((Idx == 21u) && (Data.DirectionalEnable == 0U))) {
            *ParamStatus |= (0x1ULL << 22);
            *ParamStatus |= (0x1ULL << 23);
            *ParamStatus |= (0x1ULL << 24);
        }
    }
}

static void BeforeCeWbGainProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_WB_GAIN_s Data;
    void *pField[WbGainParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_BeforeCeWbGain(&Data);
    pField[0] = &Data.GainR;
    pField[1] = &Data.GainG;
    pField[2] = &Data.GainB;
    Max = WbGainParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_BeforeCeWbGain(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void AfterCeWbGainProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_WB_GAIN_s Data;
    void *pField[WbGainParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_AfterCeWbGain(&Data);
    pField[0] = &Data.GainR;
    pField[1] = &Data.GainG;
    pField[2] = &Data.GainB;
    Max = WbGainParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_AfterCeWbGain(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void StoredIrProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_STORED_IR_s Data;
    void *pField[StoredIrParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_StoredIr(&Data);
    pField[0] = &Data.IrShift;
    pField[1] = Data.IrToneCurve;
    Max = StoredIrParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_StoredIr(&Data);
    if (ParamStatus!=NULL) {
        *ParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void ColorCorrectionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_COLOR_CORRECTION_s Data;
    void *pField[ColorCorrectionParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_ColorCorrection(&Data);
    pField[0] = Data.ThreeDPath;
    pField[1] = &Data.MatrixEn;
    pField[2] = Data.Matrix;
    pField[3] = &Data.MatrixShiftMinus8;
    Max = ColorCorrectionParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_ColorCorrection(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void ToneCurveProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_TONE_CURVE_s Data;
    void *pField[ToneCurveParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_ToneCurve(&Data);
    pField[0] = Data.ToneCurveRed;
    pField[1] = Data.ToneCurveGreen;
    pField[2] = Data.ToneCurveBlue;
    Max = ToneCurveParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_ToneCurve(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void FrontEndToneCurveProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_FE_TONE_CURVE_s Data;
    void *pField[FrontEndToneCurveParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_FeToneCurve(&Data);
    pField[0] = &Data.Compand.CompandEnable;
    pField[1] = Data.Compand.CompandTable;
    pField[2] = &Data.Decompand.DecompandEnable;
    pField[3] = Data.Decompand.DecompandTable;
    Max = FrontEndToneCurveParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_FeToneCurve(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
        if(((Idx == 0U) && (Data.Compand.CompandEnable == 0U)) || ((Idx == 2U) && (Data.Decompand.DecompandEnable == 0U))) {
            *ParamStatus |= SKIP_VALID_CHECK;
        }
    }
}

static void RgbToYuvMatrixProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_RGB_TO_YUV_MATRIX_s Data;
    void *pField[RgbToYuvMatrixParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_RgbToYuvMatrix(&Data);
    pField[0] = &Data.MatrixValues[0];
    pField[1] = &Data.MatrixValues[1];
    pField[2] = &Data.MatrixValues[2];
    pField[3] = &Data.MatrixValues[3];
    pField[4] = &Data.MatrixValues[4];
    pField[5] = &Data.MatrixValues[5];
    pField[6] = &Data.MatrixValues[6];
    pField[7] = &Data.MatrixValues[7];
    pField[8] = &Data.MatrixValues[8];
    pField[9] = &Data.YOffset;
    pField[10] = &Data.UOffset;
    pField[11] = &Data.VOffset;
    Max = RgbToYuvMatrixParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_RgbToYuvMatrix(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void RgbIrProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_RGB_IR_s Data;
    void *pField[RgbIrParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_RgbIr(&Data);
    pField[0] = &Data.Mode;
    pField[1] = &Data.IrcorrectOffsetB;
    pField[2] = &Data.IrcorrectOffsetGb;
    pField[3] = &Data.IrcorrectOffsetGr;
    pField[4] = &Data.IrcorrectOffsetR;
    pField[5] = &Data.MulBaseVal;
    pField[6] = &Data.MulDeltaHigh;
    pField[7] = &Data.MulHigh;
    pField[8] = &Data.MulHighVal;
    pField[9] = &Data.IrOnly;
    Max = RgbIrParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_RgbIr(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
        if(((Index == 0) && (Data.Mode== 0U))) {
            *ParamStatus |= SKIP_VALID_CHECK;
        }
    }
}

static void ChromaScaleProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_CHROMA_SCALE_s Data;
    void *pField[ChromaScaleParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_ChromaScale(&Data);
    pField[0] = &Data.Enable;
    pField[1] = Data.GainCurve;
    Max = ChromaScaleParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_ChromaScale(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0U) && (Data.Enable== 0U)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}

static void ChromaMedianFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_CHROMA_MEDIAN_FILTER_s Data;
    void *pField[ChromaMedianFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_ChromaMedianFilter(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.CbAdaptiveStrength;
    pField[2] = &Data.CrAdaptiveStrength;
    pField[3] = &Data.CbNonAdaptiveStrength;
    pField[4] = &Data.CrNonAdaptiveStrength;
    pField[5] = &Data.CbAdaptiveAmount;
    pField[6] = &Data.CrAdaptiveAmount;
    Max = ChromaMedianFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_ChromaMedianFilter(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0U) && (Data.Enable== 0UL)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}

static void DemosaicFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_DEMOSAIC_s Data;
    void *pField[DemosaicFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_DemosaicFilter(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.ActivityThresh;
    pField[2] = &Data.ActivityDifferenceThresh;
    pField[3] = &Data.GradClipThresh;
    pField[4] = &Data.GradNoiseThresh;
    pField[5] = &Data.AliasInterpolationStrength;
    pField[6] = &Data.AliasInterpolationThresh;
    Max = DemosaicFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_DemosaicFilter(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void RgbToY12Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_RGB_TO_12Y_s Data;
    void *pField[RgbToY12Params_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_RgbTo12Y(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.MatrixValues[0];
    pField[2] = &Data.MatrixValues[1];
    pField[3] = &Data.MatrixValues[2];
    pField[4] = &Data.YOffset;
    Max = RgbToY12Params_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_RgbTo12Y(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void SharpenBothProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_SHARPEN_BOTH_s Data;
    void *pField[SharpeningBothParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_SharpenBoth(&Data);
    pField[0] = &Data.BothInfo.Enable;
    pField[1] = &Data.BothInfo.Mode;
    pField[2] = &Data.BothInfo.EdgeThresh;
    pField[3] = &Data.BothInfo.WideEdgeDetect;
    pField[4] = &Data.BothInfo.MaxChangeUp5x5;
    pField[5] = &Data.BothInfo.MaxChangeDown5x5;
    Max = SharpeningBothParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_SharpenBoth(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0U) && (Data.BothInfo.Enable== 0U)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}

static void FinalSharpenBothProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_FINAL_SHARPEN_BOTH_s Data;
    void *pField[FinalSharpeningBothParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_FinalSharpenBoth(&Data);
    pField[0] = &Data.BothInfo.Enable;
    pField[1] = &Data.BothInfo.Mode;
    pField[2] = &Data.BothInfo.EdgeThresh;
    pField[3] = &Data.BothInfo.WideEdgeDetect;
    pField[4] = &Data.BothInfo.MaxChange.Up5x5;
    pField[5] = &Data.BothInfo.MaxChange.Down5x5;
    pField[6] = Data.ThreedTable.ThreeD;
    pField[7] = &Data.ThreedTable.UToneBits;
    pField[8] = &Data.ThreedTable.UToneOffset;
    pField[9] = &Data.ThreedTable.UToneShift;
    pField[10] = &Data.ThreedTable.VToneBits;
    pField[11] = &Data.ThreedTable.VToneOffset;
    pField[12] = &Data.ThreedTable.VToneShift;
    pField[13] = &Data.ThreedTable.YToneOffset;
    pField[14] = &Data.ThreedTable.YToneShift;
    pField[15] = &Data.BothInfo.MaxChange.Up;
    pField[16] = &Data.BothInfo.MaxChange.Down;
    pField[17] = Data.BothInfo.NarrowNonsmoothDetectSub;
    pField[18] = &Data.BothInfo.NarrowNonsmoothDetectShift;
    Max = FinalSharpeningBothParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_FinalSharpenBoth(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0U) && (Data.BothInfo.Enable== 0U)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}

static void SharpenNoiseProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FSTSHPNS_NOISE_s Data;
    void *pField[SharpeningNoiseParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_SharpenNoise(&Data);
    pField[0] = &Data.FirstShpFir.Specify;
    pField[1] = &Data.FirstShpFir.StrengthIso;
    pField[2] = &Data.FirstShpFir.StrengthDir;
    pField[3] = Data.FirstShpFir.PerDirFirDirAmounts;
    pField[4] = Data.FirstShpFir.PerDirFirDirStrengths;
    pField[5] = Data.FirstShpFir.PerDirFirIsoStrengths;
    pField[6] = Data.FirstShpFir.Coefs;
    pField[7] = &Data.MaxChangeDown;
    pField[8] = &Data.MaxChangeUp;
    pField[9] = &Data.LevelStrAdjust.High;
    pField[10] = &Data.LevelStrAdjust.HighDelta;
    pField[11] = &Data.LevelStrAdjust.HighStrength;
    pField[12] = &Data.LevelStrAdjust.MidStrength;
    pField[13] = &Data.LevelStrAdjust.Low;
    pField[14] = &Data.LevelStrAdjust.LowDelta;
    pField[15] = &Data.LevelStrAdjust.LowStrength;
    pField[16] = &Data.LevelStrAdjustNotT0T1LevelBased;
    pField[17] = &Data.T0;
    pField[18] = &Data.T1;
    pField[19] = &Data.AlphaMin;
    pField[20] = &Data.AlphaMax;
    Max = SharpeningNoiseParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_SharpenNoise(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void FinalSharpenNoiseProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FNLSHPNS_NOISE_s Data;
    void *pField[FinalSharpeningNoiseParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_FinalSharpenNoise(&Data);
    pField[0] = &Data.FinalShpFir.Specify;
    pField[1] = &Data.FinalShpFir.StrengthIso;
    pField[2] = &Data.FinalShpFir.StrengthDir;
    pField[3] = Data.FinalShpFir.PerDirFirDirAmounts;
    pField[4] = Data.FinalShpFir.PerDirFirDirStrengths;
    pField[5] = Data.FinalShpFir.PerDirFirIsoStrengths;
    pField[6] = Data.FinalShpFir.Coefs;
    pField[7] = &Data.MaxChangeDown;
    pField[8] = &Data.MaxChangeUp;
    pField[9] = &Data.LevelStrAdjust.High;
    pField[10] = &Data.LevelStrAdjust.HighDelta;
    pField[11] = &Data.LevelStrAdjust.HighStrength;
    pField[12] = &Data.LevelStrAdjust.MidStrength;
    pField[13] = &Data.LevelStrAdjust.Low;
    pField[14] = &Data.LevelStrAdjust.LowDelta;
    pField[15] = &Data.LevelStrAdjust.LowStrength;
    pField[16] = &Data.LevelStrAdjustNotT0T1LevelBased;
    pField[17] = &Data.T0;
    pField[18] = &Data.T1;
    pField[19] = &Data.AlphaMin;
    pField[20] = &Data.AlphaMax;
    pField[21] = &Data.LevelStrAdjust.Method;
    pField[22] = &Data.DirectionalDecideT0;
    pField[23] = &Data.DirectionalDecideT1;
    pField[24] = &Data.AdvancedIso.Enable;
    pField[25] = &Data.AdvancedIso.MaxChangeMethod;
    pField[26] = Data.AdvancedIso.MaxChangeTable;
    pField[27] = &Data.AdvancedIso.NoiseLevelMethod;
    pField[28] = Data.AdvancedIso.NoiseLevelTable;
    pField[29] = &Data.AdvancedIso.Size;
    pField[30] = &Data.AdvancedIso.StrAMethod;
    pField[31] = Data.AdvancedIso.StrATable;
    pField[32] = &Data.AdvancedIso.StrBMethod;
    pField[33] = Data.AdvancedIso.StrBTable;
    Max = FinalSharpeningNoiseParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_FinalSharpenNoise(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}



static void SharpenFirProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FSTSHPNS_FIR_s Data;
    void *pField[FirstSharpeningFirParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Fir(&Data);
    pField[0] = &Data.Specify;
    pField[1] = &Data.StrengthIso;
    pField[2] = &Data.StrengthDir;
    pField[3] = Data.PerDirFirDirAmounts;
    pField[4] = Data.PerDirFirDirStrengths;
    pField[5] = Data.PerDirFirIsoStrengths;
    pField[6] = Data.Coefs;
    Max = FirstSharpeningFirParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Fir(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void SharpenCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FSTSHPNS_CORING_s Data;
    void *pField[SharpeningCoringParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Coring(&Data);
    pField[0] = Data.Coring;
    pField[1] = &Data.FractionalBits;
    Max = SharpeningCoringParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Coring(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void FinalSharpenFirProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FNLSHPNS_FIR_s Data;
    void *pField[FinalSharpeningFirParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_FinalFir(&Data);
    pField[0] = &Data.Specify;
    pField[1] = &Data.StrengthIso;
    pField[2] = &Data.StrengthDir;
    pField[3] = Data.PerDirFirDirAmounts;
    pField[4] = Data.PerDirFirDirStrengths;
    pField[5] = Data.PerDirFirIsoStrengths;
    pField[6] = Data.Coefs;
    pField[7] = &Data.PosFirArtifactReduceEnable;
    pField[8] = &Data.PosFirArtifactReduceStrength;
    pField[9] = &Data.OverShootReduceAllow;
    pField[10] = &Data.OverShootReduceStrength;
    pField[11] = &Data.UnderShootReduceAllow;
    pField[12] = &Data.UnderShootReduceStrength;
    Max = FinalSharpeningFirParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_FinalFir(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void FinalSharpenCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FNLSHPNS_CORING_s Data;
    void *pField[FianlSharpeningCoringParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_FinalCoring(&Data);
    pField[0] = Data.Coring;
    pField[1] = &Data.FractionalBits;
    Max = FianlSharpeningCoringParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_FinalCoring(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void CoringIndexScaleProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FSTSHPNS_COR_IDX_SCL_s Data;
    void *pField[SharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_CoringIndexScale(&Data);
    pField[0] = &Data.High;
    pField[1] = &Data.HighDelta;
    pField[2] = &Data.HighStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.Low;
    pField[5] = &Data.LowDelta;
    pField[6] = &Data.LowStrength;
    Max = SharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_CoringIndexScale(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void MinCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FSTSHPNS_MIN_COR_RST_s Data;
    void *pField[SharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_MinCoringResult(&Data);
    pField[0] = &Data.High;
    pField[1] = &Data.HighDelta;
    pField[2] = &Data.HighStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.Low;
    pField[5] = &Data.LowDelta;
    pField[6] = &Data.LowStrength;
    Max = SharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_MinCoringResult(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void MaxCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FSTSHPNS_MAX_COR_RST_s Data;
    void *pField[SharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_MaxCoringResult(&Data);
    pField[0] = &Data.High;
    pField[1] = &Data.HighDelta;
    pField[2] = &Data.HighStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.Low;
    pField[5] = &Data.LowDelta;
    pField[6] = &Data.LowStrength;
    Max = SharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_MaxCoringResult(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void ScaleCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FSTSHPNS_SCL_COR_s Data;
    void *pField[SharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_ScaleCoring(&Data);
    pField[0] = &Data.High;
    pField[1] = &Data.HighDelta;
    pField[2] = &Data.HighStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.Low;
    pField[5] = &Data.LowDelta;
    pField[6] = &Data.LowStrength;
    Max = SharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_ScaleCoring(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void FinalCoringIndexScaleProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FNLSHPNS_COR_IDX_SCL_s Data;
    void *pField[FinalSharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_FinalCoringIndexScale(&Data);
    pField[0] = &Data.High;
    pField[1] = &Data.HighDelta;
    pField[2] = &Data.HighStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.Low;
    pField[5] = &Data.LowDelta;
    pField[6] = &Data.LowStrength;
    pField[7] = &Data.Method;
    Max = FinalSharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_FinalCoringIndexScale(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void FinalMinCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FNLSHPNS_MIN_COR_RST_s Data;
    void *pField[FinalSharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_FinalMinCoringResult(&Data);
    pField[0] = &Data.High;
    pField[1] = &Data.HighDelta;
    pField[2] = &Data.HighStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.Low;
    pField[5] = &Data.LowDelta;
    pField[6] = &Data.LowStrength;
    pField[7] = &Data.Method;
    Max = FinalSharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_FinalMinCoringResult(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void FinalMaxCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FNLSHPNS_MAX_COR_RST_s Data;
    void *pField[FinalSharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_FinalMaxCoringResult(&Data);
    pField[0] = &Data.High;
    pField[1] = &Data.HighDelta;
    pField[2] = &Data.HighStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.Low;
    pField[5] = &Data.LowDelta;
    pField[6] = &Data.LowStrength;
    pField[7] = &Data.Method;
    Max = FinalSharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_FinalMaxCoringResult(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void FinalScaleCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FNLSHPNS_SCL_COR_s Data;
    void *pField[FinalSharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_FinalScaleCoring(&Data);
    pField[0] = &Data.High;
    pField[1] = &Data.HighDelta;
    pField[2] = &Data.HighStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.Low;
    pField[5] = &Data.LowDelta;
    pField[6] = &Data.LowStrength;
    pField[7] = &Data.Method;
    Max = FinalSharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_FinalScaleCoring(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void VideoMctfProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_VIDEO_MCTF_s Data;
    void *pField[VideoMctfParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_VideoMctf(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.YNarrowNonsmoothDetectShift;
    pField[2] = Data.YNarrowNonsmoothDetectSub;
    pField[3] = &Data.UseLevelBasedTa;
    pField[4] = &Data.YSpatBlend.High;
    pField[5] = &Data.YSpatBlend.HighDelta;
    pField[6] = &Data.YSpatBlend.HighStrength;
    pField[7] = &Data.YSpatBlend.Low;
    pField[8] = &Data.YSpatBlend.LowDelta;
    pField[9] = &Data.YSpatBlend.LowStrength;
    pField[10] = &Data.YSpatBlend.Method;
    pField[11] = &Data.YSpatBlend.MidStrength;
    pField[12] = &Data.CbSpatBlend.High;
    pField[13] = &Data.CbSpatBlend.HighDelta;
    pField[14] = &Data.CbSpatBlend.HighStrength;
    pField[15] = &Data.CbSpatBlend.Low;
    pField[16] = &Data.CbSpatBlend.LowDelta;
    pField[17] = &Data.CbSpatBlend.LowStrength;
    pField[18] = &Data.CbSpatBlend.Method;
    pField[19] = &Data.CbSpatBlend.MidStrength;
    pField[20] = &Data.CrSpatBlend.High;
    pField[21] = &Data.CrSpatBlend.HighDelta;
    pField[22] = &Data.CrSpatBlend.HighStrength;
    pField[23] = &Data.CrSpatBlend.Low;
    pField[24] = &Data.CrSpatBlend.LowDelta;
    pField[25] = &Data.CrSpatBlend.LowStrength;
    pField[26] = &Data.CrSpatBlend.Method;
    pField[27] = &Data.CrSpatBlend.MidStrength;
    /*advanced iso*/
    pField[28] = Data.YAdvancedIso.MaxChangeTable;
    pField[29] = &Data.YAdvancedIso.MaxChangeMethod;
    pField[30] = &Data.YAdvancedIso.Size;
    pField[31] = &Data.YAdvancedIso.NoiseLevel;
    pField[32] = &Data.YAdvancedIso.Enable;
    pField[33] = Data.CbAdvancedIso.MaxChangeTable;
    pField[34] = &Data.CbAdvancedIso.MaxChangeMethod;
    pField[35] = &Data.CbAdvancedIso.Size;
    pField[36] = &Data.CbAdvancedIso.NoiseLevel;
    pField[37] = &Data.CbAdvancedIso.Enable;
    pField[38] = Data.CrAdvancedIso.MaxChangeTable;
    pField[39] = &Data.CrAdvancedIso.MaxChangeMethod;
    pField[40] = &Data.CrAdvancedIso.Size;
    pField[41] = &Data.CrAdvancedIso.NoiseLevel;
    pField[42] = &Data.CrAdvancedIso.Enable;
    /*3d maxchange*/
    pField[43] = &Data.YOverallMaxChange.High;
    pField[44] = &Data.YOverallMaxChange.HighDelta;
    pField[45] = &Data.YOverallMaxChange.HighStrength;
    pField[46] = &Data.YOverallMaxChange.Low;
    pField[47] = &Data.YOverallMaxChange.LowDelta;
    pField[48] = &Data.YOverallMaxChange.LowStrength;
    pField[49] = &Data.YOverallMaxChange.Method;
    pField[50] = &Data.YOverallMaxChange.MidStrength;
    pField[51] = &Data.CbOverallMaxChange.High;
    pField[52] = &Data.CbOverallMaxChange.HighDelta;
    pField[53] = &Data.CbOverallMaxChange.HighStrength;
    pField[54] = &Data.CbOverallMaxChange.Low;
    pField[55] = &Data.CbOverallMaxChange.LowDelta;
    pField[56] = &Data.CbOverallMaxChange.LowStrength;
    pField[57] = &Data.CbOverallMaxChange.Method;
    pField[58] = &Data.CbOverallMaxChange.MidStrength;
    pField[59] = &Data.CrOverallMaxChange.High;
    pField[60] = &Data.CrOverallMaxChange.HighDelta;
    pField[61] = &Data.CrOverallMaxChange.HighStrength;
    pField[62] = &Data.CrOverallMaxChange.Low;
    pField[63] = &Data.CrOverallMaxChange.LowDelta;
    pField[64] = &Data.CrOverallMaxChange.LowStrength;
    pField[65] = &Data.CrOverallMaxChange.Method;
    pField[66] = &Data.CrOverallMaxChange.MidStrength;
    /*3d maxchange*/
    pField[67] = &Data.Y3dMaxchange.High;
    pField[68] = &Data.Y3dMaxchange.HighDelta;
    pField[69] = &Data.Y3dMaxchange.HighStrength;
    pField[70] = &Data.Y3dMaxchange.Low;
    pField[71] = &Data.Y3dMaxchange.LowDelta;
    pField[72] = &Data.Y3dMaxchange.LowStrength;
    pField[73] = &Data.Y3dMaxchange.Method;
    pField[74] = &Data.Y3dMaxchange.MidStrength;
    pField[75] = &Data.Cb3dMaxchange.High;
    pField[76] = &Data.Cb3dMaxchange.HighDelta;
    pField[77] = &Data.Cb3dMaxchange.HighStrength;
    pField[78] = &Data.Cb3dMaxchange.Low;
    pField[79] = &Data.Cb3dMaxchange.LowDelta;
    pField[80] = &Data.Cb3dMaxchange.LowStrength;
    pField[81] = &Data.Cb3dMaxchange.Method;
    pField[82] = &Data.Cb3dMaxchange.MidStrength;
    pField[83] = &Data.Cr3dMaxchange.High;
    pField[84] = &Data.Cr3dMaxchange.HighDelta;
    pField[85] = &Data.Cr3dMaxchange.HighStrength;
    pField[86] = &Data.Cr3dMaxchange.Low;
    pField[87] = &Data.Cr3dMaxchange.LowDelta;
    pField[88] = &Data.Cr3dMaxchange.LowStrength;
    pField[89] = &Data.Cr3dMaxchange.Method;
    pField[90] = &Data.Cr3dMaxchange.MidStrength;
    /*spat_filt_max_smth_change*/
    pField[91] = &Data.YSpatFiltMaxSmthChange.High;
    pField[92] = &Data.YSpatFiltMaxSmthChange.HighDelta;
    pField[93] = &Data.YSpatFiltMaxSmthChange.HighStrength;
    pField[94] = &Data.YSpatFiltMaxSmthChange.Low;
    pField[95] = &Data.YSpatFiltMaxSmthChange.LowDelta;
    pField[96] = &Data.YSpatFiltMaxSmthChange.LowStrength;
    pField[97] = &Data.YSpatFiltMaxSmthChange.Method;
    pField[98] = &Data.YSpatFiltMaxSmthChange.MidStrength;
    pField[99] = &Data.CbSpatFiltMaxSmthChange.High;
    pField[100] = &Data.CbSpatFiltMaxSmthChange.HighDelta;
    pField[101] = &Data.CbSpatFiltMaxSmthChange.HighStrength;
    pField[102] = &Data.CbSpatFiltMaxSmthChange.Low;
    pField[103] = &Data.CbSpatFiltMaxSmthChange.LowDelta;
    pField[104] = &Data.CbSpatFiltMaxSmthChange.LowStrength;
    pField[105] = &Data.CbSpatFiltMaxSmthChange.Method;
    pField[106] = &Data.CbSpatFiltMaxSmthChange.MidStrength;
    pField[107] = &Data.CrSpatFiltMaxSmthChange.High;
    pField[108] = &Data.CrSpatFiltMaxSmthChange.HighDelta;
    pField[109] = &Data.CrSpatFiltMaxSmthChange.HighStrength;
    pField[110] = &Data.CrSpatFiltMaxSmthChange.Low;
    pField[111] = &Data.CrSpatFiltMaxSmthChange.LowDelta;
    pField[112] = &Data.CrSpatFiltMaxSmthChange.LowStrength;
    pField[113] = &Data.CrSpatFiltMaxSmthChange.Method;
    pField[114] = &Data.CrSpatFiltMaxSmthChange.MidStrength;
    /*spat_smth_dir*/
    pField[115] = &Data.YSpatSmthWideEdgeDetect;
    pField[116] = &Data.YSpatSmthEdgeThresh;
    pField[117] = &Data.YSpatSmthDir.High;
    pField[118] = &Data.YSpatSmthDir.HighDelta;
    pField[119] = &Data.YSpatSmthDir.HighStrength;
    pField[120] = &Data.YSpatSmthDir.Low;
    pField[121] = &Data.YSpatSmthDir.LowDelta;
    pField[122] = &Data.YSpatSmthDir.LowStrength;
    pField[123] = &Data.YSpatSmthDir.Method;
    pField[124] = &Data.YSpatSmthDir.MidStrength;
    pField[125] = &Data.CbSpatSmthWideEdgeDetect;
    pField[126] = &Data.CbSpatSmthEdgeThresh;
    pField[127] = &Data.CbSpatSmthDir.High;
    pField[128] = &Data.CbSpatSmthDir.HighDelta;
    pField[129] = &Data.CbSpatSmthDir.HighStrength;
    pField[130] = &Data.CbSpatSmthDir.Low;
    pField[131] = &Data.CbSpatSmthDir.LowDelta;
    pField[132] = &Data.CbSpatSmthDir.LowStrength;
    pField[133] = &Data.CbSpatSmthDir.Method;
    pField[134] = &Data.CbSpatSmthDir.MidStrength;
    pField[135] = &Data.CrSpatSmthWideEdgeDetect;
    pField[136] = &Data.CrSpatSmthEdgeThresh;
    pField[137] = &Data.CrSpatSmthDir.High;
    pField[138] = &Data.CrSpatSmthDir.HighDelta;
    pField[139] = &Data.CrSpatSmthDir.HighStrength;
    pField[140] = &Data.CrSpatSmthDir.Low;
    pField[141] = &Data.CrSpatSmthDir.LowDelta;
    pField[142] = &Data.CrSpatSmthDir.LowStrength;
    pField[143] = &Data.CrSpatSmthDir.Method;
    pField[144] = &Data.CrSpatSmthDir.MidStrength;
    /*spat_smth_iso*/
    pField[145] = &Data.YSpatSmthIso.High;
    pField[146] = &Data.YSpatSmthIso.HighDelta;
    pField[147] = &Data.YSpatSmthIso.HighStrength;
    pField[148] = &Data.YSpatSmthIso.Low;
    pField[149] = &Data.YSpatSmthIso.LowDelta;
    pField[150] = &Data.YSpatSmthIso.LowStrength;
    pField[151] = &Data.YSpatSmthIso.Method;
    pField[152] = &Data.YSpatSmthIso.MidStrength;
    pField[153] = &Data.CbSpatSmthIso.High;
    pField[154] = &Data.CbSpatSmthIso.HighDelta;
    pField[155] = &Data.CbSpatSmthIso.HighStrength;
    pField[156] = &Data.CbSpatSmthIso.Low;
    pField[157] = &Data.CbSpatSmthIso.LowDelta;
    pField[158] = &Data.CbSpatSmthIso.LowStrength;
    pField[159] = &Data.CbSpatSmthIso.Method;
    pField[160] = &Data.CbSpatSmthIso.MidStrength;
    pField[161] = &Data.CrSpatSmthIso.High;
    pField[162] = &Data.CrSpatSmthIso.HighDelta;
    pField[163] = &Data.CrSpatSmthIso.HighStrength;
    pField[164] = &Data.CrSpatSmthIso.Low;
    pField[165] = &Data.CrSpatSmthIso.LowDelta;
    pField[166] = &Data.CrSpatSmthIso.LowStrength;
    pField[167] = &Data.CrSpatSmthIso.Method;
    pField[168] = &Data.CrSpatSmthIso.MidStrength;
    /*level_based_ta*/
    pField[169] = &Data.YLevelBasedTa.High;
    pField[170] = &Data.YLevelBasedTa.HighDelta;
    pField[171] = &Data.YLevelBasedTa.HighStrength;
    pField[172] = &Data.YLevelBasedTa.Low;
    pField[173] = &Data.YLevelBasedTa.LowDelta;
    pField[174] = &Data.YLevelBasedTa.LowStrength;
    pField[175] = &Data.YLevelBasedTa.Method;
    pField[176] = &Data.YLevelBasedTa.MidStrength;
    pField[177] = &Data.CbLevelBasedTa.High;
    pField[178] = &Data.CbLevelBasedTa.HighDelta;
    pField[179] = &Data.CbLevelBasedTa.HighStrength;
    pField[180] = &Data.CbLevelBasedTa.Low;
    pField[181] = &Data.CbLevelBasedTa.LowDelta;
    pField[182] = &Data.CbLevelBasedTa.LowStrength;
    pField[183] = &Data.CbLevelBasedTa.Method;
    pField[184] = &Data.CbLevelBasedTa.MidStrength;
    pField[185] = &Data.CrLevelBasedTa.High;
    pField[186] = &Data.CrLevelBasedTa.HighDelta;
    pField[187] = &Data.CrLevelBasedTa.HighStrength;
    pField[188] = &Data.CrLevelBasedTa.Low;
    pField[189] = &Data.CrLevelBasedTa.LowDelta;
    pField[190] = &Data.CrLevelBasedTa.LowStrength;
    pField[191] = &Data.CrLevelBasedTa.Method;
    pField[192] = &Data.CrLevelBasedTa.MidStrength;
    /*temporal_min_target*/
    pField[193] = &Data.YCurve.TemporalAlpha0;
    pField[194] = &Data.YCurve.TemporalAlpha1;
    pField[195] = &Data.YCurve.TemporalAlpha2;
    pField[196] = &Data.YCurve.TemporalAlpha3;
    pField[197] = &Data.YCurve.TemporalT0;
    pField[198] = &Data.YCurve.TemporalT1;
    pField[199] = &Data.YCurve.TemporalT2;
    pField[200] = &Data.YCurve.TemporalT3;
    pField[201] = &Data.YTemporalMaxChange;
    pField[202] = &Data.YTemporalMinTarget.High;
    pField[203] = &Data.YTemporalMinTarget.HighDelta;
    pField[204] = &Data.YTemporalMinTarget.HighStrength;
    pField[205] = &Data.YTemporalMinTarget.Low;
    pField[206] = &Data.YTemporalMinTarget.LowDelta;
    pField[207] = &Data.YTemporalMinTarget.LowStrength;
    pField[208] = &Data.YTemporalMinTarget.Method;
    pField[209] = &Data.YTemporalMinTarget.MidStrength;
    pField[210] = &Data.CbCurve.TemporalAlpha0;
    pField[211] = &Data.CbCurve.TemporalAlpha1;
    pField[212] = &Data.CbCurve.TemporalAlpha2;
    pField[213] = &Data.CbCurve.TemporalAlpha3;
    pField[214] = &Data.CbCurve.TemporalT0;
    pField[215] = &Data.CbCurve.TemporalT1;
    pField[216] = &Data.CbCurve.TemporalT2;
    pField[217] = &Data.CbCurve.TemporalT3;
    pField[218] = &Data.CbTemporalMaxChange;
    pField[219] = &Data.CbTemporalMinTarget.High;
    pField[220] = &Data.CbTemporalMinTarget.HighDelta;
    pField[221] = &Data.CbTemporalMinTarget.HighStrength;
    pField[222] = &Data.CbTemporalMinTarget.Low;
    pField[223] = &Data.CbTemporalMinTarget.LowDelta;
    pField[224] = &Data.CbTemporalMinTarget.LowStrength;
    pField[225] = &Data.CbTemporalMinTarget.Method;
    pField[226] = &Data.CbTemporalMinTarget.MidStrength;
    pField[227] = &Data.CrCurve.TemporalAlpha0;
    pField[228] = &Data.CrCurve.TemporalAlpha1;
    pField[229] = &Data.CrCurve.TemporalAlpha2;
    pField[230] = &Data.CrCurve.TemporalAlpha3;
    pField[231] = &Data.CrCurve.TemporalT0;
    pField[232] = &Data.CrCurve.TemporalT1;
    pField[233] = &Data.CrCurve.TemporalT2;
    pField[234] = &Data.CrCurve.TemporalT3;
    pField[235] = &Data.CrTemporalMaxChange;
    pField[236] = &Data.CrTemporalMinTarget.High;
    pField[237] = &Data.CrTemporalMinTarget.HighDelta;
    pField[238] = &Data.CrTemporalMinTarget.HighStrength;
    pField[239] = &Data.CrTemporalMinTarget.Low;
    pField[240] = &Data.CrTemporalMinTarget.LowDelta;
    pField[241] = &Data.CrTemporalMinTarget.LowStrength;
    pField[242] = &Data.CrTemporalMinTarget.Method;
    pField[243] = &Data.CrTemporalMinTarget.MidStrength;
    /*spatial_max_temporal*/
    pField[244] = &Data.YSpatialMaxChange;
    pField[245] = &Data.YSpatialMaxTemporal.High;
    pField[246] = &Data.YSpatialMaxTemporal.HighDelta;
    pField[247] = &Data.YSpatialMaxTemporal.HighStrength;
    pField[248] = &Data.YSpatialMaxTemporal.Low;
    pField[249] = &Data.YSpatialMaxTemporal.LowDelta;
    pField[250] = &Data.YSpatialMaxTemporal.LowStrength;
    pField[251] = &Data.YSpatialMaxTemporal.Method;
    pField[252] = &Data.YSpatialMaxTemporal.MidStrength;
    pField[253] = &Data.CbSpatialMaxChange;
    pField[254] = &Data.CbSpatialMaxTemporal.High;
    pField[255] = &Data.CbSpatialMaxTemporal.HighDelta;
    pField[256] = &Data.CbSpatialMaxTemporal.HighStrength;
    pField[257] = &Data.CbSpatialMaxTemporal.Low;
    pField[258] = &Data.CbSpatialMaxTemporal.LowDelta;
    pField[259] = &Data.CbSpatialMaxTemporal.LowStrength;
    pField[260] = &Data.CbSpatialMaxTemporal.Method;
    pField[261] = &Data.CbSpatialMaxTemporal.MidStrength;
    pField[262] = &Data.CrSpatialMaxChange;
    pField[263] = &Data.CrSpatialMaxTemporal.High;
    pField[264] = &Data.CrSpatialMaxTemporal.HighDelta;
    pField[265] = &Data.CrSpatialMaxTemporal.HighStrength;
    pField[266] = &Data.CrSpatialMaxTemporal.Low;
    pField[267] = &Data.CrSpatialMaxTemporal.LowDelta;
    pField[268] = &Data.CrSpatialMaxTemporal.LowStrength;
    pField[269] = &Data.CrSpatialMaxTemporal.Method;
    pField[270] = &Data.CrSpatialMaxTemporal.MidStrength;
    pField[271] = &Data.YSpatSmthDirectDecideT0;
    pField[272] = &Data.YSpatSmthDirectDecideT1;
    pField[273] = &Data.CbSpatSmthDirectDecideT0;
    pField[274] = &Data.CbSpatSmthDirectDecideT1;
    pField[275] = &Data.CrSpatSmthDirectDecideT0;
    pField[276] = &Data.CrSpatSmthDirectDecideT1;
    pField[277] = Data.YSpatialWeighting;
    pField[278] = Data.CbSpatialWeighting;
    pField[279] = Data.CrSpatialWeighting;
    pField[280] = &Data.YStrength3d;
    pField[281] = &Data.CbStrength3d;
    pField[282] = &Data.CrStrength3d;
    pField[283] = &Data.YCombineStrength;
    pField[284] = &Data.LocalLighting;
    pField[285] = &Data.CompressionEnable;
    pField[286] = &Data.YTemporalGhostPrevent;
    pField[287] = &Data.CbTemporalGhostPrevent;
    pField[288] = &Data.CrTemporalGhostPrevent;
    pField[289] = &Data.YTemporalArtifactGuard;
    pField[290] = &Data.CbTemporalArtifactGuard;
    pField[291] = &Data.CrTemporalArtifactGuard;
    pField[292] = &Data.CompressionBitRateLuma;
    pField[293] = &Data.CompressionBitRateChroma;
    pField[294] = &Data.CompressionDitherDisable;

    Max = VideoMctfParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_VideoMctf(&Data);
    if (ParamStatus != NULL) {
        if ((Idx == 0U) && (Data.Enable== 0U)) {
            ParamStatus[0] = SKIP_VALID_CHECK;
            ParamStatus[1] = SKIP_VALID_CHECK;
            ParamStatus[2] = SKIP_VALID_CHECK;
            ParamStatus[3] = SKIP_VALID_CHECK;
            ParamStatus[4] = SKIP_VALID_CHECK;
        } else {
            ParamStatus[Idx>>6U] |= (0x1ULL << (Idx&0x3FU));
        }
    }
}

static void VideoMctfLevelProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_VIDEO_MCTF_s Data;
    void *pField[VideoMctfLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_VideoMctfLevel(&Data);
    pField[0] = &Data.YTemporalMaxChangeNotT0T1LevelBased;
    pField[1] = &Data.YTemporalEitherMaxChangeOrT0T1Add.High;
    pField[2] = &Data.YTemporalEitherMaxChangeOrT0T1Add.HighDelta;
    pField[3] = &Data.YTemporalEitherMaxChangeOrT0T1Add.HighStrength;
    pField[4] = &Data.YTemporalEitherMaxChangeOrT0T1Add.Low;
    pField[5] = &Data.YTemporalEitherMaxChangeOrT0T1Add.LowDelta;
    pField[6] = &Data.YTemporalEitherMaxChangeOrT0T1Add.LowStrength;
    pField[7] = &Data.YTemporalEitherMaxChangeOrT0T1Add.Method;
    pField[8] = &Data.YTemporalEitherMaxChangeOrT0T1Add.MidStrength;

    pField[9] = &Data.CbTemporalMaxChangeNotT0T1LevelBased;
    pField[10] = &Data.CbTemporalEitherMaxChangeOrT0T1Add.High;
    pField[11] = &Data.CbTemporalEitherMaxChangeOrT0T1Add.HighDelta;
    pField[12] = &Data.CbTemporalEitherMaxChangeOrT0T1Add.HighStrength;
    pField[13] = &Data.CbTemporalEitherMaxChangeOrT0T1Add.Low;
    pField[14] = &Data.CbTemporalEitherMaxChangeOrT0T1Add.LowDelta;
    pField[15] = &Data.CbTemporalEitherMaxChangeOrT0T1Add.LowStrength;
    pField[16] = &Data.CbTemporalEitherMaxChangeOrT0T1Add.Method;
    pField[17] = &Data.CbTemporalEitherMaxChangeOrT0T1Add.MidStrength;

    pField[18] = &Data.CrTemporalMaxChangeNotT0T1LevelBased;
    pField[19] = &Data.CrTemporalEitherMaxChangeOrT0T1Add.High;
    pField[20] = &Data.CrTemporalEitherMaxChangeOrT0T1Add.HighDelta;
    pField[21] = &Data.CrTemporalEitherMaxChangeOrT0T1Add.HighStrength;
    pField[22] = &Data.CrTemporalEitherMaxChangeOrT0T1Add.Low;
    pField[23] = &Data.CrTemporalEitherMaxChangeOrT0T1Add.LowDelta;
    pField[24] = &Data.CrTemporalEitherMaxChangeOrT0T1Add.LowStrength;
    pField[25] = &Data.CrTemporalEitherMaxChangeOrT0T1Add.Method;
    pField[26] = &Data.CrTemporalEitherMaxChangeOrT0T1Add.MidStrength;
    Max = VideoMctfLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_VideoMctfLevel(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void VideoMctfTemporalAdjustProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_VIDEO_MCTF_TA_s Data;
    void *pField[VideoMctfTemporalAdjustParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_VideoMctfTemporalAdjust(&Data);
    pField[0] = &Data.EdgeAdjustB;
    pField[1] = &Data.EdgeAdjustL;
    pField[2] = &Data.EdgeAdjustR;
    pField[3] = &Data.EdgeAdjustT;
    pField[4] = &Data.SlowMoSensitivity;
    pField[5] = &Data.ScoreNoiseRobust;
    pField[6] = &Data.LevAdjustLow;
    pField[7] = &Data.LevAdjustLowDelta;
    pField[8] = &Data.LevAdjustLowStrength;
    pField[9] = &Data.LevAdjustMidStrength;
    pField[10] = &Data.LevAdjustHigh;
    pField[11] = &Data.LevAdjustHighDelta;
    pField[12] = &Data.LevAdjustHighStrength;
    pField[13] = &Data.YMctfTa.Max;
    pField[14] = &Data.YMctfTa.Min;
    pField[15] = &Data.YMctfTa.MotionResponse;
    pField[16] = &Data.YMctfTa.NoiseBase;
    pField[17] = &Data.YMctfTa.StillThresh;
    pField[18] = &Data.CrMctfTa.Max;
    pField[19] = &Data.CrMctfTa.Min;
    pField[20] = &Data.CrMctfTa.MotionResponse;
    pField[21] = &Data.CrMctfTa.NoiseBase;
    pField[22] = &Data.CrMctfTa.StillThresh;
    pField[23] = &Data.CbMctfTa.Max;
    pField[24] = &Data.CbMctfTa.Min;
    pField[25] = &Data.CbMctfTa.MotionResponse;
    pField[26] = &Data.CbMctfTa.NoiseBase;
    pField[27] = &Data.CbMctfTa.StillThresh;
    Max = VideoMctfTemporalAdjustParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_VideoMctfTemporalAdjust(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void VideoMctfAndFinalSharpenProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_POS_DSP33_t Data;
    void *pField[VideoMctfAndFinalSharpenParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_VideoMctfAndFinalSharpen(&Data);
    pField[0] = Data.PosDep;
    Max = VideoMctfAndFinalSharpenParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_VideoMctfAndFinalSharpen(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void ShpAOrSpatialFilterSelectproc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FIRST_LUMA_PROC_MODE_s Data;
    void *pField[VideoMctfAndFinalSharpenParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_ShpAOrSpatialFilterSelect(&Data);
    pField[0] = &Data.UseSharpenNotAsf;
    Max = VideoMctfAndFinalSharpenParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_ShpAOrSpatialFilterSelect(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void AdvanceSpatialFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_ASF_INFO_s Data;
    void *pField[AdvanceSpatialFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_AsfInfo(&Data);
    pField[0] = &Data.AsfInfo.Enable;
    pField[1] = &Data.AsfInfo.Fir.Specify;
    pField[2] = &Data.AsfInfo.Fir.StrengthIso;
    pField[3] = &Data.AsfInfo.Fir.StrengthDir;
    pField[4] = Data.AsfInfo.Fir.PerDirFirDirAmounts;
    pField[5] = Data.AsfInfo.Fir.PerDirFirDirStrengths;
    pField[6] = Data.AsfInfo.Fir.PerDirFirIsoStrengths;
    pField[7] = Data.AsfInfo.Fir.Coefs;
    pField[8] = &Data.AsfInfo.Fir.WideEdgeDetect;
    pField[9] = &Data.AsfInfo.DirectionalDecideT0;
    pField[10] = &Data.AsfInfo.DirectionalDecideT1;
    pField[11] = &Data.AsfInfo.Adapt.AlphaMaxDown;
    pField[12] = &Data.AsfInfo.Adapt.AlphaMaxUp;
    pField[13] = &Data.AsfInfo.Adapt.AlphaMinDown;
    pField[14] = &Data.AsfInfo.Adapt.AlphaMinUp;
    pField[15] = &Data.AsfInfo.Adapt.T0Down;
    pField[16] = &Data.AsfInfo.Adapt.T0Up;
    pField[17] = &Data.AsfInfo.Adapt.T1Down;
    pField[18] = &Data.AsfInfo.Adapt.T1Up;
    pField[19] = &Data.AsfInfo.LevelStrAdjust.High;
    pField[20] = &Data.AsfInfo.LevelStrAdjust.HighDelta;
    pField[21] = &Data.AsfInfo.LevelStrAdjust.HighStrength;
    pField[22] = &Data.AsfInfo.LevelStrAdjust.MidStrength;
    pField[23] = &Data.AsfInfo.LevelStrAdjust.Low;
    pField[24] = &Data.AsfInfo.LevelStrAdjust.LowDelta;
    pField[25] = &Data.AsfInfo.LevelStrAdjust.LowStrength;
    pField[26] = &Data.AsfInfo.T0T1Div.High;
    pField[27] = &Data.AsfInfo.T0T1Div.HighDelta;
    pField[28] = &Data.AsfInfo.T0T1Div.HighStrength;
    pField[29] = &Data.AsfInfo.T0T1Div.Low;
    pField[30] = &Data.AsfInfo.T0T1Div.LowDelta;
    pField[31] = &Data.AsfInfo.T0T1Div.LowStrength;
    pField[32] = &Data.AsfInfo.T0T1Div.MidStrength;
    pField[33] = &Data.AsfInfo.MaxChangeDown;
    pField[34] = &Data.AsfInfo.MaxChangeUp;
    pField[35] = &Data.AsfInfo.MaxChangeNotT0T1Alpha;
    Max = AdvanceSpatialFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_AsfInfo(&Data);
    if (ParamStatus != NULL) {
        if ((Idx == 0U) && (Data.AsfInfo.Enable == 0U)) {
            *ParamStatus |= SKIP_VALID_CHECK;
        } else {
            *ParamStatus |= (0x1ULL << Idx);
        }
    }
}

static void LumaNoiseReductionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LUMA_NOISE_REDUCTION_s Data;
    void *pField[LumaNoiseReductionParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_LumaNoiseReduction(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.SensorWbB;
    pField[2] = &Data.SensorWbG;
    pField[3] = &Data.SensorWbR;
    pField[4] = &Data.Strength0;
    pField[5] = &Data.Strength1;
    pField[6] = &Data.Strength2;
    pField[7] = &Data.Strength2MaxChange;
    Max = LumaNoiseReductionParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_LumaNoiseReduction(&Data);
    if (ParamStatus != NULL) {
        if ((Idx == 0U) && (Data.Enable == 0U)) {
            *ParamStatus |= SKIP_VALID_CHECK;
        } else {
            *ParamStatus |= (0x1ULL << Idx);
        }
    }
}

static void HiLumaNoiseReductionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LUMA_NOISE_REDUCTION_s Data;
    void *pField[HiLumaNoiseReductionParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiLumaNoiseReduction(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.SensorWbB;
    pField[2] = &Data.SensorWbG;
    pField[3] = &Data.SensorWbR;
    pField[4] = &Data.Strength0;
    pField[5] = &Data.Strength1;
    pField[6] = &Data.Strength2;
    pField[7] = &Data.Strength2MaxChange;
    Max = HiLumaNoiseReductionParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Index]);
    AmbaItuner_Set_HiLumaNoiseReduction(&Data);
    if (ParamStatus != NULL) {
        if ((Idx == 0U) && (Data.Enable == 0U)) {
            *ParamStatus |= SKIP_VALID_CHECK;
        } else {
            *ParamStatus |= (0x1ULL << Idx);
        }
    }
}

static void Li2LumaNoiseReductionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LUMA_NOISE_REDUCTION_s Data;
    void *pField[Li2LumaNoiseReductionParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Li2LumaNoiseReduction(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.SensorWbB;
    pField[2] = &Data.SensorWbG;
    pField[3] = &Data.SensorWbR;
    pField[4] = &Data.Strength0;
    pField[5] = &Data.Strength1;
    pField[6] = &Data.Strength2;
    pField[7] = &Data.Strength2MaxChange;
    Max = Li2LumaNoiseReductionParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Index]);
    AmbaItuner_Set_Li2LumaNoiseReduction(&Data);
    if (ParamStatus != NULL) {
        if ((Idx == 0U) && (Data.Enable == 0U)) {
            *ParamStatus |= SKIP_VALID_CHECK;
        } else {
            *ParamStatus |= (0x1ULL << Idx);
        }
    }
}

static void ChromaFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_CHROMA_FILTER_s Data;
    void *pField[ChromaFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_ChromaFilter(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.NoiseLevelCb;
    pField[2] = &Data.NoiseLevelCr;
    pField[3] = &Data.OriginalBlendStrengthCb;
    pField[4] = &Data.OriginalBlendStrengthCr;
    pField[5] = &Data.Radius;
    Max = ChromaFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_ChromaFilter(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0U) && (Data.Enable== 0U)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}

static void GbGrMismatchProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_GRGB_MISMATCH_s Data;
    void *pField[GbGrMismatchParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_GbGrMismatch(&Data);
    pField[0] = &Data.NarrowEnable;
    pField[1] = &Data.WideEnable;
    pField[2] = &Data.WideSafety;
    pField[3] = &Data.WideThresh;
    Max = GbGrMismatchParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_GbGrMismatch(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void ChromaAberrationProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_CHROMA_ABERRATION_s Data;
    void *pField[ChromaAberrationParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_GetCawarpInfo(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.CalibVersion;
    pField[2] = &Data.HorizontalGridNumber;
    pField[3] = &Data.VerticalGridNumber;
    pField[4] = &Data.TileWidthExponent;
    pField[5] = &Data.TileHeightExponent;
    pField[6] = &Data.VinSensorStartX;
    pField[7] = &Data.VinSensorStartY;
    pField[8] = &Data.VinSensorWidth;
    pField[9] = &Data.VinSensorHeight;
    pField[10] = &Data.VinSensorHSubSampleFactorNum;
    pField[11] = &Data.VinSensorHSubSampleFactorDen;
    pField[12] = &Data.VinSensorVSubSampleFactorNum;
    pField[13] = &Data.VinSensorVSubSampleFactorDen;
    pField[14] = Data.RedCaGridTablePath;
    pField[15] = Data.BlueCaGridTablePath;
    Max = ChromaAberrationParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_SetCawarpInfo(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0U) && (Data.Enable == 0U)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}

static void ChromaAberrationInternalProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_CHROMA_ABERRATION_INTERNAL_s Data;
    void *pField[ChromaAberrationInternalParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_GetCaWarpInternal(&Data);
    pField[0] = &Data.HorzWarpEnable;
    pField[1] = &Data.VertWarpEnable;
    pField[2] = &Data.HorzPassGridArrayWidth;
    pField[3] = &Data.HorzPassGridArrayHeight;
    pField[4] = &Data.HorzPassHorzGridSpacingExponent;
    pField[5] = &Data.HorzPassVertGridSpacingExponent;
    pField[6] = &Data.VertPassGridArrayWidth;
    pField[7] = &Data.VertPassGridArrayHeight;
    pField[8] = &Data.VertPassHorzGridSpacingExponent;
    pField[9] = &Data.VertPassVertGridSpacingExponent;
    pField[10] = Data.WarpHorzTablePathRed;
    pField[11] = Data.WarpVertTablePathRed;
    pField[12] = Data.WarpHorzTablePathBlue;
    pField[13] = Data.WarpVertTablePathBlue;
    Max = ChromaAberrationInternalParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_SetCaWarpInternal(&Data);
    if (ParamStatus != NULL) {
        if ((Idx == 0U) && (Data.HorzWarpEnable == 0U)) {
            *ParamStatus |= ((0x1ULL << 0) | //HorzWarpEnable
                             (0x1ULL << 2) | //HorzPassGridArrayWidth
                             (0x1ULL << 3) | //HorzPassGridArrayHeight
                             (0x1ULL << 4) | //HorzPassHorzGridSpacingExponent
                             (0x1ULL << 5) | //HorzPassVertGridSpacingExponent
                             (0x1ULL << 10) | //WarpHorzTablePath
                             (0x1ULL << 12)); //WarpHorzTablePath
        } else if ((Idx == 1U) && (Data.VertWarpEnable == 0U)) {
            *ParamStatus |= ((0x1ULL << 1) | //VertWarpEnable
                             (0x1ULL << 6) | //VertPassGridArrayWidth
                             (0x1ULL << 7) | //VertPassGridArrayHeight
                             (0x1ULL << 8) | //VertPassHorzGridSpacingExponent
                             (0x1ULL << 9) | //VertPassVertGridSpacingExponent
                             (0x1ULL << 11) | //WarpVertTablePath
                             (0x1ULL << 13)); //WarpVertTablePath
        } else {
            *ParamStatus |= (0x1ULL << Idx);
        }
    }
}

static void ContrastEnhance_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    ITUNER_VIDEO_CONTRAST_ENHC_s Data;
    void *pField[ContrastEnhance_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_VideoContrastEnhance(&Data);
    pField[0] = &Data.ContrastEnhance.Enable;
    pField[1] = &Data.ContrastEnhance.LumaAvgMethod;
    pField[2] = &Data.ContrastEnhance.LumaAvgWeightR;
    pField[3] = &Data.ContrastEnhance.LumaAvgWeightGr;
    pField[4] = &Data.ContrastEnhance.LumaAvgWeightGb;
    pField[5] = &Data.ContrastEnhance.LumaAvgWeightB;
    pField[6] = &Data.ContrastEnhance.FirEnable;
    pField[7] = Data.ContrastEnhance.FirCoeff;
    pField[8] = &Data.ContrastEnhance.CoringIndexScaleShift;
    pField[9] = &Data.ContrastEnhance.CoringIndexScaleLowStrength;
    pField[10] = &Data.ContrastEnhance.CoringIndexScaleMidStrength;
    pField[11] = &Data.ContrastEnhance.CoringIndexScaleHighStrength;
    pField[12] = &Data.ContrastEnhance.CoringIndexScaleLow;
    pField[13] = &Data.ContrastEnhance.CoringIndexScaleHigh;
    pField[14] = &Data.ContrastEnhance.CoringIndexScaleLowDelta;
    pField[15] = &Data.ContrastEnhance.CoringIndexScaleHighDelta;
    pField[16] = &Data.ContrastEnhance.CoringGainShift;
    pField[17] = &Data.ContrastEnhance.CoringGainLowStrength;
    pField[18] = &Data.ContrastEnhance.CoringGainMidStrength;
    pField[19] = &Data.ContrastEnhance.CoringGainHighStrength;
    pField[20] = &Data.ContrastEnhance.CoringGainLow;
    pField[21] = &Data.ContrastEnhance.CoringGainHigh;
    pField[22] = &Data.ContrastEnhance.CoringGainLowDelta;
    pField[23] = &Data.ContrastEnhance.CoringGainHighDelta;
    pField[24] = &Data.ContrastEnhance.CoringTable[0];
    pField[25] = &Data.ContrastEnhanceOutTable.OutputEnable;
    pField[26] = &Data.ContrastEnhanceOutTable.OutputShift;
    pField[27] = &Data.ContrastEnhanceOutTable.OutputTable[0];
    pField[28] = &Data.ContrastEnhanceInputTable.InputEnable;
    pField[29] = &Data.ContrastEnhanceInputTable.InputTable[0];
    pField[30] = &Data.ContrastEnhance.BoostGainShift;
    pField[31] = Data.ContrastEnhance.BoostTable;
    pField[32] = &Data.ContrastEnhance.BoostTableSizeExp;
    pField[33] = &Data.ContrastEnhance.Radius;
    pField[34] = &Data.ContrastEnhance.Epsilon;
    Max = ContrastEnhance_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_VideoContrastEnhance(&Data);
    if (ParamStatus != NULL) {
        if ((Idx == 0U) && (Data.ContrastEnhance.Enable == 0U)) {
            *ParamStatus |= SKIP_VALID_CHECK;
        } else if ((Idx == 28U) || (Idx == 29U)) {
            *ParamStatus |= ((0x1ULL << 28) | (0x1ULL << 29));
        } else if ((Idx == 1U) && (Data.ContrastEnhance.Radius!= 0U)) {
            // Note: if radius != 0, ignore internal decimation factor
            *ParamStatus |= ((0x1ULL << 30) | (0x1ULL << 31) | (0x1ULL << 32) | (0x1ULL << 1));
        } else {
            *ParamStatus |= (0x1ULL << Idx);
        }
    }
}

static void HdrBlend_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HDR_BLEND_s Data;
    INT32 MisraI32 = (Index / HdrBlendParams_Count);
    UINT8 ArrayIndex = (UINT8)MisraI32;
    void *pField[HdrBlendParams_Count];
    UINT32 Idx;
    AmbaItuner_Get_VideoHdrBlend(ArrayIndex, &Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.DeltaT0;
    pField[2] = &Data.DeltaT1;
    pField[3] = &Data.FlickerThreshold;
    pField[4] = &Data.T0Offset;
    pField[5] = &Data.T1Offset;
    pField[6] = &Data.A0;
    pField[7] = &Data.A1;
    pField[8] = &Data.B0;
    pField[9] = &Data.B1;
    Idx = (UINT32)Index;
    ituner_param_proc(pParam, pField[Index % HdrBlendParams_Count]);
    AmbaItuner_Set_VideoHdrBlend(ArrayIndex, &Data);
    if (ParamStatus != NULL) {
        if ((Idx == 0U) && (Data.Enable== 0U)) {
            ParamStatus[0] = SKIP_VALID_CHECK;
        } else {
            *ParamStatus |= (0x1ULL << Idx);
        }
    }
}

static void HdrFrontEndWbgainExp0_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FE_WB_GAIN_s Data;
    UINT8 ArrayIndex = 0;
    void *pField[HdrFrontEndWbgain_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_VideoHdrFrontEndWbGain(ArrayIndex, &Data);
    pField[0] = &Data.Rgain;
    pField[1] = &Data.Ggain;
    pField[2] = &Data.Bgain;
    pField[3] = &Data.IrGain;
    pField[4] = &Data.ShutterRatio;
    Max = HdrFrontEndWbgain_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_VideoHdrFrontEndWbGain(ArrayIndex, &Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HdrFrontEndWbgainExp1_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FE_WB_GAIN_s Data;
    UINT8 ArrayIndex = 1;
    void *pField[HdrFrontEndWbgain_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_VideoHdrFrontEndWbGain(ArrayIndex, &Data);
    pField[0] = &Data.Rgain;
    pField[1] = &Data.Ggain;
    pField[2] = &Data.Bgain;
    pField[3] = &Data.IrGain;
    pField[4] = &Data.ShutterRatio;
    Max = HdrFrontEndWbgain_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_VideoHdrFrontEndWbGain(ArrayIndex, &Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HdrFrontEndWbgainExp2_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_FE_WB_GAIN_s Data;
    UINT8 ArrayIndex = 2;
    void *pField[HdrFrontEndWbgain_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_VideoHdrFrontEndWbGain(ArrayIndex, &Data);
    pField[0] = &Data.Rgain;
    pField[1] = &Data.Ggain;
    pField[2] = &Data.Bgain;
    pField[3] = &Data.IrGain;
    pField[4] = &Data.ShutterRatio;
    Max = HdrFrontEndWbgain_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_VideoHdrFrontEndWbGain(ArrayIndex, &Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HdrRawInfo_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_ITN_VIDEO_HDR_RAW_INFO_s Data;
    void *pField[HdrRawInfo_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_VideoHdrRawInfo(&Data);
    pField[0] = &Data.RawAreaWidth[0];
    pField[1] = &Data.RawAreaWidth[1];
    pField[2] = &Data.RawAreaWidth[2];
    pField[3] = &Data.RawAreaHeight[0];
    pField[4] = &Data.RawAreaHeight[1];
    pField[5] = &Data.RawAreaHeight[2];
    pField[6] = &Data.Offset.XOffset[0];
    pField[7] = &Data.Offset.XOffset[1];
    pField[8] = &Data.Offset.XOffset[2];
    pField[9] = &Data.Offset.YOffset[0];
    pField[10] = &Data.Offset.YOffset[1];
    pField[11] = &Data.Offset.YOffset[2];
    Max = HdrRawInfo_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_VideoHdrRawInfo(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HdrStaticBlackLevelExp0_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_STATIC_BLC_LVL_s Data;
    UINT8 ArrayIndex = 0;
    void *pField[StaticBlackLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_VideoHdrStaticBlackLevel(ArrayIndex, &Data);
    pField[0] = &Data.BlackR;
    pField[1] = &Data.BlackGr;
    pField[2] = &Data.BlackGb;
    pField[3] = &Data.BlackB;
    pField[4] = &Data.BlackG;
    pField[5] = &Data.BlackIr;
    Max = StaticBlackLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_VideoHdrStaticBlackLevel(ArrayIndex, &Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HdrStaticBlackLevelExp1_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_STATIC_BLC_LVL_s Data;
    UINT8 ArrayIndex = 1;
    void *pField[StaticBlackLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_VideoHdrStaticBlackLevel(ArrayIndex, &Data);
    pField[0] = &Data.BlackR;
    pField[1] = &Data.BlackGr;
    pField[2] = &Data.BlackGb;
    pField[3] = &Data.BlackB;
    pField[4] = &Data.BlackG;
    pField[5] = &Data.BlackIr;
    Max = StaticBlackLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_VideoHdrStaticBlackLevel(ArrayIndex, &Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HdrStaticBlackLevelExp2_Proc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_STATIC_BLC_LVL_s Data;
    UINT8 ArrayIndex = 2;
    void *pField[StaticBlackLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_VideoHdrStaticBlackLevel(ArrayIndex, &Data);
    pField[0] = &Data.BlackR;
    pField[1] = &Data.BlackGr;
    pField[2] = &Data.BlackGb;
    pField[3] = &Data.BlackB;
    pField[4] = &Data.BlackG;
    pField[5] = &Data.BlackIr;
    Max = StaticBlackLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_VideoHdrStaticBlackLevel(ArrayIndex, &Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void WideChromaFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_WIDE_CHROMA_FILTER_s Data;
    void *pField[WideChromaFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_WideChroma(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.NoiseLevelCb;
    pField[2] = &Data.NoiseLevelCr;
    Max = WideChromaFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_WideChroma(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0u) && (Data.Enable== 0u)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}

static void WideChromaFilterCombineProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_WIDE_CHROMA_FILTER_COMBINE_s Data;
    void *pField[WideChromaFilterCombineParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_WideChromaCombine(&Data);
    pField[0] = &Data.T0Cb;
    pField[1] = &Data.T0Cr;
    pField[2] = &Data.T1Cb;
    pField[3] = &Data.T1Cr;
    pField[4] = &Data.AlphaMaxCb;
    pField[5] = &Data.AlphaMaxCr;
    pField[6] = &Data.AlphaMinCb;
    pField[7] = &Data.AlphaMinCr;
    pField[8] = &Data.MaxChangeCb;
    pField[9] = &Data.MaxChangeCr;
    Max = WideChromaFilterCombineParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_WideChromaCombine(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void DummyProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_DUMMY_MARGIN_RANGE_s Data;
    void *pField[DummyParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Dummy(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Left;
    pField[2] = &Data.Right;
    pField[3] = &Data.Top;
    pField[4] = &Data.Bottom;
    Max = DummyParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Dummy(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0U) && (Data.Enable== 0U)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}

static void ActiveProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_VIN_ACTIVE_WINDOW_s Data;
    void *pField[ActiveParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Active(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.ActiveGeo.StartX;
    pField[2] = &Data.ActiveGeo.StartY;
    pField[3] = &Data.ActiveGeo.Width;
    pField[4] = &Data.ActiveGeo.Height;
    Max = ActiveParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Active(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0U) && (Data.Enable== 0U)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}
// hiso
static void LiChromaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_LOW_CHROMA_ASF_s Data;
    void *pField[HiChromaAdvanceSpatialFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_LiChromaAsf(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Fir.Specify;
    pField[2] = &Data.Fir.PerDirFirIsoStrengths;
    pField[3] = &Data.Fir.PerDirFirDirStrengths;
    pField[4] = &Data.Fir.PerDirFirDirAmounts;
    pField[5] = &Data.Fir.Coefs;
    pField[6] = &Data.Fir.StrengthIso;
    pField[7] = &Data.Fir.StrengthDir;
    pField[8] = &Data.Fir.WideEdgeDetect;
    pField[9] = &Data.DirectionalDecideT0;
    pField[10] = &Data.DirectionalDecideT1;
    pField[11] = &Data.AlphaMin;
    pField[12] = &Data.AlphaMax;
    pField[13] = &Data.T0;
    pField[14] = &Data.T1;
    pField[15] = &Data.LevelStrAdjust.Low;
    pField[16] = &Data.LevelStrAdjust.LowDelta;
    pField[17] = &Data.LevelStrAdjust.LowStrength;
    pField[18] = &Data.LevelStrAdjust.MidStrength;
    pField[19] = &Data.LevelStrAdjust.High;
    pField[20] = &Data.LevelStrAdjust.HighDelta;
    pField[21] = &Data.LevelStrAdjust.HighStrength;
    pField[22] = &Data.T0T1Div.Low;
    pField[23] = &Data.T0T1Div.LowDelta;
    pField[24] = &Data.T0T1Div.LowStrength;
    pField[25] = &Data.T0T1Div.MidStrength;
    pField[26] = &Data.T0T1Div.High;
    pField[27] = &Data.T0T1Div.HighDelta;
    pField[28] = &Data.T0T1Div.HighStrength;
    pField[29] = &Data.MaxChangeNotT0T1Alpha;
    pField[30] = &Data.MaxChange;
    Max = HiChromaAdvanceSpatialFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_LiChromaAsf(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiChromaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_CHROMA_ASF_s Data;
    void *pField[HiChromaAdvanceSpatialFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiChromaAsf(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Fir.Specify;
    pField[2] = &Data.Fir.PerDirFirIsoStrengths;
    pField[3] = &Data.Fir.PerDirFirDirStrengths;
    pField[4] = &Data.Fir.PerDirFirDirAmounts;
    pField[5] = &Data.Fir.Coefs;
    pField[6] = &Data.Fir.StrengthIso;
    pField[7] = &Data.Fir.StrengthDir;
    pField[8] = &Data.Fir.WideEdgeDetect;
    pField[9] = &Data.DirectionalDecideT0;
    pField[10] = &Data.DirectionalDecideT1;
    pField[11] = &Data.AlphaMin;
    pField[12] = &Data.AlphaMax;
    pField[13] = &Data.T0;
    pField[14] = &Data.T1;
    pField[15] = &Data.LevelStrAdjust.Low;
    pField[16] = &Data.LevelStrAdjust.LowDelta;
    pField[17] = &Data.LevelStrAdjust.LowStrength;
    pField[18] = &Data.LevelStrAdjust.MidStrength;
    pField[19] = &Data.LevelStrAdjust.High;
    pField[20] = &Data.LevelStrAdjust.HighDelta;
    pField[21] = &Data.LevelStrAdjust.HighStrength;
    pField[22] = &Data.T0T1Div.Low;
    pField[23] = &Data.T0T1Div.LowDelta;
    pField[24] = &Data.T0T1Div.LowStrength;
    pField[25] = &Data.T0T1Div.MidStrength;
    pField[26] = &Data.T0T1Div.High;
    pField[27] = &Data.T0T1Div.HighDelta;
    pField[28] = &Data.T0T1Div.HighStrength;
    pField[29] = &Data.MaxChangeNotT0T1Alpha;
    pField[30] = &Data.MaxChange;
    Max = HiChromaAdvanceSpatialFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiChromaAsf(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiLumaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_ASF_s Data;
    void *pField[HiLumaAdvanceSpatialFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiASF(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Fir.Specify;
    pField[2] = &Data.Fir.PerDirFirIsoStrengths;
    pField[3] = &Data.Fir.PerDirFirDirStrengths;
    pField[4] = &Data.Fir.PerDirFirDirAmounts;
    pField[5] = &Data.Fir.Coefs;
    pField[6] = &Data.Fir.StrengthIso;
    pField[7] = &Data.Fir.StrengthDir;
    pField[8] = &Data.Fir.WideEdgeDetect;
    pField[9] = &Data.DirectionalDecideT0;
    pField[10] = &Data.DirectionalDecideT1;
    pField[11] = &Data.Adapt.AlphaMinUp;
    pField[12] = &Data.Adapt.AlphaMaxUp;
    pField[13] = &Data.Adapt.T0Up;
    pField[14] = &Data.Adapt.T1Up;
    pField[15] = &Data.Adapt.AlphaMinDown;
    pField[16] = &Data.Adapt.AlphaMaxDown;
    pField[17] = &Data.Adapt.T0Down;
    pField[18] = &Data.Adapt.T1Down;
    pField[19] = &Data.LevelStrAdjust.Low;
    pField[20] = &Data.LevelStrAdjust.LowDelta;
    pField[21] = &Data.LevelStrAdjust.LowStrength;
    pField[22] = &Data.LevelStrAdjust.MidStrength;
    pField[23] = &Data.LevelStrAdjust.High;
    pField[24] = &Data.LevelStrAdjust.HighDelta;
    pField[25] = &Data.LevelStrAdjust.HighStrength;
    pField[26] = &Data.T0T1Div.Low;
    pField[27] = &Data.T0T1Div.LowDelta;
    pField[28] = &Data.T0T1Div.LowStrength;
    pField[29] = &Data.T0T1Div.MidStrength;
    pField[30] = &Data.T0T1Div.High;
    pField[31] = &Data.T0T1Div.HighDelta;
    pField[32] = &Data.T0T1Div.HighStrength;
    pField[33] = &Data.MaxChangeNotT0T1Alpha;
    pField[34] = &Data.MaxChangeUp;
    pField[35] = &Data.MaxChangeDown;
    Max = HiLumaAdvanceSpatialFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiASF(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiCfaNoiseFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_CFA_NOISE_FILTER_s Data;
    void *pField[HiCfaNoiseFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiCfaNoiseFilter(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.MinNoiseLevel[0];
    pField[2] = &Data.MinNoiseLevel[1];
    pField[3] = &Data.MinNoiseLevel[2];
    pField[4] = &Data.MaxNoiseLevel[0];
    pField[5] = &Data.MaxNoiseLevel[1];
    pField[6] = &Data.MaxNoiseLevel[2];
    pField[7] = &Data.OriginalBlendStr[0];
    pField[8] = &Data.OriginalBlendStr[1];
    pField[9] = &Data.OriginalBlendStr[2];
    pField[10] = &Data.ExtentRegular[0];
    pField[11] = &Data.ExtentRegular[1];
    pField[12] = &Data.ExtentRegular[2];
    pField[13] = &Data.ExtentFine[0];
    pField[14] = &Data.ExtentFine[1];
    pField[15] = &Data.ExtentFine[2];
    pField[16] = &Data.StrengthFine[0];
    pField[17] = &Data.StrengthFine[1];
    pField[18] = &Data.StrengthFine[2];
    pField[19] = &Data.SelectivityRegular;
    pField[20] = &Data.SelectivityFine;
    pField[21] = &Data.DirectionalEnable;
    pField[22] = &Data.DirectionalHorvertEdgeStrength;
    pField[23] = &Data.DirectionalHorvertStrengthBias;
    pField[24] = &Data.DirectionalHorvertOriginalBlendStrength;
    pField[25] = &Data.DirLevMax[0];
    pField[26] = &Data.DirLevMax[1];
    pField[27] = &Data.DirLevMax[2];
    pField[28] = &Data.DirLevMin[0];
    pField[29] = &Data.DirLevMin[1];
    pField[30] = &Data.DirLevMin[2];
    pField[31] = &Data.DirLevMul[0];
    pField[32] = &Data.DirLevMul[1];
    pField[33] = &Data.DirLevMul[2];
    pField[34] = &Data.DirLevOffset[0];
    pField[35] = &Data.DirLevOffset[1];
    pField[36] = &Data.DirLevOffset[2];
    pField[37] = &Data.DirLevShift[0];
    pField[38] = &Data.DirLevShift[1];
    pField[39] = &Data.DirLevShift[2];
    pField[40] = &Data.LevMul[0];
    pField[41] = &Data.LevMul[1];
    pField[42] = &Data.LevMul[2];
    pField[43] = &Data.LevOffset[0];
    pField[44] = &Data.LevOffset[1];
    pField[45] = &Data.LevOffset[2];
    pField[46] = &Data.LevShift[0];
    pField[47] = &Data.LevShift[1];
    pField[48] = &Data.LevShift[2];
    pField[49] = &Data.ApplyToColorDiffRed;
    pField[50] = &Data.ApplyToColorDiffBlue;
    pField[51] = &Data.OptForRccbRed;
    pField[52] = &Data.OptForRccbBlue;
    Max = HiCfaNoiseFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiCfaNoiseFilter(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiCfaLeakageFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_CFA_LEAKAGE_FILTER_s Data;
    void *pField[HiCfaLeakageFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiCfaLeakageFilter(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.AlphaRr;
    pField[2] = &Data.AlphaRb;
    pField[3] = &Data.AlphaBr;
    pField[4] = &Data.AlphaBb;
    pField[5] = &Data.SaturationLevel;
    Max = HiCfaLeakageFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiCfaLeakageFilter(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiAntiAliasingProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_ANTI_ALIASING_s Data;
    void *pField[HiAntiAliasingParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiAntiAliasing(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Thresh;
    pField[2] = &Data.LogFractionalCorrect;
    Max = HiAntiAliasingParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiAntiAliasing(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiAutoBadPixelCorrectionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_DYNAMIC_BPC_s Data;
    void *pField[HiAutoBadPixelCorrectionParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiDynamicBpc(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.HotPixelStrength;
    pField[2] = &Data.DarkPixelStrength;
    pField[3] = &Data.CorrectionMethod;
    Max = HiAutoBadPixelCorrectionParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiDynamicBpc(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiChromaFilterHighProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_CHROMA_FILTER_HIGH_s Data;
    void *pField[HiChromaFilterHighParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiChromaHigh(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.NoiseLevelCb;
    pField[2] = &Data.NoiseLevelCr;
    pField[3] = &Data.OriginalBlendStrengthCb;
    pField[4] = &Data.OriginalBlendStrengthCr;
    pField[5] = &Data.Radius;
    Max = HiChromaFilterHighParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiChromaHigh(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiChromaFilterLowProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_CHROMA_FILTER_LOW_s Data;
    void *pField[HiChromaFilterLowParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiChromaLow(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.NoiseLevelCb;
    pField[2] = &Data.NoiseLevelCr;
    pField[3] = &Data.OriginalBlendStrengthCb;
    pField[4] = &Data.OriginalBlendStrengthCr;
    Max = HiChromaFilterLowParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiChromaLow(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiChromaFilterLowCombineProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_CHROMA_FLTR_LOW_COM_s Data;
    void *pField[HiChromaFilterLowCombineParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiChromaLowCombine(&Data);
    pField[0] = &Data.T0Cb;
    pField[1] = &Data.T0Cr;
    pField[2] = &Data.T1Cb;
    pField[3] = &Data.T1Cr;
    pField[4] = &Data.AlphaMaxCb;
    pField[5] = &Data.AlphaMaxCr;
    pField[6] = &Data.AlphaMinCb;
    pField[7] = &Data.AlphaMinCr;
    pField[8] = &Data.MaxChangeLevelCb.Low;
    pField[9] = &Data.MaxChangeLevelCb.LowDelta;
    pField[10] = &Data.MaxChangeLevelCb.LowStrength;
    pField[11] = &Data.MaxChangeLevelCb.MidStrength;
    pField[12] = &Data.MaxChangeLevelCb.High;
    pField[13] = &Data.MaxChangeLevelCb.HighDelta;
    pField[14] = &Data.MaxChangeLevelCb.HighStrength;
    pField[15] = &Data.MaxChangeLevelCr.Low;
    pField[16] = &Data.MaxChangeLevelCr.LowDelta;
    pField[17] = &Data.MaxChangeLevelCr.LowStrength;
    pField[18] = &Data.MaxChangeLevelCr.MidStrength;
    pField[19] = &Data.MaxChangeLevelCr.High;
    pField[20] = &Data.MaxChangeLevelCr.HighDelta;
    pField[21] = &Data.MaxChangeLevelCr.HighStrength;
    pField[22] = &Data.T0T1AddLevelCb.Low;
    pField[23] = &Data.T0T1AddLevelCb.LowDelta;
    pField[24] = &Data.T0T1AddLevelCb.LowStrength;
    pField[25] = &Data.T0T1AddLevelCb.MidStrength;
    pField[26] = &Data.T0T1AddLevelCb.High;
    pField[27] = &Data.T0T1AddLevelCb.HighDelta;
    pField[28] = &Data.T0T1AddLevelCb.HighStrength;
    pField[29] = &Data.T0T1AddLevelCr.Low;
    pField[30] = &Data.T0T1AddLevelCr.LowDelta;
    pField[31] = &Data.T0T1AddLevelCr.LowStrength;
    pField[32] = &Data.T0T1AddLevelCr.MidStrength;
    pField[33] = &Data.T0T1AddLevelCr.High;
    pField[34] = &Data.T0T1AddLevelCr.HighDelta;
    pField[35] = &Data.T0T1AddLevelCr.HighStrength;
    pField[36] = &Data.SignalPreserveCb;
    pField[37] = &Data.SignalPreserveCr;
    Max = HiChromaFilterLowCombineParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiChromaLowCombine(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiChromaFilterMedianProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_CHROMA_FILTER_MED_s Data;
    void *pField[HiChromaFilterMedianParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiChromaMed(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.NoiseLevelCb;
    pField[2] = &Data.NoiseLevelCr;
    pField[3] = &Data.OriginalBlendStrengthCb;
    pField[4] = &Data.OriginalBlendStrengthCr;
    Max = HiChromaFilterMedianParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiChromaMed(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiChromaFilterMedianCombineProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_CHROMA_FLTR_MED_COM_s Data;
    void *pField[HiChromaFilterMedianCombineParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiChromaMedCombine(&Data);
    pField[0] = &Data.T0Cb;
    pField[1] = &Data.T0Cr;
    pField[2] = &Data.T1Cb;
    pField[3] = &Data.T1Cr;
    pField[4] = &Data.AlphaMaxCb;
    pField[5] = &Data.AlphaMaxCr;
    pField[6] = &Data.AlphaMinCb;
    pField[7] = &Data.AlphaMinCr;
    pField[8] = &Data.MaxChangeLevelCb.Low;
    pField[9] = &Data.MaxChangeLevelCb.LowDelta;
    pField[10] = &Data.MaxChangeLevelCb.LowStrength;
    pField[11] = &Data.MaxChangeLevelCb.MidStrength;
    pField[12] = &Data.MaxChangeLevelCb.High;
    pField[13] = &Data.MaxChangeLevelCb.HighDelta;
    pField[14] = &Data.MaxChangeLevelCb.HighStrength;
    pField[15] = &Data.MaxChangeLevelCr.Low;
    pField[16] = &Data.MaxChangeLevelCr.LowDelta;
    pField[17] = &Data.MaxChangeLevelCr.LowStrength;
    pField[18] = &Data.MaxChangeLevelCr.MidStrength;
    pField[19] = &Data.MaxChangeLevelCr.High;
    pField[20] = &Data.MaxChangeLevelCr.HighDelta;
    pField[21] = &Data.MaxChangeLevelCr.HighStrength;
    pField[22] = &Data.T0T1AddLevelCb.Low;
    pField[23] = &Data.T0T1AddLevelCb.LowDelta;
    pField[24] = &Data.T0T1AddLevelCb.LowStrength;
    pField[25] = &Data.T0T1AddLevelCb.MidStrength;
    pField[26] = &Data.T0T1AddLevelCb.High;
    pField[27] = &Data.T0T1AddLevelCb.HighDelta;
    pField[28] = &Data.T0T1AddLevelCb.HighStrength;
    pField[29] = &Data.T0T1AddLevelCr.Low;
    pField[30] = &Data.T0T1AddLevelCr.LowDelta;
    pField[31] = &Data.T0T1AddLevelCr.LowStrength;
    pField[32] = &Data.T0T1AddLevelCr.MidStrength;
    pField[33] = &Data.T0T1AddLevelCr.High;
    pField[34] = &Data.T0T1AddLevelCr.HighDelta;
    pField[35] = &Data.T0T1AddLevelCr.HighStrength;
    pField[36] = &Data.SignalPreserveCb;
    pField[37] = &Data.SignalPreserveCr;
    Max = HiChromaFilterMedianCombineParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiChromaMedCombine(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiChromaFilterPreProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_CHROMA_FILTER_PRE_s Data;
    void *pField[HiChromaFilterPreParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiChromaPre(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.NoiseLevelCb;
    pField[2] = &Data.NoiseLevelCr;
    pField[3] = &Data.OriginalBlendStrengthCb;
    pField[4] = &Data.OriginalBlendStrengthCr;
    Max = HiChromaFilterPreParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiChromaPre(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiChromaFilterVeryLowProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_CHROMA_FILTER_VERY_LOW_s Data;
    void *pField[HiChromaFilterVeryLowParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiChromaVeryLow(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.NoiseLevelCb;
    pField[2] = &Data.NoiseLevelCr;
    pField[3] = &Data.OriginalBlendStrengthCb;
    pField[4] = &Data.OriginalBlendStrengthCr;
    Max = HiChromaFilterVeryLowParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiChromaVeryLow(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiChromaFilterVeryLowCombineProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_CHROMA_FLTR_VERY_LOW_COM_s Data;
    void *pField[HiChromaFilterVeryLowCombineParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiChromaVeryLowCombine(&Data);
    pField[0] = &Data.T0Cb;
    pField[1] = &Data.T0Cr;
    pField[2] = &Data.T1Cb;
    pField[3] = &Data.T1Cr;
    pField[4] = &Data.AlphaMaxCb;
    pField[5] = &Data.AlphaMaxCr;
    pField[6] = &Data.AlphaMinCb;
    pField[7] = &Data.AlphaMinCr;
    pField[8] = &Data.MaxChangeLevelCb.Low;
    pField[9] = &Data.MaxChangeLevelCb.LowDelta;
    pField[10] = &Data.MaxChangeLevelCb.LowStrength;
    pField[11] = &Data.MaxChangeLevelCb.MidStrength;
    pField[12] = &Data.MaxChangeLevelCb.High;
    pField[13] = &Data.MaxChangeLevelCb.HighDelta;
    pField[14] = &Data.MaxChangeLevelCb.HighStrength;
    pField[15] = &Data.MaxChangeLevelCr.Low;
    pField[16] = &Data.MaxChangeLevelCr.LowDelta;
    pField[17] = &Data.MaxChangeLevelCr.LowStrength;
    pField[18] = &Data.MaxChangeLevelCr.MidStrength;
    pField[19] = &Data.MaxChangeLevelCr.High;
    pField[20] = &Data.MaxChangeLevelCr.HighDelta;
    pField[21] = &Data.MaxChangeLevelCr.HighStrength;
    pField[22] = &Data.T0T1AddLevelCb.Low;
    pField[23] = &Data.T0T1AddLevelCb.LowDelta;
    pField[24] = &Data.T0T1AddLevelCb.LowStrength;
    pField[25] = &Data.T0T1AddLevelCb.MidStrength;
    pField[26] = &Data.T0T1AddLevelCb.High;
    pField[27] = &Data.T0T1AddLevelCb.HighDelta;
    pField[28] = &Data.T0T1AddLevelCb.HighStrength;
    pField[29] = &Data.T0T1AddLevelCr.Low;
    pField[30] = &Data.T0T1AddLevelCr.LowDelta;
    pField[31] = &Data.T0T1AddLevelCr.LowStrength;
    pField[32] = &Data.T0T1AddLevelCr.MidStrength;
    pField[33] = &Data.T0T1AddLevelCr.High;
    pField[34] = &Data.T0T1AddLevelCr.HighDelta;
    pField[35] = &Data.T0T1AddLevelCr.HighStrength;
    pField[36] = &Data.SignalPreserveCb;
    pField[37] = &Data.SignalPreserveCr;
    Max = HiChromaFilterVeryLowCombineParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiChromaVeryLowCombine(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiChromaMedianFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_CHROMA_MEDIAN_FILTER_s Data;
    void *pField[HiChromaMedianFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiChromaMedianFilter(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.CbAdaptiveStrength;
    pField[2] = &Data.CrAdaptiveStrength;
    pField[3] = &Data.CbNonAdaptiveStrength;
    pField[4] = &Data.CrNonAdaptiveStrength;
    pField[5] = &Data.CbAdaptiveAmount;
    pField[6] = &Data.CrAdaptiveAmount;
    Max = HiChromaMedianFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiChromaMedianFilter(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiDemosaicFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_DEMOSAIC_s Data;
    void *pField[HiDemosaicFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiDemosaic(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.ActivityThresh;
    pField[2] = &Data.ActivityDifferenceThresh;
    pField[3] = &Data.GradClipThresh;
    pField[4] = &Data.GradNoiseThresh;
    pField[5] = &Data.AliasInterpolationStrength;
    pField[6] = &Data.AliasInterpolationThresh;
    Max = HiDemosaicFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiDemosaic(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiGbGrMismatchProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_GRGB_MISMATCH_s Data;
    void *pField[HiGbGrMismatchParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiGrGbMismatch(&Data);
    pField[0] = &Data.NarrowEnable;
    pField[1] = &Data.WideEnable;
    pField[2] = &Data.WideSafety;
    pField[3] = &Data.WideThresh;
    Max = HiGbGrMismatchParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiGrGbMismatch(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiHigh2LumaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_HIGH2_ASF_s Data;
    void *pField[HiHigh2LumaAdvanceSpatialFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiHigh2Asf(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Fir.Specify;
    pField[2] = &Data.Fir.PerDirFirIsoStrengths;
    pField[3] = &Data.Fir.PerDirFirDirStrengths;
    pField[4] = &Data.Fir.PerDirFirDirAmounts;
    pField[5] = &Data.Fir.Coefs;
    pField[6] = &Data.Fir.StrengthIso;
    pField[7] = &Data.Fir.StrengthDir;
    pField[8] = &Data.Fir.WideEdgeDetect;
    pField[9] = &Data.DirectionalDecideT0;
    pField[10] = &Data.DirectionalDecideT1;
    pField[11] = &Data.Adapt.AlphaMinUp;
    pField[12] = &Data.Adapt.AlphaMaxUp;
    pField[13] = &Data.Adapt.T0Up;
    pField[14] = &Data.Adapt.T1Up;
    pField[15] = &Data.Adapt.AlphaMinDown;
    pField[16] = &Data.Adapt.AlphaMaxDown;
    pField[17] = &Data.Adapt.T0Down;
    pField[18] = &Data.Adapt.T1Down;
    pField[19] = &Data.LevelStrAdjust.Low;
    pField[20] = &Data.LevelStrAdjust.LowDelta;
    pField[21] = &Data.LevelStrAdjust.LowStrength;
    pField[22] = &Data.LevelStrAdjust.MidStrength;
    pField[23] = &Data.LevelStrAdjust.High;
    pField[24] = &Data.LevelStrAdjust.HighDelta;
    pField[25] = &Data.LevelStrAdjust.HighStrength;
    pField[26] = &Data.LevelStrAdjust.Method;
    pField[27] = &Data.T0T1Div.Low;
    pField[28] = &Data.T0T1Div.LowDelta;
    pField[29] = &Data.T0T1Div.LowStrength;
    pField[30] = &Data.T0T1Div.MidStrength;
    pField[31] = &Data.T0T1Div.High;
    pField[32] = &Data.T0T1Div.HighDelta;
    pField[33] = &Data.T0T1Div.HighStrength;
    pField[34] = &Data.T0T1Div.Method;
    pField[35] = &Data.MaxChangeNotT0T1Alpha;
    pField[36] = &Data.MaxChangeUp;
    pField[37] = &Data.MaxChangeDown;
    Max = HiHigh2LumaAdvanceSpatialFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiHigh2Asf(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiHighLumaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_HIGH_ASF_s Data;
    void *pField[HiHighLumaAdvanceSpatialFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiHighAsf(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Fir.Specify;
    pField[2] = &Data.Fir.PerDirFirIsoStrengths;
    pField[3] = &Data.Fir.PerDirFirDirStrengths;
    pField[4] = &Data.Fir.PerDirFirDirAmounts;
    pField[5] = &Data.Fir.Coefs;
    pField[6] = &Data.Fir.StrengthIso;
    pField[7] = &Data.Fir.StrengthDir;
    pField[8] = &Data.Fir.WideEdgeDetect;
    pField[9] = &Data.DirectionalDecideT0;
    pField[10] = &Data.DirectionalDecideT1;
    pField[11] = &Data.Adapt.AlphaMinUp;
    pField[12] = &Data.Adapt.AlphaMaxUp;
    pField[13] = &Data.Adapt.T0Up;
    pField[14] = &Data.Adapt.T1Up;
    pField[15] = &Data.Adapt.AlphaMinDown;
    pField[16] = &Data.Adapt.AlphaMaxDown;
    pField[17] = &Data.Adapt.T0Down;
    pField[18] = &Data.Adapt.T1Down;
    pField[19] = &Data.LevelStrAdjust.Low;
    pField[20] = &Data.LevelStrAdjust.LowDelta;
    pField[21] = &Data.LevelStrAdjust.LowStrength;
    pField[22] = &Data.LevelStrAdjust.MidStrength;
    pField[23] = &Data.LevelStrAdjust.High;
    pField[24] = &Data.LevelStrAdjust.HighDelta;
    pField[25] = &Data.LevelStrAdjust.HighStrength;
    pField[26] = &Data.LevelStrAdjust.Method;
    pField[27] = &Data.T0T1Div.Low;
    pField[28] = &Data.T0T1Div.LowDelta;
    pField[29] = &Data.T0T1Div.LowStrength;
    pField[30] = &Data.T0T1Div.MidStrength;
    pField[31] = &Data.T0T1Div.High;
    pField[32] = &Data.T0T1Div.HighDelta;
    pField[33] = &Data.T0T1Div.HighStrength;
    pField[34] = &Data.T0T1Div.Method;
    pField[35] = &Data.MaxChangeUp;
    pField[36] = &Data.MaxChangeDown;
    Max = HiHighLumaAdvanceSpatialFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiHighAsf(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiHighSharpenBothProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_HIGH_SHPNS_BOTH_s Data;
    void *pField[HiHighSharpeningBothParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiHighShpnsBoth(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Mode;
    pField[2] = &Data.EdgeThresh;
    pField[3] = &Data.WideEdgeDetect;
    pField[4] = &Data.MaxChangeUp5x5;
    pField[5] = &Data.MaxChangeDown5x5;
    Max = HiHighSharpeningBothParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiHighShpnsBoth(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiHighSharpenNoiseProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_HIGH_SHPNS_NOISE_s Data;
    void *pField[HiHighSharpeningNoiseParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiHighShpnsNoise(&Data);
    pField[0] = &Data.MaxChangeUp;
    pField[1] = &Data.MaxChangeDown;
    pField[2] = &Data.SpatialFir.Specify;
    pField[3] = &Data.SpatialFir.PerDirFirIsoStrengths;
    pField[4] = &Data.SpatialFir.PerDirFirDirStrengths;
    pField[5] = &Data.SpatialFir.PerDirFirDirAmounts;
    pField[6] = &Data.SpatialFir.Coefs;
    pField[7] = &Data.SpatialFir.StrengthIso;
    pField[8] = &Data.SpatialFir.StrengthDir;
    pField[9] = &Data.LevelStrAdjust.Low;
    pField[10] = &Data.LevelStrAdjust.LowDelta;
    pField[11] = &Data.LevelStrAdjust.LowStrength;
    pField[12] = &Data.LevelStrAdjust.MidStrength;
    pField[13] = &Data.LevelStrAdjust.High;
    pField[14] = &Data.LevelStrAdjust.HighDelta;
    pField[15] = &Data.LevelStrAdjust.HighStrength;
    pField[16] = &Data.LevelStrAdjust.Method;
    pField[17] = &Data.LevelStrAdjustNotT0T1LevelBased;
    pField[18] = &Data.T0;
    pField[19] = &Data.T1;
    pField[20] = &Data.AlphaMin;
    pField[21] = &Data.AlphaMax;
    Max = HiHighSharpeningNoiseParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiHighShpnsNoise(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiHighSharpenFirProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_HIGH_SHPNS_FIR_s Data;
    void *pField[HiHighFirstSharpeningFirParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiHighShpnsFir(&Data);
    pField[0] = &Data.Specify;
    pField[1] = &Data.PerDirFirIsoStrengths;
    pField[2] = &Data.PerDirFirDirStrengths;
    pField[3] = &Data.PerDirFirDirAmounts;
    pField[4] = &Data.Coefs;
    pField[5] = &Data.StrengthIso;
    pField[6] = &Data.StrengthDir;
    Max = HiHighFirstSharpeningFirParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiHighShpnsFir(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiHighSharpenCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_HIGH_SHPNS_CORING_s Data;
    void *pField[HiHighSharpeningCoringParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiHighShpnsCoring(&Data);
    pField[0] = &Data.Coring;
    pField[1] = &Data.FractionalBits;
    Max = HiHighSharpeningCoringParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiHighShpnsCoring(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiHighCoringIndexScaleProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_HIGH_SHPNS_COR_IDX_SCL_s Data;
    void *pField[HiHighSharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiHighShpnsCorIdxScl(&Data);
    pField[0] = &Data.Low;
    pField[1] = &Data.LowDelta;
    pField[2] = &Data.LowStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.High;
    pField[5] = &Data.HighDelta;
    pField[6] = &Data.HighStrength;
    pField[7] = &Data.Method;
    Max = HiHighSharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiHighShpnsCorIdxScl(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiHighMaxCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_HIGH_SHPNS_MAX_COR_RST_s Data;
    void *pField[HiHighSharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiHighShpnsMaxCorRst(&Data);
    pField[0] = &Data.Low;
    pField[1] = &Data.LowDelta;
    pField[2] = &Data.LowStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.High;
    pField[5] = &Data.HighDelta;
    pField[6] = &Data.HighStrength;
    pField[7] = &Data.Method;
    Max = HiHighSharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiHighShpnsMaxCorRst(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiHighMinCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_HIGH_SHPNS_MIN_COR_RST_s Data;
    void *pField[HiHighSharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiHighShpnsMinCorRst(&Data);
    pField[0] = &Data.Low;
    pField[1] = &Data.LowDelta;
    pField[2] = &Data.LowStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.High;
    pField[5] = &Data.HighDelta;
    pField[6] = &Data.HighStrength;
    pField[7] = &Data.Method;
    Max = HiHighSharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiHighShpnsMinCorRst(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiHighScaleCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_HIGH_SHPNS_SCL_COR_s Data;
    void *pField[HiHighSharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiHighShpnsSclCor(&Data);
    pField[0] = &Data.Low;
    pField[1] = &Data.LowDelta;
    pField[2] = &Data.LowStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.High;
    pField[5] = &Data.HighDelta;
    pField[6] = &Data.HighStrength;
    pField[7] = &Data.Method;
    Max = HiHighSharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiHighShpnsSclCor(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiLowLumaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_LOW_ASF_s Data;
    void *pField[HiLowLumaAdvanceSpatialFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiLowAsf(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Fir.Specify;
    pField[2] = &Data.Fir.PerDirFirIsoStrengths;
    pField[3] = &Data.Fir.PerDirFirDirStrengths;
    pField[4] = &Data.Fir.PerDirFirDirAmounts;
    pField[5] = &Data.Fir.Coefs;
    pField[6] = &Data.Fir.StrengthIso;
    pField[7] = &Data.Fir.StrengthDir;
    pField[8] = &Data.Fir.WideEdgeDetect;
    pField[9] = &Data.DirectionalDecideT0;
    pField[10] = &Data.DirectionalDecideT1;
    pField[11] = &Data.Adapt.AlphaMinUp;
    pField[12] = &Data.Adapt.AlphaMaxUp;
    pField[13] = &Data.Adapt.T0Up;
    pField[14] = &Data.Adapt.T1Up;
    pField[15] = &Data.Adapt.AlphaMinDown;
    pField[16] = &Data.Adapt.AlphaMaxDown;
    pField[17] = &Data.Adapt.T0Down;
    pField[18] = &Data.Adapt.T1Down;
    pField[19] = &Data.LevelStrAdjust.Low;
    pField[20] = &Data.LevelStrAdjust.LowDelta;
    pField[21] = &Data.LevelStrAdjust.LowStrength;
    pField[22] = &Data.LevelStrAdjust.MidStrength;
    pField[23] = &Data.LevelStrAdjust.High;
    pField[24] = &Data.LevelStrAdjust.HighDelta;
    pField[25] = &Data.LevelStrAdjust.HighStrength;
    pField[26] = &Data.T0T1Div.Low;
    pField[27] = &Data.T0T1Div.LowDelta;
    pField[28] = &Data.T0T1Div.LowStrength;
    pField[29] = &Data.T0T1Div.MidStrength;
    pField[30] = &Data.T0T1Div.High;
    pField[31] = &Data.T0T1Div.HighDelta;
    pField[32] = &Data.T0T1Div.HighStrength;
    pField[33] = &Data.MaxChangeUp;
    pField[34] = &Data.MaxChangeDown;
    Max = HiLowLumaAdvanceSpatialFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiLowAsf(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiLowLumaASFCombineProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_LOW_ASF_COMBINE_s Data;
    void *pField[HiLowLumaAdvanceSpatialFilterCombineParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiLowAsfCombine(&Data);
    pField[0] = &Data.T0;
    pField[1] = &Data.T1;
    pField[2] = &Data.AlphaMax;
    pField[3] = &Data.AlphaMin;
    pField[4] = &Data.SignalPreserve;
    pField[5] = &Data.T0T1Add.Low;
    pField[6] = &Data.T0T1Add.LowDelta;
    pField[7] = &Data.T0T1Add.LowStrength;
    pField[8] = &Data.T0T1Add.MidStrength;
    pField[9] = &Data.T0T1Add.High;
    pField[10] = &Data.T0T1Add.HighDelta;
    pField[11] = &Data.T0T1Add.HighStrength;
    pField[12] = &Data.T0T1Add.Method;
    pField[13] = &Data.MaxChange.Low;
    pField[14] = &Data.MaxChange.LowDelta;
    pField[15] = &Data.MaxChange.LowStrength;
    pField[16] = &Data.MaxChange.MidStrength;
    pField[17] = &Data.MaxChange.High;
    pField[18] = &Data.MaxChange.HighDelta;
    pField[19] = &Data.MaxChange.HighStrength;
    pField[20] = &Data.MaxChange.Method;
    Max = HiLowLumaAdvanceSpatialFilterCombineParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiLowAsfCombine(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiLumaNoiseCombineProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_LUMA_COMBINE_s Data;
    void *pField[HiLumaNoiseCombineParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiLumaCombine(&Data);
    pField[0] = &Data.T0;
    pField[1] = &Data.T1;
    pField[2] = &Data.AlphaMax;
    pField[3] = &Data.AlphaMin;
    pField[4] = &Data.SignalPreserve;
    pField[5] = &Data.T0T1Add.Low;
    pField[6] = &Data.T0T1Add.LowDelta;
    pField[7] = &Data.T0T1Add.LowStrength;
    pField[8] = &Data.T0T1Add.MidStrength;
    pField[9] = &Data.T0T1Add.High;
    pField[10] = &Data.T0T1Add.HighDelta;
    pField[11] = &Data.T0T1Add.HighStrength;
    pField[12] = &Data.T0T1Add.Method;
    pField[13] = &Data.MaxChange.Low;
    pField[14] = &Data.MaxChange.LowDelta;
    pField[15] = &Data.MaxChange.LowStrength;
    pField[16] = &Data.MaxChange.MidStrength;
    pField[17] = &Data.MaxChange.High;
    pField[18] = &Data.MaxChange.HighDelta;
    pField[19] = &Data.MaxChange.HighStrength;
    pField[20] = &Data.MaxChange.Method;
    Max = HiLumaNoiseCombineParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiLumaCombine(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiMedian1LumaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_MED1_ASF_s Data;
    void *pField[HiMedian1LumaAdvanceSpatialFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiMed1Asf(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Fir.Specify;
    pField[2] = &Data.Fir.PerDirFirIsoStrengths;
    pField[3] = &Data.Fir.PerDirFirDirStrengths;
    pField[4] = &Data.Fir.PerDirFirDirAmounts;
    pField[5] = &Data.Fir.Coefs;
    pField[6] = &Data.Fir.StrengthIso;
    pField[7] = &Data.Fir.StrengthDir;
    pField[8] = &Data.Fir.WideEdgeDetect;
    pField[9] = &Data.DirectionalDecideT0;
    pField[10] = &Data.DirectionalDecideT1;
    pField[11] = &Data.Adapt.AlphaMinUp;
    pField[12] = &Data.Adapt.AlphaMaxUp;
    pField[13] = &Data.Adapt.T0Up;
    pField[14] = &Data.Adapt.T1Up;
    pField[15] = &Data.Adapt.AlphaMinDown;
    pField[16] = &Data.Adapt.AlphaMaxDown;
    pField[17] = &Data.Adapt.T0Down;
    pField[18] = &Data.Adapt.T1Down;
    pField[19] = &Data.LevelStrAdjust.Low;
    pField[20] = &Data.LevelStrAdjust.LowDelta;
    pField[21] = &Data.LevelStrAdjust.LowStrength;
    pField[22] = &Data.LevelStrAdjust.MidStrength;
    pField[23] = &Data.LevelStrAdjust.High;
    pField[24] = &Data.LevelStrAdjust.HighDelta;
    pField[25] = &Data.LevelStrAdjust.HighStrength;
    pField[26] = &Data.LevelStrAdjust.Method;
    pField[27] = &Data.T0T1Div.Low;
    pField[28] = &Data.T0T1Div.LowDelta;
    pField[29] = &Data.T0T1Div.LowStrength;
    pField[30] = &Data.T0T1Div.MidStrength;
    pField[31] = &Data.T0T1Div.High;
    pField[32] = &Data.T0T1Div.HighDelta;
    pField[33] = &Data.T0T1Div.HighStrength;
    pField[34] = &Data.T0T1Div.Method;
    pField[35] = &Data.MaxChangeNotT0T1Alpha;
    pField[36] = &Data.MaxChangeUp;
    pField[37] = &Data.MaxChangeDown;
    Max = HiMedian1LumaAdvanceSpatialFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiMed1Asf(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiMedian2LumaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_MED2_ASF_s Data;
    void *pField[HiMedian2LumaAdvanceSpatialFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiMed2Asf(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Fir.Specify;
    pField[2] = &Data.Fir.PerDirFirIsoStrengths;
    pField[3] = &Data.Fir.PerDirFirDirStrengths;
    pField[4] = &Data.Fir.PerDirFirDirAmounts;
    pField[5] = &Data.Fir.Coefs;
    pField[6] = &Data.Fir.StrengthIso;
    pField[7] = &Data.Fir.StrengthDir;
    pField[8] = &Data.Fir.WideEdgeDetect;
    pField[9] = &Data.DirectionalDecideT0;
    pField[10] = &Data.DirectionalDecideT1;
    pField[11] = &Data.Adapt.AlphaMinUp;
    pField[12] = &Data.Adapt.AlphaMaxUp;
    pField[13] = &Data.Adapt.T0Up;
    pField[14] = &Data.Adapt.T1Up;
    pField[15] = &Data.Adapt.AlphaMinDown;
    pField[16] = &Data.Adapt.AlphaMaxDown;
    pField[17] = &Data.Adapt.T0Down;
    pField[18] = &Data.Adapt.T1Down;
    pField[19] = &Data.LevelStrAdjust.Low;
    pField[20] = &Data.LevelStrAdjust.LowDelta;
    pField[21] = &Data.LevelStrAdjust.LowStrength;
    pField[22] = &Data.LevelStrAdjust.MidStrength;
    pField[23] = &Data.LevelStrAdjust.High;
    pField[24] = &Data.LevelStrAdjust.HighDelta;
    pField[25] = &Data.LevelStrAdjust.HighStrength;
    pField[26] = &Data.LevelStrAdjust.Method;
    pField[27] = &Data.T0T1Div.Low;
    pField[28] = &Data.T0T1Div.LowDelta;
    pField[29] = &Data.T0T1Div.LowStrength;
    pField[30] = &Data.T0T1Div.MidStrength;
    pField[31] = &Data.T0T1Div.High;
    pField[32] = &Data.T0T1Div.HighDelta;
    pField[33] = &Data.T0T1Div.HighStrength;
    pField[34] = &Data.T0T1Div.Method;
    pField[35] = &Data.MaxChangeUp;
    pField[36] = &Data.MaxChangeDown;
    Max = HiMedian2LumaAdvanceSpatialFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiMed2Asf(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiMedianSharpenBothProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_MED_SHPNS_BOTH_s Data;
    void *pField[HiMedianSharpeningBothParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiMedShpnsBoth(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Mode;
    pField[2] = &Data.EdgeThresh;
    pField[3] = &Data.WideEdgeDetect;
    pField[4] = &Data.MaxChangeUp5x5;
    pField[5] = &Data.MaxChangeDown5x5;
    Max = HiMedianSharpeningBothParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiMedShpnsBoth(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiMedianSharpenNoiseProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_MED_SHPNS_NOISE_s Data;
    void *pField[HiMedianSharpeningNoiseParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiMedShpnsNoise(&Data);
    pField[0] = &Data.MaxChangeUp;
    pField[1] = &Data.MaxChangeDown;
    pField[2] = &Data.SpatialFir.Specify;
    pField[3] = &Data.SpatialFir.PerDirFirIsoStrengths;
    pField[4] = &Data.SpatialFir.PerDirFirDirStrengths;
    pField[5] = &Data.SpatialFir.PerDirFirDirAmounts;
    pField[6] = &Data.SpatialFir.Coefs;
    pField[7] = &Data.SpatialFir.StrengthIso;
    pField[8] = &Data.SpatialFir.StrengthDir;
    pField[9] = &Data.LevelStrAdjust.Low;
    pField[10] = &Data.LevelStrAdjust.LowDelta;
    pField[11] = &Data.LevelStrAdjust.LowStrength;
    pField[12] = &Data.LevelStrAdjust.MidStrength;
    pField[13] = &Data.LevelStrAdjust.High;
    pField[14] = &Data.LevelStrAdjust.HighDelta;
    pField[15] = &Data.LevelStrAdjust.HighStrength;
    pField[16] = &Data.LevelStrAdjust.Method;
    pField[17] = &Data.LevelStrAdjustNotT0T1LevelBased;
    pField[18] = &Data.T0;
    pField[19] = &Data.T1;
    pField[20] = &Data.AlphaMin;
    pField[21] = &Data.AlphaMax;
    Max = HiMedianSharpeningNoiseParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiMedShpnsNoise(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiMedianSharpenFirProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_MED_SHPNS_FIR_s Data;
    void *pField[HiMedianFirstSharpeningFirParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiMedShpnsFir(&Data);
    pField[0] = &Data.Specify;
    pField[1] = &Data.PerDirFirIsoStrengths;
    pField[2] = &Data.PerDirFirDirStrengths;
    pField[3] = &Data.PerDirFirDirAmounts;
    pField[4] = &Data.Coefs;
    pField[5] = &Data.StrengthIso;
    pField[6] = &Data.StrengthDir;
    Max = HiMedianFirstSharpeningFirParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiMedShpnsFir(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiMedianSharpenCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_MED_SHPNS_CORING_s Data;
    void *pField[HiMedianSharpeningCoringParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiMedShpnsCoring(&Data);
    pField[0] = &Data.Coring;
    pField[1] = &Data.FractionalBits;
    Max = HiMedianSharpeningCoringParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiMedShpnsCoring(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiMedianCoringIndexScaleProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_MED_SHPNS_COR_IDX_SCL_s Data;
    void *pField[HiMedianSharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiMedShpnsCorIdxScl(&Data);
    pField[0] = &Data.Low;
    pField[1] = &Data.LowDelta;
    pField[2] = &Data.LowStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.High;
    pField[5] = &Data.HighDelta;
    pField[6] = &Data.HighStrength;
    pField[7] = &Data.Method;
    Max = HiMedianSharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiMedShpnsCorIdxScl(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiMedianMaxCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_MED_SHPNS_MAX_COR_RST_s Data;
    void *pField[HiMedianSharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiMedShpnsMaxCorRst(&Data);
    pField[0] = &Data.Low;
    pField[1] = &Data.LowDelta;
    pField[2] = &Data.LowStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.High;
    pField[5] = &Data.HighDelta;
    pField[6] = &Data.HighStrength;
    pField[7] = &Data.Method;
    Max = HiMedianSharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiMedShpnsMaxCorRst(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiMedianMinCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_MED_SHPNS_MIN_COR_RST_s Data;
    void *pField[HiMedianSharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiMedShpnsMinCorRst(&Data);
    pField[0] = &Data.Low;
    pField[1] = &Data.LowDelta;
    pField[2] = &Data.LowStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.High;
    pField[5] = &Data.HighDelta;
    pField[6] = &Data.HighStrength;
    pField[7] = &Data.Method;
    Max = HiMedianSharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiMedShpnsMinCorRst(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiMedianScaleCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_MED_SHPNS_SCL_COR_s Data;
    void *pField[HiMedianSharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiMedShpnsSclCor(&Data);
    pField[0] = &Data.Low;
    pField[1] = &Data.LowDelta;
    pField[2] = &Data.LowStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.High;
    pField[5] = &Data.HighDelta;
    pField[6] = &Data.HighStrength;
    pField[7] = &Data.Method;
    Max = HiMedianSharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiMedShpnsSclCor(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiSelectProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_SELECT_s Data;
    void *pField[HiSelectParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiSelect(&Data);
    pField[0] = &Data.UseSharpenNotAsf;
    Max = HiSelectParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiSelect(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiLumaBlendProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_LUMA_BLEND_s Data;
    void *pField[HiLumaBlendParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiLumaBlend(&Data);
    pField[0] = &Data.LumaLevel.Low;
    pField[1] = &Data.LumaLevel.LowDelta;
    pField[2] = &Data.LumaLevel.LowStrength;
    pField[3] = &Data.LumaLevel.MidStrength;
    pField[4] = &Data.LumaLevel.High;
    pField[5] = &Data.LumaLevel.HighDelta;
    pField[6] = &Data.LumaLevel.HighStrength;
    pField[7] = &Data.LumaLevel.Method;
    Max = HiLumaBlendParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiLumaBlend(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiLiCombineProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HILI_COMBINE_s Data;
    void *pField[HiLiCombineParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiLiCombine(&Data);
    pField[0] = &Data.T0Cb;
    pField[1] = &Data.T0Cr;
    pField[2] = &Data.T0Y;
    pField[3] = &Data.T1Cb;
    pField[4] = &Data.T1Cr;
    pField[5] = &Data.T1Y;
    pField[6] = &Data.AlphaMaxY;
    pField[7] = &Data.AlphaMaxCb;
    pField[8] = &Data.AlphaMaxCr;
    pField[9] = &Data.AlphaMinY;
    pField[10] = &Data.AlphaMinCb;
    pField[11] = &Data.AlphaMinCr;
    pField[12] = &Data.MaxChangeLevelY.Low;
    pField[13] = &Data.MaxChangeLevelY.LowDelta;
    pField[14] = &Data.MaxChangeLevelY.LowStrength;
    pField[15] = &Data.MaxChangeLevelY.MidStrength;
    pField[16] = &Data.MaxChangeLevelY.High;
    pField[17] = &Data.MaxChangeLevelY.HighDelta;
    pField[18] = &Data.MaxChangeLevelY.HighStrength;
    pField[19] = &Data.MaxChangeLevelY.Method;
    pField[20] = &Data.MaxChangeLevelCb.Low;
    pField[21] = &Data.MaxChangeLevelCb.LowDelta;
    pField[22] = &Data.MaxChangeLevelCb.LowStrength;
    pField[23] = &Data.MaxChangeLevelCb.MidStrength;
    pField[24] = &Data.MaxChangeLevelCb.High;
    pField[25] = &Data.MaxChangeLevelCb.HighDelta;
    pField[26] = &Data.MaxChangeLevelCb.HighStrength;
    pField[27] = &Data.MaxChangeLevelCr.Low;
    pField[28] = &Data.MaxChangeLevelCr.LowDelta;
    pField[29] = &Data.MaxChangeLevelCr.LowStrength;
    pField[30] = &Data.MaxChangeLevelCr.MidStrength;
    pField[31] = &Data.MaxChangeLevelCr.High;
    pField[32] = &Data.MaxChangeLevelCr.HighDelta;
    pField[33] = &Data.MaxChangeLevelCr.HighStrength;
    pField[34] = &Data.T0T1AddLevelY.Low;
    pField[35] = &Data.T0T1AddLevelY.LowDelta;
    pField[36] = &Data.T0T1AddLevelY.LowStrength;
    pField[37] = &Data.T0T1AddLevelY.MidStrength;
    pField[38] = &Data.T0T1AddLevelY.High;
    pField[39] = &Data.T0T1AddLevelY.HighDelta;
    pField[40] = &Data.T0T1AddLevelY.HighStrength;
    pField[41] = &Data.T0T1AddLevelY.Method;
    pField[42] = &Data.T0T1AddLevelCb.Low;
    pField[43] = &Data.T0T1AddLevelCb.LowDelta;
    pField[44] = &Data.T0T1AddLevelCb.LowStrength;
    pField[45] = &Data.T0T1AddLevelCb.MidStrength;
    pField[46] = &Data.T0T1AddLevelCb.High;
    pField[47] = &Data.T0T1AddLevelCb.HighDelta;
    pField[48] = &Data.T0T1AddLevelCb.HighStrength;
    pField[49] = &Data.T0T1AddLevelCr.Low;
    pField[50] = &Data.T0T1AddLevelCr.LowDelta;
    pField[51] = &Data.T0T1AddLevelCr.LowStrength;
    pField[52] = &Data.T0T1AddLevelCr.MidStrength;
    pField[53] = &Data.T0T1AddLevelCr.High;
    pField[54] = &Data.T0T1AddLevelCr.HighDelta;
    pField[55] = &Data.T0T1AddLevelCr.HighStrength;
    pField[56] = &Data.SignalPreserveCb;
    pField[57] = &Data.SignalPreserveCr;
    pField[58] = &Data.SignalPreserveY;
    pField[59] = &Data.MinTarLevelY.Low;
    pField[60] = &Data.MinTarLevelY.LowDelta;
    pField[61] = &Data.MinTarLevelY.LowStrength;
    pField[62] = &Data.MinTarLevelY.MidStrength;
    pField[63] = &Data.MinTarLevelY.High;
    pField[64] = &Data.MinTarLevelY.HighDelta;
    pField[65] = &Data.MinTarLevelY.HighStrength;
    pField[66] = &Data.MinTarLevelY.Method;

    Max = HiLiCombineParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiLiCombine(&Data);
    ParamStatus[0] = SKIP_VALID_CHECK;
    ParamStatus[1] = SKIP_VALID_CHECK;
    ParamStatus[2] = SKIP_VALID_CHECK;
    ParamStatus[3] = SKIP_VALID_CHECK;
    ParamStatus[4] = SKIP_VALID_CHECK;

}

static void HiLiSharpenBothProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HILI_SHPNS_BOTH_s Data;
    void *pField[HiLiSharpeningBothParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiLiShpnsBoth(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Mode;
    pField[2] = &Data.EdgeThresh;
    pField[3] = &Data.WideEdgeDetect;
    pField[4] = &Data.MaxChangeUp5x5;
    pField[5] = &Data.MaxChangeDown5x5;
    pField[6] = &Data.MaxChangeUp;
    pField[7] = &Data.MaxChangeDown;
    Max = HiLiSharpeningBothParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiLiShpnsBoth(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiLiSharpenNoiseProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HILI_SHPNS_NOISE_s Data;
    void *pField[HiLiSharpeningNoiseParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiLiShpnsNoise(&Data);
    pField[0] = &Data.MaxChangeUp;
    pField[1] = &Data.MaxChangeDown;
    pField[2] = &Data.SpatialFir.Specify;
    pField[3] = &Data.SpatialFir.PerDirFirIsoStrengths;
    pField[4] = &Data.SpatialFir.PerDirFirDirStrengths;
    pField[5] = &Data.SpatialFir.PerDirFirDirAmounts;
    pField[6] = &Data.SpatialFir.Coefs;
    pField[7] = &Data.SpatialFir.StrengthIso;
    pField[8] = &Data.SpatialFir.StrengthDir;
    pField[9] = &Data.LevelStrAdjust.Low;
    pField[10] = &Data.LevelStrAdjust.LowDelta;
    pField[11] = &Data.LevelStrAdjust.LowStrength;
    pField[12] = &Data.LevelStrAdjust.MidStrength;
    pField[13] = &Data.LevelStrAdjust.High;
    pField[14] = &Data.LevelStrAdjust.HighDelta;
    pField[15] = &Data.LevelStrAdjust.HighStrength;
    pField[16] = &Data.LevelStrAdjust.Method;
    pField[17] = &Data.LevelStrAdjustNotT0T1LevelBased;
    pField[18] = &Data.T0;
    pField[19] = &Data.T1;
    pField[20] = &Data.AlphaMin;
    pField[21] = &Data.AlphaMax;
    Max = HiLiSharpeningNoiseParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiLiShpnsNoise(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiLiSharpenFirProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HILI_SHPNS_FIR_s Data;
    void *pField[HiLiFirstSharpeningFirParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiLiShpnsFir(&Data);
    pField[0] = &Data.Specify;
    pField[1] = &Data.PerDirFirIsoStrengths;
    pField[2] = &Data.PerDirFirDirStrengths;
    pField[3] = &Data.PerDirFirDirAmounts;
    pField[4] = &Data.Coefs;
    pField[5] = &Data.StrengthIso;
    pField[6] = &Data.StrengthDir;
    Max = HiLiFirstSharpeningFirParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiLiShpnsFir(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiLiSharpenCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HILI_SHPNS_CORING_s Data;
    void *pField[HiLiSharpeningCoringParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiLiShpnsCoring(&Data);
    pField[0] = &Data.Coring;
    Max = HiLiSharpeningCoringParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiLiShpnsCoring(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiLiCoringIndexScaleProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HILI_SHPNS_COR_IDX_SCL_s Data;
    void *pField[HiLiSharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiLiShpnsCorIdxScl(&Data);
    pField[0] = &Data.Low;
    pField[1] = &Data.LowDelta;
    pField[2] = &Data.LowStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.High;
    pField[5] = &Data.HighDelta;
    pField[6] = &Data.HighStrength;
    pField[7] = &Data.Method;
    Max = HiLiSharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiLiShpnsCorIdxScl(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiLiMaxCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HILI_SHPNS_MAX_COR_RST_s Data;
    void *pField[HiLiSharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiLiShpnsMaxCorRst(&Data);
    pField[0] = &Data.Low;
    pField[1] = &Data.LowDelta;
    pField[2] = &Data.LowStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.High;
    pField[5] = &Data.HighDelta;
    pField[6] = &Data.HighStrength;
    pField[7] = &Data.Method;
    Max = HiLiSharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiLiShpnsMaxCorRst(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiLiMinCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HILI_SHPNS_MIN_COR_RST_s Data;
    void *pField[HiLiSharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiLiShpnsMinCorRst(&Data);
    pField[0] = &Data.Low;
    pField[1] = &Data.LowDelta;
    pField[2] = &Data.LowStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.High;
    pField[5] = &Data.HighDelta;
    pField[6] = &Data.HighStrength;
    pField[7] = &Data.Method;
    Max = HiLiSharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiLiShpnsMinCorRst(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiLiScaleCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HILI_SHPNS_SCL_COR_s Data;
    void *pField[HiLiSharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiLiShpnsSclCor(&Data);
    pField[0] = &Data.Low;
    pField[1] = &Data.LowDelta;
    pField[2] = &Data.LowStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.High;
    pField[5] = &Data.HighDelta;
    pField[6] = &Data.HighStrength;
    pField[7] = &Data.Method;
    Max = HiLiSharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiLiShpnsSclCor(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void Li2LumaASFProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_ASF_s Data;
    void *pField[Li2LumaAdvanceSpatialFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Li2Asf(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Fir.Specify;
    pField[2] = &Data.Fir.PerDirFirIsoStrengths;
    pField[3] = &Data.Fir.PerDirFirDirStrengths;
    pField[4] = &Data.Fir.PerDirFirDirAmounts;
    pField[5] = &Data.Fir.Coefs;
    pField[6] = &Data.Fir.StrengthIso;
    pField[7] = &Data.Fir.StrengthDir;
    pField[8] = &Data.Fir.WideEdgeDetect;
    pField[9] = &Data.DirectionalDecideT0;
    pField[10] = &Data.DirectionalDecideT1;
    pField[11] = &Data.Adapt.AlphaMinUp;
    pField[12] = &Data.Adapt.AlphaMaxUp;
    pField[13] = &Data.Adapt.T0Up;
    pField[14] = &Data.Adapt.T1Up;
    pField[15] = &Data.Adapt.AlphaMinDown;
    pField[16] = &Data.Adapt.AlphaMaxDown;
    pField[17] = &Data.Adapt.T0Down;
    pField[18] = &Data.Adapt.T1Down;
    pField[19] = &Data.LevelStrAdjust.Low;
    pField[20] = &Data.LevelStrAdjust.LowDelta;
    pField[21] = &Data.LevelStrAdjust.LowStrength;
    pField[22] = &Data.LevelStrAdjust.MidStrength;
    pField[23] = &Data.LevelStrAdjust.High;
    pField[24] = &Data.LevelStrAdjust.HighDelta;
    pField[25] = &Data.LevelStrAdjust.HighStrength;
    pField[26] = &Data.T0T1Div.Low;
    pField[27] = &Data.T0T1Div.LowDelta;
    pField[28] = &Data.T0T1Div.LowStrength;
    pField[29] = &Data.T0T1Div.MidStrength;
    pField[30] = &Data.T0T1Div.High;
    pField[31] = &Data.T0T1Div.HighDelta;
    pField[32] = &Data.T0T1Div.HighStrength;
    pField[33] = &Data.MaxChangeNotT0T1Alpha;
    pField[34] = &Data.MaxChangeUp;
    pField[35] = &Data.MaxChangeDown;
    Max = Li2LumaAdvanceSpatialFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Li2Asf(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void Li2AntiAliasingProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_ANTI_ALIASING_s Data;
    void *pField[Li2AntiAliasingParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Li2AntiAliasing(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Thresh;
    pField[2] = &Data.LogFractionalCorrect;
    Max = Li2AntiAliasingParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Li2AntiAliasing(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void Li2AutoBadPixelCorrectionProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_DYNAMIC_BPC_s Data;
    void *pField[Li2AutoBadPixelCorrectionParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Li2DynamicBpc(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.HotPixelStrength;
    pField[2] = &Data.DarkPixelStrength;
    pField[3] = &Data.CorrectionMethod;
    Max = Li2AutoBadPixelCorrectionParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Li2DynamicBpc(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void Li2CfaLeakageFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_CFA_LEAKAGE_FILTER_s Data;
    void *pField[Li2CfaLeakageFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Li2CfaLeakageFilter(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.AlphaRr;
    pField[2] = &Data.AlphaRb;
    pField[3] = &Data.AlphaBr;
    pField[4] = &Data.AlphaBb;
    pField[5] = &Data.SaturationLevel;
    Max = Li2CfaLeakageFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Li2CfaLeakageFilter(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void Li2CfaNoiseFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_CFA_NOISE_FILTER_s Data;
    void *pField[Li2CfaNoiseFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Li2CfaNoiseFilter(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.MinNoiseLevel[0];
    pField[2] = &Data.MinNoiseLevel[1];
    pField[3] = &Data.MinNoiseLevel[2];
    pField[4] = &Data.MaxNoiseLevel[0];
    pField[5] = &Data.MaxNoiseLevel[1];
    pField[6] = &Data.MaxNoiseLevel[2];
    pField[7] = &Data.OriginalBlendStr[0];
    pField[8] = &Data.OriginalBlendStr[1];
    pField[9] = &Data.OriginalBlendStr[2];
    pField[10] = &Data.ExtentRegular[0];
    pField[11] = &Data.ExtentRegular[1];
    pField[12] = &Data.ExtentRegular[2];
    pField[13] = &Data.ExtentFine[0];
    pField[14] = &Data.ExtentFine[1];
    pField[15] = &Data.ExtentFine[2];
    pField[16] = &Data.StrengthFine[0];
    pField[17] = &Data.StrengthFine[1];
    pField[18] = &Data.StrengthFine[2];
    pField[19] = &Data.SelectivityRegular;
    pField[20] = &Data.SelectivityFine;
    pField[21] = &Data.DirectionalEnable;
    pField[22] = &Data.DirectionalHorvertEdgeStrength;
    pField[23] = &Data.DirectionalHorvertStrengthBias;
    pField[24] = &Data.DirectionalHorvertOriginalBlendStrength;
    pField[25] = &Data.DirLevMax[0];
    pField[26] = &Data.DirLevMax[1];
    pField[27] = &Data.DirLevMax[2];
    pField[28] = &Data.DirLevMin[0];
    pField[29] = &Data.DirLevMin[1];
    pField[30] = &Data.DirLevMin[2];
    pField[31] = &Data.DirLevMul[0];
    pField[32] = &Data.DirLevMul[1];
    pField[33] = &Data.DirLevMul[2];
    pField[34] = &Data.DirLevOffset[0];
    pField[35] = &Data.DirLevOffset[1];
    pField[36] = &Data.DirLevOffset[2];
    pField[37] = &Data.DirLevShift[0];
    pField[38] = &Data.DirLevShift[1];
    pField[39] = &Data.DirLevShift[2];
    pField[40] = &Data.LevMul[0];
    pField[41] = &Data.LevMul[1];
    pField[42] = &Data.LevMul[2];
    pField[43] = &Data.LevOffset[0];
    pField[44] = &Data.LevOffset[1];
    pField[45] = &Data.LevOffset[2];
    pField[46] = &Data.LevShift[0];
    pField[47] = &Data.LevShift[1];
    pField[48] = &Data.LevShift[2];
    pField[49] = &Data.ApplyToColorDiffRed;
    pField[50] = &Data.ApplyToColorDiffBlue;
    pField[51] = &Data.OptForRccbRed;
    pField[52] = &Data.OptForRccbBlue;
    Max = Li2CfaNoiseFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Li2CfaNoiseFilter(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void Li2DemosaicFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_DEMOSAIC_s Data;
    void *pField[Li2DemosaicFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Li2Demosaic(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.ActivityThresh;
    pField[2] = &Data.ActivityDifferenceThresh;
    pField[3] = &Data.GradClipThresh;
    pField[4] = &Data.GradNoiseThresh;
    pField[5] = &Data.AliasInterpolationStrength;
    pField[6] = &Data.AliasInterpolationThresh;
    Max = Li2DemosaicFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Li2Demosaic(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void Li2GbGrMismatchProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_GRGB_MISMATCH_s Data;
    void *pField[Li2GbGrMismatchParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Li2GrGbMismatch(&Data);
    pField[0] = &Data.NarrowEnable;
    pField[1] = &Data.WideEnable;
    pField[2] = &Data.WideSafety;
    pField[3] = &Data.WideThresh;
    Max = Li2GbGrMismatchParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Li2GrGbMismatch(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void Li2SharpenBothProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_SHPNS_BOTH_s Data;
    void *pField[Li2SharpeningBothParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Li2ShpnsBoth(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.Mode;
    pField[2] = &Data.EdgeThresh;
    pField[3] = &Data.WideEdgeDetect;
    pField[4] = &Data.MaxChangeUp5x5;
    pField[5] = &Data.MaxChangeDown5x5;
    pField[6] = &Data.MaxChangeUp;
    pField[7] = &Data.MaxChangeDown;
    Max = Li2SharpeningBothParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Li2ShpnsBoth(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void Li2SharpenNoiseProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_SHPNS_NOISE_s Data;
    void *pField[Li2SharpeningNoiseParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Li2ShpnsNoise(&Data);
    pField[0] = &Data.MaxChangeUp;
    pField[1] = &Data.MaxChangeDown;
    pField[2] = &Data.SpatialFir.Specify;
    pField[3] = &Data.SpatialFir.PerDirFirIsoStrengths;
    pField[4] = &Data.SpatialFir.PerDirFirDirStrengths;
    pField[5] = &Data.SpatialFir.PerDirFirDirAmounts;
    pField[6] = &Data.SpatialFir.Coefs;
    pField[7] = &Data.SpatialFir.StrengthIso;
    pField[8] = &Data.SpatialFir.StrengthDir;
    pField[9] = &Data.LevelStrAdjust.Low;
    pField[10] = &Data.LevelStrAdjust.LowDelta;
    pField[11] = &Data.LevelStrAdjust.LowStrength;
    pField[12] = &Data.LevelStrAdjust.MidStrength;
    pField[13] = &Data.LevelStrAdjust.High;
    pField[14] = &Data.LevelStrAdjust.HighDelta;
    pField[15] = &Data.LevelStrAdjust.HighStrength;
    pField[16] = &Data.LevelStrAdjust.Method;
    pField[17] = &Data.LevelStrAdjustNotT0T1LevelBased;
    pField[18] = &Data.T0;
    pField[19] = &Data.T1;
    pField[20] = &Data.AlphaMin;
    pField[21] = &Data.AlphaMax;
    Max = Li2SharpeningNoiseParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Li2ShpnsNoise(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void Li2SharpenFirProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_SHPNS_FIR_s Data;
    void *pField[Li2FirstSharpeningFirParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Li2ShpnsFir(&Data);
    pField[0] = &Data.Specify;
    pField[1] = &Data.PerDirFirIsoStrengths;
    pField[2] = &Data.PerDirFirDirStrengths;
    pField[3] = &Data.PerDirFirDirAmounts;
    pField[4] = &Data.Coefs;
    pField[5] = &Data.StrengthIso;
    pField[6] = &Data.StrengthDir;
    Max = Li2FirstSharpeningFirParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Li2ShpnsFir(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void Li2SharpenCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_SHPNS_CORING_s Data;
    void *pField[Li2SharpeningCoringParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Li2ShpnsCoring(&Data);
    pField[0] = &Data.Coring;
    Max = Li2SharpeningCoringParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Li2ShpnsCoring(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void Li2CoringIndexScaleProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_SHPNS_COR_IDX_SCL_s Data;
    void *pField[Li2SharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Li2ShpnsCorIdxScl(&Data);
    pField[0] = &Data.Low;
    pField[1] = &Data.LowDelta;
    pField[2] = &Data.LowStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.High;
    pField[5] = &Data.HighDelta;
    pField[6] = &Data.HighStrength;
    pField[7] = &Data.Method;
    Max = Li2SharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Li2ShpnsCorIdxScl(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void Li2MaxCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_SHPNS_MAX_COR_RST_s Data;
    void *pField[Li2SharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Li2ShpnsMaxCorRst(&Data);
    pField[0] = &Data.Low;
    pField[1] = &Data.LowDelta;
    pField[2] = &Data.LowStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.High;
    pField[5] = &Data.HighDelta;
    pField[6] = &Data.HighStrength;
    pField[7] = &Data.Method;
    Max = Li2SharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Li2ShpnsMaxCorRst(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void Li2MinCoringResultProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_SHPNS_MIN_COR_RST_s Data;
    void *pField[Li2SharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Li2ShpnsMinCorRst(&Data);
    pField[0] = &Data.Low;
    pField[1] = &Data.LowDelta;
    pField[2] = &Data.LowStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.High;
    pField[5] = &Data.HighDelta;
    pField[6] = &Data.HighStrength;
    pField[7] = &Data.Method;
    Max = Li2SharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Li2ShpnsMinCorRst(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void Li2ScaleCoringProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_SHPNS_SCL_COR_s Data;
    void *pField[Li2SharpeningLevelParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_Li2ShpnsSclCor(&Data);
    pField[0] = &Data.Low;
    pField[1] = &Data.LowDelta;
    pField[2] = &Data.LowStrength;
    pField[3] = &Data.MidStrength;
    pField[4] = &Data.High;
    pField[5] = &Data.HighDelta;
    pField[6] = &Data.HighStrength;
    pField[7] = &Data.Method;
    Max = Li2SharpeningLevelParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_Li2ShpnsSclCor(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void HiNonsmoothDetectProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_HI_NONSMOOTH_DETECT_s Data;
    void *pField[HiNonsmoothDetectParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_HiNonsmoothDetect(&Data);
    pField[0] = &Data.Smooth;
    pField[1] = &Data.LevAdjust;
    Max = HiNonsmoothDetectParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_HiNonsmoothDetect(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void Li2WideChromaFilterProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_WIDE_CHROMA_FILTER_s Data;
    void *pField[GetArraySize(WideChromaFilterParams)];
    AmbaItuner_Get_Li2WideChromaFilter(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.NoiseLevelCb;
    pField[2] = &Data.NoiseLevelCr;
    //OVER_FLOW_CHECK(Index, (INT32)GetArraySize(WideChromaFilterParams) - 1);
    ituner_param_proc(pParam, pField[Index]);
    AmbaItuner_Set_Li2WideChromaFilter(&Data);
    if (ParamStatus!=NULL) {
        *ParamStatus |= (((Index == 0) && (Data.Enable == 0U)) ? SKIP_VALID_CHECK : (0x1ULL << (UINT32)Index));
    }
}

static void Li2WideChromaFilterCombineProc(INT32 Index, UINT64 *ParamStatus, const PARAM_s *pParam)
{
    AMBA_IK_LI2_WIDE_CHROMA_FILTER_COMBINE_s Data;
    void *pField[GetArraySize(WideChromaFilterCombineParams)];
    AmbaItuner_Get_Li2WideChromaFilterCombine(&Data);
    pField[0] = &Data.T0Cb;
    pField[1] = &Data.T0Cr;
    pField[2] = &Data.T1Cb;
    pField[3] = &Data.T1Cr;
    pField[4] = &Data.AlphaMaxCb;
    pField[5] = &Data.AlphaMaxCr;
    pField[6] = &Data.AlphaMinCb;
    pField[7] = &Data.AlphaMinCr;
    pField[8] = &Data.MaxChangeCb;
    pField[9] = &Data.MaxChangeCr;
    //OVER_FLOW_CHECK(Index, (INT32)GetArraySize(WideChromaFilterCombineParams) - 1);
    ituner_param_proc(pParam, pField[Index]);
    AmbaItuner_Set_Li2WideChromaFilterCombine(&Data);
    if (ParamStatus!= NULL) {
        *ParamStatus |= (0x1ULL << (UINT32)Index);
    }
}


