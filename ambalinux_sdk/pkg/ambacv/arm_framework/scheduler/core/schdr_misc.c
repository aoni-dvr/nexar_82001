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
#include "schdr_util.h"
#include "cvtask_errno.h"
#include "cvapi_logger_interface.h"
#include "ambacv_kal.h"

#if defined(AMBACV_KERNEL_SUPPORT) || defined(RTOS_BUILD)
scheduler_trace_t *drv_schdr_get_pSchedulerTrace(void)
{
    uint32_t retcode = ERRCODE_NONE;
    scheduler_trace_t *ptrace = NULL;
    ambacv_get_value_t arg;
    const void *ptr;

    if (ambacv_fd < 0) {
        ambacv_fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
        if (ambacv_fd < 0) {
            console_printU5("[ERROR] drv_schdr_get_pSchedulerTrace() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
        } /* if (ambacv_fd < 0) : open("/dev/ambacv", ...) */
    } /* if (ambacv_fd == 0) */

    if(retcode == ERRCODE_NONE) {
        arg.index = AMBACV_ID_GET_SCHDR_TRACE;
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_GET_VALUE, &arg);
        if (retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] drv_schdr_get_pSchedulerTrace() : Can't retrieve pSchedulerTrace ret = 0x%x\n", retcode, 0U, 0U, 0U, 0U);
        } /* if (retcode != ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
        else { /* if (retcode == ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
            ptr = ambacv_c2v(arg.value);
            typecast(&ptrace,&ptr);
        } /* if (retcode == ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
    }
    return ptrace;
} /* drv_schdr_get_pSchedulerTrace() */

autorun_trace_t *drv_schdr_get_pAutoRunTrace(void)
{
    uint32_t retcode = ERRCODE_NONE;
    autorun_trace_t *ptrace = NULL;
    ambacv_get_value_t arg;
    const void *ptr;

    if (ambacv_fd < 0) {
        ambacv_fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
        if (ambacv_fd < 0) {
            console_printU5("[ERROR] drv_schdr_get_pAutoRunTrace() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
        } /* if (ambacv_fd < 0) : open("/dev/ambacv", ...) */
    } /* if (ambacv_fd == 0) */

    if(retcode == ERRCODE_NONE) {
        arg.index = AMBACV_ID_GET_AUTORUN_TRACE;
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_GET_VALUE, &arg);
        if (retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] drv_schdr_get_pAutoRunTrace() : Can't retrieve drv_schdr_get_pAutoRunTrace ret = 0x%x\n", retcode, 0U, 0U, 0U, 0U);
        } /* if (retcode != ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
        else { /* if (retcode == ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
            ptr = ambacv_c2v(arg.value);
            typecast(&ptrace,&ptr);
        } /* if (retcode == ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
    }
    return ptrace;
} /* drv_schdr_get_pAutoRunTrace() */

flexidag_trace_t *drv_schdr_get_pFlexidagTrace(void)
{
    flexidag_trace_t *ptrace = NULL;
    uint32_t retcode = ERRCODE_NONE;
    ambacv_get_value_t arg;
    const void *ptr;

    if (ambacv_fd < 0) {
        ambacv_fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
        if (ambacv_fd < 0) {
            console_printU5("[ERROR] drv_schdr_get_pFlexidagTrace() : Can't open /dev/ambacv !", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_DEV_UNAVAILABLE;
        } /* if (ambacv_fd < 0) : open("/dev/ambacv", ...) */
    } /* if (ambacv_fd == 0) */

    if(retcode == ERRCODE_NONE) {
        arg.index = AMBACV_ID_GET_FLEXIDAG_TRACE;
        retcode = (uint32_t) ambacv_ioctl(ambacv_fd, AMBACV_GET_VALUE, &arg);
        if (retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] drv_schdr_get_pFlexidagTrace() : Can't retrieve pFlexidagTrace retcode = 0x%x\n", retcode, 0U, 0U, 0U, 0U);
        } /* if (retcode != ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
        else { /* if (retcode == ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
            ptr = ambacv_c2v(arg.value);
            typecast(&ptrace,&ptr);
        } /* if (retcode == ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
    }
    return ptrace;
}

static uint32_t cavalry_dummy = 0U;
cavalry_trace_t *drv_schdr_get_pCavalryTrace(void)
{
    cavalry_trace_t *ptrace = NULL;
    uint32_t retcode = ERRCODE_NONE;
    ambacv_get_value_t arg;
    const void *ptr;

    (void) cavalry_dummy;
    if (ambacv_fd < 0) {
        ambacv_fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
        if (ambacv_fd < 0) {
            console_printU5("[ERROR] drv_schdr_get_pCavalryTrace() : Can't open /dev/ambacv !", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
        } /* if (ambacv_fd < 0) : open("/dev/ambacv", ...) */
    } /* if (ambacv_fd == 0) */

    if(retcode == ERRCODE_NONE) {
        arg.index = AMBACV_ID_GET_CAVALRY_TRACE;
        retcode = (uint32_t) ambacv_ioctl(ambacv_fd, AMBACV_GET_VALUE, &arg);
        if (retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] drv_schdr_get_pCavalryTrace() : Can't retrieve pFlexidagTrace ret = 0x%x\n", retcode, 0U, 0U, 0U, 0U);
        } /*if (retcode != ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
        else { /*if (retcode == ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
            if(arg.value != 0UL) {
                ptr = ambacv_c2v(arg.value);
                typecast(&ptrace,&ptr);
            }
        } /* if (retcode == ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
    }
    return ptrace;
}

void drv_schdr_set_pSchedulerTrace(uint64_t trace_daddr)
{
    static uint32_t schdr_trace_dummy = 0U;

    (void) trace_daddr;
    (void) schdr_trace_dummy;
    schdr_trace_dummy = 1U;
    /* do nothing */
}

void drv_schdr_set_pAutoRunTrace(uint64_t trace_daddr)
{
    static uint32_t autorun_trace_dummy = 0U;

    (void) trace_daddr;
    (void) autorun_trace_dummy;
    autorun_trace_dummy = 1U;
    /* do nothing */
}

void drv_schdr_set_pFlexidagTrace(uint64_t trace_daddr)
{
    static uint32_t flexidag_trace_dummy = 0U;

    (void) trace_daddr;
    (void) flexidag_trace_dummy;
    flexidag_trace_dummy = 1U;

    /* do nothing */
} /* drv_flexidag_system_set_trace_daddr() */

void drv_schdr_set_pCavalryTrace(uint64_t trace_daddr)
{
    (void) trace_daddr;
    cavalry_dummy = 1U;

    /* do nothing */
} /* drv_schdr_set_pCavalryTrace() */

void drv_handle_softreset(const armvis_msg_t *pMsg)
{
    static uint32_t reset_dummy = 0U;

    reset_dummy = 1U;
    (void)reset_dummy;
    (void)pMsg;
    /* Handled this in kernel */
}

uint32_t cvtask_get_timestamp(void)
{
    uint32_t at_time = 0U;

    if (ambacv_fd < 0) {
        ambacv_fd = ambacv_open("/dev/ambacv", O_RDONLY);
    }
    if (ambacv_fd >= 0) {
        if( ambacv_ioctl(ambacv_fd, AMBACV_GET_TIME, &at_time) != ERRCODE_NONE) {
            console_printU5("[ERROR] cvtask_get_timestamp() : ambacv_ioctl fail ", 0U, 0U, 0U, 0U, 0U);
        }
    } else {
        console_printU5("[ERROR] cvtask_get_timestamp() : Can't open /dev/ambacv !\n ", 0U, 0U, 0U, 0U, 0U);
    }

    return at_time;
}

uint32_t drv_schdr_find_msg(uint64_t addr, uint64_t *entry)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_get_value_t arg;

    arg.index = addr;
    retcode = ambacv_ioctl(ambacv_fd, AMBACV_FIND_CVTASKMSG, &arg);
    if(retcode != ERRCODE_NONE) {
        console_printU5("[ERROR] drv_schdr_find_msg() : AMBACV_FIND_CVTASKMSG fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
    }
    *entry = arg.value;
    return retcode;
}

uint32_t schdr_get_audio_tick(uint32_t *freq)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_get_value_t arg= {0};

    if (ambacv_fd < 0) {
        ambacv_fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
        if (ambacv_fd < 0) {
            console_printU5("[ERROR] schdr_get_audio_tick() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
        } /* if (ambacv_fd < 0) : open("/dev/ambacv", ...) */
    } /* if (ambacv_fd == 0) */

    if(retcode == ERRCODE_NONE) {
        arg.index = AMBACV_ID_GET_AUDIO_CLOCK;
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_GET_VALUE, &arg);
        if (retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_get_audio_tick() :  ambacv_ioctl fail ret = 0x%x\n", retcode, 0U, 0U, 0U, 0U);
        } /* if (retcode != ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
        else { /* if (retcode == ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
            *freq = (uint32_t)arg.value;
        } /* if (retcode == ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
    }

    return retcode;
}

uint32_t schdr_get_perf_last_wridx_daddr(uint32_t visorc, uint32_t *w_index)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_get_value_t arg= {0};

    if (ambacv_fd < 0) {
        ambacv_fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
        if (ambacv_fd < 0) {
            console_printU5("[ERROR] schdr_get_perf_last_wridx_daddr() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
        } /* if (ambacv_fd < 0) : open("/dev/ambacv", ...) */
    } /* if (ambacv_fd == 0) */

    if(visorc >= SYSTEM_MAX_NUM_VISORC) {
        retcode = ERR_DRV_SCHDR_VISCORE_UNAVAILABLE;
    } else {
        if(retcode == ERRCODE_NONE) {
            arg.index = AMBACV_ID_GET_ORC_PERF_WI_BASE + (uint64_t)visorc;
            retcode = ambacv_ioctl(ambacv_fd, AMBACV_GET_VALUE, &arg);
            if (retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_get_perf_last_wridx_daddr() :  ambacv_ioctl fail ret = 0x%x\n", retcode, 0U, 0U, 0U, 0U);
            } /* if (retcode != ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
            else { /* if (retcode == ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
                *w_index = (uint32_t)arg.value;
            } /* if (retcode == ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
        }
    }

    return retcode;
}

uint32_t schdr_get_arm_perf_last_wridx_daddr(uint32_t *w_index)
{
    uint32_t retcode = ERRCODE_NONE;

    if (schdr_sys_state.pLinfo != NULL) {
        *w_index = schdr_sys_state.pLinfo[2].wridx;
    } else {
        *w_index = 0U;
    }

    return retcode;
}

uint32_t schdr_get_mmb_cache(uint64_t base, uint32_t *is_cached)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_get_info_t arg = {0};

    if (ambacv_fd < 0) {
        ambacv_fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
        if (ambacv_fd < 0) {
            console_printU5("[ERROR] schdr_get_mmb_cache() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
        } /* if (ambacv_fd < 0) : open("/dev/ambacv", ...) */
    } /* if (ambacv_fd == 0) */

    if(retcode == ERRCODE_NONE) {
        arg.version = 0x1U;
        arg.index[0] = 0x1U;
        arg.index[1] = base;
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_GET_MMB_INFO, &arg);
        if (retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_get_mmb_cache() :  ambacv_ioctl fail ret = 0x%x\n", retcode, 0U, 0U, 0U, 0U);
        } else {
            *is_cached = (uint32_t)arg.value[0];
        }
    }

    return retcode;
}

#endif

int32_t schdr_set_callback(schdr_cb cb)
{
    uint32_t retcode = ERRCODE_NONE;

    if(cb == NULL) {
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        schdr_sys_state.app_callback = cb;
    }

    return (int32_t)retcode;
}

int32_t schdrcfg_set_flexidag_arm_scratchpad_size(uint32_t size)
{
    uint32_t retcode = ERRCODE_NONE;

    if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
        schdr_sys_cfg.max_sp_size = size;
    } else {
        console_printU5("[ERROR] schdrcfg_set_arm_scratchpad_size() : (%d) : Scheduler instance already active - cannot reconfigure scheduler arm scratchpad size \n", size, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
    }

    return (int32_t)retcode;
} /* schdrcfg_set_flexidag_arm_scratchpad_size() */

int32_t schdr_flush_visorc_printf(uint32_t flush_type)
{
    uint32_t retcode;
#if defined(AMBACV_KERNEL_SUPPORT) || defined(RTOS_BUILD)
    int32_t fd = -1;

    if (schdr_sys_state.state == SCHDR_STATE_ACTIVE) {
        fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
        if (fd < 0) {
            console_printU5("[ERROR] schdr_flush_visorc_printf() : Can't open /dev/ambacv !", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
        } /* if (local_ambacv_fd < 0) */
        else { /* if (local_ambacv_fd >= 0) */
            ambacv_get_value_t getval;

            getval.index  = 9999U;
            getval.value  = 0U;
            retcode       = ambacv_ioctl(fd, AMBACV_GET_VALUE, &getval);

            if ((retcode == ERRCODE_NONE) && (getval.value != 0U)) {
                armvis_msg_t flushreq = {0};

                flushreq.hdr.message_type     = (uint16_t) SCHEDMSG_FLUSH_VISPRINTF_REQ;
                flushreq.hdr.message_id       = (uint16_t) 0U;
                flushreq.hdr.message_retcode  = ERRCODE_NONE;
                flushreq.msg.flush_visprintf_request.token_id   = (uint32_t)getval.value;
                flushreq.msg.flush_visprintf_request.flush_type = flush_type;
                retcode = schdrmsg_send_msg(&flushreq, (uint32_t)(sizeof(flushreq.hdr) + sizeof(flushreq.msg.flush_visprintf_request)));
                if (retcode == ERRCODE_NONE) {
                    getval.index = getval.value;
                    getval.value = 0U;
                    retcode = ambacv_ioctl(fd, AMBACV_GET_VALUE, &getval);  /* wait on token */
                } /* if (retcode == ERRCODE_NONE) : schdrmsg_send_msg() */
            } /* if ((retcode == ERRCODE_NONE) && (getval.value != 0U)) : ioctl(AMBACV_GET_VALUE) */
            if (ambacv_close(fd) != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_flush_visorc_printf() : ambacv_close fail ",0U, 0U, 0U, 0U, 0U);
            }
        } /* if (local_ambacv_fd >= 0) */
    } else {
        retcode = ERR_DRV_SCHDR_NOT_ACTIVATE;
        console_printU5("[ERROR] schdr_flush_visorc_printf() : scheduler not active \n", 0U, 0U, 0U, 0U, 0U);
    }
#else
    console_printU5("[WARNING] schdr_flush_visorc_printf() : not support", 0U, 0U, 0U, 0U, 0U);
#endif
    return (int32_t)retcode;

} /* schdr_flush_visorc_printf() */

errcode_enum_t  schdr_set_error_callback(void *vpHandle, schdr_error_cb error_callback, void *vpParameter)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) vpHandle;
    (void) vpParameter;
    if(error_callback == NULL) {
        console_printU5("[ERROR] schdr_set_error_callback() : error_callback == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        thread_unused(vpHandle);
        thread_unused(vpParameter);
        schdr_sys_state.error_callback        = error_callback;
    }
    return retcode;
} /* schdr_set_error_callback() */

errcode_enum_t  schdr_set_coredump_callback(void *vpHandle, uint32_t cvcore_type, schdr_error_cb error_callback, void *vpParameter)
{
    uint32_t retcode = ERRCODE_NONE;

    if(error_callback == NULL) {
        console_printU5("[ERROR] schdr_set_coredump_callback() : error_callback == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        if (cvcore_type == (uint32_t)CVCORE_VP0) {
            schdr_sys_state.vpcoredump_callback        = error_callback;
            schdr_sys_state.pVPCoredumpCallbackHandle  = vpHandle;
            schdr_sys_state.pVPCoredumpCallbackParam   = vpParameter;
        } /* if (cvcore_type == CVCORE_VP0) */
        else { /* if (cvcore_type != CVCORE_VP0) */
            retcode = ERR_DRV_SCHDR_VISCORE_UNAVAILABLE;
        } /* if (cvcore_type != CVCORE_VP0) */
    }

    return retcode;
} /* schdr_set_coredump_callback() */

int32_t visorc_security_send_payload(void *vpPayload)
{
    uint32_t retcode = ERRCODE_NONE;
    armvis_msg_t msg = {0};

    if (schdr_sys_state.state == SCHDR_STATE_ACTIVE) {
        if(vpPayload == NULL) {
            console_printU5("[ERROR] visorc_security_send_payload() : vpPayload == NULL ", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
        } else {
            (void) vpPayload;
            thread_unused(vpPayload);
            msg.hdr.message_type  = SCHEDMSG_LICENSE_UPDATE;
            retcode = thread_memcpy(&msg.msg.license_update_req.payload[0], vpPayload, sizeof(msg.msg.license_update_req.payload));
            if(retcode == ERRCODE_NONE) {
                retcode = schdrmsg_send_msg(&msg, (uint32_t)sizeof(msg));
                if(retcode != ERRCODE_NONE) {
                    console_printU5("[ERROR] visorc_security_send_payload() : schdrmsg_send_msg fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
                }
            }
        }
    } else {
        retcode = ERR_DRV_SCHDR_NOT_ACTIVATE;
        console_printU5("[ERROR] visorc_security_send_payload() : scheduler not active \n", 0U, 0U, 0U, 0U, 0U);
    }
    return (int32_t)retcode;
} /* visorc_security_send_payload() */

scheduler_trace_t *schdr_get_pSchedulerTrace(void)
{
    scheduler_trace_t *ptr = NULL;

    if (schdr_sys_state.state == SCHDR_STATE_ACTIVE) {
        ptr = drv_schdr_get_pSchedulerTrace();
    } else {
        console_printU5("[ERROR] schdr_get_pSchedulerTrace() : scheduler not active \n", 0U, 0U, 0U, 0U, 0U);
    }
    return ptr;
} /* schdr_get_pSchedulerTrace() */

flexidag_trace_t *schdr_get_pFlexidagTrace(void)
{
    flexidag_trace_t *ptr = NULL;

    if (schdr_sys_state.state == SCHDR_STATE_ACTIVE) {
        ptr = drv_schdr_get_pFlexidagTrace();
    } else {
        console_printU5("[ERROR] schdr_get_pSchedulerTrace() : scheduler not active \n", 0U, 0U, 0U, 0U, 0U);
    }
    return ptr;
} /* schdr_get_pFlexidagTrace() */

cavalry_trace_t *schdr_get_pCavalryTrace(void)
{
    cavalry_trace_t *ptr = NULL;

#ifndef DISABLE_CAVALRY
    if (schdr_sys_state.state == SCHDR_STATE_ACTIVE) {
        ptr = drv_schdr_get_pCavalryTrace();
    } else {
        console_printU5("[ERROR] schdr_get_pCavalryTrace() : scheduler not active \n", 0U, 0U, 0U, 0U, 0U);
    }
#else
    console_printU5("[ERROR] schdr_get_pSchedulerTrace() : DISABLE_CAVALRY \n", 0U, 0U, 0U, 0U, 0U);
#endif

    return ptr;
} /* schdr_get_pCavalryTrace() */

autorun_trace_t *schdr_get_pAutoRunTrace(void)
{
    autorun_trace_t *ptr = NULL;

    if (schdr_sys_state.state == SCHDR_STATE_ACTIVE) {
        ptr = drv_schdr_get_pAutoRunTrace();
    } else {
        console_printU5("[ERROR] schdr_get_pAutoRunTrace() : scheduler not active \n", 0U, 0U, 0U, 0U, 0U);
    }
    return ptr;
} /* schdr_get_pAutoRunTrace() */

int32_t schdr_get_rtos_region(uint64_t *base, uint64_t *size, uint32_t *is_cached)
{
    uint32_t retcode = ERRCODE_NONE;

    if (schdr_sys_state.state == SCHDR_STATE_ACTIVE) {
        if(base == NULL) {
            console_printU5("[ERROR] schdr_get_rtos_region() : base == NULL ", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
        } else if(size == NULL) {
            console_printU5("[ERROR] schdr_get_rtos_region() : size == NULL ", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
        } else if(is_cached == NULL) {
            console_printU5("[ERROR] schdr_get_rtos_region() : is_cached == NULL ", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
        } else {
            if(schdr_sys_state.visorc_init.rtos_region_size != 0U) {
                *base = ambacv_c2p(schdr_sys_state.visorc_init.rtos_region_daddr);
                *size = schdr_sys_state.visorc_init.rtos_region_size;
            } else {
                *base = 0U;
                *size = 0U;
            }
            if((schdr_sys_state.visorc_init.region_cache_flags & CACHE_FLAG_RTOS) != 0U) {
                *is_cached = 1U;
            } else {
                *is_cached = 0U;
            }
        }
    } else {
        retcode = ERR_DRV_SCHDR_NOT_ACTIVATE;
        console_printU5("[ERROR] schdr_get_rtos_region() : scheduler not active \n", 0U, 0U, 0U, 0U, 0U);
    }
    return (int32_t)retcode;
} /* schdr_get_rtos_region() */

#if defined(AMBACV_KERNEL_SUPPORT) || defined(RTOS_BUILD)
errcode_enum_t schdr_get_cma_region_info(uint64_t *pBase, uint64_t *pSize)
{
    uint32_t retcode = ERRCODE_NONE;

    if (schdr_sys_state.state == SCHDR_STATE_ACTIVE) {
        if(pBase == NULL) {
            console_printU5("[ERROR] schdr_get_cma_region_info() : pBase == NULL ", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
        } else if(pSize == NULL) {
            console_printU5("[ERROR] schdr_get_cma_region_info() : pSize == NULL ", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
        } else {
            visorc_init_params_t  local_system_config;
            int32_t local_ambacv_fd;

            local_ambacv_fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
            if (local_ambacv_fd < 0) {
                console_printU5("[ERROR] schdr_get_cma_region_info() : Can't open /dev/ambacv !", 0U, 0U, 0U, 0U, 0U);
                *pBase = 0U;
                *pSize = 0U;
                retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
            } /* if (local_ambacv_fd < 0) */
            else { /* if (local_ambacv_fd >= 0) */
                retcode = ambacv_ioctl(local_ambacv_fd, AMBACV_RESUME_VISORC, &local_system_config);
                if (retcode == ERRCODE_NONE) {
                    *pBase  = ambacv_c2p(local_system_config.cma_region_daddr);
                    *pSize  = local_system_config.cma_region_size;
                } /* if (retcode == ERRCODE_NONE) */
                else { /* if (retcode != ERRCODE_NONE) */
                    *pBase = 0U;
                    *pSize = 0U;
                } /* if (retcode != ERRCODE_NONE) */
                retcode = ambacv_close(local_ambacv_fd);
            } /* if (local_ambacv_fd >= 0) */
        } /* if ((pBase != NULL) && (pSize != NULL)) */
    } else {
        retcode = ERR_DRV_SCHDR_NOT_ACTIVATE;
        console_printU5("[ERROR] schdr_get_cma_region_info() : scheduler not active \n", 0U, 0U, 0U, 0U, 0U);
    }

    return retcode;

} /* schdr_get_cma_region() */

int32_t schdr_get_cvschdr_region_info(uint64_t *base, uint64_t *size)
{
    ambacv_mem_t mem;
    uint32_t retval = ERRCODE_NONE;
    int32_t fd = -1;

    if (schdr_sys_state.state == SCHDR_STATE_ACTIVE) {
        fd = ambacv_open("/dev/ambacv", O_SYNC | O_RDWR);
        if (fd < 0) {
            console_printU5("[ERROR] schdr_get_cvschdr_region_info() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
        } else {
            retval = ambacv_ioctl(fd, AMBACV_GET_MEM_LAYOUT, &mem);
            if (retval != 0U) {
                console_printU5("[ERROR] schdr_get_cvschdr_region_info() : Can't get memory layout from kernel !\n", 0U, 0U, 0U, 0U, 0U);
            } else if (mem.all.size == 0U) {
                console_printU5("[ERROR] schdr_get_cvschdr_region_info() : mem.all.size = 0 !\n", 0U, 0U, 0U, 0U, 0U);
            } else {
                *base = ambacv_c2p(mem.all.base);
                *size = mem.all.size;
            }

            if(ambacv_close(fd) != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_get_cvschdr_region_info() : ambacv_close fail", 0U, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        retval = ERR_DRV_SCHDR_NOT_ACTIVATE;
        console_printU5("[ERROR] schdr_get_cvschdr_region_info() : scheduler not active \n", 0U, 0U, 0U, 0U, 0U);
    }
    return (int32_t)retval;
}

int32_t schdr_get_cvshm_info(uint64_t *base, uint64_t *size)
{
    ambacv_mem_t mem;
    uint64_t start = 0UL, end = 0UL;
    uint32_t retval = ERRCODE_NONE;
    int32_t fd = -1;

    if (schdr_sys_state.state == SCHDR_STATE_ACTIVE) {
        fd = ambacv_open("/dev/ambacv", O_SYNC | O_RDWR);
        if (fd < 0) {
            console_printU5("[ERROR] schdr_get_cvshm_info() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
        } else {
            /*  Get ATT info    */
            retval = ambacv_att_init();
            if(retval == ERRCODE_NONE) {
                retval = ambacv_ioctl(fd, AMBACV_GET_MEM_LAYOUT, &mem);
                if (retval != 0U) {
                    console_printU5("[ERROR] schdr_get_cvshm_info() : Can't get memory layout from kernel !\n", 0U, 0U, 0U, 0U, 0U);
                } else if ((mem.arm.size == 0U)|| (mem.arm.base == 0U)) {
                    console_printU5("[ERROR] schdr_get_cvshm_info() : mem.arm.size || mem.arm.base !\n", 0U, 0U, 0U, 0U, 0U);
                } else {
                    // align memory region to 4K page
                    start = ambacv_c2p(mem.arm.base) & (~0xFFFUL);
                    end = (ambacv_c2p(mem.arm.base) + mem.arm.size + 0xFFFUL) & (~0xFFFUL);

                    if (base != NULL) {
                        *base = start;
                    }

                    if (size != NULL) {
                        *size = end - start;
                    }
                }
            }
            if(ambacv_close(fd) != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_get_cvshm_info() : ambacv_close fail", 0U, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        retval = ERR_DRV_SCHDR_NOT_ACTIVATE;
        console_printU5("[ERROR] schdr_get_cvshm_info() : scheduler not active \n", 0U, 0U, 0U, 0U, 0U);
    }
    return (int32_t)retval;
}

uint32_t schdr_get_event_time(void)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t event_time = 0U;
    ambacv_get_value_t arg= {0};

    if (ambacv_fd < 0) {
        ambacv_fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
        if (ambacv_fd < 0) {
            console_printU5("[ERROR] schdr_get_event_time() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
        } /* if (ambacv_fd < 0) : open("/dev/ambacv", ...) */
    } /* if (ambacv_fd == 0) */

    if(retcode == ERRCODE_NONE) {
        arg.index = AMBACV_ID_GET_EVENT_TIME;
        retcode = ambacv_ioctl(ambacv_fd, AMBACV_GET_VALUE, &arg);
        if (retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_get_event_time() :  ambacv_ioctl fail ret = 0x%x\n", retcode, 0U, 0U, 0U, 0U);
        } /* if (retcode != ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
        else { /* if (retcode == ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
            event_time = (uint32_t)arg.value;
        } /* if (retcode == ERRCODE_NONE) : ioctl(AMBACV_GET_VALUE) */
    }

    return event_time;
}
#endif

