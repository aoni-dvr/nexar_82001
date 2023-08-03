#include "can_ctest.h"

INT32 can_ctest_disable_main(void)
{
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &IoDiag_CanCtrl.Config);

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    AmbaSafety_CanSetSafeState(AMBA_CAN_CHANNEL0, 0xFF);
    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);
    AmbaSafety_CanSetSafeState(AMBA_CAN_CHANNEL0, 0);
#endif

    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);
    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);
    AmbaCAN_Disable(AMBA_NUM_CAN_CHANNEL);
    return 0;
}

