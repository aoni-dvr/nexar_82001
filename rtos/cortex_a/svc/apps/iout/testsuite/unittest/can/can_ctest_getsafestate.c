#include "can_ctest.h"

INT32 can_ctest_getsafestate_main(void)
{
    UINT32 State;
    AmbaSafety_CanGetSafeState(AMBA_CAN_CHANNEL0, &State);
    AmbaSafety_CanGetSafeState(AMBA_CAN_CHANNEL0, NULL);
    AmbaSafety_CanGetSafeState(AMBA_NUM_CAN_CHANNEL, &State);
    return 0;
}

