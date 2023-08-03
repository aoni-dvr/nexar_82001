// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
*  Copyright (c) 2021 Ambarella International LP
*/

#include "os_kernel.h"
#include "cavalry_ioctl.h"
#include "ambacv_kal.h"
#include "visorc_kernel.h"
#include "idsporc_kernel.h"
#include "vinorc_kernel.h"
#include "cache_kernel.h"
#include "msg_kernel.h"
#include "schdr_kernel.h"
#include "flexidag_kernel.h"
#if defined(ENABLE_AMBA_MAL)
#include "AmbaMAL.h"
#endif

static uint32_t             ambacv_global_init = 0U;
static ksem_t               ambacv_core_mutex;
static visorc_init_params_t *ambacv_sysinit_vaddr;
uint32_t ambacv_state = 0U;
/*------------------------------------------------------------------------------------------------*/
/*-= AmbaCV core token management (IOCTL/KERNEL) =------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
#define   MAX_AMBACV_TOKENS         32U
#define   AMBACV_TOKEN_VALUE_BASE   AMBACV_ID_WAIT_SCHDR_TOKEN_BASE

typedef struct {
    kmutex_t                token_mutex;
    ksem_t                  token[MAX_AMBACV_TOKENS];
    uint32_t                next_token_index;
    uint8_t                 token_state[MAX_AMBACV_TOKENS];
    errcode_enum_t          token_retcode[MAX_AMBACV_TOKENS];
} ambacv_token_state_t;

#define AMBACV_TOKEN_UNUSED     0U
#define AMBACV_TOKEN_USED       1U
#define AMBACV_TOKEN_WAITING    2U
#define AMBACV_TOKEN_FINISHED   3U

static ambacv_token_state_t ambacv_token_state;

static uint32_t krn_ambacv_token_init(void)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  ret = ERRCODE_NONE;
    uint32_t  loop;

    retcode = krn_memset(&ambacv_token_state, 0, sizeof(ambacv_token_state));
    if(retcode == ERRCODE_NONE) {
        retcode = krn_mutex_init(&ambacv_token_state.token_mutex);
    }

    if(retcode == ERRCODE_NONE) {
        retcode = krn_mutex_lock(&ambacv_token_state.token_mutex);
        if(retcode == ERRCODE_NONE) {
            for (loop = 0U; loop < MAX_AMBACV_TOKENS; loop++) {
                ambacv_token_state.token_state[loop]   = AMBACV_TOKEN_UNUSED;
                ambacv_token_state.token_retcode[loop] = ERRCODE_NONE;
                retcode = krn_sem_init(&(ambacv_token_state.token[loop]), 0, 0U);
                if(retcode != ERRCODE_NONE) {
                    break;
                }
            } /* for (loop = 0; loop < MAX_AMBACV_TOKENS; loop++) */
            ret = krn_mutex_unlock(&ambacv_token_state.token_mutex);
            if(retcode == ERRCODE_NONE) {
                retcode = ret;
            }
        }
    }

    return retcode;
} /* krn_ambacv_token_init() */

uint32_t krn_ambacv_find_token(uint32_t *token)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  token_found = 0U;
    uint32_t  token_num = 0U;
    uint32_t  token_id = 0U;
    uint32_t  loop = 0U;

    if(token == NULL) {
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_ambacv_find_token() : token == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        retcode = krn_mutex_lock(&ambacv_token_state.token_mutex);
        if(retcode == ERRCODE_NONE) {
            while ((token_found == 0U) && (loop < MAX_AMBACV_TOKENS)) {
                token_id = (ambacv_token_state.next_token_index + loop) % MAX_AMBACV_TOKENS;
                if (ambacv_token_state.token_state[token_id] == (uint8_t)AMBACV_TOKEN_UNUSED) {
                    token_found = 1U;
                    token_num   = token_id;
                    ambacv_token_state.token_state[token_id] = AMBACV_TOKEN_USED;
                    ambacv_token_state.next_token_index      = (token_id + 1U) % MAX_AMBACV_TOKENS;
                } /* if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_UNUSED) */
                loop++;
            } /* while ((token_found == 0) && (loop < MAX_AMBACV_TOKENS)) */
            retcode = krn_mutex_unlock(&ambacv_token_state.token_mutex);
        }

        if (token_found == 0U) {
            /* ERROR */
            krn_printU5("[ERROR] krn_ambacv_find_token() : Unable to find token in system\n",0U,0U,0U,0U,0U);
            retcode = ERR_DRV_SCHDR_TOKEN_UNABLE_TO_FIND;
        } /* if (token_found == 0) */
        else { /* if (token_found != 0) */
            *token = token_num + AMBACV_TOKEN_VALUE_BASE;
        } /* if (token_found != 0) */
    }

    return retcode;
} /* krn_ambacv_find_token() */

uint32_t krn_ambacv_wait_token(uint32_t token_id)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  ret = ERRCODE_NONE;
    uint32_t  wait_valid = 0U;
    uint32_t  token_num = token_id - AMBACV_TOKEN_VALUE_BASE;

    retcode = krn_mutex_lock(&ambacv_token_state.token_mutex);
    if(retcode == ERRCODE_NONE) {
        if ((token_id < AMBACV_TOKEN_VALUE_BASE) || (token_id >= (AMBACV_TOKEN_VALUE_BASE + MAX_AMBACV_TOKENS))) {
            /* ERROR */
            wait_valid  = 0U;
            krn_printU5("[ERROR] : krn_ambacv_wait_token(token_id=%3d) : Token is out of range", token_id, 0U, 0U, 0U, 0U);
            retcode     = ERR_DRV_SCHDR_TOKEN_OUT_OF_RANGE;
        } /* if (token_id >= MAX_AMBACV_TOKENS) */
        else { /* if (token_id < MAX_AMBACV_TOKENS) */
            if (ambacv_token_state.token_state[token_num] == (uint8_t)AMBACV_TOKEN_FINISHED) { /* release called before wait */
                wait_valid = 1;
            } /* if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_FINISHED) */
            else if (ambacv_token_state.token_state[token_num] == (uint8_t)AMBACV_TOKEN_UNUSED) {
                wait_valid = 0; /* Do nothing here */
            } /* if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_UNUSED) */
            else if (ambacv_token_state.token_state[token_num] == (uint8_t)AMBACV_TOKEN_USED) { /* wait called before release */
                ambacv_token_state.token_state[token_num] = (uint8_t)AMBACV_TOKEN_WAITING;
                wait_valid = 1;
            } /* if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_USED) */
            else { /* if (ambacv_core_state.token_state[token_id] == [invalid state]) */
                /* ERROR */
                wait_valid  = 0;
                krn_printU5("[ERROR] : krn_ambacv_wait_token(token_id=%3d) : Token state invalid (%d, expected %d)\n",
                            token_num,  ambacv_token_state.token_state[token_num], AMBACV_TOKEN_USED, 0U, 0U);
                retcode     = ERR_DRV_SCHDR_TOKEN_STATE_MISMATCHED;
            } /* if (ambacv_core_state.token_state[token_id] == [invalid state]) */
        } /* if (token_id < MAX_AMBACV_TOKENS) */
        ret = krn_mutex_unlock(&ambacv_token_state.token_mutex);
        if(retcode == ERRCODE_NONE) {
            retcode = ret;
        }
    }

    if (wait_valid != 0U) {
        retcode = krn_sem_wait(&ambacv_token_state.token[token_num], TOKEN_WAIT_TIMEOUT);
        if(retcode != ERRCODE_NONE) {
            krn_printU5("[ERROR] krn_ambacv_wait_token() : krn_sem_wait fail token_id %d ret 0x%x \n", token_num, retcode, 0U, 0U, 0U);
        }

        retcode = krn_mutex_lock(&ambacv_token_state.token_mutex);
        if(retcode == ERRCODE_NONE) {
            if (ambacv_token_state.token_state[token_num] == (uint8_t)AMBACV_TOKEN_FINISHED) {
                retcode = ambacv_token_state.token_retcode[token_num];
                if(retcode != ERRCODE_NONE) {
                    krn_printU5("[ERROR] krn_ambacv_wait_token() : token_retcode fail token_id %d ret 0x%x \n", token_num, retcode, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_VISMSG_RET_FAIL;
                }
            } /* if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_FINISHED) */
            else { /* if (ambacv_core_state.token_state[token_id] != AMBACV_TOKEN_FINISHED) */
                krn_printU5("[ERROR] krn_ambacv_wait_token() : (token_id=%3d) : Token state invalid (%d, expected %d)\n",
                            token_num, ambacv_token_state.token_state[token_num], AMBACV_TOKEN_FINISHED, 0U, 0U);
                retcode = ERR_DRV_SCHDR_TOKEN_STATE_MISMATCHED;
            } /* if (ambacv_core_state.token_state[token_id] != AMBACV_TOKEN_FINISHED) */
            ambacv_token_state.token_state[token_num] = (uint8_t)AMBACV_TOKEN_UNUSED;
            ret = krn_mutex_unlock(&ambacv_token_state.token_mutex);
            if(retcode == ERRCODE_NONE) {
                retcode = ret;
            }
        }
    } /* if (wait_valid != 0) */

    return retcode;
} /* ambacv_wait_token() */

uint32_t krn_ambacv_release_token(uint32_t token_id, uint32_t function_retcode)
{
    uint32_t  release_valid = 0U;
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  ret = ERRCODE_NONE;
    uint32_t  token_num = token_id - AMBACV_TOKEN_VALUE_BASE;

    if ((token_id < AMBACV_TOKEN_VALUE_BASE) || (token_id >= (AMBACV_TOKEN_VALUE_BASE + MAX_AMBACV_TOKENS))) {
        /* ERROR */
        krn_printU5("[ERROR] : krn_ambacv_release_token(token_id=%3d) : Token is out of range", token_id, 0U, 0U, 0U, 0U);
        retcode     = ERR_DRV_SCHDR_TOKEN_OUT_OF_RANGE;
    } /* if (token_id >= MAX_AMBACV_TOKENS) */
    else { /* if (token_id < MAX_AMBACV_TOKENS) */
        retcode = krn_mutex_lock(&ambacv_token_state.token_mutex);
        if(retcode == ERRCODE_NONE) {
            if (ambacv_token_state.token_state[token_num] == (uint8_t)AMBACV_TOKEN_USED) { /* release is called before wait */
                release_valid = 1U;
            } /* if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_USED) */
            else if (ambacv_token_state.token_state[token_num] == (uint8_t)AMBACV_TOKEN_WAITING) { /* wait is called before release */
                release_valid = 1U;
            } /* if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_WAITING) */
            else { /* invalid state */
                release_valid = 0U;
                krn_printU5("[ERROR] : krn_ambacv_release_token(token_id=%3d, retcode=0x%x) : Token state invalid (%d, expected %d)\n",
                            token_num, function_retcode, ambacv_token_state.token_state[token_num], AMBACV_TOKEN_WAITING, 0U);
                retcode       = ERR_DRV_SCHDR_TOKEN_STATE_MISMATCHED;
            } /* invalid state */

            if (release_valid != 0U) {
                ambacv_token_state.token_state[token_num]       = (uint8_t)AMBACV_TOKEN_FINISHED;
                ambacv_token_state.token_retcode[token_num]     = function_retcode;
            } /* if (release_valid != 0) */
            ret = krn_mutex_unlock(&ambacv_token_state.token_mutex);
            if(retcode == ERRCODE_NONE) {
                retcode = ret;
            }
        }

        if (release_valid != 0U) {
            retcode = krn_sem_post(&ambacv_token_state.token[token_num]);
        }
    } /* if (token_id < MAX_AMBACV_TOKENS) */
    return retcode;
} /* ambacv_release_token() */

void krn_ambacv_set_sysinit(visorc_init_params_t *ptr)
{
    ambacv_sysinit_vaddr = ptr;
}

visorc_init_params_t *krn_ambacv_get_sysinit(void)
{
    return ambacv_sysinit_vaddr;
}

ambacv_log_t *krn_ambacv_get_log(void)
{
    static ambacv_log_t ambacv_global_log;

    return &ambacv_global_log;
}

ambacv_all_mem_t *krn_ambacv_get_mem(void)
{
    static ambacv_all_mem_t ambacv_global_mem;

    return &ambacv_global_mem;
}

static uint32_t krn_ambacv_get_att(ambacv_att_region_t *arg)
{
    uint32_t retcode = ERRCODE_NONE;
    const ambacv_all_mem_t *mem_all;

    if(arg == NULL) {
        krn_printU5("[ERROR] krn_ambacv_get_att() : arg == NULL", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        mem_all = krn_ambacv_get_mem();
        arg->ca = mem_all->cv_att_region.base;
        arg->pa = (uint64_t) krn_c2p(mem_all->cv_att_region.base);
        arg->size = mem_all->cv_att_region.size;
    }

    return retcode;
}

static uint32_t krn_ambacv_get_value(ambacv_get_value_t *arg)
{
    uint32_t retcode = ERRCODE_NONE;

    if (arg->index == AMBACV_ID_GET_SCHDR_TRACE) {
        arg->value = krn_schdr_get_trace_daddr();
    } /* if (arg == AMBACV_ID_GET_SCHDR_TRACE) */
    else if (arg->index == AMBACV_ID_GET_FLEXIDAG_TRACE) {
        arg->value = krn_flexidag_system_get_trace_daddr();
    } /* if (arg == AMBACV_ID_GET_FLEXIDAG_TRACE) */
    else if (arg->index == AMBACV_ID_GET_AUTORUN_TRACE) {
        arg->value = krn_schdr_get_autorun_trace_daddr();
    } /* if (arg == AMBACV_ID_GET_AUTORUN_TRACE) */
    else if (arg->index == AMBACV_ID_GET_CAVALRY_TRACE) {
        arg->value = 0U;
    } /* if (arg == AMBACV_ID_GET_FLEXIDAG_TRACE) */
    else if (arg->index == AMBACV_ID_GET_ORC_PERF_WI_BASE) {
        retcode = krn_cache_invalidate(&krn_print_info->visorc_perf_debugbuf[0].last_wridx_daddr, sizeof(uint32_t));
        if (retcode == ERRCODE_NONE) {
            arg->value = (uint64_t)krn_print_info->visorc_perf_debugbuf[0].last_wridx_daddr;
        }
    } /* if (arg == AMBACV_ID_GET_ORC_PERF_WI_BASE) */
    else if (arg->index == AMBACV_ID_GET_AUDIO_CLOCK) {
        uint32_t freq = 0U;

        retcode = krn_get_clock(AUDIO_CLOCK_ID, &freq);
        if (retcode == ERRCODE_NONE) {
            arg->value = (uint64_t)freq;
        }
    } /* if (arg == AMBACV_ID_GET_AUDIO_CLOCK) */
    else if (arg->index == AMBACV_ID_GET_EVENT_TIME) {
        arg->value = (uint64_t)krn_schdrmsg_get_event_time();
    } /* if (arg == AMBACV_ID_GET_EVENT_TIME) */
    else if (arg->index == AMBACV_ID_GET_CLUSTER_ID) {
        arg->value = (uint64_t)cluster_id;
    } /* if (arg == AMBACV_ID_GET_CLUSTER_ID) */
    else if (arg->index == AMBACV_ID_GET_ORC_CMD_WI) { /* Get orc wi */
        arg->value = krn_schdrmsg_get_orcarm_wi();
    } /* if (ambacv_value.index == AMBACV_ID_GET_ORC_CMD_WI) */
    else if (arg->index == AMBACV_ID_GET_SCHDR_TOKEN) { /* Find and return core token */
        uint32_t  token_id;

        retcode = krn_ambacv_find_token(&token_id); /* Repurpose this system for the token here */
        if (retcode == ERRCODE_NONE) {
            arg->value = token_id;
        }
    } /* if (ambacv_value.index == AMBACV_ID_GET_SCHDR_TOKEN) */
    else if ((arg->index >= AMBACV_ID_WAIT_SCHDR_TOKEN_BASE) && (arg->index < (AMBACV_ID_WAIT_SCHDR_TOKEN_BASE + MAX_AMBACV_TOKENS))) {
        retcode = krn_ambacv_wait_token((uint32_t)arg->index); /* Wait on token */
    } /* if ((ambacv_value.index >= AMBACV_ID_WAIT_SCHDR_TOKEN_BASE) && (ambacv_value.index < (AMBACV_ID_WAIT_SCHDR_TOKEN_BASE + MAX_AMBACV_TOKENS))) */
    else { /* if (arg != [0..2]) */
        krn_printU5("[ERROR] krn_ambacv_get_value() : unknow index index %d ", arg->index, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } /* if (arg != [0..2]) */

    return retcode;
}

static uint32_t krn_ambacv_schdr_part1_ioctl(const void* f, uint32_t cmd, void * arg, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;
    visorc_init_params_t *arg_init_param;
    ambacv_get_value_t *arg_get_value;
    const armvis_msg_t *arg_vis_msg;
    ambacv_mem_t *arg_mem;
    const ambacv_mem_region_t *arg_mem_region;
    const ambacv_log_t *arg_log;
    const ambacv_all_mem_t *pmem_all;
    ambacv_log_t *plog;
    const ambacv_idspvis_msg_t *arg_idspvis_msg;
    const ambacv_vinvis_msg_t *arg_vinvis_msg;
    const ambacv_cfg_addr_t *arg_cfg_addr;

    (void) f;
    (void) arg;
    *hit = 1U;
    switch (cmd) {
    case AMBACV_START_VISORC:
        ambacv_state = 2U;
        krn_typecast(&arg_init_param, &arg);
        retcode = krn_schdr_start_visorc(arg_init_param);
        break;
    case AMBACV_FIND_CVTASKMSG:
        krn_typecast(&arg_get_value, &arg);
        retcode = krn_schdr_find_entry(arg_get_value);
        break;
    case AMBACV_GET_SYSCONFIG:
        krn_typecast(&arg_init_param, &arg);
        retcode = krn_schdr_get_sysconfig(arg_init_param);
        break;
    case AMBACV_RECV_ORCMSG:
        krn_typecast(&arg_get_value, &arg);
        retcode = krn_schdrmsg_recv_orcmsg(arg_get_value);
        break;
    case AMBACV_SEND_ARMMSG:
        krn_typecast(&arg_vis_msg, &arg);
        retcode = krn_schdrmsg_send_armmsg(arg_vis_msg, 0U);
        break;
    case AMBACV_GET_MEM_LAYOUT:
        krn_typecast(&arg_mem, &arg);
        pmem_all = krn_ambacv_get_mem();
        retcode = krn_memcpy(arg_mem,pmem_all,sizeof(ambacv_mem_t));
        break;
    case AMBACV_UCODE_LOADED:
        krn_typecast(&arg_mem, &arg);
        retcode = krn_visorc_finalize_loading(arg_mem);
        break;
    case AMBACV_STOP_VISORC:
        if(ambacv_state == 1U) {
            ambacv_state = 0U;
            retcode = krn_schdr_stop_visorc();
        }
        break;
    case AMBACV_SEND_VPMSG:
        krn_typecast(&arg_idspvis_msg, &arg);
        retcode = krn_idsporc_send_vpmsg(arg_idspvis_msg);
        break;
    case AMBACV_CONFIG_IDSP:
        krn_typecast(&arg_cfg_addr, &arg);
        retcode = krn_idsporc_config(arg_cfg_addr);
        break;
    case AMBACV_SEND_VINMSG:
        krn_typecast(&arg_vinvis_msg, &arg);
        retcode = krn_vinorc_send_vinmsg(arg_vinvis_msg);
        break;
    case AMBACV_CONFIG_VIN:
        krn_typecast(&arg_cfg_addr, &arg);
        retcode = krn_vinorc_config(arg_cfg_addr);
        break;
    case AMBACV_CACHE_CLEAN:
        krn_typecast(&arg_mem_region, &arg);
        retcode = krn_cache_ioctl_clean(arg_mem_region);
        break;
    case AMBACV_CACHE_INVLD:
        krn_typecast(&arg_mem_region, &arg);
        retcode = krn_cache_ioctl_invld(arg_mem_region);
        break;
    case AMBACV_SET_BIN_PATH:
        krn_typecast(&arg_log, &arg);
        plog = krn_ambacv_get_log();
        retcode = krn_memcpy(plog->binary_path, arg_log->binary_path, 256);
        krn_module_printS5("Set bin path %s\n", plog->binary_path, NULL, NULL, NULL, NULL);
        break;
    default:
        *hit = 0U;
        break;
    }
    return retcode;
}

static uint32_t krn_ambacv_schdr_part2_ioctl(const void* f, uint32_t cmd, void * arg, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;
    visorc_init_params_t *arg_init_param;
    ambacv_get_value_t *arg_get_value;
    uint32_t *arg_value;
    ambacv_att_region_t *arg_att_region;
    ambacv_log_t *arg_log;
    const ambacv_log_t *plog;
    const ambacv_asynvis_msg_t *arg_asynvis_msg;

    (void) f;
    (void) arg;
    *hit = 1U;
    switch (cmd) {
    case AMBACV_GET_LOG_INFO:
        krn_typecast(&arg_log, &arg);
        plog = krn_ambacv_get_log();
        krn_module_printS5("Get bin path %s\n", plog->binary_path, NULL, NULL, NULL, NULL);
        retcode = krn_memcpy(arg_log, plog, sizeof(ambacv_log_t));
        break;
    case AMBACV_SEND_ASYNCMSG:
        krn_typecast(&arg_asynvis_msg, &arg);
        retcode = krn_idsporc_send_asyncmsg(arg_asynvis_msg);
        break;
    case AMBACV_GET_VERSION:
        krn_typecast(&arg_value, &arg);
        *arg_value = ((uint32_t)CVSCHEDULER_INTERFACE_VERSION << 8U);
        *arg_value = (*arg_value | CVSCHEDULER_KERNEL_VERSION);
        break;
    case AMBACV_GET_MAL_VERSION:
        krn_typecast(&arg_value, &arg);
#if defined(ENABLE_AMBA_MAL)
        *arg_value = (uint32_t)AMBA_MAL_VERSION;
#else
        *arg_value = 0U;
#endif
        break;
    case AMBACV_GET_VALUE:
        krn_typecast(&arg_get_value, &arg);
        retcode = krn_ambacv_get_value(arg_get_value);
        break;
    case AMBACV_CORE_MUTEX:
        krn_typecast(&arg_value, &arg);
        if (*arg_value == 0U) {
            retcode = krn_sem_wait(&ambacv_core_mutex, GLOBAL_LOCK_WAIT_TIMEOUT);
        } /* if (arg == 0) */
        else { /* if (arg != 0) */
            retcode = krn_sem_post(&ambacv_core_mutex);
        } /* if (arg != 0) */
        break;
    case AMBACV_RESUME_VISORC:
        krn_typecast(&arg_init_param, &arg);
        retcode = krn_schdr_resume_visorc(arg_init_param);
        break;
    case AMBACV_GET_STATE:
        krn_typecast(&arg_value, &arg);
        *arg_value = ambacv_state;
        break;
    case AMBACV_GET_ATT:
        krn_typecast(&arg_att_region, &arg);
        retcode = krn_ambacv_get_att(arg_att_region);
        break;
    case AMBACV_GET_TIME:
        krn_typecast(&arg_value, &arg);
        *arg_value = krn_visorc_get_curtime();
        break;
    case AMBACV_WAIT_ORCMSG:
        krn_typecast(&arg_value, &arg);
        retcode = krn_schdrmsg_rx_wait(arg_value);
        break;
    default:
        *hit = 0U;
        break;
    }
    return retcode;

}

static uint32_t krn_ambacv_cavalry_ioctl(const void* f, uint32_t cmd, const void * arg, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;

    (void)f;
    (void)arg;
    *hit = 1U;
    switch (cmd) {
    /*-= Superdag/Cavalry specific IOCTL =------------------------------------*/
    case CAVALRY_ENABLE:
        break;
    case CAVALRY_ASSOCIATE_BUF:
        break;
    case CAVALRY_SET_MEMORY_BLOCK: {
        armvis_msg_t msgbase;
        schedmsg_cavalry_setup_t *pSetupMsg;

        msgbase.hdr.message_type    = (uint16_t)SCHEDMSG_CAVALRY_SETUP;
        msgbase.hdr.message_type    |= MSGTYPEBIT_ARM_TO_VISORC;
        pSetupMsg                   = &msgbase.msg.cavalry_setup;
        pSetupMsg->log_base_daddr   = 0U;
        pSetupMsg->log_size         = 0U;
        retcode = krn_schdrmsg_send_armmsg(&msgbase, 1U);
    }
    break;
    case CAVALRY_HANDLE_VISORC_REPLY:
        retcode = 0U; // Moved the handler for this into the kernel
        break;

    default:
        *hit = 0U;
        break;
    }
    return retcode;

}

static uint32_t krn_ambacv_cma_ioctl(const void* f, uint32_t cmd,const void * arg, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t owner = 0UL;

    (void)f;
    *hit = 1U;
    switch (cmd) {
    /*-= Cavalry CMA specific IOCTL =---------------------------------------------*/
    case CAVALRY_ALLOC_MEM:
        krn_typecast(&owner, &f);
        retcode = krn_cma_alloc(arg, owner);
        break;
    case CAVALRY_FREE_MEM:
        retcode = krn_cma_free(arg);
        break;
    case CAVALRY_SYNC_CACHE_MEM:
        retcode = krn_cma_sync_cache(arg);
        break;
    case CAVALRY_GET_USAGE_MEM:
        retcode = krn_cma_get_usage(arg);
        break;
    default:
        *hit = 0U;
        break;
    }
    return retcode;

}

static uint32_t krn_ambacv_flexidag_part1_ioctl(void* f, uint32_t cmd, void * arg, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;

    *hit = 1U;
    switch (cmd) {
    /*-= Flexidag specific IOCTL =--------------------------------------------*/
    case AMBACV_FLEXIDAG_ENABLE:
    case AMBACV_FLEXIDAG_CREATE:
    case AMBACV_FLEXIDAG_CREATE_QUERY:
    case AMBACV_FLEXIDAG_SET_PARAMETER:
    case AMBACV_FLEXIDAG_ADD_TBAR:
    case AMBACV_FLEXIDAG_ADD_SFB:
    case AMBACV_FLEXIDAG_OPEN:
    case AMBACV_FLEXIDAG_OPEN_QUERY:
    case AMBACV_FLEXIDAG_SET_STATE_BUFFER:
    case AMBACV_FLEXIDAG_SET_TEMP_BUFFER:
    case AMBACV_FLEXIDAG_INIT:
    case AMBACV_FLEXIDAG_INIT_QUERY:
        retcode = krn_flexidag_ioctl_process(f, cmd, arg);
        break;
    default:
        *hit = 0U;
        break;
    }
    return retcode;

}

static uint32_t krn_ambacv_flexidag_part2_ioctl(void* f, uint32_t cmd, void * arg, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;

    *hit = 1U;
    switch (cmd) {
    /*-= Flexidag specific IOCTL =--------------------------------------------*/
    case AMBACV_FLEXIDAG_PREP_RUN:
    case AMBACV_FLEXIDAG_SET_INPUT_BUFFER:
    case AMBACV_FLEXIDAG_SET_FEEDBACK_BUFFER:
    case AMBACV_FLEXIDAG_SET_OUTPUT_BUFFER:
    case AMBACV_FLEXIDAG_RUN:
    case AMBACV_FLEXIDAG_RUN_QUERY:
    case AMBACV_FLEXIDAG_WAIT_RUN_FINISH:
    case AMBACV_FLEXIDAG_CLOSE:
    case AMBACV_FLEXIDAG_CLOSE_QUERY:
    case AMBACV_FLEXIDAG_INFO:
    case AMBACV_FLEXIDAG_HANDLE_MSG:
        retcode = krn_flexidag_ioctl_process(f, cmd, arg);
        break;
    default:
        *hit = 0U;
        break;
    }
    return retcode;

}

uint32_t krn_ambacv_ioctl(void* f, uint32_t cmd, void * arg)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t hit;

    (void) f;
    retcode = krn_ambacv_schdr_part1_ioctl(f, cmd, arg, &hit);
    if(hit == 0U) {
        retcode = krn_ambacv_schdr_part2_ioctl(f, cmd, arg, &hit);
        if(hit == 0U) {
            retcode = krn_ambacv_cavalry_ioctl(f, cmd, arg, &hit);
            if(hit == 0U) {
                retcode = krn_ambacv_cma_ioctl(f, cmd, arg, &hit);
                if(hit == 0U) {
                    retcode = krn_ambacv_flexidag_part1_ioctl(f, cmd, arg, &hit);
                    if(hit == 0U) {
                        retcode = krn_ambacv_flexidag_part2_ioctl(f, cmd, arg, &hit);
                        if(hit == 0U) {
                            krn_printU5("[ERROR] krn_ambacv_ioctl() : unknown IOCTL 0x%x\n", cmd, 0U, 0U, 0U, 0U);
                            retcode = ERR_DRV_SCHDR_IOCTL_UNKNOW;
                        }
                    }
                }
            }
        }
    }
    return retcode;
}

uint32_t krn_ambacv_init(void)
{
    uint32_t irq_num = 0U;
    uint32_t retcode = ERRCODE_NONE;

    if(ambacv_global_init == 0U) {
        retcode = krn_sem_init(&ambacv_core_mutex, 0, 1U);

        /* Initialize os kernel */
        if(retcode == ERRCODE_NONE) {
            retcode = krn_os_kernel_init();
            if(retcode != ERRCODE_NONE) {
                krn_printU5("[ERROR] krn_ambacv_init() : krn_os_kernel_init fail ret 0x%x\n", retcode, 0U, 0U, 0U, 0U);
            }
        }

        /* Initialize ambacv_token system */
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_token_init();
            if(retcode != ERRCODE_NONE) {
                krn_printU5("[ERROR] krn_ambacv_init() : krn_ambacv_token_init fail ret 0x%x\n", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            /* Initialize scheduler */
            retcode = krn_schdrmsg_init();
            if(retcode != ERRCODE_NONE) {
                krn_printU5("[ERROR] krn_ambacv_init() : krn_schdrmsg_init fail ret 0x%x\n", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            /* Initialize flexidag  */
            retcode =  krn_flexidag_system_init();
            if(retcode != ERRCODE_NONE) {
                krn_printU5("[ERROR] krn_ambacv_init() : krn_flexidag_system_init fail ret 0x%x\n", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            /* Initialize visorc */
            retcode = krn_visorc_init();
            if(retcode != ERRCODE_NONE) {
                krn_printU5("[ERROR] krn_ambacv_init() : krn_visorc_init fail ret 0x%x\n", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            /* Enale IRQ*/
            retcode = krn_get_sod_irq(&irq_num);
            if(retcode != ERRCODE_NONE) {
                krn_printU5("[ERROR] krn_ambacv_init() : krn_get_sod_irq fail ret 0x%x\n", retcode, 0U, 0U, 0U, 0U);
            } else {
                retcode = krn_enable_irq(irq_num, krn_schdr_isr);
                if(retcode != ERRCODE_NONE) {
                    krn_printU5("[ERROR] krn_ambacv_init() : krn_enable_irq fail ret 0x%x\n", retcode, 0U, 0U, 0U, 0U);
                } else {
                    krn_module_printU5("ambacv: module init\n", 0U, 0U, 0U, 0U, 0U);
                    ambacv_global_init = 1U;
                }
            }
        }
    }

    return retcode;
}

void krn_ambacv_exit(void)
{
    uint32_t irq_num = 0U;

    if(ambacv_global_init == 1U) {
        /* Disable IRQ*/
        if( krn_get_sod_irq(&irq_num) != ERRCODE_NONE) {
            krn_module_printU5("[ERROR] krn_ambacv_exit: krn_get_sod_irq fail", 0U, 0U, 0U, 0U, 0U);
        } else {
            if( krn_disable_irq(irq_num) != ERRCODE_NONE) {
                krn_module_printU5("[ERROR] krn_ambacv_exit: krn_disable_irq fail", 0U, 0U, 0U, 0U, 0U);
            }
        }

        /* Exit flexidag */
        krn_flexidag_system_shutdown();

        /* Exit scheduler */
        krn_schdrmsg_shutdown();

        /* Exit visorc */
        krn_visorc_shutdown();

        /* Exit os kernel */
        krn_os_kernel_exit();

        krn_module_printU5("ambacv: module exit\n", 0U, 0U, 0U, 0U, 0U);
        ambacv_global_init = 0U;
    }
    return;

}

