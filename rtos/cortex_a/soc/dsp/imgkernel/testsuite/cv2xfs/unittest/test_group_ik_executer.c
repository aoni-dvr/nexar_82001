#include "test_ik_global.h"

//for idsp_ik_flow_ctrl_t
#include "idspdrv_imgknl_if.h"//rtos\cortex_a\soc\dsp\imgkernel\imgkernel\cv2xfs\inc\config

//for amba_ik_flow_tables_list_t
#include "AmbaDSP_ImgConfigEntity.h"//rtos\cortex_a\soc\dsp\imgkernel\imgkernel\cv2xfs\inc\config

//for amba_ik_filter_t
#include "AmbaDSP_ImgContextEntity.h"//rtos\cortex_a\soc\dsp\imgkernel\imgkernel\cv2xfs\inc\context

#include "AmbaDSP_ImgExecuterComponentIF_static.c"

// for img_ctx_get_context
#include "AmbaDSP_ImgContextComponentIF.h"//rtos\cortex_a\soc\dsp\imgkernel\imgkernel\cv2xfs\inc\context

#define ALIGN_N(x_, n_)   ((((x_)+((n_)-1)) & ~((n_)-1)))

static UINT8 ik_working_buffer[5<<20];
static void *pBinDataAddr;

FILE *pFile;
UINT32 id_cnt;

static UINT8 work_buf[120000+8832];

static UINT8 WarpGrid[4*256*192];
static UINT8 CawarpRedGrid[4*64*96];
static UINT8 CawarpBlueGrid[4*64*96];
static UINT8 FPNMap[IK_AMALGAM_TABLE_SBP_SIZE];


/*typedef struct {
    int32 warp_enable;
    uint32 ability;
    ik_window_size_info_t *p_window_size_info;
    ik_vin_active_window_t *p_active_window;
    ik_dummy_margin_range_t *p_dmy_range;
    ik_dzoom_info_t *p_dzoom_info;
    ik_stitch_info_t *p_stitching_info;
    amba_ik_calc_win_result_t *p_result_win;
} amba_ik_win_calc_t;*/

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
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_QueryArchMemorySize", "OK case", id_cnt++);
    rval = AmbaIK_InitArch(&ContextSetting, (void*)ik_working_buffer, sizeof(ik_working_buffer));
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitArch", "OK case", id_cnt++);
    rval = AmbaIK_InitDefBinary(pBinDataAddr);
    LOG_RVAL(pFile, rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitDefBinary", "OK case", id_cnt++);

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

static void IK_Set_ImgFilters(const AMBA_IK_MODE_CFG_s *pMode0)
{

    {
        // Safety info
        ik_safety_info_t safety_info = {0};

        safety_info.update_freq = 1;
        ik_set_safety_info(pMode0->ContextId, &safety_info);
    }

    {
        // Sensor info
        ik_vin_sensor_info_t sensor_info = {0};

        ik_set_vin_sensor_info(pMode0->ContextId, &sensor_info);
    }

    {
        // Before CE
        ik_wb_gain_t before_ce_wb_gain = {0};

        before_ce_wb_gain.gain_b = 4096;
        before_ce_wb_gain.gain_g = 4096;
        before_ce_wb_gain.gain_r = 4096;
        ik_set_before_ce_wb_gain(pMode0->ContextId, &before_ce_wb_gain);
    }

    {
        // After CE
        ik_wb_gain_t after_ce_wb_gain = {0};

        after_ce_wb_gain.gain_b = 4096;
        after_ce_wb_gain.gain_g = 4096;
        after_ce_wb_gain.gain_r = 4096;
        ik_set_after_ce_wb_gain(pMode0->ContextId, &after_ce_wb_gain);
    }

    {
        // CFA leakage
        ik_cfa_leakage_filter_t cfa_leakage_filter = {0};

        ik_set_cfa_leakage_filter(pMode0->ContextId, &cfa_leakage_filter);
    }

    {
        // Anti aliasing
        ik_anti_aliasing_t anti_aliasing = {0};

        ik_set_anti_aliasing(pMode0->ContextId, &anti_aliasing);
    }

    {
        // DBP
        ik_dynamic_bad_pixel_correction_t dynamic_bpc = {0};

        ik_set_dynamic_bad_pixel_corr(pMode0->ContextId, &dynamic_bpc);
    }

    {
        // SBP
        uint32 enb = 0;
        ik_static_bad_pxl_cor_t static_bpc = {0};

        ik_set_static_bad_pxl_corr_enb(pMode0->ContextId, enb);

        static_bpc.calib_sbp_info.version = 0x20180401;
        static_bpc.calib_sbp_info.sbp_buffer = FPNMap;
        static_bpc.vin_sensor_geo.width = 1920;
        static_bpc.vin_sensor_geo.height = 1080;
        static_bpc.vin_sensor_geo.h_sub_sample.factor_den = 1;
        static_bpc.vin_sensor_geo.h_sub_sample.factor_num = 1;
        static_bpc.vin_sensor_geo.v_sub_sample.factor_den = 1;
        static_bpc.vin_sensor_geo.v_sub_sample.factor_num = 1;
        ik_set_static_bad_pxl_corr(pMode0->ContextId, &static_bpc);
    }

    {
        // CFA noise
        ik_cfa_noise_filter_t cfa_noise_filter = {0};

        ik_set_cfa_noise_filter(pMode0->ContextId, &cfa_noise_filter);
    }

    {
        // Demosaic
        ik_demosaic_t demosaic = {0};

        ik_set_demosaic(pMode0->ContextId, &demosaic);
    }

    {
        // RGB_12Y
        ik_rgb_to_12y_t rgb_to_12y = {0};

        ik_set_rgb_to_12y(pMode0->ContextId, &rgb_to_12y);
    }

    {
        // LNL
        ik_luma_noise_reduction_t luma_noise_reduce = {0};

        ik_set_luma_noise_reduction(pMode0->ContextId, &luma_noise_reduce);
    }

    {
        // Pre CC gain
        ik_pre_cc_gain_t pre_cc_gain = {0};

        ik_set_pre_cc_gain(pMode0->ContextId, &pre_cc_gain);
    }

    {
        // CC
        ik_color_correction_t color_correction = {0};

        ik_set_color_correction(pMode0->ContextId, &color_correction);
    }

    {
        // tone curve
        ik_tone_curve_t tone_curve = {0};

        ik_set_tone_curve(pMode0->ContextId, &tone_curve);
    }

    {
        // RGB to YUV
        ik_rgb_to_yuv_matrix_t rgb_to_yuv_matrix = {0};

        ik_set_rgb_to_yuv_matrix(pMode0->ContextId, &rgb_to_yuv_matrix);
    }

    {
        // RGBIR
        ik_rgb_ir_t rgb_ir = {0};

        ik_set_rgb_ir(pMode0->ContextId, &rgb_ir);
    }

    {
        // Chroma scale
        ik_chroma_scale_t chroma_scale = {0};

        ik_set_chroma_scale(pMode0->ContextId, &chroma_scale);
    }

    {
        // Chroma median
        ik_chroma_median_filter_t chroma_median_filter = {0};

        ik_set_chroma_median_filter(pMode0->ContextId, &chroma_median_filter);
    }

    {
        // SHPA proc mode
        ik_first_luma_process_mode_t first_luma_process_mode = {0};

        ik_set_fst_luma_process_mode(pMode0->ContextId, &first_luma_process_mode);
    }

    {
        // ASF
        ik_adv_spatial_filter_t advance_spatial_filter = {0};

        ik_set_adv_spatial_filter(pMode0->ContextId, &advance_spatial_filter);
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

        ik_set_fst_shp_both(pMode0->ContextId, &first_sharpen_both);

        first_sharpen_noise.level_str_adjust.high = 100;
        ik_set_fst_shp_noise(pMode0->ContextId, &first_sharpen_noise);

        ik_set_fst_shp_fir(pMode0->ContextId, &first_sharpen_fir);

        first_sharpen_coring.fractional_bits = 2;
        ik_set_fst_shp_coring(pMode0->ContextId, &first_sharpen_coring);

        first_sharpen_coring_idx_scale.high = 100;
        ik_set_fst_shp_coring_idx_scale(pMode0->ContextId, &first_sharpen_coring_idx_scale);

        first_sharpen_min_coring_result.high = 100;
        ik_set_fst_shp_min_coring_rslt(pMode0->ContextId, &first_sharpen_min_coring_result);

        first_sharpen_max_coring_result.high = 100;
        ik_set_fst_shp_max_coring_rslt(pMode0->ContextId, &first_sharpen_max_coring_result);

        first_sharpen_scale_coring.high = 100;
        ik_set_fst_shp_scale_coring(pMode0->ContextId, &first_sharpen_scale_coring);
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

        ik_set_fnl_shp_both(pMode0->ContextId, &final_sharpen_both);

        final_sharpen_noise.level_str_adjust.high = 100;
        ik_set_fnl_shp_noise(pMode0->ContextId, &final_sharpen_noise);

        ik_set_fnl_shp_fir(pMode0->ContextId, &final_sharpen_fir);

        final_sharpen_coring.fractional_bits = 2;
        ik_set_fnl_shp_coring(pMode0->ContextId, &final_sharpen_coring);

        final_sharpen_coring_idx_scale.high = 100;
        ik_set_fnl_shp_coring_idx_scale(pMode0->ContextId, &final_sharpen_coring_idx_scale);

        final_sharpen_min_coring_result.high = 100;
        ik_set_fnl_shp_min_coring_rslt(pMode0->ContextId, &final_sharpen_min_coring_result);

        final_sharpen_max_coring_result.high = 100;
        ik_set_fnl_shp_max_coring_rslt(pMode0->ContextId, &final_sharpen_max_coring_result);

        final_sharpen_scale_coring.high = 100;
        ik_set_fnl_shp_scale_coring(pMode0->ContextId, &final_sharpen_scale_coring);

        ik_set_fnl_shp_three_d_table(pMode0->ContextId, &final_sharpen_both_three_d_table);
    }

    {
        // Chroma filter
        ik_chroma_filter_t chroma_filter = {0};

        chroma_filter.radius = 32;
        ik_set_chroma_filter(pMode0->ContextId, &chroma_filter);
    }

    {
        // Wide chroma filter
        ik_wide_chroma_filter_t wide_chroma_filter = {0};

        ik_set_wide_chroma_filter(pMode0->ContextId, &wide_chroma_filter);
    }

    {
        // Wide chroma filter combine
        ik_wide_chroma_filter_combine_t chroma_filter_combine = {0};

        chroma_filter_combine.T1_cb = 10;
        chroma_filter_combine.T1_cr = 10;
        ik_set_wide_chroma_filter_combine(pMode0->ContextId, &chroma_filter_combine);
    }

    {
        // grgb mismatch
        ik_grgb_mismatch_t grgb_mismatch = {0};

        ik_set_grgb_mismatch(pMode0->ContextId, &grgb_mismatch);
    }

    {
        // MCTF
        ik_video_mctf_t video_mctf = {0};
        ik_video_mctf_ta_t video_mctf_ta = {0};
        ik_pos_dep33_t video_mctf_and_final_sharpen = {0};

        ik_set_video_mctf(pMode0->ContextId, &video_mctf);

        ik_set_video_mctf_ta(pMode0->ContextId, &video_mctf_ta);

        ik_set_video_mctf_and_fnl_shp(pMode0->ContextId, &video_mctf_and_final_sharpen);
    }

    {
        // Vig
        uint32 enb = 0;
        ik_vignette_t vignette = {0};

        ik_set_vignette_enb(pMode0->ContextId, enb);
        ik_set_vignette(pMode0->ContextId, &vignette);
    }

    {
        // HDR blc
        ik_static_blc_level_t frontend_static_blc = {0};

        ik_set_exp0_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
        ik_set_exp1_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
        ik_set_exp2_frontend_static_blc(pMode0->ContextId, &frontend_static_blc);
    }

    {
        // HDR gain
        ik_frontend_wb_gain_t frontend_wb_gain = {0};

        frontend_wb_gain.shutter_ratio = 4096;
        ik_set_exp0_frontend_wb_gain(pMode0->ContextId, &frontend_wb_gain);
        ik_set_exp1_frontend_wb_gain(pMode0->ContextId, &frontend_wb_gain);
        ik_set_exp2_frontend_wb_gain(pMode0->ContextId, &frontend_wb_gain);
    }

    {
        // CE
        ik_ce_t ce = {0};
        ik_ce_input_table_t ce_input_table = {0};
        ik_ce_output_table_t ce_out_table = {0};
        ce.coring_gain_high = 1U;
        ce.coring_index_scale_high = 1U;

        ik_set_ce(pMode0->ContextId, &ce);
        ik_set_ce_input_table(pMode0->ContextId, &ce_input_table);
        ik_set_ce_out_table(pMode0->ContextId, &ce_out_table);
    }

    {
        // HDR blend
        ik_hdr_blend_t hdr_blend = {0};

        ik_set_hdr_blend(pMode0->ContextId, &hdr_blend);
    }

    {
        // HDR tone curve
        ik_frontend_tone_curve_t fe_tc = {0};

        ik_set_frontend_tone_curve(pMode0->ContextId, &fe_tc);
    }

    {
        // resampler strength
        ik_resampler_strength_t resampler_strength = {0};

        ik_set_resampler_strength(pMode0->ContextId, &resampler_strength);
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
        ik_set_aaa_stat_info(pMode0->ContextId, &stat_info);
        ik_set_aaa_pg_af_stat_info(pMode0->ContextId, &pg_af_stat_info);
        ik_set_af_stat_ex_info(pMode0->ContextId, &af_stat_ex_info);
        ik_set_pg_af_stat_ex_info(pMode0->ContextId, &pg_af_stat_ex_info);
        ik_set_histogram_info(pMode0->ContextId, &hist_info);
        ik_set_pg_histogram_info(pMode0->ContextId, &hist_info);
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
        ik_set_window_size_info(pMode0->ContextId, &window_size_info);
    }

    {
        // Warp
        uint32 enb = 0;
        ik_warp_info_t calib_warp_info = {0};

        ik_set_warp_enb(pMode0->ContextId, enb);

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
        ik_set_warp_info(pMode0->ContextId, &calib_warp_info);
    }

    {
        // CA
        uint32 enb = 0;
        ik_cawarp_info_t calib_ca_warp_info = {0};

        ik_set_cawarp_enb(pMode0->ContextId, enb);

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
        ik_set_cawarp_info(pMode0->ContextId, &calib_ca_warp_info);
    }

    {
        // DZoom
        ik_dzoom_info_t dzoom_info = {0};

        ik_set_dzoom_info(pMode0->ContextId, &dzoom_info);
    }

    {
        // Dummy
        ik_dummy_margin_range_t dmy_margin_range = {0};

        ik_set_dummy_margin_range(pMode0->ContextId, &dmy_margin_range);
    }

    {
        // Active win
        ik_vin_active_window_t vin_active_win = {0};

        ik_set_vin_active_win(pMode0->ContextId, &vin_active_win);
    }

    {
        // HDR RAW info
        ik_hdr_raw_info_t hdr_raw_info = {0};

        ik_set_hdr_raw_offset(pMode0->ContextId, &hdr_raw_info);
    }

    {
        // flip mode
        uint32 mode = 0;

        ik_set_flip_mode(pMode0->ContextId, mode);
    }

    {
        // Warp buf info
        ik_warp_buffer_info_t warp_buf_info = {0};

        warp_buf_info.luma_wait_lines = 8;
        warp_buf_info.luma_dma_size = 32;
        ik_set_warp_buffer_info(pMode0->ContextId, &warp_buf_info);
    }

    {
        // Stitch info
        ik_stitch_info_t stitch_info = {0};

        stitch_info.tile_num_x = 1;
        stitch_info.tile_num_y = 1;
        ik_set_stitching_info(pMode0->ContextId, &stitch_info);
    }

    {
        // Burst tile
        ik_burst_tile_t burst_tile = {0};

        ik_set_burst_tile(pMode0->ContextId, &burst_tile);
    }
}

static void IK_TestCovr_ImgExecuterUnitWindowCalc(void)
{
    UINT32 Rval = IK_OK;
    amba_ik_win_calc_t info= {0};
    ik_window_size_info_t window_size_info= {0};
    ik_vin_active_window_t active_window= {0};
    ik_dummy_margin_range_t dmy_range= {0};
    ik_dzoom_info_t dzoom_info= {0};
    ik_stitch_info_t stitching_info= {0};
    amba_ik_calc_win_result_t result_win= {0};
    extern uint32 exe_win_calc(const amba_ik_win_calc_t *p_info);

    info.warp_enable = 0;
    info.ability = AMBA_IK_VIDEO_LINEAR;

    Rval = exe_win_calc(NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "exe_win_calc", "null-ptr fail case", id_cnt++);

    Rval = exe_win_calc(&info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "exe_win_calc", "null-ptr fail case", id_cnt++);

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
    window_size_info.vin_sensor.start_x = 0;
    window_size_info.vin_sensor.start_y = 0;
    window_size_info.vin_sensor.width = 4096;
    window_size_info.vin_sensor.height = 3008;

    window_size_info.main_win.width = 4096;
    window_size_info.main_win.width = 3008;
    window_size_info.prev[0].width = 1280;
    window_size_info.prev[0].height = 720;
    window_size_info.prev[1].width = 1280;
    window_size_info.prev[1].height = 720;
    window_size_info.prev[2].width = 1280;
    window_size_info.prev[2].height = 720;
    window_size_info.thumbnail.width = 1280;
    window_size_info.thumbnail.height = 720;
    window_size_info.screennail.width = 1280;
    window_size_info.screennail.height = 720;

    active_window.enable = 0;
    dmy_range.enable = 1;
    dmy_range.right = 32768;
    dzoom_info.enable = 0;
    stitching_info.enable = 1;
    stitching_info.tile_num_x = 1;
    stitching_info.tile_num_y = 1;

    Rval = exe_win_calc(&info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "exe_win_calc", "stitching fail case", id_cnt++);

    stitching_info.enable = 1;
    stitching_info.tile_num_x = 2;
    stitching_info.tile_num_y = 1;

    active_window.enable = 1;
    active_window.active_geo.start_x = 128;
    active_window.active_geo.start_y = 16;
    active_window.active_geo.width = 3840;
    active_window.active_geo.height = 2160;

    dzoom_info.enable = 1;
    dzoom_info.zoom_x = 65536*2;
    dzoom_info.zoom_y = 65536*2;

    info.warp_enable = 1;
    dmy_range.enable = 1;
    dmy_range.left = 65536;
    dmy_range.right = 32768;
    dmy_range.top = 65536;
    dmy_range.bottom = 65536;

    Rval = exe_win_calc(&info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "exe_win_calc", "OK case", id_cnt++);

    info.warp_enable = 0;

    active_window.active_geo.start_x = 0;
    active_window.active_geo.start_y = 0;

    dmy_range.enable = 1;
    dmy_range.left = 32768;
    dmy_range.right = 65536;

    Rval = exe_win_calc(&info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "exe_win_calc", "OK case", id_cnt++);

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

    Rval = exe_win_calc(&info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "exe_win_calc", "OK case", id_cnt++);

    dzoom_info.enable = 1;
    dzoom_info.zoom_x = 65537;
    dzoom_info.zoom_y = 65537;
    dzoom_info.shift_x = 65536*-1;
    dzoom_info.shift_y = 65536*-1;

    Rval = exe_win_calc(&info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitWindowCalc.c", "exe_win_calc", "OK case", id_cnt++);

}

static void IK_TestCovr_ImgExecuterUnitConfig(void)
{
    UINT32 Rval = IK_OK;
    idsp_ik_flow_ctrl_t flow= {0};
    amba_ik_flow_tables_list_t flow_tbl_list= {0};
    amba_ik_filter_t filters= {0};

    extern uint32 exe_process_calib_cr(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters);
    extern uint32 exe_process_hdr_ce_cr(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num);
    extern uint32 exe_process_cfa_cr(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters);
    extern uint32 exe_process_rgb_cr(amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters);
    extern uint32 exe_process_yuv_cr(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters);
    extern uint32 exe_process_hdr_ce_sub_cr(amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num);
    extern uint32 exe_process_mctf_mcts_cr(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters);

    memset(&filters.update_flags.iso, 0x1, sizeof(filters.update_flags.iso));
    Rval = exe_process_calib_cr(&flow, &flow_tbl_list, &filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_calib_cr", "ring buffer fail case", id_cnt++);
    filters.input_param.ctx_buf.sbp_internal_mode_flag = 1u;
    filters.input_param.ctx_buf.cawarp_internal_mode_flag = 1u;
    filters.input_param.ctx_buf.warp_internal_mode_flag = 1u;
    filters.input_param.sensor_info.sensor_mode = 2;
    Rval = exe_process_calib_cr(&flow, &flow_tbl_list, &filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_calib_cr", "ring buffer fail case", id_cnt++);
    filters.input_param.ctx_buf.sbp_internal_mode_flag = 0u;
    filters.input_param.ctx_buf.cawarp_internal_mode_flag = 0u;
    filters.input_param.ctx_buf.warp_internal_mode_flag = 0u;

    filters.input_param.sensor_info.sensor_mode = 0;
    filters.input_param.rgb_ir.mode = 1;
    Rval = exe_process_calib_cr(&flow, &flow_tbl_list, &filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_calib_cr", "invalid sensor mode + ir_mode fail case", id_cnt++);

    filters.input_param.sensor_info.sensor_mode = 1;
    filters.input_param.rgb_ir.mode = 0;
    Rval = exe_process_calib_cr(&flow, &flow_tbl_list, &filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_calib_cr", "invalid sensor mode + ir_mode fail case", id_cnt++);

    filters.input_param.sensor_info.sensor_mode = 2;
    filters.input_param.rgb_ir.mode = 1;
    Rval = exe_process_calib_cr(&flow, &flow_tbl_list, &filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_calib_cr", "invalid sensor mode + ir_mode fail case", id_cnt++);
    filters.input_param.sensor_info.sensor_mode = 0;
    filters.input_param.rgb_ir.mode = 0;

    filters.update_flags.iso.warp_enable_updated = 1;
    filters.update_flags.cr.warp = 0;
    filters.update_flags.iso.calib_warp_info_updated = 0;
    Rval = exe_process_calib_cr(&flow, &flow_tbl_list, &filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "warp_flow_table_op_u32", "(compare2 == 1U) false case", id_cnt++);
    filters.update_flags.iso.warp_enable_updated = 0;

    filters.update_flags.iso.is_1st_frame = 0;
    filters.input_param.sensor_info.compression = IK_RAW_COMPRESS_10p5;
    Rval = exe_process_hdr_ce_cr(&flow, &flow_tbl_list, &filters, 3);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_hdr_ce_cr", "ring buffer fail case", id_cnt++);
    filters.update_flags.iso.is_1st_frame = 1u;
    filters.input_param.sensor_info.compression = 0u;

    filters.input_param.fe_tone_curve.decompand_enable = 1u;
    filters.input_param.sensor_info.compression_offset = 1u;
    filters.input_param.sensor_info.compression = IK_RAW_COMPACT_8B;
    Rval = exe_process_hdr_ce_cr(&flow, &flow_tbl_list, &filters, 3);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_hdr_ce_cr", "decompand + compression offset fail case", id_cnt++);
    filters.input_param.fe_tone_curve.decompand_enable = 0u;
    filters.input_param.sensor_info.compression_offset = 0u;
    filters.input_param.sensor_info.compression = 0u;

    filters.input_param.ability = AMBA_IK_VIDEO_Y2Y;
    Rval = exe_process_hdr_ce_cr(&flow, &flow_tbl_list, &filters, 3);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_hdr_ce_cr", "ability fail case", id_cnt++);
    filters.input_param.ability = 0;

    Rval = exe_process_hdr_ce_cr(&flow, &flow_tbl_list, &filters, 4);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_hdr_ce_cr", "exposure number fail case", id_cnt++);

    Rval = exe_process_cfa_cr(&flow, &flow_tbl_list, &filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_cfa_cr", "ring buffer fail case", id_cnt++);

    Rval = exe_process_rgb_cr(&flow_tbl_list, &filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_rgb_cr", "ring buffer fail case", id_cnt++);

    filters.input_param.ability = AMBA_IK_VIDEO_Y2Y;
    filters.input_param.use_cc_for_yuv2yuv = 0u;
    filters.input_param.use_cc = 1u;
    filters.input_param.use_tone_curve = 1u;
    Rval = exe_process_rgb_cr(&flow_tbl_list, &filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_rgb_cr", "ring buffer fail case", id_cnt++);
    filters.input_param.ability = 0;
    filters.input_param.use_cc = 0u;
    filters.input_param.use_tone_curve = 0u;

    filters.input_param.ability = AMBA_IK_VIDEO_Y2Y;
    filters.update_flags.iso.is_1st_frame = 0u;
    filters.update_flags.iso.color_correction_updated = 0u;
    filters.update_flags.iso.tone_curve_updated = 0u;
    Rval = exe_process_rgb_cr(&flow_tbl_list, &filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_rgb_cr", "ring buffer fail case", id_cnt++);
    filters.input_param.ability = 0;
    filters.update_flags.iso.is_1st_frame = 1u;
    filters.update_flags.iso.color_correction_updated = 1u;
    filters.update_flags.iso.tone_curve_updated = 1u;

    Rval = exe_process_yuv_cr(&flow, &flow_tbl_list, &filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_yuv_cr", "ring buffer fail case", id_cnt++);

    filters.input_param.first_luma_process_mode.use_sharpen_not_asf  = 1u;
    filters.update_flags.iso.tone_curve_updated = 1u;
    Rval = exe_process_yuv_cr(&flow, &flow_tbl_list, &filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_yuv_cr", "ring buffer fail case", id_cnt++);
    filters.input_param.first_luma_process_mode.use_sharpen_not_asf  = 0u;
    filters.update_flags.iso.tone_curve_updated = 0u;

    filters.input_param.first_luma_process_mode.use_sharpen_not_asf  = 1u;
    filters.update_flags.iso.advance_spatial_filter_updated = 0u;
    filters.update_flags.iso.fstshpns_both_updated = 0u;
    filters.update_flags.iso.fstshpns_noise_updated = 0u;
    filters.update_flags.iso.fstshpns_fir_updated = 0u;
    filters.update_flags.iso.fstshpns_coring_updated = 0u;
    filters.update_flags.iso.fstshpns_coring_index_scale_updated = 0u;
    filters.update_flags.iso.fstshpns_min_coring_result_updated = 0u;
    filters.update_flags.iso.fstshpns_max_coring_result_updated = 0u;
    filters.update_flags.iso.fstshpns_scale_coring_updated = 0u;
    Rval = exe_process_yuv_cr(&flow, &flow_tbl_list, &filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_yuv_cr", "ring buffer fail case", id_cnt++);
    filters.input_param.first_luma_process_mode.use_sharpen_not_asf  = 0u;
    filters.update_flags.iso.advance_spatial_filter_updated = 1u;
    filters.update_flags.iso.fstshpns_both_updated = 1u;
    filters.update_flags.iso.fstshpns_noise_updated = 1u;
    filters.update_flags.iso.fstshpns_fir_updated = 1u;
    filters.update_flags.iso.fstshpns_coring_updated = 1u;
    filters.update_flags.iso.fstshpns_coring_index_scale_updated = 1u;
    filters.update_flags.iso.fstshpns_min_coring_result_updated = 1u;
    filters.update_flags.iso.fstshpns_max_coring_result_updated = 1u;
    filters.update_flags.iso.fstshpns_scale_coring_updated = 1u;

    filters.input_param.burst_tile.enable = 1;
    filters.input_param.warp_buf_info.dram_efficiency = 2;
    Rval = exe_process_yuv_cr(&flow, &flow_tbl_list, &filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_yuv_cr", "invalid burst tile mode + dram_efficiency fail case", id_cnt++);
    filters.input_param.burst_tile.enable = 0;
    filters.input_param.warp_buf_info.dram_efficiency = 0;

    filters.input_param.sensor_info.compression = IK_RAW_COMPRESS_10p5;
    Rval = exe_process_hdr_ce_sub_cr(&flow_tbl_list, &filters, 3);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_hdr_ce_sub_cr", "ring buffer fail case", id_cnt++);
    filters.input_param.sensor_info.compression = 0u;

    filters.input_param.ability = AMBA_IK_VIDEO_Y2Y;
    Rval = exe_process_hdr_ce_sub_cr(&flow_tbl_list, &filters, 3);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_hdr_ce_sub_cr", "ability fail case", id_cnt++);
    filters.input_param.ability = 0;

    Rval = exe_process_hdr_ce_sub_cr(&flow_tbl_list, &filters, 4);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_hdr_ce_sub_cr", "exposure number fail case", id_cnt++);

    Rval = exe_process_hdr_ce_sub_cr(&flow_tbl_list, &filters, 0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_hdr_ce_sub_cr", "exposure number fail case", id_cnt++);

    Rval = exe_process_mctf_mcts_cr(&flow, &flow_tbl_list, &filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_mctf_mcts_cr", "ring buffer fail case", id_cnt++);

    filters.input_param.video_mctf.y_temporal_either_max_change_or_t0_t1_add.method = 1U;
    filters.input_param.video_mctf.cb_temporal_either_max_change_or_t0_t1_add.method = 1U;
    filters.input_param.video_mctf.cr_temporal_either_max_change_or_t0_t1_add.method = 1U;
    filters.input_param.video_mctf.y_spat_blend.method = 1U;
    filters.input_param.video_mctf.cb_spat_blend.method = 1U;
    filters.input_param.video_mctf.cr_spat_blend.method = 1U;
    filters.input_param.video_mctf.y_advanced_iso_max_change_method = 1U;
    filters.input_param.video_mctf.cb_advanced_iso_max_change_method = 1U;
    filters.input_param.video_mctf.cr_advanced_iso_max_change_method = 1U;
    filters.input_param.video_mctf.y_3d_maxchange.method = 1U;
    filters.input_param.video_mctf.cb_3d_maxchange.method = 1U;
    filters.input_param.video_mctf.cr_3d_maxchange.method = 1U;
    filters.input_param.video_mctf.y_overall_max_change.method = 1U;
    filters.input_param.video_mctf.cb_overall_max_change.method = 1U;
    filters.input_param.video_mctf.cr_overall_max_change.method = 1U;
    filters.input_param.video_mctf.y_spat_filt_max_smth_change.method = 1U;
    filters.input_param.video_mctf.cb_spat_filt_max_smth_change.method = 1U;
    filters.input_param.video_mctf.cr_spat_filt_max_smth_change.method = 1U;
    filters.input_param.video_mctf.y_spat_smth_dir.method = 1U;
    filters.input_param.video_mctf.cb_spat_smth_dir.method = 1U;
    filters.input_param.video_mctf.cr_spat_smth_dir.method = 1U;
    filters.input_param.video_mctf.y_spat_smth_iso.method = 1U;
    filters.input_param.video_mctf.cb_spat_smth_iso.method = 1U;
    filters.input_param.video_mctf.cr_spat_smth_iso.method = 1U;
    filters.input_param.video_mctf.y_level_based_ta.method = 1U;
    filters.input_param.video_mctf.cb_level_based_ta.method = 1U;
    filters.input_param.video_mctf.cr_level_based_ta.method = 1U;
    filters.input_param.video_mctf.y_temporal_min_target.method = 1U;
    filters.input_param.video_mctf.cb_temporal_min_target.method = 1U;
    filters.input_param.video_mctf.cr_temporal_min_target.method = 1U;
    filters.input_param.video_mctf.y_spatial_max_temporal.method = 1U;
    filters.input_param.video_mctf.cb_spatial_max_temporal.method = 1U;
    filters.input_param.video_mctf.cr_spatial_max_temporal.method = 1U;
    filters.input_param.final_sharpen_noise.level_str_adjust.method = 1U;
    filters.input_param.final_sharpen_noise.advanced_iso.max_change_method = 1U;
    filters.input_param.final_sharpen_noise.advanced_iso.noise_level_method = 1U;
    filters.input_param.final_sharpen_noise.advanced_iso.str_a_method = 1U;
    filters.input_param.final_sharpen_noise.advanced_iso.str_b_method = 1U;
    filters.input_param.final_sharpen_coring_idx_scale.method = 1U;
    filters.input_param.final_sharpen_min_coring_result.method = 1U;
    filters.input_param.final_sharpen_max_coring_result.method = 1U;
    filters.input_param.final_sharpen_scale_coring.method = 1U;
    filters.input_param.final_sharpen_coring_idx_scale.method = 1U;
    Rval = exe_process_mctf_mcts_cr(&flow, &flow_tbl_list, &filters);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_process_mctf_mcts_cr", "mctf check fail case", id_cnt++);
    memset(&filters.input_param, 0x0, sizeof(filters.input_param));

    {
        //group_cmd
        ik_ability_t ability= {0};
        ik_calc_geo_settings_t info= {0};
        ik_calc_geo_result_t result= {0};
        ik_calc_geo_settings_inpar_t info_in= {0};
        ik_warp_info_t calib_warp_info= {0};
        ik_cawarp_info_t calib_ca_warp_info= {0};
        ik_window_size_info_t window_size_info= {0};
        ik_rgb_ir_t rgb_ir= {0};

        extern uint32 exe_query_calc_geo_buf_size(ik_query_calc_geo_buf_size_t *p_buf_size_info);
        extern uint32 exe_calc_geo_settings(const ik_ability_t *p_ability, const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result);

        Rval = exe_query_calc_geo_buf_size(NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_query_calc_geo_buf_size", "null-ptr fail case", id_cnt++);

        Rval = exe_calc_geo_settings(NULL, NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_calc_geo_settings", "null-ptr fail case", id_cnt++);

        Rval = exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_calc_geo_settings", "null-ptr fail case", id_cnt++);

        info.p_in = &info_in;
        info.p_work_buf = (void *) (ALIGN_N((uintptr)work_buf+ 120000, 128)+1);
        info.work_buf_size = 8832-1;

        result.p_out = (void *) (ALIGN_N((uintptr)work_buf, 32)+1);
        result.out_buf_size = 111104-1;

        info_in.p_window_size_info = NULL;
        info_in.cawarp_enable = 1;
        info_in.use_cawarp = 1;
        info_in.p_calib_ca_warp_info = NULL;
        info_in.warp_enable = 1;
        info_in.p_calib_warp_info = NULL;

        Rval = exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_calc_geo_settings", "null-ptr fail case", id_cnt++);

        Rval = exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_calc_geo_settings", "null-ptr fail case", id_cnt++);

        info_in.p_calib_warp_info = &calib_warp_info;
        info_in.p_calib_ca_warp_info = &calib_ca_warp_info;
        calib_warp_info.pwarp = NULL;
        calib_ca_warp_info.p_cawarp_red = NULL;
        calib_ca_warp_info.p_cawarp_blue = NULL;

        Rval = exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_calc_geo_settings", "null-ptr fail case", id_cnt++);

        ability.pipe = AMBA_IK_PIPE_STILL;
        Rval = exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_calc_geo_settings", "invalid ability fail case", id_cnt++);
        ability.pipe =  AMBA_IK_PIPE_VIDEO;

        info_in.cawarp_enable = 0;
        info_in.use_cawarp = 0;
        info_in.warp_enable = 0;
        info_in.p_window_size_info = &window_size_info;
        Rval = exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_calc_geo_settings", "invalid geometry fail case", id_cnt++);

        ability.pipe = AMBA_IK_PIPE_VIDEO;
        ability.video_pipe = AMBA_IK_VIDEO_Y2Y;

        info.p_in = &info_in;
        info.p_work_buf = (void *) ALIGN_N((uintptr)work_buf + 120000, 128);
        info.work_buf_size = 8832;

        result.p_out = (void *) ALIGN_N((uintptr)work_buf, 32);
        result.out_buf_size = 111104;

        window_size_info.vin_sensor.v_sub_sample.factor_num = 1;
        window_size_info.vin_sensor.v_sub_sample.factor_den = 1;
        window_size_info.vin_sensor.h_sub_sample.factor_num = 1;
        window_size_info.vin_sensor.h_sub_sample.factor_den = 1;
        window_size_info.vin_sensor.width = 1920;
        window_size_info.vin_sensor.height = 1080;
        window_size_info.main_win.width = 1920;
        window_size_info.main_win.height = 1080;

        info_in.p_rgb_ir = &rgb_ir;
        info_in.chroma_filter_radius = 32;

        Rval = exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_calc_geo_settings", "OK case", id_cnt++);

        rgb_ir.ir_only = 1;
        ability.video_pipe = AMBA_IK_VIDEO_LINEAR;
        Rval = exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_calc_geo_settings", "OK case", id_cnt++);
        rgb_ir.ir_only = 0;

        window_size_info.vin_sensor.width = 3840;
        window_size_info.vin_sensor.height = 2160;
        Rval = exe_calc_geo_settings(&ability, &info, &result);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterUnitConfig.c", "exe_calc_geo_settings", "window calculate fail case", id_cnt++);

    }

}

static void IK_TestCovr_ImgExecuterComponentIF(void)
{
    UINT32 Rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    ik_query_frame_info_t frame_info = {0};
    amba_ik_filter_update_flags_t update_flag_dummy= {0};
    amba_ik_cr_index_t table_index_dummy= {0};
    idsp_ik_flow_ctrl_t flow_dummy= {0};
    amba_ik_flow_tables_list_t flow_tbl_list_dummy= {0};
    amba_ik_filter_t filters_dummy= {0};
    ik_ability_t ability= {0};
    amba_ik_exe_cr_utilities_t utilities= {0};
    UINT32 tmp;
    UINT64 tmp_u64;

    (void)img_ctx_get_context(0, &p_ctx);
    tmp = p_ctx->organization.active_cr_state.active_flow_idx;
    p_ctx->organization.active_cr_state.active_flow_idx = 999;
    Rval = img_exe_inject_safety_error(0,0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_inject_safety_error", "ring buffer fail case", id_cnt++);

    p_ctx->organization.active_cr_state.active_flow_idx = tmp;
    Rval = img_exe_inject_safety_error(799,0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_inject_safety_error", "ctx-id fail case", id_cnt++);

    Rval = img_exe_inject_safety_error(0,799);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_inject_safety_error", "invalid params fail case", id_cnt++);

    img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    tmp = p_ik_buffer_info->safety_crc_enable;
    p_ik_buffer_info->safety_crc_enable = 0;
    Rval = img_exe_inject_safety_error(0,AMBA_IK_SAFETY_ERROR_CRC_MISMATCH);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_inject_safety_error", "ring buffer fail case", id_cnt++);
    p_ik_buffer_info->safety_crc_enable = tmp;

    Rval = img_exe_init_safety_executer(799);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_init_safety_executer", "ctx-id fail case", id_cnt++);

    Rval = img_exe_query_frame_info(0, 0, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_query_frame_info", "null-ptr fail case", id_cnt++);

    Rval = img_exe_query_frame_info(799, 0, &frame_info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_query_frame_info", "ctx-id fail case", id_cnt++);

    Rval = img_exe_query_frame_info(0, 0, &frame_info);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_query_frame_info", "invalid ik-id fail case", id_cnt++);

    {
        amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
        uintptr addr;
        void* ptr_tmp;

        tmp = p_ctx->organization.active_cr_state.cr_running_number;
        p_ctx->organization.active_cr_state.cr_running_number = 2;//hack runnung number = 2nd frame.
        (void)img_ctx_get_flow_tbl_list(0, 0/*runnung number-1*/, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
        ptr_tmp = p_flow_tbl_list->p_frame_info;//keep original p_frame_info;
        p_flow_tbl_list->p_frame_info = NULL;//hack p_frame_info = NULL for corner case testing.
        Rval = img_exe_query_frame_info(0, 1, &frame_info);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_query_frame_info", "null flow_tbl_list content fail case", id_cnt++);
        p_flow_tbl_list->p_frame_info = ptr_tmp;//revert p_frame_info
        p_ctx->organization.active_cr_state.cr_running_number = tmp;//revert runnung number.
    }

    Rval = img_exe_init_executer(799);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_init_executer", "ctx-id fail case", id_cnt++);

    tmp = p_ctx->organization.initial_flag;
    p_ctx->organization.initial_flag = 0;
    Rval = img_exe_init_executer(0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_init_executer", "non-init fail case", id_cnt++);
    p_ctx->organization.initial_flag = tmp;

    tmp = p_ctx->organization.attribute.ability.pipe;
    p_ctx->organization.attribute.ability.pipe = 99;
    Rval = img_exe_init_executer(0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_init_executer", "invalid pipe fail case", id_cnt++);
    p_ctx->organization.attribute.ability.pipe = tmp;

    tmp = p_ctx->organization.attribute.ability.video_pipe;
    p_ctx->organization.attribute.ability.video_pipe = 999;
    Rval = img_exe_init_executer(0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_init_executer", "invalid pipe fail case", id_cnt++);
    p_ctx->organization.attribute.ability.video_pipe = tmp;

    //_static function test
    //static uint32 exe_query_frame_info_check_run_num(uint32 current_running_num, uint32 max_running_num, uint32 running_num, uint32 cr_ring_number);
    Rval = exe_query_frame_info_check_run_num(1, 65532, 65535, 4);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_query_frame_info_check_run_num", "running number fail case", id_cnt++);

    Rval = exe_query_frame_info_check_run_num(65530, 65532, 1, 4);//gap = 65529
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_query_frame_info_check_run_num", "running number fail case", id_cnt++);

    Rval = exe_query_frame_info_check_run_num(1, 65532, 65530, 4);//gap = 3
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_query_frame_info_check_run_num", "running number OK case", id_cnt++);

    Rval = exe_query_frame_info_check_run_num(1, 65532, 65525, 4);//gap = 7
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_query_frame_info_check_run_num", "running number fail case", id_cnt++);

    Rval = exe_query_frame_info_check_run_num(1, 65532, 1, 4);//gap = 65532
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_query_frame_info_check_run_num", "running number fail case", id_cnt++);

    Rval = exe_clone_flow_control_and_tbl_list(799, NULL, NULL, 999);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_clone_flow_control_and_tbl_list", "ctx-id fail case", id_cnt++);

    Rval = exe_get_next_flow_control_index(0, NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_flow_control_index", "null-ptr fail case", id_cnt++);

    Rval = exe_get_next_cr_index(0, 0, NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index", "null-ptr fail case", id_cnt++);

    Rval = exe_get_next_cr_index(799, 1, &update_flag_dummy, &table_index_dummy);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index", "ctx-id fail case", id_cnt++);

    update_flag_dummy.iso.sensor_information_updated = 1u;
    Rval = exe_get_next_cr_index(0, 1, &update_flag_dummy, &table_index_dummy);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part1", "if (p_update_flag->iso.sensor_information_updated != 0U) case", id_cnt++);
    update_flag_dummy.iso.sensor_information_updated = 0u;

    update_flag_dummy.iso.chroma_filter_updated = 1u;
    update_flag_dummy.iso.calib_warp_info_updated = 1u;
    Rval = exe_get_next_cr_index(0, 1, &update_flag_dummy, &table_index_dummy);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part1", "if((non_equal_op_u32(p_update_flag->iso.calib_warp_info_updated, 0U) + non_equal_op_u32(p_update_flag->iso.warp_internal_updated, 0U)) != 0U) true OK case", id_cnt++);
    update_flag_dummy.iso.calib_warp_info_updated = 0u;
    update_flag_dummy.iso.chroma_filter_updated = 0u;

    update_flag_dummy.iso.dgain_sat_lvl_updated  = 1u;
    Rval = exe_get_next_cr_index(0, 1, &update_flag_dummy, &table_index_dummy);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part2", "if (p_update_flag->iso.dgain_sat_lvl_updated  != 0U) case", id_cnt++);
    update_flag_dummy.iso.dgain_sat_lvl_updated  = 0u;

    update_flag_dummy.iso.pre_cc_gain_updated  = 1u;
    Rval = exe_get_next_cr_index(0, 1, &update_flag_dummy, &table_index_dummy);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_next_cr_index_part2", "if (p_update_flag->iso.pre_cc_gain_updated  != 0U) case", id_cnt++);
    update_flag_dummy.iso.pre_cc_gain_updated  = 0u;

    Rval = exe_check_next_cr_index(799, NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_check_next_cr_index", "ctx-id fail case", id_cnt++);

    ability.pipe = AMBA_IK_PIPE_VIDEO;
    ability.video_pipe = AMBA_IK_VIDEO_LINEAR_CE;
    Rval = exe_get_cr_utilities(&ability, &utilities);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_cr_utilities", "hdr_1x OK case", id_cnt++);

    Rval = utilities.compute_cr(&flow_dummy, &flow_tbl_list_dummy, &filters_dummy);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_compute_hdr_1x_cr", "hdr_1x OK case", id_cnt++);

    ability.pipe = AMBA_IK_PIPE_VIDEO;
    ability.video_pipe = AMBA_IK_VIDEO_MAX;
    Rval = exe_get_cr_utilities(&ability, &utilities);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_cr_utilities", "video pipe fail case", id_cnt++);

    ability.pipe = 99;
    ability.video_pipe = AMBA_IK_VIDEO_MAX;
    Rval = exe_get_cr_utilities(&ability, &utilities);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_cr_utilities", "pipe fail case", id_cnt++);

    Rval = utilities.compute_cr(&flow_dummy, &flow_tbl_list_dummy, &filters_dummy);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_cmpt_invalid_cr_utility", "invalid execute utility fail case", id_cnt++);

    Rval = utilities.compute_cr(NULL, NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_cmpt_invalid_cr_utility", "invalid execute utility fail case", id_cnt++);

    Rval = exe_set_flow_header(799, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_set_flow_header", "ctx-id fail case", id_cnt++);

    Rval = exe_clean_flow_ctrl_cache_sec2(NULL, 0, NULL, 0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_clean_flow_ctrl_cache_sec2", "wron cr_id fail case", id_cnt++);

    Rval = exe_clean_flow_ctrl_cache_sec2(NULL, 0, NULL, 999);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_clean_flow_ctrl_cache_sec2", "wron cr_id fail case", id_cnt++);

    Rval = exe_clean_flow_ctrl_cache_sec3(NULL, 0, NULL, 0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_clean_flow_ctrl_cache_sec3", "wron cr_id fail case", id_cnt++);

    Rval = exe_clean_flow_ctrl_cache_sec3(NULL, 0, NULL, 999);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_clean_flow_ctrl_cache_sec3", "wron cr_id fail case", id_cnt++);

    Rval = exe_clean_flow_ctrl_cache_sec4(NULL, 0, NULL, 0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_clean_flow_ctrl_cache_sec4", "wron cr_id fail case", id_cnt++);

    Rval = exe_clean_flow_ctrl_cache_sec4(NULL, 0, NULL, 999);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_clean_flow_ctrl_cache_sec4", "wron cr_id fail case", id_cnt++);

    Rval = exe_clean_flow_ctrl_cache_sec18(NULL, 0, NULL, 0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_clean_flow_ctrl_cache_sec18", "wron cr_id fail case", id_cnt++);

    Rval = exe_clean_flow_ctrl_cache_sec18(NULL, 0, NULL, 999);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_clean_flow_ctrl_cache_sec18", "wron cr_id fail case", id_cnt++);

    Rval = exe_clean_flow_ctrl_cache(NULL, NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_clean_flow_ctrl_cache", "null-ptr fail case", id_cnt++);

    Rval = exe_flow_ctrl_ring_check(0, NULL, NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_flow_ctrl_ring_check", "null-ptr fail case", id_cnt++);

    tmp_u64 = 0;
    Rval = ik_cr_memory_fence_check(0, 6, (uint8*)&tmp_u64);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "ik_cr_memory_fence_check", "CR fence corruption fail case", id_cnt++);

    {
        //exe_get_previous_flow_control_index(const amba_ik_context_organization_t *p_ctx_org, const uint32 idx, uint32 previous_count, uint32 *p_flow_idx)
        amba_ik_context_organization_t ctx_org = {0};

        Rval = exe_get_previous_flow_control_index(NULL, 0, 0, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_previous_flow_control_index", "null-ptr fail case", id_cnt++);

        ctx_org.attribute.cr_ring_number = 5;
        Rval = exe_get_previous_flow_control_index(&ctx_org, 0, 6, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_get_previous_flow_control_index", "invalid previous_count fail case", id_cnt++);
    }

    Rval = exe_ik_check_cr_mem_fence_4(799, 0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_check_cr_mem_fence_4", "ctx-id fail case", id_cnt++);

    Rval = exe_ik_check_cr_mem_fence_4(0, 999);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_check_cr_mem_fence_4", "flow_idx fail case", id_cnt++);

    Rval = exe_ik_check_cr_mem_fence_3(799, 0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_check_cr_mem_fence_3", "ctx-id fail case", id_cnt++);

    Rval = exe_ik_check_cr_mem_fence_3(0, 999);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_check_cr_mem_fence_3", "flow_idx fail case", id_cnt++);

    Rval = exe_ik_check_cr_mem_fence_2(799, 0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_check_cr_mem_fence_2", "ctx-id fail case", id_cnt++);

    Rval = exe_ik_check_cr_mem_fence_2(0, 999);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_check_cr_mem_fence_2", "flow_idx fail case", id_cnt++);

    Rval = exe_ik_check_cr_mem_fence_1(799, 0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_check_cr_mem_fence_1", "ctx-id fail case", id_cnt++);

    Rval = exe_ik_check_cr_mem_fence_1(0, 999);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_check_cr_mem_fence_1", "flow_idx fail case", id_cnt++);

    Rval = exe_ik_check_flow_tbl_memory_fence(799);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_check_flow_tbl_memory_fence", "ctx-id fail case", id_cnt++);

    Rval = exe_ik_check_ctx_mem_fence_2(799);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_check_ctx_mem_fence_2", "ctx-id fail case", id_cnt++);

    Rval = exe_ik_check_ctx_mem_fence_1(799);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_check_ctx_mem_fence_2", "ctx-id fail case", id_cnt++);

    {
        amba_ik_context_entity_t ctx = {0};
        const amba_ik_flow_tables_list_t flow_tbl_list = {0};
        Rval = img_exe_inject_safety_error_mem_fence(&ctx, &flow_tbl_list);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_inject_safety_error_mem_fence", "OK case", id_cnt++);
    }

    {
        const amba_ik_flow_tables_list_t flow_tbl_list = {0};
        Rval = img_exe_inject_safety_crc_mismatch_error(&flow_tbl_list);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_inject_safety_crc_mismatch_error", "OK case", id_cnt++);
    }

    Rval = exe_set_frame_info(799, 0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_set_frame_info", "ctx-id fail case", id_cnt++);

    Rval = img_exe_execute_invalid_cr(0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_execute_invalid_cr", "test fail case", id_cnt++);

    Rval = img_exe_execute_safety_cr(0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_execute_safety_cr", "test fail case", id_cnt++);

    {
        amba_ik_filter_update_flags_t update_flag= {0};
        update_flag.iso.first_luma_processing_mode_updated = 1;
        Rval = filter_update_check_asf_fstshp(&update_flag,0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "filter_update_check_asf_fstshp", "test fail case", id_cnt++);

        Rval = filter_update_check_asf_fstshp(&update_flag,1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "filter_update_check_asf_fstshp", "test fail case", id_cnt++);
    }

    {
        amba_ik_filter_update_flags_t update_flag= {0};
        ability.pipe = AMBA_IK_PIPE_VIDEO;
        ability.video_pipe = AMBA_IK_VIDEO_LINEAR;
        Rval = filter_update_check_non_linear(&ability, &update_flag);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "filter_update_check_non_linear", "OK case", id_cnt++);
    }

    Rval = img_exe_execute_first_cr(799);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_execute_first_cr", "ctx-id fail case", id_cnt++);

    tmp = p_ctx->organization.initial_flag;
    p_ctx->organization.initial_flag = 0;
    Rval = img_exe_execute_first_cr(0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_execute_first_cr", "non-init ctx fail case", id_cnt++);
    p_ctx->organization.initial_flag = tmp;

    tmp = p_ctx->organization.attribute.cr_ring_number;
    p_ctx->organization.attribute.cr_ring_number = 0;
    Rval = img_exe_execute_first_cr(0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_execute_first_cr", "get flow_ctr fail case", id_cnt++);
    p_ctx->organization.attribute.cr_ring_number = tmp;

    Rval = img_exe_execute_update_cr(799);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_execute_update_cr", "ctx-id fail case", id_cnt++);

    tmp = p_ctx->organization.initial_flag;
    p_ctx->organization.initial_flag = 0;
    Rval = img_exe_execute_update_cr(0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_execute_update_cr", "non-init ctx fail case", id_cnt++);
    p_ctx->organization.initial_flag = tmp;

    tmp = p_ctx->organization.attribute.cr_ring_number;
    p_ctx->organization.attribute.cr_ring_number = 0;
    Rval = img_exe_execute_update_cr(0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_execute_update_cr", "get flow_ctr fail case", id_cnt++);
    p_ctx->organization.attribute.cr_ring_number = tmp;

    {
        ik_buffer_info_t *p_ik_buffer_info = NULL;

        img_arch_get_ik_working_buffer(&p_ik_buffer_info);
        tmp = p_ik_buffer_info->safety_state;
        p_ik_buffer_info->safety_state = IK_SAFETY_STATE_ERROR;
        Rval = img_exe_execute_update_cr(0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_execute_update_cr", "test already enter safety state fail case", id_cnt++);
        p_ik_buffer_info->safety_state = tmp;
    }
    Rval = uint32_min_ik(0, 1);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "uint32_min_ik", "uint32_min_ik(0, 1) = 0 OK case", id_cnt++);

#ifndef EARLYTEST_ENV
    {
        Rval = exe_ik_select_crc_check_list(33);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_select_crc_check_list", "test get ctx fail NG case", id_cnt++);

        Rval = exe_ik_calc_crc32_hdr(33, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32_hdr", "test get ctx fail NG case", id_cnt++);

        memset(&p_ctx->filters.crc_check_list, 0, sizeof(amba_ik_crc_check_list_t));
        Rval = exe_ik_calc_crc32_hdr(0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32_hdr", "test crc_check_list are 0 OK case", id_cnt++);

        Rval = exe_ik_calc_crc32_cfa(33, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32_cfa", "test get ctx fail NG case", id_cnt++);
        Rval = exe_ik_calc_crc32_cfa(0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32_cfa", "test crc_check_list are 0 OK case", id_cnt++);

        Rval = exe_ik_calc_crc32_rgb(33, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32_rgb", "test get ctx fail NG case", id_cnt++);
        Rval = exe_ik_calc_crc32_rgb(0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32_rgb", "test crc_check_list are 0 OK case", id_cnt++);

        Rval = exe_ik_calc_crc32_yuv(33, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32_yuv", "test get ctx fail NG case", id_cnt++);
        Rval = exe_ik_calc_crc32_yuv(0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32_yuv", "test crc_check_list are 0 OK case", id_cnt++);

        Rval = exe_ik_calc_crc32_sec3(33, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32_sec3", "test get ctx fail NG case", id_cnt++);
        Rval = exe_ik_calc_crc32_sec3(0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32_sec3", "test crc_check_list are 0 OK case", id_cnt++);

        Rval = exe_ik_calc_crc32_sec4(33, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32_sec4", "test get ctx fail NG case", id_cnt++);

        p_ctx->organization.attribute.ability.video_pipe = AMBA_IK_VIDEO_LINEAR_CE;
        p_ctx->filters.crc_check_list.cr_45 = 1;
        p_ctx->filters.crc_check_list.cr_46 = 1;
        p_ctx->filters.crc_check_list.cr_47 = 1;
        p_ctx->filters.crc_check_list.cr_48 = 1;
        p_ctx->filters.crc_check_list.cr_49 = 1;
        p_ctx->filters.crc_check_list.cr_50 = 1;
        p_ctx->filters.crc_check_list.cr_51 = 1;
        p_ctx->filters.crc_check_list.cr_52 = 1;
        Rval = exe_ik_calc_crc32_sec4(0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32_sec4", "test ability.pipe == HDR or CE OK case", id_cnt++);

        p_ctx->organization.attribute.ability.video_pipe = AMBA_IK_VIDEO_HDR_EXPO_2;
        Rval = exe_ik_calc_crc32_sec4(0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32_sec4", "test ability.pipe == HDR or CE OK case", id_cnt++);

        p_ctx->organization.attribute.ability.video_pipe = AMBA_IK_VIDEO_HDR_EXPO_3;
        memset(&p_ctx->filters.crc_check_list, 0, sizeof(amba_ik_crc_check_list_t));
        Rval = exe_ik_calc_crc32_sec4(0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32_sec4", "test crc_check_list are 0 OK case", id_cnt++);

        Rval = exe_ik_calc_crc32_sec18(33, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32_sec18", "test get ctx fail NG case", id_cnt++);
        Rval = exe_ik_calc_crc32_sec18(0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32_sec18", "test crc_check_list are 0 OK case", id_cnt++);

        Rval = exe_ik_calc_crc32(33, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32", "test get ctx fail NG case", id_cnt++);

        p_ctx->filters.input_param.stitching_info.enable = 1;
        Rval = exe_ik_calc_crc32(0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32", "test crc_check_list are 0 OK case", id_cnt++);

        p_ctx->filters.crc_check_list.flow_ctrl = 1;
        p_ctx->filters.input_param.safety_info.update_freq = 0;
        Rval = exe_ik_calc_crc32(0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "exe_ik_calc_crc32", "test if(p_ctx->filters.input_param.safety_info.update_freq == 0u) true OK case", id_cnt++);
    }
#endif

    {
        AMBA_IK_ABILITY_s Ability = {0};
        AMBA_IK_MODE_CFG_s Mode;

        Ability.Pipe = AMBA_IK_PIPE_VIDEO;
        Ability.VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_3;
        _Reset_Image_Kernel(&Ability);
        Mode.ContextId = 0;
        IK_Set_ImgFilters(&Mode);
        (void)img_ctx_get_context(0, &p_ctx);

        p_ik_buffer_info->safety_crc_enable = 0;
        Rval = img_exe_execute_first_cr(0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_execute_first_cr", "test if(p_ik_buffer_info->safety_crc_enable == 1u) false OK case", id_cnt++);
        Rval = img_exe_execute_update_cr(0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_execute_update_cr", "test if(p_ik_buffer_info->safety_crc_enable == 1u) false OK case", id_cnt++);

        _Reset_Image_Kernel(&Ability);
        Mode.ContextId = 0;
        IK_Set_ImgFilters(&Mode);
        (void)img_ctx_get_context(0, &p_ctx);

        p_ik_buffer_info->safety_crc_enable = 1;
        p_ctx->filters.input_param.safety_info.update_freq = 2;
        p_ctx->organization.active_cr_state.cr_running_number = 3;
        Rval = img_exe_execute_first_cr(0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_execute_first_cr", "test (p_ctx->filters.input_param.safety_info.update_freq == 1u) false and ((((uint32)p_ctx->organization.active_cr_state.cr_running_number - 1u) % p_ctx->filters.input_param.safety_info.update_freq) == 0u) true OK case", id_cnt++);
        Rval = img_exe_execute_update_cr(0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_execute_update_cr", "test (p_ctx->filters.input_param.safety_info.update_freq == 1u) false and ((((uint32)p_ctx->organization.active_cr_state.cr_running_number - 1u) % p_ctx->filters.input_param.safety_info.update_freq) == 0u) true OK case", id_cnt++);

        _Reset_Image_Kernel(&Ability);
        Mode.ContextId = 0;
        IK_Set_ImgFilters(&Mode);
        (void)img_ctx_get_context(0, &p_ctx);

        p_ik_buffer_info->safety_crc_enable = 1;
        p_ctx->filters.input_param.safety_info.update_freq = 2;
        p_ctx->organization.active_cr_state.cr_running_number = 4;
        Rval = img_exe_execute_first_cr(0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_execute_first_cr", "test (p_ctx->filters.input_param.safety_info.update_freq == 1u) false and ((((uint32)p_ctx->organization.active_cr_state.cr_running_number - 1u) % p_ctx->filters.input_param.safety_info.update_freq) == 0u) false OK case", id_cnt++);
        Rval = img_exe_execute_update_cr(0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgExecuterComponentIF.c", "img_exe_execute_update_cr", "test (p_ctx->filters.input_param.safety_info.update_freq == 1u) false and ((((uint32)p_ctx->organization.active_cr_state.cr_running_number - 1u) % p_ctx->filters.input_param.safety_info.update_freq) == 0u) false OK case", id_cnt++);
    }
}

INT32 ik_ctest_executer_main(void)
{
    UINT32 Rval = IK_OK;
    AMBA_IK_ABILITY_s Ability = {0};
    AMBA_IK_MODE_CFG_s Mode;

    memset(ik_working_buffer, 0, sizeof(ik_working_buffer));
    memset(WarpGrid, 0, sizeof(WarpGrid));
    memset(CawarpRedGrid, 0, sizeof(CawarpRedGrid));
    memset(CawarpBlueGrid, 0, sizeof(CawarpBlueGrid));
    memset(FPNMap, 0, sizeof(FPNMap));
    id_cnt = 0;
    pFile = fopen("data/ik_ctest_executer.txt", "w");

    // init IK
    _load_idsp_default_binary();
    Ability.Pipe = AMBA_IK_PIPE_VIDEO;
    Ability.VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_3;

    _Init_Image_Kernel(&Ability);
    _Init_Image_Kernel_Core();

    Mode.ContextId = 0;
    Rval |= AmbaIK_InitContext(&Mode, &Ability);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_InitContext", "OK case", id_cnt++);

    IK_TestCovr_ImgExecuterUnitWindowCalc();
    IK_TestCovr_ImgExecuterUnitConfig();
    IK_TestCovr_ImgExecuterComponentIF();

    fclose(pFile);

    fake_create_mutex(NULL);
    fake_destroy_mutex(NULL);
    fake_lock_mutex(NULL, 0);
    fake_unlock_mutex(NULL);
    fake_clean_cache(NULL, 0);
    fake_convert_to_physical_address(NULL);
    fake_convert_to_virtual_address(NULL);

    img_arch_deinit_architecture();
    iks_arch_deinit_architecture();

    return Rval;
}

