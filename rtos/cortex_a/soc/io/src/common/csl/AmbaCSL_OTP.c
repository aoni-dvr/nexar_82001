/**
 *  @file AmbaCSL_USB_PHY.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details OTP PHY Control CSL
 *
 */

#include <AmbaTypes.h>
#include <AmbaCSL_OTP.h>
#include <AmbaIOUtility.h>

#if defined(CONFIG_SOC_CV2)

// CV2 configuration

#if defined(CONFIG_BUILD_SSP_OTP) || defined(CONFIG_QNX)

#define OTP_CTRL1_REG_OFFSET  ((UINT32)0x760U)
#define OTP_OBSV_REG_OFFSET   ((UINT32)0x768U)
#define OTP_DATA0_REG_OFFSET  ((UINT32)0x76CU)
static UINT64 otp_base_addr = 0xED080000U; // RCT register

#endif // #if defined(CONFIG_BUILD_SSP_OTP) || defined(CONFIG_QNX)

#elif defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)

// CV22/CV25/CV28/H32 configuration

#if defined(CONFIG_BUILD_SSP_OTP) || defined(CONFIG_QNX)

#define OTP_CTRL1_REG_OFFSET  ((UINT32)0xA0U)
#define OTP_OBSV_REG_OFFSET   ((UINT32)0xA4U)
#define OTP_DATA0_REG_OFFSET  ((UINT32)0xA8U)

static UINT64 otp_base_addr = 0xE8001000U;

#endif // #if defined(CONFIG_BUILD_SSP_OTP) || defined(CONFIG_QNX)

#elif defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)

// CV2FS configuration

#define OTP_CTRL1_REG_OFFSET  ((UINT32)0xA0U)
#define OTP_OBSV_REG_OFFSET   ((UINT32)0xA4U)
#define OTP_DATA0_REG_OFFSET  ((UINT32)0xA8U)
#define OTP_DATA1_REG_OFFSET  ((UINT32)0xB8U)
static UINT64 otp_base_addr = 0xE002F000U; // Secure Scratchpad register

#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)

// CV5 configuration

#define OTP_CTRL1_REG_OFFSET  ((UINT32)0xA0U)
#define OTP_OBSV_REG_OFFSET   ((UINT32)0xA4U)
#define OTP_DATA0_REG_OFFSET  ((UINT32)0xA8U)

static UINT64 otp_base_addr = 0x20E002F000U; // Secure Scratchpad register

#else
#error "Chip does not support OTP program/dump"
#endif

#if defined(CONFIG_BUILD_SSP_OTP) || defined(CONFIG_QNX) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52) || defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)

void   AmbaCSL_OtpBaseAddressSet(UINT64 Address)
{
    otp_base_addr = Address;
}

UINT32 AmbaCSL_OtpCtrl1Read(void)
{
    UINT32 uret = IO_UtilityRegRead32(otp_base_addr + OTP_CTRL1_REG_OFFSET);
    return uret;
}

UINT32 AmbaCSL_OtpObsvRead(void)
{
    UINT32 uret = IO_UtilityRegRead32(otp_base_addr + OTP_OBSV_REG_OFFSET);
    return uret;
}

UINT32 AmbaCSL_OtpDataRead(void)
{
    UINT32 uret = IO_UtilityRegRead32(otp_base_addr + OTP_DATA0_REG_OFFSET);
    return uret;
}

void AmbaCSL_OtpCtrl1Write(UINT32 Value)
{
    IO_UtilityRegWrite32(otp_base_addr + OTP_CTRL1_REG_OFFSET, Value);
}

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
UINT32 AmbaCSL_OtpCrc8Read(void)
{
    UINT32 uret = IO_UtilityRegRead32(otp_base_addr + OTP_DATA1_REG_OFFSET);
    return uret;
}
#endif // #if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#endif // #if defined(CONFIG_BUILD_SSP_OTP) || defined(CONFIG_QNX) || defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)

