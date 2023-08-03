/**
 * @file fdt_boot.c
 *
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @details fdt boot functions are implemented here.
 *
 */
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaLink.h"
#include "AmbaLinkPrivate.h"
#include "AmbaNAND.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "libfdt.h"
#include "AmbaSD.h"
#include "AmbaSD_STD.h"
#include "AmbaSD_Ctrl.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaSpiNOR_Ctrl.h"

#define SIZE_1MB_MASK        (SIZE_1MB - 1)

extern int AmbaLink_ReadPartitionInfo(int, UINT32, AMBA_PARTITION_ENTRY_s *);
extern AMBA_LINK_CTRL_s AmbaLinkCtrl;

void fdt_print_error(const char *str, int err)
{
    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s %s!", str, fdt_strerror(err), NULL, NULL, NULL);
    AmbaPrint_Flush();
}

static int fdtput_chosen(void *fdt, const char *cmdline, UINT32 cpux_jump,
    UINT32 initrd2_start, UINT32 initrd2_size)
{
    int ret_val = 0;
    int offset;

    offset = fdt_path_offset (fdt, "/chosen");
    if (offset < 0) {
        ret_val = offset;
        goto fdtput_chosen_exit;
    }

    if ((cmdline != NULL) && (cmdline[0] != '\0')) {
        ret_val = fdt_setprop_string(fdt, offset, "bootargs", cmdline);
        if (ret_val < 0) {
            goto fdtput_chosen_exit;
        }
    }

    if (cpux_jump) {
        ret_val = fdt_setprop_u32(fdt, offset,
            "ambarella,cpux_jump", cpux_jump);
        if (ret_val < 0) {
            goto fdtput_chosen_exit;
        }
    }

    if ((initrd2_start != 0x0) && (initrd2_size != 0x0)) {
        ret_val = fdt_setprop_u32(fdt, offset, "linux,initrd-start",
                initrd2_start);
        if (ret_val < 0) {
            goto fdtput_chosen_exit;
        }
        ret_val = fdt_setprop_u32(fdt, offset, "linux,initrd-end",
                initrd2_start + initrd2_size);
        if (ret_val < 0) {
            goto fdtput_chosen_exit;
        }
    }

fdtput_chosen_exit:
    return ret_val;
}

static void __hex_to_str(const UINT32 hex, char *dest)
{
    char i, c;

    for (i = 0; i < 32; i += 4) {
        c = (hex >> (28 - i)) & 0xf;
        if (c >= 10)
            dest[i/4] = 'a' + c - 10;
        else
            dest[i/4] = '0' + c;
    }

    dest[8] = '\0';
}

int fdtput_spinor(void *fdt)
{
    int i, ret_val = -1;
    const char *pathp;
    int offset, suboffset;
    UINT32 val[6];
    AMBA_PARTITION_ENTRY_s PartInfo;
    AMBA_NORSPI_DEV_INFO_s *pNorDevInfo;

    pathp = fdt_get_alias(fdt, "spinor");
    if (pathp == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "libfdt fdt_get_alias() failed", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return ret_val;
    }

    offset = fdt_path_offset(fdt, pathp);
    if (offset < 0) {
        fdt_print_error("fdt_path_offset(nand) error:", offset);
        return ret_val;
    }

    pNorDevInfo = AmbaSpiNOR_GetDevInfo();

    for (i = AMBA_NUM_USER_PARTITION - 1; i >= 0; i--) {
        char name[32], addr_str[16];
        UINT32 addr, size;

        if (AmbaLink_ReadPartitionInfo(1, i, &PartInfo) != OK)
            return ret_val; /* should never happen */

        if (PartInfo.BlkCount == 0 || PartInfo.BlkCount == 0xffffffff)
            continue;

        addr = PartInfo.StartBlkAddr * pNorDevInfo->EraseBlockSize;
        size = PartInfo.BlkCount * pNorDevInfo->EraseBlockSize;

        strcpy(name, "partition@");
        __hex_to_str(addr, addr_str);
        strcat(name, addr_str);

        suboffset = fdt_add_subnode(fdt, offset, name);
        if (suboffset < 0) {
            fdt_print_error("fdt_add_subnode(partition) error:", ret_val);
            return ret_val;
        }

        ret_val = fdt_setprop_string(fdt, suboffset, "label",
                        (const void *)PartInfo.PartitionName);
        if (ret_val < 0) {
            fdt_print_error("fdt_setprop_string(label) error:", ret_val);
            return ret_val;
        }

        val[0] = cpu_to_fdt32(addr);
        val[1] = cpu_to_fdt32(size);
        ret_val = fdt_setprop(fdt, suboffset, "reg", &val, (sizeof(UINT32) * 2));
        if (ret_val < 0){
            fdt_print_error("fdt_setprop(reg) error:", ret_val);
            return ret_val;
        }
    }

    return ret_val;
}

int fdtput_nand(void *fdt)
{
    int i, ret_val = -1;
    const char *pathp;
    int offset, suboffset;
    UINT32 val[6], blk_size;
    AMBA_PARTITION_ENTRY_s PartInfo;
    AMBA_NAND_COMMON_INFO_s *pAmbaNAND_DevInfo = AmbaNAND_GetCommonInfo();

    pathp = fdt_get_alias(fdt, "nand");
    if (pathp == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "libfdt fdt_get_alias() failed", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return ret_val;
    }

    offset = fdt_path_offset (fdt, pathp);
    if (offset < 0) {
        fdt_print_error("fdt_path_offset(nand) error:", offset);
        return ret_val;
    }

    blk_size = pAmbaNAND_DevInfo->BlockPageSize * pAmbaNAND_DevInfo->MainByteSize;

    for (i = (int)(AMBA_NUM_USER_PARTITION - 1); i >= 0; i--) {
        char name[32], addr_str[16];
        UINT32 addr, size;

        if (AmbaLink_ReadPartitionInfo(1, i, &PartInfo) != OK)
            return ret_val; /* should never happen */

        if (PartInfo.BlkCount == 0 || PartInfo.BlkCount == 0xffffffff)
            continue;

        addr = PartInfo.StartBlkAddr * blk_size;
        size = PartInfo.BlkCount * blk_size;

        strcpy(name, "partition@");
        __hex_to_str(addr, addr_str);
        strcat(name, addr_str);

        suboffset = fdt_add_subnode(fdt, offset, name);
        if (suboffset < 0) {
            fdt_print_error("fdt_add_subnode(partition) error:",
                        suboffset);
            return suboffset;
        }

        ret_val = fdt_setprop_string(fdt, suboffset,
                    "label", (const void *)PartInfo.PartitionName);
        if (ret_val < 0) {
            fdt_print_error("fdt_setprop_string(label) error:",
                        ret_val);
            return ret_val;
        }

        val[0] = cpu_to_fdt32(addr);
        val[1] = cpu_to_fdt32(size);
        ret_val = fdt_setprop(fdt, suboffset, "reg",
                        &val, (sizeof(UINT32) * 2));
        if (ret_val < 0){
            fdt_print_error("fdt_setprop(reg) error:",
                        ret_val);
            return ret_val;
        }
#if 0
        if (i < (int)AMBA_USER_PARTITION_LINUX_KERNEL) {
            ret_val = fdt_setprop(fdt, suboffset, "read-only", NULL, 0);
            if (ret_val < 0){
                fdt_print_error("fdt_setprop(read-only) error:",
                            ret_val);
                return ret_val;
            }
        }
#endif
    }

    return ret_val;
}

int fdtput_emmc(void *fdt)
{
    int i, ret_val = -1;
    const char *pathp;
    int offset, suboffset;
    UINT64 val[6], sec_size;
    AMBA_PARTITION_ENTRY_s PartInfo;

    pathp = fdt_get_alias(fdt, "emmc");
    if (pathp == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "libfdt fdt_get_alias() failed", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return ret_val;
    }

    offset = fdt_path_offset(fdt, pathp);
    if (offset < 0) {
        fdt_print_error("fdt_path_offset(emmc) error:", offset);
        return ret_val;
    }

    sec_size = 512;
    for (i = AMBA_NUM_USER_PARTITION - 1; i >= 0; i--) {
        char name[32], addr_str[16];
        UINT64 addr, size;
        if (AmbaLink_ReadPartitionInfo(1, i, &PartInfo) != OK) {
            return ret_val; /* should never happen */
        }

        #if defined(CONFIG_MUTI_BOOT_DEVICE)
        if(((PartInfo.Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6U) == AMBA_NVM_SPI_NOR) {
            continue;
        }
        if(((PartInfo.Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6U) == AMBA_NVM_SPI_NAND) {
            continue;
        }
        #endif

        if (PartInfo.BlkCount == 0 || PartInfo.BlkCount == 0xffffffff)
            continue;

        addr = PartInfo.StartBlkAddr * sec_size;
        size = PartInfo.BlkCount * sec_size;

        strcpy(name, "partition@");
        __hex_to_str(addr, addr_str);
        strcat(name, addr_str);

        suboffset = fdt_add_subnode(fdt, offset, name);
        if (suboffset < 0) {
            fdt_print_error("fdt_add_subnode(partition) error:", suboffset);
            return suboffset;
        }

        ret_val = fdt_setprop_string(fdt, suboffset, "label",
                                    (const void *)PartInfo.PartitionName);
        if (ret_val < 0) {
            fdt_print_error("fdt_setprop_string(label) error:", ret_val);
            return ret_val;
        }

        val[0] = cpu_to_fdt64(addr);
        val[1] = cpu_to_fdt64(size);
        ret_val = fdt_setprop(fdt, suboffset, "reg", &val, (sizeof(UINT64) * 2));
        if (ret_val < 0){
            fdt_print_error("fdt_setprop(reg) error:", ret_val);
            return ret_val;
        }
    }

    return ret_val;
}

int fdtput_memory(void *fdt, LinuxMemoryInfo *info)
{
    int ret_val = -1;
    int offset;
    UINT32 val[4];

    if (fdt == NULL)
        goto fdtput_memory_exit;

    offset = fdt_node_offset_by_prop_value(fdt, -1,
            "device_type", "memory", 7);
    if (offset < 0) {
        ret_val = offset;
        goto fdtput_memory_exit;
    }
#if defined(CONFIG_CPU_CORTEX_A76)
    val[0] = cpu_to_fdt32(0x0);
    val[1] = cpu_to_fdt32(info->KernelBase);
    val[2] = cpu_to_fdt32(0x0);
    val[3] = cpu_to_fdt32(info->KernelSize);
    ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 4));
    if (ret_val < 0) {
        fdt_print_error("fdt_setprop error:", ret_val);
        goto fdtput_memory_exit;
    }

    val[0] = cpu_to_fdt32(0x0);
    val[1] = cpu_to_fdt32(0x0);
    val[2] = cpu_to_fdt32(0x0);
    val[3] = cpu_to_fdt32(CONFIG_FWPROG_SYS_LOADADDR);
    ret_val = fdt_appendprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 4));
    if (ret_val < 0) {
        fdt_print_error("fdt_setprop error:", ret_val);
        goto fdtput_memory_exit;
    }
#else
    val[0] = cpu_to_fdt32(info->KernelBase);
    val[1] = cpu_to_fdt32(info->KernelSize);
    ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
    if (ret_val < 0) {
        fdt_print_error("fdt_setprop error:", ret_val);
        goto fdtput_memory_exit;
    }

    val[0] = cpu_to_fdt32(0x0);
    val[1] = cpu_to_fdt32(CONFIG_FWPROG_SYS_LOADADDR);
    ret_val = fdt_appendprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
    if (ret_val < 0) {
        fdt_print_error("fdt_setprop error:", ret_val);
        goto fdtput_memory_exit;
    }
#endif
    /* create a new node "/shm" (offset 0 is root level) */
#if defined(CONFIG_AMBALINK_RPMSG_G2) && !defined(CONFIG_AMBALINK_LOCK_ADDR)
    ret_val = fdtput_shared_memory("shm", fdt, info->ShmBase, info->ShmSize, "noncached");
#else
    ret_val = fdtput_shared_memory("shm", fdt, info->ShmBase, info->ShmSize, NULL);
#endif
    if (ret_val < 0) {
        fdt_print_error("create /shm error:", ret_val);
        goto fdtput_memory_exit;
    }

    /* create a new node "/ppm2" (offset 0 is root level) */
    ret_val = fdtput_shared_memory("ppm2", fdt, info->Ppm2Base, info->Ppm2Size, NULL);
    if (ret_val < 0) {
        fdt_print_error("create /ppm2 error:", ret_val);
        goto fdtput_memory_exit;
    }

#ifdef CONFIG_AMBALINK_LOCK_ADDR
    /* Spinlock/Mutex would be shared with another cluster, e.g. R52 */
    ret_val = fdtput_shared_memory("lock_addr", fdt, CONFIG_AMBALINK_LOCK_ADDR, CONFIG_AMBALINK_LOCK_SIZE, "noncached");
    if (ret_val < 0) {
        fdt_print_error("create /lock_addr error:", ret_val);
        goto fdtput_memory_exit;
    }
#endif

    /* find or create a new node "/reserved-memory" (offset 0 is root level) */
    offset = fdt_node_offset_by_prop_value(fdt, -1,
                        "device_type", "reserved-memory", 16);
    if (offset < 0) {
        offset = fdt_add_subnode(fdt, 0, "reserved-memory");
        if (offset < 0)
            goto fdtput_memory_exit;
        ret_val = fdt_setprop_string(fdt, offset, "device_type", "reserved-memory");
        if (ret_val < 0) {
            fdt_print_error("fdt_setprop_string error:", ret_val);
            goto fdtput_memory_exit;
        }
    }

#if defined(CONFIG_CPU_CORTEX_A76)
    val[0] = cpu_to_fdt32(2);
#else
    val[0] = cpu_to_fdt32(1);
#endif
    ret_val = fdt_setprop(fdt, offset, "#size-cells", &val[0], sizeof(UINT32));
    ret_val = fdt_setprop(fdt, offset, "#address-cells", &val[0], sizeof(UINT32));
    ret_val = fdt_setprop(fdt, offset, "ranges", NULL, 0);

    /* add a new node "reserved@0x0" to reserve the memory to
       make the start address of linux to be 0x0. */
    offset = fdt_add_subnode(fdt, offset, "reserved@0x0");
    if (offset < 0)
        goto fdtput_memory_exit;

#if defined(CONFIG_CPU_CORTEX_A76)
    val[0] = cpu_to_fdt32(0x0);
    val[1] = cpu_to_fdt32(0x0);
    val[2] = cpu_to_fdt32(0x0);
    val[3] = cpu_to_fdt32(CONFIG_FWPROG_SYS_LOADADDR);
    ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 4));
#else
    val[0] = cpu_to_fdt32(0x0);
    val[1] = cpu_to_fdt32(CONFIG_FWPROG_SYS_LOADADDR);
    ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
#endif

    if (ret_val < 0)
        goto fdtput_memory_exit;

    ret_val = fdt_setprop(fdt, offset, "no-map", NULL, 0);
    if (ret_val < 0){
        fdt_print_error("fdt_setprop(no-map) error:", ret_val);
    }

fdtput_memory_exit:
    return ret_val;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  fdtput_shared_memory
 *
 *  @Description::  general function to create a level 0 new node
 *
 *      $name {
 *          reg = <$base $size>;
 *          device_type = "$name";
 *          amb,ioremap-method = "$method"
 *      };
 *
 *  @Return     ::
 *      int : OK(0)
              NG(non-zero)
\*----------------------------------------------------------------------------*/
int fdtput_shared_memory(const char *name, void *fdt, UINT64 base, UINT64 size, const char *method)
{
    int ret_val = -1;
    int offset;
    UINT32 val[2];

    if (fdt == NULL) {
        fdt_print_error("fdtput_shared_memory error:", ret_val);
        return ret_val;
    }

    offset = fdt_add_subnode(fdt, 0, name);
    if (offset < 0) {
        char buf[64];
        const SIZE_t len = AmbaUtility_StringLength(name) + 1U;
        buf[0] = '/';
        AmbaUtility_StringCopy(&buf[1], len, name);
        offset = fdt_path_offset (fdt, buf);
        if (offset < 0) {
            fdt_print_error("fdt_path_offset error:", offset);
            return -1;
        }
    }

    ret_val = fdt_setprop_string(fdt, offset, "device_type", name);
    if (ret_val < 0) {
        fdt_print_error("fdt_setprop_string error:", ret_val);
        return ret_val;
    }

    val[0] = cpu_to_fdt32(base);
    val[1] = cpu_to_fdt32(size);
    ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
    if (ret_val < 0) {
        fdt_print_error("fdt_setprop error:", ret_val);
        return ret_val;
    }

    if (method) {
        ret_val = fdt_setprop_string(fdt, offset, "amb,ioremap-method", method);
        if (ret_val < 0) {
            fdt_print_error("fdt_setprop_string error:", ret_val);
            return ret_val;
        }
    }

    return ret_val;
}

#if 0
UINT32 fdtput_tags(void *jump_addr, const char *cmdline, UINT32 cpux_jump,
    UINT32 initrd2_start, UINT32 initrd2_size, int verbose)
{
    int ret_val;
    UINT32 mem_base;
    UINT32 mem_size;
    UINT32 kernelp;
    UINT32 kernels;
    UINT32 idspp;
    UINT32 idsps;
    UINT32 dtb_addr;

    mem_base = (((UINT32)jump_addr) & (~SIZE_1MB_MASK));
    mem_size = (IDSP_RAM_START - mem_base);
    K_ASSERT(mem_base >= DRAM_START_ADDR);
    K_ASSERT(mem_size <= DRAM_SIZE);

    dtb_addr = AMBOOT_DTB_ADDR((UINT32)jump_addr);

    ret_val = fdtput_chosen((void *)dtb_addr, cmdline,
        cpux_jump, initrd2_start, initrd2_size);
    if (ret_val < 0) {
        if (verbose) {
            fdt_print_error("fdtput_chosen:", ret_val);
        }
        goto fdtput_tags_exit;
    }
    if (verbose) {
        if (cmdline) {
            putstr("cmdline: ");
            putstr(cmdline);
            putstr("\r\n");
        }

        putstr("cpux_jump: 0x");
        puthex(cpux_jump);
        putstr("\r\n");

        putstr("initrd2_start: 0x");
        puthex(initrd2_start);
        putstr(" initrd2_size: 0x");
        puthex(initrd2_size);
        putstr("\r\n");
    }

    kernelp = ARM11_TO_CORTEX(mem_base);
    kernels = mem_size;
    idspp = ARM11_TO_CORTEX(IDSP_RAM_START);
    idsps = (DRAM_SIZE - (IDSP_RAM_START - DRAM_START_ADDR));
    if (verbose) {
        putstr("kernelp: 0x");
        puthex(kernelp);
        putstr(" kernels: 0x");
        puthex(kernels);
        putstr("\r\n");

        putstr("idspp: 0x");
        puthex(idspp);
        putstr(" idsps: 0x");
        puthex(idsps);
        putstr("\r\n");
    }
    ret_val = fdtput_memory((void *)dtb_addr,
        kernelp, mem_size, idspp, idsps);
    if (ret_val < 0) {
        if (verbose) {
            fdt_print_error("fdtput_memory:", ret_val);
        }
        goto fdtput_tags_exit;
    }

fdtput_tags_exit:
    return ARM11_TO_CORTEX(dtb_addr);
}
#endif

int fdtput_cmdline(void *fdt, const char *cmdline)
{
    return fdtput_chosen(fdt, cmdline, 0, 0, 0);
}

