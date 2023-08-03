#include "test_ik_global.h"
#include "AmbaDSP_ImgContextDebugUtility.h"


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
    AMBA_IK_ABILITY_s Ability[3];
    AMBA_IK_MODE_CFG_s Mode[3];

    img_arch_deinit_architecture();
    iks_arch_deinit_architecture();

    memset(ik_working_buffer, 0, sizeof(ik_working_buffer));

    memcpy(Ability, pAbility, sizeof(AMBA_IK_ABILITY_s)*3);

    ContextSetting.ContextNumber = 3;
    ContextSetting.ConfigSetting[0].ConfigNumber = 5;
    ContextSetting.ConfigSetting[0].pAbility = &Ability[0];
    ContextSetting.ConfigSetting[1].ConfigNumber = 5;
    ContextSetting.ConfigSetting[1].pAbility = &Ability[1];
    ContextSetting.ConfigSetting[2].ConfigNumber = 5;
    ContextSetting.ConfigSetting[2].pAbility = &Ability[2];

    _reload_idsp_default_binary();
    AmbaIK_QueryArchMemorySize(&ContextSetting, &TotalCtxSize);
    AmbaIK_InitArch(&ContextSetting, (void*)ik_working_buffer, sizeof(ik_working_buffer));
    AmbaIK_InitDefBinary(pBinDataAddr);

    Mode[0].ContextId = 0;
    Rval |= AmbaIK_InitContext(&Mode[0], &Ability[0]);

    Mode[1].ContextId = 1;
    Rval |= AmbaIK_InitContext(&Mode[1], &Ability[1]);

    Mode[2].ContextId = 2;
    Rval |= AmbaIK_InitContext(&Mode[2], &Ability[2]);
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

    ContextSetting.ContextNumber = 3;
    ContextSetting.ConfigSetting[0].ConfigNumber = 5;
    ContextSetting.ConfigSetting[0].pAbility = p_ability;
    ContextSetting.ConfigSetting[1].ConfigNumber = 5;
    ContextSetting.ConfigSetting[1].pAbility = ++p_ability;
    ContextSetting.ConfigSetting[2].ConfigNumber = 5;
    ContextSetting.ConfigSetting[2].pAbility = ++p_ability;

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

static void IK_TestCovr_ImgContextComponentCalib(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;
    uint32 enable = 0;
    amba_ik_context_entity_t *p_ctx = NULL;

    img_ctx_get_context(pMode->ContextId, &p_ctx);

    {
        // SBP
        ik_static_bad_pxl_cor_t static_bpc = {0};
        ik_static_bad_pixel_correction_internal_t static_bpc_internal = {0};
        extern uint32 img_ctx_ivd_set_static_bpc(uint32 context_id, const ik_static_bad_pxl_cor_t *p_static_bpc);
        extern uint32 img_ctx_ivd_set_static_bpc_internal(uint32 context_id, const ik_static_bad_pixel_correction_internal_t *static_bpc_internal);
        extern uint32 img_ctx_ivd_set_static_bpc_enable_info(uint32 context_id, const uint32 enable);
        extern uint32 img_ctx_ivd_get_static_bpc_enable_info(uint32 context_id, uint32 *p_enable);
        extern uint32 img_ctx_ivd_get_static_bpc(uint32 context_id, ik_static_bad_pxl_cor_t *p_static_bpc);
        extern uint32 img_ctx_ivd_get_static_bpc_internal(uint32 context_id, ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal);

        Rval = img_ctx_ivd_set_static_bpc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_static_bpc", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_static_bpc_internal(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_static_bpc_internal", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_static_bpc_enable_info(pMode->ContextId, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_static_bpc_enable_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_static_bpc_enable_info(pMode->ContextId, &enable);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_static_bpc_enable_info", "test if (p_enable != NULL) true case", id_cnt++);
        Rval = img_ctx_ivd_get_static_bpc_enable_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_static_bpc_enable_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_static_bpc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_static_bpc", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_static_bpc(pMode->ContextId, &static_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_static_bpc", "test if (p_static_bpc != NULL) true case", id_cnt++);
        Rval = img_ctx_ivd_get_static_bpc_internal(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_static_bpc_internal", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_static_bpc_internal(pMode->ContextId, &static_bpc_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_static_bpc_internal", "test if (p_static_bpc_internal != NULL) true case", id_cnt++);
    }

    {
        // Vig
        ik_vignette_t vignette = {0};
        extern uint32 img_ctx_ivd_set_vignette(uint32 context_id, const ik_vignette_t *p_vignette);
        extern uint32 img_ctx_ivd_set_vignette_enable_info(uint32 context_id, const uint32 enable);
        extern uint32 img_ctx_ivd_get_vignette_enable_info(uint32 context_id, uint32 *p_enable);
        extern uint32 img_ctx_ivd_get_vignette(uint32 context_id, ik_vignette_t *p_vignette);

        Rval = img_ctx_ivd_set_vignette(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_vignette", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_vignette_enable_info(pMode->ContextId, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_vignette_enable_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_vignette_enable_info(pMode->ContextId, &enable);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_vignette_enable_info", "test if (p_enable != NULL) true case", id_cnt++);
        Rval = img_ctx_ivd_get_vignette_enable_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_vignette_enable_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_vignette(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_vignette", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_vignette(pMode->ContextId, &vignette);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_vignette", "test if (p_vignette != NULL) true case", id_cnt++);
    }

    {
        // Warp
        ik_warp_info_t calib_warp_info = {0};
        ik_warp_internal_info_t warp_internal = {0};
        extern uint32 img_ctx_ivd_set_calib_warp_info(uint32 context_id, const ik_warp_info_t *p_calib_warp_info);
        extern uint32 img_ctx_ivd_set_warp_enable_info(uint32 context_id, const uint32 enable);
        extern uint32 img_ctx_ivd_get_calib_warp_info(uint32 context_id, ik_warp_info_t *p_calib_warp_info);
        extern uint32 img_ctx_ivd_get_warp_enable_info(uint32 context_id, uint32 *p_enable);
        extern uint32 img_ctx_ivd_set_warp_internal(uint32 context_id, const ik_warp_internal_info_t *p_warp_internal);
        extern uint32 img_ctx_ivd_get_warp_internal(uint32 context_id, ik_warp_internal_info_t *p_warp_internal);

        Rval = img_ctx_ivd_set_calib_warp_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_calib_warp_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_warp_enable_info(pMode->ContextId, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_warp_enable_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_calib_warp_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_calib_warp_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_calib_warp_info(pMode->ContextId, &calib_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_calib_warp_info", "test (p_calib_warp_info != NULL) true case", id_cnt++);
        Rval = img_ctx_ivd_get_warp_enable_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_warp_enable_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_warp_enable_info(pMode->ContextId, &enable);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_warp_enable_info", "test (p_enable != NULL) true case", id_cnt++);
        Rval = img_ctx_ivd_set_warp_internal(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_warp_internal", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_warp_internal(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_warp_internal", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_warp_internal(pMode->ContextId, &warp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_warp_internal", "test (p_warp_internal != NULL) true case", id_cnt++);
    }

    {
        // CA
        ik_cawarp_info_t calib_ca_warp_info = {0};
        ik_cawarp_internal_info_t cawarp_internal = {0};
        extern uint32 img_ctx_ivd_set_calib_ca_warp_info(uint32 context_id, const ik_cawarp_info_t *p_calib_ca_warp_info);
        extern uint32 img_ctx_ivd_set_cawarp_enable_info(uint32 context_id, const uint32 enable);
        extern uint32 img_ctx_ivd_get_calib_ca_warp_info(uint32 context_id, ik_cawarp_info_t *p_calib_ca_warp_info);
        extern uint32 img_ctx_ivd_get_cawarp_enable_info(uint32 context_id, uint32 *p_enable);
        extern uint32 img_ctx_ivd_set_cawarp_internal(uint32 context_id, const ik_cawarp_internal_info_t *p_cawarp_internal);
        extern uint32 img_ctx_ivd_get_cawarp_internal(uint32 context_id, ik_cawarp_internal_info_t *p_cawarp_internal);

        Rval = img_ctx_ivd_set_calib_ca_warp_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_calib_ca_warp_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_cawarp_enable_info(pMode->ContextId, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_cawarp_enable_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_calib_ca_warp_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_calib_ca_warp_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_calib_ca_warp_info(pMode->ContextId, &calib_ca_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_calib_ca_warp_info", "test (p_calib_ca_warp_info != NULL) true case", id_cnt++);
        Rval = img_ctx_ivd_get_cawarp_enable_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_cawarp_enable_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_cawarp_enable_info(pMode->ContextId, &enable);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_cawarp_enable_info", "test (p_enable != NULL) true case", id_cnt++);
        Rval = img_ctx_ivd_set_cawarp_internal(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_cawarp_internal", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_cawarp_internal(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_cawarp_internal", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_cawarp_internal(pMode->ContextId, &cawarp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_cawarp_internal", "test (p_cawarp_internal != NULL) true case", id_cnt++);
    }

    {
        // DZoom
        ik_dzoom_info_t dzoom_info = {0};
        extern uint32 img_ctx_ivd_set_dzoom_info(uint32 context_id, const ik_dzoom_info_t *p_dzoom_info);
        extern uint32 img_ctx_ivd_get_dzoom_info(uint32 context_id, ik_dzoom_info_t *p_dzoom_info);

        Rval = img_ctx_ivd_set_dzoom_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_dzoom_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_dzoom_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_dzoom_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_dzoom_info(pMode->ContextId, &dzoom_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_dzoom_info", "test (p_dzoom_info != NULL) true case", id_cnt++);
    }

    {
        // Dummy
        ik_dummy_margin_range_t dmy_range = {0};
        extern uint32 img_ctx_ivd_set_dummy_win_margin_range_info(uint32 context_id, const ik_dummy_margin_range_t *p_dmy_range);
        extern uint32 img_ctx_set_dummy_win_margin_range_info(uint32 context_id, const ik_dummy_margin_range_t *p_dmy_range);
        extern uint32 img_ctx_ivd_get_dummy_win_margin_range_info(uint32 context_id, ik_dummy_margin_range_t *p_dmy_range);

        Rval = img_ctx_ivd_set_dummy_win_margin_range_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_dummy_win_margin_range_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_dummy_win_margin_range_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_dummy_win_margin_range_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_dummy_win_margin_range_info(pMode->ContextId, &dmy_range);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_dummy_win_margin_range_info", "test (dmy_range != NULL) true case", id_cnt++);

        ik_ctx_hook_debug_check();
        p_ctx->filters.input_param.stitching_info.enable = 1;
        p_ctx->filters.input_param.stitching_info.tile_num_x = 1;
        Rval = img_ctx_set_dummy_win_margin_range_info(pMode->ContextId, &dmy_range);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_set_dummy_win_margin_range_info", "test ((p_ctx->filters.input_param.stitching_info.enable != 0)&&(p_ctx->filters.input_param.stitching_info.tile_num_x > 1)) false case", id_cnt++);
        p_ctx->filters.input_param.stitching_info.enable = 0;
        p_ctx->filters.input_param.stitching_info.tile_num_x = 0;
        ik_init_debug_check_func();
    }

    {
        // Flip mode
        uint32 mode = 0;
        extern uint32 img_ctx_ivd_set_flip_mode(uint32 context_id, const uint32 mode);
        extern uint32 img_ctx_ivd_get_flip_mode(uint32 context_id, uint32 *mode);

        Rval = img_ctx_ivd_set_flip_mode(pMode->ContextId, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_flip_mode", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_flip_mode(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_flip_mode", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_flip_mode(pMode->ContextId, &mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_flip_mode", "test (mode != NULL) true case", id_cnt++);
    }

    {
        // Warp buffer info
        ik_warp_buffer_info_t warp_buf_info = {0};
        extern uint32 img_ctx_ivd_set_warp_buffer_info(uint32 context_id, const ik_warp_buffer_info_t *p_warp_buf_info);
        extern uint32 img_ctx_ivd_get_warp_buffer_info(uint32 context_id, ik_warp_buffer_info_t *p_warp_buf_info);

        Rval = img_ctx_ivd_set_warp_buffer_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_warp_buffer_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_warp_buffer_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_warp_buffer_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_warp_buffer_info(pMode->ContextId, &warp_buf_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_warp_buffer_info", "test (p_warp_buf_info != NULL) true case", id_cnt++);
    }

    {
        // Stitch info
        ik_stitch_info_t stitch_info = {0};
        extern uint32 img_ctx_ivd_set_stitching_info(uint32 context_id, const ik_stitch_info_t *p_stitch_info);
        extern uint32 img_ctx_set_stitching_info(uint32 context_id, const ik_stitch_info_t *p_stitch_info);
        extern uint32 img_ctx_ivd_get_stitching_info(uint32 context_id, ik_stitch_info_t *p_stitch_info);

        Rval = img_ctx_ivd_set_stitching_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_stitching_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_stitching_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_stitching_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_stitching_info(pMode->ContextId, &stitch_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_stitching_info", "test (p_stitch_info != NULL) true case", id_cnt++);

        p_ctx->filters.update_flags.iso.is_1st_frame = 0;
        Rval = img_ctx_set_stitching_info(pMode->ContextId, &stitch_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_set_stitching_info", "test if(p_ctx->filters.update_flags.iso.is_1st_frame == 0U) true case", id_cnt++);
        p_ctx->filters.update_flags.iso.is_1st_frame = 1;
    }

    {
        // burst tile
        ik_burst_tile_t burst_tile = {0};

        p_ctx->filters.update_flags.iso.is_1st_frame = 0;
        Rval = img_ctx_set_burst_tile(pMode->ContextId, &burst_tile);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_set_burst_tile", "test if(p_ctx->filters.update_flags.iso.is_1st_frame == 0U) true case", id_cnt++);
        p_ctx->filters.update_flags.iso.is_1st_frame = 1;
    }

    {
        // Active win
        ik_vin_active_window_t active_win = {0};
        extern uint32 img_ctx_ivd_set_vin_active_window(uint32 context_id, const ik_vin_active_window_t *p_active_win);
        extern uint32 img_ctx_ivd_get_vin_active_window(uint32 context_id, ik_vin_active_window_t *p_active_win);

        Rval = img_ctx_ivd_set_vin_active_window(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_vin_active_window", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_vin_active_window(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_vin_active_window", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_vin_active_window(pMode->ContextId, &active_win);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_vin_active_window", "test (p_active_win != NULL) true case", id_cnt++);
    }

    {
        // burst tile
        ik_burst_tile_t burst_tile = {0};

        Rval = img_ctx_ivd_set_burst_tile(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_set_burst_tile", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_burst_tile(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_burst_tile", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_burst_tile(pMode->ContextId, &burst_tile);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_burst_tile", "test (p_burst_tile != NULL) true case", id_cnt++);
    }
}

static void IK_TestCovr_ImgContextComponentHDR(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;

    {
        // Front-end BLC
        ik_static_blc_level_t frontend_static_blc = {0};
        extern uint32 img_ctx_ivd_set_exp0_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp0_frontend_static_blc);
        extern uint32 img_ctx_ivd_set_exp1_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp1_frontend_static_blc);
        extern uint32 img_ctx_ivd_set_exp2_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp2_frontend_static_blc);
        extern uint32 img_ctx_ivd_get_exp0_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp0_frontend_static_blc);
        extern uint32 img_ctx_ivd_get_exp1_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp1_frontend_static_blc);
        extern uint32 img_ctx_ivd_get_exp2_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp2_frontend_static_blc);

        Rval = img_ctx_ivd_set_exp0_fe_static_blc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_set_exp0_fe_static_blc", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_exp1_fe_static_blc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_set_exp1_fe_static_blc", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_exp2_fe_static_blc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_set_exp2_fe_static_blc", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_exp0_fe_static_blc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_get_exp0_fe_static_blc", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_exp1_fe_static_blc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_get_exp1_fe_static_blc", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_exp2_fe_static_blc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_get_exp2_fe_static_blc", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_exp0_fe_static_blc(pMode->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_exp0_fe_static_blc", "test (p_exp0_frontend_static_blc != NULL) true case", id_cnt++);
        Rval = img_ctx_ivd_get_exp1_fe_static_blc(pMode->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_exp1_fe_static_blc", "test (p_exp1_frontend_static_blc != NULL) true case", id_cnt++);
        Rval = img_ctx_ivd_get_exp2_fe_static_blc(pMode->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_exp2_fe_static_blc", "test (p_exp2_frontend_static_blc != NULL) true case", id_cnt++);
    }

    {
        // Front-end wb gain
        ik_frontend_wb_gain_t frontend_wb_gain = {0};
        extern uint32 img_ctx_ivd_set_exp0_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain);
        extern uint32 img_ctx_ivd_set_exp1_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain);
        extern uint32 img_ctx_ivd_set_exp2_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain);
        extern uint32 img_ctx_ivd_get_exp0_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain);
        extern uint32 img_ctx_ivd_get_exp1_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain);
        extern uint32 img_ctx_ivd_get_exp2_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain);

        Rval = img_ctx_ivd_set_exp0_fe_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_set_exp0_fe_wb_gain", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_exp1_fe_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_set_exp1_fe_wb_gain", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_exp2_fe_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_set_exp2_fe_wb_gain", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_exp0_fe_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_get_exp0_fe_wb_gain", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_exp1_fe_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_get_exp1_fe_wb_gain", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_exp2_fe_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_get_exp2_fe_wb_gain", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_exp0_fe_wb_gain(pMode->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_exp0_fe_wb_gain", "test (p_exp0_frontend_wb_gain != NULL) true case", id_cnt++);
        Rval = img_ctx_ivd_get_exp1_fe_wb_gain(pMode->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_exp1_fe_wb_gain", "test (p_exp1_frontend_wb_gain != NULL) true case", id_cnt++);
        Rval = img_ctx_ivd_get_exp2_fe_wb_gain(pMode->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_exp2_fe_wb_gain", "test (p_exp2_frontend_wb_gain != NULL) true case", id_cnt++);
    }

    {
        // Raw info
        ik_hdr_raw_info_t raw_info = {0};
        extern uint32 img_ctx_ivd_set_hdr_raw_offset(uint32 context_id, const ik_hdr_raw_info_t *p_raw_info);
        extern uint32 img_ctx_ivd_get_hdr_raw_offset(uint32 context_id, ik_hdr_raw_info_t *p_raw_info);

        Rval = img_ctx_ivd_set_hdr_raw_offset(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_set_hdr_raw_offset", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_hdr_raw_offset(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_get_hdr_raw_offset", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_hdr_raw_offset(pMode->ContextId, &raw_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_hdr_raw_offset", "test (p_raw_info != NULL) true case", id_cnt++);
    }

    {
        // CE
        ik_ce_t ce = {0};
        ik_ce_input_table_t ce_input_table = {0};
        ik_ce_output_table_t ce_out_table = {0};
        extern uint32 img_ctx_ivd_set_ce(uint32 context_id, const ik_ce_t *p_ce);
        extern uint32 img_ctx_ivd_set_ce_input_table(uint32 context_id, const ik_ce_input_table_t *p_ce_input_table);
        extern uint32 img_ctx_ivd_set_ce_out_table(uint32 context_id, const ik_ce_output_table_t *p_ce_out_table);
        extern uint32 img_ctx_ivd_get_ce(uint32 context_id, ik_ce_t *p_ce);
        extern uint32 img_ctx_ivd_get_ce_input_table(uint32 context_id, ik_ce_input_table_t *p_ce_input_table);
        extern uint32 img_ctx_ivd_get_ce_out_table(uint32 context_id, ik_ce_output_table_t *p_ce_out_table);

        Rval = img_ctx_ivd_set_ce(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_set_ce", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_ce_input_table(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_set_ce_input_table", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_ce_out_table(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_set_ce_out_table", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_ce(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_get_ce", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_ce_input_table(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_get_ce_input_table", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_ce_out_table(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_get_ce_out_table", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_ce(pMode->ContextId, &ce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_ce", "test (p_ce != NULL) true case", id_cnt++);
        Rval = img_ctx_ivd_get_ce_input_table(pMode->ContextId, &ce_input_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_ce_input_table", "test (p_ce_input_table != NULL) true case", id_cnt++);
        Rval = img_ctx_ivd_get_ce_out_table(pMode->ContextId, &ce_out_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_ce_out_table", "test (p_ce_out_table != NULL) true case", id_cnt++);
    }

    {
        // HDR blend
        ik_hdr_blend_t hdr_blend = {0};
        extern uint32 img_ctx_ivd_set_hdr_blend(uint32 context_id, const ik_hdr_blend_t *p_hdr_blend);
        extern uint32 img_ctx_ivd_get_hdr_blend(uint32 context_id, ik_hdr_blend_t *p_hdr_blend);

        Rval = img_ctx_ivd_set_hdr_blend(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_set_hdr_blend", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_hdr_blend(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentHDR.c", "img_ctx_ivd_get_hdr_blend", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_hdr_blend(pMode->ContextId, &hdr_blend);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_ivd_get_hdr_blend", "test (p_hdr_blend != NULL) true case", id_cnt++);
    }
}

static void IK_TestCovr_ImgContextComponentNormalFilter(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    img_ctx_get_context(pMode->ContextId, &p_ctx);

    {
        // Safety info
        ik_safety_info_t safety_info = {0};
        extern uint32 img_ctx_ivd_set_safety_info(uint32 context_id, const ik_safety_info_t *p_safety_info);
        extern uint32 img_ctx_ivd_get_safety_info(uint32 context_id, ik_safety_info_t *p_safety_info);

        Rval = img_ctx_ivd_set_safety_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_safety_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_safety_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_safety_info", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_safety_info(pMode->ContextId, &safety_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_safety_info", "test if (p_safety_info==NULL) false case", id_cnt++);
    }

    {
        // Vin sensor info
        ik_vin_sensor_info_t sensor_info = {0};
        extern uint32 img_ctx_ivd_set_vin_sensor_info(uint32 context_id, const ik_vin_sensor_info_t *p_sensor_info);
        extern uint32 img_ctx_set_vin_sensor_info(uint32 context_id, const ik_vin_sensor_info_t *p_sensor_info);
        extern uint32 img_ctx_ivd_get_vin_sensor_info(uint32 context_id, ik_vin_sensor_info_t *p_sensor_info);

        Rval = img_ctx_ivd_set_vin_sensor_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_vin_sensor_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_vin_sensor_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_vin_sensor_info", "test invalid API fail case", id_cnt++);

        p_ctx->filters.update_flags.iso.is_1st_frame = 0;
        Rval = img_ctx_set_vin_sensor_info(pMode->ContextId, &sensor_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentCalib.c", "img_ctx_set_vin_sensor_info", "test if(p_ctx->filters.update_flags.iso.is_1st_frame == 0U) true case", id_cnt++);
        p_ctx->filters.update_flags.iso.is_1st_frame = 1;

        Rval = img_ctx_ivd_get_vin_sensor_info(pMode->ContextId, &sensor_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_vin_sensor_info", "test if (p_sensor_info==NULL) false case", id_cnt++);
    }

    {
        // YUV mode
        uint32 yuv_mode = 0;
        extern uint32 img_ctx_ivd_set_yuv_mode(uint32 context_id, const uint32 yuv_mode);
        extern uint32 img_ctx_ivd_get_yuv_mode(uint32 context_id, uint32 *p_yuv_mode);

        Rval = img_ctx_ivd_set_yuv_mode(pMode->ContextId, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_yuv_mode", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_yuv_mode(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_yuv_mode", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_yuv_mode(pMode->ContextId, &yuv_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_yuv_mode", "test if (p_yuv_mode != NULL) true case", id_cnt++);
    }

    {
        // before CE
        ik_wb_gain_t before_ce_wb_gain = {0};
        extern uint32 img_ctx_ivd_set_before_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_before_ce_wb_gain);
        extern uint32 img_ctx_ivd_get_before_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_before_ce_wb_gain);

        Rval = img_ctx_ivd_set_before_ce_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_before_ce_wb_gain", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_before_ce_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_before_ce_wb_gain", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_before_ce_wb_gain(pMode->ContextId, &before_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_before_ce_wb_gain", "test if (p_before_ce_wb_gain == NULL) false case", id_cnt++);
    }

    {
        // after CE
        ik_wb_gain_t after_ce_wb_gain = {0};
        extern uint32 img_ctx_ivd_set_after_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_after_ce_wb_gain);
        extern uint32 img_ctx_ivd_get_after_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_after_ce_wb_gain);

        Rval = img_ctx_ivd_set_after_ce_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_after_ce_wb_gain", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_after_ce_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_after_ce_wb_gain", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_after_ce_wb_gain(pMode->ContextId, &after_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_after_ce_wb_gain", "test if (p_after_ce_wb_gain == NULL) false case", id_cnt++);
    }

    {
        // CFA leakage
        ik_cfa_leakage_filter_t cfa_leakage_filter = {0};
        extern uint32 img_ctx_ivd_set_cfa_leakage_filter(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage_filter);
        extern uint32 img_ctx_ivd_get_cfa_leakage_filter(uint32 context_id, ik_cfa_leakage_filter_t *p_cfa_leakage_filter);

        Rval = img_ctx_ivd_set_cfa_leakage_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_cfa_leakage_filter", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_cfa_leakage_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_cfa_leakage_filter", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_cfa_leakage_filter(pMode->ContextId, &cfa_leakage_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_cfa_leakage_filter", "test if (p_cfa_leakage_filter==NULL) false case", id_cnt++);
    }

    {
        // anti aliasing
        ik_anti_aliasing_t anti_aliasing = {0};
        extern uint32 img_ctx_ivd_set_anti_aliasing(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing);
        extern uint32 img_ctx_ivd_get_anti_aliasing(uint32 context_id, ik_anti_aliasing_t *p_anti_aliasing);

        Rval = img_ctx_ivd_set_anti_aliasing(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_anti_aliasing", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_anti_aliasing(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_anti_aliasing", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_anti_aliasing(pMode->ContextId, &anti_aliasing);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_anti_aliasing", "test if (p_anti_aliasing == NULL) false case", id_cnt++);
    }

    {
        // DBP
        ik_dynamic_bad_pixel_correction_t dynamic_bpc = {0};
        extern uint32 img_ctx_ivd_set_dynamic_bpc(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc);
        extern uint32 img_ctx_ivd_get_dynamic_bpc(uint32 context_id, ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc);

        Rval = img_ctx_ivd_set_dynamic_bpc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_dynamic_bpc", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_dynamic_bpc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_dynamic_bpc", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_dynamic_bpc(pMode->ContextId, &dynamic_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_dynamic_bpc", "test if (p_dynamic_bpc == NULL) false case", id_cnt++);
    }

    {
        // CFA noise
        ik_cfa_noise_filter_t cfa_noise_filter = {0};
        extern uint32 img_ctx_ivd_set_cfa_noise_filter(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter);
        extern uint32 img_ctx_ivd_get_cfa_noise_filter(uint32 context_id, ik_cfa_noise_filter_t *p_cfa_noise_filter);

        Rval = img_ctx_ivd_set_cfa_noise_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_cfa_noise_filter", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_cfa_noise_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_cfa_noise_filter", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_cfa_noise_filter(pMode->ContextId, &cfa_noise_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_cfa_noise_filter", "test if (p_cfa_noise_filter == NULL) false case", id_cnt++);
    }

    {
        // demosaic
        ik_demosaic_t demosaic = {0};
        extern uint32 img_ctx_ivd_set_demosaic(uint32 context_id, const ik_demosaic_t *p_demosaic);
        extern uint32 img_ctx_ivd_get_demosaic(uint32 context_id, ik_demosaic_t *p_demosaic);

        Rval = img_ctx_ivd_set_demosaic(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_demosaic", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_demosaic(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_demosaic", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_demosaic(pMode->ContextId, &demosaic);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_demosaic", "test if (p_demosaic == NULL) false case", id_cnt++);
    }

    {
        // rgb to 12y
        ik_rgb_to_12y_t rgb_to_12y = {0};
        extern uint32 img_ctx_ivd_set_rgb_to_12y(uint32 context_id, const ik_rgb_to_12y_t *p_rgb_to_12y);
        extern uint32 img_ctx_ivd_get_rgb_to_12y(uint32 context_id, ik_rgb_to_12y_t *p_rgb_to_12y);

        Rval = img_ctx_ivd_set_rgb_to_12y(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_rgb_to_12y", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_rgb_to_12y(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_rgb_to_12y", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_rgb_to_12y(pMode->ContextId, &rgb_to_12y);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_rgb_to_12y", "test if (p_rgb_to_12y == NULL) false case", id_cnt++);
    }

    {
        // LNL
        ik_luma_noise_reduction_t luma_noise_reduce = {0};
        extern uint32 img_ctx_ivd_set_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_luma_noise_reduce);
        extern uint32 img_ctx_ivd_get_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_luma_noise_reduce);

        Rval = img_ctx_ivd_set_luma_noise_reduction(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_luma_noise_reduction", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_luma_noise_reduction(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_luma_noise_reduction", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_luma_noise_reduction(pMode->ContextId, &luma_noise_reduce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_luma_noise_reduction", "test if (p_luma_noise_reduce == NULL) false case", id_cnt++);
    }

    {
        // Pre CC gain
        ik_pre_cc_gain_t pre_cc_gain = {0};
        extern uint32 img_ctx_ivd_set_pre_cc_gain(uint32 context_id, const ik_pre_cc_gain_t *p_pre_cc_gain);
        extern uint32 img_ctx_ivd_get_pre_cc_gain(uint32 context_id, ik_pre_cc_gain_t *p_pre_cc_gain);

        Rval = img_ctx_ivd_set_pre_cc_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_pre_cc_gain", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_pre_cc_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_pre_cc_gain", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_pre_cc_gain(pMode->ContextId, &pre_cc_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_pre_cc_gain", "test if (p_pre_cc_gain  == NULL) false case", id_cnt++);
    }

    {
        // CC
        ik_color_correction_t color_correction = {0};
        extern uint32 img_ctx_ivd_set_color_correction(uint32 context_id, const ik_color_correction_t *p_color_correction);
        extern uint32 img_ctx_ivd_get_color_correction(uint32 context_id, ik_color_correction_t *p_color_correction);

        Rval = img_ctx_ivd_set_color_correction(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_color_correction", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_color_correction(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_color_correction", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_color_correction(pMode->ContextId, &color_correction);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_color_correction", "test if (p_color_correction == NULL) false case", id_cnt++);
    }

    {
        // tone curve
        ik_tone_curve_t tone_curve = {0};
        extern uint32 img_ctx_ivd_set_tone_curve(uint32 context_id, const ik_tone_curve_t *p_tone_curve);
        extern uint32 img_ctx_ivd_get_tone_curve(uint32 context_id, ik_tone_curve_t *p_tone_curve);

        Rval = img_ctx_ivd_set_tone_curve(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_tone_curve", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_tone_curve(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_tone_curve", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_tone_curve(pMode->ContextId, &tone_curve);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_tone_curve", "test if (p_tone_curve == NULL) false case", id_cnt++);
    }

    {
        // rgb 2 yuv
        ik_rgb_to_yuv_matrix_t rgb_to_yuv_matrix = {0};
        extern uint32 img_ctx_ivd_set_rgb_to_yuv_matrix(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);
        extern uint32 img_ctx_ivd_get_rgb_to_yuv_matrix(uint32 context_id, ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);

        Rval = img_ctx_ivd_set_rgb_to_yuv_matrix(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_rgb_to_yuv_matrix", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_rgb_to_yuv_matrix(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_rgb_to_yuv_matrix", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_rgb_to_yuv_matrix(pMode->ContextId, &rgb_to_yuv_matrix);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_rgb_to_yuv_matrix", "test if (p_rgb_to_yuv_matrix == NULL) false case", id_cnt++);
    }

    {
        // rgbir
        ik_rgb_ir_t rgb_ir = {0};
        extern uint32 img_ctx_ivd_set_rgb_ir(uint32 context_id, const ik_rgb_ir_t *p_rgb_ir);
        extern uint32 img_ctx_ivd_get_rgb_ir(uint32 context_id, ik_rgb_ir_t *p_rgb_ir);

        Rval = img_ctx_ivd_set_rgb_ir(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_rgb_ir", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_rgb_ir(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_rgb_ir", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_rgb_ir(pMode->ContextId, &rgb_ir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_rgb_ir", "test if (p_rgb_ir == NULL) false case", id_cnt++);
    }

    {
        // chroma scale
        ik_chroma_scale_t chroma_scale = {0};
        extern uint32 img_ctx_ivd_set_chroma_scale(uint32 context_id, const ik_chroma_scale_t *p_chroma_scale);
        extern uint32 img_ctx_ivd_get_chroma_scale(uint32 context_id, ik_chroma_scale_t *p_chroma_scale);

        Rval = img_ctx_ivd_set_chroma_scale(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_chroma_scale", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_chroma_scale(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_chroma_scale", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_chroma_scale(pMode->ContextId, &chroma_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_chroma_scale", "test if (p_chroma_scale == NULL) false case", id_cnt++);
    }

    {
        // chroma median
        ik_chroma_median_filter_t chroma_median_filter = {0};
        extern uint32 img_ctx_ivd_set_chroma_median_filter(uint32 context_id, const ik_chroma_median_filter_t *p_chroma_median_filter);
        extern uint32 img_ctx_ivd_get_chroma_median_filter(uint32 context_id, ik_chroma_median_filter_t *p_chroma_median_filter);

        Rval = img_ctx_ivd_set_chroma_median_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_chroma_median_filter", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_chroma_median_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_chroma_median_filter", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_chroma_median_filter(pMode->ContextId, &chroma_median_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_chroma_median_filter", "test if (p_chroma_median_filter == NULL) false case", id_cnt++);
    }

    {
        // luma processing mode
        ik_first_luma_process_mode_t first_luma_process_mode = {0};
        extern uint32 img_ctx_ivd_set_first_luma_processing_mode(uint32 context_id, const ik_first_luma_process_mode_t *p_first_luma_process_mode);
        extern uint32 img_ctx_ivd_get_first_luma_processing_mode(uint32 context_id, ik_first_luma_process_mode_t *p_first_luma_process_mode);

        Rval = img_ctx_ivd_set_first_luma_processing_mode(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_first_luma_processing_mode", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_first_luma_processing_mode(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_first_luma_processing_mode", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_first_luma_processing_mode(pMode->ContextId, &first_luma_process_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_first_luma_processing_mode", "test if (p_first_luma_process_mode == NULL) false case", id_cnt++);
    }

    {
        // ASF
        ik_adv_spatial_filter_t advance_spatial_filter = {0};
        extern uint32 img_ctx_ivd_set_advance_spatial_filter(uint32 context_id, const ik_adv_spatial_filter_t *p_advance_spatial_filter);
        extern uint32 img_ctx_ivd_get_advance_spatial_filter(uint32 context_id, ik_adv_spatial_filter_t *p_advance_spatial_filter);

        Rval = img_ctx_ivd_set_advance_spatial_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_advance_spatial_filter", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_advance_spatial_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_advance_spatial_filter", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_advance_spatial_filter(pMode->ContextId, &advance_spatial_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_advance_spatial_filter", "test if (p_advance_spatial_filter == NULL) false case", id_cnt++);
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
        extern uint32 img_ctx_ivd_set_fstshpns_both(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both);
        extern uint32 img_ctx_ivd_set_fstshpns_noise(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise);
        extern uint32 img_ctx_ivd_set_fstshpns_fir(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir);
        extern uint32 img_ctx_ivd_set_fstshpns_coring(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring);
        extern uint32 img_ctx_ivd_set_fstshpns_cor_idx_scl(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale);
        extern uint32 img_ctx_ivd_set_fstshpns_min_coring_result(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_fstshpns_min_coring_result);
        extern uint32 img_ctx_ivd_set_fstshpns_max_coring_result(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_fstshpns_max_coring_result);
        extern uint32 img_ctx_ivd_set_fstshpns_scale_coring(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_fstshpns_scale_coring);
        extern uint32 img_ctx_ivd_get_fstshpns_both(uint32 context_id, ik_first_sharpen_both_t *p_first_sharpen_both);
        extern uint32 img_ctx_ivd_get_fstshpns_noise(uint32 context_id, ik_first_sharpen_noise_t *p_first_sharpen_noise);
        extern uint32 img_ctx_ivd_get_fstshpns_fir(uint32 context_id, ik_first_sharpen_fir_t *p_first_sharpen_fir);
        extern uint32 img_ctx_ivd_get_fstshpns_coring(uint32 context_id, ik_first_sharpen_coring_t *p_first_sharpen_coring);
        extern uint32 img_ctx_ivd_get_fstshpns_cor_idx_scl(uint32 context_id, ik_first_sharpen_coring_idx_scale_t *p_fstshpns_coring_index_scale);
        extern uint32 img_ctx_ivd_get_fstshpns_min_coring_result(uint32 context_id, ik_first_sharpen_min_coring_result_t *p_fstshpns_min_coring_result);
        extern uint32 img_ctx_ivd_get_fstshpns_max_coring_result(uint32 context_id, ik_first_sharpen_max_coring_result_t *p_fstshpns_max_coring_result);
        extern uint32 img_ctx_ivd_get_fstshpns_scale_coring(uint32 context_id, ik_first_sharpen_scale_coring_t *p_fstshpns_scale_coring);

        Rval = img_ctx_ivd_set_fstshpns_both(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fstshpns_both", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_fstshpns_noise(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fstshpns_noise", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_fstshpns_fir(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fstshpns_fir", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_fstshpns_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fstshpns_coring", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_fstshpns_cor_idx_scl(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fstshpns_cor_idx_scl", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_fstshpns_min_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fstshpns_min_coring_result", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_fstshpns_max_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fstshpns_max_coring_result", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_fstshpns_scale_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fstshpns_scale_coring", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fstshpns_both(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fstshpns_both", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fstshpns_noise(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fstshpns_noise", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fstshpns_fir(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fstshpns_fir", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fstshpns_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fstshpns_coring", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fstshpns_cor_idx_scl(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fstshpns_cor_idx_scl", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fstshpns_min_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fstshpns_min_coring_result", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fstshpns_max_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fstshpns_max_coring_result", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fstshpns_scale_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fstshpns_scale_coring", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_fstshpns_both(pMode->ContextId, &first_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fstshpns_both", "test if (p_first_sharpen_both == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_fstshpns_noise(pMode->ContextId, &first_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fstshpns_noise", "test if (p_first_sharpen_noise == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_fstshpns_fir(pMode->ContextId, &first_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fstshpns_fir", "test if (p_first_sharpen_fir == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_fstshpns_coring(pMode->ContextId, &first_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fstshpns_coring", "test if (p_first_sharpen_coring == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_fstshpns_cor_idx_scl(pMode->ContextId, &fstshpns_coring_index_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fstshpns_cor_idx_scl", "test if (p_fstshpns_coring_index_scale == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_fstshpns_min_coring_result(pMode->ContextId, &fstshpns_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fstshpns_min_coring_result", "test if (p_fstshpns_min_coring_result == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_fstshpns_max_coring_result(pMode->ContextId, &fstshpns_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fstshpns_max_coring_result", "test if (p_fstshpns_max_coring_result == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_fstshpns_scale_coring(pMode->ContextId, &fstshpns_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fstshpns_scale_coring", "test if (p_fstshpns_scale_coring == NULL) false case", id_cnt++);
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
        extern uint32 img_ctx_ivd_set_fnlshpns_both(uint32 context_id, const ik_final_sharpen_both_t *p_final_sharpen_both);
        extern uint32 img_ctx_ivd_set_fnlshpns_noise(uint32 context_id, const ik_final_sharpen_noise_t *p_final_sharpen_noise);
        extern uint32 img_ctx_ivd_set_fnlshpns_fir(uint32 context_id, const ik_final_sharpen_fir_t *p_final_sharpen_fir);
        extern uint32 img_ctx_ivd_set_fnlshpns_coring(uint32 context_id, const ik_final_sharpen_coring_t *p_final_sharpen_coring);
        extern uint32 img_ctx_ivd_set_fnlshpns_cor_idx_scl(uint32 context_id, const ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale);
        extern uint32 img_ctx_ivd_set_fnlshpns_min_coring_result(uint32 context_id, const ik_final_sharpen_min_coring_result_t *p_fnlshpns_min_coring_result);
        extern uint32 img_ctx_ivd_set_fnlshpns_max_coring_result(uint32 context_id, const ik_final_sharpen_max_coring_result_t *p_fnlshpns_max_coring_result);
        extern uint32 img_ctx_ivd_set_fnlshpns_scale_coring(uint32 context_id, const ik_final_sharpen_scale_coring_t *p_fnlshpns_scale_coring);
        extern uint32 img_ctx_ivd_set_fnlshpns_both_tdt(uint32 context_id, const ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table);
        extern uint32 img_ctx_ivd_get_fnlshpns_both(uint32 context_id, ik_final_sharpen_both_t *p_final_sharpen_both);
        extern uint32 img_ctx_ivd_get_fnlshpns_noise(uint32 context_id, ik_final_sharpen_noise_t *p_final_sharpen_noise);
        extern uint32 img_ctx_ivd_get_fnlshpns_fir(uint32 context_id, ik_final_sharpen_fir_t *p_final_sharpen_fir);
        extern uint32 img_ctx_ivd_get_fnlshpns_coring(uint32 context_id, ik_final_sharpen_coring_t *p_final_sharpen_coring);
        extern uint32 img_ctx_ivd_get_fnlshpns_cor_idx_scl(uint32 context_id, ik_final_sharpen_coring_idx_scale_t *p_fnlshpns_coring_index_scale);
        extern uint32 img_ctx_ivd_get_fnlshpns_min_coring_result(uint32 context_id, ik_final_sharpen_min_coring_result_t *p_fnlshpns_min_coring_result);
        extern uint32 img_ctx_ivd_get_fnlshpns_max_coring_result(uint32 context_id, ik_final_sharpen_max_coring_result_t *p_fnlshpns_max_coring_result);
        extern uint32 img_ctx_ivd_get_fnlshpns_scale_coring(uint32 context_id, ik_final_sharpen_scale_coring_t *p_fnlshpns_scale_coring);
        extern uint32 img_ctx_ivd_get_fnlshpns_both_tdt(uint32 context_id, ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table);

        Rval = img_ctx_ivd_set_fnlshpns_both(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fnlshpns_both", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_fnlshpns_noise(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fnlshpns_noise", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_fnlshpns_fir(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fnlshpns_fir", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_fnlshpns_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fnlshpns_coring", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_fnlshpns_cor_idx_scl(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fnlshpns_cor_idx_scl", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_fnlshpns_min_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fnlshpns_min_coring_result", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_fnlshpns_max_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fnlshpns_max_coring_result", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_fnlshpns_scale_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fnlshpns_scale_coring", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_fnlshpns_both_tdt(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fnlshpns_both_tdt", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_both(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_both", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_noise(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_noise", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_fir(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_fir", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_coring", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_cor_idx_scl(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_cor_idx_scl", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_min_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_min_coring_result", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_max_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_max_coring_result", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_scale_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_scale_coring", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_both_tdt(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_both_tdt", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_fnlshpns_both(pMode->ContextId, &final_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_both", "test if (p_final_sharpen_both == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_noise(pMode->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_noise", "test if (p_final_sharpen_noise == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_fir(pMode->ContextId, &final_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_fir", "test if (p_final_sharpen_fir == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_coring(pMode->ContextId, &final_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_coring", "test if (p_final_sharpen_coring == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_cor_idx_scl(pMode->ContextId, &fnlshpns_coring_index_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_cor_idx_scl", "test if (p_fnlshpns_coring_index_scale == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_min_coring_result(pMode->ContextId, &fnlshpns_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_min_coring_result", "test if (p_fnlshpns_min_coring_result == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_max_coring_result(pMode->ContextId, &fnlshpns_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_max_coring_result", "test if (p_fnlshpns_max_coring_result == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_scale_coring(pMode->ContextId, &fnlshpns_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_scale_coring", "test if (p_fnlshpns_scale_coring == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_fnlshpns_both_tdt(pMode->ContextId, &final_sharpen_both_three_d_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fnlshpns_both_tdt", "test if (p_final_sharpen_both_three_d_table == NULL) false case", id_cnt++);
    }

    {
        // chroma filter
        ik_chroma_filter_t chroma_filter = {0};
        extern uint32 img_ctx_ivd_set_chroma_filter(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter);
        extern uint32 img_ctx_ivd_get_chroma_filter(uint32 context_id, ik_chroma_filter_t *p_chroma_filter);

        Rval = img_ctx_ivd_set_chroma_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_chroma_filter", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_chroma_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_chroma_filter", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_chroma_filter(pMode->ContextId, &chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_chroma_filter", "test if (p_chroma_filter == NULL) false case", id_cnt++);
    }

    {
        // wide chroma filter
        ik_wide_chroma_filter_t wide_chroma_filter = {0};
        extern uint32 img_ctx_ivd_set_wide_chroma_ft(uint32 context_id, const ik_wide_chroma_filter_t *p_wide_chroma_filter);
        extern uint32 img_ctx_ivd_get_wide_chroma_ft(uint32 context_id, ik_wide_chroma_filter_t *p_wide_chroma_filter);

        Rval = img_ctx_ivd_set_wide_chroma_ft(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_wide_chroma_ft", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_wide_chroma_ft(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_wide_chroma_ft", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_wide_chroma_ft(pMode->ContextId, &wide_chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_wide_chroma_ft", "test if (p_wide_chroma_filter == NULL) false case", id_cnt++);
    }

    {
        // wide chroma filter combine
        ik_wide_chroma_filter_combine_t chroma_filter_combine = {0};
        extern uint32 img_ctx_ivd_set_wide_chroma_ft_combine(uint32 context_id, const ik_wide_chroma_filter_combine_t *p_chroma_filter_combine);
        extern uint32 img_ctx_ivd_get_wide_chroma_ft_combine(uint32 context_id, ik_wide_chroma_filter_combine_t *p_chroma_filter_combine);

        Rval = img_ctx_ivd_set_wide_chroma_ft_combine(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_wide_chroma_ft_combine", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_wide_chroma_ft_combine(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_wide_chroma_ft_combine", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_wide_chroma_ft_combine(pMode->ContextId, &chroma_filter_combine);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_wide_chroma_ft_combine", "test if (p_chroma_filter_combine == NULL) false case", id_cnt++);
    }

    {
        // grgb mismatch
        ik_grgb_mismatch_t grgb_mismatch = {0};
        extern uint32 img_ctx_ivd_set_grgb_mismatch(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch);
        extern uint32 img_ctx_ivd_get_grgb_mismatch(uint32 context_id, ik_grgb_mismatch_t *p_grgb_mismatch);

        Rval = img_ctx_ivd_set_grgb_mismatch(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_grgb_mismatch", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_grgb_mismatch(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_grgb_mismatch", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_grgb_mismatch(pMode->ContextId, &grgb_mismatch);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_grgb_mismatch", "test if (p_grgb_mismatch == NULL) false case", id_cnt++);
    }

    {
        // MCTF
        ik_video_mctf_t video_mctf = {0};
        ik_video_mctf_ta_t video_mctf_ta = {0};
        ik_pos_dep33_t video_mctf_and_final_sharpen = {0};
        extern uint32 img_ctx_ivd_set_video_mctf(uint32 context_id, const ik_video_mctf_t *p_video_mctf);
        extern uint32 img_ctx_set_video_mctf(uint32 context_id, const ik_video_mctf_t *p_video_mctf);
        extern uint32 img_ctx_ivd_set_mctf_ta(uint32 context_id, const ik_video_mctf_ta_t *p_video_mctf_ta);
        extern uint32 img_ctx_ivd_set_mctf_and_final_sharpen(uint32 context_id, const ik_pos_dep33_t *p_video_mctf_and_final_sharpen);
        extern uint32 img_ctx_ivd_get_video_mctf(uint32 context_id, ik_video_mctf_t *p_video_mctf);
        extern uint32 img_ctx_ivd_get_mctf_ta(uint32 context_id, ik_video_mctf_ta_t *p_video_mctf_ta);
        extern uint32 img_ctx_ivd_get_mctf_and_final_sharpen(uint32 context_id, ik_pos_dep33_t *p_video_mctf_and_final_sharpen);

        Rval = img_ctx_ivd_set_video_mctf(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_video_mctf", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_mctf_ta(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_mctf_ta", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_mctf_and_final_sharpen(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_mctf_and_final_sharpen", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_video_mctf(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_video_mctf", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_mctf_ta(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_mctf_ta", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_mctf_and_final_sharpen(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_mctf_and_final_sharpen", "test invalid API fail case", id_cnt++);

        p_ctx->filters.update_flags.iso.is_1st_frame = 1;
        Rval = img_ctx_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_set_video_mctf", "test p_ctx->filters.update_flags.iso.is_1st_frame == 0U false case", id_cnt++);
        p_ctx->filters.update_flags.iso.is_1st_frame = 0;

        p_ctx->filters.input_param.video_mctf.compression_enable = 0;
        video_mctf.compression_enable = 1;
        Rval = img_ctx_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_set_video_mctf", "test p_ctx->filters.input_param.video_mctf.compression_enable != p_video_mctf->compression_enable true case", id_cnt++);

        video_mctf.compression_enable = 0;
        Rval = img_ctx_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_set_video_mctf", "test p_ctx->filters.input_param.video_mctf.compression_enable != p_video_mctf->compression_enable false case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.compression_bit_rate_luma = 20;
        video_mctf.compression_bit_rate_luma = 10;
        Rval = img_ctx_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_set_video_mctf", "test p_ctx->filters.input_param.video_mctf.compression_bit_rate_luma != p_video_mctf->compression_bit_rate_luma true case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.compression_bit_rate_luma = 10;
        video_mctf.compression_bit_rate_luma = 10;
        Rval = img_ctx_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_set_video_mctf", "test p_ctx->filters.input_param.video_mctf.compression_bit_rate_luma != p_video_mctf->compression_bit_rate_luma false case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.compression_bit_rate_chroma = 20;
        video_mctf.compression_bit_rate_chroma = 10;
        Rval = img_ctx_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_set_video_mctf", "test p_ctx->filters.input_param.video_mctf.compression_bit_rate_chroma != p_video_mctf->compression_bit_rate_chroma true case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.compression_bit_rate_chroma = 10;
        video_mctf.compression_bit_rate_chroma = 10;
        Rval = img_ctx_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_set_video_mctf", "test p_ctx->filters.input_param.video_mctf.compression_bit_rate_chroma != p_video_mctf->compression_bit_rate_chroma false case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.compression_dither_disable = 1;
        video_mctf.compression_dither_disable = 0;
        Rval = img_ctx_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_set_video_mctf", "test p_ctx->filters.input_param.video_mctf.compression_dither_disable != p_video_mctf->compression_dither_disable true case", id_cnt++);

        Rval = img_ctx_ivd_get_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_video_mctf", "test if (p_video_mctf == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_mctf_ta(pMode->ContextId, &video_mctf_ta);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_mctf_ta", "test if (p_video_mctf_ta == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_mctf_and_final_sharpen(pMode->ContextId, &video_mctf_and_final_sharpen);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_mctf_and_final_sharpen", "test if (p_video_mctf_and_final_sharpen == NULL) false case", id_cnt++);
    }

    {
        // AAA
        ik_aaa_stat_info_t stat_info = {0};
        ik_aaa_pg_af_stat_info_t pg_af_stat_info = {0};
        ik_af_stat_ex_info_t af_stat_ex_info = {0};
        ik_pg_af_stat_ex_info_t pg_af_stat_ex_info = {0};
        ik_histogram_info_t hist_info = {0};
        extern uint32 img_ctx_ivd_set_aaa_stat_info(uint32 context_id, const ik_aaa_stat_info_t *p_stat_info);
        extern uint32 img_ctx_ivd_set_aaa_pg_af_info(uint32 context_id, const ik_aaa_pg_af_stat_info_t *p_pg_af_stat_info);
        extern uint32 img_ctx_ivd_set_af_stat_ex_info(uint32 context_id, const ik_af_stat_ex_info_t *p_af_stat_ex_info);
        extern uint32 img_ctx_ivd_set_pg_af_stat_ex_info(uint32 context_id, const ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info);
        extern uint32 img_ctx_ivd_set_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info);
        extern uint32 img_ctx_ivd_set_pg_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info);
        extern uint32 img_ctx_ivd_get_aaa_stat_info(uint32 context_id, ik_aaa_stat_info_t *p_stat_info);
        extern uint32 img_ctx_ivd_get_aaa_pg_af_info(uint32 context_id, ik_aaa_pg_af_stat_info_t *p_pg_af_stat_info);
        extern uint32 img_ctx_ivd_get_af_stat_ex_info(uint32 context_id, ik_af_stat_ex_info_t *p_af_stat_ex_info);
        extern uint32 img_ctx_ivd_get_pg_af_stat_ex_info(uint32 context_id, ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info);
        extern uint32 img_ctx_ivd_get_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info);
        extern uint32 img_ctx_ivd_get_pg_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info);

        Rval = img_ctx_ivd_set_aaa_stat_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_aaa_stat_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_aaa_pg_af_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_aaa_pg_af_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_af_stat_ex_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_af_stat_ex_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_pg_af_stat_ex_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_pg_af_stat_ex_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_histogram_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_histogram_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_set_pg_histogram_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_pg_histogram_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_aaa_stat_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_aaa_stat_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_aaa_pg_af_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_aaa_pg_af_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_af_stat_ex_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_af_stat_ex_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_pg_af_stat_ex_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_pg_af_stat_ex_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_histogram_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_histogram_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_pg_histogram_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_pg_histogram_info", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_aaa_stat_info(pMode->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_aaa_stat_info", "test if (p_stat_info == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_aaa_pg_af_info(pMode->ContextId, &pg_af_stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_aaa_pg_af_info", "test if (p_pg_af_stat_info == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_af_stat_ex_info(pMode->ContextId, &af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_af_stat_ex_info", "test if (p_af_stat_ex_info == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_pg_af_stat_ex_info(pMode->ContextId, &pg_af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_pg_af_stat_ex_info", "test if (p_pg_af_stat_ex_info == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_histogram_info(pMode->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_histogram_info", "test if (p_hist_info == NULL) false case", id_cnt++);
        Rval = img_ctx_ivd_get_pg_histogram_info(pMode->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_pg_histogram_info", "test if (p_hist_info == NULL) false case", id_cnt++);
    }

    {
        // window size info
        ik_window_size_info_t window_size_info = {0};
        extern uint32 img_ctx_ivd_set_window_size_info(uint32 context_id, const ik_window_size_info_t *p_window_size_info);
        extern uint32 img_ctx_ivd_get_window_size_info(uint32 context_id, ik_window_size_info_t *p_window_size_info);

        Rval = img_ctx_ivd_set_window_size_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_window_size_info", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_window_size_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_window_size_info", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_window_size_info(pMode->ContextId, &window_size_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_window_size_info", "test if (p_window_size_info == NULL) false case", id_cnt++);
    }

    {
        // front-end tone curve
        ik_frontend_tone_curve_t fe_tc = {0};
        extern uint32 img_ctx_ivd_set_fe_tone_curve(uint32 context_id, const ik_frontend_tone_curve_t *p_fe_tc);
        extern uint32 img_ctx_ivd_get_fe_tone_curve(uint32 context_id, ik_frontend_tone_curve_t *p_fe_tc);

        Rval = img_ctx_ivd_set_fe_tone_curve(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_fe_tone_curve", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_fe_tone_curve(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fe_tone_curve", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_fe_tone_curve(pMode->ContextId, &fe_tc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_fe_tone_curve", "test if (p_fe_tc == NULL) false case", id_cnt++);
    }

    {
        // resampler strength
        ik_resampler_strength_t resample_str = {0};
        extern uint32 img_ctx_ivd_set_resampler_str(uint32 context_id, const ik_resampler_strength_t *p_resample_str);
        extern uint32 img_ctx_ivd_get_resampler_str(uint32 context_id, ik_resampler_strength_t *p_resample_str);

        Rval = img_ctx_ivd_set_resampler_str(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_set_resampler_str", "test invalid API fail case", id_cnt++);
        Rval = img_ctx_ivd_get_resampler_str(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_resampler_str", "test invalid API fail case", id_cnt++);

        Rval = img_ctx_ivd_get_resampler_str(pMode->ContextId, &resample_str);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentNormalFilter.c", "img_ctx_ivd_get_resampler_str", "test if (p_resample_str == NULL) false case", id_cnt++);
    }
}

static void IK_TestCovr_ImgContextDebugCheck(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_MODE_CFG_s *pModeY2y)
{
    UINT32 Rval = IK_OK;

    ik_ctx_hook_debug_check();

    {
        // Safety info
        ik_safety_info_t safety_info = {0};
        extern uint32 img_ctx_set_safety_info(uint32 context_id, const ik_safety_info_t *p_safety_info);

        Rval = img_ctx_set_safety_info(33, &safety_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_safety_info", "test get context fail case", id_cnt++);

        safety_info.update_freq = 2;
        Rval = ik_set_safety_info(pMode->ContextId, &safety_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_safety_info", "test (mapping_ring_depth_to_update_freq[p_ctx->organization.attribute.cr_ring_number - 1u][i] == p_safety_info->update_freq) false case", id_cnt++);
        safety_info.update_freq = 0;
        Rval = ik_set_safety_info(pMode->ContextId, &safety_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_safety_info", "test (mapping_ring_depth_to_update_freq[p_ctx->organization.attribute.cr_ring_number - 1u][i] != 0u) false case", id_cnt++);
        safety_info.update_freq = 1;
    }

    {
        // Vin sensor info
        ik_vin_sensor_info_t sensor_info = {0};

        sensor_info.compression = 10;
        Rval = ik_set_vin_sensor_info(pMode->ContextId, &sensor_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_vin_sensor_info", "test (p_vin_sensor->compression>8UL) true case", id_cnt++);
        sensor_info.compression = 300;
        Rval = ik_set_vin_sensor_info(pMode->ContextId, &sensor_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_vin_sensor_info", "test (p_vin_sensor->compression<=259UL) true case", id_cnt++);
        sensor_info.compression = 259;
        Rval = ik_set_vin_sensor_info(pMode->ContextId, &sensor_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_vin_sensor_info", "test (p_vin_sensor->compression<=259UL) false case", id_cnt++);
        sensor_info.compression = 0;
    }

    {
        // front-end tone curve
        ik_frontend_tone_curve_t fe_tc = {0};

        fe_tc.decompand_enable = 1;
        Rval = ik_set_frontend_tone_curve(pMode->ContextId, &fe_tc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fe_tone_curve", "test (p_fe_tc->decompand_enable == 1UL) true case", id_cnt++);
        fe_tc.decompand_table[0] = 1048576;
        Rval = ik_set_frontend_tone_curve(pMode->ContextId, &fe_tc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fe_tone_curve", "test p_vin_sensor->decompand_table out of range fail case", id_cnt++);
        fe_tc.decompand_enable = 0;
        fe_tc.decompand_table[0] = 0;
    }

    {
        // Front-end BLC
        ik_static_blc_level_t frontend_static_blc = {0};

        frontend_static_blc.black_r = 16384;
        Rval = ik_set_exp0_frontend_static_blc(pMode->ContextId, &frontend_static_blc);
        Rval = ik_set_exp1_frontend_static_blc(pMode->ContextId, &frontend_static_blc);
        Rval = ik_set_exp2_frontend_static_blc(pMode->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fe_static_blc", "test frontend_static_blc out of range fail case", id_cnt++);
    }

    {
        // Front-end wb gain
        ik_frontend_wb_gain_t frontend_wb_gain = {0};

        frontend_wb_gain.r_gain = 0x400001;
        Rval = ik_set_exp0_frontend_wb_gain(pMode->ContextId, &frontend_wb_gain);
        Rval = ik_set_exp1_frontend_wb_gain(pMode->ContextId, &frontend_wb_gain);
        Rval = ik_set_exp2_frontend_wb_gain(pMode->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fe_wb_gain", "test frontend_wb_gain out of range fail case", id_cnt++);
    }

    {
        // DBP
        ik_dynamic_bad_pixel_correction_t dynamic_bpc = {0};

        dynamic_bpc.enable = 1;
        Rval = ik_set_dynamic_bad_pixel_corr(pMode->ContextId, &dynamic_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_dynamic_bpc", "test (dynamic_bpc->enable == 1U) true case", id_cnt++);
        dynamic_bpc.enable = 0;
    }

    {
        // AAA
        ik_histogram_info_t hist_info = {0};
        ik_aaa_stat_info_t stat_info = {0};
        ik_aaa_pg_af_stat_info_t pg_af_stat_info = {0};
        ik_af_stat_ex_info_t af_stat_ex_info = {0};
        ik_pg_af_stat_ex_info_t pg_af_stat_ex_info = {0};

        hist_info.ae_tile_mask[0] = 0xFFFFFF1;
        Rval = ik_set_histogram_info(pMode->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "amba_ik_debug_check_histogram_info", "test ae_tile_mask out of range fail case", id_cnt++);

        stat_info.awb_pix_min_value = 20;
        stat_info.awb_pix_max_value = 10;
        Rval = ik_set_aaa_stat_info(pMode->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "aaa_stat_info_valid_range_part1", "test awb_pix_min_value > awb_pix_max_value fail case", id_cnt++);

        stat_info.ae_pix_min_value = 20;
        stat_info.ae_pix_max_value = 10;
        Rval = ik_set_aaa_stat_info(pMode->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "aaa_stat_info_valid_range_part1", "test ae_pix_min_value > ae_pix_max_value fail case", id_cnt++);

        stat_info.awb_tile_col_start = 0;
        stat_info.awb_tile_width = 80;
        stat_info.awb_tile_num_col = 64;
        Rval = ik_set_aaa_stat_info(pMode->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "aaa_stat_info_valid_range_part1", "test awb w out of range fail case", id_cnt++);

        stat_info.awb_tile_row_start = 0;
        stat_info.awb_tile_height = 80;
        stat_info.awb_tile_num_row = 64;
        Rval = ik_set_aaa_stat_info(pMode->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "aaa_stat_info_valid_range_part1", "test awb h out of range fail case", id_cnt++);

        stat_info.ae_tile_col_start = 0;
        stat_info.ae_tile_width = 200;
        stat_info.ae_tile_num_col = 24;
        Rval = ik_set_aaa_stat_info(pMode->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "aaa_stat_info_valid_range_part1", "test ae w out of range fail case", id_cnt++);

        stat_info.ae_tile_row_start = 0;
        stat_info.ae_tile_height = 300;
        stat_info.ae_tile_num_row = 16;
        Rval = ik_set_aaa_stat_info(pMode->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "aaa_stat_info_valid_range_part1", "test ae h out of range fail case", id_cnt++);

        stat_info.af_tile_col_start = 0;
        stat_info.af_tile_width = 200;
        stat_info.af_tile_num_col = 24;
        Rval = ik_set_aaa_stat_info(pMode->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "aaa_stat_info_valid_range_part2", "test af w out of range fail case", id_cnt++);

        stat_info.af_tile_row_start = 0;
        stat_info.af_tile_height = 300;
        stat_info.af_tile_num_row = 16;
        Rval = ik_set_aaa_stat_info(pMode->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "aaa_stat_info_valid_range_part2", "test af h out of range fail case", id_cnt++);

        stat_info.awb_tile_active_width = 64;
        stat_info.awb_tile_width = 32;
        Rval = ik_set_aaa_stat_info(pMode->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "aaa_stat_info_valid_range_part2", "test awb active_w > w fail case", id_cnt++);

        stat_info.awb_tile_active_height = 64;
        stat_info.awb_tile_height = 32;
        Rval = ik_set_aaa_stat_info(pMode->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "aaa_stat_info_valid_range_part2", "test awb active_h > h fail case", id_cnt++);

        stat_info.af_tile_active_width = 24;
        stat_info.af_tile_width = 12;
        Rval = ik_set_aaa_stat_info(pMode->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "aaa_stat_info_valid_range_part2", "test af active_w > w fail case", id_cnt++);

        stat_info.af_tile_active_height = 16;
        stat_info.af_tile_height = 8;
        Rval = ik_set_aaa_stat_info(pMode->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "aaa_stat_info_valid_range_part2", "test af active_h > h fail case", id_cnt++);

        pg_af_stat_info.af_tile_col_start = 0;
        pg_af_stat_info.af_tile_width = 200;
        pg_af_stat_info.af_tile_num_col = 24;
        Rval = ik_set_aaa_pg_af_stat_info(pMode->ContextId, &pg_af_stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_aaa_pg_af_info_valid_range", "test pg af w out of range fail case", id_cnt++);

        pg_af_stat_info.af_tile_row_start = 0;
        pg_af_stat_info.af_tile_height = 300;
        pg_af_stat_info.af_tile_num_row = 16;
        Rval = ik_set_aaa_pg_af_stat_info(pMode->ContextId, &pg_af_stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_aaa_pg_af_info_valid_range", "test pg af h out of range fail case", id_cnt++);

        pg_af_stat_info.af_tile_active_width = 24;
        pg_af_stat_info.af_tile_width = 12;
        Rval = ik_set_aaa_pg_af_stat_info(pMode->ContextId, &pg_af_stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_aaa_pg_af_info_valid_range", "test pg af active_w > w fail case", id_cnt++);

        pg_af_stat_info.af_tile_active_height = 16;
        pg_af_stat_info.af_tile_height = 8;
        Rval = ik_set_aaa_pg_af_stat_info(pMode->ContextId, &pg_af_stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_aaa_pg_af_info_valid_range", "test pg af active_h > h fail case", id_cnt++);

        af_stat_ex_info.af_horizontal_filter1_thresh = 2048;
        Rval = ik_set_af_stat_ex_info(pMode->ContextId, &af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_af_stat_ex_info_valid_range", "test af_horizontal_filter1_thresh out of range fail case", id_cnt++);

        pg_af_stat_ex_info.af_horizontal_filter1_thresh = 2048;
        Rval = ik_set_pg_af_stat_ex_info(pMode->ContextId, &pg_af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_pg_af_stat_ex_info_valid_range", "test af_horizontal_filter1_thresh out of range fail case", id_cnt++);

        hist_info.ae_tile_mask[0] = 0xFFFFFF1;
        Rval = ik_set_pg_histogram_info(pMode->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "amba_ik_debug_check_histogram_info", "test ae_tile_mask out of range fail case", id_cnt++);
    }

    {
        // SBP
        ik_static_bad_pxl_cor_t static_bpc = {0};
        ik_static_bad_pixel_correction_internal_t static_bpc_internal = {0};

        static_bpc.vin_sensor_geo.width = 100;
        Rval = ik_set_static_bad_pxl_corr(pMode->ContextId, &static_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_bpc_valid_range", "test vin_sensor_geo.width not 32 alignment fail case", id_cnt++);

        Rval = ik_set_static_bad_pxl_corr_itnl(pMode->ContextId, &static_bpc_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_static_bpc_internal_valid_range", "test p_map == NULL fail case", id_cnt++);
    }

    {
        // anti aliasing
        ik_anti_aliasing_t anti_aliasing = {0};

        anti_aliasing.enable = 4;
        Rval = ik_set_anti_aliasing(pMode->ContextId, &anti_aliasing);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_anti_aliasing_valid_range", "test (anti_aliasing->enable == 4U) true case", id_cnt++);

        anti_aliasing.log_fractional_correct = 8;
        Rval = ik_set_anti_aliasing(pMode->ContextId, &anti_aliasing);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_anti_aliasing_valid_range", "test log_fractional_correct out of range fail case", id_cnt++);
    }

    {
        // Warp
        ik_warp_internal_info_t warp_internal = {0};
        ik_warp_info_t calib_warp_info = {0};

        Rval = ik_set_warp_internal(pMode->ContextId, &warp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_warp_internal_valid_range", "test (warp_internal->pwarp_horizontal_table == NULL) true case", id_cnt++);

        warp_internal.pwarp_horizontal_table = (int16 *)WarpGrid;
        Rval = ik_set_warp_internal(pMode->ContextId, &warp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_warp_internal_valid_range", "test (warp_internal->pwarp_vertical_table == NULL) true case", id_cnt++);

        warp_internal.pwarp_vertical_table = (int16 *)(WarpGrid + (128*96*4));
        warp_internal.grid_array_width = WARP_VIDEO_MAXHORGRID + 1;
        Rval = ik_set_warp_internal(pMode->ContextId, &warp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_warp_internal_valid_range", "test grid_array_width out of range fail case", id_cnt++);

        warp_internal.grid_array_height = WARP_VIDEO_MAXVERGRID + 1;
        Rval = ik_set_warp_internal(pMode->ContextId, &warp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_warp_internal_valid_range", "test grid_array_height out of range fail case", id_cnt++);

        warp_internal.vert_warp_grid_array_width = WARP_VIDEO_MAXHORGRID + 1;
        Rval = ik_set_warp_internal(pMode->ContextId, &warp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_warp_internal_valid_range", "test vert_warp_grid_array_width out of range fail case", id_cnt++);

        warp_internal.vert_warp_grid_array_height = WARP_VIDEO_MAXVERGRID + 1;
        Rval = ik_set_warp_internal(pMode->ContextId, &warp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_warp_internal_valid_range", "test vert_warp_grid_array_height out of range fail case", id_cnt++);

        Rval = ik_set_warp_info(pMode->ContextId, &calib_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_calib_warp_info_valid_range", "test pwarp == NULL fail case", id_cnt++);

        calib_warp_info.vin_sensor_geo.width = 1920;
        calib_warp_info.hor_grid_num = 20;
        calib_warp_info.tile_width_exp = 4;
        Rval = ik_set_warp_info(pMode->ContextId, &calib_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_calib_warp_info_valid_range", "test h grid can not cover vin width fail case", id_cnt++);

        calib_warp_info.vin_sensor_geo.height = 1080;
        calib_warp_info.ver_grid_num = 20;
        calib_warp_info.tile_height_exp = 4;
        Rval = ik_set_warp_info(pMode->ContextId, &calib_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_calib_warp_info_valid_range", "test v grid can not cover vin height fail case", id_cnt++);

        calib_warp_info.enb_2_stage_compensation = 1;
        Rval = ik_set_warp_info(pMode->ContextId, &calib_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_calib_warp_info_valid_range", "test enb_2_stage_compensation not support fail case", id_cnt++);
    }

    {
        // CA
        ik_cawarp_info_t calib_ca_warp_info = {0};
        ik_cawarp_internal_info_t cawarp_internal = {0};

        Rval = ik_set_cawarp_info(pMode->ContextId, &calib_ca_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_calib_ca_warp_info_valid_range", "test NULL CA tables fail case", id_cnt++);

        calib_ca_warp_info.hor_grid_num = 10;
        calib_ca_warp_info.tile_width_exp = 4;
        calib_ca_warp_info.vin_sensor_geo.width = 1920;
        Rval = ik_set_cawarp_info(pMode->ContextId, &calib_ca_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_calib_ca_warp_info_valid_range", "test horizontal CA out of range fail case", id_cnt++);

        calib_ca_warp_info.ver_grid_num = 10;
        calib_ca_warp_info.tile_height_exp = 4;
        calib_ca_warp_info.vin_sensor_geo.height = 1080;
        Rval = ik_set_cawarp_info(pMode->ContextId, &calib_ca_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_calib_ca_warp_info_valid_range", "test vertical CA out of range fail case", id_cnt++);

        calib_ca_warp_info.Enb2StageCompensation = 1;
        Rval = ik_set_cawarp_info(pMode->ContextId, &calib_ca_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_calib_ca_warp_info_valid_range", "test Enb2StageCompensation out of range fail case", id_cnt++);

        cawarp_internal.horz_pass_grid_array_width = 33;
        Rval = ik_set_cawarp_internal(pMode->ContextId, &cawarp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_ca_warp_internal_valid_range", "test horz_pass_grid_array_width out of range fail case", id_cnt++);

        cawarp_internal.horz_pass_grid_array_height = 50;
        Rval = ik_set_cawarp_internal(pMode->ContextId, &cawarp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_ca_warp_internal_valid_range", "test horz_pass_grid_array_height out of range fail case", id_cnt++);

        cawarp_internal.vert_pass_grid_array_width = 33;
        Rval = ik_set_cawarp_internal(pMode->ContextId, &cawarp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_ca_warp_internal_valid_range", "test vert_pass_grid_array_width out of range fail case", id_cnt++);

        cawarp_internal.vert_pass_grid_array_height = 50;
        Rval = ik_set_cawarp_internal(pMode->ContextId, &cawarp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_ca_warp_internal_valid_range", "test vert_pass_grid_array_height out of range fail case", id_cnt++);
    }

    {
        // CE
        ik_ce_t ce = {0};
        ik_ce_input_table_t ce_input_table = {0};
        ik_ce_output_table_t ce_out_table = {0};
        ce.coring_gain_high = 1U;
        ce.coring_index_scale_high = 1U;

        ce.boost_table[0] = 200;
        Rval = ik_set_ce(pMode->ContextId, &ce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_ce_valid_range", "test boost_table out of range fail case", id_cnt++);

        ce_input_table.input_table[0] = 1048576;
        Rval = ik_set_ce_input_table(pMode->ContextId, &ce_input_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_ce_input_table_valid_range", "test input_table out of range fail case", id_cnt++);

        ce_out_table.output_shift = 20;
        Rval = ik_set_ce_out_table(pMode->ContextId, &ce_out_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_ce_out_table_valid_range", "test output_shift out of range fail case", id_cnt++);
    }

    {
        // CFA noise
        ik_cfa_noise_filter_t cfa_noise_filter = {0};

        cfa_noise_filter.selectivity_regular = 52;
        cfa_noise_filter.selectivity_fine = 52;
        Rval = ik_set_cfa_noise_filter(pMode->ContextId, &cfa_noise_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_cfa_noise_filter_valid_range", "test selectivity_regular and selectivity_fine not 50 alignment fail case", id_cnt++);
    }

    {
        // rgbir
        ik_rgb_ir_t rgb_ir = {0};

        rgb_ir.mode = 2;
        Rval = ik_set_rgb_ir(pMode->ContextId, &rgb_ir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_rgb_ir_valid_range", "test (rgb_ir->mode != 1U) true case", id_cnt++);

        rgb_ir.mode = 3;
        Rval = ik_set_rgb_ir(pMode->ContextId, &rgb_ir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_rgb_ir_valid_range", "test (rgb_ir->mode != 2U) true case", id_cnt++);
    }

    {
        // chroma filter
        ik_chroma_filter_t chroma_filter = {0};

        chroma_filter.radius = 128;
        Rval = ik_set_chroma_filter(pMode->ContextId, &chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_chroma_filter_valid_range", "test (chroma_filter->radius != 64U true case", id_cnt++);

        chroma_filter.radius = 256;
        Rval = ik_set_chroma_filter(pMode->ContextId, &chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_chroma_filter_valid_range", "test (chroma_filter->radius != 128 true case", id_cnt++);
    }

    {
        // ASF
        ik_adv_spatial_filter_t advance_spatial_filter = {0};
        advance_spatial_filter.level_str_adjust.high = 1;

        advance_spatial_filter.enable = 1;
        Rval = ik_set_adv_spatial_filter(pMode->ContextId, &advance_spatial_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_advance_spatial_filter_valid_range", "test max_change_not_t0_t1_alpha != 0 false case", id_cnt++);

        advance_spatial_filter.max_change_not_t0_t1_alpha = 1;
        advance_spatial_filter.adapt.t0_down = 3;
        advance_spatial_filter.adapt.t0_up = 3;
        advance_spatial_filter.adapt.t1_down = 3;
        advance_spatial_filter.adapt.t1_up = 3;
        Rval = ik_set_adv_spatial_filter(pMode->ContextId, &advance_spatial_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_advance_spatial_filter_valid_range", "test t0 t1 not 2 alignment fail case", id_cnt++);

        advance_spatial_filter.adapt.t0_down = 5;
        advance_spatial_filter.adapt.t0_up = 5;
        advance_spatial_filter.adapt.t1_down = 3;
        advance_spatial_filter.adapt.t1_up = 3;
        Rval = ik_set_adv_spatial_filter(pMode->ContextId, &advance_spatial_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_advance_spatial_filter_valid_range", "test t0 > t1 fail case", id_cnt++);
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
        first_sharpen_noise.level_str_adjust.high = 1;
        first_sharpen_coring_idx_scale.high = 1;
        first_sharpen_min_coring_result.high = 1;
        first_sharpen_max_coring_result.high = 1;
        first_sharpen_scale_coring.high = 1;

        first_sharpen_both.mode = 3;
        Rval = ik_set_fst_shp_both(pMode->ContextId, &first_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_both_valid_range", "test mode != 0 or 2 fail case", id_cnt++);

        first_sharpen_noise.spatial_fir.specify = 1;
        Rval = ik_set_fst_shp_noise(pMode->ContextId, &first_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_noise_valid_range", "test spatial_fir.specify == 1 true case", id_cnt++);

        first_sharpen_noise.spatial_fir.specify = 3;
        Rval = ik_set_fst_shp_noise(pMode->ContextId, &first_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_noise_valid_range", "test spatial_fir.specify == 3 true case", id_cnt++);

        first_sharpen_noise.spatial_fir.specify = 4;
        Rval = ik_set_fst_shp_noise(pMode->ContextId, &first_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_noise_valid_range", "test spatial_fir.specify == 4 true case", id_cnt++);

        first_sharpen_noise.spatial_fir.specify = 5;
        Rval = ik_set_fst_shp_noise(pMode->ContextId, &first_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_noise_valid_range", "test spatial_fir.specify != 0 to 4 true case", id_cnt++);

        first_sharpen_noise.t0 = 10;
        first_sharpen_noise.t1 = 5;
        Rval = ik_set_fst_shp_noise(pMode->ContextId, &first_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_noise_valid_range", "test t0 > t1 fail case", id_cnt++);

        first_sharpen_noise.t0 = 10;
        first_sharpen_noise.t1 = 30;
        Rval = ik_set_fst_shp_noise(pMode->ContextId, &first_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_noise_valid_range", "test t1 > t0 over 15 fail case", id_cnt++);

        first_sharpen_fir.specify = 1;
        Rval = ik_set_fst_shp_fir(pMode->ContextId, &first_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_fir_valid_range", "test fir.specify == 1 true case", id_cnt++);

        first_sharpen_fir.specify = 2;
        Rval = ik_set_fst_shp_fir(pMode->ContextId, &first_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_fir_valid_range", "test fir.specify == 2 true case", id_cnt++);

        first_sharpen_fir.specify = 3;
        Rval = ik_set_fst_shp_fir(pMode->ContextId, &first_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_fir_valid_range", "test fir.specify == 3 true case", id_cnt++);

        first_sharpen_fir.specify = 4;
        Rval = ik_set_fst_shp_fir(pMode->ContextId, &first_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_fir_valid_range", "test fir.specify == 4 true case", id_cnt++);

        first_sharpen_fir.specify = 5;
        Rval = ik_set_fst_shp_fir(pMode->ContextId, &first_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_fir_valid_range", "test fir.specify != 0 to 4 true case", id_cnt++);

        first_sharpen_coring.fractional_bits = 5;
        Rval = ik_set_fst_shp_coring(pMode->ContextId, &first_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_coring_valid_range", "test fractional_bits out of range fail case", id_cnt++);

        first_sharpen_coring_idx_scale.low = 255;
        Rval = ik_set_fst_shp_coring_idx_scale(pMode->ContextId, &first_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_coring_index_scale_valid_range", "test low out of range fail case", id_cnt++);

        first_sharpen_min_coring_result.low = 255;
        Rval = ik_set_fst_shp_min_coring_rslt(pMode->ContextId, &first_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_min_coring_result_valid_range", "test low out of range fail case", id_cnt++);

        first_sharpen_max_coring_result.low = 255;
        Rval = ik_set_fst_shp_max_coring_rslt(pMode->ContextId, &first_sharpen_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_max_coring_result_valid_range", "test low out of range fail case", id_cnt++);

        first_sharpen_scale_coring.low = 255;
        Rval = ik_set_fst_shp_scale_coring(pMode->ContextId, &first_sharpen_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fstshpns_scale_coring_valid_range", "test low out of range fail case", id_cnt++);
    }

    {
        // MCTF
        ik_video_mctf_t video_mctf = {0};
        ik_video_mctf_ta_t video_mctf_ta = {0};
        ik_pos_dep33_t video_mctf_and_final_sharpen = {0};
        video_mctf.y_3d_maxchange.high = 1;
        video_mctf.cb_3d_maxchange.high = 1;
        video_mctf.cr_3d_maxchange.high = 1;
        video_mctf.y_level_based_ta.high = 1;
        video_mctf.cb_level_based_ta.high = 1;
        video_mctf.cr_level_based_ta.high = 1;
        video_mctf.y_overall_max_change.high = 1;
        video_mctf.cb_overall_max_change.high = 1;
        video_mctf.cr_overall_max_change.high = 1;
        video_mctf.y_spat_blend.high = 1;
        video_mctf.cb_spat_blend.high = 1;
        video_mctf.cr_spat_blend.high = 1;
        video_mctf.y_spat_filt_max_smth_change.high = 1;
        video_mctf.cb_spat_filt_max_smth_change.high = 1;
        video_mctf.cr_spat_filt_max_smth_change.high = 1;
        video_mctf.y_spat_smth_dir.high = 1;
        video_mctf.cb_spat_smth_dir.high = 1;
        video_mctf.cr_spat_smth_dir.high = 1;
        video_mctf.y_spat_smth_iso.high = 1;
        video_mctf.cb_spat_smth_iso.high = 1;
        video_mctf.cr_spat_smth_iso.high = 1;
        video_mctf.y_spatial_max_temporal.high = 1;
        video_mctf.cb_spatial_max_temporal.high = 1;
        video_mctf.cr_spatial_max_temporal.high = 1;
        video_mctf.y_temporal_min_target.high = 1;
        video_mctf.cb_temporal_min_target.high = 1;
        video_mctf.cr_temporal_min_target.high = 1;
        video_mctf.y_temporal_either_max_change_or_t0_t1_add.high = 1;
        video_mctf.cb_temporal_either_max_change_or_t0_t1_add.high = 1;
        video_mctf.cr_temporal_either_max_change_or_t0_t1_add.high = 1;


        video_mctf.enable = 1;
        video_mctf.y_3d_maxchange.method = 3;
        Rval = ik_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_video_mctf_level_control_valid_range", "test invalid method fail case", id_cnt++);

        video_mctf.y_advanced_iso_max_change_method = 3;
        Rval = ik_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "video_mctf_valid_range_part1", "test invalid y_advanced_iso_max_change_method fail case", id_cnt++);

        video_mctf.cb_advanced_iso_max_change_method = 3;
        Rval = ik_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "video_mctf_valid_range_part1", "test invalid cb_advanced_iso_max_change_method fail case", id_cnt++);

        video_mctf.cr_advanced_iso_max_change_method = 3;
        Rval = ik_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "video_mctf_valid_range_part1", "test invalid cr_advanced_iso_max_change_method fail case", id_cnt++);

        video_mctf.y_overall_max_change.low = 0;
        video_mctf.y_overall_max_change.low_delta = 1;
        video_mctf.y_overall_max_change.high = 1;
        video_mctf.y_overall_max_change.high_delta = 1;
        Rval = ik_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "video_mctf_valid_range_part2", "test invalid cr_advanced_iso_max_change_method fail case", id_cnt++);

        video_mctf.y_spat_smth_dir.low = 0;
        video_mctf.y_spat_smth_dir.low_delta = 1;
        video_mctf.y_spat_smth_dir.high = 1;
        video_mctf.y_spat_smth_dir.high_delta = 1;
        Rval = ik_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "video_mctf_valid_range_part3", "test invalid cr_advanced_iso_max_change_method fail case", id_cnt++);

        video_mctf.y_temporal_min_target.low = 0;
        video_mctf.y_temporal_min_target.low_delta = 1;
        video_mctf.y_temporal_min_target.high = 1;
        video_mctf.y_temporal_min_target.high_delta = 1;
        Rval = ik_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "video_mctf_valid_range_part4", "test invalid cr_advanced_iso_max_change_method fail case", id_cnt++);

        video_mctf_ta.slow_mo_sensitivity = 20;
        Rval = ik_set_video_mctf_ta(pMode->ContextId, &video_mctf_ta);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_video_mctf_ta_valid_range", "test slow_mo_sensitivity out of range fail case", id_cnt++);

        video_mctf_and_final_sharpen.pos_dep[0][0] = 32;
        Rval = ik_set_video_mctf_and_fnl_shp(pMode->ContextId, &video_mctf_and_final_sharpen);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_pos_dep33_valid_range", "test pos_dep out of range fail case", id_cnt++);
    }

    {
        // SHPB
        ik_final_sharpen_both_t final_sharpen_both = {0};
        ik_final_sharpen_noise_t final_sharpen_noise = {0};
        ik_final_sharpen_fir_t final_sharpen_fir = {0};
        ik_final_sharpen_coring_idx_scale_t final_sharpen_coring_idx_scale = {0};
        ik_final_sharpen_min_coring_result_t final_sharpen_min_coring_result = {0};
        ik_final_sharpen_max_coring_result_t final_sharpen_max_coring_result = {0};
        ik_final_sharpen_scale_coring_t final_sharpen_scale_coring = {0};
        ik_final_sharpen_coring_t final_sharpen_coring = {0};
        ik_final_sharpen_both_three_d_table_t final_sharpen_both_three_d_table = {0};
        final_sharpen_noise.level_str_adjust.high = 1;
        final_sharpen_coring_idx_scale.high = 1;
        final_sharpen_min_coring_result.high = 1;
        final_sharpen_max_coring_result.high = 1;
        final_sharpen_scale_coring.high = 1;

        final_sharpen_both.mode = 1;
        Rval = ik_set_fnl_shp_both(pMode->ContextId, &final_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_both_valid_range", "test mode != 0 or 2 fail case", id_cnt++);

        final_sharpen_noise.t0 = 10;
        final_sharpen_noise.t1 = 5;
        Rval = ik_set_fnl_shp_noise(pMode->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "fnlshpns_noise_valid_range_p1_a", "test t0 > t1 fail case", id_cnt++);

        final_sharpen_noise.t0 = 10;
        final_sharpen_noise.t1 = 30;
        Rval = ik_set_fnl_shp_noise(pMode->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "fnlshpns_noise_valid_range_p1_a", "test t1 > t0 over 15 fail case", id_cnt++);

        final_sharpen_noise.level_str_adjust.method = 3;
        Rval = ik_set_fnl_shp_noise(pMode->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "fnlshpns_noise_valid_range_p1_a", "test invalid method fail case", id_cnt++);

        final_sharpen_noise.spatial_fir.specify = 1;
        Rval = ik_set_fnl_shp_noise(pMode->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "fnlshpns_noise_valid_range_p1", "test fir.specify == 1 true case", id_cnt++);

        final_sharpen_noise.spatial_fir.specify = 2;
        Rval = ik_set_fnl_shp_noise(pMode->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "fnlshpns_noise_valid_range_p1", "test fir.specify == 2 true case", id_cnt++);

        final_sharpen_noise.spatial_fir.specify = 3;
        Rval = ik_set_fnl_shp_noise(pMode->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "fnlshpns_noise_valid_range_p1", "test fir.specify == 3 true case", id_cnt++);

        final_sharpen_noise.spatial_fir.specify = 4;
        Rval = ik_set_fnl_shp_noise(pMode->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "fnlshpns_noise_valid_range_p1", "test fir.specify == 4 true case", id_cnt++);

        final_sharpen_noise.spatial_fir.specify = 5;
        Rval = ik_set_fnl_shp_noise(pMode->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "fnlshpns_noise_valid_range_p1", "test fir.specify != 0 to 4 true case", id_cnt++);

        final_sharpen_noise.advanced_iso.max_change_method = 3;
        Rval = ik_set_fnl_shp_noise(pMode->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "fnlshpns_noise_valid_range_p2", "test invalid advanced_iso.max_change_method fail case", id_cnt++);

        final_sharpen_noise.advanced_iso.noise_level_method = 3;
        Rval = ik_set_fnl_shp_noise(pMode->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "fnlshpns_noise_valid_range_p2", "test invalid advanced_iso.noise_level_method fail case", id_cnt++);

        final_sharpen_noise.advanced_iso.str_a_method = 3;
        Rval = ik_set_fnl_shp_noise(pMode->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "fnlshpns_noise_valid_range_p2", "test invalid advanced_iso.str_a_method fail case", id_cnt++);

        final_sharpen_noise.advanced_iso.str_b_method = 3;
        Rval = ik_set_fnl_shp_noise(pMode->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "fnlshpns_noise_valid_range_p2", "test invalid advanced_iso.str_b_method fail case", id_cnt++);

        final_sharpen_fir.specify = 1;
        Rval = ik_set_fnl_shp_fir(pMode->ContextId, &final_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_fir_valid_range", "test fir.specify == 1 true case", id_cnt++);

        final_sharpen_fir.specify = 2;
        Rval = ik_set_fnl_shp_fir(pMode->ContextId, &final_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_fir_valid_range", "test fir.specify == 2 true case", id_cnt++);

        final_sharpen_fir.specify = 3;
        Rval = ik_set_fnl_shp_fir(pMode->ContextId, &final_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_fir_valid_range", "test fir.specify == 3 true case", id_cnt++);

        final_sharpen_fir.specify = 4;
        Rval = ik_set_fnl_shp_fir(pMode->ContextId, &final_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_fir_valid_range", "test fir.specify == 4 true case", id_cnt++);

        final_sharpen_fir.specify = 5;
        Rval = ik_set_fnl_shp_fir(pMode->ContextId, &final_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_fir_valid_range", "test fir.specify != 0 to 4 true case", id_cnt++);

        final_sharpen_coring_idx_scale.method = 3;
        Rval = ik_set_fnl_shp_coring_idx_scale(pMode->ContextId, &final_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_coring_index_scale_valid_range", "test invalid method fail case", id_cnt++);

        final_sharpen_coring_idx_scale.method = 1;
        Rval = ik_set_fnl_shp_coring_idx_scale(pMode->ContextId, &final_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_coring_index_scale_valid_range", "test (fnlshpns_cor_idx_scl->method != 1U) false case", id_cnt++);

        final_sharpen_coring_idx_scale.method = 2;
        Rval = ik_set_fnl_shp_coring_idx_scale(pMode->ContextId, &final_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_coring_index_scale_valid_range", "test (fnlshpns_cor_idx_scl->method != 2U) false case", id_cnt++);

        final_sharpen_coring_idx_scale.method = 6;
        Rval = ik_set_fnl_shp_coring_idx_scale(pMode->ContextId, &final_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_coring_index_scale_valid_range", "test (fnlshpns_cor_idx_scl->method != 6U) false case", id_cnt++);

        final_sharpen_coring_idx_scale.method = 7;
        Rval = ik_set_fnl_shp_coring_idx_scale(pMode->ContextId, &final_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_coring_index_scale_valid_range", "test (fnlshpns_cor_idx_scl->method != 7U) false case", id_cnt++);

        final_sharpen_coring_idx_scale.method = 12;
        Rval = ik_set_fnl_shp_coring_idx_scale(pMode->ContextId, &final_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_coring_index_scale_valid_range", "test (fnlshpns_cor_idx_scl->method != 12U) false case", id_cnt++);

        final_sharpen_coring_idx_scale.method = 14;
        Rval = ik_set_fnl_shp_coring_idx_scale(pMode->ContextId, &final_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_coring_index_scale_valid_range", "test (fnlshpns_cor_idx_scl->method != 14U) false case", id_cnt++);

        final_sharpen_min_coring_result.method = 3;
        Rval = ik_set_fnl_shp_min_coring_rslt(pMode->ContextId, &final_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_min_coring_result_valid_range", "test invalid method fail case", id_cnt++);

        final_sharpen_min_coring_result.method = 1;
        Rval = ik_set_fnl_shp_min_coring_rslt(pMode->ContextId, &final_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_min_coring_result_valid_range", "test (fnlshpns_min_cor_rst->method != 1U) false case", id_cnt++);

        final_sharpen_min_coring_result.method = 2;
        Rval = ik_set_fnl_shp_min_coring_rslt(pMode->ContextId, &final_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_min_coring_result_valid_range", "test (fnlshpns_min_cor_rst->method != 2U) false case", id_cnt++);

        final_sharpen_min_coring_result.method = 6;
        Rval = ik_set_fnl_shp_min_coring_rslt(pMode->ContextId, &final_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_min_coring_result_valid_range", "test (fnlshpns_min_cor_rst->method != 6U) false case", id_cnt++);

        final_sharpen_min_coring_result.method = 7;
        Rval = ik_set_fnl_shp_min_coring_rslt(pMode->ContextId, &final_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_min_coring_result_valid_range", "test (fnlshpns_min_cor_rst->method != 7U) false case", id_cnt++);

        final_sharpen_min_coring_result.method = 12;
        Rval = ik_set_fnl_shp_min_coring_rslt(pMode->ContextId, &final_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_min_coring_result_valid_range", "test (fnlshpns_min_cor_rst->method != 12U) false case", id_cnt++);

        final_sharpen_min_coring_result.method = 14;
        Rval = ik_set_fnl_shp_min_coring_rslt(pMode->ContextId, &final_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_min_coring_result_valid_range", "test (fnlshpns_min_cor_rst->method != 14U) false case", id_cnt++);

        final_sharpen_max_coring_result.method = 3;
        Rval = ik_set_fnl_shp_max_coring_rslt(pMode->ContextId, &final_sharpen_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_max_coring_result_valid_range", "test invalid method fail case", id_cnt++);

        final_sharpen_max_coring_result.high = 128;
        final_sharpen_max_coring_result.high_delta = 7;
        Rval = ik_set_fnl_shp_max_coring_rslt(pMode->ContextId, &final_sharpen_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_max_coring_result_valid_range", "test (High0 > 255UL) fail case", id_cnt++);

        final_sharpen_max_coring_result.low = 128;
        final_sharpen_max_coring_result.low_delta = 7;
        Rval = ik_set_fnl_shp_max_coring_rslt(pMode->ContextId, &final_sharpen_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_max_coring_result_valid_range", "test (Low0 > fnlshpns_max_cor_rst->high) fail case", id_cnt++);

        final_sharpen_scale_coring.method = 3;
        Rval = ik_set_fnl_shp_scale_coring(pMode->ContextId, &final_sharpen_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_scale_coring_valid_range", "test invalid method fail case", id_cnt++);

        final_sharpen_scale_coring.high = 128;
        final_sharpen_scale_coring.high_delta = 7;
        Rval = ik_set_fnl_shp_scale_coring(pMode->ContextId, &final_sharpen_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_scale_coring_valid_range", "test (High0 > 255UL) fail case", id_cnt++);

        final_sharpen_scale_coring.low = 128;
        final_sharpen_scale_coring.low_delta = 7;
        Rval = ik_set_fnl_shp_scale_coring(pMode->ContextId, &final_sharpen_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_scale_coring_valid_range", "test (Low0 > fnlshpns_scl_cor->high) fail case", id_cnt++);

        Rval = ik_set_fnl_shp_coring(pMode->ContextId, &final_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_coring_valid_range", "test fractional_bits out of range fail case", id_cnt++);

        final_sharpen_both_three_d_table.y_tone_shift = 8;
        Rval = ik_set_fnl_shp_three_d_table(pMode->ContextId, &final_sharpen_both_three_d_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_fnlshpns_both_tdt_valid_range", "test y_tone_shift out of range fail case", id_cnt++);
    }

    {
        // DZoom
        ik_dzoom_info_t dzoom_info = {0};

        dzoom_info.enable = 1;
        dzoom_info.zoom_x = 32768;
        Rval = ik_set_dzoom_info(pMode->ContextId, &dzoom_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_dzoom_info_valid_range", "test (dzoom_info->zoom_x < 0x10000UL) && (dzoom_info->enable==1U) fail case", id_cnt++);

        dzoom_info.zoom_x = 131072;
        Rval = ik_set_dzoom_info(pMode->ContextId, &dzoom_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_dzoom_info_valid_range", "test (dzoom_info->zoom_x < 0x10000UL) false case", id_cnt++);

        dzoom_info.zoom_y = 32768;
        Rval = ik_set_dzoom_info(pMode->ContextId, &dzoom_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_dzoom_info_valid_range", "test (dzoom_info->zoom_y < 0x10000UL) && (dzoom_info->enable==1U) fail case", id_cnt++);

        dzoom_info.zoom_y = 131072;
        Rval = ik_set_dzoom_info(pMode->ContextId, &dzoom_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_dzoom_info_valid_range", "test (dzoom_info->zoom_y < 0x10000UL) false case", id_cnt++);
    }

    {
        // Flip mode
        uint32 mode = 0;

        mode = IK_FLIP_RAW_V;
        Rval = ik_set_flip_mode(pMode->ContextId, mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_flip_mode_info_valid_range", "test valid mode OK case", id_cnt++);

        mode = IK_FLIP_YUV_H;
        Rval = ik_set_flip_mode(pMode->ContextId, mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_flip_mode_info_valid_range", "test valid mode OK case", id_cnt++);

        mode = IK_FLIP_RAW_V_YUV_H;
        Rval = ik_set_flip_mode(pMode->ContextId, mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_flip_mode_info_valid_range", "test valid mode OK case", id_cnt++);
    }

    {
        // window size info
        ik_window_size_info_t window_size_info = {0};

        Rval = ik_set_window_size_info(pMode->ContextId, &window_size_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "win_size_info_valid_range_p1", "test invalid vin_sensor.width fail case", id_cnt++);

        window_size_info.vin_sensor.width = 1900;
        Rval = ik_set_window_size_info(pMode->ContextId, &window_size_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "win_size_info_valid_range_p1", "test invalid vin_sensor.width not 128-aligned fail case", id_cnt++);

        window_size_info.vin_sensor.width = 1900;
        Rval = ik_set_window_size_info(pModeY2y->ContextId, &window_size_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "win_size_info_valid_range_p1", "test invalid vin_sensor.width not 64-aligned fail case", id_cnt++);

        window_size_info.main_win.width = 1900;
        Rval = ik_set_window_size_info(pMode->ContextId, &window_size_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "win_size_info_valid_range_p1", "test invalid main.width not 64-aligned fail case", id_cnt++);

        window_size_info.main_win.width = 1920;
        window_size_info.prev[0].width = 3840;
        Rval = ik_set_window_size_info(pMode->ContextId, &window_size_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "win_size_info_valid_range_p2", "test invalid prev.width > main.width fail case", id_cnt++);

        window_size_info.thumbnail.width = 3840;
        Rval = ik_set_window_size_info(pMode->ContextId, &window_size_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "win_size_info_valid_range_p2", "test invalid thumbnail.width > main.width fail case", id_cnt++);

        window_size_info.screennail.width = 3840;
        Rval = ik_set_window_size_info(pMode->ContextId, &window_size_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "win_size_info_valid_range_p2", "test invalid screennail.width > main.width fail case", id_cnt++);
    }

    {
        // Active win
        ik_vin_active_window_t active_win = {0};

        active_win.enable = 1;
        active_win.active_geo.width = 0;
        active_win.active_geo.height = 0;
        Rval = ik_set_vin_active_win(pMode->ContextId, &active_win);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_vin_active_win_valid_range", "test active_win = 0 fail case", id_cnt++);

        active_win.active_geo.width = 1920;
        Rval = ik_set_vin_active_win(pMode->ContextId, &active_win);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_vin_active_win_valid_range", "test active_win.width != 0, but height = 0 fail case", id_cnt++);

        active_win.active_geo.height = 1080;
        Rval = ik_set_vin_active_win(pMode->ContextId, &active_win);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_vin_active_win_valid_range", "test active_win.width != 0, but height != 0 OK case", id_cnt++);

        active_win.active_geo.width = 1900;
        Rval = ik_set_vin_active_win(pMode->ContextId, &active_win);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_vin_active_win_valid_range", "test active_win.width not 128-align fail case", id_cnt++);

        active_win.active_geo.width = 1920;
        active_win.active_geo.height = 1075;
        Rval = ik_set_vin_active_win(pMode->ContextId, &active_win);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_vin_active_win_valid_range", "test active_win.height not 8-align fail case", id_cnt++);
    }

    {
        // Dummy
        ik_dummy_margin_range_t dmy_range = {0};
        ik_stitch_info_t stitch_info = {0};

        dmy_range.bottom = 131072;
        Rval = ik_set_dummy_margin_range(pMode->ContextId, &dmy_range);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_set_dummy_win_margin_range_info_valid_range", "test dmy_range->bottom > 0x10000UL fail case", id_cnt++);

        dmy_range.top = 131072;
        Rval = ik_set_dummy_margin_range(pMode->ContextId, &dmy_range);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_set_dummy_win_margin_range_info_valid_range", "test dmy_range->top > 0x10000UL fail case", id_cnt++);

        dmy_range.left = 131072;
        Rval = ik_set_dummy_margin_range(pMode->ContextId, &dmy_range);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_set_dummy_win_margin_range_info_valid_range", "test dmy_range->left > 0x10000UL fail case", id_cnt++);

        dmy_range.right = 131072;
        Rval = ik_set_dummy_margin_range(pMode->ContextId, &dmy_range);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_set_dummy_win_margin_range_info_valid_range", "test dmy_range->right > 0x10000UL fail case", id_cnt++);

        dmy_range.left = 32768;
        dmy_range.right = 131072;
        stitch_info.enable = 1;
        stitch_info.tile_num_x = 2;
        stitch_info.tile_num_y = 1;
        Rval = ik_set_stitching_info(pMode->ContextId, &stitch_info);
        Rval = ik_set_dummy_margin_range(pMode->ContextId, &dmy_range);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_set_dummy_win_margin_range_info_valid_range", "test left != right fail case", id_cnt++);
    }

    {
        // wide chroma filter combine
        ik_wide_chroma_filter_combine_t chroma_filter_combine = {0};

        chroma_filter_combine.T0_cb = 20;
        chroma_filter_combine.T1_cb = 10;
        Rval = ik_set_wide_chroma_filter_combine(pMode->ContextId, &chroma_filter_combine);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_wide_chroma_filter_combine", "test t0_cb > t1_cb fail case", id_cnt++);

        chroma_filter_combine.T0_cr = 20;
        chroma_filter_combine.T1_cr = 10;
        Rval = ik_set_wide_chroma_filter_combine(pMode->ContextId, &chroma_filter_combine);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_wide_chroma_filter_combine", "test t0_cr > t1_cr fail case", id_cnt++);
    }

    {
        // Stitch info
        ik_stitch_info_t stitch_info = {0};

        stitch_info.enable = 1;
        stitch_info.tile_num_x = 1;
        stitch_info.tile_num_y = 1;
        Rval = ik_set_stitching_info(pMode->ContextId, &stitch_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_stitching_info", "test if((p_stitch_info->tile_num_x==1)&&(p_stitch_info->tile_num_y==1)) on stitch enable fail case", id_cnt++);

        stitch_info.tile_num_y = 2;
        Rval = ik_set_stitching_info(pMode->ContextId, &stitch_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_stitching_info", "test (p_stitch_info->tile_num_y==1) false on stitch enable case", id_cnt++);

        stitch_info.tile_num_y = 12;
        Rval = ik_set_stitching_info(pMode->ContextId, &stitch_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_stitching_info", "test tile_num_y out of range NG case", id_cnt++);
    }

    {
        // Vig
        ik_vignette_t vignette = {0};

        vignette.calib_mode_enable = 1;

        Rval = ik_set_vignette(pMode->ContextId, &vignette);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_vignette_compensation", "test vin_sensor_geo.width == 0 fail case", id_cnt++);
    }

    {
        // Warp buffer info
        ik_warp_buffer_info_t warp_buf_info = {0};

        warp_buf_info.dram_efficiency = 3;
        Rval = ik_set_warp_buffer_info(pMode->ContextId, &warp_buf_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_warp_buffer_info", "test dram_efficiency out of range fail case", id_cnt++);
    }

    {
        // HDR blend
        ik_hdr_blend_t hdr_blend = {0};

        hdr_blend.flicker_threshold = 300;
        Rval = ik_set_hdr_blend(pMode->ContextId, &hdr_blend);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_hdr_blend", "test flicker_threshold out of range fail case", id_cnt++);
    }

    {
        // before CE
        ik_wb_gain_t before_ce_wb_gain = {0};

        Rval = ik_set_before_ce_wb_gain(pMode->ContextId, &before_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_wb_gain_valid_range", "test before_ce_wb_gain out of range fail case", id_cnt++);
    }

    {
        // after CE
        ik_wb_gain_t after_ce_wb_gain = {0};

        Rval = ik_set_after_ce_wb_gain(pMode->ContextId, &after_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_wb_gain_valid_range", "test after_ce_wb_gain out of range fail case", id_cnt++);
    }

    {
        // CFA leakage
        ik_cfa_leakage_filter_t cfa_leakage_filter = {0};

        cfa_leakage_filter.saturation_level = 16385;
        Rval = ik_set_cfa_leakage_filter(pMode->ContextId, &cfa_leakage_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_cfa_leakage_filter_valid_range", "test saturation_level out of range fail case", id_cnt++);
    }

    {
        // demosaic
        ik_demosaic_t demosaic = {0};

        demosaic.activity_thresh = 32;
        Rval = ik_set_demosaic(pMode->ContextId, &demosaic);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_demosaic_valid_range", "test activity_thresh out of range fail case", id_cnt++);
    }

    {
        // rgb to 12y
        ik_rgb_to_12y_t rgb_to_12y = {0};

        rgb_to_12y.y_offset = 32768;
        Rval = ik_set_rgb_to_12y(pMode->ContextId, &rgb_to_12y);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_rgb_to_12y_valid_range", "test y_offset out of range fail case", id_cnt++);
    }

    {
        // LNL
        ik_luma_noise_reduction_t luma_noise_reduce = {0};

        luma_noise_reduce.strength2_max_change = 256;
        Rval = ik_set_luma_noise_reduction(pMode->ContextId, &luma_noise_reduce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_luma_noise_reduction_valid_range", "test strength2_max_change out of range fail case", id_cnt++);
    }

    {
        // Pre CC gain
        ik_pre_cc_gain_t pre_cc_gain = {0};

        pre_cc_gain.enable = 3;
        Rval = ik_set_pre_cc_gain(pMode->ContextId, &pre_cc_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_pre_cc_gain_valid_range", "test enable out of range fail case", id_cnt++);
    }

    {
        // CC
        ik_color_correction_t color_correction = {0};

        color_correction.matrix_shift_minus_8 = 3;
        Rval = ik_set_color_correction(pMode->ContextId, &color_correction);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_color_correction_valid_range", "test matrix_shift_minus_8 out of range fail case", id_cnt++);
    }

    {
        // tone curve
        ik_tone_curve_t tone_curve = {0};

        tone_curve.tone_curve_red[0] = 1024;
        Rval = ik_set_tone_curve(pMode->ContextId, &tone_curve);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_tone_curve_valid_range", "test tone_curve_red out of range fail case", id_cnt++);
    }

    {
        // rgb 2 yuv
        ik_rgb_to_yuv_matrix_t rgb_to_yuv_matrix = {0};

        rgb_to_yuv_matrix.y_offset = 1024;
        Rval = ik_set_rgb_to_yuv_matrix(pMode->ContextId, &rgb_to_yuv_matrix);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_rgb_to_yuv_matrix_valid_range", "test y_offset out of range fail case", id_cnt++);
    }

    {
        // chroma scale
        ik_chroma_scale_t chroma_scale = {0};

        chroma_scale.gain_curve[0] = 4096;
        Rval = ik_set_chroma_scale(pMode->ContextId, &chroma_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_chroma_scale_valid_range", "test gain_curve out of range fail case", id_cnt++);
    }

    {
        // chroma median
        ik_chroma_median_filter_t chroma_median_filter = {0};

        chroma_median_filter.cb_adaptive_amount = 300;
        Rval = ik_set_chroma_median_filter(pMode->ContextId, &chroma_median_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_chroma_median_filter_valid_range", "test cb_adaptive_amount out of range fail case", id_cnt++);
    }

    {
        // luma processing mode
        ik_first_luma_process_mode_t first_luma_process_mode = {0};

        first_luma_process_mode.use_sharpen_not_asf = 2;
        Rval = ik_set_fst_luma_process_mode(pMode->ContextId, &first_luma_process_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_first_luma_processing_mode_valid_range", "test use_sharpen_not_asf out of range fail case", id_cnt++);
    }

    {
        // wide chroma filter
        ik_wide_chroma_filter_t wide_chroma_filter = {0};

        wide_chroma_filter.noise_level_cb = 256;
        Rval = ik_set_wide_chroma_filter(pMode->ContextId, &wide_chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_wide_chroma_filter_valid_range", "test noise_level_cb out of range fail case", id_cnt++);
    }

    {
        // grgb mismatch
        ik_grgb_mismatch_t grgb_mismatch = {0};

        grgb_mismatch.wide_safety = 300;
        Rval = ik_set_grgb_mismatch(pMode->ContextId, &grgb_mismatch);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_grgb_mismatch_valid_range", "test wide_safety out of range fail case", id_cnt++);
    }

    {
        // resampler strength
        ik_resampler_strength_t resample_str = {0};

        resample_str.cfa_cut_off_freq = 8;
        Rval = ik_set_resampler_strength(pMode->ContextId, &resample_str);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_resampler_str_valid_range", "test cfa_cut_off_freq out of range fail case", id_cnt++);
    }

    {
        // burst tile
        ik_burst_tile_t burst_tile = {0};

        burst_tile.enable = 2;
        Rval = ik_set_burst_tile(pMode->ContextId, &burst_tile);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextDebugCheck.c", "ctx_check_burst_tile", "test burst_tile.enable out of range fail case", id_cnt++);
    }

    ik_init_debug_check_func();
}

static void IK_TestCovr_ImgContextComponentSafetyFilter(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    p_ik_buffer_info->safety_state = IK_SAFETY_STATE_ERROR;

    {
        // hook safety filters
        AMBA_IK_EXECUTE_CONTAINER_s ExecuteContainer = {0};
        Rval = AmbaIK_ExecuteConfig(pMode, &ExecuteContainer);
    }

    // set
    {
        // Safety info
        ik_safety_info_t safety_info = {0};

        safety_info.update_freq = 1;
        Rval = ik_set_safety_info(pMode->ContextId, &safety_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_safety_info", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Sensor info
        ik_vin_sensor_info_t sensor_info = {0};

        Rval = ik_set_vin_sensor_info(pMode->ContextId, &sensor_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_vin_sensor_info", "test set SafetyFilters case", id_cnt++);
    }

    {
        // YUV mode
        uint32 yuv_mode = 0;

        Rval = ik_set_yuv_mode(pMode->ContextId, yuv_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_yuv_mode", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Before CE
        ik_wb_gain_t before_ce_wb_gain = {0};

        before_ce_wb_gain.gain_b = 4096;
        before_ce_wb_gain.gain_g = 4096;
        before_ce_wb_gain.gain_r = 4096;
        Rval = ik_set_before_ce_wb_gain(pMode->ContextId, &before_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_before_ce_wb_gain", "test set SafetyFilters case", id_cnt++);
    }

    {
        // After CE
        ik_wb_gain_t after_ce_wb_gain = {0};

        after_ce_wb_gain.gain_b = 4096;
        after_ce_wb_gain.gain_g = 4096;
        after_ce_wb_gain.gain_r = 4096;
        Rval = ik_set_after_ce_wb_gain(pMode->ContextId, &after_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_after_ce_wb_gain", "test set SafetyFilters case", id_cnt++);
    }

    {
        // CFA leakage
        ik_cfa_leakage_filter_t cfa_leakage_filter = {0};

        Rval = ik_set_cfa_leakage_filter(pMode->ContextId, &cfa_leakage_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_cfa_leakage_filter", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Anti aliasing
        ik_anti_aliasing_t anti_aliasing = {0};

        Rval = ik_set_anti_aliasing(pMode->ContextId, &anti_aliasing);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_anti_aliasing", "test set SafetyFilters case", id_cnt++);
    }

    {
        // DBP
        ik_dynamic_bad_pixel_correction_t dynamic_bpc = {0};

        Rval = ik_set_dynamic_bad_pixel_corr(pMode->ContextId, &dynamic_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_dynamic_bpc", "test set SafetyFilters case", id_cnt++);
    }

    {
        // SBP
        uint32 enb = 0;
        ik_static_bad_pxl_cor_t static_bpc = {0};

        Rval = ik_set_static_bad_pxl_corr_enb(pMode->ContextId, enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_static_bpc_enable_info", "test set SafetyFilters case", id_cnt++);

        static_bpc.calib_sbp_info.version = 0x20180401;
        static_bpc.calib_sbp_info.sbp_buffer = FPNMap;
        static_bpc.vin_sensor_geo.width = 1920;
        static_bpc.vin_sensor_geo.height = 1080;
        static_bpc.vin_sensor_geo.h_sub_sample.factor_den = 1;
        static_bpc.vin_sensor_geo.h_sub_sample.factor_num = 1;
        static_bpc.vin_sensor_geo.v_sub_sample.factor_den = 1;
        static_bpc.vin_sensor_geo.v_sub_sample.factor_num = 1;
        Rval = ik_set_static_bad_pxl_corr(pMode->ContextId, &static_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_static_bpc", "test set SafetyFilters case", id_cnt++);
    }

    {
        // SBP internal
        ik_static_bad_pixel_correction_internal_t static_bpc_internal = {0};

        static_bpc_internal.p_map = FPNMap;
        Rval = ik_set_static_bad_pxl_corr_itnl(pMode->ContextId, &static_bpc_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_static_bpc_internal", "test set SafetyFilters case", id_cnt++);
    }

    {
        // CFA noise
        ik_cfa_noise_filter_t cfa_noise_filter = {0};

        Rval = ik_set_cfa_noise_filter(pMode->ContextId, &cfa_noise_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_cfa_noise_filter", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Demosaic
        ik_demosaic_t demosaic = {0};

        Rval = ik_set_demosaic(pMode->ContextId, &demosaic);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_demosaic", "test set SafetyFilters case", id_cnt++);
    }

    {
        // RGB_12Y
        ik_rgb_to_12y_t rgb_to_12y = {0};

        Rval = ik_set_rgb_to_12y(pMode->ContextId, &rgb_to_12y);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_rgb_to_12y", "test set SafetyFilters case", id_cnt++);
    }

    {
        // LNL
        ik_luma_noise_reduction_t luma_noise_reduce = {0};

        Rval = ik_set_luma_noise_reduction(pMode->ContextId, &luma_noise_reduce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_luma_noise_reduction", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Pre CC gain
        ik_pre_cc_gain_t pre_cc_gain = {0};

        Rval = ik_set_pre_cc_gain(pMode->ContextId, &pre_cc_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_pre_cc_gain", "test set SafetyFilters case", id_cnt++);
    }

    {
        // CC
        ik_color_correction_t color_correction = {0};

        Rval = ik_set_color_correction(pMode->ContextId, &color_correction);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_color_correction", "test set SafetyFilters case", id_cnt++);
    }

    {
        // tone curve
        ik_tone_curve_t tone_curve = {0};

        Rval = ik_set_tone_curve(pMode->ContextId, &tone_curve);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_tone_curve", "test set SafetyFilters case", id_cnt++);
    }

    {
        // RGB to YUV
        ik_rgb_to_yuv_matrix_t rgb_to_yuv_matrix = {0};

        Rval = ik_set_rgb_to_yuv_matrix(pMode->ContextId, &rgb_to_yuv_matrix);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_rgb_to_yuv_matrix", "test set SafetyFilters case", id_cnt++);
    }

    {
        // RGBIR
        ik_rgb_ir_t rgb_ir = {0};

        Rval = ik_set_rgb_ir(pMode->ContextId, &rgb_ir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_rgb_ir", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Chroma scale
        ik_chroma_scale_t chroma_scale = {0};

        Rval = ik_set_chroma_scale(pMode->ContextId, &chroma_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_chroma_scale", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Chroma median
        ik_chroma_median_filter_t chroma_median_filter = {0};

        Rval = ik_set_chroma_median_filter(pMode->ContextId, &chroma_median_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_chroma_median_filter", "test set SafetyFilters case", id_cnt++);
    }

    {
        // SHPA proc mode
        ik_first_luma_process_mode_t first_luma_process_mode = {0};

        Rval = ik_set_fst_luma_process_mode(pMode->ContextId, &first_luma_process_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_first_luma_processing_mode", "test set SafetyFilters case", id_cnt++);
    }

    {
        // ASF
        ik_adv_spatial_filter_t advance_spatial_filter = {0};

        Rval = ik_set_adv_spatial_filter(pMode->ContextId, &advance_spatial_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_advance_spatial_filter", "test set SafetyFilters case", id_cnt++);
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

        Rval = ik_set_fst_shp_both(pMode->ContextId, &first_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fstshpns_both", "test set SafetyFilters case", id_cnt++);

        first_sharpen_noise.level_str_adjust.high = 100;
        Rval = ik_set_fst_shp_noise(pMode->ContextId, &first_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fstshpns_noise", "test set SafetyFilters case", id_cnt++);

        Rval = ik_set_fst_shp_fir(pMode->ContextId, &first_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fstshpns_fir", "test set SafetyFilters case", id_cnt++);

        first_sharpen_coring.fractional_bits = 2;
        Rval = ik_set_fst_shp_coring(pMode->ContextId, &first_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fstshp_coring", "test set SafetyFilters case", id_cnt++);

        first_sharpen_coring_idx_scale.high = 100;
        Rval = ik_set_fst_shp_coring_idx_scale(pMode->ContextId, &first_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fstshp_cor_idx_scl", "test set SafetyFilters case", id_cnt++);

        first_sharpen_min_coring_result.high = 100;
        Rval = ik_set_fst_shp_min_coring_rslt(pMode->ContextId, &first_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fstshpns_min_coring_result", "test set SafetyFilters case", id_cnt++);

        first_sharpen_max_coring_result.high = 100;
        Rval = ik_set_fst_shp_max_coring_rslt(pMode->ContextId, &first_sharpen_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fstshpns_max_coring_result", "test set SafetyFilters case", id_cnt++);

        first_sharpen_scale_coring.high = 100;
        Rval = ik_set_fst_shp_scale_coring(pMode->ContextId, &first_sharpen_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fstshpns_scale_coring", "test set SafetyFilters case", id_cnt++);
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

        Rval = ik_set_fnl_shp_both(pMode->ContextId, &final_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fnlshp_both", "test set SafetyFilters case", id_cnt++);

        final_sharpen_noise.level_str_adjust.high = 100;
        Rval = ik_set_fnl_shp_noise(pMode->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fnlshpns_noise", "test set SafetyFilters case", id_cnt++);

        Rval = ik_set_fnl_shp_fir(pMode->ContextId, &final_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fnlshpns_fir", "test set SafetyFilters case", id_cnt++);

        final_sharpen_coring.fractional_bits = 2;
        Rval = ik_set_fnl_shp_coring(pMode->ContextId, &final_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fnlshp_coring", "test set SafetyFilters case", id_cnt++);

        final_sharpen_coring_idx_scale.high = 100;
        Rval = ik_set_fnl_shp_coring_idx_scale(pMode->ContextId, &final_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fnlshp_cor_idx_scl", "test set SafetyFilters case", id_cnt++);

        final_sharpen_min_coring_result.high = 100;
        Rval = ik_set_fnl_shp_min_coring_rslt(pMode->ContextId, &final_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fnlshpns_min_coring_result", "test set SafetyFilters case", id_cnt++);

        final_sharpen_max_coring_result.high = 100;
        Rval = ik_set_fnl_shp_max_coring_rslt(pMode->ContextId, &final_sharpen_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fnlshpns_max_coring_result", "test set SafetyFilters case", id_cnt++);

        final_sharpen_scale_coring.high = 100;
        Rval = ik_set_fnl_shp_scale_coring(pMode->ContextId, &final_sharpen_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fnlshpns_scale_coring", "test set SafetyFilters case", id_cnt++);

        Rval = ik_set_fnl_shp_three_d_table(pMode->ContextId, &final_sharpen_both_three_d_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fnlshp_both_tdt", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Chroma filter
        ik_chroma_filter_t chroma_filter = {0};

        chroma_filter.radius = 32;
        Rval = ik_set_chroma_filter(pMode->ContextId, &chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_chroma_filter", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Wide chroma filter
        ik_wide_chroma_filter_t wide_chroma_filter = {0};

        Rval = ik_set_wide_chroma_filter(pMode->ContextId, &wide_chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_w_chroma_ft", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Wide chroma filter combine
        ik_wide_chroma_filter_combine_t chroma_filter_combine = {0};

        chroma_filter_combine.T1_cb = 10;
        chroma_filter_combine.T1_cr = 10;
        Rval = ik_set_wide_chroma_filter_combine(pMode->ContextId, &chroma_filter_combine);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_w_chroma_ft_combine", "test set SafetyFilters case", id_cnt++);
    }

    {
        // grgb mismatch
        ik_grgb_mismatch_t grgb_mismatch = {0};

        Rval = ik_set_grgb_mismatch(pMode->ContextId, &grgb_mismatch);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_grgb_mismatch", "test set SafetyFilters case", id_cnt++);
    }

    {
        // MCTF
        ik_video_mctf_t video_mctf = {0};
        ik_video_mctf_ta_t video_mctf_ta = {0};
        ik_pos_dep33_t video_mctf_and_final_sharpen = {0};

        Rval = ik_set_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_video_mctf", "test set SafetyFilters case", id_cnt++);

        Rval = ik_set_video_mctf_ta(pMode->ContextId, &video_mctf_ta);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_mctf_ta", "test set SafetyFilters case", id_cnt++);

        Rval = ik_set_video_mctf_and_fnl_shp(pMode->ContextId, &video_mctf_and_final_sharpen);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_mctf_and_final_sharpen", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Vig
        uint32 enb = 0;
        ik_vignette_t vignette = {0};

        Rval = ik_set_vignette_enb(pMode->ContextId, enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_vignette_enable_info", "test set SafetyFilters case", id_cnt++);
        Rval = ik_set_vignette(pMode->ContextId, &vignette);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_vignette", "test set SafetyFilters case", id_cnt++);
    }

    {
        // HDR blc
        ik_static_blc_level_t frontend_static_blc = {0};

        Rval = ik_set_exp0_frontend_static_blc(pMode->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_exp0_fe_static_blc", "test set SafetyFilters case", id_cnt++);
        Rval = ik_set_exp1_frontend_static_blc(pMode->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_exp1_fe_static_blc", "test set SafetyFilters case", id_cnt++);
        Rval = ik_set_exp2_frontend_static_blc(pMode->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_exp2_fe_static_blc", "test set SafetyFilters case", id_cnt++);
    }

    {
        // HDR gain
        ik_frontend_wb_gain_t frontend_wb_gain = {0};

        frontend_wb_gain.shutter_ratio = 4096;
        Rval = ik_set_exp0_frontend_wb_gain(pMode->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_exp0_fe_wb_gain", "test set SafetyFilters case", id_cnt++);
        Rval = ik_set_exp1_frontend_wb_gain(pMode->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_exp1_fe_wb_gain", "test set SafetyFilters case", id_cnt++);
        Rval = ik_set_exp2_frontend_wb_gain(pMode->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_exp2_fe_wb_gain", "test set SafetyFilters case", id_cnt++);
    }

    {
        // CE
        ik_ce_t ce = {0};
        ik_ce_input_table_t ce_input_table = {0};
        ik_ce_output_table_t ce_out_table = {0};

        Rval = ik_set_ce(pMode->ContextId, &ce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_ce", "test set SafetyFilters case", id_cnt++);
        Rval = ik_set_ce_input_table(pMode->ContextId, &ce_input_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_ce_input_table", "test set SafetyFilters case", id_cnt++);
        Rval = ik_set_ce_out_table(pMode->ContextId, &ce_out_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_ce_out_table", "test set SafetyFilters case", id_cnt++);
    }

    {
        // HDR blend
        ik_hdr_blend_t hdr_blend = {0};

        Rval = ik_set_hdr_blend(pMode->ContextId, &hdr_blend);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_hdr_blend", "test set SafetyFilters case", id_cnt++);
    }

    {
        // HDR tone curve
        ik_frontend_tone_curve_t fe_tc = {0};

        Rval = ik_set_frontend_tone_curve(pMode->ContextId, &fe_tc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_fe_tone_curve", "test set SafetyFilters case", id_cnt++);
    }

    {
        // resampler strength
        ik_resampler_strength_t resampler_strength = {0};

        Rval = ik_set_resampler_strength(pMode->ContextId, &resampler_strength);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_resampler_str", "test set SafetyFilters case", id_cnt++);
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
        Rval = ik_set_aaa_stat_info(pMode->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_aaa_stat_info", "test set SafetyFilters case", id_cnt++);
        Rval = ik_set_aaa_pg_af_stat_info(pMode->ContextId, &pg_af_stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_aaa_pg_af_info", "test set SafetyFilters case", id_cnt++);
        Rval = ik_set_af_stat_ex_info(pMode->ContextId, &af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_af_stat_ex_info", "test set SafetyFilters case", id_cnt++);
        Rval = ik_set_pg_af_stat_ex_info(pMode->ContextId, &pg_af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_pg_af_stat_ex_info", "test set SafetyFilters case", id_cnt++);
        Rval = ik_set_histogram_info(pMode->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_histogram_info", "test set SafetyFilters case", id_cnt++);
        Rval = ik_set_pg_histogram_info(pMode->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_pg_histogram_info", "test set SafetyFilters case", id_cnt++);
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
        Rval = ik_set_window_size_info(pMode->ContextId, &window_size_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_window_size_info", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Warp
        uint32 enb = 0;
        ik_warp_info_t calib_warp_info = {0};

        Rval = ik_set_warp_enb(pMode->ContextId, enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_warp_enable_info", "test set SafetyFilters case", id_cnt++);

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
        Rval = ik_set_warp_info(pMode->ContextId, &calib_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_calib_warp_info", "test set SafetyFilters case", id_cnt++);
    }

    {
        // CA
        uint32 enb = 0;
        ik_cawarp_info_t calib_ca_warp_info = {0};

        Rval = ik_set_cawarp_enb(pMode->ContextId, enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_cawarp_enable_info", "test set SafetyFilters case", id_cnt++);

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
        Rval = ik_set_cawarp_info(pMode->ContextId, &calib_ca_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_calib_ca_warp_info", "test set SafetyFilters case", id_cnt++);
    }

    {
        // DZoom
        ik_dzoom_info_t dzoom_info = {0};

        Rval = ik_set_dzoom_info(pMode->ContextId, &dzoom_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_dzoom_info", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Dummy
        ik_dummy_margin_range_t dmy_margin_range = {0};

        Rval = ik_set_dummy_margin_range(pMode->ContextId, &dmy_margin_range);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_dummy_win_margin_range_info", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Active win
        ik_vin_active_window_t vin_active_win = {0};

        Rval = ik_set_vin_active_win(pMode->ContextId, &vin_active_win);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_vin_active_window", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Warp internal
        ik_warp_internal_info_t warp_dzoom_internal = {0};

        warp_dzoom_internal.pwarp_horizontal_table = (int16 *)WarpGrid;
        warp_dzoom_internal.pwarp_vertical_table = (int16 *)(WarpGrid + (128*96*4));
        Rval = ik_set_warp_internal(pMode->ContextId, &warp_dzoom_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_warp_internal", "test set SafetyFilters case", id_cnt++);
    }

    {
        // CA internal
        ik_cawarp_internal_info_t cawarp_internal = {0};

        Rval = ik_set_cawarp_internal(pMode->ContextId, &cawarp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_cawarp_internal", "test set SafetyFilters case", id_cnt++);
    }

    {
        // HDR RAW info
        ik_hdr_raw_info_t hdr_raw_info = {0};

        Rval = ik_set_hdr_raw_offset(pMode->ContextId, &hdr_raw_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_hdr_raw_offset", "test set SafetyFilters case", id_cnt++);
    }

    {
        // flip mode
        uint32 mode = 0;

        Rval = ik_set_flip_mode(pMode->ContextId, mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_flip_mode", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Warp buf info
        ik_warp_buffer_info_t warp_buf_info = {0};

        warp_buf_info.luma_wait_lines = 8;
        warp_buf_info.luma_dma_size = 32;
        Rval = ik_set_warp_buffer_info(pMode->ContextId, &warp_buf_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_warp_buffer_info", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Stitch info
        ik_stitch_info_t stitch_info = {0};

        Rval = ik_set_stitching_info(pMode->ContextId, &stitch_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_stitching_info", "test set SafetyFilters case", id_cnt++);
    }

    {
        // Burst tile
        ik_burst_tile_t burst_tile = {0};

        Rval = ik_set_burst_tile(pMode->ContextId, &burst_tile);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_set_burst_tile", "test set SafetyFilters case", id_cnt++);
    }

    // get
    {
        // Safety info
        ik_safety_info_t safety_info = {0};

        Rval = ik_get_safety_info(pMode->ContextId, &safety_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_safety_info", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Sensor info
        ik_vin_sensor_info_t sensor_info = {0};

        Rval = ik_get_vin_sensor_info(pMode->ContextId, &sensor_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_vin_sensor_info", "test get SafetyFilters case", id_cnt++);
    }

    {
        // YUV mode
        uint32 yuv_mode = 0;

        Rval = ik_get_yuv_mode(pMode->ContextId, &yuv_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_yuv_mode", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Before CE
        ik_wb_gain_t before_ce_wb_gain = {0};

        Rval = ik_get_before_ce_wb_gain(pMode->ContextId, &before_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_before_ce_wb_gain", "test get SafetyFilters case", id_cnt++);
    }

    {
        // After CE
        ik_wb_gain_t after_ce_wb_gain = {0};

        Rval = ik_get_after_ce_wb_gain(pMode->ContextId, &after_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_after_ce_wb_gain", "test get SafetyFilters case", id_cnt++);
    }

    {
        // CFA leakage
        ik_cfa_leakage_filter_t cfa_leakage_filter = {0};

        Rval = ik_get_cfa_leakage_filter(pMode->ContextId, &cfa_leakage_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_cfa_leakage_filter", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Anti aliasing
        ik_anti_aliasing_t anti_aliasing = {0};

        Rval = ik_get_anti_aliasing(pMode->ContextId, &anti_aliasing);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_anti_aliasing", "test get SafetyFilters case", id_cnt++);
    }

    {
        // DBP
        ik_dynamic_bad_pixel_correction_t dynamic_bpc = {0};

        Rval = ik_get_dynamic_bad_pixel_corr(pMode->ContextId, &dynamic_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_dynamic_bpc", "test get SafetyFilters case", id_cnt++);
    }

    {
        // SBP
        uint32 enb = 0;
        ik_static_bad_pxl_cor_t static_bpc = {0};

        Rval = ik_get_static_bad_pxl_corr_enb(pMode->ContextId, &enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_static_bpc_enable_info", "test get SafetyFilters case", id_cnt++);
        Rval = ik_get_static_bad_pxl_corr(pMode->ContextId, &static_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_static_bpc", "test get SafetyFilters case", id_cnt++);
    }

    {
        // SBP internal
        ik_static_bad_pixel_correction_internal_t static_bpc_internal = {0};

        Rval = ik_get_static_bad_pxl_corr_itnl(pMode->ContextId, &static_bpc_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_static_bpc_internal", "test get SafetyFilters case", id_cnt++);
    }

    {
        // CFA noise
        ik_cfa_noise_filter_t cfa_noise_filter = {0};

        Rval = ik_get_cfa_noise_filter(pMode->ContextId, &cfa_noise_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_cfa_noise_filter", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Demosaic
        ik_demosaic_t demosaic = {0};

        Rval = ik_get_demosaic(pMode->ContextId, &demosaic);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_demosaic", "test get SafetyFilters case", id_cnt++);
    }

    {
        // RGB_12Y
        ik_rgb_to_12y_t rgb_to_12y = {0};

        Rval = ik_get_rgb_to_12y(pMode->ContextId, &rgb_to_12y);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_rgb_to_12y", "test get SafetyFilters case", id_cnt++);
    }

    {
        // LNL
        ik_luma_noise_reduction_t luma_noise_reduce = {0};

        Rval = ik_get_luma_noise_reduction(pMode->ContextId, &luma_noise_reduce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_luma_noise_reduction", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Pre CC gain
        ik_pre_cc_gain_t pre_cc_gain = {0};

        Rval = ik_get_pre_cc_gain(pMode->ContextId, &pre_cc_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_pre_cc_gain", "test get SafetyFilters case", id_cnt++);
    }

    {
        // CC
        ik_color_correction_t color_correction = {0};

        Rval = ik_get_color_correction(pMode->ContextId, &color_correction);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_color_correction", "test get SafetyFilters case", id_cnt++);
    }

    {
        // tone curve
        ik_tone_curve_t tone_curve = {0};

        Rval = ik_get_tone_curve(pMode->ContextId, &tone_curve);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_tone_curve", "test get SafetyFilters case", id_cnt++);
    }

    {
        // RGB to YUV
        ik_rgb_to_yuv_matrix_t rgb_to_yuv_matrix = {0};

        Rval = ik_get_rgb_to_yuv_matrix(pMode->ContextId, &rgb_to_yuv_matrix);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_rgb_to_yuv_matrix", "test get SafetyFilters case", id_cnt++);
    }

    {
        // RGBIR
        ik_rgb_ir_t rgb_ir = {0};

        Rval = ik_get_rgb_ir(pMode->ContextId, &rgb_ir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_rgb_ir", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Chroma scale
        ik_chroma_scale_t chroma_scale = {0};

        Rval = ik_get_chroma_scale(pMode->ContextId, &chroma_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_chroma_scale", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Chroma median
        ik_chroma_median_filter_t chroma_median_filter = {0};

        Rval = ik_get_chroma_median_filter(pMode->ContextId, &chroma_median_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_chroma_median_filter", "test get SafetyFilters case", id_cnt++);
    }

    {
        // SHPA proc mode
        ik_first_luma_process_mode_t first_luma_process_mode = {0};

        Rval = ik_get_fst_luma_process_mode(pMode->ContextId, &first_luma_process_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_first_luma_processing_mode", "test get SafetyFilters case", id_cnt++);
    }

    {
        // ASF
        ik_adv_spatial_filter_t advance_spatial_filter = {0};

        Rval = ik_get_adv_spatial_filter(pMode->ContextId, &advance_spatial_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_advance_spatial_filter", "test get SafetyFilters case", id_cnt++);
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

        Rval = ik_get_fst_shp_both(pMode->ContextId, &first_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fstshpns_both", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_fst_shp_noise(pMode->ContextId, &first_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fstshpns_noise", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_fst_shp_fir(pMode->ContextId, &first_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fstshpns_fir", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_fst_shp_coring(pMode->ContextId, &first_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fstshp_coring", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_fst_shp_coring_idx_scale(pMode->ContextId, &first_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fstshp_cor_idx_scl", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_fst_shp_min_coring_rslt(pMode->ContextId, &first_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fstshpns_min_coring_result", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_fst_shp_max_coring_rslt(pMode->ContextId, &first_sharpen_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fstshpns_max_coring_result", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_fst_shp_scale_coring(pMode->ContextId, &first_sharpen_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fstshpns_scale_coring", "test get SafetyFilters case", id_cnt++);
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

        Rval = ik_get_fnl_shp_both(pMode->ContextId, &final_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshp_both", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_fnl_shp_noise(pMode->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshpns_noise", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_fnl_shp_fir(pMode->ContextId, &final_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshpns_fir", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_fnl_shp_coring(pMode->ContextId, &final_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshp_coring", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_fnl_shp_coring_idx_scale(pMode->ContextId, &final_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshp_cor_idx_scl", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_fnl_shp_min_coring_rslt(pMode->ContextId, &final_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshpns_min_coring_result", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_fnl_shp_max_coring_rslt(pMode->ContextId, &final_sharpen_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshpns_max_coring_result", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_fnl_shp_scale_coring(pMode->ContextId, &final_sharpen_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshpns_scale_coring", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_fnl_shp_three_d_table(pMode->ContextId, &final_sharpen_both_three_d_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshp_both_tdt", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Chroma filter
        ik_chroma_filter_t chroma_filter = {0};

        Rval = ik_get_chroma_filter(pMode->ContextId, &chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_chroma_filter", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Wide chroma filter
        ik_wide_chroma_filter_t wide_chroma_filter = {0};

        Rval = ik_get_wide_chroma_filter(pMode->ContextId, &wide_chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_w_chroma_ft", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Wide chroma filter combine
        ik_wide_chroma_filter_combine_t chroma_filter_combine = {0};

        Rval = ik_get_wide_chroma_filter_combine(pMode->ContextId, &chroma_filter_combine);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_w_chroma_ft_combine", "test get SafetyFilters case", id_cnt++);
    }

    {
        // grgb mismatch
        ik_grgb_mismatch_t grgb_mismatch = {0};

        Rval = ik_get_grgb_mismatch(pMode->ContextId, &grgb_mismatch);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_grgb_mismatch", "test get SafetyFilters case", id_cnt++);
    }

    {
        // MCTF
        ik_video_mctf_t video_mctf = {0};
        ik_video_mctf_ta_t video_mctf_ta = {0};
        ik_pos_dep33_t video_mctf_and_final_sharpen = {0};

        Rval = ik_get_video_mctf(pMode->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_video_mctf", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_video_mctf_ta(pMode->ContextId, &video_mctf_ta);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_mctf_ta", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_video_mctf_and_fnl_shp(pMode->ContextId, &video_mctf_and_final_sharpen);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_mctf_and_final_sharpen", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Vig
        uint32 enb = 0;
        ik_vignette_t vignette = {0};

        Rval = ik_get_vignette_enb(pMode->ContextId, &enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_vignette_enable_info", "test get SafetyFilters case", id_cnt++);
        Rval = ik_get_vignette(pMode->ContextId, &vignette);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_vignette", "test get SafetyFilters case", id_cnt++);
    }

    {
        // HDR blc
        ik_static_blc_level_t frontend_static_blc = {0};

        Rval = ik_get_exp0_frontend_static_blc(pMode->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_exp0_fe_static_blc", "test get SafetyFilters case", id_cnt++);
        Rval = ik_get_exp1_frontend_static_blc(pMode->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_exp1_fe_static_blc", "test get SafetyFilters case", id_cnt++);
        Rval = ik_get_exp2_frontend_static_blc(pMode->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_exp2_fe_static_blc", "test get SafetyFilters case", id_cnt++);
    }

    {
        // HDR gain
        ik_frontend_wb_gain_t frontend_wb_gain = {0};

        Rval = ik_get_exp0_frontend_wb_gain(pMode->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_exp0_fe_wb_gain", "test get SafetyFilters case", id_cnt++);
        Rval = ik_get_exp1_frontend_wb_gain(pMode->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_exp1_fe_wb_gain", "test get SafetyFilters case", id_cnt++);
        Rval = ik_get_exp2_frontend_wb_gain(pMode->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_exp2_fe_wb_gain", "test get SafetyFilters case", id_cnt++);
    }

    {
        // CE
        ik_ce_t ce = {0};
        ik_ce_input_table_t ce_input_table = {0};
        ik_ce_output_table_t ce_out_table = {0};

        Rval = ik_get_ce(pMode->ContextId, &ce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_ce", "test get SafetyFilters case", id_cnt++);
        Rval = ik_get_ce_input_table(pMode->ContextId, &ce_input_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_ce_input_table", "test get SafetyFilters case", id_cnt++);
        Rval = ik_get_ce_out_table(pMode->ContextId, &ce_out_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_ce_out_table", "test get SafetyFilters case", id_cnt++);
    }

    {
        // HDR blend
        ik_hdr_blend_t hdr_blend = {0};

        Rval = ik_get_hdr_blend(pMode->ContextId, &hdr_blend);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_hdr_blend", "test get SafetyFilters case", id_cnt++);
    }

    {
        // HDR tone curve
        ik_frontend_tone_curve_t fe_tc = {0};

        Rval = ik_get_frontend_tone_curve(pMode->ContextId, &fe_tc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fe_tone_curve", "test get SafetyFilters case", id_cnt++);
    }

    {
        // resampler strength
        ik_resampler_strength_t resampler_strength = {0};

        Rval = ik_get_resampler_strength(pMode->ContextId, &resampler_strength);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_resampler_str", "test get SafetyFilters case", id_cnt++);
    }

    {
        // AAA
        ik_aaa_stat_info_t stat_info = {0};
        ik_aaa_pg_af_stat_info_t pg_af_stat_info = {0};
        ik_af_stat_ex_info_t af_stat_ex_info = {0};
        ik_pg_af_stat_ex_info_t pg_af_stat_ex_info = {0};
        ik_histogram_info_t hist_info = {0};

        Rval = ik_get_aaa_stat_info(pMode->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_aaa_stat_info", "test get SafetyFilters case", id_cnt++);
        Rval = ik_get_aaa_pg_af_stat_info(pMode->ContextId, &pg_af_stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_aaa_pg_af_info", "test get SafetyFilters case", id_cnt++);
        Rval = ik_get_af_stat_ex_info(pMode->ContextId, &af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_af_stat_ex_info", "test get SafetyFilters case", id_cnt++);
        Rval = ik_get_pg_af_stat_ex_info(pMode->ContextId, &pg_af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_pg_af_stat_ex_info", "test get SafetyFilters case", id_cnt++);
        Rval = ik_get_histogram_info(pMode->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_histogram_info", "test get SafetyFilters case", id_cnt++);
        Rval = ik_get_pg_histogram_info(pMode->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_pg_histogram_info", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Window info
        ik_window_size_info_t window_size_info = {0};

        Rval = ik_get_window_size_info(pMode->ContextId, &window_size_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_window_size_info", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Warp
        uint32 enb = 0;
        ik_warp_info_t calib_warp_info = {0};

        Rval = ik_get_warp_enb(pMode->ContextId, &enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_warp_enable_info", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_warp_info(pMode->ContextId, &calib_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_calib_warp_info", "test get SafetyFilters case", id_cnt++);
    }

    {
        // CA
        uint32 enb = 0;
        ik_cawarp_info_t calib_ca_warp_info = {0};

        Rval = ik_get_cawarp_enb(pMode->ContextId, &enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_cawarp_enable_info", "test get SafetyFilters case", id_cnt++);

        Rval = ik_get_cawarp_info(pMode->ContextId, &calib_ca_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_calib_ca_warp_info", "test get SafetyFilters case", id_cnt++);
    }

    {
        // DZoom
        ik_dzoom_info_t dzoom_info = {0};

        Rval = ik_get_dzoom_info(pMode->ContextId, &dzoom_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_dzoom_info", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Dummy
        ik_dummy_margin_range_t dmy_margin_range = {0};

        Rval = ik_get_dummy_margin_range(pMode->ContextId, &dmy_margin_range);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_dummy_win_margin_range_info", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Active win
        ik_vin_active_window_t vin_active_win = {0};

        Rval = ik_get_vin_active_win(pMode->ContextId, &vin_active_win);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_vin_active_window", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Warp internal
        ik_warp_internal_info_t warp_dzoom_internal = {0};

        Rval = ik_get_warp_internal(pMode->ContextId, &warp_dzoom_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_warp_internal", "test get SafetyFilters case", id_cnt++);
    }

    {
        // CA internal
        ik_cawarp_internal_info_t cawarp_internal = {0};

        Rval = ik_get_cawarp_internal(pMode->ContextId, &cawarp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_cawarp_internal", "test get SafetyFilters case", id_cnt++);
    }

    {
        // HDR RAW info
        ik_hdr_raw_info_t hdr_raw_info = {0};

        Rval = ik_get_hdr_raw_offset(pMode->ContextId, &hdr_raw_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_hdr_raw_offset", "test get SafetyFilters case", id_cnt++);
    }

    {
        // flip mode
        uint32 mode = 0;

        Rval = ik_get_flip_mode(pMode->ContextId, &mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_flip_mode", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Warp buf info
        ik_warp_buffer_info_t warp_buf_info = {0};

        Rval = ik_get_warp_buffer_info(pMode->ContextId, &warp_buf_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_warp_buffer_info", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Stitch info
        ik_stitch_info_t stitch_info = {0};

        Rval = ik_get_stitching_info(pMode->ContextId, &stitch_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_stitching_info", "test get SafetyFilters case", id_cnt++);
    }

    {
        // Burst tile
        ik_burst_tile_t burst_tile = {0};

        Rval = ik_get_burst_tile(pMode->ContextId, &burst_tile);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_burst_tile", "test get SafetyFilters case", id_cnt++);
    }

    // get null ptr
    {
        // Safety info
        Rval = img_ctx_safety_get_safety_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_safety_info", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Sensor info
        Rval = img_ctx_safety_get_vin_sensor_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_vin_sensor_info", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // YUV mode
        Rval = img_ctx_safety_get_yuv_mode(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_yuv_mode", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Before CE
        Rval = img_ctx_safety_get_before_ce_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_before_ce_wb_gain", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // After CE
        Rval = img_ctx_safety_get_after_ce_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_after_ce_wb_gain", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // CFA leakage
        Rval = img_ctx_safety_get_cfa_leakage_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_cfa_leakage_filter", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Anti aliasing
        Rval = img_ctx_safety_get_anti_aliasing(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_anti_aliasing", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // DBP
        Rval = img_ctx_safety_get_dynamic_bpc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_dynamic_bpc", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // SBP
        Rval = img_ctx_safety_get_static_bpc_enable_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_static_bpc_enable_info", "test get SafetyFilters null ptr case", id_cnt++);
        Rval = img_ctx_safety_get_static_bpc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_static_bpc", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // SBP internal
        Rval = img_ctx_safety_get_static_bpc_internal(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_static_bpc_internal", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // CFA noise
        Rval = img_ctx_safety_get_cfa_noise_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_cfa_noise_filter", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Demosaic
        Rval = img_ctx_safety_get_demosaic(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_demosaic", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // RGB_12Y
        Rval = img_ctx_safety_get_rgb_to_12y(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_rgb_to_12y", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // LNL
        Rval = img_ctx_safety_get_luma_noise_reduction(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_luma_noise_reduction", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Pre CC gain
        Rval = img_ctx_safety_get_pre_cc_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_pre_cc_gain", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // CC
        Rval = img_ctx_safety_get_color_correction(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_color_correction", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // tone curve
        Rval = img_ctx_safety_get_tone_curve(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_tone_curve", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // RGB to YUV
        Rval = img_ctx_safety_get_rgb_to_yuv_matrix(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_rgb_to_yuv_matrix", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // RGBIR
        Rval = img_ctx_safety_get_rgb_ir(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_rgb_ir", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Chroma scale
        Rval = img_ctx_safety_get_chroma_scale(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_chroma_scale", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Chroma median
        Rval = img_ctx_safety_get_chroma_median_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_chroma_median_filter", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // SHPA proc mode
        Rval = img_ctx_safety_get_first_luma_processing_mode(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_first_luma_processing_mode", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // ASF
        Rval = img_ctx_safety_get_advance_spatial_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_advance_spatial_filter", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // SHPA
        Rval = img_ctx_safety_get_fstshpns_both(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fstshpns_both", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_fstshpns_noise(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fstshpns_noise", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_fstshpns_fir(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fstshpns_fir", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_fstshp_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fstshp_coring", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_fstshp_cor_idx_scl(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fstshp_cor_idx_scl", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_fstshpns_min_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fstshpns_min_coring_result", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_fstshpns_max_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fstshpns_max_coring_result", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_fstshpns_scale_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fstshpns_scale_coring", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // SHPB
        Rval = img_ctx_safety_get_fnlshp_both(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshp_both", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_fnlshpns_noise(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshpns_noise", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_fnlshpns_fir(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshpns_fir", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_fnlshp_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshp_coring", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_fnlshp_cor_idx_scl(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshp_cor_idx_scl", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_fnlshpns_min_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshpns_min_coring_result", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_fnlshpns_max_coring_result(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshpns_max_coring_result", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_fnlshpns_scale_coring(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshpns_scale_coring", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_fnlshp_both_tdt(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fnlshp_both_tdt", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Chroma filter
        Rval = img_ctx_safety_get_chroma_filter(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_chroma_filter", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Wide chroma filter
        Rval = img_ctx_safety_get_w_chroma_ft(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_w_chroma_ft", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Wide chroma filter combine
        Rval = img_ctx_safety_get_w_chroma_ft_combine(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_w_chroma_ft_combine", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // grgb mismatch
        Rval = img_ctx_safety_get_grgb_mismatch(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_grgb_mismatch", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // MCTF
        Rval = img_ctx_safety_get_video_mctf(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_video_mctf", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_mctf_ta(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_mctf_ta", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_mctf_and_final_sharpen(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_mctf_and_final_sharpen", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Vig
        Rval = img_ctx_safety_get_vignette_enable_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_vignette_enable_info", "test get SafetyFilters null ptr case", id_cnt++);
        Rval = img_ctx_safety_get_vignette(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_vignette", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // HDR blc
        Rval = img_ctx_safety_get_exp0_fe_static_blc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_exp0_fe_static_blc", "test get SafetyFilters null ptr case", id_cnt++);
        Rval = img_ctx_safety_get_exp1_fe_static_blc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_exp1_fe_static_blc", "test get SafetyFilters null ptr case", id_cnt++);
        Rval = img_ctx_safety_get_exp2_fe_static_blc(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_exp2_fe_static_blc", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // HDR gain
        Rval = img_ctx_safety_get_exp0_fe_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_exp0_fe_wb_gain", "test get SafetyFilters null ptr case", id_cnt++);
        Rval = img_ctx_safety_get_exp1_fe_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_exp1_fe_wb_gain", "test get SafetyFilters null ptr case", id_cnt++);
        Rval = img_ctx_safety_get_exp2_fe_wb_gain(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_exp2_fe_wb_gain", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // CE
        Rval = img_ctx_safety_get_ce(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_ce", "test get SafetyFilters null ptr case", id_cnt++);
        Rval = img_ctx_safety_get_ce_input_table(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_ce_input_table", "test get SafetyFilters null ptr case", id_cnt++);
        Rval = img_ctx_safety_get_ce_out_table(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_ce_out_table", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // HDR blend
        Rval = img_ctx_safety_get_hdr_blend(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_hdr_blend", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // HDR tone curve
        Rval = img_ctx_safety_get_fe_tone_curve(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_fe_tone_curve", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // resampler strength
        Rval = img_ctx_safety_get_resampler_str(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_resampler_str", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // AAA
        Rval = img_ctx_safety_get_aaa_stat_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_aaa_stat_info", "test get SafetyFilters null ptr case", id_cnt++);
        Rval = img_ctx_safety_get_aaa_pg_af_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_aaa_pg_af_info", "test get SafetyFilters null ptr case", id_cnt++);
        Rval = img_ctx_safety_get_af_stat_ex_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_af_stat_ex_info", "test get SafetyFilters null ptr case", id_cnt++);
        Rval = img_ctx_safety_get_pg_af_stat_ex_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_pg_af_stat_ex_info", "test get SafetyFilters null ptr case", id_cnt++);
        Rval = img_ctx_safety_get_histogram_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_histogram_info", "test get SafetyFilters null ptr case", id_cnt++);
        Rval = img_ctx_safety_get_pg_histogram_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_pg_histogram_info", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Window info
        Rval = img_ctx_safety_get_window_size_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_window_size_info", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Warp
        Rval = img_ctx_safety_get_warp_enable_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_warp_enable_info", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_calib_warp_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_calib_warp_info", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // CA
        Rval = img_ctx_safety_get_cawarp_enable_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_cawarp_enable_info", "test get SafetyFilters null ptr case", id_cnt++);

        Rval = img_ctx_safety_get_calib_ca_warp_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_calib_ca_warp_info", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // DZoom
        Rval = img_ctx_safety_get_dzoom_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_dzoom_info", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Dummy
        Rval = img_ctx_safety_get_dummy_win_margin_range_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_dummy_win_margin_range_info", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Active win
        Rval = img_ctx_safety_get_vin_active_window(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_vin_active_window", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Warp internal
        Rval = img_ctx_safety_get_warp_internal(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_warp_internal", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // CA internal
        Rval = img_ctx_safety_get_cawarp_internal(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_cawarp_internal", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // HDR RAW info
        Rval = img_ctx_safety_get_hdr_raw_offset(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_hdr_raw_offset", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // flip mode
        Rval = img_ctx_safety_get_flip_mode(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_flip_mode", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Warp buf info
        Rval = img_ctx_safety_get_warp_buffer_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_warp_buffer_info", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Stitch info
        Rval = img_ctx_safety_get_stitching_info(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_stitching_info", "test get SafetyFilters null ptr case", id_cnt++);
    }

    {
        // Burst tile
        Rval = img_ctx_safety_get_burst_tile(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentSafetyFilter.c", "img_ctx_safety_get_burst_tile", "test get SafetyFilters null ptr case", id_cnt++);
    }

    p_ik_buffer_info->safety_state = IK_SAFETY_STATE_OK;
}

static void IK_TestCovr_ImgContextUnitUtility()
{
    UINT32 Rval = IK_OK;

    {
        extern void ctx_raise_update_flag(uint8 *iso_filter_update_flag);
        ctx_raise_update_flag(NULL);
    }

    {
        extern uint32 ctx_check_uint32_parameter_valid_range(char const *parameter_name, uint32 parameter_value, uint32 min_value, uint32 max_value);
        Rval = ctx_check_uint32_parameter_valid_range(NULL, 0, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextUnitUtility.c", "ctx_check_uint32_parameter_valid_range", "test null ptr fail case", id_cnt++);
    }

    {
        extern uint32 ctx_check_int32_parameter_valid_range(char const *parameter_name, int32 parameter_value, int32 min_value, int32 max_value);
        Rval = ctx_check_int32_parameter_valid_range(NULL, 0, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextUnitUtility.c", "ctx_check_int32_parameter_valid_range", "test null ptr fail case", id_cnt++);

        Rval = ctx_check_int32_parameter_valid_range("test", 1, 2, 3);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextUnitUtility.c", "ctx_check_int32_parameter_valid_range", "test (parameter_value < min_value) true fail case", id_cnt++);
    }

    {
        extern uint32 ctx_check_level_control_uint32_parameter_valid_range(char const *prefix_parameter_name, char const *postfix_parameter_name, uint32 parameter_value, uint32 min_value, uint32 max_value);
        Rval = ctx_check_level_control_uint32_parameter_valid_range(NULL, "postfix_name", 0u, 0u, 0u);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextUnitUtility.c", "ctx_check_level_control_uint32_parameter_valid_range", "test null ptr fail case", id_cnt++);
        Rval = ctx_check_level_control_uint32_parameter_valid_range("prefix_name", NULL, 0u, 0u, 0u);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextUnitUtility.c", "ctx_check_level_control_uint32_parameter_valid_range", "test null ptr fail case", id_cnt++);
    }
}

static void IK_TestCovr_ImgContextComponentIF(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    ik_ability_t ability = {0};
    amba_ik_context_entity_t *p_ctx;

    img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    img_ctx_get_context(pMode->ContextId, &p_ctx);

    {
        Rval = img_ctx_query_context_memory_size(NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_query_context_memory_size", "test null ptr fail case", id_cnt++);
    }

    {
        Rval = img_ctx_reset_update_flags(5);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_reset_update_flags", "test (context_id > img_arch_get_context_number()) fail case", id_cnt++);
    }

    {
        ik_system_api_t sys_api;

        ability.pipe = AMBA_IK_PIPE_VIDEO;
        ability.video_pipe = AMBA_IK_VIDEO_LINEAR;
        p_ik_buffer_info->init_flag = 0;
        Rval = img_ctx_init_context(pMode->ContextId, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_init_context", "test (p_ik_buffer_info->init_flag == 1u) false case", id_cnt++);
        p_ik_buffer_info->init_flag = 1;

        Rval = img_ctx_init_context(33, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_init_context", "test get ctx fail case", id_cnt++);

        ability.pipe = AMBA_IK_PIPE_STILL;
        Rval = img_ctx_init_context(pMode->ContextId, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_init_context", "test (p_ability->pipe == AMBA_IK_PIPE_STILL) fail case", id_cnt++);

        ability.pipe = AMBA_IK_PIPE_VIDEO;
        ability.video_pipe = AMBA_IK_VIDEO_MAX;
        Rval = img_ctx_init_context(pMode->ContextId, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_init_context", "test (p_ability->video_pipe < AMBA_IK_VIDEO_MAX) fail case", id_cnt++);

        ability.pipe = 2;
        Rval = img_ctx_init_context(pMode->ContextId, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_init_context", "test (p_ability->pipe != VIDEO and STILL) fail case", id_cnt++);

        ability.pipe = AMBA_IK_PIPE_VIDEO;
        ability.video_pipe = AMBA_IK_VIDEO_LINEAR;
        p_ik_buffer_info->init_mode = AMBA_IK_ARCH_SOFT_INIT;
        Rval = img_ctx_init_context(pMode->ContextId, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_init_context", "test p_ik_buffer_info->init_mode == AMBA_IK_ARCH_HARD_INIT false case", id_cnt++);
        p_ik_buffer_info->init_mode = AMBA_IK_ARCH_HARD_INIT;

        sys_api.system_create_mutex = (ik_system_create_mutex_t)fake_create_mutex_fail;
        img_arch_registe_system_create_mutex(&(sys_api.system_create_mutex));
        p_ctx->organization.initial_flag = 0U;
        Rval = img_ctx_init_context(pMode->ContextId, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_init_context", "test amba_ik_system_create_mutex() fail case", id_cnt++);
        p_ctx->organization.initial_flag = 1U;
        sys_api.system_create_mutex = (ik_system_create_mutex_t)fake_create_mutex;
        img_arch_registe_system_create_mutex(&(sys_api.system_create_mutex));
    }

    {
        Rval = img_ctx_get_context_ability(pMode->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_get_context_ability", "test null ptr fail case", id_cnt++);

        Rval = img_ctx_get_context_ability(33, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_get_context_ability", "test get ctx fail case", id_cnt++);

        p_ctx->organization.initial_flag = 0U;
        Rval = img_ctx_get_context_ability(pMode->ContextId, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_get_context_ability", "test (p_ctx->organization.initial_flag != 0U) fail case", id_cnt++);
        p_ctx->organization.initial_flag = 1U;
    }

    {
        Rval = img_ctx_get_flow_control(pMode->ContextId, 0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_get_flow_control", "test null ptr fail case", id_cnt++);
    }

    {
        uintptr addr = 0;

        Rval = img_ctx_get_flow_tbl(pMode->ContextId, 0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_get_flow_tbl", "test null ptr fail case", id_cnt++);
        Rval = img_ctx_get_flow_tbl(33, 0, &addr);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_get_flow_tbl", "test get ctx fail case", id_cnt++);
        Rval = img_ctx_get_flow_tbl(pMode->ContextId, 33, &addr);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_get_flow_tbl", "test (p_ctx->organization.attribute.cr_ring_number > flow_idx) fail case", id_cnt++);
    }

    {
        Rval = img_ctx_get_flow_tbl_list(pMode->ContextId, 0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_get_flow_tbl_list", "test null ptr fail case", id_cnt++);
    }

    {
        uintptr addr = 0;

        Rval = img_ctx_get_crc_data(pMode->ContextId, 0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_get_crc_data", "test null ptr fail case", id_cnt++);
        Rval = img_ctx_get_crc_data(33, 0, &addr);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_get_crc_data", "test get ctx fail case", id_cnt++);
        Rval = img_ctx_get_crc_data(pMode->ContextId, 33, &addr);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_get_crc_data", "test (p_ctx->organization.attribute.cr_ring_number > flow_idx) fail case", id_cnt++);

        Rval = img_ctx_get_crc_data(pMode->ContextId, 0, &addr);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_get_crc_data", "test OK case", id_cnt++);
    }
}

static void IK_TestCovr_ImgContextComponentIF_PrepareCtxMem(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;
    uintptr tmp = 0x12345678;

    {
        Rval = img_ctx_prepare_context_memory(pMode->ContextId, 0, 100);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_prepare_context_memory", "test null ptr fail case", id_cnt++);

        Rval = img_ctx_prepare_context_memory(pMode->ContextId, tmp, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_prepare_context_memory", "test (mem_size == (size_t)0) fail case", id_cnt++);

        Rval = img_ctx_prepare_context_memory(5, tmp, 100);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_prepare_context_memory", "test (context_id > context_number) fail case", id_cnt++);

        img_arch_deinit_architecture();
        Rval = img_ctx_prepare_context_memory(pMode->ContextId, tmp, 100);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgContextComponentIF.c", "img_ctx_prepare_context_memory", "test img_arch_get_ik_working_buffer() fail case", id_cnt++);
    }
}

INT32 ik_ctest_context_main(void)
{
    UINT32 Rval = IK_OK;
    AMBA_IK_ABILITY_s Ability[3];
    AMBA_IK_MODE_CFG_s Mode[3];

    memset(ik_working_buffer, 0, sizeof(ik_working_buffer));
    memset(WarpGrid, 0, sizeof(WarpGrid));
    memset(CawarpRedGrid, 0, sizeof(CawarpRedGrid));
    memset(CawarpBlueGrid, 0, sizeof(CawarpBlueGrid));
    memset(FPNMap, 0, sizeof(FPNMap));
    id_cnt = 0;

    pFile = fopen("data/ik_ctest_context.txt", "w");

    // init IK
    _load_idsp_default_binary();
    Ability[0].Pipe = AMBA_IK_PIPE_VIDEO;
    Ability[0].VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_3;
    Ability[1].Pipe = AMBA_IK_PIPE_VIDEO;
    Ability[1].VideoPipe = AMBA_IK_VIDEO_Y2Y;
    Ability[2].Pipe = AMBA_IK_PIPE_VIDEO;
    Ability[2].VideoPipe = AMBA_IK_VIDEO_LINEAR_CE;

    _Init_Image_Kernel(Ability);
    _Init_Image_Kernel_Core();

    Mode[0].ContextId = 0;
    Rval |= AmbaIK_InitContext(&Mode[0], &Ability[0]);

    Mode[1].ContextId = 1;
    Rval |= AmbaIK_InitContext(&Mode[1], &Ability[1]);

    Mode[2].ContextId = 2;
    Rval |= AmbaIK_InitContext(&Mode[2], &Ability[2]);

    ik_init_debug_check_func();

    // test AmbaDSP_ImgContextComponentCalib.c
    IK_TestCovr_ImgContextComponentCalib(&Mode[0]);
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgContextComponentHDR.c
    IK_TestCovr_ImgContextComponentHDR(&Mode[0]);
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgContextComponentNormalFilter.c
    IK_TestCovr_ImgContextComponentNormalFilter(&Mode[0]);
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgContextDebugCheck.c
    IK_TestCovr_ImgContextDebugCheck(&Mode[0], &Mode[1]);
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgContextComponentSafetyFilter.c
    IK_TestCovr_ImgContextComponentSafetyFilter(&Mode[0]);
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgContextUnitUtility.c
    IK_TestCovr_ImgContextUnitUtility();
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgContextComponentIF.c
    IK_TestCovr_ImgContextComponentIF(&Mode[0]);
    _Reset_Image_Kernel(Ability);
    IK_TestCovr_ImgContextComponentIF_PrepareCtxMem(&Mode[0]);
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

