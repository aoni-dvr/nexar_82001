/**
 *  @file board_sysctrl.c
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
 *  @details board system control APIs
 *
 */

#include "startup.h"
#include "aarch64/aarch64_tlb.h"
#include "board.h"

/*
 * Enable/disable the various system controls.
 * This code is hardware dependent and may have to be changed
 * changed by end users.
 */

uint64_t aarch64_tlb[TLB_SIZE] __attribute__ ((aligned(64 * 1024)));

#if !defined(CONFIG_XEN_SUPPORT_QNX)

aarch64_tlb_t board_tlb[] = {
    {
        .start = AMBA_SDRAM_BANK1_BASE,
        .len   = AMBA_SDRAM_BANK1_SIZE,
        .attr  = (0x4 << 2),
    },
    {
        -1, -1, 0,
    },
};

#else

aarch64_tlb_t board_tlb[] = {
	{ 0x40000000, MEG(256), (0x4 << 2), },	// FIXME! - get this from dtb
	{ -1, -1, 0, },
};

#endif

void
board_mmu_enable()
{
    aarch64_setup_tlb(board_tlb, aarch64_tlb);
    aarch64_enable_mmu((uint64_t)aarch64_tlb);
}

void
board_mmu_disable()
{
    aarch64_disable_mmu();
}

void
board_alignment_check_enable()
{
    aarch64_alignment_check_enable();
}

void
board_alignment_check_disable()
{
    aarch64_alignment_check_disable();
}

void
board_dcache_enable()
{
    aarch64_dcache_enable();
}

void
board_dcache_disable()
{
    aarch64_dcache_disable();
}

void
board_icache_enable()
{
    aarch64_icache_enable();
}

void
board_icache_disable()
{
    aarch64_icache_disable();
}

void
board_enable_caches()
{
    board_icache_enable();
    board_dcache_enable();
}

void
board_disable_caches()
{
    board_icache_disable();
    board_dcache_disable();
}
