// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
*  Copyright (c) 2021 Ambarella International LP
*/

#include "os_kernel.h"
#include "ambacv_kal.h"
#include "ambacv_kernel.h"
#include "visorc_kernel.h"
#include "cache_kernel.h"
#include "msg_kernel.h"
#include "schdr_kernel.h"
#include "flexidag_kernel.h"
#include "schdr_api.h"

uint32_t cluster_id = 0U;
uint32_t is_booting_os = 0U;
uint32_t audio_clock_ms = 12288U;
static uint32_t schdr_isr_num = 0U;

cvflow_scheduler_support_t  scheduler_support;

uint32_t krn_schdr_set_trace_daddr(uint64_t trace_daddr)
{
    uint32_t retcode = ERRCODE_NONE;

    if (trace_daddr != 0UL) {
        if (trace_daddr != scheduler_support.scheduler_trace_daddr) {
            if(krn_c2v(trace_daddr) != NULL) {
                scheduler_support.scheduler_trace_daddr = trace_daddr;
                scheduler_support.vpSchedulerTrace      = krn_c2v(trace_daddr);
            } else {
                retcode = ERR_DRV_SCHDR_TRACE_INVALID;
                krn_printU5("[ERROR] krn_schdr_set_trace_daddr() : Invalid trace_daddr 0x%x \n", trace_daddr, 0U, 0U, 0U, 0U);
            }
        } /* if (trace_daddr != scheduler_trace_daddr) */
    } /* if (trace_daddr != 0) */

    return retcode;
} /* scheduler_system_set_scheduler_trace() */

uint64_t krn_schdr_get_trace_daddr(void)
{
    return scheduler_support.scheduler_trace_daddr;
} /* scheduler_system_get_trace() */

uint32_t krn_schdr_set_autorun_trace_daddr(uint64_t trace_daddr)
{
    uint32_t retcode = ERRCODE_NONE;

    if (trace_daddr != 0UL) {
        if (trace_daddr != scheduler_support.autorun_trace_daddr) {
            if(krn_c2v(trace_daddr) != NULL) {
                scheduler_support.autorun_trace_daddr = trace_daddr;
                scheduler_support.vpAutoRunTrace      = krn_c2v(trace_daddr);
            } else {
                retcode = ERR_DRV_SCHDR_AUTORUN_TRACE_INVALID;
                krn_printU5("[ERROR] krn_schdr_set_autorun_trace_daddr() : Invalid trace_daddr 0x%x \n", trace_daddr, 0U, 0U, 0U, 0U);
            }
        } /* if (trace_daddr != scheduler_support.autorun_trace_daddr) */
    } /* if (trace_daddr != 0) */

    return retcode;
} /* scheduler_autorun_set_trace_daddr() */

uint64_t krn_schdr_get_autorun_trace_daddr(void)
{
    return scheduler_support.autorun_trace_daddr;
} /* scheduler_autorun_get_trace_daddr() */

void krn_schdr_get_isr_num(uint32_t *num)
{
    *num = schdr_isr_num;
}

#if defined (CONFIG_ARM32) || defined (CONFIG_ARM64)
#pragma GCC push_options
#pragma GCC target("general-regs-only")
void krn_schdr_isr(uint32_t irq, void* args)
{
    (void) irq;
    (void) args;
    krn_unused(args);
    schdr_isr_num++;
    (void) krn_eventflag_set(&rx_lock, EFLAG_NEW_MSG);
}
#pragma GCC pop_options
#else
void krn_schdr_isr(uint32_t irq, void* args)
{
    (void) irq;
    (void) args;
    schdr_isr_num++;
    (void) krn_eventflag_set(&rx_lock, EFLAG_NEW_MSG);
}
#endif

static uint32_t krn_schdr_reset_sysconfig(void)
{
    uint32_t retcode = ERRCODE_NONE;
    armvis_message_queue_t *conf;
    uint32_t size_32;
    uint64_t conf_base = 0UL, buff_base = 0UL, top = 0UL, size = 0UL;
    const ambacv_all_mem_t *mem_all;
    armvis_msg_t *msg;
    const void *ptr;

    mem_all = krn_ambacv_get_mem();
    // setup local queue config according to scheduler_id
    top  = (mem_all->all.base + mem_all->all.size);
    conf_base = top - (uint64_t)SYSCONF_QINFO_SIZE;
    size = (uint64_t)scheduler_id * sizeof(*conf);
    conf_base = conf_base + (uint64_t)(2UL * size);
    buff_base = top - (uint64_t)SCHDRMSG_MEM_REGION_SIZE;
    size_32 = MSG_QUEUE_LENGTH;
    size = (uint64_t)size_32;
    size = scheduler_id * size;
    buff_base = buff_base + (2UL * size);

    qconfig.reqQ_addr = conf_base;
    ptr = krn_c2v(qconfig.reqQ_addr);
    if(ptr == NULL) {
        retcode = ERR_DRV_SCHDR_VISMSG_QUEUE_ORC2ARM_INVALID;
    } else {
        krn_typecast(&conf,&ptr);
        retcode = krn_memset(conf, 0, sizeof(*conf));
        if(retcode == ERRCODE_NONE) {
            conf->msg_array_daddr     = (uint32_t)buff_base;
            conf->max_count           = ARMVIS_MESSAGE_NUM;
            conf->queue_id            = scheduler_id;
            conf->rd_msg_array_daddr  = conf->msg_array_daddr;  /* Shadow copies  */
            conf->wr_msg_array_daddr  = conf->msg_array_daddr;  /*  |             */
            conf->rd_queue_id         = conf->queue_id;         /*  |             */
            conf->wr_queue_id         = conf->queue_id;         /* -/             */

            qconfig.from_orc_r    = &(conf->rd_idx);
            qconfig.from_orc_w    = &(conf->wr_idx);
            ptr = krn_c2v(conf->msg_array_daddr);
            if(ptr == NULL) {
                retcode = ERR_DRV_SCHDR_VISMSG_QUEUE_ORC2ARM_INVALID;
            } else {
                krn_typecast(&msg,&ptr);
                qconfig.from_orc_q = msg;
            }
        }
    }

    if(retcode == ERRCODE_NONE) {
        qconfig.rplQ_addr = conf_base + sizeof(*conf);
        ptr = krn_c2v(qconfig.rplQ_addr);
        if(ptr == NULL) {
            retcode = ERR_DRV_SCHDR_VISMSG_QUEUE_ARM2ORC_INVALID;
        } else {
            krn_typecast(&conf,&ptr);
            retcode = krn_memset(conf, 0, sizeof(*conf));
            if(retcode == ERRCODE_NONE) {
                conf->msg_array_daddr     = (uint32_t)(buff_base + MSG_QUEUE_LENGTH);
                conf->max_count           = ARMVIS_MESSAGE_NUM;
                conf->queue_id            = scheduler_id;
                conf->rd_msg_array_daddr  = conf->msg_array_daddr;  /* Shadow copies  */
                conf->wr_msg_array_daddr  = conf->msg_array_daddr;  /*  |             */
                conf->rd_queue_id         = conf->queue_id;         /*  |             */
                conf->wr_queue_id         = conf->queue_id;         /* -/             */
                qconfig.from_arm_r    = &(conf->rd_idx);
                qconfig.from_arm_w    = &(conf->wr_idx);
                ptr = krn_c2v(conf->msg_array_daddr);
                if(ptr == NULL) {
                    retcode = ERR_DRV_SCHDR_VISMSG_QUEUE_ARM2ORC_INVALID;
                } else {
                    krn_typecast(&msg,&ptr);
                    qconfig.from_arm_q = msg;
                }
            }
        }
    }
    return retcode;
}

uint32_t krn_schdr_get_sysconfig(visorc_init_params_t *arg)
{
    uint32_t retcode = ERRCODE_NONE;
    armvis_message_queue_t *q;
    const ambacv_all_mem_t *mem_all;
    const void *ptr;
    uint64_t base = 0UL,size = 0UL;

    if(arg == NULL) {
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_schdr_get_sysconfig() : arg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        //krn_schdrmsg_shutdown();
        mem_all = krn_ambacv_get_mem();

        /* Initialize scheduler */
        retcode = krn_schdr_reset_sysconfig();
        if(retcode == ERRCODE_NONE) {
            /* reset queue config */
            qconfig.from_orc_wi = 0U;
            qconfig.from_orc_w_seqno = 0U;
            qconfig.from_arm_w_seqno = 0U;
            ptr = krn_c2v(qconfig.reqQ_addr);
            if(ptr == NULL) {
                retcode = ERR_DRV_SCHDR_VISMSG_QUEUE_ORC2ARM_INVALID;
            } else {
                krn_typecast(&q,&ptr);
                q->rd_idx = 0U;
                q->wr_idx = 0U;
                retcode = krn_cache_clean(q, sizeof(*q));

                if(retcode == ERRCODE_NONE) {
                    ptr = krn_c2v(qconfig.rplQ_addr);
                    krn_typecast(&q,&ptr);
                    q->rd_idx = 0U;
                    q->wr_idx = 0U;
                    retcode = krn_cache_clean(q, sizeof(*q));
                }
            }

            if(retcode == ERRCODE_NONE) {
                /* return info back to user */
                /* first 6MB is reserved for orc binaries */
                /*  last 1MB is reserved for arm-orc messag queue */
                retcode = krn_memset(arg, 0, sizeof(visorc_init_params_t));
                if(retcode == ERRCODE_NONE) {
                    base = mem_all->all.base;
                    size = mem_all->all.size;
                    arg->sysinit_dram_block_baseaddr = (uint32_t)(base + VISORC_MEM_REGION_SIZE);
                    arg->sysinit_dram_block_size     = (uint32_t)(size - VISORC_MEM_REGION_SIZE - SCHDRMSG_MEM_REGION_SIZE);

                    /* use copressor_cfg to pass size of shdrmsg region to user */
                    arg->coprocessor_cfg = SCHDRMSG_MEM_REGION_SIZE;

                    arg->MsgQueue_orc_to_arm_daddr[scheduler_id] = (uint32_t)qconfig.reqQ_addr;
                    arg->MsgQueue_arm_to_orc_daddr[scheduler_id] = (uint32_t)qconfig.rplQ_addr;
                    arg->kernel_region_base_daddr  = arg->sysinit_dram_block_baseaddr + arg->sysinit_dram_block_size;
                    arg->kernel_region_size        = SCHDRMSG_MEM_REGION_SIZE;
                    arg->cma_region_daddr  = (uint32_t) mem_all->cma_region.base;
                    arg->cma_region_size   = (uint32_t) mem_all->cma_region.size;
                    arg->rtos_region_daddr = (uint32_t) mem_all->rtos_region.base;
                    arg->rtos_region_size  = (uint32_t) mem_all->rtos_region.size;
                    arg->region_cache_flags = 0U;
                    if(enable_arm_cache != 0U) {
                        arg->region_cache_flags = arg->region_cache_flags | CACHE_FLAG_SUPERDAG;
                    }

                    if(enable_orc_cache != 0U) {
                        arg->region_cache_flags = arg->region_cache_flags | CACHE_FLAG_VISORC;
                    }

                    if(enable_rtos_cache != 0U) {
                        arg->region_cache_flags = arg->region_cache_flags | CACHE_FLAG_RTOS;
                    }
                }
            }
        }
    }
    return retcode;
}

static void krn_schdr_get_clock(visorc_init_params_t *sysconfig)
{
    uint32_t freq = 0;
    uint32_t clock_ms;

    if(krn_get_clock(AUDIO_CLOCK_ID, &freq) == ERRCODE_NONE) {
        audio_clock_ms = (freq + 500U)/1000U; /* rounding */
        sysconfig->chip_audio_ticks_per_ms = audio_clock_ms;
        krn_module_printU5("audio clock %d \n", freq, 0U, 0U, 0U, 0U);
    } else {
        krn_module_printU5("audio clock not found \n", 0U, 0U, 0U, 0U, 0U);
    }

#if defined(CHIP_CV6)
    if(krn_get_clock(HSM_CLOCK_ID, &freq) == ERRCODE_NONE) {
        clock_ms = (freq + 500U)/1000U; /* rounding */
        sysconfig->chip_visorc_ticks_per_ms = clock_ms;
        krn_module_printU5("vision clock %d \n", freq, 0U, 0U, 0U, 0U);
    } else {
        krn_module_printU5("vision clock not found\n", 0U, 0U, 0U, 0U, 0U);
    }

    if(krn_get_clock(NVP_CLOCK_ID, &freq) == ERRCODE_NONE) {
        clock_ms = (freq + 500U)/1000U; /* rounding */
        sysconfig->chip_nvp_ticks_per_ms = clock_ms;
        krn_module_printU5("nvp clock %d \n", freq, 0U, 0U, 0U, 0U);
    } else {
        krn_module_printU5("nvp clock not found\n", 0U, 0U, 0U, 0U, 0U);
    }

    if(krn_get_clock(GVP_CLOCK_ID, &freq) == ERRCODE_NONE) {
        clock_ms = (freq + 500U)/1000U; /* rounding */
        sysconfig->chip_gvp_ticks_per_ms = clock_ms;
        krn_module_printU5("gvp clock %d \n", freq, 0U, 0U, 0U, 0U);
    } else {
        krn_module_printU5("gvp clock not found\n", 0U, 0U, 0U, 0U, 0U);
    }

    if(krn_get_clock(FEX_CLOCK_ID, &freq) == ERRCODE_NONE) {
        clock_ms = (freq + 500U)/1000U; /* rounding */
        sysconfig->chip_fex_ticks_per_ms = clock_ms;
        krn_module_printU5("fex clock %d \n", freq, 0U, 0U, 0U, 0U);
    } else {
        krn_module_printU5("fex clock not found\n", 0U, 0U, 0U, 0U, 0U);
    }

    if(krn_get_clock(FMA_CLOCK_ID, &freq) == ERRCODE_NONE) {
        clock_ms = (freq + 500U)/1000U; /* rounding */
        sysconfig->chip_fma_ticks_per_ms = clock_ms;
        krn_module_printU5("fma clock %d \n", freq, 0U, 0U, 0U, 0U);
    } else {
        krn_module_printU5("fma clock not found\n", 0U, 0U, 0U, 0U, 0U);
    }
#else
    if(krn_get_clock(VISION_CLOCK_ID, &freq) == ERRCODE_NONE) {
        clock_ms = (freq + 500U)/1000U; /* rounding */
        sysconfig->chip_visorc_ticks_per_ms = clock_ms;
        krn_module_printU5("vision clock %d \n", freq, 0U, 0U, 0U, 0U);
    } else {
        krn_module_printU5("vision clock not found\n", 0U, 0U, 0U, 0U, 0U);
    }
#endif
}

static uint32_t krn_schdr_wait_non_bootos(visorc_init_params_t *sysconfig)
{
    uint32_t loop = 1U,ret,i;
    uint32_t retcode = ERRCODE_NONE;

    /* wait for all reqQ/rplQ becoming ready */
    krn_module_printU5("Waiting for %d sub-schedulers\n",
                       sysconfig->coprocessor_cfg, 0U, 0U, 0U, 0U);
    while (loop == 1U) {
        const volatile uint32_t *ptr0, *ptr1;

        ret = 0U;
        retcode = krn_cache_invalidate(&sysconfig->MsgQueue_orc_to_arm_daddr[0], SYSTEM_MAX_NUM_CORTEX * sizeof(sysconfig->MsgQueue_orc_to_arm_daddr[0]));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_cache_invalidate(&sysconfig->MsgQueue_arm_to_orc_daddr[0], SYSTEM_MAX_NUM_CORTEX * sizeof(sysconfig->MsgQueue_arm_to_orc_daddr[0]));
        }

        if(retcode == ERRCODE_NONE) {
            for (i = 0U; i < (uint32_t)SYSTEM_MAX_NUM_CORTEX; i++) {
                ptr0 = (volatile uint32_t*) &sysconfig->MsgQueue_orc_to_arm_daddr[i];
                ptr1 = (volatile uint32_t*) &sysconfig->MsgQueue_arm_to_orc_daddr[i];
                if (*ptr0 != 0U) {
                    if (*ptr1 != 0U) {
                        ret++;
                    }
                }
            }
            if (ret == sysconfig->coprocessor_cfg) {
                loop = 0U;
            }
            if( krn_sleep(10) != ERRCODE_NONE) {
                krn_module_printU5("[ERROR] krn_schdr_wait_non_bootos() : krn_sleep fail", 0U, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode != ERRCODE_NONE) {
            break;
        }
    }
    return retcode;
}

static uint32_t krn_schdr_start_bootos(const visorc_init_params_t *arg, visorc_init_params_t *sysconfig, volatile uint32_t *slock)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_all_mem_t *mem_all;
    static uint32_t init_visorc = 0U;
    uint64_t start,end;

    (void) arg;
    mem_all = krn_ambacv_get_mem();
    retcode = krn_memset(sysconfig, 0, sizeof(*sysconfig));
    if(retcode == ERRCODE_NONE) {
        sysconfig->sysinit_dram_block_baseaddr  = arg->sysinit_dram_block_baseaddr;
        sysconfig->sysinit_dram_block_size      = arg->sysinit_dram_block_size;
        sysconfig->coprocessor_cfg              = arg->coprocessor_cfg & 0xFFU;
        sysconfig->debug_buffer_sod_cvtask_size = arg->debug_buffer_sod_cvtask_size;
        sysconfig->debug_buffer_vp_cvtask_size  = arg->debug_buffer_vp_cvtask_size;
        sysconfig->debug_buffer_sod_sched_size  = arg->debug_buffer_sod_sched_size;
        sysconfig->debug_buffer_vp_sched_size   = arg->debug_buffer_vp_sched_size;
        sysconfig->debug_buffer_sod_perf_size   = arg->debug_buffer_sod_perf_size;
        sysconfig->debug_buffer_vp_perf_size    = arg->debug_buffer_vp_perf_size;
        sysconfig->hotlink_region_daddr         = arg->hotlink_region_daddr;
        sysconfig->hotlink_region_size          = arg->hotlink_region_size;
        sysconfig->cavalry_base_daddr           = arg->cavalry_base_daddr;
        sysconfig->cavalry_num_slots            = arg->cavalry_num_slots;
        sysconfig->flexidag_num_slots           = arg->flexidag_num_slots;
        sysconfig->flexidag_slot_size           = arg->flexidag_slot_size;
        sysconfig->cma_region_daddr             = arg->cma_region_daddr;
        sysconfig->cma_region_size              = arg->cma_region_size;
        sysconfig->rtos_region_daddr            = arg->rtos_region_daddr;
        sysconfig->rtos_region_size             = arg->rtos_region_size;
        sysconfig->region_cache_flags           = arg->region_cache_flags;
        sysconfig->kernel_region_base_daddr     = arg->kernel_region_base_daddr;
        sysconfig->kernel_region_size           = arg->kernel_region_size;
        // Propagate autorun values
        sysconfig->autorun_fextest_config       = arg->autorun_fextest_config;
        sysconfig->autorun_interval_in_ms       = arg->autorun_interval_in_ms;
#if defined(CHIP_CV1)
        sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV1 | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV2)
        sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV2 | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV22)
        sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV22 | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV25)
        sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV25 | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV28)
        sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV28 | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV2A) || defined(CHIP_CV2FS)
        sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV2A | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV22A) || defined(CHIP_CV22FS)
        sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV22A | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV5)
        sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV5 | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV52)
        sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV52 | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV6)
        sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV6 | CVSCHEDULER_KERNEL_VERSION);
#else
#error "unsupported CHIP ID, please check your PROJECT setting"
#endif
        sysconfig->cvsched_and_cvtask_api_ver   = (((uint32_t)CVSCHEDULER_INTERFACE_VERSION <<16) | CVTASK_API_VERSION);
        krn_schdr_get_clock(sysconfig);

        mem_all->hotlink_region.base      = arg->hotlink_region_daddr;
        mem_all->hotlink_region.size      = arg->hotlink_region_size;
        mem_all->rtos_region.base         = arg->rtos_region_daddr;
        mem_all->rtos_region.size         = arg->rtos_region_size;

        sysconfig->disable_startup_dram_clear = arg->disable_startup_dram_clear;
        if(enable_orc_cache != 0U) {
            sysconfig->disable_cache = 0U;
        } else {
            sysconfig->disable_cache = 0x4D15AB1EU;
        }
        sysconfig->MsgQueue_orc_to_arm_daddr[scheduler_id] = (uint32_t)qconfig.reqQ_addr;
        sysconfig->MsgQueue_arm_to_orc_daddr[scheduler_id] = (uint32_t)qconfig.rplQ_addr;

        /* release spin-lock so other sub-schedulers can continue */
        *slock = 0xBABEFACEU;
        retcode = krn_cache_clean(sysconfig, sizeof(*sysconfig));

        if(retcode == ERRCODE_NONE) {
            retcode = krn_schdr_wait_non_bootos(sysconfig);
        }

        if(retcode == ERRCODE_NONE) {
            /* reset spin lock as soon as we can */
            *slock = 0;
            retcode = krn_cache_clean(&sysconfig->rtos_spin_lock, sizeof(sysconfig->rtos_spin_lock));
        }

        if(retcode == ERRCODE_NONE) {
            /* Reset internal flexidag kernel state */
            retcode = krn_flexidag_system_reset(1U);
        }

        if(retcode == ERRCODE_NONE) {
            if (enable_arm_cache != 0U) {
                /* to speed up boot time, make assumption about there is not dirty data in cv dram range. Only treat dirt data after second visorc start */
                if(init_visorc == 1U) {
                    if(sysconfig->sysinit_dram_block_baseaddr != 0U) {
                        start = krn_c2p(sysconfig->sysinit_dram_block_baseaddr);
                        end = krn_c2p(sysconfig->sysinit_dram_block_baseaddr) + sysconfig->sysinit_dram_block_size - 1U;
                        krn_module_printU5("Explicitly clearing VISORC area from cache [0x%llx - 0x%llx]\n", start, end, 0U, 0U, 0U);
                        retcode = krn_cache_invalidate(krn_c2v(sysconfig->sysinit_dram_block_baseaddr), sysconfig->sysinit_dram_block_size);
                    } else {
                        retcode = ERR_DRV_SCHDR_SYSTEM_START_INVALID;
                    }
                } else {
                    init_visorc = 1U;
                }
            } /* if (enable_arm_cache != 0) */
        }

        if(retcode == ERRCODE_NONE) {
            sysconfig->init_params_fnv1a_hash = krn_schdrmsg_calc_hash(sysconfig, ((uint32_t)sizeof(visorc_init_params_t)) - 4U, FNV1A_32_INIT_HASH);
            retcode = krn_cache_clean(&sysconfig->init_params_fnv1a_hash, sizeof(sysconfig->init_params_fnv1a_hash));
            if(retcode == ERRCODE_NONE) {
                /* trigger ORC to start */
                krn_module_printU5("start visorc\n", 0U, 0U, 0U, 0U, 0U);
                retcode = krn_visorc_start(arg->coprocessor_cfg & 0x100U, sysconfig);
                krn_visorc_kick();
                krn_schdrmsg_set_event_time();
            }
        }
    }
    return retcode;
}

static uint32_t krn_schdr_start_non_bootos(const visorc_init_params_t *arg, visorc_init_params_t *sysconfig, const volatile uint32_t *slock)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) arg;
    (void) slock;

    retcode = krn_cache_invalidate(sysconfig, sizeof(*sysconfig));
    if(retcode == ERRCODE_NONE) {
        /* wait for spin-lock from booting sub-scheduler */
        krn_module_printU5("waiting for spin lock\n", 0U, 0U, 0U, 0U, 0U);
        while (*slock != 0xBABEFACEU) {
            if( krn_sleep(10) != ERRCODE_NONE) {
                krn_module_printU5("[ERROR] krn_schdr_start_non_bootos() : krn_sleep fail", 0U, 0U, 0U, 0U, 0U);
            }
            retcode = krn_cache_invalidate(&sysconfig->rtos_spin_lock, sizeof(sysconfig->rtos_spin_lock));
            if(retcode != ERRCODE_NONE) {
                break;
            }
        }

        if(retcode == ERRCODE_NONE) {
            krn_module_printU5("setup message queue\n", 0U, 0U, 0U, 0U, 0U);
            sysconfig->MsgQueue_orc_to_arm_daddr[scheduler_id] = (uint32_t)qconfig.reqQ_addr;
            sysconfig->MsgQueue_arm_to_orc_daddr[scheduler_id] = (uint32_t)qconfig.rplQ_addr;
            retcode = krn_cache_clean(&sysconfig->MsgQueue_orc_to_arm_daddr[scheduler_id], sizeof(sysconfig->MsgQueue_orc_to_arm_daddr[scheduler_id]));
            if(retcode == ERRCODE_NONE) {
                retcode = krn_cache_clean(&sysconfig->MsgQueue_arm_to_orc_daddr[scheduler_id], sizeof(sysconfig->MsgQueue_arm_to_orc_daddr[scheduler_id]));
            }

            if(retcode == ERRCODE_NONE) {
                /* Reset internal flexidag kernel state */
                retcode = krn_flexidag_system_reset(1U);
                krn_schdrmsg_set_event_time();
                if(sysconfig->flexidag_num_slots != arg->flexidag_num_slots) {
                    retcode = ERR_DRV_SCHDR_FLEXIDAG_SLOT_MISMATCHED;
                    krn_printU5("[ERROR] krn_schdr_start_non_bootos() : flexidag slot is not the same, sys slot (%d), local slot (%d)\n", sysconfig->flexidag_num_slots, arg->flexidag_num_slots, 0U, 0U, 0U);
                }
            }
        }
    }
    return retcode;
}

uint32_t krn_schdr_start_visorc(const visorc_init_params_t *arg)
{
    static char krx_stack[KRX_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    uint32_t retcode = ERRCODE_NONE;
    visorc_init_params_t *sysconfig;
    volatile uint32_t *slock;
    static char name[64] = "krn_schdrmsg_rx";

    (void) arg;
    if(arg == NULL) {
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_schdr_start_visorc() : arg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        /*  Reset eventflag */
        schdr_isr_num = 0U;
        retcode = krn_eventflag_clear(&rx_lock,EFLAG_MASK);
        if(retcode != ERRCODE_NONE) {
            krn_printU5("[ERROR] krn_schdr_start_visorc() : krn_eventflag_clear rx_lock fail", 0U, 0U, 0U, 0U, 0U);
        }
        retcode = krn_eventflag_clear(&msg_wq,EFLAG_MASK);
        if(retcode != ERRCODE_NONE) {
            krn_printU5("[ERROR] krn_schdr_start_visorc() : krn_eventflag_clear msg_wq fail", 0U, 0U, 0U, 0U, 0U);
        }

        sysconfig = krn_ambacv_get_sysinit();
        slock = (volatile uint32_t *) &sysconfig->rtos_spin_lock;

        if ((arg->coprocessor_cfg & SCHDR_NO_INTERNAL_THREAD) != 0U) {
            krn_schdr_with_thread = 0U;
        }
        is_booting_os = arg->coprocessor_cfg & 0xFFU;
        if (is_booting_os != 0U) {
            retcode = krn_schdr_start_bootos(arg, sysconfig, slock);
        } else {
            retcode = krn_schdr_start_non_bootos(arg, sysconfig, slock);
        }

        if(retcode == ERRCODE_NONE) {
            /* start armvis_msg_t recevier thread */
            rx_kthread_state = 1U;
            if(krn_schdr_with_thread == 1U) {
                prx_kthread = krn_thread_create(prx_kthread, krn_schdrmsg_rx_entry, NULL,
                                                KRX_THREAD_PRIO, 0x1, KRX_STACK_SIZE, krx_stack, name);
            }
        }
    }

    return retcode;
}

uint32_t krn_schdr_resume_visorc(visorc_init_params_t *arg)
{
    uint32_t retcode = ERRCODE_NONE;
    const visorc_init_params_t *sysconfig;
    void *ptr;

    if(arg == NULL) {
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_schdr_resume_visorc() : arg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        sysconfig = krn_ambacv_get_sysinit();
        krn_typecast(&ptr,&sysconfig);
        krn_module_printU5("[CVFLOW] : Restoring kernel sysconfig (0x%x)\n", krn_v2c(ptr), 0U, 0U, 0U, 0U);

        retcode = krn_memcpy(arg, sysconfig, sizeof(*sysconfig));
        if(krn_schdr_with_thread == 0U) {
            arg->coprocessor_cfg |= SCHDR_NO_INTERNAL_THREAD;
        }
    }

    return retcode;
}

uint32_t  krn_schdr_stop_visorc(void)
{
    krn_schdrmsg_shutdown();
    return ERRCODE_NONE;
}

uint32_t krn_schdr_find_entry(ambacv_get_value_t *arg)
{
    cvmem_messagepool_t *pool;
    cvmem_messagebuf_t *entry, *found;
    const char *base;
    const void *ptr = NULL;
    uint32_t pos = 0U, top = 0U, end = 0U, step = 0U;
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;

    (void) arg;
    if(arg == NULL) {
        krn_printU5("[ERROR] krn_schdr_find_entry() : arg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        ptr = krn_c2v(arg->index);
        if(ptr == NULL) {
            krn_printU5("[ERROR] krn_schdr_find_entry() : ptr = krn_c2v((uint32_t)entry_addr); fail\n", 0U, 0U, 0U, 0U, 0U);
        } else {
            krn_typecast(&pool,&ptr);
            retcode = krn_cache_invalidate(pool, sizeof(*pool));
            found = NULL;

            if ((pool->hdr_variable.message_num != 0U) && (retcode == ERRCODE_NONE)) {
                pos = pool->hdr_variable.wr_index;
                end = pool->hdr_variable.wr_index;
                top = pool->hdr_variable.message_num;
                step = pool->hdr_variable.message_buffersize;
                if(pool->hdr_variable.messagepool_basedaddr != 0U) {
                    ptr = krn_c2v(pool->hdr_variable.messagepool_basedaddr);
                    krn_typecast(&base,&ptr);

                    retcode = krn_mutex_lock(&find_lock);
                    if(retcode == ERRCODE_NONE) {
                        do {
                            ptr = &base[pos * step];
                            krn_typecast(&entry,&ptr);
                            retcode = krn_cache_invalidate(&entry->hdr_variable, sizeof(entry->hdr_variable));

                            if (++pos == top) {
                                pos = 0U;
                            }

                            if ((retcode == ERRCODE_NONE) && (entry->hdr_variable.message_in_use == entry->hdr_variable.message_processed)) {
                                //printk("find entry at index %d %d %d", top,end,pos);
                                pool->hdr_variable.wr_index = pos;
                                entry->hdr_variable.message_in_use    = 1U;
                                entry->hdr_variable.message_processed = 0U;
                                retcode = krn_cache_clean(&entry->hdr_variable, sizeof(entry->hdr_variable));
                                found = entry;
                                break;
                            }
                        } while (pos != end);
                        ret = krn_mutex_unlock(&find_lock);
                        if(retcode == ERRCODE_NONE) {
                            retcode = ret;
                        }
                    }
                }
            }

            if(found != NULL) {
                arg->value = krn_v2c(found);
            } else {
                retcode = ERR_DRV_SCHDR_MSG_ENTRY_UNABLE_TO_FIND;
            }
        }
    }
    return retcode;
}

uint32_t krn_schdr_autorun_info(void)
{
    uint32_t retcode = ERRCODE_NONE;
    const autorun_trace_t *pAutoRunTraceEntry = NULL;
    uint32_t  num_entries = 0U;

    krn_typecast(&pAutoRunTraceEntry,&scheduler_support.vpAutoRunTrace);
    if(pAutoRunTraceEntry != NULL) {
        num_entries = pAutoRunTraceEntry->autorun_cvtask_num;
        if (num_entries == 0U) {
            krn_printU5("[AUTORUN] : No AutoRUN tasks exist in the system ", 0U, 0U, 0U, 0U, 0U);
        } /* if (num_entries == 0) */
        else if (num_entries < MAX_AUTORUN_CVTASKS) {
            uint32_t  loop = 0U;

            krn_printU5("--- AUTORUN_CVTASK_NAME -------- : per(ms) | ptime(ms) |   num_pass @(  passtime) |   num_fail @(  failtime) | failrc", 0U, 0U, 0U, 0U, 0U);
            for (loop = 0U; loop < num_entries; loop++) {
                char  stringbuf[256];
                uint32_t   numwritten = 0U;

                numwritten += krn_snprintf_str1(&stringbuf[numwritten], ((uint32_t)sizeof(stringbuf) - numwritten), "%32s :   ", &pAutoRunTraceEntry->autorun_cvtask_name[0]);
                numwritten += krn_snprintf_uint5(&stringbuf[numwritten], ((uint32_t)sizeof(stringbuf) - numwritten), "%5u |     %5u | %10u @(%10u) | %10u @", pAutoRunTraceEntry->autorun_period_ms,
                                                 pAutoRunTraceEntry->autorun_timeout_ms,
                                                 pAutoRunTraceEntry->autorun_num_pass,
                                                 pAutoRunTraceEntry->autorun_last_pass_tick,
                                                 pAutoRunTraceEntry->autorun_num_fail);
                numwritten += krn_snprintf_uint2(&stringbuf[numwritten], ((uint32_t)sizeof(stringbuf) - numwritten), "(%10u) | 0x%x\n", pAutoRunTraceEntry->autorun_last_fail_tick, pAutoRunTraceEntry->autorun_last_fail_retcode);
                (void) numwritten;
                krn_printS5("%s", stringbuf, NULL, NULL, NULL, NULL);
                pAutoRunTraceEntry++;
            } /* for (loop = 0; loop < num_entries; loop++) */
        } /* if (num_entries < MAX_AUTORUN_CVTASKS) */
        else { /* if (num_entries >= MAX_AUTORUN_CVTASKS) */
            krn_printU5("[AUTORUN] : System corrupted : MAX_AUTORUN_CVTASKS = %d, num_entries in table = %d\n", MAX_AUTORUN_CVTASKS, num_entries, 0U, 0U, 0U);
        } /* if (num_entries >= MAX_AUTORUN_CVTASKS) */
    }
    return retcode;
}

