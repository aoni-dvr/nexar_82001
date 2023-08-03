#include "can_ctest.h"

INT32 can_ctest_read_main(void)
{
    AMBA_CAN_CONFIG_s   Config;
    AMBA_CAN_MSG_s CanMessage;
    AMBA_CAN_FD_MSG_s CanFdMessage;
    UINT32 Timeout = 0xFFFFFFFF;
    UINT32 i;

    memcpy(&Config, &(IoDiag_CanCtrl.Config), sizeof(AMBA_CAN_CONFIG_s)); // reset settings
    /* mode switch */
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // DMA mode
    for (i = 0; i < 100; i++) {
        AMBA_CAN_REG_s *reg;
        reg = CTS_CanGetRegAddr(AMBA_CAN_CHANNEL0);
        CTestHw_CanSetIntStatus(reg, 0x010000); // RX_DMA_DESC_DONE
        CTestHw_SetCanCanDmaMsgCtrl(0);
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        can_ctest_isr(AMBA_INT_SPI_ID039_CAN0);
#else if (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52))
        can_ctest_isr(AMBA_INT_SPI_ID39_CAN0);
#endif
        AmbaCAN_Read(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // update read buffer index
        CTestHw_SetCanCanDmaMsgCtrl(0x40);
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        can_ctest_isr(AMBA_INT_SPI_ID039_CAN0);
#else if (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52))
        can_ctest_isr(AMBA_INT_SPI_ID39_CAN0);
#endif
        AmbaCAN_ReadFd(AMBA_CAN_CHANNEL0, &CanFdMessage, Timeout);
    }

    for (i = 0; i < 100; i++) {
        AMBA_CAN_REG_s *reg;
        reg = CTS_CanGetRegAddr(AMBA_CAN_CHANNEL0);
        CTestHw_CanSetIntStatus(reg, 0x010000); // RX_DMA_DESC_DONE
        CTestHw_SetCanCanDmaMsgCtrl(0x80); // RTR
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        can_ctest_isr(AMBA_INT_SPI_ID039_CAN0);
#else if (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52))
        can_ctest_isr(AMBA_INT_SPI_ID39_CAN0);
#endif
        AmbaCAN_Read(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // update read buffer index
    }

    CTest_SetCanSemaStatus(1); // semaphore error
    AmbaCAN_Read(AMBA_CAN_CHANNEL0, &CanMessage, Timeout);
    AmbaCAN_ReadFd(AMBA_CAN_CHANNEL0, &CanFdMessage, Timeout);
    CTest_SetCanMutexStatus(1); // mutex error
    AmbaCAN_Read(AMBA_CAN_CHANNEL0, &CanMessage, Timeout);
    AmbaCAN_ReadFd(AMBA_CAN_CHANNEL0, &CanFdMessage, Timeout);
    CTest_SetCanMutexStatus(0); // clean
    CTest_SetCanSemaStatus(0); // clean

    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

    Config.FdMaxRxRingBufSize = 0;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // non-FD mode
    for (i = 0; i < 100; i++) {
        AmbaCAN_Read(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // update read buffer index
    }
    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

    memcpy(&Config, &(IoDiag_CanCtrl.Config), sizeof(AMBA_CAN_CONFIG_s)); // reset settings
    Config.EnableDMA = 0;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // non-DMA mode
    for (i = 0; i < 100; i++) {
        AmbaCAN_Read(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // update read buffer index
        AmbaCAN_ReadFd(AMBA_CAN_CHANNEL0, &CanFdMessage, Timeout);
    }
    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

    Config.FdMaxRxRingBufSize = 0;
    AmbaCAN_Enable(AMBA_CAN_CHANNEL0, &Config); // non-FD mode
    for (i = 0; i < 100; i++) {
        AmbaCAN_Read(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // update read buffer index
    }
    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    /* safe state */
    AmbaSafety_CanSetSafeState(AMBA_CAN_CHANNEL0, 0xFF);
    AmbaCAN_Read(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // error safe state
    AmbaCAN_ReadFd(AMBA_CAN_CHANNEL0, &CanFdMessage, Timeout); // error safe state
    AmbaSafety_CanSetSafeState(AMBA_CAN_CHANNEL0, 0);
#endif

    /* error arguments */

    AmbaCAN_Read(AMBA_NUM_CAN_CHANNEL, &CanMessage, Timeout); // error argument
    AmbaCAN_ReadFd(AMBA_NUM_CAN_CHANNEL, &CanFdMessage, Timeout); // error argument
    AmbaCAN_Read(AMBA_CAN_CHANNEL0, NULL, Timeout); // error argument
    AmbaCAN_ReadFd(AMBA_CAN_CHANNEL0, NULL, Timeout); // error argument

    AmbaCAN_Disable(AMBA_CAN_CHANNEL0);

    /* invalide arguments */

    AmbaCAN_Read(AMBA_CAN_CHANNEL0, &CanMessage, Timeout); // flow error
    AmbaCAN_ReadFd(AMBA_CAN_CHANNEL0, &CanFdMessage, Timeout); // flow error
    return 0;
}

