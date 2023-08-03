/**
 *  @file AmbaAudio_TLV320AIC3007.c
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

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"

#include "AmbaI2C.h"
#include "AmbaAudio_TLV320AIC3007.h"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Tlv320Aic3007_Write
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
static UINT32 Tlv320Aic3007_Write(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, UINT32 Data)
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

    if (Addr < TLV320AIC3007_REG_NUM) {
        RetVal = AmbaI2C_MasterWrite(pCodecCtrl->I2cCtrl.I2cChanNo, pCodecCtrl->I2cCtrl.I2cSpeed,
                                     &I2cConfig, &TxSize, AMBA_KAL_WAIT_FOREVER);
    } else {
        RetVal = AUCODEC_ERR_ARG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Tlv320Aic3007_Read
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
static UINT32 Tlv320Aic3007_Read(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, const UINT32 *pRxData)
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

    if (Addr < TLV320AIC3007_REG_NUM) {
        RetVal = AmbaI2C_MasterReadAfterWrite(pCodecCtrl->I2cCtrl.I2cChanNo, pCodecCtrl->I2cCtrl.I2cSpeed,
                                              1U, &I2cTxConfig, &I2cRxConfig, &TxSize, AMBA_KAL_WAIT_FOREVER);
    } else {
        RetVal = AUCODEC_ERR_ARG;
    }
    return RetVal;
}

static UINT32 Tlv320Aic3007_Write_App(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, UINT32 Data)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        RetVal = Tlv320Aic3007_Write(pCodecCtrl, Addr, Data);
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }
    return RetVal;
}

static UINT32 Tlv320Aic3007_Read_App(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, const UINT32 *pRxData)
{

    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        RetVal = Tlv320Aic3007_Read(pCodecCtrl, Addr, pRxData);
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }
    return RetVal;
}

static void tlv320aic3007_DAC_set(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data = 0xFFUL;

    /* Set DAC input */
    (void)Tlv320Aic3007_Read(pCodecCtrl, 7U, &Data);
    Data &= ~0x1eUL;
    Data |= 0x0aUL;// left(right) channel
    (void)Tlv320Aic3007_Write(pCodecCtrl, 7U, Data);

    /* DAC power up */
    Data = 0xc0UL;// Left/Right DAC powered on
    (void)Tlv320Aic3007_Write(pCodecCtrl, 37U, Data);

    /* Output Soft-stepping disable */
    Data = 0x02UL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 40U, Data);

    /* Driver power on time =400ms, driver ramp up =1ms */
    Data = 0x84UL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 42U, Data);

    /* Left & Right DAC Volume control */
    Data = 0x00UL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 43U, Data);
    (void)Tlv320Aic3007_Write(pCodecCtrl, 44U, Data);
}

/**
 * Audio Codec Initialization
 */
static UINT32 Tlv320Aic3007_Init(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data;
    UINT32 RetVal = AUCODEC_ERR_NONE;
    static char TLV320AIC3007_MutexName[20] = "TLV320AIC3007_Mutex";

    if (AmbaKAL_MutexCreate(&pCodecCtrl->Mutex, TLV320AIC3007_MutexName) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        /* Set Page 0 */
        Data= 0UL;
        (void)Tlv320Aic3007_Write(pCodecCtrl, 0U, Data);
        /* Software reset */
        Data=  0x80UL;
        (void)Tlv320Aic3007_Write(pCodecCtrl, 1U, Data);

        Data= 0UL;
        (void)Tlv320Aic3007_Write(pCodecCtrl, 95U, Data);
        (void)AmbaKAL_TaskSleep(5U);

        tlv320aic3007_DAC_set(pCodecCtrl);

        /* Set IIS bit clock rate */
        Data= 0xFFUL;
        (void)Tlv320Aic3007_Read(pCodecCtrl, 9U, &Data);
        Data &= ~0x30UL;
        Data |= 0x08UL;
        (void)Tlv320Aic3007_Write(pCodecCtrl, 9U, Data);
    }

    return RetVal;
}

/**
 * Change operating frequency
 */
static UINT32 Tlv320Aic3007_FreqConfig(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Freq)
{
    UINT32 Data;
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {

        Data=0UL;
        (void)Tlv320Aic3007_Write(pCodecCtrl, 0U, Data);
        Data= 0xFFUL;
        (void)Tlv320Aic3007_Read(pCodecCtrl, 7U, &Data);
        switch (Freq) {
        case 48000U :
        case 96000U :
            Data &= ~((UINT16)0x80UL);
            break;
        case 44100 :
            Data |= ((UINT16)0x80UL);
            break;
        default:
            RetVal = AUCODEC_ERR_ARG;
            break;
        }
        (void)Tlv320Aic3007_Write(pCodecCtrl, 7U, Data);
        Data =0x00UL;
        (void)Tlv320Aic3007_Write(pCodecCtrl, 2U, Data);
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }

    return RetVal;
}

/**
 * Mode configuration
 */
static UINT32 Tlv320Aic3007_ModeConfig(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Mode)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        if (Mode != AUCODEC_I2S) {
            RetVal = AUCODEC_ERR_ARG;
        }
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Tlv320Aic3007_SetMute
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
static UINT32 Tlv320Aic3007_SetMute(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 MuteEnable)
{
    UINT32 Data = 0xFFUL;
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {

        if (MuteEnable != 0U) {
            /* DAC Power & Output Driver control */
            (void)Tlv320Aic3007_Read(pCodecCtrl, 37U, &Data);
            Data = Data & 0x3FUL;
            (void)Tlv320Aic3007_Write(pCodecCtrl, 37U, Data);//Left & Right DAC not powered up

            /* DAC output muted */
            (void)Tlv320Aic3007_Read(pCodecCtrl, 43U, &Data);
            Data = Data |0x80UL;
            (void)Tlv320Aic3007_Write(pCodecCtrl, 43U,Data); //Left DAC Digital Volume :bit8=muted control

            (void)Tlv320Aic3007_Read(pCodecCtrl, 44U, &Data);
            Data = Data | 0x80UL;
            (void)Tlv320Aic3007_Write(pCodecCtrl, 44U,Data);//Right DAC Digital Volume :bit8=muted control
        } else {
            /* DAC Power & Output Driver control */
            (void)Tlv320Aic3007_Read(pCodecCtrl, 37U, &Data);
            Data = Data |0xC0UL;
            (void)Tlv320Aic3007_Write(pCodecCtrl, 37U, Data);

            /* HP output mute off */
            (void)Tlv320Aic3007_Read(pCodecCtrl, 43U, &Data);
            Data = Data & ~(0x80UL);
            (void)Tlv320Aic3007_Write(pCodecCtrl, 43U,Data);

            (void)Tlv320Aic3007_Read(pCodecCtrl, 44U, &Data);
            Data = Data & ~(0x80UL);
            (void)Tlv320Aic3007_Write(pCodecCtrl, 44U,Data);
        }
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }

    return RetVal;
}

/**
 * Turn On Recording Path
 */
static void Tlv320Aic3007_StereoInputOn(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data ;

    /* PGA Gain  */

    Data = 0x00UL; //
    (void)Tlv320Aic3007_Write(pCodecCtrl, 15U, Data);
    (void)Tlv320Aic3007_Write(pCodecCtrl, 16U, Data);

    Data = 0x07UL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 19U, Data);

    (void)Tlv320Aic3007_Write(pCodecCtrl, 22U, Data);
}

/**
 * Turn Off Recording Path
 */
static void Tlv320Aic3007_StereoInputOff(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data ;

    Data = 0x00UL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 19U, Data);

    (void)Tlv320Aic3007_Write(pCodecCtrl, 22U, Data);
}

/**
 * Turn On Recording Path
 */
static void Tlv320Aic3007_MicInputOn(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data ;


    /* ADC power up */

    Data = 0x7CUL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 19U, Data);


    Data = 0x7CUL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 22U, Data);


    /* Left & Right AGC */

    Data = 0xB0UL;// AGC enable & AGC target gain=-5dB
    (void)Tlv320Aic3007_Write(pCodecCtrl, 26U, Data);
    (void)Tlv320Aic3007_Write(pCodecCtrl, 29U, Data);


    /*AGC gain setting*/
    Data = 0xA0UL;    // (0xB4 >> 1 = 90) * 0.5dB = 45dB. (AGC Max. gain)
    (void)Tlv320Aic3007_Write(pCodecCtrl, 27U, Data);
    (void)Tlv320Aic3007_Write(pCodecCtrl, 30U, Data);

    /*AGC Noise threshold control*/
    Data = 0xFFUL;    //
    (void)Tlv320Aic3007_Write(pCodecCtrl, 28U, Data);
    (void)Tlv320Aic3007_Write(pCodecCtrl, 31U, Data);

    /*AGC  Noise gate debounce*/
    Data = 0x39UL;    //
    (void)Tlv320Aic3007_Write(pCodecCtrl, 34U, Data);
    (void)Tlv320Aic3007_Write(pCodecCtrl, 35U, Data);



    /* MICBIAS Control */

    Data  = 0x40UL;// MICBIAS output is powered to 2V
    (void)Tlv320Aic3007_Write(pCodecCtrl, 25U, Data);

    /* Audio Codec Digital Filter

    Data = 0x50;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 12, Data);*/

    /*   High power outputs for AC-coupled driver config
    Data = 0x08;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 14, Data)*/;
    /* PGA Gain

    Data = 0x28;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 15, Data);
    (void)Tlv320Aic3007_Write(pCodecCtrl, 16, Data);*/

    Data = 0x0FUL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 17U, Data);
    Data = 0xF0UL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 18U, Data);
}

/**
 * Turn Off Recording Path
 */
static void Tlv320Aic3007_MicInputOff(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data ;

    /* ADC power down */
    Data = 0x00UL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 19U, Data);


    Data = 0x00UL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 22U, Data);

    Data  = 0x00;// MICBIAS output is powered down
    (void)Tlv320Aic3007_Write(pCodecCtrl, 25U, Data);

    Data = 0x00UL;// AGC disenable
    (void)Tlv320Aic3007_Write(pCodecCtrl, 26U, Data);
    (void)Tlv320Aic3007_Write(pCodecCtrl, 29U, Data);


    Data = 0xFFUL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 17U, Data);
    Data = 0xFFUL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 18U, Data);
}

/**
 * Turn on Playback Path
 */
static void Tlv320Aic3007_LineOutPowerOn(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data;

    /* DAC L1 to LEFT_LOP/M 0dB*/
    Data = 0x80UL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 82U, Data);

    /* DAC R1 to RIGHT_LOP/M 0dB */
    Data = 0x80UL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 92U, Data);

    /* LEFT_LOP/M power up unmute 0dB */
    Data = 0x0BUL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 86U, Data);

    /* RIGHT_LOP power up unmute 0dB */
    Data = 0x0BUL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 93U, Data);
}

/**
 * Turn Off Playback Path
 */
static void Tlv320Aic3007_LineOutPowerOff(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data ;

    /* DAC L1 to LEFT_LOP/M 0dB*/
    Data = 0x00UL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 82U, Data);

    /* DAC R1 to RIGHT_LOP/M 0dB */
    Data = 0x00UL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 92U, Data);
}

/**
 * Turn on Playback Path
 */
static void Tlv320Aic3007_SpeakerOutPowerOn(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data ;

    Data =  0x0CUL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 73U, Data);

#if 0
    /*HPROUT Output Level control*/
    Data = (void)Tlv320Aic3007_Read(pCodecCtrl, 65) & 0xF6; /*bit[3] & bit[0] : Muted & HPROUT  power off.*/
    (void)Tlv320Aic3007_Write(pCodecCtrl, 65,Data);
#endif

    /* LEFT_LOP/M Output level control */
    Data = 0x0BUL;//bit[3] & bit[0]=1 & 1 : Not muted & powered up
    (void)Tlv320Aic3007_Write(pCodecCtrl, 86U, Data);
}

/**
 * Turn Off Playback Path
 */
static void Tlv320Aic3007_SpeakerOutPowerOff(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 Data;

    Data =  0x00UL;
    (void)Tlv320Aic3007_Write(pCodecCtrl, 73U, Data);

#if 0
    /*HPROUT Output Level control*/
    Data = (void)Tlv320Aic3007_Read(pCodecCtrl, 65) & 0xF6;/*bit[3] & bit[0] : Muted & HPROUT  power off.*/
    (void)Tlv320Aic3007_Write(pCodecCtrl, 65,Data);
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Tlv320Aic3007_SetOutput
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
static UINT32 Tlv320Aic3007_SetOutput(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 OutputMode)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        if ((OutputMode & AUCODEC_LINE_OUT) != 0U) {
            Tlv320Aic3007_SpeakerOutPowerOff(pCodecCtrl);
            Tlv320Aic3007_LineOutPowerOn(pCodecCtrl);
        } else if ((OutputMode & AUCODEC_SPEAKER_OUT) != 0U) {
            Tlv320Aic3007_LineOutPowerOff(pCodecCtrl);
            Tlv320Aic3007_SpeakerOutPowerOn(pCodecCtrl);
        } else {
            RetVal = AUCODEC_ERR_ARG;
        }

        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Tlv320Aic3007_SetInput
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
static UINT32 Tlv320Aic3007_SetInput(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 InputMode)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        if (InputMode == AUCODEC_LINE_IN) {
            Tlv320Aic3007_MicInputOff(pCodecCtrl);
            Tlv320Aic3007_StereoInputOn(pCodecCtrl);
        } else if(InputMode == AUCODEC_AMIC_IN) {
            Tlv320Aic3007_StereoInputOff(pCodecCtrl);
            Tlv320Aic3007_MicInputOn(pCodecCtrl);
        } else {
            RetVal = AUCODEC_ERR_ARG;
        }
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }

    return RetVal;
}

AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_Tlv320Aic3007Obj = {
    .Init       = Tlv320Aic3007_Init,
    .ModeConfig = Tlv320Aic3007_ModeConfig,
    .FreqConfig = Tlv320Aic3007_FreqConfig,
    .SetOutput  = Tlv320Aic3007_SetOutput,
    .SetInput   = Tlv320Aic3007_SetInput,
    .SetMute    = Tlv320Aic3007_SetMute,
    .Write      = Tlv320Aic3007_Write_App,
    .Read       = Tlv320Aic3007_Read_App,
};
