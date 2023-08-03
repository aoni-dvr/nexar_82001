/**
 *  @file AmbaRTSL_PWC.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details PWC RTSL APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaSYS.h"

#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_PWC.h"
#include "AmbaCSL_PWC.h"
#include "AmbaCSL_RCT.h"
#include "AmbaCSL_USB.h"
#include "AmbaCSL_WDT.h"

/**
 *  AmbaRTSL_PwcReboot - Reboot the system
 */
void AmbaRTSL_PwcReboot(void)
{
    AmbaCSL_WdtClearWdtResetStatus();   /* Clear WDT reset status otherwise it will be kept. */
    AmbaCSL_RctSetUsbHostSoftReset(1U); /* If USB host is turned on, need to reset it to make chip soft reset work */
    AmbaCSL_RctChipSoftReset();
}

/**
 *  AmbaRTSL_PwcWriteScratchpad - Set User-Defined status (4-bit)
 *  @param[in] Status User-Defined status (4-bit)
 */
void AmbaRTSL_PwcWriteScratchpad(UINT32 Status)
{
    if ((Status & 0x1U) != 0U) {
        AmbaCSL_PwcSetScratchpad(PWC_SCRATCHPAD_RESERVED0);
    } else {
        AmbaCSL_PwcClearScratchpad(PWC_SCRATCHPAD_RESERVED0);
    }

    if ((Status & 0x2U) != 0U) {
        AmbaCSL_PwcSetScratchpad(PWC_SCRATCHPAD_RESERVED1);
    } else {
        AmbaCSL_PwcClearScratchpad(PWC_SCRATCHPAD_RESERVED1);
    }

    if ((Status & 0x4U) != 0U) {
        AmbaCSL_PwcSetScratchpad(PWC_SCRATCHPAD_RESERVED2);
    } else {
        AmbaCSL_PwcClearScratchpad(PWC_SCRATCHPAD_RESERVED2);
    }
}

/**
 *  AmbaRTSL_PwcGetBootDevice - Get boot device type
 *  @return Boot device ID
 */
UINT32 AmbaRTSL_PwcGetBootDevice(void)
{
    UINT32 BootDevice;

    if (pAmbaRCT_Reg->SysConfig.UsbBoot != 0U) {
        /* For debug purpose */
#if defined(CONFIG_ENABLE_SPINOR_BOOT)
        BootDevice = AMBA_SYS_BOOT_FROM_SPI_NOR;
#elif defined(CONFIG_ENABLE_SPINAND_BOOT)
        BootDevice = AMBA_SYS_BOOT_FROM_SPI_NAND;
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
        BootDevice = AMBA_SYS_BOOT_FROM_EMMC;
#else
        BootDevice = AMBA_SYS_BOOT_FROM_USB;
#endif
    } else {
        /* For normal case */
        switch (pAmbaRCT_Reg->SysConfig.BootMode) {
        case 0U:
            BootDevice = AMBA_SYS_BOOT_FROM_SPI_NOR;
            break;

        case 1U:
            BootDevice = AMBA_SYS_BOOT_FROM_SPI_NAND;
            break;

        case 2U:
            BootDevice = AMBA_SYS_BOOT_FROM_EMMC;
            break;

        case 3U:
        default:
            BootDevice = 0xFFFFFFFFUL;
            break;
        }
    }

    return BootDevice;
}
