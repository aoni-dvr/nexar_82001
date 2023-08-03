/**
 *  @file AmbaImg_Adjustment_Def.h
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
 *  @Description    :: Structure type definiton of ADJ
 *
 */

#ifndef AMBA_IMG_ADJUSTMENT_DEF_H
#define AMBA_IMG_ADJUSTMENT_DEF_H

#include "AmbaImg_Proc.h"
#include "AmbaImg_AeAwb.h"
#include "AmbaDSP_ImageFilter.h"

#define SCENE_UNIT              128

#define LS_TONE_CTRL_CONFIG_SIZE    4096
#define NUM_EXPOSURE_CURVE          256
#define NUM_CHROMA_GAIN_CURVE       128
#define NUM_BOOST_TABLE             IK_CE_BOOST_TABLE_SIZE

#define NUM_IN_LOOKUP               192*3
#define NUM_MATRIX                  16*16*16
#define NUM_OUT_LOOKUP              256

#define SIZE_IN                    (NUM_IN_LOOKUP*sizeof(UINT32))    /* 2112 bytes      NUM_IN_LOOKUP @ iav/iav_struct.h*/
#define CC_3D_SIZE                 IK_CC_3D_SIZE           /* (NUM_MATRIX*sizeof(UINT32))     16384 bytes      NUM_MATRIX @ iav/iav_struct.h*/
// #define CC_REG_SIZE                IK_CC_REG_SIZE
#define SIZE_OUT                   (NUM_OUT_LOOKUP*sizeof(UINT32))    /* 1024 bytes     NUM_OUT_LOOKUP @ iav/iav_struct.h*/
#define TONE_CURVE_SIZE            (256)

#define ADJ_EV_TABLE_COUNT    25
#define ADJ_NF_TABLE_COUNT    10
//#define ADJ_LISO_NF_TABLE_COUNT    12
#define ADJ_HISO_NF_TABLE_COUNT    10
#define ADJ_MOTOIN_SPEED_TABLE_COUNT   3

#define IQ_PARAMS_IMG_DEF                0U    //IMG_PARAM
#define IQ_PARAMS_AAA                    1U    //AAA_DEFAULT
#define IQ_VIDEO_ADJ_AWBAE               2U    //VIDEO
#define IQ_VIDEO_ADJ_FILTPARAM           3U
#define IQ_VIDEO_ADJ_MCTFINFO            4U
#define IQ_VIDEO_HDRSHORTPARAM           5U
#define IQ_STILL_ADJ_LISO_FILTERPARAM    6U    //STILL
#define IQ_STILL_ADJ_HISO_LIPARAM        7U
#define IQ_STILL_ADJ_HISO_HIPARAM        8U
#define IQ_PARAMS_VIDEO_IDX_INFO_ADJ     9U    //VIDEO IDX
#define IQ_PARAMS_STILL_IDX_INFO_ADJ     10U   //STILL IDX
#define IQ_PARAMS_VIDEO_MSM_ADJ          11U   //MOTION SPEED
#define IQ_PARAMS_VIDEO_MSH_ADJ          12U
#define IQ_PARAMS_PHOTO_MSM_ADJ          13U
#define IQ_PARAMS_PHOTO_MSH_ADJ          14U
#define IQ_PARAM_SCENE                   15U   //SCENE
#define IQ_PARAM_DE                      16U   //DE



typedef struct {
    INT32        Value[4];
} ADJ_LUT_4_s;

typedef struct {
    INT32        Value[8];
} ADJ_LUT_8_s;

typedef struct {
    INT32        Value[16];
} ADJ_LUT_16_s;

typedef struct {
    INT32        Value[24];
} ADJ_LUT_24_s;

typedef struct {
    INT32        Value[32];
} ADJ_LUT_s;

typedef struct {
    UINT32      VersionNumber;
    UINT32      ParamVersionNum;
    UINT8       MaxTableCount;
    ADJ_LUT_16_s Table[ADJ_EV_TABLE_COUNT];
} ADJ_AWB_AE_s;
#define ADJ_AWBAE_VER (0x18100100U)

typedef struct{
    UINT8 Coeffs[64];
}ADJ_COEFFICIENT_64_TABLE_s;

typedef struct {
    UINT8      Enable;
    UINT32     BoostGainShift;
    UINT32     BoostTableSizeExp;
    ADJ_LUT_16_s  LumaAvgCoringIndexTable[ADJ_EV_TABLE_COUNT];
    ADJ_LUT_16_s  CoringGainTable[ADJ_EV_TABLE_COUNT];
    ADJ_COEFFICIENT_64_TABLE_s CoringTable[ADJ_EV_TABLE_COUNT];
    INT32     BoostTable255[NUM_BOOST_TABLE];
    INT32     BoostTable128[NUM_BOOST_TABLE];
    INT32     BoostTable0[NUM_BOOST_TABLE];
} ADJ_CONTRAST_ENHANCE_s;

typedef struct {
    UINT8          TableCount;
    ADJ_LUT_16_s   Enable;
    ADJ_LUT_16_s   EvTable[ADJ_EV_TABLE_COUNT];
    ADJ_LUT_8_s    NfTable[ADJ_NF_TABLE_COUNT];
    UINT8          AQPEnable;
    ADJ_LUT_16_s   EvAQPTable[ADJ_EV_TABLE_COUNT];
    ADJ_CONTRAST_ENHANCE_s AdjContrastEnahance;
    ADJ_LUT_16_s   ContrastEnahanceFog[ADJ_EV_TABLE_COUNT];
} ADJ_FILTER_INFO_s;

typedef struct {
    UINT8        TableCount;
    ADJ_LUT_s    Enable;
    ADJ_LUT_s    EvTable[ADJ_EV_TABLE_COUNT];
} ADJ_HISO_FILTER_INFO_s;

typedef struct {
    UINT16    GainR;
    UINT16    GainB;
    ULONG     MatrixThreeDTableAddr;
    INT16     CcMatrix[9];
} ADJ_COLOR_CONTROL_s; //adj_color_control_t;

typedef struct {
    UINT8               Type;
    UINT8               Control;
    ADJ_COLOR_CONTROL_s Table[5];
} COLOR_3D_s; //color_3d_t;

typedef struct {
    UINT8                        BlackLevelEnable;
    UINT16                       StartShutterIdx;
    UINT16                       ShutterTableNo;
    ADJ_LUT_8_s                  BlackLevelBase;
    ADJ_LUT_8_s                  ShutterBlackLevel[25];
    ADJ_LUT_24_s                 BlackLevel[ADJ_NF_TABLE_COUNT];
} BlackLevel_INFO_s;


#define NUM_EXPOSURE_CURVE        256
#define NUM_CHROMA_GAIN_CURVE        128
typedef struct {

    COLOR_3D_s                   Color;
    BlackLevel_INFO_s            BlackLevelInfo;
    AMBA_IK_TONE_CURVE_s         Ratio255Gamma;
    AMBA_IK_TONE_CURVE_s         Ratio128Gamma;
    AMBA_IK_TONE_CURVE_s         Ratio0Gamma;
    UINT32                       ChromaCurve255[NUM_CHROMA_GAIN_CURVE];
    UINT32                       ChromaCurve128[NUM_CHROMA_GAIN_CURVE];
    UINT32                       ChromaCurve0[NUM_CHROMA_GAIN_CURVE];
} ADJ_DEF_s; //adj_def_t;

typedef struct {
     INT32 MinThreshold;
     INT32 MaxThreshold;
} ADJ_MOTION_SPEED_THRESHOLD_s;

typedef struct {
    INT32 Min;
    INT32 Mid;
    INT32 Max;
} ADJ_MS_THRESHOLD_s;

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

/* Adjustment parameters for agc noise control, WBGain Color control */
typedef struct {
    UINT16    P120Bin[2];
    UINT16    P120NoNin[2];
    UINT16    P60Bin[2];
    UINT16    P60NoBin[2];
    UINT16    P30Bin[2];
    UINT16    P30NoBin[2];
} ADJ_CFA_NOISE_RATIO_VIDEO_s; //adj_cfa_noise_ratio_video_t

typedef struct {
    UINT16    P120Bin[3];
    UINT16    P120NoNin[3];
    UINT16    P60Bin[3];
    UINT16    P60NoBin[3];
    UINT16    P30Bin[3];
    UINT16    P30NoBin[3];
} ADJ_LUMA_SHARPEN_RATIO_VIDEO_s; //adj_luma_sharpen_ratio_video_t


/**
 * Still AF Lock Info
 */
#define FOCUSED_AREA        1
#define NON_FOCUSED_AREA    0
typedef struct {
    UINT8     Focused;    /* 1 if FOCUSED_AREA; 0 otherwise */
    UINT16    ColStart;    /* focus area column start in capture window */
    UINT16    RowStart;    /* focus area row start in capture window */
    UINT16    Width;        /* focus area width in capture window */
    UINT16    Height;        /* focus area height in capture window */
    UINT32    Score;        /* focus area score in capture window */
} FOCUS_AREA_s; //focus_area_t

#define SAF_FOCUS_FOUND        1
#define SAF_FOCUS_NOT_FOUND    0
typedef struct {
    UINT8           LockState;    /* 1 if SAF_FOCUS_FOUND; 0 otherwise */
    UINT8           TotalAreas;    /* maximum number of focus areas */
    FOCUS_AREA_s    Area[40];    /* maximum 40 focus areas to support up to most AF tiles */
} STILL_AF_INFO_s; //still_af_info_t;

typedef struct {
    INT16     Str;
    UINT16    Offset;
    UINT16    Adapt;
} STR_INFO_s; //str_info_t

#define STR_MAX_VALUE        19U
typedef struct {
    STR_INFO_s    Info[24];
} STR_LUT_s;

typedef struct {
    INT16        Start;
    INT16        End;

    STR_LUT_s    Lut;
} LUT_CONTROL_s; //;lut_control_t

typedef struct {
        UINT16    Index;
    UINT32    PrevIdx;
    UINT32    NextIdx;
    INT32    PrevWeight;
    INT32    NextWeight;
    UINT8    ShiftBit;
} INTERPO_INFO_s;//___INTERPO_INFO_s;

typedef struct {
    INT32    PrevWeight;
    INT32    NextWeight;
    UINT8    ShiftBit;
} WB_INTERPO_s;//___WB_INTERPO_s;

typedef struct {
    INT32    DOWN;
    INT32    UP;
} LIMIT_s;//__LIMIT_s;

typedef struct {
    UINT16      LowTempRTarget;
    UINT16      LowTempBTarget;
    UINT16      D50RTarget;
    UINT16      D50BTarget;
    UINT16      HighTempRTarget;
    UINT16      HighTempBTarget;
    UINT16      AeTarget;
    UINT16      AutoKnee;
    UINT16      SrvAeDgnMin;
    UINT16      SrvAeDgnMax;
    UINT16      MVinDgainMax;
    UINT16      MVinWbLimit;
} ADJ_AEAWB_CONTROL_s; //adj_aeawb_control_t;

typedef struct {
    UINT8        TableCount;
    UINT8        Enable;
    INT16        Start;
    INT16        End;

    STR_LUT_s    Lut[12];
}ADJ_STR_CONTROL_s; //adj_str_control_t;

#define ISO_IDX_TYPE    0U
#define AGC_IDX_TYPE    1U
typedef struct {
    UINT16    IdxType;
    INT32     MinValue;
    INT32     MinNf;
} ADJ_IDX_INFO_s; //adj_idx_info_t;

typedef struct {
    UINT32            VersionNumber;
    UINT32            ParamVersionNum;
    ADJ_IDX_INFO_s    LowIso;
    ADJ_IDX_INFO_s    HighIso;
} ADJ_STILL_IDX_INFO_s;
#define ADJ_STILL_IDX_INFO_VER (0x14051500U)

typedef struct {
    UINT32            VersionNumber;
    UINT32            ParamVersionNum;
    ADJ_IDX_INFO_s    LowIso;
    ADJ_IDX_INFO_s    HighIso;
} ADJ_VIDEO_IDX_INFO_s;
#define ADJ_VIDEO_IDX_INFO_VER (0x14100800U)

typedef struct {
    UINT8                        Mode;
    AMBA_AE_INFO_s               Ae;
    AMBA_IK_WB_GAIN_s       Wb;
    UINT16                       DZoomStep;
    UINT32                       AwbAeParamAdd;
    UINT32                       FilterParamAdd;
    UINT32                       HdrShortParamAdd;
    UINT16                       AdjTableNo;
    UINT32                       HisoFilterParamAdd;
    UINT32                       VideoMSMParamAdd;
    UINT32                       VideoMSHParamAdd;
} ADJ_IQ_INFO_s;

typedef struct {
    UINT8                  VinNum;
    UINT8                  Mode;
    UINT16                 EvIndex;
    AMBA_IK_WB_GAIN_s      WbGain;
    UINT32                 AwbAeParamAdd;
} ADJ_AWBAE_CONTROL_s;

typedef struct {
    UINT8                  Mode;
    UINT8                  Enable;
    UINT8                  VinNum;
    UINT16                 ShIndex;
    UINT16                 EvIndex;
    UINT16                 NfIndex;
    AMBA_IK_WB_GAIN_s      WbGain;
    UINT16                 DZoomStep;
    UINT32                 FilterParamAdd;
} ADJ_VIDEO_CONTROL_s;

typedef struct {
    UINT8                     StillMode;
    UINT16                    ShIndex;
    UINT16                    EvIndex;
    UINT16                    NfIndex;
    AMBA_IK_WB_GAIN_s         WbGain;
    UINT16                    DZoomStep;
    UINT8                     FlashMode;
    UINT8                     LutNo;
} ADJ_STILL_CONTROL_s;


#define ADJ_SENSOR_X1PC        201
#define ADJ_SENSOR_X2PC        202
#define ADJ_SENSOR_X3PC        203
#define ADJ_SENSOR_X4PC        204
#define ADJ_SENSOR_X8PC        205

#define ADJ_SENSOR_X1TV        101
#define ADJ_SENSOR_X2TV        102
#define ADJ_SENSOR_X3TV        103
#define ADJ_SENSOR_X4TV        104
#define ADJ_SENSOR_X8TV        105

#define ADJ_R3840x2160P030V1C1X01  118042001
#define ADJ_R_P_V_C_X__  100000000


#define ADJ_R1920X1080P030V1C1X02  104042202
#define ADJ_R1280X0720P030V1C1X02  108042202
#define ADJ_R1920X1080P030V1C1X01  104042201

#define ADJ_R1920X1080P060V0C1X01  104091201
#define ADJ_R2304X1296P030V1C1X01  103042201

#define ADJ_R1920X1080P045V0C1X01  104061201

#define ADJ_R2560X1080P030V1C1X01  102042201
#define ADJ_R_________P030V_C_X__  100040000
#define ADJ_R2304X1296P030V1C1X01  103042201
#define ADJ_R1920X1080P045V0C1X01  104061201

#define ADJ_R1920X1080P060V0C1X02  104091202
#define ADJ_R1920X1080P048V0C1X02  104071202
#define ADJ_R1920X1080P024V1C1X02  104022202
#define ADJ_R_________P060V0C0X01  100091101
#define ADJ_R_________P030V0C0X01  100041101
#define ADJ_R1280X0720P060V0C1X02  108091202

/******************************************************

#define ADJ_R2560X1080P030V1C1X01  102042201

  ADJ_ R2560X1080  P030  V1  C1  X01
              102    04   2   2   01

 R_________ : 100  ignore
 R2560X1440 : 101    R2560X1080 : 102    R2304X1296 : 103
 R1920X1080 : 104    R1600X1200 : 105    R1600X0900 : 106
 R1440X1080 : 107    R1280X0720 : 108    R1280X0960 : 109
 R0960X0540 : 110    R0848X0480 : 111    R0720X0480 : 112
 R0640X0480 : 113    R0640X0360 : 114    R0432X0240 : 115
 R0352X0240 : 116    R0320X0240 : 117    R3840X2160 : 118

 P___ : 00  ignore
 P015 : 01    P024 : 02    P025 : 03    P030 : 04    P040 : 05
 P045 : 06    P048 : 07    P050 : 08    P060 : 09    P100 : 10
 P120 : 11    P200 : 12    P240 : 13

 V_ : 0   ignore
 V0 : 1   oversaming mode is not enabled
 V1 : 2   oversaming mode is enabled

 C_ : 0   ignore
 C0 : 1   TV mode
 C1 : 2   PC mode

 X__ : 00   ignore
 X01 : 01   non_Binnning        X02 : 02  Binnning_2X
 X03 : 03   Binnning_3X         X04 : 04  Binnning_4X
 X05 : 05   Binnning_8X

**********************************************************/
typedef struct {
    UINT32    Define;
    UINT32    Resolution;
    UINT32    FrameRate;
    UINT8     OverSampling;
    UINT8     ColorStyle;
    UINT32    Binning;
} ADJ_VIDEO_MODE_s;

#define ADJ_TABLE_COUNT_MAX_NUM    8
#define ADJ_TABLE_SAME_MAX_NUM    8

typedef struct {
    UINT32    MaxTableCount;
    UINT32    MaxTableSame;
    UINT32    Index[ADJ_TABLE_COUNT_MAX_NUM][ADJ_TABLE_SAME_MAX_NUM];
} ADJ_TABLE_IDX_s;

typedef struct {
    UINT32             VersionNum;
    UINT32             ParamVersionNum;
    ADJ_TABLE_IDX_s    Video;
    ADJ_TABLE_IDX_s    PhotoPreview;
} ADJ_TABLE_PARAM_s;
#define ADJ_TABLE_PARAM_VER (0x14010300)

typedef struct {
    UINT8        UpdateFlg;
    ADJ_LUT_16_s    AQPParams;
} ADJ_AQP_INFO_s;


#endif  /*__AMBA_ADJUSTMENT_DEF_H__ */
