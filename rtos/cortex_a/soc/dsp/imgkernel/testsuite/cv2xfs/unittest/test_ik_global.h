#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageFilterInternal.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImgProcess.h"
#include "AmbaDSP_ImgArchSpec.h"
#include "AmbaDSP_ImgArchComponentIF.h"
#include "AmbaDSP_ImgCommon.h"
#include "AmbaDSP_ImgContextEntity.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgConfigEntity.h"
#include "AmbaDSP_ImgConfigComponentIF.h"
#include "AmbaDSP_IksUtility.h"
#include "idspdrv.h"


void _ctest_log_rval(FILE *pFile, uint32 rval, char *p_file_name, char *p_func_name, char *p_desc, uint32 id);
#define LOG_RVAL(pfile_, rval_, file_name_, func_name_, desc_, id_, ...)        \
    do {                                                                 \
        _ctest_log_rval(pFile, rval_, file_name_, func_name_, desc_, id_);      \
    } while (0)

void AmbaPrint_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);

/* The pointer to the type must be in 32-bit alignment */
void AmbaMisra_TypeCast32(void * pNewType, const void * pOldType);

/* The pointer to the type must be in 64-bit alignment */
void AmbaMisra_TypeCast64(void * pNewType, const void * pOldType);

void AmbaMisra_TypeCast(void * pNewType, const void * pOldType);

extern void img_arch_deinit_architecture(void);
extern void iks_arch_deinit_architecture(void);

#define ITUNER_MAX_FPN_MAP_SIZE (6000*4000/8)

// ituner data
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
    UINT32 Enable;
    UINT32 PixelMapWidth;
    UINT32 PixelMapHeight;
    UINT32 PixelMapPitch;
    char MapPath[128];
} ITUNER_FPN_INTERNAL_s;

typedef struct {
    UINT8 SensorId;
    UINT8 BayerPattern;
    UINT8 SensorResolution;
    UINT32 ReadoutMode;
} ITUNER_SENSOR_INPUT_s;

typedef struct {
    UINT32 MatrixEn;
    UINT32 MatrixShiftMinus8;
    INT32 Matrix[6];
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
    UINT32 RawAreaWidth[3];
    UINT32 RawAreaHeight[3];
    AMBA_IK_HDR_RAW_INFO_s Offset;
} AMBA_ITN_VIDEO_HDR_RAW_INFO_s;

typedef struct {
    AMBA_IK_STATIC_BLC_LVL_s LEStaticBlackLevel[3];
    AMBA_IK_HDR_BLEND_s HdrBlend;
    AMBA_ITN_VIDEO_HDR_RAW_INFO_s RawInfo;
} ITUNER_VIDEO_HDR_s;

typedef struct {
    UINT32 CompandEnable;
    UINT32 CompandTable[IK_NUM_COMPAND_TABLE];
} ITUNER_FE_TONE_CURVE_COMPAND_s;

typedef struct {
    ITUNER_FE_TONE_CURVE_COMPAND_s Compand;
    AMBA_IK_FE_TONE_CURVE_s Decompand;
} ITUNER_FE_TONE_CURVE_s;

typedef struct {
    UINT32 Enable;
} AMBA_IK_DEFERRED_BLC_s;

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
    AMBA_IK_DEFERRED_BLC_s DeferredBlackLevel;
    AMBA_IK_DYNAMIC_BAD_PXL_COR_s DynamicBadPixelCorrection;
    AMBA_IK_CFA_LEAKAGE_FILTER_s CfaLeakageFilter;
    AMBA_IK_CFA_NOISE_FILTER_s CfaNoiseFilter;
    AMBA_IK_ANTI_ALIASING_s AntiAliasing;
    AMBA_IK_WB_GAIN_s BeforeCeWbGain;
    AMBA_IK_WB_GAIN_s AfterCeWbGain;
    AMBA_IK_COLOR_CORRECTION_s CcThreeD;
    ITUNER_COLOR_CORRECTION_s ColorCorrection;
    AMBA_IK_TONE_CURVE_s ToneCurve;
    ITUNER_FE_TONE_CURVE_s FeToneCurve;
    UINT32 Decompand146Reserved;
    AMBA_IK_RGB_TO_YUV_MATRIX_s RgbToYuvMatrix;
    AMBA_IK_RGB_IR_s RgbIr;
    AMBA_IK_CHROMA_SCALE_s ChromaScale;
    AMBA_IK_CHROMA_MEDIAN_FILTER_s ChromaMedianFilter;
    AMBA_IK_DEMOSAIC_s DemosaicFilter;
    AMBA_IK_RGB_TO_12Y_s RgbTo12Y;
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
    AMBA_IK_LUMA_NOISE_REDUCTION_s LumaNoiseReduction;
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
    AMBA_IK_WARP_INTERNAL_INFO_s WarpInternal;
    AMBA_IK_CAWARP_INTERNAL_INFO_s CAInternal;
    AMBA_IK_CAWARP_INFO_s CaWarpCompensationInfo;
    AMBA_IK_STATIC_BAD_PXL_COR_s SbpCorr;
    AMBA_IK_SBP_INTERNAL_INFO_s SbpInternal;
    AMBA_IK_FE_WB_GAIN_s FrontEndWbGain[4];
    AMBA_IK_STATIC_BLC_LVL_s StaticBlackLevel[4];
    AMBA_IK_WIDE_CHROMA_FILTER_s WideChromaFilter;
    AMBA_IK_WIDE_CHROMA_FILTER_COMBINE_s WideChromaFilterCombine;
    AMBA_IK_DUMMY_MARGIN_RANGE_s Dummy;
    AMBA_IK_VIN_ACTIVE_WINDOW_s Active;
} ITUNER_DATA_s;

typedef struct {
    UINT32 ItunerRev;
    char SensorId[32];
    char TuningMode[32];
    char RawPath[128];
    UINT32 RawWidth;
    UINT32 RawHeight;
    UINT32 RawPitch;
    UINT32 RawResolution;
    UINT32 RawBayer;
    UINT32 MainWidth;
    UINT32 MainHeight;
    UINT32 InputPicCnt;
    UINT32 CompressedRaw;
    UINT32 SensorReadoutMode;
    UINT32 RawStartX;
    UINT32 RawStartY;
    UINT32 HSubSampleFactorNum;
    UINT32 HSubSampleFactorDen;
    UINT32 VSubSampleFactorNum;
    UINT32 VSubSampleFactorDen;
    UINT32 NumberOfExposures;
    UINT32 SensorMode; // 0:normal, 1:rgb_ir, 2:rccc
    INT32 CompressionOffset;
    UINT32 Ability;
    UINT32 YuvMode;
    UINT32 FlipH;
    UINT32 FlipV;
    UINT32 FrameNumber;
    UINT32 NumberOfFrames;
} AMBA_ITN_SYSTEM_s;

typedef struct {
    UINT8 FPNMap[ITUNER_MAX_FPN_MAP_SIZE]; //!< FPN Map Need 4 Bytes Alignment
    UINT8 WarpGrid[4*256*192];
    UINT8 WarpHor[2*128*96];
    UINT8 WarpVer[2*128*96];
    UINT32 Cc3d[IK_CC_3D_SIZE/4U];
    UINT8 CawarpRedGrid[4*64*96];
    UINT8 CawarpBlueGrid[4*64*96];
    UINT32 *pVignetteMap;
} ITUNER_Calib_Table_s;

typedef struct {
    UINT32 Reserved0;
    UINT32 Reserved1;
    UINT32 IdspFlowAddr;     /* dram address of this idsp_ik_flow_ctrl_t instance */
} IDSP_FLOW_HEADER_s;

typedef struct {
    uint32 is_group_cmd;
    // from p_in
    int32   cawarp_enable;
    uint32  flip_mode;
    uint32  stitch_enable;
    const ik_window_size_info_t *p_window_size_info;
    const amba_ik_calc_win_result_t *p_result_win;

    const ik_cawarp_info_t *p_calib_ca_warp_info;

    // from p_out
    cawarp_cfg_t *p_ca_warp_control;
} amba_ik_cawarp_calc_t;

typedef struct {
    uint32 is_group_cmd;
    // from p_in
    int32   warp_enable;
    uint32  flip_mode;
    const ik_stitch_info_t *p_stitching_info;
    const ik_window_size_info_t *p_window_size_info;
    const ik_vin_active_window_t *p_active_window;
    const ik_dummy_margin_range_t *p_dmy_range;
    amba_ik_calc_win_result_t *p_result_win;
    amba_ik_warp_phase_t *p_warp_phase;

    const ik_warp_info_t *p_calib_warp_info;

    // from p_out
    set_warp_control_t *p_geo_warp_control;
} amba_ik_warp_calc_t;

typedef struct {
    amba_ik_calc_win_result_t result_win;
    uint8 reserved0[128u-(sizeof(amba_ik_calc_win_result_t)%128u)];
    idsp_flow_info_t flow_info;
    uint8 reserved1[128u-(sizeof(idsp_flow_info_t)%128u)];
    idsp_stitch_data_t stitch;
    uint8 reserved2[128u-(sizeof(idsp_stitch_data_t)%128u)];
} amba_iks_calc_geo_work_buf_t;

typedef struct {
    uint32 specify;
    uint32 per_dir_fir_iso_strengths[9];
    uint32 per_dir_fir_dir_strengths[9];
    uint32 per_dir_fir_dir_amounts[9];
    int32 coefs[9][25];
    uint32 strength_iso;
    uint32 strength_dir;
    uint32 wide_edge_detect;
    uint32 edge_thresh;
} iks_user_fir_interface;

typedef struct {
    int32 enable;
    iks_user_fir_interface user_fir;
    ik_level_method_t Adapt;
    ik_level_method_t TurnOff;
    int32 directional_decide_t0;
    int32 directional_decide_t1;
    int32 FiltChroma;
    int32 Coring[256];

    // for non-A10 that are int32 (not level based)
    int32             max_change_up_int;
    int32             max_change_down_int;
} iks_advanced_spatial_filt_gui_t;

typedef struct {
    uint32 t;
    int32 enable;
    int32 is9x9;
    int32 transpose;
    int32 subtract_iden;
    int32 desired_sum;
    int32 convert_low_pass_2_high_pass;
} iks_fir_programming_control;

typedef struct {
    int32    FIR[9][25];        // SHP BCD
    int32    FIR_SHIFT[9];
} idsp_fir;

typedef struct {
    const ik_first_sharpen_both_t *sharpen_both;
    const ik_first_sharpen_noise_t *sharpen_noise;
    const ik_first_sharpen_fir_t *iks_shp_fir;
    const ik_first_sharpen_coring_t *coring;
    const ik_first_sharpen_min_coring_result_t *min_coring_result; //8//minimum_sharpeness_level
    const ik_first_sharpen_max_coring_result_t *max_coring_result; //8//minimum_sharpeness_level
    const ik_first_sharpen_coring_idx_scale_t *coring_index_scale; //10//overall_sharpeness_level
    const ik_first_sharpen_scale_coring_t *scale_coring; //7
    //Working buffer
    int32 sharp_coefs[9][25];
    int32 filt_coefs[9][25];
} amba_ik_input_shp_t;

typedef struct {
    /*
    uint8 low;
    uint8 low_delta;
    uint8 low_strength;
    uint8 mid_strength;
    uint8 high;
    uint8 high_delta;
    uint8 high_strength;
    */
    int32 enable;
    int32 low;
    int32 low0;
    int32 low_delta;
    int32 low_strength;
    int32 mid_strength;
    int32 high;
    int32 high0;
    int32 high_delta;
    int32 high_strength;
    int32 method;
} iks_level_control;

typedef struct {
    int32 warp_enable;
    uint32 ability;
    ik_window_size_info_t *p_window_size_info;
    ik_vin_active_window_t *p_active_window;
    ik_dummy_margin_range_t *p_dmy_range;
    ik_dzoom_info_t *p_dzoom_info;
    ik_stitch_info_t *p_stitching_info;
    amba_ik_calc_win_result_t *p_result_win;
} amba_iks_win_calc_t;


// ok cases
INT32 ik_ctest_hdr20_main(void);
INT32 ik_ctest_ir_only20_main(void);
INT32 ik_ctest_y2y422_main(void);
INT32 ik_ctest_y2y420_cc_main(void);
INT32 ik_ctest_calib16_main(void);
INT32 ik_ctest_vid15_sbp_main(void);
INT32 ik_ctest_vid15_ca_main(void);
INT32 ik_ctest_vid15_warp_main(void);
INT32 ik_ctest_vid8_ldly_tile2x4_main(void);
INT32 ik_ctest_aaa_main(void);
INT32 ik_ctest_remap_main(void);

// other cases
INT32 ik_ctest_main(void);
INT32 ik_ctest_arch_main(void);
INT32 ik_ctest_config_main(void);
INT32 ik_ctest_context_main(void);
INT32 ik_ctest_executer_main(void);
INT32 ikc_ctest_main(void);
INT32 iks_ctest_main(void);
INT32 iks_ctest_arch_main(void);
INT32 iks_ctest_config_main(void);
INT32 iks_ctest_context_main(void);
INT32 iks_ctest_executer_main(void);
INT32 iks_ctest_idspdrv_main(void);

// misc
INT32 safety_stub_ctest_main(void);

