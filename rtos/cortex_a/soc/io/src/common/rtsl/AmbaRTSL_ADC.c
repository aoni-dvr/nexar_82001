/*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"

#if defined(CONFIG_THREADX)
#include "AmbaRTSL_GIC.h"
#endif
#include "AmbaRTSL_ADC.h"
#include "AmbaCSL_ADC.h"

#if defined(CONFIG_THREADX)
#include "AmbaRTSL_PLL.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include <AmbaIOUtility.h>
#endif

AMBA_ADC_REG_s * pAmbaADC_Reg;
#else
#include "hw/ambarella_clk.h"
#include "Amba_adc.h"
#endif
typedef struct {
    AMBA_ADC_FIFO_s         FifoConfig[AMBA_NUM_ADC_FIFO];          /* ADC FIFO Allocation */
    AMBA_ADC_CHANNEL_s      ChannelConfig[AMBA_NUM_ADC_CHANNEL];    /* ADC Channel Configuration */
    UINT32                  CurrentClock;                           /* ADC Current Clock Frequency */
    UINT32                  SampleRate;                             /* ADC Sample rate */
    UINT32 *                pCollectBuf;                            /* Collection buffer that stores sampled data */
    UINT32                  BufferIdx;                              /* Write point for collection buffer */
    UINT32                  BufferSize;                             /* Buffer size of collection buffer */
    AMBA_ADC_ISR_HANDLER_f  DataHandler;
} AMBA_ADC_CTRL_s;

#if defined(CONFIG_THREADX)
static void ADC_ISR(UINT32 IntID, UINT32 IsrArg);
#endif

static AMBA_ADC_CTRL_s AmbaAdcCtrl;

#if defined(CONFIG_QNX)
#pragma GCC optimize ("O0")
void AmbaDelayCycles(UINT32 Delay)
{
    UINT32 Tmp = Delay;

    while (Tmp > 0) {
        Tmp--;
    }
}
#pragma GCC push_options
#endif

#if defined(CONFIG_THREADX)
/**
 *  ADC_GetIntStatus - Get ADC interrupt status
 *  @param[out] pIntStatus pointer to ADC interrupt status
 */
static void ADC_GetIntStatus(AMBA_ADC_INT_STATUS_s *pIntStatus)
{
    UINT32 FifoError;
    UINT32 FifoIntStatus;

    if ((pAmbaADC_Reg->CtrlIntStatus & 0x1U) != 0x0U) {
        /* ADC error occurs */
        FifoError = pAmbaADC_Reg->ErrorStatus.ErrFifo;
        FifoIntStatus = pAmbaADC_Reg->FifoIntStatus;
        if ((FifoError == 0x1U) && (FifoIntStatus == 0x0U)) {
            /* Invalid FIFO setting or FIFO underflow */
            pIntStatus->InterruptFlags |= AMBA_ADC_INT_INVAILD_FIFO_PARAM;
        }

        if (pAmbaADC_Reg->ErrorStatus.OverPeriod != 0x0U) {
            /* Wrong slot period setting */
            pIntStatus->InterruptFlags |= AMBA_ADC_INT_SLOT_PERIOD_ERR;
        }
    }

    if ((pAmbaADC_Reg->CtrlIntStatus & 0x2U) != 0x0U) {
        /* VCM event counter over the threshold */
        pIntStatus->InterruptFlags |= AMBA_ADC_INT_EVENT_COUNTER;
    }

    pIntStatus->FifoIntStatus = pAmbaADC_Reg->FifoIntStatus;
    if (pIntStatus->FifoIntStatus != 0x0U) {
        /* FIFO full, underflow, or overflow */
        pIntStatus->InterruptFlags |= AMBA_ADC_INT_FIFO_ERROR;
    }

    pIntStatus->DataIntStatus = pAmbaADC_Reg->DataIntStatus;
    if (pIntStatus->DataIntStatus != 0x0U) {
        /* ADC channel data value is out of range */
        pIntStatus->InterruptFlags |= AMBA_ADC_INT_DATA_EXCEPTION;
    }
}

/**
 *  ADC_ClearIntStatus - Clear ADC interrupt status
 */
static void ADC_ClearIntStatus(void)
{
    UINT32 WriteOneClear = 0xffffffffU;

    pAmbaADC_Reg->CtrlIntStatus = WriteOneClear;
    pAmbaADC_Reg->FifoIntStatus = WriteOneClear;
    pAmbaADC_Reg->DataIntStatus = WriteOneClear;
}

#pragma GCC push_options
#pragma GCC target("general-regs-only")
/**
 *  ADC_ISR - Handle the ADC level change interrupt.
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void ADC_ISR(UINT32 IntID, UINT32 IsrArg)
{
    AMBA_ADC_CTRL_s *pAdcCtrl = &AmbaAdcCtrl;
    AMBA_ADC_INT_STATUS_s IntStatus = {0};
    UINT32 Mask = 0x1U, i;
    UINT32 NumData;

    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&IsrArg);

    ADC_GetIntStatus(&IntStatus);

    if (IntStatus.FifoIntStatus != 0x0U) {
        for (i = 0U; i < AMBA_NUM_ADC_FIFO; i++) {
            if ((IntStatus.FifoIntStatus & Mask) != 0x0U) {
                NumData = pAmbaADC_Reg->FifoStatus[i].FifoCount;
                if ((pAdcCtrl->BufferIdx + NumData) > pAdcCtrl->BufferSize) {
                    NumData = pAdcCtrl->BufferSize - pAdcCtrl->BufferIdx;
                }

                while (NumData != 0x0U) {
                    pAdcCtrl->pCollectBuf[pAdcCtrl->BufferIdx] = pAmbaADC_Reg->FifoData[i][0];
                    pAdcCtrl->BufferIdx++;
                    NumData--;
                }

                if ((pAdcCtrl->BufferIdx == pAdcCtrl->BufferSize) && (pAdcCtrl->DataHandler != NULL)) {
                    pAdcCtrl->DataHandler(pAdcCtrl->BufferIdx);
                }
            }
            Mask = Mask << 1U;
        }
    }

    ADC_ClearIntStatus();
}
#pragma GCC pop_options
#endif

#if defined(CONFIG_QNX)
/**
 *  ADC_SetAdcClk - Set the ADC clock frequency
 */
static UINT32 ADC_SetAdcClk(UINT32 Frequency)
{
    UINT32 RetVal = ADC_ERR_NONE;
    int fd;
    clk_freq_t ClkFreq;

    fd = open("/dev/clock", O_RDWR);

    if (fd == -1) {
        RetVal = ADC_ERR_ARG;
    } else {
        ClkFreq.id = AMBA_CLK_ADC;

        ClkFreq.freq = Frequency;

        if (devctl(fd, DCMD_CLOCK_SET_FREQ, &ClkFreq, sizeof(ClkFreq), NULL) != EOK) {
            RetVal = ADC_ERR_ARG;
        }

        close(fd);
    }

    return RetVal;
}

/**
 *  ADC_GetAdcClk - Get the ADC clock frequency
 */
static UINT32 ADC_GetAdcClk(UINT32 *Frequency)
{
    UINT32 RetVal = ADC_ERR_NONE;
    int fd;
    clk_freq_t ClkFreq;

    fd = open("/dev/clock", O_RDWR);

    if (fd == -1) {
        RetVal = ADC_ERR_ARG;
    } else {
        ClkFreq.id = AMBA_CLK_ADC;

        ClkFreq.freq = *Frequency;

        if (devctl(fd, DCMD_CLOCK_GET_FREQ, &ClkFreq, sizeof(ClkFreq), NULL) != EOK) {
            RetVal = ADC_ERR_ARG;
        }

        close(fd);
    }

    return RetVal;
}
#endif

/**
 *  AmbaRTSL_AdcInit - Initialize the ADC module
 */
void AmbaRTSL_AdcInit(void)
{
#if defined(CONFIG_THREADX)
    AMBA_INT_CONFIG_s IntConfig = {
        .TriggerType = INT_TRIG_HIGH_LEVEL, /* Sensitivity type */
        .IrqType = INT_TYPE_IRQ,                    /* Interrupt type: IRQ or FIQ */
        .CpuTargets = 0x01U,                        /* Target cores */
    };

    UINT32 i;
    ULONG base_addr;
    INT32 offset = 0;
    (void) offset;

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    base_addr = AMBA_CORTEX_A76_ADC_BASE_ADDR;
#else
    base_addr = AMBA_CORTEX_A53_ADC_BASE_ADDR;
#endif
    AmbaMisra_TypeCast(&pAmbaADC_Reg, &base_addr);

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    base_addr = IO_UtilityFDTPropertyU32Quick(offset, "ambarella,adc", "reg", 0U);
    if ( base_addr != 0U ) {
#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)
        base_addr = base_addr | AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR;
#endif
        AmbaMisra_TypeCast(&pAmbaADC_Reg, &base_addr);
    }
#endif

    (void)AmbaRTSL_PllSetAdcClk(12000000U);         /* Max sample rate is 1MSPS => 20MHz (20 ticks per sample) */
    AmbaAdcCtrl.CurrentClock = AmbaRTSL_PllGetAdcClk();
#else
    UINT32 i;
    UINT32 Frequency = 0U;
    UINT32 RetVal = ADC_ERR_NONE;

    RetVal = ADC_SetAdcClk(12000000U);         /* Max sample rate is 1MSPS => 20MHz (20 ticks per sample) */

    if (RetVal == EOK) {
        RetVal = ADC_GetAdcClk(&Frequency);
    }

    if (RetVal == EOK) {
        AmbaAdcCtrl.CurrentClock = Frequency;
    }
#endif
    AmbaAdcCtrl.SampleRate = AmbaAdcCtrl.CurrentClock / (AMBA_ADC_ONE_SLOT_PERIOD * AMBA_NUM_ADC_CHANNEL);
    AmbaCSL_AdcSetTimeSlotPeriod(AMBA_ADC_ONE_SLOT_PERIOD * AMBA_NUM_ADC_CHANNEL);

    for (i = 0U; i < AMBA_NUM_ADC_FIFO; i ++) {
        AmbaAdcCtrl.FifoConfig[i].AdcChanNo = AMBA_ADC_CHANNEL0;
        AmbaAdcCtrl.FifoConfig[i].FifoSize = 0U;
    }

    for (i = 0U; i < AMBA_NUM_ADC_CHANNEL; i ++) {
        AmbaAdcCtrl.ChannelConfig[i].DataUpperBound = (UINT16)(ADC_RESOLUTION - (UINT16)1U);
        AmbaAdcCtrl.ChannelConfig[i].DataLowerBound = 0U;
    }

#if defined(CONFIG_THREADX)
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    (void)AmbaRTSL_GicIntConfig(AMBA_INT_SPI_ID74_ADC, &IntConfig, ADC_ISR, 0U);
#else
    (void)AmbaRTSL_GicIntConfig(AMBA_INT_SPI_ID45_ADC, &IntConfig, ADC_ISR, 0U);
#endif
#endif
}

/**
 *  AmbaRTSL_AdcConfig - Configure the ADC module to target sample rate
 *  @param[in] SampleRate the value of target sample
 *  @return error code
 */
UINT32 AmbaRTSL_AdcConfig(UINT32 SampleRate)
{
    UINT32 RetVal = OK;
    UINT32 TimeSlotPeriod;

    TimeSlotPeriod = SampleRate;

    if ((AmbaAdcCtrl.CurrentClock % TimeSlotPeriod) == 0x0U) {
        /* In CV2, the ADC slot period time must be fixed to "active channels multiplied by 20". */
        TimeSlotPeriod = AmbaAdcCtrl.CurrentClock / TimeSlotPeriod;
        TimeSlotPeriod = (TimeSlotPeriod / (AMBA_ADC_ONE_SLOT_PERIOD * AMBA_NUM_ADC_CHANNEL)) * (AMBA_ADC_ONE_SLOT_PERIOD * AMBA_NUM_ADC_CHANNEL);

        AmbaAdcCtrl.SampleRate = AmbaAdcCtrl.CurrentClock / TimeSlotPeriod;
    } else {
        RetVal = ADC_ERR_ARG;
    }

    return RetVal;
}


/**
 *  AmbaRTSL_AdcFifoConfig - Configure the given FIFO
 *  @param[in] AdcFifoNo ADC FIFO number
 *  @param[in] AdcCh ADC channel number
 *  @param[in] ReqFifoSize Requested FIFO size
 *  @return error code
 */
static UINT32 AmbaRTSL_AdcFifoConfig(UINT32 AdcFifoNo, UINT32 AdcCh, UINT32 ReqFifoSize)
{
    AMBA_ADC_CTRL_s *pAdcCtrl = &AmbaAdcCtrl;
    AMBA_ADC_FIFO_s *pFifoCfg;
    UINT32 FifoAvailSize;
    UINT32 i, RetVal = OK;

    if (AdcFifoNo < AMBA_NUM_ADC_FIFO) {
        /* Get the available FIFO size */
        FifoAvailSize = AMBA_ADC_FIFO_SIZE;
        for (i = 0; i < AMBA_NUM_ADC_FIFO; i++) {
            if (i != AdcFifoNo) {
                pFifoCfg = &pAdcCtrl->FifoConfig[i];
                FifoAvailSize -= pFifoCfg->FifoSize;
            }
        }

        /* Ensure the requested FIFO size does not exceed the available FIFO size */
        if (FifoAvailSize >= ReqFifoSize) {
            pFifoCfg = &pAdcCtrl->FifoConfig[AdcFifoNo];
            pFifoCfg->AdcChanNo = AdcCh;
            pFifoCfg->FifoSize = (UINT16)ReqFifoSize;

            AmbaCSL_AdcConfigFifo(AdcFifoNo, ReqFifoSize, pFifoCfg->AdcChanNo);
        } else if (FifoAvailSize > 0U) {
            pFifoCfg = &pAdcCtrl->FifoConfig[AdcFifoNo];
            pFifoCfg->AdcChanNo = AdcCh;
            pFifoCfg->FifoSize = (UINT16)FifoAvailSize;

            AmbaCSL_AdcConfigFifo(AdcFifoNo, FifoAvailSize, pFifoCfg->AdcChanNo);
        } else {
            RetVal = ADC_ERR_ARG;
        }
    }

    return RetVal;
}
/**
 *  AmbaRTSL_AdcSeamlessRead - Setup ADC to sample numbers of continuous data
 *  @param[in] AdcCh ADC channel number
 *  @param[in] BufSize Sofeware buffer size
 *  @param[in] pBuf pointer to collection buffer
 *  @return error code
 */
UINT32 AmbaRTSL_AdcSeamlessRead(UINT32 AdcCh, UINT32 BufSize, UINT32 * const pBuf)
{
    UINT32 ChanMask[AMBA_NUM_ADC_TIME_SLOT], RetVal;
    AMBA_ADC_CTRL_s *pAdcCtrl = &AmbaAdcCtrl;
    UINT32 TimeSlotPeriod;
#if defined(CONFIG_THREADX)
    UINT32 CycleUs = AmbaRTSL_PllGetNumCpuCycleUs();
    UINT32 AdcClk = AmbaRTSL_PllGetAdcClk();
#else
    UINT32 CycleUs = 1000U;
    UINT32 AdcClk = 0U;

    (void)ADC_GetAdcClk(&AdcClk);
#endif
    /* ADC software reset */
    pAmbaADC_Reg->Ctrl.Clear = 0x1U;

    /* Set to ADC continuous sampling mode and enable ADC */
    pAmbaADC_Reg->Ctrl.Mode = ADC_CTRL_CONTINUOUS_SAMPLING;
    pAmbaADC_Reg->Ctrl.Enable = 0x1U;

    /* Wait for 3 microseconds (stabilization time before ADC conversion) */
#if defined(CONFIG_THREADX)
    AmbaDelayCycles(3U * AmbaRTSL_PllGetNumCpuCycleUs());
#else
    AmbaDelayCycles(3U * 1000U);
#endif

    /* Set default slot and the sampling channels */
    ChanMask[0] = (UINT32)1U << AdcCh;
    AmbaCSL_AdcConfigTimeSlot(1U, ChanMask);

    /* Configure adc sampling time slot period */
    TimeSlotPeriod = pAdcCtrl->CurrentClock / pAdcCtrl->SampleRate;
    AmbaCSL_AdcSetTimeSlotPeriod(TimeSlotPeriod);

    /* Initialize FIFO */
    pAdcCtrl->BufferIdx = 0U;
    pAdcCtrl->BufferSize = BufSize;
    pAdcCtrl->pCollectBuf = pBuf;
    RetVal = AmbaRTSL_AdcFifoConfig(AMBA_ADC_FIFO0, AdcCh, BufSize);

    if (RetVal == OK) {
        /* Turn on ADC and T2V */
        pAmbaADC_Reg->FifoReset = 1U;
        AmbaCSL_AdcPowerDownDisable();
#if defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#else
        AmbaCSL_AdcT2vPowerDownDisable();
#endif

#if defined(CONFIG_THREADX)
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        (void)AmbaRTSL_GicIntEnable(AMBA_INT_SPI_ID74_ADC);
#else
        (void)AmbaRTSL_GicIntEnable(AMBA_INT_SPI_ID45_ADC);
#endif
#endif

        /* Set ADC start */
        pAmbaADC_Reg->Ctrl.Start = 0x1U;

        if (AdcClk != 0UL) {
            /* Wait for two ADC conversion time because the first one is incorrect */
            AmbaDelayCycles(2U * ((1000000U * AMBA_NUM_ADC_CHANNEL * AMBA_ADC_ONE_SLOT_PERIOD / AdcClk) + 1U) * CycleUs);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_AdcSingleRead - Get the number converted from voltage.
 *  @param[in] AdcCh ADC channel number
 *  @param[out] pData Converted ADC number
 *  @return error code
 */
UINT32 AmbaRTSL_AdcSingleRead(UINT32 AdcCh, UINT32 *pData)
{
    UINT32 ChanMask[AMBA_NUM_ADC_TIME_SLOT];
#if defined(CONFIG_THREADX)
    UINT32 DelayTick = AmbaRTSL_PllGetNumCpuCycleUs();
    UINT32 AdcClock = AmbaRTSL_PllGetAdcClk();
#else
    UINT32 DelayTick = 1000U;
    UINT32 AdcClock = 0U;

    (void)ADC_GetAdcClk(&AdcClock);
#endif

    /* ADC software reset */
    pAmbaADC_Reg->Ctrl.Clear = 0x1U;

    /* Set to ADC continuous sampling mode and enable ADC */
    pAmbaADC_Reg->Ctrl.Mode = ADC_CTRL_SINGLE_SAMPLING;
    pAmbaADC_Reg->Ctrl.Enable = 0x1U;

    /* Wait for 3 microseconds (stabilization time before ADC conversion) */
    AmbaDelayCycles(3U * DelayTick);

    /* Set default slot and the sampling channels */
    ChanMask[0] = ((UINT32)1U << AMBA_NUM_ADC_CHANNEL) - 1U;

    AmbaCSL_AdcConfigTimeSlot(1U, ChanMask);
    AmbaCSL_AdcSetTimeSlotPeriod(AMBA_ADC_ONE_SLOT_PERIOD * AMBA_NUM_ADC_CHANNEL);

    /* Turn on ADC and T2V */
    pAmbaADC_Reg->FifoReset = 1U;
    AmbaCSL_AdcPowerDownDisable();
#if defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV28)
    /* No T2V */
#else
    AmbaCSL_AdcT2vPowerDownDisable();
#endif

    /* Set ADC start */
    pAmbaADC_Reg->Ctrl.Start = 0x1U;

    if (AdcClock != 0UL) {
        /* Wait for two ADC conversion time because the first one is incorrect */
        AmbaDelayCycles(2U * ((1000000U * AMBA_NUM_ADC_CHANNEL * AMBA_ADC_ONE_SLOT_PERIOD / AdcClock) + 1U) * DelayTick);
    }

    while (pAmbaADC_Reg->Status.Status != ADC_STATUS_COMPLETE) {
        AmbaDelayCycles(DelayTick);
    }

    *pData = pAmbaADC_Reg->Data[AdcCh] & 0x00000fffU;

    return OK;
}

/**
 *  AmbaRTSL_AdcStop - Stop Analog-to-Digital conversion
 *  @param[out] pActualSize Number of actual converted ADC values
 */
void AmbaRTSL_AdcStop(UINT32 * pActualSize)
{
    AMBA_ADC_CTRL_s *pAdcCtrl = &AmbaAdcCtrl;
    AMBA_ADC_FIFO_s *pFifoCfg;
    UINT32 i, NumData;

#if defined(CONFIG_THREADX)
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    (void)AmbaRTSL_GicIntDisable(AMBA_INT_SPI_ID74_ADC);
#else
    (void)AmbaRTSL_GicIntDisable(AMBA_INT_SPI_ID45_ADC);
#endif
#endif

    NumData = pAmbaADC_Reg->FifoStatus[AMBA_ADC_FIFO0].FifoCount;
    if ((pAdcCtrl->BufferIdx + NumData) > pAdcCtrl->BufferSize) {
        NumData = pAdcCtrl->BufferSize - pAdcCtrl->BufferIdx;
    }

    while (NumData != 0x0U) {
        pAdcCtrl->pCollectBuf[pAdcCtrl->BufferIdx] = pAmbaADC_Reg->FifoData[AMBA_ADC_FIFO0][0];
        pAdcCtrl->BufferIdx++;
        NumData--;
    }

    pAmbaADC_Reg->Ctrl.Start = 0x0U;
#if defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    /* No T2V */
#else
    AmbaCSL_AdcT2vPowerDownEnable();
#endif
    AmbaCSL_AdcPowerDownEnable();
    pAmbaADC_Reg->Ctrl.Clear = 0x1U;

    if (pActualSize != NULL) {
        *pActualSize = pAdcCtrl->BufferIdx;
    }

    for (i = 0U; i < AMBA_NUM_ADC_FIFO; i++) {
        pFifoCfg = &pAdcCtrl->FifoConfig[i];
        pFifoCfg->FifoSize = 0U;
    }
    pAdcCtrl->BufferIdx = 0U;
}

/**
 *  AmbaRTSL_AdcHookIntHandler - Hook interrupt handler
 *  @param[in] CallBackFunc Interrupt handler
 */
void AmbaRTSL_AdcHookIntHandler(AMBA_ADC_ISR_HANDLER_f CallBackFunc)
{
    AmbaAdcCtrl.DataHandler = CallBackFunc;
}

/**
 *  AmbaRTSL_AdcGetInfo - Get current configuration
 *  @param[out] pSampleRate Sampling rate
 *  @param[out] pActiveChanList A list of all the active adc channels
 */
void AmbaRTSL_AdcGetInfo(UINT32 *pSampleRate, UINT32 *pActiveChanList)
{
    *pSampleRate = AmbaAdcCtrl.SampleRate;

    if (pAmbaADC_Reg->Ctrl.Start != 0U) {
        /* In current design, we only use time slot 0 */
        *pActiveChanList = pAmbaADC_Reg->TimeSlotCtrl[0];
    } else {
        *pActiveChanList = 0U;
    }
}
