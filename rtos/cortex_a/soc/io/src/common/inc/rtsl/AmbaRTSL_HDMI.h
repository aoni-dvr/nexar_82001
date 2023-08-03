/**
 * @file AmbaRTSL_HDMI.h
 *
 * Copyright (c) 2020 Ambarella International LP
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
 */

#ifndef AMBA_RTSL_HDMI_H
#define AMBA_RTSL_HDMI_H

#ifndef AMBA_HDMI_DEF_H
#include "AmbaHDMI_Def.h"
#endif

typedef struct {
    UINT32  FirstTap:       8;      /* 1st-tap Pre-Emphasis, affect the rising edge       -----  <-2   */
    UINT32  SecondTap:      8;      /* 2nd-tap Pre-Emphasis, affect the level part    1->/     \ <-3   */
    UINT32  ThirdTap:       8;      /* 3rd-tap Pre-Emphasis, affect the falling edge ___/       \___   */
    UINT32  Reserved:       8;      /* Reserved */
} AMBA_HDMI_SINK_PREEMPHASIS_s;

#define AMBA_VIDEO_FIELD_RATE_UNKNOWN   0U
#define AMBA_VIDEO_FIELD_RATE_24HZ      1U  /* 24 or 24/1.001 Hz */
#define AMBA_VIDEO_FIELD_RATE_25HZ      2U  /* 25 Hz */
#define AMBA_VIDEO_FIELD_RATE_30HZ      3U  /* 30 or 30/1.001 Hz */
#define AMBA_VIDEO_FIELD_RATE_50HZ      4U  /* 50 Hz */
#define AMBA_VIDEO_FIELD_RATE_60HZ      5U  /* 60 or 60/1.001 Hz */
#define AMBA_VIDEO_FIELD_RATE_100HZ     6U  /* 100 Hz */
#define AMBA_VIDEO_FIELD_RATE_120HZ     7U  /* 120 or 120/1.001 Hz */
#define AMBA_VIDEO_FIELD_RATE_200HZ     8U  /* 200 Hz */
#define AMBA_VIDEO_FIELD_RATE_240HZ     9U  /* 240 or 240/1.001 Hz */

#define AMBA_VIDEO_SYNC_EMBEDDED        0U  /* BT.656  embedded sync mode */
#define AMBA_VIDEO_SYNC_H_POS_V_POS     1U  /* BT.601  Positive H-sync pulse  Positive V-sync pulse */
#define AMBA_VIDEO_SYNC_H_POS_V_NEG     2U  /* BT.601  Positive H-sync pulse  Negative V-sync pulse */
#define AMBA_VIDEO_SYNC_H_NEG_V_POS     3U  /* BT.601  Negative H-sync pulse  Positive V-sync pulse */
#define AMBA_VIDEO_SYNC_H_NEG_V_NEG     4U  /* BT.601  Negative H-sync pulse  Negative V-sync pulse */

#define AMBA_VIDEO_SCAN_PROGRESSIVE     0U  /* Progressive scan format */
#define AMBA_VIDEO_SCAN_INTERLACED      1U  /* Interlaced scan format */

/*
 * Definitions for IEC 60958 Channel Status
 */
typedef struct {
    UINT32  Use:                        1;      /* [0] 0 = Consumer use (S/PDIF), 1 = Professional use (AES/EBU) */
    UINT32  LinearPCM:                  1;      /* [1] 0 = Audio sample word represents linear PCM samples */
    UINT32  Copyright:                  1;      /* [2] 0 = Copy restrict, 1 = Copy permit */
    UINT32  Preemphasis:                3;      /* [5:3] 0 = No pre-emphasis */
    UINT32  Mode:                       2;      /* [7:6] Subsequent bytes, always zero */

    UINT32  Category:                   7;      /* [14:8] Audio source category (general, CD-DA, DVD, etc.) */
    UINT32  L:                          1;      /* [15] L-bit, original or copy (see text) */

    UINT32  SourceNo:                   4;      /* [19:16] Source number, 0 = Unspecified */
    UINT32  ChannelNo:                  4;      /* [23:20] Channel number, 0 = Unspecified */
    UINT32  SampleFreq:                 4;      /* [27:24] 1 = Not indicated */
    UINT32  ClockAccuracy:              2;      /* [29:28] 0 = Level II, 1 = Level I, 2 = Level III */
    UINT32  Reserved:                   2;      /* [31:30] */

    UINT32  MaxWordLength:              1;      /* [32] 0 = 20 bits, 1 = 24 bits word length at maximum */
    UINT32  WordLength:                 3;      /* [35:33] Sample word length */
    UINT32  OriSampleFreq:              4;      /* [39:36] Original sampling frequency */

    UINT32  CGMSA:                      2;      /* [41:40] CGMS-A information */
    UINT32  Reserved1:                  22;     /* [63:42] */
    UINT32  Reserved2[4];                       /* [191:64] */
} AMBA_HDMI_AUDIO_CHANNEL_STATUS_s;

/*
 * RTSL HDMI Sink Current Structure
 */
typedef struct {
    UINT32      BiasCurrentAdj;     /* Bias current adjust */
    UINT32      SinkCurrentInc;     /* Sink current increment */
} AMBA_HDMI_SINK_CURRENT_COEF_s;

/*
 * RTSL HDMI Audio Clock Regeneration Structure
 */
typedef struct {
    UINT32  N;              /* N parameter */
    UINT32  CTS;            /* CTS paramter */
} AMBA_RTSL_HDMI_ACR_PARAM_s;

/*
 * Defined in AmbaRTSL_HDMI.c
 */
void AmbaRTSL_HdmiInit(void);
#if defined(CONFIG_LINUX)
UINT32 AmbaRTSL_HdmiHookHpdHandler(AMBA_HDMI_HPD_ISR_f IntHandler);
UINT32 AmbaRTSL_HdmiHookCecHandler(AMBA_HDMI_CEC_ISR_f IntHandler);
#endif
void AmbaRTSL_HdmiSetPowerCtrl(UINT32 EnableFlag);
void AmbaRTSL_HdmiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal);
void AmbaRTSL_HdmiCheckConnection(UINT32 *pHotPlug, UINT32 *pRxSense);
void AmbaRTSL_HdmiStart(UINT32 IsHDMI);
void AmbaRTSL_HdmiStop(void);
void AmbaRTSL_HdmiPacketUpdate(void);
void AmbaRTSL_HdmiDataIslandPackCtrl(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, UINT32 Mode);
UINT32 AmbaRTSL_HdmiReset(UINT8 ResetFlag);
void AmbaRTSL_HdmiConfigScrambler(UINT8 EnableFlag);
void AmbaRTSL_HdmiSendAVMUTE(UINT32 EnableFlag);

void AmbaRTSL_HdmiCecSetClkFreq(void);
void AmbaRTSL_HdmiCecEnable(void);
void AmbaRTSL_HdmiCecDisable(void);
void AmbaRTSL_HdmiCecSetLogicalAddr(UINT32 LogicalAddress);
UINT32 AmbaRTSL_HdmiCecTransmit(const UINT8 *pMessage, UINT32 MsgSize);
void AmbaRTSL_HdmiCecReceive(UINT8 *pMessage, UINT32 *pMsgSize);

/*
 * Defined in AmbaRTSL_HDMI_Audio.c
 */
void AmbaRTSL_HdmiGetNumAudioChan(UINT32 SpeakerAlloc, UINT32 *pNumSpeaker);
UINT32 AmbaRTSL_HdmiEnableAudioChan(UINT32 AudChanNo);
UINT32 AmbaRTSL_HdmiDisableAudioChan(UINT32 AudChanNo);
UINT32 AmbaRTSL_HdmiConfigAudio(UINT64 TmdsClock, UINT32 AudioClkFreq, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig);
void AmbaRTSL_HdmiSetAudioSrc(UINT32 I2sChanNo);
void AmbaRTSL_HdmiSetAudioDataSource(UINT8 SourceMode);

/*
 * Defined in AmbaRTSL_HDMI_Video.c
 */
UINT32 AmbaRTSL_HdmiDtdCopy(AMBA_HDMI_VIDEO_TIMING_s *pDstDtd, const AMBA_HDMI_VIDEO_TIMING_s *pSrcDtd);
UINT32 AmbaRTSL_HdmiVicTag(AMBA_HDMI_VIDEO_TIMING_s *pDtd);
UINT32 AmbaRTSL_HdmiVicGet(UINT32 VideoID, AMBA_HDMI_VIDEO_TIMING_s * pDTD);
UINT32 AmbaRTSL_HdmiConfigVideo(const AMBA_HDMI_VIDEO_TIMING_s *pVideoTiming);
void AmbaRTSL_HdmiSetVideoDataSource(UINT8 SourceMode);

#endif /* AMBA_RTSL_HDMI_H */
