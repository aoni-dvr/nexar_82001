/**
*  @file AmbaDSP_EncodeAPI_Def.h
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
*  @details Definitions & Constants for the APIs of SSP Encoder API definition
*
*/

#ifndef AMBADSP_ENCODEAPI_DEF_H
#define AMBADSP_ENCODEAPI_DEF_H

#include "AmbaDSP_ContextUtility.h"

//#define TEST_FHD_TILE_MODE

/* Some useful resolution number */
#define UHD_WIDTH                               (3840U)
#define UHD_HEIGHT                              (2160U)
#define UHD_8K_WIDTH                            (7680U)
#define UHD_8K_HEIGHT                           (4320U)

/* Following Define HW specific Limitation */
#define DOWNSMAPLE_MAX_RATIO                    (8U)
#define EXTRA_DOWNSMAPLE_MAX_RATIO              (16U)
#define PREVA_EXTRA_2X_DOWNSMAPLE_MAX_WIDTH     (960U)
#define PREVC_EXTRA_2X_DOWNSMAPLE_MAX_WIDTH     (384U)
#define MAIN_MAX_WIDTH                          (7680U) //TBD
#if defined(TEST_FHD_TILE_MODE)
#define PREVA_MAX_WIDTH                         (1280U)
#define PREVB_MAX_WIDTH                         (1504U)
#define PREVC_MAX_WIDTH                         (768U)
#define PREVD_MAX_WIDTH                         (1504U)
#define SEC2_MAX_IN_WIDTH                       (1504U)
#define SEC2_MAX_OUT_WIDTH                      (1504U)
#define SEC2_MAX_OUT_WIDTH_INTERNAL             (1504U) //according to ucoder
#define SEC2_MAX_OUT_WIDTH_SMEM                 (SEC2_MAX_OUT_WIDTH) //not test yes
#define SEC4_MAX_IN_WIDTH                       (376U)
#define PREVA_2X_MAX_WIDTH                      (640U)
#define PREVB_2X_MAX_WIDTH                      (1504U) //no extra 2x
#define PREVC_2X_MAX_WIDTH                      (384U)
#elif defined(TEST_UHD_4TILE_MODE)
#define PREVA_MAX_WIDTH                         (1504U)
#define PREVB_MAX_WIDTH                         (1504U)
#define PREVC_MAX_WIDTH                         (1280U)
#define PREVD_MAX_WIDTH                         (736U)
#define SEC2_MAX_IN_WIDTH                       (1280U)
#define SEC2_MAX_OUT_WIDTH                      (1280U)
#define SEC2_MAX_OUT_WIDTH_INTERNAL             (1280U) //according to ucoder
#define SEC2_MAX_OUT_WIDTH_SMEM                 (SEC2_MAX_OUT_WIDTH) //not test yes
#define SEC4_MAX_IN_WIDTH                       (320U)
#define PREVA_2X_MAX_WIDTH                      (1504U)
#define PREVB_2X_MAX_WIDTH                      (1504U) //no extra 2x
#define PREVC_2X_MAX_WIDTH                      (1280U)
#elif defined(TEST_MAIN_ONLY)
#define PREVA_MAX_WIDTH                         (256U)
#define PREVB_MAX_WIDTH                         (256U)
#define PREVC_MAX_WIDTH                         (256U)
#define PREVD_MAX_WIDTH                         (256U)
#define SEC2_MAX_IN_WIDTH                       (2048U) //actual is 2048
#define SEC2_MAX_OUT_WIDTH                      (2048U) //actual is 2048
#define SEC4_MAX_IN_WIDTH                       (512U)
#define PREVA_2X_MAX_WIDTH                      (256U)
#define PREVB_2X_MAX_WIDTH                      (256U) //no extra 2x
#define PREVC_2X_MAX_WIDTH                      (256U)
#elif defined(TEST_MAIN_B_ONLY)
#define PREVA_MAX_WIDTH                         (256U)
#define PREVB_MAX_WIDTH                         (2048U)
#define PREVC_MAX_WIDTH                         (256U)
#define PREVD_MAX_WIDTH                         (256U)
#define SEC2_MAX_IN_WIDTH                       (2048U) //actual is 2048
#define SEC2_MAX_OUT_WIDTH                      (2048U) //actual is 2048
#define SEC4_MAX_IN_WIDTH                       (512U)
#define PREVA_2X_MAX_WIDTH                      (256U)
#define PREVB_2X_MAX_WIDTH                      (2048U) //no extra 2x
#define PREVC_2X_MAX_WIDTH                      (256U)
#elif defined(TEST_MAIN_B_A_ONLY)
#define PREVA_MAX_WIDTH                         (2048U)
#define PREVB_MAX_WIDTH                         (2048U)
#define PREVC_MAX_WIDTH                         (256U)
#define PREVD_MAX_WIDTH                         (256U)
#define SEC2_MAX_IN_WIDTH                       (2048U) //actual is 2048
#define SEC2_MAX_OUT_WIDTH                      (2048U) //actual is 2048
#define SEC4_MAX_IN_WIDTH                       (512U)
#define PREVA_2X_MAX_WIDTH                      (2048U)
#define PREVB_2X_MAX_WIDTH                      (2048U) //no extra 2x
#define PREVC_2X_MAX_WIDTH                      (256U)
#else
#define PREVA_MAX_WIDTH                         (2048U)
#define PREVB_MAX_WIDTH                         (2048U)
#define PREVC_MAX_WIDTH                         (1408U)
#define PREVD_MAX_WIDTH                         (2048U)
#define SEC2_MAX_IN_WIDTH                       (2048U) //actual is 2048
#define SEC2_MAX_OUT_WIDTH                      (2048U) //actual is 2048
#define SEC2_MAX_OUT_WIDTH_INTERNAL             (2048U) //according to ucoder
#define SEC2_MAX_OUT_WIDTH_SMEM                 (SEC2_MAX_OUT_WIDTH) //not test yes
#define SEC4_MAX_IN_WIDTH                       (512U)
#define PREVA_2X_MAX_WIDTH                      (2048U)
#define PREVB_2X_MAX_WIDTH                      (2048U) //no extra 2x
#define PREVC_2X_MAX_WIDTH                      (1408U)
#endif
#define PREV_MAX_RATIO_8X                       (8U)
#define PREV_MAX_RATIO_16X                      (16U)
#define PREV_MAX_RATIO_32X                      (32U)
#define WARP_DMA_MAX_VALUE                      (64U)
#define TILE_OVERLAP_WIDTH                      (128U)
#define TILE_WIDTH_ALIGN                        (32U)
#define WARP_GRID_EXP                           (4U)
#define TILE_OVERLAP_PADDING                    (2U) //use when main-rescale
#define EFCY_TILE_OVERLAP_WIDTH                 (64U)
#define EFCY_TILE_OVERLAP_PADDING               (18U) //use when main-rescale
#define DEC_VIEW_TILE_OVERLAP_WIDTH             (64U)
#define MAX_TILE_NUM                            (12U)
#define VPROC_DRAM_SIZE                         (4U)    //0 = 2MB, use larger size to avoid assertion
#define SEC10_MAX_IN_WIDTH                      (2048U)

#define IS_VP_MSG_CTRL_EXIST                    (1U)
#define IS_HEIR_FILTER_EXIST                    (1U)
#define IS_LNDT_FILTER_EXIST                    (1U)
#define IS_MAIN_Y12_FILTER_EXIST                (1U)
#define IS_MAIN_IR_FILTER_EXIST                 (1U)

#define CV2X_MAX_RAW_FRM_NUM                    (5U)
#define CV2X_MAX_C2Y_FB_NUM                     (3U)
#define CV2X_MAX_DEF_C2Y_FB_NUM                 (CV2X_MAX_C2Y_FB_NUM) //HL's internal depth is 3
#define CV2X_MAX_MAIN_FB_NUM                    (7U/*5U*/)
#define CV2X_MAX_PREV_FB_NUM                    (6U)
#define CV2X_DEFAULT_HIER_FB_NUM                (5U)
#define CV2X_DEFAULT_LNDT_FB_NUM                (5U)
#define CV2X_MAX_VIRT_VIN_RAW_FRM_NUM           (1U)
#define CV2X_MAX_VIRT_VIN_DEC_FRM_NUM           (2U)

#define CV2X_EXTRA_SLICE_CAP_LINE               (128U)
// 2021/0331 sync with IK, 8line in dram2warp
#define CV2X_WARP_WAITLINE                      (8U)
// 2021/0331 sync with IK. different ChromaRadius require differnt min. lines
// Radius_32 = ALIGN8(28), Radius_64 = ALIGN8(40), Radius_128 = ALIGN8(64)
// here we choose worst case ie Radius_128
#define CV2X_WARP_SMEM2WARP_WAITLINE            (64U)
#define CV2X_WARP_OVERLAP_Y_HIER_ON             (128U)
#define CV2X_WARP_OVERLAP_Y_HIER_OFF            (32U)

// 2021/0331 sync with IK
// chroma = ALIGN2((Luma - 6)/2 + 6)
#define CV5X_WARP_DMA_SIZE                      (32U)
#define CV5X_WARP_CHROMA_DMA_SIZE               (20U)

#define CV2X_MAX_STL_C2Y_FB_NUM                 (1U)
#define CV2X_MAX_STL_MAIN_FB_NUM                (1U) // ucode pipeline depth is 2
#define CV2X_MAX_STL_PREVA_FB_NUM               (1U)
#define CV2X_MAX_STL_PREVB_FB_NUM               (1U)
#define CV2X_MAX_STL_PREVC_FB_NUM               (1U)

/* Project specific */
#define CV2X_CRUISE_LD_WIDTH                    (2880U)
#define CV2X_CRUISE_LD_HEIGHT                   (1860U)
#define CV2X_CRUISE_LD_TILE_OVERLAP_WIDTH       (32U)

#define CV5X_IDSP_CORE_NUM                      (2U)
#define CV5X_VDSP_CORE_NUM                      (2U)

#define CV5X_VPROC_DUMMY_SIZE                   (256U)

#endif //AMBADSP_ENCODEAPI_DEF_H
