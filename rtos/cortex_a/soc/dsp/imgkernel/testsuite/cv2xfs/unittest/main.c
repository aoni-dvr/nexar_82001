#include "test_ik_global.h"

#define PRINT_LOG_RVAL 0

void AmbaPrint_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    printf(pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);

    return;
}

/* The pointer to the type must be in 32-bit alignment */
void AmbaMisra_TypeCast32(void * pNewType, const void * pOldType)
{
    memcpy(pNewType, pOldType, sizeof(UINT32));

    return;
}

/* The pointer to the type must be in 64-bit alignment */
void AmbaMisra_TypeCast64(void * pNewType, const void * pOldType)
{
    memcpy(pNewType, pOldType, sizeof(UINT64));

    return;
}

void AmbaMisra_TypeCast(void * pNewType, const void * pOldType)
{
    if(sizeof(void *) == sizeof(UINT32)) {
        AmbaMisra_TypeCast32(pNewType, pOldType);
    }
    if(sizeof(void *) == sizeof(UINT64)) {
        AmbaMisra_TypeCast64(pNewType, pOldType);
    }

    return;
}

void _ctest_log_rval(FILE *pFile, uint32 rval, char *p_file_name, char *p_func_name, char *p_desc, uint32 id)
{
    fprintf(pFile, "Rval = 0x%08X, %s, %s(), ID = %d, description : %s\r\n", rval, p_file_name, p_func_name, id, p_desc);
#if PRINT_LOG_RVAL
    printf("Rval = 0x%08X, %s, %s(), ID = %d, description : %s\r\n", rval, p_file_name, p_func_name, id, p_desc);
#endif
}

INT32 main(void)
{
    UINT32 Rval = IK_OK;

    // ok cases
    ik_ctest_hdr20_main();
    ik_ctest_ir_only20_main();
    ik_ctest_y2y422_main();
    ik_ctest_y2y420_cc_main();
    ik_ctest_calib16_main();
    ik_ctest_vid15_sbp_main();
    ik_ctest_vid15_ca_main();
    ik_ctest_vid15_warp_main();
    ik_ctest_vid8_ldly_tile2x4_main();
    ik_ctest_aaa_main();
    ik_ctest_remap_main();

    // other cases
    ik_ctest_main();
    ik_ctest_arch_main();
    ik_ctest_config_main();
    ik_ctest_context_main();
    ik_ctest_executer_main();
    ikc_ctest_main();
    iks_ctest_main();
    iks_ctest_arch_main();
    iks_ctest_config_main();
    iks_ctest_context_main();
    iks_ctest_executer_main();
    iks_ctest_idspdrv_main();

    // misc
    safety_stub_ctest_main();

    return Rval;
}

