/**
 *  @file flexidag_kernel.h
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
 *  @details Definitions & Constants for flexidag kernel
 *
 */

#ifndef FLEXIDAG_KERNEL_H_FILE
#define FLEXIDAG_KERNEL_H_FILE

#include "ambacv_kal.h"

#if defined (CHIP_CV28)
#define   MAX_FLEXIDAG_TOKENS   32U
#elif defined(CHIP_CV6)
#define   MAX_FLEXIDAG_TOKENS   256U
#else
#define   MAX_FLEXIDAG_TOKENS   128U
#endif

typedef struct {
    ksem_t      run_mutex;
    ksem_t      pending_run_semaphore;

    uint64_t    owner;
    uint32_t    flexidag_slot_id;
    uint32_t    pending_token_id;
    int32_t     pending_run_count;
    /*-= Flexidag config =--------------------------------------------*/

    uint32_t    fdparam_interface_cvmsg_num;
    uint32_t    fdparam_internal_cvmsg_num;
    uint32_t    fdparam_log_size_arm_perf[SYSTEM_MAX_NUM_CORTEX];
    uint32_t    fdparam_log_size_arm_sched[SYSTEM_MAX_NUM_CORTEX];
    uint32_t    fdparam_log_size_arm_cvtask[SYSTEM_MAX_NUM_CORTEX];
    uint32_t    fdparam_log_size_vis_perf[SYSTEM_MAX_NUM_VISORC];
    uint32_t    fdparam_log_size_vis_sched[SYSTEM_MAX_NUM_VISORC];
    uint32_t    fdparam_log_size_vis_cvtask[SYSTEM_MAX_NUM_VISORC];
    uint32_t    fdparam_token_timeout;

    /*-= Flexidag memory requirements =-------------------------------*/
    flexidag_memory_requirements_t memreq;

    void       *vpFlexiBin;
    void       *vpStateBuffer;
    void       *vpTempBuffer;
    void       *vpInputBuffer[FLEXIDAG_MAX_INPUTS];
    void       *vpFeedbackBuffer[FLEXIDAG_MAX_FEEDBACKS];
    void       *vpOutputBuffer[FLEXIDAG_MAX_OUTPUTS];
    uint64_t   pNextOutputMemblk[FLEXIDAG_MAX_OUTPUTS];
    void       *vpExtraTbar[FLEXIDAG_MAX_TBAR];
    void       *vpExtraSfb[FLEXIDAG_MAX_SFB];

    uint64_t    flexibin_daddr;
    uint64_t    flexibin_size;
    uint64_t    state_buffer_daddr;
    uint64_t    state_buffer_size;
    uint64_t    temp_buffer_daddr;
    uint64_t    temp_buffer_size;
    uint64_t    input_buffer_daddr[FLEXIDAG_MAX_INPUTS];
    uint64_t    feedback_buffer_daddr[FLEXIDAG_MAX_FEEDBACKS];
    uint64_t    output_buffer_daddr[FLEXIDAG_MAX_OUTPUTS];
    uint64_t    input_buffer_size[FLEXIDAG_MAX_INPUTS];
    uint64_t    feedback_buffer_size[FLEXIDAG_MAX_FEEDBACKS];
    uint64_t    output_buffer_size[FLEXIDAG_MAX_OUTPUTS];
    uint64_t    extra_tbar_daddr[FLEXIDAG_MAX_TBAR];
    uint32_t    num_extra_tbar;
    uint64_t    extra_sfb_daddr[FLEXIDAG_MAX_SFB];
    uint16_t    extra_sfb_numlines[FLEXIDAG_MAX_SFB];
    uint32_t    num_extra_sfb;
} flexidag_slot_state_t;


typedef struct {
    kmutex_t                slot_mutex;
    kmutex_t                token_mutex;
    ksem_t                  token[MAX_FLEXIDAG_TOKENS];
    flexidag_slot_state_t  *pSlotState;
    uint8_t                 num_slots_enabled;
    uint8_t                 slot_state[FLEXIDAG_MAX_SLOTS];
    uint64_t                pOutputMemBlk[MAX_FLEXIDAG_TOKENS][FLEXIDAG_MAX_OUTPUTS];
    uint32_t                next_token_index;
    uint64_t                flexidag_trace_daddr;
    void                    *vpFlexidagTrace;
    uint8_t                 token_state[MAX_FLEXIDAG_TOKENS];
    uint32_t                token_retcode[MAX_FLEXIDAG_TOKENS];
    uint32_t                token_start_time[MAX_FLEXIDAG_TOKENS];
    uint32_t                token_end_time[MAX_FLEXIDAG_TOKENS];
    uint32_t                token_output_not_generated[MAX_FLEXIDAG_TOKENS];
    uint32_t                token_output_donetime[MAX_FLEXIDAG_TOKENS][FLEXIDAG_MAX_OUTPUTS];
} flexidag_system_support_t;

#define FLEXIDAG_SLOT_UNUSED      0U
#define FLEXIDAG_SLOT_USED        1U
#define FLEXIDAG_SLOT_UNAVAIL     2U

extern flexidag_system_support_t  flexidag_system_support;

uint32_t krn_flexidag_system_init(void);
uint32_t krn_flexidag_system_reset(uint32_t is_init);
uint32_t krn_flexidag_ioctl_process(void* f, uint32_t cmd, void * arg);
void krn_flexidag_system_shutdown(void);
uint64_t krn_flexidag_system_get_trace_daddr(void);
uint32_t krn_flexidag_system_set_trace_daddr(uint64_t trace_daddr);
uint32_t krn_flexidag_ioctl_handle_message(armvis_msg_t *pMsg);
void krn_flexidag_close_by_owner(uint64_t owner);
void krn_flexidag_shutdown(void);
#endif /* ?FLEXIDAG_KERNEL_H_FILE */

