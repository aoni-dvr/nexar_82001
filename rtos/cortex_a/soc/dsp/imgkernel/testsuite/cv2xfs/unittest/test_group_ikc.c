#include "test_ik_global.h"
#include "AmbaDSP_IkcRefUtility.h"

static UINT8 ik_working_buffer[5<<20];
static void *pBinDataAddr;

FILE *pFile;
uint32 id_cnt;

static INT32 fake_create_mutex(AMBA_KAL_MUTEX_t *mutex)
{
    return 0;
}

static INT32 fake_destroy_mutex(AMBA_KAL_MUTEX_t *mutex)
{
    return 0;
}

static INT32 fake_lock_mutex(AMBA_KAL_MUTEX_t *mutex, uint32 timeout)
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
    uint32 DefSize;

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
    uint32 rval = IK_OK;
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

static void IK_TestCovr_IkcRefUtility(void)
{
    {
        extern int32 getDirectionalCenterWeightValue(int32 blend_strength);

        getDirectionalCenterWeightValue(8);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_IkcRefUtility.c", "getDirectionalCenterWeightValue", "test if (blend_strength < 8) false OK case", id_cnt++);
    }

    {
        ik_cfa_noise_filter_t cnf_auto = {0};
        cfa_noise_filter_t cnf_manual = {0};
        extern void cfa_noise_filter_auto_to_manual(const ik_cfa_noise_filter_t *cnf_auto, cfa_noise_filter_t *cnf_manual);

        cnf_auto.original_blend_str[0] = 256;
        cfa_noise_filter_auto_to_manual(&cnf_auto, &cnf_manual);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_IkcRefUtility.c", "get_center_shift_value", "test if (temp > 31) true OK case", id_cnt++);

        cnf_auto.extent_regular[0] = 256;
        cfa_noise_filter_auto_to_manual(&cnf_auto, &cnf_manual);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_IkcRefUtility.c", "get_cutoff_value", "test (fine_target < 128) false OK case", id_cnt++);

        cnf_auto.selectivity_regular = 500;
        cfa_noise_filter_auto_to_manual(&cnf_auto, &cnf_manual);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_IkcRefUtility.c", "get_ring_shift_value", "test if (tmp_index > 4) true OK case", id_cnt++);

        cnf_auto.lev_mul[0] = 65535;
        cnf_auto.lev_shift[0] = 10;
        cfa_noise_filter_auto_to_manual(&cnf_auto, &cnf_manual);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_IkcRefUtility.c", "comp_lev_dep_mul_shift", "test if( *p_adjusted_shift > 10 ) true OK case", id_cnt++);

        cnf_auto.original_blend_str[0] = 10;
        cnf_auto.selectivity_regular = 1000;
        cfa_noise_filter_auto_to_manual(&cnf_auto, &cnf_manual);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_IkcRefUtility.c", "cfa_noise_filter_auto_to_manual", "test if (cnf_manual->shift_center_red < cnf_manual->shift_fine_ring1) true OK case", id_cnt++);
    }

    {
        ik_rgb_ir_t rgb_ir = {0};
        uint16 rgbir_base_val;
        uint16 rgbir_high_val;
        uint8 shift_val;

        extern void set_ircorrect_mul_shift(const ik_rgb_ir_t *p_rgb_ir, uint16 *rgbir_base_val, uint16 *rgbir_high_val, uint8 *shift_val);

        rgb_ir.mul_base_val = 134217728;
        rgb_ir.mul_high_val = 134217728;
        set_ircorrect_mul_shift(&rgb_ir, &rgbir_base_val, &rgbir_high_val, &shift_val);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_IkcRefUtility.c", "set_ircorrect_mul_shift", "test (b1 > 511.0) false OK case", id_cnt++);
    }

    {
        int32 curve[10];

        extern void make_curve(int32 curve[], int32 alpha, int32 T0, int32 T1, int32 array_length, int32 max_val, int32 div2);

        make_curve(curve, 0, 1, 0, 2, 0, 0);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_IkcRefUtility.c", "make_curve", "test if ((array_length-1) == misra_int32_T0) true OK case", id_cnt++);
    }

    {
        uint8 shift;
        uint8 mantissa;
        extern void find_shift_and_8bit_mantissa( uint32 diff, uint8 *shift, uint8 *mantissa);

        find_shift_and_8bit_mantissa(1, &shift, &mantissa);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_IkcRefUtility.c", "find_shift_and_8bit_mantissa", "test s > 7 false OK case", id_cnt++);
    }
}

static void IK_TestCovr_IkcUnitAaa(void)
{
    uint32 Rval = IK_OK;
    uintptr addr = 0;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_filter_t *p_filters = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    (void)img_ctx_get_flow_tbl_list(0, 0, &addr);
    (void)memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
    (void)img_ctx_get_context(0, &p_ctx);
    p_filters = &p_ctx->filters;

    {
        ikc_in_aaa_t in_aaa;
        ikc_out_aaa_t out_aaa;
        in_aaa.p_sensor_info = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.sensor_info;//determine RGB-IR.
        in_aaa.p_rgb_ir = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.rgb_ir;
        in_aaa.p_aaa_stat_info = &p_filters->input_param.aaa_stat_info;
        in_aaa.p_aaa_pg_af_stat_info = &p_filters->input_param.aaa_pg_stat_info;
        in_aaa.p_af_stat_ex_info= &p_filters->input_param.af_stat_ex_info;
        in_aaa.p_pg_af_stat_ex_info = &p_filters->input_param.pg_af_stat_ex_info;
        in_aaa.p_hist_info = &p_filters->input_param.hist_info;
        in_aaa.p_hist_info_pg = &p_filters->input_param.hist_info_pg;
        in_aaa.p_logical_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.logical_dmy_win_geo;
        in_aaa.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
        in_aaa.p_main = &p_filters->input_param.window_size_info.main_win;
        in_aaa.p_stitching_info = &p_filters->input_param.stitching_info;
        out_aaa.p_cr_9 = p_flow_tbl_list->p_CR_buf_9;
        out_aaa.cr_9_size = CR_SIZE_9;
        out_aaa.p_cr_12 = p_flow_tbl_list->p_CR_buf_12;
        out_aaa.cr_12_size = CR_SIZE_12;
        out_aaa.p_cr_21 = p_flow_tbl_list->p_CR_buf_21;
        out_aaa.cr_21_size = CR_SIZE_21;
        out_aaa.p_cr_28 = p_flow_tbl_list->p_CR_buf_28;
        out_aaa.cr_28_size = CR_SIZE_28;
        out_aaa.p_stitching_aaa = p_flow_tbl_list->p_aaa;

        Rval = ikc_aaa(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitAaa.c", "ikc_aaa", "null-ptr fail case", id_cnt++);

        in_aaa.is_yuv_mode = 0UL;

        in_aaa.p_aaa_pg_af_stat_info->af_tile_num_col = 24U;
        in_aaa.p_aaa_pg_af_stat_info->af_tile_num_row = 16U;
        in_aaa.p_aaa_pg_af_stat_info->af_tile_col_start = 0;
        in_aaa.p_aaa_pg_af_stat_info->af_tile_row_start = 0;
        in_aaa.p_aaa_pg_af_stat_info->af_tile_width = 170U;
        in_aaa.p_aaa_pg_af_stat_info->af_tile_height = 256U;
        in_aaa.p_aaa_pg_af_stat_info->af_tile_active_width = 170U;
        in_aaa.p_aaa_pg_af_stat_info->af_tile_active_height = 256U;

        in_aaa.p_logical_dmy_win_geo->start_x = 0;
        in_aaa.p_logical_dmy_win_geo->start_y = 0;
        in_aaa.p_logical_dmy_win_geo->width = 1920U;
        in_aaa.p_logical_dmy_win_geo->height = 1080U;

        in_aaa.p_cfa_win_dim->width = 1920U;
        in_aaa.p_cfa_win_dim->height = 1080U;

        in_aaa.p_main->width = 1920U;
        in_aaa.p_main->height = 1080U;

        in_aaa.p_stitching_info->enable = 0U;

        Rval = ikc_aaa(&in_aaa, &out_aaa);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitAaa.c", "ikc_aaa", "OK case", id_cnt++);

        in_aaa.p_stitching_info->enable = 1U;
        in_aaa.p_stitching_info->tile_num_x = 2U;
        in_aaa.p_stitching_info->tile_num_y = 2U;

        in_aaa.is_yuv_mode = 1U;

        in_aaa.p_logical_dmy_win_geo->width = 254U;
        in_aaa.p_logical_dmy_win_geo->height = 254U;

        in_aaa.p_cfa_win_dim->width = 254U;
        in_aaa.p_cfa_win_dim->height = 254U;

        in_aaa.p_main->width = 254U;
        in_aaa.p_main->height = 254U;

        Rval = ikc_aaa(&in_aaa, &out_aaa);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitAaa.c", "ikc_aaa", "OK case", id_cnt++);
    }

}

static void IK_TestCovr_IkcUnitCaWarp(void)
{
    uint32 Rval = IK_OK;
    uintptr addr = 0;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_filter_t *p_filters = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    idsp_ik_flow_ctrl_t *p_flow = NULL;
    (void)img_ctx_get_flow_tbl_list(0, 0, &addr);
    (void)memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
    (void)img_ctx_get_context(0, &p_ctx);
    p_filters = &p_ctx->filters;
    (void)img_ctx_get_flow_control(0, 0, &addr);
    (void)memcpy(&p_flow, &addr, sizeof(void *));

    {
        ikc_in_cawarp_t in_ca = {0};
        ikc_out_cawarp_t out_ca = {0};
        ik_grid_point_t cawarp_red[32*48] = {0};
        ik_grid_point_t cawarp_blue[32*48] = {0};

        in_ca.is_group_cmd = 0u;
        in_ca.cawarp_enable = (uint32)p_filters->input_param.cawarp_enable;
        in_ca.flip_mode = p_filters->input_param.flip_mode;
        in_ca.p_stitching_info = &p_filters->input_param.stitching_info;
        in_ca.p_window_size_info = &p_filters->input_param.window_size_info;
        in_ca.p_result_win = &p_filters->input_param.ctx_buf.result_win;
        in_ca.p_calib_ca_warp_info = &p_filters->input_param.calib_ca_warp_info;
        out_ca.p_cr_16 = p_flow_tbl_list->p_CR_buf_16;
        out_ca.cr_16_size = CR_SIZE_16;
        out_ca.p_cr_17 = p_flow_tbl_list->p_CR_buf_17;
        out_ca.cr_17_size = CR_SIZE_17;
        out_ca.p_cr_18 = p_flow_tbl_list->p_CR_buf_18;
        out_ca.cr_18_size = CR_SIZE_18;
        out_ca.p_cr_19 = p_flow_tbl_list->p_CR_buf_19;
        out_ca.cr_19_size = CR_SIZE_19;
        out_ca.p_cr_20 = p_flow_tbl_list->p_CR_buf_20;
        out_ca.cr_20_size = CR_SIZE_20;
        out_ca.p_calib = &p_flow->calib;
        out_ca.p_cawarp_hor_table_addr_red = p_flow_tbl_list->p_ca_warp_hor_red;
        out_ca.h_red_size = CAWARP_VIDEO_MAXSIZE;
        out_ca.p_cawarp_vertical_table_addr_red = p_flow_tbl_list->p_ca_warp_ver_red;
        out_ca.v_red_size = CAWARP_VIDEO_MAXSIZE;
        out_ca.p_cawarp_hor_table_addr_blue = p_flow_tbl_list->p_ca_warp_hor_blue;
        out_ca.h_blue_size = CAWARP_VIDEO_MAXSIZE;
        out_ca.p_cawarp_vertical_table_addr_blue = p_flow_tbl_list->p_ca_warp_ver_blue;
        out_ca.v_blue_size = CAWARP_VIDEO_MAXSIZE;

        Rval = ikc_cawarp(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCaWarp.c", "ikc_cawarp", "null-ptr fail case", id_cnt++);

        in_ca.cawarp_enable = 1u;
        in_ca.p_calib_ca_warp_info->p_cawarp_red = cawarp_red;
        in_ca.p_calib_ca_warp_info->p_cawarp_blue = cawarp_blue;
        in_ca.p_calib_ca_warp_info->hor_grid_num = 32;
        in_ca.p_calib_ca_warp_info->ver_grid_num = 48;
        in_ca.p_calib_ca_warp_info->tile_width_exp = 7;
        in_ca.p_calib_ca_warp_info->tile_height_exp = 7;
        in_ca.p_calib_ca_warp_info->vin_sensor_geo.start_x = 0;
        in_ca.p_calib_ca_warp_info->vin_sensor_geo.start_y = 0;
        in_ca.p_calib_ca_warp_info->vin_sensor_geo.width = 3840;
        in_ca.p_calib_ca_warp_info->vin_sensor_geo.height = 2160;
        in_ca.p_calib_ca_warp_info->vin_sensor_geo.h_sub_sample.factor_num = 1;
        in_ca.p_calib_ca_warp_info->vin_sensor_geo.h_sub_sample.factor_den = 1;
        in_ca.p_calib_ca_warp_info->vin_sensor_geo.v_sub_sample.factor_num = 1;
        in_ca.p_calib_ca_warp_info->vin_sensor_geo.v_sub_sample.factor_den = 1;

        in_ca.p_stitching_info->enable = 1;
        in_ca.p_stitching_info->tile_num_x = 2;
        in_ca.p_stitching_info->tile_num_y = 1;

        in_ca.p_window_size_info->vin_sensor.start_x = 0;
        in_ca.p_window_size_info->vin_sensor.start_y = 0;
        in_ca.p_window_size_info->vin_sensor.width = 3840;
        in_ca.p_window_size_info->vin_sensor.height = 2160;
        in_ca.p_window_size_info->vin_sensor.h_sub_sample.factor_num = 1;
        in_ca.p_window_size_info->vin_sensor.h_sub_sample.factor_den = 1;
        in_ca.p_window_size_info->vin_sensor.v_sub_sample.factor_num = 1;
        in_ca.p_window_size_info->vin_sensor.v_sub_sample.factor_den = 1;
        in_ca.p_window_size_info->main_win.width = 3840;
        in_ca.p_window_size_info->main_win.height = 2160;

        in_ca.p_result_win->act_win_crop.left_top_x = 0;
        in_ca.p_result_win->act_win_crop.right_bot_x = 3840<<16;
        in_ca.p_result_win->act_win_crop.left_top_y = 0;
        in_ca.p_result_win->act_win_crop.right_bot_y = 2160<<16;
        in_ca.p_result_win->dmy_win_geo.start_x = 0;
        in_ca.p_result_win->dmy_win_geo.start_y = 0;
        in_ca.p_result_win->dmy_win_geo.width = 3840;
        in_ca.p_result_win->dmy_win_geo.height = 2160;
        in_ca.p_result_win->logical_dmy_win_geo.start_x = 0;
        in_ca.p_result_win->logical_dmy_win_geo.start_y = 0;
        in_ca.p_result_win->logical_dmy_win_geo.width = 3840;
        in_ca.p_result_win->logical_dmy_win_geo.height = 2160;
        in_ca.p_result_win->cfa_win_dim.width = 3840;
        in_ca.p_result_win->cfa_win_dim.height = 2160;

        in_ca.flip_mode = (IK_FLIP_RAW_H | IK_FLIP_RAW_V);

        Rval = ikc_cawarp(&in_ca, &out_ca);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCaWarp.c", "ikc_cawarp", "OK case", id_cnt++);

        in_ca.p_calib_ca_warp_info->vin_sensor_geo.start_x = 0;
        in_ca.p_calib_ca_warp_info->vin_sensor_geo.start_y = 0;
        in_ca.p_calib_ca_warp_info->vin_sensor_geo.width = 1920;
        in_ca.p_calib_ca_warp_info->vin_sensor_geo.height = 1080;
        Rval = ikc_cawarp(&in_ca, &out_ca);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCaWarp.c", "ikc_cawarp", "Invalid geometry fail case", id_cnt++);

    }
    {
        ikc_in_cawarp_internal_t in_ca = {0};
        ikc_out_cawarp_t out_ca = {0};

        in_ca.p_stitching_info = &p_filters->input_param.stitching_info;
        in_ca.p_ca_warp_internal = &p_filters->input_param.ca_warp_internal;
        out_ca.p_cr_16 = p_flow_tbl_list->p_CR_buf_16;
        out_ca.cr_16_size = CR_SIZE_16;
        out_ca.p_cr_17 = p_flow_tbl_list->p_CR_buf_17;
        out_ca.cr_17_size = CR_SIZE_17;
        out_ca.p_cr_18 = p_flow_tbl_list->p_CR_buf_18;
        out_ca.cr_18_size = CR_SIZE_18;
        out_ca.p_cr_19 = p_flow_tbl_list->p_CR_buf_19;
        out_ca.cr_19_size = CR_SIZE_19;
        out_ca.p_cr_20 = p_flow_tbl_list->p_CR_buf_20;
        out_ca.cr_20_size = CR_SIZE_20;
        out_ca.p_calib = &p_flow->calib;
        out_ca.p_cawarp_hor_table_addr_red = p_flow_tbl_list->p_ca_warp_hor_red;
        out_ca.h_red_size = CAWARP_VIDEO_MAXSIZE;
        out_ca.p_cawarp_vertical_table_addr_red = p_flow_tbl_list->p_ca_warp_ver_red;
        out_ca.v_red_size = CAWARP_VIDEO_MAXSIZE;
        out_ca.p_cawarp_hor_table_addr_blue = p_flow_tbl_list->p_ca_warp_hor_blue;
        out_ca.h_blue_size = CAWARP_VIDEO_MAXSIZE;
        out_ca.p_cawarp_vertical_table_addr_blue = p_flow_tbl_list->p_ca_warp_ver_blue;
        out_ca.v_blue_size = CAWARP_VIDEO_MAXSIZE;

        in_ca.p_stitching_info->enable = 1;
        in_ca.p_stitching_info->tile_num_x = 2;
        in_ca.p_stitching_info->tile_num_y = 1;

        in_ca.p_ca_warp_internal->horz_warp_enable = 0;
        in_ca.p_ca_warp_internal->vert_warp_enable = 0;
        in_ca.p_ca_warp_internal->horz_pass_grid_array_width = 32;
        in_ca.p_ca_warp_internal->horz_pass_grid_array_height = 48;
        in_ca.p_ca_warp_internal->horz_pass_horz_grid_spacing_exponent = 6;
        in_ca.p_ca_warp_internal->horz_pass_vert_grid_spacing_exponent = 6;
        in_ca.p_ca_warp_internal->vert_pass_grid_array_width = 32;
        in_ca.p_ca_warp_internal->vert_pass_grid_array_height = 48;
        in_ca.p_ca_warp_internal->vert_pass_horz_grid_spacing_exponent = 6;
        in_ca.p_ca_warp_internal->vert_pass_vert_grid_spacing_exponent = 6;

        Rval = ikc_cawarp_internal(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCaWarp.c", "ikc_cawarp_internal", "null-ptr fail case", id_cnt++);

        Rval = ikc_cawarp_internal(&in_ca, &out_ca);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCaWarp.c", "ikc_cawarp_internal", "OK case", id_cnt++);

        in_ca.p_ca_warp_internal->horz_pass_grid_array_width = 33;
        in_ca.p_ca_warp_internal->horz_pass_grid_array_height = 49;
        in_ca.p_ca_warp_internal->vert_pass_grid_array_width = 33;
        in_ca.p_ca_warp_internal->vert_pass_grid_array_height = 49;

        Rval = ikc_cawarp_internal(&in_ca, &out_ca);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCaWarp.c", "ikc_cawarp_internal", "Invalid grid number fail case", id_cnt++);

    }

}

static void IK_TestCovr_IkcUnitWarp(void)
{
    uint32 Rval = IK_OK;
    uintptr addr = 0;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_filter_t *p_filters = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    idsp_ik_flow_ctrl_t *p_flow = NULL;
    (void)img_ctx_get_flow_tbl_list(0, 0, &addr);
    (void)memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
    (void)img_ctx_get_context(0, &p_ctx);
    p_filters = &p_ctx->filters;
    (void)img_ctx_get_flow_control(0, 0, &addr);
    (void)memcpy(&p_flow, &addr, sizeof(void *));

    {
        ikc_in_warp_t in_warp;
        ikc_out_warp_t out_warp;
        ik_grid_point_t warp_tbl[32*48] = {0};

        in_warp.is_group_cmd = 0u;
        in_warp.warp_enable = (uint32)p_filters->input_param.warp_enable;
        in_warp.flip_mode = p_filters->input_param.flip_mode;
        in_warp.p_window_size_info = &p_filters->input_param.window_size_info;
        in_warp.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
        in_warp.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
        in_warp.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
        in_warp.p_dmy_range = &p_filters->input_param.dmy_range;
        in_warp.p_stitching_info = &p_filters->input_param.stitching_info;
        in_warp.p_calib_warp_info = &p_filters->input_param.calib_warp_info;
        in_warp.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
        in_warp.force_vscale_sec_n = 0;

        out_warp.p_cr_33 = p_flow_tbl_list->p_CR_buf_33;
        out_warp.cr_33_size = CR_SIZE_33;
        out_warp.p_cr_34 = p_flow_tbl_list->p_CR_buf_34;
        out_warp.cr_34_size = CR_SIZE_34;
        out_warp.p_cr_35 = p_flow_tbl_list->p_CR_buf_35;
        out_warp.cr_35_size = CR_SIZE_35;
        out_warp.p_cr_42 = p_flow_tbl_list->p_CR_buf_42;
        out_warp.cr_42_size = CR_SIZE_42;
        out_warp.p_cr_43 = p_flow_tbl_list->p_CR_buf_43;
        out_warp.cr_43_size = CR_SIZE_43;
        out_warp.p_cr_44 = p_flow_tbl_list->p_CR_buf_44;
        out_warp.cr_44_size = CR_SIZE_44;
        out_warp.p_flow_info = &p_flow->flow_info;
        out_warp.p_window = &p_flow->window;
        out_warp.p_phase = &p_flow->phase;
        out_warp.p_calib = &p_flow->calib;
        out_warp.p_stitch = &p_flow->stitch;
        out_warp.p_warp_horizontal_table_address = p_flow_tbl_list->p_warp_hor;
        out_warp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
        out_warp.p_warp_vertical_table_address = p_flow_tbl_list->p_warp_ver;
        out_warp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

        Rval = ikc_warp(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp", "null-ptr fail case", id_cnt++);

        in_warp.warp_enable = 1u;
        in_warp.flip_mode = (IK_FLIP_RAW_H | IK_FLIP_RAW_V);

        in_warp.p_window_size_info->vin_sensor.start_x = 0;
        in_warp.p_window_size_info->vin_sensor.start_y = 0;
        in_warp.p_window_size_info->vin_sensor.width = 3840;
        in_warp.p_window_size_info->vin_sensor.height = 2160;
        in_warp.p_window_size_info->vin_sensor.h_sub_sample.factor_num = 1;
        in_warp.p_window_size_info->vin_sensor.h_sub_sample.factor_den = 1;
        in_warp.p_window_size_info->vin_sensor.v_sub_sample.factor_num = 1;
        in_warp.p_window_size_info->vin_sensor.v_sub_sample.factor_den = 1;
        in_warp.p_window_size_info->main_win.width = 3840;
        in_warp.p_window_size_info->main_win.height = 2160;

        in_warp.p_act_win_crop->left_top_x = 0;
        in_warp.p_act_win_crop->right_bot_x = 3840<<16;
        in_warp.p_act_win_crop->left_top_y = 0;
        in_warp.p_act_win_crop->right_bot_y = 2160<<16;
        in_warp.p_dmy_win_geo->start_x = 0;
        in_warp.p_dmy_win_geo->start_y = 0;
        in_warp.p_dmy_win_geo->width = 3840;
        in_warp.p_dmy_win_geo->height = 2160;
        in_warp.p_cfa_win_dim->width = 3840;
        in_warp.p_cfa_win_dim->height = 2160;

        in_warp.p_dmy_range->enable = 0;

        in_warp.p_stitching_info->enable = 1;
        in_warp.p_stitching_info->tile_num_x = 2;
        in_warp.p_stitching_info->tile_num_y = 1;

        in_warp.p_calib_warp_info->pwarp = warp_tbl;
        in_warp.p_calib_warp_info->hor_grid_num = 32;
        in_warp.p_calib_warp_info->ver_grid_num = 48;
        in_warp.p_calib_warp_info->tile_width_exp = 7;
        in_warp.p_calib_warp_info->tile_height_exp = 7;
        in_warp.p_calib_warp_info->vin_sensor_geo.start_x = 0;
        in_warp.p_calib_warp_info->vin_sensor_geo.start_y = 0;
        in_warp.p_calib_warp_info->vin_sensor_geo.width = 3840;
        in_warp.p_calib_warp_info->vin_sensor_geo.height = 2160;
        in_warp.p_calib_warp_info->vin_sensor_geo.h_sub_sample.factor_num = 1;
        in_warp.p_calib_warp_info->vin_sensor_geo.h_sub_sample.factor_den = 1;
        in_warp.p_calib_warp_info->vin_sensor_geo.v_sub_sample.factor_num = 1;
        in_warp.p_calib_warp_info->vin_sensor_geo.v_sub_sample.factor_den = 1;

        in_warp.dram_efficiency = 0;

        Rval = ikc_warp(&in_warp, &out_warp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp", "OK case", id_cnt++);

        in_warp.p_dmy_range->enable = 1;
        in_warp.p_dmy_range->top = 1000;
        in_warp.warp_enable = 0u;
        in_warp.p_act_win_crop->left_top_y = 100;
        in_warp.p_act_win_crop->right_bot_y = 2060<<16;
        in_warp.p_stitching_info->tile_num_x = 3;
        Rval = ikc_warp(&in_warp, &out_warp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp", "OK case", id_cnt++);
        in_warp.p_dmy_range->enable = 0;
        in_warp.p_dmy_range->top = 0;
        in_warp.warp_enable = 1u;
        in_warp.p_act_win_crop->left_top_y = 0;
        in_warp.p_act_win_crop->right_bot_y = 2160<<16;
        in_warp.p_stitching_info->tile_num_x = 2;

        out_warp.h_size = 0;
        out_warp.v_size = 0;
        Rval = ikc_warp(&in_warp, &out_warp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp", "Invalid buffer size fail case", id_cnt++);
        out_warp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
        out_warp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

        in_warp.p_calib_warp_info->vin_sensor_geo.start_x = 100;
        in_warp.p_calib_warp_info->vin_sensor_geo.width = 1920;
        in_warp.p_calib_warp_info->vin_sensor_geo.height = 1080;
        Rval = ikc_warp(&in_warp, &out_warp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp", "Invalid warp geometry fail case", id_cnt++);
        in_warp.p_calib_warp_info->vin_sensor_geo.width = 3840;
        in_warp.p_calib_warp_info->vin_sensor_geo.height = 2160;

        in_warp.p_act_win_crop->left_top_x = 4000<<16;
        in_warp.p_act_win_crop->right_bot_x = 4000<<16;
        in_warp.p_act_win_crop->left_top_y = 3000<<16;
        in_warp.p_act_win_crop->right_bot_y = 3000<<16;
        in_warp.p_dmy_win_geo->start_x = 1000;
        in_warp.p_dmy_win_geo->start_y = 1000;
        in_warp.p_dmy_win_geo->width = 3840;
        in_warp.p_dmy_win_geo->height = 2160;
        in_warp.p_cfa_win_dim->width = 960;
        in_warp.p_cfa_win_dim->height = 540;
        in_warp.p_window_size_info->main_win.width = 480;
        in_warp.p_window_size_info->main_win.height = 270;
        Rval = ikc_warp(&in_warp, &out_warp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp", "Invalid window geometry fail case", id_cnt++);

    }

    {
        ikc_in_warp_radius_t in_warp_radius;
        ikc_out_warp_radius_t out_warp_radius;
        in_warp_radius.chroma_radius = p_filters->input_param.chroma_filter.radius;
        in_warp_radius.is_y2y_420input = 0;
        in_warp_radius.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
        in_warp_radius.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
        in_warp_radius.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;

        out_warp_radius.p_cr_35 = p_flow_tbl_list->p_CR_buf_35;
        out_warp_radius.cr_35_size = CR_SIZE_35;
        out_warp_radius.p_cr_42 = p_flow_tbl_list->p_CR_buf_42;
        out_warp_radius.cr_42_size = CR_SIZE_42;
        out_warp_radius.p_window = &p_flow->window;
        out_warp_radius.p_calib = &p_flow->calib;

        Rval = ikc_warp_radius(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp_radius", "null-ptr fail case", id_cnt++);

        in_warp_radius.luma_wait_lines = 0;
        in_warp_radius.luma_dma_size = 0;
        in_warp_radius.chroma_radius = 32;
        in_warp_radius.dram_efficiency = 2;
        Rval = ikc_warp_radius(&in_warp_radius, &out_warp_radius);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp_radius", "OK case", id_cnt++);

        in_warp_radius.chroma_radius = 64;
        Rval = ikc_warp_radius(&in_warp_radius, &out_warp_radius);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp_radius", "OK case", id_cnt++);

        in_warp_radius.chroma_radius = 128;
        Rval = ikc_warp_radius(&in_warp_radius, &out_warp_radius);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp_radius", "OK case", id_cnt++);

        in_warp_radius.dram_efficiency = 1;
        Rval = ikc_warp_radius(&in_warp_radius, &out_warp_radius);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp_radius", "OK case", id_cnt++);

        memset(out_warp_radius.p_window, 0x0, sizeof(idsp_ik_window_info_t));//set HA = 0, to hit condition.
        Rval = ikc_warp_radius(&in_warp_radius, &out_warp_radius);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp_radius", "OK case", id_cnt++);

        memset(out_warp_radius.p_window, 0x0, sizeof(idsp_ik_window_info_t));//set HA = 0, to hit condition.
        in_warp_radius.is_y2y_420input = 1;
        Rval = ikc_warp_radius(&in_warp_radius, &out_warp_radius);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp_radius", "OK case", id_cnt++);

    }

    {
        ikc_in_warp_internal_t in_warp_internal;
        ikc_out_warp_t out_warp;
        static int16 warp_horztable[1536] = {0};
        static int16 warp_verttable[1536] = {0};

        in_warp_internal.flip_mode = p_filters->input_param.flip_mode;
        in_warp_internal.p_window_size_info = &p_filters->input_param.window_size_info;
        in_warp_internal.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
        in_warp_internal.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
        in_warp_internal.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
        in_warp_internal.p_dmy_range = &p_filters->input_param.dmy_range;
        in_warp_internal.p_stitching_info = &p_filters->input_param.stitching_info;
        in_warp_internal.p_warp_internal = &p_filters->input_param.warp_internal;
        in_warp_internal.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;

        out_warp.p_cr_33 = p_flow_tbl_list->p_CR_buf_33;
        out_warp.cr_33_size = CR_SIZE_33;
        out_warp.p_cr_34 = p_flow_tbl_list->p_CR_buf_34;
        out_warp.cr_34_size = CR_SIZE_34;
        out_warp.p_cr_35 = p_flow_tbl_list->p_CR_buf_35;
        out_warp.cr_35_size = CR_SIZE_35;
        out_warp.p_cr_42 = p_flow_tbl_list->p_CR_buf_42;
        out_warp.cr_42_size = CR_SIZE_42;
        out_warp.p_cr_43 = p_flow_tbl_list->p_CR_buf_43;
        out_warp.cr_43_size = CR_SIZE_43;
        out_warp.p_cr_44 = p_flow_tbl_list->p_CR_buf_44;
        out_warp.cr_44_size = CR_SIZE_44;
        out_warp.p_flow_info = &p_flow->flow_info;
        out_warp.p_window = &p_flow->window;
        out_warp.p_phase = &p_flow->phase;
        out_warp.p_calib = &p_flow->calib;
        out_warp.p_stitch = &p_flow->stitch;
        out_warp.p_warp_horizontal_table_address = p_flow_tbl_list->p_warp_hor;
        out_warp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
        out_warp.p_warp_vertical_table_address = p_flow_tbl_list->p_warp_ver;
        out_warp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

        Rval = ikc_warp_internal(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp_internal", "null-ptr fail case", id_cnt++);

        in_warp_internal.flip_mode = 0;

        in_warp_internal.p_window_size_info->vin_sensor.start_x = 0;
        in_warp_internal.p_window_size_info->vin_sensor.start_y = 0;
        in_warp_internal.p_window_size_info->vin_sensor.width = 3840;
        in_warp_internal.p_window_size_info->vin_sensor.height = 2160;
        in_warp_internal.p_window_size_info->vin_sensor.h_sub_sample.factor_num = 1;
        in_warp_internal.p_window_size_info->vin_sensor.h_sub_sample.factor_den = 1;
        in_warp_internal.p_window_size_info->vin_sensor.v_sub_sample.factor_num = 1;
        in_warp_internal.p_window_size_info->vin_sensor.v_sub_sample.factor_den = 1;
        in_warp_internal.p_window_size_info->main_win.width = 3840;
        in_warp_internal.p_window_size_info->main_win.height = 2160;

        in_warp_internal.p_act_win_crop->left_top_x = 0;
        in_warp_internal.p_act_win_crop->right_bot_x = 3840<<16;
        in_warp_internal.p_act_win_crop->left_top_y = 0;
        in_warp_internal.p_act_win_crop->right_bot_y = 2160<<16;
        in_warp_internal.p_dmy_win_geo->start_x = 0;
        in_warp_internal.p_dmy_win_geo->start_y = 0;
        in_warp_internal.p_dmy_win_geo->width = 3840;
        in_warp_internal.p_dmy_win_geo->height = 2160;
        in_warp_internal.p_cfa_win_dim->width = 3840;
        in_warp_internal.p_cfa_win_dim->height = 2160;

        in_warp_internal.p_dmy_range->enable = 0;

        in_warp_internal.p_stitching_info->enable = 1;
        in_warp_internal.p_stitching_info->tile_num_x = 2;
        in_warp_internal.p_stitching_info->tile_num_y = 1;

        in_warp_internal.dram_efficiency = 2;

        in_warp_internal.p_warp_internal->enable = 1;
        in_warp_internal.p_warp_internal->grid_array_width = 32;
        in_warp_internal.p_warp_internal->grid_array_height = 48;
        in_warp_internal.p_warp_internal->horz_grid_spacing_exponent = 7;
        in_warp_internal.p_warp_internal->vert_grid_spacing_exponent = 6;
        in_warp_internal.p_warp_internal->vert_warp_grid_array_width = 32;
        in_warp_internal.p_warp_internal->vert_warp_grid_array_height = 48;
        in_warp_internal.p_warp_internal->vert_warp_horzgrid_spacing_exponent = 7;
        in_warp_internal.p_warp_internal->vert_warp_vert_grid_spacing_exponent = 6;
        in_warp_internal.p_warp_internal->pwarp_horizontal_table = warp_horztable;
        in_warp_internal.p_warp_internal->pwarp_vertical_table = warp_verttable;

        Rval = ikc_warp_internal(&in_warp_internal, &out_warp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp_internal", "OK case", id_cnt++);

        out_warp.h_size = 0;
        out_warp.v_size = 0;
        Rval = ikc_warp_internal(&in_warp_internal, &out_warp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp_internal", "Invalid buffer size fail case", id_cnt++);
        out_warp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
        out_warp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

        in_warp_internal.p_act_win_crop->left_top_x = 4000<<16;
        in_warp_internal.p_act_win_crop->right_bot_x = 4000<<16;
        in_warp_internal.p_act_win_crop->left_top_y = 8640<<16;
        in_warp_internal.p_act_win_crop->right_bot_y = 8640<<16;
        in_warp_internal.p_dmy_win_geo->start_x = 1000;
        in_warp_internal.p_dmy_win_geo->start_y = 1000;
        in_warp_internal.p_dmy_win_geo->width = 3840;
        in_warp_internal.p_dmy_win_geo->height = 2160;
        in_warp_internal.p_cfa_win_dim->width = 960;
        in_warp_internal.p_cfa_win_dim->height = 540;
        in_warp_internal.p_window_size_info->main_win.width = 480;
        in_warp_internal.p_window_size_info->main_win.height = 270;
        Rval = ikc_warp_internal(&in_warp_internal, &out_warp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp_internal", "Invalid window geometry fail case", id_cnt++);

    }

    Rval = ikc_warp_radius_internal(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitWarp.c", "ikc_warp_radius_internal", "null-ptr fail case", id_cnt++);

}

static void IK_TestCovr_IkcUnitResampler(void)
{
    uint32 Rval = IK_OK;
    uintptr addr = 0;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_filter_t *p_filters = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    idsp_ik_flow_ctrl_t *p_flow = NULL;
    (void)img_ctx_get_flow_tbl_list(0, 0, &addr);
    (void)memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
    (void)img_ctx_get_context(0, &p_ctx);
    p_filters = &p_ctx->filters;
    (void)img_ctx_get_flow_control(0, 0, &addr);
    (void)memcpy(&p_flow, &addr, sizeof(void *));

    {
        ikc_in_main_resamp_t in_main_resamp;
        ikc_out_main_resamp_t out_main_resamp;

        in_main_resamp.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
        in_main_resamp.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
        in_main_resamp.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
        in_main_resamp.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
        in_main_resamp.p_main = &p_filters->input_param.window_size_info.main_win;
        in_main_resamp.p_dmy_range = &p_filters->input_param.dmy_range;
        in_main_resamp.chroma_radius = p_filters->input_param.chroma_filter.radius;
        in_main_resamp.is_y2y_420input = 0;
        in_main_resamp.flip_mode = p_filters->input_param.flip_mode;
        in_main_resamp.p_burst_tile = &p_filters->input_param.burst_tile;
        in_main_resamp.force_vscale_sec_n = 0;
        out_main_resamp.p_cr_33 = p_flow_tbl_list->p_CR_buf_33;
        out_main_resamp.cr_33_size = CR_SIZE_33;
        out_main_resamp.p_cr_35 = p_flow_tbl_list->p_CR_buf_35;
        out_main_resamp.cr_35_size = CR_SIZE_35;
        out_main_resamp.p_cr_37 = p_flow_tbl_list->p_CR_buf_37;
        out_main_resamp.cr_37_size = CR_SIZE_37;
        out_main_resamp.p_cr_42 = p_flow_tbl_list->p_CR_buf_42;
        out_main_resamp.cr_42_size = CR_SIZE_42;

        Rval = ikc_main_resampler(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_main_resampler", "null-ptr fail case", id_cnt++);

        in_main_resamp.main_cut_off_freq = 0;
        in_main_resamp.p_act_win_crop->left_top_x = 0;
        in_main_resamp.p_act_win_crop->right_bot_x = 480<<16;
        in_main_resamp.p_act_win_crop->left_top_y = 0;
        in_main_resamp.p_act_win_crop->right_bot_y = 270<<16;
        in_main_resamp.p_dmy_win_geo->start_x = 0;
        in_main_resamp.p_dmy_win_geo->start_y = 0;
        in_main_resamp.p_dmy_win_geo->width = 480;
        in_main_resamp.p_dmy_win_geo->height = 270;
        in_main_resamp.p_cfa_win_dim->width = 480;
        in_main_resamp.p_cfa_win_dim->height = 270;
        in_main_resamp.p_main->width = 3840;
        in_main_resamp.p_main->height = 2160;
        in_main_resamp.p_dmy_range->enable = 0;
        in_main_resamp.chroma_radius = 32;
        in_main_resamp.flip_mode = 0;
        in_main_resamp.p_burst_tile->enable = 0;

        Rval = ikc_main_resampler(&in_main_resamp, &out_main_resamp);//up > 4x
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_main_resampler", "OK case", id_cnt++);

        in_main_resamp.force_vscale_sec_n = 2;
        Rval = ikc_main_resampler(&in_main_resamp, &out_main_resamp);//v-scale up > 4x, by sec2.
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_main_resampler", "OK case", id_cnt++);
        in_main_resamp.force_vscale_sec_n = 0;

        in_main_resamp.main_cut_off_freq = 0;
        in_main_resamp.p_act_win_crop->left_top_x = 0;
        in_main_resamp.p_act_win_crop->right_bot_x = 1920<<16;
        in_main_resamp.p_act_win_crop->left_top_y = 0;
        in_main_resamp.p_act_win_crop->right_bot_y = 1080<<16;
        in_main_resamp.p_dmy_win_geo->start_x = 0;
        in_main_resamp.p_dmy_win_geo->start_y = 0;
        in_main_resamp.p_dmy_win_geo->width = 1920;
        in_main_resamp.p_dmy_win_geo->height = 1080;
        in_main_resamp.p_cfa_win_dim->width = 1920;
        in_main_resamp.p_cfa_win_dim->height = 1080;
        in_main_resamp.p_main->width = 3840;
        in_main_resamp.p_main->height = 2160;
        in_main_resamp.p_dmy_range->enable = 0;
        in_main_resamp.chroma_radius = 32;
        in_main_resamp.flip_mode = 0;
        in_main_resamp.p_burst_tile->enable = 1;
        in_main_resamp.is_y2y_420input = 1;

        Rval = ikc_main_resampler(&in_main_resamp, &out_main_resamp);//up 2x
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_main_resampler", "OK case", id_cnt++);

        in_main_resamp.p_burst_tile->enable = 0;
        in_main_resamp.is_y2y_420input = 0;
        in_main_resamp.force_vscale_sec_n = 2;
        Rval = ikc_main_resampler(&in_main_resamp, &out_main_resamp);//v-scale up 2x, by sec2
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_main_resampler", "OK case", id_cnt++);
        in_main_resamp.force_vscale_sec_n = 0;

        in_main_resamp.main_cut_off_freq = 7;
        in_main_resamp.p_act_win_crop->left_top_x = 0;
        in_main_resamp.p_act_win_crop->right_bot_x = 4000<<16;
        in_main_resamp.p_act_win_crop->left_top_y = 0;
        in_main_resamp.p_act_win_crop->right_bot_y = 3000<<16;
        in_main_resamp.p_dmy_win_geo->start_x = 0;
        in_main_resamp.p_dmy_win_geo->start_y = 0;
        in_main_resamp.p_dmy_win_geo->width = 4000;
        in_main_resamp.p_dmy_win_geo->height = 3000;
        in_main_resamp.p_cfa_win_dim->width = 4000;
        in_main_resamp.p_cfa_win_dim->height = 3000;
        in_main_resamp.p_main->width = 3840;
        in_main_resamp.p_main->height = 2160;
        in_main_resamp.p_dmy_range->enable = 0;
        in_main_resamp.chroma_radius = 32;
        in_main_resamp.flip_mode = 0;
        in_main_resamp.p_burst_tile->enable = 1;

        Rval = ikc_main_resampler(&in_main_resamp, &out_main_resamp);//v-scale down by sec2
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_main_resampler", "OK case", id_cnt++);

        in_main_resamp.force_vscale_sec_n = 3;
        Rval = ikc_main_resampler(&in_main_resamp, &out_main_resamp);//v-scale down by sec3
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_main_resampler", "OK case", id_cnt++);
        in_main_resamp.force_vscale_sec_n = 0;

        in_main_resamp.main_cut_off_freq = 0;
        in_main_resamp.p_act_win_crop->left_top_x = 0;
        in_main_resamp.p_act_win_crop->right_bot_x = 3840<<16;
        in_main_resamp.p_act_win_crop->left_top_y = 0;
        in_main_resamp.p_act_win_crop->right_bot_y = 2160<<16;
        in_main_resamp.p_dmy_win_geo->start_x = 0;
        in_main_resamp.p_dmy_win_geo->start_y = 0;
        in_main_resamp.p_dmy_win_geo->width = 3840;
        in_main_resamp.p_dmy_win_geo->height = 2160;
        in_main_resamp.p_cfa_win_dim->width = 3840;
        in_main_resamp.p_cfa_win_dim->height = 2160;
        in_main_resamp.p_main->width = 960;
        in_main_resamp.p_main->height = 540;
        in_main_resamp.p_dmy_range->enable = 1;
        in_main_resamp.chroma_radius = 64;
        in_main_resamp.flip_mode = 0;
        in_main_resamp.p_burst_tile->enable = 0;

        Rval = ikc_main_resampler(&in_main_resamp, &out_main_resamp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_main_resampler", "Invalid window geometry fail case", id_cnt++);

        in_main_resamp.main_cut_off_freq = 0;
        in_main_resamp.p_act_win_crop->left_top_x = 4000<<16;
        in_main_resamp.p_act_win_crop->right_bot_x = 4000<<16;
        in_main_resamp.p_act_win_crop->left_top_y = 8640<<16;
        in_main_resamp.p_act_win_crop->right_bot_y = 8640<<16;
        in_main_resamp.p_dmy_win_geo->start_x = 1000;
        in_main_resamp.p_dmy_win_geo->start_y = 1000;
        in_main_resamp.p_dmy_win_geo->width = 3840;
        in_main_resamp.p_dmy_win_geo->height = 2160;
        in_main_resamp.p_cfa_win_dim->width = 960;
        in_main_resamp.p_cfa_win_dim->height = 540;
        in_main_resamp.p_main->width = 480;
        in_main_resamp.p_main->height = 270;
        in_main_resamp.p_dmy_range->enable = 0;
        in_main_resamp.chroma_radius = 32;
        in_main_resamp.flip_mode = 0;
        in_main_resamp.p_burst_tile->enable = 0;

        Rval = ikc_main_resampler(&in_main_resamp, &out_main_resamp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_main_resampler", "Invalid window geometry fail case", id_cnt++);

    }
    extern uint8 radius2hw_scale_code(uint32 chroma_radius);
    Rval = radius2hw_scale_code(16);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "radius2hw_scale_code", "OK case", id_cnt++);

    Rval = radius2hw_scale_code(0);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "radius2hw_scale_code", "OK case", id_cnt++);

    {
        ikc_in_cfa_prescale_t in_cfa_prescale;
        ikc_out_cfa_prescale_t out_cfa_prescale;

        in_cfa_prescale.sensor_readout_mode = p_filters->input_param.sensor_info.sensor_readout_mode;
        in_cfa_prescale.cfa_cut_off_freq = p_filters->input_param.resample_str.cfa_cut_off_freq;
        in_cfa_prescale.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
        in_cfa_prescale.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
        in_cfa_prescale.cawarp_enable = (uint32)p_filters->input_param.cawarp_enable;
        out_cfa_prescale.p_cr_16 = p_flow_tbl_list->p_CR_buf_16;
        out_cfa_prescale.cr_16_size = CR_SIZE_16;

        Rval = ikc_cfa_prescale(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_cfa_prescale", "null-ptr fail case", id_cnt++);

        in_cfa_prescale.sensor_readout_mode = (1<<8) + 1;
        in_cfa_prescale.cfa_cut_off_freq = 0;
        in_cfa_prescale.p_dmy_win_geo->start_x = 0;
        in_cfa_prescale.p_dmy_win_geo->start_y = 0;
        in_cfa_prescale.p_dmy_win_geo->width = 1920;
        in_cfa_prescale.p_dmy_win_geo->height = 1080;
        in_cfa_prescale.p_cfa_win_dim->width = 1920;
        in_cfa_prescale.p_cfa_win_dim->height = 1080;
        in_cfa_prescale.cawarp_enable = 0;

        Rval = ikc_cfa_prescale(&in_cfa_prescale, &out_cfa_prescale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_cfa_prescale", "OK case", id_cnt++);

        in_cfa_prescale.sensor_readout_mode = (2<<8) + 2;
        Rval = ikc_cfa_prescale(&in_cfa_prescale, &out_cfa_prescale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_cfa_prescale", "OK case", id_cnt++);

        in_cfa_prescale.sensor_readout_mode = (3<<8) + 3;
        Rval = ikc_cfa_prescale(&in_cfa_prescale, &out_cfa_prescale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_cfa_prescale", "OK case", id_cnt++);

        in_cfa_prescale.sensor_readout_mode = (4<<8) + 4;
        Rval = ikc_cfa_prescale(&in_cfa_prescale, &out_cfa_prescale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_cfa_prescale", "OK case", id_cnt++);

        in_cfa_prescale.sensor_readout_mode = (5<<8) + 5;
        Rval = ikc_cfa_prescale(&in_cfa_prescale, &out_cfa_prescale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_cfa_prescale", "OK case", id_cnt++);

        in_cfa_prescale.sensor_readout_mode = (6<<8) + 6;
        Rval = ikc_cfa_prescale(&in_cfa_prescale, &out_cfa_prescale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_cfa_prescale", "OK case", id_cnt++);

        in_cfa_prescale.p_dmy_win_geo->width = 3840;
        in_cfa_prescale.p_dmy_win_geo->height = 2160;
        in_cfa_prescale.sensor_readout_mode = 0;
        Rval = ikc_cfa_prescale(&in_cfa_prescale, &out_cfa_prescale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitResampler.c", "ikc_cfa_prescale", "OK case", id_cnt++);

    }

}

static void IK_TestCovr_IkcUnitVignette(void)
{
    uint32 Rval = IK_OK;
    uintptr addr = 0;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_filter_t *p_filters = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    idsp_ik_flow_ctrl_t *p_flow = NULL;
    (void)img_ctx_get_flow_tbl_list(0, 0, &addr);
    (void)memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
    (void)img_ctx_get_context(0, &p_ctx);
    p_filters = &p_ctx->filters;
    (void)img_ctx_get_flow_control(0, 0, &addr);
    (void)memcpy(&p_flow, &addr, sizeof(void *));

    {
        ikc_in_vignette_t in_vig;
        ikc_out_vignette_t out_vig;

        in_vig.vig_enable = p_filters->input_param.vig_enable;
        in_vig.flip_mode = p_filters->input_param.flip_mode;
        in_vig.p_vignette_compensation = &p_filters->input_param.vignette_compensation;
        in_vig.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
        in_vig.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
        out_vig.p_cr_9 = p_flow_tbl_list->p_CR_buf_9;
        out_vig.cr_9_size = CR_SIZE_9;
        out_vig.p_cr_10 = p_flow_tbl_list->p_CR_buf_10;
        out_vig.cr_10_size = CR_SIZE_10;
        out_vig.p_cr_11 = p_flow_tbl_list->p_CR_buf_11;
        out_vig.cr_11_size = CR_SIZE_11;
        out_vig.p_flow = &p_flow->flow_info;

        Rval = ikc_vignette(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitVignette.c", "ikc_vignette", "null-ptr fail case", id_cnt++);

        in_vig.vig_enable = 1;
        in_vig.flip_mode = (IK_FLIP_RAW_H | IK_FLIP_RAW_V);
        in_vig.p_vignette_compensation->calib_mode_enable = 1;
        in_vig.p_vignette_compensation->vin_sensor_geo.start_x = 0;
        in_vig.p_vignette_compensation->vin_sensor_geo.start_y = 0;
        in_vig.p_vignette_compensation->vin_sensor_geo.width = 1920;
        in_vig.p_vignette_compensation->vin_sensor_geo.height = 1080;
        in_vig.p_vignette_compensation->vin_sensor_geo.h_sub_sample.factor_num = 1;
        in_vig.p_vignette_compensation->vin_sensor_geo.h_sub_sample.factor_den = 1;
        in_vig.p_vignette_compensation->vin_sensor_geo.v_sub_sample.factor_num = 1;
        in_vig.p_vignette_compensation->vin_sensor_geo.v_sub_sample.factor_den = 1;
        in_vig.p_vignette_compensation->calib_vignette_info.version = 0x20180401UL;
        in_vig.p_vignette_compensation->calib_vignette_info.num_radial_bins_coarse = 64;
        in_vig.p_vignette_compensation->calib_vignette_info.size_radial_bins_coarse_log = 4;
        in_vig.p_vignette_compensation->calib_vignette_info.num_radial_bins_fine = 64;
        in_vig.p_vignette_compensation->calib_vignette_info.size_radial_bins_fine_log = 2;
        in_vig.p_vignette_compensation->calib_vignette_info.model_center_x_R = 960;
        in_vig.p_vignette_compensation->calib_vignette_info.model_center_x_Gr = 960;
        in_vig.p_vignette_compensation->calib_vignette_info.model_center_x_B = 960;
        in_vig.p_vignette_compensation->calib_vignette_info.model_center_x_Gb = 960;
        in_vig.p_vignette_compensation->calib_vignette_info.model_center_y_R = 540;
        in_vig.p_vignette_compensation->calib_vignette_info.model_center_y_Gr = 540;
        in_vig.p_vignette_compensation->calib_vignette_info.model_center_y_B = 540;
        in_vig.p_vignette_compensation->calib_vignette_info.model_center_y_Gb = 540;
        //in_vig.p_vignette_compensation->calib_vignette_info.vig_gain_tbl_R[4][128];            // pointer to [4 bayer chan][4 direction][128 vertices] 0:9b mult 10:13b shift
        //in_vig.p_vignette_compensation->calib_vignette_info.vig_gain_tbl_Gr[4][128];
        //in_vig.p_vignette_compensation->calib_vignette_info.vig_gain_tbl_B[4][128];
        //in_vig.p_vignette_compensation->calib_vignette_info.vig_gain_tbl_Gb[4][128];
        in_vig.p_vin_sensor->start_x = 0;
        in_vig.p_vin_sensor->start_y = 0;
        in_vig.p_vin_sensor->width = 960;
        in_vig.p_vin_sensor->height = 540;
        in_vig.p_vin_sensor->h_sub_sample.factor_num = 1;
        in_vig.p_vin_sensor->h_sub_sample.factor_den = 2;
        in_vig.p_vin_sensor->v_sub_sample.factor_num = 1;
        in_vig.p_vin_sensor->v_sub_sample.factor_den = 2;
        in_vig.p_dmy_win_geo->start_x = 0;
        in_vig.p_dmy_win_geo->start_y = 0;
        in_vig.p_dmy_win_geo->width = 960;
        in_vig.p_dmy_win_geo->height = 540;

        Rval = ikc_vignette(&in_vig, &out_vig);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitVignette.c", "ikc_vignette", "OK case", id_cnt++);

        in_vig.p_vin_sensor->width = 480;
        in_vig.p_vin_sensor->height = 270;
        in_vig.p_vin_sensor->h_sub_sample.factor_num = 1;
        in_vig.p_vin_sensor->h_sub_sample.factor_den = 4;
        in_vig.p_vin_sensor->v_sub_sample.factor_num = 1;
        in_vig.p_vin_sensor->v_sub_sample.factor_den = 4;
        in_vig.p_dmy_win_geo->width = 480;
        in_vig.p_dmy_win_geo->height = 270;
        Rval = ikc_vignette(&in_vig, &out_vig);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitVignette.c", "ikc_vignette", "OK case", id_cnt++);

        in_vig.p_vignette_compensation->calib_vignette_info.size_radial_bins_coarse_log = 0;
        in_vig.p_vignette_compensation->calib_vignette_info.size_radial_bins_fine_log = 0;
        Rval = ikc_vignette(&in_vig, &out_vig);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitVignette.c", "ikc_vignette", "wrong binning mode fail case", id_cnt++);
        in_vig.p_vignette_compensation->calib_vignette_info.size_radial_bins_coarse_log = 4;
        in_vig.p_vignette_compensation->calib_vignette_info.size_radial_bins_fine_log = 2;

        in_vig.p_vin_sensor->width = 240;
        in_vig.p_vin_sensor->height = 135;
        in_vig.p_vin_sensor->h_sub_sample.factor_num = 1;
        in_vig.p_vin_sensor->h_sub_sample.factor_den = 8;
        in_vig.p_vin_sensor->v_sub_sample.factor_num = 1;
        in_vig.p_vin_sensor->v_sub_sample.factor_den = 8;
        in_vig.p_dmy_win_geo->width = 240;
        in_vig.p_dmy_win_geo->height = 135;
        Rval = ikc_vignette(&in_vig, &out_vig);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitVignette.c", "ikc_vignette", "wrong binning mode fail case", id_cnt++);

        in_vig.p_vin_sensor->width = 1920;
        in_vig.p_vin_sensor->height = 540;
        in_vig.p_vin_sensor->h_sub_sample.factor_num = 1;
        in_vig.p_vin_sensor->h_sub_sample.factor_den = 1;
        in_vig.p_vin_sensor->v_sub_sample.factor_num = 1;
        in_vig.p_vin_sensor->v_sub_sample.factor_den = 2;
        in_vig.p_dmy_win_geo->width = 1920;
        in_vig.p_dmy_win_geo->height = 540;
        Rval = ikc_vignette(&in_vig, &out_vig);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitVignette.c", "ikc_vignette", "wrong binning mode fail case", id_cnt++);

        in_vig.p_vignette_compensation->vin_sensor_geo.width = 1000;
        in_vig.p_vignette_compensation->vin_sensor_geo.height = 1080;
        in_vig.p_vin_sensor->width = 1920;
        in_vig.p_vin_sensor->height = 1080;
        in_vig.p_vin_sensor->h_sub_sample.factor_num = 1;
        in_vig.p_vin_sensor->h_sub_sample.factor_den = 1;
        in_vig.p_vin_sensor->v_sub_sample.factor_num = 1;
        in_vig.p_vin_sensor->v_sub_sample.factor_den = 1;
        in_vig.p_dmy_win_geo->width = 1920;
        in_vig.p_dmy_win_geo->height = 1080;
        Rval = ikc_vignette(&in_vig, &out_vig);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitVignette.c", "ikc_vignette", "Invalid calib window geometry fail case", id_cnt++);
    }

    {
        ikc_in_vignette_t in_vig;
        ikc_out_vignette_sub_t out_vig_sub;

        in_vig.vig_enable = p_filters->input_param.vig_enable;
        in_vig.flip_mode = p_filters->input_param.flip_mode;
        in_vig.p_vignette_compensation = &p_filters->input_param.vignette_compensation;
        in_vig.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
        in_vig.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
        out_vig_sub.p_cr_47 = p_flow_tbl_list->p_CR_buf_47;
        out_vig_sub.cr_47_size = CR_SIZE_47;
        out_vig_sub.p_cr_48 = p_flow_tbl_list->p_CR_buf_48;
        out_vig_sub.cr_48_size = CR_SIZE_48;
        out_vig_sub.p_cr_49 = p_flow_tbl_list->p_CR_buf_49;
        out_vig_sub.cr_49_size = CR_SIZE_49;

        Rval = ikc_vignette_sub(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitVignette.c", "ikc_vignette_sub", "null-ptr fail case", id_cnt++);

        in_vig.vig_enable = 1;
        in_vig.flip_mode = (IK_FLIP_RAW_H | IK_FLIP_RAW_V);
        in_vig.p_vignette_compensation->calib_mode_enable = 0;
        in_vig.p_vignette_compensation->vin_sensor_geo.start_x = 0;
        in_vig.p_vignette_compensation->vin_sensor_geo.start_y = 0;
        in_vig.p_vignette_compensation->vin_sensor_geo.width = 1920;
        in_vig.p_vignette_compensation->vin_sensor_geo.height = 1080;
        in_vig.p_vignette_compensation->vin_sensor_geo.h_sub_sample.factor_num = 1;
        in_vig.p_vignette_compensation->vin_sensor_geo.h_sub_sample.factor_den = 1;
        in_vig.p_vignette_compensation->vin_sensor_geo.v_sub_sample.factor_num = 1;
        in_vig.p_vignette_compensation->vin_sensor_geo.v_sub_sample.factor_den = 1;
        in_vig.p_vignette_compensation->calib_vignette_info.version = 0x20180401UL;
        in_vig.p_vignette_compensation->calib_vignette_info.num_radial_bins_coarse = 64;
        in_vig.p_vignette_compensation->calib_vignette_info.size_radial_bins_coarse_log = 4;
        in_vig.p_vignette_compensation->calib_vignette_info.num_radial_bins_fine = 64;
        in_vig.p_vignette_compensation->calib_vignette_info.size_radial_bins_fine_log = 2;
        in_vig.p_vignette_compensation->calib_vignette_info.model_center_x_R = 960;
        in_vig.p_vignette_compensation->calib_vignette_info.model_center_x_Gr = 960;
        in_vig.p_vignette_compensation->calib_vignette_info.model_center_x_B = 960;
        in_vig.p_vignette_compensation->calib_vignette_info.model_center_x_Gb = 960;
        in_vig.p_vignette_compensation->calib_vignette_info.model_center_y_R = 540;
        in_vig.p_vignette_compensation->calib_vignette_info.model_center_y_Gr = 540;
        in_vig.p_vignette_compensation->calib_vignette_info.model_center_y_B = 540;
        in_vig.p_vignette_compensation->calib_vignette_info.model_center_y_Gb = 540;
        //in_vig.p_vignette_compensation->calib_vignette_info.vig_gain_tbl_R[4][128];            // pointer to [4 bayer chan][4 direction][128 vertices] 0:9b mult 10:13b shift
        //in_vig.p_vignette_compensation->calib_vignette_info.vig_gain_tbl_Gr[4][128];
        //in_vig.p_vignette_compensation->calib_vignette_info.vig_gain_tbl_B[4][128];
        //in_vig.p_vignette_compensation->calib_vignette_info.vig_gain_tbl_Gb[4][128];
        in_vig.p_vin_sensor->start_x = 0;
        in_vig.p_vin_sensor->start_y = 0;
        in_vig.p_vin_sensor->width = 1920;
        in_vig.p_vin_sensor->height = 1080;
        in_vig.p_vin_sensor->h_sub_sample.factor_num = 1;
        in_vig.p_vin_sensor->h_sub_sample.factor_den = 1;
        in_vig.p_vin_sensor->v_sub_sample.factor_num = 1;
        in_vig.p_vin_sensor->v_sub_sample.factor_den = 1;
        in_vig.p_dmy_win_geo->start_x = 0;
        in_vig.p_dmy_win_geo->start_y = 0;
        in_vig.p_dmy_win_geo->width = 1920;
        in_vig.p_dmy_win_geo->height = 1080;

        Rval = ikc_vignette_sub(&in_vig, &out_vig_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitVignette.c", "ikc_vignette_sub", "OK case", id_cnt++);

        in_vig.p_vignette_compensation->calib_mode_enable = 1;
        in_vig.p_vignette_compensation->vin_sensor_geo.width = 1000;
        in_vig.p_vignette_compensation->vin_sensor_geo.height = 1080;
        Rval = ikc_vignette_sub(&in_vig, &out_vig_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitVignette.c", "ikc_vignette_sub", "Invalid calib window geometry fail case", id_cnt++);

    }
}

static void IK_TestCovr_IkcUnitStaticBadPixel(void)
{
    uint32 Rval = IK_OK;
    uintptr addr = 0;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_filter_t *p_filters = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    idsp_ik_flow_ctrl_t *p_flow = NULL;
    (void)img_ctx_get_flow_tbl_list(0, 0, &addr);
    (void)memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
    (void)img_ctx_get_context(0, &p_ctx);
    p_filters = &p_ctx->filters;
    (void)img_ctx_get_flow_control(0, 0, &addr);
    (void)memcpy(&p_flow, &addr, sizeof(void *));

    {
        ikc_in_static_bad_pixel_t in_sbp;
        ikc_out_static_bad_pixel_t out_sbp;
        ik_static_bad_pxl_cor_t static_bpc_tmp;
        uint8 test_sbp_tbl[4*32];

        in_sbp.dbp_enable = p_filters->input_param.dynamic_bpc.enable;
        in_sbp.sbp_enable = p_filters->input_param.sbp_enable;
        in_sbp.p_static_bpc = &static_bpc_tmp;
        in_sbp.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
        out_sbp.p_cr_12 = p_flow_tbl_list->p_CR_buf_12;
        out_sbp.cr_12_size = CR_SIZE_12;
        out_sbp.p_flow = &p_flow->flow_info;

        Rval = ikc_static_bad_pixel(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitStaticBadPixel.c", "ikc_static_bad_pixel", "null-ptr fail case", id_cnt++);

        in_sbp.dbp_enable = 0;
        in_sbp.sbp_enable = 1;
        in_sbp.p_static_bpc->calib_sbp_info.version = 0x20180401;
        in_sbp.p_static_bpc->calib_sbp_info.sbp_buffer = test_sbp_tbl;
        in_sbp.p_static_bpc->vin_sensor_geo.start_x = 0;
        in_sbp.p_static_bpc->vin_sensor_geo.start_y = 0;
        in_sbp.p_static_bpc->vin_sensor_geo.width = 32;
        in_sbp.p_static_bpc->vin_sensor_geo.height = 32;
        in_sbp.p_static_bpc->vin_sensor_geo.h_sub_sample.factor_num = 1;
        in_sbp.p_static_bpc->vin_sensor_geo.h_sub_sample.factor_den = 1;
        in_sbp.p_static_bpc->vin_sensor_geo.v_sub_sample.factor_num = 1;
        in_sbp.p_static_bpc->vin_sensor_geo.v_sub_sample.factor_den = 1;
        in_sbp.p_vin_sensor->start_x = 0;
        in_sbp.p_vin_sensor->start_y = 0;
        in_sbp.p_vin_sensor->width = 32;
        in_sbp.p_vin_sensor->height = 32;
        in_sbp.p_vin_sensor->h_sub_sample.factor_num = 1;
        in_sbp.p_vin_sensor->h_sub_sample.factor_den = 1;
        in_sbp.p_vin_sensor->v_sub_sample.factor_num = 1;
        in_sbp.p_vin_sensor->v_sub_sample.factor_den = 1;

        Rval = ikc_static_bad_pixel(&in_sbp, &out_sbp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitStaticBadPixel.c", "ikc_static_bad_pixel", "OK case", id_cnt++);

        in_sbp.p_vin_sensor->start_x = 2;
        in_sbp.p_vin_sensor->width = 30;
        Rval = ikc_static_bad_pixel(&in_sbp, &out_sbp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitStaticBadPixel.c", "ikc_static_bad_pixel", "wrong offset alignment fail case", id_cnt++);
        in_sbp.p_vin_sensor->start_x = 0;
        in_sbp.p_vin_sensor->width = 32;


        in_sbp.p_static_bpc->calib_sbp_info.sbp_buffer = NULL;
        in_sbp.p_vin_sensor->h_sub_sample.factor_num = 0;
        Rval = ikc_static_bad_pixel(&in_sbp, &out_sbp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitStaticBadPixel.c", "ikc_static_bad_pixel", "wrong binning mode fail case", id_cnt++);

    }
    {
        ikc_in_static_bad_pixel_internal_t in_sbp_internal;
        ikc_out_static_bad_pixel_internal_t out_sbp;
        ik_static_bad_pixel_correction_internal_t sbp_internal_tmp;

        in_sbp_internal.dbp_enable = p_filters->input_param.dynamic_bpc.enable;
        in_sbp_internal.p_sbp_internal = &sbp_internal_tmp;
        out_sbp.p_cr_12 = p_flow_tbl_list->p_CR_buf_12;
        out_sbp.cr_12_size = CR_SIZE_12;
        out_sbp.p_flow = &p_flow->flow_info;

        Rval = ikc_static_bad_pixel_internal(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitStaticBadPixel.c", "ikc_static_bad_pixel_internal", "null-ptr fail case", id_cnt++);

        in_sbp_internal.dbp_enable = 1;
        in_sbp_internal.p_sbp_internal->enable = 1;
        in_sbp_internal.p_sbp_internal->pixel_map_width = 32;
        in_sbp_internal.p_sbp_internal->pixel_map_height = 32;
        in_sbp_internal.p_sbp_internal->pixel_map_pitch = 32;
        in_sbp_internal.p_sbp_internal->p_map = NULL;

        Rval = ikc_static_bad_pixel_internal(&in_sbp_internal, &out_sbp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitStaticBadPixel.c", "ikc_static_bad_pixel_internal", "null-ptr fail case", id_cnt++);

    }
}

static void IK_TestCovr_IkcUnitYuvFilters(void)
{
    uint32 Rval = IK_OK;
    uintptr addr = 0;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_filter_t *p_filters = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    idsp_ik_flow_ctrl_t *p_flow = NULL;
    (void)img_ctx_get_flow_tbl_list(0, 0, &addr);
    (void)memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
    (void)img_ctx_get_context(0, &p_ctx);
    p_filters = &p_ctx->filters;
    (void)img_ctx_get_flow_control(0, 0, &addr);
    (void)memcpy(&p_flow, &addr, sizeof(void *));

    {
        ikc_in_chroma_scale_t in_chroma_scale;
        ikc_out_chroma_scale_t out_chroma_scale;

        in_chroma_scale.is_yuv_mode = 0UL;
        in_chroma_scale.p_chroma_scale = &p_filters->input_param.chroma_scale;
        out_chroma_scale.p_cr_29 = p_flow_tbl_list->p_CR_buf_29;
        out_chroma_scale.cr_29_size = CR_SIZE_29;

        Rval = ikc_chroma_scale(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitYuvFilters.c", "ikc_chroma_scale", "null-ptr fail case", id_cnt++);

        in_chroma_scale.p_chroma_scale->enable = 2;
        Rval = ikc_chroma_scale(&in_chroma_scale, &out_chroma_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitYuvFilters.c", "ikc_chroma_scale", "OK case", id_cnt++);

        in_chroma_scale.p_chroma_scale->enable = 3;
        Rval = ikc_chroma_scale(&in_chroma_scale, &out_chroma_scale);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitYuvFilters.c", "ikc_chroma_scale", "OK case", id_cnt++);

    }

    {
        ikc_in_chroma_median_t in_chroma_median;
        ikc_out_chroma_median_t out_chroma_median;

        in_chroma_median.is_yuv_mode = 0;
        in_chroma_median.p_chroma_median_filter = &p_filters->input_param.chroma_median_filter;
        out_chroma_median.p_cr_30 = p_flow_tbl_list->p_CR_buf_30;
        out_chroma_median.cr_30_size = CR_SIZE_30;
        out_chroma_median.p_cr_31 = p_flow_tbl_list->p_CR_buf_31;
        out_chroma_median.cr_31_size = CR_SIZE_31;

        Rval = ikc_chroma_median(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitYuvFilters.c", "ikc_chroma_median", "null-ptr fail case", id_cnt++);

        in_chroma_median.p_chroma_median_filter->enable = 1;
        in_chroma_median.p_chroma_median_filter->cb_adaptive_strength = 256;
        in_chroma_median.p_chroma_median_filter->cr_adaptive_strength = 256;
        in_chroma_median.p_chroma_median_filter->cb_non_adaptive_strength = 0;
        in_chroma_median.p_chroma_median_filter->cr_non_adaptive_strength = 0;
        in_chroma_median.p_chroma_median_filter->cb_adaptive_amount = 0;
        in_chroma_median.p_chroma_median_filter->cr_adaptive_amount = 0;

        Rval = ikc_chroma_median(&in_chroma_median, &out_chroma_median);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitYuvFilters.c", "ikc_chroma_median", "OK case", id_cnt++);

    }

    {
        ikc_in_chroma_noise_t in_chroma_noise;
        ikc_out_chroma_noise_t out_chroma_noise;

        in_chroma_noise.is_group_cmd = 0u;
        in_chroma_noise.p_chroma_filter = &p_filters->input_param.chroma_filter;
        in_chroma_noise.p_wide_chroma_filter = &p_filters->input_param.wide_chroma_filter;
        in_chroma_noise.p_wide_chroma_combine = &p_filters->input_param.wide_chroma_combine;
        out_chroma_noise.p_cr_36 = p_flow_tbl_list->p_CR_buf_36;
        out_chroma_noise.cr_36_size = CR_SIZE_36;
        out_chroma_noise.p_window = &p_flow->window;

        Rval = ikc_chroma_noise(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitYuvFilters.c", "ikc_chroma_noise", "null-ptr fail case", id_cnt++);

        in_chroma_noise.p_wide_chroma_filter->enable = 0;
        in_chroma_noise.p_chroma_filter->enable = 1;
        in_chroma_noise.p_chroma_filter->radius = 32;
        in_chroma_noise.p_chroma_filter->original_blend_strength_cb = 256;

        Rval = ikc_chroma_noise(&in_chroma_noise, &out_chroma_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitYuvFilters.c", "ikc_chroma_noise", "OK case", id_cnt++);

        in_chroma_noise.p_wide_chroma_filter->enable = 1;

        Rval = ikc_chroma_noise(&in_chroma_noise, &out_chroma_noise);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitYuvFilters.c", "ikc_chroma_noise", "Invalid parameter fail case", id_cnt++);

    }
}

static void IK_TestCovr_IkcUnitRgbFilters(void)
{
    uint32 Rval = IK_OK;
    uintptr addr = 0;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_filter_t *p_filters = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    idsp_ik_flow_ctrl_t *p_flow = NULL;
    (void)img_ctx_get_flow_tbl_list(0, 0, &addr);
    (void)memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
    (void)img_ctx_get_context(0, &p_ctx);
    p_filters = &p_ctx->filters;
    (void)img_ctx_get_flow_control(0, 0, &addr);
    (void)memcpy(&p_flow, &addr, sizeof(void *));

    {
        ikc_in_demosaic_t in_demosaic;
        ikc_out_demosaic_t out_demosaic;

        in_demosaic.p_demosaic = &p_filters->input_param.demosaic;
        out_demosaic.p_cr_12 = p_flow_tbl_list->p_CR_buf_12;
        out_demosaic.cr_12_size = CR_SIZE_12;
        out_demosaic.p_cr_23 = p_flow_tbl_list->p_CR_buf_23;
        out_demosaic.cr_23_size = CR_SIZE_23;
        out_demosaic.p_cr_24 = p_flow_tbl_list->p_CR_buf_24;
        out_demosaic.cr_24_size = CR_SIZE_24;

        Rval = ikc_demosaic_filter(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRgbFilters.c", "ikc_demosaic_filter", "null-ptr fail case", id_cnt++);

        in_demosaic.p_demosaic->enable = 0;
        in_demosaic.p_demosaic->activity_thresh = 0;
        in_demosaic.p_demosaic->activity_difference_thresh = 0;
        in_demosaic.p_demosaic->grad_clip_thresh = 0;
        in_demosaic.p_demosaic->grad_noise_thresh = 0;
        in_demosaic.p_demosaic->alias_interpolation_strength = 33;
        in_demosaic.p_demosaic->alias_interpolation_thresh = 1;

        Rval = ikc_demosaic_filter(&in_demosaic, &out_demosaic);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRgbFilters.c", "ikc_demosaic_filter", "OK case", id_cnt++);

        in_demosaic.p_demosaic->alias_interpolation_strength = 38;

        Rval = ikc_demosaic_filter(&in_demosaic, &out_demosaic);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRgbFilters.c", "ikc_demosaic_filter", "OK case", id_cnt++);

    }
    Rval = ikc_color_correction_enable(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRgbFilters.c", "ikc_color_correction_enable", "null-ptr fail case", id_cnt++);

    Rval = ikc_color_correction_reg_y2y(NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRgbFilters.c", "ikc_color_correction_reg_y2y", "null-ptr fail case", id_cnt++);

    Rval = ikc_color_correction(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRgbFilters.c", "ikc_color_correction", "null-ptr fail case", id_cnt++);

    Rval = ikc_color_correction_out(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRgbFilters.c", "ikc_color_correction_out", "null-ptr fail case", id_cnt++);

    Rval = ikc_rgb2yuv(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRgbFilters.c", "ikc_rgb2yuv", "null-ptr fail case", id_cnt++);

    {
        ikc_in_rgb12y_t in_rgb12y;
        ikc_out_rgb12y_t out_rgb12y;

        in_rgb12y.p_rgb_to_12y = &p_filters->input_param.rgb_to_12y;
        out_rgb12y.p_cr_25 = p_flow_tbl_list->p_CR_buf_25;
        out_rgb12y.cr_25_size = CR_SIZE_25;

        Rval = ikc_rgb12y(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRgbFilters.c", "ikc_rgb12y", "null-ptr fail case", id_cnt++);

        in_rgb12y.p_rgb_to_12y->enable = 1;
        in_rgb12y.p_rgb_to_12y ->y_offset = 0;
        in_rgb12y.p_rgb_to_12y ->matrix_values[0] = 4000;
        in_rgb12y.p_rgb_to_12y ->matrix_values[1] = 0;
        in_rgb12y.p_rgb_to_12y ->matrix_values[2] = 0;

        Rval = ikc_rgb12y(&in_rgb12y, &out_rgb12y);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRgbFilters.c", "ikc_rgb12y", "OK case", id_cnt++);

    }

    {
        ikc_in_pre_cc_gain_t in_pre_cc_gain;
        ikc_out_pre_cc_gain_t out_pre_cc_gain;
        extern uint32 ikc_pre_cc_gain(const ikc_in_pre_cc_gain_t *p_in_pre_cc_gain, ikc_out_pre_cc_gain_t *p_out_pre_cc_gain);

        Rval = ikc_pre_cc_gain(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRgbFilters.c", "ikc_pre_cc_gain", "test if (rval == IK_OK) false NG case", id_cnt++);

        in_pre_cc_gain.p_pre_cc_gain = &p_filters->input_param.pre_cc_gain;
        out_pre_cc_gain.p_cr_26 = p_flow_tbl_list->p_CR_buf_26;
        out_pre_cc_gain.cr_26_size = CR_SIZE_26;

        p_filters->input_param.pre_cc_gain.enable = 1;
        Rval = ikc_pre_cc_gain(&in_pre_cc_gain, &out_pre_cc_gain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRgbFilters.c", "ikc_pre_cc_gain", "test if (p_in_pre_cc_gain->p_pre_cc_gain->enable == 1u) true OK case", id_cnt++);
    }
}

static void IK_TestCovr_IkcUnitCfaFilters(void)
{
    uint32 Rval = IK_OK;
    uintptr addr = 0;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_filter_t *p_filters = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    idsp_ik_flow_ctrl_t *p_flow = NULL;
    (void)img_ctx_get_flow_tbl_list(0, 0, &addr);
    (void)memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
    (void)img_ctx_get_context(0, &p_ctx);
    p_filters = &p_ctx->filters;
    (void)img_ctx_get_flow_control(0, 0, &addr);
    (void)memcpy(&p_flow, &addr, sizeof(void *));

    {
        ikc_in_dynamic_bad_pixel_t in_dbp;
        ikc_out_dynamic_bad_pixel_t out_dbp;

        in_dbp.sbp_enable = 0;
        in_dbp.rgb_ir_mode = 0u;
        in_dbp.p_dbp = &p_filters->input_param.dynamic_bpc;
        out_dbp.p_cr_11 = p_flow_tbl_list->p_CR_buf_11;
        out_dbp.cr_11_size = CR_SIZE_11;
        out_dbp.p_cr_12 = p_flow_tbl_list->p_CR_buf_12;
        out_dbp.cr_12_size = CR_SIZE_12;

        Rval = ikc_dynamic_bad_pixel(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_dynamic_bad_pixel", "null-ptr fail case", id_cnt++);

        in_dbp.p_dbp->correction_method = 1;
        in_dbp.p_dbp->hot_pixel_strength = 1;
        in_dbp.p_dbp->dark_pixel_strength = 1;

        Rval = ikc_dynamic_bad_pixel(&in_dbp, &out_dbp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_dynamic_bad_pixel", "OK case", id_cnt++);

    }

    Rval = ikc_grgb_mismatch(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_grgb_mismatch", "null-ptr fail case", id_cnt++);

    {
        ikc_in_cfa_leak_anti_alias_t in_cfa_leak_anti_alias;
        ikc_out_cfa_leak_anti_alias_t out_cfa_leak_anti_alias;

        in_cfa_leak_anti_alias.p_cfa_leakage_filter = &p_filters->input_param.cfa_leakage_filter;
        in_cfa_leak_anti_alias.p_anti_aliasing = &p_filters->input_param.anti_aliasing;
        out_cfa_leak_anti_alias.p_cr_12 = p_flow_tbl_list->p_CR_buf_12;
        out_cfa_leak_anti_alias.cr_12_size = CR_SIZE_12;

        Rval = ikc_cfa_leakage_anti_aliasing(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_cfa_leakage_anti_aliasing", "null-ptr fail case", id_cnt++);

        in_cfa_leak_anti_alias.p_cfa_leakage_filter->enable = 0;
        in_cfa_leak_anti_alias.p_anti_aliasing->enable = 2;

        Rval = ikc_cfa_leakage_anti_aliasing(&in_cfa_leak_anti_alias, &out_cfa_leak_anti_alias);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_cfa_leakage_anti_aliasing", "OK case", id_cnt++);

        in_cfa_leak_anti_alias.p_anti_aliasing->enable = 3;

        Rval = ikc_cfa_leakage_anti_aliasing(&in_cfa_leak_anti_alias, &out_cfa_leak_anti_alias);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_cfa_leakage_anti_aliasing", "OK case", id_cnt++);

        in_cfa_leak_anti_alias.p_anti_aliasing->enable = 4;

        Rval = ikc_cfa_leakage_anti_aliasing(&in_cfa_leak_anti_alias, &out_cfa_leak_anti_alias);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_cfa_leakage_anti_aliasing", "OK case", id_cnt++);

        in_cfa_leak_anti_alias.p_anti_aliasing->enable = 5;

        Rval = ikc_cfa_leakage_anti_aliasing(&in_cfa_leak_anti_alias, &out_cfa_leak_anti_alias);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_cfa_leakage_anti_aliasing", "fail case", id_cnt++);

    }

    Rval = ikc_cfa_noise(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_cfa_noise", "null-ptr fail case", id_cnt++);

    Rval = ikc_after_ce_gain(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_after_ce_gain", "null-ptr fail case", id_cnt++);

    {
        ikc_in_rgb_ir_t in_rgb_ir;
        ikc_out_rgb_ir_t out_rgb_ir;

        in_rgb_ir.flip_mode = p_filters->input_param.flip_mode;
        (void)memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.rgb_ir, &p_filters->input_param.rgb_ir, sizeof(ik_rgb_ir_t));
        in_rgb_ir.p_rgb_ir = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.rgb_ir;
        in_rgb_ir.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
        in_rgb_ir.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
        (void)memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain, &p_filters->input_param.exp0_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
        in_rgb_ir.p_exp0_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain;
        out_rgb_ir.p_cr_4 = p_flow_tbl_list->p_CR_buf_4;
        out_rgb_ir.cr_4_size = CR_SIZE_4;
        out_rgb_ir.p_cr_6 = p_flow_tbl_list->p_CR_buf_6;
        out_rgb_ir.cr_6_size = CR_SIZE_6;
        out_rgb_ir.p_cr_7 = p_flow_tbl_list->p_CR_buf_7;
        out_rgb_ir.cr_7_size = CR_SIZE_7;
        out_rgb_ir.p_cr_8 = p_flow_tbl_list->p_CR_buf_8;
        out_rgb_ir.cr_8_size = CR_SIZE_8;
        out_rgb_ir.p_cr_9 = p_flow_tbl_list->p_CR_buf_9;
        out_rgb_ir.cr_9_size = CR_SIZE_9;
        out_rgb_ir.p_cr_11 = p_flow_tbl_list->p_CR_buf_11;
        out_rgb_ir.cr_11_size = CR_SIZE_11;
        out_rgb_ir.p_cr_12 = p_flow_tbl_list->p_CR_buf_12;
        out_rgb_ir.cr_12_size = CR_SIZE_12;
        out_rgb_ir.p_cr_21 = p_flow_tbl_list->p_CR_buf_21;
        out_rgb_ir.cr_21_size = CR_SIZE_21;
        out_rgb_ir.p_flow = &p_flow->flow_info;

        Rval = ikc_rgb_ir(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_rgb_ir", "null-ptr fail case", id_cnt++);

        in_rgb_ir.flip_mode = IK_FLIP_RAW_H;
        in_rgb_ir.p_rgb_ir->mode = 1;
        in_rgb_ir.p_before_ce_wb_gain->gain_r = 16777215;
        in_rgb_ir.p_before_ce_wb_gain->gain_g = 16777215;
        in_rgb_ir.p_before_ce_wb_gain->gain_b = 16777215;
        in_rgb_ir.p_after_ce_wb_gain->gain_r = 16777215;
        in_rgb_ir.p_after_ce_wb_gain->gain_g = 16777215;
        in_rgb_ir.p_after_ce_wb_gain->gain_b = 16777215;
        in_rgb_ir.p_exp0_frontend_wb_gain->ir_gain = 0;

        Rval = ikc_rgb_ir(&in_rgb_ir, &out_rgb_ir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_rgb_ir", "OK case", id_cnt++);

        in_rgb_ir.flip_mode = IK_FLIP_RAW_V;
        in_rgb_ir.p_rgb_ir->mode = 1;
        in_rgb_ir.p_exp0_frontend_wb_gain->r_gain = 4194303;
        in_rgb_ir.p_exp0_frontend_wb_gain->g_gain = 4194303;
        in_rgb_ir.p_exp0_frontend_wb_gain->b_gain = 4194303;
        in_rgb_ir.p_exp0_frontend_wb_gain->ir_gain = 1;

        Rval = ikc_rgb_ir(&in_rgb_ir, &out_rgb_ir);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_rgb_ir", "OK case", id_cnt++);

    }

    {
        ikc_in_rgb_ir_t in_rgb_ir;
        ikc_out_rgb_ir_sub_t out_rgb_ir_sub;

        in_rgb_ir.flip_mode = p_filters->input_param.flip_mode;
        (void)memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.rgb_ir, &p_filters->input_param.rgb_ir, sizeof(ik_rgb_ir_t));
        in_rgb_ir.p_rgb_ir = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.rgb_ir;
        in_rgb_ir.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
        in_rgb_ir.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
        (void)memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain, &p_filters->input_param.exp0_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
        in_rgb_ir.p_exp0_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain;
        out_rgb_ir_sub.p_cr_45 = p_flow_tbl_list->p_CR_buf_45;
        out_rgb_ir_sub.cr_45_size = CR_SIZE_45;
        out_rgb_ir_sub.p_cr_47 = p_flow_tbl_list->p_CR_buf_47;
        out_rgb_ir_sub.cr_47_size = CR_SIZE_47;
        out_rgb_ir_sub.p_cr_49 = p_flow_tbl_list->p_CR_buf_49;
        out_rgb_ir_sub.cr_49_size = CR_SIZE_49;
        out_rgb_ir_sub.p_cr_50 = p_flow_tbl_list->p_CR_buf_50;
        out_rgb_ir_sub.cr_50_size = CR_SIZE_50;

        Rval = ikc_rgb_ir_sub(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_rgb_ir_sub", "null-ptr fail case", id_cnt++);

        in_rgb_ir.flip_mode = 0;
        in_rgb_ir.p_rgb_ir->mode = 1;

        Rval = ikc_rgb_ir_sub(&in_rgb_ir, &out_rgb_ir_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_rgb_ir_sub", "OK case", id_cnt++);

        in_rgb_ir.flip_mode = IK_FLIP_RAW_H;
        in_rgb_ir.p_rgb_ir->mode = 1;

        Rval = ikc_rgb_ir_sub(&in_rgb_ir, &out_rgb_ir_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_rgb_ir_sub", "OK case", id_cnt++);

        in_rgb_ir.flip_mode = IK_FLIP_RAW_V;
        in_rgb_ir.p_rgb_ir->mode = 1;

        Rval = ikc_rgb_ir_sub(&in_rgb_ir, &out_rgb_ir_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitCfaFilters.c", "ikc_rgb_ir_sub", "OK case", id_cnt++);

    }
}

static void IK_TestCovr_IkcUnitMctfAlgo(void)
{
    uint32 Rval = IK_OK;
    uintptr addr = 0;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_filter_t *p_filters = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    idsp_ik_flow_ctrl_t *p_flow = NULL;
    (void)img_ctx_get_flow_tbl_list(0, 0, &addr);
    (void)memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
    (void)img_ctx_get_context(0, &p_ctx);
    p_filters = &p_ctx->filters;
    (void)img_ctx_get_flow_control(0, 0, &addr);
    (void)memcpy(&p_flow, &addr, sizeof(void *));

    {
        ikc_in_mctf_t in_mctf;
        ikc_out_mctf_t out_mctf;

        in_mctf.p_video_mctf = &p_filters->input_param.video_mctf;
        in_mctf.p_internal_video_mctf = &p_filters->input_param.internal_video_mctf;
        in_mctf.p_video_mctf_ta = &p_filters->input_param.video_mctf_ta;
        in_mctf.p_final_sharpen_both = &p_filters->input_param.final_sharpen_both;
        in_mctf.p_main = &p_filters->input_param.window_size_info.main_win;
        out_mctf.p_cr_111 = p_flow_tbl_list->p_CR_buf_111;
        out_mctf.cr_111_size = CR_SIZE_111;
        out_mctf.p_cr_112 = p_flow_tbl_list->p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_113 = p_flow_tbl_list->p_CR_buf_113;
        out_mctf.cr_113_size = CR_SIZE_113;
        out_mctf.p_cr_116 = p_flow_tbl_list->p_CR_buf_116;
        out_mctf.cr_116_size = CR_SIZE_116;
        out_mctf.p_flow_info = &p_flow->flow_info;
        out_mctf.p_phase = &p_flow->phase;

        Rval = ikc_mctf(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitMctfAlgo.c", "ikc_mctf", "null-ptr fail case", id_cnt++);

        {
            //vid8 mctf config.
            in_mctf.p_video_mctf->enable = 1;
            in_mctf.p_video_mctf->y_narrow_nonsmooth_detect_shift = 2;
            //in_mctf.p_video_mctf->y_narrow_nonsmooth_detect_sub[17];
            in_mctf.p_video_mctf->use_level_based_ta = 1;
            in_mctf.p_video_mctf->compression_enable = 1;
            //3d max change level
            in_mctf.p_video_mctf->y_3d_maxchange.low = 0;
            in_mctf.p_video_mctf->y_3d_maxchange.low_delta = 0;
            in_mctf.p_video_mctf->y_3d_maxchange.low_strength = 255;
            in_mctf.p_video_mctf->y_3d_maxchange.mid_strength = 255;
            in_mctf.p_video_mctf->y_3d_maxchange.high = 3;
            in_mctf.p_video_mctf->y_3d_maxchange.high_delta = 2;
            in_mctf.p_video_mctf->y_3d_maxchange.high_strength = 255;
            in_mctf.p_video_mctf->y_3d_maxchange.method = 0;

            in_mctf.p_video_mctf->cb_3d_maxchange.low = 0;
            in_mctf.p_video_mctf->cb_3d_maxchange.low_delta = 0;
            in_mctf.p_video_mctf->cb_3d_maxchange.low_strength = 255;
            in_mctf.p_video_mctf->cb_3d_maxchange.mid_strength = 255;
            in_mctf.p_video_mctf->cb_3d_maxchange.high = 3;
            in_mctf.p_video_mctf->cb_3d_maxchange.high_delta = 2;
            in_mctf.p_video_mctf->cb_3d_maxchange.high_strength = 255;
            in_mctf.p_video_mctf->cb_3d_maxchange.method = 0;

            in_mctf.p_video_mctf->cr_3d_maxchange.low = 0;
            in_mctf.p_video_mctf->cr_3d_maxchange.low_delta = 0;
            in_mctf.p_video_mctf->cr_3d_maxchange.low_strength = 255;
            in_mctf.p_video_mctf->cr_3d_maxchange.mid_strength = 255;
            in_mctf.p_video_mctf->cr_3d_maxchange.high = 3;
            in_mctf.p_video_mctf->cr_3d_maxchange.high_delta = 2;
            in_mctf.p_video_mctf->cr_3d_maxchange.high_strength = 255;
            in_mctf.p_video_mctf->cr_3d_maxchange.method = 0;

            //advanced iso
            in_mctf.p_video_mctf->y_advanced_iso_enable = 0;
            in_mctf.p_video_mctf->y_advanced_iso_max_change_method = 0;
            //in_mctf.p_video_mctf->y_advanced_iso_max_change_table[32];
            in_mctf.p_video_mctf->y_advanced_iso_noise_level = 0;
            in_mctf.p_video_mctf->y_advanced_iso_size = 1;
            in_mctf.p_video_mctf->cb_advanced_iso_enable = 0;
            in_mctf.p_video_mctf->cb_advanced_iso_max_change_method = 0;
            //in_mctf.p_video_mctf->cb_advanced_iso_max_change_table[32];
            in_mctf.p_video_mctf->cb_advanced_iso_noise_level = 0;
            in_mctf.p_video_mctf->cb_advanced_iso_size = 1;
            in_mctf.p_video_mctf->cr_advanced_iso_enable = 0;
            in_mctf.p_video_mctf->cr_advanced_iso_max_change_method = 0;
            //in_mctf.p_video_mctf->cr_advanced_iso_max_change_table[32];
            in_mctf.p_video_mctf->cr_advanced_iso_noise_level = 0;
            in_mctf.p_video_mctf->cr_advanced_iso_size = 1;
            //level based ta
            in_mctf.p_video_mctf->y_level_based_ta.low = 0;
            in_mctf.p_video_mctf->y_level_based_ta.low_delta = 0;
            in_mctf.p_video_mctf->y_level_based_ta.low_strength = 0;
            in_mctf.p_video_mctf->y_level_based_ta.mid_strength = 0;
            in_mctf.p_video_mctf->y_level_based_ta.high = 3;
            in_mctf.p_video_mctf->y_level_based_ta.high_delta = 2;
            in_mctf.p_video_mctf->y_level_based_ta.high_strength = 0;
            in_mctf.p_video_mctf->y_level_based_ta.method = 0;

            in_mctf.p_video_mctf->cb_level_based_ta.low = 0;
            in_mctf.p_video_mctf->cb_level_based_ta.low_delta = 0;
            in_mctf.p_video_mctf->cb_level_based_ta.low_strength = 0;
            in_mctf.p_video_mctf->cb_level_based_ta.mid_strength = 0;
            in_mctf.p_video_mctf->cb_level_based_ta.high = 3;
            in_mctf.p_video_mctf->cb_level_based_ta.high_delta = 2;
            in_mctf.p_video_mctf->cb_level_based_ta.high_strength = 0;
            in_mctf.p_video_mctf->cb_level_based_ta.method = 0;

            in_mctf.p_video_mctf->cr_level_based_ta.low = 0;
            in_mctf.p_video_mctf->cr_level_based_ta.low_delta = 0;
            in_mctf.p_video_mctf->cr_level_based_ta.low_strength = 0;
            in_mctf.p_video_mctf->cr_level_based_ta.mid_strength = 0;
            in_mctf.p_video_mctf->cr_level_based_ta.high = 3;
            in_mctf.p_video_mctf->cr_level_based_ta.high_delta = 2;
            in_mctf.p_video_mctf->cr_level_based_ta.high_strength = 0;
            in_mctf.p_video_mctf->cr_level_based_ta.method = 0;

            //overall max change level
            in_mctf.p_video_mctf->y_overall_max_change.low = 4;
            in_mctf.p_video_mctf->y_overall_max_change.low_delta = 3;
            in_mctf.p_video_mctf->y_overall_max_change.low_strength = 4;
            in_mctf.p_video_mctf->y_overall_max_change.mid_strength = 5;
            in_mctf.p_video_mctf->y_overall_max_change.high = 20;
            in_mctf.p_video_mctf->y_overall_max_change.high_delta = 2;
            in_mctf.p_video_mctf->y_overall_max_change.high_strength = 4;
            in_mctf.p_video_mctf->y_overall_max_change.method = 7;

            in_mctf.p_video_mctf->cb_overall_max_change.low = 0;
            in_mctf.p_video_mctf->cb_overall_max_change.low_delta = 1;
            in_mctf.p_video_mctf->cb_overall_max_change.low_strength = 4;
            in_mctf.p_video_mctf->cb_overall_max_change.mid_strength = 4;
            in_mctf.p_video_mctf->cb_overall_max_change.high = 20;
            in_mctf.p_video_mctf->cb_overall_max_change.high_delta = 2;
            in_mctf.p_video_mctf->cb_overall_max_change.high_strength = 4;
            in_mctf.p_video_mctf->cb_overall_max_change.method = 7;

            in_mctf.p_video_mctf->cr_overall_max_change.low = 0;
            in_mctf.p_video_mctf->cr_overall_max_change.low_delta = 1;
            in_mctf.p_video_mctf->cr_overall_max_change.low_strength = 3;
            in_mctf.p_video_mctf->cr_overall_max_change.mid_strength = 3;
            in_mctf.p_video_mctf->cr_overall_max_change.high = 20;
            in_mctf.p_video_mctf->cr_overall_max_change.high_delta = 2;
            in_mctf.p_video_mctf->cr_overall_max_change.high_strength = 3;
            in_mctf.p_video_mctf->cr_overall_max_change.method = 7;

            //spat_blend level
            in_mctf.p_video_mctf->y_spat_blend.low = 0;
            in_mctf.p_video_mctf->y_spat_blend.low_delta = 3;
            in_mctf.p_video_mctf->y_spat_blend.low_strength = 0;
            in_mctf.p_video_mctf->y_spat_blend.mid_strength = 30;
            in_mctf.p_video_mctf->y_spat_blend.high = 200;
            in_mctf.p_video_mctf->y_spat_blend.high_delta = 4;
            in_mctf.p_video_mctf->y_spat_blend.high_strength = 128;
            in_mctf.p_video_mctf->y_spat_blend.method = 2;

            in_mctf.p_video_mctf->cb_spat_blend.low = 0;
            in_mctf.p_video_mctf->cb_spat_blend.low_delta = 0;
            in_mctf.p_video_mctf->cb_spat_blend.low_strength = 0;
            in_mctf.p_video_mctf->cb_spat_blend.mid_strength = 0;
            in_mctf.p_video_mctf->cb_spat_blend.high = 20;
            in_mctf.p_video_mctf->cb_spat_blend.high_delta = 2;
            in_mctf.p_video_mctf->cb_spat_blend.high_strength = 0;
            in_mctf.p_video_mctf->cb_spat_blend.method = 0;

            in_mctf.p_video_mctf->cr_spat_blend.low = 0;
            in_mctf.p_video_mctf->cr_spat_blend.low_delta = 0;
            in_mctf.p_video_mctf->cr_spat_blend.low_strength = 0;
            in_mctf.p_video_mctf->cr_spat_blend.mid_strength = 0;
            in_mctf.p_video_mctf->cr_spat_blend.high = 20;
            in_mctf.p_video_mctf->cr_spat_blend.high_delta = 2;
            in_mctf.p_video_mctf->cr_spat_blend.high_strength = 0;
            in_mctf.p_video_mctf->cr_spat_blend.method = 0;

            //spat filt max smth change level
            in_mctf.p_video_mctf->y_spat_filt_max_smth_change.low = 0;
            in_mctf.p_video_mctf->y_spat_filt_max_smth_change.low_delta = 0;
            in_mctf.p_video_mctf->y_spat_filt_max_smth_change.low_strength = 255;
            in_mctf.p_video_mctf->y_spat_filt_max_smth_change.mid_strength = 255;
            in_mctf.p_video_mctf->y_spat_filt_max_smth_change.high = 3;
            in_mctf.p_video_mctf->y_spat_filt_max_smth_change.high_delta = 2;
            in_mctf.p_video_mctf->y_spat_filt_max_smth_change.high_strength = 255;
            in_mctf.p_video_mctf->y_spat_filt_max_smth_change.method = 0;

            in_mctf.p_video_mctf->cb_spat_filt_max_smth_change.low = 0;
            in_mctf.p_video_mctf->cb_spat_filt_max_smth_change.low_delta = 0;
            in_mctf.p_video_mctf->cb_spat_filt_max_smth_change.low_strength = 255;
            in_mctf.p_video_mctf->cb_spat_filt_max_smth_change.mid_strength = 255;
            in_mctf.p_video_mctf->cb_spat_filt_max_smth_change.high = 3;
            in_mctf.p_video_mctf->cb_spat_filt_max_smth_change.high_delta = 2;
            in_mctf.p_video_mctf->cb_spat_filt_max_smth_change.high_strength = 255;
            in_mctf.p_video_mctf->cb_spat_filt_max_smth_change.method = 0;

            in_mctf.p_video_mctf->cr_spat_filt_max_smth_change.low = 0;
            in_mctf.p_video_mctf->cr_spat_filt_max_smth_change.low_delta = 0;
            in_mctf.p_video_mctf->cr_spat_filt_max_smth_change.low_strength = 255;
            in_mctf.p_video_mctf->cr_spat_filt_max_smth_change.mid_strength = 255;
            in_mctf.p_video_mctf->cr_spat_filt_max_smth_change.high = 3;
            in_mctf.p_video_mctf->cr_spat_filt_max_smth_change.high_delta = 2;
            in_mctf.p_video_mctf->cr_spat_filt_max_smth_change.high_strength = 255;
            in_mctf.p_video_mctf->cr_spat_filt_max_smth_change.method = 0;

            /*spat_smth_dir*/
            in_mctf.p_video_mctf->y_spat_smth_wide_edge_detect = 0;
            in_mctf.p_video_mctf->y_spat_smth_edge_thresh = 0;
            in_mctf.p_video_mctf->y_spat_smth_dir.low = 40;
            in_mctf.p_video_mctf->y_spat_smth_dir.low_delta = 5;
            in_mctf.p_video_mctf->y_spat_smth_dir.low_strength = 35;
            in_mctf.p_video_mctf->y_spat_smth_dir.mid_strength = 35;
            in_mctf.p_video_mctf->y_spat_smth_dir.high = 137;
            in_mctf.p_video_mctf->y_spat_smth_dir.high_delta = 5;
            in_mctf.p_video_mctf->y_spat_smth_dir.high_strength = 35;
            in_mctf.p_video_mctf->y_spat_smth_dir.method = 0;

            in_mctf.p_video_mctf->cb_spat_smth_wide_edge_detect = 0;
            in_mctf.p_video_mctf->cb_spat_smth_edge_thresh = 0;
            in_mctf.p_video_mctf->cb_spat_smth_dir.low = 40;
            in_mctf.p_video_mctf->cb_spat_smth_dir.low_delta = 5;
            in_mctf.p_video_mctf->cb_spat_smth_dir.low_strength = 79;
            in_mctf.p_video_mctf->cb_spat_smth_dir.mid_strength = 79;
            in_mctf.p_video_mctf->cb_spat_smth_dir.high = 137;
            in_mctf.p_video_mctf->cb_spat_smth_dir.high_delta = 5;
            in_mctf.p_video_mctf->cb_spat_smth_dir.high_strength = 79;
            in_mctf.p_video_mctf->cb_spat_smth_dir.method = 0;

            in_mctf.p_video_mctf->cr_spat_smth_wide_edge_detect = 0;
            in_mctf.p_video_mctf->cr_spat_smth_edge_thresh = 0;
            in_mctf.p_video_mctf->cr_spat_smth_dir.low = 40;
            in_mctf.p_video_mctf->cr_spat_smth_dir.low_delta = 5;
            in_mctf.p_video_mctf->cr_spat_smth_dir.low_strength = 38;
            in_mctf.p_video_mctf->cr_spat_smth_dir.mid_strength = 38;
            in_mctf.p_video_mctf->cr_spat_smth_dir.high = 137;
            in_mctf.p_video_mctf->cr_spat_smth_dir.high_delta = 5;
            in_mctf.p_video_mctf->cr_spat_smth_dir.high_strength = 38;
            in_mctf.p_video_mctf->cr_spat_smth_dir.method = 0;

            /*spat_smth_iso*/
            in_mctf.p_video_mctf->y_spat_smth_iso.low = 40;
            in_mctf.p_video_mctf->y_spat_smth_iso.low_delta = 5;
            in_mctf.p_video_mctf->y_spat_smth_iso.low_strength = 0;
            in_mctf.p_video_mctf->y_spat_smth_iso.mid_strength = 0;
            in_mctf.p_video_mctf->y_spat_smth_iso.high = 137;
            in_mctf.p_video_mctf->y_spat_smth_iso.high_delta = 5;
            in_mctf.p_video_mctf->y_spat_smth_iso.high_strength = 0;
            in_mctf.p_video_mctf->y_spat_smth_iso.method = 0;

            in_mctf.p_video_mctf->cb_spat_smth_iso.low = 40;
            in_mctf.p_video_mctf->cb_spat_smth_iso.low_delta = 5;
            in_mctf.p_video_mctf->cb_spat_smth_iso.low_strength = 79;
            in_mctf.p_video_mctf->cb_spat_smth_iso.mid_strength = 79;
            in_mctf.p_video_mctf->cb_spat_smth_iso.high = 137;
            in_mctf.p_video_mctf->cb_spat_smth_iso.high_delta = 5;
            in_mctf.p_video_mctf->cb_spat_smth_iso.high_strength = 79;
            in_mctf.p_video_mctf->cb_spat_smth_iso.method = 0;

            in_mctf.p_video_mctf->cr_spat_smth_iso.low = 40;
            in_mctf.p_video_mctf->cr_spat_smth_iso.low_delta = 5;
            in_mctf.p_video_mctf->cr_spat_smth_iso.low_strength = 38;
            in_mctf.p_video_mctf->cr_spat_smth_iso.mid_strength = 38;
            in_mctf.p_video_mctf->cr_spat_smth_iso.high = 137;
            in_mctf.p_video_mctf->cr_spat_smth_iso.high_delta = 5;
            in_mctf.p_video_mctf->cr_spat_smth_iso.high_strength = 38;
            in_mctf.p_video_mctf->cr_spat_smth_iso.method = 0;

            in_mctf.p_video_mctf->y_spat_smth_direct_decide_t0 = 0;
            in_mctf.p_video_mctf->y_spat_smth_direct_decide_t1 = 0;
            in_mctf.p_video_mctf->cb_spat_smth_direct_decide_t0 = 0;
            in_mctf.p_video_mctf->cb_spat_smth_direct_decide_t1 = 0;
            in_mctf.p_video_mctf->cr_spat_smth_direct_decide_t0 = 0;
            in_mctf.p_video_mctf->cr_spat_smth_direct_decide_t1 = 0;
            //spatial max temporal level
            in_mctf.p_video_mctf->y_spatial_max_change = 0;
            in_mctf.p_video_mctf->y_spatial_max_temporal.low = 0;
            in_mctf.p_video_mctf->y_spatial_max_temporal.low_delta = 0;
            in_mctf.p_video_mctf->y_spatial_max_temporal.low_strength = 255;
            in_mctf.p_video_mctf->y_spatial_max_temporal.mid_strength = 255;
            in_mctf.p_video_mctf->y_spatial_max_temporal.high = 3;
            in_mctf.p_video_mctf->y_spatial_max_temporal.high_delta = 2;
            in_mctf.p_video_mctf->y_spatial_max_temporal.high_strength = 255;
            in_mctf.p_video_mctf->y_spatial_max_temporal.method = 0;

            in_mctf.p_video_mctf->cb_spatial_max_change = 0;
            in_mctf.p_video_mctf->cb_spatial_max_temporal.low = 0;
            in_mctf.p_video_mctf->cb_spatial_max_temporal.low_delta = 0;
            in_mctf.p_video_mctf->cb_spatial_max_temporal.low_strength = 255;
            in_mctf.p_video_mctf->cb_spatial_max_temporal.mid_strength = 255;
            in_mctf.p_video_mctf->cb_spatial_max_temporal.high = 3;
            in_mctf.p_video_mctf->cb_spatial_max_temporal.high_delta = 2;
            in_mctf.p_video_mctf->cb_spatial_max_temporal.high_strength = 255;
            in_mctf.p_video_mctf->cb_spatial_max_temporal.method = 0;

            in_mctf.p_video_mctf->cr_spatial_max_change = 0;
            in_mctf.p_video_mctf->cr_spatial_max_temporal.low = 0;
            in_mctf.p_video_mctf->cr_spatial_max_temporal.low_delta = 0;
            in_mctf.p_video_mctf->cr_spatial_max_temporal.low_strength = 255;
            in_mctf.p_video_mctf->cr_spatial_max_temporal.mid_strength = 255;
            in_mctf.p_video_mctf->cr_spatial_max_temporal.high = 3;
            in_mctf.p_video_mctf->cr_spatial_max_temporal.high_delta = 2;
            in_mctf.p_video_mctf->cr_spatial_max_temporal.high_strength = 255;
            in_mctf.p_video_mctf->cr_spatial_max_temporal.method = 0;

            in_mctf.p_video_mctf->y_curve.temporal_alpha0 = 70;
            in_mctf.p_video_mctf->y_curve.temporal_alpha1 = 70;
            in_mctf.p_video_mctf->y_curve.temporal_alpha2 = 159;
            in_mctf.p_video_mctf->y_curve.temporal_alpha3 = 224;
            in_mctf.p_video_mctf->y_curve.temporal_t0 = 6;
            in_mctf.p_video_mctf->y_curve.temporal_t1 = 7;
            in_mctf.p_video_mctf->y_curve.temporal_t2 = 10;
            in_mctf.p_video_mctf->y_curve.temporal_t3 = 13;

            in_mctf.p_video_mctf->y_temporal_max_change = 64;
            in_mctf.p_video_mctf->y_temporal_min_target.low = 0;
            in_mctf.p_video_mctf->y_temporal_min_target.low_delta = 0;
            in_mctf.p_video_mctf->y_temporal_min_target.low_strength = 0;
            in_mctf.p_video_mctf->y_temporal_min_target.mid_strength = 0;
            in_mctf.p_video_mctf->y_temporal_min_target.high = 3;
            in_mctf.p_video_mctf->y_temporal_min_target.high_delta = 2;
            in_mctf.p_video_mctf->y_temporal_min_target.high_strength = 0;
            in_mctf.p_video_mctf->y_temporal_min_target.method = 0;

            in_mctf.p_video_mctf->cb_curve.temporal_alpha0 = 100;
            in_mctf.p_video_mctf->cb_curve.temporal_alpha1 = 100;
            in_mctf.p_video_mctf->cb_curve.temporal_alpha2 = 159;
            in_mctf.p_video_mctf->cb_curve.temporal_alpha3 = 224;
            in_mctf.p_video_mctf->cb_curve.temporal_t0 = 14;
            in_mctf.p_video_mctf->cb_curve.temporal_t1 = 16;
            in_mctf.p_video_mctf->cb_curve.temporal_t2 = 20;
            in_mctf.p_video_mctf->cb_curve.temporal_t3 = 26;

            in_mctf.p_video_mctf->cb_temporal_max_change = 22;
            in_mctf.p_video_mctf->cb_temporal_min_target.low = 0;
            in_mctf.p_video_mctf->cb_temporal_min_target.low_delta = 0;
            in_mctf.p_video_mctf->cb_temporal_min_target.low_strength = 0;
            in_mctf.p_video_mctf->cb_temporal_min_target.mid_strength = 0;
            in_mctf.p_video_mctf->cb_temporal_min_target.high = 3;
            in_mctf.p_video_mctf->cb_temporal_min_target.high_delta = 2;
            in_mctf.p_video_mctf->cb_temporal_min_target.high_strength = 0;
            in_mctf.p_video_mctf->cb_temporal_min_target.method = 0;

            in_mctf.p_video_mctf->cr_curve.temporal_alpha0 = 100;
            in_mctf.p_video_mctf->cr_curve.temporal_alpha1 = 100;
            in_mctf.p_video_mctf->cr_curve.temporal_alpha2 = 159;
            in_mctf.p_video_mctf->cr_curve.temporal_alpha3 = 224;
            in_mctf.p_video_mctf->cr_curve.temporal_t0 = 14;
            in_mctf.p_video_mctf->cr_curve.temporal_t1 = 16;
            in_mctf.p_video_mctf->cr_curve.temporal_t2 = 20;
            in_mctf.p_video_mctf->cr_curve.temporal_t3 = 26;

            in_mctf.p_video_mctf->cr_temporal_max_change = 22;
            in_mctf.p_video_mctf->cr_temporal_min_target.low = 0;
            in_mctf.p_video_mctf->cr_temporal_min_target.low_delta = 0;
            in_mctf.p_video_mctf->cr_temporal_min_target.low_strength = 0;
            in_mctf.p_video_mctf->cr_temporal_min_target.mid_strength = 0;
            in_mctf.p_video_mctf->cr_temporal_min_target.high = 3;
            in_mctf.p_video_mctf->cr_temporal_min_target.high_delta = 2;
            in_mctf.p_video_mctf->cr_temporal_min_target.high_strength = 0;
            in_mctf.p_video_mctf->cr_temporal_min_target.method = 0;

            in_mctf.p_video_mctf->y_temporal_ghost_prevent = 1;
            in_mctf.p_video_mctf->cb_temporal_ghost_prevent = 0;
            in_mctf.p_video_mctf->cr_temporal_ghost_prevent = 0;
            in_mctf.p_video_mctf->compression_bit_rate_luma = 32;
            in_mctf.p_video_mctf->compression_bit_rate_chroma = 20;
            in_mctf.p_video_mctf->compression_dither_disable = 0;
            in_mctf.p_video_mctf->y_temporal_artifact_guard = 0;
            in_mctf.p_video_mctf->cb_temporal_artifact_guard = 0;
            in_mctf.p_video_mctf->cr_temporal_artifact_guard = 0;
            in_mctf.p_video_mctf->y_combine_strength = 0;
            in_mctf.p_video_mctf->y_strength_3d = 12;
            //in_mctf.p_video_mctf->y_spatial_weighting[64]; // 0:255
            in_mctf.p_video_mctf->cb_strength_3d = 0;
            //in_mctf.p_video_mctf->cb_spatial_weighting[64]; // 0:255
            in_mctf.p_video_mctf->cr_strength_3d = 0;
            //in_mctf.p_video_mctf->cr_spatial_weighting[64]; // 0:255

            // video_mctf_level
            in_mctf.p_video_mctf->y_temporal_max_change_not_t0_t1_level_based = 0;
            in_mctf.p_video_mctf->y_temporal_either_max_change_or_t0_t1_add.low = 0;
            in_mctf.p_video_mctf->y_temporal_either_max_change_or_t0_t1_add.low_delta = 0;
            in_mctf.p_video_mctf->y_temporal_either_max_change_or_t0_t1_add.low_strength = 64;
            in_mctf.p_video_mctf->y_temporal_either_max_change_or_t0_t1_add.mid_strength = 16;
            in_mctf.p_video_mctf->y_temporal_either_max_change_or_t0_t1_add.high = 6;
            in_mctf.p_video_mctf->y_temporal_either_max_change_or_t0_t1_add.high_delta = 4;
            in_mctf.p_video_mctf->y_temporal_either_max_change_or_t0_t1_add.high_strength = 0;
            in_mctf.p_video_mctf->y_temporal_either_max_change_or_t0_t1_add.method = 7;

            in_mctf.p_video_mctf->cb_temporal_max_change_not_t0_t1_level_based = 0;
            in_mctf.p_video_mctf->cb_temporal_either_max_change_or_t0_t1_add.low = 30;
            in_mctf.p_video_mctf->cb_temporal_either_max_change_or_t0_t1_add.low_delta = 5;
            in_mctf.p_video_mctf->cb_temporal_either_max_change_or_t0_t1_add.low_strength = 0;
            in_mctf.p_video_mctf->cb_temporal_either_max_change_or_t0_t1_add.mid_strength = 0;
            in_mctf.p_video_mctf->cb_temporal_either_max_change_or_t0_t1_add.high = 160;
            in_mctf.p_video_mctf->cb_temporal_either_max_change_or_t0_t1_add.high_delta = 5;
            in_mctf.p_video_mctf->cb_temporal_either_max_change_or_t0_t1_add.high_strength = 0;
            in_mctf.p_video_mctf->cb_temporal_either_max_change_or_t0_t1_add.method = 0;

            in_mctf.p_video_mctf->cr_temporal_max_change_not_t0_t1_level_based = 0;
            in_mctf.p_video_mctf->cr_temporal_either_max_change_or_t0_t1_add.low = 30;
            in_mctf.p_video_mctf->cr_temporal_either_max_change_or_t0_t1_add.low_delta = 5;
            in_mctf.p_video_mctf->cr_temporal_either_max_change_or_t0_t1_add.low_strength = 0;
            in_mctf.p_video_mctf->cr_temporal_either_max_change_or_t0_t1_add.mid_strength = 0;
            in_mctf.p_video_mctf->cr_temporal_either_max_change_or_t0_t1_add.high = 160;
            in_mctf.p_video_mctf->cr_temporal_either_max_change_or_t0_t1_add.high_delta = 5;
            in_mctf.p_video_mctf->cr_temporal_either_max_change_or_t0_t1_add.high_strength = 0;
            in_mctf.p_video_mctf->cr_temporal_either_max_change_or_t0_t1_add.method = 0;

            in_mctf.p_video_mctf_ta->slow_mo_sensitivity = 9;
            in_mctf.p_video_mctf_ta->score_noise_robust = 0;
            in_mctf.p_video_mctf_ta->lev_adjust_low = 45;
            in_mctf.p_video_mctf_ta->lev_adjust_low_delta = 3;
            in_mctf.p_video_mctf_ta->lev_adjust_low_strength = 207;
            in_mctf.p_video_mctf_ta->lev_adjust_mid_strength = 120;
            in_mctf.p_video_mctf_ta->lev_adjust_high = 65;
            in_mctf.p_video_mctf_ta->lev_adjust_high_delta = 2;
            in_mctf.p_video_mctf_ta->lev_adjust_high_strength = 130;

            in_mctf.p_video_mctf_ta->y_mctf_ta.max = 15;
            in_mctf.p_video_mctf_ta->y_mctf_ta.min = 0;
            in_mctf.p_video_mctf_ta->y_mctf_ta.motion_response = 200;
            in_mctf.p_video_mctf_ta->y_mctf_ta.noise_base = 14;
            in_mctf.p_video_mctf_ta->y_mctf_ta.still_thresh = 14;

            in_mctf.p_video_mctf_ta->cb_mctf_ta.max = 15;
            in_mctf.p_video_mctf_ta->cb_mctf_ta.min = 3;
            in_mctf.p_video_mctf_ta->cb_mctf_ta.motion_response = 200;
            in_mctf.p_video_mctf_ta->cb_mctf_ta.noise_base = 20;
            in_mctf.p_video_mctf_ta->cb_mctf_ta.still_thresh = 20;

            in_mctf.p_video_mctf_ta->cr_mctf_ta.max = 15;
            in_mctf.p_video_mctf_ta->cr_mctf_ta.min = 3;
            in_mctf.p_video_mctf_ta->cr_mctf_ta.motion_response = 200;
            in_mctf.p_video_mctf_ta->cr_mctf_ta.noise_base = 20;
            in_mctf.p_video_mctf_ta->cr_mctf_ta.still_thresh = 20;

            in_mctf.p_final_sharpen_both->enable = 1;
            in_mctf.p_final_sharpen_both->mode = 2;
            in_mctf.p_final_sharpen_both->edge_thresh = 75;
            in_mctf.p_final_sharpen_both->wide_edge_detect = 1;
            in_mctf.p_final_sharpen_both->max_change.up5x5 = 20;
            in_mctf.p_final_sharpen_both->max_change.down5x5 = 10;
            in_mctf.p_final_sharpen_both->max_change.up = 255;
            in_mctf.p_final_sharpen_both->max_change.down = 255;

            //in_mctf.p_final_sharpen_both->narrow_nonsmooth_detect_sub[17];
            in_mctf.p_final_sharpen_both->narrow_nonsmooth_detect_shift = 3;

            in_mctf.p_main->width = 1920;
            in_mctf.p_main->height = 1080;
        }

        in_mctf.p_video_mctf->compression_dither_disable = 1;
        in_mctf.p_video_mctf->y_temporal_max_change_not_t0_t1_level_based = 1;
        in_mctf.p_video_mctf->cb_temporal_max_change_not_t0_t1_level_based = 1;
        in_mctf.p_video_mctf->cr_temporal_max_change_not_t0_t1_level_based = 1;
        in_mctf.p_video_mctf->y_temporal_artifact_guard = 1;
        in_mctf.p_video_mctf->cb_temporal_artifact_guard = 1;
        in_mctf.p_video_mctf->cr_temporal_artifact_guard = 1;
        in_mctf.p_video_mctf->y_advanced_iso_enable = 1;
        in_mctf.p_video_mctf->cb_advanced_iso_enable = 1;
        in_mctf.p_video_mctf->cr_advanced_iso_enable = 1;
        in_mctf.p_video_mctf->y_strength_3d = 257;
        in_mctf.p_internal_video_mctf->ta_frames_combine_num1 = 1;

        Rval = ikc_mctf(&in_mctf, &out_mctf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitMctfAlgo.c", "ikc_mctf", "OK case", id_cnt++);

    }

    Rval = ikc_mctf_pos_dep33(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitMctfAlgo.c", "ikc_mctf_pos_dep33", "null-ptr fail case", id_cnt++);

}

static void IK_TestCovr_IkcUnitSharpen(void)
{
    uint32 Rval = IK_OK;
    uintptr addr = 0;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_filter_t *p_filters = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    idsp_ik_flow_ctrl_t *p_flow = NULL;
    ikc_out_asf_t out_asf;
    ikc_out_sharpen_t out_sharpen;
    ikc_out_final_sharpen_t out_fnl_shp;
    (void)img_ctx_get_flow_tbl_list(0, 0, &addr);
    (void)memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
    (void)img_ctx_get_context(0, &p_ctx);
    p_filters = &p_ctx->filters;
    (void)img_ctx_get_flow_control(0, 0, &addr);
    (void)memcpy(&p_flow, &addr, sizeof(void *));

    out_asf.p_cr_30 = p_flow_tbl_list->p_CR_buf_30;
    out_asf.cr_30_size = CR_SIZE_30;

    out_sharpen.p_cr_30 = p_flow_tbl_list->p_CR_buf_30;
    out_sharpen.cr_30_size = CR_SIZE_30;

    out_fnl_shp.p_cr_117 = p_flow_tbl_list->p_CR_buf_117;
    out_fnl_shp.cr_117_size = CR_SIZE_117;

    {
        ikc_in_asf_t in_asf;
        in_asf.p_advance_spatial_filter = &p_filters->input_param.advance_spatial_filter;
        in_asf.first_luma_processing_mode_updated = (uint32)p_filters->update_flags.iso.first_luma_processing_mode_updated;

        Rval = ikc_asf(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_asf", "null-ptr fail case", id_cnt++);

        {
            //vid16 config
            in_asf.first_luma_processing_mode_updated = 1;
            in_asf.p_advance_spatial_filter->enable = 1;

            in_asf.p_advance_spatial_filter->fir.specify = 2;
            //in_asf.p_advance_spatial_filter->fir.per_dir_fir_iso_strengths[9];
            //in_asf.p_advance_spatial_filter->fir.per_dir_fir_dir_strengths[9];
            //in_asf.p_advance_spatial_filter->fir.per_dir_fir_dir_amounts[9];
            //in_asf.p_advance_spatial_filter->fir.coefs[9][25];
            in_asf.p_advance_spatial_filter->fir.strength_iso = 128;
            in_asf.p_advance_spatial_filter->fir.strength_dir = 168;
            in_asf.p_advance_spatial_filter->fir.wide_edge_detect = 0;

            in_asf.p_advance_spatial_filter->directional_decide_t0 = 5;
            in_asf.p_advance_spatial_filter->directional_decide_t1 = 10;
            in_asf.p_advance_spatial_filter->adapt.alpha_min_up = 0;
            in_asf.p_advance_spatial_filter->adapt.alpha_max_up = 4;
            in_asf.p_advance_spatial_filter->adapt.t0_up = 2;
            in_asf.p_advance_spatial_filter->adapt.t1_up = 4;
            in_asf.p_advance_spatial_filter->adapt.alpha_min_down = 0;
            in_asf.p_advance_spatial_filter->adapt.alpha_max_down = 4;
            in_asf.p_advance_spatial_filter->adapt.t0_down = 2;
            in_asf.p_advance_spatial_filter->adapt.t1_down = 4;

            in_asf.p_advance_spatial_filter->level_str_adjust.low = 40;
            in_asf.p_advance_spatial_filter->level_str_adjust.low_delta = 3;
            in_asf.p_advance_spatial_filter->level_str_adjust.low_strength = 16;
            in_asf.p_advance_spatial_filter->level_str_adjust.mid_strength = 16;
            in_asf.p_advance_spatial_filter->level_str_adjust.high = 160;
            in_asf.p_advance_spatial_filter->level_str_adjust.high_delta = 3;
            in_asf.p_advance_spatial_filter->level_str_adjust.high_strength = 16;

            in_asf.p_advance_spatial_filter->t0_t1_div.low = 40;
            in_asf.p_advance_spatial_filter->t0_t1_div.low_delta = 3;
            in_asf.p_advance_spatial_filter->t0_t1_div.low_strength = 16;
            in_asf.p_advance_spatial_filter->t0_t1_div.mid_strength = 16;
            in_asf.p_advance_spatial_filter->t0_t1_div.high = 160;
            in_asf.p_advance_spatial_filter->t0_t1_div.high_delta = 3;
            in_asf.p_advance_spatial_filter->t0_t1_div.high_strength = 16;

            in_asf.p_advance_spatial_filter->max_change_not_t0_t1_alpha = 1;
            in_asf.p_advance_spatial_filter->max_change_up = 26;
            in_asf.p_advance_spatial_filter->max_change_down = 26;
        }

        in_asf.p_advance_spatial_filter->fir.specify = 0;
        in_asf.p_advance_spatial_filter->max_change_up = 0;
        in_asf.p_advance_spatial_filter->max_change_down = 0;

        Rval = ikc_asf(&in_asf, &out_asf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_asf", "OK case", id_cnt++);

        in_asf.p_advance_spatial_filter->fir.specify = 1;
        in_asf.first_luma_processing_mode_updated = 0;

        Rval = ikc_asf(&in_asf, &out_asf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_asf", "OK case", id_cnt++);

        in_asf.p_advance_spatial_filter->fir.specify = 3;

        Rval = ikc_asf(&in_asf, &out_asf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_asf", "OK case", id_cnt++);

        in_asf.p_advance_spatial_filter->fir.specify = 4;

        Rval = ikc_asf(&in_asf, &out_asf);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_asf", "OK case", id_cnt++);

    }
    {
        ikc_in_sharpen_t in_sharpen;

        in_sharpen.sharpen_mode = p_filters->input_param.first_sharpen_both.mode;
        in_sharpen.p_first_sharpen_noise = &p_filters->input_param.first_sharpen_noise;
        in_sharpen.p_first_sharpen_fir = &p_filters->input_param.first_sharpen_fir;
        in_sharpen.p_working_buffer = &p_filters->input_param.ctx_buf.first_sharpen_working_buffer[0];
        in_sharpen.working_buffer_size = sizeof(p_filters->input_param.ctx_buf.first_sharpen_working_buffer);

        Rval = ikc_sharpen(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_sharpen", "null-ptr fail case", id_cnt++);

        {
            //vid8 config
            in_sharpen.sharpen_mode = 2;
            in_sharpen.p_first_sharpen_noise->max_change_up = 20;
            in_sharpen.p_first_sharpen_noise->max_change_down = 20;

            in_sharpen.p_first_sharpen_noise->spatial_fir.specify = 2;
            //in_sharpen.p_first_sharpen_noise->spatial_fir.per_dir_fir_iso_strengths[9];
            //in_sharpen.p_first_sharpen_noise->spatial_fir.per_dir_fir_dir_strengths[9];
            //in_sharpen.p_first_sharpen_noise->spatial_fir.per_dir_fir_dir_amounts[9];
            //in_sharpen.p_first_sharpen_noise->spatial_fir.coefs[9][25];
            in_sharpen.p_first_sharpen_noise->spatial_fir.strength_iso = 0;
            in_sharpen.p_first_sharpen_noise->spatial_fir.strength_dir = 0;

            in_sharpen.p_first_sharpen_noise->level_str_adjust.low = 20;
            in_sharpen.p_first_sharpen_noise->level_str_adjust.low_delta = 4;
            in_sharpen.p_first_sharpen_noise->level_str_adjust.low_strength = 8;
            in_sharpen.p_first_sharpen_noise->level_str_adjust.mid_strength = 8;
            in_sharpen.p_first_sharpen_noise->level_str_adjust.high = 128;
            in_sharpen.p_first_sharpen_noise->level_str_adjust.high_delta = 5;
            in_sharpen.p_first_sharpen_noise->level_str_adjust.high_strength = 8;

            in_sharpen.p_first_sharpen_noise->level_str_adjust_not_t0_t1_level_based = 1;
            in_sharpen.p_first_sharpen_noise->t0 = 0;
            in_sharpen.p_first_sharpen_noise->t1 = 0;
            in_sharpen.p_first_sharpen_noise->alpha_min = 0;
            in_sharpen.p_first_sharpen_noise->alpha_max = 0;

            in_sharpen.p_first_sharpen_fir->specify = 4;
            //in_sharpen.p_first_sharpen_fir->per_dir_fir_iso_strengths[9];
            //in_sharpen.p_first_sharpen_fir->per_dir_fir_dir_strengths[9];
            //in_sharpen.p_first_sharpen_fir->per_dir_fir_dir_amounts[9];
            //in_sharpen.p_first_sharpen_fir->coefs[9][25];
            in_sharpen.p_first_sharpen_fir->strength_iso = 0;
            in_sharpen.p_first_sharpen_fir->strength_dir = 192;
        }

        in_sharpen.sharpen_mode = 0;
        in_sharpen.p_first_sharpen_fir->specify = 0;
        in_sharpen.p_first_sharpen_noise->spatial_fir.specify = 0;

        Rval = ikc_sharpen(&in_sharpen, &out_sharpen);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_sharpen", "OK case", id_cnt++);

        in_sharpen.sharpen_mode = 2;
        in_sharpen.p_first_sharpen_noise->level_str_adjust_not_t0_t1_level_based = 0;
        in_sharpen.p_first_sharpen_fir->specify = 1;
        in_sharpen.p_first_sharpen_noise->spatial_fir.specify = 1;

        Rval = ikc_sharpen(&in_sharpen, &out_sharpen);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_sharpen", "OK case", id_cnt++);

        in_sharpen.p_first_sharpen_fir->specify = 3;
        in_sharpen.p_first_sharpen_noise->spatial_fir.specify = 3;

        Rval = ikc_sharpen(&in_sharpen, &out_sharpen);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_sharpen", "OK case", id_cnt++);

        in_sharpen.p_first_sharpen_fir->specify = 2;
        in_sharpen.p_first_sharpen_noise->spatial_fir.specify = 4;

        Rval = ikc_sharpen(&in_sharpen, &out_sharpen);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_sharpen", "OK case", id_cnt++);

    }

    {
        ikc_in_sharpen_both_t in_sharpen_both;
        in_sharpen_both.p_first_sharpen_both = &p_filters->input_param.first_sharpen_both;

        Rval = ikc_sharpen_both(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_sharpen_both", "null-ptr fail case", id_cnt++);

        {
            //vid8 config
            in_sharpen_both.p_first_sharpen_both->enable = 1;
            in_sharpen_both.p_first_sharpen_both->mode = 2;
            in_sharpen_both.p_first_sharpen_both->edge_thresh = 38;
            in_sharpen_both.p_first_sharpen_both->wide_edge_detect = 0;
            in_sharpen_both.p_first_sharpen_both->max_change_up5x5 = 45;
            in_sharpen_both.p_first_sharpen_both->max_change_down5x5 = 50;
        }

        in_sharpen_both.p_first_sharpen_both->enable = 0;
        in_sharpen_both.p_first_sharpen_both->mode = 0;

        Rval = ikc_sharpen_both(&in_sharpen_both, &out_sharpen);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_sharpen_both", "OK case", id_cnt++);

    }

    Rval = ikc_sharpen_coring(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_sharpen_coring", "null-ptr fail case", id_cnt++);

    Rval = ikc_sharpen_coring_idx_scale(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_sharpen_coring_idx_scale", "null-ptr fail case", id_cnt++);

    Rval = ikc_sharpen_min_coring(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_sharpen_min_coring", "null-ptr fail case", id_cnt++);

    Rval = ikc_sharpen_max_coring(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_sharpen_max_coring", "null-ptr fail case", id_cnt++);

    Rval = ikc_sharpen_scale_coring(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_sharpen_scale_coring", "null-ptr fail case", id_cnt++);

    {
        ikc_in_lnl_t in_lnl;
        ikc_out_lnl_t out_lnl;

        in_lnl.p_luma_noise_reduce = &p_filters->input_param.luma_noise_reduce;
        in_lnl.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
        in_lnl.p_rgb_to_12y = &p_filters->input_param.rgb_to_12y;
        in_lnl.p_cfa_win = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
        out_lnl.p_cr_30 = p_flow_tbl_list->p_CR_buf_30;
        out_lnl.cr_30_size = CR_SIZE_30;
        out_lnl.p_flow_info = &p_flow->flow_info;

        Rval = ikc_lnl(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_lnl", "null-ptr fail case", id_cnt++);

        in_lnl.p_luma_noise_reduce->enable = 1;
        in_lnl.p_luma_noise_reduce->sensor_wb_r = 4096;
        in_lnl.p_luma_noise_reduce->sensor_wb_g = 4096;
        in_lnl.p_luma_noise_reduce->sensor_wb_b = 4096;
        in_lnl.p_luma_noise_reduce->strength0 = 0;
        in_lnl.p_luma_noise_reduce->strength1 = 0;
        in_lnl.p_luma_noise_reduce->strength2 = 0;
        in_lnl.p_luma_noise_reduce->strength2_max_change = 0;
        in_lnl.p_after_ce_wb_gain->gain_r = 4096;
        in_lnl.p_after_ce_wb_gain->gain_g = 4096;
        in_lnl.p_after_ce_wb_gain->gain_b = 4096;
        in_lnl.p_rgb_to_12y->enable = 0;
        in_lnl.p_cfa_win->width = 1920;
        in_lnl.p_cfa_win->height = 1080;

        Rval = ikc_lnl(&in_lnl, &out_lnl);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_lnl", "OK case", id_cnt++);

    }
    Rval = ikc_lnl_tbl(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_lnl_tbl", "null-ptr fail case", id_cnt++);

    {
        ikc_in_final_sharpen_t in_fnl_shp;
        in_fnl_shp.sharpen_mode = p_filters->input_param.final_sharpen_both.mode;
        in_fnl_shp.sharpen_both_enable = p_filters->input_param.final_sharpen_both.enable;
        in_fnl_shp.p_final_sharpen_noise = &p_filters->input_param.final_sharpen_noise;
        in_fnl_shp.p_final_sharpen_fir = &p_filters->input_param.final_sharpen_fir;
        in_fnl_shp.p_main = &p_filters->input_param.window_size_info.main_win;

        Rval = ikc_fnlshp(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_fnlshp", "null-ptr fail case", id_cnt++);

        {
            //vid8 config
            in_fnl_shp.sharpen_mode = 2;
            in_fnl_shp.sharpen_both_enable = 1;

            in_fnl_shp.p_final_sharpen_noise->max_change_up = 0;
            in_fnl_shp.p_final_sharpen_noise->max_change_down = 0;
            in_fnl_shp.p_final_sharpen_noise->spatial_fir.specify = 2;
            //in_fnl_shp.p_final_sharpen_noise->spatial_fir.per_dir_fir_iso_strengths[9];
            //in_fnl_shp.p_final_sharpen_noise->spatial_fir.per_dir_fir_dir_strengths[9];
            //in_fnl_shp.p_final_sharpen_noise->spatial_fir.per_dir_fir_dir_amounts[9];
            //in_fnl_shp.p_final_sharpen_noise->spatial_fir.coefs[9][25];
            in_fnl_shp.p_final_sharpen_noise->spatial_fir.strength_iso = 0;
            in_fnl_shp.p_final_sharpen_noise->spatial_fir.strength_dir = 12;
            in_fnl_shp.p_final_sharpen_noise->level_str_adjust.low = 20;
            in_fnl_shp.p_final_sharpen_noise->level_str_adjust.low_delta = 4;
            in_fnl_shp.p_final_sharpen_noise->level_str_adjust.low_strength = 8;
            in_fnl_shp.p_final_sharpen_noise->level_str_adjust.mid_strength = 8;
            in_fnl_shp.p_final_sharpen_noise->level_str_adjust.high = 128;
            in_fnl_shp.p_final_sharpen_noise->level_str_adjust.high_delta = 5;
            in_fnl_shp.p_final_sharpen_noise->level_str_adjust.high_strength = 8;
            in_fnl_shp.p_final_sharpen_noise->level_str_adjust_not_t0_t1_level_based = 1;
            in_fnl_shp.p_final_sharpen_noise->t0 = 0;
            in_fnl_shp.p_final_sharpen_noise->t1 = 0;
            in_fnl_shp.p_final_sharpen_noise->alpha_min = 0;
            in_fnl_shp.p_final_sharpen_noise->alpha_max = 0;

            in_fnl_shp.p_final_sharpen_noise->directional_decide_t0 = 0;
            in_fnl_shp.p_final_sharpen_noise->directional_decide_t1 = 0;

            in_fnl_shp.p_final_sharpen_noise->advanced_iso.enable = 0;
            in_fnl_shp.p_final_sharpen_noise->advanced_iso.max_change_method = 0;
            //in_fnl_shp.p_final_sharpen_noise->advanced_iso.max_change_table[32];
            in_fnl_shp.p_final_sharpen_noise->advanced_iso.noise_level_method = 0;
            //in_fnl_shp.p_final_sharpen_noise->advanced_iso.noise_level_table[32];
            in_fnl_shp.p_final_sharpen_noise->advanced_iso.size = 0;
            in_fnl_shp.p_final_sharpen_noise->advanced_iso.str_a_method = 0;
            //in_fnl_shp.p_final_sharpen_noise->advanced_iso.str_a_table[32];
            in_fnl_shp.p_final_sharpen_noise->advanced_iso.str_b_method = 0;
            //in_fnl_shp.p_final_sharpen_noise->advanced_iso.str_b_table[32];

            in_fnl_shp.p_final_sharpen_fir->specify = 2;
            //in_fnl_shp.p_final_sharpen_fir->per_dir_fir_iso_strengths[9];
            //in_fnl_shp.p_final_sharpen_fir->per_dir_fir_dir_strengths[9];
            //in_fnl_shp.p_final_sharpen_fir->per_dir_fir_dir_amounts[9];
            //in_fnl_shp.p_final_sharpen_fir->coefs[9][25];
            in_fnl_shp.p_final_sharpen_fir->strength_iso = 192;
            in_fnl_shp.p_final_sharpen_fir->strength_dir = 0;
            in_fnl_shp.p_final_sharpen_fir->pos_fir_artifact_reduce_enable = 0;
            in_fnl_shp.p_final_sharpen_fir->pos_fir_artifact_reduce_strength = 0;
            in_fnl_shp.p_final_sharpen_fir->over_shoot_reduce_allow = 255;
            in_fnl_shp.p_final_sharpen_fir->over_shoot_reduce_strength = 0;
            in_fnl_shp.p_final_sharpen_fir->under_shoot_reduce_allow = 255;
            in_fnl_shp.p_final_sharpen_fir->under_shoot_reduce_strength = 0;

            in_fnl_shp.p_main->width = 1920;
            in_fnl_shp.p_main->height = 1080;

        }

        in_fnl_shp.p_final_sharpen_noise->advanced_iso.enable = 1;
        in_fnl_shp.p_final_sharpen_fir->pos_fir_artifact_reduce_enable = 1;
        in_fnl_shp.p_final_sharpen_noise->spatial_fir.specify = 1;
        in_fnl_shp.p_final_sharpen_fir->specify = 1;

        Rval = ikc_fnlshp(&in_fnl_shp, &out_fnl_shp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_fnlshp", "OK case", id_cnt++);

        in_fnl_shp.p_final_sharpen_noise->spatial_fir.specify = 3;
        in_fnl_shp.p_final_sharpen_fir->specify = 3;

        Rval = ikc_fnlshp(&in_fnl_shp, &out_fnl_shp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_fnlshp", "OK case", id_cnt++);

        in_fnl_shp.p_final_sharpen_noise->spatial_fir.specify = 4;
        in_fnl_shp.p_final_sharpen_fir->specify = 4;

        Rval = ikc_fnlshp(&in_fnl_shp, &out_fnl_shp);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_fnlshp", "OK case", id_cnt++);

    }
    Rval = ikc_fnlshp_both(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_fnlshp_both", "null-ptr fail case", id_cnt++);

    Rval = ikc_fnlshp_coring(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_fnlshp_coring", "null-ptr fail case", id_cnt++);

    Rval = ikc_fnlshp_coring_idx_scale(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_fnlshp_coring_idx_scale", "null-ptr fail case", id_cnt++);

    Rval = ikc_fnlshp_min_coring(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_fnlshp_min_coring", "null-ptr fail case", id_cnt++);

    Rval = ikc_fnlshp_max_coring(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_fnlshp_max_coring", "null-ptr fail case", id_cnt++);

    Rval = ikc_fnlshp_scale_coring(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_fnlshp_scale_coring", "null-ptr fail case", id_cnt++);

    Rval = ikc_fnlshp_both_3d(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitSharpen.c", "ikc_fnlshp_both_3d", "null-ptr fail case", id_cnt++);
}

static void IK_TestCovr_IkcUnitConfig(void)
{
    uint32 Rval = IK_OK;
    uintptr addr = 0;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_filter_t *p_filters = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    idsp_ik_flow_ctrl_t *p_flow = NULL;
    (void)img_ctx_get_flow_tbl_list(0, 0, &addr);
    (void)memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
    (void)img_ctx_get_context(0, &p_ctx);
    p_filters = &p_ctx->filters;
    (void)img_ctx_get_flow_control(0, 0, &addr);
    (void)memcpy(&p_flow, &addr, sizeof(void *));

    {
        uint32 minor_ver;
        extern uint32 ikc_check_version(const uint32 ik_major_num, uint32 ikc_major_num);
        extern uint32 ikc_get_minor_version(uint32 *p_minor_ver);

        Rval = ikc_check_version(0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitConfig.c", "ikc_check_version", "test (ik_major_num != IK_VERSION_MAJOR) true NG case", id_cnt++);

        Rval = ikc_check_version(0, IKC_VERSION_MAJOR);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitConfig.c", "ikc_check_version", "test (ikc_major_num != IKC_VERSION_MAJOR) false OK case", id_cnt++);

        Rval = ikc_get_minor_version(NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitConfig.c", "ikc_get_minor_version", "test NULL ptr NG case", id_cnt++);

        Rval = ikc_get_minor_version(&minor_ver);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitConfig.c", "ikc_get_minor_version", "test if(p_minor_ver != NULL) true OK case", id_cnt++);
    }

    {
        ikc_in_input_mode_t in_input = {0};
        ikc_out_input_mode_t out_input = {0};
        extern uint32 ikc_input_mode(const ikc_in_input_mode_t *p_in_input, ikc_out_input_mode_t *p_out_input);

        Rval = ikc_input_mode(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitConfig.c", "ikc_input_mode", "test if (rval == IK_OK) false NG case", id_cnt++);

        in_input.flip_mode = 0;
        in_input.p_sensor_info = &p_filters->input_param.sensor_info;
        in_input.is_yuv_mode = 0;

        out_input.p_cr_4 = p_flow_tbl_list->p_CR_buf_4;
        out_input.cr_4_size = CR_SIZE_4;
        out_input.p_cr_6 = p_flow_tbl_list->p_CR_buf_6;
        out_input.cr_6_size = CR_SIZE_6;
        out_input.p_cr_7 = p_flow_tbl_list->p_CR_buf_7;
        out_input.cr_7_size = CR_SIZE_7;
        out_input.p_cr_8 = p_flow_tbl_list->p_CR_buf_8;
        out_input.cr_8_size = CR_SIZE_8;
        out_input.p_cr_9 = p_flow_tbl_list->p_CR_buf_9;
        out_input.cr_9_size = CR_SIZE_9;
        out_input.p_cr_11 = p_flow_tbl_list->p_CR_buf_11;
        out_input.cr_11_size = CR_SIZE_11;
        out_input.p_cr_12 = p_flow_tbl_list->p_CR_buf_12;
        out_input.cr_12_size = CR_SIZE_12;
        out_input.p_cr_13 = p_flow_tbl_list->p_CR_buf_13;
        out_input.cr_13_size = CR_SIZE_13;
        out_input.p_cr_16 = p_flow_tbl_list->p_CR_buf_16;
        out_input.cr_16_size = CR_SIZE_16;
        out_input.p_cr_21 = p_flow_tbl_list->p_CR_buf_21;
        out_input.cr_21_size = CR_SIZE_21;
        out_input.p_cr_22 = p_flow_tbl_list->p_CR_buf_22;
        out_input.cr_22_size = CR_SIZE_22;
        out_input.p_cr_23 = p_flow_tbl_list->p_CR_buf_23;
        out_input.cr_23_size = CR_SIZE_23;
        out_input.p_cr_24 = p_flow_tbl_list->p_CR_buf_24;
        out_input.cr_24_size = CR_SIZE_24;
        out_input.p_cr_26 = p_flow_tbl_list->p_CR_buf_26;
        out_input.cr_26_size = CR_SIZE_26;
        out_input.p_cr_29 = p_flow_tbl_list->p_CR_buf_29;
        out_input.cr_29_size = CR_SIZE_29;
        out_input.p_cr_30 = p_flow_tbl_list->p_CR_buf_30;
        out_input.cr_30_size = CR_SIZE_30;
        out_input.p_cr_31 = p_flow_tbl_list->p_CR_buf_31;
        out_input.cr_31_size = CR_SIZE_31;
        out_input.p_cr_32 = p_flow_tbl_list->p_CR_buf_32;
        out_input.cr_32_size = CR_SIZE_32;
        out_input.p_cr_117 = p_flow_tbl_list->p_CR_buf_117;
        out_input.cr_117_size = CR_SIZE_117;
        out_input.p_flow = &p_flow->flow_info;
        out_input.p_calib = &p_flow->calib;

        in_input.flip_mode = IK_FLIP_RAW_H | IK_FLIP_RAW_V;
        Rval = ikc_input_mode(&in_input, &out_input);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitConfig.c", "ikc_input_mode", "test flip H and flip V true OK case", id_cnt++);

        p_filters->input_param.sensor_info.sensor_mode = IK_SENSOR_MODE_RCCC;
        Rval = ikc_input_mode(&in_input, &out_input);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitConfig.c", "ikc_input_mode", "test if(p_in_input->p_sensor_info->sensor_mode == IK_SENSOR_MODE_RCCC) true OK case", id_cnt++);

        p_filters->input_param.sensor_info.compression = 1;
        Rval = ikc_input_mode(&in_input, &out_input);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitConfig.c", "ikc_input_mode", "test ((p_in_input->p_sensor_info->compression > 0UL)&&(p_in_input->p_sensor_info->compression < 9UL)) true OK case", id_cnt++);

        p_filters->input_param.sensor_info.compression = 11;
        Rval = ikc_input_mode(&in_input, &out_input);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitConfig.c", "ikc_input_mode", "test ((p_in_input->p_sensor_info->compression > 0UL)&&(p_in_input->p_sensor_info->compression < 9UL)) false OK case", id_cnt++);
    }

    {
        ikc_in_input_mode_sub_t in_input_mode_sub;
        ikc_out_input_mode_sub_t out_input_mode_sub;
        extern uint32 ikc_input_mode_sub(const ikc_in_input_mode_sub_t *p_in_input_sub, ikc_out_input_mode_sub_t *p_out_input_sub);

        Rval = ikc_input_mode_sub(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitConfig.c", "ikc_input_mode_sub", "test if (rval == IK_OK) false NG case", id_cnt++);

        in_input_mode_sub.flip_mode = p_filters->input_param.flip_mode;
        in_input_mode_sub.is_yuv_mode = 0u;
        in_input_mode_sub.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
        out_input_mode_sub.p_cr_45 = p_flow_tbl_list->p_CR_buf_45;
        out_input_mode_sub.cr_45_size = CR_SIZE_45;
        out_input_mode_sub.p_cr_47 = p_flow_tbl_list->p_CR_buf_47;
        out_input_mode_sub.cr_47_size = CR_SIZE_47;
        out_input_mode_sub.p_cr_49 = p_flow_tbl_list->p_CR_buf_49;
        out_input_mode_sub.cr_49_size = CR_SIZE_49;
        out_input_mode_sub.p_cr_50 = p_flow_tbl_list->p_CR_buf_50;
        out_input_mode_sub.cr_50_size = CR_SIZE_50;
        out_input_mode_sub.p_cr_51 = p_flow_tbl_list->p_CR_buf_51;
        out_input_mode_sub.cr_51_size = CR_SIZE_51;

        in_input_mode_sub.is_yuv_mode = 1u;
        Rval = ikc_input_mode_sub(&in_input_mode_sub, &out_input_mode_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitConfig.c", "ikc_input_mode_sub", "test if(p_in_input_sub->is_yuv_mode == 0UL) false NG case", id_cnt++);
        in_input_mode_sub.is_yuv_mode = 0u;

        in_input_mode_sub.flip_mode = IK_FLIP_RAW_H | IK_FLIP_RAW_V;
        Rval = ikc_input_mode_sub(&in_input_mode_sub, &out_input_mode_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitConfig.c", "ikc_input_mode_sub", "test flip H and flip V true OK case", id_cnt++);
    }
}

static void IK_TestCovr_IkcUnitHdrCeFilters(void)
{
    uint32 Rval = IK_OK;
    uintptr addr = 0;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_filter_t *p_filters = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    idsp_ik_flow_ctrl_t *p_flow = NULL;
    (void)img_ctx_get_flow_tbl_list(0, 0, &addr);
    (void)memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
    (void)img_ctx_get_context(0, &p_ctx);
    p_filters = &p_ctx->filters;
    (void)img_ctx_get_flow_control(0, 0, &addr);
    (void)memcpy(&p_flow, &addr, sizeof(void *));

    {
        ikc_in_decompress_t in_decompress;
        ikc_out_decompress_t out_decompress;
        extern uint32 ikc_decompression(const ikc_in_decompress_t *p_in_decompress, ikc_out_decompress_t *p_out_decompress);

        Rval = ikc_decompression(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_decompression", "test if (rval == IK_OK) false NG case", id_cnt++);

        in_decompress.exp_num = 1;
        in_decompress.compress_mode = p_filters->input_param.sensor_info.compression;
        in_decompress.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
        out_decompress.p_cr_4 = p_flow_tbl_list->p_CR_buf_4;
        out_decompress.cr_4_size = CR_SIZE_4;

        in_decompress.compress_mode = 256;
        Rval = ikc_decompression(&in_decompress, &out_decompress);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_decompression", "test (p_in_decompress->compress_mode >= IK_RAW_COMPACT_8B) true OK case", id_cnt++);
        in_decompress.compress_mode = 100;
        Rval = ikc_decompression(&in_decompress, &out_decompress);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_decompression", "test (p_in_decompress->compress_mode >= IK_RAW_COMPACT_8B) false OK case", id_cnt++);

        in_decompress.exp_num = 3;

        in_decompress.compress_mode = 256;
        Rval = ikc_decompression(&in_decompress, &out_decompress);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_decompression", "test (p_in_decompress->compress_mode >= IK_RAW_COMPACT_8B) true OK case", id_cnt++);
        in_decompress.compress_mode = 100;
        Rval = ikc_decompression(&in_decompress, &out_decompress);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_decompression", "test (p_in_decompress->compress_mode >= IK_RAW_COMPACT_8B) false OK case", id_cnt++);
    }

    {
        ikc_in_decompress_t in_decompress;
        ikc_out_decompress_sub_t out_decompress_sub;
        extern uint32 ikc_decompression_sub(const ikc_in_decompress_t *p_in_decompress, ikc_out_decompress_sub_t *p_out_decompress_sub);

        Rval = ikc_decompression_sub(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_decompression_sub", "test if (rval == IK_OK) false NG case", id_cnt++);

        in_decompress.exp_num = 1;
        in_decompress.compress_mode = p_filters->input_param.sensor_info.compression;
        in_decompress.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
        out_decompress_sub.p_cr_45 = p_flow_tbl_list->p_CR_buf_45;
        out_decompress_sub.cr_45_size = CR_SIZE_45;

        in_decompress.compress_mode = 0;
        Rval = ikc_decompression_sub(&in_decompress, &out_decompress_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_decompression_sub", "test (p_in_decompress->compress_mode == 0UL) true OK case", id_cnt++);
        in_decompress.compress_mode = 256;
        Rval = ikc_decompression_sub(&in_decompress, &out_decompress_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_decompression_sub", "test (p_in_decompress->compress_mode >= IK_RAW_COMPACT_8B) true OK case", id_cnt++);
        in_decompress.compress_mode = 100;
        Rval = ikc_decompression_sub(&in_decompress, &out_decompress_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_decompression_sub", "test (p_in_decompress->compress_mode >= IK_RAW_COMPACT_8B) false OK case", id_cnt++);

        in_decompress.exp_num = 3;

        in_decompress.compress_mode = 256;
        Rval = ikc_decompression_sub(&in_decompress, &out_decompress_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_decompression_sub", "test (p_in_decompress->compress_mode >= IK_RAW_COMPACT_8B) true OK case", id_cnt++);
        in_decompress.compress_mode = 100;
        Rval = ikc_decompression_sub(&in_decompress, &out_decompress_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_decompression_sub", "test (p_in_decompress->compress_mode >= IK_RAW_COMPACT_8B) false OK case", id_cnt++);
    }

    {
        ikc_in_hdr_blend_t in_hdr_blend;
        ikc_out_hdr_blend_t out_hdr_blend;
        extern uint32 ikc_hdr_blend(const ikc_in_hdr_blend_t *p_in_hdr_blend, ikc_out_hdr_blend_t *p_out_hdr_blend);

        Rval = ikc_hdr_blend(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_blend", "test if (rval == IK_OK) false NG case", id_cnt++);

        in_hdr_blend.exp_num = 1;
        in_hdr_blend.p_hdr_raw_info = &p_filters->input_param.hdr_raw_info;
        in_hdr_blend.p_hdr_blend = &p_filters->input_param.hdr_blend;
        out_hdr_blend.p_cr_4 = p_flow_tbl_list->p_CR_buf_4;
        out_hdr_blend.cr_4_size = CR_SIZE_4;
        out_hdr_blend.p_flow = &p_flow->flow_info;

        Rval = ikc_hdr_blend(&in_hdr_blend, &out_hdr_blend);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_blend", "test if(p_in_hdr_blend->exp_num <= 1u) true OK case", id_cnt++);
    }

    {
        ikc_in_hdr_blend_t in_hdr_blend;
        ikc_out_hdr_blend_sub_t out_hdr_blend_sub;
        extern uint32 ikc_hdr_blend_sub(const ikc_in_hdr_blend_t *p_in_hdr_blend, ikc_out_hdr_blend_sub_t *p_out_hdr_blend_sub);

        Rval = ikc_hdr_blend_sub(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_blend_sub", "test if (rval == IK_OK) false NG case", id_cnt++);

        in_hdr_blend.exp_num = 1;
        in_hdr_blend.p_hdr_raw_info = &p_filters->input_param.hdr_raw_info;
        in_hdr_blend.p_hdr_blend = &p_filters->input_param.hdr_blend;
        out_hdr_blend_sub.p_cr_45 = p_flow_tbl_list->p_CR_buf_45;
        out_hdr_blend_sub.cr_45_size = CR_SIZE_45;

        Rval = ikc_hdr_blend_sub(&in_hdr_blend, &out_hdr_blend_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_blend_sub", "test if(p_in_hdr_blend->exp_num <= 1u) true OK case", id_cnt++);
    }

    {
        ikc_in_hdr_tone_curve_t in_hdr_tc;
        ikc_out_hdr_tone_curve_t out_hdr_tc;
        extern uint32 ikc_front_end_tone_curve(const ikc_in_hdr_tone_curve_t *p_in_hdr_tone_curve, ikc_out_hdr_tone_curve_t *p_out_hdr_tone_curve);

        Rval = ikc_front_end_tone_curve(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_front_end_tone_curve", "test if (rval == IK_OK) false NG case", id_cnt++);

        in_hdr_tc.p_fe_tone_curve = &p_filters->input_param.fe_tone_curve;
        out_hdr_tc.p_cr_4 = p_flow_tbl_list->p_CR_buf_4;
        out_hdr_tc.cr_4_size = CR_SIZE_4;
        out_hdr_tc.p_cr_5 = p_flow_tbl_list->p_CR_buf_5;
        out_hdr_tc.cr_5_size = CR_SIZE_5;

        p_filters->input_param.fe_tone_curve.decompand_enable = 1;
        Rval = ikc_front_end_tone_curve(&in_hdr_tc, &out_hdr_tc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_front_end_tone_curve", "test if(p_in_hdr_tone_curve->p_fe_tone_curve->decompand_enable == 1u) true OK case", id_cnt++);
    }

    {
        ikc_in_hdr_tone_curve_t in_hdr_tc;
        ikc_out_hdr_tone_curve_sub_t out_hdr_tc_sub;
        extern uint32 ikc_front_end_tone_curve_sub(const ikc_in_hdr_tone_curve_t *p_in_hdr_tone_curve, ikc_out_hdr_tone_curve_sub_t *p_out_hdr_tone_curve_sub);

        Rval = ikc_front_end_tone_curve_sub(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_front_end_tone_curve_sub", "test if (rval == IK_OK) false NG case", id_cnt++);

        in_hdr_tc.p_fe_tone_curve = &p_filters->input_param.fe_tone_curve;
        out_hdr_tc_sub.p_cr_45 = p_flow_tbl_list->p_CR_buf_45;
        out_hdr_tc_sub.cr_45_size = CR_SIZE_45;
        out_hdr_tc_sub.p_cr_46 = p_flow_tbl_list->p_CR_buf_46;
        out_hdr_tc_sub.cr_46_size = CR_SIZE_46;

        p_filters->input_param.fe_tone_curve.decompand_enable = 1;
        Rval = ikc_front_end_tone_curve_sub(&in_hdr_tc, &out_hdr_tc_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_front_end_tone_curve_sub", "test if(p_in_hdr_tone_curve->p_fe_tone_curve->decompand_enable == 1u) true OK case", id_cnt++);
    }

    {
        ikc_in_hdr_blc_t in_hdr_blc;
        ikc_out_hdr_blc_t out_hdr_blc;
        extern uint32 ikc_hdr_black_level(const ikc_in_hdr_blc_t *p_in_hdr_blc, ikc_out_hdr_blc_t *p_out_hdr_blc);

        Rval = ikc_hdr_black_level(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_black_level", "test if (rval == IK_OK) false NG case", id_cnt++);

        in_hdr_blc.exp_num = 3;
        in_hdr_blc.rgb_ir_mode = 1;
        in_hdr_blc.compression_offset = 0;
        (void)memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_static_blc, &p_filters->input_param.exp0_frontend_static_blc, sizeof(ik_static_blc_level_t));
        (void)memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc, &p_filters->input_param.exp1_frontend_static_blc, sizeof(ik_static_blc_level_t));
        (void)memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc, &p_filters->input_param.exp2_frontend_static_blc, sizeof(ik_static_blc_level_t));
        in_hdr_blc.p_exp0_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_static_blc;
        in_hdr_blc.p_exp1_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc;
        in_hdr_blc.p_exp2_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc;
        out_hdr_blc.p_cr_4 = p_flow_tbl_list->p_CR_buf_4;
        out_hdr_blc.cr_4_size = CR_SIZE_4;
        out_hdr_blc.p_cr_6 = p_flow_tbl_list->p_CR_buf_6;
        out_hdr_blc.cr_6_size = CR_SIZE_6;
        out_hdr_blc.p_cr_7 = p_flow_tbl_list->p_CR_buf_7;
        out_hdr_blc.cr_7_size = CR_SIZE_7;
        out_hdr_blc.p_cr_8 = p_flow_tbl_list->p_CR_buf_8;
        out_hdr_blc.cr_8_size = CR_SIZE_8;

        Rval = ikc_hdr_black_level(&in_hdr_blc, &out_hdr_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_black_level", "test if (p_in_hdr_blc->rgb_ir_mode != 0UL) true OK case", id_cnt++);

        in_hdr_blc.exp_num = 4;
        Rval = ikc_hdr_black_level(&in_hdr_blc, &out_hdr_blc);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_black_level", "test else if(i == 2UL) false OK case", id_cnt++);
    }

    {
        ikc_in_hdr_blc_t in_hdr_blc;
        ikc_out_hdr_blc_sub_t out_hdr_blc_sub;
        extern uint32 ikc_hdr_black_level_sub(const ikc_in_hdr_blc_t *p_in_hdr_blc, ikc_out_hdr_blc_sub_t *p_out_hdr_blc_sub);

        Rval = ikc_hdr_black_level_sub(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_black_level_sub", "test if (rval == IK_OK) false NG case", id_cnt++);

        in_hdr_blc.exp_num = 1;
        in_hdr_blc.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
        in_hdr_blc.compression_offset = 0;
        (void)memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_static_blc, &p_filters->input_param.exp0_frontend_static_blc, sizeof(ik_static_blc_level_t));
        (void)memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc, &p_filters->input_param.exp1_frontend_static_blc, sizeof(ik_static_blc_level_t));
        (void)memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc, &p_filters->input_param.exp2_frontend_static_blc, sizeof(ik_static_blc_level_t));
        in_hdr_blc.p_exp0_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_static_blc;
        in_hdr_blc.p_exp1_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc;
        in_hdr_blc.p_exp2_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc;
        out_hdr_blc_sub.p_cr_45 = p_flow_tbl_list->p_CR_buf_45;
        out_hdr_blc_sub.cr_45_size = CR_SIZE_45;

        Rval = ikc_hdr_black_level_sub(&in_hdr_blc, &out_hdr_blc_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_black_level_sub", "test if(p_in_hdr_blc->exp_num <= 1UL) true OK case", id_cnt++);

        in_hdr_blc.rgb_ir_mode = 1;
        Rval = ikc_hdr_black_level_sub(&in_hdr_blc, &out_hdr_blc_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_black_level_sub", "test if (p_in_hdr_blc->rgb_ir_mode != 0UL) true OK case", id_cnt++);

        in_hdr_blc.exp_num = 3;
        Rval = ikc_hdr_black_level_sub(&in_hdr_blc, &out_hdr_blc_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_black_level_sub", "test if(p_in_hdr_blc->exp_num <= 1UL) false and if (p_in_hdr_blc->rgb_ir_mode != 0UL) true OK case", id_cnt++);

        in_hdr_blc.exp_num = 4;
        Rval = ikc_hdr_black_level_sub(&in_hdr_blc, &out_hdr_blc_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_black_level_sub", "test else if(i == 2UL) false OK case", id_cnt++);
    }

    {
        ikc_in_hdr_dgain_t in_hdr_dgain;
        ikc_out_hdr_dgain_t out_hdr_dgain;
        extern uint32 ikc_hdr_dgain(const ikc_in_hdr_dgain_t *p_in_hdr_dgain, ikc_out_hdr_dgain_t *p_out_hdr_dgain);

        Rval = ikc_hdr_dgain(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_dgain", "test if (rval == IK_OK) false NG case", id_cnt++);

        in_hdr_dgain.exp_num = 3;
        in_hdr_dgain.rgb_ir_mode = 1;
        p_filters->input_param.exp0_frontend_wb_gain.shutter_ratio = 1024;
        p_filters->input_param.exp1_frontend_wb_gain.shutter_ratio = 1024;
        p_filters->input_param.exp2_frontend_wb_gain.shutter_ratio = 0;
        (void)memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain, &p_filters->input_param.exp0_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
        (void)memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_wb_gain, &p_filters->input_param.exp1_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
        (void)memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_wb_gain, &p_filters->input_param.exp2_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
        in_hdr_dgain.p_exp0_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain;
        in_hdr_dgain.p_exp1_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_wb_gain;
        in_hdr_dgain.p_exp2_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_wb_gain;

        (void)memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_static_blc, &p_filters->input_param.exp0_frontend_static_blc, sizeof(ik_static_blc_level_t));
        (void)memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc, &p_filters->input_param.exp1_frontend_static_blc, sizeof(ik_static_blc_level_t));
        (void)memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc, &p_filters->input_param.exp2_frontend_static_blc, sizeof(ik_static_blc_level_t));
        in_hdr_dgain.p_exp0_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_static_blc;
        in_hdr_dgain.p_exp1_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc;
        in_hdr_dgain.p_exp2_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc;

        out_hdr_dgain.p_cr_4 = p_flow_tbl_list->p_CR_buf_4;
        out_hdr_dgain.cr_4_size = CR_SIZE_4;

        Rval = ikc_hdr_dgain(&in_hdr_dgain, &out_hdr_dgain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_dgain", "test if (p_in_hdr_dgain->rgb_ir_mode != 0UL) true OK case", id_cnt++);

        in_hdr_dgain.exp_num = 4;
        Rval = ikc_hdr_dgain(&in_hdr_dgain, &out_hdr_dgain);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_dgain", "test else if(i == 2UL) false OK case", id_cnt++);
    }

    {
        ikc_in_hdr_dgain_t in_hdr_dgain;
        ikc_out_hdr_dgain_sub_t out_hdr_dgain_sub;
        extern uint32 ikc_hdr_dgain_sub(const ikc_in_hdr_dgain_t *p_in_hdr_dgain, ikc_out_hdr_dgain_sub_t *p_out_hdr_dgain_sub);

        Rval = ikc_hdr_dgain_sub(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_dgain_sub", "test if (rval == IK_OK) false NG case", id_cnt++);

        in_hdr_dgain.exp_num = 1;
        in_hdr_dgain.rgb_ir_mode = 0;
        p_filters->input_param.exp0_frontend_wb_gain.shutter_ratio = 1024;
        p_filters->input_param.exp1_frontend_wb_gain.shutter_ratio = 1024;
        p_filters->input_param.exp2_frontend_wb_gain.shutter_ratio = 0;
        (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain, &p_filters->input_param.exp0_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
        (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_wb_gain, &p_filters->input_param.exp1_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
        (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_wb_gain, &p_filters->input_param.exp2_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
        in_hdr_dgain.p_exp0_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain;
        in_hdr_dgain.p_exp1_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_wb_gain;
        in_hdr_dgain.p_exp2_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_wb_gain;
        in_hdr_dgain.p_exp0_frontend_static_blc = &p_filters->input_param.exp0_frontend_static_blc;
        in_hdr_dgain.p_exp1_frontend_static_blc = &p_filters->input_param.exp1_frontend_static_blc;
        in_hdr_dgain.p_exp2_frontend_static_blc = &p_filters->input_param.exp2_frontend_static_blc;
        out_hdr_dgain_sub.p_cr_45 = p_flow_tbl_list->p_CR_buf_45;
        out_hdr_dgain_sub.cr_45_size = CR_SIZE_45;

        Rval = ikc_hdr_dgain_sub(&in_hdr_dgain, &out_hdr_dgain_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_dgain_sub", "test if(p_in_hdr_dgain->exp_num <= 1UL) true OK case", id_cnt++);

        in_hdr_dgain.rgb_ir_mode = 1;
        Rval = ikc_hdr_dgain_sub(&in_hdr_dgain, &out_hdr_dgain_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_dgain_sub", "test if (p_in_hdr_dgain->rgb_ir_mode != 0UL) true OK case", id_cnt++);

        in_hdr_dgain.exp_num = 3;
        Rval = ikc_hdr_dgain_sub(&in_hdr_dgain, &out_hdr_dgain_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_dgain_sub", "test if(p_in_hdr_dgain->exp_num <= 1UL) false and if (p_in_hdr_dgain->rgb_ir_mode != 0UL) true OK case", id_cnt++);

        in_hdr_dgain.exp_num = 4;
        Rval = ikc_hdr_dgain_sub(&in_hdr_dgain, &out_hdr_dgain_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_hdr_dgain_sub", "test else if(i == 2UL) false OK case", id_cnt++);
    }

    {
        ikc_in_ce_t in_ce;
        ikc_out_ce_t out_ce;
        extern uint32 ikc_contrast_enhancement(const ikc_in_ce_t *p_in_ce, ikc_out_ce_t *p_out_ce);

        Rval = ikc_contrast_enhancement(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_contrast_enhancement", "test if (rval == IK_OK) false NG case", id_cnt++);

        in_ce.exp_num = 1;
        in_ce.sensor_pattern = 1;
        in_ce.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
        in_ce.p_ce = &p_filters->input_param.ce;
        out_ce.p_cr_13 = p_flow_tbl_list->p_CR_buf_13;
        out_ce.cr_13_size = CR_SIZE_13;
        out_ce.p_flow = &p_flow->flow_info;

        Rval = ikc_contrast_enhancement(&in_ce, &out_ce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_contrast_enhancement", "test if (p_in_ce->sensor_pattern == IKC_BAYER_PATTERN_BG) true OK case", id_cnt++);

        in_ce.sensor_pattern = 3;
        Rval = ikc_contrast_enhancement(&in_ce, &out_ce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_contrast_enhancement", "test if (p_in_ce->sensor_pattern == IKC_BAYER_PATTERN_GB) true OK case", id_cnt++);

        in_ce.sensor_pattern = 4;
        Rval = ikc_contrast_enhancement(&in_ce, &out_ce);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_contrast_enhancement", "test sensor_pattern out of range OK case", id_cnt++);
    }

    {
        extern uint32 ikc_contrast_enhancement_input(const ikc_in_ce_input_t *p_in_ce_input, ikc_out_ce_input_t *p_out_ce_input);

        Rval = ikc_contrast_enhancement_input(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_contrast_enhancement_input", "test if (rval == IK_OK) false NG case", id_cnt++);
    }

    {
        extern uint32 ikc_contrast_enhancement_output(const ikc_in_ce_output_t *p_in_ce_output, ikc_out_ce_t *p_out_ce);

        Rval = ikc_contrast_enhancement_output(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_contrast_enhancement_output", "test if (rval == IK_OK) false NG case", id_cnt++);
    }

    {
        ikc_in_ce_t in_ce;
        ikc_out_ce_sub_t out_ce_sub;
        extern uint32 ikc_contrast_enhancement_sub(const ikc_in_ce_t *p_in_ce, ikc_out_ce_sub_t *p_out_ce_sub);

        Rval = ikc_contrast_enhancement_sub(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_contrast_enhancement_sub", "test if (rval == IK_OK) false NG case", id_cnt++);

        in_ce.sensor_pattern = 1;
        in_ce.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
        in_ce.p_ce = &p_filters->input_param.ce;
        out_ce_sub.p_cr_49 = p_flow_tbl_list->p_CR_buf_49;
        out_ce_sub.cr_49_size = CR_SIZE_49;
        out_ce_sub.p_cr_50 = p_flow_tbl_list->p_CR_buf_50;
        out_ce_sub.cr_50_size = CR_SIZE_50;
        out_ce_sub.p_cr_51 = p_flow_tbl_list->p_CR_buf_51;
        out_ce_sub.cr_51_size = CR_SIZE_51;

        Rval = ikc_contrast_enhancement_sub(&in_ce, &out_ce_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_contrast_enhancement_sub", "test if (p_in_ce->sensor_pattern == IKC_BAYER_PATTERN_BG) true OK case", id_cnt++);

        in_ce.sensor_pattern = 3;
        Rval = ikc_contrast_enhancement_sub(&in_ce, &out_ce_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_contrast_enhancement_sub", "test if (p_in_ce->sensor_pattern == IKC_BAYER_PATTERN_GB) true OK case", id_cnt++);

        in_ce.sensor_pattern = 4;
        Rval = ikc_contrast_enhancement_sub(&in_ce, &out_ce_sub);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_contrast_enhancement_sub", "test sensor_pattern out of range OK case", id_cnt++);
    }

    {
        extern uint32 ikc_contrast_enhancement_input_sub(const ikc_in_ce_input_sub_t *p_in_ce_input_sub, ikc_out_ce_input_sub_t *p_out_ce_input_sub);

        Rval = ikc_contrast_enhancement_input_sub(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_contrast_enhancement_input_sub", "test if (rval == IK_OK) false NG case", id_cnt++);
    }

    {
        extern uint32 ikc_before_ce_gain(const ikc_in_before_ce_gain_t *p_in_ce_gain, ikc_out_before_ce_gain_t *p_out_ce_gain);

        Rval = ikc_before_ce_gain(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_before_ce_gain", "test if (rval == IK_OK) false NG case", id_cnt++);
    }

    {
        extern uint32 ikc_before_ce_gain_sub(const ikc_in_before_ce_gain_t *p_in_ce_gain, ikc_out_before_ce_gain_sub_t *p_out_ce_gain_sub);

        Rval = ikc_before_ce_gain_sub(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitHdrCeFilters.c", "ikc_before_ce_gain_sub", "test if (rval == IK_OK) false NG case", id_cnt++);
    }
}

static void IK_TestCovr_IkcUtility(void)
{
    uint32 Rval = IK_OK;

    {
        extern uint32 ikc_check_uint8_parameter_valid_range(char const *parameter_name, uint8 parameter_value, uint8 min_value, uint8 max_value);

        Rval = ikc_check_uint8_parameter_valid_range(NULL, 0, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_uint8_parameter_valid_range", "test if (parameter_name==NULL) true NG case", id_cnt++);

        Rval = ikc_check_uint8_parameter_valid_range("test_u8_valid", 0, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_uint8_parameter_valid_range", "test if (parameter_name==NULL) false OK case", id_cnt++);

        Rval = ikc_check_uint8_parameter_valid_range("test_u8_valid", 0, 1, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_uint8_parameter_valid_range", "test (parameter_value < min_value) true NG case", id_cnt++);

        Rval = ikc_check_uint8_parameter_valid_range("test_u8_valid", 1, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_uint8_parameter_valid_range", "test (parameter_value > max_value) true NG case", id_cnt++);
    }

    {
        extern uint32 ikc_check_int8_parameter_valid_range(char const *parameter_name, int8 parameter_value, int8 min_value, int8 max_value);

        Rval = ikc_check_int8_parameter_valid_range(NULL, 0, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_int8_parameter_valid_range", "test if (parameter_name==NULL) true NG case", id_cnt++);

        Rval = ikc_check_int8_parameter_valid_range("test_i8_valid", 0, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_int8_parameter_valid_range", "test if (parameter_name==NULL) false OK case", id_cnt++);

        Rval = ikc_check_int8_parameter_valid_range("test_i8_valid", 0, 1, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_int8_parameter_valid_range", "test (parameter_value < min_value) true NG case", id_cnt++);

        Rval = ikc_check_int8_parameter_valid_range("test_i8_valid", 1, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_int8_parameter_valid_range", "test (parameter_value > max_value) true NG case", id_cnt++);
    }

    {
        extern uint32 ikc_check_uint16_parameter_valid_range(char const *parameter_name, uint16 parameter_value, uint16 min_value, uint16 max_value);

        Rval = ikc_check_uint16_parameter_valid_range(NULL, 0, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_uint16_parameter_valid_range", "test if (parameter_name==NULL) true NG case", id_cnt++);

        Rval = ikc_check_uint16_parameter_valid_range("test_u16_valid", 0, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_uint16_parameter_valid_range", "test if (parameter_name==NULL) false OK case", id_cnt++);

        Rval = ikc_check_uint16_parameter_valid_range("test_u16_valid", 0, 1, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_uint16_parameter_valid_range", "test (parameter_value < min_value) true NG case", id_cnt++);

        Rval = ikc_check_uint16_parameter_valid_range("test_u16_valid", 1, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_uint16_parameter_valid_range", "test (parameter_value > max_value) true NG case", id_cnt++);
    }

    {
        extern uint32 ikc_check_int16_parameter_valid_range(char const *parameter_name, int16 parameter_value, int16 min_value, int16 max_value);

        Rval = ikc_check_int16_parameter_valid_range(NULL, 0, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_int16_parameter_valid_range", "test if (parameter_name==NULL) true NG case", id_cnt++);

        Rval = ikc_check_int16_parameter_valid_range("test_i16_valid", 0, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_int16_parameter_valid_range", "test if (parameter_name==NULL) false OK case", id_cnt++);

        Rval = ikc_check_int16_parameter_valid_range("test_i16_valid", 0, 1, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_int16_parameter_valid_range", "test (parameter_value < min_value) true NG case", id_cnt++);

        Rval = ikc_check_int16_parameter_valid_range("test_i16_valid", 1, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_int16_parameter_valid_range", "test (parameter_value > max_value) true NG case", id_cnt++);
    }

    {
        extern uint32 ikc_check_uint32_parameter_valid_range(char const *parameter_name, uint32 parameter_value, uint32 min_value, uint32 max_value);

        Rval = ikc_check_uint32_parameter_valid_range(NULL, 0, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_uint32_parameter_valid_range", "test if (parameter_name==NULL) true NG case", id_cnt++);

        Rval = ikc_check_uint32_parameter_valid_range("test_u32_valid", 0, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_uint32_parameter_valid_range", "test if (parameter_name==NULL) false OK case", id_cnt++);

        Rval = ikc_check_uint32_parameter_valid_range("test_u32_valid", 0, 1, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_uint32_parameter_valid_range", "test (parameter_value < min_value) true NG case", id_cnt++);

        Rval = ikc_check_uint32_parameter_valid_range("test_u32_valid", 1, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_uint32_parameter_valid_range", "test (parameter_value > max_value) true NG case", id_cnt++);
    }

    {
        extern uint32 ikc_check_int32_parameter_valid_range(char const *parameter_name, int32 parameter_value, int32 min_value, int32 max_value);

        Rval = ikc_check_int32_parameter_valid_range(NULL, 0, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_int32_parameter_valid_range", "test if (parameter_name==NULL) true NG case", id_cnt++);

        Rval = ikc_check_int32_parameter_valid_range("test_u32_valid", 0, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_int32_parameter_valid_range", "test if (parameter_name==NULL) false OK case", id_cnt++);

        Rval = ikc_check_int32_parameter_valid_range("test_u32_valid", 0, 1, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_int32_parameter_valid_range", "test (parameter_value < min_value) true NG case", id_cnt++);

        Rval = ikc_check_int32_parameter_valid_range("test_u32_valid", 1, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_int32_parameter_valid_range", "test (parameter_value > max_value) true NG case", id_cnt++);
    }

    {
        extern uint32 ikc_check_cr_size_validation(char const *parameter_name, uint32 parameter_value, uint32 expected_value);

        Rval = ikc_check_cr_size_validation(NULL, 0, 0);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_cr_size_validation", "test if (parameter_name==NULL) true NG case", id_cnt++);

        Rval = ikc_check_cr_size_validation("test_cr_size_valid", 0, 1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_cr_size_validation", "test if ((parameter_value != expected_value)) true NG case", id_cnt++);
    }

    {
        extern uint32 ikc_check_addr_validation(char const *parameter_name, void const *p);

        Rval = ikc_check_addr_validation(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_addr_validation", "test if (parameter_name==NULL) true NG case", id_cnt++);

        Rval = ikc_check_addr_validation("test_addr_valid", (void *)1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_addr_validation", "test (ikc_valid_mem_start > addr) true NG case", id_cnt++);

        Rval = ikc_check_addr_validation("test_addr_valid", (void *)-1);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_addr_validation", "test (ikc_valid_mem_end < addr) true NG case", id_cnt++);
    }

    {
        extern uint32 ikc_check_addr_null(char const *parameter_name, void const *p);

        Rval = ikc_check_addr_null(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "ikc_check_addr_null", "test if (parameter_name==NULL) true NG case", id_cnt++);
    }

    {
        extern int32 clamp(int32 x, int32 minimun, int32 maximun);

        Rval = clamp(0, 1, 2);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_IkcUtility.c", "clamp", "test if (x < minimun) true OK case", id_cnt++);
        Rval = clamp(3, 1, 2);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_IkcUtility.c", "clamp", "test if (x > maximun) true OK case", id_cnt++);
    }

    {
        extern int32 shift_int32_right_int32_offset(int32 target, int32 offset);

        Rval = shift_int32_right_int32_offset(0, -1);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_IkcUtility.c", "shift_int32_right_int32_offset", "test if (offset>=0) false OK case", id_cnt++);
    }

    {
        extern int32 double_round(double64 d);

        Rval = double_round(-1.0);
        LOG_RVAL(pFile, IK_OK, "AmbaDSP_IkcUtility.c", "double_round", "test if (d < 0.0) true OK case", id_cnt++);
    }

    {
        ik_vin_sensor_geometry_t calib_geo = {0};
        ik_vin_sensor_geometry_t vin_geo = {0};
        extern uint32 calib_geometry_check(const ik_vin_sensor_geometry_t *p_calib_geo, const ik_vin_sensor_geometry_t *p_vin_geo, char const *parameter_name);

        Rval = calib_geometry_check(NULL, &vin_geo, "test_calib_geo");
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "calib_geometry_check", "test NULL ptr NG case", id_cnt++);
        Rval = calib_geometry_check(&calib_geo, NULL, "test_calib_geo");
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "calib_geometry_check", "test NULL ptr NG case", id_cnt++);

        Rval = calib_geometry_check(&calib_geo, &vin_geo, "test_calib_geo");
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "calib_geometry_check", "test calib factor_den or factor_num zero NG case", id_cnt++);

        calib_geo.h_sub_sample.factor_num = 1;
        calib_geo.h_sub_sample.factor_den = 1;
        calib_geo.v_sub_sample.factor_num = 1;
        calib_geo.v_sub_sample.factor_den = 1;

        vin_geo.h_sub_sample.factor_num = 1;
        vin_geo.h_sub_sample.factor_den = 1;
        vin_geo.v_sub_sample.factor_num = 1;
        vin_geo.v_sub_sample.factor_den = 1;

        calib_geo.start_x = 10;
        vin_geo.start_x = 0;
        Rval = calib_geometry_check(&calib_geo, &vin_geo, "test_calib_geo");
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "calib_geometry_check", "test (p_calib_geo->start_x> p_vin_geo->start_x) true NG case", id_cnt++);
        calib_geo.start_x = 0;

        calib_geo.start_y = 10;
        vin_geo.start_y = 0;
        Rval = calib_geometry_check(&calib_geo, &vin_geo, "test_calib_geo");
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "calib_geometry_check", "test (p_calib_geo->start_y> p_vin_geo->start_y) true NG case", id_cnt++);
        calib_geo.start_y = 0;

        calib_geo.start_y = 0;
        vin_geo.start_y = 10;
        Rval = calib_geometry_check(&calib_geo, &vin_geo, "test_calib_geo");
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUtility.c", "calib_geometry_check", "test ((p_calib_geo->start_y+calib_non_binning_height)<(p_vin_geo->start_y+vin_non_binning_height)) true NG case", id_cnt++);
        vin_geo.start_y = 0;
    }
}

INT32 ikc_ctest_main(void)
{
    uint32 Rval = IK_OK;
    AMBA_IK_ABILITY_s Ability = {0};
    AMBA_IK_MODE_CFG_s Mode;

    id_cnt = 0;
    pFile = fopen("data/ikc_ctest.txt", "w");

    // init IK
    _load_idsp_default_binary();
    Ability.Pipe = AMBA_IK_PIPE_VIDEO;
    Ability.VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_3;

    _Init_Image_Kernel(&Ability);
    _Init_Image_Kernel_Core();

    Mode.ContextId = 0;
    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    IK_TestCovr_IkcRefUtility();

    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    IK_TestCovr_IkcUnitConfig();

    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    IK_TestCovr_IkcUnitHdrCeFilters();

    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    IK_TestCovr_IkcUtility();

    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    IK_TestCovr_IkcUnitAaa();

    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    IK_TestCovr_IkcUnitCaWarp();

    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    IK_TestCovr_IkcUnitWarp();

    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    IK_TestCovr_IkcUnitResampler();

    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    IK_TestCovr_IkcUnitVignette();

    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    IK_TestCovr_IkcUnitStaticBadPixel();

    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    IK_TestCovr_IkcUnitYuvFilters();

    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    IK_TestCovr_IkcUnitRgbFilters();

    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    IK_TestCovr_IkcUnitCfaFilters();

    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    IK_TestCovr_IkcUnitMctfAlgo();

    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    IK_TestCovr_IkcUnitSharpen();

    Rval |= AmbaIK_InitContext(&Mode, &Ability);


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

