#include "test_ik_global.h"


#define ALIGN_N(x_, n_)   ((((x_)+((n_)-1)) & ~((n_)-1)))

static UINT8 ik_working_buffer[5<<20];
static void *pBinDataAddr;
FILE *pFile;
static AMBA_ITN_SYSTEM_s _System;
static ITUNER_DATA_s GData;
static ITUNER_DATA_s GDataGet;
UINT32 id_cnt;


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
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_QueryArchMemorySize", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_InitArch(&ContextSetting, (void*)ik_working_buffer, sizeof(ik_working_buffer));
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitArch", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_InitDefBinary(pBinDataAddr);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitDefBinary", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_InitCheckParamFunc();
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitCheckParamFunc", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetDebugLogId(255U, 1U);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_SetDebugLogId", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
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
    AMBA_IK_STITCH_INFO_s StitchInfo = {0};
    AMBA_IK_WARP_BUFFER_INFO_s WarpBufInfo = {0};
    UINT32 FlipInfo;

    _System.RawWidth = 1920;
    _System.RawHeight = 1080;
    _System.RawPitch = 3840;
    _System.RawResolution = 14;
    _System.RawBayer = 1;
    _System.HSubSampleFactorNum = 1U;
    _System.HSubSampleFactorDen = 1U;
    _System.VSubSampleFactorNum = 1U;
    _System.VSubSampleFactorDen = 1U;
    _System.MainWidth = 1920;
    _System.MainHeight = 1080;
    //_System.StitchEnable = 1;
    //_System.StitchTileNumX = 2;
    //_System.StitchTileNumY = 4;
    //_System.DramEfficiency = 2;
    //_System.vWarpWaitLine = 64;
    //_System.vWarpLumaDma = 32;
    //_System.vWarpChromaDma = 32;
    _System.Ability = AMBA_IK_VIDEO_LINEAR;

    rval = AmbaIK_GetWindowSizeInfo(pMode, &WindowSizeInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetWindowSizeInfo", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
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
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWindowSizeInfo", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);

    // Update Flip Info
    FlipInfo = 0UL;
    rval = AmbaIK_SetFlipMode(pMode, FlipInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFlipMode", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);

    StitchInfo.Enable = 1;
    StitchInfo.TileNum_x = 2;
    StitchInfo.TileNum_y = 4;
    rval = AmbaIK_SetStitchingInfo(pMode, &StitchInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetStitchingInfo", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);

    WarpBufInfo.DramEfficiency = 2;
    WarpBufInfo.LumaWaitLines = 64;
    WarpBufInfo.LumaDmaSize = 32;
    WarpBufInfo.Reserved = 32; // chroma_dma

    rval = AmbaIK_SetWarpBufferInfo(pMode, &WarpBufInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWarpBufferInfo", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
}

static void _GData_VinSensorExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0u;

    rval= AmbaIK_GetVinSensorInfo(pMode, &GData.SensorInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetVinSensorInfo", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    GData.SensorInfo.SensorResolution = _System.RawResolution;
    GData.SensorInfo.SensorPattern = _System.RawBayer;
    GData.SensorInfo.SensorReadoutMode = _System.SensorReadoutMode;
    GData.SensorInfo.Compression = _System.CompressedRaw;
    GData.SensorInfo.SensorMode = _System.SensorMode;
    GData.SensorInfo.CompressionOffset = _System.CompressionOffset;
    rval= AmbaIK_SetVinSensorInfo(pMode, &GData.SensorInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVinSensorInfo", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
}

static void _GData_RgbIrExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval= AmbaIK_SetRgbIr(pMode, &GData.RgbIr);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbIr", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
}

static void _GData_Raw2RawExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_SetFeToneCurve(pMode, &GData.FeToneCurve.Decompand);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeToneCurve", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetFeStaticBlc(pMode, &GData.StaticBlackLevel[0], 0UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeStaticBlc", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetDynamicBadPxlCor(pMode, &GData.DynamicBadPixelCorrection);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDynamicBadPxlCor", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetCfaLeakageFilter(pMode, &GData.CfaLeakageFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCfaLeakageFilter", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetCfaNoiseFilter(pMode, &GData.CfaNoiseFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCfaNoiseFilter", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetAntiAliasing(pMode, &GData.AntiAliasing);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAntiAliasing", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetFeWbGain(pMode, &GData.FrontEndWbGain[0], 0UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeWbGain", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetAfterCeWbGain(pMode, &GData.AfterCeWbGain);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAfterCeWbGain", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetBeforeCeWbGain(pMode, &GData.BeforeCeWbGain);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetBeforeCeWbGain", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetDemosaic(pMode, &GData.DemosaicFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDemosaic", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetGrgbMismatch(pMode, &GData.GbGrMismatch);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetGrgbMismatch", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetRgbTo12Y(pMode, &GData.RgbTo12Y);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbTo12Y", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
}

static void _GData_Yuv2YuvExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0u;
    FILE *pBinFile;
    uint32 BinSize;

    {
        pBinFile = fopen("../util/bin/night1_CC_3d.bin", "rb");

        BinSize = IK_CC_3D_SIZE;

        fread(GData.CcThreeD.MatrixThreeDTable, 1, BinSize, pBinFile);
        fclose(pBinFile);
    }
    rval= AmbaIK_SetColorCorrection(pMode, &GData.CcThreeD);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetColorCorrection", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetToneCurve(pMode, &GData.ToneCurve);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetToneCurve", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetRgbToYuvMatrix(pMode, &GData.RgbToYuvMatrix);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbToYuvMatrix", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetChromaScale(pMode, &GData.ChromaScale);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaScale", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetChromaMedianFilter(pMode, &GData.ChromaMedianFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaMedianFilter", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetChromaFilter(pMode, &GData.ChromaFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaFilter", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetWideChromaFilter(pMode, &GData.WideChromaFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWideChromaFilter", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetWideChromaFilterCombine(pMode, &GData.WideChromaFilterCombine);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWideChromaFilterCombine", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetLumaNoiseReduction(pMode, &GData.LumaNoiseReduction);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetLumaNoiseReduction", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetFirstLumaProcMode(pMode, &GData.FirstLumaProcessMode);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFirstLumaProcMode", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);

    rval = AmbaIK_SetFstShpNsBoth(pMode, &GData.SharpenBoth.BothInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsBoth", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetFstShpNsNoise(pMode, &GData.SharpenNoise);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsNoise", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetFstShpNsFir(pMode, &GData.Fir);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsFir", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetFstShpNsCoring(pMode, &GData.Coring);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsCoring", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetFstShpNsCorIdxScl(pMode, &GData.CoringIndexScale);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsCorIdxScl", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetFstShpNsMinCorRst(pMode, &GData.MinCoringResult);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsMinCorRst", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetFstShpNsMaxCorRst(pMode, &GData.MaxCoringResult);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsMaxCorRst", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetFstShpNsSclCor(pMode, &GData.ScaleCoring);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsSclCor", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
}

static void _GData_MctfExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_SetFnlShpNsBoth(pMode, &GData.FinalSharpenBoth.BothInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsBoth", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsBothTdt(pMode, &GData.FinalSharpenBoth.ThreedTable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsBothTdt", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsNoise(pMode, &GData.FinalSharpenNoise);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsNoise", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsFir(pMode, &GData.FinalFir);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsFir", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsCoring(pMode, &GData.FinalCoring);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsCoring", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsCorIdxScl(pMode, &GData.FinalCoringIndexScale);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsCorIdxScl", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsMinCorRst(pMode, &GData.FinalMinCoringResult);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsMinCorRst", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsMaxCorRst(pMode, &GData.FinalMaxCoringResult);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsMaxCorRst", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsSclCor(pMode, &GData.FinalScaleCoring);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsSclCor", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetVideoMctf(pMode, &GData.VideoMctf);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctf", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetVideoMctfTa(pMode, &GData.VideoMctfTemporalAdjust);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctfTa", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetVideoMctfAndFnlshp(pMode, &GData.VideoMctfAndFinalSharpen);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctfAndFnlshp", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
}

static void _GData_Raw2yuvCalibExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0u;

    rval= AmbaIK_SetVinActiveWin(pMode, &GData.Active);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVinActiveWin", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetDummyMarginRange(pMode, &GData.Dummy);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDummyMarginRange", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetDzoomInfo(pMode, &GData.Dzoom);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDzoomInfo", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetStaticBadPxlCorEnb(pMode, GData.StaticBadPixelCorrection.Enable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetStaticBadPxlCorEnb", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetVignetteEnb(pMode, GData.VignetteCompensation.Enable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVignetteEnb", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_SetCawarpEnb(pMode, (UINT32)GData.ChromaAberrationInfo.Enable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCawarpEnb", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_SetWarpEnb(pMode, 0);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWarpEnb", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
}

static void _GData_ExecuteVideoLinear(const AMBA_IK_MODE_CFG_s *pMode)
{
    _GData_SystemExecute(pMode);
    _GData_VinSensorExecute(pMode);
    _GData_RgbIrExecute(pMode);
    _GData_Raw2RawExecute(pMode);
    _GData_Yuv2YuvExecute(pMode);
    _GData_MctfExecute(pMode);
    _GData_Raw2yuvCalibExecute(pMode);
}

static void _GData_Execute(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_ABILITY_s *pAbility)
{
    FILE *pBinFile;
    uint32 BinSize;

    {
        pBinFile = fopen("../util/bin/vid8_ldly_tile2x4_gdata.bin", "rb");

        fseek(pBinFile, 0, SEEK_END);
        BinSize = ftell(pBinFile);
        rewind(pBinFile);

        fseek(pBinFile, sizeof(AMBA_ITN_SYSTEM_s), SEEK_SET);
        BinSize -= sizeof(AMBA_ITN_SYSTEM_s);
        fread(&GData, 1, BinSize, pBinFile);
        fclose(pBinFile);
    }
    _GData_ExecuteVideoLinear(pMode);
}

static void _GData_SystemRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;
    AMBA_IK_STITCH_INFO_s StitchInfo = {0};
    AMBA_IK_WARP_BUFFER_INFO_s WarpBufInfo = {0};
    UINT32 FlipInfo;

    rval = AmbaIK_GetFlipMode(pMode, &FlipInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFlipMode", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    fprintf(pFile, "FlipInfo = %d\r\n", FlipInfo);

    rval = AmbaIK_GetStitchingInfo(pMode, &StitchInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetStitchingInfo", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    fprintf(pFile, "StitchInfo.Enable = %d\r\n", StitchInfo.Enable);
    fprintf(pFile, "StitchInfo.TileNum_x = %d\r\n", StitchInfo.TileNum_x);
    fprintf(pFile, "StitchInfo.TileNum_y = %d\r\n", StitchInfo.TileNum_y);

    rval = AmbaIK_GetWarpBufferInfo(pMode, &WarpBufInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetWarpBufferInfo", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    fprintf(pFile, "WarpBufInfo.DramEfficiency = %d\r\n", WarpBufInfo.DramEfficiency);
    fprintf(pFile, "WarpBufInfo.LumaWaitLines = %d\r\n", WarpBufInfo.LumaWaitLines);
    fprintf(pFile, "WarpBufInfo.LumaDmaSize = %d\r\n", WarpBufInfo.LumaDmaSize);
    fprintf(pFile, "WarpBufInfo.Reserved = %d\r\n", WarpBufInfo.Reserved); // chroma_dma
}

static void _GData_VinSensorRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_GetVinSensorInfo(pMode, &GDataGet.SensorInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetVinSensorInfo", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
}

static void _GData_RgbIrRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_GetRgbIr(pMode, &GDataGet.RgbIr);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetRgbIr", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
}

static void _GData_Raw2RawRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_GetFeToneCurve(pMode, &GDataGet.FeToneCurve.Decompand);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFeToneCurve", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetFeStaticBlc(pMode, &GDataGet.StaticBlackLevel[0], 0UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFeStaticBlc", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetDynamicBadPxlCor(pMode, &GDataGet.DynamicBadPixelCorrection);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetDynamicBadPxlCor", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetCfaLeakageFilter(pMode, &GDataGet.CfaLeakageFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetCfaLeakageFilter", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetCfaNoiseFilter(pMode, &GDataGet.CfaNoiseFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetCfaNoiseFilter", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetAntiAliasing(pMode, &GDataGet.AntiAliasing);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetAntiAliasing", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetFeWbGain(pMode, &GDataGet.FrontEndWbGain[0], 0UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFeWbGain", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetAfterCeWbGain(pMode, &GDataGet.AfterCeWbGain);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetAfterCeWbGain", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetBeforeCeWbGain(pMode, &GDataGet.BeforeCeWbGain);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetBeforeCeWbGain", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetRgbTo12Y(pMode, &GDataGet.RgbTo12Y);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetRgbTo12Y", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetDemosaic(pMode, &GDataGet.DemosaicFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetDemosaic", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetGrgbMismatch(pMode, &GDataGet.GbGrMismatch);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetGrgbMismatch", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
}

static void _GData_Yuv2YuvRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_GetColorCorrection(pMode, &GDataGet.CcThreeD);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetColorCorrection", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);

    GDataGet.ColorCorrection.MatrixEn = GDataGet.CcThreeD.MatrixEn;
    GDataGet.ColorCorrection.MatrixShiftMinus8 = GDataGet.CcThreeD.MatrixShiftMinus8;
    memcpy(&GDataGet.ColorCorrection.Matrix[0], &GDataGet.CcThreeD.Matrix[0], sizeof(UINT32)*6UL);

    rval = AmbaIK_GetToneCurve(pMode, &GDataGet.ToneCurve);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetToneCurve", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetRgbToYuvMatrix(pMode, &GDataGet.RgbToYuvMatrix);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetRgbToYuvMatrix", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetChromaScale(pMode, &GDataGet.ChromaScale);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetChromaScale", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetChromaMedianFilter(pMode, &GDataGet.ChromaMedianFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetChromaMedianFilter", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetChromaFilter(pMode, &GDataGet.ChromaFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetChromaFilter", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetWideChromaFilter(pMode, &GDataGet.WideChromaFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetWideChromaFilter", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetWideChromaFilterCombine(pMode, &GDataGet.WideChromaFilterCombine);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetWideChromaFilterCombine", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetLumaNoiseReduction(pMode, &GDataGet.LumaNoiseReduction);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetLumaNoiseReduction", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetFirstLumaProcMode(pMode, &GDataGet.FirstLumaProcessMode);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFirstLumaProcMode", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);

    rval = AmbaIK_GetFstShpNsBoth(pMode, &GDataGet.SharpenBoth.BothInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFstShpNsBoth", "OK case in ik_ctest_vid8_ldly_tile2x4.c.c", id_cnt++);
    rval = AmbaIK_GetFstShpNsNoise(pMode, &GDataGet.SharpenNoise);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFstShpNsNoise", "OK case in ik_ctest_vid8_ldly_tile2x4.c.c", id_cnt++);
    rval = AmbaIK_GetFstShpNsFir(pMode, &GDataGet.Fir);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFstShpNsFir", "OK case in ik_ctest_vid8_ldly_tile2x4.c.c", id_cnt++);
    rval = AmbaIK_GetFstShpNsCoring(pMode, &GDataGet.Coring);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFstShpNsCoring", "OK case in ik_ctest_vid8_ldly_tile2x4.c.c", id_cnt++);
    rval = AmbaIK_GetFstShpNsCorIdxScl(pMode, &GDataGet.CoringIndexScale);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFstShpNsCorIdxScl", "OK case in ik_ctest_vid8_ldly_tile2x4.c.c", id_cnt++);
    rval = AmbaIK_GetFstShpNsMinCorRst(pMode, &GDataGet.MinCoringResult);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFstShpNsMinCorRst", "OK case in ik_ctest_vid8_ldly_tile2x4.c.c", id_cnt++);
    rval = AmbaIK_GetFstShpNsMaxCorRst(pMode, &GDataGet.MaxCoringResult);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFstShpNsMaxCorRst", "OK case in ik_ctest_vid8_ldly_tile2x4.c.c", id_cnt++);
    rval = AmbaIK_GetFstShpNsSclCor(pMode, &GDataGet.ScaleCoring);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFstShpNsSclCor", "OK case in ik_ctest_vid8_ldly_tile2x4.c.c", id_cnt++);
}

static void _GData_MctfRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_GetFnlShpNsBoth(pMode, &GDataGet.FinalSharpenBoth.BothInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsBoth", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetFnlShpNsBothTdt(pMode, &GDataGet.FinalSharpenBoth.ThreedTable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsBothTdt", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetFnlShpNsNoise(pMode, &GDataGet.FinalSharpenNoise);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsNoise", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetFnlShpNsFir(pMode, &GDataGet.FinalFir);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsFir", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetFnlShpNsCoring(pMode, &GDataGet.FinalCoring);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsCoring", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetFnlShpNsCorIdxScl(pMode, &GDataGet.FinalCoringIndexScale);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsCorIdxScl", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetFnlShpNsMinCorRst(pMode, &GDataGet.FinalMinCoringResult);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsMinCorRst", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetFnlShpNsMaxCorRst(pMode, &GDataGet.FinalMaxCoringResult);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsMaxCorRst", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetFnlShpNsSclCor(pMode, &GDataGet.FinalScaleCoring);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsSclCor", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetVideoMctf(pMode, &GDataGet.VideoMctf);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetVideoMctf", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetVideoMctfTa(pMode, &GDataGet.VideoMctfTemporalAdjust);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetVideoMctfTa", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval = AmbaIK_GetVideoMctfAndFnlshp(pMode, &GDataGet.VideoMctfAndFinalSharpen);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetVideoMctfAndFnlshp", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
}

static void _GData_Raw2yuvCalibRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;
    UINT32 enable = 0;

    rval= AmbaIK_GetVinActiveWin(pMode, &GDataGet.Active);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetVinActiveWin", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_GetDummyMarginRange(pMode, &GDataGet.Dummy);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetDummyMarginRange", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_GetDzoomInfo(pMode, &GDataGet.Dzoom);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetDzoomInfo", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    rval= AmbaIK_GetWarpInfo(pMode, &GDataGet.WarpCompensationInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetWarpInfo", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);

    rval = AmbaIK_GetWarpEnb(pMode, &enable);
    fprintf(pFile, "WarpEnb = %d\r\n", enable);
    memset(&GDataGet.WarpCompensation, 0x0, sizeof(ITUNER_WARP_s));

    rval= AmbaIK_GetCawarpInfo(pMode, &GDataGet.CaWarpCompensationInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetCawarpInfo", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);

    rval = AmbaIK_GetCawarpEnb(pMode, &enable);
    fprintf(pFile, "CawarpEnb = %d\r\n", enable);
    memset(&GDataGet.ChromaAberrationInfo, 0x0, sizeof(ITUNER_CHROMA_ABERRATION_s));

    rval= AmbaIK_GetVignette(pMode, &GDataGet.VignetteCompensation.VignetteInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetVignette", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);

    rval = AmbaIK_GetVignetteEnb(pMode, &enable);
    fprintf(pFile, "VignetteEnb = %d\r\n", enable);
    memset(&GDataGet.VignetteCompensation, 0x0, sizeof(ITUNER_VIGNETTE_s));

    rval= AmbaIK_GetStaticBadPxlCor(pMode, &GDataGet.SbpCorr);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetStaticBadPxlCor", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);

    rval = AmbaIK_GetStaticBadPxlCorEnb(pMode, &enable);
    fprintf(pFile, "StaticBadPxlCorEnb = %d\r\n", enable);
    memset(&GDataGet.StaticBadPixelCorrection, 0x0, sizeof(ITUNER_FPN_s));
}

static void _GData_RefreshVideoLinear(const AMBA_IK_MODE_CFG_s *pMode)
{
    _GData_SystemRefresh(pMode);
    _GData_VinSensorRefresh(pMode);
    _GData_RgbIrRefresh(pMode);
    _GData_Raw2RawRefresh(pMode);
    _GData_Yuv2YuvRefresh(pMode);
    _GData_MctfRefresh(pMode);
    _GData_Raw2yuvCalibRefresh(pMode);
}

static void _GData_Refresh(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_ABILITY_s *pAbility)
{
    AMBA_IK_ABILITY_s Ability = {0};
    UINT32 rval = 0;
    FILE *pBinFile;

    rval = AmbaIK_GetContextAbility(pMode, &Ability);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_GetContextAbility", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);
    fprintf(pFile, "Ability.Pipe = %d, Ability.StillPipe = %d, Ability.VideoPipe = %d\r\n",
            Ability.Pipe, Ability.StillPipe, Ability.VideoPipe);

    _GData_RefreshVideoLinear(pMode);

    {
        pBinFile = fopen("data/ik_ctest_vid8_ldly_tile2x4_gdata_get.bin", "wb");

        fwrite(&GDataGet, 1, sizeof(ITUNER_DATA_s), pBinFile);
        fclose(pBinFile);
    }
}

INT32 ik_ctest_vid8_ldly_tile2x4_main(void)
{
    UINT32 Rval = IK_OK;
    AMBA_IK_ABILITY_s Ability = {0};
    AMBA_IK_MODE_CFG_s Mode;
    AMBA_IK_EXECUTE_CONTAINER_s ExecuteContainer = {0};

    memset(ik_working_buffer, 0, sizeof(ik_working_buffer));
    memset(&GData, 0, sizeof(ITUNER_DATA_s));
    memset(&GDataGet, 0, sizeof(ITUNER_DATA_s));
    memset(&_System, 0, sizeof(AMBA_ITN_SYSTEM_s));
    id_cnt = 0;

    pFile = fopen("data/ik_ctest_vid8_ldly_tile2x4.txt", "w");

    // init IK
    _load_idsp_default_binary();
    Ability.Pipe = AMBA_IK_PIPE_VIDEO;
    Ability.VideoPipe = AMBA_IK_VIDEO_LINEAR;

    _Init_Image_Kernel(&Ability);
    _Init_Image_Kernel_Core();

    Mode.ContextId = 0;
    Rval |= AmbaIK_InitContext(&Mode, &Ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitContext", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);

    // test set
    _GData_Execute(&Mode, &Ability);

    Rval |= AmbaIK_ExecuteConfig(&Mode, &ExecuteContainer);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);

    {
        SIZE_t size;
        UINT32 size_align;
        void *buffer, *data;
        IDSP_FLOW_HEADER_s *p_header;
        Rval |= AmbaIK_QueryDebugConfigSize(&size);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_QueryDebugConfigSize", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);

        size_align = size + 128; // extra 128 bytes for cr buf alignment
        buffer = (void*)malloc(size_align);
        data = (void *) ALIGN_N((uintptr)buffer, 128);
        Rval |= AmbaIK_DumpDebugConfig(&Mode, 0, data, size);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_DumpDebugConfig", "OK case in ik_ctest_vid8_ldly_tile2x4.c", id_cnt++);

        p_header = (IDSP_FLOW_HEADER_s *)data;
        p_header->IdspFlowAddr = 0;

        {
            //write ikc
            FILE *fid;

            fid = fopen("data/ik_ctest_vid8_ldly_tile2x4.bin", "wb");
            fwrite(data, 1, sizeof(amalgam_flow_data_t), fid);
            fclose(fid);
        }
        free(buffer);
    }

    // test get
    _GData_Refresh(&Mode, &Ability);

    free((UINT8 *) pBinDataAddr);
    pBinDataAddr = NULL;
    fclose(pFile);

    fake_destroy_mutex(NULL);
    fake_clean_cache(NULL, 0);

    img_arch_deinit_architecture();
    iks_arch_deinit_architecture();

    return Rval;
}

