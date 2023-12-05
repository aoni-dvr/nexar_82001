#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"

#include "AmbaI2C.h"
#include "AmbaAudio_CL1009.h"

static UINT32 CL1009_Write(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, UINT32 Data)
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

static UINT32 CL1009_Read(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, const UINT32 *pRxData)
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

static UINT32 CL1009_Write_App(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, UINT32 Data)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        RetVal = CL1009_Write(pCodecCtrl, Addr, Data);
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }
    return RetVal;
}

static UINT32 CL1009_Read_App(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, const UINT32 *pRxData)
{

    UINT32 RetVal = AUCODEC_ERR_NONE;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        RetVal = CL1009_Read(pCodecCtrl, Addr, pRxData);
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }
    return RetVal;
}

static UINT32 CL1009_SetOutput(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 OutputMode)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    return RetVal;
}

static UINT32 CL1009_SetInput(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 InputMode)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    return RetVal;
}

static UINT32 CL1009_SetMute(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 MuteEnable)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    return RetVal;
}

static UINT32 CL1009_ModeConfig(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Mode)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    return RetVal;
}

static UINT32 CL1009_FreqConfig(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Freq)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    return RetVal;
}

static UINT32 CL1009_Init(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;
    static char CL1009_MutexName[16] = "CL1009_Mutex";

    if (AmbaKAL_MutexCreate(&pCodecCtrl->Mutex, CL1009_MutexName) != KAL_ERR_NONE) {
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
        CL1009_Write(pCodecCtrl, 0x62, 0x95);
        CL1009_Write(pCodecCtrl, 0x00, 0x88);
        CL1009_Write(pCodecCtrl, 0x01, 0xa8);
        CL1009_Write(pCodecCtrl, 0x05, 0x07);
        CL1009_Write(pCodecCtrl, 0x02, 0x13);
        CL1009_Write(pCodecCtrl, 0x21, 0x00);
        CL1009_Write(pCodecCtrl, 0x22, 0x00);
        CL1009_Write(pCodecCtrl, 0x23, 0x00);
        CL1009_Write(pCodecCtrl, 0x24, 0x00);
        CL1009_Write(pCodecCtrl, 0x03, 0x20);
        CL1009_Write(pCodecCtrl, 0x06, 0x04);
        CL1009_Write(pCodecCtrl, 0x39, 0x14);
        CL1009_Write(pCodecCtrl, 0x1b, 0x03);
        CL1009_Write(pCodecCtrl, 0x1e, 0x38);
        CL1009_Write(pCodecCtrl, 0x30, 0x20);
        CL1009_Write(pCodecCtrl, 0x2c, 0x91);
        CL1009_Write(pCodecCtrl, 0x46, 0x90);
        CL1009_Write(pCodecCtrl, 0x45, 0x18);
        CL1009_Write(pCodecCtrl, 0x3b, 0x9a);
        AmbaKAL_TaskSleep(10U);


        CL1009_Write(pCodecCtrl, 0x00 ,0x88);
        //EQ Band 1
        CL1009_Write(pCodecCtrl, 0x54 ,0x36);
        CL1009_Write(pCodecCtrl, 0x55 ,0x24);
        CL1009_Write(pCodecCtrl, 0x56 ,0xe1);
        CL1009_Write(pCodecCtrl, 0x57 ,0xe8);
        CL1009_Write(pCodecCtrl, 0x58 ,0x87);
        CL1009_Write(pCodecCtrl, 0x59 ,0x5c);
        CL1009_Write(pCodecCtrl, 0x5a ,0x1e);
        CL1009_Write(pCodecCtrl, 0x5b ,0x0f);
        CL1009_Write(pCodecCtrl, 0x5c ,0xca);
        CL1009_Write(pCodecCtrl, 0x5d ,0xc9);
        CL1009_Write(pCodecCtrl, 0x64 ,0x6d);
        CL1009_Write(pCodecCtrl, 0x65 ,0x84);
        CL1009_Write(pCodecCtrl, 0x66 ,0x18);
        CL1009_Write(pCodecCtrl, 0x67 ,0xfb);
        CL1009_Write(pCodecCtrl, 0x68 ,0x3f);
        //EQ Band 2
        CL1009_Write(pCodecCtrl, 0x7e ,0x81);
        CL1009_Write(pCodecCtrl, 0x7f ,0x00);
        CL1009_Write(pCodecCtrl, 0x7e ,0x82);
        CL1009_Write(pCodecCtrl, 0x7f ,0x00);
        CL1009_Write(pCodecCtrl, 0x7e ,0x83);
        CL1009_Write(pCodecCtrl, 0x7f ,0x00);
        CL1009_Write(pCodecCtrl, 0x7e ,0x84);
        CL1009_Write(pCodecCtrl, 0x7f ,0x00);
        CL1009_Write(pCodecCtrl, 0x7e ,0x85);
        CL1009_Write(pCodecCtrl, 0x7f ,0x00);
        CL1009_Write(pCodecCtrl, 0x7e ,0x86);
        CL1009_Write(pCodecCtrl, 0x7f ,0x00);
        CL1009_Write(pCodecCtrl, 0x7e ,0x87);
        CL1009_Write(pCodecCtrl, 0x7f ,0x40);
        CL1009_Write(pCodecCtrl, 0x7e ,0x88);
        CL1009_Write(pCodecCtrl, 0x7f ,0x00);
        CL1009_Write(pCodecCtrl, 0x7e ,0x89);
        CL1009_Write(pCodecCtrl, 0x7f ,0x00);
        CL1009_Write(pCodecCtrl, 0x7e ,0x8a);
        CL1009_Write(pCodecCtrl, 0x7f ,0x00);
        CL1009_Write(pCodecCtrl, 0x7e ,0x8b);
        CL1009_Write(pCodecCtrl, 0x7f ,0x00);
        CL1009_Write(pCodecCtrl, 0x7e ,0x8c);
        CL1009_Write(pCodecCtrl, 0x7f ,0x00);
        CL1009_Write(pCodecCtrl, 0x7e ,0x8d);
        CL1009_Write(pCodecCtrl, 0x7f ,0x00);
        CL1009_Write(pCodecCtrl, 0x7e ,0x8e);
        CL1009_Write(pCodecCtrl, 0x7f ,0x00);
        CL1009_Write(pCodecCtrl, 0x7e ,0x8f);
        CL1009_Write(pCodecCtrl, 0x7f ,0x00);
        //EN
        CL1009_Write(pCodecCtrl, 0x18 ,0x61);

      
    }
    return RetVal;
}

static UINT32 CL1009_SetVolume(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Level)
{
#if 0
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
    CL1009_Write(pCodecCtrl, 0x31, data);
    CL1009_Write(pCodecCtrl, 0x32, data);
#endif
    return 0;
}

static UINT32 CL1009_EnableSpeaker(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Enable)
{
    if (Enable) {
        CL1009_Write(pCodecCtrl, 0x05, 0x07);
        CL1009_Write(pCodecCtrl, 0x03, 0x20);
    } else {
        CL1009_Write(pCodecCtrl, 0x05, 0x27);
        CL1009_Write(pCodecCtrl, 0x03, 0x23);
    }
    
    return 0;
}

AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_CL1009Obj = {
    .Init       = CL1009_Init,
    .ModeConfig = CL1009_ModeConfig,
    .FreqConfig = CL1009_FreqConfig,
    .SetOutput  = CL1009_SetOutput,
    .SetInput   = CL1009_SetInput,
    .SetMute    = CL1009_SetMute,
    .Write      = CL1009_Write_App,
    .Read       = CL1009_Read_App,
    .SetVolume  = CL1009_SetVolume,
    .EnableSpeaker = CL1009_EnableSpeaker,
};
