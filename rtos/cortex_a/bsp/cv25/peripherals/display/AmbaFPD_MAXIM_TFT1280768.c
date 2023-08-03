/**
 *  @file AmbaFPD_MAXIM_TFT1280768.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Control APIs of DZXtech Display FPD panel MAXIM_TFT1280768
 *
 */

#include "AmbaTypes.h"

#include "AmbaKAL.h"
#include "AmbaI2C.h"
#include "AmbaFPD.h"
#include "AmbaVOUT.h"
#include "AmbaFPD_MAXIM_Define.h"
#include "AmbaFPD_MAXIM_TFT1280768.h"
#if MAXIM_TFT1280768_GPIO_FORWARD_MODE == MAXIM_GPIO_FORWARD_PWM_AND_KEY
#include "AmbaGPIO.h"
#include "AmbaPWM.h"
#endif

static AMBA_FPD_MAXIM_CONFIG_s *pDispConfig;

/*-----------------------------------------------------------------------------------------------*\
 * MAXIM video mode info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_FPD_MAXIM_CONFIG_s MAXIM_TFT1280768_Config[AMBA_FPD_MAXIM_SERDES_NUM_MODE] = {
    [AMBA_FPD_MAXIM_SINGLE_A_4LANE_60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_4LANE,
        .DisplayTiming    = {
            .PixelClkFreq       = 72000000,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 141,
            .HsyncPulseWidth    = 4,
            .HsyncBackPorch     = 5,
            .VsyncFrontPorch    = 68,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 2,
            .ActivePixels       = 1280,
            .ActiveLines        = 768,
        },
        .BlankPacket = {
            .LenHBP     = 2,
            .LenHSA     = 3,
            .LenHFP     = 413,
            .LenBLLP    = 0,
        }
    },
    [AMBA_FPD_MAXIM_SINGLE_A_4LANE_A60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_4LANE,
        .DisplayTiming    = {
            .PixelClkFreq       = 72072000,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 141,
            .HsyncPulseWidth    = 4,
            .HsyncBackPorch     = 5,
            .VsyncFrontPorch    = 68,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 2,
            .ActivePixels       = 1280,
            .ActiveLines        = 768,
        },
        .BlankPacket = {
            .LenHBP     = 2,
            .LenHSA     = 3,
            .LenHFP     = 413,
            .LenBLLP    = 0,
        }
    },
    [AMBA_FPD_MAXIM_SINGLE_B_4LANE_60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_4LANE,
        .DisplayTiming    = {
            .PixelClkFreq       = 72000000,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 141,
            .HsyncPulseWidth    = 4,
            .HsyncBackPorch     = 5,
            .VsyncFrontPorch    = 68,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 2,
            .ActivePixels       = 1280,
            .ActiveLines        = 768,
        },
        .BlankPacket = {
            .LenHBP     = 2,
            .LenHSA     = 3,
            .LenHFP     = 413,
            .LenBLLP    = 0,
        }
    },
    [AMBA_FPD_MAXIM_SINGLE_B_4LANE_A60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_4LANE,
        .DisplayTiming    = {
            .PixelClkFreq       = 72072000,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 141,
            .HsyncPulseWidth    = 4,
            .HsyncBackPorch     = 5,
            .VsyncFrontPorch    = 68,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 2,
            .ActivePixels       = 1280,
            .ActiveLines        = 768,
        },
        .BlankPacket = {
            .LenHBP     = 2,
            .LenHSA     = 3,
            .LenHFP     = 413,
            .LenBLLP    = 0,
        }
    },
    [AMBA_FPD_MAXIM_DUAL_4LANE_60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_4LANE,
        .DisplayTiming    = {
            .PixelClkFreq       = 143737223,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 168,
            .HsyncPulseWidth    = 56,
            .HsyncBackPorch     = 64,
            .VsyncFrontPorch    = 52,
            .VsyncPulseWidth    = 11,
            .VsyncBackPorch     = 11,
            .ActivePixels       = 2560,
            .ActiveLines        = 768,
        },
        .BlankPacket = {
            .LenHBP     = 182,
            .LenHSA     = 156,
            .LenHFP     = 494,
            .LenBLLP    = 0,
        }
    },
    [AMBA_FPD_MAXIM_DUAL_4LANE_A60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_4LANE,
        .DisplayTiming    = {
            .PixelClkFreq       = 143880960,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 168,
            .HsyncPulseWidth    = 56,
            .HsyncBackPorch     = 64,
            .VsyncFrontPorch    = 52,
            .VsyncPulseWidth    = 11,
            .VsyncBackPorch     = 11,
            .ActivePixels       = 2560,
            .ActiveLines        = 768,
        },
        .BlankPacket = {
            .LenHBP     = 182,
            .LenHSA     = 156,
            .LenHFP     = 494,
            .LenBLLP    = 0,
        }
    },
    [AMBA_FPD_MAXIM_SINGLE_A_2LANE_60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_2LANE,
        .DisplayTiming    = {
            .PixelClkFreq       = 72000000,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 141,
            .HsyncPulseWidth    = 4,
            .HsyncBackPorch     = 5,
            .VsyncFrontPorch    = 68,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 2,
            .ActivePixels       = 1280,
            .ActiveLines        = 768,
        },
        .BlankPacket = {
            .LenHBP     = 2,
            .LenHSA     = 3,
            .LenHFP     = 413,
            .LenBLLP    = 0,
        }
    },
    [AMBA_FPD_MAXIM_SINGLE_A_2LANE_A60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_2LANE,
        .DisplayTiming    = {
            .PixelClkFreq       = 72072000,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 141,
            .HsyncPulseWidth    = 4,
            .HsyncBackPorch     = 5,
            .VsyncFrontPorch    = 68,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 2,
            .ActivePixels       = 1280,
            .ActiveLines        = 768,
        },
        .BlankPacket = {
            .LenHBP     = 2,
            .LenHSA     = 3,
            .LenHFP     = 413,
            .LenBLLP    = 0,
        }
    },
    [AMBA_FPD_MAXIM_SINGLE_B_2LANE_60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_2LANE,
        .DisplayTiming    = {
            .PixelClkFreq       = 72000000,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 141,
            .HsyncPulseWidth    = 4,
            .HsyncBackPorch     = 5,
            .VsyncFrontPorch    = 68,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 2,
            .ActivePixels       = 1280,
            .ActiveLines        = 768,
        },
        .BlankPacket = {
            .LenHBP     = 2,
            .LenHSA     = 3,
            .LenHFP     = 413,
            .LenBLLP    = 0,
        }
    },
    [AMBA_FPD_MAXIM_SINGLE_B_2LANE_A60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_2LANE,
        .DisplayTiming    = {
            .PixelClkFreq       = 72072000,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 141,
            .HsyncPulseWidth    = 4,
            .HsyncBackPorch     = 5,
            .VsyncFrontPorch    = 68,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 2,
            .ActivePixels       = 1280,
            .ActiveLines        = 768,
        },
        .BlankPacket = {
            .LenHBP     = 2,
            .LenHSA     = 3,
            .LenHFP     = 413,
            .LenBLLP    = 0,
        }
    },
    [AMBA_FPD_MAXIM_DUAL_2LANE_60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_2LANE,
        .DisplayTiming    = {
            .PixelClkFreq       = 143737223,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 168,
            .HsyncPulseWidth    = 56,
            .HsyncBackPorch     = 64,
            .VsyncFrontPorch    = 52,
            .VsyncPulseWidth    = 11,
            .VsyncBackPorch     = 11,
            .ActivePixels       = 2560,
            .ActiveLines        = 768,
        },
        .BlankPacket = {
            .LenHBP     = 182,
            .LenHSA     = 156,
            .LenHFP     = 494,
            .LenBLLP    = 0,
        }
    },
    [AMBA_FPD_MAXIM_DUAL_2LANE_A60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_2LANE,
        .DisplayTiming    = {
            .PixelClkFreq       = 143880960,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 168,
            .HsyncPulseWidth    = 56,
            .HsyncBackPorch     = 64,
            .VsyncFrontPorch    = 52,
            .VsyncPulseWidth    = 11,
            .VsyncBackPorch     = 11,
            .ActivePixels       = 2560,
            .ActiveLines        = 768,
        },
        .BlankPacket = {
            .LenHBP     = 182,
            .LenHSA     = 156,
            .LenHFP     = 494,
            .LenBLLP    = 0,
        }
    }
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAXIM_RegRead
 *
 *  @Description:: Read the register of MAXIM_I2C serializer Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MAXIM_RegRead(UINT32 I2cChanNo, UINT32 SlaveAddr, UINT16 RegisterAddr, UINT8 *pRxData)
{
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0U};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0U};
    UINT8 TxData[2] = {0U};
    UINT32 TxSize = 1U;

    I2cTxConfig.SlaveAddr = SlaveAddr;
    I2cTxConfig.DataSize  = 2U;
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8)(RegisterAddr >> 8U);
    TxData[1] = (UINT8)(RegisterAddr & 0xffU);

    I2cRxConfig.SlaveAddr = (SlaveAddr | (UINT32)1U);
    I2cRxConfig.DataSize  = 1;
    I2cRxConfig.pDataBuf  = pRxData;

    return AmbaI2C_MasterReadAfterWrite(I2cChanNo, AMBA_I2C_SPEED_FAST, 1U, &I2cTxConfig, &I2cRxConfig, &TxSize, 1000U);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAXIM_RegWrite
 *
 *  @Description:: Write the register of MAXIM_I2C serializer Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MAXIM_RegWrite(UINT32 I2cChanNo, UINT32 SlaveAddr, UINT16 RegisterAddr, UINT8 Data)
{
    AMBA_I2C_TRANSACTION_s I2cConfig = {0U};
    UINT8 TxDataBuf[3] = {0U};
    UINT32 TxSize = 1U;

    I2cConfig.SlaveAddr = SlaveAddr;
    I2cConfig.DataSize  = 3U;
    I2cConfig.pDataBuf  = TxDataBuf;

    TxDataBuf[0] = (UINT8) ((RegisterAddr & 0xff00U) >> 8U);
    TxDataBuf[1] = (UINT8) (RegisterAddr & 0x00ffU);
    TxDataBuf[2] = Data;

    return AmbaI2C_MasterWrite(I2cChanNo, AMBA_I2C_SPEED_FAST, &I2cConfig, &TxSize, 1000U);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAXIM_RegWriteByTable
 *
 *  @Description:: Write the register of MAXIM_I2C serializer Device by table
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MAXIM_RegWriteByTable(const MAXIM_I2C_CTRL_s* Register_Table, UINT32 length)
{
    UINT32 RetVal = ERR_NONE;
    UINT32 i = 0U;
    UINT8 pRxData = 0U;

    //AmbaPrint_PrintUInt5("Start to program the serializer and deserializer", 0U, 0U, 0U, 0U, 0U);
    if (MAXIM_RegRead(Register_Table[0].I2cChanNo, Register_Table[0].SlaveAddr, MAXIM_REG_DEV_ID, &pRxData) == (UINT32)I2C_ERR_NONE) {
        for (i = 0; i < length; i++) {
            RetVal = MAXIM_RegWrite(Register_Table[i].I2cChanNo, Register_Table[i].SlaveAddr, Register_Table[i].RegisterAddr, Register_Table[i].pRxData);

            //resetting function need 100msec delay.
            (void)AmbaKAL_TaskSleep(Register_Table[i].Delay);
        }

    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAX96755_GetLinkInfo
 *
 *  @Description:: To get link information of MAX96755
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MAX96755_GetLinkInfo(void)
{
    UINT32 RetVal = ERR_NONE;
    UINT8 pRxData = 0U;

    if (MAXIM_RegRead(MAXIM_TFT1280768_96755_A_I2C_CHANNEL, MAXIM_TFT1280768_96755_A_I2C_ADDR, MAXIM_REG_DEV_ID, &pRxData) == I2C_ERR_NONE) {
        if (pRxData == (UINT8)DEV_ID_MAX96755) {
            RetVal = SERDES_LINK_OK;
        } else {
            RetVal = SERDES_LINK_WRONG;
        }
    } else {
        RetVal = SERDES_LINK_ERROR;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAX96752_A_GetLinkInfo
 *
 *  @Description:: To get link information of MAX96752
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MAX96752_A_GetLinkInfo(void)
{
    UINT32 RetVal = SERDES_LINK_OK;
    UINT8 pRxData = 0U;

    if (MAXIM_RegRead(MAXIM_TFT1280768_96752_A_I2C_CHANNEL, MAXIM_TFT1280768_96752_A_I2C_ADDR, MAXIM_REG_DEV_ID, &pRxData) == I2C_ERR_NONE) {
        if (pRxData == (UINT8)DEV_ID_MAX96752) {
            RetVal = SERDES_LINK_OK;
        } else {
            RetVal = SERDES_LINK_WRONG;
        }
    } else {
        RetVal = SERDES_LINK_ERROR;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAX96752_B_GetLinkInfo
 *
 *  @Description:: To get link information of MAX96752
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MAX96752_B_GetLinkInfo(void)
{
    UINT32 RetVal = SERDES_LINK_OK;
    UINT8 pRxData = 0U;

    if (MAXIM_RegRead(MAXIM_TFT1280768_96752_B_I2C_CHANNEL, MAXIM_TFT1280768_96752_B_I2C_ADDR, MAXIM_REG_DEV_ID, &pRxData) == I2C_ERR_NONE) {
        if (pRxData == (UINT8)DEV_ID_MAX96752) {
            RetVal = SERDES_LINK_OK;
        } else {
            RetVal = SERDES_LINK_WRONG;
        }
    } else {
        RetVal = SERDES_LINK_ERROR;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAX96755_GetMipiDsiRxCRCInfo
 *
 *  @Description:: To Get the MIPI DSI CRC error information of MAX96755
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MAX96755_GetMipiDsiRxCRCInfo(void)
{
#define MAX96755_INTR7    0x001F
#define MIPI_ERR_FLAG     0x01

    UINT32 RetVal = ERR_NONE;
    UINT8 pRxData = 0U;

    if (MAXIM_RegRead(MAXIM_TFT1280768_96755_A_I2C_CHANNEL, MAXIM_TFT1280768_96755_A_I2C_ADDR, MAX96755_INTR7, &pRxData) == I2C_ERR_NONE) {
        if (pRxData == (UINT8)MIPI_ERR_FLAG) {
            RetVal = MIPI_DSI_CRC_NONE;
        } else {
            RetVal = MIPI_DSI_CRC_ERROR;
        }
    } else {
        RetVal = MIPI_DSI_CRC_NOLINK;
    }

    return RetVal;
}

#if MAXIM_TFT1280768_ENABLE_IOKEY
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAXIM_TFT1280768_InitIOKey
 *
 *  @Description:: Initialize IO Key
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 MAXIM_TFT1280768_InitIOKey(void)
{
    //Initialize MAX96752_A
    const MAXIM_I2C_CTRL_s Register_Table_MAXIM_96752_IOKEY_INIT[] = {
        //SW10_GPIO01_ENTER
        //GPIO_R_EN = 1, The GPIO source enabled for GMSL2 reception
        //GPIO_OUT_DIS = 1, Output driver enabled
        //Device address       Register Address    Data
        //0xD4,                0x0203,             0x85,
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_IOKEY_ENTER,   0x85,    MAXIM_I2C_DELAY_COMMAND    },

        //SW11_GPIO04_UP
        //GPIO_R_EN = 1, The GPIO source enabled for GMSL2 reception
        //GPIO_OUT_DIS = 1, Output driver enabled
        //Device address       Register Address    Data
        //0xD4,                0x020C,             0x85
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_IOKEY_UP,      0x85,    MAXIM_I2C_DELAY_COMMAND    },

        //SW12_GPIO09_DOWN
        //bit2, GPIO_R_EN = 1, The GPIO source enabled for GMSL2 reception
        //bit0, GPIO_OUT_DIS = 1, Output driver enabled
        //Device address       Register Address    Data
        //0xD4,                0x021B,             0x85
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_IOKEY_DOWN,    0x85,    MAXIM_I2C_DELAY_COMMAND    },

        //SW13_GPIO13_MENU
        //AUD_EN_RX=0, Audio receiver disabled for GPIO13 changed function mode
        //Device address       Register Address    Data
        //0xD4,                0x0140,             0x00
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0140,                  0x00,    MAXIM_I2C_DELAY_COMMAND    },

        //bit2, GPIO_R_EN = 1, The GPIO source enabled for GMSL2 reception
        //bit0, GPIO_OUT_DIS = 1, Output driver enabled
        //Device address       Register Address    Data
        //0xD4,                0x0227,             0x85
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_IOKEY_MENU,    0x85,    MAXIM_I2C_DELAY_COMMAND    },
    };

    return MAXIM_RegWriteByTable(Register_Table_MAXIM_96752_IOKEY_INIT, sizeof(Register_Table_MAXIM_96752_IOKEY_INIT) / sizeof(Register_Table_MAXIM_96752_IOKEY_INIT[0]));
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAXIM_TFT1280768_GetIOKeyInfo
 *
 *  @Description:: Read IO Key
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 MAXIM_TFT1280768_GetIOKeyInfo(UINT8 pIOKeyMode, UINT8 *pIOKeyInfo)
{
    UINT8 pRxData;
    UINT32 RetVal = ERR_NONE;

    if (pIOKeyMode < MAXIM_IOKEY_NUM_MODE) {
        const MAXIM_I2C_CTRL_s Register_Table_MAXIM_96752_IOKEY_READ[MAXIM_IOKEY_NUM_MODE] = {
            //SW10_GPIO01_ENTER
            //Device address       Register Address    Data
            //0xD4,                0x0203,             0x00,
            {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_IOKEY_ENTER,   0x00,    MAXIM_I2C_DELAY_COMMAND    },

            //SW11_GPIO04_UP
            //Device address       Register Address    Data
            //0xD4,                0x020C,             0x00
            {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_IOKEY_UP,      0x00,    MAXIM_I2C_DELAY_COMMAND    },

            //SW12_GPIO09_DOWN
            //Device address       Register Address    Data
            //0xD4,                0x021B,             0x00
            {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_IOKEY_DOWN,    0x00,    MAXIM_I2C_DELAY_COMMAND    },

            //SW13_GPIO13_MENU
            //Device address       Register Address    Data
            //0xD4,                0x0227,             0x00
            {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_IOKEY_MENU,    0x00,    MAXIM_I2C_DELAY_COMMAND    },
        };

        //read bit3 GPIO_IN
        RetVal = MAXIM_RegRead(Register_Table_MAXIM_96752_IOKEY_READ[pIOKeyMode].I2cChanNo, Register_Table_MAXIM_96752_IOKEY_READ[pIOKeyMode].SlaveAddr, Register_Table_MAXIM_96752_IOKEY_READ[pIOKeyMode].RegisterAddr, &pRxData);
        if (RetVal == ERR_NONE) {
            if ((pRxData & MAXIM_GPIO_IN) != 0U) {
                *pIOKeyInfo = MAXIM_GPIO_LEVEL_LOW;
            } else {
                *pIOKeyInfo = MAXIM_GPIO_LEVEL_HIGH;
            }
        }
    } else {
        RetVal = ERR_ARG;
    }

    return RetVal;
}
#endif    //MAXIM_TFT1280768_ENABLE_IOKEY

#if MAXIM_TFT1280768_GPIO_FORWARD_MODE == MAXIM_GPIO_FORWARD_KEY
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAXIM_ConfigGpioForward
 *
 *  @Description:: configure GPIO forward
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MAXIM_ConfigGpioForward(void)
{
    //Config. GPIO forward
    const MAXIM_I2C_CTRL_s Register_Table_MAXIM_CONFIG_GPIO_FORWARD[] = {
        //Disable LOCK_EN and LOC_MS_EN for GPIO00
        //Device address       Register Address    Data
        //0xC0,                0x0005,             0x40,
        //0xC0,                0x0048,             0x42,
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x0005,    0x40,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x0048,    0x42,    MAXIM_I2C_DELAY_COMMAND    },

        //MIPI_DSI_Button1
        //CV25DK_GPIO003 <--- MAX96755_GPIO00 <--- MAX96752_GPIO01
        //Device address       Register Address    Data
        //0xC0,                0x02F4,             0xA4,
        //0xC0,                0x02F5,             0xA1,
        //0xC0,                0x02F6,             0x12,
        //0xD4,                0x0203,             0xA3,
        //0xD4,                0x0204,             0xB2,
        //0xD4,                0x0205,             0x01,
        //Reset GPIO IN status [0xB2 -> 0xA3]
        //0xD4,                0x0203,             0x80,
        //0xD4,                0x0203,             0x90,
        //0xD4,                0x0203,             0xA3,
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F4,    0xA4,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F5,    0xA1,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F6,    0x02,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0203,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0204,    0xA2,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0205,    0x01,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0203,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0203,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0203,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },

        //MIPI_DSI_Button2
        //CV25DK_GPIO100 <--- MAX96755_GPIO17 <--- MAX96752_GPIO04
        //Device address       Register Address    Data
        //0xC0,                0x02F1,             0xA4,
        //0xC0,                0x02F2,             0xA4,
        //0xC0,                0x02F3,             0x11,
        //0xD4,                0x020C,             0xA3,
        //0xD4,                0x020D,             0x71,
        //0xD4,                0x020E,             0x04,
        //Reset GPIO IN status [0xB2 -> 0xA3]
        //0xD4,                0x020C,             0x80,
        //0xD4,                0x020C,             0x90,
        //0xD4,                0x020C,             0xA3,

        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F1,    0xA4,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F2,    0xA4,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F3,    0x11,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x020C,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x020D,    0x71,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x020E,    0x04,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x020C,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x020C,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x020C,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },

        //MIPI_DSI_Button3
        //CV25DK_GPIO009 <--- MAX96755_GPIO16 <--- MAX96752_GPIO09
        //Device address       Register Address    Data
        //0xC0,                0x02EE,             0xA4,
        //0xC0,                0x02EF,             0xA9,
        //0xC0,                0x02F0,             0x10,
        //0xD4,                0x021B,             0xA3,
        //0xD4,                0x021C,             0xB0,
        //0xD4,                0x021D,             0x09,
        //Reset GPIO IN status [0xB2 -> 0xA3]
        //0xD4,                0x021B,             0x80,
        //0xD4,                0x021B,             0x90,
        //0xD4,                0x021B,             0xA3,
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02EE,    0xA4,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02EF,    0xA9,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F0,    0x10,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x021B,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x021C,    0xB0,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x021D,    0x09,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x021B,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x021B,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x021B,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },

        //MIPI_DSI_Button4
        //CV25DK_GPIO080 <--- MAX96755_GPIO15 <--- MAX96752_GPIO13
        //Device address       Register Address    Data
        //0xC0,                0x02EB,             0xA4,
        //0xC0,                0x02EC,             0xAD,
        //0xC0,                0x02ED,             0x0F,
        //0xD4,                0x0227,             0xA3,
        //0xD4,                0x0228,             0xAF,
        //0xD4,                0x0229,             0x0D,
        //Reset GPIO IN status [0xB2 -> 0xA3]
        //0xD4,                0x0227,             0x80,
        //0xD4,                0x0227,             0x90,
        //0xD4,                0x0227,             0xA3,
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02EB,    0xA4,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02EC,    0xAD,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02ED,    0x0F,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0227,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0228,    0xAF,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0229,    0x0D,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0227,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0227,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0227,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },

        //AUD_EN_RX=0, Audio receiver disabled for GPIO13 changed function mode
        //0xD4,                0x0140,             0x00,
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0140,    0x00,    MAXIM_I2C_DELAY_COMMAND    },
    };

    return MAXIM_RegWriteByTable(Register_Table_MAXIM_CONFIG_GPIO_FORWARD, sizeof(Register_Table_MAXIM_CONFIG_GPIO_FORWARD) / sizeof(Register_Table_MAXIM_CONFIG_GPIO_FORWARD[0]));
}
#elif MAXIM_TFT1280768_GPIO_FORWARD_MODE == MAXIM_GPIO_FORWARD_PWM_AND_KEY
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAXIM_ConfigGpioForward
 *
 *  @Description:: configure GPIO forward
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MAXIM_ConfigGpioForward(void)
{
    //Config. GPIO forward
    const MAXIM_I2C_CTRL_s Register_Table_MAXIM_CONFIG_GPIO_FORWARD[] = {
        //Disable LOCK_EN and LOC_MS_EN for GPIO00
        //Device address       Register Address    Data
        //0xC0,                0x0005,             0x40,
        //0xC0,                0x0048,             0x42,
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x0005,    0x40,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x0048,    0x42,    MAXIM_I2C_DELAY_COMMAND    },

        //MIPI_DSI_Button1
        //CV25DK_GPIO003 <--- MAX96755_GPIO00 <--- MAX96752_GPIO01
        //Device address       Register Address    Data
        //0xC0,                0x02BE,             0xA4,
        //0xC0,                0x02BF,             0xA1,
        //0xC0,                0x02C0,             0x12,
        //0xD4,                0x0203,             0xA3,
        //0xD4,                0x0204,             0xB2,
        //0xD4,                0x0205,             0x01,
        //Reset GPIO IN status [0xB2 -> 0xA3]
        //0xD4,                0x0203,             0x80,
        //0xD4,                0x0203,             0x90,
        //0xD4,                0x0203,             0xA3,
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02BE,    0xA4,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02BF,    0xA1,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02C0,    0x02,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0203,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0204,    0xA2,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0205,    0x01,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0203,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0203,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0203,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },

        //MIPI_DSI_Button2
        //CV25DK_GPIO100 <--- MAX96755_GPIO17 <--- MAX96752_GPIO04
        //Device address       Register Address    Data
        //0xC0,                0x02F1,             0xA4,
        //0xC0,                0x02F2,             0xA4,
        //0xC0,                0x02F3,             0x11,
        //0xD4,                0x020C,             0xA3,
        //0xD4,                0x020D,             0x71,
        //0xD4,                0x020E,             0x04,
        //Reset GPIO IN status [0xB2 -> 0xA3]
        //0xD4,                0x020C,             0x80,
        //0xD4,                0x020C,             0x90,
        //0xD4,                0x020C,             0xA3,

        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F1,    0xA4,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F2,    0xA4,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F3,    0x11,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x020C,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x020D,    0x71,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x020E,    0x04,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x020C,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x020C,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x020C,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },

        //MIPI_DSI_Button3
        //CV25DK_GPIO009 <--- MAX96755_GPIO16 <--- MAX96752_GPIO09
        //Device address       Register Address    Data
        //0xC0,                0x02EE,             0xA4,
        //0xC0,                0x02EF,             0xA9,
        //0xC0,                0x02F0,             0x10,
        //0xD4,                0x021B,             0xA3,
        //0xD4,                0x021C,             0xB0,
        //0xD4,                0x021D,             0x09,
        //Reset GPIO IN status [0xB2 -> 0xA3]
        //0xD4,                0x021B,             0x80,
        //0xD4,                0x021B,             0x90,
        //0xD4,                0x021B,             0xA3,
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02EE,    0xA4,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02EF,    0xA9,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F0,    0x10,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x021B,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x021C,    0xB0,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x021D,    0x09,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x021B,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x021B,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x021B,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },

        //MIPI_DSI_Button4
        //CV25DK_GPIO080 <--- MAX96755_GPIO15 <--- MAX96752_GPIO13
        //Device address       Register Address    Data
        //0xC0,                0x02EB,             0xA4,
        //0xC0,                0x02EC,             0xAD,
        //0xC0,                0x02ED,             0x0F,
        //0xD4,                0x0227,             0xA3,
        //0xD4,                0x0228,             0xAF,
        //0xD4,                0x0229,             0x0D,
        //Reset GPIO IN status [0xB2 -> 0xA3]
        //0xD4,                0x0227,             0x80,
        //0xD4,                0x0227,             0x90,
        //0xD4,                0x0227,             0xA3,
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02EB,    0xA4,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02EC,    0xAD,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02ED,    0x0F,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0227,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0228,    0xAF,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0229,    0x0D,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0227,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0227,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0227,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },

        //LCD Brightness PWM
        //CV25DK_GPIO99 ---> MAX96755_GPIO18 ---> MAX96752_GPIO08
        //Device address       Register Address    Data
        //0xC0,                0x02F4,             0xA3,
        //0xC0,                0x02F5,             0xA8,
        //0xC0,                0x02F6,             0x12,
        //0xD4,                0x0218,             0xA4,
        //0xD4,                0x0219,             0xB2,
        //0xD4,                0x021A,             0x08,
        //Reset GPIO status
        //0xC0,                0x02F4,             0x80,
        //0xC0,                0x02F4,             0x90,
        //0xC0,                0x02F4,             0xA3,
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F4,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F5,    0xA8,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F6,    0x12,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0218,    0xA4,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0219,    0xB2,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x021A,    0x08,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F4,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F4,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x02F4,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },

        //AUD_EN_RX=0, Audio receiver disabled for GPIO13 changed function mode
        //0xD4,                0x0140,             0x00,
        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0140,    0x00,    MAXIM_I2C_DELAY_COMMAND    },
    };

    return MAXIM_RegWriteByTable(Register_Table_MAXIM_CONFIG_GPIO_FORWARD, sizeof(Register_Table_MAXIM_CONFIG_GPIO_FORWARD) / sizeof(Register_Table_MAXIM_CONFIG_GPIO_FORWARD[0]));
}
#endif    //MAXIM_TFT1280768_GPIO_FORWARD_MODE

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_MAXIMEnable
 *
 *  @Description:: Enable FPD panel device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_MAXIMEnable(void)
{
    UINT32 RetVal = ERR_NONE;
    UINT32 LinkInfoMAX96755 = MAX96755_GetLinkInfo();
    UINT32 LinkInfoMAX96752A = MAX96752_A_GetLinkInfo();
    UINT32 LinkInfoMAX96752B = MAX96752_B_GetLinkInfo();

    if ((LinkInfoMAX96755 == SERDES_LINK_OK) && (LinkInfoMAX96752A == SERDES_LINK_OK) && (LinkInfoMAX96752B == SERDES_LINK_OK)) {
#if MAXIM_TFT1280768_ENABLE_GPIO
        const MAXIM_I2C_CTRL_s Table_MAXIM_96752_On[] = {
            //Set RSTB high(GPIO7)  [0x0215]
            //Device address       Register Address    Data
            //0xD4,                0x0215,             0x90,
            //0xD8,                0x0215,             0x90,
            {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_07,    0x90,    0     },
            {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_07,    0x90,    10    },

            //Set STBYB high(GPIO6) [0x0212]
            //Device address       Register Address    Data
            //0xD4,                0x0212,             0x90
            //0xD8,                0x0212,             0x90
            {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_06,    0x90,    0      },
            {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_06,    0x90,    110    },

            //Backlight on (GPIO2) [0x0206]
            //Device address       Register Address    Data
            //0xD4,                0x0206,             0x90
            //0xD8,                0x0206,             0x90
            {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_02,    0x90,    0      },
            {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_02,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
        };

        //Enable MAX96752 A and B
        RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_On, sizeof(Table_MAXIM_96752_On) / sizeof(Table_MAXIM_96752_On[0]));
#else
        RetVal = ERR_NONE;
#endif    //MAXIM_TFT1280768_ENABLE_GPIO
    } else if ((LinkInfoMAX96755 == SERDES_LINK_OK) && (LinkInfoMAX96752A == SERDES_LINK_OK)) {
#if MAXIM_TFT1280768_ENABLE_GPIO
        const MAXIM_I2C_CTRL_s Table_MAXIM_96752_On[] = {
            //Set RSTB high(GPIO7)  [0x0215]
            //Device address       Register Address    Data
            //0xD4,                0x0215,             0x90,
            {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_07,    0x90,    10    },

            //Set STBYB high(GPIO6) [0x0212]
            //Device address       Register Address    Data
            //0xD4,                0x0212,             0x90
            {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_06,    0x90,    110    },

            //Backlight on (GPIO2) [0x0206]
            //Device address       Register Address    Data
            //0xD4,                0x0206,             0x90
            {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_02,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
        };

        //Enable MAX96752 A
        RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_On, sizeof(Table_MAXIM_96752_On) / sizeof(Table_MAXIM_96752_On[0]));
#else
        RetVal = ERR_NONE;
#endif    //MAXIM_TFT1280768_ENABLE_GPIO
    } else if ((LinkInfoMAX96755 == SERDES_LINK_OK) && (LinkInfoMAX96752B == SERDES_LINK_OK)) {
#if MAXIM_TFT1280768_ENABLE_GPIO
        const MAXIM_I2C_CTRL_s Table_MAXIM_96752_On[] = {
            //Set RSTB high(GPIO7)  [0x0215]
            //Device address       Register Address    Data
            //0xD8,                0x0215,             0x90,
            {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_07,    0x90,    10    },

            //Set STBYB high(GPIO6) [0x0212]
            //Device address       Register Address    Data
            //0xD8,                0x0212,             0x90
            {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_06,    0x90,    110    },

            //Backlight on (GPIO2) [0x0206]
            //Device address       Register Address    Data
            //0xD8,                0x0206,             0x90
            {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_02,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
        };

        //Enable MAX96752 B
        RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_On, sizeof(Table_MAXIM_96752_On) / sizeof(Table_MAXIM_96752_On[0]));
#else
        RetVal = ERR_NONE;
#endif   //MAXIM_TFT1280768_ENABLE_GPIO
    } else {
        RetVal = ERR_NA;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_MAXIMDisable
 *
 *  @Description:: Disable FPD panel device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_MAXIMDisable(void)
{
    UINT32 RetVal = ERR_NONE;
    UINT32 LinkInfoMAX96755 = MAX96755_GetLinkInfo();
    UINT32 LinkInfoMAX96752A = MAX96752_A_GetLinkInfo();
    UINT32 LinkInfoMAX96752B = MAX96752_B_GetLinkInfo();

    if ((LinkInfoMAX96755 == SERDES_LINK_OK) && (LinkInfoMAX96752A == SERDES_LINK_OK) && (LinkInfoMAX96752B == SERDES_LINK_OK)) {
#if MAXIM_TFT1280768_ENABLE_GPIO
        const MAXIM_I2C_CTRL_s Table_MAXIM_96752_Off[] = {
            //Set STBYB high(GPIO6) [0x0212]
            //Device address       Register Address    Data
            //0xD4,                0x0212,             0x90
            //0xD8,                0x0212,             0x90
            {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_06,    0x80,    0     },
            {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_06,    0x80,    50    },

            //Backlight on (GPIO2) [0x0206]
            //Device address       Register Address    Data
            //0xD4,                0x0206,             0x90
            //0xD8,                0x0206,             0x90
            {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_02,    0x80,    0     },
            {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_02,    0x80,    80    },

            //Set RSTB high(GPIO7)  [0x0215]
            //Device address       Register Address    Data
            //0xD4,                0x0215,             0x90,
            //0xD8,                0x0215,             0x90,
            {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_07,    0x80,    0      },
            {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_07,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
        };

        //Disable MAX96752 A and B
        RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_Off, sizeof(Table_MAXIM_96752_Off) / sizeof(Table_MAXIM_96752_Off[0]));
#else
        RetVal = ERR_NONE;
#endif    //MAXIM_TFT1280768_ENABLE_GPIO
    } else if ((LinkInfoMAX96755 == SERDES_LINK_OK) && (LinkInfoMAX96752A == SERDES_LINK_OK)) {
#if MAXIM_TFT1280768_ENABLE_GPIO
        const MAXIM_I2C_CTRL_s Table_MAXIM_96752_Off[] = {
            //Set STBYB high(GPIO6) [0x0212]
            //Device address       Register Address    Data
            //0xD4,                0x0212,             0x90
            {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_06,    0x80,    50    },

            //Backlight on (GPIO2) [0x0206]
            //Device address       Register Address    Data
            //0xD4,                0x0206,             0x90
            {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_02,    0x80,    80    },

            //Set RSTB high(GPIO7)  [0x0215]
            //Device address       Register Address    Data
            //0xD4,                0x0215,             0x90,
            {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_07,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
        };

        //Disable MAX96752 A
        RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_Off, sizeof(Table_MAXIM_96752_Off) / sizeof(Table_MAXIM_96752_Off[0]));
#else
        RetVal = ERR_NONE;
#endif    //MAXIM_TFT1280768_ENABLE_GPIO
    } else if ((LinkInfoMAX96755 == SERDES_LINK_OK) && (LinkInfoMAX96752B == SERDES_LINK_OK)) {
#if MAXIM_TFT1280768_ENABLE_GPIO
        const MAXIM_I2C_CTRL_s Table_MAXIM_96752_Off[] = {
            //Set STBYB high(GPIO6) [0x0212]
            //Device address       Register Address    Data
            //0xD8,                0x0212,             0x90
            {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_06,    0x80,    50    },

            //Backlight on (GPIO2) [0x0206]
            //Device address       Register Address    Data
            //0xD8,                0x0206,             0x90
            {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_02,    0x80,    80    },

            //Set RSTB high(GPIO7)  [0x0215]
            //Device address       Register Address    Data
            //0xD8,                0x0215,             0x90,
            {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_07,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
        };

        //Disable MAX96752 B
        RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_Off, sizeof(Table_MAXIM_96752_Off) / sizeof(Table_MAXIM_96752_Off[0]));
#else
        RetVal = ERR_NONE;
#endif    //MAXIM_TFT1280768_ENABLE_GPIO
    } else {
        RetVal = ERR_NA;
    }

    if (RetVal == ERR_NONE) {
        RetVal = AmbaVout_MipiDsiSetPhyCtrl(VOUT_PHY_MIPI_DPHY_POWER_DOWN, 0U);
    }

#if MAXIM_TFT1280768_ENABLE_TURNOFF_RESET
    if (RetVal == ERR_NONE) {
        //Reset MAX96752 A
        //Device address    Register Address    Data
        //0xD4,             0x0010,             0x91,
        RetVal = MAXIM_RegWrite(MAXIM_TFT1280768_96752_A_I2C_CHANNEL, MAXIM_TFT1280768_96752_A_I2C_ADDR, 0x0010, 0x91);
        (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96752);

        //Reset MAX96752 B
        if (RetVal == (UINT32)ERR_NONE) {
            //Device address    Register Address    Data
            //0xD8,             0x0010,             0x91,
            RetVal = MAXIM_RegWrite(MAXIM_TFT1280768_96752_B_I2C_CHANNEL, MAXIM_TFT1280768_96752_B_I2C_ADDR, 0x0010, 0x91);
            (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96752);
        }

        //Reset MAX96755
        if (RetVal == (UINT32)ERR_NONE) {
            //Device address    Register Address    Data
            //0xC0,             0x0010,             0x91,
            RetVal = MAXIM_RegWrite(MAXIM_TFT1280768_96755_A_I2C_CHANNEL, MAXIM_TFT1280768_96755_A_I2C_ADDR, 0x0010, 0x91);
            (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96755);
        }
    }
#endif    //MAXIM_TFT1280768_ENABLE_TURNOFF_RESET

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_MAXIMGetInfo
 *
 *  @Description:: Get vout configuration for current FPD display mode
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      pInfo:      pointer to FPD display mode info
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_MAXIMGetInfo(AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;
    UINT8 pRxData = 0U;

    if ((pDispConfig == NULL)) {
        RetVal = ERR_ARG;
    } else if (MAXIM_RegRead(MAXIM_TFT1280768_96755_A_I2C_CHANNEL, MAXIM_TFT1280768_96755_A_I2C_ADDR, MAXIM_REG_DEV_ID, &pRxData) != (UINT32)ERR_NONE) {
        RetVal = ERR_ARG;
    } else {
        pInfo->Width = pDispConfig->DisplayTiming.ActivePixels;
        pInfo->Height = pDispConfig->DisplayTiming.ActiveLines;
        pInfo->AspectRatio.X = 16U;
        pInfo->AspectRatio.Y = 9U;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_MAXIMGetModeInfo
 *
 *  @Description:: Get vout configuration for specific mode
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      pInfo:      pointer to FPD display mode info
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_MAXIMGetModeInfo(UINT8 mode, AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;
    UINT8 pRxData = 0U;

    if ( (mode >= (UINT8)AMBA_FPD_MAXIM_SERDES_NUM_MODE) || (pInfo == NULL)) {
        RetVal = ERR_ARG;
    } else if (MAXIM_RegRead(MAXIM_TFT1280768_96755_A_I2C_CHANNEL, MAXIM_TFT1280768_96755_A_I2C_ADDR, MAXIM_REG_DEV_ID, &pRxData) != (UINT32)ERR_NONE) {
        RetVal = ERR_ARG;
    } else {
        pDispConfig = &MAXIM_TFT1280768_Config[mode];
        pInfo->Width = pDispConfig->DisplayTiming.ActivePixels;
        pInfo->Height = pDispConfig->DisplayTiming.ActiveLines;
        pInfo->AspectRatio.X = 16U;
        pInfo->AspectRatio.Y = 9U;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_MAXIMConfig
 *
 *  @Description:: Configure FPD display mode
 *
 *  @Input      ::
 *      pFpdConfig: configuration of FPD display mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_MAXIMConfig(UINT8 Mode)
{
    UINT32 RetVal = ERR_NONE;
    UINT32 ColorOrder = 0U;
    UINT8 RawMode = MAXIM_GetRawMode(Mode);
    UINT8 LINK_CFG = 0U;

    if (RawMode >= AMBA_FPD_MAXIM_SERDES_NUM_MODE) {
        RetVal = ERR_ARG;
    } else {
        pDispConfig = &MAXIM_TFT1280768_Config[RawMode];

        /*-----------------------------------------------------------------------*\
        * Configure serializer and Deserializer's registers by I2C command
        \*-----------------------------------------------------------------------*/

        if (MAXIM_GetNoResetMode(Mode) == 0U) {
            //Reset MAX96755
            //Device address    Register Address    Data
            //0xC0,             0x0010,             0x91,
            RetVal = MAXIM_RegWrite(MAXIM_TFT1280768_96755_A_I2C_CHANNEL, MAXIM_TFT1280768_96755_A_I2C_ADDR, 0x0010, 0x91);
            (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96755);
        } else {
            RetVal = ERR_NONE;
        }

        //Get the setting for the link configuration
        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = MAXIM_RegRead(MAXIM_TFT1280768_96755_A_I2C_CHANNEL, MAXIM_TFT1280768_96755_A_I2C_ADDR, 0x0010, &LINK_CFG);
            //MAX96755 CTRL0 LINK_CFG [1:0] Register Address is 0x0010[1:0]
            LINK_CFG = LINK_CFG & 0x03U;
        }

        if (RetVal == (UINT32)ERR_NONE) {
            switch (RawMode) {
            case AMBA_FPD_MAXIM_DUAL_60HZ:
            case AMBA_FPD_MAXIM_DUAL_A60HZ:
                //If the setting is the same, do not configure it again.
                if (LINK_CFG != 0x03U) {
                    //Set Dual link
                    //Device address    Register Address    Data
                    //0xC0,             0x0010,             0x33,
                    RetVal = MAXIM_RegWrite(MAXIM_TFT1280768_96755_A_I2C_CHANNEL, MAXIM_TFT1280768_96755_A_I2C_ADDR, 0x0010, 0x33);
                    (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96755);
                }
                break;
            case AMBA_FPD_MAXIM_SINGLE_B_60HZ:
            case AMBA_FPD_MAXIM_SINGLE_B_A60HZ:
                //If the setting is the same, do not configure it again.
                if (LINK_CFG != 0x02U) {
                    //Set Single link
                    //Device address    Register Address    Data
                    //0xC0,             0x0010,             0x22,
                    RetVal = MAXIM_RegWrite(MAXIM_TFT1280768_96755_A_I2C_CHANNEL, MAXIM_TFT1280768_96755_A_I2C_ADDR, 0x0010, 0x22);
                    (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96755);
                }
                break;
            case AMBA_FPD_MAXIM_SINGLE_A_60HZ:
            case AMBA_FPD_MAXIM_SINGLE_A_A60HZ:
            default:
                //If the setting is the same, do not configure it again.
                if (LINK_CFG != 0x01U) {
                    //Set Single link
                    //Device address    Register Address    Data
                    //0xC0,             0x0010,             0x31,
                    RetVal = MAXIM_RegWrite(MAXIM_TFT1280768_96755_A_I2C_CHANNEL, MAXIM_TFT1280768_96755_A_I2C_ADDR, 0x0010, 0x31);
                    (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96755);
                }
                break;
            }
        }

        //If one link is not connected,Set Auto link to get another link
        if (RetVal == (UINT32)ERR_NONE) {
            UINT32 LinkInfoMAX96752A = MAX96752_A_GetLinkInfo();
            UINT32 LinkInfoMAX96752B = MAX96752_B_GetLinkInfo();

            if (!(LinkInfoMAX96752A == SERDES_LINK_OK) && !(LinkInfoMAX96752B == SERDES_LINK_OK)) {
                //Set Auto link
                //Device address    Register Address    Data
                //0xC0,             0x0010,             0x31,
                RetVal = MAXIM_RegWrite(MAXIM_TFT1280768_96755_A_I2C_CHANNEL, MAXIM_TFT1280768_96755_A_I2C_ADDR, 0x0010, 0x31);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96755);
            }
        }

        //Reset MAX96752 A
        if (RetVal == (UINT32)ERR_NONE) {
            if (MAXIM_GetNoResetMode(Mode) == 0U) {
                if (MAX96752_A_GetLinkInfo() == SERDES_LINK_OK) {
                    //Device address    Register Address    Data
                    //0xD4,             0x0010,             0x91,
                    RetVal = MAXIM_RegWrite(MAXIM_TFT1280768_96752_A_I2C_CHANNEL, MAXIM_TFT1280768_96752_A_I2C_ADDR, 0x0010, 0x91);
                    (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96752);
                }
            }
        }

        //Reset MAX96752 B
        if (RetVal == (UINT32)ERR_NONE) {
            if (MAXIM_GetNoResetMode(Mode) == 0U) {
                if (MAX96752_B_GetLinkInfo() == SERDES_LINK_OK) {
                    //Device address    Register Address    Data
                    //0xD8,             0x0010,             0x91,
                    RetVal = MAXIM_RegWrite(MAXIM_TFT1280768_96752_B_I2C_CHANNEL, MAXIM_TFT1280768_96752_B_I2C_ADDR, 0x0010, 0x91);
                    (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96752);
                }
            }
        }

        //Config. MAX96755
        if (RetVal == (UINT32)ERR_NONE) {
            const MAXIM_I2C_CTRL_s Table_MAXIM_96755[] = {
                //------------------Serializer MIPI Setup-----------------------
                // Enable 1x4 mode Port A
                // number of lanes
                // Lane mapping straight mapping
                //Device address       Register Address    Data
                //0xC0,                0x0330,             0x04,
                {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x0330,    0x04,    MAXIM_I2C_DELAY_COMMAND    },

                //Device address       Register Address    Data
                //0xC0,                0x0332,             0x4E,
                {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x0332,    0x4E,    MAXIM_I2C_DELAY_COMMAND    },

                //Device address       Register Address    Data
                //0xC0,                0x0333,             0xE4,
                {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x0333,    0xE4,    MAXIM_I2C_DELAY_COMMAND    },

                //Disable PRBS test mode
                //0xC0,                0x0370,             0x08,
                {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x0370,    0x08,    MAXIM_I2C_DELAY_COMMAND    },
            };

            RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96755, sizeof(Table_MAXIM_96755) / sizeof(Table_MAXIM_96755[0]));
        }

        //Config. MAX96755 MIPI Lane setting
        if (RetVal == (UINT32)ERR_NONE) {
            UINT32 OutputMode = pDispConfig->OutputMode;
            switch (OutputMode) {
            case VOUT_MIPI_DSI_MODE_888_1LANE:
                //Device address    Register Address    Data
                //0xC0,             0x0331,             0x30,
                RetVal = MAXIM_RegWrite(MAXIM_TFT1280768_96755_A_I2C_CHANNEL, MAXIM_TFT1280768_96755_A_I2C_ADDR, 0x0331, 0x30);
                break;
            case VOUT_MIPI_DSI_MODE_888_2LANE:
                //Device address    Register Address    Data
                //0xC0,             0x0331,             0x31,
                RetVal = MAXIM_RegWrite(MAXIM_TFT1280768_96755_A_I2C_CHANNEL, MAXIM_TFT1280768_96755_A_I2C_ADDR, 0x0331, 0x31);
                break;
            case VOUT_MIPI_DSI_MODE_888_4LANE:
            default:
                //Device address    Register Address    Data
                //0xC0,             0x0331,             0x33,
                RetVal = MAXIM_RegWrite(MAXIM_TFT1280768_96755_A_I2C_CHANNEL, MAXIM_TFT1280768_96755_A_I2C_ADDR, 0x0331, 0x33);
                break;
            }

            (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_COMMAND);
        }

        //Config. MAX96755 View Setting
        if (RetVal == (UINT32)ERR_NONE) {
            switch (RawMode) {
            case AMBA_FPD_MAXIM_DUAL_60HZ:
            case AMBA_FPD_MAXIM_DUAL_A60HZ:
                if (MAX96755_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Table_MAXIM_96755[] = {
                        //------------------MAX96755 Dual View Setting-----------------------
                        // VID_TX_EN_X = 1, VID_TX_EN_X = 1
                        //Device address       Register Address    Data
                        //0xC0,                0x0002,             0x33,
                        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x0002,    0x33,    MAXIM_I2C_DELAY_COMMAND    },

                        // MAX96755 - DV0
                        //Device address       Register Address    Data
                        //0xC0,                0x032A,             0x07,
                        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x032A,    0x07,    MAXIM_I2C_DELAY_COMMAND    },

                        //Device address       Register Address    Data
                        //0xC0,                0x032C,             0x60,
                        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x032C,    0x60,    MAXIM_I2C_DELAY_COMMAND    },

                        // MAX96755 - TX3
                        //Device address       Register Address    Data
                        //0xC0,                0x0053,             0x30,
                        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x0053,    0x30,    MAXIM_I2C_DELAY_COMMAND    },

                        //Device address       Register Address    Data
                        //0xC0,                0x0057,             0x31,
                        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x0057,    0x31,    MAXIM_I2C_DELAY_COMMAND    },

                        //0xC0,                0x005B,             0x32,
                        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x005B,    0x32,    MAXIM_I2C_DELAY_COMMAND    },

                        //0xC0,                0x005F,             0x33,
                        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x005F,    0x33,    MAXIM_I2C_DELAY_COMMAND    },

                        //0xC0,                0x0311,             0x03,
                        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x0311,    0x03,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96755, sizeof(Table_MAXIM_96755) / sizeof(Table_MAXIM_96755[0]));
                }
                break;
            case AMBA_FPD_MAXIM_SINGLE_A_60HZ:
            case AMBA_FPD_MAXIM_SINGLE_A_A60HZ:
            case AMBA_FPD_MAXIM_SINGLE_B_60HZ:
            case AMBA_FPD_MAXIM_SINGLE_B_A60HZ:
            default:
                if (MAX96755_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Table_MAXIM_96755[] = {
                        //------------------MAX96755 Dual View Setting-----------------------
                        // VID_TX_EN_X = 1, VID_TX_EN_X = 1
                        //Device address       Register Address    Data
                        //0xC0,                0x0002,             0x53,
                        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x0002,    0x53,    MAXIM_I2C_DELAY_COMMAND    },

                        // MAX96755 - DV0
                        //Device address       Register Address    Data
                        //0xC0,                0x032A,             0x04,
                        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x032A,    0x04,    MAXIM_I2C_DELAY_COMMAND    },

                        //Device address       Register Address    Data
                        //0xC0,                0x032C,             0x00,
                        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x032C,    0x00,    MAXIM_I2C_DELAY_COMMAND    },

                        // MAX96755 - TX3
                        //Device address       Register Address    Data
                        //0xC0,                0x0053,             0x10,
                        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x0053,    0x10,    MAXIM_I2C_DELAY_COMMAND    },

                        //Device address       Register Address    Data
                        //0xC0,                0x0057,             0x11,
                        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x0057,    0x11,    MAXIM_I2C_DELAY_COMMAND    },

                        //0xC0,                0x005B,             0x12,
                        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x005B,    0x12,    MAXIM_I2C_DELAY_COMMAND    },

                        //0xC0,                0x005F,             0x13,
                        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x005F,    0x13,    MAXIM_I2C_DELAY_COMMAND    },

                        //0xC0,                0x0311,             0x55,
                        {MAXIM_TFT1280768_96755_A_I2C_CHANNEL,    MAXIM_TFT1280768_96755_A_I2C_ADDR,    0x0311,    0x55,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96755, sizeof(Table_MAXIM_96755) / sizeof(Table_MAXIM_96755[0]));
                }
                break;
            }
        }

        if (RetVal == (UINT32)ERR_NONE) {
            switch (RawMode) {
            case AMBA_FPD_MAXIM_DUAL_60HZ:
            case AMBA_FPD_MAXIM_DUAL_A60HZ:
                //Config. MAX96752 A View Setting
                if (MAX96752_A_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Table_MAXIM_96752[] = {
                        //MAX96752 Link A oLDI setup and steam ID setup
                        //Set HS84 video stream selects
                        //Device address       Register Address    Data
                        //0xD4,                0x0050,             0x00,
                        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0050,    0x00,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752, sizeof(Table_MAXIM_96752) / sizeof(Table_MAXIM_96752[0]));
                }

                //Config. MAX96752 B View Setting
                if (MAX96752_B_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Table_MAXIM_96752[] = {
                        //MAX96752 Link B oLDI setup and steam ID setup
                        //Set HS84 video stream selects
                        //Device address       Register Address    Data
                        //0xD8,                0x0050,             0x01,
                        {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    0x0050,    0x01,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752, sizeof(Table_MAXIM_96752) / sizeof(Table_MAXIM_96752[0]));
                }
                break;
            case AMBA_FPD_MAXIM_SINGLE_A_60HZ:
            case AMBA_FPD_MAXIM_SINGLE_A_A60HZ:
            case AMBA_FPD_MAXIM_SINGLE_B_60HZ:
            case AMBA_FPD_MAXIM_SINGLE_B_A60HZ:
            default:
                //Config. MAX96752 A View Setting
                if (MAX96752_A_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Table_MAXIM_96752[] = {
                        //MAX96752 Link A oLDI setup and steam ID setup
                        //Set HS84 video stream selects
                        //Device address       Register Address    Data
                        //0xD4,                0x0050,             0x00,
                        {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0050,    0x00,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752, sizeof(Table_MAXIM_96752) / sizeof(Table_MAXIM_96752[0]));
                }

                //Config. MAX96752 B View Setting
                if (MAX96752_B_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Table_MAXIM_96752[] = {
                        //MAX96752 Link B oLDI setup and steam ID setup
                        //Set HS84 video stream selects
                        //Device address       Register Address    Data
                        //0xD8,                0x0050,             0x0,
                        {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    0x0050,    0x00,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752, sizeof(Table_MAXIM_96752) / sizeof(Table_MAXIM_96752[0]));
                }
                break;
            }
        }

        //Config. MAX96752 A
        if (RetVal == (UINT32)ERR_NONE) {
            if (MAX96752_A_GetLinkInfo() == SERDES_LINK_OK) {
                const MAXIM_I2C_CTRL_s Table_MAXIM_96752[] = {
                    //set up deserializers oLDI output
                    //Device address       Register Address    Data
                    //0xD4,                0x01CE,             0x5F,
                    {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x01CE,    MAXIM_TFT1280768_OLDI_SETTING,    MAXIM_I2C_DELAY_COMMAND    },

                    //Audio transmit disable for GPIO6 and GPIO7
                    //Device address       Register Address    Data
                    //0xD4,                0x0002,             0x43,
                    {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0002,    0x43,    MAXIM_I2C_DELAY_COMMAND    },

                    //Device address       Register Address    Data
                    //0xD4,                0x0103,             0x47,
                    {MAXIM_TFT1280768_96752_A_I2C_CHANNEL,    MAXIM_TFT1280768_96752_A_I2C_ADDR,    0x0103,    0x47,    MAXIM_I2C_DELAY_COMMAND    },
                };

                RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752, sizeof(Table_MAXIM_96752) / sizeof(Table_MAXIM_96752[0]));
            }
        }

        //Config. MAX96752 B
        if (RetVal == (UINT32)ERR_NONE) {
            if (MAX96752_B_GetLinkInfo() == SERDES_LINK_OK) {
                const MAXIM_I2C_CTRL_s Table_MAXIM_96752[] = {
                    //set up deserializers oLDI output
                    //Device address       Register Address    Data
                    //0xD8,                0x01CE,             0x5F,
                    {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    0x01CE,    MAXIM_TFT1280768_OLDI_SETTING,    MAXIM_I2C_DELAY_COMMAND    },

                    //Audio transmit disable for GPIO6 and GPIO7
                    //Device address       Register Address    Data
                    //0xD8,                0x0002,             0x43,
                    {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    0x0002,    0x43,    MAXIM_I2C_DELAY_COMMAND    },

                    //Device address       Register Address    Data
                    //0xD8,                0x0103,             0x47,
                    {MAXIM_TFT1280768_96752_B_I2C_CHANNEL,    MAXIM_TFT1280768_96752_B_I2C_ADDR,    0x0103,    0x47,    MAXIM_I2C_DELAY_COMMAND    },
                };

                RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752, sizeof(Table_MAXIM_96752) / sizeof(Table_MAXIM_96752[0]));
            }
        }

#if (MAXIM_TFT1280768_GPIO_FORWARD_MODE == MAXIM_GPIO_FORWARD_KEY) || (MAXIM_TFT1280768_GPIO_FORWARD_MODE == MAXIM_GPIO_FORWARD_PWM_AND_KEY)
        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = MAXIM_ConfigGpioForward();
        }
#endif

        //Config. MIPI DSI BLANK PACKET
        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaVout_MipiDsiSetBlankPkt(&pDispConfig->BlankPacket);
        }

        //Enable MIPI DSI
        if (RetVal == (UINT32)ERR_NONE) {
            UINT32 OutputMode = pDispConfig->OutputMode;
            AmbaVout_MipiSetDphyContClk(&OutputMode);
            AmbaVout_MipiEnableEotpMode(&OutputMode);
            RetVal = AmbaVout_MipiDsiEnable(OutputMode, ColorOrder, &pDispConfig->DisplayTiming);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_MAXIMSetBacklight
 *
 *  @Description:: Turn FPD Backlight On/Off
 *
 *  @Input      ::
 *      EnableFlag: 1 = Backlight On, 0 = Backlight Off
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_MAXIMSetBacklight(UINT32 EnableFlag)
{
#if MAXIM_TFT1280768_GPIO_FORWARD_MODE == MAXIM_GPIO_FORWARD_PWM_AND_KEY
    UINT32 RetVal = ERR_NONE;

    if (EnableFlag == 1U) {
        //LCD Brightness PWM
        //CV25DK_GPIO99 ---> MAX96755_GPIO18 ---> MAX96752_GPIO08
        RetVal = AmbaGPIO_SetFuncAlt(GPIO_PIN_99_PWM7);

        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaPWM_Config(AMBA_PWM_CHANNEL7, 1000U);
        }

        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaPWM_Start(AMBA_PWM_CHANNEL7, 10000U, 9999U);
        }
    } else {
        //LCD Brightness PWM
        //CV25DK_GPIO99 ---> MAX96755_GPIO18 ---> MAX96752_GPIO08
        RetVal = AmbaGPIO_SetFuncAlt(GPIO_PIN_99_PWM7);

        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaPWM_Config(AMBA_PWM_CHANNEL7, 1000U);
        }

        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaPWM_Start(AMBA_PWM_CHANNEL7, 10000U, 0U);
        }
    }

    return RetVal;
#else
    extern void AmbaUserGPIO_FpdCtrl(UINT32 FpdFlag);
    AmbaUserGPIO_FpdCtrl(EnableFlag);

    return ERR_NONE;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_MAXIMGetSinkStatus
 *
 *  @Description:: Get the link status of MAXIM vout Serdes
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      pLinkStatus:      pointer to Link Status
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_MAXIMGetSinkStatus(UINT32 *pLinkStatus)
{
    UINT32 RetVal = ERR_NONE;

    if ( pLinkStatus == NULL ) {
        RetVal = ERR_ARG;
    } else {
        *pLinkStatus = 0U;

        if (MAX96755_GetLinkInfo() == SERDES_LINK_OK) {
            *pLinkStatus |= LINK_STATUS_VOU0_MAX96755_A;
        } else {
            *pLinkStatus &= ~LINK_STATUS_VOU0_MAX96755_A;
        }

        if (MAX96752_A_GetLinkInfo() == SERDES_LINK_OK) {
            *pLinkStatus |= LINK_STATUS_VOU0_MAX96752_A;
        } else {
            *pLinkStatus &= ~LINK_STATUS_VOU0_MAX96752_A;
        }

        if (MAX96752_B_GetLinkInfo() == SERDES_LINK_OK) {
            *pLinkStatus |= LINK_STATUS_VOU0_MAX96752_B;
        } else {
            *pLinkStatus &= ~LINK_STATUS_VOU0_MAX96752_B;
        }

        if (MAX96755_GetMipiDsiRxCRCInfo() == MIPI_DSI_CRC_NONE) {
            *pLinkStatus |= LINK_MIPI_DSI_CRC_ERR_MAX96755;
        } else {
            *pLinkStatus &= ~LINK_MIPI_DSI_CRC_ERR_MAX96755;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 * FPD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_FPD_OBJECT_s AmbaFPD_MAXIM_TFT128Obj = {
    .FpdEnable          = FPD_MAXIMEnable,
    .FpdDisable         = FPD_MAXIMDisable,
    .FpdGetInfo         = FPD_MAXIMGetInfo,
    .FpdGetModeInfo     = FPD_MAXIMGetModeInfo,
    .FpdConfig          = FPD_MAXIMConfig,
    .FpdSetBacklight    = FPD_MAXIMSetBacklight,
    .FpdGetLinkStatus   = FPD_MAXIMGetSinkStatus,

    .pName = "MAXIM TFT1280768"
};
