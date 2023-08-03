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
#include <AmbaCSL_RNG.h>
#include <AmbaIOUtility.h>


#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
static UINT64 rng_rct_base_addr = 0xED080000U; // RCT register
static UINT64 rng_sp_base_addr = 0xE8001000U; // Secure Scratchpad register

#elif defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
static UINT64 rng_rct_base_addr = 0xED080000U; // RCT register
static UINT64 rng_sp_base_addr = 0xE002F000U; // Secure Scratchpad register

#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
static UINT64 rng_rct_base_addr = 0x20ED080000U; // RCT register
static UINT64 rng_sp_base_addr = 0x20E002F000U; // Secure Scratchpad register

#else
#error "Unknown chip version"

#endif

#define RNG_RCT_CTRL_REG_OFFSET           ((UINT32)0x1A4U)
#define RNG_RCT_CTRL_POWER_DOWN_MASK      ((UINT32)0x01U)

#if defined (CONFIG_SOC_CV2)
#define RNG_SP_CTRL_REG_OFFSET            ((UINT32)0x40U)
#define RNG_SP_DATA0_REG_OFFSET           ((UINT32)0x44U)
#define RNG_SP_DATA1_REG_OFFSET           ((UINT32)0x48U)
#define RNG_SP_DATA2_REG_OFFSET           ((UINT32)0x4CU)
#define RNG_SP_DATA3_REG_OFFSET           ((UINT32)0x50U)
#else
#define RNG_SP_CTRL_REG_OFFSET            ((UINT32)0x00U)
#define RNG_SP_DATA0_REG_OFFSET           ((UINT32)0x04U)
#define RNG_SP_DATA1_REG_OFFSET           ((UINT32)0x08U)
#define RNG_SP_DATA2_REG_OFFSET           ((UINT32)0x0CU)
#define RNG_SP_DATA3_REG_OFFSET           ((UINT32)0x10U)
#define RNG_SP_DATA4_REG_OFFSET           ((UINT32)0xB0U)
#endif
#define RNG_SP_CTRL_SAMPLE_COMPLETE_MASK  ((UINT32)0x01U)
#define RNG_SP_CTRL_START_SAMPLE_MASK     ((UINT32)0x02U)
#define RNG_SP_CTRL_SAMPLE_RATE_MASK      ((UINT32)0x30U)
#define RNG_SP_CTRL_SAMPLE_RATE_SHIFT     ((UINT32)0x4U)


#if defined(CONFIG_QNX)
void AmbaCSL_RngRctBaseAddressSet(UINT64 Address)
{
    rng_rct_base_addr = Address;
}

void AmbaCSL_RngSpBaseAddressSet(UINT64 Address)
{
    rng_sp_base_addr = Address;
}
#endif

void AmbaCSL_RngEnable(void)
{
    UINT64 addr = rng_rct_base_addr + RNG_RCT_CTRL_REG_OFFSET;
    UINT32 value = IO_UtilityRegRead32(addr);
    value = value & (~RNG_RCT_CTRL_POWER_DOWN_MASK);
    IO_UtilityRegWrite32(addr, value);
}
void AmbaCSL_RngDisable(void)
{
    UINT64 addr = rng_rct_base_addr + RNG_RCT_CTRL_REG_OFFSET;
    UINT32 value = IO_UtilityRegRead32(addr);
    value = value | RNG_RCT_CTRL_POWER_DOWN_MASK;
    IO_UtilityRegWrite32(addr, value);
}
void AmbaCSL_SetRngMode(UINT16 mode)
{
    IO_UtilityRegWrite32(rng_rct_base_addr + RNG_RCT_CTRL_REG_OFFSET, mode);
}
void AmbaCSL_RngStart(void)
{
    UINT64 addr = rng_sp_base_addr + RNG_SP_CTRL_REG_OFFSET;
    UINT32 value = IO_UtilityRegRead32(addr);
    value = value | RNG_SP_CTRL_START_SAMPLE_MASK;
    IO_UtilityRegWrite32(addr, value);
    //IO_UtilityRegWrite32(0xE002F000, 0x2);
}
void AmbaCSL_RngCleanStatus(void)
{
    UINT32 value = IO_UtilityRegRead32(rng_sp_base_addr + RNG_SP_CTRL_REG_OFFSET);
    value = value & (~RNG_SP_CTRL_SAMPLE_COMPLETE_MASK);
    IO_UtilityRegWrite32(rng_sp_base_addr + RNG_SP_CTRL_REG_OFFSET, value);
}
UINT32 AmbaCSL_RngIsBusy(void)
{
    UINT32 uret;
    UINT64 addr = rng_sp_base_addr + RNG_SP_CTRL_REG_OFFSET;
    UINT32 value = IO_UtilityRegRead32(addr);

    if ((value & RNG_SP_CTRL_SAMPLE_COMPLETE_MASK) != 0U) {
        uret = 1;
    } else {
        uret = 0;
    }
    return uret;
}
UINT32 AmbaCSL_RngGetData0(void)
{
    UINT32 uret = IO_UtilityRegRead32(rng_sp_base_addr + RNG_SP_DATA0_REG_OFFSET);
    return uret;
}
UINT32 AmbaCSL_RngGetData1(void)
{
    UINT32 uret = IO_UtilityRegRead32(rng_sp_base_addr + RNG_SP_DATA1_REG_OFFSET);
    return uret;
}
UINT32 AmbaCSL_RngGetData2(void)
{
    UINT32 uret = IO_UtilityRegRead32(rng_sp_base_addr + RNG_SP_DATA2_REG_OFFSET);
    return uret;
}
UINT32 AmbaCSL_RngGetData3(void)
{
    UINT32 uret = IO_UtilityRegRead32(rng_sp_base_addr + RNG_SP_DATA3_REG_OFFSET);
    return uret;
}
#if !defined(CONFIG_SOC_CV2) // CV2 only support 4 outputs
UINT32 AmbaCSL_RngGetData4(void)
{
    UINT32 uret = IO_UtilityRegRead32(rng_sp_base_addr + RNG_SP_DATA4_REG_OFFSET);
    return uret;
}
#endif
