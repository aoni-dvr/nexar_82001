#include "test_ik_global.h"


#define ALIGN_N(x_, n_)   ((((x_)+((n_)-1)) & ~((n_)-1)))

static UINT8 ik_working_buffer[5<<21];
static UINT8 am_dump_buffer[5<<20];
static void *pBinDataAddr;
FILE *pFile;
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

static void IK_TestCovr_ImgConfigComponentIF(const AMBA_IK_MODE_CFG_s *pMode)
{
    UINT32 Rval = IK_OK;
    ik_ability_t ability = {0};

    {
        Rval = img_cfg_query_flow_control_memory_size(NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_query_flow_control_memory_size", "test null ptr fail case", id_cnt++);
    }

    {
        Rval = img_cfg_query_flow_tbl_memory_size(NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_query_flow_tbl_memory_size", "test null ptr fail case", id_cnt++);
    }

    {
        Rval = img_cfg_query_flow_tbl_list_memory_size(NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_query_flow_tbl_list_memory_size", "test null ptr fail case", id_cnt++);
    }

    {
        Rval = img_cfg_query_crc_data_memory_size(NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_query_crc_data_memory_size", "test null ptr fail case", id_cnt++);
    }

    {
        size_t flow_ctrl_size;
        idsp_ik_flow_ctrl_t *p_idsp_flow_ctrl;

        img_cfg_query_flow_control_memory_size(&flow_ctrl_size);
        Rval = img_cfg_prepare_flow_control_memory(NULL, 0, 0, flow_ctrl_size, AMBA_IK_ARCH_HARD_INIT);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_prepare_flow_control_memory", "test p_flow_mem null ptr fail case", id_cnt++);
        Rval = img_cfg_prepare_flow_control_memory(&p_idsp_flow_ctrl, 0, 100, flow_ctrl_size, AMBA_IK_ARCH_HARD_INIT);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_prepare_flow_control_memory", "test mem_addr null ptr fail case", id_cnt++);
        Rval = img_cfg_prepare_flow_control_memory(&p_idsp_flow_ctrl, 0x12345678, 0, flow_ctrl_size, AMBA_IK_ARCH_HARD_INIT);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_prepare_flow_control_memory", "test size null ptr fail case", id_cnt++);
        Rval = img_cfg_prepare_flow_control_memory(&p_idsp_flow_ctrl, 0x12345678, 100, flow_ctrl_size, AMBA_IK_ARCH_HARD_INIT);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_prepare_flow_control_memory", "test (flow_ctrl_size < size) fail case", id_cnt++);
    }

    {
        size_t flow_tbl_size;
        amba_ik_flow_tables_t *p_ik_flow_tbl;

        img_cfg_query_flow_tbl_memory_size(&flow_tbl_size);
        Rval = img_cfg_prepare_flow_tbl_memory(NULL, 0, 0, flow_tbl_size, AMBA_IK_ARCH_HARD_INIT);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_prepare_flow_tbl_memory", "test p_flow_tbl_mem null ptr fail case", id_cnt++);
        Rval = img_cfg_prepare_flow_tbl_memory(&p_ik_flow_tbl, 0, 100, flow_tbl_size, AMBA_IK_ARCH_HARD_INIT);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_prepare_flow_tbl_memory", "test mem_addr null ptr fail case", id_cnt++);
        Rval = img_cfg_prepare_flow_tbl_memory(&p_ik_flow_tbl, 0x12345678, 0, flow_tbl_size, AMBA_IK_ARCH_HARD_INIT);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_prepare_flow_tbl_memory", "test size null ptr fail case", id_cnt++);
        Rval = img_cfg_prepare_flow_tbl_memory(&p_ik_flow_tbl, 0x12345678, 100, flow_tbl_size, AMBA_IK_ARCH_HARD_INIT);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_prepare_flow_tbl_memory", "test (flow_tbl_size < size) fail case", id_cnt++);
    }

    {
        size_t flow_tbl_list_size;
        amba_ik_flow_tables_list_t *p_ik_flow_tbl_list;

        img_cfg_query_flow_tbl_list_memory_size(&flow_tbl_list_size);
        Rval = img_cfg_prepare_flow_tbl_list_memory(NULL, 0, 0, flow_tbl_list_size, AMBA_IK_ARCH_HARD_INIT);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_prepare_flow_tbl_list_memory", "test p_flow_tbl_list_mem null ptr fail case", id_cnt++);
        Rval = img_cfg_prepare_flow_tbl_list_memory(&p_ik_flow_tbl_list, 0, 100, flow_tbl_list_size, AMBA_IK_ARCH_HARD_INIT);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_prepare_flow_tbl_list_memory", "test mem_addr null ptr fail case", id_cnt++);
        Rval = img_cfg_prepare_flow_tbl_list_memory(&p_ik_flow_tbl_list, 0x12345678, 0, flow_tbl_list_size, AMBA_IK_ARCH_HARD_INIT);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_prepare_flow_tbl_list_memory", "test size null ptr fail case", id_cnt++);
        Rval = img_cfg_prepare_flow_tbl_list_memory(&p_ik_flow_tbl_list, 0x12345678, 100, flow_tbl_list_size, AMBA_IK_ARCH_HARD_INIT);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_prepare_flow_tbl_list_memory", "test (flow_tbl_list_size < size) fail case", id_cnt++);
    }

    {
        size_t crc_data_size;
        idsp_crc_data_t *p_crc_data;

        img_cfg_query_crc_data_memory_size(&crc_data_size);
        Rval = img_cfg_prepare_crc_data_memory(NULL, 0, 0, crc_data_size, AMBA_IK_ARCH_HARD_INIT);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_prepare_crc_data_memory", "test p_crc_data_mem null ptr fail case", id_cnt++);
        Rval = img_cfg_prepare_crc_data_memory(&p_crc_data, 0, 100, crc_data_size, AMBA_IK_ARCH_HARD_INIT);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_prepare_crc_data_memory", "test mem_addr null ptr fail case", id_cnt++);
        Rval = img_cfg_prepare_crc_data_memory(&p_crc_data, 0x12345678, 0, crc_data_size, AMBA_IK_ARCH_HARD_INIT);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_prepare_crc_data_memory", "test size null ptr fail case", id_cnt++);
        Rval = img_cfg_prepare_crc_data_memory(&p_crc_data, 0x12345678, 100, crc_data_size, AMBA_IK_ARCH_HARD_INIT);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_prepare_crc_data_memory", "test (crc_data_size < size) fail case", id_cnt++);
    }

    {
        amba_ik_context_entity_t *p_ctx;
        idsp_ik_flow_ctrl_t *p_flow = NULL;
        amba_ik_flow_tables_t *p_flow_tbl = NULL;
        amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
        uintptr addr = 0;

        img_ctx_get_context(pMode->ContextId, &p_ctx);
        img_ctx_get_flow_control(pMode->ContextId, 0, &addr);
        memcpy(&p_flow, &addr, sizeof(void *));
        img_ctx_get_flow_tbl(pMode->ContextId, 0, &addr);
        memcpy(&p_flow_tbl, &addr, sizeof(void *));
        img_ctx_get_flow_tbl_list(pMode->ContextId, 0, &addr);
        memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

        ability.pipe = AMBA_IK_PIPE_STILL;
        ability.video_pipe = AMBA_IK_VIDEO_LINEAR;
        Rval = img_cfg_init_flow_control(pMode->ContextId, p_flow, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_set_idsp_step_info", "test (p_ability->pipe == AMBA_IK_PIPE_VIDEO) false case", id_cnt++);

        ability.pipe = AMBA_IK_PIPE_VIDEO;
        ability.video_pipe = AMBA_IK_VIDEO_MAX;
        Rval = img_cfg_init_flow_control(pMode->ContextId, p_flow, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_set_idsp_step_info", "test switch default fail case", id_cnt++);

        ability.video_pipe = AMBA_IK_VIDEO_LINEAR;
        ability.pipe = 2;
        Rval = img_cfg_init_flow_control(pMode->ContextId, p_flow, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_set_idsp_step_info", "test test pipe != video or still fail case", id_cnt++);
        ability.pipe = AMBA_IK_PIPE_VIDEO;

        Rval = img_cfg_init_flow_control(pMode->ContextId, NULL, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_init_flow_control", "test NULL ptr fail case", id_cnt++);

        Rval = img_cfg_init_flow_control(pMode->ContextId, p_flow, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_init_flow_control", "test NULL ptr fail case", id_cnt++);

        Rval = img_cfg_init_flow_tbl(NULL, p_flow_tbl_list, &ability, p_ctx->organization.p_bin_data_dram_addr);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_init_flow_tbl", "test NULL ptr fail case", id_cnt++);

        Rval = img_cfg_init_flow_tbl(p_flow_tbl, NULL, &ability, p_ctx->organization.p_bin_data_dram_addr);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_init_flow_tbl", "test NULL ptr fail case", id_cnt++);

        Rval = img_cfg_init_flow_tbl(p_flow_tbl, p_flow_tbl_list, NULL, p_ctx->organization.p_bin_data_dram_addr);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_init_flow_tbl", "test NULL ptr fail case", id_cnt++);

        ability.pipe = AMBA_IK_PIPE_STILL;
        Rval = img_cfg_init_flow_tbl(p_flow_tbl, p_flow_tbl_list, &ability, p_ctx->organization.p_bin_data_dram_addr);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_init_flow_tbl", "test (p_ability->pipe == AMBA_IK_PIPE_VIDEO) false case", id_cnt++);
        ability.pipe = AMBA_IK_PIPE_VIDEO;

        ability.video_pipe = 6;
        Rval = img_cfg_init_flow_tbl(p_flow_tbl, p_flow_tbl_list, &ability, p_ctx->organization.p_bin_data_dram_addr);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_init_flow_tbl", "test (p_ability->video_pipe < AMBA_IK_VIDEO_MAX) false case", id_cnt++);
        ability.video_pipe = AMBA_IK_VIDEO_LINEAR;

        ability.pipe = 2;
        Rval = img_cfg_init_flow_tbl(p_flow_tbl, p_flow_tbl_list, &ability, p_ctx->organization.p_bin_data_dram_addr);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_init_flow_tbl", "test (p_ability->pipe == AMBA_IK_PIPE_STILL) false case", id_cnt++);
        ability.pipe = AMBA_IK_PIPE_VIDEO;

        Rval = img_cfg_init_flow_tbl_list(p_flow_tbl, NULL, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_init_flow_tbl_list", "test NULL ptr fail case", id_cnt++);

        Rval = img_cfg_init_flow_tbl_list(p_flow_tbl, p_flow_tbl_list, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_init_flow_tbl_list", "test NULL ptr fail case", id_cnt++);

        ability.pipe = AMBA_IK_PIPE_STILL;
        Rval = img_cfg_init_flow_tbl_list(p_flow_tbl, p_flow_tbl_list, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_init_flow_tbl_list", "test (p_ability->pipe == AMBA_IK_PIPE_VIDEO) false case", id_cnt++);
        ability.pipe = AMBA_IK_PIPE_VIDEO;

        ability.video_pipe = 6;
        Rval = img_cfg_init_flow_tbl_list(p_flow_tbl, p_flow_tbl_list, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_init_flow_tbl_list", "test (p_ability->video_pipe < AMBA_IK_VIDEO_MAX) false case", id_cnt++);
        ability.video_pipe = AMBA_IK_VIDEO_LINEAR;

        ability.pipe = 2;
        Rval = img_cfg_init_flow_tbl_list(p_flow_tbl, p_flow_tbl_list, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_init_flow_tbl_list", "test (p_ability->pipe == AMBA_IK_PIPE_STILL) false case", id_cnt++);
        ability.pipe = AMBA_IK_PIPE_VIDEO;

        p_flow_tbl_list->p_ca_warp_hor_red = NULL;
        p_flow_tbl_list->p_ca_warp_hor_blue = NULL;
        p_flow_tbl_list->p_ca_warp_ver_red = NULL;
        p_flow_tbl_list->p_ca_warp_ver_blue = NULL;
        p_flow_tbl_list->p_warp_hor = NULL;
        p_flow_tbl_list->p_warp_ver = NULL;
        p_flow_tbl_list->p_aaa = NULL;
        Rval = img_cfg_amalgam_data_update(p_flow, p_flow_tbl_list, (void *)am_dump_buffer);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_amalgam_data_update", "test some CR lists are NULL case", id_cnt++);

        Rval = img_cfg_amalgam_data_ctx_update(33, (void *)am_dump_buffer);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_amalgam_data_ctx_update", "test get ctx fail case", id_cnt++);

        Rval = img_cfg_query_debug_flow_data_size(NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_query_debug_flow_data_size", "test NULL ptr fail case", id_cnt++);

        Rval = img_cfg_set_cr_memory_fence(p_ctx->fence, NULL, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_set_cr_memory_fence", "test NULL ptr fail case", id_cnt++);

        Rval = img_cfg_set_cr_memory_fence(p_ctx->fence, p_flow_tbl, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_set_cr_memory_fence", "test NULL ptr fail case", id_cnt++);

        ability.pipe = AMBA_IK_PIPE_STILL;
        Rval = img_cfg_set_cr_memory_fence(p_ctx->fence, p_flow_tbl, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_set_cr_memory_fence", "test (p_ability->pipe == AMBA_IK_PIPE_VIDEO) false case", id_cnt++);
        ability.pipe = AMBA_IK_PIPE_VIDEO;

        ability.video_pipe = 6;
        Rval = img_cfg_set_cr_memory_fence(p_ctx->fence, p_flow_tbl, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_set_cr_memory_fence", "test (p_ability->video_pipe < AMBA_IK_VIDEO_MAX) false case", id_cnt++);
        ability.video_pipe = AMBA_IK_VIDEO_LINEAR;

        ability.pipe = 2;
        Rval = img_cfg_set_cr_memory_fence(p_ctx->fence, p_flow_tbl, &ability);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_set_cr_memory_fence", "test (p_ability->pipe == AMBA_IK_PIPE_STILL) false case", id_cnt++);
        ability.pipe = AMBA_IK_PIPE_VIDEO;

        Rval = img_cfg_init_cr_memory_fence(NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_ImgConfigComponentIF.c", "img_cfg_init_cr_memory_fence", "test NULL ptr fail case", id_cnt++);
    }
}

INT32 ik_ctest_config_main(void)
{
    UINT32 Rval = IK_OK;
    AMBA_IK_ABILITY_s Ability;
    AMBA_IK_MODE_CFG_s Mode;

    memset(ik_working_buffer, 0, sizeof(ik_working_buffer));
    memset(am_dump_buffer, 0, sizeof(am_dump_buffer));
    id_cnt = 0;

    pFile = fopen("data/ik_ctest_config.txt", "w");

    // init IK
    _load_idsp_default_binary();
    Ability.Pipe = AMBA_IK_PIPE_VIDEO;
    Ability.VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_3;

    _Init_Image_Kernel(&Ability);
    _Init_Image_Kernel_Core();

    Mode.ContextId = 0;
    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    // test
    IK_TestCovr_ImgConfigComponentIF(&Mode);
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

