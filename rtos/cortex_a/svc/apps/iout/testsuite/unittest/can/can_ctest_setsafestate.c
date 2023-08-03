#include "can_ctest.h"

INT32 can_ctest_setsafestate_main(void)
{
    UINT32 State = 0xFF;
    AmbaSafety_CanSetSafeState(AMBA_CAN_CHANNEL0, State);
    AmbaSafety_CanSetSafeState(AMBA_NUM_CAN_CHANNEL, State);
    return 0;
}

