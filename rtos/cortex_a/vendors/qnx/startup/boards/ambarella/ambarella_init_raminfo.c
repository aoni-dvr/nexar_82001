/**
 *  @file ambarella_init_raminfo.c
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
 *  @details ram initial APIs
 *
 */

/*
 * init_raminfo.c
 *    Tell syspage about our RAM configuration
 */
#include "startup.h"
#include "board.h"

#ifdef CONFIG_DEVICE_TREE_SUPPORT

#else
static amba_add_typemem(char *name, unsigned size, unsigned align)
{
    struct reserved_ram     *next;

    if(size != 0) {
        next = ws_alloc(sizeof(struct reserved_ram));
        if(next == NULL) {
            crash("No memory for reserved_ram structure.\n");
        }
        next->next = reserved_ram_list;
        reserved_ram_list = next;

        reserved_ram_list->size = size;
        reserved_ram_list->align = align;
        reserved_ram_list->name = name;
        reserved_ram_list->low_region = 0;
    }
}

/* Amba Type memory reserved from bottom of DRAM. The order of amba_add_typemem will from top to bottom */
/* For example, if you want cv memory on the bottom of DRAM. Put cv_schdr in the last of code */
static void amba_init_typemem_info(void)
{

//#if defined(CONFIG_XEN_SUPPORT_QNX)
//	size = MEG(256);  // FIXME - should get this from dtb!
//	addr = 0x40000000;  // FIXME - should also get this from dtb!
//#endif

    amba_add_typemem("flexidag_sys",CONFIG_CV_MEM_SIZE, 0x1000U);
}
#endif

void amba_init_raminfo(void)
{
    paddr_t size = CONFIG_DDR_SIZE - AMBA_SDRAM_BANK1_BASE;
    paddr_t addr = AMBA_SDRAM_BANK1_BASE;

#ifdef CONFIG_DEVICE_TREE_SUPPORT
    fdt_init(CONFIG_DTB_LOADADDR);
    fdt_asinfo();
    init_raminfo_fdt();
#else
    amba_init_typemem_info();
#if defined(CONFIG_XEN_QNX_IMG)
    size = MEG(128);  // FIXME - should get this from dtb!
    addr = 0x40000000;  // FIXME - should also get this from dtb!
#endif

	add_ram(addr, size);
#endif
}

