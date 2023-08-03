/**
 *  @file AmbaFPD_MAXIM_Dual_ZS095BH.c
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
 *  @details Control APIs of DZXtech Display FPD panel MAXIM_Dual_ZS095BH
 *
 */

#include "AmbaTypes.h"

#include "AmbaKAL.h"
#include "AmbaI2C.h"
#include "AmbaFPD.h"
#include "AmbaHDMI.h"
#include "bsp.h"
#include "AmbaFPD_MAXIM_Define.h"
#include "AmbaFPD_MAXIM_Dual_ZS095BH.h"
#if MAXIM_Dual_ZS_GPIO_FORWARD_MODE == MAXIM_DUAL_GPIO_FORWARD_PWM
#include "AmbaGPIO.h"
#include "AmbaPWM.h"
#endif

static AMBA_HDMI_VIDEO_CONFIG_s *pDispConfig;

/*-----------------------------------------------------------------------------------------------*\
 * MAXIM video mode info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_HDMI_VIDEO_CONFIG_s MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_DUAL_SERDES_NUM_MODE] = {
    [AMBA_FPD_MAXIM_SINGLEVIEW_60HZ] = {
        .VideoIdCode              = HDMI_VIC_CUSTOM_NORMAL,
        .PixelFormat              = AMBA_HDMI_PIXEL_FORMAT_RGB_444,
        .ColorDepth               = AMBA_HDMI_COLOR_DEPTH_24BPP,
        .QuantRange               = AMBA_HDMI_QRANGE_DEFAULT,
        .CustomDTD = {
            .PixelClock           = 63063,                                      /* Unit: KHz, frame rate 59.94 */
            .HsyncFrontPorch      = 67,                                         /* Horizontal synchronization front porch */
            .HsyncPulseWidth      = 3,                                          /* Horizontal synchronization pulse width */
            .HsyncBackPorch       = 12,                                         /* Horizontal synchronization back porch */
            .VsyncFrontPorch      = 10,                                         /* Vertical synchronization front porch */
            .VsyncPulseWidth      = 3,                                          /* Vertical synchronization pulse width */
            .VsyncBackPorch       = 32,                                         /* Vertical synchronization back porch */
            .ActivePixels         = 1920,                                       /* Unit: cycles */
            .ActiveLines          = 480,                                        /* Unit: lines */
            .FieldRate            = 5U,                                         /* Field rate for informational purposes only */
            .SyncPolarity         = 1U,                                         /* Polarity of H Sync and V Sync */
            .ScanFormat           = 0U,                                         /* Video scanning format */
            .PixelRepeat          = 0U,
        }
    },
    [AMBA_FPD_MAXIM_MULTIVIEW_60HZ] = {
        .VideoIdCode              = HDMI_VIC_CUSTOM_NORMAL,
        .PixelFormat              = AMBA_HDMI_PIXEL_FORMAT_RGB_444,
        .ColorDepth               = AMBA_HDMI_COLOR_DEPTH_24BPP,
        .QuantRange               = AMBA_HDMI_QRANGE_DEFAULT,
        .CustomDTD = {
            .PixelClock           = 126126,                                     /* Unit: KHz, frame rate 59.94 */
            .HsyncFrontPorch      = 134,                                        /* Horizontal synchronization front porch */
            .HsyncPulseWidth      = 6,                                          /* Horizontal synchronization pulse width */
            .HsyncBackPorch       = 24,                                         /* Horizontal synchronization back porch */
            .VsyncFrontPorch      = 10,                                         /* Vertical synchronization front porch */
            .VsyncPulseWidth      = 3,                                          /* Vertical synchronization pulse width */
            .VsyncBackPorch       = 32,                                         /* Vertical synchronization back porch */
            .ActivePixels         = 3840,                                       /* Unit: cycles */
            .ActiveLines          = 480,                                        /* Unit: lines */
            .FieldRate            = 5U,                                         /* Field rate for informational purposes only */
            .SyncPolarity         = 1U,                                         /* Polarity of H Sync and V Sync */
            .ScanFormat           = 0U,                                         /* Video scanning format */
            .PixelRepeat          = 0U,
        }
    },
    [AMBA_FPD_MAXIM_SINGLEVIEW_A60HZ] = {
        .VideoIdCode              = HDMI_VIC_CUSTOM_STRICT,
        .PixelFormat              = AMBA_HDMI_PIXEL_FORMAT_RGB_444,
        .ColorDepth               = AMBA_HDMI_COLOR_DEPTH_24BPP,
        .QuantRange               = AMBA_HDMI_QRANGE_DEFAULT,
        .CustomDTD = {
            .PixelClock           = 63063,                                      /* Unit: KHz, frame rate 60.00 */
            .HsyncFrontPorch      = 67,                                         /* Horizontal synchronization front porch */
            .HsyncPulseWidth      = 3,                                          /* Horizontal synchronization pulse width */
            .HsyncBackPorch       = 12,                                         /* Horizontal synchronization back porch */
            .VsyncFrontPorch      = 10,                                         /* Vertical synchronization front porch */
            .VsyncPulseWidth      = 3,                                          /* Vertical synchronization pulse width */
            .VsyncBackPorch       = 32,                                         /* Vertical synchronization back porch */
            .ActivePixels         = 1920,                                       /* Unit: cycles */
            .ActiveLines          = 480,                                        /* Unit: lines */
            .FieldRate            = 5U,                                         /* Field rate for informational purposes only */
            .SyncPolarity         = 1U,                                         /* Polarity of H Sync and V Sync */
            .ScanFormat           = 0U,                                         /* Video scanning format */
            .PixelRepeat          = 0U,
        }
    },
    [AMBA_FPD_MAXIM_MULTIVIEW_A60HZ] = {
        .VideoIdCode              = HDMI_VIC_CUSTOM_STRICT,
        .PixelFormat              = AMBA_HDMI_PIXEL_FORMAT_RGB_444,
        .ColorDepth               = AMBA_HDMI_COLOR_DEPTH_24BPP,
        .QuantRange               = AMBA_HDMI_QRANGE_DEFAULT,
        .CustomDTD = {
            .PixelClock           = 126126,                                     /* Unit: KHz, frame rate 60.00 */
            .HsyncFrontPorch      = 134,                                        /* Horizontal synchronization front porch */
            .HsyncPulseWidth      = 6,                                          /* Horizontal synchronization pulse width */
            .HsyncBackPorch       = 24,                                         /* Horizontal synchronization back porch */
            .VsyncFrontPorch      = 10,                                         /* Vertical synchronization front porch */
            .VsyncPulseWidth      = 3,                                          /* Vertical synchronization pulse width */
            .VsyncBackPorch       = 32,                                         /* Vertical synchronization back porch */
            .ActivePixels         = 3840,                                       /* Unit: cycles */
            .ActiveLines          = 480,                                        /* Unit: lines */
            .FieldRate            = 5U,                                         /* Field rate for informational purposes only */
            .SyncPolarity         = 1U,                                         /* Polarity of H Sync and V Sync */
            .ScanFormat           = 0U,                                         /* Video scanning format */
            .PixelRepeat          = 0U,
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
 *  @RoutineName:: MAX96751_A_GetLinkInfo
 *
 *  @Description:: To get link information of MAX96751_A
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MAX96751_A_GetLinkInfo(void)
{
    UINT32 RetVal = ERR_NONE;
    UINT8 pRxData = 0U;

    if (MAXIM_RegRead(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, MAXIM_REG_DEV_ID, &pRxData) == I2C_ERR_NONE) {
        if (pRxData == (UINT8)DEV_ID_MAX96751) {
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
 *  @Description:: To get link information of MAX96752_A
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

    if (MAXIM_RegRead(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96752_A_I2C_ADDR, MAXIM_REG_DEV_ID, &pRxData) == I2C_ERR_NONE) {
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
 *  @Description:: To get link information of MAX96752_B
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MAX96752_B_GetLinkInfo(void)
{
    UINT32 RetVal = ERR_NONE;
    UINT8 pRxData = 0U;

    if (MAXIM_RegRead(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96752_B_I2C_ADDR, MAXIM_REG_DEV_ID, &pRxData) == I2C_ERR_NONE) {
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

#if MAXIM_Dual_ZS_ENABLE_IOKEY
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAXIM_Dual_ZS095BH_InitIOKey
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
UINT32 MAXIM_Dual_ZS095BH_InitIOKey(void)
{
    UINT32 RetVal = ERR_NONE;

    //Initialize MAX96752_A
    if (MAX96752_A_GetLinkInfo() == SERDES_LINK_OK) {
        const MAXIM_I2C_CTRL_s Register_Table_MAXIM_96752_IOKEY_INIT[] = {
            //SW10_GPIO01_ENTER
            //GPIO_R_EN = 1, The GPIO source enabled for GMSL2 reception
            //GPIO_OUT_DIS = 1, Output driver enabled
            //Device address       Register Address    Data
            //0xD0,                0x0203,             0x85,
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_A_REG_IOKEY_ENTER,   0x85,    MAXIM_I2C_DELAY_COMMAND    },

            //SW11_GPIO04_UP
            //GPIO_R_EN = 1, The GPIO source enabled for GMSL2 reception
            //GPIO_OUT_DIS = 1, Output driver enabled
            //Device address       Register Address    Data
            //0xD0,                0x020C,             0x85
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_A_REG_IOKEY_UP,      0x85,    MAXIM_I2C_DELAY_COMMAND    },

            //SW12_GPIO09_DOWN
            //bit2, GPIO_R_EN = 1, The GPIO source enabled for GMSL2 reception
            //bit0, GPIO_OUT_DIS = 1, Output driver enabled
            //Device address       Register Address    Data
            //0xD0,                0x021B,             0x85
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_A_REG_IOKEY_DOWN,    0x85,    MAXIM_I2C_DELAY_COMMAND    },

            //SW13_GPIO13_MENU
            //AUD_EN_RX=0, Audio receiver disabled for GPIO13 changed function mode
            //Device address       Register Address    Data
            //0xD0,                0x0140,             0x00
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x0140,                       0x00,    MAXIM_I2C_DELAY_COMMAND    },

            //bit2, GPIO_R_EN = 1, The GPIO source enabled for GMSL2 reception
            //bit0, GPIO_OUT_DIS = 1, Output driver enabled
            //Device address       Register Address    Data
            //0xD0,                0x0227,             0x85
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_A_REG_IOKEY_MENU,    0x85,    MAXIM_I2C_DELAY_COMMAND    },
        };

        RetVal = MAXIM_RegWriteByTable(Register_Table_MAXIM_96752_IOKEY_INIT, sizeof(Register_Table_MAXIM_96752_IOKEY_INIT) / sizeof(Register_Table_MAXIM_96752_IOKEY_INIT[0]));
    }

    //Initialize MAX96752_B
    if (RetVal == ERR_NONE) {
        if (MAX96752_B_GetLinkInfo() == SERDES_LINK_OK) {
            const MAXIM_I2C_CTRL_s Register_Table_MAXIM_96752_IOKEY_INIT[] = {
                //SW10_GPIO01_ENTER
                //GPIO_R_EN = 1, The GPIO source enabled for GMSL2 reception
                //GPIO_OUT_DIS = 1, Output driver enabled
                //Device address       Register Address    Data
                //0xD4,               0x0203,             0x85,
                {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_B_REG_IOKEY_ENTER,   0x85,    MAXIM_I2C_DELAY_COMMAND    },

                //SW11_GPIO04_UP
                //GPIO_R_EN = 1, The GPIO source enabled for GMSL2 reception
                //GPIO_OUT_DIS = 1, Output driver enabled
                //Device address       Register Address    Data
                //0xD4,                0x020C,             0x85
                {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_B_REG_IOKEY_UP,      0x85,    MAXIM_I2C_DELAY_COMMAND    },

                //SW12_GPIO09_DOWN
                //bit2, GPIO_R_EN = 1, The GPIO source enabled for GMSL2 reception
                //bit0, GPIO_OUT_DIS = 1, Output driver enabled
                //Device address       Register Address    Data
                //0xD4,                0x021B,             0x85
                {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_B_REG_IOKEY_DOWN,    0x85,    MAXIM_I2C_DELAY_COMMAND    },

                //SW13_GPIO13_MENU
                //AUD_EN_RX=0, Audio receiver disabled for GPIO13 changed function mode
                //Device address       Register Address    Data
                //0xD4,                0x0140,             0x00
                {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x0140,                       0x00,    MAXIM_I2C_DELAY_COMMAND    },

                //bit2, GPIO_R_EN = 1, The GPIO source enabled for GMSL2 reception
                //bit0, GPIO_OUT_DIS = 1, Output driver enabled
                //Device address       Register Address    Data
                //0xD4,                0x0227,             0x85
                {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_B_REG_IOKEY_MENU,    0x85,    MAXIM_I2C_DELAY_COMMAND    },
            };

            RetVal = MAXIM_RegWriteByTable(Register_Table_MAXIM_96752_IOKEY_INIT, sizeof(Register_Table_MAXIM_96752_IOKEY_INIT) / sizeof(Register_Table_MAXIM_96752_IOKEY_INIT[0]));
        }
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAXIM_Dual_ZS095BH_GetIOKeyInfo
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
UINT32 MAXIM_Dual_ZS095BH_GetIOKeyInfo(UINT8 pIOKeyMode, UINT8 *pIOKeyInfo)
{
    UINT8 pRxData;
    UINT32 RetVal = ERR_NONE;

    if (pIOKeyMode < MAXIM_DUAL_IOKEY_NUM_MODE) {
        const MAXIM_I2C_CTRL_s Register_Table_MAXIM_96752_IOKEY_READ[MAXIM_DUAL_IOKEY_NUM_MODE] = {
            //SW10_GPIO01_ENTER
            //Device address       Register Address    Data
            //0xD0,                0x0203,             0x00,
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_A_REG_IOKEY_ENTER,   0x00,    MAXIM_I2C_DELAY_COMMAND    },

            //SW11_GPIO04_UP
            //Device address       Register Address    Data
            //0xD0,                0x020C,             0x00
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_A_REG_IOKEY_UP,      0x00,    MAXIM_I2C_DELAY_COMMAND    },

            //SW12_GPIO09_DOWN
            //Device address       Register Address    Data
            //0xD0,                0x021B,             0x00
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_A_REG_IOKEY_DOWN,    0x00,    MAXIM_I2C_DELAY_COMMAND    },

            //SW13_GPIO13_MENU
            //Device address       Register Address    Data
            //0xD0,                0x0227,             0x00
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_A_REG_IOKEY_MENU,    0x00,    MAXIM_I2C_DELAY_COMMAND    },

            //SW10_GPIO01_ENTER
            //Device address       Register Address    Data
            //0xD4,                0x0203,             0x00,
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_B_REG_IOKEY_ENTER,   0x00,    MAXIM_I2C_DELAY_COMMAND    },

            //SW11_GPIO04_UP
            //Device address       Register Address    Data
            //0xD4,                0x020C,             0x00
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_B_REG_IOKEY_UP,      0x00,    MAXIM_I2C_DELAY_COMMAND    },

            //SW12_GPIO09_DOWN
            //Device address       Register Address    Data
            //0xD4,                0x021B,             0x00
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_B_REG_IOKEY_DOWN,    0x00,    MAXIM_I2C_DELAY_COMMAND    },

            //SW13_GPIO13_MENU
            //Device address       Register Address    Data
            //0xD4,                0x0227,             0x00
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_B_REG_IOKEY_MENU,    0x00,    MAXIM_I2C_DELAY_COMMAND    },

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
#endif    //MAXIM_Dual_ZS_ENABLE_IOKEY

#if MAXIM_Dual_ZS_GPIO_FORWARD_MODE == MAXIM_DUAL_GPIO_FORWARD_PWM
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
    UINT32 RetVal = ERR_NONE;

    //configure MAX96752_A GPIO forward
    if (MAX96752_A_GetLinkInfo() == SERDES_LINK_OK) {
        const MAXIM_I2C_CTRL_s Register_Table_MAXIM_CONFIG_GPIO_FORWARD[] = {
            //Left LCD Brightness PWM
            //MAX96751_GPIO06 ---> MAX96752_GPIO08
            //Device address       Register Address    Data
            //0x88,                0x0212,             0xA3,
            //0x88,                0x0213,             0xA2,
            //0x88,                0x0214,             0x01,
            //0x50,                0x0218,             0xA4,
            //0x50,                0x0219,             0xA1,
            //0x50,                0x021A,             0x02,
            //Reset GPIO status
            //0x88,                0x0212,             0x80,
            //0x88,                0x0212,             0x90,
            //0x88,                0x0212,             0xA3,
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x0212,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x0213,    0xA2,    MAXIM_I2C_DELAY_COMMAND    },
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x0214,    0x01,    MAXIM_I2C_DELAY_COMMAND    },
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x0218,    0xA4,    MAXIM_I2C_DELAY_COMMAND    },
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x0219,    0xA1,    MAXIM_I2C_DELAY_COMMAND    },
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x021A,    0x02,    MAXIM_I2C_DELAY_COMMAND    },
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x0212,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x0212,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x0212,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },

            //AUD_EN_RX=0, Audio receiver disabled for GPIO13 changed function mode
            //Device address       Register Address    Data
            //0x50,                0x0140,             0x00,
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x0140,    0x00,    MAXIM_I2C_DELAY_COMMAND    },
        };

        RetVal =  MAXIM_RegWriteByTable(Register_Table_MAXIM_CONFIG_GPIO_FORWARD, sizeof(Register_Table_MAXIM_CONFIG_GPIO_FORWARD) / sizeof(Register_Table_MAXIM_CONFIG_GPIO_FORWARD[0]));
    }

    //configure MAX96752_B GPIO forward
    if (RetVal == ERR_NONE) {
        if (MAX96752_B_GetLinkInfo() == SERDES_LINK_OK) {
            const MAXIM_I2C_CTRL_s Register_Table_MAXIM_CONFIG_GPIO_FORWARD[] = {
                //Right LCD Brightness PWM
                //MAX96751_GPIO10 ---> MAX96752_GPIO08
                //Device address       Register Address    Data
                //0x88,                0x021E,             0xA3,
                //0x88,                0x021F,             0xA4,
                //0x88,                0x0220,             0x03,
                //0x54,                0x0218,             0xA4,
                //0x54,                0x0219,             0xA3,
                //0x54,                0x021A,             0x04,
                //Reset GPIO status
                //0x88,                0x021E,             0x80,
                //0x88,                0x021E,             0x90,
                //0x88,                0x021E,             0xA3,
                {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x021E,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },
                {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x021F,    0xA4,    MAXIM_I2C_DELAY_COMMAND    },
                {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x0220,    0x03,    MAXIM_I2C_DELAY_COMMAND    },
                {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x0218,    0xA4,    MAXIM_I2C_DELAY_COMMAND    },
                {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x0219,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },
                {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x021A,    0x04,    MAXIM_I2C_DELAY_COMMAND    },
                {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x021E,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
                {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x021E,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
                {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x021E,    0xA3,    MAXIM_I2C_DELAY_COMMAND    },

                //AUD_EN_RX=0, Audio receiver disabled for GPIO13 changed function mode
                //Device address       Register Address    Data
                //0x54,                0x0140,             0x00,
                {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x0140,    0x00,    MAXIM_I2C_DELAY_COMMAND    },
            };

            RetVal =  MAXIM_RegWriteByTable(Register_Table_MAXIM_CONFIG_GPIO_FORWARD, sizeof(Register_Table_MAXIM_CONFIG_GPIO_FORWARD) / sizeof(Register_Table_MAXIM_CONFIG_GPIO_FORWARD[0]));
        }
    }

    return RetVal;
}
#endif    //MAXIM_Dual_ZS_GPIO_FORWARD_MODE

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
#if MAXIM_Dual_ZS_ENABLE_GPIO
    UINT32 RetVal = ERR_NONE;
    UINT32 LinkInfoA = MAX96752_A_GetLinkInfo();
    UINT32 LinkInfoB = MAX96752_B_GetLinkInfo();

    if ((LinkInfoA == SERDES_LINK_OK) && (LinkInfoB == SERDES_LINK_OK)) {
        //Enable MAX96752_A
        const MAXIM_I2C_CTRL_s Table_MAXIM_96752_On[] = {
            //Set RSTB high(GPIO7)  [0x0215]
            //Device address       Register Address    Data
            //0xD0,                0x0215,             0x90,
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_07,    0x90,    0    },

            //Set RSTB high(GPIO7)  [0x0215]
            //Device address       Register Address    Data
            //0xD4,                0x0215,             0x90,
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_07,    0x90,    1    },

            //Set STBYB high(GPIO6) [0x0212]
            //Device address       Register Address    Data
            //0xD0,                0x0212,             0x90
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_06,    0x90,    0    },

            //Set STBYB high(GPIO6) [0x0212]
            //Device address       Register Address    Data
            //0xD4,                0x0212,             0x90
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_06,    0x90,    130    },

            //Backlight on (GPIO2) [0x0206]
            //Device address       Register Address    Data
            //0xD0,                0x0206,             0x90
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_02,    0x90,    0    },

            //Backlight on (GPIO2) [0x0206]
            //Device address       Register Address    Data
            //0xD4,                0x0206,             0x90
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_02,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
        };

        RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_On, sizeof(Table_MAXIM_96752_On) / sizeof(Table_MAXIM_96752_On[0]));
    } else if (LinkInfoA == SERDES_LINK_OK) {
        //Enable MAX96752_A
        const MAXIM_I2C_CTRL_s Table_MAXIM_96752_A_On[] = {
            //Set RSTB high(GPIO7)  [0x0215]
            //Device address       Register Address    Data
            //0xD0,                0x0215,             0x90,
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_07,    0x90,    1    },

            //Set STBYB high(GPIO6) [0x0212]
            //Device address       Register Address    Data
            //0xD0,                0x0212,             0x90
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_06,    0x90,    130    },

            //Backlight on (GPIO2) [0x0206]
            //Device address       Register Address    Data
            //0xD0,                0x0206,             0x90
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_02,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
        };

        RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_A_On, sizeof(Table_MAXIM_96752_A_On) / sizeof(Table_MAXIM_96752_A_On[0]));
    } else if (LinkInfoB == SERDES_LINK_OK) {
        //Enable MAX96752_B
        const MAXIM_I2C_CTRL_s Table_MAXIM_96752_B_On[] = {
            //Set RSTB high(GPIO7)  [0x0215]
            //Device address       Register Address    Data
            //0xD4,                0x0215,             0x90,
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_07,    0x90,    1    },

            //Set STBYB high(GPIO6) [0x0212]
            //Device address       Register Address    Data
            //0xD4,                0x0212,             0x90
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_06,    0x90,    130    },

            //Backlight on (GPIO2) [0x0206]
            //Device address       Register Address    Data
            //0xD4,                0x0206,             0x90
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_02,    0x90,    MAXIM_I2C_DELAY_COMMAND    },
        };

        RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_B_On, sizeof(Table_MAXIM_96752_B_On) / sizeof(Table_MAXIM_96752_B_On[0]));
    } else {
        RetVal = ERR_NONE;
    }

    return RetVal;
#else
    return ERR_NONE;
#endif    //MAXIM_Dual_ZS_ENABLE_GPIO
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
#if MAXIM_Dual_ZS_ENABLE_GPIO
    UINT32 RetVal = ERR_NONE;
    UINT32 LinkInfoA = MAX96752_A_GetLinkInfo();
    UINT32 LinkInfoB = MAX96752_B_GetLinkInfo();

    if ((LinkInfoA == SERDES_LINK_OK) && (LinkInfoB == SERDES_LINK_OK)) {
        //Disable MAX96752
        const MAXIM_I2C_CTRL_s Table_MAXIM_96752_Off[] = {
            //Set STBYB high(GPIO6) [0x0212]
            //Device address       Register Address    Data
            //0xD0,                0x0212,             0x90
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_06,    0x80,    0    },

            //Device address       Register Address    Data
            //0xD4,                0x0212,             0x90
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_06,    0x80,    10    },

            //Backlight on (GPIO2) [0x0206]
            //Device address       Register Address    Data
            //0xD0,                0x0206,             0x90
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_02,    0x80,    0    },

            //Device address       Register Address    Data
            //0xD4,                0x0206,             0x90
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_02,    0x80,    10    },

            //Set RSTB high(GPIO7)  [0x0215]
            //Device address       Register Address    Data
            //0xD0,                0x0215,             0x90,
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_07,    0x80,    MAXIM_I2C_DELAY_COMMAND    },

            //Device address       Register Address    Data
            //0xD4,                0x0215,             0x90,
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_07,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
        };

        RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_Off, sizeof(Table_MAXIM_96752_Off) / sizeof(Table_MAXIM_96752_Off[0]));
    } else if (LinkInfoA == SERDES_LINK_OK) {
        //Disable MAX96752_A
        const MAXIM_I2C_CTRL_s Table_MAXIM_96752_A_Off[] = {
            //Set STBYB high(GPIO6) [0x0212]
            //Device address       Register Address    Data
            //0xD0,                0x0212,             0x90
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_06,    0x80,    10    },

            //Backlight on (GPIO2) [0x0206]
            //Device address       Register Address    Data
            //0xD0,                0x0206,             0x90
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_02,    0x80,    10    },

            //Set RSTB high(GPIO7)  [0x0215]
            //Device address       Register Address    Data
            //0xD0,                0x0215,             0x90,
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    MAXIM_REG_GPIO_07,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
        };

        RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_A_Off, sizeof(Table_MAXIM_96752_A_Off) / sizeof(Table_MAXIM_96752_A_Off[0]));
    } else if (LinkInfoB == SERDES_LINK_OK) {
        //Disable MAX96752_B
        const MAXIM_I2C_CTRL_s Table_MAXIM_96752_B_Off[] = {
            //Set STBYB high(GPIO6) [0x0212]
            //Device address       Register Address    Data
            //0xD4,                0x0212,             0x90
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_06,    0x80,    10    },

            //Backlight on (GPIO2) [0x0206]
            //Device address       Register Address    Data
            //0xD4,                0x0206,             0x90
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_02,    0x80,    10    },

            //Set RSTB high(GPIO7)  [0x0215]
            //Device address       Register Address    Data
            //0xD4,                0x0215,             0x90,
            {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    MAXIM_REG_GPIO_07,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
        };

        RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_B_Off, sizeof(Table_MAXIM_96752_B_Off) / sizeof(Table_MAXIM_96752_B_Off[0]));
    } else {
        RetVal = ERR_NONE;
    }

    return RetVal;
#else
    return ERR_NONE;
#endif    //MAXIM_Dual_ZS_ENABLE_GPIO
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

    if ((pDispConfig == NULL)) {
        RetVal = ERR_ARG;
    } else if (MAX96751_A_GetLinkInfo() != SERDES_LINK_OK) {
        RetVal = ERR_ARG;
    } else {
        pInfo->Width = pDispConfig->CustomDTD.ActivePixels;
        pInfo->Height = pDispConfig->CustomDTD.ActiveLines;
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

    if ( (mode >= (UINT8)AMBA_FPD_MAXIM_DUAL_SERDES_NUM_MODE) || (pInfo == NULL)) {
        RetVal = ERR_ARG;
    } else if (MAX96751_A_GetLinkInfo() != SERDES_LINK_OK) {
        RetVal = ERR_ARG;
    } else {
        pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[mode];
        pInfo->Width = pDispConfig->CustomDTD.ActivePixels;
        pInfo->Height = pDispConfig->CustomDTD.ActiveLines;
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

    if (Mode >= MAXIM_STATE_VIEW_NUM) {
        RetVal = ERR_ARG;
    } else {
        /*-----------------------------------------------------------------------*\
        * Configure serializer and Deserializer's registers by I2C command
        \*-----------------------------------------------------------------------*/
        //Reset parts
        //Reset MAX96751
        //Device address    Register Address    Data
        //0x88,             0x0010,             0x91,
        RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x91);
        (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);

        switch (Mode) {
        case MAXIM_MULTIVIEW_A_60Hz:
        case MAXIM_MULTIVIEW_A_A60HZ:
            if (RetVal == ERR_NONE) {
                if (Mode == MAXIM_MULTIVIEW_A_60Hz) {
                    pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_MULTIVIEW_60HZ];
                } else {
                    pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_MULTIVIEW_A60HZ];
                }

                //Device address    Register Address    Data
                //0x88,             0x0010,             0x21,
                RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x21);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);
            }
            break;
        case MAXIM_SINGLEVIEW_A_60HZ:
        case MAXIM_SINGLEVIEW_A_A60HZ:
            if (RetVal == ERR_NONE) {
                if (Mode == MAXIM_SINGLEVIEW_A_60HZ) {
                    pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_SINGLEVIEW_60HZ];
                } else {
                    pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_SINGLEVIEW_A60HZ];
                }

                //Device address    Register Address    Data
                //0x88,             0x0010,             0x21,
                RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x21);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);
            }
            break;
        case MAXIM_MULTIVIEW_B_60Hz:
        case MAXIM_MULTIVIEW_B_A60HZ:
            if (RetVal == ERR_NONE) {
                if (Mode == MAXIM_MULTIVIEW_B_60Hz) {
                    pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_MULTIVIEW_60HZ];
                } else {
                    pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_MULTIVIEW_A60HZ];
                }

                //Device address    Register Address    Data
                //0x88,             0x0010,             0x22,
                RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x22);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);
            }
            break;
        case MAXIM_SINGLEVIEW_B_60HZ:
        case MAXIM_SINGLEVIEW_B_A60HZ:
            if (RetVal == ERR_NONE) {
                if (Mode == MAXIM_SINGLEVIEW_B_60HZ) {
                    pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_SINGLEVIEW_60HZ];
                } else {
                    pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_SINGLEVIEW_A60HZ];
                }

                //Device address    Register Address    Data
                //0x88,             0x0010,             0x22,
                RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x22);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);
            }
            break;
        case MAXIM_SINGLEVIEW_AUTO_60HZ:
        case MAXIM_SINGLEVIEW_AUTO_A60HZ:
            if (RetVal == ERR_NONE) {
                if (Mode == MAXIM_SINGLEVIEW_AUTO_60HZ) {
                    pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_SINGLEVIEW_60HZ];
                } else {
                    pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_SINGLEVIEW_A60HZ];
                }

                //Just reset MAX96751 once
            }
            break;
        case MAXIM_NO_LINK:
            if (RetVal == ERR_NONE) {
                pDispConfig = NULL;

                //Device address    Register Address    Data
                //0x88,             0x0010,             0x20,
                RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x20);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);
            }
            break;
        case MAXIM_MULTIVIEW_SPLITTER_60HZ:
        case MAXIM_MULTIVIEW_SPLITTER_A60HZ:
            //Set Dual link
            if (RetVal == ERR_NONE) {
                if (Mode == MAXIM_MULTIVIEW_SPLITTER_60HZ) {
                    pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_MULTIVIEW_60HZ];
                } else {
                    pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_MULTIVIEW_A60HZ];
                }

                //Device address    Register Address    Data
                //0x88,             0x0010,             0x33,
                RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x33);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);
            }

            //If one link is not connected,Set Auto link to get another link
            if (RetVal == ERR_NONE) {
                if (MAX96752_A_GetLinkInfo() != SERDES_LINK_OK) {
                    //Set Auto link
                    //Device address    Register Address    Data
                    //0x88,             0x0010,             0x91,
                    RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x91);
                    (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);
                }
            }

            if (RetVal == ERR_NONE) {
                if (MAX96752_B_GetLinkInfo() != SERDES_LINK_OK) {
                    //Set Auto link
                    //Device address    Register Address    Data
                    //0x88,             0x0010,             0x91,
                    RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x91);
                    (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);
                }
            }
            break;
        default:
#if (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_MULTIVIEW_A_60Hz) || \
    (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_MULTIVIEW_A_A60HZ)
            if (RetVal == ERR_NONE) {
#if (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_MULTIVIEW_A_60Hz)
                pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_MULTIVIEW_60HZ];
#else
                pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_MULTIVIEW_A60HZ];
#endif

                //Device address    Register Address    Data
                //0x88,             0x0010,             0x21,
                RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x21);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);
            }
#elif (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_SINGLEVIEW_A_60HZ) || \
      (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_SINGLEVIEW_A_A60HZ)
            if (RetVal == ERR_NONE) {
#if (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_SINGLEVIEW_A_60HZ)
                pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_SINGLEVIEW_60HZ];
#else
                pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_SINGLEVIEW_A60HZ];
#endif

                //Device address    Register Address    Data
                //0x88,             0x0010,             0x21,
                RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x21);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);
            }
#elif (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_MULTIVIEW_B_60Hz) || \
      (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_MULTIVIEW_B_A60HZ)
            if (RetVal == ERR_NONE) {
#if (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_MULTIVIEW_B_60Hz)
                pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_MULTIVIEW_60HZ];
#else
                pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_MULTIVIEW_A60HZ];
#endif

                //Device address    Register Address    Data
                //0x88,             0x0010,             0x22,
                RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x22);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);
            }
#elif (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_SINGLEVIEW_B_60HZ) || \
      (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_SINGLEVIEW_B_A60HZ)
            if (RetVal == ERR_NONE) {
#if (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_SINGLEVIEW_B_60HZ)
                pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_SINGLEVIEW_60HZ];
#else
                pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_SINGLEVIEW_A60HZ];
#endif

                //Device address    Register Address    Data
                //0x88,             0x0010,             0x22,
                RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x22);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);
            }
#elif (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_SINGLEVIEW_AUTO_60HZ) || \
      (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_SINGLEVIEW_AUTO_A60HZ)
            if (RetVal == ERR_NONE) {
#if (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_SINGLEVIEW_AUTO_60HZ)
                pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_SINGLEVIEW_60HZ];
#else
                pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_SINGLEVIEW_A60HZ];
#endif

                //Just reset MAX96751 once
            }
#elif (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_NO_LINK)
            if (RetVal == ERR_NONE) {
                pDispConfig = NULL;

                //Device address    Register Address    Data
                //0x88,             0x0010,             0x20,
                RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x20);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);
            }
#else       //(MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_MULTIVIEW_SPLITTER_60HZ) ||
            //(MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_MULTIVIEW_SPLITTER_A60HZ)
            //Set Dual link
            if (RetVal == ERR_NONE) {
#if (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_MULTIVIEW_SPLITTER_60HZ)
                pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_MULTIVIEW_60HZ];
#else
                pDispConfig = &MAXIM_Dual_ZS_Video_Ctrl[AMBA_FPD_MAXIM_MULTIVIEW_A60HZ];
#endif

                //Device address    Register Address    Data
                //0x88,             0x0010,             0x33,
                RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x33);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);
            }

            //If one link is not connected,Set Auto link to get another link
            if (RetVal == ERR_NONE) {
                if (MAX96752_A_GetLinkInfo() != SERDES_LINK_OK) {
                    //Set Auto link
                    //Device address    Register Address    Data
                    //0x88,             0x0010,             0x91,
                    RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x91);
                    (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);
                }
            }

            if (RetVal == ERR_NONE) {
                if (MAX96752_B_GetLinkInfo() != SERDES_LINK_OK) {
                    //Set Auto link
                    //Device address    Register Address    Data
                    //0x88,             0x0010,             0x91,
                    RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96751_A_I2C_ADDR, 0x0010, 0x91);
                    (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96751);
                }
            }
#endif
            break;
        }

        //Reset MAX96752
        if (RetVal == ERR_NONE) {
            UINT32 LinkInfoA = MAX96752_A_GetLinkInfo();
            UINT32 LinkInfoB = MAX96752_B_GetLinkInfo();

            if ((LinkInfoA == SERDES_LINK_OK) && (LinkInfoB == SERDES_LINK_OK)) {
                //Device address    Register Address    Data
                //0xD0,             0x0010,             0x91,
                RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96752_A_I2C_ADDR, 0x0010, 0x91);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96752);

                if (RetVal == ERR_NONE) {
                    //Device address    Register Address    Data
                    //0xD4,             0x0010,             0x91,
                    RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96752_B_I2C_ADDR, 0x0010, 0x91);
                    (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96752);
                }
            } else if (LinkInfoA == SERDES_LINK_OK) {
                //Device address    Register Address    Data
                //0xD0,             0x0010,             0x91,
                RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96752_A_I2C_ADDR, 0x0010, 0x91);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96752);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96752);
            } else if (LinkInfoB == SERDES_LINK_OK) {
                //Device address    Register Address    Data
                //0xD4,             0x0010,             0x91,
                RetVal = MAXIM_RegWrite(AMBA_DISPLAY_I2C_CHANNEL, MAXIM_Dual_ZS_96752_B_I2C_ADDR, 0x0010, 0x91);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96752);
                (void)AmbaKAL_TaskSleep(MAXIM_I2C_DELAY_RESET_MAX96752);
            } else {
                RetVal = ERR_NONE;
            }
        }

        //Config. MAX96751
        if (RetVal == ERR_NONE) {
            if (MAX96751_A_GetLinkInfo() == SERDES_LINK_OK) {
                const MAXIM_I2C_CTRL_s Table_MAXIM_96751[] = {
                    //set up serializer
                    //Device address       Register Address       Data
                    //0x88,                0x0001,                0x88,
                    //0x88,                0x0053,                0x10,
                    //0x88,                0x0057,                0x21,
                    {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x0001,    0x88,    MAXIM_I2C_DELAY_COMMAND    },
                    {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x0053,    0x10,    MAXIM_I2C_DELAY_COMMAND    },
                    {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x0057,    0x21,    MAXIM_I2C_DELAY_COMMAND    },

                    //disable LFLT interrupt on ERRB pin
                    //Device address       Register Address       Data
                    //0x88,                0x001A,                0x03,
                    {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x001A,    0x03,    MAXIM_I2C_DELAY_COMMAND    },

                    //turn on HPD
                    //Device Address       Register Address       Data
                    //0x88,                0x20F5,                0x01,
                    {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x20F5,    0x01,    MAXIM_I2C_DELAY_COMMAND    },
                };

                RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96751, sizeof(Table_MAXIM_96751) / sizeof(Table_MAXIM_96751[0]));
            }
        }

        //Config. MAX96751's Multiview or Singleview
        switch (Mode) {
#if (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_SINGLEVIEW_A_60HZ) || \
    (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_SINGLEVIEW_A_A60HZ) || \
    (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_SINGLEVIEW_B_60HZ) || \
    (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_SINGLEVIEW_B_A60HZ) || \
    (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_SINGLEVIEW_AUTO_60HZ) || \
    (MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_SINGLEVIEW_AUTO_A60HZ)
        case MAXIM_MULTIVIEW_A_60Hz:
        case MAXIM_MULTIVIEW_A_A60HZ:
        case MAXIM_MULTIVIEW_B_60Hz:
        case MAXIM_MULTIVIEW_B_A60HZ:
        case MAXIM_MULTIVIEW_SPLITTER_60HZ:
        case MAXIM_MULTIVIEW_SPLITTER_A60HZ:
            if (RetVal == ERR_NONE) {
                if (MAX96751_A_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Register_Table_MAXIM_96751_A_Multiview[] = {
                        //Dual View Registers
                        //Device address       Register Address    Data
                        //0x88,                0x01A0,             0x87,
                        //0x88,                0x01A1,             0x80,
                        //0x88,                0x01A2,             0x6A,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x01A0,    0x87,    MAXIM_I2C_DELAY_COMMAND    },
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x01A1,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x01A2,    0x6A,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Register_Table_MAXIM_96751_A_Multiview, sizeof(Register_Table_MAXIM_96751_A_Multiview) / sizeof(Register_Table_MAXIM_96751_A_Multiview[0]));
                }
            }

            if (RetVal == ERR_NONE) {
                if (MAX96752_A_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Table_MAXIM_96752_A_Multiview[] = {
                        //set up deserializers oLDI output
                        //Device address         Register Address    Data
                        //0xD0,                  0x01CE,             0x47,
                        {AMBA_DISPLAY_I2C_CHANNEL,  MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x01CE,    MAXIM_Dual_ZS_OLDI_SETTING,    MAXIM_I2C_DELAY_COMMAND    },

                        //Device address         Register Address    Data
                        //0xD0,                  0x0103,             0x47,
                        {AMBA_DISPLAY_I2C_CHANNEL,  MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x0103,    0x47,    MAXIM_I2C_DELAY_COMMAND    },

                        //Set HS84 video stream selects
                        //Device address         Register Address    Data
                        //0xD0,                  0x0050,             0x00,
                        {AMBA_DISPLAY_I2C_CHANNEL,  MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x0050,    0x00,    MAXIM_I2C_DELAY_COMMAND    },

                        //Audio transmit disable for GPIO6 and GPIO7
                        //Device address         Register Address    Data
                        //0xD0,                  0x0002,             0x43,
                        {AMBA_DISPLAY_I2C_CHANNEL,  MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x0002,    0x43,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_A_Multiview, sizeof(Table_MAXIM_96752_A_Multiview) / sizeof(Table_MAXIM_96752_A_Multiview[0]));
                }
            }

            if (RetVal == ERR_NONE ) {
                if (MAX96752_B_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Table_MAXIM_96752_B_Multiview[] = {
                        //set up deserializers oLDI output
                        //Device address       Register Address    Data
                        //0xD4,                0x01CE,             0x47,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x01CE,    MAXIM_Dual_ZS_OLDI_SETTING,    MAXIM_I2C_DELAY_COMMAND    },

                        //Device address       Register Address    Data
                        //0xD4,                0x0103,             0x47,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x0103,    0x47,    MAXIM_I2C_DELAY_COMMAND    },

                        //Set HS84 video stream selects
                        //Device address       Register Address    Data
                        //0xD4,                0x0050,             0x01,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x0050,    0x01,    MAXIM_I2C_DELAY_COMMAND    },

                        //Audio transmit disable for GPIO6 and GPIO7
                        //Device address         Register Address    Data
                        //0xD4,                  0x0002,             0x43,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x0002,    0x43,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_B_Multiview, sizeof(Table_MAXIM_96752_B_Multiview) / sizeof(Table_MAXIM_96752_B_Multiview[0]));
                }
            }

            break;
        case MAXIM_SINGLEVIEW_A_60HZ:
        case MAXIM_SINGLEVIEW_A_A60HZ:
        case MAXIM_SINGLEVIEW_B_60HZ:
        case MAXIM_SINGLEVIEW_B_A60HZ:
        case MAXIM_SINGLEVIEW_AUTO_60HZ:
        case MAXIM_SINGLEVIEW_AUTO_A60HZ:
        default:
            if (RetVal == ERR_NONE) {
                if (MAX96751_A_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Register_Table_MAXIM_96751_A_Singleview[] = {
                        //Single View Registers
                        //Device address       Register Address    Data
                        //0x88,                0x01A0,             0x04,
                        //0x88,                0x01A1,             0x00,
                        //0x88,                0x01A2,             0x20,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x01A0,    0x04,    MAXIM_I2C_DELAY_COMMAND    },
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x01A1,    0x00,    MAXIM_I2C_DELAY_COMMAND    },
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x01A2,    0x20,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Register_Table_MAXIM_96751_A_Singleview, sizeof(Register_Table_MAXIM_96751_A_Singleview) / sizeof(Register_Table_MAXIM_96751_A_Singleview[0]));
                }
            }

            if (RetVal == ERR_NONE) {
                if (MAX96752_A_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Table_MAXIM_96752_A_Singleview[] = {
                        //set up deserializers oLDI output
                        //Device address       Register Address    Data
                        //0xD0,                0x01CE,             0x47,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x01CE,    MAXIM_Dual_ZS_OLDI_SETTING,    MAXIM_I2C_DELAY_COMMAND    },

                        //Device address       Register Address    Data
                        //0xD0,                0x0103,             0x47,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x0103,    0x47,    MAXIM_I2C_DELAY_COMMAND    },

                        //Set HS84 video stream selects
                        //Device address       Register Address    Data
                        //0xD0,                0x0050,             0x00,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x0050,    0x00,    MAXIM_I2C_DELAY_COMMAND    },

                        //Audio transmit disable for GPIO6 and GPIO7
                        //Device address       Register Address    Data
                        //0xD0,                0x0002,             0x43,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x0002,    0x43,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_A_Singleview, sizeof(Table_MAXIM_96752_A_Singleview) / sizeof(Table_MAXIM_96752_A_Singleview[0]));
                }
            }

            if (RetVal == ERR_NONE) {
                if (MAX96752_B_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Table_MAXIM_96752_B_Singleview[] = {
                        //set up deserializers oLDI output
                        //Device address       Register Address    Data
                        //0xD4,                0x01CE,             0x47,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x01CE,    MAXIM_Dual_ZS_OLDI_SETTING,    MAXIM_I2C_DELAY_COMMAND    },

                        //Device address       Register Address    Data
                        //0xD4,                0x0103,             0x47,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x0103,    0x47,    MAXIM_I2C_DELAY_COMMAND    },

                        //Set HS84 video stream selects
                        //Device address       Register Address    Data
                        //0xD4,                0x0050,             0x01,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x0050,    0x00,    MAXIM_I2C_DELAY_COMMAND    },

                        //Audio transmit disable for GPIO6 and GPIO7
                        //Device address       Register Address    Data
                        //0xD4,                0x0002,             0x43,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x0002,    0x43,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_B_Singleview, sizeof(Table_MAXIM_96752_B_Singleview) / sizeof(Table_MAXIM_96752_B_Singleview[0]));
                }
            }

            break;
#else       //(MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_MULTIVIEW_A_60Hz) ||
        //(MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_MULTIVIEW_A_A60HZ) ||
        //(MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_MULTIVIEW_B_60Hz) ||
        //(MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_MULTIVIEW_B_A60HZ) ||
        //(MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_MULTIVIEW_SPLITTER_60HZ) ||
        //(MAXIM_Dual_ZS_VIEWMODE_DEFAULT == MAXIM_MULTIVIEW_SPLITTER_A60HZ)
        case MAXIM_SINGLEVIEW_A_60HZ:
        case MAXIM_SINGLEVIEW_A_A60HZ:
        case MAXIM_SINGLEVIEW_B_60HZ:
        case MAXIM_SINGLEVIEW_B_A60HZ:
        case MAXIM_SINGLEVIEW_AUTO_60HZ:
        case MAXIM_SINGLEVIEW_AUTO_A60HZ:
            if (RetVal == ERR_NONE) {
                if (MAX96751_A_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Register_Table_MAXIM_96751_A_Singleview[] = {
                        //Single View Registers
                        //Device address       Register Address    Data
                        //0x88,                0x01A0,             0x04,
                        //0x88,                0x01A1,             0x00,
                        //0x88,                0x01A2,             0x20,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x01A0,    0x04,    MAXIM_I2C_DELAY_COMMAND    },
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x01A1,    0x00,    MAXIM_I2C_DELAY_COMMAND    },
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x01A2,    0x20,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Register_Table_MAXIM_96751_A_Singleview, sizeof(Register_Table_MAXIM_96751_A_Singleview) / sizeof(Register_Table_MAXIM_96751_A_Singleview[0]));
                }
            }

            if (RetVal == ERR_NONE) {
                if (MAX96752_A_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Table_MAXIM_96752_A_Singleview[] = {
                        //set up deserializers oLDI output
                        //Device address       Register Address    Data
                        //0xD0,                0x01CE,             0x47,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x01CE,    MAXIM_Dual_ZS_OLDI_SETTING,    MAXIM_I2C_DELAY_COMMAND    },

                        //Device address       Register Address    Data
                        //0xD0,                0x0103,             0x47,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x0103,    0x47,    MAXIM_I2C_DELAY_COMMAND    },

                        //Set HS84 video stream selects
                        //Device address       Register Address    Data
                        //0xD0,                0x0050,             0x00,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x0050,    0x00,    MAXIM_I2C_DELAY_COMMAND    },

                        //Audio transmit disable for GPIO6 and GPIO7
                        //Device address       Register Address    Data
                        //0xD0,                0x0002,             0x43,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x0002,    0x43,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_A_Singleview, sizeof(Table_MAXIM_96752_A_Singleview) / sizeof(Table_MAXIM_96752_A_Singleview[0]));
                }
            }

            if (RetVal == ERR_NONE) {
                if (MAX96752_B_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Table_MAXIM_96752_B_Singleview[] = {
                        //set up deserializers oLDI output
                        //Device address       Register Address    Data
                        //0xD4,                0x01CE,             0x47,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x01CE,    MAXIM_Dual_ZS_OLDI_SETTING,    MAXIM_I2C_DELAY_COMMAND    },

                        //Device address       Register Address    Data
                        //0xD4,                0x0103,             0x47,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x0103,    0x47,    MAXIM_I2C_DELAY_COMMAND    },

                        //Set HS84 video stream selects
                        //Device address       Register Address    Data
                        //0xD4,                0x0050,             0x01,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x0050,    0x00,    MAXIM_I2C_DELAY_COMMAND    },

                        //Audio transmit disable for GPIO6 and GPIO7
                        //Device address       Register Address    Data
                        //0xD4,                0x0002,             0x43,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x0002,    0x43,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_B_Singleview, sizeof(Table_MAXIM_96752_B_Singleview) / sizeof(Table_MAXIM_96752_B_Singleview[0]));
                }
            }

            break;
        case MAXIM_MULTIVIEW_A_60Hz:
        case MAXIM_MULTIVIEW_A_A60HZ:
        case MAXIM_MULTIVIEW_B_60Hz:
        case MAXIM_MULTIVIEW_B_A60HZ:
        case MAXIM_MULTIVIEW_SPLITTER_60HZ:
        case MAXIM_MULTIVIEW_SPLITTER_A60HZ:
        default:
            if (RetVal == ERR_NONE) {
                if (MAX96751_A_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Register_Table_MAXIM_96751_A_Multiview[] = {
                        //Dual View Registers
                        //Device address       Register Address    Data
                        //0x88,                0x01A0,             0x87,
                        //0x88,                0x01A1,             0x80,
                        //0x88,                0x01A2,             0x6A,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x01A0,    0x87,    MAXIM_I2C_DELAY_COMMAND    },
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x01A1,    0x80,    MAXIM_I2C_DELAY_COMMAND    },
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96751_A_I2C_ADDR,    0x01A2,    0x6A,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Register_Table_MAXIM_96751_A_Multiview, sizeof(Register_Table_MAXIM_96751_A_Multiview) / sizeof(Register_Table_MAXIM_96751_A_Multiview[0]));
                }
            }

            if (RetVal == ERR_NONE) {
                if (MAX96752_A_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Table_MAXIM_96752_A_Multiview[] = {
                        //set up deserializers oLDI output
                        //Device address         Register Address    Data
                        //0xD0,                  0x01CE,             0x47,
                        {AMBA_DISPLAY_I2C_CHANNEL,  MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x01CE,    MAXIM_Dual_ZS_OLDI_SETTING,    MAXIM_I2C_DELAY_COMMAND    },

                        //Device address         Register Address    Data
                        //0xD0,                  0x0103,             0x47,
                        {AMBA_DISPLAY_I2C_CHANNEL,  MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x0103,    0x47,    MAXIM_I2C_DELAY_COMMAND    },

                        //Set HS84 video stream selects
                        //Device address         Register Address    Data
                        //0xD0,                  0x0050,             0x00,
                        {AMBA_DISPLAY_I2C_CHANNEL,  MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x0050,    0x00,    MAXIM_I2C_DELAY_COMMAND    },

                        //Audio transmit disable for GPIO6 and GPIO7
                        //Device address         Register Address    Data
                        //0xD0,                  0x0002,             0x43,
                        {AMBA_DISPLAY_I2C_CHANNEL,  MAXIM_Dual_ZS_96752_A_I2C_ADDR,    0x0002,    0x43,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_A_Multiview, sizeof(Table_MAXIM_96752_A_Multiview) / sizeof(Table_MAXIM_96752_A_Multiview[0]));
                }
            }

            if (RetVal == ERR_NONE ) {
                if (MAX96752_B_GetLinkInfo() == SERDES_LINK_OK) {
                    const MAXIM_I2C_CTRL_s Table_MAXIM_96752_B_Multiview[] = {
                        //set up deserializers oLDI output
                        //Device address       Register Address    Data
                        //0xD4,                0x01CE,             0x47,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x01CE,    MAXIM_Dual_ZS_OLDI_SETTING,    MAXIM_I2C_DELAY_COMMAND    },

                        //Device address       Register Address    Data
                        //0xD4,                0x0103,             0x47,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x0103,    0x47,    MAXIM_I2C_DELAY_COMMAND    },

                        //Set HS84 video stream selects
                        //Device address       Register Address    Data
                        //0xD4,                0x0050,             0x01,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x0050,    0x01,    MAXIM_I2C_DELAY_COMMAND    },

                        //Audio transmit disable for GPIO6 and GPIO7
                        //Device address         Register Address    Data
                        //0xD4,                  0x0002,             0x43,
                        {AMBA_DISPLAY_I2C_CHANNEL,    MAXIM_Dual_ZS_96752_B_I2C_ADDR,    0x0002,    0x43,    MAXIM_I2C_DELAY_COMMAND    },
                    };

                    RetVal = MAXIM_RegWriteByTable(Table_MAXIM_96752_B_Multiview, sizeof(Table_MAXIM_96752_B_Multiview) / sizeof(Table_MAXIM_96752_B_Multiview[0]));
                }
            }

            break;
#endif
        }

#if MAXIM_Dual_ZS_GPIO_FORWARD_MODE == MAXIM_DUAL_GPIO_FORWARD_PWM
        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = MAXIM_ConfigGpioForward();
        }
#endif

        //Set HDMI mode
        if (RetVal == ERR_NONE) {
            const AMBA_HDMI_AUDIO_CONFIG_s Audio_Ctrl = {
                .SampleRate                 = HDMI_AUDIO_FS_48K,
                .SpeakerMap                 = HDMI_CA_2CH,
            };

            RetVal = AmbaHDMI_TxSetMode(AMBA_HDMI_TX_PORT0, pDispConfig, &Audio_Ctrl);
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
#if MAXIM_Dual_ZS_GPIO_FORWARD_MODE == MAXIM_DUAL_GPIO_FORWARD_PWM
    UINT32 RetVal = ERR_NONE;

    if (EnableFlag == 1U) {
        //Left LCD Brightness PWM
        //MAX96751_GPIO06 ---> MAX96752_GPIO08
        RetVal = AmbaGPIO_SetFuncAlt(MAXIM_Dual_ZS_A_BL_PWM_IO);

        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaPWM_Config(MAXIM_Dual_ZS_A_BL_PWM_CHANNEL, 1000U);
        }
        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaPWM_Start(MAXIM_Dual_ZS_A_BL_PWM_CHANNEL, 10000U, 9999U);
        }

        //Right LCD Brightness PWM
        //MAX96751_GPIO10 ---> MAX96752_GPIO08
        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaGPIO_SetFuncAlt(MAXIM_Dual_ZS_B_BL_PWM_IO);
        }
        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaPWM_Config(MAXIM_Dual_ZS_B_BL_PWM_CHANNEL, 1000U);
        }
        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaPWM_Start(MAXIM_Dual_ZS_B_BL_PWM_CHANNEL, 10000U, 9999U);
        }
    } else {
        //Left LCD Brightness PWM
        //MAX96751_GPIO06 ---> MAX96752_GPIO08
        RetVal = AmbaGPIO_SetFuncAlt(MAXIM_Dual_ZS_A_BL_PWM_IO);

        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaPWM_Config(MAXIM_Dual_ZS_A_BL_PWM_CHANNEL, 1000U);
        }
        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaPWM_Start(MAXIM_Dual_ZS_A_BL_PWM_CHANNEL, 10000U, 0U);
        }

        //Right LCD Brightness PWM
        //MAX96751_GPIO10 ---> MAX96752_GPIO08
        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaGPIO_SetFuncAlt(MAXIM_Dual_ZS_B_BL_PWM_IO);
        }
        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaPWM_Config(MAXIM_Dual_ZS_B_BL_PWM_CHANNEL, 1000U);
        }
        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaPWM_Start(MAXIM_Dual_ZS_B_BL_PWM_CHANNEL, 10000U, 0U);
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

        if (MAX96751_A_GetLinkInfo() == SERDES_LINK_OK) {
            *pLinkStatus |= LINK_STATUS_VOU1_MAX96751_A;
        } else {
            *pLinkStatus &= ~LINK_STATUS_VOU1_MAX96751_A;
        }

        if (MAX96752_A_GetLinkInfo() == SERDES_LINK_OK) {
            *pLinkStatus |= LINK_STATUS_VOU1_MAX96752_A;
        } else {
            *pLinkStatus &= ~LINK_STATUS_VOU1_MAX96752_A;
        }

        if (MAX96752_B_GetLinkInfo() == SERDES_LINK_OK) {
            *pLinkStatus |= LINK_STATUS_VOU1_MAX96752_B;
        } else {
            *pLinkStatus &= ~LINK_STATUS_VOU1_MAX96752_B;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 * FPD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_FPD_OBJECT_s AmbaFPD_MAXIM_Dual_ZS095BHObj = {
    .FpdEnable          = FPD_MAXIMEnable,
    .FpdDisable         = FPD_MAXIMDisable,
    .FpdGetInfo         = FPD_MAXIMGetInfo,
    .FpdGetModeInfo     = FPD_MAXIMGetModeInfo,
    .FpdConfig          = FPD_MAXIMConfig,
    .FpdSetBacklight    = FPD_MAXIMSetBacklight,
    .FpdGetLinkStatus   = FPD_MAXIMGetSinkStatus,

    .pName = "MAXIM Daul ZS095BH"
};
