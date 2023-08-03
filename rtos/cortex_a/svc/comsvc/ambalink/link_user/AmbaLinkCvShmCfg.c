/**
 * @file AmbaLinkCvShmCfg.c
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
\*-------------------------------------------------------------------------------------------------------------------*/

#include "AmbaTypes.h"
#ifdef CONFIG_CPU_CORTEX_A53
#include "AmbaCortexA53.h"
#endif
#include "AmbaMMU_Def.h"
#include "AmbaLink.h"
#include "AmbaKAL.h"
#include "AmbaCache.h"
#include "AmbaNVM_Partition.h"
#include "AmbaMMU.h"
#include "AmbaPrint.h"
#include "AmbaRTSL_DRAMC.h"
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include "AmbaFDT.h"
#else

#if defined(CONFIG_APP_MEM_SIZE)
extern void *__app_start;
#else
#define CONFIG_APP_MEM_SIZE                     0
#endif

#if defined(CONFIG_CV_MEM_SIZE)
extern void *__cv_start;
#else
#define CONFIG_CV_MEM_SIZE                      0
#endif

#if defined(CONFIG_CV_MEM_RTOS_SIZE)
extern void *__cv_rtos_user_start;
extern void *__cv_rtos_user_end;
#else
#define CONFIG_CV_MEM_RTOS_SIZE                 0
#endif

#if defined(CONFIG_CV_MEM_SCHDR_SIZE)
extern void *__cv_sys_start;
extern void *__cv_sys_end;
#else
#define CONFIG_CV_MEM_SCHDR_SIZE                0
#endif
#endif

AMBA_LINK_CVSHM_CTRL_s AmbaLinkCvShmCtrl = {0};
void AmbaLink_CvCfg(void)
{
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    const void *fdt;
    INT32 ret, offset, len;
    const char *chr1;
    const UINT32 *ptr1;
    const UINT64 *ptr64;
    const struct fdt_property *prop = NULL;
    ULONG dtb_addr;
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_H32)
    ULONG ca, ca_size; 
#endif

    dtb_addr = (ULONG)CONFIG_DTB_LOADADDR;
    AmbaMisra_TypeCast(&fdt, &dtb_addr);

    ret = AmbaFDT_CheckHeader(fdt);
    if (ret != 0) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaLink_CvCfg() : AmbaFDT_CheckHeader ret = %d", (UINT32)ret, 0U, 0U, 0U, 0U);
    } else {
        offset = AmbaFDT_PathOffset(fdt, "scheduler");
        if (offset < 0) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaLink_CvCfg() : invalid fdt scheduler offset %d", (UINT32)offset, 0U, 0U, 0U, 0U);
        } else {
            prop = AmbaFDT_GetProperty(fdt, offset, "cv_att_pa", &len);
            if ((prop != NULL) && (len == 4)) {
                chr1 = (const char *)&prop->data[0];
                AmbaMisra_TypeCast(&ptr1, &chr1);
                AmbaLinkCvShmCtrl.CVSharedMemAddr = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
                AmbaLinkCvShmCtrl.CVSchdrMemAddr = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
            } else if ((prop != NULL) && (len == 8)) {
                chr1 = (const char *)&prop->data[0];
                AmbaMisra_TypeCast(&ptr64, &chr1);
                AmbaLinkCvShmCtrl.CVSharedMemAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
                AmbaLinkCvShmCtrl.CVSchdrMemAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
            } else {
                AmbaLinkCvShmCtrl.CVSharedMemAddr = (ULONG)0UL;
                AmbaLinkCvShmCtrl.CVSchdrMemAddr = (ULONG)0UL;
            }
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_H32)
            if( (AmbaLinkCvShmCtrl.CVSharedMemAddr != 0UL) && (AmbaRTSL_DramGetClientInfo(AMBA_DRAM_CLIENT_ORCVP, &ca, &ca_size) == 0U) ) {
                AmbaRTSL_DramQueryAttP2V(AmbaLinkCvShmCtrl.CVSharedMemAddr, AMBA_DRAM_CLIENT_ORCVP, &AmbaLinkCvShmCtrl.CVSharedMemClientAddr);
            } else
#endif
            {
                AmbaLinkCvShmCtrl.CVSharedMemClientAddr = AmbaLinkCvShmCtrl.CVSharedMemAddr;
            }

            prop = AmbaFDT_GetProperty(fdt, offset, "cv_att_size", &len);
            if ((prop != NULL) && (len == 4)) {
                chr1 = (const char *)&prop->data[0];
                AmbaMisra_TypeCast(&ptr1, &chr1);
                AmbaLinkCvShmCtrl.CVSharedMemSize = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
            } else if ((prop != NULL) && (len == 8)) {
                chr1 = (const char *)&prop->data[0];
                AmbaMisra_TypeCast(&ptr64, &chr1);
                AmbaLinkCvShmCtrl.CVSharedMemSize = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
            } else {
                AmbaLinkCvShmCtrl.CVSharedMemSize = (ULONG)0UL;
            }

            prop = AmbaFDT_GetProperty(fdt, offset, "cv_schdr_size", &len);
            if ((prop != NULL) && (len == 4)) {
                chr1 = (const char *)&prop->data[0];
                AmbaMisra_TypeCast(&ptr1, &chr1);
                AmbaLinkCvShmCtrl.CVSchdrMemSize = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
            } else if ((prop != NULL) && (len == 8)) {
                chr1 = (const char *)&prop->data[0];
                AmbaMisra_TypeCast(&ptr64, &chr1);
                AmbaLinkCvShmCtrl.CVSchdrMemSize = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
            } else {
                AmbaLinkCvShmCtrl.CVSchdrMemSize = (ULONG)0UL;
            }
        }

        offset = AmbaFDT_PathOffset(fdt, "flexidag_sys");
        if (offset < 0) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaLink_CvCfg() : invalid fdt flexidag_sys offset %d", (UINT32)offset, 0U, 0U, 0U, 0U);
        } else {
            prop = AmbaFDT_GetProperty(fdt, offset, "reg", &len);
            if ((prop != NULL) && (len == 8)) {
                chr1 = (const char *)&prop->data[0];
                AmbaMisra_TypeCast(&ptr1, &chr1);
                AmbaLinkCvShmCtrl.CVSysMemAddr = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
                AmbaLinkCvShmCtrl.CVSysMemSize = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[1]);
            } else if ((prop != NULL) && (len == 16)) {
                chr1 = (const char *)&prop->data[0];
                AmbaMisra_TypeCast(&ptr64, &chr1);
                AmbaLinkCvShmCtrl.CVSysMemAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
                AmbaLinkCvShmCtrl.CVSysMemSize = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[1]);
            } else {
                AmbaLinkCvShmCtrl.CVSysMemAddr = (ULONG)0UL;
                AmbaLinkCvShmCtrl.CVSysMemSize = (ULONG)0UL;
            }
        }

        offset = AmbaFDT_PathOffset(fdt, "flexidag_rtos");
        if (offset < 0) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaLink_CvCfg() : invalid fdt flexidag_sys offset %d", (UINT32)offset, 0U, 0U, 0U, 0U);
        } else {
            prop = AmbaFDT_GetProperty(fdt, offset, "reg", &len);
            if ((prop != NULL) && (len == 8)) {
                chr1 = (const char *)&prop->data[0];
                AmbaMisra_TypeCast(&ptr1, &chr1);
                AmbaLinkCvShmCtrl.CVRtosMemAddr = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
                AmbaLinkCvShmCtrl.CVRtosMemSize = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[1]);
            } else if ((prop != NULL) && (len == 16)) {
                chr1 = (const char *)&prop->data[0];
                AmbaMisra_TypeCast(&ptr64, &chr1);
                AmbaLinkCvShmCtrl.CVRtosMemAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
                AmbaLinkCvShmCtrl.CVRtosMemSize = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[1]);
            } else {
                AmbaLinkCvShmCtrl.CVRtosMemAddr = (ULONG)0UL;
                AmbaLinkCvShmCtrl.CVRtosMemSize = (ULONG)0UL;
            }
        }
    }
#else

#if (CONFIG_CV_MEM_SIZE != 0)

#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_H32)
    ULONG  ca, ca_size;
#endif

    AmbaLinkCvShmCtrl.CVSharedMemAddr            = (ULONG) &__cv_start;
    AmbaLinkCvShmCtrl.CVSharedMemSize            = CONFIG_CV_MEM_SIZE;

#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_H32)
    if( AmbaRTSL_DramGetClientInfo(AMBA_DRAM_CLIENT_ORCVP, &ca, &ca_size) == 0U ) {
        AmbaRTSL_DramQueryAttP2V(AmbaLinkCvShmCtrl.CVSharedMemAddr, AMBA_DRAM_CLIENT_ORCVP, &AmbaLinkCvShmCtrl.CVSharedMemClientAddr);
    } else
#endif
    {
        AmbaLinkCvShmCtrl.CVSharedMemClientAddr    = AmbaLinkCvShmCtrl.CVSharedMemAddr;
    }
#else
    AmbaLinkCvShmCtrl.CVSharedMemSize = 0U;
#endif

#if (CONFIG_CV_MEM_RTOS_SIZE != 0)
    AmbaLinkCvShmCtrl.CVRtosMemAddr              = (ULONG) &__cv_rtos_user_start;
    AmbaLinkCvShmCtrl.CVRtosMemSize              = CONFIG_CV_MEM_RTOS_SIZE;
#else
    AmbaLinkCvShmCtrl.CVRtosMemSize              = 0U;
#endif

#if (CONFIG_CV_MEM_SIZE != 0)
    AmbaLinkCvShmCtrl.CVSysMemAddr               = (ULONG) &__cv_sys_start;
    AmbaLinkCvShmCtrl.CVSysMemSize               = CONFIG_CV_MEM_SIZE - CONFIG_CV_MEM_RTOS_SIZE;
#else
    AmbaLinkCvShmCtrl.CVSysMemSize               = 0U;
#endif

#if (CONFIG_CV_MEM_SCHDR_SIZE != 0)
    AmbaLinkCvShmCtrl.CVSchdrMemAddr             = (ULONG) &__cv_sys_start;
    AmbaLinkCvShmCtrl.CVSchdrMemSize             = CONFIG_CV_MEM_SCHDR_SIZE;
#else
    AmbaLinkCvShmCtrl.CVSchdrMemSize             = 0U;
#endif

#if (CONFIG_APP_MEM_SIZE != 0)
    AmbaLinkCvShmCtrl.AppSharedMemAddr           = (ULONG) &__app_start;
    AmbaLinkCvShmCtrl.AppSharedMemSize           = CONFIG_APP_MEM_SIZE;
#else
    AmbaLinkCvShmCtrl.AppSharedMemSize           = 0U;
#endif
#endif

}

