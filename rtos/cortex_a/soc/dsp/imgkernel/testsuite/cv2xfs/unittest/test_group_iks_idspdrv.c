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

static void IKS_TestCovr_idspdrv_aaa(const AMBA_IK_MODE_CFG_s *pMode)
{
    aaa_statistics_setup_t aaa_setup = {0};
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        extern void idspdrv_setup_aaa_cfa_awb_tile_info(uintptr aaa_data, const aaa_statistics_setup_t *p_aaa_setup);

        aaa_setup.slice_width = 65535;
        aaa_setup.total_slices_x = 2;
        idspdrv_setup_aaa_cfa_awb_tile_info((uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "uint_min", "test if (x > y) false OK case", id_cnt++);

        aaa_setup.slice_width = 128;
        aaa_setup.slice_height = 128;
        aaa_setup.total_slices_x = 2;
        aaa_setup.total_slices_y = 2;
        idspdrv_setup_aaa_cfa_awb_tile_info((uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_setup_aaa_cfa_awb_tile_info", "test while((awb_tile_num_col * p_aaa_setup->total_slices_x * 4u) > p_aaa_setup->slice_width) true OK case", id_cnt++);
    }

    {
        extern void idspdrv_setup_aaa_cfa_ae_tile_info(uintptr aaa_data, const aaa_statistics_setup_t *p_aaa_setup);

        aaa_setup.ae_tile_width = 170;
        aaa_setup.ae_tile_col_start = 0;
        aaa_setup.slice_width = 1000;
        aaa_setup.total_slices_x = 1;
        aaa_setup.total_slices_y = 1;
        idspdrv_setup_aaa_cfa_ae_tile_info((uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_setup_aaa_cfa_ae_tile_info", "test (p_aaa_setup->slice_width == 1920UL) false OK case", id_cnt++);

        aaa_setup.ae_tile_col_start = 10;
        idspdrv_setup_aaa_cfa_ae_tile_info((uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_setup_aaa_cfa_ae_tile_info", "test (p_aaa_setup->ae_tile_col_start == 0UL) false OK case", id_cnt++);
    }

    {
        extern void idspdrv_setup_aaa_cfa_af_tile_info(uintptr aaa_data, const aaa_statistics_setup_t *p_aaa_setup);

        aaa_setup.af_tile_width = 170;
        aaa_setup.af_tile_col_start = 0;
        aaa_setup.slice_width = 1000;
        idspdrv_setup_aaa_cfa_af_tile_info((uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_setup_aaa_cfa_af_tile_info", "test (p_aaa_setup->slice_width == 1920UL) false OK case", id_cnt++);

        aaa_setup.af_tile_col_start = 10;
        idspdrv_setup_aaa_cfa_af_tile_info((uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_setup_aaa_cfa_af_tile_info", "test (p_aaa_setup->af_tile_col_start == 0UL) false OK case", id_cnt++);

        aaa_setup.af_tile_active_width = 170;
        aaa_setup.af_tile_width = 1280;
        aaa_setup.af_tile_col_start = 0;
        aaa_setup.slice_width = 1920;
        idspdrv_setup_aaa_cfa_af_tile_info((uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_setup_aaa_cfa_af_tile_info", "test (p_aaa_data[stitch_idx].cfa_af.af_tile_width == (80U - 1U)) false OK case", id_cnt++);

        aaa_setup.slice_width = 256;
        idspdrv_setup_aaa_cfa_af_tile_info((uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_setup_aaa_cfa_af_tile_info", "test (p_aaa_setup->slice_width == 1920UL) false OK case", id_cnt++);
    }

    {
        extern void idspdrv_setup_aaa_ae_tile_info(uintptr aaa_data, const aaa_statistics_setup_t *p_aaa_setup);

        aaa_setup.ae_tile_width = 170;
        aaa_setup.ae_tile_col_start = 10;
        aaa_setup.slice_width = 1920;
        aaa_setup.total_slices_x = 1;
        aaa_setup.total_slices_y = 1;
        idspdrv_setup_aaa_ae_tile_info((uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_setup_aaa_ae_tile_info", "test (p_aaa_setup->ae_tile_col_start == 0UL) false OK case", id_cnt++);
    }

    {
        extern void idspdrv_setup_aaa_af_tile_info(uintptr aaa_data, const aaa_statistics_setup_t *p_aaa_setup);

        aaa_setup.pg_af_tile_num_col = 30;
        aaa_setup.pg_af_tile_num_row = 10;
        aaa_setup.total_slices_x = 2;
        aaa_setup.total_slices_y = 2;
        idspdrv_setup_aaa_af_tile_info((uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_setup_aaa_af_tile_info", "test if(p_aaa_setup->pg_af_tile_num_col == AF_TILE_COL_COUNT) false OK case", id_cnt++);

        aaa_setup.pg_af_tile_width = 170;
        aaa_setup.pg_af_tile_col_start = 0;
        aaa_setup.slice_width = 1920;
        aaa_setup.total_slices_x = 1;
        aaa_setup.total_slices_y = 1;
        idspdrv_setup_aaa_af_tile_info((uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_setup_aaa_af_tile_info", "test if(p_aaa_setup->pg_af_tile_width == 170UL) true OK case", id_cnt++);

        aaa_setup.slice_width = 1000;
        idspdrv_setup_aaa_af_tile_info((uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_setup_aaa_af_tile_info", "test (p_aaa_setup->slice_width == 1920UL) false OK case", id_cnt++);

        aaa_setup.pg_af_tile_col_start = 10;
        idspdrv_setup_aaa_af_tile_info((uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_setup_aaa_af_tile_info", "test (p_aaa_setup->pg_af_tile_col_start == 0UL) false OK case", id_cnt++);

        aaa_setup.pg_af_tile_active_width = 170;
        aaa_setup.pg_af_tile_width = 1280;
        aaa_setup.pg_af_tile_col_start = 0;
        aaa_setup.slice_width = 1920;
        idspdrv_setup_aaa_af_tile_info((uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_setup_aaa_af_tile_info", "test (p_aaa_data[stitch_idx].pg_af.af_tile_width == (80U - 1U)) false OK case", id_cnt++);

        aaa_setup.slice_width = 256;
        idspdrv_setup_aaa_af_tile_info((uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_setup_aaa_af_tile_info", "test (p_aaa_setup->slice_width == 1920UL) false OK case", id_cnt++);

        aaa_setup.pg_af_tile_width = 170;
        aaa_setup.slice_width = 1920;
        idspdrv_setup_aaa_af_tile_info((uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_setup_aaa_af_tile_info", "test (p_aaa_setup->slice_width == 1920UL) true OK case", id_cnt++);
    }

    {
        extern void idspdrv_aaa_cfa_setup(uintptr sect_header_addr, uintptr aaa_data, const aaa_statistics_setup_t *p_aaa_setup, uint32 sensor_mode, uint32 bayer_pattern);

        idspdrv_aaa_cfa_setup(p_flow_tbl->sect_header_addr[0][0], (uintptr)FPNMap, &aaa_setup, 2, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_aaa_cfa_setup", "test (sensor_mode == 2u) if(bayer_pattern == 0U) true OK case", id_cnt++);
        idspdrv_aaa_cfa_setup(p_flow_tbl->sect_header_addr[0][0], (uintptr)FPNMap, &aaa_setup, 2, 1);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_aaa_cfa_setup", "test (sensor_mode == 2u) if(bayer_pattern == 1U) true OK case", id_cnt++);
        idspdrv_aaa_cfa_setup(p_flow_tbl->sect_header_addr[0][0], (uintptr)FPNMap, &aaa_setup, 2, 2);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_aaa_cfa_setup", "test (sensor_mode == 2u) if(bayer_pattern == 2U) true OK case", id_cnt++);

        idspdrv_aaa_cfa_setup(p_flow_tbl->sect_header_addr[0][0], (uintptr)FPNMap, &aaa_setup, 1, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_aaa_cfa_setup", "test (sensor_mode == 1u) if(bayer_pattern == 0U) true OK case", id_cnt++);
        idspdrv_aaa_cfa_setup(p_flow_tbl->sect_header_addr[0][0], (uintptr)FPNMap, &aaa_setup, 1, 1);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_aaa_cfa_setup", "test (sensor_mode == 1u) if(bayer_pattern == 1U) true OK case", id_cnt++);
        idspdrv_aaa_cfa_setup(p_flow_tbl->sect_header_addr[0][0], (uintptr)FPNMap, &aaa_setup, 1, 3);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_aaa_cfa_setup", "test (sensor_mode == 1u) if(bayer_pattern == 3U) true OK case", id_cnt++);

        aaa_setup.total_slices_x = 1;
        aaa_setup.total_slices_y = 2;
        idspdrv_aaa_cfa_setup(p_flow_tbl->sect_header_addr[0][0], (uintptr)FPNMap, &aaa_setup, 1, 3);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_aaa_cfa_setup", "test (p_aaa_setup->total_slices_y > 1u) true OK case", id_cnt++);
    }

    {
        extern void idspdrv_aaa_setup(uintptr sect_header_addr, uintptr aaa_data, const aaa_statistics_setup_t *p_aaa_setup);

        aaa_setup.total_slices_x = 1;
        aaa_setup.total_slices_y = 2;
        idspdrv_aaa_setup(p_flow_tbl->sect_header_addr[0][0], (uintptr)FPNMap, &aaa_setup);
        LOG_RVAL(pFile, IK_OK, "idspdrv_aaa.c", "idspdrv_aaa_setup", "test (p_aaa_setup->total_slices_y > 1u) true OK case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_black_level(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        vin_stat_exp_blc_t blc_lvl = {0};

        extern void idspdrv_config_vin_stat_black_level(uintptr sect_header_addr,
                uint8  index,
                const vin_stat_exp_blc_t *p_blc_lvl);

        idspdrv_config_vin_stat_black_level(p_flow_tbl->sect_header_addr[0][0], 3, &blc_lvl);
        LOG_RVAL(pFile, IK_OK, "idspdrv_black_level.c", "idspdrv_config_vin_stat_black_level", "test default OK case", id_cnt++);
    }

    {
        dgain_blc_t dgain_blc = {0};

        void idspdrv_set_hdr_dgain_black_level_offset(uintptr sect_header_addr,
                uint8 idx,
                const dgain_blc_t *p_dgain_blc);

        idspdrv_set_hdr_dgain_black_level_offset(p_flow_tbl->sect_header_addr[0][2], 6, &dgain_blc);
        LOG_RVAL(pFile, IK_OK, "idspdrv_black_level.c", "idspdrv_set_hdr_dgain_black_level_offset", "test index != 0, 1, or 2 OK case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_cfa_prescaler(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        cfa_prescale_param_t param = {0};

        extern void idspdrv_config_cfa_prescale(uintptr sect_header_addr,
                                                const cfa_prescale_param_t *p_param,
                                                uint32 verticalZoomFactor,
                                                uint32 horizontalZoomFactor);

        param.size_info.inputNumCols = 3840;
        param.size_info.inputNumRows = 2160;
        param.size_info.outputNumCols = 1920;
        param.size_info.outputNumRows = 1080;
        param.inputSampleMode = 1;
        idspdrv_config_cfa_prescale(p_flow_tbl->sect_header_addr[0][0], &param, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_cfa_prescaler.c", "idspdrv_adjust_binning_phase_shift", "test different readoutMode OK case", id_cnt++);
        param.inputSampleMode = 2;
        idspdrv_config_cfa_prescale(p_flow_tbl->sect_header_addr[0][0], &param, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_cfa_prescaler.c", "idspdrv_adjust_binning_phase_shift", "test different readoutMode OK case", id_cnt++);
        param.inputSampleMode = 3;
        idspdrv_config_cfa_prescale(p_flow_tbl->sect_header_addr[0][0], &param, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_cfa_prescaler.c", "idspdrv_adjust_binning_phase_shift", "test different readoutMode OK case", id_cnt++);
        param.inputSampleMode = 4;
        idspdrv_config_cfa_prescale(p_flow_tbl->sect_header_addr[0][0], &param, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_cfa_prescaler.c", "idspdrv_adjust_binning_phase_shift", "test different readoutMode OK case", id_cnt++);
        param.inputSampleMode = 5;
        idspdrv_config_cfa_prescale(p_flow_tbl->sect_header_addr[0][0], &param, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_cfa_prescaler.c", "idspdrv_adjust_binning_phase_shift", "test different readoutMode OK case", id_cnt++);
        param.inputSampleMode = 6;
        idspdrv_config_cfa_prescale(p_flow_tbl->sect_header_addr[0][0], &param, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_cfa_prescaler.c", "idspdrv_adjust_binning_phase_shift", "test different readoutMode OK case", id_cnt++);
        param.inputSampleMode = 0;

        idspdrv_set_resamp_control(RESAMP_COEFF_RECTWIN, RESAMP_SELECT_CFA);
        idspdrv_config_cfa_prescale(p_flow_tbl->sect_header_addr[0][0], &param, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_cfa_prescaler.c", "idspdrv_config_cfa_prescale_h_coef", "test ((resamp_control & RESAMP_COEFF_RECTWIN) != 0u) true OK case", id_cnt++);
        idspdrv_set_resamp_control(0u, RESAMP_SELECT_CFA);

        param.size_info.inputNumCols = 1920;
        param.size_info.inputWindowLeft = 16384;
        idspdrv_config_cfa_prescale(p_flow_tbl->sect_header_addr[0][0], &param, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_cfa_prescaler.c", "idspdrv_config_cfa_prescale_h_coef", "test (bool_result == 2U) 1:1 true OK case", id_cnt++);
        param.size_info.inputNumCols = 3840;
        param.size_info.inputWindowLeft = 0;

        idspdrv_set_resamp_control(RESAMP_COEFF_RECTWIN, RESAMP_SELECT_CFA_V);
        idspdrv_config_cfa_prescale(p_flow_tbl->sect_header_addr[0][0], &param, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_cfa_prescaler.c", "idspdrv_config_cfa_prescale_v_coef", "test if ((resamp_select & RESAMP_SELECT_CFA_V) != 0u) true OK case", id_cnt++);
        idspdrv_set_resamp_control(RESAMP_COEFF_M2, RESAMP_SELECT_CFA_V);
        idspdrv_config_cfa_prescale(p_flow_tbl->sect_header_addr[0][0], &param, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_cfa_prescaler.c", "idspdrv_config_cfa_prescale_v_coef", "test ((resamp_control & RESAMP_COEFF_RECTWIN) != 0u) false OK case", id_cnt++);
        idspdrv_set_resamp_control(0u, RESAMP_SELECT_CFA_V);

        param.size_info.inputNumRows = 1080;
        param.size_info.inputWindowTop = 16384;
        idspdrv_config_cfa_prescale(p_flow_tbl->sect_header_addr[0][0], &param, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_cfa_prescaler.c", "idspdrv_config_cfa_prescale_v_coef", "test (bool_result == 2U) 1:1 true OK case", id_cnt++);
        param.size_info.inputNumRows = 2160;
        param.size_info.inputWindowTop = 0;

        idspdrv_config_cfa_prescale(p_flow_tbl->sect_header_addr[0][0], &param, 0, 65536);
        LOG_RVAL(pFile, IK_OK, "idspdrv_cfa_prescaler.c", "idspdrv_config_cfa_prescale", "test if (horizontalZoomFactor != 0U) true OK case", id_cnt++);

        idspdrv_config_cfa_prescale(p_flow_tbl->sect_header_addr[0][0], &param, 65536, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_cfa_prescaler.c", "idspdrv_config_cfa_prescale", "test if (verticalZoomFactor != 0U) true OK case", id_cnt++);

        param.outputSampleMode = IDSP_RGB_SAMPLE_MODE_BIN2;
        idspdrv_config_cfa_prescale(p_flow_tbl->sect_header_addr[0][0], &param, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_cfa_prescaler.c", "idspdrv_config_cfa_prescale", "test if (p_param->outputSampleMode == IDSP_RGB_SAMPLE_MODE_NORMAL) false OK case", id_cnt++);
        param.outputSampleMode = IDSP_RGB_SAMPLE_MODE_NORMAL;
    }
}

static void IKS_TestCovr_idspdrv_chroma_median(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        chroma_median_thresh_reg_t thresh = {0};
        extern void idspdrv_config_chroma_median(uintptr sect_header_addr,
                uint32 enable,
                uintptr k0123_table_addr,
                const chroma_median_thresh_reg_t *p_thresh);

        idspdrv_config_chroma_median(p_flow_tbl->sect_header_addr[0][0], 2, 0, &thresh);
        LOG_RVAL(pFile, IK_OK, "idspdrv_chroma_median.c", "idspdrv_config_chroma_median", "test (enable == 2U) true and (0u != k0123_table_addr) false OK case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_chroma_noise(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        chroma_noise_wide_t cfg = {0};
        extern void idspdrv_config_wide_chroma_noise_filter(uintptr sect_header_addr, const chroma_noise_wide_t *p_cfg);

        idspdrv_config_wide_chroma_noise_filter(p_flow_tbl->sect_header_addr[0][0], &cfg);
        LOG_RVAL(pFile, IK_OK, "idspdrv_chroma_noise.c", "idspdrv_config_wide_chroma_noise_filter", "test wide_tbl NULL ptr OK case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_chromatic_abberation(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        cawarp_cfg_t cfg = {0};
        active_size_info_t act = {0};
        extern void idspdrv_update_CAwarp_field_in_active_window(uintptr sect_header_addr,
                cawarp_cfg_t *p_cfg,
                const active_size_info_t *p_act);

        act.active_width = 1920;
        idspdrv_update_CAwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][0], &cfg, &act);
        LOG_RVAL(pFile, IK_OK, "idspdrv_chroma_noise.c", "idspdrv_update_ca_warp", "test (active_grid_array_width <= 32U) false OK case", id_cnt++);
        cfg.horz_pass_horz_grid_spacing_exponent = 2;

        act.active_height = 1080;
        idspdrv_update_CAwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][0], &cfg, &act);
        LOG_RVAL(pFile, IK_OK, "idspdrv_chroma_noise.c", "idspdrv_update_ca_warp", "test (active_grid_array_height <= 48U) false OK case", id_cnt++);
        cfg.horz_pass_vert_grid_spacing_exponent = 2;

        cfg.horz_pass_grid_array_width = 31;
        act.active_horz_start = 2048;
        idspdrv_update_CAwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][0], &cfg, &act);
        LOG_RVAL(pFile, IK_OK, "idspdrv_chroma_noise.c", "idspdrv_update_ca_warp", "test ((active_grid_array_horz_start + active_grid_array_width) <= grid_array_width) false OK case", id_cnt++);
        act.active_horz_start = 0;

        cfg.horz_pass_grid_array_height = 47;
        act.active_vert_start = 3072;
        idspdrv_update_CAwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][0], &cfg, &act);
        LOG_RVAL(pFile, IK_OK, "idspdrv_chroma_noise.c", "idspdrv_update_ca_warp", "test ((active_grid_array_horz_start + active_grid_array_width) <= grid_array_width) false OK case", id_cnt++);
        act.active_vert_start = 0;
    }

    {
        cawarp_intpl_info intpl_info = {0};
        cawarp_calib_info calib_info = {0};
        extern void idspdrv_cawarp_grid_interpolate(uintptr sect_header_addr,
                uint32 enable,
                const cawarp_intpl_info *p_intpl_info,
                int32 intpl_type,
                const cawarp_calib_info *p_calib_info);

        intpl_info.hor_grid_num = 1;
        intpl_info.ver_grid_num = 1;
        intpl_info.p_red_table_address = (int16 *)CawarpRedGrid;
        intpl_info.p_blue_table_address = (int16 *)CawarpBlueGrid;

        calib_info.p_cawarp_red = (grid_point_t *)CawarpRedGrid;
        calib_info.p_cawarp_blue = (grid_point_t *)CawarpBlueGrid;

        intpl_info.x_offset = -200;
        intpl_info.x_scale = 65536;
        idspdrv_cawarp_grid_interpolate(p_flow_tbl->sect_header_addr[0][0], 1, &intpl_info, INTERPOLATE_HOR, &calib_info);
        LOG_RVAL(pFile, IK_OK, "idspdrv_chroma_noise.c", "idspdrv_update_ca_warp", "test if (x < 0) true OK case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_circular_vignette(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        vignette_table_bins_t vig_table_bins = {0};
        vignette_center_t vig_center = {0};
        extern void idspdrv_load_vignette_table(uintptr sect_header_addr,
                                                uint8  enable,
                                                const vignette_table_bins_t *p_vig_table_bins,
                                                const vignette_center_t *p_vig_center,
                                                uintptr vignette_gain_table_daddr);

        idspdrv_load_vignette_table(p_flow_tbl->sect_header_addr[0][0], 0, &vig_table_bins, &vig_center, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_circular_vignette.c", "idspdrv_load_vignette_table", "test if(vignette_gain_table_daddr != 0u) false OK case", id_cnt++);
    }

    {
        vignette_table_bins_t vig_table_bins = {0};
        vignette_center_t vig_center = {0};
        extern void idspdrv_load_ce_fe_vignette_table(uintptr sect_header_addr,
                uint8  enable,
                const vignette_table_bins_t *p_vig_table_bins,
                const vignette_center_t *p_vig_center,
                uintptr vignette_gain_table_daddr);

        idspdrv_load_ce_fe_vignette_table(p_flow_tbl->sect_header_addr[0][2], 0, &vig_table_bins, &vig_center, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_circular_vignette.c", "idspdrv_load_ce_fe_vignette_table", "test if(vignette_gain_table_daddr != 0u) false OK case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_color_correct(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        extern void idspdrv_set_color_correction_output_table(uintptr sect_header_addr, uintptr out_lookup_dram_addr);

        idspdrv_set_color_correction_output_table(p_flow_tbl->sect_header_addr[0][0], 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_color_correct.c", "idspdrv_set_color_correction_output_table", "test if (out_lookup_dram_addr != 0u) false OK case", id_cnt++);
    }

    {
        extern void idspdrv_set_color_correction_matrix(uintptr sect_header_addr, uintptr matrix_dram_addr);

        idspdrv_set_color_correction_matrix(p_flow_tbl->sect_header_addr[0][0], 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_color_correct.c", "idspdrv_set_color_correction_matrix", "test if (matrix_dram_addr != 0u) false OK case", id_cnt++);
    }

    {
        color_correction_premul_t pre_cc_gain = {0};
        extern void idspdrv_set_pre_cc_gain(uintptr sect_header_addr, const color_correction_premul_t *p_pre_cc_gain);

        idspdrv_set_pre_cc_gain(p_flow_tbl->sect_header_addr[0][0], &pre_cc_gain);
        LOG_RVAL(pFile, IK_OK, "idspdrv_color_correct.c", "idspdrv_set_pre_cc_gain", "test pre_cc_gain line coverage case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_compress(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        lossy_comp_cfg_t cfg = {0};
        idsp_size_info_t info = {0};
        extern void idspdrv_config_hdr_decompression(uintptr sect_header_addr,
                uint8 idx,
                const lossy_comp_cfg_t *p_cfg,
                const idsp_size_info_t *p_info);

        idspdrv_config_hdr_decompression(p_flow_tbl->sect_header_addr[0][2], 6, &cfg, &info);
        LOG_RVAL(pFile, IK_OK, "idspdrv_compress.c", "idspdrv_set_hdr_decompression", "test if (exposure_index == 2u) false OK case", id_cnt++);

        cfg.bits_per_mantissa = 10;
        idspdrv_config_hdr_decompression(p_flow_tbl->sect_header_addr[0][0], 0, &cfg, &info);
        LOG_RVAL(pFile, IK_OK, "idspdrv_compress.c", "idspdrv_set_hdr_decompression", "test (p_cfg->bits_per_mantissa <= 9U) false OK case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_config_cv2a(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        IDSP_CMD cmd = {0};
        extern uint32 idspdrv_compose_idsp_cfg(IDSP_CMD *cmd,
                                               uintptr sect_sect_header_addr,
                                               uint32 sect_cfg_buf_sz,
                                               idsp_sect_id_t sec_id);

        idspdrv_compose_idsp_cfg(&cmd, p_flow_tbl->sect_header_addr[0][0], MAX_SEC_2_REG_CFG_SZ + IDSP_CONFIG_HDR_SIZE, IDSP_SEC_5);
        LOG_RVAL(pFile, IK_OK, "idspdrv_config_cv2a.c", "idspdrv_populate_sect_header", "test switch default OK case", id_cnt++);

        idspdrv_compose_idsp_cfg(&cmd, p_flow_tbl->sect_header_addr[0][0], IDSP_CONFIG_HDR_SIZE, IDSP_SEC_2);
        LOG_RVAL(pFile, IK_OK, "idspdrv_config_cv2a.c", "idspdrv_populate_sect_header", "test if(sect_cfg_buf_sz >= (IDSP_CONFIG_HDR_SIZE + MAX_SEC_2_REG_CFG_SZ)) false OK case", id_cnt++);
        idspdrv_compose_idsp_cfg(&cmd, p_flow_tbl->sect_header_addr[0][1], IDSP_CONFIG_HDR_SIZE, IDSP_SEC_3);
        LOG_RVAL(pFile, IK_OK, "idspdrv_config_cv2a.c", "idspdrv_populate_sect_header", "test if(sect_cfg_buf_sz >= (IDSP_CONFIG_HDR_SIZE + MAX_SEC_3_REG_CFG_SZ)) false OK case", id_cnt++);
        idspdrv_compose_idsp_cfg(&cmd, p_flow_tbl->sect_header_addr[0][2], IDSP_CONFIG_HDR_SIZE, IDSP_SEC_4);
        LOG_RVAL(pFile, IK_OK, "idspdrv_config_cv2a.c", "idspdrv_populate_sect_header", "test if(sect_cfg_buf_sz >= (IDSP_CONFIG_HDR_SIZE + MAX_SEC_4_REG_CFG_SZ)) false OK case", id_cnt++);
        idspdrv_compose_idsp_cfg(&cmd, p_flow_tbl->sect_header_addr[0][3], IDSP_CONFIG_HDR_SIZE, IDSP_SEC_18);
        LOG_RVAL(pFile, IK_OK, "idspdrv_config_cv2a.c", "idspdrv_populate_sect_header", "test if(sect_cfg_buf_sz >= (IDSP_CONFIG_HDR_SIZE + MAX_SEC_18_REG_CFG_SZ)) false OK case", id_cnt++);
    }

    {
        extern uintptr idspdrv_get_filter_addr(idspFilter_t filter_index);

        (void)idspdrv_get_filter_addr(FILTER_VIDEO_IN);
        LOG_RVAL(pFile, IK_OK, "idspdrv_config_cv2a.c", "idspdrv_get_filter_section", "test (filter_index >= FILTER_SEC2_START) false OK case", id_cnt++);
        (void)idspdrv_get_filter_addr(FILTER_VIDEO_IN_8);
        LOG_RVAL(pFile, IK_OK, "idspdrv_config_cv2a.c", "idspdrv_get_filter_section", "test (filter_index < FILTER_SEC19_START) false OK case", id_cnt++);
    }

    {
        extern uint32 idspdrv_get_cr_index(idsp_sect_id_t sect_id, idspFilter_t filter_index);

        (void)idspdrv_get_cr_index(IDSP_SEC_2, FILTER_VIDEO_IN);
        LOG_RVAL(pFile, IK_OK, "idspdrv_config_cv2a.c", "idspdrv_get_cr_index", "test if(sect_id == idspdrv_get_filter_section(index)) false OK case", id_cnt++);
        (void)idspdrv_get_cr_index(0xFFFFFFFFUL, FILTER_VIDEO_IN);
        LOG_RVAL(pFile, IK_OK, "idspdrv_config_cv2a.c", "idspdrv_get_cr_index", "test switch default OK case", id_cnt++);
    }

    {
        sect_config_slim_header_t *p_header = NULL;
        extern uintptr idspdrv_get_filter_addr_in_config_buf_with_header(uintptr sect_header_addr, idspFilter_t filter_index);

        (void)idspdrv_get_filter_addr_in_config_buf_with_header(0, FILTER_HDR_BLEND);
        LOG_RVAL(pFile, IK_OK, "idspdrv_config_cv2a.c", "idspdrv_get_filter_addr_in_config_buf_with_header", "test if (sect_header_addr == 0u) true OK case", id_cnt++);

        (void)idspdrv_get_filter_addr_in_config_buf_with_header(p_flow_tbl->sect_header_addr[0][0], FILTER_VIDEO_IN);
        LOG_RVAL(pFile, IK_OK, "idspdrv_config_cv2a.c", "idspdrv_get_filter_addr_in_config_buf_with_header", "test if(header.sect_id == (sec_id + 1u)) false OK case", id_cnt++);

        p_header = (sect_config_slim_header_t *)p_flow_tbl->sect_header_addr[0][0];
        p_header->config_region_cnt = 1;
        (void)idspdrv_get_filter_addr_in_config_buf_with_header(p_flow_tbl->sect_header_addr[0][0], FILTER_VIGNETTE);
        LOG_RVAL(pFile, IK_OK, "idspdrv_config_cv2a.c", "idspdrv_get_filter_addr_in_config_buf_with_header", "test if(cr_index < header.config_region_cnt) false OK case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_contrast_enhance(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        ce_coring_param_t ce_coring = {0};
        extern void idspdrv_config_ce_coring_config(uintptr sect_header_addr, const ce_coring_param_t* p_ce_coring);

        idspdrv_config_ce_coring_config(p_flow_tbl->sect_header_addr[0][0], &ce_coring);
        LOG_RVAL(pFile, IK_OK, "idspdrv_contrast_enhance.c", "idspdrv_config_ce_coring_config", "test if(0u != p_ce_coring->coring_table_addr) false OK case", id_cnt++);
    }

    {
        ce_output_table_t out_tbl_cfg = {0};
        extern void idspdrv_config_ce_output_table(uintptr sect_header_addr, const ce_output_table_t* p_out_tbl_cfg);

        idspdrv_config_ce_output_table(p_flow_tbl->sect_header_addr[0][0], &out_tbl_cfg);
        LOG_RVAL(pFile, IK_OK, "idspdrv_contrast_enhance.c", "idspdrv_config_ce_output_table", "test if(0u != p_out_tbl_cfg->output_table_addr) false OK case", id_cnt++);
    }

    {
        ce_output_table_t out_tbl_cfg = {0};
        extern void idspdrv_config_ce_boost_table(uintptr sect_header_addr, const ce_output_table_t* p_out_tbl_cfg);

        idspdrv_config_ce_boost_table(p_flow_tbl->sect_header_addr[0][0], &out_tbl_cfg);
        LOG_RVAL(pFile, IK_OK, "idspdrv_contrast_enhance.c", "idspdrv_config_ce_boost_table", "test if(0u != p_out_tbl_cfg->boost_table_addr) false OK case", id_cnt++);
    }

    {
        extern void idspdrv_config_ce_linear_table(uintptr sect_header_addr,
                uint32 linear_en,
                uintptr linear_table_addr);

        idspdrv_config_ce_linear_table(p_flow_tbl->sect_header_addr[0][0], 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_contrast_enhance.c", "idspdrv_config_ce_linear_table", "test if(0u != linear_table_addr) false OK case", id_cnt++);
    }

    {
        ce_fe_linear_param_t param = {0};
        extern void idspdrv_config_ce_fe_lookup_table(uintptr sect_header_addr,
                const ce_fe_linear_param_t *p_param,
                uintptr linear_table_addr,
                uintptr epsilon_table_addr);

        idspdrv_config_ce_fe_lookup_table(p_flow_tbl->sect_header_addr[0][2], &param, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_contrast_enhance.c", "idspdrv_config_ce_fe_lookup_table", "test if(0u != linear_table_addr) false OK case, test if(0u != epsilon_table_addr) false OK case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_digital_gain(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        rgb_gain_adjust_t hdr_dgain = {0};
        extern void idspdrv_set_hdr_dgain(uintptr sect_header_addr,
                                          uint8 idx,
                                          const rgb_gain_adjust_t* p_hdr_dgain);

        idspdrv_set_hdr_dgain(p_flow_tbl->sect_header_addr[0][2], 6, &hdr_dgain);
        LOG_RVAL(pFile, IK_OK, "idspdrv_digital_gain.c", "idspdrv_set_hdr_dgain", "test idx out of range OK case", id_cnt++);
    }

    {
        frontend_dgain_saturation_t hdr_dgain_sat = {0};
        extern void idspdrv_set_hdr_dgain_saturation(uintptr sect_header_addr,
                uint8 idx,
                const frontend_dgain_saturation_t *p_hdr_dgain_sat);

        idspdrv_set_hdr_dgain_saturation(p_flow_tbl->sect_header_addr[0][2], 6, &hdr_dgain_sat);
        LOG_RVAL(pFile, IK_OK, "idspdrv_digital_gain.c", "idspdrv_set_hdr_dgain_saturation", "test idx out of range OK case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_dma(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        extern void idspdrv_set_reg(uintptr cntrlreg_base_addr,
                                    uint32 offset16,
                                    uint32 start_bit,
                                    uint32 num_bits,
                                    data_unit_t data_unit,
                                    uint32 index,
                                    uint32 value);

        idspdrv_set_reg(p_flow_tbl->sect_header_addr[0][0], 0, 5, 20, WORD_UNIT, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_dma.c", "idspdrv_setbits16", "test if((p+n) <= 16u) false OK case", id_cnt++);

        idspdrv_set_reg(p_flow_tbl->sect_header_addr[0][0], 0, 0, 5, BYTE_UNIT, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_dma.c", "idspdrv_setbits8", "test if((p+n) <= 8u) true OK case", id_cnt++);
        idspdrv_set_reg(p_flow_tbl->sect_header_addr[0][0], 0, 5, 5, BYTE_UNIT, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_dma.c", "idspdrv_setbits8", "test if((p+n) <= 8u) false OK case", id_cnt++);

        idspdrv_set_reg(p_flow_tbl->sect_header_addr[0][0], 0, 5, 35, DOUBLE_UNIT, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_dma.c", "idspdrv_setbits32", "test if((p+n) <= 32u) false OK case", id_cnt++);

        idspdrv_set_reg(p_flow_tbl->sect_header_addr[0][0], 0, 0, 5, BIT_UNIT, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_dma.c", "idspdrv_set_reg", "test if(data_unit == BIT_UNIT) true OK case", id_cnt++);
        idspdrv_set_reg(p_flow_tbl->sect_header_addr[0][0], 0, 5, 5, BIT_UNIT, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_dma.c", "idspdrv_set_reg", "test if ((bit_offset + num_bits) > 8u) true OK case", id_cnt++);
    }

    {
        extern uint32 idspdrv_get_reg(uintptr cntrlreg_base_addr,
                                      uint32 offset16,
                                      uint32 start_bit,
                                      uint32 num_bits,
                                      data_unit_t data_unit,
                                      uint32 index);

        idspdrv_get_reg(p_flow_tbl->sect_header_addr[0][0], 0, 5, 5, BIT_UNIT, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_dma.c", "idspdrv_get_reg", "test if(data_unit == BIT_UNIT) true OK case", id_cnt++);

        idspdrv_get_reg(p_flow_tbl->sect_header_addr[0][0], 0, 5, 5, BYTE_UNIT, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_dma.c", "idspdrv_get_reg", "test if(data_unit == BYTE_UNIT) true OK case", id_cnt++);

        idspdrv_get_reg(p_flow_tbl->sect_header_addr[0][0], 0, 5, 35, DOUBLE_UNIT, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_dma.c", "idspdrv_get_reg", "test if (data_unit == DOUBLE_UNIT) true OK case", id_cnt++);
    }

    {
        extern void idspdrv_save_filter(const void *caddr, uintptr daddr, uint32 num_bytes);

        idspdrv_save_filter(NULL, 0, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_dma.c", "idspdrv_save_filter", "test NULL ptr OK case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_dynamic_bad_pixel(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        bad_pixel_correct_cfg_t cfg = {0};
        extern void idspdrv_config_cfa_bad_pixel_correction(uintptr sect_header_addr, const bad_pixel_correct_cfg_t *p_cfg);

        idspdrv_config_cfa_bad_pixel_correction(p_flow_tbl->sect_header_addr[0][0], &cfg);
        LOG_RVAL(pFile, IK_OK, "idspdrv_dynamic_bad_pixel.c", "idspdrv_config_cfa_bad_pixel_correction", "test hot_pixel_thresh_addr and dark_pixel_thresh_addr == 0 OK case", id_cnt++);
    }

    {
        bad_pixel_correct_cfg_t cfg = {0};
        extern void idspdrv_config_ce_cfa_bad_pixel_correction(uintptr sect_header_addr, const bad_pixel_correct_cfg_t *p_cfg);

        cfg.dynamic_bad_pixel_detection_mode = 1;
        idspdrv_config_ce_cfa_bad_pixel_correction(p_flow_tbl->sect_header_addr[0][2], &cfg);
        LOG_RVAL(pFile, IK_OK, "idspdrv_dynamic_bad_pixel.c", "idspdrv_config_ce_cfa_bad_pixel_correction", "test if (p_cfg->dynamic_bad_pixel_detection_mode == 0U) false OK case", id_cnt++);

        cfg.hot_pixel_thresh_addr = (uintptr)FPNMap;
        cfg.dark_pixel_thresh_addr = (uintptr)FPNMap;
        idspdrv_config_ce_cfa_bad_pixel_correction(p_flow_tbl->sect_header_addr[0][2], &cfg);
        LOG_RVAL(pFile, IK_OK, "idspdrv_dynamic_bad_pixel.c", "idspdrv_config_ce_cfa_bad_pixel_correction", "test hot_pixel_thresh_addr and dark_pixel_thresh_addr != 0 OK case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_global()
{
    UINT32 Rval = IK_OK;

    {
        extern uint32 idspdrv_prepare_global_memory(uintptr mem_addr);

        Rval = idspdrv_prepare_global_memory(0);
        LOG_RVAL(pFile, Rval, "idspdrv_global.c", "idspdrv_prepare_global_memory", "test if (mem_addr == 0U) true NG case", id_cnt++);
    }

    {
        idspdrv_global_t *p_global;
        extern void idspdrv_init_global(void);
        extern uint32 idspdrv_get_global(idspdrv_global_t **p_idspdrv);

        idspdrv_init_global();
        Rval = idspdrv_get_global(&p_global);
        LOG_RVAL(pFile, Rval, "idspdrv_global.c", "idspdrv_get_global", "test if(idspdrv_global == NULL) true NG case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_luma_sharpen(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        luma_sharpening_FIR_config_t cfg = {0};
        extern void idspdrv_config_luma_sharpen_FIR(uintptr sect_header_addr, const luma_sharpening_FIR_config_t *p_cfg);

        cfg.enable_FIR1 = 2;
        idspdrv_config_luma_sharpen_FIR(p_flow_tbl->sect_header_addr[0][0], &cfg);
        LOG_RVAL(pFile, IK_OK, "idspdrv_luma_sharpen.c", "idspdrv_config_luma_sharpen_FIR", "test if (p_cfg->enable_FIR1 != 2U) false OK case", id_cnt++);
        cfg.enable_FIR1 = 1;
        idspdrv_config_luma_sharpen_FIR(p_flow_tbl->sect_header_addr[0][0], &cfg);
        LOG_RVAL(pFile, IK_OK, "idspdrv_luma_sharpen.c", "idspdrv_config_luma_sharpen_FIR", "test coeff_FIR1_addr coeff_FIR2_addr and coring_table_addr NULL OK case", id_cnt++);
    }

    // test idspdrv_find_shift_and_8bit_mantissa
    {
        uint8 shift;
        uint8 mantissa;
        extern void idspdrv_find_shift_and_8bit_mantissa(uint32 diff, uint8 *shift, uint8 *mantissa);

        idspdrv_find_shift_and_8bit_mantissa(1, &shift, &mantissa);
        LOG_RVAL(pFile, IK_OK, "idspdrv_luma_sharpen.c", "idspdrv_find_shift_and_8bit_mantissa", "test s > 7 false OK case", id_cnt++);
    }

    {
        luma_sharpening_level_control_t cfg = {0};
        extern void idspdrv_config_luma_sharpen_level_control(uintptr sect_header_addr, const luma_sharpening_level_control_t *p_cfg);

        idspdrv_config_luma_sharpen_level_control(p_flow_tbl->sect_header_addr[0][0], &cfg);
        LOG_RVAL(pFile, IK_OK, "idspdrv_luma_sharpen.c", "idspdrv_config_luma_sharpen_level_control", "test p_cfg->luma_shp_lvl_select is 0 OK case", id_cnt++);

        cfg.luma_shp_lvl_select = 3;
        idspdrv_config_luma_sharpen_level_control(p_flow_tbl->sect_header_addr[0][0], &cfg);
        LOG_RVAL(pFile, IK_OK, "idspdrv_luma_sharpen.c", "idspdrv_config_luma_sharpen_level_control", "test p_cfg->luma_shp_lvl_select is 3 OK case", id_cnt++);

        cfg.luma_shp_lvl_select = 20;
        idspdrv_config_luma_sharpen_level_control(p_flow_tbl->sect_header_addr[0][0], &cfg);
        LOG_RVAL(pFile, IK_OK, "idspdrv_luma_sharpen.c", "idspdrv_config_luma_sharpen_level_control", "test default OK case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_rgbir(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        ir_cfa_convert_cfg_t cfg = {0};
        extern void idspdrv_config_cfa_irconvert_ircorrect(uintptr sect_header_addr, const ir_cfa_convert_cfg_t *p_cfg);

        cfg.ir_cfa_convert_select = 5;
        idspdrv_config_cfa_irconvert_ircorrect(p_flow_tbl->sect_header_addr[0][0], &cfg);
        LOG_RVAL(pFile, IK_OK, "idspdrv_rgbir.c", "idspdrv_config_cfa_irconvert_ircorrect", "test if(p_cfg->ir_cfa_convert_select <= 3U) false OK case", id_cnt++);
    }

    {
        ir_cfa_convert_cfg_t cfg = {0};
        extern void idspdrv_config_ce_fe_irconvert_ircorrect(uintptr sect_header_addr, const ir_cfa_convert_cfg_t *p_cfg);

        cfg.ir_cfa_convert_select = 5;
        idspdrv_config_ce_fe_irconvert_ircorrect(p_flow_tbl->sect_header_addr[0][2], &cfg);
        LOG_RVAL(pFile, IK_OK, "idspdrv_rgbir.c", "idspdrv_config_ce_fe_irconvert_ircorrect", "test if(p_cfg->ir_cfa_convert_select <= 3U) false OK case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_warp(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        hwarp_cfg_t cfg = {0};
        extern void idspdrv_config_horizontal_warp_chroma_format(uintptr sect_header_addr, const hwarp_cfg_t *p_cfg);

        cfg.input_chroma_format = 1;
        idspdrv_config_horizontal_warp_chroma_format(p_flow_tbl->sect_header_addr[0][0], &cfg);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_config_horizontal_warp_chroma_format", "test (p_cfg->input_chroma_format == 0U) false OK case", id_cnt++);

        cfg.warp_enable = 1;
        idspdrv_config_horizontal_warp_chroma_format(p_flow_tbl->sect_header_addr[0][0], &cfg);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_config_horizontal_warp_chroma_format", "test if (p_cfg->warp_enable != 2U) true OK case", id_cnt++);

        cfg.output_luma_chroma_ratio = 1;
        idspdrv_config_horizontal_warp_chroma_format(p_flow_tbl->sect_header_addr[0][0], &cfg);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_config_horizontal_warp_chroma_format", "test if (p_cfg->output_luma_chroma_ratio == 1U) true OK case", id_cnt++);
    }

    {
        extern void idspdrv_config_vertical_warp_chroma_format(uintptr sect_header_addr,
                uint8 input_luma_chroma_ratio,
                uint8 output_chroma_format);

        idspdrv_config_vertical_warp_chroma_format(p_flow_tbl->sect_header_addr[0][1], 1, 1);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_config_vertical_warp_chroma_format", "test if (input_luma_chroma_ratio == 1U) true OK case", id_cnt++);
    }

    {
        set_warp_control_t cmd_buf = {0};
        extern void idspdrv_update_hwarp_field(uintptr sect_header_addr,
                                               const set_warp_control_t *cmd_buf,
                                               uint32 tile_mode_enable);

        idspdrv_update_hwarp_field(p_flow_tbl->sect_header_addr[0][0], &cmd_buf, 1);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_hwarp_field", "test if(tile_mode_enable == 0u) false OK case", id_cnt++);
    }

    {
        set_warp_control_t cmd_buf = {0};
        active_size_info_t act = {0};
        extern void idspdrv_update_hwarp_field_in_active_window(uintptr sect_header_addr,
                set_warp_control_t *cmd_buf,
                const active_size_info_t *p_act,
                uint32 tile_mode_enable);

        act.active_width = 1920;
        idspdrv_update_hwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][0], &cmd_buf, &act, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_hwarp_field_in_active_window", "test (active_grid_array_width <= 128U) false OK case", id_cnt++);
        cmd_buf.horz_grid_spacing_exponent = 2;

        act.active_height = 1080;
        idspdrv_update_hwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][0], &cmd_buf, &act, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_hwarp_field_in_active_window", "test (active_grid_array_height <= 96U) false OK case", id_cnt++);
        cmd_buf.vert_grid_spacing_exponent = 2;

        cmd_buf.grid_array_width = 127;
        act.active_horz_start = 4096;
        idspdrv_update_hwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][0], &cmd_buf, &act, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_hwarp_field_in_active_window", "test ((active_grid_array_horz_start + active_grid_array_width) <= grid_array_width) false OK case", id_cnt++);
        act.active_horz_start = 0;

        cmd_buf.grid_array_height = 95;
        act.active_vert_start = 3072;
        idspdrv_update_hwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][0], &cmd_buf, &act, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_hwarp_field_in_active_window", "test ((active_grid_array_vert_start + active_grid_array_height) <= grid_array_height)) false OK case", id_cnt++);
        act.active_vert_start = 0;

        idspdrv_update_hwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][0], &cmd_buf, &act, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_hwarp_field_in_active_window", "test if ((grid_array_width == 128U) && (grid_array_height == 96U) && (active_grid_array_horz_start == 0U) && (active_grid_array_vert_start == 0U)) true OK case", id_cnt++);

        cmd_buf.grid_array_height = 80;
        idspdrv_update_hwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][0], &cmd_buf, &act, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_hwarp_field_in_active_window", "test (grid_array_height == 96U) false OK case", id_cnt++);
        cmd_buf.grid_array_height = 95;

        act.active_horz_start = 64;
        idspdrv_update_hwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][0], &cmd_buf, &act, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_hwarp_field_in_active_window", "test (active_grid_array_horz_start == 0U) false OK case", id_cnt++);
        act.active_horz_start = 0;

        act.active_vert_start = 64;
        idspdrv_update_hwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][0], &cmd_buf, &act, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_hwarp_field_in_active_window", "test (active_grid_array_vert_start == 0U) false OK case", id_cnt++);
        act.active_vert_start = 0;
    }

    {
        set_warp_control_t cmd_buf = {0};
        active_size_info_t act = {0};
        extern void idspdrv_update_vwarp_field_in_active_window(uintptr sect_header_addr,
                const set_warp_control_t *cmd_buf,
                const active_size_info_t *p_act);

        act.active_width = 1920;
        idspdrv_update_vwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][1], &cmd_buf, &act);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_vwarp_field_in_active_window", "test (active_grid_array_width <= 128U) false OK case", id_cnt++);
        cmd_buf.vert_warp_horz_grid_spacing_exponent = 2;

        act.active_height = 1080;
        idspdrv_update_vwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][1], &cmd_buf, &act);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_vwarp_field_in_active_window", "test (active_grid_array_height <= 96U) false OK case", id_cnt++);
        cmd_buf.vert_warp_vert_grid_spacing_exponent = 2;

        cmd_buf.vert_warp_grid_array_width = 127;
        act.active_horz_start = 4096;
        idspdrv_update_vwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][1], &cmd_buf, &act);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_vwarp_field_in_active_window", "test ((active_grid_array_horz_start + active_grid_array_width) <= grid_array_width) false OK case", id_cnt++);
        act.active_horz_start = 0;

        cmd_buf.vert_warp_grid_array_height = 95;
        act.active_vert_start = 3072;
        idspdrv_update_vwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][1], &cmd_buf, &act);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_vwarp_field_in_active_window", "test ((active_grid_array_vert_start + active_grid_array_height) <= grid_array_height)) false OK case", id_cnt++);
        act.active_vert_start = 0;

        idspdrv_update_vwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][1], &cmd_buf, &act);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_vwarp_field_in_active_window", "test if ((grid_array_width == 128U) && (grid_array_height == 96U) && (active_grid_array_horz_start == 0U) && (active_grid_array_vert_start == 0U)) true OK case", id_cnt++);

        cmd_buf.vert_warp_grid_array_height = 80;
        idspdrv_update_vwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][1], &cmd_buf, &act);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_vwarp_field_in_active_window", "test (grid_array_height == 96U) false OK case", id_cnt++);
        cmd_buf.vert_warp_grid_array_height = 95;

        act.active_horz_start = 64;
        idspdrv_update_vwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][1], &cmd_buf, &act);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_vwarp_field_in_active_window", "test (active_grid_array_horz_start == 0U) false OK case", id_cnt++);
        act.active_horz_start = 0;

        act.active_vert_start = 64;
        idspdrv_update_vwarp_field_in_active_window(p_flow_tbl->sect_header_addr[0][1], &cmd_buf, &act);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_update_vwarp_field_in_active_window", "test (active_grid_array_vert_start == 0U) false OK case", id_cnt++);
        act.active_vert_start = 0;
    }

    {
        warp_intpl_info intpl_info = {0};
        warp_calib_info calib_info = {0};
        extern void idspdrv_warp_grid_interpolate(uintptr sect_header_addr,
                uint32 enable,
                warp_intpl_info *p_intpl_info,
                int32 intpl_type,
                const warp_calib_info *p_calib_info);

        intpl_info.hor_grid_num = 10;
        intpl_info.ver_grid_num = 10;
        intpl_info.map_pitch = 10;
        intpl_info.calib_2_warp_input_scale = 68108866 << 4;

        calib_info.hor_grid_num = 20;
        calib_info.ver_grid_num = 20;
        calib_info.tile_width_exp = 3;
        calib_info.tile_height_exp = 3;
        calib_info.pwarp = (grid_point_t *)WarpGrid;
        memset(WarpGrid, 255, sizeof(4*256*192));
        idspdrv_warp_grid_interpolate(p_flow_tbl->sect_header_addr[0][1], 1, &intpl_info, INTERPOLATE_VER, &calib_info);
        LOG_RVAL(pFile, IK_OK, "idspdrv_warp.c", "idspdrv_warp_grid_interpolate", "test if(iks_abs_uint32((int32)warp_vert_val_chk) > warp_vert_max_val) true OK case", id_cnt++);
    }
}

static void IKS_TestCovr_idspdrv_yuv_resamp(const AMBA_IK_MODE_CFG_s *pMode)
{
    uintptr addr = 0;
    idsp_flow_tbl_t *p_flow_tbl = NULL;
    idsp_yuv_resamp_params_t resamp = {0};

    extern void idspdrv_config_generic_yuv_resamp(uintptr sect_header_addr,
            idspdrv_resampler_t resampler,
            idsp_yuv_resamp_params_t *resamp);

    iks_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
    memcpy(&p_flow_tbl, &addr, sizeof(void *));

    {
        extern void idspdrv_set_resamp_control(uint16 resamp_ctrl, uint16 resamp_select);

        resamp.inputNumLumaRows = 2160;
        resamp.outputNumLumaRows = 1080;

        resamp.inputNumLumaCols = 3840;
        resamp.outputNumLumaCols = 1920;
        idspdrv_set_resamp_control(RESAMP_COEFF_RECTWIN, RESAMP_SELECT_MAIN);
        idspdrv_config_generic_yuv_resamp(p_flow_tbl->sect_header_addr[0][0], RESAMP_MAIN, &resamp);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_config_horizontal_Y_resamp", "test if (((resamp_select & RESAMP_SELECT_MAIN) != 0u) && ((resamp_control & RESAMP_COEFF_RECTWIN) != 0u)) true OK case", id_cnt++);

        idspdrv_set_resamp_control(RESAMP_COEFF_RECTWIN, RESAMP_SELECT_MAIN_V);
        idspdrv_config_generic_yuv_resamp(p_flow_tbl->sect_header_addr[0][0], RESAMP_MAIN, &resamp);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_config_horizontal_Y_resamp", "test ((resamp_select & RESAMP_SELECT_MAIN) != 0u) false OK case", id_cnt++);

        resamp.inputNumLumaCols = 1920;
        resamp.outputNumLumaCols = 8000;
        idspdrv_config_generic_yuv_resamp(p_flow_tbl->sect_header_addr[0][0], RESAMP_MAIN, &resamp);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_config_horizontal_Y_resamp", "test if (horizontalLumaPhaseIncrement < 2048u) true OK case", id_cnt++);

        idspdrv_set_resamp_control(RESAMP_COEFF_LP_STRONG, RESAMP_SELECT_MAIN_V);
        idspdrv_config_generic_yuv_resamp(p_flow_tbl->sect_header_addr[0][0], RESAMP_MAIN, &resamp);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_config_vertical_Y_resamp", "test ((resamp_control & RESAMP_COEFF_RECTWIN) != 0u) false OK case", id_cnt++);

        resamp.inputNumLumaRows = 1080;
        resamp.outputNumLumaRows = 6000;
        idspdrv_config_generic_yuv_resamp(p_flow_tbl->sect_header_addr[0][0], RESAMP_MAIN, &resamp);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_config_vertical_Y_resamp", "test if (verticalLumaPhaseIncrement < 2048u) true OK case", id_cnt++);

        resamp.inputNumLumaRows = 1080;
        resamp.outputNumLumaRows = 2160;
        idspdrv_config_generic_yuv_resamp(p_flow_tbl->sect_header_addr[0][0], RESAMP_MAIN, &resamp);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_config_vertical_Y_resamp", "test /* up-sampling: 4-tap */ OK case", id_cnt++);
    }

    {
        extern uint32 idspdrv_calc_M(uint32 phase_inc_q13, int32 coeff_adj);

        idspdrv_calc_M(6000, -1);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_calc_M", "test if (M < 16u) OK case", id_cnt++);
    }

    {
        extern void idspdrv_enable_resampler(uintptr sect_header_addr,
                                             idspdrv_resampler_t resampler,
                                             uint8 vert_enable,
                                             uint8 horz_enable);

        idspdrv_enable_resampler(p_flow_tbl->sect_header_addr[0][0], RESAMP_MAIN, 1, 1);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_enable_resampler", "test (resampler == RESAMP_MAIN) true OK case", id_cnt++);
        idspdrv_enable_resampler(p_flow_tbl->sect_header_addr[0][0], RESAMP_MAIN_LUMA, 1, 1);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_enable_resampler", "test (resampler == RESAMP_MAIN_LUMA) true OK case", id_cnt++);
        idspdrv_enable_resampler(p_flow_tbl->sect_header_addr[0][0], RESAMP_MAIN_CHROMA, 1, 1);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_enable_resampler", "test (resampler == RESAMP_MAIN_CHROMA) true OK case", id_cnt++);
        idspdrv_enable_resampler(p_flow_tbl->sect_header_addr[0][1], RESAMP_VERTICAL_WARP, 1, 1);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_enable_resampler", "test (resampler == RESAMP_VERTICAL_WARP) true OK case", id_cnt++);
        idspdrv_enable_resampler(p_flow_tbl->sect_header_addr[0][0], RESAMP_HORIZONTAL_CHROMA, 1, 1);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_enable_resampler", "test (resampler == RESAMP_HORIZONTAL_CHROMA) true OK case", id_cnt++);
        idspdrv_enable_resampler(p_flow_tbl->sect_header_addr[0][0], 7, 1, 1);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_enable_resampler", "test else true OK case", id_cnt++);
    }

    {
        extern void idspdrv_config_chroma_horizontal_upsampler(uintptr sect_header_addr, const idsp_yuv_resamp_params_t *resamp);

        idspdrv_config_chroma_horizontal_upsampler(p_flow_tbl->sect_header_addr[0][0], &resamp);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_config_chroma_horizontal_upsampler", "test if (horizontalChromaZoomFactor != 0u) false OK case", id_cnt++);

        resamp.inputNumLumaCols = 3840;
        resamp.outputNumLumaCols = 1920;
        idspdrv_config_chroma_horizontal_upsampler(p_flow_tbl->sect_header_addr[0][0], &resamp);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_config_chroma_horizontal_upsampler", "test else /* downsampling */ OK case", id_cnt++);

        resamp.inputNumLumaCols = 1920;
        resamp.outputNumLumaCols = 1920;
        resamp.inputWindowLeft = 0x1000;
        idspdrv_config_chroma_horizontal_upsampler(p_flow_tbl->sect_header_addr[0][0], &resamp);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_config_chroma_horizontal_upsampler", "test (resamp->inputWindowLeft > 0x800u) OK case", id_cnt++);
        resamp.inputWindowLeft = 0;
    }

    {
        uint16 resamp_ctrl;
        uint16 resamp_select;
        extern void idspdrv_get_resamp_control(idspdrv_resampler_t resampler,
                                               uint16* resamp_ctrl,
                                               uint16* resamp_select);

        idspdrv_get_resamp_control(RESAMP_CFAPRE, NULL, &resamp_select);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_get_resamp_control", "test NULL ptr OK case", id_cnt++);
        idspdrv_get_resamp_control(RESAMP_CFAPRE, &resamp_ctrl, NULL);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_get_resamp_control", "test NULL ptr OK case", id_cnt++);

        idspdrv_get_resamp_control(RESAMP_CFAPRE, &resamp_ctrl, &resamp_select);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_get_resamp_control", "test case RESAMP_CFAPRE OK case", id_cnt++);
        idspdrv_get_resamp_control(7, &resamp_ctrl, &resamp_select);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_get_resamp_control", "test case default OK case", id_cnt++);
    }

    {
        vert_yuv_resamp_param_t param = {0};
        extern void idspdrv_config_vertical_resampler(uintptr sect_header_addr,
                vert_yuv_resamp_param_t *p_param,
                uint32 verticalZoomFactor);

        param.size_info.inputNumLumaRows = 1920;
        param.size_info.outputNumLumaRows = 3840;
        idspdrv_config_vertical_resampler(p_flow_tbl->sect_header_addr[0][1], &param, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_config_vertical_resampler", "test if (verticalZoomFactor != 0u) false OK case", id_cnt++);

        param.size_info.inputNumLumaRows = 0;
        idspdrv_config_vertical_resampler(p_flow_tbl->sect_header_addr[0][1], &param, 65536);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_config_vertical_resampler", "test if(p_param->size_info.inputNumLumaRows==0u) true OK case", id_cnt++);

        param.outputSampleMode = YUV422_HCOSITED_PROGRESSIVE;
        idspdrv_config_vertical_resampler(p_flow_tbl->sect_header_addr[0][1], &param, 65536);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_config_vertical_resampler", "test if (p_param->outputSampleMode <= YUV420_HCOSITED_BOT_FIELD) false OK case", id_cnt++);

        param.size_info.inputWindowTopLuma = 1000;
        idspdrv_config_vertical_resampler(p_flow_tbl->sect_header_addr[0][1], &param, 65536);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_config_vertical_resampler", "test if (verticalLumaInitialPhase < (0x2000u - INITIAL_PHASE_ROUNDING)) false OK case", id_cnt++);
        param.size_info.inputWindowTopLuma = 0;

        param.size_info.inputNumLumaRows = 3840;
        param.size_info.outputNumLumaRows = 1920;
        idspdrv_config_vertical_resampler(p_flow_tbl->sect_header_addr[0][1], &param, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_config_vertical_resampler", "test if(verticalLumaPhaseIncrement > 8192u) true OK case", id_cnt++);

        param.size_info.inputNumLumaRows = 1920;
        param.size_info.outputNumLumaRows = 8000;
        idspdrv_config_vertical_resampler(p_flow_tbl->sect_header_addr[0][1], &param, 0);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_config_vertical_resampler", "test if (verticalLumaPhaseIncrement < 2048u) true OK case", id_cnt++);

        param.size_info.inputWindowTopChroma = 1000;
        idspdrv_config_vertical_resampler(p_flow_tbl->sect_header_addr[0][1], &param, 65536);
        LOG_RVAL(pFile, IK_OK, "idspdrv_yuv_resamp.c", "idspdrv_config_vertical_resampler", "test if (verticalChromaInitialPhase < (0x2000u - INITIAL_PHASE_ROUNDING)) false OK case", id_cnt++);
        param.size_info.inputWindowTopChroma = 0;
    }
}

INT32 iks_ctest_idspdrv_main(void)
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

    pFile = fopen("data/iks_ctest_idspdrv.txt", "w");

    // init IK
    _load_idsp_default_binary();
    Ability.Pipe = AMBA_IK_PIPE_VIDEO;
    Ability.VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_3;

    _Init_Image_Kernel(&Ability);
    _Init_Image_Kernel_Core();

    Mode.ContextId = 0;
    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    // test idspdrv_aaa.c
    IKS_TestCovr_idspdrv_aaa(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_black_level.c
    IKS_TestCovr_idspdrv_black_level(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_cfa_prescaler.c
    IKS_TestCovr_idspdrv_cfa_prescaler(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_chroma_median.c
    IKS_TestCovr_idspdrv_chroma_median(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_chroma_noise.c
    IKS_TestCovr_idspdrv_chroma_noise(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_chromatic_abberation.c
    IKS_TestCovr_idspdrv_chromatic_abberation(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_circular_vignette.c
    IKS_TestCovr_idspdrv_circular_vignette(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_color_correct.c
    IKS_TestCovr_idspdrv_color_correct(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_compress.c
    IKS_TestCovr_idspdrv_compress(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_config_cv2a.c
    IKS_TestCovr_idspdrv_config_cv2a(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_contrast_enhance.c
    IKS_TestCovr_idspdrv_contrast_enhance(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_digital_gain.c
    IKS_TestCovr_idspdrv_digital_gain(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_dma.c
    IKS_TestCovr_idspdrv_dma(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_dynamic_bad_pixel.c
    IKS_TestCovr_idspdrv_dynamic_bad_pixel(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_global.c
    IKS_TestCovr_idspdrv_global();
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_luma_sharpen.c
    IKS_TestCovr_idspdrv_luma_sharpen(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_rgbir.c
    IKS_TestCovr_idspdrv_rgbir(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_warp.c
    IKS_TestCovr_idspdrv_warp(&Mode);
    _Reset_Image_Kernel(&Ability);

    // test idspdrv_yuv_resamp.c
    IKS_TestCovr_idspdrv_yuv_resamp(&Mode);
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

