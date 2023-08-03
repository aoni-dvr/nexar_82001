/**
 *  @file AmbaCameraModule.h
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
#ifndef AMBA_CAMERA_MODULE_H
#define AMBA_CAMERA_MODULE_H

#include "AmbaDSP_ImageFilter.h"
#include "AmbaCalib_VignetteCV2Def.h"


//The following define is used on module/master version 02
#define MASTERVERSION    (0x2)
#define ONED_TBL_MAX_SIZE 64
#define VIG_ONED_GAIN_TABLE_SIZE 128
#define VIG_ONED_GAIN_DIRECTION 4
#define VIG_ONED_GAIN_CH 4

typedef enum {
    BLACKLEVEL          = 0x42,
    WHITEBLANCE         = 0x57,
    CAMERA_INTRINSIC    = 0x01,
    LDC_ONED_TABLE      = 0x02,
    CA_ONED_TABLE       = 0x03,
    VIG_ONED_TABLE      = 0x04,
    BADPIXELS           = 0x05,
    RESERVED_A9AQ_1     = 0x49,
    RESERVED_A9AQ_2     = 0x44,
    RESERVED_A9AQ_3     = 0x43,
    RESERVED_A9AQ_4     = 0x53,
    RESERVED_A9AQ_5     = 0x56,
    RESERVED_A9AQ_6     = 0x46
} MODULE_CAL_ID_e;

typedef struct {
    UINT32 X;
    UINT32 Y;
} XY_POSITION_UINT32_S;

typedef struct {
    DOUBLE ScaleFactorX;
    DOUBLE ScaleFactorY;
    DOUBLE Skew;
    XY_POSITION_UINT32_S Center;
    DOUBLE FocusLength;
} FULLVIN_INTRINSIC_INFORMATION_s;

typedef struct {
    DOUBLE DISTORTION_REAL[ONED_TBL_MAX_SIZE];
    DOUBLE DISTORTION_EXPECT[ONED_TBL_MAX_SIZE];
} LENS_DISTORTION_TABLE_s;

typedef struct {
    DOUBLE CA_EXCEPT[ONED_TBL_MAX_SIZE];
    DOUBLE CA_REDLINE[ONED_TBL_MAX_SIZE];
    DOUBLE CA_BLUELINE[ONED_TBL_MAX_SIZE];
} CHROMA_ABERRATION_TABLE_s;

typedef AMBA_CAL_1D_VIG_CALIB_DATA_s ONED_VIG_CALIB_DATA_s;

typedef struct {
    INT16   BlackR;
    INT16   BlackGr;
    INT16   BlackGb;
    INT16   BlackB;
} BLACK_LEVEL_s;

typedef struct {
    UINT32 GainR;       /**< Red color gain value */
    UINT32 GainG;       /**< Green color gain value */
    UINT32 GainB;       /**< Blue color gain value */
} WB_Gain_s;

typedef struct {
    WB_Gain_s OrgWB[2];  //< original wb, 2: LCT and HCT
    WB_Gain_s RefWB[2];  //< reference wb, 2: LCT and HCT
} WHITE_BLANCE_s;

typedef  struct {
    AMBA_IK_VIN_SENSOR_GEOMETRY_s  CalibVinSensorGeo;  //< vin sensor geometry
    UINT8                               OBEnable;           //< Enable flag for OB
    UINT32                              MaskSize;           //< size of unpacked size
    UINT32                              PackedSize;         //< packed size
    void                                *CompressedMap;     //< Compressed bad pixel map address in momory
}BAD_PIXELS_s;

typedef struct {
    UINT32 Version; //0x(MODULE_CAL_ID_e)(masterVersion)(subVersion): 0x(I)(002)(000)
    UINT32 Size;
    UINT32 CheckSum;
}Modual_Cal_Header_s;

typedef struct {
    UINT64 Version;
    UINT32 ModuleID;
    UINT32 Reserve;
    //Modual_V2_Cal_Header_s Cal_Header[n]
    //UINT32 CheckSum
}Modual_Cal_Param_s;

#endif //AMBA_CAMERA_MODULE_H


