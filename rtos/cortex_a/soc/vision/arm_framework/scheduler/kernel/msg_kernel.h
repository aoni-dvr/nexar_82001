/**
 *  @file msg_kernel.h
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
 *  @details Definitions & Constants for msg kernel
 *
 */

#ifndef MSG_KERNEL_H_FILE
#define MSG_KERNEL_H_FILE

#include "ambacv_kal.h"

#define EFLAG_NEW_MSG           0x1U
#define EFLAG_STOP              0x2U
#define EFLAG_MASK              0x3U

#define FNV1A_32_INIT_HASH              0x811C9DC5U

typedef struct {
    uint32_t*           from_orc_r;
    uint32_t*           from_orc_w;
    armvis_msg_t*       from_orc_q;
    uint32_t            from_orc_wi;
    uint32_t            from_orc_w_seqno;

    uint32_t*           from_arm_r;
    uint32_t*           from_arm_w;
    armvis_msg_t*       from_arm_q;
    uint32_t            from_arm_w_seqno;

    uint64_t            reqQ_addr;
    uint64_t            rplQ_addr;
} q_config_t ;

extern kthread_t        rx_kthread;
extern kthread_t        *prx_kthread;
extern uint32_t         rx_kthread_state;
extern keven_t          rx_lock;
extern kmutex_t         find_lock;
extern keven_t          msg_wq;
extern q_config_t       qconfig;
extern uint32_t         krn_schdr_with_thread;

uint32_t krn_schdrmsg_rx_entry(void* arg);
uint32_t krn_schdrmsg_get_orcarm_wi(void);
uint32_t krn_schdrmsg_init(void);
void krn_schdrmsg_shutdown(void);
uint32_t krn_schdrmsg_recv_orcmsg(ambacv_get_value_t *arg);
uint32_t krn_schdrmsg_send_armmsg(const armvis_msg_t *arg, uint32_t is_from_kernel_space);
uint32_t krn_schdrmsg_rx_wait(uint32_t *ret);
uint32_t krn_schdrmsg_calc_hash(const void* buf, uint32_t len, uint32_t hval);

void krn_schdrmsg_set_event_time(void);
uint32_t krn_schdrmsg_get_event_time(void);

#endif /* ?MSG_KERNEL_H_FILE */

