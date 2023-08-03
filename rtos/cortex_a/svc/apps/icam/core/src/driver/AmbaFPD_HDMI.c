/**
 *  @file AmbaFPD_HDMI.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you 
 * distribute this sample code (whether in source, object, or binary code form), it will be 
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Control APIs of DZXtech Display FPD panel MAXIM_TFT1280768
 *
 */

#include "AmbaTypes.h"
#include "AmbaHDMI.h"
#include "AmbaFPD.h"
#include "AmbaPrint.h"
#include "AmbaFPD_HDMI.h"

#include "AmbaMisraFix.h"

static UINT32 FPD_HDMIEnable(void)
{
    UINT32 RetVal = ERR_NONE;

    return RetVal;
}

static UINT32 FPD_HDMIDisable(void)
{
    UINT32 RetVal = ERR_NONE;
    RetVal |= AmbaHDMI_TxStop(AMBA_HDMI_TX_PORT0);
    RetVal |= AmbaHDMI_TxDisable(AMBA_HDMI_TX_PORT0);

    return RetVal;
}

static UINT32 FPD_HDMIGetInfo(AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;

    AmbaMisra_TouchUnused(pInfo);
    AmbaMisra_TouchUnused(&pInfo);

    return RetVal;
}

static UINT32 FPD_HDMIGetModeInfo(UINT8 Mode, AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;

    AmbaMisra_TouchUnused(&Mode);
    AmbaMisra_TouchUnused(pInfo);
    AmbaMisra_TouchUnused(&pInfo);

    return RetVal;
}

static UINT32 FPD_HDMIConfig(UINT8 Mode)
{
    UINT32 RetVal = ERR_NONE;
    AMBA_HDMI_VIDEO_CONFIG_s  HdmiCfg = {0};
    AMBA_HDMI_AUDIO_CONFIG_s  AudioCfg = {0};
    AMBA_HDMI_MODE_INFO_s     HdmiInfo = {0};

    if (Mode == (UINT8) HDMI_VIC_7680_4320_30P) {
        HdmiCfg.PixelFormat = AMBA_HDMI_PIXEL_FORMAT_YCC_420;
    } else {
        HdmiCfg.PixelFormat = AMBA_HDMI_PIXEL_FORMAT_RGB_444;
    }
    HdmiCfg.ColorDepth  = AMBA_HDMI_COLOR_DEPTH_24BPP;
    HdmiCfg.QuantRange  = AMBA_HDMI_QRANGE_FULL;
    HdmiCfg.VideoIdCode = Mode;

    AudioCfg.SampleRate  = HDMI_AUDIO_FS_48K;
    AudioCfg.SpeakerMap  = HDMI_CA_2CH;

    {
        typedef void (*Svc_FP_HDMI_AudioChannelSelect)(UINT32 HDMIAudioChannel);
        extern void AmbaHDMI_AudioChannelSelect(UINT32 HDMIAudioChannel) GNU_WEAK_SYMBOL;
        Svc_FP_HDMI_AudioChannelSelect pAmbaHDMI_AudioChannelSelect = AmbaHDMI_AudioChannelSelect;

        if (pAmbaHDMI_AudioChannelSelect == NULL) {
            AmbaPrint_PrintUInt5("[FPD_HDMI] Config not support Channel Select", 0U, 0U, 0U, 0U, 0U);
        } else {
#if defined(CONFIG_ICAM_AUDIO_USED) && defined(CONFIG_ICAM_I2S_CHANNEL)
            (pAmbaHDMI_AudioChannelSelect)(CONFIG_ICAM_I2S_CHANNEL);
#endif
        }
    }

    RetVal |= AmbaHDMI_TxEnable(AMBA_HDMI_TX_PORT0);
    RetVal |= AmbaHDMI_TxSetMode(AMBA_HDMI_TX_PORT0, &HdmiCfg, &AudioCfg);
    RetVal |= AmbaHDMI_TxGetCurrModeInfo(AMBA_HDMI_TX_PORT0, &HdmiInfo);
    RetVal |= AmbaHDMI_TxStart(AMBA_HDMI_TX_PORT0);

    return RetVal;
}

static UINT32 FPD_HDMISetBacklight(UINT32 EnableFlag)
{
    AmbaMisra_TouchUnused(&EnableFlag);
    AmbaPrint_PrintUInt5("[FPD_HDMI] SetBacklight not support", 0U, 0U, 0U, 0U, 0U);

    return ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 * FPD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_FPD_OBJECT_s AmbaFPD_HDMI_Obj = {
    .FpdEnable          = FPD_HDMIEnable,
    .FpdDisable         = FPD_HDMIDisable,
    .FpdGetInfo         = FPD_HDMIGetInfo,
    .FpdGetModeInfo     = FPD_HDMIGetModeInfo,
    .FpdConfig          = FPD_HDMIConfig,
    .FpdSetBacklight    = FPD_HDMISetBacklight,

    .pName = "HDMI"
};
