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

/* Following Define HW specific Limitation */
#define DOWNSMAPLE_MAX_RATIO                    (8U)
#define EXTRA_DOWNSMAPLE_MAX_RATIO              (16U)
#define PREVA_EXTRA_2X_DOWNSMAPLE_MAX_WIDTH     (960U)
#define PREVC_EXTRA_2X_DOWNSMAPLE_MAX_WIDTH     (384U)
#define MAIN_MAX_WIDTH                          (7680U) //TBD
#define PREVA_MAX_WIDTH                         (1280U)
#define PREVB_MAX_WIDTH                         (2048U)
#define PREVC_MAX_WIDTH                         (768U)
#define PREVD_MAX_WIDTH                         (2048U)
#define SEC2_MAX_IN_WIDTH                       (2048U) //actual is 2048
#define SEC2_MAX_OUT_WIDTH                      (2048U) //actual is 2048
#define SEC2_MAX_OUT_WIDTH_INTERNAL             (1921U) //according to ucoder
#define SEC2_MAX_OUT_WIDTH_SMEM                 (SEC2_MAX_OUT_WIDTH) //not test yes
#define SEC4_MAX_IN_WIDTH                       (512U)
#define PREVA_2X_MAX_WIDTH                      (640U)
#define PREVB_2X_MAX_WIDTH                      (2048U) //no extra 2x
#define PREVC_2X_MAX_WIDTH                      (384U)
#define PREV_MAX_RATIO_8X                       (8U)
#define PREV_MAX_RATIO_16X                      (16U)
#define WARP_DMA_MAX_VALUE                      (64U)
#define TILE_OVERLAP_WIDTH                      (128U)
#define TILE_WIDTH_ALIGN                        (32U)
#define WARP_GRID_EXP                           (4U)
#define EFCY_TILE_OVERLAP_WIDTH                 (64U)
#define MAX_TILE_NUM                            (8U)
#define VPROC_DRAM_SIZE                         (4U)    //0 = 2MB, use larger size to avoid assertion

#define IS_VP_MSG_CTRL_EXIST                    (1U)
#define IS_HEIR_FILTER_EXIST                    (1U)
#define IS_LNDT_FILTER_EXIST                    (1U)

#define CV2X_MAX_RAW_FRM_NUM                    (5U)
#define CV2X_MAX_C2Y_FB_NUM                     (2U)
#define CV2X_MAX_MAIN_FB_NUM                    (7U/*5U*/)
#define CV2X_MAX_PREV_FB_NUM                    (6U)
#define CV2X_DEFAULT_HIER_FB_NUM                (5U)
#define CV2X_DEFAULT_LNDT_FB_NUM                (5U)

#define CV2X_EXTRA_SLICE_CAP_LINE               (128U)
#define CV2X_WARP_WAITLINE                      (48U)
#define CV2X_WARP_OVERLAP_Y_HIER_ON             (128U)
#define CV2X_WARP_OVERLAP_Y_HIER_OFF            (32U)

#define CV2X_MAX_STL_C2Y_FB_NUM                 (2U)
#define CV2X_MAX_STL_MAIN_FB_NUM                (3U) // ucode pipeline depth is 2
#define CV2X_MAX_STL_PREVA_FB_NUM               (2U)
#define CV2X_MAX_STL_PREVB_FB_NUM               (2U)
#define CV2X_MAX_STL_PREVC_FB_NUM               (2U)

/* Project specific */
#define CV2X_CRUISE_LD_WIDTH                    (2880U)
#define CV2X_CRUISE_LD_HEIGHT                   (1860U)
#define CV2X_CRUISE_LD_TILE_OVERLAP_WIDTH       (32U)

#define CV2X_VPROC_DUMMY_SIZE                   (256U)

#endif //AMBADSP_ENCODEAPI_DEF_H
