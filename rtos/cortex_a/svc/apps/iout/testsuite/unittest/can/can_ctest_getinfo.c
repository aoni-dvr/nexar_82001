#include "can_ctest.h"

INT32 can_ctest_getinfo_main(void)
{
    AMBA_CAN_BIT_INFO_s BitInfo;
    AMBA_CAN_FILTER_s FilterInfo[32];
    UINT32 FilterLen;

    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &IoDiag_CanCtrl.Config);
    CTestHw_SetCanPllFb(0); // pll = 0
    AmbaCAN_GetInfo(AMBA_CAN_CHANNEL0, &BitInfo, &FilterLen, FilterInfo);

    CTestHw_SetCanPllFb(1);
    AmbaCAN_GetInfo(AMBA_CAN_CHANNEL0, &BitInfo, &FilterLen, FilterInfo);

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    AmbaSafety_CanSetSafeState(AMBA_CAN_CHANNEL0, 0xFF);
    AmbaCAN_GetInfo(AMBA_CAN_CHANNEL0, &BitInfo, &FilterLen, FilterInfo);
    AmbaSafety_CanSetSafeState(AMBA_CAN_CHANNEL0, 0);
#endif

    AmbaCAN_GetInfo(AMBA_NUM_CAN_CHANNEL, &BitInfo, &FilterLen, FilterInfo);

    /* erro arguments */
    AmbaCAN_GetInfo(AMBA_CAN_CHANNEL0, NULL, &FilterLen, FilterInfo);
    AmbaCAN_GetInfo(AMBA_CAN_CHANNEL0, &BitInfo, NULL, FilterInfo);
    AmbaCAN_GetInfo(AMBA_CAN_CHANNEL0, &BitInfo, &FilterLen, NULL);

    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);
    AmbaCAN_GetInfo(AMBA_CAN_CHANNEL0, &BitInfo, &FilterLen, FilterInfo); // flow error
    return 0;
}

