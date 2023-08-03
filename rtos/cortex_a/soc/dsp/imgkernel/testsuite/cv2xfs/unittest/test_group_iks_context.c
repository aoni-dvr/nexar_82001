#include "test_ik_global.h"
#include "AmbaDSP_ImgContextUtility.h"


#define IK_AMALGAM_TABLE_SBP_SIZE 3686400UL
#define ALIGN_N(x_, n_)   ((((x_)+((n_)-1)) & ~((n_)-1)))

static UINT8 ik_working_buffer[5<<21];
static void *pBinDataAddr;
FILE *pFile;
UINT32 id_cnt;
static UINT8 WarpGrid[4*256*192];
static UINT8 CawarpRedGrid[4*64*96];
static UINT8 CawarpBlueGrid[4*64*96];
static UINT8 FPNMap[IK_AMALGAM_TABLE_SBP_SIZE];


static INT32 fake_create_mutex(AMBA_KAL_MUTEX_t *mutex)
{
    return 0;
}

static INT32 fake_create_mutex_fail(AMBA_KAL_MUTEX_t *mutex)
{
    return -1;
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
    AMBA_IK_ABILITY_s Ability;
    AMBA_IK_MODE_CFG_s Mode;

    img_arch_deinit_architecture();
    iks_arch_deinit_architecture();

    memset(ik_working_buffer, 0, sizeof(ik_working_buffer));

    memcpy(&Ability, pAbility, sizeof(AMBA_IK_ABILITY_s));

    ContextSetting.ContextNumber = 1;
    ContextSetting.ConfigSetting[0].ConfigNumber = 5;
    ContextSetting.ConfigSetting[0].pAbility = &Ability;

    _reload_idsp_default_binary();
    AmbaIK_QueryArchMemorySize(&ContextSetting, &TotalCtxSize);
    AmbaIK_InitArch(&ContextSetting, (void*)ik_working_buffer, sizeof(ik_working_buffer));
    AmbaIK_InitDefBinary(pBinDataAddr);

    Mode.ContextId = 0;
    Rval |= AmbaIK_InitContext(&Mode, &Ability);
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

    ContextSetting.ContextNumber = 1;
    ContextSetting.ConfigSetting[0].ConfigNumber = 5;
    ContextSetting.ConfigSetting[0].pAbility = p_ability;

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

static void IKS_TestCovr_ImgContextComponentCalib(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;
    uint32 enable = 0;

    {
        // SBP
        ik_static_bad_pxl_cor_t static_bpc = {0};
        ik_static_bad_pixel_correction_internal_t static_bpc_internal = {0};

        Rval = iks_ctx_ivd_set_static_bpc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_static_bpc", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_static_bpc_internal(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_static_bpc_internal", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_static_bpc_enable_info(pMode->ContextId, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_static_bpc_enable_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_static_bpc_enable_info(pMode->ContextId, &enable);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_static_bpc_enable_info", "test if (p_enable != NULL) true case", id_cnt++);
        Rval = iks_ctx_ivd_get_static_bpc_enable_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_static_bpc_enable_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_static_bpc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_static_bpc", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_static_bpc(pMode->ContextId, &static_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_static_bpc", "test if (p_static_bpc != NULL) true case", id_cnt++);
        Rval = iks_ctx_ivd_get_static_bpc_internal(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_static_bpc_internal", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_static_bpc_internal(pMode->ContextId, &static_bpc_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_static_bpc_internal", "test if (p_static_bpc_internal != NULL) true case", id_cnt++);

        Rval = iks_ctx_set_static_bpc_enable_info(pMode->ContextId, 2);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_set_static_bpc_enable_info", "test if (enable > 1UL) fail case", id_cnt++);
    }

    {
        // Vig
        ik_vignette_t vignette = {0};

        Rval = iks_ctx_ivd_set_vignette(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_vignette", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_vignette_enable_info(pMode->ContextId, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_vignette_enable_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_vignette_enable_info(pMode->ContextId, &enable);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_vignette_enable_info", "test if (p_enable != NULL) true case", id_cnt++);
        Rval = iks_ctx_ivd_get_vignette_enable_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_vignette_enable_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_vignette(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_vignette", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_vignette(pMode->ContextId, &vignette);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_vignette", "test if (p_vignette != NULL) true case", id_cnt++);

        Rval = iks_ctx_set_vignette_enable_info(pMode->ContextId, 2);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_set_vignette_enable_info", "test if (enable > 1UL) fail case", id_cnt++);
    }

    {
        // Warp
        ik_warp_info_t calib_warp_info = {0};
        ik_warp_internal_info_t warp_internal = {0};

        Rval = iks_ctx_ivd_set_calib_warp_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_calib_warp_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_warp_enable_info(pMode->ContextId, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_warp_enable_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_calib_warp_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_calib_warp_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_calib_warp_info(pMode->ContextId, &calib_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_calib_warp_info", "test (p_calib_warp_info != NULL) true case", id_cnt++);
        Rval = iks_ctx_ivd_get_warp_enable_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_warp_enable_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_warp_enable_info(pMode->ContextId, &enable);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_warp_enable_info", "test (p_enable != NULL) true case", id_cnt++);
        Rval = iks_ctx_ivd_set_warp_internal(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_warp_internal", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_warp_internal(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_warp_internal", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_warp_internal(pMode->ContextId, &warp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_warp_internal", "test (p_warp_internal != NULL) true case", id_cnt++);

        Rval = iks_ctx_set_warp_enable_info(pMode->ContextId, 2);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_set_warp_enable_info", "test if (enable > 1UL) fail case", id_cnt++);
    }

    {
        // CA
        ik_cawarp_info_t calib_ca_warp_info = {0};
        ik_cawarp_internal_info_t cawarp_internal = {0};

        Rval = iks_ctx_ivd_set_calib_ca_warp_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_calib_ca_warp_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_cawarp_enable_info(pMode->ContextId, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_cawarp_enable_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_calib_ca_warp_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_calib_ca_warp_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_calib_ca_warp_info(pMode->ContextId, &calib_ca_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_calib_ca_warp_info", "test (p_calib_ca_warp_info != NULL) true case", id_cnt++);
        Rval = iks_ctx_ivd_get_cawarp_enable_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_cawarp_enable_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_cawarp_enable_info(pMode->ContextId, &enable);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_cawarp_enable_info", "test (p_enable != NULL) true case", id_cnt++);
        Rval = iks_ctx_ivd_set_cawarp_internal(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_cawarp_internal", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_cawarp_internal(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_cawarp_internal", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_cawarp_internal(pMode->ContextId, &cawarp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_cawarp_internal", "test (p_cawarp_internal != NULL) true case", id_cnt++);

        Rval = iks_ctx_set_cawarp_enable_info(pMode->ContextId, 2);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_set_cawarp_enable_info", "test if (enable > 1UL) fail case", id_cnt++);
    }

    {
        // DZoom
        ik_dzoom_info_t dzoom_info = {0};

        Rval = iks_ctx_ivd_set_dzoom_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_dzoom_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_dzoom_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_dzoom_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_dzoom_info(pMode->ContextId, &dzoom_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_dzoom_info", "test (p_dzoom_info != NULL) true case", id_cnt++);
    }

    {
        // Dummy
        ik_dummy_margin_range_t dmy_range = {0};

        Rval = iks_ctx_ivd_set_dummy_win_margin_range_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_dummy_win_margin_range_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_dummy_win_margin_range_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_dummy_win_margin_range_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_dummy_win_margin_range_info(pMode->ContextId, &dmy_range);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_dummy_win_margin_range_info", "test (dmy_range != NULL) true case", id_cnt++);
    }

    {
        // Flip mode
        uint32 mode = 0;

        Rval = iks_ctx_ivd_set_flip_mode(pMode->ContextId, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_flip_mode", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_flip_mode(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_flip_mode", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_flip_mode(pMode->ContextId, &mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_flip_mode", "test (mode != NULL) true case", id_cnt++);
    }

    {
        // Warp buffer info
        ik_warp_buffer_info_t warp_buf_info = {0};

        Rval = iks_ctx_ivd_set_warp_buffer_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_warp_buffer_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_warp_buffer_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_warp_buffer_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_warp_buffer_info(pMode->ContextId, &warp_buf_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_warp_buffer_info", "test (p_warp_buf_info != NULL) true case", id_cnt++);
    }

    {
        // Stitch info
        ik_stitch_info_t stitch_info = {0};

        Rval = iks_ctx_ivd_set_stitching_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_stitching_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_stitching_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_stitching_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_stitching_info(pMode->ContextId, &stitch_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_stitching_info", "test (p_stitch_info != NULL) true case", id_cnt++);
    }

    {
        // Active win
        ik_vin_active_window_t active_win = {0};

        Rval = iks_ctx_ivd_set_vin_active_window(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_vin_active_window", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_vin_active_window(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_vin_active_window", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_vin_active_window(pMode->ContextId, &active_win);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_vin_active_window", "test (p_active_win != NULL) true case", id_cnt++);
    }

    {
        // burst tile
        ik_burst_tile_t burst_tile = {0};

        Rval = iks_ctx_ivd_set_burst_tile(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_set_burst_tile", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_burst_tile(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_burst_tile", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_burst_tile(pMode->ContextId, &burst_tile);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_burst_tile", "test (p_burst_tile != NULL) true case", id_cnt++);
    }
}

static void IKS_TestCovr_ImgContextComponentHDR(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;

    {
        // Front-end BLC
        ik_static_blc_level_t frontend_static_blc = {0};

        Rval = iks_ctx_ivd_set_exp0_fe_static_blc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_set_exp0_fe_static_blc", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_exp1_fe_static_blc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_set_exp1_fe_static_blc", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_exp2_fe_static_blc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_set_exp2_fe_static_blc", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_exp0_fe_static_blc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_get_exp0_fe_static_blc", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_exp1_fe_static_blc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_get_exp1_fe_static_blc", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_exp2_fe_static_blc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_get_exp2_fe_static_blc", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_exp0_fe_static_blc(pMode->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_exp0_fe_static_blc", "test (p_exp0_frontend_static_blc != NULL) true case", id_cnt++);
        Rval = iks_ctx_ivd_get_exp1_fe_static_blc(pMode->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_exp1_fe_static_blc", "test (p_exp1_frontend_static_blc != NULL) true case", id_cnt++);
        Rval = iks_ctx_ivd_get_exp2_fe_static_blc(pMode->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_exp2_fe_static_blc", "test (p_exp2_frontend_static_blc != NULL) true case", id_cnt++);
    }

    {
        // Front-end wb gain
        ik_frontend_wb_gain_t frontend_wb_gain = {0};

        Rval = iks_ctx_ivd_set_exp0_fe_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_set_exp0_fe_wb_gain", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_exp1_fe_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_set_exp1_fe_wb_gain", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_exp2_fe_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_set_exp2_fe_wb_gain", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_exp0_fe_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_get_exp0_fe_wb_gain", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_exp1_fe_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_get_exp1_fe_wb_gain", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_exp2_fe_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_get_exp2_fe_wb_gain", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_exp0_fe_wb_gain(pMode->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_exp0_fe_wb_gain", "test (p_exp0_frontend_wb_gain != NULL) true case", id_cnt++);
        Rval = iks_ctx_ivd_get_exp1_fe_wb_gain(pMode->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_exp1_fe_wb_gain", "test (p_exp1_frontend_wb_gain != NULL) true case", id_cnt++);
        Rval = iks_ctx_ivd_get_exp2_fe_wb_gain(pMode->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_exp2_fe_wb_gain", "test (p_exp2_frontend_wb_gain != NULL) true case", id_cnt++);
    }

    {
        // Raw info
        ik_hdr_raw_info_t raw_info = {0};

        Rval = iks_ctx_ivd_set_hdr_raw_offset(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_set_hdr_raw_offset", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_hdr_raw_offset(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_get_hdr_raw_offset", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_hdr_raw_offset(pMode->ContextId, &raw_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_hdr_raw_offset", "test (p_raw_info != NULL) true case", id_cnt++);
    }

    {
        // CE
        ik_ce_t ce = {0};
        ik_ce_input_table_t ce_input_table = {0};
        ik_ce_output_table_t ce_out_table = {0};

        Rval = iks_ctx_ivd_set_ce(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_set_ce", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_ce_input_table(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_set_ce_input_table", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_ce_out_table(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_set_ce_out_table", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_ce(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_get_ce", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_ce_input_table(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_get_ce_input_table", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_ce_out_table(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_get_ce_out_table", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_ce(pMode->ContextId, &ce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_ce", "test (p_ce != NULL) true case", id_cnt++);
        Rval = iks_ctx_ivd_get_ce_input_table(pMode->ContextId, &ce_input_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_ce_input_table", "test (p_ce_input_table != NULL) true case", id_cnt++);
        Rval = iks_ctx_ivd_get_ce_out_table(pMode->ContextId, &ce_out_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_ce_out_table", "test (p_ce_out_table != NULL) true case", id_cnt++);
    }

    {
        // HDR blend
        ik_hdr_blend_t hdr_blend = {0};

        Rval = iks_ctx_ivd_set_hdr_blend(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_set_hdr_blend", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_hdr_blend(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "iks_ctx_ivd_get_hdr_blend", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_hdr_blend(pMode->ContextId, &hdr_blend);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_ivd_get_hdr_blend", "test (p_hdr_blend != NULL) true case", id_cnt++);
    }
}

static void IKS_TestCovr_ImgContextComponentNormalFilter(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;
    amba_iks_context_entity_t *p_ctx = NULL;

    iks_ctx_get_context(pMode->ContextId, &p_ctx);

    {
        // Safety info
        ik_safety_info_t safety_info = {0};

        Rval = iks_ctx_ivd_set_safety_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_safety_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_safety_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_safety_info", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_safety_info(pMode->ContextId, &safety_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_safety_info", "test if (p_safety_info==NULL) false case", id_cnt++);
    }

    {
        // Vin sensor info
        ik_vin_sensor_info_t sensor_info = {0};

        Rval = iks_ctx_ivd_set_vin_sensor_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_vin_sensor_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_vin_sensor_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_vin_sensor_info", "test invalid API fail case", id_cnt++);

        p_ctx->filters.update_flags.iso.is_1st_frame = 0;
        Rval = iks_ctx_set_vin_sensor_info(pMode->ContextId, &sensor_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_set_vin_sensor_info", "test if(p_ctx->filters.update_flags.iso.is_1st_frame == 0U) true case", id_cnt++);
        p_ctx->filters.update_flags.iso.is_1st_frame = 1;

        Rval = iks_ctx_ivd_get_vin_sensor_info(pMode->ContextId, &sensor_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_vin_sensor_info", "test if (p_sensor_info==NULL) false case", id_cnt++);
    }

    {
        // YUV mode
        uint32 yuv_mode = 0;

        Rval = iks_ctx_ivd_set_yuv_mode(pMode->ContextId, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_yuv_mode", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_yuv_mode(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_yuv_mode", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_yuv_mode(pMode->ContextId, &yuv_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_yuv_mode", "test if (p_yuv_mode != NULL) true case", id_cnt++);

        Rval = iks_ctx_set_yuv_mode(pMode->ContextId, 2);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "iks_ctx_set_yuv_mode", "test if (yuv_mode > 1UL) fail case", id_cnt++);
    }

    {
        // before CE
        ik_wb_gain_t before_ce_wb_gain = {0};

        Rval = iks_ctx_ivd_set_before_ce_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_before_ce_wb_gain", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_before_ce_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_before_ce_wb_gain", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_before_ce_wb_gain(pMode->ContextId, &before_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_before_ce_wb_gain", "test if (p_before_ce_wb_gain == NULL) false case", id_cnt++);
    }

    {
        // after CE
        ik_wb_gain_t after_ce_wb_gain = {0};

        Rval = iks_ctx_ivd_set_after_ce_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_after_ce_wb_gain", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_after_ce_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_after_ce_wb_gain", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_after_ce_wb_gain(pMode->ContextId, &after_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_after_ce_wb_gain", "test if (p_after_ce_wb_gain == NULL) false case", id_cnt++);
    }

    {
        // CFA leakage
        ik_cfa_leakage_filter_t cfa_leakage_filter = {0};

        Rval = iks_ctx_ivd_set_cfa_leakage_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_cfa_leakage_filter", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_cfa_leakage_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_cfa_leakage_filter", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_cfa_leakage_filter(pMode->ContextId, &cfa_leakage_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_cfa_leakage_filter", "test if (p_cfa_leakage_filter==NULL) false case", id_cnt++);
    }

    {
        // anti aliasing
        ik_anti_aliasing_t anti_aliasing = {0};

        Rval = iks_ctx_ivd_set_anti_aliasing(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_anti_aliasing", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_anti_aliasing(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_anti_aliasing", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_anti_aliasing(pMode->ContextId, &anti_aliasing);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_anti_aliasing", "test if (p_anti_aliasing == NULL) false case", id_cnt++);
    }

    {
        // DBP
        ik_dynamic_bad_pixel_correction_t dynamic_bpc = {0};

        Rval = iks_ctx_ivd_set_dynamic_bpc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_dynamic_bpc", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_dynamic_bpc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_dynamic_bpc", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_dynamic_bpc(pMode->ContextId, &dynamic_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_dynamic_bpc", "test if (p_dynamic_bpc == NULL) false case", id_cnt++);
    }

    {
        // CFA noise
        ik_cfa_noise_filter_t cfa_noise_filter = {0};

        Rval = iks_ctx_ivd_set_cfa_noise_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_cfa_noise_filter", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_cfa_noise_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_cfa_noise_filter", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_cfa_noise_filter(pMode->ContextId, &cfa_noise_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_cfa_noise_filter", "test if (p_cfa_noise_filter == NULL) false case", id_cnt++);
    }

    {
        // demosaic
        ik_demosaic_t demosaic = {0};

        Rval = iks_ctx_ivd_set_demosaic(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_demosaic", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_demosaic(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_demosaic", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_demosaic(pMode->ContextId, &demosaic);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_demosaic", "test if (p_demosaic == NULL) false case", id_cnt++);
    }

    {
        // rgb to 12y
        ik_rgb_to_12y_t rgb_to_12y = {0};

        Rval = iks_ctx_ivd_set_rgb_to_12y(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_rgb_to_12y", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_rgb_to_12y(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_rgb_to_12y", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_rgb_to_12y(pMode->ContextId, &rgb_to_12y);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_rgb_to_12y", "test if (p_rgb_to_12y == NULL) false case", id_cnt++);
    }

    {
        // LNL
        ik_luma_noise_reduction_t luma_noise_reduce = {0};

        Rval = iks_ctx_ivd_set_luma_noise_reduction(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_luma_noise_reduction", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_luma_noise_reduction(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_luma_noise_reduction", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_luma_noise_reduction(pMode->ContextId, &luma_noise_reduce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_luma_noise_reduction", "test if (p_luma_noise_reduce == NULL) false case", id_cnt++);
    }

    {
        // Pre CC gain
        ik_pre_cc_gain_t pre_cc_gain = {0};

        Rval = iks_ctx_ivd_set_pre_cc_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_pre_cc_gain", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_pre_cc_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_pre_cc_gain", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_pre_cc_gain(pMode->ContextId, &pre_cc_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_pre_cc_gain", "test if (p_pre_cc_gain == NULL) false case", id_cnt++);
    }

    {
        // CC
        ik_color_correction_t color_correction = {0};

        Rval = iks_ctx_ivd_set_color_correction(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_color_correction", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_color_correction(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_color_correction", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_color_correction(pMode->ContextId, &color_correction);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_color_correction", "test if (p_color_correction == NULL) false case", id_cnt++);
    }

    {
        // tone curve
        ik_tone_curve_t tone_curve = {0};

        Rval = iks_ctx_ivd_set_tone_curve(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_tone_curve", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_tone_curve(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_tone_curve", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_tone_curve(pMode->ContextId, &tone_curve);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_tone_curve", "test if (p_tone_curve == NULL) false case", id_cnt++);
    }

    {
        // rgb 2 yuv
        ik_rgb_to_yuv_matrix_t rgb_to_yuv_matrix = {0};

        Rval = iks_ctx_ivd_set_rgb_to_yuv_matrix(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_rgb_to_yuv_matrix", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_rgb_to_yuv_matrix(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_rgb_to_yuv_matrix", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_rgb_to_yuv_matrix(pMode->ContextId, &rgb_to_yuv_matrix);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_rgb_to_yuv_matrix", "test if (p_rgb_to_yuv_matrix == NULL) false case", id_cnt++);
    }

    {
        // rgbir
        ik_rgb_ir_t rgb_ir = {0};

        Rval = iks_ctx_ivd_set_rgb_ir(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_rgb_ir", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_rgb_ir(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_rgb_ir", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_rgb_ir(pMode->ContextId, &rgb_ir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_rgb_ir", "test if (p_rgb_ir == NULL) false case", id_cnt++);
    }

    {
        // chroma scale
        ik_chroma_scale_t chroma_scale = {0};

        Rval = iks_ctx_ivd_set_chroma_scale(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_chroma_scale", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_chroma_scale(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_chroma_scale", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_chroma_scale(pMode->ContextId, &chroma_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_chroma_scale", "test if (p_chroma_scale == NULL) false case", id_cnt++);
    }

    {
        // chroma median
        ik_chroma_median_filter_t chroma_median_filter = {0};

        Rval = iks_ctx_ivd_set_chroma_median_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_chroma_median_filter", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_chroma_median_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_chroma_median_filter", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_chroma_median_filter(pMode->ContextId, &chroma_median_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_chroma_median_filter", "test if (p_chroma_median_filter == NULL) false case", id_cnt++);
    }

    {
        // luma processing mode
        ik_first_luma_process_mode_t first_luma_process_mode = {0};

        Rval = iks_ctx_ivd_set_first_luma_processing_mode(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_first_luma_processing_mode", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_first_luma_processing_mode(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_first_luma_processing_mode", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_first_luma_processing_mode(pMode->ContextId, &first_luma_process_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_first_luma_processing_mode", "test if (p_first_luma_process_mode == NULL) false case", id_cnt++);
    }

    {
        // ASF
        ik_adv_spatial_filter_t advance_spatial_filter = {0};

        Rval = iks_ctx_ivd_set_advance_spatial_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_advance_spatial_filter", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_advance_spatial_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_advance_spatial_filter", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_advance_spatial_filter(pMode->ContextId, &advance_spatial_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_advance_spatial_filter", "test if (p_advance_spatial_filter == NULL) false case", id_cnt++);
    }

    {
        // SHPA
        ik_first_sharpen_both_t first_sharpen_both = {0};
        ik_first_sharpen_noise_t first_sharpen_noise = {0};
        ik_first_sharpen_fir_t first_sharpen_fir = {0};
        ik_first_sharpen_coring_t first_sharpen_coring = {0};
        ik_first_sharpen_coring_idx_scale_t fstshpns_coring_index_scale = {0};
        ik_first_sharpen_min_coring_result_t fstshpns_min_coring_result = {0};
        ik_first_sharpen_max_coring_result_t fstshpns_max_coring_result = {0};
        ik_first_sharpen_scale_coring_t fstshpns_scale_coring = {0};

        Rval = iks_ctx_ivd_set_fstshpns_both(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fstshpns_both", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_fstshpns_noise(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fstshpns_noise", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_fstshpns_fir(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fstshpns_fir", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_fstshpns_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fstshpns_coring", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_fstshpns_cor_idx_scl(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fstshpns_cor_idx_scl", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_fstshpns_min_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fstshpns_min_coring_result", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_fstshpns_max_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fstshpns_max_coring_result", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_fstshpns_scale_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fstshpns_scale_coring", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fstshpns_both(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fstshpns_both", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fstshpns_noise(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fstshpns_noise", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fstshpns_fir(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fstshpns_fir", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fstshpns_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fstshpns_coring", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fstshpns_cor_idx_scl(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fstshpns_cor_idx_scl", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fstshpns_min_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fstshpns_min_coring_result", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fstshpns_max_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fstshpns_max_coring_result", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fstshpns_scale_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fstshpns_scale_coring", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_fstshpns_both(pMode->ContextId, &first_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fstshpns_both", "test if (p_first_sharpen_both == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_fstshpns_noise(pMode->ContextId, &first_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fstshpns_noise", "test if (p_first_sharpen_noise == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_fstshpns_fir(pMode->ContextId, &first_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fstshpns_fir", "test if (p_first_sharpen_fir == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_fstshpns_coring(pMode->ContextId, &first_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fstshpns_coring", "test if (p_first_sharpen_coring == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_fstshpns_cor_idx_scl(pMode->ContextId, &fstshpns_coring_index_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fstshpns_cor_idx_scl", "test if (p_fstshpns_coring_index_scale == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_fstshpns_min_coring_result(pMode->ContextId, &fstshpns_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fstshpns_min_coring_result", "test if (p_fstshpns_min_coring_result == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_fstshpns_max_coring_result(pMode->ContextId, &fstshpns_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fstshpns_max_coring_result", "test if (p_fstshpns_max_coring_result == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_fstshpns_scale_coring(pMode->ContextId, &fstshpns_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fstshpns_scale_coring", "test if (p_fstshpns_scale_coring == NULL) false case", id_cnt++);
    }

    {
        // SHPB
        ik_final_sharpen_both_t final_sharpen_both = {0};
        ik_final_sharpen_noise_t final_sharpen_noise = {0};
        ik_final_sharpen_fir_t final_sharpen_fir = {0};
        ik_final_sharpen_coring_t final_sharpen_coring = {0};
        ik_final_sharpen_coring_idx_scale_t fnlshpns_coring_index_scale = {0};
        ik_final_sharpen_min_coring_result_t fnlshpns_min_coring_result = {0};
        ik_final_sharpen_max_coring_result_t fnlshpns_max_coring_result = {0};
        ik_final_sharpen_scale_coring_t fnlshpns_scale_coring = {0};
        ik_final_sharpen_both_three_d_table_t final_sharpen_both_three_d_table = {0};

        Rval = iks_ctx_ivd_set_fnlshpns_both(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fnlshpns_both", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_fnlshpns_noise(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fnlshpns_noise", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_fnlshpns_fir(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fnlshpns_fir", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_fnlshpns_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fnlshpns_coring", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_fnlshpns_cor_idx_scl(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fnlshpns_cor_idx_scl", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_fnlshpns_min_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fnlshpns_min_coring_result", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_fnlshpns_max_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fnlshpns_max_coring_result", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_fnlshpns_scale_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fnlshpns_scale_coring", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_fnlshpns_both_tdt(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fnlshpns_both_tdt", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_both(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_both", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_noise(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_noise", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_fir(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_fir", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_coring", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_cor_idx_scl(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_cor_idx_scl", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_min_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_min_coring_result", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_max_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_max_coring_result", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_scale_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_scale_coring", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_both_tdt(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_both_tdt", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_fnlshpns_both(pMode->ContextId, &final_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_both", "test if (p_final_sharpen_both == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_noise(pMode->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_noise", "test if (p_final_sharpen_noise == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_fir(pMode->ContextId, &final_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_fir", "test if (p_final_sharpen_fir == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_coring(pMode->ContextId, &final_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_coring", "test if (p_final_sharpen_coring == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_cor_idx_scl(pMode->ContextId, &fnlshpns_coring_index_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_cor_idx_scl", "test if (p_fnlshpns_coring_index_scale == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_min_coring_result(pMode->ContextId, &fnlshpns_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_min_coring_result", "test if (p_fnlshpns_min_coring_result == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_max_coring_result(pMode->ContextId, &fnlshpns_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_max_coring_result", "test if (p_fnlshpns_max_coring_result == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_scale_coring(pMode->ContextId, &fnlshpns_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_scale_coring", "test if (p_fnlshpns_scale_coring == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_fnlshpns_both_tdt(pMode->ContextId, &final_sharpen_both_three_d_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fnlshpns_both_tdt", "test if (p_final_sharpen_both_three_d_table == NULL) false case", id_cnt++);
    }

    {
        // chroma filter
        ik_chroma_filter_t chroma_filter = {0};

        Rval = iks_ctx_ivd_set_chroma_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_chroma_filter", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_chroma_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_chroma_filter", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_chroma_filter(pMode->ContextId, &chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_chroma_filter", "test if (p_chroma_filter == NULL) false case", id_cnt++);
    }

    {
        // wide chroma filter
        ik_wide_chroma_filter_t wide_chroma_filter = {0};

        Rval = iks_ctx_ivd_set_wide_chroma_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_wide_chroma_ft", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_wide_chroma_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_wide_chroma_ft", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_wide_chroma_filter(pMode->ContextId, &wide_chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_wide_chroma_ft", "test if (p_wide_chroma_filter == NULL) false case", id_cnt++);
    }

    {
        // wide chroma filter combine
        ik_wide_chroma_filter_combine_t chroma_filter_combine = {0};

        Rval = iks_ctx_ivd_set_wide_chroma_combine(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_wide_chroma_ft_combine", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_wide_chroma_combine(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_wide_chroma_ft_combine", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_wide_chroma_combine(pMode->ContextId, &chroma_filter_combine);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_wide_chroma_ft_combine", "test if (p_chroma_filter_combine == NULL) false case", id_cnt++);
    }

    {
        // grgb mismatch
        ik_grgb_mismatch_t grgb_mismatch = {0};

        Rval = iks_ctx_ivd_set_grgb_mismatch(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_grgb_mismatch", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_grgb_mismatch(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_grgb_mismatch", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_grgb_mismatch(pMode->ContextId, &grgb_mismatch);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_grgb_mismatch", "test if (p_grgb_mismatch == NULL) false case", id_cnt++);
    }

    {
        // MCTF
        ik_video_mctf_t video_mctf = {0};
        ik_video_mctf_ta_t video_mctf_ta = {0};
        ik_pos_dep33_t video_mctf_and_final_sharpen = {0};

        Rval = iks_ctx_ivd_set_video_mctf(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_video_mctf", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_mctf_ta(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_mctf_ta", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_mctf_and_final_sharpen(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_mctf_and_final_sharpen", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_video_mctf(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_video_mctf", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_mctf_ta(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_mctf_ta", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_mctf_and_final_sharpen(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_mctf_and_final_sharpen", "test invalid API fail case", id_cnt++);

        p_ctx->filters.update_flags.iso.is_1st_frame = 1;
        Rval = iks_ctx_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_set_video_mctf", "test p_ctx->filters.update_flags.iso.is_1st_frame == 0U false case", id_cnt++);
        p_ctx->filters.update_flags.iso.is_1st_frame = 0;

        p_ctx->filters.input_param.video_mctf.compression_enable = 0;
        video_mctf.compression_enable = 1;
        Rval = iks_ctx_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_set_video_mctf", "test p_ctx->filters.input_param.video_mctf.compression_enable != p_video_mctf->compression_enable true case", id_cnt++);

        video_mctf.compression_enable = 0;
        Rval = iks_ctx_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_set_video_mctf", "test p_ctx->filters.input_param.video_mctf.compression_enable != p_video_mctf->compression_enable false case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.compression_bit_rate_luma = 20;
        video_mctf.compression_bit_rate_luma = 10;
        Rval = iks_ctx_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_set_video_mctf", "test p_ctx->filters.input_param.video_mctf.compression_bit_rate_luma != p_video_mctf->compression_bit_rate_luma true case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.compression_bit_rate_luma = 10;
        video_mctf.compression_bit_rate_luma = 10;
        Rval = iks_ctx_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_set_video_mctf", "test p_ctx->filters.input_param.video_mctf.compression_bit_rate_luma != p_video_mctf->compression_bit_rate_luma false case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.compression_bit_rate_chroma = 20;
        video_mctf.compression_bit_rate_chroma = 10;
        Rval = iks_ctx_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_set_video_mctf", "test p_ctx->filters.input_param.video_mctf.compression_bit_rate_chroma != p_video_mctf->compression_bit_rate_chroma true case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.compression_bit_rate_chroma = 10;
        video_mctf.compression_bit_rate_chroma = 10;
        Rval = iks_ctx_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_set_video_mctf", "test p_ctx->filters.input_param.video_mctf.compression_bit_rate_chroma != p_video_mctf->compression_bit_rate_chroma false case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.compression_dither_disable = 1;
        video_mctf.compression_dither_disable = 0;
        Rval = iks_ctx_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_set_video_mctf", "test p_ctx->filters.input_param.video_mctf.compression_dither_disable != p_video_mctf->compression_dither_disable true case", id_cnt++);

        Rval = iks_ctx_ivd_get_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_video_mctf", "test if (p_video_mctf == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_mctf_ta(pMode->ContextId, &video_mctf_ta);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_mctf_ta", "test if (p_video_mctf_ta == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_mctf_and_final_sharpen(pMode->ContextId, &video_mctf_and_final_sharpen);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_mctf_and_final_sharpen", "test if (p_video_mctf_and_final_sharpen == NULL) false case", id_cnt++);
    }

    {
        // AAA
        ik_aaa_stat_info_t stat_info = {0};
        ik_aaa_pg_af_stat_info_t pg_af_stat_info = {0};
        ik_af_stat_ex_info_t af_stat_ex_info = {0};
        ik_pg_af_stat_ex_info_t pg_af_stat_ex_info = {0};
        ik_histogram_info_t hist_info = {0};

        Rval = iks_ctx_ivd_set_aaa_stat_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_aaa_stat_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_aaa_pg_af_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_aaa_pg_af_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_af_stat_ex_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_af_stat_ex_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_pg_af_stat_ex_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_pg_af_stat_ex_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_histogram_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_histogram_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_set_pg_histogram_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_pg_histogram_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_aaa_stat_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_aaa_stat_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_aaa_pg_af_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_aaa_pg_af_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_af_stat_ex_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_af_stat_ex_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_pg_af_stat_ex_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_pg_af_stat_ex_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_histogram_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_histogram_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_pg_histogram_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_pg_histogram_info", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_aaa_stat_info(pMode->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_aaa_stat_info", "test if (p_stat_info == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_aaa_pg_af_info(pMode->ContextId, &pg_af_stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_aaa_pg_af_info", "test if (p_pg_af_stat_info == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_af_stat_ex_info(pMode->ContextId, &af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_af_stat_ex_info", "test if (p_af_stat_ex_info == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_pg_af_stat_ex_info(pMode->ContextId, &pg_af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_pg_af_stat_ex_info", "test if (p_pg_af_stat_ex_info == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_histogram_info(pMode->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_histogram_info", "test if (p_hist_info == NULL) false case", id_cnt++);
        Rval = iks_ctx_ivd_get_pg_histogram_info(pMode->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_pg_histogram_info", "test if (p_hist_info == NULL) false case", id_cnt++);
    }

    {
        // window size info
        ik_window_size_info_t window_size_info = {0};

        Rval = iks_ctx_ivd_set_window_size_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_window_size_info", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_window_size_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_window_size_info", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_window_size_info(pMode->ContextId, &window_size_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_window_size_info", "test if (p_window_size_info == NULL) false case", id_cnt++);
    }

    {
        // front-end tone curve
        ik_frontend_tone_curve_t fe_tc = {0};

        Rval = iks_ctx_ivd_set_fe_tone_curve(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_fe_tone_curve", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_fe_tone_curve(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fe_tone_curve", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_fe_tone_curve(pMode->ContextId, &fe_tc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_fe_tone_curve", "test if (p_fe_tc == NULL) false case", id_cnt++);
    }

    {
        // resampler strength
        ik_resampler_strength_t resample_str = {0};

        Rval = iks_ctx_ivd_set_resampler_str(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_set_resampler_str", "test invalid API fail case", id_cnt++);
        Rval = iks_ctx_ivd_get_resampler_str(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_resampler_str", "test invalid API fail case", id_cnt++);

        Rval = iks_ctx_ivd_get_resampler_str(pMode->ContextId, &resample_str);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "iks_ctx_ivd_get_resampler_str", "test if (p_resample_str == NULL) false case", id_cnt++);
    }
}

static void IKS_TestCovr_ImgContextUnitUtility()
{
    UINT32 Rval = IK_OK;

    iks_ctx_raise_update_flag(NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextUnitUtility.c", "iks_ctx_raise_update_flag", "test NULL ptr fail case", id_cnt++);
}

static void IKS_TestCovr_ImgContextComponentIF(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;
    ik_buffer_info_t *p_iks_buffer_info = NULL;
    ik_ability_t ability = {0};
    ik_system_api_t sys_api;
    uintptr mem_addr = 0xBABEFACE;
    amba_iks_context_entity_t *p_ctx = NULL;

    iks_ctx_get_context(pMode->ContextId, &p_ctx);

    ability.pipe = AMBA_IK_PIPE_VIDEO;
    ability.video_pipe = AMBA_IK_VIDEO_HDR_EXPO_3;

    Rval = iks_ctx_query_context_memory_size(NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_query_context_memory_size", "test NULL ptr fail case", id_cnt++);

    Rval = iks_ctx_reset_update_flags(33);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_reset_update_flags", "test (context_id > iks_arch_get_context_number()) fail case", id_cnt++);

    Rval = iks_ctx_init_context(pMode->ContextId, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_init_context", "test NULL ptr fail case", id_cnt++);

    iks_arch_get_ik_working_buffer(&p_iks_buffer_info);
    p_iks_buffer_info->init_flag = 0u;
    Rval = iks_ctx_init_context(pMode->ContextId, &ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_init_context", "test NULL ptr fail case", id_cnt++);
    p_iks_buffer_info->init_flag = 1u;

    Rval = iks_ctx_init_context(33, &ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_init_context", "test get ctx fail case", id_cnt++);

    ability.pipe = AMBA_IK_PIPE_STILL;
    Rval = iks_ctx_init_context(pMode->ContextId, &ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_init_context", "test (p_ability->pipe == AMBA_IK_PIPE_STILL) fail case", id_cnt++);

    ability.pipe = AMBA_IK_PIPE_VIDEO;
    ability.video_pipe = AMBA_IK_VIDEO_MAX;
    Rval = iks_ctx_init_context(pMode->ContextId, &ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_init_context", "test (p_ability->video_pipe < AMBA_IK_VIDEO_MAX) fail case", id_cnt++);

    ability.pipe = 2;
    Rval = iks_ctx_init_context(pMode->ContextId, &ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_init_context", "test (p_ability->pipe != VIDEO and STILL) fail case", id_cnt++);

    ability.pipe = AMBA_IK_PIPE_VIDEO;
    ability.video_pipe = AMBA_IK_VIDEO_HDR_EXPO_3;
    p_iks_buffer_info->init_mode = AMBA_IK_ARCH_SOFT_INIT;
    Rval = iks_ctx_init_context(pMode->ContextId, &ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_init_context", "test p_ik_buffer_info->init_mode == AMBA_IK_ARCH_HARD_INIT false case", id_cnt++);
    p_iks_buffer_info->init_mode = AMBA_IK_ARCH_HARD_INIT;

    sys_api.system_create_mutex = (ik_system_create_mutex_t)fake_create_mutex_fail;
    img_arch_registe_system_create_mutex(&(sys_api.system_create_mutex));
    p_ctx->organization.initial_flag = 0U;
    Rval = iks_ctx_init_context(pMode->ContextId, &ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_init_context", "test amba_ik_system_create_mutex() fail case", id_cnt++);
    p_ctx->organization.initial_flag = 1U;
    sys_api.system_create_mutex = (ik_system_create_mutex_t)fake_create_mutex;
    img_arch_registe_system_create_mutex(&(sys_api.system_create_mutex));

    Rval = iks_ctx_get_context_ability(pMode->ContextId, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_get_context_ability", "test null ptr fail case", id_cnt++);

    Rval = iks_ctx_get_context_ability(33, &ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_get_context_ability", "test get ctx fail case", id_cnt++);

    p_ctx->organization.initial_flag = 0U;
    Rval = iks_ctx_get_context_ability(pMode->ContextId, &ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_get_context_ability", "test (p_ctx->organization.initial_flag != 0U) fail case", id_cnt++);
    p_ctx->organization.initial_flag = 1U;

    Rval = iks_ctx_get_context_ability(pMode->ContextId, &ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_get_context_ability", "test (p_ctx->organization.initial_flag != 0U) OK case", id_cnt++);

    Rval = iks_ctx_get_flow_control(pMode->ContextId, 0, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_get_flow_control", "test null ptr fail case", id_cnt++);
    Rval = iks_ctx_get_flow_control(33, 0, &mem_addr);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_get_flow_control", "test get ctx fail case", id_cnt++);
    Rval = iks_ctx_get_flow_control(pMode->ContextId, 6, &mem_addr);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_get_flow_control", "test (p_ctx->organization.attribute.cr_ring_number > flow_idx) fail case", id_cnt++);

    Rval = iks_ctx_get_flow_tbl(pMode->ContextId, 0, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_get_flow_tbl", "test null ptr fail case", id_cnt++);
    Rval = iks_ctx_get_flow_tbl(33, 0, &mem_addr);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_get_flow_tbl", "test get ctx fail case", id_cnt++);
    Rval = iks_ctx_get_flow_tbl(pMode->ContextId, 6, &mem_addr);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_get_flow_tbl", "test (p_ctx->organization.attribute.cr_ring_number > flow_idx) fail case", id_cnt++);

    Rval = iks_ctx_get_crc_data(pMode->ContextId, 0, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_get_crc_data", "test null ptr fail case", id_cnt++);
    Rval = iks_ctx_get_crc_data(33, 0, &mem_addr);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_get_crc_data", "test get ctx fail case", id_cnt++);
    Rval = iks_ctx_get_crc_data(pMode->ContextId, 33, &mem_addr);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_get_crc_data", "test (p_ctx->organization.attribute.cr_ring_number > flow_idx) fail case", id_cnt++);
}

static void IKS_TestCovr_ImgContextComponentIF_PrepareCtxMem(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;
    uintptr mem_addr = 0xBABEFACE;
    size_t size = 100;

    {
        Rval = iks_ctx_prepare_context_memory(pMode->ContextId, 0u, size);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_prepare_context_memory", "test NULL ptr fail case", id_cnt++);

        Rval = iks_ctx_prepare_context_memory(pMode->ContextId, mem_addr, 0u);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_prepare_context_memory", "test NULL ptr fail case", id_cnt++);

        Rval = iks_ctx_prepare_context_memory(33, mem_addr, size);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_prepare_context_memory", "test (context_id > context_number) fail case", id_cnt++);

        iks_arch_deinit_architecture();
        Rval = iks_ctx_prepare_context_memory(pMode->ContextId, mem_addr, size);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "iks_ctx_prepare_context_memory", "test (rval == IK_OK) false case", id_cnt++);
    }
}

INT32 iks_ctest_context_main(void)
{
    UINT32 Rval = IK_OK;
    AMBA_IK_ABILITY_s Ability;
    AMBA_IK_MODE_CFG_s Mode;

    memset(ik_working_buffer, 0, sizeof(ik_working_buffer));
    memset(WarpGrid, 0, sizeof(WarpGrid));
    memset(CawarpRedGrid, 0, sizeof(CawarpRedGrid));
    memset(CawarpBlueGrid, 0, sizeof(CawarpBlueGrid));
    memset(FPNMap, 0, sizeof(FPNMap));
    id_cnt = 0;

    pFile = fopen("data/iks_ctest_context.txt", "w");

    // init IK
    _load_idsp_default_binary();
    Ability.Pipe = AMBA_IK_PIPE_VIDEO;
    Ability.VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_3;

    _Init_Image_Kernel(&Ability);
    _Init_Image_Kernel_Core();

    Mode.ContextId = 0;
    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    // test AmbaDSP_ImgContextComponentCalib.c
    IKS_TestCovr_ImgContextComponentCalib(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test AmbaDSP_ImgContextComponentHDR.c
    IKS_TestCovr_ImgContextComponentHDR(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test AmbaDSP_ImgContextComponentNormalFilter.c
    IKS_TestCovr_ImgContextComponentNormalFilter(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test AmbaDSP_ImgContextUnitUtility.c
    IKS_TestCovr_ImgContextUnitUtility();
    _Reset_Image_Kernel(&Ability);

    // test AmbaDSP_ImgContextComponentIF.c
    IKS_TestCovr_ImgContextComponentIF(&Mode);
    _Reset_Image_Kernel(&Ability);
    IKS_TestCovr_ImgContextComponentIF_PrepareCtxMem(&Mode);
    _Reset_Image_Kernel(&Ability);


    free((UINT8 *) pBinDataAddr);
    pBinDataAddr = NULL;
    fclose(pFile);

    fake_destroy_mutex(NULL);
    fake_clean_cache(NULL, 0);

    img_arch_deinit_architecture();
    iks_arch_deinit_architecture();

    return Rval;
}

