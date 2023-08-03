/*
*  @file AmbaTUNE_ImgCalibItuner_cv2x.h
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

#ifndef AMBA_IMG_CALIB_ITUNER_CV2X_H
#define AMBA_IMG_CALIB_ITUNER_CV2X_H
#include "AmbaDSP.h"
#include "AmbaTUNE_SystemApi_cv2x.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaTUNE_HdlrManager_cv2x.h"
#include "AmbaDSP_ImageHisoFilter.h"

#define ITUNER_VER_MAJOR (1)
#define ITUNER_VER_MINOR (0)
#define MAX_PARAMS (16)
#define APP_MAX_FN_SIZE (128U)
#define APP_MAX_DIR_SIZE (64)
#define APP_MAX_TITLE_SIZE (20)
#define ITUNER_MAX_FPN_MAP_SIZE (6000*4000/8)
#define ITUNER_VIGNETTE_GAIN_MAP_MULT_SIZE (4*4*128*4) // pointer to [4 bayer chan][4 direction][128 vertices]

UINT32 AmbaItuner_CalFpnMapSize(UINT32 width, UINT32 height);
UINT32 AmbaItuner_CalInternalFpnMapSize(UINT32 width, UINT32 height);
UINT32 AmbaItuner_CalWarpTableSize(UINT32 width, UINT32 height);
UINT32 AmbaItuner_CalCaTableSize(UINT32 width, UINT32 height);


#define MAX_IDSP_CONFIG_VIDEO_CHANNEL_NUM   (5)
#define MAX_IDSP_CONFIG_VIDEO_SLICE_NUM     (2)
#define MAX_IDSP_CONFIG_STILL_SLICE_NUM     (4)
/*!@brief Specified the tuning mode*/
typedef enum {
    IMG_MODE_VIDEO = 0, //!< Video Tuning
    IMG_MODE_PREVIEW, //!< Preview Tuning, This tuning mode will skip ituner check
    IMG_MODE_HIGH_ISO_STILL,
    IMG_MODE_NUMBER
} TUNING_MODE_e;


typedef struct {
    UINT8 AeOp;
    UINT8 AwbOp;
    UINT8 AfOp;
    UINT8 AdjOp;
    UINT8 Reserved1;
    UINT8 Reserved2;
    UINT8 Reserved3;
    UINT8 Reserved4;
} AAA_FUNC_s;

typedef struct {
    UINT16 EvIndex;
    UINT16 NfIndex;
    INT16 ShutterIndex;
    INT16 AgcIndex;
    INT16 IrisIndex;
    INT32 Dgain;
    UINT16 IsoValue;
    UINT16 Flash;
    UINT16 Mode;
    INT32 ShutterTime;
    INT32 AgcGain;
    UINT16 Target;
    UINT16 LumaStat;
    INT16 LimitStatus;
    UINT32 Multiplier;
} ITUNER_AE_INFO_s;

typedef struct {
    UINT16 LumaIdx;
    UINT16 OutDoorIdx;
    UINT16 HighLightIdx;
    UINT16 LowLightIdx;
    UINT32 AwbRatio[2];
    UINT32 Reserved;
    UINT32 Reserved1;
    UINT32 Reserved2;
    UINT32 Reserved3;
} ITUNER_WB_SIM_INFO_s;

typedef struct {
    UINT8 Enable;
    UINT32 CalibVersion;
    UINT32 HorizontalGridNumber;
    UINT32 VerticalGridNumber;
    UINT32 TileWidthExponent;
    UINT32 TileHeightExponent;
    UINT32 VinSensorStartX;
    UINT32 VinSensorStartY;
    UINT32 VinSensorWidth;
    UINT32 VinSensorHeight;
    UINT32 VinSensorHSubSampleFactorNum;
    UINT32 VinSensorHSubSampleFactorDen;
    UINT32 VinSensorVSubSampleFactorNum;
    UINT32 VinSensorVSubSampleFactorDen;
    UINT32 Enb2StageCompensation; /*1:enable, 0 disable*/
    char WarpGridTablePath[128];
} ITUNER_WARP_s;

typedef struct {
    /* warp part */
    UINT32 Enable;
    UINT8 GridArrayWidth;
    UINT8 GridArrayHeight;
    UINT8 HorzGridSpacingExponent;
    UINT8 VertGridSpacingExponent;
    UINT8 VertWarpGridArrayWidth;
    UINT8 VertWarpGridArrayHeight;
    UINT8 VertWarpHorzGridSpacingExponent;
    UINT8 VertWarpVertGridSpacingExponent;
    char WarpHorizontalTablePath[128];
    char WarpVerticalTablePath[128];
} ITUNER_WARP_DZOOM_INTERNAL_s;

typedef struct {
    UINT32 Enable;
    AMBA_IK_VIGNETTE_s VignetteInfo;
    char VigGainTblPath[128];
} ITUNER_VIGNETTE_s;

typedef struct {
    UINT8 Enable;
    UINT32 CalibVersion;
    UINT32 CalibVinStartX;
    UINT32 CalibVinStartY;
    UINT32 CalibVinWidth;
    UINT32 CalibVinHeight;
    UINT32 CalibVinHSubSampleFactorNum;
    UINT32 CalibVinHSubSampleFactorDen;
    UINT32 CalibVinVSubSampleFactorNum;
    UINT32 CalibVinVSubSampleFactorDen;
    char MapPath[128];
} ITUNER_FPN_s;

typedef struct {
    UINT8 Enable;
    UINT16 PixelMapWidth;
    UINT16 PixelMapHeight;
    UINT16 PixelMapPitch;
    char MapPath[128];
} ITUNER_FPN_INTERNAL_s;

typedef struct {
    UINT8 SensorId;
    UINT8 BayerPattern;
    UINT8 SensorResolution;
    UINT32 ReadoutMode;
} ITUNER_SENSOR_INPUT_s;

typedef struct {
    UINT32 PreCcGainEnable;
    UINT32 PreCcGainR;
    UINT32 PreCcGainG;
    UINT32 PreCcGainB;
} ITUNER_PRE_CC_GAIN_s;

typedef struct {
    char RegPath[128];
    char ThreeDPath[128];
} ITUNER_COLOR_CORRECTION_s;

typedef struct {
    UINT8 Enable;
    UINT32 CalibVersion;
    UINT32 HorizontalGridNumber;
    UINT32 VerticalGridNumber;
    UINT32 TileWidthExponent;
    UINT32 TileHeightExponent;
    UINT32 VinSensorStartX;
    UINT32 VinSensorStartY;
    UINT32 VinSensorWidth;
    UINT32 VinSensorHeight;
    UINT32 VinSensorHSubSampleFactorNum;
    UINT32 VinSensorHSubSampleFactorDen;
    UINT32 VinSensorVSubSampleFactorNum;
    UINT32 VinSensorVSubSampleFactorDen;
    UINT32 Enb2StageCompensation;
    char RedCaGridTablePath[128];
    char BlueCaGridTablePath[128];
} ITUNER_CHROMA_ABERRATION_s;


typedef struct {
    UINT32 HorzWarpEnable;
    UINT32 VertWarpEnable;
    UINT32 HorzPassGridArrayWidth;
    UINT32 HorzPassGridArrayHeight;
    UINT32 HorzPassHorzGridSpacingExponent;
    UINT32 HorzPassVertGridSpacingExponent;
    UINT32 VertPassGridArrayWidth;
    UINT32 VertPassGridArrayHeight;
    UINT32 VertPassHorzGridSpacingExponent;
    UINT32 VertPassVertGridSpacingExponent;
    char WarpHorzTablePathRed[128];
    char WarpVertTablePathRed[128];
    char WarpHorzTablePathBlue[128];
    char WarpVertTablePathBlue[128];
} ITUNER_CHROMA_ABERRATION_INTERNAL_s;

typedef struct {
    AMBA_IK_ADV_SPAT_FLTR_s AsfInfo;
} ITUNER_ASF_INFO_s;

typedef struct {
    UINT32 GainR;
    UINT32 GainG;
    UINT32 GainB;
} ituner_secondary_wb_gain_t;

typedef struct {
    AMBA_IK_FSTSHPNS_BOTH_s BothInfo;
} ITUNER_SHARPEN_BOTH_s;

typedef struct {
    AMBA_IK_FNLSHPNS_BOTH_s BothInfo;
    AMBA_IK_FNLSHPNS_BOTH_TDT_s ThreedTable;
} ITUNER_FINAL_SHARPEN_BOTH_s;

typedef struct {
    AMBA_IK_CE_s ContrastEnhance;
    AMBA_IK_CE_INPUT_TABLE_s ContrastEnhanceInputTable;
    AMBA_IK_CE_OUT_TABLE_s ContrastEnhanceOutTable;
    char OutputTablePath[128];
} ITUNER_VIDEO_CONTRAST_ENHC_s;

typedef struct {
    AMBA_IK_STATIC_BLC_LVL_s LEStaticBlackLevel[3];
    AMBA_IK_HDR_BLEND_s HdrBlend;
    AMBA_ITN_VIDEO_HDR_RAW_INFO_s RawInfo;
} ITUNER_VIDEO_HDR_s;

typedef struct {
    INT8  sbp_highlight;
    UINT8 diag_mode;
    UINT8 video_mctf_y_10;
    UINT8 video_mctf_fast_start;
} Ituner_Internal_s;


typedef struct {
    /* System */
    UINT8 SliceNumberX;
    UINT8 SliceNumberY;
    /* Raw frame DMA */
    AMBA_DSP_WINDOW_s RawFrnDmaInfo;
    /* Cfa Scaler */
    AMBA_DSP_WINDOW_s CfaScalerOutInfo;
    /* Mctf */
    AMBA_DSP_WINDOW_s MctfOutWin;
    UINT32 MctfOutCfgAddr;
    /* IDSP section 2 */
    AMBA_DSP_WINDOW_s Sec2CfgWin;
    UINT32 Sec2CfgAddr;
    /* IDSP section 3 */
    AMBA_DSP_WINDOW_s Sec3CfgWin;
    UINT32 Sec3CfgAddr;
} IMG_SLICE_INFO_s;

typedef struct {
    UINT32 IdspCfgInfoSize;
    AMBA_DSP_WINDOW_s VCapWin;
    AMBA_DSP_WINDOW_s EncWin;
    UINT8 TotalVideoSliceNumberX;
    UINT8 TotalVideoSliceNumberY;
    UINT8 TotalStillSliceNumberX;
    UINT8 TotalStillSliceNumberY;
    IMG_SLICE_INFO_s VideoSliceInfo[MAX_IDSP_CONFIG_VIDEO_CHANNEL_NUM][MAX_IDSP_CONFIG_VIDEO_SLICE_NUM];
    IMG_SLICE_INFO_s StillSliceInfo[MAX_IDSP_CONFIG_STILL_SLICE_NUM];
    UINT32 Sec2CfgLen;
    UINT32 Sec3CfgLen;
    UINT32 MctfCfgAddr;
    UINT32 MctsCfgAddr;
    UINT32 TaCfgAddr;
} IMG_IDSP_CONFIG_INFO_s;

typedef struct {
    /* System */
    AMBA_DSP_RAW_BUF_s HdsBlendInfo;
    AMBA_DSP_RAW_BUF_s BlendInfo;
    AMBA_DSP_RAW_BUF_s PreBlendInfo;
    AMBA_DSP_RAW_BUF_s PreBlend2Info;
    UINT32 Sec9CfgLen;
    UINT32 Sec9CfgAddr;
    UINT32 Sec10CfgLen;
    UINT32 Sec10CfgAddr;
    UINT32 Sec14CfgLen;
    UINT32 Sec14CfgAddr;
    UINT32 Sec15CfgLen;
    UINT32 Sec15CfgAddr;
} IMG_IDSP_CONFIG_EXTRA_INFO_s;

/*!@brief Specified Calib Table*/
typedef struct {
    UINT8 FPNMap[ITUNER_MAX_FPN_MAP_SIZE]; //!< FPN Map Need 4 Bytes Alignment
    UINT8 WarpGrid[4*256*192];
    UINT8 WarpHor[2*128*96];
    UINT8 WarpVer[2*128*96];
    UINT32 CcReg[IK_CC_REG_SIZE/4U];
    UINT32 Cc3d[IK_CC_3D_SIZE/4U];
    UINT8 CawarpRedGrid[4*64*96];
    UINT8 CawarpBlueGrid[4*64*96];
    UINT32 *pVignetteMap;
} ITUNER_Calib_Table_s;

typedef struct {
    UINT32 CompandEnable;
    UINT32 CompandTable[IK_NUM_COMPAND_TABLE];
} ITUNER_FE_TONE_CURVE_COMPAND_s;

typedef struct {
    ITUNER_FE_TONE_CURVE_COMPAND_s Compand;
    AMBA_IK_FE_TONE_CURVE_s Decompand;
} ITUNER_FE_TONE_CURVE_s;



/*!@brief Specified Ituner Filter Type*/
/*Normal*/
#define ITUNER_SYSTEM_INFO  0U
#define ITUNER_INTERNAL_INFO  1U
#define ITUNER_AAA_FUNCTION  2U
#define ITUNER_AE_INFO  3U
#define ITUNER_WB_SIM_INFO  4U
#define ITUNER_STATIC_BAD_PIXEL_CORRECTION  5U
#define ITUNER_STATIC_BAD_PIXEL_INTERNAL  6U
#define ITUNER_VIGNETTE_COMPENSATION  7U
#define ITUNER_VIGNETTE_COMPENSATION_INTERNAL  8U
#define ITUNER_CHROMA_ABERRATION_INFO  9U
#define ITUNER_CHROMA_ABERRATION_INFO_INTERNAL 10U
#define ITUNER_WARP_COMPENSATION  11U
#define ITUNER_WARP_COMPENSATION_DZOOM_INTERNAL  12U
#define ITUNER_DZOOM  13U
#define ITUNER_DEFERRED_BLACK_LEVEL  14U
#define ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION  15U
#define ITUNER_CFA_LEAKAGE_FILTER 16U
#define ITUNER_CFA_NOISE_FILTER  17U
#define ITUNER_ANTI_ALIASING_STRENGTH  18U
#define ITUNER_BEFORE_CE_WB_GAIN  19U
#define ITUNER_AFTER_CE_WB_GAIN  20U
#define ITUNER_PRE_CC_GAIN  21U
#define ITUNER_COLOR_CORRECTION  22U
#define ITUNER_TONE_CURVE 23U
#define ITUNER_FRONT_END_TONE_CURVE 24U
#define ITUNER_RGB_TO_YUV_MATRIX 25U
#define ITUNER_RGB_IR 26U
#define ITUNER_CHROMA_SCALE  27U
#define ITUNER_CHROMA_MEDIAN_FILTER  28U
#define ITUNER_DEMOSAIC_FILTER  29U
#define ITUNER_GB_GR_MISMATCH  30U
#define ITUNER_VIDEO_MCTF  31U
#define ITUNER_VIDEO_MCTF_LEVEL  32U
#define ITUNER_VIDEO_MCTF_TEMPORAL_ADJUST  33U
#define ITUNER_VIDEO_MCTF_Y_TEMPORAL_ADJUST  34U
#define ITUNER_VIDEO_MCTF_CB_TEMPORAL_ADJUST  35U
#define ITUNER_VIDEO_MCTF_CR_TEMPORAL_ADJUST  36U
#define ITUNER_VIDEO_MCTF_AND_FINAL_SHARPEN  37U
#define ITUNER_SENSOR_INPUT_INFO  38U
#define ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT  39U
#define ITUNER_ASF_INFO  40U
#define ITUNER_CHROMA_FILTER  41U
#define ITUNER_SHARPEN_BOTH  42U
#define ITUNER_SHARPEN_NOISE  43U
#define ITUNER_FIR  44U
#define ITUNER_CORING  45U
#define ITUNER_CORING_INDEX_SCALE  46U
#define ITUNER_MIN_CORING_RESULT  47U
#define ITUNER_MAX_CORING_RESULT  48U
#define ITUNER_SCALE_CORING  49U
#define ITUNER_FINAL_SHARPEN_BOTH  50U
#define ITUNER_FINAL_SHARPEN_NOISE  51U
#define ITUNER_FINAL_FIR  52U
#define ITUNER_FINAL_CORING  53U
#define ITUNER_FINAL_CORING_INDEX_SCALE  54U
#define ITUNER_FINAL_MIN_CORING_RESULT  55U
#define ITUNER_FINAL_MAX_CORING_RESULT  56U
#define ITUNER_FINAL_SCALE_CORING  57U
#define ITUNER_CONTRAST_ENHANCE  58U
// HDR
#define ITUNER_VHDR_BLEND  59U
#define ITUNER_FRONT_END_WB_GAIN_EXP0  60U
#define ITUNER_FRONT_END_WB_GAIN_EXP1  61U
#define ITUNER_FRONT_END_WB_GAIN_EXP2  62U
#define ITUNER_VHDR_RAW_INFO  63U
#define ITUNER_VHDR_SECONDARY_WB_GAIN  64U
#define ITUNER_STATIC_BLACK_LEVEL_EXP0  65U
#define ITUNER_STATIC_BLACK_LEVEL_EXP1  66U
#define ITUNER_STATIC_BLACK_LEVEL_EXP2  67U
#define ITUNER_DUMMY 68U
#define ITUNER_ACTIVE 69U

// FIX ME
#define ITUNER_HDS_AMP_LINEAR  70U
#define ITUNER_SENSOR_INFO  71U
#define ITUNER_VHDR_LE_STATIC_BLACK_LEVEL  72U
#define ITUNER_VHDR_ALPHA_CALC_CFG  73U
#define ITUNER_VHDR_ALPHA_CALC_THRESH  74U
#define ITUNER_VHDR_ALPHA_CALC_BLACK_LEVEL  75U
#define ITUNER_VHDR_ALPHA_CALC_ALPHA  76U
#define ITUNER_VHDR_AMP_LINEAR  77U
#define ITUNER_VHDR_AMP_LINEAR_BLEND  78U

#define ITUNER_LI_CHROMA_ASF  79U
#define ITUNER_HI_CHROMA_ASF  80U
#define ITUNER_HI_LUMA_ASF  81U
#define ITUNER_HI_CFA_NOISE_FILTER  82U
#define ITUNER_HI_CFA_LEAKAGE_FILTER  83U
#define ITUNER_HI_ANTI_ALIASING  84U
#define ITUNER_HI_AUTO_BAD_PIXEL_CORRECTION  85U
#define ITUNER_HI_CHROMA_FLTR_HIGH  86U
#define ITUNER_HI_CHROMA_FLTR_LOW  87U
#define ITUNER_HI_CHROMA_FLTR_LOW_COMBINE  88U
#define ITUNER_HI_CHROMA_FLTR_MEDIAN  89U
#define ITUNER_HI_CHROMA_FLTR_MEDIAN_COMBINE  90U
#define ITUNER_HI_CHROMA_FLTR_PRE  91U
#define ITUNER_HI_CHROMA_FLTR_VERY_LOW  92U
#define ITUNER_HI_CHROMA_FLTR_VERY_LOW_COMBINE  93U
#define ITUNER_HI_CHROMA_MEDIAN_FILTER  94U
#define ITUNRE_HI_DEMOSAIC_FILTER  95U
#define ITUNER_HI_GB_GR_MISMATCH  96U
#define ITUNER_HI_HIGH2_LUMA_ASF  97U
#define ITUNER_HI_HIGH_LUMA_ASF  98U
#define ITUNER_HI_HIGH_SHARPEN_BOTH  99U
#define ITUNER_HI_HIGH_SHARPEN_NOISE  100U
#define ITUNER_HI_HIGH_FIR  101U
#define ITUNER_HI_HIGH_CORING  102U
#define ITUNER_HI_HIGH_CORING_INDEX_SCALE  103U
#define ITUNER_HI_HIGH_MAX_CORING_RESULT  104U
#define ITUNER_HI_HIGH_MIN_CORING_RESULT  105U
#define ITUNER_HI_HIGH_SCALE_CORING  106U
#define ITUNER_HI_LOW_LUMA_ASF  107U
#define ITUNER_HI_LOW_LUMA_ASF_COMBINE  108U
#define ITUNER_HI_LUMA_NOISE_COMBINE  109U
#define ITUNER_HI_MED1_LUMA_ASF  110U
#define ITUNER_HI_MED2_LUMA_ASF  111U
#define ITUNER_HI_MED_SHARPEN_BOTH  112U
#define ITUNER_HI_MED_SHARPEN_NOISE  113U
#define ITUNER_HI_MED_FIR  114U
#define ITUNER_HI_MED_CORING  115U
#define ITUNER_HI_MED_CORING_INDEX_SCALE  116U
#define ITUNER_HI_MED_MAX_CORING_RESULT  117U
#define ITUNER_HI_MED_MIN_CORING_RESULT  118U
#define ITUNER_HI_MED_SCALE_CORING  119U
#define ITUNER_HI_SELECT  120U
#define ITUNER_HI_LUMA_BLEND  121U
#define ITUNER_HILI_COMBINE  122U
#define ITUNER_HI_MED_HIGH_FREQ_RECOVER  123U
#define ITUNER_HILI_SHARPEN_BOTH  124U
#define ITUNER_HILi_SHARPEN_NOISE  125U
#define ITUNER_HILI_FIR  126U
#define ITUNER_HILI_CORING  127U
#define ITUNER_HILI_CORING_INDEX_SCALE  128U
#define ITUNER_HILI_MAX_CORING_RESULT  129U
#define ITUNER_HILI_MIN_CORING_RESULT  130U
#define ITUNER_HILI_SCALE_CORING  131U
#define ITUNER_LI2_LUMA_ASF  132U
#define ITUNER_LI2_ANTI_ALIASING  133U
#define ITUNER_LI2_AUTO_BAD_PIXEL_CORRECTION  134U
#define ITUNER_LI2_CFA_LEAKAGE_FILTER  135U
#define ITUNER_LI2_CFA_NOISE_FILTER  136U
#define ITUNRE_LI2_DEMOSAIC_FILTER  137U
#define ITUNER_LI2_GB_GR_MISMATCH  138U
#define ITUNER_LI2_SHARPEN_BOTH  139U
#define ITUNER_LI2_SHARPEN_NOISE  140U
#define ITUNER_LI2_FIR  141U
#define ITUNER_LI2_CORING  142U
#define ITUNER_LI2_CORING_INDEX_SCALE  143U
#define ITUNER_LI2_MAX_CORING_RESULT  144U
#define ITUNER_LI2_MIN_CORING_RESULT  145U
#define ITUNER_LI2_SCALE_CORING  146U
#define ITUNER_HI_NONSMOOTH_DETECT  147U

#define ITUNER_MAX  148U


/*!@brief Specified Filter Valid Array*/
typedef UINT8 AMBA_ITUNER_VALID_FILTER_t[ITUNER_MAX];

/*!@brief Specified External File Type that Rawenc, Rawcpat or ituner need*/
typedef enum {
    FILE_EXT_FPN_MAP = 0,
    FILE_EXT_INTERNAL_FPN_MAP,
    FILE_EXT_VIGNETTE,
    FILE_EXT_INTERNAL_VIGNETTE_MULT,
    FILE_EXT_INTERNAL_VIGNETTE_SHIFT,
    FILE_EXT_WARP_TABLE,
    FILE_EXT_INTERNAL_WARP_HORIZONTAL_TABLE,
    FILE_EXT_INTERNAL_WARP_VERTICAL_TABLE,
    FILE_EXT_CA_RED_TABLE,
    FILE_EXT_CA_BLUE_TABLE,
    FILE_EXT_INTERNAL_CA_RED_HORIZONTAL_TABLE,
    FILE_EXT_INTERNAL_CA_RED_VERTICAL_TABLE,
    FILE_EXT_INTERNAL_CA_BLUE_HORIZONTAL_TABLE,
    FILE_EXT_INTERNAL_CA_BLUE_VERTICAL_TABLE,
    FILE_EXT_CC_REG,
    FILE_EXT_CC_THREED,
    FILE_EXT_ASF_THREE_D_TABLE,
    FILE_EXT_FIRST_SHARPEN,
    FILE_EXT_FINAL_SHARPEN_BOTH_THREE_D_TABLE,
    FILE_EXT_ASF,
    FILE_EXT_RAW,
    FILE_EXT_JPG,
    FILE_EXT_YUV,
    FILE_EXT_HDR_ALPHA_TABLE,
    FILE_EXT_HDR_LINEAR_TABLE,
    FILE_EXT_HDR_CONTRAST_ENHANCE_TABLE,
    FILE_EXT_MAX
} Ituner_Ext_File_Type_e;


typedef struct {
    void *Address;
    UINT32 Size;
} Load_Data_Info_s;

typedef struct {
    UINT8 Reserved;
} Save_Data_Info_s;

typedef struct {
    void *Address;
    UINT32 Max_Size;
} Load_Data_Info_Unknow_Size_s;

typedef struct {
    void *Address;
    UINT16 StartX;
    UINT16 StartY;
    UINT32 Width;
    UINT32 Height;
    UINT32 Pitch;
    char *Target_File_Path;
} Save_Data_to_Path_Info_s;

typedef struct {
    Save_Data_to_Path_Info_s Y_Info;
    Save_Data_to_Path_Info_s UV_Info;
} Save_YUV_Info_s;

typedef struct {
    void *Address;
    UINT32 Size;
} Save_Raw_Info_s;

typedef struct {
    void *Address;
    UINT32 Size;
    char *Target_File_Path;
} Save_JPG_Info_s;

typedef struct {
    UINT8 Index;
    void *Address;
    UINT32 Size;
} Load_Data_HDR_Alpha_Info_s;

typedef struct {
    UINT8 Index;
    void *Address;
    UINT32 Size;
} Load_Data_HDR_Linear_Info_s;

/*!@brief Specified the parameters that we need to use when load and save external files*/
typedef struct {
    Load_Data_Info_Unknow_Size_s Raw_Load_Param;
    Save_Raw_Info_s Raw_Save_Param;
    Save_Data_Info_s FPN_MAP_Ext_Save_Param;
    Load_Data_Info_s FPN_MAP_Ext_Load_Param;
    Save_Data_Info_s Internal_FPN_MAP_Ext_Save_Param;
    Load_Data_Info_s Internal_FPN_MAP_Ext_Load_Param;
    Save_Data_Info_s Vignette_Gain_Save_Param;
    Load_Data_Info_s Vignette_Gain_Load_Param;
    Save_Data_Info_s Internal_Vignette_Gain_Save_Param;
    Load_Data_Info_s Internal_Vignette_Gain_Load_Param;
    Save_Data_Info_s Warp_Table_Save_Param;
    Load_Data_Info_s Warp_Table_Load_Param;
    Load_Data_Info_Unknow_Size_s Internal_Warp_Horizontal_Table_Load_Param;
    Load_Data_Info_Unknow_Size_s Internal_Warp_Vertical_Table_Load_Param;
    Save_Data_Info_s Ca_Table_Save_Param;
    Load_Data_Info_s Ca_Table_Load_Param;
    Load_Data_Info_Unknow_Size_s Internal_Ca_Horizontal_Table_Load_Param;
    Load_Data_Info_Unknow_Size_s Internal_Ca_Vertical_Table_Load_Param;
    Save_Data_Info_s CC_Reg_Save_Param;
    Load_Data_Info_s CC_Reg_Load_Param;
    Save_Data_Info_s CC_ThreeD_Save_Param;
    Load_Data_Info_s CC_ThreeD_Load_Param;

    Save_Data_Info_s FIRST_SHARPEN_Save_Param;
    Load_Data_Info_s FIRST_SHARPEN_Load_Param;
    Save_Data_Info_s FirstShpBothThreedTableSaveParam;
    Load_Data_Info_s FirstShpBothThreedTableLoadParam;
    Save_Data_Info_s AsfThreedTableSaveParam;
    Load_Data_Info_s AsfThreedTableLoadParam;
    Save_Data_Info_s FinalShpBothThreedTableSaveParam;
    Load_Data_Info_s FinalShpBothThreedTableLoadParam;
    Save_Data_Info_s FINAL_SHARPEN_Save_Param;
    Load_Data_Info_s FINAL_SHARPEN_Load_Param;
    Save_Data_Info_s ASF_Save_Param;
    Load_Data_Info_s ASF_Load_Param;

    Save_YUV_Info_s YUV_Save_Param;
    Save_JPG_Info_s JPG_Save_Param;
    Load_Data_HDR_Alpha_Info_s HDR_AlphaTable_Load_Param;
    Load_Data_HDR_Linear_Info_s HDR_Linear_Load_Param;
    Load_Data_Info_s HDR_Contrast_Enhance_Load_Param;
} Ituner_Ext_File_Param_s;

/*!@brief Specified the call back function, we need to use when ituner load and ituner save*/
typedef struct {
    INT32 (*Save_Data)(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s *Ext_File_Param); /**!<Callback Function, use to save calib data, retun 0 means success*/
    INT32 (*Load_Data)(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s *Ext_File_Param); /**!<Callback Function, use to load calib data, retun 0 means success*/
} AmbaItuner_Hook_Func_t;

typedef struct {
    AmbaItuner_Hook_Func_t Hook_Func; //!< Specified the method of read / write calib data, detail please reference AmbaItuner_Hook_Func_t
    void *pWorkingBuffer;
} AmbaItuner_Config_t;

typedef struct {
    UINT32 Reserved;
} AMBA_ITUNER_PROC_INFO_s;

typedef struct {
    void *pItunerBuffer;
} Ituner_Initial_Config_t;


/**
 * @brief AmbaItuner Initial
 * If you want to use any AmbaItuner_xxx(), please call this function first.
 *
 * @param[in] Config: Setup call back function and other param, that tuning need.\n
 *                                         Detail please reference AmbaItuner_Confit_t
 *
 * @return 0 (Success) / -1 (Failure)
 */
INT32 AmbaItuner_Init(const AmbaItuner_Config_t *Config);

INT32 AmbaItuner_FrontEndToneRefresh(const AMBA_IK_MODE_CFG_s *pMode);

INT32 AmbaItuner_Refresh(const AMBA_IK_MODE_CFG_s *pMode);

INT32 AmbaItuner_FrontEndToneExecute(void);

INT32 AmbaItuner_Execute(const AMBA_IK_MODE_CFG_s *pMode);

/**
 * @brief Set SystemInfo of GData
 * This function will replace SystemInfo of GData, and set ITUNER_SYSTEM_INFO filter status is valid.\n
 * If you want to update a part of SystemInfo, you should call AmbaItuner_Get_SystemInfo() to get current SystemInfo before the replace process.
 *
 * @param[in] System: System Info Config. Detail please reference AMBA_ITN_SYSTEM_s.
 *
 * @return None
 */
void AmbaItuner_Set_SystemInfo(const AMBA_ITN_SYSTEM_s *System);

/**
 * @brief Get SystemInfo of GData
 * This function will copy SystemInfo of GData.
 *
 * @param[out] System: System Info Config. Detail please reference AMBA_ITN_SYSTEM_s.
 *
 * @return None
 */
void AmbaItuner_Get_SystemInfo(AMBA_ITN_SYSTEM_s *System);

/**
 * @brief Set InternalInfo of GData
 * This function will replace InternalInfo of GData, and set ITUNER_INTERNAL_INFO filter status is valid.\n
 * If you want to update a part of InternalInfo, you should call AmbaItuner_Get_InternalInfo() to get current InternalInfo before the replace process.
 *
 * @param[in] Internal: Internal Info Config. Detail please reference ITUNER_Internal_s.
 *
 * @return None
 */
void AmbaItuner_Set_InternalInfo(const Ituner_Internal_s *Internal);

/**
 * @brief Get InternalInfo of GData
 * This function will copy InternalInfo of GData.
 *
 * @param[out] Internal: Internal Info Config. Detail please reference Ituner_Internal_s.
 *
 * @return None
 */
void AmbaItuner_Get_InternalInfo(Ituner_Internal_s *Internal);

/**
 * @brief Set AaaFunctionInfo of GData
 * This function will replace AaaFunctionInfo of GData, and set ITUNER_AAA_FUNCTION filter status is valid.\n
 * If you want to update a part of AaaFunctionInfo, you should call AmbaItuner_Get_AAAFunction() to get current AaaFunctionInfo before the replace process.
 *
 * @param[in] AaaFunction: AaaFunction Info Config. Detail please reference AAA_FUNC_s.
 *
 * @return None
 */
void AmbaItuner_Set_AAAFunction(const AAA_FUNC_s *AaaFunction);

/**
 * @brief Get AaaFunctionInfo of GData
 * This function will copy AaaFunctionInfo of GData.
 *
 * @param[out] AaaFunction: AaaFunction Info Config. Detail please reference AAA_FUNC_s.
 *
 * @return None
 */
void AmbaItuner_Get_AAAFunction(AAA_FUNC_s *AaaFunction);

/**
 * @brief Set AeInfo of GData
 * This function will replace AeInfo of GData, and set ITUNER_AE_INFO is valid.\n
 * If you want to update a part of AeInfo, you should call AmbaItuner_Get_AeInfo() to get current AeInfo before the replace process.
 *
 * @param[in] AeInfo: AeInfo Config. Detail please reference ITUNER_AE_INFO_s.
 *
 * @return None
 */
void AmbaItuner_Set_AeInfo(const ITUNER_AE_INFO_s *AeInfo);

/**
 * @brief Get AeInfo of GData
 * This function will copy AeInfo of GData.
 *
 * @param[out] AeInfo: AeInfo Config. Detail please reference ITUNER_AE_INFO_s.
 *
 * @return None
 */
void AmbaItuner_Get_AeInfo(ITUNER_AE_INFO_s *AeInfo);

/**
 * @brief Set WbSimInfo of GData
 * This function will replace WbSimInfo of GData, and set ITUNER_WB_SIM_INFO is valid.\n
 * If you want to update a part of WbSimInfo, you should call AmbaItuner_Get_WbSimInfo() to get current AeInfo before the replace process.
 *
 * @param[in] WbSimInfo: WbSimInfo Config. Detail please reference ITUNER_WB_SIM_INFO_s.
 *
 * @return None
 */
void AmbaItuner_Set_WbSimInfo(const ITUNER_WB_SIM_INFO_s *WbSimInfo);

/**
 * @brief Get WbSimInfo of GData
 * This function will copy WbSimInfo of GData.
 *
 * @param[out] WbSimInfo: WbSimInfo Config. Detail please reference ITUNER_WB_SIM_INFO_s.
 *
 * @return None
 */
void AmbaItuner_Get_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo);




/**
 * @brief Set SensorInputInfo of GData
 * This function will replace SensorInputInfo of GData, and set ITUNER_SENSOR_INPUT_INFO is valid.\n
 * If you want to update a part of SensorInputInfo, you should call AmbaItuner_Get_SensorInputInfo() to get current SensorInputInfo before the replace process.
 *
 * @param[in] SensorInputInfo: SensorInputInfo Config. Detail please reference ITUNER_SENSOR_INPUT_s.
 *
 * @return None
 */
void AmbaItuner_Set_SensorInputInfo(const ITUNER_SENSOR_INPUT_s *SensorInputInfo);

/**
 * @brief Get SensorInputInfo of GData
 * This function will copy SensorInputInfo of GData.
 *
 * @param[out] SensorInputInfo: SensorInputInfo Config. Detail please reference ITUNER_SENSOR_INPUT_s.
 *
 * @return None
 */
void AmbaItuner_Get_SensorInputInfo(ITUNER_SENSOR_INPUT_s *SensorInputInfo);
#if 0 //The following function should be internal.
typedef struct {
    UINT32 Enable;
} AMBA_IK_DEFERRED_BLC_s;
/**
 * @brief Set DeferredBlackLevel of GData
 * This function will replace DeferredBlackLevel of GData, and set ITUNER_DEFERRED_BLACK_LEVEL is valid.\n
 * If you want to update a part of DeferredBlackLevel, you should call AmbaItuner_Get_DeferredBlackLevel() to get current DeferredBlackLevel before the replace process.
 *
 * @param[in] DeferredBlackLevel: DeferredBlackLevel Config. Detail please reference ik_deferred_blc_level_t.
 *
 * @return None
 */
void AmbaItuner_Set_DeferredBlackLevel(const AMBA_IK_DEFERRED_BLC_s *DeferredBlackLevel);

/**
 * @brief Get DeferredBlackLevel of GData
 * This function will copy DeferredBlackLevel of GData.
 *
 * @param[out] DeferredBlackLevel: DeferredBlackLevel Config. Detail please reference ik_deferred_blc_level_t.
 *
 * @return None
 */
void AmbaItuner_Get_DeferredBlackLevel(AMBA_IK_DEFERRED_BLC_s *DeferredBlackLevel);
#endif
/**
 * @brief Set DynamicBadPixelCorrection of GData
 * This function will replace DynamicBadPixelCorrection of GData, and set ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION is valid.\n
 * If you want to update a part of DynamicBadPixelCorrection, you should call AmbaItuner_Get_DynamicBadPixelCorrection() to get current DynamicBadPixelCorrection before the replace process.
 *
 * @param[in] DynamicBadPixelCorrection: DynamicBadPixelCorrection Config. Detail please reference ik_dynamic_bad_pixel_correction_t.
 *
 * @return None
 */
void AmbaItuner_Set_DynamicBadPixelCorrection(const AMBA_IK_DYNAMIC_BAD_PXL_COR_s *DynamicBadPixelCorrection);

/**
 * @brief Get DynamicBadPixelCorrection of GData
 * This function will copy DynamicBadPixelCorrection of GData.
 *
 * @param[out] DynamicBadPixelCorrection: DynamicBadPixelCorrection Config. Detail please reference ik_dynamic_bad_pixel_correction_t.
 *
 * @return None
 */
void AmbaItuner_Get_DynamicBadPixelCorrection(AMBA_IK_DYNAMIC_BAD_PXL_COR_s *DynamicBadPixelCorrection);

/**
 * @brief Set CfaLeakageFilter of GData
 * This function will replace CfaLeakageFilter of GData, and set ITUNER_CFA_LEAKAGE_FILTER is valid.\n
 * If you want to update a part of CfaLeakageFilter, you should call AmbaItuner_Get_CfaLeakageFilter() to get current CfaLeakageFilter before the replace process.
 *
 * @param[in] CfaLeakageFilter: CfaLeakageFilter Config. Detail please reference ik_cfa_leakage_filter_t.
 *
 * @return None
 */
void AmbaItuner_Set_CfaLeakageFilter(const AMBA_IK_CFA_LEAKAGE_FILTER_s *CfaLeakageFilter);

/**
 * @brief Get CfaLeakageFilter of GData
 * This function will copy CfaLeakageFilter of GData.
 *
 * @param[out] CfaLeakageFilter: CfaLeakageFilter Config. Detail please reference ik_cfa_leakage_filter_t.
 *
 * @return None
 */
void AmbaItuner_Get_CfaLeakageFilter(AMBA_IK_CFA_LEAKAGE_FILTER_s *CfaLeakageFilter);

/**
 * @brief Set CfaNoiseFilter of GData
 * This function will replace CfaNoiseFilter of GData, and set ITUNER_CFA_NOISE_FILTER is valid.\n
 * If you want to update a part of CfaNoiseFilter, you should call AmbaItuner_Get_CfaNoiseFilter() to get current CfaNoiseFilter before the replace process.
 *
 * @param[in] CfaNoiseFilter: CfaNoiseFilter Config. Detail please reference ik_cfa_noise_filter_t.
 *
 * @return None
 */
void AmbaItuner_Set_CfaNoiseFilter(const AMBA_IK_CFA_NOISE_FILTER_s *CfaNoiseFilter);

/**
 * @brief Get CfaNoiseFilter of GData
 * This function will copy CfaNoiseFilter of GData.
 *
 * @param[out] CfaNoiseFilter: CfaNoiseFilter Config. Detail please reference ik_cfa_noise_filter_t.
 *
 * @return None
 */
void AmbaItuner_Get_CfaNoiseFilter(AMBA_IK_CFA_NOISE_FILTER_s *CfaNoiseFilter);

/**
 * @brief Set AntiAliasingStrength of GData
 * This function will replace AntiAliasingStrength of GData, and set ITUNER_ANTI_ALIASING_STRENGTH is valid.\n
 * If you want to update a part of AntiAliasingStrength, you should call AmbaItuner_Get_AntiAliasingStrength() to get current AntiAliasingStrength before the replace process.
 *
 * @param[in] AntiAliasing: AntiAliasingStrength Config.
 *
 * @return None
 */
void AmbaItuner_Set_AntiAliasing(const AMBA_IK_ANTI_ALIASING_s *AntiAliasing);

/**
 * @brief Get AntiAliasingStrength of GData
 * This function will copy AntiAliasingStrength of GData.
 *
 * @param[out] AntiAliasing: AntiAliasingStrength Config.
 *
 * @return None
 */
void AmbaItuner_Get_AntiAliasing(AMBA_IK_ANTI_ALIASING_s *AntiAliasing);

/**
 * @brief Set BeforeCeWbGain of GData
 * This function will replace WbGain of GData, and set ITUNER_BEFORE_CE_WB_GAIN is valid.\n
 * If you want to update a part of WbGain, you should call AmbaItuner_Get_BeforeCeWbGain() to get current WbGain before the replace process.
 *
 * @param[in] BeforeCeWbGain : WbGain Config. Detail please reference ik_wb_gain_t.
 *
 * @return None
 */
void AmbaItuner_Set_BeforeCeWbGain(const AMBA_IK_WB_GAIN_s *BeforeCeWbGain);

/**
 * @brief Get BeforeCeWbGain of GData
 * This function will copy WbGain of GData.
 *
 * @param[out] BeforeCeWbGain : WbGain Config. Detail please reference ik_wb_gain_t.
 *
 * @return None
 */
void AmbaItuner_Get_BeforeCeWbGain(AMBA_IK_WB_GAIN_s *BeforeCeWbGain);

/**
 * @brief Set AfterCeWbGain of GData
 * This function will replace WbGain of GData, and set ITUNER_AFTER_CE_WB_GAIN is valid.\n
 * If you want to update a part of WbGain, you should call AmbaItuner_Get_AfterCeWbGain() to get current WbGain before the replace process.
 *
 * @param[in] : AfterCeWbGain : WbGain Config. Detail please reference ik_wb_gain_t.
 *
 * @return None
 */
void AmbaItuner_Set_AfterCeWbGain(const AMBA_IK_WB_GAIN_s *AfterCeWbGain);

/**
 * @brief Get AfterCeWbGain of GData
 * This function will copy WbGain of GData.
 *
 * @param[out] AfterCeWbGain: WbGain Config. Detail please reference ik_wb_gain_t.
 *
 * @return None
 */
void AmbaItuner_Get_AfterCeWbGain(AMBA_IK_WB_GAIN_s *AfterCeWbGain);

/**
 * @brief Set PreCcGain of GData
 * This function will replace PreCcGain of GData, and set ITUNER_PRE_CC_GAIN is valid.\n
 * If you want to update a part of PreCcGain, you should call AmbaItuner_Set_PreCcGain() to get current PreCcGain before the replace process.
 *
 * @param[in] : PreCcGain : PreCcGain Config. Detail please reference ITUNER_PRE_CC_GAIN_s.
 *
 * @return None
 */
void AmbaItuner_Set_PreCcGain(const ITUNER_PRE_CC_GAIN_s *PreCcGain);

/**
 * @brief Get PreCcGain of GData
 * This function will copy PreCcGain of GData.
 *
 * @param[out] PreCcGain: PreCcGain Config. Detail please reference ITUNER_PRE_CC_GAIN_s.
 *
 * @return None
 */
void AmbaItuner_Get_PreCcGain(ITUNER_PRE_CC_GAIN_s *PreCcGain);

/**
 * @brief Set ColorCorrection of GData
 * This function will replace ColorCorrection of GData, and set ITUNER_COLOR_CORRECTION is valid.\n
 * If you want to update a part of ColorCorrection, you should call AmbaItuner_Get_ColorCorrection() to get current ColorCorrection before the replace process.
 *
 * @param[in] : ColorCorrection : ColorCorrection Config. Detail please reference ITUNER_COLOR_CORRECTION_s.
 *
 * @return None
 */
void AmbaItuner_Set_ColorCorrection(const ITUNER_COLOR_CORRECTION_s *ColorCorrection);

/**
 * @brief Get ColorCorrection of GData
 * This function will copy ColorCorrection of GData.
 *
 * @param[out] ColorCorrection: ColorCorrection Config. Detail please reference ITUNER_COLOR_CORRECTION_s.
 *
 * @return None
 */
void AmbaItuner_Get_ColorCorrection(ITUNER_COLOR_CORRECTION_s *ColorCorrection);

/**
 * @brief Get Filter Status
 * This function will report that which filters are valid.
 *
 * @param[out] FilterStatus: Filter Status Array, the index relationship please reference ITUNER_VALID_s
 *
 * @return None
 */
void AmbaItuner_Get_FilterStatus(AMBA_ITUNER_VALID_FILTER_t *FilterStatus);

/**
 * @brief Get CcReg Table
 * This function will copy CcReg Table of GData address.(Read only)
 *
 * @param[out] CcReg: CcReg Table, It's read only, please don't edit it.
 *
 * @return None
 */
void AmbaItuner_Get_CcReg(AMBA_IK_COLOR_CORRECTION_REG_s **pCcReg);

/**
 * @brief Get CcThreeD Table
 * This function will copy CcThreeD Table of GData address.(Read only)
 *
 * @param[out] CcThreeD: CcThreeD Table, It's read only, please don't edit it.
 *
 * @return None
 */
void AmbaItuner_Get_CcThreeD(AMBA_IK_COLOR_CORRECTION_s **pCcThreeD);

/**
 * @brief Set ToneCurve of GData
 * This function will replace ToneCurve of GData, and set ITUNER_TONE_CURVE is valid.\n
 * If you want to update a part of ToneCurve, you should call AmbaItuner_Get_ToneCurve() to get current ToneCurve before the replace process.
 *
 * @param[in] ToneCurve: ToneCurve Config. Detail please reference ik_tone_curve_t.
 *
 * @return None
 */
void AmbaItuner_Set_ToneCurve(const AMBA_IK_TONE_CURVE_s *ToneCurve);

/**
 * @brief Get ToneCurve of GData
 * This function will copy ToneCurve of GData.
 *
 * @param[out] ToneCurve: ToneCurve Config. Detail please reference ik_tone_curve_t.
 *
 * @return None
 */
void AmbaItuner_Get_ToneCurve(AMBA_IK_TONE_CURVE_s *ToneCurve);

/**
 * @brief Set FeToneCurve of GData
 * This function will replace FeToneCurve of GData, and set ITUNER_TONE_CURVE is valid.\n
 * If you want to update a part of FeToneCurve, you should call AmbaItuner_Get_FeToneCurve() to get current FeToneCurve before the replace process.
 *
 * @param[in] FeToneCurve: FeToneCurve Config. Detail please reference ik_tone_curve_t.
 *
 * @return None
 */
void AmbaItuner_Set_FeToneCurve(const ITUNER_FE_TONE_CURVE_s *FeToneCurve);

/**
 * @brief Get FeToneCurve of GData
 * This function will copy FeToneCurve of GData.
 *
 * @param[out] FeToneCurve: FeToneCurve Config. Detail please reference ik_tone_curve_t.
 *
 * @return None
 */
void AmbaItuner_Get_FeToneCurve(ITUNER_FE_TONE_CURVE_s *FeToneCurve);

/**
 * @brief Set RgbToYuvMatrix of GData
 * This function will replace RgbToYuvMatrix of GData, and set ITUNER_RGB_TO_YUV_MATRIX is valid.\n
 * If you want to update a part of RgbToYuvMatrix, you should call AmbaItuner_Get_RgbToYuvMatrix() to get current RgbToYuvMatrix before the replace process.
 *
 * @param[in] RgbToYuvMatrix: RgbToYuvMatrix Config. Detail please reference ik_rgb_to_yuv_matrix_t.
 *
 * @return None
 */
void AmbaItuner_Set_RgbToYuvMatrix(const AMBA_IK_RGB_TO_YUV_MATRIX_s *RgbToYuvMatrix);

/**
 * @brief Get RgbToYuvMatrix of GData
 * This function will copy RgbToYuvMatrix of GData.
 *
 * @param[out] RgbToYuvMatrix: RgbToYuvMatrix Config. Detail please reference ik_rgb_to_yuv_matrix_t.
 *
 * @return None
 */
void AmbaItuner_Get_RgbToYuvMatrix(AMBA_IK_RGB_TO_YUV_MATRIX_s *RgbToYuvMatrix);


void AmbaItuner_Set_RgbIr(const AMBA_IK_RGB_IR_s *RgbIr);

void AmbaItuner_Get_RgbIr(AMBA_IK_RGB_IR_s *RgbIr);
/**
 * @brief Set ChromaScale of GData
 * This function will replace ChromaScale of GData, and set ITUNER_CHROMA_SCALE is valid.\n
 * If you want to update a part of ChromaScale, you should call AmbaItuner_Get_ChromaScale() to get current ChromaScale before the replace process.
 *
 * @param[in] ChromaScale: ChromaScale Config. Detail please reference ik_chroma_scale_t.
 *
 * @return None
 */
void AmbaItuner_Set_ChromaScale(const AMBA_IK_CHROMA_SCALE_s *ChromaScale);

/**
 * @brief Get ChromaScale of GData
 * This function will copy ChromaScale of GData.
 *
 * @param[out] ChromaScale: ChromaScale Config. Detail please reference ik_chroma_scale_t.
 *
 * @return None
 */
void AmbaItuner_Get_ChromaScale(AMBA_IK_CHROMA_SCALE_s *ChromaScale);

/**
 * @brief Set ChromaMedianFilter of GData
 * This function will replace ChromaMedianFilter of GData, and set ITUNER_CHROMA_MEDIAN_FILTER is valid.\n
 * If you want to update a part of ChromaMedianFilter, you should call AmbaItuner_Get_ChromaMedianFilter() to get current ChromaMedianFilter before the replace process.
 *
 * @param[in] ChromaMedianFilter: ChromaMedianFilter Config. Detail please reference ik_chroma_median_filter_t.
 *
 * @return None
 */
void AmbaItuner_Set_ChromaMedianFilter(const AMBA_IK_CHROMA_MEDIAN_FILTER_s *ChromaMedianFilter);

/**
 * @brief Get ChromaMedianFilter of GData
 * This function will copy ChromaMedianFilter of GData.
 *
 * @param[out] ChromaMedianFilter: ChromaMedianFilter Config. Detail please reference ik_chroma_median_filter_t.
 *
 * @return None
 */
void AmbaItuner_Get_ChromaMedianFilter(AMBA_IK_CHROMA_MEDIAN_FILTER_s *ChromaMedianFilter);

/**
 * @brief Set DemosaicFilter of GData
 * This function will replace DemosaicFilter of GData, and set ITUNER_DEMOSAIC_FILTER is valid.\n
 * If you want to update a part of DemosaicFilter, you should call AmbaItuner_Get_DemosaicFilter() to get current DemosaicFilter before the replace process.
 *
 * @param[in] DemosaicFilter: DemosaicFilter Config. Detail please reference ik_demosaic_t.
 *
 * @return None
 */
void AmbaItuner_Set_DemosaicFilter(const AMBA_IK_DEMOSAIC_s *DemosaicFilter);

/**
 * @brief Get DemosaicFilter of GData
 * This function will copy DemosaicFilter of GData.
 *
 * @param[out] DemosaicFilter: DemosaicFilter Config. Detail please reference ik_demosaic_t.
 *
 * @return None
 */
void AmbaItuner_Get_DemosaicFilter(AMBA_IK_DEMOSAIC_s *DemosaicFilter);

/**
 * @brief Set SharpenBoth of GData
 * This function will replace SharpenBoth of GData, and set ITUNER_SHARPEN_BOTH is valid.\n
 * If you want to update a part of SharpenBoth, you should call AmbaItuner_Get_SharpenBoth() to get current SharpenBoth before the replace process.
 *
 * @param[in] SharpenBoth: SharpenBoth Config. Detail please reference AMBA_DSP_IMG_SHARPEN_BOTH_s.
 *
 * @return None
 */
void AmbaItuner_Set_SharpenBoth(const ITUNER_SHARPEN_BOTH_s *SharpenBoth);

/**
 * @brief Get SharpenBoth of GData
 * This function will copy SharpenBoth of GData.
 *
 * @param[out] SharpenBoth: SharpenBoth Config. Detail please reference AMBA_DSP_IMG_SHARPEN_BOTH_s.
 *
 * @return None
 */
void AmbaItuner_Get_SharpenBoth(ITUNER_SHARPEN_BOTH_s *SharpenBoth);

/**
 * @brief Set FinalSharpenBoth of GData
 * This function will replace FinalSharpenBoth of GData, and set ITUNER_FINAL_SHARPEN_BOTH is valid.\n
 * If you want to update a part of FinalSharpenBoth, you should call AmbaItuner_Get_FinalSharpenBoth() to get current FinalSharpenBoth before the replace process.
 *
 * @param[in] FinalSharpenBoth: FinalSharpenBoth Config. Detail please reference AMBA_DSP_IMG_SHARPEN_BOTH_s.
 *
 * @return None
 */
void AmbaItuner_Set_FinalSharpenBoth(const ITUNER_FINAL_SHARPEN_BOTH_s *FinalSharpenBoth);

/**
 * @brief Get FinalSharpenBoth of GData
 * This function will copy FinalSharpenBoth of GData.
 *
 * @param[out] FinalSharpenBoth: FinalSharpenBoth Config. Detail please reference AMBA_DSP_IMG_SHARPEN_BOTH_s.
 *
 * @return None
 */
void AmbaItuner_Get_FinalSharpenBoth(ITUNER_FINAL_SHARPEN_BOTH_s *FinalSharpenBoth);

/**
 * @brief Set SharpenNoise of GData
 * This function will replace SharpenNoise of GData, and set ITUNER_SHARPEN_NOISE is valid.\n
 * If you want to update a part of SharpenNoise, you should call AmbaItuner_Get_SharpenNoise() to get current SharpenNoise before the replace process.
 *
 * @param[in] SharpenNoise: SharpenNoise Config. Detail please reference ik_first_sharpen_noise_t.
 *
 * @return None
 */
void AmbaItuner_Set_SharpenNoise(const AMBA_IK_FSTSHPNS_NOISE_s *SharpenNoise);

/**
 * @brief Get SharpenNoise of GData
 * This function will copy SharpenNoise of GData.
 *
 * @param[out] SharpenNoise: SharpenNoise Config. Detail please reference ik_first_sharpen_noise_t.
 *
 * @return None
 */
void AmbaItuner_Get_SharpenNoise(AMBA_IK_FSTSHPNS_NOISE_s *SharpenNoise);

/**
 * @brief Set FinalSharpenNoise of GData
 * This function will replace FinalSharpenNoise of GData, and set ITUNER_FINAL_SHARPEN_NOISE is valid.\n
 * If you want to update a part of FinalSharpenNoise, you should call AmbaItuner_Get_FinalSharpenNoise() to get current FinalSharpenNoise before the replace process.
 *
 * @param[in] FinalSharpenNoise: FinalSharpenNoise Config. Detail please reference ik_first_sharpen_noise_t.
 *
 * @return None
 */
void AmbaItuner_Set_FinalSharpenNoise(const AMBA_IK_FNLSHPNS_NOISE_s *FinalSharpenNoise);

void AmbaItuner_Get_FinalSharpenNoise(AMBA_IK_FNLSHPNS_NOISE_s *FinalSharpenNoise);

void AmbaItuner_Set_Fir(const AMBA_IK_FSTSHPNS_FIR_s *Fir);

void AmbaItuner_Get_Fir(AMBA_IK_FSTSHPNS_FIR_s *Fir);

void AmbaItuner_Set_FinalFir(const AMBA_IK_FNLSHPNS_FIR_s *FinalFir);

void AmbaItuner_Get_FinalFir(AMBA_IK_FNLSHPNS_FIR_s *FinalFir);

void AmbaItuner_Set_Coring(const AMBA_IK_FSTSHPNS_CORING_s *Coring);

void AmbaItuner_Get_Coring(AMBA_IK_FSTSHPNS_CORING_s *Coring);

void AmbaItuner_Set_FinalCoring(const AMBA_IK_FNLSHPNS_CORING_s *FinalCoring);

void AmbaItuner_Get_FinalCoring(AMBA_IK_FNLSHPNS_CORING_s *FinalCoring);

void AmbaItuner_Set_CoringIndexScale(const AMBA_IK_FSTSHPNS_COR_IDX_SCL_s *CoringIndexScale);

void AmbaItuner_Get_CoringIndexScale(AMBA_IK_FSTSHPNS_COR_IDX_SCL_s *CoringIndexScale);

void AmbaItuner_Set_MinCoringResult(const AMBA_IK_FSTSHPNS_MIN_COR_RST_s *MinCoringResult);

void AmbaItuner_Get_MinCoringResult(AMBA_IK_FSTSHPNS_MIN_COR_RST_s *MinCoringResult);

void AmbaItuner_Set_MaxCoringResult(const AMBA_IK_FSTSHPNS_MAX_COR_RST_s *MaxCoringResult);

void AmbaItuner_Get_MaxCoringResult(AMBA_IK_FSTSHPNS_MAX_COR_RST_s *MaxCoringResult);

void AmbaItuner_Set_FinalCoringIndexScale(const AMBA_IK_FNLSHPNS_COR_IDX_SCL_s *FinalCoringIndexScale);

void AmbaItuner_Get_FinalCoringIndexScale(AMBA_IK_FNLSHPNS_COR_IDX_SCL_s *FinalCoringIndexScale);

void AmbaItuner_Set_FinalMinCoringResult(const AMBA_IK_FNLSHPNS_MIN_COR_RST_s *FinalMinCoringResult);

void AmbaItuner_Get_FinalMinCoringResult(AMBA_IK_FNLSHPNS_MIN_COR_RST_s *FinalMinCoringResult);

void AmbaItuner_Set_FinalMaxCoringResult(const AMBA_IK_FNLSHPNS_MAX_COR_RST_s *FinalMaxCoringResult);

void AmbaItuner_Get_FinalMaxCoringResult(AMBA_IK_FNLSHPNS_MAX_COR_RST_s *FinalMaxCoringResult);

/**
 * @brief Get FinalScaleCoring of GData
 * This function will copy FinalScaleCoring of GData.
 *
 * @param[out] FinalScaleCoring: FinalScaleCoring Config. Detail please reference AMBA_IK_LEVEL_s.
 *
 * @return None
 */

void AmbaItuner_Set_FinalScaleCoring(const AMBA_IK_FNLSHPNS_SCL_COR_s *FinalScaleCoring);

void AmbaItuner_Get_FinalScaleCoring(AMBA_IK_FNLSHPNS_SCL_COR_s *FinalScaleCoring);

void AmbaItuner_Set_ScaleCoring(const AMBA_IK_FSTSHPNS_SCL_COR_s *ScaleCoring);

void AmbaItuner_Get_ScaleCoring(AMBA_IK_FSTSHPNS_SCL_COR_s *ScaleCoring);

/**
 * @brief Set VideoMctf of GData
 * This function will replace VideoMctf of GData, and set ITUNER_VIDEO_MCTF is valid.\n
 * If you want to update a part of VideoMctf, you should call AmbaItuner_Get_VideoMctf() to get current VideoMctf before the replace process.
 *
 * @param[in] VideoMctf: VideoMctf Config. Detail please reference ik_video_mctf_t.
 *
 * @return None
 */
void AmbaItuner_Set_VideoMctf(const AMBA_IK_VIDEO_MCTF_s *VideoMctf);

/**
 * @brief Get VideoMctf of GData
 * This function will copy VideoMctf of GData.
 *
 * @param[out] VideoMctf: VideoMctf Config. Detail please reference ik_video_mctf_t.
 *
 * @return None
 */
void AmbaItuner_Get_VideoMctf(AMBA_IK_VIDEO_MCTF_s *VideoMctf);

/**
 * @brief Set VideoMctfLevel of GData
 * This function will replace VideoMctfLevel of GData, and set ITUNER_VIDEO_MCTF_LEVEL is valid
 * If you want to update a part of VideoMctfLevel, you should call AmbaItuner_Get_VideoMctfLevel() to get current VideoMctf before the replace process.
 *
 * @param[in] VideoMctf: VideoMctfLevel Config. Detail please reference AMBA_DSP_IMG_VIDEO_MCTF_LEVEL_s.
 *
 * @return None
 */
void AmbaItuner_Set_VideoMctfLevel(const AMBA_IK_VIDEO_MCTF_s *VideoMctf);

/**
 * @brief Get VideoMctfLevel of GData
 * This function will copy VideoMctfLevel of GData.
 *
 * @param[out] VideoMctf: VideoMctfLevel Config. Detail please reference AMBA_DSP_IMG_VIDEO_MCTF_LEVEL_s.
 *
 * @return None
 */
void AmbaItuner_Get_VideoMctfLevel(AMBA_IK_VIDEO_MCTF_s *VideoMctf);

void AmbaItuner_Set_VideoMctfTemporalAdjust(const AMBA_IK_VIDEO_MCTF_TA_s *VideoMctfTemporalAdjust);

void AmbaItuner_Get_VideoMctfTemporalAdjust(AMBA_IK_VIDEO_MCTF_TA_s *VideoMctfTemporalAdjust);

void AmbaItuner_Set_VideoMctfAndFinalSharpen(const AMBA_IK_POS_DSP33_t *VideoMctfAndFinalSharpen);

void AmbaItuner_Get_VideoMctfAndFinalSharpen(AMBA_IK_POS_DSP33_t *VideoMctfAndFinalSharpen);

/**
 * @brief Set ShpAOrSpatialFilterSelect of GData
 * This function will replace ShpAOrSpatialFilterSelect of GData, and set ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT is valid.\n
 * If you want to update a part of ShpAOrSpatialFilterSelect, you should call AmbaItuner_Get_ShpAOrSpatialFilterSelect() to get current ShpAOrSpatialFilterSelect before the replace process.
 *
 * @param[in] FirstLumaProcessMode: ShpAOrSpatialFilterSelect Config.
 *
 * @return None
 */
void AmbaItuner_Set_ShpAOrSpatialFilterSelect(const AMBA_IK_FIRST_LUMA_PROC_MODE_s *FirstLumaProcessMode);

/**
 * @brief Get ShpAOrSpatialFilterSelect of GData
 * This function will copy ShpAOrSpatialFilterSelect of GData.
 *
 * @param[out] FirstLumaProcessMode: ShpAOrSpatialFilterSelect Config.
 *
 * @return None
 */
void AmbaItuner_Get_ShpAOrSpatialFilterSelect(AMBA_IK_FIRST_LUMA_PROC_MODE_s *FirstLumaProcessMode);

/**
 * @brief Set AsfInfo of GData
 * This function will replace AsfInfo of GData, and set ITUNER_ASF_INFO is valid.\n
 * If you want to update a part of AsfInfo, you should call AmbaItuner_Get_AsfInfo() to get current AsfInfo before the replace process.
 *
 * @param[in] AsfInfo: AsfInfo Config. Detail please reference AMBA_DSP_IMG_ASF_INFO_s.
 *
 * @return None
 */
void AmbaItuner_Set_AsfInfo(const ITUNER_ASF_INFO_s *AsfInfo);

/**
 * @brief Get AsfInfo of GData
 * This function will copy AsfInfo of GData.
 *
 * @param[out] AsfInfo: AsfInfo Config. Detail please reference AMBA_DSP_IMG_ASF_INFO_s.
 *
 * @return None
 */
void AmbaItuner_Get_AsfInfo(ITUNER_ASF_INFO_s *AsfInfo);

/**
 * @brief Set ChromaFilter of GData
 * This function will replace ChromaFilter of GData, and set ITUNER_CHROMA_FILTER is valid.\n
 * If you want to update a part of ChromaFilter, you should call AmbaItuner_Get_ChromaFilter() to get current ChromaFilter before the replace process.
 *
 * @param[in] ChromaFilter: ChromaFilter Config. Detail please reference ik_chroma_filter_t.
 *
 * @return None
 */
void AmbaItuner_Set_ChromaFilter(const AMBA_IK_CHROMA_FILTER_s *ChromaFilter);

/**
 * @brief Get ChromaFilter of GData
 * This function will copy ChromaFilter of GData.
 *
 * @param[out] ChromaFilter: ChromaFilter Config. Detail please reference ik_chroma_filter_t.
 *
 * @return None
 */
void AmbaItuner_Get_ChromaFilter(AMBA_IK_CHROMA_FILTER_s *ChromaFilter);

/**
 * @brief Set GbGrMismatch of GData
 * This function will replace GbGrMismatch of GData, and set ITUNER_GB_GR_MISMATCH is valid.\n
 * If you want to update a part of GbGrMismatch, you should call AmbaItuner_Get_GbGrMismatch() to get current GbGrMismatch before the replace process.
 *
 * @param[in] GbGrMismatch: GbGrMismatch Config. Detail please reference ik_grgb_mismatch_t.
 *
 * @return None
 */
void AmbaItuner_Set_GbGrMismatch(const AMBA_IK_GRGB_MISMATCH_s *GbGrMismatch);

/**
 * @brief Get GbGrMismatch of GData
 * This function will copy GbGrMismatch of GData.
 *
 * @param[out] GbGrMismatch: GbGrMismatch Config. Detail please reference ik_grgb_mismatch_t.
 *
 * @return None
 */
void AmbaItuner_Get_GbGrMismatch(AMBA_IK_GRGB_MISMATCH_s *GbGrMismatch);

/**
 * @brief Set SensorInfo of GData
 * This function will replace SensorInfo of GData, and set ITUNER_SENSOR_INFO is valid.\n
 * If you want to update a part of SensorInfo, you should call AmbaItuner_Get_SensorInfo() to get current SensorInfo before the replace process.
 *
 * @param[in] SensorInfo: SensorInfo Config. Detail please reference ik_vin_sensor_info_t.
 *
 * @return None
 */
void AmbaItuner_Set_SensorInfo(const AMBA_IK_VIN_SENSOR_INFO_s *SensorInfo);

/**
 * @brief Get SensorInfo of GData
 * This function will copy SensorInfo of GData.
 *
 * @param[out] SensorInfo: SensorInfo Config. Detail please reference ik_vin_sensor_info_t.
 *
 * @return None
 */
void AmbaItuner_Get_SensorInfo(AMBA_IK_VIN_SENSOR_INFO_s *SensorInfo);

void AmbaItuner_Set_VideoContrastEnhance(const ITUNER_VIDEO_CONTRAST_ENHC_s *ContrastEnhance);
void AmbaItuner_Get_VideoContrastEnhance(ITUNER_VIDEO_CONTRAST_ENHC_s *ContrastEnhance);

void AmbaItuner_Set_VideoHdrLEStaticBlackLevel(UINT8 Index, const AMBA_IK_STATIC_BLC_LVL_s*StaticBlackLevel);
void AmbaItuner_Get_VideoHdrLEStaticBlackLevel(UINT8 Index, AMBA_IK_STATIC_BLC_LVL_s *StaticBlackLevel);
void AmbaItuner_Set_VideoHdrBlend(UINT8 Index, const AMBA_IK_HDR_BLEND_s *HdrBlend);
void AmbaItuner_Get_VideoHdrBlend(UINT8 Index, AMBA_IK_HDR_BLEND_s *HdrBlend);
void AmbaItuner_Set_VideoHdrFrontEndWbGain(UINT8 Index, const AMBA_IK_FE_WB_GAIN_s *FrontEndWbGain);
void AmbaItuner_Get_VideoHdrFrontEndWbGain(UINT8 Index, AMBA_IK_FE_WB_GAIN_s *FrontEndWbGain);
void AmbaItuner_Set_VideoHdrRawInfo(const AMBA_ITN_VIDEO_HDR_RAW_INFO_s *RawInfo);
void AmbaItuner_Get_VideoHdrRawInfo(AMBA_ITN_VIDEO_HDR_RAW_INFO_s *RawInfo);
void AmbaItuner_Set_VideoHdrStaticBlackLevel(UINT8 Index, const AMBA_IK_STATIC_BLC_LVL_s *StaticBlackLevel);
void AmbaItuner_Get_VideoHdrStaticBlackLevel(UINT8 Index, AMBA_IK_STATIC_BLC_LVL_s *StaticBlackLevel);


void AmbaItuner_Set_StaticBadPixelCorrection(const ITUNER_FPN_s *StaticBadPixelCorrection);
void AmbaItuner_Get_StaticBadPixelCorrection(ITUNER_FPN_s *StaticBadPixelCorrection);
void AmbaItuner_Set_SBP_CorrectionInternal(const ITUNER_FPN_INTERNAL_s *StaticBadPixelCorrectionInternal);
void AmbaItuner_Get_SBP_CorrectionInternal(ITUNER_FPN_INTERNAL_s *StaticBadPixelCorrectionInternal);
void AmbaItuner_GetSbpInfoRetrive(AMBA_IK_STATIC_BAD_PXL_COR_s *SbpCorrection);
void AmbaItuner_Set_VignetteCompensation(const ITUNER_VIGNETTE_s *VignetteCompensation);
void AmbaItuner_Get_VignetteCompensation(ITUNER_VIGNETTE_s *VignetteCompensation);
void AmbaItuner_SetCawarpInfo(const ITUNER_CHROMA_ABERRATION_s *ChromaAberrationInfo);
void AmbaItuner_GetCawarpInfo(ITUNER_CHROMA_ABERRATION_s *ChromaAberrationInfo);
void AmbaItuner_GetCawarpInfoRetrive(AMBA_IK_CAWARP_INFO_s *CawarpCompensationInfo);
void AmbaItuner_SetCaWarpInternal(const ITUNER_CHROMA_ABERRATION_INTERNAL_s *ChromaAberrationInfoInternal);
void AmbaItuner_GetCaWarpInternal(ITUNER_CHROMA_ABERRATION_INTERNAL_s *ChromaAberrationInfoInternal);
void AmbaItuner_SetWarpInfo(const ITUNER_WARP_s *WarpCompensation);
void AmbaItuner_GetWarpInfo(ITUNER_WARP_s *WarpCompensation);
void AmbaItuner_GetWarpInfoRetrive(AMBA_IK_WARP_INFO_s *WarpCompensationInfo);

void AmbaItuner_Set_WarpCompensationDzoomInternal(const ITUNER_WARP_DZOOM_INTERNAL_s *WarpCompensationDzoomInternal);
void AmbaItuner_Get_WarpCompensationDzoomInternal(ITUNER_WARP_DZOOM_INTERNAL_s *WarpCompensationDzoomInternal);

void AmbaItuner_Set_Dzoom(const AMBA_IK_DZOOM_INFO_s *Dzoom);
void AmbaItuner_Get_Dzoom(AMBA_IK_DZOOM_INFO_s *Dzoom);
void AmbaItuner_Set_Dummy(const AMBA_IK_DUMMY_MARGIN_RANGE_s *Dummy);
void AmbaItuner_Get_Dummy(AMBA_IK_DUMMY_MARGIN_RANGE_s *Dummy);
void AmbaItuner_Set_Active(const AMBA_IK_VIN_ACTIVE_WINDOW_s *Active);
void AmbaItuner_Get_Active(AMBA_IK_VIN_ACTIVE_WINDOW_s *Active);
//hiso
void AmbaItuner_Set_ChromaAsf(const AMBA_IK_HI_LOW_CHROMA_ASF_s *ChromaAsf);
void AmbaItuner_Get_ChromaAsf(AMBA_IK_HI_LOW_CHROMA_ASF_s *ChromaAsf);
void AmbaItuner_Set_HiChromaAsf(const AMBA_IK_HI_CHROMA_ASF_s *HiChromaAsf);
void AmbaItuner_Get_HiChromaAsf(AMBA_IK_HI_CHROMA_ASF_s *HiChromaAsf);
void AmbaItuner_Set_HiASF(const AMBA_IK_HI_ASF_s *HiAsf);
void AmbaItuner_Get_HiASF(AMBA_IK_HI_ASF_s *HiAsf);
void AmbaItuner_Set_HiCfaNoiseFilter(const AMBA_IK_HI_CFA_NOISE_FILTER_s *HiCfaNoiseFilter);
void AmbaItuner_Get_HiCfaNoiseFilter(AMBA_IK_HI_CFA_NOISE_FILTER_s *HiCfaNoiseFilter);
void AmbaItuner_Set_HiCfaLeakageFilter(const AMBA_IK_HI_CFA_LEAKAGE_FILTER_s *HiCfaLeakageFilter);
void AmbaItuner_Get_HiCfaLeakageFilter(AMBA_IK_HI_CFA_LEAKAGE_FILTER_s *HiCfaLeakageFilter);
void AmbaItuner_Set_HiAntiAliasing(const AMBA_IK_HI_ANTI_ALIASING_s *HiAntiAliasing);
void AmbaItuner_Get_HiAntiAliasing(AMBA_IK_HI_ANTI_ALIASING_s *HiAntiAliasing);
void AmbaItuner_Set_HiDynamicBpc(const AMBA_IK_HI_DYNAMIC_BPC_s *HiDynamicBpc);
void AmbaItuner_Get_HiDynamicBpc(AMBA_IK_HI_DYNAMIC_BPC_s *HiDynamicBpc);
void AmbaItuner_Set_HiChromaHigh(const AMBA_IK_HI_CHROMA_FILTER_HIGH_s *HiChromaFilterHigh);
void AmbaItuner_Get_HiChromaHigh(AMBA_IK_HI_CHROMA_FILTER_HIGH_s *HiChromaFilterHigh);
void AmbaItuner_Set_HiChromaLow(const AMBA_IK_HI_CHROMA_FILTER_LOW_s *HiChromaFilterLow);
void AmbaItuner_Get_HiChromaLow(AMBA_IK_HI_CHROMA_FILTER_LOW_s *HiChromaFilterLow);
void AmbaItuner_Set_HiChromaLowCombine(const AMBA_IK_HI_CHROMA_FLTR_LOW_COM_s *HiChromaFilterLowCombine);
void AmbaItuner_Get_HiChromaLowCombine(AMBA_IK_HI_CHROMA_FLTR_LOW_COM_s *HiChromaFilterLowCombine);
void AmbaItuner_Set_HiChromaMed(const AMBA_IK_HI_CHROMA_FILTER_MED_s *HiChromaFilterMed);
void AmbaItuner_Get_HiChromaMed(AMBA_IK_HI_CHROMA_FILTER_MED_s *HiChromaFilterMed);
void AmbaItuner_Set_HiChromaMedCombine(const AMBA_IK_HI_CHROMA_FLTR_MED_COM_s *HiChromaFilterMedCombine);
void AmbaItuner_Get_HiChromaMedCombine(AMBA_IK_HI_CHROMA_FLTR_MED_COM_s *HiChromaFilterMedCombine);
void AmbaItuner_Set_HiChromaPre(const AMBA_IK_HI_CHROMA_FILTER_PRE_s *HiChromaFilterPre);
void AmbaItuner_Get_HiChromaPre(AMBA_IK_HI_CHROMA_FILTER_PRE_s *HiChromaFilterPre);
void AmbaItuner_Set_HiChromaVeryLow(const AMBA_IK_HI_CHROMA_FILTER_VERY_LOW_s *HiChromaFilterVeryLow);
void AmbaItuner_Get_HiChromaVeryLow(AMBA_IK_HI_CHROMA_FILTER_VERY_LOW_s *HiChromaFilterVeryLow);
void AmbaItuner_Set_HiChromaVeryLowCombine(const AMBA_IK_HI_CHROMA_FLTR_VERY_LOW_COM_s *HiChromaFilterVeryLowCombine);
void AmbaItuner_Get_HiChromaVeryLowCombine(AMBA_IK_HI_CHROMA_FLTR_VERY_LOW_COM_s *HiChromaFilterVeryLowCombine);
void AmbaItuner_Set_HiChromaMedianFilter(const AMBA_IK_HI_CHROMA_MEDIAN_FILTER_s *HiChromaMedianFilter);
void AmbaItuner_Get_HiChromaMedianFilter(AMBA_IK_HI_CHROMA_MEDIAN_FILTER_s *HiChromaMedianFilter);
void AmbaItuner_Set_HiDemosaic(const AMBA_IK_HI_DEMOSAIC_s *HiDemosaic);
void AmbaItuner_Get_HiDemosaic(AMBA_IK_HI_DEMOSAIC_s *HiDemosaic);
void AmbaItuner_Set_HiGrGbMismatch(const AMBA_IK_HI_GRGB_MISMATCH_s *HiGrGbMismatch);
void AmbaItuner_Get_HiGrGbMismatch(AMBA_IK_HI_GRGB_MISMATCH_s *HiGrGbMismatch);
void AmbaItuner_Set_HiHigh2Asf(const AMBA_IK_HI_HIGH2_ASF_s *HiHigh2Asf);
void AmbaItuner_Get_HiHigh2Asf(AMBA_IK_HI_HIGH2_ASF_s *HiHigh2Asf);
void AmbaItuner_Set_HiHighAsf(const AMBA_IK_HI_HIGH_ASF_s *HiHighAsf);
void AmbaItuner_Get_HiHighAsf(AMBA_IK_HI_HIGH_ASF_s *HiHighAsf);
void AmbaItuner_Set_HiHighShpnsBoth(const AMBA_IK_HI_HIGH_SHPNS_BOTH_s *HiHighShpnsBoth);
void AmbaItuner_Get_HiHighShpnsBoth(AMBA_IK_HI_HIGH_SHPNS_BOTH_s *HiHighShpnsBoth);
void AmbaItuner_Set_HiHighShpnsNoise(const AMBA_IK_HI_HIGH_SHPNS_NOISE_s *HiHighShpnsNoise);
void AmbaItuner_Get_HiHighShpnsNoise(AMBA_IK_HI_HIGH_SHPNS_NOISE_s *HiHighShpnsNoise);
void AmbaItuner_Set_HiHighShpnsFir(const AMBA_IK_HI_HIGH_SHPNS_FIR_s *HiHighShpnsFir);
void AmbaItuner_Get_HiHighShpnsFir(AMBA_IK_HI_HIGH_SHPNS_FIR_s *HiHighShpnsFir);
void AmbaItuner_Set_HiHighShpnsCoring(const AMBA_IK_HI_HIGH_SHPNS_CORING_s *HiHighShpnsCoring);
void AmbaItuner_Get_HiHighShpnsCoring(AMBA_IK_HI_HIGH_SHPNS_CORING_s *HiHighShpnsCoring);
void AmbaItuner_Set_HiHighShpnsCorIdxScl(const AMBA_IK_HI_HIGH_SHPNS_COR_IDX_SCL_s *HiHighShpnsCorIdxScl);
void AmbaItuner_Get_HiHighShpnsCorIdxScl(AMBA_IK_HI_HIGH_SHPNS_COR_IDX_SCL_s *HiHighShpnsCorIdxScl);
void AmbaItuner_Set_HiHighShpnsMaxCorRst(const AMBA_IK_HI_HIGH_SHPNS_MAX_COR_RST_s *HiHighShpnsMaxCorRst);
void AmbaItuner_Get_HiHighShpnsMaxCorRst(AMBA_IK_HI_HIGH_SHPNS_MAX_COR_RST_s *HiHighShpnsMaxCorRst);
void AmbaItuner_Set_HiHighShpnsMinCorRst(const AMBA_IK_HI_HIGH_SHPNS_MIN_COR_RST_s *HiHighShpnsMinCorRst);
void AmbaItuner_Get_HiHighShpnsMinCorRst(AMBA_IK_HI_HIGH_SHPNS_MIN_COR_RST_s *HiHighShpnsMinCorRst);
void AmbaItuner_Set_HiHighShpnsSclCor(const AMBA_IK_HI_HIGH_SHPNS_SCL_COR_s *HiHighShpnsSclCor);
void AmbaItuner_Get_HiHighShpnsSclCor(AMBA_IK_HI_HIGH_SHPNS_SCL_COR_s *HiHighShpnsSclCor);
void AmbaItuner_Set_HiLowAsf(const AMBA_IK_HI_LOW_ASF_s *HiLowAsf);
void AmbaItuner_Get_HiLowAsf(AMBA_IK_HI_LOW_ASF_s *HiLowAsf);
void AmbaItuner_Set_HiLowAsfCombine(const AMBA_IK_HI_LOW_ASF_COMBINE_s *HiLowAsfCombine);
void AmbaItuner_Get_HiLowAsfCombine(AMBA_IK_HI_LOW_ASF_COMBINE_s *HiLowAsfCombine);
void AmbaItuner_Set_HiLumaCombine(const AMBA_IK_HI_LUMA_COMBINE_s *HiLumaCombine);
void AmbaItuner_Get_HiLumaCombine(AMBA_IK_HI_LUMA_COMBINE_s *HiLumaCombine);
void AmbaItuner_Set_HiMed1Asf(const AMBA_IK_HI_MED1_ASF_s *HiMed1Asf);
void AmbaItuner_Get_HiMed1Asf(AMBA_IK_HI_MED1_ASF_s *HiMed1Asf);
void AmbaItuner_Set_HiMed2Asf(const AMBA_IK_HI_MED2_ASF_s *HiMed2Asf);
void AmbaItuner_Get_HiMed2Asf(AMBA_IK_HI_MED2_ASF_s *HiMed2Asf);
void AmbaItuner_Set_HiMedShpnsBoth(const AMBA_IK_HI_MED_SHPNS_BOTH_s *HiMedShpnsBoth);
void AmbaItuner_Get_HiMedShpnsBoth(AMBA_IK_HI_MED_SHPNS_BOTH_s *HiMedShpnsBoth);
void AmbaItuner_Set_HiMedShpnsNoise(const AMBA_IK_HI_MED_SHPNS_NOISE_s *HiMedShpnsNoise);
void AmbaItuner_Get_HiMedShpnsNoise(AMBA_IK_HI_MED_SHPNS_NOISE_s *HiMedShpnsNoise);
void AmbaItuner_Set_HiMedShpnsFir(const AMBA_IK_HI_MED_SHPNS_FIR_s *HiMedShpnsFir);
void AmbaItuner_Get_HiMedShpnsFir(AMBA_IK_HI_MED_SHPNS_FIR_s *HiMedShpnsFir);
void AmbaItuner_Set_HiMedShpnsCoring(const AMBA_IK_HI_MED_SHPNS_CORING_s *HiMedShpnsCoring);
void AmbaItuner_Get_HiMedShpnsCoring(AMBA_IK_HI_MED_SHPNS_CORING_s *HiMedShpnsCoring);
void AmbaItuner_Set_HiMedShpnsCorIdxScl(const AMBA_IK_HI_MED_SHPNS_COR_IDX_SCL_s *HiMedShpnsCorIdxScl);
void AmbaItuner_Get_HiMedShpnsCorIdxScl(AMBA_IK_HI_MED_SHPNS_COR_IDX_SCL_s *HiMedShpnsCorIdxScl);
void AmbaItuner_Set_HiMedShpnsMaxCorRst(const AMBA_IK_HI_MED_SHPNS_MAX_COR_RST_s *HiMedShpnsMaxCorRst);
void AmbaItuner_Get_HiMedShpnsMaxCorRst(AMBA_IK_HI_MED_SHPNS_MAX_COR_RST_s *HiMedShpnsMaxCorRst);
void AmbaItuner_Set_HiMedShpnsMinCorRst(const AMBA_IK_HI_MED_SHPNS_MIN_COR_RST_s *HiMedShpnsMinCorRst);
void AmbaItuner_Get_HiMedShpnsMinCorRst(AMBA_IK_HI_MED_SHPNS_MIN_COR_RST_s *HiMedShpnsMinCorRst);
void AmbaItuner_Set_HiMedShpnsSclCor(const AMBA_IK_HI_MED_SHPNS_SCL_COR_s *HiMedShpnsSclCor);
void AmbaItuner_Get_HiMedShpnsSclCor(AMBA_IK_HI_MED_SHPNS_SCL_COR_s *HiMedShpnsSclCor);
void AmbaItuner_Set_HiSelect(const AMBA_IK_HI_SELECT_s *HiSelect);
void AmbaItuner_Get_HiSelect(AMBA_IK_HI_SELECT_s *HiSelect);
void AmbaItuner_Set_HiLumaBlend(const AMBA_IK_HI_LUMA_BLEND_s *HiLumaBlend);
void AmbaItuner_Get_HiLumaBlend(AMBA_IK_HI_LUMA_BLEND_s *HiLumaBlend);
void AmbaItuner_Set_HiLiCombine(const AMBA_IK_HILI_COMBINE_s *HiLiCombine);
void AmbaItuner_Get_HiLiCombine(AMBA_IK_HILI_COMBINE_s *HiLiCombine);
void AmbaItuner_Set_HiMidHighFreqRecover(const AMBA_IK_HI_MID_HIGH_FREQ_RECOVER_s *HiMidHighFreqRecover);
void AmbaItuner_Get_HiMidHighFreqRecover(AMBA_IK_HI_MID_HIGH_FREQ_RECOVER_s *HiMidHighFreqRecover);
void AmbaItuner_Set_HiLiShpnsBoth(const AMBA_IK_HILI_SHPNS_BOTH_s *HiLiShpnsBoth);
void AmbaItuner_Get_HiLiShpnsBoth(AMBA_IK_HILI_SHPNS_BOTH_s *HiLiShpnsBoth);
void AmbaItuner_Set_HiLiShpnsNoise(const AMBA_IK_HILI_SHPNS_NOISE_s *HiLiShpnsNoise);
void AmbaItuner_Get_HiLiShpnsNoise(AMBA_IK_HILI_SHPNS_NOISE_s *HiLiShpnsNoise);
void AmbaItuner_Set_HiLiShpnsFir(const AMBA_IK_HILI_SHPNS_FIR_s *HiLiShpnsFir);
void AmbaItuner_Get_HiLiShpnsFir(AMBA_IK_HILI_SHPNS_FIR_s *HiLiShpnsFir);
void AmbaItuner_Set_HiLiShpnsCoring(const AMBA_IK_HILI_SHPNS_CORING_s *HiLiShpnsCoring);
void AmbaItuner_Get_HiLiShpnsCoring(AMBA_IK_HILI_SHPNS_CORING_s *HiLiShpnsCoring);
void AmbaItuner_Set_HiLiShpnsCorIdxScl(const AMBA_IK_HILI_SHPNS_COR_IDX_SCL_s *HiLiShpnsCorIdxScl);
void AmbaItuner_Get_HiLiShpnsCorIdxScl(AMBA_IK_HILI_SHPNS_COR_IDX_SCL_s *HiLiShpnsCorIdxScl);
void AmbaItuner_Set_HiLiShpnsMaxCorRst(const AMBA_IK_HILI_SHPNS_MAX_COR_RST_s *HiLiShpnsMaxCorRst);
void AmbaItuner_Get_HiLiShpnsMaxCorRst(AMBA_IK_HILI_SHPNS_MAX_COR_RST_s *HiLiShpnsMaxCorRst);
void AmbaItuner_Set_HiLiShpnsMinCorRst(const AMBA_IK_HILI_SHPNS_MIN_COR_RST_s *HiLiShpnsMinCorRst);
void AmbaItuner_Get_HiLiShpnsMinCorRst(AMBA_IK_HILI_SHPNS_MIN_COR_RST_s *HiLiShpnsMinCorRst);
void AmbaItuner_Set_HiLiShpnsSclCor(const AMBA_IK_HILI_SHPNS_SCL_COR_s *HiLiShpnsSclCor);
void AmbaItuner_Get_HiLiShpnsSclCor(AMBA_IK_HILI_SHPNS_SCL_COR_s *HiLiShpnsSclCor);
void AmbaItuner_Set_Li2Asf(const AMBA_IK_LI2_ASF_s *Li2Asf);
void AmbaItuner_Get_Li2Asf(AMBA_IK_LI2_ASF_s *Li2Asf);
void AmbaItuner_Set_Li2AntiAliasing(const AMBA_IK_LI2_ANTI_ALIASING_s *Li2AntiAliasing);
void AmbaItuner_Get_Li2AntiAliasing(AMBA_IK_LI2_ANTI_ALIASING_s *Li2AntiAliasing);
void AmbaItuner_Set_Li2DynamicBpc(const AMBA_IK_LI2_DYNAMIC_BPC_s *Li2DynamicBpc);
void AmbaItuner_Get_Li2DynamicBpc(AMBA_IK_LI2_DYNAMIC_BPC_s *Li2DynamicBpc);
void AmbaItuner_Set_Li2CfaLeakageFilter(const AMBA_IK_LI2_CFA_LEAKAGE_FILTER_s *Li2CfaLeakageFilter);
void AmbaItuner_Get_Li2CfaLeakageFilter(AMBA_IK_LI2_CFA_LEAKAGE_FILTER_s *Li2CfaLeakageFilter);
void AmbaItuner_Set_Li2CfaNoiseFilter(const AMBA_IK_LI2_CFA_NOISE_FILTER_s *Li2CfaNoiseFilter);
void AmbaItuner_Get_Li2CfaNoiseFilter(AMBA_IK_LI2_CFA_NOISE_FILTER_s *Li2CfaNoiseFilter);
void AmbaItuner_Set_Li2Demosaic(const AMBA_IK_LI2_DEMOSAIC_s *Li2Demosaic);
void AmbaItuner_Get_Li2Demosaic(AMBA_IK_LI2_DEMOSAIC_s *Li2Demosaic);
void AmbaItuner_Set_Li2GrGbMismatch(const AMBA_IK_LI2_GRGB_MISMATCH_s *Li2GrGbMismatch);
void AmbaItuner_Get_Li2GrGbMismatch(AMBA_IK_LI2_GRGB_MISMATCH_s *Li2GrGbMismatch);
void AmbaItuner_Set_Li2ShpnsBoth(const AMBA_IK_LI2_SHPNS_BOTH_s *Li2ShpnsBoth);
void AmbaItuner_Get_Li2ShpnsBoth(AMBA_IK_LI2_SHPNS_BOTH_s *Li2ShpnsBoth);
void AmbaItuner_Set_Li2ShpnsNoise(const AMBA_IK_LI2_SHPNS_NOISE_s *Li2ShpnsNoise);
void AmbaItuner_Get_Li2ShpnsNoise(AMBA_IK_LI2_SHPNS_NOISE_s *Li2ShpnsNoise);
void AmbaItuner_Set_Li2ShpnsFir(const AMBA_IK_LI2_SHPNS_FIR_s *Li2ShpnsFir);
void AmbaItuner_Get_Li2ShpnsFir(AMBA_IK_LI2_SHPNS_FIR_s *Li2ShpnsFir);
void AmbaItuner_Set_Li2ShpnsCoring(const AMBA_IK_LI2_SHPNS_CORING_s *Li2ShpnsCoring);
void AmbaItuner_Get_Li2ShpnsCoring(AMBA_IK_LI2_SHPNS_CORING_s *Li2ShpnsCoring);
void AmbaItuner_Set_Li2ShpnsCorIdxScl(const AMBA_IK_LI2_SHPNS_COR_IDX_SCL_s *Li2ShpnsCorIdxScl);
void AmbaItuner_Get_Li2ShpnsCorIdxScl(AMBA_IK_LI2_SHPNS_COR_IDX_SCL_s *Li2ShpnsCorIdxScl);
void AmbaItuner_Set_Li2ShpnsMaxCorRst(const AMBA_IK_LI2_SHPNS_MAX_COR_RST_s *Li2ShpnsMaxCorRst);
void AmbaItuner_Get_Li2ShpnsMaxCorRst(AMBA_IK_LI2_SHPNS_MAX_COR_RST_s *Li2ShpnsMaxCorRst);
void AmbaItuner_Set_Li2ShpnsMinCorRst(const AMBA_IK_LI2_SHPNS_MIN_COR_RST_s *Li2ShpnsMinCorRst);
void AmbaItuner_Get_Li2ShpnsMinCorRst(AMBA_IK_LI2_SHPNS_MIN_COR_RST_s *Li2ShpnsMinCorRst);
void AmbaItuner_Set_Li2ShpnsSclCor(const AMBA_IK_LI2_SHPNS_SCL_COR_s *Li2ShpnsSclCor);
void AmbaItuner_Get_Li2ShpnsSclCor(AMBA_IK_LI2_SHPNS_SCL_COR_s *Li2ShpnsSclCor);
void AmbaItuner_Set_HiNonsmoothDetect(const AMBA_IK_HI_NONSMOOTH_DETECT_s *HiNonsmoothDetect);
void AmbaItuner_Get_HiNonsmoothDetect(AMBA_IK_HI_NONSMOOTH_DETECT_s *HiNonsmoothDetect);

UINT32 AmbaItuner_Get_SaveInternalParam(void);
INT32 AmbaItuner_Get_Calib_Table(ITUNER_Calib_Table_s **Ituner_Calib_Table);
#endif /* _AMBA_IMG_CALIB_ITUNER_H_ */
/*!
 *
 * @} end of addtogroup Ituner
 */

