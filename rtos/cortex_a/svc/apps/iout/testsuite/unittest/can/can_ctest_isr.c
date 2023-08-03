#include "can_ctest.h"

INT32 can_ctest_isr_main(void)
{
    AMBA_CAN_REG_s *reg;
    AMBA_CAN_FD_MSG_s CanMessage;
    UINT32 Timeout = 0xFFFFFFFF;
    AMBA_CAN_CONFIG_s   Config;
    UINT32 ch;
    UINT32 i;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    UINT32 IntID = AMBA_INT_SPI_ID039_CAN0;
#else if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 IntID = AMBA_INT_SPI_ID39_CAN0;
#endif

    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &IoDiag_CanCtrl.Config); // fd

    reg = CTS_CanGetRegAddr(AMBA_CAN_CHANNEL0);


    CTestHw_CanSetIntStatus(reg, 0xFFFFFFFF);

    CtestHw_CanSetRxDoneStatus(reg, 0x00000004);
    CtestHw_CanSetTxDoneStatus(reg, 0x00000002);
    CTestHw_CanSetRetryFail(reg, 0x00000002);

    can_ctest_isr(IntID);
    // TBD: clear interrupt status?

    CTestHw_CanSetMsgCtrl(reg, 2, 0x06); // eld = 0, dlc = 0x6
    can_ctest_isr(IntID); // update write buffer index

    CTestHw_CanSetMsgCtrl(reg, 2, 0x46); // eld = 1, dlc = 0x6
    for (i = 0; i < 100; i++) {
        can_ctest_isr(IntID); // update write buffer index
    }


    CTestHw_CanSetMsgId(reg, 2, 0x20000000); // extension format
    CTestHw_CanSetMsgCtrl(reg, 2, 0x76); // eld = 1, brs = 1, esi = 1, dlc = 0x6

    for (i = 0; i < 100; i++) {
        can_ctest_isr(IntID); // update write buffer index
    }

    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);


    memcpy(&Config, &(IoDiag_CanCtrl.Config), sizeof(AMBA_CAN_CONFIG_s));
    Config.pFdRxRingBuf = NULL;
    Config.FdMaxRxRingBufSize = 0;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // non-fd

    reg = CTS_CanGetRegAddr(AMBA_CAN_CHANNEL0);

    CTestHw_CanSetIntStatus(reg, 0xFFFFFFFF);
    CtestHw_CanSetRxDoneStatus(reg, 0x00000004);

    CTestHw_CanSetMsgCtrl(reg, 2, 0x08);
    for (i = 0; i < 100; i++) {
        can_ctest_isr(IntID); // update write buffer index
    }

    CTestHw_CanSetMsgCtrl(reg, 2, 0x80); // RTR = 0x1
    can_ctest_isr(IntID);
    CTestHw_CanSetIntStatus(reg, 0x010000); //RX_DMA_DESC_DONE
    can_ctest_isr(IntID);
    CTestHw_CanSetIntStatus(reg, 0x020000); //RX_DMA_DONE
    can_ctest_isr(IntID);
    CTestHw_CanSetIntStatus(reg, 0x000200); //RX_DONE
    can_ctest_isr(IntID);
    CTestHw_CanSetIntStatus(reg, 0x000400); //TX_DONE
    can_ctest_isr(IntID);
    CTestHw_CanSetIntStatus(reg, 0x002000); //RETRY_FAIL
    can_ctest_isr(IntID);

    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    AmbaCAN_Enable(AMBA_CAN_CHANNEL1, &IoDiag_CanCtrl.Config);
    can_ctest_isr(AMBA_INT_SPI_ID040_CAN1);
    AmbaCAN_Disable(AMBA_CAN_CHANNEL1);

    AmbaCAN_Enable(AMBA_CAN_CHANNEL2, &IoDiag_CanCtrl.Config);
    can_ctest_isr(AMBA_INT_SPI_ID041_CAN2);
    AmbaCAN_Disable(AMBA_CAN_CHANNEL2);

    AmbaCAN_Enable(AMBA_CAN_CHANNEL3, &IoDiag_CanCtrl.Config);
    can_ctest_isr(AMBA_INT_SPI_ID042_CAN3);
    AmbaCAN_Disable(AMBA_CAN_CHANNEL3);

    AmbaCAN_Enable(AMBA_CAN_CHANNEL4, &IoDiag_CanCtrl.Config);
    can_ctest_isr(AMBA_INT_SPI_ID043_CAN4);
    AmbaCAN_Disable(AMBA_CAN_CHANNEL4);

    AmbaCAN_Enable(AMBA_CAN_CHANNEL5, &IoDiag_CanCtrl.Config);
    can_ctest_isr(AMBA_INT_SPI_ID044_CAN5);
    AmbaCAN_Disable(AMBA_CAN_CHANNEL5);
#else if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    AmbaCAN_Enable(AMBA_CAN_CHANNEL1, &IoDiag_CanCtrl.Config);
    can_ctest_isr(AMBA_INT_SPI_ID40_CAN1);
    AmbaCAN_Disable(AMBA_CAN_CHANNEL1);
#endif

    return 0;
}

