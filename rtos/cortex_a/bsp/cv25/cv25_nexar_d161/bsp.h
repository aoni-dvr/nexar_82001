/**
 *  @file bsp.h
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
 *  @details Header of Board Support Package
 *
 */

#ifndef BSP_H
#define BSP_H


#define BSPNAME "CV25 Nexar"

/* for sensor control */
#define AMBA_SENSOR_SPI_CHANNEL         (AMBA_SPI_MASTER0)
#define AMBA_SENSOR_SPI_SLAVE_ID        (0U)

#define AMBA_SENSOR_I2C_CHANNEL         (AMBA_I2C_CHANNEL0)
#define AMBA_SENSOR_I2C_CHANNEL_PIP     (AMBA_I2C_CHANNEL1)
#define AMBA_SENSOR_I2C_CHANNEL_PIP2    (AMBA_I2C_CHANNEL3)

#define AMBA_SENSOR_MSYNC_CHANNEL       (AMBA_VIN_MSYNC0)
#define AMBA_SENSOR_MSYNC_CHANNEL_PIP   (AMBA_VIN_MSYNC0)
#define AMBA_SENSOR_MSYNC_CHANNEL_PIP2  (AMBA_VIN_MSYNC0)

/* for YUV input control */
#define AMBA_YUV_I2C_CHANNEL            (AMBA_I2C_CHANNEL0)
#define AMBA_YUV_I2C_CHANNEL_PIP        (AMBA_I2C_CHANNEL1)
#define AMBA_YUV_I2C_CHANNEL_PIP2       (AMBA_I2C_CHANNEL3)

/* for MAXIM control */
#define AMBA_MAXIM_I2C_CHANNEL          (AMBA_I2C_CHANNEL0)
#define AMBA_MAXIM_I2C_CHANNEL_PIP      (AMBA_I2C_CHANNEL1)
#define AMBA_MAXIM_I2C_CHANNEL_PIP2     (AMBA_I2C_CHANNEL3)

/* for B6 control */
#define AMBA_B6_SPI_CHANNEL             (AMBA_SPI_MASTER0)
#define AMBA_B6_SPI_SLAVE_ID            (0U)
#define AMBA_B6_SPI_CHANNEL_PIP         (AMBA_SPI_MASTER0)
#define AMBA_B6_SPI_SLAVE_ID_PIP        (1U)

/* USB */
#define AMBA_USB0_EHCI_PWR				(GPIO_PIN_26_USB0_EHCI_PWR)
#define AMBA_USB0_EHCI_OC				(GPIO_PIN_25_USB0_EHCI_OC)

extern void AmbaUserConsole_Write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut);
extern UINT32 AmbaUserShell_Read(UINT32 StringSize, char *StringBuf, UINT32 TimeOut);
extern UINT32 AmbaUserShell_Write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut);
extern UINT32 AmbaUserGPIO_SensorResetCtrl(UINT32 VinID, UINT32 PinLevel);
extern UINT32 AmbaUserGPIO_YuvResetCtrl(UINT32 VinID, UINT32 PinLevel);
extern UINT32 AmbaUserGPIO_SerdesResetCtrl(UINT32 VinID, UINT32 PinLevel);
extern UINT32 AmbaUserGPIO_SerdesPowerCtrl(UINT32 VinID, UINT32 PinLevel);
extern UINT32 AmbaUserVIN_SensorClkCtrl(UINT32 VinID, UINT32 Frequency);
extern UINT32 AmbaMMU_CheckCached(UINT32 StartAddr, UINT32 Size);
extern void AmbaMemProt_Config(UINT32 EnableFlag);
extern void AmbaUser_SetRtosUartEnable(int enable);
extern int AmbaUser_GetRtosUartEnable(void);
extern void AmbaUserSD_PhyCtrl_SetDebug(int enable);
extern int AmbaUserSD_PhyCtrl_GetDebug(void);

#define LED_I2C_SLAVE_ADDR      (0x8A)
#define LED_I2C_CHANNEL         (AMBA_I2C_CHANNEL2)

#ifdef CONFIG_PCBA_DVT
#define IR_I2C_SLAVE_ADDR      (0x68)
#else
#define IR_I2C_SLAVE_ADDR      (0x28)
#endif
#define IR_I2C_CHANNEL         (AMBA_I2C_CHANNEL1)

#define CHARGER_I2C_SLAVE_ADDR      (0xd6)
#define CHARGER_I2C_CHANNEL         (AMBA_I2C_CHANNEL3)

#define MAIN_MCU_I2C_SLAVE_ADDR      (0x0A)
#define MAIN_MCU_I2C_CHANNEL         (AMBA_I2C_CHANNEL2)

#define LTE_MCU_I2C_SLAVE_ADDR      (0x72)
#define LTE_MCU_I2C_CHANNEL         (AMBA_I2C_CHANNEL1)

#define IMU_I2C_CHANNEL      (AMBA_I2C_CHANNEL3)

#define BUTTON_GPIO_PIN             (GPIO_PIN_21)
#define LED_IRQ_PIN                 (GPIO_PIN_15)
#define IMU_IRQ_PIN                 (GPIO_PIN_43)

#endif /* BSP_H */
