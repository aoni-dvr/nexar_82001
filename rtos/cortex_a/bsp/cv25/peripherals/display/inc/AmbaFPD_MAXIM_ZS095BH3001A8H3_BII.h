/**
 *  @file AmbaFPD_MAXIM_ZS095BH3001A8H3_BII.h
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
 *  @details Definitions & Constants for DZXtech FPD panel MAXIM ZS095BH3001A8H3 BII APIs.
 *
 */

#ifndef AMBA_FPD_MAXIM_ZS095BH3001A8H3_BII_H
#define AMBA_FPD_MAXIM_ZS095BH3001A8H3_BII_H

/*-----------------------------------------------------------------------------------------------*\
 * MAXIM serdes defination
\*-----------------------------------------------------------------------------------------------*/
#define MAXIM_ZS095_ENABLE_GPIO                 1U
#define MAXIM_ZS095_ENABLE_IOKEY                0U
#define MAXIM_ZS095_ENABLE_TURNOFF_RESET        0U
#if defined(CONFIG_BSP_CV25DK_V100) || defined(CONFIG_BSP_CV25DK_8LAYER_V110)
#define MAXIM_ZS095_GPIO_FORWARD_MODE           MAXIM_GPIO_FORWARD_PWM_AND_KEY
#else
#define MAXIM_ZS095_GPIO_FORWARD_MODE           MAXIM_GPIO_FORWARD_DISABLE
#endif
#define MAXIM_ZS095_OLDI_SETTING                0x47

#if defined(CONFIG_BSP_CV25DK_V100) || defined(CONFIG_BSP_CV25DK_8LAYER_V110)
#define MAXIM_ZS095_96755_A_I2C_CHANNEL         AMBA_I2C_CHANNEL2
#define MAXIM_ZS095_96752_A_I2C_CHANNEL         AMBA_I2C_CHANNEL2
#define MAXIM_ZS095_96752_B_I2C_CHANNEL         AMBA_I2C_CHANNEL2
#define MAXIM_ZS095_96755_A_I2C_ADDR            0x000000C0
#define MAXIM_ZS095_96752_A_I2C_ADDR            0x00000054
#define MAXIM_ZS095_96752_B_I2C_ADDR            0x00000050
#else
#define MAXIM_ZS095_96755_A_I2C_CHANNEL         AMBA_I2C_CHANNEL1
#define MAXIM_ZS095_96752_A_I2C_CHANNEL         AMBA_I2C_CHANNEL1
#define MAXIM_ZS095_96752_B_I2C_CHANNEL         AMBA_I2C_CHANNEL1
#define MAXIM_ZS095_96755_A_I2C_ADDR            0x000000C0
#define MAXIM_ZS095_96752_A_I2C_ADDR            0x000000D4
#define MAXIM_ZS095_96752_B_I2C_ADDR            0x000000D8
#endif

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaFPD_MAXIM_ZS095BH3001A8H3_BII.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_FPD_OBJECT_s AmbaFPD_MAXIM_ZS095Obj;
#if MAXIM_ZS095_ENABLE_IOKEY
extern UINT32 MAXIM_ZS095_InitIOKey(void);
extern UINT32 MAXIM_ZS095_GetIOKeyInfo(UINT8 pIOKeyMode, UINT8 *pIOKeyInfo);
#endif

#endif /* AMBA_FPD_MAXIM_ZS095BH3001A8H3_BII_H */
