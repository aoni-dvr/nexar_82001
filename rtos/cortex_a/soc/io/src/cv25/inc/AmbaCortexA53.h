/**
 *  @file AmbaCortexA53.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Definitions & Constants for Cortex-A53 inside CV25
 *
 */

#ifndef AMBA_CORTEX_A53_H
#define AMBA_CORTEX_A53_H

#include "AmbaMemMap.h"

#define AMBA_NUM_CORTEX_A53_CORE                        4U           /* Total number of Cores */

#define AMBA_CORTEX_A53_DRAM_PHYS_BASE_ADDR             0x00000000U  /* External DRAM Base Address 3.5GB */
#define AMBA_CORTEX_A53_DRAM_CONFIG_PHYS_BASE_ADDR      0xdffe0000U  /* DRAM Configuration Base Address 32KB */
#define AMBA_CORTEX_A53_NONSECURE_AHB_PHYS_BASE_ADDR    0xe0000000U  /* Non-Secure AHB Mapped Peripherals 64MB */
#define AMBA_CORTEX_A53_NONSECURE_APB_PHYS_BASE_ADDR    0xe4000000U  /* Non-Secure APB Mapped Peripherals 64MB */
#define AMBA_CORTEX_A53_SECURE_AHB_PHYS_BASE_ADDR       0xe8000000U  /* Secure AHB Mapped Peripherals 64MB */
#define AMBA_CORTEX_A53_SECURE_APB_PHYS_BASE_ADDR       0xec000000U  /* Secure APB Mapped Peripherals 64MB */
#define AMBA_CORTEX_A53_GPV_N0_PHYS_BASE_ADDR           0xf1000000U  /* GPV for NIC-400 N0 Configuration Base Address 1MB */
#define AMBA_CORTEX_A53_AXI_CONFIG_PHYS_BASE_ADDR       0xf2000000U  /* AXI Configuration Base Address 64KB */
#define AMBA_CORTEX_A53_GIC_PHYS_BASE_ADDR              0xf3000000U  /* GIC Base Address 16MB */

#define AMBA_CORTEX_A53_DRAM_VIRT_BASE_ADDR             AMBA_CORTEX_A53_DRAM_PHYS_BASE_ADDR             /* DRAM Virtual Base Address */
#define AMBA_CORTEX_A53_DRAM_CONFIG_VIRT_BASE_ADDR      AMBA_CORTEX_A53_DRAM_CONFIG_PHYS_BASE_ADDR      /* DRAM Configuration */
#define AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR    AMBA_CORTEX_A53_NONSECURE_AHB_PHYS_BASE_ADDR    /* Non-Secure AHB Mapped Peripherals */
#define AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR    AMBA_CORTEX_A53_NONSECURE_APB_PHYS_BASE_ADDR    /* Non-Secure APB Mapped Peripherals */
#define AMBA_CORTEX_A53_SECURE_AHB_VIRT_BASE_ADDR       AMBA_CORTEX_A53_SECURE_AHB_PHYS_BASE_ADDR       /* Secure AHB Mapped Peripherals */
#define AMBA_CORTEX_A53_SECURE_APB_VIRT_BASE_ADDR       AMBA_CORTEX_A53_SECURE_APB_PHYS_BASE_ADDR       /* Secure APB Mapped Peripherals */
#define AMBA_CORTEX_A53_GPV_N0_VIRT_BASE_ADDR           AMBA_CORTEX_A53_GPV_N0_PHYS_BASE_ADDR           /* GPV for NIC-400 N0 Configuration Base Address */
#define AMBA_CORTEX_A53_AXI_CONFIG_VIRT_BASE_ADDR       AMBA_CORTEX_A53_AXI_CONFIG_PHYS_BASE_ADDR       /* AXI Configuration Base Address */
#define AMBA_CORTEX_A53_GIC_VIRT_BASE_ADDR              AMBA_CORTEX_A53_GIC_PHYS_BASE_ADDR              /* GIC Base Address */

#define AMBA_CORTEX_A53_DRAM_CTRL_BASE_ADDR             (AMBA_CORTEX_A53_DRAM_CONFIG_VIRT_BASE_ADDR)                /* DRAM Controller */
#define AMBA_CORTEX_A53_DDR_CTRL0_BASE_ADDR             (AMBA_CORTEX_A53_DRAM_CONFIG_VIRT_BASE_ADDR + 0x800U)       /* DDR SDRAM Controller-0 */
#define AMBA_CORTEX_A53_DDR_CTRL1_BASE_ADDR             (AMBA_CORTEX_A53_DRAM_CONFIG_VIRT_BASE_ADDR + 0xa00U)       /* DDR SDRAM Controller-1 */
#define AMBA_CORTEX_A53_DDR_CTRL_ALL_BASE_ADDR          (AMBA_CORTEX_A53_DRAM_CONFIG_VIRT_BASE_ADDR + 0x600U)       /* DDR SDRAM Controller-0&1 */

#define AMBA_CORTEX_A53_AXI_CONFIG_BASE_ADDR            (AMBA_CORTEX_A53_AXI_CONFIG_VIRT_BASE_ADDR)                 /* AXI Configuration */
#define AMBA_CORTEX_A53_GIC_DISTRIBUTOR_BASE_ADDR       (AMBA_CORTEX_A53_GIC_VIRT_BASE_ADDR + 0x1000U)              /* GIC-400 Distributor */
#define AMBA_CORTEX_A53_GIC_CPU_IF_BASE_ADDR            (AMBA_CORTEX_A53_GIC_VIRT_BASE_ADDR + 0x2000U)              /* GIC-400 CPU Interfaces */
#define AMBA_CORTEX_A53_GIC_VIRTUAL_IF_CTRL_BASE_ADDR   (AMBA_CORTEX_A53_GIC_VIRT_BASE_ADDR + 0x4000U)              /* GIC-400 Virtual Interface Control Block */
#define AMBA_CORTEX_A53_GIC_VIRTUAL_CPU_IF_BASE_ADDR    (AMBA_CORTEX_A53_GIC_VIRT_BASE_ADDR + 0x6000U)              /* GIC-400 Virtual CPU Interfaces */

#define AMBA_CORTEX_A53_CPUID_BASE_ADDR                 (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x00000U)
#define AMBA_CORTEX_A53_NOR_SPI_BASE_ADDR               (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x01000U)
#define AMBA_CORTEX_A53_FLASH_CPU_BASE_ADDR             (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x02000U)   /* NAND Flash Controller */
#define AMBA_CORTEX_A53_FLASH_IO_BASE_ADDR              (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x03000U)   /* NAND Flash I/O Controller */
#define AMBA_CORTEX_A53_SD_BASE_ADDR                    (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x04000U)   /* SDHC Host */
#define AMBA_CORTEX_A53_SDIO0_BASE_ADDR                 (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x05000U)   /* SDIO Host 0 */
#define AMBA_CORTEX_A53_USB_BASE_ADDR                   (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x06000U)   /* USB Device */
#define AMBA_CORTEX_A53_VOUT_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x08000U)
#define AMBA_CORTEX_A53_HDMI_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x0b000U)
#define AMBA_CORTEX_A53_GDMA_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x0c000U)   /* Graphics DMA */
#define AMBA_CORTEX_A53_ETH_MAC_BASE_ADDR               (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x0e000U)   /* Ethernet (MAC) */
#define AMBA_CORTEX_A53_ETH_DMA_BASE_ADDR               (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x0f000U)   /* Ethernet (DMA) */
#define AMBA_CORTEX_A53_SSI_SLAVE_BASE_ADDR             (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x10000U)   /* SSI Slave */
#define AMBA_CORTEX_A53_SSI0_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x11000U)   /* SSI Master 0 */
#define AMBA_CORTEX_A53_SSI1_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x12000U)   /* SSI Master 1 */
#define AMBA_CORTEX_A53_SSI2_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x13000U)   /* SSI Master 2 */
#define AMBA_CORTEX_A53_SSI3_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x14000U)   /* SSI Master 3 */
#define AMBA_CORTEX_A53_DMIC_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x15000U)   /* DMIC */
#define AMBA_CORTEX_A53_SDIO1_BASE_ADDR                 (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x16000U)   /* SDIO Host 1 */
#define AMBA_CORTEX_A53_UART0_BASE_ADDR                 (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x17000U)   /* UART AHB 0 */
#define AMBA_CORTEX_A53_UART1_BASE_ADDR                 (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x18000U)   /* UART AHB 1 */
#define AMBA_CORTEX_A53_UART2_BASE_ADDR                 (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x19000U)   /* UART AHB 2 */
#define AMBA_CORTEX_A53_UART3_BASE_ADDR                 (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x1a000U)   /* UART AHB 3 */
#define AMBA_CORTEX_A53_I2S0_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x1d000U)   /* I2S Master 0 */
#define AMBA_CORTEX_A53_OHCI_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x1e000U)   /* OHCI */
#define AMBA_CORTEX_A53_EHCI_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x1f000U)   /* EHCI */
#define AMBA_CORTEX_A53_DMA0_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x20000U)   /* DMA Controller */
#define AMBA_CORTEX_A53_DMA1_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x21000U)   /* DMA Controller */
#define AMBA_CORTEX_A53_SCRATCHPAD_NS_BASE_ADDR         (AMBA_CORTEX_A53_NONSECURE_AHB_VIRT_BASE_ADDR + 0x22000U)   /* Scratchpad (Non-Secure Part) */

#define AMBA_CORTEX_A53_SCRATCHPAD_S_BASE_ADDR          (AMBA_CORTEX_A53_SECURE_AHB_VIRT_BASE_ADDR + 0x01000U)      /* Scratchpad (Secure Part) */
#define AMBA_CORTEX_A53_CAN0_BASE_ADDR                  (AMBA_CORTEX_A53_SECURE_AHB_VIRT_BASE_ADDR + 0x04000U)      /* CAN Bus */

#define AMBA_CORTEX_A53_UART_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR)              /* UART APB */
#define AMBA_CORTEX_A53_IR_INTERFACE_BASE_ADDR          (AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR + 0x01000U)   /* InfraRed Interface */
#define AMBA_CORTEX_A53_ADC_BASE_ADDR                   (AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR + 0x03000U)   /* ADC */
#define AMBA_CORTEX_A53_PWM0_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR + 0x04000U)   /* PWM 0 */
#define AMBA_CORTEX_A53_TIMER0_BASE_ADDR                (AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR + 0x05000U)   /* Interval Timer 0 */
#define AMBA_CORTEX_A53_TIMER1_BASE_ADDR                (AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR + 0x06000U)   /* Interval Timer 1 */
#define AMBA_CORTEX_A53_I2C_SLAVE_BASE_ADDR             (AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR + 0x07000U)   /* I2C Slave */
#define AMBA_CORTEX_A53_I2C0_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR + 0x08000U)   /* I2C Master 0 */
#define AMBA_CORTEX_A53_I2C1_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR + 0x09000U)   /* I2C Master 1 */
#define AMBA_CORTEX_A53_I2C2_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR + 0x0a000U)   /* I2C Master 2 */
#define AMBA_CORTEX_A53_I2C3_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR + 0x0b000U)   /* I2C Master 3 */
#define AMBA_CORTEX_A53_PWM1_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR + 0x0c000U)   /* PWM 1 */
#define AMBA_CORTEX_A53_PWM2_BASE_ADDR                  (AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR + 0x0d000U)   /* PWM 2 */

#define AMBA_CORTEX_A53_IO_MUX_BASE_ADDR                (AMBA_CORTEX_A53_SECURE_APB_VIRT_BASE_ADDR)                 /* IOMUX */
#define AMBA_CORTEX_A53_WDT_BASE_ADDR                   (AMBA_CORTEX_A53_SECURE_APB_VIRT_BASE_ADDR + 0x02000U)      /* Watchdog Timer */
#define AMBA_CORTEX_A53_GPIO0_BASE_ADDR                 (AMBA_CORTEX_A53_SECURE_APB_VIRT_BASE_ADDR + 0x03000U)      /* GPIO Group 0 */
#define AMBA_CORTEX_A53_GPIO1_BASE_ADDR                 (AMBA_CORTEX_A53_SECURE_APB_VIRT_BASE_ADDR + 0x04000U)      /* GPIO Group 1 */
#define AMBA_CORTEX_A53_GPIO2_BASE_ADDR                 (AMBA_CORTEX_A53_SECURE_APB_VIRT_BASE_ADDR + 0x05000U)      /* GPIO Group 2 */
#define AMBA_CORTEX_A53_GPIO3_BASE_ADDR                 (AMBA_CORTEX_A53_SECURE_APB_VIRT_BASE_ADDR + 0x06000U)      /* GPIO Group 3 */

#endif /* AMBA_CORTEX_A53_H */
