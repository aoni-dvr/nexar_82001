/**
 *  @file amalgam_kernel.c
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
 *  @details Amalgam kernel APIs
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
#include "schdr_api.h"
#include <schdr.h>
#include "schdr_internal.h"
#include "msg.h"
#include "flexidag.h"
#include "flexidag_internal.h"
#include "cavalry.h"

#include <cmd_msg_vin.h>

#define tx_lock()       pthread_mutex_lock(&tx_mutex)
#define tx_unlock()     pthread_mutex_unlock(&tx_mutex)
#define find_lock()     pthread_mutex_lock(&pool_mutex)
#define find_unlock()   pthread_mutex_unlock(&pool_mutex)
#define MSG_INDEX_MASK  (ARMVIS_MESSAGE_NUM - 1)

static pthread_mutex_t  tx_mutex;
static pthread_mutex_t  pool_mutex;

static dsp_msg_q_info_t *vp_qcb;
static uint64_t         vp_qcb_daddr;
static uint64_t         async_q_daddr;

static vin_msg_q_info_t *vin_qcb;
static uint64_t          vin_qcb_daddr;

static uint32_t         ambacv_state = 0U;

extern void cache_invld_schdrmsg(armvis_msg_t *msg);
extern void cache_clean_schdrmsg(armvis_msg_t *msg);

typedef struct {
    uint32_t*               from_orc_r;
    uint32_t*               from_orc_w;
    armvis_msg_t*             from_orc_q;
    uint32_t                from_orc_wi;

    uint32_t*               from_arm_r;
    uint32_t*               from_arm_w;
    armvis_msg_t*             from_arm_q;
} q_config_t;

static pthread_mutex_t  rx_mutex;
static pthread_cond_t   rx_cond;
static pthread_mutex_t  vp_mutex;
static pthread_mutex_t  vin_mutex;
static int32_t          new_msg_posted;
static q_config_t       qconfig;
static void*            kernel_rx_entry(void *arg);

void sod_orc_isr(int32_t id)
{
    // trigger message receiving
    pthread_mutex_lock(&rx_mutex);
    if (!new_msg_posted) {
        new_msg_posted = 1;
        pthread_cond_signal(&rx_cond);
    }
    pthread_mutex_unlock(&rx_mutex);
}

static void invalidate_orc_schdrmsg(void)
{
    armvis_msg_t *msg;
    unsigned index, wi;

    ambacv_cache_invalidate((char*)qconfig.from_orc_w, sizeof(int32_t));
    ambacv_cache_barrier();
    wi = *qconfig.from_orc_w;
    index = qconfig.from_orc_wi;

    //printf("%s: %d --> %d\n", __func__, index, wi);
    while (index != wi) {
        // get one armvis_msg_t from the queue
        msg = &(qconfig.from_orc_q[index & MSG_INDEX_MASK]);
        cache_invld_schdrmsg(msg);
        index++;
    }
    qconfig.from_orc_wi = wi;
}

static void* kernel_rx_entry(void *arg)
{
    for (;;) {
        // wait for network layer to indicate new message
        pthread_mutex_lock(&rx_mutex);
        if (!new_msg_posted) {
            pthread_cond_wait(&rx_cond, &rx_mutex);
        }
        new_msg_posted = 0;
        pthread_mutex_unlock(&rx_mutex);

        // invalidate armvis_msg_t to make it accessible to user-space
        invalidate_orc_schdrmsg();
    }
    return NULL;
}

uint32_t drv_schdr_init_module(visorc_init_params_t *sysconfig)
{
    armvis_message_queue_t *q;
    uint32_t index, buff;

    sysconfig->sysinit_dram_block_baseaddr  = CVTASK_DRAM_BOM;
    sysconfig->sysinit_dram_block_size      = CVTASK_DRAM_SIZE;

    /* mmap the armvis_msg_t region */
    schdr_mmap_init(CVTASK_DRAM_TOP, CVTASK_DRAM_END - CVTASK_DRAM_TOP);

    buff = SCHDR_QBUFF_BASE;
    q = (armvis_message_queue_t *) schdr_p2v(SCHDR_QCONF_BASE);
    for (index = 0; index < SYSTEM_MAX_NUM_CORTEX; index++) {
        /* setup request queue for coprocessor i */
        memset(q, 0, sizeof(*q));
        q->max_count          = ARMVIS_MESSAGE_NUM;
        q->msg_array_daddr    = buff;
        q->queue_id           = index;
        q->rd_msg_array_daddr = q->msg_array_daddr;  /* Shadow copies  */
        q->wr_msg_array_daddr = q->msg_array_daddr;  /*  |             */
        q->rd_queue_id        = q->queue_id;         /*  |             */
        q->wr_queue_id        = q->queue_id;         /* -/             */
        sysconfig->MsgQueue_orc_to_arm_daddr[index]   = schdr_v2p(q);
        q += 1;
        buff += MSG_QUEUE_LENGTH;

        /* setup reply queue for coprocessor i */
        memset(q, 0, sizeof(*q));
        q->max_count          = ARMVIS_MESSAGE_NUM;
        q->msg_array_daddr    = buff;
        q->queue_id           = index;
        q->rd_msg_array_daddr = q->msg_array_daddr;  /* Shadow copies  */
        q->wr_msg_array_daddr = q->msg_array_daddr;  /*  |             */
        q->rd_queue_id        = q->queue_id;         /*  |             */
        q->wr_queue_id        = q->queue_id;         /* -/             */
        sysconfig->MsgQueue_arm_to_orc_daddr[index]   = schdr_v2p(q);
        q += 1;
        buff += MSG_QUEUE_LENGTH;
    }

    // create tx/rx thread for socket @kfd
    pthread_mutex_init(&tx_mutex, NULL);
    pthread_mutex_init(&pool_mutex, NULL);
    pthread_mutex_init(&rx_mutex, NULL);
    pthread_cond_init(&rx_cond, NULL);
    pthread_mutex_init(&vp_mutex, NULL);
    pthread_mutex_init(&vin_mutex, NULL);

    cavalry_system_init();
    drv_flexidag_system_init();
    return 0;
}

uint32_t drv_schdr_resume_module(visorc_init_params_t *sysconfig)
{
    (void) sysconfig;
    return 0;
}

uint32_t drv_schdr_start_visorc(visorc_init_params_t *sysconfig)
{
    int32_t index;
    armvis_message_queue_t *conf;
    pthread_t thread;

    ambacv_state = 1U;
    index = sysconfig->coprocessor_cfg >> 8;

    /* setup local qconfig */
    conf = (armvis_message_queue_t *) schdr_p2v(sysconfig->MsgQueue_orc_to_arm_daddr[index]);
    qconfig.from_orc_r = &(conf->rd_idx);
    qconfig.from_orc_w = &(conf->wr_idx);
    qconfig.from_orc_q = schdr_p2v(conf->msg_array_daddr);

    conf = (armvis_message_queue_t *) schdr_p2v(sysconfig->MsgQueue_arm_to_orc_daddr[index]);
    qconfig.from_arm_r = &(conf->rd_idx);
    qconfig.from_arm_w = &(conf->wr_idx);
    qconfig.from_arm_q = schdr_p2v(conf->msg_array_daddr);

    /* create armvis_msg_t recevier thread */
    pthread_create(&thread, NULL, kernel_rx_entry, 0);

    index = sysconfig->coprocessor_cfg & 0xFF;
    if (index == 0) {
        //dag_send_sync_count(ORC_SYNC_COUNTER);
        return 0;
    }

    if (sysconfig->cavalry_num_slots != 0) {
        sysconfig->sysinit_dram_block_size -= (sysconfig->cavalry_num_slots * CAVALRY_SLOT_SIZE);
        sysconfig->cavalry_base_daddr       = sysconfig->sysinit_dram_block_baseaddr + sysconfig->sysinit_dram_block_size;
    } /* if (sysconfig->cavalry_num_slots != 0) */
    else { /* if (sysconfig->cavalry_num_slots == 0) */
        sysconfig->cavalry_base_daddr = 0;
    } /* if (sysconfig->cavalry_num_slots == 0) */

    if (sysconfig->hotlink_region_size != 0) {
        sysconfig->sysinit_dram_block_size -= (sysconfig->hotlink_region_size);
        sysconfig->hotlink_region_daddr     = sysconfig->sysinit_dram_block_baseaddr + sysconfig->sysinit_dram_block_size;
    } /* if (sysconfig->hotlink_region_size != 0) */
    else { /* if (sysconfig->hotlink_region_size == 0) */
        sysconfig->hotlink_region_daddr = 0;
    } /* if (sysconfig->hotlink_region_size == 0) */

    printf("SUB SCHEDULER booting ORC, num_ss=%d\n", index);
    sysconfig->coprocessor_cfg = index;
    // sync system config struct
    ambacv_cache_clean_phy(sysconfig, sizeof(*sysconfig), CVTASK_SYSPARA_BASE);

    // sync queue configuration
    for (index = 0; index < sysconfig->coprocessor_cfg; index++) {
        void *ptr = schdr_p2v(sysconfig->MsgQueue_orc_to_arm_daddr[index]);
        ambacv_cache_clean(ptr, sizeof(armvis_message_queue_t));
        ptr = schdr_p2v(sysconfig->MsgQueue_arm_to_orc_daddr[index]);
        ambacv_cache_clean(ptr, sizeof(armvis_message_queue_t));
    }
    // trigger ORC to start
    dag_send_sync_count(ORC_SYNC_COUNTER);

#if 1
    printf("=========ORC INIT DATA===========\n");
    printf("     dram_addr: 0x%08X\n", sysconfig->sysinit_dram_block_baseaddr);
    printf("     dram_size: 0x%08X\n", sysconfig->sysinit_dram_block_size);
    printf("     num_copro: %d\n", sysconfig->coprocessor_cfg);
    printf("    sod_cvtask: %X\n", sysconfig->debug_buffer_sod_cvtask_size);
    printf("    sod_sched : %X\n", sysconfig->debug_buffer_sod_sched_size);
    printf("    sod_perf  : %X\n", sysconfig->debug_buffer_sod_perf_size);
    printf("     vp_cvtask: %X\n", sysconfig->debug_buffer_vp_cvtask_size);
    printf("     vp_sched : %X\n", sysconfig->debug_buffer_vp_sched_size);
    printf("     vp_perf  : %X\n", sysconfig->debug_buffer_vp_perf_size);
    for (index = 0; index < sysconfig->coprocessor_cfg; index++) {
        printf("     --------[%d]-------\n", index);
        conf = (armvis_message_queue_t *)
               schdr_p2v(sysconfig->MsgQueue_orc_to_arm_daddr[index]);
        printf("    max_count: %d\n",  conf->max_count);
        printf("         addr: %X\n",  conf->msg_array_daddr);
        conf = (armvis_message_queue_t *)
               schdr_p2v(sysconfig->MsgQueue_arm_to_orc_daddr[index]);
        printf("    max_count: %d\n",  conf->max_count);
        printf("         addr: %X\n",  conf->msg_array_daddr);
    }
    if (sysconfig->cavalry_num_slots != 0) {
        printf("cavalry_slots: %d\n", sysconfig->cavalry_num_slots);
    } /* if (sysconfig->cavalry_num_slots != 0) */
    if ((sysconfig->hotlink_region_daddr != 0) && (sysconfig->hotlink_region_size != 0)) {
        printf("hotlink area : [0x%08x -- 0x%08x]\n", sysconfig->hotlink_region_daddr, sysconfig->hotlink_region_daddr + sysconfig->hotlink_region_size - 1);
    } /* if ((sysconfig->hotlink_region_daddr != 0) && (sysconfig->hotlink_region_size != 0)) */
#endif
    return 0;
}

uint32_t drv_schdr_find_msg(uint64_t addr, uint64_t *retentry)
{
    cvmem_messagepool_t  *pool;
    cvmem_messagebuf_t *entry, *found;
    char *base;
    int32_t pos, top, end, step;
    uint32_t retcode = ERRCODE_NONE;

    find_lock();
    pool = ambacv_p2v(addr);
    ambacv_cache_invalidate(pool, sizeof(*pool));
    ambacv_cache_barrier();

    pos = end = pool->hdr_variable.wr_index;
    top = pool->hdr_variable.message_num;
    step = pool->hdr_variable.message_buffersize;
    base = (char*)ambacv_p2v(pool->hdr_variable.messagepool_basedaddr);
    found = NULL;

    do {
        entry = (cvmem_messagebuf_t *)(base + pos * step);
        ambacv_cache_invalidate(&entry->hdr_variable, sizeof(entry->hdr_variable));
        ambacv_cache_barrier();

        if (++pos == top)
            pos = 0;

        if (entry->hdr_variable.message_in_use == entry->hdr_variable.message_processed) {
            pool->hdr_variable.wr_index = pos;
            entry->hdr_variable.message_in_use    = 1;
            entry->hdr_variable.message_processed = 0;
            found = entry;
            break;
        }

    } while (pos != end);

    find_unlock();

    if(found != NULL) {
        *retentry = ambacv_v2p(found);
    } else {
        retcode = ERR_DRV_SCHDR_MSG_ENTRY_UNABLE_TO_FIND;
    }

    return retcode;
}

static uint32_t msg_rx_w_seqno  = 0;
static uint32_t msg_tx_w_seqno  = 0;

#ifdef ASIL_COMPLIANCE

uint32_t amalgam_schdrmsg_calc_hash(void* buf, uint32_t len, uint32_t hval)
{
    uint8_t *ptr = NULL;
    uint32_t count = 0U;
    uint32_t new_hval = 0U;

    if((buf != NULL) && (len != 0U)) {
        ptr = (uint8_t *)buf;
        new_hval = hval;
        while (count < len) {
            new_hval = new_hval ^(uint32_t)ptr[count];
            new_hval = new_hval + (new_hval<<1) + (new_hval<<4) + (new_hval<<7) + (new_hval<<8) + (new_hval<<24);
            count++;
        }
    }
    return new_hval;
}
#endif /* ?ASIL_COMPLIANCE */

uint32_t schdrmsg_recv_orcmsg(uint32_t ri, uint32_t *wi)
{
    uint32_t retcode = ERRCODE_NONE;

    *qconfig.from_orc_r = ri;
    ambacv_cache_clean(qconfig.from_orc_r, sizeof(uint32_t));

    while (ri == qconfig.from_orc_wi) {
        usleep(10000);
    }

    {
        armvis_msg_t *dst;
        dst = &(qconfig.from_orc_q[ri & MSG_INDEX_MASK]);
        if (dst->hdr.message_seqno != msg_rx_w_seqno) {
            printf("[ASIL][ERROR] : Incoming message mismatched seqno (%d, expected %d) type = %04x\n", dst->hdr.message_seqno, msg_rx_w_seqno, dst->hdr.message_type);
            if ((dst->hdr.message_id >= (uint16_t)0x100U) && (dst->hdr.message_id < (uint16_t)(0x0100U + FLEXIDAG_MAX_SLOTS))) {
                retcode = ERR_DRV_FLEXIDAG_VISMSG_SEQNO_MISMATCHED;
            } else {
                retcode = ERR_DRV_SCHDR_VISMSG_SEQNO_MISMATCHED;
            }
            assert(0);
            *wi = ri;
        } /* if (dst->hdr.message_seqno != msg_rx_w_seqno) */
#ifdef ASIL_COMPLIANCE
        else { /* if (dst->hdr.message_seqno == msg_rx_w_seqno) */
            uint32_t hash = 0x811C9DC5U;
            hash = amalgam_schdrmsg_calc_hash(&dst->hdr, 12U, hash);
            hash = amalgam_schdrmsg_calc_hash(&dst->msg, sizeof(dst->msg), hash);
            if (dst->hdr.message_checksum != hash) {
                printf("[ASIL][ERROR] : Incoming message mismatched csum (0x%08x, expected 0x%08x) type=%04x\n", dst->hdr.message_checksum, hash, dst->hdr.message_type);
                if ((dst->hdr.message_id >= (uint16_t)0x100U) && (dst->hdr.message_id < (uint16_t)(0x0100U + FLEXIDAG_MAX_SLOTS))) {
                    retcode = ERR_DRV_FLEXIDAG_VISMSG_CHECKSUM_MISMATCHED;
                } else {
                    retcode = ERR_DRV_SCHDR_VISMSG_CHECKSUM_MISMATCHED;
                }
                assert(0);
                *wi = ri;
            } /* if (dst->hdr.message_checksum != hash) */
            else { /* if (dst->hdr.message_checksum == hash) */
                // Transfer out one at a time, with checksum validation
                msg_rx_w_seqno++;
                *wi = ri+1;
            } /* if (dst->hdr.message_checksum == hash) */
        } /* if (dst->hdr.message_seqno == msg_rx_w_seqno) */
#else /* !ASIL_COMPLIANCE */
        else { /* if (dst->hdr.message_seqno == msg_rx_w_seqno) */
            // Transfer out one at a time, with checksum validation
            msg_rx_w_seqno++;
            *wi = ri+1;
        } /* if (dst->hdr.message_seqno == msg_rx_w_seqno) */
#endif /* ?ASIL_COMPLIANCE */
    } // check block

    return retcode;
}

uint32_t schdrmsg_send_msg(armvis_msg_t *msg, uint32_t size)
{
    uint32_t wi, ri;
    armvis_msg_t *dst;

    tx_lock();

    msg->hdr.message_type |= MSGTYPEBIT_ARM_TO_VISORC;
    msg->hdr.message_seqno    = msg_tx_w_seqno;
    msg_tx_w_seqno++;
#ifdef ASIL_COMPLIANCE
    wi = amalgam_schdrmsg_calc_hash(&msg->hdr, 12U, 0x811C9DC5U);
    msg->hdr.message_checksum = amalgam_schdrmsg_calc_hash(&msg->msg, sizeof(msg->msg), wi);
#endif /* ?ASIL_COMPLIANCE */

    ambacv_cache_invalidate((char*)qconfig.from_arm_r, sizeof(int32_t));
    ambacv_cache_invalidate((char*)qconfig.from_arm_w, sizeof(int32_t));
    ambacv_cache_barrier();

    ri = *qconfig.from_arm_r;
    wi = *qconfig.from_arm_w;

    while (wi - ri >= ARMVIS_MESSAGE_NUM) {
        printf("arm->orc message full %d %d\n", ri, wi);
        usleep(10000);
        ambacv_cache_invalidate((char*)qconfig.from_arm_r, sizeof(int32_t));
        ambacv_cache_barrier();
        ri = *qconfig.from_arm_r;
    }
    dst = &(qconfig.from_arm_q[wi & MSG_INDEX_MASK]);

    memcpy(dst, msg, sizeof(*msg));

    cache_clean_schdrmsg(dst);

    *qconfig.from_arm_w = ++wi;

    ambacv_cache_clean(qconfig.from_arm_w, sizeof(uint32_t));
    // trigger ORC to receive message
    dag_send_sync_count(ORC_SYNC_COUNTER);
    tx_unlock();

    return 0;
}

uint32_t schdr_stop_visorc(void)
{
    ambacv_state = 0U;
    return 0;
}

uint32_t schdrmsg_config_idsp(uint64_t vp_addr, uint64_t async_addr)
{
    // we can't do p2v conversion there because mmap is not finalized yet
    // so we just keep a copy of physicall address here
    vp_qcb_daddr = vp_addr;
    async_q_daddr = async_addr;
    return 0;
}

uint32_t schdrmsg_config_vin(uint64_t vin_addr)
{
    // we can't do p2v conversion there because mmap is not finalized yet
    // so we just keep a copy of physicall address here
    vin_qcb_daddr = vin_addr;
    return 0;
}

uint32_t schdrmsg_get_orcarm_wi(uint32_t *ri)
{
    *ri = qconfig.from_orc_wi;
    return 0U;
}

uint32_t cvtask_get_timestamp(void)
{
    static uint32_t tv_base;
    struct timeval tv;

    if (tv_base == 0) {
        /* set time base as start of the day */
        gettimeofday(&tv, NULL);
        tv_base = (tv.tv_sec / (24*3600)) * (24*3600);
    }

    gettimeofday(&tv, NULL);
    return (tv.tv_sec - tv_base) * 1000 + (tv.tv_usec>>10);
}

uint32_t schdr_get_event_time(void)
{

    return cvtask_get_timestamp();
}

uint32_t schdrmsg_send_vpmsg(const idsp_vis_picinfo_t *msg)
{
    static idsp_vis_picinfo_t *queue;
    idsp_vis_picinfo_t *dst;

    pthread_mutex_lock(&vp_mutex);

    if (vp_qcb == NULL) {
        vp_qcb = (dsp_msg_q_info_t*)ambacv_p2v(vp_qcb_daddr);
        ambacv_cache_invalidate(vp_qcb, sizeof(*vp_qcb));
        ambacv_cache_barrier();
        queue = (idsp_vis_picinfo_t*)ambacv_p2v(vp_qcb->base_daddr);
    }

    dst = &queue[vp_qcb->write_index];
    memcpy(dst, msg, sizeof(*dst));
    ambacv_cache_clean(dst, sizeof(*dst));

    // update write index
    if (++vp_qcb->write_index == vp_qcb->max_num_msg) {
        vp_qcb->write_index = 0;
    }
    ambacv_cache_clean(&(vp_qcb->write_index), sizeof(uint32_t));

    pthread_mutex_unlock(&vp_mutex);

    amalgam_send_idspmsg_sync(ORC_SYNC_COUNTER);  // Send Arm-VPQUEUE sync message

    return 0;
}

uint32_t schdrmsg_send_vinmsg(const void *msg)
{
    static msg_vin_sensor_raw_capture_status_t *vin_queue;
    idsp_vis_picinfo_t *dst;

    pthread_mutex_lock(&vin_mutex);

    if (vin_qcb == NULL) {
        vin_qcb = (vin_msg_q_info_t *)ambacv_p2v(vin_qcb_daddr);
        ambacv_cache_invalidate(vin_qcb, sizeof(*vin_qcb));
        ambacv_cache_barrier();
        vin_queue = (msg_vin_sensor_raw_capture_status_t*)ambacv_p2v(vin_qcb->base_daddr);
    }

    dst = &vin_queue[vin_qcb->write_index];
    memcpy(dst, msg, sizeof(*dst));
    ambacv_cache_clean(dst, sizeof(*dst));

    // update write index
    if (++vin_qcb->write_index == vin_qcb->max_num_msg) {
        vin_qcb->write_index = 0;
    }
    ambacv_cache_clean(&(vin_qcb->write_index), sizeof(uint32_t));

    pthread_mutex_unlock(&vin_mutex);

    amalgam_send_vinmsg_sync(ORC_SYNC_COUNTER);  // Send Arm-VIN queue sync message

    return 0;
}

/*-= TSR Scheduler related : Not supported in PACE mode =---------------------*/
uint32_t drv_schdr_resume_visorc(visorc_init_params_t *sysconfig)
{
    printf(">>> [WARNING] <<< TSR ARM scheduler not supported in PACE mode; restarting kernel\n");
    return drv_schdr_start_visorc(sysconfig);
} /* drv_schdr_resume_visorc() */

uint32_t schdr_get_ambacv_state(uint32_t *pState)
{
    if (pState == NULL) {
        printf(">>> [WARNING] <<< : schdr_get_ambacv_state() : Invalid pState in (%p)\n", pState);
    } /* if (pState == NULL) */
    else { /* if (pState != NULL) */
        *pState = ambacv_state;
    } /* if (pState != NULL) */
    return 0;
} /* schdr_get_ambacv_state() */

void hotlink_system_init(void)
{
    ;
}

uint32_t schdr_get_cluster_id(void)
{
    return 0;
}

uint32_t schdr_core_mutex(uint32_t mode)
{
    return 0;
}

int32_t drv_hotlink_process_reply(armvis_msg_t *msg)
{
    return 0;
}

void drv_handle_softreset(const armvis_msg_t *pMsg)
{
    schedmsg_softreset_t *req = &pMsg->msg.hw_reset;
    schedmsg_softreset_t *rpl = &pMsg->msg.reply_hw_reset;

    printf("[AMALGAM] auto-replying for soft reset %3d message", req->hardware_unit);

    schdrmsg_send_msg(pMsg, sizeof(*rpl));
}

scheduler_trace_t  *drv_schdr_get_pSchedulerTrace(void)
{
    printf("[AMALGAM] Unsupported call to drv_schdr_get_pSchedulerTrace; returning NULL\n");
    return NULL;
} /* drv_schdr_get_pSchedulerTrace() */

autorun_trace_t  *drv_schdr_get_pAutoRunTrace(void)
{
    printf("[AMALGAM] Unsupported call to drv_schdr_get_pAutoRunTrace; returning NULL\n");
    return NULL;
} /* drv_schdr_get_pAutoRunTrace() */

void drv_schdr_set_pSchedulerTrace(uint64_t trace_daddr)
{
    /* do nothing, not supported */
} /* drv_schdr_set_pSchedulerTrace() */

void drv_schdr_set_pAutoRunTrace(uint64_t trace_daddr)
{
    /* do nothing, not supported */
} /* drv_schdr_set_pAutoRunTrace() */

uint32_t cv_pa_start,cv_pa_end;
uint32_t cv_ca_start,cv_ca_end;

uint32_t ambacv_att_init(void)
{
    printf("[AMALGAM] ATT not supported; using default values\n");
    cv_pa_start = 0x00000000U;
    cv_pa_end   = 0xFFFFFFFFU;
    cv_ca_start = 0x00000000U;
    cv_ca_end   = 0xFFFFFFFFU;
    printf("  ATT PA region : [0x%08x -- 0x%08x]\n", cv_pa_start, cv_pa_end);
    printf("  ATT CA region : [0x%08x -- 0x%08x]\n", cv_ca_start, cv_ca_end);
    return 0U;
}

