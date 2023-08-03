/**
 *  @file AmbaAudio_TLV320AIC3007.h
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
 *  @details TI tlv320aic3007 audio AD/DA CODEC driver
 *
 */

#ifndef AMBA_AUDIO_TLV320AIC3007_H
#define AMBA_AUDIO_TLV320AIC3007_H

// Example in bsp
/*
#ifdef CONFIG_AUDIO_TITLV320AIC3007
//I2C
#include "AmbaAudio_TLV320AIC3007.h"

    extern AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_Tlv320Aic3007Obj;
    AMBA_AUDIO_CODEC_CTRL_s CodecCtrl;

    CodecCtrl.CtrlMode = AMBA_AUDIO_CODEC_I2C_CONTROL;
    CodecCtrl.I2cCtrl.I2cChanNo = AMBA_I2C_CHANNEL0;
    CodecCtrl.I2cCtrl.SlaveAddr = (TLV320AIC3007_I2C_ADDR << 1);
    CodecCtrl.I2cCtrl.I2cSpeed = AMBA_I2C_SPEED_STANDARD;
    AmbaAudio_CodecHook(AMBA_AUDIO_CODEC_0, &AmbaAudio_Tlv320Aic3007Obj, &CodecCtrl);

    AmbaGPIO_ConfigOutput(GPIO_PIN_54, AMBA_GPIO_LEVEL_LOW);
    AmbaKAL_TaskSleep(1);
    AmbaGPIO_ConfigOutput(GPIO_PIN_54, AMBA_GPIO_LEVEL_HIGH);
    AmbaKAL_TaskSleep(1);
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

#define TLV320AIC3007_I2C_ADDR  0x18U
#define TLV320AIC3007_REG_NUM  127U

extern AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_Tlv320Aic3007Obj;

#endif /* End of AMBA_AUDIO_TLV320AIC3007_H */
