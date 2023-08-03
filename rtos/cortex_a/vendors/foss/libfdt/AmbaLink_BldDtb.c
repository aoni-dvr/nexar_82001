/**
 *  @file AmbaLink_BldDtb.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details ATAG related structure and function are implemented here.
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaIOUtility.h"

#include "AmbaNVM_Partition.h"

#include "AmbaRTSL_GPIO.h"
#include "AmbaRTSL_CPU.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_UART.h"

#include "AmbaRTSL_SPINAND.h"

#include "AmbaRTSL_DRAMC.h"
#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_NAND_Ctrl.h"
#include "AmbaRTSL_NAND_BBM.h"
#include "AmbaRTSL_NAND_OP.h"

#include "AmbaSpiNOR_Ctrl.h"
#include "AmbaRTSL_SpiNOR.h"

#include "AmbaRTSL_SD.h"

#include "AmbaCSL_Scratchpad.h"

#include "libfdt.h"

#define PRINT_BUF_SIZE  (1024U)
static void BLD_PrintStr(const char *pFmt)
{
    static UINT8 BldPrintBuf[PRINT_BUF_SIZE] GNU_SECTION_NOZEROINIT;
    UINT32 ActualTxSize, UartTxSize, Index = 0U;
    char *pString;
    const UINT8 *pBuf = BldPrintBuf;
    const char *pArg = NULL;

#ifdef CONFIG_ARM32
    AmbaMisra_TypeCast32(&pString, &pBuf);
#else
    AmbaMisra_TypeCast64(&pString, &pBuf);
#endif
    UartTxSize = IO_UtilityStringPrintStr(pString, PRINT_BUF_SIZE, pFmt, (UINT32)1U, &pArg);
    while (UartTxSize > 0U) {
        if (AmbaRTSL_UartWrite(AMBA_UART_APB_CHANNEL0, UartTxSize, &BldPrintBuf[Index], &ActualTxSize) != UART_ERR_NONE) {
            break;
        } else {
            UartTxSize -= ActualTxSize;
            Index += ActualTxSize;
        }
    }
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

static int fdt_set_shared_memory(const char *name, void *fdt, UINT64 base, UINT64 size, const char *method)
{
    int ret_val = -1;
    int offset;
    UINT32 val[2];

    if (fdt == NULL) {
        return ret_val;
    }

    offset = fdt_add_subnode(fdt, 0, name);
    if (offset < 0) {
        BLD_PrintStr("# fdt_add_subnode failed\r\n#\r\n");
        return -1;
    }

    ret_val = fdt_setprop_string(fdt, offset, "device_type", name);
    if (ret_val < 0) {
        BLD_PrintStr("# fdt_setprop_string failed\r\n#\r\n");
        return ret_val;
    }

    val[0] = cpu_to_fdt32(base);
    val[1] = cpu_to_fdt32(size);
    ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
    if (ret_val < 0) {
        BLD_PrintStr("# fdt_setprop failed\r\n#\r\n");
        return ret_val;
    }

    if (method) {
        ret_val = fdt_setprop_string(fdt, offset, "amb,ioremap-method", method);
        if (ret_val < 0) {
            BLD_PrintStr("# fdt_setprop failed\r\n#\r\n");
            return ret_val;
        }
    }

    return ret_val;
}

#if defined(CONFIG_LINUX_MEM_SIZE)
#if defined(CONFIG_DSP_WORK_SIZE) || defined(CONFIG_DSP_PROT_BUF_SIZE) || defined(CONFIG_DSP_BIN_BUF_SIZE) || defined(CONFIG_DSP_STAT_BUF_SIZE) || defined(CONFIG_DSP_DATA_BUF_SIZE) || defined(CONFIG_DSP_LOG_BUF_SIZE)
#define KERNEL_ALIGN 0x400000U

static int fdt_update_dspshm(void *fdt)
{
    int ret_val = -1;
    int offset;
    UINT32 val[2];
    const char *pathp;
#if defined(CONFIG_CV_MEM_SIZE)
    UINT32 ReserveEnd = (UINT32)CONFIG_LINUX_MEM_ADDR + (UINT32)CONFIG_LINUX_MEM_SIZE - (UINT32)CONFIG_CV_MEM_SIZE;
#else
    UINT32 ReserveEnd = (UINT32)CONFIG_LINUX_MEM_ADDR + (UINT32)CONFIG_LINUX_MEM_SIZE;
#endif
    UINT32 UsedSize = 0U;
    UINT32 DspProtAddr;
    UINT32 DspProtSize;
    UINT32 DspProtCacheAddr;
    UINT32 DspProtCacheSize;
    UINT32 DspBinAddr;
    UINT32 DspBinSize;
    UINT32 DspDataAddr;
    UINT32 DspDataSize; /* includes work/log/data */

#if defined(CONFIG_DSP_PROT_BUF_SIZE)
    DspProtSize = ((CONFIG_DSP_PROT_BUF_SIZE + KERNEL_ALIGN - 1U) & (~(KERNEL_ALIGN - 1U)));
#else
    DspProtSize = 0U;
#endif
    UsedSize += DspProtSize;
    DspProtAddr = ReserveEnd - UsedSize;

#if defined(CONFIG_DSP_PROT_CACHE_BUF_SIZE)
    DspProtCacheSize = ((CONFIG_DSP_PROT_CACHE_BUF_SIZE + KERNEL_ALIGN - 1U) & (~(KERNEL_ALIGN - 1U)));
#else
    DspProtCacheSize = 0U;
#endif
    UsedSize += DspProtCacheSize;
    DspProtCacheAddr = ReserveEnd - UsedSize;

#if defined(CONFIG_DSP_BIN_BUF_SIZE)
    DspBinSize = ((CONFIG_DSP_BIN_BUF_SIZE + KERNEL_ALIGN - 1U) & (~(KERNEL_ALIGN - 1U)));
#else
    DspBinSize = 0U;
#endif
    UsedSize += DspBinSize;
    DspBinAddr = ReserveEnd - UsedSize;

#if defined(CONFIG_DSP_DATA_BUF_SIZE)
    DspDataSize = ((CONFIG_DSP_DATA_BUF_SIZE + KERNEL_ALIGN - 1U) & (~(KERNEL_ALIGN - 1U)));
#else
    DspDataSize = 0U;
#endif
    UsedSize += DspDataSize;
    DspDataAddr = ReserveEnd - UsedSize;

    if(DspProtSize != 0) {
        pathp = fdt_get_alias(fdt, "dsp_prot_buf");
        if (pathp == NULL) {
            BLD_PrintStr("# libfdt fdt_get_alias(dsp_prot_buf) failed \r\n#\r\n");
            return ret_val;
        }

        offset = fdt_path_offset (fdt, pathp);
        if (offset < 0) {
            BLD_PrintStr("# fdt_path_offset(dsp_prot_buf) error \r\n#\r\n");
            return ret_val;
        }

        val[0] = cpu_to_fdt32((UINT32)DspProtAddr);
        val[1] = cpu_to_fdt32((UINT32)DspProtSize);
        ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
        if (ret_val < 0)
            goto fdt_update_memory_exit;

        ret_val = fdt_setprop(fdt, offset, "no-map", NULL, 0);
        if (ret_val < 0){
            BLD_PrintStr("# fdt_setprop(reusable) error \r\n#\r\n");
        }
    } else {
        pathp = fdt_get_alias(fdt, "dsp");
        if (pathp == NULL) {
            goto fdt_update_memory_exit;
        }

        offset = fdt_path_offset (fdt, pathp);
        if (offset < 0) {
            goto fdt_update_memory_exit;
        }

        fdt_delprop(fdt, offset, "memory-region");
    }

    if(DspProtCacheSize != 0) {
        pathp = fdt_get_alias(fdt, "dsp_prot_cache_buf");
        if (pathp == NULL) {
            BLD_PrintStr("# libfdt fdt_get_alias(dsp_prot_cache_buf) failed \r\n#\r\n");
            return ret_val;
        }

        offset = fdt_path_offset(fdt, pathp);
        if (offset < 0) {
            BLD_PrintStr("# fdt_path_offset(dsp_prot_cache_buf) error \r\n#\r\n");
            return ret_val;
        }

        val[0] = cpu_to_fdt32((UINT32)DspProtCacheAddr);
        val[1] = cpu_to_fdt32((UINT32)DspProtCacheSize);
        ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
        if (ret_val < 0)
            goto fdt_update_memory_exit;

        ret_val = fdt_setprop(fdt, offset, "no-map", NULL, 0);
        if (ret_val < 0){
            BLD_PrintStr("# fdt_setprop(reusable) error \r\n#\r\n");
        }
    } else {
        pathp = fdt_get_alias(fdt, "dsp");
        if (pathp == NULL) {
            goto fdt_update_memory_exit;
        }

        offset = fdt_path_offset(fdt, pathp);
        if (offset < 0) {
            goto fdt_update_memory_exit;
        }

        fdt_delprop(fdt, offset, "memory-region1");
    }

    if (DspBinSize != 0) {
        pathp = fdt_get_alias(fdt, "dsp_bin_buf");
        if (pathp == NULL) {
            BLD_PrintStr("# libfdt fdt_get_alias(dsp_bin_buf) failed \r\n#\r\n");
            return ret_val;
        }

        offset = fdt_path_offset(fdt, pathp);
        if (offset < 0) {
            BLD_PrintStr("# fdt_path_offset(dsp_bin_buf) error \r\n#\r\n");
            return ret_val;
        }

        val[0] = cpu_to_fdt32((UINT32)DspBinAddr);
        val[1] = cpu_to_fdt32((UINT32)DspBinSize);
        ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
        if (ret_val < 0)
            goto fdt_update_memory_exit;

        ret_val = fdt_setprop(fdt, offset, "no-map", NULL, 0);
        if (ret_val < 0){
            BLD_PrintStr("# fdt_setprop(reusable) error \r\n#\r\n");
        }
    } else {
        pathp = fdt_get_alias(fdt, "dsp");
        if (pathp == NULL) {
            goto fdt_update_memory_exit;
        }

        offset = fdt_path_offset(fdt, pathp);
        if (offset < 0) {
            goto fdt_update_memory_exit;
        }
        fdt_delprop(fdt, offset, "memory-region2");
    }

    if (DspDataSize != 0) {
        pathp = fdt_get_alias(fdt, "dsp_data_buf");
        if (pathp == NULL) {
            BLD_PrintStr("# libfdt fdt_get_alias(dsp_data_buf) failed \r\n#\r\n");
            return ret_val;
        }

        offset = fdt_path_offset(fdt, pathp);
        if (offset < 0) {
            BLD_PrintStr("# fdt_path_offset(dsp_data_buf) error \r\n#\r\n");
            return ret_val;
        }

        val[0] = cpu_to_fdt32((UINT32)DspDataAddr);
        val[1] = cpu_to_fdt32((UINT32)DspDataSize);
        ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
        if (ret_val < 0)
            goto fdt_update_memory_exit;

        ret_val = fdt_setprop(fdt, offset, "no-map", NULL, 0);
        if (ret_val < 0){
            BLD_PrintStr("# fdt_setprop(reusable) error \r\n#\r\n");
        }
    } else {
        pathp = fdt_get_alias(fdt, "dsp");
        if (pathp == NULL) {
            goto fdt_update_memory_exit;
        }

        offset = fdt_path_offset(fdt, pathp);
        if (offset < 0) {
            goto fdt_update_memory_exit;
        }
        fdt_delprop(fdt, offset, "memory-region3");
    }

fdt_update_memory_exit:
    return ret_val;
}
#endif
#endif

#if defined(CONFIG_CV_MEM_SIZE) && defined(CONFIG_LINUX_MEM_SIZE)
int fdt_check_is_cv_sys_cma(void *fdt)
{
    const char *pathp;
    int offset;
    const void *ptr;
    int ret = 1;

    pathp = fdt_get_alias(fdt, "scheduler");
    if (pathp != NULL) {
        offset = fdt_path_offset (fdt, pathp);
        if (offset >= 0) {
            ptr = fdt_getprop(fdt, offset, "memory-region", NULL);
            if(ptr == NULL) {
                ret = 0;
            }
        }
    }

    return ret;
}

#if !defined(CONFIG_CV_MEM_RTOS_SIZE)
#define CONFIG_CV_MEM_RTOS_SIZE                 0
#endif

static int fdt_update_cvshm(void *fdt)
{
    int ret_val = -1;
    int is_cv_sys_cma = 1;
    int offset;
    UINT32 val[2];
    const char *pathp;
    UINT32 LinuxEnd = (UINT32)CONFIG_LINUX_MEM_ADDR + (UINT32)CONFIG_LINUX_MEM_SIZE;
    UINT32 CVSharedMemAddr;
    UINT32 CVSharedMemSize;
    UINT32 CVSysMemAddr;
    UINT32 CVSysMemSize;
    //UINT32 CVSchdrMemAddr;
    UINT32 CVSchdrMemSize;
    UINT32 CVRtosMemAddr;
    UINT32 CVRtosMemSize;
    UINT32 CVSharedMemClientAddr;

    CVSharedMemSize = CONFIG_CV_MEM_SIZE;
    CVSharedMemAddr = LinuxEnd - CONFIG_CV_MEM_SIZE;

    CVSysMemSize = CONFIG_CV_MEM_SIZE - CONFIG_CV_MEM_RTOS_SIZE;
    CVSysMemAddr = LinuxEnd - CONFIG_CV_MEM_SIZE;

    CVSchdrMemSize = CONFIG_CV_MEM_SCHDR_SIZE;
    //CVSchdrMemAddr = LinuxEnd - CONFIG_CV_MEM_SIZE;

    CVRtosMemSize = CONFIG_CV_MEM_RTOS_SIZE;
    CVRtosMemAddr = LinuxEnd - CONFIG_CV_MEM_RTOS_SIZE;

    CVSharedMemClientAddr = CVSharedMemAddr;

    /* create a new node "/cv_shm" (offset 0 is root level) */
    if(CVSharedMemSize != 0) {
        ret_val = fdt_set_shared_memory("cv_shm", fdt, (UINT64)CVSharedMemAddr, (UINT64)CVSharedMemSize, NULL);
        if (ret_val < 0) {
            BLD_PrintStr("# create /cv_shm error \r\n#\r\n");
            goto fdt_update_memory_exit;
        }
    }

    /* add a new node "cv_sys_mem@" to reserve the memory for flexidag_sys*/
    if(CVSysMemSize != 0) {
        pathp = fdt_get_alias(fdt, "flexidag_sys");
        if (pathp == NULL) {
            BLD_PrintStr("# libfdt fdt_get_alias(flexidag_sys) failed \r\n#\r\n");
            return ret_val;
        }

        offset = fdt_path_offset (fdt, pathp);
        if (offset < 0) {
            BLD_PrintStr("# fdt_path_offset(flexidag_sys) error \r\n#\r\n");
            return ret_val;
        }

        val[0] = cpu_to_fdt32((UINT32)CVSysMemAddr);
        val[1] = cpu_to_fdt32((UINT32)CVSysMemSize);
        ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
        if (ret_val < 0)
            goto fdt_update_memory_exit;

        is_cv_sys_cma = fdt_check_is_cv_sys_cma(fdt);
        if(is_cv_sys_cma == 1) {
            ret_val = fdt_setprop(fdt, offset, "reusable", NULL, 0);
            if (ret_val < 0){
                BLD_PrintStr("# fdt_setprop(reusable) error \r\n#\r\n");
            }
        } else {
            ret_val = fdt_setprop(fdt, offset, "no-map", NULL, 0);
            if (ret_val < 0){
                BLD_PrintStr("# fdt_setprop(no-map) error \r\n#\r\n");
            }
        }

        ret_val = fdt_setprop_string(fdt, offset, "compatible", "shared-dma-pool");
        if (ret_val < 0) {
            BLD_PrintStr("# fdt_setprop_string error \r\n#\r\n");
            goto fdt_update_memory_exit;
        }

        if(CVRtosMemSize != 0) {
            /* add a new node "cv_rtos_mem@" to reserve the memory for flexidag_sys*/
            pathp = fdt_get_alias(fdt, "flexidag_rtos");
            if (pathp == NULL) {
                BLD_PrintStr("# libfdt fdt_get_alias(flexidag_rtos) failed \r\n#\r\n");
                return ret_val;
            }

            offset = fdt_path_offset (fdt, pathp);
            if (offset < 0) {
                BLD_PrintStr("# fdt_path_offset(flexidag_rtos) error \r\n#\r\n");
                return ret_val;
            }

            val[0] = cpu_to_fdt32((UINT32)CVRtosMemAddr);
            val[1] = cpu_to_fdt32((UINT32)CVRtosMemSize);
            ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
            if (ret_val < 0)
                goto fdt_update_memory_exit;

            ret_val = fdt_setprop(fdt, offset, "no-map", NULL, 0);
            if (ret_val < 0){
                BLD_PrintStr("# fdt_setprop(reusable) error \r\n#\r\n");
            }
        } else {
            pathp = fdt_get_alias(fdt, "scheduler");
            if (pathp == NULL) {
                goto fdt_update_memory_exit;
            }

            offset = fdt_path_offset (fdt, pathp);
            if (offset < 0) {
                goto fdt_update_memory_exit;
            }

            fdt_delprop(fdt, offset, "memory-region1");
        }

        if(CVSchdrMemSize != 0U) {
            pathp = fdt_get_alias(fdt, "scheduler");
            if (pathp == NULL) {
                goto fdt_update_memory_exit;
            }

            offset = fdt_path_offset (fdt, pathp);
            if (offset < 0) {
                goto fdt_update_memory_exit;
            }
            val[0] = cpu_to_fdt32((UINT32)CVSchdrMemSize);
            fdt_setprop(fdt, offset, "cv_schdr_size", &val, sizeof(UINT32));
        }

        pathp = fdt_get_alias(fdt, "scheduler");
        if (pathp == NULL) {
            goto fdt_update_memory_exit;
        }

        offset = fdt_path_offset (fdt, pathp);
        if (offset < 0) {
            goto fdt_update_memory_exit;
        }
        val[0] = cpu_to_fdt32((UINT32)CVSharedMemAddr);
        fdt_setprop(fdt, offset, "cv_att_pa", &val, sizeof(UINT32));
        val[0] = cpu_to_fdt32((UINT32)CVSharedMemClientAddr);
        fdt_setprop(fdt, offset, "cv_att_ca", &val, sizeof(UINT32));
        val[0] = cpu_to_fdt32((UINT32)CVSharedMemSize);
        fdt_setprop(fdt, offset, "cv_att_size", &val, sizeof(UINT32));

    } else {
        pathp = fdt_get_alias(fdt, "scheduler");
        if (pathp == NULL) {
            goto fdt_update_memory_exit;
        }

        offset = fdt_path_offset (fdt, pathp);
        if (offset < 0) {
            goto fdt_update_memory_exit;
        }

        fdt_delprop(fdt, offset, "memory-region");
    }

fdt_update_memory_exit:
    return ret_val;
}
#endif

int fdt_update_memory(void *fdt)
{
#if defined(CONFIG_THREADX) && defined(CONFIG_ENABLE_AMBALINK) && defined(CONFIG_AMBALINK_BOOT_OS)
    UINT64 KernelSize = CONFIG_AMBALINK_MEM_SIZE - CONFIG_AMBALINK_SHARED_MEM_SIZE;
    UINT64 ShmBase = (UINT64)fdt - CONFIG_AMBALINK_SHARED_MEM_SIZE;
    UINT64 ShmSize = CONFIG_AMBALINK_SHARED_MEM_SIZE;
#elif defined(CONFIG_LINUX) && defined(CONFIG_LINUX_MEM_SIZE)
    UINT64 KernelSize = CONFIG_LINUX_MEM_SIZE;
    UINT64 ShmBase = 0;
    UINT64 ShmSize = 0;
#elif defined(CONFIG_XEN_SUPPORT)
    // TODO
    UINT64 KernelSize = 0;
    UINT64 ShmBase = 0;
    UINT64 ShmSize = 0;
#else // Unknown OS
    UINT64 KernelSize = 0;
    UINT64 ShmBase = 0;
    UINT64 ShmSize = 0;
#endif // CONFIG_OS
    int ret_val = -1;
    int offset;
    UINT32 val[2];
    UINT64 KernelBase = (UINT64)fdt;
    UINT64 Ppm2Base = CONFIG_FWPROG_SYS_LOADADDR;
    UINT64 Ppm2Size = ShmBase- CONFIG_FWPROG_SYS_LOADADDR;

    if (fdt_magic(fdt) != FDT_MAGIC) {
        BLD_PrintStr("# fdt_magic failed\r\n#\r\n");
        return ret_val;
    }

    /*
        memory {
                device_type = "memory";
                reg = <0x76600000 0x9a00000 0x0 0xD00000>;
        };
    */
    offset = fdt_node_offset_by_prop_value(fdt, -1,
            "device_type", "memory", 7);
    if (offset < 0) {
        BLD_PrintStr("# fdt_node_offset_by_prop_value failed\r\n#\r\n");
        ret_val = offset;
        return ret_val;
    }

    val[0] = cpu_to_fdt32(KernelBase);
    val[1] = cpu_to_fdt32(KernelSize);
    ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
    if (ret_val < 0) {
        BLD_PrintStr("# fdt_setprop failed\r\n#\r\n");
        return ret_val;
    }

    val[0] = cpu_to_fdt32(0x0);
    val[1] = cpu_to_fdt32(CONFIG_FWPROG_SYS_LOADADDR);
    ret_val = fdt_appendprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
    if (ret_val < 0) {
        BLD_PrintStr("# fdt_appendprop failed\r\n#\r\n");
        return ret_val;
    }

    /*
        shm {
                reg = <0x76000000 0x600000>;
                device_type = "shm";
        };
    */
#ifdef CONFIG_SOC_CV2FS
    ret_val = fdt_set_shared_memory("shm", fdt, ShmBase, ShmSize, "noncached");
#else
    ret_val = fdt_set_shared_memory("shm", fdt, ShmBase, ShmSize, NULL);
#endif
    if (ret_val < 0) {
        BLD_PrintStr("# fdt_set_shared_memory failed\r\n#\r\n");
        return ret_val;
    }

    /*
        ppm2 {
                reg = <0xD00000 0x75300000>;
                device_type = "ppm2";
        };

    */
    ret_val = fdt_set_shared_memory("ppm2", fdt, Ppm2Base, Ppm2Size, NULL);
    if (ret_val < 0) {
        return ret_val;
    }

    /*
        reserved-memory {
                ranges;
                #address-cells = <0x1>;
                #size-cells = <0x1>;
                device_type = "reserved-memory";

                reserved@0x0 {
                        no-map;
                        reg = <0x0 0xD00000>;
                };
        };
    */
    offset = fdt_node_offset_by_prop_value(fdt, -1,
                        "device_type", "reserved-memory", 16);
    if (offset < 0) {
        offset = fdt_add_subnode(fdt, 0, "reserved-memory");
        if (offset < 0)
            return ret_val;
        ret_val = fdt_setprop_string(fdt, offset, "device_type", "reserved-memory");
        if (ret_val < 0) {
            BLD_PrintStr("# fdt_setprop_string failed\r\n#\r\n");
            return ret_val;
        }
    }

    val[0] = cpu_to_fdt32(1);
    ret_val = fdt_setprop(fdt, offset, "#size-cells", &val[0], sizeof(UINT32));
    ret_val = fdt_setprop(fdt, offset, "#address-cells", &val[0], sizeof(UINT32));
    ret_val = fdt_setprop(fdt, offset, "ranges", NULL, 0);

    /* add a new node "reserved@0x0" to reserve the memory to
       make the start address of linux to be 0x0. */
    offset = fdt_add_subnode(fdt, offset, "reserved@0x0");
    if (offset < 0)
        return ret_val;

    val[0] = cpu_to_fdt32(0x0);
    val[1] = cpu_to_fdt32(CONFIG_FWPROG_SYS_LOADADDR);
    ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
    if (ret_val < 0)
        return ret_val;

    ret_val = fdt_setprop(fdt, offset, "no-map", NULL, 0);
    if (ret_val < 0){
        BLD_PrintStr("# fdt_setprop failed\r\n#\r\n");
    }

#if defined(CONFIG_CV_MEM_SIZE) && defined(CONFIG_LINUX_MEM_SIZE)
    ret_val = fdt_update_cvshm(fdt);
    if (ret_val < 0) {
        BLD_PrintStr("# fdt_update_cvshm failed\r\n#\r\n");
        return ret_val;
    }
#endif

#if defined(CONFIG_LINUX_MEM_SIZE)
#if defined(CONFIG_DSP_WORK_SIZE) || defined(CONFIG_DSP_PROT_BUF_SIZE) || defined(CONFIG_DSP_BIN_BUF_SIZE) || defined(CONFIG_DSP_STAT_BUF_SIZE) || defined(CONFIG_DSP_DATA_BUF_SIZE) || defined(CONFIG_DSP_LOG_BUF_SIZE)
    ret_val = fdt_update_dspshm(fdt);
    if (ret_val < 0) {
        BLD_PrintStr("# fdt_update_dspshm failed\r\n#\r\n");
        return ret_val;
    }
#endif
#endif

    return ret_val;
}

int fdt_update_chosen(void *fdt)
{
    int ret_val = 0;
    int offset;

    if (fdt_magic(fdt) != FDT_MAGIC) {
        BLD_PrintStr("# fdt_magic failed\r\n#\r\n");
        return ret_val;
    }

    offset = fdt_path_offset (fdt, "/chosen");
    if (offset < 0) {
        BLD_PrintStr("# fdt_path_offset failed\r\n#\r\n");
        ret_val = offset;
        return ret_val;
    }

#if defined(CONFIG_LINUX_CMDLINE)
    ret_val = fdt_setprop_string(fdt, offset, "bootargs", CONFIG_LINUX_CMDLINE);
#elif defined(CONFIG_AMBALINK_CMDLINE)
    ret_val = fdt_setprop_string(fdt, offset, "bootargs", CONFIG_AMBALINK_CMDLINE);
#else // Unknown OS
    ret_val = 0;
#endif
    if (offset < 0) {
        BLD_PrintStr("# fdt_setprop_string failed\r\n#\r\n");
        return ret_val;
    }

    return ret_val;
}

extern const AMBA_NORSPI_DEV_INFO_s AmbaNORSPI_DevInfo;

#ifdef CONFIG_ENABLE_SPINOR_BOOT
int fdt_update_spinor(void *fdt)
{
    int i, ret_val = -1;
    const char *pathp;
    int offset, suboffset;
    UINT32 val[6];
    const AMBA_PARTITION_ENTRY_s *pPartEntry;
    const AMBA_NORSPI_DEV_INFO_s *pNorDevInfo;

    if (fdt_magic(fdt) != FDT_MAGIC) {
        BLD_PrintStr("# fdt_magic failed\r\n#\r\n");
        return ret_val;
    }

    pathp = fdt_get_alias(fdt, "spinor");
    if (pathp == NULL) {
        BLD_PrintStr("# fdt_get_alias failed\r\n#\r\n");
        return ret_val;
    }

    offset = fdt_path_offset(fdt, pathp);
    if (offset < 0) {
        BLD_PrintStr("# fdt_path_offset failed\r\n#\r\n");
        return ret_val;
    }

    pNorDevInfo = &AmbaNORSPI_DevInfo;

    for (i = AMBA_NUM_USER_PARTITION - 1; i >= 0; i--) {
        char name[32], addr_str[16];
        UINT32 addr, size;

        pPartEntry = AmbaRTSL_NorSpiGetPartEntry(1, i);
        if (pPartEntry == NULL)
            continue;

        if (pPartEntry->BlkCount == 0 || pPartEntry->BlkCount == 0xffffffff)
            continue;

        addr = pPartEntry->StartBlkAddr * pNorDevInfo->EraseBlockSize;
        size = pPartEntry->BlkCount * pNorDevInfo->EraseBlockSize;

        IO_UtilityStringCopy(name, sizeof(name), "partition@");
        __hex_to_str(addr, addr_str);
        IO_UtilityStringAppend(name, sizeof(name), addr_str);

        suboffset = fdt_add_subnode(fdt, offset, name);
        if (suboffset < 0) {
            BLD_PrintStr("# fdt_add_subnode failed\r\n#\r\n");
            return ret_val;
        }

        ret_val = fdt_setprop_string(fdt, suboffset, "label",
                        (const void *)pPartEntry->PartitionName);
        if (ret_val < 0) {
            BLD_PrintStr("# fdt_setprop_string failed\r\n#\r\n");
            return ret_val;
        }

#if defined(CONFIG_LINUX)
        //Mount the whole nor on AMBA_USER_PARTITION_PTB for pure linux.
        if (i == AMBA_USER_PARTITION_PTB){
            addr = 0;
            size = pNorDevInfo->TotalByteSize;
        }
#endif

        val[0] = cpu_to_fdt32(addr);
        val[1] = cpu_to_fdt32(size);
        ret_val = fdt_setprop(fdt, suboffset, "reg", &val, (sizeof(UINT32) * 2));
        if (ret_val < 0){
             BLD_PrintStr("# fdt_setprop failed\r\n#\r\n");
            return ret_val;
        }
    }

    return ret_val;
}
#endif // CONFIG_ENABLE_SPINOR_BOOT

#if defined(CONFIG_ENABLE_NAND_BOOT) || defined(CONFIG_ENABLE_SPINAND_BOOT)
int fdt_update_nand(void *fdt)
{
    int i, ret_val = -1;
    const char *pathp;
    int offset, suboffset;
    UINT32 val[6], blk_size;
    const AMBA_PARTITION_ENTRY_s *pPartEntry;
    AMBA_NAND_COMMON_INFO_s *pAmbaNAND_DevInfo = AmbaRTSL_NandCommonInfo;

    if (fdt_magic(fdt) != FDT_MAGIC) {
        BLD_PrintStr("# fdt_magic failed\r\n#\r\n");
        return ret_val;
    }

    pathp = fdt_get_alias(fdt, "nand");
    if (pathp == NULL) {
        BLD_PrintStr("# fdt_get_alias failed\r\n#\r\n");
        return ret_val;
    }

    offset = fdt_path_offset (fdt, pathp);
    if (offset < 0) {
        BLD_PrintStr("# fdt_path_offset failed\r\n#\r\n");
        return ret_val;
    }

    blk_size = pAmbaNAND_DevInfo->BlockPageSize * pAmbaNAND_DevInfo->MainByteSize;

    for (i = (int)(AMBA_NUM_USER_PARTITION - 1); i >= 0; i--) {
        char name[32], addr_str[16];
        UINT32 addr, size;

        pPartEntry = AmbaRTSL_NandGetPartEntry(1, i);

        if (pPartEntry->BlkCount == 0 || pPartEntry->BlkCount == 0xffffffff)
            continue;

        addr = pPartEntry->StartBlkAddr * blk_size;
        size = pPartEntry->BlkCount * blk_size;

        IO_UtilityStringCopy(name, sizeof(name), "partition@");
        __hex_to_str(addr, addr_str);
        IO_UtilityStringAppend(name, sizeof(name), addr_str);

        suboffset = fdt_add_subnode(fdt, offset, name);
        if (suboffset < 0) {
            BLD_PrintStr("# fdt_add_subnode failed\r\n#\r\n");
            return suboffset;
        }

        ret_val = fdt_setprop_string(fdt, suboffset,
                    "label", (const void *)pPartEntry->PartitionName);
        if (ret_val < 0) {
            BLD_PrintStr("# fdt_setprop_string failed\r\n#\r\n");
            return ret_val;
        }

#if defined(CONFIG_LINUX)
        //Mount the whole nand on AMBA_USER_PARTITION_PTB for pure linux.
        if (i == AMBA_USER_PARTITION_PTB){
            addr = 0;
            size = pAmbaNAND_DevInfo->TotalPlanes * pAmbaNAND_DevInfo->PlaneBlockSize * pAmbaNAND_DevInfo->BlockPageSize * pAmbaNAND_DevInfo->MainByteSize ;
        }
#endif
        val[0] = cpu_to_fdt32(addr);
        val[1] = cpu_to_fdt32(size);
        ret_val = fdt_setprop(fdt, suboffset, "reg",
                        &val, (sizeof(UINT32) * 2));
        if (ret_val < 0){
            BLD_PrintStr("# fdt_setprop failed\r\n#\r\n");
            return ret_val;
        }

#if 0   //Linux SDK need to self upgrade
        if (i < (int)AMBA_USER_PARTITION_LINUX_KERNEL) {
            ret_val = fdt_setprop(fdt, suboffset, "read-only", NULL, 0);
            if (ret_val < 0){
                    BLD_PrintStr("# fdt_setprop failed\r\n#\r\n");
                return ret_val;
            }
        }
#endif
    }

    return ret_val;
}
#endif // defined(CONFIG_ENABLE_NAND_BOOT) || defined(CONFIG_ENABLE_SPINAND_BOOT)

#if defined(CONFIG_ENABLE_EMMC_BOOT)
int fdt_update_emmc(void *fdt)
{
    int i, ret_val = -1;
    const char *pathp;
    int offset, suboffset;
    UINT64 val[6], sec_size;
    const AMBA_PARTITION_ENTRY_s *pPartEntry;

    if (fdt_magic(fdt) != FDT_MAGIC) {
        BLD_PrintStr("# fdt_magic failed\r\n#\r\n");
        return ret_val;
    }

    pathp = fdt_get_alias(fdt, "emmc");
    if (pathp == NULL) {
        pathp = fdt_get_alias(fdt, "sd0");
        if (pathp == NULL) {
            BLD_PrintStr("# fdt_get_alias failed\r\n#\r\n");
            return ret_val;
        }
    }

    offset = fdt_path_offset(fdt, pathp);
    if (offset < 0) {
        BLD_PrintStr("# fdt_path_offset failed\r\n#\r\n");
        return ret_val;
    }

    sec_size = 512;

    for (i = AMBA_NUM_USER_PARTITION - 1; i >= 0; i--) {
        char name[32], addr_str[16];
        UINT64 addr, size;

        pPartEntry = AmbaRTSL_EmmcGetPartEntry(1, i);

        if (pPartEntry->BlkCount == 0 || pPartEntry->BlkCount == 0xffffffff)
            continue;

        addr = pPartEntry->StartBlkAddr * sec_size;
        size = pPartEntry->BlkCount * sec_size;

        IO_UtilityStringCopy(name, sizeof(name), "partition@");
        __hex_to_str(addr, addr_str);
        IO_UtilityStringAppend(name, sizeof(name), addr_str);

        suboffset = fdt_add_subnode(fdt, offset, name);
        if (suboffset < 0) {
            BLD_PrintStr("# fdt_add_subnode failed\r\n#\r\n");
            return suboffset;
        }

        ret_val = fdt_setprop_string(fdt, suboffset, "label",
                                    (const void *)pPartEntry->PartitionName);
        if (ret_val < 0) {
            BLD_PrintStr("# fdt_setprop_string failed\r\n#\r\n");
            return ret_val;
        }

        val[0] = cpu_to_fdt64(addr);
        val[1] = cpu_to_fdt64(size);
        ret_val = fdt_setprop(fdt, suboffset, "reg", &val, (sizeof(UINT64) * 2));
        if (ret_val < 0){
            BLD_PrintStr("# fdt_setprop failed\r\n#\r\n");
            return ret_val;
        }
    }

    return ret_val;
}
#endif // defined(CONFIG_ENABLE_EMMC_BOOT)

