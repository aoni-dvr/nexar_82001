/**
 *  @file amalgam_flexidag_kernel.c
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
 *  @details Amalgam Flexidag kernel APIs
 *
 */

#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <errno.h>
#include "dram_sync.h"
#include "schdrmsg_def.h"
#include "cvtask_hw.h"
#include "os_api.h"
#include "ambacv_kal.h"
#include "idsp_vis_msg.h"
#include <cvtask_errno.h>
#include <schdr_api.h>
#include <cvapi_flexidag.h>
#include <schdr.h>
#include "schdr_internal.h"
#include "msg.h"
#include "flexidag.h"
#include "flexidag_internal.h"
#include "cavalry.h"
#include <cvsched_sysflow_format.h>   /* for sysflow_entry_t */

#define   MAX_FLEXIDAG_TOKENS   32

typedef struct {
    pmutex_t    run_mutex;

    uint32_t    flexidag_slot_id;
    uint32_t    pending_token_id;

    /*-= Flexidag config =--------------------------------------------*/

    uint32_t    fdparam_interface_cvmsg_num;
    uint32_t    fdparam_internal_cvmsg_num;
    uint32_t    fdparam_log_size_arm_perf[SYSTEM_MAX_NUM_CORTEX];
    uint32_t    fdparam_log_size_arm_sched[SYSTEM_MAX_NUM_CORTEX];
    uint32_t    fdparam_log_size_arm_cvtask[SYSTEM_MAX_NUM_CORTEX];
    uint32_t    fdparam_log_size_vis_perf[SYSTEM_MAX_NUM_VISORC];
    uint32_t    fdparam_log_size_vis_sched[SYSTEM_MAX_NUM_VISORC];
    uint32_t    fdparam_log_size_vis_cvtask[SYSTEM_MAX_NUM_VISORC];

    /*-= Flexidag memory requirements =-------------------------------*/
    flexidag_memory_requirements_t memreq;

    void       *vpFlexiBin;
    void       *vpStateBuffer;
    void       *vpTempBuffer;
    void       *vpInputBuffer[FLEXIDAG_MAX_INPUTS];
    void       *vpFeedbackBuffer[FLEXIDAG_MAX_FEEDBACKS];
    void       *vpOutputBuffer[FLEXIDAG_MAX_OUTPUTS];
    flexidag_memblk_t *pNextOutputMemblk[FLEXIDAG_MAX_OUTPUTS];
    void       *vpExtraTbar[FLEXIDAG_MAX_TBAR];
    void       *vpExtraSfb[FLEXIDAG_MAX_SFB];

    uint32_t    flexibin_daddr;
    uint32_t    flexibin_size;
    uint32_t    state_buffer_daddr;
    uint32_t    temp_buffer_daddr;
    uint32_t    input_buffer_daddr[FLEXIDAG_MAX_INPUTS];
    uint32_t    feedback_buffer_daddr[FLEXIDAG_MAX_FEEDBACKS];
    uint32_t    output_buffer_daddr[FLEXIDAG_MAX_OUTPUTS];
    uint32_t    input_buffer_size[FLEXIDAG_MAX_INPUTS];
    uint32_t    feedback_buffer_size[FLEXIDAG_MAX_FEEDBACKS];
    uint32_t    output_buffer_size[FLEXIDAG_MAX_OUTPUTS];
    uint32_t    extra_tbar_daddr[FLEXIDAG_MAX_TBAR];
    uint32_t    extra_sfb_daddr[FLEXIDAG_MAX_SFB];
    uint16_t    extra_sfb_numlines[FLEXIDAG_MAX_SFB];
    uint32_t    num_extra_tbar;
    uint32_t    num_extra_sfb;

} flexidag_slot_state_t;

typedef struct {
    uint8_t                 num_slots_enabled;
    uint8_t                 slot_state[FLEXIDAG_MAX_SLOTS];
    flexidag_memblk_t      *pOutputMemBlk[MAX_FLEXIDAG_TOKENS][FLEXIDAG_MAX_OUTPUTS];
    pmutex_t                slot_mutex;
    pmutex_t                token_mutex;
    uint32_t                next_token_index;
    sem_t                   token[MAX_FLEXIDAG_TOKENS];
    uint8_t                 token_state[MAX_FLEXIDAG_TOKENS];
    uint32_t                token_retcode[MAX_FLEXIDAG_TOKENS];
    uint32_t                token_start_time[MAX_FLEXIDAG_TOKENS];
    uint32_t                token_end_time[MAX_FLEXIDAG_TOKENS];
    uint32_t                token_output_donetime[MAX_FLEXIDAG_TOKENS][FLEXIDAG_MAX_OUTPUTS];
} flexidag_system_support_t;


#define FLEXIDAG_TOKEN_UNUSED     0
#define FLEXIDAG_TOKEN_USED       1
#define FLEXIDAG_TOKEN_WAITING    2
#define FLEXIDAG_TOKEN_FINISHED   3

#define FLEXIDAG_SLOT_UNUSED      0
#define FLEXIDAG_SLOT_USED        1
#define FLEXIDAG_SLOT_UNAVAIL     2

static flexidag_slot_state_t      flexidag_slot_state[FLEXIDAG_MAX_SLOTS];
static flexidag_system_support_t  flexidag_system_support;

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG token management (AMALGAM)  =-------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t  flexidag_find_token(void)
{
    uint32_t  token_found;
    uint32_t  token_num;
    uint32_t  token_id;
    uint32_t  retcode;
    uint32_t  loop;

    token_found = 0;
    token_num   = 0;
    loop        = 0;

    thread_mutex_lock(&flexidag_system_support.token_mutex);
    while ((token_found == 0) && (loop < MAX_FLEXIDAG_TOKENS)) {
        token_id = (flexidag_system_support.next_token_index + loop) % MAX_FLEXIDAG_TOKENS;
        if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_UNUSED) {
            uint32_t  output_loop;
            token_found = 1;
            token_num   = token_id;
            flexidag_system_support.token_state[token_id]   = FLEXIDAG_TOKEN_USED;
            flexidag_system_support.token_retcode[token_id] = ERRCODE_NONE;
            flexidag_system_support.token_start_time[token_id] = 0;
            flexidag_system_support.token_end_time[token_id] = 0;
            for (output_loop = 0; output_loop < FLEXIDAG_MAX_OUTPUTS; output_loop++) {
                flexidag_system_support.token_output_donetime[token_id][output_loop] = 0;
            }
            flexidag_system_support.next_token_index        = (token_id + 1) % MAX_FLEXIDAG_TOKENS;
        } /* if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_UNUSED) */
        loop++;
    } /* while ((token_found == 0) && (loop < MAX_FLEXIDAG_TOKENS)) */
    thread_mutex_unlock(&flexidag_system_support.token_mutex);

    if (token_found == 0) {
        /* ERROR */
        retcode = 0xFFFFFFFF;
    } /* if (token_found == 0) */
    else { /* if (token_found != 0) */
        retcode = token_num;
    } /* if (token_found != 0) */

    return retcode;

} /* flexidag_find_token() */

uint32_t  flexidag_wait_token(uint32_t token_id, uint32_t block)
{
    uint32_t  wait_valid;
    uint32_t  retcode;

    retcode       = ERRCODE_NONE;
    if (token_id >= MAX_FLEXIDAG_TOKENS) {
        /* ERROR */
        wait_valid  = 0;
        retcode     = 0xFFFFFFFFU;
    } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
    else { /* if (token_id < MAX_FLEXIDAG_TOKENS) */
        if (flexidag_system_support.token_state[token_id] != FLEXIDAG_TOKEN_USED) {
            /* ERROR */
            wait_valid  = 0;
            retcode     = 0xFFFFFFFFU;
        } /* if (flexidag_system_support.token_state[token_id] != FLEXIDAG_TOKEN_USED) */
        else if (block == 0) {
            wait_valid = 0;
            flexidag_system_support.token_state[token_id] = FLEXIDAG_TOKEN_FINISHED;
        } /* if ((flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_USED) && (block != 0)) */
        else { /* if (block == 0) */
            flexidag_system_support.token_state[token_id] = FLEXIDAG_TOKEN_WAITING;
            wait_valid = 1;
        }
    } /* if (token_id < MAX_FLEXIDAG_TOKENS) */

    thread_mutex_unlock(&flexidag_system_support.token_mutex);

    if (wait_valid != 0) {
        sem_wait(&flexidag_system_support.token[token_id]);
        thread_mutex_lock(&flexidag_system_support.token_mutex);
        if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_FINISHED) {
            retcode = flexidag_system_support.token_retcode[token_id];
        } /* if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_FINISHED) */
        else { /* if (flexidag_system_support.token_state[token_id] != FLEXIDAG_TOKEN_FINISHED) */
            /* ERROR */
            retcode = 0xFFFFFFFFU;
        } /* if (flexidag_system_support.token_state[token_id] != FLEXIDAG_TOKEN_FINISHED) */
        flexidag_system_support.token_state[token_id] = FLEXIDAG_TOKEN_UNUSED;
        thread_mutex_unlock(&flexidag_system_support.token_mutex);
    } /* if (wait_valid != 0) */

    return retcode;

} /* flexidag_wait_token() */

uint32_t  flexidag_release_token(uint32_t token_id, uint32_t function_retcode, uint32_t function_start_time, uint32_t function_end_time)
{
    uint32_t  release_valid;
    uint32_t  retcode;

    retcode       = ERRCODE_NONE;
    if (token_id >= MAX_FLEXIDAG_TOKENS) {
        /* ERROR */
        release_valid = 0;
        retcode       = 0xFFFFFFFFU;
    } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
    else { /* if (token_id < MAX_FLEXIDAG_TOKENS) */
        thread_mutex_lock(&flexidag_system_support.token_mutex);
        if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_FINISHED) {
            release_valid = 0; /* Token wasn't blocking */
            flexidag_system_support.token_state[token_id] = FLEXIDAG_TOKEN_UNUSED;
        } /* if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_FINISHED) */
        else if (flexidag_system_support.token_state[token_id] != FLEXIDAG_TOKEN_WAITING) {
            /* ERROR */
            release_valid = 0;
            retcode       = 0xFFFFFFFFU;
        } /* if (flexidag_system_support.token_state[token_id] != FLEXIDAG_TOKEN_WAITING) */
        else { /* if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_WAITING) */
            release_valid = 1;
        } /* if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_WAITING) */
        thread_mutex_unlock(&flexidag_system_support.token_mutex);
    } /* if (token_id < MAX_FLEXIDAG_TOKENS) */

    if (release_valid != 0) {
        thread_mutex_lock(&flexidag_system_support.token_mutex);
        flexidag_system_support.token_state[token_id]       = FLEXIDAG_TOKEN_FINISHED;
        flexidag_system_support.token_retcode[token_id]     = function_retcode;
        flexidag_system_support.token_start_time[token_id]  = function_start_time;
        flexidag_system_support.token_end_time[token_id]    = function_end_time;
        thread_mutex_unlock(&flexidag_system_support.token_mutex);
        sem_post(&flexidag_system_support.token[token_id]);
    } /* if (release_valid != 0) */

    return retcode;

} /* flexidag_release_token() */


/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG system init (AMALGAM) =-------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t drv_flexidag_system_init(void)
{
    uint32_t   loop;

    thread_mutex_init(&flexidag_system_support.token_mutex);
    thread_mutex_lock(&flexidag_system_support.token_mutex);

    thread_mutex_init(&flexidag_system_support.slot_mutex);
    thread_mutex_lock(&flexidag_system_support.slot_mutex);

    memset(&flexidag_system_support, 0, sizeof(flexidag_system_support_t));

    flexidag_system_support.num_slots_enabled = 0;
    flexidag_system_support.next_token_index  = 0;
    for (loop = 0; loop < FLEXIDAG_MAX_SLOTS; loop++) {
        memset(&flexidag_slot_state[loop], 0, sizeof(flexidag_slot_state_t));
        flexidag_system_support.slot_state[loop]    = FLEXIDAG_SLOT_UNUSED;
        flexidag_slot_state[loop].flexidag_slot_id  = loop;
        thread_mutex_init(&flexidag_slot_state[loop].run_mutex);
    } /* for (loop = 0; loop < FLEXIDAG_MAX_SLOTS; loop++) */
    for (loop = 0; loop < MAX_FLEXIDAG_TOKENS; loop++) {
        flexidag_system_support.token_state[loop]   = FLEXIDAG_TOKEN_UNUSED;
        flexidag_system_support.token_retcode[loop] = ERRCODE_NONE;
        sem_init(&(flexidag_system_support.token[loop]), 0, 0);
    } /* for (loop = 0; loop < MAX_FLEXIDAG_TOKENS; loop++) */

    thread_mutex_unlock(&flexidag_system_support.slot_mutex);
    thread_mutex_unlock(&flexidag_system_support.token_mutex);

    return 0U;
} /* drv_flexidag_system_init() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG system enable (AMALGAM) =-----------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t drv_flexidag_enable(uint32_t num_slots)
{
    int32_t retval;

    thread_mutex_lock(&flexidag_system_support.slot_mutex);

    if (num_slots > FLEXIDAG_MAX_SLOTS) {
        flexidag_system_support.num_slots_enabled = 0;
        printf("[error] : drv_flexidag_enable() : Invalid number of slots (%d, max %d)\n", num_slots, FLEXIDAG_MAX_SLOTS);
        retval = -EINVAL;
    } /* if (num_slots > FLEXIDAG_MAX_SLOTS) */
    else { /* if (num_slots <= FLEXIDAG_MAX_SLOTS) */
        uint32_t  slot_loop;

        retval = 0;

        flexidag_system_support.num_slots_enabled = num_slots;
        for (slot_loop = 0; slot_loop < num_slots; slot_loop++) {
            flexidag_system_support.slot_state[slot_loop] = FLEXIDAG_SLOT_UNUSED;
        } /* for (slot_loop = 0; slot_loop < num_slots; slot_loop++) */
        for (slot_loop = num_slots; slot_loop < FLEXIDAG_MAX_SLOTS; slot_loop++) {
            flexidag_system_support.slot_state[slot_loop] = FLEXIDAG_SLOT_UNAVAIL;
        } /* for (slot_loop = num_slots; slot_loop < FLEXIDAG_MAX_SLOTS; slot_loop++) */
    } /* if (num_slots <= FLEXIDAG_MAX_SLOTS) */

    thread_mutex_unlock(&flexidag_system_support.slot_mutex);

    return retval;

} /* drv_flexidag_enable() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG API call : flexidag_create() (AMALGAM) =--------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/

uint32_t  drv_flexidag_create(void **vppHandle, uint32_t *pSlotId)
{
    int32_t   retval;

    if (vppHandle == NULL) {
        printf("[error] : drv_flexidag_create() : Invalid vppHandle (%p)\n", (void *)vppHandle);
        retval = -EINVAL;
    } /* if (vppHandle == NULL) */
    else if (pSlotId == NULL) {
        printf("[error] : drv_flexidag_create() : Invalid pSlotId (%p)\n", (void *)pSlotId);
        retval = -EINVAL;
    } /* if (pSlotId == NULL) */
    else { /* if ((vppHandle != NULL) && (pSlotId != NULL)) */
        flexidag_slot_state_t  *pFoundHandle;
        uint32_t  found_slot_id;
        uint32_t  token_id;

        pFoundHandle  = NULL;
        found_slot_id = FLEXIDAG_INVALID_SLOT_ID;
        token_id      = flexidag_find_token();
        if (token_id >= MAX_FLEXIDAG_TOKENS) {
            printf("[error] : flexidag_ioctl_handle_create() : Unable to find token in system (max=%d)\n", flexidag_system_support.num_slots_enabled);
            retval = -EINVAL;
        } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
        else { /* if (token_id < MAX_FLEXIDAG_TOKENS) */
            armvis_msg_t msgbase;

            msgbase.hdr.message_type    = (SCHEDMSG_FLEXIDAG_CREATE_REQUEST | 0x8000);
            msgbase.hdr.message_id      = 0;
            msgbase.hdr.message_retcode = ERRCODE_NONE;
            msgbase.msg.flexidag_create_request.flexidag_token = token_id;

            thread_mutex_lock(&flexidag_system_support.token_mutex);

            schdrmsg_send_msg(&msgbase, sizeof(msgbase));

            retval = flexidag_wait_token(token_id, 1);

            if (retval != 0) {
                printf("[error] : drv_flexidag_create() : Could not find available slot (%d enabled), rc=0x%08x\n",
                       flexidag_system_support.num_slots_enabled, (uint32_t)retval);
                retval = -EINVAL;
            } /* if (retval != 0) */
            else { /* if (retval == 0) */
                thread_mutex_lock(&flexidag_system_support.slot_mutex);

                found_slot_id = flexidag_system_support.token_start_time[token_id];
                pFoundHandle  = &flexidag_slot_state[found_slot_id];
                flexidag_system_support.slot_state[found_slot_id] = FLEXIDAG_SLOT_USED;

                /*-= Reset slot state to default states =-----------------------------------------*/
                memset(&pFoundHandle->flexidag_slot_id, 0x0, sizeof(flexidag_slot_state_t) - sizeof(pmutex_t));

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
                thread_mutex_unlock(&flexidag_system_support.slot_mutex);
            } /* if (retval != 0) : flexidag_wait_token() CREATE */
        } /* if (token_id < MAX_FLEXIDAG_TOKENS) */
        *vppHandle  = pFoundHandle;
        *pSlotId    = found_slot_id;
    } /* if ((vppHandle != NULL) && (pSlotId != NULL)) */

    return retval;

} /* drv_flexidag_create() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG API call : flexidag_set_parameter() (AMALGAM) =-------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t  drv_flexidag_set_parameter(void *vpHandle, uint32_t parameter_id, uint32_t parameter_value)
{
    int32_t  retval;

    if (vpHandle == NULL) {
        printf("[error] : drv_flexidag_set_parameter() : Invalid vpHandle (%p)\n", (void *)vpHandle);
        retval = -EINVAL;
    } /* if (vpHandle == NULL) */
    else { /* if (vpHandle != NULL) */
        flexidag_slot_state_t *pHandle;
        flexidag_slot_state_t *pHandle_expected;

        pHandle           = (flexidag_slot_state_t *)vpHandle;
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            printf("[error] : drv_flexidag_set_parameter() : Invalid vpHandle (%p), slot marked = %d, expected (%p) for that slot\n",
                   (void *)pHandle, pHandle->flexidag_slot_id, (void *)pHandle_expected);
            retval = -EINVAL;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            retval = 0;
            switch (parameter_id) {
            case FDPARAM_INTERFACE_MSGNUM_ID:
                pHandle->fdparam_interface_cvmsg_num = parameter_value;
                break;
            case FDPARAM_INTERNAL_MSGNUM_ID:
                pHandle->fdparam_internal_cvmsg_num = parameter_value;
                break;
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
                printf("[error] : drv_flexidag_set_parameter() : Invalid parameter_id (%d), value = %d\n", parameter_id, parameter_value);
                retval = -EINVAL;
                break;
            } /* switch (parameter_id) */
        } /* if (pHandle == pHandle_expected) */
    } /* if (vpHandle != NULL) */

    return retval;

} /* drv_flexidag_set_parameter() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG API call : drv_flexidag_add_tbar() (AMALGAM) =--------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t  drv_flexidag_add_tbar(void *vpHandle, flexidag_memblk_t *pblk_tbar)
{
    int32_t  retval;

    if (vpHandle == NULL) {
        printf("[error] : drv_flexidag_add_tbar() : Invalid vpHandle (%p)\n", (void *)vpHandle);
        retval = -EINVAL;
    } /* if (vpHandle == NULL) */
    else { /* if (vpHandle != NULL) */
        flexidag_slot_state_t *pHandle;
        flexidag_slot_state_t *pHandle_expected;

        pHandle           = (flexidag_slot_state_t *)vpHandle;
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            printf("[error] : drv_flexidag_add_tbar() : Invalid vpHandle (%p), slot marked = %d, expected (%p) for that slot\n",
                   (void *)pHandle, pHandle->flexidag_slot_id, (void *)pHandle_expected);
            retval = -EINVAL;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            retval = 0;
            if (pHandle->num_extra_tbar >= FLEXIDAG_MAX_TBAR) {
                printf("[error] : drv_flexidag_add_tbar() : Too many TBAR files attached already (%d, max %d)\n",
                       pHandle->num_extra_tbar, FLEXIDAG_MAX_TBAR);
                retval = -EINVAL;
            } /* if (pHandle->num_extra_tbar >= FLEXIDAG_MAX_TBAR) */
            else { /* if (pHandle->num_extra_tbar < FLEXIDAG_MAX_TBAR) */
                int32_t index;
                index = pHandle->num_extra_tbar;
                pHandle->vpExtraTbar[index]       = pblk_tbar->pBuffer;
                pHandle->extra_tbar_daddr[index]  = pblk_tbar->buffer_daddr;
                pHandle->num_extra_tbar++;
            } /* if (pHandle->num_extra_tbar < FLEXIDAG_MAX_TBAR) */
        } /* if (pHandle == pHandle_expected) */
    } /* if (vpHandle != NULL) */

    return retval;

} /* drv_flexidag_add_tbar() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG API call : drv_flexidag_add_sfb() (AMALGAM) =---------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t  drv_flexidag_add_sfb(void *vpHandle, flexidag_memblk_t *pblk_sfb)
{
    int32_t  retval;

    if (vpHandle == NULL) {
        printf("[error] : drv_flexidag_add_sfb() : Invalid vpHandle (%p)\n", (void *)vpHandle);
        retval = -EINVAL;
    } /* if (vpHandle == NULL) */
    else { /* if (vpHandle != NULL) */
        flexidag_slot_state_t *pHandle;
        flexidag_slot_state_t *pHandle_expected;

        pHandle           = (flexidag_slot_state_t *)vpHandle;
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            printf("[error] : drv_flexidag_add_sfb() : Invalid vpHandle (%p), slot marked = %d, expected (%p) for that slot\n",
                   (void *)pHandle, pHandle->flexidag_slot_id, (void *)pHandle_expected);
            retval = -EINVAL;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            retval = 0;
            if (pHandle->num_extra_sfb >= FLEXIDAG_MAX_SFB) {
                printf("[error] : drv_flexidag_add_sfb() : Too many SFB files attached already (%d, max %d)\n",
                       pHandle->num_extra_sfb, FLEXIDAG_MAX_SFB);
                retval = -EINVAL;
            } /* if (pHandle->num_extra_sfb >= FLEXIDAG_MAX_SFB) */
            else { /* if (pHandle->num_extra_sfb < FLEXIDAG_MAX_SFB) */
                int32_t index;
                index = pHandle->num_extra_sfb;
                pHandle->vpExtraSfb[index]          = pblk_sfb->pBuffer;
                pHandle->extra_sfb_daddr[index]     = pblk_sfb->buffer_daddr;
                {
                    uint16_t  scan_loop, num_lines;
                    uint32_t  numlines_size;
                    uint8_t  *pRecast = NULL;

                    scan_loop = 0U;
                    numlines_size = (uint32_t) sizeof(sysflow_entry_t);
                    num_lines = (uint16_t)(pblk_sfb->buffer_size / numlines_size);
                    pRecast   = (uint8_t *)pblk_sfb->pBuffer;
                    if(pRecast != NULL) {
                        while (scan_loop < num_lines) {
                            if (pRecast[(scan_loop * numlines_size)] == 0x00U) {
                                num_lines = scan_loop;
                                break;
                            } else {
                                scan_loop++;
                            }
                        }
                        pHandle->extra_sfb_numlines[index]  = num_lines;
                        printf("[info] : drv_flexidag_add_sfb() : sfb size %d, num_lines %d \n", pblk_sfb->buffer_size, num_lines, 0, 0, 0);
                    }
                }
                pHandle->num_extra_sfb++;
            } /* if (pHandle->num_extra_sfb < FLEXIDAG_MAX_SFB) */
        } /* if (pHandle == pHandle_expected) */
    } /* if (vpHandle != NULL) */

    return retval;

} /* drv_flexidag_add_sfb() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG API call : flexidag_set_open() (AMALGAM) =------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t  drv_flexidag_open(void *vpHandle, flexidag_memblk_t *pFlexiBlk, flexidag_memory_requirements_t *pFlexiRequirements)
{
    int32_t retval;

    if (vpHandle == NULL) {
        printf("[error] : drv_flexidag_open() : Invalid vpHandle (%p)\n", (void *)vpHandle);
        retval = -EINVAL;
    } /* if (vpHandle == NULL) */
    else if (pFlexiBlk == NULL) {
        printf("[error] : drv_flexidag_open() : Invalid pFlexiBlk (%p)\n", (void *)pFlexiBlk);
        retval = -EINVAL;
    } /* if (pFlexiBlk == NULL) */
    else if (pFlexiRequirements == NULL) {
        printf("[error] : drv_flexidag_open() : Invalid vpFlexiRequirements (%p)\n", pFlexiRequirements);
        retval = -EINVAL;
    } /* if (pFlexiRequirements == NULL) */
    else { /* if ((vpHandle != NULL) && (vpFlexiBlk != NULL) && (pFlexiRequirements != NULL)) */
        flexidag_slot_state_t *pHandle;
        flexidag_slot_state_t *pHandle_expected;

        pHandle           = (flexidag_slot_state_t *)vpHandle;
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            printf("[error] : drv_flexidag_open() : Invalid vpHandle (%p), slot marked = %d, expected (%p) for that slot\n",
                   (void *)pHandle, pHandle->flexidag_slot_id, (void *)pHandle_expected);
            retval = -EINVAL;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            void *vpFlexiBin;
            uint32_t  flexibin_daddr;
            uint32_t  flexibin_size;

            vpFlexiBin              = pFlexiBlk->pBuffer;
            flexibin_daddr          = pFlexiBlk->buffer_daddr;
            flexibin_size           = pFlexiBlk->buffer_size;
            if ((vpFlexiBin == NULL) || (flexibin_daddr == 0) || (flexibin_size == 0)) {
                printf("[ERROR] : flexidag_ioctl_handle_open() : Invalid vpFlexiBin (%p), flexibin_daddr (0x%08x), or flexibin_size (%d)\n",
                       vpFlexiBin, flexibin_daddr, flexibin_size);
                retval = -EINVAL;
            } /* if ((vpFlexiBin == NULL) || (flexibin_daddr == 0) || (flexibin_size == 0)) */
            else { /* if ((vpFlexiBin != NULL) && (flexibin_daddr != 0) && (flexibin_size != 0)) */
                uint32_t  token_id;

                pHandle->vpFlexiBin     = vpFlexiBin;
                pHandle->flexibin_daddr = flexibin_daddr;
                pHandle->flexibin_size  = flexibin_size;

                token_id = flexidag_find_token();

                if (token_id >= MAX_FLEXIDAG_TOKENS) {
                    printf("[error] : drv_flexidag_open(%p) : Unable to find token in system (max=%d)\n", (void *)vpHandle, MAX_FLEXIDAG_TOKENS);
                    retval = -EINVAL;
                } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
                else { /* if (token_id < MAX_FLEXIDAG_TOKENS) */
                    armvis_msg_t msgbase;
                    schedmsg_flexidag_open_request_t *pMsg;
                    uint32_t   core_loop;

                    msgbase.hdr.message_type    = (SCHEDMSG_FLEXIDAG_OPEN_REQUEST | 0x8000);
                    msgbase.hdr.message_id      = 0x0100 + pHandle->flexidag_slot_id;
                    msgbase.hdr.message_retcode = ERRCODE_NONE;

                    pMsg                        = &msgbase.msg.flexidag_open_request;
                    pMsg->flexidag_token        = token_id;
                    pMsg->flexidag_binary_daddr = flexibin_daddr;
                    pMsg->arm_cvmsg_num         = pHandle->fdparam_interface_cvmsg_num;
                    pMsg->flexidag_cvmsg_num    = pHandle->fdparam_internal_cvmsg_num;
                    for (core_loop = 0; core_loop < SYSTEM_MAX_NUM_VISORC; core_loop++) {
                        pMsg->visorc_perf_log_entry_num[core_loop]    = pHandle->fdparam_log_size_vis_perf[core_loop];
                        pMsg->visorc_sched_log_entry_num[core_loop]   = pHandle->fdparam_log_size_vis_sched[core_loop];
                        pMsg->visorc_cvtask_log_entry_num[core_loop]  = pHandle->fdparam_log_size_vis_cvtask[core_loop];
                    } /* for (core_loop = 0; core_loop < SYSTEM_MAX_NUM_VISORC; core_loop++) */
                    for (core_loop = 0; core_loop < SYSTEM_MAX_NUM_CORTEX; core_loop++) {
                        pMsg->arm_perf_log_entry_num[core_loop]       = pHandle->fdparam_log_size_arm_perf[core_loop];
                        pMsg->arm_sched_log_entry_num[core_loop]      = pHandle->fdparam_log_size_arm_sched[core_loop];
                        pMsg->arm_cvtask_log_entry_num[core_loop]     = pHandle->fdparam_log_size_arm_cvtask[core_loop];
                    } /* for (core_loop = 0; core_loop < SYSTEM_MAX_NUM_CORTEX; core_loop++) */
                    for (core_loop = 0; core_loop < FLEXIDAG_MAX_TBAR; core_loop++) {
                        pMsg->additional_tbar_daddr[core_loop]        = pHandle->extra_tbar_daddr[core_loop];
                    } /* for (core_loop = 0; core_loop < FLEXIDAG_MAX_TBAR; core_loop++) */
                    for (core_loop = 0; core_loop < FLEXIDAG_MAX_SFB; core_loop++) {
                        pMsg->additional_sfb_daddr[core_loop]         = pHandle->extra_sfb_daddr[core_loop];
                        pMsg->additional_sfb_numlines[core_loop]      = pHandle->extra_sfb_numlines[core_loop];
                    } /* for (core_loop = 0; core_loop < FLEXIDAG_MAX_SFB; core_loop++) */
                    thread_mutex_lock(&flexidag_system_support.token_mutex);

                    schdrmsg_send_msg(&msgbase, sizeof(msgbase));

                    retval = flexidag_wait_token(token_id, 1);

                    memcpy(pFlexiRequirements, &pHandle->memreq, sizeof(flexidag_memory_requirements_t));
                } /* if ((vpFlexiBin != NULL) && (flexibin_daddr != 0) && (flexibin_size != 0)) */
            } /* if (token_id < MAX_FLEXIDAG_TOKENS) */
        } /* if (pHandle == pHandle_expected) */
    } /* if ((vpHandle != NULL) && (vpFlexiBlk != NULL) && (pFlexiRequirements != NULL)) */

    return retval;

} /* drv_flexidag_open() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG API call : flexidag_set_state_buffer() (AMALGAM) =----------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t  drv_flexidag_set_state_buffer(void *vpHandle, flexidag_memblk_t *pFlexiBlk)
{
    int32_t retval;

    if (pFlexiBlk == NULL) {
        printf("[error] : drv_flexidag_set_state_buffer() : Invalid pFlexiBlk (%p)\n", (void *)pFlexiBlk);
        retval = -EINVAL;
    } /* if (pFlexiBlk == NULL) */
    else { /* if (pFlexiBlk != NULL) */
        void *vpBuffer;
        uint32_t buffer_daddr;

        vpBuffer      = (void *)pFlexiBlk->pBuffer;
        buffer_daddr  = pFlexiBlk->buffer_daddr;

        if (vpHandle == NULL) {
            printf("[error] : drv_flexidag_set_state_buffer() : Invalid vpHandle (%p)\n", (void *)vpHandle);
            retval = -EINVAL;
        } /* if (vpHandle == NULL) */
        else { /* if (vpHandle != NULL) */
            flexidag_slot_state_t *pHandle;
            flexidag_slot_state_t *pHandle_expected;

            pHandle           = (flexidag_slot_state_t *)vpHandle;
            pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
            if (pHandle != pHandle_expected) {
                printf("[error] : drv_flexidag_set_state_buffer() : Invalid vpHandle (%p), slot marked = %d, expected (%p) for that slot\n",
                       (void *)pHandle, pHandle->flexidag_slot_id, (void *)pHandle_expected);
                retval = -EINVAL;
            } /* if (pHandle != pHandle_expected) */
            else { /* if (pHandle == pHandle_expected) */
                retval = 0;
                pHandle->vpStateBuffer      = vpBuffer;
                pHandle->state_buffer_daddr = buffer_daddr;
            } /* if (pHandle == pHandle_expected) */
        } /* if (vpHandle != NULL) */
    } /* if (pFlexiBlk != NULL) */
    return retval;
} /* flexidag_set_state_buffer() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG API call : flexidag_set_temp_buffer() (AMALGAM) =-----------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t  drv_flexidag_set_temp_buffer(void *vpHandle, flexidag_memblk_t *pFlexiBlk)
{
    int32_t retval;

    if (pFlexiBlk == NULL) {
        printf("[error] : drv_flexidag_set_temp_buffer() : Invalid pFlexiBlk (%p)\n", (void *)pFlexiBlk);
        retval = -EINVAL;
    } /* if (pFlexiBlk == NULL) */
    else { /* if (pFlexiBlk != NULL) */
        void *vpBuffer;
        uint32_t buffer_daddr;

        vpBuffer      = (void *)pFlexiBlk->pBuffer;
        buffer_daddr  = pFlexiBlk->buffer_daddr;

        if (vpHandle == NULL) {
            printf("[error] : flexidag_set_temp_buffer() : Invalid vpHandle (%p)\n", (void *)vpHandle);
            retval = -EINVAL;
        } /* if (vpHandle == NULL) */
        else { /* if (vpHandle != NULL) */
            flexidag_slot_state_t *pHandle;
            flexidag_slot_state_t *pHandle_expected;

            pHandle           = (flexidag_slot_state_t *)vpHandle;
            pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
            if (pHandle != pHandle_expected) {
                printf("[error] : flexidag_set_temp_buffer() : Invalid vpHandle (%p), slot marked = %d, expected (%p) for that slot\n",
                       (void *)pHandle, pHandle->flexidag_slot_id, (void *)pHandle_expected);
                retval = -EINVAL;
            } /* if (pHandle != pHandle_expected) */
            else { /* if (pHandle == pHandle_expected) */
                retval = 0;
                pHandle->vpTempBuffer       = vpBuffer;
                pHandle->temp_buffer_daddr  = buffer_daddr;
            } /* if (pHandle == pHandle_expected) */
        } /* if (vpHandle != NULL) */
    } /* if (pFlexiBlk != NULL) */
    return retval;
} /* flexidag_set_temp_buffer() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG API call : flexidag_init() (AMALGAM) =----------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t  drv_flexidag_init(void *vpHandle)
{
    int32_t retval;

    if (vpHandle == NULL) {
        printf("[error] : drv_flexidag_init() : Invalid vpHandle (%p)\n", (void *)vpHandle);
        retval = -EINVAL;
    } /* if (vpHandle == NULL) */
    else { /* if (vpHandle != NULL) */
        flexidag_slot_state_t *pHandle;
        flexidag_slot_state_t *pHandle_expected;

        pHandle           = (flexidag_slot_state_t *)vpHandle;
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            printf("[error] : drv_flexidag_init() : Invalid vpHandle (%p), slot marked = %d, expected (%p) for that slot\n",
                   (void *)pHandle, pHandle->flexidag_slot_id, (void *)pHandle_expected);
            retval = -EINVAL;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            if ((pHandle->vpStateBuffer == NULL) || (pHandle->state_buffer_daddr == 0)) {
                printf("[error] : drv_flexidag_init() : FD[%2d] : Slot has invalid vpStateBuffer (%p) / state_buffer_daddr = 0x%08x\n",
                       pHandle->flexidag_slot_id, pHandle->vpStateBuffer, pHandle->state_buffer_daddr);
                retval = -EINVAL;
            } /* if ((pHandle->vpStateBuffer == NULL) || (pHandle->state_buffer_daddr == 0)) */
            else { /* if ((pHandle->vpStateBuffer != NULL) && (pHandle->state_buffer_daddr != 0)) */
                uint32_t  token_id;

                token_id = flexidag_find_token();

                if (token_id >= MAX_FLEXIDAG_TOKENS) {
                    printf("[error] : drv_flexidag_open(%p) : Unable to find token in system (max=%d)\n", (void *)vpHandle, MAX_FLEXIDAG_TOKENS);
                    retval = -EINVAL;
                } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
                else { /* if (token_id < MAX_FLEXIDAG_TOKENS) */
                    armvis_msg_t msgbase;
                    schedmsg_flexidag_init_request_t *pMsg;
                    uint32_t   output_loop;

                    msgbase.hdr.message_type    = (SCHEDMSG_FLEXIDAG_INIT_REQUEST | 0x8000);
                    msgbase.hdr.message_id      = 0x0100 + pHandle->flexidag_slot_id;
                    msgbase.hdr.message_retcode = ERRCODE_NONE;

                    pMsg                              = &msgbase.msg.flexidag_init_request;
                    pMsg->flexidag_token              = token_id;
                    pMsg->flexidag_state_buffer_daddr = pHandle->state_buffer_daddr;
                    pMsg->flexidag_temp_buffer_daddr  = pHandle->temp_buffer_daddr;
                    for (output_loop = 0; output_loop < FLEXIDAG_MAX_OUTPUTS; output_loop++) {
                        pMsg->flexidag_callback_id[output_loop] = 0xFFFFFFFFU;  /* TODO: Link callback id /tokens */
                    } /* for (output_loop = 0; output_loop < FLEXIDAG_MAX_OUTPUTS; output_loop++) */

                    thread_mutex_lock(&flexidag_system_support.token_mutex);

                    schdrmsg_send_msg(&msgbase, sizeof(msgbase));

                    retval = flexidag_wait_token(token_id, 1);

                } /* if (token_id < MAX_FLEXIDAG_TOKENS) */
            } /* if ((pHandle->vpStateBuffer != NULL) && (pHandle->state_buffer_daddr != 0)) */
        } /* if (pHandle == pHandle_expected) */
    } /* if (vpHandle != NULL) */

    return retval;

} /* drv_flexidag_init() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG API call : flexidag_prep_run() (AMALGAM) =------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t drv_flexidag_prep_run(void *vpHandle, uint32_t *pTokenId)
{
    int32_t retval;

    if (vpHandle == NULL) {
        printf("[error] : drv_flexidag_prep_run() : Invalid vpHandle (%p)\n", (void *)vpHandle);
        retval = -EINVAL;
    } /* if (vpHandle == NULL) */
    else { /* if (vpHandle != NULL) */
        flexidag_slot_state_t *pHandle;
        flexidag_slot_state_t *pHandle_expected;

        pHandle           = (flexidag_slot_state_t *)vpHandle;
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            printf("[error] : drv_flexidag_prep_run() : Invalid vpHandle (%p), slot marked = %d, expected (%p) for that slot\n",
                   (void *)pHandle, pHandle->flexidag_slot_id, (void *)pHandle_expected);
            retval = -EINVAL;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            pHandle->pending_token_id = flexidag_find_token();
            if (pHandle->pending_token_id < MAX_FLEXIDAG_TOKENS) {
                retval = 0;
                thread_mutex_lock(&pHandle->run_mutex);
            } /* if (pHandle->pending_token_id < MAX_FLEXIDAG_TOKENS) */
            else { /* if (pHandle->pending_token_id >= MAX_FLEXIDAG_TOKENS) */
                retval = -EFAULT;
            } /* if (pHandle->pending_token_id >= MAX_FLEXIDAG_TOKENS) */
            if (pTokenId != NULL) {
                *pTokenId = pHandle->pending_token_id;
            } /* if (pTokenId != NULL) */
        } /* if (pHandle == pHandle_expected) */
    } /* if (vpHandle != NULL) */

    return retval;

} /* drv_flexidag_prep_run() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG API call : flexidag_set_input_buffer() (AMALGAM) =----------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t  drv_flexidag_set_input_buffer(void *vpHandle, uint32_t input_num, flexidag_memblk_t *pFlexiBlk)
{
    int32_t retval;

    if (pFlexiBlk == NULL) {
        printf("[error] : drv_flexidag_set_input_buffer() : Invalid pFlexiBlk (%p)\n", (void *)pFlexiBlk);
        retval = -EINVAL;
    } /* if (pFlexiBlk == NULL) */
    else { /* if (pFlexiBlk != NULL) */
        void *vpBuffer;
        uint32_t buffer_daddr;

        vpBuffer      = (void *)pFlexiBlk->pBuffer;
        buffer_daddr  = pFlexiBlk->buffer_daddr;

        if (vpHandle == NULL) {
            printf("[error] : drv_flexidag_set_input_buffer() : Invalid vpHandle (%p)\n", (void *)vpHandle);
            retval = -EINVAL;
        } /* if (vpHandle == NULL) */
        else if (vpBuffer == NULL) {
            printf("[error] : drv_flexidag_set_input_buffer() : Invalid vpBuffer (%p)\n", (void *)vpBuffer);
            retval = -EINVAL;
        } /* if (vpBuffer == NULL) */
        else if (input_num >= FLEXIDAG_MAX_INPUTS) {
            printf("[error] : drv_flexidag_set_input_buffer() : Invalid input_num (%d; max %d)\n", input_num, FLEXIDAG_MAX_INPUTS);
            retval = -EINVAL;
        } /* if (input_num >= FLEXIDAG_MAX_INPUTS) */
        else { /* if ((vpHandle != NULL) && (vpBuffer != NULL) && (input_num < FLEXIDAG_MAX_INPUTS)) */
            flexidag_slot_state_t *pHandle;
            flexidag_slot_state_t *pHandle_expected;

            pHandle           = (flexidag_slot_state_t *)vpHandle;
            pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
            if (pHandle != pHandle_expected) {
                printf("[error] : drv_flexidag_set_input_buffer() : Invalid vpHandle (%p), slot marked = %d, expected (%p) for that slot\n",
                       (void *)pHandle, pHandle->flexidag_slot_id, (void *)pHandle_expected);
                retval = -EINVAL;
            } /* if (pHandle != pHandle_expected) */
            else { /* if (pHandle == pHandle_expected) */
                retval = 0;
                pHandle->vpInputBuffer[input_num]       = vpBuffer;
                pHandle->input_buffer_daddr[input_num]  = buffer_daddr;
                pHandle->input_buffer_size[input_num]   = pFlexiBlk->buffer_size;
            } /* if (pHandle == pHandle_expected) */
        } /* if ((vpHandle != NULL) && (vpBuffer != NULL) && (input_num < FLEXIDAG_MAX_INPUTS)) */
    } /* if (pFlexiBlk != NULL) */

    return retval;

} /* drv_flexidag_set_input_buffer() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG API call : flexidag_set_feedback_buffer() (AMALGAM) =-------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t  drv_flexidag_set_feedback_buffer(void *vpHandle, uint32_t feedback_num, flexidag_memblk_t *pFlexiBlk)
{
    int32_t retval;

    if (pFlexiBlk == NULL) {
        printf("[error] : drv_flexidag_set_feedback_buffer() : Invalid pFlexiBlk (%p)\n", (void *)pFlexiBlk);
        retval = -EINVAL;
    } /* if (pFlexiBlk == NULL) */
    else { /* if (pFlexiBlk != NULL) */
        void *vpBuffer;
        uint32_t buffer_daddr;

        vpBuffer      = (void *)pFlexiBlk->pBuffer;
        buffer_daddr  = pFlexiBlk->buffer_daddr;

        if (vpHandle == NULL) {
            printf("[error] : drv_flexidag_set_feedback_buffer() : Invalid vpHandle (%p)\n", (void *)vpHandle);
            retval = -EINVAL;
        } /* if (vpHandle == NULL) */
        else if (vpBuffer == NULL) {
            printf("[error] : drv_flexidag_set_feedback_buffer() : Invalid vpBuffer (%p)\n", (void *)vpBuffer);
            retval = -EINVAL;
        } /* if (vpBuffer == NULL) */
        else if (feedback_num >= FLEXIDAG_MAX_FEEDBACKS) {
            printf("[error] : drv_flexidag_set_feedback_buffer() : Invalid feedback_num (%d; max %d)\n", feedback_num, FLEXIDAG_MAX_FEEDBACKS);
            retval = -EINVAL;
        } /* if (feedback_num >= FLEXIDAG_MAX_INPUTS) */
        else { /* if ((vpHandle != NULL) && (vpBuffer != NULL) && (feedback_num < FLEXIDAG_MAX_INPUTS)) */
            flexidag_slot_state_t *pHandle;
            flexidag_slot_state_t *pHandle_expected;

            pHandle           = (flexidag_slot_state_t *)vpHandle;
            pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
            if (pHandle != pHandle_expected) {
                printf("[error] : drv_flexidag_set_feedback_buffer() : Invalid vpHandle (%p), slot marked = %d, expected (%p) for that slot\n",
                       (void *)pHandle, pHandle->flexidag_slot_id, (void *)pHandle_expected);
                retval = -EINVAL;
            } /* if (pHandle != pHandle_expected) */
            else { /* if (pHandle == pHandle_expected) */
                retval = 0;
                pHandle->vpFeedbackBuffer[feedback_num]       = vpBuffer;
                pHandle->feedback_buffer_daddr[feedback_num]  = buffer_daddr;
                pHandle->feedback_buffer_size[feedback_num]   = pFlexiBlk->buffer_size;
            } /* if (pHandle == pHandle_expected) */
        } /* if ((vpHandle != NULL) && (vpBuffer != NULL) && (feedback_num < FLEXIDAG_MAX_FEEDBACKS)) */
    } /* if (pFlexiBlk != NULL) */
    return retval;

} /* drv_flexidag_set_feedback_buffer() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG API call : flexidag_set_output_buffer() (AMALGAM) =---------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t  drv_flexidag_set_output_buffer(void *vpHandle, uint32_t output_num, flexidag_memblk_t *pFlexiBlk)
{
    int32_t retval = 0;

    if (pFlexiBlk == NULL) {
        printf("[error] : drv_flexidag_set_output_buffer() : Invalid pFlexiBlk (%p)\n", (void *)pFlexiBlk);
        retval = -EINVAL;
    } /* if (pFlexiBlk == NULL) */
    else { /* if (pFlexiBlk != NULL) */
        void *vpBuffer;
        uint32_t buffer_daddr;

        vpBuffer      = (void *)pFlexiBlk->pBuffer;
        buffer_daddr  = pFlexiBlk->buffer_daddr;

        if (vpHandle == NULL) {
            printf("[error] : drv_flexidag_set_output_buffer() : Invalid vpHandle (%p)\n", (void *)vpHandle);
            retval = -EINVAL;
        } /* if (vpHandle == NULL) */
        else if (vpBuffer == NULL) {
            printf("[error] : drv_flexidag_set_output_buffer() : Invalid vpBuffer (%p)\n", (void *)vpBuffer);
            retval = -EINVAL;
        } /* if (vpBuffer == NULL) */
        else if (output_num >= FLEXIDAG_MAX_OUTPUTS) {
            printf("[error] : drv_flexidag_set_output_buffer() : Invalid output_num (%d; max %d)\n", output_num, FLEXIDAG_MAX_OUTPUTS);
            retval = -EINVAL;
        } /* if (output_num >= FLEXIDAG_MAX_INPUTS) */
        else { /* if ((vpHandle != NULL) && (vpBuffer != NULL) && (output_num < FLEXIDAG_MAX_OUTPUTS)) */
            flexidag_slot_state_t *pHandle;
            flexidag_slot_state_t *pHandle_expected;

            pHandle           = (flexidag_slot_state_t *)vpHandle;
            pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
            if (pHandle != pHandle_expected) {
                printf("[error] : drv_flexidag_set_output_buffer() : Invalid vpHandle (%p), slot marked = %d, expected (%p) for that slot\n",
                       (void *)pHandle, pHandle->flexidag_slot_id, (void *)pHandle_expected);
                retval = -EINVAL;
            } /* if (pHandle != pHandle_expected) */
            else { /* if (pHandle == pHandle_expected) */
                pHandle->vpOutputBuffer[output_num]       = vpBuffer;
                pHandle->output_buffer_daddr[output_num]  = buffer_daddr;
                pHandle->output_buffer_size[output_num]   = pFlexiBlk->buffer_size;
                pHandle->pNextOutputMemblk[output_num]    = (flexidag_memblk_t *)pFlexiBlk;
            } /* if (pHandle == pHandle_expected) */
        } /* if ((vpHandle != NULL) && (vpBuffer != NULL) && (output_num < FLEXIDAG_MAX_OUTPUTS)) */
    } /* if (pFlexiBlk != NULL) */
    return retval;
} /* drv_flexidag_set_output_buffer() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG API call : flexidag_run() (AMALGAM) =-----------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t  drv_flexidag_run(void *vpHandle, flexidag_runinfo_t *pRunInfo)
{
    int32_t retval;

    if (vpHandle == NULL) {
        printf("[error] : drv_flexidag_run() : Invalid vpHandle (%p)\n", (void *)vpHandle);
        retval = -EINVAL;
    } /* if (vpHandle == NULL) */
    else { /* if (vpHandle != NULL) */
        flexidag_slot_state_t *pHandle;
        flexidag_slot_state_t *pHandle_expected;

        pHandle           = (flexidag_slot_state_t *)vpHandle;
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            printf("[error] : drv_flexidag_run() : Invalid vpHandle (%p), slot marked = %d, expected (%p) for that slot\n",
                   (void *)pHandle, pHandle->flexidag_slot_id, (void *)pHandle_expected);
            retval = -EINVAL;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            if ((pHandle->vpStateBuffer == NULL) || (pHandle->state_buffer_daddr == 0)) {
                printf("[error] : drv_flexidag_run() : FD[%2d] : Slot has invalid vpStateBuffer (%p) / state_buffer_daddr = 0x%08x\n",
                       pHandle->flexidag_slot_id, pHandle->vpStateBuffer, pHandle->state_buffer_daddr);
                retval = -EINVAL;
            } /* if ((pHandle->vpStateBuffer == NULL) || (pHandle->state_buffer_daddr == 0)) */
            else { /* if ((pHandle->vpStateBuffer != NULL) && (pHandle->state_buffer_daddr != 0)) */
                uint32_t  token_id;

                token_id = pHandle->pending_token_id;

                if (token_id >= MAX_FLEXIDAG_TOKENS) {
                    printf("[error] : drv_flexidag_run(%p) : Unable to find token in system (max=%d)\n", (void *)vpHandle, MAX_FLEXIDAG_TOKENS);
                    retval = -EINVAL;
                } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
                else { /* if (token_id < MAX_FLEXIDAG_TOKENS) */
                    armvis_msg_t msgbase;
                    schedmsg_flexidag_run_request_t *pMsg;
                    uint32_t   io_loop;

                    msgbase.hdr.message_type    = (SCHEDMSG_FLEXIDAG_RUN_REQUEST | 0x8000);
                    msgbase.hdr.message_id      = 0x0100 + pHandle->flexidag_slot_id;
                    msgbase.hdr.message_retcode = ERRCODE_NONE;

                    pMsg                              = &msgbase.msg.flexidag_run_request;
                    pMsg->flexidag_token              = token_id;
                    pMsg->flexidag_state_buffer_daddr = pHandle->state_buffer_daddr;
                    pMsg->flexidag_temp_buffer_daddr  = pHandle->temp_buffer_daddr;

                    for (io_loop = 0; io_loop < FLEXIDAG_MAX_INPUTS; io_loop++) {
                        pMsg->flexidag_input_buffer_daddr[io_loop]    = pHandle->input_buffer_daddr[io_loop];
                        pMsg->flexidag_input_buffer_size[io_loop]     = pHandle->input_buffer_size[io_loop];
                    } /* for (io_loop = 0; io_loop < FLEXIDAG_MAX_INPUTS; io_loop++) */
                    for (io_loop = 0; io_loop < FLEXIDAG_MAX_FEEDBACKS; io_loop++) {
                        pMsg->flexidag_feedback_buffer_daddr[io_loop] = pHandle->feedback_buffer_daddr[io_loop];
                        pMsg->flexidag_feedback_buffer_size[io_loop]  = pHandle->feedback_buffer_size[io_loop];
                    } /* for (io_loop = 0; io_loop < FLEXIDAG_MAX_FEEDBACKS; io_loop++) */
                    for (io_loop = 0; io_loop < FLEXIDAG_MAX_OUTPUTS; io_loop++) {
                        pMsg->flexidag_output_buffer_daddr[io_loop]   = pHandle->output_buffer_daddr[io_loop];
                        pMsg->flexidag_output_buffer_size[io_loop]    = pHandle->output_buffer_size[io_loop];
                        flexidag_system_support.pOutputMemBlk[token_id][io_loop] = pHandle->pNextOutputMemblk[io_loop];
                    } /* for (io_loop = 0; io_loop < FLEXIDAG_MAX_OUTPUTS; io_loop++) */

                    thread_mutex_lock(&flexidag_system_support.token_mutex);

                    schdrmsg_send_msg(&msgbase, sizeof(msgbase));

                    retval = flexidag_wait_token(token_id, 1);

                    thread_mutex_unlock(&pHandle->run_mutex);
                    pHandle->pending_token_id = 0xFFFFFFFFU;

                    if (pRunInfo != NULL) {
                        uint32_t  loop;
                        memset(pRunInfo, 0, sizeof(*pRunInfo));
                        pRunInfo->version           = 1;
                        pRunInfo->size              = sizeof(*pRunInfo);
                        pRunInfo->overall_retcode   = flexidag_system_support.token_retcode[token_id];
                        pRunInfo->start_time        = flexidag_system_support.token_start_time[token_id];
                        pRunInfo->end_time          = flexidag_system_support.token_end_time[token_id];
                        for (loop = 0; loop < FLEXIDAG_MAX_OUTPUTS; loop++) {
                            pRunInfo->output_donetime[loop] = flexidag_system_support.token_output_donetime[token_id][loop];
                        }
                    }
                } /* if (token_id < MAX_FLEXIDAG_TOKENS) */
            } /* if ((pHandle->vpStateBuffer != NULL) && (pHandle->state_buffer_daddr != 0)) */
        } /* if (pHandle == pHandle_expected) */
    } /* if (vpHandle != NULL) */

    return retval;

} /* drv_flexidag_run() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG API call : flexidag_run_noblock() (AMALGAM) =---------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t  drv_flexidag_run_noblock(void *vpHandle, uint32_t *pTokenId)
{
    int32_t retval;

    if (vpHandle == NULL) {
        printf("[error] : drv_flexidag_run_noblock() : Invalid vpHandle (%p)\n", (void *)vpHandle);
        retval = -EINVAL;
    } /* if (vpHandle == NULL) */
    else { /* if (vpHandle != NULL) */
        flexidag_slot_state_t *pHandle;
        flexidag_slot_state_t *pHandle_expected;

        pHandle           = (flexidag_slot_state_t *)vpHandle;
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            printf("[error] : drv_flexidag_run_noblock() : Invalid vpHandle (%p), slot marked = %d, expected (%p) for that slot\n",
                   (void *)pHandle, pHandle->flexidag_slot_id, (void *)pHandle_expected);
            retval = -EINVAL;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            if ((pHandle->vpStateBuffer == NULL) || (pHandle->state_buffer_daddr == 0)) {
                printf("[error] : drv_flexidag_run_noblock() : FD[%2d] : Slot has invalid vpStateBuffer (%p) / state_buffer_daddr = 0x%08x\n",
                       pHandle->flexidag_slot_id, pHandle->vpStateBuffer, pHandle->state_buffer_daddr);
                retval = -EINVAL;
            } /* if ((pHandle->vpStateBuffer == NULL) || (pHandle->state_buffer_daddr == 0)) */
            else { /* if ((pHandle->vpStateBuffer != NULL) && (pHandle->state_buffer_daddr != 0)) */
                uint32_t  token_id;

                token_id = pHandle->pending_token_id;

                if (token_id >= MAX_FLEXIDAG_TOKENS) {
                    printf("[error] : drv_flexidag_run_noblock(%p) : Unable to find token in system (max=%d)\n", (void *)vpHandle, MAX_FLEXIDAG_TOKENS);
                    retval = -EINVAL;
                } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
                else { /* if (token_id < MAX_FLEXIDAG_TOKENS) */
                    armvis_msg_t msgbase;
                    schedmsg_flexidag_run_request_t *pMsg;
                    uint32_t   io_loop;

                    msgbase.hdr.message_type    = (SCHEDMSG_FLEXIDAG_RUN_REQUEST | 0x8000);
                    msgbase.hdr.message_id      = 0x0100 + pHandle->flexidag_slot_id;
                    msgbase.hdr.message_retcode = ERRCODE_NONE;

                    pMsg                              = &msgbase.msg.flexidag_run_request;
                    pMsg->flexidag_token              = token_id;
                    pMsg->flexidag_state_buffer_daddr = pHandle->state_buffer_daddr;
                    pMsg->flexidag_temp_buffer_daddr  = pHandle->temp_buffer_daddr;

                    for (io_loop = 0; io_loop < FLEXIDAG_MAX_INPUTS; io_loop++) {
                        pMsg->flexidag_input_buffer_daddr[io_loop]    = pHandle->input_buffer_daddr[io_loop];
                        pMsg->flexidag_input_buffer_size[io_loop]     = pHandle->input_buffer_size[io_loop];
                    } /* for (io_loop = 0; io_loop < FLEXIDAG_MAX_INPUTS; io_loop++) */
                    for (io_loop = 0; io_loop < FLEXIDAG_MAX_FEEDBACKS; io_loop++) {
                        pMsg->flexidag_feedback_buffer_daddr[io_loop] = pHandle->feedback_buffer_daddr[io_loop];
                        pMsg->flexidag_feedback_buffer_size[io_loop]  = pHandle->feedback_buffer_size[io_loop];
                    } /* for (io_loop = 0; io_loop < FLEXIDAG_MAX_FEEDBACKS; io_loop++) */
                    for (io_loop = 0; io_loop < FLEXIDAG_MAX_OUTPUTS; io_loop++) {
                        pMsg->flexidag_output_buffer_daddr[io_loop]   = pHandle->output_buffer_daddr[io_loop];
                        pMsg->flexidag_output_buffer_size[io_loop]    = pHandle->output_buffer_size[io_loop];
                        flexidag_system_support.pOutputMemBlk[token_id][io_loop] = pHandle->pNextOutputMemblk[io_loop];
                    } /* for (io_loop = 0; io_loop < FLEXIDAG_MAX_OUTPUTS; io_loop++) */

                    thread_mutex_lock(&flexidag_system_support.token_mutex);

                    schdrmsg_send_msg(&msgbase, sizeof(msgbase));

                    retval = flexidag_wait_token(token_id, 0);

                    thread_mutex_unlock(&pHandle->run_mutex);
                    pHandle->pending_token_id = 0xFFFFFFFFU;

                    if (pTokenId != NULL) {
                        *pTokenId = token_id;
                    } /* if (pTokenId != NULL) */
                } /* if (token_id < MAX_FLEXIDAG_TOKENS) */
            } /* if ((pHandle->vpStateBuffer != NULL) && (pHandle->state_buffer_daddr != 0)) */
        } /* if (pHandle == pHandle_expected) */
    } /* if (vpHandle != NULL) */

    return retval;

} /* drv_flexidag_run_noblock() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG API call : drv_flexidag_wait_run_finish() (AMALGAM) =-------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t drv_flexidag_wait_run_finish(void *vpHandle, uint32_t token_id, flexidag_runinfo_t *pRunInfo)
{
    int32_t retval;

    if (vpHandle == NULL) {
        printf("[ERROR] : drv_flexidag_wait_run_finish() : Invalid vpHandle (%p)\n", vpHandle);
        retval = -EINVAL;
    } /* if (vpHandle == NULL) */
    else if (token_id >= MAX_FLEXIDAG_TOKENS) {
        printf("[ERROR] : drv_flexidag_wait_run_finish() : Invalid token_id (%d, max %d)\n", token_id, MAX_FLEXIDAG_TOKENS);
        retval = -EINVAL;
    } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
    else { /* if ((vpHandle != NULL) && (token_id < MAX_FLEXIDAG_TOKENS)) */
        flexidag_slot_state_t *pHandle;
        flexidag_slot_state_t *pHandle_expected;

        pHandle           = (flexidag_slot_state_t *)vpHandle;
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            printf("[ERROR] : drv_flexidag_wait_run_finish() : kernel possibly corrupted : vpHandle(%p) slot_id=%d; expected pHandle(%p) slot_id=%d\n",
                   (void *)pHandle, pHandle->flexidag_slot_id, pHandle_expected, pHandle_expected->flexidag_slot_id);
            retval = -EINVAL;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            thread_mutex_lock(&flexidag_system_support.token_mutex);

            retval = flexidag_wait_token(token_id, 2);

            if (pRunInfo != NULL) {
                uint32_t  loop;
                memset(pRunInfo, 0, sizeof(*pRunInfo));
                pRunInfo->version           = 1;
                pRunInfo->size              = sizeof(*pRunInfo);
                pRunInfo->overall_retcode   = flexidag_system_support.token_retcode[token_id];
                pRunInfo->start_time        = flexidag_system_support.token_start_time[token_id];
                pRunInfo->end_time          = flexidag_system_support.token_end_time[token_id];
                for (loop = 0; loop < FLEXIDAG_MAX_OUTPUTS; loop++) {
                    pRunInfo->output_donetime[loop] = flexidag_system_support.token_output_donetime[token_id][loop];
                }
            } /* if (pRunInfo != NULL) */
        } /* if (pHandle == pHandle_expected) */
    } /* if ((vpHandle != NULL) && (token_id < MAX_FLEXIDAG_TOKENS)) */

    return retval;

} /* drv_flexidag_wait_run_finish() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG API call : flexidag_close() (AMALGAM) =---------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t  drv_flexidag_close(void *vpHandle)
{
    int32_t retval;

    if (vpHandle == NULL) {
        printf("[error] : drv_flexidag_close() : Invalid vpHandle (%p)\n", (void *)vpHandle);
        retval = -EINVAL;
    } /* if (vpHandle == NULL) */
    else { /* if (vpHandle != NULL) */
        flexidag_slot_state_t *pHandle;
        flexidag_slot_state_t *pHandle_expected;

        pHandle           = (flexidag_slot_state_t *)vpHandle;
        pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
        if (pHandle != pHandle_expected) {
            printf("[error] : drv_flexidag_close() : Invalid vpHandle (%p), slot marked = %d, expected (%p) for that slot\n",
                   (void *)pHandle, pHandle->flexidag_slot_id, (void *)pHandle_expected);
            retval = -EINVAL;
        } /* if (pHandle != pHandle_expected) */
        else { /* if (pHandle == pHandle_expected) */
            uint32_t  token_id;

            token_id = flexidag_find_token();

            if (token_id >= MAX_FLEXIDAG_TOKENS) {
                printf("[error] : drv_flexidag_open(%p) : Unable to find token in system (max=%d)\n", (void *)vpHandle, MAX_FLEXIDAG_TOKENS);
                retval = -EINVAL;
            } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
            else { /* if (token_id < MAX_FLEXIDAG_TOKENS) */
                armvis_msg_t msgbase;
                schedmsg_flexidag_close_request_t *pMsg;

                msgbase.hdr.message_type    = (SCHEDMSG_FLEXIDAG_CLOSE_REQUEST | 0x8000);
                msgbase.hdr.message_id      = 0x0100 + pHandle->flexidag_slot_id;
                msgbase.hdr.message_retcode = ERRCODE_NONE;

                pMsg                  = &msgbase.msg.flexidag_close_request;
                pMsg->flexidag_token  = token_id;

                thread_mutex_lock(&flexidag_system_support.token_mutex);

                schdrmsg_send_msg(&msgbase, sizeof(msgbase));

                retval = flexidag_wait_token(token_id, 1);

                thread_mutex_lock(&flexidag_system_support.slot_mutex);
                flexidag_system_support.slot_state[pHandle->flexidag_slot_id] = FLEXIDAG_SLOT_UNUSED;
                thread_mutex_unlock(&flexidag_system_support.slot_mutex);
            } /* if (token_id < MAX_FLEXIDAG_TOKENS) */
        } /* if (pHandle == pHandle_expected) */
    } /* if (vpHandle != NULL) */

    return retval;

} /* drv_flexidag_close() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG reply handlers (AMALGAM) =----------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t drv_flexidag_process_kernel_reply(armvis_msg_t *pMsg)
{
    int32_t retval;

    if (pMsg == NULL) {
        printf("[error] : drv_flexidag_process_kernel_reply(%p) : Invalid pMsg\n", (void *)pMsg);
        retval = -EINVAL;
    } /* if (pMsg == NULL) */
    else { /* if (pMsg  != NULL) */
        if ((pMsg->hdr.message_id >= 0x100) && (pMsg->hdr.message_id < (0x0100+FLEXIDAG_MAX_SLOTS))) {
            uint32_t  flexidag_slot_id;

            retval            = 0;
            flexidag_slot_id  = pMsg->hdr.message_id - 0x0100;

            switch (pMsg->hdr.message_type) {
            case SCHEDMSG_FLEXIDAG_CREATE_REPLY:
                flexidag_release_token(pMsg->msg.flexidag_create_reply.flexidag_token, pMsg->hdr.message_retcode, pMsg->msg.flexidag_create_reply.flexidag_slot_id, 0);
                break;

            case SCHEDMSG_FLEXIDAG_OPEN_REPLY: {
                uint32_t output_loop;
                flexidag_slot_state_t *pHandle;

                pHandle = &flexidag_slot_state[flexidag_slot_id];
                pHandle->memreq.flexidag_num_outputs        = pMsg->msg.flexidag_open_reply.flexidag_num_outputs;
                pHandle->memreq.flexidag_temp_buffer_size   = pMsg->msg.flexidag_open_reply.flexidag_memsize_temp;
                pHandle->memreq.flexidag_state_buffer_size  = pMsg->msg.flexidag_open_reply.flexidag_memsize_state;
                for (output_loop = 0; output_loop < FLEXIDAG_MAX_OUTPUTS; output_loop++) {
                    pHandle->memreq.flexidag_output_buffer_size[output_loop] = pMsg->msg.flexidag_open_reply.flexidag_output_size[output_loop];
                } /* for (output_loop = 0; output_loop < FLEXIDAG_MAX_OUTPUTS; output_loop++) */
            }
            flexidag_release_token(pMsg->msg.flexidag_open_reply.flexidag_token, pMsg->hdr.message_retcode, 0, 0);
            break;

            case SCHEDMSG_FLEXIDAG_INIT_REPLY:
                flexidag_release_token(pMsg->msg.flexidag_init_reply.flexidag_token, pMsg->hdr.message_retcode, 0, 0);
                break;

            case SCHEDMSG_FLEXIDAG_OUTPUT_DONE:
                pMsg->msg.flexidag_output_reply.vpOutputBlk = flexidag_system_support.pOutputMemBlk[pMsg->msg.flexidag_output_reply.flexidag_token][pMsg->msg.flexidag_output_reply.flexidag_output_num];
                flexidag_system_support.token_output_donetime[pMsg->msg.flexidag_output_reply.flexidag_token][pMsg->msg.flexidag_output_reply.flexidag_output_num] = pMsg->msg.flexidag_output_reply.flexidag_output_donetime;
                break;

            case SCHEDMSG_FLEXIDAG_RUN_REPLY:
                flexidag_release_token(pMsg->msg.flexidag_run_reply.flexidag_token, pMsg->hdr.message_retcode, pMsg->msg.flexidag_run_reply.start_time, pMsg->msg.flexidag_run_reply.end_time);
                break;

            case SCHEDMSG_FLEXIDAG_CLOSE_REPLY:
                flexidag_release_token(pMsg->msg.flexidag_close_reply.flexidag_token, pMsg->hdr.message_retcode, 0, 0);
                break;

            default:
                printf("[error] : drv_flexidag_process_kernel_reply(%p) : Invalid pMsg->hdr.message_type(%04x)\n", (void *)pMsg, pMsg->hdr.message_type);
                retval = -EINVAL;
                break;
            } /* switch (pMsg->hdr.message_type) */

        } /* if ((pMsg->hdr.message_id >= 0x100) && (pMsg->hdr.message_id < (0x0100+FLEXIDAG_MAX_SLOTS))) */
        else { /* if ((pMsg->hdr.message_id < 0x100) || (pMsg->hdr.message_id >= (0x0100+FLEXIDAG_MAX_SLOTS))) */
            printf("[error] : drv_flexidag_process_kernel_reply(%p) : pMsg->hdr.message_id is not valid for flexidag (0x%04x - should be 0x%04x - 0x%04x\n",
                   (void *)pMsg, pMsg->hdr.message_id, 0x0100, 0x0100 + FLEXIDAG_MAX_SLOTS);
            retval = -EINVAL;
        } /* if ((pMsg->hdr.message_id < 0x100) || (pMsg->hdr.message_id >= (0x0100+FLEXIDAG_MAX_SLOTS)) */
    } /* if (pMsg  != NULL) */

    return retval;

} /* drv_flexidag_process_kernel_reply() */

flexidag_trace_t *drv_schdr_get_pFlexidagTrace(void)
{
    printf("[AMALGAM] Unsupported call to drv_schdr_get_pFlexidagTrace; returning NULL\n");
    return NULL;
} /* drv_schdr_get_pFlexidagTrace() */

void drv_schdr_set_pFlexidagTrace(uint64_t trace_daddr)
{
    /* do nothing */
}

uint32_t drv_flexidag_show_info(void *vpHandle)
{
    /* do nothing */
    return 0;
}
