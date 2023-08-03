/**
 *  @file AmbaSensor_MAX9295_96712_IMX390Table.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Control APIs of MAX9295_96712 plus SONY IMX390 CMOS sensor with MIPI interface
 *
 */

//#include <AmbaSSP.h>

//#include "AmbaDSP.h"
#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_96712_IMX390.h"
#include "AmbaSensor_MAX9295_96712_IMX390_IOUT.h"
#define HJ6040_F28
//#define J8300_F20
/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/

const AMBA_SENSOR_OUTPUT_INFO_s MX01_IMX390_VCC_OutputInfo[MX01_IMX390_NUM_MODE] = {
    /* MX01_IMX390_1920_1080_60P_HDR   */ {500000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1936, 1110, {8, 18, 1920, 1080}, {0}},
    /* MX01_IMX390_1920_1080_30P       */ {500000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1936, 1110, {8, 18, 1920, 1080}, {0}},
    /* MX01_IMX390_1920_1080_30P_HDR   */ {500000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1936, 1110, {8, 18, 1920, 1080}, {0}},
    /* MX01_IMX390_1936_1106_60P_HDR   */ {500000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1936, 1110, {0,  0, 1936, 1106}, {0}},
    /* MX01_IMX390_1936_1106_30P       */ {500000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1936, 1110, {0,  0, 1936, 1106}, {0}},
    /* MX01_IMX390_1920_1080_30P_HDR_1 */ {500000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1936, 1110, {8, 18, 1920, 1080}, {0}},

#ifndef MX01_CPHY
    /* MX01_IMX390_1920_1080_60P_HDR_1 */ {500000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1936, 1110, {8, 18, 1920, 1080}, {0}},
#else
    /* MX01_IMX390_1920_1080_60P_HDR_1 */ {1142857143, 2, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1936, 1110, {8, 18, 1920, 1080}, {0}}, //500M SR -> 1142857143, 1G SR -> 2285714286
#endif
    /* MX01_IMX390_1920_1080_30P_HDR_2 */ {500000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1936, 1110, {8, 18, 1920, 1080}, {0}},
    /* MX01_IMX390_1920_1080_30P_HDR_3 */ {500000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1936, 1110, {8, 18, 1920, 1080}, {0}},

    /* MX01_IMX390_1920_1080_A60P_HDR  */ {500000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1936, 1110, {8, 18, 1920, 1080}, {0}},
    /* MX01_IMX390_1936_1106_A60P_HDR  */ {500000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1936, 1110, {0,  0, 1936, 1106}, {0}},
    /* MX01_IMX390_1920_1080_A30P_HDR  */ {500000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1936, 1110, {8, 18, 1920, 1080}, {0}},
};