/**
 *  @file AmbaFPD_MAXIM_Dual_ZS095BH.h
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
 *  @details Definitions & Constants for DZXtech FPD panel MAXIM ZS095BH APIs.
 *
 */

#ifndef AMBA_FPD_MAXIM_DUAL_ZS095BH_H
#define AMBA_FPD_MAXIM_DUAL_ZS095BH_H

/*-----------------------------------------------------------------------------------------------*\
 * MAXIM serdes defination
\*-----------------------------------------------------------------------------------------------*/
#define MAXIM_Dual_ZS_ENABLE_GPIO                 1U
#define MAXIM_Dual_ZS_ENABLE_IOKEY                0U
#define MAXIM_Dual_ZS_GPIO_FORWARD_MODE           MAXIM_DUAL_GPIO_FORWARD_DISABLE
#define MAXIM_Dual_ZS_OLDI_SETTING                0x47
#define MAXIM_Dual_ZS_VIEWMODE_DEFAULT            MAXIM_SINGLEVIEW_AUTO_60HZ
#define MAXIM_Dual_ZS_96751_A_I2C_ADDR            0x00000088
#define MAXIM_Dual_ZS_96752_A_I2C_ADDR            0x000000D0
#define MAXIM_Dual_ZS_96752_B_I2C_ADDR            0x000000D4
#define MAXIM_Dual_ZS_A_BL_PWM_IO                 GPIO_PIN_12_PWM0
#define MAXIM_Dual_ZS_B_BL_PWM_IO                 GPIO_PIN_20_PWM9
#define MAXIM_Dual_ZS_A_BL_PWM_CHANNEL            AMBA_PWM_CHANNEL0
#define MAXIM_Dual_ZS_B_BL_PWM_CHANNEL            AMBA_PWM_CHANNEL9

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaFPD_MAXIM_Dual_ZS095BH.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_FPD_OBJECT_s AmbaFPD_MAXIM_Dual_ZS095BHObj;
#if MAXIM_Dual_ZS_ENABLE_IOKEY
extern UINT32 MAXIM_Dual_ZS095BH_InitIOKey(void);
extern UINT32 MAXIM_Dual_ZS095BH_GetIOKeyInfo(UINT8 pIOKeyMode, UINT8 *pIOKeyInfo);
#endif

#endif /* AMBA_FPD_MAXIM_DUAL_ZS095BH_H */
