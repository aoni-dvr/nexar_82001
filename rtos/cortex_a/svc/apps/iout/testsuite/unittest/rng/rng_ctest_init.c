#include "rng_ctest.h"

INT32 rng_ctest_init_main(void)
{
    AmbaRNG_Init();// ok
    AmbaRNG_Init();// ng
    AmbaRNG_Deinit();
    return 0;
}

