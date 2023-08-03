#include "rng_ctest.h"

INT32 rng_ctest_deinit_main(void)
{
    AmbaRNG_Init();
    AmbaRNG_Deinit(); // ok
    AmbaRNG_Deinit(); // ng
    return 0;
}

