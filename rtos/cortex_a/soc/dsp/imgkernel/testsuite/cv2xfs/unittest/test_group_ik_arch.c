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

static void IK_TestCovr_ImgArchInitArchitecture()
{
    UINT32 Rval = IK_OK;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    img_arch_get_ik_working_buffer(&p_ik_buffer_info);

    Rval = img_arch_init_architecture((void*)ik_working_buffer, 1, sizeof(ik_working_buffer), 2);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchComponentIF.c", "img_arch_init_architecture", "test ik_init_mode != HARD or SOFT case", id_cnt++);

    p_ik_buffer_info->init_flag = 0;
    Rval = img_arch_init_architecture((void*)ik_working_buffer, 1, sizeof(ik_working_buffer), AMBA_IK_ARCH_SOFT_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchComponentIF.c", "img_arch_init_architecture", "test (p_ik_working_buffer_info->init_flag != 1UL) fail case", id_cnt++);

    p_ik_buffer_info->ik_version_major = 0;
    Rval = img_arch_init_architecture((void*)ik_working_buffer, 1, sizeof(ik_working_buffer), AMBA_IK_ARCH_SOFT_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchComponentIF.c", "img_arch_init_architecture", "test (p_ik_working_buffer_info->ik_version_major != IK_VERSION_MAJOR) fail case", id_cnt++);

    p_ik_buffer_info->ik_version_minor = 0;
    Rval = img_arch_init_architecture((void*)ik_working_buffer, 1, sizeof(ik_working_buffer), AMBA_IK_ARCH_SOFT_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchComponentIF.c", "img_arch_init_architecture", "test (p_ik_working_buffer_info->ik_version_minor != IK_VERSION_MINOR) fail case", id_cnt++);

    p_ik_buffer_info->number_of_context = 0;
    Rval = img_arch_init_architecture((void*)ik_working_buffer, 1, sizeof(ik_working_buffer), AMBA_IK_ARCH_SOFT_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchComponentIF.c", "img_arch_init_architecture", "test (p_ik_working_buffer_info->number_of_context != context_number) fail case", id_cnt++);

    p_ik_buffer_info->buffer_size = 0;
    Rval = img_arch_init_architecture((void*)ik_working_buffer, 1, sizeof(ik_working_buffer), AMBA_IK_ARCH_SOFT_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchComponentIF.c", "img_arch_init_architecture", "test (p_ik_working_buffer_info->buffer_size != mem_size) fail case", id_cnt++);
}

static void IK_TestCovr_ImgArchGetContextNumber()
{
    UINT32 Rval = IK_OK;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    img_arch_get_ik_working_buffer(&p_ik_buffer_info);

    p_ik_buffer_info->init_flag = 0;
    Rval = img_arch_get_context_number();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchComponentIF.c", "img_arch_get_context_number", "test (p_ik_working_buffer_info->init_flag == 1U) fail case", id_cnt++);
}

static void IK_TestCovr_ImgArchCheckIkWorkingBuffer()
{
    UINT32 Rval = IK_OK;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    img_arch_get_ik_working_buffer(&p_ik_buffer_info);

    p_ik_buffer_info->safety_state = IK_SAFETY_STATE_ERROR;
    Rval = img_arch_check_ik_working_buffer(AMBA_IK_ARCH_HARD_INIT);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchComponentIF.c", "img_arch_check_ik_working_buffer", "test (p_ik_working_buffer_info->safety_state != IK_SAFETY_STATE_OK) fail case", id_cnt++);
}

static inline void _Reset_System_API()
{
    ik_system_api_t sys_api;

    // regist sys api back
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
}

static void IK_TestCovr_ImgArchSystemApi()
{
    UINT32 Rval = IK_OK;


    img_arch_unregiste_system_memcpy();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_unregiste_system_memcpy", "test unregist system API memcpy case", id_cnt++);
    Rval = img_arch_check_system_control_callback_function();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_check_system_control_callback_function", "test check system API memcpy NULL fail case", id_cnt++);
    _Reset_System_API();

    img_arch_unregiste_system_memset();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_unregiste_system_memset", "test unregist system API memset case", id_cnt++);
    Rval = img_arch_check_system_control_callback_function();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_check_system_control_callback_function", "test check system API memset NULL fail case", id_cnt++);
    _Reset_System_API();

    img_arch_unregiste_system_memcmp();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_unregiste_system_memcmp", "test unregist system API memcmp case", id_cnt++);
    Rval = img_arch_check_system_control_callback_function();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_check_system_control_callback_function", "test check system API memcmp NULL fail case", id_cnt++);
    _Reset_System_API();

    img_arch_unregiste_system_create_mutex();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_unregiste_system_create_mutex", "test unregist system API create mutex case", id_cnt++);
    Rval = img_arch_check_system_control_callback_function();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_check_system_control_callback_function", "test check system API create mutex NULL fail case", id_cnt++);
    _Reset_System_API();

    img_arch_unregiste_system_destroy_mutex();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_unregiste_system_destroy_mutex", "test unregist system API destroy mutex case", id_cnt++);
    Rval = img_arch_check_system_control_callback_function();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_check_system_control_callback_function", "test check system API destroy mutex NULL fail case", id_cnt++);
    _Reset_System_API();

    img_arch_unregiste_system_lock_mutex();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_unregiste_system_lock_mutex", "test unregist system API lock mutex case", id_cnt++);
    Rval = img_arch_check_system_control_callback_function();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_check_system_control_callback_function", "test check system API lock mutex NULL fail case", id_cnt++);
    _Reset_System_API();

    img_arch_unregiste_system_unlock_mutex();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_unregiste_system_unlock_mutex", "test unregist system API unlock mutex case", id_cnt++);
    Rval = img_arch_check_system_control_callback_function();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_check_system_control_callback_function", "test check system API unlock mutex NULL fail case", id_cnt++);
    _Reset_System_API();

    img_arch_unregiste_system_print();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_unregiste_system_print", "test unregist system API print case", id_cnt++);
    Rval = img_arch_check_system_control_callback_function();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_check_system_control_callback_function", "test check system API print NULL fail case", id_cnt++);
    _Reset_System_API();

    img_arch_unregiste_system_clean_cache();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_unregiste_system_clean_cache", "test unregist system API clean cache case", id_cnt++);
    Rval = img_arch_check_system_control_callback_function();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_check_system_control_callback_function", "test check system API clean cache NULL fail case", id_cnt++);
    _Reset_System_API();

    img_arch_unregiste_system_virtual_to_physical_address();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_unregiste_system_virtual_to_physical_address", "test unregist system API vir to phy case", id_cnt++);
    Rval = img_arch_check_system_control_callback_function();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_check_system_control_callback_function", "test check system API vir to phy NULL fail case", id_cnt++);
    _Reset_System_API();

    img_arch_unregiste_system_physical_to_virtual_address();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_unregiste_system_physical_to_virtual_address", "test unregist system API phy to vir case", id_cnt++);
    Rval = img_arch_check_system_control_callback_function();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_check_system_control_callback_function", "test check system API phy to vir NULL fail case", id_cnt++);
    _Reset_System_API();

    Rval = img_arch_check_system_control_callback_function();
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImgArchSystemApi.c", "img_arch_check_system_control_callback_function", "test check system API not NULL OK case", id_cnt++);
}

INT32 ik_ctest_arch_main(void)
{
    UINT32 Rval = IK_OK;
    AMBA_IK_ABILITY_s Ability;
    AMBA_IK_MODE_CFG_s Mode;

    memset(ik_working_buffer, 0, sizeof(ik_working_buffer));
    id_cnt = 0;

    pFile = fopen("data/ik_ctest_arch.txt", "w");

    // init IK
    _load_idsp_default_binary();
    Ability.Pipe = AMBA_IK_PIPE_VIDEO;
    Ability.VideoPipe = AMBA_IK_VIDEO_HDR_EXPO_3;

    _Init_Image_Kernel(&Ability);
    _Init_Image_Kernel_Core();

    Mode.ContextId = 0;
    Rval |= AmbaIK_InitContext(&Mode, &Ability);

    // test img_arch_init_architecture()
    IK_TestCovr_ImgArchInitArchitecture();
    _Reset_Image_Kernel(&Ability);

    // test img_arch_get_context_number()
    IK_TestCovr_ImgArchGetContextNumber();
    _Reset_Image_Kernel(&Ability);

    // test img_arch_check_ik_working_buffer()
    IK_TestCovr_ImgArchCheckIkWorkingBuffer();
    _Reset_Image_Kernel(&Ability);

    // test AmbaDSP_ImgArchSystemApi.c
    IK_TestCovr_ImgArchSystemApi();
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

