// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
*  Copyright (c) 2021 Ambarella International LP
*/

#include "os_kernel.h"
#include "ambacv_kal.h"
#include "ambacv_kernel.h"
#include "visorc_kernel.h"
#include "idsporc_kernel.h"
#include "vinorc_kernel.h"
#include "cache_kernel.h"
#include "msg_kernel.h"
#include "schdr_kernel.h"
#include "flexidag_kernel.h"

#define MSG_INDEX_MASK          (ARMVIS_MESSAGE_NUM - 1U)

kthread_t        rx_kthread;
kthread_t        *prx_kthread = &rx_kthread;
uint32_t         rx_kthread_state = 0U;
keven_t          rx_lock;
static kmutex_t         tx_lock;
kmutex_t         find_lock;
static kmutex_t         wait_lock;
keven_t          msg_wq;
static uint32_t         visorc_rewake_time = 0U;  /* zero is disabled, non-zero means next wake point for the VISORC in audio ticks */
q_config_t       qconfig;
uint32_t         krn_schdr_with_thread = 1U;

#if defined (CHIP_CV6)
static kmutex_t  event_time_lock;
#endif
static uint32_t  event_time_ctrl = 0U;

/**
 *  @brief      Calculate hash by FNV-1a.
 *              hash = offset_basis
 *              for each octet_of_data to be hashed
 *                  hash = hash xor octet_of_data
 *                  hash = hash * FNV_prime
 *              return hash
 *
 *  @param      buf : [IN] ptr of buffer to do hash
 *  @param      len : [IN] size of buffer to do hash
 *
 *  @return     hash value
 */
uint32_t krn_schdrmsg_calc_hash(const void* buf, uint32_t len, uint32_t hval)
{
    uint8_t *ptr = NULL;
    uint32_t count = 0U;
    uint32_t new_hval = 0U;

    (void) ptr;
    (void) buf;
    if((buf != NULL) && (len != 0U)) {
        krn_typecast(&ptr, &buf);
        new_hval = hval;
        while (count < len) {
            new_hval = new_hval ^(uint32_t)ptr[count];
            new_hval = new_hval + (new_hval<<1) + (new_hval<<4) + (new_hval<<7) + (new_hval<<8) + (new_hval<<24);
            count++;
        }
    }
    return new_hval;
}

uint32_t krn_schdrmsg_get_orcarm_wi(void)
{
    return qconfig.from_orc_wi;
} /* krn_schdrmsg_get_orcarm_wi() */

static uint32_t krn_process_flexidag_schdrmsg(armvis_msg_t *msg, uint32_t type,uint32_t *pflexidag_msg)
{
    uint32_t ret = ERRCODE_NONE;
    const void *ptr = NULL;

    switch (type) {
    case SCHEDMSG_FLEXIDAG_CREATE_REPLY:
    case SCHEDMSG_FLEXIDAG_OPEN_REPLY:
    case SCHEDMSG_FLEXIDAG_OPEN_PARSED:
    case SCHEDMSG_FLEXIDAG_INIT_REPLY:
    case SCHEDMSG_FLEXIDAG_OUTPUT_DONE:
    case SCHEDMSG_FLEXIDAG_RUN_REPLY:
    case SCHEDMSG_FLEXIDAG_CLOSE_REPLY:
        *pflexidag_msg = 1U;
        ret = krn_flexidag_ioctl_handle_message(msg);
        if(ret != ERRCODE_NONE) {
            krn_printU5("[ERROR] krn_process_flexidag_schdrmsg() : type (0x%x) fail retcode (0x%x)", type, ret, 0U, 0U, 0U);
        }
        break;
    default:
        (void) ptr;
        break;
    }

    return ret;
}

static uint32_t krn_process_schdrmsg(armvis_msg_t *msg,uint32_t *changed)
{
    uint32_t retcode = ERRCODE_NONE, ret = ERRCODE_NONE;
    uint32_t type;
    uint32_t flexidag_msg = 0U;
    uint32_t num;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_process_schdrmsg() : msg == NULL", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        type = msg->hdr.message_type;
        switch (type) {
        case SCHEDMSG_HWUNIT_RESET:
            krn_visorc_softreset(msg->msg.hw_reset.hardware_unit);
            ret = krn_schdrmsg_send_armmsg(msg, 1U);
            if(ret != ERRCODE_NONE) {
                krn_printU5("[ERROR] krn_process_schdrmsg() : type (0x%x) fail retcode (0x%x)", type, ret, 0U, 0U, 0U);
            }
            break;

        case SCHEDMSG_BOOT_SETUP1:
            ret = krn_schdr_set_trace_daddr(msg->msg.boot_setup1.scheduler_trace_daddr);
            if(ret == ERRCODE_NONE) {
                ret = krn_flexidag_system_set_trace_daddr(msg->msg.boot_setup1.flexidag_trace_daddr);
            }
            if(ret == ERRCODE_NONE) {
                ret = krn_schdr_set_autorun_trace_daddr(msg->msg.boot_setup1.autorun_trace_daddr);
            }
            break;
        case SCHEDMSG_SET_REWAKE_TIME:
            if (msg->msg.set_rewake_time.rewake_time_delta == 0U) {
                visorc_rewake_time = 0U;
            } /* if (msg->set_rewake_time.rewake_time_delta == 0U) */
            else { /* if (msg->set_rewake_time.rewake_time_delta != 0U) */
                uint32_t curr_time = krn_visorc_get_curtime();
                visorc_rewake_time = ((curr_time + msg->msg.set_rewake_time.rewake_time_delta) - (curr_time - msg->msg.set_rewake_time.sent_time)) | 1U;
            } /* if (msg->set_rewake_time.rewake_time_delta != 0U) */
            break;
        case SCHEDMSG_FLUSH_VISPRINTF_REPLY:
            ret = krn_ambacv_release_token(msg->msg.flush_visprintf_reply.token_id, msg->hdr.message_retcode);
            break;
        case SCHEDMSG_CVSCHEDULER_ACTIVE:
            krn_schdr_get_isr_num(&num);
            if (num == 0U) {
                krn_printU5("[ERROR] krn_process_schdrmsg() : ISR is not activate", 0U, 0U, 0U, 0U, 0U);
                ret = ERR_DRV_SCHDR_ISR_NOT_ACTIVATE;
            }
            break;
        default:
            ret = krn_process_flexidag_schdrmsg(msg, type, &flexidag_msg);
            break;
        } /* switch (msg->hdr.message_type) */
        if(ret != ERRCODE_NONE) {
            msg->hdr.message_retcode = ret;
            retcode = krn_cache_clean((char*)&msg->hdr, sizeof(msg->hdr));
            if(retcode == ERRCODE_NONE) {
                retcode = krn_cache_invalidate((char*)&msg->hdr, sizeof(msg->hdr));
            }
            *changed = 1U;
        } else {
            if(msg->hdr.message_retcode != ERRCODE_NONE) {
                krn_printU5("[ERROR] krn_process_schdrmsg() : type 0x%x, visorc message_retcode 0x%x \n", type, msg->hdr.message_retcode, 0U, 0U, 0U);
                if(flexidag_msg == 1U) {
                    if( (msg->hdr.message_retcode >= ERR_SYSTEM_FATAL_BASE) && (msg->hdr.message_retcode <= 0xFFFEFFFFU)) {
                        msg->hdr.message_retcode = ERR_DRV_SCHDR_VISMSG_RET_FAIL;
                    } else {
                        msg->hdr.message_retcode = ERR_DRV_FLEXIDAG_VISMSG_RET_FAIL;
                    }
                } else {
                    msg->hdr.message_retcode = ERR_DRV_SCHDR_VISMSG_RET_FAIL;
                }
                retcode = krn_cache_clean((char*)&msg->hdr, sizeof(msg->hdr));
                if(retcode == ERRCODE_NONE) {
                    retcode = krn_cache_invalidate((char*)&msg->hdr, sizeof(msg->hdr));
                }
                *changed = 1U;
            }
        }
    }

    return retcode;
}

#ifdef ASIL_COMPLIANCE
static uint32_t krn_update_schdrmsg_checksum(armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t hash = FNV1A_32_INIT_HASH;

    if(msg->hdr.message_checksum != 0U) {
        hash = krn_schdrmsg_calc_hash(&msg->hdr, 12U, hash);
        hash = krn_schdrmsg_calc_hash(&msg->msg, sizeof(msg->msg), hash);
        msg->hdr.message_checksum = hash;
        retcode = krn_cache_clean((char*)&msg->hdr, sizeof(msg->hdr));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_cache_invalidate((char*)&msg->hdr, sizeof(msg->hdr));
        }
    }

    return retcode;
}
#endif /* ?ASIL_COMPLIANCE */

static uint32_t krn_check_schdrmsg(armvis_msg_t *msg,uint32_t *changed)
{
    uint32_t retcode = ERRCODE_NONE;

    if(msg->hdr.message_seqno != qconfig.from_orc_w_seqno) {
        if ((msg->hdr.message_id >= (uint16_t)0x100U) && (msg->hdr.message_id < (uint16_t)(0x0100U + FLEXIDAG_MAX_SLOTS))) {
            retcode = ERR_DRV_FLEXIDAG_VISMSG_SEQNO_MISMATCHED;
        } else {
            retcode = ERR_DRV_SCHDR_VISMSG_SEQNO_MISMATCHED;
        }
#ifdef ASIL_COMPLIANCE
    } else {
        uint32_t hash = FNV1A_32_INIT_HASH;

        if(msg->hdr.message_checksum != 0U) {
            hash = krn_schdrmsg_calc_hash(&msg->hdr, 12U, hash);
            hash = krn_schdrmsg_calc_hash(&msg->msg, sizeof(msg->msg), hash);
            if(hash != msg->hdr.message_checksum) {
                if ((msg->hdr.message_id >= (uint16_t)0x100U) && (msg->hdr.message_id < (uint16_t)(0x0100U + FLEXIDAG_MAX_SLOTS))) {
                    retcode = ERR_DRV_FLEXIDAG_VISMSG_CHECKSUM_MISMATCHED;
                } else {
                    retcode = ERR_DRV_SCHDR_VISMSG_CHECKSUM_MISMATCHED;
                }
            }
        }
#endif /* ?ASIL_COMPLIANCE */
    }

    if(retcode != ERRCODE_NONE) {
        msg->hdr.message_retcode = retcode;
        *changed = 1U;
    }
    qconfig.from_orc_w_seqno++;
    return retcode;
}

static uint32_t krn_invalidate_orc_schdrmsg(void)
{
    uint32_t retcode = ERRCODE_NONE, tmp_retcode = ERRCODE_NONE;
    armvis_msg_t *msg;
    uint32_t index, wi;

    retcode = krn_cache_invalidate(qconfig.from_orc_w, sizeof(*qconfig.from_orc_w));
    if(retcode == ERRCODE_NONE) {
        wi = *qconfig.from_orc_w;
        index = qconfig.from_orc_wi;

        while (index != wi) {
            uint32_t msg_changed = 0U;

            // get one armvis_msg_t from the queue
            msg = &(qconfig.from_orc_q[index & MSG_INDEX_MASK]);
            tmp_retcode = krn_cache_invalidate((char*)msg, sizeof(*msg));
            if(tmp_retcode != ERRCODE_NONE) {
                krn_printU5("[ERROR] krn_invalidate_orc_schdrmsg() : krn_cache_invalidate fail retcode (0x%x)", tmp_retcode, 0U, 0U, 0U, 0U);
            } else {
                tmp_retcode = krn_check_schdrmsg(msg, &msg_changed);
                if(tmp_retcode != ERRCODE_NONE) {
                    krn_printU5("[ERROR] krn_invalidate_orc_schdrmsg() : krn_chek_orc_schdrmsg fail retcode (0x%x)", tmp_retcode, 0U, 0U, 0U, 0U);
                } else {
                    tmp_retcode = krn_cache_invld_schdrmsg(msg, &msg_changed);
                    if(tmp_retcode != ERRCODE_NONE) {
                        krn_printU5("[ERROR] krn_invalidate_orc_schdrmsg() : krn_cache_invld_schdrmsg fail retcode (0x%x)", tmp_retcode, 0U, 0U, 0U, 0U);
                    } else {
                        tmp_retcode = krn_process_schdrmsg(msg, &msg_changed);
                        if(tmp_retcode != ERRCODE_NONE) {
                            krn_printU5("[ERROR] krn_invalidate_orc_schdrmsg() : krn_process_schdrmsg fail retcode (0x%x)", tmp_retcode, 0U, 0U, 0U, 0U);
                        }
                    }
                }

                if(msg->hdr.message_type == SCHEDMSG_FLEXIDAG_OUTPUT_DONE) {
                    msg_changed = 1U;
                }
                if(msg_changed != 0U) {
#ifdef ASIL_COMPLIANCE
                    if(krn_update_schdrmsg_checksum(msg) != ERRCODE_NONE) {
                        krn_printU5("[ERROR] krn_invalidate_orc_schdrmsg() : krn_update_schdrmsg_checksum fail retcode (0x%x)", tmp_retcode, 0U, 0U, 0U, 0U);
                    }
#endif
                }
            }
            index++;
        }

        qconfig.from_orc_wi = wi;
        // wake-up waiting AMBACV_RECV_ORCMSG ioctl
        retcode = krn_eventcond_set(&msg_wq);
    }

    return retcode;
}

static uint32_t krn_schdrmsg_get_rewake_time(void)
{
    uint32_t num_ms_wait;

    if (visorc_rewake_time != 0U) {
        uint32_t  at_time;
        uint32_t  atick_delta;

        at_time = krn_visorc_get_curtime(); /* Adjustment*/
        atick_delta = (uint32_t)(visorc_rewake_time - at_time);

        if (atick_delta >= 0x80000000U) { /* Expired */
            num_ms_wait         = 100U;
            visorc_rewake_time  = 0U;
        } /* if (atick_delta >= 0x80000000) */
        else if (atick_delta >= (100U * audio_clock_ms)) { /* Too far away */
            num_ms_wait = 100U;
        } /* if (atick_delta >= (100U * audio_clock_ms)) */
        else { /* if (atick_delta < (100U * audio_clock_ms)) : Under default 100ms tick*/
            num_ms_wait = (atick_delta + (audio_clock_ms - 1U)) / audio_clock_ms;
            if (num_ms_wait < 2U) { /* Clip to 2ms minimum wait */
                num_ms_wait = 2U;
            } /* if (num_ms_wait < 2) */
        } /* if (atick_delta < (100U * audio_clock_ms)) */
    } /* if (autorewake_time != 0U) */
    else { /* if (autorewake_time == 0U) */
        num_ms_wait = 100U;
    } /* if (autorewake_time == 0U) */

    return num_ms_wait;
}

void krn_schdrmsg_set_event_time(void)
{
#if defined (CHIP_CV6)
    uint32_t retcode = ERRCODE_NONE;
    uint32_t base_event_time;

    base_event_time = krn_visorc_get_curtime();
    retcode = krn_mutex_lock(&event_time_lock);
    if(retcode == ERRCODE_NONE) {
        uint32_t  audclk_time;
        uint32_t  extclk_time;

        base_event_time = base_event_time >>6U;
        audclk_time = (base_event_time << 6U);
        extclk_time = (base_event_time >> 26U);
        event_time_ctrl = extclk_time | ((audclk_time >> 25U) & 0x40U);

        retcode = krn_mutex_unlock(&event_time_lock);
        (void) retcode;
    }
#else
    event_time_ctrl = krn_visorc_get_curtime();
#endif
}

uint32_t krn_schdrmsg_get_event_time(void)
{
    uint32_t  retval = 0U;
#if defined (CHIP_CV6)
    uint32_t retcode = ERRCODE_NONE;

    retcode = krn_mutex_lock(&event_time_lock);
    if(retcode == ERRCODE_NONE) {
        uint32_t  curtime;
        uint32_t  ctrlval;
        uint32_t  flag;

        curtime = krn_visorc_get_curtime();
        ctrlval = event_time_ctrl;
        flag    = ((curtime >> 31U) | (ctrlval & 0x40U));

        if (flag == 0x40U) {
            ctrlval = (ctrlval + 1U) & 0x3F;
            event_time_ctrl = ctrlval;
        } else if (flag == 1U) {
            ctrlval |= 0x40;
            event_time_ctrl = ctrlval;
        } else { /* if ((flag == 0U) || (flag == 0x41U)) */
            ; /* Do nothing, MISRA 15.7 */
        } /* if ((flag == 0U) || (flag == 0x41U)) */
        retval  = curtime >> 6;
        retval |= (ctrlval & 0x3FU) << 26;

        retcode = krn_mutex_unlock(&event_time_lock);
        (void) retcode;
    }
#else
    event_time_ctrl = krn_visorc_get_curtime();
    retval = event_time_ctrl;
#endif

    return retval;
}

static void krn_schdrmsg_report_time(void)
{
    uint32_t sec, msec;
    uint32_t event_time;

    event_time = krn_schdrmsg_get_event_time();
    if (krn_get_time(UNIX_TIME_ID, &sec, &msec) == 0U) {
        armvis_msg_t msgbase = {0};
        schedmsg_time_report_t *pMsg;

        msgbase.hdr.message_type  = (uint16_t)SCHEDMSG_TIME_REPORT;
        pMsg = &msgbase.msg.time_report;
        pMsg->linux_epoch_time = sec;
        pMsg->linux_mstime = msec;
        pMsg->linux_event_time = event_time;
        if (krn_schdrmsg_send_armmsg(&msgbase, 1U) != 0U) {
            krn_printU5("[ERROR] krn_schdrmsg_report_time() : krn_schdrmsg_send_armmsg fail \n", 0U, 0U, 0U, 0U, 0U);
        }
    } else {
        krn_printU5("[ERROR] krn_schdrmsg_report_time() : krn_get_time fail \n", 0U, 0U, 0U, 0U, 0U);
    }
}

static void krn_schdrmsg_rewake_report_time(void)
{
    static uint32_t report_period;
    static uint32_t last_report_time = 0U;
    uint32_t current_time;
    uint32_t  atick_delta;

    if(ambacv_state == 1U) {
        current_time = krn_visorc_get_curtime();
        if(last_report_time == 0U) {
            report_period = (100U * audio_clock_ms);
            last_report_time = current_time;
        } else {
            atick_delta = (uint32_t)((last_report_time + report_period) - current_time);
            if (atick_delta >= 0x80000000U) { /* Expired */
                krn_schdrmsg_report_time();
                last_report_time = current_time;
            } /* if (atick_delta >= 0x80000000) */
            else if (atick_delta >= (100U * audio_clock_ms)) { /* Too far away */
                krn_schdrmsg_report_time();
                last_report_time = current_time;
            } /* if (atick_delta >= (100U * audio_clock_ms)) */
            else { /* if (atick_delta < (100U * audio_clock_ms)) : Under default 100ms tick*/
                /* Do nothing */
            } /* if (atick_delta < (100U * audio_clock_ms)) */
        }
    }
}

uint32_t krn_schdrmsg_rx_entry(void* arg)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t actual_flag;
    uint32_t loop = 1U;
    uint32_t rewake_time;

    (void) arg;
    krn_unused(arg);
    while (loop == 1U) {
        rewake_time = krn_schdrmsg_get_rewake_time();
        retcode = krn_eventflag_get(&rx_lock, EFLAG_MASK, &actual_flag, rewake_time);
        if (retcode != ERRCODE_NONE) {
            if (krn_thread_should_stop(prx_kthread) != 0U) {
                loop = 0U;
            }

            if(loop == 1U) {
                if(is_booting_os != 0U) {
                    /* kick ORC after time-out */
                    krn_visorc_kick();
                }
            }
        }

        /* break out if stop flag is set */
        if ( (actual_flag & EFLAG_STOP) != 0U) {
            loop = 0U;
        }

        if(loop == 1U) {
            /* invalidate all incoming messages */
            retcode = krn_invalidate_orc_schdrmsg();
            if(retcode != ERRCODE_NONE) {
                krn_printU5("[ERROR] krn_schdrmsg_rx_entry() : krn_invalidate_orc_schdrmsg fail retcode = 0x%x\n", retcode, 0U, 0U, 0U, 0U);
            }
        }
        krn_schdrmsg_rewake_report_time();
    }
    return 0U;
}

uint32_t krn_schdrmsg_rx_wait(uint32_t *ret)
{
    uint32_t retcode = ERRCODE_NONE,tmp_retcode = ERRCODE_NONE;
    uint32_t rewake_time;
    uint32_t actual_flag;

    if(krn_schdr_with_thread == 0U) {
        if(rx_kthread_state == 1U) {
            tmp_retcode = krn_mutex_try_lock(&wait_lock);
            if(tmp_retcode == ERRCODE_NONE) {
                rewake_time = krn_schdrmsg_get_rewake_time();
                if(rewake_time > 5000U) {
                    krn_printU5("[WARN] krn_schdrmsg_rx_wait() : rewake_time %d too long", rewake_time, 0U, 0U, 0U, 0U);
                }
                tmp_retcode = krn_eventflag_get(&rx_lock, EFLAG_MASK, &actual_flag, rewake_time);
                if (tmp_retcode != ERRCODE_NONE) {
                    if(is_booting_os != 0U) {
                        /* kick ORC after time-out */
                        krn_visorc_kick();
                    }
                }
                if ( (actual_flag & EFLAG_STOP) == 0U) {
                    /* invalidate all incoming messages */
                    retcode = krn_invalidate_orc_schdrmsg();
                }
                krn_schdrmsg_rewake_report_time();
                if( krn_mutex_unlock(&wait_lock) != ERRCODE_NONE) {
                    krn_printU5("[ERROR] krn_schdrmsg_rx_wait() : krn_mutex_unlock fail ", 0U, 0U, 0U, 0U, 0U);
                }
            } else {
                /* wait until we have new request message */
                tmp_retcode = krn_eventcond_get(&msg_wq, &qconfig.from_orc_wi, qconfig.from_orc_r, EVENT_OPT_GL, 100U);
            }
        } else {
            krn_printU5("[ERROR] krn_schdrmsg_rx_wait() : invalid to call this api, rx_kthread_state %d, ", rx_kthread_state, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_INTERNAL_THREAD_STATE_MISMATCHED;
        }
    } else {
        krn_printU5("[ERROR] krn_schdrmsg_rx_wait() : invalid to call this api, krn_schdr_with_thread %d, ", krn_schdr_with_thread, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INTERNAL_THREAD_STATE_MISMATCHED;
    }

    *ret = tmp_retcode;
    return retcode;
}

uint32_t krn_schdrmsg_recv_orcmsg(ambacv_get_value_t *arg)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;
    uint32_t ri;

    (void) arg;
    if(rx_kthread_state == 1U) {
        /* update read index of request queue */
        ri = (uint32_t)arg->index;
        *qconfig.from_orc_r = ri;
        retcode = krn_cache_clean(qconfig.from_orc_r, sizeof(*qconfig.from_orc_r));
        if(retcode == ERRCODE_NONE) {
            /* wait until we have new request message */
            if(krn_schdr_with_thread == 1U) {
                ret = krn_eventcond_get(&msg_wq, &qconfig.from_orc_wi, &ri, EVENT_OPT_GL, 100U);
                if(ret != ERR_DRV_SCHDR_EVENTFLAG_GET_TIMEOUT) {
                    retcode = ret;
                }
            }
            arg->value = qconfig.from_orc_wi;
        }
    }
    return retcode;
}

static uint32_t krn_schdrmsg_wait_send_available(void)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t wi = 0U, ri = 0U;
    uint32_t loop = 0U;

    ri = *qconfig.from_arm_r;
    wi = *qconfig.from_arm_w;
    while ((wi - ri) >= ARMVIS_MESSAGE_NUM) {
        if (loop == 0U) {
            krn_printU5("[WARNING] krn_schdrmsg_send_armmsg() : arm->orc message full %d %d\n", ri, wi, 0U, 0U, 0U);
        }
        loop++;
        if( krn_sleep(10) != ERRCODE_NONE) {
            krn_printU5("[ERROR] krn_schdrmsg_send_armmsg() : krn_sleep fail", 0U, 0U, 0U, 0U, 0U);
        }
        retcode = krn_cache_invalidate((char*)qconfig.from_arm_r, sizeof(*qconfig.from_arm_r));
        if(retcode == ERRCODE_NONE) {
            ri = *qconfig.from_arm_r;

            if(ambacv_state == 0U) {
                retcode =  ERR_DRV_SCHDR_NOT_ACTIVATE;
                krn_printU5("[ERROR] krn_schdrmsg_send_armmsg() : ERR_DRV_SCHDR_NOT_ACTIVATE ", 0U, 0U, 0U, 0U, 0U);
            } else if(loop > (TOKEN_WAIT_TIMEOUT/10U)) {
                retcode =  ERR_DRV_SCHDR_VISMSG_QUEUE_ARM2ORC_INVALID;
                krn_printU5("[ERROR] krn_schdrmsg_send_armmsg() : arm->orc message full", ri, wi, 0U, 0U, 0U);
            } else {
                /* Do nothing */
            }
        }

        if(retcode != 0U) {
            break;
        }
    }
    return retcode;
}


uint32_t krn_schdrmsg_send_armmsg(const armvis_msg_t *arg, uint32_t is_from_kernel_space)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;
    uint32_t wi = 0U;
    armvis_msg_t *dst;

    (void) arg;
    (void) is_from_kernel_space;
    if(arg == NULL) {
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
        krn_printU5("[ERROR] krn_schdrmsg_send_armmsg() : arg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        retcode = krn_mutex_lock(&tx_lock);
        if(retcode == ERRCODE_NONE) {
            retcode = krn_cache_invalidate((char*)qconfig.from_arm_r, sizeof(*qconfig.from_arm_r));
            if(retcode == ERRCODE_NONE) {
                retcode = krn_cache_invalidate((char*)qconfig.from_arm_w, sizeof(*qconfig.from_arm_w));
            }

            wi = *qconfig.from_arm_w;
            if(retcode == ERRCODE_NONE) {
                retcode = krn_schdrmsg_wait_send_available();
                if(retcode == ERRCODE_NONE) {
#ifdef ASIL_COMPLIANCE
                    uint32_t hash = FNV1A_32_INIT_HASH;
#endif /* ?ASIL_COMPLIANCE */

                    dst = &(qconfig.from_arm_q[wi & MSG_INDEX_MASK]);
                    retcode = krn_memcpy(dst, arg, sizeof(armvis_msg_t));
                    dst->hdr.message_type |= MSGTYPEBIT_ARM_TO_VISORC; /* Make sure bit is set */
                    /* seqno */
                    dst->hdr.message_seqno = qconfig.from_arm_w_seqno;
                    qconfig.from_arm_w_seqno++;
                    /* checksum */
#ifdef ASIL_COMPLIANCE
                    hash = krn_schdrmsg_calc_hash(&dst->hdr, 12U, hash);
                    hash = krn_schdrmsg_calc_hash(&dst->msg, sizeof(dst->msg), hash);
                    dst->hdr.message_checksum = hash;
#else /* !ASIL_COMPLIANCE */
                    dst->hdr.message_checksum = 0;
#endif /* ?ASIL_COMPLIANCE */


                    if(retcode == ERRCODE_NONE) {
                        //krn_schdrmsg_log_msg(dst);
                        retcode = krn_cache_clean_schdrmsg(dst);
                    }
                }

                if(retcode == ERRCODE_NONE) {
                    wi = wi + 1U;
                    *qconfig.from_arm_w = wi;
                    retcode = krn_cache_clean(qconfig.from_arm_w, sizeof(*qconfig.from_arm_w));
                    // trigger ORC to receive message
                    krn_visorc_armidsp_queue_kick();
                }
            }
            ret = krn_mutex_unlock(&tx_lock);
            if(retcode == ERRCODE_NONE) {
                retcode = ret;
            }
        }
    }
    return retcode;
}

uint32_t krn_schdrmsg_init(void)
{
    uint32_t retcode = ERRCODE_NONE;
    static uint32_t schdrmsg_inited = 0U;

    if (schdrmsg_inited == 0U) {
        retcode = krn_eventflag_init(&rx_lock);
        if(retcode == ERRCODE_NONE) {
            retcode = krn_eventflag_init(&msg_wq);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = krn_mutex_init(&tx_lock);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = krn_mutex_init(&find_lock);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = krn_mutex_init(&wait_lock);
        }

#if defined (CHIP_CV6)
        if(retcode == ERRCODE_NONE) {
            retcode = krn_mutex_init(&event_time_lock);
        }
#endif

        if(retcode == ERRCODE_NONE) {
            retcode = krn_idsporc_init();
        }

        if(retcode == ERRCODE_NONE) {
            retcode = krn_vinorc_init();
        }

        if(retcode == ERRCODE_NONE) {
            schdrmsg_inited = 1U;
        }
    }

    return retcode;
}

void krn_schdrmsg_shutdown(void)
{
    ambacv_all_mem_t *mem_all;

    if(rx_kthread_state == 1U) {
        if(krn_eventflag_set(&rx_lock, EFLAG_STOP) != ERRCODE_NONE) {
            krn_printU5("[ERROR] krn_schdrmsg_shutdown() : krn_eventflag_set fail", 0U, 0U, 0U, 0U, 0U);
        }
        if(krn_schdr_with_thread == 1U) {
            if(krn_thread_delete(prx_kthread) != ERRCODE_NONE) {
                krn_printU5("[ERROR] krn_schdrmsg_shutdown() : krn_thread_delete fail", 0U, 0U, 0U, 0U, 0U);
            }
        }
        rx_kthread_state = 0U;
    }
    prx_kthread = &rx_kthread;
    krn_flexidag_shutdown();
    mem_all = krn_ambacv_get_mem();
    mem_all->arm.base = 0UL;
    mem_all->arm.size = 0UL;
    krn_module_printU5("ambacv: schdr cleanup finished\n", 0U, 0U, 0U, 0U, 0U);
}

