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
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_QueryArchMemorySize", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_InitArch(&ContextSetting, (void*)ik_working_buffer, sizeof(ik_working_buffer));
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitArch", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_InitDefBinary(pBinDataAddr);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitDefBinary", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_InitCheckParamFunc();
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitCheckParamFunc", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetDebugLogId(255U, 1U);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_SetDebugLogId", "OK case in ik_ctest_hdr20.c", id_cnt++);
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
    UINT32 FlipInfo;
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
    _System.NumberOfExposures = 3;
    _System.Ability = AMBA_IK_VIDEO_HDR_EXPO_3;

    rval = AmbaIK_GetWindowSizeInfo(pMode, &WindowSizeInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetWindowSizeInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
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
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWindowSizeInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);

    // Update Flip Info
    FlipInfo = 0UL;
    rval = AmbaIK_SetFlipMode(pMode, FlipInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFlipMode", "OK case in ik_ctest_hdr20.c", id_cnt++);

    // make img_exe_execute_filter_update_check() happy...
    StitchInfo.Enable = 0;
    rval = AmbaIK_SetStitchingInfo(pMode, &StitchInfo);
    WarpBufInfo.LumaWaitLines = 8;
    WarpBufInfo.LumaDmaSize = 32;
    WarpBufInfo.Reserved = 32; // chroma_dma

    rval = AmbaIK_SetWarpBufferInfo(pMode, &WarpBufInfo);

    // go through 3A to make them covered
#if 1
    {
        AMBA_IK_AWB_STAT_INFO_s AwbStatInfo = {0};
        AMBA_IK_AE_STAT_INFO_s AeStatInfo = {0};
        AMBA_IK_AF_STAT_INFO_s AfStatInfo = {0};
        AMBA_IK_AF_STAT_INFO_s PgAfStatInfo = {0};
        AMBA_IK_AF_STAT_EX_INFO_s AfStatExInfo = {0};
        AMBA_IK_PG_AF_STAT_EX_INFO_s PgAfStatExInfo = {0};
        AMBA_IK_HISTOGRAM_INFO_s HistInfo = {0};
        AMBA_IK_HISTOGRAM_INFO_s PgHistInfo = {0};

        AwbStatInfo.AwbTileNumCol = 64UL;
        AwbStatInfo.AwbTileNumRow = 64UL;
        AwbStatInfo.AwbTileColStart = 0UL;
        AwbStatInfo.AwbTileRowStart = 0UL;
        AwbStatInfo.AwbTileWidth = 64UL;
        AwbStatInfo.AwbTileHeight = 64UL;
        AwbStatInfo.AwbTileActiveWidth = 64UL;
        AwbStatInfo.AwbTileActiveHeight = 64UL;
        AwbStatInfo.AwbPixMinValue = 0UL;
        AwbStatInfo.AwbPixMaxValue = 16383UL;
        rval = AmbaIK_SetAwbStatInfo(pMode, &AwbStatInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAwbStatInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);

        AeStatInfo.AeTileNumCol = 24UL;
        AeStatInfo.AeTileNumRow = 16UL;
        AeStatInfo.AeTileColStart = 0UL;
        AeStatInfo.AeTileRowStart = 0UL;
        AeStatInfo.AeTileWidth = 170UL;
        AeStatInfo.AeTileHeight = 256UL;
        AeStatInfo.AePixMinValue = 0UL;
        AeStatInfo.AePixMaxValue = 16383UL;
        rval = AmbaIK_SetAeStatInfo(pMode, &AeStatInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAeStatInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);

        AfStatInfo.AfTileNumCol = 24UL;
        AfStatInfo.AfTileNumRow = 16UL;
        AfStatInfo.AfTileColStart = 0UL;
        AfStatInfo.AfTileRowStart = 0UL;
        AfStatInfo.AfTileWidth = 170UL;
        AfStatInfo.AfTileHeight = 256UL;
        AfStatInfo.AfTileActiveWidth = 170UL;
        AfStatInfo.AfTileActiveHeight = 256UL;
        rval = AmbaIK_SetAfStatInfo(pMode, &AfStatInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAfStatInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);

        PgAfStatInfo.AfTileNumCol = 32UL;
        PgAfStatInfo.AfTileNumRow = 16UL;
        PgAfStatInfo.AfTileColStart = 0UL;
        PgAfStatInfo.AfTileRowStart = 0UL;
        PgAfStatInfo.AfTileWidth = 128UL;
        PgAfStatInfo.AfTileHeight = 256UL;
        PgAfStatInfo.AfTileActiveWidth = 128UL;
        PgAfStatInfo.AfTileActiveHeight = 256UL;
        rval = AmbaIK_SetPgAfStatInfo(pMode, &PgAfStatInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPgAfStatInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);

        AfStatExInfo.AfHorizontalFilter1Stage1Enb = 1UL;
        AfStatExInfo.AfHorizontalFilter1Stage2Enb = 1UL;
        AfStatExInfo.AfHorizontalFilter1Stage3Enb = 1UL;
        AfStatExInfo.AfHorizontalFilter2Stage1Enb = 1UL;
        AfStatExInfo.AfHorizontalFilter2Stage2Enb = 1UL;
        AfStatExInfo.AfHorizontalFilter2Stage3Enb = 1UL;
        AfStatExInfo.AfHorizontalFilter2Thresh = 50UL;
        AfStatExInfo.AfTileFv1HorizontalShift = 8UL;
        AfStatExInfo.AfTileFv2HorizontalShift = 8UL;
        AfStatExInfo.AfHorizontalFilter1Gain[0] = 188;
        AfStatExInfo.AfHorizontalFilter1Gain[1] = 467;
        AfStatExInfo.AfHorizontalFilter1Gain[2] = -235;
        AfStatExInfo.AfHorizontalFilter1Gain[3] = 375;
        AfStatExInfo.AfHorizontalFilter1Gain[4] = -184;
        AfStatExInfo.AfHorizontalFilter1Gain[5] = 276;
        AfStatExInfo.AfHorizontalFilter1Gain[6] = -206;

        AfStatExInfo.AfHorizontalFilter1Shift[0] = 7;
        AfStatExInfo.AfHorizontalFilter1Shift[1] = 2;
        AfStatExInfo.AfHorizontalFilter1Shift[2] = 2;
        AfStatExInfo.AfHorizontalFilter1Shift[3] = 0;

        AfStatExInfo.AfHorizontalFilter2Gain[0] = 188;
        AfStatExInfo.AfHorizontalFilter2Gain[1] = 467;
        AfStatExInfo.AfHorizontalFilter2Gain[2] = -235;
        AfStatExInfo.AfHorizontalFilter2Gain[3] = 375;
        AfStatExInfo.AfHorizontalFilter2Gain[4] = -184;
        AfStatExInfo.AfHorizontalFilter2Gain[5] = 276;
        AfStatExInfo.AfHorizontalFilter2Gain[6] = -206;

        AfStatExInfo.AfHorizontalFilter2Shift[0] = 7;
        AfStatExInfo.AfHorizontalFilter2Shift[1] = 2;
        AfStatExInfo.AfHorizontalFilter2Shift[2] = 2;
        AfStatExInfo.AfHorizontalFilter2Shift[3] = 0;
        rval = AmbaIK_SetAfStatExInfo(pMode, &AfStatExInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAfStatExInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);

        PgAfStatExInfo.AfHorizontalFilter1Stage1Enb = 1UL;
        PgAfStatExInfo.AfHorizontalFilter1Stage2Enb = 1UL;
        PgAfStatExInfo.AfHorizontalFilter1Stage3Enb = 1UL;
        PgAfStatExInfo.AfHorizontalFilter2Stage1Enb = 1UL;
        PgAfStatExInfo.AfHorizontalFilter2Stage2Enb = 1UL;
        PgAfStatExInfo.AfHorizontalFilter2Stage3Enb = 1UL;
        PgAfStatExInfo.AfVerticalFilter1Thresh = 100UL;
        PgAfStatExInfo.AfTileFv1HorizontalShift = 7UL;
        PgAfStatExInfo.AfTileFv1HorizontalWeight = 255UL;
        PgAfStatExInfo.AfVerticalFilter2Thresh = 100UL;
        PgAfStatExInfo.AfTileFv2HorizontalShift = 7UL;
        PgAfStatExInfo.AfTileFv2HorizontalWeight = 255UL;
        PgAfStatExInfo.AfHorizontalFilter1Gain[0] = 188;
        PgAfStatExInfo.AfHorizontalFilter1Gain[1] = 467;
        PgAfStatExInfo.AfHorizontalFilter1Gain[2] = -235;
        PgAfStatExInfo.AfHorizontalFilter1Gain[3] = 375;
        PgAfStatExInfo.AfHorizontalFilter1Gain[4] = -184;
        PgAfStatExInfo.AfHorizontalFilter1Gain[5] = 276;
        PgAfStatExInfo.AfHorizontalFilter1Gain[6] = -206;

        PgAfStatExInfo.AfHorizontalFilter1Shift[0] = 7;
        PgAfStatExInfo.AfHorizontalFilter1Shift[1] = 2;
        PgAfStatExInfo.AfHorizontalFilter1Shift[2] = 2;
        PgAfStatExInfo.AfHorizontalFilter1Shift[3] = 0;

        PgAfStatExInfo.AfHorizontalFilter2Gain[0] = 188;
        PgAfStatExInfo.AfHorizontalFilter2Gain[1] = 467;
        PgAfStatExInfo.AfHorizontalFilter2Gain[2] = -235;
        PgAfStatExInfo.AfHorizontalFilter2Gain[3] = 375;
        PgAfStatExInfo.AfHorizontalFilter2Gain[4] = -184;
        PgAfStatExInfo.AfHorizontalFilter2Gain[5] = 276;
        PgAfStatExInfo.AfHorizontalFilter2Gain[6] = -206;

        PgAfStatExInfo.AfHorizontalFilter2Shift[0] = 7;
        PgAfStatExInfo.AfHorizontalFilter2Shift[1] = 2;
        PgAfStatExInfo.AfHorizontalFilter2Shift[2] = 2;
        PgAfStatExInfo.AfHorizontalFilter2Shift[3] = 0;
        rval = AmbaIK_SetPgAfStatExInfo(pMode, &PgAfStatExInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPgAfStatExInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);

        HistInfo.AeTileMask[0] = 0xFFFFFF;
        HistInfo.AeTileMask[1] = 0xFFFFFF;
        HistInfo.AeTileMask[2] = 0xFFFFFF;
        HistInfo.AeTileMask[3] = 0xFFFFFF;
        HistInfo.AeTileMask[4] = 0xFFFFFF;
        HistInfo.AeTileMask[5] = 0xFFFFFF;
        HistInfo.AeTileMask[6] = 0xFFFFFF;
        HistInfo.AeTileMask[7] = 0xFFFFFF;
        HistInfo.AeTileMask[8] = 0xFFFFFF;
        HistInfo.AeTileMask[9] = 0xFFFFFF;
        HistInfo.AeTileMask[10] = 0xFFFFFF;
        HistInfo.AeTileMask[11] = 0xFFFFFF;
        HistInfo.AeTileMask[12] = 0xFFFFFF;
        HistInfo.AeTileMask[13] = 0xFFFFFF;
        HistInfo.AeTileMask[14] = 0xFFFFFF;
        HistInfo.AeTileMask[15] = 0xFFFFFF;
        rval = AmbaIK_SetHistogramInfo(pMode, &HistInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetHistogramInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);

        PgHistInfo.AeTileMask[0] = 0xFFFFFF;
        PgHistInfo.AeTileMask[1] = 0xFFFFFF;
        PgHistInfo.AeTileMask[2] = 0xFFFFFF;
        PgHistInfo.AeTileMask[3] = 0xFFFFFF;
        PgHistInfo.AeTileMask[4] = 0xFFFFFF;
        PgHistInfo.AeTileMask[5] = 0xFFFFFF;
        PgHistInfo.AeTileMask[6] = 0xFFFFFF;
        PgHistInfo.AeTileMask[7] = 0xFFFFFF;
        PgHistInfo.AeTileMask[8] = 0xFFFFFF;
        PgHistInfo.AeTileMask[9] = 0xFFFFFF;
        PgHistInfo.AeTileMask[10] = 0xFFFFFF;
        PgHistInfo.AeTileMask[11] = 0xFFFFFF;
        PgHistInfo.AeTileMask[12] = 0xFFFFFF;
        PgHistInfo.AeTileMask[13] = 0xFFFFFF;
        PgHistInfo.AeTileMask[14] = 0xFFFFFF;
        PgHistInfo.AeTileMask[15] = 0xFFFFFF;
        rval = AmbaIK_SetPgHistogramInfo(pMode, &PgHistInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPgHistogramInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
    }

    {
        // resamp strength
        AMBA_IK_RESAMPLER_STR_s ResamplerStr = {0};

        rval = AmbaIK_SetResampStrength(pMode, &ResamplerStr);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetResampStrength", "OK case in ik_ctest_hdr20.c", id_cnt++);
    }

    {
        // safety info
        AMBA_IK_SAFETY_INFO_s SafetyInfo;

        SafetyInfo.UpdateInterval = 1;
        rval = AmbaIK_SetSafetyInfo(pMode, &SafetyInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_SetSafetyInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
    }
#endif
}

static void _GData_VinSensorExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0u;

    rval= AmbaIK_GetVinSensorInfo(pMode, &GData.SensorInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetVinSensorInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
    GData.SensorInfo.SensorResolution = _System.RawResolution;
    GData.SensorInfo.SensorPattern = _System.RawBayer;
    GData.SensorInfo.SensorReadoutMode = _System.SensorReadoutMode;
    GData.SensorInfo.Compression = _System.CompressedRaw;
    GData.SensorInfo.SensorMode = _System.SensorMode;
    GData.SensorInfo.CompressionOffset = _System.CompressionOffset;
    rval= AmbaIK_SetVinSensorInfo(pMode, &GData.SensorInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVinSensorInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
}

static void _GData_RgbIrExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval= AmbaIK_SetRgbIr(pMode, &GData.RgbIr);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbIr", "OK case in ik_ctest_hdr20.c", id_cnt++);
}

static void _GData_Raw2RawExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_SetFeToneCurve(pMode, &GData.FeToneCurve.Decompand);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeToneCurve", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetFeStaticBlc(pMode, &GData.StaticBlackLevel[0], 0UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeStaticBlc", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetDynamicBadPxlCor(pMode, &GData.DynamicBadPixelCorrection);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDynamicBadPxlCor", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetCfaLeakageFilter(pMode, &GData.CfaLeakageFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCfaLeakageFilter", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetCfaNoiseFilter(pMode, &GData.CfaNoiseFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCfaNoiseFilter", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetAntiAliasing(pMode, &GData.AntiAliasing);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAntiAliasing", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetFeWbGain(pMode, &GData.FrontEndWbGain[0], 0UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeWbGain", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetAfterCeWbGain(pMode, &GData.AfterCeWbGain);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAfterCeWbGain", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetBeforeCeWbGain(pMode, &GData.BeforeCeWbGain);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetBeforeCeWbGain", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetDemosaic(pMode, &GData.DemosaicFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDemosaic", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetGrgbMismatch(pMode, &GData.GbGrMismatch);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetGrgbMismatch", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetRgbTo12Y(pMode, &GData.RgbTo12Y);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbTo12Y", "OK case in ik_ctest_hdr20.c", id_cnt++);
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
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetColorCorrection", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetToneCurve(pMode, &GData.ToneCurve);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetToneCurve", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetRgbToYuvMatrix(pMode, &GData.RgbToYuvMatrix);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbToYuvMatrix", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetChromaScale(pMode, &GData.ChromaScale);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaScale", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetChromaMedianFilter(pMode, &GData.ChromaMedianFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaMedianFilter", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetChromaFilter(pMode, &GData.ChromaFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaFilter", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetWideChromaFilter(pMode, &GData.WideChromaFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWideChromaFilter", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetWideChromaFilterCombine(pMode, &GData.WideChromaFilterCombine);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWideChromaFilterCombine", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetLumaNoiseReduction(pMode, &GData.LumaNoiseReduction);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetLumaNoiseReduction", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetFirstLumaProcMode(pMode, &GData.FirstLumaProcessMode);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFirstLumaProcMode", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetAdvSpatFltr(pMode, &GData.AsfInfo.AsfInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAdvSpatFltr", "OK case in ik_ctest_hdr20.c", id_cnt++);
}

static void _GData_MctfExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_SetFnlShpNsBoth(pMode, &GData.FinalSharpenBoth.BothInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsBoth", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsBothTdt(pMode, &GData.FinalSharpenBoth.ThreedTable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsBothTdt", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsNoise(pMode, &GData.FinalSharpenNoise);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsNoise", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsFir(pMode, &GData.FinalFir);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsFir", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsCoring(pMode, &GData.FinalCoring);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsCoring", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsCorIdxScl(pMode, &GData.FinalCoringIndexScale);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsCorIdxScl", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsMinCorRst(pMode, &GData.FinalMinCoringResult);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsMinCorRst", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsMaxCorRst(pMode, &GData.FinalMaxCoringResult);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsMaxCorRst", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetFnlShpNsSclCor(pMode, &GData.FinalScaleCoring);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsSclCor", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetVideoMctf(pMode, &GData.VideoMctf);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctf", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetVideoMctfTa(pMode, &GData.VideoMctfTemporalAdjust);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctfTa", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetVideoMctfAndFnlshp(pMode, &GData.VideoMctfAndFinalSharpen);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctfAndFnlshp", "OK case in ik_ctest_hdr20.c", id_cnt++);
}

static void _GData_Raw2yuvCalibExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0u;

    rval= AmbaIK_SetVinActiveWin(pMode, &GData.Active);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVinActiveWin", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetDummyMarginRange(pMode, &GData.Dummy);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDummyMarginRange", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetDzoomInfo(pMode, &GData.Dzoom);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDzoomInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetStaticBadPxlCorEnb(pMode, GData.StaticBadPixelCorrection.Enable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetStaticBadPxlCorEnb", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetVignetteEnb(pMode, GData.VignetteCompensation.Enable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVignetteEnb", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetCawarpEnb(pMode, (UINT32)GData.ChromaAberrationInfo.Enable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCawarpEnb", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_SetWarpEnb(pMode, 0);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWarpEnb", "OK case in ik_ctest_hdr20.c", id_cnt++);
}

static void _GData_CeExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0u;

    rval = AmbaIK_SetCe(pMode, &GData.ContrastEnhance.ContrastEnhance);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCe", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetCeInputTable(pMode, &GData.ContrastEnhance.ContrastEnhanceInputTable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCeInputTable", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetCeOutputTable(pMode, &GData.ContrastEnhance.ContrastEnhanceOutTable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCeOutputTable", "OK case in ik_ctest_hdr20.c", id_cnt++);
}

static void _GData_VideoHdrExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_SetHdrBlend(pMode, &GData.VideoHdr.HdrBlend);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetHdrBlend", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetHdrRawOffset(pMode, &GData.VideoHdr.RawInfo.Offset);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetHdrRawOffset", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetFeWbGain(pMode, &GData.FrontEndWbGain[1], 1UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeWbGain", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetFeWbGain(pMode, &GData.FrontEndWbGain[2], 2UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeWbGain", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetFeStaticBlc(pMode, &GData.StaticBlackLevel[1], 1UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeStaticBlc", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_SetFeStaticBlc(pMode, &GData.StaticBlackLevel[2], 2UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeStaticBlc", "OK case in ik_ctest_hdr20.c", id_cnt++);
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
        pBinFile = fopen("../util/bin/hdr20_gdata.bin", "rb");

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

static void _GData_SystemRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;
    UINT32 FlipInfo;

    rval = AmbaIK_GetFlipMode(pMode, &FlipInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFlipMode", "OK case in ik_ctest_hdr20.c", id_cnt++);
    fprintf(pFile, "FlipInfo = %d\r\n", FlipInfo);

    // go through 3A to make them covered
#if 1
    {
        AMBA_IK_AWB_STAT_INFO_s AwbStatInfo = {0};
        AMBA_IK_AE_STAT_INFO_s AeStatInfo = {0};
        AMBA_IK_AF_STAT_INFO_s AfStatInfo = {0};
        AMBA_IK_AF_STAT_INFO_s PgAfStatInfo = {0};
        AMBA_IK_AF_STAT_EX_INFO_s AfStatExInfo = {0};
        AMBA_IK_PG_AF_STAT_EX_INFO_s PgAfStatExInfo = {0};
        AMBA_IK_HISTOGRAM_INFO_s HistInfo = {0};
        AMBA_IK_HISTOGRAM_INFO_s PgHistInfo = {0};

        rval = AmbaIK_GetAwbStatInfo(pMode, &AwbStatInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetAwbStatInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
        fprintf(pFile, "AwbStatInfo.AwbTileNumCol = %d\r\n", AwbStatInfo.AwbTileNumCol);
        fprintf(pFile, "AwbStatInfo.AwbTileNumRow = %d\r\n", AwbStatInfo.AwbTileNumRow);
        fprintf(pFile, "AwbStatInfo.AwbTileColStart = %d\r\n", AwbStatInfo.AwbTileColStart);
        fprintf(pFile, "AwbStatInfo.AwbTileRowStart = %d\r\n", AwbStatInfo.AwbTileRowStart);
        fprintf(pFile, "AwbStatInfo.AwbTileWidth = %d\r\n", AwbStatInfo.AwbTileWidth);
        fprintf(pFile, "AwbStatInfo.AwbTileHeight = %d\r\n", AwbStatInfo.AwbTileHeight);
        fprintf(pFile, "AwbStatInfo.AwbTileActiveWidth = %d\r\n", AwbStatInfo.AwbTileActiveWidth);
        fprintf(pFile, "AwbStatInfo.AwbTileActiveHeight = %d\r\n", AwbStatInfo.AwbTileActiveHeight);
        fprintf(pFile, "AwbStatInfo.AwbPixMinValue = %d\r\n", AwbStatInfo.AwbPixMinValue);
        fprintf(pFile, "AwbStatInfo.AwbPixMaxValue = %d\r\n", AwbStatInfo.AwbPixMaxValue);

        rval = AmbaIK_GetAeStatInfo(pMode, &AeStatInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetAeStatInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
        fprintf(pFile, "AeStatInfo.AeTileNumCol = %d\r\n", AeStatInfo.AeTileNumCol);
        fprintf(pFile, "AeStatInfo.AeTileNumRow = %d\r\n", AeStatInfo.AeTileNumRow);
        fprintf(pFile, "AeStatInfo.AeTileColStart = %d\r\n", AeStatInfo.AeTileColStart);
        fprintf(pFile, "AeStatInfo.AeTileRowStart = %d\r\n", AeStatInfo.AeTileRowStart);
        fprintf(pFile, "AeStatInfo.AeTileWidth = %d\r\n", AeStatInfo.AeTileWidth);
        fprintf(pFile, "AeStatInfo.AeTileHeight = %d\r\n", AeStatInfo.AeTileHeight);
        fprintf(pFile, "AeStatInfo.AePixMinValue = %d\r\n", AeStatInfo.AePixMinValue);
        fprintf(pFile, "AeStatInfo.AePixMaxValue = %d\r\n", AeStatInfo.AePixMaxValue);

        rval = AmbaIK_GetAfStatInfo(pMode, &AfStatInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetAfStatInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
        fprintf(pFile, "AfStatInfo.AfTileNumCol = %d\r\n", AfStatInfo.AfTileNumCol);
        fprintf(pFile, "AfStatInfo.AfTileNumRow = %d\r\n", AfStatInfo.AfTileNumRow);
        fprintf(pFile, "AfStatInfo.AfTileColStart = %d\r\n", AfStatInfo.AfTileColStart);
        fprintf(pFile, "AfStatInfo.AfTileRowStart = %d\r\n", AfStatInfo.AfTileRowStart);
        fprintf(pFile, "AfStatInfo.AfTileWidth = %d\r\n", AfStatInfo.AfTileWidth);
        fprintf(pFile, "AfStatInfo.AfTileHeight = %d\r\n", AfStatInfo.AfTileHeight);
        fprintf(pFile, "AfStatInfo.AfTileActiveWidth = %d\r\n", AfStatInfo.AfTileActiveWidth);
        fprintf(pFile, "AfStatInfo.AfTileActiveHeight = %d\r\n", AfStatInfo.AfTileActiveHeight);

        rval = AmbaIK_GetPgAfStatInfo(pMode, &PgAfStatInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetPgAfStatInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
        fprintf(pFile, "PgAfStatInfo.AfTileNumCol = %d\r\n", PgAfStatInfo.AfTileNumCol);
        fprintf(pFile, "PgAfStatInfo.AfTileNumRow = %d\r\n", PgAfStatInfo.AfTileNumRow);
        fprintf(pFile, "PgAfStatInfo.AfTileColStart = %d\r\n", PgAfStatInfo.AfTileColStart);
        fprintf(pFile, "PgAfStatInfo.AfTileRowStart = %d\r\n", PgAfStatInfo.AfTileRowStart);
        fprintf(pFile, "PgAfStatInfo.AfTileWidth = %d\r\n", PgAfStatInfo.AfTileWidth);
        fprintf(pFile, "PgAfStatInfo.AfTileHeight = %d\r\n", PgAfStatInfo.AfTileHeight);
        fprintf(pFile, "PgAfStatInfo.AfTileActiveWidth = %d\r\n", PgAfStatInfo.AfTileActiveWidth);
        fprintf(pFile, "PgAfStatInfo.AfTileActiveHeight = %d\r\n", PgAfStatInfo.AfTileActiveHeight);

        rval = AmbaIK_GetAfStatExInfo(pMode, &AfStatExInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetAfStatExInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter1Stage1Enb = %d\r\n", AfStatExInfo.AfHorizontalFilter1Stage1Enb);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter1Stage2Enb = %d\r\n", AfStatExInfo.AfHorizontalFilter1Stage2Enb);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter1Stage3Enb = %d\r\n", AfStatExInfo.AfHorizontalFilter1Stage3Enb);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter2Stage1Enb = %d\r\n", AfStatExInfo.AfHorizontalFilter2Stage1Enb);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter2Stage2Enb = %d\r\n", AfStatExInfo.AfHorizontalFilter2Stage2Enb);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter2Stage3Enb = %d\r\n", AfStatExInfo.AfHorizontalFilter2Stage3Enb);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter2Thresh = %d\r\n", AfStatExInfo.AfHorizontalFilter2Thresh);
        fprintf(pFile, "AfStatExInfo.AfTileFv1HorizontalShift = %d\r\n", AfStatExInfo.AfTileFv1HorizontalShift);
        fprintf(pFile, "AfStatExInfo.AfTileFv2HorizontalShift = %d\r\n", AfStatExInfo.AfTileFv2HorizontalShift);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter1Gain[0] = %d\r\n", AfStatExInfo.AfHorizontalFilter1Gain[0]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter1Gain[1] = %d\r\n", AfStatExInfo.AfHorizontalFilter1Gain[1]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter1Gain[2] = %d\r\n", AfStatExInfo.AfHorizontalFilter1Gain[2]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter1Gain[3] = %d\r\n", AfStatExInfo.AfHorizontalFilter1Gain[3]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter1Gain[4] = %d\r\n", AfStatExInfo.AfHorizontalFilter1Gain[4]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter1Gain[5] = %d\r\n", AfStatExInfo.AfHorizontalFilter1Gain[5]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter1Gain[6] = %d\r\n", AfStatExInfo.AfHorizontalFilter1Gain[6]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter1Shift[0] = %d\r\n", AfStatExInfo.AfHorizontalFilter1Shift[0]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter1Shift[1] = %d\r\n", AfStatExInfo.AfHorizontalFilter1Shift[1]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter1Shift[2] = %d\r\n", AfStatExInfo.AfHorizontalFilter1Shift[2]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter1Shift[3] = %d\r\n", AfStatExInfo.AfHorizontalFilter1Shift[3]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter2Gain[0] = %d\r\n", AfStatExInfo.AfHorizontalFilter2Gain[0]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter2Gain[1] = %d\r\n", AfStatExInfo.AfHorizontalFilter2Gain[1]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter2Gain[2] = %d\r\n", AfStatExInfo.AfHorizontalFilter2Gain[2]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter2Gain[3] = %d\r\n", AfStatExInfo.AfHorizontalFilter2Gain[3]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter2Gain[4] = %d\r\n", AfStatExInfo.AfHorizontalFilter2Gain[4]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter2Gain[5] = %d\r\n", AfStatExInfo.AfHorizontalFilter2Gain[5]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter2Gain[6] = %d\r\n", AfStatExInfo.AfHorizontalFilter2Gain[6]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter2Shift[0] = %d\r\n", AfStatExInfo.AfHorizontalFilter2Shift[0]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter2Shift[1] = %d\r\n", AfStatExInfo.AfHorizontalFilter2Shift[1]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter2Shift[2] = %d\r\n", AfStatExInfo.AfHorizontalFilter2Shift[2]);
        fprintf(pFile, "AfStatExInfo.AfHorizontalFilter2Shift[3] = %d\r\n", AfStatExInfo.AfHorizontalFilter2Shift[3]);

        rval = AmbaIK_GetPgAfStatExInfo(pMode, &PgAfStatExInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetPgAfStatExInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter1Stage1Enb = %d\r\n", PgAfStatExInfo.AfHorizontalFilter1Stage1Enb);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter1Stage2Enb = %d\r\n", PgAfStatExInfo.AfHorizontalFilter1Stage2Enb);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter1Stage3Enb = %d\r\n", PgAfStatExInfo.AfHorizontalFilter1Stage3Enb);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter2Stage1Enb = %d\r\n", PgAfStatExInfo.AfHorizontalFilter2Stage1Enb);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter2Stage2Enb = %d\r\n", PgAfStatExInfo.AfHorizontalFilter2Stage2Enb);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter2Stage3Enb = %d\r\n", PgAfStatExInfo.AfHorizontalFilter2Stage3Enb);
        fprintf(pFile, "PgAfStatExInfo.AfVerticalFilter1Thresh = %d\r\n", PgAfStatExInfo.AfVerticalFilter1Thresh);
        fprintf(pFile, "PgAfStatExInfo.AfTileFv1HorizontalShift = %d\r\n", PgAfStatExInfo.AfTileFv1HorizontalShift);
        fprintf(pFile, "PgAfStatExInfo.AfTileFv1HorizontalWeight = %d\r\n", PgAfStatExInfo.AfTileFv1HorizontalWeight);
        fprintf(pFile, "PgAfStatExInfo.AfVerticalFilter2Thresh = %d\r\n", PgAfStatExInfo.AfVerticalFilter2Thresh);
        fprintf(pFile, "PgAfStatExInfo.AfTileFv2HorizontalShift = %d\r\n", PgAfStatExInfo.AfTileFv2HorizontalShift);
        fprintf(pFile, "PgAfStatExInfo.AfTileFv2HorizontalWeight = %d\r\n", PgAfStatExInfo.AfTileFv2HorizontalWeight);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter1Gain[0] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter1Gain[0]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter1Gain[1] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter1Gain[1]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter1Gain[2] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter1Gain[2]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter1Gain[3] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter1Gain[3]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter1Gain[4] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter1Gain[4]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter1Gain[5] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter1Gain[5]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter1Gain[6] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter1Gain[6]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter1Shift[0] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter1Shift[0]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter1Shift[1] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter1Shift[1]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter1Shift[2] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter1Shift[2]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter1Shift[3] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter1Shift[3]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter2Gain[0] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter2Gain[0]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter2Gain[1] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter2Gain[1]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter2Gain[2] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter2Gain[2]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter2Gain[3] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter2Gain[3]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter2Gain[4] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter2Gain[4]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter2Gain[5] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter2Gain[5]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter2Gain[6] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter2Gain[6]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter2Shift[0] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter2Shift[0]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter2Shift[1] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter2Shift[1]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter2Shift[2] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter2Shift[2]);
        fprintf(pFile, "PgAfStatExInfo.AfHorizontalFilter2Shift[3] = %d\r\n", PgAfStatExInfo.AfHorizontalFilter2Shift[3]);

        rval = AmbaIK_GetHistogramInfo(pMode, &HistInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetHistogramInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
        fprintf(pFile, "HistInfo.AeTileMask[0] = 0x%X\r\n", HistInfo.AeTileMask[0]);
        fprintf(pFile, "HistInfo.AeTileMask[1] = 0x%X\r\n", HistInfo.AeTileMask[1]);
        fprintf(pFile, "HistInfo.AeTileMask[2] = 0x%X\r\n", HistInfo.AeTileMask[2]);
        fprintf(pFile, "HistInfo.AeTileMask[3] = 0x%X\r\n", HistInfo.AeTileMask[3]);
        fprintf(pFile, "HistInfo.AeTileMask[4] = 0x%X\r\n", HistInfo.AeTileMask[4]);
        fprintf(pFile, "HistInfo.AeTileMask[5] = 0x%X\r\n", HistInfo.AeTileMask[5]);
        fprintf(pFile, "HistInfo.AeTileMask[6] = 0x%X\r\n", HistInfo.AeTileMask[6]);
        fprintf(pFile, "HistInfo.AeTileMask[7] = 0x%X\r\n", HistInfo.AeTileMask[7]);
        fprintf(pFile, "HistInfo.AeTileMask[8] = 0x%X\r\n", HistInfo.AeTileMask[8]);
        fprintf(pFile, "HistInfo.AeTileMask[9] = 0x%X\r\n", HistInfo.AeTileMask[9]);
        fprintf(pFile, "HistInfo.AeTileMask[10] = 0x%X\r\n", HistInfo.AeTileMask[10]);
        fprintf(pFile, "HistInfo.AeTileMask[11] = 0x%X\r\n", HistInfo.AeTileMask[11]);
        fprintf(pFile, "HistInfo.AeTileMask[12] = 0x%X\r\n", HistInfo.AeTileMask[12]);
        fprintf(pFile, "HistInfo.AeTileMask[13] = 0x%X\r\n", HistInfo.AeTileMask[13]);
        fprintf(pFile, "HistInfo.AeTileMask[14] = 0x%X\r\n", HistInfo.AeTileMask[14]);
        fprintf(pFile, "HistInfo.AeTileMask[15] = 0x%X\r\n", HistInfo.AeTileMask[15]);

        rval = AmbaIK_GetPgHistogramInfo(pMode, &PgHistInfo);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetPgHistogramInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
        fprintf(pFile, "PgHistInfo.AeTileMask[0] = 0x%X\r\n", PgHistInfo.AeTileMask[0]);
        fprintf(pFile, "PgHistInfo.AeTileMask[1] = 0x%X\r\n", PgHistInfo.AeTileMask[1]);
        fprintf(pFile, "PgHistInfo.AeTileMask[2] = 0x%X\r\n", PgHistInfo.AeTileMask[2]);
        fprintf(pFile, "PgHistInfo.AeTileMask[3] = 0x%X\r\n", PgHistInfo.AeTileMask[3]);
        fprintf(pFile, "PgHistInfo.AeTileMask[4] = 0x%X\r\n", PgHistInfo.AeTileMask[4]);
        fprintf(pFile, "PgHistInfo.AeTileMask[5] = 0x%X\r\n", PgHistInfo.AeTileMask[5]);
        fprintf(pFile, "PgHistInfo.AeTileMask[6] = 0x%X\r\n", PgHistInfo.AeTileMask[6]);
        fprintf(pFile, "PgHistInfo.AeTileMask[7] = 0x%X\r\n", PgHistInfo.AeTileMask[7]);
        fprintf(pFile, "PgHistInfo.AeTileMask[8] = 0x%X\r\n", PgHistInfo.AeTileMask[8]);
        fprintf(pFile, "PgHistInfo.AeTileMask[9] = 0x%X\r\n", PgHistInfo.AeTileMask[9]);
        fprintf(pFile, "PgHistInfo.AeTileMask[10] = 0x%X\r\n", PgHistInfo.AeTileMask[10]);
        fprintf(pFile, "PgHistInfo.AeTileMask[11] = 0x%X\r\n", PgHistInfo.AeTileMask[11]);
        fprintf(pFile, "PgHistInfo.AeTileMask[12] = 0x%X\r\n", PgHistInfo.AeTileMask[12]);
        fprintf(pFile, "PgHistInfo.AeTileMask[13] = 0x%X\r\n", PgHistInfo.AeTileMask[13]);
        fprintf(pFile, "PgHistInfo.AeTileMask[14] = 0x%X\r\n", PgHistInfo.AeTileMask[14]);
        fprintf(pFile, "PgHistInfo.AeTileMask[15] = 0x%X\r\n", PgHistInfo.AeTileMask[15]);
    }

    {
        // resamp strength
        AMBA_IK_RESAMPLER_STR_s ResamplerStr = {0};

        rval = AmbaIK_GetResampStrength(pMode, &ResamplerStr);
        LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetResampStrength", "OK case in ik_ctest_hdr20.c", id_cnt++);
        fprintf(pFile, "ResamplerStr.CfaCutoffFreq = %d\r\n", ResamplerStr.CfaCutoffFreq);
        fprintf(pFile, "ResamplerStr.MainCutoffFreq = %d\r\n", ResamplerStr.MainCutoffFreq);
    }
#endif
}

static void _GData_VinSensorRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_GetVinSensorInfo(pMode, &GDataGet.SensorInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetVinSensorInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
}

static void _GData_RgbIrRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_GetRgbIr(pMode, &GDataGet.RgbIr);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetRgbIr", "OK case in ik_ctest_hdr20.c", id_cnt++);
}

static void _GData_Raw2RawRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_GetFeToneCurve(pMode, &GDataGet.FeToneCurve.Decompand);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFeToneCurve", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetFeStaticBlc(pMode, &GDataGet.StaticBlackLevel[0], 0UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFeStaticBlc", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetDynamicBadPxlCor(pMode, &GDataGet.DynamicBadPixelCorrection);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetDynamicBadPxlCor", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetCfaLeakageFilter(pMode, &GDataGet.CfaLeakageFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetCfaLeakageFilter", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetCfaNoiseFilter(pMode, &GDataGet.CfaNoiseFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetCfaNoiseFilter", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetAntiAliasing(pMode, &GDataGet.AntiAliasing);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetAntiAliasing", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetFeWbGain(pMode, &GDataGet.FrontEndWbGain[0], 0UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFeWbGain", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetAfterCeWbGain(pMode, &GDataGet.AfterCeWbGain);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetAfterCeWbGain", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetBeforeCeWbGain(pMode, &GDataGet.BeforeCeWbGain);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetBeforeCeWbGain", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetRgbTo12Y(pMode, &GDataGet.RgbTo12Y);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetRgbTo12Y", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetDemosaic(pMode, &GDataGet.DemosaicFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetDemosaic", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetGrgbMismatch(pMode, &GDataGet.GbGrMismatch);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetGrgbMismatch", "OK case in ik_ctest_hdr20.c", id_cnt++);
}

static void _GData_Yuv2YuvRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_GetColorCorrection(pMode, &GDataGet.CcThreeD);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetColorCorrection", "OK case in ik_ctest_hdr20.c", id_cnt++);

    GDataGet.ColorCorrection.MatrixEn = GDataGet.CcThreeD.MatrixEn;
    GDataGet.ColorCorrection.MatrixShiftMinus8 = GDataGet.CcThreeD.MatrixShiftMinus8;
    memcpy(&GDataGet.ColorCorrection.Matrix[0], &GDataGet.CcThreeD.Matrix[0], sizeof(UINT32)*6UL);

    rval = AmbaIK_GetToneCurve(pMode, &GDataGet.ToneCurve);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetToneCurve", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetRgbToYuvMatrix(pMode, &GDataGet.RgbToYuvMatrix);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetRgbToYuvMatrix", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetChromaScale(pMode, &GDataGet.ChromaScale);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetChromaScale", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetChromaMedianFilter(pMode, &GDataGet.ChromaMedianFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetChromaMedianFilter", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetChromaFilter(pMode, &GDataGet.ChromaFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetChromaFilter", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetWideChromaFilter(pMode, &GDataGet.WideChromaFilter);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetWideChromaFilter", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetWideChromaFilterCombine(pMode, &GDataGet.WideChromaFilterCombine);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetWideChromaFilterCombine", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetLumaNoiseReduction(pMode, &GDataGet.LumaNoiseReduction);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetLumaNoiseReduction", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetFirstLumaProcMode(pMode, &GDataGet.FirstLumaProcessMode);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFirstLumaProcMode", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetAdvSpatFltr(pMode, &GDataGet.AsfInfo.AsfInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetAdvSpatFltr", "OK case in ik_ctest_hdr20.c", id_cnt++);
}

static void _GData_MctfRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_GetFnlShpNsBoth(pMode, &GDataGet.FinalSharpenBoth.BothInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsBoth", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetFnlShpNsBothTdt(pMode, &GDataGet.FinalSharpenBoth.ThreedTable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsBothTdt", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetFnlShpNsNoise(pMode, &GDataGet.FinalSharpenNoise);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsNoise", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetFnlShpNsFir(pMode, &GDataGet.FinalFir);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsFir", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetFnlShpNsCoring(pMode, &GDataGet.FinalCoring);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsCoring", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetFnlShpNsCorIdxScl(pMode, &GDataGet.FinalCoringIndexScale);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsCorIdxScl", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetFnlShpNsMinCorRst(pMode, &GDataGet.FinalMinCoringResult);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsMinCorRst", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetFnlShpNsMaxCorRst(pMode, &GDataGet.FinalMaxCoringResult);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsMaxCorRst", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetFnlShpNsSclCor(pMode, &GDataGet.FinalScaleCoring);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFnlShpNsSclCor", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetVideoMctf(pMode, &GDataGet.VideoMctf);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetVideoMctf", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetVideoMctfTa(pMode, &GDataGet.VideoMctfTemporalAdjust);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetVideoMctfTa", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetVideoMctfAndFnlshp(pMode, &GDataGet.VideoMctfAndFinalSharpen);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetVideoMctfAndFnlshp", "OK case in ik_ctest_hdr20.c", id_cnt++);
}

static void _GData_Raw2yuvCalibRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;
    UINT32 enable = 0;

    rval= AmbaIK_GetVinActiveWin(pMode, &GDataGet.Active);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetVinActiveWin", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_GetDummyMarginRange(pMode, &GDataGet.Dummy);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetDummyMarginRange", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_GetDzoomInfo(pMode, &GDataGet.Dzoom);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetDzoomInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_GetWarpInfo(pMode, &GDataGet.WarpCompensationInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetWarpInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);

    rval = AmbaIK_GetWarpEnb(pMode, &enable);
    fprintf(pFile, "WarpEnb = %d\r\n", enable);
    memset(&GDataGet.WarpCompensation, 0x0, sizeof(ITUNER_WARP_s));

    rval= AmbaIK_GetCawarpInfo(pMode, &GDataGet.CaWarpCompensationInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetCawarpInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);

    rval = AmbaIK_GetCawarpEnb(pMode, &enable);
    fprintf(pFile, "CawarpEnb = %d\r\n", enable);
    memset(&GDataGet.ChromaAberrationInfo, 0x0, sizeof(ITUNER_CHROMA_ABERRATION_s));

    rval= AmbaIK_GetVignette(pMode, &GDataGet.VignetteCompensation.VignetteInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetVignette", "OK case in ik_ctest_hdr20.c", id_cnt++);

    rval = AmbaIK_GetVignetteEnb(pMode, &enable);
    fprintf(pFile, "VignetteEnb = %d\r\n", enable);
    memset(&GDataGet.VignetteCompensation, 0x0, sizeof(ITUNER_VIGNETTE_s));

    rval= AmbaIK_GetStaticBadPxlCor(pMode, &GDataGet.SbpCorr);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetStaticBadPxlCor", "OK case in ik_ctest_hdr20.c", id_cnt++);

    rval = AmbaIK_GetStaticBadPxlCorEnb(pMode, &enable);
    fprintf(pFile, "StaticBadPxlCorEnb = %d\r\n", enable);
    memset(&GDataGet.StaticBadPixelCorrection, 0x0, sizeof(ITUNER_FPN_s));
}

static void _GData_CeRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval = 0;

    rval = AmbaIK_GetCe(pMode, &GDataGet.ContrastEnhance.ContrastEnhance);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetCe", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetCeInputTable(pMode, &GDataGet.ContrastEnhance.ContrastEnhanceInputTable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetCeInputTable", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetCeOutputTable(pMode, &GDataGet.ContrastEnhance.ContrastEnhanceOutTable);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetCeOutputTable", "OK case in ik_ctest_hdr20.c", id_cnt++);
}

static void _GData_VideoHdrRefresh(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 rval;
    AMBA_IK_WINDOW_SIZE_INFO_s WindowSizeInfo;

    rval= AmbaIK_GetHdrBlend(pMode, &GDataGet.VideoHdr.HdrBlend);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetHdrBlend", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_GetHdrRawOffset(pMode, &GDataGet.VideoHdr.RawInfo.Offset);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetHdrRawOffset", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval = AmbaIK_GetWindowSizeInfo(pMode, &WindowSizeInfo);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetWindowSizeInfo", "OK case in ik_ctest_hdr20.c", id_cnt++);
    GDataGet.VideoHdr.RawInfo.RawAreaWidth[0] = WindowSizeInfo.VinSensor.Width;
    GDataGet.VideoHdr.RawInfo.RawAreaWidth[1] = WindowSizeInfo.VinSensor.Width;
    GDataGet.VideoHdr.RawInfo.RawAreaWidth[2] = WindowSizeInfo.VinSensor.Width;
    GDataGet.VideoHdr.RawInfo.RawAreaHeight[0] = WindowSizeInfo.VinSensor.Height;
    GDataGet.VideoHdr.RawInfo.RawAreaHeight[1] = WindowSizeInfo.VinSensor.Height;
    GDataGet.VideoHdr.RawInfo.RawAreaHeight[2] = WindowSizeInfo.VinSensor.Height;

    rval= AmbaIK_GetFeWbGain(pMode, &GDataGet.FrontEndWbGain[1], 1UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFeWbGain", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_GetFeWbGain(pMode, &GDataGet.FrontEndWbGain[2], 2UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFeWbGain", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_GetFeStaticBlc(pMode, &GDataGet.StaticBlackLevel[1], 1UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFeStaticBlc", "OK case in ik_ctest_hdr20.c", id_cnt++);
    rval= AmbaIK_GetFeStaticBlc(pMode, &GDataGet.StaticBlackLevel[2], 2UL);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFeStaticBlc", "OK case in ik_ctest_hdr20.c", id_cnt++);
}

static void _GData_RefreshVideoHdrExpo(const AMBA_IK_MODE_CFG_s *pMode)
{
    _GData_SystemRefresh(pMode);
    _GData_VinSensorRefresh(pMode);
    _GData_RgbIrRefresh(pMode);
    _GData_Raw2RawRefresh(pMode);
    _GData_Yuv2YuvRefresh(pMode);
    _GData_MctfRefresh(pMode);
    _GData_Raw2yuvCalibRefresh(pMode);
    _GData_CeRefresh(pMode);
    _GData_VideoHdrRefresh(pMode);
}

static void _GData_Refresh(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_ABILITY_s *pAbility)
{
    AMBA_IK_ABILITY_s Ability = {0};
    UINT32 rval = 0;
    FILE *pBinFile;

    rval = AmbaIK_GetContextAbility(pMode, &Ability);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_GetContextAbility", "OK case in ik_ctest_hdr20.c", id_cnt++);
    fprintf(pFile, "Ability.Pipe = %d, Ability.StillPipe = %d, Ability.VideoPipe = %d\r\n",
            Ability.Pipe, Ability.StillPipe, Ability.VideoPipe);

    _GData_RefreshVideoHdrExpo(pMode);

    {
        pBinFile = fopen("data/ik_ctest_hdr20_gdata_get.bin", "wb");

        fwrite(&GDataGet, 1, sizeof(ITUNER_DATA_s), pBinFile);
        fclose(pBinFile);
    }
}

INT32 ik_ctest_hdr20_main(void)
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

    pFile = fopen("data/ik_ctest_hdr20.txt", "w");

    // init IK
    _load_idsp_default_binary();
    Ability.Pipe = AMBA_IK_PIPE_VIDEO;
    Ability.VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_3;

    _Init_Image_Kernel(&Ability);
    _Init_Image_Kernel_Core();

    Mode.ContextId = 0;
    Rval |= AmbaIK_InitContext(&Mode, &Ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitContext", "OK case in ik_ctest_hdr20.c", id_cnt++);

    // test set
    ik_init_debug_print_func();
    ik_hook_debug_print();
    Rval = AmbaIK_SetDebugLogId(LOG_SHOW_ALL, 1);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgDebugPrint.c", "AmbaIK_SetDebugLogId", "test OK case, if (filter_debug_log[all_ik_filters] == 1U) true", id_cnt++);
    _GData_Execute(&Mode, &Ability);

    Rval |= AmbaIK_ExecuteConfig(&Mode, &ExecuteContainer);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "OK case in ik_ctest_hdr20.c", id_cnt++);

    {
        SIZE_t size;
        UINT32 size_align;
        void *buffer, *data;
        IDSP_FLOW_HEADER_s *p_header;
        Rval |= AmbaIK_QueryDebugConfigSize(&size);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_QueryDebugConfigSize", "OK case in ik_ctest_hdr20.c", id_cnt++);

        size_align = size + 128; // extra 128 bytes for cr buf alignment
        buffer = (void*)malloc(size_align);
        data = (void *) ALIGN_N((uintptr)buffer, 128);
        Rval |= AmbaIK_DumpDebugConfig(&Mode, 0, data, size);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_DumpDebugConfig", "OK case in ik_ctest_hdr20.c", id_cnt++);

        p_header = (IDSP_FLOW_HEADER_s *)data;
        p_header->IdspFlowAddr = 0;

        {
            //write ikc
            FILE *fid;

            fid = fopen("data/ik_ctest_hdr20.bin", "wb");
            fwrite(data, 1, sizeof(amalgam_flow_data_t), fid);
            fclose(fid);
        }
#if 0
        {
            FILE *fid;
            amalgam_flow_data_t *p_cr = (amalgam_flow_data_t *)data;
            void *cr_addr = (void *)p_cr->sec2_cfg_buf + IDSP_CONFIG_HDR_SIZE;

            fid = fopen("data/ik_ctest_hdr20_sec2.bin", "wb");
            fwrite(cr_addr, 1, MAX_SEC_2_CFG_SZ, fid);
            fclose(fid);
        }
#endif
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

