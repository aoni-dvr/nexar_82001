/**
 *  @file proto.h
 *
 *  @copyright Copyright (c) 2021 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details proto for HDMI device module
 *
 */

#ifndef __PROTO_H_INCLUDED
#define __PROTO_H_INCLUDED

#include <hw/ambarella_hdmi.h>
#include "AmbaRTSL_HDMI.h"
#include "AmbaCSL_HDMI.h"

#define AMBA_RCT_BASE_ADDR                      0x20ed080000
#define AMBA_HDMI_BASE_ADDR                     0x20e0016000
#define AMBA_SCRATCHPADNS_BASE_ADDR             0x20e0024000
#define AMBA_VOUT_TOP_BASE_ADDR                 0x20e0008a00
#define AMBA_VOUT_DISPLAY2_CONFIG_BASE_ADDR     0x20e0008c00
#define AMBA_I2S0_BASE_ADDR                     0x20e001c000
#define AMBA_I2S1_BASE_ADDR                     0x20e001d000

#define AMBA_RCT_MMAP_SIZE                      0x1000
#define AMBA_HDMI_MMAP_SIZE                     0x1000
#define AMBA_SCRATCHPADNS_MMAP_SIZE             0x1B0
#define AMBA_VOUT_TOP_MMAP_SIZE                 0x10
#define AMBA_VOUT_DISPLAY2_CONFIG_MMAP_SIZE     0x1000
#define AMBA_I2S_MMAP_SIZE                      0x40

/* Defined in lib.c */
int hdmi_dev_init(void);
int hdmi_dev_dinit(void);

int hdmi_TxEnable(UINT32 HdmiPort);
int hdmi_TxDisable(UINT32 HdmiPort);
int hdmi_HookHpdHandler(UINT32 HdmiPort, AMBA_HDMI_HPD_ISR_f IntHandler);
int hdmi_TxGetSinkInfo(UINT32 HdmiPort, UINT32 *pHotPlugDetect, UINT32 *pEdidDetect, AMBA_HDMI_SINK_INFO_s *pSinkInfo);
int hdmi_TxSetMode(UINT32 HdmiPort, const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig, UINT32 IsSuccessor, AMBA_HDMI_VIDEO_CONFIG_s *pWorkingVideoConfig);
int hdmi_TxGetCurrModeInfo(UINT32 HdmiPort, AMBA_HDMI_MODE_INFO_s *pModeInfo, UINT8 *pStrictFrameRate, UINT16 *pFrameWidth, UINT16 *pFrameHeight);
int hdmi_TxSetPhyCtrl(UINT32 HdmiPort, UINT32 ParamID, UINT32 ParamVal);
int hdmi_TxStart(UINT32 HdmiPort);
int hdmi_TxStop(UINT32 HdmiPort);
int hdmi_TxSetSourceInfo(UINT32 HdmiPort, const AMBA_HDMI_SOURCE_PRODUCT_INFO_s *pSrcProductInfo);
int hdmi_TxSetInfoFramePacket(UINT32 HdmiPort, const AMBA_HDMI_DATA_ISLAND_PACKET_s *pInfoFramePkt, UINT32 TxMode);
int hdmi_TxGetInfoFramePacket(UINT32 HdmiPort, UINT32 InfoFrameType, AMBA_HDMI_DATA_ISLAND_PACKET_s *pInfoFramePkt);
int hdmi_TxSetAvMute(UINT32 HdmiPort);
int hdmi_TxClearAvMute(UINT32 HdmiPort);

/* Defined in AmbaHDMI_Info.c */
UINT32 hdmi_TxCopySinkInfo(AMBA_HDMI_SINK_INFO_s *pSinkInfo);
int hdmi_AudioChannelSelect(UINT32 HDMIAudioChannel, UINT32 AudioClk);
UINT32 hdmi_TxConfigAudio(const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig);
UINT32 hdmi_TxSendAudioInfoFrame(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig, UINT32 TxMode);
UINT32 hdmi_TxSendSpdInfoFrame(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, const AMBA_HDMI_SOURCE_PRODUCT_INFO_s *pSPD, UINT32 TxMode);
UINT32 hdmi_TxSendAviInfoFrame(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, UINT32 TxMode);
UINT32 hdmi_TxSendVendorInfoFrame(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, UINT32 TxMode);
UINT32 hdmi_TxParseEDID(const UINT8 pEDID[256][128], UINT32 *pEdidDetect);

#endif

