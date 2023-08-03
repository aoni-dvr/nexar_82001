#include "test_ik_global.h"


#define IK_AMALGAM_TABLE_SBP_SIZE 3686400UL

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
    AMBA_IK_ABILITY_s Ability[4];
    AMBA_IK_MODE_CFG_s Mode[4];

    img_arch_deinit_architecture();
    iks_arch_deinit_architecture();

    memset(ik_working_buffer, 0, sizeof(ik_working_buffer));

    memcpy(Ability, pAbility, sizeof(AMBA_IK_ABILITY_s)*4);

    ContextSetting.ContextNumber = 4;
    ContextSetting.ConfigSetting[0].ConfigNumber = 5;
    ContextSetting.ConfigSetting[0].pAbility = &Ability[0];
    ContextSetting.ConfigSetting[1].ConfigNumber = 5;
    ContextSetting.ConfigSetting[1].pAbility = &Ability[1];
    ContextSetting.ConfigSetting[2].ConfigNumber = 1;
    ContextSetting.ConfigSetting[2].pAbility = &Ability[2];
    ContextSetting.ConfigSetting[3].ConfigNumber = 1;
    ContextSetting.ConfigSetting[3].pAbility = &Ability[3];

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

    Mode[3].ContextId = 3;
    Rval |= AmbaIK_InitContext(&Mode[3], &Ability[3]);
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

    ContextSetting.ContextNumber = 4;
    ContextSetting.ConfigSetting[0].ConfigNumber = 5;
    ContextSetting.ConfigSetting[0].pAbility = p_ability;
    ContextSetting.ConfigSetting[1].ConfigNumber = 5;
    ContextSetting.ConfigSetting[1].pAbility = ++p_ability;
    ContextSetting.ConfigSetting[2].ConfigNumber = 1;
    ContextSetting.ConfigSetting[2].pAbility = ++p_ability;
    ContextSetting.ConfigSetting[3].ConfigNumber = 1;
    ContextSetting.ConfigSetting[3].pAbility = ++p_ability;

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

static void IKS_Executer_Set_ImgFilters(const AMBA_IK_MODE_CFG_s *pMode0)
{
    {
        // Safety info
        ik_safety_info_t safety_info = {0};

        safety_info.update_freq = 1;
        iks_set_safety_info(pMode0->ContextId, &safety_info);
    }

    {
        // Sensor info
        ik_vin_sensor_info_t sensor_info = {0};

        iks_set_vin_sensor_info(pMode0->ContextId, &sensor_info);
    }

    {
        // Before CE
        ik_wb_gain_t before_ce_wb_gain = {0};

        before_ce_wb_gain.gain_b = 4096;
        before_ce_wb_gain.gain_g = 4096;
        before_ce_wb_gain.gain_r = 4096;
        iks_set_before_ce_wb_gain(pMode0->ContextId, &before_ce_wb_gain);
    }

    {
        // After CE
        ik_wb_gain_t after_ce_wb_gain = {0};

        after_ce_wb_gain.gain_b = 4096;
        after_ce_wb_gain.gain_g = 4096;
        after_ce_wb_gain.gain_r = 4096;
        iks_set_after_ce_wb_gain(pMode0->ContextId, &after_ce_wb_gain);
    }

    {
        // CFA leakage
        ik_cfa_leakage_filter_t cfa_leakage_filter = {0};

        iks_set_cfa_leakage_filter(pMode0->ContextId, &cfa_leakage_filter);
    }

    {
        // Anti aliasing
        ik_anti_aliasing_t anti_aliasing = {0};

        iks_set_anti_aliasing(pMode0->ContextId, &anti_aliasing);
    }

    {
        // DBP
        ik_dynamic_bad_pixel_correction_t dynamic_bpc = {0};

        iks_set_dynamic_bad_pixel_corr(pMode0->ContextId, &dynamic_bpc);
    }

    {
        // SBP
        uint32 enb = 0;
        ik_static_bad_pxl_cor_t static_bpc = {0};

        iks_set_static_bad_pxl_corr_enb(pMode0->ContextId, enb);

        static_bpc.calib_sbp_info.version = 0x20180401;
        static_bpc.calib_sbp_info.sbp_buffer = FPNMap;
        static_bpc.vin_sensor_geo.width = 1920;
        static_bpc.vin_sensor_geo.height = 1080;
        static_bpc.vin_sensor_geo.h_sub_sample.factor_den = 1;
        static_bpc.vin_sensor_geo.h_sub_sample.factor_num = 1;
        static_bpc.vin_sensor_geo.v_sub_sample.factor_den = 1;
        static_bpc.vin_sensor_geo.v_sub_sample.factor_num = 1;
        iks_set_static_bad_pxl_corr(pMode0->ContextId, &static_bpc);
    }

    {
        // SBP internal
        ik_static_bad_pixel_correction_internal_t static_bpc_internal = {0};

        static_bpc_internal.p_map = FPNMap;
        iks_set_static_bad_pxl_corr_itnl(pMode0->ContextId, &static_bpc_internal);
    }

    {
        // CFA noise
        ik_cfa_noise_filter_t cfa_noise_filter = {0};

        iks_set_cfa_noise_filter(pMode0->ContextId, &cfa_noise_filter);
    }

    {
        // Demosaic
        ik_demosaic_t demosaic = {0};

        iks_set_demosaic(pMode0->ContextId, &demosaic);
    }

    {
        // RGB_12Y
        ik_rgb_to_12y_t rgb_to_12y = {0};

        iks_set_rgb_to_12y(pMode0->ContextId, &rgb_to_12y);
    }

    {
        // LNL
        ik_luma_noise_reduction_t luma_noise_reduce = {0};

        iks_set_luma_noise_reduction(pMode0->ContextId, &luma_noise_reduce);
    }

    {
        // CC
        ik_color_correction_t color_correction = {0};

        iks_set_color_correction(pMode0->ContextId, &color_correction);
    }

    {
        // tone curve
        ik_tone_curve_t tone_curve = {0};

        iks_set_tone_curve(pMode0->ContextId, &tone_curve);
    }

    {
        // RGB to YUV
        ik_rgb_to_yuv_matrix_t rgb_to_yuv_matrix = {0};

        iks_set_rgb_to_yuv_matrix(pMode0->ContextId, &rgb_to_yuv_matrix);
    }

    {
        // RGBIR
        ik_rgb_ir_t rgb_ir = {0};

        iks_set_rgb_ir(pMode0->ContextId, &rgb_ir);
    }

    {
        // Chroma scale
        ik_chroma_scale_t chroma_scale = {0};

        iks_set_chroma_scale(pMode0->ContextId, &chroma_scale);
    }

    {
        // Chroma median
        ik_chroma_median_filter_t chroma_median_filter = {0};

        iks_set_chroma_median_filter(pMode0->ContextId, &chroma_median_filter);
    }

    {
        // SHPA proc mode
        ik_first_luma_process_mode_t first_luma_process_mode = {0};

        iks_set_fst_luma_process_mode(pMode0->ContextId, &first_luma_process_mode);
    }

    {
        // ASF
        ik_adv_spatial_filter_t advance_spatial_filter = {0};

        iks_set_adv_spatial_filter(pMode0->ContextId, &advance_spatial_filter);
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

        iks_set_fst_shp_both(pMode0->ContextId, &first_sharpen_both);

        first_sharpen_noise.level_str_adjust.high = 100;
        iks_set_fst_shp_noise(pMode0->ContextId, &first_sharpen_noise);

        iks_set_fst_shp_fir(pMode0->ContextId, &first_sharpen_fir);

        first_sharpen_coring.fractional_bits = 2;
        iks_set_fst_shp_coring(pMode0->ContextId, &first_sharpen_coring);

        first_sharpen_coring_idx_scale.high = 100;
        iks_set_fst_shp_coring_idx_scale(pMode0->ContextId, &first_sharpen_coring_idx_scale);

        first_sharpen_min_coring_result.high = 100;
        iks_set_fst_shp_min_coring_rslt(pMode0->ContextId, &first_sharpen_min_coring_result);

        first_sharpen_max_coring_result.high = 100;
        iks_set_fst_shp_max_coring_rslt(pMode0->ContextId, &first_sharpen_max_coring_result);

        first_sharpen_scale_coring.high = 100;
        iks_set_fst_shp_scale_coring(pMode0->ContextId, &first_sharpen_scale_coring);
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

        iks_set_fnl_shp_both(pMode0->ContextId, &final_sharpen_both);

        final_sharpen_noise.level_str_adjust.high = 100;
        iks_set_fnl_shp_noise(pMode0->ContextId, &final_sharpen_noise);

        iks_set_fnl_shp_fir(pMode0->ContextId, &final_sharpen_fir);

        final_sharpen_coring.fractional_bits = 2;
        iks_set_fnl_shp_coring(pMode0->ContextId, &final_sharpen_coring);

        final_sharpen_coring_idx_scale.high = 100;
        iks_set_fnl_shp_coring_idx_scale(pMode0->ContextId, &final_sharpen_coring_idx_scale);

        final_sharpen_min_coring_result.high = 100;
        iks_set_fnl_shp_min_coring_rslt(pMode0->ContextId, &final_sharpen_min_coring_result);

        final_sharpen_max_coring_result.high = 100;
        iks_set_fnl_shp_max_coring_rslt(pMode0->ContextId, &final_sharpen_max_coring_result);

        final_sharpen_scale_coring.high = 100;
        iks_set_fnl_shp_scale_coring(pMode0->ContextId, &final_sharpen_scale_coring);

        iks_set_fnl_shp_three_d_table(pMode0->ContextId, &final_sharpen_both_three_d_table);
    }

    {
        // Chroma filter
        ik_chroma_filter_t chroma_filter = {0};

        chroma_filter.radius = 32;
        iks_set_chroma_filter(pMode0->ContextId, &chroma_filter);
    }

    {
        // Wide chroma filter
        ik_wide_chroma_filter_t wide_chroma_filter = {0};

        iks_set_wide_chroma_filter(pMode0->ContextId, &wide_chroma_filter);
    }

    {
        // Wide chroma filter combine
        ik_wide_chroma_filter_combine_t chroma_filter_combine = {0};

        chroma_filter_combine.T1_cb = 10;
        chroma_filter_combine.T1_cr = 10;
        iks_set_wide_chroma_filter_combine(pMode0->ContextId, &chroma_filter_combine);
    }

    {
        // grgb mismatch
        ik_grgb_mismatch_t grgb_mismatch = {0};

        iks_set_grgb_mismatch(pMode0->ContextId, &grgb_mismatch);
    }

    {
        // MCTF
        ik_video_mctf_t video_mctf = {0};
        ik_video_mctf_ta_t video_mctf_ta = {0};
        ik_pos_dep33_t video_mctf_and_final_sharpen = {0};

        iks_set_video_mctf(pMode0->ContextId, &video_mctf);

        iks_set_video_mctf_ta(pMode0->ContextId, &video_mctf_ta);

        iks_set_video_mctf_and_fnl_shp(pMode0->ContextId, &video_mctf_and_final_sharpen);
    }

    {
        // Vig
        uint32 enb = 0;
        ik_vignette_t vignette = {0};

        iks_set_vignette_enb(pMode0->ContextId, enb);
        iks_set_vignette(pMode0->ContextId, &vignette);
    }

    {
        // HDR blc
        ik_static_blc_level_t frontend_static_blc = {0};

        iks_set_exp0_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
    }

    {
        // HDR gain
        ik_frontend_wb_gain_t frontend_wb_gain = {0};

        frontend_wb_gain.shutter_ratio = 4096;
        iks_set_exp0_frontend_wb_gain(pMode0->ContextId, &frontend_wb_gain);
    }

    {
        // CE
        ik_ce_t ce = {0};
        ik_ce_input_table_t ce_input_table = {0};
        ik_ce_output_table_t ce_out_table = {0};
        ce.coring_gain_high = 1U;
        ce.coring_index_scale_high = 1U;

        iks_set_ce(pMode0->ContextId, &ce);
        iks_set_ce_input_table(pMode0->ContextId, &ce_input_table);
        iks_set_ce_out_table(pMode0->ContextId, &ce_out_table);
    }

    {
        // HDR tone curve
        ik_frontend_tone_curve_t fe_tc = {0};

        iks_set_frontend_tone_curve(pMode0->ContextId, &fe_tc);
    }

    {
        // resampler strength
        ik_resampler_strength_t resampler_strength = {0};

        iks_set_resampler_strength(pMode0->ContextId, &resampler_strength);
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
        iks_set_aaa_stat_info(pMode0->ContextId, &stat_info);
        iks_set_aaa_pg_af_stat_info(pMode0->ContextId, &pg_af_stat_info);
        iks_set_af_stat_ex_info(pMode0->ContextId, &af_stat_ex_info);
        iks_set_pg_af_stat_ex_info(pMode0->ContextId, &pg_af_stat_ex_info);
        iks_set_histogram_info(pMode0->ContextId, &hist_info);
        iks_set_pg_histogram_info(pMode0->ContextId, &hist_info);
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
        iks_set_window_size_info(pMode0->ContextId, &window_size_info);
    }

    {
        // Warp
        uint32 enb = 0;
        ik_warp_info_t calib_warp_info = {0};

        iks_set_warp_enb(pMode0->ContextId, enb);

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
        iks_set_warp_info(pMode0->ContextId, &calib_warp_info);
    }

    {
        // CA
        uint32 enb = 0;
        ik_cawarp_info_t calib_ca_warp_info = {0};

        iks_set_cawarp_enb(pMode0->ContextId, enb);

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
        iks_set_cawarp_info(pMode0->ContextId, &calib_ca_warp_info);
    }

    {
        // DZoom
        ik_dzoom_info_t dzoom_info = {0};

        iks_set_dzoom_info(pMode0->ContextId, &dzoom_info);
    }

    {
        // Dummy
        ik_dummy_margin_range_t dmy_margin_range = {0};

        iks_set_dummy_margin_range(pMode0->ContextId, &dmy_margin_range);
    }

    {
        // Active win
        ik_vin_active_window_t vin_active_win = {0};

        iks_set_vin_active_win(pMode0->ContextId, &vin_active_win);
    }

    {
        // Warp internal
        ik_warp_internal_info_t warp_dzoom_internal = {0};

        warp_dzoom_internal.pwarp_horizontal_table = (int16 *)WarpGrid;
        warp_dzoom_internal.pwarp_vertical_table = (int16 *)(WarpGrid + (128*96*4));
        iks_set_warp_internal(pMode0->ContextId, &warp_dzoom_internal);
    }

    {
        // CA internal
        ik_cawarp_internal_info_t cawarp_internal = {0};

        iks_set_cawarp_internal(pMode0->ContextId, &cawarp_internal);
    }

    {
        // flip mode
        uint32 mode = 0;

        iks_set_flip_mode(pMode0->ContextId, mode);
    }

    {
        // Warp buf info
        ik_warp_buffer_info_t warp_buf_info = {0};

        warp_buf_info.luma_wait_lines = 8;
        warp_buf_info.luma_dma_size = 32;
        iks_set_warp_buffer_info(pMode0->ContextId, &warp_buf_info);
    }

    {
        // Stitch info
        ik_stitch_info_t stitch_info = {0};

        stitch_info.tile_num_x = 1;
        stitch_info.tile_num_y = 1;
        iks_set_stitching_info(pMode0->ContextId, &stitch_info);
    }

    {
        // Burst tile
        ik_burst_tile_t burst_tile = {0};

        iks_set_burst_tile(pMode0->ContextId, &burst_tile);
    }
}

static void IKS_Executer_Set_ImgFilters_Y2Y(const AMBA_IK_MODE_CFG_s *pMode1)
{
    {
        // YUV mode
        uint32 yuv_mode = 0;

        iks_set_yuv_mode(pMode1->ContextId, yuv_mode);
    }

    {
        // Chroma scale
        ik_chroma_scale_t chroma_scale = {0};

        iks_set_chroma_scale(pMode1->ContextId, &chroma_scale);
    }

    {
        // Chroma median
        ik_chroma_median_filter_t chroma_median_filter = {0};

        iks_set_chroma_median_filter(pMode1->ContextId, &chroma_median_filter);
    }

    {
        // SHPA proc mode
        ik_first_luma_process_mode_t first_luma_process_mode = {0};

        iks_set_fst_luma_process_mode(pMode1->ContextId, &first_luma_process_mode);
    }

    {
        // ASF
        ik_adv_spatial_filter_t advance_spatial_filter = {0};

        iks_set_adv_spatial_filter(pMode1->ContextId, &advance_spatial_filter);
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

        iks_set_fst_shp_both(pMode1->ContextId, &first_sharpen_both);

        first_sharpen_noise.level_str_adjust.high = 100;
        iks_set_fst_shp_noise(pMode1->ContextId, &first_sharpen_noise);

        iks_set_fst_shp_fir(pMode1->ContextId, &first_sharpen_fir);

        first_sharpen_coring.fractional_bits = 2;
        iks_set_fst_shp_coring(pMode1->ContextId, &first_sharpen_coring);

        first_sharpen_coring_idx_scale.high = 100;
        iks_set_fst_shp_coring_idx_scale(pMode1->ContextId, &first_sharpen_coring_idx_scale);

        first_sharpen_min_coring_result.high = 100;
        iks_set_fst_shp_min_coring_rslt(pMode1->ContextId, &first_sharpen_min_coring_result);

        first_sharpen_max_coring_result.high = 100;
        iks_set_fst_shp_max_coring_rslt(pMode1->ContextId, &first_sharpen_max_coring_result);

        first_sharpen_scale_coring.high = 100;
        iks_set_fst_shp_scale_coring(pMode1->ContextId, &first_sharpen_scale_coring);
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

        iks_set_fnl_shp_both(pMode1->ContextId, &final_sharpen_both);

        final_sharpen_noise.level_str_adjust.high = 100;
        iks_set_fnl_shp_noise(pMode1->ContextId, &final_sharpen_noise);

        iks_set_fnl_shp_fir(pMode1->ContextId, &final_sharpen_fir);

        final_sharpen_coring.fractional_bits = 2;
        iks_set_fnl_shp_coring(pMode1->ContextId, &final_sharpen_coring);

        final_sharpen_coring_idx_scale.high = 100;
        iks_set_fnl_shp_coring_idx_scale(pMode1->ContextId, &final_sharpen_coring_idx_scale);

        final_sharpen_min_coring_result.high = 100;
        iks_set_fnl_shp_min_coring_rslt(pMode1->ContextId, &final_sharpen_min_coring_result);

        final_sharpen_max_coring_result.high = 100;
        iks_set_fnl_shp_max_coring_rslt(pMode1->ContextId, &final_sharpen_max_coring_result);

        final_sharpen_scale_coring.high = 100;
        iks_set_fnl_shp_scale_coring(pMode1->ContextId, &final_sharpen_scale_coring);

        iks_set_fnl_shp_three_d_table(pMode1->ContextId, &final_sharpen_both_three_d_table);
    }

    {
        // Chroma filter
        ik_chroma_filter_t chroma_filter = {0};

        chroma_filter.radius = 32;
        iks_set_chroma_filter(pMode1->ContextId, &chroma_filter);
    }

    {
        // Wide chroma filter
        ik_wide_chroma_filter_t wide_chroma_filter = {0};

        iks_set_wide_chroma_filter(pMode1->ContextId, &wide_chroma_filter);
    }

    {
        // Wide chroma filter combine
        ik_wide_chroma_filter_combine_t chroma_filter_combine = {0};

        chroma_filter_combine.T1_cb = 10;
        chroma_filter_combine.T1_cr = 10;
        iks_set_wide_chroma_filter_combine(pMode1->ContextId, &chroma_filter_combine);
    }

    {
        // MCTF
        ik_video_mctf_t video_mctf = {0};
        ik_video_mctf_ta_t video_mctf_ta = {0};
        ik_pos_dep33_t video_mctf_and_final_sharpen = {0};

        iks_set_video_mctf(pMode1->ContextId, &video_mctf);

        iks_set_video_mctf_ta(pMode1->ContextId, &video_mctf_ta);

        iks_set_video_mctf_and_fnl_shp(pMode1->ContextId, &video_mctf_and_final_sharpen);
    }

    {
        // AAA
        ik_aaa_pg_af_stat_info_t pg_af_stat_info = {0};
        ik_pg_af_stat_ex_info_t pg_af_stat_ex_info = {0};
        ik_histogram_info_t hist_info = {0};

        pg_af_stat_info.af_tile_num_col = 32;
        pg_af_stat_info.af_tile_num_row = 16;
        iks_set_aaa_pg_af_stat_info(pMode1->ContextId, &pg_af_stat_info);
        iks_set_pg_af_stat_ex_info(pMode1->ContextId, &pg_af_stat_ex_info);
        iks_set_pg_histogram_info(pMode1->ContextId, &hist_info);
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
        iks_set_window_size_info(pMode1->ContextId, &window_size_info);
    }

    {
        // Warp
        uint32 enb = 0;
        ik_warp_info_t calib_warp_info = {0};

        iks_set_warp_enb(pMode1->ContextId, enb);

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
        iks_set_warp_info(pMode1->ContextId, &calib_warp_info);
    }

    {
        // DZoom
        ik_dzoom_info_t dzoom_info = {0};

        iks_set_dzoom_info(pMode1->ContextId, &dzoom_info);
    }

    {
        // Active win
        ik_vin_active_window_t vin_active_win = {0};

        iks_set_vin_active_win(pMode1->ContextId, &vin_active_win);
    }

    {
        // flip mode
        uint32 mode = 0;

        iks_set_flip_mode(pMode1->ContextId, mode);
    }

    {
        // Warp buf info
        ik_warp_buffer_info_t warp_buf_info = {0};

        warp_buf_info.luma_wait_lines = 8;
        warp_buf_info.luma_dma_size = 32;
        iks_set_warp_buffer_info(pMode1->ContextId, &warp_buf_info);
    }

    {
        // Stitch info
        ik_stitch_info_t stitch_info = {0};

        iks_set_stitching_info(pMode1->ContextId, &stitch_info);
    }

    {
        // Burst tile
        ik_burst_tile_t burst_tile = {0};

        iks_set_burst_tile(pMode1->ContextId, &burst_tile);
    }
}

static void IKS_Executer_Set_TestUpdateCheck(const AMBA_IK_MODE_CFG_s *pMode0)
{
    {
        // Sensor info
        ik_vin_sensor_info_t sensor_info = {0};

        iks_set_vin_sensor_info(pMode0->ContextId, &sensor_info);
    }

    {
        // HDR blc
        ik_static_blc_level_t frontend_static_blc = {0};

        iks_set_exp0_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
    }

    {
        // DBP
        ik_dynamic_bad_pixel_correction_t dynamic_bpc = {0};

        iks_set_dynamic_bad_pixel_corr(pMode0->ContextId, &dynamic_bpc);
    }

    {
        // CFA leakage
        ik_cfa_leakage_filter_t cfa_leakage_filter = {0};

        iks_set_cfa_leakage_filter(pMode0->ContextId, &cfa_leakage_filter);
    }

    {
        // Anti aliasing
        ik_anti_aliasing_t anti_aliasing = {0};

        iks_set_anti_aliasing(pMode0->ContextId, &anti_aliasing);
    }

    {
        // grgb mismatch
        ik_grgb_mismatch_t grgb_mismatch = {0};

        iks_set_grgb_mismatch(pMode0->ContextId, &grgb_mismatch);
    }

    {
        // Before CE
        ik_wb_gain_t before_ce_wb_gain = {0};

        before_ce_wb_gain.gain_b = 4096;
        before_ce_wb_gain.gain_g = 4096;
        before_ce_wb_gain.gain_r = 4096;
        iks_set_before_ce_wb_gain(pMode0->ContextId, &before_ce_wb_gain);
    }

    {
        // After CE
        ik_wb_gain_t after_ce_wb_gain = {0};

        after_ce_wb_gain.gain_b = 4096;
        after_ce_wb_gain.gain_g = 4096;
        after_ce_wb_gain.gain_r = 4096;
        iks_set_after_ce_wb_gain(pMode0->ContextId, &after_ce_wb_gain);
    }

    {
        // CE
        ik_ce_t ce = {0};
        ik_ce_input_table_t ce_input_table = {0};
        ik_ce_output_table_t ce_out_table = {0};
        ce.coring_gain_high = 1U;
        ce.coring_index_scale_high = 1U;

        iks_set_ce(pMode0->ContextId, &ce);
        iks_set_ce_input_table(pMode0->ContextId, &ce_input_table);
        iks_set_ce_out_table(pMode0->ContextId, &ce_out_table);
    }

    {
        // CFA noise
        ik_cfa_noise_filter_t cfa_noise_filter = {0};

        iks_set_cfa_noise_filter(pMode0->ContextId, &cfa_noise_filter);
    }

    {
        // Demosaic
        ik_demosaic_t demosaic = {0};

        iks_set_demosaic(pMode0->ContextId, &demosaic);
    }

    {
        // CC
        ik_color_correction_t color_correction = {0};

        iks_set_color_correction(pMode0->ContextId, &color_correction);
    }

    {
        // tone curve
        ik_tone_curve_t tone_curve = {0};

        iks_set_tone_curve(pMode0->ContextId, &tone_curve);
    }

    {
        // RGB to YUV
        ik_rgb_to_yuv_matrix_t rgb_to_yuv_matrix = {0};

        iks_set_rgb_to_yuv_matrix(pMode0->ContextId, &rgb_to_yuv_matrix);
    }

    {
        // Chroma scale
        ik_chroma_scale_t chroma_scale = {0};

        iks_set_chroma_scale(pMode0->ContextId, &chroma_scale);
    }

    {
        // Chroma median
        ik_chroma_median_filter_t chroma_median_filter = {0};

        iks_set_chroma_median_filter(pMode0->ContextId, &chroma_median_filter);
    }

    {
        // Chroma filter
        ik_chroma_filter_t chroma_filter = {0};

        chroma_filter.radius = 32;
        iks_set_chroma_filter(pMode0->ContextId, &chroma_filter);
    }

    {
        // Wide chroma filter
        ik_wide_chroma_filter_t wide_chroma_filter = {0};

        iks_set_wide_chroma_filter(pMode0->ContextId, &wide_chroma_filter);
    }

    {
        // Wide chroma filter combine
        ik_wide_chroma_filter_combine_t chroma_filter_combine = {0};

        chroma_filter_combine.T1_cb = 10;
        chroma_filter_combine.T1_cr = 10;
        iks_set_wide_chroma_filter_combine(pMode0->ContextId, &chroma_filter_combine);
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

        iks_set_fnl_shp_both(pMode0->ContextId, &final_sharpen_both);

        final_sharpen_noise.level_str_adjust.high = 100;
        iks_set_fnl_shp_noise(pMode0->ContextId, &final_sharpen_noise);

        iks_set_fnl_shp_fir(pMode0->ContextId, &final_sharpen_fir);

        final_sharpen_coring.fractional_bits = 2;
        iks_set_fnl_shp_coring(pMode0->ContextId, &final_sharpen_coring);

        final_sharpen_coring_idx_scale.high = 100;
        iks_set_fnl_shp_coring_idx_scale(pMode0->ContextId, &final_sharpen_coring_idx_scale);

        final_sharpen_min_coring_result.high = 100;
        iks_set_fnl_shp_min_coring_rslt(pMode0->ContextId, &final_sharpen_min_coring_result);

        final_sharpen_max_coring_result.high = 100;
        iks_set_fnl_shp_max_coring_rslt(pMode0->ContextId, &final_sharpen_max_coring_result);

        final_sharpen_scale_coring.high = 100;
        iks_set_fnl_shp_scale_coring(pMode0->ContextId, &final_sharpen_scale_coring);

        iks_set_fnl_shp_three_d_table(pMode0->ContextId, &final_sharpen_both_three_d_table);
    }

    {
        // MCTF
        ik_video_mctf_t video_mctf = {0};
        ik_video_mctf_ta_t video_mctf_ta = {0};
        ik_pos_dep33_t video_mctf_and_final_sharpen = {0};

        iks_set_video_mctf(pMode0->ContextId, &video_mctf);

        iks_set_video_mctf_ta(pMode0->ContextId, &video_mctf_ta);

        iks_set_video_mctf_and_fnl_shp(pMode0->ContextId, &video_mctf_and_final_sharpen);
    }
}

static void IKS_TestCovr_ImgExecuterComponentIF(const AMBA_IK_MODE_CFG_s *pMode0,
        const AMBA_IK_MODE_CFG_s *pMode1,
        const AMBA_IK_MODE_CFG_s *pMode2,
        const AMBA_IK_MODE_CFG_s *pMode3,
        AMBA_IK_ABILITY_s *pAbility)
{
    UINT32 Rval = IK_OK;
    amba_iks_context_entity_t *p_ctx = NULL;
    ik_execute_container_t execute_container = {0};
    ik_buffer_info_t *p_iks_buffer_info = NULL;
    ik_first_luma_process_mode_t first_luma_process_mode = {0};

    iks_ctx_get_context(pMode0->ContextId, &p_ctx);

    IKS_Executer_Set_ImgFilters(pMode0); // setup filters for filter update checker
    IKS_Executer_Set_ImgFilters_Y2Y(pMode1); // setup filters for filter update checker
    Rval = iks_execute(pMode0->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr
    Rval = iks_execute(pMode1->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr

    p_ctx->organization.active_cr_state.cr_index.raw_cmpr =
        (p_ctx->organization.active_cr_state.cr_index.raw_cmpr + 1u) % p_ctx->organization.attribute.cr_ring_number; // ring next to prevent ring buffer checker error
    p_ctx->filters.update_flags.cr.raw_cmpr = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container); // test here
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_buffer_hdr_address", "test case FILTER_RAW_COMPRESSION CR update OK", id_cnt++);

    p_ctx->organization.active_cr_state.cr_index.chroma_down_smp =
        (p_ctx->organization.active_cr_state.cr_index.chroma_down_smp + 1u) % p_ctx->organization.attribute.cr_ring_number; // ring next to prevent ring buffer checker error
    p_ctx->filters.update_flags.cr.chroma_down_smp = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container); // test here
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_buffer_yuv_address", "test case FILTER_CHROMA_DOWNSAMPLING_FILTER CR update OK", id_cnt++);

    p_ctx->organization.active_cr_state.cr_index.me_resmp =
        (p_ctx->organization.active_cr_state.cr_index.me_resmp + 1u) % p_ctx->organization.attribute.cr_ring_number; // ring next to prevent ring buffer checker error
    p_ctx->filters.update_flags.cr.me_resmp = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container); // test here
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_buffer_yuv_address", "test case FILTER_ME_RESAMPLING_FILTER CR update OK", id_cnt++);

    p_ctx->organization.active_cr_state.cr_index.luma_privacy =
        (p_ctx->organization.active_cr_state.cr_index.luma_privacy + 1u) % p_ctx->organization.attribute.cr_ring_number; // ring next to prevent ring buffer checker error
    p_ctx->filters.update_flags.cr.luma_privacy = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container); // test here
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_buffer_yuv_address", "test case FILTER_LUMA_PRIVACY_MASK CR update OK", id_cnt++);

    p_ctx->organization.active_cr_state.cr_index.chroma_privacy =
        (p_ctx->organization.active_cr_state.cr_index.chroma_privacy + 1u) % p_ctx->organization.attribute.cr_ring_number; // ring next to prevent ring buffer checker error
    p_ctx->filters.update_flags.cr.chroma_privacy = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container); // test here
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_buffer_yuv_address", "test case FILTER_CHROMA_PRIVACY_MASK CR update OK", id_cnt++);

    p_ctx->organization.active_cr_state.cr_index.me_resmp_2x =
        (p_ctx->organization.active_cr_state.cr_index.me_resmp_2x + 1u) % p_ctx->organization.attribute.cr_ring_number; // ring next to prevent ring buffer checker error
    p_ctx->filters.update_flags.cr.me_resmp_2x = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container); // test here
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_buffer_yuv_address", "test case FILTER_ME_RESAMPLING_2X CR update OK", id_cnt++);

    p_ctx->filters.update_flags.iso.af_stat_ex_info_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part1", "test (p_update_flag->iso.af_stat_ex_info_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.pg_af_stat_ex_info_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part1", "test (p_update_flag->iso.pg_af_stat_ex_info_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.histogram_info_update = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part1", "test (p_update_flag->iso.histogram_info_update != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.histogram_info_pg_update = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part1", "test (p_update_flag->iso.histogram_info_pg_update != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.static_bpc_internal_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part1", "test (p_update_flag->iso.static_bpc_internal_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.vignette_compensation_internal_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part1", "test (p_update_flag->iso.vignette_compensation_internal_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.warp_internal_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part1", "test (p_update_flag->iso.warp_internal_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.warp_buf_info_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part1", "test (p_update_flag->iso.warp_buf_info_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.calib_ca_warp_info_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part1", "test (p_update_flag->iso.calib_ca_warp_info_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.cawarp_internal_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part1", "test (p_update_flag->iso.cawarp_internal_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.chroma_filter_updated = 1u;
    p_ctx->filters.update_flags.iso.calib_warp_info_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part1", "(p_update_flag->iso.calib_warp_info_updated != 0u) true OK case", id_cnt++);
    p_ctx->filters.update_flags.iso.calib_warp_info_updated = 0u;
    p_ctx->filters.update_flags.iso.chroma_filter_updated = 0u;

    p_ctx->filters.update_flags.iso.sensor_information_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part2", "test (p_update_flag->iso.sensor_information_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.rgb_ir_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part2", "test (p_update_flag->iso.rgb_ir_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.dgain_sat_lvl_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part2", "test (p_update_flag->iso.dgain_sat_lvl_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.pre_cc_gain_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part2", "test (p_update_flag->iso.pre_cc_gain_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.advance_spatial_filter_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.advance_spatial_filter_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fstshpns_fir_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fstshpns_fir_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fstshpns_both_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fstshpns_both_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fstshpns_noise_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fstshpns_noise_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fstshpns_coring_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fstshpns_coring_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fstshpns_coring_index_scale_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fstshpns_coring_index_scale_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fstshpns_min_coring_result_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fstshpns_min_coring_result_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fstshpns_max_coring_result_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fstshpns_max_coring_result_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fstshpns_scale_coring_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fstshpns_scale_coring_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.internal_video_mctf_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.internal_video_mctf_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.video_mctf_ta_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.video_mctf_ta_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fnlshpns_both_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fnlshpns_both_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fnlshpns_noise_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fnlshpns_noise_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fnlshpns_fir_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fnlshpns_fir_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fnlshpns_coring_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fnlshpns_coring_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fnlshpns_coring_index_scale_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fnlshpns_coring_index_scale_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fnlshpns_min_coring_result_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fnlshpns_min_coring_result_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fnlshpns_max_coring_result_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fnlshpns_max_coring_result_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fnlshpns_scale_coring_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fnlshpns_scale_coring_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.fnlshpns_both_tdt_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.fnlshpns_both_tdt_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.video_mctf_and_final_sharpen_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.video_mctf_and_final_sharpen_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.wide_chroma_filter = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.wide_chroma_filter != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.wide_chroma_filter_combine = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part3", "test (p_update_flag->iso.wide_chroma_filter_combine != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.exp1_fe_static_blc_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part4", "test (p_update_flag->iso.exp1_fe_static_blc_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.exp2_fe_static_blc_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part4", "test (p_update_flag->iso.exp2_fe_static_blc_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.exp1_fe_wb_gain_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part4", "test (p_update_flag->iso.exp1_fe_wb_gain_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.exp2_fe_wb_gain_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part4", "test (p_update_flag->iso.exp2_fe_wb_gain_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.ce_input_table_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part4", "test (p_update_flag->iso.ce_input_table_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.ce_out_table_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part4", "test (p_update_flag->iso.ce_out_table_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.dmy_range_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index", "test (p_update_flag->iso.dmy_range_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.dzoom_info_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index", "test (p_update_flag->iso.dzoom_info_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.vin_active_win_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index", "test (p_update_flag->iso.vin_active_win_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.stitching_info_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index", "test (p_update_flag->iso.stitching_info_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.flip_mode_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index", "test (p_update_flag->iso.flip_mode_updated != 0U) OK", id_cnt++);

    p_ctx->filters.update_flags.iso.warp_enable_updated = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index", "test (p_update_flag->iso.warp_enable_updated != 0U) OK", id_cnt++);

    Rval = iks_execute(pMode1->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_update_liso_sect_header", "test if(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC4] != 0u) false OK case", id_cnt++);

    extern uint32 iks_exe_inject_safety_error(uint32 context_id, const uint32 selection);
    p_ctx->filters.update_flags.cr.cawarp = 1u;
    p_ctx->filters.update_flags.cr.warp = 1u;
    p_ctx->filters.update_flags.cr.aaa_data = 1u;
    p_ctx->filters.update_flags.cr.hdr_blend = 1u;
    iks_exe_inject_safety_error(pMode0->ContextId, 1u);
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "_ring_buf_checker", "test update case ring buffer error case", id_cnt++);

    // reset to clear safety state
    _Reset_Image_Kernel(pAbility);
    IKS_Executer_Set_ImgFilters(pMode0); // setup filters for filter update checker
    Rval = iks_execute(pMode0->ContextId, &execute_container); // first exe

    iks_exe_inject_safety_error(pMode0->ContextId, 1u);
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "_ring_buf_checker", "test non-update case ring buffer error case", id_cnt++);

    // reset to clear safety state
    _Reset_Image_Kernel(pAbility);
    IKS_Executer_Set_ImgFilters(pMode0); // setup filters for filter update checker
    IKS_Executer_Set_ImgFilters_Y2Y(pMode1); // setup filters for filter update checker
    Rval = iks_execute(pMode0->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr
    Rval = iks_execute(pMode1->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr

    iks_exe_inject_safety_error(pMode0->ContextId, 5u);
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "iks_cr_memory_fence_check", "test cr mem fence error case", id_cnt++);

    // reset to clear safety state
    _Reset_Image_Kernel(pAbility);
    IKS_Executer_Set_ImgFilters(pMode0); // setup filters for filter update checker
    IKS_Executer_Set_ImgFilters_Y2Y(pMode1); // setup filters for filter update checker
    Rval = iks_execute(pMode0->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr
    Rval = iks_execute(pMode1->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr

    iks_exe_inject_safety_error(pMode0->ContextId, 0u);
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_iks_check_ctx_mem_fence", "test ctx mem fence error case", id_cnt++);

    // reset to clear safety state
    _Reset_Image_Kernel(pAbility);
    IKS_Executer_Set_ImgFilters(pMode0); // setup filters for filter update checker
    IKS_Executer_Set_ImgFilters_Y2Y(pMode1); // setup filters for filter update checker
    Rval = iks_execute(pMode0->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr
    Rval = iks_execute(pMode1->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr

    // test crc_check_list here
    iks_exe_inject_safety_error(pMode0->ContextId, 6u);
    p_ctx->organization.attribute.ability.video_pipe = AMBA_IK_VIDEO_HDR_EXPO_3;
    //p_ctx->filters.crc_check_list.hdr_blend = 1u;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_iks_trans_crc_check_list", "test if(p_ctx->filters.crc_check_list.hdr_blend != 0u) false OK case", id_cnt++);

    // reset to clear safety state
    _Reset_Image_Kernel(pAbility);
    IKS_Executer_Set_ImgFilters(pMode0); // setup filters for filter update checker
    IKS_Executer_Set_ImgFilters_Y2Y(pMode1); // setup filters for filter update checker
    Rval = iks_execute(pMode0->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr
    Rval = iks_execute(pMode1->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr

    p_ctx->organization.attribute.ability.video_pipe = AMBA_IK_VIDEO_MAX;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_cr_utilities", "test default fail case", id_cnt++);

    // reset to clear safety state
    _Reset_Image_Kernel(pAbility);
    IKS_Executer_Set_ImgFilters(pMode0); // setup filters for filter update checker
    IKS_Executer_Set_ImgFilters_Y2Y(pMode1); // setup filters for filter update checker
    Rval = iks_execute(pMode0->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr
    Rval = iks_execute(pMode1->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr

    p_ctx->organization.attribute.ability.pipe = AMBA_IK_PIPE_STILL;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_cr_utilities", "test pipe != AMBA_IK_PIPE_VIDEO fail case", id_cnt++);

    // reset to clear safety state
    _Reset_Image_Kernel(pAbility);
    IKS_Executer_Set_ImgFilters(pMode0); // setup filters for filter update checker
    IKS_Executer_Set_ImgFilters_Y2Y(pMode1); // setup filters for filter update checker
    Rval = iks_execute(pMode0->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr
    Rval = iks_execute(pMode1->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr

    iks_arch_deinit_architecture();
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "iks_exe_execute_update_cr", "test iks_arch_get_ik_working_buffer fail case", id_cnt++);

    _Reset_Image_Kernel(pAbility);
    IKS_Executer_Set_ImgFilters(pMode0); // setup filters for filter update checker
    IKS_Executer_Set_ImgFilters_Y2Y(pMode1); // setup filters for filter update checker
    Rval = iks_execute(pMode0->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr
    Rval = iks_execute(pMode1->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr

    Rval = iks_arch_get_ik_working_buffer(&p_iks_buffer_info);
    p_iks_buffer_info->safety_state = IK_SAFETY_STATE_ERROR;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "iks_exe_execute_update_cr", "test if(p_iks_buffer_info->safety_state != IK_SAFETY_STATE_ERROR) false case", id_cnt++);
    p_iks_buffer_info->safety_state = IK_SAFETY_STATE_OK;

    IKS_Executer_Set_ImgFilters(pMode2);
    Rval = iks_execute(pMode2->ContextId, &execute_container);
    Rval = iks_execute(pMode2->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "iks_exe_execute_update_cr", "test if(flow_idx != src_flow_idx) false OK case", id_cnt++);

    _Reset_Image_Kernel(pAbility);

    IKS_Executer_Set_TestUpdateCheck(pMode0);
    iks_set_fst_luma_process_mode(pMode0->ContextId, &first_luma_process_mode);
    Rval = iks_execute(pMode0->ContextId, &execute_container); // first exe for iks_exe_execute_first_cr
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "iks_exe_execute_filter_r2y_4_update_check", "test if (p_update_flag->iso.advance_spatial_filter_updated != 1U) true fail case", id_cnt++);

    first_luma_process_mode.use_sharpen_not_asf = 1;
    iks_set_fst_luma_process_mode(pMode0->ContextId, &first_luma_process_mode);
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "iks_exe_execute_filter_r2y_4_update_check", "test if (p_update_flag->iso.fstshpns_xxx_updated != 1U) true fail case", id_cnt++);

    iks_arch_deinit_architecture();
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "iks_exe_execute_first_cr", "test iks_arch_get_ik_working_buffer fail case", id_cnt++);

    _Reset_Image_Kernel(pAbility);
    IKS_Executer_Set_ImgFilters(pMode0); // setup filters for filter update checker

    Rval = iks_arch_get_ik_working_buffer(&p_iks_buffer_info);
    p_iks_buffer_info->safety_state = IK_SAFETY_STATE_ERROR;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "iks_exe_execute_first_cr", "test if(p_iks_buffer_info->safety_state != IK_SAFETY_STATE_ERROR) false case", id_cnt++);
    p_iks_buffer_info->safety_state = IK_SAFETY_STATE_OK;

    iks_exe_inject_safety_error(pMode0->ContextId, 0u);
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "iks_exe_execute_first_cr", "test if(rval == IK_OK) false by ctx mem fence error case", id_cnt++);

    // reset to clear safety state
    _Reset_Image_Kernel(pAbility);
    IKS_Executer_Set_ImgFilters(pMode0); // setup filters for filter update checker

    iks_exe_inject_safety_error(pMode0->ContextId, 5u);
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "iks_exe_execute_first_cr", "test if(rval == IK_OK) false by cr mem fence error case", id_cnt++);

    // reset to clear safety state
    _Reset_Image_Kernel(pAbility);
    IKS_Executer_Set_ImgFilters(pMode0); // setup filters for filter update checker

    iks_exe_inject_safety_error(pMode0->ContextId, 6u);
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "iks_exe_execute_first_cr", "test if(p_ctx->filters.input_param.select_crc_check_list_injection != 0U) true OK case", id_cnt++);

    extern uint32 iks_exe_init_executer(uint32 context_id);
    p_ctx->organization.attribute.ability.pipe = AMBA_IK_PIPE_STILL;
    Rval = iks_exe_init_executer(pMode0->ContextId);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "iks_exe_init_executer", "test pipe != AMBA_IK_PIPE_VIDEO fail case", id_cnt++);

    p_ctx->organization.attribute.ability.pipe = AMBA_IK_PIPE_VIDEO;
    p_ctx->organization.attribute.ability.video_pipe = AMBA_IK_VIDEO_MAX;
    Rval = iks_exe_init_executer(pMode0->ContextId);
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "iks_exe_init_executer", "test video_pipe out of range fail case", id_cnt++);

    iks_exe_inject_safety_error(pMode0->ContextId, 100u);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "iks_exe_inject_safety_error", "test default fail case", id_cnt++);

    // reset to clear safety state
    _Reset_Image_Kernel(pAbility);
    IKS_Executer_Set_ImgFilters(pMode0); // setup filters for filter update checker
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_iks_calc_crc32_sec4_cr", "test exe_iks_calc_crc32_sec4_cr OK case", id_cnt++);

    // reset to clear safety state
    _Reset_Image_Kernel(pAbility);
    IKS_Executer_Set_ImgFilters(pMode0); // setup filters for filter update checker
    iks_exe_inject_safety_error(pMode0->ContextId, 6u);
    p_ctx->filters.input_param.stitching_info.enable = 1;
    p_ctx->filters.crc_check_list.cawarp = 0;
    p_ctx->filters.crc_check_list.warp = 0;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_iks_calc_crc32_flow_tbl_cr", "test (p_ctx->filters.crc_check_list.cawarp/warp != 0u) false OK case", id_cnt++);

    {
        // reset to clear safety state
        _Reset_Image_Kernel(pAbility);
        IKS_Executer_Set_ImgFilters(pMode0); // setup filters for filter update checker

        p_ctx->organization.attribute.ability.video_pipe = AMBA_IK_VIDEO_Y2Y;
        Rval = iks_execute(pMode0->ContextId, &execute_container);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_iks_calc_crc32_cr", "test if(p_ctx->filters.input_param.safety_info.update_freq == 0u) true OK case", id_cnt++);
        p_ctx->organization.attribute.ability.video_pipe = AMBA_IK_VIDEO_LINEAR_CE;
    }

    // reset to clear safety state
    _Reset_Image_Kernel(pAbility);
    IKS_Executer_Set_ImgFilters(pMode0); // setup filters for filter update checker

    p_iks_buffer_info->safety_crc_enable = 0;
    Rval = iks_execute(pMode0->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "iks_exe_execute_first_cr", "test if(p_iks_buffer_info->safety_crc_enable == 1u) false OK case", id_cnt++);
    p_iks_buffer_info->safety_crc_enable = 1;

    // reset to clear safety state
    _Reset_Image_Kernel(pAbility);

    Rval = iks_execute(pMode3->ContextId, &execute_container);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "iks_exe_execute_filter_update_check", "test iks_exe_execute_filter_update_check fail NG case", id_cnt++);
}

static void IKS_TestCovr_ImgExecuterUnitCaWarp(const AMBA_IK_MODE_CFG_s *pMode0)
{
    UINT32 Rval = IK_OK;
    amba_ik_cawarp_calc_t cawarp_calc = {0};
    ik_cawarp_info_t cawarp_info = {0};
    ik_window_size_info_t window_size_info = {0};
    amba_ik_calc_win_result_t result_win = {0};
    cawarp_cfg_t cawarp_cfg = {0};
    amba_iks_context_entity_t *p_ctx;
    idsp_flow_ctrl_t *p_flow = NULL;
    idsp_flow_tbl_t *p_flow_tbl = NULL;
    uintptr addr = 0;
    extern uint32 iks_cawarp_calc(uintptr sec2_header_addr, amba_ik_cawarp_calc_t *p_info);

    iks_ctx_get_context(pMode0->ContextId, &p_ctx);
    iks_ctx_get_flow_control(pMode0->ContextId, 0, &addr);
    (void)amba_ik_system_memcpy(&p_flow, &addr, sizeof(void *));
    iks_ctx_get_flow_tbl(pMode0->ContextId, 0, &addr);
    (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        cawarp_calc.p_calib_ca_warp_info = &cawarp_info;
        cawarp_calc.p_window_size_info = &window_size_info;
        cawarp_calc.p_result_win = &result_win;
        cawarp_calc.p_ca_warp_control = &cawarp_cfg;

        cawarp_calc.cawarp_enable = 1u;
        cawarp_info.vin_sensor_geo.h_sub_sample.factor_num = 1u;
        cawarp_info.vin_sensor_geo.h_sub_sample.factor_den = 1u;
        cawarp_info.vin_sensor_geo.v_sub_sample.factor_num = 1u;
        cawarp_info.vin_sensor_geo.v_sub_sample.factor_den = 1u;
        window_size_info.vin_sensor.h_sub_sample.factor_num = 1u;
        window_size_info.vin_sensor.h_sub_sample.factor_den = 1u;
        window_size_info.vin_sensor.v_sub_sample.factor_num = 1u;
        window_size_info.vin_sensor.v_sub_sample.factor_den = 1u;
        result_win.dmy_win_geo.width = 1920;
        result_win.dmy_win_geo.height = 1080;

        cawarp_info.vin_sensor_geo.start_x = 100;
        window_size_info.vin_sensor.start_x = 10;
        cawarp_info.vin_sensor_geo.start_y = 100;
        window_size_info.vin_sensor.start_y = 10;
        Rval = iks_cawarp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2], &cawarp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitCaWarp.c", "check_cawarp_valid_range_non_zero", "test invalid CA vin size fail case", id_cnt++);

        result_win.cfa_win_dim.width = 1920;
        window_size_info.main_win.width = 1920;
        Rval = iks_cawarp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2], &cawarp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitCaWarp.c", "check_cawarp_valid_range_win_size", "test invalid CA win size fail case", id_cnt++);

        window_size_info.vin_sensor.h_sub_sample.factor_num = 0u;
        Rval = iks_cawarp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2], &cawarp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitCaWarp.c", "check_cawarp_valid_range", "test invalid CA SubSmp fail case", id_cnt++);

        window_size_info.vin_sensor.h_sub_sample.factor_num = 1u;
        window_size_info.vin_sensor.h_sub_sample.factor_den = 0u;
        Rval = iks_cawarp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2], &cawarp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitCaWarp.c", "check_cawarp_valid_range", "test invalid CA SubSmp fail case", id_cnt++);

        window_size_info.vin_sensor.h_sub_sample.factor_den = 1u;
        window_size_info.vin_sensor.v_sub_sample.factor_num = 0u;
        Rval = iks_cawarp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2], &cawarp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitCaWarp.c", "check_cawarp_valid_range", "test invalid CA SubSmp fail case", id_cnt++);

        window_size_info.vin_sensor.v_sub_sample.factor_num = 1u;
        window_size_info.vin_sensor.v_sub_sample.factor_den = 0u;
        Rval = iks_cawarp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2], &cawarp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitCaWarp.c", "check_cawarp_valid_range", "test invalid CA SubSmp fail case", id_cnt++);

        window_size_info.vin_sensor.v_sub_sample.factor_den = 1u;
        cawarp_info.vin_sensor_geo.h_sub_sample.factor_num = 0u;
        Rval = iks_cawarp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2], &cawarp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitCaWarp.c", "check_cawarp_valid_range", "test invalid CA SubSmp fail case", id_cnt++);

        cawarp_info.vin_sensor_geo.h_sub_sample.factor_num = 1u;
        cawarp_info.vin_sensor_geo.h_sub_sample.factor_den = 0u;
        Rval = iks_cawarp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2], &cawarp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitCaWarp.c", "check_cawarp_valid_range", "test invalid CA SubSmp fail case", id_cnt++);

        cawarp_info.vin_sensor_geo.h_sub_sample.factor_den = 1u;
        cawarp_info.vin_sensor_geo.v_sub_sample.factor_num = 0u;
        Rval = iks_cawarp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2], &cawarp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitCaWarp.c", "check_cawarp_valid_range", "test invalid CA SubSmp fail case", id_cnt++);

        cawarp_info.vin_sensor_geo.v_sub_sample.factor_num = 1u;
        cawarp_info.vin_sensor_geo.v_sub_sample.factor_den = 0u;
        Rval = iks_cawarp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2], &cawarp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitCaWarp.c", "check_cawarp_valid_range", "test invalid CA SubSmp fail case", id_cnt++);
        cawarp_info.vin_sensor_geo.v_sub_sample.factor_den = 1u;
    }

    {
        cawarp_intpl_info intpl = {0};
        extern void iks_setup_cawarp_horz_grid_size(cawarp_cfg_t *p_ca_cfg, cawarp_intpl_info *p_intpl, const amba_ik_calc_win_result_t *p_result_win);
        extern void iks_setup_cawarp_vert_grid_size(cawarp_cfg_t *p_ca_cfg, cawarp_intpl_info *p_intpl, const amba_ik_calc_win_result_t *p_result_win);

        result_win.cfa_win_dim.width = 3840;
        iks_setup_cawarp_horz_grid_size(&cawarp_cfg, &intpl, &result_win);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitCaWarp.c", "iks_setup_cawarp_horz_grid_size", "test if (p_result_win->cfa_win_dim.width > stitch_threshold) true OK case", id_cnt++);

        result_win.cfa_win_dim.width = 5760;
        iks_setup_cawarp_horz_grid_size(&cawarp_cfg, &intpl, &result_win);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitCaWarp.c", "iks_setup_cawarp_horz_grid_size", "test if(((p_result_win->cfa_win_dim.width >> 1UL) + 128UL) > stitch_threshold) true OK case", id_cnt++);

        result_win.cfa_win_dim.width = 3840;
        iks_setup_cawarp_vert_grid_size(&cawarp_cfg, &intpl, &result_win);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitCaWarp.c", "iks_setup_cawarp_vert_grid_size", "test if (p_result_win->cfa_win_dim.width > stitch_threshold) true OK case", id_cnt++);

        result_win.cfa_win_dim.width = 5760;
        iks_setup_cawarp_vert_grid_size(&cawarp_cfg, &intpl, &result_win);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitCaWarp.c", "iks_setup_cawarp_vert_grid_size", "test if(((p_result_win->cfa_win_dim.width >> 1UL) + 128UL) > stitch_threshold) true OK case", id_cnt++);
    }

    {
        cawarp_info.vin_sensor_geo.h_sub_sample.factor_num = 1u;
        cawarp_info.vin_sensor_geo.h_sub_sample.factor_den = 1u;
        cawarp_info.vin_sensor_geo.v_sub_sample.factor_num = 1u;
        cawarp_info.vin_sensor_geo.v_sub_sample.factor_den = 1u;
        cawarp_info.vin_sensor_geo.start_x = 0;
        cawarp_info.vin_sensor_geo.start_y = 0;
        cawarp_info.vin_sensor_geo.width = 1920;
        cawarp_info.vin_sensor_geo.height = 1080;
        cawarp_info.hor_grid_num = 32;
        cawarp_info.ver_grid_num = 19;
        cawarp_info.tile_width_exp = 6;
        cawarp_info.tile_height_exp = 6;
        cawarp_info.p_cawarp_red = (ik_grid_point_t *)CawarpRedGrid;
        cawarp_info.p_cawarp_blue = (ik_grid_point_t *)CawarpBlueGrid;

        window_size_info.vin_sensor.h_sub_sample.factor_num = 1u;
        window_size_info.vin_sensor.h_sub_sample.factor_den = 1u;
        window_size_info.vin_sensor.v_sub_sample.factor_num = 1u;
        window_size_info.vin_sensor.v_sub_sample.factor_den = 1u;
        window_size_info.vin_sensor.start_x = 0;
        window_size_info.vin_sensor.start_y = 0;
        window_size_info.vin_sensor.width = 1920;
        window_size_info.vin_sensor.height = 1080;
        window_size_info.main_win.width = 1920;
        window_size_info.main_win.height = 1080;

        result_win.cfa_win_dim.width = 1920;
        result_win.cfa_win_dim.height = 1080;

        cawarp_calc.stitch_enable = 0u;
        cawarp_calc.is_group_cmd = 1u;
        Rval = iks_cawarp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2], &cawarp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitCaWarp.c", "iks_cawarp_calc_preproc_horz_table", "test (p_info->is_group_cmd == 1u) true OK case", id_cnt++);

        Rval = iks_cawarp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2], NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitCaWarp.c", "iks_cawarp_calc", "test NULL ptr fail case", id_cnt++);

        cawarp_calc.flip_mode = IK_FLIP_RAW_H | IK_FLIP_RAW_V;
        Rval = iks_cawarp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2], &cawarp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitCaWarp.c", "iks_cawarp_calc", "test flip OK case", id_cnt++);
    }

    {
        extern void iks_ca_warp_internal(idsp_flow_ctrl_t *p_flow, cawarp_cfg_t *p_ca_cfg, const amba_iks_filter_t *p_filters);

        p_flow->calib.cawarp_horizontal_table_addr_red = (uint32)CawarpRedGrid;
        p_flow->calib.cawarp_horizontal_table_addr_blue = (uint32)CawarpBlueGrid;
        p_flow->calib.cawarp_vertical_table_addr_red = (uint32)(CawarpRedGrid + 2*64*96);
        p_flow->calib.cawarp_vertical_table_addr_blue = (uint32)(CawarpBlueGrid + 2*64*96);

        p_ctx->filters.input_param.stitching_info.enable = 1;
        iks_ca_warp_internal(p_flow, &cawarp_cfg, &p_ctx->filters);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitCaWarp.c", "iks_ca_warp_internal", "test stitch enable OK case", id_cnt++);

        p_ctx->filters.input_param.ca_warp_internal.horz_warp_enable = 1;
        p_ctx->filters.input_param.ca_warp_internal.vert_warp_enable = 1;
        iks_ca_warp_internal(p_flow, &cawarp_cfg, &p_ctx->filters);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitCaWarp.c", "iks_ca_warp_internal", "test stitch + cawarp enable OK case", id_cnt++);
    }
}

static void IKS_TestCovr_ImgExecuterUnitWarp(const AMBA_IK_MODE_CFG_s *pMode0)
{
    UINT32 Rval = IK_OK;
    amba_ik_warp_calc_t warp_calc = {0};
    ik_warp_info_t warp_info = {0};
    ik_window_size_info_t window_size_info = {0};
    amba_ik_calc_win_result_t result_win = {0};
    set_warp_control_t geo_warp_control = {0};
    ik_stitch_info_t stitching_info = {0};
    ik_dummy_margin_range_t dmy_range = {0};
    amba_ik_warp_phase_t warp_phase = {0};
    amba_iks_context_entity_t *p_ctx;
    idsp_flow_ctrl_t *p_flow = NULL;
    idsp_flow_tbl_t *p_flow_tbl = NULL;
    uintptr addr = 0;

    extern uint32 iks_warp_calc(uintptr sec2_header_addr, uintptr sec3_header_addr, const amba_ik_warp_calc_t *p_info);

    iks_ctx_get_context(pMode0->ContextId, &p_ctx);
    iks_ctx_get_flow_control(pMode0->ContextId, 0, &addr);
    (void)amba_ik_system_memcpy(&p_flow, &addr, sizeof(void *));
    iks_ctx_get_flow_tbl(pMode0->ContextId, 0, &addr);
    (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        warp_calc.p_calib_warp_info = &warp_info;
        warp_calc.p_window_size_info = &window_size_info;
        warp_calc.p_result_win = &result_win;
        warp_calc.p_geo_warp_control = &geo_warp_control;
        warp_calc.p_stitching_info = &stitching_info;
        warp_calc.p_dmy_range = &dmy_range;
        warp_calc.p_warp_phase = &warp_phase;

        warp_calc.warp_enable = 1u;
        warp_info.vin_sensor_geo.h_sub_sample.factor_num = 1u;
        warp_info.vin_sensor_geo.h_sub_sample.factor_den = 1u;
        warp_info.vin_sensor_geo.v_sub_sample.factor_num = 1u;
        warp_info.vin_sensor_geo.v_sub_sample.factor_den = 1u;
        window_size_info.vin_sensor.h_sub_sample.factor_num = 1u;
        window_size_info.vin_sensor.h_sub_sample.factor_den = 1u;
        window_size_info.vin_sensor.v_sub_sample.factor_num = 1u;
        window_size_info.vin_sensor.v_sub_sample.factor_den = 1u;

        result_win.dmy_win_geo.width = 1920;
        result_win.dmy_win_geo.height = 1080;

        warp_info.vin_sensor_geo.start_x = 100;
        window_size_info.vin_sensor.start_x = 10;
        warp_info.vin_sensor_geo.start_y = 100;
        window_size_info.vin_sensor.start_y = 10;
        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "check_warp_valid_range_non_zero", "test invalid warp vin start offset fail case", id_cnt++);

        result_win.cfa_win_dim.width = 1920;
        window_size_info.main_win.width = 1920;
        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "check_warp_valid_range_non_zero", "test invalid warp vin win size fail case", id_cnt++);

        window_size_info.vin_sensor.h_sub_sample.factor_num = 0u;
        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "check_warp_valid_range", "test invalid warp SubSmp fail case", id_cnt++);

        window_size_info.vin_sensor.h_sub_sample.factor_num = 1u;
        window_size_info.vin_sensor.h_sub_sample.factor_den = 0u;
        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "check_warp_valid_range", "test invalid warp SubSmp fail case", id_cnt++);

        window_size_info.vin_sensor.h_sub_sample.factor_den = 1u;
        window_size_info.vin_sensor.v_sub_sample.factor_num = 0u;
        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "check_warp_valid_range", "test invalid warp SubSmp fail case", id_cnt++);

        window_size_info.vin_sensor.v_sub_sample.factor_num = 1u;
        window_size_info.vin_sensor.v_sub_sample.factor_den = 0u;
        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "check_warp_valid_range", "test invalid warp SubSmp fail case", id_cnt++);

        window_size_info.vin_sensor.v_sub_sample.factor_den = 1u;
        warp_info.vin_sensor_geo.h_sub_sample.factor_num = 0u;
        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "check_warp_valid_range", "test invalid warp SubSmp fail case", id_cnt++);

        warp_info.vin_sensor_geo.h_sub_sample.factor_num = 1u;
        warp_info.vin_sensor_geo.h_sub_sample.factor_den = 0u;
        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "check_warp_valid_range", "test invalid warp SubSmp fail case", id_cnt++);

        warp_info.vin_sensor_geo.h_sub_sample.factor_den = 1u;
        warp_info.vin_sensor_geo.v_sub_sample.factor_num = 0u;
        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "check_warp_valid_range", "test invalid warp SubSmp fail case", id_cnt++);

        warp_info.vin_sensor_geo.v_sub_sample.factor_num = 1u;
        warp_info.vin_sensor_geo.v_sub_sample.factor_den = 0u;
        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "check_warp_valid_range", "test invalid warp SubSmp fail case", id_cnt++);
        warp_info.vin_sensor_geo.v_sub_sample.factor_den = 1u;
    }

    {
        warp_info.vin_sensor_geo.h_sub_sample.factor_num = 1u;
        warp_info.vin_sensor_geo.h_sub_sample.factor_den = 1u;
        warp_info.vin_sensor_geo.v_sub_sample.factor_num = 1u;
        warp_info.vin_sensor_geo.v_sub_sample.factor_den = 1u;
        warp_info.vin_sensor_geo.start_x = 0;
        warp_info.vin_sensor_geo.start_y = 0;
        warp_info.vin_sensor_geo.width = 1920;
        warp_info.vin_sensor_geo.height = 1080;
        warp_info.hor_grid_num = 32;
        warp_info.ver_grid_num = 19;
        warp_info.tile_width_exp = 6;
        warp_info.tile_height_exp = 6;
        warp_info.pwarp = (ik_grid_point_t *)WarpGrid;

        window_size_info.vin_sensor.h_sub_sample.factor_num = 1u;
        window_size_info.vin_sensor.h_sub_sample.factor_den = 1u;
        window_size_info.vin_sensor.v_sub_sample.factor_num = 1u;
        window_size_info.vin_sensor.v_sub_sample.factor_den = 1u;
        window_size_info.vin_sensor.start_x = 0;
        window_size_info.vin_sensor.start_y = 0;
        window_size_info.vin_sensor.width = 1920;
        window_size_info.vin_sensor.height = 1080;
        window_size_info.main_win.width = 1920;
        window_size_info.main_win.height = 1080;

        result_win.act_win_crop.left_top_x = 0;
        result_win.act_win_crop.left_top_y = 0;
        result_win.act_win_crop.right_bot_x = 125829120;
        result_win.act_win_crop.right_bot_y = 70778880;
        result_win.cfa_win_dim.width = 1920;
        result_win.cfa_win_dim.height = 1080;

        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "img_dsp_core_calc_intpl_scale_offset", "test normal OK case", id_cnt++);

        warp_calc.flip_mode = IK_FLIP_RAW_H | IK_FLIP_RAW_V | IK_FLIP_YUV_V;
        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "img_dsp_core_calc_intpl_scale_offset", "test flip OK case", id_cnt++);
    }

    {
        uint32 sec2_vert_scale = 0UL;
        extern void iks_setup_warp_phase_size(const amba_ik_calc_win_result_t *p_result_win,
                                              amba_ik_warp_phase_t *p_warp_phase,
                                              const ik_window_size_info_t *p_window_size_info,
                                              const ik_dummy_margin_range_t *p_dmy_range,
                                              const uint32 flip_mode,
                                              uint32 *p_sec2_vert_scale);
        extern void iks_setup_warp_phase_size_internal(const amba_ik_calc_win_result_t *p_result_win,
                amba_ik_warp_phase_t *p_warp_phase,
                const ik_window_size_info_t *p_window_size_info,
                const uint32 zoom_y);

        result_win.act_win_crop.left_top_y = 1920 << 16;
        iks_setup_warp_phase_size(&result_win, &warp_phase, &window_size_info, &dmy_range, 0, &sec2_vert_scale);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitWarp.c", "iks_setup_warp_phase_size", "test if (act_prime_h_q16 > (p_window_size_info->main_win.height << SFT)) true OK case", id_cnt++);
        iks_setup_warp_phase_size_internal(&result_win, &warp_phase, &window_size_info, 65536);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitWarp.c", "iks_setup_warp_phase_size_internal", "test if (act_prime_h_q16 > (p_window_size_info->main_win.height<<SFT)) true OK case", id_cnt++);
        result_win.act_win_crop.left_top_y = 0;

        dmy_range.enable = 1u;
        iks_setup_warp_phase_size(&result_win, &warp_phase, &window_size_info, &dmy_range, 0, &sec2_vert_scale);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitWarp.c", "iks_setup_warp_phase_size", "test if(p_dmy_range->enable == 0u) false OK case", id_cnt++);
    }

    {
        vwarp_cfg_t vwarp_cfg = {0};
        extern void iks_setup_vwarp_dma(const amba_iks_filter_t *p_filters, vwarp_cfg_t *p_vwarp_cfg);

        p_ctx->filters.input_param.ctx_buf.warp_internal_mode_flag = 0;
        p_ctx->filters.input_param.warp_buf_info.luma_dma_size = 0;
        iks_setup_vwarp_dma(&p_ctx->filters, &vwarp_cfg);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitWarp.c", "iks_setup_vwarp_dma", "test if(p_filters->input_param.warp_buf_info.luma_dma_size != 0u) false OK case", id_cnt++);
    }

    {
        stitching_info.enable = 1;
        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "iks_warp_calc_set_warp_control", "test stitch enable OK case", id_cnt++);

        warp_calc.flip_mode = IK_FLIP_YUV_V;
        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "iks_warp_calc_set_warp_window", "test (dma_flip_v == 0U) false OK case", id_cnt++);

        stitching_info.enable = 0;
        warp_calc.is_group_cmd = 1;
        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "iks_warp_calc_preproc_horz_table", "test (p_info->is_group_cmd == 1u) true OK case", id_cnt++);

        result_win.cfa_win_dim.width = 5760;
        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "iks_warp_calc_set_hdrag", "test if(((((uint32)p_info->p_geo_warp_control->cfa_output_width >> 1UL) + 128UL) > stitch_threshold) true OK case", id_cnt++);

        stitching_info.enable = 1;
        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             &warp_calc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "iks_warp_calc_set_hdrag", "test (Stitch>0UL) true OK case", id_cnt++);

        Rval = iks_warp_calc(p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC2],
                             p_flow_tbl->sect_header_addr[0][SECT_CFG_INDEX_SEC3],
                             NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWarp.c", "iks_warp_calc_set_hdrag", "test NULL ptr fail case", id_cnt++);
    }

    {
        extern void iks_warp_internal(idsp_flow_ctrl_t *p_flow, set_warp_control_t *p_warp_cfg, amba_iks_filter_t *p_filters);

        p_flow->calib.warp_horizontal_table_address = (uint32)WarpGrid;
        p_flow->calib.warp_vertical_table_address = (uint32)(WarpGrid + 2*256*192);
        p_ctx->filters.input_param.warp_internal.pwarp_horizontal_table = (int16 *)WarpGrid;
        p_ctx->filters.input_param.warp_internal.pwarp_vertical_table = (int16 *)(WarpGrid + 2*256*192);

        p_ctx->filters.input_param.window_size_info.main_win.width = 1920;
        p_ctx->filters.input_param.window_size_info.main_win.height = 1080;
        p_ctx->filters.input_param.ctx_buf.result_win.act_win_crop.left_top_x = 0;
        p_ctx->filters.input_param.ctx_buf.result_win.act_win_crop.left_top_y = 0;
        p_ctx->filters.input_param.ctx_buf.result_win.act_win_crop.right_bot_x = 1920 << 16;
        p_ctx->filters.input_param.ctx_buf.result_win.act_win_crop.right_bot_y = 1080 << 16;
        p_ctx->filters.input_param.ctx_buf.result_win.cfa_win_dim.width = 1920;
        p_ctx->filters.input_param.ctx_buf.result_win.cfa_win_dim.height = 1080;
        p_ctx->filters.input_param.ctx_buf.result_win.dmy_win_geo.width = 1920;
        p_ctx->filters.input_param.ctx_buf.result_win.dmy_win_geo.height = 1080;

        p_ctx->filters.input_param.warp_internal.grid_array_width = 32;
        p_ctx->filters.input_param.warp_internal.grid_array_height = 19;
        p_ctx->filters.input_param.warp_internal.horz_grid_spacing_exponent = 6;
        p_ctx->filters.input_param.warp_internal.vert_grid_spacing_exponent = 6;
        p_ctx->filters.input_param.warp_internal.vert_warp_grid_array_width = 32;
        p_ctx->filters.input_param.warp_internal.vert_warp_grid_array_height = 19;
        p_ctx->filters.input_param.warp_internal.vert_warp_horzgrid_spacing_exponent = 6;
        p_ctx->filters.input_param.warp_internal.vert_warp_vert_grid_spacing_exponent = 6;

        p_ctx->filters.input_param.stitching_info.enable = 1;
        iks_warp_internal(p_flow, &geo_warp_control, &p_ctx->filters);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitWarp.c", "iks_warp_internal", "test stitch enable OK case", id_cnt++);

        p_ctx->filters.input_param.warp_internal.enable = 1;
        iks_warp_internal(p_flow, &geo_warp_control, &p_ctx->filters);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitWarp.c", "iks_warp_internal", "test stitch + warp enable OK case", id_cnt++);
    }
}

static void IKS_TestCovr_ImgExecuterUnitVignette(const AMBA_IK_MODE_CFG_s *pMode0)
{
    UINT32 Rval = IK_OK;
    vignette_table_bins_t vig_ext_table_bins = {0};
    vignette_center_t vig_ext_center = {0};
    amba_iks_context_entity_t *p_ctx;
    extern uint32 iks_vig_calc(vignette_table_bins_t *p_vig_ext_table_bins, vignette_center_t *p_vig_ext_center, const amba_iks_filter_t *p_filters);

    iks_ctx_get_context(pMode0->ContextId, &p_ctx);

    p_ctx->filters.input_param.window_size_info.vin_sensor.h_sub_sample.factor_num = 1;
    p_ctx->filters.input_param.window_size_info.vin_sensor.h_sub_sample.factor_den = 1;
    p_ctx->filters.input_param.window_size_info.vin_sensor.v_sub_sample.factor_num = 1;
    p_ctx->filters.input_param.window_size_info.vin_sensor.v_sub_sample.factor_den = 1;
    Rval = iks_vig_calc(&vig_ext_table_bins, &vig_ext_center, &p_ctx->filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitVignette.c", "get_shift_value", "test (factor == 1UL) OK case", id_cnt++);

    p_ctx->filters.input_param.window_size_info.vin_sensor.h_sub_sample.factor_den = 2;
    p_ctx->filters.input_param.window_size_info.vin_sensor.v_sub_sample.factor_den = 2;
    Rval = iks_vig_calc(&vig_ext_table_bins, &vig_ext_center, &p_ctx->filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitVignette.c", "get_shift_value", "test (factor == 2UL) OK case", id_cnt++);

    p_ctx->filters.input_param.window_size_info.vin_sensor.h_sub_sample.factor_den = 4;
    p_ctx->filters.input_param.window_size_info.vin_sensor.v_sub_sample.factor_den = 4;
    Rval = iks_vig_calc(&vig_ext_table_bins, &vig_ext_center, &p_ctx->filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitVignette.c", "get_shift_value", "test (factor == 4UL) OK case", id_cnt++);

    p_ctx->filters.input_param.window_size_info.vin_sensor.h_sub_sample.factor_den = 8;
    p_ctx->filters.input_param.window_size_info.vin_sensor.v_sub_sample.factor_den = 8;
    Rval = iks_vig_calc(&vig_ext_table_bins, &vig_ext_center, &p_ctx->filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitVignette.c", "get_shift_value", "test (factor == 8UL) OK case", id_cnt++);

    p_ctx->filters.input_param.window_size_info.vin_sensor.h_sub_sample.factor_den = 16;
    p_ctx->filters.input_param.window_size_info.vin_sensor.v_sub_sample.factor_den = 16;
    Rval = iks_vig_calc(&vig_ext_table_bins, &vig_ext_center, &p_ctx->filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitVignette.c", "get_shift_value", "test (factor == 16UL) OK case", id_cnt++);

    p_ctx->filters.input_param.window_size_info.vin_sensor.h_sub_sample.factor_den = 32;
    p_ctx->filters.input_param.window_size_info.vin_sensor.v_sub_sample.factor_den = 32;
    Rval = iks_vig_calc(&vig_ext_table_bins, &vig_ext_center, &p_ctx->filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitVignette.c", "get_shift_value", "test (factor == 32UL) OK case", id_cnt++);

    p_ctx->filters.input_param.window_size_info.vin_sensor.h_sub_sample.factor_den = 64;
    p_ctx->filters.input_param.window_size_info.vin_sensor.v_sub_sample.factor_den = 64;
    Rval = iks_vig_calc(&vig_ext_table_bins, &vig_ext_center, &p_ctx->filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitVignette.c", "get_shift_value", "test else OK case", id_cnt++);
}

static void IKS_TestCovr_ImgExecuterUnitStaticBadPixel(const AMBA_IK_MODE_CFG_s *pMode0)
{
    UINT32 Rval = IK_OK;
    ik_vin_sensor_geometry_t calib_geo = {0};
    ik_vin_sensor_geometry_t curr_geo = {0};
    uint32 offset;
    extern uint32 iks_sbp_map_offset(const ik_vin_sensor_geometry_t *p_calib_geo, const ik_vin_sensor_geometry_t *p_curr_geo, uint32 num_of_bit, uint32 *p_offset);

    calib_geo.h_sub_sample.factor_num = 1;
    calib_geo.h_sub_sample.factor_den = 1;
    calib_geo.v_sub_sample.factor_num = 1;
    calib_geo.v_sub_sample.factor_den = 1;

    curr_geo.start_x = 6;
    calib_geo.start_x = 0;
    Rval = iks_sbp_map_offset(&calib_geo, &curr_geo, 1, &offset);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitStaticBadPixel.c", "iks_sbp_map_offset", "test invalid offset_x NG case", id_cnt++);

    curr_geo.start_x = 0;
    calib_geo.start_x = 16;
    Rval = iks_sbp_map_offset(&calib_geo, &curr_geo, 1, &offset);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitStaticBadPixel.c", "iks_sbp_map_offset", "test invalid offset_x NG case", id_cnt++);
    calib_geo.start_x = 0;

    curr_geo.width = 100;
    Rval = iks_sbp_map_offset(&calib_geo, &curr_geo, 1, &offset);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitStaticBadPixel.c", "iks_sbp_map_offset", "test p_curr_geo->width not 32 align NG case", id_cnt++);
}

static void IKS_TestCovr_ImgExecuterUnitConfig(const AMBA_IK_MODE_CFG_s *pMode0)
{
    UINT32 Rval = IK_OK;
    ik_execute_container_t execute_container = {0};
    amba_iks_context_entity_t *p_ctx;
    idsp_flow_ctrl_t *p_flow = NULL;
    uintptr addr = 0;

    IKS_Executer_Set_ImgFilters(pMode0);
    Rval = iks_execute(pMode0->ContextId, &execute_container);

    iks_ctx_get_context(pMode0->ContextId, &p_ctx);
    iks_ctx_get_flow_control(pMode0->ContextId, 0, &addr);
    (void)amba_ik_system_memcpy(&p_flow, &addr, sizeof(void *));

    {
        extern uint32 iks_idsp_first_yuv(idsp_flow_ctrl_t *p_flow, const amba_iks_filter_t *p_filters);

        p_ctx->filters.update_flags.iso.chroma_filter_updated = 1;
        p_ctx->filters.update_flags.cr.luma_resmp = 1;
        p_ctx->filters.update_flags.cr.chroma_resmp = 1;
        p_ctx->filters.update_flags.cr.chroma_noise = 1;
        p_ctx->filters.update_flags.cr.vwarp = 1;

        p_ctx->filters.input_param.ctx_buf.iks_warp_phase.sec2_hscale_phase_inc = 65536;
        p_ctx->filters.input_param.chroma_filter.radius = 64;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_iso_cfg_check_raw2yuv_chroma_filter", "test sec2_chroma_hscale_phase_inc out of range NG case", id_cnt++);

        p_ctx->filters.input_param.ctx_buf.iks_warp_phase.sec2_hscale_phase_inc = 16384;
        p_ctx->filters.input_param.ctx_buf.iks_warp_phase.sec2_vscale_phase_inc = 65536;
        p_ctx->filters.input_param.chroma_filter.radius = 64;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_iso_cfg_check_raw2yuv_chroma_filter", "test sec2_chroma_vscale_phase_inc out of range NG case", id_cnt++);
        p_ctx->filters.input_param.ctx_buf.iks_warp_phase.sec2_vscale_phase_inc = 16384;

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));
    }

    {
        extern uint32 iks_idsp_first_hdr_ce(idsp_flow_ctrl_t *p_flow, amba_iks_filter_t *p_filters, uint32 exp_num);
        extern uint32 iks_idsp_first_hdr_ce_prep(idsp_flow_ctrl_t *p_flow, const amba_iks_filter_t *p_filters, uint32 exp_num);

        p_ctx->filters.update_flags.iso.hdr_blend_updated = 1;
        p_ctx->filters.update_flags.cr.hdr_blend = 1;

        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_hdr_blend", "test exp_num <=1 OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.fe_tone_curve_updated = 1;
        p_ctx->filters.update_flags.cr.hdr_decomp = 1;

        p_ctx->filters.input_param.fe_tone_curve.decompand_enable = 1;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_hdr_tone_curve", "test decompand_enable true OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.ce_updated = 1;
        p_ctx->filters.update_flags.cr.ce = 1;

        p_ctx->filters.input_param.sensor_info.sensor_pattern = 1;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_ce", "test sensor_pattern for ce luma avg. weight OK case", id_cnt++);
        p_ctx->filters.input_param.sensor_info.sensor_pattern = 2;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_ce", "test sensor_pattern for ce luma avg. weight OK case", id_cnt++);
        p_ctx->filters.input_param.sensor_info.sensor_pattern = 3;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_ce", "test sensor_pattern for ce luma avg. weight OK case", id_cnt++);
        p_ctx->filters.input_param.sensor_info.sensor_pattern = 4;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_ce", "test sensor_pattern for ce luma avg. weight OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.sensor_information_updated = 1;

        p_ctx->filters.input_param.sensor_info.compression = 300;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_decompress", "test sensor_info.compression >= 256 OK case", id_cnt++);
        p_ctx->filters.input_param.sensor_info.compression = 200;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_decompress", "test sensor_info.compression < 256 OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.exp0_fe_static_blc_updated = 1;
        p_ctx->filters.update_flags.cr.vin_stat_exp0 = 1;

        p_ctx->filters.input_param.exp0_frontend_wb_gain.shutter_ratio = 0;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 2);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_hdr_exp", "test shutter_ratio == 0 OK case", id_cnt++);
        p_ctx->filters.input_param.exp0_frontend_wb_gain.shutter_ratio = 1;

        p_ctx->filters.input_param.sensor_info.compression = 1;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_hdr_exp", "test 0 < compression < 9 OK case", id_cnt++);

        p_ctx->filters.input_param.rgb_ir.ir_only = 1;
        p_ctx->filters.input_param.sensor_info.sensor_mode = 0;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_hdr_exp", "test (p_filters->input_param.sensor_info.sensor_mode==1U) false OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.vignette_compensation_updated = 1;
        p_ctx->filters.update_flags.cr.hdr_blend_b = 1;
        p_ctx->filters.update_flags.cr.hdr_decomp_b = 1;
        p_ctx->filters.update_flags.cr.dgain_vig_b = 1;
        p_ctx->filters.update_flags.cr.vig_mult_b = 1;
        p_ctx->filters.update_flags.cr.ce_fe = 1;
        p_ctx->filters.update_flags.cr.dbp_b = 1;
        p_ctx->filters.update_flags.cr.sbp_grgb_b = 1;
        p_ctx->filters.input_param.vig_enable = 1;

        p_ctx->filters.input_param.sensor_info.sensor_pattern = 1;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_ce_fe", "test sensor_pattern for ce_fe luma avg. weight OK case", id_cnt++);
        p_ctx->filters.input_param.sensor_info.sensor_pattern = 2;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_ce_fe", "test sensor_pattern for ce_fe luma avg. weight OK case", id_cnt++);
        p_ctx->filters.input_param.sensor_info.sensor_pattern = 3;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_ce_fe", "test sensor_pattern for ce_fe luma avg. weight OK case", id_cnt++);
        p_ctx->filters.input_param.sensor_info.sensor_pattern = 4;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_ce_fe", "test sensor_pattern for ce_fe luma avg. weight OK case", id_cnt++);

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));
    }

    {
        extern uint32 iks_idsp_first_calib(idsp_flow_ctrl_t *p_flow, amba_iks_filter_t *p_filters);

        p_ctx->filters.update_flags.iso.is_1st_frame = 1;
        p_ctx->filters.update_flags.cr.luma_resmp = 1;
        p_ctx->filters.update_flags.cr.hwarp_map = 1;
        p_ctx->filters.update_flags.cr.chroma_resmp = 1;
        p_ctx->filters.update_flags.cr.vwarp = 1;
        p_ctx->filters.update_flags.cr.vwarp_map = 1;
        p_ctx->filters.update_flags.cr.vwarp_me0 = 1;

        p_ctx->filters.input_param.flip_mode = IK_FLIP_RAW_H | IK_FLIP_RAW_V;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_sec2_bayer_pattern", "test H and V flip OK case", id_cnt++);

        p_ctx->filters.input_param.sensor_info.compression = 10;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_set_vin_sensor", "test 0 < compression < 9 false OK case", id_cnt++);

        p_ctx->filters.input_param.rgb_ir.mode = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_set_vin_sensor", "test incorrect sensor_mode for rgbir NG case", id_cnt++);

        p_ctx->filters.input_param.sensor_info.sensor_mode = IK_SENSOR_MODE_RGBIR;
        p_ctx->filters.input_param.rgb_ir.mode = 0;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_set_vin_sensor", "test incorrect sensor_mode for rgbir NG case", id_cnt++);

        p_ctx->filters.input_param.sensor_info.sensor_mode = IK_SENSOR_MODE_RCCC;
        p_ctx->filters.input_param.rgb_ir.mode = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_set_vin_sensor", "test incorrect sensor_mode for rgbir NG case", id_cnt++);

        p_ctx->filters.input_param.rgb_ir.mode = 0;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_set_vin_sensor", "test sensor_mode for rgbir OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.vignette_compensation_updated = 1;
        p_ctx->filters.update_flags.cr.dgain_vig = 1;
        p_ctx->filters.update_flags.cr.vig_mult = 1;
        p_ctx->filters.update_flags.cr.dbp = 1;

        p_ctx->filters.input_param.vig_enable = 1;
        p_ctx->filters.input_param.vignette_compensation.calib_mode_enable = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_vig", "test vig calib_mode_enable OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.chroma_filter_updated = 1;

        p_ctx->filters.input_param.ctx_buf.warp_internal_mode_flag = 1;
        p_ctx->filters.input_param.warp_buf_info.dram_efficiency = 2;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_warp_radius", "test internal mode dram_efficiency == 2 OK case", id_cnt++);

        p_ctx->filters.input_param.ctx_buf.warp_internal_mode_flag = 0;
        p_ctx->filters.input_param.warp_buf_info.luma_wait_lines = 0;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_warp_radius", "test normal mode wait_line == 0 default OK case", id_cnt++);

        p_ctx->filters.input_param.chroma_filter.radius = 32;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_warp_radius", "test normal mode different wait_line default value OK case", id_cnt++);

        p_ctx->filters.input_param.chroma_filter.radius = 128;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_warp_radius", "test normal mode different wait_line default value OK case", id_cnt++);

        p_ctx->filters.input_param.chroma_filter.radius = 256;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_warp_radius", "test normal mode different wait_line default value OK case", id_cnt++);

        p_ctx->filters.input_param.warp_buf_info.dram_efficiency = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_warp_radius", "test normal mode dram_efficiency == 1 wait_line default value OK case", id_cnt++);

        p_ctx->filters.input_param.yuv_mode = 0;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_warp_radius", "test yuv_mode = 0 but ability is not Y2Y OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.window_size_info_updated = 1;

        p_ctx->filters.update_flags.cr.cawarp = 1;
        p_ctx->filters.update_flags.cr.cfa_prescale = 1;
        p_ctx->filters.update_flags.cr.ca_tbl = 1;

        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sbp", "test _liso_idsp_first_set_sbp ring buf error NG case", id_cnt++);
        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.iso.dynamic_bpc_updated = 1;
        p_ctx->filters.update_flags.cr.sbp_grgb = 1;

        p_ctx->filters.input_param.sbp_enable = 1;
        p_ctx->filters.input_param.window_size_info.vin_sensor.width = 100;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sbp", "test iks_sbp_map_offset != 0 OK case", id_cnt++);

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.input_param.window_size_info.vin_sensor.width = 1920;
        p_ctx->filters.input_param.window_size_info.vin_sensor.height = 1080;
        p_ctx->filters.input_param.ctx_buf.result_win.act_win_crop.left_top_x = 0;
        p_ctx->filters.input_param.ctx_buf.result_win.act_win_crop.left_top_y = 0;
        p_ctx->filters.input_param.ctx_buf.result_win.act_win_crop.right_bot_x = 1920 << 16;
        p_ctx->filters.input_param.ctx_buf.result_win.act_win_crop.right_bot_y = 1080 << 16;
        p_ctx->filters.input_param.dmy_range.enable = 0;

        p_ctx->filters.update_flags.cr.sbp_grgb = 1;
        p_ctx->filters.update_flags.cr.cawarp = 1;
        p_ctx->filters.update_flags.cr.cfa_prescale = 1;
        p_ctx->filters.update_flags.cr.ca_tbl = 1;
        p_ctx->filters.update_flags.cr.warp = 1;
        p_ctx->filters.update_flags.cr.luma_resmp = 1;
        p_ctx->filters.update_flags.cr.hwarp_map = 1;
        p_ctx->filters.update_flags.cr.chroma_resmp = 1;
        p_ctx->filters.update_flags.cr.vwarp = 1;
        p_ctx->filters.update_flags.cr.vwarp_map = 1;
        p_ctx->filters.update_flags.cr.vwarp_me0 = 1;

        p_ctx->filters.update_flags.iso.window_size_info_updated = 1;

        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_cfa_calib", "test _liso_idsp_first_set_vig ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.dgain_vig = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_cfa_calib", "test _liso_idsp_first_set_vig ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.vig_mult = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_cfa_calib", "test _liso_idsp_first_set_vig ring buf error NG case", id_cnt++);
        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.iso.vignette_compensation_updated = 1;
        p_ctx->filters.update_flags.cr.dgain_vig = 1;
        p_ctx->filters.update_flags.cr.vig_mult = 1;
        p_ctx->filters.update_flags.cr.dbp = 1;
        p_ctx->filters.crc_check_list.dgain_vig = 0;
        p_ctx->filters.crc_check_list.vig_mult = 0;
        p_ctx->filters.crc_check_list.dbp = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_cfa_calib", "test p_filters->crc_check_list.dbp true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.vig_mult = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_cfa_calib", "test p_filters->crc_check_list.dbp true OK case", id_cnt++);
        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.cr.sbp_grgb = 1;
        p_ctx->filters.update_flags.cr.dgain_vig = 1;
        p_ctx->filters.update_flags.cr.vig_mult = 1;
        p_ctx->filters.update_flags.cr.dbp = 1;
        p_ctx->filters.update_flags.cr.warp = 1;
        p_ctx->filters.update_flags.cr.luma_resmp = 1;
        p_ctx->filters.update_flags.cr.hwarp_map = 1;
        p_ctx->filters.update_flags.cr.chroma_resmp = 1;
        p_ctx->filters.update_flags.cr.vwarp = 1;
        p_ctx->filters.update_flags.cr.vwarp_map = 1;
        p_ctx->filters.update_flags.cr.vwarp_me0 = 1;

        p_ctx->filters.update_flags.iso.calib_ca_warp_info_updated = 1;
        p_ctx->filters.update_flags.cr.cawarp = 1;
        p_ctx->filters.update_flags.cr.cfa_prescale = 1;
        p_ctx->filters.update_flags.cr.ca_tbl = 0;

        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_cfa_calib", "test _liso_idsp_first_set_ca ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.cawarp = 0;
        p_ctx->filters.update_flags.cr.cfa_prescale = 0;
        p_ctx->filters.update_flags.cr.ca_tbl = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_cfa_calib", "test _liso_idsp_first_set_ca ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.cawarp = 1;
        p_ctx->filters.update_flags.cr.cfa_prescale = 0;
        p_ctx->filters.update_flags.cr.ca_tbl = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_cfa_calib", "test _liso_idsp_first_set_ca ring buf error NG case", id_cnt++);
        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.iso.calib_ca_warp_info_updated = 1;
        p_ctx->filters.update_flags.cr.cawarp = 1;
        p_ctx->filters.update_flags.cr.cfa_prescale = 1;
        p_ctx->filters.update_flags.cr.ca_tbl = 1;
        p_ctx->filters.crc_check_list.cawarp = 0;
        p_ctx->filters.crc_check_list.cfa_prescale = 0;
        p_ctx->filters.crc_check_list.ca_tbl = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_cfa_calib", "test p_filters->crc_check_list.cawarp true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.cfa_prescale = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_cfa_calib", "test p_filters->crc_check_list.cawarp true OK case", id_cnt++);
        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.iso.calib_warp_info_updated = 1;
        p_ctx->filters.update_flags.iso.warp_buf_info_updated = 1;
        p_ctx->filters.update_flags.cr.luma_resmp = 1;
        p_ctx->filters.update_flags.cr.hwarp_map = 1;
        p_ctx->filters.update_flags.cr.chroma_resmp = 1;
        p_ctx->filters.update_flags.cr.vwarp = 1;
        p_ctx->filters.update_flags.cr.vwarp_map = 1;
        p_ctx->filters.update_flags.cr.vwarp_me0 = 0;

        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_warp_radius", "test _liso_idsp_first_set_warp_radius ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.hwarp_map = 0;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_warp_radius", "test _liso_idsp_first_set_warp_radius ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.hwarp_map = 1;
        p_ctx->filters.update_flags.cr.chroma_resmp = 0;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_warp_radius", "test _liso_idsp_first_set_warp_radius ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.chroma_resmp = 1;
        p_ctx->filters.update_flags.cr.vwarp = 0;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_warp_radius", "test _liso_idsp_first_set_warp_radius ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.vwarp = 1;
        p_ctx->filters.update_flags.cr.vwarp_map = 0;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_warp_radius", "test _liso_idsp_first_set_warp_radius ring buf error NG case", id_cnt++);
        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.iso.window_size_info_updated = 1;
        p_ctx->filters.update_flags.iso.calib_warp_info_updated = 1;
        p_ctx->filters.update_flags.cr.warp = 1;
        p_ctx->filters.update_flags.cr.luma_resmp = 1;
        p_ctx->filters.update_flags.cr.hwarp_map = 1;
        p_ctx->filters.update_flags.cr.chroma_resmp = 1;
        p_ctx->filters.update_flags.cr.vwarp = 1;
        p_ctx->filters.update_flags.cr.vwarp_map = 1;
        p_ctx->filters.update_flags.cr.vwarp_me0 = 1;
        p_ctx->filters.update_flags.cr.sbp_grgb = 1;
        p_ctx->filters.update_flags.cr.dgain_vig = 1;
        p_ctx->filters.update_flags.cr.vig_mult = 1;
        p_ctx->filters.update_flags.cr.dbp = 1;
        p_ctx->filters.update_flags.cr.cawarp = 1;
        p_ctx->filters.update_flags.cr.cfa_prescale = 1;
        p_ctx->filters.update_flags.cr.ca_tbl = 1;

        p_ctx->filters.input_param.window_size_info.vin_sensor.width = 1920;
        p_ctx->filters.input_param.window_size_info.vin_sensor.height = 1080;
        p_ctx->filters.input_param.ctx_buf.result_win.act_win_crop.left_top_x = 0;
        p_ctx->filters.input_param.ctx_buf.result_win.act_win_crop.left_top_y = 0;
        p_ctx->filters.input_param.ctx_buf.result_win.act_win_crop.right_bot_x = 1920 << 16;
        p_ctx->filters.input_param.ctx_buf.result_win.act_win_crop.right_bot_y = 1080 << 16;
        p_ctx->filters.input_param.dmy_range.enable = 0;

        p_ctx->filters.crc_check_list.luma_resmp = 0;
        p_ctx->filters.crc_check_list.hwarp_map = 0;
        p_ctx->filters.crc_check_list.chroma_resmp = 0;
        p_ctx->filters.crc_check_list.vwarp = 0;
        p_ctx->filters.crc_check_list.vwarp_map = 0;
        p_ctx->filters.crc_check_list.vwarp_me0 = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_warp_radius", "test p_filters->crc_check_list.warp true OK case", id_cnt++);

        p_ctx->filters.crc_check_list.luma_resmp = 0;
        p_ctx->filters.crc_check_list.hwarp_map = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_warp_radius", "test p_filters->crc_check_list.warp true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.hwarp_map = 0;
        p_ctx->filters.crc_check_list.chroma_resmp = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_warp_radius", "test p_filters->crc_check_list.warp true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.chroma_resmp = 0;
        p_ctx->filters.crc_check_list.vwarp = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_warp_radius", "test p_filters->crc_check_list.warp true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.vwarp = 0;
        p_ctx->filters.crc_check_list.vwarp_map = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_warp_radius", "test p_filters->crc_check_list.warp true OK case", id_cnt++);

        p_ctx->filters.input_param.window_size_info.main_win.height = 96;
        p_ctx->filters.input_param.chroma_filter.radius = 128;
#ifdef EARLYTEST_ENV
        p_ctx->filters.input_param.warp_buf_info.luma_dma_size = 64;
        p_ctx->filters.input_param.warp_buf_info.chroma_dma_size = 64;
#else
        p_ctx->filters.input_param.warp_buf_info.luma_dma_size = 128;
        p_ctx->filters.input_param.warp_buf_info.reserved = 64;
#endif
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_warp_radius", "test if (p_flow->window.height_chroma < (uint16)(vwarp_cfg.dma_size_uv*2UL)) true OK case", id_cnt++);
        p_ctx->filters.input_param.yuv_mode = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_warp_radius", "test if (p_flow->window.height_chroma < (uint16)(vwarp_cfg.dma_size_uv*2UL)) true OK case", id_cnt++);

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.iso.calib_warp_info_updated = 1;
        p_ctx->filters.update_flags.cr.warp = 1;
        p_ctx->filters.update_flags.cr.luma_resmp = 1;
        p_ctx->filters.update_flags.cr.hwarp_map = 1;
        p_ctx->filters.update_flags.cr.chroma_resmp = 1;
        p_ctx->filters.update_flags.cr.vwarp = 1;
        p_ctx->filters.update_flags.cr.vwarp_map = 1;
        p_ctx->filters.update_flags.cr.vwarp_me0 = 0;

        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_calib", "test _liso_idsp_first_set_warp ring buf error NG case", id_cnt++);

        p_ctx->filters.update_flags.cr.warp = 0;
        p_ctx->filters.update_flags.iso.calib_warp_info_updated = 0;
        p_ctx->filters.update_flags.iso.warp_internal_updated = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_calib", "test _liso_idsp_first_set_warp ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.warp = 1;
        p_ctx->filters.update_flags.iso.calib_warp_info_updated = 1;

        p_ctx->filters.update_flags.cr.luma_resmp = 0;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_calib", "test _liso_idsp_first_set_warp ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.luma_resmp = 1;
        p_ctx->filters.update_flags.cr.hwarp_map = 0;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_calib", "test _liso_idsp_first_set_warp ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.hwarp_map = 1;
        p_ctx->filters.update_flags.cr.chroma_resmp = 0;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_calib", "test _liso_idsp_first_set_warp ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.chroma_resmp = 1;
        p_ctx->filters.update_flags.cr.vwarp = 0;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_calib", "test _liso_idsp_first_set_warp ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.vwarp = 1;
        p_ctx->filters.update_flags.cr.vwarp_map = 0;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_calib", "test _liso_idsp_first_set_warp ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.vwarp_map = 1;
        p_ctx->filters.update_flags.cr.vwarp_me0 = 1;

        p_ctx->filters.crc_check_list.warp = 0;
        p_ctx->filters.crc_check_list.luma_resmp = 0;
        p_ctx->filters.crc_check_list.hwarp_map = 0;
        p_ctx->filters.crc_check_list.chroma_resmp = 0;
        p_ctx->filters.crc_check_list.vwarp = 0;
        p_ctx->filters.crc_check_list.vwarp_map = 0;
        p_ctx->filters.crc_check_list.vwarp_me0 = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_calib", "test p_filters->crc_check_list.vwarp_me0 true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.luma_resmp = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_calib", "test p_filters->crc_check_list.warp true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.luma_resmp = 0;
        p_ctx->filters.crc_check_list.hwarp_map = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_calib", "test p_filters->crc_check_list.warp true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.hwarp_map = 0;
        p_ctx->filters.crc_check_list.chroma_resmp = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_calib", "test p_filters->crc_check_list.warp true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.chroma_resmp = 0;
        p_ctx->filters.crc_check_list.vwarp = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_calib", "test p_filters->crc_check_list.warp true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.vwarp = 0;
        p_ctx->filters.crc_check_list.vwarp_map = 1;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_calib", "test p_filters->crc_check_list.warp true OK case", id_cnt++);

        memset(&p_ctx->filters.crc_check_list, 0xff, sizeof(amba_iks_crc_check_list_t));

        p_ctx->filters.input_param.warp_enable = 0;
        p_ctx->filters.input_param.dmy_range.enable = 1;
        p_ctx->filters.input_param.ctx_buf.result_win.act_win_crop.left_top_y = 131072;
        p_ctx->filters.input_param.ctx_buf.result_win.act_win_crop.right_bot_y = 131072 + (1080 << 16);
        p_ctx->filters.input_param.ctx_buf.result_win.cfa_win_dim.height = 1080;
        p_ctx->filters.input_param.ctx_buf.result_win.dmy_win_geo.height = 1080;
        Rval = iks_idsp_first_calib(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_calib", "test p_filters->crc_check_list.warp true OK case", id_cnt++);
        p_ctx->filters.input_param.dmy_range.enable = 0;
        p_ctx->filters.input_param.ctx_buf.result_win.act_win_crop.left_top_y = 0;
        p_ctx->filters.input_param.ctx_buf.result_win.act_win_crop.right_bot_y = 1080 << 16;

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));
    }

    {
        extern uint32 iks_idsp_first_cfa(idsp_flow_ctrl_t *p_flow, const amba_iks_filter_t *p_filters);

        p_ctx->filters.update_flags.iso.before_ce_wb_gain_updated = 1;
        p_ctx->filters.update_flags.cr.ce = 1;

        p_ctx->filters.input_param.before_ce_wb_gain.gain_r = 1024;
        p_ctx->filters.input_param.before_ce_wb_gain.gain_g = 1024;
        p_ctx->filters.input_param.before_ce_wb_gain.gain_b = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_before_ce_gain", "test before_ce_wb_gain == 0 NG case", id_cnt++);

        p_ctx->filters.input_param.before_ce_wb_gain.gain_g = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_before_ce_gain", "test before_ce_wb_gain == 0 NG case", id_cnt++);

        p_ctx->filters.input_param.before_ce_wb_gain.gain_r = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_before_ce_gain", "test before_ce_wb_gain == 0 NG case", id_cnt++);
        p_ctx->filters.input_param.before_ce_wb_gain.gain_r = 1024;
        p_ctx->filters.input_param.before_ce_wb_gain.gain_g = 1024;
        p_ctx->filters.input_param.before_ce_wb_gain.gain_b = 1024;

        p_ctx->filters.update_flags.iso.demosaic_updated = 1;
        p_ctx->filters.update_flags.cr.sbp_grgb = 1;
        p_ctx->filters.update_flags.cr.cfa_noise = 1;
        p_ctx->filters.update_flags.cr.demosaic = 1;

        p_ctx->filters.input_param.demosaic.alias_interpolation_strength = 38;
        p_ctx->filters.input_param.demosaic.alias_interpolation_thresh = 10;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_thresh_mono_grad", "test interpolation_strength > 0 OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.rgb_ir_updated = 1;
        p_ctx->filters.update_flags.cr.hdr_blend = 1;
        p_ctx->filters.update_flags.cr.vin_stat_exp0 = 1;
        p_ctx->filters.update_flags.cr.vin_stat_exp1 = 1;
        p_ctx->filters.update_flags.cr.vin_stat_exp2 = 1;
        p_ctx->filters.update_flags.cr.dgain_vig = 1;
        p_ctx->filters.update_flags.cr.dbp = 1;
        p_ctx->filters.update_flags.cr.sbp_grgb = 1;
        p_ctx->filters.update_flags.cr.aaa_cfa = 1;

        p_ctx->filters.input_param.before_ce_wb_gain.gain_r = 16777215;
        p_ctx->filters.input_param.before_ce_wb_gain.gain_g = 16777215;
        p_ctx->filters.input_param.before_ce_wb_gain.gain_b = 16777215;
        p_ctx->filters.input_param.after_ce_wb_gain.gain_r = 16777215;
        p_ctx->filters.input_param.after_ce_wb_gain.gain_g = 16777215;
        p_ctx->filters.input_param.after_ce_wb_gain.gain_b = 16777215;
        p_ctx->filters.input_param.exp0_frontend_wb_gain.r_gain = 0xFFFFFFFF;
        p_ctx->filters.input_param.exp0_frontend_wb_gain.g_gain = 0xFFFFFFFF;
        p_ctx->filters.input_param.exp0_frontend_wb_gain.b_gain = 0xFFFFFFFF;
        p_ctx->filters.input_param.exp0_frontend_wb_gain.ir_gain = 1024;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_calc_ir_tot", "test gain overflow protection OK case", id_cnt++);

        p_ctx->filters.input_param.rgb_ir.mode = 1;
        p_ctx->filters.input_param.flip_mode = IK_FLIP_RAW_H | IK_FLIP_RAW_V;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_rgbir", "test flip OK case", id_cnt++);
        p_ctx->filters.input_param.flip_mode = IK_FLIP_RAW_H;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_rgbir", "test flip OK case", id_cnt++);
        p_ctx->filters.input_param.flip_mode = IK_FLIP_RAW_V;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_set_rgbir", "test flip OK case", id_cnt++);
        p_ctx->filters.input_param.rgb_ir.mode = 0;
        p_ctx->filters.input_param.flip_mode = 0;

        p_ctx->filters.update_flags.iso.resample_str_update = 1;
        p_ctx->filters.update_flags.cr.cfa_prescale = 1;

        p_ctx->filters.input_param.cawarp_enable = 0;
        p_ctx->filters.input_param.ctx_buf.result_win.dmy_win_geo.width = 1920;
        p_ctx->filters.input_param.ctx_buf.result_win.dmy_win_geo.height = 1080;
        p_ctx->filters.input_param.ctx_buf.result_win.cfa_win_dim.width = 1920;
        p_ctx->filters.input_param.ctx_buf.result_win.cfa_win_dim.height = 1080;
        p_flow->flow_info.sensor_readout_mode_horz = 0xf;
        p_flow->flow_info.sensor_readout_mode_vert = 0xf;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_cfa_prescale", "test cfa downsample OK case", id_cnt++);
        p_ctx->filters.input_param.ctx_buf.result_win.dmy_win_geo.height = 2160;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_cfa_prescale", "test cfa downsample OK case", id_cnt++);
        p_ctx->filters.input_param.ctx_buf.result_win.dmy_win_geo.width = 3840;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_cfa_prescale", "test cfa downsample OK case", id_cnt++);
        p_ctx->filters.input_param.ctx_buf.result_win.dmy_win_geo.width = 1920;
        p_ctx->filters.input_param.ctx_buf.result_win.dmy_win_geo.height = 1080;
        p_flow->flow_info.sensor_readout_mode_horz = 0;
        p_flow->flow_info.sensor_readout_mode_vert = 0;

        p_ctx->filters.update_flags.iso.anti_aliasing_updated = 1;
        p_ctx->filters.update_flags.cr.sbp_grgb = 1;

        p_ctx->filters.input_param.anti_aliasing.enable = 2;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_anti_aliasing", "test different anti_aliasing.enable OK case", id_cnt++);
        p_ctx->filters.input_param.anti_aliasing.enable = 3;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_anti_aliasing", "test different anti_aliasing.enable OK case", id_cnt++);
        p_ctx->filters.input_param.anti_aliasing.enable = 4;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_anti_aliasing", "test different anti_aliasing.enable OK case", id_cnt++);
        p_ctx->filters.input_param.anti_aliasing.enable = 5;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_anti_aliasing", "test anti_aliasing.enable out of range NG case", id_cnt++);
        p_ctx->filters.input_param.anti_aliasing.enable = 2;

        p_ctx->filters.update_flags.iso.dynamic_bpc_updated = 1;
        p_ctx->filters.update_flags.cr.dbp = 1;

        p_ctx->filters.input_param.dynamic_bpc.correction_method = 1;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_DBP", "test different dynamic_bad_pixel_correction_method OK case", id_cnt++);

        p_ctx->filters.input_param.dynamic_bpc.hot_pixel_strength = 20;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_DBP", "test (hot_strength <= 10U) false OK case", id_cnt++);

        p_ctx->filters.input_param.dynamic_bpc.hot_pixel_strength = 10;
        p_ctx->filters.input_param.dynamic_bpc.dark_pixel_strength = 20;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_DBP", "test (dark_strength <= 10U) false OK case", id_cnt++);

        p_ctx->filters.input_param.dynamic_bpc.hot_pixel_strength = 0;
        p_ctx->filters.input_param.dynamic_bpc.dark_pixel_strength = 10;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_DBP", "test (dark_strength == 0U) false OK case", id_cnt++);

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));
    }

    {
        extern uint32 iks_idsp_first_rgb(idsp_flow_ctrl_t *p_flow, amba_iks_filter_t *p_filters);

        p_ctx->filters.update_flags.iso.color_correction_updated = 1;
        p_ctx->filters.update_flags.cr.cc = 1;
        p_ctx->filters.update_flags.cr.cc_3d = 1;

        p_ctx->filters.input_param.rgb_ir.ir_only = 1;
        p_ctx->filters.input_param.sensor_info.sensor_mode = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_cc_3d", "test sensor_info.sensor_mode == 0 OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.rgb_to_yuv_matrix_updated = 1;
        p_ctx->filters.update_flags.cr.rgb_2_yuv = 1;

        p_ctx->filters.input_param.rgb_ir.ir_only = 1;
        p_ctx->filters.input_param.sensor_info.sensor_mode = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_rgb2yuv", "test sensor_info.sensor_mode == 0 OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.demosaic_updated = 1;
        p_ctx->filters.update_flags.cr.demosaic = 1;

        p_ctx->filters.input_param.demosaic.alias_interpolation_strength = 38;
        p_ctx->filters.input_param.demosaic.alias_interpolation_thresh = 10;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_demosaic", "test interpolation_strength > 0 OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.rgb_to_12y_updated = 1;
        p_ctx->filters.update_flags.cr.rgb_12y = 1;

        p_ctx->filters.input_param.rgb_to_12y.enable = 1;
        p_ctx->filters.input_param.rgb_to_12y.y_offset = 0;
        p_ctx->filters.input_param.rgb_to_12y.matrix_values[0] = 4000;
        p_ctx->filters.input_param.rgb_to_12y.matrix_values[1] = 0;
        p_ctx->filters.input_param.rgb_to_12y.matrix_values[2] = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_rgb_2_12y", "test while((dif > 3) && (count<100)) false OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.pre_cc_gain_updated = 1;
        p_ctx->filters.update_flags.cr.cc = 1;

        p_ctx->filters.input_param.pre_cc_gain.enable = 1;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_rgb", "test if (p_filters->input_param.pre_cc_gain.enable == 1u) true OK case", id_cnt++);

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));
    }

    {
        extern uint32 iks_idsp_first_yuv(idsp_flow_ctrl_t *p_flow, const amba_iks_filter_t *p_filters);

        p_ctx->filters.update_flags.iso.chroma_scale_updated = 1;
        p_ctx->filters.update_flags.cr.rgb_2_yuv = 1;

        p_ctx->filters.input_param.chroma_scale.enable = 2;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_chroma_scale", "test different chroma_scale.weight OK case", id_cnt++);
        p_ctx->filters.input_param.chroma_scale.enable = 3;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_chroma_scale", "test different chroma_scale.weight OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.chroma_median_filter_updated = 1;
        p_ctx->filters.update_flags.cr.luma_shp = 1;
        p_ctx->filters.update_flags.cr.chroma_median = 1;

        p_ctx->filters.input_param.chroma_median_filter.enable = 1;
        p_ctx->filters.input_param.chroma_median_filter.cb_adaptive_strength = 256;
        p_ctx->filters.input_param.chroma_median_filter.cr_adaptive_strength = 256;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_chroma_median_filter", "test if (index_lo == 3UL) true OK case", id_cnt++);
        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_Y2Y;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_chroma_median_filter", "test if (p_filters->input_param.ability == AMBA_IK_VIDEO_Y2Y) true OK case", id_cnt++);
        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_LINEAR_CE;

        p_ctx->filters.update_flags.iso.chroma_filter_updated = 1;
        p_ctx->filters.update_flags.cr.chroma_noise = 1;
        p_ctx->filters.update_flags.cr.luma_resmp = 1;
        p_ctx->filters.update_flags.cr.chroma_resmp = 1;
        p_ctx->filters.update_flags.cr.vwarp = 1;

        p_ctx->filters.input_param.chroma_filter.radius = 32;
        p_ctx->filters.input_param.ctx_buf.iks_warp_phase.sec2_vscale_phase_inc = 8192;
        p_ctx->filters.input_param.chroma_filter.original_blend_strength_cb = 16;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "getLog2", "test while ((1UL << k) <= a) true OK case", id_cnt++);

        p_ctx->filters.input_param.wide_chroma_filter.enable = 1;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_chroma_noise_filter", "test enable wide filter when chroma_radius is 32 NG case", id_cnt++);
        p_ctx->filters.input_param.wide_chroma_filter.enable = 0;

        p_ctx->filters.update_flags.iso.advance_spatial_filter_updated = 1;
        p_ctx->filters.update_flags.cr.luma_shp = 1;

        p_ctx->filters.input_param.first_luma_process_mode.use_sharpen_not_asf = 0;
        p_ctx->filters.input_param.advance_spatial_filter.enable = 1;
        p_ctx->filters.update_flags.iso.first_luma_processing_mode_updated = 0;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_luma_proc_asf", "test if(p_filters->update_flags.iso.first_luma_processing_mode_updated == 1U) false OK case", id_cnt++);

        p_ctx->filters.input_param.first_luma_process_mode.use_sharpen_not_asf = 1;
        p_ctx->filters.input_param.first_sharpen_both.enable = 0;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_sharpen_control", "test if(p_sharpen_both->enable == 0U) true OK case", id_cnt++);

        p_ctx->filters.input_param.first_sharpen_both.mode = 1;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_luma_proc_sharpen", "test if (input_shp.sharpen_both->mode == 2U) false OK case", id_cnt++);

        p_ctx->filters.input_param.first_sharpen_both.mode = 2;
        p_ctx->filters.input_param.first_sharpen_noise.level_str_adjust_not_t0_t1_level_based = 0;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_luma_proc_sharpen", "test if(input_shp.sharpen_noise->level_str_adjust_not_t0_t1_level_based==0U) true OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.window_size_info_updated = 1;
        p_ctx->filters.update_flags.cr.luma_resmp = 1;
        p_ctx->filters.update_flags.cr.chroma_resmp = 1;
        p_ctx->filters.update_flags.cr.chroma_noise = 1;
        p_ctx->filters.update_flags.cr.vwarp = 1;

        p_ctx->filters.input_param.yuv_mode = 0;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_main_resampler", "test non-YUV420 OK case", id_cnt++);

        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_Y2Y;
        p_ctx->filters.input_param.ctx_buf.iks_warp_phase.sec2_output.height = 96;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_main_resampler", "test YUV420 if (output_height < (chroma_dma_size << 1u)) true OK case", id_cnt++);
        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_LINEAR_CE;

        p_ctx->filters.input_param.burst_tile.enable = 1;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_main_resampler", "test burst tile OK case", id_cnt++);

        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_Y2Y;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_main_resampler", "test burst tile YUV420 OK case", id_cnt++);
        p_ctx->filters.input_param.ctx_buf.iks_warp_phase.sec2_output.height = 1080;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_main_resampler", "test burst tile YUV420 if (output_height < (chroma_dma_size << 1u)) false OK case", id_cnt++);
        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_LINEAR_CE;

        p_ctx->filters.input_param.yuv_mode = 1;
        p_ctx->filters.input_param.ctx_buf.iks_warp_phase.sec2_output.height = 96;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_main_resampler", "test burst tile YUV422 OK case", id_cnt++);
        p_ctx->filters.input_param.ctx_buf.iks_warp_phase.sec2_output.height = 1080;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_idsp_first_main_resampler", "test burst tile YUV422 if (((uint32)output_height >> 1u) < chroma_dma_size) false OK case", id_cnt++);

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));
    }

    {
        extern uint32 iks_idsp_first_hdr_ce(idsp_flow_ctrl_t *p_flow, amba_iks_filter_t *p_filters, uint32 exp_num);

        p_ctx->filters.update_flags.iso.hdr_blend_updated = 1;

        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_hdr", "test _liso_idsp_first_set_hdr_blend ring buf error NG case", id_cnt++);

        p_ctx->filters.update_flags.cr.hdr_blend = 1;

        p_ctx->filters.crc_check_list.hdr_blend = 0;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_hdr", "test p_filters->crc_check_list.hdr_blend false OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.fe_tone_curve_updated = 1;
        p_ctx->filters.update_flags.cr.hdr_blend = 0;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_hdr", "test _liso_idsp_first_set_hdr_tone_curve ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.hdr_blend = 1;
        p_ctx->filters.update_flags.cr.hdr_decomp = 0;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_hdr", "test _liso_idsp_first_set_hdr_tone_curve ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.hdr_decomp = 1;

        p_ctx->filters.crc_check_list.hdr_blend = 0;
        p_ctx->filters.crc_check_list.hdr_decomp = 1;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_hdr", "test p_filters->crc_check_list.hdr_decomp true OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.exp0_fe_wb_gain_updated = 1;
        p_ctx->filters.update_flags.cr.hdr_blend = 0;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_hdr", "test _liso_idsp_first_set_hdr_exp ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.hdr_blend = 1;

        p_ctx->filters.update_flags.iso.exp0_fe_static_blc_updated = 1;
        p_ctx->filters.update_flags.cr.vin_stat_exp0 = 0;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_hdr", "test _liso_idsp_first_set_hdr_exp vin_stat ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.vin_stat_exp0 = 1;

        p_ctx->filters.update_flags.iso.exp1_fe_static_blc_updated = 1;
        p_ctx->filters.update_flags.cr.vin_stat_exp1 = 0;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_hdr", "test _liso_idsp_first_set_hdr_exp vin_stat ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.vin_stat_exp1 = 1;

        p_ctx->filters.update_flags.iso.exp2_fe_static_blc_updated = 1;
        p_ctx->filters.update_flags.cr.vin_stat_exp2 = 0;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_hdr", "test _liso_idsp_first_set_hdr_exp vin_stat ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.vin_stat_exp2 = 1;

        p_ctx->filters.crc_check_list.hdr_blend = 0;
        p_ctx->filters.crc_check_list.vin_stat_exp0 = 1;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_hdr", "test p_filters->crc_check_list.vin_stat_exp true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.vin_stat_exp0 = 0;

        p_ctx->filters.crc_check_list.vin_stat_exp1 = 1;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_hdr", "test p_filters->crc_check_list.vin_stat_exp true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.vin_stat_exp1 = 0;

        p_ctx->filters.crc_check_list.vin_stat_exp2 = 1;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_hdr", "test p_filters->crc_check_list.vin_stat_exp true OK case", id_cnt++);

        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_Y2Y;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce", "test Y2Y NG case", id_cnt++);
        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_LINEAR_CE;

        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 4);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce", "test exp_num out of range NG case", id_cnt++);

        p_ctx->filters.update_flags.iso.sensor_information_updated = 1;
        p_ctx->filters.update_flags.cr.hdr_blend = 0;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce", "test _liso_idsp_first_set_decompress ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.hdr_blend = 1;

        p_ctx->filters.input_param.fe_tone_curve.decompand_enable = 1;
        p_ctx->filters.input_param.ctx_buf.first_compression_offset = 1;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce", "test invalid decompand_enable and first_compression_offset NG case", id_cnt++);
        p_ctx->filters.input_param.ctx_buf.first_compression_offset = 0;

        p_ctx->filters.update_flags.iso.ce_updated = 1;
        p_ctx->filters.update_flags.cr.ce = 0;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce", "test _liso_idsp_first_set_ce ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.ce = 1;

        p_ctx->filters.update_flags.iso.ce_input_table_updated = 1;
        p_ctx->filters.update_flags.cr.ce_linear = 0;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce", "test _liso_idsp_first_set_ce_linear ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.ce_linear = 1;

        p_ctx->filters.crc_check_list.ce = 0;
        p_ctx->filters.crc_check_list.ce_linear = 1;
        Rval = iks_idsp_first_hdr_ce(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce", "test p_filters->crc_check_list.ce_linear true OK case", id_cnt++);

        memset(&p_ctx->filters.crc_check_list, 0xff, sizeof(amba_iks_crc_check_list_t));
        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));
    }

    {
        extern uint32 iks_idsp_first_hdr_ce_prep(idsp_flow_ctrl_t *p_flow, const amba_iks_filter_t *p_filters, uint32 exp_num);

        p_ctx->filters.update_flags.iso.hdr_blend_updated = 1;
        p_ctx->filters.update_flags.cr.hdr_blend_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_hdr_misc", "test _liso_idsp_first_set_hdr_blend_b ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.hdr_blend_b = 1;

        p_ctx->filters.crc_check_list.hdr_blend_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_hdr_misc", "test p_filters->crc_check_list.hdr_blend_b true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.hdr_blend_b = 1;

        p_ctx->filters.update_flags.iso.fe_tone_curve_updated = 1;
        p_ctx->filters.update_flags.cr.hdr_blend_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_hdr_misc", "test _liso_idsp_first_set_hdr_tone_curve_b ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.hdr_blend_b = 1;
        p_ctx->filters.update_flags.cr.hdr_decomp_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_hdr_misc", "test _liso_idsp_first_set_hdr_tone_curve_b ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.hdr_decomp_b = 1;

        p_ctx->filters.crc_check_list.hdr_blend_b = 0;
        p_ctx->filters.crc_check_list.hdr_decomp_b = 1;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_hdr_misc", "test p_filters->crc_check_list.hdr_decomp_b true OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.ce_updated = 1;
        p_ctx->filters.update_flags.cr.dbp_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_ce", "test _liso_idsp_first_set_ce_fe ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.dbp_b = 1;

        p_ctx->filters.update_flags.cr.sbp_grgb_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_ce", "test _liso_idsp_first_set_ce_fe ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.sbp_grgb_b = 1;

        p_ctx->filters.update_flags.cr.ce_fe = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_ce", "test _liso_idsp_first_set_ce_fe ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.ce_fe = 1;

        p_ctx->filters.update_flags.iso.ce_updated = 0;
        p_ctx->filters.update_flags.iso.ce_input_table_updated = 1;
        p_ctx->filters.update_flags.cr.ce_fe = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_ce", "test _liso_idsp_first_set_ce_fe_linear ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.ce_fe = 1;
        p_ctx->filters.update_flags.cr.ce_fe_linear = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_ce", "test _liso_idsp_first_set_ce_fe_linear ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.ce_fe_linear = 1;

        p_ctx->filters.update_flags.iso.ce_updated = 1;
        p_ctx->filters.update_flags.cr.dbp_b = 1;
        p_ctx->filters.update_flags.cr.sbp_grgb_b = 1;
        p_ctx->filters.update_flags.cr.ce_fe = 1;

        p_ctx->filters.crc_check_list.dbp_b = 0;
        p_ctx->filters.crc_check_list.sbp_grgb_b = 0;
        p_ctx->filters.crc_check_list.ce_fe = 1;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_ce", "test p_filters->crc_check_list.ce_fe true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.sbp_grgb_b = 0;

        p_ctx->filters.update_flags.iso.ce_input_table_updated = 1;
        p_ctx->filters.update_flags.cr.ce_fe_linear = 1;

        p_ctx->filters.crc_check_list.ce_fe = 0;
        p_ctx->filters.crc_check_list.ce_fe_linear = 1;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_ce", "test p_filters->crc_check_list.ce_fe_linear true OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.rgb_ir_updated = 1;
        p_ctx->filters.update_flags.cr.hdr_blend_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_rgbir", "test _liso_idsp_first_set_rgbir_b ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.hdr_blend_b = 1;

        p_ctx->filters.update_flags.cr.dgain_vig_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_rgbir", "test _liso_idsp_first_set_rgbir_b ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.dgain_vig_b = 1;

        p_ctx->filters.update_flags.cr.dbp_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_rgbir", "test _liso_idsp_first_set_rgbir_b ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.dbp_b = 1;

        p_ctx->filters.update_flags.cr.sbp_grgb_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_rgbir", "test _liso_idsp_first_set_rgbir_b ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.sbp_grgb_b = 1;

        p_ctx->filters.crc_check_list.hdr_blend_b = 0;
        p_ctx->filters.crc_check_list.dgain_vig_b = 1;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_rgbir", "test p_filters->crc_check_list.dgain_vig_b true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.dgain_vig_b = 0;

        p_ctx->filters.crc_check_list.dbp_b = 1;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_rgbir", "test p_filters->crc_check_list.dbp_b true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.dbp_b = 0;

        p_ctx->filters.crc_check_list.sbp_grgb_b = 1;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_rgbir", "test p_filters->crc_check_list.sbp_grgb_b true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.sbp_grgb_b = 0;

        p_ctx->filters.update_flags.iso.before_ce_wb_gain_updated = 1;
        p_ctx->filters.update_flags.cr.ce_fe = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_before_ce_gain", "test _liso_idsp_first_set_before_ce_gain_b ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.ce_fe = 1;

        p_ctx->filters.update_flags.iso.sensor_information_updated = 1;
        p_ctx->filters.update_flags.cr.hdr_blend_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_decompress", "test _liso_idsp_first_set_decompress_b ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.hdr_blend_b = 1;

        p_ctx->filters.update_flags.iso.exp0_fe_wb_gain_updated = 1;
        p_ctx->filters.update_flags.cr.hdr_blend = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_hdr_exp", "test _liso_idsp_first_set_hdr_exp_b ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.hdr_blend = 1;

        p_ctx->filters.update_flags.iso.vignette_compensation_updated = 1;
        p_ctx->filters.update_flags.cr.dgain_vig_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_vig", "test _liso_idsp_first_set_vig_b ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.dgain_vig_b = 1;

        p_ctx->filters.update_flags.cr.vig_mult_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_vig", "test _liso_idsp_first_set_vig_b ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.vig_mult_b = 1;

        p_ctx->filters.update_flags.cr.dbp_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_vig", "test _liso_idsp_first_set_vig_b ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.dbp_b = 1;

        p_ctx->filters.crc_check_list.dgain_vig_b = 0;
        p_ctx->filters.crc_check_list.vig_mult_b = 1;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_vig", "test p_filters->crc_check_list.vig_mult_b true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.vig_mult_b = 0;

        p_ctx->filters.crc_check_list.dbp_b = 1;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec4_vig", "test p_filters->crc_check_list.dbp_b true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.dbp_b = 0;

        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_Y2Y;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce_prep", "test Y2Y NG case", id_cnt++);
        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_LINEAR_CE;

        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 4);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce_prep", "test exp_num out of range NG case", id_cnt++);

        p_ctx->filters.update_flags.iso.is_1st_frame = 1;
        p_ctx->filters.update_flags.iso.sensor_information_updated = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce_prep", "test (p_filters->update_flags.iso.sensor_information_updated == 1u) false OK case", id_cnt++);
        p_ctx->filters.update_flags.iso.sensor_information_updated = 1;

        p_ctx->filters.update_flags.cr.hdr_blend_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce_prep", "test idspdrv_set_bayer_pattern_sec4 ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.hdr_blend_b = 1;

        p_ctx->filters.update_flags.cr.dgain_vig_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce_prep", "test idspdrv_set_bayer_pattern_sec4 ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.dgain_vig_b = 1;

        p_ctx->filters.update_flags.cr.dbp_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce_prep", "test idspdrv_set_bayer_pattern_sec4 ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.dbp_b = 1;

        p_ctx->filters.update_flags.cr.sbp_grgb_b = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce_prep", "test idspdrv_set_bayer_pattern_sec4 ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.sbp_grgb_b = 1;

        p_ctx->filters.update_flags.cr.ce_fe = 0;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce_prep", "test idspdrv_set_bayer_pattern_sec4 ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.ce_fe = 1;

        p_ctx->filters.input_param.flip_mode = IK_FLIP_RAW_H | IK_FLIP_RAW_V;
        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce_prep", "test flip OK case", id_cnt++);

        Rval = iks_idsp_first_hdr_ce_prep(p_flow, &p_ctx->filters, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_hdr_ce_prep", "test if(exp_num > 0U) false OK case", id_cnt++);

        memset(&p_ctx->filters.crc_check_list, 0xff, sizeof(amba_iks_crc_check_list_t));
        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));
    }

    {
        extern uint32 iks_idsp_first_cfa(idsp_flow_ctrl_t *p_flow, const amba_iks_filter_t *p_filters);

        p_ctx->filters.update_flags.iso.before_ce_wb_gain_updated = 1;
        p_ctx->filters.update_flags.cr.ce = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_before_ce_gain", "test _liso_idsp_first_set_before_ce_gain ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.ce = 1;

        p_ctx->filters.update_flags.iso.dynamic_bpc_updated = 1;
        p_ctx->filters.update_flags.cr.dbp = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_dbp", "test _liso_idsp_first_DBP ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.dbp = 1;

        p_ctx->filters.update_flags.cr.sbp_grgb = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_dbp", "test _liso_idsp_first_DBP ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.sbp_grgb = 1;

        p_ctx->filters.crc_check_list.dbp = 0;
        p_ctx->filters.crc_check_list.sbp_grgb = 1;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_dbp", "test p_filters->crc_check_list.sbp_grgb true OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.grgb_mismatch_updated = 1;
        p_ctx->filters.update_flags.cr.cfa_noise = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_grgb_mismatch", "test _liso_idsp_first_grgb_mismatch ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.cfa_noise = 1;

        p_ctx->filters.update_flags.iso.cfa_leakage_filter_updated = 1;
        p_ctx->filters.update_flags.cr.sbp_grgb = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_cfa_leakage", "test _liso_idsp_first_cfa_leakage_filter ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.sbp_grgb = 1;

        p_ctx->filters.update_flags.iso.anti_aliasing_updated = 1;
        p_ctx->filters.update_flags.cr.sbp_grgb = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_anti_aliasing", "test _liso_idsp_first_anti_aliasing ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.sbp_grgb = 1;

        p_ctx->filters.update_flags.iso.demosaic_updated = 1;
        p_ctx->filters.update_flags.cr.sbp_grgb = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_thresh_mono_grad", "test _liso_idsp_first_set_thresh_mono_grad ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.sbp_grgb = 1;

        p_ctx->filters.update_flags.cr.cfa_noise = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_thresh_mono_grad", "test _liso_idsp_first_set_thresh_mono_grad ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.cfa_noise = 1;

        p_ctx->filters.update_flags.cr.demosaic = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_thresh_mono_grad", "test _liso_idsp_first_set_thresh_mono_grad ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.demosaic = 1;

        p_ctx->filters.crc_check_list.sbp_grgb = 0;
        p_ctx->filters.crc_check_list.cfa_noise = 1;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_thresh_mono_grad", "test p_filters->crc_check_list.cfa_noise true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.cfa_noise = 0;
        p_ctx->filters.crc_check_list.demosaic = 1;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_thresh_mono_grad", "test p_filters->crc_check_list.demosaic true OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.cfa_noise_filter_updated = 1;
        p_ctx->filters.update_flags.cr.cfa_noise = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_cfa_noise", "test _liso_idsp_first_cfa_noise_filter ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.cfa_noise = 1;

        p_ctx->filters.update_flags.iso.after_ce_wb_gain_updated = 1;
        p_ctx->filters.update_flags.cr.dgain = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_wb_gain", "test _liso_idsp_first_wb_gain ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.dgain = 1;

        p_ctx->filters.update_flags.iso.rgb_ir_updated = 1;
        p_ctx->filters.update_flags.cr.hdr_blend = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rgb_ir", "test _liso_idsp_first_set_rgbir ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.hdr_blend = 1;

        p_ctx->filters.update_flags.cr.vin_stat_exp0 = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rgb_ir", "test _liso_idsp_first_set_rgbir ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.vin_stat_exp0 = 1;

        p_ctx->filters.update_flags.cr.vin_stat_exp1 = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rgb_ir", "test _liso_idsp_first_set_rgbir ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.vin_stat_exp1 = 1;

        p_ctx->filters.update_flags.cr.vin_stat_exp2 = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rgb_ir", "test _liso_idsp_first_set_rgbir ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.vin_stat_exp2 = 1;

        p_ctx->filters.update_flags.cr.dgain_vig = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rgb_ir", "test _liso_idsp_first_set_rgbir ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.dgain_vig = 1;

        p_ctx->filters.update_flags.cr.dbp = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rgb_ir", "test _liso_idsp_first_set_rgbir ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.dbp = 1;

        p_ctx->filters.update_flags.cr.sbp_grgb = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rgb_ir", "test _liso_idsp_first_set_rgbir ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.sbp_grgb = 1;

        p_ctx->filters.update_flags.cr.aaa_cfa = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rgb_ir", "test _liso_idsp_first_set_rgbir ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.aaa_cfa = 1;

        p_ctx->filters.crc_check_list.hdr_blend = 0;
        p_ctx->filters.crc_check_list.vin_stat_exp0 = 1;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rgb_ir", "test p_filters->crc_check_list.vin_stat_exp0 true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.vin_stat_exp0 = 0;

        p_ctx->filters.crc_check_list.vin_stat_exp1 = 1;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rgb_ir", "test p_filters->crc_check_list.vin_stat_exp1 true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.vin_stat_exp1 = 0;

        p_ctx->filters.crc_check_list.vin_stat_exp2 = 1;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rgb_ir", "test p_filters->crc_check_list.vin_stat_exp2 true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.vin_stat_exp2 = 0;

        p_ctx->filters.crc_check_list.dgain_vig = 1;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rgb_ir", "test p_filters->crc_check_list.dgain_vig true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.dgain_vig = 0;

        p_ctx->filters.crc_check_list.dbp = 1;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rgb_ir", "test p_filters->crc_check_list.dbp true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.dbp = 0;

        p_ctx->filters.crc_check_list.sbp_grgb = 1;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rgb_ir", "test p_filters->crc_check_list.sbp_grgb true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.sbp_grgb = 0;

        p_ctx->filters.crc_check_list.aaa_cfa = 1;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rgb_ir", "test p_filters->crc_check_list.aaa_cfa true OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.resample_str_update = 1;
        p_ctx->filters.update_flags.cr.cfa_prescale = 0;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_pre_scaler", "test _liso_idsp_first_cfa_prescale ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.cfa_prescale = 1;

        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_Y2Y;
        Rval = iks_idsp_first_cfa(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_cfa", "test Y2Y NG case", id_cnt++);
        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_LINEAR_CE;

        memset(&p_ctx->filters.crc_check_list, 0xff, sizeof(amba_iks_crc_check_list_t));
        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));
    }

    {
        extern uint32 iks_idsp_first_rgb(idsp_flow_ctrl_t *p_flow, amba_iks_filter_t *p_filters);

        p_ctx->filters.update_flags.iso.is_1st_frame = 1;
        p_ctx->filters.input_param.sensor_info.sensor_mode = IK_SENSOR_MODE_RCCC;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rccc", "test sensor_mode == IK_SENSOR_MODE_RCCC OK case", id_cnt++);

        p_ctx->filters.update_flags.iso.demosaic_updated = 1;
        p_ctx->filters.update_flags.cr.demosaic = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_demosaic", "test _liso_idsp_first_demosaic ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.demosaic = 1;

        p_ctx->filters.update_flags.iso.rgb_to_12y_updated = 1;
        p_ctx->filters.update_flags.cr.rgb_12y = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_rgb_12y", "test _liso_idsp_first_rgb_2_12y ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.rgb_12y = 1;

        p_ctx->filters.update_flags.iso.tone_curve_updated = 1;
        p_ctx->filters.update_flags.cr.cc = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_cc_out", "test _liso_idsp_first_cc_out ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.cc = 1;

        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_Y2Y;
        p_ctx->filters.input_param.use_cc_for_yuv2yuv = 0;
        p_ctx->filters.input_param.use_cc = 1;
        p_ctx->filters.input_param.use_tone_curve = 1;
        p_ctx->filters.update_flags.cr.cc = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_y2y_cc", "test idspdrv_set_color_correction_input_table_reg_y2y ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.cc = 1;

        p_ctx->filters.input_param.use_tone_curve = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_y2y_cc", "test (p_filters->input_param.use_tone_curve == 1U) false OK case", id_cnt++);
        p_ctx->filters.input_param.use_tone_curve = 1;

        p_ctx->filters.input_param.use_cc_for_yuv2yuv = 1;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_y2y_cc", "test (p_filters->input_param.use_cc_for_yuv2yuv == 0U) false OK case", id_cnt++);
        p_ctx->filters.input_param.use_cc_for_yuv2yuv = 0;

        p_ctx->filters.crc_check_list.cc = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_y2y_cc", "test p_filters->crc_check_list.cc true OK case", id_cnt++);

        p_ctx->filters.input_param.use_cc_for_yuv2yuv = 1;

        p_ctx->filters.update_flags.iso.color_correction_updated = 1;
        p_ctx->filters.update_flags.cr.cc = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_y2y_cc", "test _liso_idsp_first_cc_3d ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.cc = 1;

        p_ctx->filters.update_flags.cr.cc_3d = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_y2y_cc", "test _liso_idsp_first_cc_3d ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.cc_3d = 1;

        p_ctx->filters.crc_check_list.cc = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_y2y_cc", "test p_filters->crc_check_list.cc true OK case", id_cnt++);

        p_ctx->filters.crc_check_list.cc_3d = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_y2y_cc", "test p_filters->crc_check_list.cc_3d true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.cc = 1;
        p_ctx->filters.crc_check_list.cc_3d = 1;

        p_ctx->filters.input_param.use_cc_for_yuv2yuv = 0;
        p_ctx->filters.input_param.use_cc = 0;

        p_ctx->filters.update_flags.iso.color_correction_updated = 1;
        p_ctx->filters.update_flags.iso.is_1st_frame = 1;
        p_ctx->filters.update_flags.cr.cc = 1;
        p_ctx->filters.update_flags.cr.rgb_2_yuv = 1;
        p_ctx->filters.crc_check_list.cc = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_y2y_cc", "test p_filters->crc_check_list.cc true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.cc = 1;
        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_LINEAR_CE;

        p_ctx->filters.update_flags.iso.window_size_info_updated = 1;
        p_ctx->filters.update_flags.cr.aaa_data = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_aaa", "test _liso_idsp_first_3a ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.aaa_data = 1;

        p_ctx->filters.update_flags.cr.dgain_vig = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_aaa", "test _liso_idsp_first_3a ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.dgain_vig = 1;

        p_ctx->filters.update_flags.cr.sbp_grgb = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_aaa", "test _liso_idsp_first_3a ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.sbp_grgb = 1;

        p_ctx->filters.update_flags.cr.aaa_cfa = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_aaa", "test _liso_idsp_first_3a ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.aaa_cfa = 1;

        p_ctx->filters.update_flags.cr.aaa = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_aaa", "test _liso_idsp_first_3a ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.aaa = 1;

        p_ctx->filters.crc_check_list.aaa_data = 0;
        p_ctx->filters.crc_check_list.dgain_vig = 1;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_aaa", "test p_filters->crc_check_list.dgain_vig true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.dgain_vig = 0;

        p_ctx->filters.crc_check_list.sbp_grgb = 1;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_aaa", "test p_filters->crc_check_list.sbp_grgb true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.sbp_grgb = 0;

        p_ctx->filters.crc_check_list.aaa_cfa = 1;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_aaa", "test p_filters->crc_check_list.aaa_cfa true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.aaa_cfa = 0;

        p_ctx->filters.crc_check_list.aaa = 1;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_aaa", "test p_filters->crc_check_list.aaa true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.aaa = 0;

        p_ctx->filters.update_flags.iso.color_correction_updated = 1;
        p_ctx->filters.update_flags.cr.cc = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_rgb", "test _liso_idsp_first_cc_3d ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.cc = 1;

        p_ctx->filters.update_flags.cr.cc_3d = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_rgb", "test _liso_idsp_first_cc_3d ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.cc_3d = 1;

        p_ctx->filters.crc_check_list.cc = 0;
        p_ctx->filters.crc_check_list.cc_3d = 1;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_rgb", "test p_filters->crc_check_list.cc_3d true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.cc_3d = 0;

        p_ctx->filters.update_flags.iso.pre_cc_gain_updated = 1;
        p_ctx->filters.update_flags.cr.cc = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_rgb", "test _liso_idsp_first_pre_cc_gain ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.cc = 1;

        p_ctx->filters.crc_check_list.cc = 0;
        Rval = iks_idsp_first_rgb(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_rgb", "test p_filters->crc_check_list.cc false OK case", id_cnt++);
        p_ctx->filters.crc_check_list.cc = 1;

        memset(&p_ctx->filters.crc_check_list, 0xff, sizeof(amba_iks_crc_check_list_t));
        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));
    }

    {
        extern uint32 iks_idsp_first_yuv(idsp_flow_ctrl_t *p_flow, const amba_iks_filter_t *p_filters);

        p_ctx->filters.update_flags.iso.chroma_scale_updated = 1;
        p_ctx->filters.update_flags.cr.rgb_2_yuv = 0;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_chroma_scale", "test _liso_idsp_first_chroma_scale ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.rgb_2_yuv = 1;

        p_ctx->filters.update_flags.iso.chroma_median_filter_updated = 1;
        p_ctx->filters.update_flags.cr.luma_shp = 0;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_chroma_median", "test _liso_idsp_first_chroma_median_filter ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.luma_shp = 1;

        p_ctx->filters.update_flags.cr.chroma_median = 0;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_chroma_median", "test _liso_idsp_first_chroma_median_filter ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.chroma_median = 1;

        p_ctx->filters.crc_check_list.luma_shp = 0;
        p_ctx->filters.crc_check_list.chroma_median = 1;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_chroma_median", "test p_filters->crc_check_list.chroma_median true OK case", id_cnt++);
        p_ctx->filters.crc_check_list.chroma_median = 0;

        p_ctx->filters.update_flags.iso.chroma_filter_updated = 1;
        p_ctx->filters.update_flags.cr.chroma_noise = 0;
        p_ctx->filters.update_flags.cr.luma_resmp = 1;
        p_ctx->filters.update_flags.cr.chroma_resmp = 1;
        p_ctx->filters.update_flags.cr.vwarp = 1;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_set_sec2_chroma_noise", "test _liso_idsp_first_chroma_noise_filter ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.chroma_noise = 1;

        p_ctx->filters.update_flags.iso.chroma_filter_updated = 0;
        p_ctx->filters.update_flags.iso.advance_spatial_filter_updated = 1;
        p_ctx->filters.update_flags.cr.luma_shp = 0;
        p_ctx->filters.update_flags.cr.chroma_median = 1;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_yuv", "test _liso_idsp_first_luma_processing ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.luma_shp = 1;

        p_ctx->filters.update_flags.iso.lnl_updated = 1;
        p_ctx->filters.update_flags.cr.luma_shp = 0;
        p_ctx->filters.update_flags.cr.chroma_median = 1;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_yuv", "test _liso_idsp_first_lnl ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.luma_shp = 1;

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.iso.after_ce_wb_gain_updated = 1;
        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_Y2Y;
        p_ctx->filters.update_flags.cr.luma_shp = 0;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_yuv", "test (p_filters->input_param.ability != AMBA_IK_VIDEO_Y2Y) false OK case", id_cnt++);
        p_ctx->filters.update_flags.cr.luma_shp = 1;
        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_LINEAR_CE;

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.iso.rgb_to_12y_updated = 1;
        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_Y2Y;
        p_ctx->filters.update_flags.cr.luma_shp = 0;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_yuv", "test (p_filters->input_param.ability != AMBA_IK_VIDEO_Y2Y) false OK case", id_cnt++);
        p_ctx->filters.update_flags.cr.luma_shp = 1;
        p_ctx->filters.input_param.ability = AMBA_IK_VIDEO_LINEAR_CE;

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.iso.rgb_to_12y_updated = 1;
        p_ctx->filters.update_flags.cr.luma_shp = 0;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_yuv", "test _liso_idsp_first_lnl ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.luma_shp = 1;

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.iso.color_correction_updated = 1;
        p_ctx->filters.update_flags.cr.luma_shp = 0;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_yuv", "test _liso_idsp_first_lnl ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.luma_shp = 1;

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.iso.tone_curve_updated = 1;
        p_ctx->filters.update_flags.cr.luma_shp = 0;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_yuv", "test _liso_idsp_first_lnl ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.luma_shp = 1;

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.iso.burst_tile_updated = 1;
        p_ctx->filters.update_flags.cr.vwarp = 0;
        p_ctx->filters.update_flags.cr.chroma_noise = 1;
        p_ctx->filters.update_flags.cr.luma_resmp = 1;
        p_ctx->filters.update_flags.cr.chroma_resmp = 1;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_yuv", "test _liso_idsp_first_main_resampler ring buf error NG case", id_cnt++);

        p_ctx->filters.update_flags.iso.window_size_info_updated = 1;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_yuv", "test _liso_idsp_first_main_resampler ring buf error NG case", id_cnt++);

        p_ctx->filters.update_flags.iso.resample_str_update = 1;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_yuv", "test _liso_idsp_first_main_resampler ring buf error NG case", id_cnt++);

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.iso.burst_tile_updated = 1;
        p_ctx->filters.update_flags.cr.chroma_resmp = 0;
        p_ctx->filters.update_flags.cr.vwarp = 1;
        p_ctx->filters.update_flags.cr.chroma_noise = 1;
        p_ctx->filters.update_flags.cr.luma_resmp = 1;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_yuv", "test _liso_idsp_first_main_resampler ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.chroma_resmp = 1;

        p_ctx->filters.update_flags.iso.burst_tile_updated = 1;
        p_ctx->filters.update_flags.cr.luma_resmp = 0;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_yuv", "test _liso_idsp_first_main_resampler ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.luma_resmp = 1;

        p_ctx->filters.crc_check_list.vwarp = 1;
        p_ctx->filters.crc_check_list.luma_resmp = 0;
        p_ctx->filters.crc_check_list.chroma_resmp = 0;
        p_ctx->filters.crc_check_list.chroma_noise = 0;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_yuv", "test p_filters->crc_check_list.vwarp true OK case", id_cnt++);

        p_ctx->filters.crc_check_list.chroma_noise = 1;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_yuv", "test p_filters->crc_check_list.chroma_noise true OK case", id_cnt++);

        p_ctx->filters.crc_check_list.chroma_resmp = 1;
        Rval = iks_idsp_first_yuv(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_yuv", "test p_filters->crc_check_list.chroma_resmp true OK case", id_cnt++);

        memset(&p_ctx->filters.crc_check_list, 0xff, sizeof(amba_iks_crc_check_list_t));
        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));
    }

    {
        extern uint32 iks_idsp_first_set_mctf_mcts(idsp_flow_ctrl_t *p_flow, const amba_iks_filter_t *p_filters);

        p_ctx->filters.update_flags.iso.window_size_info_updated = 1;
        p_ctx->filters.update_flags.cr.shpb = 0;
        p_ctx->filters.update_flags.cr.mctf = 1;
        p_ctx->filters.update_flags.cr.mctf_ref_remap = 1;
        Rval = iks_idsp_first_set_mctf_mcts(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_set_mctf_mcts", "test _ik_calc_mctf ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.shpb = 1;

        p_ctx->filters.update_flags.cr.mctf_ref_remap = 0;
        Rval = iks_idsp_first_set_mctf_mcts(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_set_mctf_mcts", "test _ik_calc_mctf ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.mctf_ref_remap = 1;

        p_ctx->filters.update_flags.cr.mctf = 0;
        Rval = iks_idsp_first_set_mctf_mcts(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_set_mctf_mcts", "test _ik_calc_mctf ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.mctf = 1;

        p_ctx->filters.crc_check_list.shpb = 1;
        p_ctx->filters.crc_check_list.mctf = 0;
        p_ctx->filters.crc_check_list.mctf_ref_remap = 0;
        Rval = iks_idsp_first_set_mctf_mcts(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_set_mctf_mcts", "test p_filters->crc_check_list.shpb true OK case", id_cnt++);

        p_ctx->filters.crc_check_list.mctf_ref_remap = 1;
        Rval = iks_idsp_first_set_mctf_mcts(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_set_mctf_mcts", "test p_filters->crc_check_list.mctf_ref_remap true OK case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.compression_dither_disable = 1;
        Rval = iks_idsp_first_set_mctf_mcts(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_set_mctf_mcts", "test if(p_filters->input_param.video_mctf.compression_dither_disable == 0u) false OK case", id_cnt++);

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.iso.video_mctf_updated = 1;
        p_ctx->filters.update_flags.cr.shpb = 0;
        Rval = iks_idsp_first_set_mctf_mcts(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_set_mctf_mcts", "test _ik_calc_shpb ring buf error NG case", id_cnt++);

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.iso.fnlshpns_both_updated = 1;
        p_ctx->filters.update_flags.iso.fnlshpns_both_tdt_updated = 1;
        p_ctx->filters.update_flags.cr.shpb_tc = 0;
        p_ctx->filters.update_flags.cr.shpb = 1;
        Rval = iks_idsp_first_set_mctf_mcts(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_set_mctf_mcts", "test _ik_calc_shpb_tc ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.shpb_tc = 1;

        p_ctx->filters.crc_check_list.shpb = 0;
        p_ctx->filters.crc_check_list.shpb_tc = 1;
        Rval = iks_idsp_first_set_mctf_mcts(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_set_mctf_mcts", "test p_filters->crc_check_list.shpb_tc true OK case", id_cnt++);

        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));

        p_ctx->filters.update_flags.iso.video_mctf_and_final_sharpen_updated = 1;
        p_ctx->filters.update_flags.cr.shpb_coring = 0;
        p_ctx->filters.update_flags.cr.mctf_pos_dep = 1;
        Rval = iks_idsp_first_set_mctf_mcts(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_set_mctf_mcts", "test _ik_calc_flow_depend ring buf error NG case", id_cnt++);

        p_ctx->filters.update_flags.cr.shpb_coring = 1;
        p_ctx->filters.update_flags.cr.mctf_pos_dep = 0;
        Rval = iks_idsp_first_set_mctf_mcts(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_set_mctf_mcts", "test _ik_calc_flow_depend ring buf error NG case", id_cnt++);
        p_ctx->filters.update_flags.cr.mctf_pos_dep = 1;

        p_ctx->filters.crc_check_list.mctf_pos_dep = 0;
        p_ctx->filters.crc_check_list.shpb_coring = 1;
        Rval = iks_idsp_first_set_mctf_mcts(p_flow, &p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_idsp_first_set_mctf_mcts", "test p_filters->crc_check_list.shpb_coring true OK case", id_cnt++);

        memset(&p_ctx->filters.crc_check_list, 0xff, sizeof(amba_iks_crc_check_list_t));
        memset(&p_ctx->filters.update_flags, 0, sizeof(amba_iks_filter_update_flags_t));
    }

    {
        ik_ability_t ability = {0};
        ik_calc_geo_settings_t info = {0};
        ik_calc_geo_settings_inpar_t in = {0};
        ik_window_size_info_t window_size_info = {0};
        ik_cawarp_info_t calib_ca_warp_info = {0};
        ik_warp_info_t calib_warp_info = {0};
        ik_warp_buffer_info_t warp_buf_info = {0};
        ik_vin_active_window_t active_window = {0};
        ik_dummy_margin_range_t dmy_range = {0};
        ik_dzoom_info_t dzoom_info = {0};
        ik_vin_sensor_info_t sensor_info = {0};
        ik_rgb_ir_t rgb_ir = {0};
        ik_aaa_stat_info_t aaa_stat_info = {0};
        ik_aaa_pg_af_stat_info_t aaa_pg_af_stat_info = {0};
        ik_af_stat_ex_info_t af_stat_ex_info = {0};
        ik_pg_af_stat_ex_info_t pg_af_stat_ex_info = {0};
        ik_histogram_info_t hist_info = {0};
        ik_histogram_info_t hist_info_pg = {0};
        amba_iks_calc_geo_work_buf_t work_buf = {0};
        ik_calc_geo_result_t result = {0};
        extern uint32 liso_exe_calc_geo_settings(const ik_ability_t *p_ability, const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result);

        Rval = liso_exe_calc_geo_settings(&ability, &info, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_group_cmd_check_addr", "test NULL ptr NG case", id_cnt++);
        Rval = liso_exe_calc_geo_settings(&ability, NULL, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_group_cmd_check_addr", "test NULL ptr NG case", id_cnt++);

        info.p_in = &in;
        info.p_work_buf = (void *)&work_buf;
        result.p_out = NULL;
        Rval = liso_exe_calc_geo_settings(NULL, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_group_cmd_check_addr", "test NULL ptr NG case", id_cnt++);

        result.p_out = (void *)FPNMap;
        info.p_in = &in;
        info.p_work_buf = NULL;
        Rval = liso_exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_group_cmd_check_addr", "test NULL ptr NG case", id_cnt++);

        info.p_work_buf = (void *)&work_buf;
        info.p_in = NULL;
        Rval = liso_exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_group_cmd_check_addr", "test NULL ptr NG case", id_cnt++);

        info.p_in = &in;

        ability.pipe = AMBA_IK_PIPE_STILL;
        Rval = liso_exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_group_cmd_check_addr", "test if(p_ability->pipe != AMBA_IK_PIPE_VIDEO) true NG case", id_cnt++);
        ability.pipe = AMBA_IK_PIPE_VIDEO;

        in.p_window_size_info = &window_size_info;
        window_size_info.vin_sensor.h_sub_sample.factor_num = 1;
        window_size_info.vin_sensor.h_sub_sample.factor_den = 1;
        window_size_info.vin_sensor.v_sub_sample.factor_num = 1;
        window_size_info.vin_sensor.v_sub_sample.factor_den = 1;
        window_size_info.vin_sensor.width = 1920;
        window_size_info.vin_sensor.height = 1080;
        window_size_info.main_win.width = 1920;
        window_size_info.main_win.height = 1080;

        in.use_cawarp = 1;
        in.cawarp_enable = 1;
        in.p_calib_ca_warp_info = &calib_ca_warp_info;
        calib_ca_warp_info.vin_sensor_geo.h_sub_sample.factor_num = 1;
        calib_ca_warp_info.vin_sensor_geo.h_sub_sample.factor_den = 1;
        calib_ca_warp_info.vin_sensor_geo.v_sub_sample.factor_num = 1;
        calib_ca_warp_info.vin_sensor_geo.v_sub_sample.factor_den = 1;
        calib_ca_warp_info.vin_sensor_geo.width = 1920;
        calib_ca_warp_info.vin_sensor_geo.height = 1080;
        calib_ca_warp_info.p_cawarp_red = (ik_grid_point_t *)CawarpRedGrid;
        calib_ca_warp_info.p_cawarp_blue = (ik_grid_point_t *)CawarpBlueGrid;

        in.warp_enable = 1;
        in.p_calib_warp_info = &calib_warp_info;
        calib_warp_info.vin_sensor_geo.h_sub_sample.factor_num = 1;
        calib_warp_info.vin_sensor_geo.h_sub_sample.factor_den = 1;
        calib_warp_info.vin_sensor_geo.v_sub_sample.factor_num = 1;
        calib_warp_info.vin_sensor_geo.v_sub_sample.factor_den = 1;
        calib_warp_info.vin_sensor_geo.width = 1920;
        calib_warp_info.vin_sensor_geo.height = 1080;
        calib_warp_info.pwarp = (ik_grid_point_t *)WarpGrid;

        in.p_warp_buf_info = &warp_buf_info;
        in.chroma_filter_radius = 32;

        Rval = liso_exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_group_cmd_set_win_calc_addr", "test NULL ptr OK case", id_cnt++);

        in.p_active_window = &active_window;
        in.p_dmy_range = &dmy_range;
        in.p_dzoom_info = &dzoom_info;
        Rval = liso_exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_group_cmd_set_win_calc_addr", "test non-NULL ptr OK case", id_cnt++);

        in.p_sensor_info = &sensor_info;
        in.p_aaa_stat_info = &aaa_stat_info;
        in.p_aaa_pg_af_stat_info = &aaa_pg_af_stat_info;
        in.p_af_stat_ex_info = &af_stat_ex_info;
        in.p_pg_af_stat_ex_info = &pg_af_stat_ex_info;
        in.p_hist_info = &hist_info;
        in.p_hist_info_pg = &hist_info_pg;
        Rval = liso_exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_group_cmd_set_aaa_data", "test non-NULL ptr OK case", id_cnt++);

        in.p_rgb_ir = &rgb_ir;
        Rval = liso_exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_group_cmd_set_aaa_vin_histogram", "test non-NULL ptr OK case", id_cnt++);

        rgb_ir.ir_only = 1;
        Rval = liso_exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "iks_group_cmd_set_aaa_vin_histogram", "test if(p_info->p_in->p_rgb_ir->ir_only == 0U) false OK case", id_cnt++);
        rgb_ir.ir_only = 0;

        window_size_info.vin_sensor.width = 3840;
        Rval = liso_exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_exe_calc_geo_settings", "test iks_win_calc fail NG case", id_cnt++);
        window_size_info.vin_sensor.width = 1920;

        ability.video_pipe = AMBA_IK_VIDEO_Y2Y;
        Rval = liso_exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_exe_calc_geo_settings", "test AMBA_IK_VIDEO_Y2Y OK case", id_cnt++);
        ability.video_pipe = AMBA_IK_VIDEO_LINEAR;

        memset(FPNMap, 0, sizeof(FPNMap));
        in.use_cawarp = 0;
        Rval = liso_exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_exe_calc_geo_settings", "test use_cawarp disable OK case", id_cnt++);
        in.use_cawarp = 1;

        in.warp_enable = 0;
        Rval = liso_exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_exe_calc_geo_settings", "test use_cawarp disable OK case", id_cnt++);
        in.warp_enable = 1;

        warp_buf_info.luma_dma_size = 32;
        Rval = liso_exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_exe_calc_geo_settings", "test (p_info->p_in->p_warp_buf_info->luma_dma_size != 0u) true OK case", id_cnt++);

        in.p_warp_buf_info = NULL;
        Rval = liso_exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_exe_calc_geo_settings", "test if(p_info->p_in->p_warp_buf_info != NULL) false OK case", id_cnt++);
        in.p_warp_buf_info = &warp_buf_info;
#ifdef EARLYTEST_ENV
        warp_buf_info.chroma_dma_size = 32;
#else
        warp_buf_info.reserved = 32;
#endif
        Rval = liso_exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_exe_calc_geo_settings", "test if(p_info->p_in->p_warp_buf_info->chroma_dma_size != 0u) true OK case", id_cnt++);

        in.yuv_mode = 1;
        Rval = liso_exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "liso_exe_calc_geo_settings", "test if(p_info->p_in->yuv_mode == 0u) false OK case", id_cnt++);
    }
}

static void IKS_TestCovr_ImgExecuterUnitMCTFAlgo(const AMBA_IK_MODE_CFG_s *pMode0)
{
    UINT32 Rval = IK_OK;
    ik_execute_container_t execute_container = {0};
    amba_iks_context_entity_t *p_ctx;

    IKS_Executer_Set_ImgFilters(pMode0);
    Rval = iks_execute(pMode0->ContextId, &execute_container);

    iks_ctx_get_context(pMode0->ContextId, &p_ctx);

    {
        extern uint32 iks_calc_mctf(const amba_iks_filter_t *p_filters);

        p_ctx->filters.input_param.video_mctf.y_advanced_iso_enable = 1;
        Rval = iks_calc_mctf(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "y_video_mctf_auto_2_manual_one_component", "test if(p_in_video_mctf->y_advanced_iso_enable == 1U) true OK case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.cb_advanced_iso_enable = 1;
        Rval = iks_calc_mctf(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "cb_video_mctf_auto_2_manual_one_component", "test if(p_in_video_mctf->cb_advanced_iso_enable == 1U) true OK case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.cr_advanced_iso_enable = 1;
        Rval = iks_calc_mctf(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "cr_video_mctf_auto_2_manual_one_component", "test if(p_in_video_mctf->cr_advanced_iso_enable == 1U) true OK case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.y_temporal_artifact_guard = 1;
        Rval = iks_calc_mctf(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "Added_MCTF_Regs_One_Comp_y", "test if (p_in_video_mctf->y_temporal_artifact_guard == 0U) false OK case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.cb_temporal_artifact_guard = 1;
        Rval = iks_calc_mctf(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "Added_MCTF_Regs_One_Comp_cb", "test if (p_in_video_mctf->cb_temporal_artifact_guard == 0U) false OK case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.cr_temporal_artifact_guard = 1;
        Rval = iks_calc_mctf(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "Added_MCTF_Regs_One_Comp_cr", "test if (p_in_video_mctf->cr_temporal_artifact_guard == 0U) false OK case", id_cnt++);

        p_ctx->filters.input_param.internal_video_mctf.ta_frames_combine_num1 = 1;
        Rval = iks_calc_mctf(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "video_mctf_auto_to_file", "test if (p_in_internal_video_mctf->ta_frames_combine_num1 == 0U) false OK case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.y_temporal_max_change_not_t0_t1_level_based = 1;
        Rval = iks_calc_mctf(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "video_mctf_level_to_regs", "test if (in_video_mctf->y_temporal_max_change_not_t0_t1_level_based == 1U) true OK case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.cb_temporal_max_change_not_t0_t1_level_based = 1;
        Rval = iks_calc_mctf(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "video_mctf_level_to_regs", "test if (in_video_mctf->cb_temporal_max_change_not_t0_t1_level_based == 1U) true OK case", id_cnt++);

        p_ctx->filters.input_param.video_mctf.cr_temporal_max_change_not_t0_t1_level_based = 1;
        Rval = iks_calc_mctf(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "video_mctf_level_to_regs", "test if (in_video_mctf->cr_temporal_max_change_not_t0_t1_level_based == 1U) true OK case", id_cnt++);
    }

    {
        extern uint32 iks_calc_shpb(const amba_iks_filter_t *p_filters);

        p_ctx->filters.input_param.final_sharpen_fir.specify = 1;
        Rval = iks_calc_shpb(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "update_shpb_fir_coeffs_fir1_specify", "test case (uint8)SPECIFY_MANUAL_ISO: OK case", id_cnt++);

        p_ctx->filters.input_param.final_sharpen_fir.specify = 3;
        Rval = iks_calc_shpb(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "update_shpb_fir_coeffs_fir1_specify", "test case (uint8)SPECIFY_PER_DIR_STRENGTHS: OK case", id_cnt++);

        p_ctx->filters.input_param.final_sharpen_fir.specify = 5;
        Rval = iks_calc_shpb(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "update_shpb_fir_coeffs_fir1_specify", "test fir1->specify default: OK case", id_cnt++);

        p_ctx->filters.input_param.final_sharpen_noise.spatial_fir.specify = 1;
        Rval = iks_calc_shpb(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "update_shpb_fir_coeffs_fir2_specify", "test case (uint8)SPECIFY_MANUAL_ISO: OK case", id_cnt++);

        p_ctx->filters.input_param.final_sharpen_noise.spatial_fir.specify = 3;
        Rval = iks_calc_shpb(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "update_shpb_fir_coeffs_fir2_specify", "test case (uint8)SPECIFY_PER_DIR_STRENGTHS: OK case", id_cnt++);

        p_ctx->filters.input_param.final_sharpen_noise.spatial_fir.specify = 4;
        Rval = iks_calc_shpb(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "update_shpb_fir_coeffs_fir2_specify", "test case (uint8)SPECIFY_MANUAL: OK case", id_cnt++);

        p_ctx->filters.input_param.final_sharpen_noise.spatial_fir.specify = 5;
        Rval = iks_calc_shpb(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "update_shpb_fir_coeffs_fir2_specify", "test fir2->specify default: OK case", id_cnt++);

        p_ctx->filters.input_param.final_sharpen_both.mode = 1;
        Rval = iks_calc_shpb(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "iks_calc_shpb", "test if (p_filters->input_param.final_sharpen_both.mode == 2U) false OK case", id_cnt++);

        p_ctx->filters.input_param.final_sharpen_noise.advanced_iso.enable = 1;
        Rval = iks_calc_shpb(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "iks_calc_shpb", "test if (p_filters->input_param.final_sharpen_noise.advanced_iso.enable == 1U) true OK case", id_cnt++);

        p_ctx->filters.input_param.final_sharpen_fir.pos_fir_artifact_reduce_enable = 1;
        Rval = iks_calc_shpb(&p_ctx->filters);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitMCTFAlgo.c", "iks_calc_shpb", "test if (p_filters->input_param.final_sharpen_fir.pos_fir_artifact_reduce_enable == 1U) true OK case", id_cnt++);
    }
}

static void IKS_TestCovr_ImgExecuterUnitPostUtility(const AMBA_IK_MODE_CFG_s *pMode0)
{
    amba_iks_context_entity_t *p_ctx;

    iks_ctx_get_context(pMode0->ContextId, &p_ctx);

    {
        ik_cfa_noise_filter_t cnf_auto = {0};
        cfa_noise_filter_cfg_t cnf_manual = {0};

        extern int32 iks_clamp(int32 x, int32 minimun, int32 maximun);
        extern int64 iks_int64_round(int64 x, int64 divi);
        extern uint32 iks_uint_max(uint32 x, uint32 y);
        extern void iks_cfa_noise_filter_auto_to_manual(const ik_cfa_noise_filter_t *cnf_auto, cfa_noise_filter_cfg_t *cnf_manual);

        iks_clamp(-1, 0, 1);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_clamp", "test if (x < minimun) true OK case", id_cnt++);
        iks_clamp(1, -1, 0);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_clamp", "test if (x > maximun) true OK case", id_cnt++);

        iks_int64_round(-1, 1);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_int64_round", "test if (x > 0) false OK case", id_cnt++);

        iks_uint_max(0, 1);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_uint_max", "test if (x < y) true OK case", id_cnt++);

        iks_uint_max(1, 0);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_uint_max", "test if (x < y) false OK case", id_cnt++);

        cnf_auto.lev_shift[0] = -1;
        iks_cfa_noise_filter_auto_to_manual(&cnf_auto, &cnf_manual);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_shift_int32_left_int32_offset", "test if (offset>=0) false OK case", id_cnt++);
    }

    {
        cc_3D_File cc = {0};
        ik_tone_curve_t tc = {0};
        uint32 combined_tc = 0;

        extern void iks_compute_combined_tone_curve(const cc_3D_File *cc, const ik_tone_curve_t *tc, const int32 internal_do_not_use_post_tone_curve_for_cdnr, uint32 *combined_tc);

        iks_compute_combined_tone_curve(&cc, &tc, 1, &combined_tc);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_compute_combined_tone_curve", "test if (internal_do_not_use_post_tone_curve_for_cdnr == 1) true OK case", id_cnt++);
    }

    {
        iks_normalized_gain_t normalize_gain = {0};
        ik_luma_noise_reduction_t cdnr = {0};
        ik_rgb_to_12y_t r12y = {0};
        int32 weights = 0;

        extern void iks_set_lnl(const iks_normalized_gain_t *p_normalize_gain, const ik_luma_noise_reduction_t *p_cdnr, const ik_rgb_to_12y_t *p_r12y, int32 *p_weights);

        cdnr.sensor_wb_r = 4096;
        cdnr.sensor_wb_g = 4096;
        cdnr.sensor_wb_b = 4096;
        iks_set_lnl(&normalize_gain, &cdnr, &r12y, &weights);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_set_lnl", "test if( p_cdnr->sensor_wb_r != 0u ) true OK case", id_cnt++);
    }

    {
        extern uint32 iks_radius_2_size(uint32 full_size, uint32 radius, uint32 max_size);

        iks_radius_2_size(1920, 32, 128);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_radius_2_size", "test if (s1 > max_size) true OK case", id_cnt++);
    }

    {
        int32 breaks[3] = {0};
        int32 vals[3] = {0};
        extern int32 iks_interpolate_from_list(const int32 entries, const int32 breaks[], const int32 vals[], int32 strength);

        iks_interpolate_from_list(3, breaks, vals, 1);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_interpolate_from_list", "test k < entries false OK case", id_cnt++);
    }

    {
        extern int32 iks_map_level(const int32 user_value, int32 method_map);

        iks_map_level(1, 0);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_map_level", "test case 0: OK case", id_cnt++);
        iks_map_level(1, 1);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_map_level", "test case 1: OK case", id_cnt++);
        iks_map_level(1, 3);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_map_level", "test case 3: OK case", id_cnt++);
        iks_map_level(1, 4);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_map_level", "test case 4: OK case", id_cnt++);
        iks_map_level(1, 5);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_map_level", "test case 5: OK case", id_cnt++);
        iks_map_level(1, 7);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_map_level", "test default: OK case", id_cnt++);
    }

    {
        int32 coefsA[25] = {0};
        int32 coefsB[25] = {0};
        int32 coefs[25] = {0};
        int32 shift = 0;

        extern void iks_convolve_2_firs_unique_coefs(const int32 coefsA[25], const int32 coefsB[25], int32 desired_sum, int32 coefs[25], int32 *shift);

        iks_convolve_2_firs_unique_coefs(coefsA, coefsB, -1, coefs, &shift);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_convolve_2_firs_unique_coefs", "test if (desired_sum >= 0) false OK case", id_cnt++);
    }

    {
        int32 coefsA[25] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -28, 0, 0, 0, 0, 0, -28, -27};
        int32 coefsB[25] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 7, 254};
        int32 coefs[25] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -28, 0, 0, 0, 0, 0, -28, -27};
        int32 shift = 0;

        extern void iks_convolve_2_firs_unique_coefs(const int32 coefsA[25], const int32 coefsB[25], int32 desired_sum, int32 coefs[25], int32 *shift);

        iks_convolve_2_firs_unique_coefs(coefsA, coefsB, -1, coefs, &shift);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "convolve7x7", "test if ((sum + add)>=0) false OK case", id_cnt++);
    }

    {
        iks_advanced_spatial_filt_gui_t gui = {0};

        extern void iks_advance_spatial_filter_coring(const iks_advanced_spatial_filt_gui_t *gui, uintptr coring_addr, uint8 shp_mode);

        iks_advance_spatial_filter_coring(&gui, (uintptr)p_ctx->filters.input_param.ctx_buf.shp_a_coring, 2);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "print_coring_table", "test print_coring_table OK case", id_cnt++);
    }

    {
        extern int32 iks_transpose_direction(int32 dir);

        iks_transpose_direction(9);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_transpose_direction", "test default: OK case", id_cnt++);
    }

    {
        int32 shift = 0;
        int32 coefs[25] = {0};

        extern void iks_get_fir_one_dir(int32 strength, int32 make_in_bounds,
                                        uint32 t, int32 direction,
                                        int32 *shift, int32 coefs[25]);

        iks_get_fir_one_dir(0, 0, 2, 0, &shift, coefs);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_get_fir_one_dir", "test if (HIGH_PASS_INVERTED == t) true OK case", id_cnt++);
        iks_get_fir_one_dir(0, 2, 2, 0, &shift, coefs);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_get_fir_one_dir", "test if (make_in_bounds >= 1) true OK case", id_cnt++);
    }

    {
        iks_user_fir_interface ui_fir = {0};
        idsp_fir fir_out = {0};
        iks_fir_programming_control control = {0};

        extern void iks_simple_fir_program(const iks_user_fir_interface *ui_fir, idsp_fir *fir_out, const iks_fir_programming_control *control);

        control.is9x9 = 1;
        iks_simple_fir_program(&ui_fir, &fir_out, &control);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_simple_fir_program", "test if (control->is9x9 >= 1) true OK case", id_cnt++);
    }

    {
        ik_adv_spatial_filter_t UserAsf = {0};
        iks_advanced_spatial_filt_gui_t InAsfManualInfo = {0};

        extern void iks_user_asf_struct_2_gui(const ik_adv_spatial_filter_t *pUserAsf, iks_advanced_spatial_filt_gui_t *pInAsfManualInfo, int32 IsChroma);

        UserAsf.max_change_not_t0_t1_alpha = 1;
        UserAsf.max_change_down = 1;
        UserAsf.max_change_up = 0;
        iks_user_asf_struct_2_gui(&UserAsf, &InAsfManualInfo, 0);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "coring_max_change_lp", "test (0 == max_change_down) false OK case", id_cnt++);
        UserAsf.max_change_down = 0;
        iks_user_asf_struct_2_gui(&UserAsf, &InAsfManualInfo, 0);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "coring_max_change_lp", "test (0 == max_change_down) true OK case", id_cnt++);
    }

    {
        int32 curve[10];

        extern void iks_make_curve(int32 curve[], int32 alpha, int32 T0, int32 T1, int32 array_length, int32 max_val, int32 div2);

        iks_make_curve(curve, 0, 1, 0, 2, 0, 0);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_make_curve", "test if ((array_length-1) == misra_int32_T0) true OK case", id_cnt++);
    }

    {
        int32 in;
        int32 ou;

        extern void iks_iso_to_full(const int32 *in, int32 *ou, int32 num_unique);

        iks_iso_to_full(&in, &ou, 41);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_iso_to_full", "test if (UNIQUE_FIR_COEFS9x9 == (uint32)num_unique) true OK case", id_cnt++);
        iks_iso_to_full(&in, &ou, 30);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_iso_to_full", "test else OK case", id_cnt++);
    }

    {
        amba_ik_input_shp_t ShpInfo = {0};
        ik_first_sharpen_both_t sharpen_both = {0};
        ik_first_sharpen_noise_t sharpen_noise = {0};
        ik_first_sharpen_fir_t iks_shp_fir = {0};

        extern void iks_update_fir_coeffs(uint8 shp_mode,
                                          const uint8 *pShpAFir1Addr,
                                          const uint8 *pShpAFir2Addr,
                                          amba_ik_input_shp_t *pShpInfo);

        ShpInfo.sharpen_both = &sharpen_both;
        ShpInfo.sharpen_noise = &sharpen_noise;
        ShpInfo.iks_shp_fir = &iks_shp_fir;
        iks_shp_fir.specify = 2;
        iks_update_fir_coeffs(0, p_ctx->filters.input_param.ctx_buf.shp_a_fir1, p_ctx->filters.input_param.ctx_buf.shp_a_fir2, &ShpInfo);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "update_fir_coeffs_fir1_specify", "test if ((uint8)SPECIFY_ISO_DIR_STRENGTH !=  p_TempFir1->specify) false OK case", id_cnt++);

        iks_shp_fir.specify = 1;
        iks_update_fir_coeffs(0, p_ctx->filters.input_param.ctx_buf.shp_a_fir1, p_ctx->filters.input_param.ctx_buf.shp_a_fir2, &ShpInfo);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "update_fir_coeffs_fir1_specify", "test case (uint8)SPECIFY_MANUAL_ISO: OK case", id_cnt++);
        iks_shp_fir.specify = 3;
        iks_update_fir_coeffs(0, p_ctx->filters.input_param.ctx_buf.shp_a_fir1, p_ctx->filters.input_param.ctx_buf.shp_a_fir2, &ShpInfo);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "update_fir_coeffs_fir1_specify", "test case (uint8)SPECIFY_PER_DIR_STRENGTHS: OK case", id_cnt++);
        iks_shp_fir.specify = 5;
        iks_update_fir_coeffs(0, p_ctx->filters.input_param.ctx_buf.shp_a_fir1, p_ctx->filters.input_param.ctx_buf.shp_a_fir2, &ShpInfo);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "update_fir_coeffs_fir1_specify", "test default: OK case", id_cnt++);

        sharpen_noise.spatial_fir.specify = 1;
        iks_update_fir_coeffs(0, p_ctx->filters.input_param.ctx_buf.shp_a_fir1, p_ctx->filters.input_param.ctx_buf.shp_a_fir2, &ShpInfo);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "update_fir_coeffs_fir2_specify", "test case (uint8)SPECIFY_MANUAL_ISO: OK case", id_cnt++);
        sharpen_noise.spatial_fir.specify = 3;
        iks_update_fir_coeffs(0, p_ctx->filters.input_param.ctx_buf.shp_a_fir1, p_ctx->filters.input_param.ctx_buf.shp_a_fir2, &ShpInfo);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "update_fir_coeffs_fir2_specify", "test case (uint8)SPECIFY_PER_DIR_STRENGTHS: OK case", id_cnt++);
        sharpen_noise.spatial_fir.specify = 4;
        iks_update_fir_coeffs(0, p_ctx->filters.input_param.ctx_buf.shp_a_fir1, p_ctx->filters.input_param.ctx_buf.shp_a_fir2, &ShpInfo);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "update_fir_coeffs_fir2_specify", "test case (uint8)SPECIFY_MANUAL: OK case", id_cnt++);
        sharpen_noise.spatial_fir.specify = 5;
        iks_update_fir_coeffs(0, p_ctx->filters.input_param.ctx_buf.shp_a_fir1, p_ctx->filters.input_param.ctx_buf.shp_a_fir2, &ShpInfo);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "update_fir_coeffs_fir2_specify", "test case default: OK case", id_cnt++);

        iks_update_fir_coeffs(0, NULL, p_ctx->filters.input_param.ctx_buf.shp_a_fir2, &ShpInfo);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_update_fir_coeffs", "test if (pShpAFir1Addr==NULL) true OK case", id_cnt++);
        iks_update_fir_coeffs(0, p_ctx->filters.input_param.ctx_buf.shp_a_fir1, NULL, &ShpInfo);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_update_fir_coeffs", "test if (pShpAFir2Addr==NULL) true OK case", id_cnt++);
    }

    {
        ik_adv_spatial_filter_t UserAsf = {0};
        iks_advanced_spatial_filt_gui_t InAsfManualInfo = {0};

        extern void iks_user_asf_struct_2_gui(const ik_adv_spatial_filter_t *pUserAsf, iks_advanced_spatial_filt_gui_t *pInAsfManualInfo, int32 IsChroma);

        UserAsf.fir.specify = 1;
        iks_user_asf_struct_2_gui(&UserAsf, &InAsfManualInfo, 0);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_user_asf_struct_2_gui", "test case (uint8)SPECIFY_MANUAL_ISO: OK case", id_cnt++);
        UserAsf.fir.specify = 3;
        iks_user_asf_struct_2_gui(&UserAsf, &InAsfManualInfo, 0);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_user_asf_struct_2_gui", "test case (uint8)SPECIFY_PER_DIR_STRENGTHS: OK case", id_cnt++);
        UserAsf.fir.specify = 4;
        iks_user_asf_struct_2_gui(&UserAsf, &InAsfManualInfo, 0);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_user_asf_struct_2_gui", "test case (uint8)SPECIFY_MANUAL: OK case", id_cnt++);
        UserAsf.fir.specify = 5;
        iks_user_asf_struct_2_gui(&UserAsf, &InAsfManualInfo, 0);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_user_asf_struct_2_gui", "test default: OK case", id_cnt++);
    }

    {
        iks_level_control ui = {0};

        extern void iks_adapt_with_fir_scale(iks_level_control *ui, int32 T0, int32 T1, int32 alpha_min, int32 alpha_max);

        iks_adapt_with_fir_scale(&ui, 0, 1, 32, 64);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_double_round", "test if (d < 0.0) true OK case", id_cnt++);
    }

    {
        iks_advanced_spatial_filt_gui_t gui = {0};
        luma_sharpening_level_control_t coring_index_scale = {0};
        luma_sharpening_level_control_t min_coring_result = {0};

        extern void advance_spatial_filter_level_adaptation(const iks_advanced_spatial_filt_gui_t *gui,
                uintptr level_addr,
                uint8  shp_mode,
                uint8  highiso);
        extern void advance_spatial_filter_level_str_adjust(const iks_advanced_spatial_filt_gui_t *gui,
                uintptr level_addr,
                uint8  shp_mode,
                uint8  highiso);

        advance_spatial_filter_level_adaptation(&gui, (uintptr)&coring_index_scale, 1, 0);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "advance_spatial_filter_level_adaptation", "test if (shp_mode == (uint8)SHPA) false OK case", id_cnt++);
        advance_spatial_filter_level_adaptation(&gui, (uintptr)&coring_index_scale, 0, 1);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "advance_spatial_filter_level_adaptation", "test if (highiso == 0U) false OK case", id_cnt++);
        gui.FiltChroma = 1;
        advance_spatial_filter_level_adaptation(&gui, (uintptr)&coring_index_scale, 0, 0);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "advance_spatial_filter_level_adaptation", "test if (gui->FiltChroma>=1) true OK case", id_cnt++);

        advance_spatial_filter_level_str_adjust(&gui, (uintptr)&min_coring_result, 1, 0);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "advance_spatial_filter_level_str_adjust", "test if (shp_mode == (uint8)SHPA) false OK case", id_cnt++);
        advance_spatial_filter_level_str_adjust(&gui, (uintptr)&min_coring_result, 0, 1);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "advance_spatial_filter_level_str_adjust", "test if (highiso == 0U) false OK case", id_cnt++);
        gui.FiltChroma = 1;
        advance_spatial_filter_level_str_adjust(&gui, (uintptr)&min_coring_result, 0, 0);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "advance_spatial_filter_level_str_adjust", "test if (gui->FiltChroma>=1) true OK case", id_cnt++);
    }

    {
        ik_rgb_ir_t rgb_ir = {0};
        uint16 rgbir_base_val;
        uint16 rgbir_high_val;
        uint8 shift_val;

        extern void iks_set_ircorrect_mul_shift(const ik_rgb_ir_t *p_rgb_ir, uint16 *rgbir_base_val, uint16 *rgbir_high_val, uint8 *shift_val);

        rgb_ir.mul_base_val = 134217728;
        rgb_ir.mul_high_val = 134217728;
        iks_set_ircorrect_mul_shift(&rgb_ir, &rgbir_base_val, &rgbir_high_val, &shift_val);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_set_ircorrect_mul_shift", "test (b1 > 511.0) false OK case", id_cnt++);
    }

    {
        ik_cfa_noise_filter_t cnf_auto = {0};
        cfa_noise_filter_cfg_t cnf_manual = {0};

        extern void iks_cfa_noise_filter_auto_to_manual(const ik_cfa_noise_filter_t *cnf_auto, cfa_noise_filter_cfg_t *cnf_manual);

        cnf_auto.original_blend_str[0] = 256;
        iks_cfa_noise_filter_auto_to_manual(&cnf_auto, &cnf_manual);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "get_center_shift_value", "test if (temp > 31) true OK case", id_cnt++);

        cnf_auto.extent_regular[0] = 256;
        iks_cfa_noise_filter_auto_to_manual(&cnf_auto, &cnf_manual);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "get_cutoff_value", "test (fine_target < 128) false OK case", id_cnt++);

        cnf_auto.selectivity_regular = 500;
        iks_cfa_noise_filter_auto_to_manual(&cnf_auto, &cnf_manual);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "get_ring_shift_value", "test if (tmp_index > 4) true OK case", id_cnt++);

        cnf_auto.lev_mul[0] = 65535;
        cnf_auto.lev_shift[0] = 20;
        iks_cfa_noise_filter_auto_to_manual(&cnf_auto, &cnf_manual);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "comp_lev_dep_mul_shift", "test if( *p_adjusted_shift > 10 ) true OK case", id_cnt++);

        cnf_auto.original_blend_str[0] = 10;
        cnf_auto.selectivity_regular = 1000;
        iks_cfa_noise_filter_auto_to_manual(&cnf_auto, &cnf_manual);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_cfa_noise_filter_auto_to_manual", "test if (cnf_manual->shift_center_red < cnf_manual->shift_fine_ring1) true OK case", id_cnt++);
    }

    {
        extern int32 iks_getDirectionalCenterWeightValue(int32 blend_strength);

        iks_getDirectionalCenterWeightValue(10);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_ImgExecuterUnitPostUtility.c", "iks_getDirectionalCenterWeightValue", "test if (blend_strength < 8) false OK case", id_cnt++);
    }
}

static void IKS_TestCovr_ImgExecuterUnitWindowCalc(const AMBA_IK_MODE_CFG_s *pMode0)
{
    UINT32 Rval = IK_OK;
    extern uint32 iks_win_calc(const amba_iks_win_calc_t *p_info);
    amba_iks_win_calc_t info = {0};
    ik_window_size_info_t window_size_info = {0};
    ik_vin_active_window_t active_window = {0};
    ik_dummy_margin_range_t dmy_range = {0};
    ik_dzoom_info_t dzoom_info = {0};
    ik_stitch_info_t stitching_info = {0};
    amba_ik_calc_win_result_t result_win = {0};

    info.p_window_size_info = &window_size_info;
    info.p_active_window = &active_window;
    info.p_dmy_range = &dmy_range;
    info.p_dzoom_info = &dzoom_info;
    info.p_stitching_info = &stitching_info;
    info.p_result_win = &result_win;

    window_size_info.vin_sensor.h_sub_sample.factor_num = 1;
    window_size_info.vin_sensor.h_sub_sample.factor_den = 1;
    window_size_info.vin_sensor.v_sub_sample.factor_num = 1;
    window_size_info.vin_sensor.v_sub_sample.factor_den = 1;
    window_size_info.vin_sensor.width = 1920;
    window_size_info.vin_sensor.height = 1080;
    window_size_info.main_win.width = 1920;
    window_size_info.main_win.height = 1080;

    active_window.enable = 1;
    active_window.active_geo.width = 1920;
    active_window.active_geo.height = 1080;

    Rval = iks_win_calc(NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "iks_win_calc", "test NULL ptr NG case", id_cnt++);

    active_window.active_geo.start_x = 1;
    active_window.active_geo.start_y = 1;
    window_size_info.vin_sensor.width = 1922;
    window_size_info.vin_sensor.height = 1082;
    Rval = iks_win_calc(&info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "iks_win_calc", "test p_info->p_active_window->active_geo.start >= 1 case", id_cnt++);
    active_window.active_geo.start_x = 0;
    active_window.active_geo.start_y = 0;
    window_size_info.vin_sensor.width = 1920;
    window_size_info.vin_sensor.height = 1080;

    {
        Rval = iks_win_calc(&info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "exe_iso_cfg_check_active_win_validataion", "test if (p_info->p_active_window->enable == 1UL) true OK case", id_cnt++);

        active_window.active_geo.start_x = 100;
        active_window.active_geo.start_y = 100;
        Rval = iks_win_calc(&info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "exe_iso_cfg_check_active_win_validataion", "test (start + size) > vin true OK case", id_cnt++);
        active_window.active_geo.start_x = 0;
        active_window.active_geo.start_y = 0;
    }

    dzoom_info.enable = 1;
    dzoom_info.zoom_x = 65536;
    dzoom_info.zoom_y = 65536;

    {
        dzoom_info.shift_x = 131072;
        dzoom_info.shift_y = 131072;
        Rval = iks_win_calc(&info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "exe_iso_cfg_actual_win_range_check", "test p_Act->right_bot > vin true OK case", id_cnt++);

        dzoom_info.shift_x = -131072;
        dzoom_info.shift_y = -131072;
        Rval = iks_win_calc(&info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "exe_iso_cfg_actual_win_range_check", "test p_Act->left_top < Compensate true OK case", id_cnt++);
        dzoom_info.shift_x = 0;
        dzoom_info.shift_y = 0;
    }

    stitching_info.enable = 1;
    stitching_info.tile_num_x = 2;
    stitching_info.tile_num_y = 1;

    info.warp_enable = 1;
    dmy_range.enable = 1;
    dmy_range.left = 65536;
    dmy_range.right = 32768;
    dmy_range.top = 65536;
    dmy_range.bottom = 65536;

    active_window.active_geo.start_x = 128;
    active_window.active_geo.start_y = 16;

    dzoom_info.zoom_x = 65536*2;
    dzoom_info.zoom_y = 65536*2;

    {
        Rval = iks_win_calc(&info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "exe_stitching_symmetric_calculate", "test p_act_win_crop < range true OK case", id_cnt++);

        dzoom_info.shift_x = 20316160;
        Rval = iks_win_calc(&info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "exe_stitching_symmetric_calculate", "test if(ideal_max_overlap > (p_act_win_crop->left_top_x>>SFT)) false OK case", id_cnt++);

        window_size_info.vin_sensor.width = 3969;
        window_size_info.vin_sensor.height = 2945;

        window_size_info.main_win.width = 3969;
        window_size_info.main_win.width = 2945;

        active_window.enable = 1;
        active_window.active_geo.start_x = 0;
        active_window.active_geo.start_y = 0;
        active_window.active_geo.width = 4096;
        active_window.active_geo.height = 3008;

        dmy_range.enable = 0;

        dzoom_info.enable = 1;
        dzoom_info.zoom_x = 67684;
        dzoom_info.zoom_y = 66543;
        dzoom_info.shift_x = 65536*65;
        dzoom_info.shift_y = 65536*31;

        Rval = iks_win_calc(&info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "exe_iks_max_cols_protect", "test if(p_dmy->start_x < aligned_dma_boud) true OK case", id_cnt++);

        dzoom_info.enable = 1;
        dzoom_info.zoom_x = 65537;
        dzoom_info.zoom_y = 65537;
        dzoom_info.shift_x = 65536*-1;
        dzoom_info.shift_y = 65536*-1;
        Rval = iks_win_calc(&info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "iks_dummy_y_range_check", "test if (tmp < 0) true OK case", id_cnt++);

        info.ability = AMBA_IK_VIDEO_HDR_EXPO_3;
        dzoom_info.shift_x = 65536+15000;
        dzoom_info.zoom_x = 131072;
        Rval = iks_win_calc(&info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "iks_dummy_y_range_check", "test CE or HDR OK case", id_cnt++);
        info.ability = AMBA_IK_VIDEO_LINEAR;
        dzoom_info.shift_x = 65536*-1;
        dzoom_info.zoom_x = 65537;
    }

    {
        window_size_info.vin_sensor.width = 4096;
        window_size_info.vin_sensor.height = 3008;

        window_size_info.main_win.width = 4096;
        window_size_info.main_win.width = 3008;

        active_window.enable = 0;
        dmy_range.enable = 1;
        dmy_range.right = 32768;
        dzoom_info.enable = 0;
        stitching_info.enable = 1;
        stitching_info.tile_num_x = 1;
        stitching_info.tile_num_y = 1;
        Rval = iks_win_calc(&info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "iks_win_calc", "test dmy win calc fail NG case", id_cnt++);
    }
}

INT32 iks_ctest_executer_main(void)
{
    UINT32 Rval = IK_OK;
    AMBA_IK_ABILITY_s Ability[4];
    AMBA_IK_MODE_CFG_s Mode[4];

    memset(ik_working_buffer, 0, sizeof(ik_working_buffer));
    memset(WarpGrid, 0, sizeof(WarpGrid));
    memset(CawarpRedGrid, 0, sizeof(CawarpRedGrid));
    memset(CawarpBlueGrid, 0, sizeof(CawarpBlueGrid));
    memset(FPNMap, 0, sizeof(FPNMap));
    id_cnt = 0;

    pFile = fopen("data/iks_ctest_executer.txt", "w");

    // init IK
    _load_idsp_default_binary();
    Ability[0].Pipe = AMBA_IK_PIPE_VIDEO;
    Ability[0].VideoPipe = AMBA_IK_VIDEO_LINEAR_CE;
    Ability[1].Pipe = AMBA_IK_PIPE_VIDEO;
    Ability[1].VideoPipe = AMBA_IK_VIDEO_Y2Y;
    Ability[2].Pipe = AMBA_IK_PIPE_VIDEO;
    Ability[2].VideoPipe = AMBA_IK_VIDEO_LINEAR;
    Ability[3].Pipe = AMBA_IK_PIPE_VIDEO;
    Ability[3].VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_3;

    _Init_Image_Kernel(Ability);
    _Init_Image_Kernel_Core();

    Mode[0].ContextId = 0;
    Rval |= AmbaIK_InitContext(&Mode[0], &Ability[0]);

    Mode[1].ContextId = 1;
    Rval |= AmbaIK_InitContext(&Mode[1], &Ability[1]);

    Mode[2].ContextId = 2;
    Rval |= AmbaIK_InitContext(&Mode[2], &Ability[2]);

    Mode[3].ContextId = 3;
    Rval |= AmbaIK_InitContext(&Mode[3], &Ability[3]);

    // test AmbaDSP_ImgExecuterComponentIF.c
    IKS_TestCovr_ImgExecuterComponentIF(&Mode[0], &Mode[1], &Mode[2], &Mode[3], Ability);
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgExecuterUnitCaWarp.c
    IKS_TestCovr_ImgExecuterUnitCaWarp(&Mode[0]);
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgExecuterUnitWarp.c
    IKS_TestCovr_ImgExecuterUnitWarp(&Mode[0]);
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgExecuterUnitVignette.c
    IKS_TestCovr_ImgExecuterUnitVignette(&Mode[0]);
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgExecuterUnitStaticBadPixel.c
    IKS_TestCovr_ImgExecuterUnitStaticBadPixel(&Mode[0]);
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgExecuterUnitConfig.c
    IKS_TestCovr_ImgExecuterUnitConfig(&Mode[0]);
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgExecuterUnitMCTFAlgo.c
    IKS_TestCovr_ImgExecuterUnitMCTFAlgo(&Mode[0]);
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgExecuterUnitPostUtility.c
    IKS_TestCovr_ImgExecuterUnitPostUtility(&Mode[0]);
    _Reset_Image_Kernel(Ability);

    // test AmbaDSP_ImgExecuterUnitWindowCalc.c
    IKS_TestCovr_ImgExecuterUnitWindowCalc(&Mode[0]);
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

