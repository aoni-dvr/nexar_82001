/**
 *  @file schdr_kernel.h
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
 *  @details Definitions & Constants for schdr kernel
 *
 */

#ifndef SCHDR_KERNEL_H_FILE
#define SCHDR_KERNEL_H_FILE

#include "ambacv_kal.h"

typedef struct {
    uint64_t  scheduler_trace_daddr;
    void     *vpSchedulerTrace;
    uint64_t  autorun_trace_daddr;
    void     *vpAutoRunTrace;
} cvflow_scheduler_support_t;

extern uint32_t ambacv_state;

extern uint32_t is_booting_os;
extern uint32_t cluster_id;
extern uint32_t audio_clock_ms;

extern cvflow_scheduler_support_t  scheduler_support;

void krn_schdr_get_isr_num(uint32_t *num);
void krn_schdr_isr(uint32_t irq, void* args);
uint32_t krn_schdr_start_visorc(const visorc_init_params_t *arg);
uint32_t krn_schdr_stop_visorc(void);
uint32_t krn_schdr_resume_visorc(visorc_init_params_t *arg);
uint32_t krn_schdr_get_sysconfig(visorc_init_params_t *arg);
uint32_t krn_schdr_find_entry(ambacv_get_value_t *arg);
uint32_t krn_schdr_set_trace_daddr(uint64_t trace_daddr);
uint64_t krn_schdr_get_trace_daddr(void);
uint32_t krn_schdr_set_autorun_trace_daddr(uint64_t trace_daddr);
uint64_t krn_schdr_get_autorun_trace_daddr(void);
uint32_t krn_schdr_autorun_info(void);

#endif /* ?SCHDRMSG_KERNEL_H_FILE */

