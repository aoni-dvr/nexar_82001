/**
 *  @file AmbaAudio_NAU8822.c
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

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"

#include "AmbaI2C.h"
#include "AmbaAudio_NAU8822.h"

/* Register default values */
static UINT32 NAU8822_RegCache[NAU8822_REG_NUM] = {
    0x000U, 0x000U, 0x000U, 0x000U, 0x050U, 0x000U, 0x140U, 0x000U,
    0x000U, 0x000U, 0x000U, 0x0ffU, 0x0ffU, 0x000U, 0x100U, 0x0ffU,
    0x0ffU, 0x000U, 0x12cU, 0x02cU, 0x02cU, 0x02cU, 0x02cU, 0x000U,
    0x032U, 0x000U, 0x000U, 0x000U, 0x000U, 0x000U, 0x000U, 0x000U,
    0x038U, 0x00bU, 0x032U, 0x000U, 0x008U, 0x00cU, 0x093U, 0x0e9U,
    0x000U, 0x000U, 0x000U, 0x000U, 0x033U, 0x010U, 0x010U, 0x100U,
    0x100U, 0x002U, 0x001U, 0x001U, 0x039U, 0x039U, 0x039U, 0x039U,
    0x001U, 0x001U
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: NAU8822_Write
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
static UINT32 NAU8822_Write(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, UINT32 Data)
{
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[2];
    UINT32 TxSize;
    UINT32 RetVal = AUCODEC_ERR_NONE;
    UINT32 Temp;

    I2cConfig.SlaveAddr = pCodecCtrl->I2cCtrl.SlaveAddr;
    I2cConfig.DataSize = 2U;
    I2cConfig.pDataBuf = TxDataBuf;

    Temp = Data & 0x000001ffU;
    Temp = Temp >> 8U;

    TxDataBuf[0] = (UINT8)(((Addr << 1U) | Temp) & 0xffU);
    TxDataBuf[1] = (UINT8)(Data & 0xffU);

    if (Addr < (UINT32)NAU8822_REG_NUM) {
        RetVal = AmbaI2C_MasterWrite(pCodecCtrl->I2cCtrl.I2cChanNo, pCodecCtrl->I2cCtrl.I2cSpeed,
                                     &I2cConfig, &TxSize, AMBA_KAL_WAIT_FOREVER);
        NAU8822_RegCache[Addr] = Data;
    } else {
        RetVal = AUCODEC_ERR_ARG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: NAU8822_Read
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
static UINT32 NAU8822_Read(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, const UINT32 *pRxData)
{
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};

    UINT8 TxData;
    UINT8 Temp;
    UINT32 TxSize;
    UINT32 RetVal = AUCODEC_ERR_NONE;

    I2cTxConfig.SlaveAddr = pCodecCtrl->I2cCtrl.SlaveAddr;
    I2cTxConfig.DataSize = 1U;
    I2cTxConfig.pDataBuf = &TxData;

    TxData = (UINT8)((Addr << 1U) & 0xffU);

    I2cRxConfig.SlaveAddr = pCodecCtrl->I2cCtrl.SlaveAddr | 1U;
    I2cRxConfig.DataSize  = 2U;
    AmbaMisra_TypeCast(&(I2cRxConfig.pDataBuf), &pRxData);/*I2cRxConfig.pDataBuf  = (UINT8 *)pRxData;*/

//     if (Addr < NAU8822_REG_NUM) {
//         *pRxData = NAU8822_RegCache[Addr];
//     } else {
//         RetVal = (UINT32)AUCODEC_ERR_ARG;
//     }
    if (Addr < (UINT32)NAU8822_REG_NUM) {
        RetVal = AmbaI2C_MasterReadAfterWrite(pCodecCtrl->I2cCtrl.I2cChanNo, pCodecCtrl->I2cCtrl.I2cSpeed,
                                              1U, &I2cTxConfig, &I2cRxConfig, &TxSize, AMBA_KAL_WAIT_FOREVER);
        Temp = I2cRxConfig.pDataBuf[0];
        I2cRxConfig.pDataBuf[0] = I2cRxConfig.pDataBuf[1];
        I2cRxConfig.pDataBuf[1] = Temp;
    } else {
        RetVal = AUCODEC_ERR_ARG;
    }

    return RetVal;
}

static UINT32 NAU8822_Write_App(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, UINT32 Data)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        RetVal = NAU8822_Write(pCodecCtrl, Addr, Data);
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }
    return RetVal;
}

static UINT32 NAU8822_Read_App(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, const UINT32 *pRxData)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        RetVal = NAU8822_Read(pCodecCtrl, Addr, pRxData);
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }
    return RetVal;
}

static void NAU8822_HpLinePowerOn(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data = 0xFFU;

    (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_PWR_MANAGE2_REG, &Data);
    Data = Data | (UINT32)(NAU8822_R02H_LHPEN | NAU8822_R02H_RHPEN);
    (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_PWR_MANAGE2_REG, Data);
}

static void NAU8822_HpLinePowerDown(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data = 0xFFU;

    (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_PWR_MANAGE2_REG, &Data);
    Data = Data & (UINT32)((~(NAU8822_R02H_LHPEN)) & (~(NAU8822_R02H_RHPEN)));
    (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_PWR_MANAGE2_REG, Data);
}

static void NAU8822_SpeakerOutPowerOn(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data = 0xFFU;

    (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_PWR_MANAGE3_REG, &Data);
    Data = Data | (UINT32)(NAU8822_R03H_RSPKEN | NAU8822_R03H_LSPKEN);
    (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_PWR_MANAGE3_REG, Data);
}

static void NAU8822_SpeakerOutPowerOff(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data = 0xFFU;

    (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_PWR_MANAGE3_REG, &Data);
    Data = Data & (UINT32)((~(NAU8822_R03H_RSPKEN)) & (~(NAU8822_R03H_LSPKEN)));
    (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_PWR_MANAGE3_REG, Data);
}

static void NAU8822_LineInPowerOn(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data = 0xFFU;

    // Use LLIN/RLIN as input path
    // mute PGA input
    (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_L_IN_PGA_VOL_REG, &Data);
    Data = Data & ((UINT32)NAU8822_R2DH_PGA_MASK);
    Data = Data | ((UINT32)NAU8822_R2DH_PGAMT);
    Data = Data | ((UINT32)NAU8822_R2DH_PGA_0dB);
    (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_L_IN_PGA_VOL_REG, Data);

    (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_R_IN_PGA_VOL_REG, &Data);
    Data = Data & ((UINT32)NAU8822_R2DH_PGA_MASK);
    Data = Data | ((UINT32)NAU8822_R2DH_PGAMT);
    Data = Data | (UINT32)(NAU8822_R2DH_PGA_0dB | NAU8822_R2EH_PGA_UPDATE);
    (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_R_IN_PGA_VOL_REG, Data);

    // Disable the path between the MIC and PGA
    (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_INPUT_CTRL_REG, 0U);

    // open LLIN/RLIN boost path and set 0dB and disable other paths
    (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_L_ADC_BST_REG, 0x050U);
    (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_R_ADC_BST_REG, 0x050U);
}

//static void NAU8822_LineInPowerOff(void)
//{
//
//}

static void NAU8822_MicInPowerOn(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data = 0xFFU;

    (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_L_IN_PGA_VOL_REG, &Data);
    Data = Data & (UINT32)(NAU8822_R2DH_PGA_MASK & (~(NAU8822_R2DH_PGAMT)));
    Data = Data | ((UINT32)NAU8822_R2DH_PGA_MAX);
    (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_L_IN_PGA_VOL_REG, Data);

    (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_R_IN_PGA_VOL_REG, &Data);
    Data = Data & (UINT32)(NAU8822_R2DH_PGA_MASK & (~(NAU8822_R2DH_PGAMT)));
    Data = Data | (UINT32)(NAU8822_R2DH_PGA_MAX | NAU8822_R2EH_PGA_UPDATE);
    (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_R_IN_PGA_VOL_REG, Data);

    // Enable the path between the MIC and PGA
    (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_INPUT_CTRL_REG, 0x33U);

    // boost PGA path + 0dB and disable other paths
    (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_L_ADC_BST_REG, 0x0U);
    (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_R_ADC_BST_REG, 0x0U);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: NAU8822_SetOutput
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
static UINT32 NAU8822_SetOutput(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 OutputMode)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        if ((OutputMode & AUCODEC_LINE_OUT) != 0U) {
            NAU8822_SpeakerOutPowerOff(pCodecCtrl);
            NAU8822_HpLinePowerOn(pCodecCtrl);
        } else if ((OutputMode & AUCODEC_SPEAKER_OUT) != 0U) {
            NAU8822_HpLinePowerDown(pCodecCtrl);
            NAU8822_SpeakerOutPowerOn(pCodecCtrl);
        } else {
            RetVal = AUCODEC_ERR_ARG;
        }

        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: NAU8822_SetInput
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
static UINT32 NAU8822_SetInput(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 InputMode)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        if (InputMode == AUCODEC_LINE_IN) {
            NAU8822_LineInPowerOn(pCodecCtrl);
        } else if(InputMode == AUCODEC_AMIC_IN) {
            NAU8822_MicInPowerOn(pCodecCtrl);
        } else {
            RetVal = AUCODEC_ERR_ARG;
        }
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: NAU8822_SetMute
 *
 *  @Description:: Audio Codec Mute
 *
 *  @Input      ::
 *      MuteEnable:   1: Mute the Codec, 0: Un-mute the codec
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 NAU8822_SetMute(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 MuteEnable)
{
    UINT32 Data = 0xFFU;
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        if (MuteEnable != 0U) {
            (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_DAC_CTRL_REG, &Data);
            Data = Data | ((UINT32)NAU8822_R0AH_SOFTMT);
            (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_DAC_CTRL_REG, Data);
        } else {
            (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_DAC_CTRL_REG, &Data);
            Data = Data & (~((UINT32)NAU8822_R0AH_SOFTMT));
            (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_DAC_CTRL_REG, Data);
        }
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: NAU8822_ModeConfig
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
static UINT32 NAU8822_ModeConfig(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Mode)
{
    UINT32 Data = 0xFFU;
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {

        (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_AU_INTERFACE_CTRL_REG, &Data);
        Data = Data & ((UINT32)NAU8822_R04H_WLEN_MASK);

        if (Mode == AUCODEC_I2S) {
            Data = Data | ((UINT32)NAU8822_R04H_WLEN_24B); /* Only support I2S now */
        }

        (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_AU_INTERFACE_CTRL_REG, Data);

        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: NAU8822_FreqConfig
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
static UINT32 NAU8822_FreqConfig(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Freq)
{
    UINT32 Data = 0xFFU;
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {

        (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_CLK_CTRL2_REG, &Data);
        Data = Data & ((UINT32)NAU8822_R07H_SMPLR_MASK);

        switch (Freq) {
        default:
        case 48000 :
        case 44100 :
            //Data = Data | ((UINT32)NAU8822_R07H_SMPLR_48K);
            break;
        case 32000 :
            Data = Data | ((UINT32)NAU8822_R07H_SMPLR_32K);
            break;
        case 24000 :
            Data = Data | ((UINT32)NAU8822_R07H_SMPLR_24K);
            break;
        case 22050 :
            Data = Data | ((UINT32)NAU8822_R07H_SMPLR_24K);
            break;
        case 16000 :
            Data = Data | ((UINT32)NAU8822_R07H_SMPLR_16K);
            break;
        case 12000 :
            Data = Data | ((UINT32)NAU8822_R07H_SMPLR_12K);
            break;
        case 11025 :
            Data = Data | ((UINT32)NAU8822_R07H_SMPLR_12K);
            break;
        case 8000 :
            Data = Data | ((UINT32)NAU8822_R07H_SMPLR_8K);
            break;
        }

        (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_CLK_CTRL2_REG, Data);

        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: NAU8822_Init
 *
 *  @Description:: Audio Codec Initialization
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 NAU8822_Init(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data = 0xFFU;
    UINT32 Loop;
    UINT32 RetVal = AUCODEC_ERR_NONE;
    static char NAU8822_MutexName[15] = "NAU8822_Mutex";

    if (AmbaKAL_MutexCreate(&pCodecCtrl->Mutex, NAU8822_MutexName) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        /* Software Reset */
        (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_SW_RESET_REG, 0U);
        (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_SW_RESET_REG, 0U);
        (void)NAU8822_Write(pCodecCtrl, (UINT32)NAU8822_SW_RESET_REG, 0U);

        /* Write default value to the instance  */
        for (Loop = 0; Loop < NAU8822_REG_NUM; Loop++) {
            (void)NAU8822_Write(pCodecCtrl, Loop, NAU8822_RegCache[Loop]);
        }

        /* Invert RSPKOUT output for speaker */
        (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_RSPK_SUBMIX_REG, &Data);
        Data = Data | ((UINT32)NAU8822_R2BH_RSUBBYP);
        (void)NAU8822_Write(pCodecCtrl, NAU8822_RSPK_SUBMIX_REG, Data);

        /* Clock control, set codec as slave mode */
        (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_CLK_CTRL1_REG, &Data);
        //Data = Data & ((UINT32)NAU8822_R06H_CODEC_SLAVE);
        (void)NAU8822_Write(pCodecCtrl, NAU8822_CLK_CTRL1_REG, 0U/*Data*/);

        /* Set IOBUFEN = 1 and REFIMP = 75kohm */
        (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_PWR_MANAGE1_REG, &Data);
        Data = Data & ((UINT32)NAU8822_R01H_REFIMP_MASK);
        Data = Data | (UINT32)((NAU8822_R01H_REFIMP_75K | NAU8822_R01H_ABIASEN) | NAU8822_R01H_IOBUFEN);
        (void)NAU8822_Write(pCodecCtrl, NAU8822_PWR_MANAGE1_REG, Data);

        /* input boost enable*/
        (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_PWR_MANAGE2_REG, &Data);
        Data = Data | (UINT32)(NAU8822_R02H_LBSTEN | NAU8822_R02H_RBSTEN);
        (void)NAU8822_Write(pCodecCtrl, NAU8822_PWR_MANAGE2_REG, Data);

        /* ADC power up */
        /* L/R input PGA enable */
        (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_PWR_MANAGE2_REG, &Data);
        Data = Data | (UINT32)(NAU8822_R02H_LPGAEN | NAU8822_R02H_RPGAEN);
        /* ADC enable */
        Data = Data | (UINT32)(NAU8822_R02H_LADCEN | NAU8822_R02H_RADCEN);
        (void)NAU8822_Write(pCodecCtrl, NAU8822_PWR_MANAGE2_REG, Data);

        /* Mic Bias enable */
        (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_PWR_MANAGE1_REG, &Data);
        Data = Data | ((UINT32)NAU8822_R01H_MICBIASEN);
        (void)NAU8822_Write(pCodecCtrl, NAU8822_PWR_MANAGE1_REG, Data);

        /* DAC power up */
        /* L/R output mixer enable */
        (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_PWR_MANAGE3_REG, &Data);
        Data = Data | (UINT32)(NAU8822_R03H_LMIXEN | NAU8822_R03H_RMIXEN);
        /* DAC enable */
        Data = Data | ((UINT32)NAU8822_R03H_LDACEN | (UINT32)NAU8822_R03H_RDACEN);
        (void)NAU8822_Write(pCodecCtrl, NAU8822_PWR_MANAGE3_REG, Data);

        /* DAC OSR 128x */
        (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_DAC_CTRL_REG, &Data);
        Data = Data | ((UINT32)NAU8822_R0AH_DACOS);
        (void)NAU8822_Write(pCodecCtrl, NAU8822_DAC_CTRL_REG, Data);

        /* ADC OSR 128x and high pass filter enable*/
        (void)NAU8822_Read(pCodecCtrl, (UINT32)NAU8822_ADC_CTRL_REG, &Data);
        Data = Data | (UINT32)(NAU8822_R0EH_ADCOS | NAU8822_R0EH_HPFEN);
        (void)NAU8822_Write(pCodecCtrl, NAU8822_ADC_CTRL_REG, Data);
    }

    return RetVal;
}

AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_NAU8822Obj = {
    .Init       = NAU8822_Init,
    .ModeConfig = NAU8822_ModeConfig,
    .FreqConfig = NAU8822_FreqConfig,
    .SetOutput  = NAU8822_SetOutput,
    .SetInput   = NAU8822_SetInput,
    .SetMute    = NAU8822_SetMute,
    .Write      = NAU8822_Write_App,
    .Read       = NAU8822_Read_App,
};
