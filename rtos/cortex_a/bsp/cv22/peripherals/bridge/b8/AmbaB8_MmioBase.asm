/**
 *  @file AmbaB8_MmioBase.asm
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details for B8 MMIO register base address definition
 *
 */

 #include "AmbaB8_IoMap.h"

        .data
        .align 4
        .arm

        .globl pAmbaB8_PllReg
        .globl pAmbaB8_PackerReg
        .globl pAmbaB8_Depacker4Reg
        .globl pAmbaB8_DepackerReg
        .globl pAmbaB8_FsyncReg
        .globl pAmbaB8_PhyReg
        .globl pAmbaB8_CodecReg
        .globl pAmbaB8_MergerReg
        .globl pAmbaB8_PwmEncReg
        .globl pAmbaB8_PwmDecReg
        .globl pAmbaB8_VicReg
        .globl pAmbaB8_SpiReg
        .globl pAmbaB8_VinReg
        .globl pAmbaB8_PrescalerReg
        .globl pAmbaB8_CompressorReg
        .globl pAmbaB8_DeCompressorReg
        .globl pAmbaB8_ScratchpadReg
        .globl pAmbaB8_VoutReg
        .globl pAmbaB8_IoMuxReg
        .globl pAmbaB8_GPIOReg
        .globl pAmbaB8_I2c0_BaseAddr
        .globl pAmbaB8_IoCtrlReg
        .globl pAmbaB8_PwmReg

        pAmbaB8_PllReg:          .word    B8_AHB_RCT_BASE_ADDR
        pAmbaB8_PackerReg:       .word    B8_AHB_PACKER0_BASE_ADDR
        pAmbaB8_Depacker4Reg:    .word    B8_AHB_DEPACKER_4_BASE_ADDR
        pAmbaB8_DepackerReg:     .word    B8_AHB_DEPACKER_0_BASE_ADDR
        pAmbaB8_FsyncReg:        .word    B8_AHB_FSYNC_BASE_ADDR
        pAmbaB8_PhyReg:          .word    B8_AHB_MIPI_DBG_BASE_ADDR
        pAmbaB8_CodecReg:        .word    B8_AHB_CFG_CODEC_BASE_ADDR
        pAmbaB8_MergerReg:       .word    B8_AHB_MERGER_BASE_ADDR
        pAmbaB8_PwmEncReg:       .word    B8_AHB_PWM_ENCODER_BASE_ADDR
        pAmbaB8_PwmDecReg:       .word    B8_AHB_PWM_DECODER_BASE_ADDR
        pAmbaB8_VicReg:          .word    B8_AHB_VIC0_BASE_ADDR
        pAmbaB8_SpiReg:          .word    B8_AHB_SSI_BASE_ADDR
        pAmbaB8_VinReg:          .word    B8_AHB_VIN0_BASE_ADDR, B8_AHB_VIN1_BASE_ADDR
        pAmbaB8_PrescalerReg:    .word    B8_AHB_PRESCALE0_BASE_ADDR, B8_AHB_PRESCALE1_BASE_ADDR
        pAmbaB8_CompressorReg:   .word    B8_AHB_COMPRESSOR0_BASE_ADDR
        pAmbaB8_DeCompressorReg: .word    B8_AHB_DECOMPRESSOR0_BASE_ADDR
        pAmbaB8_ScratchpadReg:   .word    B8_AHB_SCRATCHPAD_BASE_ADDR
        pAmbaB8_VoutReg:         .word    B8_AHB_VOUT0_BASE_ADDR, B8_AHB_VOUT1_BASE_ADDR

        pAmbaB8_IoMuxReg:        .word    B8_APB_IOMUX_BASE_ADDR
        pAmbaB8_GPIOReg:         .word    B8_APB_GPIO_BASE_ADDR
        pAmbaB8_I2c0_BaseAddr:   .word    B8_APB_I2C0_BASE_ADDR
        pAmbaB8_IoCtrlReg:       .word    B8_APB_I2C_IOCTRL_BASE_ADDR
        pAmbaB8_PwmReg:          .word    B8_APB_PWM_BASE_ADDR

