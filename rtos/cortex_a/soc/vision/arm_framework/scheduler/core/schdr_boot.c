/**
 *  @file schdr_api.c
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
 *  @details Scheduler APIs
 *
 */

#include "os_api.h"
#include "schdrmsg_def.h"
#include "schdr_api.h"
#include "dram_mmap.h"
#include "schdr.h"
#include "schdr_internal.h"
#include "msg.h"
#include "flexidag.h"
#include "cavalry.h"
#include "thpool.h"
#include "cvtask_errno.h"
#include "cvapi_logger_interface.h"
#include "ambacv_kal.h"
#if defined(ENABLE_AMBA_MAL)
#include "AmbaMAL.h"
#endif

/*      system cfg    */
schdr_system_cfg_t schdr_sys_cfg = {
    .cpus_map = 0xDU,
    .flags = 0x0U,
    .boot_cfg = 0x1U,
    .cavalry_num_slots = 0x0U,
    .flexidag_num_slots = 0x0U,
    .verbosity = LVL_CRITICAL,
    .max_sp_size = SCHDR_MAX_SCRATCHPAD_SIZE,
    .internal_thread = 1U,
    .autorun_interval = 0U,
    .autorun_fex_cfg = 0U,
    .log_entries_visschdr = SCHDR_LOG_ENTRIES_VISSCHDR,
    .log_entries_viscvtask = SCHDR_LOG_ENTRIES_VISCVTASK,
    .log_entries_visperf = SCHDR_LOG_ENTRIES_VISPERF
};

/*      system state    */
schdr_system_state_t schdr_sys_state = {0};

int32_t ambacv_fd = -1;
psem_t *pStartupSemaphore = NULL;

#if defined(AMBACV_KERNEL_SUPPORT) || defined(RTOS_BUILD)
uint32_t schdr_get_cluster_id(void)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t cid = 0U;
    ambacv_get_value_t arg= {0};

    if (ambacv_fd < 0) {
        ambacv_fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
        if (ambacv_fd < 0) {
            console_printU5("[ERROR] schdr_get_cluster_id() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
        } /* if (ambacv_fd < 0) : open("/dev/ambacv", ...) */
    } /* if (ambacv_fd == 0) */

    if(retcode == ERRCODE_NONE) {
        arg.index = AMBACV_ID_GET_CLUSTER_ID;
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_GET_VALUE, &arg);
        if (retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_get_cluster_id() :  ambacv_ioctl fail ret = 0x%x\n", retcode, 0U, 0U, 0U, 0U);
        } /* if (retcode != ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
        else { /* if (retcode == ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
            cid = (uint32_t)arg.value;
        } /* if (retcode == ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
    }

    return cid;
}

static uint32_t drv_schdr_init_module(visorc_init_params_t *sysconfig)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t cfg_bk;
    uint32_t version = 0U,offset_h = 0U,offset_l = 0U;
    uint32_t cid = 0U;

    if(sysconfig == NULL) {
        console_printU5("[ERROR] drv_schdr_init_module() : sysconfig == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        cfg_bk = sysconfig->coprocessor_cfg;
        /* only open this file once */
        if (ambacv_fd < 0) {
            ambacv_fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
        } /* if (ambacv_fd == 0) */

        if (ambacv_fd < 0) {
            console_printU5("[ERROR] drv_schdr_init_module() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
        } /* if (ambacv_fd < 0) */

        if(retcode == ERRCODE_NONE) {
            retcode = ambacv_ioctl(ambacv_fd, AMBACV_GET_SYSCONFIG, sysconfig);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] drv_schdr_init_module() : AMBACV_GET_SYSCONFIG fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
            }

            if(retcode == ERRCODE_NONE) {
                /* mmap the armvis_msg_t region */
                retcode = schdr_mmap_init(ambacv_c2p((uint64_t)sysconfig->sysinit_dram_block_baseaddr + (uint64_t)sysconfig->sysinit_dram_block_size),
                                          sysconfig->coprocessor_cfg);
                if(retcode != ERRCODE_NONE) {
                    console_printU5("[ERROR] drv_schdr_init_module() : schdr_mmap_init fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
                }
            }

            if(retcode == ERRCODE_NONE) {
                retcode = schdr_check_version(ambacv_fd, &version);
                if(retcode != ERRCODE_NONE) {
                    console_printU5("[ERROR] drv_schdr_init_module() : schdr_check_version fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
                }
            }

            if(retcode == ERRCODE_NONE) {
                offset_h = (version >> 8U);
                offset_l = (version & 0xFFU);
                module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"Ambacv kernel module version: %d.%d\n", offset_h, offset_l, 0U, 0U, 0U);

                cid = schdr_get_cluster_id();
                /* set hosting cluster */
                module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"Host Cluster ID = %d\n", cid, 0U, 0U, 0U, 0U);
                retcode = cvtask_set_cluster_id(cid);
                if(retcode != ERRCODE_NONE) {
                    console_printU5("[ERROR] drv_schdr_init_module() : cvtask_set_cluster_id fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
                }

                sysconfig->coprocessor_cfg = cfg_bk;
            }
        }
    }
    return retcode;
}

static uint32_t drv_schdr_resume_module(visorc_init_params_t *sysconfig)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t version = 0U,offset_h = 0U,offset_l = 0U;
    uint32_t cid = 0U;

    if(sysconfig == NULL) {
        console_printU5("[ERROR] drv_schdr_resume_module() : sysconfig == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_RESUME_VISORC, sysconfig);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] drv_schdr_resume_module() : AMBACV_RESUME_VISORC fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
        } else {
            if ((sysconfig->coprocessor_cfg & SCHDR_NO_INTERNAL_THREAD) != 0U) {
                schdr_sys_cfg.internal_thread = 0U;
                console_printU5("schdr use external cmd msg thread !\n", 0U, 0U, 0U, 0U, 0U);
            } else {
                console_printU5("schdr use internal cmd msg thread !\n", 0U, 0U, 0U, 0U, 0U);
            }
            schdr_sys_cfg.cavalry_num_slots = schdr_sys_state.visorc_init.cavalry_num_slots;
            schdr_sys_cfg.flexidag_num_slots = schdr_sys_state.visorc_init.flexidag_num_slots;
            schdr_sys_cfg.flags = (schdr_sys_cfg.flags | SCHDR_IS_SECONDARY_SCHEDULER | SCHDR_DISABLE_SS_LOG); /* Explicitly set additional flags, including SECONDARY_SCHEDULER */
        }

        if(retcode == ERRCODE_NONE) {
            retcode = schdr_check_version(ambacv_fd, &version);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] drv_schdr_resume_module() : schdr_check_version fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            offset_h = (version >> 8U);
            offset_l = (version & 0xFFU);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"Ambacv kernel module version: %d.%d\n", offset_h, offset_l, 0U, 0U, 0U);

            cid = schdr_get_cluster_id();
            /* set hosting cluster */
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"Host Cluster ID = %d\n", cid, 0U, 0U, 0U, 0U);
            retcode = cvtask_set_cluster_id(cid);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] drv_schdr_resume_module() : cvtask_set_cluster_id fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
            }
        }
    }

    return retcode;
}

static uint32_t drv_schdr_start_visorc(visorc_init_params_t *sysconfig)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t index;
    uint64_t start,end;
    const armvis_message_queue_t *conf;
    const void *ptr;

    if(sysconfig == NULL) {
        console_printU5("[ERROR] drv_schdr_start_visorc() : sysconfig == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        if (sysconfig->cavalry_num_slots != 0U) {
            sysconfig->sysinit_dram_block_size  = sysconfig->sysinit_dram_block_size - (sysconfig->cavalry_num_slots * CAVALRY_SLOT_SIZE);
            sysconfig->cavalry_base_daddr       = sysconfig->sysinit_dram_block_baseaddr + sysconfig->sysinit_dram_block_size;
        } /* if (sysconfig->cavalry_num_slots != 0) */
        else { /* if (sysconfig->cavalry_num_slots == 0) */
            sysconfig->cavalry_base_daddr = 0U;
        } /* if (sysconfig->cavalry_num_slots == 0) */

        if (sysconfig->hotlink_region_size != 0U) {
            sysconfig->sysinit_dram_block_size -= (sysconfig->hotlink_region_size);
            sysconfig->hotlink_region_daddr     = sysconfig->sysinit_dram_block_baseaddr + sysconfig->sysinit_dram_block_size;
        } /* if (sysconfig->hotlink_region_size != 0) */
        else { /* if (sysconfig->hotlink_region_size == 0) */
            sysconfig->hotlink_region_daddr = 0U;
        } /* if (sysconfig->hotlink_region_size == 0) */

        retcode = ambacv_ioctl(ambacv_fd, AMBACV_START_VISORC, sysconfig);

        if (schdr_sys_cfg.verbosity >= LVL_DEBUG) {
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"=========ORC INIT DATA===========\n", 0U, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"    num_copro : %d\n", sysconfig->coprocessor_cfg, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"    sod_cvtask: %X\n", sysconfig->debug_buffer_sod_cvtask_size, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"    sod_sched : %X\n", sysconfig->debug_buffer_sod_sched_size, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"    sod_perf  : %X\n", sysconfig->debug_buffer_sod_perf_size, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"     vp_cvtask: %X\n", sysconfig->debug_buffer_vp_cvtask_size, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"     vp_sched : %X\n", sysconfig->debug_buffer_vp_sched_size, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"     vp_perf  : %X\n", sysconfig->debug_buffer_vp_perf_size, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID," cavalry_slots: %d\n", sysconfig->cavalry_num_slots, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"flexidag_slots: %d\n", sysconfig->flexidag_num_slots, 0U, 0U, 0U, 0U);
            // Propagate autorun values
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"autorun_intval: %d\n", sysconfig->autorun_interval_in_ms, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"autorun_fexcfg: %X\n", sysconfig->autorun_fextest_config, 0U, 0U, 0U, 0U);

            if (sysconfig->sysinit_dram_block_baseaddr != 0U) {
                start = ambacv_c2p(sysconfig->sysinit_dram_block_baseaddr);
                end = ambacv_c2p(sysconfig->sysinit_dram_block_baseaddr) + sysconfig->sysinit_dram_block_size - 1U;
                module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"    dram area : [0x%llX -- 0x%llX]\n", start, end, 0U, 0U, 0U);
            } /* if (sysconfig->dram_addr != 0) */

            if (sysconfig->hotlink_region_daddr != 0U) {
                start = ambacv_c2p(sysconfig->hotlink_region_daddr);
                end = ambacv_c2p(sysconfig->hotlink_region_daddr) + sysconfig->hotlink_region_size - 1U;
                module_printU5(AMBA_SCHDR_PRINT_MODULE_ID," hotlink area : [0x%llX -- 0x%llX]\n", start, end, 0U, 0U, 0U);
            } /* if (sysconfig->hotlink_region_daddr != 0) */

            if (sysconfig->cavalry_base_daddr != 0U) {
                start = ambacv_c2p(sysconfig->cavalry_base_daddr);
                end = ambacv_c2p(sysconfig->cavalry_base_daddr) + ((uint64_t)sysconfig->cavalry_num_slots * CAVALRY_SLOT_SIZE) - 1U;
                module_printU5(AMBA_SCHDR_PRINT_MODULE_ID," Cavalry area : [0x%llX -- 0x%llX]\n", start, end, 0U, 0U, 0U);
            } /* if (sysconfig->cavalry_base_daddr != 0) */

            if (sysconfig->kernel_region_base_daddr != 0U) {
                start = ambacv_c2p(sysconfig->kernel_region_base_daddr);
                end = ambacv_c2p(sysconfig->kernel_region_base_daddr) + sysconfig->kernel_region_size - 1U;
                module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"kernel region : [0x%llX -- 0x%llX]\n", start, end, 0U, 0U, 0U);
            } /* if (sysconfig->kernel_region_base_daddr != 0) */

            if (sysconfig->cma_region_daddr != 0U) {
                start = ambacv_c2p(sysconfig->cma_region_daddr);
                end = ambacv_c2p(sysconfig->cma_region_daddr) + sysconfig->cma_region_size - 1U;
                module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"   CMA region : [0x%llX -- 0x%llX]\n", start, end, 0U, 0U, 0U);
            } /* if (sysconfig->cma_region_daddr != 0) */

            if (sysconfig->rtos_region_daddr != 0U) {
                start = ambacv_c2p(sysconfig->rtos_region_daddr);
                end = ambacv_c2p(sysconfig->rtos_region_daddr) + sysconfig->rtos_region_size - 1U;
                module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"  RTOS region : [0x%llX -- 0x%llX]\n", start, end, 0U, 0U, 0U);
            } /* if (sysconfig->rtos_region_daddr != 0) */

            for (index = 0U; index < 4U; index++) {
                if (sysconfig->MsgQueue_orc_to_arm_daddr[index] != 0U) {
                    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"   --------[SSID:%d]-------\n", index, 0U, 0U, 0U, 0U);
                    ptr = ambacv_c2v(sysconfig->MsgQueue_orc_to_arm_daddr[index]);
                    if(ptr != NULL) {
                        typecast(&conf,&ptr);
                        module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"    max_count: %d\n",  conf->max_count, 0U, 0U, 0U, 0U);
                        module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"         addr: %llX\n",  ambacv_c2p(conf->msg_array_daddr), 0U, 0U, 0U, 0U);
                    }
                    ptr = ambacv_c2v(sysconfig->MsgQueue_arm_to_orc_daddr[index]);
                    if(ptr != NULL) {
                        ptr = ambacv_c2v(sysconfig->MsgQueue_arm_to_orc_daddr[index]);
                        typecast(&conf,&ptr);
                        module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"    max_count: %d\n",  conf->max_count, 0U, 0U, 0U, 0U);
                        module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"         addr: %llX\n",  ambacv_c2p(conf->msg_array_daddr), 0U, 0U, 0U, 0U);
                    }
                    break;
                }
            }
        }
    }
    return retcode;
}


static uint32_t drv_schdr_resume_visorc(const visorc_init_params_t *sysconfig)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t start,size,end;

    (void) sysconfig;
    if(sysconfig == NULL) {
        console_printU5("[ERROR] drv_schdr_resume_visorc() : sysconfig == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        /* mmap the armvis_msg_t region */
        start = ambacv_c2p(sysconfig->kernel_region_base_daddr);
        size = sysconfig->kernel_region_size;
        retcode = schdr_mmap_init(start, size);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] drv_schdr_resume_visorc() : schdr_mmap_init fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
        }

        if(retcode == ERRCODE_NONE) {
            start = ambacv_c2p(sysconfig->cvtask_region_base_daddr);
            end = ambacv_c2p(sysconfig->kernel_region_base_daddr);
            retcode = cvtask_mmap_init(start, end);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] drv_schdr_resume_visorc() : cvtask_mmap_init fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            start = ambacv_c2p(sysconfig->cma_region_daddr);
            end = start + sysconfig->cma_region_size;
            retcode = schdr_cma_mmap_init(start, end);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] drv_schdr_resume_visorc() : schdr_cma_mmap_init fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            if(sysconfig->rtos_region_size != 0U) {
                start = ambacv_c2p(sysconfig->rtos_region_daddr);
                end = start + sysconfig->rtos_region_size;
            } else {
                start = 0UL;
                end = 0UL;
            }
            retcode = schdr_rtos_mmap_init(start, end);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] drv_schdr_resume_visorc() : schdr_rtos_mmap_init fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
            }
        }
    }
    return retcode;
}

uint32_t schdr_get_ambacv_state(uint32_t *pState)
{
    uint32_t retcode = ERRCODE_NONE;

    if(pState == NULL) {
        console_printU5("[ERROR] schdr_get_ambacv_state() : pState == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        if (ambacv_fd < 0) {
            ambacv_fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
        }
        if (ambacv_fd < 0) {
            console_printU5("[ERROR] schdr_get_ambacv_state() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
        } /* if (fd < 0) : open("/dev/ambacv", ...) */

        if(retcode == ERRCODE_NONE) {
            retcode = ambacv_ioctl(ambacv_fd, AMBACV_GET_STATE, pState);
        }
    }
    return retcode;
} /* schdr_get_ambacv_state() */

uint32_t schdr_core_mutex(uint32_t mode)
{
    uint32_t retcode = ERRCODE_NONE;

    if (ambacv_fd < 0) {
        ambacv_fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
        if (ambacv_fd < 0) {
            console_printU5("[ERROR] schdr_core_mutex() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
            ambacv_fd = 0;
            retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
        } /* if (ambacv_fd < 0) : open("/dev/ambacv", ...) */
    } /* if (ambacv_fd == 0) */
    if (retcode == ERRCODE_NONE) {
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_CORE_MUTEX, &mode);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_core_mutex() : AMBACV_CORE_MUTEX fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
        }
    } /* if (ret == 0) */
    return retcode;
} /* kernel_ambacv_startup_mutex() */
#endif

uint32_t schdr_get_state(uint32_t *pState)
{
    uint32_t retcode = ERRCODE_NONE;

    if(pState == NULL) {
        console_printU5("[ERROR] schdr_get_state() : pState == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        *pState = schdr_sys_state.state;
    }
    return retcode;
} /* schdr_get_ambacv_state() */

static uint32_t schdr_sys_state_reset(void)
{
    uint32_t retcode = ERRCODE_NONE;
    schdr_cb appcb_bak;                         // TEMPORARY PATCH: some apps set the callback before schdr_init().

    appcb_bak = schdr_sys_state.app_callback;   // TEMPORARY PATCH: some apps set the callback before schdr_init().
    retcode = thread_memset(&schdr_sys_state, 0, sizeof(schdr_sys_state));
    schdr_sys_state.app_callback = appcb_bak;   // TEMPORARY PATCH: some apps set the callback before schdr_init().
    return retcode;
}

uint32_t schdr_set_env(cvtask_thpool_env_t *env, tid_t thread_id)
{
    uint32_t retcode = ERRCODE_NONE;

    if(env == NULL) {
        console_printU5("[ERROR] thpool_set_env_sched() : env == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        schdr_sys_state.tdb.tid = thread_id;
        schdr_sys_state.tdb.env = env;
        env->flexidag_slot_id = FLEXIDAG_INVALID_SLOT_ID;
    }
    return retcode;
}

cvtask_thpool_env_t* schdr_get_thenv(void)
{
    const tid_t tid = thread_self();
    cvtask_thpool_env_t *env = NULL;

    (void) tid;
#if defined(CONFIG_BUILD_CV_THREADX)
    thread_unused(tid);
#endif
    env = thpool_get_env();
    if(env == NULL) {
        if (schdr_sys_state.tdb.tid == tid) {
            env = schdr_sys_state.tdb.env;
        }
    }
    return env;
}

#if defined(AMBACV_KERNEL_SUPPORT) || defined(RTOS_BUILD)
uint32_t schdr_check_version(int32_t fd, uint32_t *version)
{
    uint32_t retcode = ERRCODE_NONE;

    if (fd < 0) {
        console_printU5("[ERROR] schdr_check_version() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
    } /* if (ambacv_fd < 0) */

    if(retcode == ERRCODE_NONE) {
        /* check ambamal kernel module version */
        retcode = ambacv_ioctl(fd, AMBACV_GET_MAL_VERSION, version);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_check_version() : AMBACV_GET_MAL_VERSION fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
        }

        if(retcode == ERRCODE_NONE) {
#if defined(ENABLE_AMBA_MAL)
            if (*version != AMBA_MAL_VERSION) {
                console_printU5("[ERROR] schdr_check_version() : PANIC!! scheduler/ko MAL version mismatch\n", 0U, 0U, 0U, 0U, 0U);
                console_printU5("[ERROR] schdr_check_version() :        expecting %d, got %d \n", AMBA_MAL_VERSION, *version, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_VERSION_MISMATCHED;
            }
#else
            if (*version != 0U) {
                console_printU5("[ERROR] schdr_check_version() : PANIC!! scheduler/ko MAL version mismatch\n", 0U, 0U, 0U, 0U, 0U);
                console_printU5("[ERROR] schdr_check_version() :        expecting %d, got %d \n", 0U, *version, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_VERSION_MISMATCHED;
            }
#endif
        }
    }

    if(retcode == ERRCODE_NONE) {
        /* check ambacv kernel module version */
        retcode = ambacv_ioctl(fd, AMBACV_GET_VERSION, version);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_check_version() : AMBACV_GET_VERSION fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
        }

        if(retcode == ERRCODE_NONE) {
            if (*version > 0U) {
                uint32_t major = ((*version) >> 8);
                uint32_t minor = ((*version) & 0xFFU);
                if ((major != CVSCHEDULER_INTERFACE_VERSION) || (minor != CVSCHEDULER_KERNEL_VERSION)) {
                    console_printU5("[ERROR] schdr_check_version() : PANIC!! scheduler/ko version mismatch\n", 0U, 0U, 0U, 0U, 0U);
                    console_printU5("[ERROR] schdr_check_version() :        expecting %d.%d, got %d.%d \n", CVSCHEDULER_INTERFACE_VERSION, CVSCHEDULER_KERNEL_VERSION, major, minor, 0U);
                    retcode = ERR_DRV_SCHDR_VERSION_MISMATCHED;
                }
            }
        }
    }

    return retcode;
}
#endif

static uint32_t schdr_init_sysconfig(visorc_init_params_t *conf)
{
    uint32_t retcode = ERRCODE_NONE;

    if(conf == NULL) {
        console_printU5("[ERROR] schdr_init_sysconfig() : conf == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        retcode = drv_schdr_init_module(conf);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_init_sysconfig() : drv_schdr_init_module fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
        }
        conf->debug_buffer_sod_cvtask_size  = schdr_sys_cfg.log_entries_viscvtask * ((uint32_t)sizeof(cvlog_cvtask_entry_t));
        conf->debug_buffer_vp_cvtask_size   = schdr_sys_cfg.log_entries_viscvtask * ((uint32_t)sizeof(cvlog_cvtask_entry_t));
        conf->debug_buffer_sod_sched_size   = schdr_sys_cfg.log_entries_visschdr * ((uint32_t)sizeof(cvlog_sched_entry_t));
        conf->debug_buffer_vp_sched_size    = schdr_sys_cfg.log_entries_visschdr * ((uint32_t)sizeof(cvlog_sched_entry_t));
        conf->debug_buffer_sod_perf_size    = schdr_sys_cfg.log_entries_visperf * ((uint32_t)sizeof(cvlog_perf_entry_t));
        conf->debug_buffer_vp_perf_size     = schdr_sys_cfg.log_entries_visperf * ((uint32_t)sizeof(cvlog_perf_entry_t));
    }
    return retcode;
}

static uint32_t schdr_resume_sysconfig(visorc_init_params_t *conf)
{
    uint32_t retcode = ERRCODE_NONE;

    if(conf == NULL) {
        console_printU5("[ERROR] schdr_resume_sysconfig() : conf == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        retcode = drv_schdr_resume_module(conf);
    }
    return retcode;
}

static void schdr_set_visorc_init(void)
{
    schdr_sys_state.visorc_init.coprocessor_cfg = schdr_sys_cfg.boot_cfg;
    if ((schdr_sys_cfg.flags & SCHDR_ENABLE_VP_RESET) != 0U) {
        schdr_sys_state.visorc_init.coprocessor_cfg |= SCHDR_ENABLE_VP_RESET;
    }

    if ((schdr_sys_cfg.flags & SCHDR_FASTBOOT) != 0U) {
        schdr_sys_state.visorc_init.disable_startup_dram_clear = 0xDEFEED00U;
    } else {
        schdr_sys_state.visorc_init.disable_startup_dram_clear = 0U;
    }

    if ((schdr_sys_cfg.flags & SCHDR_NO_INTERNAL_THREAD) != 0U) {
        schdr_sys_state.visorc_init.coprocessor_cfg |= SCHDR_NO_INTERNAL_THREAD;
        schdr_sys_cfg.internal_thread = 0U;
        console_printU5("schdr use external cmd msg thread !\n", 0U, 0U, 0U, 0U, 0U);
    } else {
        console_printU5("schdr use internal cmd msg thread !\n", 0U, 0U, 0U, 0U, 0U);
    }

    if (schdr_sys_cfg.autorun_interval != 0U) {
        schdr_sys_state.visorc_init.autorun_interval_in_ms = schdr_sys_cfg.autorun_interval;
    } else {
        schdr_sys_state.visorc_init.autorun_interval_in_ms = 0U;
    }

    if (schdr_sys_cfg.autorun_fex_cfg != 0U) {
        schdr_sys_state.visorc_init.autorun_fextest_config = schdr_sys_cfg.autorun_fex_cfg;
    } else {
        schdr_sys_state.visorc_init.autorun_fextest_config = 0U;
    }
}

int32_t schdr_start(void)
{
    uint32_t retcode = ERRCODE_NONE;

    /*
     * Init dram system: depending on compilation configuration,
     * this could be network based for amalgam simulation mode or
     * physical DRAM based for real SoC
     */
    retcode = ambacv_cache_init();
    if(retcode == ERRCODE_NONE) {
        retcode = schdr_check_cvtable_size();
    }

    if(retcode == ERRCODE_NONE) {
        /* start messaging with ORC */
        schdr_set_visorc_init();

        /* need to start visorc first before runing rx thread */
        retcode = drv_schdr_start_visorc(&schdr_sys_state.visorc_init);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_start() : drv_schdr_start_visorc fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = schdrmsg_start(&schdr_sys_state.visorc_init, 1U);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_start() : schdrmsg_start fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        console_printU5("[ERROR] schdr_start() : ambacv_cache_init fail ret 0x%x \n", retcode, 0U, 0U, 0U, 0U);
    }

    return (int32_t)retcode;
}

static uint32_t schdr_start_resume(const visorc_init_params_t *sysconfig)
{
    uint32_t retcode = ERRCODE_NONE;

    if(sysconfig == NULL) {
        console_printU5("[ERROR] schdr_start_resume() : sysconfig == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        /* Restore for case REGISTRATION_PHASE */
        retcode = schdr_set_env(&schdr_sys_state.thenv, thread_self());
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_start_resume() : schdr_set_env fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = thpool_create_thenv();
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_start_resume() : thpool_create_thenv fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
            }
        }

        /* Restore for case INITIALIZATION_PHASE  */
        if(retcode == ERRCODE_NONE) {
            retcode = thpool_create_thenv_scratchpads(schdr_sys_cfg.max_sp_size); /* TODO: Get proper scratchpad sizes */
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_start_resume() : thpool_create_thenv_scratchpads fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
            }
        }

        /* Restore system config back to core */
        if(retcode == ERRCODE_NONE) {
            retcode = drv_schdr_resume_visorc(sysconfig);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_start_resume() : drv_schdr_resume_visorc fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            retcode = schdr_log_reattach(sysconfig->arm_linfo_base_daddr);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_start_resume() : schdr_log_reattach fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
            }
        }
        if(retcode == ERRCODE_NONE) {
            /* start receiving thread */
            retcode = schdrmsg_start(sysconfig, 0U);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_start_resume() : schdrmsg_start fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            schdr_sys_state.state = SCHDR_STATE_ACTIVE;
        }
    }

    return retcode;
}

int32_t schdr_init(int32_t cpus_map, int32_t flags, int32_t boot_cfg)
{
    uint32_t retcode = ERRCODE_NONE;

    schdr_sys_cfg.cpus_map = (uint32_t) cpus_map;
    schdr_sys_cfg.flags = (uint32_t) flags;
    schdr_sys_cfg.boot_cfg = (uint32_t) boot_cfg;

    retcode = ambacv_att_init();
    if(retcode != ERRCODE_NONE) {
        console_printU5("[ERROR] schdr_init() : ambacv_att_init fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
    }

    /* reset sys state */
    if(retcode == ERRCODE_NONE) {
        retcode = schdr_sys_state_reset();
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_init() : schdr_sys_state_reset fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
        }
    }

    if(retcode == ERRCODE_NONE) {
        /* init names module */
        schdr_init_names();
    }

    if(retcode == ERRCODE_NONE) {
        /* get system configuration */
        retcode = schdr_init_sysconfig(&schdr_sys_state.visorc_init);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_init() : schdr_init_sysconfig fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
        }
    }

#ifndef DISABLE_ARM_CVTASK
    if(retcode == ERRCODE_NONE) {
        /* init thread pool */
        retcode = thpool_init(schdr_sys_cfg.cpus_map);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_init() : thpool_init fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
        }
    }

    if(retcode == ERRCODE_NONE) {
        /* init each cvtask module */
        retcode = cvtask_init_modules();
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_init() : cvtask_init_modules fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
        }
    }
#else
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "schdr disable arm cvtask \n", 0U, 0U, 0U, 0U, 0U);
#endif

    /* reset schedulr cvtable */
    if(retcode == ERRCODE_NONE) {
        retcode = schdr_internal_cvtable_reset();
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_init() : schdr_internal_cvtable_reset fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
        }
    }

    if(retcode == ERRCODE_NONE) {
        schdr_sys_state.module_init = schdr_sys_state.module_init | SCHDR_MODULE;
    }
    return (int32_t)retcode;
}

static uint32_t schdr_init_resume(void)
{
    uint32_t retcode = ERRCODE_NONE;

    retcode = ambacv_att_init();
    if(retcode != ERRCODE_NONE) {
        console_printU5("[ERROR] schdr_resume() : ambacv_att_init fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
    }

    /* reset sys state */
    if(retcode == ERRCODE_NONE) {
        retcode = schdr_sys_state_reset();
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_resume() : schdr_sys_state_reset fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
        }
    }

    if(retcode == ERRCODE_NONE) {
        /* init names module */
        schdr_init_names();
    }

    if(retcode == ERRCODE_NONE) {
        /* get system configuration */
        retcode = schdr_resume_sysconfig(&schdr_sys_state.visorc_init);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_resume() : schdr_resume_sysconfig fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
        }
    }

#ifndef DISABLE_ARM_CVTASK
    if(retcode == ERRCODE_NONE) {
        /* init thread pool */
        retcode = thpool_init(schdr_sys_cfg.cpus_map);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_resume() : thpool_init fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
        }
    }

    if(retcode == ERRCODE_NONE) {
        /* init each cvtask module */
        retcode = cvtask_init_modules();
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_resume() : cvtask_init_modules fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
        }
    }
#endif

#ifdef DISABLE_ARM_CVTASK
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "schdr disable arm cvtask \n", 0U, 0U, 0U, 0U, 0U);
#endif

    if(retcode == ERRCODE_NONE) {
        schdr_sys_state.module_init = schdr_sys_state.module_init | SCHDR_MODULE;
    }
    return retcode;
}

static uint32_t schdr_standalone_core_init(void)
{
    uint32_t retcode = ERRCODE_NONE;

    retcode = (uint32_t)schdr_init((int32_t)schdr_sys_cfg.cpus_map, (int32_t)schdr_sys_cfg.flags, (int32_t)schdr_sys_cfg.boot_cfg);
    if(retcode != ERRCODE_NONE) {
        console_printU5("[ERROR] schdr_standalone_core_init() : schdr_init fail retcode (0x%x) ", retcode, 0U, 0U, 0U, 0U);
    }

#ifndef DISABLE_CAVALRY
    if (retcode == ERRCODE_NONE) {
        if(schdr_sys_cfg.cavalry_num_slots != 0U) {
            retcode = (uint32_t)cavalry_init(CAVALRY_VERSION, schdr_sys_cfg.cavalry_num_slots);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_standalone_core_init() : cavalry_init fail retcode (0x%x) ", retcode, 0U, 0U, 0U, 0U);
            }
        }
    }
#endif

    if (retcode == ERRCODE_NONE) {
        retcode = (uint32_t)flexidag_system_init(FLEXIDAG_VERSION, schdr_sys_cfg.flexidag_num_slots);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_standalone_core_init() : flexidag_system_init fail retcode (0x%x) ", retcode, 0U, 0U, 0U, 0U);
        }
    }

    if (retcode == ERRCODE_NONE) {
        retcode = (uint32_t)schdr_start();
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_standalone_core_init() : schdr_start fail retcode (0x%x) ", retcode, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

static uint32_t flexidag_schdr_core_start(void)
{
    uint32_t retcode = ERRCODE_NONE;

    retcode = schdr_init_resume();
    if(retcode != ERRCODE_NONE) {
        console_printU5("[ERROR] flexidag_schdr_core_start() : schdr_init_resume fail retcode (0x%x) ", retcode, 0U, 0U, 0U, 0U);
    }

#ifndef DISABLE_CAVALRY
    if (retcode == ERRCODE_NONE) {
        if(schdr_sys_cfg.cavalry_num_slots != 0U) {
            retcode = cavalry_resume(CAVALRY_VERSION);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] flexidag_schdr_core_start() : cavalry_resume fail retcode (0x%x) ", retcode, 0U, 0U, 0U, 0U);
            }
        }
    }
#endif

    if (retcode == ERRCODE_NONE) {
        retcode = flexidag_system_resume(FLEXIDAG_VERSION);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] flexidag_schdr_core_start() : flexidag_system_resume fail retcode (0x%x) ", retcode, 0U, 0U, 0U, 0U);
        }
    }

    if(retcode == ERRCODE_NONE) {
        retcode = schdr_start_resume(&schdr_sys_state.visorc_init);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] flexidag_schdr_core_start() : schdr_start_resume fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
        } else {
            schdr_sys_state.state = SCHDR_STATE_ACTIVE;
        }
    }

    return retcode;
}

int32_t schdr_standalone_start(void)
{
    static psem_t standalone_semaphore;
    static uint32_t standalone_init = 0U;
    uint32_t  tmp_state;
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;

    if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
        retcode = schdr_core_mutex(0U);
        if(retcode == ERRCODE_NONE) {
            retcode = schdr_get_ambacv_state(&tmp_state);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_standalone_start() : schdr_get_ambacv_state fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
            }

            if (retcode == ERRCODE_NONE) {
                if (tmp_state == 0U) {
                    if(standalone_init == 0U) {
                        retcode = thread_sem_init(&standalone_semaphore, 0, 0U);
                        if(retcode != ERRCODE_NONE) {
                            console_printU5("[ERROR] schdr_standalone_start() : thread_sem_init fail retcode (0x%x) ", retcode, 0U, 0U, 0U, 0U);
                        } else {
                            standalone_init = 1U;
                        }
                    }
                    pStartupSemaphore = &standalone_semaphore;
                    if (retcode == ERRCODE_NONE) {
                        retcode = schdr_standalone_core_init();
                    }

                    if (retcode == ERRCODE_NONE) {
#ifdef PACE_DIAG
                        retcode = thread_sem_wait(&standalone_semaphore, 1200000);  // Change to 2 minutes for PACE
#else /* !PACE_DIAG */
                        retcode = thread_sem_wait(&standalone_semaphore, SCHDR_START_TIMEOUT);
#endif /* ?PACE_DIAG */
                        if(retcode != ERRCODE_NONE) {
                            console_printU5("[ERROR] schdr_standalone_start() : thread_sem_wait fail retcode (0x%x) ", retcode, 0U, 0U, 0U, 0U);
                        }
                        if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
                            retcode = ERR_DRV_SCHDR_ACTIVATE_FAIL;
                            console_printU5("[ERROR] schdr_standalone_start() : scheduler shutdown - system has not started\n", 0U, 0U, 0U, 0U, 0U);
                        }
                        pStartupSemaphore = NULL;
                    }
                } else {
                    retcode = flexidag_schdr_core_start();
                    if (retcode != ERRCODE_NONE) {
                        console_printU5("[ERROR] schdr_standalone_start() : schdr_resume fail retcode (0x%x) ", retcode, 0U, 0U, 0U, 0U);
                    }
                }
            }
            ret = schdr_core_mutex(1U);
            if (retcode == ERRCODE_NONE) {
                retcode = ret;
            }
        }
    } else {
        retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
        console_printU5("[ERROR] schdr_standalone_start() : scheduler already active \n", 0U, 0U, 0U, 0U, 0U);
    }

    return (int32_t)retcode;

} /* schdr_standalone_start() */

int32_t flexidag_schdr_start(void)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;

    if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
        retcode = schdr_core_mutex(0U);
        if (retcode == ERRCODE_NONE) {
            uint32_t  tmp_state;

            retcode = schdr_get_ambacv_state(&tmp_state);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] flexidag_schdr_start() : schdr_get_ambacv_state fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
            }

            if (retcode == ERRCODE_NONE) {
                if (tmp_state == 0U) {
                    console_printU5("[ERROR] flexidag_schdr_start() : Unable to attach to flexidag scheduler - system has not started\n", 0U, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_ACTIVATE_FAIL;
                } else {
                    retcode = flexidag_schdr_core_start();
                    if (retcode != ERRCODE_NONE) {
                        console_printU5("[ERROR] flexidag_schdr_start() : schdr_resume fail retcode (0x%x) ", retcode, 0U, 0U, 0U, 0U);
                    }
                }
            } /* if (ret == 0) : schdr_get_ambacv_state() */
            ret = schdr_core_mutex(1U);
            if (retcode == ERRCODE_NONE) {
                retcode = ret;
            }
        } /* if (ret == 0) : schdr_core_mutex() */
    } else {
        console_printU5("[ERROR] flexidag_schdr_start() : standalone instance already started, ignoring request\n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
    }

    return (int32_t)retcode;
} /* flexidag_schdr_start() */

int32_t schdrcfg_set_flexidag_num_slots(int32_t num_slots)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;

    if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
        retcode = schdr_core_mutex(0U);
        if (retcode == ERRCODE_NONE) {
            uint32_t  tmp_state;
            tmp_state = 0U;
            retcode = schdr_get_ambacv_state(&tmp_state);
            if (retcode == ERRCODE_NONE) {
                if (tmp_state == 0U) {
                    schdr_sys_cfg.flexidag_num_slots = (uint32_t)num_slots;
                } /* if (ambacv_state == 0) */
                else { /* if (ambacv_state != 0) */
                    console_printU5("[ERROR] schdrcfg_set_flexidag_num_slots() : (%d) : Scheduler already active - cannot reconfigure flexidag_num_slots\n", (uint32_t)num_slots, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
                } /* if (ambacv_state != 0) */
            } /* if (ret == 0) : schdr_get_ambacv_state() */
            ret = schdr_core_mutex(1U);
            if(retcode == ERRCODE_NONE) {
                retcode = ret;
            }
        } /* if (ret == 0) : schdr_core_mutex() */
    } else {
        console_printU5("[ERROR] schdrcfg_set_flexidag_num_slots() : Scheduler instance already active \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
    }
    return (int32_t)retcode;

} /* schdrcfg_set_flexidag_num_slots() */

int32_t schdrcfg_set_cavalry_num_slots(int32_t num_slots)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;

    if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
        retcode = schdr_core_mutex(0U);
        if (retcode == ERRCODE_NONE) {
            uint32_t  tmp_state;
            tmp_state = 0U;
            retcode = schdr_get_ambacv_state(&tmp_state);
            if (retcode == ERRCODE_NONE) {
                if (tmp_state == 0U) {
                    schdr_sys_cfg.cavalry_num_slots = (uint32_t)num_slots;
                } /* if (ambacv_state == 0) */
                else { /* if (ambacv_state != 0) */
                    console_printU5("[ERROR] schdrcfg_set_cavalry_num_slots() : (%d) : Scheduler already active - cannot reconfigure cavalry_num_slots\n", (uint32_t)num_slots, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
                } /* if (ambacv_state != 0) */
            } /* if (ret == 0) : schdr_get_ambacv_state() */
            ret = schdr_core_mutex(1U);
            if(retcode == ERRCODE_NONE) {
                retcode = ret;
            }
        } /* if (ret == 0) : schdr_core_mutex() */
    } else {
        console_printU5("[ERROR] schdrcfg_set_cavalry_num_slots() : Scheduler instance already active \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
    }

    return (int32_t)retcode;
} /* schdrcfg_set_cavalry_num_slots() */

int32_t schdrcfg_set_cpu_map(int32_t cpu_map)
{
    uint32_t retcode = ERRCODE_NONE;

    if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
        schdr_sys_cfg.cpus_map = (uint32_t)cpu_map;
    } else {
        console_printU5("[ERROR] schdrcfg_set_cpu_map() : (%d) : Scheduler instance already active - cannot reconfigure cpu_map\n", (uint32_t)cpu_map, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
    }

    return (int32_t)retcode;
} /* schdrcfg_set_cpu_map() */

int32_t schdrcfg_set_verbosity(int32_t verbosity)
{
    uint32_t retcode = ERRCODE_NONE;

    if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
        schdr_sys_cfg.verbosity = verbosity;
    } else {
        console_printU5("[ERROR] schdrcfg_set_verbosity() : (%d) : Scheduler instance already active - cannot reconfigure scheduler verbosity\n", (uint32_t)verbosity, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
    }

    return (int32_t)retcode;
} /* schdrcfg_set_verbosity() */

int32_t schdrcfg_set_flags(int32_t flags)
{
    uint32_t retcode = ERRCODE_NONE;

    if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
        schdr_sys_cfg.flags = (uint32_t)flags;
    } else {
        console_printU5("[ERROR] schdrcfg_set_flags() : (%d) : Scheduler instance already active - cannot reconfigure scheduler startup flags\n", (uint32_t)flags, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
    }

    return (int32_t)retcode;
} /* schdrcfg_set_verbosity() */

int32_t schdrcfg_set_boot_cfg(int32_t cfg)
{
    uint32_t retcode = ERRCODE_NONE;

    if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
        schdr_sys_cfg.boot_cfg = (uint32_t)cfg;
    } else {
        console_printU5("[ERROR] schdrcfg_set_boot_cfg() : (%d) : Scheduler instance already active - cannot reconfigure scheduler startup flags\n", (uint32_t)cfg, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
    }

    return (int32_t)retcode;
} /* schdrcfg_set_verbosity() */

int32_t schdrcfg_set_autorun_interval(int32_t interval)
{
    uint32_t retcode = ERRCODE_NONE;

    if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
        schdr_sys_cfg.autorun_interval = (uint32_t)interval;
    } else {
        console_printU5("[ERROR] schdrcfg_set_autorun_interval() : (%d) : Scheduler instance already active - cannot reconfigure scheduler startup flags\n", (uint32_t)interval, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
    }

    return (int32_t)retcode;
} /* schdrcfg_set_autorun_interval() */

int32_t schdrcfg_set_autorun_fex_cfg(int32_t cfg)
{
    uint32_t retcode = ERRCODE_NONE;

    if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
        if(cfg == 1) {
            schdr_sys_cfg.autorun_fex_cfg = (uint32_t)0x2d15ab1eU;
        }
    } else {
        console_printU5("[ERROR] schdrcfg_set_autorun_fex_cfg() : (%d) : Scheduler instance already active - cannot reconfigure scheduler startup flags\n", (uint32_t)cfg, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
    }

    return (int32_t)retcode;
} /* schdrcfg_set_autorun_interval() */

int32_t schdrcfg_set_log_entries_visschdr(uint32_t size)
{
    uint32_t retcode = ERRCODE_NONE;

    if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
        schdr_sys_cfg.log_entries_visschdr = size;
    } else {
        console_printU5("[ERROR] schdrcfg_set_log_entries_visschdr() : (%d) : Scheduler instance already active - cannot reconfigure scheduler startup flags\n", size, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
    }

    return (int32_t)retcode;
} /* schdrcfg_set_log_entries_visschdr() */

int32_t schdrcfg_set_log_entries_viscvtask(uint32_t size)
{
    uint32_t retcode = ERRCODE_NONE;

    if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
        schdr_sys_cfg.log_entries_viscvtask = size;
    } else {
        console_printU5("[ERROR] schdrcfg_set_log_entries_viscvtask() : (%d) : Scheduler instance already active - cannot reconfigure scheduler startup flags\n", size, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
    }

    return (int32_t)retcode;
} /* schdrcfg_set_log_entries_viscvtask() */

int32_t schdrcfg_set_log_entries_visperf(uint32_t size)
{
    uint32_t retcode = ERRCODE_NONE;

    if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
        schdr_sys_cfg.log_entries_visperf = size;
    } else {
        console_printU5("[ERROR] schdrcfg_set_log_entries_visperf() : (%d) : Scheduler instance already active - cannot reconfigure scheduler startup flags\n", size, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
    }

    return (int32_t)retcode;
} /* schdrcfg_set_log_entries_visperf() */

/*****************  Shut down   ******************/
#if defined(AMBACV_KERNEL_SUPPORT) || defined(RTOS_BUILD)
uint32_t schdr_stop_visorc(void)
{
    return ambacv_ioctl(ambacv_fd, AMBACV_STOP_VISORC, NULL);
}
#endif

int32_t flexidag_schdr_stop(void)
{
    uint32_t retcode = ERRCODE_NONE;

    schdr_sys_state.module_init = (schdr_sys_state.module_init & ~((uint32_t)FLEXIDAG_MODULE));
    return (int32_t)retcode;
} /* flexidag_schdr_stop() */

uint32_t schdr_process_shutdown(uint32_t flag, uint64_t state_addr)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t tmp_boot_os = schdr_sys_state.visorc_init.coprocessor_cfg;
    uint32_t  tmp_state = 0U;

    schdr_sys_state.module_init = 0U;
    schdr_sys_state.state = SCHDR_STATE_SAFE;
    retcode = thpool_fini();
    if(retcode == ERRCODE_NONE) {
        /* Only process one time */
        if(flag != SCHDR_ARM_RESET) {
            if(schdr_core_mutex(0U) != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_process_shutdown() : schdr_core_mutex lock fail", 0U, 0U, 0U, 0U, 0U);
            } else {
                retcode = schdr_get_ambacv_state(&tmp_state);
                if (retcode == ERRCODE_NONE) {
                    if (tmp_state >= 1U) {
                        if( schdr_log_flush(state_addr, tmp_boot_os) != ERRCODE_NONE) {
                            console_printU5("[ERROR] schdr_process_shutdown() : schdr_log_flush fail", 0U, 0U, 0U, 0U, 0U);
                        }
                        retcode = schdr_stop_visorc();
                    }
                }
                if(schdr_core_mutex(1U) != ERRCODE_NONE) {
                    console_printU5("[ERROR] schdr_process_shutdown() : schdr_core_mutex unlock fail", 0U, 0U, 0U, 0U, 0U);
                }
            }
        }

        /* Process for each */
        if(retcode == ERRCODE_NONE) {
            retcode = schdrmsg_stop();
        }

        if (retcode == ERRCODE_NONE) {
            retcode = (uint32_t)flexidag_schdr_stop();
        }

        if (retcode == ERRCODE_NONE) {
            schdr_sysflow_cvtable_reset();
            /* we are shuting down, check if user want to handle it */
            if (schdr_sys_state.app_callback != NULL) {
                if(schdr_sys_state.app_callback((int32_t)SCHDR_CB_START_SHUTDOWN, NULL) != 0) {
                    console_printU5("[ERROR] schdr_process_shutdown() : cb fail ", 0U, 0U, 0U, 0U, 0U);
                }
            }
        }

        if (retcode == ERRCODE_NONE) {
            if(schdr_shutdown_unmap() != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_process_shutdown() : schdr_shutdown_unmap fail ", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }

    schdr_sys_state.state = SCHDR_STATE_INACTIVE;
    return retcode;
}

int32_t schdr_shutdown(int32_t reserved)
{
    uint32_t retcode = ERRCODE_NONE;

    /* if schdr is not started yet, return immediately */
    if (schdr_sys_state.state == SCHDR_STATE_ACTIVE) {
        if((uint32_t)reserved == SCHDR_ARM_RESET) {
            retcode = schdr_process_shutdown(SCHDR_ARM_RESET, 0U);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_shutdown() : schdr_process_shutdown fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
            }
        } else {
            retcode = (uint32_t)schdr_request_shutdown(reserved);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_shutdown() : schdr_request_shutdown fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
            }
        }

        if (retcode == ERRCODE_NONE) {
            /* wait unitl shutdown is finished */
            retcode = schdrmsg_waitfor_shutdown();
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_shutdown() : schdrmsg_waitfor_shutdown fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
            }
        }
    }
    return (int32_t)retcode;
}

int32_t schdr_request_shutdown(int32_t reserved)
{
    armvis_msg_t msg = {0};
    schedmsg_arm_shutdown_req_t *rpl = &msg.msg.arm_shutdown_req;

    /* request master scheduder to shutdown */
    msg.hdr.message_type  = (uint16_t)SCHEDMSG_ARM_REQUESTS_SHUTDOWN;
    rpl->dummyval = (uint32_t)reserved;
    return (int32_t)schdrmsg_send_msg(&msg, (uint32_t)sizeof(msg));
} /* schdr_request_shutdown() */

