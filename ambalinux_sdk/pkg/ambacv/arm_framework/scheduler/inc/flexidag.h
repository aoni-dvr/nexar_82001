/**
 *  @file flexidag.h
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
 *  @details Definitions & Constants for Flexidag APIs
 *
 */

#ifndef FLEXIDAG_H
#define FLEXIDAG_H

typedef struct {
    char*                   pName;
    void*                   pHandle;

    sysflow_entry_t*        pSysFlow;
    uint32_t                sysflow_numlines;
    cvtable_entry_t*        pCvtable;
    uint32_t                cvtable_numlines;
    cvmem_messagepool_t*    pMsgpool;
    schdr_log_info_t*       pLinfo[SYSTEM_MAX_NUM_CORTEX];

    cvlog_buffer_info_t*    pVisPerfLogCtrl[SYSTEM_MAX_NUM_VISORC];
    cvlog_buffer_info_t*    pVisSchedLogCtrl[SYSTEM_MAX_NUM_VISORC];
    cvlog_buffer_info_t*    pVisCVTaskLogCtrl[SYSTEM_MAX_NUM_VISORC];
    cvlog_buffer_info_t*    pArmPerfLogCtrl[SYSTEM_MAX_NUM_CORTEX];
    cvlog_buffer_info_t*    pArmSchedLogCtrl[SYSTEM_MAX_NUM_CORTEX];
    cvlog_buffer_info_t*    pArmCVTaskLogCtrl[SYSTEM_MAX_NUM_CORTEX];
    uint32_t                vis_perf_rdidx[SYSTEM_MAX_NUM_VISORC];
    uint32_t                vis_sched_rdidx[SYSTEM_MAX_NUM_VISORC];
    uint32_t                vis_cvtask_rdidx[SYSTEM_MAX_NUM_VISORC];
    uint32_t                arm_perf_rdidx[SYSTEM_MAX_NUM_CORTEX];
    uint32_t                arm_sched_rdidx[SYSTEM_MAX_NUM_CORTEX];
    uint32_t                arm_cvtask_rdidx[SYSTEM_MAX_NUM_CORTEX];

    void*                   pHotlinkBase;
    void*                   pVisorcBase[SYSTEM_MAX_NUM_VISORC];
    uint16_t                curr_frameset_id;
    uint16_t                finish_frameset_id;

    void*                   pCallbackParam[FLEXIDAG_MAX_OUTPUTS];
    flexidag_cb             callback_array[FLEXIDAG_MAX_OUTPUTS];
    void*                   pErrorCallbackParam;
    flexidag_error_cb       error_callback;
    void*                   pVPCoredumpCallbackParam;
    flexidag_error_cb       vpcoredump_callback;
} flexidag_system_state_t;

extern flexidag_system_state_t flexidag_sys_state[FLEXIDAG_MAX_SLOTS];
extern psem_t                  flexidag_sys_sem[FLEXIDAG_MAX_SLOTS];

uint32_t is_associated_flexidag(const armvis_msg_t *pMsg);

uint32_t drv_flexidag_process_kernel_reply(armvis_msg_t *pMsg);

void fd_prof_unit_meta(uint32_t event, uint32_t sub_event, uint32_t flexidag_slot_id, uint16_t frameset_id);
uint32_t flexidag_system_resume(uint32_t version);
uint32_t flexidag_log_system_init(const armvis_msg_t *pMsg);
uint32_t flexidag_init_system_msg(uint32_t flexidag_slot_id, uint64_t msgpool);
uint32_t flexidag_init_sysflow(uint32_t flexidag_slot_id, uint64_t base, uint32_t numlines);
uint32_t flexidag_init_cvtable(uint32_t flexidag_slot_id, uint64_t base, uint32_t numlines, uint32_t disable_log);
uint32_t flexidag_log_init(const armvis_msg_t *pMsg);
uint32_t flexidag_system_reset_slot(uint32_t flexidag_slot_id);
uint32_t flexidag_handle_open_parsed(uint32_t flexidag_slot_id, const schedmsg_flexidag_open_parsed_t *pOpenParsed);
uint32_t flexidag_handle_reg(const armvis_msg_t *msg);

#endif //FLEXIDAG_H

