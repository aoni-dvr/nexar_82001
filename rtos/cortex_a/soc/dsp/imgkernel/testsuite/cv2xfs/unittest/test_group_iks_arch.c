#include "test_ik_global.h"


static UINT8 ik_working_buffer[5<<21];
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

static void IKS_TestCovr_ImgArchComponentIF()
{
    UINT32 Rval = IK_OK;
    ik_context_setting_t setting_of_each_context = {0};
    ik_buffer_info_t *p_iks_buffer_info = NULL;
    ik_ability_t ability = {0};

    ability.pipe = AMBA_IK_PIPE_VIDEO;
    ability.video_pipe = AMBA_IK_VIDEO_HDR_EXPO_3;
    setting_of_each_context.cr_ring_setting[0].p_ability = &ability;
    setting_of_each_context.context_number = 1;
    setting_of_each_context.cr_ring_setting[0].cr_ring_number = 3;

    Rval = iks_arch_init_architecture((void*)ik_working_buffer, setting_of_each_context.context_number,
                                      sizeof(ik_working_buffer), 2u);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchComponentIF.c", "iks_arch_init_architecture", "test ik_init_mode != HARD or SOFT true case", id_cnt++);

    iks_arch_get_ik_working_buffer(&p_iks_buffer_info);
    p_iks_buffer_info->init_flag = 0u;
    Rval = iks_arch_init_architecture((void*)ik_working_buffer, setting_of_each_context.context_number,
                                      sizeof(ik_working_buffer), AMBA_IK_ARCH_SOFT_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchComponentIF.c", "iks_arch_init_architecture", "test (p_iks_working_buffer_info->init_flag != 1UL) fail case", id_cnt++);
    p_iks_buffer_info->init_flag = 1u;

    p_iks_buffer_info->ik_version_major = 0u;
    Rval = iks_arch_init_architecture((void*)ik_working_buffer, setting_of_each_context.context_number,
                                      sizeof(ik_working_buffer), AMBA_IK_ARCH_SOFT_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchComponentIF.c", "iks_arch_init_architecture", "test (p_iks_working_buffer_info->ik_version_major != IK_VERSION_MAJOR) fail case", id_cnt++);
    p_iks_buffer_info->ik_version_major = 1u;

    p_iks_buffer_info->ik_version_minor = 0u;
    Rval = iks_arch_init_architecture((void*)ik_working_buffer, setting_of_each_context.context_number,
                                      sizeof(ik_working_buffer), AMBA_IK_ARCH_SOFT_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchComponentIF.c", "iks_arch_init_architecture", "test (p_iks_working_buffer_info->ik_version_minor != IK_VERSION_MINOR) fail case", id_cnt++);
    p_iks_buffer_info->ik_version_minor = 1u;

    p_iks_buffer_info->number_of_context = 0u;
    Rval = iks_arch_init_architecture((void*)ik_working_buffer, setting_of_each_context.context_number,
                                      sizeof(ik_working_buffer), AMBA_IK_ARCH_SOFT_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchComponentIF.c", "iks_arch_init_architecture", "test (p_iks_working_buffer_info->number_of_context != context_number) fail case", id_cnt++);
    p_iks_buffer_info->number_of_context = 1u;

    p_iks_buffer_info->buffer_size = 0u;
    Rval = iks_arch_init_architecture((void*)ik_working_buffer, setting_of_each_context.context_number,
                                      sizeof(ik_working_buffer), AMBA_IK_ARCH_SOFT_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchComponentIF.c", "iks_arch_init_architecture", "test (p_iks_working_buffer_info->number_of_context != context_number) fail case", id_cnt++);
    p_iks_buffer_info->buffer_size = sizeof(ik_working_buffer);

    p_iks_buffer_info->init_flag = 0u;
    Rval = iks_arch_get_context_number();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchComponentIF.c", "iks_arch_get_context_number", "test (p_iks_working_buffer_info->init_flag == 1U) false case", id_cnt++);
    p_iks_buffer_info->init_flag = 1u;
}

INT32 iks_ctest_arch_main(void)
{
    UINT32 Rval = IK_OK;
    AMBA_IK_ABILITY_s Ability;
    AMBA_IK_MODE_CFG_s Mode;

    memset(ik_working_buffer, 0, sizeof(ik_working_buffer));
    id_cnt = 0;

    pFile = fopen("data/iks_ctest_arch.txt", "w");

    // init IK
    _load_idsp_default_binary();
    Ability.Pipe = AMBA_IK_PIPE_VIDEO;
    Ability.VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_3;

    _Init_Image_Kernel(&Ability);
    _Init_Image_Kernel_Core();

    Mode.ContextId = 0;
    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    // test AmbaDSP_ImgArchComponentIF.c
    IKS_TestCovr_ImgArchComponentIF();
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

