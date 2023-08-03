/**
 *  @file AmbaMmioBase_A64.asm
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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

        .globl pAmbaADC_Reg
        .globl pAmbaCAN_Reg
        .globl pAmbaDDRC_Reg
        .globl pAmbaDMA_Reg
        .globl pAmbaDRAMC_Reg
        .globl pAmbaFIO_Reg
        .globl pAmbaHDMI_Reg
        .globl pAmbaIOMUX_Reg
        .globl pAmbaI2C_MasterReg
        .globl pAmbaI2C_SlaveReg
        .globl pAmbaI2S_Reg
        .globl pAmbaIR_Reg
        .globl pAmbaPWM_Reg
        .globl pAmbaRCT_Reg
        .globl pAmbaScratchpadS_Reg
        .globl pAmbaScratchpadNS_Reg
        .globl pAmbaSD_Reg
        .globl pAmbaSPI_MasterReg
        .globl pAmbaSPI_SlaveReg
        .globl pAmbaTMR_Regs
        .globl pAmbaUSB_Reg
        .globl pAmbaNOR_Reg

        .globl pAmbaORC_Reg
        .globl pAmbaDRAM_Reg

        pAmbaADC_Reg:               .dword  AMBA_CORTEX_A53_ADC_BASE_ADDR
        pAmbaCAN_Reg:               .dword  AMBA_CORTEX_A53_CAN0_BASE_ADDR
        pAmbaDMA_Reg:               .dword  AMBA_CORTEX_A53_DMA0_BASE_ADDR, AMBA_CORTEX_A53_DMA1_BASE_ADDR
        pAmbaDRAMC_Reg:             .dword  AMBA_CORTEX_A53_DRAM_CTRL_BASE_ADDR
        pAmbaFIO_Reg:               .dword  AMBA_CORTEX_A53_FLASH_CPU_BASE_ADDR
        pAmbaHDMI_Reg:              .dword  AMBA_CORTEX_A53_HDMI_BASE_ADDR
        pAmbaIOMUX_Reg:             .dword  AMBA_CORTEX_A53_IO_MUX_BASE_ADDR
        pAmbaI2C_MasterReg:         .dword  AMBA_CORTEX_A53_I2C0_BASE_ADDR, AMBA_CORTEX_A53_I2C1_BASE_ADDR, AMBA_CORTEX_A53_I2C2_BASE_ADDR, AMBA_CORTEX_A53_I2C3_BASE_ADDR
        pAmbaI2C_SlaveReg:          .dword  AMBA_CORTEX_A53_I2C_SLAVE_BASE_ADDR
        pAmbaI2S_Reg:               .dword  AMBA_CORTEX_A53_I2S0_BASE_ADDR
        pAmbaIR_Reg:                .dword  AMBA_CORTEX_A53_IR_INTERFACE_BASE_ADDR
        pAmbaPWM_Reg:               .dword  AMBA_CORTEX_A53_PWM0_BASE_ADDR, AMBA_CORTEX_A53_PWM1_BASE_ADDR, AMBA_CORTEX_A53_PWM2_BASE_ADDR
        pAmbaRCT_Reg:               .dword  AMBA_DBG_PORT_RCT_BASE_ADDR
        pAmbaScratchpadS_Reg:       .dword  AMBA_CORTEX_A53_SCRATCHPAD_S_BASE_ADDR
        pAmbaScratchpadNS_Reg:      .dword  AMBA_CORTEX_A53_SCRATCHPAD_NS_BASE_ADDR
        pAmbaSD_Reg:                .dword  AMBA_CORTEX_A53_SD_BASE_ADDR, AMBA_CORTEX_A53_SDIO0_BASE_ADDR
        pAmbaSPI_MasterReg:         .dword  AMBA_CORTEX_A53_SSI0_BASE_ADDR, AMBA_CORTEX_A53_SSI1_BASE_ADDR, AMBA_CORTEX_A53_SSI2_BASE_ADDR, AMBA_CORTEX_A53_SSI3_BASE_ADDR
        pAmbaSPI_SlaveReg:          .dword  AMBA_CORTEX_A53_SSI_SLAVE_BASE_ADDR
        pAmbaTMR_Regs:              .dword  AMBA_CORTEX_A53_TIMER0_BASE_ADDR, AMBA_CORTEX_A53_TIMER1_BASE_ADDR
        pAmbaUSB_Reg:               .dword  AMBA_CORTEX_A53_USB_BASE_ADDR
        pAmbaNOR_Reg:               .dword  AMBA_CORTEX_A53_NOR_SPI_BASE_ADDR

        pAmbaORC_Reg:               .dword  AMBA_DBG_PORT_CORC_BASE_ADDR, AMBA_DBG_PORT_EORC_BASE_ADDR, AMBA_DBG_PORT_DORC_BASE_ADDR, AMBA_DBG_PORT_VORC_BASE_ADDR
        pAmbaDRAM_Reg:              .dword  AMBA_DBG_PORT_DRAM_BASE_ADDR
        pAmbaDDRC_Reg:              .dword  AMBA_CORTEX_A53_DDR_CTRL0_BASE_ADDR

