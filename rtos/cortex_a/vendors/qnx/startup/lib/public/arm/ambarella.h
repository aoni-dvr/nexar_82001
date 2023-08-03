/**
 *  @file ambarella.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for Ambarella BSP
 *
 */

#ifndef    __ARM_AMBA_H_INCLUDED
#define    __ARM_AMBA_H_INCLUDED

/* -------------------------------------------------------------------------
 * General Information
 * -------------------------------------------------------------------------
 */

/* -------------------------------------------------------------------------
 * Devices
 * -------------------------------------------------------------------------
 */
#define AMBA_HWI_UART0                       "uart0"
#define AMBA_HWI_UART1                       "uart1"
#define AMBA_HWI_WDT                         "wdt"
#define AMBA_HWI_DMA                         "dma"
#define AMBA_HWI_ENET                        "gem"
#define AMBA_HWI_ENET_MDIO                   "gem,mdio"

/* -------------------------------------------------------------------------
 * SDRAM
 * -------------------------------------------------------------------------
 */
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#define AMBA_SDRAM_BANK1_BASE        CONFIG_DTB_LOADADDR + CONFIG_DTB_SIZE
#else
#define AMBA_SDRAM_BANK1_BASE        0x20000
#endif

#define AMBA_SDRAM_BANK2_BASE        MEG(2 * 1024)
#define AMBA_SDRAM_BANK1_SIZE        (MEG(2048) - AMBA_SDRAM_BANK1_BASE)
#define AMBA_SDRAM_BANK2_SIZE        MEG(2048)

#define AMBA_SDRAM_64_BIT_BASE                 0x00100000

/* -------------------------------------
 * EMAC
 * -------------------------------------
 */

#define AMBA_EMAC0_BASE           0xE000B000
#define AMBA_EMAC1_BASE           0xE000C000
#define AMBA_EMAC_SIZE            0x1000

/* -------------------------------------
 * I2C
 * -------------------------------------
 */
#define AMBA_I2CS_BASE_ADDR            0xe4007000
#define AMBA_I2C0_BASE_ADDR            0xe4008000
#define AMBA_I2C1_BASE_ADDR            0xe4009000
#define AMBA_I2C2_BASE_ADDR            0xe400A000
#define AMBA_I2C3_BASE_ADDR            0xe400B000
#define AMBA_I2C_REG_SIZE          0x1000

/* -------------------------------------
 * GPIO
 * -------------------------------------
 */
#define AMBA_IOMUX_BASE             0xEC000000
#define AMBA_IOMUX_SIZE             0x1000

#define AMBA_MISC_BASE              0xEC001000
#define AMBA_MISC_SIZE              0x1000

/* -------------------------------------------------------------------------
 * ARM / SMP
 * -------------------------------------------------------------------------
 */
/* ARM Generic Interrupt Controller (GIC400) Distributor base */

#define AMBA_GIC_BASE               0xF3001000
/* CPU interface registers offset */
#define AMBA_GICC_OFFSET            0x1000

/* -------------------------------------------------------------------------
 * Clocks
 * -------------------------------------------------------------------------
 */
#define AMBA_RCT_BASE               0xed080000
#define AMBA_RCT_SIZE               0x1000

#define AMBA_REF_CLOCK              24000000UL

#define AMBA_RCT_SW_RESET_OFFSET    (0x68U)
/* -------------------------------------------------------------------------
 * Timers
 * -------------------------------------------------------------------------
 */
#ifdef CONFIG_SOC_CV2FS
#define RTC_CUR_TIME_ADDR       0xe002f034U
#elif CONFIG_SOC_CV2
#define RTC_CUR_TIME_ADDR       0xe8001030U
#endif

/* Interrupt */
#ifdef CONFIG_SOC_CV2FS
#define AMBA_APB_UART_INT   AMBA_INT_SPI_ID053_UART_APB
#else
#define AMBA_APB_UART_INT   AMBA_INT_SPI_ID53_UART_APB
#endif

/* Channel status register */
#define AMBA_UART_SR_RXFULL     0x00000010     /* RX FIFO full */
#define AMBA_UART_SR_RXNOTEMT   0x00000008     /* RX not empty */
#define AMBA_UART_SR_TXEMPTY    0x00000004     /* TX empty */
#define AMBA_UART_SR_TXNOTFULL  0x00000002     /* TX not FULL */
#define AMBA_UART_SR_BUSY       0x00000001     /* UART busy */

/* UART base addresses */
#define    AMBA_UART0_BASE        0xe4000000
#define    AMBA_UART1_BASE        0xe0017000

#define    AMBA_UART_SIZE         0x1000

/* UART registers */
#define AMBA_UART_FIFO_REG      0x0 /* Transmit and Receive FIFO */
#define AMBA_UART_IRQC_REG      0X4 /* IRQ CTRL */
#define AMBA_UART_FIFOC_REG     0X8 /* FIFO CTRL */
#define AMBA_UART_LINEC_REG     0XC /* LINE CTRL */
#define AMBA_UART_MODEMCR_REG   0x10 /* Modem control register */
#define AMBA_UART_LINESR_REG    0x14 /* Line Status register */
#define AMBA_UART_MODEMSR_REG   0x18 /* Modem status register */
#define AMBA_UART_DMACR_REG     0x28 /* DMA CTRL register */
#define AMBA_UART_SR_REG        0x7c /* Channel status register */
#define AMBA_UART_RESET_REG     0x88
#define AMBA_UART_HaltTx_REG    0xA4

/* Interrupt registers */
#define AMBA_UART_IRQ_MODEM_STATUS_CHANGED  0x00U
#define AMBA_UART_IRQ_NO_PENDING            0x01U
#define AMBA_UART_IRQ_THRESHOLD_EMPTY       0x02U
#define AMBA_UART_IRQ_RX_DATA_READY         0x04U
#define AMBA_UART_IRQ_RX_LINE_STATUS        0x06U
#define AMBA_UART_IRQ_BUSY                  0x07U
#define AMBA_UART_IRQ_TIME_OUT              0x0CU

/* -------------------------------------
 * SCRATCHPAD_NS
 * -------------------------------------
 */
#define AMBA_SKPDNS_BASE        0xE0022000
#define AMBA_SKPDNS_SIZE        0x1000

/* -------------------------------------
 * CAN Bus
 * -------------------------------------
 */
#ifdef CONFIG_SOC_CV2FS
#define AMBA_CAN0_BASE       0xE0028000
#define AMBA_CAN1_BASE       0xE0029000
#define AMBA_CAN2_BASE       0xE002A000
#define AMBA_CAN3_BASE       0xE002B000
#define AMBA_CAN4_BASE       0xE002C000
#define AMBA_CAN5_BASE       0xE002D000
#define AMBA_CAN_SIZE        0x1000
#elif CONFIG_SOC_CV2
#define AMBA_CAN0_BASE       0xE8004000
#define AMBA_CAN1_BASE       0xE8005000
#define AMBA_CAN_SIZE        0x1000
#endif

#endif //__ARM_AMBA_H_INCLUDED

