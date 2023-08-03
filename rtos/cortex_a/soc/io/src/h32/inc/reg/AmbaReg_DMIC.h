/**
 *  @file AmbaReg_DMIC.h
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
 *  @details Definitions & Constants for Digital Microphone(DMIC) Controller Registers
 *
 */

#ifndef AMBA_REG_DMIC_H
#define AMBA_REG_DMIC_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * DMIC Enable Registers
 */
typedef struct {
    UINT32 DmicEn:          1; /* [0]: Enable DMIC */
    UINT32 Reserved:        1; /* [1] */
    UINT32 DmicEnSt:        1; /* [2]: The actual status of DMIC enable */
    UINT32 Reserved1:       1; /* [3] */
    UINT32 WndEn:           1; /* [4]: Enable wind noise detection */
    UINT32 WnfEn:           1; /* [5]: Enable wind noise filter */
    UINT32 Reserved2:       26; /* [31:6] */
} AMBA_DMIC_ENABLE_REG_u;

/*
 * DMIC Audio Codec DP Reset Registers
 */
typedef struct {
    UINT32 AdcDpReset:      1; /* [0]: Reset ADC data path */
    UINT32 Reserved:       31; /* [31:1] */
} AMBA_DMIC_AUDIO_CODEC_DP_RESET_REG_u;

/*
 * DMIC Decimation Factor Registers
 */
typedef struct {
    UINT32 DecFactor0:      6; /* [5:0]: Decimation Factor at CIC */
    UINT32 Reserved:        2; /* [7:6] */
    UINT32 DecFactor1:      3; /* [10:8]: Decimation Factor after HBF */
    UINT32 Reserved1:       1; /* [11] */
    UINT32 DecWnf:          1; /* [12]: Decimation factor in WNF */
    UINT32 Reserved2:       3; /* [15:13] */
    UINT32 DecFs:          16; /* [31:16]: Frequency ratio of gclk_au/fs */
} AMBA_DMIC_DECIMATION_FACTOR_REG_u;

/*
 * DMIC CIC Multiplier Registers
 */
typedef struct {
    UINT32 CicMult:        20; /* [19:0]: The multiplier for the output of CIC data */
    UINT32 Reserved:       12; /* [31:20] */
} AMBA_DMIC_CIC_MULTIPLIER_REG_u;

/*
 * DMIC Clock Divisor Registers
 */
typedef struct {
    UINT32 FdivPdm:         8; /* [7:0]: DMIC PDM clock frequency divisor */
    UINT32 Reserved:       24; /* [31:8] */
} AMBA_DMIC_CLOCK_DIVISOR_REG_u;

/*
 * DMIC Enable Registers
 */
typedef struct {
    UINT32 PhaseLeft:       8; /* [7:0]: DMIC PDM data latching phase (in units of gclk_au cycle) for left channel */
    UINT32 Reserved:        8; /* [15:8] */
    UINT32 PhaseRight:      8; /* [23:16]: DMIC PDM data latching phase (in units of gclk_au cycle) for right channel */
    UINT32 Reserved1:       8; /* [31:24] */
} AMBA_DMIC_DATA_PHASE_REG_u;

/*
 * DMIC Clock Enable Registers
 */
typedef struct {
    UINT32 ClkPdmEn:        1; /* [0]: DMIC PDM clock enable */
    UINT32 ClkAuEn:         1; /* [1]: DMIC Filter clock enable */
    UINT32 Reserved:       30; /* [31:2] */
} AMBA_DMIC_CLOCK_ENABLE_REG_u;

/*
 * DMIC Wind Noise Filter Gamma Control Registers
 */
typedef struct {
    UINT32 WnfSoftMode:     1; /* [0]: Software mode enabled */
    UINT32 Reserved:        3; /* [3:1] */
    UINT32 WnfLpfGamma:     3; /* [6:4] Gamma value for low-pass filter (LPF) in SW mode */
    UINT32 Reserved1:       1; /* [7] */
    UINT32 WndHpfGamma:     3; /* [10:8]: Gamma value for high-pass filter (HPF) in SW mode */
    UINT32 Reserved2:       1; /* [11] */
    UINT32 WndGamma:        3; /* [14:12]: Gamma value from wind noise detection */
    UINT32 Reserved3:      17; /* [31:15] */
} AMBA_DMIC_WIND_NOISE_FILTER_GAMMA_CTRL_REG_u;

/*
 * DMIC Wind Noise Filter Control Registers
 */
typedef struct {
    UINT32 CepstrumThd:     3; /* [2:0]: Cepstrum Threshold / 2 */
    UINT32 Reserved:        1; /* [3] */
    UINT32 WnfChSel:        1; /* [4]: Audio Channel used in wind noise filter pitch_detection/LPC/Vocoder */
    UINT32 Reserved1:      11; /* [15:5] */
    UINT32 WnfBufThld:      9; /* [24:16]: Wind noise filter (WNF) initial latency */
    UINT32 Reserved2:       7; /* [31:25] */
} AMBA_DMIC_WIND_NOISE_FILTER_CTRL_REG_u;

/*
 * DMIC I2S Control Registers
 */
typedef struct {
    UINT32 I2sMsbAlign:     1; /* [0]: Output data alignment at I2S interface */
    UINT32 Reserved:       31; /* [31:1] */
} AMBA_DMIC_I2S_CTRL_REG_u;

/*
 * DMIC Droop Comp Filter Coefficient Registers
 */
typedef struct {
    UINT32 Reserved:       12; /* [11:0] */
    UINT32 DrpCoef:        20; /* [31:12]: droop comp filter coefficient */
} AMBA_DMIC_DROOP_COMP_FILTER_COEFF_REG_u;

/*
 * DMIC Half Band Filter Coefficient Registers
 */
typedef struct {
    UINT32 Reserved:       12; /* [11:0] */
    UINT32 HbfCoef:        20; /* [31:12]: half band filter coefficient */
} AMBA_DMIC_HALF_BAND_FILTER_COEFF_REG_u;

/*
 * DMIC Wind Noise Filter Hpf Coefficient Registers
 */
typedef struct {
    UINT32 Reserved:       12; /* [11:0] */
    UINT32 WnfDynHbfCoef:  20; /* [31:12]: Wind Noise Filter Hpf Coefficient */
} AMBA_DMIC_WIND_NOISE_FILTER_HPF_COEFF_REG_u;

/*
 * DMIC Wind Noise Filter Lpf Coefficient Registers
 */
typedef struct {
    UINT32 Reserved:       12; /* [11:0] */
    UINT32 WnfDynLbfCoef:  20; /* [31:12]: Wind Noise Filter Lpf Coefficient */
} AMBA_DMIC_WIND_NOISE_FILTER_LPF_COEFF_REG_u;

/*
 * DMIC Wind Noise Filter Wnd Coefficient Registers
 */
typedef struct {
    UINT32 Reserved:       12; /* [11:0] */
    UINT32 WnfWndLpfCoef:  20; /* [31:12]: Wind Noise Filter Wnd Coefficient */
} AMBA_DMIC_WIND_NOISE_FILTER_WND_COEFF_REG_u;

/*
 * DMIC Wind Noise Filter Voc HPF coefficient Registers
 */
typedef struct {
    UINT32 Reserved:       12; /* [11:0] */
    UINT32 WnfVocHpfCoef:  20; /* [31:12]: Wind Noise Filter Voc HPF coefficient registers */
} AMBA_DMIC_WNF_VOC_HPF_COEFF_REG_u;

/*
 * DMIC Wind Noise Filter Voc Lpf Coefficient Registers
 */
typedef struct {
    UINT32 Reserved:       18; /* [17:0] */
    UINT32 WnfVocLpfCoef:  14; /* [31:18]: Wind Noise Filter Voc Lpf Coefficient Coefficient */
} AMBA_DMIC_WNF_VOC_LPF_COEFF_REG_u;

/*
 * DMIC All Registers
 */
typedef struct {
    volatile AMBA_DMIC_ENABLE_REG_u                   DmicEnable;                             /* [0x0000] DMIC Enable */
    volatile AMBA_DMIC_AUDIO_CODEC_DP_RESET_REG_u     DmicAdcDpReset;                         /* [0x0004] Audio codec dp reset */
    volatile AMBA_DMIC_DECIMATION_FACTOR_REG_u        DmicDecFactor;                          /* [0x0008] Decimation factor */
    volatile UINT32                                   Reserved0;
    volatile UINT32                                   DmicStatus;                             /* [0x0010] DMIC status */
    volatile AMBA_DMIC_CIC_MULTIPLIER_REG_u           DmicCicMult;                            /* [0x0014] The multiplier for the output of CIC data */
    volatile UINT32                                   Reserved1[58];
    volatile AMBA_DMIC_CLOCK_DIVISOR_REG_u            DmicClockDivisor;                       /* [0x0100] DMIC clock divisor */
    volatile AMBA_DMIC_DATA_PHASE_REG_u               DmicDataPhase;                          /* [0x0104] DMIC data phase */
    volatile AMBA_DMIC_CLOCK_ENABLE_REG_u             DmicClockEnable;                        /* [0x0108] DMIC clock enable */
    volatile UINT32                                   DmicWindNoiseFilterGammaControl;        /* [0x010C] wind noise filter gamma control */
    volatile AMBA_DMIC_WIND_NOISE_FILTER_CTRL_REG_u   DmicWindNoiseFilterControl;             /* [0x0110] wind noise filter control */
    volatile AMBA_DMIC_I2S_CTRL_REG_u                 DmicI2sControl;                         /* [0x0114] DMIC I2S control */
    volatile UINT32                                   DmicDebugStatus0;                       /* [0x0118] debug status 0 */
    volatile UINT32                                   DmicDebugStatus1;                       /* [0x011C] debug status 1 */
    volatile UINT32                                   DmicDebugStatus2;                       /* [0x0120] debug status 2 */
    volatile UINT32                                   Reserved2[55];
    volatile UINT32                                   DroopCompFilterCoefficient[64];         /* [0x0200:0x02FC] droop comp filter coefficient #0~#63 */
    volatile UINT32                                   HalfBandFilterrCoefficient[64];         /* [0x0300:0x03FC] half band filter coefficient #0~#63 */
    volatile UINT32                                   WindNoiseFilterHpfCoeffcient[68];       /* [0x0400:0x050C] wind noise filter hpf coefficient #0~#67 */
    volatile UINT32                                   WindNoiseFilterLpfCoeffcient[68];       /* [0x0510:0x061C] wind noise filter lpf coefficient #0~#67 */
    volatile UINT32                                   WindNoiseFilterWndCoeffcient[17];       /* [0x0620:0x0660] wind noise filter wnd coefficient #0~#16 */
    volatile UINT32                                   WindNoiseFilterVocHpfCoeffcient[17];    /* [0x0664:0x06A4] wind noise filter voc hpf coefficient #0~#16 */
    volatile UINT32                                   WindNoiseFilterVocLpfCoeffcient[11];    /* [0x06A8:0x06D0] wind noise filter voc lpf coefficient #0~#10 */
    volatile UINT32                                   CustomIIRCoeffcient[17];                /* [0x06D4:0x0714] custom iir filter coefficient #0~#16 */
} AMBA_DMIC_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_DMIC_REG_s *const pAmbaDMIC_Reg;

#endif /* AMBA_REG_DMIC_H */
