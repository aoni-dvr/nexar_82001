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
#include "AmbaMisraFix.h"

#include "AmbaRTSL_PLL.h"
#ifndef CONFIG_QNX
#include "AmbaRTSL_GIC.h"
#endif
#include "AmbaRTSL_UART.h"
#include "AmbaCSL_UART.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include <AmbaIOUtility.h>
#endif

AMBA_UART_REG_s *pAmbaUART_Reg[AMBA_NUM_UART_CHANNEL];

typedef struct {
    UINT32  ReadPtr;                /* pointer to the next rx ring buffer data for read */
    UINT32  WritePtr;               /* pointer to the next rx ring buffer data for write */

    UINT32  RingBufSize;            /* Size of the rx ring buffer */
    UINT8   *pRingBuf;              /* pointer to the rx ring buffer */
} AMBA_RTSL_UART_RX_BUF_CTRL_s;

typedef struct {
    AMBA_RTSL_UART_RX_BUF_CTRL_s    RxBufCtrl;
    UINT32  TxDataSize;             /* Number of data in tx data buffer */
    UINT8   *pTxDataBuf;            /* pointer to tx data buffer */

    UINT32  FlowCtrlSupport;        /* UART flow control support */

    AMBA_UART_ISR_f UsrRxReadyISR;  /* Callback function for rx data ready */
    UINT32 UsrRxReadyIsrArg;        /* argument attached to the rx data ready callback */

    UINT16  IntID;                  /* Interrupt ID */
    void    (*pISR)(UINT32 IntID, UINT32 UartCh);
    UINT32  DebugInfo;
} AMBA_RTSL_UART_CTRL_s;

static void UART_IntHandler(UINT32 Irq, UINT32 UartCh);
static void UART_SetBaudRate(UINT32 SourceClk, AMBA_UART_REG_s *pUartReg, UINT32 BaudRate);

/* Call back function when completed transactions */
static AMBA_UART_ISR_f AmbaUartTxIsrCallBack;
static AMBA_UART_ISR_f AmbaUartRxIsrCallBack;

static AMBA_RTSL_UART_CTRL_s AmbaUartCtrl[AMBA_NUM_UART_CHANNEL] = {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    [AMBA_UART_APB_CHANNEL0] = {
        .RxBufCtrl = {
            .ReadPtr  = 0U,             /* pointer to the next rx ring buffer data for read */
            .WritePtr = 0U,             /* pointer to the next rx ring buffer data for write */

            .RingBufSize = 0U,          /* Size of the rx ring buffer */
            .pRingBuf = NULL,           /* pointer to the rx ring buffer */
        },
        .TxDataSize = 0U,               /* Number of data in tx data buffer */
        .pTxDataBuf = NULL,             /* pointer to tx data buffer */
        .FlowCtrlSupport = UART_FLOW_CTRL_NONE,
#ifndef CONFIG_QNX
        .IntID  = AMBA_INT_SPI_ID053_UART_APB,
#endif
        .pISR = UART_IntHandler,
        .DebugInfo = 0U,
    },

    [AMBA_UART_AHB_CHANNEL0] = {
        .RxBufCtrl = {
            .ReadPtr  = 0U,             /* pointer to the next rx ring buffer data for read */
            .WritePtr = 0U,             /* pointer to the next rx ring buffer data for write */

            .RingBufSize = 0U,          /* Size of the rx ring buffer */
            .pRingBuf = NULL,           /* pointer to the rx ring buffer */
        },
        .TxDataSize = 0U,               /* Number of data in tx data buffer */
        .pTxDataBuf = NULL,             /* pointer to tx data buffer */
        .FlowCtrlSupport = UART_FLOW_CTRL_HW,
#ifndef CONFIG_QNX
        .IntID  = AMBA_INT_SPI_ID097_UART_AHB0,
#endif
        .pISR = UART_IntHandler,
        .DebugInfo = 0U,
    },

    [AMBA_UART_AHB_CHANNEL1] = {
        .RxBufCtrl = {
            .ReadPtr  = 0U,             /* pointer to the next rx ring buffer data for read */
            .WritePtr = 0U,             /* pointer to the next rx ring buffer data for write */

            .RingBufSize = 0U,          /* Size of the rx ring buffer */
            .pRingBuf = NULL,           /* pointer to the rx ring buffer */
        },
        .TxDataSize = 0U,               /* Number of data in tx data buffer */
        .pTxDataBuf = NULL,             /* pointer to tx data buffer */
        .FlowCtrlSupport = UART_FLOW_CTRL_HW,
#ifndef CONFIG_QNX
        .IntID  = AMBA_INT_SPI_ID098_UART_AHB1,
#endif
        .pISR = UART_IntHandler,
        .DebugInfo = 0U,
    },

    [AMBA_UART_AHB_CHANNEL2] = {
        .RxBufCtrl = {
            .ReadPtr  = 0U,             /* pointer to the next rx ring buffer data for read */
            .WritePtr = 0U,             /* pointer to the next rx ring buffer data for write */

            .RingBufSize = 0U,          /* Size of the rx ring buffer */
            .pRingBuf = NULL,           /* pointer to the rx ring buffer */
        },
        .TxDataSize = 0U,               /* Number of data in tx data buffer */
        .pTxDataBuf = NULL,             /* pointer to tx data buffer */
        .FlowCtrlSupport = UART_FLOW_CTRL_HW,
#ifndef CONFIG_QNX
        .IntID  = AMBA_INT_SPI_ID099_UART_AHB2,
#endif
        .pISR = UART_IntHandler,
        .DebugInfo = 0U,
    },

    [AMBA_UART_AHB_CHANNEL3] = {
        .RxBufCtrl = {
            .ReadPtr  = 0U,             /* pointer to the next rx ring buffer data for read */
            .WritePtr = 0U,             /* pointer to the next rx ring buffer data for write */

            .RingBufSize = 0U,          /* Size of the rx ring buffer */
            .pRingBuf = NULL,           /* pointer to the rx ring buffer */
        },
        .TxDataSize = 0U,               /* Number of data in tx data buffer */
        .pTxDataBuf = NULL,             /* pointer to tx data buffer */
        .FlowCtrlSupport = UART_FLOW_CTRL_HW,
#ifndef CONFIG_QNX
        .IntID  = AMBA_INT_SPI_ID100_UART_AHB3,
#endif
        .pISR = UART_IntHandler,
        .DebugInfo = 0U,
    },

#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)

    [AMBA_UART_APB_CHANNEL0] = {
        .RxBufCtrl = {
            .ReadPtr  = 0U,             /* pointer to the next rx ring buffer data for read */
            .WritePtr = 0U,             /* pointer to the next rx ring buffer data for write */

            .RingBufSize = 0U,          /* Size of the rx ring buffer */
            .pRingBuf = NULL,           /* pointer to the rx ring buffer */
        },
        .TxDataSize = 0U,               /* Number of data in tx data buffer */
        .pTxDataBuf = NULL,             /* pointer to tx data buffer */
        .FlowCtrlSupport = UART_FLOW_CTRL_NONE,

        .IntID  = AMBA_INT_SPI_ID53_UART_APB,
        .pISR = UART_IntHandler,
        .DebugInfo = 0U,
    },

    [AMBA_UART_AHB_CHANNEL0] = {
        .RxBufCtrl = {
            .ReadPtr  = 0U,             /* pointer to the next rx ring buffer data for read */
            .WritePtr = 0U,             /* pointer to the next rx ring buffer data for write */

            .RingBufSize = 0U,          /* Size of the rx ring buffer */
            .pRingBuf = NULL,           /* pointer to the rx ring buffer */
        },
        .TxDataSize = 0U,               /* Number of data in tx data buffer */
        .pTxDataBuf = NULL,             /* pointer to tx data buffer */
        .FlowCtrlSupport = UART_FLOW_CTRL_HW,

        .IntID  = AMBA_INT_SPI_ID93_UART_AHB0,
        .pISR = UART_IntHandler,
        .DebugInfo = 0U,
    },

    [AMBA_UART_AHB_CHANNEL1] = {
        .RxBufCtrl = {
            .ReadPtr  = 0U,             /* pointer to the next rx ring buffer data for read */
            .WritePtr = 0U,             /* pointer to the next rx ring buffer data for write */

            .RingBufSize = 0U,          /* Size of the rx ring buffer */
            .pRingBuf = NULL,           /* pointer to the rx ring buffer */
        },
        .TxDataSize = 0U,               /* Number of data in tx data buffer */
        .pTxDataBuf = NULL,             /* pointer to tx data buffer */
        .FlowCtrlSupport = UART_FLOW_CTRL_HW,

        .IntID  = AMBA_INT_SPI_ID94_UART_AHB1,
        .pISR = UART_IntHandler,
        .DebugInfo = 0U,
    },

    [AMBA_UART_AHB_CHANNEL2] = {
        .RxBufCtrl = {
            .ReadPtr  = 0U,             /* pointer to the next rx ring buffer data for read */
            .WritePtr = 0U,             /* pointer to the next rx ring buffer data for write */

            .RingBufSize = 0U,          /* Size of the rx ring buffer */
            .pRingBuf = NULL,           /* pointer to the rx ring buffer */
        },
        .TxDataSize = 0U,               /* Number of data in tx data buffer */
        .pTxDataBuf = NULL,             /* pointer to tx data buffer */
        .FlowCtrlSupport = UART_FLOW_CTRL_HW,

        .IntID  = AMBA_INT_SPI_ID95_UART_AHB2,
        .pISR = UART_IntHandler,
        .DebugInfo = 0U,
    },

    [AMBA_UART_AHB_CHANNEL3] = {
        .RxBufCtrl = {
            .ReadPtr  = 0U,             /* pointer to the next rx ring buffer data for read */
            .WritePtr = 0U,             /* pointer to the next rx ring buffer data for write */

            .RingBufSize = 0U,          /* Size of the rx ring buffer */
            .pRingBuf = NULL,           /* pointer to the rx ring buffer */
        },
        .TxDataSize = 0U,               /* Number of data in tx data buffer */
        .pTxDataBuf = NULL,             /* pointer to tx data buffer */
        .FlowCtrlSupport = UART_FLOW_CTRL_HW,

        .IntID  = AMBA_INT_SPI_ID96_UART_AHB3,
        .pISR = UART_IntHandler,
        .DebugInfo = 0U,
    },

#else
    [AMBA_UART_APB_CHANNEL0] = {
        .RxBufCtrl = {
            .ReadPtr  = 0U,             /* pointer to the next rx ring buffer data for read */
            .WritePtr = 0U,             /* pointer to the next rx ring buffer data for write */

            .RingBufSize = 0U,          /* Size of the rx ring buffer */
            .pRingBuf = NULL,           /* pointer to the rx ring buffer */
        },
        .TxDataSize = 0U,               /* Number of data in tx data buffer */
        .pTxDataBuf = NULL,             /* pointer to tx data buffer */
        .FlowCtrlSupport = UART_FLOW_CTRL_NONE,

#ifndef CONFIG_QNX
        .IntID  = AMBA_INT_SPI_ID53_UART_APB,
#else
        .IntID  = 53,
#endif
        .pISR = UART_IntHandler,
        .DebugInfo = 0U,
    },

#ifndef CONFIG_QNX
    [AMBA_UART_AHB_CHANNEL0] = {
        .RxBufCtrl = {
            .ReadPtr  = 0U,             /* pointer to the next rx ring buffer data for read */
            .WritePtr = 0U,             /* pointer to the next rx ring buffer data for write */

            .RingBufSize = 0U,          /* Size of the rx ring buffer */
            .pRingBuf = NULL,           /* pointer to the rx ring buffer */
        },
        .TxDataSize = 0U,               /* Number of data in tx data buffer */
        .pTxDataBuf = NULL,             /* pointer to tx data buffer */
        .FlowCtrlSupport = UART_FLOW_CTRL_HW,

        .IntID  = AMBA_INT_SPI_ID91_UART_AHB0,
        .pISR = UART_IntHandler,
        .DebugInfo = 0U,
    },

    [AMBA_UART_AHB_CHANNEL1] = {
        .RxBufCtrl = {
            .ReadPtr  = 0U,             /* pointer to the next rx ring buffer data for read */
            .WritePtr = 0U,             /* pointer to the next rx ring buffer data for write */

            .RingBufSize = 0U,          /* Size of the rx ring buffer */
            .pRingBuf = NULL,           /* pointer to the rx ring buffer */
        },
        .TxDataSize = 0U,               /* Number of data in tx data buffer */
        .pTxDataBuf = NULL,             /* pointer to tx data buffer */
        .FlowCtrlSupport = UART_FLOW_CTRL_HW,

        .IntID  = AMBA_INT_SPI_ID92_UART_AHB1,
        .pISR = UART_IntHandler,
        .DebugInfo = 0U,
    },

    [AMBA_UART_AHB_CHANNEL2] = {
        .RxBufCtrl = {
            .ReadPtr  = 0U,             /* pointer to the next rx ring buffer data for read */
            .WritePtr = 0U,             /* pointer to the next rx ring buffer data for write */

            .RingBufSize = 0U,          /* Size of the rx ring buffer */
            .pRingBuf = NULL,           /* pointer to the rx ring buffer */
        },
        .TxDataSize = 0U,               /* Number of data in tx data buffer */
        .pTxDataBuf = NULL,             /* pointer to tx data buffer */
        .FlowCtrlSupport = UART_FLOW_CTRL_HW,

        .IntID  = AMBA_INT_SPI_ID93_UART_AHB2,
        .pISR = UART_IntHandler,
        .DebugInfo = 0U,
    },

    [AMBA_UART_AHB_CHANNEL3] = {
        .RxBufCtrl = {
            .ReadPtr  = 0U,             /* pointer to the next rx ring buffer data for read */
            .WritePtr = 0U,             /* pointer to the next rx ring buffer data for write */

            .RingBufSize = 0U,          /* Size of the rx ring buffer */
            .pRingBuf = NULL,           /* pointer to the rx ring buffer */
        },
        .TxDataSize = 0U,               /* Number of data in tx data buffer */
        .pTxDataBuf = NULL,             /* pointer to tx data buffer */
        .FlowCtrlSupport = UART_FLOW_CTRL_HW,

        .IntID  = AMBA_INT_SPI_ID94_UART_AHB3,
        .pISR = UART_IntHandler,
        .DebugInfo = 0U,
    },
#endif
#endif
};

#if defined(CONFIG_QNX)
/**
 *  AmbaRTSL_UartInit - UART RTSL initializations
 *  @return error code
 */
UINT32 AmbaRTSL_UartInit(void)
{
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    UINT32 i;

    for (i = 0U; i < AMBA_NUM_UART_CHANNEL; i++) {
        if (i == AMBA_UART_R52_CONSOLE) {
            continue;
        }
        AmbaCSL_UartDisableAllIRQ(pAmbaUART_Reg[i]);   /* Disable All IRQs */
    }
#endif
    return UART_ERR_NONE;
}

/**
 *  AmbaRTSL_UartInitIrq - UART RTSL initializations IRQ
 *  @return error code
 */
UINT32 AmbaRTSL_UartInitIrq(AMBA_UART_ISR_f TxIsrCb, AMBA_UART_ISR_f RxIsrCb)
{
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    const AMBA_RTSL_UART_CTRL_s *pUartCtrl;
    UINT32 i;

    /* clear the pointer of Call back function when completed transactions */
    AmbaUartTxIsrCallBack = TxIsrCb;
    AmbaUartRxIsrCallBack = RxIsrCb;

    for (i = 0U; i < AMBA_NUM_UART_CHANNEL; i++) {
        if (i == AMBA_UART_R52_CONSOLE) {
            continue;
        }
        AmbaCSL_UartDisableAllIRQ(pAmbaUART_Reg[i]);   /* Disable All IRQs */

        pUartCtrl = &AmbaUartCtrl[i];
    }
#endif
    return UART_ERR_NONE;
}
#else
/**
 *  AmbaRTSL_UartInit - UART RTSL initializations
 *  @return error code
 */
UINT32 AmbaRTSL_UartInit(void)
{
    UINT32 i;
    ULONG base_addr;
    INT32 offset = 0;
    (void)offset;

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    base_addr = AMBA_CA53_UART_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaUART_Reg[AMBA_UART_APB_CHANNEL0], &base_addr);
    base_addr = AMBA_CA53_UART0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaUART_Reg[1], &base_addr);
    base_addr = AMBA_CA53_UART1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaUART_Reg[2], &base_addr);
    base_addr = AMBA_CA53_UART2_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaUART_Reg[3], &base_addr);
    base_addr = AMBA_CA53_UART3_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaUART_Reg[4], &base_addr);
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    base_addr = AMBA_CORTEX_A76_UART_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaUART_Reg[AMBA_UART_APB_CHANNEL0], &base_addr);
    base_addr = AMBA_CORTEX_A76_UART0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaUART_Reg[1], &base_addr);
    base_addr = AMBA_CORTEX_A76_UART1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaUART_Reg[2], &base_addr);
    base_addr = AMBA_CORTEX_A76_UART2_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaUART_Reg[3], &base_addr);
    base_addr = AMBA_CORTEX_A76_UART3_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaUART_Reg[4], &base_addr);
#else
    base_addr = AMBA_CORTEX_A53_UART_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaUART_Reg[AMBA_UART_APB_CHANNEL0], &base_addr);
    base_addr = AMBA_CORTEX_A53_UART0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaUART_Reg[1], &base_addr);
    base_addr = AMBA_CORTEX_A53_UART1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaUART_Reg[2], &base_addr);
    base_addr = AMBA_CORTEX_A53_UART2_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaUART_Reg[3], &base_addr);
    base_addr = AMBA_CORTEX_A53_UART3_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaUART_Reg[4], &base_addr);
#endif

#if !defined(CONFIG_XEN_SUPPORT)
    //TODO: sync xen dtb object path to re-use this
    //TODO: parse status="ok"
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    for (i = 0U; i < AMBA_NUM_UART_CHANNEL; i++) {
        if (i != 0U) {
            offset = IO_UtilityFDTNodeOffsetByCID(offset, "ambarella,uart");
        }
        base_addr = IO_UtilityFDTPropertyU32Quick(offset, "ambarella,uart", "reg", 0U);
        if ( base_addr != 0UL ) {
#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)
            if (i == 0U) {
                base_addr = base_addr | AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR;
            } else {
                base_addr = base_addr | AMBA_CORTEX_A76_AHB_PHYS_BASE_ADDR;
            }
#endif
            AmbaMisra_TypeCast(&pAmbaUART_Reg[i], &base_addr);
        }
    }
#endif
#endif

    /* gclk_uart = CLK_REF */
    (void)AmbaRTSL_PllSetUartApbClkConfig(AMBA_PLL_UART_CLK_REF);
    (void)AmbaRTSL_PllSetUartApbClk(AmbaRTSL_PllGetClkRefFreq());
    (void)AmbaRTSL_PllSetUart0ClkConfig(AMBA_PLL_UART_CLK_REF);
    (void)AmbaRTSL_PllSetUart0Clk(AmbaRTSL_PllGetClkRefFreq());
    (void)AmbaRTSL_PllSetUart1ClkConfig(AMBA_PLL_UART_CLK_REF);
    (void)AmbaRTSL_PllSetUart1Clk(AmbaRTSL_PllGetClkRefFreq());
    (void)AmbaRTSL_PllSetUart2ClkConfig(AMBA_PLL_UART_CLK_REF);
    (void)AmbaRTSL_PllSetUart2Clk(AmbaRTSL_PllGetClkRefFreq());
    (void)AmbaRTSL_PllSetUart3ClkConfig(AMBA_PLL_UART_CLK_REF);
    (void)AmbaRTSL_PllSetUart3Clk(AmbaRTSL_PllGetClkRefFreq());

#if defined(CONFIG_XEN_SUPPORT)
    //TODO: parse status="ok"
    for (i = 1U; i < 3; i++) {
#else
    for (i = 0U; i < AMBA_NUM_UART_CHANNEL; i++) {
#endif
#if defined(CONFIG_BLD_SEQ_LINUX) || defined(CONFIG_BLD_SEQ_ATF_LINUX)
        /* TODO: List of devices used by Linux */
        if (i != 0) {
            continue;
        }
#endif
#if defined(CONFIG_SOC_CV2FS)
        if (i == AMBA_UART_R52_CONSOLE) {
            continue;
        }
#endif
        AmbaCSL_UartDisableAllIRQ(pAmbaUART_Reg[i]);   /* Disable All IRQs */
    }

    return UART_ERR_NONE;
}

/**
 *  AmbaRTSL_UartInitIrq - UART RTSL initializations IRQ
 *  @return error code
 */
UINT32 AmbaRTSL_UartInitIrq(AMBA_UART_ISR_f TxIsrCb, AMBA_UART_ISR_f RxIsrCb)
{
    const AMBA_RTSL_UART_CTRL_s *pUartCtrl;
    AMBA_INT_CONFIG_s IntConfig = {0U};

#ifdef CONFIG_BOOT_CORE_SRTOS
    UINT8 CpuOffset = CONFIG_BOOT_CORE_SRTOS;
#else
    UINT8 CpuOffset = 0U;
#endif

    UINT32 i, Irq;

    IntConfig.TriggerType = AMBA_INT_HIGH_LEVEL_TRIGGER;
    IntConfig.CpuTargets = 0x1U << CpuOffset;
    IntConfig.IrqType = AMBA_INT_FIQ;

    /* clear the pointer of Call back function when completed transactions */
    AmbaUartTxIsrCallBack = TxIsrCb;
    AmbaUartRxIsrCallBack = RxIsrCb;

#if defined(CONFIG_XEN_SUPPORT)
    //TODO: parse status="ok"
    for (i = 1U; i < 3; i++) {
#else
    for (i = 0U; i < AMBA_NUM_UART_CHANNEL; i++) {
#endif
#if defined(CONFIG_BLD_SEQ_LINUX) || defined(CONFIG_BLD_SEQ_ATF_LINUX)
        /* TODO: List of devices used by Linux */
        if (i != 0) {
            continue;
        }
#endif
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        if (i == AMBA_UART_R52_CONSOLE) {
            continue;
        }
#endif
        AmbaCSL_UartDisableAllIRQ(pAmbaUART_Reg[i]);   /* Disable All IRQs */

        pUartCtrl = &AmbaUartCtrl[i];
        Irq = pUartCtrl->IntID;
        (void)AmbaRTSL_GicIntConfig(Irq, &IntConfig, pUartCtrl->pISR, i);
        (void)AmbaRTSL_GicIntEnable(Irq);
    }

    return UART_ERR_NONE;
}
#endif

/**
 *  AmbaRTSL_UartConfig - UART Configurations
 *  @param[in] UartCh UART channel number
 *  @param[in] BaudRate Preset baud rate
 *  @param[in] pUartConfig pointer to UART configurations
 *  @return error code
 */
UINT32 AmbaRTSL_UartConfig(UINT32 UartCh, UINT32 SourceClk, UINT32 BaudRate, const AMBA_UART_CONFIG_s *pUartConfig)
{
    AMBA_UART_REG_s *pUartReg;
    AMBA_RTSL_UART_CTRL_s *pUartCtrl;
    UINT32 RetVal = UART_ERR_NONE;

    if ((UartCh >= AMBA_NUM_UART_CHANNEL) || (pUartConfig == NULL)) {
        RetVal = UART_ERR_ARG;
    } else {
        pUartReg = pAmbaUART_Reg[UartCh];
        pUartCtrl = &AmbaUartCtrl[UartCh];

        AmbaCSL_UartDisableAllIRQ(pUartReg);
        AmbaCSL_UartReset(pUartReg);

        UART_SetBaudRate(SourceClk, pUartReg, BaudRate);
        AmbaCSL_UartSetConfig(pUartReg, pUartConfig);

        pUartCtrl->TxDataSize = 0U;
        pUartCtrl->pTxDataBuf = NULL;

        if (UartCh == AMBA_UART_APB_CHANNEL0) {
            /*
             * Re-enable hardware FIFO control. (UART_fcr is write-only register!)
             *
             * FIFO mode:               enable
             * Tx empty trigger:        1/2 full
             * Rx data ready trigger:   One character in the FIFO
             */
            AmbaCSL_UartSetFifoCtrl(pUartReg, 0x31U);
            AmbaCSL_UartResetAllFIFO(pUartReg);

            /* enable all interrupts except Tx FIFO empty */
            AmbaCSL_UartEnableAllExceptTxIRQ(pUartReg);
            AmbaCSL_UartDmaDisable(pUartReg);
        } else {
            /*
             * Re-enable hardware FIFO control. (UART_fcr is write-only register!)
             *
             * FIFO mode:               enable
             * Tx empty trigger:        FIFO empty
             * Rx data ready trigger:   FIFO receive 1 char (non-dma mode)
             * DMA mode:                Tx enable
             */
            /* Only enable UART DMA Tx */
            AmbaCSL_UartSetFifoCtrl(pUartReg, 0x9U);
            AmbaCSL_UartResetAllFIFO(pUartReg);

            AmbaCSL_UartEnableAllExceptTxIRQ(pUartReg);
            AmbaCSL_UartDmaRxDisable(pUartReg);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_UartSetFlowCtrl - Set flow control
 *  @param[in] UartCh UART channel number
 *  @param[in] FlowCtrl Flow control selection
 *  @return error code
 */
UINT32 AmbaRTSL_UartSetFlowCtrl(UINT32 UartCh, UINT32 FlowCtrl)
{
    AMBA_UART_REG_s *pUartReg;
    UINT32 RetVal = UART_ERR_NONE;

    if (UartCh >= AMBA_NUM_UART_CHANNEL) {
        RetVal = UART_ERR_ARG;
    } else {
        if (FlowCtrl > AmbaUartCtrl[UartCh].FlowCtrlSupport) {
            RetVal = UART_ERR_ARG;
        } else {
            pUartReg = pAmbaUART_Reg[UartCh];
            AmbaCSL_UartSetFlowCtrl(pUartReg, FlowCtrl);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_UartHookRxReadyISR - The function registers a handler on RX data ready event of the specified UART channel
 *  @param[in] UartCh UART channel number
 *  @param[in] BufSize Number of bytes to store data frames
 *  @param[in] pBuf The buffer used to store data frames
 *  @return error code
 */
UINT32 AmbaRTSL_UartHookRxReadyISR(UINT32 UartCh, AMBA_UART_ISR_f NotifyFunc, UINT32 NotifyFuncArg)
{
    UINT32 RetVal = UART_ERR_NONE;

    if (UartCh >= AMBA_NUM_UART_CHANNEL) {
        RetVal = UART_ERR_ARG;
    } else {
        AmbaUartCtrl[UartCh].UsrRxReadyISR = NotifyFunc;
        AmbaUartCtrl[UartCh].UsrRxReadyIsrArg = NotifyFuncArg;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_UartHookRxBuf - The function registers a deferred buffer to store the data from UART receiver.
 *                           These data could be retrieved later by the AmbaUART_Read function
 *  @param[in] UartCh UART channel number
 *  @param[in] BufSize Number of bytes to store data frames
 *  @param[in] pBuf The buffer used to store data frames
 *  @return error code
 */
UINT32 AmbaRTSL_UartHookRxBuf(UINT32 UartCh, UINT32 BufSize, UINT8 *pBuf)
{
    UINT32 RetVal = UART_ERR_NONE;

    if (UartCh >= AMBA_NUM_UART_CHANNEL) {
        RetVal = UART_ERR_ARG;
    } else {
        if (pBuf == NULL) {
            AmbaUartCtrl[UartCh].RxBufCtrl.pRingBuf = NULL;
            AmbaUartCtrl[UartCh].RxBufCtrl.RingBufSize = 0U;
        } else {
            AmbaUartCtrl[UartCh].RxBufCtrl.pRingBuf = pBuf;
            AmbaUartCtrl[UartCh].RxBufCtrl.RingBufSize = BufSize;
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_UartClearRxRingBuf - clear Rx Ring Buffer
 *  @param[in] UartCh UART channel number
 *  @return error code
 */
UINT32 AmbaRTSL_UartClearRxRingBuf(UINT32 UartCh)
{
    AMBA_UART_REG_s *pUartReg;
    UINT32 RetVal = UART_ERR_NONE;

    if (UartCh >= AMBA_NUM_UART_CHANNEL) {
        RetVal = UART_ERR_ARG;
    } else {
        pUartReg = pAmbaUART_Reg[UartCh];

        AmbaCSL_UartDisableRxDataReadyIRQ(pUartReg);    /* Disable Rx Data Ready IRQ */
        AmbaCSL_UartResetRxFIFO(pUartReg);              /* Reset UART Rx FIFO */

        AmbaUartCtrl[UartCh].RxBufCtrl.ReadPtr = 0U;
        AmbaUartCtrl[UartCh].RxBufCtrl.WritePtr = 0U;

        AmbaCSL_UartEnableRxDataReadyIRQ(pUartReg);     /* Enable Rx Data Ready IRQ */
    }

    return RetVal;
}

/**
 *  AmbaRTSL_UartWrite - Send data through UART
 *  @param[in] UartCh UART channel number
 *  @param[in] TxDataSize Data size in Byte
 *  @param[in] pTxDataBuf pointer to the Tx data buffer
 *  @param[out] pActualTxSize The actual number of the TX data
 *  @return error code
 */
UINT32 AmbaRTSL_UartWrite(UINT32 UartCh, UINT32 TxDataSize, const UINT8 *pTxDataBuf, UINT32 *pActualTxSize)
{
    AMBA_UART_REG_s *pUartReg;
    UINT32 NumTxByte = 0U;
    UINT32 RetVal = UART_ERR_NONE;

    if ((UartCh >= AMBA_NUM_UART_CHANNEL) || (TxDataSize == 0U) || (pTxDataBuf == NULL)) {
        RetVal = UART_ERR_ARG;
    } else {
        pUartReg = pAmbaUART_Reg[UartCh];

        /* Fill Tx FIFO until Full */
        while (NumTxByte < TxDataSize) {
            if (AmbaCSL_UartGetTxNotFullState(pUartReg) == 0x0U) {
                break;
            } else {
                AmbaCSL_UartWriteData(pUartReg, pTxDataBuf[NumTxByte]);
                NumTxByte++;
            }
        }
    }

    if (pActualTxSize != NULL) {
        *pActualTxSize = NumTxByte;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_UartRead - Read data from UART for Non-OS only
 *  @param[in] UartCh UART channel number
 *  @param[in] RxDataSize Data size in Byte
 *  @param[in] pRxDataBuf pointer to the Rx data buffer
 *  @param[out] pActualRxSize The actual number of the RX data
 *  @return error code
 */
UINT32 AmbaRTSL_UartRead(UINT32 UartCh, UINT32 RxDataSize, UINT8 *pRxDataBuf, UINT32 *pActualRxSize)
{
    const AMBA_UART_REG_s *pUartReg;
    UINT32 NumRxByte = 0U;
    UINT32 RetVal = UART_ERR_NONE;

    if ((UartCh >= AMBA_NUM_UART_CHANNEL) || (RxDataSize == 0U) || (pRxDataBuf == NULL)) {
        RetVal = UART_ERR_ARG;
    } else {
        pUartReg = pAmbaUART_Reg[UartCh];

        /* Get Rx data if data ready */
        while (NumRxByte < RxDataSize) {
            if (AmbaCSL_UartGetDataReadyState(pUartReg) == 0U) {
                break;
            } else {
                pRxDataBuf[NumRxByte] = (UINT8)AmbaCSL_UartReadData(pUartReg);
                NumRxByte++;
            }
        }
    }

    if (pActualRxSize != NULL) {
        *pActualRxSize = NumRxByte;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_UartGetPreReadData - Fetch one Byte from UART Rx Ring Buffer
 *  @param[in] UartCh UART channel number
 *  @param[out] pRxData pointer to store Rx data
 *  @return error code
 */
UINT32 AmbaRTSL_UartGetPreReadData(UINT32 UartCh, UINT8 *pRxData)
{
    AMBA_RTSL_UART_RX_BUF_CTRL_s *pRxBufCtrl;
    UINT32 RetVal = UART_ERR_NONE;

    if ((UartCh >= AMBA_NUM_UART_CHANNEL) || (pRxData == NULL)) {
        RetVal = UART_ERR_ARG;
    } else {
        pRxBufCtrl = &(AmbaUartCtrl[UartCh].RxBufCtrl);

        if ((pRxBufCtrl->RingBufSize == 0U) || (pRxBufCtrl->pRingBuf == NULL)) {
            RetVal = UART_ERR_UNAVAIL;
        } else {
            *pRxData = pRxBufCtrl->pRingBuf[pRxBufCtrl->ReadPtr];
            pRxBufCtrl->ReadPtr++;

            /* fetch one Byte from the Ring Buffer */
            if (pRxBufCtrl->ReadPtr >= pRxBufCtrl->RingBufSize) {
                /* move farward Read Pointer */
                pRxBufCtrl->ReadPtr = 0U;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_UartGetStatus - Get UART line status
 *  @param[in] UartCh UART channel number
 *  @param[out] pStatus pointer to store current line status
 *  @return error code
 */
UINT32 AmbaRTSL_UartGetStatus(UINT32 UartCh, UINT32 *pStatus)
{
    const AMBA_UART_REG_s *pUartReg;
    UINT32 Tmp;
    AMBA_UART_STATUS_REG_s Status;
    AMBA_UART_LINE_STATUS_REG_s LineStatus;
    UINT32 UartInfo = 0U, RetVal = UART_ERR_NONE;

    if ((UartCh >= AMBA_NUM_UART_CHANNEL) || (pStatus == NULL)) {
        RetVal = UART_ERR_ARG;
    } else {
        pUartReg = pAmbaUART_Reg[UartCh];

        Tmp = pUartReg->Status;
        AmbaMisra_TypeCast32(&Status, &Tmp);
        if (Status.Busy != 0U) {
            if (Status.TxFifoEmpty != 0U) {
                UartInfo = UartInfo | 0x1U;     /* Tx Busy */
            }
            if (Status.RxFifoNotEmpty != 0U) {
                UartInfo = UartInfo | 0x2U;     /* Rx Busy */
            }
        }

        Tmp = pUartReg->LineStatus;
        AmbaMisra_TypeCast32(&LineStatus, &Tmp);
        if (LineStatus.OverrunError != 0U) {
            UartInfo = UartInfo | 0x8U;         /* Underrun error */
        }
        if (LineStatus.FramingError != 0U) {
            UartInfo = UartInfo | 0x10U;        /* FramingError */
        }
        if (LineStatus.ParityError != 0U) {
            UartInfo = UartInfo | 0x20U;        /* ParityError */
        }
        if (LineStatus.BreakIrq != 0U) {
            UartInfo = UartInfo | 0x40U;        /* BreakIrq */
        }

        *pStatus = UartInfo;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_UartGetTxEmpty - Get transmitter empty state.
 *  @param[in] UartCh UART channel number
 *  @param[out] pTxEmpty pointer to store tx empty status
 *  @return error code
 */
UINT32 AmbaRTSL_UartGetTxEmpty(UINT32 UartCh, UINT32 *pTxEmpty)
{
    AMBA_UART_REG_s *pUartReg;
    UINT32 RetVal = UART_ERR_NONE;

    if ((UartCh >= AMBA_NUM_UART_CHANNEL) || (pTxEmpty == NULL)) {
        RetVal = UART_ERR_ARG;
    } else {
        pUartReg = pAmbaUART_Reg[UartCh];

        if (AmbaCSL_UartGetTxEmptyState(pUartReg) != 0UL) {
            AmbaCSL_UartResetTxFIFO(pUartReg);  /* shall be safe to reset fifo */
            *pTxEmpty = 1U;
        } else {
            *pTxEmpty = 0U;
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_UartRecoverTxHalt - Recover uart transmitter halt
 *  @param[in] UartCh UART channel number
 */
void AmbaRTSL_UartRecoverTxHalt(UINT32 UartCh)
{
    if (UartCh < AMBA_NUM_UART_CHANNEL) {
        AmbaUartCtrl[UartCh].DebugInfo = 1U;
    }
}

/**
 *  AmbaRTSL_UartGetDataPortAddr - Get data port register address for dma
 *  @param[in] UartCh UART channel number
 *  @param[out] pDataPortAddr Data port register address
 */
void AmbaRTSL_UartGetDataPortAddr(UINT32 UartCh, ULONG *pDataPortAddr)
{
    const AMBA_UART_REG_s *pUartReg = pAmbaUART_Reg[UartCh];
    volatile const UART_DATA_BUF_REG_s *pDataBuf;

    pDataBuf = &pUartReg->DataBurst[0];
    AmbaMisra_TypeCast(pDataPortAddr, &pDataBuf);
}

/**
 *  AmbaRTSL_UartSetTxDmaCtrl - Start/Stop UART TX DMA
 *  @param[in] UartCh UART channel number
 *  @param[in] EnableFlag 0 for disable while 1 for enable
 */
void AmbaRTSL_UartSetTxDmaCtrl(UINT32 UartCh, UINT32 EnableFlag)
{
    AMBA_UART_REG_s *pUartReg = pAmbaUART_Reg[UartCh];

    if (EnableFlag == 0U) {
        AmbaCSL_UartDmaTxDisable(pUartReg);
    } else {
        AmbaCSL_UartDmaTxEnable(pUartReg);
    }
}

/**
 *  AmbaRTSL_UartSetRxDmaCtrl - Start/Stop UART RX DMA
 *  @param[in] UartCh UART channel number
 *  @param[in] EnableFlag 0 for disable while 1 for enable
 */
void AmbaRTSL_UartSetRxDmaCtrl(UINT32 UartCh, UINT32 EnableFlag)
{
    AMBA_UART_REG_s *pUartReg = pAmbaUART_Reg[UartCh];

    if (EnableFlag == 0U) {
        AmbaCSL_UartDmaRxDisable(pUartReg);
    } else {
        AmbaCSL_UartDmaRxEnable(pUartReg);
        /* For RX DMA, we enable PTIME IRQ only */
        AmbaCSL_UartEnablePtimeIRQ(pUartReg);
        /* Set RX FIFO trigger level to 8 chars when enable RX DMA */
        AmbaCSL_UartSetFifoCtrl(pUartReg, 0x89U);
    }
}

/**
 *  AmbaRTSL_UartGetRxFifoDataSize - Get data size in RX FIFO
 *  @param[in] UartCh UART channel number
 *  @return RxFifoDataSize
 */
UINT32 AmbaRTSL_UartGetRxFifoDataSize(UINT32 UartCh)
{
    const AMBA_UART_REG_s *pUartReg;
    UINT32 RetVal = UART_ERR_NONE;
    if ((UartCh >= AMBA_NUM_UART_CHANNEL) ) {
        RetVal = UART_ERR_ARG;
    } else {
        pUartReg = pAmbaUART_Reg[UartCh];
        RetVal = AmbaCSL_UartGetRxFifoDataSize(pUartReg);
    }
    return RetVal;
}

#pragma GCC push_options
#pragma GCC target("general-regs-only")
/**
 *   UART_IntRxDataReady - UART ISR for Data Ready
 *   @param[in] UartCh UART channel number
 *   @param[in] PreRegVal Post return value
 */
static UINT32 UART_IntRxDataReady(UINT32 UartCh, UINT32 PreRegVal)
{
    const AMBA_UART_REG_s *pUartReg = pAmbaUART_Reg[UartCh];
    AMBA_RTSL_UART_CTRL_s *pUartCtrl = &(AmbaUartCtrl[UartCh]);
    AMBA_RTSL_UART_RX_BUF_CTRL_s *pRxBufCtrl;
    UINT32 RegVal = PreRegVal;

    pRxBufCtrl = &(pUartCtrl->RxBufCtrl);
    if ((pRxBufCtrl->RingBufSize != 0U) && (pRxBufCtrl->pRingBuf != NULL)) {
        /* Read character from hardware FIFO */
        RegVal = AmbaCSL_UartReadData(pUartReg);

        /* Write one Byte to the Ring Buffer */
        pRxBufCtrl->pRingBuf[pRxBufCtrl->WritePtr] = (UINT8)RegVal;
        pRxBufCtrl->WritePtr++;
        if (pRxBufCtrl->WritePtr >= pRxBufCtrl->RingBufSize) {
            /* move farward Write Pointer */
            pRxBufCtrl->WritePtr = 0U;
        }
    }

    /* Notify that a new data is either in fifo or preread to deffered buffer */
    if (AmbaUartRxIsrCallBack != NULL) {
        AmbaUartRxIsrCallBack(UartCh);
    }

    /* Notify user that one UART RX data available */
    if (pUartCtrl->UsrRxReadyISR != NULL) {
        pUartCtrl->UsrRxReadyISR(pUartCtrl->UsrRxReadyIsrArg);
    }

    return RegVal;
}

/**
 *   UART_IntIrqThresholdEmpty - UART ISR for Threshold Empty
 *   @param[in] UartCh UART channel number
 */
static void UART_IntIrqThresholdEmpty(UINT32 UartCh)
{
    AMBA_UART_REG_s *pUartReg = pAmbaUART_Reg[UartCh];
    AMBA_RTSL_UART_CTRL_s *pUartCtrl = &(AmbaUartCtrl[UartCh]);

    /* Fill Tx FIFO until Full */
    while (pUartCtrl->TxDataSize > 0U) {
        if (AmbaCSL_UartGetTxNotFullState(pUartReg) == 0x0U) {
            break;
        } else {
            AmbaCSL_UartWriteData(pUartReg, *(pUartCtrl->pTxDataBuf));
            pUartCtrl->pTxDataBuf++;
            pUartCtrl->TxDataSize--;
        }
    }

    if (pUartCtrl->TxDataSize == 0U) {
        /* Tx Data have been filled into Tx FIFO completely ! */
        AmbaCSL_UartDisableTxBufEmptyIRQ(pUartReg); /* Disable Tx Buffer Empty IRQ */

        if (AmbaUartTxIsrCallBack != NULL) {
            AmbaUartTxIsrCallBack(UartCh);
        }
    }
}

static UINT32 UART_IntHandleItem(const AMBA_UART_REG_s *pUartReg, UINT32 Item)
{
    UINT32 RegVal = 0U;

    switch (Item) {
    case AMBA_UART_IRQ_RX_LINE_STATUS:
        /* reset the IRQ: Read the Line Status Register */
        RegVal = pUartReg->LineStatus;
        break;

    case AMBA_UART_IRQ_MODEM_STATUS_CHANGED:
        /* reset the IRQ: Read the Modem Status Register */
        RegVal = pUartReg->ModemStatus;
        break;

    case AMBA_UART_IRQ_BUSY:
        /* reset the IRQ: Read the Modem Status Register */
        RegVal = pUartReg->Status;
        break;

    case AMBA_UART_IRQ_TIME_OUT:
        /* reset the IRQ: Read the Receiver Buffer Register */
        RegVal = (UINT32)AmbaCSL_UartReadData(pUartReg);
        break;

    case AMBA_UART_IRQ_NO_PENDING:
    default:
        /* Do nothing */
        break;
    }

    return RegVal;
}

/**
 *  UART_IntHandler - UART ISR for reading data
 *  @param[in] Irq Interrupt ID
 *  @param[in] UartCh UART channel number
 */
static void UART_IntHandler(UINT32 Irq, UINT32 UartCh)
{
    const AMBA_UART_REG_s *pUartReg = pAmbaUART_Reg[UartCh];
    static UINT32 RegVal = 0U;
    UINT32 Item = AmbaCSL_UartGetIrqID(pUartReg);

    AmbaMisra_TouchUnused(&Irq);

    switch (Item) {
    case AMBA_UART_IRQ_RX_DATA_READY:
        RegVal = UART_IntRxDataReady(UartCh, RegVal);
        break;

    case AMBA_UART_IRQ_THRESHOLD_EMPTY:
        UART_IntIrqThresholdEmpty(UartCh);
        break;

    case AMBA_UART_IRQ_NO_PENDING:
    default:
        RegVal = UART_IntHandleItem(pUartReg, Item);
        break;
    }
}
#pragma GCC pop_options

/**
 *  UART_SetBaudRate - Set baudrate
 *  @param[in] UartCh UART channel number
 *  @param[in] pUartReg pointer to the UART registers
 *  @param[in] BaudRate Baud rate
 */
static void UART_SetBaudRate(UINT32 SourceClk, AMBA_UART_REG_s *pUartReg, UINT32 BaudRate)
{
    UINT32 Freq, Divider = BaudRate << 4U;  /* Clock frequency = 16x baudrate */

    Freq = SourceClk;
    /* Divider = gclk_uart / (16 * BaudRate) + 0.5 */
    Divider = (Freq + (Divider >> 1U)) / Divider;
    AmbaCSL_UartSetDivider(pUartReg, Divider);
}
