#include "test_ik_global.h"
#include "AmbaDSP_ImgContextDebugUtility.h"


#define ALIGN_N(x_, n_)   ((((x_)+((n_)-1)) & ~((n_)-1)))

static UINT8 ik_working_buffer[5<<21];
static void *pBinDataAddr;
FILE *pFile;
static AMBA_ITN_SYSTEM_s _System;
static ITUNER_DATA_s GData;
UINT32 id_cnt;
static UINT8 WarpGrid[4*256*192];
static UINT8 CawarpRedGrid[4*64*96];
static UINT8 CawarpBlueGrid[4*64*96];
static UINT8 FPNMap[IK_AMALGAM_TABLE_SBP_SIZE];


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

static INT32 _reload_idsp_default_binary(void)
{
    INT32 rval = 0;
    FILE *pFileDefBin;
    UINT32 DefSize;

    //read default binary
    pFileDefBin = fopen("../../../../imgkernelcore/cv2xfs/early_test/idspdrv_init_cv2a.bin", "rb" );

    fseek(pFileDefBin, 0, SEEK_END);
    DefSize = ftell(pFileDefBin);
    rewind(pFileDefBin);

    fread((UINT8 *)pBinDataAddr, 1, DefSize, pFileDefBin);

    fclose(pFileDefBin);

    return rval;
}

static void _Reset_Image_Kernel(AMBA_IK_ABILITY_s *pAbility)
{
    UINT32 Rval = 0;
    AMBA_IK_CONTEXT_SETTING_s ContextSetting = {0};
    SIZE_t TotalCtxSize = 0;
    AMBA_IK_ABILITY_s Ability[2];
    AMBA_IK_MODE_CFG_s Mode[2];

    img_arch_deinit_architecture();
    iks_arch_deinit_architecture();

    memset(ik_working_buffer, 0, sizeof(ik_working_buffer));

    memcpy(Ability, pAbility, sizeof(AMBA_IK_ABILITY_s)*2);

    ContextSetting.ContextNumber = 2;
    ContextSetting.ConfigSetting[0].ConfigNumber = 5;
    ContextSetting.ConfigSetting[0].pAbility = &Ability[0];
    ContextSetting.ConfigSetting[1].ConfigNumber = 5;
    ContextSetting.ConfigSetting[1].pAbility = &Ability[1];

    _reload_idsp_default_binary();
    AmbaIK_QueryArchMemorySize(&ContextSetting, &TotalCtxSize);
    AmbaIK_InitArch(&ContextSetting, (void*)ik_working_buffer, sizeof(ik_working_buffer));
    AmbaIK_InitDefBinary(pBinDataAddr);

    Mode[0].ContextId = 0;
    Rval |= AmbaIK_InitContext(&Mode[0], &Ability[0]);

    Mode[1].ContextId = 1;
    Rval |= AmbaIK_InitContext(&Mode[1], &Ability[1]);
}

static void _Init_Image_Kernel(AMBA_IK_ABILITY_s *p_ability)
{
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

    ContextSetting.ContextNumber = 2;
    ContextSetting.ConfigSetting[0].ConfigNumber = 5;
    ContextSetting.ConfigSetting[0].pAbility = p_ability;
    ContextSetting.ConfigSetting[1].ConfigNumber = 5;
    ContextSetting.ConfigSetting[1].pAbility = ++p_ability;

    AmbaIK_QueryArchMemorySize(&ContextSetting, &TotalCtxSize);
    AmbaIK_InitArch(&ContextSetting, (void*)ik_working_buffer, sizeof(ik_working_buffer));
    AmbaIK_InitDefBinary(pBinDataAddr);
    AmbaIK_InitCheckParamFunc();
    AmbaIK_SetDebugLogId(255U, 1U);
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

    (void)AmbaIK_GetWindowSizeInfo(pMode, &WindowSizeInfo);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetWindowSizeInfo", "OK case in ik_ctest.c", id_cnt++);
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
    (void)AmbaIK_SetWindowSizeInfo(pMode, &WindowSizeInfo);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWindowSizeInfo", "OK case in ik_ctest.c", id_cnt++);

    // Update Flip Info
    FlipInfo = 0UL;
    (void)AmbaIK_SetFlipMode(pMode, FlipInfo);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFlipMode", "OK case in ik_ctest.c", id_cnt++);

    // make img_exe_execute_filter_update_check() happy...
    StitchInfo.Enable = 0;
    (void)AmbaIK_SetStitchingInfo(pMode, &StitchInfo);
    WarpBufInfo.LumaWaitLines = 8;
    WarpBufInfo.LumaDmaSize = 32;
    WarpBufInfo.Reserved = 32; // chroma_dma

    (void)AmbaIK_SetWarpBufferInfo(pMode, &WarpBufInfo);

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
        (void)AmbaIK_SetAwbStatInfo(pMode, &AwbStatInfo);
        //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAwbStatInfo", "OK case in ik_ctest.c", id_cnt++);

        AeStatInfo.AeTileNumCol = 24UL;
        AeStatInfo.AeTileNumRow = 16UL;
        AeStatInfo.AeTileColStart = 0UL;
        AeStatInfo.AeTileRowStart = 0UL;
        AeStatInfo.AeTileWidth = 170UL;
        AeStatInfo.AeTileHeight = 256UL;
        AeStatInfo.AePixMinValue = 0UL;
        AeStatInfo.AePixMaxValue = 16383UL;
        (void)AmbaIK_SetAeStatInfo(pMode, &AeStatInfo);
        //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAeStatInfo", "OK case in ik_ctest.c", id_cnt++);

        AfStatInfo.AfTileNumCol = 24UL;
        AfStatInfo.AfTileNumRow = 16UL;
        AfStatInfo.AfTileColStart = 0UL;
        AfStatInfo.AfTileRowStart = 0UL;
        AfStatInfo.AfTileWidth = 170UL;
        AfStatInfo.AfTileHeight = 256UL;
        AfStatInfo.AfTileActiveWidth = 170UL;
        AfStatInfo.AfTileActiveHeight = 256UL;
        (void)AmbaIK_SetAfStatInfo(pMode, &AfStatInfo);
        //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAfStatInfo", "OK case in ik_ctest.c", id_cnt++);

        PgAfStatInfo.AfTileNumCol = 32UL;
        PgAfStatInfo.AfTileNumRow = 16UL;
        PgAfStatInfo.AfTileColStart = 0UL;
        PgAfStatInfo.AfTileRowStart = 0UL;
        PgAfStatInfo.AfTileWidth = 128UL;
        PgAfStatInfo.AfTileHeight = 256UL;
        PgAfStatInfo.AfTileActiveWidth = 128UL;
        PgAfStatInfo.AfTileActiveHeight = 256UL;
        (void)AmbaIK_SetPgAfStatInfo(pMode, &PgAfStatInfo);
        //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPgAfStatInfo", "OK case in ik_ctest.c", id_cnt++);

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
        (void)AmbaIK_SetAfStatExInfo(pMode, &AfStatExInfo);
        //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAfStatExInfo", "OK case in ik_ctest.c", id_cnt++);

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
        (void)AmbaIK_SetPgAfStatExInfo(pMode, &PgAfStatExInfo);
        //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPgAfStatExInfo", "OK case in ik_ctest.c", id_cnt++);

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
        (void)AmbaIK_SetHistogramInfo(pMode, &HistInfo);
        //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetHistogramInfo", "OK case in ik_ctest.c", id_cnt++);

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
        (void)AmbaIK_SetPgHistogramInfo(pMode, &PgHistInfo);
        //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPgHistogramInfo", "OK case in ik_ctest.c", id_cnt++);
    }

    {
        // resamp strength
        AMBA_IK_RESAMPLER_STR_s ResamplerStr = {0};

        (void)AmbaIK_SetResampStrength(pMode, &ResamplerStr);
        //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetResampStrength", "OK case in ik_ctest.c", id_cnt++);
    }

    {
        // safety info
        AMBA_IK_SAFETY_INFO_s SafetyInfo;

        SafetyInfo.UpdateInterval = 1;
        (void)AmbaIK_SetSafetyInfo(pMode, &SafetyInfo);
        //LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_SetSafetyInfo", "OK case in ik_ctest.c", id_cnt++);
    }
#endif
}

static void _GData_VinSensorExecute(const AMBA_IK_MODE_CFG_s *pMode)
{

    (void)AmbaIK_GetVinSensorInfo(pMode, &GData.SensorInfo);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetVinSensorInfo", "OK case in ik_ctest.c", id_cnt++);
    GData.SensorInfo.SensorResolution = _System.RawResolution;
    GData.SensorInfo.SensorPattern = _System.RawBayer;
    GData.SensorInfo.SensorReadoutMode = _System.SensorReadoutMode;
    GData.SensorInfo.Compression = _System.CompressedRaw;
    GData.SensorInfo.SensorMode = _System.SensorMode;
    GData.SensorInfo.CompressionOffset = _System.CompressionOffset;
    (void)AmbaIK_SetVinSensorInfo(pMode, &GData.SensorInfo);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVinSensorInfo", "OK case in ik_ctest.c", id_cnt++);
}

static void _GData_RgbIrExecute(const AMBA_IK_MODE_CFG_s *pMode)
{

    (void)AmbaIK_SetRgbIr(pMode, &GData.RgbIr);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbIr", "OK case in ik_ctest.c", id_cnt++);
}

static void _GData_Raw2RawExecute(const AMBA_IK_MODE_CFG_s *pMode)
{

    (void)AmbaIK_SetFeToneCurve(pMode, &GData.FeToneCurve.Decompand);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeToneCurve", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetFeStaticBlc(pMode, &GData.StaticBlackLevel[0], 0UL);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeStaticBlc", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetDynamicBadPxlCor(pMode, &GData.DynamicBadPixelCorrection);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDynamicBadPxlCor", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetCfaLeakageFilter(pMode, &GData.CfaLeakageFilter);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCfaLeakageFilter", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetCfaNoiseFilter(pMode, &GData.CfaNoiseFilter);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCfaNoiseFilter", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetAntiAliasing(pMode, &GData.AntiAliasing);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAntiAliasing", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetFeWbGain(pMode, &GData.FrontEndWbGain[0], 0UL);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeWbGain", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetAfterCeWbGain(pMode, &GData.AfterCeWbGain);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAfterCeWbGain", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetBeforeCeWbGain(pMode, &GData.BeforeCeWbGain);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetBeforeCeWbGain", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetDemosaic(pMode, &GData.DemosaicFilter);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDemosaic", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetGrgbMismatch(pMode, &GData.GbGrMismatch);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetGrgbMismatch", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetRgbTo12Y(pMode, &GData.RgbTo12Y);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbTo12Y", "OK case in ik_ctest.c", id_cnt++);
}

static void _GData_Yuv2YuvExecute(const AMBA_IK_MODE_CFG_s *pMode)
{
    FILE *pBinFile;
    uint32 BinSize;

    {
        pBinFile = fopen("../util/bin/mn34220pl_0p3lux_cc_3d.bin", "rb");

        BinSize = IK_CC_3D_SIZE;

        fread(GData.CcThreeD.MatrixThreeDTable, 1, BinSize, pBinFile);
        fclose(pBinFile);
    }
    (void)AmbaIK_SetColorCorrection(pMode, &GData.CcThreeD);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetColorCorrection", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetToneCurve(pMode, &GData.ToneCurve);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetToneCurve", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetRgbToYuvMatrix(pMode, &GData.RgbToYuvMatrix);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbToYuvMatrix", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetChromaScale(pMode, &GData.ChromaScale);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaScale", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetChromaMedianFilter(pMode, &GData.ChromaMedianFilter);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaMedianFilter", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetChromaFilter(pMode, &GData.ChromaFilter);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaFilter", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetWideChromaFilter(pMode, &GData.WideChromaFilter);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWideChromaFilter", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetWideChromaFilterCombine(pMode, &GData.WideChromaFilterCombine);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWideChromaFilterCombine", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetLumaNoiseReduction(pMode, &GData.LumaNoiseReduction);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetLumaNoiseReduction", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetFirstLumaProcMode(pMode, &GData.FirstLumaProcessMode);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFirstLumaProcMode", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetAdvSpatFltr(pMode, &GData.AsfInfo.AsfInfo);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAdvSpatFltr", "OK case in ik_ctest.c", id_cnt++);
}

static void _GData_MctfExecute(const AMBA_IK_MODE_CFG_s *pMode)
{

    (void)AmbaIK_SetFnlShpNsBoth(pMode, &GData.FinalSharpenBoth.BothInfo);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsBoth", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetFnlShpNsBothTdt(pMode, &GData.FinalSharpenBoth.ThreedTable);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsBothTdt", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetFnlShpNsNoise(pMode, &GData.FinalSharpenNoise);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsNoise", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetFnlShpNsFir(pMode, &GData.FinalFir);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsFir", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetFnlShpNsCoring(pMode, &GData.FinalCoring);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsCoring", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetFnlShpNsCorIdxScl(pMode, &GData.FinalCoringIndexScale);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsCorIdxScl", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetFnlShpNsMinCorRst(pMode, &GData.FinalMinCoringResult);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsMinCorRst", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetFnlShpNsMaxCorRst(pMode, &GData.FinalMaxCoringResult);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsMaxCorRst", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetFnlShpNsSclCor(pMode, &GData.FinalScaleCoring);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsSclCor", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetVideoMctf(pMode, &GData.VideoMctf);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctf", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetVideoMctfTa(pMode, &GData.VideoMctfTemporalAdjust);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctfTa", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetVideoMctfAndFnlshp(pMode, &GData.VideoMctfAndFinalSharpen);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctfAndFnlshp", "OK case in ik_ctest.c", id_cnt++);
}

static void _GData_Raw2yuvCalibExecute(const AMBA_IK_MODE_CFG_s *pMode)
{

    (void)AmbaIK_SetVinActiveWin(pMode, &GData.Active);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVinActiveWin", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetDummyMarginRange(pMode, &GData.Dummy);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDummyMarginRange", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetDzoomInfo(pMode, &GData.Dzoom);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDzoomInfo", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetStaticBadPxlCorEnb(pMode, GData.StaticBadPixelCorrection.Enable);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetStaticBadPxlCorEnb", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetVignetteEnb(pMode, GData.VignetteCompensation.Enable);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVignetteEnb", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetCawarpEnb(pMode, (UINT32)GData.ChromaAberrationInfo.Enable);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCawarpEnb", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetWarpEnb(pMode, 0);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWarpEnb", "OK case in ik_ctest.c", id_cnt++);
}

static void _GData_CeExecute(const AMBA_IK_MODE_CFG_s *pMode)
{

    (void)AmbaIK_SetCe(pMode, &GData.ContrastEnhance.ContrastEnhance);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCe", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetCeInputTable(pMode, &GData.ContrastEnhance.ContrastEnhanceInputTable);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCeInputTable", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetCeOutputTable(pMode, &GData.ContrastEnhance.ContrastEnhanceOutTable);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCeOutputTable", "OK case in ik_ctest.c", id_cnt++);
}

static void _GData_VideoHdrExecute(const AMBA_IK_MODE_CFG_s *pMode)
{

    (void)AmbaIK_SetHdrBlend(pMode, &GData.VideoHdr.HdrBlend);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetHdrBlend", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetHdrRawOffset(pMode, &GData.VideoHdr.RawInfo.Offset);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetHdrRawOffset", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetFeWbGain(pMode, &GData.FrontEndWbGain[1], 1UL);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeWbGain", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetFeWbGain(pMode, &GData.FrontEndWbGain[2], 2UL);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeWbGain", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetFeStaticBlc(pMode, &GData.StaticBlackLevel[1], 1UL);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeStaticBlc", "OK case in ik_ctest.c", id_cnt++);
    (void)AmbaIK_SetFeStaticBlc(pMode, &GData.StaticBlackLevel[2], 2UL);
    //LOG_RVAL(pFile, rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeStaticBlc", "OK case in ik_ctest.c", id_cnt++);
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

static void IK_TestCovr_Set_Filter_Safety_Disable(const AMBA_IK_MODE_CFG_s *pMode0, const AMBA_IK_MODE_CFG_s *pMode1, AMBA_IK_ABILITY_s *pAbility)
{
    UINT32 Rval = IK_OK;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    p_ik_buffer_info->safety_enable = 0;

    Rval = AmbaIK_SetDebugLogId(LOG_SHOW_ALL, 0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgDebugPrint.c", "AmbaIK_SetDebugLogId", "test OK case, if (filter_debug_log[all_ik_filters] == 1U) false", id_cnt++);

    {
        // Sensor info
        AMBA_IK_VIN_SENSOR_INFO_s VinSensorInfo = {0};

        Rval = AmbaIK_SetVinSensorInfo(pMode0, &VinSensorInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVinSensorInfo", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Tone Curve
        AMBA_IK_TONE_CURVE_s ToneCurve = {0};

        Rval = AmbaIK_SetToneCurve(pMode0, &ToneCurve);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetToneCurve", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // flip
        UINT32 FlipMode = 0;

        Rval = AmbaIK_SetFlipMode(pMode0, FlipMode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFlipMode", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Warp buf info
        AMBA_IK_WARP_BUFFER_INFO_s WarpBufInfo = {0};

        WarpBufInfo.LumaWaitLines = 8;
        WarpBufInfo.LumaDmaSize = 8;
        Rval = AmbaIK_SetWarpBufferInfo(pMode0, &WarpBufInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWarpBufferInfo", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Stitch info
        AMBA_IK_STITCH_INFO_s StitchInfo = {0};

        Rval = AmbaIK_SetStitchingInfo(pMode0, &StitchInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetStitchingInfo", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Burst tile
        AMBA_IK_BURST_TILE_s BurstTile = {0};

        Rval = AmbaIK_SetBurstTile(pMode0, &BurstTile);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetBurstTile", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Vin and Main size
        AMBA_IK_WINDOW_SIZE_INFO_s WindowSizeInfo = {0};

        WindowSizeInfo.VinSensor.Width = 1920;
        WindowSizeInfo.VinSensor.Height = 1080;
        WindowSizeInfo.VinSensor.HSubSample.FactorDen = 1;
        WindowSizeInfo.VinSensor.HSubSample.FactorNum = 1;
        WindowSizeInfo.VinSensor.VSubSample.FactorDen = 1;
        WindowSizeInfo.VinSensor.VSubSample.FactorNum = 1;
        WindowSizeInfo.Main.Width = 1920;
        WindowSizeInfo.Main.Height = 1080;
        Rval = AmbaIK_SetWindowSizeInfo(pMode0, &WindowSizeInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWindowSizeInfo", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Dzoom
        AMBA_IK_DZOOM_INFO_s DzoomInfo = {0};

        Rval = AmbaIK_SetDzoomInfo(pMode0, &DzoomInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDzoomInfo", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Dummy win
        AMBA_IK_DUMMY_MARGIN_RANGE_s DummyMarginRange = {0};

        Rval = AmbaIK_SetDummyMarginRange(pMode0, &DummyMarginRange);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDummyMarginRange", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Vin active win
        AMBA_IK_VIN_ACTIVE_WINDOW_s VinActiveWin = {0};

        Rval = AmbaIK_SetVinActiveWin(pMode0, &VinActiveWin);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVinActiveWin", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // SBP
        AMBA_IK_STATIC_BAD_PXL_COR_s StaticBadPxlCor = {0};
        UINT32 StaticBpcEnb = 1;

        Rval = AmbaIK_SetStaticBadPxlCorEnb(pMode0, StaticBpcEnb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetStaticBadPxlCorEnb", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);

        StaticBadPxlCor.VinSensorGeo.HSubSample.FactorDen = 1;
        StaticBadPxlCor.VinSensorGeo.HSubSample.FactorNum = 1;
        StaticBadPxlCor.VinSensorGeo.VSubSample.FactorDen = 1;
        StaticBadPxlCor.VinSensorGeo.VSubSample.FactorNum = 1;
        StaticBadPxlCor.VinSensorGeo.Width = 1920;
        StaticBadPxlCor.VinSensorGeo.Height = 1080;
        StaticBadPxlCor.CalibSbpInfo.Version = 0x20180401;
        StaticBadPxlCor.CalibSbpInfo.pSbpBuffer = FPNMap;
        Rval = AmbaIK_SetStaticBadPxlCor(pMode0, &StaticBadPxlCor);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetStaticBadPxlCor", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Vig
        AMBA_IK_VIGNETTE_s Vignette = {0};
        UINT32 VignetteEnb = 1;

        Rval = AmbaIK_SetVignetteEnb(pMode0, VignetteEnb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVignetteEnb", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);

        Vignette.CalibModeEnable = 1;
        Vignette.VinSensorGeo.HSubSample.FactorDen = 1;
        Vignette.VinSensorGeo.HSubSample.FactorNum = 1;
        Vignette.VinSensorGeo.VSubSample.FactorDen = 1;
        Vignette.VinSensorGeo.VSubSample.FactorNum = 1;
        Vignette.VinSensorGeo.Width = 1920;
        Vignette.VinSensorGeo.Height = 1080;
        Vignette.CalibVignetteInfo.Version = 0x20180401;
        Rval = AmbaIK_SetVignette(pMode0, &Vignette);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVignette", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // CA
        AMBA_IK_CAWARP_INFO_s CawarpInfo = {0};
        UINT32 CawarpEnb = 1;

        Rval = AmbaIK_SetCawarpEnb(pMode0, CawarpEnb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCawarpEnb", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);

        CawarpInfo.HorGridNum = 32;
        CawarpInfo.VerGridNum = 48;
        CawarpInfo.TileWidthExp = 6;
        CawarpInfo.TileHeightExp = 5;
        CawarpInfo.VinSensorGeo.HSubSample.FactorDen = 1;
        CawarpInfo.VinSensorGeo.HSubSample.FactorNum = 1;
        CawarpInfo.VinSensorGeo.VSubSample.FactorDen = 1;
        CawarpInfo.VinSensorGeo.VSubSample.FactorNum = 1;
        CawarpInfo.VinSensorGeo.Width = 1920;
        CawarpInfo.VinSensorGeo.Height = 1080;
        CawarpInfo.Version = 0x20180401;
        CawarpInfo.pCawarpRed = (AMBA_IK_GRID_POINT_s *)CawarpRedGrid;
        CawarpInfo.pCawarpBlue = (AMBA_IK_GRID_POINT_s *)CawarpBlueGrid;
        Rval = AmbaIK_SetCawarpInfo(pMode0, &CawarpInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCawarpInfo", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Warp
        AMBA_IK_WARP_INFO_s WarpInfo = {0};
        UINT32 WarpEnb = 1;

        Rval = AmbaIK_SetWarpEnb(pMode0, WarpEnb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWarpEnb", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);

        WarpInfo.HorGridNum = 128;
        WarpInfo.VerGridNum = 96;
        WarpInfo.TileWidthExp = 4;
        WarpInfo.TileHeightExp = 4;
        WarpInfo.VinSensorGeo.HSubSample.FactorDen = 1;
        WarpInfo.VinSensorGeo.HSubSample.FactorNum = 1;
        WarpInfo.VinSensorGeo.VSubSample.FactorDen = 1;
        WarpInfo.VinSensorGeo.VSubSample.FactorNum = 1;
        WarpInfo.VinSensorGeo.Width = 1920;
        WarpInfo.VinSensorGeo.Height = 1080;
        WarpInfo.Version = 0x20180401;
        WarpInfo.pWarp = (AMBA_IK_GRID_POINT_s *)WarpGrid;
        Rval = AmbaIK_SetWarpInfo(pMode0, &WarpInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWarpInfo", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // HDR blend
        AMBA_IK_HDR_BLEND_s HdrBlend = {0};

        Rval = AmbaIK_SetHdrBlend(pMode0, &HdrBlend);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetHdrBlend", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // HDR raw info
        AMBA_IK_HDR_RAW_INFO_s HdrRawInfo = {0};

        Rval = AmbaIK_SetHdrRawOffset(pMode0, &HdrRawInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetHdrRawOffset", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // HDR tone curve
        AMBA_IK_FE_TONE_CURVE_s FeToneCurve = {0};

        Rval = AmbaIK_SetFeToneCurve(pMode0, &FeToneCurve);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeToneCurve", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // HDR front-end blc
        AMBA_IK_STATIC_BLC_LVL_s FeStaticBlc = {0};

        Rval = AmbaIK_SetFeStaticBlc(pMode0, &FeStaticBlc, 0);
        Rval = AmbaIK_SetFeStaticBlc(pMode0, &FeStaticBlc, 1);
        Rval = AmbaIK_SetFeStaticBlc(pMode0, &FeStaticBlc, 2);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeStaticBlc", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // HDR front-end wb
        AMBA_IK_FE_WB_GAIN_s FeWbGain = {0};

        FeWbGain.ShutterRatio = 16383;
        FeWbGain.IrGain = 4096;
        Rval = AmbaIK_SetFeWbGain(pMode0, &FeWbGain, 0);
        Rval = AmbaIK_SetFeWbGain(pMode0, &FeWbGain, 1);
        Rval = AmbaIK_SetFeWbGain(pMode0, &FeWbGain, 2);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeWbGain", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // CE
        AMBA_IK_CE_s Ce = {0};
        Ce.CoringIndexScaleHigh = 1U;
        Ce.CoringGainHigh = 1U;

        Rval = AmbaIK_SetCe(pMode0, &Ce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCe", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // CE input
        AMBA_IK_CE_INPUT_TABLE_s CeInputTable = {0};

        Rval = AmbaIK_SetCeInputTable(pMode0, &CeInputTable);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCeInputTable", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // CE output
        AMBA_IK_CE_OUT_TABLE_s CeOutTable = {0};

        Rval = AmbaIK_SetCeOutputTable(pMode0, &CeOutTable);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCeOutputTable", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // before CE wb
        AMBA_IK_WB_GAIN_s BeforeCeWbGain = {0};

        BeforeCeWbGain.GainR = 4096;
        BeforeCeWbGain.GainG = 4096;
        BeforeCeWbGain.GainB = 4096;
        Rval = AmbaIK_SetBeforeCeWbGain(pMode0, &BeforeCeWbGain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetBeforeCeWbGain", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // after CE wb
        AMBA_IK_WB_GAIN_s AfterCeWbGain = {0};

        AfterCeWbGain.GainR = 4096;
        AfterCeWbGain.GainG = 4096;
        AfterCeWbGain.GainB = 4096;
        Rval = AmbaIK_SetAfterCeWbGain(pMode0, &AfterCeWbGain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAfterCeWbGain", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // CFA leakage
        AMBA_IK_CFA_LEAKAGE_FILTER_s CfaLeakageFilter = {0};

        Rval = AmbaIK_SetCfaLeakageFilter(pMode0, &CfaLeakageFilter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCfaLeakageFilter", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // grgb mismatch
        AMBA_IK_GRGB_MISMATCH_s GrgbMismatch = {0};

        Rval = AmbaIK_SetGrgbMismatch(pMode0, &GrgbMismatch);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetGrgbMismatch", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Anti Aliasing
        AMBA_IK_ANTI_ALIASING_s AntiAliasing = {0};

        Rval = AmbaIK_SetAntiAliasing(pMode0, &AntiAliasing);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAntiAliasing", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // DBP
        AMBA_IK_DYNAMIC_BAD_PXL_COR_s DynamicBadPxlCor = {0};

        Rval = AmbaIK_SetDynamicBadPxlCor(pMode0, &DynamicBadPxlCor);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDynamicBadPxlCor", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // CFA noise
        AMBA_IK_CFA_NOISE_FILTER_s CfaNoiseFilter = {0};

        Rval = AmbaIK_SetCfaNoiseFilter(pMode0, &CfaNoiseFilter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCfaNoiseFilter", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // RGBIR
        AMBA_IK_RGB_IR_s RgbIr = {0};

        Rval = AmbaIK_SetRgbIr(pMode0, &RgbIr);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbIr", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // resamp strength
        AMBA_IK_RESAMPLER_STR_s ResamplerStr = {0};

        Rval = AmbaIK_SetResampStrength(pMode0, &ResamplerStr);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetResampStrength", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Demosaic
        AMBA_IK_DEMOSAIC_s Demosaic = {0};

        Rval = AmbaIK_SetDemosaic(pMode0, &Demosaic);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDemosaic", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // RGB to YUV
        AMBA_IK_RGB_TO_YUV_MATRIX_s RgbToYuvMatrix = {0};

        Rval = AmbaIK_SetRgbToYuvMatrix(pMode0, &RgbToYuvMatrix);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbToYuvMatrix", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // RGB to 12Y
        AMBA_IK_RGB_TO_12Y_s RgbTo12Y = {0};

        Rval = AmbaIK_SetRgbTo12Y(pMode0, &RgbTo12Y);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbTo12Y", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Pre CC gain
        AMBA_IK_PRE_CC_GAIN_s PreCcGain = {0};

        Rval = AmbaIK_SetPreCcGain(pMode0, &PreCcGain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPreCcGain", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // CC
        AMBA_IK_COLOR_CORRECTION_s ColorCorrection = {0};

        Rval = AmbaIK_SetColorCorrection(pMode0, &ColorCorrection);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetColorCorrection", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // AAA
        AMBA_IK_AE_STAT_INFO_s AeStatInfo = {0};
        AMBA_IK_AWB_STAT_INFO_s AwbStatInfo = {0};
        AMBA_IK_AF_STAT_INFO_s AfStatInfo = {0};
        AMBA_IK_AF_STAT_EX_INFO_s AfStatExInfo = {0};
        AMBA_IK_PG_AF_STAT_EX_INFO_s PgAfStatExInfo = {0};
        AMBA_IK_HISTOGRAM_INFO_s HistInfo = {0};

        AeStatInfo.AeTileNumCol = 24;
        AeStatInfo.AeTileNumRow = 16;
        Rval = AmbaIK_SetAeStatInfo(pMode0, &AeStatInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAeStatInfo", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);

        AwbStatInfo.AwbTileNumCol = 64;
        AwbStatInfo.AwbTileNumRow = 64;
        Rval = AmbaIK_SetAwbStatInfo(pMode0, &AwbStatInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAwbStatInfo", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);

        AfStatInfo.AfTileNumCol = 24;
        AfStatInfo.AfTileNumRow = 16;
        Rval = AmbaIK_SetAfStatInfo(pMode0, &AfStatInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAfStatInfo", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        Rval = AmbaIK_SetPgAfStatInfo(pMode0, &AfStatInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPgAfStatInfo", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        Rval = AmbaIK_SetAfStatExInfo(pMode0, &AfStatExInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAfStatExInfo", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        Rval = AmbaIK_SetPgAfStatExInfo(pMode0, &PgAfStatExInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPgAfStatExInfo", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        Rval = AmbaIK_SetHistogramInfo(pMode0, &HistInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetHistogramInfo", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        Rval = AmbaIK_SetPgHistogramInfo(pMode0, &HistInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPgHistogramInfo", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Chroma filters
        AMBA_IK_CHROMA_FILTER_s ChromaFilter = {0};

        ChromaFilter.Radius = 32;
        Rval = AmbaIK_SetChromaFilter(pMode0, &ChromaFilter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaFilter", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Wide Chroma filters
        AMBA_IK_WIDE_CHROMA_FILTER_s WideChromaFilter = {0};

        Rval = AmbaIK_SetWideChromaFilter(pMode0, &WideChromaFilter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWideChromaFilter", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Wide Chroma filters combine
        AMBA_IK_WIDE_CHROMA_FILTER_COMBINE_s WideChromaFilterCombine = {0};

        WideChromaFilterCombine.T1Cb = 10;
        WideChromaFilterCombine.T1Cr = 10;
        Rval = AmbaIK_SetWideChromaFilterCombine(pMode0, &WideChromaFilterCombine);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWideChromaFilterCombine", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Chroma median
        AMBA_IK_CHROMA_MEDIAN_FILTER_s ChromaMedianFilter = {0};

        Rval = AmbaIK_SetChromaMedianFilter(pMode0, &ChromaMedianFilter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaMedianFilter", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Chroma scale
        AMBA_IK_CHROMA_SCALE_s ChromaScale = {0};

        Rval = AmbaIK_SetChromaScale(pMode0, &ChromaScale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaScale", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Luma processing mode
        AMBA_IK_FIRST_LUMA_PROC_MODE_s FirstLumaProcMode = {0};

        Rval = AmbaIK_SetFirstLumaProcMode(pMode0, &FirstLumaProcMode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFirstLumaProcMode", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // ASF
        AMBA_IK_ADV_SPAT_FLTR_s AdvSpatFltr = {0};

        AdvSpatFltr.Fir.Specify = 2;
        AdvSpatFltr.LevelStrAdjust.High = 100;
        Rval = AmbaIK_SetAdvSpatFltr(pMode0, &AdvSpatFltr);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAdvSpatFltr", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // First Sharpen
        AMBA_IK_FSTSHPNS_BOTH_s FstShpNsBoth = {0};
        AMBA_IK_FSTSHPNS_FIR_s FstShpNsFir = {0};
        AMBA_IK_FSTSHPNS_NOISE_s FstShpNsNoise = {0};
        AMBA_IK_FSTSHPNS_CORING_s FstShpNsCoring = {0};
        AMBA_IK_FSTSHPNS_COR_IDX_SCL_s FstShpNsCorIdxScl = {0};
        AMBA_IK_FSTSHPNS_MIN_COR_RST_s FstShpNsMinCorRst = {0};
        AMBA_IK_FSTSHPNS_MAX_COR_RST_s FstShpNsMaxCorRst = {0};
        AMBA_IK_FSTSHPNS_SCL_COR_s FstShpNsSclCor = {0};

        Rval = AmbaIK_SetFstShpNsBoth(pMode0, &FstShpNsBoth);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsBoth", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        Rval = AmbaIK_SetFstShpNsFir(pMode0, &FstShpNsFir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsFir", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        FstShpNsNoise.LevelStrAdjust.High = 100;
        Rval = AmbaIK_SetFstShpNsNoise(pMode0, &FstShpNsNoise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsNoise", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        FstShpNsCoring.FractionalBits = 2;
        Rval = AmbaIK_SetFstShpNsCoring(pMode0, &FstShpNsCoring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsCoring", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        FstShpNsCorIdxScl.High = 100;
        Rval = AmbaIK_SetFstShpNsCorIdxScl(pMode0, &FstShpNsCorIdxScl);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsCorIdxScl", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        FstShpNsMinCorRst.High = 100;
        Rval = AmbaIK_SetFstShpNsMinCorRst(pMode0, &FstShpNsMinCorRst);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsMinCorRst", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        FstShpNsMaxCorRst.High = 100;
        Rval = AmbaIK_SetFstShpNsMaxCorRst(pMode0, &FstShpNsMaxCorRst);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsMaxCorRst", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        FstShpNsSclCor.High = 100;
        Rval = AmbaIK_SetFstShpNsSclCor(pMode0, &FstShpNsSclCor);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsSclCor", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // LNL
        AMBA_IK_LUMA_NOISE_REDUCTION_s LumaNoiseReduction = {0};

        Rval = AmbaIK_SetLumaNoiseReduction(pMode0, &LumaNoiseReduction);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetLumaNoiseReduction", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // MCTF
        AMBA_IK_VIDEO_MCTF_s VideoMctf = {0};

        Rval = AmbaIK_SetVideoMctf(pMode0, &VideoMctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctf", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // TA
        AMBA_IK_VIDEO_MCTF_TA_s VideoMctfTa = {0};

        Rval = AmbaIK_SetVideoMctfTa(pMode0, &VideoMctfTa);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctfTa", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Final Sharpen
        AMBA_IK_POS_DSP33_t VideoMctfAndFnlshp = {0};
        AMBA_IK_FNLSHPNS_BOTH_s FnlShpNsBoth = {0};
        AMBA_IK_FNLSHPNS_FIR_s FnlShpNsFir = {0};
        AMBA_IK_FNLSHPNS_NOISE_s FnlShpNsNoise = {0};
        AMBA_IK_FNLSHPNS_CORING_s FnlShpNsCoring = {0};
        AMBA_IK_FNLSHPNS_COR_IDX_SCL_s FnlShpNsCorIdxScl = {0};
        AMBA_IK_FNLSHPNS_MIN_COR_RST_s FnlShpNsMinCorRst = {0};
        AMBA_IK_FNLSHPNS_MAX_COR_RST_s FnlShpNsMaxCorRst = {0};
        AMBA_IK_FNLSHPNS_SCL_COR_s FnlShpNsSclCor = {0};
        AMBA_IK_FNLSHPNS_BOTH_TDT_s FnlShpNsBothTdt = {0};

        Rval = AmbaIK_SetVideoMctfAndFnlshp(pMode0, &VideoMctfAndFnlshp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctfAndFnlshp", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        Rval = AmbaIK_SetFnlShpNsBoth(pMode0, &FnlShpNsBoth);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsBoth", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        Rval = AmbaIK_SetFnlShpNsFir(pMode0, &FnlShpNsFir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsFir", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        FnlShpNsNoise.LevelStrAdjust.High = 100;
        Rval = AmbaIK_SetFnlShpNsNoise(pMode0, &FnlShpNsNoise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsNoise", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        FnlShpNsCoring.FractionalBits = 2;
        Rval = AmbaIK_SetFnlShpNsCoring(pMode0, &FnlShpNsCoring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsCoring", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        FnlShpNsCorIdxScl.High = 100;
        Rval = AmbaIK_SetFnlShpNsCorIdxScl(pMode0, &FnlShpNsCorIdxScl);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsCorIdxScl", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        FnlShpNsMinCorRst.High = 100;
        Rval = AmbaIK_SetFnlShpNsMinCorRst(pMode0, &FnlShpNsMinCorRst);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsMinCorRst", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        FnlShpNsMaxCorRst.High = 100;
        Rval = AmbaIK_SetFnlShpNsMaxCorRst(pMode0, &FnlShpNsMaxCorRst);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsMaxCorRst", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        FnlShpNsSclCor.High = 100;
        Rval = AmbaIK_SetFnlShpNsSclCor(pMode0, &FnlShpNsSclCor);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsSclCor", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
        Rval = AmbaIK_SetFnlShpNsBothTdt(pMode0, &FnlShpNsBothTdt);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsBothTdt", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // YUV mode
        UINT32 YuvMode = 0;

        Rval = AmbaIK_SetYuvMode(pMode1, YuvMode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetYuvMode", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // Warp internal
        AMBA_IK_WARP_INTERNAL_INFO_s WarpInternal = {0};

        WarpInternal.GridArrayWidth = 128;
        WarpInternal.GridArrayHeight = 96;
        WarpInternal.HorzGridSpacingExponent = 4;
        WarpInternal.VertGridSpacingExponent = 4;
        WarpInternal.VertWarpGridArrayWidth = 128;
        WarpInternal.VertWarpGridArrayHeight = 96;
        WarpInternal.VertWarpHorzGridSpacingExponent = 4;
        WarpInternal.VertWarpVertGridSpacingExponent = 4;
        WarpInternal.pWarpHorizontalTable = (INT16 *)WarpGrid;
        WarpInternal.pWarpVerticalTable = (INT16 *)WarpGrid;
        Rval = AmbaIK_SetWarpInternal(pMode0, &WarpInternal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWarpInternal", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // CA internal
        AMBA_IK_CAWARP_INTERNAL_INFO_s CawarpInternal = {0};

        Rval = AmbaIK_SetCaWarpInternal(pMode0, &CawarpInternal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCaWarpInternal", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // SBP internal
        AMBA_IK_SBP_INTERNAL_INFO_s SbpInternal = {0};

        SbpInternal.pMap = FPNMap;
        Rval = AmbaIK_SetStaticBpcInternal(pMode0, &SbpInternal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetStaticBpcInternal", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        // safety info
        AMBA_IK_SAFETY_INFO_s SafetyInfo;

        SafetyInfo.UpdateInterval = 1;
        Rval = AmbaIK_SetSafetyInfo(pMode0, &SafetyInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_SetSafetyInfo", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    {
        AMBA_IK_EXECUTE_CONTAINER_s ExecuteContainer = {0};
        Rval = AmbaIK_ExecuteConfig(pMode0, &ExecuteContainer);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);
    }

    p_ik_buffer_info->safety_enable = 1;

    {
        AMBA_IK_EXECUTE_CONTAINER_s ExecuteContainer = {0};
        SIZE_t Size = 0;
        void *Buffer, *Data;
        UINT32 Size_Align;

        _Reset_Image_Kernel(pAbility);

        AmbaIK_QueryDebugConfigSize(&Size);
        Size_Align = Size + 128; // extra 128 bytes for cr buf alignment
        Buffer = (void*)malloc(Size_Align);

        Data = (void *) ALIGN_N((uintptr)Buffer, 128);

        Rval = AmbaIK_DumpDebugConfig(pMode0, 0, NULL, Size+2);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgDebugUtility.c", "ik_dump_debug_config", "if(size < (uint32)amalgam_size) false case", id_cnt++);
        Rval = AmbaIK_DumpDebugConfig(pMode0, 5, Data, Size);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgDebugUtility.c", "ik_dump_debug_config", "if(previous_count > (p_ctx->organization.attribute.cr_ring_number - 1u)) true case", id_cnt++);
        Rval = AmbaIK_DumpDebugConfig(pMode0, 3, Data, Size);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgDebugUtility.c", "ik_dump_debug_config",
                 "if ((p_ctx->organization.active_cr_state.cr_running_number < p_ctx->organization.attribute.cr_ring_number) && (flow_id < 0)) ==> (flow_id < 0) true case", id_cnt++);

        _GData_Execute(pMode0, pAbility);

        AmbaIK_ExecuteConfig(pMode0, &ExecuteContainer);
        AmbaIK_ExecuteConfig(pMode0, &ExecuteContainer);
        AmbaIK_ExecuteConfig(pMode0, &ExecuteContainer);
        AmbaIK_ExecuteConfig(pMode0, &ExecuteContainer);
        AmbaIK_ExecuteConfig(pMode0, &ExecuteContainer);
        AmbaIK_ExecuteConfig(pMode0, &ExecuteContainer);
        AmbaIK_ExecuteConfig(pMode0, &ExecuteContainer);
        AmbaIK_ExecuteConfig(pMode0, &ExecuteContainer);

        {
            AMBA_IK_QUERY_FRAME_INFO_s FrameInfo = {0};
            Rval = AmbaIK_QueryFrameInfo(pMode0, 0x10008, &FrameInfo);
            LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_QueryFrameInfo", "test OK case", id_cnt++);
            fprintf(pFile, "FrameInfo.HdrRawInfo.XOffset[0] = %d, FrameInfo.HdrRawInfo.YOffset[0] = %d\r\n", FrameInfo.HdrRawInfo.XOffset[0], FrameInfo.HdrRawInfo.YOffset[0]);
            fprintf(pFile, "FrameInfo.HdrRawInfo.XOffset[1] = %d, FrameInfo.HdrRawInfo.YOffset[1] = %d\r\n", FrameInfo.HdrRawInfo.XOffset[1], FrameInfo.HdrRawInfo.YOffset[1]);
            fprintf(pFile, "FrameInfo.HdrRawInfo.XOffset[2] = %d, FrameInfo.HdrRawInfo.YOffset[2] = %d\r\n", FrameInfo.HdrRawInfo.XOffset[2], FrameInfo.HdrRawInfo.YOffset[2]);
        }

        Rval = AmbaIK_DumpDebugConfig(pMode0, 4, Data, Size);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgDebugUtility.c", "ik_dump_debug_config", "(flow_id < 0) true case", id_cnt++);

        free(Buffer);
    }
}

static void IK_TestCovr_Set_Filter_Fail(const AMBA_IK_MODE_CFG_s *pMode0, const AMBA_IK_MODE_CFG_s *pMode1)
{
    UINT32 Rval = IK_OK;

    {
        // Sensor info
        Rval = AmbaIK_SetVinSensorInfo(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVinSensorInfo", "test ik_set_vin_sensor_info() fail", id_cnt++);
    }

    {
        // Tone Curve
        Rval = AmbaIK_SetToneCurve(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetToneCurve", "test ik_set_tone_curve() fail", id_cnt++);
    }

    {
        // flip
        Rval = AmbaIK_SetFlipMode(pMode0, 0xFFFFFFFFUL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFlipMode", "test ik_set_flip_mode() fail", id_cnt++);
    }

    {
        // Warp buf info
        Rval = AmbaIK_SetWarpBufferInfo(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWarpBufferInfo", "test ik_set_warp_buffer_info() fail", id_cnt++);
    }

    {
        // Stitch info
        Rval = AmbaIK_SetStitchingInfo(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetStitchingInfo", "test ik_set_stitching_info() fail", id_cnt++);
    }

    {
        // Burst tile
        Rval = AmbaIK_SetBurstTile(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetBurstTile", "test ik_set_burst_tile() fail", id_cnt++);
    }

    {
        // Unsupport IK API
        AMBA_IK_AAA_PSEUDO_INFO_s PseudoYInfo = {0};

        Rval = AmbaIK_SetPseudoYInfo(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPseudoYInfo", "test NotSupportApi() fail", id_cnt++);
        Rval = AmbaIK_GetPseudoYInfo(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetPseudoYInfo", "test NotSupportApi() fail", id_cnt++);
        Rval = AmbaIK_SetPseudoYInfo(pMode0, &PseudoYInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPseudoYInfo", "test NotSupportApi() fail", id_cnt++);
        Rval = AmbaIK_GetPseudoYInfo(pMode0, &PseudoYInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetPseudoYInfo", "test NotSupportApi() fail", id_cnt++);
    }

    {
        // Vin and Main size
        Rval = AmbaIK_SetWindowSizeInfo(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWindowSizeInfo", "test ik_set_window_size_info() fail", id_cnt++);
    }

    {
        // Dzoom
        Rval = AmbaIK_SetDzoomInfo(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDzoomInfo", "test ik_set_dzoom_info() fail", id_cnt++);
    }

    {
        // Dummy win
        Rval = AmbaIK_SetDummyMarginRange(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDummyMarginRange", "test ik_set_dummy_margin_range() fail", id_cnt++);
    }

    {
        // Vin active win
        Rval = AmbaIK_SetVinActiveWin(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVinActiveWin", "test ik_set_vin_active_win() fail", id_cnt++);
    }

    {
        // SBP
        Rval = AmbaIK_SetStaticBadPxlCorEnb(pMode0, 2);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetStaticBadPxlCorEnb", "test ik_set_static_bad_pxl_corr_enb() fail", id_cnt++);
        Rval = AmbaIK_SetStaticBadPxlCor(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetStaticBadPxlCor", "test ik_set_static_bad_pxl_corr() fail", id_cnt++);
    }

    {
        // Vig
        Rval = AmbaIK_SetVignetteEnb(pMode0, 2);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVignetteEnb", "test ik_set_vignette_enb() fail", id_cnt++);
        Rval = AmbaIK_SetVignette(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVignette", "test ik_set_vignette() fail", id_cnt++);
    }

    {
        // CA
        Rval = AmbaIK_SetCawarpEnb(pMode0, 2);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCawarpEnb", "test ik_set_cawarp_enb() fail", id_cnt++);
        Rval = AmbaIK_SetCawarpInfo(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCawarpInfo", "test ik_set_cawarp_info() fail", id_cnt++);
    }

    {
        // Warp
        Rval = AmbaIK_SetWarpEnb(pMode0, 2);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWarpEnb", "test ik_set_warp_enb() fail", id_cnt++);
        Rval = AmbaIK_SetWarpInfo(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWarpInfo", "test ik_set_warp_info() fail", id_cnt++);
    }

    {
        // HDR blend
        Rval = AmbaIK_SetHdrBlend(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetHdrBlend", "test ik_set_hdr_blend() fail", id_cnt++);
    }

    {
        // HDR raw info
        Rval = AmbaIK_SetHdrRawOffset(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetHdrRawOffset", "test ik_set_hdr_raw_offset() fail", id_cnt++);
    }

    {
        // HDR tone curve
        Rval = AmbaIK_SetFeToneCurve(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeToneCurve", "test ik_set_frontend_tone_curve() fail", id_cnt++);
    }

    {
        // HDR front-end blc
        AMBA_IK_STATIC_BLC_LVL_s FeStaticBlc = {0};

        Rval = AmbaIK_SetFeStaticBlc(pMode0, NULL, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeStaticBlc", "test ik_set_exp0_frontend_static_blc() fail", id_cnt++);
        Rval = AmbaIK_SetFeStaticBlc(pMode0, NULL, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeStaticBlc", "test ik_set_exp1_frontend_static_blc() fail", id_cnt++);
        Rval = AmbaIK_SetFeStaticBlc(pMode0, NULL, 2);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeStaticBlc", "test ik_set_exp2_frontend_static_blc() fail", id_cnt++);
        Rval = AmbaIK_SetFeStaticBlc(pMode0, &FeStaticBlc, 3);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeStaticBlc", "test ik_set_expX_frontend_static_blc() fail", id_cnt++);
        Rval = AmbaIK_GetFeStaticBlc(pMode0, &FeStaticBlc, 3);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFeStaticBlc", "test ik_get_expX_frontend_static_blc() fail", id_cnt++);
    }

    {
        // HDR front-end wb
        AMBA_IK_FE_WB_GAIN_s FeWbGain = {0};

        Rval = AmbaIK_SetFeWbGain(pMode0, NULL, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeWbGain", "test ik_set_exp0_frontend_wb_gain() fail", id_cnt++);
        Rval = AmbaIK_SetFeWbGain(pMode0, NULL, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeWbGain", "test ik_set_exp1_frontend_wb_gain() fail", id_cnt++);
        Rval = AmbaIK_SetFeWbGain(pMode0, NULL, 2);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeWbGain", "test ik_set_exp2_frontend_wb_gain() fail", id_cnt++);
        Rval = AmbaIK_SetFeWbGain(pMode0, &FeWbGain, 3);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFeWbGain", "test ik_set_expX_frontend_wb_gain() fail", id_cnt++);
        Rval = AmbaIK_GetFeWbGain(pMode0, &FeWbGain, 3);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetFeWbGain", "test ik_get_expX_frontend_wb_gain() fail", id_cnt++);
    }

    {
        // CE
        Rval = AmbaIK_SetCe(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCe", "test ik_set_ce() fail", id_cnt++);
    }

    {
        // CE input
        Rval = AmbaIK_SetCeInputTable(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCeInputTable", "test ik_set_ce_input_table() fail", id_cnt++);
    }

    {
        // CE output
        Rval = AmbaIK_SetCeOutputTable(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCeOutputTable", "test ik_set_ce_out_table() fail", id_cnt++);
    }

    {
        // before CE wb
        Rval = AmbaIK_SetBeforeCeWbGain(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetBeforeCeWbGain", "test ik_set_before_ce_wb_gain() fail", id_cnt++);
    }

    {
        // after CE wb
        Rval = AmbaIK_SetAfterCeWbGain(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAfterCeWbGain", "test ik_set_after_ce_wb_gain() fail", id_cnt++);
    }

    {
        // CFA leakage
        Rval = AmbaIK_SetCfaLeakageFilter(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCfaLeakageFilter", "test ik_set_cfa_leakage_filter() fail", id_cnt++);
    }

    {
        // grgb mismatch
        Rval = AmbaIK_SetGrgbMismatch(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetGrgbMismatch", "test ik_set_grgb_mismatch() fail", id_cnt++);
    }

    {
        // Anti Aliasing
        Rval = AmbaIK_SetAntiAliasing(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAntiAliasing", "test ik_set_anti_aliasing() fail", id_cnt++);
    }

    {
        // DBP
        Rval = AmbaIK_SetDynamicBadPxlCor(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDynamicBadPxlCor", "test ik_set_dynamic_bad_pixel_corr() fail", id_cnt++);
    }

    {
        // CFA noise
        Rval = AmbaIK_SetCfaNoiseFilter(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCfaNoiseFilter", "test ik_set_cfa_noise_filter() fail", id_cnt++);
    }

    {
        // RGBIR
        Rval = AmbaIK_SetRgbIr(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbIr", "test ik_set_rgb_ir() fail", id_cnt++);
    }

    {
        // resamp strength
        Rval = AmbaIK_SetResampStrength(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetResampStrength", "test ik_set_resampler_strength() fail", id_cnt++);
    }

    {
        // Demosaic
        Rval = AmbaIK_SetDemosaic(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetDemosaic", "test ik_set_demosaic() fail", id_cnt++);
    }

    {
        // RGB to YUV
        Rval = AmbaIK_SetRgbToYuvMatrix(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbToYuvMatrix", "test ik_set_rgb_to_yuv_matrix() fail", id_cnt++);
    }

    {
        // RGB to 12Y
        Rval = AmbaIK_SetRgbTo12Y(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetRgbTo12Y", "test ik_set_rgb_to_12y() fail", id_cnt++);
    }

    {
        // Pre CC gain
        Rval = AmbaIK_SetPreCcGain(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPreCcGain", "test ik_set_pre_cc_gain() fail", id_cnt++);
    }

    {
        // CC
        Rval = AmbaIK_SetColorCorrection(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetColorCorrection", "test ik_set_color_correction() fail", id_cnt++);
    }

    {
        // AAA
        AMBA_IK_AE_STAT_INFO_s AeStatInfo = {0};
        AMBA_IK_AWB_STAT_INFO_s AwbStatInfo = {0};
        AMBA_IK_AF_STAT_INFO_s AfStatInfo = {0};

        AeStatInfo.AeTileNumCol = 25;
        Rval = AmbaIK_SetAeStatInfo(pMode0, &AeStatInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAeStatInfo", "test ik_set_aaa_stat_info() fail", id_cnt++);
        AwbStatInfo.AwbTileNumCol = 65;
        Rval = AmbaIK_SetAwbStatInfo(pMode0, &AwbStatInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAwbStatInfo", "test ik_set_aaa_stat_info() fail", id_cnt++);
        AfStatInfo.AfTileNumCol = 25;
        Rval = AmbaIK_SetAfStatInfo(pMode0, &AfStatInfo);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAfStatInfo", "test ik_set_aaa_stat_info() fail", id_cnt++);
        Rval = AmbaIK_SetPgAfStatInfo(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPgAfStatInfo", "test ik_set_aaa_pg_af_stat_info() fail", id_cnt++);

        Rval = AmbaIK_SetAfStatExInfo(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAfStatExInfo", "test ik_set_af_stat_ex_info() fail", id_cnt++);
        Rval = AmbaIK_SetPgAfStatExInfo(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPgAfStatExInfo", "test ik_set_pg_af_stat_ex_info() fail", id_cnt++);
        Rval = AmbaIK_SetHistogramInfo(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetHistogramInfo", "test ik_set_histogram_info() fail", id_cnt++);
        Rval = AmbaIK_SetPgHistogramInfo(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPgHistogramInfo", "test ik_set_pg_histogram_info() fail", id_cnt++);
    }

    {
        // Chroma filters
        Rval = AmbaIK_SetChromaFilter(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaFilter", "test ik_set_chroma_filter() fail", id_cnt++);
    }

    {
        // Wide Chroma filters
        Rval = AmbaIK_SetWideChromaFilter(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWideChromaFilter", "test ik_set_wide_chroma_filter() fail", id_cnt++);
    }

    {
        // Wide Chroma filters combine
        Rval = AmbaIK_SetWideChromaFilterCombine(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWideChromaFilterCombine", "test ik_set_wide_chroma_filter_combine() fail", id_cnt++);
    }

    {
        // Chroma median
        Rval = AmbaIK_SetChromaMedianFilter(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaMedianFilter", "test ik_set_chroma_median_filter() fail", id_cnt++);
    }

    {
        // Chroma scale
        Rval = AmbaIK_SetChromaScale(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetChromaScale", "test ik_set_chroma_scale() fail", id_cnt++);
    }

    {
        // Luma processing mode
        Rval = AmbaIK_SetFirstLumaProcMode(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFirstLumaProcMode", "test ik_set_fst_luma_process_mode() fail", id_cnt++);
    }

    {
        // ASF
        Rval = AmbaIK_SetAdvSpatFltr(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetAdvSpatFltr", "test ik_set_adv_spatial_filter() fail", id_cnt++);
    }

    {
        // First Sharpen
        Rval = AmbaIK_SetFstShpNsBoth(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsBoth", "test ik_set_fst_shp_both() fail", id_cnt++);
        Rval = AmbaIK_SetFstShpNsFir(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsFir", "test ik_set_fst_shp_fir() fail", id_cnt++);
        Rval = AmbaIK_SetFstShpNsNoise(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsNoise", "test ik_set_fst_shp_noise() fail", id_cnt++);
        Rval = AmbaIK_SetFstShpNsCoring(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsCoring", "test ik_set_fst_shp_coring() fail", id_cnt++);
        Rval = AmbaIK_SetFstShpNsCorIdxScl(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsCorIdxScl", "test ik_set_fst_shp_coring_idx_scale() fail", id_cnt++);
        Rval = AmbaIK_SetFstShpNsMinCorRst(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsMinCorRst", "test ik_set_fst_shp_min_coring_rslt() fail", id_cnt++);
        Rval = AmbaIK_SetFstShpNsMaxCorRst(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsMaxCorRst", "test ik_set_fst_shp_max_coring_rslt() fail", id_cnt++);
        Rval = AmbaIK_SetFstShpNsSclCor(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFstShpNsSclCor", "test ik_set_fst_shp_scale_coring() fail", id_cnt++);
    }

    {
        // LNL
        Rval = AmbaIK_SetLumaNoiseReduction(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetLumaNoiseReduction", "test ik_set_luma_noise_reduction() fail", id_cnt++);
    }

    {
        // MCTF
        Rval = AmbaIK_SetVideoMctf(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctf", "test ik_set_video_mctf() fail", id_cnt++);
    }

    {
        // TA
        Rval = AmbaIK_SetVideoMctfTa(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctfTa", "test ik_set_video_mctf_ta() fail", id_cnt++);
    }

    {
        // Final Sharpen
        Rval = AmbaIK_SetVideoMctfAndFnlshp(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetVideoMctfAndFnlshp", "test ik_set_video_mctf_and_fnl_shp() fail", id_cnt++);
        Rval = AmbaIK_SetFnlShpNsBoth(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsBoth", "test ik_set_fnl_shp_both() fail", id_cnt++);
        Rval = AmbaIK_SetFnlShpNsFir(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsFir", "test ik_set_fnl_shp_fir() fail", id_cnt++);
        Rval = AmbaIK_SetFnlShpNsNoise(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsNoise", "test ik_set_fnl_shp_noise() fail", id_cnt++);
        Rval = AmbaIK_SetFnlShpNsCoring(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsCoring", "test ik_set_fnl_shp_coring() fail", id_cnt++);
        Rval = AmbaIK_SetFnlShpNsCorIdxScl(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsCorIdxScl", "test ik_set_fnl_shp_coring_idx_scale() fail", id_cnt++);
        Rval = AmbaIK_SetFnlShpNsMinCorRst(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsMinCorRst", "test ik_set_fnl_shp_min_coring_rslt() fail", id_cnt++);
        Rval = AmbaIK_SetFnlShpNsMaxCorRst(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsMaxCorRst", "test ik_set_fnl_shp_max_coring_rslt() fail", id_cnt++);
        Rval = AmbaIK_SetFnlShpNsSclCor(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsSclCor", "test ik_set_fnl_shp_scale_coring() fail", id_cnt++);
        Rval = AmbaIK_SetFnlShpNsBothTdt(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetFnlShpNsBothTdt", "test ik_set_fnl_shp_three_d_table() fail", id_cnt++);
    }

    {
        // YUV mode
        Rval = AmbaIK_SetYuvMode(pMode1, 2);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetYuvMode", "test ik_set_yuv_mode() fail", id_cnt++);
    }

    {
        // Warp internal
        Rval = AmbaIK_SetWarpInternal(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetWarpInternal", "test ik_set_warp_internal() fail", id_cnt++);
    }

    {
        // CA internal
        Rval = AmbaIK_SetCaWarpInternal(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetCaWarpInternal", "test ik_set_cawarp_internal() fail", id_cnt++);
    }

    {
        // SBP internal
        Rval = AmbaIK_SetStaticBpcInternal(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetStaticBpcInternal", "test ik_set_static_bad_pxl_corr_itnl() fail", id_cnt++);
    }

    {
        // safety info
        Rval = AmbaIK_SetSafetyInfo(pMode0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_SetSafetyInfo", "test ik_set_safety_info() fail", id_cnt++);
    }
}

static void IK_TestCovr_Arch_Init_Fail(AMBA_IK_ABILITY_s *pAbility)
{
    UINT32 Rval = IK_OK;
    AMBA_IK_CONTEXT_SETTING_s ContextSetting = {0};

    Rval = AmbaIK_InitArch(&ContextSetting, (void*)ik_working_buffer, sizeof(ik_working_buffer));
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitArch", "test ik_init_arch() fail due to re-init", id_cnt++);

    {
        AMBA_IK_CONTEXT_SETTING_s ContextSetting = {0};
        extern uint32 MonitorConfigSelect;

        ContextSetting.ContextNumber = 2;
        ContextSetting.ConfigSetting[0].ConfigNumber = 5;
        ContextSetting.ConfigSetting[0].pAbility = pAbility;
        ContextSetting.ConfigSetting[1].ConfigNumber = 5;
        ContextSetting.ConfigSetting[1].pAbility = pAbility++;

        img_arch_deinit_architecture();
        iks_arch_deinit_architecture();

        MonitorConfigSelect = 0xff;
        Rval = AmbaIK_InitArch(&ContextSetting, (void*)ik_working_buffer, sizeof(ik_working_buffer));
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitArch", "test AmbaDSP_GetMonitorConfig return fail", id_cnt++);
        MonitorConfigSelect = 1;

        img_arch_deinit_architecture();
        iks_arch_deinit_architecture();
        Rval = AmbaIK_InitArch(&ContextSetting, (void*)ik_working_buffer, sizeof(ik_working_buffer));
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitArch", "test if((MonCfg.BitMask & DSP_MON_BITMASK_CRC) false OK case", id_cnt++);
        MonitorConfigSelect = 2;

        img_arch_deinit_architecture();
        iks_arch_deinit_architecture();
        Rval = AmbaIK_InitArch(&ContextSetting, (void*)ik_working_buffer, sizeof(ik_working_buffer));
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitArch", "test if((MonCfg.BitMask & DSP_MON_BITMASK_LOGIC) false OK case", id_cnt++);
        MonitorConfigSelect = 0;

        _Reset_Image_Kernel(pAbility);
    }
}

static void IK_TestCovr_Def_Bin_Init_Fail()
{
    UINT32 Rval = IK_OK;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    img_arch_get_ik_working_buffer(&p_ik_buffer_info);

    Rval = AmbaIK_InitDefBinary(pBinDataAddr);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitDefBinary", "test ik_init_default_binary() fail due to re-init", id_cnt++);
    p_ik_buffer_info->p_bin_data_dram_addr = NULL;
    p_ik_buffer_info->safety_enable = 0;
    Rval = AmbaIK_InitDefBinary(pBinDataAddr);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitDefBinary", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);

    p_ik_buffer_info->safety_enable = 1;
}

static void IK_TestCovr_Ctx_Init_Fail(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_ABILITY_s *pAbility)
{
    UINT32 Rval = IK_OK;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    Rval = AmbaIK_InitContext(pMode, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitContext", "test ik_init_context() fail due to null-ptr", id_cnt++);
    p_ik_buffer_info->safety_enable = 0;
    Rval = AmbaIK_InitContext(pMode, pAbility);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitContext", "test p_ik_buffer_info->safety_enable == 1u false case", id_cnt++);

    p_ik_buffer_info->safety_enable = 1;
}

static void IK_TestCovr_IK_Exe_Fail(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_ABILITY_s *pAbility)
{
    UINT32 Rval = IK_OK;

    Rval = AmbaIK_ExecuteConfig(pMode, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "test ik_execute() fail due to null-ptr", id_cnt++);

    {
        AMBA_IK_EXECUTE_CONTAINER_s ExecuteContainer = {0};
        ik_buffer_info_t *p_ik_buffer_info = NULL;
        ik_buffer_info_t *p_iks_buffer_info = NULL;

        img_arch_get_ik_working_buffer(&p_ik_buffer_info);
        iks_arch_get_ik_working_buffer(&p_iks_buffer_info);

        p_ik_buffer_info->safety_crc_enable = 0;
        _GData_Execute(pMode, pAbility);
        Rval = AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "test if(p_ik_buffer_info->safety_crc_enable == 1u) false OK case", id_cnt++);
        p_ik_buffer_info->safety_crc_enable = 1;

        _Reset_Image_Kernel(pAbility);

        _GData_Execute(pMode, pAbility);
#ifndef EARLYTEST_ENV
        AmbaIK_InjectSaftyError(pMode, AMBA_IK_SAFETY_ERROR_CRC_MISMATCH);
#else
        ExecuteContainer.Reserved = 100;
#endif
        Rval = AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "test if(p_execute_container->ik_crc != iks_execute_container.ik_crc) true NG case", id_cnt++);
        ExecuteContainer.Reserved = 0;

        _Reset_Image_Kernel(pAbility);

        img_arch_get_ik_working_buffer(&p_ik_buffer_info);
        iks_arch_get_ik_working_buffer(&p_iks_buffer_info);

        p_ik_buffer_info->safety_enable = 0;
        Rval = AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "test if(p_ik_buffer_info->safety_enable == 1u) false NG case", id_cnt++);

        p_ik_buffer_info->safety_enable = 1;
        p_ik_buffer_info->safety_logic_enable = 1;
        p_iks_buffer_info->safety_logic_enable = 1;
        p_ik_buffer_info->safety_state = IK_SAFETY_STATE_OK;
        p_iks_buffer_info->safety_state = IK_SAFETY_STATE_ERROR;
        Rval = AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "test (p_iks_buffer_info->safety_state == IK_SAFETY_STATE_ERROR) true NG case", id_cnt++);

        p_iks_buffer_info->safety_logic_enable = 0;
        Rval = AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "test (p_iks_buffer_info->safety_logic_enable == 1u) false NG case", id_cnt++);

        p_ik_buffer_info->safety_logic_enable = 0;
        Rval = AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "test (p_ik_buffer_info->safety_logic_enable == 1u) false NG case", id_cnt++);
    }
}

static void IK_TestCovr_Print_Func_Fail(void)
{
    UINT32 Rval = IK_OK;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    Rval = AmbaIK_InitPrintFunc();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitPrintFunc", "test AmbaIK_InitPrintFunc OK case", id_cnt++);
    img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    p_ik_buffer_info->init_flag = 0;
    Rval = AmbaIK_InitPrintFunc();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitPrintFunc", "test p_ik_buffer_info->init_flag == 1 false case", id_cnt++);
    img_arch_deinit_architecture();
    Rval = AmbaIK_InitPrintFunc();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitPrintFunc", "test img_arch_get_ik_working_buffer false case", id_cnt++);
}

static void IK_TestCovr_Check_Func_Fail(void)
{
    UINT32 Rval = IK_OK;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    p_ik_buffer_info->init_flag = 0;
    Rval = AmbaIK_InitCheckParamFunc();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitCheckParamFunc", "test p_ik_buffer_info->init_flag == 1 false case", id_cnt++);
    img_arch_deinit_architecture();
    Rval = AmbaIK_InitCheckParamFunc();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitCheckParamFunc", "test img_arch_get_ik_working_buffer false case", id_cnt++);
}

static void IK_TestCovr_QueryIdspClock(void)
{
    UINT32 Rval = IK_OK;
    AMBA_IK_QUERY_IDSP_CLOCK_INFO_s QueryIdspClockInfo = {0};
    AMBA_IK_QUERY_IDSP_CLOCK_RST_s QueryIdspClockRst = {0};

    Rval = AmbaIK_QueryIdspClock(NULL, &QueryIdspClockRst);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_QueryIdspClock", "test null_ptr fail case", id_cnt++);
    Rval = AmbaIK_QueryIdspClock(&QueryIdspClockInfo, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_QueryIdspClock", "test null_ptr fail case", id_cnt++);

    QueryIdspClockInfo.VinSensorGeo.Width = 1920;
    QueryIdspClockInfo.VinSensorGeo.Height = 1080;
    QueryIdspClockInfo.MainWindow.Width = 1920;
    QueryIdspClockInfo.MainWindow.Height = 1080;
    QueryIdspClockInfo.Fps = 30;
    Rval = AmbaIK_QueryIdspClock(&QueryIdspClockInfo, &QueryIdspClockRst);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_QueryIdspClock", "test OK case", id_cnt++);

    QueryIdspClockInfo.VinSensorGeo.Width = 0;
    QueryIdspClockInfo.MainWindow.Width = 0;
    QueryIdspClockInfo.Fps = 0;
    Rval = AmbaIK_QueryIdspClock(&QueryIdspClockInfo, &QueryIdspClockRst);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_QueryIdspClock", "test ik_query_idsp_clock fail check case", id_cnt++);

    QueryIdspClockInfo.VinSensorGeo.Width = 1920;
    QueryIdspClockInfo.VinSensorGeo.Height = 0;
    QueryIdspClockInfo.MainWindow.Width = 1920;
    QueryIdspClockInfo.MainWindow.Height = 0;
    Rval = AmbaIK_QueryIdspClock(&QueryIdspClockInfo, &QueryIdspClockRst);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_QueryIdspClock", "test ik_query_idsp_clock fail check case", id_cnt++);
}

static void IK_TestCovr_QueryCalcGeoBufSize(void)
{
    UINT32 Rval = IK_OK;
    AMBA_IK_QUERY_CALC_GEO_BUF_SIZE_s BufSizeInfo = {0};

    Rval = AmbaIK_QueryCalcGeoBufSize(NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_QueryCalcGeoBufSize", "test null_ptr fail case", id_cnt++);
    Rval = AmbaIK_QueryCalcGeoBufSize(&BufSizeInfo);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_QueryCalcGeoBufSize", "test OK case", id_cnt++);
}

static void IK_TestCovr_CalcGeoSettings(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;
    AMBA_IK_ABILITY_s Ability = {0};
    AMBA_IK_CALC_GEO_SETTINGS_s Info = {0};
    AMBA_IK_CALC_GEO_RESULT_s GeoRst = {0};
    AMBA_IK_CALC_GEO_SETTINGS_IN_s In = {0};
    AMBA_IK_VIN_SENSOR_INFO_s SensorInfo = {0};
    AMBA_IK_WINDOW_SIZE_INFO_s WindowSizeInfo = {0};
    AMBA_IK_VIN_ACTIVE_WINDOW_s VinActiveWindow = {0};
    AMBA_IK_DUMMY_MARGIN_RANGE_s DmyRange = {0};
    AMBA_IK_DZOOM_INFO_s DzoomInfo = {0};
    AMBA_IK_AAA_STAT_INFO_s AaaStatInfo = {0};
    AMBA_IK_AF_STAT_INFO_s AaaPgAfStatInfo = {0};
    AMBA_IK_AF_STAT_EX_INFO_s AfStatExInfo = {0};
    AMBA_IK_PG_AF_STAT_EX_INFO_s PgAfStatExInfo = {0};
    AMBA_IK_HISTOGRAM_INFO_s HistInfo = {0};
    AMBA_IK_HISTOGRAM_INFO_s HistInfoPg = {0};
    AMBA_IK_WARP_BUFFER_INFO_s WarpBufInfo = {0};
    AMBA_IK_CAWARP_INFO_s CalibCaWarpInfo = {0};
    AMBA_IK_WARP_INFO_s CalibWarpInfo = {0};
    FILE *pBinFile;
    uint32 BinSize;

    Rval |= AmbaIK_GetContextAbility(pMode, &Ability);
    Ability.VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_2;

    memset((void *)FPNMap, 0, IK_AMALGAM_TABLE_SBP_SIZE);
    memset((void *)WarpGrid, 0, 4*256*192);
    memset((void *)CawarpRedGrid, 0, 4*64*96);
    memset((void *)CawarpBlueGrid, 0, 4*64*96);
    Info.pIn = &In;
    Info.pIn->pSensorInfo = &SensorInfo;
    Info.pIn->pWindowSizeInfo = &WindowSizeInfo;
    Info.pIn->pVinActiveWindow = &VinActiveWindow;
    Info.pIn->pDmyRange = &DmyRange;
    Info.pIn->pDzoomInfo = &DzoomInfo;
    Info.pIn->pAaaStatInfo = &AaaStatInfo;
    Info.pIn->pAaaPgAfStatInfo = &AaaPgAfStatInfo;
    Info.pIn->pAfStatExInfo = &AfStatExInfo;
    Info.pIn->pPgAfStatExInfo = &PgAfStatExInfo;
    Info.pIn->pHistInfo = &HistInfo;
    Info.pIn->pHistInfoPg = &HistInfoPg;
    Info.pIn->pWarpBufInfo = &WarpBufInfo;
    Info.pIn->pCalibCaWarpInfo = &CalibCaWarpInfo;
    Info.pIn->pWarpInfo = &CalibWarpInfo;
    Info.pWorkBuf = (void *) ALIGN_N((uintptr)FPNMap + 120000, 128);
    Info.WorkBufSize = 8832;
    GeoRst.pOut = (void *)FPNMap;
    GeoRst.OutBufSize = 111104;

    SensorInfo.SensorPattern = 2;
    SensorInfo.SensorResolution = 14;

    DzoomInfo.Enable = 1;
    DzoomInfo.ZoomX = 131072;
    DzoomInfo.ZoomY = 131072;

    WindowSizeInfo.VinSensor.Width = 1920;
    WindowSizeInfo.VinSensor.Height = 1080;
    WindowSizeInfo.VinSensor.HSubSample.FactorDen = 1;
    WindowSizeInfo.VinSensor.HSubSample.FactorNum = 1;
    WindowSizeInfo.VinSensor.VSubSample.FactorDen = 1;
    WindowSizeInfo.VinSensor.VSubSample.FactorNum = 1;
    WindowSizeInfo.Main.Width = 1920;
    WindowSizeInfo.Main.Height = 1080;

    WarpBufInfo.DramEfficiency = 0;
    WarpBufInfo.LumaWaitLines = 8;
    WarpBufInfo.LumaDmaSize = 32;

    In.ChromaFilterRadius = 64;
    In.UseCawarp = 1;
    In.CawarpEnable = 1;
    In.WarpEnable = 1;
    In.YuvMode = 1;

    CalibCaWarpInfo.Version = 538444801;
    CalibCaWarpInfo.HorGridNum = 32;
    CalibCaWarpInfo.VerGridNum = 19;
    CalibCaWarpInfo.TileWidthExp = 6;
    CalibCaWarpInfo.TileHeightExp = 6;
    CalibCaWarpInfo.VinSensorGeo.Width = 1920;
    CalibCaWarpInfo.VinSensorGeo.Height = 1088;
    CalibCaWarpInfo.VinSensorGeo.HSubSample.FactorDen = 1;
    CalibCaWarpInfo.VinSensorGeo.HSubSample.FactorNum = 1;
    CalibCaWarpInfo.VinSensorGeo.VSubSample.FactorDen = 1;
    CalibCaWarpInfo.VinSensorGeo.VSubSample.FactorNum = 1;
    CalibCaWarpInfo.Enb2StageCompensation = 0;
    CalibCaWarpInfo.pCawarpRed = (AMBA_IK_GRID_POINT_s *)CawarpRedGrid;
    CalibCaWarpInfo.pCawarpBlue = (AMBA_IK_GRID_POINT_s *)CawarpBlueGrid;
    {
        pBinFile = fopen("../util/bin/cawarp_31x35_positive.bin", "rb");

        BinSize = CalibCaWarpInfo.HorGridNum * CalibCaWarpInfo.VerGridNum * sizeof(AMBA_IK_GRID_POINT_s);

        fread(CawarpRedGrid, 1, BinSize, pBinFile);
        fclose(pBinFile);
    }
    {
        pBinFile = fopen("../util/bin/cawarp_31x35_negative.bin", "rb");

        BinSize = CalibCaWarpInfo.HorGridNum * CalibCaWarpInfo.VerGridNum * sizeof(AMBA_IK_GRID_POINT_s);

        fread(CawarpBlueGrid, 1, BinSize, pBinFile);
        fclose(pBinFile);
    }

    CalibWarpInfo.Version = 538444801;
    CalibWarpInfo.HorGridNum = 32;
    CalibWarpInfo.VerGridNum = 19;
    CalibWarpInfo.TileWidthExp = 6;
    CalibWarpInfo.TileHeightExp = 6;
    CalibWarpInfo.VinSensorGeo.Width = 1920;
    CalibWarpInfo.VinSensorGeo.Height = 1088;
    CalibWarpInfo.VinSensorGeo.HSubSample.FactorNum = 1;
    CalibWarpInfo.VinSensorGeo.HSubSample.FactorDen = 1;
    CalibWarpInfo.VinSensorGeo.VSubSample.FactorNum = 1;
    CalibWarpInfo.VinSensorGeo.VSubSample.FactorDen = 1;
    CalibWarpInfo.Enb_2StageCompensation = 0u;
    CalibWarpInfo.pWarp = (AMBA_IK_GRID_POINT_s *)WarpGrid;
    {
        pBinFile = fopen("../util/bin/warp.bin", "rb");

        BinSize = CalibWarpInfo.HorGridNum * CalibWarpInfo.VerGridNum * sizeof(AMBA_IK_GRID_POINT_s);

        fread(WarpGrid, 1, BinSize, pBinFile);
        fclose(pBinFile);
    }

    AaaStatInfo.AeTileNumCol = 24;
    AaaStatInfo.AeTileNumRow = 16;
    AaaStatInfo.AeTileWidth = 171UL;
    AaaStatInfo.AeTileHeight = 256UL;
    AaaStatInfo.AePixMaxValue = 16383UL;

    AaaStatInfo.AfTileNumCol = 24;
    AaaStatInfo.AfTileNumRow = 16;
    AaaStatInfo.AfTileWidth = 171UL;
    AaaStatInfo.AfTileHeight = 256UL;
    AaaStatInfo.AfTileActiveWidth = 171UL;
    AaaStatInfo.AfTileActiveHeight = 256UL;

    AaaStatInfo.AwbTileNumCol = 64;
    AaaStatInfo.AwbTileNumRow = 64;
    AaaStatInfo.AwbTileWidth = 64UL;
    AaaStatInfo.AwbTileHeight = 64UL;
    AaaStatInfo.AwbTileActiveWidth = 64UL;
    AaaStatInfo.AwbTileActiveHeight = 64UL;
    AaaStatInfo.AwbPixMaxValue = 16383UL;

    AaaPgAfStatInfo.AfTileNumCol = 32UL;
    AaaPgAfStatInfo.AfTileNumRow = 16UL;
    AaaPgAfStatInfo.AfTileWidth = 128UL;
    AaaPgAfStatInfo.AfTileHeight = 256UL;
    AaaPgAfStatInfo.AfTileActiveWidth = 128UL;
    AaaPgAfStatInfo.AfTileActiveHeight= 256UL;

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

    HistInfoPg.AeTileMask[0] = 0xFFFFFF;
    HistInfoPg.AeTileMask[1] = 0xFFFFFF;
    HistInfoPg.AeTileMask[2] = 0xFFFFFF;
    HistInfoPg.AeTileMask[3] = 0xFFFFFF;
    HistInfoPg.AeTileMask[4] = 0xFFFFFF;
    HistInfoPg.AeTileMask[5] = 0xFFFFFF;
    HistInfoPg.AeTileMask[6] = 0xFFFFFF;
    HistInfoPg.AeTileMask[7] = 0xFFFFFF;
    HistInfoPg.AeTileMask[8] = 0xFFFFFF;
    HistInfoPg.AeTileMask[9] = 0xFFFFFF;
    HistInfoPg.AeTileMask[10] = 0xFFFFFF;
    HistInfoPg.AeTileMask[11] = 0xFFFFFF;
    HistInfoPg.AeTileMask[12] = 0xFFFFFF;
    HistInfoPg.AeTileMask[13] = 0xFFFFFF;
    HistInfoPg.AeTileMask[14] = 0xFFFFFF;
    HistInfoPg.AeTileMask[15] = 0xFFFFFF;
    Rval = AmbaIK_CalcGeoSettings(&Ability, &Info, &GeoRst);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_CalcGeoSettings", "test group cmd OK case", id_cnt++);

    {
        //write ikc
        FILE *fid;

        fid = fopen("data/group_cmd.bin", "wb");
        {
            // hack the addresses to all 0, we don't compare memory addresses.
            idsp_ik_group_update_info_t *p_info = (idsp_ik_group_update_info_t *)GeoRst.pOut;
            p_info->calib.cawarp_horizontal_table_addr_red = 0;
            p_info->calib.cawarp_horizontal_table_addr_blue = 0;
            p_info->calib.cawarp_vertical_table_addr_red = 0;
            p_info->calib.cawarp_vertical_table_addr_blue = 0;
            p_info->calib.warp_horizontal_table_address = 0;
            p_info->calib.warp_vertical_table_address = 0;
        }
        fwrite(GeoRst.pOut, 1, GeoRst.OutBufSize, fid);
        fclose(fid);
    }

    Rval = AmbaIK_CalcGeoSettings(NULL, &Info, &GeoRst);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_CalcGeoSettings", "test null_ptr fail case", id_cnt++);
    Rval = AmbaIK_CalcGeoSettings(&Ability, NULL, &GeoRst);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_CalcGeoSettings", "test null_ptr fail case", id_cnt++);
    Rval = AmbaIK_CalcGeoSettings(&Ability, &Info, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_CalcGeoSettings", "test null_ptr fail case", id_cnt++);
}

static void IK_TestCovr_CalcWarpResources(void)
{
    UINT32 Rval = IK_OK;
    AMBA_IK_QUERY_WARP_s QueryParams = {0};
    AMBA_IK_QUERY_WARP_RESULT_s QueryResult = {0};

    Rval = AmbaIK_CalcWarpResources(&QueryParams, &QueryResult);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_CalcWarpResources", "test OK case", id_cnt++);
}

static void IK_TestCovr_QueryFrameInfo(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;

    Rval = AmbaIK_QueryFrameInfo(pMode, 0, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_QueryFrameInfo", "test null_ptr fail case", id_cnt++);
}

static void IK_TestCovr_InjectSaftyError(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_ABILITY_s *pAbility)
{
    UINT32 Rval = IK_OK;
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    AMBA_IK_EXECUTE_CONTAINER_s ExecuteContainer = {0};

    img_arch_get_ik_working_buffer(&p_ik_buffer_info);

    {
        AMBA_IK_VIN_SENSOR_INFO_s VinSensorInfo= {0};

        //test 1st execute error injection.
        _GData_Execute(pMode, pAbility);
        (void)AmbaIK_GetVinSensorInfo(pMode, &VinSensorInfo);
        VinSensorInfo.SensorMode = 1;//let 1st execute calculation error.
        (void)AmbaIK_SetVinSensorInfo(pMode, &VinSensorInfo);
        Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "test 1st execute enter safety state fail case", id_cnt++);

        p_ik_buffer_info->safety_state = IK_SAFETY_STATE_OK;
        _Reset_Image_Kernel(pAbility);
    }

    //test 1st execute error injection.
    _GData_Execute(pMode, pAbility);
    Rval = AmbaIK_InjectSaftyError(pMode, AMBA_IK_FORCE_SAFETY_ERROR);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InjectSaftyError", "test set AMBA_IK_FORCE_SAFETY_ERROR OK case", id_cnt++);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "test 1st execute safety injection force safety error fail case", id_cnt++);

    p_ik_buffer_info->safety_state = IK_SAFETY_STATE_OK;
    _Reset_Image_Kernel(pAbility);

    //test 1st execute error injection.
    _GData_Execute(pMode, pAbility);
    Rval = AmbaIK_InjectSaftyError(pMode, AMBA_IK_SAFETY_ERROR_MEM_FENCE);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InjectSaftyError", "test set AMBA_IK_SAFETY_ERROR_MEM_FENCE OK case", id_cnt++);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "test 1st execute safety injection memory fence fail case", id_cnt++);

    p_ik_buffer_info->safety_state = IK_SAFETY_STATE_OK;
    _Reset_Image_Kernel(pAbility);

    //test ring buffer depth + 2 execution time error injection, update filter case.
    _GData_Execute(pMode, pAbility);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    _GData_Execute(pMode, pAbility);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    _GData_Execute(pMode, pAbility);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    _GData_Execute(pMode, pAbility);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    _GData_Execute(pMode, pAbility);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    _GData_Execute(pMode, pAbility);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    _GData_Execute(pMode, pAbility);
    Rval = AmbaIK_InjectSaftyError(pMode, AMBA_IK_SAFETY_ERROR_MEM_FENCE);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InjectSaftyError", "test set AMBA_IK_SAFETY_ERROR_MEM_FENCE OK case", id_cnt++);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "test couple round execute safety injection memory fence fail case", id_cnt++);

    p_ik_buffer_info->safety_state = IK_SAFETY_STATE_OK;
    _Reset_Image_Kernel(pAbility);

    //test ring buffer depth + 1 execution time error injection, update filter case.
    _GData_Execute(pMode, pAbility);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    _GData_Execute(pMode, pAbility);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    _GData_Execute(pMode, pAbility);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    _GData_Execute(pMode, pAbility);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    _GData_Execute(pMode, pAbility);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    _GData_Execute(pMode, pAbility);
    Rval = AmbaIK_InjectSaftyError(pMode, AMBA_IK_SAFETY_ERROR_RING_BUF);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InjectSaftyError", "test set AMBA_IK_SAFETY_ERROR_RING_BUF OK case", id_cnt++);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "test couple round execute safety injection ring buffer fail case", id_cnt++);

    p_ik_buffer_info->safety_state = IK_SAFETY_STATE_OK;
    _Reset_Image_Kernel(pAbility);

    //test 2nd execute error injection, no update filter case.
    _GData_Execute(pMode, pAbility);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    Rval = AmbaIK_InjectSaftyError(pMode, AMBA_IK_SAFETY_ERROR_RING_BUF);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InjectSaftyError", "test set AMBA_IK_SAFETY_ERROR_RING_BUF OK case", id_cnt++);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "test 2nd execute safety injection ring buffer fail case", id_cnt++);

    p_ik_buffer_info->safety_state = IK_SAFETY_STATE_OK;
    _Reset_Image_Kernel(pAbility);

    //test 1st execute error injection.
    _GData_Execute(pMode, pAbility);
    Rval = AmbaIK_InjectSaftyError(pMode, AMBA_IK_SAFETY_ERROR_CRC_MISMATCH);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InjectSaftyError", "test set AMBA_IK_SAFETY_ERROR_CRC_MISMATCH OK case", id_cnt++);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "test 1st execute safety injection CRC_mismatched fail send signal to R52 OK case", id_cnt++);

    p_ik_buffer_info->safety_state = IK_SAFETY_STATE_OK;
    _Reset_Image_Kernel(pAbility);

    //test 2nd execute error injection.
    _GData_Execute(pMode, pAbility);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    _GData_Execute(pMode, pAbility);
    Rval = AmbaIK_InjectSaftyError(pMode, AMBA_IK_SAFETY_ERROR_CRC_MISMATCH);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InjectSaftyError", "test set AMBA_IK_SAFETY_ERROR_CRC_MISMATCH OK case", id_cnt++);
    Rval |= AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_ExecuteConfig", "test 2nd execute safety injection CRC_mismatched fail send signal to R52 OK case", id_cnt++);

    p_ik_buffer_info->safety_state = IK_SAFETY_STATE_OK;
    _Reset_Image_Kernel(pAbility);

    //test error injection, by Injection API use memset(NULL) get failure to send siginal to R52.
    Rval = AmbaIK_InjectSaftyError(pMode, AMBA_IK_SAFETY_SYS_API_ERROR);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InjectSaftyError", "test set AMBA_IK_SAFETY_SYS_API_ERROR OK case", id_cnt++);

    p_ik_buffer_info->safety_state = IK_SAFETY_STATE_OK;

}

static void IK_TestCovr_SetDebugLogId(void)
{
    UINT32 Rval = IK_OK;

    Rval = AmbaIK_SetDebugLogId(0, 0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgDebugPrint.c", "AmbaIK_SetDebugLogId", "test OK case, if(debug_id == LOG_SHOW_CMD_ID_LIST) true", id_cnt++);
    Rval = AmbaIK_SetDebugLogId(1, 0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgDebugPrint.c", "AmbaIK_SetDebugLogId", "test OK case, else if (debug_id == LOG_SHOW_ALL) false", id_cnt++);
}

static void IK_TestCovr_Debug_Utility(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;
    extern uint32 ik_dump_debug_config(uint32 context_id, uint32 previous_count, const void *p_user_buffer, size_t size);

    Rval = ik_dump_debug_config(33, 0, NULL, 0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgDebugUtility.c", "ik_dump_debug_config", "test get_ctx fail case", id_cnt++);
    Rval = ik_dump_debug_config(pMode->ContextId, 0, NULL, 0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgDebugUtility.c", "ik_dump_debug_config", "test null-ptr fail", id_cnt++);
    Rval = ik_dump_debug_config(pMode->ContextId, 0, (void *)ik_working_buffer, 2);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgDebugUtility.c", "ik_dump_debug_config", "test (size < (uint32)amalgam_size) true case", id_cnt++);

#if 0
    {
        AMBA_IK_EXECUTE_CONTAINER_s ExecuteContainer = {0};
        amba_ik_context_entity_t *p_ctx = NULL;
        uint32 amalgam_size;

        _Reset_Image_Kernel(pAbility);
        img_ctx_get_context(pMode->ContextId, &p_ctx);

        _GData_Execute(pMode, pAbility);
        Rval = AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
        Rval = AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
        Rval = AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
        Rval = AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
        Rval = AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);

        img_cfg_query_debug_flow_data_size(&amalgam_size);
        p_ctx->organization.active_cr_state.active_flow_idx = 2;
        p_ctx->organization.active_cr_state.cr_running_number = 5;
        Rval = ik_dump_debug_config(pMode->ContextId, 3, (void *)FPNMap, amalgam_size);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgDebugUtility.c", "ik_dump_debug_config", "test if (flow_id < 0) true OK case", id_cnt++);
    }
#endif
}

static void IK_TestCovr_ImgExecuter(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    ik_execute_container_t execute_container = {0};
    amba_ik_context_entity_t *p_ctx = NULL;
    extern uint32 ik_execute(uint32 context_id, ik_execute_container_t *p_execute_container);
    extern uint32 ik_inject_safety_error(uint32 context_id, const uint32 selection);

    img_ctx_get_context(pMode->ContextId, &p_ctx);
    img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    Rval = ik_execute(33, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuter.c", "ik_execute", "test get_ctx fail case", id_cnt++);
    p_ik_buffer_info->safety_crc_enable = 0;
    Rval = ik_execute(pMode->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuter.c", "ik_execute", "test p_ik_buffer_info->safety_crc_enable false case", id_cnt++);
    p_ik_buffer_info->safety_crc_enable = 1;
    p_ctx->organization.initial_flag = 0;
    Rval = ik_execute(pMode->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuter.c", "ik_execute", "test (p_ctx->organization.initial_flag != 0U) fail case", id_cnt++);
    p_ctx->organization.initial_flag = 1;
    p_ctx->organization.active_cr_state.cr_running_number = p_ctx->organization.active_cr_state.max_running_number;
    Rval = ik_execute(pMode->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuter.c", "ik_execute", "test if(p_ctx->organization.active_cr_state.cr_running_number == p_ctx->organization.active_cr_state.max_running_number) true case", id_cnt++);

    p_ctx->filters.input_param.safety_info.update_freq = 2;
    p_ctx->organization.active_cr_state.cr_running_number = 2;
    Rval = ik_execute(pMode->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuter.c", "ik_execute", "test (p_ctx->filters.input_param.safety_info.update_freq == 1u) false and ((((uint32)p_ctx->organization.active_cr_state.cr_running_number - 1u) % p_ctx->filters.input_param.safety_info.update_freq) == 0u) true OK case", id_cnt++);
    p_ctx->organization.active_cr_state.cr_running_number = 3;
    Rval = ik_execute(pMode->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuter.c", "ik_execute", "test (p_ctx->filters.input_param.safety_info.update_freq == 1u) false and ((((uint32)p_ctx->organization.active_cr_state.cr_running_number - 1u) % p_ctx->filters.input_param.safety_info.update_freq) == 0u) false OK case", id_cnt++);
    p_ctx->filters.input_param.safety_info.update_freq = 1;

    Rval = ik_inject_safety_error(33, 0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuter.c", "ik_inject_safety_error", "test get_ctx fail case", id_cnt++);
}

static void IK_TestCovr_ImgInit(void)
{
    UINT32 Rval = IK_OK;
    ik_context_setting_t setting_of_each_context = {0};
    size_t size;
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    ik_ability_t ability = {0};
    extern uint32 ik_init_arch(const ik_context_setting_t *p_setting_of_each_context, void *p_mem_addr, size_t mem_size, uint32 ik_init_mode);
    extern uint32 ik_query_arch_memory_size(const ik_context_setting_t *p_setting_of_each_context,
                                            size_t *p_total_ctx_size,
                                            size_t *p_flow_ctrl_size,
                                            size_t *p_flow_tbl_size,
                                            size_t *p_flow_tbl_list_size,
                                            size_t *p_crc_data_size);
    extern uint32 ik_init_default_binary(void *p_bin_data_dram_addr);
    extern uint32 ik_init_context(uint32 context_id, const ik_ability_t *p_ability);

    img_arch_deinit_architecture();
    Rval = ik_init_arch(NULL, NULL, 0, AMBA_IK_ARCH_HARD_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "amba_ik_arch_init_check", "test null-ptr fail case", id_cnt++);

    setting_of_each_context.context_number = 33;
    Rval = ik_init_arch(&setting_of_each_context, NULL, 0, AMBA_IK_ARCH_HARD_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "amba_ik_arch_init_check", "test if (p_setting_of_each_context->context_number > MAX_CONTEXT_NUM) true case", id_cnt++);

    setting_of_each_context.context_number = 2;
    setting_of_each_context.cr_ring_setting[0].cr_ring_number = 33;
    Rval = ik_init_arch(&setting_of_each_context, NULL, 0, AMBA_IK_ARCH_HARD_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "amba_ik_arch_init_check", "test if (p_setting_of_each_context->cr_ring_setting[ctx].cr_ring_number > MAX_CR_RING_NUM) true case", id_cnt++);

    setting_of_each_context.cr_ring_setting[0].cr_ring_number = 5;
    setting_of_each_context.context_number = 1;
    Rval = ik_init_arch(&setting_of_each_context, (void*)&ik_working_buffer[1], sizeof(ik_working_buffer), AMBA_IK_ARCH_HARD_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "ik_init_arch", "test if ((usable_mem_addr & 0x1FU) > 0U) true case", id_cnt++);

    img_arch_deinit_architecture();
    ik_init_default_binary(NULL);
    Rval = ik_init_arch(&setting_of_each_context, (void*)&ik_working_buffer[0], 512, AMBA_IK_ARCH_HARD_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "ik_init_arch", "test if(ikc_register_valid_mem_range(usable_mem_addr, usable_mem_addr + usable_mem_size) != IK_OK) true case", id_cnt++);

    img_arch_deinit_architecture();
    ik_init_arch(&setting_of_each_context, (void*)&ik_working_buffer[0], sizeof(ik_working_buffer), AMBA_IK_ARCH_HARD_INIT);
    Rval = ik_init_arch(&setting_of_each_context, (void*)&ik_working_buffer[0], sizeof(ik_working_buffer), AMBA_IK_ARCH_SOFT_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "ik_init_arch", "test if (ik_init_mode == AMBA_IK_ARCH_HARD_INIT) false case", id_cnt++);
    ik_init_context(0, &ability);

    Rval = ik_query_arch_memory_size(NULL, NULL, &size, &size, &size, &size);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "ik_query_arch_memory_size", "test null-ptr fail case", id_cnt++);
    Rval = ik_query_arch_memory_size(NULL, &size, NULL, &size, &size, &size);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "ik_query_arch_memory_size", "test null-ptr fail case", id_cnt++);
    Rval = ik_query_arch_memory_size(NULL, &size, &size, NULL, &size, &size);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "ik_query_arch_memory_size", "test null-ptr fail case", id_cnt++);
    Rval = ik_query_arch_memory_size(NULL, &size, &size, &size, NULL, &size);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "ik_query_arch_memory_size", "test null-ptr fail case", id_cnt++);
    Rval = ik_query_arch_memory_size(NULL, &size, &size, &size, &size, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "ik_query_arch_memory_size", "test null-ptr fail case", id_cnt++);

    img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    p_ik_buffer_info->safety_state = IK_SAFETY_STATE_ERROR;
    Rval = ik_init_default_binary(NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "ik_init_default_binary", "test if(p_ik_buffer_info->safety_state == IK_SAFETY_STATE_OK) false case", id_cnt++);
    Rval = ik_init_context(0, &ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "ik_init_context", "test if(p_ik_buffer_info->safety_state == IK_SAFETY_STATE_OK) false case", id_cnt++);
    p_ik_buffer_info->safety_state = IK_SAFETY_STATE_OK;

    p_ik_buffer_info->init_flag = 0;
    Rval = ik_init_default_binary(NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "ik_init_default_binary", "test if(p_ik_buffer_info->init_flag == 1u) false case", id_cnt++);
    p_ik_buffer_info->init_flag = 1;
    img_arch_deinit_architecture();
}

static void IK_TestCovr_DefBinMemFenceCheckFail(void)
{
    UINT32 Rval = IK_OK;
    ik_ability_t ability = {0};
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    UINT32 offset;
    UINT8 *p_tmp = (UINT8 *)pBinDataAddr;
    extern uint32 ik_init_context(uint32 context_id, const ik_ability_t *p_ability);

    offset = (128U + (128U*9U)) + (146UL<<2) + (128U*4U);
    p_tmp += offset;
    *p_tmp = 0;
    Rval = ik_init_context(0, &ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "ik_init_context", "test ik_default_binary_init_check fail case", id_cnt++);

    img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    p_ik_buffer_info->safety_logic_enable = 0;
    p_ik_buffer_info->safety_state = IK_SAFETY_STATE_OK;
    Rval = ik_init_context(0, &ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "ik_init_context", "test if(p_ik_buffer_info->safety_logic_enable == 1u) false NG case", id_cnt++);
}

static void IK_Set_ImgFilters(const AMBA_IK_MODE_CFG_s *pMode0, const AMBA_IK_MODE_CFG_s *pMode1, char *p_desc_name)
{
    UINT32 Rval = IK_OK;

    {
        // Safety info
        ik_safety_info_t safety_info = {0};

        safety_info.update_freq = 1;
        Rval = ik_set_safety_info(pMode0->ContextId, &safety_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_safety_info", p_desc_name, id_cnt++);
    }

    {
        // Sensor info
        ik_vin_sensor_info_t sensor_info = {0};

        Rval = ik_set_vin_sensor_info(pMode0->ContextId, &sensor_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_vin_sensor_info", p_desc_name, id_cnt++);
    }

    {
        // YUV mode
        uint32 yuv_mode = 0;

        Rval = ik_set_yuv_mode(pMode1->ContextId, yuv_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_yuv_mode", p_desc_name, id_cnt++);
    }

    {
        // Before CE
        ik_wb_gain_t before_ce_wb_gain = {0};

        before_ce_wb_gain.gain_b = 4096;
        before_ce_wb_gain.gain_g = 4096;
        before_ce_wb_gain.gain_r = 4096;
        Rval = ik_set_before_ce_wb_gain(pMode0->ContextId, &before_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_before_ce_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // After CE
        ik_wb_gain_t after_ce_wb_gain = {0};

        after_ce_wb_gain.gain_b = 4096;
        after_ce_wb_gain.gain_g = 4096;
        after_ce_wb_gain.gain_r = 4096;
        Rval = ik_set_after_ce_wb_gain(pMode0->ContextId, &after_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_after_ce_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // CFA leakage
        ik_cfa_leakage_filter_t cfa_leakage_filter = {0};

        Rval = ik_set_cfa_leakage_filter(pMode0->ContextId, &cfa_leakage_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_cfa_leakage_filter", p_desc_name, id_cnt++);
    }

    {
        // Anti aliasing
        ik_anti_aliasing_t anti_aliasing = {0};

        Rval = ik_set_anti_aliasing(pMode0->ContextId, &anti_aliasing);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_anti_aliasing", p_desc_name, id_cnt++);
    }

    {
        // DBP
        ik_dynamic_bad_pixel_correction_t dynamic_bpc = {0};

        Rval = ik_set_dynamic_bad_pixel_corr(pMode0->ContextId, &dynamic_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_dynamic_bad_pixel_corr", p_desc_name, id_cnt++);
    }

    {
        // SBP
        uint32 enb = 0;
        ik_static_bad_pxl_cor_t static_bpc = {0};

        Rval = ik_set_static_bad_pxl_corr_enb(pMode0->ContextId, enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_static_bad_pxl_corr_enb", p_desc_name, id_cnt++);

        static_bpc.calib_sbp_info.version = 0x20180401;
        static_bpc.calib_sbp_info.sbp_buffer = FPNMap;
        static_bpc.vin_sensor_geo.width = 1920;
        static_bpc.vin_sensor_geo.height = 1080;
        static_bpc.vin_sensor_geo.h_sub_sample.factor_den = 1;
        static_bpc.vin_sensor_geo.h_sub_sample.factor_num = 1;
        static_bpc.vin_sensor_geo.v_sub_sample.factor_den = 1;
        static_bpc.vin_sensor_geo.v_sub_sample.factor_num = 1;
        Rval = ik_set_static_bad_pxl_corr(pMode0->ContextId, &static_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_static_bad_pxl_corr", p_desc_name, id_cnt++);
    }

    {
        // SBP internal
        ik_static_bad_pixel_correction_internal_t static_bpc_internal = {0};

        static_bpc_internal.p_map = FPNMap;
        Rval = ik_set_static_bad_pxl_corr_itnl(pMode0->ContextId, &static_bpc_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_static_bad_pxl_corr_itnl", p_desc_name, id_cnt++);
    }

    {
        // CFA noise
        ik_cfa_noise_filter_t cfa_noise_filter = {0};

        Rval = ik_set_cfa_noise_filter(pMode0->ContextId, &cfa_noise_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_cfa_noise_filter", p_desc_name, id_cnt++);
    }

    {
        // Demosaic
        ik_demosaic_t demosaic = {0};

        Rval = ik_set_demosaic(pMode0->ContextId, &demosaic);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_demosaic", p_desc_name, id_cnt++);
    }

    {
        // RGB_12Y
        ik_rgb_to_12y_t rgb_to_12y = {0};

        Rval = ik_set_rgb_to_12y(pMode0->ContextId, &rgb_to_12y);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_rgb_to_12y", p_desc_name, id_cnt++);
    }

    {
        // LNL
        ik_luma_noise_reduction_t luma_noise_reduce = {0};

        Rval = ik_set_luma_noise_reduction(pMode0->ContextId, &luma_noise_reduce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_luma_noise_reduction", p_desc_name, id_cnt++);
    }

    {
        // Pre CC gain
        ik_pre_cc_gain_t pre_cc_gain = {0};

        Rval = ik_set_pre_cc_gain(pMode0->ContextId, &pre_cc_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_pre_cc_gain", p_desc_name, id_cnt++);
    }

    {
        // CC
        ik_color_correction_t color_correction = {0};

        Rval = ik_set_color_correction(pMode0->ContextId, &color_correction);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_color_correction", p_desc_name, id_cnt++);
    }

    {
        // tone curve
        ik_tone_curve_t tone_curve = {0};

        Rval = ik_set_tone_curve(pMode0->ContextId, &tone_curve);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_tone_curve", p_desc_name, id_cnt++);
    }

    {
        // RGB to YUV
        ik_rgb_to_yuv_matrix_t rgb_to_yuv_matrix = {0};

        Rval = ik_set_rgb_to_yuv_matrix(pMode0->ContextId, &rgb_to_yuv_matrix);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_rgb_to_yuv_matrix", p_desc_name, id_cnt++);
    }

    {
        // RGBIR
        ik_rgb_ir_t rgb_ir = {0};

        Rval = ik_set_rgb_ir(pMode0->ContextId, &rgb_ir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_rgb_ir", p_desc_name, id_cnt++);
    }

    {
        // Chroma scale
        ik_chroma_scale_t chroma_scale = {0};

        Rval = ik_set_chroma_scale(pMode0->ContextId, &chroma_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_chroma_scale", p_desc_name, id_cnt++);
    }

    {
        // Chroma median
        ik_chroma_median_filter_t chroma_median_filter = {0};

        Rval = ik_set_chroma_median_filter(pMode0->ContextId, &chroma_median_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_chroma_median_filter", p_desc_name, id_cnt++);
    }

    {
        // SHPA proc mode
        ik_first_luma_process_mode_t first_luma_process_mode = {0};

        Rval = ik_set_fst_luma_process_mode(pMode0->ContextId, &first_luma_process_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fst_luma_process_mode", p_desc_name, id_cnt++);
    }

    {
        // ASF
        ik_adv_spatial_filter_t advance_spatial_filter = {0};

        Rval = ik_set_adv_spatial_filter(pMode0->ContextId, &advance_spatial_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_adv_spatial_filter", p_desc_name, id_cnt++);
    }

    {
        // SHPA
        ik_first_sharpen_both_t first_sharpen_both = {0};
        ik_first_sharpen_noise_t first_sharpen_noise = {0};
        ik_first_sharpen_fir_t first_sharpen_fir = {0};
        ik_first_sharpen_coring_t first_sharpen_coring = {0};
        ik_first_sharpen_coring_idx_scale_t first_sharpen_coring_idx_scale = {0};
        ik_first_sharpen_min_coring_result_t first_sharpen_min_coring_result = {0};
        ik_first_sharpen_max_coring_result_t first_sharpen_max_coring_result = {0};
        ik_first_sharpen_scale_coring_t first_sharpen_scale_coring = {0};

        Rval = ik_set_fst_shp_both(pMode0->ContextId, &first_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fst_shp_both", p_desc_name, id_cnt++);

        first_sharpen_noise.level_str_adjust.high = 100;
        Rval = ik_set_fst_shp_noise(pMode0->ContextId, &first_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fst_shp_noise", p_desc_name, id_cnt++);

        Rval = ik_set_fst_shp_fir(pMode0->ContextId, &first_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fst_shp_fir", p_desc_name, id_cnt++);

        first_sharpen_coring.fractional_bits = 2;
        Rval = ik_set_fst_shp_coring(pMode0->ContextId, &first_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fst_shp_coring", p_desc_name, id_cnt++);

        first_sharpen_coring_idx_scale.high = 100;
        Rval = ik_set_fst_shp_coring_idx_scale(pMode0->ContextId, &first_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fst_shp_coring_idx_scale", p_desc_name, id_cnt++);

        first_sharpen_min_coring_result.high = 100;
        Rval = ik_set_fst_shp_min_coring_rslt(pMode0->ContextId, &first_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fst_shp_min_coring_rslt", p_desc_name, id_cnt++);

        first_sharpen_max_coring_result.high = 100;
        Rval = ik_set_fst_shp_max_coring_rslt(pMode0->ContextId, &first_sharpen_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fst_shp_max_coring_rslt", p_desc_name, id_cnt++);

        first_sharpen_scale_coring.high = 100;
        Rval = ik_set_fst_shp_scale_coring(pMode0->ContextId, &first_sharpen_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fst_shp_scale_coring", p_desc_name, id_cnt++);
    }

    {
        // SHPB
        ik_final_sharpen_both_t final_sharpen_both = {0};
        ik_final_sharpen_noise_t final_sharpen_noise = {0};
        ik_final_sharpen_fir_t final_sharpen_fir = {0};
        ik_final_sharpen_coring_t final_sharpen_coring = {0};
        ik_final_sharpen_coring_idx_scale_t final_sharpen_coring_idx_scale = {0};
        ik_final_sharpen_min_coring_result_t final_sharpen_min_coring_result = {0};
        ik_final_sharpen_max_coring_result_t final_sharpen_max_coring_result = {0};
        ik_final_sharpen_scale_coring_t final_sharpen_scale_coring = {0};
        ik_final_sharpen_both_three_d_table_t final_sharpen_both_three_d_table = {0};

        Rval = ik_set_fnl_shp_both(pMode0->ContextId, &final_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fnl_shp_both", p_desc_name, id_cnt++);

        final_sharpen_noise.level_str_adjust.high = 100;
        Rval = ik_set_fnl_shp_noise(pMode0->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fnl_shp_noise", p_desc_name, id_cnt++);

        Rval = ik_set_fnl_shp_fir(pMode0->ContextId, &final_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fnl_shp_fir", p_desc_name, id_cnt++);

        final_sharpen_coring.fractional_bits = 2;
        Rval = ik_set_fnl_shp_coring(pMode0->ContextId, &final_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fnl_shp_coring", p_desc_name, id_cnt++);

        final_sharpen_coring_idx_scale.high = 100;
        Rval = ik_set_fnl_shp_coring_idx_scale(pMode0->ContextId, &final_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fnl_shp_coring_idx_scale", p_desc_name, id_cnt++);

        final_sharpen_min_coring_result.high = 100;
        Rval = ik_set_fnl_shp_min_coring_rslt(pMode0->ContextId, &final_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fnl_shp_min_coring_rslt", p_desc_name, id_cnt++);

        final_sharpen_max_coring_result.high = 100;
        Rval = ik_set_fnl_shp_max_coring_rslt(pMode0->ContextId, &final_sharpen_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fnl_shp_max_coring_rslt", p_desc_name, id_cnt++);

        final_sharpen_scale_coring.high = 100;
        Rval = ik_set_fnl_shp_scale_coring(pMode0->ContextId, &final_sharpen_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fnl_shp_scale_coring", p_desc_name, id_cnt++);

        Rval = ik_set_fnl_shp_three_d_table(pMode0->ContextId, &final_sharpen_both_three_d_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_fnl_shp_three_d_table", p_desc_name, id_cnt++);
    }

    {
        // Chroma filter
        ik_chroma_filter_t chroma_filter = {0};

        chroma_filter.radius = 32;
        Rval = ik_set_chroma_filter(pMode0->ContextId, &chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_chroma_filter", p_desc_name, id_cnt++);
    }

    {
        // Wide chroma filter
        ik_wide_chroma_filter_t wide_chroma_filter = {0};

        Rval = ik_set_wide_chroma_filter(pMode0->ContextId, &wide_chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_wide_chroma_filter", p_desc_name, id_cnt++);
    }

    {
        // Wide chroma filter combine
        ik_wide_chroma_filter_combine_t chroma_filter_combine = {0};

        chroma_filter_combine.T1_cb = 10;
        chroma_filter_combine.T1_cr = 10;
        Rval = ik_set_wide_chroma_filter_combine(pMode0->ContextId, &chroma_filter_combine);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_wide_chroma_filter_combine", p_desc_name, id_cnt++);
    }

    {
        // grgb mismatch
        ik_grgb_mismatch_t grgb_mismatch = {0};

        Rval = ik_set_grgb_mismatch(pMode0->ContextId, &grgb_mismatch);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_grgb_mismatch", p_desc_name, id_cnt++);
    }

    {
        // MCTF
        ik_video_mctf_t video_mctf = {0};
        ik_video_mctf_ta_t video_mctf_ta = {0};
        ik_pos_dep33_t video_mctf_and_final_sharpen = {0};

        Rval = ik_set_video_mctf(pMode0->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_video_mctf", p_desc_name, id_cnt++);

        Rval = ik_set_video_mctf_ta(pMode0->ContextId, &video_mctf_ta);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_video_mctf_ta", p_desc_name, id_cnt++);

        Rval = ik_set_video_mctf_and_fnl_shp(pMode0->ContextId, &video_mctf_and_final_sharpen);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_video_mctf_and_fnl_shp", p_desc_name, id_cnt++);
    }

    {
        // Vig
        uint32 enb = 0;
        ik_vignette_t vignette = {0};

        Rval = ik_set_vignette_enb(pMode0->ContextId, enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_vignette_enb", p_desc_name, id_cnt++);
        Rval = ik_set_vignette(pMode0->ContextId, &vignette);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_vignette", p_desc_name, id_cnt++);
    }

    {
        // HDR blc
        ik_static_blc_level_t frontend_static_blc = {0};

        Rval = ik_set_exp0_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_exp0_frontend_static_blc", p_desc_name, id_cnt++);
        Rval = ik_set_exp1_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_exp1_frontend_static_blc", p_desc_name, id_cnt++);
        Rval = ik_set_exp2_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_exp2_frontend_static_blc", p_desc_name, id_cnt++);
    }

    {
        // HDR gain
        ik_frontend_wb_gain_t frontend_wb_gain = {0};

        frontend_wb_gain.shutter_ratio = 4096;
        Rval = ik_set_exp0_frontend_wb_gain(pMode0->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_exp0_frontend_wb_gain", p_desc_name, id_cnt++);
        Rval = ik_set_exp1_frontend_wb_gain(pMode0->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_exp1_frontend_wb_gain", p_desc_name, id_cnt++);
        Rval = ik_set_exp2_frontend_wb_gain(pMode0->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_exp2_frontend_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // CE
        ik_ce_t ce = {0};
        ik_ce_input_table_t ce_input_table = {0};
        ik_ce_output_table_t ce_out_table = {0};
        ce.coring_gain_high = 1U;
        ce.coring_index_scale_high = 1U;

        Rval = ik_set_ce(pMode0->ContextId, &ce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_ce", p_desc_name, id_cnt++);
        Rval = ik_set_ce_input_table(pMode0->ContextId, &ce_input_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_ce_input_table", p_desc_name, id_cnt++);
        Rval = ik_set_ce_out_table(pMode0->ContextId, &ce_out_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_ce_out_table", p_desc_name, id_cnt++);
    }

    {
        // HDR blend
        ik_hdr_blend_t hdr_blend = {0};

        Rval = ik_set_hdr_blend(pMode0->ContextId, &hdr_blend);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_hdr_blend", p_desc_name, id_cnt++);
    }

    {
        // HDR tone curve
        ik_frontend_tone_curve_t fe_tc = {0};

        Rval = ik_set_frontend_tone_curve(pMode0->ContextId, &fe_tc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_frontend_tone_curve", p_desc_name, id_cnt++);
    }

    {
        // resampler strength
        ik_resampler_strength_t resampler_strength = {0};

        Rval = ik_set_resampler_strength(pMode0->ContextId, &resampler_strength);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_resampler_strength", p_desc_name, id_cnt++);
    }

    {
        // AAA
        ik_aaa_stat_info_t stat_info = {0};
        ik_aaa_pg_af_stat_info_t pg_af_stat_info = {0};
        ik_af_stat_ex_info_t af_stat_ex_info = {0};
        ik_pg_af_stat_ex_info_t pg_af_stat_ex_info = {0};
        ik_histogram_info_t hist_info = {0};

        stat_info.awb_tile_num_col = 64;
        stat_info.awb_tile_num_row = 64;
        stat_info.ae_tile_num_col = 24;
        stat_info.ae_tile_num_row = 16;
        stat_info.af_tile_num_col = 24;
        stat_info.af_tile_num_row = 16;
        pg_af_stat_info.af_tile_num_col = 32;
        pg_af_stat_info.af_tile_num_row = 16;
        Rval = ik_set_aaa_stat_info(pMode0->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_aaa_stat_info", p_desc_name, id_cnt++);
        Rval = ik_set_aaa_pg_af_stat_info(pMode0->ContextId, &pg_af_stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_aaa_pg_af_stat_info", p_desc_name, id_cnt++);
        Rval = ik_set_af_stat_ex_info(pMode0->ContextId, &af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_af_stat_ex_info", p_desc_name, id_cnt++);
        Rval = ik_set_pg_af_stat_ex_info(pMode0->ContextId, &pg_af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_pg_af_stat_ex_info", p_desc_name, id_cnt++);
        Rval = ik_set_histogram_info(pMode0->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_histogram_info", p_desc_name, id_cnt++);
        Rval = ik_set_pg_histogram_info(pMode0->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_pg_histogram_info", p_desc_name, id_cnt++);
    }

    {
        // Window info
        ik_window_size_info_t window_size_info = {0};

        window_size_info.vin_sensor.width = 1920;
        window_size_info.vin_sensor.height = 1080;
        window_size_info.vin_sensor.h_sub_sample.factor_den = 1;
        window_size_info.vin_sensor.h_sub_sample.factor_num = 1;
        window_size_info.vin_sensor.v_sub_sample.factor_den = 1;
        window_size_info.vin_sensor.v_sub_sample.factor_num = 1;
        window_size_info.main_win.width = 1920;
        window_size_info.main_win.height = 1080;
        Rval = ik_set_window_size_info(pMode0->ContextId, &window_size_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_window_size_info", p_desc_name, id_cnt++);
    }

    {
        // Warp
        uint32 enb = 0;
        ik_warp_info_t calib_warp_info = {0};

        Rval = ik_set_warp_enb(pMode0->ContextId, enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_warp_enb", p_desc_name, id_cnt++);

        calib_warp_info.version = 0x20180401;
        calib_warp_info.hor_grid_num = 128;
        calib_warp_info.ver_grid_num = 96;
        calib_warp_info.tile_width_exp = 4;
        calib_warp_info.tile_height_exp = 4;
        calib_warp_info.vin_sensor_geo.width = 1920;
        calib_warp_info.vin_sensor_geo.height = 1080;
        calib_warp_info.vin_sensor_geo.h_sub_sample.factor_den = 1;
        calib_warp_info.vin_sensor_geo.h_sub_sample.factor_num = 1;
        calib_warp_info.vin_sensor_geo.v_sub_sample.factor_den = 1;
        calib_warp_info.vin_sensor_geo.v_sub_sample.factor_num = 1;
        calib_warp_info.pwarp = (ik_grid_point_t *)WarpGrid;
        Rval = ik_set_warp_info(pMode0->ContextId, &calib_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_warp_info", p_desc_name, id_cnt++);
    }

    {
        // CA
        uint32 enb = 0;
        ik_cawarp_info_t calib_ca_warp_info = {0};

        Rval = ik_set_cawarp_enb(pMode0->ContextId, enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_cawarp_enb", p_desc_name, id_cnt++);

        calib_ca_warp_info.version = 0x20180401;
        calib_ca_warp_info.hor_grid_num = 32;
        calib_ca_warp_info.ver_grid_num = 48;
        calib_ca_warp_info.tile_width_exp = 6;
        calib_ca_warp_info.tile_height_exp = 5;
        calib_ca_warp_info.vin_sensor_geo.width = 1920;
        calib_ca_warp_info.vin_sensor_geo.height = 1080;
        calib_ca_warp_info.vin_sensor_geo.h_sub_sample.factor_den = 1;
        calib_ca_warp_info.vin_sensor_geo.h_sub_sample.factor_num = 1;
        calib_ca_warp_info.vin_sensor_geo.v_sub_sample.factor_den = 1;
        calib_ca_warp_info.vin_sensor_geo.v_sub_sample.factor_num = 1;
        calib_ca_warp_info.p_cawarp_red = (ik_grid_point_t *)CawarpRedGrid;
        calib_ca_warp_info.p_cawarp_blue = (ik_grid_point_t *)CawarpBlueGrid;
        Rval = ik_set_cawarp_info(pMode0->ContextId, &calib_ca_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_cawarp_info", p_desc_name, id_cnt++);
    }

    {
        // DZoom
        ik_dzoom_info_t dzoom_info = {0};

        Rval = ik_set_dzoom_info(pMode0->ContextId, &dzoom_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_dzoom_info", p_desc_name, id_cnt++);
    }

    {
        // Dummy
        ik_dummy_margin_range_t dmy_margin_range = {0};

        Rval = ik_set_dummy_margin_range(pMode0->ContextId, &dmy_margin_range);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_dummy_margin_range", p_desc_name, id_cnt++);
    }

    {
        // Active win
        ik_vin_active_window_t vin_active_win = {0};

        Rval = ik_set_vin_active_win(pMode0->ContextId, &vin_active_win);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_vin_active_win", p_desc_name, id_cnt++);
    }

    {
        // Warp internal
        ik_warp_internal_info_t warp_dzoom_internal = {0};

        warp_dzoom_internal.pwarp_horizontal_table = (int16 *)WarpGrid;
        warp_dzoom_internal.pwarp_vertical_table = (int16 *)(WarpGrid + (128*96*4));
        Rval = ik_set_warp_internal(pMode0->ContextId, &warp_dzoom_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_warp_internal", p_desc_name, id_cnt++);
    }

    {
        // CA internal
        ik_cawarp_internal_info_t cawarp_internal = {0};

        Rval = ik_set_cawarp_internal(pMode0->ContextId, &cawarp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_cawarp_internal", p_desc_name, id_cnt++);
    }

    {
        // HDR RAW info
        ik_hdr_raw_info_t hdr_raw_info = {0};

        Rval = ik_set_hdr_raw_offset(pMode0->ContextId, &hdr_raw_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_hdr_raw_offset", p_desc_name, id_cnt++);
    }

    {
        // flip mode
        uint32 mode = 0;

        Rval = ik_set_flip_mode(pMode0->ContextId, mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_flip_mode", p_desc_name, id_cnt++);
    }

    {
        // Warp buf info
        ik_warp_buffer_info_t warp_buf_info = {0};

        warp_buf_info.luma_wait_lines = 8;
        warp_buf_info.luma_dma_size = 32;
        Rval = ik_set_warp_buffer_info(pMode0->ContextId, &warp_buf_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_warp_buffer_info", p_desc_name, id_cnt++);
    }

    {
        // Stitch info
        ik_stitch_info_t stitch_info = {0};

        Rval = ik_set_stitching_info(pMode0->ContextId, &stitch_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_stitching_info", p_desc_name, id_cnt++);
    }

    {
        // Burst tile
        ik_burst_tile_t burst_tile = {0};

        Rval = ik_set_burst_tile(pMode0->ContextId, &burst_tile);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_burst_tile", p_desc_name, id_cnt++);
    }
}

static void IK_Get_ImgFilters(const AMBA_IK_MODE_CFG_s *pMode0, const AMBA_IK_MODE_CFG_s *pMode1, char *p_desc_name)
{
    UINT32 Rval = IK_OK;

    {
        // Safety info
        ik_safety_info_t safety_info = {0};

        Rval = ik_get_safety_info(pMode0->ContextId, &safety_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_safety_info", p_desc_name, id_cnt++);
    }

    {
        // Sensor info
        ik_vin_sensor_info_t sensor_info = {0};

        Rval = ik_get_vin_sensor_info(pMode0->ContextId, &sensor_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_vin_sensor_info", p_desc_name, id_cnt++);
    }

    {
        // YUV mode
        uint32 yuv_mode = 0;

        Rval = ik_get_yuv_mode(pMode1->ContextId, &yuv_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_yuv_mode", p_desc_name, id_cnt++);
    }

    {
        // Before CE
        ik_wb_gain_t before_ce_wb_gain = {0};

        Rval = ik_get_before_ce_wb_gain(pMode0->ContextId, &before_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_before_ce_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // After CE
        ik_wb_gain_t after_ce_wb_gain = {0};

        Rval = ik_get_after_ce_wb_gain(pMode0->ContextId, &after_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_after_ce_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // CFA leakage
        ik_cfa_leakage_filter_t cfa_leakage_filter = {0};

        Rval = ik_get_cfa_leakage_filter(pMode0->ContextId, &cfa_leakage_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_cfa_leakage_filter", p_desc_name, id_cnt++);
    }

    {
        // Anti aliasing
        ik_anti_aliasing_t anti_aliasing = {0};

        Rval = ik_get_anti_aliasing(pMode0->ContextId, &anti_aliasing);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_anti_aliasing", p_desc_name, id_cnt++);
    }

    {
        // DBP
        ik_dynamic_bad_pixel_correction_t dynamic_bpc = {0};

        Rval = ik_get_dynamic_bad_pixel_corr(pMode0->ContextId, &dynamic_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_dynamic_bad_pixel_corr", p_desc_name, id_cnt++);
    }

    {
        // SBP
        uint32 enb = 0;
        ik_static_bad_pxl_cor_t static_bpc = {0};

        Rval = ik_get_static_bad_pxl_corr_enb(pMode0->ContextId, &enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_static_bad_pxl_corr_enb", p_desc_name, id_cnt++);
        Rval = ik_get_static_bad_pxl_corr(pMode0->ContextId, &static_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_static_bad_pxl_corr", p_desc_name, id_cnt++);
    }

    {
        // SBP internal
        ik_static_bad_pixel_correction_internal_t static_bpc_internal = {0};

        Rval = ik_get_static_bad_pxl_corr_itnl(pMode0->ContextId, &static_bpc_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_static_bad_pxl_corr_itnl", p_desc_name, id_cnt++);
    }

    {
        // CFA noise
        ik_cfa_noise_filter_t cfa_noise_filter = {0};

        Rval = ik_get_cfa_noise_filter(pMode0->ContextId, &cfa_noise_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_cfa_noise_filter", p_desc_name, id_cnt++);
    }

    {
        // Demosaic
        ik_demosaic_t demosaic = {0};

        Rval = ik_get_demosaic(pMode0->ContextId, &demosaic);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_demosaic", p_desc_name, id_cnt++);
    }

    {
        // RGB_12Y
        ik_rgb_to_12y_t rgb_to_12y = {0};

        Rval = ik_get_rgb_to_12y(pMode0->ContextId, &rgb_to_12y);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_rgb_to_12y", p_desc_name, id_cnt++);
    }

    {
        // LNL
        ik_luma_noise_reduction_t luma_noise_reduce = {0};

        Rval = ik_get_luma_noise_reduction(pMode0->ContextId, &luma_noise_reduce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_luma_noise_reduction", p_desc_name, id_cnt++);
    }

    {
        // Pre CC gain
        ik_pre_cc_gain_t pre_cc_gain = {0};

        Rval = ik_get_pre_cc_gain(pMode0->ContextId, &pre_cc_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_pre_cc_gain", p_desc_name, id_cnt++);
    }

    {
        // CC
        ik_color_correction_t color_correction = {0};

        Rval = ik_get_color_correction(pMode0->ContextId, &color_correction);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_color_correction", p_desc_name, id_cnt++);
    }

    {
        // tone curve
        ik_tone_curve_t tone_curve = {0};

        Rval = ik_get_tone_curve(pMode0->ContextId, &tone_curve);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_tone_curve", p_desc_name, id_cnt++);
    }

    {
        // RGB to YUV
        ik_rgb_to_yuv_matrix_t rgb_to_yuv_matrix = {0};

        Rval = ik_get_rgb_to_yuv_matrix(pMode0->ContextId, &rgb_to_yuv_matrix);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_rgb_to_yuv_matrix", p_desc_name, id_cnt++);
    }

    {
        // RGBIR
        ik_rgb_ir_t rgb_ir = {0};

        Rval = ik_get_rgb_ir(pMode0->ContextId, &rgb_ir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_rgb_ir", p_desc_name, id_cnt++);
    }

    {
        // Chroma scale
        ik_chroma_scale_t chroma_scale = {0};

        Rval = ik_get_chroma_scale(pMode0->ContextId, &chroma_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_chroma_scale", p_desc_name, id_cnt++);
    }

    {
        // Chroma median
        ik_chroma_median_filter_t chroma_median_filter = {0};

        Rval = ik_get_chroma_median_filter(pMode0->ContextId, &chroma_median_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_chroma_median_filter", p_desc_name, id_cnt++);
    }

    {
        // SHPA proc mode
        ik_first_luma_process_mode_t first_luma_process_mode = {0};

        Rval = ik_get_fst_luma_process_mode(pMode0->ContextId, &first_luma_process_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_luma_process_mode", p_desc_name, id_cnt++);
    }

    {
        // ASF
        ik_adv_spatial_filter_t advance_spatial_filter = {0};

        Rval = ik_get_adv_spatial_filter(pMode0->ContextId, &advance_spatial_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_adv_spatial_filter", p_desc_name, id_cnt++);
    }

    {
        // SHPA
        ik_first_sharpen_both_t first_sharpen_both = {0};
        ik_first_sharpen_noise_t first_sharpen_noise = {0};
        ik_first_sharpen_fir_t first_sharpen_fir = {0};
        ik_first_sharpen_coring_t first_sharpen_coring = {0};
        ik_first_sharpen_coring_idx_scale_t first_sharpen_coring_idx_scale = {0};
        ik_first_sharpen_min_coring_result_t first_sharpen_min_coring_result = {0};
        ik_first_sharpen_max_coring_result_t first_sharpen_max_coring_result = {0};
        ik_first_sharpen_scale_coring_t first_sharpen_scale_coring = {0};

        Rval = ik_get_fst_shp_both(pMode0->ContextId, &first_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_shp_both", p_desc_name, id_cnt++);

        Rval = ik_get_fst_shp_noise(pMode0->ContextId, &first_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_shp_noise", p_desc_name, id_cnt++);

        Rval = ik_get_fst_shp_fir(pMode0->ContextId, &first_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_shp_fir", p_desc_name, id_cnt++);

        Rval = ik_get_fst_shp_coring(pMode0->ContextId, &first_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_shp_coring", p_desc_name, id_cnt++);

        Rval = ik_get_fst_shp_coring_idx_scale(pMode0->ContextId, &first_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_shp_coring_idx_scale", p_desc_name, id_cnt++);

        Rval = ik_get_fst_shp_min_coring_rslt(pMode0->ContextId, &first_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_shp_min_coring_rslt", p_desc_name, id_cnt++);

        Rval = ik_get_fst_shp_max_coring_rslt(pMode0->ContextId, &first_sharpen_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_shp_max_coring_rslt", p_desc_name, id_cnt++);

        Rval = ik_get_fst_shp_scale_coring(pMode0->ContextId, &first_sharpen_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_shp_scale_coring", p_desc_name, id_cnt++);
    }

    {
        // SHPB
        ik_final_sharpen_both_t final_sharpen_both = {0};
        ik_final_sharpen_noise_t final_sharpen_noise = {0};
        ik_final_sharpen_fir_t final_sharpen_fir = {0};
        ik_final_sharpen_coring_t final_sharpen_coring = {0};
        ik_final_sharpen_coring_idx_scale_t final_sharpen_coring_idx_scale = {0};
        ik_final_sharpen_min_coring_result_t final_sharpen_min_coring_result = {0};
        ik_final_sharpen_max_coring_result_t final_sharpen_max_coring_result = {0};
        ik_final_sharpen_scale_coring_t final_sharpen_scale_coring = {0};
        ik_final_sharpen_both_three_d_table_t final_sharpen_both_three_d_table = {0};

        Rval = ik_get_fnl_shp_both(pMode0->ContextId, &final_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_both", p_desc_name, id_cnt++);

        Rval = ik_get_fnl_shp_noise(pMode0->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_noise", p_desc_name, id_cnt++);

        Rval = ik_get_fnl_shp_fir(pMode0->ContextId, &final_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_fir", p_desc_name, id_cnt++);

        Rval = ik_get_fnl_shp_coring(pMode0->ContextId, &final_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_coring", p_desc_name, id_cnt++);

        Rval = ik_get_fnl_shp_coring_idx_scale(pMode0->ContextId, &final_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_coring_idx_scale", p_desc_name, id_cnt++);

        Rval = ik_get_fnl_shp_min_coring_rslt(pMode0->ContextId, &final_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_min_coring_rslt", p_desc_name, id_cnt++);

        Rval = ik_get_fnl_shp_max_coring_rslt(pMode0->ContextId, &final_sharpen_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_max_coring_rslt", p_desc_name, id_cnt++);

        Rval = ik_get_fnl_shp_scale_coring(pMode0->ContextId, &final_sharpen_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_scale_coring", p_desc_name, id_cnt++);

        Rval = ik_get_fnl_shp_three_d_table(pMode0->ContextId, &final_sharpen_both_three_d_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_three_d_table", p_desc_name, id_cnt++);
    }

    {
        // Chroma filter
        ik_chroma_filter_t chroma_filter = {0};

        Rval = ik_get_chroma_filter(pMode0->ContextId, &chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_chroma_filter", p_desc_name, id_cnt++);
    }

    {
        // Wide chroma filter
        ik_wide_chroma_filter_t wide_chroma_filter = {0};

        Rval = ik_get_wide_chroma_filter(pMode0->ContextId, &wide_chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_wide_chroma_filter", p_desc_name, id_cnt++);
    }

    {
        // Wide chroma filter combine
        ik_wide_chroma_filter_combine_t chroma_filter_combine = {0};

        Rval = ik_get_wide_chroma_filter_combine(pMode0->ContextId, &chroma_filter_combine);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_wide_chroma_filter_combine", p_desc_name, id_cnt++);
    }

    {
        // grgb mismatch
        ik_grgb_mismatch_t grgb_mismatch = {0};

        Rval = ik_get_grgb_mismatch(pMode0->ContextId, &grgb_mismatch);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_grgb_mismatch", p_desc_name, id_cnt++);
    }

    {
        // MCTF
        ik_video_mctf_t video_mctf = {0};
        ik_video_mctf_ta_t video_mctf_ta = {0};
        ik_pos_dep33_t video_mctf_and_final_sharpen = {0};

        Rval = ik_get_video_mctf(pMode0->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_video_mctf", p_desc_name, id_cnt++);

        Rval = ik_get_video_mctf_ta(pMode0->ContextId, &video_mctf_ta);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_video_mctf_ta", p_desc_name, id_cnt++);

        Rval = ik_get_video_mctf_and_fnl_shp(pMode0->ContextId, &video_mctf_and_final_sharpen);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_video_mctf_and_fnl_shp", p_desc_name, id_cnt++);
    }

    {
        // Vig
        uint32 enb = 0;
        ik_vignette_t vignette = {0};

        Rval = ik_get_vignette_enb(pMode0->ContextId, &enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_vignette_enb", p_desc_name, id_cnt++);
        Rval = ik_get_vignette(pMode0->ContextId, &vignette);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_vignette", p_desc_name, id_cnt++);
    }

    {
        // HDR blc
        ik_static_blc_level_t frontend_static_blc = {0};

        Rval = ik_get_exp0_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_exp0_frontend_static_blc", p_desc_name, id_cnt++);
        Rval = ik_get_exp1_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_exp1_frontend_static_blc", p_desc_name, id_cnt++);
        Rval = ik_get_exp2_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_exp2_frontend_static_blc", p_desc_name, id_cnt++);
    }

    {
        // HDR gain
        ik_frontend_wb_gain_t frontend_wb_gain = {0};

        Rval = ik_get_exp0_frontend_wb_gain(pMode0->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_exp0_frontend_wb_gain", p_desc_name, id_cnt++);
        Rval = ik_get_exp1_frontend_wb_gain(pMode0->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_exp1_frontend_wb_gain", p_desc_name, id_cnt++);
        Rval = ik_get_exp2_frontend_wb_gain(pMode0->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_exp2_frontend_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // CE
        ik_ce_t ce = {0};
        ik_ce_input_table_t ce_input_table = {0};
        ik_ce_output_table_t ce_out_table = {0};

        Rval = ik_get_ce(pMode0->ContextId, &ce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_ce", p_desc_name, id_cnt++);
        Rval = ik_get_ce_input_table(pMode0->ContextId, &ce_input_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_ce_input_table", p_desc_name, id_cnt++);
        Rval = ik_get_ce_out_table(pMode0->ContextId, &ce_out_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_ce_out_table", p_desc_name, id_cnt++);
    }

    {
        // HDR blend
        ik_hdr_blend_t hdr_blend = {0};

        Rval = ik_get_hdr_blend(pMode0->ContextId, &hdr_blend);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_hdr_blend", p_desc_name, id_cnt++);
    }

    {
        // HDR tone curve
        ik_frontend_tone_curve_t fe_tc = {0};

        Rval = ik_get_frontend_tone_curve(pMode0->ContextId, &fe_tc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_frontend_tone_curve", p_desc_name, id_cnt++);
    }

    {
        // resampler strength
        ik_resampler_strength_t resampler_strength = {0};

        Rval = ik_get_resampler_strength(pMode0->ContextId, &resampler_strength);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_resampler_strength", p_desc_name, id_cnt++);
    }

    {
        // AAA
        ik_aaa_stat_info_t stat_info = {0};
        ik_aaa_pg_af_stat_info_t pg_af_stat_info = {0};
        ik_af_stat_ex_info_t af_stat_ex_info = {0};
        ik_pg_af_stat_ex_info_t pg_af_stat_ex_info = {0};
        ik_histogram_info_t hist_info = {0};

        Rval = ik_get_aaa_stat_info(pMode0->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_aaa_stat_info", p_desc_name, id_cnt++);
        Rval = ik_get_aaa_pg_af_stat_info(pMode0->ContextId, &pg_af_stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_aaa_pg_af_stat_info", p_desc_name, id_cnt++);
        Rval = ik_get_af_stat_ex_info(pMode0->ContextId, &af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_af_stat_ex_info", p_desc_name, id_cnt++);
        Rval = ik_get_pg_af_stat_ex_info(pMode0->ContextId, &pg_af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_pg_af_stat_ex_info", p_desc_name, id_cnt++);
        Rval = ik_get_histogram_info(pMode0->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_histogram_info", p_desc_name, id_cnt++);
        Rval = ik_get_pg_histogram_info(pMode0->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_pg_histogram_info", p_desc_name, id_cnt++);
    }

    {
        // Window info
        ik_window_size_info_t window_size_info = {0};

        Rval = ik_get_window_size_info(pMode0->ContextId, &window_size_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_window_size_info", p_desc_name, id_cnt++);
    }

    {
        // Warp
        uint32 enb = 0;
        ik_warp_info_t calib_warp_info = {0};

        Rval = ik_get_warp_enb(pMode0->ContextId, &enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_warp_enb", p_desc_name, id_cnt++);

        Rval = ik_get_warp_info(pMode0->ContextId, &calib_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_warp_info", p_desc_name, id_cnt++);
    }

    {
        // CA
        uint32 enb = 0;
        ik_cawarp_info_t calib_ca_warp_info = {0};

        Rval = ik_get_cawarp_enb(pMode0->ContextId, &enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_cawarp_enb", p_desc_name, id_cnt++);

        Rval = ik_get_cawarp_info(pMode0->ContextId, &calib_ca_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_cawarp_info", p_desc_name, id_cnt++);
    }

    {
        // DZoom
        ik_dzoom_info_t dzoom_info = {0};

        Rval = ik_get_dzoom_info(pMode0->ContextId, &dzoom_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_dzoom_info", p_desc_name, id_cnt++);
    }

    {
        // Dummy
        ik_dummy_margin_range_t dmy_margin_range = {0};

        Rval = ik_get_dummy_margin_range(pMode0->ContextId, &dmy_margin_range);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_dummy_margin_range", p_desc_name, id_cnt++);
    }

    {
        // Active win
        ik_vin_active_window_t vin_active_win = {0};

        Rval = ik_get_vin_active_win(pMode0->ContextId, &vin_active_win);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_vin_active_win", p_desc_name, id_cnt++);
    }

    {
        // Warp internal
        ik_warp_internal_info_t warp_dzoom_internal = {0};

        Rval = ik_get_warp_internal(pMode0->ContextId, &warp_dzoom_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_warp_internal", p_desc_name, id_cnt++);
    }

    {
        // CA internal
        ik_cawarp_internal_info_t cawarp_internal = {0};

        Rval = ik_get_cawarp_internal(pMode0->ContextId, &cawarp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_cawarp_internal", p_desc_name, id_cnt++);
    }

    {
        // HDR RAW info
        ik_hdr_raw_info_t hdr_raw_info = {0};

        Rval = ik_get_hdr_raw_offset(pMode0->ContextId, &hdr_raw_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_hdr_raw_offset", p_desc_name, id_cnt++);
    }

    {
        // flip mode
        uint32 mode = 0;

        Rval = ik_get_flip_mode(pMode0->ContextId, &mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_flip_mode", p_desc_name, id_cnt++);
    }

    {
        // Warp buf info
        ik_warp_buffer_info_t warp_buf_info = {0};

        Rval = ik_get_warp_buffer_info(pMode0->ContextId, &warp_buf_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_warp_buffer_info", p_desc_name, id_cnt++);
    }

    {
        // Stitch info
        ik_stitch_info_t stitch_info = {0};

        Rval = ik_get_stitching_info(pMode0->ContextId, &stitch_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_stitching_info", p_desc_name, id_cnt++);
    }

    {
        // Burst tile
        ik_burst_tile_t burst_tile = {0};

        Rval = ik_get_burst_tile(pMode0->ContextId, &burst_tile);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_burst_tile", p_desc_name, id_cnt++);
    }
}

static void IK_Get_ImgFilters_NullPtr(const AMBA_IK_MODE_CFG_s *pMode0, const AMBA_IK_MODE_CFG_s *pMode1, char *p_desc_name)
{
    UINT32 Rval = IK_OK;

    {
        // Safety info
        Rval = ik_get_safety_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_safety_info", p_desc_name, id_cnt++);
    }

    {
        // Sensor info
        Rval = ik_get_vin_sensor_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_vin_sensor_info", p_desc_name, id_cnt++);
    }

    {
        // YUV mode
        Rval = ik_get_yuv_mode(pMode1->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_yuv_mode", p_desc_name, id_cnt++);
    }

    {
        // Before CE
        Rval = ik_get_before_ce_wb_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_before_ce_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // After CE
        Rval = ik_get_after_ce_wb_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_after_ce_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // CFA leakage
        Rval = ik_get_cfa_leakage_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_cfa_leakage_filter", p_desc_name, id_cnt++);
    }

    {
        // Anti aliasing
        Rval = ik_get_anti_aliasing(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_anti_aliasing", p_desc_name, id_cnt++);
    }

    {
        // DBP
        Rval = ik_get_dynamic_bad_pixel_corr(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_dynamic_bad_pixel_corr", p_desc_name, id_cnt++);
    }

    {
        // SBP
        Rval = ik_get_static_bad_pxl_corr_enb(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_static_bad_pxl_corr_enb", p_desc_name, id_cnt++);
        Rval = ik_get_static_bad_pxl_corr(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_static_bad_pxl_corr", p_desc_name, id_cnt++);
    }

    {
        // SBP internal
        Rval = ik_get_static_bad_pxl_corr_itnl(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_static_bad_pxl_corr_itnl", p_desc_name, id_cnt++);
    }

    {
        // CFA noise
        Rval = ik_get_cfa_noise_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_cfa_noise_filter", p_desc_name, id_cnt++);
    }

    {
        // Demosaic
        Rval = ik_get_demosaic(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_demosaic", p_desc_name, id_cnt++);
    }

    {
        // RGB_12Y
        Rval = ik_get_rgb_to_12y(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_rgb_to_12y", p_desc_name, id_cnt++);
    }

    {
        // LNL
        Rval = ik_get_luma_noise_reduction(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_luma_noise_reduction", p_desc_name, id_cnt++);
    }

    {
        // Pre CC gain
        Rval = ik_get_pre_cc_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_pre_cc_gain", p_desc_name, id_cnt++);
    }

    {
        // CC
        Rval = ik_get_color_correction(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_color_correction", p_desc_name, id_cnt++);
    }

    {
        // tone curve
        Rval = ik_get_tone_curve(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_tone_curve", p_desc_name, id_cnt++);
    }

    {
        // RGB to YUV
        Rval = ik_get_rgb_to_yuv_matrix(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_rgb_to_yuv_matrix", p_desc_name, id_cnt++);
    }

    {
        // RGBIR
        Rval = ik_get_rgb_ir(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_rgb_ir", p_desc_name, id_cnt++);
    }

    {
        // Chroma scale
        Rval = ik_get_chroma_scale(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_chroma_scale", p_desc_name, id_cnt++);
    }

    {
        // Chroma median
        Rval = ik_get_chroma_median_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_chroma_median_filter", p_desc_name, id_cnt++);
    }

    {
        // SHPA proc mode
        Rval = ik_get_fst_luma_process_mode(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_luma_process_mode", p_desc_name, id_cnt++);
    }

    {
        // ASF
        Rval = ik_get_adv_spatial_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_adv_spatial_filter", p_desc_name, id_cnt++);
    }

    {
        // SHPA
        Rval = ik_get_fst_shp_both(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_shp_both", p_desc_name, id_cnt++);

        Rval = ik_get_fst_shp_noise(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_shp_noise", p_desc_name, id_cnt++);

        Rval = ik_get_fst_shp_fir(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_shp_fir", p_desc_name, id_cnt++);

        Rval = ik_get_fst_shp_coring(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_shp_coring", p_desc_name, id_cnt++);

        Rval = ik_get_fst_shp_coring_idx_scale(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_shp_coring_idx_scale", p_desc_name, id_cnt++);

        Rval = ik_get_fst_shp_min_coring_rslt(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_shp_min_coring_rslt", p_desc_name, id_cnt++);

        Rval = ik_get_fst_shp_max_coring_rslt(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_shp_max_coring_rslt", p_desc_name, id_cnt++);

        Rval = ik_get_fst_shp_scale_coring(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fst_shp_scale_coring", p_desc_name, id_cnt++);
    }

    {
        // SHPB
        Rval = ik_get_fnl_shp_both(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_both", p_desc_name, id_cnt++);

        Rval = ik_get_fnl_shp_noise(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_noise", p_desc_name, id_cnt++);

        Rval = ik_get_fnl_shp_fir(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_fir", p_desc_name, id_cnt++);

        Rval = ik_get_fnl_shp_coring(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_coring", p_desc_name, id_cnt++);

        Rval = ik_get_fnl_shp_coring_idx_scale(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_coring_idx_scale", p_desc_name, id_cnt++);

        Rval = ik_get_fnl_shp_min_coring_rslt(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_min_coring_rslt", p_desc_name, id_cnt++);

        Rval = ik_get_fnl_shp_max_coring_rslt(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_max_coring_rslt", p_desc_name, id_cnt++);

        Rval = ik_get_fnl_shp_scale_coring(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_scale_coring", p_desc_name, id_cnt++);

        Rval = ik_get_fnl_shp_three_d_table(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_fnl_shp_three_d_table", p_desc_name, id_cnt++);
    }

    {
        // Chroma filter
        Rval = ik_get_chroma_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_chroma_filter", p_desc_name, id_cnt++);
    }

    {
        // Wide chroma filter
        Rval = ik_get_wide_chroma_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_wide_chroma_filter", p_desc_name, id_cnt++);
    }

    {
        // Wide chroma filter combine
        Rval = ik_get_wide_chroma_filter_combine(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_wide_chroma_filter_combine", p_desc_name, id_cnt++);
    }

    {
        // grgb mismatch
        Rval = ik_get_grgb_mismatch(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_grgb_mismatch", p_desc_name, id_cnt++);
    }

    {
        // MCTF
        Rval = ik_get_video_mctf(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_video_mctf", p_desc_name, id_cnt++);

        Rval = ik_get_video_mctf_ta(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_video_mctf_ta", p_desc_name, id_cnt++);

        Rval = ik_get_video_mctf_and_fnl_shp(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_video_mctf_and_fnl_shp", p_desc_name, id_cnt++);
    }

    {
        // Vig
        Rval = ik_get_vignette_enb(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_vignette_enb", p_desc_name, id_cnt++);
        Rval = ik_get_vignette(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_vignette", p_desc_name, id_cnt++);
    }

    {
        // HDR blc
        Rval = ik_get_exp0_frontend_static_blc(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_exp0_frontend_static_blc", p_desc_name, id_cnt++);
        Rval = ik_get_exp1_frontend_static_blc(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_exp1_frontend_static_blc", p_desc_name, id_cnt++);
        Rval = ik_get_exp2_frontend_static_blc(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_exp2_frontend_static_blc", p_desc_name, id_cnt++);
    }

    {
        // HDR gain
        Rval = ik_get_exp0_frontend_wb_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_exp0_frontend_wb_gain", p_desc_name, id_cnt++);
        Rval = ik_get_exp1_frontend_wb_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_exp1_frontend_wb_gain", p_desc_name, id_cnt++);
        Rval = ik_get_exp2_frontend_wb_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_exp2_frontend_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // CE
        Rval = ik_get_ce(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_ce", p_desc_name, id_cnt++);
        Rval = ik_get_ce_input_table(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_ce_input_table", p_desc_name, id_cnt++);
        Rval = ik_get_ce_out_table(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_ce_out_table", p_desc_name, id_cnt++);
    }

    {
        // HDR blend
        Rval = ik_get_hdr_blend(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_hdr_blend", p_desc_name, id_cnt++);
    }

    {
        // HDR tone curve
        Rval = ik_get_frontend_tone_curve(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_frontend_tone_curve", p_desc_name, id_cnt++);
    }

    {
        // resampler strength
        Rval = ik_get_resampler_strength(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_resampler_strength", p_desc_name, id_cnt++);
    }

    {
        // AAA
        Rval = ik_get_aaa_stat_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_aaa_stat_info", p_desc_name, id_cnt++);
        Rval = ik_get_aaa_pg_af_stat_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_aaa_pg_af_stat_info", p_desc_name, id_cnt++);
        Rval = ik_get_af_stat_ex_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_af_stat_ex_info", p_desc_name, id_cnt++);
        Rval = ik_get_pg_af_stat_ex_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_pg_af_stat_ex_info", p_desc_name, id_cnt++);
        Rval = ik_get_histogram_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_histogram_info", p_desc_name, id_cnt++);
        Rval = ik_get_pg_histogram_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_pg_histogram_info", p_desc_name, id_cnt++);
    }

    {
        // Window info
        Rval = ik_get_window_size_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_window_size_info", p_desc_name, id_cnt++);
    }

    {
        // Warp
        Rval = ik_get_warp_enb(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_warp_enb", p_desc_name, id_cnt++);

        Rval = ik_get_warp_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_warp_info", p_desc_name, id_cnt++);
    }

    {
        // CA
        Rval = ik_get_cawarp_enb(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_cawarp_enb", p_desc_name, id_cnt++);

        Rval = ik_get_cawarp_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_cawarp_info", p_desc_name, id_cnt++);
    }

    {
        // DZoom
        Rval = ik_get_dzoom_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_dzoom_info", p_desc_name, id_cnt++);
    }

    {
        // Dummy
        Rval = ik_get_dummy_margin_range(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_dummy_margin_range", p_desc_name, id_cnt++);
    }

    {
        // Active win
        Rval = ik_get_vin_active_win(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_vin_active_win", p_desc_name, id_cnt++);
    }

    {
        // Warp internal
        Rval = ik_get_warp_internal(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_warp_internal", p_desc_name, id_cnt++);
    }

    {
        // CA internal
        Rval = ik_get_cawarp_internal(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_cawarp_internal", p_desc_name, id_cnt++);
    }

    {
        // HDR RAW info
        Rval = ik_get_hdr_raw_offset(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_hdr_raw_offset", p_desc_name, id_cnt++);
    }

    {
        // flip mode
        Rval = ik_get_flip_mode(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_flip_mode", p_desc_name, id_cnt++);
    }

    {
        // Warp buf info
        Rval = ik_get_warp_buffer_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_warp_buffer_info", p_desc_name, id_cnt++);
    }

    {
        // Stitch info
        Rval = ik_get_stitching_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_stitching_info", p_desc_name, id_cnt++);
    }

    {
        // Burst tile
        Rval = ik_get_burst_tile(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_get_burst_tile", p_desc_name, id_cnt++);
    }
}

static void IK_TestCovr_ImgFilters(AMBA_IK_MODE_CFG_s *pMode0, AMBA_IK_MODE_CFG_s *pMode1)
{
    UINT32 Rval = IK_OK;
    amba_ik_context_entity_t *p_ctx0 = NULL;
    amba_ik_context_entity_t *p_ctx1 = NULL;

    img_ctx_get_context(pMode0->ContextId, &p_ctx0);
    img_ctx_get_context(pMode1->ContextId, &p_ctx1);

    ik_init_debug_print_func();
    IK_Set_ImgFilters(pMode0, pMode1, "test if(p_print_fun != NULL) false case");
    IK_Get_ImgFilters(pMode0, pMode1, "test get ctx OK case");

    ik_init_debug_check_func();
    IK_Set_ImgFilters(pMode0, pMode1, "test if(p_check_func != NULL) false case");

    pMode0->ContextId = 33;
    pMode1->ContextId = 33;
    IK_Set_ImgFilters(pMode0, pMode1, "test get ctx fail case");
    IK_Get_ImgFilters(pMode0, pMode1, "test get ctx fail case");
    pMode0->ContextId = 0;
    pMode1->ContextId = 1;

    p_ctx0->organization.initial_flag = 0;
    p_ctx1->organization.initial_flag = 0;
    IK_Set_ImgFilters(pMode0, pMode1, "test p_ctx->organization.initial_flag false case");
    IK_Get_ImgFilters(pMode0, pMode1, "test p_ctx->organization.initial_flag false case");
    p_ctx0->organization.initial_flag = 1;
    p_ctx1->organization.initial_flag = 1;

    IK_Get_ImgFilters_NullPtr(pMode0, pMode1, "test IK filter parameters Null-ptr fail case");

    Rval = ik_set_aaa_stat_info(pMode0->ContextId, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "ik_set_aaa_stat_info", "test Null-ptr fail case", id_cnt++);
}

static void IK_TestCovr_Misc(AMBA_IK_MODE_CFG_s *pMode0)
{
    UINT32 Rval = IK_OK;
    void *Old, *New;

    {
        // Burst tile
        AMBA_IK_BURST_TILE_s BurstTile = {0};

        Rval = AmbaIK_SetBurstTile(pMode0, &BurstTile);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetBurstTile", "test (p_ik_buffer_info->safety_enable == 1u) OK case", id_cnt++);

        Rval = AmbaIK_GetBurstTile(pMode0, &BurstTile);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetBurstTile", "test AmbaIK_GetBurstTile OK case", id_cnt++);
    }

    {
        // Pre CC gain
        AMBA_IK_PRE_CC_GAIN_s PreCcGain = {0};

        ik_hook_debug_print();
        Rval = AmbaIK_SetDebugLogId(LOG_PRE_CC_GAIN, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgDebugPrint.c", "amba_ik_debug_print_pre_cc_gain", "test if (filter_debug_log[LOG_PRE_CC_GAIN] == 1U) true OK case", id_cnt++);

        Rval = AmbaIK_SetPreCcGain(pMode0, &PreCcGain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_SetPreCcGain", "test (p_ik_buffer_info->safety_enable == 1u) OK case", id_cnt++);

        Rval = AmbaIK_GetPreCcGain(pMode0, &PreCcGain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImageFilters.c", "AmbaIK_GetPreCcGain", "test AmbaIK_GetPreCcGain OK case", id_cnt++);

        ik_init_debug_print_func();
    }

    AmbaMisra_TypeCast64(&New, &Old);
}

INT32 ik_ctest_main(void)
{
    UINT32 Rval = IK_OK;
    AMBA_IK_ABILITY_s Ability[2];
    AMBA_IK_MODE_CFG_s Mode[2];

    memset(ik_working_buffer, 0, sizeof(ik_working_buffer));
    memset(WarpGrid, 0, sizeof(WarpGrid));
    memset(CawarpRedGrid, 0, sizeof(CawarpRedGrid));
    memset(CawarpBlueGrid, 0, sizeof(CawarpBlueGrid));
    memset(FPNMap, 0, sizeof(FPNMap));
    id_cnt = 0;

    pFile = fopen("data/ik_ctest.txt", "w");

    // init IK
    _load_idsp_default_binary();
    Ability[0].Pipe = AMBA_IK_PIPE_VIDEO;
    Ability[0].VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_3;
    Ability[1].Pipe = AMBA_IK_PIPE_VIDEO;
    Ability[1].VideoPipe = AMBA_IK_VIDEO_Y2Y;

    _Init_Image_Kernel(Ability);
    _Init_Image_Kernel_Core();

    Mode[0].ContextId = 0;
    Rval |= AmbaIK_InitContext(&Mode[0], &Ability[0]);

    Mode[1].ContextId = 1;
    Rval |= AmbaIK_InitContext(&Mode[1], &Ability[1]);

    // test
    ik_init_debug_print_func();
    ik_hook_debug_print();

    IK_TestCovr_Set_Filter_Fail(&Mode[0], &Mode[1]);
    IK_TestCovr_Set_Filter_Safety_Disable(&Mode[0], &Mode[1], Ability);
    ik_init_debug_print_func();
    _Reset_Image_Kernel(Ability);

    // test arch_init fail
    IK_TestCovr_Arch_Init_Fail(Ability);
    _Reset_Image_Kernel(Ability);

    // test default_binary_init fail
    IK_TestCovr_Def_Bin_Init_Fail();
    _Reset_Image_Kernel(Ability);

    // test ctx_init fail
    IK_TestCovr_Ctx_Init_Fail(&Mode[0], &Ability[0]);
    _Reset_Image_Kernel(Ability);

    // test ik_exe fail
    IK_TestCovr_IK_Exe_Fail(&Mode[0], Ability);
    _Reset_Image_Kernel(Ability);

    // test AmbaIK_InitPrintFunc fail
    IK_TestCovr_Print_Func_Fail();
    _Reset_Image_Kernel(Ability);

    // test AmbaIK_InitCheckParamFunc fail
    IK_TestCovr_Check_Func_Fail();
    _Reset_Image_Kernel(Ability);

    // test AmbaIK_QueryIdspClock
    IK_TestCovr_QueryIdspClock();

    // test group cmd query size
    IK_TestCovr_QueryCalcGeoBufSize();

    // test group cmd
    IK_TestCovr_CalcGeoSettings(&Mode[0]);

    // test CalcWarpResources
    IK_TestCovr_CalcWarpResources();

    // test QueryFrameInfo
    IK_TestCovr_QueryFrameInfo(&Mode[0]);

    // test InjectSaftyError
    IK_TestCovr_InjectSaftyError(&Mode[0], &Ability[0]);
    _Reset_Image_Kernel(Ability);

    // test SetDebugLogId
    IK_TestCovr_SetDebugLogId();

    // test AmbaDSP_ImgDebugUtility.c
    IK_TestCovr_Debug_Utility(&Mode[0]);
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgExecuter.c
    IK_TestCovr_ImgExecuter(&Mode[0]);
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgInit.c
    IK_TestCovr_ImgInit();
    _Reset_Image_Kernel(Ability);

    // test def bin mem fence check fail
    IK_TestCovr_DefBinMemFenceCheckFail();
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgFilters.c
    IK_TestCovr_ImgFilters(&Mode[0], &Mode[1]);
    _Reset_Image_Kernel(Ability);

    // test misc
    IK_TestCovr_Misc(&Mode[0]);
    _Reset_Image_Kernel(Ability);


    free((UINT8 *) pBinDataAddr);
    pBinDataAddr = NULL;
    fclose(pFile);

    fake_destroy_mutex(NULL);
    fake_clean_cache(NULL, 0);

    img_arch_deinit_architecture();
    iks_arch_deinit_architecture();

    return Rval;
}

