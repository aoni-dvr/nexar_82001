#include "test_ik_global.h"


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

static void IKS_Set_ImgFilters_NullPtr(const AMBA_IK_MODE_CFG_s *pMode0, char *p_desc_name)
{
    UINT32 Rval = IK_OK;

    {
        // Safety info
        Rval = iks_set_safety_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_safety_info", p_desc_name, id_cnt++);
    }

    {
        // Sensor info
        Rval = iks_set_vin_sensor_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_vin_sensor_info", p_desc_name, id_cnt++);
    }

    {
        // Before CE
        Rval = iks_set_before_ce_wb_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_before_ce_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // After CE
        Rval = iks_set_after_ce_wb_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_after_ce_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // CFA leakage
        Rval = iks_set_cfa_leakage_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_cfa_leakage_filter", p_desc_name, id_cnt++);
    }

    {
        // Anti aliasing
        Rval = iks_set_anti_aliasing(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_anti_aliasing", p_desc_name, id_cnt++);
    }

    {
        // DBP
        Rval = iks_set_dynamic_bad_pixel_corr(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_dynamic_bad_pixel_corr", p_desc_name, id_cnt++);
    }

    {
        // SBP
        Rval = iks_set_static_bad_pxl_corr(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_static_bad_pxl_corr", p_desc_name, id_cnt++);
    }

    {
        // SBP internal
        Rval = iks_set_static_bad_pxl_corr_itnl(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_static_bad_pxl_corr_itnl", p_desc_name, id_cnt++);
    }

    {
        // CFA noise
        Rval = iks_set_cfa_noise_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_cfa_noise_filter", p_desc_name, id_cnt++);
    }

    {
        // Demosaic
        Rval = iks_set_demosaic(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_demosaic", p_desc_name, id_cnt++);
    }

    {
        // RGB_12Y
        Rval = iks_set_rgb_to_12y(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_rgb_to_12y", p_desc_name, id_cnt++);
    }

    {
        // LNL
        Rval = iks_set_luma_noise_reduction(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_luma_noise_reduction", p_desc_name, id_cnt++);
    }

    {
        // Pre CC gain
        Rval = iks_set_pre_cc_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_pre_cc_gain", p_desc_name, id_cnt++);
    }

    {
        // CC
        Rval = iks_set_color_correction(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_color_correction", p_desc_name, id_cnt++);
    }

    {
        // tone curve
        Rval = iks_set_tone_curve(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_tone_curve", p_desc_name, id_cnt++);
    }

    {
        // RGB to YUV
        Rval = iks_set_rgb_to_yuv_matrix(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_rgb_to_yuv_matrix", p_desc_name, id_cnt++);
    }

    {
        // RGBIR
        Rval = iks_set_rgb_ir(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_rgb_ir", p_desc_name, id_cnt++);
    }

    {
        // Chroma scale
        Rval = iks_set_chroma_scale(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_chroma_scale", p_desc_name, id_cnt++);
    }

    {
        // Chroma median
        Rval = iks_set_chroma_median_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_chroma_median_filter", p_desc_name, id_cnt++);
    }

    {
        // SHPA proc mode
        Rval = iks_set_fst_luma_process_mode(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_luma_process_mode", p_desc_name, id_cnt++);
    }

    {
        // ASF
        Rval = iks_set_adv_spatial_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_adv_spatial_filter", p_desc_name, id_cnt++);
    }

    {
        // SHPA
        Rval = iks_set_fst_shp_both(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_shp_both", p_desc_name, id_cnt++);

        Rval = iks_set_fst_shp_noise(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_shp_noise", p_desc_name, id_cnt++);

        Rval = iks_set_fst_shp_fir(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_shp_fir", p_desc_name, id_cnt++);

        Rval = iks_set_fst_shp_coring(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_shp_coring", p_desc_name, id_cnt++);

        Rval = iks_set_fst_shp_coring_idx_scale(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_shp_coring_idx_scale", p_desc_name, id_cnt++);

        Rval = iks_set_fst_shp_min_coring_rslt(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_shp_min_coring_rslt", p_desc_name, id_cnt++);

        Rval = iks_set_fst_shp_max_coring_rslt(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_shp_max_coring_rslt", p_desc_name, id_cnt++);

        Rval = iks_set_fst_shp_scale_coring(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_shp_scale_coring", p_desc_name, id_cnt++);
    }

    {
        // SHPB
        Rval = iks_set_fnl_shp_both(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_both", p_desc_name, id_cnt++);

        Rval = iks_set_fnl_shp_noise(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_noise", p_desc_name, id_cnt++);

        Rval = iks_set_fnl_shp_fir(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_fir", p_desc_name, id_cnt++);

        Rval = iks_set_fnl_shp_coring(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_coring", p_desc_name, id_cnt++);

        Rval = iks_set_fnl_shp_coring_idx_scale(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_coring_idx_scale", p_desc_name, id_cnt++);

        Rval = iks_set_fnl_shp_min_coring_rslt(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_min_coring_rslt", p_desc_name, id_cnt++);

        Rval = iks_set_fnl_shp_max_coring_rslt(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_max_coring_rslt", p_desc_name, id_cnt++);

        Rval = iks_set_fnl_shp_scale_coring(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_scale_coring", p_desc_name, id_cnt++);

        Rval = iks_set_fnl_shp_three_d_table(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_three_d_table", p_desc_name, id_cnt++);
    }

    {
        // Chroma filter
        Rval = iks_set_chroma_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_chroma_filter", p_desc_name, id_cnt++);
    }

    {
        // Wide chroma filter
        Rval = iks_set_wide_chroma_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_wide_chroma_filter", p_desc_name, id_cnt++);
    }

    {
        // Wide chroma filter combine
        Rval = iks_set_wide_chroma_filter_combine(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_wide_chroma_filter_combine", p_desc_name, id_cnt++);
    }

    {
        // grgb mismatch
        Rval = iks_set_grgb_mismatch(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_grgb_mismatch", p_desc_name, id_cnt++);
    }

    {
        // MCTF
        Rval = iks_set_video_mctf(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_video_mctf", p_desc_name, id_cnt++);

        Rval = iks_set_video_mctf_ta(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_video_mctf_ta", p_desc_name, id_cnt++);

        Rval = iks_set_video_mctf_and_fnl_shp(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_video_mctf_and_fnl_shp", p_desc_name, id_cnt++);
    }

    {
        // Vig
        Rval = iks_set_vignette(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_vignette", p_desc_name, id_cnt++);
    }

    {
        // HDR blc
        Rval = iks_set_exp0_frontend_static_blc(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_exp0_frontend_static_blc", p_desc_name, id_cnt++);
        Rval = iks_set_exp1_frontend_static_blc(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_exp1_frontend_static_blc", p_desc_name, id_cnt++);
        Rval = iks_set_exp2_frontend_static_blc(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_exp2_frontend_static_blc", p_desc_name, id_cnt++);
    }

    {
        // HDR gain
        Rval = iks_set_exp0_frontend_wb_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_exp0_frontend_wb_gain", p_desc_name, id_cnt++);
        Rval = iks_set_exp1_frontend_wb_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_exp1_frontend_wb_gain", p_desc_name, id_cnt++);
        Rval = iks_set_exp2_frontend_wb_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_exp2_frontend_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // CE
        Rval = iks_set_ce(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_ce", p_desc_name, id_cnt++);
        Rval = iks_set_ce_input_table(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_ce_input_table", p_desc_name, id_cnt++);
        Rval = iks_set_ce_out_table(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_ce_out_table", p_desc_name, id_cnt++);
    }

    {
        // HDR blend
        Rval = iks_set_hdr_blend(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_hdr_blend", p_desc_name, id_cnt++);
    }

    {
        // HDR tone curve
        Rval = iks_set_frontend_tone_curve(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_frontend_tone_curve", p_desc_name, id_cnt++);
    }

    {
        // resampler strength
        Rval = iks_set_resampler_strength(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_resampler_strength", p_desc_name, id_cnt++);
    }

    {
        // AAA
        Rval = iks_set_aaa_stat_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_aaa_stat_info", p_desc_name, id_cnt++);
        Rval = iks_set_aaa_pg_af_stat_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_aaa_pg_af_stat_info", p_desc_name, id_cnt++);
        Rval = iks_set_af_stat_ex_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_af_stat_ex_info", p_desc_name, id_cnt++);
        Rval = iks_set_pg_af_stat_ex_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_pg_af_stat_ex_info", p_desc_name, id_cnt++);
        Rval = iks_set_histogram_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_histogram_info", p_desc_name, id_cnt++);
        Rval = iks_set_pg_histogram_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_pg_histogram_info", p_desc_name, id_cnt++);
    }

    {
        // Window info
        Rval = iks_set_window_size_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_window_size_info", p_desc_name, id_cnt++);
    }

    {
        // Warp
        Rval = iks_set_warp_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_warp_info", p_desc_name, id_cnt++);
    }

    {
        // CA
        Rval = iks_set_cawarp_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_cawarp_info", p_desc_name, id_cnt++);
    }

    {
        // DZoom
        Rval = iks_set_dzoom_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_dzoom_info", p_desc_name, id_cnt++);
    }

    {
        // Dummy
        Rval = iks_set_dummy_margin_range(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_dummy_margin_range", p_desc_name, id_cnt++);
    }

    {
        // Active win
        Rval = iks_set_vin_active_win(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_vin_active_win", p_desc_name, id_cnt++);
    }

    {
        // Warp internal
        Rval = iks_set_warp_internal(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_warp_internal", p_desc_name, id_cnt++);
    }

    {
        // CA internal
        Rval = iks_set_cawarp_internal(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_cawarp_internal", p_desc_name, id_cnt++);
    }

    {
        // HDR RAW info
        Rval = iks_set_hdr_raw_offset(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_hdr_raw_offset", p_desc_name, id_cnt++);
    }

    {
        // Warp buf info
        Rval = iks_set_warp_buffer_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_warp_buffer_info", p_desc_name, id_cnt++);
    }

    {
        // Stitch info
        Rval = iks_set_stitching_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_stitching_info", p_desc_name, id_cnt++);
    }

    {
        // Burst tile
        Rval = iks_set_burst_tile(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_burst_tile", p_desc_name, id_cnt++);
    }
}

static void IKS_Set_ImgFilters(const AMBA_IK_MODE_CFG_s *pMode0, const AMBA_IK_MODE_CFG_s *pMode1, char *p_desc_name)
{
    UINT32 Rval = IK_OK;

    {
        // Safety info
        ik_safety_info_t safety_info = {0};

        safety_info.update_freq = 1;
        Rval = iks_set_safety_info(pMode0->ContextId, &safety_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_safety_info", p_desc_name, id_cnt++);
    }

    {
        // Sensor info
        ik_vin_sensor_info_t sensor_info = {0};

        Rval = iks_set_vin_sensor_info(pMode0->ContextId, &sensor_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_vin_sensor_info", p_desc_name, id_cnt++);
    }

    {
        // YUV mode
        uint32 yuv_mode = 0;

        Rval = iks_set_yuv_mode(pMode1->ContextId, yuv_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_yuv_mode", p_desc_name, id_cnt++);
    }

    {
        // Before CE
        ik_wb_gain_t before_ce_wb_gain = {0};

        before_ce_wb_gain.gain_b = 4096;
        before_ce_wb_gain.gain_g = 4096;
        before_ce_wb_gain.gain_r = 4096;
        Rval = iks_set_before_ce_wb_gain(pMode0->ContextId, &before_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_before_ce_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // After CE
        ik_wb_gain_t after_ce_wb_gain = {0};

        after_ce_wb_gain.gain_b = 4096;
        after_ce_wb_gain.gain_g = 4096;
        after_ce_wb_gain.gain_r = 4096;
        Rval = iks_set_after_ce_wb_gain(pMode0->ContextId, &after_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_after_ce_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // CFA leakage
        ik_cfa_leakage_filter_t cfa_leakage_filter = {0};

        Rval = iks_set_cfa_leakage_filter(pMode0->ContextId, &cfa_leakage_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_cfa_leakage_filter", p_desc_name, id_cnt++);
    }

    {
        // Anti aliasing
        ik_anti_aliasing_t anti_aliasing = {0};

        Rval = iks_set_anti_aliasing(pMode0->ContextId, &anti_aliasing);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_anti_aliasing", p_desc_name, id_cnt++);
    }

    {
        // DBP
        ik_dynamic_bad_pixel_correction_t dynamic_bpc = {0};

        Rval = iks_set_dynamic_bad_pixel_corr(pMode0->ContextId, &dynamic_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_dynamic_bad_pixel_corr", p_desc_name, id_cnt++);
    }

    {
        // SBP
        uint32 enb = 0;
        ik_static_bad_pxl_cor_t static_bpc = {0};

        Rval = iks_set_static_bad_pxl_corr_enb(pMode0->ContextId, enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_static_bad_pxl_corr_enb", p_desc_name, id_cnt++);

        static_bpc.calib_sbp_info.version = 0x20180401;
        static_bpc.calib_sbp_info.sbp_buffer = FPNMap;
        static_bpc.vin_sensor_geo.width = 1920;
        static_bpc.vin_sensor_geo.height = 1080;
        static_bpc.vin_sensor_geo.h_sub_sample.factor_den = 1;
        static_bpc.vin_sensor_geo.h_sub_sample.factor_num = 1;
        static_bpc.vin_sensor_geo.v_sub_sample.factor_den = 1;
        static_bpc.vin_sensor_geo.v_sub_sample.factor_num = 1;
        Rval = iks_set_static_bad_pxl_corr(pMode0->ContextId, &static_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_static_bad_pxl_corr", p_desc_name, id_cnt++);
    }

    {
        // SBP internal
        ik_static_bad_pixel_correction_internal_t static_bpc_internal = {0};

        static_bpc_internal.p_map = FPNMap;
        Rval = iks_set_static_bad_pxl_corr_itnl(pMode0->ContextId, &static_bpc_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_static_bad_pxl_corr_itnl", p_desc_name, id_cnt++);
    }

    {
        // CFA noise
        ik_cfa_noise_filter_t cfa_noise_filter = {0};

        Rval = iks_set_cfa_noise_filter(pMode0->ContextId, &cfa_noise_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_cfa_noise_filter", p_desc_name, id_cnt++);
    }

    {
        // Demosaic
        ik_demosaic_t demosaic = {0};

        Rval = iks_set_demosaic(pMode0->ContextId, &demosaic);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_demosaic", p_desc_name, id_cnt++);
    }

    {
        // RGB_12Y
        ik_rgb_to_12y_t rgb_to_12y = {0};

        Rval = iks_set_rgb_to_12y(pMode0->ContextId, &rgb_to_12y);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_rgb_to_12y", p_desc_name, id_cnt++);
    }

    {
        // LNL
        ik_luma_noise_reduction_t luma_noise_reduce = {0};

        Rval = iks_set_luma_noise_reduction(pMode0->ContextId, &luma_noise_reduce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_luma_noise_reduction", p_desc_name, id_cnt++);
    }

    {
        // Pre CC gain
        ik_pre_cc_gain_t pre_cc_gain = {0};

        Rval = iks_set_pre_cc_gain(pMode0->ContextId, &pre_cc_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_pre_cc_gain", p_desc_name, id_cnt++);
    }

    {
        // CC
        ik_color_correction_t color_correction = {0};

        Rval = iks_set_color_correction(pMode0->ContextId, &color_correction);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_color_correction", p_desc_name, id_cnt++);
    }

    {
        // tone curve
        ik_tone_curve_t tone_curve = {0};

        Rval = iks_set_tone_curve(pMode0->ContextId, &tone_curve);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_tone_curve", p_desc_name, id_cnt++);
    }

    {
        // RGB to YUV
        ik_rgb_to_yuv_matrix_t rgb_to_yuv_matrix = {0};

        Rval = iks_set_rgb_to_yuv_matrix(pMode0->ContextId, &rgb_to_yuv_matrix);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_rgb_to_yuv_matrix", p_desc_name, id_cnt++);
    }

    {
        // RGBIR
        ik_rgb_ir_t rgb_ir = {0};

        Rval = iks_set_rgb_ir(pMode0->ContextId, &rgb_ir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_rgb_ir", p_desc_name, id_cnt++);
    }

    {
        // Chroma scale
        ik_chroma_scale_t chroma_scale = {0};

        Rval = iks_set_chroma_scale(pMode0->ContextId, &chroma_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_chroma_scale", p_desc_name, id_cnt++);
    }

    {
        // Chroma median
        ik_chroma_median_filter_t chroma_median_filter = {0};

        Rval = iks_set_chroma_median_filter(pMode0->ContextId, &chroma_median_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_chroma_median_filter", p_desc_name, id_cnt++);
    }

    {
        // SHPA proc mode
        ik_first_luma_process_mode_t first_luma_process_mode = {0};

        Rval = iks_set_fst_luma_process_mode(pMode0->ContextId, &first_luma_process_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_luma_process_mode", p_desc_name, id_cnt++);
    }

    {
        // ASF
        ik_adv_spatial_filter_t advance_spatial_filter = {0};

        Rval = iks_set_adv_spatial_filter(pMode0->ContextId, &advance_spatial_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_adv_spatial_filter", p_desc_name, id_cnt++);
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

        Rval = iks_set_fst_shp_both(pMode0->ContextId, &first_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_shp_both", p_desc_name, id_cnt++);

        first_sharpen_noise.level_str_adjust.high = 100;
        Rval = iks_set_fst_shp_noise(pMode0->ContextId, &first_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_shp_noise", p_desc_name, id_cnt++);

        Rval = iks_set_fst_shp_fir(pMode0->ContextId, &first_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_shp_fir", p_desc_name, id_cnt++);

        first_sharpen_coring.fractional_bits = 2;
        Rval = iks_set_fst_shp_coring(pMode0->ContextId, &first_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_shp_coring", p_desc_name, id_cnt++);

        first_sharpen_coring_idx_scale.high = 100;
        Rval = iks_set_fst_shp_coring_idx_scale(pMode0->ContextId, &first_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_shp_coring_idx_scale", p_desc_name, id_cnt++);

        first_sharpen_min_coring_result.high = 100;
        Rval = iks_set_fst_shp_min_coring_rslt(pMode0->ContextId, &first_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_shp_min_coring_rslt", p_desc_name, id_cnt++);

        first_sharpen_max_coring_result.high = 100;
        Rval = iks_set_fst_shp_max_coring_rslt(pMode0->ContextId, &first_sharpen_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_shp_max_coring_rslt", p_desc_name, id_cnt++);

        first_sharpen_scale_coring.high = 100;
        Rval = iks_set_fst_shp_scale_coring(pMode0->ContextId, &first_sharpen_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fst_shp_scale_coring", p_desc_name, id_cnt++);
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

        Rval = iks_set_fnl_shp_both(pMode0->ContextId, &final_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_both", p_desc_name, id_cnt++);

        final_sharpen_noise.level_str_adjust.high = 100;
        Rval = iks_set_fnl_shp_noise(pMode0->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_noise", p_desc_name, id_cnt++);

        Rval = iks_set_fnl_shp_fir(pMode0->ContextId, &final_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_fir", p_desc_name, id_cnt++);

        final_sharpen_coring.fractional_bits = 2;
        Rval = iks_set_fnl_shp_coring(pMode0->ContextId, &final_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_coring", p_desc_name, id_cnt++);

        final_sharpen_coring_idx_scale.high = 100;
        Rval = iks_set_fnl_shp_coring_idx_scale(pMode0->ContextId, &final_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_coring_idx_scale", p_desc_name, id_cnt++);

        final_sharpen_min_coring_result.high = 100;
        Rval = iks_set_fnl_shp_min_coring_rslt(pMode0->ContextId, &final_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_min_coring_rslt", p_desc_name, id_cnt++);

        final_sharpen_max_coring_result.high = 100;
        Rval = iks_set_fnl_shp_max_coring_rslt(pMode0->ContextId, &final_sharpen_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_max_coring_rslt", p_desc_name, id_cnt++);

        final_sharpen_scale_coring.high = 100;
        Rval = iks_set_fnl_shp_scale_coring(pMode0->ContextId, &final_sharpen_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_scale_coring", p_desc_name, id_cnt++);

        Rval = iks_set_fnl_shp_three_d_table(pMode0->ContextId, &final_sharpen_both_three_d_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_fnl_shp_three_d_table", p_desc_name, id_cnt++);
    }

    {
        // Chroma filter
        ik_chroma_filter_t chroma_filter = {0};

        chroma_filter.radius = 32;
        Rval = iks_set_chroma_filter(pMode0->ContextId, &chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_chroma_filter", p_desc_name, id_cnt++);
    }

    {
        // Wide chroma filter
        ik_wide_chroma_filter_t wide_chroma_filter = {0};

        Rval = iks_set_wide_chroma_filter(pMode0->ContextId, &wide_chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_wide_chroma_filter", p_desc_name, id_cnt++);
    }

    {
        // Wide chroma filter combine
        ik_wide_chroma_filter_combine_t chroma_filter_combine = {0};

        chroma_filter_combine.T1_cb = 10;
        chroma_filter_combine.T1_cr = 10;
        Rval = iks_set_wide_chroma_filter_combine(pMode0->ContextId, &chroma_filter_combine);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_wide_chroma_filter_combine", p_desc_name, id_cnt++);
    }

    {
        // grgb mismatch
        ik_grgb_mismatch_t grgb_mismatch = {0};

        Rval = iks_set_grgb_mismatch(pMode0->ContextId, &grgb_mismatch);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_grgb_mismatch", p_desc_name, id_cnt++);
    }

    {
        // MCTF
        ik_video_mctf_t video_mctf = {0};
        ik_video_mctf_ta_t video_mctf_ta = {0};
        ik_pos_dep33_t video_mctf_and_final_sharpen = {0};

        Rval = iks_set_video_mctf(pMode0->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_video_mctf", p_desc_name, id_cnt++);

        Rval = iks_set_video_mctf_ta(pMode0->ContextId, &video_mctf_ta);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_video_mctf_ta", p_desc_name, id_cnt++);

        Rval = iks_set_video_mctf_and_fnl_shp(pMode0->ContextId, &video_mctf_and_final_sharpen);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_video_mctf_and_fnl_shp", p_desc_name, id_cnt++);
    }

    {
        // Vig
        uint32 enb = 0;
        ik_vignette_t vignette = {0};

        Rval = iks_set_vignette_enb(pMode0->ContextId, enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_vignette_enb", p_desc_name, id_cnt++);
        Rval = iks_set_vignette(pMode0->ContextId, &vignette);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_vignette", p_desc_name, id_cnt++);
    }

    {
        // HDR blc
        ik_static_blc_level_t frontend_static_blc = {0};

        Rval = iks_set_exp0_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_exp0_frontend_static_blc", p_desc_name, id_cnt++);
        Rval = iks_set_exp1_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_exp1_frontend_static_blc", p_desc_name, id_cnt++);
        Rval = iks_set_exp2_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_exp2_frontend_static_blc", p_desc_name, id_cnt++);
    }

    {
        // HDR gain
        ik_frontend_wb_gain_t frontend_wb_gain = {0};

        frontend_wb_gain.shutter_ratio = 4096;
        Rval = iks_set_exp0_frontend_wb_gain(pMode0->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_exp0_frontend_wb_gain", p_desc_name, id_cnt++);
        Rval = iks_set_exp1_frontend_wb_gain(pMode0->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_exp1_frontend_wb_gain", p_desc_name, id_cnt++);
        Rval = iks_set_exp2_frontend_wb_gain(pMode0->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_exp2_frontend_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // CE
        ik_ce_t ce = {0};
        ik_ce_input_table_t ce_input_table = {0};
        ik_ce_output_table_t ce_out_table = {0};
        ce.coring_gain_high = 1U;
        ce.coring_index_scale_high = 1U;

        Rval = iks_set_ce(pMode0->ContextId, &ce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_ce", p_desc_name, id_cnt++);
        Rval = iks_set_ce_input_table(pMode0->ContextId, &ce_input_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_ce_input_table", p_desc_name, id_cnt++);
        Rval = iks_set_ce_out_table(pMode0->ContextId, &ce_out_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_ce_out_table", p_desc_name, id_cnt++);
    }

    {
        // HDR blend
        ik_hdr_blend_t hdr_blend = {0};

        Rval = iks_set_hdr_blend(pMode0->ContextId, &hdr_blend);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_hdr_blend", p_desc_name, id_cnt++);
    }

    {
        // HDR tone curve
        ik_frontend_tone_curve_t fe_tc = {0};

        Rval = iks_set_frontend_tone_curve(pMode0->ContextId, &fe_tc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_frontend_tone_curve", p_desc_name, id_cnt++);
    }

    {
        // resampler strength
        ik_resampler_strength_t resampler_strength = {0};

        Rval = iks_set_resampler_strength(pMode0->ContextId, &resampler_strength);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_resampler_strength", p_desc_name, id_cnt++);
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
        Rval = iks_set_aaa_stat_info(pMode0->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_aaa_stat_info", p_desc_name, id_cnt++);
        Rval = iks_set_aaa_pg_af_stat_info(pMode0->ContextId, &pg_af_stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_aaa_pg_af_stat_info", p_desc_name, id_cnt++);
        Rval = iks_set_af_stat_ex_info(pMode0->ContextId, &af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_af_stat_ex_info", p_desc_name, id_cnt++);
        Rval = iks_set_pg_af_stat_ex_info(pMode0->ContextId, &pg_af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_pg_af_stat_ex_info", p_desc_name, id_cnt++);
        Rval = iks_set_histogram_info(pMode0->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_histogram_info", p_desc_name, id_cnt++);
        Rval = iks_set_pg_histogram_info(pMode0->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_pg_histogram_info", p_desc_name, id_cnt++);
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
        Rval = iks_set_window_size_info(pMode0->ContextId, &window_size_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_window_size_info", p_desc_name, id_cnt++);
    }

    {
        // Warp
        uint32 enb = 0;
        ik_warp_info_t calib_warp_info = {0};

        Rval = iks_set_warp_enb(pMode0->ContextId, enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_warp_enb", p_desc_name, id_cnt++);

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
        Rval = iks_set_warp_info(pMode0->ContextId, &calib_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_warp_info", p_desc_name, id_cnt++);
    }

    {
        // CA
        uint32 enb = 0;
        ik_cawarp_info_t calib_ca_warp_info = {0};

        Rval = iks_set_cawarp_enb(pMode0->ContextId, enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_cawarp_enb", p_desc_name, id_cnt++);

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
        Rval = iks_set_cawarp_info(pMode0->ContextId, &calib_ca_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_cawarp_info", p_desc_name, id_cnt++);
    }

    {
        // DZoom
        ik_dzoom_info_t dzoom_info = {0};

        Rval = iks_set_dzoom_info(pMode0->ContextId, &dzoom_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_dzoom_info", p_desc_name, id_cnt++);
    }

    {
        // Dummy
        ik_dummy_margin_range_t dmy_margin_range = {0};

        Rval = iks_set_dummy_margin_range(pMode0->ContextId, &dmy_margin_range);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_dummy_margin_range", p_desc_name, id_cnt++);
    }

    {
        // Active win
        ik_vin_active_window_t vin_active_win = {0};

        Rval = iks_set_vin_active_win(pMode0->ContextId, &vin_active_win);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_vin_active_win", p_desc_name, id_cnt++);
    }

    {
        // Warp internal
        ik_warp_internal_info_t warp_dzoom_internal = {0};

        warp_dzoom_internal.pwarp_horizontal_table = (int16 *)WarpGrid;
        warp_dzoom_internal.pwarp_vertical_table = (int16 *)(WarpGrid + (128*96*4));
        Rval = iks_set_warp_internal(pMode0->ContextId, &warp_dzoom_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_warp_internal", p_desc_name, id_cnt++);
    }

    {
        // CA internal
        ik_cawarp_internal_info_t cawarp_internal = {0};

        Rval = iks_set_cawarp_internal(pMode0->ContextId, &cawarp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_cawarp_internal", p_desc_name, id_cnt++);
    }

    {
        // HDR RAW info
        ik_hdr_raw_info_t hdr_raw_info = {0};

        Rval = iks_set_hdr_raw_offset(pMode0->ContextId, &hdr_raw_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_hdr_raw_offset", p_desc_name, id_cnt++);
    }

    {
        // flip mode
        uint32 mode = 0;

        Rval = iks_set_flip_mode(pMode0->ContextId, mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_flip_mode", p_desc_name, id_cnt++);
    }

    {
        // Warp buf info
        ik_warp_buffer_info_t warp_buf_info = {0};

        warp_buf_info.luma_wait_lines = 8;
        warp_buf_info.luma_dma_size = 32;
        Rval = iks_set_warp_buffer_info(pMode0->ContextId, &warp_buf_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_warp_buffer_info", p_desc_name, id_cnt++);
    }

    {
        // Stitch info
        ik_stitch_info_t stitch_info = {0};

        Rval = iks_set_stitching_info(pMode0->ContextId, &stitch_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_stitching_info", p_desc_name, id_cnt++);
    }

    {
        // Burst tile
        ik_burst_tile_t burst_tile = {0};

        Rval = iks_set_burst_tile(pMode0->ContextId, &burst_tile);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_set_burst_tile", p_desc_name, id_cnt++);
    }
}

static void IKS_Get_ImgFilters_NullPtr(const AMBA_IK_MODE_CFG_s *pMode0, const AMBA_IK_MODE_CFG_s *pMode1, char *p_desc_name)
{
    UINT32 Rval = IK_OK;

    {
        // Safety info
        Rval = iks_get_safety_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_safety_info", p_desc_name, id_cnt++);
    }

    {
        // Sensor info
        Rval = iks_get_vin_sensor_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_vin_sensor_info", p_desc_name, id_cnt++);
    }

    {
        // YUV mode
        Rval = iks_get_yuv_mode(pMode1->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_yuv_mode", p_desc_name, id_cnt++);
    }

    {
        // Before CE
        Rval = iks_get_before_ce_wb_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_before_ce_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // After CE
        Rval = iks_get_after_ce_wb_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_after_ce_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // CFA leakage
        Rval = iks_get_cfa_leakage_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_cfa_leakage_filter", p_desc_name, id_cnt++);
    }

    {
        // Anti aliasing
        Rval = iks_get_anti_aliasing(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_anti_aliasing", p_desc_name, id_cnt++);
    }

    {
        // DBP
        Rval = iks_get_dynamic_bad_pixel_corr(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_dynamic_bad_pixel_corr", p_desc_name, id_cnt++);
    }

    {
        // SBP
        Rval = iks_get_static_bad_pxl_corr_enb(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_static_bad_pxl_corr_enb", p_desc_name, id_cnt++);
        Rval = iks_get_static_bad_pxl_corr(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_static_bad_pxl_corr", p_desc_name, id_cnt++);
    }

    {
        // SBP internal
        Rval = iks_get_static_bad_pxl_corr_itnl(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_static_bad_pxl_corr_itnl", p_desc_name, id_cnt++);
    }

    {
        // CFA noise
        Rval = iks_get_cfa_noise_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_cfa_noise_filter", p_desc_name, id_cnt++);
    }

    {
        // Demosaic
        Rval = iks_get_demosaic(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_demosaic", p_desc_name, id_cnt++);
    }

    {
        // RGB_12Y
        Rval = iks_get_rgb_to_12y(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_rgb_to_12y", p_desc_name, id_cnt++);
    }

    {
        // LNL
        Rval = iks_get_luma_noise_reduction(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_luma_noise_reduction", p_desc_name, id_cnt++);
    }

    {
        // Pre CC gain
        Rval = iks_get_pre_cc_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_pre_cc_gain", p_desc_name, id_cnt++);
    }

    {
        // CC
        Rval = iks_get_color_correction(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_color_correction", p_desc_name, id_cnt++);
    }

    {
        // tone curve
        Rval = iks_get_tone_curve(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_tone_curve", p_desc_name, id_cnt++);
    }

    {
        // RGB to YUV
        Rval = iks_get_rgb_to_yuv_matrix(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_rgb_to_yuv_matrix", p_desc_name, id_cnt++);
    }

    {
        // RGBIR
        Rval = iks_get_rgb_ir(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_rgb_ir", p_desc_name, id_cnt++);
    }

    {
        // Chroma scale
        Rval = iks_get_chroma_scale(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_chroma_scale", p_desc_name, id_cnt++);
    }

    {
        // Chroma median
        Rval = iks_get_chroma_median_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_chroma_median_filter", p_desc_name, id_cnt++);
    }

    {
        // SHPA proc mode
        Rval = iks_get_fst_luma_process_mode(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_luma_process_mode", p_desc_name, id_cnt++);
    }

    {
        // ASF
        Rval = iks_get_adv_spatial_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_adv_spatial_filter", p_desc_name, id_cnt++);
    }

    {
        // SHPA
        Rval = iks_get_fst_shp_both(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_shp_both", p_desc_name, id_cnt++);

        Rval = iks_get_fst_shp_noise(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_shp_noise", p_desc_name, id_cnt++);

        Rval = iks_get_fst_shp_fir(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_shp_fir", p_desc_name, id_cnt++);

        Rval = iks_get_fst_shp_coring(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_shp_coring", p_desc_name, id_cnt++);

        Rval = iks_get_fst_shp_coring_idx_scale(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_shp_coring_idx_scale", p_desc_name, id_cnt++);

        Rval = iks_get_fst_shp_min_coring_rslt(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_shp_min_coring_rslt", p_desc_name, id_cnt++);

        Rval = iks_get_fst_shp_max_coring_rslt(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_shp_max_coring_rslt", p_desc_name, id_cnt++);

        Rval = iks_get_fst_shp_scale_coring(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_shp_scale_coring", p_desc_name, id_cnt++);
    }

    {
        // SHPB
        Rval = iks_get_fnl_shp_both(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_both", p_desc_name, id_cnt++);

        Rval = iks_get_fnl_shp_noise(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_noise", p_desc_name, id_cnt++);

        Rval = iks_get_fnl_shp_fir(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_fir", p_desc_name, id_cnt++);

        Rval = iks_get_fnl_shp_coring(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_coring", p_desc_name, id_cnt++);

        Rval = iks_get_fnl_shp_coring_idx_scale(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_coring_idx_scale", p_desc_name, id_cnt++);

        Rval = iks_get_fnl_shp_min_coring_rslt(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_min_coring_rslt", p_desc_name, id_cnt++);

        Rval = iks_get_fnl_shp_max_coring_rslt(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_max_coring_rslt", p_desc_name, id_cnt++);

        Rval = iks_get_fnl_shp_scale_coring(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_scale_coring", p_desc_name, id_cnt++);

        Rval = iks_get_fnl_shp_three_d_table(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_three_d_table", p_desc_name, id_cnt++);
    }

    {
        // Chroma filter
        Rval = iks_get_chroma_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_chroma_filter", p_desc_name, id_cnt++);
    }

    {
        // Wide chroma filter
        Rval = iks_get_wide_chroma_filter(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_wide_chroma_filter", p_desc_name, id_cnt++);
    }

    {
        // Wide chroma filter combine
        Rval = iks_get_wide_chroma_filter_combine(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_wide_chroma_filter_combine", p_desc_name, id_cnt++);
    }

    {
        // grgb mismatch
        Rval = iks_get_grgb_mismatch(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_grgb_mismatch", p_desc_name, id_cnt++);
    }

    {
        // MCTF
        Rval = iks_get_video_mctf(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_video_mctf", p_desc_name, id_cnt++);

        Rval = iks_get_video_mctf_ta(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_video_mctf_ta", p_desc_name, id_cnt++);

        Rval = iks_get_video_mctf_and_fnl_shp(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_video_mctf_and_fnl_shp", p_desc_name, id_cnt++);
    }

    {
        // Vig
        Rval = iks_get_vignette_enb(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_vignette_enb", p_desc_name, id_cnt++);
        Rval = iks_get_vignette(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_vignette", p_desc_name, id_cnt++);
    }

    {
        // HDR blc
        Rval = iks_get_exp0_frontend_static_blc(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_exp0_frontend_static_blc", p_desc_name, id_cnt++);
        Rval = iks_get_exp1_frontend_static_blc(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_exp1_frontend_static_blc", p_desc_name, id_cnt++);
        Rval = iks_get_exp2_frontend_static_blc(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_exp2_frontend_static_blc", p_desc_name, id_cnt++);
    }

    {
        // HDR gain
        Rval = iks_get_exp0_frontend_wb_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_exp0_frontend_wb_gain", p_desc_name, id_cnt++);
        Rval = iks_get_exp1_frontend_wb_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_exp1_frontend_wb_gain", p_desc_name, id_cnt++);
        Rval = iks_get_exp2_frontend_wb_gain(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_exp2_frontend_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // CE
        Rval = iks_get_ce(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_ce", p_desc_name, id_cnt++);
        Rval = iks_get_ce_input_table(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_ce_input_table", p_desc_name, id_cnt++);
        Rval = iks_get_ce_out_table(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_ce_out_table", p_desc_name, id_cnt++);
    }

    {
        // HDR blend
        Rval = iks_get_hdr_blend(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_hdr_blend", p_desc_name, id_cnt++);
    }

    {
        // HDR tone curve
        Rval = iks_get_frontend_tone_curve(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_frontend_tone_curve", p_desc_name, id_cnt++);
    }

    {
        // resampler strength
        Rval = iks_get_resampler_strength(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_resampler_strength", p_desc_name, id_cnt++);
    }

    {
        // AAA
        Rval = iks_get_aaa_stat_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_aaa_stat_info", p_desc_name, id_cnt++);
        Rval = iks_get_aaa_pg_af_stat_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_aaa_pg_af_stat_info", p_desc_name, id_cnt++);
        Rval = iks_get_af_stat_ex_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_af_stat_ex_info", p_desc_name, id_cnt++);
        Rval = iks_get_pg_af_stat_ex_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_pg_af_stat_ex_info", p_desc_name, id_cnt++);
        Rval = iks_get_histogram_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_histogram_info", p_desc_name, id_cnt++);
        Rval = iks_get_pg_histogram_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_pg_histogram_info", p_desc_name, id_cnt++);
    }

    {
        // Window info
        Rval = iks_get_window_size_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_window_size_info", p_desc_name, id_cnt++);
    }

    {
        // Warp
        Rval = iks_get_warp_enb(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_warp_enb", p_desc_name, id_cnt++);

        Rval = iks_get_warp_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_warp_info", p_desc_name, id_cnt++);
    }

    {
        // CA
        Rval = iks_get_cawarp_enb(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_cawarp_enb", p_desc_name, id_cnt++);

        Rval = iks_get_cawarp_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_cawarp_info", p_desc_name, id_cnt++);
    }

    {
        // DZoom
        Rval = iks_get_dzoom_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_dzoom_info", p_desc_name, id_cnt++);
    }

    {
        // Dummy
        Rval = iks_get_dummy_margin_range(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_dummy_margin_range", p_desc_name, id_cnt++);
    }

    {
        // Active win
        Rval = iks_get_vin_active_win(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_vin_active_win", p_desc_name, id_cnt++);
    }

    {
        // Warp internal
        Rval = iks_get_warp_internal(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_warp_internal", p_desc_name, id_cnt++);
    }

    {
        // CA internal
        Rval = iks_get_cawarp_internal(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_cawarp_internal", p_desc_name, id_cnt++);
    }

    {
        // HDR RAW info
        Rval = iks_get_hdr_raw_offset(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_hdr_raw_offset", p_desc_name, id_cnt++);
    }

    {
        // flip mode
        Rval = iks_get_flip_mode(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_flip_mode", p_desc_name, id_cnt++);
    }

    {
        // Warp buf info
        Rval = iks_get_warp_buffer_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_warp_buffer_info", p_desc_name, id_cnt++);
    }

    {
        // Stitch info
        Rval = iks_get_stitching_info(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_stitching_info", p_desc_name, id_cnt++);
    }

    {
        // Burst tile
        Rval = iks_get_burst_tile(pMode0->ContextId, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_burst_tile", p_desc_name, id_cnt++);
    }
}

static void IKS_Get_ImgFilters(const AMBA_IK_MODE_CFG_s *pMode0, const AMBA_IK_MODE_CFG_s *pMode1, char *p_desc_name)
{
    UINT32 Rval = IK_OK;

    {
        // Safety info
        ik_safety_info_t safety_info = {0};

        Rval = iks_get_safety_info(pMode0->ContextId, &safety_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_safety_info", p_desc_name, id_cnt++);
    }

    {
        // Sensor info
        ik_vin_sensor_info_t sensor_info = {0};

        Rval = iks_get_vin_sensor_info(pMode0->ContextId, &sensor_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_vin_sensor_info", p_desc_name, id_cnt++);
    }

    {
        // YUV mode
        uint32 yuv_mode = 0;

        Rval = iks_get_yuv_mode(pMode1->ContextId, &yuv_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_yuv_mode", p_desc_name, id_cnt++);
    }

    {
        // Before CE
        ik_wb_gain_t before_ce_wb_gain = {0};

        Rval = iks_get_before_ce_wb_gain(pMode0->ContextId, &before_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_before_ce_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // After CE
        ik_wb_gain_t after_ce_wb_gain = {0};

        Rval = iks_get_after_ce_wb_gain(pMode0->ContextId, &after_ce_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_after_ce_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // CFA leakage
        ik_cfa_leakage_filter_t cfa_leakage_filter = {0};

        Rval = iks_get_cfa_leakage_filter(pMode0->ContextId, &cfa_leakage_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_cfa_leakage_filter", p_desc_name, id_cnt++);
    }

    {
        // Anti aliasing
        ik_anti_aliasing_t anti_aliasing = {0};

        Rval = iks_get_anti_aliasing(pMode0->ContextId, &anti_aliasing);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_anti_aliasing", p_desc_name, id_cnt++);
    }

    {
        // DBP
        ik_dynamic_bad_pixel_correction_t dynamic_bpc = {0};

        Rval = iks_get_dynamic_bad_pixel_corr(pMode0->ContextId, &dynamic_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_dynamic_bad_pixel_corr", p_desc_name, id_cnt++);
    }

    {
        // SBP
        uint32 enb = 0;
        ik_static_bad_pxl_cor_t static_bpc = {0};

        Rval = iks_get_static_bad_pxl_corr_enb(pMode0->ContextId, &enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_static_bad_pxl_corr_enb", p_desc_name, id_cnt++);
        Rval = iks_get_static_bad_pxl_corr(pMode0->ContextId, &static_bpc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_static_bad_pxl_corr", p_desc_name, id_cnt++);
    }

    {
        // SBP internal
        ik_static_bad_pixel_correction_internal_t static_bpc_internal = {0};

        Rval = iks_get_static_bad_pxl_corr_itnl(pMode0->ContextId, &static_bpc_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_static_bad_pxl_corr_itnl", p_desc_name, id_cnt++);
    }

    {
        // CFA noise
        ik_cfa_noise_filter_t cfa_noise_filter = {0};

        Rval = iks_get_cfa_noise_filter(pMode0->ContextId, &cfa_noise_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_cfa_noise_filter", p_desc_name, id_cnt++);
    }

    {
        // Demosaic
        ik_demosaic_t demosaic = {0};

        Rval = iks_get_demosaic(pMode0->ContextId, &demosaic);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_demosaic", p_desc_name, id_cnt++);
    }

    {
        // RGB_12Y
        ik_rgb_to_12y_t rgb_to_12y = {0};

        Rval = iks_get_rgb_to_12y(pMode0->ContextId, &rgb_to_12y);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_rgb_to_12y", p_desc_name, id_cnt++);
    }

    {
        // LNL
        ik_luma_noise_reduction_t luma_noise_reduce = {0};

        Rval = iks_get_luma_noise_reduction(pMode0->ContextId, &luma_noise_reduce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_luma_noise_reduction", p_desc_name, id_cnt++);
    }

    {
        // Pre CC gain
        ik_pre_cc_gain_t pre_cc_gain = {0};

        Rval = iks_get_pre_cc_gain(pMode0->ContextId, &pre_cc_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_pre_cc_gain", p_desc_name, id_cnt++);
    }

    {
        // CC
        ik_color_correction_t color_correction = {0};

        Rval = iks_get_color_correction(pMode0->ContextId, &color_correction);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_color_correction", p_desc_name, id_cnt++);
    }

    {
        // tone curve
        ik_tone_curve_t tone_curve = {0};

        Rval = iks_get_tone_curve(pMode0->ContextId, &tone_curve);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_tone_curve", p_desc_name, id_cnt++);
    }

    {
        // RGB to YUV
        ik_rgb_to_yuv_matrix_t rgb_to_yuv_matrix = {0};

        Rval = iks_get_rgb_to_yuv_matrix(pMode0->ContextId, &rgb_to_yuv_matrix);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_rgb_to_yuv_matrix", p_desc_name, id_cnt++);
    }

    {
        // RGBIR
        ik_rgb_ir_t rgb_ir = {0};

        Rval = iks_get_rgb_ir(pMode0->ContextId, &rgb_ir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_rgb_ir", p_desc_name, id_cnt++);
    }

    {
        // Chroma scale
        ik_chroma_scale_t chroma_scale = {0};

        Rval = iks_get_chroma_scale(pMode0->ContextId, &chroma_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_chroma_scale", p_desc_name, id_cnt++);
    }

    {
        // Chroma median
        ik_chroma_median_filter_t chroma_median_filter = {0};

        Rval = iks_get_chroma_median_filter(pMode0->ContextId, &chroma_median_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_chroma_median_filter", p_desc_name, id_cnt++);
    }

    {
        // SHPA proc mode
        ik_first_luma_process_mode_t first_luma_process_mode = {0};

        Rval = iks_get_fst_luma_process_mode(pMode0->ContextId, &first_luma_process_mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_luma_process_mode", p_desc_name, id_cnt++);
    }

    {
        // ASF
        ik_adv_spatial_filter_t advance_spatial_filter = {0};

        Rval = iks_get_adv_spatial_filter(pMode0->ContextId, &advance_spatial_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_adv_spatial_filter", p_desc_name, id_cnt++);
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

        Rval = iks_get_fst_shp_both(pMode0->ContextId, &first_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_shp_both", p_desc_name, id_cnt++);

        Rval = iks_get_fst_shp_noise(pMode0->ContextId, &first_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_shp_noise", p_desc_name, id_cnt++);

        Rval = iks_get_fst_shp_fir(pMode0->ContextId, &first_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_shp_fir", p_desc_name, id_cnt++);

        Rval = iks_get_fst_shp_coring(pMode0->ContextId, &first_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_shp_coring", p_desc_name, id_cnt++);

        Rval = iks_get_fst_shp_coring_idx_scale(pMode0->ContextId, &first_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_shp_coring_idx_scale", p_desc_name, id_cnt++);

        Rval = iks_get_fst_shp_min_coring_rslt(pMode0->ContextId, &first_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_shp_min_coring_rslt", p_desc_name, id_cnt++);

        Rval = iks_get_fst_shp_max_coring_rslt(pMode0->ContextId, &first_sharpen_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_shp_max_coring_rslt", p_desc_name, id_cnt++);

        Rval = iks_get_fst_shp_scale_coring(pMode0->ContextId, &first_sharpen_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fst_shp_scale_coring", p_desc_name, id_cnt++);
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

        Rval = iks_get_fnl_shp_both(pMode0->ContextId, &final_sharpen_both);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_both", p_desc_name, id_cnt++);

        Rval = iks_get_fnl_shp_noise(pMode0->ContextId, &final_sharpen_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_noise", p_desc_name, id_cnt++);

        Rval = iks_get_fnl_shp_fir(pMode0->ContextId, &final_sharpen_fir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_fir", p_desc_name, id_cnt++);

        Rval = iks_get_fnl_shp_coring(pMode0->ContextId, &final_sharpen_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_coring", p_desc_name, id_cnt++);

        Rval = iks_get_fnl_shp_coring_idx_scale(pMode0->ContextId, &final_sharpen_coring_idx_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_coring_idx_scale", p_desc_name, id_cnt++);

        Rval = iks_get_fnl_shp_min_coring_rslt(pMode0->ContextId, &final_sharpen_min_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_min_coring_rslt", p_desc_name, id_cnt++);

        Rval = iks_get_fnl_shp_max_coring_rslt(pMode0->ContextId, &final_sharpen_max_coring_result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_max_coring_rslt", p_desc_name, id_cnt++);

        Rval = iks_get_fnl_shp_scale_coring(pMode0->ContextId, &final_sharpen_scale_coring);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_scale_coring", p_desc_name, id_cnt++);

        Rval = iks_get_fnl_shp_three_d_table(pMode0->ContextId, &final_sharpen_both_three_d_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_fnl_shp_three_d_table", p_desc_name, id_cnt++);
    }

    {
        // Chroma filter
        ik_chroma_filter_t chroma_filter = {0};

        Rval = iks_get_chroma_filter(pMode0->ContextId, &chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_chroma_filter", p_desc_name, id_cnt++);
    }

    {
        // Wide chroma filter
        ik_wide_chroma_filter_t wide_chroma_filter = {0};

        Rval = iks_get_wide_chroma_filter(pMode0->ContextId, &wide_chroma_filter);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_wide_chroma_filter", p_desc_name, id_cnt++);
    }

    {
        // Wide chroma filter combine
        ik_wide_chroma_filter_combine_t chroma_filter_combine = {0};

        Rval = iks_get_wide_chroma_filter_combine(pMode0->ContextId, &chroma_filter_combine);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_wide_chroma_filter_combine", p_desc_name, id_cnt++);
    }

    {
        // grgb mismatch
        ik_grgb_mismatch_t grgb_mismatch = {0};

        Rval = iks_get_grgb_mismatch(pMode0->ContextId, &grgb_mismatch);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_grgb_mismatch", p_desc_name, id_cnt++);
    }

    {
        // MCTF
        ik_video_mctf_t video_mctf = {0};
        ik_video_mctf_ta_t video_mctf_ta = {0};
        ik_pos_dep33_t video_mctf_and_final_sharpen = {0};

        Rval = iks_get_video_mctf(pMode0->ContextId, &video_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_video_mctf", p_desc_name, id_cnt++);

        Rval = iks_get_video_mctf_ta(pMode0->ContextId, &video_mctf_ta);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_video_mctf_ta", p_desc_name, id_cnt++);

        Rval = iks_get_video_mctf_and_fnl_shp(pMode0->ContextId, &video_mctf_and_final_sharpen);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_video_mctf_and_fnl_shp", p_desc_name, id_cnt++);
    }

    {
        // Vig
        uint32 enb = 0;
        ik_vignette_t vignette = {0};

        Rval = iks_get_vignette_enb(pMode0->ContextId, &enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_vignette_enb", p_desc_name, id_cnt++);
        Rval = iks_get_vignette(pMode0->ContextId, &vignette);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_vignette", p_desc_name, id_cnt++);
    }

    {
        // HDR blc
        ik_static_blc_level_t frontend_static_blc = {0};

        Rval = iks_get_exp0_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_exp0_frontend_static_blc", p_desc_name, id_cnt++);
        Rval = iks_get_exp1_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_exp1_frontend_static_blc", p_desc_name, id_cnt++);
        Rval = iks_get_exp2_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_exp2_frontend_static_blc", p_desc_name, id_cnt++);
    }

    {
        // HDR gain
        ik_frontend_wb_gain_t frontend_wb_gain = {0};

        Rval = iks_get_exp0_frontend_wb_gain(pMode0->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_exp0_frontend_wb_gain", p_desc_name, id_cnt++);
        Rval = iks_get_exp1_frontend_wb_gain(pMode0->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_exp1_frontend_wb_gain", p_desc_name, id_cnt++);
        Rval = iks_get_exp2_frontend_wb_gain(pMode0->ContextId, &frontend_wb_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_exp2_frontend_wb_gain", p_desc_name, id_cnt++);
    }

    {
        // CE
        ik_ce_t ce = {0};
        ik_ce_input_table_t ce_input_table = {0};
        ik_ce_output_table_t ce_out_table = {0};

        Rval = iks_get_ce(pMode0->ContextId, &ce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_ce", p_desc_name, id_cnt++);
        Rval = iks_get_ce_input_table(pMode0->ContextId, &ce_input_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_ce_input_table", p_desc_name, id_cnt++);
        Rval = iks_get_ce_out_table(pMode0->ContextId, &ce_out_table);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_ce_out_table", p_desc_name, id_cnt++);
    }

    {
        // HDR blend
        ik_hdr_blend_t hdr_blend = {0};

        Rval = iks_get_hdr_blend(pMode0->ContextId, &hdr_blend);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_hdr_blend", p_desc_name, id_cnt++);
    }

    {
        // HDR tone curve
        ik_frontend_tone_curve_t fe_tc = {0};

        Rval = iks_get_frontend_tone_curve(pMode0->ContextId, &fe_tc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_frontend_tone_curve", p_desc_name, id_cnt++);
    }

    {
        // resampler strength
        ik_resampler_strength_t resampler_strength = {0};

        Rval = iks_get_resampler_strength(pMode0->ContextId, &resampler_strength);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_resampler_strength", p_desc_name, id_cnt++);
    }

    {
        // AAA
        ik_aaa_stat_info_t stat_info = {0};
        ik_aaa_pg_af_stat_info_t pg_af_stat_info = {0};
        ik_af_stat_ex_info_t af_stat_ex_info = {0};
        ik_pg_af_stat_ex_info_t pg_af_stat_ex_info = {0};
        ik_histogram_info_t hist_info = {0};

        Rval = iks_get_aaa_stat_info(pMode0->ContextId, &stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_aaa_stat_info", p_desc_name, id_cnt++);
        Rval = iks_get_aaa_pg_af_stat_info(pMode0->ContextId, &pg_af_stat_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_aaa_pg_af_stat_info", p_desc_name, id_cnt++);
        Rval = iks_get_af_stat_ex_info(pMode0->ContextId, &af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_af_stat_ex_info", p_desc_name, id_cnt++);
        Rval = iks_get_pg_af_stat_ex_info(pMode0->ContextId, &pg_af_stat_ex_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_pg_af_stat_ex_info", p_desc_name, id_cnt++);
        Rval = iks_get_histogram_info(pMode0->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_histogram_info", p_desc_name, id_cnt++);
        Rval = iks_get_pg_histogram_info(pMode0->ContextId, &hist_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_pg_histogram_info", p_desc_name, id_cnt++);
    }

    {
        // Window info
        ik_window_size_info_t window_size_info = {0};

        Rval = iks_get_window_size_info(pMode0->ContextId, &window_size_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_window_size_info", p_desc_name, id_cnt++);
    }

    {
        // Warp
        uint32 enb = 0;
        ik_warp_info_t calib_warp_info = {0};

        Rval = iks_get_warp_enb(pMode0->ContextId, &enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_warp_enb", p_desc_name, id_cnt++);

        Rval = iks_get_warp_info(pMode0->ContextId, &calib_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_warp_info", p_desc_name, id_cnt++);
    }

    {
        // CA
        uint32 enb = 0;
        ik_cawarp_info_t calib_ca_warp_info = {0};

        Rval = iks_get_cawarp_enb(pMode0->ContextId, &enb);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_cawarp_enb", p_desc_name, id_cnt++);

        Rval = iks_get_cawarp_info(pMode0->ContextId, &calib_ca_warp_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_cawarp_info", p_desc_name, id_cnt++);
    }

    {
        // DZoom
        ik_dzoom_info_t dzoom_info = {0};

        Rval = iks_get_dzoom_info(pMode0->ContextId, &dzoom_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_dzoom_info", p_desc_name, id_cnt++);
    }

    {
        // Dummy
        ik_dummy_margin_range_t dmy_margin_range = {0};

        Rval = iks_get_dummy_margin_range(pMode0->ContextId, &dmy_margin_range);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_dummy_margin_range", p_desc_name, id_cnt++);
    }

    {
        // Active win
        ik_vin_active_window_t vin_active_win = {0};

        Rval = iks_get_vin_active_win(pMode0->ContextId, &vin_active_win);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_vin_active_win", p_desc_name, id_cnt++);
    }

    {
        // Warp internal
        ik_warp_internal_info_t warp_dzoom_internal = {0};

        Rval = iks_get_warp_internal(pMode0->ContextId, &warp_dzoom_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_warp_internal", p_desc_name, id_cnt++);
    }

    {
        // CA internal
        ik_cawarp_internal_info_t cawarp_internal = {0};

        Rval = iks_get_cawarp_internal(pMode0->ContextId, &cawarp_internal);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_cawarp_internal", p_desc_name, id_cnt++);
    }

    {
        // HDR RAW info
        ik_hdr_raw_info_t hdr_raw_info = {0};

        Rval = iks_get_hdr_raw_offset(pMode0->ContextId, &hdr_raw_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_hdr_raw_offset", p_desc_name, id_cnt++);
    }

    {
        // flip mode
        uint32 mode = 0;

        Rval = iks_get_flip_mode(pMode0->ContextId, &mode);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_flip_mode", p_desc_name, id_cnt++);
    }

    {
        // Warp buf info
        ik_warp_buffer_info_t warp_buf_info = {0};

        Rval = iks_get_warp_buffer_info(pMode0->ContextId, &warp_buf_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_warp_buffer_info", p_desc_name, id_cnt++);
    }

    {
        // Stitch info
        ik_stitch_info_t stitch_info = {0};

        Rval = iks_get_stitching_info(pMode0->ContextId, &stitch_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_stitching_info", p_desc_name, id_cnt++);
    }

    {
        // Burst tile
        ik_burst_tile_t burst_tile = {0};

        Rval = iks_get_burst_tile(pMode0->ContextId, &burst_tile);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgFilters.c", "iks_get_burst_tile", p_desc_name, id_cnt++);
    }
}

static void IKS_TestCovr_ImgFilters(AMBA_IK_MODE_CFG_s *pMode0, AMBA_IK_MODE_CFG_s *pMode1)
{
    amba_iks_context_entity_t *p_ctx0 = NULL;
    amba_iks_context_entity_t *p_ctx1 = NULL;

    iks_ctx_get_context(pMode0->ContextId, &p_ctx0);
    iks_ctx_get_context(pMode1->ContextId, &p_ctx1);

    pMode0->ContextId = 33;
    pMode1->ContextId = 33;
    IKS_Set_ImgFilters(pMode0, pMode1, "test get ctx fail case");
    pMode0->ContextId = 0;
    pMode1->ContextId = 1;

    p_ctx0->organization.initial_flag = 0u;
    p_ctx1->organization.initial_flag = 0u;
    IKS_Set_ImgFilters(pMode0, pMode1, "test ctx not init fail case");
    p_ctx0->organization.initial_flag = 1u;
    p_ctx1->organization.initial_flag = 1u;

    IKS_Set_ImgFilters_NullPtr(pMode0, "test NULL ptr fail case");

    IKS_Get_ImgFilters(pMode0, pMode1, "test get filters OK case");

    pMode0->ContextId = 33;
    pMode1->ContextId = 33;
    IKS_Get_ImgFilters(pMode0, pMode1, "test get ctx fail case");
    pMode0->ContextId = 0;
    pMode1->ContextId = 1;

    p_ctx0->organization.initial_flag = 0u;
    p_ctx1->organization.initial_flag = 0u;
    IKS_Get_ImgFilters(pMode0, pMode1, "test ctx not init fail case");
    p_ctx0->organization.initial_flag = 1u;
    p_ctx1->organization.initial_flag = 1u;

    IKS_Get_ImgFilters_NullPtr(pMode0, pMode1, "test NULL ptr fail case");
}

static void IKS_TestCovr_ImgExecuter(AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_MODE_CFG_s *pMode1)
{
    UINT32 Rval = IK_OK;
    ik_execute_container_t execute_container = {0};
    amba_iks_context_entity_t *p_ctx = NULL;
    ik_buffer_info_t *p_iks_buffer_info = NULL;

    iks_ctx_get_context(pMode->ContextId, &p_ctx);

    Rval = iks_execute(pMode->ContextId, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuter.c", "iks_execute", "test NULL ptr fail case", id_cnt++);

    Rval = iks_execute(33, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuter.c", "iks_execute", "test get ctx fail case", id_cnt++);

    p_ctx->organization.initial_flag = 0u;
    Rval = iks_execute(pMode->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuter.c", "iks_execute", "test ctx not init fail case", id_cnt++);
    p_ctx->organization.initial_flag = 1u;

    IKS_Set_ImgFilters(pMode, pMode1, "OK case to setup iks_execute");

    p_ctx->organization.active_cr_state.cr_running_number = p_ctx->organization.active_cr_state.max_running_number;
    Rval = iks_execute(pMode->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuter.c", "iks_execute", "test cr_running_number = max_running_number OK case", id_cnt++);
    p_ctx->organization.active_cr_state.cr_running_number = 1u;

    p_ctx->filters.input_param.safety_enable = 0u;
    Rval = iks_execute(pMode->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuter.c", "iks_execute", "test safety disable OK case", id_cnt++);
    p_ctx->filters.input_param.safety_enable = 1u;

    p_ctx->filters.input_param.safety_info.update_freq = 2u;
    Rval = iks_execute(pMode->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuter.c", "iks_execute", "test update_freq == 2 OK case", id_cnt++);
    p_ctx->filters.input_param.safety_info.update_freq = 1u;

    p_ctx->organization.active_cr_state.cr_running_number = 3u;
    p_ctx->filters.input_param.safety_info.update_freq = 2u;
    Rval = iks_execute(pMode->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuter.c", "iks_execute", "test ((((uint32)p_ctx->organization.active_cr_state.cr_running_number - 1u) % p_ctx->filters.input_param.safety_info.update_freq) == 0u) false case", id_cnt++);
    p_ctx->filters.input_param.safety_info.update_freq = 1u;
    p_ctx->organization.active_cr_state.cr_running_number = 1u;

    iks_arch_get_ik_working_buffer(&p_iks_buffer_info);
    p_iks_buffer_info->safety_crc_enable = 0u;
    Rval = iks_execute(pMode->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuter.c", "iks_execute", "test safety_crc_enable disable OK case", id_cnt++);
    p_iks_buffer_info->safety_crc_enable = 1u;

    Rval = iks_calc_geo_settings(NULL, NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuter.c", "iks_calc_geo_settings", "test NULL ptr fail case", id_cnt++);
}

static void IKS_TestCovr_ImgInit(AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;
    ik_context_setting_t setting_of_each_context = {0};
    iks_check_info_t info = {0};
    ik_ability_t ability = {0};
    size_t iks_ctx_size = 0;
    size_t iks_cr_size = 0;
    ik_buffer_info_t *p_iks_buffer_info = NULL;
    UINT32 iks_minor_ver = 0u;

    ability.pipe = AMBA_IK_PIPE_VIDEO;
    ability.video_pipe = AMBA_IK_VIDEO_HDR_EXPO_3;
    setting_of_each_context.cr_ring_setting[0].p_ability = &ability;

    Rval = iks_init_arch(NULL, (void*)ik_working_buffer,
                         sizeof(ik_working_buffer), AMBA_IK_ARCH_HARD_INIT, &info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "amba_iks_arch_init_check", "test NULL ptr fail case", id_cnt++);

    setting_of_each_context.context_number = 33;
    Rval = iks_init_arch(&setting_of_each_context, (void*)ik_working_buffer,
                         sizeof(ik_working_buffer), AMBA_IK_ARCH_HARD_INIT, &info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "amba_iks_arch_init_check", "test (p_setting_of_each_context->context_number > MAX_CONTEXT_NUM) fail case", id_cnt++);

    setting_of_each_context.context_number = 1;
    setting_of_each_context.cr_ring_setting[0].cr_ring_number = 33;
    Rval = iks_init_arch(&setting_of_each_context, (void*)ik_working_buffer,
                         sizeof(ik_working_buffer), AMBA_IK_ARCH_HARD_INIT, &info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "amba_iks_arch_init_check", "test (p_setting_of_each_context->cr_ring_setting[ctx].cr_ring_number > MAX_CR_RING_NUM) fail case", id_cnt++);

    setting_of_each_context.cr_ring_setting[0].cr_ring_number = 0;
    Rval = iks_init_arch(&setting_of_each_context, (void*)ik_working_buffer,
                         sizeof(ik_working_buffer), AMBA_IK_ARCH_HARD_INIT, &info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "amba_iks_arch_init_check", "test (p_setting_of_each_context->cr_ring_setting[ctx].cr_ring_number <= 0u) fail case", id_cnt++);
    setting_of_each_context.cr_ring_setting[0].cr_ring_number = 3;

    info.iks_major_num = 1;
    Rval = iks_init_arch(&setting_of_each_context, (void*)ik_working_buffer,
                         sizeof(ik_working_buffer), AMBA_IK_ARCH_HARD_INIT, &info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "iks_check_version", "test (iks_major_num != IKS_VERSION_MAJOR) false case", id_cnt++);

    Rval = iks_query_arch_memory_size(NULL, &iks_ctx_size, &iks_cr_size);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "iks_query_arch_memory_size", "test (rval == IK_OK) false case", id_cnt++);

    Rval = iks_query_arch_memory_size(&setting_of_each_context, NULL, &iks_cr_size);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "iks_query_arch_memory_size", "test NULL ptr fail case", id_cnt++);
    Rval = iks_query_arch_memory_size(&setting_of_each_context, &iks_ctx_size, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "iks_query_arch_memory_size", "test NULL ptr fail case", id_cnt++);

    Rval = iks_init_arch(&setting_of_each_context, (void*)(ik_working_buffer + 1u),
                         sizeof(ik_working_buffer), AMBA_IK_ARCH_HARD_INIT, &info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "iks_init_arch", "test ((usable_mem_addr & 0x1FU) > 0U) fail case", id_cnt++);

    Rval = iks_init_arch(&setting_of_each_context, (void*)ik_working_buffer,
                         1024, AMBA_IK_ARCH_HARD_INIT, &info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "iks_init_arch", "test ((arch_memory + cr_memory) > mem_size) fail case", id_cnt++);

    iks_init_arch(&setting_of_each_context, (void*)ik_working_buffer,
                  sizeof(ik_working_buffer), AMBA_IK_ARCH_HARD_INIT, &info);
    Rval = iks_init_arch(&setting_of_each_context, (void*)ik_working_buffer,
                         sizeof(ik_working_buffer), AMBA_IK_ARCH_SOFT_INIT, &info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "iks_init_arch", "test (ik_init_mode == AMBA_IK_ARCH_HARD_INIT) false case", id_cnt++);

    iks_arch_deinit_architecture();
    Rval = iks_init_default_binary(NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "iks_init_default_binary", "test (rval == IK_OK) false case", id_cnt++);

    iks_init_arch(&setting_of_each_context, (void*)ik_working_buffer,
                  sizeof(ik_working_buffer), AMBA_IK_ARCH_HARD_INIT, &info);
    iks_arch_get_ik_working_buffer(&p_iks_buffer_info);
    p_iks_buffer_info->init_flag = 0u;
    Rval = iks_init_default_binary(NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "iks_init_default_binary", "test (p_ik_buffer_info->init_flag == 1u) false case", id_cnt++);
    p_iks_buffer_info->init_flag = 1u;

    Rval = iks_init_context(33, &ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "iks_init_context", "test get ctx fail case", id_cnt++);

    p_iks_buffer_info->init_mode = AMBA_IK_ARCH_SOFT_INIT;
    Rval = iks_init_context(pMode->ContextId, &ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "iks_init_context", "test (p_ik_buffer_info->init_mode == AMBA_IK_ARCH_HARD_INIT) false case", id_cnt++);
    p_iks_buffer_info->init_mode = AMBA_IK_ARCH_HARD_INIT;

    Rval = iks_get_context_ability(pMode->ContextId, &ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "iks_get_context_ability", "test OK case", id_cnt++);

    Rval = iks_get_minor_version(&iks_minor_ver);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "iks_get_minor_version", "test OK case", id_cnt++);

    Rval = iks_get_minor_version(NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgInit.c", "iks_get_minor_version", "test NULL ptr fail case", id_cnt++);
}

INT32 iks_ctest_main(void)
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

    pFile = fopen("data/iks_ctest.txt", "w");

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
    IKS_TestCovr_ImgFilters(&Mode[0], &Mode[1]);
    // test AmbaDSP_ImgExecuter.c
    IKS_TestCovr_ImgExecuter(&Mode[0], &Mode[1]);
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgInit.c
    IKS_TestCovr_ImgInit(&Mode[0]);
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

