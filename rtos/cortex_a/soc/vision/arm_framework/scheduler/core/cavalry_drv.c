/**
 *  @file cavalry_drv.c
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
 *  @details Cavalry driver APIs
 *
 */

#include "dram_mmap.h"
#include "cavalry_ioctl.h"
#include "schdrmsg_def.h"
#include "os_api.h"
#include "ambacv_kal.h"
#include "idsp_vis_msg.h"
#include "schdr.h"
#include "cavalry.h"
#include "schdr_api.h"

#ifndef DISABLE_CAVALRY
int32_t cavalry_init(uint32_t version, uint32_t num_slots)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if (version != CAVALRY_VERSION) {
        retcode = ERR_DRV_SCHDR_CAVALRY_VERSION_MISMATCHED;
        console_printU5("[ERROR] cavalry_init() : Cavalry system cannot initialize : mismatched versions (requested %d, built %d)\n", version, CAVALRY_VERSION, 0U, 0U, 0U);
    } /* if (version != CAVALRY_VERSION) */
    else { /* if (version == CAVALRY_VERSION) */
        ret = drv_cavalry_enable(num_slots);
        if (ret >= 0) {
            if (num_slots > 0U) {
                module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "cavalry_init() : Cavalry system enabled with %d slots (ret=%d)\n", num_slots, (uint32_t)retcode, 0U, 0U, 0U);
            } /* if (num_slots > 0) */
            schdr_sys_state.visorc_init.cavalry_num_slots   = num_slots;
        } /* if (retcode >= 0) */
        else { /* if (retcode < 0) */
            console_printU5("[ERROR] cavalry_init() : Cavalry system cannot initialize : error from cavalry driver (ret=%d)\n", (uint32_t)ret, 0U, 0U, 0U, 0U);
        } /* if (retcode < 0) */
    } /* if (version == CAVALRY_VERSION) */

    if(retcode == ERRCODE_NONE) {
        schdr_sys_state.module_init = schdr_sys_state.module_init | CAVALRY_MODULE;
    }
    return (int32_t)retcode;

} /* cavalry_init() */

uint32_t cavalry_resume(uint32_t version)
{
    uint32_t retcode = ERRCODE_NONE;

    if (version != CAVALRY_VERSION) {
        retcode = ERR_DRV_SCHDR_CAVALRY_VERSION_MISMATCHED;
        console_printU5("[ERROR] cavalry_resume() : Cavalry system cannot initialize : mismatched versions (requested %d, built %d)\n", version, CAVALRY_VERSION, 0U, 0U, 0U);
    } /* if (version != CAVALRY_VERSION) */
    else { /* if (version == CAVALRY_VERSION) */
        schdr_sys_state.module_init = schdr_sys_state.module_init | CAVALRY_MODULE;
    }

    return retcode;

} /* cavalry_init() */

#if defined(AMBACV_KERNEL_SUPPORT) || defined(RTOS_BUILD)

/*------------------------------------------------------------------------------------------------*/
/*-= Cavalry related driver interface =-----------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
int32_t drv_cavalry_query_buf(void *pQueryBuf)
{
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_QUERY_BUF, pQueryBuf);
} /* drv_cavalry_query_buf() */

int32_t drv_cavalry_start_vp(void *vpUserData)
{
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_START_VP, vpUserData);
} /* drv_cavalry_start_vp() */

int32_t drv_cavalry_stop_vp(void *vpUserData)
{
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_STOP_VP, vpUserData);
} /* drv_cavalry_stop_vp() */

int32_t drv_cavalry_run_dags(void *vpParams)
{
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_RUN_DAGS, vpParams);
} /* drv_cavalry_start_log() */

int32_t drv_cavalry_start_log(void *vpUserData)
{
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_START_LOG, vpUserData);
} /* drv_cavalry_start_log() */

int32_t drv_cavalry_stop_log(void *vpUserData)
{
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_STOP_LOG, vpUserData);
} /* drv_cavalry_stop_log() */

int32_t drv_cavalry_early_quit(void *pEarlyQuit)
{
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_EARLY_QUIT, pEarlyQuit);
} /* drv_cavalry_early_quit() */

int32_t drv_cavalry_alloc_mem(void *pCavalryMem)
{
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_ALLOC_MEM, pCavalryMem);
} /* drv_cavalry_alloc_mem() */

int32_t drv_cavalry_free_mem(void *pCavalryMem)
{
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_FREE_MEM, pCavalryMem);
} /* drv_cavalry_free_mem() */

int32_t drv_cavalry_sync_cache_mem(void *pCavalryCacheMem)
{
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_SYNC_CACHE_MEM, pCavalryCacheMem);
} /* drv_cavalry_sync_cache_mem() */

int32_t drv_cavalry_get_usage_mem(void *pCavalryUsageMem)
{
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_GET_USAGE_MEM, pCavalryUsageMem);
} /* drv_cavalry_get_usage_mem() */

int32_t drv_cavalry_run_hotlink_slot(void *pCavalryHotlinkSlot)
{
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_RUN_HOTLINK_SLOT, pCavalryHotlinkSlot);
} /* drv_cavalry_get_usage_mem() */

int32_t drv_cavalry_set_hotlink_slot_cfg(void *pCavalrySlotCfg)
{
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_SET_HOTLINK_SLOT_CFG, pCavalrySlotCfg);
} /* drv_cavalry_set_hotlink_slot_cfg() */

int32_t drv_cavalry_get_hotlink_slot_cfg(void *pCavalrySlotCfg)
{
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_GET_HOTLINK_SLOT_CFG, pCavalrySlotCfg);
} /* drv_cavalry_get_hotlink_slot_cfg() */

int32_t drv_cavalry_enable(uint32_t num_slots)
{
    uint64_t value = 0UL;
    void *ptr;

    value = num_slots;
    typecast(&ptr,&value);
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_ENABLE, ptr);
} /* drv_cavalry_enable() */

int32_t drv_cavalry_associate_buf(uint64_t cavalry_base_daddr)
{
    uint64_t value = 0UL;
    void *ptr;

    value = cavalry_base_daddr;
    typecast(&ptr,&value);
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_ASSOCIATE_BUF, ptr);
} /* drv_cavalry_associate_buf() */

int32_t drv_cavalry_set_memory_block(armvis_msg_t *pMsg)
{
    void *ptr;

    (void) pMsg;
    typecast(&ptr,&pMsg);
    return (int32_t)ambacv_ioctl(ambacv_fd, CAVALRY_SET_MEMORY_BLOCK, ptr);
} /* drv_cavalry_set_memory_block() */

int32_t drv_cavalry_process_reply(const armvis_msg_t *pMsg)
{
    (void) pMsg;
    return 0; /* This has been moved into the kernel */
} /* drv_cavalry_process_reply() */

#endif
#endif
