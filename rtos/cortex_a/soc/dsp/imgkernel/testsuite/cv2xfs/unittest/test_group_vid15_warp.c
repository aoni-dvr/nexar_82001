#include "test_ik_global.h"


#define ALIGN_N(x_, n_)   ((((x_)+((n_)-1)) & ~((n_)-1)))

static UINT8 ik_working_buffer[5<<20];
static void *pBinDataAddr;
FILE *pFile;
static AMBA_ITN_SYSTEM_s _System;
static ITUNER_DATA_s GData;
UINT32 id_cnt;
static UINT8 WarpGrid[4*256*192];


static INT32 fake_create_mutex(AMBA_KAL_MUTEX_t *mutex)
{
    return 0;
}

static INT32 fake_destroy_mutex(AMBA_KAL_MUTEX_t *mutex)
{
    return 0;
}

static INT32 fake_lock_mutex(AMBA_KAL_MUTEX_t *mutex, UINT32 timeout)
{
    return 0;
}

static INT32 fake_unlock_mutex(AMBA_KAL_MUTEX_t *mutex)
{
    return 0;
}

static INT32 fake_clean_cache(void *addr, size_t size)
{
    return 0;
}

static void* fake_convert_to_physical_address(void* virtual)
{
    return virtual;
}

static void* fake_convert_to_virtual_address(void* phys)
{
    return phys;
}

static INT32 _load_idsp_default_binary(void)
{
    INT32 rval = 0;
    FILE *pFileDefBin;
    UINT8 *pDefBin;
    UINT32 DefSize;

    //read default binary
    pFileDefBin = fopen("../../../../imgkernelcore/cv2xfs/early_test/idspdrv_init_cv2a.bin", "rb" );

    fseek(pFileDefBin, 0, SEEK_END);
    DefSize = ftell(pFileDefBin);
    rewind(pFileDefBin);

    pDefBin = (UINT8 *)malloc(sizeof(UINT8) * DefSize);

    fread(pDefBin, 1, DefSize, pFileDefBin);

    pBinDataAddr = (void *)pDefBin;
    fclose(pFileDefBin);

    return rval;
}

static void _Init_Image_Kernel(AMBA_IK_ABILITY_s *p_ability)
{
    UINT32 rval = IK_OK;
    AMBA_IK_CONTEXT_SETTING_s ContextSetting = {0};
    ik_system_api_t sys_api;
    SIZE_t TotalCtxSize = 0;

    sys_api.system_memcpy = memcpy;
    sys_api.system_memset = memset;
    sys_api.system_memcmp = memcmp;
    sys_api.system_create_mutex = (ik_system_create_mutex_t)fake_create_mutex;
    sys_api.system_destroy_mutex = (ik_system_destroy_mutex_t)fake_destroy_mutex;
    sys_api.system_lock_mutex = (ik_system_lock_mutex_t)fake_lock_mutex;
    sys_api.system_unlock_mutex = (ik_system_unlock_mutex_t)fake_unlock_mutex;
    sys_api.system_print = (ik_system_print_t)printf;
    sys_api.system_clean_cache = (ik_system_clean_cache_t)fake_clean_cache;
    sys_api.system_virtual_to_physical_address = (ik_system_convert_virtual_to_physical_t)fake_convert_to_physical_address;
    sys_api.system_physical_to_virtual_address = (ik_system_convert_physical_to_virtual_t)fake_convert_to_virtual_address;
    ik_import_system_callback_func(&sys_api);

    ContextSetting.ContextNumber = 1;
    ContextSetting.ConfigSetting[0].ConfigNumber = 5;
    ContextSetting.ConfigSetting[0].pAbility = p_ability;

    rval = AmbaIK_QueryArchMemorySize(&ContextSetting, &TotalCtxSize);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_QueryArchMemorySize", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_InitArch(&ContextSetting, (void*)ik_working_buffer, sizeof(ik_working_buffer));
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitArch", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_InitDefBinary(pBinDataAddr);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitDefBinary", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_InitCheckParamFunc();
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitCheckParamFunc", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetDebugLogId(255U, 1U);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_SetDebugLogId", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
}

static void _Init_Image_Kernel_Core(void)
{
    ikc_system_api_t sys_api;

    sys_api.system_memcpy = memcpy;
    sys_api.system_memset = memset;
    sys_api.system_memcmp = memcmp;
    sys_api.system_print = (ikc_system_print_t)printf;
    ikc_import_system_callback_func(&sys_api);
}

static void _GData_SystemExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;
    AMBA_IK_WINDOW_SIZE_INFO_s WindowSizeInfo = {0};
    UINT32 FlipInfo = 0;
    AMBA_IK_STITCH_INFO_s StitchInfo = {0};
    AMBA_IK_WARP_BUFFER_INFO_s WarpBufInfo = {0};

    _System.RawWidth = 1920;
    _System.RawHeight = 1080;
    _System.RawPitch = 3840;
    _System.RawResolution = 14;
    _System.RawBayer = 2;
    _System.HSubSampleFactorNum = 1U;
    _System.HSubSampleFactorDen = 1U;
    _System.VSubSampleFactorNum = 1U;
    _System.VSubSampleFactorDen = 1U;
    _System.MainWidth = 1920;
    _System.MainHeight = 1080;
    _System.NumberOfExposures = 2;
    _System.Ability = AMBA_IK_VIDEO_HDR_EXPO_2;

    rval = AmbaIK_GetWindowSizeInfo(pMode, &WindowSizeInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetWindowSizeInfo", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    WindowSizeInfo.VinSensor.StartX                 = _System.RawStartX;
    WindowSizeInfo.VinSensor.StartY                 = _System.RawStartY;
    WindowSizeInfo.VinSensor.Width                  = _System.RawWidth;
    WindowSizeInfo.VinSensor.Height                 = _System.RawHeight;
    WindowSizeInfo.VinSensor.HSubSample.FactorNum   = _System.HSubSampleFactorNum;
    WindowSizeInfo.VinSensor.HSubSample.FactorDen   = _System.HSubSampleFactorDen;
    WindowSizeInfo.VinSensor.VSubSample.FactorNum   = _System.VSubSampleFactorNum;
    WindowSizeInfo.VinSensor.VSubSample.FactorDen   = _System.VSubSampleFactorDen;
    WindowSizeInfo.Main.Width                       = _System.MainWidth;
    WindowSizeInfo.Main.Height                      = _System.MainHeight;
    rval = AmbaIK_SetWindowSizeInfo(pMode, &WindowSizeInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWindowSizeInfo", "OK case in ik_ctest_vid15_warp.c", id_cnt++);

    // Update Flip Info
    FlipInfo = 0UL;
    rval = AmbaIK_SetFlipMode(pMode, FlipInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFlipMode", "OK case in ik_ctest_vid15_warp.c", id_cnt++);

    // make img_exe_execute_filter_update_check() happy...
    StitchInfo.Enable = 0;
    rval = AmbaIK_SetStitchingInfo(pMode, &StitchInfo);
    WarpBufInfo.LumaWaitLines = 8;
    WarpBufInfo.LumaDmaSize = 32;
    WarpBufInfo.Reserved = 32; // chroma_dma

    rval = AmbaIK_SetWarpBufferInfo(pMode, &WarpBufInfo);
}

static void _GData_VinSensorExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0u;

    rval= AmbaIK_GetVinSensorInfo(pMode, &GData.SensorInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetVinSensorInfo", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    GData.SensorInfo.SensorResolution = _System.RawResolution;
    GData.SensorInfo.SensorPattern = _System.RawBayer;
    GData.SensorInfo.SensorReadoutMode = _System.SensorReadoutMode;
    GData.SensorInfo.Compression = _System.CompressedRaw;
    GData.SensorInfo.SensorMode = _System.SensorMode;
    GData.SensorInfo.CompressionOffset = _System.CompressionOffset;
    rval= AmbaIK_SetVinSensorInfo(pMode, &GData.SensorInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVinSensorInfo", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
}

static void _GData_RgbIrExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval= AmbaIK_SetRgbIr(pMode, &GData.RgbIr);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbIr", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
}

static void _GData_Raw2RawExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_SetFeToneCurve(pMode, &GData.FeToneCurve.Decompand);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeToneCurve", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetFeStaticBlc(pMode, &GData.StaticBlackLevel[0], 0UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeStaticBlc", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetDynamicBadPxlCor(pMode, &GData.DynamicBadPixelCorrection);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDynamicBadPxlCor", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetCfaLeakageFilter(pMode, &GData.CfaLeakageFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCfaLeakageFilter", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetCfaNoiseFilter(pMode, &GData.CfaNoiseFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCfaNoiseFilter", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetAntiAliasing(pMode, &GData.AntiAliasing);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAntiAliasing", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetFeWbGain(pMode, &GData.FrontEndWbGain[0], 0UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeWbGain", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetAfterCeWbGain(pMode, &GData.AfterCeWbGain);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAfterCeWbGain", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetBeforeCeWbGain(pMode, &GData.BeforeCeWbGain);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetBeforeCeWbGain", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetDemosaic(pMode, &GData.DemosaicFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDemosaic", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetGrgbMismatch(pMode, &GData.GbGrMismatch);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetGrgbMismatch", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetRgbTo12Y(pMode, &GData.RgbTo12Y);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbTo12Y", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
}

static void _GData_Yuv2YuvExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0u;
    FILE *pBinFile;
    uint32 BinSize;

    {
        pBinFile = fopen("../util/bin/mn34220pl_0p3lux_cc_3d.bin", "rb");

        BinSize = IK_CC_3D_SIZE;

        fread(GData.CcThreeD.MatrixThreeDTable, 1, BinSize, pBinFile);
        fclose(pBinFile);
    }
    rval= AmbaIK_SetColorCorrection(pMode, &GData.CcThreeD);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetColorCorrection", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetToneCurve(pMode, &GData.ToneCurve);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetToneCurve", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetRgbToYuvMatrix(pMode, &GData.RgbToYuvMatrix);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbToYuvMatrix", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetChromaScale(pMode, &GData.ChromaScale);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaScale", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetChromaMedianFilter(pMode, &GData.ChromaMedianFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaMedianFilter", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetChromaFilter(pMode, &GData.ChromaFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaFilter", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetWideChromaFilter(pMode, &GData.WideChromaFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWideChromaFilter", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetWideChromaFilterCombine(pMode, &GData.WideChromaFilterCombine);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWideChromaFilterCombine", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetLumaNoiseReduction(pMode, &GData.LumaNoiseReduction);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetLumaNoiseReduction", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetFirstLumaProcMode(pMode, &GData.FirstLumaProcessMode);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFirstLumaProcMode", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetAdvSpatFltr(pMode, &GData.AsfInfo.AsfInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAdvSpatFltr", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
}

static void _GData_MctfExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_SetFnlShpNsBoth(pMode, &GData.FinalSharpenBoth.BothInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsBoth", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsBothTdt(pMode, &GData.FinalSharpenBoth.ThreedTable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsBothTdt", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsNoise(pMode, &GData.FinalSharpenNoise);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsNoise", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsFir(pMode, &GData.FinalFir);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsFir", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsCoring(pMode, &GData.FinalCoring);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsCoring", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsCorIdxScl(pMode, &GData.FinalCoringIndexScale);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsCorIdxScl", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsMinCorRst(pMode, &GData.FinalMinCoringResult);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsMinCorRst", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsMaxCorRst(pMode, &GData.FinalMaxCoringResult);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsMaxCorRst", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsSclCor(pMode, &GData.FinalScaleCoring);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsSclCor", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetVideoMctf(pMode, &GData.VideoMctf);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctf", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetVideoMctfTa(pMode, &GData.VideoMctfTemporalAdjust);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctfTa", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetVideoMctfAndFnlshp(pMode, &GData.VideoMctfAndFinalSharpen);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctfAndFnlshp", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
}

static void _GData_Raw2yuvCalibExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0u;
    AMBA_IK_WARP_INFO_s CalibWarpInfo;
    FILE *pBinFile;
    uint32 BinSize;

    rval= AmbaIK_SetVinActiveWin(pMode, &GData.Active);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVinActiveWin", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetDummyMarginRange(pMode, &GData.Dummy);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDummyMarginRange", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetDzoomInfo(pMode, &GData.Dzoom);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDzoomInfo", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval= AmbaIK_SetStaticBadPxlCorEnb(pMode, GData.StaticBadPixelCorrection.Enable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetStaticBadPxlCorEnb", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetVignetteEnb(pMode, GData.VignetteCompensation.Enable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVignetteEnb", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetCawarpEnb(pMode, (UINT32)GData.ChromaAberrationInfo.Enable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCawarpEnb", "OK case in ik_ctest_vid15_warp.c", id_cnt++);

    rval= AmbaIK_SetWarpEnb(pMode, 1);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWarpEnb", "OK case in ik_ctest_vid15_warp.c", id_cnt++);

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
    CalibWarpInfo.Enb_2StageCompensation = 0u; // remap in ik service, not support
    CalibWarpInfo.pWarp = (AMBA_IK_GRID_POINT_s *)WarpGrid;

    {
        pBinFile = fopen("../util/bin/warp.bin", "rb");

        BinSize = CalibWarpInfo.HorGridNum * CalibWarpInfo.VerGridNum * sizeof(AMBA_IK_GRID_POINT_s);

        fread(WarpGrid, 1, BinSize, pBinFile);
        fclose(pBinFile);
    }
    rval= AmbaIK_SetWarpInfo(pMode, &CalibWarpInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWarpInfo", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
}

static void _GData_CeExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0u;

    rval = AmbaIK_SetCe(pMode, &GData.ContrastEnhance.ContrastEnhance);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCe", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetCeInputTable(pMode, &GData.ContrastEnhance.ContrastEnhanceInputTable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCeInputTable", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetCeOutputTable(pMode, &GData.ContrastEnhance.ContrastEnhanceOutTable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCeOutputTable", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
}

static void _GData_VideoHdrExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_SetHdrBlend(pMode, &GData.VideoHdr.HdrBlend);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetHdrBlend", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetHdrRawOffset(pMode, &GData.VideoHdr.RawInfo.Offset);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetHdrRawOffset", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetFeWbGain(pMode, &GData.FrontEndWbGain[1], 1UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeWbGain", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
    rval = AmbaIK_SetFeStaticBlc(pMode, &GData.StaticBlackLevel[1], 1UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeStaticBlc", "OK case in ik_ctest_vid15_warp.c", id_cnt++);
}

static void _GData_ExecuteVideoHdrExpo(const AMBA_IK_MODE_CFG_s *pMode)
{
    _GData_SystemExecute(pMode);
    _GData_VinSensorExecute(pMode);
    _GData_RgbIrExecute(pMode);
    _GData_Raw2RawExecute(pMode);
    _GData_Yuv2YuvExecute(pMode);
    _GData_MctfExecute(pMode);
    _GData_Raw2yuvCalibExecute(pMode);
    _GData_CeExecute(pMode);
    _GData_VideoHdrExecute(pMode);
}

static void _GData_Execute(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_ABILITY_s *pAbility)
{
    FILE *pBinFile;
    uint32 BinSize;

    {
        pBinFile = fopen("../util/bin/vid15_warp_gdata.bin", "rb");

        fseek(pBinFile, 0, SEEK_END);
        BinSize = ftell(pBinFile);
        rewind(pBinFile);

        fseek(pBinFile, sizeof(AMBA_ITN_SYSTEM_s), SEEK_SET);
        BinSize -= sizeof(AMBA_ITN_SYSTEM_s);
        fread(&GData, 1, BinSize, pBinFile);
        fclose(pBinFile);
    }
    _GData_ExecuteVideoHdrExpo(pMode);
}

INT32 ik_ctest_vid15_warp_main(void)
{
    UINT32 Rval = IK_OK;
    AMBA_IK_ABILITY_s Ability;
    AMBA_IK_MODE_CFG_s Mode;
    AMBA_IK_EXECUTE_CONTAINER_s ExecuteContainer = {0};

    memset(ik_working_buffer, 0, sizeof(ik_working_buffer));
    memset(&GData, 0, sizeof(ITUNER_DATA_s));
    memset(&_System, 0, sizeof(AMBA_ITN_SYSTEM_s));
    memset(WarpGrid, 0, sizeof(WarpGrid));
    id_cnt = 0;

    pFile = fopen("data/ik_ctest_vid15_warp.txt", "w");

    // init IK
    _load_idsp_default_binary();
    Ability.Pipe = AMBA_IK_PIPE_VIDEO;
    Ability.VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_2;

    _Init_Image_Kernel(&Ability);
    _Init_Image_Kernel_Core();

    Mode.ContextId = 0;
    Rval |= AmbaIK_InitContext(&Mode, &Ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitContext", "OK case in ik_ctest_vid15_warp.c", id_cnt++);

    // test
    _GData_Execute(&Mode, &Ability);

    Rval |= AmbaIK_ExecuteConfig(&Mode, &ExecuteContainer);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "OK case in ik_ctest_vid15_warp.c", id_cnt++);

    {
        SIZE_t size;
        UINT32 size_align;
        void *buffer, *data;
        IDSP_FLOW_HEADER_s *p_header;
        Rval |= AmbaIK_QueryDebugConfigSize(&size);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_QueryDebugConfigSize", "OK case in ik_ctest_vid15_warp.c", id_cnt++);

        size_align = size + 128; // extra 128 bytes for cr buf alignment
        buffer = (void*)malloc(size_align);
        data = (void *) ALIGN_N((uintptr)buffer, 128);
        Rval |= AmbaIK_DumpDebugConfig(&Mode, 0, data, size);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_DumpDebugConfig", "OK case in ik_ctest_vid15_warp.c", id_cnt++);

        p_header = (IDSP_FLOW_HEADER_s *)data;
        p_header->IdspFlowAddr = 0;

        {
            //write ikc
            FILE *fid;

            fid = fopen("data/ik_ctest_vid15_warp.bin", "wb");
            fwrite(data, 1, sizeof(amalgam_flow_data_t), fid);
            fclose(fid);
        }
        free(buffer);
    }

    free((UINT8 *) pBinDataAddr);
    pBinDataAddr = NULL;
    fclose(pFile);

    fake_destroy_mutex(NULL);
    fake_clean_cache(NULL, 0);

    img_arch_deinit_architecture();
    iks_arch_deinit_architecture();

    return Rval;
}

