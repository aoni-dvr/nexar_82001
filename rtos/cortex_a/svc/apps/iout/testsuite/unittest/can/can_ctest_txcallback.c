#include "can_ctest.h"

INT32 can_ctest_txcallback_main(void)
{
    AMBA_CAN_FD_MSG_s CanMessage = {
        .Id = 0x111,
        .Extension = 1,
        .DataLengthCode = 15,
    };
    UINT32 Timeout = 0xFFFFFFFF;
    UINT32 TrackId;

    AmbaCAN_WaitForTxComplete(AMBA_CAN_CHANNEL0, &TrackId); // flow error


    CTestHw_CanSetMsgBufRequestFb(2); // request and grant for buffer
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &IoDiag_CanCtrl.Config);
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // normal flow
    CTestHw_SetCanMqRxFb(0); // mq receive error
    AmbaCAN_WaitForTxComplete(AMBA_CAN_CHANNEL0, &TrackId);
    CTestHw_SetCanMqRxFb(1); // mq receive ok, tx complete
    AmbaCAN_WaitForTxComplete(AMBA_CAN_CHANNEL0, &TrackId);

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    AmbaSafety_CanSetSafeState(AMBA_CAN_CHANNEL0, 0xFF);
    AmbaCAN_WaitForTxComplete(AMBA_CAN_CHANNEL0, &TrackId); // error safe state
    AmbaSafety_CanSetSafeState(AMBA_CAN_CHANNEL0, 0);
#endif

    AmbaCAN_WaitForTxComplete(AMBA_NUM_CAN_CHANNEL, &TrackId); // error argument
    AmbaCAN_WaitForTxComplete(AMBA_CAN_CHANNEL0, NULL); // error argument

    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

    return 0;
}

