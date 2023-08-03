/**
 *  @file AmbaB8_GPIO.h
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
 *  @details Definitions & Constants for GPIO APIs
 *
 */

#ifndef AMBA_B8_GPIO_H
#define AMBA_B8_GPIO_H

static inline UINT32 B8_GPIO_SET_PIN_FUNC(UINT32 PinNo, UINT32 FuncNo)
{
    return ((FuncNo << 8U) | PinNo);
}
static inline UINT32 B8_GPIO_GET_ALT_FUNC(UINT32 PinID)
{
    return (((PinID) >> 8U) & 0x7U);
}

/* GPIO PIN ID */
#define B8_GPIO_PIN0                0U
#define B8_GPIO_PIN1                1U
#define B8_GPIO_PIN2                2U
#define B8_GPIO_PIN3                3U
#define B8_GPIO_PIN4                4U
#define B8_GPIO_PIN5                5U
#define B8_GPIO_PIN6                6U
#define B8_GPIO_PIN7                7U
#define B8_GPIO_PIN8                8U
#define B8_GPIO_PIN9                9U
#define B8_GPIO_PIN10               10U
#define B8_GPIO_PIN11               11U
#define B8_GPIO_PIN12               12U
#define B8_GPIO_PIN13               13U
#define B8_GPIO_PIN14               14U
#define B8_GPIO_PIN15               15U
#define B8_GPIO_PIN16               16U
#define B8_GPIO_PIN17               17U
#define B8_GPIO_PIN18               18U
#define B8_GPIO_PIN19               19U
#define B8_GPIO_PIN20               20U
#define B8_GPIO_PIN21               21U
#define B8_NUM_GPIO_PIN             31U

#define B8_GPIO_PIN4_I2C0_SCL2      (((UINT32)3U << 8U) | B8_GPIO_PIN4)
#define B8_GPIO_PIN5_I2C0_SDA2      (((UINT32)3U << 8U) | B8_GPIO_PIN5)
#define B8_GPIO_PIN9_I2C0_SCL       (((UINT32)2U << 8U) | B8_GPIO_PIN9)
#define B8_GPIO_PIN10_I2C0_SDA      (((UINT32)2U << 8U) | B8_GPIO_PIN10)

#define B8_GPIO_PIN4_SPI_CLK        (((UINT32)2U << 8U) | B8_GPIO_PIN4)
#define B8_GPIO_PIN5_SPI_EN0        (((UINT32)2U << 8U) | B8_GPIO_PIN5)
#define B8_GPIO_PIN6_SPI_EN1        (((UINT32)2U << 8U) | B8_GPIO_PIN6)
#define B8_GPIO_PIN7_SPI_MOSI       (((UINT32)2U << 8U) | B8_GPIO_PIN7)
#define B8_GPIO_PIN8_SPI_MISO       (((UINT32)2U << 8U) | B8_GPIO_PIN8)

#define B8_GPIO_PIN9_UART_RX        (((UINT32)3U << 8U) | B8_GPIO_PIN9)
#define B8_GPIO_PIN10_UART_TX       (((UINT32)3U << 8U) | B8_GPIO_PIN10)
#define B8_GPIO_PIN15_UART_CLK      (((UINT32)5U << 8U) | B8_GPIO_PIN15)

#define B8_GPIO_PIN11_VIN_VSYNC0    (((UINT32)1U << 8U) | B8_GPIO_PIN11)
#define B8_GPIO_PIN12_VIN_HSYNC0    (((UINT32)1U << 8U) | B8_GPIO_PIN12)
#define B8_GPIO_PIN13_VIN_VSYNC1    (((UINT32)1U << 8U) | B8_GPIO_PIN13)
#define B8_GPIO_PIN14_VIN_HSYNC1    (((UINT32)1U << 8U) | B8_GPIO_PIN14)

#define B8_GPIO_PIN13_PWM0          (((UINT32)2U << 8U) | B8_GPIO_PIN13)
#define B8_GPIO_PIN14_PWM1          (((UINT32)2U << 8U) | B8_GPIO_PIN14)

#define B8_GPIO_PIN15_IRQ           (((UINT32)1U << 8U) | B8_GPIO_PIN15)
#define B8_GPIO_PIN16_VSYNC_EXT     (((UINT32)1U << 8U) | B8_GPIO_PIN16)

#define B8_GPIO_PIN18_CFG_SPI_CLK   (((UINT32)2U << 8U) | B8_GPIO_PIN18)
#define B8_GPIO_PIN19_CFG_SPI_EN    (((UINT32)2U << 8U) | B8_GPIO_PIN19)
#define B8_GPIO_PIN20_CFG_SPI_MOSI  (((UINT32)2U << 8U) | B8_GPIO_PIN20)
#define B8_GPIO_PIN21_CFG_SPI_MISO  (((UINT32)2U << 8U) | B8_GPIO_PIN21)

/* pin level */
#define B8_GPIO_LEVEL_LOW           0U  /* Pin level state is low */
#define B8_GPIO_LEVEL_HIGH          1U  /* Pin level state is high */
#define B8_GPIO_LEVEL_UNKNOWN       2U  /* Pin level state is unknown (due to pin mask is disabled) */

/* pim config (alt function) */
#define B8_GPIO_CONFIG_INPUT        0U  /* Pin is configured as input */
#define B8_GPIO_CONFIG_OUTPUT       1U  /* Pin is configured as output */
#define B8_GPIO_CONFIG_ALTERNATE    2U  /* Pin is configured as alternate function */

typedef struct {
    UINT32  Config;  /* defined as MBA_B8_GPIO_CONFIG_xxx */
    UINT32  Level;   /* defined as B8_GPIO_LEVEL_xxx */
} B8_GPIO_PIN_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaB8_GPIO.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_GpioPinMuxInit(UINT32 ChipID, const B8_PIN_CONFIG_s *pPinConfig);
#if 0 /* TBD */
int AmbaB8D_GpioPinMuxInit(UINT32 ChipID, AMBA_B8D_PIN_CONFIG_s *pPinConfig);
int AmbaB8AD_GpioPinMuxInit(UINT32 ChipID, AMBA_B8D_PIN_CONFIG_s *pPinConfig);
#endif
UINT32 AmbaB8_GpioSetInput(UINT32 ChipID, UINT32 PinID);
UINT32 AmbaB8_GpioSetOutput(UINT32 ChipID, UINT32 PinID, UINT32 PinLevel);
UINT32 AmbaB8_GpioSetAltFunc(UINT32 ChipID, UINT32 PinID);

UINT32 AmbaB8_GpioGetPinInfo(UINT32 ChipID, UINT32 PinID, B8_GPIO_PIN_INFO_s *pPinInfo);

#endif /* AMBA_B8_GPIO_H */
