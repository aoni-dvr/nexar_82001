/**
 *  @file amalgam_cavalry_kernel.c
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
 *  @details Amalgam Cavalry APIs
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
#include <cavalry_ioctl.h>
#include "schdrmsg_def.h"
#include "cvtask_hw.h"
#include "os_api.h"
#include "ambacv_kal.h"
#include "idsp_vis_msg.h"
#include <cvtask_errno.h>
#include <schdr_api.h>
#include <schdr.h>
#include "schdr_internal.h"
#include "msg.h"
#include "flexidag.h"
#include "flexidag_internal.h"
#include "cavalry.h"

/*-= From original cavalry.h =----------------------------------------------------------*/

#define MSG_SIZE    (128U)
#define MAX_MSG_CNT (32)
// CMD_Q should be big enough to hold all DAGs' descriptors
#define CAVALRY_MEM_CMD_SIZE (1 << 20)
#define CAVALRY_MEM_MSG_SIZE (MSG_SIZE * MAX_MSG_CNT)
#define CAVALRY_MEM_LOG_SIZE    (128 << 10)

#define CAVALRY_MEM_CMD_OFFSET  (0x0 + MSG_SIZE)
#define CAVALRY_MEM_MSG_OFFSET  (CAVALRY_MEM_CMD_OFFSET + CAVALRY_MEM_CMD_SIZE)
#define CAVALRY_MEM_LOG_OFFSET  (CAVALRY_MEM_MSG_OFFSET + CAVALRY_MEM_MSG_SIZE)
#define CAVALRY_MEM_UCODE_OFFSET    (0x400000)
#define CAVALRY_MEM_USER_OFFSET (0x600000)

#define CAVALRY_LOG_RING_BUF_SIZE   (1 << 20)

struct cavalry_mem_info {
    unsigned long phys;
    void *virt;
    unsigned long size;
};

typedef struct {
    pmutex_t        token_mutex;
    uint32_t        num_slots_enabled;
    uint32_t        next_slot_id;
    uint8_t         slot_state[MAX_CAVALRY_SLOTS];
    uint32_t        slot_daddr[MAX_CAVALRY_SLOTS];
    uint32_t        slot_retcode[MAX_CAVALRY_SLOTS];
    sem_t           slot_token[MAX_CAVALRY_SLOTS];
    uint32_t        cavalry_memblock_base;
    uint32_t        cavalry_memblock_size;

    /*-= From original cavalry driver =-----------------------------*/
    struct cavalry_mem_info cavalry_mem_info[CAVALRY_MEM_NUM];
    pmutex_t        cavalry_buf_query_mutex;

} cavalry_support_t;

#define CAV_SLOT_AVAILABLE      0
#define CAV_SLOT_USED           1
#define CAV_SLOT_WAITING        2
#define CAV_SLOT_FINISHED       3

cavalry_support_t   cavalry_support;

/*------------------------------------------------------------------------------------------------*/
/*-= CAVALRY system init (AMALGAM) =--------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void cavalry_system_init(void)
{
    uint32_t   loop;

    thread_mutex_init(&cavalry_support.token_mutex);
    thread_mutex_lock(&cavalry_support.token_mutex);

    cavalry_support.num_slots_enabled = 0;
    cavalry_support.next_slot_id      = 0;
    for (loop = 0; loop < MAX_CAVALRY_SLOTS; loop++) {
        cavalry_support.slot_state[loop]    = CAV_SLOT_AVAILABLE;
        cavalry_support.slot_daddr[loop]    = 0;
        cavalry_support.slot_retcode[loop]  = 0;
        sem_init(&(cavalry_support.slot_token[loop]), 0, 0);
    } /* for (loop = 0; loop < MAX_CAVALRY_SLOTS; loop++) */

    cavalry_support.cavalry_memblock_base = 0;
    cavalry_support.cavalry_memblock_size = 0;

    thread_mutex_unlock(&cavalry_support.token_mutex);

} /* cavalry_tokens_init() */

/*------------------------------------------------------------------------------------------------*/
/*-= CAVALRY token management (AMALGAM)  =--------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t  cavalry_find_slot(uint32_t *pRunDagDaddr)
{
    uint32_t  slot_found;
    uint32_t  slot_num;
    uint32_t  slot_id;
    uint32_t  retcode;
    uint32_t  loop;

    slot_found  = 0;
    slot_num    = 0;
    slot_id     = cavalry_support.next_slot_id;
    retcode     = 0;
    loop        = 0;

    thread_mutex_lock(&cavalry_support.token_mutex);
    while ((slot_found == 0) && (loop < cavalry_support.num_slots_enabled)) {
        if (cavalry_support.slot_state[slot_id] == CAV_SLOT_AVAILABLE) {
            slot_found  = 1;
            slot_num    = slot_id;
            cavalry_support.slot_state[slot_id]   = CAV_SLOT_USED;
            cavalry_support.slot_retcode[slot_id] = ERRCODE_NONE;
        } /* if (cavalry_support.slot_state[slot_id] == CAV_SLOT_AVAILABLE) */
        else { /* if (cavalry_support.slot_state[slot_id] != CAV_SLOT_AVAILABLE) */
            loop++;
            slot_id++;
            if (slot_id >= cavalry_support.num_slots_enabled) slot_id = 0;
        } /* if (cavalry_support.slot_state[slot_id] != CAV_SLOT_AVAILABLE) */
    } /* while ((token_found == 0) && (loop < MAX_CAVALRY_TOKENS)) */
    thread_mutex_unlock(&cavalry_support.token_mutex);

    if (slot_found == 0) {
        /* ERROR */
        retcode = 0xFFFFFFFF;
    } /* if (slot_found == 0) */
    else { /* if (slot_found != 0) */
        retcode       = slot_id;
        *pRunDagDaddr = cavalry_support.slot_daddr[slot_id];
        cavalry_support.next_slot_id = (slot_id + 1) % (cavalry_support.num_slots_enabled);
    } /* if (slot_found != 0) */

    return retcode;

} /* cavalry_find_slot() */

uint32_t  cavalry_wait_slot(uint32_t slot_id)
{
    uint32_t  wait_valid;
    uint32_t  retcode;

    retcode       = ERRCODE_NONE;
    if (slot_id >= cavalry_support.num_slots_enabled) {
        /* ERROR */
        wait_valid  = 0;
        retcode     = 0xFFFFFFFFU;
    } /* if (slot_id >= cavalry_support.num_slots_enabled) */
    else { /* if (slot_id < cavalry_support.num_slots_enabled) */
        thread_mutex_lock(&cavalry_support.token_mutex);
        if (cavalry_support.slot_state[slot_id] != CAV_SLOT_USED) {
            /* ERROR */
            wait_valid  = 0;
            retcode     = 0xFFFFFFFFU;
        } /* if (cavalry_support.slot_state[slot_id] != CAV_SLOT_USED) */
        else { /* if (cavalry_support.slot_state[slot_id] == CAV_SLOT_USED) */
            cavalry_support.slot_state[slot_id] = CAV_SLOT_WAITING;
            wait_valid = 1;
        } /* if (cavalry_support.slot_state[slot_id] == CAV_SLOT_USED) */
        thread_mutex_unlock(&cavalry_support.token_mutex);
    } /* if (slot_id < cavalry_support.num_slots_enabled) */

    if (wait_valid != 0) {
        sem_wait(&cavalry_support.slot_token[slot_id]);
        thread_mutex_lock(&cavalry_support.token_mutex);
        if (cavalry_support.slot_state[slot_id] == CAV_SLOT_FINISHED) {
            retcode = cavalry_support.slot_retcode[slot_id];
        } /* if (cavalry_support.slot_state[slot_id] == CAV_SLOT_FINISHED) */
        else { /* if (cavalry_support.slot_state[slot_id] != CAV_SLOT_FINISHED) */
            /* ERROR */
            retcode = 0xFFFFFFFFU;
        } /* if (cavalry_support.slot_state[slot_id] != CAV_SLOT_FINISHED) */
        cavalry_support.slot_state[slot_id] = CAV_SLOT_AVAILABLE;
        thread_mutex_unlock(&cavalry_support.token_mutex);
    } /* if (wait_valid != 0) */

    return retcode;

} /* cavalry_wait_slot() */

uint32_t  cavalry_release_slot(uint32_t slot_id, uint32_t function_retcode)
{
    uint32_t  release_valid;
    uint32_t  retcode;

    retcode       = ERRCODE_NONE;
    if (slot_id >= cavalry_support.num_slots_enabled) {
        /* ERROR */
        release_valid = 0;
        retcode       = 0xFFFFFFFFU;
    } /* if (slot_id >= cavalry_support.num_slots_enabled) */
    else { /* if (slot_id < cavalry_support.num_slots_enabled) */
        thread_mutex_lock(&cavalry_support.token_mutex);
        if (cavalry_support.slot_state[slot_id] != CAV_SLOT_WAITING) {
            /* ERROR */
            release_valid = 0;
            retcode       = 0xFFFFFFFFU;
        } /* if (cavalry_support.slot_state[slot_id] != CAV_SLOT_WAITING) */
        else { /* if (cavalry_support.slot_state[slot_id] == CAV_SLOT_WAITING) */
            release_valid = 1;
        } /* if (cavalry_support.slot_state[slot_id] == CAV_SLOT_WAITING) */
        thread_mutex_unlock(&cavalry_support.token_mutex);
    } /* if (slot_id < cavalry_support.num_slots_enabled) */

    if (release_valid != 0) {
        thread_mutex_lock(&cavalry_support.token_mutex);
        cavalry_support.slot_state[slot_id]   = CAV_SLOT_FINISHED;
        cavalry_support.slot_retcode[slot_id] = function_retcode;
        thread_mutex_unlock(&cavalry_support.token_mutex);
        sem_post(&cavalry_support.slot_token[slot_id]);
    } /* if (release_valid != 0) */

    return retcode;

} /* cavalry_release_slot() */

/*-----------------------------------------------------------------------------------------------------*/
/*-= CAVALRY driver API (AMALGAM) =--------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------*/
int32_t drv_cavalry_enable(uint32_t num_slots)
{
    int32_t retval;

    if (num_slots > MAX_CAVALRY_SLOTS) {
        cavalry_support.num_slots_enabled = 0;
        printf("[error] : drv_cavalry_enable() : Invalid number of slots (%d, max %d)\n", num_slots, MAX_CAVALRY_SLOTS);
        retval = -ECAVALRY_INVALID_SLOT_ID;
    } /* if (num_slots > MAX_CAVALRY_SLOTS) */
    else { /* if (num_slots <= MAX_CAVALRY_SLOTS) */
        cavalry_support.num_slots_enabled = num_slots;
        printf("drv_cavalry_enable() : Cavalry enabled for %d slots\n", cavalry_support.num_slots_enabled);
        retval = (CAVALRY_VERSION << 8);
    } /* if (num_slots <= MAX_CAVALRY_SLOTS) */

    return retval;

} /* drv_cavalry_enable() */

/*------------------------------------------------------------------------------------------------*/
/*-= Cavalry driver interface (AMALGAM) =---------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
int32_t drv_cavalry_query_buf(void *pQueryBuf)
{
    struct cavalry_querybuf cavalry_querybuf;
    cavalry_support_t *cavalry;
    int32_t buf_id = 0;
    int32_t rval = 0;

    cavalry = &cavalry_support;

    memcpy(&cavalry_querybuf, pQueryBuf, sizeof(cavalry_querybuf));

    buf_id = cavalry_querybuf.buf;

    if ((buf_id < CAVALRY_MEM_FIRST) || (buf_id > CAVALRY_MEM_LAST)) {
        printf("Invalid buf ID %u\n", buf_id);
        rval = -EINVAL;
    } /* if ((buf_id < CAVALRY_MEM_FIRST) || (buf_id > CAVALRY_MEM_LAST)) */
    else { /* if ((buf_id >= CAVALRY_MEM_FIRST) && (buf_id <= CAVALRY_MEM_LAST)) */
        thread_mutex_lock(&cavalry->cavalry_buf_query_mutex);
        cavalry_querybuf.length = cavalry->cavalry_mem_info[buf_id].size;
        cavalry_querybuf.offset = cavalry->cavalry_mem_info[buf_id].phys;
        thread_mutex_unlock(&cavalry->cavalry_buf_query_mutex);
        memcpy(pQueryBuf, &cavalry_querybuf, sizeof(cavalry_querybuf));
    } /* if ((buf_id >= CAVALRY_MEM_FIRST) && (buf_id <= CAVALRY_MEM_LAST)) */

    return rval;

} /* drv_cavalry_query_buf() */

int32_t drv_cavalry_start_vp(void *vpUserData)
{
    /* Dummy call for now */
    return 0;
} /* drv_cavalry_start_vp() */

int32_t drv_cavalry_stop_vp(void *vpUserData)
{
    /* Dummy call for now */
    return 0;
} /* drv_cavalry_stop_vp() */

int32_t drv_cavalry_run_dags(void *pParams)
{
    int32_t retval;

    if (pParams == NULL) {
        printf("[error] : drv_cavalry_run_dags() : Invalid pParams(%p)\n", (void *)pParams);
        retval = -EINVAL;
    } /* if (pParams == NULL) */
    else { /* if (pParams != NULL) */
        uint32_t  slot_id;
        uint32_t  slot_daddr;
        struct cavalry_run_dags *pRunDags;

        slot_id = cavalry_find_slot(&slot_daddr);
        pRunDags = ambacv_p2v(slot_daddr);

        if (slot_id >= cavalry_support.num_slots_enabled) {
            printf("[error] : drv_cavalry_run_dags(%p) : Unable to find slot in system (enabled = %d)\n", (void *)pParams, cavalry_support.num_slots_enabled);
            retval = -ECAVALRY_UNABLE_TO_FIND_TOKEN;
        } /* if (slot_id >= cavalry_support.num_slots_enabled) */
        else { /* if (slot_id >= cavalry_support.num_slots_enabled) */
            armvis_msg_t msgbase;
            schedmsg_cavalry_vp_run_request_t  *pCavalryMsg;
            struct cavalry_run_dags *pSrcRunDags;
            uint32_t  loop;
            uint32_t  clean_size;

            pSrcRunDags = (struct cavalry_run_dags *)pParams;

            /* memcpy in PACE, copy_from_user in real kernel */
            clean_size = 20;
            memcpy((void *)&pRunDags->dag_cnt, (void *)&pSrcRunDags->dag_cnt, sizeof(pRunDags->dag_cnt));
            for (loop = 0; loop < pRunDags->dag_cnt; loop++) {
                memcpy((void *)&pRunDags->dag_desc[loop], (void *)&pSrcRunDags->dag_desc[loop], sizeof(pRunDags->dag_desc[loop]));
                clean_size += sizeof(pRunDags->dag_desc[loop]);
            } /* for (loop = 0; loop < pRunDags->dag_cnt; loop++) */
            clean_size = (clean_size + 64) & 0xFFFFFFC0;
            ambacv_cache_clean(pRunDags, clean_size);

            memset(&msgbase, 0, sizeof(msgbase));

            retval      = 0;
            pCavalryMsg = &msgbase.msg.cavalry_vp_run_request;
            msgbase.hdr.message_type                = SCHEDMSG_CAVALRY_VP_RUN_REQUEST;
            pCavalryMsg->requested_function_token   = slot_id;
            pCavalryMsg->priority                   = 1;
            pCavalryMsg->parameter_daddr            = slot_daddr;
            pCavalryMsg->local_printf_buffer_daddr  = 0;
            pCavalryMsg->local_printf_buffer_size   = 0;

            schdrmsg_send_msg(&msgbase, sizeof(msgbase));

            retval = cavalry_wait_slot(slot_id);

            /* memcpy in PACE, copy_to_user in real kernel */
            ambacv_cache_invalidate(pRunDags, 64);
            ambacv_cache_barrier();
            memcpy(&pSrcRunDags->rval, &pRunDags->rval, 20);

        } /* if (slot_id >= cavalry_support.num_slots_enabled) */
    } /* if (pParams != NULL) */

    return retval;

} /* drv_cavalry_run_dags() */

int32_t drv_cavalry_start_log(void *vpUserData)
{
    /* Dummy call for now */
    return 0;
} /* drv_cavalry_start_log() */

int32_t drv_cavalry_stop_log(void *vpUserData)
{
    /* Dummy call for now */
    return 0;
} /* drv_cavalry_stop_log() */

int32_t drv_cavalry_early_quit(void *pEarlyQuit)
{
    /* Dummy call for now */
    return 0;
} /* drv_cavalry_early_quit() */

int32_t drv_cavalry_process_reply(const armvis_msg_t *pMsg)
{
    schedmsg_cavalry_vp_run_reply_t  *rpl = &pMsg->msg.cavalry_vp_run_reply;

    printf("Received CAVALRY run-function reply for token_id 0x%08x, retcode 0x%08x\n", rpl->requested_function_token, rpl->function_retcode);

    cavalry_release_slot(rpl->requested_function_token, rpl->function_retcode);

    return 0;
} /* drv_cavalry_process_reply() */

int32_t drv_cavalry_set_memory_block(armvis_msg_t *pSchdrMsg)
{
    int32_t retval = ERRCODE_NONE;

    if (pSchdrMsg == NULL) {
        printf("[error] : drv_cavalry_set_memory_block() : Invalid message provided (%p)\n", (void *)pSchdrMsg);
        retval = -EINVAL;
    } /* if (pSchdrMsg == NULL) */
    else { /* if (pSchdrMsg != NULL) */
        if (pSchdrMsg->hdr.message_type == SCHEDMSG_CVSCHEDULER_REPORT_MEMORY) {
            uint64_t  cavalry_start;
            uint64_t  cavalry_end;
            schedmsg_cvscheduler_memory_report_t *pMsg;

            pMsg = &pSchdrMsg->msg.memory_report;

            if (AMALGAM_CMA_SIZE != 0U) {
                cavalry_start = AMALGAM_CMA_BASE;
                cavalry_end   = cavalry_start + AMALGAM_CMA_SIZE;
            } /* if (AMALGAM_CMA_SIZE != 0U) */
            else { /* if (AMALGAM_CMA_SIZE == 0U) */
                cavalry_start = pMsg->orc_uncached_baseaddr + pMsg->orc_uncached_size;
                cavalry_end   = pMsg->armblock_baseaddr;
            } /* if (AMALGAM_CMA_SIZE == 0U) */

            cavalry_start = (cavalry_start + 4095) & 0xFFFFFFF000UL;
            cavalry_end   = (cavalry_end) & 0xFFFFFFF000UL;

            cavalry_support.cavalry_memblock_base = cavalry_start;
            cavalry_support.cavalry_memblock_size = cavalry_end - cavalry_start;
            printf("drv_cavalry_set_memory_block() : CAVALRY block set up : 0x%010lx -> 0x%010lx (size=%d)\n",
                   cavalry_start, cavalry_end, cavalry_support.cavalry_memblock_size);
            schdr_cma_mmap_init(cavalry_start, cavalry_end);
            schdr_sys_state.visorc_init.cma_region_daddr  = cavalry_support.cavalry_memblock_base; /* Patch this in for use  */
            schdr_sys_state.visorc_init.cma_region_size   = cavalry_support.cavalry_memblock_size; /* -/                     */

            /*-= Partition memory block as per original cavalry driver =--------------------------*/
            {
                uint64_t  base;
                uint64_t  size;
                uint8_t  *vbase;

                cavalry_support_t *cavalry;

                base    = cavalry_start;
                size    = cavalry_end - cavalry_start;
                vbase   = ambacv_p2v(base);
                cavalry = &cavalry_support;

                cavalry->cavalry_mem_info[CAVALRY_MEM_ALL].phys = base;
                cavalry->cavalry_mem_info[CAVALRY_MEM_ALL].virt = vbase;
                cavalry->cavalry_mem_info[CAVALRY_MEM_ALL].size = size;

                cavalry->cavalry_mem_info[CAVALRY_MEM_CMD].phys = base + CAVALRY_MEM_CMD_OFFSET;
                cavalry->cavalry_mem_info[CAVALRY_MEM_CMD].virt = vbase + CAVALRY_MEM_CMD_OFFSET;
                cavalry->cavalry_mem_info[CAVALRY_MEM_CMD].size = CAVALRY_MEM_CMD_SIZE;

                cavalry->cavalry_mem_info[CAVALRY_MEM_MSG].phys = base + CAVALRY_MEM_MSG_OFFSET;
                cavalry->cavalry_mem_info[CAVALRY_MEM_MSG].virt = vbase + CAVALRY_MEM_MSG_OFFSET;
                cavalry->cavalry_mem_info[CAVALRY_MEM_MSG].size = CAVALRY_MEM_MSG_SIZE;

                cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].phys = base + CAVALRY_MEM_LOG_OFFSET;
                cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].virt = vbase + CAVALRY_MEM_LOG_OFFSET;
                cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].size = CAVALRY_MEM_LOG_SIZE;

                cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].phys = base + CAVALRY_MEM_UCODE_OFFSET;
                cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].virt = vbase + CAVALRY_MEM_UCODE_OFFSET;
                cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].size = CAVALRY_MEM_USER_OFFSET - CAVALRY_MEM_UCODE_OFFSET;

                cavalry->cavalry_mem_info[CAVALRY_MEM_USER].phys = base + CAVALRY_MEM_USER_OFFSET;
                cavalry->cavalry_mem_info[CAVALRY_MEM_USER].virt = vbase + CAVALRY_MEM_USER_OFFSET;
                cavalry->cavalry_mem_info[CAVALRY_MEM_USER].size = size - CAVALRY_MEM_USER_OFFSET;

                thread_mutex_init(&cavalry->cavalry_buf_query_mutex);
            }

            /* Send setup message */
            if (cavalry_start <= 0xFFFFFFFFUL) { // do not send cavalry setup, if using simulated cma space for 40-bit addressing tests
                armvis_msg_t msgbase;
                schedmsg_cavalry_setup_t *pSetupMsg;
                cavalry_support_t *cavalry;

                cavalry = &cavalry_support;
                msgbase.hdr.message_type    = SCHEDMSG_CAVALRY_SETUP;
                pSetupMsg                   = &msgbase.msg.cavalry_setup;
                pSetupMsg->all_base_daddr   = cavalry->cavalry_mem_info[CAVALRY_MEM_ALL].phys;
                pSetupMsg->all_size         = cavalry->cavalry_mem_info[CAVALRY_MEM_ALL].size;
                pSetupMsg->cmd_base_daddr   = cavalry->cavalry_mem_info[CAVALRY_MEM_CMD].phys;
                pSetupMsg->cmd_size         = cavalry->cavalry_mem_info[CAVALRY_MEM_CMD].size;
                pSetupMsg->msg_base_daddr   = cavalry->cavalry_mem_info[CAVALRY_MEM_MSG].phys;
                pSetupMsg->msg_size         = cavalry->cavalry_mem_info[CAVALRY_MEM_MSG].size;
                pSetupMsg->log_base_daddr   = cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].phys;
                pSetupMsg->log_size         = cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].size;
                pSetupMsg->ucode_base_daddr = cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].phys;
                pSetupMsg->ucode_size       = cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].size;
                pSetupMsg->user_base_daddr  = cavalry->cavalry_mem_info[CAVALRY_MEM_USER].phys;
                pSetupMsg->user_size        = cavalry->cavalry_mem_info[CAVALRY_MEM_USER].size;
                schdrmsg_send_msg(&msgbase, sizeof(msgbase));
            }
        } /* if (pSchdrMsg->hdr.message_type == SCHEDMSG_CVSCHEDULER_REPORT_MEMORY) */
        else { /* if (pSchdrMsg->hdr.message_type != SCHEDMSG_CVSCHEDULER_REPORT_MEMORY) */
            printf("[error] : drv_cavalry_set_memory_block() : Invalid message provided (%p) - type = 0x%04x (should be 0x%04x)- Ignoring\n",
                   (void *)pSchdrMsg, pSchdrMsg->hdr.message_type, SCHEDMSG_CVSCHEDULER_REPORT_MEMORY);
            retval = -EINVAL;
        } /* if (pSchdrMsg->hdr.message_type != SCHEDMSG_CVSCHEDULER_REPORT_MEMORY) */
    } /* if (pSchdrMsg != NULL) */

    return retval;

} /* drv_cavalry_set_memory_block() */

int32_t drv_cavalry_associate_buf(uint64_t cavalry_base_daddr)
{
    uint32_t loop;
    uint32_t  cavalry_curr_daddr;

    cavalry_curr_daddr = cavalry_base_daddr;
    for (loop = 0; loop < cavalry_support.num_slots_enabled; loop++) {
        cavalry_support.slot_daddr[loop]  = cavalry_curr_daddr;
        cavalry_curr_daddr += CAVALRY_SLOT_SIZE;
    } /* for (loop = 0; loop < cavalry_support.num_slots_enabled; loop++) */
    for (loop = cavalry_support.num_slots_enabled; loop < MAX_CAVALRY_SLOTS; loop++) {
        cavalry_support.slot_daddr[loop]  = 0;
    } /* for (loop = cavalry_support.num_slots_enabled; loop < MAX_CAVALRY_SLOTS; loop++) */

    return 0;
} /* drv_cavalry_associate_buf() */

cavalry_trace_t *drv_schdr_get_pCavalryTrace(void)
{
    printf("[AMALGAM] Unsupported call to drv_schdr_get_pCavalryTrace; returning NULL\n");
    return NULL;
} /* drv_schdr_get_pCavalryTrace() */

void drv_schdr_set_pCavalryTrace(uint64_t trace_daddr)
{
    /* do nothing */
}

