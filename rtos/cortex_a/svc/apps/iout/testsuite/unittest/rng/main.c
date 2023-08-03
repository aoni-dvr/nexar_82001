#include "rng_ctest.h"


void AmbaPrint_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    printf(pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);

    return;
}

INT32 main(void)
{

    rng_ctest_init_main();
    rng_ctest_dataget_main();
    rng_ctest_deinit_main();

    return 0;
}

