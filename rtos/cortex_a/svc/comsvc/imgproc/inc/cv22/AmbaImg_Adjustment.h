/**
 *  @file AmbaImg_Adjustment.h
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

#ifndef AMBA_IMG_ADJUSTMENT_CV28_H
#define AMBA_IMG_ADJUSTMENT_CV28_H

#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageHisoFilter.h"
#include "AmbaImg_Adjustment_Def.h"
#include "AmbaImg_AeAwb.h"

#define STILL_PARAM_LUT_NO    2

#define VIDEO_TV_TABLE_NO     4
#define VIDEO_PC_TABLE_NO     4
#define PHOTO_TABLE_NO        4

#define MAX_AEB_NUM           9U
#define AEB_MAX_NUM           9U
//for Amba Print Function
#define PrintBufSize    40U
#define DecimalFormat   10U
#define HexFormat       16U
#define AfterPowint     6U
#define AfterPowint8    8U


/* AEB control parameters */
typedef struct {
    UINT8    Num;             // 1~MAX_AEB_NUM
    INT8     EvBias[MAX_AEB_NUM];    // unit ev is 32, +4 31/32 EV ~ -4 31/32 EV
} AEB_INFO_s;

typedef struct {
    UINT8     Enable;
    ADJ_LUT_24_s LowTab[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_24_s HighTab[ADJ_NF_TABLE_COUNT];
} ADJ_LUT_AGC_WB_s;

typedef struct {
    UINT8            BadPixelEnable;
    ADJ_LUT_8_s      BadPixel[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_16_s     AliasingGbGrResampleEnable;
    ADJ_LUT_16_s     AliasingGbGrResample[ADJ_NF_TABLE_COUNT];
    UINT8            ChromaMedianEnable;
    ADJ_LUT_8_s      LowChromaMedian[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_8_s      HighChromaMedian[ADJ_NF_TABLE_COUNT];
    UINT8            DemosaicEnable;
    ADJ_LUT_8_s      Demosaic[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_AGC_WB_s CfaFilter;
    UINT8            RgbIrEnable;
    ADJ_LUT_16_s     LowRgbIr[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_16_s     HighRgbIr[ADJ_NF_TABLE_COUNT];
}ADJ_BASIC_s;

typedef struct {
    INT16        Value[64];
} COEFFS64_s;

typedef struct {
    INT16 Coefs[9*25];
}FIR_COEFS_s;

typedef struct{
    UINT8 SmoothSelect[256];
}SMOOTH_SELECT_s;

typedef struct {
    ADJ_LUT_24_s   FirDirAmtStr[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_24_s   FirIsoStr[ADJ_NF_TABLE_COUNT];
    FIR_COEFS_s    FirCoefs[ADJ_NF_TABLE_COUNT];
} DEF_FIR_s;

typedef struct {
    UINT32   Coring[IK_NUM_CORING_TABLE_INDEX];
} ADJ_CORING_s;

typedef struct {
    ADJ_LUT_8_s           FinShpNfBoth[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_16_s          FinShpNfNLevelStrAdjust[ADJ_NF_TABLE_COUNT];
    DEF_FIR_s             FinShpNfN;

    DEF_FIR_s             FinShpNfS;
    ADJ_CORING_s          FinShpNfSCoring[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_16_s          FinShpNfSCoringIdxScale[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_8_s           FinShpNfSMinCoringIdxResult[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_8_s           FinShpNfSScaleCoring[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_8_s           FinShpNfSMaxCoringResult[ADJ_NF_TABLE_COUNT];
} DEF_SHARP_s;

typedef struct {
    ADJ_LUT_8_s           FinShpNfBoth[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_16_s          FnlShpBothLevel[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_24_s          FnlShpBothNonSmoothDetSub[ADJ_NF_TABLE_COUNT];

    ADJ_LUT_24_s          FinShpNfNLevelStrAdjust[ADJ_NF_TABLE_COUNT];
    DEF_FIR_s             FinShpNfN;
    ADJ_LUT_8_s           FinShpNfNAdvIso[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s             FinShpNfNAdvIsoMCTable[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s             FinShpNfNAdvIsoNLTable[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s             FinShpNfNAdvIsoStrATable[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s             FinShpNfNAdvIsoStrBTable[ADJ_NF_TABLE_COUNT];

    DEF_FIR_s             FinShpNfS;
    ADJ_CORING_s          FinShpNfSCoring[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_16_s          FinShpNfSCoringIdxScale[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_8_s           FinShpNfSMinCoringIdxResult[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_8_s           FinShpNfSScaleCoring[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_8_s           FinShpNfSMaxCoringResult[ADJ_NF_TABLE_COUNT];
} DEF_FNL_SHARP_s;

typedef struct {
    UINT8         Enable;
    ADJ_LUT_16_s  SpatialFilter[ADJ_HISO_NF_TABLE_COUNT];
    ADJ_LUT_8_s   SpatialT0T1Div[ADJ_HISO_NF_TABLE_COUNT];
    ADJ_LUT_8_s   SpatialLevelStrAdjust[ADJ_HISO_NF_TABLE_COUNT];
    DEF_FIR_s     SpatialFir;
} DEF_ASF_INFO_s;

typedef struct{
    ADJ_LUT_s LiWideChromaCombineLowTab[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s LiWideChromaCombineHighTab[ADJ_NF_TABLE_COUNT];
} LI_WIDE_CHROMA_FILTER_COMBINE_s;

typedef struct {
    UINT8       Enable;
    ADJ_LUT_s   ChromaFltComb[ADJ_HISO_NF_TABLE_COUNT];
    ADJ_LUT_s   Tab0[ADJ_HISO_NF_TABLE_COUNT];
    ADJ_LUT_s   Tab1[ADJ_HISO_NF_TABLE_COUNT];
} CHROMA_FILTER_COMBINE_s;

typedef struct {
    UINT8       Enable;
    ADJ_LUT_s   HiSelect[ADJ_HISO_NF_TABLE_COUNT];
} ADJ_HI_SELECT_s;

typedef struct {
    UINT8       Enable;
    ADJ_LUT_s   LumaComb[ADJ_HISO_NF_TABLE_COUNT];
} LUMA_COMBINE_s;

typedef struct {
    UINT8       Enable;
    ADJ_LUT_s   LumaBlend[ADJ_HISO_NF_TABLE_COUNT];
} ADJ_LUMA_BLEND_s;

typedef struct{
    INT32     Value[49];
} NON_SMOOTH_LEVEL_LUT_s;

typedef struct{
    INT32 Rgain;
    INT32 Bgain;
} WB_INFO_s;

typedef struct {
    WB_INFO_s                WbBoundary[3];
    NON_SMOOTH_LEVEL_LUT_s   NonSmooth[ADJ_HISO_NF_TABLE_COUNT];
} ADJ_NON_SMOOTH_s;

typedef struct{
    UINT8 Enable;
    ADJ_NON_SMOOTH_s LowTmp;
    ADJ_NON_SMOOTH_s HighTmp;
    ADJ_NON_SMOOTH_s UnitCC;
} ADJ_GROUP_NON_SMOOTH_s;

typedef struct{
    WB_INFO_s WB;
    INT32 Smooth;
    INT32 NonSmoothLevel[16];
} WB_NONSMOOTHLEVEL_s;


typedef struct {
    UINT8           ShpEnable;
    ADJ_LUT_16_s    SpatialFilter[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_8_s     SpatialT0T1Div[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_8_s     SpatialLevelStrAdjust[ADJ_NF_TABLE_COUNT];
    DEF_FIR_s       DefSharpAFir;
    DEF_SHARP_s     DefSharpA;
    DEF_FNL_SHARP_s DefSharpB;
} DEF_SHARP_INFO_s;

typedef struct {
    UINT32            VersionNumber;
    UINT32            ParamVersionNum;
    UINT8             Enable;
    ADJ_LUT_s         LowMctfA[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         HighMctfA[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         LowMctfB[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         HighMctfB[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         LowMctfC[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         HighMctfC[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         LowMctfD[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         HighMctfD[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_8_s       LowMctfE[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_8_s       HighMctfE[ADJ_NF_TABLE_COUNT];
    COEFFS64_s        LoYSpatialWeighting[ADJ_NF_TABLE_COUNT];
    COEFFS64_s        HiYSpatialWeighting[ADJ_NF_TABLE_COUNT];
    COEFFS64_s        LoCbSpatialWeighting[ADJ_NF_TABLE_COUNT];
    COEFFS64_s        HiCbSpatialWeighting[ADJ_NF_TABLE_COUNT];
    COEFFS64_s        LoCrSpatialWeighting[ADJ_NF_TABLE_COUNT];
    COEFFS64_s        HiCrSpatialWeighting[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_24_s      LowMctfF[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_24_s      HighMctfF[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_16_s      LowMctfG[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_16_s      HighMctfG[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_16_s      LowMctfH[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_16_s      HighMctfH[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_24_s      LowMctfI[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_24_s      HighMctfI[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_24_s      LowMctfJ[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_24_s      HighMctfJ[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_16_s      LowMctfK[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_16_s      HighMctfK[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         LowMctfL[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         HighMctfL[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         LowMctfM[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         HighMctfM[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         LowMctfN[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         HighMctfN[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_24_s      LowMctfO[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_24_s      HighMctfO[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_24_s      LowMctfP[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_24_s      HighMctfP[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         LowMctfQ[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         HighMctfQ[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_24_s      LowMctfR[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_24_s      HighMctfR[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         LowMctfS[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s         HighMctfS[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_16_s       LowMctfT[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_16_s       HighMctfT[ADJ_NF_TABLE_COUNT];

}ADJ_MCTF_INFO_s;
#define ADJ_MCTF_VER (0x18031500U)

typedef struct {
    UINT32            VersionNumber;
    UINT32            ParamVersionNum;
    BlackLevel_INFO_s BlackLevelInfo;
} VIDEO_HDR_SHORT_PARAM_s;
#define ADJ_VHDR_SHORT_PARAM_VER (0x17122600U)


typedef struct{
    UINT32            VersionNumber;
    UINT32            ParamVersionNum;
    ADJ_FILTER_INFO_s EvImg;
    UINT8             NfMaxTableCount;
    ADJ_DEF_s         Def;
    ADJ_BASIC_s       Basic;
    ADJ_MCTF_INFO_s   MctfInfo;
    ADJ_LUT_AGC_WB_s  ChromaFilter;
    UINT8             LiProcessingSeletEnable;
    ADJ_LUT_4_s       LiProcessingSelect[ADJ_NF_TABLE_COUNT];
    DEF_SHARP_INFO_s  SharpInfo;
} VIDEO_FILTER_PARAM_s;
#define ADJ_VF_PARAM_VER (0x20091600U)

/****************************************************************/
/*****        Video Adjustment parameters Structure         *****/
/****************************************************************/
typedef struct {
    ADJ_AWB_AE_s            AwbAe;                       //ADJ DEF
    VIDEO_FILTER_PARAM_s    FilterParam;
    VIDEO_HDR_SHORT_PARAM_s HdrShortParam;
} ADJ_VIDEO_PARAM_s;

/****************************************************************/
/*****        Video Motion Speed parameters Structure         *****/
/****************************************************************/
typedef struct {
    UINT32                 VersionNumber;
    UINT32                 ParamVersionNum;
    ADJ_MCTF_INFO_s        MctfInfo;
    DEF_SHARP_INFO_s       SharpInfo;
} VIDEO_MOTION_PARAM_s;
#define ADJ_MOTION_PARAM_VER (0x16120600U)

/****************************************************************/
/*****        STILL Adjustment parameters Structure         *****/
/****************************************************************/
typedef struct {

    UINT32               VersionNum;
    UINT32               ParamVersionNum;
    ////////////////////////////////////////////
    ADJ_FILTER_INFO_s    EvImg;
    UINT8                NfMaxTableCount;
    ADJ_DEF_s            Def;
    // ////////////////////////////////////////////
    ADJ_BASIC_s          Basic;
    ADJ_LUT_AGC_WB_s     ChromaFilter;
    UINT8                LiProcessingSeletEnable;
    ADJ_LUT_4_s          LiProcessingSelect[ADJ_NF_TABLE_COUNT];
    DEF_SHARP_INFO_s     SharpInfo;
    BlackLevel_INFO_s    HdrShortBlkInfo;    //for short hdr black level
} STILL_LISO_FILTER_PARAM_s;
#define ADJ_STILL_LISO_PARAM_VER (0x20091600U)

typedef struct {

    UINT32                 VersionNum;
    UINT32                 ParamVersionNum;
    ////////////////////////////////////////////
    UINT8                  NfMaxTableCount;
    ADJ_FILTER_INFO_s      NormalEvImg;
    ADJ_DEF_s              Def;
    ////////////////////////////////////////////
    ADJ_BASIC_s            Basic;

    ADJ_LUT_AGC_WB_s       ChromaFilter;
    //DEF_SHARP_INFO_s     SharpInfo;
    //ADJ_STR_CONTROL_s    DzoomControl;
    //////////////////////////////////////////// Start of HISO
    ADJ_HISO_FILTER_INFO_s HIsoNormalEvImg;
    ADJ_BASIC_s            HIsoBasic;
    ADJ_BASIC_s            HIsoLi2Basic;

    ADJ_HI_SELECT_s        HIsoSelect;

    DEF_ASF_INFO_s         HIsoAsf;
    DEF_ASF_INFO_s         HIsoHighAsf;
    DEF_ASF_INFO_s         HIsoHigh2Asf;
    DEF_ASF_INFO_s         HIsoMedAsf;
    DEF_ASF_INFO_s         HIsoMed2Asf;
    DEF_ASF_INFO_s         HIsoLowAsf;
    DEF_ASF_INFO_s         HIsoLiAsf;
    DEF_ASF_INFO_s         HIsoLi2Asf;

    DEF_SHARP_s            HIsoHighSharp;
    DEF_SHARP_s            HIsoMedSharp;
    DEF_SHARP_s            HIsoLiSharp;
    DEF_SHARP_s            HIsoLi2Sharp;

    DEF_ASF_INFO_s         HIsoLiChromaAsf;
    DEF_ASF_INFO_s         HIsoChromaAsf;

    ADJ_LUT_AGC_WB_s       HIsoChromaFilterPre;
    ADJ_LUT_AGC_WB_s       HIsoChromaFilterHigh;
    ADJ_LUT_AGC_WB_s       HIsoChromaFilterMed;
    ADJ_LUT_AGC_WB_s       HIsoChromaFilterLow;
    ADJ_LUT_AGC_WB_s       HIsoChromaFilterVLow;

    CHROMA_FILTER_COMBINE_s HIsoChromaFilterMedCombine;
    CHROMA_FILTER_COMBINE_s HIsoChromaFilterLowCombine;
    CHROMA_FILTER_COMBINE_s HIsoChromaFilterVLowCombine;

    CHROMA_FILTER_COMBINE_s HIsoHiLiCombine;

    LUMA_COMBINE_s          HIsoLumaFilterCombine;
    LUMA_COMBINE_s          HIsoLowAsfCombine;

    UINT8                   HIsoLiLumaMidHightFreqRcvrEnable;
    DEF_FIR_s               HIsoLiLumaMidHightFreqRcvr;

    ADJ_LUMA_BLEND_s        HIsoLumaBlend;
    ADJ_GROUP_NON_SMOOTH_s  HIsoNonSmooth;
    BlackLevel_INFO_s       HdrShortBlkInfo;    //for short hdr black level
} STILL_HISO_FILTER_PARAM_s;
#define ADJ_STILL_HIGH_ISO_PARAM_VER (0x20091600U)

typedef STILL_HISO_FILTER_PARAM_s ADJ_STILL_HISO_PARAM_s;


typedef struct{
    UINT8                    Mode; //IP_MODE_PREVIEW, IP_MODE_CAPTURE
    AMBA_AE_INFO_s           AE;
    AMBA_IK_WB_GAIN_s        WB;
    ADJ_AWB_AE_s             *pAwbAe;           //Must
    VIDEO_FILTER_PARAM_s     *pFilterParam;     //Must
    VIDEO_HDR_SHORT_PARAM_s  *pHdrShortParam;   //HDR
    VIDEO_MOTION_PARAM_s     *pVideoMSMParam;   //Motion Speed  //structure should rename to VIDEO_MOTION_PARAM_s
    VIDEO_MOTION_PARAM_s     *pVideoMSHParam;   //Motion Speed
} ADJ_VIDEO_IQ_INFO_s;


typedef struct{
    UINT8                     Mode; //0:IP_STILL_LISO_MODE, 1:IP_STILL_HISO_MODE
    AMBA_AE_INFO_s            AE;
    AMBA_IK_WB_GAIN_s         WB;
    ADJ_AWB_AE_s              *pAwbAe;
    STILL_LISO_FILTER_PARAM_s *pStillLISOParam;
    STILL_HISO_FILTER_PARAM_s *pStillHISOParam;
    UINT32                    AEBIndex;

} ADJ_STILL_IQ_INFO_s;


typedef struct {
    UINT8                          Type;// 0:li, 1:hili/hi_med/hi_high
    UINT8                          SharpenBothUpdate;
    AMBA_IK_FSTSHPNS_BOTH_s        SharpenBoth;
    UINT8                          SharpenNoiseUpdate;
    AMBA_IK_FSTSHPNS_NOISE_s       SharpenNoise;
    UINT8                          SharpenFirUpdate;
    AMBA_IK_FSTSHPNS_FIR_s         SharpenFir;
    UINT8                          SharpenCoringUpdate;
    AMBA_IK_CORING_s               SharpenCoring;
    UINT8                          SharpenCoringIndexScaleUpdate;
    AMBA_IK_LEVEL_s                SharpenCoringIndexScale;
    UINT8                          SharpenMinCoringResultUpdate;
    AMBA_IK_LEVEL_s                SharpenMinCoringResult;
    UINT8                          SharpenScaleCoringUpdate;
    AMBA_IK_LEVEL_s                SharpenScaleCoring;
    UINT8                          SharpenMaxCoringResultUpdate;
    AMBA_IK_LEVEL_s                SharpenMaxCoringResult;


   UINT8                           SharpenBBothUpdate;
   AMBA_IK_HI_SHPNS_BOTH_B_s       SharpenBBoth;
   UINT8                           SharpenBCoringUpdate;
   AMBA_IK_HI_CORING_B_s           SharpenBCoring;
   UINT8                           SharpenNoiseHIsoUpdate;
   AMBA_IK_HI_HIGH_SHPNS_NOISE_s   SharpenNoiseHIso;
    UINT8                          SharpenCoringIndexScaleMethodUpdate;
    AMBA_IK_LEVEL_METHOD_s         SharpenCoringIndexScaleMethod;
    UINT8                          SharpenMinCoringResultMethodUpdate;
    AMBA_IK_LEVEL_METHOD_s         SharpenMinCoringResultMethod;
    UINT8                          SharpenScaleCoringMethodUpdate;
    AMBA_IK_LEVEL_METHOD_s         SharpenScaleCoringMethod;
    UINT8                          SharpenMaxCoringResultMethodUpdate;
    AMBA_IK_LEVEL_METHOD_s         SharpenMaxCoringResultMethod;

} ADJ_SHARP_FILTER_s;

typedef struct {
    UINT8                          SharpenBBothUpdate;
    AMBA_IK_FNLSHPNS_BOTH_s        SharpenBBoth;
    UINT8                          SharpenBNoiseUpdate;
    AMBA_IK_FNLSHPNS_NOISE_s       SharpenBNoise;
    UINT8                          SharpenBFirUpdate;
    //AMBA_IK_SHPNS_FIR_s                SharpenBFir;
    AMBA_IK_FNLSHPNS_FIR_s         SharpenBFir;//  CV22_NEW should use this
    UINT8                          SharpenBCoringUpdate;
    AMBA_IK_CORING_s               SharpenBCoring;
    UINT8                          SharpenBCoringIndexScaleUpdate;
    AMBA_IK_LEVEL_METHOD_s         SharpenBCoringIndexScale;
    UINT8                          SharpenBMinCoringResultUpdate;
    AMBA_IK_LEVEL_METHOD_s         SharpenBMinCoringResult;
    UINT8                          SharpenBScaleCoringUpdate;
    AMBA_IK_LEVEL_METHOD_s         SharpenBScaleCoring;
    UINT8                          SharpenBMaxCoringResultUpdate;
    AMBA_IK_LEVEL_METHOD_s         SharpenBMaxCoringResult;

} ADJ_Final_SHARP_FILTER_s;

typedef struct {
    UINT32 Enable;
    AMBA_IK_ASF_FIR_s Fir;//510
    UINT32 DirectionalDecideT0;
    UINT32 DirectionalDecideT1;
    AMBA_IK_FULL_ADAPTATION_s Adapt;//8
    AMBA_IK_LEVEL_s LevelStrAdjust;//7
    AMBA_IK_LEVEL_s T0T1Div;//7
    AMBA_IK_LEVEL_METHOD_s LevelStrAdjustMethod;//8
    AMBA_IK_LEVEL_METHOD_s t0T1DivMethod;//8
    UINT32 MaxChangeNotT0T1Alpha;
    UINT32 MaxChangeUp;
    UINT32 MaxChangeDown;
} ADJ_ASF_FILTRE_s;


typedef struct {
    AMBA_AE_INFO_s       AeInfo[4];    // 0:long 1:short 2:very short 3:very very short
    AMBA_IK_WB_GAIN_s    WbGain[4];    // 0:long 1:short 2:very short 3:very very short
} HDR_INFO_s;

typedef struct {
    DOUBLE                   Value;
} GAMMA_INFO_s;
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/**
 * Image schdlr exposure info definitions
 */
#define AMBA_IP_VIN_COMPAND_ELEMENT_NUM    ((UINT32)45)
#define AMBA_IP_VIN_DECOMPAND_ELEMENT_NUM  ((UINT32)145)

/**
 * Video vin tone curve structure
 */
typedef struct {
    UINT32 DecompandTable[AMBA_IP_VIN_DECOMPAND_ELEMENT_NUM];
    UINT16 CompandTable[AMBA_IP_VIN_COMPAND_ELEMENT_NUM];
} AMBA_IP_VIN_TONE_CURVE_s;


/**
 * pipeline control for AAA algorithms
 */
 typedef struct {
    UINT8                            ModeUpdate;
    UINT8                            WbGainUpdate;
    AMBA_IK_WB_GAIN_s                WbGain;

    //
    //UINT8                          VsyncExposureUpdate;
    //UINT8                          ExposureUpdate;
    //AMBA_AE_INFO_s                 ExposureInfo;
    STILL_AF_INFO_s                  AfInfoStill;    /* for showing SAF focused areas on GUI after FLOCK */

    UINT8                            RgbYuvMatrixUpdate;
    AMBA_IK_RGB_TO_YUV_MATRIX_s      RgbYuvMatrix;

    /* CFA domain filters */
    UINT8                            BlackCorrUpdate;                   //vp_01
    AMBA_IK_STATIC_BLC_LVL_s         BlackCorr;
    UINT8                            AntiAliasingUpdate;                //vp_02
    AMBA_IK_ANTI_ALIASING_s          AntiAliasing;
    UINT8                            BadpixCorrUpdate;                  //vp_03
    AMBA_IK_DYNAMIC_BAD_PXL_COR_s    BadpixCorr;

    UINT8                            CfaFilterUpdate;                   //vp_04
    AMBA_IK_CFA_NOISE_FILTER_s       CfaFilter;

    UINT8                            RgbIrUpdate;
    AMBA_IK_RGB_IR_s                 RgbIr;

    /* RGB domain filters */
    UINT8                            DemosaicUpdate;                    //vp_06
    AMBA_IK_DEMOSAIC_s               Demosaic;

    UINT8                            ColorCorrUpdate;                   //vp_07
    AMBA_IK_COLOR_CORRECTION_s       ColorCorr;

    UINT8                            GammaUpdate;                       //vp_08
    AMBA_IK_TONE_CURVE_s             GammaTable;

    //UINT8                          SpecificCcUpdate;
    //AMBA_DSP_IMG_SPECIFIG_CC_s     SpecificCc;

    /* Y domain filters */
    UINT8                            ChromaScaleUpdate;                 //vp_09
    AMBA_IK_CHROMA_SCALE_s           ChromaScale;
    UINT8                            ChromaMedianUpdate;                //vp_10
    AMBA_IK_CHROMA_MEDIAN_FILTER_s   ChromaMedian;

    UINT8                            LisoProcessSelectUpdate;
    AMBA_IK_FIRST_LUMA_PROC_MODE_s   LisoProcessSelect;

    UINT8                            AsfUpdate;                         //vp_12
    AMBA_IK_ADV_SPAT_FLTR_s          Asf;

    UINT8                            SharpenBothUpdate;                 //vp_13
    AMBA_IK_FSTSHPNS_BOTH_s          SharpenBoth;
    UINT8                            SharpenNoiseUpdate;                //vp_14
    AMBA_IK_FSTSHPNS_NOISE_s         SharpenNoise;
    UINT8                            SharpenFirUpdate;                  //vp_15
    AMBA_IK_FSTSHPNS_FIR_s           SharpenFir;
    UINT8                            SharpenCoringUpdate;               //vp_16
    AMBA_IK_CORING_s                 SharpenCoring;
    UINT8                            SharpenCoringIndexScaleUpdate;     //vp_17
    AMBA_IK_LEVEL_s                  SharpenCoringIndexScale;
    UINT8                            SharpenMinCoringResultUpdate;      //vp_18
    AMBA_IK_LEVEL_s                  SharpenMinCoringResult;
    UINT8                            SharpenScaleCoringUpdate;          //vp_19
    AMBA_IK_LEVEL_s                  SharpenScaleCoring;
    UINT8                            SharpenMaxCoringResultUpdate;      //vp_18
    AMBA_IK_LEVEL_s                  SharpenMaxCoringResult;

    UINT8                            SharpenBBothUpdate;
    AMBA_IK_FNLSHPNS_BOTH_s          SharpenBBoth;
    UINT8                            SharpenBNoiseUpdate;
    AMBA_IK_FNLSHPNS_NOISE_s         SharpenBNoise;
    UINT8                            SharpenBFirUpdate;
    AMBA_IK_FNLSHPNS_FIR_s           SharpenBFir;
    UINT8                            SharpenBCoringUpdate;
    AMBA_IK_CORING_s                 SharpenBCoring;
    UINT8                            SharpenBCoringIndexScaleUpdate;
    AMBA_IK_LEVEL_METHOD_s           SharpenBCoringIndexScale;
    UINT8                            SharpenBMinCoringResultUpdate;
    AMBA_IK_LEVEL_METHOD_s           SharpenBMinCoringResult;
    UINT8                            SharpenBScaleCoringUpdate;
    AMBA_IK_LEVEL_METHOD_s           SharpenBScaleCoring;
    UINT8                            SharpenBMaxCoringResultUpdate;
    AMBA_IK_LEVEL_METHOD_s           SharpenBMaxCoringResult;

    UINT8                            ChromaFilterUpdate;                //vp_27
    AMBA_IK_CHROMA_FILTER_s          ChromaFilter;
    UINT8                            GbGrMismatchUpdate;                //vp_28
    AMBA_IK_GRGB_MISMATCH_s          GbGrMismatch;

    UINT8                            ResamplerStrUpdate;
    AMBA_IK_RESAMPLER_STR_s          ResamplerStr;

    /* Warp and MCTF related filters */
    UINT8                            MctfInfoUpdate;                    //vp_29
    AMBA_IK_VIDEO_MCTF_s             MctfInfo;
    AMBA_IK_VIDEO_MCTF_TA_s          MctfTemporalAdjust;


    UINT8                            CEInfoUpdate;
    AMBA_IK_CE_s                     CEInfo;
    UINT8                            CEInputTableUpdate;
    AMBA_IK_CE_INPUT_TABLE_s         CEInputTable;
    UINT8                            CEOutputTableUpdate;
    AMBA_IK_CE_OUT_TABLE_s           CEOutputTable;
    UINT8                            FEToneCurveUpdate;
    AMBA_IK_FE_TONE_CURVE_s          FEToneCurve;
    UINT8                            HDSToneCurveUpdate;
    AMBA_IP_VIN_TONE_CURVE_s         HDSToneCurve;

/* Start of Video HDR filter */
//adj control
//hdr control, set by scheduler
    UINT8                            HdrBlendUpdate;
    AMBA_IK_HDR_BLEND_s              HdrBlend;

    UINT8                            HdrBlackCorr0Update;
    AMBA_IK_STATIC_BLC_LVL_s         HdrBlackCorr0;
    UINT8                            HdrBlackCorr1Update;
    AMBA_IK_STATIC_BLC_LVL_s         HdrBlackCorr1;
    UINT8                            HdrBlackCorr2Update;
    AMBA_IK_STATIC_BLC_LVL_s         HdrBlackCorr2;

    UINT8                            HdrWbGain0Update;
    AMBA_IK_FE_WB_GAIN_s             HdrWbGain0;
    UINT8                            HdrWbGain1Update;
    AMBA_IK_FE_WB_GAIN_s             HdrWbGain1;
    UINT8                            HdrWbGain2Update;
    AMBA_IK_FE_WB_GAIN_s             HdrWbGain2;

} PIPELINE_CONTROL_s;

typedef struct {

    UINT8                            StillMode;
    UINT8                            ModeUpdate;

    UINT8                            WbGainUpdate;
    AMBA_IK_WB_GAIN_s                WbGain;

    UINT8                            VsyncExposureUpdate;
    UINT8                            ExposureUpdate;
    AMBA_AE_INFO_s                   ExposureInfo;
    STILL_AF_INFO_s                  AfInfoStill;    /* for showing SAF focused areas on GUI after FLOCK */

    UINT8                            RgbYuvMatrixUpdate;
    AMBA_IK_RGB_TO_YUV_MATRIX_s      RgbYuvMatrix;

    /* CFA domain filters */
    UINT8                            BlackCorrUpdate;
    AMBA_IK_STATIC_BLC_LVL_s         BlackCorr;
    UINT8                            AntiAliasingUpdate;
    AMBA_IK_ANTI_ALIASING_s          AntiAliasing;
    UINT8                            BadpixCorrUpdate;
    AMBA_IK_DYNAMIC_BAD_PXL_COR_s    BadpixCorr;

    UINT8                            CfaFilterUpdate;
    AMBA_IK_CFA_NOISE_FILTER_s       CfaFilter;

    /* RGB domain filters */
    UINT8                            DemosaicUpdate;
    AMBA_IK_DEMOSAIC_s               Demosaic;

    //UINT8                          ColorCorrRegUpdate;
    //AMBA_IK_COLOR_CORRECTION_REG_s ColorCorrReg;
    UINT8                            ColorCorrUpdate;
    AMBA_IK_COLOR_CORRECTION_s       ColorCorr;

    UINT8                            GammaUpdate;
    AMBA_IK_TONE_CURVE_s             GammaTable;

    //UINT8                          SpecificCcUpdate;
    //AMBA_DSP_IMG_SPECIFIG_CC_s     SpecificCc;

    /* Y domain filters */
    UINT8                            ChromaScaleUpdate;
    AMBA_IK_CHROMA_SCALE_s           ChromaScale;
    UINT8                            ChromaMedianUpdate;
    AMBA_IK_CHROMA_MEDIAN_FILTER_s   ChromaMedian;

    UINT8                            LisoProcessSelectUpdate;
    AMBA_IK_FIRST_LUMA_PROC_MODE_s   LisoProcessSelect;

    //UINT8                          ShpASelectUpdate;
    //AMBA_DSP_IMG_SHP_A_SELECT_e    ShpASelect;

    UINT8                            AsfUpdate;
    AMBA_IK_ADV_SPAT_FLTR_s          Asf;

    UINT8                            SharpenBothUpdate;
    AMBA_IK_FSTSHPNS_BOTH_s          SharpenBoth;
    UINT8                            SharpenNoiseUpdate;
    AMBA_IK_FSTSHPNS_NOISE_s         SharpenNoise;
    UINT8                            SharpenFirUpdate;
    AMBA_IK_FSTSHPNS_FIR_s           SharpenFir;
    UINT8                            SharpenCoringUpdate;
    AMBA_IK_CORING_s                 SharpenCoring;
    UINT8                            SharpenCoringIndexScaleUpdate;
    AMBA_IK_LEVEL_s                  SharpenCoringIndexScale;
    UINT8                            SharpenMinCoringResultUpdate;
    AMBA_IK_LEVEL_s                  SharpenMinCoringResult;
    UINT8                            SharpenScaleCoringUpdate;
    AMBA_IK_LEVEL_s                  SharpenScaleCoring;
    UINT8                            SharpenMaxCoringResultUpdate;
    AMBA_IK_LEVEL_s                  SharpenMaxCoringResult;

    UINT8                            SharpenBBothUpdate;
    AMBA_IK_FNLSHPNS_BOTH_s          SharpenBBoth;
    UINT8                            SharpenBNoiseUpdate;
    AMBA_IK_FNLSHPNS_NOISE_s         SharpenBNoise;
    UINT8                            SharpenBFirUpdate;
    AMBA_IK_FNLSHPNS_FIR_s           SharpenBFir;
    UINT8                            SharpenBCoringUpdate;
    AMBA_IK_CORING_s                 SharpenBCoring;
    UINT8                            SharpenBCoringIndexScaleUpdate;
    AMBA_IK_LEVEL_METHOD_s           SharpenBCoringIndexScale;
    UINT8                            SharpenBMinCoringResultUpdate;
    AMBA_IK_LEVEL_METHOD_s           SharpenBMinCoringResult;
    UINT8                            SharpenBScaleCoringUpdate;
    AMBA_IK_LEVEL_METHOD_s           SharpenBScaleCoring;
    UINT8                            SharpenBMaxCoringResultUpdate;
    AMBA_IK_LEVEL_METHOD_s           SharpenBMaxCoringResult;

    //UINT8                          SharpenBLinStrengthUpdate;
    //UINT16                         SharpenBLinStrength;
    UINT8                            ChromaFilterUpdate;
    AMBA_IK_CHROMA_FILTER_s          ChromaFilter;
    UINT8                            GbGrMismatchUpdate;
    AMBA_IK_GRGB_MISMATCH_s          GbGrMismatch;

    UINT8                            ResamplerStrUpdate;
    AMBA_IK_RESAMPLER_STR_s          ResamplerStr;

    UINT8                            CEInfoUpdate;
    AMBA_IK_CE_s                     CEInfo;
    UINT8                            CEInputTableUpdate;
    AMBA_IK_CE_INPUT_TABLE_s         CEInputTable;
    UINT8                            CEOutputTableUpdate;
    AMBA_IK_CE_OUT_TABLE_s           CEOutputTable;
    UINT8                            FEToneCurveUpdate;
    AMBA_IK_FE_TONE_CURVE_s          FEToneCurve;
    UINT8                            HDSToneCurveUpdate;
    AMBA_IP_VIN_TONE_CURVE_s         HDSToneCurve;

    /* Warp and MCTF related filters */
    UINT8                            MctfInfoUpdate;
    AMBA_IK_VIDEO_MCTF_s             MctfInfo;
    AMBA_IK_VIDEO_MCTF_TA_s          MctfTemporalAdjust;


    /* Start of HISO filter */
    UINT8                             HIsoCfaLeakageFilterUpdate;
    AMBA_IK_CFA_LEAKAGE_FILTER_s      HIsoCfaLeakageFilter;

    UINT8                             HIsoAntiAliasingUpdate;
    AMBA_IK_ANTI_ALIASING_s           HIsoAntiAliasing;
    UINT8                             HIsoBadpixCorrUpdate;
    AMBA_IK_DYNAMIC_BAD_PXL_COR_s     HIsoBadpixCorr;
    UINT8                             HIsoCfaFilterUpdate;
    AMBA_IK_CFA_NOISE_FILTER_s        HIsoCfaFilter;
    UINT8                             HIsoGbGrMismatchUpdate;
    AMBA_IK_GRGB_MISMATCH_s           HIsoGbGrMismatch;
    UINT8                             HIsoDemosaicUpdate;
    AMBA_IK_DEMOSAIC_s                HIsoDemosaic;
    UINT8                             HIsoChromaMedianUpdate;
    AMBA_IK_CHROMA_MEDIAN_FILTER_s    HIsoChromaMedian;

    UINT8                             HIsoLi2CfaLeakageFilterUpdate;
    AMBA_IK_CFA_LEAKAGE_FILTER_s      HIsoLi2CfaLeakageFilter;
    UINT8                             HIsoLi2AntiAliasingUpdate;
    AMBA_IK_ANTI_ALIASING_s           HIsoLi2AntiAliasing;
    UINT8                             HIsoLi2BadpixCorrUpdate;
    AMBA_IK_DYNAMIC_BAD_PXL_COR_s     HIsoLi2BadpixCorr;
    UINT8                             HIsoLi2CfaFilterUpdate;
    AMBA_IK_CFA_NOISE_FILTER_s        HIsoLi2CfaFilter;
    UINT8                             HIsoLi2GbGrMismatchUpdate;
    AMBA_IK_GRGB_MISMATCH_s           HIsoLi2GbGrMismatch;
    UINT8                             HIsoLi2DemosaicUpdate;
    AMBA_IK_DEMOSAIC_s                HIsoLi2Demosaic;

    UINT8                             HIsoSelectUpdate;
    AMBA_IK_HI_SELECT_s               HIsoSelect;

    // HISO Luma ASF
    UINT8                             HIsoAsfUpdate;
    AMBA_IK_ADV_SPAT_FLTR_s           HIsoAsf;
    UINT8                             HIsoHighAsfUpdate;
    AMBA_IK_HI_ASF_B2_s               HIsoHighAsf;
    UINT8                             HIsoMedAsfUpdate;
    AMBA_IK_HI_ASF_A_s                HIsoMedAsf;
    UINT8                             HIsoLowAsfUpdate;
    AMBA_IK_HI_ASF_B1_s               HIsoLowAsf;

    UINT8                             HIsoMed2AsfUpdate;
    AMBA_IK_HI_ASF_B2_s               HIsoMed2Asf;
    UINT8                             HIsoLi2AsfUpdate;
    AMBA_IK_ADV_SPAT_FLTR_s           HIsoLi2Asf;
    UINT8                             HIsoHigh2AsfUpdate;
    AMBA_IK_HI_ASF_A_s                HIsoHigh2Asf;


   //High Sharpen
   UINT8                              HIsoHighSharpenBothUpdate;
   AMBA_IK_FSTSHPNS_BOTH_s            HIsoHighSharpenBoth;
   UINT8                              HIsoHighSharpenNoiseUpdate;
   AMBA_IK_HI_HIGH_SHPNS_NOISE_s      HIsoHighSharpenNoise;
   UINT8                              HIsoHighSharpenFirUpdate;
   AMBA_IK_SHPNS_FIR_s                HIsoHighSharpenFir;
   UINT8                              HIsoHighSharpenCoringUpdate;
   AMBA_IK_CORING_s                   HIsoHighSharpenCoring;
   UINT8                              HIsoHighSharpenCoringIndxScaleUpdate;
   AMBA_IK_LEVEL_METHOD_s             HIsoHighSharpenCoringIndxScale;
   UINT8                              HIsoHighSharpenMinCoringResultUpdate;
   AMBA_IK_LEVEL_METHOD_s             HIsoHighSharpenMinCoringResult;
   UINT8                              HIsoHighSharpenScaleCoringUpdate;
   AMBA_IK_LEVEL_METHOD_s             HIsoHighSharpenScaleCoring;
   UINT8                              HIsoHighSharpenMaxCoringResultUpdate;
   AMBA_IK_LEVEL_METHOD_s             HIsoHighSharpenMaxCoringResult;


   //Med Sharpen
    UINT8                             HIsoMedSharpenBothUpdate;
    AMBA_IK_FSTSHPNS_BOTH_s           HIsoMedSharpenBoth;
    UINT8                             HIsoMedSharpenNoiseUpdate;
    AMBA_IK_HI_HIGH_SHPNS_NOISE_s     HIsoMedSharpenNoise;
    UINT8                             HIsoMedSharpenFirUpdate;
    AMBA_IK_SHPNS_FIR_s               HIsoMedSharpenFir;
    UINT8                             HIsoMedSharpenCoringUpdate;
    AMBA_IK_CORING_s                  HIsoMedSharpenCoring;
    UINT8                             HIsoMedSharpenCoringIndexScaleUpdate;
    AMBA_IK_LEVEL_METHOD_s            HIsoMedSharpenCoringIndexScale;
    UINT8                             HIsoMedSharpenMinCoringResultUpdate;
    AMBA_IK_LEVEL_METHOD_s            HIsoMedSharpenMinCoringResult;
    UINT8                             HIsoMedSharpenScaleCoringUpdate;
    AMBA_IK_LEVEL_METHOD_s            HIsoMedSharpenScaleCoring;
    UINT8                             HIsoMedSharpenMaxCoringResultUpdate;
    AMBA_IK_LEVEL_METHOD_s            HIsoMedSharpenMaxCoringResult;

    //Hili Sharpen
    UINT8                             HIsoLiSharpenBBothUpdate;
    AMBA_IK_HI_SHPNS_BOTH_B_s         HIsoLiSharpenBBoth;
    UINT8                             HIsoLiSharpenNoiseUpdate;
    AMBA_IK_HI_HIGH_SHPNS_NOISE_s     HIsoLiSharpenNoise;
    UINT8                             HIsoLiSharpenFirUpdate;
    AMBA_IK_SHPNS_FIR_s               HIsoLiSharpenFir;
    UINT8                             HIsoLiSharpenBCoringUpdate;
    AMBA_IK_HI_CORING_B_s             HIsoLiSharpenBCoring;
    UINT8                             HIsoLiSharpenCoringIndexScaleUpdate;
    AMBA_IK_LEVEL_METHOD_s            HIsoLiSharpenCoringIndexScale;
    UINT8                             HIsoLiSharpenMinCoringResultUpdate;
    AMBA_IK_LEVEL_METHOD_s            HIsoLiSharpenMinCoringResult;
    UINT8                             HIsoLiSharpenScaleCoringUpdate;
    AMBA_IK_LEVEL_METHOD_s            HIsoLiSharpenScaleCoring;
    UINT8                             HIsoLiSharpenMaxCoringResultUpdate;
    AMBA_IK_LEVEL_METHOD_s            HIsoLiSharpenMaxCoringResult;

    //Li2 Sharpen
    UINT8                             HIsoLi2SharpenBBothUpdate;
    AMBA_IK_HI_SHPNS_BOTH_B_s         HIsoLi2SharpenBBoth;
    UINT8                             HIsoLi2SharpenNoiseUpdate;
    AMBA_IK_HI_HIGH_SHPNS_NOISE_s     HIsoLi2SharpenNoise;
    UINT8                             HIsoLi2SharpenFirUpdate;
    AMBA_IK_SHPNS_FIR_s               HIsoLi2SharpenFir;
    UINT8                             HIsoLi2SharpenBCoringUpdate;
    AMBA_IK_HI_CORING_B_s             HIsoLi2SharpenBCoring;
    UINT8                             HIsoLi2SharpenCoringIndexScaleUpdate;
    AMBA_IK_LEVEL_METHOD_s            HIsoLi2SharpenCoringIndexScale;
    UINT8                             HIsoLi2SharpenMinCoringResultUpdate;
    AMBA_IK_LEVEL_METHOD_s            HIsoLi2SharpenMinCoringResult;
    UINT8                             HIsoLi2SharpenScaleCoringUpdate;
    AMBA_IK_LEVEL_METHOD_s            HIsoLi2SharpenScaleCoring;
    UINT8                             HIsoLi2SharpenMaxCoringResultUpdate;
    AMBA_IK_LEVEL_METHOD_s            HIsoLi2SharpenMaxCoringResult;

   //HISO Li Chroma ASF
    UINT8                             HIsoLiChromaAsfUpdate;
    AMBA_IK_CHROMA_ASF_s              HIsoLiChromaAsf;

   //HISO Chroma ASF
    UINT8                             HIsoChromaAsfUpdate;
    AMBA_IK_CHROMA_ASF_s              HIsoChromaAsf;

    //HISO Chroma filter
    UINT8                              HIsoChromaFilterPreUpdate;
    AMBA_IK_HI_CHROMA_FILTER_B_s       HIsoChromaFilterPre;
    UINT8                              HIsoChromaFilterHighUpdate;
    AMBA_IK_CHROMA_FILTER_s            HIsoChromaFilterHigh;
    UINT8                              HIsoChromaFilterMedUpdate;
    AMBA_IK_HI_CHROMA_FILTER_B_s       HIsoChromaFilterMed;
    UINT8                              HIsoChromaFilterLowUpdate;
    AMBA_IK_HI_CHROMA_FILTER_B_s       HIsoChromaFilterLow;
    UINT8                              HIsoChromaFilterVLowUpdate;
    AMBA_IK_HI_CHROMA_FILTER_B_s       HIsoChromaFilterVLow;


    //HISO Chroma Combine
    UINT8                              HIsoChromaFilterMedCombineUpdate;
    AMBA_IK_HI_CHROMA_FILTER_COMBINE_s HIsoChromaFilterMedCombine;
    UINT8                              HIsoChromaFilterLowCombineUpdate;
    AMBA_IK_HI_CHROMA_FILTER_COMBINE_s HIsoChromaFilterLowCombine;
    UINT8                              HIsoChromaFilterVLowCombineUpdate;
    AMBA_IK_HI_CHROMA_FILTER_COMBINE_s HIsoChromaFilterVLowCombine;

    //HISO Hili Combine
    UINT8                              HIsoHiLiCombineUpdate;
    AMBA_IK_HILI_COMBINE_s             HIsoHiLiCombine;

    //HISO Luma Combine
    UINT8                              HIsoLumaFilterCombineUpdate;
    AMBA_IK_HI_LUMA_COMBINE_s          HIsoLumaFilterCombine;
    UINT8                              HIsoLowAsfCombineUpdate;
    AMBA_IK_HI_LUMA_COMBINE_s          HIsoLowAsfCombine;

    //LISO/HISO Combine
    UINT8                              HIsoLiLumaMidHightFreqRcvrUpdate;
    AMBA_IK_HI_MID_HIGH_FREQ_RECOVER_s HIsoLiLumaMidHightFreqRcvr;

    UINT8                              HIsoLumaBlendUpdate;
    AMBA_IK_HI_LUMA_BLEND_s            HIsoLumaBlend;

    UINT8                              HIsoNonSmoothUpdate;
    AMBA_IK_HI_NONSMOOTH_DETECT_s      HIsoNonSmooth;

    UINT8                               HdrBlendUpdate[MAX_AEB_NUM];
    AMBA_IK_HDR_BLEND_s                 HdrBlend[MAX_AEB_NUM];

    UINT8                               HdrBlackCorr0Update;
    AMBA_IK_STATIC_BLC_LVL_s            HdrBlackCorr0;
    UINT8                               HdrBlackCorr1Update;
    AMBA_IK_STATIC_BLC_LVL_s            HdrBlackCorr1;
    UINT8                               HdrBlackCorr2Update;
    AMBA_IK_STATIC_BLC_LVL_s            HdrBlackCorr2;

} PIPELINE_STILL_CONTROL_s;

typedef struct {
    UINT8     enable;
    ADJ_LUT_s low_tab[ADJ_NF_TABLE_COUNT];
    ADJ_LUT_s high_tab[ADJ_NF_TABLE_COUNT];
} WB_1_TAB_s; //wb_1_tab_t

typedef struct {
    ADJ_VIDEO_PARAM_s        *pAdjVideoParam;
    UINT8                    ColorStyle;
} ADJ_VIDEO_ALL_PARAM_s;

typedef struct {
    ADJ_VIDEO_PARAM_s        *pAdjPhotoParam;
} ADJ_PHOTO_ALL_PARAM_s;

typedef struct {
    DOUBLE     AgcGainMinMax[2];
    DOUBLE     ShutterMinMax[2];
} ADJ_EXPOSURE_LIMIT_s;

#define HDR_DECOMPAND_CUSTOM 0xFFFFFFFFU
typedef struct {
    UINT32     DecompressMode;
    UINT32     RawBits;
    UINT32     MaxBits;
    UINT32     MinBits;
    UINT32     RealMaxBits;
    DOUBLE     LongBaseRatio;
    ADJ_EXPOSURE_LIMIT_s ExposureLimit[MAX_EXP_GROUP_NUM];
} ADJ_SENSOR_HDR_INFO_s;

#define HDR_DECOMPAND_TABLE_ENTRY   145U
#define HDR_COMPAND_TABLE_ENTRY      45U
#define HDR_CE_TABLE_ENTRY          256U

#define HISTOGRAM_CFA_Mode 0
#define HISTOGRAM_RGB_Mode 1
#define HISTOGRAM_VIN_Mode 2

typedef struct {
    UINT32                          VersionNum;

    AMBA_IK_CFA_LEAKAGE_FILTER_s    CfaLeakageFilterVideo;
    AMBA_IK_CFA_LEAKAGE_FILTER_s    CfaLeakageFilterStill;

    AMBA_IK_AAA_STAT_INFO_s         AaaStatisticsInfo;

    UINT32                          ColorCorrVideoAddr;
    UINT32                          ColorCorrStillAddr;

    AMBA_IK_RGB_TO_YUV_MATRIX_s     RgbYuvMatrixVideoTv;
    AMBA_IK_RGB_TO_YUV_MATRIX_s     RgbYuvMatrixVideoPc;
    AMBA_IK_RGB_TO_YUV_MATRIX_s     RgbYuvMatrixStill;

    AMBA_IK_CHROMA_SCALE_s          ChromaScaleVideo;
    AMBA_IK_CHROMA_SCALE_s          ChromaScaleStill;

    AMBA_IK_CFA_LEAKAGE_FILTER_s    HIsoCfaLeakageFilter;
    AMBA_IK_CFA_LEAKAGE_FILTER_s    HIsoLi2CfaLeakageFilter;

    /* HDR */
    UINT32                          HistogramMode;    //  0:CFA    1:RGB
    UINT32                          HdrFrameNum;
    UINT32                          HdrExposNum;
    UINT32                          WbPositionCheck;  // 16:Sensor,  32:Front End,  48:Before CE,  64:After CE
    UINT32                          ChkExposMode;     // 0 : none, 1 : SensorDgain,  2 : LinearDgain, 3 : IdspDgain
    ADJ_SENSOR_HDR_INFO_s           SensorHdrInfo;
    DOUBLE                          CeInPutTable;
    DOUBLE                          CeOutPutTable;
} IMG_PARAM_s;
#define IMG_PARAM_VER (0x19042200U)

#endif  /*__AMBA_ADJUSTMENT_H12_H__ */
