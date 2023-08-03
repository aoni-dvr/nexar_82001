/*
*  @file AmbaTUNE_ImgCalibItunerImpl_cv2x.c
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

#include "AmbaDSP_ImageUtility.h"
#include "AmbaTUNE_ImgCalibItuner_cv2x.h"

/* defnine function */
void AmbaItuner_Set_SaveInternalParam(UINT32 value);

#define SPECIAL_USE_NONE 0
#define SPECIAL_USE_SKIP 1
#if 0
typedef struct {
    // Warp part
    UINT32   Enable;
    UINT32   GridArrayWidth;
    UINT32   GridArrayHeight;
    UINT32   HorzGridSpacingExponent;
    UINT32   VertGridSpacingExponent;
    UINT32   VertWarpGridArrayWidth;
    UINT32   VertWarpGridArrayHeight;
    UINT32   VertWarpHorzGridSpacingExponent;
    UINT32   VertWarpVertGridSpacingExponent;
    INT16   *pWarpHorizontalTable;
    INT16   *pWarpVerticalTable;
} AMBA_IK_WARP_INTERNAL_INFO_s;

typedef struct {
    UINT32  HorzWarpEnable;
    UINT32  VertWarpEnable;
    UINT32   HorzPassGridArrayWidth;
    UINT32   HorzPassGridArrayHeight;
    UINT32   HorzPassHorzGridSpacingExponent;
    UINT32   HorzPassVertGridSpacingExponent;
    UINT32   VertPassGridArrayWidth;
    UINT32   VertPassGridArrayHeight;
    UINT32   VertPassHorzGridSpacingExponent;
    UINT32   VertPassVertGridSpacingExponent;
    INT16   WarpHorzTableRed[1536];
    INT16   WarpVertTableRed[1536];
    INT16   WarpHorzTableBlue[1536];
    INT16   WarpVertTableBlue[1536];
} AMBA_IK_CAWARP_INTERNAL_INFO_s;
#endif
typedef struct {
    AAA_FUNC_s AaaFunction;
    ITUNER_AE_INFO_s AeInfo;
    ITUNER_WB_SIM_INFO_s WbSimInfo;
    ITUNER_FPN_s StaticBadPixelCorrection;
    ITUNER_FPN_INTERNAL_s StaticBadPixelCorrectionInternal;
    ITUNER_VIGNETTE_s VignetteCompensation;
    ITUNER_WARP_s WarpCompensation;
    AMBA_IK_DZOOM_INFO_s Dzoom;
    ITUNER_WARP_DZOOM_INTERNAL_s WarpCompensationDzoomInternal;
    ITUNER_SENSOR_INPUT_s SensorInputInfo;
    ITUNER_CHROMA_ABERRATION_s ChromaAberrationInfo;
    ITUNER_CHROMA_ABERRATION_INTERNAL_s ChromaAberrationInfoInternal;
    AMBA_IK_VIN_SENSOR_INFO_s SensorInfo;
    //AMBA_IK_DEFERRED_BLC_s DeferredBlackLevel;
    AMBA_IK_DYNAMIC_BAD_PXL_COR_s DynamicBadPixelCorrection;
    AMBA_IK_CFA_LEAKAGE_FILTER_s CfaLeakageFilter;
    AMBA_IK_CFA_NOISE_FILTER_s CfaNoiseFilter;
    AMBA_IK_ANTI_ALIASING_s AntiAliasing;
    AMBA_IK_WB_GAIN_s BeforeCeWbGain;
    AMBA_IK_WB_GAIN_s AfterCeWbGain;
    AMBA_IK_PRE_CC_GAIN_s PreCcGain;
    AMBA_IK_COLOR_CORRECTION_REG_s CcReg;
    AMBA_IK_COLOR_CORRECTION_s CcThreeD;
    ITUNER_COLOR_CORRECTION_s ColorCorrection;
    AMBA_IK_TONE_CURVE_s ToneCurve;
    ITUNER_FE_TONE_CURVE_s FeToneCurve;
    AMBA_IK_RGB_TO_YUV_MATRIX_s RgbToYuvMatrix;
    AMBA_IK_RGB_IR_s RgbIr;
    AMBA_IK_CHROMA_SCALE_s ChromaScale;
    AMBA_IK_CHROMA_MEDIAN_FILTER_s ChromaMedianFilter;
    AMBA_IK_DEMOSAIC_s DemosaicFilter;
    ITUNER_SHARPEN_BOTH_s SharpenBoth;
    AMBA_IK_FSTSHPNS_NOISE_s SharpenNoise;
    AMBA_IK_FSTSHPNS_FIR_s Fir;
    AMBA_IK_FSTSHPNS_CORING_s Coring;
    AMBA_IK_FSTSHPNS_MIN_COR_RST_s MinCoringResult;
    AMBA_IK_FSTSHPNS_MAX_COR_RST_s MaxCoringResult;
    AMBA_IK_FSTSHPNS_COR_IDX_SCL_s CoringIndexScale;
    AMBA_IK_FSTSHPNS_SCL_COR_s ScaleCoring;
    AMBA_IK_FIRST_LUMA_PROC_MODE_s FirstLumaProcessMode;
    ITUNER_ASF_INFO_s AsfInfo;
    AMBA_IK_CHROMA_FILTER_s ChromaFilter;
    AMBA_IK_GRGB_MISMATCH_s GbGrMismatch;
    ITUNER_VIDEO_CONTRAST_ENHC_s ContrastEnhance;
    ITUNER_VIDEO_HDR_s VideoHdr;
    ITUNER_FINAL_SHARPEN_BOTH_s FinalSharpenBoth;
    AMBA_IK_FNLSHPNS_NOISE_s FinalSharpenNoise;
    AMBA_IK_FNLSHPNS_FIR_s FinalFir;
    AMBA_IK_FNLSHPNS_CORING_s FinalCoring;
    AMBA_IK_FNLSHPNS_MIN_COR_RST_s FinalMinCoringResult;
    AMBA_IK_FNLSHPNS_MAX_COR_RST_s FinalMaxCoringResult;
    AMBA_IK_FNLSHPNS_COR_IDX_SCL_s FinalCoringIndexScale;
    AMBA_IK_FNLSHPNS_SCL_COR_s FinalScaleCoring;
    AMBA_IK_VIDEO_MCTF_s VideoMctf;
    AMBA_IK_VIDEO_MCTF_TA_s VideoMctfTemporalAdjust;
    AMBA_IK_POS_DSP33_t VideoMctfAndFinalSharpen;
    AMBA_IK_WARP_INFO_s WarpCompensationInfo;
    //AMBA_IK_WARP_INTERNAL_INFO_s WarpInternal;
    //AMBA_IK_CAWARP_INTERNAL_INFO_s CAInternal;
    AMBA_IK_CAWARP_INFO_s CaWarpCompensationInfo;
    AMBA_IK_STATIC_BAD_PXL_COR_s SbpCorr;
    AMBA_IK_FE_WB_GAIN_s FrontEndWbGain[4];
    AMBA_IK_STATIC_BLC_LVL_s StaticBlackLevel[4];
    AMBA_IK_DUMMY_MARGIN_RANGE_s Dummy;
    AMBA_IK_VIN_ACTIVE_WINDOW_s Active;
// hiso
    AMBA_IK_HI_ANTI_ALIASING_s HiAntiAliasing;
    AMBA_IK_HI_CFA_LEAKAGE_FILTER_s HiCfaLeakageFilter;
    AMBA_IK_HI_DYNAMIC_BPC_s HiDynamicBpc;
    AMBA_IK_HI_GRGB_MISMATCH_s HiGrGbMismatch;
    AMBA_IK_HI_CHROMA_MEDIAN_FILTER_s HiChromaMedianFilter;
    AMBA_IK_HI_CFA_NOISE_FILTER_s HiCfaNoiseFilter;
    AMBA_IK_HI_DEMOSAIC_s HiDemosaic;
    AMBA_IK_LI2_ANTI_ALIASING_s Li2AntiAliasing;
    AMBA_IK_LI2_CFA_LEAKAGE_FILTER_s Li2CfaLeakageFilter;
    AMBA_IK_LI2_DYNAMIC_BPC_s Li2DynamicBpc;
    AMBA_IK_LI2_GRGB_MISMATCH_s Li2GrGbMismatch;
    AMBA_IK_LI2_CFA_NOISE_FILTER_s Li2CfaNoiseFilter;
    AMBA_IK_LI2_DEMOSAIC_s Li2Demosaic;
    AMBA_IK_HI_ASF_s HiAsf;
    AMBA_IK_LI2_ASF_s Li2Asf;
    AMBA_IK_HI_LOW_ASF_s HiLowAsf;
    AMBA_IK_HI_MED1_ASF_s HiMed1Asf;
    AMBA_IK_HI_MED2_ASF_s HiMed2Asf;
    AMBA_IK_HI_HIGH_ASF_s HiHighAsf;
    AMBA_IK_HI_HIGH2_ASF_s HiHigh2Asf;
    AMBA_IK_HI_CHROMA_ASF_s HiChromaAsf;
    AMBA_IK_HI_LOW_CHROMA_ASF_s ChromaAsf;
    AMBA_IK_HI_HIGH_SHPNS_BOTH_s HiHighShpnsBoth;
    AMBA_IK_HI_HIGH_SHPNS_NOISE_s HiHighShpnsNoise;
    AMBA_IK_HI_HIGH_SHPNS_CORING_s HiHighShpnsCoring;
    AMBA_IK_HI_HIGH_SHPNS_FIR_s HiHighShpnsFir;
    AMBA_IK_HI_HIGH_SHPNS_COR_IDX_SCL_s HiHighShpnsCorIdxScl;
    AMBA_IK_HI_HIGH_SHPNS_MIN_COR_RST_s HiHighShpnsMinCorRst;
    AMBA_IK_HI_HIGH_SHPNS_MAX_COR_RST_s HiHighShpnsMaxCorRst;
    AMBA_IK_HI_HIGH_SHPNS_SCL_COR_s HiHighShpnsSclCor;
    AMBA_IK_HI_MED_SHPNS_BOTH_s HiMedShpnsBoth;
    AMBA_IK_HI_MED_SHPNS_NOISE_s HiMedShpnsNoise;
    AMBA_IK_HI_MED_SHPNS_CORING_s HiMedShpnsCoring;
    AMBA_IK_HI_MED_SHPNS_FIR_s HiMedShpnsFir;
    AMBA_IK_HI_MED_SHPNS_COR_IDX_SCL_s HiMedShpnsCorIdxScl;
    AMBA_IK_HI_MED_SHPNS_MIN_COR_RST_s HiMedShpnsMinCorRst;
    AMBA_IK_HI_MED_SHPNS_MAX_COR_RST_s HiMedShpnsMaxCorRst;
    AMBA_IK_HI_MED_SHPNS_SCL_COR_s HiMedShpnsSclCor;
    AMBA_IK_LI2_SHPNS_BOTH_s Li2ShpnsBoth;
    AMBA_IK_LI2_SHPNS_NOISE_s Li2ShpnsNoise;
    AMBA_IK_LI2_SHPNS_CORING_s Li2ShpnsCoring;
    AMBA_IK_LI2_SHPNS_FIR_s Li2ShpnsFir;
    AMBA_IK_LI2_SHPNS_COR_IDX_SCL_s Li2ShpnsCorIdxScl;
    AMBA_IK_LI2_SHPNS_MIN_COR_RST_s Li2ShpnsMinCorRst;
    AMBA_IK_LI2_SHPNS_MAX_COR_RST_s Li2ShpnsMaxCorRst;
    AMBA_IK_LI2_SHPNS_SCL_COR_s Li2ShpnsSclCor;
    AMBA_IK_HILI_SHPNS_BOTH_s HiLiShpnsBoth;
    AMBA_IK_HILI_SHPNS_NOISE_s HiLiShpnsNoise;
    AMBA_IK_HILI_SHPNS_CORING_s HiLiShpnsCoring;
    AMBA_IK_HILI_SHPNS_FIR_s HiLiShpnsFir;
    AMBA_IK_HILI_SHPNS_COR_IDX_SCL_s HiLiShpnsCorIdxScl;
    AMBA_IK_HILI_SHPNS_MIN_COR_RST_s HiLiShpnsMinCorRst;
    AMBA_IK_HILI_SHPNS_MAX_COR_RST_s HiLiShpnsMaxCorRst;
    AMBA_IK_HILI_SHPNS_SCL_COR_s HiLiShpnsSclCor;
    AMBA_IK_HI_CHROMA_FILTER_HIGH_s HiChromaFilterHigh;
    AMBA_IK_HI_CHROMA_FILTER_PRE_s HiChromaFilterPre;
    AMBA_IK_HI_CHROMA_FILTER_MED_s HiChromaFilterMed;
    AMBA_IK_HI_CHROMA_FILTER_LOW_s HiChromaFilterLow;
    AMBA_IK_HI_CHROMA_FILTER_VERY_LOW_s HiChromaFilterVeryLow;
    AMBA_IK_HI_LUMA_COMBINE_s HiLumaCombine;
    AMBA_IK_HI_LOW_ASF_COMBINE_s HiLowAsfCombine;
    AMBA_IK_HI_CHROMA_FLTR_MED_COM_s HiChromaFilterMedCombine;
    AMBA_IK_HI_CHROMA_FLTR_LOW_COM_s HiChromaFilterLowCombine;
    AMBA_IK_HI_CHROMA_FLTR_VERY_LOW_COM_s HiChromaFilterVeryLowCombine;
    AMBA_IK_HILI_COMBINE_s HiLiCombine;
    AMBA_IK_HI_MID_HIGH_FREQ_RECOVER_s HiMidHighFreqRecover;
    AMBA_IK_HI_LUMA_BLEND_s HiLumaBlend;
    AMBA_IK_HI_NONSMOOTH_DETECT_s HiNonsmoothDetect;
    AMBA_IK_HI_SELECT_s HiSelect;
} ITUNER_DATA_s;

typedef struct {
    UINT8  Reserved[1228];
    UINT32 Sec1[19];
} ITUNER_IDSP_s;

// Note: FPN_Map of GCalib_Table need 32 alignment
static ITUNER_Calib_Table_s *pGCalib_Table = NULL;
static AMBA_ITUNER_VALID_FILTER_t Ituner_Valid_Filter;
static ITUNER_DATA_s GData;// GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static INT32 Dummy_Func(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s *Ext_File_Param);
static AmbaItuner_Hook_Func_t Ituner_Hook_Func = { .Load_Data = Dummy_Func, .Save_Data = Dummy_Func, };
static AMBA_ITN_SYSTEM_s SystemInfo;
static Ituner_Internal_s InternalInfo;
static UINT32 AmbaTuneSaveInternalParam = 0;
static UINT32 CompandEnable = 0UL;
static UINT32 Compand[IK_NUM_COMPAND_TABLE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static UINT32 CompandCount = 0UL;
static INT32 AmbaItuner_Check_Reg(UINT32 FILTER_ID, INT32 Special_Use);
typedef void (*Set_Filter_Valid)(UINT32 Filter_Id);
//extern UINT32 AmbaIK_SetWarpInternal(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WARP_INTERNAL_INFO_s *pWarpInternal);
static inline void AmbaItuner_Set_Filter_Valid(UINT32 Filter_Id)
{
    Ituner_Valid_Filter[Filter_Id] = 1;
}

static inline void Dummy_Set_Filter_Valid(UINT32 Filter_Id)
{
    if (Filter_Id==ITUNER_SYSTEM_INFO) {
        // MisraC fix
    }
}

UINT32 AmbaItuner_Get_SaveInternalParam(void)
{
    return AmbaTuneSaveInternalParam;
}

void AmbaItuner_Set_SaveInternalParam(UINT32 value)
{
    AmbaTuneSaveInternalParam = value;
}

INT32 AmbaItuner_Get_Calib_Table(ITUNER_Calib_Table_s **Ituner_Calib_Table)
{
    *Ituner_Calib_Table = pGCalib_Table;
    return 0;
}

typedef struct {
    const char* filter_name;
    UINT32 filter_id;
} filter_n;

static const char* AmbaItuner_Get_Filter_Name(UINT32 filter_id)
{
    static volatile const filter_n Filter_Lut[ITUNER_MAX] = {
        {"ITUNER_SYSTEM_INFO", ITUNER_SYSTEM_INFO},
        {"ITUNER_INTERNAL_INFO", ITUNER_INTERNAL_INFO},
        {"ITUNER_AAA_FUNCTION", ITUNER_AAA_FUNCTION},
        {"ITUNER_AE_INFO", ITUNER_AE_INFO},
        {"ITUNER_WB_SIM_INFO", ITUNER_WB_SIM_INFO},
        {"ITUNER_STATIC_BAD_PIXEL_CORRECTION", ITUNER_STATIC_BAD_PIXEL_CORRECTION},
        {"ITUNER_STATIC_BAD_PIXEL_INTERNAL", ITUNER_STATIC_BAD_PIXEL_INTERNAL},
        {"ITUNER_VIGNETTE_COMPENSATION", ITUNER_VIGNETTE_COMPENSATION},
        {"ITUNER_VIGNETTE_COMPENSATION_INTERNAL", ITUNER_VIGNETTE_COMPENSATION_INTERNAL},
        {"ITUNER_CHROMA_ABERRATION_INFO", ITUNER_CHROMA_ABERRATION_INFO},
        {"ITUNER_CHROMA_ABERRATION_INFO_INTERNAL", ITUNER_CHROMA_ABERRATION_INFO_INTERNAL},
        {"ITUNER_WARP_COMPENSATION", ITUNER_WARP_COMPENSATION},
        {"ITUNER_WARP_COMPENSATION_DZOOM_INTERNAL", ITUNER_WARP_COMPENSATION_DZOOM_INTERNAL},
        {"ITUNER_DZOOM", ITUNER_DZOOM},
        {"ITUNER_DEFERRED_BLACK_LEVEL", ITUNER_DEFERRED_BLACK_LEVEL},
        {"ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION", ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION},
        {"ITUNER_CFA_LEAKAGE_FILTER", ITUNER_CFA_LEAKAGE_FILTER},
        {"ITUNER_CFA_NOISE_FILTER", ITUNER_CFA_NOISE_FILTER},
        {"ITUNER_ANTI_ALIASING_STRENGTH", ITUNER_ANTI_ALIASING_STRENGTH},
        {"ITUNER_BEFORE_CE_WB_GAIN", ITUNER_BEFORE_CE_WB_GAIN},
        {"ITUNER_AFTER_CE_WB_GAIN", ITUNER_AFTER_CE_WB_GAIN},
        {"ITUNER_PRE_CC_GAIN", ITUNER_PRE_CC_GAIN},
        {"ITUNER_COLOR_CORRECTION", ITUNER_COLOR_CORRECTION},
        {"ITUNER_TONE_CURVE", ITUNER_TONE_CURVE},
        {"ITUNER_FRONT_END_TONE_CURVE", ITUNER_FRONT_END_TONE_CURVE},
        {"ITUNER_RGB_TO_YUV_MATRIX", ITUNER_RGB_TO_YUV_MATRIX},
        {"ITUNER_RGB_IR", ITUNER_RGB_IR},
        {"ITUNER_CHROMA_SCALE", ITUNER_CHROMA_SCALE},
        {"ITUNER_CHROMA_MEDIAN_FILTER", ITUNER_CHROMA_MEDIAN_FILTER},
        {"ITUNER_DEMOSAIC_FILTER", ITUNER_DEMOSAIC_FILTER},
        {"ITUNER_GB_GR_MISMATCH", ITUNER_GB_GR_MISMATCH},
        {"ITUNER_VIDEO_MCTF", ITUNER_VIDEO_MCTF},
        {"ITUNER_VIDEO_MCTF_LEVEL", ITUNER_VIDEO_MCTF_LEVEL},
        {"ITUNER_VIDEO_MCTF_TEMPORAL_ADJUST", ITUNER_VIDEO_MCTF_TEMPORAL_ADJUST},
        {"ITUNER_VIDEO_MCTF_Y_TEMPORAL_ADJUST", ITUNER_VIDEO_MCTF_Y_TEMPORAL_ADJUST},
        {"ITUNER_VIDEO_MCTF_CB_TEMPORAL_ADJUST", ITUNER_VIDEO_MCTF_CB_TEMPORAL_ADJUST},
        {"ITUNER_VIDEO_MCTF_CR_TEMPORAL_ADJUST", ITUNER_VIDEO_MCTF_CR_TEMPORAL_ADJUST},
        {"ITUNER_VIDEO_MCTF_AND_FINAL_SHARPEN", ITUNER_VIDEO_MCTF_AND_FINAL_SHARPEN},
        {"ITUNER_SENSOR_INPUT_INFO", ITUNER_SENSOR_INPUT_INFO},
        {"ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT", ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT},
        {"ITUNER_ASF_INFO", ITUNER_ASF_INFO},
        {"ITUNER_CHROMA_FILTER", ITUNER_CHROMA_FILTER},
        {"ITUNER_SHARPEN_BOTH", ITUNER_SHARPEN_BOTH},
        {"ITUNER_SHARPEN_NOISE", ITUNER_SHARPEN_NOISE},
        {"ITUNER_FIR", ITUNER_FIR},
        {"ITUNER_CORING", ITUNER_CORING},
        {"ITUNER_CORING_INDEX_SCALE", ITUNER_CORING_INDEX_SCALE},
        {"ITUNER_MIN_CORING_RESULT", ITUNER_MIN_CORING_RESULT},
        {"ITUNER_MAX_CORING_RESULT", ITUNER_MAX_CORING_RESULT},
        {"ITUNER_SCALE_CORING", ITUNER_SCALE_CORING},
        {"ITUNER_FINAL_SHARPEN_BOTH", ITUNER_FINAL_SHARPEN_BOTH},
        {"ITUNER_FINAL_SHARPEN_NOISE", ITUNER_FINAL_SHARPEN_NOISE},
        {"ITUNER_FINAL_FIR", ITUNER_FINAL_FIR},
        {"ITUNER_FINAL_CORING", ITUNER_FINAL_CORING},
        {"ITUNER_FINAL_CORING_INDEX_SCALE", ITUNER_FINAL_CORING_INDEX_SCALE},
        {"ITUNER_FINAL_MIN_CORING_RESULT", ITUNER_FINAL_MIN_CORING_RESULT},
        {"ITUNER_FINAL_MAX_CORING_RESULT", ITUNER_FINAL_MAX_CORING_RESULT},
        {"ITUNER_FINAL_SCALE_CORING", ITUNER_FINAL_SCALE_CORING},
        {"ITUNER_CONTRAST_ENHANCE", ITUNER_CONTRAST_ENHANCE},
        {"ITUNER_VHDR_BLEND", ITUNER_VHDR_BLEND},
        {"ITUNER_FRONT_END_WB_GAIN_EXP0", ITUNER_FRONT_END_WB_GAIN_EXP0},
        {"ITUNER_FRONT_END_WB_GAIN_EXP1", ITUNER_FRONT_END_WB_GAIN_EXP1},
        {"ITUNER_FRONT_END_WB_GAIN_EXP2", ITUNER_FRONT_END_WB_GAIN_EXP2},
        {"ITUNER_VHDR_RAW_INFO", ITUNER_VHDR_RAW_INFO},
        {"ITUNER_VHDR_SECONDARY_WB_GAIN", ITUNER_VHDR_SECONDARY_WB_GAIN},
        {"ITUNER_STATIC_BLACK_LEVEL_EXP0", ITUNER_STATIC_BLACK_LEVEL_EXP0},
        {"ITUNER_STATIC_BLACK_LEVEL_EXP1", ITUNER_STATIC_BLACK_LEVEL_EXP1},
        {"ITUNER_STATIC_BLACK_LEVEL_EXP2", ITUNER_STATIC_BLACK_LEVEL_EXP2},
        {"ITUNER_DUMMY", ITUNER_DUMMY},
        {"ITUNER_ACTIVE", ITUNER_ACTIVE},
        {"ITUNER_HDS_AMP_LINEAR", ITUNER_HDS_AMP_LINEAR},
        {"ITUNER_SENSOR_INFO", ITUNER_SENSOR_INFO},
        {"ITUNER_VHDR_LE_STATIC_BLACK_LEVEL", ITUNER_VHDR_LE_STATIC_BLACK_LEVEL},
        {"ITUNER_VHDR_ALPHA_CALC_CFG", ITUNER_VHDR_ALPHA_CALC_CFG},
        {"ITUNER_VHDR_ALPHA_CALC_THRESH", ITUNER_VHDR_ALPHA_CALC_THRESH},
        {"ITUNER_VHDR_ALPHA_CALC_BLACK_LEVEL", ITUNER_VHDR_ALPHA_CALC_BLACK_LEVEL},
        {"ITUNER_VHDR_ALPHA_CALC_ALPHA", ITUNER_VHDR_ALPHA_CALC_ALPHA},
        {"ITUNER_VHDR_AMP_LINEAR", ITUNER_VHDR_AMP_LINEAR},
        {"ITUNER_VHDR_AMP_LINEAR_BLEND", ITUNER_VHDR_AMP_LINEAR_BLEND},
        {"ITUNER_LI_CHROMA_ASF", ITUNER_LI_CHROMA_ASF},
        {"ITUNER_HI_CHROMA_ASF", ITUNER_HI_CHROMA_ASF},
        {"ITUNER_HI_LUMA_ASF", ITUNER_HI_LUMA_ASF},
        {"ITUNER_HI_CFA_NOISE_FILTER", ITUNER_HI_CFA_NOISE_FILTER},
        {"ITUNER_HI_CFA_LEAKAGE_FILTER", ITUNER_HI_CFA_LEAKAGE_FILTER},
        {"ITUNER_HI_ANTI_ALIASING", ITUNER_HI_ANTI_ALIASING},
        {"ITUNER_HI_AUTO_BAD_PIXEL_CORRECTION", ITUNER_HI_AUTO_BAD_PIXEL_CORRECTION},
        {"ITUNER_HI_CHROMA_FLTR_HIGH", ITUNER_HI_CHROMA_FLTR_HIGH},
        {"ITUNER_HI_CHROMA_FLTR_LOW", ITUNER_HI_CHROMA_FLTR_LOW},
        {"ITUNER_HI_CHROMA_FLTR_LOW_COMBINE", ITUNER_HI_CHROMA_FLTR_LOW_COMBINE},
        {"ITUNER_HI_CHROMA_FLTR_MEDIAN", ITUNER_HI_CHROMA_FLTR_MEDIAN},
        {"ITUNER_HI_CHROMA_FLTR_MEDIAN_COMBINE", ITUNER_HI_CHROMA_FLTR_MEDIAN_COMBINE},
        {"ITUNER_HI_CHROMA_FLTR_PRE", ITUNER_HI_CHROMA_FLTR_PRE},
        {"ITUNER_HI_CHROMA_FLTR_VERY_LOW", ITUNER_HI_CHROMA_FLTR_VERY_LOW},
        {"ITUNER_HI_CHROMA_FLTR_VERY_LOW_COMBINE", ITUNER_HI_CHROMA_FLTR_VERY_LOW_COMBINE},
        {"ITUNER_HI_CHROMA_MEDIAN_FILTER", ITUNER_HI_CHROMA_MEDIAN_FILTER},
        {"ITUNRE_HI_DEMOSAIC_FILTER", ITUNRE_HI_DEMOSAIC_FILTER},
        {"ITUNER_HI_GB_GR_MISMATCH", ITUNER_HI_GB_GR_MISMATCH},
        {"ITUNER_HI_HIGH2_LUMA_ASF", ITUNER_HI_HIGH2_LUMA_ASF},
        {"ITUNER_HI_HIGH_LUMA_ASF", ITUNER_HI_HIGH_LUMA_ASF},
        {"ITUNER_HI_HIGH_SHARPEN_BOTH", ITUNER_HI_HIGH_SHARPEN_BOTH},
        {"ITUNER_HI_HIGH_SHARPEN_NOISE", ITUNER_HI_HIGH_SHARPEN_NOISE},
        {"ITUNER_HI_HIGH_FIR", ITUNER_HI_HIGH_FIR},
        {"ITUNER_HI_HIGH_CORING", ITUNER_HI_HIGH_CORING},
        {"ITUNER_HI_HIGH_CORING_INDEX_SCALE", ITUNER_HI_HIGH_CORING_INDEX_SCALE},
        {"ITUNER_HI_HIGH_MAX_CORING_RESULT", ITUNER_HI_HIGH_MAX_CORING_RESULT},
        {"ITUNER_HI_HIGH_MIN_CORING_RESULT", ITUNER_HI_HIGH_MIN_CORING_RESULT},
        {"ITUNER_HI_HIGH_SCALE_CORING", ITUNER_HI_HIGH_SCALE_CORING},
        {"ITUNER_HI_LOW_LUMA_ASF", ITUNER_HI_LOW_LUMA_ASF},
        {"ITUNER_HI_LOW_LUMA_ASF_COMBINE", ITUNER_HI_LOW_LUMA_ASF_COMBINE},
        {"ITUNER_HI_LUMA_NOISE_COMBINE", ITUNER_HI_LUMA_NOISE_COMBINE},
        {"ITUNER_HI_MED1_LUMA_ASF", ITUNER_HI_MED1_LUMA_ASF},
        {"ITUNER_HI_MED2_LUMA_ASF", ITUNER_HI_MED2_LUMA_ASF},
        {"ITUNER_HI_MED_SHARPEN_BOTH", ITUNER_HI_MED_SHARPEN_BOTH},
        {"ITUNER_HI_MED_SHARPEN_NOISE", ITUNER_HI_MED_SHARPEN_NOISE},
        {"ITUNER_HI_MED_FIR", ITUNER_HI_MED_FIR},
        {"ITUNER_HI_MED_CORING", ITUNER_HI_MED_CORING},
        {"ITUNER_HI_MED_CORING_INDEX_SCALE", ITUNER_HI_MED_CORING_INDEX_SCALE},
        {"ITUNER_HI_MED_MAX_CORING_RESULT", ITUNER_HI_MED_MAX_CORING_RESULT},
        {"ITUNER_HI_MED_MIN_CORING_RESULT", ITUNER_HI_MED_MIN_CORING_RESULT},
        {"ITUNER_HI_MED_SCALE_CORING", ITUNER_HI_MED_SCALE_CORING},
        {"ITUNER_HI_SELECT", ITUNER_HI_SELECT},
        {"ITUNER_HI_LUMA_BLEND", ITUNER_HI_LUMA_BLEND},
        {"ITUNER_HILI_COMBINE", ITUNER_HILI_COMBINE},
        {"ITUNER_HI_MED_HIGH_FREQ_RECOVER", ITUNER_HI_MED_HIGH_FREQ_RECOVER},
        {"ITUNER_HILI_SHARPEN_BOTH", ITUNER_HILI_SHARPEN_BOTH},
        {"ITUNER_HILi_SHARPEN_NOISE", ITUNER_HILi_SHARPEN_NOISE},
        {"ITUNER_HILI_FIR", ITUNER_HILI_FIR},
        {"ITUNER_HILI_CORING", ITUNER_HILI_CORING},
        {"ITUNER_HILI_CORING_INDEX_SCALE", ITUNER_HILI_CORING_INDEX_SCALE},
        {"ITUNER_HILI_MAX_CORING_RESULT", ITUNER_HILI_MAX_CORING_RESULT},
        {"ITUNER_HILI_MIN_CORING_RESULT", ITUNER_HILI_MIN_CORING_RESULT},
        {"ITUNER_HILI_SCALE_CORING", ITUNER_HILI_SCALE_CORING},
        {"ITUNER_LI2_LUMA_ASF", ITUNER_LI2_LUMA_ASF},
        {"ITUNER_LI2_ANTI_ALIASING", ITUNER_LI2_ANTI_ALIASING},
        {"ITUNER_LI2_AUTO_BAD_PIXEL_CORRECTION", ITUNER_LI2_AUTO_BAD_PIXEL_CORRECTION},
        {"ITUNER_LI2_CFA_LEAKAGE_FILTER", ITUNER_LI2_CFA_LEAKAGE_FILTER},
        {"ITUNER_LI2_CFA_NOISE_FILTER", ITUNER_LI2_CFA_NOISE_FILTER},
        {"ITUNRE_LI2_DEMOSAIC_FILTER", ITUNRE_LI2_DEMOSAIC_FILTER},
        {"ITUNER_LI2_GB_GR_MISMATCH", ITUNER_LI2_GB_GR_MISMATCH},
        {"ITUNER_LI2_SHARPEN_BOTH", ITUNER_LI2_SHARPEN_BOTH},
        {"ITUNER_LI2_SHARPEN_NOISE", ITUNER_LI2_SHARPEN_NOISE},
        {"ITUNER_LI2_FIR", ITUNER_LI2_FIR},
        {"ITUNER_LI2_CORING", ITUNER_LI2_CORING},
        {"ITUNER_LI2_CORING_INDEX_SCALE", ITUNER_LI2_CORING_INDEX_SCALE},
        {"ITUNER_LI2_MAX_CORING_RESULT", ITUNER_LI2_MAX_CORING_RESULT},
        {"ITUNER_LI2_MIN_CORING_RESULT", ITUNER_LI2_MIN_CORING_RESULT},
        {"ITUNER_LI2_SCALE_CORING", ITUNER_LI2_SCALE_CORING},
        {"ITUNER_HI_NONSMOOTH_DETECT", ITUNER_HI_NONSMOOTH_DETECT},
    };
    const char* filter_name_return = "ITUNER_UNKNOWN_FILTER";
    INT32 i;
    for (i = 0; i < (INT32)ITUNER_MAX; i++) {
        if (filter_id == Filter_Lut[i].filter_id) {
            filter_name_return = Filter_Lut[i].filter_name;
            break;
        }
    }
    return filter_name_return;
}


static INT32 AmbaItuner_Is_Video_Preview_Tuning(void)
{
    return (0 == ituner_strcmp(SystemInfo.TuningMode, "IMG_MODE_PREVIEW")) ? 1L : 0L;
}

static inline INT32 AmbaItuner_Check_Filter_Still(const AMBA_IK_ABILITY_s *Ability, const UINT8 *hiso_filter, const UINT8 size)
{
    INT32 rval = 0;
    UINT8 i = 0;
    const UINT8 *necessary_filter = NULL;

    if (Ability->StillPipe == AMBA_IK_STILL_HISO) {
        necessary_filter = hiso_filter;
    } else {
        ituner_print_uint32_5("Not support still_pipe: %d", Ability->StillPipe, DC_U, DC_U, DC_U, DC_U);
        rval = -1;
    }
    if (necessary_filter != NULL) {
        for (i = 0; i < size; i++) {
            if (Ituner_Valid_Filter[(UINT8)necessary_filter[i]] == 0U) {
                ituner_print_uint32_5("still pipe: %d %d", Ability->StillPipe, DC_U, DC_U, DC_U, DC_U);
                ituner_print_str_5(" Lose Filter %s ", AmbaItuner_Get_Filter_Name(necessary_filter[i]), DC_S, DC_S, DC_S, DC_S);
                rval = -1;
            }
        }
    }
    return rval;
}

static INT32 AmbaItuner_Check_Filter(const AMBA_IK_ABILITY_s *Ability)
{
    const UINT8 liso_filter[] = {
        ITUNER_STATIC_BLACK_LEVEL_EXP0,
        ITUNER_FRONT_END_WB_GAIN_EXP0,
        ITUNER_COLOR_CORRECTION,
        ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION,
        ITUNER_CFA_LEAKAGE_FILTER,
        ITUNER_CFA_NOISE_FILTER,
        ITUNER_ANTI_ALIASING_STRENGTH,
        ITUNER_BEFORE_CE_WB_GAIN,
        ITUNER_AFTER_CE_WB_GAIN,
        ITUNER_TONE_CURVE,
        ITUNER_RGB_TO_YUV_MATRIX,
        ITUNER_CHROMA_SCALE,
        ITUNER_CHROMA_MEDIAN_FILTER,
        ITUNER_DEMOSAIC_FILTER,
        ITUNER_GB_GR_MISMATCH,
        ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT,
        ITUNER_CHROMA_FILTER,
        ITUNER_VIDEO_MCTF,
        ITUNER_VIDEO_MCTF_LEVEL,
        ITUNER_FINAL_SHARPEN_BOTH,
        ITUNER_FINAL_SHARPEN_NOISE,
        ITUNER_FINAL_FIR,
        ITUNER_FINAL_CORING,
        ITUNER_FINAL_CORING_INDEX_SCALE,
        ITUNER_FINAL_MIN_CORING_RESULT,
        ITUNER_FINAL_SCALE_CORING,
    };
    const UINT8 liso_md_filter[] = {
        ITUNER_STATIC_BLACK_LEVEL_EXP0,
        ITUNER_FRONT_END_WB_GAIN_EXP0,
        ITUNER_COLOR_CORRECTION,
        ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION,
        ITUNER_CFA_LEAKAGE_FILTER,
        ITUNER_CFA_NOISE_FILTER,
        ITUNER_ANTI_ALIASING_STRENGTH,
        ITUNER_BEFORE_CE_WB_GAIN,
        ITUNER_AFTER_CE_WB_GAIN,
        ITUNER_TONE_CURVE,
        ITUNER_RGB_TO_YUV_MATRIX,
        ITUNER_CHROMA_SCALE,
        ITUNER_CHROMA_MEDIAN_FILTER,
        ITUNER_DEMOSAIC_FILTER,
        ITUNER_GB_GR_MISMATCH,
        ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT,
        ITUNER_CHROMA_FILTER,
        ITUNER_VIDEO_MCTF,
        ITUNER_VIDEO_MCTF_LEVEL,
        ITUNER_VIDEO_MCTF_AND_FINAL_SHARPEN,
        ITUNER_FINAL_SHARPEN_BOTH,
        ITUNER_FINAL_SHARPEN_NOISE,
        ITUNER_FINAL_FIR,
        ITUNER_FINAL_CORING,
        ITUNER_FINAL_CORING_INDEX_SCALE,
        ITUNER_FINAL_MIN_CORING_RESULT,
        ITUNER_FINAL_SCALE_CORING,
    };
    const UINT8 linear_ce_filter[] = {
        ITUNER_STATIC_BLACK_LEVEL_EXP0,
        ITUNER_FRONT_END_WB_GAIN_EXP0,
        ITUNER_COLOR_CORRECTION,
        ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION,
        ITUNER_CFA_LEAKAGE_FILTER,
        ITUNER_CFA_NOISE_FILTER,
        ITUNER_ANTI_ALIASING_STRENGTH,
        ITUNER_BEFORE_CE_WB_GAIN,
        ITUNER_AFTER_CE_WB_GAIN,
        ITUNER_TONE_CURVE,
        ITUNER_RGB_TO_YUV_MATRIX,
        ITUNER_CHROMA_SCALE,
        ITUNER_CHROMA_MEDIAN_FILTER,
        ITUNER_DEMOSAIC_FILTER,
        ITUNER_GB_GR_MISMATCH,
        ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT,
        ITUNER_CHROMA_FILTER,
        ITUNER_VIDEO_MCTF,
        ITUNER_VIDEO_MCTF_LEVEL,
        ITUNER_FINAL_SHARPEN_BOTH,
        ITUNER_FINAL_SHARPEN_NOISE,
        ITUNER_FINAL_FIR,
        ITUNER_FINAL_CORING,
        ITUNER_FINAL_CORING_INDEX_SCALE,
        ITUNER_FINAL_MIN_CORING_RESULT,
        ITUNER_FINAL_SCALE_CORING,
        ITUNER_CONTRAST_ENHANCE,
        ITUNER_FRONT_END_TONE_CURVE,
    };
    const UINT8 hdr_2x_filter[] = {
        ITUNER_COLOR_CORRECTION,
        ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION,
        ITUNER_CFA_LEAKAGE_FILTER,
        ITUNER_CFA_NOISE_FILTER,
        ITUNER_ANTI_ALIASING_STRENGTH,
        ITUNER_BEFORE_CE_WB_GAIN,
        ITUNER_AFTER_CE_WB_GAIN,
        ITUNER_TONE_CURVE,
        ITUNER_RGB_TO_YUV_MATRIX,
        ITUNER_CHROMA_SCALE,
        ITUNER_CHROMA_MEDIAN_FILTER,
        ITUNER_DEMOSAIC_FILTER,
        ITUNER_GB_GR_MISMATCH,
        ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT,
        ITUNER_CHROMA_FILTER,
        ITUNER_VIDEO_MCTF,
        ITUNER_VIDEO_MCTF_LEVEL,
        ITUNER_FINAL_SHARPEN_BOTH,
        ITUNER_FINAL_SHARPEN_NOISE,
        ITUNER_FINAL_FIR,
        ITUNER_FINAL_CORING,
        ITUNER_FINAL_CORING_INDEX_SCALE,
        ITUNER_FINAL_MIN_CORING_RESULT,
        ITUNER_FINAL_SCALE_CORING,
        ITUNER_CONTRAST_ENHANCE,
        ITUNER_STATIC_BLACK_LEVEL_EXP0,
        ITUNER_STATIC_BLACK_LEVEL_EXP1,
        ITUNER_FRONT_END_WB_GAIN_EXP0,
        ITUNER_FRONT_END_WB_GAIN_EXP1,
        ITUNER_VHDR_BLEND,
    };

    const UINT8 hdr_3x_filter[] = {
        ITUNER_COLOR_CORRECTION,
        ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION,
        ITUNER_CFA_LEAKAGE_FILTER,
        ITUNER_CFA_NOISE_FILTER,
        ITUNER_ANTI_ALIASING_STRENGTH,
        ITUNER_BEFORE_CE_WB_GAIN,
        ITUNER_AFTER_CE_WB_GAIN,
        ITUNER_TONE_CURVE,
        ITUNER_RGB_TO_YUV_MATRIX,
        ITUNER_CHROMA_SCALE,
        ITUNER_CHROMA_MEDIAN_FILTER,
        ITUNER_DEMOSAIC_FILTER,
        ITUNER_GB_GR_MISMATCH,
        ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT,
        ITUNER_CHROMA_FILTER,
        ITUNER_VIDEO_MCTF,
        ITUNER_VIDEO_MCTF_LEVEL,
        ITUNER_FINAL_SHARPEN_BOTH,
        ITUNER_FINAL_SHARPEN_NOISE,
        ITUNER_FINAL_FIR,
        ITUNER_FINAL_CORING,
        ITUNER_FINAL_CORING_INDEX_SCALE,
        ITUNER_FINAL_MIN_CORING_RESULT,
        ITUNER_FINAL_SCALE_CORING,
        ITUNER_CONTRAST_ENHANCE,
        ITUNER_STATIC_BLACK_LEVEL_EXP0,
        ITUNER_STATIC_BLACK_LEVEL_EXP1,
        ITUNER_STATIC_BLACK_LEVEL_EXP2,
        ITUNER_FRONT_END_WB_GAIN_EXP0,
        ITUNER_FRONT_END_WB_GAIN_EXP1,
        ITUNER_FRONT_END_WB_GAIN_EXP2,
        ITUNER_VHDR_BLEND,
    };

    const UINT8 linear_ce_md_filter[] = {
        ITUNER_STATIC_BLACK_LEVEL_EXP0,
        ITUNER_FRONT_END_WB_GAIN_EXP0,
        ITUNER_COLOR_CORRECTION,
        ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION,
        ITUNER_CFA_LEAKAGE_FILTER,
        ITUNER_CFA_NOISE_FILTER,
        ITUNER_ANTI_ALIASING_STRENGTH,
        ITUNER_BEFORE_CE_WB_GAIN,
        ITUNER_AFTER_CE_WB_GAIN,
        ITUNER_TONE_CURVE,
        ITUNER_RGB_TO_YUV_MATRIX,
        ITUNER_CHROMA_SCALE,
        ITUNER_CHROMA_MEDIAN_FILTER,
        ITUNER_DEMOSAIC_FILTER,
        ITUNER_GB_GR_MISMATCH,
        ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT,
        ITUNER_CHROMA_FILTER,
        ITUNER_VIDEO_MCTF,
        ITUNER_VIDEO_MCTF_LEVEL,
        ITUNER_VIDEO_MCTF_AND_FINAL_SHARPEN,
        ITUNER_FINAL_SHARPEN_BOTH,
        ITUNER_FINAL_SHARPEN_NOISE,
        ITUNER_FINAL_FIR,
        ITUNER_FINAL_CORING,
        ITUNER_FINAL_CORING_INDEX_SCALE,
        ITUNER_FINAL_MIN_CORING_RESULT,
        ITUNER_FINAL_SCALE_CORING,
        ITUNER_CONTRAST_ENHANCE,
        ITUNER_FRONT_END_TONE_CURVE,
    };
    const UINT8 hdr_2x_md_filter[] = {
        ITUNER_COLOR_CORRECTION,
        ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION,
        ITUNER_CFA_LEAKAGE_FILTER,
        ITUNER_CFA_NOISE_FILTER,
        ITUNER_ANTI_ALIASING_STRENGTH,
        ITUNER_BEFORE_CE_WB_GAIN,
        ITUNER_AFTER_CE_WB_GAIN,
        ITUNER_TONE_CURVE,
        ITUNER_RGB_TO_YUV_MATRIX,
        ITUNER_CHROMA_SCALE,
        ITUNER_CHROMA_MEDIAN_FILTER,
        ITUNER_DEMOSAIC_FILTER,
        ITUNER_GB_GR_MISMATCH,
        ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT,
        ITUNER_CHROMA_FILTER,
        ITUNER_VIDEO_MCTF,
        ITUNER_VIDEO_MCTF_LEVEL,
        ITUNER_VIDEO_MCTF_AND_FINAL_SHARPEN,
        ITUNER_FINAL_SHARPEN_BOTH,
        ITUNER_FINAL_SHARPEN_NOISE,
        ITUNER_FINAL_FIR,
        ITUNER_FINAL_CORING,
        ITUNER_FINAL_CORING_INDEX_SCALE,
        ITUNER_FINAL_MIN_CORING_RESULT,
        ITUNER_FINAL_SCALE_CORING,
        ITUNER_CONTRAST_ENHANCE,
        ITUNER_STATIC_BLACK_LEVEL_EXP0,
        ITUNER_STATIC_BLACK_LEVEL_EXP1,
        ITUNER_FRONT_END_WB_GAIN_EXP0,
        ITUNER_FRONT_END_WB_GAIN_EXP1,
        ITUNER_VHDR_BLEND,
    };

    const UINT8 hdr_3x_md_filter[] = {
        ITUNER_COLOR_CORRECTION,
        ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION,
        ITUNER_CFA_LEAKAGE_FILTER,
        ITUNER_CFA_NOISE_FILTER,
        ITUNER_ANTI_ALIASING_STRENGTH,
        ITUNER_BEFORE_CE_WB_GAIN,
        ITUNER_AFTER_CE_WB_GAIN,
        ITUNER_TONE_CURVE,
        ITUNER_RGB_TO_YUV_MATRIX,
        ITUNER_CHROMA_SCALE,
        ITUNER_CHROMA_MEDIAN_FILTER,
        ITUNER_DEMOSAIC_FILTER,
        ITUNER_GB_GR_MISMATCH,
        ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT,
        ITUNER_CHROMA_FILTER,
        ITUNER_VIDEO_MCTF,
        ITUNER_VIDEO_MCTF_LEVEL,
        ITUNER_FINAL_SHARPEN_BOTH,
        ITUNER_FINAL_SHARPEN_NOISE,
        ITUNER_FINAL_FIR,
        ITUNER_FINAL_CORING,
        ITUNER_FINAL_CORING_INDEX_SCALE,
        ITUNER_FINAL_MIN_CORING_RESULT,
        ITUNER_FINAL_SCALE_CORING,
        ITUNER_CONTRAST_ENHANCE,
        ITUNER_STATIC_BLACK_LEVEL_EXP0,
        ITUNER_STATIC_BLACK_LEVEL_EXP1,
        ITUNER_STATIC_BLACK_LEVEL_EXP2,
        ITUNER_FRONT_END_WB_GAIN_EXP0,
        ITUNER_FRONT_END_WB_GAIN_EXP1,
        ITUNER_FRONT_END_WB_GAIN_EXP2,
        ITUNER_VHDR_BLEND,
    };
    const UINT8 liso_y2y_filter[] = {
        ITUNER_CHROMA_SCALE,
        ITUNER_CHROMA_MEDIAN_FILTER,
        ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT,
        ITUNER_CHROMA_FILTER,
        ITUNER_VIDEO_MCTF,
        ITUNER_VIDEO_MCTF_LEVEL,
        ITUNER_VIDEO_MCTF_AND_FINAL_SHARPEN,
        ITUNER_FINAL_SHARPEN_BOTH,
        ITUNER_FINAL_SHARPEN_NOISE,
        ITUNER_FINAL_FIR,
        ITUNER_FINAL_CORING,
        ITUNER_FINAL_CORING_INDEX_SCALE,
        ITUNER_FINAL_MIN_CORING_RESULT,
        ITUNER_FINAL_SCALE_CORING,
    };
    const UINT8 liso_y2y_mipi_filter[] = {
        ITUNER_VIDEO_MCTF,
        ITUNER_VIDEO_MCTF_LEVEL,
        ITUNER_VIDEO_MCTF_AND_FINAL_SHARPEN,
        ITUNER_FINAL_SHARPEN_BOTH,
        ITUNER_FINAL_SHARPEN_NOISE,
        ITUNER_FINAL_FIR,
        ITUNER_FINAL_CORING,
        ITUNER_FINAL_CORING_INDEX_SCALE,
        ITUNER_FINAL_MIN_CORING_RESULT,
        ITUNER_FINAL_SCALE_CORING,
    };
    const UINT8 hiso_filter[] = {
        ITUNER_STATIC_BLACK_LEVEL_EXP0,
        ITUNER_FRONT_END_WB_GAIN_EXP0,
        ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION,
        ITUNER_CFA_LEAKAGE_FILTER,
        ITUNER_ANTI_ALIASING_STRENGTH,
        ITUNER_GB_GR_MISMATCH,
        ITUNER_BEFORE_CE_WB_GAIN,
        ITUNER_CONTRAST_ENHANCE,
        ITUNER_AFTER_CE_WB_GAIN,
        ITUNER_CFA_NOISE_FILTER,
        ITUNER_DEMOSAIC_FILTER,
        ITUNER_COLOR_CORRECTION,
        ITUNER_TONE_CURVE,
        ITUNER_RGB_TO_YUV_MATRIX,
        ITUNER_CHROMA_SCALE,
        ITUNER_CHROMA_FILTER,
        ITUNER_CHROMA_MEDIAN_FILTER,
        ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT,
        ITUNER_ASF_INFO,
        ITUNER_SYSTEM_INFO,
        ITUNER_LI_CHROMA_ASF,
        ITUNER_HI_CHROMA_ASF,
        ITUNER_HI_LUMA_ASF,
        ITUNER_HI_CFA_NOISE_FILTER,
        ITUNER_HI_CFA_LEAKAGE_FILTER,
        ITUNER_HI_ANTI_ALIASING,
        ITUNER_HI_AUTO_BAD_PIXEL_CORRECTION,
        ITUNER_HI_CHROMA_FLTR_HIGH,
        ITUNER_HI_CHROMA_FLTR_LOW,
        ITUNER_HI_CHROMA_FLTR_LOW_COMBINE,
        ITUNER_HI_CHROMA_FLTR_MEDIAN,
        ITUNER_HI_CHROMA_FLTR_MEDIAN_COMBINE,
        ITUNER_HI_CHROMA_FLTR_PRE,
        ITUNER_HI_CHROMA_FLTR_VERY_LOW,
        ITUNER_HI_CHROMA_FLTR_VERY_LOW_COMBINE,
        ITUNER_HI_CHROMA_MEDIAN_FILTER,
        ITUNRE_HI_DEMOSAIC_FILTER,
        ITUNER_HI_GB_GR_MISMATCH,
        ITUNER_HI_HIGH2_LUMA_ASF,
        ITUNER_HI_HIGH_LUMA_ASF,
        ITUNER_HI_HIGH_SHARPEN_BOTH,
        ITUNER_HI_HIGH_SHARPEN_NOISE,
        ITUNER_HI_HIGH_FIR,
        ITUNER_HI_HIGH_CORING,
        ITUNER_HI_HIGH_CORING_INDEX_SCALE,
        ITUNER_HI_HIGH_MAX_CORING_RESULT,
        ITUNER_HI_HIGH_MIN_CORING_RESULT,
        ITUNER_HI_HIGH_SCALE_CORING,
        ITUNER_HI_LOW_LUMA_ASF,
        ITUNER_HI_LOW_LUMA_ASF_COMBINE,
        ITUNER_HI_LUMA_NOISE_COMBINE,
        ITUNER_HI_MED1_LUMA_ASF,
        ITUNER_HI_MED2_LUMA_ASF,
        ITUNER_HI_MED_SHARPEN_BOTH,
        ITUNER_HI_MED_SHARPEN_NOISE,
        ITUNER_HI_MED_FIR,
        ITUNER_HI_MED_CORING,
        ITUNER_HI_MED_CORING_INDEX_SCALE,
        ITUNER_HI_MED_MAX_CORING_RESULT,
        ITUNER_HI_MED_MIN_CORING_RESULT,
        ITUNER_HI_MED_SCALE_CORING,
        ITUNER_HI_SELECT,
        ITUNER_HI_LUMA_BLEND,
        ITUNER_HILI_COMBINE,
        ITUNER_HI_MED_HIGH_FREQ_RECOVER,
        ITUNER_HILI_SHARPEN_BOTH,
        ITUNER_HILi_SHARPEN_NOISE,
        ITUNER_HILI_FIR,
        ITUNER_HILI_CORING,
        ITUNER_HILI_CORING_INDEX_SCALE,
        ITUNER_HILI_MAX_CORING_RESULT,
        ITUNER_HILI_MIN_CORING_RESULT,
        ITUNER_HILI_SCALE_CORING,
        ITUNER_LI2_LUMA_ASF,
        ITUNER_LI2_ANTI_ALIASING,
        ITUNER_LI2_AUTO_BAD_PIXEL_CORRECTION,
        ITUNER_LI2_CFA_LEAKAGE_FILTER,
        ITUNER_LI2_CFA_NOISE_FILTER,
        ITUNRE_LI2_DEMOSAIC_FILTER,
        ITUNER_LI2_GB_GR_MISMATCH,
        ITUNER_LI2_SHARPEN_BOTH,
        ITUNER_LI2_SHARPEN_NOISE,
        ITUNER_LI2_FIR,
        ITUNER_LI2_CORING,
        ITUNER_LI2_CORING_INDEX_SCALE,
        ITUNER_LI2_MAX_CORING_RESULT,
        ITUNER_LI2_MIN_CORING_RESULT,
        ITUNER_LI2_SCALE_CORING,
        ITUNER_HI_NONSMOOTH_DETECT,
    };
    INT32 rval = 0;
    UINT8 i = 0;
    const UINT8 *necessary_filter = NULL;
    UINT8 size = 0;

    if (Ability->Pipe== AMBA_IK_PIPE_VIDEO) {
        if (Ability->VideoPipe == AMBA_IK_VIDEO_LINEAR) {
            necessary_filter = liso_filter;
            size = (UINT8)(sizeof(liso_filter) / sizeof((liso_filter)[0]));
        } else if (Ability->VideoPipe== AMBA_IK_VIDEO_LINEAR_CE) {
            necessary_filter = linear_ce_filter;
            size = (UINT8)(sizeof(linear_ce_filter) / sizeof((linear_ce_filter)[0]));
        } else if (Ability->VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_2) {
            necessary_filter = hdr_2x_filter;
            size = (UINT8)(sizeof(hdr_2x_filter) / sizeof((hdr_2x_filter)[0]));
        } else if (Ability->VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_3) {
            necessary_filter = hdr_3x_filter;
            size = (UINT8)(sizeof(hdr_3x_filter) / sizeof((hdr_3x_filter)[0]));
        } else if (Ability->VideoPipe == AMBA_IK_VIDEO_LINEAR_MD) {
            necessary_filter = liso_md_filter;
            size = (UINT8)(sizeof(liso_md_filter) / sizeof((liso_md_filter)[0]));
        } else if (Ability->VideoPipe == AMBA_IK_VIDEO_LINEAR_CE_MD) {
            necessary_filter = linear_ce_md_filter;
            size = (UINT8)(sizeof(linear_ce_md_filter) / sizeof((linear_ce_md_filter)[0]));
        } else if (Ability->VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_2_MD) {
            necessary_filter = hdr_2x_md_filter;
            size = (UINT8)(sizeof(hdr_2x_md_filter) / sizeof((hdr_2x_md_filter)[0]));
        } else if (Ability->VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_3_MD) {
            necessary_filter = hdr_3x_md_filter;
            size = (UINT8)(sizeof(hdr_3x_md_filter) / sizeof((hdr_3x_md_filter)[0]));
        } else if (Ability->VideoPipe == AMBA_IK_VIDEO_Y2Y) {
            necessary_filter = liso_y2y_filter;
            size = (UINT8)(sizeof(liso_y2y_filter) / sizeof((liso_y2y_filter)[0]));
        } else if (Ability->VideoPipe == AMBA_IK_VIDEO_Y2Y_MIPI) {
            necessary_filter = liso_y2y_mipi_filter;
            size = (UINT8)(sizeof(liso_y2y_mipi_filter) / sizeof((liso_y2y_mipi_filter)[0]));
        } else {
            ituner_print_uint32_5("Not support video_pipe: %d", Ability->VideoPipe, DC_U, DC_U, DC_U, DC_U);
            rval = -1;
        }
        if (necessary_filter != NULL) {
            for (i = 0; i < size; i++) {
                if (Ituner_Valid_Filter[(UINT8)necessary_filter[i]] == 0U) {
                    ituner_print_uint32_5("video pipe: %d %d", Ability->VideoPipe, DC_U, DC_U, DC_U, DC_U);
                    ituner_print_str_5(" Lose Filter %s ", AmbaItuner_Get_Filter_Name(necessary_filter[i]), DC_S, DC_S, DC_S, DC_S);
                    rval = -1;
                }
            }
        }
        // FIXME
        if ((necessary_filter != NULL) && (i < size)) {
            (void)AmbaItuner_Get_Filter_Name(necessary_filter[i]);
        }
    } else if (Ability->Pipe== AMBA_IK_PIPE_STILL) {
        size = (UINT8)(sizeof(hiso_filter) / sizeof((hiso_filter)[0]));
        rval = AmbaItuner_Check_Filter_Still(Ability, hiso_filter, size);
    } else {
        rval = -1;
    }
    return rval;
}

static inline void AmbaItuner_GData_Init(void)
{
    (void)ituner_memset(Ituner_Valid_Filter, 0x0, sizeof(Ituner_Valid_Filter));
    (void)ituner_memset(&GData, 0x0, sizeof(ITUNER_DATA_s));
}

static inline void AmbaItuner_System_Init(void)
{
    (void)ituner_memset(&SystemInfo, 0x0, sizeof(SystemInfo));
    ituner_strncpy(&SystemInfo.TuningMode[0], "IMG_MODE_PREVIEW", (INT32)sizeof(SystemInfo.TuningMode));
    SystemInfo.ItunerRev = ITN_VERSION_MAJOR;
    SystemInfo.HSubSampleFactorNum = 1UL;
    SystemInfo.HSubSampleFactorDen = 1UL;
    SystemInfo.VSubSampleFactorNum = 1UL;
    SystemInfo.VSubSampleFactorDen = 1UL;
    SystemInfo.RawResolution = 16UL;
    SystemInfo.NumberOfFrames = 1UL;
    SystemInfo.FrameNumber = 1UL;
}

static inline void AmbaItuner_Internal_Init(void)
{
    (void)ituner_memset(&InternalInfo, 0x0, sizeof(InternalInfo));
    InternalInfo.sbp_highlight = -1;
}

void AmbaItuner_Set_InternalInfo(const Ituner_Internal_s *Internal)
{
    (void)ituner_memcpy(&InternalInfo, Internal, sizeof(InternalInfo));
    AmbaItuner_Set_Filter_Valid(ITUNER_INTERNAL_INFO);
}

void AmbaItuner_Get_InternalInfo(Ituner_Internal_s *Internal)
{
    (void) ituner_memcpy(Internal, &InternalInfo, sizeof(Ituner_Internal_s));
}

void AmbaItuner_Set_SystemInfo(const AMBA_ITN_SYSTEM_s *System)
{
    (void)ituner_memcpy(&SystemInfo, System, sizeof(SystemInfo));
    AmbaItuner_Set_Filter_Valid(ITUNER_SYSTEM_INFO);
}

void AmbaItuner_Get_SystemInfo(AMBA_ITN_SYSTEM_s *System)
{
    (void)ituner_memcpy(System, &SystemInfo, sizeof(AMBA_ITN_SYSTEM_s));
}

void AmbaItuner_Set_AAAFunction(const AAA_FUNC_s *AaaFunction)
{
    (void)ituner_memcpy(&GData.AaaFunction, AaaFunction, sizeof(GData.AaaFunction));
    AmbaItuner_Set_Filter_Valid(ITUNER_AAA_FUNCTION);
}

void AmbaItuner_Get_AAAFunction(AAA_FUNC_s *AaaFunction)
{
    (void)ituner_memcpy(AaaFunction, &GData.AaaFunction, sizeof(AAA_FUNC_s));
}

void AmbaItuner_Set_AeInfo(const ITUNER_AE_INFO_s *AeInfo)
{
    (void)ituner_memcpy(&GData.AeInfo, AeInfo, sizeof(GData.AeInfo));
    AmbaItuner_Set_Filter_Valid(ITUNER_AE_INFO);
}

void AmbaItuner_Get_AeInfo(ITUNER_AE_INFO_s *AeInfo)
{
    (void)ituner_memcpy(AeInfo, &GData.AeInfo, sizeof(ITUNER_AE_INFO_s));
}

void AmbaItuner_Set_WbSimInfo(const ITUNER_WB_SIM_INFO_s *WbSimInfo)
{
    (void)ituner_memcpy(&GData.WbSimInfo, WbSimInfo, sizeof(GData.WbSimInfo));
    AmbaItuner_Set_Filter_Valid(ITUNER_WB_SIM_INFO);
}

void AmbaItuner_Get_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo)
{
    (void)ituner_memcpy(WbSimInfo, &GData.WbSimInfo, sizeof(ITUNER_WB_SIM_INFO_s));
}

void AmbaItuner_Set_StaticBadPixelCorrection(const ITUNER_FPN_s *StaticBadPixelCorrection)
{
    (void)ituner_memcpy(&GData.StaticBadPixelCorrection, StaticBadPixelCorrection, sizeof(GData.StaticBadPixelCorrection));
    AmbaItuner_Set_Filter_Valid(ITUNER_STATIC_BAD_PIXEL_CORRECTION);
}

void AmbaItuner_Get_StaticBadPixelCorrection(ITUNER_FPN_s *StaticBadPixelCorrection)
{
    (void)ituner_memcpy(StaticBadPixelCorrection, &GData.StaticBadPixelCorrection, sizeof(ITUNER_FPN_s));
}

void AmbaItuner_Set_SBP_CorrectionInternal(const ITUNER_FPN_INTERNAL_s *StaticBadPixelCorrectionInternal)
{
    (void)ituner_memcpy(&GData.StaticBadPixelCorrectionInternal, StaticBadPixelCorrectionInternal, sizeof(GData.StaticBadPixelCorrectionInternal));
    AmbaItuner_Set_Filter_Valid(ITUNER_STATIC_BAD_PIXEL_INTERNAL);
}

void AmbaItuner_Get_SBP_CorrectionInternal(ITUNER_FPN_INTERNAL_s *StaticBadPixelCorrectionInternal)
{
    (void)ituner_memcpy(StaticBadPixelCorrectionInternal, &GData.StaticBadPixelCorrectionInternal, sizeof(ITUNER_FPN_INTERNAL_s));
}

void AmbaItuner_GetSbpInfoRetrive(AMBA_IK_STATIC_BAD_PXL_COR_s *SbpCorrection)
{
    (void)ituner_memcpy(SbpCorrection, &GData.SbpCorr, sizeof(AMBA_IK_STATIC_BAD_PXL_COR_s));
}

void AmbaItuner_Set_SensorInputInfo(const ITUNER_SENSOR_INPUT_s *SensorInputInfo)
{
    (void)ituner_memcpy(&GData.SensorInputInfo, SensorInputInfo, sizeof(GData.SensorInputInfo));
    AmbaItuner_Set_Filter_Valid(ITUNER_SENSOR_INPUT_INFO);
}

void AmbaItuner_Get_SensorInputInfo(ITUNER_SENSOR_INPUT_s *SensorInputInfo)
{
    (void)ituner_memcpy(SensorInputInfo, &GData.SensorInputInfo, sizeof(ITUNER_SENSOR_INPUT_s));
}
#if 0
void AmbaItuner_Set_DeferredBlackLevel(const AMBA_IK_DEFERRED_BLC_s *DeferredBlackLevel)
{
    (void)ituner_memcpy(&GData.DeferredBlackLevel, DeferredBlackLevel, sizeof(GData.DeferredBlackLevel));
    AmbaItuner_Set_Filter_Valid(ITUNER_DEFERRED_BLACK_LEVEL);
}

void AmbaItuner_Get_DeferredBlackLevel(AMBA_IK_DEFERRED_BLC_s *DeferredBlackLevel)
{
    (void)ituner_memcpy(DeferredBlackLevel, &GData.DeferredBlackLevel, sizeof(AMBA_IK_DEFERRED_BLC_s));
}
#endif
void AmbaItuner_Set_DynamicBadPixelCorrection(const AMBA_IK_DYNAMIC_BAD_PXL_COR_s *DynamicBadPixelCorrection)
{
    (void)ituner_memcpy(&GData.DynamicBadPixelCorrection, DynamicBadPixelCorrection, sizeof(GData.DynamicBadPixelCorrection));
    AmbaItuner_Set_Filter_Valid(ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION);
}

void AmbaItuner_Get_DynamicBadPixelCorrection(AMBA_IK_DYNAMIC_BAD_PXL_COR_s *DynamicBadPixelCorrection)
{
    (void)ituner_memcpy(DynamicBadPixelCorrection, &GData.DynamicBadPixelCorrection, sizeof(AMBA_IK_DYNAMIC_BAD_PXL_COR_s));
}

void AmbaItuner_Set_CfaLeakageFilter(const AMBA_IK_CFA_LEAKAGE_FILTER_s *CfaLeakageFilter)
{
    (void)ituner_memcpy(&GData.CfaLeakageFilter, CfaLeakageFilter, sizeof(GData.CfaLeakageFilter));
    AmbaItuner_Set_Filter_Valid(ITUNER_CFA_LEAKAGE_FILTER);
}

void AmbaItuner_Get_CfaLeakageFilter(AMBA_IK_CFA_LEAKAGE_FILTER_s *CfaLeakageFilter)
{
    (void)ituner_memcpy(CfaLeakageFilter, &GData.CfaLeakageFilter, sizeof(AMBA_IK_CFA_LEAKAGE_FILTER_s));
}

void AmbaItuner_Set_CfaNoiseFilter(const AMBA_IK_CFA_NOISE_FILTER_s *CfaNoiseFilter)
{
    (void)ituner_memcpy(&GData.CfaNoiseFilter, CfaNoiseFilter, sizeof(GData.CfaNoiseFilter));
    AmbaItuner_Set_Filter_Valid(ITUNER_CFA_NOISE_FILTER);
}

void AmbaItuner_Get_CfaNoiseFilter(AMBA_IK_CFA_NOISE_FILTER_s *CfaNoiseFilter)
{
    (void)ituner_memcpy(CfaNoiseFilter, &GData.CfaNoiseFilter, sizeof(AMBA_IK_CFA_NOISE_FILTER_s));
}

void AmbaItuner_Set_AntiAliasing(const AMBA_IK_ANTI_ALIASING_s *AntiAliasing)
{
    (void)ituner_memcpy(&GData.AntiAliasing, AntiAliasing, sizeof(AMBA_IK_ANTI_ALIASING_s));
    AmbaItuner_Set_Filter_Valid(ITUNER_ANTI_ALIASING_STRENGTH);
}

void AmbaItuner_Get_AntiAliasing(AMBA_IK_ANTI_ALIASING_s *AntiAliasing)
{
    (void)ituner_memcpy(AntiAliasing, &GData.AntiAliasing, sizeof(AMBA_IK_ANTI_ALIASING_s));
}

void AmbaItuner_Set_BeforeCeWbGain(const AMBA_IK_WB_GAIN_s *BeforeCeWbGain)
{
    (void)ituner_memcpy(&GData.BeforeCeWbGain, BeforeCeWbGain, sizeof(GData.BeforeCeWbGain));
    AmbaItuner_Set_Filter_Valid(ITUNER_BEFORE_CE_WB_GAIN);
}

void AmbaItuner_Get_BeforeCeWbGain(AMBA_IK_WB_GAIN_s *BeforeCeWbGain)
{
    (void)ituner_memcpy(BeforeCeWbGain, &GData.BeforeCeWbGain, sizeof(AMBA_IK_WB_GAIN_s));
}

void AmbaItuner_Set_AfterCeWbGain(const AMBA_IK_WB_GAIN_s *AfterCeWbGain)
{
    (void)ituner_memcpy(&GData.AfterCeWbGain, AfterCeWbGain, sizeof(GData.AfterCeWbGain));
    AmbaItuner_Set_Filter_Valid(ITUNER_AFTER_CE_WB_GAIN);
}

void AmbaItuner_Get_AfterCeWbGain(AMBA_IK_WB_GAIN_s *AfterCeWbGain)
{
    (void)ituner_memcpy(AfterCeWbGain, &GData.AfterCeWbGain, sizeof(AMBA_IK_WB_GAIN_s));
}

void AmbaItuner_Get_FilterStatus(AMBA_ITUNER_VALID_FILTER_t *FilterStatus)
{
    // Note: Only Get, without Set Api
    (void)ituner_memcpy(FilterStatus, &Ituner_Valid_Filter[0], sizeof(AMBA_ITUNER_VALID_FILTER_t));
}

void AmbaItuner_Set_PreCcGain(const ITUNER_PRE_CC_GAIN_s *PreCcGain)
{
    (void)ituner_memcpy(&GData.PreCcGain, PreCcGain, sizeof(GData.PreCcGain));
    AmbaItuner_Set_Filter_Valid(ITUNER_PRE_CC_GAIN);
}

void AmbaItuner_Get_PreCcGain(ITUNER_PRE_CC_GAIN_s *PreCcGain)
{
    (void)ituner_memcpy(PreCcGain, &GData.PreCcGain, sizeof(ITUNER_PRE_CC_GAIN_s));
}

void AmbaItuner_Get_CcReg(AMBA_IK_COLOR_CORRECTION_REG_s **pCcReg)
{
    // Note: Only Get, without Set Api
    //(void)ituner_memcpy(CcReg, &GData.CcReg, sizeof(AMBA_IK_COLOR_CORRECTION_REG_s));
    *pCcReg = &GData.CcReg;
}

void AmbaItuner_Get_CcThreeD(AMBA_IK_COLOR_CORRECTION_s **pCcThreeD)
{
    // Note: Only Get, without Set Api
    //(void)ituner_memcpy(CcThreeD, &GData.CcThreeD, sizeof(AMBA_IK_COLOR_CORRECTION_s));
    *pCcThreeD = &GData.CcThreeD;
}

void AmbaItuner_Set_ColorCorrection(const ITUNER_COLOR_CORRECTION_s *ColorCorrection)
{
    (void)ituner_memcpy(&GData.ColorCorrection, ColorCorrection, sizeof(GData.ColorCorrection));
    AmbaItuner_Set_Filter_Valid(ITUNER_COLOR_CORRECTION);
}

void AmbaItuner_Get_ColorCorrection(ITUNER_COLOR_CORRECTION_s *ColorCorrection)
{
    (void)ituner_memcpy(ColorCorrection, &GData.ColorCorrection, sizeof(ITUNER_COLOR_CORRECTION_s));
}

void AmbaItuner_Set_ToneCurve(const AMBA_IK_TONE_CURVE_s *ToneCurve)
{
    (void)ituner_memcpy(&GData.ToneCurve, ToneCurve, sizeof(GData.ToneCurve));
    AmbaItuner_Set_Filter_Valid(ITUNER_TONE_CURVE);
}

void AmbaItuner_Get_ToneCurve(AMBA_IK_TONE_CURVE_s *ToneCurve)
{
    (void)ituner_memcpy(ToneCurve, &GData.ToneCurve, sizeof(AMBA_IK_TONE_CURVE_s));
}

void AmbaItuner_Set_FeToneCurve(const ITUNER_FE_TONE_CURVE_s *FeToneCurve)
{
    (void)ituner_memcpy(&GData.FeToneCurve, FeToneCurve, sizeof(GData.FeToneCurve));
    AmbaItuner_Set_Filter_Valid(ITUNER_FRONT_END_TONE_CURVE);
}

void AmbaItuner_Get_FeToneCurve(ITUNER_FE_TONE_CURVE_s *FeToneCurve)
{
    (void)ituner_memcpy(FeToneCurve, &GData.FeToneCurve, sizeof(ITUNER_FE_TONE_CURVE_s));
}

void AmbaItuner_Set_RgbToYuvMatrix(const AMBA_IK_RGB_TO_YUV_MATRIX_s *RgbToYuvMatrix)
{
    (void)ituner_memcpy(&GData.RgbToYuvMatrix, RgbToYuvMatrix, sizeof(GData.RgbToYuvMatrix));
    AmbaItuner_Set_Filter_Valid(ITUNER_RGB_TO_YUV_MATRIX);
}

void AmbaItuner_Get_RgbToYuvMatrix(AMBA_IK_RGB_TO_YUV_MATRIX_s *RgbToYuvMatrix)
{
    (void)ituner_memcpy(RgbToYuvMatrix, &GData.RgbToYuvMatrix, sizeof(AMBA_IK_RGB_TO_YUV_MATRIX_s));
}

void AmbaItuner_Set_RgbIr(const AMBA_IK_RGB_IR_s *RgbIr)
{
    (void)ituner_memcpy(&GData.RgbIr, RgbIr, sizeof(GData.RgbIr));
    AmbaItuner_Set_Filter_Valid(ITUNER_RGB_IR);
}

void AmbaItuner_Get_RgbIr(AMBA_IK_RGB_IR_s *RgbIr)
{
    (void)ituner_memcpy(RgbIr, &GData.RgbIr, sizeof(AMBA_IK_RGB_IR_s));
}

void AmbaItuner_Set_ChromaScale(const AMBA_IK_CHROMA_SCALE_s *ChromaScale)
{
    (void)ituner_memcpy(&GData.ChromaScale, ChromaScale, sizeof(GData.ChromaScale));
    AmbaItuner_Set_Filter_Valid(ITUNER_CHROMA_SCALE);
}

void AmbaItuner_Get_ChromaScale(AMBA_IK_CHROMA_SCALE_s *ChromaScale)
{
    (void)ituner_memcpy(ChromaScale, &GData.ChromaScale, sizeof(AMBA_IK_CHROMA_SCALE_s));
}

void AmbaItuner_Set_ChromaMedianFilter(const AMBA_IK_CHROMA_MEDIAN_FILTER_s *ChromaMedianFilter)
{
    (void)ituner_memcpy(&GData.ChromaMedianFilter, ChromaMedianFilter, sizeof(GData.ChromaMedianFilter));
    AmbaItuner_Set_Filter_Valid(ITUNER_CHROMA_MEDIAN_FILTER);
}

void AmbaItuner_Get_ChromaMedianFilter(AMBA_IK_CHROMA_MEDIAN_FILTER_s *ChromaMedianFilter)
{
    (void)ituner_memcpy(ChromaMedianFilter, &GData.ChromaMedianFilter, sizeof(AMBA_IK_CHROMA_MEDIAN_FILTER_s));
}

void AmbaItuner_Set_DemosaicFilter(const AMBA_IK_DEMOSAIC_s *DemosaicFilter)
{
    (void)ituner_memcpy(&GData.DemosaicFilter, DemosaicFilter, sizeof(AMBA_IK_DEMOSAIC_s));
    AmbaItuner_Set_Filter_Valid(ITUNER_DEMOSAIC_FILTER);
}

void AmbaItuner_Get_DemosaicFilter(AMBA_IK_DEMOSAIC_s *DemosaicFilter)
{
    (void)ituner_memcpy(DemosaicFilter, &GData.DemosaicFilter, sizeof(GData.DemosaicFilter));
}

void AmbaItuner_Set_SharpenBoth(const ITUNER_SHARPEN_BOTH_s *SharpenBoth)
{
    (void)ituner_memcpy(&GData.SharpenBoth, SharpenBoth, sizeof(GData.SharpenBoth));
    AmbaItuner_Set_Filter_Valid(ITUNER_SHARPEN_BOTH);
}

void AmbaItuner_Get_SharpenBoth(ITUNER_SHARPEN_BOTH_s *SharpenBoth)
{
    (void)ituner_memcpy(SharpenBoth, &GData.SharpenBoth, sizeof(ITUNER_SHARPEN_BOTH_s));
}

void AmbaItuner_Set_FinalSharpenBoth(const ITUNER_FINAL_SHARPEN_BOTH_s *FinalSharpenBoth)
{
    (void)ituner_memcpy(&GData.FinalSharpenBoth, FinalSharpenBoth, sizeof(GData.FinalSharpenBoth));
    AmbaItuner_Set_Filter_Valid(ITUNER_FINAL_SHARPEN_BOTH);
}

void AmbaItuner_Get_FinalSharpenBoth(ITUNER_FINAL_SHARPEN_BOTH_s *FinalSharpenBoth)
{
    (void)ituner_memcpy(FinalSharpenBoth, &GData.FinalSharpenBoth, sizeof(ITUNER_FINAL_SHARPEN_BOTH_s));
}

void AmbaItuner_Set_SharpenNoise(const AMBA_IK_FSTSHPNS_NOISE_s *SharpenNoise)
{
    (void)ituner_memcpy(&GData.SharpenNoise, SharpenNoise, sizeof(GData.SharpenNoise));
    AmbaItuner_Set_Filter_Valid(ITUNER_SHARPEN_NOISE);
}

void AmbaItuner_Get_SharpenNoise(AMBA_IK_FSTSHPNS_NOISE_s *SharpenNoise)
{
    (void)ituner_memcpy(SharpenNoise, &GData.SharpenNoise, sizeof(AMBA_IK_FSTSHPNS_NOISE_s));
}

void AmbaItuner_Set_FinalSharpenNoise(const AMBA_IK_FNLSHPNS_NOISE_s *FinalSharpenNoise)
{
    (void)ituner_memcpy(&GData.FinalSharpenNoise, FinalSharpenNoise, sizeof(GData.FinalSharpenNoise));
    AmbaItuner_Set_Filter_Valid(ITUNER_FINAL_SHARPEN_NOISE);
}

void AmbaItuner_Get_FinalSharpenNoise(AMBA_IK_FNLSHPNS_NOISE_s *FinalSharpenNoise)
{
    (void)ituner_memcpy(FinalSharpenNoise, &GData.FinalSharpenNoise, sizeof(AMBA_IK_FNLSHPNS_NOISE_s));
}

void AmbaItuner_Set_Fir(const AMBA_IK_FSTSHPNS_FIR_s *Fir)
{
    (void)ituner_memcpy(&GData.Fir, Fir, sizeof(GData.Fir));
    AmbaItuner_Set_Filter_Valid(ITUNER_FIR);
}

void AmbaItuner_Get_Fir(AMBA_IK_FSTSHPNS_FIR_s *Fir)
{
    (void)ituner_memcpy(Fir, &GData.Fir, sizeof(AMBA_IK_FSTSHPNS_FIR_s));
}

void AmbaItuner_Set_FinalFir(const AMBA_IK_FNLSHPNS_FIR_s *FinalFir)
{
    (void)ituner_memcpy(&GData.FinalFir, FinalFir, sizeof(GData.FinalFir));
    AmbaItuner_Set_Filter_Valid(ITUNER_FINAL_FIR);
}

void AmbaItuner_Get_FinalFir(AMBA_IK_FNLSHPNS_FIR_s *FinalFir)
{
    (void)ituner_memcpy(FinalFir, &GData.FinalFir, sizeof(AMBA_IK_FNLSHPNS_FIR_s));
}

void AmbaItuner_Set_Coring(const AMBA_IK_FSTSHPNS_CORING_s *Coring)
{
    (void)ituner_memcpy(&GData.Coring, Coring, sizeof(GData.Coring));
    AmbaItuner_Set_Filter_Valid(ITUNER_CORING);
}

void AmbaItuner_Get_Coring(AMBA_IK_FSTSHPNS_CORING_s *Coring)
{
    (void)ituner_memcpy(Coring, &GData.Coring, sizeof(AMBA_IK_FSTSHPNS_CORING_s));
}

void AmbaItuner_Set_FinalCoring(const AMBA_IK_FNLSHPNS_CORING_s *FinalCoring)
{
    (void)ituner_memcpy(&GData.FinalCoring, FinalCoring, sizeof(GData.FinalCoring));
    AmbaItuner_Set_Filter_Valid(ITUNER_FINAL_CORING);
}

void AmbaItuner_Get_FinalCoring(AMBA_IK_FNLSHPNS_CORING_s *FinalCoring)
{
    (void)ituner_memcpy(FinalCoring, &GData.FinalCoring, sizeof(AMBA_IK_FNLSHPNS_CORING_s));
}

void AmbaItuner_Set_CoringIndexScale(const AMBA_IK_FSTSHPNS_COR_IDX_SCL_s *CoringIndexScale)
{
    (void)ituner_memcpy(&GData.CoringIndexScale, CoringIndexScale, sizeof(GData.CoringIndexScale));
    AmbaItuner_Set_Filter_Valid(ITUNER_CORING_INDEX_SCALE);
}

void AmbaItuner_Get_CoringIndexScale(AMBA_IK_FSTSHPNS_COR_IDX_SCL_s *CoringIndexScale)
{
    (void)ituner_memcpy(CoringIndexScale, &GData.CoringIndexScale, sizeof(AMBA_IK_FSTSHPNS_COR_IDX_SCL_s));
}

void AmbaItuner_Set_MinCoringResult(const AMBA_IK_FSTSHPNS_MIN_COR_RST_s *MinCoringResult)
{
    (void)ituner_memcpy(&GData.MinCoringResult, MinCoringResult, sizeof(GData.MinCoringResult));
    AmbaItuner_Set_Filter_Valid(ITUNER_MIN_CORING_RESULT);
}

void AmbaItuner_Get_MinCoringResult(AMBA_IK_FSTSHPNS_MIN_COR_RST_s *MinCoringResult)
{
    (void)ituner_memcpy(MinCoringResult, &GData.MinCoringResult, sizeof(AMBA_IK_FSTSHPNS_MIN_COR_RST_s));
}

void AmbaItuner_Set_MaxCoringResult(const AMBA_IK_FSTSHPNS_MAX_COR_RST_s *MaxCoringResult)
{
    (void)ituner_memcpy(&GData.MaxCoringResult, MaxCoringResult, sizeof(GData.MaxCoringResult));
    AmbaItuner_Set_Filter_Valid(ITUNER_MAX_CORING_RESULT);
}

void AmbaItuner_Get_MaxCoringResult(AMBA_IK_FSTSHPNS_MAX_COR_RST_s *MaxCoringResult)
{
    (void)ituner_memcpy(MaxCoringResult, &GData.MaxCoringResult, sizeof(AMBA_IK_FSTSHPNS_MAX_COR_RST_s));
}

void AmbaItuner_Set_ScaleCoring(const AMBA_IK_FSTSHPNS_SCL_COR_s *ScaleCoring)
{
    (void)ituner_memcpy(&GData.ScaleCoring, ScaleCoring, sizeof(GData.ScaleCoring));
    AmbaItuner_Set_Filter_Valid(ITUNER_SCALE_CORING);
}

void AmbaItuner_Get_ScaleCoring(AMBA_IK_FSTSHPNS_SCL_COR_s *ScaleCoring)
{
    (void)ituner_memcpy(ScaleCoring, &GData.ScaleCoring, sizeof(AMBA_IK_FSTSHPNS_SCL_COR_s));
}

void AmbaItuner_Set_FinalCoringIndexScale(const AMBA_IK_FNLSHPNS_COR_IDX_SCL_s *FinalCoringIndexScale)
{
    (void)ituner_memcpy(&GData.FinalCoringIndexScale, FinalCoringIndexScale, sizeof(GData.FinalCoringIndexScale));
    AmbaItuner_Set_Filter_Valid(ITUNER_FINAL_CORING_INDEX_SCALE);
}

void AmbaItuner_Get_FinalCoringIndexScale(AMBA_IK_FNLSHPNS_COR_IDX_SCL_s *FinalCoringIndexScale)
{
    (void)ituner_memcpy(FinalCoringIndexScale, &GData.FinalCoringIndexScale, sizeof(AMBA_IK_FNLSHPNS_COR_IDX_SCL_s));
}

void AmbaItuner_Set_FinalMinCoringResult(const AMBA_IK_FNLSHPNS_MIN_COR_RST_s *FinalMinCoringResult)
{
    (void)ituner_memcpy(&GData.FinalMinCoringResult, FinalMinCoringResult, sizeof(GData.FinalMinCoringResult));
    AmbaItuner_Set_Filter_Valid(ITUNER_FINAL_MIN_CORING_RESULT);
}

void AmbaItuner_Get_FinalMinCoringResult(AMBA_IK_FNLSHPNS_MIN_COR_RST_s *FinalMinCoringResult)
{
    (void)ituner_memcpy(FinalMinCoringResult, &GData.FinalMinCoringResult, sizeof(AMBA_IK_FNLSHPNS_MIN_COR_RST_s));
}

void AmbaItuner_Set_FinalMaxCoringResult(const AMBA_IK_FNLSHPNS_MAX_COR_RST_s *FinalMaxCoringResult)
{
    (void)ituner_memcpy(&GData.FinalMaxCoringResult, FinalMaxCoringResult, sizeof(GData.FinalMaxCoringResult));
    AmbaItuner_Set_Filter_Valid(ITUNER_FINAL_MAX_CORING_RESULT);
}

void AmbaItuner_Get_FinalMaxCoringResult(AMBA_IK_FNLSHPNS_MAX_COR_RST_s *FinalMaxCoringResult)
{
    (void)ituner_memcpy(FinalMaxCoringResult, &GData.FinalMaxCoringResult, sizeof(AMBA_IK_FNLSHPNS_MAX_COR_RST_s));
}

void AmbaItuner_Set_FinalScaleCoring(const AMBA_IK_FNLSHPNS_SCL_COR_s *FinalScaleCoring)
{
    (void)ituner_memcpy(&GData.FinalScaleCoring, FinalScaleCoring, sizeof(GData.FinalScaleCoring));
    AmbaItuner_Set_Filter_Valid(ITUNER_FINAL_SCALE_CORING);
}

void AmbaItuner_Get_FinalScaleCoring(AMBA_IK_FNLSHPNS_SCL_COR_s *FinalScaleCoring)
{
    (void)ituner_memcpy(FinalScaleCoring, &GData.FinalScaleCoring, sizeof(AMBA_IK_FNLSHPNS_SCL_COR_s));
}

void AmbaItuner_Set_VideoMctf(const AMBA_IK_VIDEO_MCTF_s *VideoMctf)
{
    (void)ituner_memcpy(&GData.VideoMctf, VideoMctf, sizeof(GData.VideoMctf));
    AmbaItuner_Set_Filter_Valid(ITUNER_VIDEO_MCTF);
}

void AmbaItuner_Get_VideoMctf(AMBA_IK_VIDEO_MCTF_s *VideoMctf)
{
    (void)ituner_memcpy(VideoMctf, &GData.VideoMctf, sizeof(AMBA_IK_VIDEO_MCTF_s));
}

void AmbaItuner_Set_VideoMctfLevel(const AMBA_IK_VIDEO_MCTF_s *VideoMctf)
{
    (void)ituner_memcpy(&GData.VideoMctf, VideoMctf, sizeof(GData.VideoMctf));
    AmbaItuner_Set_Filter_Valid(ITUNER_VIDEO_MCTF_LEVEL);
}

void AmbaItuner_Get_VideoMctfLevel(AMBA_IK_VIDEO_MCTF_s *VideoMctf)
{
    (void)ituner_memcpy(VideoMctf, &GData.VideoMctf, sizeof(AMBA_IK_VIDEO_MCTF_s));
}

void AmbaItuner_Set_VideoMctfTemporalAdjust(const AMBA_IK_VIDEO_MCTF_TA_s *VideoMctfTemporalAdjust)
{
    (void)ituner_memcpy(&GData.VideoMctfTemporalAdjust, VideoMctfTemporalAdjust, sizeof(GData.VideoMctfTemporalAdjust));
    AmbaItuner_Set_Filter_Valid(ITUNER_VIDEO_MCTF_TEMPORAL_ADJUST);
}

void AmbaItuner_Get_VideoMctfTemporalAdjust(AMBA_IK_VIDEO_MCTF_TA_s *VideoMctfTemporalAdjust)
{
    (void)ituner_memcpy(VideoMctfTemporalAdjust, &GData.VideoMctfTemporalAdjust, sizeof(AMBA_IK_VIDEO_MCTF_TA_s));
}

void AmbaItuner_Set_VideoMctfAndFinalSharpen(const AMBA_IK_POS_DSP33_t *VideoMctfAndFinalSharpen)
{
    (void)ituner_memcpy(&GData.VideoMctfAndFinalSharpen, VideoMctfAndFinalSharpen, sizeof(GData.VideoMctfAndFinalSharpen));
    AmbaItuner_Set_Filter_Valid(ITUNER_VIDEO_MCTF_AND_FINAL_SHARPEN);
}

void AmbaItuner_Get_VideoMctfAndFinalSharpen(AMBA_IK_POS_DSP33_t *VideoMctfAndFinalSharpen)
{
    (void)ituner_memcpy(VideoMctfAndFinalSharpen, &GData.VideoMctfAndFinalSharpen, sizeof(AMBA_IK_POS_DSP33_t));
}

void AmbaItuner_Set_ShpAOrSpatialFilterSelect(const AMBA_IK_FIRST_LUMA_PROC_MODE_s *FirstLumaProcessMode)
{
    (void)ituner_memcpy(&GData.FirstLumaProcessMode, FirstLumaProcessMode, sizeof(GData.FirstLumaProcessMode));
    AmbaItuner_Set_Filter_Valid(ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT);
}

void AmbaItuner_Get_ShpAOrSpatialFilterSelect(AMBA_IK_FIRST_LUMA_PROC_MODE_s *FirstLumaProcessMode)
{
    (void)ituner_memcpy(FirstLumaProcessMode, &GData.FirstLumaProcessMode, sizeof(AMBA_IK_FIRST_LUMA_PROC_MODE_s));
}

void AmbaItuner_Set_AsfInfo(const ITUNER_ASF_INFO_s *AsfInfo)
{
    (void)ituner_memcpy(&GData.AsfInfo, AsfInfo, sizeof(GData.AsfInfo));
    AmbaItuner_Set_Filter_Valid(ITUNER_ASF_INFO);
}

void AmbaItuner_Get_AsfInfo(ITUNER_ASF_INFO_s *AsfInfo)
{
    (void)ituner_memcpy(AsfInfo, &GData.AsfInfo, sizeof(ITUNER_ASF_INFO_s));
}

void AmbaItuner_Set_ChromaFilter(const AMBA_IK_CHROMA_FILTER_s *ChromaFilter)
{
    (void)ituner_memcpy(&GData.ChromaFilter, ChromaFilter, sizeof(GData.ChromaFilter));
    AmbaItuner_Set_Filter_Valid(ITUNER_CHROMA_FILTER);
}

void AmbaItuner_Get_ChromaFilter(AMBA_IK_CHROMA_FILTER_s *ChromaFilter)
{
    (void)ituner_memcpy(ChromaFilter, &GData.ChromaFilter, sizeof(AMBA_IK_CHROMA_FILTER_s));
}

void AmbaItuner_Set_GbGrMismatch(const AMBA_IK_GRGB_MISMATCH_s *GbGrMismatch)
{
    (void)ituner_memcpy(&GData.GbGrMismatch, GbGrMismatch, sizeof(GData.GbGrMismatch));
    AmbaItuner_Set_Filter_Valid(ITUNER_GB_GR_MISMATCH);
}

void AmbaItuner_Get_GbGrMismatch(AMBA_IK_GRGB_MISMATCH_s *GbGrMismatch)
{
    (void)ituner_memcpy(GbGrMismatch, &GData.GbGrMismatch, sizeof(AMBA_IK_GRGB_MISMATCH_s));
}

void AmbaItuner_Set_SensorInfo(const AMBA_IK_VIN_SENSOR_INFO_s *SensorInfo)
{
    (void)ituner_memcpy(&GData.SensorInfo, SensorInfo, sizeof(GData.SensorInfo));
    AmbaItuner_Set_Filter_Valid(ITUNER_SENSOR_INFO);
}

void AmbaItuner_Get_SensorInfo(AMBA_IK_VIN_SENSOR_INFO_s *SensorInfo)
{
    (void)ituner_memcpy(SensorInfo, &GData.SensorInfo, sizeof(AMBA_IK_VIN_SENSOR_INFO_s));
}

void AmbaItuner_Set_VideoContrastEnhance(const ITUNER_VIDEO_CONTRAST_ENHC_s *ContrastEnhance)
{
    (void)ituner_memcpy(&GData.ContrastEnhance, ContrastEnhance, sizeof(ITUNER_VIDEO_CONTRAST_ENHC_s));
    AmbaItuner_Set_Filter_Valid(ITUNER_CONTRAST_ENHANCE);
}

void AmbaItuner_Get_VideoContrastEnhance(ITUNER_VIDEO_CONTRAST_ENHC_s *ContrastEnhance)
{
    (void)ituner_memcpy(ContrastEnhance, &GData.ContrastEnhance, sizeof(ITUNER_VIDEO_CONTRAST_ENHC_s));
}

void AmbaItuner_Set_VideoHdrLEStaticBlackLevel(UINT8 Index, const AMBA_IK_STATIC_BLC_LVL_s *StaticBlackLevel)
{
    (void)ituner_memcpy(&GData.VideoHdr.LEStaticBlackLevel[Index], StaticBlackLevel, sizeof(AMBA_IK_STATIC_BLC_LVL_s));
    AmbaItuner_Set_Filter_Valid(ITUNER_VHDR_LE_STATIC_BLACK_LEVEL);
}

void AmbaItuner_Get_VideoHdrLEStaticBlackLevel(UINT8 Index, AMBA_IK_STATIC_BLC_LVL_s *StaticBlackLevel)
{
    (void)ituner_memcpy(StaticBlackLevel, &GData.VideoHdr.LEStaticBlackLevel[Index], sizeof(AMBA_IK_STATIC_BLC_LVL_s));
}

void AmbaItuner_Set_VideoHdrBlend(UINT8 Index, const AMBA_IK_HDR_BLEND_s *HdrBlend)
{
    if (Index == 0U) {
        // FIXME
    }
    (void)ituner_memcpy(&GData.VideoHdr.HdrBlend, HdrBlend, sizeof(AMBA_IK_HDR_BLEND_s));
    AmbaItuner_Set_Filter_Valid(ITUNER_VHDR_BLEND);
}

void AmbaItuner_Get_VideoHdrBlend(UINT8 Index, AMBA_IK_HDR_BLEND_s *HdrBlend)
{
    if (Index == 0U) {
        // FIXME
    }
    (void)ituner_memcpy(HdrBlend, &GData.VideoHdr.HdrBlend, sizeof(AMBA_IK_HDR_BLEND_s));
}

void AmbaItuner_Set_VideoHdrFrontEndWbGain(UINT8 Index, const AMBA_IK_FE_WB_GAIN_s *FrontEndWbGain)
{
    (void)ituner_memcpy(&GData.FrontEndWbGain[Index], FrontEndWbGain, sizeof(AMBA_IK_FE_WB_GAIN_s));
    if (Index == 0U) {
        AmbaItuner_Set_Filter_Valid(ITUNER_FRONT_END_WB_GAIN_EXP0);
    } else if (Index==1U) {
        AmbaItuner_Set_Filter_Valid(ITUNER_FRONT_END_WB_GAIN_EXP1);
    } else if (Index==2U) {
        AmbaItuner_Set_Filter_Valid(ITUNER_FRONT_END_WB_GAIN_EXP2);
    } else {
        ituner_print_str_5(" Index error ", DC_S, DC_S, DC_S, DC_S, DC_S);
    }
}

void AmbaItuner_Get_VideoHdrFrontEndWbGain(UINT8 Index, AMBA_IK_FE_WB_GAIN_s *FrontEndWbGain)
{
    (void)ituner_memcpy(FrontEndWbGain, &GData.FrontEndWbGain[Index], sizeof(AMBA_IK_FE_WB_GAIN_s));
}

void AmbaItuner_Set_VideoHdrRawInfo(const AMBA_ITN_VIDEO_HDR_RAW_INFO_s *RawInfo)
{
    (void)ituner_memcpy(&GData.VideoHdr.RawInfo, RawInfo, sizeof(AMBA_ITN_VIDEO_HDR_RAW_INFO_s));
    AmbaItuner_Set_Filter_Valid(ITUNER_VHDR_RAW_INFO);
}

void AmbaItuner_Get_VideoHdrRawInfo(AMBA_ITN_VIDEO_HDR_RAW_INFO_s *RawInfo)
{
    (void)ituner_memcpy(RawInfo, &GData.VideoHdr.RawInfo, sizeof(AMBA_ITN_VIDEO_HDR_RAW_INFO_s));
}

void AmbaItuner_Set_VideoHdrStaticBlackLevel(UINT8 Index, const AMBA_IK_STATIC_BLC_LVL_s *StaticBlackLevel)
{
    (void)ituner_memcpy(&GData.StaticBlackLevel[Index], StaticBlackLevel, sizeof(AMBA_IK_STATIC_BLC_LVL_s));
    switch(Index) {
    case 0:
        AmbaItuner_Set_Filter_Valid(ITUNER_STATIC_BLACK_LEVEL_EXP0);
        break;
    case 1:
        AmbaItuner_Set_Filter_Valid(ITUNER_STATIC_BLACK_LEVEL_EXP1);
        break;
    case 2:
        AmbaItuner_Set_Filter_Valid(ITUNER_STATIC_BLACK_LEVEL_EXP2);
        break;
    default:
        ituner_print_str_5(" Index error ", DC_S, DC_S, DC_S, DC_S, DC_S);
        break;
    }
}

void AmbaItuner_Get_VideoHdrStaticBlackLevel(UINT8 Index, AMBA_IK_STATIC_BLC_LVL_s *StaticBlackLevel)
{
    (void)ituner_memcpy(StaticBlackLevel, &GData.StaticBlackLevel[Index], sizeof(AMBA_IK_STATIC_BLC_LVL_s));
}


void AmbaItuner_Set_VignetteCompensation(const ITUNER_VIGNETTE_s *VignetteCompensation)
{
    (void)ituner_memcpy(&GData.VignetteCompensation, VignetteCompensation, sizeof(GData.VignetteCompensation));
    AmbaItuner_Set_Filter_Valid(ITUNER_VIGNETTE_COMPENSATION);
}

void AmbaItuner_Get_VignetteCompensation(ITUNER_VIGNETTE_s *VignetteCompensation)
{
    (void)ituner_memcpy(VignetteCompensation, &GData.VignetteCompensation, sizeof(ITUNER_VIGNETTE_s));
}

void AmbaItuner_SetCawarpInfo(const ITUNER_CHROMA_ABERRATION_s *ChromaAberrationInfo)
{
    (void)ituner_memcpy(&GData.ChromaAberrationInfo, ChromaAberrationInfo, sizeof(GData.ChromaAberrationInfo));
    AmbaItuner_Set_Filter_Valid(ITUNER_CHROMA_ABERRATION_INFO);
}

void AmbaItuner_GetCawarpInfo(ITUNER_CHROMA_ABERRATION_s *ChromaAberrationInfo)
{
    (void)ituner_memcpy(ChromaAberrationInfo, &GData.ChromaAberrationInfo, sizeof(ITUNER_CHROMA_ABERRATION_s));
}

void AmbaItuner_GetCawarpInfoRetrive(AMBA_IK_CAWARP_INFO_s *CawarpCompensationInfo)
{
    (void)ituner_memcpy(CawarpCompensationInfo, &GData.CaWarpCompensationInfo, sizeof(AMBA_IK_CAWARP_INFO_s));
}

void AmbaItuner_SetCaWarpInternal(const ITUNER_CHROMA_ABERRATION_INTERNAL_s *ChromaAberrationInfoInternal)
{
    (void)ituner_memcpy(&GData.ChromaAberrationInfoInternal, ChromaAberrationInfoInternal, sizeof(GData.ChromaAberrationInfoInternal));
    AmbaItuner_Set_Filter_Valid(ITUNER_CHROMA_ABERRATION_INFO_INTERNAL);
}

void AmbaItuner_GetCaWarpInternal(ITUNER_CHROMA_ABERRATION_INTERNAL_s *ChromaAberrationInfoInternal)
{
    (void)ituner_memcpy(ChromaAberrationInfoInternal, &GData.ChromaAberrationInfoInternal, sizeof(ITUNER_CHROMA_ABERRATION_INTERNAL_s));
}

void AmbaItuner_Set_WarpCompensationDzoomInternal(const ITUNER_WARP_DZOOM_INTERNAL_s *WarpCompensationDzoomInternal)
{
    (void)ituner_memcpy(&GData.WarpCompensationDzoomInternal, WarpCompensationDzoomInternal, sizeof(GData.WarpCompensationDzoomInternal));
    AmbaItuner_Set_Filter_Valid(ITUNER_WARP_COMPENSATION_DZOOM_INTERNAL);
}

void AmbaItuner_Get_WarpCompensationDzoomInternal(ITUNER_WARP_DZOOM_INTERNAL_s *WarpCompensationDzoomInternal)
{
    (void)ituner_memcpy(WarpCompensationDzoomInternal, &GData.WarpCompensationDzoomInternal, sizeof(ITUNER_WARP_DZOOM_INTERNAL_s));
}

void AmbaItuner_SetWarpInfo(const ITUNER_WARP_s *WarpCompensation)
{
    (void)ituner_memcpy(&GData.WarpCompensation, WarpCompensation, sizeof(GData.WarpCompensation));
    AmbaItuner_Set_Filter_Valid(ITUNER_WARP_COMPENSATION);
}

void AmbaItuner_GetWarpInfo(ITUNER_WARP_s *WarpCompensation)
{
    (void)ituner_memcpy(WarpCompensation, &GData.WarpCompensation, sizeof(ITUNER_WARP_s));
}

void AmbaItuner_GetWarpInfoRetrive(AMBA_IK_WARP_INFO_s *WarpCompensationInfo)
{
    (void)ituner_memcpy(WarpCompensationInfo, &GData.WarpCompensationInfo, sizeof(AMBA_IK_WARP_INFO_s));
}

void AmbaItuner_Set_Dzoom(const AMBA_IK_DZOOM_INFO_s *Dzoom)
{
    (void)ituner_memcpy(&GData.Dzoom, Dzoom, sizeof(GData.Dzoom));
    AmbaItuner_Set_Filter_Valid(ITUNER_DZOOM);
}

void AmbaItuner_Get_Dzoom(AMBA_IK_DZOOM_INFO_s *Dzoom)
{
    (void)ituner_memcpy(Dzoom, &GData.Dzoom, sizeof(AMBA_IK_DZOOM_INFO_s));
}

void AmbaItuner_Set_Dummy(const AMBA_IK_DUMMY_MARGIN_RANGE_s *Dummy)
{
    (void)ituner_memcpy(&GData.Dummy, Dummy, sizeof(GData.Dzoom));
    AmbaItuner_Set_Filter_Valid(ITUNER_DUMMY);
}

void AmbaItuner_Get_Dummy(AMBA_IK_DUMMY_MARGIN_RANGE_s *Dummy)
{
    (void)ituner_memcpy(Dummy, &GData.Dummy, sizeof(AMBA_IK_DUMMY_MARGIN_RANGE_s));
}

void AmbaItuner_Set_Active(const AMBA_IK_VIN_ACTIVE_WINDOW_s *Active)
{
    (void)ituner_memcpy(&GData.Active, Active, sizeof(GData.Active));
    AmbaItuner_Set_Filter_Valid(ITUNER_ACTIVE);
}

void AmbaItuner_Get_Active(AMBA_IK_VIN_ACTIVE_WINDOW_s *Active)
{
    (void)ituner_memcpy(Active, &GData.Active, sizeof(AMBA_IK_VIN_ACTIVE_WINDOW_s));
}

static INT32 Dummy_Func(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    if ((Ext_File_Type==FILE_EXT_FPN_MAP) || (Ext_File_Param==NULL)) {
        // FIXME
    }
    ituner_print_str_5(" Without register the call back function! ", __func__, DC_S, DC_S, DC_S, DC_S);
    return (INT32)0xFFFFFFFFUL;
}
// hiso
void AmbaItuner_Set_ChromaAsf(const AMBA_IK_HI_LOW_CHROMA_ASF_s *ChromaAsf)
{
    (void)ituner_memcpy(&GData.ChromaAsf, ChromaAsf, sizeof(GData.ChromaAsf));
    AmbaItuner_Set_Filter_Valid(ITUNER_LI_CHROMA_ASF);
}

void AmbaItuner_Get_ChromaAsf(AMBA_IK_HI_LOW_CHROMA_ASF_s *ChromaAsf)
{
    (void)ituner_memcpy(ChromaAsf, &GData.ChromaAsf, sizeof(AMBA_IK_HI_LOW_CHROMA_ASF_s));
}

void AmbaItuner_Set_HiChromaAsf(const AMBA_IK_HI_CHROMA_ASF_s *HiChromaAsf)
{
    (void)ituner_memcpy(&GData.HiChromaAsf, HiChromaAsf, sizeof(GData.HiChromaAsf));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_CHROMA_ASF);
}

void AmbaItuner_Get_HiChromaAsf(AMBA_IK_HI_CHROMA_ASF_s *HiChromaAsf)
{
    (void)ituner_memcpy(HiChromaAsf, &GData.HiChromaAsf, sizeof(AMBA_IK_HI_CHROMA_ASF_s));
}

void AmbaItuner_Set_HiASF(const AMBA_IK_HI_ASF_s *HiAsf)
{
    (void)ituner_memcpy(&GData.HiAsf, HiAsf, sizeof(GData.HiAsf));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_LUMA_ASF);
}

void AmbaItuner_Get_HiASF(AMBA_IK_HI_ASF_s *HiAsf)
{
    (void)ituner_memcpy(HiAsf, &GData.HiAsf, sizeof(AMBA_IK_HI_ASF_s));
}

void AmbaItuner_Set_HiCfaNoiseFilter(const AMBA_IK_HI_CFA_NOISE_FILTER_s *HiCfaNoiseFilter)
{
    (void)ituner_memcpy(&GData.HiCfaNoiseFilter, HiCfaNoiseFilter, sizeof(GData.HiCfaNoiseFilter));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_CFA_NOISE_FILTER);
}

void AmbaItuner_Get_HiCfaNoiseFilter(AMBA_IK_HI_CFA_NOISE_FILTER_s *HiCfaNoiseFilter)
{
    (void)ituner_memcpy(HiCfaNoiseFilter, &GData.HiCfaNoiseFilter, sizeof(AMBA_IK_HI_CFA_NOISE_FILTER_s));
}

void AmbaItuner_Set_HiCfaLeakageFilter(const AMBA_IK_HI_CFA_LEAKAGE_FILTER_s *HiCfaLeakageFilter)
{
    (void)ituner_memcpy(&GData.HiCfaLeakageFilter, HiCfaLeakageFilter, sizeof(GData.HiCfaLeakageFilter));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_CFA_LEAKAGE_FILTER);
}

void AmbaItuner_Get_HiCfaLeakageFilter(AMBA_IK_HI_CFA_LEAKAGE_FILTER_s *HiCfaLeakageFilter)
{
    (void)ituner_memcpy(HiCfaLeakageFilter, &GData.HiCfaLeakageFilter, sizeof(AMBA_IK_HI_CFA_LEAKAGE_FILTER_s));
}

void AmbaItuner_Set_HiAntiAliasing(const AMBA_IK_HI_ANTI_ALIASING_s *HiAntiAliasing)
{
    (void)ituner_memcpy(&GData.HiAntiAliasing, HiAntiAliasing, sizeof(GData.HiAntiAliasing));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_ANTI_ALIASING);
}

void AmbaItuner_Get_HiAntiAliasing(AMBA_IK_HI_ANTI_ALIASING_s *HiAntiAliasing)
{
    (void)ituner_memcpy(HiAntiAliasing, &GData.HiAntiAliasing, sizeof(AMBA_IK_HI_ANTI_ALIASING_s));
}

void AmbaItuner_Set_HiDynamicBpc(const AMBA_IK_HI_DYNAMIC_BPC_s *HiDynamicBpc)
{
    (void)ituner_memcpy(&GData.HiDynamicBpc, HiDynamicBpc, sizeof(GData.HiDynamicBpc));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_AUTO_BAD_PIXEL_CORRECTION);
}

void AmbaItuner_Get_HiDynamicBpc(AMBA_IK_HI_DYNAMIC_BPC_s *HiDynamicBpc)
{
    (void)ituner_memcpy(HiDynamicBpc, &GData.HiDynamicBpc, sizeof(AMBA_IK_HI_DYNAMIC_BPC_s));
}

void AmbaItuner_Set_HiChromaHigh(const AMBA_IK_HI_CHROMA_FILTER_HIGH_s *HiChromaFilterHigh)
{
    (void)ituner_memcpy(&GData.HiChromaFilterHigh, HiChromaFilterHigh, sizeof(GData.HiChromaFilterHigh));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_CHROMA_FLTR_HIGH);
}

void AmbaItuner_Get_HiChromaHigh(AMBA_IK_HI_CHROMA_FILTER_HIGH_s *HiChromaFilterHigh)
{
    (void)ituner_memcpy(HiChromaFilterHigh, &GData.HiChromaFilterHigh, sizeof(AMBA_IK_HI_CHROMA_FILTER_HIGH_s));
}

void AmbaItuner_Set_HiChromaLow(const AMBA_IK_HI_CHROMA_FILTER_LOW_s *HiChromaFilterLow)
{
    (void)ituner_memcpy(&GData.HiChromaFilterLow, HiChromaFilterLow, sizeof(GData.HiChromaFilterLow));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_CHROMA_FLTR_LOW);
}

void AmbaItuner_Get_HiChromaLow(AMBA_IK_HI_CHROMA_FILTER_LOW_s *HiChromaFilterLow)
{
    (void)ituner_memcpy(HiChromaFilterLow, &GData.HiChromaFilterLow, sizeof(AMBA_IK_HI_CHROMA_FILTER_LOW_s));
}

void AmbaItuner_Set_HiChromaLowCombine(const AMBA_IK_HI_CHROMA_FLTR_LOW_COM_s *HiChromaFilterLowCombine)
{
    (void)ituner_memcpy(&GData.HiChromaFilterLowCombine, HiChromaFilterLowCombine, sizeof(GData.HiChromaFilterLowCombine));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_CHROMA_FLTR_LOW_COMBINE);
}

void AmbaItuner_Get_HiChromaLowCombine(AMBA_IK_HI_CHROMA_FLTR_LOW_COM_s *HiChromaFilterLowCombine)
{
    (void)ituner_memcpy(HiChromaFilterLowCombine, &GData.HiChromaFilterLowCombine, sizeof(AMBA_IK_HI_CHROMA_FLTR_LOW_COM_s));
}

void AmbaItuner_Set_HiChromaMed(const AMBA_IK_HI_CHROMA_FILTER_MED_s *HiChromaFilterMed)
{
    (void)ituner_memcpy(&GData.HiChromaFilterMed, HiChromaFilterMed, sizeof(GData.HiChromaFilterMed));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_CHROMA_FLTR_MEDIAN);
}

void AmbaItuner_Get_HiChromaMed(AMBA_IK_HI_CHROMA_FILTER_MED_s *HiChromaFilterMed)
{
    (void)ituner_memcpy(HiChromaFilterMed, &GData.HiChromaFilterMed, sizeof(AMBA_IK_HI_CHROMA_FILTER_MED_s));
}

void AmbaItuner_Set_HiChromaMedCombine(const AMBA_IK_HI_CHROMA_FLTR_MED_COM_s *HiChromaFilterMedCombine)
{
    (void)ituner_memcpy(&GData.HiChromaFilterMedCombine, HiChromaFilterMedCombine, sizeof(GData.HiChromaFilterMedCombine));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_CHROMA_FLTR_MEDIAN_COMBINE);
}

void AmbaItuner_Get_HiChromaMedCombine(AMBA_IK_HI_CHROMA_FLTR_MED_COM_s *HiChromaFilterMedCombine)
{
    (void)ituner_memcpy(HiChromaFilterMedCombine, &GData.HiChromaFilterMedCombine, sizeof(AMBA_IK_HI_CHROMA_FLTR_MED_COM_s));
}

void AmbaItuner_Set_HiChromaPre(const AMBA_IK_HI_CHROMA_FILTER_PRE_s *HiChromaFilterPre)
{
    (void)ituner_memcpy(&GData.HiChromaFilterPre, HiChromaFilterPre, sizeof(GData.HiChromaFilterPre));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_CHROMA_FLTR_PRE);
}

void AmbaItuner_Get_HiChromaPre(AMBA_IK_HI_CHROMA_FILTER_PRE_s *HiChromaFilterPre)
{
    (void)ituner_memcpy(HiChromaFilterPre, &GData.HiChromaFilterPre, sizeof(AMBA_IK_HI_CHROMA_FILTER_PRE_s));
}

void AmbaItuner_Set_HiChromaVeryLow(const AMBA_IK_HI_CHROMA_FILTER_VERY_LOW_s *HiChromaFilterVeryLow)
{
    (void)ituner_memcpy(&GData.HiChromaFilterVeryLow, HiChromaFilterVeryLow, sizeof(GData.HiChromaFilterVeryLow));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_CHROMA_FLTR_VERY_LOW);
}

void AmbaItuner_Get_HiChromaVeryLow(AMBA_IK_HI_CHROMA_FILTER_VERY_LOW_s *HiChromaFilterVeryLow)
{
    (void)ituner_memcpy(HiChromaFilterVeryLow, &GData.HiChromaFilterVeryLow, sizeof(AMBA_IK_HI_CHROMA_FILTER_VERY_LOW_s));
}

void AmbaItuner_Set_HiChromaVeryLowCombine(const AMBA_IK_HI_CHROMA_FLTR_VERY_LOW_COM_s *HiChromaFilterVeryLowCombine)
{
    (void)ituner_memcpy(&GData.HiChromaFilterVeryLowCombine, HiChromaFilterVeryLowCombine, sizeof(GData.HiChromaFilterVeryLowCombine));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_CHROMA_FLTR_VERY_LOW_COMBINE);
}

void AmbaItuner_Get_HiChromaVeryLowCombine(AMBA_IK_HI_CHROMA_FLTR_VERY_LOW_COM_s *HiChromaFilterVeryLowCombine)
{
    (void)ituner_memcpy(HiChromaFilterVeryLowCombine, &GData.HiChromaFilterVeryLowCombine, sizeof(AMBA_IK_HI_CHROMA_FLTR_VERY_LOW_COM_s));
}

void AmbaItuner_Set_HiChromaMedianFilter(const AMBA_IK_HI_CHROMA_MEDIAN_FILTER_s *HiChromaMedianFilter)
{
    (void)ituner_memcpy(&GData.HiChromaMedianFilter, HiChromaMedianFilter, sizeof(GData.HiChromaMedianFilter));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_CHROMA_MEDIAN_FILTER);
}

void AmbaItuner_Get_HiChromaMedianFilter(AMBA_IK_HI_CHROMA_MEDIAN_FILTER_s *HiChromaMedianFilter)
{
    (void)ituner_memcpy(HiChromaMedianFilter, &GData.HiChromaMedianFilter, sizeof(AMBA_IK_HI_CHROMA_MEDIAN_FILTER_s));
}

void AmbaItuner_Set_HiDemosaic(const AMBA_IK_HI_DEMOSAIC_s *HiDemosaic)
{
    (void)ituner_memcpy(&GData.HiDemosaic, HiDemosaic, sizeof(GData.HiDemosaic));
    AmbaItuner_Set_Filter_Valid(ITUNRE_HI_DEMOSAIC_FILTER);
}

void AmbaItuner_Get_HiDemosaic(AMBA_IK_HI_DEMOSAIC_s *HiDemosaic)
{
    (void)ituner_memcpy(HiDemosaic, &GData.HiDemosaic, sizeof(AMBA_IK_HI_DEMOSAIC_s));
}

void AmbaItuner_Set_HiGrGbMismatch(const AMBA_IK_HI_GRGB_MISMATCH_s *HiGrGbMismatch)
{
    (void)ituner_memcpy(&GData.HiGrGbMismatch, HiGrGbMismatch, sizeof(GData.HiGrGbMismatch));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_GB_GR_MISMATCH);
}

void AmbaItuner_Get_HiGrGbMismatch(AMBA_IK_HI_GRGB_MISMATCH_s *HiGrGbMismatch)
{
    (void)ituner_memcpy(HiGrGbMismatch, &GData.HiGrGbMismatch, sizeof(AMBA_IK_HI_GRGB_MISMATCH_s));
}

void AmbaItuner_Set_HiHigh2Asf(const AMBA_IK_HI_HIGH2_ASF_s *HiHigh2Asf)
{
    (void)ituner_memcpy(&GData.HiHigh2Asf, HiHigh2Asf, sizeof(GData.HiHigh2Asf));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_HIGH2_LUMA_ASF);
}

void AmbaItuner_Get_HiHigh2Asf(AMBA_IK_HI_HIGH2_ASF_s *HiHigh2Asf)
{
    (void)ituner_memcpy(HiHigh2Asf, &GData.HiHigh2Asf, sizeof(AMBA_IK_HI_HIGH2_ASF_s));
}

void AmbaItuner_Set_HiHighAsf(const AMBA_IK_HI_HIGH_ASF_s *HiHighAsf)
{
    (void)ituner_memcpy(&GData.HiHighAsf, HiHighAsf, sizeof(GData.HiHighAsf));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_HIGH_LUMA_ASF);
}

void AmbaItuner_Get_HiHighAsf(AMBA_IK_HI_HIGH_ASF_s *HiHighAsf)
{
    (void)ituner_memcpy(HiHighAsf, &GData.HiHighAsf, sizeof(AMBA_IK_HI_HIGH_ASF_s));
}

void AmbaItuner_Set_HiHighShpnsBoth(const AMBA_IK_HI_HIGH_SHPNS_BOTH_s *HiHighShpnsBoth)
{
    (void)ituner_memcpy(&GData.HiHighShpnsBoth, HiHighShpnsBoth, sizeof(GData.HiHighShpnsBoth));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_HIGH_SHARPEN_BOTH);
}

void AmbaItuner_Get_HiHighShpnsBoth(AMBA_IK_HI_HIGH_SHPNS_BOTH_s *HiHighShpnsBoth)
{
    (void)ituner_memcpy(HiHighShpnsBoth, &GData.HiHighShpnsBoth, sizeof(AMBA_IK_HI_HIGH_SHPNS_BOTH_s));
}

void AmbaItuner_Set_HiHighShpnsNoise(const AMBA_IK_HI_HIGH_SHPNS_NOISE_s *HiHighShpnsNoise)
{
    (void)ituner_memcpy(&GData.HiHighShpnsNoise, HiHighShpnsNoise, sizeof(GData.HiHighShpnsNoise));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_HIGH_SHARPEN_NOISE);
}

void AmbaItuner_Get_HiHighShpnsNoise(AMBA_IK_HI_HIGH_SHPNS_NOISE_s *HiHighShpnsNoise)
{
    (void)ituner_memcpy(HiHighShpnsNoise, &GData.HiHighShpnsNoise, sizeof(AMBA_IK_HI_HIGH_SHPNS_NOISE_s));
}

void AmbaItuner_Set_HiHighShpnsFir(const AMBA_IK_HI_HIGH_SHPNS_FIR_s *HiHighShpnsFir)
{
    (void)ituner_memcpy(&GData.HiHighShpnsFir, HiHighShpnsFir, sizeof(GData.HiHighShpnsFir));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_HIGH_FIR);
}

void AmbaItuner_Get_HiHighShpnsFir(AMBA_IK_HI_HIGH_SHPNS_FIR_s *HiHighShpnsFir)
{
    (void)ituner_memcpy(HiHighShpnsFir, &GData.HiHighShpnsFir, sizeof(AMBA_IK_HI_HIGH_SHPNS_FIR_s));
}

void AmbaItuner_Set_HiHighShpnsCoring(const AMBA_IK_HI_HIGH_SHPNS_CORING_s *HiHighShpnsCoring)
{
    (void)ituner_memcpy(&GData.HiHighShpnsCoring, HiHighShpnsCoring, sizeof(GData.HiHighShpnsCoring));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_HIGH_CORING);
}

void AmbaItuner_Get_HiHighShpnsCoring(AMBA_IK_HI_HIGH_SHPNS_CORING_s *HiHighShpnsCoring)
{
    (void)ituner_memcpy(HiHighShpnsCoring, &GData.HiHighShpnsCoring, sizeof(AMBA_IK_HI_HIGH_SHPNS_CORING_s));
}

void AmbaItuner_Set_HiHighShpnsCorIdxScl(const AMBA_IK_HI_HIGH_SHPNS_COR_IDX_SCL_s *HiHighShpnsCorIdxScl)
{
    (void)ituner_memcpy(&GData.HiHighShpnsCorIdxScl, HiHighShpnsCorIdxScl, sizeof(GData.HiHighShpnsCorIdxScl));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_HIGH_CORING_INDEX_SCALE);
}

void AmbaItuner_Get_HiHighShpnsCorIdxScl(AMBA_IK_HI_HIGH_SHPNS_COR_IDX_SCL_s *HiHighShpnsCorIdxScl)
{
    (void)ituner_memcpy(HiHighShpnsCorIdxScl, &GData.HiHighShpnsCorIdxScl, sizeof(AMBA_IK_HI_HIGH_SHPNS_COR_IDX_SCL_s));
}

void AmbaItuner_Set_HiHighShpnsMaxCorRst(const AMBA_IK_HI_HIGH_SHPNS_MAX_COR_RST_s *HiHighShpnsMaxCorRst)
{
    (void)ituner_memcpy(&GData.HiHighShpnsMaxCorRst, HiHighShpnsMaxCorRst, sizeof(GData.HiHighShpnsMaxCorRst));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_HIGH_MAX_CORING_RESULT);
}

void AmbaItuner_Get_HiHighShpnsMaxCorRst(AMBA_IK_HI_HIGH_SHPNS_MAX_COR_RST_s *HiHighShpnsMaxCorRst)
{
    (void)ituner_memcpy(HiHighShpnsMaxCorRst, &GData.HiHighShpnsMaxCorRst, sizeof(AMBA_IK_HI_HIGH_SHPNS_MAX_COR_RST_s));
}

void AmbaItuner_Set_HiHighShpnsMinCorRst(const AMBA_IK_HI_HIGH_SHPNS_MIN_COR_RST_s *HiHighShpnsMinCorRst)
{
    (void)ituner_memcpy(&GData.HiHighShpnsMinCorRst, HiHighShpnsMinCorRst, sizeof(GData.HiHighShpnsMinCorRst));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_HIGH_MIN_CORING_RESULT);
}

void AmbaItuner_Get_HiHighShpnsMinCorRst(AMBA_IK_HI_HIGH_SHPNS_MIN_COR_RST_s *HiHighShpnsMinCorRst)
{
    (void)ituner_memcpy(HiHighShpnsMinCorRst, &GData.HiHighShpnsMinCorRst, sizeof(AMBA_IK_HI_HIGH_SHPNS_MIN_COR_RST_s));
}

void AmbaItuner_Set_HiHighShpnsSclCor(const AMBA_IK_HI_HIGH_SHPNS_SCL_COR_s *HiHighShpnsSclCor)
{
    (void)ituner_memcpy(&GData.HiHighShpnsSclCor, HiHighShpnsSclCor, sizeof(GData.HiHighShpnsSclCor));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_HIGH_SCALE_CORING);
}

void AmbaItuner_Get_HiHighShpnsSclCor(AMBA_IK_HI_HIGH_SHPNS_SCL_COR_s *HiHighShpnsSclCor)
{
    (void)ituner_memcpy(HiHighShpnsSclCor, &GData.HiHighShpnsSclCor, sizeof(AMBA_IK_HI_HIGH_SHPNS_SCL_COR_s));
}

void AmbaItuner_Set_HiLowAsf(const AMBA_IK_HI_LOW_ASF_s *HiLowAsf)
{
    (void)ituner_memcpy(&GData.HiLowAsf, HiLowAsf, sizeof(GData.HiLowAsf));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_LOW_LUMA_ASF);
}

void AmbaItuner_Get_HiLowAsf(AMBA_IK_HI_LOW_ASF_s *HiLowAsf)
{
    (void)ituner_memcpy(HiLowAsf, &GData.HiLowAsf, sizeof(AMBA_IK_HI_LOW_ASF_s));
}

void AmbaItuner_Set_HiLowAsfCombine(const AMBA_IK_HI_LOW_ASF_COMBINE_s *HiLowAsfCombine)
{
    (void)ituner_memcpy(&GData.HiLowAsfCombine, HiLowAsfCombine, sizeof(GData.HiLowAsfCombine));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_LOW_LUMA_ASF_COMBINE);
}

void AmbaItuner_Get_HiLowAsfCombine(AMBA_IK_HI_LOW_ASF_COMBINE_s *HiLowAsfCombine)
{
    (void)ituner_memcpy(HiLowAsfCombine, &GData.HiLowAsfCombine, sizeof(AMBA_IK_HI_LOW_ASF_COMBINE_s));
}

void AmbaItuner_Set_HiLumaCombine(const AMBA_IK_HI_LUMA_COMBINE_s *HiLumaCombine)
{
    (void)ituner_memcpy(&GData.HiLumaCombine, HiLumaCombine, sizeof(GData.HiLumaCombine));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_LUMA_NOISE_COMBINE);
}

void AmbaItuner_Get_HiLumaCombine(AMBA_IK_HI_LUMA_COMBINE_s *HiLumaCombine)
{
    (void)ituner_memcpy(HiLumaCombine, &GData.HiLumaCombine, sizeof(AMBA_IK_HI_LUMA_COMBINE_s));
}

void AmbaItuner_Set_HiMed1Asf(const AMBA_IK_HI_MED1_ASF_s *HiMed1Asf)
{
    (void)ituner_memcpy(&GData.HiMed1Asf, HiMed1Asf, sizeof(GData.HiMed1Asf));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_MED1_LUMA_ASF);
}

void AmbaItuner_Get_HiMed1Asf(AMBA_IK_HI_MED1_ASF_s *HiMed1Asf)
{
    (void)ituner_memcpy(HiMed1Asf, &GData.HiMed1Asf, sizeof(AMBA_IK_HI_MED1_ASF_s));
}

void AmbaItuner_Set_HiMed2Asf(const AMBA_IK_HI_MED2_ASF_s *HiMed2Asf)
{
    (void)ituner_memcpy(&GData.HiMed2Asf, HiMed2Asf, sizeof(GData.HiMed2Asf));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_MED2_LUMA_ASF);
}

void AmbaItuner_Get_HiMed2Asf(AMBA_IK_HI_MED2_ASF_s *HiMed2Asf)
{
    (void)ituner_memcpy(HiMed2Asf, &GData.HiMed2Asf, sizeof(AMBA_IK_HI_MED2_ASF_s));
}

void AmbaItuner_Set_HiMedShpnsBoth(const AMBA_IK_HI_MED_SHPNS_BOTH_s *HiMedShpnsBoth)
{
    (void)ituner_memcpy(&GData.HiMedShpnsBoth, HiMedShpnsBoth, sizeof(GData.HiMedShpnsBoth));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_MED_SHARPEN_BOTH);
}

void AmbaItuner_Get_HiMedShpnsBoth(AMBA_IK_HI_MED_SHPNS_BOTH_s *HiMedShpnsBoth)
{
    (void)ituner_memcpy(HiMedShpnsBoth, &GData.HiMedShpnsBoth, sizeof(AMBA_IK_HI_MED_SHPNS_BOTH_s));
}

void AmbaItuner_Set_HiMedShpnsNoise(const AMBA_IK_HI_MED_SHPNS_NOISE_s *HiMedShpnsNoise)
{
    (void)ituner_memcpy(&GData.HiMedShpnsNoise, HiMedShpnsNoise, sizeof(GData.HiMedShpnsNoise));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_MED_SHARPEN_NOISE);
}

void AmbaItuner_Get_HiMedShpnsNoise(AMBA_IK_HI_MED_SHPNS_NOISE_s *HiMedShpnsNoise)
{
    (void)ituner_memcpy(HiMedShpnsNoise, &GData.HiMedShpnsNoise, sizeof(AMBA_IK_HI_MED_SHPNS_NOISE_s));
}

void AmbaItuner_Set_HiMedShpnsFir(const AMBA_IK_HI_MED_SHPNS_FIR_s *HiMedShpnsFir)
{
    (void)ituner_memcpy(&GData.HiMedShpnsFir, HiMedShpnsFir, sizeof(GData.HiMedShpnsFir));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_MED_FIR);
}

void AmbaItuner_Get_HiMedShpnsFir(AMBA_IK_HI_MED_SHPNS_FIR_s *HiMedShpnsFir)
{
    (void)ituner_memcpy(HiMedShpnsFir, &GData.HiMedShpnsFir, sizeof(AMBA_IK_HI_MED_SHPNS_FIR_s));
}

void AmbaItuner_Set_HiMedShpnsCoring(const AMBA_IK_HI_MED_SHPNS_CORING_s *HiMedShpnsCoring)
{
    (void)ituner_memcpy(&GData.HiMedShpnsCoring, HiMedShpnsCoring, sizeof(GData.HiMedShpnsCoring));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_MED_CORING);
}

void AmbaItuner_Get_HiMedShpnsCoring(AMBA_IK_HI_MED_SHPNS_CORING_s *HiMedShpnsCoring)
{
    (void)ituner_memcpy(HiMedShpnsCoring, &GData.HiMedShpnsCoring, sizeof(AMBA_IK_HI_MED_SHPNS_CORING_s));
}

void AmbaItuner_Set_HiMedShpnsCorIdxScl(const AMBA_IK_HI_MED_SHPNS_COR_IDX_SCL_s *HiMedShpnsCorIdxScl)
{
    (void)ituner_memcpy(&GData.HiMedShpnsCorIdxScl, HiMedShpnsCorIdxScl, sizeof(GData.HiMedShpnsCorIdxScl));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_MED_CORING_INDEX_SCALE);
}

void AmbaItuner_Get_HiMedShpnsCorIdxScl(AMBA_IK_HI_MED_SHPNS_COR_IDX_SCL_s *HiMedShpnsCorIdxScl)
{
    (void)ituner_memcpy(HiMedShpnsCorIdxScl, &GData.HiMedShpnsCorIdxScl, sizeof(AMBA_IK_HI_MED_SHPNS_COR_IDX_SCL_s));
}

void AmbaItuner_Set_HiMedShpnsMaxCorRst(const AMBA_IK_HI_MED_SHPNS_MAX_COR_RST_s *HiMedShpnsMaxCorRst)
{
    (void)ituner_memcpy(&GData.HiMedShpnsMaxCorRst, HiMedShpnsMaxCorRst, sizeof(GData.HiMedShpnsMaxCorRst));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_MED_MAX_CORING_RESULT);
}

void AmbaItuner_Get_HiMedShpnsMaxCorRst(AMBA_IK_HI_MED_SHPNS_MAX_COR_RST_s *HiMedShpnsMaxCorRst)
{
    (void)ituner_memcpy(HiMedShpnsMaxCorRst, &GData.HiMedShpnsMaxCorRst, sizeof(AMBA_IK_HI_MED_SHPNS_MAX_COR_RST_s));
}

void AmbaItuner_Set_HiMedShpnsMinCorRst(const AMBA_IK_HI_MED_SHPNS_MIN_COR_RST_s *HiMedShpnsMinCorRst)
{
    (void)ituner_memcpy(&GData.HiMedShpnsMinCorRst, HiMedShpnsMinCorRst, sizeof(GData.HiMedShpnsMinCorRst));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_MED_MIN_CORING_RESULT);
}

void AmbaItuner_Get_HiMedShpnsMinCorRst(AMBA_IK_HI_MED_SHPNS_MIN_COR_RST_s *HiMedShpnsMinCorRst)
{
    (void)ituner_memcpy(HiMedShpnsMinCorRst, &GData.HiMedShpnsMinCorRst, sizeof(AMBA_IK_HI_MED_SHPNS_MIN_COR_RST_s));
}

void AmbaItuner_Set_HiMedShpnsSclCor(const AMBA_IK_HI_MED_SHPNS_SCL_COR_s *HiMedShpnsSclCor)
{
    (void)ituner_memcpy(&GData.HiMedShpnsSclCor, HiMedShpnsSclCor, sizeof(GData.HiMedShpnsSclCor));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_MED_SCALE_CORING);
}

void AmbaItuner_Get_HiMedShpnsSclCor(AMBA_IK_HI_MED_SHPNS_SCL_COR_s *HiMedShpnsSclCor)
{
    (void)ituner_memcpy(HiMedShpnsSclCor, &GData.HiMedShpnsSclCor, sizeof(AMBA_IK_HI_MED_SHPNS_SCL_COR_s));
}

void AmbaItuner_Set_HiSelect(const AMBA_IK_HI_SELECT_s *HiSelect)
{
    (void)ituner_memcpy(&GData.HiSelect, HiSelect, sizeof(GData.HiSelect));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_SELECT);
}

void AmbaItuner_Get_HiSelect(AMBA_IK_HI_SELECT_s *HiSelect)
{
    (void)ituner_memcpy(HiSelect, &GData.HiSelect, sizeof(AMBA_IK_HI_SELECT_s));
}

void AmbaItuner_Set_HiLumaBlend(const AMBA_IK_HI_LUMA_BLEND_s *HiLumaBlend)
{
    (void)ituner_memcpy(&GData.HiLumaBlend, HiLumaBlend, sizeof(GData.HiLumaBlend));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_LUMA_BLEND);
}

void AmbaItuner_Get_HiLumaBlend(AMBA_IK_HI_LUMA_BLEND_s *HiLumaBlend)
{
    (void)ituner_memcpy(HiLumaBlend, &GData.HiLumaBlend, sizeof(AMBA_IK_HI_LUMA_BLEND_s));
}

void AmbaItuner_Set_HiLiCombine(const AMBA_IK_HILI_COMBINE_s *HiLiCombine)
{
    (void)ituner_memcpy(&GData.HiLiCombine, HiLiCombine, sizeof(GData.HiLiCombine));
    AmbaItuner_Set_Filter_Valid(ITUNER_HILI_COMBINE);
}

void AmbaItuner_Get_HiLiCombine(AMBA_IK_HILI_COMBINE_s *HiLiCombine)
{
    (void)ituner_memcpy(HiLiCombine, &GData.HiLiCombine, sizeof(AMBA_IK_HILI_COMBINE_s));
}

void AmbaItuner_Set_HiMidHighFreqRecover(const AMBA_IK_HI_MID_HIGH_FREQ_RECOVER_s *HiMidHighFreqRecover)
{
    (void)ituner_memcpy(&GData.HiMidHighFreqRecover, HiMidHighFreqRecover, sizeof(GData.HiMidHighFreqRecover));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_MED_HIGH_FREQ_RECOVER);
}

void AmbaItuner_Get_HiMidHighFreqRecover(AMBA_IK_HI_MID_HIGH_FREQ_RECOVER_s *HiMidHighFreqRecover)
{
    (void)ituner_memcpy(HiMidHighFreqRecover, &GData.HiMidHighFreqRecover, sizeof(AMBA_IK_HI_MID_HIGH_FREQ_RECOVER_s));
}

void AmbaItuner_Set_HiLiShpnsBoth(const AMBA_IK_HILI_SHPNS_BOTH_s *HiLiShpnsBoth)
{
    (void)ituner_memcpy(&GData.HiLiShpnsBoth, HiLiShpnsBoth, sizeof(GData.HiLiShpnsBoth));
    AmbaItuner_Set_Filter_Valid(ITUNER_HILI_SHARPEN_BOTH);
}

void AmbaItuner_Get_HiLiShpnsBoth(AMBA_IK_HILI_SHPNS_BOTH_s *HiLiShpnsBoth)
{
    (void)ituner_memcpy(HiLiShpnsBoth, &GData.HiLiShpnsBoth, sizeof(AMBA_IK_HILI_SHPNS_BOTH_s));
}

void AmbaItuner_Set_HiLiShpnsNoise(const AMBA_IK_HILI_SHPNS_NOISE_s *HiLiShpnsNoise)
{
    (void)ituner_memcpy(&GData.HiLiShpnsNoise, HiLiShpnsNoise, sizeof(GData.HiLiShpnsNoise));
    AmbaItuner_Set_Filter_Valid(ITUNER_HILi_SHARPEN_NOISE);
}

void AmbaItuner_Get_HiLiShpnsNoise(AMBA_IK_HILI_SHPNS_NOISE_s *HiLiShpnsNoise)
{
    (void)ituner_memcpy(HiLiShpnsNoise, &GData.HiLiShpnsNoise, sizeof(AMBA_IK_HILI_SHPNS_NOISE_s));
}

void AmbaItuner_Set_HiLiShpnsFir(const AMBA_IK_HILI_SHPNS_FIR_s *HiLiShpnsFir)
{
    (void)ituner_memcpy(&GData.HiLiShpnsFir, HiLiShpnsFir, sizeof(GData.HiLiShpnsFir));
    AmbaItuner_Set_Filter_Valid(ITUNER_HILI_FIR);
}

void AmbaItuner_Get_HiLiShpnsFir(AMBA_IK_HILI_SHPNS_FIR_s *HiLiShpnsFir)
{
    (void)ituner_memcpy(HiLiShpnsFir, &GData.HiLiShpnsFir, sizeof(AMBA_IK_HILI_SHPNS_FIR_s));
}

void AmbaItuner_Set_HiLiShpnsCoring(const AMBA_IK_HILI_SHPNS_CORING_s *HiLiShpnsCoring)
{
    (void)ituner_memcpy(&GData.HiLiShpnsCoring, HiLiShpnsCoring, sizeof(GData.HiLiShpnsCoring));
    AmbaItuner_Set_Filter_Valid(ITUNER_HILI_CORING);
}

void AmbaItuner_Get_HiLiShpnsCoring(AMBA_IK_HILI_SHPNS_CORING_s *HiLiShpnsCoring)
{
    (void)ituner_memcpy(HiLiShpnsCoring, &GData.HiLiShpnsCoring, sizeof(AMBA_IK_HILI_SHPNS_CORING_s));
}

void AmbaItuner_Set_HiLiShpnsCorIdxScl(const AMBA_IK_HILI_SHPNS_COR_IDX_SCL_s *HiLiShpnsCorIdxScl)
{
    (void)ituner_memcpy(&GData.HiLiShpnsCorIdxScl, HiLiShpnsCorIdxScl, sizeof(GData.HiLiShpnsCorIdxScl));
    AmbaItuner_Set_Filter_Valid(ITUNER_HILI_CORING_INDEX_SCALE);
}

void AmbaItuner_Get_HiLiShpnsCorIdxScl(AMBA_IK_HILI_SHPNS_COR_IDX_SCL_s *HiLiShpnsCorIdxScl)
{
    (void)ituner_memcpy(HiLiShpnsCorIdxScl, &GData.HiLiShpnsCorIdxScl, sizeof(AMBA_IK_HILI_SHPNS_COR_IDX_SCL_s));
}

void AmbaItuner_Set_HiLiShpnsMaxCorRst(const AMBA_IK_HILI_SHPNS_MAX_COR_RST_s *HiLiShpnsMaxCorRst)
{
    (void)ituner_memcpy(&GData.HiLiShpnsMaxCorRst, HiLiShpnsMaxCorRst, sizeof(GData.HiLiShpnsMaxCorRst));
    AmbaItuner_Set_Filter_Valid(ITUNER_HILI_MAX_CORING_RESULT);
}

void AmbaItuner_Get_HiLiShpnsMaxCorRst(AMBA_IK_HILI_SHPNS_MAX_COR_RST_s *HiLiShpnsMaxCorRst)
{
    (void)ituner_memcpy(HiLiShpnsMaxCorRst, &GData.HiLiShpnsMaxCorRst, sizeof(AMBA_IK_HILI_SHPNS_MAX_COR_RST_s));
}

void AmbaItuner_Set_HiLiShpnsMinCorRst(const AMBA_IK_HILI_SHPNS_MIN_COR_RST_s *HiLiShpnsMinCorRst)
{
    (void)ituner_memcpy(&GData.HiLiShpnsMinCorRst, HiLiShpnsMinCorRst, sizeof(GData.HiLiShpnsMinCorRst));
    AmbaItuner_Set_Filter_Valid(ITUNER_HILI_MIN_CORING_RESULT);
}

void AmbaItuner_Get_HiLiShpnsMinCorRst(AMBA_IK_HILI_SHPNS_MIN_COR_RST_s *HiLiShpnsMinCorRst)
{
    (void)ituner_memcpy(HiLiShpnsMinCorRst, &GData.HiLiShpnsMinCorRst, sizeof(AMBA_IK_HILI_SHPNS_MIN_COR_RST_s));
}

void AmbaItuner_Set_HiLiShpnsSclCor(const AMBA_IK_HILI_SHPNS_SCL_COR_s *HiLiShpnsSclCor)
{
    (void)ituner_memcpy(&GData.HiLiShpnsSclCor, HiLiShpnsSclCor, sizeof(GData.HiLiShpnsSclCor));
    AmbaItuner_Set_Filter_Valid(ITUNER_HILI_SCALE_CORING);
}

void AmbaItuner_Get_HiLiShpnsSclCor(AMBA_IK_HILI_SHPNS_SCL_COR_s *HiLiShpnsSclCor)
{
    (void)ituner_memcpy(HiLiShpnsSclCor, &GData.HiLiShpnsSclCor, sizeof(AMBA_IK_HILI_SHPNS_SCL_COR_s));
}

void AmbaItuner_Set_Li2Asf(const AMBA_IK_LI2_ASF_s *Li2Asf)
{
    (void)ituner_memcpy(&GData.Li2Asf, Li2Asf, sizeof(GData.Li2Asf));
    AmbaItuner_Set_Filter_Valid(ITUNER_LI2_LUMA_ASF);
}

void AmbaItuner_Get_Li2Asf(AMBA_IK_LI2_ASF_s *Li2Asf)
{
    (void)ituner_memcpy(Li2Asf, &GData.Li2Asf, sizeof(AMBA_IK_LI2_ASF_s));
}

void AmbaItuner_Set_Li2AntiAliasing(const AMBA_IK_LI2_ANTI_ALIASING_s *Li2AntiAliasing)
{
    (void)ituner_memcpy(&GData.Li2AntiAliasing, Li2AntiAliasing, sizeof(GData.Li2AntiAliasing));
    AmbaItuner_Set_Filter_Valid(ITUNER_LI2_ANTI_ALIASING);
}

void AmbaItuner_Get_Li2AntiAliasing(AMBA_IK_LI2_ANTI_ALIASING_s *Li2AntiAliasing)
{
    (void)ituner_memcpy(Li2AntiAliasing, &GData.Li2AntiAliasing, sizeof(AMBA_IK_LI2_ANTI_ALIASING_s));
}

void AmbaItuner_Set_Li2DynamicBpc(const AMBA_IK_LI2_DYNAMIC_BPC_s *Li2DynamicBpc)
{
    (void)ituner_memcpy(&GData.Li2DynamicBpc, Li2DynamicBpc, sizeof(GData.Li2DynamicBpc));
    AmbaItuner_Set_Filter_Valid(ITUNER_LI2_AUTO_BAD_PIXEL_CORRECTION);
}

void AmbaItuner_Get_Li2DynamicBpc(AMBA_IK_LI2_DYNAMIC_BPC_s *Li2DynamicBpc)
{
    (void)ituner_memcpy(Li2DynamicBpc, &GData.Li2DynamicBpc, sizeof(AMBA_IK_LI2_DYNAMIC_BPC_s));
}

void AmbaItuner_Set_Li2CfaLeakageFilter(const AMBA_IK_LI2_CFA_LEAKAGE_FILTER_s *Li2CfaLeakageFilter)
{
    (void)ituner_memcpy(&GData.Li2CfaLeakageFilter, Li2CfaLeakageFilter, sizeof(GData.Li2CfaLeakageFilter));
    AmbaItuner_Set_Filter_Valid(ITUNER_LI2_CFA_LEAKAGE_FILTER);
}

void AmbaItuner_Get_Li2CfaLeakageFilter(AMBA_IK_LI2_CFA_LEAKAGE_FILTER_s *Li2CfaLeakageFilter)
{
    (void)ituner_memcpy(Li2CfaLeakageFilter, &GData.Li2CfaLeakageFilter, sizeof(AMBA_IK_LI2_CFA_LEAKAGE_FILTER_s));
}

void AmbaItuner_Set_Li2CfaNoiseFilter(const AMBA_IK_LI2_CFA_NOISE_FILTER_s *Li2CfaNoiseFilter)
{
    (void)ituner_memcpy(&GData.Li2CfaNoiseFilter, Li2CfaNoiseFilter, sizeof(GData.Li2CfaNoiseFilter));
    AmbaItuner_Set_Filter_Valid(ITUNER_LI2_CFA_NOISE_FILTER);
}

void AmbaItuner_Get_Li2CfaNoiseFilter(AMBA_IK_LI2_CFA_NOISE_FILTER_s *Li2CfaNoiseFilter)
{
    (void)ituner_memcpy(Li2CfaNoiseFilter, &GData.Li2CfaNoiseFilter, sizeof(AMBA_IK_LI2_CFA_NOISE_FILTER_s));
}

void AmbaItuner_Set_Li2Demosaic(const AMBA_IK_LI2_DEMOSAIC_s *Li2Demosaic)
{
    (void)ituner_memcpy(&GData.Li2Demosaic, Li2Demosaic, sizeof(GData.Li2Demosaic));
    AmbaItuner_Set_Filter_Valid(ITUNRE_LI2_DEMOSAIC_FILTER);
}

void AmbaItuner_Get_Li2Demosaic(AMBA_IK_LI2_DEMOSAIC_s *Li2Demosaic)
{
    (void)ituner_memcpy(Li2Demosaic, &GData.Li2Demosaic, sizeof(AMBA_IK_LI2_DEMOSAIC_s));
}

void AmbaItuner_Set_Li2GrGbMismatch(const AMBA_IK_LI2_GRGB_MISMATCH_s *Li2GrGbMismatch)
{
    (void)ituner_memcpy(&GData.Li2GrGbMismatch, Li2GrGbMismatch, sizeof(GData.Li2GrGbMismatch));
    AmbaItuner_Set_Filter_Valid(ITUNER_LI2_GB_GR_MISMATCH);
}

void AmbaItuner_Get_Li2GrGbMismatch(AMBA_IK_LI2_GRGB_MISMATCH_s *Li2GrGbMismatch)
{
    (void)ituner_memcpy(Li2GrGbMismatch, &GData.Li2GrGbMismatch, sizeof(AMBA_IK_LI2_GRGB_MISMATCH_s));
}

void AmbaItuner_Set_Li2ShpnsBoth(const AMBA_IK_LI2_SHPNS_BOTH_s *Li2ShpnsBoth)
{
    (void)ituner_memcpy(&GData.Li2ShpnsBoth, Li2ShpnsBoth, sizeof(GData.Li2ShpnsBoth));
    AmbaItuner_Set_Filter_Valid(ITUNER_LI2_SHARPEN_BOTH);
}

void AmbaItuner_Get_Li2ShpnsBoth(AMBA_IK_LI2_SHPNS_BOTH_s *Li2ShpnsBoth)
{
    (void)ituner_memcpy(Li2ShpnsBoth, &GData.Li2ShpnsBoth, sizeof(AMBA_IK_LI2_SHPNS_BOTH_s));
}

void AmbaItuner_Set_Li2ShpnsNoise(const AMBA_IK_LI2_SHPNS_NOISE_s *Li2ShpnsNoise)
{
    (void)ituner_memcpy(&GData.Li2ShpnsNoise, Li2ShpnsNoise, sizeof(GData.Li2ShpnsNoise));
    AmbaItuner_Set_Filter_Valid(ITUNER_LI2_SHARPEN_NOISE);
}

void AmbaItuner_Get_Li2ShpnsNoise(AMBA_IK_LI2_SHPNS_NOISE_s *Li2ShpnsNoise)
{
    (void)ituner_memcpy(Li2ShpnsNoise, &GData.Li2ShpnsNoise, sizeof(AMBA_IK_LI2_SHPNS_NOISE_s));
}

void AmbaItuner_Set_Li2ShpnsFir(const AMBA_IK_LI2_SHPNS_FIR_s *Li2ShpnsFir)
{
    (void)ituner_memcpy(&GData.Li2ShpnsFir, Li2ShpnsFir, sizeof(GData.Li2ShpnsFir));
    AmbaItuner_Set_Filter_Valid(ITUNER_LI2_FIR);
}

void AmbaItuner_Get_Li2ShpnsFir(AMBA_IK_LI2_SHPNS_FIR_s *Li2ShpnsFir)
{
    (void)ituner_memcpy(Li2ShpnsFir, &GData.Li2ShpnsFir, sizeof(AMBA_IK_LI2_SHPNS_FIR_s));
}

void AmbaItuner_Set_Li2ShpnsCoring(const AMBA_IK_LI2_SHPNS_CORING_s *Li2ShpnsCoring)
{
    (void)ituner_memcpy(&GData.Li2ShpnsCoring, Li2ShpnsCoring, sizeof(GData.Li2ShpnsCoring));
    AmbaItuner_Set_Filter_Valid(ITUNER_LI2_CORING);
}

void AmbaItuner_Get_Li2ShpnsCoring(AMBA_IK_LI2_SHPNS_CORING_s *Li2ShpnsCoring)
{
    (void)ituner_memcpy(Li2ShpnsCoring, &GData.Li2ShpnsCoring, sizeof(AMBA_IK_LI2_SHPNS_CORING_s));
}

void AmbaItuner_Set_Li2ShpnsCorIdxScl(const AMBA_IK_LI2_SHPNS_COR_IDX_SCL_s *Li2ShpnsCorIdxScl)
{
    (void)ituner_memcpy(&GData.Li2ShpnsCorIdxScl, Li2ShpnsCorIdxScl, sizeof(GData.Li2ShpnsCorIdxScl));
    AmbaItuner_Set_Filter_Valid(ITUNER_LI2_CORING_INDEX_SCALE);
}

void AmbaItuner_Get_Li2ShpnsCorIdxScl(AMBA_IK_LI2_SHPNS_COR_IDX_SCL_s *Li2ShpnsCorIdxScl)
{
    (void)ituner_memcpy(Li2ShpnsCorIdxScl, &GData.Li2ShpnsCorIdxScl, sizeof(AMBA_IK_LI2_SHPNS_COR_IDX_SCL_s));
}

void AmbaItuner_Set_Li2ShpnsMaxCorRst(const AMBA_IK_LI2_SHPNS_MAX_COR_RST_s *Li2ShpnsMaxCorRst)
{
    (void)ituner_memcpy(&GData.Li2ShpnsMaxCorRst, Li2ShpnsMaxCorRst, sizeof(GData.Li2ShpnsMaxCorRst));
    AmbaItuner_Set_Filter_Valid(ITUNER_LI2_MAX_CORING_RESULT);
}

void AmbaItuner_Get_Li2ShpnsMaxCorRst(AMBA_IK_LI2_SHPNS_MAX_COR_RST_s *Li2ShpnsMaxCorRst)
{
    (void)ituner_memcpy(Li2ShpnsMaxCorRst, &GData.Li2ShpnsMaxCorRst, sizeof(AMBA_IK_LI2_SHPNS_MAX_COR_RST_s));
}

void AmbaItuner_Set_Li2ShpnsMinCorRst(const AMBA_IK_LI2_SHPNS_MIN_COR_RST_s *Li2ShpnsMinCorRst)
{
    (void)ituner_memcpy(&GData.Li2ShpnsMinCorRst, Li2ShpnsMinCorRst, sizeof(GData.Li2ShpnsMinCorRst));
    AmbaItuner_Set_Filter_Valid(ITUNER_LI2_MIN_CORING_RESULT);
}

void AmbaItuner_Get_Li2ShpnsMinCorRst(AMBA_IK_LI2_SHPNS_MIN_COR_RST_s *Li2ShpnsMinCorRst)
{
    (void)ituner_memcpy(Li2ShpnsMinCorRst, &GData.Li2ShpnsMinCorRst, sizeof(AMBA_IK_LI2_SHPNS_MIN_COR_RST_s));
}

void AmbaItuner_Set_Li2ShpnsSclCor(const AMBA_IK_LI2_SHPNS_SCL_COR_s *Li2ShpnsSclCor)
{
    (void)ituner_memcpy(&GData.Li2ShpnsSclCor, Li2ShpnsSclCor, sizeof(GData.Li2ShpnsSclCor));
    AmbaItuner_Set_Filter_Valid(ITUNER_LI2_SCALE_CORING);
}

void AmbaItuner_Get_Li2ShpnsSclCor(AMBA_IK_LI2_SHPNS_SCL_COR_s *Li2ShpnsSclCor)
{
    (void)ituner_memcpy(Li2ShpnsSclCor, &GData.Li2ShpnsSclCor, sizeof(AMBA_IK_LI2_SHPNS_SCL_COR_s));
}

void AmbaItuner_Set_HiNonsmoothDetect(const AMBA_IK_HI_NONSMOOTH_DETECT_s *HiNonsmoothDetect)
{
    (void)ituner_memcpy(&GData.HiNonsmoothDetect, HiNonsmoothDetect, sizeof(GData.HiNonsmoothDetect));
    AmbaItuner_Set_Filter_Valid(ITUNER_HI_NONSMOOTH_DETECT);
}

void AmbaItuner_Get_HiNonsmoothDetect(AMBA_IK_HI_NONSMOOTH_DETECT_s *HiNonsmoothDetect)
{
    (void)ituner_memcpy(HiNonsmoothDetect, &GData.HiNonsmoothDetect, sizeof(AMBA_IK_HI_NONSMOOTH_DETECT_s));
}

static INT32 AmbaItuner_Check_Reg(UINT32 FILTER_ID, INT32 Special_Use)
{
    INT32 ret = 0;
    if (Ituner_Valid_Filter[FILTER_ID] == 1U) {
        ituner_print_str_5("[Execute] %s", AmbaItuner_Get_Filter_Name(FILTER_ID), DC_S, DC_S, DC_S, DC_S);
        ret = 1;
    } else if ((Ituner_Valid_Filter[FILTER_ID] == 0U) && (Special_Use == SPECIAL_USE_SKIP)) {
        ituner_print_str_5("[Skip] %s", AmbaItuner_Get_Filter_Name(FILTER_ID), DC_S, DC_S, DC_S, DC_S);
    } else {
        ituner_print_str_5("[Without] %s", AmbaItuner_Get_Filter_Name(FILTER_ID), DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

/* iTuner external API */
INT32 AmbaItuner_Init(const AmbaItuner_Config_t *Config)
{
    (void)ituner_memcpy(&Ituner_Hook_Func, &Config->Hook_Func, sizeof(Ituner_Hook_Func));
    AmbaItuner_Internal_Init();
    AmbaItuner_System_Init();
    AmbaItuner_GData_Init();

    //pGCalib_Table = (ITUNER_Calib_Table_s *) Config->pWorkingBuffer;
    (void)ituner_memcpy(&pGCalib_Table, &Config->pWorkingBuffer, sizeof(void*));

    pGCalib_Table->pVignetteMap = &GData.VignetteCompensation.VignetteInfo.CalibVignetteInfo.VigGainTblR[0][0];

    return 0;
}

static INT32 AmbaItuner_VideoHdrExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    // FIXME:
    UINT32 rval;
    UINT32 ret = 0;
    if (AmbaItuner_Check_Reg(ITUNER_VHDR_BLEND, SPECIAL_USE_NONE) > 0) {
        rval= AmbaIK_SetHdrBlend(pMode, &GData.VideoHdr.HdrBlend);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_VHDR_RAW_INFO, SPECIAL_USE_NONE) > 0) {
        rval= AmbaIK_SetHdrRawOffset(pMode, &GData.VideoHdr.RawInfo.Offset);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
    }
    if (SystemInfo.NumberOfExposures == 1U) {
        // misraC fix
    } else if (SystemInfo.NumberOfExposures == 2U) {
        if (AmbaItuner_Check_Reg(ITUNER_FRONT_END_WB_GAIN_EXP1, SPECIAL_USE_NONE) > 0) {
            rval= AmbaIK_SetFeWbGain(pMode, &GData.FrontEndWbGain[1], 1UL);
            if ((INT32)rval != 0) {
                ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            ret |= rval;
        }
        if (AmbaItuner_Check_Reg(ITUNER_STATIC_BLACK_LEVEL_EXP1, SPECIAL_USE_NONE) > 0) {
            rval= AmbaIK_SetFeStaticBlc(pMode, &GData.StaticBlackLevel[1], 1UL);
            if ((INT32)rval != 0) {
                ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            ret |= rval;
        }
    } else if (SystemInfo.NumberOfExposures == 3U) {
        if (AmbaItuner_Check_Reg(ITUNER_FRONT_END_WB_GAIN_EXP1, SPECIAL_USE_NONE) > 0) {
            rval= AmbaIK_SetFeWbGain(pMode, &GData.FrontEndWbGain[1], 1UL);
            if ((INT32)rval != 0) {
                ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            ret |= rval;
        }
        if (AmbaItuner_Check_Reg(ITUNER_FRONT_END_WB_GAIN_EXP2, SPECIAL_USE_NONE) > 0) {
            rval= AmbaIK_SetFeWbGain(pMode, &GData.FrontEndWbGain[2], 2UL);
            if ((INT32)rval != 0) {
                ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            ret |= rval;
        }
        if (AmbaItuner_Check_Reg(ITUNER_STATIC_BLACK_LEVEL_EXP1, SPECIAL_USE_NONE) > 0) {
            rval= AmbaIK_SetFeStaticBlc(pMode, &GData.StaticBlackLevel[1], 1UL);
            if ((INT32)rval != 0) {
                ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            ret |= rval;
        }
        if (AmbaItuner_Check_Reg(ITUNER_STATIC_BLACK_LEVEL_EXP2, SPECIAL_USE_NONE) > 0) {
            rval= AmbaIK_SetFeStaticBlc(pMode, &GData.StaticBlackLevel[2], 2UL);
            if ((INT32)rval != 0) {
                ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            ret |= rval;
        }
    } else {/*misrac 15.7*/;};
    return (INT32)ret;
}

static INT32 AmbaItuner_SystemExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;
    AMBA_IK_ABILITY_s Ability = {0};

    (void)AmbaIK_GetContextAbility(pMode, &Ability);

    if (Ability.Pipe== AMBA_IK_PIPE_VIDEO) {
        if (SystemInfo.HSubSampleFactorNum == 0U) {
            ituner_print_uint32_5("[Error] HSubSampleFactorNum should not be %d", SystemInfo.HSubSampleFactorNum, DC_U, DC_U, DC_U, DC_U);
            SystemInfo.HSubSampleFactorNum = 1U;
        }
        if (SystemInfo.HSubSampleFactorDen == 0U) {
            ituner_print_uint32_5("[Error] HSubSampleFactorDen should not be %d", SystemInfo.HSubSampleFactorDen, DC_U, DC_U, DC_U, DC_U);
            SystemInfo.HSubSampleFactorDen = 1U;
        }
        if (SystemInfo.VSubSampleFactorNum == 0U) {
            ituner_print_uint32_5("[Error] VSubSampleFactorNum should not be %d", SystemInfo.VSubSampleFactorNum, DC_U, DC_U, DC_U, DC_U);
            SystemInfo.VSubSampleFactorNum = 1U;
        }
        if (SystemInfo.VSubSampleFactorDen == 0U) {
            ituner_print_uint32_5("[Error] VSubSampleFactorDen should not be %d", SystemInfo.VSubSampleFactorDen, DC_U, DC_U, DC_U, DC_U);
            SystemInfo.VSubSampleFactorDen = 1U;
        }
    }

    if (AmbaItuner_Check_Reg(ITUNER_SYSTEM_INFO, SPECIAL_USE_NONE) > 0) {
        AMBA_IK_WINDOW_SIZE_INFO_s WindowSizeInfo;
        AMBA_IK_CFA_WINDOW_SIZE_INFO_s CfaWindowSizeInfo;
        (void)AmbaIK_GetWindowSizeInfo(pMode, &WindowSizeInfo);
        (void)AmbaIK_GetCfaWindowSizeInfo(pMode, &CfaWindowSizeInfo);
        WindowSizeInfo.VinSensor.StartX                 = SystemInfo.RawStartX;
        WindowSizeInfo.VinSensor.StartY                 = SystemInfo.RawStartY;
        WindowSizeInfo.VinSensor.Width                  = SystemInfo.RawWidth;
        WindowSizeInfo.VinSensor.Height                 = SystemInfo.RawHeight;
        WindowSizeInfo.VinSensor.HSubSample.FactorNum   = SystemInfo.HSubSampleFactorNum;
        WindowSizeInfo.VinSensor.HSubSample.FactorDen   = SystemInfo.HSubSampleFactorDen;
        WindowSizeInfo.VinSensor.VSubSample.FactorNum   = SystemInfo.VSubSampleFactorNum;
        WindowSizeInfo.VinSensor.VSubSample.FactorDen   = SystemInfo.VSubSampleFactorDen;
        //WindowSizeInfo.CfaWindow.Width                  = (SystemInfo.CfaWidth==0UL)?SystemInfo.RawWidth:SystemInfo.CfaWidth;
        //WindowSizeInfo.CfaWindow.Height                 = (SystemInfo.CfaHeight==0UL)?SystemInfo.RawHeight:SystemInfo.CfaHeight;
        WindowSizeInfo.Main.Width                       = SystemInfo.MainWidth;
        WindowSizeInfo.Main.Height                      = SystemInfo.MainHeight;
        CfaWindowSizeInfo.Enable                        = SystemInfo.CfaWindowEnable;
        CfaWindowSizeInfo.Cfa.Width                     = SystemInfo.CfaWidth;
        CfaWindowSizeInfo.Cfa.Height                    = SystemInfo.CfaHeight;
        rval= AmbaIK_SetWindowSizeInfo(pMode, &WindowSizeInfo);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;

        rval = AmbaIK_SetCfaWindowSizeInfo(pMode, &CfaWindowSizeInfo);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;

        if (Ability.Pipe == AMBA_IK_PIPE_VIDEO) {
            AMBA_IK_CE_EXT_HDS_s CeUseExternalHds;
            (void)AmbaIK_GetCeUseExtHds(pMode, &CeUseExternalHds);

            CeUseExternalHds.Mode = SystemInfo.UseExternalHdsMode;
            rval = AmbaIK_SetCeUseExtHds(pMode, &CeUseExternalHds);
            if ((INT32)rval != 0) {
                ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            ret |= rval;
        }
    }
    return (INT32)ret;
}

/*To reduce coverity complexity*/
static inline void check_rval_not_equal_0(const UINT32 rval, const char *function_name, const UINT32 line)
{
    if(rval!=0U) {
        ituner_print_str_5("%s() call Fail", function_name, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", line, DC_U, DC_U, DC_U, DC_U);
    }
}

static inline UINT32 AmbaItuner_Raw2yuvCalibExecute_part2(const AMBA_IK_MODE_CFG_s *pMode, INT8 check)
{
    UINT32 rval = 0;
    Ituner_Ext_File_Param_s Ext_File_Param;
    //const UINT8 *pUint8;
    //INT16 *pInt16;
    if (AmbaItuner_Check_Reg(ITUNER_CHROMA_ABERRATION_INFO_INTERNAL, SPECIAL_USE_NONE) > 0) {
#if 0
        extern UINT32 AmbaIK_SetCaWarpInternal(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CAWARP_INTERNAL_INFO_s *pCawarpInternal);
        rval |= AmbaIK_SetCawarpEnb(pMode, 1);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        // Horizontal part
        if (GData.ChromaAberrationInfoInternal.HorzWarpEnable > 0U) {
            GData.CAInternal.HorzWarpEnable = GData.ChromaAberrationInfoInternal.HorzWarpEnable;
            GData.CAInternal.HorzPassGridArrayWidth = GData.ChromaAberrationInfoInternal.HorzPassGridArrayWidth;
            GData.CAInternal.HorzPassGridArrayHeight = GData.ChromaAberrationInfoInternal.HorzPassGridArrayHeight;
            GData.CAInternal.HorzPassHorzGridSpacingExponent = GData.ChromaAberrationInfoInternal.HorzPassHorzGridSpacingExponent;
            GData.CAInternal.HorzPassVertGridSpacingExponent = GData.ChromaAberrationInfoInternal.HorzPassVertGridSpacingExponent;
            (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
            Ext_File_Param.Internal_Ca_Horizontal_Table_Load_Param.Address = &GData.CAInternal.WarpHorzTableRed[0];
            Ext_File_Param.Internal_Ca_Horizontal_Table_Load_Param.Max_Size = (UINT32)(sizeof(INT16)*32UL*48UL);
            if (Ituner_Hook_Func.Load_Data(FILE_EXT_INTERNAL_CA_RED_HORIZONTAL_TABLE, &Ext_File_Param) < 0) {
                ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_INTERNAL_CA_RED_HORIZONTAL_TABLE) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
            Ext_File_Param.Internal_Ca_Horizontal_Table_Load_Param.Address = &GData.CAInternal.WarpHorzTableBlue[0];
            Ext_File_Param.Internal_Ca_Horizontal_Table_Load_Param.Max_Size = (UINT32)(sizeof(INT16)*32UL*48UL);
            if (Ituner_Hook_Func.Load_Data(FILE_EXT_INTERNAL_CA_BLUE_HORIZONTAL_TABLE, &Ext_File_Param) < 0) {
                ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_INTERNAL_CA_BLUE_HORIZONTAL_TABLE) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
        } else {
            //amba_ik_system_memset(&GData.CAInternal.warp_horztable_red[0], 0x0, sizeof(INT16)*32*48);
        }
        // Vertical part
        if (GData.ChromaAberrationInfoInternal.VertWarpEnable > 0U) {
            GData.CAInternal.VertWarpEnable = GData.ChromaAberrationInfoInternal.VertWarpEnable;
            GData.CAInternal.VertPassGridArrayWidth = GData.ChromaAberrationInfoInternal.VertPassGridArrayWidth;
            GData.CAInternal.VertPassGridArrayHeight = GData.ChromaAberrationInfoInternal.VertPassGridArrayHeight;
            GData.CAInternal.VertPassHorzGridSpacingExponent = GData.ChromaAberrationInfoInternal.VertPassHorzGridSpacingExponent;
            GData.CAInternal.VertPassVertGridSpacingExponent = GData.ChromaAberrationInfoInternal.VertPassVertGridSpacingExponent;
            (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
            Ext_File_Param.Internal_Ca_Vertical_Table_Load_Param.Address = &GData.CAInternal.WarpVertTableRed[0];
            Ext_File_Param.Internal_Ca_Vertical_Table_Load_Param.Max_Size = (UINT32)(sizeof(INT16)*32UL*48UL);
            if (Ituner_Hook_Func.Load_Data(FILE_EXT_INTERNAL_CA_RED_VERTICAL_TABLE, &Ext_File_Param) < 0) {
                ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_INTERNAL_CA_RED_VERTICAL_TABLE) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
            Ext_File_Param.Internal_Ca_Vertical_Table_Load_Param.Address = &GData.CAInternal.WarpVertTableBlue[0];
            Ext_File_Param.Internal_Ca_Vertical_Table_Load_Param.Max_Size = (UINT32)(sizeof(INT16)*32UL*48UL);
            if (Ituner_Hook_Func.Load_Data(FILE_EXT_INTERNAL_CA_BLUE_VERTICAL_TABLE, &Ext_File_Param) < 0) {
                ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_INTERNAL_CA_BLUE_VERTICAL_TABLE) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
        } else {
            //amba_ik_system_memset(&GData.CAInternal.warp_verttable_red[0], 0x0, sizeof(INT16)*32*48);
        }

        rval |= AmbaIK_SetCaWarpInternal(pMode, &GData.CAInternal);
        check_rval_not_equal_0(rval, __func__, __LINE__);
#endif
    } else if (AmbaItuner_Check_Reg(ITUNER_CHROMA_ABERRATION_INFO, SPECIAL_USE_NONE) > 0) {
        rval = AmbaIK_SetCawarpEnb(pMode, (UINT32)GData.ChromaAberrationInfo.Enable);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        if (GData.ChromaAberrationInfo.Enable > 0U) {
            AMBA_IK_CAWARP_INFO_s CaCalibWarpInfo;
            const UINT8 *pCawarpGrid;

            CaCalibWarpInfo.Version = GData.ChromaAberrationInfo.CalibVersion;
            CaCalibWarpInfo.HorGridNum = GData.ChromaAberrationInfo.HorizontalGridNumber;
            CaCalibWarpInfo.VerGridNum = GData.ChromaAberrationInfo.VerticalGridNumber;
            CaCalibWarpInfo.TileHeightExp = GData.ChromaAberrationInfo.TileHeightExponent;
            CaCalibWarpInfo.TileWidthExp = GData.ChromaAberrationInfo.TileWidthExponent;
            CaCalibWarpInfo.VinSensorGeo.StartX = GData.ChromaAberrationInfo.VinSensorStartX;
            CaCalibWarpInfo.VinSensorGeo.StartY = GData.ChromaAberrationInfo.VinSensorStartY;
            CaCalibWarpInfo.VinSensorGeo.Width = GData.ChromaAberrationInfo.VinSensorWidth;
            CaCalibWarpInfo.VinSensorGeo.Height = GData.ChromaAberrationInfo.VinSensorHeight;
            CaCalibWarpInfo.VinSensorGeo.HSubSample.FactorNum = GData.ChromaAberrationInfo.VinSensorHSubSampleFactorNum;
            CaCalibWarpInfo.VinSensorGeo.HSubSample.FactorDen = GData.ChromaAberrationInfo.VinSensorHSubSampleFactorDen;
            CaCalibWarpInfo.VinSensorGeo.VSubSample.FactorNum = GData.ChromaAberrationInfo.VinSensorVSubSampleFactorNum;
            CaCalibWarpInfo.VinSensorGeo.VSubSample.FactorDen = GData.ChromaAberrationInfo.VinSensorVSubSampleFactorDen;
            CaCalibWarpInfo.Enb2StageCompensation = (UINT8)GData.ChromaAberrationInfo.Enb2StageCompensation;

            pCawarpGrid = &pGCalib_Table->CawarpRedGrid[0];
            (void)ituner_memcpy (&CaCalibWarpInfo.pCawarpRed, &pCawarpGrid, sizeof(void *));
            (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
            Ext_File_Param.Ca_Table_Load_Param.Address = CaCalibWarpInfo.pCawarpRed;
            Ext_File_Param.Ca_Table_Load_Param.Size = AmbaItuner_CalCaTableSize(CaCalibWarpInfo.HorGridNum, CaCalibWarpInfo.VerGridNum);
            if (Ituner_Hook_Func.Load_Data(FILE_EXT_CA_RED_TABLE, &Ext_File_Param) < 0) {
                ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_CA_RED_TABLE) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }

            pCawarpGrid = &pGCalib_Table->CawarpBlueGrid[0];
            (void)ituner_memcpy (&CaCalibWarpInfo.pCawarpBlue, &pCawarpGrid, sizeof(void *));
            (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
            Ext_File_Param.Ca_Table_Load_Param.Address = CaCalibWarpInfo.pCawarpBlue;
            Ext_File_Param.Ca_Table_Load_Param.Size = AmbaItuner_CalCaTableSize(CaCalibWarpInfo.HorGridNum, CaCalibWarpInfo.VerGridNum);
            if (Ituner_Hook_Func.Load_Data(FILE_EXT_CA_BLUE_TABLE, &Ext_File_Param) < 0) {
                ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_CA_BLUE_TABLE) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            rval |= AmbaIK_SetCawarpInfo(pMode, &CaCalibWarpInfo);
            check_rval_not_equal_0(rval, __func__, __LINE__);
        } else {
            // misraC
        }
    } else {/*15.7*/;};

    if (AmbaItuner_Check_Reg(ITUNER_WARP_COMPENSATION_DZOOM_INTERNAL, SPECIAL_USE_NONE) > 0) {
        // Set for warp bypass part
        // Bypass warp horizontal part
#if 0
        if (GData.WarpCompensationDzoomInternal.Enable > 0U) {
            GData.WarpInternal.Enable = GData.WarpCompensationDzoomInternal.Enable;
            GData.WarpInternal.GridArrayWidth = GData.WarpCompensationDzoomInternal.GridArrayWidth;
            GData.WarpInternal.GridArrayHeight = GData.WarpCompensationDzoomInternal.GridArrayHeight;
            GData.WarpInternal.HorzGridSpacingExponent = GData.WarpCompensationDzoomInternal.HorzGridSpacingExponent;
            GData.WarpInternal.VertGridSpacingExponent = GData.WarpCompensationDzoomInternal.VertGridSpacingExponent;
            pUint8 = &pGCalib_Table->WarpGrid[0];
            (void)ituner_memcpy (&pInt16, &pUint8, sizeof(INT16 *));
            GData.WarpInternal.pWarpHorizontalTable = pInt16;
            {
                (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
                Ext_File_Param.Internal_Warp_Horizontal_Table_Load_Param.Address = pGCalib_Table->WarpGrid;
                Ext_File_Param.Internal_Warp_Horizontal_Table_Load_Param.Max_Size = sizeof(pGCalib_Table->WarpGrid);
                if (Ituner_Hook_Func.Load_Data(FILE_EXT_INTERNAL_WARP_HORIZONTAL_TABLE, &Ext_File_Param) < 0) {
                    ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_BYPASS_WARP_HORIZONTAL_TABLE) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                    ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
                }
            }
        } else {
            // misraC fix
        }

        // Bypass warp vertical part
        if (GData.WarpCompensationDzoomInternal.Enable > 0U) {
            GData.WarpInternal.VertWarpGridArrayWidth = GData.WarpCompensationDzoomInternal.VertWarpGridArrayWidth;
            GData.WarpInternal.VertWarpGridArrayHeight = GData.WarpCompensationDzoomInternal.VertWarpGridArrayHeight;
            GData.WarpInternal.VertWarpHorzGridSpacingExponent = GData.WarpCompensationDzoomInternal.VertWarpHorzGridSpacingExponent;
            GData.WarpInternal.VertWarpVertGridSpacingExponent = GData.WarpCompensationDzoomInternal.VertWarpVertGridSpacingExponent;
            pUint8 = (&pGCalib_Table->WarpGrid[(sizeof(pGCalib_Table->WarpGrid) >> 1)]);
            (void)ituner_memcpy (&pInt16, &pUint8, sizeof(INT16 *));
            GData.WarpInternal.pWarpVerticalTable = pInt16;
            {
                (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
                Ext_File_Param.Internal_Warp_Vertical_Table_Load_Param.Address = GData.WarpInternal.pWarpVerticalTable;
                Ext_File_Param.Internal_Warp_Vertical_Table_Load_Param.Max_Size = sizeof(pGCalib_Table->WarpGrid);
                if (Ituner_Hook_Func.Load_Data(FILE_EXT_INTERNAL_WARP_VERTICAL_TABLE, &Ext_File_Param) < 0) {
                    ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_BYPASS_WARP_VERITCAL_TABLE) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                    ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
                }
            }
        } else {
            // misraC fix
        }
        rval |= AmbaIK_SetWarpInternal(pMode, &GData.WarpInternal);
        check_rval_not_equal_0(rval, __func__, __LINE__);
#endif
    } else if (check > 0) { /*Misra C check*/
        AMBA_IK_WARP_INFO_s CalibWarpInfo;
        if (GData.WarpCompensation.Enable > 0U) {
            const UINT8 *pWarpGrid;
            rval |= AmbaIK_SetWarpEnb(pMode, 1);
            check_rval_not_equal_0(rval, __func__, __LINE__);
            CalibWarpInfo.Version = GData.WarpCompensation.CalibVersion;
            CalibWarpInfo.HorGridNum = (UINT32)GData.WarpCompensation.HorizontalGridNumber;
            CalibWarpInfo.VerGridNum = (UINT32)GData.WarpCompensation.VerticalGridNumber;
            CalibWarpInfo.TileWidthExp = (UINT32)GData.WarpCompensation.TileWidthExponent;
            CalibWarpInfo.TileHeightExp = (UINT32)GData.WarpCompensation.TileHeightExponent;
            CalibWarpInfo.VinSensorGeo.StartX = GData.WarpCompensation.VinSensorStartX;
            CalibWarpInfo.VinSensorGeo.StartY = GData.WarpCompensation.VinSensorStartY;
            CalibWarpInfo.VinSensorGeo.Width = GData.WarpCompensation.VinSensorWidth;
            CalibWarpInfo.VinSensorGeo.Height = GData.WarpCompensation.VinSensorHeight;
            CalibWarpInfo.VinSensorGeo.HSubSample.FactorNum = (UINT32)GData.WarpCompensation.VinSensorHSubSampleFactorNum;
            CalibWarpInfo.VinSensorGeo.HSubSample.FactorDen = (UINT32)GData.WarpCompensation.VinSensorHSubSampleFactorDen;
            CalibWarpInfo.VinSensorGeo.VSubSample.FactorNum = (UINT32)GData.WarpCompensation.VinSensorVSubSampleFactorNum;
            CalibWarpInfo.VinSensorGeo.VSubSample.FactorDen = (UINT32)GData.WarpCompensation.VinSensorVSubSampleFactorDen;
            CalibWarpInfo.Enb_2StageCompensation = (UINT8)GData.WarpCompensation.Enb2StageCompensation;

            //CalibWarpInfo.pWarp = &pGCalib_Table->WarpGrid[0];
            pWarpGrid = &pGCalib_Table->WarpGrid[0];
            (void)ituner_memcpy(&CalibWarpInfo.pWarp, &pWarpGrid, sizeof(AMBA_IK_GRID_POINT_s*));

            (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
            Ext_File_Param.Warp_Table_Load_Param.Address = CalibWarpInfo.pWarp;
            Ext_File_Param.Warp_Table_Load_Param.Size = AmbaItuner_CalWarpTableSize(CalibWarpInfo.HorGridNum, CalibWarpInfo.VerGridNum);
            if (Ituner_Hook_Func.Load_Data(FILE_EXT_WARP_TABLE, &Ext_File_Param) < 0) {
                ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_WARP_TABLE) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            rval |= AmbaIK_SetWarpInfo(pMode, &CalibWarpInfo);
            check_rval_not_equal_0(rval, __func__, __LINE__);
        } else {
            rval |= AmbaIK_SetWarpEnb(pMode, 0);
            check_rval_not_equal_0(rval, __func__, __LINE__);
        }
    } else {/*15.7*/};
#if 0
    {
        AMBA_IK_QUERY_WARP_s QueryWarp;
        AMBA_IK_WINDOW_SIZE_INFO_s WindowSizeInfo;// Mandatory
        AMBA_IK_VIN_ACTIVE_WINDOW_s ActiveWindow;// Optional
        AMBA_IK_DUMMY_MARGIN_RANGE_s DmyRange;// Optional
        AMBA_IK_DZOOM_INFO_s DzoomInfo;// Optional
        AMBA_IK_WARP_INFO_s WarpInfo;// Optional
        AMBA_IK_QUERY_WARP_RESULT_s QueryWarpResult;
        static UINT8 query_buf[885988];
        const UINT8 *pWarpGrid;

        QueryWarp.In.pWindowSizeInfo = &WindowSizeInfo;
        WindowSizeInfo.VinSensor.StartX                 = SystemInfo.RawStartX;
        WindowSizeInfo.VinSensor.StartY                 = SystemInfo.RawStartY;
        WindowSizeInfo.VinSensor.Width                  = SystemInfo.RawWidth;
        WindowSizeInfo.VinSensor.Height                 = SystemInfo.RawHeight;
        WindowSizeInfo.VinSensor.HSubSample.FactorNum   = SystemInfo.HSubSampleFactorNum;
        WindowSizeInfo.VinSensor.HSubSample.FactorDen   = SystemInfo.HSubSampleFactorDen;
        WindowSizeInfo.VinSensor.VSubSample.FactorNum   = SystemInfo.VSubSampleFactorNum;
        WindowSizeInfo.VinSensor.VSubSample.FactorDen   = SystemInfo.VSubSampleFactorDen;
        WindowSizeInfo.Main.Width                       = SystemInfo.MainWidth;
        WindowSizeInfo.Main.Height                      = SystemInfo.MainHeight;

        QueryWarp.In.pActiveWindow = &ActiveWindow;
        ActiveWindow.Enable           = GData.Active.Enable;
        ActiveWindow.ActiveGeo.StartX = GData.Active.ActiveGeo.StartX;
        ActiveWindow.ActiveGeo.StartY = GData.Active.ActiveGeo.StartY;
        ActiveWindow.ActiveGeo.Width  = GData.Active.ActiveGeo.Width;
        ActiveWindow.ActiveGeo.Height = GData.Active.ActiveGeo.Height;

        QueryWarp.In.pDmyRange = &DmyRange;
        DmyRange.Enable = GData.Dummy.Enable;
        DmyRange.Left   = GData.Dummy.Left;
        DmyRange.Right  = GData.Dummy.Right;
        DmyRange.Top    = GData.Dummy.Top;
        DmyRange.Bottom = GData.Dummy.Bottom;

        QueryWarp.In.pDzoomInfo = &DzoomInfo;
        DzoomInfo.Enable = GData.Dzoom.Enable;
        DzoomInfo.ShiftX = GData.Dzoom.ShiftX;
        DzoomInfo.ShiftY = GData.Dzoom.ShiftY;
        DzoomInfo.ZoomX  = GData.Dzoom.ZoomX;
        DzoomInfo.ZoomY  = GData.Dzoom.ZoomY;

        QueryWarp.In.pWarpInfo = &WarpInfo;
        WarpInfo.Version = GData.WarpCompensation.CalibVersion;
        WarpInfo.HorGridNum = (UINT32)GData.WarpCompensation.HorizontalGridNumber;
        WarpInfo.VerGridNum = (UINT32)GData.WarpCompensation.VerticalGridNumber;
        WarpInfo.TileWidthExp = (UINT32)GData.WarpCompensation.TileWidthExponent;
        WarpInfo.TileHeightExp = (UINT32)GData.WarpCompensation.TileHeightExponent;
        WarpInfo.VinSensorGeo.StartX = GData.WarpCompensation.VinSensorStartX;
        WarpInfo.VinSensorGeo.StartY = GData.WarpCompensation.VinSensorStartY;
        WarpInfo.VinSensorGeo.Width = GData.WarpCompensation.VinSensorWidth;
        WarpInfo.VinSensorGeo.Height = GData.WarpCompensation.VinSensorHeight;
        WarpInfo.VinSensorGeo.HSubSample.FactorNum = (UINT32)GData.WarpCompensation.VinSensorHSubSampleFactorNum;
        WarpInfo.VinSensorGeo.HSubSample.FactorDen = (UINT32)GData.WarpCompensation.VinSensorHSubSampleFactorDen;
        WarpInfo.VinSensorGeo.VSubSample.FactorNum = (UINT32)GData.WarpCompensation.VinSensorVSubSampleFactorNum;
        WarpInfo.VinSensorGeo.VSubSample.FactorDen = (UINT32)GData.WarpCompensation.VinSensorVSubSampleFactorDen;
        WarpInfo.Enb_2StageCompensation = (UINT8)GData.WarpCompensation.Enb2StageCompensation;
        pWarpGrid = &pGCalib_Table->WarpGrid[0];
        (void)ituner_memcpy(&WarpInfo.pWarp, &pWarpGrid, sizeof(AMBA_IK_GRID_POINT_s*));

        (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
        Ext_File_Param.Warp_Table_Load_Param.Address = WarpInfo.pWarp;
        Ext_File_Param.Warp_Table_Load_Param.Size = AmbaItuner_CalWarpTableSize(WarpInfo.HorGridNum, WarpInfo.VerGridNum);
        if (Ituner_Hook_Func.Load_Data(FILE_EXT_WARP_TABLE, &Ext_File_Param) < 0) {
            ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_WARP_TABLE) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }

        QueryWarp.WorkBufSize = 885988;
        QueryWarp.pWorkBuf = &query_buf[0];
        QueryWarp.In.Ability = _System.Ability;
        QueryWarp.In.FlipMode = 0;
        QueryWarp.In.WarpEnb = GData.WarpCompensation.Enable;
        QueryWarp.In.CawarpEnb = GData.ChromaAberrationInfo.Enable;

        AmbaIK_CalcWarpResources(&QueryWarp, &QueryWarpResult);
        ituner_print_uint32_5("##### AmbaIK_CalcWarpResources!!!!!", DC_U, DC_U, DC_U, DC_U, DC_U);
        ituner_print_uint32_5("WaitLine:%d, DmaSize:%d, VerticalRange:%d, VerticalWarpVectorFlipNum:%d, DmyBroken:%d", QueryWarpResult.WaitLine, QueryWarpResult.DmaSize, QueryWarpResult.VerticalRange, QueryWarpResult.VerticalWarpVectorFlipNum, QueryWarpResult.DmyBroken);
        ituner_print_uint32_5("Left:%d, Right:%d, Top:%d, Bottom:%d", QueryWarpResult.SuggestedDmyRange.Left, QueryWarpResult.SuggestedDmyRange.Right, QueryWarpResult.SuggestedDmyRange.Top, QueryWarpResult.SuggestedDmyRange.Bottom, DC_U);
    }
#endif
    return rval;
}

static INT32 AmbaItuner_Raw2yuvCalibExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;
    const UINT32 *pUint32;
    Ituner_Ext_File_Param_s Ext_File_Param;
    void *pVoid;
    INT8 check;

    /* Misra C fix */
    if(AmbaItuner_Check_Reg(ITUNER_WARP_COMPENSATION, SPECIAL_USE_NONE) > 0) {
        check = 1;;
    } else if (AmbaItuner_Check_Reg(ITUNER_DZOOM, SPECIAL_USE_NONE) > 0) {
        check = 1;;
    } else {
        check = 0;;
    }

    if (AmbaItuner_Check_Reg(ITUNER_ACTIVE, SPECIAL_USE_NONE) > 0) {
        rval= AmbaIK_SetVinActiveWin(pMode, &GData.Active);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_DUMMY, SPECIAL_USE_NONE) > 0) {
        rval= AmbaIK_SetDummyMarginRange(pMode, &GData.Dummy);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_DZOOM, SPECIAL_USE_NONE) > 0) {
        rval= AmbaIK_SetDzoomInfo(pMode, &GData.Dzoom);
        check_rval_not_equal_0(rval, __func__, __LINE__);
    }
    if (AmbaItuner_Check_Reg(ITUNER_STATIC_BAD_PIXEL_INTERNAL, SPECIAL_USE_NONE) > 0) {
        if (GData.StaticBadPixelCorrectionInternal.Enable > 0U) {
        } else {
            /*15.7*/
        }
    } else {
        if (AmbaItuner_Check_Reg(ITUNER_STATIC_BAD_PIXEL_CORRECTION, (INT32)GData.StaticBadPixelCorrection.Enable) > 0) {
            if (GData.StaticBadPixelCorrection.Enable > 0U) {
                GData.SbpCorr.CalibSbpInfo.Version= GData.StaticBadPixelCorrection.CalibVersion;
                GData.SbpCorr.VinSensorGeo.StartX = GData.StaticBadPixelCorrection.CalibVinStartX;
                GData.SbpCorr.VinSensorGeo.StartY = GData.StaticBadPixelCorrection.CalibVinStartY;
                GData.SbpCorr.VinSensorGeo.Width = GData.StaticBadPixelCorrection.CalibVinWidth;
                GData.SbpCorr.VinSensorGeo.Height = GData.StaticBadPixelCorrection.CalibVinHeight;
                GData.SbpCorr.VinSensorGeo.HSubSample.FactorNum= GData.StaticBadPixelCorrection.CalibVinHSubSampleFactorNum;
                GData.SbpCorr.VinSensorGeo.HSubSample.FactorDen= GData.StaticBadPixelCorrection.CalibVinHSubSampleFactorDen;
                GData.SbpCorr.VinSensorGeo.VSubSample.FactorNum= GData.StaticBadPixelCorrection.CalibVinVSubSampleFactorNum;
                GData.SbpCorr.VinSensorGeo.VSubSample.FactorDen= GData.StaticBadPixelCorrection.CalibVinVSubSampleFactorDen;
                GData.SbpCorr.CalibSbpInfo.pSbpBuffer = pGCalib_Table->FPNMap;
                // Note: Load Ext FPN Map
                (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
                Ext_File_Param.FPN_MAP_Ext_Load_Param.Address = GData.SbpCorr.CalibSbpInfo.pSbpBuffer;
                Ext_File_Param.FPN_MAP_Ext_Load_Param.Size = AmbaItuner_CalFpnMapSize(GData.StaticBadPixelCorrection.CalibVinWidth, GData.StaticBadPixelCorrection.CalibVinHeight);
                Ext_File_Param.FPN_MAP_Ext_Load_Param.Size =
                    (Ext_File_Param.FPN_MAP_Ext_Load_Param.Size > sizeof(pGCalib_Table->FPNMap)) ? sizeof(pGCalib_Table->FPNMap) : Ext_File_Param.FPN_MAP_Ext_Load_Param.Size;
                if (Ituner_Hook_Func.Load_Data(FILE_EXT_FPN_MAP, &Ext_File_Param) < 0) {
                    ituner_print_str_5("%s() , call Ituner_Hook_Func.Load_Data(FILE_EXT_FPN_MAP) Fail\n", __func__, DC_S, DC_S, DC_S, DC_S);
                    ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);

                }
                rval= AmbaIK_SetStaticBadPxlCor(pMode, &GData.SbpCorr);
                if ((INT32)rval != 0) {
                    ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                    ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
                }
                ret |= rval;
            } else {
                /*15.7*/
            }
            rval= AmbaIK_SetStaticBadPxlCorEnb(pMode, GData.StaticBadPixelCorrection.Enable);
            check_rval_not_equal_0(rval, __func__, __LINE__);
            ret |= rval;
        }
    }

    if (AmbaItuner_Check_Reg(ITUNER_VIGNETTE_COMPENSATION, SPECIAL_USE_NONE) > 0) {
        if ((AmbaItuner_Check_Reg(ITUNER_VIGNETTE_COMPENSATION, (INT32)GData.VignetteCompensation.Enable) > 0) && (GData.VignetteCompensation.Enable > 0U) ) {
            (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
            pUint32 = &GData.VignetteCompensation.VignetteInfo.CalibVignetteInfo.VigGainTblR[0][0];
            (void)ituner_memcpy (&pVoid, &pUint32, sizeof(void *));
            Ext_File_Param.Vignette_Gain_Load_Param.Address = pVoid;
            Ext_File_Param.Vignette_Gain_Load_Param.Size = ITUNER_VIGNETTE_GAIN_MAP_MULT_SIZE;
            if (Ituner_Hook_Func.Load_Data(FILE_EXT_VIGNETTE, &Ext_File_Param) < 0) {
                ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_VIGNETTE) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            rval= AmbaIK_SetVignette(pMode, &GData.VignetteCompensation.VignetteInfo);
            check_rval_not_equal_0(rval, __func__, __LINE__);
            ret |= rval;
        }
        rval= AmbaIK_SetVignetteEnb(pMode, GData.VignetteCompensation.Enable);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    ret |= AmbaItuner_Raw2yuvCalibExecute_part2(pMode, check);
    return (INT32)ret;
}

static INT32 AmbaItuner_Yuv2yuvCalibExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;
    Ituner_Ext_File_Param_s Ext_File_Param;
    //INT16 *pInt16;
    INT8 check;
    //const UINT8 *pUint8;

    /* Misra C fix */
    if(AmbaItuner_Check_Reg(ITUNER_WARP_COMPENSATION, SPECIAL_USE_NONE) > 0) {
        check = 1;;
    } else if (AmbaItuner_Check_Reg(ITUNER_DZOOM, SPECIAL_USE_NONE) > 0) {
        check = 1;;
    } else {
        check = 0;;
    }

    if (AmbaItuner_Check_Reg(ITUNER_ACTIVE, SPECIAL_USE_NONE) > 0) {
        rval= AmbaIK_SetVinActiveWin(pMode, &GData.Active);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_DUMMY, SPECIAL_USE_NONE) > 0) {
        rval= AmbaIK_SetDummyMarginRange(pMode, &GData.Dummy);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_DZOOM, SPECIAL_USE_NONE) > 0) {
        rval= AmbaIK_SetDzoomInfo(pMode, &GData.Dzoom);
        check_rval_not_equal_0(rval, __func__, __LINE__);
    }
    if (AmbaItuner_Check_Reg(ITUNER_WARP_COMPENSATION_DZOOM_INTERNAL, SPECIAL_USE_NONE) > 0) {
        // Set for warp bypass part
        // Bypass warp horizontal part
#if 0
        if (GData.WarpCompensationDzoomInternal.Enable > 0U) {
            GData.WarpInternal.Enable = GData.WarpCompensationDzoomInternal.Enable;
            GData.WarpInternal.GridArrayWidth = GData.WarpCompensationDzoomInternal.GridArrayWidth;
            GData.WarpInternal.GridArrayHeight = GData.WarpCompensationDzoomInternal.GridArrayHeight;
            GData.WarpInternal.HorzGridSpacingExponent = GData.WarpCompensationDzoomInternal.HorzGridSpacingExponent;
            GData.WarpInternal.VertGridSpacingExponent = GData.WarpCompensationDzoomInternal.VertGridSpacingExponent;
            pUint8 = &pGCalib_Table->WarpGrid[0];
            (void)ituner_memcpy (&pInt16, &pUint8, sizeof(INT16 *));
            GData.WarpInternal.pWarpHorizontalTable = pInt16;
            {
                (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
                Ext_File_Param.Internal_Warp_Horizontal_Table_Load_Param.Address = pGCalib_Table->WarpGrid;
                Ext_File_Param.Internal_Warp_Horizontal_Table_Load_Param.Max_Size = sizeof(pGCalib_Table->WarpGrid);
                if (Ituner_Hook_Func.Load_Data(FILE_EXT_INTERNAL_WARP_HORIZONTAL_TABLE, &Ext_File_Param) < 0) {
                    ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_BYPASS_WARP_HORIZONTAL_TABLE) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                    ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
                }
            }
        } else {
            // misraC fix
        }

        // Bypass warp vertical part
        if (GData.WarpCompensationDzoomInternal.Enable > 0U) {
            GData.WarpInternal.VertWarpGridArrayWidth = GData.WarpCompensationDzoomInternal.VertWarpGridArrayWidth;
            GData.WarpInternal.VertWarpGridArrayHeight = GData.WarpCompensationDzoomInternal.VertWarpGridArrayHeight;
            GData.WarpInternal.VertWarpHorzGridSpacingExponent = GData.WarpCompensationDzoomInternal.VertWarpHorzGridSpacingExponent;
            GData.WarpInternal.VertWarpVertGridSpacingExponent = GData.WarpCompensationDzoomInternal.VertWarpVertGridSpacingExponent;
            pUint8 = (&pGCalib_Table->WarpGrid[(sizeof(pGCalib_Table->WarpGrid) >> 1)]);
            (void)ituner_memcpy (&pInt16, &pUint8, sizeof(INT16 *));
            GData.WarpInternal.pWarpVerticalTable = pInt16;
            {
                (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
                Ext_File_Param.Internal_Warp_Vertical_Table_Load_Param.Address = GData.WarpInternal.pWarpVerticalTable;
                Ext_File_Param.Internal_Warp_Vertical_Table_Load_Param.Max_Size = sizeof(pGCalib_Table->WarpGrid);
                if (Ituner_Hook_Func.Load_Data(FILE_EXT_INTERNAL_WARP_VERTICAL_TABLE, &Ext_File_Param) < 0) {
                    ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_BYPASS_WARP_VERITCAL_TABLE) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                    ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
                }
            }
        } else {
            // misraC fix
        }
        rval= AmbaIK_SetWarpInternal(pMode, &GData.WarpInternal);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
#endif
    } else if (check > 0) { /*Misra C check*/
        AMBA_IK_WARP_INFO_s CalibWarpInfo;
        if (GData.WarpCompensation.Enable > 0U) {
            const UINT8 *pWarpGrid;
            rval= AmbaIK_SetWarpEnb(pMode, 1);
            if ((INT32)rval != 0) {
                ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            CalibWarpInfo.Version = GData.WarpCompensation.CalibVersion;
            CalibWarpInfo.HorGridNum = (UINT32)GData.WarpCompensation.HorizontalGridNumber;
            CalibWarpInfo.VerGridNum = (UINT32)GData.WarpCompensation.VerticalGridNumber;
            CalibWarpInfo.TileWidthExp = (UINT32)GData.WarpCompensation.TileWidthExponent;
            CalibWarpInfo.TileHeightExp = (UINT32)GData.WarpCompensation.TileHeightExponent;
            CalibWarpInfo.VinSensorGeo.StartX = GData.WarpCompensation.VinSensorStartX;
            CalibWarpInfo.VinSensorGeo.StartY = GData.WarpCompensation.VinSensorStartY;
            CalibWarpInfo.VinSensorGeo.Width = GData.WarpCompensation.VinSensorWidth;
            CalibWarpInfo.VinSensorGeo.Height = GData.WarpCompensation.VinSensorHeight;
            CalibWarpInfo.VinSensorGeo.HSubSample.FactorNum = (UINT32)GData.WarpCompensation.VinSensorHSubSampleFactorNum;
            CalibWarpInfo.VinSensorGeo.HSubSample.FactorDen = (UINT32)GData.WarpCompensation.VinSensorHSubSampleFactorDen;
            CalibWarpInfo.VinSensorGeo.VSubSample.FactorNum = (UINT32)GData.WarpCompensation.VinSensorVSubSampleFactorNum;
            CalibWarpInfo.VinSensorGeo.VSubSample.FactorDen = (UINT32)GData.WarpCompensation.VinSensorVSubSampleFactorDen;
            CalibWarpInfo.Enb_2StageCompensation = (UINT8)GData.WarpCompensation.Enb2StageCompensation;

            //CalibWarpInfo.pWarp = &pGCalib_Table->WarpGrid[0];
            pWarpGrid = &pGCalib_Table->WarpGrid[0];
            (void)ituner_memcpy(&CalibWarpInfo.pWarp, &pWarpGrid, sizeof(AMBA_IK_GRID_POINT_s*));

            (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
            Ext_File_Param.Warp_Table_Load_Param.Address = CalibWarpInfo.pWarp;
            Ext_File_Param.Warp_Table_Load_Param.Size = AmbaItuner_CalWarpTableSize(CalibWarpInfo.HorGridNum, CalibWarpInfo.VerGridNum);
            if (Ituner_Hook_Func.Load_Data(FILE_EXT_WARP_TABLE, &Ext_File_Param) < 0) {
                ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_WARP_TABLE) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            rval= AmbaIK_SetWarpInfo(pMode, &CalibWarpInfo);
            check_rval_not_equal_0(rval, __func__, __LINE__);
            ret |= rval;
        } else {
            rval= AmbaIK_SetWarpEnb(pMode, 0);
            check_rval_not_equal_0(rval, __func__, __LINE__);
        }
    } else {/*15.7*/};
    return (INT32)ret;
}

INT32 AmbaItuner_FrontEndToneExecute(void)
{
    UINT32 rval = 0UL;

    if (CompandCount==0UL) {
        CompandCount++;
        (void)ituner_memset(&Compand[0], 0x0, sizeof(UINT32)*IK_NUM_COMPAND_TABLE);
    }

    //ituner_memset(&Compand[0], 0x0, sizeof(UINT32)*IK_NUM_COMPAND_TABLE);
    //ituner_memset(&Decompand[0], 0x0, sizeof(UINT32)*IK_NUM_DECOMPAND_TABLE);
    CompandEnable = GData.FeToneCurve.Compand.CompandEnable;
    (void)ituner_memcpy(&Compand[0], &GData.FeToneCurve.Compand.CompandTable[0], sizeof(UINT32)*IK_NUM_COMPAND_TABLE);

    return (INT32)rval;
}

static INT32 AmbaItuner_VinSensorExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    {
        rval= AmbaIK_GetVinSensorInfo(pMode, &GData.SensorInfo);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
        GData.SensorInfo.SensorResolution = SystemInfo.RawResolution;
        GData.SensorInfo.SensorPattern = SystemInfo.RawBayer;
        GData.SensorInfo.SensorReadoutMode = SystemInfo.SensorReadoutMode;
        GData.SensorInfo.Compression = SystemInfo.CompressedRaw;
        GData.SensorInfo.SensorMode = SystemInfo.SensorMode;
        GData.SensorInfo.CompressionOffset = SystemInfo.CompressionOffset;
        rval= AmbaIK_SetVinSensorInfo(pMode, &GData.SensorInfo);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
    }

    return (INT32)ret;
}

static INT32 AmbaItuner_Raw2RawExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    if (AmbaItuner_Check_Reg(ITUNER_FRONT_END_TONE_CURVE, SPECIAL_USE_NONE) >0 ) {
        rval = AmbaIK_SetFeToneCurve(pMode, &GData.FeToneCurve.Decompand);
        rval |= (UINT32)AmbaItuner_FrontEndToneExecute();
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_STATIC_BLACK_LEVEL_EXP0, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetFeStaticBlc(pMode, &GData.StaticBlackLevel[0], 0UL);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION, (INT32)GData.DynamicBadPixelCorrection.Enable)>0) {
        rval= AmbaIK_SetDynamicBadPxlCor(pMode, &GData.DynamicBadPixelCorrection);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_CFA_LEAKAGE_FILTER, (INT32)GData.CfaLeakageFilter.Enable)>0) {
        rval= AmbaIK_SetCfaLeakageFilter(pMode, &GData.CfaLeakageFilter);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_CFA_NOISE_FILTER, (INT32)GData.CfaNoiseFilter.Enable)>0) {
        rval= AmbaIK_SetCfaNoiseFilter(pMode, &GData.CfaNoiseFilter);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_ANTI_ALIASING_STRENGTH, (INT32)GData.AntiAliasing.Enable)>0) {
        rval= AmbaIK_SetAntiAliasing(pMode, &GData.AntiAliasing);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_FRONT_END_WB_GAIN_EXP0, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetFeWbGain(pMode, &GData.FrontEndWbGain[0], 0UL);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_AFTER_CE_WB_GAIN, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetAfterCeWbGain(pMode, &GData.AfterCeWbGain);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_BEFORE_CE_WB_GAIN, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetBeforeCeWbGain(pMode, &GData.BeforeCeWbGain);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_DEMOSAIC_FILTER, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetDemosaic(pMode, &GData.DemosaicFilter);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_GB_GR_MISMATCH, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetGrgbMismatch(pMode, &GData.GbGrMismatch);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_RGB_IR, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetRgbIr(pMode, &GData.RgbIr);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    return (INT32)ret;
}

static inline UINT32 check_and_set_sharpen(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;
    if (AmbaItuner_Check_Reg(ITUNER_SHARPEN_BOTH, SPECIAL_USE_NONE) >0 ) {
        rval |= AmbaIK_SetFstShpNsBoth(pMode, &GData.SharpenBoth.BothInfo);
        check_rval_not_equal_0(rval, __func__, __LINE__);
    }
    if (AmbaItuner_Check_Reg(ITUNER_SHARPEN_NOISE, SPECIAL_USE_NONE) >0 ) {
        rval |= AmbaIK_SetFstShpNsNoise(pMode, &GData.SharpenNoise);
        check_rval_not_equal_0(rval, __func__, __LINE__);
    }
    if (AmbaItuner_Check_Reg(ITUNER_FIR, SPECIAL_USE_NONE) >0 ) {
        rval |= AmbaIK_SetFstShpNsFir(pMode, &GData.Fir);
        check_rval_not_equal_0(rval, __func__, __LINE__);
    }
    if (AmbaItuner_Check_Reg(ITUNER_CORING, SPECIAL_USE_NONE) >0 ) {
        rval |= AmbaIK_SetFstShpNsCoring(pMode, &GData.Coring);
        check_rval_not_equal_0(rval, __func__, __LINE__);
    }
    if (AmbaItuner_Check_Reg(ITUNER_CORING_INDEX_SCALE, SPECIAL_USE_NONE) >0 ) {
        rval |= AmbaIK_SetFstShpNsCorIdxScl(pMode, &GData.CoringIndexScale);
        check_rval_not_equal_0(rval, __func__, __LINE__);
    }
    if (AmbaItuner_Check_Reg(ITUNER_MIN_CORING_RESULT, SPECIAL_USE_NONE) >0 ) {
        rval |= AmbaIK_SetFstShpNsMinCorRst(pMode, &GData.MinCoringResult);
        check_rval_not_equal_0(rval, __func__, __LINE__);
    }
    if (AmbaItuner_Check_Reg(ITUNER_MAX_CORING_RESULT, SPECIAL_USE_NONE) >0 ) {
        rval |= AmbaIK_SetFstShpNsMaxCorRst(pMode, &GData.MaxCoringResult);
        check_rval_not_equal_0(rval, __func__, __LINE__);
    }
    if (AmbaItuner_Check_Reg(ITUNER_SCALE_CORING, SPECIAL_USE_NONE) >0 ) {
        rval |= AmbaIK_SetFstShpNsSclCor(pMode, &GData.ScaleCoring);
        check_rval_not_equal_0(rval, __func__, __LINE__);
    }
    return rval;
}

static INT32 AmbaItuner_Yuv2YuvExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;
    const UINT32 *pUint32;
    void *pVoid;
    Ituner_Ext_File_Param_s Ext_File_Param;

    if (AmbaItuner_Check_Reg(ITUNER_PRE_CC_GAIN, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetPreCcGain(pMode, &GData.PreCcGain);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_COLOR_CORRECTION, SPECIAL_USE_NONE) > 0) {
        (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
        pUint32 = (&pGCalib_Table->CcReg[0]);
        (void)ituner_memcpy (&pVoid, &pUint32, sizeof(void *));
        Ext_File_Param.CC_Reg_Load_Param.Address = pVoid;
        Ext_File_Param.CC_Reg_Load_Param.Size = IK_CC_REG_SIZE;
        if (Ituner_Hook_Func.Load_Data(FILE_EXT_CC_REG, &Ext_File_Param) < 0) {
            ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_CC_REG) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        } else {
            (void)ituner_memcpy(&GData.CcReg.RegSettingTable[0], &pGCalib_Table->CcReg[0], IK_CC_REG_SIZE);
            rval= AmbaIK_SetColorCorrectionReg(pMode, &GData.CcReg);
            check_rval_not_equal_0(rval, __func__, __LINE__);
            ret |= rval;
        }
        (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));
        pUint32 = (&pGCalib_Table->Cc3d[0]);
        (void)ituner_memcpy (&pVoid, &pUint32, sizeof(void *));
        Ext_File_Param.CC_ThreeD_Load_Param.Address = pVoid;
        Ext_File_Param.CC_ThreeD_Load_Param.Size = IK_CC_3D_SIZE;
        if (Ituner_Hook_Func.Load_Data(FILE_EXT_CC_THREED, &Ext_File_Param) < 0) {
            ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_CC_THREED) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        } else {
            (void)ituner_memcpy(&GData.CcThreeD.MatrixThreeDTable[0], &pGCalib_Table->Cc3d[0], IK_CC_3D_SIZE);
            rval= AmbaIK_SetColorCorrection(pMode, &GData.CcThreeD);
            check_rval_not_equal_0(rval, __func__, __LINE__);
            ret |= rval;
        }
    }
    if (AmbaItuner_Check_Reg(ITUNER_TONE_CURVE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetToneCurve(pMode, &GData.ToneCurve);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_RGB_TO_YUV_MATRIX, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetRgbToYuvMatrix(pMode, &GData.RgbToYuvMatrix);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_CHROMA_SCALE, (INT32)GData.ChromaScale.Enable) >0 ) {
        rval= AmbaIK_SetChromaScale(pMode, &GData.ChromaScale);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_CHROMA_MEDIAN_FILTER, (INT32)GData.ChromaMedianFilter.Enable) >0 ) {
        rval= AmbaIK_SetChromaMedianFilter(pMode, &GData.ChromaMedianFilter);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_CHROMA_FILTER, (INT32)GData.ChromaFilter.Enable) >0 ) {
        rval= AmbaIK_SetChromaFilter(pMode, &GData.ChromaFilter);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetFirstLumaProcMode(pMode, &GData.FirstLumaProcessMode);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (GData.FirstLumaProcessMode.UseSharpenNotAsf == 0UL) {
        if (AmbaItuner_Check_Reg(ITUNER_ASF_INFO, (INT32)GData.AsfInfo.AsfInfo.Enable) >0 ) {
            rval = AmbaIK_SetAdvSpatFltr(pMode, &GData.AsfInfo.AsfInfo);
            check_rval_not_equal_0(rval, __func__, __LINE__);
            ret |= rval;
        }
    } else if (GData.FirstLumaProcessMode.UseSharpenNotAsf == 1UL) {
        ret |= check_and_set_sharpen(pMode);
    } else {
        // misraC fix
    }

    return (INT32)ret;
}

static INT32 AmbaItuner_CeExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;
    const UINT32 *pUint32;
    void *pVoid;
    INT8 check;
    Ituner_Ext_File_Param_s Ext_File_Param;

    if (AmbaItuner_Check_Reg(ITUNER_CONTRAST_ENHANCE, SPECIAL_USE_SKIP) >0 ) {
        rval = AmbaIK_SetCe(pMode, &GData.ContrastEnhance.ContrastEnhance);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
        if (GData.ContrastEnhance.OutputTablePath[0] != (char) 0) {
            (void)ituner_memset(&Ext_File_Param, 0x0, sizeof(Ext_File_Param));

            pUint32 = &GData.ContrastEnhance.ContrastEnhanceOutTable.OutputTable[0];
            (void)ituner_memcpy (&pVoid, &pUint32, sizeof(void *));
            Ext_File_Param.HDR_Contrast_Enhance_Load_Param.Address = pVoid;
            Ext_File_Param.HDR_Contrast_Enhance_Load_Param.Size = IK_CE_OUT_TABLE_SIZE * sizeof(UINT16);
            if(Ituner_Hook_Func.Load_Data != NULL) {
                if(Ituner_Hook_Func.Load_Data(FILE_EXT_HDR_CONTRAST_ENHANCE_TABLE, &Ext_File_Param) < 0) {
                    check = 1;
                } else {
                    check = 0;
                }
            } else {
                check = 0;
            }
            if (check > 0) {
                ituner_print_str_5("%s() call Ituner_Hook_Func.Load_Data(FILE_EXT_HDR_CONTRAST_ENHANCE_TABLE) Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
        }
        rval = AmbaIK_SetCeInputTable(pMode, &GData.ContrastEnhance.ContrastEnhanceInputTable);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
        rval = AmbaIK_SetCeOutputTable(pMode, &GData.ContrastEnhance.ContrastEnhanceOutTable);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
    }

    return (INT32)ret;
}

static INT32 AmbaItuner_MctfExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;
    INT8 check;

    if (AmbaItuner_Check_Reg(ITUNER_FINAL_SHARPEN_BOTH, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetFnlShpNsBoth(pMode, &GData.FinalSharpenBoth.BothInfo);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
        rval= AmbaIK_SetFnlShpNsBothTdt(pMode, &GData.FinalSharpenBoth.ThreedTable);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_FINAL_SHARPEN_NOISE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetFnlShpNsNoise(pMode, &GData.FinalSharpenNoise);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_FINAL_FIR, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetFnlShpNsFir(pMode, &GData.FinalFir);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_FINAL_CORING, SPECIAL_USE_NONE) >0 ) {
        rval=AmbaIK_SetFnlShpNsCoring(pMode, &GData.FinalCoring);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_FINAL_CORING_INDEX_SCALE, SPECIAL_USE_NONE) >0 ) {
        rval = AmbaIK_SetFnlShpNsCorIdxScl(pMode, &GData.FinalCoringIndexScale);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_FINAL_MIN_CORING_RESULT, SPECIAL_USE_NONE) >0 ) {
        rval = AmbaIK_SetFnlShpNsMinCorRst(pMode, &GData.FinalMinCoringResult);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_FINAL_MAX_CORING_RESULT, SPECIAL_USE_NONE) >0 ) {
        rval = AmbaIK_SetFnlShpNsMaxCorRst(pMode, &GData.FinalMaxCoringResult);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_FINAL_SCALE_CORING, SPECIAL_USE_NONE) >0 ) {
        rval = AmbaIK_SetFnlShpNsSclCor(pMode, &GData.FinalScaleCoring);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    // FIXME: CHECK_RET(0, AmbaIK_SetMctfInternal(pMode, &InternalMctfInfo);
    if (Ituner_Valid_Filter[ITUNER_VIDEO_MCTF] != Ituner_Valid_Filter[ITUNER_VIDEO_MCTF_LEVEL]) {
        AMBA_IK_VIDEO_MCTF_s CurrentVideoMctf;
        (void)AmbaIK_GetVideoMctf(pMode, &CurrentVideoMctf);
        if (Ituner_Valid_Filter[ITUNER_VIDEO_MCTF] == 0U) {
            GData.VideoMctf.Enable= CurrentVideoMctf.Enable;
            GData.VideoMctf.LocalLighting= CurrentVideoMctf.LocalLighting;
//                    GData.VideoMctf.frame_brightness = CurrentVideoMctf.frame_brightness;
            GData.VideoMctf.YCombineStrength = CurrentVideoMctf.YCombineStrength;
            GData.VideoMctf.YStrength3D = CurrentVideoMctf.YStrength3D;
            (void)ituner_memcpy(GData.VideoMctf.YSpatialWeighting, CurrentVideoMctf.YSpatialWeighting, sizeof(GData.VideoMctf.YSpatialWeighting));
            GData.VideoMctf.YSpatialMaxChange = CurrentVideoMctf.YSpatialMaxChange;
            GData.VideoMctf.YTemporalMaxChange = CurrentVideoMctf.YTemporalMaxChange;
            GData.VideoMctf.CbStrength3D = CurrentVideoMctf.CbStrength3D;
            (void)ituner_memcpy(GData.VideoMctf.CbSpatialWeighting, CurrentVideoMctf.CbSpatialWeighting, sizeof(GData.VideoMctf.CbSpatialWeighting));
            GData.VideoMctf.CbSpatialMaxChange = CurrentVideoMctf.CbSpatialMaxChange;
            GData.VideoMctf.CbTemporalMaxChange = CurrentVideoMctf.CbTemporalMaxChange;
            GData.VideoMctf.CrStrength3D = CurrentVideoMctf.CrStrength3D;
            (void)ituner_memcpy(GData.VideoMctf.CrSpatialWeighting, CurrentVideoMctf.CrSpatialWeighting, sizeof(GData.VideoMctf.CrSpatialWeighting));
            GData.VideoMctf.CrSpatialMaxChange = CurrentVideoMctf.CrSpatialMaxChange;
            GData.VideoMctf.CrTemporalMaxChange = CurrentVideoMctf.CrTemporalMaxChange;
            (void)ituner_memcpy(&GData.VideoMctf.YCurve, &CurrentVideoMctf.YCurve, sizeof(GData.VideoMctf.YCurve));
            (void)ituner_memcpy(&GData.VideoMctf.CbCurve, &CurrentVideoMctf.CbCurve, sizeof(GData.VideoMctf.CbCurve));
            (void)ituner_memcpy(&GData.VideoMctf.CrCurve, &CurrentVideoMctf.CrCurve, sizeof(GData.VideoMctf.CrCurve));
            GData.VideoMctf.YSpatSmthWideEdgeDetect = CurrentVideoMctf.YSpatSmthWideEdgeDetect;
            GData.VideoMctf.YSpatSmthEdgeThresh = CurrentVideoMctf.YSpatSmthEdgeThresh;
            (void)ituner_memcpy(&GData.VideoMctf.YSpatSmthDir, &CurrentVideoMctf.YSpatSmthDir, sizeof(GData.VideoMctf.YSpatSmthDir));
            (void)ituner_memcpy(&GData.VideoMctf.YSpatSmthIso, &CurrentVideoMctf.YSpatSmthIso, sizeof(GData.VideoMctf.YSpatSmthIso));
            GData.VideoMctf.CbSpatSmthWideEdgeDetect = CurrentVideoMctf.CbSpatSmthWideEdgeDetect;
            GData.VideoMctf.CbSpatSmthEdgeThresh = CurrentVideoMctf.CbSpatSmthEdgeThresh;
            (void)ituner_memcpy(&GData.VideoMctf.CbSpatSmthDir, &CurrentVideoMctf.CbSpatSmthDir, sizeof(GData.VideoMctf.CbSpatSmthDir));
            (void)ituner_memcpy(&GData.VideoMctf.CbSpatSmthIso, &CurrentVideoMctf.CbSpatSmthIso, sizeof(GData.VideoMctf.CbSpatSmthIso));
            GData.VideoMctf.CrSpatSmthWideEdgeDetect = CurrentVideoMctf.CrSpatSmthWideEdgeDetect;
            GData.VideoMctf.CrSpatSmthEdgeThresh = CurrentVideoMctf.CrSpatSmthEdgeThresh;
            (void)ituner_memcpy(&GData.VideoMctf.CrSpatSmthDir, &CurrentVideoMctf.CrSpatSmthDir, sizeof(GData.VideoMctf.CrSpatSmthDir));
            (void)ituner_memcpy(&GData.VideoMctf.CrSpatSmthIso, &CurrentVideoMctf.CrSpatSmthIso, sizeof(GData.VideoMctf.CrSpatSmthIso));

            (void)ituner_memcpy(&GData.VideoMctf.YOverallMaxChange, &CurrentVideoMctf.YOverallMaxChange, sizeof(GData.VideoMctf.YOverallMaxChange));
            (void)ituner_memcpy(&GData.VideoMctf.CbOverallMaxChange, &CurrentVideoMctf.CbOverallMaxChange, sizeof(GData.VideoMctf.CbOverallMaxChange));
            (void)ituner_memcpy(&GData.VideoMctf.CrOverallMaxChange, &CurrentVideoMctf.CrOverallMaxChange, sizeof(GData.VideoMctf.CrOverallMaxChange));

        } else { // Ituner_Valid_Filter[ITUNER_VIDEO_MCTF_LEVEL] == 0
            GData.VideoMctf.YTemporalMaxChangeNotT0T1LevelBased = CurrentVideoMctf.YTemporalMaxChangeNotT0T1LevelBased;
            GData.VideoMctf.CbTemporalMaxChangeNotT0T1LevelBased = CurrentVideoMctf.CbTemporalMaxChangeNotT0T1LevelBased;
            GData.VideoMctf.CrTemporalMaxChangeNotT0T1LevelBased = CurrentVideoMctf.CrTemporalMaxChangeNotT0T1LevelBased;
            (void)ituner_memcpy(&GData.VideoMctf.YTemporalEitherMaxChangeOrT0T1Add,
                                &CurrentVideoMctf.YTemporalEitherMaxChangeOrT0T1Add,
                                sizeof(GData.VideoMctf.YTemporalEitherMaxChangeOrT0T1Add));
            (void)ituner_memcpy(&GData.VideoMctf.CbTemporalEitherMaxChangeOrT0T1Add,
                                &CurrentVideoMctf.CbTemporalEitherMaxChangeOrT0T1Add,
                                sizeof(GData.VideoMctf.CbTemporalEitherMaxChangeOrT0T1Add));
            (void)ituner_memcpy(&GData.VideoMctf.CrTemporalEitherMaxChangeOrT0T1Add,
                                &CurrentVideoMctf.CrTemporalEitherMaxChangeOrT0T1Add,
                                sizeof(GData.VideoMctf.CrTemporalEitherMaxChangeOrT0T1Add));
        }
    }

    if (AmbaItuner_Check_Reg(ITUNER_VIDEO_MCTF, (INT32)GData.VideoMctf.Enable) >0 ) {
        check = 1;
    } else if (AmbaItuner_Check_Reg(ITUNER_VIDEO_MCTF_LEVEL, SPECIAL_USE_NONE) >0 ) {
        check = 1;
    } else {
        check = 0;
    }
    if ( check > 0 ) {
        rval = AmbaIK_SetVideoMctf(pMode, &GData.VideoMctf);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_VIDEO_MCTF_TEMPORAL_ADJUST, SPECIAL_USE_NONE) >0 ) {
        rval = AmbaIK_SetVideoMctfTa(pMode, &GData.VideoMctfTemporalAdjust);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_VIDEO_MCTF_AND_FINAL_SHARPEN, SPECIAL_USE_NONE) >0 ) {
        rval = AmbaIK_SetVideoMctfAndFnlshp(pMode, &GData.VideoMctfAndFinalSharpen);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    return (INT32)ret;
}
static inline UINT32 AmbaItuner_HisoExecute_part1(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;
    if (AmbaItuner_Check_Reg(ITUNER_LI_CHROMA_ASF, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetChromaAsf(pMode, &GData.ChromaAsf);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_CHROMA_ASF, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiChromaAsf(pMode, &GData.HiChromaAsf);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_LUMA_ASF, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiAsf(pMode, &GData.HiAsf);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_CFA_NOISE_FILTER, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiCfaNoiseFilter(pMode, &GData.HiCfaNoiseFilter);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_CFA_LEAKAGE_FILTER, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiCfaLeakageFilter(pMode, &GData.HiCfaLeakageFilter);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_ANTI_ALIASING, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiAntiAliasing(pMode, &GData.HiAntiAliasing);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_AUTO_BAD_PIXEL_CORRECTION, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiDynamicBPC(pMode, &GData.HiDynamicBpc);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_CHROMA_FLTR_HIGH, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiChromaFilterHigh(pMode, &GData.HiChromaFilterHigh);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_CHROMA_FLTR_LOW, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiChromaFilterLow(pMode, &GData.HiChromaFilterLow);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_CHROMA_FLTR_LOW_COMBINE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiChromaFltrLowCom(pMode, &GData.HiChromaFilterLowCombine);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_CHROMA_FLTR_MEDIAN, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiChromaFilterMed(pMode, &GData.HiChromaFilterMed);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_CHROMA_FLTR_MEDIAN_COMBINE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiChromaFltrMedCom(pMode, &GData.HiChromaFilterMedCombine);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_CHROMA_FLTR_PRE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiChromaFilterPre(pMode, &GData.HiChromaFilterPre);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_CHROMA_FLTR_VERY_LOW, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiChromaFilterVeryLow(pMode, &GData.HiChromaFilterVeryLow);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_CHROMA_FLTR_VERY_LOW_COMBINE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiChromaFltrVeryLowCom(pMode, &GData.HiChromaFilterVeryLowCombine);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_CHROMA_MEDIAN_FILTER, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiChromaMedianFilter(pMode, &GData.HiChromaMedianFilter);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNRE_HI_DEMOSAIC_FILTER, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiDemosaic(pMode, &GData.HiDemosaic);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    return ret;
}
static inline UINT32 AmbaItuner_HisoExecute_part2(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    if (AmbaItuner_Check_Reg(ITUNER_HI_GB_GR_MISMATCH, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiGrGbMismatch(pMode, &GData.HiGrGbMismatch);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_HIGH2_LUMA_ASF, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiHigh2Asf(pMode, &GData.HiHigh2Asf);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }
    if (AmbaItuner_Check_Reg(ITUNER_HI_HIGH_LUMA_ASF, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiHighAsf(pMode, &GData.HiHighAsf);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_HIGH_SHARPEN_BOTH, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiHighShpNsBoth(pMode, &GData.HiHighShpnsBoth);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_HIGH_SHARPEN_NOISE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiHighShpNsNoise(pMode, &GData.HiHighShpnsNoise);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_HIGH_FIR, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiHighShpNsFir(pMode, &GData.HiHighShpnsFir);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_HIGH_CORING, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiHighShpNsCoring(pMode, &GData.HiHighShpnsCoring);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_HIGH_CORING_INDEX_SCALE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiHighShpNsCorIdxScl(pMode, &GData.HiHighShpnsCorIdxScl);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_HIGH_MAX_CORING_RESULT, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiHighShpNsMaxCorRst(pMode, &GData.HiHighShpnsMaxCorRst);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_HIGH_MIN_CORING_RESULT, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiHighShpNsMinCorRst(pMode, &GData.HiHighShpnsMinCorRst);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_HIGH_SCALE_CORING, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiHighShpNsSclCor(pMode, &GData.HiHighShpnsSclCor);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_LOW_LUMA_ASF, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiLowAsf(pMode, &GData.HiLowAsf);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_LOW_LUMA_ASF_COMBINE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiLowASFCombine(pMode, &GData.HiLowAsfCombine);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_LUMA_NOISE_COMBINE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiLumaCombine(pMode, &GData.HiLumaCombine);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_MED1_LUMA_ASF, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiMed1Asf(pMode, &GData.HiMed1Asf);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_MED2_LUMA_ASF, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiMed2Asf(pMode, &GData.HiMed2Asf);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_MED_SHARPEN_BOTH, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiMedShpNsBoth(pMode, &GData.HiMedShpnsBoth);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_MED_SHARPEN_NOISE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiMedShpNsNoise(pMode, &GData.HiMedShpnsNoise);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    return ret;
}
static inline UINT32 AmbaItuner_HisoExecute_part3(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    if (AmbaItuner_Check_Reg(ITUNER_HI_MED_FIR, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiMedShpNsFir(pMode, &GData.HiMedShpnsFir);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_MED_CORING, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiMedShpNsCoring(pMode, &GData.HiMedShpnsCoring);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_MED_CORING_INDEX_SCALE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiMedShpNsCorIdxScl(pMode, &GData.HiMedShpnsCorIdxScl);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_MED_MAX_CORING_RESULT, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiMedShpNsMaxCorRst(pMode, &GData.HiMedShpnsMaxCorRst);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_MED_MIN_CORING_RESULT, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiMedShpNsMinCorRst(pMode, &GData.HiMedShpnsMinCorRst);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_MED_SCALE_CORING, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiMedShpNsSclCor(pMode, &GData.HiMedShpnsSclCor);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_SELECT, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiSelect(pMode, &GData.HiSelect);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_LUMA_BLEND, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiLumaBlend(pMode, &GData.HiLumaBlend);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HILI_COMBINE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiliCombine(pMode, &GData.HiLiCombine);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_MED_HIGH_FREQ_RECOVER, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiMidHighFreqRecover(pMode, &GData.HiMidHighFreqRecover);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HILI_SHARPEN_BOTH, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiliShpNsBoth(pMode, &GData.HiLiShpnsBoth);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HILi_SHARPEN_NOISE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiliShpNsNoise(pMode, &GData.HiLiShpnsNoise);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HILI_FIR, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiliShpNsFir(pMode, &GData.HiLiShpnsFir);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HILI_CORING, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiliShpNsCoring(pMode, &GData.HiLiShpnsCoring);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HILI_CORING_INDEX_SCALE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiliShpNsCorIdxScl(pMode, &GData.HiLiShpnsCorIdxScl);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HILI_MAX_CORING_RESULT, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiliShpNsMaxCorRst(pMode, &GData.HiLiShpnsMaxCorRst);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HILI_MIN_CORING_RESULT, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiliShpNsMinCorRst(pMode, &GData.HiLiShpnsMinCorRst);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HILI_SCALE_CORING, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiliShpNsSclCor(pMode, &GData.HiLiShpnsSclCor);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }


    return ret;
}

static INT32 AmbaItuner_HisoExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;
    ret |= AmbaItuner_HisoExecute_part1(pMode);
    ret |= AmbaItuner_HisoExecute_part2(pMode);
    ret |= AmbaItuner_HisoExecute_part3(pMode);
    if (AmbaItuner_Check_Reg(ITUNER_LI2_LUMA_ASF, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetLi2Asf(pMode, &GData.Li2Asf);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_LI2_ANTI_ALIASING, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetLi2AntiAliasing(pMode, &GData.Li2AntiAliasing);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_LI2_AUTO_BAD_PIXEL_CORRECTION, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetLi2DynamicBPC(pMode, &GData.Li2DynamicBpc);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_LI2_CFA_LEAKAGE_FILTER, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetLi2CfaLeakageFilter(pMode, &GData.Li2CfaLeakageFilter);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_LI2_CFA_NOISE_FILTER, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetLi2CfaNoiseFilter(pMode, &GData.Li2CfaNoiseFilter);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNRE_LI2_DEMOSAIC_FILTER, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetLi2Demosaic(pMode, &GData.Li2Demosaic);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_LI2_GB_GR_MISMATCH, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetLi2GrGbMismatch(pMode, &GData.Li2GrGbMismatch);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_LI2_SHARPEN_BOTH, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetLi2ShpNsBoth(pMode, &GData.Li2ShpnsBoth);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_LI2_SHARPEN_NOISE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetLi2ShpNsNoise(pMode, &GData.Li2ShpnsNoise);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_LI2_FIR, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetLi2ShpNsFir(pMode, &GData.Li2ShpnsFir);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_LI2_CORING, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetLi2ShpNsCoring(pMode, &GData.Li2ShpnsCoring);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_LI2_CORING_INDEX_SCALE, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetLi2ShpNsCorIdxScl(pMode, &GData.Li2ShpnsCorIdxScl);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_LI2_MAX_CORING_RESULT, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetLi2ShpNsMaxCorRst(pMode, &GData.Li2ShpnsMaxCorRst);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_LI2_MIN_CORING_RESULT, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetLi2ShpNsMinCorRst(pMode, &GData.Li2ShpnsMinCorRst);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_LI2_SCALE_CORING, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetLi2ShpNsSclCor(pMode, &GData.Li2ShpnsSclCor);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    if (AmbaItuner_Check_Reg(ITUNER_HI_NONSMOOTH_DETECT, SPECIAL_USE_NONE) >0 ) {
        rval= AmbaIK_SetHiNonSmoothDetect(pMode, &GData.HiNonsmoothDetect);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    }

    return (INT32)ret;
}

static INT32 AmbaItuner_ExecuteVideoLinear(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    rval = (UINT32)AmbaItuner_SystemExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_VinSensorExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2RawExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Yuv2YuvExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_MctfExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2yuvCalibExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}

static INT32 AmbaItuner_ExecuteVideoLinearCe(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    rval = (UINT32)AmbaItuner_SystemExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_VinSensorExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2RawExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Yuv2YuvExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_MctfExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2yuvCalibExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_CeExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}

static INT32 AmbaItuner_ExecuteVideoHdrExpo(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    rval = (UINT32)AmbaItuner_SystemExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_VinSensorExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2RawExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Yuv2YuvExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_MctfExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2yuvCalibExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_CeExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_VideoHdrExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}

static INT32 AmbaItuner_ExecuteVideoYuv2Yuv(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    rval = (UINT32)AmbaItuner_SystemExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Yuv2YuvExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_MctfExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Yuv2yuvCalibExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}

static INT32 AmbaItuner_ExecuteVideoYuv2YuvMipi(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    rval = (UINT32)AmbaItuner_SystemExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_MctfExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}
// hiso
static INT32 AmbaItuner_ExecuteStillLiso(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    rval = (UINT32)AmbaItuner_SystemExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_VinSensorExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2RawExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Yuv2YuvExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_CeExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_MctfExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2yuvCalibExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}

static INT32 AmbaItuner_ExecuteStillHiso(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    rval = (UINT32)AmbaItuner_SystemExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_VinSensorExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2RawExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Yuv2YuvExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_CeExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_HisoExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2yuvCalibExecute(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}

INT32 AmbaItuner_Execute(const AMBA_IK_MODE_CFG_s *pMode)
{
    INT32 ret = 0;
    AMBA_IK_ABILITY_s Ability = {0};
    (void)AmbaIK_GetContextAbility(pMode, &Ability);
    if (AmbaItuner_Is_Video_Preview_Tuning() == 0) {
        if (AmbaItuner_Check_Filter(&Ability) != 0) {
            ituner_print_str_5("%s() call _AmbaItuner_Check_Filter() Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_str_5("%s() need to fix", __func__, DC_S, DC_S, DC_S, DC_S);
        }
    }
    if (Ability.Pipe == AMBA_IK_PIPE_VIDEO) {
        if (Ability.VideoPipe == AMBA_IK_VIDEO_LINEAR) {
            ret = AmbaItuner_ExecuteVideoLinear(pMode);
        } else if (Ability.VideoPipe == AMBA_IK_VIDEO_LINEAR_CE) {
            ret = AmbaItuner_ExecuteVideoLinearCe(pMode);
        } else if (Ability.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_2) {
            ret = AmbaItuner_ExecuteVideoHdrExpo(pMode);
        } else if (Ability.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_3) {
            ret = AmbaItuner_ExecuteVideoHdrExpo(pMode);
        } else if (Ability.VideoPipe == AMBA_IK_VIDEO_Y2Y) {
            ret = AmbaItuner_ExecuteVideoYuv2Yuv(pMode);
        } else if (Ability.VideoPipe == AMBA_IK_VIDEO_Y2Y_MIPI) {
            ret = AmbaItuner_ExecuteVideoYuv2YuvMipi(pMode);
        } else {
            ituner_print_uint32_5("Ituner flow don't support your current ability:%d", Ability.VideoPipe, DC_U, DC_U, DC_U, DC_U);
            ret = -1;
        }
    } else if (Ability.Pipe== AMBA_IK_PIPE_STILL) {
// hiso
        if (Ability.StillPipe == AMBA_IK_STILL_BASE) {
            ret = AmbaItuner_ExecuteStillLiso(pMode);
        } else if (Ability.StillPipe == AMBA_IK_STILL_HISO) {
            ret = AmbaItuner_ExecuteStillHiso(pMode);
        } else {
            ret = -1;
        }
    } else {
        ret = -1;
    }
    return ret;
}

static INT32 AmbaItuner_VideoHdrRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    // FIXME:
    UINT32 rval;
    UINT32 ret = 0;
    Set_Filter_Valid Refresh_Set_Filter_Valid;

    if (AmbaItuner_Is_Video_Preview_Tuning() > 0) {
        Refresh_Set_Filter_Valid = Dummy_Set_Filter_Valid;
    } else {
        Refresh_Set_Filter_Valid = AmbaItuner_Set_Filter_Valid;
    }
    {
        Refresh_Set_Filter_Valid(ITUNER_VHDR_BLEND);
        rval= AmbaIK_GetHdrBlend(pMode, &GData.VideoHdr.HdrBlend);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
    }
    {
        AMBA_IK_WINDOW_SIZE_INFO_s WindowSizeInfo;
        Refresh_Set_Filter_Valid(ITUNER_VHDR_RAW_INFO);
        rval= AmbaIK_GetHdrRawOffset(pMode, &GData.VideoHdr.RawInfo.Offset);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        rval = AmbaIK_GetWindowSizeInfo(pMode, &WindowSizeInfo);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        GData.VideoHdr.RawInfo.RawAreaWidth[0] = WindowSizeInfo.VinSensor.Width;
        GData.VideoHdr.RawInfo.RawAreaWidth[1] = WindowSizeInfo.VinSensor.Width;
        GData.VideoHdr.RawInfo.RawAreaWidth[2] = WindowSizeInfo.VinSensor.Width;
        GData.VideoHdr.RawInfo.RawAreaHeight[0] = WindowSizeInfo.VinSensor.Height;
        GData.VideoHdr.RawInfo.RawAreaHeight[1] = WindowSizeInfo.VinSensor.Height;
        GData.VideoHdr.RawInfo.RawAreaHeight[2] = WindowSizeInfo.VinSensor.Height;
        ret |= rval;
    }
    {
        AMBA_IK_WINDOW_SIZE_INFO_s WindowSizeInfo;
        AMBA_IK_CFA_WINDOW_SIZE_INFO_s CfaWindowSizeInfo;
        Refresh_Set_Filter_Valid(ITUNER_SYSTEM_INFO);
        rval = AmbaIK_GetWindowSizeInfo(pMode, &WindowSizeInfo);
        SystemInfo.RawStartX           = WindowSizeInfo.VinSensor.StartX;
        SystemInfo.RawStartY           = WindowSizeInfo.VinSensor.StartY;
        SystemInfo.RawWidth            = WindowSizeInfo.VinSensor.Width;
        SystemInfo.RawHeight           = WindowSizeInfo.VinSensor.Height;
        SystemInfo.HSubSampleFactorNum = WindowSizeInfo.VinSensor.HSubSample.FactorNum;
        SystemInfo.HSubSampleFactorDen = WindowSizeInfo.VinSensor.HSubSample.FactorDen;
        SystemInfo.VSubSampleFactorNum = WindowSizeInfo.VinSensor.VSubSample.FactorNum;
        SystemInfo.VSubSampleFactorDen = WindowSizeInfo.VinSensor.VSubSample.FactorDen;
        SystemInfo.MainWidth           = WindowSizeInfo.Main.Width;
        SystemInfo.MainHeight          = WindowSizeInfo.Main.Height;
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;

        rval = AmbaIK_GetCfaWindowSizeInfo(pMode, &CfaWindowSizeInfo);
        SystemInfo.CfaWindowEnable     = CfaWindowSizeInfo.Enable;
        SystemInfo.CfaWidth            = CfaWindowSizeInfo.Cfa.Width;
        SystemInfo.CfaHeight           = CfaWindowSizeInfo.Cfa.Height;
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
    }
    if (SystemInfo.NumberOfExposures == 1U) {
        // misraC fix
    } else if (SystemInfo.NumberOfExposures == 2U) {
        {
            Refresh_Set_Filter_Valid(ITUNER_FRONT_END_WB_GAIN_EXP1);
            rval= AmbaIK_GetFeWbGain(pMode, &GData.FrontEndWbGain[1], 1UL);
            if ((INT32)rval != 0) {
                ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            ret |= rval;
        }
        {
            Refresh_Set_Filter_Valid(ITUNER_STATIC_BLACK_LEVEL_EXP1);
            rval= AmbaIK_GetFeStaticBlc(pMode, &GData.StaticBlackLevel[1], 1UL);
            if ((INT32)rval != 0) {
                ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            ret |= rval;
        }
    } else if (SystemInfo.NumberOfExposures == 3U) {
        {
            Refresh_Set_Filter_Valid(ITUNER_FRONT_END_WB_GAIN_EXP1);
            rval= AmbaIK_GetFeWbGain(pMode, &GData.FrontEndWbGain[1], 1UL);
            if ((INT32)rval != 0) {
                ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            ret |= rval;
        }
        {
            Refresh_Set_Filter_Valid(ITUNER_FRONT_END_WB_GAIN_EXP2);
            rval= AmbaIK_GetFeWbGain(pMode, &GData.FrontEndWbGain[2], 2UL);
            if ((INT32)rval != 0) {
                ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            ret |= rval;
        }
        {
            Refresh_Set_Filter_Valid(ITUNER_STATIC_BLACK_LEVEL_EXP1);
            rval= AmbaIK_GetFeStaticBlc(pMode, &GData.StaticBlackLevel[1], 1UL);
            if ((INT32)rval != 0) {
                ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            ret |= rval;
        }
        {
            Refresh_Set_Filter_Valid(ITUNER_STATIC_BLACK_LEVEL_EXP2);
            rval= AmbaIK_GetFeStaticBlc(pMode, &GData.StaticBlackLevel[2], 2UL);
            if ((INT32)rval != 0) {
                ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
            ret |= rval;
        }
    } else {/*15.7*/};

    return (INT32)ret;
}

static INT32 AmbaItuner_SystemRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval;
    Set_Filter_Valid Refresh_Set_Filter_Valid;
    AMBA_IK_ABILITY_s Ability = {0};

    (void)AmbaIK_GetContextAbility(pMode, &Ability);

    if (AmbaItuner_Is_Video_Preview_Tuning() >0 ) {
        Refresh_Set_Filter_Valid = Dummy_Set_Filter_Valid;
    } else {
        Refresh_Set_Filter_Valid = AmbaItuner_Set_Filter_Valid;
    }

    {
        UINT32 FlipInfo;
        AMBA_IK_WINDOW_SIZE_INFO_s WindowSizeInfo;
        AMBA_IK_CFA_WINDOW_SIZE_INFO_s CfaWindowSizeInfo;
        Refresh_Set_Filter_Valid(ITUNER_SYSTEM_INFO);
        rval = AmbaIK_GetWindowSizeInfo(pMode, &WindowSizeInfo);
        SystemInfo.RawStartX           = WindowSizeInfo.VinSensor.StartX;
        SystemInfo.RawStartY           = WindowSizeInfo.VinSensor.StartY;
        SystemInfo.RawWidth            = WindowSizeInfo.VinSensor.Width;
        SystemInfo.RawHeight           = WindowSizeInfo.VinSensor.Height;
        SystemInfo.HSubSampleFactorNum = WindowSizeInfo.VinSensor.HSubSample.FactorNum;
        SystemInfo.HSubSampleFactorDen = WindowSizeInfo.VinSensor.HSubSample.FactorDen;
        SystemInfo.VSubSampleFactorNum = WindowSizeInfo.VinSensor.VSubSample.FactorNum;
        SystemInfo.VSubSampleFactorDen = WindowSizeInfo.VinSensor.VSubSample.FactorDen;
        SystemInfo.MainWidth           = WindowSizeInfo.Main.Width;
        SystemInfo.MainHeight          = WindowSizeInfo.Main.Height;

        // Update Flip Info
        (void)AmbaIK_GetFlipMode(pMode, &FlipInfo);
        if (FlipInfo == 0UL) {
            SystemInfo.FlipH = 0UL;
            SystemInfo.FlipV = 0UL;
        } else if (FlipInfo == IK_FLIP_YUV_H) {
            SystemInfo.FlipH = 2UL;
            SystemInfo.FlipV = 0UL;
        } else if (FlipInfo == IK_FLIP_RAW_V) {
            SystemInfo.FlipH = 0UL;
            SystemInfo.FlipV = 1UL;
        } else if (FlipInfo == IK_FLIP_RAW_V_YUV_H) {
            SystemInfo.FlipH = 2UL;
            SystemInfo.FlipV = 1UL;
        } else { // rest of flip mode is not supported
            SystemInfo.FlipH = 100UL;
            SystemInfo.FlipV = 100UL;
        }
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;

        rval = AmbaIK_GetCfaWindowSizeInfo(pMode, &CfaWindowSizeInfo);
        SystemInfo.CfaWindowEnable     = CfaWindowSizeInfo.Enable;
        SystemInfo.CfaWidth            = CfaWindowSizeInfo.Cfa.Width;
        SystemInfo.CfaHeight           = CfaWindowSizeInfo.Cfa.Height;
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
    }

    if (Ability.Pipe == AMBA_IK_PIPE_VIDEO) {
        AMBA_IK_CE_EXT_HDS_s CeUseExternalHds;
        rval = AmbaIK_GetCeUseExtHds(pMode, &CeUseExternalHds);

        SystemInfo.UseExternalHdsMode  = CeUseExternalHds.Mode;
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
    }

    if (AmbaItuner_Get_SaveInternalParam() > 0U ) {
        Refresh_Set_Filter_Valid(ITUNER_INTERNAL_INFO);
    }

    {
        //Note: wb sim information
        Refresh_Set_Filter_Valid(ITUNER_WB_SIM_INFO);
    }

    return (INT32)ret;
}

static INT32 AmbaItuner_Raw2yuvCalibRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval;
    Set_Filter_Valid Refresh_Set_Filter_Valid;

    if (AmbaItuner_Is_Video_Preview_Tuning() >0 ) {
        Refresh_Set_Filter_Valid = Dummy_Set_Filter_Valid;
    } else {
        Refresh_Set_Filter_Valid = AmbaItuner_Set_Filter_Valid;
    }

    {
        // Note: active
        Refresh_Set_Filter_Valid(ITUNER_ACTIVE);
        rval= AmbaIK_GetVinActiveWin(pMode, &GData.Active);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
    }

    {
        // Note: dummy
        Refresh_Set_Filter_Valid(ITUNER_DUMMY);
        rval= AmbaIK_GetDummyMarginRange(pMode, &GData.Dummy);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
    }

    {
        // Note: dzoom
        Refresh_Set_Filter_Valid(ITUNER_DZOOM);
        rval= AmbaIK_GetDzoomInfo(pMode, &GData.Dzoom);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
    }

    {
        // Note: warp_correction
        UINT32 enable = 0;
        rval= AmbaIK_GetWarpInfo(pMode, &GData.WarpCompensationInfo);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
        (void)AmbaIK_GetWarpEnb(pMode, &enable);
        Refresh_Set_Filter_Valid(ITUNER_WARP_COMPENSATION);
        if (enable==0UL) {
            (void)ituner_memset(&GData.WarpCompensation, 0x0, sizeof(ITUNER_WARP_s));
        } else {
            GData.WarpCompensation.Enable = (UINT8)enable;
            GData.WarpCompensation.CalibVersion = GData.WarpCompensationInfo.Version;
            GData.WarpCompensation.HorizontalGridNumber = GData.WarpCompensationInfo.HorGridNum;
            GData.WarpCompensation.VerticalGridNumber = GData.WarpCompensationInfo.VerGridNum;
            GData.WarpCompensation.TileWidthExponent = GData.WarpCompensationInfo.TileWidthExp;
            GData.WarpCompensation.TileHeightExponent = GData.WarpCompensationInfo.TileHeightExp;
            GData.WarpCompensation.VinSensorStartX = GData.WarpCompensationInfo.VinSensorGeo.StartX;
            GData.WarpCompensation.VinSensorStartY = GData.WarpCompensationInfo.VinSensorGeo.StartY;
            GData.WarpCompensation.VinSensorWidth = GData.WarpCompensationInfo.VinSensorGeo.Width;
            GData.WarpCompensation.VinSensorHeight = GData.WarpCompensationInfo.VinSensorGeo.Height;
            GData.WarpCompensation.VinSensorHSubSampleFactorNum = (UINT8)GData.WarpCompensationInfo.VinSensorGeo.HSubSample.FactorNum;
            GData.WarpCompensation.VinSensorHSubSampleFactorDen = (UINT8)GData.WarpCompensationInfo.VinSensorGeo.HSubSample.FactorDen;
            GData.WarpCompensation.VinSensorVSubSampleFactorNum = (UINT8)GData.WarpCompensationInfo.VinSensorGeo.VSubSample.FactorNum;
            GData.WarpCompensation.VinSensorVSubSampleFactorDen = (UINT8)GData.WarpCompensationInfo.VinSensorGeo.VSubSample.FactorDen;
            GData.WarpCompensation.Enb2StageCompensation= GData.WarpCompensationInfo.Enb_2StageCompensation;
        }
    }

    {
        // Note: chromatic_aberration_correction
        UINT32 enable = 0;
        rval= AmbaIK_GetCawarpInfo(pMode, &GData.CaWarpCompensationInfo);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
        (void)AmbaIK_GetCawarpEnb(pMode, &enable);
        Refresh_Set_Filter_Valid(ITUNER_CHROMA_ABERRATION_INFO);
        if (enable==0UL) {
            (void)ituner_memset(&GData.ChromaAberrationInfo, 0x0, sizeof(ITUNER_CHROMA_ABERRATION_s));
        } else {
            GData.ChromaAberrationInfo.Enable = (UINT8)enable;
            GData.ChromaAberrationInfo.CalibVersion = GData.CaWarpCompensationInfo.Version;
            GData.ChromaAberrationInfo.HorizontalGridNumber = GData.CaWarpCompensationInfo.HorGridNum;
            GData.ChromaAberrationInfo.VerticalGridNumber = GData.CaWarpCompensationInfo.VerGridNum;
            GData.ChromaAberrationInfo.TileWidthExponent = GData.CaWarpCompensationInfo.TileWidthExp;
            GData.ChromaAberrationInfo.TileHeightExponent = GData.CaWarpCompensationInfo.TileHeightExp;
            GData.ChromaAberrationInfo.VinSensorStartX = GData.CaWarpCompensationInfo.VinSensorGeo.StartX;
            GData.ChromaAberrationInfo.VinSensorStartY = GData.CaWarpCompensationInfo.VinSensorGeo.StartY;
            GData.ChromaAberrationInfo.VinSensorWidth = GData.CaWarpCompensationInfo.VinSensorGeo.Width;
            GData.ChromaAberrationInfo.VinSensorHeight = GData.CaWarpCompensationInfo.VinSensorGeo.Height;
            GData.ChromaAberrationInfo.VinSensorHSubSampleFactorNum = (UINT8)GData.CaWarpCompensationInfo.VinSensorGeo.HSubSample.FactorNum;
            GData.ChromaAberrationInfo.VinSensorHSubSampleFactorDen = (UINT8)GData.CaWarpCompensationInfo.VinSensorGeo.HSubSample.FactorDen;
            GData.ChromaAberrationInfo.VinSensorVSubSampleFactorNum = (UINT8)GData.CaWarpCompensationInfo.VinSensorGeo.VSubSample.FactorNum;
            GData.ChromaAberrationInfo.VinSensorVSubSampleFactorDen = (UINT8)GData.CaWarpCompensationInfo.VinSensorGeo.VSubSample.FactorDen;
            GData.ChromaAberrationInfo.Enb2StageCompensation = (UINT32)GData.CaWarpCompensationInfo.Enb2StageCompensation;
        }
    }
#if 0
    {
        // Note: chromatic_aberration_correction_bypass
        extern UINT32 AmbaIK_GetCaWarpInternal(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CAWARP_INTERNAL_INFO_s *pCawarpInternal);
        rval = AmbaIK_GetCaWarpInternal(pMode, &GData.CAInternal);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
        Refresh_Set_Filter_Valid(ITUNER_CHROMA_ABERRATION_INFO_INTERNAL);
        GData.ChromaAberrationInfoInternal.HorzPassGridArrayHeight = GData.CAInternal.HorzPassGridArrayHeight;
        GData.ChromaAberrationInfoInternal.HorzPassGridArrayWidth = GData.CAInternal.HorzPassGridArrayWidth;
        GData.ChromaAberrationInfoInternal.HorzPassHorzGridSpacingExponent = GData.CAInternal.HorzPassHorzGridSpacingExponent;
        GData.ChromaAberrationInfoInternal.HorzPassVertGridSpacingExponent = GData.CAInternal.HorzPassVertGridSpacingExponent;
        GData.ChromaAberrationInfoInternal.HorzWarpEnable = GData.CAInternal.HorzWarpEnable;
        GData.ChromaAberrationInfoInternal.VertPassGridArrayHeight = GData.CAInternal.VertPassGridArrayHeight;
        GData.ChromaAberrationInfoInternal.VertPassGridArrayWidth = GData.CAInternal.VertPassGridArrayWidth;
        GData.ChromaAberrationInfoInternal.VertPassHorzGridSpacingExponent = GData.CAInternal.VertPassHorzGridSpacingExponent;
        GData.ChromaAberrationInfoInternal.VertPassVertGridSpacingExponent = GData.CAInternal.VertPassVertGridSpacingExponent;
        GData.ChromaAberrationInfoInternal.VertWarpEnable = GData.CAInternal.VertWarpEnable;
    }
#endif
    {
        // Note: vignette
        UINT32 enable = 0;
        rval= AmbaIK_GetVignette(pMode, &GData.VignetteCompensation.VignetteInfo);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
        (void)AmbaIK_GetVignetteEnb(pMode, &enable);
        Refresh_Set_Filter_Valid(ITUNER_VIGNETTE_COMPENSATION);
        if (enable==0UL) {
            (void)ituner_memset(&GData.VignetteCompensation, 0x0, sizeof(ITUNER_VIGNETTE_s));
        } else {
            GData.VignetteCompensation.Enable = (UINT8)enable;
        }
    }

    {
        // Note: fpn
        UINT32 enable = 0;
        rval= AmbaIK_GetStaticBadPxlCor(pMode, &GData.SbpCorr);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
        (void)AmbaIK_GetStaticBadPxlCorEnb(pMode, &enable);
        Refresh_Set_Filter_Valid(ITUNER_STATIC_BAD_PIXEL_CORRECTION);
        if (enable==0UL) {
            (void)ituner_memset(&GData.StaticBadPixelCorrection, 0x0, sizeof(ITUNER_FPN_s));
        } else {
            GData.StaticBadPixelCorrection.Enable = (UINT8)enable;
            GData.StaticBadPixelCorrection.CalibVersion = GData.SbpCorr.CalibSbpInfo.Version;
            GData.StaticBadPixelCorrection.CalibVinStartX = GData.SbpCorr.VinSensorGeo.StartX;
            GData.StaticBadPixelCorrection.CalibVinStartY = GData.SbpCorr.VinSensorGeo.StartY;
            GData.StaticBadPixelCorrection.CalibVinWidth = GData.SbpCorr.VinSensorGeo.Width;
            GData.StaticBadPixelCorrection.CalibVinHeight = GData.SbpCorr.VinSensorGeo.Height;
            GData.StaticBadPixelCorrection.CalibVinHSubSampleFactorNum = (UINT8)GData.SbpCorr.VinSensorGeo.HSubSample.FactorNum;
            GData.StaticBadPixelCorrection.CalibVinHSubSampleFactorDen = (UINT8)GData.SbpCorr.VinSensorGeo.HSubSample.FactorDen;
            GData.StaticBadPixelCorrection.CalibVinVSubSampleFactorNum = (UINT8)GData.SbpCorr.VinSensorGeo.VSubSample.FactorNum;
            GData.StaticBadPixelCorrection.CalibVinVSubSampleFactorDen = (UINT8)GData.SbpCorr.VinSensorGeo.VSubSample.FactorDen;
        }
    }

    return (INT32)ret;
}

static INT32 AmbaItuner_Yuv2yuvCalibRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval;
    Set_Filter_Valid Refresh_Set_Filter_Valid;

    if (AmbaItuner_Is_Video_Preview_Tuning() >0 ) {
        Refresh_Set_Filter_Valid = Dummy_Set_Filter_Valid;
    } else {
        Refresh_Set_Filter_Valid = AmbaItuner_Set_Filter_Valid;
    }

    {
        // Note: active
        Refresh_Set_Filter_Valid(ITUNER_ACTIVE);
        rval= AmbaIK_GetVinActiveWin(pMode, &GData.Active);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
    }

    {
        // Note: dzoom
        Refresh_Set_Filter_Valid(ITUNER_DZOOM);
        rval= AmbaIK_GetDzoomInfo(pMode, &GData.Dzoom);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
    }

    {
        // Note: warp_correction
        UINT32 enable = 0;
        rval= AmbaIK_GetWarpInfo(pMode, &GData.WarpCompensationInfo);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
        (void)AmbaIK_GetWarpEnb(pMode, &enable);
        Refresh_Set_Filter_Valid(ITUNER_WARP_COMPENSATION);
        if (enable==0UL) {
            (void)ituner_memset(&GData.WarpCompensation, 0x0, sizeof(ITUNER_WARP_s));
        } else {
            GData.WarpCompensation.Enable = (UINT8)enable;
            GData.WarpCompensation.CalibVersion = GData.WarpCompensationInfo.Version;
            GData.WarpCompensation.HorizontalGridNumber = GData.WarpCompensationInfo.HorGridNum;
            GData.WarpCompensation.VerticalGridNumber = GData.WarpCompensationInfo.VerGridNum;
            GData.WarpCompensation.TileWidthExponent = GData.WarpCompensationInfo.TileWidthExp;
            GData.WarpCompensation.TileHeightExponent = GData.WarpCompensationInfo.TileHeightExp;
            GData.WarpCompensation.VinSensorStartX = GData.WarpCompensationInfo.VinSensorGeo.StartX;
            GData.WarpCompensation.VinSensorStartY = GData.WarpCompensationInfo.VinSensorGeo.StartY;
            GData.WarpCompensation.VinSensorWidth = GData.WarpCompensationInfo.VinSensorGeo.Width;
            GData.WarpCompensation.VinSensorHeight = GData.WarpCompensationInfo.VinSensorGeo.Height;
            GData.WarpCompensation.VinSensorHSubSampleFactorNum = (UINT8)GData.WarpCompensationInfo.VinSensorGeo.HSubSample.FactorNum;
            GData.WarpCompensation.VinSensorHSubSampleFactorDen = (UINT8)GData.WarpCompensationInfo.VinSensorGeo.HSubSample.FactorDen;
            GData.WarpCompensation.VinSensorVSubSampleFactorNum = (UINT8)GData.WarpCompensationInfo.VinSensorGeo.VSubSample.FactorNum;
            GData.WarpCompensation.VinSensorVSubSampleFactorDen = (UINT8)GData.WarpCompensationInfo.VinSensorGeo.VSubSample.FactorDen;
            GData.WarpCompensation.Enb2StageCompensation= GData.WarpCompensationInfo.Enb_2StageCompensation;
        }
    }

    return (INT32)ret;
}

INT32 AmbaItuner_FrontEndToneRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0UL;
#if DSP_VIN_GET
    extern UINT32 AmbaDSP_LiveviewGetIDspCfg(UINT16 ViewZoneId, ULONG *CfgAddr);//__attribute__((weak));
    UINT32 ViewZoneID = pMode->ContextId;
    ULONG pCfgAddr = 0UL;
    const void* pCfgAddrVoid;
    ITUNER_IDSP_s IdspInfo;
    const UINT16 *pMisraU16;
    const UINT32 *pMisraU32_01, *pMisraU32_02;
    const UINT8 *pMisraU8_01, *pMisraU8_02;
    UINT32 MisraU32;

    if (CompandCount==0UL) {
        CompandCount++;
        (void)ituner_memset(&Compand[0], 0x0, sizeof(UINT32)*IK_NUM_COMPAND_TABLE);
    }

    // idsp dump
    (void)AmbaDSP_LiveviewGetIDspCfg((UINT16)ViewZoneID, &pCfgAddr);
    if (pCfgAddr==0UL) {
        ituner_print_uint32_5("[ituner][error] pCfgAddr is null", DC_U, DC_U, DC_U, DC_U, DC_U);
        // No return error
    } else {
        (void)ituner_memcpy(&pCfgAddrVoid, &pCfgAddr, sizeof(void*));
        (void)ituner_memcpy(&IdspInfo, pCfgAddrVoid, sizeof(ITUNER_IDSP_s));
        // Compand
        GData.FeToneCurve.Compand.CompandEnable = GData.FeToneCurve.Decompand.DecompandEnable;
        if (IdspInfo.Sec1[3]==0UL) {
            ituner_print_uint32_5("[ituner][note] Vin compand is null, it should be in rawenc status", DC_U, DC_U, DC_U, DC_U, DC_U);
            GData.FeToneCurve.Compand.CompandEnable = CompandEnable;
            (void)ituner_memcpy(&GData.FeToneCurve.Compand.CompandTable[0], &Compand[0], sizeof(UINT32)*IK_NUM_COMPAND_TABLE);
        } else {
            GData.FeToneCurve.Compand.CompandEnable = 1UL;
            MisraU32 = IdspInfo.Sec1[3];
            (void)ituner_memcpy(&pMisraU16, &MisraU32, sizeof(UINT32));
            (void)ituner_memcpy(GData.FeToneCurve.Compand.CompandTable, pMisraU16, sizeof(UINT32)*IK_NUM_COMPAND_TABLE);
        }
        // Decompand
        if (IdspInfo.Sec1[4]==0UL) {
            ituner_print_uint32_5("[ituner][note] Vin decompand is null, it should be in rawenc status", DC_U, DC_U, DC_U, DC_U, DC_U);
            //(void)ituner_memcpy(&GData.FeToneCurve.Decompand.DecompandTable[0], &Decompand[0], sizeof(UINT32)*IK_NUM_DECOMPAND_TABLE);
        } else {
            MisraU32 = IdspInfo.Sec1[4]; // Decompand
            (void)ituner_memcpy(&pMisraU32_01, &MisraU32, sizeof(UINT32));
            (void)ituner_memcpy(&pMisraU8_01, &pMisraU32_01, sizeof(UINT8*));
            pMisraU32_02 = &GData.FeToneCurve.Decompand.DecompandTable[0];
            (void)ituner_memcpy(&pMisraU8_02, &pMisraU32_02, sizeof(UINT8*));
            if (ituner_u8ncmp(pMisraU8_01, pMisraU8_02, IK_NUM_COMPAND_TABLE*sizeof(UINT32)) == 0) {
                // pass the check
            } else {
                ituner_print_uint32_5("[ituner][error] VinTone is not equal to FrontEndTone", DC_U, DC_U, DC_U, DC_U, DC_U);
                rval = TUNE_ERROR_GENERAL;
            }
        }
    }
#else
    // misraC fix
    ituner_print_uint32_5("[ituner][note] DSP_VIN_GET is null Ctx_id:%d", pMode->ContextId, DC_U, DC_U, DC_U, DC_U);
#endif
    return (INT32)rval;
}

static INT32 AmbaItuner_VinSensorRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0;

    ret = AmbaIK_GetVinSensorInfo(pMode, &GData.SensorInfo);
    if ((INT32)ret != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    } else {
        SystemInfo.RawResolution = GData.SensorInfo.SensorResolution;
        SystemInfo.RawBayer = GData.SensorInfo.SensorPattern;
        SystemInfo.SensorReadoutMode = GData.SensorInfo.SensorReadoutMode;
        SystemInfo.CompressedRaw = GData.SensorInfo.Compression;
        SystemInfo.SensorMode = GData.SensorInfo.SensorMode;
        SystemInfo.CompressionOffset = (INT32)GData.SensorInfo.CompressionOffset;
        if ((SystemInfo.Ability == AMBA_IK_VIDEO_LINEAR_CE) || (SystemInfo.Ability == AMBA_IK_VIDEO_LINEAR_CE_MD)) {
            SystemInfo.NumberOfExposures = 1UL;
        } else if ((SystemInfo.Ability == AMBA_IK_VIDEO_HDR_EXPO_2) || (SystemInfo.Ability == AMBA_IK_VIDEO_HDR_EXPO_2_MD)) {
            SystemInfo.NumberOfExposures = 2UL;
        } else if ((SystemInfo.Ability == AMBA_IK_VIDEO_HDR_EXPO_3) || (SystemInfo.Ability == AMBA_IK_VIDEO_HDR_EXPO_3_MD)) {
            SystemInfo.NumberOfExposures = 3UL;
        } else {
            SystemInfo.NumberOfExposures = 0UL;
        }
    }

    return (INT32)ret;
}

static INT32 AmbaItuner_Raw2RawRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;
    Set_Filter_Valid Refresh_Set_Filter_Valid;
    AMBA_IK_ABILITY_s Ability = {0};  // for Rgbir

    (void)AmbaIK_GetContextAbility(pMode, &Ability);
    if (AmbaItuner_Is_Video_Preview_Tuning() >0 ) {
        Refresh_Set_Filter_Valid = Dummy_Set_Filter_Valid;
    } else {
        Refresh_Set_Filter_Valid = AmbaItuner_Set_Filter_Valid;
    }

    // Note: front_end_tone_curve
    Refresh_Set_Filter_Valid(ITUNER_FRONT_END_TONE_CURVE);
    rval = (UINT32)AmbaIK_GetFeToneCurve(pMode, &GData.FeToneCurve.Decompand);
    rval |= (UINT32)AmbaItuner_FrontEndToneRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: static_bad_level
    Refresh_Set_Filter_Valid(ITUNER_STATIC_BLACK_LEVEL_EXP0);
    rval = (UINT32)AmbaIK_GetFeStaticBlc(pMode, &GData.StaticBlackLevel[0], 0UL);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: dynamic_bad_pixel_correction
    Refresh_Set_Filter_Valid(ITUNER_DYNAMIC_BAD_PIXEL_CORRECTION);
    rval = (UINT32)AmbaIK_GetDynamicBadPxlCor(pMode, &GData.DynamicBadPixelCorrection);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: cfa_leakage_filter
    Refresh_Set_Filter_Valid(ITUNER_CFA_LEAKAGE_FILTER);
    rval = (UINT32)AmbaIK_GetCfaLeakageFilter(pMode, &GData.CfaLeakageFilter);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: cfa_noise_filter
    Refresh_Set_Filter_Valid(ITUNER_CFA_NOISE_FILTER);
    rval = (UINT32)AmbaIK_GetCfaNoiseFilter(pMode, &GData.CfaNoiseFilter);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: anti_aliasing
    Refresh_Set_Filter_Valid(ITUNER_ANTI_ALIASING_STRENGTH);
    rval = (UINT32)AmbaIK_GetAntiAliasing(pMode, &GData.AntiAliasing);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: front_end_wb_gain
    Refresh_Set_Filter_Valid(ITUNER_FRONT_END_WB_GAIN_EXP0);
    rval = (UINT32)AmbaIK_GetFeWbGain(pMode, &GData.FrontEndWbGain[0], 0UL);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: after_ce_wb_gain
    Refresh_Set_Filter_Valid(ITUNER_AFTER_CE_WB_GAIN);
    rval = (UINT32)AmbaIK_GetAfterCeWbGain(pMode, &GData.AfterCeWbGain);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: before_ce_wb_gain
    Refresh_Set_Filter_Valid(ITUNER_BEFORE_CE_WB_GAIN);
    rval = (UINT32)AmbaIK_GetBeforeCeWbGain(pMode, &GData.BeforeCeWbGain);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: rgb_ir
    if (Ability.Pipe == AMBA_IK_PIPE_VIDEO) { //FIXME
        Refresh_Set_Filter_Valid(ITUNER_RGB_IR);
        rval = (UINT32)AmbaIK_GetRgbIr(pMode, &GData.RgbIr);
        if ((INT32)rval != 0) {
            ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= rval;
    }

    // Note: li_demosaic
    Refresh_Set_Filter_Valid(ITUNER_DEMOSAIC_FILTER);
    rval = (UINT32)AmbaIK_GetDemosaic(pMode, &GData.DemosaicFilter);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: li_gb_gr_mismatch_correct
    Refresh_Set_Filter_Valid(ITUNER_GB_GR_MISMATCH);
    rval = (UINT32)AmbaIK_GetGrgbMismatch(pMode, &GData.GbGrMismatch);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}

static INT32 AmbaItuner_Yuv2YuvRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;
    Set_Filter_Valid Refresh_Set_Filter_Valid;

    if (AmbaItuner_Is_Video_Preview_Tuning() >0 ) {
        Refresh_Set_Filter_Valid = Dummy_Set_Filter_Valid;
    } else {
        Refresh_Set_Filter_Valid = AmbaItuner_Set_Filter_Valid;
    }

    // Note: pre_cc_gain
    Refresh_Set_Filter_Valid(ITUNER_PRE_CC_GAIN);
    rval = (UINT32)AmbaIK_GetPreCcGain(pMode, &GData.PreCcGain);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: color_correction
    rval = (UINT32)AmbaIK_GetColorCorrectionReg(pMode, &GData.CcReg);
    rval |= (UINT32)AmbaIK_GetColorCorrection(pMode, &GData.CcThreeD);
    if((rval == IK_OK) &&
       (GData.CcReg.RegSettingTable[0]!=0U) &&
       (GData.CcReg.RegSettingTable[1]!=0U) &&
       (GData.CcReg.RegSettingTable[2]!=0U) &&
       (GData.CcThreeD.MatrixThreeDTable[0]!=0U) &&
       (GData.CcThreeD.MatrixThreeDTable[1]!=0U)) {
        Refresh_Set_Filter_Valid(ITUNER_COLOR_CORRECTION);
    } else {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        ret |= rval;
    }

    // Note: tone_curve
    Refresh_Set_Filter_Valid(ITUNER_TONE_CURVE);
    rval = (UINT32)AmbaIK_GetToneCurve(pMode, &GData.ToneCurve);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: rgb_to_yuv_matrix
    Refresh_Set_Filter_Valid(ITUNER_RGB_TO_YUV_MATRIX);
    rval = (UINT32)AmbaIK_GetRgbToYuvMatrix(pMode, &GData.RgbToYuvMatrix);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: chroma_scale
    Refresh_Set_Filter_Valid(ITUNER_CHROMA_SCALE);
    rval = (UINT32)AmbaIK_GetChromaScale(pMode, &GData.ChromaScale);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li_chroma_median_filter
    Refresh_Set_Filter_Valid(ITUNER_CHROMA_MEDIAN_FILTER);
    rval = (UINT32)AmbaIK_GetChromaMedianFilter(pMode, &GData.ChromaMedianFilter);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li_chroma_filter
    Refresh_Set_Filter_Valid(ITUNER_CHROMA_FILTER);
    rval = (UINT32)AmbaIK_GetChromaFilter(pMode, &GData.ChromaFilter);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li_use_1st_sharpen
    Refresh_Set_Filter_Valid(ITUNER_SHP_A_OR_SPATIAL_FILTE_SELECT);
    rval = (UINT32)AmbaIK_GetFirstLumaProcMode(pMode, &GData.FirstLumaProcessMode);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li_asf
    if (GData.FirstLumaProcessMode.UseSharpenNotAsf == 0UL) {
        Refresh_Set_Filter_Valid(ITUNER_ASF_INFO);
        rval = (UINT32)AmbaIK_GetAdvSpatFltr(pMode, &GData.AsfInfo.AsfInfo);
        check_rval_not_equal_0(rval, __func__, __LINE__);
        ret |= rval;
    } else if (GData.FirstLumaProcessMode.UseSharpenNotAsf == 1UL) {
        {
            // Note: li_sharpen_noise_filter_both
            Refresh_Set_Filter_Valid(ITUNER_SHARPEN_BOTH);
            rval = (UINT32)AmbaIK_GetFstShpNsBoth(pMode, &GData.SharpenBoth.BothInfo);
            check_rval_not_equal_0(rval, __func__, __LINE__);
            ret |= rval;
        }
        {
            // Note: li_sharpen_noise_filter_noise
            Refresh_Set_Filter_Valid(ITUNER_SHARPEN_NOISE);
            rval = (UINT32)AmbaIK_GetFstShpNsNoise(pMode, &GData.SharpenNoise);
            check_rval_not_equal_0(rval, __func__, __LINE__);
            ret |= rval;
        }
        {
            // Note: li_sharpen_noise_filter_sharpen
            Refresh_Set_Filter_Valid(ITUNER_FIR);
            rval = (UINT32)AmbaIK_GetFstShpNsFir(pMode, &GData.Fir);
            check_rval_not_equal_0(rval, __func__, __LINE__);
            ret |= rval;
        }
        {
            // Note: li_sharpen_noise_filter_sharpen_coring
            Refresh_Set_Filter_Valid(ITUNER_CORING);
            rval = (UINT32)AmbaIK_GetFstShpNsCoring(pMode, &GData.Coring);
            check_rval_not_equal_0(rval, __func__, __LINE__);
            ret |= rval;
        }
        {
            // Note: li_sharpen_noise_filter_sharpen_coring_index_scale
            Refresh_Set_Filter_Valid(ITUNER_CORING_INDEX_SCALE);
            rval = (UINT32)AmbaIK_GetFstShpNsCorIdxScl(pMode, &GData.CoringIndexScale);
            check_rval_not_equal_0(rval, __func__, __LINE__);
            ret |= rval;
        }
        {
            // Note: li_sharpen_noise_filter_sharpen_min_coring_result
            Refresh_Set_Filter_Valid(ITUNER_MIN_CORING_RESULT);
            rval = (UINT32)AmbaIK_GetFstShpNsMinCorRst(pMode, &GData.MinCoringResult);
            check_rval_not_equal_0(rval, __func__, __LINE__);
            ret |= rval;
        }
        {
            // Note: li_sharpen_noise_filter_sharpen_max_coring_result
            Refresh_Set_Filter_Valid(ITUNER_MAX_CORING_RESULT);
            rval = (UINT32)AmbaIK_GetFstShpNsMaxCorRst(pMode, &GData.MaxCoringResult);
            check_rval_not_equal_0(rval, __func__, __LINE__);
            ret |= rval;
        }
        {
            // Note: li_sharpen_noise_filter_sharpen_scale_coring
            Refresh_Set_Filter_Valid(ITUNER_SCALE_CORING);
            rval = (UINT32)AmbaIK_GetFstShpNsSclCor(pMode, &GData.ScaleCoring);
            check_rval_not_equal_0(rval, __func__, __LINE__);
            ret |= rval;
        }
    } else {
        // misraC fix
    }

    return (INT32)ret;
}

static INT32 AmbaItuner_CeRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;
    Set_Filter_Valid Refresh_Set_Filter_Valid;

    if (AmbaItuner_Is_Video_Preview_Tuning() >0 ) {
        Refresh_Set_Filter_Valid = Dummy_Set_Filter_Valid;
    } else {
        Refresh_Set_Filter_Valid = AmbaItuner_Set_Filter_Valid;
    }

    Refresh_Set_Filter_Valid(ITUNER_CONTRAST_ENHANCE);
    rval = (UINT32)AmbaIK_GetCe(pMode, &GData.ContrastEnhance.ContrastEnhance);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;
    //TBD
    rval = (UINT32)AmbaIK_GetCeInputTable(pMode, &GData.ContrastEnhance.ContrastEnhanceInputTable);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;
    rval = (UINT32)AmbaIK_GetCeOutputTable(pMode, &GData.ContrastEnhance.ContrastEnhanceOutTable);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}

static INT32 AmbaItuner_MctfRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;
    Set_Filter_Valid Refresh_Set_Filter_Valid;

    if (AmbaItuner_Is_Video_Preview_Tuning() >0 ) {
        Refresh_Set_Filter_Valid = Dummy_Set_Filter_Valid;
    } else {
        Refresh_Set_Filter_Valid = AmbaItuner_Set_Filter_Valid;
    }

    // Note: fnlshpns_filter_both
    Refresh_Set_Filter_Valid(ITUNER_FINAL_SHARPEN_BOTH);
    rval = (UINT32)AmbaIK_GetFnlShpNsBoth(pMode, &GData.FinalSharpenBoth.BothInfo);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaIK_GetFnlShpNsBothTdt(pMode, &GData.FinalSharpenBoth.ThreedTable);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: fnlshpns_filter_noise
    Refresh_Set_Filter_Valid(ITUNER_FINAL_SHARPEN_NOISE);
    rval = (UINT32)AmbaIK_GetFnlShpNsNoise(pMode, &GData.FinalSharpenNoise);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: fnlshpns_filter_sharpen
    Refresh_Set_Filter_Valid(ITUNER_FINAL_FIR);
    rval = (UINT32)AmbaIK_GetFnlShpNsFir(pMode, &GData.FinalFir);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: fnlshpns_filter_sharpen_coring
    Refresh_Set_Filter_Valid(ITUNER_FINAL_CORING);
    rval = (UINT32)AmbaIK_GetFnlShpNsCoring(pMode, &GData.FinalCoring);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: fnlshpns_filter_sharpen_coring_index_scale
    Refresh_Set_Filter_Valid(ITUNER_FINAL_CORING_INDEX_SCALE);
    rval = (UINT32)AmbaIK_GetFnlShpNsCorIdxScl(pMode, &GData.FinalCoringIndexScale);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: fnlshpns_filter_sharpen_min_coring_result
    Refresh_Set_Filter_Valid(ITUNER_FINAL_MIN_CORING_RESULT);
    rval = AmbaIK_GetFnlShpNsMinCorRst(pMode, &GData.FinalMinCoringResult);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: fnlshpns_filter_sharpen_max_coring_result
    Refresh_Set_Filter_Valid(ITUNER_FINAL_MAX_CORING_RESULT);
    rval = (UINT32)AmbaIK_GetFnlShpNsMaxCorRst(pMode, &GData.FinalMaxCoringResult);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: fnlshpns_filter_sharpen_scale_coring
    Refresh_Set_Filter_Valid(ITUNER_FINAL_SCALE_CORING);
    rval = (UINT32)AmbaIK_GetFnlShpNsSclCor(pMode, &GData.FinalScaleCoring);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: video_mctf
    Refresh_Set_Filter_Valid(ITUNER_VIDEO_MCTF);
    rval = (UINT32)AmbaIK_GetVideoMctf(pMode, &GData.VideoMctf);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    // Note: video_mctf_Level
    Refresh_Set_Filter_Valid(ITUNER_VIDEO_MCTF_LEVEL);

    // Note: ik_video_mctf_ta_t
    Refresh_Set_Filter_Valid(ITUNER_VIDEO_MCTF_TEMPORAL_ADJUST);
    rval = (UINT32)AmbaIK_GetVideoMctfTa(pMode, &GData.VideoMctfTemporalAdjust);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    Refresh_Set_Filter_Valid(ITUNER_VIDEO_MCTF_AND_FINAL_SHARPEN);
    rval = (UINT32)AmbaIK_GetVideoMctfAndFnlshp(pMode, &GData.VideoMctfAndFinalSharpen);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}

static INT32 AmbaItuner_HisoRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;
    Set_Filter_Valid Refresh_Set_Filter_Valid;

    if (AmbaItuner_Is_Video_Preview_Tuning() >0 ) {
        Refresh_Set_Filter_Valid = Dummy_Set_Filter_Valid;
    } else {
        Refresh_Set_Filter_Valid = AmbaItuner_Set_Filter_Valid;
    }

    // Note: low_chroma_asf
    Refresh_Set_Filter_Valid(ITUNER_LI_CHROMA_ASF);
    rval = (UINT32)AmbaIK_GetChromaAsf(pMode, &GData.ChromaAsf);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_chroma_asf
    Refresh_Set_Filter_Valid(ITUNER_HI_CHROMA_ASF);
    rval = (UINT32)AmbaIK_GetHiChromaAsf(pMode, &GData.HiChromaAsf);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_luma_asf
    Refresh_Set_Filter_Valid(ITUNER_HI_LUMA_ASF);
    rval= (UINT32)AmbaIK_GetHiAsf(pMode, &GData.HiAsf);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_cfa_noise_filter
    Refresh_Set_Filter_Valid(ITUNER_HI_CFA_NOISE_FILTER);
    rval= (UINT32)AmbaIK_GetHiCfaNoiseFilter(pMode, &GData.HiCfaNoiseFilter);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_cfa_leakage_filter
    Refresh_Set_Filter_Valid(ITUNER_HI_CFA_LEAKAGE_FILTER);
    rval= (UINT32)AmbaIK_GetHiCfaLeakageFilter(pMode, &GData.HiCfaLeakageFilter);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_anti_aliasing
    Refresh_Set_Filter_Valid(ITUNER_HI_ANTI_ALIASING);
    rval= (UINT32)AmbaIK_GetHiAntiAliasing(pMode, &GData.HiAntiAliasing);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_dynamic_bad_pixel_correction
    Refresh_Set_Filter_Valid(ITUNER_HI_AUTO_BAD_PIXEL_CORRECTION);
    rval= (UINT32)AmbaIK_GetHiDynamicBPC(pMode, &GData.HiDynamicBpc);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_chroma_filter_high
    Refresh_Set_Filter_Valid(ITUNER_HI_CHROMA_FLTR_HIGH);
    rval= (UINT32)AmbaIK_GetHiChromaFilterHigh(pMode, &GData.HiChromaFilterHigh);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_chroma_filter_low
    Refresh_Set_Filter_Valid(ITUNER_HI_CHROMA_FLTR_LOW);
    rval= (UINT32)AmbaIK_GetHiChromaFilterLow(pMode, &GData.HiChromaFilterLow);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_chroma_filter_low_combine
    Refresh_Set_Filter_Valid(ITUNER_HI_CHROMA_FLTR_LOW_COMBINE);
    rval= (UINT32)AmbaIK_GetHiChromaFltrLowCom(pMode, &GData.HiChromaFilterLowCombine);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_chroma_filter_median
    Refresh_Set_Filter_Valid(ITUNER_HI_CHROMA_FLTR_MEDIAN);
    rval= (UINT32)AmbaIK_GetHiChromaFilterMed(pMode, &GData.HiChromaFilterMed);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_chroma_filter_median_combine
    Refresh_Set_Filter_Valid(ITUNER_HI_CHROMA_FLTR_MEDIAN_COMBINE);
    rval= (UINT32)AmbaIK_GetHiChromaFltrMedCom(pMode, &GData.HiChromaFilterMedCombine);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_chroma_filter_pre_proc
    Refresh_Set_Filter_Valid(ITUNER_HI_CHROMA_FLTR_PRE);
    rval= (UINT32)AmbaIK_GetHiChromaFilterPre(pMode, &GData.HiChromaFilterPre);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_chroma_filter_very_low
    Refresh_Set_Filter_Valid(ITUNER_HI_CHROMA_FLTR_VERY_LOW);
    rval= (UINT32)AmbaIK_GetHiChromaFilterVeryLow(pMode, &GData.HiChromaFilterVeryLow);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_chroma_filter_very_low_combine
    Refresh_Set_Filter_Valid(ITUNER_HI_CHROMA_FLTR_VERY_LOW_COMBINE);
    rval= (UINT32)AmbaIK_GetHiChromaFltrVeryLowCom(pMode, &GData.HiChromaFilterVeryLowCombine);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_chroma_median_filter
    Refresh_Set_Filter_Valid(ITUNER_HI_CHROMA_MEDIAN_FILTER);
    rval= (UINT32)AmbaIK_GetHiChromaMedianFilter(pMode, &GData.HiChromaMedianFilter);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_demosaic_filter
    Refresh_Set_Filter_Valid(ITUNRE_HI_DEMOSAIC_FILTER);
    rval= (UINT32)AmbaIK_GetHiDemosaic(pMode, &GData.HiDemosaic);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_gb_gr_mismatch
    Refresh_Set_Filter_Valid(ITUNER_HI_GB_GR_MISMATCH);
    rval= (UINT32)AmbaIK_GetHiGrGbMismatch(pMode, &GData.HiGrGbMismatch);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_high2_luma_asf
    Refresh_Set_Filter_Valid(ITUNER_HI_HIGH2_LUMA_ASF);
    rval= (UINT32)AmbaIK_GetHiHigh2Asf(pMode, &GData.HiHigh2Asf);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_high_luma_asf
    Refresh_Set_Filter_Valid(ITUNER_HI_HIGH_LUMA_ASF);
    rval= (UINT32)AmbaIK_GetHiHighAsf(pMode, &GData.HiHighAsf);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_high_sharpen_noise_both
    Refresh_Set_Filter_Valid(ITUNER_HI_HIGH_SHARPEN_BOTH);
    rval= (UINT32)AmbaIK_GetHiHighShpNsBoth(pMode, &GData.HiHighShpnsBoth);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_high_sharpen_noise
    Refresh_Set_Filter_Valid(ITUNER_HI_HIGH_SHARPEN_NOISE);
    rval= (UINT32)AmbaIK_GetHiHighShpNsNoise(pMode, &GData.HiHighShpnsNoise);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_high_sharpen_noise_fir
    Refresh_Set_Filter_Valid(ITUNER_HI_HIGH_FIR);
    rval= (UINT32)AmbaIK_GetHiHighShpNsFir(pMode, &GData.HiHighShpnsFir);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_high_sharpen_noise_coring
    Refresh_Set_Filter_Valid(ITUNER_HI_HIGH_CORING);
    rval= (UINT32)AmbaIK_GetHiHighShpNsCoring(pMode, &GData.HiHighShpnsCoring);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_high_sharpen_noise_coring_index_scale
    Refresh_Set_Filter_Valid(ITUNER_HI_HIGH_CORING_INDEX_SCALE);
    rval= (UINT32)AmbaIK_GetHiHighShpNsCorIdxScl(pMode, &GData.HiHighShpnsCorIdxScl);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_high_sharpen_noise_filter_max_coring_result
    Refresh_Set_Filter_Valid(ITUNER_HI_HIGH_MAX_CORING_RESULT);
    rval= (UINT32)AmbaIK_GetHiHighShpNsMaxCorRst(pMode, &GData.HiHighShpnsMaxCorRst);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_high_sharpen_noise_filter_min_coring_result
    Refresh_Set_Filter_Valid(ITUNER_HI_HIGH_MIN_CORING_RESULT);
    rval= (UINT32)AmbaIK_GetHiHighShpNsMinCorRst(pMode, &GData.HiHighShpnsMinCorRst);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_high_sharpen_noise_scale_coring
    Refresh_Set_Filter_Valid(ITUNER_HI_HIGH_SCALE_CORING);
    rval= (UINT32)AmbaIK_GetHiHighShpNsSclCor(pMode, &GData.HiHighShpnsSclCor);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_low_luma_asf
    Refresh_Set_Filter_Valid(ITUNER_HI_LOW_LUMA_ASF);
    rval= (UINT32)AmbaIK_GetHiLowAsf(pMode, &GData.HiLowAsf);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_low_luma_asf_combine
    Refresh_Set_Filter_Valid(ITUNER_HI_LOW_LUMA_ASF_COMBINE);
    rval= (UINT32)AmbaIK_GetHiLowASFCombine(pMode, &GData.HiLowAsfCombine);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_luma_noise_combine
    Refresh_Set_Filter_Valid(ITUNER_HI_LUMA_NOISE_COMBINE);
    rval= (UINT32)AmbaIK_GetHiLumaCombine(pMode, &GData.HiLumaCombine);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_median1_luma_asf
    Refresh_Set_Filter_Valid(ITUNER_HI_MED1_LUMA_ASF);
    rval= (UINT32)AmbaIK_GetHiMed1Asf(pMode, &GData.HiMed1Asf);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_median2_luma_asf
    Refresh_Set_Filter_Valid(ITUNER_HI_MED2_LUMA_ASF);
    rval= (UINT32)AmbaIK_GetHiMed2Asf(pMode, &GData.HiMed2Asf);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_median_sharpen_noise_both
    Refresh_Set_Filter_Valid(ITUNER_HI_MED_SHARPEN_BOTH);
    rval= (UINT32)AmbaIK_GetHiMedShpNsBoth(pMode, &GData.HiMedShpnsBoth);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_median_sharpen_noise
    Refresh_Set_Filter_Valid(ITUNER_HI_MED_SHARPEN_NOISE);
    rval= (UINT32)AmbaIK_GetHiMedShpNsNoise(pMode, &GData.HiMedShpnsNoise);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_median_sharpen_noise_fir
    Refresh_Set_Filter_Valid(ITUNER_HI_MED_FIR);
    rval= (UINT32)AmbaIK_GetHiMedShpNsFir(pMode, &GData.HiMedShpnsFir);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_median_sharpen_noise_coring
    Refresh_Set_Filter_Valid(ITUNER_HI_MED_CORING);
    rval= (UINT32)AmbaIK_GetHiMedShpNsCoring(pMode, &GData.HiMedShpnsCoring);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_median_sharpen_noise_coring_index_scale
    Refresh_Set_Filter_Valid(ITUNER_HI_MED_CORING_INDEX_SCALE);
    rval= (UINT32)AmbaIK_GetHiMedShpNsCorIdxScl(pMode, &GData.HiMedShpnsCorIdxScl);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_median_sharpen_noise_filter_max_coring_result
    Refresh_Set_Filter_Valid(ITUNER_HI_MED_MAX_CORING_RESULT);
    rval= (UINT32)AmbaIK_GetHiMedShpNsMaxCorRst(pMode, &GData.HiMedShpnsMaxCorRst);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_median_sharpen_noise_filter_min_coring_result
    Refresh_Set_Filter_Valid(ITUNER_HI_MED_MIN_CORING_RESULT);
    rval= (UINT32)AmbaIK_GetHiMedShpNsMinCorRst(pMode, &GData.HiMedShpnsMinCorRst);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_median_sharpen_noise_scale_coring
    Refresh_Set_Filter_Valid(ITUNER_HI_MED_SCALE_CORING);
    rval= (UINT32)AmbaIK_GetHiMedShpNsSclCor(pMode, &GData.HiMedShpnsSclCor);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_select
    Refresh_Set_Filter_Valid(ITUNER_HI_SELECT);
    rval= (UINT32)AmbaIK_GetHiSelect(pMode, &GData.HiSelect);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_luma_blend
    Refresh_Set_Filter_Valid(ITUNER_HI_LUMA_BLEND);
    rval= (UINT32)AmbaIK_GetHiLumaBlend(pMode, &GData.HiLumaBlend);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hili_combine
    Refresh_Set_Filter_Valid(ITUNER_HILI_COMBINE);
    rval= (UINT32)AmbaIK_GetHiliCombine(pMode, &GData.HiLiCombine);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_median_high_frequency_recover
    Refresh_Set_Filter_Valid(ITUNER_HI_MED_HIGH_FREQ_RECOVER);
    rval= (UINT32)AmbaIK_GetHiMidHighFreqRecover(pMode, &GData.HiMidHighFreqRecover);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hili_sharpen_noise_both
    Refresh_Set_Filter_Valid(ITUNER_HILI_SHARPEN_BOTH);
    rval= (UINT32)AmbaIK_GetHiliShpNsBoth(pMode, &GData.HiLiShpnsBoth);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hili_sharpen_noise
    Refresh_Set_Filter_Valid(ITUNER_HILi_SHARPEN_NOISE);
    rval= (UINT32)AmbaIK_GetHiliShpNsNoise(pMode, &GData.HiLiShpnsNoise);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hili_sharpen_noise_fir
    Refresh_Set_Filter_Valid(ITUNER_HILI_FIR);
    rval= (UINT32)AmbaIK_GetHiliShpNsFir(pMode, &GData.HiLiShpnsFir);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hili_sharpen_noise_coring
    Refresh_Set_Filter_Valid(ITUNER_HILI_CORING);
    rval= (UINT32)AmbaIK_GetHiliShpNsCoring(pMode, &GData.HiLiShpnsCoring);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hili_sharpen_noise_coring_index_scale
    Refresh_Set_Filter_Valid(ITUNER_HILI_CORING_INDEX_SCALE);
    rval= (UINT32)AmbaIK_GetHiliShpNsCorIdxScl(pMode, &GData.HiLiShpnsCorIdxScl);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hili_sharpen_noise_filter_max_coring_result
    Refresh_Set_Filter_Valid(ITUNER_HILI_MAX_CORING_RESULT);
    rval= (UINT32)AmbaIK_GetHiliShpNsMaxCorRst(pMode, &GData.HiLiShpnsMaxCorRst);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hili_sharpen_noise_filter_min_coring_result
    Refresh_Set_Filter_Valid(ITUNER_HILI_MIN_CORING_RESULT);
    rval= (UINT32)AmbaIK_GetHiliShpNsMinCorRst(pMode, &GData.HiLiShpnsMinCorRst);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hili_sharpen_noise_scale_coring
    Refresh_Set_Filter_Valid(ITUNER_HILI_SCALE_CORING);
    rval= (UINT32)AmbaIK_GetHiliShpNsSclCor(pMode, &GData.HiLiShpnsSclCor);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li2_luma_asf
    Refresh_Set_Filter_Valid(ITUNER_LI2_LUMA_ASF);
    rval= (UINT32)AmbaIK_GetLi2Asf(pMode, &GData.Li2Asf);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li2_anti_aliasing
    Refresh_Set_Filter_Valid(ITUNER_LI2_ANTI_ALIASING);
    rval= (UINT32)AmbaIK_GetLi2AntiAliasing(pMode, &GData.Li2AntiAliasing);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li2_dynamic_bad_pixel_correction
    Refresh_Set_Filter_Valid(ITUNER_LI2_AUTO_BAD_PIXEL_CORRECTION);
    rval= (UINT32)AmbaIK_GetLi2DynamicBPC(pMode, &GData.Li2DynamicBpc);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li2_cfa_leakage_filter
    Refresh_Set_Filter_Valid(ITUNER_LI2_CFA_LEAKAGE_FILTER);
    rval= (UINT32)AmbaIK_GetLi2CfaLeakageFilter(pMode, &GData.Li2CfaLeakageFilter);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li2_cfa_noise_filter
    Refresh_Set_Filter_Valid(ITUNER_LI2_CFA_NOISE_FILTER);
    rval= (UINT32)AmbaIK_GetLi2CfaNoiseFilter(pMode, &GData.Li2CfaNoiseFilter);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li2_demosaic_filter
    Refresh_Set_Filter_Valid(ITUNRE_LI2_DEMOSAIC_FILTER);
    rval= (UINT32)AmbaIK_GetLi2Demosaic(pMode, &GData.Li2Demosaic);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li2_gb_gr_mismatch
    Refresh_Set_Filter_Valid(ITUNER_LI2_GB_GR_MISMATCH);
    rval= (UINT32)AmbaIK_GetLi2GrGbMismatch(pMode, &GData.Li2GrGbMismatch);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li2_sharpen_noise_both
    Refresh_Set_Filter_Valid(ITUNER_LI2_SHARPEN_BOTH);
    rval= (UINT32)AmbaIK_GetLi2ShpNsBoth(pMode, &GData.Li2ShpnsBoth);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li2_sharpen_noise
    Refresh_Set_Filter_Valid(ITUNER_LI2_SHARPEN_NOISE);
    rval= (UINT32)AmbaIK_GetLi2ShpNsNoise(pMode, &GData.Li2ShpnsNoise);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li2_sharpen_noise_fir
    Refresh_Set_Filter_Valid(ITUNER_LI2_FIR);
    rval= (UINT32)AmbaIK_GetLi2ShpNsFir(pMode, &GData.Li2ShpnsFir);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li2_sharpen_noise_coring
    Refresh_Set_Filter_Valid(ITUNER_LI2_CORING);
    rval= (UINT32)AmbaIK_GetLi2ShpNsCoring(pMode, &GData.Li2ShpnsCoring);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li2_sharpen_noise_coring_index_scale
    Refresh_Set_Filter_Valid(ITUNER_LI2_CORING_INDEX_SCALE);
    rval= (UINT32)AmbaIK_GetLi2ShpNsCorIdxScl(pMode, &GData.Li2ShpnsCorIdxScl);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li2_sharpen_noise_filter_max_coring_result
    Refresh_Set_Filter_Valid(ITUNER_LI2_MAX_CORING_RESULT);
    rval= (UINT32)AmbaIK_GetLi2ShpNsMaxCorRst(pMode, &GData.Li2ShpnsMaxCorRst);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li2_sharpen_noise_filter_min_coring_result
    Refresh_Set_Filter_Valid(ITUNER_LI2_MIN_CORING_RESULT);
    rval= (UINT32)AmbaIK_GetLi2ShpNsMinCorRst(pMode, &GData.Li2ShpnsMinCorRst);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: li2_sharpen_noise_scale_coring
    Refresh_Set_Filter_Valid(ITUNER_LI2_SCALE_CORING);
    rval= (UINT32)AmbaIK_GetLi2ShpNsSclCor(pMode, &GData.Li2ShpnsSclCor);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    // Note: hi_nonsmooth_detect
    Refresh_Set_Filter_Valid(ITUNER_HI_NONSMOOTH_DETECT);
    rval= (UINT32)AmbaIK_GetHiNonSmoothDetect(pMode, &GData.HiNonsmoothDetect);
    check_rval_not_equal_0(rval, __func__, __LINE__);
    ret |= rval;

    return (INT32)ret;
}

static INT32 AmbaItuner_RefreshVideoLinear(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    rval = (UINT32)AmbaItuner_SystemRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_VinSensorRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2RawRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Yuv2YuvRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_MctfRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2yuvCalibRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}

static INT32 AmbaItuner_RefreshVideoLinearCe(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    rval = (UINT32)AmbaItuner_SystemRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_VinSensorRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2RawRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Yuv2YuvRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_MctfRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2yuvCalibRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_CeRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}

static INT32 AmbaItuner_RefreshVideoHdrExpo(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    rval = (UINT32)AmbaItuner_SystemRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_VinSensorRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2RawRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Yuv2YuvRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_MctfRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2yuvCalibRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_CeRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_VideoHdrRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}

static INT32 AmbaItuner_RefreshVideoYuv2Yuv(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    rval = (UINT32)AmbaItuner_SystemRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Yuv2YuvRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_MctfRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Yuv2yuvCalibRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}

static INT32 AmbaItuner_RefreshVideoYuv2YuvMipi(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    rval = (UINT32)AmbaItuner_SystemRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_MctfRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}
// hiso
static INT32 AmbaItuner_RefreshStillLiso(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    rval = (UINT32)AmbaItuner_SystemRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_VinSensorRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2RawRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Yuv2YuvRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_CeRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_MctfRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2yuvCalibRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}

static INT32 AmbaItuner_RefreshStillHiso(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 ret = 0, rval = 0;

    rval = (UINT32)AmbaItuner_SystemRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_VinSensorRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2RawRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Yuv2YuvRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_CeRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_HisoRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    rval = (UINT32)AmbaItuner_Raw2yuvCalibRefresh(pMode);
    if ((INT32)rval != 0) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("Line:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= rval;

    return (INT32)ret;
}

INT32 AmbaItuner_Refresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    AMBA_IK_ABILITY_s Ability = {0};
    UINT32 rval;
    INT32 ret = 0;

    rval = (UINT32)AmbaIK_GetContextAbility(pMode, &Ability);
    if (rval != IK_OK) {
        ituner_print_str_5("%s() call Fail", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("call ik_get_context_ability(%d) fail", pMode->ContextId, DC_U, DC_U, DC_U, DC_U);
    }

    if (Ability.Pipe == AMBA_IK_PIPE_VIDEO) {
        SystemInfo.Ability = Ability.VideoPipe;
        if (Ability.VideoPipe == AMBA_IK_VIDEO_LINEAR) {
            ret = AmbaItuner_RefreshVideoLinear(pMode);
        } else if (Ability.VideoPipe == AMBA_IK_VIDEO_LINEAR_CE) {
            ret = AmbaItuner_RefreshVideoLinearCe(pMode);
        } else if (Ability.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_2) {
            ret = AmbaItuner_RefreshVideoHdrExpo(pMode);
        } else if (Ability.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_3) {
            ret = AmbaItuner_RefreshVideoHdrExpo(pMode);
        } else if (Ability.VideoPipe == AMBA_IK_VIDEO_Y2Y) {
            ret = AmbaItuner_RefreshVideoYuv2Yuv(pMode);
        } else if (Ability.VideoPipe == AMBA_IK_VIDEO_Y2Y_MIPI) {
            ret = AmbaItuner_RefreshVideoYuv2YuvMipi(pMode);
        } else {
            ituner_print_uint32_5("Ituner flow don't support your current ability:%d", Ability.VideoPipe, DC_U, DC_U, DC_U, DC_U);
            ret = -1;
        }
    } else if (Ability.Pipe== AMBA_IK_PIPE_STILL) {
//hiso
        SystemInfo.Ability = Ability.StillPipe;
        if (Ability.StillPipe == AMBA_IK_STILL_BASE) {
            ret = AmbaItuner_RefreshStillLiso(pMode);
        } else if (Ability.StillPipe == AMBA_IK_STILL_HISO) {
            ret = AmbaItuner_RefreshStillHiso(pMode);
        } else {
            ret = -1;
        }
    } else {
        ret = -1;
    }
    return ret;
}

UINT32 AmbaItuner_CalFpnMapSize(UINT32 width, UINT32 height)
{
    return ((width * height) >> 3U);
}

UINT32 AmbaItuner_CalInternalFpnMapSize(UINT32 width, UINT32 height)
{
    return (width * height);
}

UINT32 AmbaItuner_CalWarpTableSize(UINT32 width, UINT32 height)
{
    return (width * height * sizeof(AMBA_IK_GRID_POINT_s));
}

UINT32 AmbaItuner_CalCaTableSize(UINT32 width, UINT32 height)
{
    return (width * height * sizeof(AMBA_IK_GRID_POINT_s));
}



