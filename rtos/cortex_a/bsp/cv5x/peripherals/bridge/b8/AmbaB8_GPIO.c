/**
 *  @file AmbaB8_GPIO.c
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
 *  @details Ambarella B8 GPIO APIs
 *
 */
#include "AmbaB8.h"
#include "AmbaB8_GPIO.h"
#include "AmbaB8_Communicate.h"

#include "AmbaB8CSL_GPIO.h"
#include "AmbaB8CSL_IoMux.h"
#include "AmbaB8CSL_IoCtrl.h"
#include "AmbaB8CSL_PLL.h"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PinMuxInit
 *
 *  @Description:: Configure B8 PinMux
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      pPinConfig: Pointer to B5F PinMux Configure
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_GpioPinMuxInit(UINT32 ChipID, const B8_PIN_CONFIG_s *pPinConfig)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 PinGroup0[3] = {0U};
    UINT32 IoCtrl;
    UINT32 DataBuf32;

    /* default: VYSNC0, HSYNC0, VSYNC1, HSYNC1, CLK_SI, CFG_SPI, I2C_SCL, I2C_SDA */
    PinGroup0[0] = 0x017800U;
    PinGroup0[1] = 0x3e0000U;
    PinGroup0[2] = 0x000000U;

    /* sensor communication intf. */
    if (pPinConfig->SensorPinCtrl == B8_PIN_SENSOR_CTRL_SPI0) {
        /* pin4 = pin5 = pin7 = pin8 = alt2 (spi_clk, spi_en0, spi_mosi, spi_miso) */
        PinGroup0[1] |= ((UINT32)0x1U << (UINT32)4U);
        PinGroup0[1] |= ((UINT32)0x1U << (UINT32)5U);
        PinGroup0[1] |= ((UINT32)0x1U << (UINT32)7U);
        PinGroup0[1] |= ((UINT32)0x1U << (UINT32)8U);

    } else if (pPinConfig->SensorPinCtrl == B8_PIN_SENSOR_CTRL_SPI1) {
        /* pin4 = pin6 = pin7 = pin8 = alt2 (spi_clk, spi_en1, spi_mosi, spi_miso) */
        PinGroup0[1] |= ((UINT32)0x1U << (UINT32)4U);
        PinGroup0[1] |= ((UINT32)0x1U << (UINT32)6U);
        PinGroup0[1] |= ((UINT32)0x1U << (UINT32)7U);
        PinGroup0[1] |= ((UINT32)0x1U << (UINT32)8U);

    } else if (pPinConfig->SensorPinCtrl == B8_PIN_SENSOR_CTRL_I2C0_2) {
        /* pin4 = pin5 = alt3 (i2c0_scl2, i2c0_sda2) */
        PinGroup0[0] |= ((UINT32)0x1U << (UINT32)4U);
        PinGroup0[0] |= ((UINT32)0x1U << (UINT32)5U);
        PinGroup0[1] |= ((UINT32)0x1U << (UINT32)4U);
        PinGroup0[1] |= ((UINT32)0x1U << (UINT32)5U);

    } else {
        /* pin9 = pin10 = alt2 (i2c0_scl, i2c0_sda) */
        PinGroup0[1] |= ((UINT32)0x1U << (UINT32)9U);
        PinGroup0[1] |= ((UINT32)0x1U << (UINT32)10U);
    }
    /* PinMux Configuration */
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_IoMuxReg->PinGroup0FuncSelect0, 1, B8_DATA_WIDTH_32BIT, 3, PinGroup0);

    DataBuf32 = 0x1U;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_IoMuxReg->Enable, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);

    DataBuf32 = 0x0U;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_IoMuxReg->Enable, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);

    /* Set IoCtrl */
    IoCtrl = 0x78000U;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_IoCtrlReg->Ctrl, 0, B8_DATA_WIDTH_32BIT, 1, &IoCtrl);

    /* Enable pll_out_mux */
    DataBuf32 = 0x0U;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->ClkObserve, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_GpioSetInput
 *
 *  @Description:: Set B8D GPIO Output
 *
 *  @Input      ::
 *      ChipID:    B8D chip id
 *      GpioPinID: Pin ID
 *      PinLevel:  Pin Level
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_GpioSetInput(UINT32 ChipID, UINT32 PinID)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 DataBuf32[2];
    UINT32 RegBitPosition;

    if (PinID >= B8_NUM_GPIO_PIN) {
        RetVal = B8_ERR_ARG;
    } else {
        /* Set IOMUX to GPIO Function for this pin */
        (void) AmbaB8_GpioSetAltFunc(ChipID, B8_GPIO_SET_PIN_FUNC(PinID, 0U));

        RegBitPosition = (UINT32)1U << PinID;

        DataBuf32[0] = 0x1;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_GPIOReg->Enable, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Set pin mask to 1 */
        (void) AmbaB8_RegRead(ChipID, & pAmbaB8_GPIOReg->PinMask, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        DataBuf32[0] |= RegBitPosition;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_GPIOReg->PinMask, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Set pin direction to 0 */
        (void) AmbaB8_RegRead(ChipID, & pAmbaB8_GPIOReg->PinDirection, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        DataBuf32[0] &= ~RegBitPosition;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_GPIOReg->PinDirection, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        DataBuf32[0] = 0xffffffffU;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_GPIOReg->IntClear, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_GpioSetOutput
 *
 *  @Description:: Set B8 GPIO Output
 *
 *  @Input      ::
 *      ChipID:    B8 chip id
 *      GpioPinID: Pin ID
 *      PinLevel:  Pin Level
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_GpioSetOutput(UINT32 ChipID, UINT32 PinID, UINT32 PinLevel)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 DataBuf32[2];
    UINT32 RegBitPosition;

    if (PinID >= B8_NUM_GPIO_PIN) {
        RetVal = B8_ERR_ARG;
    } else {
        /* Set IOMUX to GPIO Function for this pin */
        (void) AmbaB8_GpioSetAltFunc(ChipID, B8_GPIO_SET_PIN_FUNC(PinID, 0U));

        RegBitPosition = (UINT32)1U << PinID;

        DataBuf32[0] = 0x1U;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_GPIOReg->Enable, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Set pin mask to 1 */
        (void) AmbaB8_RegRead(ChipID, & pAmbaB8_GPIOReg->PinMask, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        DataBuf32[0] |= RegBitPosition;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_GPIOReg->PinMask, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Set pin direction to 1 */
        (void) AmbaB8_RegRead(ChipID, & pAmbaB8_GPIOReg->PinDirection, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
        DataBuf32[0] |= RegBitPosition;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_GPIOReg->PinDirection, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Set pin level */
        (void) AmbaB8_RegRead(ChipID, & pAmbaB8_GPIOReg->PinLevel, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        if (PinLevel == B8_GPIO_LEVEL_LOW) {
            DataBuf32[0] &= ~RegBitPosition;
        } else if(PinLevel == B8_GPIO_LEVEL_HIGH) {
            DataBuf32[0] |= RegBitPosition;
        } else {
            RetVal = B8_ERR_ARG;
        }

        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_GPIOReg->PinLevel, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        DataBuf32[0] = 0xffffffffU;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_GPIOReg->IntClear, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_GpioSetAltFunc
 *
 *  @Description:: Set pin function of B8
 *
 *  @Input      ::
 *      ChipID:    B8 chip id
 *      PinFunc:   Pin function
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_GpioSetAltFunc(UINT32 ChipID, UINT32 PinID)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 PinNo = (PinID & (UINT32)0xffU);
    UINT32 AltFunc = B8_GPIO_GET_ALT_FUNC(PinID);
    UINT32 DataBuf32[3];
    UINT32 PinMask;
    UINT32 i;

    if (PinNo < 32U) {
        PinMask = (UINT32)1U << PinNo;
        (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_IoMuxReg->PinGroup0FuncSelect0), 1, B8_DATA_WIDTH_32BIT, 3, DataBuf32);

        for (i = 0U; i < 3U; i ++) {
            if ((AltFunc & ((UINT32)1U << i)) != 0U) {
                DataBuf32[i] |= PinMask;
            } else {
                DataBuf32[i] &= ~PinMask;
            }
        }
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_IoMuxReg->PinGroup0FuncSelect0), 1, B8_DATA_WIDTH_32BIT, 3, DataBuf32);
        DataBuf32[0] = 1;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_IoMuxReg->Enable), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
        DataBuf32[0] = 0;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_IoMuxReg->Enable), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_GpioGetPinInfo
 *
 *  @Description:: Query the current GPIO line configuration
 *
 *  @Input      ::
 *      ChipID:    B8D chip id
 *      GpioPinID:  GPIO pin number
 *
 *  @Output     ::
 *      pPinInfo:   GPIO pin current state
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_GpioGetPinInfo(UINT32 ChipID, UINT32 PinID, B8_GPIO_PIN_INFO_s *pPinInfo)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 RegBitPosition = (UINT32)1U << PinID;
    UINT32 DataBuf32[2];

    if (PinID > B8_NUM_GPIO_PIN) {
        RetVal = B8_ERR_ARG;
    } else {
        (void) AmbaB8_RegRead(ChipID, & pAmbaB8_GPIOReg->ModeSelect, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
        if ((DataBuf32[0] & RegBitPosition) != 0U) {
            pPinInfo->Config = B8_GPIO_CONFIG_ALTERNATE;
        } else {
            (void) AmbaB8_RegRead(ChipID, & pAmbaB8_GPIOReg->PinDirection, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
            if ((DataBuf32[0] & RegBitPosition) != 0U) {
                pPinInfo->Config = B8_GPIO_CONFIG_OUTPUT;
            } else {
                pPinInfo->Config = B8_GPIO_CONFIG_INPUT;
            }
        }

        (void) AmbaB8_RegRead(ChipID, & pAmbaB8_GPIOReg->PinMask, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
        if ((DataBuf32[0] & RegBitPosition) == 0U) {
            pPinInfo->Level = B8_GPIO_LEVEL_UNKNOWN;
        } else {
            (void) AmbaB8_RegRead(ChipID, & pAmbaB8_GPIOReg->PinLevel, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
            if ((DataBuf32[0] & RegBitPosition) != 0U) {
                pPinInfo->Level = B8_GPIO_LEVEL_HIGH;
            } else {
                pPinInfo->Level = B8_GPIO_LEVEL_LOW;
            }
        }
    }

    return RetVal;
}


