/**
 *  @file RefCV.c
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
 *  @details refcode CV
 *
 */
#if defined(CONFIG_BUILD_CV_THREADX)
#include "AmbaIntrinsics.h"
#endif
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaINT.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaMMU.h"
#include "AmbaFS.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"
#include "AmbaShell.h"
#include "cvapi_visutil.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_ambacv_accelerator.h"
#include "RefCV_Common.h"
#include "cvapi_flexidag_unittest.h"

#define MAX_THREAD_NUM              1U
#define MAX_INPUTFILE_NUM           8U
#define MAX_OUTPUTFILE_NUM          4U
#define CHECK_MAX                   ACC_DAG_ID_MAX

static UINT32 loop_max;

typedef struct {
    UINT32                          id;
    UINT32                          num_runs;
    UINT32                          cur_runs;
    UINT32                          num_err;
    UINT32                          sleep_usec;
    UINT32                          RunFlag;
    UINT32                          blocking_run;
    REF_CV_HANDLE_s                 RefCV_handle;
    flexidag_memblk_t               input[MAX_OUTPUTFILE_NUM];
} REF_CV_UT_PARAMS_s;

static REF_CV_UT_PARAMS_s ut_thread_params[MAX_THREAD_NUM];
typedef uint32_t (func_t)(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);

typedef struct {
    uint32_t ID;
    char input_file_path[128UL];
    uint32_t number_of_input;
    char golden_file_path[128UL];
    uint32_t number_of_output;
    uint32_t output_pitch;
    uint32_t output_offset;//2D:(output_pitch*offsetY)+(offsetX)
    func_t *AccFunc;
    uint32_t supported;
} FlexiBinInfo_s;

#if defined(CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_CV22FS)
static FlexiBinInfo_s regression_table_info[] = {
    {ACC_RESIZE_Y_U8_640x360_1280x720, "\\AccRegression\\00_resize_Y_u8_640x360_1280x720\\input", 2, "\\AccRegression\\00_resize_Y_u8_640x360_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunResizeYU8NHD2HD, 1},
    {ACC_RESIZE_Y_U8_420_1280x720_640x360, "\\AccRegression\\01_resize_Y_u8_1280x720_640x360\\input", 2, "\\AccRegression\\01_resize_Y_u8_1280x720_640x360\\output", 1, 0, 0, (func_t *)AmbaACC_RunResizeYU8HD2NHD, 1},
    {ACC_RESIZE_UV_U8_420_640x360_1280x720, "\\AccRegression\\02_resize_UV_u8_640x360_1280x720\\input", 2, "\\AccRegression\\02_resize_UV_u8_640x360_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunResizeUVU8NHD2HD, 1},
    {ACC_RESIZE_UV_U8_420_1280x720_640x360, "\\AccRegression\\03_resize_UV_u8_1280x720_640x360\\input", 2, "\\AccRegression\\03_resize_UV_u8_1280x720_640x360\\output", 1, 0, 0, (func_t *)AmbaACC_RunResizeUVU8HD2NHD, 1},
    {ACC_MIN_MAX_LOC_U8_1280x720, "\\AccRegression\\04_minmax_loc_u8_1280x720\\input", 1, "\\AccRegression\\04_minmax_loc_u8_1280x720\\output", 4, 0, 0, (func_t *)AmbaACC_RunMinMaxLocU8HD, 0},
    {ACC_MIN_MAX_LOC_U16_1280x720, "\\AccRegression\\05_minmax_loc_u16_1280x720\\input", 1, "\\AccRegression\\05_minmax_loc_u16_1280x720\\output", 4, 0, 0, (func_t *)AmbaACC_RunMinMaxLocU16HD, 0},
    {ACC_AND_U8_TWO_POW_19, "\\AccRegression\\06_and_u8_2pow19\\input", 2, "\\AccRegression\\06_and_u8_2pow19\\output", 1, 0, 0, (func_t *)AmbaACC_RunAndU8TwoPow19, 1},
    {ACC_AND_U8_TWO_POW_17, "\\AccRegression\\07_and_u8_2pow17\\input", 2, "\\AccRegression\\07_and_u8_2pow17\\output", 1, 0, 0, (func_t *)AmbaACC_RunAndU8TwoPow17, 1},
    {ACC_AND_U8_TWO_POW_15, "\\AccRegression\\08_and_u8_2pow15\\input", 2, "\\AccRegression\\08_and_u8_2pow15\\output", 1, 0, 0, (func_t *)AmbaACC_RunAndU8TwoPow15, 1},
    {ACC_CMP_NOT_EQUAL_U8_TWO_POW_19, "\\AccRegression\\09_neq_u8_2pow19\\input", 2, "\\AccRegression\\09_neq_u8_2pow19\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpNeqU8TwoPow19, 1},
    {ACC_CMP_NOT_EQUAL_U8_TWO_POW_17, "\\AccRegression\\10_neq_u8_2pow17\\input", 2, "\\AccRegression\\10_neq_u8_2pow17\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpNeqU8TwoPow17, 1},
    {ACC_CMP_NOT_EQUAL_U8_TWO_POW_15, "\\AccRegression\\11_neq_u8_2pow15\\input", 2, "\\AccRegression\\11_neq_u8_2pow15\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpNeqU8TwoPow15, 1},
    {ACC_CMP_GREATER_S8_TWO_POW_19, "\\AccRegression\\12_grt_s8_2pow19\\input", 2, "\\AccRegression\\12_grt_s8_2pow19\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpGtrS8TwoPow19, 1},
    {ACC_CMP_GREATER_S8_TWO_POW_17, "\\AccRegression\\13_grt_s8_2pow17\\input", 2, "\\AccRegression\\13_grt_s8_2pow17\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpGtrS8TwoPow17, 1},
    {ACC_CMP_GREATER_S8_TWO_POW_15, "\\AccRegression\\14_grt_s8_2pow15\\input", 2, "\\AccRegression\\14_grt_s8_2pow15\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpGtrS8TwoPow15, 1},
    {ACC_CMP_NEQ_U16_TWO_POW_18, "\\AccRegression\\15_neq_u16_2pow18\\input", 2, "\\AccRegression\\15_neq_u16_2pow18\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpNeqU16TwoPow18, 0},
    {ACC_CMP_NEQ_U16_TWO_POW_16, "\\AccRegression\\16_neq_u16_2pow16\\input", 2, "\\AccRegression\\16_neq_u16_2pow16\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpNeqU16TwoPow16, 0},
    {ACC_CMP_NEQ_U16_TWO_POW_14, "\\AccRegression\\17_neq_u16_2pow14\\input", 2, "\\AccRegression\\17_neq_u16_2pow14\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpNeqU16TwoPow14, 0},
    {ACC_DIVIDE_U8_TWO_POW_18, "\\AccRegression\\18_div_u8_2pow18\\input", 2, "\\AccRegression\\18_div_u8_2pow18\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideU8TwoPow18, 1},
    {ACC_DIVIDE_U8_TWO_POW_16, "\\AccRegression\\19_div_u8_2pow16\\input", 2, "\\AccRegression\\19_div_u8_2pow16\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideU8TwoPow16, 1},
    {ACC_DIVIDE_U8_TWO_POW_14, "\\AccRegression\\20_div_u8_2pow14\\input", 2, "\\AccRegression\\20_div_u8_2pow14\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideU8TwoPow14, 1},
    {ACC_DIVIDE_U16_TWO_POW_17, "\\AccRegression\\21_div_u16_2pow17\\input", 2, "\\AccRegression\\21_div_u16_2pow17\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideU16TwoPow17, 1},
    {ACC_DIVIDE_U16_TWO_POW_15, "\\AccRegression\\22_div_u16_2pow15\\input", 2, "\\AccRegression\\22_div_u16_2pow15\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideU16TwoPow15, 1},
    {ACC_DIVIDE_U16_TWO_POW_13, "\\AccRegression\\23_div_u16_2pow13\\input", 2, "\\AccRegression\\23_div_u16_2pow13\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideU16TwoPow13, 1},
    {ACC_DIVIDE_S16_TWO_POW_17, "\\AccRegression\\24_div_s16_2pow17\\input", 2, "\\AccRegression\\24_div_s16_2pow17\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideS16TwoPow17, 1},
    {ACC_DIVIDE_S16_TWO_POW_15, "\\AccRegression\\25_div_s16_2pow15\\input", 2, "\\AccRegression\\25_div_s16_2pow15\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideS16TwoPow15, 1},
    {ACC_DIVIDE_S16_TWO_POW_13, "\\AccRegression\\26_div_s16_2pow13\\input", 2, "\\AccRegression\\26_div_s16_2pow13\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideS16TwoPow13, 1},
    {ACC_THRES_BINARY_U8_TWO_POW18, "\\AccRegression\\27_thres_binary_u8_2pow18\\input", 1, "\\AccRegression\\27_thres_binary_u8_2pow18\\output", 1, 0, 0, (func_t *)AmbaACC_RunThresBinU8TwoPow18, 1},
    {ACC_THRES_BINARY_U8_TWO_POW14, "\\AccRegression\\28_thres_binary_u8_2pow14\\input", 1, "\\AccRegression\\28_thres_binary_u8_2pow14\\output", 1, 0, 0, (func_t *)AmbaACC_RunThresBinU8TwoPow14, 1},
    {ACC_THRES_BINARY_F32_TWO_POW17, "\\AccRegression\\29_thres_binary_f32_2pow17\\input", 1, "\\AccRegression\\29_thres_binary_f32_2pow17\\output", 1, 0, 0, (func_t *)AmbaACC_RunThresBinF32TwoPow17, 0},
    {ACC_THRES_BINARY_F32_TWO_POW13, "\\AccRegression\\30_thres_binary_f32_2pow13\\input", 1, "\\AccRegression\\30_thres_binary_f32_2pow13\\output", 1, 0, 0, (func_t *)AmbaACC_RunThresBinF32TwoPow13, 0},
    {ACC_THRES_TOZERO_F32_TWO_POW17, "\\AccRegression\\31_thres_tozero_f32_2pow17\\input", 1, "\\AccRegression\\31_thres_tozero_f32_2pow17\\output", 1, 0, 0, (func_t *)AmbaACC_RunThresTo0F32TwoPow17, 0},
    {ACC_THRES_TOZERO_F32_TWO_POW13, "\\AccRegression\\32_thres_tozero_f32_2pow13\\input", 1, "\\AccRegression\\32_thres_tozero_f32_2pow13\\output", 1, 0, 0, (func_t *)AmbaACC_RunThresTo0F32TwoPow13, 0},
    {ACC_REDUCE_AVG_H_U8_1280x720, "\\AccRegression\\33_reduce_avg_h_u8_1280x720\\input", 1, "\\AccRegression\\33_reduce_avg_h_u8_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunReduceAvgHeightU8HD, 1},
    {ACC_REDUCE_SUM_H_U8_1280x720, "\\AccRegression\\34_reduce_sum_h_u8_1280x720\\input", 1, "\\AccRegression\\34_reduce_sum_h_u8_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunReduceSumHeightU8HD, 1},
    {ACC_REDUCE_MIN_W_U8_1280x720, "\\AccRegression\\35_reduce_min_w_u8_1280x720\\input", 1, "\\AccRegression\\35_reduce_min_w_u8_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunReduceMinWidthU8HD, 1},
    {ACC_REDUCE_MAX_W_U8_1280x720, "\\AccRegression\\36_reduce_max_w_u8_1280x720\\input", 1, "\\AccRegression\\36_reduce_max_w_u8_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunReduceMaxWidthU8HD, 1},
    {ACC_SPLIT_U8_1280x720x3, "\\AccRegression\\37_split_u8_1280x720x3\\input", 1, "\\AccRegression\\37_split_u8_1280x720x3\\output", 3, 0, 0, (func_t *)AmbaACC_RunSplitU8HDC3, 1},
    {ACC_ERODE_U8_640x480, "\\AccRegression\\38_erode_u8_640x480\\input", 2, "\\AccRegression\\38_erode_u8_640x480\\output", 1, 0, 0, (func_t *)AmbaACC_RunErodeU8VGA, 1},
    {ACC_TRANSFORM_U8_640x480, "\\AccRegression\\39_transform_u8_640x480\\input", 2, "\\AccRegression\\39_transform_u8_640x480\\output", 1, 0, 0, (func_t *)AmbaACC_RunTransformU8VGA, 1},
    {ACC_DILATE_U8_640x480, "\\AccRegression\\40_dilate_u8_640x480\\input", 2, "\\AccRegression\\40_dilate_u8_640x480\\output", 1, 0, 0, (func_t *)AmbaACC_RunDilateU8VGA, 1},
    {ACC_MERGE_U8_1280x720x3, "\\AccRegression\\41_merge_u8_1280x720x3\\input", 3, "\\AccRegression\\41_merge_u8_1280x720x3\\output", 1, 0, 0, (func_t *)AmbaACC_RunMergeU8HDC3, 1},
    {ACC_MIXCHANNELS_U8_640x480, "\\AccRegression\\42_mixchannels_u8_640x480\\input", 1, "\\AccRegression\\42_mixchannels_u8_640x480\\output", 1, 0, 0, (func_t *)AmbaACC_RunMixChannelsBgra2RgbU8VGA, 1},
    {ACC_DIV_F32_2496, "\\AccRegression\\43_div_f32_2496\\input", 2, "\\AccRegression\\43_div_f32_2496\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivF32PAD32_46x54, 1},
    {ACC_LOG_U8_TWO_POW19, "\\AccRegression\\44_log_u8_pow19\\input", 1, "\\AccRegression\\44_log_u8_pow19\\output", 1, 0, 0, (func_t *)AmbaACC_RunLogU8TwoPow19, 1},
    {ACC_LOG_U8_TWO_POW17, "\\AccRegression\\45_log_u8_pow17\\input", 1, "\\AccRegression\\45_log_u8_pow17\\output", 1, 0, 0, (func_t *)AmbaACC_RunLogU8TwoPow17, 1},
    {ACC_LOG_U8_TWO_POW15, "\\AccRegression\\46_log_u8_pow15\\input", 1, "\\AccRegression\\46_log_u8_pow15\\output", 1, 0, 0, (func_t *)AmbaACC_RunLogU8TwoPow15, 1},
    {ACC_CONV_U8_VGA, "\\AccRegression\\47_conv_u8_640x480\\input", 2, "\\AccRegression\\47_conv_u8_640x480\\output", 1, 0, 0, (func_t *)AmbaACC_RunConvU8VGA, 1},
    {ACC_CONV_U8_HD, "\\AccRegression\\48_conv_u8_1280x720\\input", 2, "\\AccRegression\\48_conv_u8_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunConvU8HD, 1},
    {ACC_FINDNZ_U8_320x180, "\\AccRegression\\49_findNZ_u8_320x180\\input", 1, "\\AccRegression\\49_findNZ_u8_320x180\\output", 2, 0, 0, (func_t *)AmbaACC_RunFindNZU8_320x180, 1},
    {ACC_YUV2RGB_U8_HD, "\\AccRegression\\50_yuv2rgb_u8_1280x720\\input", 2, "\\AccRegression\\50_yuv2rgb_u8_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunYUV2RGBU8HD, 1},
    //Not support now
    {ACC_GAUSSIAN_U8_VGA, "\\AccRegression\\47_conv_u8_640x480\\input", 2, "\\AccRegression\\47_conv_u8_640x480\\output", 1, 0, 0, (func_t *)AmbaACC_RunGaussianBlurU8VGA, 0},
    {ACC_GAUSSIAN_U8_HD, "\\AccRegression\\48_conv_u8_1280x720\\input", 2, "\\AccRegression\\48_conv_u8_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunGaussianBlurU8HD, 0},
    {ACC_WARP_Y_640x480, "\\AccRegression\\53_warp_rotate_y_640x480\\input", 2, "\\AccRegression\\53_warp_rotate_y_640x480\\output", 1, 0, 0, (func_t *)AmbaACC_RunWarpRotateY640x480, 0},
    {ACC_WARP_UV_640x480, "\\AccRegression\\54_warp_rotate_uv_640x480\\input", 2, "\\AccRegression\\54_warp_rotate_uv_640x480\\output", 1, 0, 0, (func_t *)AmbaACC_RunWarpRotateUV640x480, 0},
    //{ACC_ScaleOSD, "\\AccRegression\\55_scale_OSD\\input", 1, "\\AccRegression\\55_scale_OSD\\output", 1, 1920, (1920*4*48)+(32*4), (func_t *)AmbaACC_RunScaleOSD, 0},
    {ACC_ERODE_640x480_RECT3X3, "\\AccRegression\\56_erode_640x480_rect3x3\\input", 1, "\\AccRegression\\56_erode_640x480_rect3x3\\output", 1, 0, 0, (func_t *)AmbaACC_RunErodeU8VGARect3x3, 1},
    {ACC_DILATE_640x480_RECT3X3, "\\AccRegression\\57_dilate_640x480_rect3x3\\input", 1, "\\AccRegression\\57_dilate_640x480_rect3x3\\output", 1, 0, 0, (func_t *)AmbaACC_RunDilateU8VGARect3x3, 1},
    {ACC_RGB2YUV_U8_HD, "\\AccRegression\\58_rgb2yuv_u8_1280x720\\input", 1, "\\AccRegression\\58_rgb2yuv_u8_1280x720\\output", 2, 0, 0, (func_t *)AmbaACC_RunRGB2YUVU8HD, 1},
    {ACC_GEMM, "\\AccRegression\\59_gemm\\input", 3, "\\AccRegression\\59_gemm\\output", 1, 0, 0, (func_t *)AmbaACC_RunGEMM, 1},

};
#else
static FlexiBinInfo_s regression_table_info[] = {
    {ACC_RESIZE_Y_U8_640x360_1280x720, "\\AccRegression\\00_resize_Y_u8_640x360_1280x720\\input", 2, "\\AccRegression\\00_resize_Y_u8_640x360_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunResizeYU8NHD2HD, 1},
    {ACC_RESIZE_Y_U8_420_1280x720_640x360, "\\AccRegression\\01_resize_Y_u8_1280x720_640x360\\input", 2, "\\AccRegression\\01_resize_Y_u8_1280x720_640x360\\output", 1, 0, 0, (func_t *)AmbaACC_RunResizeYU8HD2NHD, 1},
    {ACC_RESIZE_UV_U8_420_640x360_1280x720, "\\AccRegression\\02_resize_UV_u8_640x360_1280x720\\input", 2, "\\AccRegression\\02_resize_UV_u8_640x360_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunResizeUVU8NHD2HD, 1},
    {ACC_RESIZE_UV_U8_420_1280x720_640x360, "\\AccRegression\\03_resize_UV_u8_1280x720_640x360\\input", 2, "\\AccRegression\\03_resize_UV_u8_1280x720_640x360\\output", 1, 0, 0, (func_t *)AmbaACC_RunResizeUVU8HD2NHD, 1},
#if defined(CONFIG_SOC_CV2FS)//Not suppoted
    {ACC_MIN_MAX_LOC_U8_1280x720, "\\AccRegression\\04_minmax_loc_u8_1280x720\\input", 1, "\\AccRegression\\04_minmax_loc_u8_1280x720\\output", 4, 0, 0, (func_t *)AmbaACC_RunMinMaxLocU8HD, 0},
    {ACC_MIN_MAX_LOC_U16_1280x720, "\\AccRegression\\05_minmax_loc_u16_1280x720\\input", 1, "\\AccRegression\\05_minmax_loc_u16_1280x720\\output", 4, 0, 0, (func_t *)AmbaACC_RunMinMaxLocU16HD, 0},
#else
    {ACC_MIN_MAX_LOC_U8_1280x720, "\\AccRegression\\04_minmax_loc_u8_1280x720\\input", 1, "\\AccRegression\\04_minmax_loc_u8_1280x720\\output", 4, 0, 0, (func_t *)AmbaACC_RunMinMaxLocU8HD, 1},
    {ACC_MIN_MAX_LOC_U16_1280x720, "\\AccRegression\\05_minmax_loc_u16_1280x720\\input", 1, "\\AccRegression\\05_minmax_loc_u16_1280x720\\output", 4, 0, 0, (func_t *)AmbaACC_RunMinMaxLocU16HD, 1},
#endif
    {ACC_AND_U8_TWO_POW_19, "\\AccRegression\\06_and_u8_2pow19\\input", 2, "\\AccRegression\\06_and_u8_2pow19\\output", 1, 0, 0, (func_t *)AmbaACC_RunAndU8TwoPow19, 1},
    {ACC_AND_U8_TWO_POW_17, "\\AccRegression\\07_and_u8_2pow17\\input", 2, "\\AccRegression\\07_and_u8_2pow17\\output", 1, 0, 0, (func_t *)AmbaACC_RunAndU8TwoPow17, 1},
    {ACC_AND_U8_TWO_POW_15, "\\AccRegression\\08_and_u8_2pow15\\input", 2, "\\AccRegression\\08_and_u8_2pow15\\output", 1, 0, 0, (func_t *)AmbaACC_RunAndU8TwoPow15, 1},
    {ACC_CMP_NOT_EQUAL_U8_TWO_POW_19, "\\AccRegression\\09_neq_u8_2pow19\\input", 2, "\\AccRegression\\09_neq_u8_2pow19\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpNeqU8TwoPow19, 1},
    {ACC_CMP_NOT_EQUAL_U8_TWO_POW_17, "\\AccRegression\\10_neq_u8_2pow17\\input", 2, "\\AccRegression\\10_neq_u8_2pow17\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpNeqU8TwoPow17, 1},
    {ACC_CMP_NOT_EQUAL_U8_TWO_POW_15, "\\AccRegression\\11_neq_u8_2pow15\\input", 2, "\\AccRegression\\11_neq_u8_2pow15\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpNeqU8TwoPow15, 1},
    {ACC_CMP_GREATER_S8_TWO_POW_19, "\\AccRegression\\12_grt_s8_2pow19\\input", 2, "\\AccRegression\\12_grt_s8_2pow19\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpGtrS8TwoPow19, 1},
    {ACC_CMP_GREATER_S8_TWO_POW_17, "\\AccRegression\\13_grt_s8_2pow17\\input", 2, "\\AccRegression\\13_grt_s8_2pow17\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpGtrS8TwoPow17, 1},
    {ACC_CMP_GREATER_S8_TWO_POW_15, "\\AccRegression\\14_grt_s8_2pow15\\input", 2, "\\AccRegression\\14_grt_s8_2pow15\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpGtrS8TwoPow15, 1},
    {ACC_CMP_NEQ_U16_TWO_POW_18, "\\AccRegression\\15_neq_u16_2pow18\\input", 2, "\\AccRegression\\15_neq_u16_2pow18\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpNeqU16TwoPow18, 1},
    {ACC_CMP_NEQ_U16_TWO_POW_16, "\\AccRegression\\16_neq_u16_2pow16\\input", 2, "\\AccRegression\\16_neq_u16_2pow16\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpNeqU16TwoPow16, 1},
    {ACC_CMP_NEQ_U16_TWO_POW_14, "\\AccRegression\\17_neq_u16_2pow14\\input", 2, "\\AccRegression\\17_neq_u16_2pow14\\output", 1, 0, 0, (func_t *)AmbaACC_RunCmpNeqU16TwoPow14, 1},
    {ACC_DIVIDE_U8_TWO_POW_18, "\\AccRegression\\18_div_u8_2pow18\\input", 2, "\\AccRegression\\18_div_u8_2pow18\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideU8TwoPow18, 1},
    {ACC_DIVIDE_U8_TWO_POW_16, "\\AccRegression\\19_div_u8_2pow16\\input", 2, "\\AccRegression\\19_div_u8_2pow16\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideU8TwoPow16, 1},
    {ACC_DIVIDE_U8_TWO_POW_14, "\\AccRegression\\20_div_u8_2pow14\\input", 2, "\\AccRegression\\20_div_u8_2pow14\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideU8TwoPow14, 1},
    {ACC_DIVIDE_U16_TWO_POW_17, "\\AccRegression\\21_div_u16_2pow17\\input", 2, "\\AccRegression\\21_div_u16_2pow17\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideU16TwoPow17, 1},
    {ACC_DIVIDE_U16_TWO_POW_15, "\\AccRegression\\22_div_u16_2pow15\\input", 2, "\\AccRegression\\22_div_u16_2pow15\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideU16TwoPow15, 1},
    {ACC_DIVIDE_U16_TWO_POW_13, "\\AccRegression\\23_div_u16_2pow13\\input", 2, "\\AccRegression\\23_div_u16_2pow13\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideU16TwoPow13, 1},
    {ACC_DIVIDE_S16_TWO_POW_17, "\\AccRegression\\24_div_s16_2pow17\\input", 2, "\\AccRegression\\24_div_s16_2pow17\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideS16TwoPow17, 1},
    {ACC_DIVIDE_S16_TWO_POW_15, "\\AccRegression\\25_div_s16_2pow15\\input", 2, "\\AccRegression\\25_div_s16_2pow15\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideS16TwoPow15, 1},
    {ACC_DIVIDE_S16_TWO_POW_13, "\\AccRegression\\26_div_s16_2pow13\\input", 2, "\\AccRegression\\26_div_s16_2pow13\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivideS16TwoPow13, 1},
    {ACC_THRES_BINARY_U8_TWO_POW18, "\\AccRegression\\27_thres_binary_u8_2pow18\\input", 1, "\\AccRegression\\27_thres_binary_u8_2pow18\\output", 1, 0, 0, (func_t *)AmbaACC_RunThresBinU8TwoPow18, 1},
    {ACC_THRES_BINARY_U8_TWO_POW14, "\\AccRegression\\28_thres_binary_u8_2pow14\\input", 1, "\\AccRegression\\28_thres_binary_u8_2pow14\\output", 1, 0, 0, (func_t *)AmbaACC_RunThresBinU8TwoPow14, 1},
    {ACC_THRES_BINARY_F32_TWO_POW17, "\\AccRegression\\29_thres_binary_f32_2pow17\\input", 1, "\\AccRegression\\29_thres_binary_f32_2pow17\\output", 1, 0, 0, (func_t *)AmbaACC_RunThresBinF32TwoPow17, 1},
    {ACC_THRES_BINARY_F32_TWO_POW13, "\\AccRegression\\30_thres_binary_f32_2pow13\\input", 1, "\\AccRegression\\30_thres_binary_f32_2pow13\\output", 1, 0, 0, (func_t *)AmbaACC_RunThresBinF32TwoPow13, 1},
    {ACC_THRES_TOZERO_F32_TWO_POW17, "\\AccRegression\\31_thres_tozero_f32_2pow17\\input", 1, "\\AccRegression\\31_thres_tozero_f32_2pow17\\output", 1, 0, 0, (func_t *)AmbaACC_RunThresTo0F32TwoPow17, 1},
    {ACC_THRES_TOZERO_F32_TWO_POW13, "\\AccRegression\\32_thres_tozero_f32_2pow13\\input", 1, "\\AccRegression\\32_thres_tozero_f32_2pow13\\output", 1, 0, 0, (func_t *)AmbaACC_RunThresTo0F32TwoPow13, 1},
    {ACC_REDUCE_AVG_H_U8_1280x720, "\\AccRegression\\33_reduce_avg_h_u8_1280x720\\input", 1, "\\AccRegression\\33_reduce_avg_h_u8_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunReduceAvgHeightU8HD, 1},
    {ACC_REDUCE_SUM_H_U8_1280x720, "\\AccRegression\\34_reduce_sum_h_u8_1280x720\\input", 1, "\\AccRegression\\34_reduce_sum_h_u8_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunReduceSumHeightU8HD, 1},
    {ACC_REDUCE_MIN_W_U8_1280x720, "\\AccRegression\\35_reduce_min_w_u8_1280x720\\input", 1, "\\AccRegression\\35_reduce_min_w_u8_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunReduceMinWidthU8HD, 1},
    {ACC_REDUCE_MAX_W_U8_1280x720, "\\AccRegression\\36_reduce_max_w_u8_1280x720\\input", 1, "\\AccRegression\\36_reduce_max_w_u8_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunReduceMaxWidthU8HD, 1},
    {ACC_SPLIT_U8_1280x720x3, "\\AccRegression\\37_split_u8_1280x720x3\\input", 1, "\\AccRegression\\37_split_u8_1280x720x3\\output", 3, 0, 0, (func_t *)AmbaACC_RunSplitU8HDC3, 1},
    {ACC_ERODE_U8_640x480, "\\AccRegression\\38_erode_u8_640x480\\input", 2, "\\AccRegression\\38_erode_u8_640x480\\output", 1, 0, 0, (func_t *)AmbaACC_RunErodeU8VGA, 1},
    {ACC_TRANSFORM_U8_640x480, "\\AccRegression\\39_transform_u8_640x480\\input", 2, "\\AccRegression\\39_transform_u8_640x480\\output", 1, 0, 0, (func_t *)AmbaACC_RunTransformU8VGA, 1},
    {ACC_DILATE_U8_640x480, "\\AccRegression\\40_dilate_u8_640x480\\input", 2, "\\AccRegression\\40_dilate_u8_640x480\\output", 1, 0, 0, (func_t *)AmbaACC_RunDilateU8VGA, 1},
    {ACC_MERGE_U8_1280x720x3, "\\AccRegression\\41_merge_u8_1280x720x3\\input", 3, "\\AccRegression\\41_merge_u8_1280x720x3\\output", 1, 0, 0, (func_t *)AmbaACC_RunMergeU8HDC3, 1},
    {ACC_MIXCHANNELS_U8_640x480, "\\AccRegression\\42_mixchannels_u8_640x480\\input", 1, "\\AccRegression\\42_mixchannels_u8_640x480\\output", 1, 0, 0, (func_t *)AmbaACC_RunMixChannelsBgra2RgbU8VGA, 1},
    {ACC_DIV_F32_2496, "\\AccRegression\\43_div_f32_2496\\input", 2, "\\AccRegression\\43_div_f32_2496\\output", 1, 0, 0, (func_t *)AmbaACC_RunDivF32PAD32_46x54, 1},
    {ACC_LOG_U8_TWO_POW19, "\\AccRegression\\44_log_u8_pow19\\input", 1, "\\AccRegression\\44_log_u8_pow19\\output", 1, 0, 0, (func_t *)AmbaACC_RunLogU8TwoPow19, 1},
    {ACC_LOG_U8_TWO_POW17, "\\AccRegression\\45_log_u8_pow17\\input", 1, "\\AccRegression\\45_log_u8_pow17\\output", 1, 0, 0, (func_t *)AmbaACC_RunLogU8TwoPow17, 1},
    {ACC_LOG_U8_TWO_POW15, "\\AccRegression\\46_log_u8_pow15\\input", 1, "\\AccRegression\\46_log_u8_pow15\\output", 1, 0, 0, (func_t *)AmbaACC_RunLogU8TwoPow15, 1},
    {ACC_CONV_U8_VGA, "\\AccRegression\\47_conv_u8_640x480\\input", 2, "\\AccRegression\\47_conv_u8_640x480\\output", 1, 0, 0, (func_t *)AmbaACC_RunConvU8VGA, 1},
    {ACC_CONV_U8_HD, "\\AccRegression\\48_conv_u8_1280x720\\input", 2, "\\AccRegression\\48_conv_u8_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunConvU8HD, 1},
    {ACC_FINDNZ_U8_320x180, "\\AccRegression\\49_findNZ_u8_320x180\\input", 1, "\\AccRegression\\49_findNZ_u8_320x180\\output", 2, 0, 0, (func_t *)AmbaACC_RunFindNZU8_320x180, 1},
    {ACC_YUV2RGB_U8_HD, "\\AccRegression\\50_yuv2rgb_u8_1280x720\\input", 2, "\\AccRegression\\50_yuv2rgb_u8_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunYUV2RGBU8HD, 1},
    //Not support now
    {ACC_GAUSSIAN_U8_VGA, "\\AccRegression\\47_conv_u8_640x480\\input", 2, "\\AccRegression\\47_conv_u8_640x480\\output", 1, 0, 0, (func_t *)AmbaACC_RunGaussianBlurU8VGA, 0},
    {ACC_GAUSSIAN_U8_HD, "\\AccRegression\\48_conv_u8_1280x720\\input", 2, "\\AccRegression\\48_conv_u8_1280x720\\output", 1, 0, 0, (func_t *)AmbaACC_RunGaussianBlurU8HD, 0},
    {ACC_WARP_Y_640x480, "\\AccRegression\\53_warp_rotate_y_640x480\\input", 2, "\\AccRegression\\53_warp_rotate_y_640x480\\output", 1, 0, 0, (func_t *)AmbaACC_RunWarpRotateY640x480, 1},
    {ACC_WARP_UV_640x480, "\\AccRegression\\54_warp_rotate_uv_640x480\\input", 2, "\\AccRegression\\54_warp_rotate_uv_640x480\\output", 1, 0, 0, (func_t *)AmbaACC_RunWarpRotateUV640x480, 1},
    {ACC_ScaleOSD, "\\AccRegression\\55_scale_OSD\\input", 1, "\\AccRegression\\55_scale_OSD\\output", 1, 1920, (1920*4*48)+(32*4), (func_t *)AmbaACC_RunScaleOSD, 1},
    {ACC_ERODE_640x480_RECT3X3, "\\AccRegression\\56_erode_640x480_rect3x3\\input", 1, "\\AccRegression\\56_erode_640x480_rect3x3\\output", 1, 0, 0, (func_t *)AmbaACC_RunErodeU8VGARect3x3, 1},
    {ACC_DILATE_640x480_RECT3X3, "\\AccRegression\\57_dilate_640x480_rect3x3\\input", 1, "\\AccRegression\\57_dilate_640x480_rect3x3\\output", 1, 0, 0, (func_t *)AmbaACC_RunDilateU8VGARect3x3, 1},
    {ACC_RGB2YUV_U8_HD, "\\AccRegression\\58_rgb2yuv_u8_1280x720\\input", 1, "\\AccRegression\\58_rgb2yuv_u8_1280x720\\output", 2, 0, 0, (func_t *)AmbaACC_RunRGB2YUVU8HD, 1},
    {ACC_GEMM, "\\AccRegression\\59_gemm\\input", 3, "\\AccRegression\\59_gemm\\output", 1, 0, 0, (func_t *)AmbaACC_RunGEMM, 1},

};
#endif

static FlexiBinInfo_s dbg_table_info[] = {
    {Acc_Custom, "", 0, "", 0, 0, 0, (func_t *)AmbaACC_RunCustom, 1},
};
static char custom_bin[128];

static flexidag_memblk_t ut_input_raw[MAX_THREAD_NUM][MAX_INPUTFILE_NUM];
static flexidag_memblk_t ut_bin_buffer[MAX_THREAD_NUM];
static flexidag_memblk_t ut_state_buffer[MAX_THREAD_NUM];
static flexidag_memblk_t ut_temp_buffer;//0x4A00000
static flexidag_memblk_t ut_out_buffer[MAX_THREAD_NUM][MAX_OUTPUTFILE_NUM];
static flexidag_memblk_t ut_in_buffer[MAX_THREAD_NUM][MAX_INPUTFILE_NUM];
static flexidag_memblk_t golden_buf;

static AMBA_FS_DIR* dir;
static char disk_dir[128];


#if 0
#ifdef CONFIG_ARM32
#define AmbaMisra_TypeCast AmbaMisra_TypeCast32
#else
#define AmbaMisra_TypeCast AmbaMisra_TypeCast64
#endif
#endif

static uint32_t RefCVACCAllocBuf(void)
{
    UINT32 ret = 0U;
    static UINT32 BufAlloc_flag = 0U;
    if(BufAlloc_flag == 0U) {
        UINT32 i, j;
        //ut_input_raw
        for(i = 0; i < MAX_THREAD_NUM; i++) {
            for(j = 0; j < MAX_INPUTFILE_NUM; j++) {
                ret |= RefCV_MemblkAlloc(0x300000, &ut_input_raw[i][j]);
            }
        }

        //ut_bin_buffer
        for(i = 0; i < MAX_THREAD_NUM; i++) {
            ret |= RefCV_MemblkAlloc(0x100000, &ut_bin_buffer[i]);
        }

        //ut_state_buffer
        for(i = 0; i < MAX_THREAD_NUM; i++) {
            ret |= RefCV_MemblkAlloc(0x100000, &ut_state_buffer[i]);
        }

        //ut_temp_buffer
        ret = RefCV_MemblkAlloc(0xF23100, &ut_temp_buffer);

        //ut_out_buffer
        for(i = 0; i < MAX_THREAD_NUM; i++) {
            for(j = 0; j < MAX_OUTPUTFILE_NUM; j++) {
                ret |= RefCV_MemblkAlloc(0x7e9000, &ut_out_buffer[i][j]);
            }
        }

        //ut_in_buffer
        for(i = 0; i < MAX_THREAD_NUM; i++) {
            for(j = 0; j < MAX_INPUTFILE_NUM; j++) {
                ret |= RefCV_MemblkAlloc(0x300000, &ut_in_buffer[i][j]);
            }
        }

        //golden_buf
        ret = RefCV_MemblkAlloc(0x800000, &golden_buf);
        BufAlloc_flag = 1U;
    } else {
        AmbaPrint_PrintUInt5("RefCVACCAllocBuf ut_buffer is initialized.", 0U, 0U, 0U, 0U, 0U);
    }
    return ret;
}

static UINT32 RefCVAccParaInit(UINT32 thread_id, UINT32 num_of_frame, UINT32 blocking_run, UINT32 acc_ID, UINT32 dbg_mode)
{
    UINT32 i,j, ret = 0U;
    uint32_t input_num, output_num;
#if !defined(CONFIG_QNX)
    ULONG Paddr=0,addr=0;
#endif
    static UINT32 acc_init[MAX_THREAD_NUM] = {0U};
    const FlexiBinInfo_s *bininfo;
    if(dbg_mode==0UL) {
        bininfo = regression_table_info;
    } else {
        bininfo = dbg_table_info;
    }

    if(thread_id > MAX_THREAD_NUM) {
        AmbaPrint_PrintUInt5("[error] thread_num(%d) > MAX_THREAD_NUM ",thread_id,MAX_THREAD_NUM, 0U, 0U, 0U);
        ret = 1U;
    } else {
        for(i = 0U; i < thread_id; i ++) {
            if(acc_init[i] == 0U) {
                ret = AmbaWrap_memset(&ut_thread_params[i].RefCV_handle, 0x0, sizeof(REF_CV_HANDLE_s));

                ut_thread_params[i].id = i;
                ut_thread_params[i].num_runs    = num_of_frame;
                ut_thread_params[i].sleep_usec = (UINT32)(20U*i);
                ut_thread_params[i].RunFlag    = 0U;

                (void) RefCVSprintfUint1(&ut_thread_params[i].RefCV_handle.name[0], sizeof(ut_thread_params[i].RefCV_handle.name), "00%02d", i);
                ut_thread_params[i].RefCV_handle.id = i;
                ut_thread_params[i].RefCV_handle.in_buf.num_of_buf = bininfo[acc_ID].number_of_input;
                input_num = bininfo[acc_ID].number_of_input;
                ut_thread_params[i].RefCV_handle.out_buf.num_of_buf = bininfo[acc_ID].number_of_output;
                output_num = bininfo[acc_ID].number_of_output;
                AmbaPrint_PrintUInt5("Init : input num %d and output num %d", input_num, output_num, 0U, 0U, 0U);

                //input raw buffer
                for(j = 0; j < ut_thread_params[i].RefCV_handle.in_buf.num_of_buf; j++) {
                    ut_thread_params[i].input[j].pBuffer = ut_input_raw[i][j].pBuffer;
#if !defined(CONFIG_QNX)
                    AmbaMisra_TypeCast(&addr, &ut_thread_params[i].input[j].pBuffer);
                    (void) AmbaMMU_VirtToPhys(addr, &Paddr);
                    ut_thread_params[i].input[j].buffer_daddr = Paddr;
                    AmbaPrint_PrintUInt5("input:%d daddr is 0x%x, paddr is 0x%x", j, ut_thread_params[i].input[j].buffer_daddr, addr, 0U, 0U);
#else
                    ut_thread_params[i].input[j].buffer_daddr = ut_input_raw[i][j].buffer_daddr;
#endif
                    ut_thread_params[i].input[j].buffer_size = ut_input_raw[i][j].buffer_size;
                }

                //bin buffer
                ut_thread_params[i].RefCV_handle.bin_buf.pBuffer = ut_bin_buffer[i].pBuffer;
#if !defined(CONFIG_QNX)
                AmbaMisra_TypeCast(&addr, &ut_thread_params[i].RefCV_handle.bin_buf.pBuffer);
                (void) AmbaMMU_VirtToPhys(addr, &Paddr);
                AmbaPrint_PrintUInt5("bin: addr is 0x%x Paddr is 0x%x \n", (uint32_t)addr, Paddr, 999, 999, 999);
                ut_thread_params[i].RefCV_handle.bin_buf.buffer_daddr = Paddr;
#else
                ut_thread_params[i].RefCV_handle.bin_buf.buffer_daddr = ut_bin_buffer[i].buffer_daddr;
#endif
                ut_thread_params[i].RefCV_handle.bin_buf.buffer_size = ut_bin_buffer[i].buffer_size;

                //state buffer
                ut_thread_params[i].RefCV_handle.init.state_buf.pBuffer = ut_state_buffer[i].pBuffer;
#if !defined(CONFIG_QNX)
                AmbaMisra_TypeCast(&addr, &ut_thread_params[i].RefCV_handle.init.state_buf.pBuffer);
                (void) AmbaMMU_VirtToPhys(addr, &Paddr);
                ut_thread_params[i].RefCV_handle.init.state_buf.buffer_daddr = Paddr;
#else
                ut_thread_params[i].RefCV_handle.init.state_buf.buffer_daddr = ut_state_buffer[i].buffer_daddr;
#endif
                ut_thread_params[i].RefCV_handle.init.state_buf.buffer_size = ut_state_buffer[i].buffer_size;

                //temp buffer
                ut_thread_params[i].RefCV_handle.init.temp_buf.pBuffer = ut_temp_buffer.pBuffer;
#if !defined(CONFIG_QNX)
                AmbaMisra_TypeCast(&addr, &ut_thread_params[i].RefCV_handle.init.temp_buf.pBuffer);
                (void) AmbaMMU_VirtToPhys(addr, &Paddr);
                ut_thread_params[i].RefCV_handle.init.temp_buf.buffer_daddr = Paddr;
#else
                ut_thread_params[i].RefCV_handle.init.temp_buf.buffer_daddr = ut_temp_buffer.buffer_daddr;
#endif
                ut_thread_params[i].RefCV_handle.init.temp_buf.buffer_size = ut_temp_buffer.buffer_size;

                //in_buf buffer
                for (j = 0; j < input_num; j++) {
                    //ptr_input = (memio_source_recv_raw_t*)&ut_in_buffer[i][j];
                    //AmbaMisra_TypeCast(&ptr_char, &ptr_input);
                    ut_thread_params[i].RefCV_handle.in_buf.buf[j].pBuffer = ut_in_buffer[i][j].pBuffer;//ptr_char;
#if !defined(CONFIG_QNX)
                    AmbaMisra_TypeCast(&addr, &ut_thread_params[i].RefCV_handle.in_buf.buf[j].pBuffer);
                    (void) AmbaMMU_VirtToPhys(addr, &Paddr);
                    ut_thread_params[i].RefCV_handle.in_buf.buf[j].buffer_daddr = Paddr;
#else
                    ut_thread_params[i].RefCV_handle.in_buf.buf[j].buffer_daddr = ut_in_buffer[i][j].buffer_daddr;
#endif
                    ut_thread_params[i].RefCV_handle.in_buf.buf[j].buffer_size = ut_in_buffer[i][j].buffer_size;
                }

                //out_buf buffer
                for(j = 0; j < output_num; j++) {
                    ut_thread_params[i].RefCV_handle.out_buf.buf[j].pBuffer = ut_out_buffer[i][j].pBuffer;
#if !defined(CONFIG_QNX)
                    AmbaMisra_TypeCast(&addr, &ut_thread_params[i].RefCV_handle.out_buf.buf[j].pBuffer);
                    (void) AmbaMMU_VirtToPhys(addr, &Paddr);
                    ut_thread_params[i].RefCV_handle.out_buf.buf[j].buffer_daddr = Paddr;
#else
                    ut_thread_params[i].RefCV_handle.out_buf.buf[j].buffer_daddr = ut_out_buffer[i][j].buffer_daddr;
#endif
                    ut_thread_params[i].RefCV_handle.out_buf.buf[j].buffer_size = ut_out_buffer[i][j].buffer_size;
                }

                ut_thread_params[i].blocking_run = blocking_run;
            } else {
                ret = AmbaWrap_memset(&ut_thread_params[i].RefCV_handle.fd_gen_handle, 0x0, sizeof(AMBA_CV_FLEXIDAG_HANDLE_s));
                ut_thread_params[i].num_runs    = num_of_frame;
                ut_thread_params[i].blocking_run = blocking_run;
                ut_thread_params[i].cur_runs = 0U;
                ut_thread_params[i].num_err = 0U;
                ut_thread_params[i].RunFlag = 0U;
            }

            acc_init[i] = 0U;
            AmbaPrint_PrintUInt5("ut_thread_params[%d].num_runs           = %d ",i,ut_thread_params[i].num_runs, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("ut_thread_params[%d].sleep_usec         = %d ",i,ut_thread_params[i].sleep_usec, 0U, 0U, 0U);
            for (j = 0; j < ut_thread_params[i].RefCV_handle.in_buf.num_of_buf; j++) {
                AmbaPrint_PrintUInt5("ut_thread_params[%d].input[%d]_file pa           = 0x%x size = 0x%x",i,j,ut_thread_params[i].input[j].buffer_daddr, ut_thread_params[i].input[j].buffer_size, 0U);
            }
            AmbaPrint_PrintUInt5("ut_thread_params[%d].bin_buf pa         = 0x%x size = 0x%x ",i,ut_thread_params[i].RefCV_handle.bin_buf.buffer_daddr, ut_thread_params[i].RefCV_handle.bin_buf.buffer_size, 0U, 0U);
            AmbaPrint_PrintUInt5("ut_thread_params[%d].state_buf pa       = 0x%x size = 0x%x ",i,ut_thread_params[i].RefCV_handle.init.state_buf.buffer_daddr, ut_thread_params[i].RefCV_handle.init.state_buf.buffer_size, 0U, 0U);
            AmbaPrint_PrintUInt5("ut_thread_params[%d].temp_buf pa        = 0x%x size = 0x%x ",i,ut_thread_params[i].RefCV_handle.init.temp_buf.buffer_daddr, ut_thread_params[i].RefCV_handle.init.temp_buf.buffer_size, 0U, 0U);
            AmbaPrint_PrintUInt5("ut_thread_params[%d].in_buf pa          = 0x%x size = 0x%x ",i,ut_thread_params[i].RefCV_handle.in_buf.buf[0].buffer_daddr, ut_thread_params[i].RefCV_handle.in_buf.buf[0].buffer_size, 0U, 0U);
            AmbaPrint_PrintUInt5("ut_thread_params[%d].out_buf pa         = 0x%x size = 0x%x ",i,ut_thread_params[i].RefCV_handle.out_buf.buf[0].buffer_daddr, ut_thread_params[i].RefCV_handle.out_buf.buf[0].buffer_size, 0U, 0U);
        }
    }

    return ret;
}

static uint32_t RefCVGetFileSize(const char *file_path)
{
    uint32_t ret;
    AMBA_FS_FILE *fp;
    UINT32 Fsize = 0U;
    UINT64 fpos = 0U;

    ret = AmbaFS_FileOpen(file_path, "rb", &fp);
    if(ret !=  0U) {
        AmbaPrint_PrintUInt5("RefCVGetFileSize AmbaFS_FileOpen fail ",0U, 0U, 0U, 0U, 0U);
    } else {
        ret = AmbaFS_FileSeek(fp, 0LL, AMBA_FS_SEEK_END);
        if(ret !=  0U) {
            AmbaPrint_PrintUInt5("RefCVGetFileSize AmbaFS_FileSeek fail ",0U, 0U, 0U, 0U, 0U);
        } else {
            ret = AmbaFS_FileTell(fp, &fpos);
            if(ret !=  0U) {
                AmbaPrint_PrintUInt5("RefCVGetFileSize AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
            } else {
                Fsize = (UINT32)(fpos & 0xFFFFFFFFU);
                ret = AmbaFS_FileClose(fp);
                if(ret !=  0U) {
                    AmbaPrint_PrintUInt5("RefCVGetFileSize AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
                } else {
                    AmbaPrint_PrintUInt5("RefCVGetFileSize AmbaFS_FileClose success ", 0U, 0U, 0U, 0U, 0U);
                }
            }
        }
    }
    return Fsize;
}

static uint32_t RefCVReadFile(const char *file_path,const flexidag_memblk_t *pBuffer)
{
    uint32_t ret;
    AMBA_FS_FILE *fp;
    UINT32 Fsize, op_res;
    UINT64 fpos = 0U;
    ret = AmbaFS_FileOpen(file_path, "rb", &fp);
    if(ret !=  0U) {
        AmbaPrint_PrintUInt5("RefCVAccRun AmbaFS_FileOpen fail ",0U, 0U, 0U, 0U, 0U);
    } else {
        ret = AmbaFS_FileSeek(fp, 0LL, AMBA_FS_SEEK_END);
        if(ret !=  0U) {
            AmbaPrint_PrintUInt5("RefCVAccRun AmbaFS_FileSeek fail ",0U, 0U, 0U, 0U, 0U);
        } else {
            ret = AmbaFS_FileTell(fp, &fpos);
            if(ret !=  0U) {
                AmbaPrint_PrintUInt5("RefCVAccRun AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
            } else {
                Fsize = (UINT32)(fpos & 0xFFFFFFFFU);
                ret = AmbaFS_FileSeek(fp, 0LL, AMBA_FS_SEEK_START);
                if(ret !=  0U) {
                    AmbaPrint_PrintUInt5("RefCVAccRun AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
                } else {
                    ret = AmbaFS_FileRead(pBuffer->pBuffer, 1U, (UINT32)Fsize, fp, &op_res);
                    if(ret !=  0U) {
                        AmbaPrint_PrintUInt5("RefCVAccRun AmbaFS_FileRead fail ", 0U, 0U, 0U, 0U, 0U);
                    } else {
                        ret = AmbaFS_FileClose(fp);
                        if(ret !=  0U) {
                            AmbaPrint_PrintUInt5("RefCVAccRun AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
                        } else {
                            ret = RefCV_MemblkClean(pBuffer);
                            if(ret !=  0U) {
                                AmbaPrint_PrintUInt5("RefCVAccRun AmbaCache_DataClean fail ", 0U, 0U, 0U, 0U, 0U);
                            }
                        }
                    }
                }
            }
        }
    }
    return ret;
}

static void RefCVACCRangeCount(UINT32 input_id, UINT32 *run_start, UINT32 *run_end)
{
    UINT32 searchID;
    uint32_t table_max, j;
    table_max = sizeof(regression_table_info) / sizeof(regression_table_info[0]);
    *run_end = table_max;
    *run_start = 0U;
    searchID = 0U;
    if(input_id > 0U) {
        for(j = 0; j < table_max; j++) {
            if(regression_table_info[j].ID == (input_id)) {
                *run_end = j + 1U;
                *run_start = j;
                searchID = 1U;
            }
        }
        if(searchID == 0U) {
            *run_end = 0U;
            *run_start = 0U;
            AmbaPrint_PrintUInt5("[ERROR] this case ID is not supported\n",0,0,0,0,0);
        }
    } else {
        *run_end = table_max;
        *run_start = 0U;
    }
}
static uint32_t check_cmp_flag[CHECK_MAX] = {0};
static uint32_t check_diff[CHECK_MAX][MAX_OUTPUTFILE_NUM] = {0};

static void RefCVACCFailCount(UINT32 run_start, UINT32 run_end, UINT32 dbg_mode)
{
    UINT32 fail_case, run_id, j;
    const FlexiBinInfo_s *bininfo;
    if(dbg_mode==0UL) {
        bininfo = regression_table_info;
    } else {
        bininfo = dbg_table_info;
    }
    fail_case = 0U;
    for(run_id = run_start ; run_id < run_end; run_id++) {
        if(check_cmp_flag[run_id] == 1U) {
            AmbaPrint_PrintUInt5("Case %d fail", bininfo[run_id].ID, 999,999,999,999);
            AmbaPrint_PrintUInt5("        Acc run Fail.", 999, 999,999,999,999);
            fail_case++;
        } else if(check_cmp_flag[run_id] == 2U) {
            AmbaPrint_PrintUInt5("Case %d fail", bininfo[run_id].ID, 999,999,999,999);
            for(j = 0; j < bininfo[run_id].number_of_output; j++) {
                AmbaPrint_PrintUInt5("        diff is %d", check_diff[run_id][j],999,999,999,999);
            }
            fail_case++;
        } else {
            //MisraC
        }
    }
    if(fail_case == 0U) {
        AmbaPrint_PrintUInt5("All cases Success       regression end!", 999,999,999,999,999);
    } else {
        AmbaPrint_PrintUInt5("Fail cases count : %d   regression end!", fail_case, 999,999,999,999);
    }
}

static UINT32 ALIGN64(UINT32 X)
{
    return ((X + 63UL) & 0xFFFFFFC0UL);
}

static uint32_t RefCVACCWriteFile(UINT32 run_id, UINT32 output_num, UINT32 dbg_mode)
{
    UINT32 j, count, output_size, pos, Rval=0, addr;
    char file_path[128];
    AMBA_FS_FILE *Fp;
    char *pChar;
    uint32_t diff;
    const FlexiBinInfo_s *bininfo;
    if(dbg_mode==0UL) {
        bininfo = regression_table_info;
    } else {
        bininfo = dbg_table_info;
    }

    for(j = 0; j < output_num; j++) {
        pos = 0U;
        pos = pos + RefCVSprintfStr(&file_path[pos], disk_dir);
        pos = pos + RefCVSprintfStr(&file_path[pos], bininfo[run_id].golden_file_path);
        (void)RefCVSprintfUint1(&file_path[pos], sizeof(file_path), "%d.bin", j);
        AmbaPrint_PrintStr5("golden path is %s", file_path, NULL, NULL, NULL, NULL);
        output_size = RefCVGetFileSize(file_path);
        if(bininfo[run_id].ID == ACC_ScaleOSD) {
            output_size = 1920UL*1080UL*4UL;
        }
        golden_buf.buffer_size = ALIGN64(output_size);
        AmbaMisra_TypeCast(&addr, &golden_buf.pBuffer);
        AmbaPrint_PrintUInt5("golden size %d  addr: 0x%x\n", golden_buf.buffer_size, addr,999,999,999);

        pChar = ut_thread_params[0].RefCV_handle.out_buf.buf[j].pBuffer;
#if !defined(CONFIG_QNX)
        AmbaMisra_TypeCast(&addr, &pChar);
        (void) AmbaCache_DataInvalidate(addr, output_size);
#else
        (void) RefCV_MemblkInvalid(&ut_thread_params[0].RefCV_handle.out_buf.buf[j]);
#endif
        (void) AmbaKAL_TaskSleep(500UL);
        Rval |= RefCVReadFile(file_path, &golden_buf);
        if(Rval == 0UL) {
            Rval |= AmbaWrap_memcmp(pChar, golden_buf.pBuffer, output_size, &diff);
            pos = 0U;
            pos = pos + RefCVSprintfStr(&file_path[pos], disk_dir);
            pos = pos + RefCVSprintfStr(&file_path[pos], bininfo[run_id].golden_file_path);
            (void)RefCVSprintfUint1(&file_path[pos], sizeof(file_path), "\\output_test_%d.bin", j);

            Rval |= AmbaFS_FileOpen(file_path, "wb", &Fp);
            if(Rval == 0UL) {
                Rval = AmbaFS_FileWrite(pChar, output_size, 1, Fp, &count);
                AmbaPrint_PrintUInt5("output %d, output size is %d daddr 0x%x", j, output_size, ut_thread_params[0].RefCV_handle.out_buf.buf[j].buffer_daddr, 0U, 0U);
                AmbaPrint_PrintStr5("Path %s",file_path, NULL, NULL, NULL, NULL);
                AmbaPrint_PrintUInt5("write output success status:%d\n", Rval, 999,999,999,999);
                Rval |= AmbaFS_FileClose(Fp);
                AmbaPrint_PrintUInt5("AmbaFS_FileClose success status:%d\n", Rval, 999,999,999,999);
            } else {
                AmbaPrint_PrintUInt5("AmbaFS_FileOpen fail \n", 999, 999,999,999,999);
            }
            AmbaPrint_PrintUInt5("Diff %d  \n", diff, 999,999,999,999);
            if(diff > 0U) {
                check_cmp_flag[run_id] = 2U;
                check_diff[run_id][j] = diff;
            }
        } else {
            AmbaPrint_PrintUInt5("RefCVReadFile read golden fail \n", 999, 999,999,999,999);
        }
    }
    return Rval;
}

static void* RefCVAccRun(void* arg)
{
    UINT32 Rval = 0U;
    UINT32 j, loop_count, input_num, output_num, run_id, run_start, run_end, pos;
    UINT32 addr;
    UINT32 ACC_REQUIRED_RUN_BUFFER_SIZE = 0U;
    UINT32 input_id;
    func_t *AccRunFunc;
    char file_path[128];
    memio_source_recv_raw_t *pSrc;
    static flexidag_memblk_t init_buf;
    static UINT8 Acc_Init = 0;

    (void) arg;
    AmbaMisra_TypeCast(&input_id, &arg);
    ACC_SetDbgMode(0U);
    if (Acc_Init == 0U) {
        Rval |= AmbaACC_RequireBufferSize_Query(&ACC_REQUIRED_RUN_BUFFER_SIZE);
        if(Rval != ERRCODE_NONE) {
            AmbaPrint_PrintUInt5("[ERROR] AmbaACC_RequireBufferSize_Query error\n",0,0,0,0,0);
        }
        (void)RefCV_MemblkAlloc(ACC_REQUIRED_RUN_BUFFER_SIZE, &init_buf);
        if(Rval == ERRCODE_NONE) {
            Rval |= AmbaACC_Init(&init_buf, 1U);
            Rval |= RefCVACCAllocBuf();
        } else {
            AmbaPrint_PrintUInt5("[ERROR] AccCV_MemblkAlloc error\n",0,0,0,0,0);
        }
        Acc_Init = 1U;
    }
    if(Rval == ERRCODE_NONE) {
        RefCVACCRangeCount(input_id, &run_start, &run_end);
        AmbaPrint_PrintUInt5("run_start %d and run_end %d", run_start, run_end, 0U, 0U, 0U);
        (void)AmbaFS_OpenDir("c:\\AccRegression", &dir);
        if(dir) {
            (void)RefCVSprintfStr(&disk_dir[0], "c:");
             AmbaFS_CloseDir(dir);
        } else {
            (void)AmbaFS_OpenDir("d:\\AccRegression", &dir);
            if(dir) {
                (void)RefCVSprintfStr(&disk_dir[0], "d:");
                AmbaFS_CloseDir(dir);
            }
        }
        for(run_id = run_start; run_id < run_end; run_id++) { //ACC_DAG_ID_MAX
            if(regression_table_info[run_id].supported == 0U) {
                AmbaPrint_PrintUInt5("case %d not support ", regression_table_info[run_id].ID, 999,999,999,999);
                continue;
            }
            Rval |= RefCVAccParaInit(1UL, 1U, 1U, run_id, 0UL);
            if(Rval != 0U) {
                AmbaPrint_PrintUInt5("Init ID %d fail ", run_id, 999,999,999,999);
                break;
            }
            check_cmp_flag[run_id] = 0U;
            AccRunFunc = regression_table_info[run_id].AccFunc;
            input_num = regression_table_info[run_id].number_of_input;
            output_num = regression_table_info[run_id].number_of_output;
            AmbaPrint_PrintUInt5("input num %d and output num %d", input_num, output_num, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("RefCVAccRun input_id %d\n", run_id, 0U, 0U, 0U, 0U);
            //read input
            for(j = 0; j < input_num; j++) {
                pos = 0U;
                pos = pos + RefCVSprintfStr(&file_path[pos], disk_dir);
                pos = pos + RefCVSprintfStr(&file_path[pos], regression_table_info[run_id].input_file_path);
                (void)RefCVSprintfUint1(&file_path[pos], sizeof(file_path), "%d.bin", j);
                Rval |= RefCVReadFile(file_path, &ut_thread_params[0].input[j]);
                AmbaPrint_PrintUInt5("case %d, read file", regression_table_info[run_id].ID, 0U, 0U, 0U, 0U);
                if(Rval == 0U) {
                    AmbaPrint_PrintUInt5("input %d read success, input size is %d daddr 0x%x", j, ut_thread_params[0].input[j].buffer_size, ut_thread_params[0].input[j].buffer_daddr, 0U, 0U);
                    AmbaPrint_PrintStr5("Path %s",file_path, NULL, NULL, NULL, NULL);
                } else {
                    AmbaPrint_PrintUInt5("Read %d Fail daddr 0x%x", j, ut_thread_params[0].input[j].buffer_daddr, 0U, 0U, 0U);
                    AmbaPrint_PrintStr5("Path %s",file_path, NULL, NULL, NULL, NULL);
                    check_cmp_flag[run_id] = 1U;
                }
            }

            for(j = 0; j < input_num; j++) {
                AmbaMisra_TypeCast(&pSrc, &ut_thread_params[0].RefCV_handle.in_buf.buf[j].pBuffer);
                pSrc->addr = ut_thread_params[0].input[j].buffer_daddr;
                pSrc->size = ut_thread_params[0].input[j].buffer_size;
                pSrc->pitch= 0;
                //Rval |= AmbaCache_DataClean(pSrc->addr, pSrc->size);
                AmbaMisra_TypeCast(&addr, &ut_thread_params[0].RefCV_handle.in_buf.buf[j].pBuffer);
                Rval |= AmbaCache_DataClean(addr, ut_thread_params[0].RefCV_handle.in_buf.buf[j].buffer_size);
                Rval |= AmbaCache_DataClean(ut_thread_params[0].RefCV_handle.in_buf.buf[j].buffer_daddr, ut_thread_params[0].RefCV_handle.in_buf.buf[j].buffer_size);
                AmbaPrint_PrintUInt5("Read input count : %d and ret is 0x%x", j, Rval, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("input %d, input size is %d daddr 0x%x", j, ut_thread_params[0].RefCV_handle.in_buf.buf[j].buffer_size, ut_thread_params[0].RefCV_handle.in_buf.buf[j].buffer_daddr, 0U, 0U);
                if(Rval != 0UL) {
                    AmbaPrint_PrintUInt5("RefCVAccRun : AmbaCache_DataClean status:%d", Rval, 0U, 0U, 0U, 0U);
                }
            }
            //set offset
            ut_thread_params[0].RefCV_handle.out_buf.buf[0].buffer_daddr = ut_thread_params[0].RefCV_handle.out_buf.buf[0].buffer_daddr + regression_table_info[run_id].output_offset;
            AmbaMisra_TypeCast(&addr, &ut_thread_params[0].RefCV_handle.out_buf.buf[0].pBuffer);
            Rval |= AmbaCache_DataClean(addr, ut_thread_params[0].RefCV_handle.out_buf.buf[0].buffer_size);
            if(Rval != 0UL) {
                AmbaPrint_PrintUInt5("RefCVAccRun : test AmbaCache_DataClean fail", 0U, 0U, 0U, 0U, 0U);
                check_cmp_flag[run_id] = 1U;
            }
            for(loop_count = 0; loop_count < loop_max; loop_count++) {
                Rval |= AccRunFunc(&ut_thread_params[0].RefCV_handle.in_buf, &ut_thread_params[0].RefCV_handle.out_buf);
            }
            if(Rval != 0UL) {
                AmbaPrint_PrintUInt5("RefCVAccRun : AccRunFunc fail\n\n", 0U, 0U, 0U, 0U, 0U);
                check_cmp_flag[run_id] = 1U;
            } else {
                (void)RefCVACCWriteFile(run_id, output_num, 0UL);
            }
        }
        RefCVACCFailCount(run_start, run_end, 0UL);
    } else {
        AmbaPrint_PrintUInt5("ACC_Init and AllocBuf fail\n\n", 0U, 0U, 0U, 0U, 0U);
    }
    (void)AmbaACC_Uninit();

    return NULL;
}

static void* RefCVAccRunCustom(void* arg)
{
    UINT32 Rval = 0U;
    UINT32 ACC_REQUIRED_RUN_BUFFER_SIZE;
    UINT32 addr;
    char bin_path[128];
    char file_path[128];
    memio_source_recv_raw_t *pSrc;
    static flexidag_memblk_t init_buf;
    UINT32 j, loop_count, pos = 0, output_size, input_num, output_num, len;
    func_t *AccRunFunc;
    static UINT8 Acc_dbg_Init = 0;

    (void) arg;
    AmbaMisra_TypeCast(&ACC_REQUIRED_RUN_BUFFER_SIZE, &arg);
    pos = pos + RefCVSprintfStr(&bin_path[0], custom_bin);
    pos = pos + RefCVSprintfStr(&bin_path[pos], ".bin");
    len = pos + 4UL;
    output_size = RefCVGetFileSize(bin_path);
    ACC_SetDbgMode(1U);
    if (Acc_dbg_Init == 0U) {
        AmbaACC_RequireBufferSize_Custom(output_size, bin_path, len, &ACC_REQUIRED_RUN_BUFFER_SIZE);
        (void)RefCV_MemblkAlloc(ACC_REQUIRED_RUN_BUFFER_SIZE, &init_buf);
        Rval |= AmbaACC_Init(&init_buf, 1U);
        Rval |= RefCVACCAllocBuf();
        Acc_dbg_Init = 1U;
    }

    Rval |= RefCVAccParaInit(1UL, 1U, 1U, 0UL, 1UL);
    if(Rval != 0U) {
        AmbaPrint_PrintUInt5("Init Custom fail ", 999, 999,999,999,999);
    }
    AccRunFunc = dbg_table_info[0].AccFunc;
    input_num = dbg_table_info[0].number_of_input;
    output_num = dbg_table_info[0].number_of_output;

    AmbaPrint_PrintUInt5("input num %d and output num %d", input_num, output_num, 0U, 0U, 0U);
    //read input
    for(j = 0; j < input_num; j++) {
        pos = 0U;
        pos = pos + RefCVSprintfStr(&file_path[pos], dbg_table_info[0].input_file_path);
        (void)RefCVSprintfUint1(&file_path[pos], sizeof(file_path), "%d.bin", j);
        Rval |= RefCVReadFile(file_path, &ut_thread_params[0].input[j]);
        AmbaPrint_PrintUInt5("case %d, read file", dbg_table_info[0].ID, 0U, 0U, 0U, 0U);
        if(Rval == 0U) {
            AmbaPrint_PrintUInt5("input %d read success, input size is %d daddr 0x%x", j, ut_thread_params[0].input[j].buffer_size, ut_thread_params[0].input[j].buffer_daddr, 0U, 0U);
            AmbaPrint_PrintStr5("Path %s",file_path, NULL, NULL, NULL, NULL);
        } else {
            AmbaPrint_PrintUInt5("Read %d Fail daddr 0x%x", j, ut_thread_params[0].input[j].buffer_daddr, 0U, 0U, 0U);
            AmbaPrint_PrintStr5("Path %s",file_path, NULL, NULL, NULL, NULL);
        }
    }

    for(j = 0; j < input_num; j++) {
        AmbaMisra_TypeCast(&pSrc, &ut_thread_params[0].RefCV_handle.in_buf.buf[j].pBuffer);
        pSrc->addr = ut_thread_params[0].input[j].buffer_daddr;
        pSrc->size = ut_thread_params[0].input[j].buffer_size;
        pSrc->pitch= 0;
        Rval |= AmbaCache_DataClean(pSrc->addr, pSrc->size);
        AmbaMisra_TypeCast(&addr, &ut_thread_params[0].RefCV_handle.in_buf.buf[j].pBuffer);
        Rval |= AmbaCache_DataClean(addr, ut_thread_params[0].RefCV_handle.in_buf.buf[j].buffer_size);
        AmbaPrint_PrintUInt5("Read input count : %d and ret is 0x%x", j, Rval, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("input %d, input size is %d daddr 0x%x", j, ut_thread_params[0].RefCV_handle.in_buf.buf[j].buffer_size, ut_thread_params[0].RefCV_handle.in_buf.buf[j].buffer_daddr, 0U, 0U);
        if(Rval != 0UL) {
            AmbaPrint_PrintUInt5("RefCVAccRun : AmbaCache_DataClean status:%d", Rval, 0U, 0U, 0U, 0U);
        }
    }

    for(loop_count = 0; loop_count < loop_max; loop_count++) {
        Rval |= AccRunFunc(&ut_thread_params[0].RefCV_handle.in_buf, &ut_thread_params[0].RefCV_handle.out_buf);
    }
    if(Rval != 0UL) {
        AmbaPrint_PrintUInt5("RefCVAccRun : AccRunFunc fail\n\n", 0U, 0U, 0U, 0U, 0U);
        check_cmp_flag[0] = 1U;
    } else {
        (void)RefCVACCWriteFile(0UL, output_num, 1UL);
    }
    RefCVACCFailCount(0UL, 1UL, 1UL);
    (void)AmbaACC_Uninit();

    return NULL;
}

void RefCV_AccUT(UINT32 params, UINT32 loop)
{
    UINT32 Rval;
    static UINT8 refcv_acc_main_stack[0x4000] GNU_SECTION_NOZEROINIT;
    static AMBA_KAL_TASK_t main_task;
    static UINT8 main_task_init = 0U;
    char task_name[32] =  "acc_main";
#if !defined(CONFIG_QNX)
    AMBA_KAL_TASK_INFO_s taskInfo;
#endif
    const void* acc_ID;
    loop_max = loop;

    if (main_task_init == 1U) {
#if !defined(CONFIG_QNX)
        (void) AmbaKAL_TaskQuery(&main_task, &taskInfo);
        if ((taskInfo.TaskState != TX_COMPLETED) && (taskInfo.TaskState != TX_TERMINATED)) {
            Rval  = AmbaKAL_TaskTerminate(&main_task);
            if ( Rval  != TX_SUCCESS ) {
                AmbaPrint_PrintUInt5("RefCV_AccUT AmbaKAL_TaskTerminate fail (%d)", Rval, 0U, 0U, 0U, 0U);
            }
        }
#endif
        Rval = AmbaKAL_TaskDelete(&main_task);
        if ( Rval != KAL_ERR_NONE ) {
            AmbaPrint_PrintUInt5("RefCV_AccUT AmbaKAL_TaskDelete fail (%d)", Rval, 0U, 0U, 0U, 0U);
        }
    }

    AmbaMisra_TypeCast(&acc_ID, &params);
    Rval = AmbaKAL_TaskCreate(&main_task,                                       /* pTask */
                              task_name,                                                    /* pTaskName */
                              100,  /* Priority */
                              RefCVAccRun,                                              /* void (*EntryFunction)(UINT32) */
                              acc_ID,                                                   /* EntryArg */
                              &refcv_acc_main_stack[0],                                 /* pStackBase */
                              sizeof(refcv_acc_main_stack),                                 /* StackByteSize */
                              0U);

    if(Rval == 0U) {
#if !defined(CONFIG_QNX)
        (void) AmbaKAL_TaskSetSmpAffinity(&main_task, 0x1U);
#endif
        (void) AmbaKAL_TaskResume(&main_task);
    } else {
        AmbaPrint_PrintUInt5("RefCV_AccUT AmbaKAL_TaskCreate fail (%d)", Rval, 0U, 0U, 0U, 0U);
    }

    if(Rval == 0U) {
    }
    main_task_init = 1U;
}
void RefCV_AccCustomUT(const char *RefCV_custom_bin_path, const char *RefCV_input_path, const char *RefCV_output_path, uint32_t RefCV_input_count, uint32_t RefCV_output_count, uint32_t RefCV_output_offset, UINT32 loop)
{
    UINT32 Rval;
    static UINT8 refcv_acc_main_stack[0x4000] GNU_SECTION_NOZEROINIT;
    static AMBA_KAL_TASK_t main_task;
    static UINT8 main_task_init = 0U;
    char task_name[32] =  "acc_custom";
#if !defined(CONFIG_QNX)
    AMBA_KAL_TASK_INFO_s taskInfo;
#endif
    //char bin_name[128];
    loop_max = loop;
    dbg_table_info[0].number_of_input = RefCV_input_count;
    dbg_table_info[0].number_of_output = RefCV_output_count;
    //dbg_table_info[0].output_pitch = output_pitch;
    dbg_table_info[0].output_offset = RefCV_output_offset;
    (void)RefCVSprintfStr(&custom_bin[0], RefCV_custom_bin_path);
    (void)RefCVSprintfStr(&dbg_table_info[0].input_file_path[0], RefCV_input_path);
    (void)RefCVSprintfStr(&dbg_table_info[0].golden_file_path[0], RefCV_output_path);

    if (main_task_init == 1U) {
#if !defined(CONFIG_QNX)
        (void) AmbaKAL_TaskQuery(&main_task, &taskInfo);
        if ((taskInfo.TaskState != TX_COMPLETED) && (taskInfo.TaskState != TX_TERMINATED)) {
            Rval  = AmbaKAL_TaskTerminate(&main_task);
            if ( Rval  != TX_SUCCESS ) {
                AmbaPrint_PrintUInt5("RefCV_AccUT_CUSTOM AmbaKAL_TaskTerminate fail (%d)", Rval, 0U, 0U, 0U, 0U);
            }
        }
#endif
        Rval = AmbaKAL_TaskDelete(&main_task);
        if ( Rval != KAL_ERR_NONE ) {
            AmbaPrint_PrintUInt5("RefCV_AccUT_CUSTOM AmbaKAL_TaskDelete fail (%d)", Rval, 0U, 0U, 0U, 0U);
        }
    }

    //bin_name = params;
    Rval = AmbaKAL_TaskCreate(&main_task,                                       /* pTask */
                              task_name,                                                    /* pTaskName */
                              100,  /* Priority */
                              RefCVAccRunCustom,                                                /* void (*EntryFunction)(UINT32) */
                              NULL,                                                 /* EntryArg */
                              &refcv_acc_main_stack[0],                                 /* pStackBase */
                              sizeof(refcv_acc_main_stack),                                 /* StackByteSize */
                              0U);

    if(Rval == 0U) {
#if !defined(CONFIG_QNX)
        (void) AmbaKAL_TaskSetSmpAffinity(&main_task, 0x1U);
#endif
        (void) AmbaKAL_TaskResume(&main_task);
    } else {
        AmbaPrint_PrintUInt5("RefCV_AccUT_CUSTOM AmbaKAL_TaskCreate fail (%d)", Rval, 0U, 0U, 0U, 0U);
    }
    if(Rval == 0U) {
    }
    main_task_init = 1U;
}

