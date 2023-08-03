/**
 *  @file AmbaCV_Flexidag.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details AmbaCV Flexidag APIs
 *
 */

#include "ambint.h"
#include "schdr_api.h"
#include "schdr_util.h"
#include "cvapi_logger_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cavalry/cavalry_ioctl.h"
#include "AmbaCV_Flexidag_Prv.h"
#if defined(CONFIG_THREADX)
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include "AmbaFDT.h"
#endif
#elif defined(CONFIG_QNX)
#include <sys/mman.h>
#include <sys/cache.h>
#else
#include <sys/mman.h>
#endif
#if defined(ENABLE_AMBA_MAL)
#include "AmbaMAL.h"
#endif

extern uint32_t ambacv_cache_clean(void *ptr, uint64_t size);
extern uint32_t ambacv_cache_invalidate(void *ptr, uint64_t size);
extern void* ambacv_c2v(uint64_t ca);
extern void* ambacv_p2v(uint64_t pa);
extern uint64_t ambacv_p2c(uint64_t pa);
extern uint32_t ambacv_att_init(void);

#define FLEXIDAG_STATE_UNUSED   0x0U
#define FLEXIDAG_STATE_INIT     0x1U
#define FLEXIDAG_STATE_READY    0x2U

#if defined (CHIP_CV2) || defined (CHIP_CV22) || defined (CHIP_CV25) || defined (CHIP_CV28)
#define VP_OUTPUT_ALIGN_SIZE   32U
#elif defined (CHIP_CV2A) || defined (CHIP_CV22A) || defined (CHIP_CV2FS) || defined (CHIP_CV22FS)
#define VP_OUTPUT_ALIGN_SIZE   64U
#elif defined (CHIP_CV5) || defined (CHIP_CV52)
#define VP_OUTPUT_ALIGN_SIZE   128U
#elif defined (CHIP_CV6)
#define VP_OUTPUT_ALIGN_SIZE   128U
#else
#error "unsupported CHIP ID, please check your PROJECT setting"
#endif

#if defined (CHIP_CV2) || defined (CHIP_CV22) || defined (CHIP_CV25) || defined (CHIP_CV28)
#define FD_MEM_ALIGN_SIZE     128U
#elif defined (CHIP_CV2A) || defined (CHIP_CV22A) || defined (CHIP_CV2FS) || defined (CHIP_CV22FS)
#define FD_MEM_ALIGN_SIZE     128U
#elif defined (CHIP_CV5) || defined (CHIP_CV52)
#define FD_MEM_ALIGN_SIZE     128U
#elif defined (CHIP_CV6)
#define FD_MEM_ALIGN_SIZE     256U
#else
#error "unsupported CHIP ID, please check your PROJECT setting"
#endif

#ifndef USE_AMBA_KAL
#define GNU_SECTION_NOZEROINIT
#endif

static flexidag_trace_t *pFlexidagTrace;
static uint32_t inter_schdr_error= 0;
static uint32_t inter_flexidag_error[FLEXIDAG_MAX_SLOTS] = {0};
static uint32_t inter_state[FLEXIDAG_MAX_SLOTS] = {0};
static AMBA_CV_FLEXIDAG_HANDLE_s *AmbaCV_Handle[FLEXIDAG_MAX_SLOTS];
static uint32_t AmbaCV_Handle_Init[FLEXIDAG_MAX_SLOTS] = {0};
static mutex_t api_run_mutex[FLEXIDAG_MAX_SLOTS];
static mutex_t global_mutex;
static uint32_t global_init = 0U;


#if defined(ENV_IS_PACE_LINUX)
#define ARGV_NUM                16U

static uint32_t CV_StrToArg(const char *str, int32_t *argc, char **argv)
{
    uint32_t retcode = ERRCODE_NONE;
    char *token;
    char str_tmp[ARGV_NUM * 64U];

    strncpy(str_tmp, str, (strlen(str) + 1U));
    token = strtok(str_tmp, " ");
    while (token != NULL) {
        if (*argc < ((int32_t)ARGV_NUM - 1)) {
            argv[*argc] = token;
            *argc = *argc + 1;
            token = strtok(NULL, " ");
        } else {
            token = NULL;
            retcode = ERR_INTF_INPUT_PARAM_INVALID;
        }
    }
    argv[*argc] = 0;


    return retcode;
}
#endif

static FLOAT CV_GetAudioClockUs(void)
{
    static FLOAT audio_clock_us = (FLOAT)0;

    if(audio_clock_us == (FLOAT)0) {
#if !defined(ENV_IS_PACE_LINUX)
        uint32_t freq,ret;

        ret = schdr_get_audio_tick(&freq);
        if(ret == 0U) {
            audio_clock_us = (FLOAT)freq;
            audio_clock_us = audio_clock_us/(FLOAT)1000000;
        } else {
            audio_clock_us = (FLOAT)12.288;
            AmbaPrint_PrintUInt5("[WARNING] AmbaCV_Flexidag : get_audio_clock fail ret = 0x%x ", ret, 0U, 0U, 0U, 0U);
        }
#else
        audio_clock_us = (FLOAT)12.288;
#endif
    }

    return audio_clock_us;
}


static uint32_t CV_GetExternalErrorCode(const AMBA_CV_FLEXIDAG_HANDLE_s *handle, uint32_t ret)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t errcode;
    uint32_t slot_id;

    if(ret != ERRCODE_NONE) {
        errcode = ret;
        if(errmgr_report_error(CV_MODULE_ID_DRIVER, ret) == ERR_DRV_SCHDR_ERRMGR_IPC_SEND_FAIL) {
            errcode = ERR_DRV_SCHDR_ERRMGR_IPC_SEND_FAIL;
        }

        if((handle == NULL) || (handle->fd_handle == NULL)) {
            inter_schdr_error = errcode;
        } else {
            slot_id = flexidag_find_slot_by_vphandle(handle->fd_handle);
            if(slot_id < FLEXIDAG_MAX_SLOTS) {
                inter_flexidag_error[slot_id] = errcode;
            }
        }

        if(is_drv_interface_err(errcode) == 1U) {
            retcode = CV_ERR_0000;
        } else if (is_drv_scheduler_err(errcode) == 1U) {
            retcode = CV_ERR_0001;
        } else if (is_drv_flexidag_err(errcode) == 1U) {
            retcode = CV_ERR_0002;
        } else {
            retcode = CV_ERR_0003;
        }
    }

    return retcode;
}

static uint64_t rtos_region_base = 0UL, rtos_region_size = 0UL;
#if !defined(ENV_IS_PACE_LINUX)
static uint64_t cma_region_base = 0UL, cma_region_size = 0UL;
static uint64_t cvschdr_region_base = 0UL, cvschdr_region_size = 0UL;
#if defined(ENABLE_AMBA_MAL)
static uint64_t dsp_region_base = 0UL, dsp_region_size = 0UL;
#endif
#endif

static uint32_t CV_SchdrGetCvRegion(void)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t is_cached;

    retcode = (uint32_t)schdr_get_rtos_region(&rtos_region_base, &rtos_region_size, &is_cached);
    if(retcode != ERRCODE_NONE) {
        AmbaPrint_PrintUInt5("[ERROR] CV_SchdrGetCvRegion() : schdr_get_rtos_region error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
#if !defined(ENV_IS_PACE_LINUX)
    } else {
        retcode = schdr_get_cma_region_info(&cma_region_base, &cma_region_size);
        if(retcode != ERRCODE_NONE) {
            AmbaPrint_PrintUInt5("[ERROR] CV_SchdrGetCvRegion() : schdr_get_cma_region_info error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        } else {
            retcode = (uint32_t)schdr_get_cvschdr_region_info(&cvschdr_region_base, &cvschdr_region_size);
            if(retcode != ERRCODE_NONE) {
                AmbaPrint_PrintUInt5("[ERROR] CV_SchdrGetCvRegion() : schdr_get_cvschdr_region_info error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
            }
#if defined(ENABLE_AMBA_MAL)
            else {
                AMBA_MAL_INFO_s DspInfo;

                if(AmbaMAL_GetInfo(AMBA_MAL_ID_DSP_DATA, &DspInfo) == 0U) {
                    dsp_region_base = DspInfo.PhysAddr;
                    dsp_region_size = DspInfo.Size;
                }
            }
#endif
        }
#endif
    }

    return retcode;
}

static uint32_t CV_FlexidagCheckCvRegion(const flexidag_memblk_t *memblk)
{
    uint32_t retcode;

    (void) memblk;
#if !defined(ENV_IS_PACE_LINUX)
    if(memblk->buffer_size == 0U) {
        retcode = ERRCODE_NONE;
    } else if((rtos_region_base <= memblk->buffer_daddr) && ((rtos_region_base + rtos_region_size) >= ((uint64_t)memblk->buffer_daddr + (uint64_t)memblk->buffer_size))) {
        retcode = ERRCODE_NONE;
    } else if((cma_region_base <= memblk->buffer_daddr) && ((cma_region_base + cma_region_size) >= ((uint64_t)memblk->buffer_daddr + (uint64_t)memblk->buffer_size))) {
        retcode = ERRCODE_NONE;
    }
#if defined(ENABLE_AMBA_MAL)
    else if((dsp_region_base <= memblk->buffer_daddr) && ((dsp_region_base + dsp_region_size) >= ((uint64_t)memblk->buffer_daddr + (uint64_t)memblk->buffer_size))) {
        retcode = ERRCODE_NONE;
    }
#endif
    else {
        retcode = ERR_INTF_FLEXIDAG_MEMBLK_OUT_OF_RANGE;
    }
#else
    retcode = ERRCODE_NONE;
#endif

    return retcode;
}

#if defined(ENABLE_AMBA_MAL)
#ifdef CONFIG_AMBALINK_BOOT_OS
static uint32_t cv_mal_id = AMBA_MAL_ID_CV_FD_SHARE;
#else
static uint32_t cv_mal_id = AMBA_MAL_ID_CV_FD;
#endif

static uint32_t CV_UtilityCmaMemInit(void)
{
    uint32_t retcode = ERRCODE_NONE;
    static uint32_t cma_init = 0U;

    if(cma_init == 0U) {
        retcode = AmbaMAL_Init();
        if(retcode == ERRCODE_NONE) {
            cma_init = 1U;
        }
    }
    return retcode;
}

/**
 *  @brief      The function allocates CMA buffer from the Linux kernel. The CMA buffer  is continuous in physical memory and aligned to 4K byte.
 *
 *  @param      req_size : [IN] The size of the request.
 *  @param      is_cached : [IN] The cache type of the request.
 *  @param      req_buf : [OUT] The buffer of the request.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemAlloc(uint32_t req_size, uint32_t is_cached, flexidag_memblk_t *req_buf)
{
    uint32_t retcode = ERRCODE_NONE;
    void *vaddr = NULL;
    uint32_t ret = 0;
    AMBA_MAL_BUF_s Buf;

    if(req_buf == NULL) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        retcode = CV_UtilityCmaMemInit();
        if(retcode == ERRCODE_NONE) {
            ret = AmbaMAL_Alloc(cv_mal_id, req_size, FD_MEM_ALIGN_SIZE, &Buf);
            if(ret != 0) {
                AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemAlloc() : AmbaMAL_Alloc fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_CMA_DEV_UNAVAILABLE;
            } else {
                if(is_cached == 1U) {
                    ret = AmbaMAL_Map(cv_mal_id, Buf.PhysAddr, Buf.RealSize, AMBA_MAL_ATTRI_CACHE, &vaddr);
                } else {
                    ret = AmbaMAL_Map(cv_mal_id, Buf.PhysAddr, Buf.RealSize, AMBA_MAL_ATTRI_NON_CACHE, &vaddr);
                }
                if (vaddr == NULL) {
                    AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemAlloc() : AmbaMAL_Map fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_CMA_ALLOC_FAIL;
                } else {
                    req_buf->pBuffer = vaddr;
                    req_buf->buffer_cacheable = is_cached;
                    req_buf->buffer_size = Buf.RealSize;
                    req_buf->buffer_daddr = Buf.PhysAddr;
                    req_buf->buffer_caddr = Buf.GlobalAddr;
                    if(req_buf->buffer_caddr == 0U) {
                        retcode = ERR_DRV_SCHDR_P2C_FAIL;
                    } else {
                        if(is_cached == 1U) {
                            if(AmbaCV_UtilityCmaMemClean(req_buf) != ERRCODE_NONE) {
                                AmbaPrint_PrintUInt5("[WARNING] AmbaCV_UtilityCmaMemAlloc() : AmbaCV_UtilityCmaMemClean fail ", 0U, 0U, 0U, 0U, 0U);
                            }
                        }
                    }
                }
            }
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemAlloc() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      The function remap CMA buffer which is allocated by another process.
 *
 *  @param      req_addr : [IN] The physical addr of the request.
 *  @param      req_size : [IN] The size of the request.
 *  @param      is_cached : [IN] The cache type of the request.
 *  @param      req_buf : [OUT] The buffer of the request.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemRemap(ulong req_addr, uint32_t req_size, uint32_t is_cached, flexidag_memblk_t *req_buf)
{
    uint32_t retcode = ERRCODE_NONE;
    void *vaddr = NULL;
    uint32_t ret = 0;
    uint32_t size_align;
    UINT64 GlobalAddr;
    AMBA_MAL_INFO_s Info;

    if(req_buf == NULL) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        retcode = CV_UtilityCmaMemInit();
        if(retcode == ERRCODE_NONE) {
            ret = AmbaMAL_GetInfoByPhys(req_addr, &Info);
            if(ret == 0U) {
                size_align = (uint32_t)((req_size + FD_MEM_ALIGN_SIZE - 1U)  & (~(FD_MEM_ALIGN_SIZE - 1U)));
                if(is_cached == 1U) {
                    ret = AmbaMAL_Map(Info.Id, req_addr, size_align, AMBA_MAL_ATTRI_CACHE, &vaddr);
                } else {
                    ret = AmbaMAL_Map(Info.Id, req_addr, size_align, AMBA_MAL_ATTRI_NON_CACHE, &vaddr);
                }

                if (vaddr == NULL) {
                    AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemRemap() : AmbaMAL_Map fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_CMA_ALLOC_FAIL;
                } else {
                    AmbaMAL_Phys2Global(req_addr, &GlobalAddr);
                    req_buf->pBuffer = vaddr;
                    req_buf->buffer_cacheable = is_cached;
                    req_buf->buffer_size = size_align;
                    req_buf->buffer_daddr = req_addr;
                    req_buf->buffer_caddr = GlobalAddr;
                    if(req_buf->buffer_caddr == 0U) {
                        retcode = ERR_DRV_SCHDR_P2C_FAIL;
                    }
                }
            } else {
                AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemRemap() : AmbaMAL_GetInfoByPhys fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
                retcode = ERR_INTF_CMA_MEMBLK_INVALID;
            }
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemRemap() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      This function unmap CMA buffer.
 *
 *  @param      buf : [IN] The buffer to unmap.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Invalid argument (null pointer).   \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemUnmap(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = 0;
    AMBA_MAL_INFO_s Info;

    if( (buf == NULL) || (buf->pBuffer == NULL)) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        ret = AmbaMAL_GetInfoByPhys(buf->buffer_daddr, &Info);
        if(ret == 0U) {
            ret = AmbaMAL_Unmap(Info.Id, buf->pBuffer, buf->buffer_size);
            if(ret != 0) {
                AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemUnmap() : AmbaMAL_Unmap fail ret = %d ", ret, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_CMA_FREE_FAIL;
            }
        } else {
            AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemUnmap() : AmbaMAL_GetInfoByPhys fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
            retcode = ERR_INTF_CMA_MEMBLK_INVALID;
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemUnmap() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      This function frees CMA buffer.
 *
 *  @param      buf : [IN] The buffer to free.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Invalid argument (null pointer).   \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemFree(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = 0;
    AMBA_MAL_BUF_s Buf;
    UINT64 GlobalAddr;

    if( (buf == NULL) || (buf->pBuffer == NULL)) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        AmbaMAL_Phys2Global(buf->buffer_daddr, &GlobalAddr);
        Buf.Id = cv_mal_id;
        Buf.PhysAddr = buf->buffer_daddr;
        Buf.GlobalAddr = GlobalAddr;
        Buf.Size = buf->buffer_size;
        Buf.RealSize = buf->buffer_size;
        Buf.Align = FD_MEM_ALIGN_SIZE;

        ret = AmbaMAL_Unmap(cv_mal_id, buf->pBuffer, buf->buffer_size);
        if(ret != 0) {
            AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemFree() : AmbaMAL_Unmap fail ret = %d ", ret, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_CMA_FREE_FAIL;
        } else {
            ret = AmbaMAL_Free(cv_mal_id, &Buf);
            if(ret != 0) {
                AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemFree() : AmbaMAL_Free fail ret = %d ", ret, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_CMA_FREE_FAIL;
            }
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemFree() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Arm cache used to clean the CMA buffer.
 *
 *  @param      buf : [IN] The buffer used to run cache clean operation.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemClean(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = 0;
    AMBA_MAL_INFO_s Info;

    if( (buf == NULL) || (buf->pBuffer == NULL)) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        ret = AmbaMAL_GetInfoByPhys(buf->buffer_daddr, &Info);
        if(ret == 0U) {
            ret = AmbaMAL_CacheClean(Info.Id, buf->pBuffer, buf->buffer_size);
            if(ret != 0) {
                retcode = ERR_DRV_SCHDR_CMA_SYNC_FAIL;
            }
        } else {
            AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemClean() : AmbaMAL_GetInfoByPhys fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
            retcode = ERR_INTF_CMA_MEMBLK_INVALID;
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemClean() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Arm cache invalidates the CMA buffer.
 *
 *  @param      buf : [IN] The buffer used to run cache invalidates operation.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemInvalid(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret = 0;
    AMBA_MAL_INFO_s Info;

    if( (buf == NULL) || (buf->pBuffer == NULL)) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        ret = AmbaMAL_GetInfoByPhys(buf->buffer_daddr, &Info);
        if(ret == 0U) {
            ret = AmbaMAL_CacheInvalid(Info.Id, buf->pBuffer, buf->buffer_size);
            if(ret != 0) {
                retcode = ERR_DRV_SCHDR_CMA_SYNC_FAIL;
            }
        } else {
            AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemInvalid() : AmbaMAL_GetInfoByPhys fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
            retcode = ERR_INTF_CMA_MEMBLK_INVALID;
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemInvalid() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}
#else
#if defined(CONFIG_QNX)
static int32_t cv_user_mem_fd = -1;
static int32_t cv_user_mem_remap_fd = -1;
static struct cache_ctrl cache_info;
static uint32_t cache_info_init = 0U;

static uint32_t CV_UtilityCmaMemInit(void)
{
    uint32_t retcode = ERRCODE_NONE;
    static uint32_t cma_init = 0U;

    if(cma_init == 0U) {
        retcode = ambacv_att_init();
        if(retcode == ERRCODE_NONE) {
            if(cv_user_mem_fd < 0) {
                cv_user_mem_fd = posix_typed_mem_open( "/ram/flexidag_sys", O_RDWR,POSIX_TYPED_MEM_ALLOCATE_CONTIG);
                if(cv_user_mem_fd < 0) {
                    retcode = ERR_DRV_SCHDR_CMA_DEV_UNAVAILABLE;
                    AmbaPrint_PrintUInt5("[ERROR] CV_UtilityCmaMemInit() : Open ALLOCATE_CONTIG fail fd = %d errno = %d", cv_user_mem_fd, errno, 0U, 0U, 0U);
                }
            }

            if(cv_user_mem_remap_fd < 0) {
                cv_user_mem_remap_fd = posix_typed_mem_open( "/ram/flexidag_sys", O_RDWR,POSIX_TYPED_MEM_MAP_ALLOCATABLE);
                if(cv_user_mem_remap_fd < 0) {
                    retcode = ERR_DRV_SCHDR_CMA_DEV_UNAVAILABLE;
                    AmbaPrint_PrintUInt5("[ERROR] CV_UtilityCmaMemInit() : Open MAP_ALLOCATABLE fail fd = %d errno = %d", cv_user_mem_remap_fd, errno, 0U, 0U, 0U);
                }
            }

            if(retcode == ERRCODE_NONE) {
                cma_init = 1U;
            }
        }
    }
    return retcode;
}

/**
 *  @brief      The function allocates CMA buffer from the Linux kernel. The CMA buffer  is continuous in physical memory and aligned to 4K byte.
 *
 *  @param      req_size : [IN] The size of the request.
 *  @param      is_cached : [IN] The cache type of the request.
 *  @param      req_buf : [OUT] The buffer of the request.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemAlloc(uint32_t req_size, uint32_t is_cached, flexidag_memblk_t *req_buf)
{
    uint32_t retcode = ERRCODE_NONE;
    struct posix_typed_mem_info info;
    void *vaddr;
    off_t offset;
    int32_t ret = 0;
    uint32_t size_align;

    if(req_buf == NULL) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        retcode = CV_UtilityCmaMemInit();
        if(retcode == ERRCODE_NONE) {
            ret = posix_typed_mem_get_info(cv_user_mem_fd, &info);
            if(ret != 0) {
                AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemAlloc() : posix_typed_mem_get_info fail ret = %d errno = %d", ret, errno, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_CMA_DEV_UNAVAILABLE;
            } else {
                size_align = (uint32_t)((req_size + FD_MEM_ALIGN_SIZE - 1U)  & (~(FD_MEM_ALIGN_SIZE - 1U)));
                if(info.posix_tmi_length < size_align) {
                    AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemAlloc() : info.posix_tmi_length(0x%x) < req_size(0x%x)", (uint32_t)info.posix_tmi_length, (uint32_t)size_align, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_CMA_ALLOC_TOTAL_SIZE_OUT_OF_RANGE;
                } else {
                    if(is_cached == 1U) {
                        vaddr = mmap( NULL, size_align, PROT_READ | PROT_WRITE,MAP_SHARED, cv_user_mem_fd, 0);
                    } else {
                        vaddr = mmap( NULL, size_align, PROT_NOCACHE | PROT_READ | PROT_WRITE,MAP_SHARED, cv_user_mem_fd, 0);
                    }
                    if (vaddr == MAP_FAILED) {
                        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemAlloc() : mmap fail errno = %d ", errno, 0U, 0U, 0U, 0U);
                        retcode = ERR_DRV_SCHDR_CMA_ALLOC_FAIL;
                    } else {
                        ret = mem_offset(vaddr, NOFD, 1, &offset, 0);
                        if(ret != 0) {
                            AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemAlloc() : mem_offset fail ret = %d errno = %d", ret, errno, 0U, 0U, 0U);
                            retcode = ERR_DRV_SCHDR_CMA_BUF_INVALID;
                        } else {
                            req_buf->pBuffer = vaddr;
                            req_buf->buffer_cacheable = is_cached;
                            req_buf->buffer_size = size_align;
                            req_buf->buffer_daddr = offset;
                            req_buf->buffer_caddr = ambacv_p2c(req_buf->buffer_daddr);
                            if(req_buf->buffer_caddr == 0U) {
                                retcode = ERR_DRV_SCHDR_P2C_FAIL;
                            } else {
                                if(is_cached == 1U) {
                                    if(AmbaCV_UtilityCmaMemClean(req_buf) != ERRCODE_NONE) {
                                        AmbaPrint_PrintUInt5("[WARNING] AmbaCV_UtilityCmaMemAlloc() : AmbaCV_UtilityCmaMemClean fail ", 0U, 0U, 0U, 0U, 0U);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemAlloc() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      The function remap CMA buffer which is allocated by another process.
 *
 *  @param      req_addr : [IN] The physical addr of the request.
 *  @param      req_size : [IN] The size of the request.
 *  @param      is_cached : [IN] The cache type of the request.
 *  @param      req_buf : [OUT] The buffer of the request.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemRemap(ulong req_addr, uint32_t req_size, uint32_t is_cached, flexidag_memblk_t *req_buf)
{
    uint32_t retcode = ERRCODE_NONE;
    struct posix_typed_mem_info info;
    void *vaddr;
    off_t offset;
    int32_t ret = 0;
    uint32_t size_align;

    if(req_buf == NULL) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        retcode = CV_UtilityCmaMemInit();
        if(retcode == ERRCODE_NONE) {
            size_align = (uint32_t)((req_size + FD_MEM_ALIGN_SIZE - 1U)  & (~(FD_MEM_ALIGN_SIZE - 1U)));
            if(is_cached == 1U) {
                vaddr = mmap( NULL, size_align, PROT_READ | PROT_WRITE,MAP_SHARED, cv_user_mem_remap_fd, req_addr);
            } else {
                vaddr = mmap( NULL, size_align, PROT_NOCACHE | PROT_READ | PROT_WRITE,MAP_SHARED, cv_user_mem_remap_fd, req_addr);
            }
            if (vaddr == MAP_FAILED) {
                AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemRemap() : mmap fail errno = %d ", errno, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_CMA_ALLOC_FAIL;
            } else {
                req_buf->pBuffer = vaddr;
                req_buf->buffer_cacheable = is_cached;
                req_buf->buffer_size = size_align;
                req_buf->buffer_daddr = req_addr;
                req_buf->buffer_caddr = ambacv_p2c(req_buf->buffer_daddr);
                if(req_buf->buffer_caddr == 0U) {
                    retcode = ERR_DRV_SCHDR_P2C_FAIL;
                }
            }
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemRemap() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      This function unmap CMA buffer.
 *
 *  @param      buf : [IN] The buffer to unmap.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Invalid argument (null pointer).   \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemUnmap(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret = 0;

    if( (buf == NULL) || (buf->pBuffer == NULL)) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        ret = munmap(buf->pBuffer, buf->buffer_size);
        if(ret != 0) {
            AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemUnmap() : munmap fail ret = %d ennro = %d", ret, errno, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_CMA_FREE_FAIL;
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemUnmap() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      This function frees CMA buffer.
 *
 *  @param      buf : [IN] The buffer to free.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Invalid argument (null pointer).   \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemFree(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret = 0;

    if( (buf == NULL) || (buf->pBuffer == NULL)) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        ret = munmap(buf->pBuffer, buf->buffer_size);
        if(ret != 0) {
            AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemFree() : munmap fail ret = %d ennro = %d", ret, errno, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_CMA_FREE_FAIL;
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemFree() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Arm cache used to clean the CMA buffer.
 *
 *  @param      buf : [IN] The buffer used to run cache clean operation.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemClean(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret = 0;

    if( (buf == NULL) || (buf->pBuffer == NULL)) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        if(cache_info_init == 0U) {
            ret = cache_init(0, &cache_info, NULL);
            if(ret != 0) {
                retcode = ERR_DRV_SCHDR_CACHE_INIT_FAIL;
                AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemClean() : cache_init fail ret = %d ennro = %d", ret, errno, 0U, 0U, 0U);
            } else {
                cache_info_init = 1U;
            }
        }

        if(retcode == ERRCODE_NONE) {
            CACHE_FLUSH(&cache_info, buf->pBuffer, buf->buffer_daddr, buf->buffer_size);
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemClean() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Arm cache invalidates the CMA buffer.
 *
 *  @param      buf : [IN] The buffer used to run cache invalidates operation.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemInvalid(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret = 0;

    if( (buf == NULL) || (buf->pBuffer == NULL)) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        if(cache_info_init == 0U) {
            ret = cache_init(0, &cache_info, NULL);
            if(ret != 0) {
                retcode = ERR_DRV_SCHDR_CACHE_INIT_FAIL;
                AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemClean() : cache_init fail ret = %d ennro = %d", ret, errno, 0U, 0U, 0U);
            } else {
                cache_info_init = 1U;
            }
        }

        if(retcode == ERRCODE_NONE) {
            CACHE_INVAL(&cache_info, buf->pBuffer, buf->buffer_daddr, buf->buffer_size);
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemInvalid() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

#elif defined(CONFIG_THREADX)
static char *pool_buf = NULL;
static uint32_t  pool_size = 0U;

static void CV_UtilityCmaMemGetRam(void)
{
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    const void *fdt;
    int32_t ret, offset, len;
    const char *chr1;
    const uint32_t *ptr1;
    const uint64_t *ptr64;
    const struct fdt_property *prop = NULL;
    ULONG dtb_addr;
    ULONG pa = 0UL,va = 0UL;
    uint32_t size = 0U;
#ifdef CONFIG_AMBALINK_BOOT_OS
    ULONG CVRtosMemAddr = 0UL,CVRtosMemSize = 0UL;
#else
    ULONG CVSchdrMemSize = 0UL,CVSysMemAddr = 0UL,CVSysMemSize = 0UL;
#endif

    dtb_addr = (ULONG)CONFIG_DTB_LOADADDR;
    AmbaMisra_TypeCast(&fdt, &dtb_addr);

    ret = AmbaFDT_CheckHeader(fdt);
    if (ret == 0) {
#ifdef CONFIG_AMBALINK_BOOT_OS
        offset = AmbaFDT_PathOffset(fdt, "flexidag_rtos");
        if (offset >= 0) {
            prop = AmbaFDT_GetProperty(fdt, offset, "reg", &len);
            if ((prop != NULL) && (len == 8)) {
                chr1 = (const char *)&prop->data[0];
                AmbaMisra_TypeCast(&ptr1, &chr1);
                CVRtosMemAddr = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
                CVRtosMemSize = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[1]);
            } else if ((prop != NULL) && (len == 16)) {
                chr1 = (const char *)&prop->data[0];
                AmbaMisra_TypeCast(&ptr64, &chr1);
                CVRtosMemAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
                CVRtosMemSize = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[1]);
            } else {
                CVRtosMemAddr = 0U;
                CVRtosMemSize = 0U;
            }
        }

        pa = CVRtosMemAddr;
        size = (uint32_t) CVRtosMemSize;
#else
        offset = AmbaFDT_PathOffset(fdt, "scheduler");
        if (offset >= 0) {
            prop = AmbaFDT_GetProperty(fdt, offset, "cv_schdr_size", &len);
            if ((prop != NULL) && (len == 4)) {
                chr1 = (const char *)&prop->data[0];
                AmbaMisra_TypeCast(&ptr1, &chr1);
                CVSchdrMemSize= (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
            } else if ((prop != NULL) && (len == 8)) {
                chr1 = (const char *)&prop->data[0];
                AmbaMisra_TypeCast(&ptr64, &chr1);
                CVSchdrMemSize= (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
            } else {
                CVSchdrMemSize= 0U;
            }
        }

        offset = AmbaFDT_PathOffset(fdt, "flexidag_sys");
        if (offset >= 0) {
            prop = AmbaFDT_GetProperty(fdt, offset, "reg", &len);
            if ((prop != NULL) && (len == 8)) {
                chr1 = (const char *)&prop->data[0];
                AmbaMisra_TypeCast(&ptr1, &chr1);
                CVSysMemAddr = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
                CVSysMemSize = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[1]);
            } else if ((prop != NULL) && (len == 16)) {
                chr1 = (const char *)&prop->data[0];
                AmbaMisra_TypeCast(&ptr64, &chr1);
                CVSysMemAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0]);
                CVSysMemSize = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[1]);
            } else {
                CVSysMemAddr = 0U;
                CVSysMemSize = 0U;
            }
        }

        pa = CVSysMemAddr + CVSchdrMemSize;
        size = (uint32_t)(CVSysMemSize - CVSchdrMemSize);
#endif
        if(AmbaMMU_PhysToVirt(pa,&va) != 0U) {
            AmbaPrint_PrintUInt5("CV_UtilityCmaMemGetRam() : AmbaMMU_PhysToVirt fail ", 0U, 0U, 0U, 0U, 0U);
        }
        AmbaMisra_TypeCast(&pool_buf, &va);
        pool_size = size;
    }
#else
#ifdef CONFIG_AMBALINK_BOOT_OS
    extern char __cv_rtos_user_start;

    pool_buf = &__cv_rtos_user_start;
    pool_size = (uint32_t) (CONFIG_CV_MEM_RTOS_SIZE);
#else
    extern char __cv_schdr_end;

    pool_buf = &__cv_schdr_end;
    pool_size = (uint32_t) (CONFIG_CV_MEM_SIZE - CONFIG_CV_MEM_SCHDR_SIZE);
#endif
#endif
}

static mutex_t cma_mutex;
static flexidag_memblk_t cma_blk;
static uint32_t cma_curr = 0U;

extern void AmbaCV_UtilityCmaMemReset(void);
void AmbaCV_UtilityCmaMemReset(void)
{
    cma_curr = 0U;
}

static uint32_t CV_UtilityCmaMemInit(void)
{
    static uint32_t cma_inited = 0U;
    static uint32_t mutex_inited = 0U;
    uint32_t retcode = ERRCODE_NONE;
    ULONG addr;
    ULONG Paddr = 0UL;

    if (mutex_inited == 0U) {
        retcode = mutex_init(&cma_mutex);
        if (retcode != ERRCODE_NONE) {
            retcode = ERR_DRV_SCHDR_MUTEX_INIT_FAIL;
        } else {
            mutex_inited = 1U;
        }
    }

    if (mutex_inited == 1U) {
        if( mutex_lock(&cma_mutex) != ERRCODE_NONE) {
            retcode = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
        } else {
            if (cma_inited == 0U) {
                CV_UtilityCmaMemGetRam();
                if(pool_buf != NULL) {
                    retcode = ambacv_att_init();
                    if(retcode == ERRCODE_NONE) {
                        cma_blk.pBuffer = &pool_buf[0];
                        AmbaMisra_TypeCast(&addr, &cma_blk.pBuffer);
                        if( AmbaMMU_VirtToPhys(addr, &Paddr) != 0U) {
                            retcode = ERR_DRV_SCHDR_CMA_BUF_INVALID;
                        } else {
                            cma_blk.buffer_cacheable = 1U;
                            cma_blk.buffer_daddr = Paddr;
                            cma_blk.buffer_size = pool_size;
                            cma_curr = 0U;
                            cma_inited = 1U;
                        }
                        AmbaPrint_PrintUInt5("CV_UtilityCmaMemInit() : CMA totoal PA 0x%x VA 0x%x Size 0x%x ", (uint32_t)cma_blk.buffer_daddr, (uint32_t)addr, (uint32_t)cma_blk.buffer_size, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("CV_UtilityCmaMemInit() : pool_buf == NULL ", 0U, 0U, 0U, 0U, 0U);
                }
            }
            if( mutex_unlock(&cma_mutex) != ERRCODE_NONE) {
                if(retcode == ERRCODE_NONE) {
                    retcode = ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL;
                }
            }
        }
    }

    return retcode;
}
/**
 *  @brief      The function allocates CMA buffer from the Linux kernel. The CMA buffer  is continuous in physical memory and aligned to 4K byte.
 *
 *  @param      req_size : [IN] The size of the request.
 *  @param      is_cached : [IN] The cache type of the request.
 *  @param      req_buf : [OUT] The buffer of the request.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemAlloc(uint32_t req_size, uint32_t is_cached, flexidag_memblk_t *req_buf)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t size_align;
    ULONG addr;

    if(req_buf == NULL) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        retcode = CV_UtilityCmaMemInit();
        if(retcode == ERRCODE_NONE) {
            if(mutex_lock(&cma_mutex) != ERRCODE_NONE) {
                retcode = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
            } else {
                size_align = (uint32_t)((req_size + FD_MEM_ALIGN_SIZE - 1U)  & (~(FD_MEM_ALIGN_SIZE - 1U)));
                if ((size_align + cma_curr) > pool_size) {
                    AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemAlloc() : cma_size(0x%x) < req_size(0x%x)", (pool_size - cma_curr), (uint32_t)size_align, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_CMA_ALLOC_TOTAL_SIZE_OUT_OF_RANGE;
                } else {
                    if(is_cached != cma_blk.buffer_cacheable) {
                        AmbaPrint_PrintUInt5("[WARNING] AmbaCV_UtilityCmaMemAlloc() : alloc cache type %d != cma support cache type", is_cached, cma_blk.buffer_cacheable, 0U, 0U, 0U);
                    }
                    req_buf->buffer_cacheable = cma_blk.buffer_cacheable;
                    req_buf->buffer_daddr = cma_blk.buffer_daddr+cma_curr;
                    req_buf->pBuffer = &cma_blk.pBuffer[cma_curr];
                    req_buf->buffer_size = size_align;
                    req_buf->buffer_caddr = (ulong)ambacv_p2c(req_buf->buffer_daddr);
                    if(req_buf->buffer_caddr == 0U) {
                        retcode = ERR_DRV_SCHDR_P2C_FAIL;
                    } else {
                        cma_curr += size_align;
                        AmbaMisra_TypeCast(&addr, &req_buf->pBuffer);
                        AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_UtilityCmaMemAlloc() : PA 0x%x VA 0x%x Size 0x%x ", (uint32_t)req_buf->buffer_daddr, (uint32_t)addr, (uint32_t)req_buf->buffer_size, 0U, 0U);
                        if(cma_blk.buffer_cacheable == 1U) {
                            if(AmbaCV_UtilityCmaMemClean(req_buf) != ERRCODE_NONE) {
                                AmbaPrint_PrintUInt5("[WARNING] AmbaCV_UtilityCmaMemAlloc() : AmbaCV_UtilityCmaMemClean fail ", 0U, 0U, 0U, 0U, 0U);
                            }
                        }
                    }
                }
                if( mutex_unlock(&cma_mutex) != ERRCODE_NONE) {
                    if(retcode == ERRCODE_NONE) {
                        retcode = ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL;
                    }
                }
            }
        }

    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemAlloc() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      The function remap CMA buffer which is allocated by another process.
 *
 *  @param      req_addr : [IN] The physical addr of the request.
 *  @param      req_size : [IN] The size of the request.
 *  @param      is_cached : [IN] The cache type of the request.
 *  @param      req_buf : [OUT] The buffer of the request.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemRemap(ulong req_addr, uint32_t req_size, uint32_t is_cached, flexidag_memblk_t *req_buf)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t size_align;
    ulong offset;
    ULONG addr;

    if(req_buf == NULL) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        retcode = CV_UtilityCmaMemInit();
        if(retcode == ERRCODE_NONE) {
            if(mutex_lock(&cma_mutex) != ERRCODE_NONE) {
                retcode = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
            } else {
                offset = req_addr - cma_blk.buffer_daddr;
                size_align = (uint32_t)((req_size + FD_MEM_ALIGN_SIZE - 1U)  & (~(FD_MEM_ALIGN_SIZE - 1U)));
                if ((size_align + (uint32_t)offset) > pool_size) {
                    AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemRemap() : cma_size(0x%x) < req_size(0x%x)", (pool_size - (uint32_t)offset), (uint32_t)size_align, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_CMA_ALLOC_TOTAL_SIZE_OUT_OF_RANGE;
                } else {
                    if(is_cached != cma_blk.buffer_cacheable) {
                        AmbaPrint_PrintUInt5("[WARNING] AmbaCV_UtilityCmaMemRemap() : alloc cache type %d != cma support cache type", is_cached, cma_blk.buffer_cacheable, 0U, 0U, 0U);
                    }
                    req_buf->buffer_cacheable = cma_blk.buffer_cacheable;
                    req_buf->buffer_daddr = cma_blk.buffer_daddr + offset;
                    req_buf->pBuffer = &cma_blk.pBuffer[offset];
                    req_buf->buffer_size = size_align;
                    req_buf->buffer_caddr = (ulong)ambacv_p2c(req_buf->buffer_daddr);
                    if(req_buf->buffer_caddr == 0U) {
                        retcode = ERR_DRV_SCHDR_P2C_FAIL;
                    } else {
                        AmbaMisra_TypeCast(&addr, &req_buf->pBuffer);
                        AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_UtilityCmaMemRemap() : PA 0x%x VA 0x%x Size 0x%x ", (uint32_t)req_buf->buffer_daddr, (uint32_t)addr, (uint32_t)req_buf->buffer_size, 0U, 0U);
                    }
                }
                if( mutex_unlock(&cma_mutex) != ERRCODE_NONE) {
                    if(retcode == ERRCODE_NONE) {
                        retcode = ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL;
                    }
                }
            }
        }

    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemAlloc() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      This function unmap CMA buffer.
 *
 *  @param      buf : [IN] The buffer to unmap.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Invalid argument (null pointer).   \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemUnmap(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) buf;
    AmbaMisra_TouchUnused(buf);

    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      This function frees CMA buffer.
 *
 *  @param      buf : [IN] The buffer to free.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Invalid argument (null pointer).   \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemFree(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) buf;
    AmbaMisra_TouchUnused(buf);
    AmbaPrint_PrintUInt5("[INFO] AmbaCV_UtilityCmaMemFree() : Only in sw reset flow can call ", 0U, 0U, 0U, 0U, 0U);
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Arm cache used to clean the CMA buffer.
 *
 *  @param      buf : [IN] The buffer used to run cache clean operation.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemClean(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = 0U;
    ULONG addr;

    (void) buf;
    if( (buf == NULL) || (buf->pBuffer == NULL)) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        AmbaMisra_TypeCast(&addr, &buf->pBuffer);
        ret = AmbaCache_DataClean(addr, buf->buffer_size);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_CMA_SYNC_FAIL;
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemClean() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Arm cache invalidates the CMA buffer.
 *
 *  @param      buf : [IN] The buffer used to run cache invalidates operation.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemInvalid(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = 0U;
    ULONG addr;

    (void) buf;
    if( (buf == NULL) || (buf->pBuffer == NULL)) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        AmbaMisra_TypeCast(&addr, &buf->pBuffer);
        ret = AmbaCache_DataInvalidate(addr, buf->buffer_size);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_CMA_SYNC_FAIL;
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemInvalid() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

#else
#define PAGE_ALIGN_SIZE   4096UL

#if !defined(ENV_IS_PACE_LINUX)
static int cavalry_fd = -1;

static uint32_t CV_UtilityCmaMemInit(void)
{
    uint32_t retcode = ERRCODE_NONE;
    static uint32_t cma_init = 0U;

    if(cma_init == 0U) {
        retcode = ambacv_att_init();
        if(retcode == ERRCODE_NONE) {
            if(cavalry_fd < 0) {
                if ((cavalry_fd = open("/dev/ambacv", O_SYNC | O_RDWR, 0)) < 0) {
                    printf("AmbaCV_UtilityCmaMemAlloc : /dev/ambacv open fail \n");
                    retcode = ERR_DRV_SCHDR_CMA_DEV_UNAVAILABLE;
                } else {
                    cma_init = 1U;
                }
            }
        }
    }
    return retcode;
}

/**
 *  @brief      The function allocates CMA buffer from the Linux kernel. The CMA buffer  is continuous in physical memory and aligned to 4K byte.
 *
 *  @param      req_size : [IN] The size of the request.
 *  @param      is_cached : [IN] The cache type of the request.
 *  @param      req_buf : [OUT] The buffer of the request.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemAlloc(uint32_t req_size, uint32_t is_cached, flexidag_memblk_t *req_buf)
{
    uint32_t retcode = ERRCODE_NONE;
    struct cavalry_mem mem;
    uint32_t size_align;

    if(req_buf == NULL) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        retcode = CV_UtilityCmaMemInit();
        if(retcode == ERRCODE_NONE) {
            size_align = (uint32_t)((req_size + FD_MEM_ALIGN_SIZE - 1U) & (~(FD_MEM_ALIGN_SIZE - 1U)));
            memset(&mem, 0, sizeof(mem));
            mem.cache_en = is_cached;
            mem.length = size_align;
            if(ioctl(cavalry_fd, CAVALRY_ALLOC_MEM, &mem) < 0) {
                printf("AmbaCV_UtilityCmaMemAlloc : CAVALRY_ALLOC_MEM fail size = 0x%x\n",size_align);
                retcode = ERR_DRV_SCHDR_CMA_ALLOC_FAIL;
            } else {
                req_buf->pBuffer = (char*)mmap(NULL, mem.length, PROT_READ | PROT_WRITE,MAP_SHARED, cavalry_fd, mem.offset);
                if(req_buf->pBuffer == MAP_FAILED) {
                    printf("AmbaCV_UtilityCmaMemAlloc : mmap fail pa = 0x%lx len = 0x%lx\n",mem.offset,mem.length);
                    retcode = ERR_DRV_SCHDR_CMA_MAP_FAIL;
                } else {
                    req_buf->buffer_cacheable = mem.cache_en;
                    req_buf->buffer_size = mem.length;
                    req_buf->buffer_daddr = mem.offset;
                    req_buf->buffer_caddr = ambacv_p2c(req_buf->buffer_daddr);
                    if(req_buf->buffer_caddr == 0U) {
                        retcode = ERR_DRV_SCHDR_P2C_FAIL;
                    } else {
                        if(is_cached == 1U) {
                            if(AmbaCV_UtilityCmaMemClean(req_buf) != ERRCODE_NONE) {
                                AmbaPrint_PrintUInt5("[WARNING] AmbaCV_UtilityCmaMemAlloc() : AmbaCV_UtilityCmaMemClean fail ", 0U, 0U, 0U, 0U, 0U);
                            }
                        }
                    }
                }
            }
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemAlloc() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      The function remap CMA buffer which is allocated by another process.
 *
 *  @param      req_addr : [IN] The physical addr of the request.
 *  @param      req_size : [IN] The size of the request.
 *  @param      is_cached : [IN] The cache type of the request.
 *  @param      req_buf : [OUT] The buffer of the request.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemRemap(ulong req_addr, uint32_t req_size, uint32_t is_cached, flexidag_memblk_t *req_buf)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t size_align;
    ulong addr_align;
    char *vaddr;
    uint32_t cache;

    if(req_buf == NULL) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        retcode = CV_UtilityCmaMemInit();
        if(retcode == ERRCODE_NONE) {
            addr_align = req_addr & (~(PAGE_ALIGN_SIZE - 1UL));
            size_align = (uint32_t)(((uint32_t)(req_addr - addr_align) + req_size + FD_MEM_ALIGN_SIZE - 1U) & (~(FD_MEM_ALIGN_SIZE - 1U)));
            vaddr = (char*)mmap(NULL, size_align, PROT_READ | PROT_WRITE,MAP_SHARED, cavalry_fd, addr_align);
            if(vaddr == MAP_FAILED) {
                printf("AmbaCV_UtilityCmaMemRemap : mmap fail pa = 0x%lx len = 0x%x\n", req_addr, req_size);
                retcode = ERR_DRV_SCHDR_CMA_MAP_FAIL;
            } else {
                retcode = schdr_get_mmb_cache(req_addr, &cache);
                if (retcode == ERRCODE_NONE) {
                    if(is_cached != cache) {
                        AmbaPrint_PrintUInt5("[WARNING] AmbaCV_UtilityCmaMemRemap() : invalid cache type %d, use default cache type %d", is_cached, cache, 0U, 0U, 0U);
                    }
                    req_buf->pBuffer = &vaddr[req_addr - addr_align];
                    req_buf->buffer_cacheable = cache;
                    req_buf->buffer_size = size_align - (uint32_t)(req_addr - addr_align);
                    req_buf->buffer_daddr = req_addr;
                    req_buf->buffer_caddr = ambacv_p2c(req_buf->buffer_daddr);
                    if(req_buf->buffer_caddr == 0U) {
                        retcode = ERR_DRV_SCHDR_P2C_FAIL;
                    }
                }
            }
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemRemap() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      This function unmap CMA buffer.
 *
 *  @param      buf : [IN] The buffer to unmap.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Invalid argument (null pointer).   \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemUnmap(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;
    ulong addr_align;
    ulong addr;
    void *paddr;

    if( (buf == NULL) || (buf->pBuffer == NULL)) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        if(cavalry_fd < 0) {
            if ((cavalry_fd = open("/dev/ambacv", O_SYNC | O_RDWR, 0)) < 0) {
                printf("AmbaCV_UtilityCmaMemUnmap : /dev/ambacv open fail \n");
                retcode = ERR_DRV_SCHDR_CMA_DEV_UNAVAILABLE;
            }
        }

        if(retcode == ERRCODE_NONE) {
            AmbaMisra_TypeCast(&addr, &buf->pBuffer);
            addr_align = addr & (~(PAGE_ALIGN_SIZE - 1UL));
            AmbaMisra_TypeCast(&paddr, &addr_align);
            if (munmap(paddr, buf->buffer_size + (addr - addr_align)) != 0) {
                printf("AmbaCV_UtilityCmaMemUnmap : munmap fail va = 0x%p pa = 0x%lx len = 0x%x\n", buf->pBuffer, buf->buffer_daddr, buf->buffer_size);
                retcode = ERR_DRV_SCHDR_CMA_UNMAP_FAIL;
            }
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemUnmap() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      This function frees CMA buffer.
 *
 *  @param      buf : [IN] The buffer to free.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Invalid argument (null pointer).   \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemFree(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;
    ulong addr_align;
    ulong addr;
    void *paddr;

    if( (buf == NULL) || (buf->pBuffer == NULL)) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        if(cavalry_fd < 0) {
            if ((cavalry_fd = open("/dev/ambacv", O_SYNC | O_RDWR, 0)) < 0) {
                printf("AmbaCV_UtilityCmaMemFree : /dev/ambacv open fail \n");
                retcode = ERR_DRV_SCHDR_CMA_DEV_UNAVAILABLE;
            }
        }

        if(retcode == ERRCODE_NONE) {
            AmbaMisra_TypeCast(&addr, &buf->pBuffer);
            addr_align = addr & (~(PAGE_ALIGN_SIZE - 1UL));
            AmbaMisra_TypeCast(&paddr, &addr_align);
            if (munmap(paddr, buf->buffer_size + (addr - addr_align)) != 0) {
                printf("AmbaCV_UtilityCmaMemFree : munmap fail va = 0x%p pa = 0x%lx len = 0x%x\n", buf->pBuffer, buf->buffer_daddr, buf->buffer_size);
                retcode = ERR_DRV_SCHDR_CMA_UNMAP_FAIL;
            }
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemFree() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Arm cache used to clean the CMA buffer.
 *
 *  @param      buf : [IN] The buffer used to run cache clean operation.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemClean(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;
    struct cavalry_cache_mem cache;

    if( (buf == NULL) || (buf->pBuffer == NULL)) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        if(cavalry_fd < 0) {
            if ((cavalry_fd = open("/dev/ambacv", O_SYNC | O_RDWR, 0)) < 0) {
                printf("AmbaCV_UtilityCmaMemAlloc : /dev/ambacv open fail \n");
                retcode = ERR_DRV_SCHDR_CMA_DEV_UNAVAILABLE;
            }
        }

        if(retcode == ERRCODE_NONE) {
            if (((uint64_t)buf->buffer_daddr >= rtos_region_base) && ((uint64_t)(buf->buffer_daddr + buf->buffer_size) < rtos_region_base + rtos_region_size)) {
                retcode = ambacv_cache_clean(ambacv_p2v(buf->buffer_daddr), buf->buffer_size);
            } else {
                memset(&cache, 0, sizeof(cache));
                cache.clean = 1;
                cache.length = buf->buffer_size;
                cache.offset = buf->buffer_daddr;
                if(ioctl(cavalry_fd, CAVALRY_SYNC_CACHE_MEM, &cache) < 0) {
                    printf("AmbaCV_UtilityCmaMemClean : CAVALRY_SYNC_CACHE_MEM fail pa = 0x%lx len = 0x%lx\n",cache.offset,cache.length);
                    retcode = ERR_DRV_SCHDR_CMA_SYNC_FAIL;
                }
            }
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemClean() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Arm cache invalidates the CMA buffer.
 *
 *  @param      buf : [IN] The buffer used to run cache invalidates operation.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemInvalid(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;
    struct cavalry_cache_mem cache;

    if( (buf == NULL) || (buf->pBuffer == NULL)) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        if(cavalry_fd < 0) {
            if ((cavalry_fd = open("/dev/ambacv", O_SYNC | O_RDWR, 0)) < 0) {
                printf("AmbaCV_UtilityCmaMemInvalid : /dev/ambacv open fail \n");
                retcode = ERR_DRV_SCHDR_CMA_DEV_UNAVAILABLE;
            }
        }

        if(retcode == ERRCODE_NONE) {
            if (((uint64_t)buf->buffer_daddr >= rtos_region_base) && ((uint64_t)(buf->buffer_daddr + buf->buffer_size) < rtos_region_base + rtos_region_size)) {
                retcode = ambacv_cache_invalidate(ambacv_p2v(buf->buffer_daddr), buf->buffer_size);
            } else {
                memset(&cache, 0, sizeof(cache));
                cache.invalid = 1;
                cache.length = buf->buffer_size;
                cache.offset = buf->buffer_daddr;
                if(ioctl(cavalry_fd, CAVALRY_SYNC_CACHE_MEM, &cache) < 0) {
                    printf("AmbaCV_UtilityCmaMemInvalid : CAVALRY_SYNC_CACHE_MEM fail pa = 0x%lx len = 0x%lx\n",cache.offset,cache.length);
                    retcode = ERR_DRV_SCHDR_CMA_SYNC_FAIL;
                }
            }
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemInvalid() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}
#else
extern int32_t drv_cavalry_query_buf(void *pQueryBuf);

static mutex_t cma_mutex;
static flexidag_memblk_t cma_blk;
static uint32_t cma_curr = 0U;

static uint32_t CV_UtilityCmaMemInit(void)
{
    static uint32_t cma_inited = 0U;
    static uint32_t mutex_inited = 0U;
    uint32_t retcode = ERRCODE_NONE;
    struct cavalry_querybuf cqbuf;
    ULONG addr;

    if (mutex_inited == 0U) {
        retcode = mutex_init(&cma_mutex);
        if (retcode != ERRCODE_NONE) {
            retcode = ERR_DRV_SCHDR_MUTEX_INIT_FAIL;
        } else {
            mutex_inited = 1U;
        }
    }

    if (mutex_inited == 1U) {
        if( mutex_lock(&cma_mutex) != ERRCODE_NONE) {
            retcode = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
        } else {
            if (cma_inited == 0U) {
                memset(&cqbuf, 0, sizeof(cqbuf));
                cqbuf.buf = CAVALRY_MEM_USER;
                drv_cavalry_query_buf(&cqbuf);
                if(cqbuf.length != 0U) {
                    cma_blk.pBuffer = (char *)ambacv_p2v(cqbuf.offset);
                    cma_blk.buffer_cacheable = 0U;
                    cma_blk.buffer_daddr = cqbuf.offset;
                    cma_blk.buffer_size = cqbuf.length;
                    cma_curr = 0U;
                    cma_inited = 1U;
                    AmbaMisra_TypeCast(&addr, &cma_blk.pBuffer);
                    AmbaPrint_PrintUInt5("CV_UtilityCmaMemInit() : CMA totoal PA 0x%x VA 0x%x Size 0x%x ", (uint32_t)cma_blk.buffer_daddr, (uint32_t)addr, (uint32_t)cma_blk.buffer_size, 0U, 0U);
                } else {
                    AmbaPrint_PrintUInt5("CV_UtilityCmaMemInit() : cqbuf is invalid ", 0U, 0U, 0U, 0U, 0U);
                }
            }
            if( mutex_unlock(&cma_mutex) != ERRCODE_NONE) {
                if(retcode == ERRCODE_NONE) {
                    retcode = ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL;
                }
            }
        }
    }

    return retcode;
}
/**
 *  @brief      The function allocates CMA buffer from the Linux kernel. The CMA buffer  is continuous in physical memory and aligned to 4K byte.
 *
 *  @param      req_size : [IN] The size of the request.
 *  @param      is_cached : [IN] The cache type of the request.
 *  @param      req_buf : [OUT] The buffer of the request.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemAlloc(uint32_t req_size, uint32_t is_cached, flexidag_memblk_t *req_buf)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t size_align;
    ULONG addr;

    if(req_buf == NULL) {
        retcode = ERR_INTF_CMA_MEMBLK_INVALID;
    } else {
        retcode = CV_UtilityCmaMemInit();
        if(retcode == ERRCODE_NONE) {
            if(mutex_lock(&cma_mutex) != ERRCODE_NONE) {
                retcode = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
            } else {
                size_align = (uint32_t)((req_size + FD_MEM_ALIGN_SIZE - 1U)  & (~(FD_MEM_ALIGN_SIZE - 1U)));
                if ((size_align + cma_curr) > cma_blk.buffer_size) {
                    retcode = ERR_DRV_SCHDR_CMA_ALLOC_TOTAL_SIZE_OUT_OF_RANGE;
                } else {
                    if(is_cached != cma_blk.buffer_cacheable) {
                        AmbaPrint_PrintUInt5("[WARNING] AmbaCV_UtilityCmaMemAlloc() : alloc cache type %d != cma support cache type", is_cached, cma_blk.buffer_cacheable, 0U, 0U, 0U);
                    }
                    req_buf->buffer_cacheable = cma_blk.buffer_cacheable;
                    req_buf->buffer_daddr = cma_blk.buffer_daddr+cma_curr;
                    req_buf->pBuffer = &cma_blk.pBuffer[cma_curr];
                    req_buf->buffer_size = size_align;
                    req_buf->buffer_caddr = (ulong)ambacv_p2c(req_buf->buffer_daddr);
                    if(req_buf->buffer_caddr == 0U) {
                        retcode = ERR_DRV_SCHDR_P2C_FAIL;
                    } else {
                        cma_curr += size_align;
                        AmbaMisra_TypeCast(&addr, &req_buf->pBuffer);
                        AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_UtilityCmaMemAlloc() : PA 0x%x VA 0x%x Size 0x%x ", (uint32_t)req_buf->buffer_daddr, (uint32_t)addr, (uint32_t)req_buf->buffer_size, 0U, 0U);
                    }
                }
                if( mutex_unlock(&cma_mutex) != ERRCODE_NONE) {
                    if(retcode == ERRCODE_NONE) {
                        retcode = ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL;
                    }
                }
            }
        }

    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityCmaMemAlloc() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      The function remap CMA buffer which is allocated by another process.
 *
 *  @param      req_addr : [IN] The physical addr of the request.
 *  @param      req_size : [IN] The size of the request.
 *  @param      is_cached : [IN] The cache type of the request.
 *  @param      req_buf : [OUT] The buffer of the request.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemRemap(ulong req_addr, uint32_t req_size, uint32_t is_cached, flexidag_memblk_t *req_buf)
{

    uint32_t retcode = ERRCODE_NONE;

    (void) req_addr;
    (void) req_size;
    (void) is_cached;
    (void) req_buf;
    AmbaPrint_PrintUInt5("[WARNING] AmbaCV_UtilityCmaMemRemap() : not support", 0U, 0U, 0U, 0U, 0U);

    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      This function unmap CMA buffer.
 *
 *  @param      buf : [IN] The buffer to unmap.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Invalid argument (null pointer).   \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemUnmap(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) buf;
    AmbaPrint_PrintUInt5("[WARNING] AmbaCV_UtilityCmaMemUnmap() : not support", 0U, 0U, 0U, 0U, 0U);

    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      This function frees CMA buffer.
 *
 *  @param      buf : [IN] The buffer to free.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Invalid argument (null pointer).   \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemFree(flexidag_memblk_t *buf)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) buf;
    AmbaPrint_PrintUInt5("[WARNING] AmbaCV_UtilityCmaMemFree() : not support", 0U, 0U, 0U, 0U, 0U);

    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Arm cache used to clean the CMA buffer.
 *
 *  @param      buf : [IN] The buffer used to run cache clean operation.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemClean(flexidag_memblk_t *buf)
{
    uint32_t retcode;

    retcode = ambacv_cache_clean(buf->pBuffer, buf->buffer_size);

    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Arm cache invalidates the CMA buffer.
 *
 *  @param      buf : [IN] The buffer used to run cache invalidates operation.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemInvalid(flexidag_memblk_t *buf)
{
    uint32_t retcode;

    retcode = ambacv_cache_invalidate(buf->pBuffer, buf->buffer_size);

    return CV_GetExternalErrorCode(NULL, retcode);
}
#endif
#endif
#endif

static uint32_t CV_UtilityFileLoad(const char *path, const flexidag_memblk_t *flexibin_buf, uint32_t *size_align)
{
    uint32_t retcode = ERRCODE_NONE;
    FILE_WRAP *ifp;
    uint64_t size;
    uint32_t read_size = 0U;

    ifp = fopen_wrap(path, "rb");
    if (ifp == NULL) {
        AmbaPrint_PrintStr5("CV_UtilityFileLoad : fopen_wrap %s fail !", path, NULL, NULL, NULL, NULL);
        retcode = ERR_INTF_FLEXIDAG_FILE_OPEN_FAIL;
    } else {
        if(fseek_wrap(ifp, 0, SEEK_END_WRAP) != 0U) {
            AmbaPrint_PrintUInt5("CV_UtilityFileLoad : fseek_wrap end fail ", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_INTF_FLEXIDAG_FILE_SEEK_FAIL;
        } else {
            size = ftell_wrap(ifp);
            *size_align = (uint32_t)((size + FD_MEM_ALIGN_SIZE - 1U) & (~(FD_MEM_ALIGN_SIZE - 1U)));
            read_size = (uint32_t)(size & 0xFFFFFFFFU);
            if(fseek_wrap(ifp, 0, SEEK_SET_WRAP) != 0U) {
                AmbaPrint_PrintUInt5("CV_UtilityFileLoad : fseek_wrap set fail ", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_INTF_FLEXIDAG_FILE_SEEK_FAIL;
            }
        }

        if((retcode == ERRCODE_NONE) && (flexibin_buf != NULL)) {
            if(read_size != fread_wrap(flexibin_buf->pBuffer, 1, read_size, ifp)) {
                AmbaPrint_PrintUInt5("CV_UtilityFileLoad : fread_wrap fail ", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_INTF_FLEXIDAG_FILE_READ_FAIL;
            } else {
                if(flexibin_buf->buffer_cacheable == 1U) {
                    flexidag_memblk_t flexibin_tmp;

                    flexibin_tmp.pBuffer = flexibin_buf->pBuffer;
                    flexibin_tmp.buffer_daddr = flexibin_buf->buffer_daddr;
                    flexibin_tmp.buffer_cacheable = flexibin_buf->buffer_cacheable;
                    flexibin_tmp.buffer_size = flexibin_buf->buffer_size;
                    flexibin_tmp.buffer_caddr = flexibin_buf->buffer_caddr;
                    if (AmbaCV_UtilityCmaMemClean(&flexibin_tmp) != 0U) {
                        AmbaPrint_PrintUInt5("CV_UtilityFileLoad : AmbaCV_UtilityCmaMemClean fail ", 0U, 0U, 0U, 0U, 0U);
                        retcode = ERR_INTF_FLEXIDAG_CACHE_CLEAN_FAIL;
                    }
                }
            }
        }

        if (fclose_wrap(ifp) != 0U) {
            AmbaPrint_PrintStr5("CV_UtilityFileLoad : fclose_wrap %s fail !", path, NULL, NULL, NULL, NULL);
        }
    }

    return retcode;

}

static uint32_t CV_ParamSetLogMsg(const AMBA_CV_FLEXIDAG_HANDLE_s *handle, const AMBA_CV_FLEXIDAG_LOG_MSG_s *set)
{
    uint32_t retcode = ERRCODE_NONE;

    retcode = flexidag_set_parameter(handle->fd_handle, FDPARAM_INTERFACE_MSGNUM_ID, set->flexidag_msg_entry);
    retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_INTERNAL_MSGNUM_ID, set->cvtask_msg_entry);

    if( (set->arm_cpu_map & 0x1U) != 0U ) {
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARMPERF0_ID, 0U);
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARMSCH0_ID, 0U);
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARM0_ID, set->arm_log_entry);
    } else {
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARMPERF0_ID, 0U);
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARMSCH0_ID, 0U);
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARM0_ID, 0U);
    }

    if( (set->arm_cpu_map & 0x2U) != 0U ) {
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARMPERF1_ID, 0U);
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARMSCH1_ID, 0U);
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARM1_ID, set->arm_log_entry);
    } else {
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARMPERF1_ID, 0U);
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARMSCH1_ID, 0U);
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARM1_ID, 0U);
    }

    if( (set->arm_cpu_map & 0x4U) != 0U ) {
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARMPERF2_ID, 0U);
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARMSCH2_ID, 0U);
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARM2_ID, set->arm_log_entry);
    } else {
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARMPERF2_ID, 0U);
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARMSCH2_ID, 0U);
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARM2_ID, 0U);
    }

    if( (set->arm_cpu_map & 0x8U) != 0U ) {
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARMPERF3_ID, 0U);
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARMSCH3_ID, 0U);
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARM3_ID, set->arm_log_entry);
    } else {
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARMPERF3_ID, 0U);
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARMSCH3_ID, 0U);
        retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_ARM3_ID, 0U);
    }

#if defined (CHIP_CV2) || defined (CHIP_CV22) || defined (CHIP_CV25) || defined (CHIP_CV28) || defined (CHIP_CV2A) || defined (CHIP_CV22A) || defined (CHIP_CV2FS) || defined (CHIP_CV22FS) || defined (CHIP_CV5) || defined (CHIP_CV52) || defined (CHIP_CV6)
    retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_VISPERF0_ID, 0U);
    retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_VISSCH0_ID, 0U);
    retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_VISORC0_ID, set->orc_log_entry);

    retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_VISPERF1_ID, 0U);
    retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_VISSCH1_ID, 0U);
    retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_VISORC1_ID, 0U);

    retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_VISPERF2_ID, 0U);
    retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_VISSCH2_ID, 0U);
    retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_VISORC2_ID, 0U);

    retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_VISPERF3_ID, 0U);
    retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_VISSCH3_ID, 0U);
    retcode |= flexidag_set_parameter(handle->fd_handle, FDPARAM_LOG_ENTRIES_VISORC3_ID, 0U);
#else
#error "unsupported CHIP ID, please check your PROJECT setting"
#endif

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("CV_ParamSetLogMsg : flexidag_set_parameter fail ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
}

static uint32_t CV_FlexidagTimestamp(void)
{
    uint32_t val;
    FLOAT fltmp;

    fltmp = (FLOAT) cvtask_get_timestamp();
    fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
    val = (uint32_t)fltmp;

    return val;
}

static uint32_t CV_SchedCheckDrvState(uint32_t state)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t real_state;

    retcode = schdr_get_state(&real_state);
    if(retcode == ERRCODE_NONE) {
        if(state != real_state) {
            if(real_state == FLEXIDAG_SCHDR_DRV_ON) {
                retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
            } else if(real_state == FLEXIDAG_SCHDR_DRV_OFF) {
                retcode = ERR_DRV_SCHDR_NOT_ACTIVATE;
            } else {
                retcode = ERR_DRV_SCHDR_IN_SAFETY_STATE;
            }
        }
    }

    return retcode;
}

static uint32_t CV_SchedCheckState(uint32_t state)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t real_state;

    retcode = schdr_get_ambacv_state(&real_state);
    if(retcode == ERRCODE_NONE) {
        if(state != real_state) {
            if(real_state == FLEXIDAG_SCHDR_ON) {
                retcode = ERR_DRV_SCHDR_VISORC_ACTIVATE;
            } else {
                retcode = ERR_DRV_SCHDR_VISORC_NOT_ACTIVATE;
            }
        }
    }

    return retcode;
}

static uint32_t CV_FlexidagCheckState(void *vpHandle, uint32_t state)
{
    uint32_t retcode = ERR_DRV_FLEXIDAG_SLOT_STATE_MISMATCHED;
    uint32_t slot_id;

    if(CV_SchedCheckState(FLEXIDAG_SCHDR_ON) != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("CV_FlexidagCheckState() : scheduler not active ", 0U, 0U, 0U, 0U, 0U);
    } else {
        if(state == FLEXIDAG_STATE_UNUSED) {
            if(vpHandle == NULL) {
                retcode = ERRCODE_NONE;
            } else {
                slot_id = flexidag_find_slot_by_vphandle(vpHandle);
                if((slot_id < FLEXIDAG_MAX_SLOTS) && (inter_state[slot_id] == state)) {
                    retcode = ERRCODE_NONE;
                }
            }
        } else {
            if(vpHandle != NULL) {
                slot_id = flexidag_find_slot_by_vphandle(vpHandle);
                if((slot_id < FLEXIDAG_MAX_SLOTS) && (inter_state[slot_id] == state)) {
                    retcode = ERRCODE_NONE;
                }
            }
        }
    }

    return retcode;
}

static uint32_t CV_FlexidagSetState(void *vpHandle, uint32_t state)
{
    uint32_t retcode = ERR_DRV_FLEXIDAG_SLOT_STATE_MISMATCHED;
    uint32_t slot_id;

    if(vpHandle != NULL) {
        slot_id = flexidag_find_slot_by_vphandle(vpHandle);
        if(slot_id < FLEXIDAG_MAX_SLOTS) {
            if((state == FLEXIDAG_STATE_UNUSED) && (inter_state[slot_id] == FLEXIDAG_STATE_READY)) {
                inter_state[slot_id] = state;
                retcode = ERRCODE_NONE;
            } else if((state == FLEXIDAG_STATE_INIT) && (inter_state[slot_id] == FLEXIDAG_STATE_UNUSED)) {
                inter_state[slot_id] = state;
                retcode = ERRCODE_NONE;
            } else if((state == FLEXIDAG_STATE_READY) && (inter_state[slot_id] == FLEXIDAG_STATE_INIT)) {
                inter_state[slot_id] = state;
                retcode = ERRCODE_NONE;
            } else {
                retcode = ERR_DRV_FLEXIDAG_SLOT_STATE_MISMATCHED;
            }
        }
    }

    return retcode;
}

/**
 *  @brief      Get alignment size of file.
 *
 *  @param      path : [IN] The path of file.
 *  @param      size_align : [OUT] The alignment size of file.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityFileSize(const char *path, uint32_t *size_align)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t start_time, end_time;

    if ((path == NULL) || (size_align == NULL)) {
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else {
        start_time = CV_FlexidagTimestamp();
        retcode = CV_UtilityFileLoad(path, NULL, size_align);
        end_time = CV_FlexidagTimestamp();
        AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_UtilityFileSize : run_time = %d us", end_time - start_time, 0, 0, 0, 0);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityFileSize() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = CV_ERR_0000;
    }
    return retcode;
}

/**
 *  @brief      This function loads the file to the buffer.
 *
 *  @param      path : [IN] The path of file.
 *  @param      flexibin_buf : [IN] The buffer which the file loads to.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityFileLoad(const char *path, const flexidag_memblk_t *flexibin_buf)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t size_align = 0U;
    uint32_t start_time, end_time;

    if (path == NULL) {
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else if ( (flexibin_buf == NULL) || (flexibin_buf->pBuffer == NULL)) {
        retcode = ERR_INTF_FLEXIDAG_BIN_MEMBLK_INVALID;
    } else if (((flexibin_buf->buffer_daddr & (FD_MEM_ALIGN_SIZE - 1U)) != 0U) || ((flexibin_buf->buffer_size & (FD_MEM_ALIGN_SIZE - 1U)) != 0U)) {
        retcode = ERR_INTF_FLEXIDAG_BIN_ALIGN_INVALID;
    } else {
        start_time = CV_FlexidagTimestamp();
        retcode = CV_UtilityFileLoad(path, flexibin_buf, &size_align);
        end_time = CV_FlexidagTimestamp();
        AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_UtilityFileLoad : run_time = %d us, size_align = 0x%x", end_time - start_time, size_align, 0, 0, 0);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_UtilityFileLoad() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = CV_ERR_0000;
    }
    return retcode;
}

/**
 *  @brief      Get scheduler drv state.\n
 *              APP get scheduler driver state.  \n
 *              If state = FLEXIDAG_SCHDR_DRV_OFF, App should call AmbaCV_StandAloneSchdrStart or AmbaCV_FlexidagSchdrStart\n
 *              If state = FLEXIDAG_SCHDR_DRV_ON, App can start to call flexidag API \n
 *              If state = FLEXIDAG_SCHDR_DRV_SAFE, App would not allow to call flexidag API \n
 *
 *  @param      status : [IN] Scheduler state   \n
 *                              FLEXIDAG_SCHDR_DRV_OFF : 0x0    \n
 *                              FLEXIDAG_SCHDR_DRV_ON :  0x1
 *                              FLEXIDAG_SCHDR_DRV_SAFE :  0x2
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrDrvState(uint32_t *status)
{
    uint32_t retcode = ERRCODE_NONE;

    if (status == NULL) {
        AmbaPrint_PrintUInt5("AmbaCV_SchdrDrvState : status == NULL", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else {
        retcode = schdr_get_state(status);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_SchdrDrvState() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Get ORC scheduler state. For a process to initialize the scheduler,   \n
 *              APP calls AmbaCV_StandAloneSchdrStart to initialize scheduler if state is FLEXIDAG_SCHDR_OFF   \n
 *              and calls AmbaCV_FlexidagSchdrStart if the state is FLEXIDAG_SCHDR_ON.
 *
 *  @param      status : [IN] Scheduler state   \n
 *                              FLEXIDAG_SCHDR_OFF : 0x0    \n
 *                              FLEXIDAG_SCHDR_ON  : 0x1
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrState(uint32_t *status)
{
    uint32_t retcode = ERRCODE_NONE;

    if (status == NULL) {
        AmbaPrint_PrintUInt5("AmbaCV_SchdrState : status == NULL", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else {
        retcode = schdr_get_ambacv_state(status);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_SchdrState() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Loads visorc.bin to bring up scheduler.
 *
 *  @param      path : [IN] Directory of visorc.bin
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrLoad(const char *path)
{
    uint32_t retcode = ERRCODE_NONE;

    if (path == NULL) {
        AmbaPrint_PrintUInt5("AmbaCV_SchdrLoad : path == NULL", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else if ( CV_SchedCheckState(FLEXIDAG_SCHDR_OFF) != ERRCODE_NONE ) {
        retcode = ERR_INTF_SCHDR_VISORC_STATE_MISMATCHED;
    } else if ( CV_SchedCheckDrvState(FLEXIDAG_SCHDR_DRV_OFF) != ERRCODE_NONE ) {
        retcode = ERR_INTF_SCHDR_STATE_MISMATCHED;
    } else {
#if !defined(ENV_IS_PACE_LINUX)
        retcode = (uint32_t)schdr_load_binary(path);
#else
        int argc = 0U;
        char *argv[ARGV_NUM];

        retcode = CV_StrToArg(path, &argc, argv);
        if (retcode == ERRCODE_NONE) {
            retcode = (uint32_t)schdrcfg_parse_amalgam_opts(argc, argv);
        }
#endif
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_SchdrLoad() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Loads visorc.bin by user callback to bring up scheduler.
 *
 *  @param      file_name : [IN] File of visorc.bin
 *
 *  @param      bin_type : [IN] vp or sod bin
 *
 *  @param      load_cb : [IN] load call back function
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrLoadExt(const char *file_name, uint32_t bin_type, flexidag_schdr_load_cb load_cb)
{
    uint32_t retcode = ERRCODE_NONE;

    if (file_name == NULL) {
        AmbaPrint_PrintUInt5("AmbaCV_SchdrLoadExt : file_name == NULL", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else if ((bin_type != FLEXIDAG_SCHDR_VP_BIN) && (bin_type != FLEXIDAG_SCHDR_SOD_BIN)) {
        AmbaPrint_PrintUInt5("AmbaCV_SchdrLoadExt : invalid bin_type %d!", bin_type, 0U, 0U, 0U, 0U);
        retcode = ERR_INTF_SCHDR_VISCORE_UNAVAILABLE;
    } else if ( CV_SchedCheckState(FLEXIDAG_SCHDR_OFF) != ERRCODE_NONE ) {
        retcode = ERR_INTF_SCHDR_VISORC_STATE_MISMATCHED;
    } else if ( CV_SchedCheckDrvState(FLEXIDAG_SCHDR_DRV_OFF) != ERRCODE_NONE ) {
        retcode = ERR_INTF_SCHDR_STATE_MISMATCHED;
    } else {
#if !defined(ENV_IS_PACE_LINUX)
        retcode = (uint32_t)schdr_load_binary_ext(file_name, bin_type, load_cb);
#else
        int argc = 0U;
        char *argv[ARGV_NUM];

        (void) load_cb;
        retcode = CV_StrToArg(file_name, &argc, argv);
        if (retcode == ERRCODE_NONE) {
            retcode = (uint32_t)schdrcfg_parse_amalgam_opts(argc, argv);
        }
#endif
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_SchdrLoadExt() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Loads cvtable.bin by user callback to bring up scheduler.
 *
 *  @param      file_name : [IN] File of cvtable.bin
 *
 *  @param      fill_cb : [IN] fill call back function
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrAddCvtable(const char *file_name, flexidag_schdr_fill_cb fill_cb)
{
    uint32_t retcode = ERRCODE_NONE;
    void *token = NULL;

    if (file_name == NULL) {
        AmbaPrint_PrintUInt5("AmbaCV_SchdrAddCvtable : file_name == NULL", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else if (fill_cb == NULL) {
        AmbaPrint_PrintUInt5("AmbaCV_SchdrAddCvtable : fill_cb == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else if ( CV_SchedCheckState(FLEXIDAG_SCHDR_OFF) != ERRCODE_NONE ) {
        retcode = ERR_INTF_SCHDR_VISORC_STATE_MISMATCHED;
    } else if ( CV_SchedCheckDrvState(FLEXIDAG_SCHDR_DRV_OFF) != ERRCODE_NONE ) {
        retcode = ERR_INTF_SCHDR_STATE_MISMATCHED;
    } else {
        AmbaMisra_TypeCast(&token, &file_name);
        retcode = (uint32_t)schdr_add_cvtable(token, fill_cb);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_SchdrAddCvtable() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Check visorc from memory.
 *
 *  @param      bin_type : [IN] vp or sod bin
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect efrror category) \n
 */
uint32_t AmbaCV_SchdrCheckVisorc(uint32_t bin_type)
{
    uint32_t retcode = ERRCODE_NONE;

    if ((bin_type != FLEXIDAG_SCHDR_VP_BIN) && (bin_type != FLEXIDAG_SCHDR_SOD_BIN)) {
        AmbaPrint_PrintUInt5("AmbaCV_SchdrCheckVisorc : invalid bin_type %d!", bin_type, 0U, 0U, 0U, 0U);
        retcode = ERR_INTF_SCHDR_VISCORE_UNAVAILABLE;
    } else {
#if !defined(ENV_IS_PACE_LINUX)
        retcode = schdr_check_visorc(bin_type);
#else
        AmbaPrint_PrintUInt5("[WARNING] AmbaCV_SchdrCheckVisorc() : not support", 0U, 0U, 0U, 0U, 0U);
#endif
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_SchdrCheckVisorc() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Get HW info.
 *
 *  @param      id : [IN] The ID of the HW info.  \n
 *                                   VP_OUTPUT_ALIGNMENT      0U
 *                                   FLEXIDAG_MEM_ALIGNMENT   1U
 *  @param      info : [IN] The pointer of the param_set structure.
 *  @param      info_size : [IN] The size of the param_set structure.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrGetHwInfo(uint32_t id, void *info, uint32_t info_size)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) info;
    if( info == NULL ) {
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else {
        if(id == VP_OUTPUT_ALIGNMENT) {
            uint32_t *pValue;

            if(info_size != sizeof(uint32_t)) {
                retcode = ERR_INTF_SCHDR_HWINFO_SIZE_MISMATCHED;
            } else {
                AmbaMisra_TypeCast(&pValue, &info);
                *pValue = VP_OUTPUT_ALIGN_SIZE;
            }
        } else if (id == FLEXIDAG_MEM_ALIGNMENT) {
            uint32_t *pValue;

            if(info_size != sizeof(uint32_t)) {
                retcode = ERR_INTF_SCHDR_HWINFO_SIZE_MISMATCHED;
            } else {
                AmbaMisra_TypeCast(&pValue, &info);
                *pValue = FD_MEM_ALIGN_SIZE;
            }
        } else {
            retcode = ERR_INTF_HWINFO_ID_UNKNOW;
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_SchdrGetHwInfo() : error id 0x%x , ret = 0x%x", id, retcode, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

static uint32_t CV_SchdrStandAloneSetLogCfg(const AMBA_CV_STANDALONE_SCHDR_CFG_s *cfg)
{
    uint32_t retcode = ERRCODE_NONE;

    retcode = (uint32_t)schdrcfg_set_cpu_map(cfg->cpu_map);
    if((cfg->log_entries_visschdr & LOG_ENTRIES_ENABLE_BIT) == LOG_ENTRIES_ENABLE_CFG) {
        retcode = (uint32_t)schdrcfg_set_log_entries_visschdr(cfg->log_entries_visschdr & ~((uint32_t)LOG_ENTRIES_ENABLE_BIT));
    }

    if(retcode == ERRCODE_NONE) {
        if((cfg->log_entries_viscvtask & LOG_ENTRIES_ENABLE_BIT) == LOG_ENTRIES_ENABLE_CFG) {
            retcode = (uint32_t)schdrcfg_set_log_entries_viscvtask(cfg->log_entries_viscvtask & ~((uint32_t)LOG_ENTRIES_ENABLE_BIT));
        }
    }

    if(retcode == ERRCODE_NONE) {
        if((cfg->log_entries_visperf & LOG_ENTRIES_ENABLE_BIT) == LOG_ENTRIES_ENABLE_CFG) {
            retcode = (uint32_t)schdrcfg_set_log_entries_visperf(cfg->log_entries_visperf & ~((uint32_t)LOG_ENTRIES_ENABLE_BIT));
        }
    }

    return retcode;
}

static uint32_t CV_SchdrStandAloneSetCfg(const AMBA_CV_STANDALONE_SCHDR_CFG_s *cfg)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t boot_cfg;

    if(cfg != NULL) {
        retcode = (uint32_t)schdrcfg_set_cpu_map(cfg->cpu_map);
        if(retcode == ERRCODE_NONE) {
            retcode = (uint32_t)schdrcfg_set_verbosity(cfg->log_level);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = (uint32_t)schdrcfg_set_flexidag_num_slots(cfg->flexidag_slot_num);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = (uint32_t)schdrcfg_set_cavalry_num_slots(cfg->cavalry_slot_num);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = (uint32_t)schdrcfg_set_flags(cfg->flag);
        }

        if((retcode == ERRCODE_NONE) && (cfg->boot_cfg >= FLEXIDAG_ENABLE_BOOT_CFG) && (cfg->boot_cfg <= (FLEXIDAG_ENABLE_BOOT_CFG + 4))) {
            boot_cfg = cfg->boot_cfg - FLEXIDAG_ENABLE_BOOT_CFG;
            retcode = (uint32_t)schdrcfg_set_boot_cfg(boot_cfg);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = (uint32_t)schdrcfg_set_autorun_interval(cfg->autorun_interval);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = (uint32_t)schdrcfg_set_autorun_fex_cfg(cfg->autorun_fex_cfg);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = (uint32_t)CV_SchdrStandAloneSetLogCfg(cfg);
        }
    }
    return retcode;
}

/**
 *  @brief      Initialize scheduler for FlexiDAG which is used for the first process.    \n
 *              In RTOS, this is the API that is used to start a FlexiDAG scheduler since the whole system is a single process.
 *
 *  @param      cfg : [IN] Config of scheduler initializes
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_StandAloneSchdrStart(const AMBA_CV_STANDALONE_SCHDR_CFG_s *cfg)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t start_time, end_time,i;

    if ( CV_SchedCheckState(FLEXIDAG_SCHDR_OFF) != ERRCODE_NONE ) {
        retcode = ERR_INTF_SCHDR_VISORC_STATE_MISMATCHED;
    } else if ( CV_SchedCheckDrvState(FLEXIDAG_SCHDR_DRV_OFF) != ERRCODE_NONE ) {
        retcode = ERR_INTF_SCHDR_STATE_MISMATCHED;
    } else {
        start_time = CV_FlexidagTimestamp();

        retcode = CV_SchdrStandAloneSetCfg(cfg);

        if(retcode == ERRCODE_NONE) {
            retcode = (uint32_t)schdr_standalone_start();
        }

        if(retcode == ERRCODE_NONE) {
            retcode = CV_SchdrGetCvRegion();
        }

#if !defined(ENV_IS_PACE_LINUX)
        if (retcode == ERRCODE_NONE) {
            (void)cvschdr_region_size;
            retcode = errmgr_ipc_send_heartbeat_config(cvschdr_region_base);
        }
#endif

        if((retcode == ERRCODE_NONE) && (global_init == 0U)) {
            if(mutex_init(&global_mutex) != 0U) {
                AmbaPrint_PrintUInt5("AmbaCV_StandAloneSchdrStart() : mutex_init fail", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_MUTEX_INIT_FAIL;
            } else {
                global_init = 1;
            }
        }
        for(i = 0U; i<FLEXIDAG_MAX_SLOTS; i++) {
            inter_state[i] = 0U;
            AmbaCV_Handle[i] = NULL;
        }
        end_time = CV_FlexidagTimestamp();
        AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_StandAloneSchdrStart : run_time = %d us", end_time - start_time, 0, 0, 0, 0);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_StandAloneSchdrStart() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Initialize the scheduler for the FlexiDAG which used for other process.   \n
 *              In RTOS, this API is not used since the whole system is a single process.
 *
 *  @param      cfg : [IN] Config of scheduler initializes
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSchdrStart(const AMBA_CV_FLEXIDAG_SCHDR_CFG_s *cfg)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t start_time, end_time;

    if ( CV_SchedCheckState(FLEXIDAG_SCHDR_ON) != ERRCODE_NONE ) {
        retcode = ERR_INTF_SCHDR_VISORC_STATE_MISMATCHED;
    } else if ( CV_SchedCheckDrvState(FLEXIDAG_SCHDR_DRV_OFF) != ERRCODE_NONE ) {
        retcode = ERR_INTF_SCHDR_STATE_MISMATCHED;
    } else {
        if(cfg == NULL) {
            retcode = ERR_INTF_INPUT_PARAM_INVALID;
        } else {
            start_time = CV_FlexidagTimestamp();
            retcode = (uint32_t)schdrcfg_set_cpu_map(cfg->cpu_map);
            if(retcode == ERRCODE_NONE) {
                retcode = (uint32_t)schdrcfg_set_verbosity(cfg->log_level);
            }

            if(retcode == ERRCODE_NONE) {
                retcode = (uint32_t)flexidag_schdr_start();
            }

            if(retcode == ERRCODE_NONE) {
                retcode = CV_SchdrGetCvRegion();
            }

            if((retcode == ERRCODE_NONE) && (global_init == 0U)) {
                if(mutex_init(&global_mutex) != 0U) {
                    AmbaPrint_PrintUInt5("AmbaCV_FlexidagSchdrStart() : mutex_init fail", 0U, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_MUTEX_INIT_FAIL;
                } else {
                    global_init = 1;
                }
            }
            end_time = CV_FlexidagTimestamp();
            AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_FlexidagSchdrStart : run_time = %d us", end_time - start_time, 0, 0, 0, 0);
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagSchdrStart() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Stop arm scheduler driver only (VISORC still active) and scheduler driver state change to inactive.
 *
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSchdrStop(void)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t start_time, end_time;

    if (CV_SchedCheckDrvState(FLEXIDAG_SCHDR_DRV_OFF) == ERRCODE_NONE) {
        retcode = ERR_INTF_SCHDR_STATE_MISMATCHED;
    } else {
        start_time = CV_FlexidagTimestamp();
        retcode = (uint32_t) schdr_shutdown((int32_t)SCHDR_ARM_RESET);
        end_time = CV_FlexidagTimestamp();
        AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_FlexidagSchdrStop : run_time = %d us", end_time - start_time, 0, 0, 0, 0);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagSchdrStop() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Shutdown scheduler and shutdown flush log generator.
 *
 *  @param      Reserved : [IN] Not used for now
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrShutdown(uint32_t Reserved)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t start_time, end_time;

    if (CV_SchedCheckState(FLEXIDAG_SCHDR_OFF) == ERRCODE_NONE) {
        retcode = ERR_INTF_SCHDR_VISORC_STATE_MISMATCHED;
    } else if (CV_SchedCheckDrvState(FLEXIDAG_SCHDR_DRV_OFF) == ERRCODE_NONE) {
        retcode = ERR_INTF_SCHDR_STATE_MISMATCHED;
    } else {
        start_time = CV_FlexidagTimestamp();
        retcode = (uint32_t) schdr_shutdown((int32_t)Reserved);
        end_time = CV_FlexidagTimestamp();
        AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_SchdrShutdown : run_time = %d us", end_time - start_time, 0, 0, 0, 0);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_SchdrShutdown() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Wait and process messages from VISORC.
 *              The function will handle cache coherence for all incoming messages and process some messages in Kernel layer due to
 *              the multiple process support of FlexiDAG operation in multiple OSes.
 *              The wait will timeout if there is no message coming and user need to call this API again to wait again.
 *
 *  @param      msg_num : [OUT] num of messages ready to process
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrWaitMsg(uint32_t *msg_num)
{
    uint32_t retcode = ERRCODE_NONE;

#if !defined(ENV_IS_PACE_LINUX)
    if(msg_num == NULL) {
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else {
        retcode = schdr_wait_msg(msg_num);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_SchdrWaitMsg() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
#else
    (void)msg_num;
    AmbaPrint_PrintUInt5("[WARNING] AmbaCV_SchdrWaitMsg() : not support", 0U, 0U, 0U, 0U, 0U);
#endif
    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Process message messages from VISORC.
 *              If no message can process, API return fail.
 *              If this API return success, user need to check if message_retcode success or fail.
 *              If message_retcode is failed, user can get failed message ID and corresponding FlexiDAG handle if
 *              the message belongs to a FlexiDAG.
 *
 *  @param      pphandle : [OUT] if *pphandle is NULL, it is scheduler message, not for a particular FlexiDAG.
 *  @param      message_type : [OUT] type of message
 *  @param      message_retcode : [OUT] return error code
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrProcessMsg(AMBA_CV_FLEXIDAG_HANDLE_s **pphandle, uint32_t *message_type, uint32_t *message_retcode)
{
    uint32_t ext_retcode = ERRCODE_NONE;
#if !defined(ENV_IS_PACE_LINUX)
    uint32_t retcode = ERRCODE_NONE;
    uint32_t msg_retcode;
    uint32_t slot_id;
    void *vpHandle = NULL;

    if(pphandle == NULL) {
        ext_retcode = CV_ERR_0000;
    } else {
        if((message_type == NULL) || (message_retcode == NULL)) {
            retcode = ERR_INTF_INPUT_PARAM_INVALID;
        } else {
            retcode = schdr_process_msg(&vpHandle, message_type, message_retcode);
            if (retcode == ERRCODE_NONE) {
                if(vpHandle != NULL) {
                    slot_id = flexidag_find_slot_by_vphandle(vpHandle);
                    if(slot_id < FLEXIDAG_MAX_SLOTS) {
                        *pphandle = AmbaCV_Handle[slot_id];
                    } else {
                        retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
                        *pphandle = NULL;
                    }
                } else {
                    *pphandle= NULL;
                }
            }
        }

        if( retcode != ERRCODE_NONE ) {
            AmbaPrint_PrintUInt5("[ERROR] AmbaCV_SchdrProcessMsg() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        } else {
            msg_retcode = CV_GetExternalErrorCode(*pphandle, *message_retcode);
            if(msg_retcode != ERRCODE_NONE) {
                AmbaPrint_PrintUInt5("[ERROR] AmbaCV_SchdrProcessMsg() : message error ret = 0x%x", *message_retcode, 0U, 0U, 0U, 0U);
            }
        }
        ext_retcode = CV_GetExternalErrorCode(*pphandle, retcode);
    }
#else
    (void)pphandle;
    (void)message_type;
    (void)message_retcode;
    AmbaPrint_PrintUInt5("[WARNING] AmbaCV_SchdrProcessMsg() : not support", 0U, 0U, 0U, 0U, 0U);
#endif

    return ext_retcode;
}

/*  This is define in ambacv_ioctl.h    */
#define SOD_SCHDR_LOG_INDEX  0U
#define VP_SCHDR_LOG_INDEX   2U

/**
 *  @brief      Dump scheduler messages from VISORC.
 *
 *  @param      type : [IN] FLEXIDAG_SCHDR_VP_BIN or FLEXIDAG_SCHDR_SOD_BIN
 *  @param      log_file : [IN] file name to dump out.If not, set NULL
 *  @param      num_lines : [IN] num line of message to dump
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrDumpLog(uint32_t type, const char *log_file, uint32_t num_lines)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t mode = 0U;

    if(type == FLEXIDAG_SCHDR_VP_BIN) {
        mode = ((uint32_t)1U << VP_SCHDR_LOG_INDEX);
    } else if(type == FLEXIDAG_SCHDR_SOD_BIN) {
        mode = ((uint32_t)1U << SOD_SCHDR_LOG_INDEX);
    } else {
        retcode = ERR_INTF_SCHDR_VISCORE_UNAVAILABLE;
    }

#if !defined(ENV_IS_PACE_LINUX)
    if((uint32_t)schdr_flush_visorc_printf(1U) != ERRCODE_NONE) {
        AmbaPrint_PrintUInt5("[[WARNING]] AmbaCV_SchdrDumpLog() : schdr_flush_visorc_printf fail ", 0U, 0U, 0U, 0U, 0U);
    }

    if(retcode == ERRCODE_NONE) {
        retcode = (uint32_t)schdr_visorc_dmsg(mode, log_file, (int32_t)num_lines);
    }
#else
    (void)mode;
    (void)log_file;
    (void)num_lines;
    AmbaPrint_PrintUInt5("[[WARNING]] AmbaCV_SchdrDumpLog() : not support", 0U, 0U, 0U, 0U, 0U);
#endif

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_SchdrDumpLog() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(NULL, retcode);
}

static uint32_t CV_FlexidagOpenCheckParam(const flexidag_memblk_t *flexibin_buf, const AMBA_CV_FLEXIDAG_HANDLE_s *handle)
{
    uint32_t retcode = ERRCODE_NONE;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(CV_FlexidagCheckState(handle->fd_handle, FLEXIDAG_STATE_UNUSED) != ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
    } else if( (flexibin_buf == NULL) || (flexibin_buf->pBuffer == NULL)) {
        retcode = ERR_INTF_FLEXIDAG_BIN_MEMBLK_INVALID;
    } else if (((flexibin_buf->buffer_daddr & (FD_MEM_ALIGN_SIZE - 1U)) != 0U) || ((flexibin_buf->buffer_size & (FD_MEM_ALIGN_SIZE - 1U)) != 0U)) {
        retcode = ERR_INTF_FLEXIDAG_BIN_ALIGN_INVALID;
    } else if (CV_FlexidagCheckCvRegion(flexibin_buf) != ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_BIN_OUT_OF_RANGE;
    } else {
        //misra c
    }

    return retcode;
}

/**
 *  @brief      Set FlexiDAG using the input tbar and return the handle.
 *
 *  @param      flexidag_tbar : [IN] The FlexiDAG buffer copies from files containing CVTasks Tbar.
 *  @param      handle : [OUT] The handle of the FlexiDAG object
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSetTbar(flexidag_memblk_t *flexidag_tbar, AMBA_CV_FLEXIDAG_HANDLE_s *handle)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t start_time, end_time;

    retcode = CV_FlexidagOpenCheckParam(flexidag_tbar, handle);
    if(retcode == ERRCODE_NONE) {
        start_time = CV_FlexidagTimestamp();
        if (handle->fd_handle == NULL) {
            retcode = flexidag_create(&handle->fd_handle);
        }

        if (retcode == ERRCODE_NONE) {
            retcode = flexidag_add_tbar(handle->fd_handle, flexidag_tbar);
        }
        end_time = CV_FlexidagTimestamp();
        AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_FlexidagSetTbar : run_time = %d us", end_time - start_time, 0, 0, 0, 0);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagSetTbar() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

/**
 *  @brief      Set FlexiDAG using the input sfb and return the handle.
 *
 *  @param      flexidag_sfb : [IN] The FlexiDAG buffer copies from files containing CVTasks sfb.
 *  @param      handle : [OUT] The handle of the FlexiDAG object
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSetSFB(flexidag_memblk_t *flexidag_sfb, AMBA_CV_FLEXIDAG_HANDLE_s *handle)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t start_time, end_time;

    retcode = CV_FlexidagOpenCheckParam(flexidag_sfb, handle);
    if(retcode == ERRCODE_NONE) {
        start_time = CV_FlexidagTimestamp();
        if (handle->fd_handle == NULL) {
            retcode = flexidag_create(&handle->fd_handle);
        }

        if (retcode == ERRCODE_NONE) {
            retcode = flexidag_add_sfb(handle->fd_handle, flexidag_sfb);
        }
        end_time = CV_FlexidagTimestamp();
        AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_FlexidagSetSFB : run_time = %d us", end_time - start_time, 0, 0, 0, 0);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagSetSFB() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

/**
 *  @brief      Open the FlexiDAG using the input FlexiBin and return the handle.
 *
 *  @param      flexibin_buf : [IN] The FlexiBin buffer copies from files containing any CVTasks that runs on the VISORC.
 *  @param      handle : [OUT] The handle of the FlexiDAG object
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagOpen(flexidag_memblk_t *flexibin_buf, AMBA_CV_FLEXIDAG_HANDLE_s *handle)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr = 0UL;
    uint32_t start_time, end_time;
    uint32_t slot_id;
    uint32_t timestamp_slot;

    AMBA_CV_FLEXIDAG_LOG_MSG_s default_log_msg = {
        .flexidag_msg_entry = 8U,
        .cvtask_msg_entry = 4U,
        .arm_cpu_map = 0xFU,
        .arm_log_entry = 0U,
        .orc_log_entry = 512U,
    };

    retcode = CV_FlexidagOpenCheckParam(flexibin_buf, handle);
    if(retcode == ERRCODE_NONE) {
        start_time = CV_FlexidagTimestamp();
        retcode = errmgr_get_timestamp_slot(&timestamp_slot);
        if (retcode == ERRCODE_NONE) {
            retcode = errmgr_ipc_send_timestamp_start(CV_MODULE_ID_DRIVER, timestamp_slot, ERR_DRV_SCHDR_ERRMGR_FLEXIDAG_OPEN_TIMEOUT, 5000U);
            if (retcode == ERRCODE_NONE) {
                if (handle->fd_handle == NULL) {
                    retcode = flexidag_create(&handle->fd_handle);
                }
                if (retcode == ERRCODE_NONE) {
                    if (handle->set_log_msg == 0U) {
                        retcode = CV_ParamSetLogMsg(handle, &default_log_msg);
                    }

                    if (retcode == ERRCODE_NONE) {
                        retcode = flexidag_open(handle->fd_handle, flexibin_buf, &handle->mem_req);
                        if (retcode == ERRCODE_NONE) {
                            AmbaMisra_TypeCast(&addr, &handle);
                            AmbaPrint_PrintUInt5("AmbaCV_FlexidagOpen : handle = 0x%x state_buffer = 0x%x, temp_buffer = 0x%x, num_outputs = %d",
                                                 (uint32_t)addr,
                                                 handle->mem_req.flexidag_state_buffer_size,
                                                 handle->mem_req.flexidag_temp_buffer_size,
                                                 handle->mem_req.flexidag_num_outputs, 0U);
                            AmbaPrint_PrintUInt5("AmbaCV_FlexidagOpen : output_size = 0x%x 0x%x 0x%x 0x%x 0x%x ",
                                                 handle->mem_req.flexidag_output_buffer_size[0],
                                                 handle->mem_req.flexidag_output_buffer_size[1],
                                                 handle->mem_req.flexidag_output_buffer_size[2],
                                                 handle->mem_req.flexidag_output_buffer_size[3],
                                                 handle->mem_req.flexidag_output_buffer_size[4]);
                            slot_id = flexidag_find_slot_by_vphandle(handle->fd_handle);
                            if(slot_id < FLEXIDAG_MAX_SLOTS) {
                                if(AmbaCV_Handle_Init[slot_id] == 0U) {
                                    if(mutex_init(&api_run_mutex[slot_id]) != 0U) {
                                        AmbaPrint_PrintUInt5("AmbaCV_FlexidagOpen() : mutex_init fail", 0U, 0U, 0U, 0U, 0U);
                                        retcode = ERR_DRV_FLEXIDAG_MUTEX_INIT_FAIL;
                                    } else {
                                        AmbaCV_Handle_Init[slot_id] = 1U;
                                    }
                                }
                                if (retcode == ERRCODE_NONE) {
                                    if(AmbaWrap_memset(handle->api_run_start, 0, sizeof(handle->api_run_start)) != 0U) {
                                        AmbaPrint_PrintUInt5("AmbaCV_FlexidagOpen() : memset fail", 0U, 0U, 0U, 0U, 0U);
                                        retcode = ERR_DRV_FLEXIDAG_MEMSET_FAIL;
                                    } else {
                                        retcode = CV_FlexidagSetState(handle->fd_handle, FLEXIDAG_STATE_INIT);
                                        AmbaCV_Handle[slot_id] = handle;
                                    }
                                }
                            }
                            handle->api_run_index = 0U;
                        }

                        if (retcode == ERRCODE_NONE) {
                            retcode = errmgr_ipc_send_timestamp_stop(CV_MODULE_ID_DRIVER, timestamp_slot, ERR_DRV_SCHDR_ERRMGR_FLEXIDAG_OPEN_TIMEOUT);
                        }
                    }
                }
            }
        }
        end_time = CV_FlexidagTimestamp();
        AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_FlexidagOpen : run_time = %d us", end_time - start_time, 0, 0, 0, 0);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagOpen() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

static uint32_t CV_FlexidagInitCheckParam(const AMBA_CV_FLEXIDAG_HANDLE_s *handle, const AMBA_CV_FLEXIDAG_INIT_s *init)
{
    uint32_t retcode = ERRCODE_NONE;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(handle->fd_handle == NULL) {
        retcode = ERR_INTF_FLEXIDAG_HANDLE_INVALID;
    } else if(CV_FlexidagCheckState(handle->fd_handle, FLEXIDAG_STATE_INIT) != ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
    } else if( (init->state_buf.pBuffer == NULL) && (handle->mem_req.flexidag_state_buffer_size != 0U)) {
        AmbaPrint_PrintUInt5("CV_FlexidagInitCheckParam : init->state_buf.pBuffer == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_INTF_FLEXIDAG_STATE_MEMBLK_INVALID;
    } else if (((init->state_buf.buffer_daddr & (FD_MEM_ALIGN_SIZE - 1U)) != 0U) || ((init->state_buf.buffer_size & (FD_MEM_ALIGN_SIZE - 1U)) != 0U)) {
        AmbaPrint_PrintUInt5("CV_FlexidagInitCheckParam : init->state_buf is unalign", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_INTF_FLEXIDAG_STATE_ALIGN_INVALID;
    } else if ( init->state_buf.buffer_size < handle->mem_req.flexidag_state_buffer_size ) {
        AmbaPrint_PrintUInt5("CV_FlexidagInitCheckParam : init->state_buf size(0x%x) < (0x%x)",init->state_buf.buffer_size,handle->mem_req.flexidag_state_buffer_size, 0U, 0U, 0U);
        retcode = ERR_INTF_FLEXIDAG_STATE_SIZE_MISMATCHED;
    } else if ( (init->temp_buf.pBuffer == NULL) && (handle->mem_req.flexidag_temp_buffer_size != 0U)) {
        AmbaPrint_PrintUInt5("CV_FlexidagInitCheckParam : init->temp_buf.pBuffer == NULL ",0U,0U, 0U, 0U, 0U);
        retcode = ERR_INTF_FLEXIDAG_TEMP_MEMBLK_INVALID;
    } else if (((init->temp_buf.buffer_daddr & (FD_MEM_ALIGN_SIZE - 1U)) != 0U) || ((init->temp_buf.buffer_size & (FD_MEM_ALIGN_SIZE - 1U)) != 0U)) {
        AmbaPrint_PrintUInt5("CV_FlexidagInitCheckParam : init->temp_buf is unalign", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_INTF_FLEXIDAG_TEMP_ALIGN_INVALID;
    } else if (init->temp_buf.buffer_size < handle->mem_req.flexidag_temp_buffer_size) {
        AmbaPrint_PrintUInt5("CV_FlexidagInitCheckParam : init->temp_buf size(0x%x) < (0x%x)",init->temp_buf.buffer_size,handle->mem_req.flexidag_temp_buffer_size, 0U, 0U, 0U);
        retcode = ERR_INTF_FLEXIDAG_TEMP_SIZE_MISMATCHED;
    } else if (CV_FlexidagCheckCvRegion(&init->state_buf) != ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_STATE_OUT_OF_RANGE;
    } else if (CV_FlexidagCheckCvRegion(&init->temp_buf) != ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_TEMP_OUT_OF_RANGE;
    } else {
        //misra c
    }

    return retcode;
}

/**
 *  @brief      Initializes FlexiDAG and assigns internal buffer.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      init : [IN] The initial buffer required by FlexiDAG.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagInit(const AMBA_CV_FLEXIDAG_HANDLE_s *handle, AMBA_CV_FLEXIDAG_INIT_s *init)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t start_time, end_time;
    uint32_t timestamp_slot;

    retcode = CV_FlexidagInitCheckParam(handle, init);
    if(retcode == ERRCODE_NONE) {
        start_time = CV_FlexidagTimestamp();
        retcode = errmgr_get_timestamp_slot(&timestamp_slot);
        if (retcode == ERRCODE_NONE) {
            retcode = errmgr_ipc_send_timestamp_start(CV_MODULE_ID_DRIVER, timestamp_slot, ERR_DRV_SCHDR_ERRMGR_FLEXIDAG_INIT_TIMEOUT, 5000U);
            if (retcode == ERRCODE_NONE) {
                retcode = flexidag_set_state_buffer(handle->fd_handle, &init->state_buf);
                if (retcode == ERRCODE_NONE) {
                    retcode = flexidag_set_temp_buffer(handle->fd_handle, &init->temp_buf);
                    if (retcode == ERRCODE_NONE) {
                        retcode = flexidag_init(handle->fd_handle);
                        if (retcode == ERRCODE_NONE) {
                            retcode = CV_FlexidagSetState(handle->fd_handle, FLEXIDAG_STATE_READY);
                            if (retcode == ERRCODE_NONE) {
                                retcode = errmgr_ipc_send_timestamp_stop(CV_MODULE_ID_DRIVER, timestamp_slot, ERR_DRV_SCHDR_ERRMGR_FLEXIDAG_INIT_TIMEOUT);
                            }
                        }
                    }
                }
            }
        }
        end_time = CV_FlexidagTimestamp();
        AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_FlexidagInit : run_time = %d us", end_time - start_time, 0, 0, 0, 0);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagInit() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

static uint32_t CV_FlexidagCheckIOBuf(const AMBA_CV_FLEXIDAG_HANDLE_s *handle, AMBA_CV_FLEXIDAG_IO_s *in,AMBA_CV_FLEXIDAG_IO_s *out, flexidag_cb cb, void *cb_param)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t i;

    for(i = 0U; i < in->num_of_buf; i++) {
        if (((in->buf[i].buffer_daddr & (FD_MEM_ALIGN_SIZE - 1U)) != 0U) || ((in->buf[i].buffer_size & (FD_MEM_ALIGN_SIZE - 1U)) != 0U)) {
            AmbaPrint_PrintUInt5("CV_FlexidagCheckIOBuf : in->buf[%d] is unalign", i, 0U, 0U, 0U, 0U);
            retcode = ERR_INTF_FLEXIDAG_INPUT_ALIGN_INVALID;
        } else if (CV_FlexidagCheckCvRegion(&in->buf[i]) != ERRCODE_NONE) {
            retcode = ERR_INTF_FLEXIDAG_INPUT_OUT_OF_RANGE;
        } else {
            //AmbaPrint_PrintUInt5("flexidag_set_input_buffer:  num = %d, va = 0x%p, pa = 0x%x, size = %d ", i, in->buf[i].pBuffer, in->buf[i].buffer_daddr, in->buf[i].buffer_size, 0U);
            retcode = flexidag_set_input_buffer(handle->fd_handle, i, &in->buf[i]);
        }

        if(retcode != ERRCODE_NONE) {
            break;
        }
    }

    if(retcode == ERRCODE_NONE) {
        for(i = 0U; i < out->num_of_buf; i++) {
            if (((out->buf[i].buffer_daddr & (FD_MEM_ALIGN_SIZE - 1U)) != 0U) || ((out->buf[i].buffer_size & (FD_MEM_ALIGN_SIZE - 1U)) != 0U)) {
                AmbaPrint_PrintUInt5("CV_FlexidagCheckIOBuf : out->buf[%d] is unalign", i, 0U, 0U, 0U, 0U);
                retcode = ERR_INTF_FLEXIDAG_OUTPUT_ALIGN_INVALID;
            } else if (out->buf[i].buffer_size < handle->mem_req.flexidag_output_buffer_size[i]) {
                AmbaPrint_PrintUInt5("CV_FlexidagCheckIOBuf : out size(0x%x) < (0x%x)", i, out->buf[i].buffer_size, handle->mem_req.flexidag_output_buffer_size[i], 0U, 0U);
                retcode = ERR_INTF_FLEXIDAG_OUTPUT_SIZE_MISMATCHED;
            } else if (CV_FlexidagCheckCvRegion(&out->buf[i]) != ERRCODE_NONE) {
                retcode = ERR_INTF_FLEXIDAG_OUTPUT_OUT_OF_RANGE;
            } else {
                //AmbaPrint_PrintUInt5("flexidag_set_output_buffer: num = %d, va = 0x%p, pa = 0x%x, size = %d ", i, out->buf[i].pBuffer, out->buf[i].buffer_daddr, out->buf[i].buffer_size, 0U);
                retcode = flexidag_set_output_buffer(handle->fd_handle, i, &out->buf[i]);
                if (retcode == ERRCODE_NONE) {
                    if(cb != NULL) {
                        retcode = flexidag_set_output_callback(handle->fd_handle, i, cb, cb_param);
                    }
                }
            }

            if(retcode != ERRCODE_NONE) {
                break;
            }
        }
    }
    return retcode;
}

static uint32_t CV_FlexidagRunCheckParam(const AMBA_CV_FLEXIDAG_HANDLE_s *handle, const AMBA_CV_FLEXIDAG_IO_s *in, const AMBA_CV_FLEXIDAG_IO_s *out, const AMBA_CV_FLEXIDAG_RUN_INFO_s *run_info)
{
    uint32_t retcode = ERRCODE_NONE;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(handle->fd_handle == NULL) {
        retcode = ERR_INTF_FLEXIDAG_HANDLE_INVALID;
    } else if(CV_FlexidagCheckState(handle->fd_handle, FLEXIDAG_STATE_READY) != ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
    } else if((in == NULL) || (out == NULL)) {
        retcode = ERR_INTF_FLEXIDAG_IO_MEMBLK_INVALID;
    } else if(run_info == NULL) {
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else {
        //misra c
    }

    return retcode;
}

/**
 *  @brief      With the provided input and output buffer, run FlexiDAG in blocking mode and return run information.  \n
 *              Application should do cache handling on input and output buffer if necessary.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      in : [IN] The input buffer required by FlexiDAG run.
 *  @param      out : [IN] The output buffer required by FlexiDAG run, the output result is filled in the buffer after this API is complete..
 *  @param      run_info : [OUT] Information of this run.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagRun(const AMBA_CV_FLEXIDAG_HANDLE_s *handle,AMBA_CV_FLEXIDAG_IO_s *in,AMBA_CV_FLEXIDAG_IO_s *out,AMBA_CV_FLEXIDAG_RUN_INFO_s *run_info)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_runinfo_t info;
    uint32_t i;
    uint32_t timestamp_slot;
    FLOAT fltmp;

    retcode = CV_FlexidagRunCheckParam(handle, in, out, run_info);
    if(retcode == ERRCODE_NONE) {
        run_info->api_start_time = cvtask_get_timestamp();
        retcode = errmgr_get_timestamp_slot(&timestamp_slot);
        if (retcode == ERRCODE_NONE) {
            retcode = errmgr_ipc_send_timestamp_start(CV_MODULE_ID_DRIVER, timestamp_slot, ERR_DRV_SCHDR_ERRMGR_FLEXIDAG_RUN_TIMEOUT, 5000U);
            if (retcode == ERRCODE_NONE) {
                retcode = flexidag_prep_run(handle->fd_handle);
                if (retcode == ERRCODE_NONE) {
                    retcode = CV_FlexidagCheckIOBuf(handle, in, out, NULL, NULL);
                    if(retcode == ERRCODE_NONE) {
                        retcode = flexidag_run(handle->fd_handle, &info);
                        if (retcode == ERRCODE_NONE) {
                            run_info->overall_retcode = info.overall_retcode;
                            run_info->output_not_generated = info.output_not_generated;
                            if(run_info->output_not_generated == 0U) {
                                fltmp = (FLOAT) info.start_time;
                                fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
                                run_info->start_time = (uint32_t)fltmp;

                                fltmp = (FLOAT) info.end_time;
                                fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
                                run_info->end_time = (uint32_t)fltmp;

                                for(i = 0U; i < out->num_of_buf; i++) {
                                    fltmp = (FLOAT) info.output_donetime[i];
                                    fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
                                    run_info->output_donetime[i] = (uint32_t)fltmp;
                                }

                                fltmp = (FLOAT) run_info->api_start_time;
                                fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
                                run_info->api_start_time = (uint32_t)fltmp;

                                fltmp = (FLOAT) cvtask_get_timestamp();
                                fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
                                run_info->api_end_time = (uint32_t)fltmp;
                            }
                            retcode = errmgr_ipc_send_timestamp_stop(CV_MODULE_ID_DRIVER, timestamp_slot, ERR_DRV_SCHDR_ERRMGR_FLEXIDAG_RUN_TIMEOUT);
                        }
                    }
                }
            }
        }
        AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_FlexidagRun : run_time = %d us", run_info->api_end_time - run_info->api_start_time, 0, 0, 0, 0);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagRun() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

/**
 *  @brief      With the provided input and output buffer, run FlexiDAG in blocking mode and return run information.  \n
 *              Application should do cache handling on input and output buffer if necessary.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      cb : [IN] The output callback function.
 *  @param      cb_param : [IN] Input parameter of callback function.
 *  @param      in : [IN] The input buffer required by FlexiDAG run.
 *  @param      out : [IN] The output buffer required by FlexiDAG run; the output result is filled in the buffer after callback woken up.
 *  @param      token_id : [OUT] Used to get run information.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagRunNonBlock(AMBA_CV_FLEXIDAG_HANDLE_s *handle, flexidag_cb cb, void *cb_param, AMBA_CV_FLEXIDAG_IO_s *in,AMBA_CV_FLEXIDAG_IO_s *out, uint32_t *token_id)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t start_time,slot_id;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(handle->fd_handle == NULL) {
        retcode = ERR_INTF_FLEXIDAG_HANDLE_INVALID;
    } else if(CV_FlexidagCheckState(handle->fd_handle, FLEXIDAG_STATE_READY) != ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
    } else if((in == NULL) || (out == NULL)) {
        retcode = ERR_INTF_FLEXIDAG_IO_MEMBLK_INVALID;
    } else {
        start_time = cvtask_get_timestamp();
        retcode = flexidag_prep_run(handle->fd_handle);
        if (retcode == ERRCODE_NONE) {
            retcode = CV_FlexidagCheckIOBuf(handle, in, out, cb, cb_param);
            if(retcode == ERRCODE_NONE) {
                retcode = flexidag_run_noblock(handle->fd_handle, token_id);
                if (retcode == ERRCODE_NONE) {
                    slot_id = flexidag_find_slot_by_vphandle(handle->fd_handle);
                    if(slot_id < FLEXIDAG_MAX_SLOTS) {
                        if(mutex_lock(&api_run_mutex[slot_id]) != 0U) {
                            AmbaPrint_PrintUInt5("AmbaCV_FlexidagRunNonBlock() : mutex_lock fail", 0U, 0U, 0U, 0U, 0U);
                            retcode = ERR_DRV_FLEXIDAG_MUTEX_LOCK_FAIL;
                        } else {
                            handle->api_run_start[handle->api_run_index].at_time = start_time;
                            handle->api_run_start[handle->api_run_index].token_id = *token_id;
                            handle->api_run_index = (handle->api_run_index + 1U)%FLEXIDAG_API_TIME_BUF;
                            if(mutex_unlock(&api_run_mutex[slot_id]) != 0U) {
                                AmbaPrint_PrintUInt5("AmbaCV_FlexidagRunNonBlock() : mutex_unlock fail", 0U, 0U, 0U, 0U, 0U);
                                retcode = ERR_DRV_FLEXIDAG_MUTEX_UNLOCK_FAIL;
                            }
                        }
                    } else {
                        AmbaPrint_PrintUInt5("AmbaCV_FlexidagRunNonBlock() : flexidag_find_slot_by_vphandle fail", 0U, 0U, 0U, 0U, 0U);
                        retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
                    }
                }
            }
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagRunNonBlock() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

static uint32_t CV_FlexidagWaitRunFinishCheckParam(const AMBA_CV_FLEXIDAG_HANDLE_s *handle)
{
    uint32_t retcode = ERRCODE_NONE;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(handle->fd_handle == NULL) {
        retcode = ERR_INTF_FLEXIDAG_HANDLE_INVALID;
    } else if(CV_FlexidagCheckState(handle->fd_handle, FLEXIDAG_STATE_READY) != ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
    } else {
        //misra c
    }

    return retcode;
}

/**
 *  @brief      This is used with non-blocking run API. When this API is called, it returns full run information by token ID until all outputs are complete.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      token_id : [IN] The input buffer required by FlexiDAG run.
 *  @param      pRunInfo : [OUT] The output buffer required by FlexiDAG run.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagWaitRunFinish(AMBA_CV_FLEXIDAG_HANDLE_s *handle, uint32_t token_id, AMBA_CV_FLEXIDAG_RUN_INFO_s *pRunInfo)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_runinfo_t info;
    uint32_t i, start_time = 0U;
    uint32_t timestamp_slot,slot_id;
    FLOAT fltmp;

    AmbaMisra_TouchUnused(handle);
    slot_id = flexidag_find_slot_by_vphandle(handle->fd_handle);
    if(slot_id < FLEXIDAG_MAX_SLOTS) {
        if(mutex_lock(&api_run_mutex[slot_id]) != 0U) {
            AmbaPrint_PrintUInt5("AmbaCV_FlexidagWaitRunFinish() : mutex_lock fail", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_MUTEX_LOCK_FAIL;
        } else {
            for(i = 0U; i < FLEXIDAG_API_TIME_BUF; i++) {
                if(handle->api_run_start[i].token_id == token_id) {
                    start_time = handle->api_run_start[i].at_time;
                    break;
                }
            }
            if(mutex_unlock(&api_run_mutex[slot_id]) != 0U) {
                AmbaPrint_PrintUInt5("AmbaCV_FlexidagWaitRunFinish() : mutex_unlock fail", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_FLEXIDAG_MUTEX_UNLOCK_FAIL;
            }
        }
    } else {
        AmbaPrint_PrintUInt5("AmbaCV_FlexidagWaitRunFinish() : flexidag_find_slot_by_vphandle fail", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
    }

    if( retcode == ERRCODE_NONE ) {
        retcode = CV_FlexidagWaitRunFinishCheckParam(handle);
        if(retcode != ERRCODE_NONE) {
            AmbaPrint_PrintUInt5("AmbaCV_FlexidagWaitRunFinish() : CV_FlexidagWaitRunFinishCheckParam fail", 0U, 0U, 0U, 0U, 0U);
        } else {
            retcode = errmgr_get_timestamp_slot(&timestamp_slot);
            if (retcode == ERRCODE_NONE) {
                retcode = errmgr_ipc_send_timestamp_start(CV_MODULE_ID_DRIVER, timestamp_slot, ERR_DRV_SCHDR_ERRMGR_FLEXIDAG_WAIT_RUN_FINISH_TIMEOUT, 5000U);
                if (retcode == ERRCODE_NONE) {
                    retcode = flexidag_wait_run_finish(handle->fd_handle, token_id, &info);
                    if (retcode == ERRCODE_NONE) {
                        pRunInfo->overall_retcode = info.overall_retcode;
                        pRunInfo->output_not_generated = info.output_not_generated;
                        if((pRunInfo->output_not_generated == 0U) && (start_time != 0U)) {
                            fltmp = (FLOAT) info.start_time;
                            fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
                            pRunInfo->start_time = (uint32_t)fltmp;

                            fltmp = (FLOAT) info.end_time;
                            fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
                            pRunInfo->end_time = (uint32_t)fltmp;

                            for(i = 0U; i < (uint32_t)FLEXIDAG_MAX_OUTPUTS; i++) {
                                if(info.output_donetime[i] != 0U) {
                                    fltmp = (FLOAT) info.output_donetime[i];
                                    fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
                                    pRunInfo->output_donetime[i] = (uint32_t)fltmp;
                                }
                            }

                            fltmp = (FLOAT) start_time;
                            fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
                            pRunInfo->api_start_time = (uint32_t)fltmp;

                            fltmp = (FLOAT) cvtask_get_timestamp();
                            fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
                            pRunInfo->api_end_time = (uint32_t)fltmp;
                            AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_FlexidagWaitRunFinish : run_time = %d us", pRunInfo->api_end_time - pRunInfo->api_start_time, 0, 0, 0, 0);
                        } else {
                            AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_FlexidagWaitRunFinish : run_time = unknow us", 0, 0, 0, 0, 0);
                        }
                        retcode = errmgr_ipc_send_timestamp_stop(CV_MODULE_ID_DRIVER, timestamp_slot, ERR_DRV_SCHDR_ERRMGR_FLEXIDAG_WAIT_RUN_FINISH_TIMEOUT);
                    }
                }
            }
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagWaitRunFinish() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

/**
 *  @brief      Dump log of the FlexiDAG. The type of the log is controlled by flags, and the log can be the running log of the FlexiDAG or performance log. \n
 *              The flags are like FLEXILOG_XXX defined in cvapi_flexidag.h..
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      path : [IN] The file path of log, if path == NULL, output to console.
 *  @param      flags : [IN] Output message type of log.    \n
 *                                  FLEXILOG_VIS_PERF             0x00000000U \n
 *                                  FLEXILOG_VIS_SCHED           0x00010000U \n
 *                                  FLEXILOG_VIS_CVTASK         0x00020000U
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagDumpLog(const AMBA_CV_FLEXIDAG_HANDLE_s *handle, const char *path, uint32_t flags)
{
    uint32_t retcode = ERRCODE_NONE;
    char buffer[1024];
    uint32_t num_written;
    uint32_t is_first = 1U;
    FILE_WRAP *Fp;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(handle->fd_handle == NULL) {
        retcode = ERR_INTF_FLEXIDAG_HANDLE_INVALID;
    } else if(CV_FlexidagCheckState(handle->fd_handle, FLEXIDAG_STATE_READY) != ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
    } else {
        if((uint32_t)schdr_flush_visorc_printf(1U) != ERRCODE_NONE) {
            AmbaPrint_PrintUInt5("[WARNING] AmbaCV_FlexidagDumpLog() : schdr_flush_visorc_printf fail ", 0U, 0U, 0U, 0U, 0U);
        }
        if ( ( flags & FLEXILOG_TYPEMASK) == FLEXILOG_VIS_CVTASK ) {
            if ( (handle->set_log_msg == 1U) && (handle->log_msg.orc_log_entry == 0U) ) {
                AmbaPrint_PrintUInt5("AmbaCV_FlexidagDumpLog : not assign orc_log_entry for FLEXILOG_VIS_CVTASK", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_INTF_FLEXIDAG_LOG_UNAVAILABLE;
            } else {
                if(path == NULL) {
                    num_written = flexidag_dump_log(handle->fd_handle, NULL, 0U, flags | FLEXILOG_PREPEND_HEADER | FLEXILOG_ECHO_TO_CONSOLE);
                    AmbaPrint_PrintUInt5("AmbaCV_FlexidagDumpLog : echo to console : size=%d", num_written, 0U, 0U, 0U, 0U);
                } else {
                    Fp = fopen_wrap(path, "wb");
                    if( NULL == Fp ) {
                        AmbaPrint_PrintStr5("AmbaCV_FlexidagDumpLog : fopen_wrap() failed : path = %s ", path, NULL, NULL, NULL, NULL);
                        retcode = ERR_INTF_FLEXIDAG_LOG_PATH_UNAVAILABLE;
                    } else {
                        do {
                            if (is_first != 0U) {
                                num_written = flexidag_dump_log(handle->fd_handle, buffer, (uint32_t)sizeof(buffer), flags | FLEXILOG_PREPEND_HEADER);
                                is_first = 0U;
                            } else {
                                num_written = flexidag_dump_log(handle->fd_handle, buffer, (uint32_t)sizeof(buffer), flags);
                            }
                            if(fwrite_wrap(buffer, 1U, num_written, Fp) != num_written) {
                                AmbaPrint_PrintUInt5("AmbaCV_FlexidagDumpLog : fwrite_wrap fail", 0U, 0U, 0U, 0U, 0U);
                                retcode = ERR_DRV_FLEXIDAG_FILE_WRITE_FAIL;
                                break;
                            }
                        } while (num_written != 0U);
                        if(fclose_wrap(Fp) != 0U) {
                            AmbaPrint_PrintUInt5("AmbaCV_FlexidagDumpLog : fclose_wrap fail", 0U, 0U, 0U, 0U, 0U);
                        }
                    }
                }
            }
        } else {
            AmbaPrint_PrintUInt5("AmbaCV_FlexidagDumpLog : only support FLEXILOG_VIS_CVTASK", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_INTF_FLEXIDAG_LOG_TYPE_UNAVAILABLE;
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagDumpLog() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

static void CV_FlexidagGetSysPerf(cvlog_perf_flexidag_entry_t **perf_ptr, uint32_t *perf_size)
{
    void *ptr;

    pFlexidagTrace = schdr_get_pFlexidagTrace();
    if(pFlexidagTrace != NULL) {
        /* invalidate flexidag_trace_t */
        AmbaMisra_TypeCast(&ptr, &pFlexidagTrace);
        if( ambacv_cache_invalidate(ptr, sizeof(flexidag_trace_t)) != ERRCODE_NONE ) {
            AmbaPrint_PrintUInt5("[ERROR] CV_FlexidagGetSysPerf() : invalidate flexidag_trace_t fail ", 0U, 0U, 0U, 0U, 0U);
        }

        /* invalidate sysvis_printbuf_perf  */
        ptr = ambacv_c2v(pFlexidagTrace->sysvis_printbuf_perf_daddr[0]);
        *perf_size =  pFlexidagTrace->sysvis_printbuf_perf_size[0];
        if( ambacv_cache_invalidate(ptr, *perf_size) != ERRCODE_NONE ) {
            AmbaPrint_PrintUInt5("[ERROR] CV_FlexidagGetSysPerf() : invalidate sysvis_printbuf_perf fail ", 0U, 0U, 0U, 0U, 0U);
        }
        AmbaMisra_TypeCast(perf_ptr, &ptr);
    } else {
        *perf_ptr = NULL;
        *perf_size = 0U;
        AmbaPrint_PrintStr5("[ERROR]CV_FlexidagGetSysPerf : pFlexidagTrace == NULL ", NULL, NULL, NULL, NULL, NULL);
    }

}

static void CV_FlexidagTickToUs(uint32_t totoal_time, AMBA_CV_FLEXIDAG_PERF_s *flow_id_perf_array, uint32_t num_of_array)
{
    uint32_t i;
    FLOAT fltmp;

    for(i = 0U; i < num_of_array; i++) {
        if(flow_id_perf_array[i].runs > 0U) {
            flow_id_perf_array[i].avg_frame_time = ( flow_id_perf_array[i].avg_frame_time/flow_id_perf_array[i].runs);
            fltmp = (FLOAT) flow_id_perf_array[i].avg_frame_time;
            fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
            flow_id_perf_array[i].avg_frame_time = (uint32_t)fltmp;

            fltmp = (FLOAT) flow_id_perf_array[i].min_frame_time;
            fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
            flow_id_perf_array[i].min_frame_time = (uint32_t)fltmp;

            fltmp = (FLOAT) flow_id_perf_array[i].max_frame_time;
            fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
            flow_id_perf_array[i].max_frame_time = (uint32_t)fltmp;

            flow_id_perf_array[i].avg_time = ( flow_id_perf_array[i].total_run_time/flow_id_perf_array[i].runs);
            fltmp = (FLOAT) flow_id_perf_array[i].avg_time;
            fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
            flow_id_perf_array[i].avg_time = (uint32_t)fltmp;

            fltmp = (FLOAT) flow_id_perf_array[i].min_time;
            fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
            flow_id_perf_array[i].min_time = (uint32_t)fltmp;

            fltmp = (FLOAT) flow_id_perf_array[i].max_time;
            fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
            flow_id_perf_array[i].max_time = (uint32_t)fltmp;

            fltmp = (FLOAT) flow_id_perf_array[i].total_run_time;
            fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
            flow_id_perf_array[i].total_run_time = (uint32_t)fltmp;
        }
        fltmp = (FLOAT) totoal_time;
        fltmp = (FLOAT)(fltmp / CV_GetAudioClockUs());
        flow_id_perf_array[i].total_time = (uint32_t)fltmp;
    }

}

typedef struct {
    uint32_t temp;
    uint32_t frame_temp;
    uint32_t diff;
    uint32_t frame_diff;
} FLEXIDAG_CALC_PERF_s;

static void CV_FlexidagPerfDoCalc(AMBA_CV_FLEXIDAG_PERF_s *result_perf, FLEXIDAG_CALC_PERF_s *calc_perf, uint32_t time_stamp)
{
    if( (calc_perf->frame_temp != 0U) && (time_stamp > calc_perf->frame_temp)) {
        // update vp stastistic
        if( (calc_perf->temp != 0U) && (time_stamp > calc_perf->temp)) {
            calc_perf->diff += (time_stamp - calc_perf->temp);

            if( (result_perf->min_time == 0U) || (result_perf->min_time > calc_perf->diff)) {
                result_perf->min_time = calc_perf->diff;
            }

            if( (result_perf->max_time == 0U) || (result_perf->max_time < calc_perf->diff) ) {
                result_perf->max_time = calc_perf->diff;
            }
            result_perf->total_run_time += calc_perf->diff;
        }
        calc_perf->temp = 0U;
        calc_perf->diff = 0U;
        // update frame stastistic
        calc_perf->frame_diff = (time_stamp - calc_perf->frame_temp);
        if( (result_perf->min_frame_time == 0U) || (result_perf->min_frame_time > calc_perf->frame_diff)) {
            result_perf->min_frame_time = calc_perf->frame_diff;
        }

        if( (result_perf->max_frame_time == 0U) || (result_perf->max_frame_time < calc_perf->frame_diff) ) {
            result_perf->max_frame_time = calc_perf->frame_diff;
        }
        result_perf->avg_frame_time += calc_perf->frame_diff;
        result_perf->runs += 1U;
    }
    calc_perf->frame_temp = 0U;
    calc_perf->frame_diff = 0U;
}

static void CV_FlexidagPerfDo(AMBA_CV_FLEXIDAG_PERF_s *result_perf, FLEXIDAG_CALC_PERF_s *calc_perf, uint8_t event, uint32_t time_stamp)
{
    if ((event == (uint8_t)EVENT_FLEXIDAG_NEW_FRAME) || (event == (uint8_t)EVENT_NEW_FRAME)) {
        calc_perf->frame_temp = time_stamp;
        calc_perf->temp = time_stamp;
    } else if ((event == (uint8_t)EVENT_FLEXIDAG_RESUME_FRAME) || (event == (uint8_t)EVENT_RESUME_FRAME)) {
        calc_perf->temp = time_stamp;
    } else if((event == (uint8_t)EVENT_FLEXIDAG_CVCORE_YIELD) || (event == (uint8_t)EVENT_CVCORE_YIELD)) {
        if( (calc_perf->temp != 0U) && (time_stamp > calc_perf->temp)) {
            calc_perf->diff += (time_stamp - calc_perf->temp);
        }
        calc_perf->temp = 0U;
    } else {
        if ((event == (uint8_t)EVENT_FLEXIDAG_CVCORE_OFF) || (event == (uint8_t)EVENT_CVCORE_OFF)) {
            CV_FlexidagPerfDoCalc(result_perf, calc_perf, time_stamp);
        }
    }

}

static uint32_t CV_FlexidagPerfCoreChk(uint32_t target_core, uint32_t expect_core)
{
    uint32_t ret;

#if defined (CHIP_CV6)
    if ((expect_core == CVCORE_VP0) && (target_core >= CVCORE_NVP0) && (target_core <= CVCORE_NVP5)) {
        ret = 1U;
    } else if ((expect_core == CVCORE_FEX) && (target_core >= CVCORE_FEX0) && (target_core <= CVCORE_FEX1)) {
        ret = 1U;
    } else if ((expect_core == CVCORE_FMA) && (target_core >= CVCORE_FMA0) && (target_core <= CVCORE_FMA1)) {
        ret = 1U;
    } else if (expect_core == target_core) {
        ret = 1U;
    } else {
        ret = 0U;
    }
#else
    if(expect_core == target_core) {
        ret = 1U;
    } else {
        ret = 0U;
    }

#endif

    return ret;
}

static uint32_t CV_FlexidagPerfTimeAdj(uint32_t time_stamp)
{
    uint32_t adj_time_stamp;

#if defined (CHIP_CV6)
    adj_time_stamp = (time_stamp&0x03FFFFFFU) << 6U;  // CV6 time stamp in perf divided audio clock by 64
#else
    adj_time_stamp = time_stamp;
#endif

    return adj_time_stamp;
}

/*          Example of a run event              */
/*          EVENT_FLEXIDAG_NEW_FRAME        */
/*          EVENT_FLEXIDAG_CVCORE_ON        */
/*          EVENT_FLEXIDAG_CVCORE_YIELD     */
/*          EVENT_FLEXIDAG_RESUME_FRAME */
/*          EVENT_FLEXIDAG_CVCORE_ON        */
/*          EVENT_FLEXIDAG_CVCORE_OFF       */
static void CV_FlexidagPerf(const AMBA_CV_FLEXIDAG_HANDLE_s *handle, uint32_t cvcore, uint32_t perf_start, uint32_t perf_end, AMBA_CV_FLEXIDAG_PERF_s *flow_id_perf_array, uint32_t num_of_array)
{
    cvlog_perf_flexidag_entry_t *perf_ptr;
    uint32_t i,total_entry,flexidag_slot_id,perf_size;
    FLEXIDAG_CALC_PERF_s calc_perf = {0};
    uint32_t perf_entry_min = 0U,perf_entry_max = 0U;

    flexidag_slot_id = flexidag_find_slot_by_vphandle(handle->fd_handle);
    CV_FlexidagGetSysPerf(&perf_ptr, &perf_size);
    if(perf_ptr != NULL) {
        total_entry = perf_size/((uint32_t)sizeof(cvlog_perf_flexidag_entry_t));
        for(i = 0U; i < total_entry; i++) {
            uint32_t adj_time_stamp;

            adj_time_stamp = CV_FlexidagPerfTimeAdj(perf_ptr[i].time_stamp);
            if( (perf_ptr[i].flexidag_slot_id != flexidag_slot_id) || (perf_ptr[i].sysflow_index >= num_of_array) || (CV_FlexidagPerfCoreChk(perf_ptr[i].cvcore,cvcore) == 0U) ) {
                continue;
            } else if ( (adj_time_stamp == 0U) || (adj_time_stamp < perf_start) || (adj_time_stamp > perf_end) ) {
                continue;
            } else {
                if((perf_entry_min == 0U) || (perf_entry_min > adj_time_stamp)) {
                    perf_entry_min = adj_time_stamp;
                }

                if(perf_entry_max < adj_time_stamp) {
                    perf_entry_max = adj_time_stamp;
                }
                CV_FlexidagPerfDo(&flow_id_perf_array[perf_ptr[i].sysflow_index], &calc_perf, (uint8_t)perf_ptr[i].event,adj_time_stamp);
            }
        }
    }
}

#if !defined(ENV_IS_PACE_LINUX)
#define INTERAL_FLOW_ID_ARRAY       8U
static AMBA_CV_FLEXIDAG_PERF_s all_result_perf[FLEXIDAG_MAX_SLOTS][INTERAL_FLOW_ID_ARRAY] GNU_SECTION_NOZEROINIT;
static AMBA_CV_FLEXIDAG_PERF_s autorun_result_perf;
static uint32_t perf_all_start = 0U,perf_all_end = 0U;
static uint32_t perf_all_entry_min = 0U, perf_all_entry_max = 0U;
static uint32_t perf_all_start_idx = 0U,perf_all_end_idx = 0U;

static void CV_FlexidagPerfAllCombine(uint32_t perf_interval,AMBA_CV_FLEXIDAG_PERF_s *flexidag_perf_array, uint32_t num_of_array, uint32_t *num_of_flexidag)
{
    uint32_t i,j,exist;

    *num_of_flexidag = 0U;
    if(autorun_result_perf.runs != 0U) {
        const char autorun_name[FLEXIDAG_LABEL_SIZE] = "auto_run";

        AmbaUtility_StringCopy(flexidag_perf_array[*num_of_flexidag].label, FLEXIDAG_LABEL_SIZE - 1U, autorun_name);
        flexidag_perf_array[*num_of_flexidag].label[FLEXIDAG_LABEL_SIZE - 1U] = '\0';

        CV_FlexidagTickToUs(perf_interval, &autorun_result_perf, 1);
        flexidag_perf_array[*num_of_flexidag].total_time = autorun_result_perf.total_time;
        flexidag_perf_array[*num_of_flexidag].runs = autorun_result_perf.runs;
        flexidag_perf_array[*num_of_flexidag].avg_time = autorun_result_perf.avg_time;
        flexidag_perf_array[*num_of_flexidag].min_time = autorun_result_perf.min_time;
        flexidag_perf_array[*num_of_flexidag].max_time = autorun_result_perf.max_time;
        flexidag_perf_array[*num_of_flexidag].avg_frame_time = autorun_result_perf.avg_frame_time;
        flexidag_perf_array[*num_of_flexidag].min_frame_time = autorun_result_perf.min_frame_time;
        flexidag_perf_array[*num_of_flexidag].max_frame_time = autorun_result_perf.max_frame_time;
        flexidag_perf_array[*num_of_flexidag].total_run_time = autorun_result_perf.total_run_time;
        *num_of_flexidag += 1U;
    }

    for( i = 0U; i < (uint32_t)FLEXIDAG_MAX_SLOTS; i++) {
        if( *num_of_flexidag >= num_of_array ) {
            AmbaPrint_PrintUInt5("[ERROR]CV_FlexidagPerfAll : num_of_array (%d) is not enough ", num_of_array, 0U, 0U, 0U, 0U);
            break;
        } else {
            exist = 0U;
            for( j = 0U; j < INTERAL_FLOW_ID_ARRAY; j++) {
                if(all_result_perf[i][j].runs != 0U) {
                    exist = 1U;
                    break;
                }
            }
            if(exist != 0U) {
                const char *flexidag_name;
                void *ptr;
                uint32_t numwritten;
                const char *args[1];

                ptr = ambacv_c2v(pFlexidagTrace->slottrace[i].flexidag_name_daddr);
                AmbaMisra_TypeCast(&flexidag_name, &ptr);
                if(flexidag_name != NULL) {
                    if( ambacv_cache_invalidate(ptr, FLEXIDAG_LABEL_SIZE) != ERRCODE_NONE ) {
                        AmbaPrint_PrintUInt5("[ERROR] CV_FlexidagPerfAllCombine() : invalidate fail ", 0U, 0U, 0U, 0U, 0U);
                    }
                    args[0] = flexidag_name;
                    numwritten = AmbaUtility_StringPrintUInt32(&flexidag_perf_array[*num_of_flexidag].label[0], FLEXIDAG_LABEL_SIZE - 1U, "[%d]", 1U, &i);
                    numwritten += AmbaUtility_StringPrintStr(&flexidag_perf_array[*num_of_flexidag].label[numwritten], FLEXIDAG_LABEL_SIZE - 1U, " %s", 1U, args);
                    flexidag_perf_array[*num_of_flexidag].label[FLEXIDAG_LABEL_SIZE - 1U] = '\0';
                    (void) numwritten;
                }

                CV_FlexidagTickToUs(perf_interval, all_result_perf[i], INTERAL_FLOW_ID_ARRAY);
                flexidag_perf_array[*num_of_flexidag].total_time = all_result_perf[i][0].total_time;
                for( j = 0U; j < INTERAL_FLOW_ID_ARRAY; j++) {
                    if(flexidag_perf_array[*num_of_flexidag].runs < all_result_perf[i][j].runs) {
                        flexidag_perf_array[*num_of_flexidag].runs = all_result_perf[i][j].runs;
                    }
                    flexidag_perf_array[*num_of_flexidag].avg_time += all_result_perf[i][j].avg_time;
                    flexidag_perf_array[*num_of_flexidag].min_time += all_result_perf[i][j].min_time;
                    flexidag_perf_array[*num_of_flexidag].max_time += all_result_perf[i][j].max_time;
                    flexidag_perf_array[*num_of_flexidag].avg_frame_time += all_result_perf[i][j].avg_frame_time;
                    flexidag_perf_array[*num_of_flexidag].min_frame_time += all_result_perf[i][j].min_frame_time;
                    flexidag_perf_array[*num_of_flexidag].max_frame_time += all_result_perf[i][j].max_frame_time;
                    flexidag_perf_array[*num_of_flexidag].total_run_time += all_result_perf[i][j].total_run_time;
                }
                *num_of_flexidag += 1U;
            }
        }
    }
}

static void CV_FlexidagPerfAll(uint32_t event, uint32_t cvcore, AMBA_CV_FLEXIDAG_PERF_s *flexidag_perf_array, uint32_t num_of_array, uint32_t *num_of_flexidag)
{
    cvlog_perf_flexidag_entry_t *perf_ptr;
    const cvlog_perf_entry_t *auto_perf_ptr;
    uint32_t i,total_entry,perf_size;
    FLEXIDAG_CALC_PERF_s autorun_calc_perf;
    static FLEXIDAG_CALC_PERF_s all_calc_perf[FLEXIDAG_MAX_SLOTS][INTERAL_FLOW_ID_ARRAY];

    CV_FlexidagGetSysPerf(&perf_ptr, &perf_size);
    AmbaMisra_TypeCast(&auto_perf_ptr, &perf_ptr);
    if(perf_ptr != NULL) {
        if(AmbaWrap_memset(&autorun_calc_perf, 0, sizeof(autorun_calc_perf)) != 0U) {
            AmbaPrint_PrintUInt5("[ERROR] CV_FlexidagPerfAll() : autorun_calc_perf memset fail ", 0U, 0U, 0U, 0U, 0U);
        }

        if(AmbaWrap_memset(&autorun_result_perf, 0, sizeof(autorun_result_perf)) != 0U) {
            AmbaPrint_PrintUInt5("[ERROR] CV_FlexidagPerfAll() : autorun_result_perf memset fail ", 0U, 0U, 0U, 0U, 0U);
        }

        for( i = 0U; i < (uint32_t)FLEXIDAG_MAX_SLOTS; i++) {
            if(AmbaWrap_memset(all_calc_perf[i], 0, sizeof(all_calc_perf[i])) != 0U) {
                AmbaPrint_PrintUInt5("[ERROR] CV_FlexidagPerfAll() : all_calc_perf[%d] memset fail ", i, 0U, 0U, 0U, 0U);
            }
            if(AmbaWrap_memset(all_result_perf[i], 0, sizeof(all_result_perf[i])) != 0U) {
                AmbaPrint_PrintUInt5("[ERROR] CV_FlexidagPerfAll() : all_result_perf[%d] memset fail ", i, 0U, 0U, 0U, 0U);
            }
        }

        total_entry = perf_size/((uint32_t)sizeof(cvlog_perf_flexidag_entry_t));
        i = perf_all_start_idx;
        while(i != perf_all_end_idx) {
            uint32_t adj_time_stamp;

            adj_time_stamp = CV_FlexidagPerfTimeAdj(perf_ptr[i].time_stamp);
            if(CV_FlexidagPerfCoreChk(perf_ptr[i].cvcore,cvcore) == 0U) {

            } else if ((adj_time_stamp == 0U) || (adj_time_stamp < perf_all_start) || (adj_time_stamp > perf_all_end)) {

            } else {
                if(event == FLEXIDAG_PERF_STOP) {
                    if((perf_all_entry_min == 0U) || (perf_all_entry_min > adj_time_stamp)) {
                        perf_all_entry_min = adj_time_stamp;
                    }

                    if(perf_all_entry_max < adj_time_stamp) {
                        perf_all_entry_max = adj_time_stamp;
                    }
                }
                if(auto_perf_ptr[i].sysflow_index == PERF_SYSFLOW_INDEX_AUTORUN) {
                    CV_FlexidagPerfDo(&autorun_result_perf, &autorun_calc_perf, (uint8_t)auto_perf_ptr[i].event, adj_time_stamp);
                } else {
                    if((perf_ptr[i].flexidag_slot_id < FLEXIDAG_MAX_SLOTS) && (perf_ptr[i].sysflow_index < INTERAL_FLOW_ID_ARRAY)) {
                        uint8_t slot_id = (uint8_t) perf_ptr[i].flexidag_slot_id;
                        uint8_t flow_id = (uint8_t) perf_ptr[i].sysflow_index;

                        CV_FlexidagPerfDo(&all_result_perf[slot_id][flow_id], &all_calc_perf[slot_id][flow_id], (uint8_t)perf_ptr[i].event, adj_time_stamp);
                    }
                }
            }
            i++;
            if(i >= total_entry) {
                i = 0U;
            }
        }
        CV_FlexidagPerfAllCombine((perf_all_entry_max - perf_all_entry_min), flexidag_perf_array, num_of_array, num_of_flexidag);
    }
}
#endif

/**
 *  @brief      This API is used to get the performance measure or the performance profiling. The profiling result is for the period between start and stop event. \n
 *              Get the execution time information in us for the CV hardware specified by cvcore.  \n
 *              The cvcore is cvcore_type_e defined in cvtask_api.h. For example, CVCORE_VP is used to profile VP usage for the FlexiDAG.  \n
 *              The event specifies the start or stop of the profile the FlexiDAG.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      event : [IN] Start or stop of performance measure.  \n
 *                                  FLEXIDAG_PERF_START         0U  \n
 *                                  FLEXIDAG_PERF_STOP          1U
 *  @param      cvcore : [IN] Cvcore to performance measure.
 *  @param      flow_id_perf_array : [OUT] The structure array of performance measure result of flow ID.
 *  @param      num_of_array : [IN] Array size of structure.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagPerf(AMBA_CV_FLEXIDAG_HANDLE_s *handle, uint32_t event, uint32_t cvcore, AMBA_CV_FLEXIDAG_PERF_s *flow_id_perf_array, uint32_t num_of_array)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t perf_end;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(handle->fd_handle == NULL) {
        retcode = ERR_INTF_FLEXIDAG_HANDLE_INVALID;
    } else if(CV_FlexidagCheckState(handle->fd_handle, FLEXIDAG_STATE_READY) != ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
    } else if(flow_id_perf_array == NULL) {
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else {
        if ((handle->set_log_msg == 1U) && (handle->log_msg.orc_log_entry == 0U)) {
            AmbaPrint_PrintUInt5("AmbaCV_FlexidagPerf : not assign orc_log_entry for FLEXILOG_ORC_PERF", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_INTF_FLEXIDAG_LOG_UNAVAILABLE;
        } else {
            if(event == FLEXIDAG_PERF_START) {
                handle->perf_start = cvtask_get_timestamp();
            } else if ( (event == FLEXIDAG_PERF_STOP) && (handle->perf_start != 0U)) {
                if(AmbaWrap_memset(flow_id_perf_array, 0, sizeof(AMBA_CV_FLEXIDAG_PERF_s)*num_of_array) != 0U) {
                    AmbaPrint_PrintUInt5("AmbaCV_FlexidagPerf() : flow_id_perf_array memset fail", 0U, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_FLEXIDAG_MEMSET_FAIL;
                }
                perf_end = cvtask_get_timestamp();
                if((uint32_t)schdr_flush_visorc_printf(1U) != ERRCODE_NONE) {
                    AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagPerf() : schdr_flush_visorc_printf fail ", 0U, 0U, 0U, 0U, 0U);
                }
                CV_FlexidagPerf(handle, cvcore, handle->perf_start, perf_end, flow_id_perf_array, num_of_array);
            } else {
                AmbaPrint_PrintUInt5("AmbaCV_FlexidagPerf : error, not support event ", event, 0U, 0U, 0U, 0U);
                retcode = ERR_INTF_FLEXIDAG_PERF_EVENT_UNKNOW;
            }
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagPerf() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

/**
 *  @brief      Performance measure for all FlexiDAGs in one process.    \n
 *              Get the execution time information in us for the CV hardware specified by cvcore.  \n
 *              The cvcore is cvcore_type_e defined in cvtask_api.h. For example, CVCORE_VP is used to profile VP usage for the FlexiDAG.  \n
 *              The event specified the start or stop to profile the FlexiDAG. \n
 *
 *  @param      event : [IN] Start or stop of performance measure.  \n
 *                                  FLEXIDAG_PERF_START         0U          \n
 *                                  FLEXIDAG_PERF_STOP          1U
 *  @param      cvcore : [IN] Cvcore to performance measure.
 *  @param      flexidag_perf_array : [OUT] the structure array of performance measure result of FlexiDAG.
 *  @param      num_of_array : [IN] Array size of structure.
 *  @param      num_of_flexidag : [OUT] Number of FlexiDAG profiled.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagPerfAll(uint32_t event, uint32_t cvcore, AMBA_CV_FLEXIDAG_PERF_s *flexidag_perf_array, uint32_t num_of_array, uint32_t *num_of_flexidag)
{
    uint32_t retcode = ERRCODE_NONE;

#if !defined(ENV_IS_PACE_LINUX)
    if (CV_SchedCheckDrvState(FLEXIDAG_SCHDR_DRV_ON) != ERRCODE_NONE) {
        retcode = ERR_INTF_SCHDR_STATE_MISMATCHED;
    } else {
        if (event == FLEXIDAG_PERF_START) {
            retcode = schdr_get_perf_last_wridx_daddr(0U,&perf_all_start_idx);
            perf_all_start = cvtask_get_timestamp();
            perf_all_end = 0U;
        } else {
            *num_of_flexidag = 0U;
            if(AmbaWrap_memset(flexidag_perf_array, 0, sizeof(AMBA_CV_FLEXIDAG_PERF_s)*num_of_array) != 0U) {
                AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagPerfAll() : flexidag_perf_array memset fail ", 0U, 0U, 0U, 0U, 0U);
            }
            if((uint32_t)schdr_flush_visorc_printf(1U) != ERRCODE_NONE) {
                AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagPerfAll() : schdr_flush_visorc_printf fail ", 0U, 0U, 0U, 0U, 0U);
            }

            if ((event == FLEXIDAG_PERF_STOP) || (perf_all_end == 0U)) {
                perf_all_end = cvtask_get_timestamp();
                retcode = schdr_get_perf_last_wridx_daddr(0U,&perf_all_end_idx);
                perf_all_entry_min = 0U;
                perf_all_entry_max = 0U;
                //AmbaPrint_PrintUInt5("[INFO] perf scan range (%d - %d) time(%d - %d)", perf_all_start_idx, perf_all_end_idx, perf_all_start, perf_all_end, 0U);
            }
            if(mutex_lock(&global_mutex) != 0U) {
                AmbaPrint_PrintUInt5("[ERROR] CV_FlexidagPerfAll() : mutex_lock fail ", 0U, 0U, 0U, 0U, 0U);
            } else {
                CV_FlexidagPerfAll(event, cvcore,flexidag_perf_array, num_of_array, num_of_flexidag);
                if(mutex_unlock(&global_mutex) != 0U) {
                    AmbaPrint_PrintUInt5("[ERROR] CV_FlexidagPerfAll() : mutex_unlock fail ", 0U, 0U, 0U, 0U, 0U);
                }
            }
        }
    }
#else
    (void)event;
    (void)cvcore;
    (void)flexidag_perf_array;
    (void)num_of_array;
    (void)num_of_flexidag;
    AmbaPrint_PrintUInt5("[WARNING] AmbaCV_FlexidagPerfAll() : not support", 0U, 0U, 0U, 0U, 0U);
#endif

    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Get the system flow table index of the CVTask corresponding to the UUID.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      uuid : [IN] UUID in the system flow table.
 *  @param      flow_id : [OUT] Flow ID of the system flow table.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagGetFlowIdByUUID(const AMBA_CV_FLEXIDAG_HANDLE_s *handle, uint32_t flow_uuid, uint16_t *flow_id)
{
    uint32_t retcode = ERRCODE_NONE;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(handle->fd_handle == NULL) {
        retcode = ERR_INTF_FLEXIDAG_HANDLE_INVALID;
    } else if(CV_FlexidagCheckState(handle->fd_handle, FLEXIDAG_STATE_READY) != ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
    } else if(flow_id == NULL) {
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else {
        retcode = flexidag_get_sysflow_index_by_UUID(handle->fd_handle, flow_uuid, flow_id);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagGetFlowIdByUUID() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

/**
 *  @brief      Get system flow table index of the CVTask corresponding to the name. There could be multiple CVTasks matching the name.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      name : [IN] Name in system flow table.
 *  @param      num_of_array : [IN] Number of array of flow ID array. (The upper bond of num_of array is set by FLEXIDAG_MAX_SFB_ENTRIES).
 *  @param      flow_id_array : [OUT] Flow ID array.
 *  @param      num_found : [OUT] Number of Flow ID existing in flow_id_array.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagGetFlowIdByName(const AMBA_CV_FLEXIDAG_HANDLE_s *handle, AMBA_CV_FLEXIDAG_NAME_s name, uint32_t num_of_array, uint16_t *flow_id_array, uint32_t *num_found)
{
    uint32_t retcode = ERRCODE_NONE;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(handle->fd_handle == NULL) {
        retcode = ERR_INTF_FLEXIDAG_HANDLE_INVALID;
    } else if(CV_FlexidagCheckState(handle->fd_handle, FLEXIDAG_STATE_READY) != ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
    } else if( (flow_id_array == NULL) || (num_found == NULL) ) {
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else {
        retcode = flexidag_get_sysflow_index_list(handle->fd_handle, name.instance, name.algorithm, name.step, name.cvtask, flow_id_array, num_of_array, num_found);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagGetFlowIdByName() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

/**
 *  @brief      Send message to CVTask for the opened FlexiDAG.   \n
 *              Flow ID is ID for certain CVTasks and can get by name or UUID in *.csv which is needed when compiling FlexiDAG bin.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      msg : [IN] Message to send to.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSendMsg(const AMBA_CV_FLEXIDAG_HANDLE_s *handle,const AMBA_CV_FLEXIDAG_MSG_s *msg)
{
    uint32_t retcode = ERRCODE_NONE;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(handle->fd_handle == NULL) {
        retcode = ERR_INTF_FLEXIDAG_HANDLE_INVALID;
    } else if(CV_FlexidagCheckState(handle->fd_handle, FLEXIDAG_STATE_READY) != ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
    } else if( (msg == NULL) || (msg->vpMessage == NULL) ) {
        retcode = ERR_INTF_FLEXIDAG_MSG_INVALID;
    } else {
        if ( (handle->set_log_msg == 1U) && (handle->log_msg.flexidag_msg_entry == 0U)) {
            AmbaPrint_PrintUInt5("AmbaCV_FlexidagSendMsg : not assign flexidag_msg_entry", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_INTF_FLEXIDAG_MSG_UNAVAILABLE;
        } else {
            retcode = flexidag_send_message(handle->fd_handle, msg->flow_id, msg->vpMessage, msg->length);
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagSendMsg() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

/**
 *  @brief      Send private message to CVTask.   \n
 *              Flow ID is ID for certain CVTasks and can get by name or UUID in *.csv which is needed when compiling FlexiDAG bin.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      msg : [IN] Message to send to.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSendPrivateMsg(const AMBA_CV_FLEXIDAG_HANDLE_s *handle,const AMBA_CV_FLEXIDAG_MSG_s *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const uint8_t *pMessage = NULL;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(handle->fd_handle == NULL) {
        retcode = ERR_INTF_FLEXIDAG_HANDLE_INVALID;
    } else if(CV_FlexidagCheckState(handle->fd_handle, FLEXIDAG_STATE_READY) != ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
    } else if( (msg == NULL) || (msg->vpMessage == NULL) ) {
        retcode = ERR_INTF_FLEXIDAG_MSG_INVALID;
    } else {
        if ( (handle->set_log_msg == 1U) && (handle->log_msg.flexidag_msg_entry == 0U)) {
            AmbaPrint_PrintUInt5("AmbaCV_FlexidagSendPrivateMsg : not assign flexidag_msg_entry", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_INTF_FLEXIDAG_MSG_UNAVAILABLE;
        } else {
            AmbaMisra_TypeCast(&pMessage, &msg->vpMessage);
            retcode = flexidag_send_private_msg(handle->fd_handle, msg->flow_id, pMessage);
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagSendPrivateMsg() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

/**
 *  @brief      Set a set of parameters of FlexiDAG. The parameters are named as FLEXIDAG_PARAMSET_XXX and listed in cvapi_ambacv_flexidag.h.    \n
 *              It is recommend to call AmbaCV_FlexidagSetParamSet before AmbaCV_FlexidagInit.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      id : [IN] The ID of the parameter set.  \n
 *                                   FLEXIDAG_PARAMSET_LOG_MSG   0U
 *  @param      param_set : [IN] The pointer of the param_set structure.
 *  @param      param_size : [IN] The size of the param_set structure.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSetParamSet(AMBA_CV_FLEXIDAG_HANDLE_s *handle, uint32_t id, const void *param_set, uint32_t param_size)
{
    uint32_t retcode = ERRCODE_NONE;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(CV_FlexidagCheckState(handle->fd_handle, FLEXIDAG_STATE_UNUSED) != ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
    } else if( param_set == NULL ) {
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else {
        if (handle->fd_handle == NULL) {
            retcode = flexidag_create(&handle->fd_handle);
        }

        if( retcode == ERRCODE_NONE ) {
            if(id == FLEXIDAG_PARAMSET_LOG_MSG) {
                if(param_size != sizeof(AMBA_CV_FLEXIDAG_LOG_MSG_s)) {
                    retcode = ERR_INTF_FLEXIDAG_PARAM_SIZE_MISMATCHED;
                } else {
                    const AMBA_CV_FLEXIDAG_LOG_MSG_s *log_msg;
                    AmbaMisra_TypeCast(&log_msg, &param_set);
                    retcode = CV_ParamSetLogMsg(handle, log_msg);
                    if( retcode == ERRCODE_NONE ) {
                        handle->set_log_msg = 1U;
                        if(AmbaWrap_memcpy(&handle->log_msg, param_set, sizeof(AMBA_CV_FLEXIDAG_LOG_MSG_s)) != 0U) {
                            AmbaPrint_PrintUInt5("AmbaCV_FlexidagSetParamSet() : handle->log_msg memcpy fail", 0U, 0U, 0U, 0U, 0U);
                            retcode = ERR_DRV_FLEXIDAG_MEMCPY_FAIL;
                        }
                    }
                }
            } else if (id == FLEXIDAG_PARAMSET_TIMEOUT) {
                if(param_size != sizeof(AMBA_CV_FLEXIDAG_TIMEOUT_s)) {
                    retcode = ERR_INTF_FLEXIDAG_PARAM_SIZE_MISMATCHED;
                } else {
                    const AMBA_CV_FLEXIDAG_TIMEOUT_s *timeout;
                    AmbaMisra_TypeCast(&timeout, &param_set);
                    retcode = flexidag_set_parameter(handle->fd_handle, FDPARAM_TOKEN_TIMEOUT_ID, timeout->run_timeout);
                }
            } else {
                retcode = ERR_INTF_FLEXIDAG_PARAM_ID_UNKNOW;
            }
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagSetParamSet() : error id 0x%x , ret = 0x%x", id, retcode, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

/**
 *  @brief      Set debug level to CVTask for the opened FlexiDAG.   \n
 *              Flow ID is ID for certain CVTasks and can get by name or UUID in *.csv which is needed when compiling FlexiDAG bin.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      debug : [IN] Debug level to set to.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSetDebugLevel(const AMBA_CV_FLEXIDAG_HANDLE_s *handle,const AMBA_CV_FLEXIDAG_DEBUG_CFG_s *debug)
{
    uint32_t retcode = ERRCODE_NONE;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(handle->fd_handle == NULL) {
        retcode = ERR_INTF_FLEXIDAG_HANDLE_INVALID;
    } else if(CV_FlexidagCheckState(handle->fd_handle, FLEXIDAG_STATE_UNUSED) == ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
    } else if(debug == NULL) {
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else {
        retcode = flexidag_set_debug_level(handle->fd_handle, debug->flow_id, debug->log_level);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagSetDebugLevel() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

/**
 *  @brief      Find cvtable data in *.tbar which is needed when compiling the FlexiDAG bin. \n
 *              Example: AmbaCV_FlexidagFindCvtable(handle, "FEEDER_IONAME_000", &cvtable_data, &cvtable_size);    \n
 *              The result of cvtable_data is "BISENET_MNV2_INPUT_0" xxxx.mnft.in "FEEDER_IONAME_000"   string    "BISENET_MNV2_INPUT_0";.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      name : [IN] Name of cvtable.
 *  @param      data : [OUT] Data of cvtable.
 *  @param      size : [OUT] Size of data.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagFindCvtable(const AMBA_CV_FLEXIDAG_HANDLE_s *handle, const char *name, const void **data, uint32_t *size)
{
    uint32_t retcode = ERRCODE_NONE;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(handle->fd_handle == NULL) {
        retcode = ERR_INTF_FLEXIDAG_HANDLE_INVALID;
    } else if(CV_FlexidagCheckState(handle->fd_handle, FLEXIDAG_STATE_READY) != ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
    } else if( (name == NULL) || (data == NULL) || (size == NULL)) {
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else {
        retcode = flexidag_cvtable_find(handle->fd_handle, name, data, size);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagFindCvtable() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

/**
 *  @brief      Call back to handle the error from the ORC scheduler.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      cb : [IN] Call back.
 *  @param      cb_param : [OUT] Parameter of call back.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSetErrorCb(const AMBA_CV_FLEXIDAG_HANDLE_s *handle, flexidag_error_cb cb, void* cb_param)
{
    uint32_t retcode = ERRCODE_NONE;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(handle->fd_handle == NULL) {
        retcode = ERR_INTF_FLEXIDAG_HANDLE_INVALID;
    } else if(CV_FlexidagCheckState(handle->fd_handle, FLEXIDAG_STATE_UNUSED) == ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
    } else if(cb == NULL) {
        retcode = ERR_INTF_FLEXIDAG_ERROR_CB_INVALID;
    } else {
        retcode = flexidag_set_error_callback(handle->fd_handle, cb, cb_param);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagSetErrorCb() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

/**
 *  @brief      Close the flexiDAG and free the resources.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagClose(AMBA_CV_FLEXIDAG_HANDLE_s *handle)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t start_time, end_time;
    uint32_t slot_id;
    uint32_t timestamp_slot;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(handle->fd_handle == NULL) {
        retcode = ERR_INTF_FLEXIDAG_HANDLE_INVALID;
    } else {
        start_time = CV_FlexidagTimestamp();
        retcode = errmgr_get_timestamp_slot(&timestamp_slot);
        if (retcode == ERRCODE_NONE) {
            retcode = errmgr_ipc_send_timestamp_start(CV_MODULE_ID_DRIVER, timestamp_slot, ERR_DRV_SCHDR_ERRMGR_FLEXIDAG_CLOSE_TIMEOUT, 5000U);
            if (retcode == ERRCODE_NONE) {
                if(CV_FlexidagSetState(handle->fd_handle, FLEXIDAG_STATE_UNUSED) != 0U) {
                    AmbaPrint_PrintUInt5("AmbaCV_FlexidagClose() : CV_FlexidagSetState fail", 0U, 0U, 0U, 0U, 0U);
                }
                slot_id = flexidag_find_slot_by_vphandle(handle->fd_handle);
                if(slot_id < FLEXIDAG_MAX_SLOTS) {
                    AmbaCV_Handle_Init[slot_id] = 0U;
                    if(mutex_delete(&api_run_mutex[slot_id]) != 0U) {
                        AmbaPrint_PrintUInt5("AmbaCV_FlexidagClose() : mutex_delete fail", 0U, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("AmbaCV_FlexidagClose() : slot_id(%d) is unavailable", slot_id, 0U, 0U, 0U, 0U);
                }
                retcode = flexidag_close(handle->fd_handle);
                handle->fd_handle = NULL;
                if (retcode == ERRCODE_NONE) {
                    retcode = errmgr_ipc_send_timestamp_stop(CV_MODULE_ID_DRIVER, timestamp_slot, ERR_DRV_SCHDR_ERRMGR_FLEXIDAG_CLOSE_TIMEOUT);
                }
            }
        }
        end_time = CV_FlexidagTimestamp();
        AmbaPrint_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID,"AmbaCV_FlexidagClose : run_time = %d us", end_time - start_time, 0, 0, 0, 0);
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagClose() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return CV_GetExternalErrorCode(handle, retcode);
}

/**
 *  @brief      Get internal error number when API return CV_ERR_0003.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      retcode : [OUT] Internal error code.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagGetInternalError(AMBA_CV_FLEXIDAG_HANDLE_s *handle, uint32_t *retcode)
{
    uint32_t ret = ERRCODE_NONE;
    uint32_t slot_id;

    if(handle == NULL) {
        *retcode = inter_schdr_error;
    } else {
        AmbaMisra_TouchUnused(handle);
        if(handle->fd_handle == NULL) {
            ret = ERR_INTF_FLEXIDAG_HANDLE_INVALID;
        } else if(CV_FlexidagCheckState(handle->fd_handle, FLEXIDAG_STATE_UNUSED) == ERRCODE_NONE) {
            ret = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
        } else {
            slot_id = flexidag_find_slot_by_vphandle(handle->fd_handle);
            if(slot_id < FLEXIDAG_MAX_SLOTS) {
                *retcode = inter_flexidag_error[slot_id];
            } else {
                ret = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
            }
        }
    }

    if( ret != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagGetInternalError() : error ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        ret = CV_ERR_0000;
    }
    return ret;
}

/**
 *  @brief      Get AmbaCV API handle from internal fd_handle.
 *
 *  @param      fd_handle : [IN] The fd_handle in AMBA_CV_FLEXIDAG_HANDLE_s.
 *  @param      handle : [OUT] The handle of the FlexiDAG object.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagFdHandleToHandle(void *fd_handle, AMBA_CV_FLEXIDAG_HANDLE_s **handle)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t slot_id;

    if(handle == NULL) {
        retcode = ERR_INTF_AMBACV_FLEXIDAG_HANDLE_INVALID;
    } else if(fd_handle == NULL) {
        retcode = ERR_INTF_FLEXIDAG_HANDLE_INVALID;
    } else if(CV_FlexidagCheckState(fd_handle, FLEXIDAG_STATE_UNUSED) == ERRCODE_NONE) {
        retcode = ERR_INTF_FLEXIDAG_SLOT_STATE_MISMATCHED;
    } else {
        slot_id = flexidag_find_slot_by_vphandle(fd_handle);
        if(slot_id < FLEXIDAG_MAX_SLOTS) {
            *handle = AmbaCV_Handle[slot_id];
        } else {
            retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
        }
    }

    if( retcode != ERRCODE_NONE ) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_FlexidagFdHandleToHandle() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = CV_ERR_0000;
    }
    return retcode;
}

/**
 *  @brief      Find metadata from FlexiDAG bin through metadata name.
 *
 *  @param      pFlexiBin : [IN] FlexiDAG bin.
 *  @param      pMetadataName : [IN] Name of metadata.
 *  @param      vppBuffer : [OUT] address of metadata in FlexiDAG bin.
 *  @param      pSize : [OUT] size of metadata in FlexiDAG bin.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagFindMetadata(void *pFlexiBin, const char *pMetadataName, const void **vppBuffer, uint32_t *pSize)
{
    uint32_t retcode = ERRCODE_NONE;

    retcode = flexibin_metadata_find(pFlexiBin, pMetadataName, vppBuffer, pSize);
    if( retcode == ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID) {
        retcode = ERR_INTF_INPUT_PARAM_INVALID;
    } else if( retcode == ERR_DRV_FLEXIDAG_METADATA_UNABLE_TO_FIND) {
        retcode = ERR_INTF_FLEXIDAG_METADATA_NAME_MISMATCHED;
    } else {
        retcode = ERRCODE_NONE;
    }

    return CV_GetExternalErrorCode(NULL, retcode);
}

/**
 *  @brief      Initialize the AmbaCV IPC service.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_IpcInit(void)
{
    uint32_t retcode;

    retcode = errmgr_init();
    if(retcode != ERRCODE_NONE) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaCV_IpcInit() : errmgr_init error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }

    return CV_GetExternalErrorCode(NULL, retcode);
}

