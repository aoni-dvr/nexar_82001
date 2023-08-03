/*
*  @file AmbaTUNE_Rule_cv2fs.c
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
#include "AmbaTUNE_Parser_cv2fs.h"
#include "AmbaTUNE_Rule_cv2fs.h"
#include "AmbaTUNE_ImgCalibItuner_cv2fs.h" //Note: AMBA_ITN_SYSTEM_s need
#include "AmbaDSP_ImageFilter.h" // Note: IK_NUM_EXPOSURE_CURVE need
#include "AmbaMisraFix.h"

static void ituner_param_proc(const TUNE_PARAM_s *pParam, const void *pField);
static void System_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void Internal_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void AaaFunctionProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void AeInfoProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void WbSimInfoProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void StaticBadPixelCorrectionProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void StaticBadPixelCorrectionInternalProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void VignetteCompensationProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void WarpCompensationProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void DzoomProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void WarpCompensationDzoomInternalProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void SensorInputProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void DeferredBlackLevelProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void DynamicBadPixelCorrectionProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void CfaLeakageFilterProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void CfaNoiseFilterProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void AntiAliasingProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void BeforeCeWbGainProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void AfterCeWbGainProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void PreCcGainProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void ColorCorrectionProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void ToneCurveProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void FrontEndToneCurveProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void RgbToYuvMatrixProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void RgbIrProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void ChromaScaleProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void ChromaMedianFilterProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void DemosaicFilterProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void RgbToY12Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void SharpenBothProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void FinalSharpenBothProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void SharpenNoiseProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void FinalSharpenNoiseProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void SharpenFirProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void SharpenCoringProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void FinalSharpenCoringProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void FinalSharpenFirProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void CoringIndexScaleProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void MinCoringResultProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void MaxCoringResultProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void ScaleCoringProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void FinalCoringIndexScaleProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void FinalScaleCoringProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void FinalMinCoringResultProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void FinalMaxCoringResultProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void VideoMctfProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void VideoMctfLevelProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void VideoMctfTemporalAdjustProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void VideoMctfAndFinalSharpenProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void ShpAOrSpatialFilterSelectproc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void AdvanceSpatialFilterProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void LumaNoiseReductionProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void ChromaFilterProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void GbGrMismatchProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void ChromaAberrationProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void ChromaAberrationInternalProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void ContrastEnhance_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);

static void HdrFrontEndWbgainExp0_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void HdrFrontEndWbgainExp1_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void HdrFrontEndWbgainExp2_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void HdrBlend_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void HdrRawInfo_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void HdrStaticBlackLevelExp0_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void HdrStaticBlackLevelExp1_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void HdrStaticBlackLevelExp2_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void WideChromaFilterProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void WideChromaFilterCombineProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void DummyProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);
static void ActiveProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam);

static TUNE_OpMode_e Opmode = ITUNER_ENC;

static const char str_tuning_mode[IMG_MODE_NUMBER][32] = {
    "IMG_MODE_VIDEO",
    "IMG_MODE_PREVIEW",
};

static const char str_tuning_mode_ext[TUNING_MODE_EXT_NUMBER][32] = {
    "SINGLE_SHOT",
    "SINGLE_SHOT_MULTI_EXPOSURE_HDR",
};

#define System_Params_Count 29
static TUNE_PARAM_s System_Params[System_Params_Count] = {
    { 0, "ituner_rev", U_32, 1, NULL, 0},
    { 1, "sensor_id", STRING, 1, NULL, 0},
    { 2, "tuning_mode", STRING, 1, NULL, 0},
    { 3, "raw_path", STRING, 1, NULL, 0},
    { 4, "raw_width", U_32, 1, NULL, 0},
    { 5, "raw_height", U_32, 1, NULL, 0},
    { 6, "raw_pitch", U_32, 1, NULL, 0},
    { 7, "raw_resolution", U_32, 1, NULL, 0},
    { 8, "raw_bayer", U_32, 1, NULL, 0},
    { 9, "main_width", U_32, 1, NULL, 0},
    { 10, "main_height", U_32, 1, NULL, 0},
    { 11, "input_pic_cnt", U_32, 1, NULL, 0},
    { 12, "compressed_raw", U_32, 1, NULL, 0},
    { 13, "sensor_readout_mode", U_32, 1, NULL, 0},
    { 14, "raw_start_x", U_32, 1, NULL, 0},
    { 15, "raw_start_y", U_32, 1, NULL, 0},
    { 16, "h_subsample_factor_num", U_32, 1, NULL, 0},
    { 17, "h_subsample_factor_den", U_32, 1, NULL, 0},
    { 18, "v_subsample_factor_num", U_32, 1, NULL, 0},
    { 19, "v_subsample_factor_den", U_32, 1, NULL, 0},
    { 20, "number_of_exposures", U_32, 1, NULL, 0},
    { 21, "sensor_mode", U_32, 1, NULL, 0},
    { 22, "compression_offset", S_32, 1, NULL, 0},
    { 23, "ability", U_32, 1, NULL, 0},
    { 24, "yuv_mode", U_32, 1, NULL, 0},
    { 25, "flip_h", U_32, 1, NULL, 0},
    { 26, "flip_v", U_32, 1, NULL, 0},
    { 27, "frame_number", U_32, 1, NULL, 0},
    { 28, "number_of_frames", U_32, 1, NULL, 0},
};

#define Internal_Params_Count 4
static TUNE_PARAM_s Internal_Params[Internal_Params_Count] = {
    { 0, "fpn_highlight", S_8, 1, NULL, 0},
    { 1, "diag_mode", U_8, 1, NULL, 0},
    { 2, "video_mctf_y_10", U_8, 1, NULL, 0},
    { 3, "video_mctf_fast_start", U_8, 1, NULL, 0},
};

#define AaaFunctionParams_Count 4
static TUNE_PARAM_s AaaFunctionParams[AaaFunctionParams_Count] = {
    { 0, "ae_op", U_8, 1, NULL, 0},
    { 1, "awb_op", U_8, 1, NULL, 0},
    { 2, "af_op", U_8, 1, NULL, 0},
    { 3, "adj_op", U_8, 1, NULL, 0},
};

#define AeInfoParams_Count 15
static TUNE_PARAM_s AeInfoParams[AeInfoParams_Count] = {
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
static TUNE_PARAM_s WbSimInfoParams[WbSimInfoParams_Count] = {
    { 0, "luma_index", U_16, 1, NULL, 0},
    { 1, "outdoor_index", U_16, 1, NULL, 0},
    { 2, "highlight_index", U_16, 1, NULL, 0},
    { 3, "low_index", U_16, 1, NULL, 0},
    { 4, "awb_ratio", U_32, 2, NULL, 0},
};

#define StaticBadPixelCorrectionParams_Count 11
static TUNE_PARAM_s StaticBadPixelCorrectionParams[StaticBadPixelCorrectionParams_Count] = {
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
static TUNE_PARAM_s StaticBadPixelCorrectionInternalParams[StaticBadPixelCorrectionInternalParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "pixel_map_width", U_32, 1, NULL, 0},
    {2, "pixel_map_height", U_32, 1, NULL, 0},
    {3, "pixel_map_pitch", U_32, 1, NULL, 0},
    {4, "map_path", EXT_PATH, 1, NULL, 0},
};

#define VignetteCompensationParams_Count 18
static TUNE_PARAM_s VignetteCompensationParams[VignetteCompensationParams_Count] = {
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
static TUNE_PARAM_s WarpCompensationParams[WarpCompensationParams_Count] = {
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
static TUNE_PARAM_s WarpCompensationDzoomInternalParams[WarpCompensationDzoomInternalParams_Count] = { //FIX ME
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
static TUNE_PARAM_s DzoomParams[DzoomParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "shiftx", S_32, 1, NULL, 0},
    {2, "shifty", S_32, 1, NULL, 0},
    {3, "zoomx", U_32, 1, NULL, 0},
    {4, "zoomy", U_32, 1, NULL, 0},
};

#define DummyParams_Count 5
static TUNE_PARAM_s DummyParams[DummyParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "left", U_32, 1, NULL, 0},
    {2, "right", U_32, 1, NULL, 0},
    {3, "top", U_32, 1, NULL, 0},
    {4, "bottom", U_32, 1, NULL, 0},
};

#define ActiveParams_Count 5
static TUNE_PARAM_s ActiveParams[ActiveParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "startx", U_32, 1, NULL, 0},
    {2, "starty", U_32, 1, NULL, 0},
    {3, "width",  U_32, 1, NULL, 0},
    {4, "height", U_32, 1, NULL, 0},
};

#define ChromaAberrationParams_Count 16
static TUNE_PARAM_s ChromaAberrationParams[ChromaAberrationParams_Count] = {
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
static TUNE_PARAM_s ChromaAberrationInternalParams[ChromaAberrationInternalParams_Count] = {
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
static TUNE_PARAM_s SensorInputParams[SensorInputParams_Count] = {
    {0, "sensor_id", U_32, 1, NULL, 0},
    {1, "bayer_pattern", U_32, 1, NULL, 0},
    {2, "sensor_resolution", U_32, 1, NULL, 0},
    {3, "readout_mode", U_32, 1, NULL, 0},
};

#define StaticBlackLevelParams_Count 6
static TUNE_PARAM_s StaticBlackLevelParams[StaticBlackLevelParams_Count] = {
    {0, "r_black", S_32, 1, NULL, 0},
    {1, "g_r_black", S_32, 1, NULL, 0},
    {2, "g_b_black", S_32, 1, NULL, 0},
    {3, "b_black", S_32, 1, NULL, 0},
    {4, "g_black", S_32, 1, NULL, 0},
    {5, "ir_black", S_32, 1, NULL, 0},
};

#define DeferredBlackLevelParams_Count 1
static TUNE_PARAM_s DeferredBlackLevelParams[DeferredBlackLevelParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
};

#define DynamicBadPixelCorrectionParams_Count 6
static TUNE_PARAM_s DynamicBadPixelCorrectionParams[DynamicBadPixelCorrectionParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "hot_pixel_detection_strength", U_32, 1, NULL, 0},
    {2, "dark_pixel_detection_strength", U_32, 1, NULL, 0},
    {3, "correction_method", U_32, 1, NULL, 0},
    {4, "hot_pixel_detection_strength_irmode_redblue", U_32, 1, NULL, 0},
    {5, "dark_pixel_detection_strength_irmode_redblue", U_32, 1, NULL, 0},
};

#define CfaLeakageFilterParams_Count 6
static TUNE_PARAM_s CfaLeakageFilterParams[CfaLeakageFilterParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "alpha_rr", S_32, 1, NULL, 0},
    {2, "alpha_rb", S_32, 1, NULL, 0},
    {3, "alpha_br", S_32, 1, NULL, 0},
    {4, "alpha_bb", S_32, 1, NULL, 0},
    {5, "saturation_level", U_32, 1, NULL, 0},
};

#define CfaNoiseFilterParams_Count 53
static TUNE_PARAM_s CfaNoiseFilterParams[CfaNoiseFilterParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "extent_fine_red", U_32, 1, NULL, 0},
    {2, "extent_fine_green", U_32, 1, NULL, 0},
    {3, "extent_fine_blue", U_32, 1, NULL, 0},
    {4, "extent_regular_red", U_32, 1, NULL, 0},
    {5, "extent_regular_green", U_32, 1, NULL, 0},
    {6, "extent_regular_blue", U_32, 1, NULL, 0},
    {7, "original_blend_strength_red", U_32, 1, NULL, 0},
    {8, "original_blend_strength_green", U_32, 1, NULL, 0},
    {9, "original_blend_strength_blue", U_32, 1, NULL, 0},
    {10, "selectivity_fine", U_32, 1, NULL, 0},
    {11, "selectivity_regular", U_32, 1, NULL, 0},
    {12, "strength_fine_red", U_32, 1, NULL, 0},
    {13, "strength_fine_green", U_32, 1, NULL, 0},
    {14, "strength_fine_blue", U_32, 1, NULL, 0},
    {15, "dir_lev_max_red", U_32, 1, NULL, 0},
    {16, "dir_lev_max_green", U_32, 1, NULL, 0},
    {17, "dir_lev_max_blue", U_32, 1, NULL, 0},
    {18, "dir_lev_min_red", U_32, 1, NULL, 0},
    {19, "dir_lev_min_green", U_32, 1, NULL, 0},
    {20, "dir_lev_min_blue", U_32, 1, NULL, 0},
    {21, "dir_lev_mul_red", U_32, 1, NULL, 0},
    {22, "dir_lev_mul_green", U_32, 1, NULL, 0},
    {23, "dir_lev_mul_blue", U_32, 1, NULL, 0},
    {24, "dir_lev_offset_red", S_32, 1, NULL, 0},
    {25, "dir_lev_offset_green", S_32, 1, NULL, 0},
    {26, "dir_lev_offset_blue", S_32, 1, NULL, 0},
    {27, "dir_lev_shift_red", U_32, 1, NULL, 0},
    {28, "dir_lev_shift_green", U_32, 1, NULL, 0},
    {29, "dir_lev_shift_blue", U_32, 1, NULL, 0},
    {30, "lev_mul_red", U_32, 1, NULL, 0},
    {31, "lev_mul_green", U_32, 1, NULL, 0},
    {32, "lev_mul_blue", U_32, 1, NULL, 0},
    {33, "lev_offset_red", S_32, 1, NULL, 0},
    {34, "lev_offset_green", S_32, 1, NULL, 0},
    {35, "lev_offset_blue", S_32, 1, NULL, 0},
    {36, "lev_shift_red", U_32, 1, NULL, 0},
    {37, "lev_shift_green", U_32, 1, NULL, 0},
    {38, "lev_shift_blue", U_32, 1, NULL, 0},
    {39, "max_noise_level_red", U_32, 1, NULL, 0},
    {40, "max_noise_level_green", U_32, 1, NULL, 0},
    {41, "max_noise_level_blue", U_32, 1, NULL, 0},
    {42, "min_noise_level_red", U_32, 1, NULL, 0},
    {43, "min_noise_level_green", U_32, 1, NULL, 0},
    {44, "min_noise_level_blue", U_32, 1, NULL, 0},
    {45, "apply_to_color_diff_red", U_32, 1, NULL, 0},
    {46, "apply_to_color_diff_blue", U_32, 1, NULL, 0},
    {47, "opt_for_rccb_red", U_32, 1, NULL, 0},
    {48, "opt_for_rccb_blue", U_32, 1, NULL, 0},
    {49, "directional_enable", U_32, 1, NULL, 0},
    {50, "directional_horvert_edge_strength", U_32, 1, NULL, 0},
    {51, "directional_horvert_edge_strength_bias", U_32, 1, NULL, 0},
    {52, "directional_horvert_original_blend_strength", U_32, 1, NULL, 0},
};

#define AntiAliasingParams_Count 3
static TUNE_PARAM_s AntiAliasingParams[AntiAliasingParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "thresh", U_32, 1, NULL, 0},
    {2, "log_fractional_correct", U_32, 1, NULL, 0},
};

#define WbGainParams_Count 3
static TUNE_PARAM_s WbGainParams[WbGainParams_Count] = {
    {0, "r_gain", U_32, 1, NULL, 0},
    {1, "g_gain", U_32, 1, NULL, 0},
    {2, "b_gain", U_32, 1, NULL, 0},
};

#define PreCcGainParams_Count 4
static TUNE_PARAM_s PreCcGainParams[PreCcGainParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "r_gain", U_32, 1, NULL, 0},
    {2, "g_gain", U_32, 1, NULL, 0},
    {3, "b_gain", U_32, 1, NULL, 0},
};

#define ColorCorrectionParams_Count 4
static TUNE_PARAM_s ColorCorrectionParams[ColorCorrectionParams_Count] = {
    {0, "three_d_path", EXT_PATH, 1, NULL, 0},
    {1, "matrix_enable", U_8, 1, NULL, 0},
    {2, "matrix", S_32, 6, NULL, 0},
    {3, "matrix_shift_minus_8", U_8, 1, NULL, 0},

};

#define ToneCurveParams_Count 3
static TUNE_PARAM_s ToneCurveParams[ToneCurveParams_Count] = {
    {0, "curve_red", U_32, IK_NUM_TONE_CURVE, NULL, 0},
    {1, "curve_green", U_32, IK_NUM_TONE_CURVE, NULL, 0},
    {2, "curve_blue", U_32, IK_NUM_TONE_CURVE, NULL, 0},
};

#define FrontEndToneCurveParams_Count 4
static TUNE_PARAM_s FrontEndToneCurveParams[FrontEndToneCurveParams_Count] = {
    {0, "compand_enable", U_32, 1, NULL, 0},
    {1, "compand_table", U_16, 45, NULL, 0},
    {2, "decompand_enable", U_32, 1, NULL, 0},
    {3, "decompand_table", U_32, 145, NULL, 0},
};

#define RgbToYuvMatrixParams_Count 12
static TUNE_PARAM_s RgbToYuvMatrixParams[RgbToYuvMatrixParams_Count] = {
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
static TUNE_PARAM_s RgbIrParams[RgbIrParams_Count] = {
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
static TUNE_PARAM_s ChromaScaleParams[ChromaScaleParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "gain_curve", U_32, IK_NUM_CHROMA_GAIN_CURVE, NULL, 0},
};

#define ChromaMedianFilterParams_Count 7
static TUNE_PARAM_s ChromaMedianFilterParams[ChromaMedianFilterParams_Count] = {
    {0, "enable", S_32, 1, NULL, 0},
    {1, "cb_adaptive_strength", U_32, 1, NULL, 0},
    {2, "cr_adaptive_strength", U_32, 1, NULL, 0},
    {3, "cb_non_adaptive_strength", U_32, 1, NULL, 0},
    {4, "cr_non_adaptive_strength", U_32, 1, NULL, 0},
    {5, "cb_adaptive_amount", U_32, 1, NULL, 0},
    {6, "cr_adaptive_amount", U_32, 1, NULL, 0},
};

#define DemosaicFilterParams_Count 7
static TUNE_PARAM_s DemosaicFilterParams[DemosaicFilterParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "activity_thresh", U_32, 1, NULL, 0},
    {2, "activity_difference_thresh", U_32, 1, NULL, 0},
    {3, "grad_clip_thresh", U_32, 1, NULL, 0},
    {4, "grad_noise_thresh", U_32, 1, NULL, 0},
    {5, "alias_interpolation_strength", U_32, 1, NULL, 0},
    {6, "alias_interpolation_thresh", U_32, 1, NULL, 0},
};

#define RgbToY12Params_Count 5
static TUNE_PARAM_s RgbToY12Params[RgbToY12Params_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "m11", S_32, 1, NULL, 0},
    {2, "m12", S_32, 1, NULL, 0},
    {3, "m13", S_32, 1, NULL, 0},
    {4, "y_offset", S_32, 1, NULL, 0},
};

#define GbGrMismatchParams_Count 4
static TUNE_PARAM_s GbGrMismatchParams[] = {
    {0, "narrow_enable", U_32, 1, NULL, 0},
    {1, "wide_enable", U_32, 1, NULL, 0},
    {2, "wide_safety", U_32, 1, NULL, 0},
    {3, "wide_thresh", U_32, 1, NULL, 0},
};

#define SharpeningBothParams_Count 6
static TUNE_PARAM_s SharpeningBothParams[SharpeningBothParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "mode", U_32, 1, NULL, 0},
    {2, "edge_thresh", U_32, 1, NULL, 0},
    {3, "wide_edge_detect", U_32, 1, NULL, 0},
    {4, "max_change_up5x5", U_32, 1, NULL, 0},
    {5, "max_change_down5x5", U_32, 1, NULL, 0},
};

#define FinalSharpeningBothParams_Count 19
static TUNE_PARAM_s FinalSharpeningBothParams[FinalSharpeningBothParams_Count] = {
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
static TUNE_PARAM_s SharpeningNoiseParams[SharpeningNoiseParams_Count] = {
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
static TUNE_PARAM_s FinalSharpeningNoiseParams[FinalSharpeningNoiseParams_Count] = {//FIX ME
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
static TUNE_PARAM_s FirstSharpeningFirParams[FirstSharpeningFirParams_Count] = {
    {0, "fir_specify", U_32, 1, NULL, 0},
    {1, "fir_strength_iso", U_32, 1, NULL, 0},
    {2, "fir_strength_dir", U_32, 1, NULL, 0},
    {3, "fir_per_dir_fir_dir_amounts", U_32, 9, NULL, 0},
    {4, "fir_per_dir_fir_dir_strengths", U_32, 9, NULL, 0},
    {5, "fir_per_dir_fir_iso_strengths", U_32, 9, NULL, 0},
    {6, "fir_coefs", S_32, 225, NULL, 0},
};

#define FinalSharpeningFirParams_Count 13
static TUNE_PARAM_s FinalSharpeningFirParams[FinalSharpeningFirParams_Count] = {
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
static TUNE_PARAM_s SharpeningCoringParams[SharpeningCoringParams_Count] = {
    {0, "coring_table", U_32, IK_NUM_CORING_TABLE_INDEX, NULL, 0},
    {1, "fractional_bits", U_32, 1, NULL, 0},
};

#define FianlSharpeningCoringParams_Count 2
static TUNE_PARAM_s FianlSharpeningCoringParams[FianlSharpeningCoringParams_Count] = {
    {0, "coring_table", U_32, IK_NUM_CORING_TABLE_INDEX, NULL, 0},
    {1, "fractional_bits", U_32, 1, NULL, 0},
};

#define SharpeningLevelParams_Count 7
static TUNE_PARAM_s SharpeningLevelParams[SharpeningLevelParams_Count] = {
    {0, "high", U_32, 1, NULL, 0},
    {1, "high_delta", U_32, 1, NULL, 0},
    {2, "high_strength", U_32, 1, NULL, 0},
    {3, "mid_strength", U_32, 1, NULL, 0},
    {4, "low", U_32, 1, NULL, 0},
    {5, "low_delta", U_32, 1, NULL, 0},
    {6, "low_strength", U_32, 1, NULL, 0},
};

#define FinalSharpeningLevelParams_Count 8
static TUNE_PARAM_s FinalSharpeningLevelParams[FinalSharpeningLevelParams_Count] = {
    {0, "high", U_32, 1, NULL, 0},
    {1, "high_delta", U_32, 1, NULL, 0},
    {2, "high_strength", U_32, 1, NULL, 0},
    {3, "mid_strength", U_32, 1, NULL, 0},
    {4, "low", U_32, 1, NULL, 0},
    {5, "low_delta", U_32, 1, NULL, 0},
    {6, "low_strength", U_32, 1, NULL, 0},
    {7, "method", U_32, 1, NULL, 0},
};

#define VideoMctfParams_Count 294
static TUNE_PARAM_s VideoMctfParams[VideoMctfParams_Count] = {
    {0,  "enable", U_32, 1, NULL, 0},
    {1,  "y_narrow_nonsmooth_detect_shift", U_32, 1, NULL, 0},
    {2,  "y_narrow_nonsmooth_detect_sub", U_32, 17, NULL, 0},
    {3,  "use_level_based_ta", U_32, 1, NULL, 0},
    {4,  "compression_enable", U_32, 1, NULL, 0},
    /*3d maxchange*/
    {5, "y_3d_maxchange_high", U_32, 1, NULL, 0},
    {6, "y_3d_maxchange_high_delta", U_32, 1, NULL, 0},
    {7, "y_3d_maxchange_high_strength", U_32, 1, NULL, 0},
    {8, "y_3d_maxchange_low", U_32, 1, NULL, 0},
    {9, "y_3d_maxchange_low_delta", U_32, 1, NULL, 0},
    {10, "y_3d_maxchange_low_strength", U_32, 1, NULL, 0},
    {11, "y_3d_maxchange_method", U_32, 1, NULL, 0},
    {12, "y_3d_maxchange_mid_strength", U_32, 1, NULL, 0},
    {13, "cb_3d_maxchange_high", U_32, 1, NULL, 0},
    {14, "cb_3d_maxchange_high_delta", U_32, 1, NULL, 0},
    {15, "cb_3d_maxchange_high_strength", U_32, 1, NULL, 0},
    {16, "cb_3d_maxchange_low", U_32, 1, NULL, 0},
    {17, "cb_3d_maxchange_low_delta", U_32, 1, NULL, 0},
    {18, "cb_3d_maxchange_low_strength", U_32, 1, NULL, 0},
    {19, "cb_3d_maxchange_method", U_32, 1, NULL, 0},
    {20, "cb_3d_maxchange_mid_strength", U_32, 1, NULL, 0},
    {21, "cr_3d_maxchange_high", U_32, 1, NULL, 0},
    {22, "cr_3d_maxchange_high_delta", U_32, 1, NULL, 0},
    {23, "cr_3d_maxchange_high_strength", U_32, 1, NULL, 0},
    {24, "cr_3d_maxchange_low", U_32, 1, NULL, 0},
    {25, "cr_3d_maxchange_low_delta", U_32, 1, NULL, 0},
    {26, "cr_3d_maxchange_low_strength", U_32, 1, NULL, 0},
    {27, "cr_3d_maxchange_method", U_32, 1, NULL, 0},
    {28, "cr_3d_maxchange_mid_strength", U_32, 1, NULL, 0},
    /*advanced iso*/
    {29, "y_advanced_iso_enable", U_32, 1, NULL, 0},
    {30, "y_advanced_iso_max_change_method", U_32, 1, NULL, 0},
    {31, "y_advanced_iso_max_change_table", U_32, 32, NULL, 0},
    {32, "y_advanced_iso_noise_level", U_32, 1, NULL, 0},
    {33, "y_advanced_iso_size", U_32, 1, NULL, 0},
    {34, "cb_advanced_iso_enable", U_32, 1, NULL, 0},
    {35, "cb_advanced_iso_max_change_method", U_32, 1, NULL, 0},
    {36, "cb_advanced_iso_max_change_table", U_32, 32, NULL, 0},
    {37, "cb_advanced_iso_noise_level", U_32, 1, NULL, 0},
    {38, "cb_advanced_iso_size", U_32, 1, NULL, 0},
    {39, "cr_advanced_iso_enable", U_32, 1, NULL, 0},
    {40, "cr_advanced_iso_max_change_method", U_32, 1, NULL, 0},
    {41, "cr_advanced_iso_max_change_table", U_32, 32, NULL, 0},
    {42, "cr_advanced_iso_noise_level", U_32, 1, NULL, 0},
    {43, "cr_advanced_iso_size", U_32, 1, NULL, 0},
    /*level_based_ta*/
    {44, "y_level_based_ta_high", U_32, 1, NULL, 0},
    {45, "y_level_based_ta_high_delta", U_32, 1, NULL, 0},
    {46, "y_level_based_ta_high_strength", U_32, 1, NULL, 0},
    {47, "y_level_based_ta_low", U_32, 1, NULL, 0},
    {48, "y_level_based_ta_low_delta", U_32, 1, NULL, 0},
    {49, "y_level_based_ta_low_strength", U_32, 1, NULL, 0},
    {50, "y_level_based_ta_method", U_32, 1, NULL, 0},
    {51, "y_level_based_ta_mid_strength", U_32, 1, NULL, 0},
    {52, "cb_level_based_ta_high", U_32, 1, NULL, 0},
    {53, "cb_level_based_ta_high_delta", U_32, 1, NULL, 0},
    {54, "cb_level_based_ta_high_strength", U_32, 1, NULL, 0},
    {55, "cb_level_based_ta_low", U_32, 1, NULL, 0},
    {56, "cb_level_based_ta_low_delta", U_32, 1, NULL, 0},
    {57, "cb_level_based_ta_low_strength", U_32, 1, NULL, 0},
    {58, "cb_level_based_ta_method", U_32, 1, NULL, 0},
    {59, "cb_level_based_ta_mid_strength", U_32, 1, NULL, 0},
    {60, "cr_level_based_ta_high", U_32, 1, NULL, 0},
    {61, "cr_level_based_ta_high_delta", U_32, 1, NULL, 0},
    {62, "cr_level_based_ta_high_strength", U_32, 1, NULL, 0},
    {63, "cr_level_based_ta_low", U_32, 1, NULL, 0},
    {64, "cr_level_based_ta_low_delta", U_32, 1, NULL, 0},
    {65, "cr_level_based_ta_low_strength", U_32, 1, NULL, 0},
    {66, "cr_level_based_ta_method", U_32, 1, NULL, 0},
    {67, "cr_level_based_ta_mid_strength", U_32, 1, NULL, 0},
    /*overall maxchange*/
    {68, "y_overall_max_change_high", U_32, 1, NULL, 0},
    {69, "y_overall_max_change_high_delta", U_32, 1, NULL, 0},
    {70, "y_overall_max_change_method", U_32, 1, NULL, 0},
    {71, "y_overall_max_change_high_strength", U_32, 1, NULL, 0},
    {72, "y_overall_max_change_low", U_32, 1, NULL, 0},
    {73, "y_overall_max_change_low_delta", U_32, 1, NULL, 0},
    {74, "y_overall_max_change_low_strength", U_32, 1, NULL, 0},
    {75, "y_overall_max_change_mid_strength", U_32, 1, NULL, 0},
    {76, "cb_overall_max_change_high", U_32, 1, NULL, 0},
    {77, "cb_overall_max_change_high_delta", U_32, 1, NULL, 0},
    {78, "cb_overall_max_change_method", U_32, 1, NULL, 0},
    {79, "cb_overall_max_change_high_strength", U_32, 1, NULL, 0},
    {80, "cb_overall_max_change_low", U_32, 1, NULL, 0},
    {81, "cb_overall_max_change_low_delta", U_32, 1, NULL, 0},
    {82, "cb_overall_max_change_low_strength", U_32, 1, NULL, 0},
    {83, "cb_overall_max_change_mid_strength", U_32, 1, NULL, 0},
    {84, "cr_overall_max_change_high", U_32, 1, NULL, 0},
    {85, "cr_overall_max_change_high_delta", U_32, 1, NULL, 0},
    {86, "cr_overall_max_change_method", U_32, 1, NULL, 0},
    {87, "cr_overall_max_change_high_strength", U_32, 1, NULL, 0},
    {88, "cr_overall_max_change_low", U_32, 1, NULL, 0},
    {89, "cr_overall_max_change_low_delta", U_32, 1, NULL, 0},
    {90, "cr_overall_max_change_low_strength", U_32, 1, NULL, 0},
    {91, "cr_overall_max_change_mid_strength", U_32, 1, NULL, 0},
    /*spat_blend*/
    {92,  "y_spat_blend_high", U_32, 1, NULL, 0},
    {93,  "y_spat_blend_high_delta", U_32, 1, NULL, 0},
    {94,  "y_spat_blend_high_strength", U_32, 1, NULL, 0},
    {95,  "y_spat_blend_low", U_32, 1, NULL, 0},
    {96,  "y_spat_blend_low_delta", U_32, 1, NULL, 0},
    {97,  "y_spat_blend_low_strength", U_32, 1, NULL, 0},
    {98,  "y_spat_blend_method", U_32, 1, NULL, 0},
    {99, "y_spat_blend_mid_strength", U_32, 1, NULL, 0},
    {100, "cb_spat_blend_high", U_32, 1, NULL, 0},
    {101, "cb_spat_blend_high_delta", U_32, 1, NULL, 0},
    {102, "cb_spat_blend_high_strength", U_32, 1, NULL, 0},
    {103, "cb_spat_blend_low", U_32, 1, NULL, 0},
    {104, "cb_spat_blend_low_delta", U_32, 1, NULL, 0},
    {105, "cb_spat_blend_low_strength", U_32, 1, NULL, 0},
    {106, "cb_spat_blend_method", U_32, 1, NULL, 0},
    {107, "cb_spat_blend_mid_strength", U_32, 1, NULL, 0},
    {108, "cr_spat_blend_high", U_32, 1, NULL, 0},
    {109, "cr_spat_blend_high_delta", U_32, 1, NULL, 0},
    {110, "cr_spat_blend_high_strength", U_32, 1, NULL, 0},
    {111, "cr_spat_blend_low", U_32, 1, NULL, 0},
    {112, "cr_spat_blend_low_delta", U_32, 1, NULL, 0},
    {113, "cr_spat_blend_low_strength", U_32, 1, NULL, 0},
    {114, "cr_spat_blend_method", U_32, 1, NULL, 0},
    {115, "cr_spat_blend_mid_strength", U_32, 1, NULL, 0},
    /*spat_filt_max_smth_change*/
    {116, "y_spat_filt_max_smth_change_high", U_32, 1, NULL, 0},
    {117, "y_spat_filt_max_smth_change_high_delta", U_32, 1, NULL, 0},
    {118, "y_spat_filt_max_smth_change_high_strength", U_32, 1, NULL, 0},
    {119, "y_spat_filt_max_smth_change_low", U_32, 1, NULL, 0},
    {120, "y_spat_filt_max_smth_change_low_delta", U_32, 1, NULL, 0},
    {121, "y_spat_filt_max_smth_change_low_strength", U_32, 1, NULL, 0},
    {122, "y_spat_filt_max_smth_change_method", U_32, 1, NULL, 0},
    {123, "y_spat_filt_max_smth_change_mid_strength", U_32, 1, NULL, 0},
    {124, "cb_spat_filt_max_smth_change_high", U_32, 1, NULL, 0},
    {125, "cb_spat_filt_max_smth_change_high_delta", U_32, 1, NULL, 0},
    {126, "cb_spat_filt_max_smth_change_high_strength", U_32, 1, NULL, 0},
    {127, "cb_spat_filt_max_smth_change_low", U_32, 1, NULL, 0},
    {128, "cb_spat_filt_max_smth_change_low_delta", U_32, 1, NULL, 0},
    {129, "cb_spat_filt_max_smth_change_low_strength", U_32, 1, NULL, 0},
    {130, "cb_spat_filt_max_smth_change_method", U_32, 1, NULL, 0},
    {131, "cb_spat_filt_max_smth_change_mid_strength", U_32, 1, NULL, 0},
    {132, "cr_spat_filt_max_smth_change_high", U_32, 1, NULL, 0},
    {133, "cr_spat_filt_max_smth_change_high_delta", U_32, 1, NULL, 0},
    {134, "cr_spat_filt_max_smth_change_high_strength", U_32, 1, NULL, 0},
    {135, "cr_spat_filt_max_smth_change_low", U_32, 1, NULL, 0},
    {136, "cr_spat_filt_max_smth_change_low_delta", U_32, 1, NULL, 0},
    {137, "cr_spat_filt_max_smth_change_low_strength", U_32, 1, NULL, 0},
    {138, "cr_spat_filt_max_smth_change_method", U_32, 1, NULL, 0},
    {139, "cr_spat_filt_max_smth_change_mid_strength", U_32, 1, NULL, 0},
    /*spat_smth_dir*/
    {140, "y_spat_smth_wide_edge_detect", U_32, 1, NULL, 0},
    {141, "y_spat_smth_edge_thresh", U_32, 1, NULL, 0},
    {142, "y_spat_smth_dir_high", U_32, 1, NULL, 0},
    {143, "y_spat_smth_dir_high_delta", U_32, 1, NULL, 0},
    {144, "y_spat_smth_dir_high_strength", U_32, 1, NULL, 0},
    {145, "y_spat_smth_dir_low", U_32, 1, NULL, 0},
    {146, "y_spat_smth_dir_low_delta", U_32, 1, NULL, 0},
    {147, "y_spat_smth_dir_low_strength", U_32, 1, NULL, 0},
    {148, "y_spat_smth_dir_method", U_32, 1, NULL, 0},
    {149, "y_spat_smth_dir_mid_strength", U_32, 1, NULL, 0},
    {150, "cb_spat_smth_wide_edge_detect", U_32, 1, NULL, 0},
    {151, "cb_spat_smth_edge_thresh", U_32, 1, NULL, 0},
    {152, "cb_spat_smth_dir_high", U_32, 1, NULL, 0},
    {153, "cb_spat_smth_dir_high_delta", U_32, 1, NULL, 0},
    {154, "cb_spat_smth_dir_high_strength", U_32, 1, NULL, 0},
    {155, "cb_spat_smth_dir_low", U_32, 1, NULL, 0},
    {156, "cb_spat_smth_dir_low_delta", U_32, 1, NULL, 0},
    {157, "cb_spat_smth_dir_low_strength", U_32, 1, NULL, 0},
    {158, "cb_spat_smth_dir_method", U_32, 1, NULL, 0},
    {159, "cb_spat_smth_dir_mid_strength", U_32, 1, NULL, 0},
    {160, "cr_spat_smth_wide_edge_detect", U_32, 1, NULL, 0},
    {161, "cr_spat_smth_edge_thresh", U_32, 1, NULL, 0},
    {162, "cr_spat_smth_dir_high", U_32, 1, NULL, 0},
    {163, "cr_spat_smth_dir_high_delta", U_32, 1, NULL, 0},
    {164, "cr_spat_smth_dir_high_strength", U_32, 1, NULL, 0},
    {165, "cr_spat_smth_dir_low", U_32, 1, NULL, 0},
    {166, "cr_spat_smth_dir_low_delta", U_32, 1, NULL, 0},
    {167, "cr_spat_smth_dir_low_strength", U_32, 1, NULL, 0},
    {168, "cr_spat_smth_dir_method", U_32, 1, NULL, 0},
    {169, "cr_spat_smth_dir_mid_strength", U_32, 1, NULL, 0},
    /*spat_smth_iso*/
    {170, "y_spat_smth_iso_high", U_32, 1, NULL, 0},
    {171, "y_spat_smth_iso_high_delta", U_32, 1, NULL, 0},
    {172, "y_spat_smth_iso_high_strength", U_32, 1, NULL, 0},
    {173, "y_spat_smth_iso_low", U_32, 1, NULL, 0},
    {174, "y_spat_smth_iso_low_delta", U_32, 1, NULL, 0},
    {175, "y_spat_smth_iso_low_strength", U_32, 1, NULL, 0},
    {176, "y_spat_smth_iso_method", U_32, 1, NULL, 0},
    {177, "y_spat_smth_iso_mid_strength", U_32, 1, NULL, 0},
    {178, "cb_spat_smth_iso_high", U_32, 1, NULL, 0},
    {179, "cb_spat_smth_iso_high_delta", U_32, 1, NULL, 0},
    {180, "cb_spat_smth_iso_high_strength", U_32, 1, NULL, 0},
    {181, "cb_spat_smth_iso_low", U_32, 1, NULL, 0},
    {182, "cb_spat_smth_iso_low_delta", U_32, 1, NULL, 0},
    {183, "cb_spat_smth_iso_low_strength", U_32, 1, NULL, 0},
    {184, "cb_spat_smth_iso_method", U_32, 1, NULL, 0},
    {185, "cb_spat_smth_iso_mid_strength", U_32, 1, NULL, 0},
    {186, "cr_spat_smth_iso_high", U_32, 1, NULL, 0},
    {187, "cr_spat_smth_iso_high_delta", U_32, 1, NULL, 0},
    {188, "cr_spat_smth_iso_high_strength", U_32, 1, NULL, 0},
    {189, "cr_spat_smth_iso_low", U_32, 1, NULL, 0},
    {190, "cr_spat_smth_iso_low_delta", U_32, 1, NULL, 0},
    {191, "cr_spat_smth_iso_low_strength", U_32, 1, NULL, 0},
    {192, "cr_spat_smth_iso_method", U_32, 1, NULL, 0},
    {193, "cr_spat_smth_iso_mid_strength", U_32, 1, NULL, 0},
    {194, "y_spat_smth_directional_decide_t0", U_32, 1, NULL, 0},
    {195, "y_spat_smth_directional_decide_t1", U_32, 1, NULL, 0},
    {196, "cb_spat_smth_directional_decide_t0", U_32, 1, NULL, 0},
    {197, "cb_spat_smth_directional_decide_t1", U_32, 1, NULL, 0},
    {198, "cr_spat_smth_directional_decide_t0", U_32, 1, NULL, 0},
    {199, "cr_spat_smth_directional_decide_t1", U_32, 1, NULL, 0},
    /*spatial_max_temporal*/
    {200, "y_spatial_max_change", U_32, 1, NULL, 0},
    {201, "y_spatial_max_temporal_high", U_32, 1, NULL, 0},
    {202, "y_spatial_max_temporal_high_delta", U_32, 1, NULL, 0},
    {203, "y_spatial_max_temporal_high_strength", U_32, 1, NULL, 0},
    {204, "y_spatial_max_temporal_low", U_32, 1, NULL, 0},
    {205, "y_spatial_max_temporal_low_delta", U_32, 1, NULL, 0},
    {206, "y_spatial_max_temporal_low_strength", U_32, 1, NULL, 0},
    {207, "y_spatial_max_temporal_method", U_32, 1, NULL, 0},
    {208, "y_spatial_max_temporal_mid_strength", U_32, 1, NULL, 0},
    {209, "cb_spatial_max_change", U_32, 1, NULL, 0},
    {210, "cb_spatial_max_temporal_high", U_32, 1, NULL, 0},
    {211, "cb_spatial_max_temporal_high_delta", U_32, 1, NULL, 0},
    {212, "cb_spatial_max_temporal_high_strength", U_32, 1, NULL, 0},
    {213, "cb_spatial_max_temporal_low", U_32, 1, NULL, 0},
    {214, "cb_spatial_max_temporal_low_delta", U_32, 1, NULL, 0},
    {215, "cb_spatial_max_temporal_low_strength", U_32, 1, NULL, 0},
    {216, "cb_spatial_max_temporal_method", U_32, 1, NULL, 0},
    {217, "cb_spatial_max_temporal_mid_strength", U_32, 1, NULL, 0},
    {218, "cr_spatial_max_change", U_32, 1, NULL, 0},
    {219, "cr_spatial_max_temporal_high", U_32, 1, NULL, 0},
    {220, "cr_spatial_max_temporal_high_delta", U_32, 1, NULL, 0},
    {221, "cr_spatial_max_temporal_high_strength", U_32, 1, NULL, 0},
    {222, "cr_spatial_max_temporal_low", U_32, 1, NULL, 0},
    {223, "cr_spatial_max_temporal_low_delta", U_32, 1, NULL, 0},
    {224, "cr_spatial_max_temporal_low_strength", U_32, 1, NULL, 0},
    {225, "cr_spatial_max_temporal_method", U_32, 1, NULL, 0},
    {226, "cr_spatial_max_temporal_mid_strength", U_32, 1, NULL, 0},
    /*temporal_min_target*/
    {227, "y_temporal_alpha0", U_32, 1, NULL, 0},
    {228, "y_temporal_alpha1", U_32, 1, NULL, 0},
    {229, "y_temporal_alpha2", U_32, 1, NULL, 0},
    {230, "y_temporal_alpha3", U_32, 1, NULL, 0},
    {231, "y_temporal_T0", U_32, 1, NULL, 0},
    {232, "y_temporal_T1", U_32, 1, NULL, 0},
    {233, "y_temporal_T2", U_32, 1, NULL, 0},
    {234, "y_temporal_T3", U_32, 1, NULL, 0},
    {235, "y_temporal_maxchange", U_32, 1, NULL, 0},
    {236, "y_temporal_min_target_high", U_32, 1, NULL, 0},
    {237, "y_temporal_min_target_high_delta", U_32, 1, NULL, 0},
    {238, "y_temporal_min_target_high_strength", U_32, 1, NULL, 0},
    {239, "y_temporal_min_target_low", U_32, 1, NULL, 0},
    {240, "y_temporal_min_target_low_delta", U_32, 1, NULL, 0},
    {241, "y_temporal_min_target_low_strength", U_32, 1, NULL, 0},
    {242, "y_temporal_min_target_method", U_32, 1, NULL, 0},
    {243, "y_temporal_min_target_mid_strength", U_32, 1, NULL, 0},
    {244, "cb_temporal_alpha0", U_32, 1, NULL, 0},
    {245, "cb_temporal_alpha1", U_32, 1, NULL, 0},
    {246, "cb_temporal_alpha2", U_32, 1, NULL, 0},
    {247, "cb_temporal_alpha3", U_32, 1, NULL, 0},
    {248, "cb_temporal_T0", U_32, 1, NULL, 0},
    {249, "cb_temporal_T1", U_32, 1, NULL, 0},
    {250, "cb_temporal_T2", U_32, 1, NULL, 0},
    {251, "cb_temporal_T3", U_32, 1, NULL, 0},
    {252, "cb_temporal_maxchange", U_32, 1, NULL, 0},
    {253, "cb_temporal_min_target_high", U_32, 1, NULL, 0},
    {254, "cb_temporal_min_target_high_delta", U_32, 1, NULL, 0},
    {255, "cb_temporal_min_target_high_strength", U_32, 1, NULL, 0},
    {256, "cb_temporal_min_target_low", U_32, 1, NULL, 0},
    {257, "cb_temporal_min_target_low_delta", U_32, 1, NULL, 0},
    {258, "cb_temporal_min_target_low_strength", U_32, 1, NULL, 0},
    {259, "cb_temporal_min_target_method", U_32, 1, NULL, 0},
    {260, "cb_temporal_min_target_mid_strength", U_32, 1, NULL, 0},
    {261, "cr_temporal_alpha0", U_32, 1, NULL, 0},
    {262, "cr_temporal_alpha1", U_32, 1, NULL, 0},
    {263, "cr_temporal_alpha2", U_32, 1, NULL, 0},
    {264, "cr_temporal_alpha3", U_32, 1, NULL, 0},
    {265, "cr_temporal_T0", U_32, 1, NULL, 0},
    {266, "cr_temporal_T1", U_32, 1, NULL, 0},
    {267, "cr_temporal_T2", U_32, 1, NULL, 0},
    {268, "cr_temporal_T3", U_32, 1, NULL, 0},
    {269, "cr_temporal_maxchange", U_32, 1, NULL, 0},
    {270, "cr_temporal_min_target_high", U_32, 1, NULL, 0},
    {271, "cr_temporal_min_target_high_delta", U_32, 1, NULL, 0},
    {272, "cr_temporal_min_target_high_strength", U_32, 1, NULL, 0},
    {273, "cr_temporal_min_target_low", U_32, 1, NULL, 0},
    {274, "cr_temporal_min_target_low_delta", U_32, 1, NULL, 0},
    {275, "cr_temporal_min_target_low_strength", U_32, 1, NULL, 0},
    {276, "cr_temporal_min_target_method", U_32, 1, NULL, 0},
    {277, "cr_temporal_min_target_mid_strength", U_32, 1, NULL, 0},
    {278, "y_temporal_ghost_prevent", U_32, 1, NULL, 0},
    {279, "cb_temporal_ghost_prevent", U_32, 1, NULL, 0},
    {280, "cr_temporal_ghost_prevent", U_32, 1, NULL, 0},
    {281, "compression_bit_rate_luma", U_32, 1, NULL, 0},
    {282, "compression_bit_rate_chroma", U_32, 1, NULL, 0},
    {283, "compression_dither_disable", U_32, 1, NULL, 0},
    {284, "y_temporal_artifact_guard", U_32, 1, NULL, 0},
    {285, "cb_temporal_artifact_guard", U_32, 1, NULL, 0},
    {286, "cr_temporal_artifact_guard", U_32, 1, NULL, 0},
    {287, "y_spatial_weighting", U_32, 64, NULL, 0},
    {288, "cb_spatial_weighting", U_32, 64, NULL, 0},
    {289, "cr_spatial_weighting", U_32, 64, NULL, 0},
    {290, "y_strength_3d", U_32, 1, NULL, 0},
    {291, "cb_strength_3d", U_32, 1, NULL, 0},
    {292, "cr_strength_3d", U_32, 1, NULL, 0},
    {293, "y_combine_strength", U_32, 1, NULL, 0},
};

#define VideoMctfLevelParams_Count 27
static TUNE_PARAM_s VideoMctfLevelParams[VideoMctfLevelParams_Count] = {
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

#define VideoMctfTemporalAdjustParams_Count 24
static TUNE_PARAM_s VideoMctfTemporalAdjustParams[VideoMctfTemporalAdjustParams_Count] = {
    {0, "lev_adjust_high", U_32, 1, NULL, 0},
    {1, "lev_adjust_high_delta", U_32, 1, NULL, 0},
    {2, "lev_adjust_high_strength", U_32, 1, NULL, 0},
    {3, "lev_adjust_low", U_32, 1, NULL, 0},
    {4, "lev_adjust_low_delta", U_32, 1, NULL, 0},
    {5, "lev_adjust_low_strength", U_32, 1, NULL, 0},
    {6, "lev_adjust_mid_strength", U_32, 1, NULL, 0},
    {7, "slow_mo_sensitivity", U_32, 1, NULL, 0},
    {8, "score_noise_robust", U_32, 1, NULL, 0},
    {9, "y_max", U_32, 1, NULL, 0},
    {10, "y_min", U_32, 1, NULL, 0},
    {11, "y_motion_response", U_32, 1, NULL, 0},
    {12, "y_noise_base", U_32, 1, NULL, 0},
    {13, "y_still_thresh", U_32, 1, NULL, 0},
    {14, "cr_max", U_32, 1, NULL, 0},
    {15, "cr_min", U_32, 1, NULL, 0},
    {16, "cr_motion_response", U_32, 1, NULL, 0},
    {17, "cr_noise_base", U_32, 1, NULL, 0},
    {18, "cr_still_thresh", U_32, 1, NULL, 0},
    {19, "cb_max", U_32, 1, NULL, 0},
    {20, "cb_min", U_32, 1, NULL, 0},
    {21, "cb_motion_response", U_32, 1, NULL, 0},
    {22, "cb_noise_base", U_32, 1, NULL, 0},
    {23, "cb_still_thresh", U_32, 1, NULL, 0},
};

#define ShpAOrSpatialFilterSelectParams_Count 1
static TUNE_PARAM_s ShpAOrSpatialFilterSelectParams[ShpAOrSpatialFilterSelectParams_Count] = {
    {0, "use_1st_sharpen_not_asf", U_32, 1, NULL, 0},
};

#define VideoMctfAndFinalSharpenParams_Count 1
static TUNE_PARAM_s VideoMctfAndFinalSharpenParams[VideoMctfAndFinalSharpenParams_Count] = {
    {0, "pos_dep", U_8, 1089, NULL, 0},
};

#define AdvanceSpatialFilterParams_Count 36
static TUNE_PARAM_s AdvanceSpatialFilterParams[AdvanceSpatialFilterParams_Count] = {
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
static TUNE_PARAM_s LumaNoiseReductionParams[LumaNoiseReductionParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0 },
    { 1, "sensor_wb_b", U_32, 1, NULL, 0 },
    { 2, "sensor_wb_g", U_32, 1, NULL, 0 },
    { 3, "sensor_wb_r", U_32, 1, NULL, 0 },
    { 4, "strength0", U_32, 1, NULL, 0 },
    { 5, "strength1", U_32, 1, NULL, 0 },
    { 6, "strength2", U_32, 1, NULL, 0 },
    { 7, "strength2_max_change", U_32, 1, NULL, 0 },
};

#define ChromaFilterParams_Count 6
static TUNE_PARAM_s ChromaFilterParams[ChromaFilterParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "noise_level_cb", U_32, 1, NULL, 0},
    { 2, "noise_level_cr", U_32, 1, NULL, 0},
    { 3, "original_blend_strength_cb", U_32, 1, NULL, 0},
    { 4, "original_blend_strength_cr", U_32, 1, NULL, 0},
    { 5, "radius", U_32, 1, NULL, 0}
};

#define WideChromaFilterParams_Count 3
static TUNE_PARAM_s WideChromaFilterParams[WideChromaFilterParams_Count] = {
    { 0, "enable", U_32, 1, NULL, 0},
    { 1, "noise_level_cb", U_32, 1, NULL, 0},
    { 2, "noise_level_cr", U_32, 1, NULL, 0}
};

#define ContrastEnhance_Count 35
static TUNE_PARAM_s ContrastEnhance[ContrastEnhance_Count] = {
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
static TUNE_PARAM_s HdrFrontEndWbgain[HdrFrontEndWbgain_Count] = {
    {0, "r_gain", U_32, 1, NULL, 0},
    {1, "g_gain", U_32, 1, NULL, 0},
    {2, "b_gain", U_32, 1, NULL, 0},
    {3, "ir_gain", U_32, 1, NULL, 0},
    {4, "shutter_ratio", U_32, 1, NULL, 0},
};

#define HdrRawInfo_Count 12
static TUNE_PARAM_s HdrRawInfo[HdrRawInfo_Count] = {
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

#define HdrBlendParams_Count 6
static TUNE_PARAM_s HdrBlendParams[HdrBlendParams_Count] = {
    {0, "enable", U_32, 1, NULL, 0},
    {1, "delta_t0", U_32, 4, NULL, 0},
    {2, "delta_t1", U_32, 4, NULL, 0},
    {3, "flicker_threshold", U_32, 1, NULL, 0},
    {4, "t0_offset", U_32, 4, NULL, 0},
    {5, "t1_offset", U_32, 4, NULL, 0},
};

#define WideChromaFilterCombineParams_Count 10
static TUNE_PARAM_s WideChromaFilterCombineParams[WideChromaFilterCombineParams_Count] = {
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


#define ImgRegs_Count 69

static TUNE_REG_s ImgRegs[ImgRegs_Count] = {
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
    { ITUNER_WARP_COMPENSATION_DZOOM_INTERNAL, "warp_compensation_internal", 1, 0, WarpCompensationDzoomInternalParams_Count, PARAM_STATUS_DEFAULT, WarpCompensationDzoomInternalParams, WarpCompensationDzoomInternalProc, ATTRIBUTE_HIDE },
    { ITUNER_DZOOM, "dzoom", 1, 0, DzoomParams_Count, PARAM_STATUS_DEFAULT, DzoomParams, DzoomProc, 0 },
    { ITUNER_DEFERRED_BLACK_LEVEL, "deferred_black_level", 1, 0, DeferredBlackLevelParams_Count, PARAM_STATUS_DEFAULT, DeferredBlackLevelParams, DeferredBlackLevelProc, 0 },
    { ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION, "li_auto_bad_pixel_correction", 1, 0, DynamicBadPixelCorrectionParams_Count, PARAM_STATUS_DEFAULT, DynamicBadPixelCorrectionParams, DynamicBadPixelCorrectionProc, 0 },
    { ITUNER_CFA_LEAKAGE_FILTER, "li_cfa_leakage_filter", 1, 0, CfaLeakageFilterParams_Count, PARAM_STATUS_DEFAULT, CfaLeakageFilterParams, CfaLeakageFilterProc, 0 },
    { ITUNER_CFA_NOISE_FILTER, "li_cfa_noise_filter", 1, 0, CfaNoiseFilterParams_Count, PARAM_STATUS_DEFAULT, CfaNoiseFilterParams, CfaNoiseFilterProc, 0 },
    { ITUNER_ANTI_ALIASING_STRENGTH, "li_anti_aliasing", 1, 0, AntiAliasingParams_Count, PARAM_STATUS_DEFAULT, AntiAliasingParams, AntiAliasingProc, 0 },
    { ITUNER_BEFORE_CE_WB_GAIN, "before_ce", 1, 0, WbGainParams_Count, PARAM_STATUS_DEFAULT, WbGainParams, BeforeCeWbGainProc, 0 },
    { ITUNER_AFTER_CE_WB_GAIN, "after_ce", 1, 0, WbGainParams_Count, PARAM_STATUS_DEFAULT, WbGainParams, AfterCeWbGainProc, 0 },
    { ITUNER_PRE_CC_GAIN, "pre_cc_gain", 1, 0, PreCcGainParams_Count, PARAM_STATUS_DEFAULT, PreCcGainParams, PreCcGainProc, 0 },
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
};

INT32 TUNE_Rule_Get_Info(TUNE_Rule_Info_t *RuleInfo)
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

static void ituner_param_proc(const TUNE_PARAM_s *pParam, const void *pField)
{
    TUNE_OpMode_e misra_OpMode = ituner_Opmode_Get();
    if (misra_OpMode == ITUNER_DEC) {
        TUNE_Parser_Dec_Proc(pParam, pField);
    } else {
        TUNE_Parser_Enc_Proc(pParam, pField);
    }
}

void ituner_Opmode_Set(TUNE_OpMode_e misra_OpMode)
{
    Opmode = misra_OpMode;
}

TUNE_OpMode_e ituner_Opmode_Get(void)
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

static void System_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
{
    AMBA_ITN_SYSTEM_s Data;
    void *pField[System_Params_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_SystemInfo(&Data);
    pField[0] = &Data.ItunerRev;
    pField[1] = Data.SensorId;
    pField[2] = Data.TuningMode;
    pField[3] = Data.RawPath;
    pField[4] = &Data.RawWidth;
    pField[5] = &Data.RawHeight;
    pField[6] = &Data.RawPitch;
    pField[7] = &Data.RawResolution;
    pField[8] = &Data.RawBayer;
    pField[9] = &Data.MainWidth;
    pField[10] = &Data.MainHeight;
    pField[11] = &Data.InputPicCnt;
    pField[12] = &Data.CompressedRaw;
    pField[13] = &Data.SensorReadoutMode;
    pField[14] = &Data.RawStartX;
    pField[15] = &Data.RawStartY;
    pField[16] = &Data.HSubSampleFactorNum;
    pField[17] = &Data.HSubSampleFactorDen;
    pField[18] = &Data.VSubSampleFactorNum;
    pField[19] = &Data.VSubSampleFactorDen;
    pField[20] = &Data.NumberOfExposures;
    pField[21] = &Data.SensorMode;
    pField[22] = &Data.CompressionOffset;
    pField[23] = &Data.Ability;
    pField[24] = &Data.YuvMode;
    pField[25] = &Data.FlipH;
    pField[26] = &Data.FlipV;
    pField[27] = &Data.FrameNumber;
    pField[28] = &Data.NumberOfFrames;
    Max = System_Params_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_SystemInfo(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus = SKIP_VALID_CHECK;
    }
}


static void Internal_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void AaaFunctionProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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


static void AeInfoProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void WbSimInfoProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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


static void StaticBadPixelCorrectionProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void StaticBadPixelCorrectionInternalProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void VignetteCompensationProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void WarpCompensationProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void DzoomProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void WarpCompensationDzoomInternalProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void SensorInputProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void DeferredBlackLevelProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void DynamicBadPixelCorrectionProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void CfaLeakageFilterProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void AntiAliasingProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void CfaNoiseFilterProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
{
    AMBA_IK_CFA_NOISE_FILTER_s Data;
    void *pField[CfaNoiseFilterParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_CfaNoiseFilter(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.ExtentFine[0];
    pField[2] = &Data.ExtentFine[1];
    pField[3] = &Data.ExtentFine[2];
    pField[4] = &Data.ExtentRegular[0];
    pField[5] = &Data.ExtentRegular[1];
    pField[6] = &Data.ExtentRegular[2];
    pField[7] = &Data.OriginalBlendStr[0];
    pField[8] = &Data.OriginalBlendStr[1];
    pField[9] = &Data.OriginalBlendStr[2];
    pField[10] = &Data.SelectivityFine;
    pField[11] = &Data.SelectivityRegular;
    pField[12] = &Data.StrengthFine[0];
    pField[13] = &Data.StrengthFine[1];
    pField[14] = &Data.StrengthFine[2];
    pField[15] = &Data.DirLevMax[0];
    pField[16] = &Data.DirLevMax[1];
    pField[17] = &Data.DirLevMax[2];
    pField[18] = &Data.DirLevMin[0];
    pField[19] = &Data.DirLevMin[1];
    pField[20] = &Data.DirLevMin[2];
    pField[21] = &Data.DirLevMul[0];
    pField[22] = &Data.DirLevMul[1];
    pField[23] = &Data.DirLevMul[2];
    pField[24] = &Data.DirLevOffset[0];
    pField[25] = &Data.DirLevOffset[1];
    pField[26] = &Data.DirLevOffset[2];
    pField[27] = &Data.DirLevShift[0];
    pField[28] = &Data.DirLevShift[1];
    pField[29] = &Data.DirLevShift[2];
    pField[30] = &Data.LevMul[0];
    pField[31] = &Data.LevMul[1];
    pField[32] = &Data.LevMul[2];
    pField[33] = &Data.LevOffset[0];
    pField[34] = &Data.LevOffset[1];
    pField[35] = &Data.LevOffset[2];
    pField[36] = &Data.LevShift[0];
    pField[37] = &Data.LevShift[1];
    pField[38] = &Data.LevShift[2];
    pField[39] = &Data.MaxNoiseLevel[0];
    pField[40] = &Data.MaxNoiseLevel[1];
    pField[41] = &Data.MaxNoiseLevel[2];
    pField[42] = &Data.MinNoiseLevel[0];
    pField[43] = &Data.MinNoiseLevel[1];
    pField[44] = &Data.MinNoiseLevel[2];
    pField[45] = &Data.ApplyToColorDiffRed;
    pField[46] = &Data.ApplyToColorDiffBlue;
    pField[47] = &Data.OptForRccbRed;
    pField[48] = &Data.OptForRccbBlue;
    pField[49] = &Data.DirectionalEnable;
    pField[50] = &Data.DirectionalHorvertEdgeStrength;
    pField[51] = &Data.DirectionalHorvertStrengthBias;
    pField[52] = &Data.DirectionalHorvertOriginalBlendStrength;
    Max = CfaNoiseFilterParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_CfaNoiseFilter(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
        if(((Idx == 49u) && (Data.DirectionalEnable == 0U))) {
            *ParamStatus |= (0x1ULL << 50);
            *ParamStatus |= (0x1ULL << 51);
            *ParamStatus |= (0x1ULL << 52);
        }
    }
}

static void BeforeCeWbGainProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void AfterCeWbGainProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void PreCcGainProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
{
    AMBA_IK_PRE_CC_GAIN_s Data;
    void *pField[PreCcGainParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_PreCcGain(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.GainR;
    pField[2] = &Data.GainG;
    pField[3] = &Data.GainB;
    Max = PreCcGainParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_PreCcGain(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (((Idx == 0U) && (Data.Enable == 0UL)) ? SKIP_VALID_CHECK : (0x1ULL << Idx));
    }
}

static void ColorCorrectionProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void ToneCurveProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void FrontEndToneCurveProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void RgbToYuvMatrixProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void RgbIrProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void ChromaScaleProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void ChromaMedianFilterProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void DemosaicFilterProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void RgbToY12Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void SharpenBothProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void FinalSharpenBothProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void SharpenNoiseProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void FinalSharpenNoiseProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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



static void SharpenFirProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void SharpenCoringProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void FinalSharpenFirProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void FinalSharpenCoringProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void CoringIndexScaleProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void MinCoringResultProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void MaxCoringResultProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void ScaleCoringProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void FinalCoringIndexScaleProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void FinalMinCoringResultProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void FinalMaxCoringResultProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void FinalScaleCoringProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void VideoMctfProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
{
    AMBA_IK_VIDEO_MCTF_s Data;
    void *pField[VideoMctfParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_VideoMctf(&Data);
    pField[0] = &Data.Enable;
    pField[1] = &Data.YNarrowNonsmoothDetectShift;
    pField[2] = &Data.YNarrowNonsmoothDetectSub;
    pField[3] = &Data.UseLevelBasedTa;
    pField[4] = &Data.CompressionEnable;
    /*3d maxchange*/
    pField[5] = &Data.Y3dMaxchange.High;
    pField[6] = &Data.Y3dMaxchange.HighDelta;
    pField[7] = &Data.Y3dMaxchange.HighStrength;
    pField[8] = &Data.Y3dMaxchange.Low;
    pField[9] = &Data.Y3dMaxchange.LowDelta;
    pField[10] = &Data.Y3dMaxchange.LowStrength;
    pField[11] = &Data.Y3dMaxchange.Method;
    pField[12] = &Data.Y3dMaxchange.MidStrength;
    pField[13] = &Data.Cb3dMaxchange.High;
    pField[14] = &Data.Cb3dMaxchange.HighDelta;
    pField[15] = &Data.Cb3dMaxchange.HighStrength;
    pField[16] = &Data.Cb3dMaxchange.Low;
    pField[17] = &Data.Cb3dMaxchange.LowDelta;
    pField[18] = &Data.Cb3dMaxchange.LowStrength;
    pField[19] = &Data.Cb3dMaxchange.Method;
    pField[20] = &Data.Cb3dMaxchange.MidStrength;
    pField[21] = &Data.Cr3dMaxchange.High;
    pField[22] = &Data.Cr3dMaxchange.HighDelta;
    pField[23] = &Data.Cr3dMaxchange.HighStrength;
    pField[24] = &Data.Cr3dMaxchange.Low;
    pField[25] = &Data.Cr3dMaxchange.LowDelta;
    pField[26] = &Data.Cr3dMaxchange.LowStrength;
    pField[27] = &Data.Cr3dMaxchange.Method;
    pField[28] = &Data.Cr3dMaxchange.MidStrength;
    /*advanced iso*/
    pField[29] = &Data.YAdvancedIsoEnable;
    pField[30] = &Data.YAdvancedIsoMaxChangeMethod;
    pField[31] = &Data.YAdvancedIsoMaxChangeTable;
    pField[32] = &Data.YAdvancedIsoNoiseLevel;
    pField[33] = &Data.YAdvancedIsoSize;
    pField[34] = &Data.CbAdvancedIsoEnable;
    pField[35] = &Data.CbAdvancedIsoMaxChangeMethod;
    pField[36] = &Data.CbAdvancedIsoMaxChangeTable;
    pField[37] = &Data.CbAdvancedIsoNoiseLevel;
    pField[38] = &Data.CbAdvancedIsoSize;
    pField[39] = &Data.CrAdvancedIsoEnable;
    pField[40] = &Data.CrAdvancedIsoMaxChangeMethod;
    pField[41] = &Data.CrAdvancedIsoMaxChangeTable;
    pField[42] = &Data.CrAdvancedIsoNoiseLevel;
    pField[43] = &Data.CrAdvancedIsoSize;
    /*level_based_ta*/
    pField[44] = &Data.YLevelBasedTa.High;
    pField[45] = &Data.YLevelBasedTa.HighDelta;
    pField[46] = &Data.YLevelBasedTa.HighStrength;
    pField[47] = &Data.YLevelBasedTa.Low;
    pField[48] = &Data.YLevelBasedTa.LowDelta;
    pField[49] = &Data.YLevelBasedTa.LowStrength;
    pField[50] = &Data.YLevelBasedTa.Method;
    pField[51] = &Data.YLevelBasedTa.MidStrength;
    pField[52] = &Data.CbLevelBasedTa.High;
    pField[53] = &Data.CbLevelBasedTa.HighDelta;
    pField[54] = &Data.CbLevelBasedTa.HighStrength;
    pField[55] = &Data.CbLevelBasedTa.Low;
    pField[56] = &Data.CbLevelBasedTa.LowDelta;
    pField[57] = &Data.CbLevelBasedTa.LowStrength;
    pField[58] = &Data.CbLevelBasedTa.Method;
    pField[59] = &Data.CbLevelBasedTa.MidStrength;
    pField[60] = &Data.CrLevelBasedTa.High;
    pField[61] = &Data.CrLevelBasedTa.HighDelta;
    pField[62] = &Data.CrLevelBasedTa.HighStrength;
    pField[63] = &Data.CrLevelBasedTa.Low;
    pField[64] = &Data.CrLevelBasedTa.LowDelta;
    pField[65] = &Data.CrLevelBasedTa.LowStrength;
    pField[66] = &Data.CrLevelBasedTa.Method;
    pField[67] = &Data.CrLevelBasedTa.MidStrength;
    /*overall maxchange*/
    pField[68] = &Data.YOverallMaxChange.High;
    pField[69] = &Data.YOverallMaxChange.HighDelta;
    pField[70] = &Data.YOverallMaxChange.Method;
    pField[71] = &Data.YOverallMaxChange.HighStrength;
    pField[72] = &Data.YOverallMaxChange.Low;
    pField[73] = &Data.YOverallMaxChange.LowDelta;
    pField[74] = &Data.YOverallMaxChange.LowStrength;
    pField[75] = &Data.YOverallMaxChange.MidStrength;
    pField[76] = &Data.CbOverallMaxChange.High;
    pField[77] = &Data.CbOverallMaxChange.HighDelta;
    pField[78] = &Data.CbOverallMaxChange.Method;
    pField[79] = &Data.CbOverallMaxChange.HighStrength;
    pField[80] = &Data.CbOverallMaxChange.Low;
    pField[81] = &Data.CbOverallMaxChange.LowDelta;
    pField[82] = &Data.CbOverallMaxChange.LowStrength;
    pField[83] = &Data.CbOverallMaxChange.MidStrength;
    pField[84] = &Data.CrOverallMaxChange.High;
    pField[85] = &Data.CrOverallMaxChange.HighDelta;
    pField[86] = &Data.CrOverallMaxChange.Method;
    pField[87] = &Data.CrOverallMaxChange.HighStrength;
    pField[88] = &Data.CrOverallMaxChange.Low;
    pField[89] = &Data.CrOverallMaxChange.LowDelta;
    pField[90] = &Data.CrOverallMaxChange.LowStrength;
    pField[91] = &Data.CrOverallMaxChange.MidStrength;
    pField[92] = &Data.YSpatBlend.High;
    pField[93] = &Data.YSpatBlend.HighDelta;
    pField[94] = &Data.YSpatBlend.HighStrength;
    pField[95] = &Data.YSpatBlend.Low;
    pField[96] = &Data.YSpatBlend.LowDelta;
    pField[97] = &Data.YSpatBlend.LowStrength;
    pField[98] = &Data.YSpatBlend.Method;
    pField[99] = &Data.YSpatBlend.MidStrength;
    pField[100] = &Data.CbSpatBlend.High;
    pField[101] = &Data.CbSpatBlend.HighDelta;
    pField[102] = &Data.CbSpatBlend.HighStrength;
    pField[103] = &Data.CbSpatBlend.Low;
    pField[104] = &Data.CbSpatBlend.LowDelta;
    pField[105] = &Data.CbSpatBlend.LowStrength;
    pField[106] = &Data.CbSpatBlend.Method;
    pField[107] = &Data.CbSpatBlend.MidStrength;
    pField[108] = &Data.CrSpatBlend.High;
    pField[109] = &Data.CrSpatBlend.HighDelta;
    pField[110] = &Data.CrSpatBlend.HighStrength;
    pField[111] = &Data.CrSpatBlend.Low;
    pField[112] = &Data.CrSpatBlend.LowDelta;
    pField[113] = &Data.CrSpatBlend.LowStrength;
    pField[114] = &Data.CrSpatBlend.Method;
    pField[115] = &Data.CrSpatBlend.MidStrength;
    /*spat_filt_max_smth_change*/
    pField[116] = &Data.YSpatFiltMaxSmthChange.High;
    pField[117] = &Data.YSpatFiltMaxSmthChange.HighDelta;
    pField[118] = &Data.YSpatFiltMaxSmthChange.HighStrength;
    pField[119] = &Data.YSpatFiltMaxSmthChange.Low;
    pField[120] = &Data.YSpatFiltMaxSmthChange.LowDelta;
    pField[121] = &Data.YSpatFiltMaxSmthChange.LowStrength;
    pField[122] = &Data.YSpatFiltMaxSmthChange.Method;
    pField[123] = &Data.YSpatFiltMaxSmthChange.MidStrength;
    pField[124] = &Data.CbSpatFiltMaxSmthChange.High;
    pField[125] = &Data.CbSpatFiltMaxSmthChange.HighDelta;
    pField[126] = &Data.CbSpatFiltMaxSmthChange.HighStrength;
    pField[127] = &Data.CbSpatFiltMaxSmthChange.Low;
    pField[128] = &Data.CbSpatFiltMaxSmthChange.LowDelta;
    pField[129] = &Data.CbSpatFiltMaxSmthChange.LowStrength;
    pField[130] = &Data.CbSpatFiltMaxSmthChange.Method;
    pField[131] = &Data.CbSpatFiltMaxSmthChange.MidStrength;
    pField[132] = &Data.CrSpatFiltMaxSmthChange.High;
    pField[133] = &Data.CrSpatFiltMaxSmthChange.HighDelta;
    pField[134] = &Data.CrSpatFiltMaxSmthChange.HighStrength;
    pField[135] = &Data.CrSpatFiltMaxSmthChange.Low;
    pField[136] = &Data.CrSpatFiltMaxSmthChange.LowDelta;
    pField[137] = &Data.CrSpatFiltMaxSmthChange.LowStrength;
    pField[138] = &Data.CrSpatFiltMaxSmthChange.Method;
    pField[139] = &Data.CrSpatFiltMaxSmthChange.MidStrength;
    /*spat_smth_dir*/
    pField[140] = &Data.YSpatSmthWideEdgeDetect;
    pField[141] = &Data.YSpatSmthEdgeThresh;
    pField[142] = &Data.YSpatSmthDir.High;
    pField[143] = &Data.YSpatSmthDir.HighDelta;
    pField[144] = &Data.YSpatSmthDir.HighStrength;
    pField[145] = &Data.YSpatSmthDir.Low;
    pField[146] = &Data.YSpatSmthDir.LowDelta;
    pField[147] = &Data.YSpatSmthDir.LowStrength;
    pField[148] = &Data.YSpatSmthDir.Method;
    pField[149] = &Data.YSpatSmthDir.MidStrength;
    pField[150] = &Data.CbSpatSmthWideEdgeDetect;
    pField[151] = &Data.CbSpatSmthEdgeThresh;
    pField[152] = &Data.CbSpatSmthDir.High;
    pField[153] = &Data.CbSpatSmthDir.HighDelta;
    pField[154] = &Data.CbSpatSmthDir.HighStrength;
    pField[155] = &Data.CbSpatSmthDir.Low;
    pField[156] = &Data.CbSpatSmthDir.LowDelta;
    pField[157] = &Data.CbSpatSmthDir.LowStrength;
    pField[158] = &Data.CbSpatSmthDir.Method;
    pField[159] = &Data.CbSpatSmthDir.MidStrength;
    pField[160] = &Data.CrSpatSmthWideEdgeDetect;
    pField[161] = &Data.CrSpatSmthEdgeThresh;
    pField[162] = &Data.CrSpatSmthDir.High;
    pField[163] = &Data.CrSpatSmthDir.HighDelta;
    pField[164] = &Data.CrSpatSmthDir.HighStrength;
    pField[165] = &Data.CrSpatSmthDir.Low;
    pField[166] = &Data.CrSpatSmthDir.LowDelta;
    pField[167] = &Data.CrSpatSmthDir.LowStrength;
    pField[168] = &Data.CrSpatSmthDir.Method;
    pField[169] = &Data.CrSpatSmthDir.MidStrength;
    /*spat_smth_iso*/
    pField[170] = &Data.YSpatSmthIso.High;
    pField[171] = &Data.YSpatSmthIso.HighDelta;
    pField[172] = &Data.YSpatSmthIso.HighStrength;
    pField[173] = &Data.YSpatSmthIso.Low;
    pField[174] = &Data.YSpatSmthIso.LowDelta;
    pField[175] = &Data.YSpatSmthIso.LowStrength;
    pField[176] = &Data.YSpatSmthIso.Method;
    pField[177] = &Data.YSpatSmthIso.MidStrength;
    pField[178] = &Data.CbSpatSmthIso.High;
    pField[179] = &Data.CbSpatSmthIso.HighDelta;
    pField[180] = &Data.CbSpatSmthIso.HighStrength;
    pField[181] = &Data.CbSpatSmthIso.Low;
    pField[182] = &Data.CbSpatSmthIso.LowDelta;
    pField[183] = &Data.CbSpatSmthIso.LowStrength;
    pField[184] = &Data.CbSpatSmthIso.Method;
    pField[185] = &Data.CbSpatSmthIso.MidStrength;
    pField[186] = &Data.CrSpatSmthIso.High;
    pField[187] = &Data.CrSpatSmthIso.HighDelta;
    pField[188] = &Data.CrSpatSmthIso.HighStrength;
    pField[189] = &Data.CrSpatSmthIso.Low;
    pField[190] = &Data.CrSpatSmthIso.LowDelta;
    pField[191] = &Data.CrSpatSmthIso.LowStrength;
    pField[192] = &Data.CrSpatSmthIso.Method;
    pField[193] = &Data.CrSpatSmthIso.MidStrength;
    pField[194] = &Data.YSpatSmthDirectDecideT0;
    pField[195] = &Data.YSpatSmthDirectDecideT1;
    pField[196] = &Data.CbSpatSmthDirectDecideT0;
    pField[197] = &Data.CbSpatSmthDirectDecideT1;
    pField[198] = &Data.CrSpatSmthDirectDecideT0;
    pField[199] = &Data.CrSpatSmthDirectDecideT1;
    /*spatial_max_temporal*/
    pField[200] = &Data.YSpatialMaxChange;
    pField[201] = &Data.YSpatialMaxTemporal.High;
    pField[202] = &Data.YSpatialMaxTemporal.HighDelta;
    pField[203] = &Data.YSpatialMaxTemporal.HighStrength;
    pField[204] = &Data.YSpatialMaxTemporal.Low;
    pField[205] = &Data.YSpatialMaxTemporal.LowDelta;
    pField[206] = &Data.YSpatialMaxTemporal.LowStrength;
    pField[207] = &Data.YSpatialMaxTemporal.Method;
    pField[208] = &Data.YSpatialMaxTemporal.MidStrength;
    pField[209] = &Data.CbSpatialMaxChange;
    pField[210] = &Data.CbSpatialMaxTemporal.High;
    pField[211] = &Data.CbSpatialMaxTemporal.HighDelta;
    pField[212] = &Data.CbSpatialMaxTemporal.HighStrength;
    pField[213] = &Data.CbSpatialMaxTemporal.Low;
    pField[214] = &Data.CbSpatialMaxTemporal.LowDelta;
    pField[215] = &Data.CbSpatialMaxTemporal.LowStrength;
    pField[216] = &Data.CbSpatialMaxTemporal.Method;
    pField[217] = &Data.CbSpatialMaxTemporal.MidStrength;
    pField[218] = &Data.CrSpatialMaxChange;
    pField[219] = &Data.CrSpatialMaxTemporal.High;
    pField[220] = &Data.CrSpatialMaxTemporal.HighDelta;
    pField[221] = &Data.CrSpatialMaxTemporal.HighStrength;
    pField[222] = &Data.CrSpatialMaxTemporal.Low;
    pField[223] = &Data.CrSpatialMaxTemporal.LowDelta;
    pField[224] = &Data.CrSpatialMaxTemporal.LowStrength;
    pField[225] = &Data.CrSpatialMaxTemporal.Method;
    pField[226] = &Data.CrSpatialMaxTemporal.MidStrength;
    /*temporal_min_target*/
    pField[227] = &Data.YCurve.TemporalAlpha0;
    pField[228] = &Data.YCurve.TemporalAlpha1;
    pField[229] = &Data.YCurve.TemporalAlpha2;
    pField[230] = &Data.YCurve.TemporalAlpha3;
    pField[231] = &Data.YCurve.TemporalT0;
    pField[232] = &Data.YCurve.TemporalT1;
    pField[233] = &Data.YCurve.TemporalT2;
    pField[234] = &Data.YCurve.TemporalT3;
    pField[235] = &Data.YTemporalMaxChange;
    pField[236] = &Data.YTemporalMinTarget.High;
    pField[237] = &Data.YTemporalMinTarget.HighDelta;
    pField[238] = &Data.YTemporalMinTarget.HighStrength;
    pField[239] = &Data.YTemporalMinTarget.Low;
    pField[240] = &Data.YTemporalMinTarget.LowDelta;
    pField[241] = &Data.YTemporalMinTarget.LowStrength;
    pField[242] = &Data.YTemporalMinTarget.Method;
    pField[243] = &Data.YTemporalMinTarget.MidStrength;
    pField[244] = &Data.CbCurve.TemporalAlpha0;
    pField[245] = &Data.CbCurve.TemporalAlpha1;
    pField[246] = &Data.CbCurve.TemporalAlpha2;
    pField[247] = &Data.CbCurve.TemporalAlpha3;
    pField[248] = &Data.CbCurve.TemporalT0;
    pField[249] = &Data.CbCurve.TemporalT1;
    pField[250] = &Data.CbCurve.TemporalT2;
    pField[251] = &Data.CbCurve.TemporalT3;
    pField[252] = &Data.CbTemporalMaxChange;
    pField[253] = &Data.CbTemporalMinTarget.High;
    pField[254] = &Data.CbTemporalMinTarget.HighDelta;
    pField[255] = &Data.CbTemporalMinTarget.HighStrength;
    pField[256] = &Data.CbTemporalMinTarget.Low;
    pField[257] = &Data.CbTemporalMinTarget.LowDelta;
    pField[258] = &Data.CbTemporalMinTarget.LowStrength;
    pField[259] = &Data.CbTemporalMinTarget.Method;
    pField[260] = &Data.CbTemporalMinTarget.MidStrength;
    pField[261] = &Data.CrCurve.TemporalAlpha0;
    pField[262] = &Data.CrCurve.TemporalAlpha1;
    pField[263] = &Data.CrCurve.TemporalAlpha2;
    pField[264] = &Data.CrCurve.TemporalAlpha3;
    pField[265] = &Data.CrCurve.TemporalT0;
    pField[266] = &Data.CrCurve.TemporalT1;
    pField[267] = &Data.CrCurve.TemporalT2;
    pField[268] = &Data.CrCurve.TemporalT3;
    pField[269] = &Data.CrTemporalMaxChange;
    pField[270] = &Data.CrTemporalMinTarget.High;
    pField[271] = &Data.CrTemporalMinTarget.HighDelta;
    pField[272] = &Data.CrTemporalMinTarget.HighStrength;
    pField[273] = &Data.CrTemporalMinTarget.Low;
    pField[274] = &Data.CrTemporalMinTarget.LowDelta;
    pField[275] = &Data.CrTemporalMinTarget.LowStrength;
    pField[276] = &Data.CrTemporalMinTarget.Method;
    pField[277] = &Data.CrTemporalMinTarget.MidStrength;
    pField[278] = &Data.YTemporalGhostPrevent;
    pField[279] = &Data.CbTemporalGhostPrevent;
    pField[280] = &Data.CrTemporalGhostPrevent;
    pField[281] = &Data.CompressionBitRateLuma;
    pField[282] = &Data.CompressionBitRateChroma;
    pField[283] = &Data.CompressionDitherDisable;
    pField[284] = &Data.YTemporalArtifactGuard;
    pField[285] = &Data.CbTemporalArtifactGuard;
    pField[286] = &Data.CrTemporalArtifactGuard;
    pField[287] = &Data.YSpatialWeighting;
    pField[288] = &Data.CbSpatialWeighting;
    pField[289] = &Data.CrSpatialWeighting;
    pField[290] = &Data.YStrength3d;
    pField[291] = &Data.CbStrength3d;
    pField[292] = &Data.CrStrength3d;
    pField[293] = &Data.YCombineStrength;

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

static void VideoMctfLevelProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void VideoMctfTemporalAdjustProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
{
    AMBA_IK_VIDEO_MCTF_TA_s Data;
    void *pField[VideoMctfTemporalAdjustParams_Count];
    UINT32 Idx;
    INT32 Max;
    AmbaItuner_Get_VideoMctfTemporalAdjust(&Data);
    pField[0] = &Data.LevAdjustHigh;
    pField[1] = &Data.LevAdjustHighDelta;
    pField[2] = &Data.LevAdjustHighStrength;
    pField[3] = &Data.LevAdjustLow;
    pField[4] = &Data.LevAdjustLowDelta;
    pField[5] = &Data.LevAdjustLowStrength;
    pField[6] = &Data.LevAdjustMidStrength;
    pField[7] = &Data.SlowMoSensitivity;
    pField[8] = &Data.ScoreNoiseRobust;
    pField[9] = &Data.YMctfTa.Max;
    pField[10] = &Data.YMctfTa.Min;
    pField[11] = &Data.YMctfTa.MotionResponse;
    pField[12] = &Data.YMctfTa.NoiseBase;
    pField[13] = &Data.YMctfTa.StillThresh;
    pField[14] = &Data.CrMctfTa.Max;
    pField[15] = &Data.CrMctfTa.Min;
    pField[16] = &Data.CrMctfTa.MotionResponse;
    pField[17] = &Data.CrMctfTa.NoiseBase;
    pField[18] = &Data.CrMctfTa.StillThresh;
    pField[19] = &Data.CbMctfTa.Max;
    pField[20] = &Data.CbMctfTa.Min;
    pField[21] = &Data.CbMctfTa.MotionResponse;
    pField[22] = &Data.CbMctfTa.NoiseBase;
    pField[23] = &Data.CbMctfTa.StillThresh;
    Max = VideoMctfTemporalAdjustParams_Count - 1;
    Idx = ituner_CheckIndex(Index, Max);
    ituner_param_proc(pParam, pField[Idx]);
    AmbaItuner_Set_VideoMctfTemporalAdjust(&Data);
    if (ParamStatus != NULL) {
        *ParamStatus |= (0x1ULL << Idx);
    }
}

static void VideoMctfAndFinalSharpenProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void ShpAOrSpatialFilterSelectproc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void AdvanceSpatialFilterProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void LumaNoiseReductionProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void ChromaFilterProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void GbGrMismatchProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void ChromaAberrationProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void ChromaAberrationInternalProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void ContrastEnhance_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void HdrBlend_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void HdrFrontEndWbgainExp0_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void HdrFrontEndWbgainExp1_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void HdrFrontEndWbgainExp2_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void HdrRawInfo_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void HdrStaticBlackLevelExp0_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void HdrStaticBlackLevelExp1_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void HdrStaticBlackLevelExp2_Proc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void WideChromaFilterProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void WideChromaFilterCombineProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void DummyProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

static void ActiveProc(INT32 Index, UINT64 *ParamStatus, const TUNE_PARAM_s *pParam)
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

