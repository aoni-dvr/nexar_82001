/**
 *  @file AmbaMmioBase_A32.asm
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
 *  @details for MMIO register base address definition
 *
 */

 #include "AmbaCortexA53.h"
 #include "AmbaDebugPort.h"

        .data
        .align 8

        .globl pAmbaCAN_Reg
        .globl pAmbaCEHU_Reg
        .globl pAmbaDDRC_Reg
        .globl pAmbaDMA_Reg
        .globl pAmbaDRAMC_Reg
        .globl pAmbaFIO_Reg
        .globl pAmbaGDMA_Reg
        #.globl pAmbaGIC_CpuIfReg
        #.globl pAmbaGIC_DistReg
        #.globl pAmbaGIC_VmCpuIfReg
        #.globl pAmbaGPIO_Reg
        .globl pAmbaIOMUX_Reg
        .globl pAmbaI2C_MasterReg
        .globl pAmbaI2C_SlaveReg
        .globl pAmbaI2S_Reg
        .globl pAmbaPWM_Reg
        .globl pAmbaRCT_Reg
        .globl pAmbaScratchpadS_Reg
        .globl pAmbaScratchpadNS_Reg
        .globl pAmbaSD_Reg
        .globl pAmbaSPI_MasterReg
        .globl pAmbaSPI_SlaveReg
        .globl pAmbaTMR_Regs
        #.globl pAmbaUART_Reg
        .globl pAmbaUSB_Reg
        .globl pAmbaVout_Reg
        .globl pAmbaVoutTvEnc_Reg
        .globl pAmbaVoutMixer0_Reg
        .globl pAmbaVoutDisplay0_Reg
        .globl pAmbaVoutMixer1_Reg
        .globl pAmbaVoutDisplay1_Reg
        .globl pAmbaVoutOsdRescale_Reg
        .globl pAmbaVoutTop_Reg
        .globl pAmbaVoutMipiDsiCmd0_Reg
        .globl pAmbaVoutMipiDsiCmd1_Reg
        .globl pAmbaNOR_Reg

        .globl pAmbaORC_Reg
        .globl pAmbaDRAM_Reg
        .globl pAmbaTempSensor_Reg

        pAmbaCAN_Reg:               .dword   AMBA_CA53_CAN0_BASE_ADDR, AMBA_CA53_CAN1_BASE_ADDR, AMBA_CA53_CAN2_BASE_ADDR
                                    .dword   AMBA_CA53_CAN3_BASE_ADDR, AMBA_CA53_CAN4_BASE_ADDR, AMBA_CA53_CAN5_BASE_ADDR
        pAmbaCEHU_Reg:              .dword   AMBA_CA53_CEHU0_BASE_ADDR, AMBA_CA53_CEHU1_BASE_ADDR
        pAmbaDDRC_Reg:              .dword   AMBA_CA53_DDR_CTRL0_BASE_ADDR, AMBA_CA53_DDR_CTRL1_BASE_ADDR
        pAmbaDMA_Reg:               .dword   AMBA_CA53_DMA0_BASE_ADDR, AMBA_CA53_DMA1_BASE_ADDR
        pAmbaDRAMC_Reg:             .dword   AMBA_CA53_DRAM_CTRL_BASE_ADDR
        pAmbaFIO_Reg:               .dword   AMBA_CA53_FLASH_CPU_BASE_ADDR
        pAmbaGDMA_Reg:              .dword   AMBA_CA53_GDMA_BASE_ADDR
        #pAmbaGIC_CpuIfReg:          .dword   AMBA_CA53_GIC_CPU_IF_BASE_ADDR
        #pAmbaGIC_DistReg:           .dword   AMBA_CA53_GIC_DISTRIBUTOR_BASE_ADDR
        #pAmbaGIC_VmCpuIfReg:        .dword   AMBA_CA53_GIC_VIRTUAL_CPU_IF_BASE_ADDR
        #pAmbaGPIO_Reg:              .dword   AMBA_CA53_GPIO0_BASE_ADDR, AMBA_CA53_GPIO1_BASE_ADDR, AMBA_CA53_GPIO2_BASE_ADDR
        #                            .dword   AMBA_CA53_GPIO3_BASE_ADDR
        pAmbaIOMUX_Reg:             .dword   AMBA_CA53_IOMUX_BASE_ADDR
        pAmbaI2C_MasterReg:         .dword   AMBA_CA53_I2C0_BASE_ADDR, AMBA_CA53_I2C1_BASE_ADDR, AMBA_CA53_I2C2_BASE_ADDR, AMBA_CA53_I2C3_BASE_ADDR
        pAmbaI2C_SlaveReg:          .dword   AMBA_CA53_I2C_SLAVE_BASE_ADDR
        pAmbaI2S_Reg:               .dword   AMBA_CA53_I2S0_BASE_ADDR
        pAmbaPWM_Reg:               .dword   AMBA_CA53_PWM0_BASE_ADDR, AMBA_CA53_PWM1_BASE_ADDR, AMBA_CA53_PWM2_BASE_ADDR
        pAmbaRCT_Reg:               .dword   AMBA_DBG_PORT_RCT_BASE_ADDR
        pAmbaSD_Reg:                .dword   AMBA_CA53_SD_BASE_ADDR, AMBA_CA53_SDIO0_BASE_ADDR
        pAmbaScratchpadS_Reg:       .dword   AMBA_CA53_SCRATCHPAD_S_BASE_ADDR
        pAmbaScratchpadNS_Reg:      .dword   AMBA_CA53_SCRATCHPAD_NS_BASE_ADDR
        pAmbaSPI_MasterReg:         .dword   AMBA_CA53_SPI0_BASE_ADDR, AMBA_CA53_SPI1_BASE_ADDR, AMBA_CA53_SPI2_BASE_ADDR, AMBA_CA53_SPI3_BASE_ADDR
        pAmbaSPI_SlaveReg:          .dword   AMBA_CA53_SPI_SLAVE_BASE_ADDR
        pAmbaTMR_Regs:              .dword   AMBA_CA53_TMR0_BASE_ADDR, AMBA_CA53_TMR1_BASE_ADDR
        #pAmbaUART_Reg:              .dword   AMBA_CA53_UART_BASE_ADDR
        #                            .dword   AMBA_CA53_UART0_BASE_ADDR, AMBA_CA53_UART1_BASE_ADDR, AMBA_CA53_UART2_BASE_ADDR, AMBA_CA53_UART3_BASE_ADDR
        pAmbaUSB_Reg:               .dword   AMBA_CA53_USB_BASE_ADDR
        pAmbaVout_Reg:              .dword   AMBA_CA53_VOUT_BASE_ADDR
        pAmbaVoutTvEnc_Reg:         .dword   AMBA_CA53_VOUT_BASE_ADDR
        pAmbaVoutMixer0_Reg:        .dword   (AMBA_CA53_VOUT_BASE_ADDR + 0x200)
        pAmbaVoutDisplay0_Reg:      .dword   (AMBA_CA53_VOUT_BASE_ADDR + 0x300)
        pAmbaVoutMixer1_Reg:        .dword   (AMBA_CA53_VOUT_BASE_ADDR + 0x500)
        pAmbaVoutDisplay1_Reg:      .dword   (AMBA_CA53_VOUT_BASE_ADDR + 0x600)
        pAmbaVoutOsdRescale_Reg:    .dword   (AMBA_CA53_VOUT_BASE_ADDR + 0x800)
        pAmbaVoutTop_Reg:           .dword   (AMBA_CA53_VOUT_BASE_ADDR + 0xA00)
        pAmbaVoutMipiDsiCmd0_Reg:   .dword   (AMBA_CA53_VOUT_BASE_ADDR + 0xB00)
        pAmbaVoutMipiDsiCmd1_Reg:   .dword   (AMBA_CA53_VOUT_BASE_ADDR + 0xE00)
        pAmbaNOR_Reg:               .dword   AMBA_CA53_NOR_SPI_BASE_ADDR

        pAmbaORC_Reg:               .dword   AMBA_DBG_PORT_CORC_BASE_ADDR, AMBA_DBG_PORT_EORC_BASE_ADDR, AMBA_DBG_PORT_DORC_BASE_ADDR, AMBA_DBG_PORT_VORC_BASE_ADDR
        pAmbaDRAM_Reg:              .dword   AMBA_DBG_PORT_DRAM_BASE_ADDR
        pAmbaTempSensor_Reg:        .dword   AMBA_DBG_PORT_TEMPSENSOR_BASE_ADDR

