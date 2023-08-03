/**
 * @file fdt_cvshm.c
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
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "libfdt.h"
#include "AmbaRTSL_DRAMC.h"

extern AMBA_LINK_CVSHM_CTRL_s AmbaLinkCvShmCtrl;

static void fdt_print_error(const char *str, int err)
{
    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s %s!", str, fdt_strerror(err), NULL, NULL, NULL);
    AmbaPrint_Flush();
}

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


int fdtput_cvshm(void *fdt)
{
    int ret_val = -1;
    int offset;
#if defined(CONFIG_CPU_CORTEX_A76)
    UINT32 val[4];
#else
    UINT32 val[2];
#endif
#ifndef CONFIG_AMBALINK_DISABLE_CV
    ULONG base = 0UL,size = 0UL;
#endif
    const char *pathp,*pathp1;
    int is_cv_sys_cma = 1;

    /* create a new node "/cv_shm" (offset 0 is root level) */
    if(AmbaLinkCvShmCtrl.CVSharedMemSize != 0) {
        ret_val = fdtput_shared_memory("cv_shm", fdt, (UINT64)AmbaLinkCvShmCtrl.CVSharedMemAddr, (UINT64)AmbaLinkCvShmCtrl.CVSharedMemSize, NULL);
        if (ret_val < 0) {
            fdt_print_error("create /cv_shm error:", ret_val);
            goto fdtput_memory_exit;
        }
    }

    /* create a new node "/app_shm" (offset 0 is root level) */
    if(AmbaLinkCvShmCtrl.AppSharedMemSize != 0) {
        ret_val = fdtput_shared_memory("app_shm", fdt, (UINT64)AmbaLinkCvShmCtrl.AppSharedMemAddr, (UINT64)AmbaLinkCvShmCtrl.AppSharedMemSize, NULL);
        if (ret_val < 0) {
            fdt_print_error("create /app_shm error:", ret_val);
            goto fdtput_memory_exit;
        }
    }

    /* add a new node "cv_sys_mem@" to reserve the memory for flexidag_sys*/
    if(AmbaLinkCvShmCtrl.CVSysMemSize != 0) {
        pathp = fdt_get_alias(fdt, "flexidag_sys");
        if (pathp == NULL) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "libfdt fdt_get_alias(flexidag_sys) failed", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            return ret_val;
        }

        offset = fdt_path_offset (fdt, pathp);
        if (offset < 0) {
            fdt_print_error("fdt_path_offset(flexidag_sys) error:", offset);
            return ret_val;
        }

        is_cv_sys_cma = fdt_check_is_cv_sys_cma(fdt);
        if(is_cv_sys_cma == 1) {
            AmbaPrint_PrintUInt5("flexidag_sys is cma mode ",0U,0U,0U,0U,0U);
            pathp1 = fdt_get_alias(fdt, "flexidag_schdr");
            if (pathp1 != NULL) {
                AmbaPrint_PrintUInt5("flexidag_schdr exist ",0U,0U,0U,0U,0U);
                /* flexidag_sys */
#ifndef CONFIG_AMBALINK_DISABLE_CV
                base = AmbaLinkCvShmCtrl.CVSchdrMemAddr + AmbaLinkCvShmCtrl.CVSchdrMemSize;
                size = AmbaLinkCvShmCtrl.CVSysMemSize - AmbaLinkCvShmCtrl.CVSchdrMemSize;
#if defined(CONFIG_CPU_CORTEX_A76)
                val[0] = cpu_to_fdt32((UINT32)(base >> 32));
                val[1] = cpu_to_fdt32((UINT32)(base & 0xFFFFFFFF));
                val[2] = cpu_to_fdt32((UINT32)(size >> 32));
                val[3] = cpu_to_fdt32((UINT32)(size & 0xFFFFFFFF));
                ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 4));
#else
                val[0] = cpu_to_fdt32((UINT32)base);
                val[1] = cpu_to_fdt32((UINT32)size);
                ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
#endif
                if (ret_val < 0)
                    goto fdtput_memory_exit;

                ret_val = fdt_setprop(fdt, offset, "reusable", NULL, 0);
                if (ret_val < 0){
                    fdt_print_error("fdt_setprop(reusable) error:", ret_val);
                }

                ret_val = fdt_setprop_string(fdt, offset, "compatible", "shared-dma-pool");
                if (ret_val < 0) {
                    fdt_print_error("fdt_setprop_string error:", ret_val);
                    goto fdtput_memory_exit;
                }
#endif

                /* flexidag_schdr */
                pathp = fdt_get_alias(fdt, "flexidag_schdr");
                if (pathp != NULL) {
                    offset = fdt_path_offset (fdt, pathp);
                }

                if(offset  >= 0) {
#if defined(CONFIG_CPU_CORTEX_A76)
                    val[0] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSchdrMemAddr >> 32));
                    val[1] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSchdrMemAddr & 0xFFFFFFFF));
                    val[2] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSchdrMemSize >> 32));
                    val[3] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSchdrMemSize & 0xFFFFFFFF));


                    ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 4));
#else
                    val[0] = cpu_to_fdt32((UINT32)AmbaLinkCvShmCtrl.CVSchdrMemAddr);
                    val[1] = cpu_to_fdt32((UINT32)AmbaLinkCvShmCtrl.CVSchdrMemSize);

                    ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
#endif
                    if (ret_val < 0)
                        goto fdtput_memory_exit;

                    ret_val = fdt_setprop(fdt, offset, "no-map", NULL, 0);
                    if (ret_val < 0){
                        fdt_print_error("fdt_setprop(no-map) error:", ret_val);
                    }
                }
           } else {
                /* flexidag_sys */
#if defined(CONFIG_CPU_CORTEX_A76)
#ifdef CONFIG_AMBALINK_DISABLE_CV
                val[0] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSchdrMemAddr >> 32));
                val[1] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSchdrMemAddr & 0xFFFFFFFF));
                val[2] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSchdrMemSize >> 32));
                val[3] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSchdrMemSize & 0xFFFFFFFF));
#else
                val[0] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSysMemAddr >> 32));
                val[1] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSysMemAddr & 0xFFFFFFFF));
                val[2] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSysMemSize >> 32));
                val[3] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSysMemSize & 0xFFFFFFFF));
#endif
                ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 4));
#else
#ifdef CONFIG_AMBALINK_DISABLE_CV
                val[0] = cpu_to_fdt32((UINT32)AmbaLinkCvShmCtrl.CVSchdrMemAddr);
                val[1] = cpu_to_fdt32((UINT32)AmbaLinkCvShmCtrl.CVSchdrMemSize);
#else
                val[0] = cpu_to_fdt32((UINT32)AmbaLinkCvShmCtrl.CVSysMemAddr);
                val[1] = cpu_to_fdt32((UINT32)AmbaLinkCvShmCtrl.CVSysMemSize);
#endif
                ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
#endif
                if (ret_val < 0)
                    goto fdtput_memory_exit;

#ifdef CONFIG_AMBALINK_DISABLE_CV
                ret_val = fdt_setprop(fdt, offset, "no-map", NULL, 0);
#else
                ret_val = fdt_setprop(fdt, offset, "reusable", NULL, 0);
#endif
                if (ret_val < 0){
                    fdt_print_error("fdt_setprop(reusable) error:", ret_val);
                }
                ret_val = fdt_setprop_string(fdt, offset, "compatible", "shared-dma-pool");
                if (ret_val < 0) {
                    fdt_print_error("fdt_setprop_string error:", ret_val);
                    goto fdtput_memory_exit;
                }
            }
        } else {
#if defined(CONFIG_CPU_CORTEX_A76)
            val[0] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSysMemAddr >> 32));
            val[1] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSysMemAddr & 0xFFFFFFFF));
            val[2] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSysMemSize >> 32));
            val[3] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSysMemSize & 0xFFFFFFFF));
            ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 4));
#else
            val[0] = cpu_to_fdt32((UINT32)AmbaLinkCvShmCtrl.CVSysMemAddr);
            val[1] = cpu_to_fdt32((UINT32)AmbaLinkCvShmCtrl.CVSysMemSize);
            ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
#endif
            if (ret_val < 0)
                goto fdtput_memory_exit;

            AmbaPrint_PrintUInt5("flexidag_sys is non-cma mode ",0U,0U,0U,0U,0U);
            AmbaPrint_PrintUInt5("# fdt_setprop_string failed\r\n#\r\n",0U,0U,0U,0U,0U);
            ret_val = fdt_setprop(fdt, offset, "no-map", NULL, 0);
            if (ret_val < 0){
                fdt_print_error("fdt_setprop(reusable) error:", ret_val);
            }
        }

        if(AmbaLinkCvShmCtrl.CVRtosMemSize != 0) {
            /* add a new node "cv_rtos_mem@" to reserve the memory for flexidag_sys*/
            pathp = fdt_get_alias(fdt, "flexidag_rtos");
            if (pathp == NULL) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "libfdt fdt_get_alias(flexidag_rtos) failed", NULL, NULL, NULL, NULL, NULL);
                AmbaPrint_Flush();
                return ret_val;
            }

            offset = fdt_path_offset (fdt, pathp);
            if (offset < 0) {
                fdt_print_error("fdt_path_offset(flexidag_rtos) error:", offset);
                return ret_val;
            }

#if defined(CONFIG_CPU_CORTEX_A76)
            val[0] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVRtosMemAddr >> 32));
            val[1] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVRtosMemAddr & 0xFFFFFFFF));
            val[2] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVRtosMemSize >> 32));
            val[3] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVRtosMemSize & 0xFFFFFFFF));
            ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 4));
#else
            val[0] = cpu_to_fdt32((UINT32)AmbaLinkCvShmCtrl.CVRtosMemAddr);
            val[1] = cpu_to_fdt32((UINT32)AmbaLinkCvShmCtrl.CVRtosMemSize);
            ret_val = fdt_setprop(fdt, offset, "reg", &val, (sizeof(UINT32) * 2));
#endif
            if (ret_val < 0)
                goto fdtput_memory_exit;

            ret_val = fdt_setprop(fdt, offset, "no-map", NULL, 0);
            if (ret_val < 0){
                fdt_print_error("fdt_setprop(reusable) error:", ret_val);
            }
        } else {
            pathp = fdt_get_alias(fdt, "scheduler");
            if (pathp == NULL) {
                goto fdtput_memory_exit;
            }

            offset = fdt_path_offset (fdt, pathp);
            if (offset < 0) {
                goto fdtput_memory_exit;
            }

            fdt_delprop(fdt, offset, "memory-region1");
        }

        if(AmbaLinkCvShmCtrl.CVSchdrMemSize != 0U) {
            pathp = fdt_get_alias(fdt, "scheduler");
            if (pathp == NULL) {
                goto fdtput_memory_exit;
            }

            offset = fdt_path_offset (fdt, pathp);
            if (offset < 0) {
                goto fdtput_memory_exit;
            }
#if defined(CONFIG_CPU_CORTEX_A76)
            val[0] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSchdrMemSize >> 32));
            val[1] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSchdrMemSize & 0xFFFFFFFF));
            fdt_setprop(fdt, offset, "cv_schdr_size", &val, (sizeof(UINT32) * 2));
#else
            val[0] = cpu_to_fdt32((UINT32)AmbaLinkCvShmCtrl.CVSchdrMemSize);
            fdt_setprop(fdt, offset, "cv_schdr_size", &val, sizeof(UINT32));
#endif
        }

        pathp = fdt_get_alias(fdt, "scheduler");
        if (pathp == NULL) {
            goto fdtput_memory_exit;
        }

        offset = fdt_path_offset (fdt, pathp);
        if (offset < 0) {
            goto fdtput_memory_exit;
        }
#if defined(CONFIG_CPU_CORTEX_A76)
        val[0] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSharedMemAddr >> 32));
        val[1] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSharedMemAddr & 0xFFFFFFFF));
        fdt_setprop(fdt, offset, "cv_att_pa", &val, (sizeof(UINT32) * 2));
        val[0] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSharedMemClientAddr >> 32));
        val[1] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSharedMemClientAddr & 0xFFFFFFFF));
        fdt_setprop(fdt, offset, "cv_att_ca", &val, (sizeof(UINT32) * 2));
        val[0] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSharedMemSize >> 32));
        val[1] = cpu_to_fdt32((UINT32)(AmbaLinkCvShmCtrl.CVSharedMemSize & 0xFFFFFFFF));
        fdt_setprop(fdt, offset, "cv_att_size", &val, (sizeof(UINT32) * 2));
#else
        val[0] = cpu_to_fdt32((UINT32)AmbaLinkCvShmCtrl.CVSharedMemAddr);
        fdt_setprop(fdt, offset, "cv_att_pa", &val, sizeof(UINT32));
        val[0] = cpu_to_fdt32((UINT32)AmbaLinkCvShmCtrl.CVSharedMemClientAddr);
        fdt_setprop(fdt, offset, "cv_att_ca", &val, sizeof(UINT32));
        val[0] = cpu_to_fdt32((UINT32)AmbaLinkCvShmCtrl.CVSharedMemSize);
        fdt_setprop(fdt, offset, "cv_att_size", &val, sizeof(UINT32));
#endif
#ifdef CONFIG_AMBALINK_DISABLE_CV
        fdt_setprop_string(fdt, offset, "status", "disabled");
#endif
    } else {
        pathp = fdt_get_alias(fdt, "scheduler");
        if (pathp == NULL) {
            goto fdtput_memory_exit;
        }

        offset = fdt_path_offset (fdt, pathp);
        if (offset < 0) {
            goto fdtput_memory_exit;
        }

        fdt_delprop(fdt, offset, "memory-region");
        fdt_delprop(fdt, offset, "memory-region1");
#ifdef CONFIG_AMBALINK_DISABLE_CV
        fdt_setprop_string(fdt, offset, "status", "disabled");
#endif
    }

fdtput_memory_exit:
    return ret_val;
}
