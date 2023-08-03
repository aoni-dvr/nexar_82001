/**
 *  @file AmbaAudio_NAU8822.h
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
 *  @details NAU8822 audio AD/DA CODEC driver
 *
 */

#ifndef AMBA_AUDIO_NAU8822_H
#define AMBA_AUDIO_NAU8822_H

// Example in bsp
/*
#ifdef CONFIG_AUDIO_NAU8822
//I2C
#include "AmbaAudio_NAU8822.h"

    extern AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_NAU8822Obj;
    AMBA_AUDIO_CODEC_CTRL_s CodecCtrl;

    CodecCtrl.CtrlMode = AMBA_AUDIO_CODEC_I2C_CONTROL;
    CodecCtrl.I2cCtrl.I2cChanNo = AMBA_I2C_CHANNEL0;
    CodecCtrl.I2cCtrl.SlaveAddr = NAU8822_I2C_ADDR
    CodecCtrl.I2cCtrl.I2cSpeed = AMBA_I2C_SPEED_FAST;
    AmbaAudio_CodecHook(AMBA_AUDIO_CODEC_0, &AmbaAudio_NAU8822Obj, &CodecCtrl);

    AmbaAudio_CodecInit(AMBA_AUDIO_CODEC_0);
    AmbaAudio_CodecModeConfig(AMBA_AUDIO_CODEC_0, I2S);
    AmbaAudio_CodecFreqConfig(AMBA_AUDIO_CODEC_0, 48000);
    AmbaAudio_CodecSetInput(AMBA_AUDIO_CODEC_0, AMBA_AUDIO_CODEC_LINE_IN);
    AmbaAudio_CodecSetOutput(AMBA_AUDIO_CODEC_0, AMBA_AUDIO_CODEC_LINE_OUT);
    AmbaAudio_CodecSetMute(AMBA_AUDIO_CODEC_0, 0);
#endif
*/
#ifndef AMBA_AUDIO_CODEC_H
#include "AmbaAudio_CODEC.h"
#endif

#define NAU8822_I2C_ADDR        0x34U
#define NAU8822AYG_I2C_ADDR     0x3AU
#define NAU8822_REG_NUM         58U

#define NAU8822_SW_RESET_REG            0x0U
#define NAU8822_PWR_MANAGE1_REG         0x1U
#define NAU8822_PWR_MANAGE2_REG         0x2U
#define NAU8822_PWR_MANAGE3_REG         0x3U
#define NAU8822_AU_INTERFACE_CTRL_REG   0x4U
//#define NAU8822_COMPANDING_CTRL_REG     0x5U
#define NAU8822_CLK_CTRL1_REG           0x6U
#define NAU8822_CLK_CTRL2_REG           0x7U
#define NAU8822_DAC_CTRL_REG            0xaU
#define NAU8822_ADC_CTRL_REG            0xeU
#define NAU8822_RSPK_SUBMIX_REG         0x2bU
#define NAU8822_INPUT_CTRL_REG          0x2cU
#define NAU8822_L_IN_PGA_VOL_REG        0x2dU
#define NAU8822_R_IN_PGA_VOL_REG        0x2eU
#define NAU8822_L_ADC_BST_REG           0x2fU
#define NAU8822_R_ADC_BST_REG           0x30U

#define NAU8822_R01H_REFIMP_MASK        0x1fcUL
#define NAU8822_R01H_REFIMP_75K         0x1UL
//#define NAU8822_R01H_REFIMP_5K          0x3UL
#define NAU8822_R01H_IOBUFEN            (1UL << 2UL)
#define NAU8822_R01H_ABIASEN            (1UL << 3UL)
#define NAU8822_R01H_MICBIASEN          (1UL << 4UL)
#define NAU8822_R02H_LADCEN             1U
#define NAU8822_R02H_RADCEN             (1UL << 1UL)
#define NAU8822_R02H_LPGAEN             (1UL << 2UL)
#define NAU8822_R02H_RPGAEN             (1UL << 3UL)
#define NAU8822_R02H_LBSTEN             (1UL << 4UL)
#define NAU8822_R02H_RBSTEN             (1UL << 5UL)
#define NAU8822_R02H_LHPEN              (1UL << 7UL)
#define NAU8822_R02H_RHPEN              (1UL << 8UL)
#define NAU8822_R03H_LDACEN             1U
#define NAU8822_R03H_RDACEN             (1UL << 1UL)
#define NAU8822_R03H_LMIXEN             (1UL << 2UL)
#define NAU8822_R03H_RMIXEN             (1UL << 3UL)
#define NAU8822_R03H_RSPKEN             (1UL << 5UL)
#define NAU8822_R03H_LSPKEN             (1UL << 6UL)
#define NAU8822_R04H_WLEN_MASK          0x19fU
//#define NAU8822_R04H_WLEN_16B           0x19fU
#define NAU8822_R04H_WLEN_24B           (1UL << 6UL)
//#define NAU8822_R05H_ADDAP              1U
#define NAU8822_R06H_CODEC_SLAVE        0x000U
#define NAU8822_R07H_SMPLR_MASK         0x1f1U
#define NAU8822_R07H_SMPLR_48K          0x0U
#define NAU8822_R07H_SMPLR_32K          0x2U
#define NAU8822_R07H_SMPLR_24K          0x4U
#define NAU8822_R07H_SMPLR_16K          0x6U
#define NAU8822_R07H_SMPLR_12K          0x8U
#define NAU8822_R07H_SMPLR_8K           0xaU
#define NAU8822_R0AH_DACOS              (1UL << 3UL)
#define NAU8822_R0AH_SOFTMT             (1UL << 6UL)
#define NAU8822_R0EH_ADCOS              (1UL << 3UL)
#define NAU8822_R0EH_HPFEN              (1UL << 8UL)
#define NAU8822_R2BH_RSUBBYP            (1UL << 4UL)
#define NAU8822_R2DH_PGA_MASK           0x1c0U
#define NAU8822_R2DH_PGA_MAX            0x03fU
#define NAU8822_R2DH_PGA_0dB            0x010U
#define NAU8822_R2DH_PGAMT              (1UL << 6UL)
//#define NAU8822_R2DH_PGAZC              (1UL << 7UL)
#define NAU8822_R2EH_PGA_UPDATE         (1UL << 8UL)

extern AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_NAU8822Obj;

#endif /* AMBA_AUDIO_NAU8822_H */
