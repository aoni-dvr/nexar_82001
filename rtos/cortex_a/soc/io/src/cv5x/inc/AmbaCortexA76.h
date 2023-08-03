/**
 *  @file AmbaCortexA76.h
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
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for Cortex-A76 inside CV5
 *
 */

#ifndef AMBA_CORTEX_A76_H
#define AMBA_CORTEX_A76_H

#include "AmbaMemMap.h"

#define AMBA_NUM_CORTEX_A76_CORE                        2U              /* Total number of Cores */

#define AMBA_CORTEX_A76_DRAM_PHYS_BASE_ADDR             0x0000000000UL  /* External DRAM Base Address 64GB */
#define AMBA_CORTEX_A76_DRAM_CONFIG_PHYS_BASE_ADDR      0x1000000000UL  /* DRAM Configuration Base Address 4GB */
#define AMBA_CORTEX_A76_AHB_PHYS_BASE_ADDR              0x20e0000000UL  /* Non-Secure AHB Mapped Peripherals 64MB */
#define AMBA_CORTEX_A76_APB_PHYS_BASE_ADDR              0x20e4000000UL  /* Non-Secure APB Mapped Peripherals 64MB */

#define AMBA_CORTEX_A76_GPV_A_PHYS_BASE_ADDR            0x20f0000000UL  /* GPV for NIC-400 A Configuration Base Address 1MB */
#define AMBA_CORTEX_A76_GPV_B_PHYS_BASE_ADDR            0x20f1000000UL  /* GPV for NIC-400 B Configuration Base Address 1MB */
#define AMBA_CORTEX_A76_AXI_CONFIG_PHYS_BASE_ADDR       0x20f2000000UL  /* AXI Configuration Base Address 64KB */
#define AMBA_CORTEX_A76_GIC_PHYS_BASE_ADDR              0x20f3000000UL  /* GIC Base Address 16MB */

#define AMBA_CORTEX_A76_PCIE_BASE_ADDR                  0x2000000000UL  /* PCIE Base Address 256MB */
#define AMBA_CORTEX_A76_PCIE_PHY_BASE_ADDR              0x2010000000UL  /* PCIE phy Base Address 256MB */
#define AMBA_CORTEX_A76_USB32_BASE_ADDR                 0x2020000000UL  /* USB32 Base Address 256MB */
#define AMBA_CORTEX_A76_USB32_PHY_BASE_ADDR             0x2030000000UL  /* USB32 phy Base Address 256MB */
#define AMBA_CORTEX_A76_PCIE_CONFIG_BASE_ADDR           0x2040000000UL  /* PCIE config Base Address 256MB */
#define AMBA_CORTEX_A76_CPHY_BASE_ADDR                  0x2050000000UL  /* CPHY Base Address 256MB */

#define AMBA_CORTEX_A76_DRAM_VIRT_BASE_ADDR             AMBA_CORTEX_A76_DRAM_PHYS_BASE_ADDR         /* DRAM Virtual Base Address */
#define AMBA_CORTEX_A76_DRAM_CONFIG_VIRT_BASE_ADDR      AMBA_CORTEX_A76_DRAM_CONFIG_PHYS_BASE_ADDR  /* DRAM Configuration */
#define AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR              AMBA_CORTEX_A76_AHB_PHYS_BASE_ADDR          /* Non-Secure AHB Mapped Peripherals */
#define AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR              AMBA_CORTEX_A76_APB_PHYS_BASE_ADDR          /* Non-Secure APB Mapped Peripherals */
#define AMBA_CORTEX_A76_GPV_A_VIRT_BASE_ADDR            AMBA_CORTEX_A76_GPV_A_PHYS_BASE_ADDR        /* GPV for NIC-400 A Configuration Base Address */
#define AMBA_CORTEX_A76_GPV_B_VIRT_BASE_ADDR            AMBA_CORTEX_A76_GPV_B_PHYS_BASE_ADDR        /* GPV for NIC-400 B Configuration Base Address */
#define AMBA_CORTEX_A76_AXI_CONFIG_VIRT_BASE_ADDR       AMBA_CORTEX_A76_AXI_CONFIG_PHYS_BASE_ADDR   /* AXI Configuration Base Address */
#define AMBA_CORTEX_A76_GIC_VIRT_BASE_ADDR              AMBA_CORTEX_A76_GIC_PHYS_BASE_ADDR          /* GIC Base Address */

#define AMBA_CORTEX_A76_DRAM_CTRL_BASE_ADDR             (AMBA_CORTEX_A76_DRAM_CONFIG_VIRT_BASE_ADDR)                /* DRAM Controller */
#define AMBA_CORTEX_A76_DDR_CTRL_ALL_BASE_ADDR          (AMBA_CORTEX_A76_DRAM_CONFIG_VIRT_BASE_ADDR + 0x4000UL)       /* DDR SDRAM Controller-0&1 */
#define AMBA_CORTEX_A76_DDR_CTRL0_BASE_ADDR             (AMBA_CORTEX_A76_DRAM_CONFIG_VIRT_BASE_ADDR + 0x5000UL)       /* DDR SDRAM Controller-0 */
#define AMBA_CORTEX_A76_DDR_CTRL1_BASE_ADDR             (AMBA_CORTEX_A76_DRAM_CONFIG_VIRT_BASE_ADDR + 0x6000UL)       /* DDR SDRAM Controller-1 */

#define AMBA_CORTEX_A76_AXI_CONFIG_BASE_ADDR            (AMBA_CORTEX_A76_AXI_CONFIG_VIRT_BASE_ADDR)                 /* AXI Configuration */
#define AMBA_CORTEX_A76_GIC_DISTRIBUTOR_BASE_ADDR       (AMBA_CORTEX_A76_GIC_VIRT_BASE_ADDR + 0x1000UL)              /* GIC-400 Distributor */
#define AMBA_CORTEX_A76_GIC_CPU_IF_BASE_ADDR            (AMBA_CORTEX_A76_GIC_VIRT_BASE_ADDR + 0x2000UL)              /* GIC-400 CPU Interfaces */
#define AMBA_CORTEX_A76_GIC_VIRTUAL_IF_CTRL_BASE_ADDR   (AMBA_CORTEX_A76_GIC_VIRT_BASE_ADDR + 0x4000UL)              /* GIC-400 Virtual Interface Control Block */
#define AMBA_CORTEX_A76_GIC_VIRTUAL_CPU_IF_BASE_ADDR    (AMBA_CORTEX_A76_GIC_VIRT_BASE_ADDR + 0x6000U)              /* GIC-400 Virtual CPU Interfaces */

#define AMBA_CORTEX_A76_CPUID_BASE_ADDR                 (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR)
#define AMBA_CORTEX_A76_NOR_SPI_BASE_ADDR               (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x01000UL)
#define AMBA_CORTEX_A76_FLASH_CPU_BASE_ADDR             (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x02000UL)   /* NAND Flash CPU */
#define AMBA_CORTEX_A76_SD_BASE_ADDR                    (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x03000UL)   /* SD Host */
#define AMBA_CORTEX_A76_SDIO0_BASE_ADDR                 (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x04000UL)   /* SDIO Host 0 */
#define AMBA_CORTEX_A76_SDIO1_BASE_ADDR                 (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x05000UL)   /* SDIO Host 1 */
#define AMBA_CORTEX_A76_USB_BASE_ADDR                   (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x06000UL)   /* USB Device */
#define AMBA_CORTEX_A76_VOUT_BASE_ADDR                  (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x08000UL)
#define AMBA_CORTEX_A76_GDMA_BASE_ADDR                  (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x0b000UL)   /* Graphics DMA */
#define AMBA_CORTEX_A76_FLASH_IO_BASE_ADDR              (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x0c000UL)   /* NAND Flash Controller FIFO */
#define AMBA_CORTEX_A76_FLASH_IO_HI_BASE_ADDR           (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x0d000UL)   /* NAND Flash Controller FIFO */
#define AMBA_CORTEX_A76_ETH_MAC_BASE_ADDR               (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x0e000UL)   /* Ethernet (MAC) */
#define AMBA_CORTEX_A76_ETH_DMA_BASE_ADDR               (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x0f000UL)   /* Ethernet (DMA) */

#define AMBA_CORTEX_A76_SSI_SLAVE_BASE_ADDR             (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x10000UL)   /* SSI Slave */
#define AMBA_CORTEX_A76_SSI0_BASE_ADDR                  (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x11000UL)   /* SSI Master 0 */
#define AMBA_CORTEX_A76_SSI1_BASE_ADDR                  (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x12000UL)   /* SSI Master 1 */
#define AMBA_CORTEX_A76_SSI2_BASE_ADDR                  (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x13000UL)   /* SSI Master 2 */
#define AMBA_CORTEX_A76_SSI3_BASE_ADDR                  (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x14000UL)   /* SSI Master 3 */
#define AMBA_CORTEX_A76_DMIC_BASE_ADDR                  (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x15000UL)   /* DMIC */
#define AMBA_CORTEX_A76_HDMI_BASE_ADDR                  (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x16000UL)   /* SDIO Host 1 */
#define AMBA_CORTEX_A76_UART0_BASE_ADDR                 (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x17000UL)   /* UART AHB 0 */
#define AMBA_CORTEX_A76_UART1_BASE_ADDR                 (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x18000UL)   /* UART AHB 1 */
#define AMBA_CORTEX_A76_UART2_BASE_ADDR                 (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x19000UL)   /* UART AHB 2 */
#define AMBA_CORTEX_A76_UART3_BASE_ADDR                 (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x1a000UL)   /* UART AHB 3 */
#define AMBA_CORTEX_A76_I2S0_BASE_ADDR                  (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x1c000UL)   /* DAI 0 */
#define AMBA_CORTEX_A76_I2S1_BASE_ADDR                  (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x1d000UL)   /* DAI 1 */

#define AMBA_CORTEX_A76_DMA0_BASE_ADDR                  (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x20000UL)   /* DMA Controller */
#define AMBA_CORTEX_A76_DMA1_BASE_ADDR                  (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x21000UL)   /* DMA Controller */
#define AMBA_CORTEX_A76_ETH1_MAC_BASE_ADDR              (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x22000UL)   /* Ethernet1 (MAC) */
#define AMBA_CORTEX_A76_ETH1_DMA_BASE_ADDR              (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x23000UL)   /* Ethernet1 (DMA) */
#define AMBA_CORTEX_A76_SCRATCHPAD_NS_BASE_ADDR         (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x24000UL)   /* Scratchpad (Non-Secure Part) */
#define AMBA_CORTEX_A76_SSI4_BASE_ADDR                  (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x25000UL)   /* SSI Master 4 */
#define AMBA_CORTEX_A76_SSI5_BASE_ADDR                  (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x26000UL)   /* SSI Master 5 */
#define AMBA_CORTEX_A76_CAN0_BASE_ADDR                  (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x27000UL)   /* CAN Bus 0 */
#define AMBA_CORTEX_A76_CAN1_BASE_ADDR                  (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x28000UL)   /* CAN Bus 1 */
#define AMBA_CORTEX_A76_SCRATCHPAD_S_BASE_ADDR          (AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR + 0x2F000UL)   /* Scratchpad (Secure Part) */

#define AMBA_CORTEX_A76_UART_BASE_ADDR                  (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR)              /* UART APB */
#define AMBA_CORTEX_A76_IR_INTERFACE_BASE_ADDR          (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x01000UL)   /* InfraRed Interface */
#define AMBA_CORTEX_A76_ADC_BASE_ADDR                   (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x03000UL)   /* ADC */
#define AMBA_CORTEX_A76_PWM0_BASE_ADDR                  (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x04000UL)   /* PWM 0 */
#define AMBA_CORTEX_A76_TIMER0_BASE_ADDR                (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x05000UL)   /* Interval Timer 0 */
#define AMBA_CORTEX_A76_TIMER1_BASE_ADDR                (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x06000UL)   /* Interval Timer 1 */
#define AMBA_CORTEX_A76_I2C_SLAVE_BASE_ADDR             (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x07000UL)   /* I2C Slave */
#define AMBA_CORTEX_A76_I2C0_BASE_ADDR                  (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x08000UL)   /* I2C Master 0 */
#define AMBA_CORTEX_A76_I2C1_BASE_ADDR                  (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x09000UL)   /* I2C Master 1 */
#define AMBA_CORTEX_A76_I2C2_BASE_ADDR                  (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x0a000UL)   /* I2C Master 2 */
#define AMBA_CORTEX_A76_I2C3_BASE_ADDR                  (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x0b000UL)   /* I2C Master 3 */
#define AMBA_CORTEX_A76_PWM1_BASE_ADDR                  (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x0c000UL)   /* PWM 1 */
#define AMBA_CORTEX_A76_PWM2_BASE_ADDR                  (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x0d000UL)   /* PWM 2 */
#define AMBA_CORTEX_A76_I2C4_BASE_ADDR                  (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x0e000UL)   /* I2C Master 4 */
#define AMBA_CORTEX_A76_I2C5_BASE_ADDR                  (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x0f000UL)   /* I2C Master 5 */

#define AMBA_CORTEX_A76_IO_MUX_BASE_ADDR                (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x10000UL)   /* IOMUX */
#define AMBA_CORTEX_A76_WDT_BASE_ADDR                   (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x12000UL)   /* Watchdog Timer */
#define AMBA_CORTEX_A76_GPIO0_BASE_ADDR                 (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x13000UL)   /* GPIO Group 0 */
#define AMBA_CORTEX_A76_GPIO1_BASE_ADDR                 (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x14000UL)   /* GPIO Group 1 */
#define AMBA_CORTEX_A76_GPIO2_BASE_ADDR                 (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x15000UL)   /* GPIO Group 2 */
#define AMBA_CORTEX_A76_GPIO3_BASE_ADDR                 (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x16000UL)   /* GPIO Group 3 */
#define AMBA_CORTEX_A76_GPIO4_BASE_ADDR                 (AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + 0x17000UL)   /* GPIO Group 4 */
#define AMBA_CORTEX_A76_CPHY0_BASE_ADDR                 (AMBA_CORTEX_A76_CPHY_BASE_ADDR)                  /* CPHY0 */
#define AMBA_CORTEX_A76_CPHY1_BASE_ADDR                 (AMBA_CORTEX_A76_CPHY_BASE_ADDR + 0x10000UL)       /* CPHY1 */

#endif /* AMBA_CORTEX_A76_H */