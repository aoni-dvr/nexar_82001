/**
 *  @file ambarella_get_clock.c
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
 *  @details Get clock APIs
 *
 */

#include "startup.h"
#include <arm/ambarella.h>

void amba_init_vp_clock(void)
{
    unsigned int delay = 0xfffff;
    uintptr_t base = AMBA_RCT_BASE;
    int pll_offset = 0x6e4;
    int pll_frac_offset = 0x6e8;
    int pll1_offset = 0x6ec;
    int pll2_offset = 0x6f0;

    out32(base+pll_offset, 0x1E100008);
    out32(base+pll_frac_offset, 0x80000000);
    out32(base+pll1_offset, 0x30520000);
    out32(base+pll2_offset, 0x88002);
    out32(base+pll_offset, 0x1E100008 | 0x1);

    while (delay > 0) {
        (delay --);
    }

    out32(base+pll_offset, 0x1E100008 | 0x0);
}

uint32_t Amba_get_vp_clock(void)
{
    uint32_t arm_clock;
    uintptr_t base = AMBA_RCT_BASE;
    int pll_offset = 0x6e4;
    int pll_frac_offset = 0x6e8;
    uint32_t pll_reg;
    uint32_t frac_value;

    pll_reg = in32(base + pll_offset);

    arm_clock = AMBA_REF_CLOCK * (((pll_reg & 0xff000000) >> 24) + 1);

    if ((pll_reg & 0x8) != 0) {
        pll_reg = in32(base + pll_frac_offset);
        frac_value = ((uint64_t)pll_reg * (uint64_t)AMBA_REF_CLOCK) >> 32;
        arm_clock = arm_clock + frac_value;
    }

    if (((pll_reg & 0x00f000) >> 12) != 0) {
        arm_clock = arm_clock * (((pll_reg & 0x00f000) >> 12) + 1);
    }

    if (((pll_reg & 0x00f0000) >> 16) != 0) {
        arm_clock = arm_clock / (((pll_reg & 0x00f0000) >> 16) + 1);
    }

    return arm_clock;
}

uint32_t amba_get_cpu_clk(void)
{
    uint32_t arm_clock;
    uintptr_t base = AMBA_RCT_BASE;
    int pll_offset = 0x264;
    uint32_t pll_reg;

    pll_reg = in32(base + pll_offset);

    arm_clock = AMBA_REF_CLOCK * (((pll_reg & 0xff000000) >> 24) + 1);
    if (((pll_reg & 0x00f000) >> 12) != 0) {
        arm_clock = arm_clock * (((pll_reg & 0x00f000) >> 12) + 1);
    }

    if (((pll_reg & 0x00f0000) >> 16) != 0) {
        arm_clock = arm_clock / (((pll_reg & 0x00f0000) >> 16) + 1);
    }

    return arm_clock;
}
