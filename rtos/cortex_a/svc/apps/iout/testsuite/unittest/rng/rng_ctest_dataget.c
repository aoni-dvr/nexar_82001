#include "rng_ctest.h"

INT32 rng_ctest_dataget_main(void)
{
    UINT32 data[5];

    AmbaRNG_Init();
#if defined (CONFIG_SOC_CV2)
    AmbaRNG_DataGet(&data[0], &data[1], &data[2], &data[3], NULL);
#endif
    AmbaRNG_DataGet(&data[0], &data[1], &data[2], &data[3], &data[4]);
    AmbaRNG_DataGet(&data[0], NULL, NULL, NULL, NULL);
    AmbaRNG_DataGet(&data[0], &data[1], &data[2], NULL, NULL);
    AmbaRNG_DataGet(&data[0], NULL, &data[2], NULL, NULL);
    AmbaRNG_DataGet(&data[0], NULL, &data[2], &data[3], NULL);
    AmbaRNG_DataGet(&data[0], NULL, NULL, &data[3], NULL);
    AmbaRNG_DataGet(&data[0], NULL, NULL, &data[3], &data[4]);
    AmbaRNG_DataGet(&data[0], NULL, NULL, NULL, &data[4]);
    AmbaRNG_DataGet(NULL, NULL, NULL, NULL, NULL);

    AmbaRNG_Deinit();

    // invalid flow
#if defined (CONFIG_SOC_CV2)
    AmbaRNG_DataGet(&data[0], &data[1], &data[2], &data[3], NULL);
#else
    AmbaRNG_DataGet(&data[0], &data[1], &data[2], &data[3], &data[4]);
#endif
    return 0;
}

