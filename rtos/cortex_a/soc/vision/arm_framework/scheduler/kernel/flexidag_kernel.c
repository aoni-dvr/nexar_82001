// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
*  Copyright (c) 2021 Ambarella International LP
*/

#include "os_kernel.h"
#include "ambacv_kal.h"
#include "ambacv_kernel.h"
#include "cache_kernel.h"
#include "msg_kernel.h"
#include "schdr_kernel.h"
#include "flexidag_kernel.h"
#include "visorc_kernel.h"
#include <schdr_api.h> /* For flexidag_(slot_)trace_t structures */
#include <cvsched_sysflow_format.h>   /* for sysflow_entry_t */

#ifdef ASIL_COMPLIANCE
#define   BUFFER_CHECKSUM_SIZE  128U
#endif

#define FLEXIDAG_TOKEN_UNUSED     0U
#define FLEXIDAG_TOKEN_USED       1U
#define FLEXIDAG_TOKEN_WAITING    2U
#define FLEXIDAG_TOKEN_FINISHED   3U

static flexidag_slot_state_t      flexidag_slot_state[FLEXIDAG_MAX_SLOTS] GNU_SECTION_NOZEROINIT;
flexidag_system_support_t  flexidag_system_support;
/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG Token Management (IOCTL/KERNEL) =---------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_mutex_lock(kmutex_t *mutex)
{
    uint32_t  retcode = ERRCODE_NONE;

    if(krn_mutex_lock(mutex) != ERRCODE_NONE) {
        retcode = ERR_DRV_FLEXIDAG_MUTEX_LOCK_FAIL;
    }

    return retcode;
}

static uint32_t krn_flexidag_mutex_unlock(kmutex_t *mutex)
{
    uint32_t  retcode = ERRCODE_NONE;

    if(krn_mutex_unlock(mutex) != ERRCODE_NONE) {
        retcode = ERR_DRV_FLEXIDAG_MUTEX_UNLOCK_FAIL;
    }

    return retcode;
}

static uint32_t krn_flexidag_sem_wait(ksem_t *sem, uint32_t timeout)
{
    uint32_t  retcode = ERRCODE_NONE;

    if(krn_sem_wait(sem, timeout) != ERRCODE_NONE) {
        retcode = ERR_DRV_FLEXIDAG_SEM_WAIT_FAIL;
    }

    return retcode;
}

static uint32_t krn_flexidag_sem_post(ksem_t *sem)
{
    uint32_t  retcode = ERRCODE_NONE;

    if(krn_sem_post(sem) != ERRCODE_NONE) {
        retcode = ERR_DRV_FLEXIDAG_SEM_POST_FAIL;
    }

    return retcode;
}

static uint32_t krn_flexidag_find_token(uint32_t *token)
{
    uint32_t  token_found;
    uint32_t  token_num;
    uint32_t  token_id;
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  loop;

    token_found = 0U;
    token_num   = 0U;
    loop        = 0U;

    if(token == NULL) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_find_token() : token == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        retcode = krn_flexidag_mutex_lock(&flexidag_system_support.token_mutex);
        if(retcode == ERRCODE_NONE) {
            while ((token_found == 0U) && (loop < MAX_FLEXIDAG_TOKENS)) {
                token_id = (flexidag_system_support.next_token_index + loop) % MAX_FLEXIDAG_TOKENS;
                if (flexidag_system_support.token_state[token_id] == (uint8_t)FLEXIDAG_TOKEN_UNUSED) {
                    uint32_t  output_loop;
                    token_found = 1U;
                    token_num   = token_id;
                    flexidag_system_support.token_state[token_id]       = FLEXIDAG_TOKEN_USED;
                    flexidag_system_support.token_retcode[token_id]     = (uint32_t)ERRCODE_NONE;
                    flexidag_system_support.token_start_time[token_id]  = 0U;
                    flexidag_system_support.token_end_time[token_id]    = 0U;
                    flexidag_system_support.token_output_not_generated[token_id] = 0U;
                    for (output_loop = 0U; output_loop < FLEXIDAG_MAX_OUTPUTS; output_loop++) {
                        flexidag_system_support.token_output_donetime[token_id][output_loop] = 0U;
                    }
                    flexidag_system_support.next_token_index        = (token_id + 1U) % MAX_FLEXIDAG_TOKENS;
                } /* if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_UNUSED) */
                loop++;
            } /* while ((token_found == 0) && (loop < MAX_FLEXIDAG_TOKENS)) */
            retcode = krn_flexidag_mutex_unlock(&flexidag_system_support.token_mutex);
        }

        if (token_found == 0U) {
            /* ERROR */
            krn_printU5("[ERROR] krn_flexidag_find_token() : Unable to find token in system\n",0U,0U,0U,0U,0U);
            retcode = ERR_DRV_FLEXIDAG_TOKEN_UNABLE_TO_FIND;
        } /* if (token_found == 0) */
        else { /* if (token_found != 0) */
            *token = token_num;
        } /* if (token_found != 0) */
    }

    return retcode;

} /* krn_flexidag_find_token() */

static uint32_t krn_flexidag_prepare_wait_token(uint32_t token_id, uint32_t block, uint32_t *pwait_valid)
{
    uint32_t  retcode = ERRCODE_NONE;

    if (token_id >= MAX_FLEXIDAG_TOKENS) {
        /* ERROR */
        *pwait_valid  = 0U;
        retcode     = ERR_DRV_FLEXIDAG_TOKEN_OUT_OF_RANGE;
    } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
    else { /* if (token_id < MAX_FLEXIDAG_TOKENS) */
        if (block == 0U) {
            *pwait_valid = 0U;
            flexidag_system_support.token_state[token_id] = FLEXIDAG_TOKEN_FINISHED;
        } /* if ((flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_USED) && (block != 0)) */
        else if (flexidag_system_support.token_state[token_id] == (uint8_t)FLEXIDAG_TOKEN_FINISHED) {
            /* Swap to waiting */
            flexidag_system_support.token_state[token_id] = (uint8_t)FLEXIDAG_TOKEN_WAITING;
            *pwait_valid = 1U;
        } else if ((flexidag_system_support.token_state[token_id] == (uint8_t)FLEXIDAG_TOKEN_UNUSED) && (block > 1U)) {
            *pwait_valid = 0U; /* Do nothing here */
        } /* if ((flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_UNUSED) && (block > 1)) */
        else if (flexidag_system_support.token_state[token_id] != (uint8_t)FLEXIDAG_TOKEN_USED) {
            /* ERROR */
            *pwait_valid  = 0U;
            krn_printU5("[ERROR] krn_flexidag_wait_token() : (token_id=%3d, block=%d) : Token state invalid (%d, expected %d)\n",
                        token_id, block, flexidag_system_support.token_state[token_id], FLEXIDAG_TOKEN_USED, 0U);
            retcode     = ERR_DRV_FLEXIDAG_TOKEN_STATE_MISMATCHED;
        } /* if (flexidag_system_support.token_state[token_id] != FLEXIDAG_TOKEN_USED) */
        else { /* if (block == 0) */
            flexidag_system_support.token_state[token_id] = FLEXIDAG_TOKEN_WAITING;
            *pwait_valid = 1U;
        }
    } /* if (token_id < MAX_FLEXIDAG_TOKENS) */

    if( krn_flexidag_mutex_unlock(&flexidag_system_support.token_mutex) != ERRCODE_NONE) {
        krn_printU5("[ERROR] krn_flexidag_prepare_wait() : krn_flexidag_mutex_unlock fail token_id 0x%x \n", token_id, 0U, 0U, 0U, 0U);
        if(retcode == ERRCODE_NONE) {
            retcode = ERR_DRV_FLEXIDAG_MUTEX_UNLOCK_FAIL;
        }
    }

    return retcode;
}

static uint32_t krn_flexidag_wait_token(uint32_t token_id, uint32_t block, uint32_t timeout)
{
    uint32_t  wait_valid;
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  ret = ERRCODE_NONE;

    retcode = krn_flexidag_prepare_wait_token(token_id, block, &wait_valid);
    if ( (retcode == ERRCODE_NONE) &&(wait_valid != 0U) && (token_id < MAX_FLEXIDAG_TOKENS)) {
        retcode = krn_sem_wait(&flexidag_system_support.token[token_id], timeout);
        if(retcode != ERRCODE_NONE) {
            krn_printU5("[ERROR] krn_flexidag_wait_token() : krn_sem_wait fail token_id %d ret 0x%x \n", token_id, retcode, 0U, 0U, 0U);
            if(retcode == ERR_DRV_SCHDR_SEM_WAIT_TIMEOUT) {
                retcode = ERR_DRV_FLEXIDAG_SEM_WAIT_TIMEOUT;
            } else {
                retcode = ERR_DRV_FLEXIDAG_SEM_WAIT_FAIL;
            }
        }

        if(retcode == ERRCODE_NONE) {
            retcode = krn_flexidag_mutex_lock(&flexidag_system_support.token_mutex);
            if(retcode == ERRCODE_NONE) {
                if (flexidag_system_support.token_state[token_id] == (uint8_t)FLEXIDAG_TOKEN_FINISHED) {
                    retcode = flexidag_system_support.token_retcode[token_id];
                    if(retcode != ERRCODE_NONE) {
                        krn_printU5("[ERROR] krn_flexidag_wait_token() : token_retcode fail token_id %d ret 0x%x \n", token_id, retcode, 0U, 0U, 0U);
                        retcode = ERR_DRV_FLEXIDAG_VISMSG_RET_FAIL;
                    }
                } /* if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_FINISHED) */
                else { /* if (flexidag_system_support.token_state[token_id] != FLEXIDAG_TOKEN_FINISHED) */
                    krn_printU5("[ERROR] krn_flexidag_wait_token() : (token_id=%3d, block=%d) : Token state invalid (%d, expected %d)\n",
                                token_id, block, flexidag_system_support.token_state[token_id], FLEXIDAG_TOKEN_FINISHED, 0U);
                    retcode = ERR_DRV_FLEXIDAG_TOKEN_STATE_MISMATCHED;
                } /* if (flexidag_system_support.token_state[token_id] != FLEXIDAG_TOKEN_FINISHED) */
                flexidag_system_support.token_state[token_id] = (uint8_t)FLEXIDAG_TOKEN_UNUSED;
                ret = krn_flexidag_mutex_unlock(&flexidag_system_support.token_mutex);
                if(retcode == ERRCODE_NONE) {
                    retcode = ret;
                }
            }
        }
    } /* if (wait_valid != 0) */

    return retcode;
} /* krn_flexidag_wait_token() */

static uint32_t krn_flexidag_release_token(uint32_t token_id, uint32_t function_retcode, uint32_t function_start_time, uint32_t function_end_time, uint32_t function_output_not_generated)
{
    uint32_t  release_valid = 0U;
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  ret = ERRCODE_NONE;

    if (token_id >= MAX_FLEXIDAG_TOKENS) {
        /* ERROR */
        retcode       = ERR_DRV_FLEXIDAG_TOKEN_OUT_OF_RANGE;
    } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
    else { /* if (token_id < MAX_FLEXIDAG_TOKENS) */
        retcode = krn_flexidag_mutex_lock(&flexidag_system_support.token_mutex);
        if(retcode == ERRCODE_NONE) {
            if (flexidag_system_support.token_state[token_id] == (uint8_t)FLEXIDAG_TOKEN_FINISHED) {
                release_valid = 0U; /* Token wasn't blocking */
                flexidag_system_support.token_state[token_id]       = (uint8_t)FLEXIDAG_TOKEN_UNUSED;
                flexidag_system_support.token_retcode[token_id]     = function_retcode;
                flexidag_system_support.token_start_time[token_id]  = function_start_time;
                flexidag_system_support.token_end_time[token_id]    = function_end_time;
                flexidag_system_support.token_output_not_generated[token_id] = function_output_not_generated;
            } /* if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_FINISHED) */
            else if (flexidag_system_support.token_state[token_id] != (uint8_t)FLEXIDAG_TOKEN_WAITING) {
                /* ERROR */
                release_valid = 0U;
                krn_printU5("[ERROR] krn_flexidag_release_token() : (token_id=%3d, retcode=0x%x, %10u %10u) : Token state invalid (%d, expected 2)\n",
                            token_id, function_retcode, function_start_time, function_end_time, flexidag_system_support.token_state[token_id]);
                retcode       = ERR_DRV_FLEXIDAG_TOKEN_STATE_MISMATCHED;
            } /* if (flexidag_system_support.token_state[token_id] != FLEXIDAG_TOKEN_WAITING) */
            else { /* if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_WAITING) */
                release_valid = 1U;
            } /* if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_WAITING) */

            if (release_valid != 0U) {
                flexidag_system_support.token_state[token_id]       = (uint8_t)FLEXIDAG_TOKEN_FINISHED;
                flexidag_system_support.token_retcode[token_id]     = function_retcode;
                flexidag_system_support.token_start_time[token_id]  = function_start_time;
                flexidag_system_support.token_end_time[token_id]    = function_end_time;
                flexidag_system_support.token_output_not_generated[token_id] = function_output_not_generated;
            } /* if (release_valid != 0) */
            ret = krn_flexidag_mutex_unlock(&flexidag_system_support.token_mutex);
            if(retcode == ERRCODE_NONE) {
                retcode = ret;
            }
        }

        if (release_valid != 0U) {
            if( krn_sem_post(&flexidag_system_support.token[token_id]) != ERRCODE_NONE) {
                retcode = ERR_DRV_FLEXIDAG_SEM_POST_FAIL;
            }
        }
    } /* if (token_id < MAX_FLEXIDAG_TOKENS) */
    return retcode;
} /* krn_flexidag_release_token() */

static uint32_t krn_flexidag_send_armmsg(const armvis_msg_t *pmsg, uint32_t token_id, uint32_t is_blocking, uint32_t timeout)
{
    uint32_t  retcode = ERRCODE_NONE;

    retcode = krn_flexidag_mutex_lock(&flexidag_system_support.token_mutex);
    if(retcode == ERRCODE_NONE) {
        if (krn_schdrmsg_send_armmsg(pmsg, 1U) != ERRCODE_NONE) {
            if(krn_flexidag_mutex_unlock(&flexidag_system_support.token_mutex) != ERRCODE_NONE) {
                krn_printU5("[ERROR] krn_flexidag_send_armmsg() : krn_flexidag_mutex_unlock fail token_id 0x%x \n", token_id, 0U, 0U, 0U, 0U);
            }
            retcode = ERR_DRV_FLEXIDAG_VISMSG_SEND_FAIL;
        } else {
            retcode = krn_flexidag_wait_token(token_id, is_blocking, timeout);
        }
    }

    return retcode;
}
/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG initialization (IOCTL/KERNEL) =-----------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t krn_flexidag_system_init(void)
{
    static uint32_t flexidag_inited = 0U;
    uint32_t retcode = ERRCODE_NONE;
    uint64_t size = 0UL;
    void *ptr;

    if(flexidag_inited == 0U) {
        retcode = krn_memset(&flexidag_system_support, 0, sizeof(flexidag_system_support_t));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_mutex_init(&flexidag_system_support.token_mutex);
        }
        if(retcode == ERRCODE_NONE) {
            retcode = krn_mutex_init(&flexidag_system_support.slot_mutex);
        }
    } else {
        uint64_t  start = 0UL, target = 0UL;

        ptr = &flexidag_system_support;
        krn_typecast(&start,&ptr);
        ptr = &flexidag_system_support.pSlotState;
        krn_typecast(&target,&ptr);
        size = sizeof(flexidag_system_support_t) - (target - start);
        retcode = krn_memset(ptr, 0, size);
    }

    if(retcode == ERRCODE_NONE) {
        retcode = krn_flexidag_system_reset(flexidag_inited);
        if(retcode == ERRCODE_NONE) {
            flexidag_system_support.pSlotState = &flexidag_slot_state[0];
            flexidag_inited = 1U;
        }
    }

    return retcode;
} /* krn_flexidag_system_init() */

uint32_t krn_flexidag_system_reset(uint32_t is_init)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;
    uint32_t   loop;

    ret = krn_mutex_lock(&flexidag_system_support.token_mutex);
    if(ret != ERRCODE_NONE) {
        retcode = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
    } else {
        ret = krn_mutex_lock(&flexidag_system_support.slot_mutex);
        if(ret != ERRCODE_NONE) {
            retcode = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
            ret = krn_mutex_unlock(&flexidag_system_support.token_mutex);
            if(ret != ERRCODE_NONE) {
                retcode = ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL;
            }
        } else {
            flexidag_system_support.next_token_index  = 0;
            for (loop = 0; loop < FLEXIDAG_MAX_SLOTS; loop++) {
                if(is_init == 0U) {
                    retcode = krn_memset(&flexidag_slot_state[loop], 0, sizeof(flexidag_slot_state_t));
                    if(retcode == ERRCODE_NONE) {
                        retcode = krn_sem_init(&flexidag_slot_state[loop].run_mutex, 0, 1U);
                        if(retcode == ERRCODE_NONE) {
                            retcode = krn_sem_init(&flexidag_slot_state[loop].pending_run_semaphore, 0, 1U);
                            flexidag_slot_state[loop].pending_run_count = 0;
                        }
                    }
                } else {
                    uint64_t  start = 0UL, target = 0UL;
                    void *ptr;
                    uint64_t size = 0UL;

                    ptr = &flexidag_slot_state[loop];
                    krn_typecast(&start,&ptr);
                    ptr = &flexidag_slot_state[loop].flexidag_slot_id;
                    krn_typecast(&target,&ptr);
                    size = sizeof(flexidag_slot_state_t) - (target - start);
                    retcode = krn_memset(ptr, 0x0, size);
                }
                flexidag_system_support.slot_state[loop]    = FLEXIDAG_SLOT_UNUSED;
                flexidag_slot_state[loop].flexidag_slot_id  = loop;

                if(retcode != ERRCODE_NONE) {
                    break;
                }
            } /* for (loop = 0; loop < MAX_HOTLINK_TOKENS; loop++) */

            if(retcode == ERRCODE_NONE) {
                for (loop = 0; loop < MAX_FLEXIDAG_TOKENS; loop++) {
                    flexidag_system_support.token_state[loop]   = (uint8_t)FLEXIDAG_TOKEN_UNUSED;
                    flexidag_system_support.token_retcode[loop] = (uint32_t) ERRCODE_NONE;
                    if(is_init == 0U) {
                        retcode = krn_sem_init(&(flexidag_system_support.token[loop]), 0, 0U);
                    }

                    if(retcode != ERRCODE_NONE) {
                        break;
                    }
                } /* for (loop = [0 .. MAX_HOTLINK_TOKENS) ) */
            }

            ret = krn_mutex_unlock(&flexidag_system_support.slot_mutex);
            ret |= krn_mutex_unlock(&flexidag_system_support.token_mutex);
            if((ret != ERRCODE_NONE) && (retcode == ERRCODE_NONE)) {
                retcode = ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL;
            }
        }
    }
    return retcode;
} /* krn_flexidag_system_reset() */

uint32_t krn_flexidag_system_set_trace_daddr(uint64_t trace_daddr)
{
    uint32_t retcode = ERRCODE_NONE;

    if (trace_daddr != 0U) {
        if (trace_daddr != flexidag_system_support.flexidag_trace_daddr) {
            if(krn_c2v(trace_daddr) != NULL) {
                flexidag_system_support.flexidag_trace_daddr  = trace_daddr;
                flexidag_system_support.vpFlexidagTrace       = krn_c2v(trace_daddr);
            } else {
                retcode = ERR_DRV_SCHDR_FLEXIDAG_TRACE_INVALID;
                krn_printU5("[ERROR] krn_flexidag_system_set_trace_daddr() : Invalid trace_daddr 0x%x \n", trace_daddr, 0U, 0U, 0U, 0U);
            }
        } /* if (trace_daddr != flexidag_system_support.flexidag_trace_daddr) */
    } /* if (trace_daddr != 0) */

    return retcode;
} /* krn_flexidag_system_set_trace_daddr() */

uint64_t krn_flexidag_system_get_trace_daddr(void)
{
    return flexidag_system_support.flexidag_trace_daddr;
} /* krn_flexidag_system_get_trace_daddr() */

static uint32_t krn_flexidag_insert_creation_header(const void* buf, uint64_t size, uint64_t daddr)
{
    uint32_t retcode = ERRCODE_NONE;
#ifdef ASIL_COMPLIANCE
    membuf_creation_header_t *pmembuf_header = NULL;

    if((buf != NULL) && (size != 0U)) {
        krn_typecast(&pmembuf_header,&buf);
        pmembuf_header->buffer_daddr = (uint32_t)daddr;
        pmembuf_header->buffer_size = (uint32_t)size;
        pmembuf_header->curr_time = krn_visorc_get_curtime();
        pmembuf_header->fnv1a_checksum = krn_schdrmsg_calc_hash(buf, sizeof(membuf_creation_header_t) - 4U, FNV1A_32_INIT_HASH);
        if(krn_cache_clean(pmembuf_header, sizeof(membuf_creation_header_t)) != ERRCODE_NONE) {
            retcode = ERR_DRV_FLEXIDAG_CACHE_CLEAN_FAIL;
            krn_printU5("[ERROR] krn_flexidag_insert_creation_header() : krn_cache_clean fail \n", 0U, 0U, 0U, 0U, 0U);
        }
    }
#else
    (void) buf;
    (void) size;
    (void) daddr;
#endif /* ?ASIL_COMPLIANCE */

    return retcode;
}

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG subprocessing - individual functions (KERNEL/IOCTL) =-------------------------------*/
/*------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : enable() =--------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_ioctl_handle_enable(uint32_t num_slots)
{
    uint32_t retcode = ERRCODE_NONE;

    if (num_slots > FLEXIDAG_MAX_SLOTS) {
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_enable() : Invalid number of slots (%d, max %d)\n", num_slots, FLEXIDAG_MAX_SLOTS, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_FLEXIDAG_SLOT_OUT_OF_RANGE;
    } /* if (num_slots > FLEXIDAG_MAX_SLOTS) */
    else { /* if (num_slots <= FLEXIDAG_MAX_SLOTS) */
        uint32_t  slot_loop;

        flexidag_system_support.num_slots_enabled = (uint8_t) num_slots;
        for (slot_loop = 0; slot_loop < num_slots; slot_loop++) {
            flexidag_system_support.slot_state[slot_loop] = FLEXIDAG_SLOT_UNUSED;
        } /* for (slot_loop = 0; slot_loop < num_slots; slot_loop++) */
        for (slot_loop = num_slots; slot_loop < FLEXIDAG_MAX_SLOTS; slot_loop++) {
            flexidag_system_support.slot_state[slot_loop] = FLEXIDAG_SLOT_UNAVAIL;
        } /* for (slot_loop = num_slots; slot_loop < FLEXIDAG_MAX_SLOTS; slot_loop++) */
    } /* if (num_slots <= FLEXIDAG_MAX_SLOTS) */

    return retcode;

} /* krn_flexidag_ioctl_handle_enable() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : create() =--------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_ioctl_handle_create(void **vppHandle, uint32_t *pSlotId, uint64_t owner, uint32_t *pTokenId, uint32_t is_blocking)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;
    uint64_t addr1 = 0UL;

    if (vppHandle == NULL) {
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_create() : Invalid vppHandle \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } /* if (vppHandle == NULL) */
    else if (pSlotId == NULL) {
        krn_typecast(&addr1,&pSlotId);
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_create() : Invalid pSlotId (%x)\n", addr1, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } /* if (vpSlotId == NULL) */
    else if ((is_blocking == 0U) && (pTokenId == NULL)) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_create() : pTokenId == NULL is_blocking %d", is_blocking, 0U, 0U, 0U, 0U);
    } /* if ((is_blocking == 0U) && (pTokenId == NULL)) */
    else { /* if ((vppHandle != NULL) && (vpSlotId != NULL)) */
        flexidag_slot_state_t  *pFoundHandle;
        uint32_t  found_slot_id;
        uint32_t  token_id;
        void *ptr;

        pFoundHandle  = NULL;
        retcode = krn_flexidag_find_token(&token_id);
        if (retcode != ERRCODE_NONE) {
            krn_printU5("[ERROR] krn_flexidag_ioctl_handle_create() : Unable to find token in system (max=%d)\n", flexidag_system_support.num_slots_enabled, 0U, 0U, 0U, 0U);
        } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
        else { /* if (token_id < MAX_FLEXIDAG_TOKENS) */
            armvis_msg_t msgbase = {0};

            msgbase.hdr.message_type    = (uint16_t)(SCHEDMSG_FLEXIDAG_CREATE_REQUEST | 0x8000U);
            msgbase.hdr.message_id      = (uint16_t)0U;
            msgbase.hdr.message_retcode = (uint32_t)ERRCODE_NONE;
            msgbase.msg.flexidag_create_request.flexidag_token = token_id;

            retcode = krn_flexidag_send_armmsg(&msgbase, token_id, is_blocking, TOKEN_WAIT_TIMEOUT);
            if (retcode == ERRCODE_NONE) {
                if(is_blocking != 0U) {
                    uint64_t  start = 0UL, target = 0UL;
                    uint64_t size = 0UL;

                    retcode = krn_flexidag_mutex_lock(&flexidag_system_support.slot_mutex);
                    if(retcode == ERRCODE_NONE) {
                        found_slot_id = flexidag_system_support.token_start_time[token_id];
                        pFoundHandle  = &flexidag_slot_state[found_slot_id];
                        flexidag_system_support.slot_state[found_slot_id] = FLEXIDAG_SLOT_USED;
                        /*-= Reset slot state to default states =-----------------------------------------*/
                        krn_typecast(&start,&pFoundHandle);
                        ptr = &pFoundHandle->flexidag_slot_id;
                        krn_typecast(&target,&ptr);
                        size = sizeof(flexidag_slot_state_t) - (target - start);
                        if( krn_memset(ptr, 0, size) != ERRCODE_NONE) {
                            retcode = ERR_DRV_FLEXIDAG_MEMSET_FAIL;
                        } else {
                            pFoundHandle->owner = owner;
                            pFoundHandle->flexidag_slot_id = found_slot_id;
                            pFoundHandle->fdparam_interface_cvmsg_num     = FDPARAM_INTERFACE_MSGNUM_DEFAULT;
                            pFoundHandle->fdparam_internal_cvmsg_num      = FDPARAM_INTERNAL_MSGNUM_DEFAULT;
                            pFoundHandle->fdparam_log_size_arm_perf[0]    = FDPARAM_LOG_ENTRIES_ARMPERF_DEFAULT;
                            pFoundHandle->fdparam_log_size_arm_perf[1]    = FDPARAM_LOG_ENTRIES_ARMPERF_DEFAULT;
                            pFoundHandle->fdparam_log_size_arm_perf[2]    = FDPARAM_LOG_ENTRIES_ARMPERF_DEFAULT;
                            pFoundHandle->fdparam_log_size_arm_perf[3]    = FDPARAM_LOG_ENTRIES_ARMPERF_DEFAULT;
                            pFoundHandle->fdparam_log_size_arm_sched[0]   = FDPARAM_LOG_ENTRIES_ARMSCH_DEFAULT;
                            pFoundHandle->fdparam_log_size_arm_sched[1]   = FDPARAM_LOG_ENTRIES_ARMSCH_DEFAULT;
                            pFoundHandle->fdparam_log_size_arm_sched[2]   = FDPARAM_LOG_ENTRIES_ARMSCH_DEFAULT;
                            pFoundHandle->fdparam_log_size_arm_sched[3]   = FDPARAM_LOG_ENTRIES_ARMSCH_DEFAULT;
                            pFoundHandle->fdparam_log_size_arm_cvtask[0]  = FDPARAM_LOG_ENTRIES_ARM_DEFAULT;
                            pFoundHandle->fdparam_log_size_arm_cvtask[1]  = FDPARAM_LOG_ENTRIES_ARM_DEFAULT;
                            pFoundHandle->fdparam_log_size_arm_cvtask[2]  = FDPARAM_LOG_ENTRIES_ARM_DEFAULT;
                            pFoundHandle->fdparam_log_size_arm_cvtask[3]  = FDPARAM_LOG_ENTRIES_ARM_DEFAULT;
                            pFoundHandle->fdparam_log_size_vis_perf[0]    = FDPARAM_LOG_ENTRIES_VISPERF_DEFAULT;
                            pFoundHandle->fdparam_log_size_vis_perf[1]    = FDPARAM_LOG_ENTRIES_VISPERF_DEFAULT;
                            pFoundHandle->fdparam_log_size_vis_perf[2]    = FDPARAM_LOG_ENTRIES_VISPERF_DEFAULT;
                            pFoundHandle->fdparam_log_size_vis_perf[3]    = FDPARAM_LOG_ENTRIES_VISPERF_DEFAULT;
                            pFoundHandle->fdparam_log_size_vis_sched[0]   = FDPARAM_LOG_ENTRIES_VISSCH_DEFAULT;
                            pFoundHandle->fdparam_log_size_vis_sched[1]   = FDPARAM_LOG_ENTRIES_VISSCH_DEFAULT;
                            pFoundHandle->fdparam_log_size_vis_sched[2]   = FDPARAM_LOG_ENTRIES_VISSCH_DEFAULT;
                            pFoundHandle->fdparam_log_size_vis_sched[3]   = FDPARAM_LOG_ENTRIES_VISSCH_DEFAULT;
                            pFoundHandle->fdparam_log_size_vis_cvtask[0]  = FDPARAM_LOG_ENTRIES_VISORC_DEFAULT;
                            pFoundHandle->fdparam_log_size_vis_cvtask[1]  = FDPARAM_LOG_ENTRIES_VISORC_DEFAULT;
                            pFoundHandle->fdparam_log_size_vis_cvtask[2]  = FDPARAM_LOG_ENTRIES_VISORC_DEFAULT;
                            pFoundHandle->fdparam_log_size_vis_cvtask[3]  = FDPARAM_LOG_ENTRIES_VISORC_DEFAULT;
                            pFoundHandle->fdparam_token_timeout           = TOKEN_WAIT_TIMEOUT;

                            *vppHandle  = pFoundHandle;
                            *pSlotId    = found_slot_id;
                        }
                        ret = krn_flexidag_mutex_unlock(&flexidag_system_support.slot_mutex);
                        if(retcode == ERRCODE_NONE) {
                            retcode = ret;
                        }
                    }
                } else {
                    *pTokenId = token_id;
                }
            } /* if (retcode == 0) : krn_flexidag_wait_token() */
        } /* if (token_id < MAX_FLEXIDAG_TOKENS) */
    } /* if ((vppHandle != NULL) && (vpFlexiRequirements != NULL)) */

    return retcode;

} /* krn_flexidag_ioctl_handle_create() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : set_parameter() =-------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static void krn_flexidag_ioctl_handle_visorc_parameter(flexidag_slot_state_t *pHandle, uint32_t parameter_id, uint32_t parameter_value, uint32_t *hit)
{
    *hit = 1U;
    switch (parameter_id) {
    case FDPARAM_INTERFACE_MSGNUM_ID:
        pHandle->fdparam_interface_cvmsg_num = parameter_value;
        break;
    case FDPARAM_INTERNAL_MSGNUM_ID:
        pHandle->fdparam_internal_cvmsg_num = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_VISPERF0_ID:
        pHandle->fdparam_log_size_vis_perf[0] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_VISPERF1_ID:
        pHandle->fdparam_log_size_vis_perf[1] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_VISPERF2_ID:
        pHandle->fdparam_log_size_vis_perf[2] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_VISPERF3_ID:
        pHandle->fdparam_log_size_vis_perf[3] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_VISSCH0_ID:
        pHandle->fdparam_log_size_vis_sched[0] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_VISSCH1_ID:
        pHandle->fdparam_log_size_vis_sched[1] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_VISSCH2_ID:
        pHandle->fdparam_log_size_vis_sched[2] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_VISSCH3_ID:
        pHandle->fdparam_log_size_vis_sched[3] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_VISORC0_ID:
        pHandle->fdparam_log_size_vis_cvtask[0] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_VISORC1_ID:
        pHandle->fdparam_log_size_vis_cvtask[1] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_VISORC2_ID:
        pHandle->fdparam_log_size_vis_cvtask[2] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_VISORC3_ID:
        pHandle->fdparam_log_size_vis_cvtask[3] = parameter_value;
        break;
    default:
        *hit = 0U;
        break;
    } /* switch (parameter_id) */
}

static void krn_flexidag_ioctl_handle_arm_parameter(flexidag_slot_state_t *pHandle, uint32_t parameter_id, uint32_t parameter_value, uint32_t *hit)
{
    *hit = 1U;
    switch (parameter_id) {
    case FDPARAM_LOG_ENTRIES_ARMPERF0_ID:
        pHandle->fdparam_log_size_arm_perf[0] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_ARMPERF1_ID:
        pHandle->fdparam_log_size_arm_perf[1] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_ARMPERF2_ID:
        pHandle->fdparam_log_size_arm_perf[2] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_ARMPERF3_ID:
        pHandle->fdparam_log_size_arm_perf[3] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_ARMSCH0_ID:
        pHandle->fdparam_log_size_arm_sched[0] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_ARMSCH1_ID:
        pHandle->fdparam_log_size_arm_sched[1] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_ARMSCH2_ID:
        pHandle->fdparam_log_size_arm_sched[2] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_ARMSCH3_ID:
        pHandle->fdparam_log_size_arm_sched[3] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_ARM0_ID:
        pHandle->fdparam_log_size_arm_cvtask[0] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_ARM1_ID:
        pHandle->fdparam_log_size_arm_cvtask[1] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_ARM2_ID:
        pHandle->fdparam_log_size_arm_cvtask[2] = parameter_value;
        break;
    case FDPARAM_LOG_ENTRIES_ARM3_ID:
        pHandle->fdparam_log_size_arm_cvtask[3] = parameter_value;
        break;
    case FDPARAM_TOKEN_TIMEOUT_ID:
        pHandle->fdparam_token_timeout = parameter_value;
        break;
    default:
        *hit = 0U;
        break;
    } /* switch (parameter_id) */
}

static uint32_t krn_flexidag_ioctl_handle_set_parameter(void *vpHandle, uint32_t parameter_id, uint32_t parameter_value)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr1 = 0UL,addr2 = 0UL;
    uint32_t hit;

    (void) vpHandle;
    if (vpHandle == NULL) {
        krn_typecast(&addr1,&vpHandle);
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_parameter() : Invalid vpHandle (%x)\n", (uint32_t)addr1, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } /* if (vpHandle == NULL) */
    else { /* if (vpHandle != NULL) */
        flexidag_slot_state_t *pHandle;
        const flexidag_slot_state_t *pHandle_expected;

        krn_typecast(&pHandle,&vpHandle);
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            krn_typecast(&addr1,&pHandle);
            krn_typecast(&addr2,&pHandle_expected);
            krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_parameter() : kernel possibly corrupted : vpHandle(%x) slot_id=%d; expected pHandle(%x) slot_id=%d\n",
                        addr1, pHandle->flexidag_slot_id, addr2, pHandle_expected->flexidag_slot_id, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_MISMATCHED;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            krn_flexidag_ioctl_handle_visorc_parameter(pHandle, parameter_id, parameter_value, &hit);
            if(hit == 0U) {
                krn_flexidag_ioctl_handle_arm_parameter(pHandle, parameter_id, parameter_value, &hit);
                if(hit == 0U) {
                    krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_parameter() : Invalid parameter_id (%d), value = %d\n", parameter_id, parameter_value, 0U, 0U, 0U);
                    retcode = ERR_DRV_FLEXIDAG_PARAM_ID_UNKNOW;
                }
            }
        } /* if (pHandle == pHandle_expected) */
    } /* if (vpHandle != NULL) */

    return retcode;
} /* krn_flexidag_ioctl_handle_set_parameter() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : add_tbar() =------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_ioctl_handle_add_tbar(void *vpHandle, const kernel_memblk_t *pblk_tbar)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr1 = 0UL,addr2 = 0UL;

    (void) vpHandle;
    (void) pblk_tbar;
    if(vpHandle == NULL) {
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_add_tbar() : vpHandle == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if(pblk_tbar == NULL) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_add_tbar() : pblk_tbar == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        flexidag_slot_state_t *pHandle;
        const flexidag_slot_state_t *pHandle_expected;

        krn_typecast(&pHandle,&vpHandle);
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            krn_typecast(&addr1,&pHandle);
            krn_typecast(&addr2,&pHandle_expected);
            krn_printU5("[ERROR] krn_flexidag_ioctl_handle_add_tbar() : kernel possibly corrupted : vpHandle(%x) slot_id=%d; expected pHandle(%x) slot_id=%d\n",
                        addr1, pHandle->flexidag_slot_id, addr2, pHandle_expected->flexidag_slot_id, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_MISMATCHED;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            if (pHandle->num_extra_tbar >= FLEXIDAG_MAX_TBAR) {
                krn_typecast(&addr1,&pHandle);
                krn_printU5("[ERROR] krn_flexidag_ioctl_handle_add_tbar() : (%x) : Additional TBAR exceeds max tbar per flexidag (%d max %d)\n",
                            addr1, pHandle->num_extra_tbar, FLEXIDAG_MAX_TBAR, 0U, 0U);
                retcode = ERR_DRV_FLEXIDAG_CVTABLE_OUT_OF_RANGE;
            } /* if (pHandle->num_extra_tbar >= FLEXIDAG_MAX_TBAR) */
            else { /* if (pHandle->num_extra_tbar < FLEXIDAG_MAX_TBAR) */
                uint32_t  index;
                index = pHandle->num_extra_tbar;
                pHandle->vpExtraTbar[index]       = pblk_tbar->pBuffer;
                pHandle->extra_tbar_daddr[index]  = krn_p2c(pblk_tbar->buffer_daddr);
                pHandle->num_extra_tbar++;
            } /* if (pHandle->num_extra_tbar < FLEXIDAG_MAX_TBAR) */
        } /* if (pHandle == pHandle_expected) */
    }

    return retcode;
} /* krn_flexidag_ioctl_handle_add_tbar() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : add_sfb() =-------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_ioctl_handle_add_sfb(void *vpHandle, const kernel_memblk_t *pblk_sfb)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr1 = 0UL,addr2 = 0UL;

    (void) vpHandle;
    (void) pblk_sfb;
    if(vpHandle == NULL) {
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_add_sfb() : vpHandle == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if(pblk_sfb == NULL) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_add_sfb() : pblk_sfb == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        flexidag_slot_state_t *pHandle;
        const flexidag_slot_state_t *pHandle_expected;

        krn_typecast(&pHandle,&vpHandle);
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            krn_typecast(&addr1,&pHandle);
            krn_typecast(&addr2,&pHandle_expected);
            krn_printU5("[ERROR] krn_flexidag_ioctl_handle_add_sfb() : kernel possibly corrupted : vpHandle(%x) slot_id=%d; expected pHandle(%x) slot_id=%d\n",
                        addr1, pHandle->flexidag_slot_id, addr2, pHandle_expected->flexidag_slot_id, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_MISMATCHED;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            if (pHandle->num_extra_sfb >= FLEXIDAG_MAX_SFB) {
                krn_typecast(&addr1,&pHandle);
                krn_printU5("[ERROR] krn_flexidag_ioctl_handle_add_sfb() : (%x) : Additional SFB exceeds max sfb per flexidag (%d max %d)\n",
                            addr1, pHandle->num_extra_sfb, FLEXIDAG_MAX_SFB, 0U, 0U);
                retcode = ERR_DRV_FLEXIDAG_SYSFLOW_OUT_OF_RANGE;
            } /* if (pHandle->num_extra_sfb >= FLEXIDAG_MAX_SFB) */
            else { /* if (pHandle->num_extra_sfb < FLEXIDAG_MAX_SFB) */
                uint32_t  index;
                index = pHandle->num_extra_sfb;
                pHandle->vpExtraSfb[index]          = pblk_sfb->pBuffer;
                pHandle->extra_sfb_daddr[index]     = krn_p2c(pblk_sfb->buffer_daddr);
                {
                    uint16_t  scan_loop, num_lines;
                    uint32_t  numlines_size;
                    const void *ptr = NULL;
                    const uint8_t  *pRecast = NULL;

                    scan_loop = 0U;
                    numlines_size = (uint32_t) sizeof(sysflow_entry_t);
                    num_lines = (uint16_t)(pblk_sfb->buffer_size / numlines_size);
                    ptr = krn_p2v(pblk_sfb->buffer_daddr);
                    if(ptr != NULL) {
                        krn_typecast(&pRecast,&ptr);
                        while (scan_loop < num_lines) {
                            if (pRecast[(scan_loop * numlines_size)] == 0x00U) {
                                num_lines = scan_loop;
                                break;
                            } else {
                                scan_loop++;
                            }
                        }
                        pHandle->extra_sfb_numlines[index]  = num_lines;
                        krn_module_printU5("[INFO] krn_flexidag_ioctl_handle_add_sfb : sfb size %d, num_lines %d \n", pblk_sfb->buffer_size, num_lines, 0, 0, 0);
                    }
                }
                pHandle->num_extra_sfb++;
            } /* if (pHandle->num_extra_sfb < FLEXIDAG_MAX_SFB) */
        } /* if (pHandle == pHandle_expected) */
    }

    return retcode;
} /* krn_flexidag_ioctl_handle_add_sfb() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : open() =----------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static void krn_flexidag_ioctl_prepare_open_msg(const flexidag_slot_state_t *pHandle, armvis_msg_t *pmsgbase, uint32_t token_id, uint64_t  flexibin_daddr)
{
    uint32_t   core_loop;
    schedmsg_flexidag_open_request_t *pMsg;

    (void) pHandle;
    pmsgbase->hdr.message_type    = (uint16_t)(SCHEDMSG_FLEXIDAG_OPEN_REQUEST | 0x8000U);
    pmsgbase->hdr.message_id      = (uint16_t)(0x0100U + pHandle->flexidag_slot_id);
    pmsgbase->hdr.message_retcode = (uint32_t)ERRCODE_NONE;
    pMsg                        = &pmsgbase->msg.flexidag_open_request;

    pMsg->flexidag_token        = token_id;
    pMsg->flexidag_binary_daddr = (uint32_t)flexibin_daddr;
    pMsg->arm_cvmsg_num         = pHandle->fdparam_interface_cvmsg_num;
    pMsg->flexidag_cvmsg_num    = pHandle->fdparam_internal_cvmsg_num;
    for (core_loop = 0U; core_loop < (uint32_t)SYSTEM_MAX_NUM_VISORC; core_loop++) {
        pMsg->visorc_perf_log_entry_num[core_loop]    = pHandle->fdparam_log_size_vis_perf[core_loop];
        pMsg->visorc_sched_log_entry_num[core_loop]   = pHandle->fdparam_log_size_vis_sched[core_loop];
        pMsg->visorc_cvtask_log_entry_num[core_loop]  = pHandle->fdparam_log_size_vis_cvtask[core_loop];
    } /* for (core_loop = 0; core_loop < SYSTEM_MAX_NUM_VISORC; core_loop++) */
    for (core_loop = 0U; core_loop < (uint32_t)SYSTEM_MAX_NUM_CORTEX; core_loop++) {
        pMsg->arm_perf_log_entry_num[core_loop]       = pHandle->fdparam_log_size_arm_perf[core_loop];
        pMsg->arm_sched_log_entry_num[core_loop]      = pHandle->fdparam_log_size_arm_sched[core_loop];
        pMsg->arm_cvtask_log_entry_num[core_loop]     = pHandle->fdparam_log_size_arm_cvtask[core_loop];
    } /* for (core_loop = 0; core_loop < SYSTEM_MAX_NUM_CORTEX; core_loop++) */
    for (core_loop = 0U; core_loop < (uint32_t)FLEXIDAG_MAX_TBAR; core_loop++) {
        pMsg->additional_tbar_daddr[core_loop]  = (uint32_t)pHandle->extra_tbar_daddr[core_loop];
    } /* for (core_loop = 0; core_loop < FLEXIDAG_MAX_TBAR; core_loop++) */
    for (core_loop = 0U; core_loop < (uint32_t)FLEXIDAG_MAX_SFB; core_loop++) {
        pMsg->additional_sfb_daddr[core_loop]     = (uint32_t)pHandle->extra_sfb_daddr[core_loop];
        pMsg->additional_sfb_numlines[core_loop]  = pHandle->extra_sfb_numlines[core_loop];
    } /* for (core_loop = 0; core_loop < FLEXIDAG_MAX_SFB; core_loop++) */
}

static uint32_t krn_flexidag_ioctl_handle_open(void *vpHandle, const kernel_memblk_t *pFlexiBlk, flexidag_memory_requirements_t *pFlexiRequirements, uint32_t *pTokenId, uint32_t is_blocking)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr1 = 0UL,addr2 = 0UL;

    (void) vpHandle;
    (void) pFlexiBlk;
    if(vpHandle == NULL) {
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_open() : vpHandle == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if(pFlexiBlk == NULL) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_open() : pFlexiBlk == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if(pFlexiRequirements == NULL) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_open() : pFlexiRequirements == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if ((is_blocking == 0U) && (pTokenId == NULL)) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_open() : pTokenId == NULL is_blocking %d", is_blocking, 0U, 0U, 0U, 0U);
    } else {
        flexidag_slot_state_t *pHandle;
        const flexidag_slot_state_t *pHandle_expected;

        krn_typecast(&pHandle,&vpHandle);
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            krn_typecast(&addr1,&pHandle);
            krn_typecast(&addr2,&pHandle_expected);
            krn_printU5("[ERROR] krn_flexidag_ioctl_handle_open() : kernel possibly corrupted : vpHandle(%x) slot_id=%d; expected pHandle(%x) slot_id=%d\n",
                        addr1, pHandle->flexidag_slot_id, addr2, pHandle_expected->flexidag_slot_id, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_MISMATCHED;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            const void *vpFlexiBin;
            uint64_t  flexibin_daddr = 0UL;
            uint64_t  flexibin_size = 0UL;

            vpFlexiBin          = pFlexiBlk->pBuffer;
            flexibin_daddr      = krn_p2c(pFlexiBlk->buffer_daddr);
            flexibin_size       = pFlexiBlk->buffer_size;
            if ((vpFlexiBin == NULL) || (flexibin_daddr == 0UL) || (flexibin_size == 0UL)) {
                krn_typecast(&addr1,&vpFlexiBin);
                krn_printU5("[ERROR] krn_flexidag_ioctl_handle_open() : Invalid vpFlexiBin (%x), flexibin_daddr (0x%llx), or flexibin_size (%d)\n",
                            addr1, flexibin_daddr, flexibin_size,0U,0U);
                retcode = ERR_DRV_FLEXIDAG_FLEXIBIN_BUF_INVALID;
            } /* if ((vpFlexiBin == NULL) || (flexibin_daddr == 0) || (flexibin_size == 0)) */
            else { /* if ((vpFlexiBin != NULL) && (flexibin_daddr != 0) && (flexibin_size != 0)) */
                uint32_t  token_id;

                pHandle->vpFlexiBin     = pFlexiBlk->pBuffer;
                pHandle->flexibin_daddr = flexibin_daddr;
                pHandle->flexibin_size  = pFlexiBlk->buffer_size;

                if (pFlexiBlk->buffer_cacheable != 0U) {
                    if( krn_cache_clean(krn_c2v(flexibin_daddr), flexibin_size) != ERRCODE_NONE) {
                        retcode = ERR_DRV_FLEXIDAG_CACHE_CLEAN_FAIL;
                    }
                }

                if(retcode == ERRCODE_NONE) {
                    retcode = krn_flexidag_find_token(&token_id);
                    if (retcode != ERRCODE_NONE) {
                        krn_typecast(&addr1,&vpHandle);
                        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_open() : (%x) : Unable to find token in system (max=%d)\n", addr1, MAX_FLEXIDAG_TOKENS,0U,0U,0U);
                    } else {
                        armvis_msg_t msgbase = {0};

                        krn_flexidag_ioctl_prepare_open_msg(pHandle, &msgbase, token_id, flexibin_daddr);
                        retcode = krn_flexidag_send_armmsg(&msgbase, token_id, is_blocking, pHandle->fdparam_token_timeout);
                        if (retcode == ERRCODE_NONE) {
                            if(is_blocking != 0U) {
                                if(krn_memcpy(pFlexiRequirements, &pHandle->memreq, sizeof(flexidag_memory_requirements_t)) != ERRCODE_NONE) {
                                    retcode = ERR_DRV_FLEXIDAG_MEMCPY_FAIL;
                                }
                            } else {
                                *pTokenId = token_id;
                            }
                        } /* if (retcode == 0) : krn_flexidag_wait_token() */
                    }
                }
            } /* if ((vpFlexiBin != NULL) && (flexibin_daddr != 0) && (flexibin_size != 0)) */
        } /* if (pHandle == pHandle_expected) */
    }

    return retcode;
} /* krn_flexidag_ioctl_handle_open() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : set_state_buffer() =----------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_ioctl_handle_set_state_buffer(void *vpHandle, kernel_memblk_t *pFlexiBlk)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr1 = 0UL,addr2 = 0UL;

    (void) vpHandle;
    (void) pFlexiBlk;
    if(vpHandle == NULL) {
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_state_buffer() : vpHandle == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if(pFlexiBlk == NULL) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_state_buffer() : pFlexiBlk == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        flexidag_slot_state_t *pHandle;
        const flexidag_slot_state_t *pHandle_expected;

        krn_typecast(&pHandle,&vpHandle);
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            krn_typecast(&addr1,&pHandle);
            krn_typecast(&addr2,&pHandle_expected);
            krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_state_buffer() : kernel possibly corrupted : vpHandle(%x) slot_id=%d; expected pHandle(%x) slot_id=%d\n",
                        addr1, pHandle->flexidag_slot_id, addr2, pHandle_expected->flexidag_slot_id, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_MISMATCHED;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            krn_typecast(&pHandle->vpStateBuffer,&pFlexiBlk->pBuffer);
            pHandle->state_buffer_daddr = krn_p2c(pFlexiBlk->buffer_daddr);
            pHandle->state_buffer_size = pFlexiBlk->buffer_size;
        } /* if (pHandle == pHandle_expected) */
    }

    return retcode;
} /* krn_flexidag_ioctl_handle_set_state_buffer() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : set_temp_buffer() =-----------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_ioctl_handle_set_temp_buffer(void *vpHandle, kernel_memblk_t *pFlexiBlk)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr1 = 0UL,addr2 = 0UL;

    (void) vpHandle;
    (void) pFlexiBlk;
    if(vpHandle == NULL) {
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_temp_buffer() : vpHandle == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if(pFlexiBlk == NULL) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_temp_buffer() : pFlexiBlk == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        flexidag_slot_state_t *pHandle;
        const flexidag_slot_state_t *pHandle_expected;

        krn_typecast(&pHandle,&vpHandle);
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            krn_typecast(&addr1,&pHandle);
            krn_typecast(&addr2,&pHandle_expected);
            krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_temp_buffer() : kernel possibly corrupted : vpHandle(%x) slot_id=%d; expected pHandle(%x) slot_id=%d\n",
                        addr1, pHandle->flexidag_slot_id, addr2, pHandle_expected->flexidag_slot_id, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_MISMATCHED;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            krn_typecast(&pHandle->vpTempBuffer,&pFlexiBlk->pBuffer);
            pHandle->temp_buffer_daddr = krn_p2c(pFlexiBlk->buffer_daddr);
            pHandle->temp_buffer_size = pFlexiBlk->buffer_size;
        } /* if (pHandle == pHandle_expected) */
    }

    return retcode;
} /* krn_flexidag_ioctl_handle_set_temp_buffer() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : init() =----------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_ioctl_handle_init(void *vpHandle, uint32_t *pTokenId, uint32_t is_blocking)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr1 = 0UL,addr2 = 0UL;

    (void) vpHandle;
    if (vpHandle == NULL) {
        krn_typecast(&addr1,&vpHandle);
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_init() : Invalid vpHandle (%x)\n", addr1, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } /* if (vpHandle == NULL) */
    else if ((is_blocking == 0U) && (pTokenId == NULL)) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_init() : pTokenId == NULL is_blocking %d", is_blocking, 0U, 0U, 0U, 0U);
    } /* if ((is_blocking == 0U) && (pTokenId == NULL)) */
    else { /* if (vpHandle != NULL) */
        const flexidag_slot_state_t *pHandle;
        const flexidag_slot_state_t *pHandle_expected;

        krn_typecast(&pHandle,&vpHandle);
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            krn_typecast(&addr1,&pHandle);
            krn_typecast(&addr2,&pHandle_expected);
            krn_printU5("[ERROR] krn_flexidag_ioctl_handle_init() : kernel possibly corrupted : vpHandle(%x) slot_id=%d; expected pHandle(%x) slot_id=%d\n",
                        addr1, pHandle->flexidag_slot_id, addr2, pHandle_expected->flexidag_slot_id, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_MISMATCHED;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            if ((pHandle->vpStateBuffer == NULL) || (pHandle->state_buffer_daddr == 0U)) {
                krn_typecast(&addr1,&pHandle->vpStateBuffer);
                krn_typecast(&addr2,&pHandle->state_buffer_daddr);
                krn_printU5("[ERROR] krn_flexidag_ioctl_handle_init() : FD[%2d] : Slot has invalid vpStateBuffer (%x) / state_buffer_daddr = 0x%llx\n",
                            pHandle->flexidag_slot_id, addr1, addr2, 0U, 0U);
                retcode = ERR_DRV_FLEXIDAG_STATE_BUF_INVALID;
            } /* if ((pHandle->vpStateBuffer == NULL) || (pHandle->state_buffer_daddr == 0)) */
            else { /* if ((pHandle->vpStateBuffer != NULL) && (pHandle->state_buffer_daddr != 0) */
                uint32_t  token_id;

                retcode = krn_flexidag_find_token(&token_id);
                if (retcode != ERRCODE_NONE) {
                    krn_typecast(&addr1,&vpHandle);
                    krn_printU5("[ERROR] krn_flexidag_ioctl_handle_init(%x) : Unable to find token in system (max=%d)\n", addr1, MAX_FLEXIDAG_TOKENS, 0U, 0U, 0U);
                } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
                else { /* if (token_id < MAX_FLEXIDAG_TOKENS) */
                    armvis_msg_t msgbase = {0};
                    schedmsg_flexidag_init_request_t *pMsg;
                    uint32_t   output_loop;

                    msgbase.hdr.message_type    = (uint16_t)(SCHEDMSG_FLEXIDAG_INIT_REQUEST | 0x8000U);
                    msgbase.hdr.message_id      = (uint16_t)(0x0100U + pHandle->flexidag_slot_id);
                    msgbase.hdr.message_retcode = (uint32_t)ERRCODE_NONE;
                    pMsg                        = &msgbase.msg.flexidag_init_request;

                    pMsg->flexidag_token              = token_id;
                    pMsg->flexidag_state_buffer_daddr = (uint32_t)pHandle->state_buffer_daddr;
                    if(pHandle->state_buffer_size != 0U) {
                        retcode = krn_flexidag_insert_creation_header(krn_c2v(pHandle->state_buffer_daddr),pHandle->state_buffer_size,pHandle->state_buffer_daddr);
                    }
                    if (retcode == ERRCODE_NONE) {
                        pMsg->flexidag_temp_buffer_daddr  = (uint32_t)pHandle->temp_buffer_daddr;
                        if(pHandle->temp_buffer_size != 0U) {
                            retcode = krn_flexidag_insert_creation_header(krn_c2v(pHandle->temp_buffer_daddr),pHandle->temp_buffer_size,pHandle->temp_buffer_daddr);
                        }
                        if (retcode == ERRCODE_NONE) {
                            for (output_loop = 0U; output_loop < FLEXIDAG_MAX_OUTPUTS; output_loop++) {
                                pMsg->flexidag_callback_id[output_loop] = 0xFFFFFFFFU;  /* TODO: Link callback id /tokens */
                            } /* for (output_loop = 0; output_loop < FLEXIDAG_MAX_OUTPUTS; output_loop++) */

                            retcode = krn_flexidag_send_armmsg(&msgbase, token_id, is_blocking, pHandle->fdparam_token_timeout);
                            if (retcode == ERRCODE_NONE) {
                                if(is_blocking == 0U) {
                                    *pTokenId = token_id;
                                }
                            }
                        }
                    }
                } /* if (token_id < MAX_FLEXIDAG_TOKENS) */
            } /* if ((pHandle->vpStateBuffer != NULL) && (pHandle->state_buffer_daddr != 0) */
        } /* if (pHandle == pHandle_expected) */
    } /* if (vpHandle != NULL) */

    return retcode;
} /* krn_flexidag_ioctl_handle_init() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : prep_run() =------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_ioctl_handle_prep_run(void *vpHandle, uint32_t *pTokenId)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr1 = 0UL,addr2 = 0UL;

    (void) vpHandle;
    if(vpHandle == NULL) {
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_prep_run() : vpHandle == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if(pTokenId == NULL) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_prep_run() : pTokenId == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        flexidag_slot_state_t *pHandle;
        const flexidag_slot_state_t *pHandle_expected;

        krn_typecast(&pHandle,&vpHandle);
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            krn_typecast(&addr1,&pHandle);
            krn_typecast(&addr2,&pHandle_expected);
            krn_printU5("[ERROR] krn_flexidag_ioctl_handle_prep_run() : kernel possibly corrupted : vpHandle(%x) slot_id=%d; expected pHandle(%x) slot_id=%d\n",
                        addr1, pHandle->flexidag_slot_id, addr2, pHandle_expected->flexidag_slot_id, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_MISMATCHED;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            retcode = krn_flexidag_find_token(&pHandle->pending_token_id);
            if (retcode == ERRCODE_NONE) {
                retcode = krn_flexidag_sem_wait(&pHandle->run_mutex, OS_KERNEL_WAIT_FOREVER);
                *pTokenId = pHandle->pending_token_id;
            }
        } /* if (pHandle == pHandle_expected) */
    }

    return retcode;
} /* krn_flexidag_ioctl_handle_prep_run() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : set_input_buffer() =----------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_ioctl_handle_set_input_buffer(void *vpHandle, uint32_t input_num, kernel_memblk_t *pFlexiBlk)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr1 = 0UL,addr2 = 0UL;

    (void) vpHandle;
    (void) pFlexiBlk;
    if(vpHandle == NULL) {
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_input_buffer() : vpHandle == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if(pFlexiBlk == NULL) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_input_buffer() : pFlexiBlk == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if (input_num >= FLEXIDAG_MAX_INPUTS) {
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_input_buffer() : Invalid input_num (%d, max %d)\n", input_num, FLEXIDAG_MAX_INPUTS, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INTPUT_NUM_OUT_OF_RANGE;
    } else {
        flexidag_slot_state_t *pHandle;
        const flexidag_slot_state_t *pHandle_expected;

        krn_typecast(&pHandle,&vpHandle);
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            krn_typecast(&addr1,&pHandle);
            krn_typecast(&addr2,&pHandle_expected);
            krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_input_buffer() : kernel possibly corrupted : vpHandle(%x) slot_id=%d; expected pHandle(%x) slot_id=%d\n",
                        addr1, pHandle->flexidag_slot_id, addr2, pHandle_expected->flexidag_slot_id, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_MISMATCHED;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            krn_typecast(&pHandle->vpInputBuffer[input_num],&pFlexiBlk->pBuffer);
            pHandle->input_buffer_daddr[input_num]  = krn_p2c(pFlexiBlk->buffer_daddr);
            pHandle->input_buffer_size[input_num]   = pFlexiBlk->buffer_size;
        } /* if (pHandle == pHandle_expected) */
    }

    return retcode;
} /* krn_flexidag_ioctl_handle_set_input_buffer() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : set_flexidag_buffer() =-------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_ioctl_handle_set_feedback_buffer(void *vpHandle, uint32_t feedback_num, kernel_memblk_t *pFlexiBlk)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr1 = 0UL,addr2 = 0UL;

    (void) vpHandle;
    (void) pFlexiBlk;
    if(vpHandle == NULL) {
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_feedback_buffer() : vpHandle == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if(pFlexiBlk == NULL) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_feedback_buffer() : pFlexiBlk == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if (feedback_num >= FLEXIDAG_MAX_FEEDBACKS) {
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_feedback_buffer() : Invalid feedback_num (%d, max %d)\n", feedback_num, FLEXIDAG_MAX_FEEDBACKS, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_FEEDBACK_NUM_OUT_OF_RANGE;
    } else {
        flexidag_slot_state_t *pHandle;
        const flexidag_slot_state_t *pHandle_expected;

        krn_typecast(&pHandle,&vpHandle);
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            krn_typecast(&addr1,&pHandle);
            krn_typecast(&addr2,&pHandle_expected);
            krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_feedback_buffer() : kernel possibly corrupted : vpHandle(%x) slot_id=%d; expected pHandle(%x) slot_id=%d\n",
                        addr1, pHandle->flexidag_slot_id, addr2, pHandle_expected->flexidag_slot_id, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_MISMATCHED;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            krn_typecast(&pHandle->vpFeedbackBuffer[feedback_num],&pFlexiBlk->pBuffer);
            pHandle->feedback_buffer_daddr[feedback_num]  = krn_p2c(pFlexiBlk->buffer_daddr);
            pHandle->feedback_buffer_size[feedback_num]   = pFlexiBlk->buffer_size;
        } /* if (pHandle == pHandle_expected) */
    }

    return retcode;
} /* krn_flexidag_ioctl_handle_set_feedback_buffer() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : set_output_buffer() =---------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_ioctl_handle_set_output_buffer(void *vpHandle, uint32_t output_num, kernel_memblk_t *pFlexiBlk, uint64_t pOrgFlexiBlk)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr1 = 0UL,addr2 = 0UL;

    (void) vpHandle;
    (void) pFlexiBlk;
    if(vpHandle == NULL) {
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_output_buffer() : vpHandle == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if(pFlexiBlk == NULL) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_output_buffer() : pFlexiBlk == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if (output_num >= FLEXIDAG_MAX_OUTPUTS) {
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_output_buffer() : Invalid output_num (%d, max %d)\n", output_num, FLEXIDAG_MAX_OUTPUTS, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_OUTPUT_NUM_OUT_OF_RANGE;
    } else {
        flexidag_slot_state_t *pHandle;
        const flexidag_slot_state_t *pHandle_expected;

        krn_typecast(&pHandle,&vpHandle);
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            krn_typecast(&addr1,&pHandle);
            krn_typecast(&addr2,&pHandle_expected);
            krn_printU5("[ERROR] krn_flexidag_ioctl_handle_set_output_buffer() : kernel possibly corrupted : vpHandle(%p) slot_id=%d; expected pHandle(%p) slot_id=%d\n",
                        addr1, pHandle->flexidag_slot_id, addr2, pHandle_expected->flexidag_slot_id, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_MISMATCHED;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            krn_typecast(&pHandle->vpOutputBuffer[output_num],&pFlexiBlk->pBuffer);
            pHandle->output_buffer_daddr[output_num]  = krn_p2c(pFlexiBlk->buffer_daddr);
            pHandle->output_buffer_size[output_num]   = pFlexiBlk->buffer_size;
            krn_typecast(&pHandle->pNextOutputMemblk[output_num],&pOrgFlexiBlk);
        } /* if (pHandle == pHandle_expected) */
    }

    return retcode;

} /* krn_flexidag_ioctl_handle_set_output_buffer() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : run() =-----------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_ioctl_prepare_run_msg(flexidag_slot_state_t *pHandle, armvis_msg_t *pmsgbase, uint32_t token_id)
{
    schedmsg_flexidag_run_request_t *pMsg;
#ifdef ASIL_COMPLIANCE
    armvis_msg_t PreMsgBase;
    schedmsg_flexidag_prerun_request_t *pPreMsg;
#endif /* ?ASIL_COMPLIANCE */
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;
    uint32_t   io_loop;

#ifdef ASIL_COMPLIANCE
    /* schedmsg_flexidag_prerun_request_t */
    PreMsgBase.hdr.message_type    = (uint16_t)(SCHEDMSG_FLEXIDAG_PRERUN_REQUEST | 0x8000U);
    PreMsgBase.hdr.message_id      = (uint16_t)(0x0100U + pHandle->flexidag_slot_id);
    PreMsgBase.hdr.message_retcode = (uint32_t)ERRCODE_NONE;
    pPreMsg                        = &PreMsgBase.msg.flexidag_prerun_request;
    pPreMsg->flexidag_token        = token_id;
#endif /* ?ASIL_COMPLIANCE */

    /* schedmsg_flexidag_run_request_t */
    pmsgbase->hdr.message_type    = (uint16_t)(SCHEDMSG_FLEXIDAG_RUN_REQUEST | 0x8000U);
    pmsgbase->hdr.message_id      = (uint16_t)(0x0100U + pHandle->flexidag_slot_id);
    pmsgbase->hdr.message_retcode = (uint32_t)ERRCODE_NONE;
    pMsg                        = &pmsgbase->msg.flexidag_run_request;

    pMsg->flexidag_token              = token_id;
    pMsg->flexidag_state_buffer_daddr = (uint32_t)pHandle->state_buffer_daddr;
    pMsg->flexidag_temp_buffer_daddr  = (uint32_t)pHandle->temp_buffer_daddr;

    for (io_loop = 0; io_loop < FLEXIDAG_MAX_INPUTS; io_loop++) {
        pMsg->flexidag_input_buffer_daddr[io_loop]    = (uint32_t)pHandle->input_buffer_daddr[io_loop];
        pMsg->flexidag_input_buffer_size[io_loop]     = (uint32_t)pHandle->input_buffer_size[io_loop];
#ifdef ASIL_COMPLIANCE
        if(pHandle->input_buffer_size[io_loop] != 0U) {
            pPreMsg->flexidag_input_buffer_csum[io_loop]  = krn_schdrmsg_calc_hash(krn_c2v(pHandle->input_buffer_daddr[io_loop]), BUFFER_CHECKSUM_SIZE, FNV1A_32_INIT_HASH);
        } else {
            pPreMsg->flexidag_input_buffer_csum[io_loop]  = 0U;
        }
#endif /* ?ASIL_COMPLIANCE */
    } /* for (io_loop = 0; io_loop < FLEXIDAG_MAX_INPUTS; io_loop++) */
    for (io_loop = 0; io_loop < FLEXIDAG_MAX_FEEDBACKS; io_loop++) {
        pMsg->flexidag_feedback_buffer_daddr[io_loop] = (uint32_t)pHandle->feedback_buffer_daddr[io_loop];
        pMsg->flexidag_feedback_buffer_size[io_loop]  = (uint32_t)pHandle->feedback_buffer_size[io_loop];
#ifdef ASIL_COMPLIANCE
        if(pHandle->feedback_buffer_size[io_loop] != 0U) {
            pPreMsg->flexidag_feedback_buffer_csum[io_loop]  = krn_schdrmsg_calc_hash(krn_c2v(pHandle->feedback_buffer_daddr[io_loop]), BUFFER_CHECKSUM_SIZE, FNV1A_32_INIT_HASH);
        } else {
            pPreMsg->flexidag_feedback_buffer_csum[io_loop]  = 0U;
        }
#endif /* ?ASIL_COMPLIANCE */
    } /* for (io_loop = 0; io_loop < FLEXIDAG_MAX_FEEDBACKS; io_loop++) */
    for (io_loop = 0; io_loop < FLEXIDAG_MAX_OUTPUTS; io_loop++) {
        pMsg->flexidag_output_buffer_daddr[io_loop]   = (uint32_t)pHandle->output_buffer_daddr[io_loop];
        pMsg->flexidag_output_buffer_size[io_loop]    = (uint32_t)pHandle->output_buffer_size[io_loop];
        flexidag_system_support.pOutputMemBlk[token_id][io_loop] = pHandle->pNextOutputMemblk[io_loop];
#ifdef ASIL_COMPLIANCE
        if(pHandle->output_buffer_size[io_loop] != 0U) {
            retcode = krn_flexidag_insert_creation_header(krn_c2v(pHandle->output_buffer_daddr[io_loop]),pHandle->output_buffer_size[io_loop],pHandle->output_buffer_daddr[io_loop]);
            if(retcode != ERRCODE_NONE) {
                break;
            }
        }
#endif /* ?ASIL_COMPLIANCE */
    } /* for (io_loop = 0; io_loop < FLEXIDAG_MAX_OUTPUTS; io_loop++) */

#ifdef ASIL_COMPLIANCE
    if(retcode == ERRCODE_NONE)
#endif
    {
        pHandle->pending_run_count++;
        retcode = krn_sem_wait(&pHandle->pending_run_semaphore, WORK_QUEUE_WAIT_TIMEOUT);
        if(retcode != ERRCODE_NONE) {
            krn_printU5("[ERROR] krn_flexidag_ioctl_prepare_run_msg() : krn_sem_wait fail ret 0x%x \n", retcode, 0U, 0U, 0U, 0U);
            if(retcode == ERR_DRV_SCHDR_SEM_WAIT_TIMEOUT) {
                retcode = ERR_DRV_FLEXIDAG_SEM_WAIT_TIMEOUT;
            } else {
                retcode = ERR_DRV_FLEXIDAG_SEM_WAIT_FAIL;
            }
        }
    }

    ret = krn_flexidag_sem_post(&pHandle->run_mutex);
    if(retcode == ERRCODE_NONE) {
        retcode = ret;
    }

#ifdef ASIL_COMPLIANCE
    if(retcode == ERRCODE_NONE) {
        retcode = krn_flexidag_send_armmsg(&PreMsgBase, token_id, 0U, pHandle->fdparam_token_timeout);
    }
#endif /* ?ASIL_COMPLIANCE */

    return retcode;
}

static uint32_t krn_flexidag_ioctl_handle_run(void *vpHandle, flexidag_runinfo_t *pRunInfo, uint32_t *pTokenId, uint32_t is_blocking)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr1 = 0UL,addr2 = 0UL;

    (void) vpHandle;
    if(vpHandle == NULL) {
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_run() : vpHandle == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if((is_blocking == 1U) && (pRunInfo == NULL)) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_run() : pRunInfo == NULL is_blocking %d", is_blocking, 0U, 0U, 0U, 0U);
    } else if((is_blocking == 0U) && (pTokenId == NULL)) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_run() : pTokenId == NULL is_blocking %d", is_blocking, 0U, 0U, 0U, 0U);
    } else {
        flexidag_slot_state_t *pHandle;
        const flexidag_slot_state_t *pHandle_expected;

        krn_typecast(&pHandle,&vpHandle);
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            krn_typecast(&addr1,&pHandle);
            krn_typecast(&addr2,&pHandle_expected);
            krn_printU5("[ERROR] krn_flexidag_ioctl_handle_run() : kernel possibly corrupted : vpHandle(%x) slot_id=%d; expected pHandle(%x) slot_id=%d\n",
                        addr1, pHandle->flexidag_slot_id, addr2, pHandle_expected->flexidag_slot_id, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_MISMATCHED;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            if ((pHandle->vpStateBuffer == NULL) || (pHandle->state_buffer_daddr == 0U)) {
                krn_typecast(&addr1,&pHandle->vpStateBuffer);
                krn_printU5("[ERROR] krn_flexidag_ioctl_handle_run() : FD[%2d] : Slot has invalid vpStateBuffer (%x) / state_buffer_daddr = 0x%llx\n",
                            pHandle->flexidag_slot_id, addr1, pHandle->state_buffer_daddr, 0U, 0U);
                retcode = ERR_DRV_FLEXIDAG_STATE_BUF_INVALID;
            } /* if ((pHandle->vpStateBuffer == NULL) || (pHandle->state_buffer_daddr == 0)) */
            else { /* if ((pHandle->vpStateBuffer != NULL) && (pHandle->state_buffer_daddr != 0) */
                uint32_t  token_id;

                token_id = pHandle->pending_token_id;

                if (token_id >= MAX_FLEXIDAG_TOKENS) {
                    krn_typecast(&addr1,&vpHandle);
                    krn_printU5("[ERROR] krn_flexidag_ioctl_handle_run(%x) : Unable to find token in system (max=%d)\n", addr1, MAX_FLEXIDAG_TOKENS, 0U, 0U, 0U);
                    retcode = ERR_DRV_FLEXIDAG_TOKEN_OUT_OF_RANGE;
                } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
                else { /* if (token_id < MAX_FLEXIDAG_TOKENS) */
                    armvis_msg_t msgbase = {0};
                    uint32_t   io_loop;

                    retcode = krn_flexidag_ioctl_prepare_run_msg(pHandle, &msgbase, token_id);
                    if(retcode == ERRCODE_NONE) {
                        if(ambacv_state == 1U) {
                            retcode = krn_flexidag_send_armmsg(&msgbase, token_id, is_blocking, pHandle->fdparam_token_timeout);
                            pHandle->pending_token_id = 0xFFFFFFFFU;
                            if(retcode == ERRCODE_NONE) {
                                if ((is_blocking != 0U) && (pRunInfo != NULL)) {
                                    if( krn_memset(pRunInfo, 0, sizeof(*pRunInfo)) != ERRCODE_NONE) {
                                        retcode = ERR_DRV_FLEXIDAG_MEMSET_FAIL;
                                    } else {
                                        pRunInfo->version         = 1U;
                                        pRunInfo->size            = (uint32_t)sizeof(*pRunInfo);
                                        pRunInfo->overall_retcode = flexidag_system_support.token_retcode[token_id];
                                        pRunInfo->start_time      = flexidag_system_support.token_start_time[token_id];
                                        pRunInfo->end_time        = flexidag_system_support.token_end_time[token_id];
                                        pRunInfo->output_not_generated = flexidag_system_support.token_output_not_generated[token_id];
                                        for (io_loop = 0; io_loop < FLEXIDAG_MAX_OUTPUTS; io_loop++) {
                                            pRunInfo->output_donetime[io_loop] = flexidag_system_support.token_output_donetime[token_id][io_loop];
                                        } /* for (io_loop = 0; io_loop < FLEXIDAG_MAX_OUTPUTS; io_loop++) */
                                    }
                                } /* if ((is_blocking != 0) && (pRunInfo!= NULL)) */
                                else if (is_blocking == 0U) {
                                    *pTokenId = token_id;
                                } /* if ((is_blocking == 0) && (pTokenId != NULL)) */
                                else {
                                    krn_printU5("[ERROR] krn_flexidag_ioctl_handle_run() : is_blocking (%x) pRunInfo or pTokenId = NULL\n",
                                                is_blocking, 0U, 0U, 0U, 0U);
                                }
                            }
                        } else {
                            krn_printU5("[ERROR] krn_flexidag_ioctl_handle_run() : scheduler have shutdown, ambacv_state (%x) \n",
                                        ambacv_state, 0U, 0U, 0U, 0U);
                            retcode = ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE;
                        }
                    }
                } /* if (token_id < MAX_FLEXIDAG_TOKENS) */
            } /* if ((pHandle->vpStateBuffer != NULL) && (pHandle->state_buffer_daddr != 0) */
        } /* if (pHandle == pHandle_expected) */
    }

    return retcode;
} /* krn_flexidag_ioctl_handle_run() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : run() =-----------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_ioctl_handle_wait(void *vpHandle, flexidag_runinfo_t *pRunInfo, uint32_t token_id)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr1 = 0UL,addr2 = 0UL;

    (void) vpHandle;
    if (vpHandle == NULL) {
        krn_typecast(&addr1,&vpHandle);
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_wait() : Invalid vpHandle (%x)\n", addr1, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } /* if (vpHandle == NULL) */
    else if (token_id >= MAX_FLEXIDAG_TOKENS) {
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_wait() : Invalid token_id (%d, max %d)\n", token_id, MAX_FLEXIDAG_TOKENS, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_TOKEN_OUT_OF_RANGE;
    } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
    else { /* if ((vpHandle != NULL) && (token_id < MAX_FLEXIDAG_TOKENS)) */
        const flexidag_slot_state_t *pHandle;
        const flexidag_slot_state_t *pHandle_expected;

        krn_typecast(&pHandle,&vpHandle);
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            krn_typecast(&addr1,&pHandle);
            krn_typecast(&addr2,&pHandle_expected);
            krn_printU5("[ERROR] krn_flexidag_ioctl_handle_wait() : kernel possibly corrupted : vpHandle(%x) slot_id=%d; expected pHandle(%x) slot_id=%d\n",
                        addr1, pHandle->flexidag_slot_id, addr2, pHandle_expected->flexidag_slot_id, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_MISMATCHED;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            retcode = krn_flexidag_mutex_lock(&flexidag_system_support.token_mutex);
            if( retcode == ERRCODE_NONE) {
                retcode = krn_flexidag_wait_token(token_id, 2U, pHandle->fdparam_token_timeout);
                if( retcode != ERRCODE_NONE) {
                    krn_printU5("[ERROR] krn_flexidag_ioctl_handle_wait() : krn_flexidag_wait_token fail ret 0x%x \n", token_id, retcode, 0U, 0U, 0U);
                } else {
                    if (pRunInfo != NULL) {
                        uint32_t  io_loop;

                        if( krn_memset(pRunInfo, 0, sizeof(*pRunInfo)) != ERRCODE_NONE) {
                            retcode = ERR_DRV_FLEXIDAG_MEMSET_FAIL;
                        } else {
                            pRunInfo->version         = 1U;
                            pRunInfo->size            = (uint32_t)sizeof(*pRunInfo);
                            pRunInfo->overall_retcode = flexidag_system_support.token_retcode[token_id];
                            pRunInfo->start_time      = flexidag_system_support.token_start_time[token_id];
                            pRunInfo->end_time        = flexidag_system_support.token_end_time[token_id];
                            pRunInfo->output_not_generated = flexidag_system_support.token_output_not_generated[token_id];
                            for (io_loop = 0; io_loop < FLEXIDAG_MAX_OUTPUTS; io_loop++) {
                                pRunInfo->output_donetime[io_loop] = flexidag_system_support.token_output_donetime[token_id][io_loop];
                            } /* for (io_loop = 0; io_loop < FLEXIDAG_MAX_OUTPUTS; io_loop++) */
                        }
                    } /* if (pRunInfo != NULL) */
                }
            }
        } /* if (pHandle == pHandle_expected) */
    } /* if ((vpHandle != NULL) && (token_id < MAX_FLEXIDAG_TOKENS)) */

    return retcode;
} /* krn_flexidag_ioctl_handle_wait() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : close() =---------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_ioctl_handle_close(void *vpHandle, uint32_t *pTokenId, uint32_t is_blocking)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr1 = 0UL,addr2 = 0UL;

    (void) vpHandle;
    if (vpHandle == NULL) {
        krn_typecast(&addr1,&vpHandle);
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_close() : Invalid vpHandle (%x)\n", addr1, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } /* if (vpHandle == NULL) */
    else if ((is_blocking == 0U) && (pTokenId == NULL)) {
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_close() : pTokenId == NULL is_blocking %d", is_blocking, 0U, 0U, 0U, 0U);
    } /* if ((is_blocking == 0U) && (pTokenId == NULL)) */
    else { /* if (vpHandle != NULL) */
        flexidag_slot_state_t *pHandle;
        const flexidag_slot_state_t *pHandle_expected;

        krn_typecast(&pHandle,&vpHandle);
        if(ambacv_state == 1U) {
            pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
            if (pHandle != pHandle_expected) {
                krn_typecast(&addr1,&pHandle);
                krn_typecast(&addr2,&pHandle_expected);
                krn_printU5("[ERROR] krn_flexidag_ioctl_handle_close() : kernel possibly corrupted : vpHandle(%x) slot_id=%d; expected pHandle(%x) slot_id=%d\n",
                            addr1, pHandle->flexidag_slot_id, addr2, pHandle_expected->flexidag_slot_id, 0U);
                retcode = ERR_DRV_FLEXIDAG_HANDLE_MISMATCHED;
            } /* if (pHandle != pHandle_expected) */
            else { /* if (pHandle == pHandle_expected) */
                uint32_t  token_id;

                retcode = krn_flexidag_find_token(&token_id);
                if (retcode != ERRCODE_NONE) {
                    krn_typecast(&addr1,&vpHandle);
                    krn_printU5("[ERROR] krn_flexidag_ioctl_handle_close(%x) : Unable to find token in system (max=%d)\n", addr1, MAX_FLEXIDAG_TOKENS, 0U, 0U, 0U);
                } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
                else { /* if (token_id < MAX_FLEXIDAG_TOKENS) */
                    armvis_msg_t msgbase = {0};
                    schedmsg_flexidag_close_request_t *pMsg;

                    msgbase.hdr.message_type    = (uint16_t)(SCHEDMSG_FLEXIDAG_CLOSE_REQUEST | 0x8000U);
                    msgbase.hdr.message_id      = (uint16_t)(0x0100U + pHandle->flexidag_slot_id);
                    msgbase.hdr.message_retcode = (uint32_t)ERRCODE_NONE;
                    pMsg                        = &msgbase.msg.flexidag_close_request;

                    pMsg->flexidag_token        = token_id;

                    retcode = krn_flexidag_send_armmsg(&msgbase, token_id, is_blocking, pHandle->fdparam_token_timeout);
                    if (retcode == ERRCODE_NONE) {
                        pHandle->owner = 0UL;
                        if(is_blocking != 0U) {
                            retcode = krn_flexidag_mutex_lock(&flexidag_system_support.slot_mutex);
                            if (retcode == ERRCODE_NONE) {
                                flexidag_system_support.slot_state[pHandle->flexidag_slot_id] = FLEXIDAG_SLOT_UNUSED;
                                retcode = krn_flexidag_mutex_unlock(&flexidag_system_support.slot_mutex);
                            }
                        } else {
                            *pTokenId = token_id;
                        }
                    }
                } /* if (token_id < MAX_FLEXIDAG_TOKENS) */

            } /* if (pHandle == pHandle_expected) */
        } else {
            retcode = krn_flexidag_mutex_lock(&flexidag_system_support.slot_mutex);
            if (retcode == ERRCODE_NONE) {
                flexidag_system_support.slot_state[pHandle->flexidag_slot_id] = FLEXIDAG_SLOT_UNUSED;
                retcode = krn_flexidag_mutex_unlock(&flexidag_system_support.slot_mutex);
            }
        }
    } /* if (vpHandle != NULL) */

    return retcode;
} /* krn_flexidag_ioctl_handle_close() */
/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : info() =---------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_ioctl_handle_info(void *vpHandle)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_system_support_t *pSysSupport = &flexidag_system_support;
    const flexidag_trace_t *pFlexidagTrace;
    const flexidag_slot_trace_t *pFlexidagSlotTrace;
    const flexidag_slot_state_t *pHandle;
    uint32_t loop,loop_end,i;

    (void) vpHandle;
    krn_typecast(&pHandle,&vpHandle);
    retcode = krn_flexidag_mutex_lock(&pSysSupport->slot_mutex);
    if(retcode == ERRCODE_NONE) {
        if(pHandle == NULL) {
            loop = 0U;
            loop_end = pSysSupport->num_slots_enabled;
        } else {
            loop = pHandle->flexidag_slot_id;
            loop_end = pHandle->flexidag_slot_id + 1U;
        }
        for (; loop < loop_end; loop++) {
            if (pSysSupport->slot_state[loop] != FLEXIDAG_SLOT_UNUSED) {
                krn_printU5("FlexiDAG[%3d] :\n", loop, 0U, 0U, 0U, 0U);
                krn_printU5("   Flexibin     PA = (0x%llx) size = (0x%llx)\n", krn_c2p(pSysSupport->pSlotState[loop].flexibin_daddr), pSysSupport->pSlotState[loop].flexibin_size, 0U, 0U, 0U);
                krn_printU5("   StateBuffer  PA = (0x%llx) size = (0x%llx)\n", krn_c2p(pSysSupport->pSlotState[loop].state_buffer_daddr), pSysSupport->pSlotState[loop].memreq.flexidag_state_buffer_size, 0U, 0U, 0U);
                krn_printU5("   TempBuffer   PA = (0x%llx) size = (0x%llx)\n", krn_c2p(pSysSupport->pSlotState[loop].temp_buffer_daddr), pSysSupport->pSlotState[loop].memreq.flexidag_temp_buffer_size, 0U, 0U, 0U);
                for (i = 0U; i < FLEXIDAG_MAX_INPUTS; i++) {
                    if((pSysSupport->pSlotState[loop].input_buffer_daddr[i] != 0U) && (pSysSupport->pSlotState[loop].input_buffer_size[i] != 0U)) {
                        krn_printU5("   Input  [%02d]  PA = (0x%llx) size = (0x%llx)\n", i, krn_c2p(pSysSupport->pSlotState[loop].input_buffer_daddr[i]), pSysSupport->pSlotState[loop].input_buffer_size[i], 0U, 0U);
                    }
                }
                for (i = 0U; i < FLEXIDAG_MAX_OUTPUTS; i++) {
                    if((pSysSupport->pSlotState[loop].output_buffer_daddr[i] != 0U) && (pSysSupport->pSlotState[loop].output_buffer_size[i] != 0U)) {
                        krn_printU5("   Output [%02d]  PA = (0x%llx) size = (0x%llx)\n", i, krn_c2p(pSysSupport->pSlotState[loop].output_buffer_daddr[i]), pSysSupport->pSlotState[loop].output_buffer_size[i], 0U, 0U);
                    }
                }
            }
        }
        retcode = krn_flexidag_mutex_unlock(&pSysSupport->slot_mutex);
    }

    krn_printU5("\n", 0U, 0U, 0U, 0U, 0U);

    krn_typecast(&pFlexidagTrace,&pSysSupport->vpFlexidagTrace);
    if (pFlexidagTrace != NULL) {
        krn_printU5("FlexiDAG System State : [wt=%10u] : slots used [%3d/%3d]\n", pFlexidagTrace->last_wake_time, pFlexidagTrace->num_slots_used, pFlexidagTrace->num_slots_enabled, 0U, 0U);
        if(pHandle == NULL) {
            loop = 0U;
            loop_end = pSysSupport->num_slots_enabled;
        } else {
            loop = pHandle->flexidag_slot_id;
            loop_end = pHandle->flexidag_slot_id + 1U;
        }
        for (; loop < loop_end; loop++) {
            pFlexidagSlotTrace = &pFlexidagTrace->slottrace[loop];
            if (pFlexidagSlotTrace->slot_state != 0U) {
                krn_printU5("FlexiDAG Trace [%3d] : [%10u] : state [%4d] : [rc=0x%x]", loop, pFlexidagSlotTrace->last_wake_time, pFlexidagSlotTrace->slot_state, pFlexidagSlotTrace->last_errorcode, 0U);
                krn_printU5("   pending/finished fset_id [%5d/%5d]\n", pFlexidagSlotTrace->pending_frameset_id, pFlexidagSlotTrace->finished_frameset_id, 0U, 0U, 0U);
            } /* if (pFlexidagSlotTrace->slot_state != 0) */
        } /* for (loop = 0; loop < pSysSupport->num_slots_enabled; loop++) */
    } /* if (pSysSupport->vpFlexidagTrace != NULL) */

    return retcode;
}

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : message handlers() =----------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static uint32_t krn_flexidag_ioctl_handle_open_parse_reply(const armvis_msg_t *pMsg,uint32_t  flexidag_slot_id)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_slot_state_t *pHandle;
    uint32_t  max_outstanding;

    (void) pMsg;
    pHandle = &flexidag_slot_state[flexidag_slot_id];
    max_outstanding = pMsg->msg.flexidag_open_parsed.max_outstanding_runs;
    if ((max_outstanding == 0U) || (max_outstanding > FLEXIDAG_FRAME_QUEUE_SIZE)) {
        max_outstanding = 1U;
    }
    retcode = krn_sem_deinit(&pHandle->pending_run_semaphore);
    if(retcode == ERRCODE_NONE) {
        retcode = krn_sem_init(&pHandle->pending_run_semaphore, 0, max_outstanding);
        pHandle->pending_run_count = 0;
        if(retcode != ERRCODE_NONE) {
            krn_printU5("[ERROR] krn_flexidag_ioctl_handle_open_parse_reply() : SCHEDMSG_FLEXIDAG_OPEN_PARSED krn_sem_init fail ret (0x%x)\n", retcode, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_SEM_INIT_FAIL;
        }
    } else {
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_open_parse_reply() : SCHEDMSG_FLEXIDAG_OPEN_PARSED krn_sem_deinit fail ret (0x%x)\n", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_SEM_DEINIT_FAIL;
    }

    return retcode;
}

static uint32_t krn_flexidag_ioctl_handle_run_reply(const armvis_msg_t *pMsg,uint32_t  flexidag_slot_id)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) pMsg;
    retcode = krn_flexidag_release_token(pMsg->msg.flexidag_run_reply.flexidag_token, pMsg->hdr.message_retcode,
                                         pMsg->msg.flexidag_run_reply.start_time, pMsg->msg.flexidag_run_reply.end_time, pMsg->msg.flexidag_run_reply.output_not_generated);
    if(retcode == ERRCODE_NONE) {
        flexidag_slot_state_t *pHandle;
        pHandle = &flexidag_slot_state[flexidag_slot_id];
        if( krn_sem_post(&pHandle->pending_run_semaphore) != ERRCODE_NONE) {
            retcode = ERR_DRV_FLEXIDAG_SEM_POST_FAIL;
        } else {
            retcode = krn_flexidag_sem_wait(&pHandle->run_mutex, OS_KERNEL_WAIT_FOREVER);
            if(retcode == ERRCODE_NONE) {
                pHandle->pending_run_count--;
                retcode = krn_flexidag_sem_post(&pHandle->run_mutex);
            }
        }
    }

    return retcode;
}

static uint32_t krn_flexidag_ioctl_handle_output_done_reply(armvis_msg_t *pMsg)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) pMsg;
    krn_typecast(&pMsg->msg.flexidag_output_reply.vpOutputBlk,&flexidag_system_support.pOutputMemBlk[pMsg->msg.flexidag_output_reply.flexidag_token][pMsg->msg.flexidag_output_reply.flexidag_output_num]);
    flexidag_system_support.token_output_donetime[pMsg->msg.flexidag_output_reply.flexidag_token][pMsg->msg.flexidag_output_reply.flexidag_output_num] = pMsg->msg.flexidag_output_reply.flexidag_output_donetime;
    if(krn_cache_clean(pMsg, 64) != ERRCODE_NONE) {
        retcode = ERR_DRV_FLEXIDAG_CACHE_CLEAN_FAIL;
    } else {
        if(krn_cache_invalidate(pMsg, 64) != ERRCODE_NONE) {
            retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
        }
    }
    return retcode;
}

uint32_t krn_flexidag_ioctl_handle_message(armvis_msg_t *pMsg)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t addr1 = 0UL;

    if (pMsg == NULL) {
        krn_typecast(&addr1,&pMsg);
        krn_printU5("[ERROR] krn_flexidag_ioctl_handle_message() : Invalid pMsg (%x)\n", addr1, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_VISMSG_INVALID;
    } /* if (pMsg == NULL) */
    else { /* if (pMsg != NULL) */
        if ((pMsg->hdr.message_id >= (uint16_t)0x100U) && (pMsg->hdr.message_id < (uint16_t)(0x0100U + FLEXIDAG_MAX_SLOTS))) {
            uint32_t  flexidag_slot_id;
            uint32_t  type;

            flexidag_slot_id  = ((uint32_t)pMsg->hdr.message_id - 0x0100U);
            type = (uint32_t)pMsg->hdr.message_type;
            switch (type) {
            case SCHEDMSG_FLEXIDAG_CREATE_REPLY:
                retcode = krn_flexidag_release_token(pMsg->msg.flexidag_create_reply.flexidag_token, pMsg->hdr.message_retcode, pMsg->msg.flexidag_create_reply.flexidag_slot_id, 0, 0);
                break;

            case SCHEDMSG_FLEXIDAG_OPEN_REPLY: {
                uint32_t output_loop;
                flexidag_slot_state_t *pHandle;

                pHandle = &flexidag_slot_state[flexidag_slot_id];
                pHandle->memreq.flexidag_num_outputs        = pMsg->msg.flexidag_open_reply.flexidag_num_outputs;
                pHandle->memreq.flexidag_temp_buffer_size   = pMsg->msg.flexidag_open_reply.flexidag_memsize_temp;
                pHandle->memreq.flexidag_state_buffer_size  = pMsg->msg.flexidag_open_reply.flexidag_memsize_state;
                for (output_loop = 0U; output_loop < FLEXIDAG_MAX_OUTPUTS; output_loop++) {
                    pHandle->memreq.flexidag_output_buffer_size[output_loop] = pMsg->msg.flexidag_open_reply.flexidag_output_size[output_loop];
                } /* for (output_loop = 0; output_loop < FLEXIDAG_MAX_OUTPUTS; output_loop++) */
                retcode = krn_flexidag_release_token(pMsg->msg.flexidag_open_reply.flexidag_token, pMsg->hdr.message_retcode, 0U, 0U, 0U);
            }
            break;

            case SCHEDMSG_FLEXIDAG_OPEN_PARSED:
                retcode = krn_flexidag_ioctl_handle_open_parse_reply(pMsg, flexidag_slot_id);
                break;

            case SCHEDMSG_FLEXIDAG_INIT_REPLY:
                retcode = krn_flexidag_release_token(pMsg->msg.flexidag_init_reply.flexidag_token, pMsg->hdr.message_retcode, 0U, 0U, 0U);
                break;

            case SCHEDMSG_FLEXIDAG_OUTPUT_DONE:
                retcode = krn_flexidag_ioctl_handle_output_done_reply(pMsg);
                break;

            case SCHEDMSG_FLEXIDAG_RUN_REPLY:
                retcode = krn_flexidag_ioctl_handle_run_reply(pMsg, flexidag_slot_id);
                break;

            case SCHEDMSG_FLEXIDAG_CLOSE_REPLY:
                retcode = krn_flexidag_release_token(pMsg->msg.flexidag_close_reply.flexidag_token, pMsg->hdr.message_retcode, 0U, 0U, 0U);
                break;

            default:
                krn_typecast(&addr1,&pMsg);
                krn_printU5("[ERROR] krn_flexidag_ioctl_handle_message() : (%x) : Invalid pMsg->hdr.message_type(%04x)\n", addr1, pMsg->hdr.message_type, 0U, 0U, 0U);
                retcode = ERR_DRV_FLEXIDAG_VISMSG_UNKNOW;
                break;
            } /* switch (pMsg->hdr.message_type) */
        } /* if ((pMsg->hdr.message_id >= 0x100) && (pMsg->hdr.message_id < (0x0100+FLEXIDAG_MAX_SLOTS))) */
        else { /* if ((pMsg->hdr.message_id < 0x100) || (pMsg->hdr.message_id >= (0x0100+FLEXIDAG_MAX_SLOTS))) */
            krn_typecast(&addr1,&pMsg);
            krn_printU5("[error] krn_flexidag_ioctl_handle_message() : (%p) : pMsg->hdr.message_id is not valid for flexidag (0x%04x - should be 0x%04x - 0x%04x\n",
                        addr1, pMsg->hdr.message_id, 0x0100U, (0x0100U + FLEXIDAG_MAX_SLOTS), 0U);
            retcode = ERR_DRV_FLEXIDAG_SLOT_OUT_OF_RANGE;
        } /* if ((pMsg->hdr.message_id < 0x100) || (pMsg->hdr.message_id >= (0x0100+FLEXIDAG_MAX_SLOTS)) */
    } /* if (pMsg  != NULL) */

    return retcode;
} /* krn_flexidag_ioctl_handle_message() */

static uint32_t krn_flexidag_ioctl_part1(void* f, uint32_t cmd, void * arg, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_create_t *arg_create;
    const flexidag_ioctl_param_t *arg_param;
    const flexidag_ioctl_memblk_t *arg_memblk;
    flexidag_ioctl_open_t *arg_open;
    flexidag_ioctl_handle_t *arg_handle;
    const uint32_t *arg_value;
    uint64_t owner = 0UL;
    void *vpHandle = NULL;
    kernel_memblk_t local_memblk;

    (void) f;
    (void) arg;
    *hit = 1U;
    switch (cmd) {
    case AMBACV_FLEXIDAG_ENABLE:
        krn_typecast(&arg_value, &arg);
        retcode = krn_flexidag_ioctl_handle_enable(*arg_value);
        break;
    case AMBACV_FLEXIDAG_CREATE:
        krn_typecast(&arg_create, &arg);
        krn_typecast(&owner, &f);
        retcode = krn_flexidag_ioctl_handle_create(&vpHandle,&arg_create->slot, owner, &arg_create->token_id, arg_create->is_blocking);
        krn_typecast(&arg_create->handle,&vpHandle);
        break;
    case AMBACV_FLEXIDAG_SET_PARAMETER:
        krn_typecast(&arg_param, &arg);
        krn_typecast(&vpHandle,&arg_param->handle);
        retcode = krn_flexidag_ioctl_handle_set_parameter(vpHandle, arg_param->id, arg_param->value);
        break;
    case AMBACV_FLEXIDAG_ADD_TBAR:
        krn_typecast(&arg_memblk, &arg);
        krn_typecast(&vpHandle,&arg_memblk->handle);
        local_memblk.buffer_daddr = arg_memblk->mem_blk.buffer_daddr;
        local_memblk.buffer_cacheable = arg_memblk->mem_blk.buffer_cacheable;
        local_memblk.buffer_size = arg_memblk->mem_blk.buffer_size;
        krn_typecast(&local_memblk.pBuffer,&arg_memblk->mem_blk.buffer_vaddr);
        retcode = krn_flexidag_ioctl_handle_add_tbar(vpHandle, &local_memblk);
        break;
    case AMBACV_FLEXIDAG_ADD_SFB:
        krn_typecast(&arg_memblk, &arg);
        krn_typecast(&vpHandle,&arg_memblk->handle);
        local_memblk.buffer_daddr = arg_memblk->mem_blk.buffer_daddr;
        local_memblk.buffer_cacheable = arg_memblk->mem_blk.buffer_cacheable;
        local_memblk.buffer_size = arg_memblk->mem_blk.buffer_size;
        krn_typecast(&local_memblk.pBuffer,&arg_memblk->mem_blk.buffer_vaddr);
        retcode = krn_flexidag_ioctl_handle_add_sfb(vpHandle, &local_memblk);
        break;
    case AMBACV_FLEXIDAG_OPEN:
        krn_typecast(&arg_open, &arg);
        krn_typecast(&vpHandle,&arg_open->handle);
        local_memblk.buffer_daddr = arg_open->mem_blk.buffer_daddr;
        local_memblk.buffer_cacheable = arg_open->mem_blk.buffer_cacheable;
        local_memblk.buffer_size = arg_open->mem_blk.buffer_size;
        krn_typecast(&local_memblk.pBuffer,&arg_open->mem_blk.buffer_vaddr);
        retcode = krn_flexidag_ioctl_handle_open(vpHandle, &local_memblk, &arg_open->mem_req, &arg_open->token_id, arg_open->is_blocking);
        break;
    case AMBACV_FLEXIDAG_SET_STATE_BUFFER:
        krn_typecast(&arg_memblk, &arg);
        krn_typecast(&vpHandle,&arg_memblk->handle);
        local_memblk.buffer_daddr = arg_memblk->mem_blk.buffer_daddr;
        local_memblk.buffer_cacheable = arg_memblk->mem_blk.buffer_cacheable;
        local_memblk.buffer_size = arg_memblk->mem_blk.buffer_size;
        krn_typecast(&local_memblk.pBuffer,&arg_memblk->mem_blk.buffer_vaddr);
        retcode = krn_flexidag_ioctl_handle_set_state_buffer(vpHandle, &local_memblk);
        break;
    case AMBACV_FLEXIDAG_SET_TEMP_BUFFER:
        krn_typecast(&arg_memblk, &arg);
        krn_typecast(&vpHandle,&arg_memblk->handle);
        local_memblk.buffer_daddr = arg_memblk->mem_blk.buffer_daddr;
        local_memblk.buffer_cacheable = arg_memblk->mem_blk.buffer_cacheable;
        local_memblk.buffer_size = arg_memblk->mem_blk.buffer_size;
        krn_typecast(&local_memblk.pBuffer,&arg_memblk->mem_blk.buffer_vaddr);
        retcode = krn_flexidag_ioctl_handle_set_temp_buffer(vpHandle, &local_memblk);
        break;
    case AMBACV_FLEXIDAG_INIT:
        krn_typecast(&arg_handle, &arg);
        krn_typecast(&vpHandle,&arg_handle->handle);
        retcode = krn_flexidag_ioctl_handle_init(vpHandle, &arg_handle->token_id, arg_handle->is_blocking);
        break;
    default:
        *hit = 0U;
        break;
    }
    return retcode;
}

static uint32_t krn_flexidag_ioctl_part2(const void* f, uint32_t cmd, void * arg, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;
    flexidag_ioctl_handle_t *arg_handle;
    flexidag_ioctl_pre_run_t *arg_pre_run;
    const flexidag_ioctl_io_memblk_t *arg_io_memblk;
    flexidag_ioctl_run_t *arg_run;
    void *vpHandle = NULL;
    kernel_memblk_t local_memblk;
    uint64_t porig_memblk = 0UL;

    (void) f;
    (void) arg;
    *hit = 1U;
    switch (cmd) {
    case AMBACV_FLEXIDAG_PREP_RUN:
        krn_typecast(&arg_pre_run, &arg);
        krn_typecast(&vpHandle,&arg_pre_run->handle);
        retcode = krn_flexidag_ioctl_handle_prep_run(vpHandle,&arg_pre_run->token_id);
        break;
    case AMBACV_FLEXIDAG_SET_INPUT_BUFFER:
        krn_typecast(&arg_io_memblk, &arg);
        krn_typecast(&vpHandle,&arg_io_memblk->handle);
        local_memblk.buffer_daddr = arg_io_memblk->mem_blk.buffer_daddr;
        local_memblk.buffer_cacheable = arg_io_memblk->mem_blk.buffer_cacheable;
        local_memblk.buffer_size = arg_io_memblk->mem_blk.buffer_size;
        krn_typecast(&local_memblk.pBuffer,&arg_io_memblk->mem_blk.buffer_vaddr);
        retcode = krn_flexidag_ioctl_handle_set_input_buffer(vpHandle, arg_io_memblk->num, &local_memblk);
        break;
    case AMBACV_FLEXIDAG_SET_FEEDBACK_BUFFER:
        krn_typecast(&arg_io_memblk, &arg);
        krn_typecast(&vpHandle,&arg_io_memblk->handle);
        local_memblk.buffer_daddr = arg_io_memblk->mem_blk.buffer_daddr;
        local_memblk.buffer_cacheable = arg_io_memblk->mem_blk.buffer_cacheable;
        local_memblk.buffer_size = arg_io_memblk->mem_blk.buffer_size;
        krn_typecast(&local_memblk.pBuffer,&arg_io_memblk->mem_blk.buffer_vaddr);
        retcode = krn_flexidag_ioctl_handle_set_feedback_buffer(vpHandle, arg_io_memblk->num, &local_memblk);
        break;
    case AMBACV_FLEXIDAG_SET_OUTPUT_BUFFER:
        krn_typecast(&arg_io_memblk, &arg);
        krn_typecast(&vpHandle,&arg_io_memblk->handle);
        local_memblk.buffer_daddr = arg_io_memblk->mem_blk.buffer_daddr;
        local_memblk.buffer_cacheable = arg_io_memblk->mem_blk.buffer_cacheable;
        local_memblk.buffer_size = arg_io_memblk->mem_blk.buffer_size;
        krn_typecast(&local_memblk.pBuffer,&arg_io_memblk->mem_blk.buffer_vaddr);
        krn_typecast(&porig_memblk,&arg_io_memblk->orig_memBlk);
        retcode = krn_flexidag_ioctl_handle_set_output_buffer(vpHandle, arg_io_memblk->num, &local_memblk, porig_memblk);
        break;
    case AMBACV_FLEXIDAG_RUN:
        krn_typecast(&arg_run, &arg);
        krn_typecast(&vpHandle,&arg_run->handle);
        retcode = krn_flexidag_ioctl_handle_run(vpHandle, &arg_run->info, &arg_run->token_id,arg_run->is_blocking);
        break;
    case AMBACV_FLEXIDAG_WAIT_RUN_FINISH:
        krn_typecast(&arg_run, &arg);
        krn_typecast(&vpHandle,&arg_run->handle);
        retcode = krn_flexidag_ioctl_handle_wait(vpHandle, &arg_run->info, arg_run->token_id);
        break;
    case AMBACV_FLEXIDAG_CLOSE:
        krn_typecast(&arg_handle, &arg);
        krn_typecast(&vpHandle,&arg_handle->handle);
        retcode = krn_flexidag_ioctl_handle_close(vpHandle, &arg_handle->token_id,arg_handle->is_blocking);
        break;
    case AMBACV_FLEXIDAG_INFO:
        krn_typecast(&arg_handle, &arg);
        krn_typecast(&vpHandle,&arg_handle->handle);
        retcode = krn_flexidag_ioctl_handle_info(vpHandle);
        if(retcode == ERRCODE_NONE) {
            if( krn_schdr_autorun_info() != ERRCODE_NONE) {
                krn_printU5("[ERROR] krn_flexidag_ioctl_part2() : krn_schdr_autorun_info fail ", 0U, 0U, 0U, 0U, 0U);
            }
        }
        break;
    case AMBACV_FLEXIDAG_HANDLE_MSG:
        //do nothing
        break;
    default:
        *hit = 0U;
        break;
    }
    return retcode;
}

uint32_t krn_flexidag_ioctl_process(void* f, uint32_t cmd, void * arg)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t hit = 0U;

    (void) arg;
    (void) f;
    if (arg == NULL) {
        krn_printU5("[ERROR] krn_flexidag_ioctl_process() : Invalid parameter \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } /* if (arg == NULL) */
    else { /* if (arg != NULL) */
        retcode = krn_flexidag_ioctl_part1(f, cmd, arg, &hit);
        if(hit == 0U) {
            retcode = krn_flexidag_ioctl_part2(f, cmd, arg, &hit);
            if(hit == 0U) {
                krn_printU5("[ERROR] krn_flexidag_ioctl_process() : Unknown interface command type (0x%x)\n", cmd, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_FLEXIDAG_IOCTL_UNKNOW;
            }
        }
    }

    return retcode;
}

void krn_flexidag_system_shutdown(void)
{
    krn_module_printU5("Scheduler system shutdown\n", 0U, 0U, 0U, 0U, 0U);
} /* krn_scheduler_system_shutdown() */

void krn_flexidag_close_by_owner(uint64_t owner)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t  token_id,i;
    flexidag_slot_state_t *pHandle;

    if(ambacv_state == 1U) {
        for(i = 0U; i < FLEXIDAG_MAX_SLOTS; i++) {
            pHandle = &flexidag_slot_state[i];
            if(pHandle->owner == owner) {
                krn_printU5("[INFO] krn_flexidag_close_by_owner slot %d \n",i,0U,0U,0U,0U);
                retcode = krn_flexidag_find_token(&token_id);
                if ((retcode != ERRCODE_NONE) || (token_id >= MAX_FLEXIDAG_TOKENS)) {
                    krn_printU5("[ERROR] : krn_flexidag_close_by_owner() : Unable to find token in system (max=%d) ret 0x%x\n", MAX_FLEXIDAG_TOKENS, retcode, 0U, 0U, 0U);
                } else {
                    armvis_msg_t msgbase = {0};
                    schedmsg_flexidag_close_request_t *pMsg;

                    msgbase.hdr.message_type    = (uint16_t)(SCHEDMSG_FLEXIDAG_CLOSE_REQUEST | 0x8000U);
                    msgbase.hdr.message_id      = (uint16_t)(0x0100U + pHandle->flexidag_slot_id);
                    msgbase.hdr.message_retcode = (uint32_t)ERRCODE_NONE;
                    pMsg                        = &msgbase.msg.flexidag_close_request;

                    pMsg->flexidag_token        = token_id;

                    if( krn_flexidag_send_armmsg(&msgbase, token_id, 0, pHandle->fdparam_token_timeout) != ERRCODE_NONE) {
                        krn_printU5("[ERROR] : krn_flexidag_close_by_owner() : krn_flexidag_send_armmsg fail ", 0U, 0U, 0U, 0U, 0U);
                    }
                    pHandle->owner = 0UL;
                    if( krn_flexidag_mutex_lock(&flexidag_system_support.slot_mutex) != ERRCODE_NONE) {
                        krn_printU5("[ERROR] : krn_flexidag_close_by_owner() : krn_flexidag_mutex_lock fail", 0U, 0U, 0U, 0U, 0U);
                    }
                    flexidag_system_support.slot_state[pHandle->flexidag_slot_id] = FLEXIDAG_SLOT_UNUSED;
                    if( krn_flexidag_mutex_unlock(&flexidag_system_support.slot_mutex) != ERRCODE_NONE) {
                        krn_printU5("[ERROR] : krn_flexidag_close_by_owner() : krn_flexidag_mutex_unlock fail", 0U, 0U, 0U, 0U, 0U);
                    }
                } /* if (token_id < MAX_FLEXIDAG_TOKENS) */
            }
        }
    }
} /* flexidag_ioctl_handle_close() */

void krn_flexidag_shutdown(void)
{
    uint32_t  i;
    flexidag_slot_state_t *pHandle;

    if(ambacv_state == 0U) {
        /*  release waiting token */
        for(i = 0U; i < MAX_FLEXIDAG_TOKENS; i++) {
            if (flexidag_system_support.token_state[i] == (uint8_t)FLEXIDAG_TOKEN_WAITING) {
                if(krn_flexidag_release_token(i, ERR_DRV_FLEXIDAG_SCHDR_UNAVAILABLE, 0U, 0U, 1U) != ERRCODE_NONE) {
                    krn_printU5("[ERROR] : krn_flexidag_shutdown() : krn_flexidag_release_token fail", 0U, 0U, 0U, 0U, 0U);
                }
            }
        }

        /*  release pending run */
        for(i = 0U; i < FLEXIDAG_MAX_SLOTS; i++) {
            pHandle = &flexidag_slot_state[i];
            while(pHandle->pending_run_count > 0) {
                if( krn_sem_post(&pHandle->pending_run_semaphore) != ERRCODE_NONE) {
                    krn_printU5("[ERROR] : krn_flexidag_shutdown() : krn_sem_post fail", 0U, 0U, 0U, 0U, 0U);
                }
                if( krn_flexidag_sem_wait(&pHandle->run_mutex, OS_KERNEL_WAIT_FOREVER) != ERRCODE_NONE) {
                    krn_printU5("[ERROR] : krn_flexidag_shutdown() : krn_flexidag_sem_wait fail", 0U, 0U, 0U, 0U, 0U);
                } else {
                    pHandle->pending_run_count--;
                    if( krn_flexidag_sem_post(&pHandle->run_mutex) != ERRCODE_NONE) {
                        krn_printU5("[ERROR] : krn_flexidag_shutdown() : krn_flexidag_sem_post fail", 0U, 0U, 0U, 0U, 0U);
                    }
                }
            }
        }
    }
}

