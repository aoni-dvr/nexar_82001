/**
 *  @file AmbaFPD_MAXIM_Dual_TFT1280768.h
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
 *  @details Definitions & Constants for DZXtech FPD panel MAXIM TFT1280768 APIs.
 *
 */

#ifndef AMBA_FPD_MAXIM_DUAL_TFT1280768_H
#define AMBA_FPD_MAXIM_DUAL_TFT1280768_H

/*-----------------------------------------------------------------------------------------------*\
 * MAXIM serdes defination
\*-----------------------------------------------------------------------------------------------*/
#define MAXIM_DUAL_TFT128_ENABLE_GPIO                 1U
#define MAXIM_DUAL_TFT128_ENABLE_IOKEY                0U
#define MAXIM_DUAL_TFT128_ENABLE_SHUTDOWN_RESET       0U
#if defined(CONFIG_BSP_CV2DK)
#define MAXIM_DUAL_TFT128_GPIO_FORWARD_MODE           MAXIM_DUAL_GPIO_FORWARD_DISABLE
#else
#define MAXIM_DUAL_TFT128_GPIO_FORWARD_MODE           MAXIM_DUAL_GPIO_FORWARD_DISABLE
#endif
#define MAXIM_DUAL_TFT128_OLDI_SETTING                0x47
#define MAXIM_DUAL_TFT128_VIEWMODE_DEFAULT            MAXIM_MULTIVIEW_SPLITTER_60HZ

#if defined(CONFIG_BSP_CV2DK)
#define MAXIM_DUAL_TFT128_96751_A_I2C_CHANNEL         AMBA_I2C_CHANNEL2
#define MAXIM_DUAL_TFT128_96752_A_I2C_CHANNEL         AMBA_I2C_CHANNEL2
#define MAXIM_DUAL_TFT128_96752_B_I2C_CHANNEL         AMBA_I2C_CHANNEL2
#define MAXIM_DUAL_TFT128_96751_A_I2C_ADDR            0x00000088
#define MAXIM_DUAL_TFT128_96752_A_I2C_ADDR            0x000000D8
#define MAXIM_DUAL_TFT128_96752_B_I2C_ADDR            0x000000D4
#else
#define MAXIM_DUAL_TFT128_96751_A_I2C_CHANNEL         AMBA_I2C_CHANNEL2
#define MAXIM_DUAL_TFT128_96752_A_I2C_CHANNEL         AMBA_I2C_CHANNEL2
#define MAXIM_DUAL_TFT128_96752_B_I2C_CHANNEL         AMBA_I2C_CHANNEL2
#define MAXIM_DUAL_TFT128_96751_A_I2C_ADDR            0x00000088
#define MAXIM_DUAL_TFT128_96752_A_I2C_ADDR            0x00000090
#define MAXIM_DUAL_TFT128_96752_B_I2C_ADDR            0x00000094
#endif

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaFPD_MAXIM_Dual_TFT1280768.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_FPD_OBJECT_s AmbaFPD_MAXIM_Dual_TFT128Obj;
#if MAXIM_DUAL_TFT128_ENABLE_IOKEY
extern UINT32 MAXIM_Dual_TFT128_InitIOKey(void);
extern UINT32 MAXIM_Dual_TFT128_GetIOKeyInfo(UINT8 pIOKeyMode, UINT8 *pIOKeyInfo);
#endif

#endif /* AMBA_FPD_MAXIM_DUAL_TFT1280768_H */
