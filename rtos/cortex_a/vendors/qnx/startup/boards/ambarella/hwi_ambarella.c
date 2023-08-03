/**
 *  @file hwi_ambarella.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details HW initial APIs
 *
 */


#include "startup.h"
#include "hwinfo_private.h"
#include <drvr/hwinfo.h>                // for hwi support routines in libdrvr
#include <arm/ambarella.h>
#include "AmbaINT_Def.h"

/*
 * Add MPSoC devices to the hardware info section of the syspage.
*/

void hwi_mpsoc()
{
    unsigned hwi_bus_internal = 0;

    /* add  UART */
    {
        unsigned hwi_off;
        hwiattr_uart_t uart_attr = HWIATTR_UART_T_INITIALIZER;
        struct hwi_inputclk clksrc = {.clk = 24000000, .div = 0};

        /* each UART has an interrupt */
        HWIATTR_UART_SET_NUM_IRQ(&uart_attr, 1);
        HWIATTR_UART_SET_NUM_CLK(&uart_attr, 1);

        /* create uart0 */
        HWIATTR_UART_SET_LOCATION(&uart_attr, AMBA_UART0_BASE, AMBA_UART_SIZE, 0, hwi_find_as(AMBA_UART0_BASE, 1));
        hwi_off = hwidev_add_uart(AMBA_HWI_UART0, &uart_attr, hwi_bus_internal);
        ASSERT(hwi_find_unit(hwi_off) == 0);
        hwitag_set_ivec(hwi_off, 0, AMBA_APB_UART_INT);
        hwitag_set_inputclk(hwi_off, 0, &clksrc);
    }
}

