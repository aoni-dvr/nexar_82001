#include "can_ctest.h"

INT32 can_ctest_writefd_main(void)
{
    AMBA_CAN_FD_MSG_s CanMessage = {
        .Id = 0x111,
        .Extension = 1,
        .DataLengthCode = 15,
    };
    UINT32 Timeout = 0xFFFFFFFF;
    UINT32 TrackId;
    AMBA_CAN_CONFIG_s   Config;

    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // flow error

    CTestHw_CanSetMsgBufRequestFb(0); // no request for buffer
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &IoDiag_CanCtrl.Config);
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout);
    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

    CTestHw_CanSetMsgBufRequestFb(1); // no grant for buffer
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &IoDiag_CanCtrl.Config);
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout);
    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

    CTestHw_CanSetMsgBufRequestFb(2); // request and grant for buffer
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &IoDiag_CanCtrl.Config);
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // normal flow

    CTest_SetCanTxBufStatus(1);
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // no available tx buffer
    CTest_SetCanTxBufStatus(2);
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // get tx buffer status error
    CTest_SetCanTxBufStatus(0);

    CTest_SetCanMutexStatus(1);
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // tx mutex error
    CTest_SetCanMutexStatus(0); // clean

    CanMessage.BitRateSwitch = 0x1;
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // setup BRS

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    AmbaSafety_CanSetSafeState(AMBA_CAN_CHANNEL0, 0xFF);
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // error safe state
    AmbaSafety_CanSetSafeState(AMBA_CAN_CHANNEL0, 0);
#endif

    CanMessage.BitRateSwitch = 0x2;
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // error argument

    CanMessage.FlexibleDataRate = 0x2;
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // error argument

    CanMessage.Extension = 0;
    CanMessage.Id = 0x8FF;
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // error argument

    CanMessage.Extension = 0;
    CanMessage.Id = 0x111;
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // error argument

    CanMessage.Extension = 1;
    CanMessage.Id = 0x2FFFFFFF;
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // error argument

    CanMessage.Priority = 0x4F;
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // error argument

    CanMessage.DataLengthCode = AMBA_NUM_CAN_DATA_LENGTH + 1;
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // error argument

    AmbaCAN_WriteFd(AMBA_NUM_CAN_CHANNEL, &CanMessage, Timeout); // error argument
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, NULL, Timeout); // error argument

    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)

    // reset value
    CanMessage.Id = 0x111;
    CanMessage.Extension = 1;
    CanMessage.Priority = 0x0;
    CanMessage.DataLengthCode = 15;
    CanMessage.BitRateSwitch = 0x1;
    CanMessage.FlexibleDataRate = 0x1;

    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &IoDiag_CanCtrl.Config);
    CanMessage.AutoAnswer = 1;
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // error argument
    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

    memcpy(&Config, &(IoDiag_CanCtrl.Config), sizeof(AMBA_CAN_CONFIG_s));
    Config.EnableAa = 1;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config);
    CanMessage.AutoAnswer = 0;
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout);

    CanMessage.AutoAnswer = 1;
    AmbaCAN_WriteFd(AMBA_CAN_CHANNEL0, &CanMessage, Timeout);

    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

#endif


    return 0;
}

