
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"

#include "AmbaI2C.h"
#include "AmbaAudio_ES7210_ES8516.h"
#include "AmbaPrint.h"
#include "AmbaGPIO.h"

typedef enum {
    A_MIC = 0x00 ,
    SD_MIC ,  // Stereo DMIC
    LD_MIC ,  // Lch DMIC
    RD_MIC    // Rch DMIC
} ES7210_MIC_TYPE_e;

typedef enum {
    INPUT_PORT1 = 0x00 ,
    INPUT_PORT2 ,
    INPUT_PORT3
} ES7210_INPUT_PORT_e;

#if 0
static UINT32 Reg_Read(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 SlaveAddr, UINT32 RegAddr, const UINT32 *pRxData)
{
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};

    UINT8 TxData;
    UINT32 TxSize;
    UINT32 RetVal = AUCODEC_ERR_NONE;

    I2cTxConfig.SlaveAddr = SlaveAddr;
    I2cTxConfig.DataSize = 1U;
    I2cTxConfig.pDataBuf = &TxData;

    AmbaMisra_TypeCast32(&TxData, &RegAddr);/*TxData = Addr;*/  /* Sub Address */

    I2cRxConfig.SlaveAddr = SlaveAddr | 1U;
    I2cRxConfig.DataSize  = 1U;
    AmbaMisra_TypeCast32(&(I2cRxConfig.pDataBuf), &pRxData);/*I2cRxConfig.pDataBuf  = (UINT8 *)pRxData;*/
    RetVal = AmbaI2C_MasterReadAfterWrite(pCodecCtrl->I2cCtrl.I2cChanNo, pCodecCtrl->I2cCtrl.I2cSpeed,
                                          1U, &I2cTxConfig, &I2cRxConfig, &TxSize, AMBA_KAL_WAIT_FOREVER);
    if (RetVal != AUCODEC_ERR_NONE) {
        AmbaPrint_PrintStr5("[audio][error]i2c read does not work", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}
#endif

static UINT32 Reg_Write(const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 SlaveAddr, UINT32 RegAddr, UINT32 Data)
{
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[2];
    UINT32 TxSize;
    UINT32 RetVal = AUCODEC_ERR_NONE;

    I2cConfig.SlaveAddr = SlaveAddr;
    I2cConfig.DataSize = 2U;
    I2cConfig.pDataBuf = TxDataBuf;

    TxDataBuf[0] = (UINT8)RegAddr;
    TxDataBuf[1] = (UINT8)Data;

    RetVal = AmbaI2C_MasterWrite(pCodecCtrl->I2cCtrl.I2cChanNo, pCodecCtrl->I2cCtrl.I2cSpeed,
                                 &I2cConfig, &TxSize, AMBA_KAL_WAIT_FOREVER);
    if (RetVal != AUCODEC_ERR_NONE) {
        AmbaPrint_PrintStr5("[audio][error]i2c write does not work", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ES7210_ES8516_SetOutput
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
static UINT32 ES7210_ES8516_SetOutput(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 OutputMode)
{
    return AUCODEC_ERR_NONE;
}

static UINT32 MicType = AUCODEC_AMIC_IN;

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ES7210_ES8516_SetInput
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
static UINT32 ES7210_ES8516_SetInput(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 InputMode)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;
    UINT32 cnt = 0;

    if (AmbaKAL_MutexTake(&pCodecCtrl->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        MicType = InputMode;
        if (InputMode == AUCODEC_DMIC_IN) {
            for (cnt = 0; cnt < ES7210_DEV_NUM; cnt++) {
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x02, 0x81);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x08, 0x12);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x10, 0x40);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x11, 0x60); //i2s-16bits
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x12, 0x02); //normal i2s mode
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x14, 0x2C);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x15, 0x2C);
                /***set DMIC gain***/
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x1B, 0xf8);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x1C, 0xf8);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x1D, 0xf8);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x1E, 0xf8);//0db: 0xbf,0.5db step,max: 0xff= +32db,min:0x00= -95.5db
            }
        } else if (InputMode == AUCODEC_DMIC2_IN) {
            for (cnt = 0; cnt < ES7210_DEV_NUM; cnt++) {
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x02, 0x81);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x08, 0x12);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x10, 0x40);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x11, 0x60); //i2s-16bits
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x12, 0x02); //normal i2s mode
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x14, 0x2C);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x15, 0x2C);
                /***set DMIC gain***/
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x1B, 0xf6);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x1C, 0xf6);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x1D, 0xf6);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x1E, 0xf6);//0db: 0xbf,0.5db step,max: 0xff= +32db,min:0x00= -95.5db
            }
        } else {
            for (cnt = 0; cnt < ES7210_DEV_NUM; cnt++) {
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x02, 0xC1);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x08, 0x10);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x10, 0x00);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x11, 0x63); //dsp-16bits
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x12, 0x01); //1fs tdm  mode
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x14, 0x00);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x15, 0x03);
                /***set DMIC gain***/
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x1B, 0xf6);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x1C, 0xf6);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x1D, 0xf6);
                Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x1E, 0xf6);//0db: 0xbf,0.5db step,max: 0xff= +32db,min:0x00= -95.5db
            }
            MicType = AUCODEC_AMIC_IN;
        }
        for (cnt = 0; cnt < ES7210_DEV_NUM; cnt++) {
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x17, 0x20);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x13, 0x02);

            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x08, 0x14);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x24, 0x59);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x25, 0xBD);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x26, 0xEF);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x27, 0x11);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x28, 0x48);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x29, 0xAC);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x2A, 0xDF);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x2B, 0x11);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x2C, 0x59);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x2D, 0xBD);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x2E, 0xEF);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x2F, 0x11);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x30, 0x01);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x31, 0x46);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x32, 0xAD);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x33, 0x08);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x34, 0x14);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x35, 0x78);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x36, 0xCE);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x37, 0x3D);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x08, 0x10);
        }
        (void)AmbaKAL_MutexGive(&pCodecCtrl->Mutex);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ES7210_ES8516_SetMute
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
static UINT32 ES7210_ES8516_SetMute(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 MuteEnable)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ES7210_ES8516_ModeConfig
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
static UINT32 ES7210_ES8516_ModeConfig(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Mode)
{
    return AUCODEC_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ES7210_ES8516_FreqConfig
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
static UINT32 ES7210_ES8516_FreqConfig(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Freq)
{
    return AUCODEC_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ES7210_ES8516_Init
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
static UINT32 ES7210_ES8516_Init(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    UINT32 RetVal = AUCODEC_ERR_NONE;
    int cnt = 0;

    if (AmbaKAL_MutexCreate(&pCodecCtrl->Mutex, "ES7210_ES8516_Mutex") != KAL_ERR_NONE) {
        RetVal = AUCODEC_ERR_MUTEX;
    } else {
        //es7210 init
        for (cnt = 0; cnt < ES7210_DEV_NUM; cnt++) {
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x00, 0xFF);//reset
            AmbaKAL_TaskSleep(5);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x00, 0x32);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x0D, 0x29);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x09, 0x30);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x0A, 0x30);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x23, 0x26);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x22, 0x06);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x21, 0x26);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x20, 0x06);
            /***set dig interface ***/
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x40, 0xC3);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x41, 0x70);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x42, 0x70);
            /***set AMIC gain***/
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x43, 0x1A); //30db pga
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x44, 0x1A); //30db pga
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x45, 0x1C); //30db pga
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x46, 0x1A); //30db pga

            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x47, 0x08);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x48, 0x08);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x49, 0x08);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x4A, 0x08);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x07, 0x20);
            //Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x02, 0x41); //MCLK/LRCK = 64 RATIO
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x06, 0x00);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x04, 0x01);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x05, 0x80);// BCLK=96*lrclk,MCLK=BCLK*8
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x4B, 0x0F);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x4C, 0x0F);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x00, 0x71);
            Reg_Write(pCodecCtrl, ES7210_CHIP_ADR_BASE + cnt, 0x00, 0x41);
        }

        //es8256 init
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x00, 0x1C);

        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x02, 0x04);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x20, 0x2A);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x21, 0x3C);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x22, 0x08);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x24, 0x07);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x23, 0x00);

        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x0A, 0x01);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x0B, 0x01);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x14, 0xBF);

        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x01, 0x21);
#ifdef CONFIG_PCBA_DVT
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x0D, 0x15);
#else
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x0D, 0x14);
#endif
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x18, 0x00);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x08, 0x3F);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x00, 0x02);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x00, 0x03);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x25, 0x20);


        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x19, 0x12);   //EQ 
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0xFC, 0x01);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x00, 0x00);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x01, 0x15);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x02, 0x08);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x03, 0x1E);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x04, 0x0F);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x05, 0x1F);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x06, 0x10);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x07, 0x10);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x08, 0x04);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x09, 0x17);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x0A, 0x0D);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x0B, 0x1F);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x0C, 0x00);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x0D, 0x00);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x0E, 0x14);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x0F, 0x07);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x10, 0x1A);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x11, 0x0D);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x12, 0x10);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x13, 0x04);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x14, 0x17);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x15, 0x1B);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x16, 0x1D);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x17, 0x1F);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x18, 0x00);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x19, 0x15);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x1A, 0x08);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x1B, 0x1E);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x1C, 0x0F);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x1D, 0x1F);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x1E, 0x04);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x1F, 0x07);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x20, 0x09);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x21, 0x1E);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x22, 0x0F);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x23, 0x1F);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x24, 0x03);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x25, 0x06);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x26, 0x08);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x27, 0x1D);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x28, 0x0E);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x29, 0x1F);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x2A, 0x00);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x2B, 0x03);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x2C, 0x15);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x2D, 0x06);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x2E, 0x1C);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x2F, 0x0D);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x30, 0x11);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x31, 0x03);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x32, 0x16);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x33, 0x17);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x34, 0x0B);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x35, 0x1C);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x36, 0x04);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x37, 0x07);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x38, 0x09);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x39, 0x1E);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x3A, 0x0F);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x3B, 0x1F);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0xFC, 0x00);
        Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x19, 0x11);
    }

    return RetVal;
}

static UINT32 ES7210_ES8516_SetVolume(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Level)
{
    UINT32 data = 0;

    if (Level > 3) {
        Level = 3;
    }
    switch(Level) {
    case 0:  //off----96db
        /* mute dis & DAC control */
        data = 0x00;
        break;
    case 1:  //low
        data = 0xa0;
        break;
    case 2:  //mid
        data = 0xb0;
        break;
    case 3:  //high es8156 0db level=1.6V Vrms
        data = 0xc0;
        break;
    default:
        data = 0xb0;
        break;
    }
    Reg_Write(pCodecCtrl, ES8156_CHIP_ADR_BASE, 0x14, data);

    return 0;
}

static UINT32 ES7210_ES8516_GetMicType(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl)
{
    return MicType;
}

static UINT32 ES7210_ES8516_EnableSpeaker(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Enable)
{
    AmbaGPIO_SetFuncGPO(GPIO_PIN_96, Enable ? AMBA_GPIO_LEVEL_HIGH : AMBA_GPIO_LEVEL_LOW);

    return 0;
}

AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_ES7210_ES8516Obj = {
    .Init       = ES7210_ES8516_Init,
    .ModeConfig = ES7210_ES8516_ModeConfig,
    .FreqConfig = ES7210_ES8516_FreqConfig,
    .SetOutput  = ES7210_ES8516_SetOutput,
    .SetInput   = ES7210_ES8516_SetInput,
    .SetMute    = ES7210_ES8516_SetMute,
    .Write      = NULL,
    .Read       = NULL,
    .SetVolume  = ES7210_ES8516_SetVolume,
    .EnableSpeaker = ES7210_ES8516_EnableSpeaker,
    .GetMicType = ES7210_ES8516_GetMicType,
};
