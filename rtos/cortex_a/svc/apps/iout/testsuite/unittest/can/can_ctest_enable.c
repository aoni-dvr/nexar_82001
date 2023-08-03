#include "can_ctest.h"



INT32 can_ctest_enable_main(void)
{
    AMBA_CAN_CONFIG_s   Config;
    UINT32 ch;
    UINT32 gic_index;
    UINT32 res_index;
    memcpy(&Config, &(IoDiag_CanCtrl.Config), sizeof(AMBA_CAN_CONFIG_s));

    for (gic_index = 1; gic_index < 8; gic_index++) {
        CTestHw_SetCanGicFb(gic_index);
        AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &IoDiag_CanCtrl.Config); // 7 gic ng cases
    }

    /* resource */
    for (res_index = 1; res_index < 8; res_index++) {
        CTest_SetCanResFlag(res_index);
        AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &IoDiag_CanCtrl.Config);
        AmbaCAN_Disable(AMBA_CAN_CHANNEL0);
    }

    CTest_SetCanResFlag(0); // ok case
    CTestHw_SetCanGicFb(0); // ok case
    for (ch = 0; ch < AMBA_NUM_CAN_CHANNEL; ch++) { // enable all channels
        AmbaCAN_Enable(ch, &IoDiag_CanCtrl.Config);
    }

    for (ch = 0; ch < AMBA_NUM_CAN_CHANNEL; ch++) { // disable all channels
        AmbaCAN_Disable(ch);
    }


    /* filter */

    Config.NumIdFilter = 0;
    Config.pIdFilter = NULL;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config);// filter
    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

    /* mode switch */

    Config.OpMode = AMBA_CAN_OP_MODE_OUTSIDE_LPBK ;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config);// outside look back mode
    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

    Config.OpMode = AMBA_CAN_OP_MODE_INSIDE_LPBK ;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config);// inside look back mode
    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

    memcpy(&Config, &(IoDiag_CanCtrl.Config), sizeof(AMBA_CAN_CONFIG_s)); // reset settings
    Config.OpMode = AMBA_CAN_OP_MODE_LISTEN;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // listen mode
    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

    Config.NumIdFilter = 0;
    Config.pIdFilter = NULL;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config);// filter
    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

    memcpy(&Config, &(IoDiag_CanCtrl.Config), sizeof(AMBA_CAN_CONFIG_s)); // reset settings
    Config.pFdRxRingBuf = NULL;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // non-fd mode
    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

    Config.pFdRxRingBuf = IoDiag_CanCtrl.Config.pFdRxRingBuf;
    Config.FdMaxRxRingBufSize = 0;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // non-fd mode
    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

    /* invalid flow */

    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &IoDiag_CanCtrl.Config); // flow error
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &IoDiag_CanCtrl.Config); // flow error

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    /* safe state */
    AmbaSafety_CanSetSafeState(AMBA_CAN_CHANNEL0, 0xFF);
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &IoDiag_CanCtrl.Config); // error safe state
    AmbaSafety_CanSetSafeState(AMBA_CAN_CHANNEL0, 0);
#endif

    /* error arguments */

    Config.TimeQuanta.BRP = 256;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.TimeQuanta.BRP = IoDiag_CanCtrl.Config.TimeQuanta.BRP;
    Config.TimeQuanta.SJW = 16;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.TimeQuanta.SJW = IoDiag_CanCtrl.Config.TimeQuanta.SJW;
    Config.TimeQuanta.PhaseSeg1 = 64;
    //Config.TimeQuanta.PhaseSeg2 = 16;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.TimeQuanta.PhaseSeg1 = IoDiag_CanCtrl.Config.TimeQuanta.PhaseSeg1;
    Config.TimeQuanta.PhaseSeg2 = 16;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.NumIdFilter = AMBA_NUM_CAN_MSG_BUF;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.pIdFilter = IoDiag_CanCtrl.Config.pIdFilter;
    Config.NumIdFilter = 0;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.pIdFilter = NULL;
    Config.NumIdFilter = IoDiag_CanCtrl.Config.NumIdFilter;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.pRxRingBuf = IoDiag_CanCtrl.Config.pRxRingBuf;;
    Config.MaxRxRingBufSize = 0;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.pRxRingBuf = NULL;
    Config.MaxRxRingBufSize = IoDiag_CanCtrl.Config.MaxRxRingBufSize;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.OpMode = AMBA_NUM_CAN_OP_MODE;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    AmbaCAN_Enable(AMBA_NUM_CAN_CHANNEL, &IoDiag_CanCtrl.Config); // error argument
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, NULL); // error argument

    Config.pFdRxRingBuf = IoDiag_CanCtrl.Config.pFdRxRingBuf;
    Config.FdMaxRxRingBufSize = 0;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.pFdRxRingBuf = NULL;
    Config.FdMaxRxRingBufSize = IoDiag_CanCtrl.Config.FdMaxRxRingBufSize;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    // recover setting
    memcpy(&Config, &(IoDiag_CanCtrl.Config), sizeof(AMBA_CAN_CONFIG_s));

    Config.FdSsp.TdcEnable = 2;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.FdSsp.TdcEnable = IoDiag_CanCtrl.Config.FdSsp.TdcEnable;
    Config.FdSsp.Mode = 2;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.FdSsp.Mode = IoDiag_CanCtrl.Config.FdSsp.Mode;
    Config.FdSsp.Offset = 256;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.FdSsp.Offset = IoDiag_CanCtrl.Config.FdSsp.Offset;
    Config.FdSsp.Position = 256;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.FdTimeQuanta.BRP = 256;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.FdTimeQuanta.BRP = IoDiag_CanCtrl.Config.FdTimeQuanta.BRP;
    Config.FdTimeQuanta.SJW = 16;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.FdTimeQuanta.SJW = IoDiag_CanCtrl.Config.FdTimeQuanta.SJW;
    Config.FdTimeQuanta.PhaseSeg1 = 32;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    Config.FdTimeQuanta.PhaseSeg1 = IoDiag_CanCtrl.Config.FdTimeQuanta.PhaseSeg1;
    Config.FdTimeQuanta.PhaseSeg2 = 16;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // error argument

    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

    return 0;
}

