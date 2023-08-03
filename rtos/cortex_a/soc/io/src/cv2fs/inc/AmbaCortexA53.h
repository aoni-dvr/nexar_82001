/**
 *  @file AmbaCortexA53.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
 *
 *  This file and its contents ("Software"U are protected by intellectual property rights including, without limitation,
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
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTIONU HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISEU ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSPIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for Cortex-A53 inside CV25
 *
 */

#ifndef AMBA_CORTEX_A53_H
#define AMBA_CORTEX_A53_H

#include "AmbaMemMap.h"

#define AMBA_NUM_CORTEX_A53_CORE                    (4U)        /* Total number of Cores */

#define AMBA_CORTEX_A53_DRAM_PHYS_BASE_ADDR         0x00000000U /* External DRAM Base Address 3.5GB */
#define AMBA_CORTEX_A53_DRAM_CONFIG_PHYS_BASE_ADDR  0xdffe0000U /* DRAM Configuration Base Address 64KB */
#define AMBA_CORTEX_A53_AHB_PHYS_BASE_ADDR          0xe0000000U /* AHB Mapped Peripherals 64MB */
#define AMBA_CORTEX_A53_APB_PHYS_BASE_ADDR          0xe4000000U /* APB Mapped Peripherals 64MB */
#define AMBA_CORTEX_A53_NIC_L0_PHYS_BASE_ADDR       0xf0000000U /* NIC-400 Layer-0 Base Address 1MB */
#define AMBA_CORTEX_A53_AXI_CONFIG_PHYS_BASE_ADDR   0xf2000000U /* AXI Configuration Base Address 64KB */
#define AMBA_CORTEX_A53_GIC_PHYS_BASE_ADDR          0xf3000000U /* GIC Base Address 16MB */

#define AMBA_CORTEX_A53_DRAM_VIRT_BASE_ADDR         AMBA_CORTEX_A53_DRAM_PHYS_BASE_ADDR                   /* DRAM Virtual Base Address */
#define AMBA_CORTEX_A53_DRAM_CFG_VIRT_BASE_ADDR     AMBA_CORTEX_A53_DRAM_CONFIG_PHYS_BASE_ADDR               /* DRAM Configuration */
#define AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR          AMBA_CORTEX_A53_AHB_PHYS_BASE_ADDR                    /* Non-Secure AHB Mapped Peripherals */
#define AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR          AMBA_CORTEX_A53_APB_PHYS_BASE_ADDR                    /* Non-Secure APB Mapped Peripherals */
#define AMBA_CORTEX_A53_NIC_L0_VIRT_BASE_ADDR       AMBA_CORTEX_A53_NIC_L0_PHYS_BASE_ADDR                 /* NIC-400 Layer-0 Base Address */
#define AMBA_CORTEX_A53_AXI_CONFIG_VIRT_BASE_ADDR   AMBA_CORTEX_A53_AXI_CONFIG_PHYS_BASE_ADDR             /* AXI Configuration Base Address */
#define AMBA_CORTEX_A53_GIC_VIRT_BASE_ADDR          AMBA_CORTEX_A53_GIC_PHYS_BASE_ADDR                    /* GIC Base Address */

#define AMBA_CA53_DRAM_CTRL_BASE_ADDR               (AMBA_CORTEX_A53_DRAM_CFG_VIRT_BASE_ADDR)             /* DRAM Controller */
#define AMBA_CA53_DDR_CTRL0_BASE_ADDR               (AMBA_CORTEX_A53_DRAM_CFG_VIRT_BASE_ADDR + 0x800U)    /* DDR SDRAM Controller-0 */
#define AMBA_CA53_DDR_CTRL1_BASE_ADDR               (AMBA_CORTEX_A53_DRAM_CFG_VIRT_BASE_ADDR + 0xa00U)    /* DDR SDRAM Controller-1 */
#define AMBA_CA53_DDR_CTRL_ALL_BASE_ADDR            (AMBA_CORTEX_A53_DRAM_CFG_VIRT_BASE_ADDR + 0x600U)    /* DDR SDRAM Controller-0&1 */

#define AMBA_CA53_GIC_DISTRIBUTOR_BASE_ADDR         (AMBA_CORTEX_A53_GIC_VIRT_BASE_ADDR + 0x1000U)        /* GIC-400 Distributor */
#define AMBA_CA53_GIC_CPU_IF_BASE_ADDR              (AMBA_CORTEX_A53_GIC_VIRT_BASE_ADDR + 0x2000U)        /* GIC-400 CPU Interfaces */
#define AMBA_CA53_GIC_VIRTUAL_IF_CTRL_BASE_ADDR     (AMBA_CORTEX_A53_GIC_VIRT_BASE_ADDR + 0x4000U)        /* GIC-400 Virtual Interface Control Block */
#define AMBA_CA53_GIC_VIRTUAL_CPU_IF_BASE_ADDR      (AMBA_CORTEX_A53_GIC_VIRT_BASE_ADDR + 0x6000U)        /* GIC-400 Virtual CPU Interfaces */

#define AMBA_CA53_CPUID_BASE_ADDR                   (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x00000U)
#define AMBA_CA53_NOR_SPI_BASE_ADDR                 (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x01000U)
#define AMBA_CA53_FLASH_CPU_BASE_ADDR               (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x02000U)       /* NAND Flash Controller */
#define AMBA_CA53_SD_BASE_ADDR                      (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x04000U)       /* SD Host 0 */
#define AMBA_CA53_SDIO0_BASE_ADDR                   (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x05000U)       /* SD Host 1 */
#define AMBA_CA53_USB_BASE_ADDR                     (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x06000U)       /* USB Device */
#define AMBA_CA53_VOUT_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x08000U)
#define AMBA_CA53_GDMA_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x0c000U)       /* Graphics DMA */
#define AMBA_CA53_ETH0_MAC_BASE_ADDR                (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x0e000U)       /* Ethernet 0 (MAC) */
#define AMBA_CA53_ETH0_DMA_BASE_ADDR                (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x0f000U)       /* Ethernet 0 (DMA) */
#define AMBA_CA53_SPI_SLAVE_BASE_ADDR               (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x10000U)       /* SPI Slave */
#define AMBA_CA53_SPI0_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x11000U)       /* SPI Master 0 */
#define AMBA_CA53_SPI1_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x12000U)       /* SPI Master 1 */
#define AMBA_CA53_SPI2_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x13000U)       /* SPI Master 2 */
#define AMBA_CA53_SPI3_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x14000U)       /* SPI Master 3 */
#define AMBA_CA53_UART0_BASE_ADDR                   (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x17000U)       /* UART AHB 0 */
#define AMBA_CA53_UART1_BASE_ADDR                   (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x18000U)       /* UART AHB 1 */
#define AMBA_CA53_UART2_BASE_ADDR                   (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x19000U)       /* UART AHB 2 */
#define AMBA_CA53_UART3_BASE_ADDR                   (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x1a000U)       /* UART AHB 3 */
#define AMBA_CA53_I2S0_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x1d000U)       /* I2S Master 0 */
#define AMBA_CA53_OHCI_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x1e000U)       /* USB Host OHCI */
#define AMBA_CA53_EHCI_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x1f000U)       /* USB Host EHCI */
#define AMBA_CA53_DMA0_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x20000U)       /* DMA Engine 0 */
#define AMBA_CA53_DMA1_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x21000U)       /* DMA Engine 1 */
#define AMBA_CA53_SCRATCHPAD_NS_BASE_ADDR           (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x22000U)       /* Scratchpad (Non-Secure Part) */
#define AMBA_CA53_FLASH_IO_BASE_ADDR                (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x23000U)       /* NAND Flash I/O SMEM (8KB) */
#define AMBA_CA53_CEHU0_BASE_ADDR                   (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x25000U)       /* CEHU0 */
#define AMBA_CA53_CEHU1_BASE_ADDR                   (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x26000U)       /* CEHU1 */
#define AMBA_CA53_ECRU_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x27000U)       /* ECRU */
#define AMBA_CA53_CAN0_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x28000U)       /* CAN Bus 0 */
#define AMBA_CA53_CAN1_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x29000U)       /* CAN Bus 1 */
#define AMBA_CA53_CAN2_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x2a000U)       /* CAN Bus 2 */
#define AMBA_CA53_CAN3_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x2b000U)       /* CAN Bus 3 */
#define AMBA_CA53_CAN4_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x2c000U)       /* CAN Bus 4 */
#define AMBA_CA53_CAN5_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x2d000U)       /* CAN Bus 5 */
#define AMBA_CA53_SCRATCHPAD_S_BASE_ADDR            (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x2f000U)       /* Scratchpad (Secure Part) */
#define AMBA_CA53_SRAM_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x30000U)       /* SRAM in scratchpad */
#define AMBA_CA53_ETH1_MAC_BASE_ADDR                (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x32000U)       /* Ethernet 1 (MAC) */
#define AMBA_CA53_ETH1_DMA_BASE_ADDR                (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x33000U)       /* Ethernet 1 (DMA) */
#define AMBA_CA53_SPI4_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x34000U)       /* SPI Master 4 */
#define AMBA_CA53_SPI5_BASE_ADDR                    (AMBA_CORTEX_A53_AHB_VIRT_BASE_ADDR + 0x35000U)       /* SPI Master 5 */

#define AMBA_CA53_UART_BASE_ADDR                    (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR)                  /* UART APB */
#define AMBA_CA53_PWM0_BASE_ADDR                    (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x03000U)       /* PWM 0 */
#define AMBA_CA53_TMR0_BASE_ADDR                    (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x04000U)       /* Interval Timer 0 */
#define AMBA_CA53_TMR1_BASE_ADDR                    (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x05000U)       /* Interval Timer 1 */
#define AMBA_CA53_TMR2_BASE_ADDR                    (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x06000U)       /* Interval Timer 2 */
#define AMBA_CA53_I2C_SLAVE_BASE_ADDR               (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x07000U)       /* I2C Slave */
#define AMBA_CA53_I2C0_BASE_ADDR                    (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x08000U)       /* I2C Master 0 */
#define AMBA_CA53_I2C1_BASE_ADDR                    (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x09000U)       /* I2C Master 1 */
#define AMBA_CA53_I2C2_BASE_ADDR                    (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x0a000U)       /* I2C Master 2 */
#define AMBA_CA53_I2C3_BASE_ADDR                    (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x0b000U)       /* I2C Master 3 */
#define AMBA_CA53_PWM1_BASE_ADDR                    (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x0c000U)       /* PWM 1 */
#define AMBA_CA53_PWM2_BASE_ADDR                    (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x0d000U)       /* PWM 2 */
#define AMBA_CA53_IOMUX_BASE_ADDR                   (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x0e000U)       /* IOMUX */
#define AMBA_CA53_WDT_BASE_ADDR                     (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x0f000U)       /* Watchdog Timer */
#define AMBA_CA53_GPIO0_BASE_ADDR                   (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x10000U)       /* GPIO Group 0 */
#define AMBA_CA53_GPIO1_BASE_ADDR                   (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x11000U)       /* GPIO Group 1 */
#define AMBA_CA53_GPIO2_BASE_ADDR                   (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x12000U)       /* GPIO Group 2 */
#define AMBA_CA53_GPIO3_BASE_ADDR                   (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x13000U)       /* GPIO Group 3 */
#define AMBA_CA53_WDT_1_BASE_ADDR                   (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x14000U)       /* Watchdog Timer for Cortex-A53 core-0 */
#define AMBA_CA53_WDT_2_BASE_ADDR                   (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x15000U)       /* Watchdog Timer for Cortex-A53 core-1 */
#define AMBA_CA53_WDT_3_BASE_ADDR                   (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x16000U)       /* Watchdog Timer for Cortex-A53 core-2 */
#define AMBA_CA53_WDT_4_BASE_ADDR                   (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x17000U)       /* Watchdog Timer for Cortex-A53 core-3 */
#define AMBA_CA53_WDT_5_BASE_ADDR                   (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x18000U)       /* Watchdog Timer for Cortex-R52 */
#define AMBA_CA53_I2C4_BASE_ADDR                    (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x19000U)       /* I2C Master 4 */
#define AMBA_CA53_I2C5_BASE_ADDR                    (AMBA_CORTEX_A53_APB_VIRT_BASE_ADDR + 0x1a000U)       /* I2C Master 5 */

#endif /* AMBA_CORTEX_A53_H */
