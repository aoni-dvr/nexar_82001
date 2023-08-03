/**
 *  @file schdr_internal.h
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
 *  @details Definitions & Constants for Scheduler internal APIs
 *
 */

#ifndef SCHDR_INTERNAL_H
#define SCHDR_INTERNAL_H

scheduler_trace_t  *drv_schdr_get_pSchedulerTrace(void);
autorun_trace_t    *drv_schdr_get_pAutoRunTrace(void);
flexidag_trace_t   *drv_schdr_get_pFlexidagTrace(void);
cavalry_trace_t    *drv_schdr_get_pCavalryTrace(void);

void drv_schdr_set_pSchedulerTrace(uint64_t trace_daddr);
void drv_schdr_set_pAutoRunTrace(uint64_t trace_daddr);
void drv_schdr_set_pFlexidagTrace(uint64_t trace_daddr);
void drv_schdr_set_pCavalryTrace(uint64_t trace_daddr);

uint32_t schdr_core_mutex(uint32_t mode);

uint32_t schdr_check_version(int32_t fd, uint32_t *version);

uint32_t schdr_log_init(uint64_t base,
                        const schedmsg_boot_setup2_t *r,
                        uint64_t cvtask_size,
                        uint64_t sched_size,
                        uint64_t perf_size);

uint32_t schdr_log_reattach(uint64_t base);

uint32_t schdr_log_reset(void);

uint32_t schdr_startup_log_flush(uint64_t state_addr, uint32_t boot_os);

uint32_t schdr_log_flush(uint64_t state_addr, uint32_t boot_os);

uint32_t schdr_set_env(cvtask_thpool_env_t *env, tid_t thread_id);

void schdr_init_names(void);

uint32_t schdr_init_sysflow(uint64_t base, uint32_t numlines);

uint32_t schdr_reinit_sysflow(void);

uint32_t schdr_init_buildinfo(uint64_t cbase, uint64_t sbase, uint32_t numlines);

uint32_t schdr_init_system_msg(uint64_t msgpool);

uint32_t schdr_cvtable_patch_addr(uint64_t base, uint64_t size);

uint32_t schdr_init_cvtable(uint64_t base, uint32_t numlines, uint32_t disable_log);

uint32_t schdr_internal_cvtable_reset(void);

uint32_t schdr_internal_cvtable_get_size(void);

uint32_t schdr_internal_cvtable_copy(char *dst);

uint32_t schdr_check_cvtable_size(void);

uint32_t schdr_cvtable_get_size(void);

uint32_t schdr_sysflow_get_size(void);

uint32_t schdr_handle_sysflow(schedmsg_boot_setup2_reply_t *rpl, uint64_t sysflow_addr);

uint32_t schdr_handle_cvtable(schedmsg_boot_setup2_reply_t *rpl, uint64_t cvtable_addr);

void schdr_sysflow_cvtable_reset(void);

uint32_t schdr_stop_visorc(void);

uint32_t schdr_process_shutdown(uint32_t flag, uint64_t state_addr);

void drv_handle_softreset(const armvis_msg_t *pMsg);

#endif //SCHDR_INTERNAL_H

