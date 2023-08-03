/**
 *  @file AmbaB8_IoMap.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Definitions & Constants for B6 Registers' Address
 *
 */

#ifndef AMBA_B8_IO_MAP_H
#define AMBA_B8_IO_MAP_H

#define B8_AHB_BASE_ADDR                       0xe0000000U
#define B8_APB_BASE_ADDR                       0xe8000000U

#define B8_AHB_RCT_BASE_ADDR                   (B8_AHB_BASE_ADDR)
#define B8_AHB_PACKER0_BASE_ADDR               (B8_AHB_BASE_ADDR + 0x0800U)
#define B8_AHB_DEPACKER_4_BASE_ADDR            (B8_AHB_BASE_ADDR + 0x1800U)
#define B8_AHB_DEPACKER_0_BASE_ADDR            (B8_AHB_BASE_ADDR + 0x2800U)
#define B8_AHB_FSYNC_BASE_ADDR                 (B8_AHB_BASE_ADDR + 0x4800U)
#define B8_AHB_MIPI_DBG_BASE_ADDR              (B8_AHB_BASE_ADDR + 0x5000U)
#define B8_AHB_CFG_CODEC_BASE_ADDR             (B8_AHB_BASE_ADDR + 0x6000U)
#define B8_AHB_MERGER_BASE_ADDR                (B8_AHB_BASE_ADDR + 0x9000U)
#define B8_AHB_PWM_ENCODER_BASE_ADDR           (B8_AHB_BASE_ADDR + 0x9800U)
#define B8_AHB_PWM_DECODER_BASE_ADDR           (B8_AHB_BASE_ADDR + 0xa000U)
#define B8_AHB_VIC0_BASE_ADDR                  (B8_AHB_BASE_ADDR + 0xa800U)
#define B8_AHB_SSI_BASE_ADDR                   (B8_AHB_BASE_ADDR + 0xb800U)
#define B8_AHB_VIN0_BASE_ADDR                  (B8_AHB_BASE_ADDR + 0xc000U)
#define B8_AHB_VIN1_BASE_ADDR                  (B8_AHB_BASE_ADDR + 0xc400U)
#define B8_AHB_PRESCALE0_BASE_ADDR             (B8_AHB_BASE_ADDR + 0xd000U)
#define B8_AHB_PRESCALE1_BASE_ADDR             (B8_AHB_BASE_ADDR + 0xd400U)
#define B8_AHB_COMPRESSOR0_BASE_ADDR           (B8_AHB_BASE_ADDR + 0xd800U)
#define B8_AHB_DECOMPRESSOR0_BASE_ADDR         (B8_AHB_BASE_ADDR + 0xe000U)
#define B8_AHB_SCRATCHPAD_BASE_ADDR            (B8_AHB_BASE_ADDR + 0xe800U)
#define B8_AHB_VOUT0_BASE_ADDR                 (B8_AHB_BASE_ADDR + 0xf000U)
#define B8_AHB_VOUT1_BASE_ADDR                 (B8_AHB_BASE_ADDR + 0xf800U)

#define B8_APB_IOMUX_BASE_ADDR                 (B8_APB_BASE_ADDR)
#define B8_APB_GPIO_BASE_ADDR                  (B8_APB_BASE_ADDR + 0x1000U)
#define B8_APB_I2C0_BASE_ADDR                  (B8_APB_BASE_ADDR + 0x2000U)
#define B8_APB_I2C_IOCTRL_BASE_ADDR            (B8_APB_BASE_ADDR + 0x4000U)
#define B8_APB_PWM_BASE_ADDR                   (B8_APB_BASE_ADDR + 0x5000U)

#endif /* AMBA_B8_IO_MAP_H */
