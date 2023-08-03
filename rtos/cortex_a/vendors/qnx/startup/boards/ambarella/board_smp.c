/**
 *  @file board_smp.c
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
 *  @details SMP system APIs
 *
 */

#include "startup.h"
#include <arm/ambarella.h>

unsigned
board_smp_num_cpu()
{
    /*
     * FIXME_AARCH64: need to figure out how to get
       the number of CPU cores for MPSoC.
     */
#if defined(CONFIG_XEN_SUPPORT_QNX)
    return 1;  // FIXME
#else
    return CONFIG_SMP_CORE_NUM;
#endif
}

void
board_smp_init(struct smp_entry *smp, unsigned num_cpus)
{
    smp->send_ipi = (void *)&sendipi_gic_v2;
}

int
board_smp_start(unsigned cpu, void (*start)(void))
{
    if (debug_flag > 1)
    {
        kprintf("board_smp_start: cpu%d -> %x\n", cpu, start);
    }
    board_mmu_disable();
    psci_smp_start(cpu, start);

    return 1;
}

unsigned
board_smp_adjust_num(unsigned cpu)
{
    return cpu;
}
