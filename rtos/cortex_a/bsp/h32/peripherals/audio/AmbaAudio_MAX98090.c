#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"

#include "AmbaI2C.h"
#include "AmbaAudio_MAX98090.h"

static UINT32 Max98090_Write(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, UINT32 Data)
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

	RetVal = AmbaI2C_MasterWrite(pCodecCtrl->I2cCtrl.I2cChanNo, pCodecCtrl->I2cCtrl.I2cSpeed,
								 &I2cConfig, &TxSize, AMBA_KAL_WAIT_FOREVER);

    return RetVal;
}

static UINT32 Max98090_Read(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, const UINT32 *pRxData)
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

	RetVal = AmbaI2C_MasterReadAfterWrite(pCodecCtrl->I2cCtrl.I2cChanNo, pCodecCtrl->I2cCtrl.I2cSpeed,
										  1U, &I2cTxConfig, &I2cRxConfig, &TxSize, AMBA_KAL_WAIT_FOREVER);
    return RetVal;
}

static UINT32 Max98090_Write_App(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, UINT32 Data)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        RetVal = Max98090_Write(pCodecCtrl, Addr, Data);
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }
    return RetVal;
}

static UINT32 Max98090_Read_App(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, const UINT32 *pRxData)
{

    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        RetVal = Max98090_Read(pCodecCtrl, Addr, pRxData);
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }
    return RetVal;
}

static UINT32 Max98090_SetOutput(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 OutputMode)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    return RetVal;
}

static UINT32 Max98090_SetInput(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 InputMode)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    return RetVal;
}

static UINT32 Max98090_SetMute(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 MuteEnable)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    return RetVal;
}

static UINT32 Max98090_ModeConfig(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Mode)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    return RetVal;
}

static UINT32 Max98090_FreqConfig(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Freq)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    return RetVal;
}

static UINT32 Max98090_Init(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;
    static char Max98090_MutexName[16] = "Max98090_Mutex";

    if (AmbaKAL_MutexCreate(&pCodecCtrl->Mutex, Max98090_MutexName) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        /* 1. PDN pin L -> H before Initial */
        extern UINT32 AmbaUserGPIO_AudioResetCtrl(UINT32 FpdFlag) __attribute__((weak));
        if (AmbaUserGPIO_AudioResetCtrl) {
            AmbaUserGPIO_AudioResetCtrl(0U);
            AmbaKAL_TaskSleep(1U);
            AmbaUserGPIO_AudioResetCtrl(1U);
            AmbaKAL_TaskSleep(1U);
        }
        Max98090_Write(pCodecCtrl, 0x00, 0x00);
        AmbaKAL_TaskSleep(10U);
        Max98090_Write(pCodecCtrl, 0x01, 0x00);
        Max98090_Write(pCodecCtrl, 0x02, 0x00);
        Max98090_Write(pCodecCtrl, 0x03, 0x00);
        Max98090_Write(pCodecCtrl, 0x04, 0x00);
        Max98090_Write(pCodecCtrl, 0x05, 0x08);
        Max98090_Write(pCodecCtrl, 0x06, 0x00);
        Max98090_Write(pCodecCtrl, 0x07, 0x00);
        Max98090_Write(pCodecCtrl, 0x08, 0x00);
        Max98090_Write(pCodecCtrl, 0x09, 0x00);
        Max98090_Write(pCodecCtrl, 0x0A, 0x00);
        Max98090_Write(pCodecCtrl, 0x0B, 0x00);
        Max98090_Write(pCodecCtrl, 0x0C, 0x00);
        Max98090_Write(pCodecCtrl, 0x0D, 0x00);
        Max98090_Write(pCodecCtrl, 0x0E, 0x00);
        Max98090_Write(pCodecCtrl, 0x0F, 0x00);
        Max98090_Write(pCodecCtrl, 0x10, 0x0A);
        Max98090_Write(pCodecCtrl, 0x11, 0x00);
        Max98090_Write(pCodecCtrl, 0x12, 0x01);
        Max98090_Write(pCodecCtrl, 0x13, 0x43);
        Max98090_Write(pCodecCtrl, 0x14, 0x00);
        Max98090_Write(pCodecCtrl, 0x15, 0x00);
        Max98090_Write(pCodecCtrl, 0x16, 0x00);
        Max98090_Write(pCodecCtrl, 0x17, 0x23);
        Max98090_Write(pCodecCtrl, 0x18, 0x23);
        Max98090_Write(pCodecCtrl, 0x19, 0x00);
        Max98090_Write(pCodecCtrl, 0x1A, 0x00);
        Max98090_Write(pCodecCtrl, 0x1B, 0x10);
        Max98090_Write(pCodecCtrl, 0x1C, 0x00);
        Max98090_Write(pCodecCtrl, 0x1D, 0x60);
        Max98090_Write(pCodecCtrl, 0x1E, 0x00);
        Max98090_Write(pCodecCtrl, 0x1F, 0x00);
        Max98090_Write(pCodecCtrl, 0x20, 0x00);
        Max98090_Write(pCodecCtrl, 0x21, 0x03);
        Max98090_Write(pCodecCtrl, 0x22, 0x04);
        Max98090_Write(pCodecCtrl, 0x23, 0x00);
        Max98090_Write(pCodecCtrl, 0x24, 0x00);
        Max98090_Write(pCodecCtrl, 0x25, 0x03);
        Max98090_Write(pCodecCtrl, 0x26, 0x40);
        Max98090_Write(pCodecCtrl, 0x27, 0x00);
        Max98090_Write(pCodecCtrl, 0x28, 0x00);
        Max98090_Write(pCodecCtrl, 0x29, 0x00);
        Max98090_Write(pCodecCtrl, 0x2A, 0x00);
        Max98090_Write(pCodecCtrl, 0x2B, 0x30);
        Max98090_Write(pCodecCtrl, 0x2C, 0x07);
        Max98090_Write(pCodecCtrl, 0x2D, 0x07);
        Max98090_Write(pCodecCtrl, 0x2E, 0x03);
        Max98090_Write(pCodecCtrl, 0x2F, 0x00);
        Max98090_Write(pCodecCtrl, 0x30, 0x00);
        Max98090_Write(pCodecCtrl, 0x31, 0x26);
        Max98090_Write(pCodecCtrl, 0x32, 0x26);
        Max98090_Write(pCodecCtrl, 0x33, 0x00);
        Max98090_Write(pCodecCtrl, 0x34, 0x00);
        Max98090_Write(pCodecCtrl, 0x35, 0x00);
        Max98090_Write(pCodecCtrl, 0x36, 0x00);
        Max98090_Write(pCodecCtrl, 0x37, 0x00);
        Max98090_Write(pCodecCtrl, 0x38, 0x00);
        Max98090_Write(pCodecCtrl, 0x39, 0x00);
        Max98090_Write(pCodecCtrl, 0x3A, 0x00);
        Max98090_Write(pCodecCtrl, 0x3B, 0x00);
        Max98090_Write(pCodecCtrl, 0x3C, 0x00);
        Max98090_Write(pCodecCtrl, 0x3D, 0x00);
        Max98090_Write(pCodecCtrl, 0x3E, 0x11);
        Max98090_Write(pCodecCtrl, 0x3F, 0x13);
        Max98090_Write(pCodecCtrl, 0x40, 0x00);
        Max98090_Write(pCodecCtrl, 0x41, 0x08);
        Max98090_Write(pCodecCtrl, 0x42, 0x00);
        Max98090_Write(pCodecCtrl, 0x43, 0x00);
        Max98090_Write(pCodecCtrl, 0x44, 0x00);
        Max98090_Write(pCodecCtrl, 0x45, 0x80);
        Max98090_Write(pCodecCtrl, 0xAF, 0x02);
        Max98090_Write(pCodecCtrl, 0xB0, 0x77);
        Max98090_Write(pCodecCtrl, 0xB1, 0xD5);
        Max98090_Write(pCodecCtrl, 0xB2, 0x00);
        Max98090_Write(pCodecCtrl, 0xB3, 0x00);
        Max98090_Write(pCodecCtrl, 0xB4, 0x00);
        Max98090_Write(pCodecCtrl, 0xB5, 0xFD);
        Max98090_Write(pCodecCtrl, 0xB6, 0x88);
        Max98090_Write(pCodecCtrl, 0xB7, 0x2B);
        Max98090_Write(pCodecCtrl, 0xB8, 0xE5);
        Max98090_Write(pCodecCtrl, 0xB9, 0x0C);
        Max98090_Write(pCodecCtrl, 0xBA, 0x8C);
        Max98090_Write(pCodecCtrl, 0xBB, 0x0B);
        Max98090_Write(pCodecCtrl, 0xBC, 0x10);
        Max98090_Write(pCodecCtrl, 0xBD, 0x54);

    }
    return RetVal;
}

static UINT32 Max98090_SetVolume(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Level)
{
    UINT32 data = 0;

    if (Level > 3) {
        Level = 3;
    }
#define VOLUME_OFF (0x80)
#define VOLUME_30  (0x21)
#define VOLUME_60  (0x26)
#define VOLUME_80  (0x39)

    switch(Level) {
    case 0://off
        data = VOLUME_OFF;
        break;
    case 1://low
        data = VOLUME_30;
        break;
    case 2://mid
        data = VOLUME_60;
        break;
    case 3://high
        data = VOLUME_80;
        break;
    default:
        break;
    }
    Max98090_Write(pCodecCtrl, 0x31, data);
    Max98090_Write(pCodecCtrl, 0x32, data);

    return 0;
}

AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_MAX98090Obj = {
    .Init       = Max98090_Init,
    .ModeConfig = Max98090_ModeConfig,
    .FreqConfig = Max98090_FreqConfig,
    .SetOutput  = Max98090_SetOutput,
    .SetInput   = Max98090_SetInput,
    .SetMute    = Max98090_SetMute,
    .Write      = Max98090_Write_App,
    .Read       = Max98090_Read_App,
    .SetVolume  = Max98090_SetVolume,
};
