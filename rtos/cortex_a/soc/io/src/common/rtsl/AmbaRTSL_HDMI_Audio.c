// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
 * @file AmbaRTSL_HDMI_Audio.c
 *
 * @copyright Copyright (c) 2021 Ambarella International LP
 *
 * @details HDMI Audio RTSL Device Driver
 *
 */

#include "AmbaDef.h"

#include "AmbaI2S.h"
#include "AmbaRTSL_HDMI.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaCSL_HDMI.h"
#include "AmbaCSL_I2S.h"

static UINT32 HdmiAudioSrc = AMBA_I2S_CHANNEL0; /* Default to use I2S channel 0 as HDMI audio src */

/**
 *  HDMI_SetAcrPacketTransmitInterval - Set ACR (audio clock regeneration) packet transmitting interval.
 *                                      It is controlled by the ratio of audio clock and sample frequency.
 *  @param[in] I2sChanNo I2S channel number
 *  @param[in] SampleRate Audio sample rate
 *  @param[in] AudioClkFreq PLL Audio clock
 *  @return error code
 */
static UINT32 HDMI_SetAcrPacketTransmitInterval(UINT32 I2sChanNo, UINT32 AudioClkFreq, UINT32 SampleRate)
{
    static const UINT32 AudioSampleFreq[NUM_HDMI_AUDIO_FS] = {
        [HDMI_AUDIO_FS_32K]     = 32000U,
        [HDMI_AUDIO_FS_44P1K]   = 44100U,
        [HDMI_AUDIO_FS_48K]     = 48000U,
        [HDMI_AUDIO_FS_88P2K]   = 88200U,
        [HDMI_AUDIO_FS_96K]     = 96000U,
        [HDMI_AUDIO_FS_176P4K]  = 176400U,
        [HDMI_AUDIO_FS_192K]    = 192000U
    };
    static const UINT16 AudioClk2SampleFreqRatio[NUM_HDMI_ACLK_ADJ] = {
        [HDMI_ACLK_ADJ_128FS]   = 128U,
        [HDMI_ACLK_ADJ_256FS]   = 256U,
        [HDMI_ACLK_ADJ_384FS]   = 384U,
        [HDMI_ACLK_ADJ_512FS]   = 512U,
        [HDMI_ACLK_ADJ_768FS]   = 768U,
        [HDMI_ACLK_ADJ_192FS]   = 192U,
        [HDMI_ACLK_ADJ_64FS]    = 64U,
        [HDMI_ACLK_ADJ_32FS]    = 32U,
    };

    UINT32 Ratio;
    UINT32 RetVal = HDMI_ERR_NONE;
    UINT8  i;

    if ((I2sChanNo >= AMBA_NUM_I2S_CHANNEL) || (SampleRate >= NUM_HDMI_AUDIO_FS)) {
        RetVal = HDMI_ERR_ARG;
    } else {
#if !(defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32))
        if (I2sChanNo == AMBA_I2S_CHANNEL0) {
            AmbaCSL_HdmiSetAudioSrc(0);
        } else {    /* AMBA_I2S_CHANNEL1 */
            AmbaCSL_HdmiSetAudioSrc(1);
        }
#endif
        Ratio = AudioClkFreq / AudioSampleFreq[SampleRate];
        for (i = 0U; i < (sizeof(AudioClk2SampleFreqRatio) / sizeof(AudioClk2SampleFreqRatio[0])); i ++) {
            if (Ratio == AudioClk2SampleFreqRatio[i]) {
                AmbaCSL_HdmiSetAudioClkToSampleFreqRatio(i);
                break;
            }
        }
    }
    return RetVal;
}

/**
 *  HDMI_ConfigI2sReceiver - Configure data format on I2S-like bus
 *  @param[in] I2sChanNo I2S channel number
 */
static UINT32 HDMI_ConfigI2sReceiver(UINT32 I2sChanNo)
{
    UINT32 RetVal = HDMI_ERR_NONE;
#if defined(CONFIG_LINUX) && (!defined(CONFIG_SOC_CV5) && !defined(CONFIG_SOC_CV52))
    /* Non cv5x chip with linux does not support */
    (void)I2sChanNo;
#else
    const AMBA_I2S_REG_s *pI2sReg = pAmbaI2S_Reg[I2sChanNo];
    UINT32 SaiSignalFmt = 0;
    UINT32 I2sMode;

    I2sMode = AmbaCSL_I2sGetMode(pI2sReg);
    AmbaCSL_HdmiSetI2sMode((UINT8)AmbaCSL_I2sGetMode(pI2sReg));
    AmbaCSL_HdmiSetI2sWordPosition((UINT8)AmbaCSL_I2sGetWordPosition(pI2sReg));

    /* I2S receiver of HDMI only supports less than 24-bit word length. */
    if (AmbaCSL_I2sGetWordLength(pI2sReg) > 23U) {
        /* The workaround only works with left-justified. */
        if ((I2sMode == AMBA_I2S_MODE_LEFT_JUSTIFIED) || (I2sMode == AMBA_I2S_MODE_I2S)) {
            AmbaCSL_HdmiSetI2sWordLength(0x17U);
        } else {
            RetVal = HDMI_ERR_NA;
        }
    } else {
        AmbaCSL_HdmiSetI2sWordLength((UINT8)AmbaCSL_I2sGetWordLength(pI2sReg));
    }

    SaiSignalFmt |= AmbaCSL_I2sGetTxWsInv(pI2sReg);
    SaiSignalFmt |= (UINT32)AmbaCSL_I2sGetTxWsMst(pI2sReg) << 1;
    SaiSignalFmt |= (UINT32)AmbaCSL_I2sGetTxOrder(pI2sReg) << 2;
    AmbaCSL_HdmiSetI2sSignalFormat(SaiSignalFmt);

    AmbaCSL_HdmiSetI2sClockEdge((UINT8)AmbaCSL_I2sGetClockPolarity(pI2sReg));
#endif
    return RetVal;
}

/**
 *  HDMI_EnableAudioSampleSource - Enable audio stream packets
 *  @param[in] SpeakerCount Number of audio channels/speakers
 */
static UINT32 HDMI_EnableAudioSampleSource(UINT32 SpeakerCount)
{
    UINT32 RetVal;

    RetVal = HDMI_ConfigI2sReceiver(HdmiAudioSrc);   /* According to I2S source */

    AmbaCSL_HdmiI2sFifoReset();
    AmbaCSL_HdmiI2sFifoResetRelease();

    /* It waits for VOUT VSYNC to leave from idle state to active state.
     * When the bit is set to 1, I2S won't receive audio data until
     * state machine is active even I2S related registers are well programmed.
     */
    AmbaCSL_HdmiSetSaiRxCtrl(1U);

    if (SpeakerCount == 0U) {
        AmbaCSL_HdmiSetAudioChan0Ctrl(0U);
        AmbaCSL_HdmiSetAudioChan1Ctrl(0U);
        AmbaCSL_HdmiSetI2sCtrl(0U);
    } else if (SpeakerCount <= 2U) {
        AmbaCSL_HdmiSetAudioChan0Ctrl(1U);
        AmbaCSL_HdmiSetAudioChan1Ctrl(0U);
        AmbaCSL_HdmiSetI2sCtrl(1U);
    } else {
        AmbaCSL_HdmiSetAudioChan0Ctrl(1U);
        AmbaCSL_HdmiSetAudioChan1Ctrl(1U);
        AmbaCSL_HdmiSetI2sCtrl(1U);
    }

    return RetVal;
}

/**
 *  HDMI_GetAcrParam - Get HDMI audio clock regeneration parameters
 *  @param[in] TmdsClock TMDS clock for audio sample clock regeneration
 *  @param[out] pHdmiAudioClkRegen pointer to an ACR parameter table
 */
static void HDMI_GetAcrParam(UINT64 TmdsClock, const AMBA_RTSL_HDMI_ACR_PARAM_s **pHdmiAudioClkRegen)
{
    static const AMBA_RTSL_HDMI_ACR_PARAM_s AudioClkReGen[NUM_HDMI_TMDS_CLOCK][NUM_HDMI_AUDIO_FS] = {
        /*       32KHz            44.1KHz              48KHz            88.2KHz            96KHz            176.4KHz           192KHz */
        /*   N      CTS         N      CTS         N      CTS         N      CTS        N        CTS        N       CTS        N       CTS */
        {{ 4576U,  28125U}, { 7007U,  31250U}, { 6864U,  28125U}, {14014U,  31250U}, {13728U,  28125U}, {28028U,  31250U}, {27456U,  28125U}},    /* PCLK 25.175 MHz */
        {{ 4096U,  25200U}, { 6272U,  28000U}, { 6144U,  25200U}, {12544U,  28000U}, {12288U,  25200U}, {25088U,  28000U}, {24576U,  25200U}},    /* PCLK 25.200 MHz */
        {{ 4096U,  27000U}, { 6272U,  30000U}, { 6144U,  27000U}, {12544U,  30000U}, {12288U,  27000U}, {25088U,  30000U}, {24576U,  27000U}},    /* PCLK 27.000 MHz */
        {{ 4096U,  27027U}, { 6272U,  30030U}, { 6144U,  27027U}, {12544U,  30030U}, {12288U,  27027U}, {25088U,  30030U}, {24576U,  27027U}},    /* PCLK 27.027 MHz */
        {{ 4096U,  54000U}, { 6272U,  60000U}, { 6144U,  54000U}, {12544U,  60000U}, {12288U,  54000U}, {25088U,  60000U}, {24576U,  54000U}},    /* PCLK 54.000 MHz */
        {{ 4096U,  54054U}, { 6272U,  60060U}, { 6144U,  54054U}, {12544U,  60060U}, {12288U,  54054U}, {25088U,  60060U}, {24576U,  54054U}},    /* PCLK 54.054 MHz */
        {{11648U, 168750U}, {17836U, 187500U}, {11648U, 112500U}, {35672U, 187500U}, {23296U, 112500U}, {71344U, 187500U}, {46592U, 112500U}},    /* PCLK 59.341 MHz */
        {{ 4096U,  59400U}, { 6272U,  66000U}, { 6144U,  59400U}, {12544U,  66000U}, {12288U,  59400U}, {25088U,  66000U}, {24576U,  59400U}},    /* PCLK 59.400 MHz */
        {{ 4096U,  72000U}, { 6272U,  80000U}, { 6144U,  72000U}, {12544U,  80000U}, {12288U,  72000U}, {25088U,  80000U}, {24576U,  72000U}},    /* PCLK 72.000 MHz */
        {{11648U, 210937U}, {17836U, 234375U}, {11648U, 140625U}, {35672U, 234375U}, {23296U, 140625U}, {71344U, 234375U}, {46592U, 140625U}},    /* PCLK 74.176 MHz */
        {{ 4096U,  74250U}, { 6272U,  82500U}, { 6144U,  74250U}, {12544U,  82500U}, {12288U,  74250U}, {25088U,  82500U}, {24576U,  74250U}},    /* PCLK 74.250 MHz */
        {{ 4096U, 108000U}, { 6272U, 120000U}, { 6144U, 108000U}, {12544U, 120000U}, {12288U, 108000U}, {25088U, 120000U}, {24576U, 108000U}},    /* PCLK 108.000 MHz */
        {{ 4096U, 108108U}, { 6272U, 120120U}, { 6144U, 108108U}, {12544U, 120120U}, {12288U, 108108U}, {25088U, 120120U}, {24576U, 108108U}},    /* PCLK 108.108 MHz */
        {{11648U, 421875U}, { 8918U, 234375U}, { 5824U, 140625U}, {17836U, 234375U}, {11648U, 140625U}, {35672U, 234375U}, {23296U, 140625U}},    /* PCLK 148.352 MHz */
        {{ 4096U, 148500U}, { 6272U, 165000U}, { 6144U, 148500U}, {12544U, 165000U}, {12288U, 148500U}, {25088U, 165000U}, {24576U, 148500U}},    /* PCLK 148.500 MHz */
        {{ 5824U, 421875U}, { 4459U, 234375U}, { 5824U, 281250U}, { 8918U, 234375U}, {11648U, 281250U}, {17836U, 234375U}, {23296U, 281250U}},    /* PCLK 296.703 MHz */
        {{ 3072U, 222750U}, { 4704U, 247500U}, { 5120U, 247500U}, { 9408U, 247500U}, {10240U, 247500U}, {18816U, 247500U}, {20480U, 247500U}},    /* PCLK 297.000 MHz */
        {{ 5824U, 843750U}, { 8918U, 937500U}, { 5824U, 562500U}, {17836U, 937500U}, {11648U, 562500U}, {35672U, 937500U}, {23296U, 562500U}},    /* PCLK 593.406 MHz */
        {{ 3072U, 445550U}, { 9408U, 990000U}, { 6144U, 594000U}, {18816U, 990000U}, {12288U, 594000U}, {37632U, 990000U}, {24576U, 594000U}},    /* PCLK 594.000 MHz */
        {{ 4096U,      0U}, { 6272U,      0U}, { 6144U,      0U}, {12544U,      0U}, {12288U,      0U}, {25088U,      0U}, {24576U,      0U}},    /* Other clocks, CTS is calculated by HW */
    };

    (void) TmdsClock;
    *pHdmiAudioClkRegen = AudioClkReGen[HDMI_TMDS_CLK_OTHER];
}

/**
 *  AmbaRTSL_HdmiSetAudioSrc - Set audio sample source selection
 *  @param[in] I2sChanNo I2S channel number
 */
void AmbaRTSL_HdmiSetAudioSrc(UINT32 I2sChanNo)
{
    HdmiAudioSrc = I2sChanNo;
}

/**
 *  AmbaRTSL_HdmiConfigAudio - Set HDMI audio output format
 *  @param[in] TmdsClock TMDS clock for audio sample clock regeneration
 *  @param[in] pAudioConfig pointer to audio format configuration
 *  @return error code
 */
UINT32 AmbaRTSL_HdmiConfigAudio(UINT64 TmdsClock, UINT32 AudioClkFreq, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig)
{
    static const UINT8 AudioSampleFreqIndex[NUM_HDMI_AUDIO_FS] = {
        [HDMI_AUDIO_FS_32K]     = 3U,
        [HDMI_AUDIO_FS_44P1K]   = 0U,
        [HDMI_AUDIO_FS_48K]     = 2U,
        [HDMI_AUDIO_FS_88P2K]   = 8U,
        [HDMI_AUDIO_FS_96K]     = 10U,
        [HDMI_AUDIO_FS_176P4K]  = 12U,
        [HDMI_AUDIO_FS_192K]    = 14U
    };

    const AMBA_RTSL_HDMI_ACR_PARAM_s *pHdmiAudioClkRegen = NULL;
    UINT32 AudioChanStatus[6];
    UINT32 N = 0U, CTS = 0U, NumAudioChan = 0U;
    UINT32 RetVal;

    /* Set ACR packet transmitting interval */
    RetVal = HDMI_SetAcrPacketTransmitInterval(HdmiAudioSrc, AudioClkFreq, pAudioConfig->SampleRate);
    if (RetVal == HDMI_ERR_NONE) {
        HDMI_GetAcrParam(TmdsClock, &pHdmiAudioClkRegen);

        if (pHdmiAudioClkRegen != NULL) {
            /* Considering the sink capability and source clock may not very precise, we use hardware auto generated CTS value.
               Hardware generated CTS value is more precise because it is determined by hardware accodirng to actual output pixelclock  */
            AmbaCSL_HdmiSetAudioRegenCtrl(0U, 0U, 0U);
            N = pHdmiAudioClkRegen[pAudioConfig->SampleRate].N;
            CTS = HDMI_CTS_AUTO_GEN;
            AmbaCSL_HdmiSetAudioRegenCtrl(1U, N, CTS);

            AmbaRTSL_HdmiGetNumAudioChan(pAudioConfig->SpeakerMap, &NumAudioChan);

            if (NumAudioChan <= 2U) {
                AmbaCSL_HdmiSetAudioSampleLayout(HDMI_AUDIO_LAYOUT_2CH_PER_SAMPLE);
            } else {
                AmbaCSL_HdmiSetAudioSampleLayout(HDMI_AUDIO_LAYOUT_8CH_PER_SAMPLE);
            }

            RetVal = HDMI_EnableAudioSampleSource(NumAudioChan);

            /* Audio Sample Subpackets (IEC 60958 or IEC 61937), bits[27:24] */
            if (pAudioConfig->SampleRate < NUM_HDMI_AUDIO_FS) {
                AudioChanStatus[0] = ((UINT32)AudioSampleFreqIndex[pAudioConfig->SampleRate] << 24);
            } else {
                AudioChanStatus[0] = ((UINT32)1U << 24);
            }

            AudioChanStatus[1] = 0;
            AudioChanStatus[2] = 0;
            AudioChanStatus[3] = 0;
            AudioChanStatus[4] = 0;
            AudioChanStatus[5] = 0;

            AmbaCSL_HdmiSetAudioChanStat(AudioChanStatus);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_HdmiGetNumAudioChan - Set HDMI audio output format
 *  @param[in] SpeakerAlloc Speaker(audio channel) allocation
 *  @param[out] pNumSpeaker Number of speakers(audio channels)
 */
void AmbaRTSL_HdmiGetNumAudioChan(UINT32 SpeakerAlloc, UINT32 *pNumSpeaker)
{
    static const UINT8 HdmiNumAudioChannels[NUM_HDMI_AUDIO_CHANNEL_MAP] = {
        [HDMI_CA_2CH]                       = 2U,
        [HDMI_CA_3CH_LFE]                   = 3U,
        [HDMI_CA_3CH_FC]                    = 3U,
        [HDMI_CA_4CH_LFE_FC]                = 4U,
        [HDMI_CA_3CH_RC]                    = 3U,
        [HDMI_CA_4CH_LFE_RC]                = 4U,
        [HDMI_CA_4CH_FC_RC]                 = 4U,
        [HDMI_CA_5CH_LFE_FC_RC]             = 5U,
        [HDMI_CA_4CH_RL_RR]                 = 4U,
        [HDMI_CA_5CH_LFE_RL_RR]             = 5U,
        [HDMI_CA_5CH_FC_RL_RR]              = 5U,
        [HDMI_CA_6CH_LFE_FC_RL_RR]          = 6U,
        [HDMI_CA_5CH_RL_RR_RC]              = 5U,
        [HDMI_CA_6CH_LFE_RL_RR_RC]          = 6U,
        [HDMI_CA_6CH_FC_RL_RR_RC]           = 6U,
        [HDMI_CA_7CH_LFE_FC_RL_RR_RC]       = 7U,
        [HDMI_CA_6CH_RL_RR_RLC_RRC]         = 6U,
        [HDMI_CA_7CH_LFE_RL_RR_RLC_RRC]     = 7U,
        [HDMI_CA_7CH_FC_RL_RR_RLC_RRC]      = 7U,
        [HDMI_CA_8CH_LFE_FC_RL_RR_RLC_RRC]  = 8U,
        [HDMI_CA_4CH_FLC_FRC]               = 4U,
        [HDMI_CA_5CH_LFE_FLC_FRC]           = 5U,
        [HDMI_CA_5CH_FC_FLC_FRC]            = 5U,
        [HDMI_CA_6CH_LFE_FC_FLC_FRC]        = 6U,
        [HDMI_CA_5CH_RC_FLC_FRC]            = 5U,
        [HDMI_CA_6CH_LFE_RC_FLC_FRC]        = 6U,
        [HDMI_CA_6CH_FC_RC_FLC_FRC]         = 6U,
        [HDMI_CA_7CH_LFE_FC_RC_FLC_FRC]     = 7U,
        [HDMI_CA_6CH_RL_RR_FLC_FRC]         = 6U,
        [HDMI_CA_7CH_LFE_RL_RR_FLC_FRC]     = 7U,
        [HDMI_CA_7CH_FC_RL_RR_FLC_FRC]      = 7U,
        [HDMI_CA_8CH_LFE_FC_RL_RR_FLC_FRC]  = 8U,
        [HDMI_CA_6CH_FC_RL_RR_FCH]          = 6U,
        [HDMI_CA_7CH_LFE_FC_RL_RR_FCH]      = 7U,
        [HDMI_CA_6CH_FC_RL_RR_TC]           = 6U,
        [HDMI_CA_7CH_LFE_FC_RL_RR_TC]       = 7U,
        [HDMI_CA_6CH_RL_RR_FLH_FRH]         = 6U,
        [HDMI_CA_7CH_LFE_RL_RR_FLH_FRH]     = 7U,
        [HDMI_CA_6CH_RL_RR_FLW_FRW]         = 6U,
        [HDMI_CA_7CH_LFE_RL_RR_FLW_FRW]     = 7U,
        [HDMI_CA_7CH_FC_RL_RR_RC_TC]        = 7U,
        [HDMI_CA_8CH_LFE_FC_RL_RR_RC_TC]    = 8U,
        [HDMI_CA_7CH_FC_RL_RR_RC_FCH]       = 7U,
        [HDMI_CA_8CH_LFE_FC_RL_RR_RC_FCH]   = 8U,
        [HDMI_CA_7CH_FC_RL_RR_FCH_TC]       = 7U,
        [HDMI_CA_8CH_LFE_FC_RL_RR_FCH_TC]   = 8U,
        [HDMI_CA_7CH_FC_RL_RR_FLH_FRH]      = 7U,
        [HDMI_CA_8CH_LFE_FC_RL_RR_FLH_FRH]  = 8U,
        [HDMI_CA_7CH_FC_RL_RR_FLW_FRW]      = 7U,
        [HDMI_CA_8CH_LFE_FC_RL_RR_FLW_FRW]  = 8U
    };

    if (SpeakerAlloc >= NUM_HDMI_AUDIO_CHANNEL_MAP) {
        *pNumSpeaker = 0U;
    } else {
        *pNumSpeaker = HdmiNumAudioChannels[SpeakerAlloc];
    }
}

/**
 *  AmbaRTSL_HdmiEnableAudioChan - Indicate the audio data of specified audio channel are present
 *  @param[in] AudChanNo Audio channel number
 *  @return error code
 */
UINT32 AmbaRTSL_HdmiEnableAudioChan(UINT32 AudChanNo)
{
    UINT32 RetVal = HDMI_ERR_NONE;

    if (AudChanNo == 0U) {
        AmbaCSL_HdmiSetAudioChan0Ctrl(1U);
    } else if (AudChanNo == 1U) {
        AmbaCSL_HdmiSetAudioChan1Ctrl(1U);
    } else {
        RetVal = HDMI_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_HdmiDisableAudioChan - Indicate the audio data of specified audio channel are not present
 *  @param[in] AudChanNo Audio channel number
 *  @return error code
 */
UINT32 AmbaRTSL_HdmiDisableAudioChan(UINT32 AudChanNo)
{
    UINT32 RetVal = HDMI_ERR_NONE;

    if (AudChanNo == 0U) {
        AmbaCSL_HdmiSetAudioChan0Ctrl(0U);
    } else if (AudChanNo == 1U) {
        AmbaCSL_HdmiSetAudioChan1Ctrl(0U);
    } else {
        RetVal = HDMI_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_HdmiSetAudioDataSource - Set HDMI audio data source mode.
 *  @param[in] SourceMode Optional audio data source mode selection
 *                        0x0 = From external source.
 *                        0x1 = Internally generated tone.
 */
void AmbaRTSL_HdmiSetAudioDataSource(UINT8 SourceMode)
{
    AmbaCSL_HdmiSetAudioDataSource(SourceMode);
}
