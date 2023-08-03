/**
 *  @file AmbaEIS_DeCfg.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 *
 *  @details AmbaEIS Defult config
 *
 */

#ifndef AMBA_EIS_DECFG
#define AMBA_EIS_DECFG


#include "AmbaEIS_Control.h"


static const AMBA_EISCtrl_CFG gEisCfgList[] = {
    {
        .ModeName         = "Default_0 IMX577_J5013 (3840x2160)",
        .FocalLength      = 360U,
        .Reserve1         = 0U,
        .Reserve2         = 0U,
        .Reserve3         = 0U,
        .Reserve4         = 0U,
        .Reserve5         = 0U,
        .WarpInfo       = {.HorGridNum = MAX_WARP_TBL_H_GRID_NUM, .VerGridNum = MAX_WARP_TBL_V_GRID_NUM, .TileWidthExp = 7U, .TileHeightExp = 7U},
        .LDCInfo        = {0U, {3840U, 2160U, 5.952, 0, 0}, 34U,
                          {0.0000, 0.0909, 0.1819, 0.2731, 0.3645, 0.4561, 0.5481, 0.6405, 0.7335, 0.8270,
                           0.9212, 1.0162, 1.1121, 1.2091, 1.3072, 1.4066, 1.5074, 1.6099, 1.7142, 1.8207,
                           1.9295, 2.0409, 2.1554, 2.2733, 2.3953, 2.5218, 2.6537, 2.7919, 2.9375, 3.0920,
                           3.2573, 3.4356, 3.6297, 3.8421, 0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                           0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                           0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                           0.0,    0.0,    0.0,    0.0},
                          {0.0000, 0.0909, 0.1820, 0.2733, 0.3650, 0.4571, 0.5498, 0.6432, 0.7375, 0.8328,
                           0.9292, 1.0268, 1.1258, 1.2264, 1.3288, 1.4331, 1.5394, 1.6481, 1.7594, 1.8734,
                           1.9904, 2.1107, 2.2347, 2.3625, 2.4947, 2.6315, 2.7735, 2.9211, 3.0748, 3.2353,
                           3.4032, 3.5793, 3.7644, 3.9596, 0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                           0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                           0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                           0.0,    0.0,    0.0,    0.0},
                           0,
                          },
        .CompRange        ={12U, 5U},
        .PostFact         ={.Enable = 0U, .CropWidth  = 200U, .CropHeight = 200U, .ShiftX = 0, .ShiftY = 0},
    },
    {
        .ModeName         = "Default_1 IMX577_J5013 (3840x2160)",
        .FocalLength     = 360U,
        .Reserve1         = 0U,
        .Reserve2         = 0U,
        .Reserve3         = 0U,
        .Reserve4         = 0U,
        .Reserve5         = 0U,
        .WarpInfo         = {.HorGridNum = MAX_WARP_TBL_H_GRID_NUM, .VerGridNum = MAX_WARP_TBL_V_GRID_NUM, .TileWidthExp = 7U, .TileHeightExp = 7U},
        .LDCInfo          = {3U, {3840U, 2160U, 5.952, 0, 0}, 34U,
                           {0.0000, 0.0909, 0.1819, 0.2731, 0.3645, 0.4561, 0.5481, 0.6405, 0.7335, 0.8270,
                            0.9212, 1.0162, 1.1121, 1.2091, 1.3072, 1.4066, 1.5074, 1.6099, 1.7142, 1.8207,
                            1.9295, 2.0409, 2.1554, 2.2733, 2.3953, 2.5218, 2.6537, 2.7919, 2.9375, 3.0920,
                            3.2573, 3.4356, 3.6297, 3.8421, 0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                            0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                            0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                            0.0,    0.0,    0.0,    0.0},
                           {0.0000, 0.0909, 0.1820, 0.2733, 0.3650, 0.4571, 0.5498, 0.6432, 0.7375, 0.8328,
                            0.9292, 1.0268, 1.1258, 1.2264, 1.3288, 1.4331, 1.5394, 1.6481, 1.7594, 1.8734,
                            1.9904, 2.1107, 2.2347, 2.3625, 2.4947, 2.6315, 2.7735, 2.9211, 3.0748, 3.2353,
                            3.4032, 3.5793, 3.7644, 3.9596, 0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                            0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                            0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                            0.0,    0.0,    0.0,    0.0},
                            0,
                            },
        .CompRange        ={12U, 5U},
        .PostFact         ={.Enable = 1U, .CropWidth  = 200U, .CropHeight = 200U, .ShiftX = 0, .ShiftY = 0},
    },
    {
        .ModeName         = "(MODE 0) ST6176_3M1_BW (2592x1440)",
        .FocalLength     = 212U,
        .Reserve1         = 0U,
        .Reserve2         = 0U,
        .Reserve3         = 0U,
        .Reserve4         = 0U,
        .Reserve5         = 0U,
        .WarpInfo         = {.HorGridNum = MAX_WARP_TBL_H_GRID_NUM, .VerGridNum = MAX_WARP_TBL_V_GRID_NUM, .TileWidthExp = 6U, .TileHeightExp = 6U},
        .LDCInfo          = {3U, {2592U, 1440U, 5.184, 0, 0}, 46U,
                            {0.0000, 0.0266, 0.0800, 0.1334, 0.1869, 0.2405, 0.2943, 0.3481, 0.4022, 0.4565,
                             0.5110, 0.5658, 0.6208, 0.6762, 0.7318, 0.7879, 0.8443, 0.9011, 0.9582, 1.0158,
                             1.0739, 1.1324, 1.1913, 1.2508, 1.3107, 1.3711, 1.4321, 1.4935, 1.5555, 1.6181,
                             1.6812, 1.7448, 1.8090, 1.8737, 1.9390, 2.0049, 2.1046, 2.2056, 2.3077, 2.4109,
                             2.5150, 2.6200, 2.7256, 2.8319, 2.9384, 3.0805, 0.0,    0.0,    0.0,    0.0,
                             0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                             0.0,    0.0,    0.0,    0.0},
                            {0.0000, 0.0266, 0.0800, 0.1335, 0.1872, 0.2411, 0.2953, 0.3499, 0.4050, 0.4606,
                             0.5168, 0.5737, 0.6314, 0.6899, 0.7494, 0.8100, 0.8717, 0.9348, 0.9992, 1.0652,
                             1.1329, 1.2025, 1.2740, 1.3478, 1.4239, 1.5026, 1.5843, 1.6690, 1.7572, 1.8491,
                             1.9451, 2.0457, 2.1513, 2.2624, 2.3796, 2.5036, 2.7042, 2.9251, 3.1702, 3.4446,
                             3.7549, 4.1096, 4.5202, 5.0028, 5.5796, 6.5545, 0.0,    0.0,    0.0,    0.0,
                             0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                             0.0,    0.0,    0.0,    0.0},
                            0U,
                            },
        .CompRange        ={10U, 3U},
        .PostFact         ={.Enable = 0U, .CropWidth  = 0U, .CropHeight = 0U, .ShiftX = 0, .ShiftY = 0},
    },
    {
        .ModeName         = "(MODE 2) IMX577_JC450 (3840x2160)",
        .FocalLength     = 375U,
        .Reserve1         = 0U,
        .Reserve2         = 0U,
        .Reserve3         = 0U,
        .Reserve4         = 0U,
        .Reserve5         = 0U,
        .WarpInfo         = {.HorGridNum = MAX_WARP_TBL_H_GRID_NUM, .VerGridNum = MAX_WARP_TBL_V_GRID_NUM, .TileWidthExp = 6U, .TileHeightExp = 6U},
        .LDCInfo          = {3U, {3840U, 2160U, 5.952, 0, 0 }, 34U,
                            { 0.0000, 0.1273, 0.2546, 0.3818, 0.5088, 0.6356, 0.7621, 0.8882, 1.0139, 1.1392,
                              1.2639, 1.3881, 1.5117, 1.6345, 1.7566, 1.8779, 1.9983, 2.1179, 2.2364, 2.3539,
                              2.4703, 2.5855, 2.6995, 2.8121, 2.9234, 3.0333, 3.1415, 3.2481, 3.3530, 3.4560,
                              3.5570, 3.6559, 3.7527, 3.8781, 0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                              0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                              0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                              0.0,    0.0,    0.0,    0.0},
                            { 0.0000, 0.1274, 0.2552, 0.3836, 0.5132, 0.6441, 0.7769, 0.9120, 1.0497, 1.1906,
                              1.3351, 1.4838, 1.6374, 1.7965, 1.9618, 2.1343, 2.3149, 2.5047, 2.7051, 2.9174,
                              3.1436, 3.3855, 3.6458, 3.9273, 4.2337, 4.5694, 4.9397, 5.3517, 5.8141, 6.3381,
                              6.9389, 7.6368, 8.4596, 9.8225, 0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                              0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                              0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,
                              0.0,    0.0,    0.0,    0.0},
                             0U,
                             },
        .CompRange        ={10U, 3U},
        .PostFact         ={.Enable = 1U, .CropWidth  = 200U, .CropHeight = 200U, .ShiftX = 0, .ShiftY = 0},
    },
};
#endif

