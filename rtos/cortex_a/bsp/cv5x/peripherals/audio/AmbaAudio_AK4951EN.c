/**
 *  @file AmbaAudio_AK4951EN.c
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

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"

#include "AmbaI2C.h"
#include "AmbaAudio_AK4951EN.h"

/*---------------  AKM Definition (Customer have to change this)-----------------------*/
/* Clock Source */
/*#define CLOCK_SOURCE    AK4951_R01H_PMPLL_EXT       */
static UINT32 CLOCK_SOURCE = AK4951_R01H_PMPLL_EXT;// Clock source: External(MCKI)/PLL(MCKI, BICK)
//      - AK4951_R01H_PMPLL_EXT
//      - AK4951_R01H_PMPLL_PLL
/*#define PLL_MODE        AK4951_R05H_PLL_BICK_64FS   */
static UINT32 PLL_MODE = AK4951_R05H_PLL_BICK_64FS;//  PLL reference clock
//      - AK4951_R05H_PLL_BICK_32FS
//      - AK4951_R05H_PLL_BICK_64FS
//      - AK4951_R05H_PLL_MCKI_11_2896M
//      - AK4951_R05H_PLL_MCKI_12_288M
//      - AK4951_R05H_PLL_MCKI_12M
//      - AK4951_R05H_PLL_MCKI_24M
//      - AK4951_R05H_PLL_MCKI_13_5M
//      - AK4951_R05H_PLL_MCKI_27M

/* Mode Control */
#define AUDIO_INTERFACE AUCODEC_I2S                         // Audio Interface Format (SDTO/SDTI)
//      - 24MSB/24LSB
//      - 24MSB/16LSB
//      - 24MSB/24MSB
//      - I2S
#define FS_SETTING      48000                       // Sampling Frequency
//      - 8000
//      - 11025
//      - 12000
//      - 16000
//      - 22050
//      - 24000
//      - 32000
//      - 44100
//      - 48000

//#define MIC_TYPE      A_MIC                        // A_MIC : Analog Mic
// SD_MIC: Stereo Digital Mic
// LD_MIC: LEFT Digital Mic
// RD_MIC: Right Digital Mic
/* Path */
#define RECORDING_USING_PROGRAMABLE_FILTER      // default enable
//#define PLAYBACK_USING_PROGRAMABLE_FILTER     // default disable
//#define MIC_RECORDING_PORT  1                   // 1:MIC - LIN1/RIN1, Line-In - LIN2/RIN2
// 2:MIC - LIN2/RIN2, Line-In - LIN1/RIN1

typedef enum {
    A_MIC = 0x00,
    SD_MIC,   // Stereo DMIC
    LD_MIC,   // Lch DMIC
    RD_MIC    // Rch DMIC
} AK4951_MIC_TYPE_e;

typedef enum {
    INPUT_PORT1 = 0x00,
    INPUT_PORT2,
    INPUT_PORT3
} AK4951_INPUT_PORT_e;

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_Write
 *
 *  @Description:: Audio Codec write register
 *
 *  @Input      ::
 *      Addr:   I2C Slave Address
 *      Data:   I2C write Data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AK4951_Write(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, UINT32 Data)
{
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[2];
    UINT32 TxSize;
    UINT32 RetVal = AUCODEC_ERR_NONE;

    I2cConfig.SlaveAddr = pCodecCtrl->I2cCtrl.SlaveAddr;
    I2cConfig.DataSize = 2U;
    I2cConfig.pDataBuf = TxDataBuf;

    TxDataBuf[0] = (UINT8)Addr;
    TxDataBuf[1] = (UINT8)Data;

    if (Addr < (UINT32)AK4951_REG_NUM) {
        RetVal = AmbaI2C_MasterWrite(pCodecCtrl->I2cCtrl.I2cChanNo, pCodecCtrl->I2cCtrl.I2cSpeed,
                                     &I2cConfig, &TxSize, AMBA_KAL_WAIT_FOREVER);
    } else {
        RetVal = AUCODEC_ERR_ARG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_Read
 *
 *  @Description:: Audio Codec read register
 *
 *  @Input      ::
 *      Addr:   I2C Slave Address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : I2C read Data / NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AK4951_Read(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, const UINT32 *pRxData)
{
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};

    UINT8 TxData;
    UINT32 TxSize;
    UINT32 RetVal = AUCODEC_ERR_NONE;

    I2cTxConfig.SlaveAddr = pCodecCtrl->I2cCtrl.SlaveAddr;
    I2cTxConfig.DataSize = 1U;
    I2cTxConfig.pDataBuf = &TxData;

    AmbaMisra_TypeCast32(&TxData, &Addr);/*TxData = Addr;*/  /* Sub Address */

    I2cRxConfig.SlaveAddr = pCodecCtrl->I2cCtrl.SlaveAddr | 1U;
    I2cRxConfig.DataSize  = 1U;
    AmbaMisra_TypeCast32(&(I2cRxConfig.pDataBuf), &pRxData);/*I2cRxConfig.pDataBuf  = (UINT8 *)pRxData;*/

    if (Addr < (UINT32)AK4951_REG_NUM) {
        RetVal = AmbaI2C_MasterReadAfterWrite(pCodecCtrl->I2cCtrl.I2cChanNo, pCodecCtrl->I2cCtrl.I2cSpeed,
                                              1U, &I2cTxConfig, &I2cRxConfig, &TxSize, AMBA_KAL_WAIT_FOREVER);
    } else {
        RetVal = AUCODEC_ERR_ARG;
    }
    return RetVal;
}

static UINT32 AK4951_Write_App(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, UINT32 Data)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        RetVal = AK4951_Write(pCodecCtrl, Addr, Data);
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }
    return RetVal;
}

static UINT32 AK4951_Read_App(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, const UINT32 *pRxData)
{

    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        RetVal = AK4951_Read(pCodecCtrl, Addr, pRxData);
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_SpeakerOutPowerOn
 *
 *  @Description:: Speaker output Power On
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AK4951_SpeakerOutPowerOn(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data = 0xFFU;

    /*set DAC path to SPK/Line-out */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_02_SIGNAL_SELECT1, &Data);
    Data = Data | (UINT32)AK4951_R02H_DACS_SWITCH_ON;
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_02_SIGNAL_SELECT1, Data);

    /* SPK output Gain */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_03_SIGNAL_SELECT2, &Data);
    Data = Data & ((UINT32)AK4951_R03H_SPK_OUT_GAIN_MASK);
    /*Data = Data | (UINT32)AK4951_R03H_SPK_OUT_GAIN_6DB4;*/
    (void)AK4951_Write(pCodecCtrl,  (UINT32)AK4951_03_SIGNAL_SELECT2, Data);

    /* Digital output Volume - 0dB */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_13_L_DVOL, &Data);
    Data = Data | 0x18U;
    (void)AK4951_Write(pCodecCtrl,  (UINT32)AK4951_13_L_DVOL, Data);
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_14_R_DVOL, &Data);
    Data = Data | 0x18U;
    (void)AK4951_Write(pCodecCtrl,  (UINT32)AK4951_14_R_DVOL, Data);

    /* OPTION: Programmable Filter settings here */
#ifdef PLAYBACK_USING_PROGRAMABLE_FILTER
    /* TODO: settings in programmable filter */
#endif

    /* set output to SPK */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_01_POWER_MANAGEMENT2, &Data);
    Data = Data  & (~(UINT32)AK4951_R01H_LINEOUT_SEL);
    (void)AK4951_Write(pCodecCtrl,  (UINT32)AK4951_01_POWER_MANAGEMENT2, Data);

    /* Power up ProgrammableFilter & DAC */
    (void)AK4951_Read(pCodecCtrl,  (UINT32)AK4951_00_POWER_MANAGEMENT1, &Data);
#ifdef PLAYBACK_USING_PROGRAMABLE_FILTER
    Data = Data | (UINT32)AK4951_R00H_PMPFIL_PWR_UP;
#endif
    Data = Data | (UINT32)AK4951_R00H_PMDAC_PWR_UP;
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, Data);

    /* Power up SPK/Line-out */
    (void)AK4951_Read(pCodecCtrl,  (UINT32)AK4951_01_POWER_MANAGEMENT2, &Data);
    Data = Data | (((UINT32)AK4951_R01H_PMSL_PWR_UP));
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_01_POWER_MANAGEMENT2, Data);

    /* wait > 1ms */

    /* Normal output(leaving power-saving)  */
    (void)AK4951_Read(pCodecCtrl,  (UINT32)AK4951_02_SIGNAL_SELECT1, &Data);
    Data = Data  | (((UINT32)AK4951_R02H_SLPSN));
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_02_SIGNAL_SELECT1, Data);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_StereoLineOutPowerOn
 *
 *  @Description:: Stereo Line-out Power On
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AK4951_StereoLineOutPowerOn(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data = 0xFFU;

    /*set DAC path to SPK/Line-out */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_02_SIGNAL_SELECT1, &Data);
    Data = Data  | ((UINT32)AK4951_R02H_DACS_SWITCH_ON);
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_02_SIGNAL_SELECT1, Data);

    /* Digital output Volume - 0dB */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_13_L_DVOL, &Data);
    Data = Data | 0x18U;
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_13_L_DVOL, Data);

    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_14_R_DVOL, &Data);
    Data = Data | 0x18U;
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_14_R_DVOL, Data);

    /* OPTION: Programmable Filter settings here */
#ifdef PLAYBACK_USING_PROGRAMABLE_FILTER
    /* TODO: settings in programmable filter */
#endif

    /* set output to Line-out */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_01_POWER_MANAGEMENT2, &Data);
    Data = Data | ((UINT32)AK4951_R01H_LINEOUT_SEL);
    (void)AK4951_Write(pCodecCtrl,  (UINT32)AK4951_01_POWER_MANAGEMENT2, Data);

    /* Power up ProgrammableFilter & DAC */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, &Data);
#ifdef PLAYBACK_USING_PROGRAMABLE_FILTER
    Data = Data | (((UINT32)AK4951_R00H_PMPFIL_PWR_UP));
#endif
    Data = Data | (((UINT32)AK4951_R00H_PMDAC_PWR_UP));
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, Data);

    /* Power up SPK/Line-out */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_01_POWER_MANAGEMENT2, &Data);
    Data = Data | (((UINT32)AK4951_R01H_PMSL_PWR_UP));
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_01_POWER_MANAGEMENT2, Data);

    /* wait > 1ms */

    /* Normal output(leaving power-saving)  */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_02_SIGNAL_SELECT1, &Data);
    Data = Data | (((UINT32)AK4951_R02H_SLPSN));
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_02_SIGNAL_SELECT1, Data);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_SpeakerLineOutPowerOff
 *
 *  @Description:: Speaker or Stereo Line-out Power Off
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AK4951_SpeakerLineOutPowerOff(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data = 0xFFU;

    /* Power-saving */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_02_SIGNAL_SELECT1, &Data);
    Data = Data & (~(UINT32)AK4951_R02H_SLPSN);
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_02_SIGNAL_SELECT1, Data);

    /* Power down SPK/Line-out */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_01_POWER_MANAGEMENT2, &Data);
    Data = Data & (~((UINT32)AK4951_R01H_PMSL_PWR_UP));
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_01_POWER_MANAGEMENT2, Data);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_StereoHeadphonePowerOn
 *
 *  @Description:: Stereo Headphone Power On
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AK4951_StereoHeadphonePowerOn(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data = 0xFFU;

    /* Digital output Volume - 0dB */

    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_13_L_DVOL, &Data);
    Data = Data | 0x18U;
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_13_L_DVOL, Data);
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_14_R_DVOL, &Data);
    Data = Data | 0x18U;
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_14_R_DVOL, Data);

    /* OPTION: Programmable Filter settings here */
#ifdef PLAYBACK_USING_PROGRAMABLE_FILTER
    /* TODO: settings in programmable filter */
#endif

    /* Power up ProgrammableFilter & DAC */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, &Data);
#ifdef PLAYBACK_USING_PROGRAMABLE_FILTER
    Data = Data | (((UINT32)AK4951_R00H_PMPFIL_PWR_UP));
#endif
    Data = Data | (((UINT32)AK4951_R00H_PMDAC_PWR_UP));
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, Data);

    /* Power up Headphone Amp */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_01_POWER_MANAGEMENT2, &Data);
    Data = Data | (UINT32)AK4951_R01H_PMHPL_PWR_UP;
    Data = Data | (UINT32)AK4951_R01H_PMHPR_PWR_UP;
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_01_POWER_MANAGEMENT2, Data);

    /* wait 34.2ms */
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_StereoHeadphonePowerOff
 *
 *  @Description:: Stereo Headphone Power Off
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AK4951_StereoHeadphonePowerOff(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data = 0xFFU;

    /* Power down Headphone Amp */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_01_POWER_MANAGEMENT2, &Data);
    Data = Data & (~((UINT32)AK4951_R01H_PMHPL_PWR_UP));
    Data = Data & (~((UINT32)AK4951_R01H_PMHPR_PWR_UP));
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_01_POWER_MANAGEMENT2, Data);
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_MicInPowerOn
 *
 *  @Description:: A-Mic Power On
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AK4951_MicInPowerOn(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, AK4951_MIC_TYPE_e MicType, AK4951_INPUT_PORT_e InputPort)
{
    UINT32 Data = 0xFFU;

    /* Source select */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_03_SIGNAL_SELECT2, &Data);
#if 0
    if (InputPort == INPUT_PORT1) {
        Data = Data | ((UINT32)AK4951_R03H_INPUT_SEL_LIN1_RIN1);        /* L=LIN1, R=RIN1 */
    } else if (InputPort == INPUT_PORT2) {
        Data = Data | ((UINT32)AK4951_R03H_INPUT_SEL_LIN2_RIN2);        /* L=LIN2, R=RIN2 */
    } else if (InputPort == INPUT_PORT3) {
        Data = Data | ((UINT32)AK4951_R03H_INPUT_SEL_LIN3_RIN3);        /* L=LIN3, R=RIN3 */
    } else {
        Data = Data | ((UINT32)AK4951_R03H_INPUT_SEL_LIN1_RIN1);        /* L=LIN1, R=RIN1 (default) */
    }
#else
    if (InputPort == INPUT_PORT2) {
        Data = Data & ((UINT32)AK4951_R03H_INPUT_SEL_MASK);
        Data = Data | ((UINT32)AK4951_R03H_INPUT_SEL_LIN2_RIN2);        /* L=LIN2, R=RIN2 */
    } else if (InputPort == INPUT_PORT3) {
        Data = Data & ((UINT32)AK4951_R03H_INPUT_SEL_MASK);
        Data = Data | ((UINT32)AK4951_R03H_INPUT_SEL_LIN3_RIN3);        /* L=LIN3, R=RIN3 */
    } else {
        Data = Data & ((UINT32)AK4951_R03H_INPUT_SEL_MASK);
    }
#endif
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_03_SIGNAL_SELECT2, Data);

    /* Analog Mic */
    if (MicType == A_MIC) {
        (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_08_DIGITAL_MIC, &Data);
        Data = Data & (~(UINT32)AK4951_R08H_DMIC);
        (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_08_DIGITAL_MIC, Data);
    } else {
        (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_08_DIGITAL_MIC, &Data);
        Data = Data & (~(UINT32)(AK4951_R08H_DMIC |AK4951_R08H_PMDMLR|AK4951_R08H_DMCLK_CONTROL|AK4951_R08H_DATA_LATCH_EDGE));
        if(MicType == SD_MIC) {
            Data= Data |((UINT32)(AK4951_R08H_DMIC |AK4951_R08H_PMDMLR|AK4951_R08H_DMCLK_CONTROL));
        } else if (MicType == LD_MIC) {
            Data= Data |((UINT32)(AK4951_R08H_DMIC |AK4951_R08H_PMDMLL|AK4951_R08H_DMCLK_CONTROL));
        } else if (MicType == RD_MIC) {
            Data= Data |((UINT32)(AK4951_R08H_DMIC |AK4951_R08H_PMDMRR|AK4951_R08H_DMCLK_CONTROL));
        } else {
            Data= Data |((UINT32)(AK4951_R08H_DMIC |AK4951_R08H_PMDMLR|AK4951_R08H_DMCLK_CONTROL));
        }
        (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_08_DIGITAL_MIC, Data);
    }

    /* Set Input Volume = +30dB (0xe1)*/
    Data = 0xe1;
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_0D_L_IN_VOLUME, Data);
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_0E_R_IN_VOLUME, Data);

    /* OPTION: Programmable Filter settings here */
#ifdef RECORDING_USING_PROGRAMABLE_FILTER
    /* TODO: settings in programmable filter */
#endif
    if (MicType == A_MIC) {
        /* Mic bias enable */
        (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_02_SIGNAL_SELECT1, &Data);
        Data = Data & ((UINT32)AK4951_R02H_MPWR_MASK);
#if 0
        if (InputPort == INPUT_PORT1) {
            Data = Data | ((UINT32)AK4951_R02H_MPWR1);      /* MPWR1 - Mic Bias selection */
        } else if ( InputPort == INPUT_PORT2) {
            Data = Data | ((UINT32)AK4951_R02H_MPWR2);      /* MPWR2 - Mic Bias selection */
        } else {
            Data = Data | ((UINT32)AK4951_R02H_MPWR1);      /* MPWR1 - Mic Bias selection (default)*/
        }
#else
        if (InputPort == INPUT_PORT2) {
            Data = Data | ((UINT32)AK4951_R02H_MPWR2);      /* MPWR2 - Mic Bias selection */
        }
#endif
        Data = Data & ((UINT32)AK4951_R02H_MGAIN_MASK);
        Data = Data | ((UINT32)AK4951_R02H_MGAIN_18DB);     /* MGAIN - Mic Gain 18dB */
        Data = Data | ((UINT32)AK4951_R02H_PMMP_PWR_UP);    /* PMMP - Mic Bias power up */
    } else {
        /* Mic bias Disable */
        (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_02_SIGNAL_SELECT1, &Data);
        Data = Data & (~(UINT32)AK4951_R02H_PMMP_PWR_UP);
    }
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_02_SIGNAL_SELECT1, Data);

    /* Power up ProgrammableFilter */
#ifdef RECORDING_USING_PROGRAMABLE_FILTER
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, &Data);
    Data = Data | ((UINT32)AK4951_R00H_PMPFIL_PWR_UP);
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, Data);
#endif

    /* AK4951 ADC Power up  */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, &Data);
    Data = Data | ((UINT32)AK4951_R00H_PMADL_PWR_UP); /* when digital MIC enable , PMADL/R setting don't affect the Digi Mic power manager */
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, Data);
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, &Data);
    Data = Data | ((UINT32)AK4951_R00H_PMADR_PWR_UP);
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, Data);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_LineInPowerOn
 *
 *  @Description:: Line-in Power On
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AK4951_LineInPowerOn(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, AK4951_INPUT_PORT_e InputPort)
{
    UINT32 Data;

    /* Set 0dB (0x91)*/
    Data = 0x91U;
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_0D_L_IN_VOLUME, Data);
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_0E_R_IN_VOLUME, Data);

    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_03_SIGNAL_SELECT2, &Data);
    Data = Data & ((UINT32)AK4951_R03H_INPUT_SEL_MASK);
    if (InputPort == INPUT_PORT1) {
        Data = Data & ((UINT32)AK4951_R03H_INPUT_SEL_MASK);
        /*Data = Data | ((UINT32)AK4951_R03H_INPUT_SEL_LIN1_RIN1);*/        /* L=LIN1, R=RIN1 */
    } else if (InputPort == INPUT_PORT2) {
        Data = Data | ((UINT32)AK4951_R03H_INPUT_SEL_LIN2_RIN2);        /* L=LIN2, R=RIN2 */
    } else if (InputPort == INPUT_PORT3) {
        Data = Data | ((UINT32)AK4951_R03H_INPUT_SEL_LIN3_RIN3);        /* L=LIN3, R=RIN3 */
    } else {
        Data = Data & ((UINT32)AK4951_R03H_INPUT_SEL_MASK);
        /*Data = Data | ((UINT32)AK4951_R03H_INPUT_SEL_LIN1_RIN1);*/        /* L=LIN1, R=RIN1 (default) */
    }
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_03_SIGNAL_SELECT2, Data);

    /* Power up ProgrammableFilter */
#ifdef RECORDING_USING_PROGRAMABLE_FILTER
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, &Data);
    Data = Data | ((UINT32)AK4951_R00H_PMPFIL_PWR_UP);
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, Data);
#endif

    /* AK4951 ADC Power up  */
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, &Data);
    Data = Data | ((UINT32)AK4951_R00H_PMADL_PWR_UP);
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, Data);
    (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, &Data);
    Data = Data | ((UINT32)AK4951_R00H_PMADR_PWR_UP);
    (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, Data);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_SetOutput
 *
 *  @Description:: Audio Codec set output path
 *
 *  @Input      ::
 *      AMBA_AUDIO_CODEC_OUTPUT_MODE:   Audio Codec Output path
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AK4951_SetOutput(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 OutputMode)
{
    UINT32 Data = 0xFFU;
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_1D_D_FILTER_MODE, &Data);
        /* reference Fig.36 */
#ifdef PLAYBACK_USING_PROGRAMABLE_FILTER
        /* Path from Programmable Filter(ADC) */
        Data = Data & (~(UINT32)AK4951_R1DH_ADCPF);         /* ADCPF = 0 */
        Data = Data & (UINT32)AK4951_R1DH_PFDAC_MASK;
        Data = Data | ((UINT32)AK4951_R1DH_PFDAC_PFVOL);    /* set PFDAC = PFVOL */
        Data = Data & (UINT32)AK4951_R1DH_PFVOL_MASK;
        Data = Data | (UINT32)AK4951_R1DH_PFVOL_0DB;        /* PFVOL = 0dB */
#else
        /* Path from SDTI */
        Data = Data & (UINT32)AK4951_R1DH_PFDAC_MASK;
        /*Data = Data | ((UINT32)AK4951_R1DH_PFDAC_SDTI);*/     /* set PFDAC = SDTI(00) */
#endif
        (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_1D_D_FILTER_MODE, Data);

        if ((OutputMode & (AUCODEC_LINE_OUT | AUCODEC_SPEAKER_OUT)) != 0U) {
            if ((OutputMode & AUCODEC_LINE_OUT) != 0U) {
                AK4951_StereoLineOutPowerOn(pCodecCtrl);
            } else {
                AK4951_SpeakerOutPowerOn(pCodecCtrl);
            }
        } else {
            AK4951_SpeakerLineOutPowerOff(pCodecCtrl);
        }

        if ((OutputMode & AUCODEC_HEADPHONE_OUT) != 0U) {
            AK4951_StereoHeadphonePowerOn(pCodecCtrl);
        } else {
            AK4951_StereoHeadphonePowerOff(pCodecCtrl);
        }

        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_SetInput
 *
 *  @Description:: Audio Codec set input path
 *
 *  @Input      ::
 *      AMBA_AUDIO_CODEC_INPUT_MODE:   Audio Codec Input path
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AK4951_SetInput(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 InputMode)
{
    UINT32 Data = 0xFFU;
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_1D_D_FILTER_MODE, &Data);
#ifdef RECORDING_USING_PROGRAMABLE_FILTER
        Data = Data | ((UINT32)AK4951_R1DH_ADCPF);  /* ADCPF = 1 */
        Data = Data | ((UINT32)AK4951_R1DH_PFSDO);  /* PFSDO = 1 */
#else
        Data = Data & (~(UINT32)AK4951_R1DH_PFSDO); /* PFSDO = 0 */
#endif
        (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_1D_D_FILTER_MODE, Data);

        if (InputMode == AUCODEC_LINE_IN) {
            AK4951_LineInPowerOn(pCodecCtrl, INPUT_PORT3);
        } else if (InputMode == AUCODEC_DMIC_IN) {
            AK4951_MicInPowerOn(pCodecCtrl, SD_MIC, INPUT_PORT1);
        } else if (InputMode == AUCODEC_AMIC_IN) {
            AK4951_MicInPowerOn(pCodecCtrl, A_MIC, INPUT_PORT1);
        } else if (InputMode == AUCODEC_AMIC2_IN) {
            AK4951_MicInPowerOn(pCodecCtrl, A_MIC, INPUT_PORT2);
        } else {
            AK4951_MicInPowerOn(pCodecCtrl, SD_MIC, INPUT_PORT1);
        }

        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_SetMute
 *
 *  @Description:: Audio Codec Mute
 *
 *  @Input      ::
 *      MuteEnable:   1: Mute the DAC output , 0: Un-mute the DAC output
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AK4951_SetMute(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 MuteEnable)
{
    UINT32 Data = 0xFFU;
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        if (MuteEnable != 0U) {
            (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_07_MODE_CONTROL_3, &Data);
            Data = Data | ((UINT32)AK4951_R07H_SOFT_MUTE);
            (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_07_MODE_CONTROL_3, Data);
        } else {
            (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_07_MODE_CONTROL_3, &Data);
            Data = Data & (~((UINT32)AK4951_R07H_SOFT_MUTE));
            (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_07_MODE_CONTROL_3, Data);
        }
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_ModeConfig
 *
 *  @Description:: Audio Codec Data protocol configuration
 *
 *  @Input      ::
 *      Mode:   Audio Codec Data protocol
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AK4951_ModeConfig(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Mode)
{
    UINT32 Data = 0xFFU;
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {

        (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_05_MODE_CONTROL_1, &Data);
        Data = Data & ((UINT32)AK4951_R05H_DIF_MASK);
#if 0
        switch(Mode) {
        case AUCODEC_I2S:
        default:
            Data = Data | ((UINT32)AK4951_R05H_DIF_I2S);
            break;
        }
#else
        if (Mode == AUCODEC_I2S) {
            Data = Data | ((UINT32)AK4951_R05H_DIF_I2S); /* Only support I2S now */
        }
#endif
        (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_05_MODE_CONTROL_1, Data);

        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_FreqConfig
 *
 *  @Description:: Audio Codec sampling frequency configuration
 *
 *  @Input      ::
 *      Freq:   Audio Codec sampling frequency (Hz)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AK4951_FreqConfig(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Freq)
{
    UINT32 Data = 0xFFU;
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {

        (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_06_MODE_CONTROL_2, &Data);
        Data = Data & ((UINT32)AK4951_R06H_FS_MASK);

        if((CLOCK_SOURCE == AK4951_R01H_PMPLL_PLL) &&
           ((PLL_MODE == AK4951_R05H_PLL_BICK_32FS)||(PLL_MODE == AK4951_R05H_PLL_BICK_64FS))) {
            /* Clock: PLL BICK mode */
            switch (Freq) {
            default:
            case 48000 :
                Data = Data | ((UINT32)AK4951_R06H_FS_PLL_BICK_48KHZ);
                break;
            case 44100 :
                Data = Data | ((UINT32)AK4951_R06H_FS_PLL_BICK_44K1HZ);
                break;
            case 32000 :
                Data = Data | ((UINT32)AK4951_R06H_FS_PLL_BICK_32KHZ);
                break;
            case 24000 :
                Data = Data | ((UINT32)AK4951_R06H_FS_PLL_BICK_24KHZ);
                break;
            case 22050 :
                Data = Data | ((UINT32)AK4951_R06H_FS_PLL_BICK_22K05HZ);
                break;
            case 16000 :
                Data = Data | ((UINT32)AK4951_R06H_FS_PLL_BICK_16KHZ);
                break;
            case 12000 :
                Data = Data | ((UINT32)AK4951_R06H_FS_PLL_BICK_12KHZ);
                break;
            case 11025 :
                Data = Data | ((UINT32)AK4951_R06H_FS_PLL_BICK_11K025HZ);
                break;
            case 8000 :
                /*Data = Data | ((UINT32)AK4951_R06H_FS_PLL_BICK_8KHZ);*/
                break;
            }
        } else {
            /* Clock : External Slave/External Master/PLL MCKI mode */
            switch (Freq) {
            default:
            case 48000 :
                Data = Data | ((UINT32)AK4951_R06H_FS_48KHZ);
                break;
            case 44100 :
                Data = Data | ((UINT32)AK4951_R06H_FS_44K1HZ);
                break;
            case 32000 :
                Data = Data | ((UINT32)AK4951_R06H_FS_32KHZ);
                break;
            case 24000 :
                Data = Data | ((UINT32)AK4951_R06H_FS_24KHZ);
                break;
            case 22050 :
                Data = Data | ((UINT32)AK4951_R06H_FS_22K05HZ);
                break;
            case 16000 :
                Data = Data | ((UINT32)AK4951_R06H_FS_16KHZ);
                break;
            case 12000 :
                Data = Data | ((UINT32)AK4951_R06H_FS_12KHZ);
                break;
            case 11025 :
                Data = Data | ((UINT32)AK4951_R06H_FS_11K025HZ);
                break;
            case 8000 :
                /*Data = Data | ((UINT32)AK4951_R06H_FS_8KHZ);*/
                break;
            }
        }
        (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_06_MODE_CONTROL_2, Data);

        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AK4951_Init
 *
 *  @Description:: Audio Codec Initialization(Setup clock)
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AK4951_Init(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data = 0xFFU;
    UINT32 RetVal = AUCODEC_ERR_NONE;
    static char AK4951_MutexName[16] = "AK4951EN_Mutex";

    /*#define MS_MODE         AK4951_R01H_MS_SLAVE_MODE   */
    static UINT32 MS_MODE = AK4951_R01H_MS_SLAVE_MODE;// M/S: Clock Master/Slave mode
    //      - AK4951_R01H_MS_MASTER_MODE
    //      - AK4951_R01H_MS_SLAVE_MODE

    if (AmbaKAL_MutexCreate(&pCodecCtrl->Mutex, AK4951_MutexName) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {

        /* 1. PDN pin L -> H before Initial */

        /* 2. Dummy command */
        (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, 0x00);

        /* 3-1. DIF */
        (void)AK4951_ModeConfig(pCodecCtrl, AUDIO_INTERFACE);

        /* 3-2. PLL mode */
        (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_05_MODE_CONTROL_1, &Data);
        Data = Data & ((UINT32)AK4951_R05H_PLL_REF_CLK_MASK);
        Data = Data | (UINT32)PLL_MODE;
        (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_05_MODE_CONTROL_1, Data);

        /* 3-3. FS */
        (void)AK4951_FreqConfig(pCodecCtrl, FS_SETTING);

        /* 3-4. M/S bit */
        (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_01_POWER_MANAGEMENT2, &Data);
        Data = Data & ((UINT32)AK4951_R01H_MS_MASK);
        /*Data = Data | (UINT32)MS_MODE;*/
        (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_01_POWER_MANAGEMENT2, Data);

        /* 4. AK4951_vcom_pwr_up */
        (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, &Data);
        Data = Data | ((UINT32)AK4951_R00H_PMVCM_PWR_UP);
        (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_00_POWER_MANAGEMENT1, Data);

        /* 5.wait 2ms */
        (void)AmbaKAL_TaskSleep(2);

        /* 6. Power up PLL(if has) */
        if(CLOCK_SOURCE == AK4951_R01H_PMPLL_PLL) {
            (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_01_POWER_MANAGEMENT2, &Data);
            Data = Data & ((UINT32)AK4951_R01H_PMPLL_MASK);
            Data = Data | (UINT32)AK4951_R01H_PMPLL_PLL;
            (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_01_POWER_MANAGEMENT2, Data);
            /* 7.wait PLL lock */
            if(MS_MODE == AK4951_R01H_MS_MASTER_MODE) {
                (void)AmbaKAL_TaskSleep(5);
            } else {
                (void)AmbaKAL_TaskSleep(2);
            }
        }

        /* 8.DAC AOUT ALL MUTE  */
        (void)AK4951_Read(pCodecCtrl, (UINT32)AK4951_07_MODE_CONTROL_3, &Data);
        Data = Data |((UINT32)AK4951_R07H_SOFT_MUTE);
        (void)AK4951_Write(pCodecCtrl, (UINT32)AK4951_07_MODE_CONTROL_3, Data);
    }
    return RetVal;
}

AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_AK4951Obj = {
    .Init       = AK4951_Init,
    .ModeConfig = AK4951_ModeConfig,
    .FreqConfig = AK4951_FreqConfig,
    .SetOutput  = AK4951_SetOutput,
    .SetInput   = AK4951_SetInput,
    .SetMute    = AK4951_SetMute,
    .Write      = AK4951_Write_App,
    .Read       = AK4951_Read_App,
};
