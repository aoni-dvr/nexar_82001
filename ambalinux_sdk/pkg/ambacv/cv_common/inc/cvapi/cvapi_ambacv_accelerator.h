/**
 *  @file AmbaCV_Flexidag.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Definitions & Constants for Ambarella DSP Support Package
 *
 */

#define ACC_20200924 1UL
#define ACC_20201106 1UL
#ifndef CVAPI_AMBACV_ACCELERATOR_H
#define CVAPI_AMBACV_ACCELERATOR_H

#include "cvapi_ambacv_flexidag.h"
#if !defined(CONFIG_BUILD_CV_THREADX) && !defined(__QNXNTO__)
#include "rtos/AmbaRTOSWrapper.h"
#else
#include "AmbaErrorCode.h"
#endif

#define Acc_Custom                               0UL
#define ACC_RESIZE_Y_U8_640x360_1280x720         1UL
#define ACC_RESIZE_Y_U8_420_1280x720_640x360     2UL
#define ACC_RESIZE_UV_U8_420_640x360_1280x720    3UL
#define ACC_RESIZE_UV_U8_420_1280x720_640x360    4UL
#define ACC_MIN_MAX_LOC_U8_1280x720              5UL
#define ACC_MIN_MAX_LOC_U16_1280x720             6UL
#define ACC_AND_U8_TWO_POW_19                    7UL
#define ACC_AND_U8_TWO_POW_17                    8UL
#define ACC_AND_U8_TWO_POW_15                    9UL
#define ACC_CMP_NOT_EQUAL_U8_TWO_POW_19          10UL
#define ACC_CMP_NOT_EQUAL_U8_TWO_POW_17          11UL
#define ACC_CMP_NOT_EQUAL_U8_TWO_POW_15          12UL
#define ACC_CMP_GREATER_S8_TWO_POW_19            13UL
#define ACC_CMP_GREATER_S8_TWO_POW_17            14UL
#define ACC_CMP_GREATER_S8_TWO_POW_15            15UL
#define ACC_CMP_NEQ_U16_TWO_POW_18               16UL
#define ACC_CMP_NEQ_U16_TWO_POW_16               17UL
#define ACC_CMP_NEQ_U16_TWO_POW_14               18UL
#define ACC_DIVIDE_U8_TWO_POW_18                 19UL
#define ACC_DIVIDE_U8_TWO_POW_16                 20UL
#define ACC_DIVIDE_U8_TWO_POW_14                 21UL
#define ACC_DIVIDE_U16_TWO_POW_17                22UL
#define ACC_DIVIDE_U16_TWO_POW_15                23UL
#define ACC_DIVIDE_U16_TWO_POW_13                24UL
#define ACC_DIVIDE_S16_TWO_POW_17                25UL
#define ACC_DIVIDE_S16_TWO_POW_15                26UL
#define ACC_DIVIDE_S16_TWO_POW_13                27UL
#define ACC_THRES_BINARY_U8_TWO_POW18            28UL
#define ACC_THRES_BINARY_U8_TWO_POW14            29UL
#define ACC_THRES_BINARY_F32_TWO_POW17           30UL
#define ACC_THRES_BINARY_F32_TWO_POW13           31UL
#define ACC_THRES_TOZERO_F32_TWO_POW17           32UL
#define ACC_THRES_TOZERO_F32_TWO_POW13           33UL
#define ACC_REDUCE_AVG_H_U8_1280x720             34UL
#define ACC_REDUCE_SUM_H_U8_1280x720             35UL
#define ACC_REDUCE_MIN_W_U8_1280x720             36UL
#define ACC_REDUCE_MAX_W_U8_1280x720             37UL
#define ACC_SPLIT_U8_1280x720x3                  38UL
#define ACC_ERODE_U8_640x480                     39UL
#define ACC_TRANSFORM_U8_640x480                 40UL
#define ACC_DILATE_U8_640x480                    41UL
#define ACC_MERGE_U8_1280x720x3                  42UL
#define ACC_MIXCHANNELS_U8_640x480               43UL
#define ACC_DIV_F32_2496                         44UL
#define ACC_LOG_U8_TWO_POW19                     45UL
#define ACC_LOG_U8_TWO_POW17                     46UL
#define ACC_LOG_U8_TWO_POW15                     47UL
#define ACC_CONV_U8_VGA                          48UL
#define ACC_CONV_U8_HD                           49UL
#define ACC_FINDNZ_U8_320x180                    50UL
#define ACC_YUV2RGB_U8_HD                        51UL
#define ACC_GAUSSIAN_U8_VGA                      52UL
#define ACC_GAUSSIAN_U8_HD                       53UL
#define ACC_WARP_Y_640x480                       54UL
#define ACC_WARP_UV_640x480                      55UL
#define ACC_ScaleOSD                             56UL
#define ACC_ERODE_640x480_RECT3X3                57UL
#define ACC_DILATE_640x480_RECT3X3               58UL
#define ACC_RGB2YUV_U8_HD                        59UL
#define ACC_GEMM                                 60UL
#define ACC_DAG_ID_MAX                           61UL

#define ACC_Temp_Buffer (15872256UL)
#define ACC_Debug_Mode_Buffer 429568UL
//#define ACC_REQUIRED_BUFFER_SIZE 4151040UL//bin buffer(2977408) + state buffer(234496) + temp buffer(468352) + debug mode buffer(429568)

#define ACC_ERROR_BASE (CVACC_ERR_BASE)

#define ACC_ERR_0001 (ACC_ERROR_BASE | 1UL) // ACC_ERRCODE_MEMORY_NOT_ENOUGH
#define ACC_ERR_0002 (ACC_ERROR_BASE | 2UL) // ACC_ERRCODE_INVALID_INPUTS
#define ACC_ERR_0003 (ACC_ERROR_BASE | 3UL) // ACC_ERRCODE_MEMORY_NOT_ALIGN
#define ACC_ERR_0004 (ACC_ERROR_BASE | 4UL) // ACC_ERRCODE_ACC_NOT_SUPPORT

void ACC_SetDbgMode(UINT32 dbg_switch);
//uint32_t AmbaACC_QueryMemorySize(uint32_t *size, flexidag_memblk_t *pBinBuffer);
void AmbaACC_RequireBufferSize_Custom(UINT32 bin_size, const char * file_path, UINT32 path_len, UINT32 * query_size);
uint32_t AmbaACC_RequireBufferSize_Query(UINT32 *query_size);
//uint32_t AmbaACC_SetPitch(UINT32 ID, uint32_t pitch);
//uint32_t AmbaACC_SetOffset(uint32_t addr , uint32_t offset);
uint32_t AmbaACC_Init(const flexidag_memblk_t *pBuffer, uint32_t InitMode);
uint32_t AmbaACC_Uninit(void);
uint32_t AmbaACC_RunCustom(AMBA_CV_FLEXIDAG_IO_s * Input, AMBA_CV_FLEXIDAG_IO_s * Output);
uint32_t AmbaACC_RunResizeYU8NHD2HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunResizeYU8HD2NHD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunResizeUVU8NHD2HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunResizeUVU8HD2NHD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunMinMaxLocU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunMinMaxLocU16HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunAndU8TwoPow19(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunAndU8TwoPow17(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunAndU8TwoPow15(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunCmpNeqU8TwoPow19(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunCmpNeqU8TwoPow17(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunCmpNeqU8TwoPow15(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunCmpGtrS8TwoPow19(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunCmpGtrS8TwoPow17(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunCmpGtrS8TwoPow15(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunCmpNeqU16TwoPow18(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunCmpNeqU16TwoPow16(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunCmpNeqU16TwoPow14(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunDivideU8TwoPow18(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunDivideU8TwoPow16(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunDivideU8TwoPow14(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunDivideU16TwoPow17(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunDivideU16TwoPow15(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunDivideU16TwoPow13(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunDivideS16TwoPow17(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunDivideS16TwoPow15(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunDivideS16TwoPow13(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunThresBinU8TwoPow18(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunThresBinU8TwoPow14(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunThresBinF32TwoPow17(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunThresBinF32TwoPow13(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunThresTo0F32TwoPow17(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunThresTo0F32TwoPow13(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunReduceAvgHeightU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunReduceSumHeightU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunReduceMinWidthU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunReduceMaxWidthU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunSplitU8HDC3(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunErodeU8VGA(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunTransformU8VGA(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunDilateU8VGA(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunMergeU8HDC3(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunMixChannelsBgra2RgbU8VGA(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunDivF32PAD32_46x54(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunLogU8TwoPow19(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunLogU8TwoPow17(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunLogU8TwoPow15(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunConvU8VGA(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunConvU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunFindNZU8_320x180(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunYUV2RGBU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunGaussianBlurU8VGA(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunGaussianBlurU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunWarpRotateY640x480(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunWarpRotateUV640x480(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunScaleOSD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunErodeU8VGARect3x3(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunDilateU8VGARect3x3(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunRGB2YUVU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);
uint32_t AmbaACC_RunGEMM(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output);

#endif // CVAPI_AMBACV_IMG_SERVICE_H

