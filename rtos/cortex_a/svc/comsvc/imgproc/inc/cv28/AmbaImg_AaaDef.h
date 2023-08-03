/**
 *  @file AmbaImg_AaaDef.h
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
 *  @Description    :: Structure type Definiton of ADJ
 *
 */
#ifndef AMBA_Img_AAADEF_H
#define AMBA_Img_AAADEF_H

#include "AmbaImg_AeAwb.h"
#include "AmbaImg_Adjustment.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaImg_Proc.h"

#define DEF_CC_TABLE_NO           (5U)

/**
* Capturing format
**/
#define IMG_CAP_NORMAL                 0x0000
#define IMG_CAP_AEB                    0x0001

/**
* histogram length
**/
#define    HISTOGRAM_LEN          64U
#define    HISTOGRAM_LEN_CFA      64U
#define    HISTOGRAM_LEN_RGB      64U
#define    HISTOGRAM_LEN_VIN      128U
#define    HISTOGRAM_LEN_HDR      128U

/*
 * AE Metering Mode definition
 */
#define AE_METER_CENTER        0
#define AE_METER_AVERAGE       1
#define AE_METER_SPOT          2
#define AE_METER_MANUAL        255

/*
 * AE Bias Unit Definition
*/
#define AE_BIAS_UNIT        32U

/* Flash mode setting */
#define FLASH_ALWAYS_OFF   0U
#define FLASH_AUTO         1U
#define FLASH_ALWAYS_ON    2U

/* Flash Flow Related defnitions */
#define PREFLASH_ON             0xFFFF
#define FLASH_IDX_BASE          0    /**< use flash index as flash strenght */
#define FLASH_TIM_BASE          1    /**< use flash time(us) as flash strenght */

/*
 * AE Flash Mode definition
 */
#define AE_FLASH_NORMAL         0U
#define AE_FLASH_SLOW           1U

/*
 * AE iris Settings definition
 */
#define AE_IRIS_AUTO        0U
#define AE_IRIS_1           100U
#define AE_IRIS_1P12        112U
#define AE_IRIS_1P25        125U
#define AE_IRIS_1P4         140U
#define AE_IRIS_1P6         160U
#define AE_IRIS_1P75        175U
#define AE_IRIS_2           200U
#define AE_IRIS_2P25        225U
#define AE_IRIS_2P5         250U
#define AE_IRIS_2P8         280U
#define AE_IRIS_3P2         320U
#define AE_IRIS_3P5         350U
#define AE_IRIS_4           400U
#define AE_IRIS_4P5         450U
#define AE_IRIS_5           500U
#define AE_IRIS_5P6         560U
#define AE_IRIS_6P3         630U
#define AE_IRIS_7P1         710U
#define AE_IRIS_8           800U
#define AE_IRIS_9           900U
#define AE_IRIS_10          1000U
#define AE_IRIS_11          1100U
#define AE_IRIS_12P6        1260U
#define AE_IRIS_14P2        1420U
#define AE_IRIS_16          1600U
#define AE_IRIS_18          1800U
#define AE_IRIS_20          2000U
#define AE_IRIS_22          2200U
#define AE_IRIS_25          2500U
#define AE_IRIS_28          2800U
#define AE_IRIS_32          3200U
#define AE_IRIS_36          3600U
#define AE_IRIS_40          4000U
#define AE_IRIS_44          4400U
#define AE_IRIS_50          5000U
#define AE_IRIS_56          5600U
#define AE_IRIS_64          6400U
#define AE_IRIS_72          7200U
#define AE_IRIS_80          8000U
#define AE_IRIS_MIN         65534U
#define AE_IRIS_MAX         65535U

/*
 * AE ISO Settings definition
 */
#define AE_ISO_AUTO        0U
#define AE_ISO_AUTO_HISO   1U
#define AE_ISO_6           6U
#define AE_ISO_12          12U
#define AE_ISO_25          25U
#define AE_ISO_50          50U
#define AE_ISO_100         100U
#define AE_ISO_200         200U
#define AE_ISO_400         400U
#define AE_ISO_800         800U
#define AE_ISO_1600        1600U
#define AE_ISO_3200        3200U
#define AE_ISO_6400        6400U
#define AE_ISO_12800       12800U
#define AE_ISO_25600       25600U
#define AE_ISO_51200       51200U
#define AE_ISO_102400      102400U
#define AE_ISO_204800      204800U
#define AE_ISO_MIN         2U
#define AE_ISO_MAX         3U

/*
 * AE shutter Settings definition
 */
#define AE_SHUTTER_AUTO       0U
#define AE_SHUTTER_8          1U
#define AE_SHUTTER_7          26U
#define AE_SHUTTER_6          53U
#define AE_SHUTTER_5          87U
#define AE_SHUTTER_4          128U
#define AE_SHUTTER_3P2        169U
#define AE_SHUTTER_3          182U
#define AE_SHUTTER_2P5        215U
#define AE_SHUTTER_2          256U
#define AE_SHUTTER_1P6        297U
#define AE_SHUTTER_1P3        336U
#define AE_SHUTTER_1          384U
#define AE_SHUTTER_1D1P25     425U    //0.8
#define AE_SHUTTER_1D1P67     478U    //0.6
#define AE_SHUTTER_1D2        512U
#define AE_SHUTTER_1D2P5      553U    //0.4
#define AE_SHUTTER_1D3        606U    //0.3
#define AE_SHUTTER_1D4        640U    //0.25
#define AE_SHUTTER_1D5        681U
#define AE_SHUTTER_1D6P25     722U
#define AE_SHUTTER_1D8P       768U
#define AE_SHUTTER_1D10       809U
#define AE_SHUTTER_1D12P5     850U
#define AE_SHUTTER_1D15       884U
#define AE_SHUTTER_1D20       937U
#define AE_SHUTTER_1D25       978U
#define AE_SHUTTER_1D30       1012U
#define AE_SHUTTER_1D40       1065U
#define AE_SHUTTER_1D50       1106U
#define AE_SHUTTER_1D60       1140U
#define AE_SHUTTER_1D80       1193U
#define AE_SHUTTER_1D100      1234U
#define AE_SHUTTER_1D120      1268U
#define AE_SHUTTER_1D160      1321U
#define AE_SHUTTER_1D200      1362U
#define AE_SHUTTER_1D240      1396U
#define AE_SHUTTER_1D320      1449U
#define AE_SHUTTER_1D400      1490U
#define AE_SHUTTER_1D500      1532U
#define AE_SHUTTER_1D640      1577U
#define AE_SHUTTER_1D800      1618U
#define AE_SHUTTER_1D1000     1660U
#define AE_SHUTTER_1D1250     1701U
#define AE_SHUTTER_1D1600     1746U
#define AE_SHUTTER_1D2000     1788U
#define AE_SHUTTER_1D2500     1829U
#define AE_SHUTTER_1D3200     1874U
#define AE_SHUTTER_1D4000     1916U
#define AE_SHUTTER_1D5000     1957U
#define AE_SHUTTER_1D6400     2002U
#define AE_SHUTTER_1D8000     2044U
#define AE_SHUTTER_MIN        65534U
#define AE_SHUTTER_MAX        65535U

/* AE P mode setting */
#define AE_P_MODE_OFF           0U
#define AE_P_MODE_ISO_IRIS      1U
#define AE_P_MODE_ISO_SHUTTER   2U
#define AE_P_MODE_IRIS_ISO      3U
#define AE_P_MODE_IRIS_SHUTTER  4U
#define AE_P_MODE_SHUTTER_ISO   5U
#define AE_P_MODE_SHUTTER_IRIS  6U

/*
 * AE Anti Flicker Mode definition
 */
#define ANTI_FLICKER_AUTO    0U
#define ANTI_FLICKER_60HZ    1U
#define ANTI_FLICKER_50HZ    2U
#define ANTI_FLICKER_NO_60HZ    10U
#define ANTI_FLICKER_NO_50HZ    20U


/*
 * AE Scene Mode definition
 */
#define AE_AGC_ONLY              0U
#define AE_SHUTTER_FIRST         1U
#define AE_SHUTTER_FIRST_AGC_NO  2U
#define AE_SHUTTER_ONLY          3U
#define AE_SHUTTER_NO            4U
#define AE_APERTURE_FIRST        5U
#define AE_APERTURE_FIRST_AGC_NO 6U
#define AE_APERTURE_ONLY         7U
#define AE_APERTURE_NO           8U
#define AE_DGAIN_ONLY            9U

/*
 * AWB Mode definition
 */
#define    WB_AUTOMATIC        0U
#define    WB_INCANDESCENT     1U
#define    WB_D4000            2U
#define    WB_D5000            3U
#define    WB_SUNNY            4U
#define    WB_CLOUDY           5U
#define    WB_D9000            6U
#define    WB_D10000           7U
#define    WB_FLASH            8U
#define    WB_FLUORESCENT      9U
#define    WB_FLUORESCENT_2    10U
#define    WB_FLUORESCENT_3    11U
#define    WB_FLUORESCENT_4    12U
#define    WB_WATER            13U
#define    WB_OUTDOOR          14U
#define    WB_LASTMODE         14U

#define    WB_DISABLE          254U

/*
 * AWB Menu Mode definition
 */
#define WB_MENU_REGION        0U
#define WB_MENU_FIX           1U

/*
 * AWB Metering Mode definition
 */
#define AWB_METER_CENTER    0U
#define AWB_METER_AVERAGE   1U
#define AWB_METER_SPOT      2U

#define AWB_NO_WHITE_LAST_GAIN          0U
#define AWB_NO_WHITE_DEFAULT_GAIN       1U

#define AWB_TILE_ALL                    0U
#define AWB_TILE_RIGHT                  2U
#define AWB_TILE_LEFT                   1U

//AF
/*
 * FOCUS SEARCH RANGE definition
 */
#define AF_FULL_RANGE        0
#define AF_STANDARD          1
#define AF_INFINITY          2
#define AF_MACRO             3
#define AF_SUPER_MACRO       4

/*
 * CAF Area definition
 */
#define CAF_AREA_NORMAL      0
#define CAF_AREA_CENTER      1
#define CAF_AREA2            2
#define CAF_AREA3            3
#define CAF_CUST_AREA        65535

/*
 * SAF Area definition
 */
#define SAF_AREA_MULTI_9     0
#define SAF_AREA_CENTER_1    1
#define SAF_AREA2            2
#define SAF_AREA3            3
#define SAF_CUST_AREA        65535

#define AF_PARAM_VER         0
#define CAF_PARAM            1
#define SAF_PARAM            2
#define ZTRACK_PARAM         3
#define TAF_PARAM            4
#define FTAF_PARAM           5
#define AF_PARAM_DIS         15

#define AF_TIME_OUT_MIN      1800    /* minimus af time-out timing for still af */

#define AF_PS_LEVEL0         0
#define AF_PS_LEVEL1         0
#define AF_PS_LEVEL2         0

#define SYSTEM_DEFAULT           255U
/*
 * Scene Mode definition
 */
#define SCENE_AUTO                      254
#define SCENE_FOG_DETECT_SZ             253
#define SCENE_FOG_DETECT_CAR_DV         252
#define SCENE_FOG_DETECT_EMIRROR        251

#define SCENE_OFF                       0
#define SCENE_DEFAULT                   0
#define SCENE_FLASH                     1
#define SCENE_DEFAULT_FOG_DETECT        1 //use the same number for SCENE_FOG_DETECT_DEFAULT and SCENE_FLASH
#define SCENE_TV_OFF                    2
#define SCENE_AV_OFF                    3
#define SCENE_SV_OFF                    4
#define SCENE_TV_ONLY                   5
#define SCENE_AV_ONLY                   6
#define SCENE_SV_ONLY                   7

#define SCENE_CAR_DV2                   6 //use the same number for CAR_DV2 and AV_ONLY
#define SCENE_CAR_DV_WFHD               7 //use the same number for CAR_DV_WFHD and SV_ONLY

#define    SCENE_NIGHT                     8
#define    SCENE_NIGHT_PORTRAIT            9
#define    SCENE_SPORTS                    10
#define    SCENE_LANDSCAPE                 11
#define    SCENE_PORTRAIT                  12
#define    SCENE_SUNSET                    13
#define    SCENE_SAND_SNOW                 14
#define    SCENE_FLOWER                    15

#define    SCENE_FIRE_WORK                 16
#define    SCENE_WATER                     17
#define    SCENE_BACK_LIGHT                18
#define    SCENE_BACK_LIGHT_PORTRAIT       19
#define    SCENE_TRIPOD                    20
#define    SCENE_BLUE_SKY                  21
#define    SCENE_MACRO                     22
#define    SCENE_MACRO_TEXT                23

#define    SCENE_ARENA                     24
#define SCENE_D_LIGHTING                   25
#define SCENE_MUSEUM                       26
#define    SCENE_BEACH                     27
#define    SCENE_CHILDREN                  28
#define    SCENE_PARTY                     29
#define    SCENE_FISHEYE                   30
#define    SCENE_INDOOR                    31

#define    SCENE_THROUGH_GLASS             32 //use the same number for DEFOG and THROUGH_GLASS
#define    SCENE_DEFOG                     32
#define    SCENE_PANNING                   33
#define    SCENE_PHOTO_FRAME               34
#define    SCENE_LOMO                      35
#define    SCENE_SELF_PORTRAIT             36
#define    SCENE_CAR_DV                    37
#define    SCENE_CAR_DV1                   38

#define    SCENE_CAR_HDR                   39
#define    SCENE_CAR_HDR1                  40

#define    SCENE_LAST                      40

#define    DGAIN_OFFSET_3DB                3
#define    DGAIN_OFFSET_6DB                6
#define    DGAIN_OFFSET_9DB                9
#define    DGAIN_OFFSET_12DB               12
#define    DGAIN_OFFSET_15DB               15
#define    DGAIN_OFFSET_18DB               18

typedef struct  {
    INT16   Brightness;
    UINT16  Contrast;
    UINT16  Saturation;
    INT16   Hue;
} IMAGE_PROPERTY_s;

typedef struct  {
    INT32 TaskEnableId[MAX_VIEW_NUM];
}IMGPROC_TASK_ENABLE_s;

//AE control settings
typedef struct  {
    UINT16    DefAeTarget;
    UINT8     CE_Index_Mode;
    //UINT8     VideoManualModeEnable;
    INT32     SlowShutter;
    UINT8     SlowShutterFps;         //60fps,50fps, 30fps,25fps, 15fps,12fps, 7fps,6fps
    UINT8     PhotoManualModeEnable;
    INT32     PhotoSlowShutter;
    INT32     RepsAgToDg;             //ReplaceAGCgainToDigitalGain
    UINT8     VideoAeSpeed;          //0~6
    UINT8     PhotoAeSpeed;          //0~6
    UINT8     MeteringMode;
    UINT8     FlashTableNumber;
    INT16     EvBias;
    UINT8     VideoFlash;
    UINT8     TargetStatisticDiffEnable;   //  VideoFlashFunction

    UINT8     StillFlash;
    UINT8     StillFlashType;
    UINT8     StillFlashFlickerChk;//0~4
    UINT8     StillNightShot;
    UINT8     StillIs;
    UINT8     StillFlickerChk;      //0~4
    UINT32    StillIris;
    UINT32    StillIso;
    UINT32    StillShutter;

    UINT8     StillPMode;
    INT16     StillPModeStr;       //1EV : 128

    UINT8     StillNormalPost;
    UINT8     StillFlashPost;
    UINT8     StillContinousPost;

    UINT8     TimeLapseEnable;
    UINT32    TimeLapseSkipFrames;

    UINT8     DualMainVideoEnable;
    UINT8     DualSecondVideoEnable;
    UINT8     DualSecondVideoAeTarget;
    INT16     DualSecondVideoEvBias;
    UINT8     DualSecondVideoAeSpeed;
    UINT8     DualSecondVideoMeteringMode;

    INT32     StillContinueSpeed;       //0~128
} AE_CONTROL_s;

typedef struct  {
    UINT16    Db240;
    UINT16    Db120;
    UINT16    Db60;
    UINT16    Db30;
    UINT16    Db15;
    UINT16    Db7p5;
    UINT16    DbLow;
} MAX_DB_INFO_s;

typedef struct  {

    INT32            MinAgcInfo;             //0db~6db
    MAX_DB_INFO_s    MaxAgcInfo[8];          //9db~27db
    MAX_DB_INFO_s    MaxDGainInfo[8];        //9db~27db
} AE_EV_INFO_s;

#define AA_TOTAL_01VIN_01AE     1001001
#define AA_TOTAL_02VIN_02AE     1002002
#define AA_TOTAL_03VIN_03AE     1003003
#define AA_TOTAL_04VIN_04AE     1004004
#define AA_TOTAL_01VIN_02AE     1001002
#define AA_TOTAL_02VIN_03AE     1002003

#define AA_NUM01VIN_NUM01AE     2001001
#define AA_NUM02VIN_NUM02AE     2002002
#define AA_NUM03VIN_NUM03AE     2003003
#define AA_NUM04VIN_NUM04AE     2004004
#define AA_NUM01VIN_NUM02AE     2001002
#define AA_NUM02VIN_NUM03AE     2002003

#define AA_TILE_LEFT         -1
#define AA_TILE_RIGHT        -2
#define AA_TILE_01_FRAME      1
#define AA_TILE_02_FRAMES     2
#define AA_TILE_03_FRAMES     3
#define AA_TILE_04_FRAMES     4
#define AA_TILE_05_FRAMES     5
#define AA_TILE_06_FRAMES     6
#define AA_TILE_07_FRAMES     7
#define AA_TILE_08_FRAMES     8
#define AA_TILE_09_FRAMES     9
#define AA_TILE_10_FRAMES    10
#define AA_TILE_11_FRAMES    11

typedef struct  {
    UINT8           FlickerMode;
    UINT16          VideoShutterMaxIdx;
    AE_EV_INFO_s    Lut;
    UINT32           VinTotal;
    UINT32           NumVinAe;
    UINT32           TilesSide;       // 0:All   1:Left   2:Right
} AE_EV_LUT_s;

/**
 * ISO setting for AE
 */
typedef struct  {
    INT32    Iso6AgcIdx;
    INT32    Iso12AgcIdx;
    INT32    Iso25AgcIdx;
    INT32    Iso50AgcIdx;
    INT32    Iso100AgcIdx; //0db
    INT32    Iso200AgcIdx; //6db
    INT32    Iso400AgcIdx; //12db
    INT32    Iso800AgcIdx; //18db
    INT32    Iso1600AgcIdx; //24db
    INT32    Iso3200AgcIdx; //30db
    INT32    Iso6400AgcIdx; //36db
    INT32    Iso12800AgcIdx; //42db
    INT32    Iso25600AgcIdx; //48db
    INT32    Iso51200AgcIdx; //56db
    INT32    Iso102400AgcIdx; //64db
    INT32    Iso204800AgcIdx; //72db
} AE_ISO_INFO_s;

typedef struct  {
    UINT8    Enable;                //EnableAeFaceDetection;
    UINT8    TargetGain;            //FaceDetection_exposure_Target_Gain to nirmal ae Target, unit:128
    UINT8    LumaPriority;          //FaceLumaPriority 0 ~ 16
    UINT8    GetFaceDelayFps;       //get_FacedelayFps relate to 30Fps
    UINT8    AfterGetFaceDelayFps;  //after_get_FacedelayFps  relate to 30Fps
} AE_FACE_DETECTION_s;

typedef struct  {

    UINT32    GlobalDGain;
    UINT16    VideoDarkLuma;
    UINT16    PhotoPreviewDarkLuma;
    UINT16    StillDarkLuma;

    UINT16    StillShutterMinNormal;
    UINT16    StillShutterMinNight;
    UINT16    StillShutterMinFlash;
    UINT16    StillShutterMinFlashSlow;
    UINT16    StillShutterMinContinue;
    UINT16    StillShutterMinBurst;
    UINT16    StillShutterMaxNormal;
    UINT16    StillShutterMaxFlash;

    AE_FACE_DETECTION_s    VideoFaceDetect;
    AE_FACE_DETECTION_s    StillFaceDetect;

    UINT8     HighLightLvNo;
    UINT8     OutdoorLvNo;
    UINT8     LowLightLvNo;
    LUT_CONTROL_s    FlashFocusDistanceTargetRatio; //unit:128
} AE_DEF_SETTING_s;

typedef struct  {

    AE_ISO_INFO_s       AeVideoIsoInfo;
    AE_ISO_INFO_s       AeStillIsoInfo;
    AE_DEF_SETTING_s    DefSetting;
    UINT8               RoiCnt;
    UINT8               RoiInfo[10][96];
    UINT16              LumaInfo[128];
} AE_ALGO_INFO_s;

 /**
 * ROI setting for AE
 */
typedef struct  {
    UINT16  Cnt;
    UINT8   Weight[AE_MAX_TILES];
} AE_ROI_INFO_s;

 /**
 * ROI setting for AE(VR)
 */
typedef struct  {
    UINT16  Cnt;
    UINT8   Weight[AE_MAX_TILES*MAX_VIEW_NUM];
} DS_AE_ROI_INFO_s;

/**
 * Lens Mechanical IRIS Information
 */
typedef struct  {
    UINT8  Id;
    UINT8  Type;
#define FIXED_IRIS          0x00U
#define INTERCHANGE_IRIS    0x01U
#define CONTINUOUS_IRIS        0x02U

#define IRIS_INVALID_INDEX    0U
#define FNO_MIN            0U
#define FNO_MAX            65535U
    UINT16 TotalIndex;        /**< total number of linear IRIS index steps supported **/
    UINT16 MinIndex;        /**< The IRIS index for the full-open condition **/
    UINT16 MaxIndex;        /**< The IRIS index for the minimum-open condition **/
    UINT16 DoubleStep;        /**< The total index number per EV (double exposure) **/

        UINT16 ActualIrisNum;
        UINT16 ActualIrisIdx[128];
} IRIS_INFO_s;

/**
 * ROI setting for AWB
 */
typedef struct  {
    UINT16  Cnt;
    UINT8   Weight[WB_MAX_TILES];
} AWB_ROI_INFO_s;

typedef struct  {
    UINT8    Method;
    UINT8    Speed;
    UINT8    VideoSkipFrames;
    UINT8    PhotoSkipFrames;
    UINT8    MenuMode;
    UINT8    MenuModeType;
    UINT8    MeteringMode;
    UINT8    VideoNoWhite;
    UINT8    StillNoWhite;

    INT32    StillContinueSpeed;       //0~128

    UINT8    TileSideChk[5];
} AWB_CONTROL_s;

//AWB G/B strip range Definition (at specific G/R)
typedef struct  {
    INT32    GrMin;
    INT32    GrMax;
    INT32    GbMin;
    INT32    GbMax;
    INT32    YAMinSlope;
    INT32    YAMin;
    INT32    YAMaxSlope;
    INT32    YAMax;
    INT32    YBMinSlope;
    INT32    YBMin;
    INT32    YBMaxSlope;
    INT32    YBMax;
    INT32    Weight;
} AWB_LUT_UINT_s;

//AWB lookup Table configuRation
typedef struct  {
    UINT8             Lut_No;
    AWB_LUT_UINT_s    AwbLut[20];
    UINT8             LumaWg[64];
} AWB_LUT_s;

typedef struct  {
    AMBA_IK_WB_GAIN_s    WbGain;
    UINT16      RgbRatio[3];
} DEFAULTMENU_s;

typedef struct  {
    AWB_LUT_s         WhiteRegionLut;
    UINT8             AwbLutNum[20][2];
    DEFAULTMENU_s     DefMenuInfo[20];

    UINT16            RoiCnt;
    UINT8             RoiInfo[10][WB_MAX_TILES];
} AWB_ALGO_INFO_s;

// EXIF debug info
typedef struct  {
    UINT32            Vin_Num;
    UINT8            Ae_Auto_Knee;
    UINT16            Ae_Tile_Row;
    UINT16            Ae_Tile_Columns;
    UINT16            Ae_Tiles_Avg[AE_MAX_TILES];
    UINT32            Awb_Tile_Row;
    UINT32            Awb_Tile_Column;
    UINT8            Awb_White_Chk[WB_MAX_TILES];
    UINT32            Awb_Rg_Chk[WB_MAX_TILES];
    UINT32            Awb_Bg_Chk[WB_MAX_TILES];
    UINT32            Awb_Luma_WG[WB_MAX_TILES];
    UINT32            Awb_Y[WB_MAX_TILES];
    UINT8            Awb_Outdoor_Chk;
    UINT8            Awb_No_White;
    UINT32            Awb_RG_Ratio[2];
} EXIF_DEBUG_INFO_s;

typedef struct  {
    UINT16  BinSize;
    UINT16  BinCount;
    UINT32 *LumHistogram;
} GET_LUMINANCE_HISTOGRAM_s;

typedef struct  {
    INT32    LumaStat[16];
    INT32    StrengthGain[16];
} FLASH_PREFLASH_INFO_s;

typedef struct  {
    UINT16                   VideoFlashAutoOnIso;
    UINT16                   VideoFlashAutoOnShutter;
    UINT16                   VideoFlashAutoOnIris;
    UINT16                   StillFlashAutoOnIso;
    UINT16                   StillFlashAutoOnShutter;
    UINT16                   StillFlashAutoOnIris;
    UINT16                   PreFlashTimes;
    UINT16                   PreFlashStrength;
    UINT16                   ChargeVsync;
    INT16                    ChkRows[20];
    INT16                    ChkCols[20];
    INT32                    PreFlashBaseCalibRation;
    LUT_CONTROL_s            ZoomVector;
    FLASH_PREFLASH_INFO_s    PreFlashInfo[5];
    INT32                    PreFlashBase;
    UINT16                   Reserved0;
    UINT16                   Reserved1;
    UINT16                   Reserved2;
    INT16                    Reserved3[20];

} FLASH_AE_AWB_INFO_s;

typedef struct  {
    UINT16    PrimeTableNum;
    INT16     *Prime;
    UINT16    SeconStartIdx;
    UINT16    SeconTableNum;
    INT16     *Secon;
} AE_SENSOR_COMPEN_s;

/**
 * AF Control settings
 */
typedef struct  {
    UINT16    FocusRange;
    UINT16    Area;
    UINT32    Param;
    UINT16    AfTimeOut;
} AF_CONTROL_SETTING_s;

typedef struct  {
    UINT32                  AfCapEnable;
    AF_CONTROL_SETTING_s    VideoAfSetting;
    AF_CONTROL_SETTING_s    StillAfSetting;
    AF_CONTROL_SETTING_s    TouchAfSetting;
    AF_CONTROL_SETTING_s    ReserveSetting0;
    AF_CONTROL_SETTING_s    ReserveSetting1;
} AF_CONTROL_s;

/**
 * EIS paraMeters setting
 */
typedef struct  {
    UINT8    VideoEisSamplingRate;
    UINT8    StillEisSamplingRate;
    UINT8    MaxEisRangePerct;
    UINT8    MinEisRangePerct;
    UINT8    FlgEisDzEffect;
    UINT8    XEisStrength;
    UINT8    YEisStrength;
    UINT8    XRscStrength;
    UINT8    YRscStrength;
    UINT8    MinSupportFr;
} EIS_CONTROL_s;

typedef struct  {
    UINT8              AutoKneeEnable;
    UINT16             AutoKneeStartLvl;
    UINT16             AutoKneeEndLvl;
    UINT16             AutoKneeMinNo;
    UINT16             AutoKneeMaxNo;

    UINT8              GammaRatioEnable;
    UINT8              LExpoRatioEnable;
    UINT16             GammaStartLvl;
    UINT16             GammaEndLvl;
    UINT16             GammaMinNo;
    UINT16             GammaMaxNo;

} SCENE_LIGHT_CONDITION_s;//scene_light_condition_t;

typedef struct  {
    UINT8              Face;
    UINT16             LvNoMin;
    UINT16             LvNoMax;
    UINT16             DistanceMin;
    UINT16             DistanceMax;
    UINT16             MotionMin;
    UINT16             MotionMax;
    UINT16             LightLow;
    UINT16             LowThreshold;
    UINT16             LightHigh;
    UINT16             HighThreshold;
} SCENE_DETECT_CONDITION_s;//scene_detect_condition_t;

typedef struct  {
    SCENE_LIGHT_CONDITION_s         BackLight;
    SCENE_LIGHT_CONDITION_s         DLight;
    INT32                             DetectPriority[36];
    UINT8                           SceneDetectNo;
    UINT8                           SkipFrames;
    SCENE_DETECT_CONDITION_s        DetectCondition[36];
    UINT8                           SportsIsThreshold;
    UINT8                           TripodIsThreshold;
} SCENE_CONTROL_s;//scene_control_t;

typedef struct  {
    UINT8     Enable;
    UINT16    StartLvl;
    UINT16    EndLvl;
    UINT16    HistoMinNo;
    UINT16    HistoMidMinNo;
    UINT16    HistoMidMaxNo;
    UINT16    HistoMaxNo;
} HISTO_INFO_s;

typedef struct  {
    HISTO_INFO_s    AutoKnee;
    HISTO_INFO_s    Gamma;
    HISTO_INFO_s    BstRatio;    //boost ratio

    HISTO_INFO_s    CbCr;
    HISTO_INFO_s    LCon00;
    HISTO_INFO_s    LCon01;
} SC_LIGHT_CONDITION_s;

typedef struct  {
    SC_LIGHT_CONDITION_s     LightCondition[5];
    INT32                    DetectPriority[36];
    UINT8                    SceneDetectNo;
    UINT8                    SkipFrames;
    SCENE_DETECT_CONDITION_s DetectCondition[36];
} SCENEMODE_CONTROL_s;

typedef struct  {
    UINT8        ColorTable;
    UINT8        DigitalEffect;
} SCENE_DEF_s;//scene_def_t;

typedef struct  {
    UINT8       AvTvMode;
    UINT32      DefExp[4];
    INT32       SlowShutter;
    UINT8       Fps60SlowshutterFps;    //60fps,50fps, 30fps,25fps, 15fps,12fps, 7fps,6fps
    UINT8       Fps30SlowshutterFps;    //60fps,50fps, 30fps,25fps, 15fps,12fps, 7fps,6fps
    UINT8       Flash;
} SCENE_VIDEO_s;//scene_video_t;

typedef struct  {
    UINT32        Value[16];
} AE_LUT_s;//ae_lut_t;

/* EXPO_LUT_CHK */
#define AE_EXPO_CHK_ISO            0
#define AE_EXPO_CHK_SHUTTER    1
typedef struct  {
    UINT32           MinIsoValue;
    UINT32           MaxIsoValue;
    UINT32           MaxIsoValueHiso;

    LUT_CONTROL_s    ZoomVector;
    UINT8            TableCount;
    UINT8            ExpoLutChk;
    AE_LUT_s         ExpoLut[24];
    LUT_CONTROL_s    MotionIsoRatio; //unit:128
} AE_EXPO_CONTROL_s;//ae_expo_control_t;

typedef struct  {
    AE_EXPO_CONTROL_s    ExpoControl;
    UINT8                NightShot;
    UINT8                Is;
    UINT8                Flash;
    UINT8                FlashType;
} SCENE_STILL_s;//scene_still_t;

typedef struct  {
    SCENE_VIDEO_s    Video;
    SCENE_VIDEO_s    Photo;
    SCENE_STILL_s    Still;
} SCENE_AE_s;//scene_ae_t;

typedef struct  {
    UINT8    MenuMode;
    UINT8    MenuModeType;
} SCENE_AWB_s;//scene_awb_t;

#define LIGHT_CONDITION_OFF     255
#define HDR_LIGHT_CONDITION_OFF     0
typedef struct  {
    UINT16       LightCondition;
    ADJ_LUT_s    VideoTable[32];
    ADJ_LUT_s    StillTable[32];
} SCENE_ADJ_s;//scene_adj_t;

typedef struct  {
    UINT32         VersionNumber;
    UINT32         ParamVersionNum;
    UINT16         SceneMode;
    SCENE_DEF_s    Def;
    SCENE_AE_s     AeControl;
    SCENE_AWB_s    AwbControl;
    SCENE_ADJ_s    AdjControl;
    DOUBLE         HdrMaxBlendRatio;
    SCENE_ADJ_s    HdrControl;
} SCENE_DATA_s;//scene_data_t;
#define SCENE_VER (0x18072500U)

typedef struct  {
    UINT16              SceneModeIdx[48];
    UINT16              MaxTableCount;
    UINT16              VideoSceneMode[8];
    UINT16              StillSceneMode[8];
    UINT16              VideoLvLut[21];
    UINT16              StillLvLut[21];
    SCENE_CONTROL_s     SceneControl;
    SCENEMODE_CONTROL_s SceneModeControl;
    //SCENE_DATA_s      data[36];
} SCENEMODE_s;


typedef struct  {
    UINT8    VinNum;
    INT32      Mode;
    INT32      *SceneMode;
} MULTI_SCENE_MODE_s;

typedef struct  {
    UINT32                 StructVersionNum;
    UINT32                 ParamVersionNum;
    AMBA_AAA_OP_INFO_s     AaaFunc;
    AE_CONTROL_s           AeControlMode;
    AE_EV_LUT_s            AeEvLut;
    AE_ALGO_INFO_s         AeAlgoInfo;
    AWB_CONTROL_s          AwbControlMode;
    AWB_ALGO_INFO_s        AwbAlgoInfo;
    FLASH_AE_AWB_INFO_s    FlashAeAwbInfo;
    AE_SENSOR_COMPEN_s     SensorCompenInfo;
    AF_CONTROL_s           AfControlMode;
    EIS_CONTROL_s          EisControlMode;
    SCENEMODE_s            SceneMode;
} AAA_PARAM_s;
#define AAA_PARAM_VER (0x17110900U)


/*
 * Flash function definition
 */
#define FLASH_OV2312     1
#define FLASH_AR0239     2


/*
 * Digital effect definition
 */
#define DIGITAL_NO_EFFECT     0
#define DIGITAL_ART           1
#define DIGITAL_SEPIA         2
#define DIGITAL_NEGATIVE      3
#define DIGITAL_BW            4
#define DIGITAL_VIVID         5
#define DIGITAL_70FILM        6
#define DIGITAL_PUNK          7
#define DIGITAL_POPART        8


#define DIGITAL_WEDDING       9
#define DIGITAL_PINHOLE       10
#define DIGITAL_MINIATURE     11
#define DIGITAL_DRAWING       12
#define DIGITAL_WATERCOLOR    13

#define DIGITAL_CUSTOMER_0    18
#define DIGITAL_CUSTOMER_1    19
#define DIGITAL_CUSTOMER_2    20
#define DIGITAL_CUSTOMER_3    21
#define DIGITAL_CUSTOMER_4    22
#define DIGITAL_CUSTOMER_5    23

#define DIGITAL_LAST          24

/**
 * Digital_Effect control settings
 */

typedef struct {
    UINT32 AeRev;
    UINT32 AwbRev;
    UINT32 AdjRev;
} AMBA_IP_VER;

typedef  struct  {
    INT16 MatrixValues[9];
} RGB_TO_RGB_INFO_s;

typedef struct  {
    UINT16              DeMode;
    UINT16              CfaStr;
    UINT16              SharpStr;
    UINT16              LumaSmoothStr;

    UINT8               CcChangeEnable;
    UINT16              Cc3dNo;
    RGB_TO_RGB_INFO_s   CcMatrix;
    INT16               Rgb2Yuv[9];
    INT16               YuvOffset[3];
    UINT16              ToneCurveNo;
    UINT16              AeTargetRatio;
    UINT16              VignetteNo;
    UINT16              WarpNo;
} DE_SETTING_s;

typedef struct  {
    DE_SETTING_s    VideoMode;
    DE_SETTING_s    StillMode;
} DE_INFO_s;

typedef struct  {
     UINT16    Red[256];
     UINT16    Green[256];
     UINT16    Blue[256];
} TONE_CURVE_s;

typedef struct  {
    UINT32        VersionNum;
    UINT32        ParamVersionNum;
    DE_INFO_s     DeInfo[24];

    TONE_CURVE_s  ToneCurve[6];
    UINT16        Vignette[6][1089];
} DIGITAL_EFFECT_PARAN_s;

typedef struct  {
    UINT32        VersionNum;
    UINT32        ParamVersionNum;
    DE_SETTING_s     DeInfo[24];

    TONE_CURVE_s  ToneCurve[6];
    UINT16        Vignette[6][1089];
} DE_PARAM_s;
#define DIGITAL_EFFECT_PARAM_VER (0x10112400)


typedef struct  {
    UINT16 MainW;        /**< main/encode width */
    UINT16 MainH;        /**< main/encode height */
    UINT32 FrameRateInt;     /**< Integer framerate. 60 for 59.94, 30 for 29.97 */
    UINT8  OverSamplingEnable;    /**< oversaming mode is enabled or not */
    UINT8  BinningHNum;    /**< binning H numerator */
    UINT8  BinningHDen;    /**< binning H denominator */
    UINT8  BinningVNum;    /**< binning V numerator */
    UINT8  BinningVDen;    /**< binning V denominator */
    FLOAT  MinMainShtTime[MAX_SUBCHANNEL_NUM];     /**< Shutter time in sec unit */
    FLOAT  MaxMainShtTime[MAX_SUBCHANNEL_NUM];     /**< Shutter time in sec unit */
} LIVEVIEW_INFO_s;



typedef struct {
    INT32 BlackR;
    INT32 BlackGr;
    INT32 BlackGb;
    INT32 BlackB;
} AMBA_BLACK_CORRECTION_s;

typedef struct  {
//    UINT8    Enable;
    UINT8    Mode;    //0:Video, 1:Still
    UINT8    Type;    //0:Normal Mode, 1:HDR mode
    UINT8    ExpNum;  //0:Long exposure, 1:Short exposue, 2:Very short exposure
} AMBA_OB_MODE_INFO_s;



typedef  struct {
    UINT32 R : 10;
    UINT32 G : 10;
    UINT32 B : 10;
    UINT32 RSVD : 2;
} COLOR_s;//;COLOR_s;

typedef struct{
    UINT16  BitsCount;
    UINT16  TileNumCol;
    UINT16  TileNumRow;
    UINT32  LinY_Long[USER_AE_MAX_TILES];
    UINT32  LinY_Mid[USER_AE_MAX_TILES];
    UINT32  LinY_Short[USER_AE_MAX_TILES];
    UINT32  MaxThreshold;
    UINT32  MinThreshold;
} AMBA_IP_USR_AE_s;
typedef struct{
    UINT16  BinCount;
    UINT32  HisBinY[USER_HISTOGRAM_MAX_COUNT];
} AMBA_IP_USR_HISTO_s;

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


#endif


