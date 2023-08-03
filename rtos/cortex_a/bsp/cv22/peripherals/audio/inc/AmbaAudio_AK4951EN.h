/**
 *  @file AmbaAudio_AK4951EN.h
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
 *  @details AK4951EN audio AD/DA CODEC driver
 *
 */

#ifndef AMBA_AUDIO_AK4951_H
#define AMBA_AUDIO_AK4951_H

#ifndef AMBA_AUDIO_CODEC_H
#include "AmbaAudio_CODEC.h"
#endif

// Example in bsp
/*
#ifdef CONFIG_AUDIO_AK4951EN
#include "AmbaAudio_AK4951EN.h"

    extern AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_AK4951Obj;
    AMBA_AUDIO_CODEC_CTRL_s CodecCtrl;

    CodecCtrl.CtrlMode = AMBA_AUDIO_CODEC_I2C_CONTROL;
    CodecCtrl.I2cCtrl.I2cChanNo = AMBA_I2C_CHANNEL2;
    CodecCtrl.I2cCtrl.SlaveAddr = AK4951_I2C_ADDR;
    CodecCtrl.I2cCtrl.I2cSpeed = AMBA_I2C_SPEED_FAST;
    AmbaAudio_CodecHook(AMBA_AUDIO_CODEC_0, &AmbaAudio_AK4951Obj, &CodecCtrl);

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

#define AK4951_I2C_ADDR           0x24U        /* CAD0 = 0 */

typedef enum {
    AK4951_00_POWER_MANAGEMENT1 = 0x00U,
    AK4951_01_POWER_MANAGEMENT2,
    AK4951_02_SIGNAL_SELECT1,
    AK4951_03_SIGNAL_SELECT2,
    AK4951_04_SIGNAL_SELECT3,
    AK4951_05_MODE_CONTROL_1,
    AK4951_06_MODE_CONTROL_2,
    AK4951_07_MODE_CONTROL_3,
    AK4951_08_DIGITAL_MIC,
    AK4951_09_TIMER_SELECT,
    AK4951_0A_ALC_TIMER_SELECT,
    AK4951_0B_ALC_MODE_CONTROL_1,
    AK4951_0C_ALC_MODE_CONTROL_2,
    AK4951_0D_L_IN_VOLUME,
    AK4951_0E_R_IN_VOLUME,
    AK4951_0F_ALC_VOLUME,
    AK4951_10_L_MIC_GAIN,
    AK4951_11_R_MIC_GAIN,
    AK4951_12_BEEP_CONTROL,
    AK4951_13_L_DVOL,
    AK4951_14_R_DVOL,
    AK4951_15_EQ_GAIN,
    AK4951_16_EQ2_GAIN,
    AK4951_17_EQ3_GAIN,
    AK4951_18_EQ4_GAIN,
    AK4951_19_EQ5_GAIN,
    AK4951_1A_HPF_CONTROL,
    AK4951_1B_D_FILTER_SELECT1,
    AK4951_1C_D_FILTER_SELECT2,
    AK4951_1D_D_FILTER_MODE,
    AK4951_1E_HPF2_COEFFICIENT0,
    AK4951_1F_HPF2_COEFFICIENT1,
    AK4951_20_HPF2_COEFFICIENT2,
    AK4951_21_HPF2_COEFFICIENT3,
    AK4951_22_LPF_COEFFICIENT0,
    AK4951_23_LPF_COEFFICIENT1,
    AK4951_24_LPF_COEFFICIENT2,
    AK4951_25_LPF_COEFFICIENT3,
    AK4951_26_FIL3_COEFFICIENT0,
    AK4951_27_FIL3_COEFFICIENT1,
    AK4951_28_FIL3_COEFFICIENT2,
    AK4951_29_FIL3_COEFFICIENT3,
    AK4951_2A_EQ_COEFFICIENT0,
    AK4951_2B_EQ_COEFFICIENT1,
    AK4951_2C_EQ_COEFFICIENT2,
    AK4951_2D_EQ_COEFFICIENT3,
    AK4951_2E_EQ_COEFFICIENT4,
    AK4951_2F_EQ_COEFFICIENT5,
    AK4951_30_D_FILTER_SELECT3,
    AK4951_31_DEV_INFO,
    AK4951_32_E1_COEFFICIENT0,
    AK4951_33_E1_COEFFICIENT1,
    AK4951_34_E1_COEFFICIENT2,
    AK4951_35_E1_COEFFICIENT3,
    AK4951_36_E1_COEFFICIENT4,
    AK4951_37_E1_COEFFICIENT5,
    AK4951_38_E2_COEFFICIENT0,
    AK4951_39_E2_COEFFICIENT1,
    AK4951_3A_E2_COEFFICIENT2,
    AK4951_3B_E2_COEFFICIENT3,
    AK4951_3C_E2_COEFFICIENT4,
    AK4951_3D_E2_COEFFICIENT5,
    AK4951_3E_E3_COEFFICIENT0,
    AK4951_3F_E3_COEFFICIENT1,
    AK4951_40_E3_COEFFICIENT2,
    AK4951_41_E3_COEFFICIENT3,
    AK4951_42_E3_COEFFICIENT4,
    AK4951_43_E3_COEFFICIENT5,
    AK4951_44_E4_COEFFICIENT0,
    AK4951_45_E4_COEFFICIENT1,
    AK4951_46_E4_COEFFICIENT2,
    AK4951_47_E4_COEFFICIENT3,
    AK4951_48_E4_COEFFICIENT4,
    AK4951_49_E4_COEFFICIENT5,
    AK4951_4A_E5_COEFFICIENT0,
    AK4951_4B_E5_COEFFICIENT1,
    AK4951_4C_E5_COEFFICIENT2,
    AK4951_4D_E5_COEFFICIENT3,
    AK4951_4E_E5_COEFFICIENT4,
    AK4951_4F_E5_COEFFICIENT5,
    AK4951_REG_NUM
} AK4951_REG_e;

#define AK4951_R00H_PMADL_PWR_UP        1UL                /* Power Up for MIC-AMP and ADC L-channel */
#define AK4951_R00H_PMADR_PWR_UP        (1UL << 1UL)        /* Power Up for MIC-AMP and ADC R-channel */
#define AK4951_R00H_PMDAC_PWR_UP        (1UL << 2UL)        /* Power Up for DAC */
#define AK4951_R00H_PMBP_PWR_UP         (1UL << 5UL)        /* Power Up for BEEP */
#define AK4951_R00H_PMVCM_PWR_UP        (1UL << 6UL)        /* Power Up for VCOM */
#define AK4951_R00H_PMPFIL_PWR_UP       (1UL << 7UL)        /* Power Up for Programming Filter */

#define AK4951_R01H_LINEOUT_SEL         1UL                /* Stereo Line-out select */
#define AK4951_R01H_PMSL_PWR_UP         (1UL << 1UL)        /* power up for Speaker Amp or Stereo Line-out */
#define AK4951_R01H_PMPLL_MASK          0xFBUL            /* PMPLL: Power up PLL */
#define AK4951_R01H_PMPLL_EXT           0x00UL            /* PMPLL: External clock mode */
#define AK4951_R01H_PMPLL_PLL           0x04UL            /* PMPLL: PLL clock Mode */
#define AK4951_R01H_MS_MASK             0xF7UL            /* MS: Select Master/Slave Mode */
#define AK4951_R01H_MS_MASTER_MODE      0x08UL            /* MS: Set as clock Master */
#define AK4951_R01H_MS_SLAVE_MODE       0x00UL            /* MS: Set as clock Slave */
#define AK4951_R01H_PMHPL_PWR_UP        (1UL << 4UL)        /* Power Up for Headphone-Amp Rch */
#define AK4951_R01H_PMHPR_PWR_UP        (1UL << 5UL)        /* Power Up for Headphone-Amp Lch */
#define AK4951_R01H_PMOSC_PWR_UP        (1UL << 7UL)        /* Power Up for Internal Oscillator */

#define AK4951_R02H_MGAIN_MASK          0xB8UL        /* MIC-Amp Gain Control Mask (Table.24) */
#define AK4951_R02H_MGAIN_0DB           0x00UL        /* MiC-Amp Gain = 0dB */
#define AK4951_R02H_MGAIN_3DB           0x01UL        /* MiC-Amp Gain = +3dB */
#define AK4951_R02H_MGAIN_6DB           0x02UL        /* MiC-Amp Gain = +6dB */
#define AK4951_R02H_MGAIN_9DB           0x03UL        /* MiC-Amp Gain = +9dB */
#define AK4951_R02H_MGAIN_12B           0x04UL        /* MiC-Amp Gain = +12dB */
#define AK4951_R02H_MGAIN_15DB          0x05UL        /* MiC-Amp Gain = +15dB */
#define AK4951_R02H_MGAIN_18DB          0x06UL        /* MiC-Amp Gain = +18dB (Default) */
#define AK4951_R02H_MGAIN_21DB          0x07UL        /* MiC-Amp Gain = +21dB */
#define AK4951_R02H_MGAIN_24DB          0x40UL        /* MiC-Amp Gain = +24dB */
#define AK4951_R02H_MGAIN_27DB          0x41UL        /* MiC-Amp Gain = +27dB */
#define AK4951_R02H_MGAIN_30DB          0x42UL        /* MiC-Amp Gain = +30dB */
#define AK4951_R02H_PMMP_PWR_UP         0x08UL        /* Power Up for MPWR pin */
#define AK4951_R02H_MPWR_MASK           0xEFUL        /* MPWR output select  */
#define AK4951_R02H_MPWR1               0x00UL        /* MPWR output select: MPWR1(0) (Default) */
#define AK4951_R02H_MPWR2               0x10UL        /* MPWR output select: MPWR2(1) */
#define AK4951_R02H_DACS_SWITCH_ON      (1UL << 5UL)    /* Swtich from DAC to Speaker Amp */
#define AK4951_R02H_SLPSN               (1UL << 7UL)    /* Power save mode of speaker Amp or Stereo Line-out */

#define AK4951_R03H_INPUT_SEL_MASK      0x0FUL        /* ADC Input Source select Mask (Table.23) */
#define AK4951_R03H_INPUT_SEL_LIN1_RIN1 0x00UL        /* ADC Input: L=LIN1, R=RIN1 */
#define AK4951_R03H_INPUT_SEL_LIN1_RIN2 0x01UL        /* ADC Input: L=LIN1, R=RIN2 */
#define AK4951_R03H_INPUT_SEL_LIN1_RIN3 0x02UL        /* ADC Input: L=LIN1, R=RIN3 */
#define AK4951_R03H_INPUT_SEL_LIN2_RIN1 0x04UL        /* ADC Input: L=LIN2, R=RIN1 */
#define AK4951_R03H_INPUT_SEL_LIN2_RIN2 0x05UL        /* ADC Input: L=LIN2, R=RIN2 */
#define AK4951_R03H_INPUT_SEL_LIN2_RIN3 0x06UL        /* ADC Input: L=LIN2, R=RIN3 */
#define AK4951_R03H_INPUT_SEL_LIN3_RIN1 0x08UL        /* ADC Input: L=LIN3, R=RIN1 */
#define AK4951_R03H_INPUT_SEL_LIN3_RIN2 0x09UL        /* ADC Input: L=LIN3, R=RIN2 */
#define AK4951_R03H_INPUT_SEL_LIN3_RIN3 0x0AUL        /* ADC Input: L=LIN3, R=RIN3 */
#define AK4951_R03H_MIC_LEVEL           (1UL << 4UL)    /* MPWR ouput Voltage select(0=2.4V/1=2.0V) */
#define AK4951_R03H_SPK_OUT_GAIN_MASK   0x3FUL        /* SPKG: Speaker Amp Output Gain Select */
#define AK4951_R03H_SPK_OUT_GAIN_6DB4   0x00UL        /* SPKG:  +6.4dB, 3.37Vpp (Default) */
#define AK4951_R03H_SPK_OUT_GAIN_8DB4   0x40UL        /* SPKG:  +8.4dB, 4.23Vpp */
#define AK4951_R03H_SPK_OUT_GAIN_11DB1  0x80UL        /* SPKG:  +11.1dB, 5.33Vpp */
#define AK4951_R03H_SPK_OUT_GAIN_14DB9  0xC0UL        /* SPKG:  +14.9dB, 8.47Vpp */

#define AK4951_R04H_DAC_MONO_MASK       0xFCUL        /* MONO: DAC Mono/Stereo mode */
#define AK4951_R04H_DAC_MONO_L_R        0x00UL        /* MONO: L=L, R=R [Default]*/
#define AK4951_R04H_DAC_MONO_L_L        0x01UL        /* MONO: L=L, R=L */
#define AK4951_R04H_DAC_MONO_R_R        0x02UL        /* MONO: L=R, R=R */
#define AK4951_R04H_DAC_MONO_LR_LR      0x03UL        /* MONO: L=(L+R)/2, R=(L+R)/2 */
#define AK4951_R04H_BEEP_SOFT_TRAN_0    (1UL << 2UL)    /* BEEP(to HP) ON/OFF Soft-Transition Time 0 */
#define AK4951_R04H_BEEP_SOFT_TRAN_1    (1UL << 2UL)    /* BEEP(to HP) ON/OFF Soft-Transition Time 1 */
#define AK4951_R04H_DAC_TO_LINE_AMP     (1UL << 5UL)    /* DAC: signal from DAC to Stereo Line Amplifier */
#define AK4951_R04H_LVCM_MASK           0x3FUL        /* LVCM: Stereo Line output gain and common voltage setting */
#define AK4951_R04H_LVCM_0DB_1V3        0x00UL        /* LVCM: 2.8V~3.6V, 0dB, 1.3V*/
#define AK4951_R04H_LVCM_2DB_1V5        0x40UL        /* LVCM: 3.0V~3.6V, +2dB, 1.5V*/
#define AK4951_R04H_LVCM_2DB_1V3        0x80UL        /* LVCM: 2.8V~3.6V, +2dB, 1.3V*/
#define AK4951_R04H_LVCM_4DB_1V5        0xC0UL        /* LVCM: 3.0V~3.6V, +4dB, 1.5V*/

#define AK4951_R05H_DIF_MASK            0xFCUL
#define AK4951_R05H_DIF_O24MSB_I24LSB   0x00UL        /* DIFx: Output(ADC)24MSB, Input(DAC)24LSB  */
#define AK4951_R05H_DIF_O24MSB_I16LSB   0x01UL        /* DIFx: Output(ADC)24MSB, Input(DAC)16LSB */
#define AK4951_R05H_DIF_O24MSB_I24MSB   0x02UL        /* DIFx: Output(ADC)24MSB, Input(DAC)24MSB */
#define AK4951_R05H_DIF_I2S             0x03UL        /* DIFx: I2S compatible */
#define AK4951_R05H_CLOCK_OUTPUT        (1UL << 2UL )   /* CKOFF: BICK,LRCK,SDTO output in master mode (=Output/Stop)*/
#define AK4951_R05H_BICK_OUTPUT         (1UL << 3UL )   /* BCKO: BICK output frequency in master mode (=32fs/64fs)*/

#define AK4951_R05H_PLL_REF_CLK_MASK    0x0FUL
#define AK4951_R05H_PLL_BICK_32FS       0x20UL        /* input pin: BICK 32fs */
#define AK4951_R05H_PLL_BICK_64FS       0x30UL        /* input pin: BICK 64fs */
#define AK4951_R05H_PLL_MCKI_11_2896M   0x40UL        /* input pin: MCKI 11.2896MHz */
#define AK4951_R05H_PLL_MCKI_12_288M    0x50UL        /* input pin: MCKI 12.288MHz (default) */
#define AK4951_R05H_PLL_MCKI_12M        0x60UL        /* input pin: MCKI 12MHz */
#define AK4951_R05H_PLL_MCKI_24M        0x70UL        /* input pin: MCKI 24MHz */
#define AK4951_R05H_PLL_MCKI_13_5M      0xC0UL        /* input pin: MCKI 13.5MHz */
#define AK4951_R05H_PLL_MCKI_27M        0xD0UL        /* input pin: MCKI 27MHz */

#define AK4951_R06H_FS_MASK             0xF0UL        /* Sampling frequency */
/* PLL_MCKI or EXT_SLAVE(MCKI) or EXT_MASTER(MCKI) mode */
#define AK4951_R06H_FS_8KHZ             0x00UL        /*  fs=8KHz */
#define AK4951_R06H_FS_12KHZ            0x01UL        /*  fs=12KHz */
#define AK4951_R06H_FS_16KHZ            0x02UL        /*  fs=16KHz */
#define AK4951_R06H_FS_11K025HZ         0x05UL        /*  fs=11.025KHz */
#define AK4951_R06H_FS_22K05HZ          0x07UL        /*  fs=22.05KHz */
#define AK4951_R06H_FS_24KHZ            0x09UL        /*  fs=24KHz */
#define AK4951_R06H_FS_32KHZ            0x0AUL        /*  fs=32KHz */
#define AK4951_R06H_FS_48KHZ            0x0BUL        /*  fs=48KHz (Default) */
#define AK4951_R06H_FS_44K1HZ           0x0FUL        /*  fs=44.1KHz */
/* PLL_BICK mode */
#define AK4951_R06H_FS_PLL_BICK_8KHZ    0x00UL        /* PLL = BICK, fs=8KHz */
#define AK4951_R06H_FS_PLL_BICK_12KHZ   0x01UL        /* PLL = BICK, fs=12KHz */
#define AK4951_R06H_FS_PLL_BICK_11K025HZ    0x02UL    /* PLL = BICK, fs=11.025KHz */
#define AK4951_R06H_FS_PLL_BICK_16KHZ   0x05UL        /* PLL = BICK, fs=16KHz */
#define AK4951_R06H_FS_PLL_BICK_24KHZ   0x06UL        /* PLL = BICK, fs=24KHz */
#define AK4951_R06H_FS_PLL_BICK_22K05HZ 0x07UL        /* PLL = BICK, fs=22.05KHz */
#define AK4951_R06H_FS_PLL_BICK_44K1HZ  0x08UL        /* PLL = BICK, fs=44.1KHz */
#define AK4951_R06H_FS_PLL_BICK_32KHZ   0x0AUL        /* PLL = BICK, fs=32KHz */
#define AK4951_R06H_FS_PLL_BICK_48KHZ   0x0BUL        /* PLL = BICK, fs=48KHz (Default) */
/* CM */
#define AK4951_R06H_CM_MASK             0x3FUL        /* EXT mode, MCKI input frequency settings */
#define AK4951_R06H_CM_256FS            0x00UL        /* EXT(MCKI) freq: 256fs, 8KHz~48KHz (Default) */
#define AK4951_R06H_CM_384FS            0x01UL        /* EXT(MCKI) freq: 384fs, 8KHz~48KHz */
#define AK4951_R06H_CM_512FS            0x02UL        /* EXT(MCKI) freq: 512fs, 8KHz~48KHz */
#define AK4951_R06H_CM_1024FS           0x03UL        /* EXT(MCKI) freq: 1024fs, 8KHz~24KHz  */

#define AK4951_R07H_IN_VOL_CTRL_MODE    (1UL << 2UL)    /* IVOLC: Input Digital Volume control mode - 0:Independent/1:dependent(Default) */
#define AK4951_R07H_OUT_VOL_CTRL_MODE   (1UL << 4UL)    /* DVOLC: Output Digital Volume control mode - 0:Independent/1:dependent(Default) */
#define AK4951_R07H_SOFT_MUTE           (1UL << 5UL)    /* SMUTE: Soft Mute Control - 0:Normal(default)/1:Muted */
#define AK4951_R07H_THERMAL_SHUT_DET    (1UL << 6UL)    /* THDET: Thermal Shutdown Detect Result - 0:Normal(default)/1:DuringThermalShutdown */
#define AK4951_R07H_THERMAL_SHUT_MODE   (1UL << 7UL)    /* TSDSEL: Thermal Shutdown Mode Select - 0:AutoPowerUp(default)/1:ManualPowerUp */

#define AK4951_R08H_DMIC                1UL           /* DMIC: Digital Microphone Connection Set - 0:AnalogMic(default)/1:DigitalMic */
#define AK4951_R08H_DATA_LATCH_EDGE     (1UL << 1UL)    /* DCLKP: Data Latching Edge Select - Lch data is locked at DMCLK 0:RisingEdge(default)/1:FallingEdge */
#define AK4951_R08H_DMCLK_CONTROL       (1UL << 3UL)    /* DCLKE: DMCLK output clock - 0:LOutput(default)/1:64fs */
#define AK4951_R08H_PMDMLR              0x30UL        /* PMDML: Input Signal Select with DMIC, 00:00/01:RR/10:LL/11:LR */
#define AK4951_R08H_PMDMLL              0x20UL
#define AK4951_R08H_PMDMRR              0x10UL
#define AK4951_R08H_PMDMNONE            0x00UL
#define AK4951_R08H_3WIRE_READ_EN       (1UL << 7UL)    /* READ: 3-Wire Serial Read Enable - 0:Disable(Default)/1:Enable */

#define AK4951_R09H_OUTVOL_SOFT_TRANS   1UL           /* DVTM: Output Volume Soft Transition - 0:816fs(default)/1:204fs */
#define AK4951_R09H_BEEP_SOFT_TRANS     (1UL << 1UL)    /* MOFF: Beep to Headphone Soft Transition - 0:Enable(default)/1:Disable */
#define AK4951_R09H_ALC_FAST_RECOV      (1UL << 4UL)    /* FRN: ALC Fast Recovery - 0:Enable(default)/1:Disable */
#define AK4951_R09H_ALC_REF_ATTEN       (1UL << 5UL)    /* FRATT: ALC Fast Recovery Reference Volume Attenuation Amount- 0:4fs(default)/1:16fs */
#define AK4951_R09H_ADC_INIT_MASK       0x3FUL        /* ADRST: ADC Initial Cycle Setting Mask (Table.19) */
#define AK4951_R09H_ADC_INIT_1059FS     0x00UL        /* ADRST: 1059fs (default) */
#define AK4951_R09H_ADC_INIT_267FS      0x40UL        /* ADRST: 267fs */
#define AK4951_R09H_ADC_INIT_531FS      0x80UL        /* ADRST: 531fs */
#define AK4951_R09H_ADC_INIT_135FS      0xC0UL        /* ADRST: 135fs */

#define AK4951_R0AH_ALC_FAST_RECOV_MASK 0xFCUL        /* RFST: ALC Fast Recovery Settings Mask (Table.41) */
#define AK4951_R0AH_ALC_FAST_RECOV_32   0x00UL        /* RFST: 0.0032 dB (default) */
#define AK4951_R0AH_ALC_FAST_RECOV_42   0x01UL        /* RFST: 0.0042 dB */
#define AK4951_R0AH_ALC_FAST_RECOV_64   0x02UL        /* RFST: 0.0064 dB */
#define AK4951_R0AH_ALC_FAST_RECOV_127  0x03UL        /* RFST: 0.0127 dB */
#define AK4951_R0AH_ALC_WAIT_MASK       0xF3UL        /* WTM: ALC Recovery Waiting Period Mask (Table.38) */
#define AK4951_R0AH_ALC_WAIT_128FS      0x00UL        /* WTM: 128fs (default) */
#define AK4951_R0AH_ALC_WAIT_256FS      0x04UL        /* WTM: 256fs */
#define AK4951_R0AH_ALC_WAIT_512FS      0x08UL        /* WTM: 512fs */
#define AK4951_R0AH_ALC_WAIT_1024FS     0x0CUL        /* WTM: 1024fs */
#define AK4951_R0AH_ALC_EQ_FREQ_MASK    0xCFUL        /* EQFC: ALCEQ Frequency Settings Mask (Table.35) */
#define AK4951_R0AH_ALC_EQ_FREQ_12KHZ   0x00UL        /* EQFC: 8KHz <= fs <= 12KHz */
#define AK4951_R0AH_ALC_EQ_FREQ_24KHZ   0x10UL        /* EQFC: 12KHz <= fs <= 24KHz */
#define AK4951_R0AH_ALC_EQ_FREQ_48KHZ   0x20UL        /* EQFC: 24KHz <= fs <= 48KHz (default) */

#define AK4951_R0BH_ALC_LIMITER_MASK    0xBCUL       /* LMTH: ALC Limiter Detection Level/Recovery Counter Reset Level (Table.36) */
#define AK4951_R0BH_ALC_LIMITER_41DBFS  0x00UL        /* LMTH: -2.5dBFS > ALC Output >= -4.1dBFS (default)  */
#define AK4951_R0BH_ALC_LIMITER_33DBFS  0x01UL        /* LMTH: -2.5dBFS > ALC Output >= -3.3dBFS */
#define AK4951_R0BH_ALC_LIMITER_60DBFS  0x02UL        /* LMTH: -4.1dBFS > ALC Output >= -6.0dBFS */
#define AK4951_R0BH_ALC_LIMITER_50DBFS  0x03UL        /* LMTH: -4.1dBFS > ALC Output >= -5.0dBFS */
#define AK4951_R0BH_ALC_LIMITER_85DBFS  0x40UL        /* LMTH: -6.0dBFS > ALC Output >= -8.5dBFS */
#define AK4951_R0BH_ALC_LIMITER_72DBFS  0x41UL        /* LMTH: -6.0dBFS > ALC Output >= -7.2dBFS */
#define AK4951_R0BH_ALC_LIMITER_120DBFS 0x42UL        /* LMTH: -8.5dBFS > ALC Output >= -12.0dBFS */
#define AK4951_R0BH_ALC_LIMITER_101DBFS 0x43UL        /* LMTH: -8.5dBFS > ALC Output >= -10.1BFS */
#define AK4951_R0BH_ALC_RGAIN_MASK      0xE3UL        /* RGAIN: ALC Recovery Gain Step (Table.39) */
#define AK4951_R0BH_ALC_RGAIN_424_1FS   0x00UL        /* RGAIN: Gain=0.00424, Timing = 1fs  (default) */
#define AK4951_R0BH_ALC_RGAIN_212_1FS   0x04UL        /* RGAIN: Gain=0.00212, Timing = 1fs */
#define AK4951_R0BH_ALC_RGAIN_106_1FS   0x08UL        /* RGAIN: Gain=0.00106, Timing = 1fs */
#define AK4951_R0BH_ALC_RGAIN_106_2FS   0x0CUL        /* RGAIN: Gain=0.00106, Timing = 2fs */
#define AK4951_R0BH_ALC_RGAIN_106_4FS   0x10UL        /* RGAIN: Gain=0.00106, Timing = 4fs */
#define AK4951_R0BH_ALC_RGAIN_106_8FS   0x14UL        /* RGAIN: Gain=0.00106, Timing = 8fs */
#define AK4951_R0BH_ALC_RGAIN_106_16FS  0x18UL        /* RGAIN: Gain=0.00106, Timing = 16fs */
#define AK4951_R0BH_ALC_RGAIN_106_32FS  0x1CUL        /* RGAIN: Gain=0.00106, Timing = 32fs */
#define AK4951_R0BH_ALC                 (1UL << 5UL)    /* ALC: ALC Enable - 0:Diable(Default)/1:Enable */
#define AK4951_R0BH_ALC_EQ              (1UL << 7UL)    /* ALCEQN: ALC EQ Enable - 0:On(Default)/1:Off */

#define AK4951_R12H_BEEP_LEVEL_MASK     0xF0UL        /* BPLVL: Beep output level mask (Table.55) */
#define AK4951_R12H_BEEP_LEVEL_0        0x00UL        /* Beep Gain = 0dB (default) */
#define AK4951_R12H_BEEP_LEVEL_MINUS6       0x01UL        /* Beep Gain = -6dB   */
#define AK4951_R12H_BEEP_LEVEL_MINUS12      0x02UL        /* Beep Gain = -12dB  */
#define AK4951_R12H_BEEP_LEVEL_MINUS18      0x03UL        /* Beep Gain = -18dB  */
#define AK4951_R12H_BEEP_LEVEL_MINUS24      0x04UL        /* Beep Gain = -24dB  */
#define AK4951_R12H_BEEP_LEVEL_MINUS30      0x05UL        /* Beep Gain = -30dB  */
#define AK4951_R12H_BEEP_LEVEL_MINUS33      0x06UL        /* Beep Gain = -33dB  */
#define AK4951_R12H_BEEP_LEVEL_MINUS36      0x07UL        /* Beep Gain = -36dB  */
#define AK4951_R12H_BEEP_LEVEL_MINUS39      0x08UL        /* Beep Gain = -39dB  */
#define AK4951_R12H_BEEP_LEVEL_MINUS42      0x09UL        /* Beep Gain = -42dB  */
#define AK4951_R12H_BEEP_TO_HP          (1UL << 4UL)    /* BEEPH: Beep to Headphone Amp - 0:OFF(Default)/1:ON */
#define AK4951_R12H_BEEP_TO_SPK         (1UL << 5UL)    /* BEEPS: Beep to Speaker Amp - 0:OFF(Default)/1:ON */
#define AK4951_R12H_BEEP_VOLTAGE        (1UL << 6UL)    /* BPVCM: Common Voltage setting of BEEP input Amp - 0:1.15V(Default)/1:1.65V */
#define AK4951_R12H_HP_PULL_DOWN        (1UL << 7UL)    /* HPZ: Headphone Amp Pull-down setting - 0:by 10 Ohm(Default)/1:Hi-Z */

#define AK4951_R15H_EQ2_GAIN            (1UL << 1UL)    /* EQC2: Equalizer 2 Common Gain - 0:Disable(Default)/1:Enable */
#define AK4951_R15H_EQ3_GAIN            (1UL << 2UL)    /* EQC3: Equalizer 3 Common Gain - 0:Disable(Default)/1:Enable */
#define AK4951_R15H_EQ4_GAIN            (1UL << 3UL)    /* EQC4: Equalizer 4 Common Gain - 0:Disable(Default)/1:Enable */
#define AK4951_R15H_EQ5_GAIN            (1UL << 4UL)    /* EQC5: Equalizer 5 Common Gain - 0:Disable(Default)/1:Enable */

#define AK4951_R1AH_WIND_NOISE_ATT_MASK 0xFCUL        /* STG: Attenuation level for Wind Noise Reduction Filter (Table.31) */
#define AK4951_R1AH_WIND_NOISE_ATT_LOW  0x00UL        /* STG: Low */
#define AK4951_R1AH_WIND_NOISE_ATT_MID1 0x01UL        /* STG: Middle 1 */
#define AK4951_R1AH_WIND_NOISE_ATT_MID2 0x02UL        /* STG: Middle 2 */
#define AK4951_R1AH_WIND_NOISE_ATT_HIGH 0x03UL        /* STG: High */
#define AK4951_R1AH_WIND_NOISE_SENSI_MASK   0xE3UL    /* SENC: Wind Noise Detection Sensitivity (Table.30)  */
#define AK4951_R1AH_WIND_NOISE_SENSI_05 0x00UL        /* SENC: 0.5 (Low) */
#define AK4951_R1AH_WIND_NOISE_SENSI_10 0x04UL        /* SENC: 1.0  */
#define AK4951_R1AH_WIND_NOISE_SENSI_15 0x08UL        /* SENC: 1.5  */
#define AK4951_R1AH_WIND_NOISE_SENSI_20 0x0CUL        /* SENC: 2.0 (Default) */
#define AK4951_R1AH_WIND_NOISE_SENSI_25 0x10UL        /* SENC: 2.5  */
#define AK4951_R1AH_WIND_NOISE_SENSI_30 0x14UL        /* SENC: 3.0  */
#define AK4951_R1AH_WIND_NOISE_SENSI_35 0x18UL        /* SENC: 3.5  */
#define AK4951_R1AH_WIND_NOISE_SENSI_40 0x1CUL        /* SENC: 4.0(HIGH)  */
#define AK4951_R1AH_WIND_NOISE          (1UL << 5UL)    /* AHPF: Automatic Wind Noise Reduction Filter - 0:OFF(Default)/1:ON  */

#define AK4951_R1BH_HPF1_ENABLE         (1UL << 4UL)    /* HPFAD: HPF1 Settings Enable - 0:OFF/1:ON(Default) */
#define AK4951_R1BH_HPF1_CUTOFF_MASK    0xF9UL        /* HPFC: HPF1 Cut-off frequency (Table.28) */
#define AK4951_R1BH_HPF1_CUTOFF_0       0x00UL        /* HPFC: HPF1 Cut-off frequency 0 (Default) */
#define AK4951_R1BH_HPF1_CUTOFF_1       0x02UL        /* HPFC: HPF1 Cut-off frequency 1 */
#define AK4951_R1BH_HPF1_CUTOFF_2       0x04UL        /* HPFC: HPF1 Cut-off frequency 2 */
#define AK4951_R1BH_HPF1_CUTOFF_3       0x06UL        /* HPFC: HPF1 Cut-off frequency 3 */

#define AK4951_R1CH_HPF2_ENABLE         1UL           /* HPF: HPF2 Coefficient Setting Enable - 0:OFF(Default)/1:ON */
#define AK4951_R1CH_LPF_ENABLE          (1UL << 1UL)    /* LPF: LPF Coefficient Setting Enable - 0:OFF(Default)/1:ON */
#define AK4951_R1CH_FIL3_ENABLE         (1UL << 4UL)    /* FIL3: FIL3(Stereo Emphasis Filter) Coefficient Setting Enable - 0:Disable(Default)/1:Enable */
#define AK4951_R1CH_EQ0_ENABLE          (1UL << 5UL)    /* EQ0: Equalizer 0 Coefficient Setting Enable - 0:Disable(Default)/1:Enable */
#define AK4951_R1CH_GAIN_MASK           0x3FUL        /* GN: Gain Setting of Gain Block (Table.32) */
#define AK4951_R1CH_GAIN_0DB            0x00UL        /* GN: Gain = 0dB */
#define AK4951_R1CH_GAIN_12DB           0x40UL        /* GN: Gain = +12dB */
#define AK4951_R1CH_GAIN_24DB           0x80UL        /* GN: Gain = +24dB */

#define AK4951_R1DH_PFSDO               1UL           /* PFSDO: SDTO Output Signal Select - 0:ADC/1:ProgrammableFilter(default) */
#define AK4951_R1DH_ADCPF               (1UL << 1UL)    /* ADCPF: ProgrammableFilter Input Signal Select - 0:SDTI/1:ADC(default) */
#define AK4951_R1DH_PFDAC_MASK          0xF3UL        /* PFDAC: DAC Input Signal Select (Table.49) */
#define AK4951_R1DH_PFDAC_SDTI          0x00UL        /* PFDAC: SDTI (Default) */
#define AK4951_R1DH_PFDAC_PFVOL         0x04UL        /* PFDAC: PFVOL */
#define AK4951_R1DH_PFDAC_SDTI_PFVOL    0x08UL        /* PFDAC: (SDTI+PFVOL)/2 */
#define AK4951_R1DH_PFVOL_MASK          0xCFUL        /* PFVOL: Sidetone Digital Volume (Table.48) */
#define AK4951_R1DH_PFVOL_0DB           0x00UL        /* PFVOL: 0dB */
#define AK4951_R1DH_PFVOL_MINUS6DB          0x10UL        /* PFVOL: -6dB */
#define AK4951_R1DH_PFVOL_MINUS12DB         0x20UL        /* PFVOL: -12dB*/
#define AK4951_R1DH_PFVOL_MINUS18DB         0x30UL        /* PFVOL: -18dB */

#define AK4951_R30H_EQ1                 1UL           /* EQ1: EQ1 Coefficient Setting Enable - 0:Disable(Default)/1:Enable */
#define AK4951_R30H_EQ2                 (1UL << 1UL)    /* EQ2: EQ2 Coefficient Setting Enable - 0:Disable(Default)/1:Enable */
#define AK4951_R30H_EQ3                 (1UL << 2UL)    /* EQ3: EQ3 Coefficient Setting Enable - 0:Disable(Default)/1:Enable */
#define AK4951_R30H_EQ4                 (1UL << 3UL)    /* EQ4: EQ4 Coefficient Setting Enable - 0:Disable(Default)/1:Enable */
#define AK4951_R30H_EQ5                 (1UL << 4UL)    /* EQ5: EQ5 Coefficient Setting Enable - 0:Disable(Default)/1:Enable */

#define AK4951_R31H_DVN_MASK            0xF0UL        /* DVN: Device No ID (read only) */
#define AK4951_R31H_REV_MASK            0x0FUL        /* REV: Device Revision ID (read only) */

extern AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_AK4951Obj;

#endif /* AMBA_AUDIO_AK4951_H */
