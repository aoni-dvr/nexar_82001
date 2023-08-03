/**
 *  @file dsp_osal.c
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
 *  @details DSP OS abstract layer APIs
 *
 */

#include "dsp_osal.h"
#include "AmbaDSP_Def.h"
#include "AmbaDSP_Priv.h"
#include "ambadsp_ioctl.h"
#include "AmbaDSP_EventCtrl.h"
#if defined (CONFIG_BUILD_MAL)
#include "AmbaMAL.h"
#endif

struct device_node *dsp_dev_node = NULL;

typedef struct {
    uint32_t DspCode[DSP_MAX_IRQ_CODE_NUM];
    uint32_t VinSof[DSP_MAX_IRQ_VIN_NUM];
    uint32_t VinEof[DSP_MAX_IRQ_VIN_NUM];
    uint32_t Vout[DSP_MAX_IRQ_VOUT_NUM];
} osal_irq_id_t;
static osal_irq_id_t osal_irq_id = {0};

typedef struct {
    ULONG       Base;       /* Virtual Address */
    ULONG       PhysBase;   /* Physical Address */
    uint32_t    size;
    uint32_t    UsedSize;
    uint32_t    is_cached;
} osal_buf_pool_t;

typedef struct {
    ULONG       Base;     /* Virtual Address */
    uint32_t    size;
    uint32_t    pool_id;
} osal_buf_t;

static osal_buf_pool_t osal_buf_pool[DSP_BUF_POOL_NUM] = {0};

#if defined (CONFIG_BUILD_MAL)
#define DSP_AUX_BUF_POOL_NUM    (3U)
const UINT32 osal_aux_buf_mal_id_tbl[DSP_AUX_BUF_POOL_NUM] = {
    [0U] = AMBA_MAL_ID_CV_SCHDR,
    [1U] = AMBA_MAL_ID_CV_ARMINT,
    [2U] = AMBA_MAL_ID_CV_SDG_IDSP,
};

static osal_buf_pool_t osal_aux_buf_pool[DSP_AUX_BUF_POOL_NUM] = {0};
#endif

static inline ULONG ADDR_ALIGN_NUM(ULONG Addr, UINT32 Num)
{
    return (Addr + ((Num - (Addr & ((ULONG)Num-1U))) % Num));
}

/******************************************************************************/
/*                                  print                                 */
/******************************************************************************/
UINT32 dsp_osal_get_u64msb(UINT64 U64Val)
{
    return (UINT32)((U64Val >> 32U) & 0xFFFFFFFFU);
}

UINT32 dsp_osal_get_u64lsb(UINT64 U64Val)
{
    return (UINT32)(U64Val & 0xFFFFFFFFU);
}

void dsp_osal_print_flush(void)
{
//    OSAL_PrintFlush();
    // DO NOTHING
}

void dsp_osal_print_stop_and_flush(void)
{
//    OSAL_PrintStopAndFlush();
    // DO NOTHING
}

void dsp_osal_printU5(const char *fmt,
                      uint32_t argv0,
                      uint32_t argv1,
                      uint32_t argv2,
                      uint32_t argv3,
                      uint32_t argv4)
{
    OSAL_PrintUInt5(fmt, argv0, argv1, argv2, argv3, argv4);
}

void dsp_osal_printS5(const char *fmt,
                      const char *argv0,
                      const char *argv1,
                      const char *argv2,
                      const char *argv3,
                      const char *argv4)
{
    OSAL_PrintStr5(fmt, argv0, argv1, argv2, argv3, argv4);
}

void dsp_osal_module_printU5(uint32_t module_id, const char *fmt,
                             uint32_t argv0, uint32_t argv1,
                             uint32_t argv2, uint32_t argv3,
                             uint32_t argv4)
{
    OSAL_ModulePrintUInt5(module_id, fmt, argv0, argv1, argv2, argv3, argv4);
}

void dsp_osal_module_printS5(uint32_t module_id, const char *fmt,
                             const char *argv0, const char *argv1,
                             const char *argv2, const char *argv3,
                             const char *argv4)
{
    OSAL_ModulePrintStr5(module_id, fmt, argv0, argv1, argv2, argv3, argv4);
}

void dsp_osal_module_printI5(uint32_t module_id, const char *fmt,
                             int32_t argv0, int32_t argv1,
                             int32_t argv2, int32_t argv3,
                             int32_t argv4)
{
    OSAL_ModulePrintInt5(module_id, fmt, argv0, argv1, argv2, argv3, argv4);
}

void dsp_osal_print_err_line(uint32_t rval, const char *fn, uint32_t line_num)
{
    if (rval != 0U) {
        dsp_osal_printS5("%s got NG result", fn, NULL, NULL, NULL, NULL);
        dsp_osal_printU5("    Rval = 0x%x @ %d", rval, line_num, 0U, 0U, 0U);
    }
}

/******************************************************************************/
/*                                  OS relative                               */
/******************************************************************************/
uint32_t dsp_osal_sem_init(osal_sem_t *sem, char *pName, int32_t pshared, uint32_t value)
{
(void)pshared;
    return OSAL_SemaphoreCreate(sem, pName, value);
}

uint32_t dsp_osal_sem_deinit(osal_sem_t *sem)
{
    return OSAL_SemaphoreDelete(sem);
}

uint32_t dsp_osal_sem_post(osal_sem_t *sem)
{
    return OSAL_SemaphoreGive(sem);
}

uint32_t dsp_osal_sem_wait(osal_sem_t *sem, uint32_t timeout)
{
    return OSAL_SemaphoreTake(sem, timeout);
}

uint32_t dsp_osal_mutex_init(osal_mutex_t *mutex, char *pName)
{
    return OSAL_MutexCreate(mutex, pName);
}

uint32_t dsp_osal_mutex_deinit(osal_mutex_t *mutex)
{
    return OSAL_MutexDelete(mutex);
}

uint32_t dsp_osal_mutex_lock(osal_mutex_t *mutex, uint32_t timeout)
{
    return OSAL_MutexTake(mutex, timeout);
}

uint32_t dsp_osal_mutex_unlock(osal_mutex_t *mutex)
{
    return OSAL_MutexGive(mutex);
}

uint32_t dsp_osal_eventflag_init(osal_even_t *event, char *pName)
{
    AMBA_KAL_EVENT_FLAG_t *pEvnt = NULL;
(void)pEvnt;

    dsp_osal_typecast(&pEvnt, &event);
    return OSAL_EventFlagCreate(pEvnt, pName);
}

uint32_t dsp_osal_eventflag_set(osal_even_t *event, uint32_t flag)
{
    AMBA_KAL_EVENT_FLAG_t *pEvnt = NULL;
(void)pEvnt;

    dsp_osal_typecast(&pEvnt, &event);
    return OSAL_EventFlagSet(pEvnt, flag);
}

uint32_t dsp_osal_eventflag_get(osal_even_t *event, uint32_t reqflag, uint32_t all, uint32_t clear, uint32_t *actflag, uint32_t timeout)
{
    AMBA_KAL_EVENT_FLAG_t *pEvnt = NULL;
(void)pEvnt;

    dsp_osal_typecast(&pEvnt, &event);
    return OSAL_EventFlagGet(pEvnt, reqflag, all, clear, actflag, timeout);
}

uint32_t dsp_osal_eventflag_clear(osal_even_t *event, uint32_t flag)
{
    AMBA_KAL_EVENT_FLAG_t *pEvnt = NULL;
(void)pEvnt;

    dsp_osal_typecast(&pEvnt, &event);
    return OSAL_EventFlagClear(pEvnt, flag);
}

static DEFINE_SPINLOCK(dsp_spin_lock);
uint32_t dsp_osal_spinlock(osal_spinlock_t *pSpinlock)
{
    uint32_t retcode = DSP_ERR_NONE;

    spin_lock_irqsave(&dsp_spin_lock, pSpinlock->spin_lock_flag);

    return retcode;
}

uint32_t dsp_osal_spinunlock(osal_spinlock_t *pSpinlock)
{
    uint32_t retcode = DSP_ERR_NONE;

    spin_unlock_irqrestore(&dsp_spin_lock, pSpinlock->spin_lock_flag);

    return retcode;
}

//#define USE_CMD_MTX
uint32_t dsp_osal_cmdlock(osal_mutex_t *mutex, osal_spinlock_t *pSpinlock)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t retcode = 0U;

#ifdef USE_CMD_MTX
    retcode = dsp_osal_mutex_lock(mutex, DSP_CMD_LOCK_TIMEOUT);
    if (retcode != DSP_ERR_NONE) {
        dsp_osal_printU5("[ERR] dsp_osal_cmdlock : mtx lock fail[0x%X]", retcode, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
#else
    {
        (void)mutex;
#endif
        retcode = dsp_osal_spinlock(pSpinlock);
        if (retcode != DSP_ERR_NONE) {
            dsp_osal_printU5("[ERR] dsp_osal_cmdlock : spinlock lock fail[0x%X]", retcode, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
        }
    }
    return Rval;
}

uint32_t dsp_osal_cmdunlock(osal_mutex_t *mutex, const osal_spinlock_t *pSpinlock)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t retcode = 0U;

    retcode = dsp_osal_spinunlock(pSpinlock);
    if (retcode != DSP_ERR_NONE) {
        dsp_osal_printU5("[ERR] dsp_osal_cmdunlock : spinunlock unlock fail[0x%X]", retcode, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    }
#ifdef USE_CMD_MTX
    else {
        retcode = dsp_osal_mutex_unlock(mutex);
        if (retcode != DSP_ERR_NONE) {
            dsp_osal_printU5("[ERR] dsp_osal_cmdunlock : mtx unlock fail[0x%X]", retcode, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
        }
    }
#else
    (void)mutex;
#endif
    return Rval;
}

uint32_t dsp_osal_mq_create(osal_mq_t *mq, char *name, uint32_t msg_size, void *msg_q_buf, uint32_t msg_q_buf_size)
{
#ifdef SUPPORT_OSAL_MQ
    return OSAL_MsgQueueCreate(mq, name, msg_size, msg_q_buf, msg_q_buf_size);
#else
    static char dsp_mq_mtx_name[] = "DspMqMtx";
    static char dsp_mq_flg_name[] = "DspMqFlg";

    uint32_t Rval = DSP_ERR_NONE;
    uint32_t msg_num;

    if ((mq == NULL) ||
        (name == NULL) ||
        (msg_q_buf == NULL)) {
        dsp_osal_printU5("[ERR] dsp_osal_mq_create : Null mq, name or msg_q_buf", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else if ((msg_size == 0U) ||
               (msg_q_buf_size == 0U)) {
        dsp_osal_printU5("[ERR] dsp_osal_mq_create : Zero msg_size or msg_q_buf_size", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
        msg_num = msg_q_buf_size/msg_size;

        if ((msg_num == 0U) ||
            (msg_num > DSP_MQ_MAX_NUM)) {
            dsp_osal_printU5("[ERR] dsp_osal_mq_create : Invalid msg_num[%d]", msg_num, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    }

    /* Mtx */
    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_mutex_init(&mq->mtx, dsp_mq_mtx_name);
        if (Rval != DSP_ERR_NONE) {
            dsp_osal_printU5("[ERR] dsp_osal_mq_create : mutex init fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
        }
    }

    /* Evnt */
    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_eventflag_init(&mq->event, dsp_mq_flg_name);
        if (Rval != DSP_ERR_NONE) {
            dsp_osal_printU5("[ERR] dsp_osal_mq_create : flag init fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
        }
    }

    /* Init */
    if (Rval == DSP_ERR_NONE) {
        mq->wp = msg_num - 1U;
        mq->rp = msg_num - 1U;
        mq->is_empty = 1U;
        mq->msg_unit_size = msg_size;
        mq->msg_num = msg_num;
        mq->mq_buf = msg_q_buf;
    }

    return Rval;
#endif
}

uint32_t dsp_osal_mq_send(osal_mq_t *mq, void *msg, uint32_t time_out)
{
#ifdef SUPPORT_OSAL_MQ
    return OSAL_MsgQueueSend(mq, msg, time_out);
#else
    uint32_t Rval = DSP_ERR_NONE;
    uint16_t wp, rp;
    uint32_t offset;
    UINT8 *pU8;
    uint32_t req_flg = DSP_MQ_FLG_MSG_CONSUMED;
    uint32_t ack_flg = DSP_MQ_FLG_MSG_ARRIVED;
    uint32_t act_flg = 0U;

    if ((mq == NULL) ||
        (msg == NULL)) {
        dsp_osal_printU5("[ERR] dsp_osal_mq_send : Null mq or msg", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
        // DO NOTHING
    }

    if (Rval == DSP_ERR_NONE) {
        /* Get WP/RP */
        Rval = dsp_osal_mutex_lock(&mq->mtx, DSP_MQ_MTX_TIMEOUT);
        if (Rval != DSP_ERR_NONE) {
            dsp_osal_printU5("[ERR] dsp_osal_mq_send : mtx lock before cpy fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
        } else {
            wp = mq->wp;
            wp++;
            wp %= mq->msg_num;
            rp = mq->rp;
        }
        (void)dsp_osal_mutex_unlock(&mq->mtx);

        if (wp == rp) {
            /* Wait */
            Rval = dsp_osal_eventflag_get(&mq->event, req_flg, 1U/*all*/, 1U/*Clear*/, &act_flg, time_out);
            if (Rval != DSP_ERR_NONE) {
                dsp_osal_printU5("[ERR] dsp_osal_mq_send : eventflag_get[0x%X 0x%X] fail[0x%X]", req_flg, act_flg, Rval, 0U, 0U);
                Rval = DSP_ERR_0000;
            }
        }

        if (Rval == DSP_ERR_NONE) {
            /* copy msg and update wp */
            offset = mq->msg_unit_size*wp;
            dsp_osal_typecast(&pU8, &mq->mq_buf);
            (void)dsp_osal_memcpy(&pU8[offset], msg, mq->msg_unit_size);

            Rval = dsp_osal_mutex_lock(&mq->mtx, DSP_MQ_MTX_TIMEOUT);
            if (Rval != DSP_ERR_NONE) {
                dsp_osal_printU5("[ERR] dsp_osal_mq_send : mtx lock after cpy fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0000;
            } else {
                mq->wp = wp;
                mq->is_empty = 0U;
            }
            (void)dsp_osal_mutex_unlock(&mq->mtx);

            /* send flag */
            Rval = dsp_osal_eventflag_set(&mq->event, ack_flg);
            if (Rval != DSP_ERR_NONE) {
                dsp_osal_printU5("[ERR] dsp_osal_mq_send : eventflag_set fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0000;
            }
        }
    }

    return Rval;
#endif
}

uint32_t dsp_osal_mq_recv(osal_mq_t *mq, void *msg, uint32_t time_out)
{
#ifdef SUPPORT_OSAL_MQ
    return OSAL_MsgQueueReceive(mq, msg, time_out);
#else
    uint32_t Rval = DSP_ERR_NONE;
    uint16_t wp, rp;
    uint32_t is_empty;
    uint32_t offset;
    UINT8 *pU8;
    uint32_t req_flg = DSP_MQ_FLG_MSG_ARRIVED;
    uint32_t ack_flg = DSP_MQ_FLG_MSG_CONSUMED;
    uint32_t act_flg = 0U;

    if ((mq == NULL) ||
        (msg == NULL)) {
        dsp_osal_printU5("[ERR] dsp_osal_mq_recv : Null mq or msg", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
        // DO NOTHING
    }

    if (Rval == DSP_ERR_NONE) {
        /* Get WP/RP */
        Rval = dsp_osal_mutex_lock(&mq->mtx, DSP_MQ_MTX_TIMEOUT);
        if (Rval != DSP_ERR_NONE) {
            dsp_osal_printU5("[ERR] dsp_osal_mq_recv : mtx lock before cpy fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
        } else {
            rp = mq->rp;
            rp++;
            rp %= mq->msg_num;
            wp = mq->wp;
            is_empty = mq->is_empty;
        }
        (void)dsp_osal_mutex_unlock(&mq->mtx);

        if (is_empty == 1U) {
            /* Wait */
            Rval = dsp_osal_eventflag_get(&mq->event, &req_flg, 1U/*all*/, 1U/*Clear*/, &act_flg, time_out);
            if (Rval != DSP_ERR_NONE) {
                dsp_osal_printU5("[ERR] dsp_osal_mq_recv : eventflag_get[0x%X 0x%X] fail[0x%X]", req_flg, act_flg, Rval, 0U, 0U);
                Rval = DSP_ERR_0000;
            }
        }

        if (Rval == DSP_ERR_NONE) {
            /* copy msg and update rp */
            offset = mq->msg_unit_size*rp;
            dsp_osal_typecast(&pU8, &mq->mq_buf);
            (void)dsp_osal_memcpy(msg, &pU8[offset], mq->msg_unit_size);

            Rval = dsp_osal_mutex_lock(&mq->mtx, DSP_MQ_MTX_TIMEOUT);
            if (Rval != DSP_ERR_NONE) {
                dsp_osal_printU5("[ERR] dsp_osal_mq_recv : mtx lock after cpy fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0000;
            } else {
                mq->rp = rp;
                if (mq->wp == mq->rp) {
                    mq->is_empty = 1U;
                }
            }
            (void)dsp_osal_mutex_unlock(&mq->mtx);

            /* send flag */
            Rval = dsp_osal_eventflag_set(&mq->event, ack_flg);
            if (Rval != DSP_ERR_NONE) {
                dsp_osal_printU5("[ERR] dsp_osal_mq_recv : eventflag_set fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0000;
            }
        }
    }

    return Rval;
#endif
}

uint32_t dsp_osal_mq_query(osal_mq_t *mq, osal_mq_info_t *mq_info)
{
    uint32_t Rval = DSP_ERR_NONE;
#ifdef SUPPORT_OSAL_MQ
    AMBA_KAL_MSG_QUEUE_INFO_s MsgQInfo = {0};

    Rval = OSAL_MsgQueueQuery(mq, &MsgQInfo);
    mq_info->PendingNum = MsgQInfo.NumEnqueued;
    mq_info->FreeNum = MsgQInfo.NumAvailable;
#else

    if ((mq == NULL) ||
        (mq_info == NULL)) {
        dsp_osal_printU5("[ERR] dsp_osal_mq_query : Null mq or mq_info", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
        Rval = dsp_osal_mutex_lock(&mq->mtx, DSP_MQ_MTX_TIMEOUT);
        if (Rval != DSP_ERR_NONE) {
            dsp_osal_printU5("[ERR] dsp_osal_mq_query : mtx lock fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
        } else {
            if (mq->is_empty == 1U) {
                mq_info->PendingNum = 0U;
            } else {
                if (mq->wp > mq->rp) {
                    mq_info->PendingNum = mq->wp - mq->rp;
                } else {
                    mq_info->PendingNum = (mq->msg_num - mq->rp) + mq->wp;
                }
            }
            mq_info->FreeNum = mq->msg_num - mq_info->PendingNum;
        }
        (void)dsp_osal_mutex_unlock(&mq->mtx);
    }
#endif
    return Rval;
}

uint32_t dsp_osal_sleep(uint32_t msec)
{
    return OSAL_TaskSleep(msec);
}

uint32_t dsp_osal_get_sys_tick(uint32_t *msec)
{
    uint32_t retcode = DSP_ERR_NONE;

//    dsp_osal_printU5("[ERR] dsp_osal_get_sys_tick : n/a", 0U, 0U, 0U, 0U, 0U);

    return retcode;
}

uint32_t dsp_osal_cache_size_align(uint32_t size)
{
    return ((size) + AMBA_CACHE_LINE_SIZE - 1U) & ~(AMBA_CACHE_LINE_SIZE - 1U);
}

uint32_t dsp_osal_cache_addr_align(uint32_t addr)
{
    return addr & ~(AMBA_CACHE_LINE_SIZE - 1U);
}

uint32_t dsp_osal_cache_clean(const void *ptr, uint32_t size)
{
    uint32_t Rval = DSP_ERR_NONE;
    ULONG ULAddr;
    if (size != 0U) {
        if (ptr == NULL) {
            Rval = DSP_ERR_0003;
            dsp_osal_printU5("[ERR] dsp_osal_cache_clean : fail : ptr == NULL ", 0U, 0U, 0U, 0U, 0U);
        } else {
            dsp_osal_typecast(&ULAddr, &ptr);
            Rval = OSAL_Cache_DataClean(ULAddr, size);
        }
    }

    return Rval;
}

uint32_t dsp_osal_cache_invalidate(const void *ptr, uint32_t size)
{
    uint32_t Rval = DSP_ERR_NONE;
    ULONG ULAddr;
    if (size != 0U) {
        if (ptr == NULL) {
            Rval = DSP_ERR_0003;
            dsp_osal_printU5("[ERR] dsp_osal_cache_invalidate : fail : ptr == NULL ", 0U, 0U, 0U, 0U, 0U);
        } else {
            dsp_osal_typecast(&ULAddr, &ptr);
            Rval = OSAL_Cache_DataInvalidate(ULAddr, size);
        }
    }

    return Rval;
}

uint32_t dsp_osal_is_cached_addr(const ULONG Virt, uint32_t *pCached)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t i, exit_i_loop;
    ULONG ul_pool_end_addr;

    if (pCached == NULL) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_is_cached_addr : Null addr", 0U, 0U, 0U, 0U, 0U);
    } else if (Virt == 0UL) {
        *pCached = 1U;
    } else {
        exit_i_loop = 0U;
        for (i = 0U; i < DSP_BUF_POOL_NUM; i++) {
            if (osal_buf_pool[i].size > 0U) {
                ul_pool_end_addr = (osal_buf_pool[i].Base + osal_buf_pool[i].size) - 1U;

                if ((Virt >= osal_buf_pool[i].Base) &&
                    (Virt <= ul_pool_end_addr)) {
                    *pCached = osal_buf_pool[i].is_cached;
                    exit_i_loop = 1U;
                    break;
                }
            }
        }

        if (exit_i_loop == 0U) {
            dsp_osal_printU5("[ERR] dsp_osal_is_cached_addr : fail for 0x%X%X", dsp_osal_get_u64msb((UINT64)Virt), dsp_osal_get_u64lsb((UINT64)Virt), 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        }

    }
    return Rval;
}

uint32_t dsp_osal_virt2phys(const ULONG Virt, ULONG *pPhys)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t i, exit_i_loop;
    ULONG ul_pool_end_addr, offset;

    if (pPhys == NULL) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_virt2phys : Null addr", 0U, 0U, 0U, 0U, 0U);
    } else if (Virt == 0UL) {
        *pPhys = 0U;
    } else {
        exit_i_loop = 0U;
        for (i = 0U; i < DSP_BUF_POOL_NUM; i++) {
            if (osal_buf_pool[i].size > 0U) {
                ul_pool_end_addr = (osal_buf_pool[i].Base + osal_buf_pool[i].size) - 1U;

                if ((Virt >= osal_buf_pool[i].Base) &&
                    (Virt <= ul_pool_end_addr)) {
                    offset = (Virt - osal_buf_pool[i].Base);
                    *pPhys = osal_buf_pool[i].PhysBase + offset;
                    exit_i_loop = 1U;
                    break;
                }
            }
        }

#if defined (CONFIG_BUILD_MAL)
        if (exit_i_loop == 0U) {
            for (i = 0U; i < DSP_AUX_BUF_POOL_NUM; i++) {
                if (osal_aux_buf_pool[i].size > 0U) {
                    ul_pool_end_addr = (osal_aux_buf_pool[i].Base + osal_aux_buf_pool[i].size) - 1U;

                    if ((Virt >= osal_aux_buf_pool[i].Base) &&
                        (Virt <= ul_pool_end_addr)) {
                        offset = (Virt - osal_aux_buf_pool[i].Base);
                        *pPhys = osal_aux_buf_pool[i].PhysBase + offset;
                        exit_i_loop = 1U;
                        break;
                    }
                }
            }
        }
#endif

        if (exit_i_loop == 0U) {
            dsp_osal_printU5("[ERR] dsp_osal_virt2phys : fail for 0x%X%X", dsp_osal_get_u64msb((UINT64)Virt), dsp_osal_get_u64lsb((UINT64)Virt), 0U, 0U, 0U);
            *pPhys = Virt;
            Rval = DSP_ERR_0003;
        }
    }
    return Rval;
}

uint32_t dsp_osal_phys2virt(const ULONG Phys, ULONG *pVirt)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t i, exit_i_loop;
    ULONG pool_end_addr, offset;

    if (pVirt == NULL) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_phys2virt : Null addr", 0U, 0U, 0U, 0U, 0U);
    } else if ((Phys == 0U) ||
               (Phys == 0xFFFFFFFFFFFFFFFFUL)) {
        *pVirt = 0U;
    } else {
        exit_i_loop = 0U;
        for (i = 0U; i < DSP_BUF_POOL_NUM; i++) {
            if (osal_buf_pool[i].size > 0U) {
                pool_end_addr = (osal_buf_pool[i].PhysBase + osal_buf_pool[i].size) - 1U;
                if ((Phys >= osal_buf_pool[i].PhysBase) &&
                    (Phys <= pool_end_addr)) {
                    offset = Phys - osal_buf_pool[i].PhysBase;
                    *pVirt = osal_buf_pool[i].Base + offset;
                    exit_i_loop = 1U;
                    break;
                }
            }
        }

#if defined (CONFIG_BUILD_MAL)
        if (exit_i_loop == 0U) {
            for (i = 0U; i < DSP_AUX_BUF_POOL_NUM; i++) {
                if (osal_aux_buf_pool[i].size > 0U) {
                    pool_end_addr = (osal_aux_buf_pool[i].PhysBase + osal_aux_buf_pool[i].size) - 1U;
                    if ((Phys >= osal_aux_buf_pool[i].PhysBase) &&
                        (Phys <= pool_end_addr)) {
                        offset = Phys - osal_aux_buf_pool[i].PhysBase;
                        *pVirt = osal_aux_buf_pool[i].Base + offset;
                        exit_i_loop = 1U;
                        break;
                    }
                }
            }
        }
#endif

        if (exit_i_loop == 0U) {
            dsp_osal_printU5("[ERR] dsp_osal_phys2virt : fail for 0x%X \n", Phys, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        }

    }
    return Rval;

}

uint32_t dsp_osal_virt2cli(const ULONG Virt, uint32_t *pCli)
{
    UINT32 Rval = DSP_ERR_NONE;
    ULONG Phys;

    dsp_osal_virt2phys(Virt, &Phys);
    *pCli = (uint32_t)Phys;

    return Rval;
}

uint32_t dsp_osal_cli2virt(const uint32_t Cli, ULONG *pVirt)
{
    UINT32 Rval = DSP_ERR_NONE;
    ULONG Phys = (ULONG)Cli;

    dsp_osal_phys2virt(Phys, pVirt);

    return Rval;
}

uint32_t dsp_osal_copy_to_user(void *to, const void *from, uint32_t n)
{
    uint32_t Rval = DSP_ERR_NONE;

    if ((to == NULL) || (from == NULL)) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_copy_to_user : fail Rval = 0x%x", Rval, 0U, 0U, 0U, 0U);
    } else {
        Rval = copy_to_user(to, from, n);

    }
    return Rval;
}

uint32_t dsp_osal_copy_from_user(void *to, const void *from, uint32_t n)
{
    uint32_t Rval = DSP_ERR_NONE;

    if ((to == NULL) || (from == NULL)) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_copy_from_user : fail Rval = 0x%x", Rval, 0U, 0U, 0U, 0U);
    } else {
        Rval = copy_from_user(to, from, n);
    }
    return Rval;
}

uint32_t dsp_osal_readl_relaxed(const void *reg)
{
    uint32_t value = 0U;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    uint64_t phys_addr;
#else
    uint32_t phys_addr;
#endif
    void __iomem *virt;
    uint32_t size = 4U; //4byte

    if (reg == NULL) {
        dsp_osal_printU5("[ERR] dsp_osal_readl_relaxed : fail reg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        dsp_osal_typecast(&phys_addr, &reg);

        virt = ioremap(phys_addr, size);
        if (virt != NULL) {
            value = readl(virt);
            iounmap(virt);
        }
    }

    return value;
}

void dsp_osal_writel_relaxed(uint32_t val, const void *reg)
{
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    uint64_t phys_addr;
#else
    uint32_t phys_addr;
#endif
    void __iomem *virt;
    uint64_t virt_addr;
    uint32_t size = 4U; //4byte

    if (reg == NULL) {
        dsp_osal_printU5("[ERR] dsp_osal_writel_relaxed : fail reg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        dsp_osal_typecast(&phys_addr, &reg);

        virt = ioremap(phys_addr, size);
        if (virt != NULL) {
            dsp_osal_typecast(&virt_addr, &virt);
            writel_relaxed(val, virt);
            iounmap(virt);
        }
    }
}

void *dsp_osal_ioremap(uint32_t pa, uint32_t size)
{
    void *ptr = NULL;

    if ((pa == 0U) || (size == 0U)) {
        dsp_osal_printU5("[ERR] dsp_osal_ioremap : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        ptr = ioremap(pa, size);
    }
    return ptr;
}

void dsp_osal_iounmap(void *va, uint32_t size)
{
(void)size;
    iounmap(va);
}

static inline uint32_t bit_to_idx(uint32_t bit_mask, uint32_t *idx)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t i;

    if (idx == NULL) {
        Rval = DSP_ERR_0000;
    } else if (bit_mask == 0U) {
        Rval = DSP_ERR_0001;
    } else {
        for (i = 0U; i < 32U; i++) {
            if (((bit_mask >> i) & 0x1U) > 0U) {
                *idx = i;
                break;
            }
        }
    }

    return Rval;
}

uint32_t dsp_osal_irq_config(uint32_t irq_id, const osal_irq_t *irq_cfg, osal_isr_entry_f hdlr, uint32_t arg)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;
    uint32_t flag;
    uint32_t cpu_id = 0U;

    if (irq_cfg == NULL) {
        Rval = DSP_ERR_0003;
    } else {
        if(irq_cfg->trigger_type == AMBA_INT_RISING_EDGE_TRIGGER) {
            flag = IRQF_TRIGGER_RISING;
        } else {
            flag = IRQF_TRIGGER_HIGH;
        }

        retcode = request_irq(irq_id, hdlr, flag, NULL, NULL);
        if (retcode != 0) {
            dsp_osal_printU5("[ERR] dsp_osal_irq_config[%d] : request_irq fail[%d]", irq_id, retcode, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        } else {
            Rval = bit_to_idx(irq_cfg->cpu_targets, &cpu_id);
            if (Rval != DSP_ERR_NONE) {
                dsp_osal_printU5("[ERR] dsp_osal_irq_config[%d] : bit_to_idx fail[%d]", irq_id, Rval, 0U, 0U, 0U);
            } else {
                /* affinity */
                retcode = irq_set_affinity_hint(irq_id, get_cpu_mask(cpu_id));
                if (retcode != 0) {
                    dsp_osal_printU5("[ERR] dsp_osal_irq_config[%d] : set_affinity fail[%d]", irq_id, retcode, 0U, 0U, 0U);
                    Rval = DSP_ERR_0003;
                }
            }

        }
    }

    return Rval;
}

uint32_t dsp_osal_irq_enable(uint32_t irq_id)
{
    uint32_t retcode = DSP_ERR_NONE;

(void)irq_id;

    return retcode;
}

uint32_t dsp_osal_irq_disable(uint32_t irq_id)
{
    uint32_t retcode = DSP_ERR_NONE;

    free_irq(irq_id, NULL);

    return retcode;
}

uint32_t dsp_osal_irq_id_query(uint32_t type, uint32_t idx, uint32_t *irq_id)
{
    uint32_t Rval = DSP_ERR_NONE;

    if (type >= NUM_DSP_IRQ_TYPE) {
        dsp_osal_printU5("[ERR] dsp_osal_irq_id_query : Invalid type[%d]", type, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0001;
    } else if (irq_id == NULL) {
        dsp_osal_printU5("[ERR] dsp_osal_irq_id_query : Null input", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
        if (type == DSP_IRQ_TYPE_VIN_SOF) {
            if (idx >= DSP_MAX_IRQ_VIN_NUM) {
                dsp_osal_printU5("[ERR] dsp_osal_irq_id_query : [%d]Invalid idx[%d]", type, idx, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            } else {
                *irq_id = osal_irq_id.VinSof[idx];
            }
        } else if (type == DSP_IRQ_TYPE_VIN_EOF) {
            if (idx >= DSP_MAX_IRQ_VIN_NUM) {
                dsp_osal_printU5("[ERR] dsp_osal_irq_id_query : [%d]Invalid idx[%d]", type, idx, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            } else {
                *irq_id = osal_irq_id.VinEof[idx];
            }
        } else if (type == DSP_IRQ_TYPE_VOUT) {
            if (idx >= DSP_MAX_IRQ_VOUT_NUM) {
                dsp_osal_printU5("[ERR] dsp_osal_irq_id_query : [%d]Invalid idx[%d]", type, idx, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            } else {
                *irq_id = osal_irq_id.Vout[idx];
            }
        } else {
            if (idx >= DSP_MAX_IRQ_CODE_NUM) {
                dsp_osal_printU5("[ERR] dsp_osal_irq_id_query : [%d]Invalid idx[%d]", type, idx, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            } else {
                *irq_id = osal_irq_id.DspCode[idx];
            }
        }
    }

    return Rval;
}

uint32_t dsp_osal_cma_alloc(void *arg, uint64_t owner)
{
(void)arg;
(void)owner;
    dsp_osal_printU5("[ERR] dsp_osal_cma_alloc : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003;
}

uint32_t dsp_osal_cma_free(void *arg)
{
(void)arg;
    dsp_osal_printU5("[ERR] dsp_osal_cma_free : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003;
}

uint32_t dsp_osal_cma_sync_cache(void *arg)
{
(void) arg;
    dsp_osal_printU5("[ERR] dsp_osal_cma_sync_cache : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003;
}

uint32_t dsp_osal_cma_get_usage(void *arg)
{
(void)arg;
    dsp_osal_printU5("[ERR] dsp_osal_cma_get_usage : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003;
}

uint32_t dsp_osal_cma_p2v(uint32_t phy, uint32_t *pCacheFlag, ULONG *pAddr)
{
(void)phy;
(void)pCacheFlag;
(void)pAddr;
    dsp_osal_printU5("[ERR] dsp_osal_cma_p2v : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003;
}

uint32_t dsp_osal_cma_v2p(ULONG virt, uint32_t *pCacheFlag)
{
(void)virt;
(void)pCacheFlag;
    dsp_osal_printU5("[ERR] dsp_osal_cma_v2p : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003;
}

/******************************************************************************/
/*                                  DSP                                       */
/******************************************************************************/
#if defined (CONFIG_SOC_CV22)
#define DBG_PORT_CORC_BASE_ADDR         (0xED030000)
#define DBG_PORT_EORC_BASE_ADDR         (0xED020000)
#define DBG_PORT_DORC_BASE_ADDR         (0xED0C0000)
#define DBG_PORT_SYNCNT_BASE_ADDR       (0xED05F000)
#elif defined (CONFIG_SOC_CV2)
#define DBG_PORT_CORC_BASE_ADDR         (0xED030000)
#define DBG_PORT_EORC_BASE_ADDR         (0xED020000)
#define DBG_PORT_DORC_BASE_ADDR         (0xED0C0000)
#define DBG_PORT_SYNCNT_BASE_ADDR       (0xED05F000)
#elif defined (CONFIG_SOC_CV25)
#define DBG_PORT_CORC_BASE_ADDR         (0xED030000)
#define DBG_PORT_EORC_BASE_ADDR         (0xED020000)
#define DBG_PORT_DORC_BASE_ADDR         (0xED0C0000)
#define DBG_PORT_SYNCNT_BASE_ADDR       (0xED05F000)
#elif defined (CONFIG_SOC_H32)
#define DBG_PORT_CORC_BASE_ADDR         (0xED030000)
#define DBG_PORT_EORC_BASE_ADDR         (0xED020000)
#define DBG_PORT_DORC_BASE_ADDR         (0xED0C0000)
#define DBG_PORT_SYNCNT_BASE_ADDR       (0xED05F000)
#elif defined (CONFIG_SOC_CV28)
#define DBG_PORT_CORC_BASE_ADDR         (0xED030000)
#define DBG_PORT_EORC_BASE_ADDR         (0xED020000)
#define DBG_PORT_DORC_BASE_ADDR         (0xED0C0000)
#define DBG_PORT_SYNCNT_BASE_ADDR       (0xED05F000)
#elif defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define DBG_PORT_CORC_BASE_ADDR         (0x20ED140000)
#define DBG_PORT_CORC1_BASE_ADDR        (0x20ED150000)
#define DBG_PORT_CORC2_BASE_ADDR        (0x20ED160000)
#define DBG_PORT_CORC3_BASE_ADDR        (0x20ED170000)
#define DBG_PORT_EORC_BASE_ADDR         (0x20ED020000)
#define DBG_PORT_EORC1_BASE_ADDR        (0x20ED320000)
#define DBG_PORT_DORC_BASE_ADDR         (0x20ED030000)
#define DBG_PORT_DORC1_BASE_ADDR        (0x20ED330000)
#define DBG_PORT_SYNCNT_BASE_ADDR       (0x20ED05F000)
#else //CV2FS
#define DBG_PORT_CORC_BASE_ADDR         (0xED160000)
#define DBG_PORT_EORC_BASE_ADDR         (0xED020000)
#define DBG_PORT_DORC_BASE_ADDR         (0xED030000)
#define DBG_PORT_SYNCNT_BASE_ADDR       (0xED05F000)
#endif
#define DBG_PORT_ORC_RESET_OFFSET       (0)
#define DBG_PORT_ORC_PC_OFFSET          (8)
#define DBG_PORT_CORC_RESET_VALUE       (0xFF)
#define DBG_PORT_EORC_RESET_VALUE       (0xF)
#define DBG_PORT_DORC_RESET_VALUE       (0xF)
#define DBG_PORT_CORC_SHUTDOWN_VALUE    (0xFF00)
#define DBG_PORT_EORC_SHUTDOWN_VALUE    (0xF0)
#define DBG_PORT_DORC_SHUTDOWN_VALUE    (0xF0)
#define DBG_PORT_SYNCNT_SIZE            (0x4) //4byte
#define DBG_PORT_SYNCNT_NUM             (128)
#define DBG_PORT_ORC_TIMER_OFFSET       (0x44)
#define DBG_PORT_SMEM_GRP_BASE_ADDR     (0xED05E000)
#define DBG_PORT_SMEM_GRP_OFFSET        (0x4)
#define DBG_PORT_DRAM_PAGE_BASE_ADDR    (0xDFFE0800)
#define DBG_PORT_DRAM_PAGE_OFFSET       (0x4)
#define DBG_PORT_SMEM_INIT_BASE_ADDR    (0xED05E000)
#define DBG_PORT_SMEM_INIT_OFFSET       (16)

/* Init SMEM via Sydney Reader */
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
void dsp_osal_safety_init(void)
{
    void *pVoid;
    UINT32 SmemInitAddr = (DBG_PORT_SMEM_INIT_BASE_ADDR + DBG_PORT_SMEM_INIT_OFFSET);
    UINT32 SmemInitData = 0x808080U;

    dsp_osal_typecast(&pVoid, &SmemInitAddr);
    dsp_osal_writel_relaxed(SmemInitData, pVoid);
    dsp_osal_printU5("[DSP_INIT]SmemInit", 0U, 0U, 0U, 0U, 0U);
}
#endif

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
void dsp_osal_orc_set_pc(const osal_ucode_addr_t *pBasePhysAddr, const uint8_t IdspBinShareMode)
#else
void dsp_osal_orc_set_pc(const osal_ucode_addr_t *pBasePhysAddr)
#endif
{
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    uint64_t Addr;
#else
    uint32_t Addr;
#endif
    void *pVoid;

    if (pBasePhysAddr->CodeAddr != 0) {
        Addr = (DBG_PORT_CORC_BASE_ADDR + DBG_PORT_ORC_PC_OFFSET);
        dsp_osal_typecast(&pVoid, &Addr);
        dsp_osal_writel_relaxed((UINT32)pBasePhysAddr->CodeAddr, pVoid);
    }
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    if (pBasePhysAddr->Code1Addr != 0) {
        Addr = (DBG_PORT_CORC1_BASE_ADDR + DBG_PORT_ORC_PC_OFFSET);
        dsp_osal_typecast(&pVoid, &Addr);
        dsp_osal_writel_relaxed((UINT32)pBasePhysAddr->Code1Addr, pVoid);
    }
    if (pBasePhysAddr->Code2Addr != 0) {
        Addr = (DBG_PORT_CORC2_BASE_ADDR + DBG_PORT_ORC_PC_OFFSET);
        dsp_osal_typecast(&pVoid, &Addr);
        dsp_osal_writel_relaxed((UINT32)pBasePhysAddr->Code2Addr, pVoid);
    }

    if (IdspBinShareMode == 1U) {
        if (pBasePhysAddr->Code2Addr != 0) {
            Addr = (DBG_PORT_CORC3_BASE_ADDR + DBG_PORT_ORC_PC_OFFSET);
            dsp_osal_typecast(&pVoid, &Addr);
            dsp_osal_writel_relaxed((UINT32)pBasePhysAddr->Code2Addr, pVoid);
        }
    } else {
        if (pBasePhysAddr->Code3Addr != 0) {
            Addr = (DBG_PORT_CORC3_BASE_ADDR + DBG_PORT_ORC_PC_OFFSET);
            dsp_osal_typecast(&pVoid, &Addr);
            dsp_osal_writel_relaxed((UINT32)pBasePhysAddr->Code3Addr, pVoid);
        }
    }
#endif

    if (pBasePhysAddr->MeAddr != 0U) {
        Addr = (DBG_PORT_EORC_BASE_ADDR + DBG_PORT_ORC_PC_OFFSET);
        dsp_osal_typecast(&pVoid, &Addr);
        dsp_osal_writel_relaxed(pBasePhysAddr->MeAddr, pVoid);
    }
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    if (pBasePhysAddr->Me1Addr != 0) {
        Addr = (DBG_PORT_EORC1_BASE_ADDR + DBG_PORT_ORC_PC_OFFSET);
        dsp_osal_typecast(&pVoid, &Addr);
        dsp_osal_writel_relaxed((UINT32)pBasePhysAddr->Me1Addr, pVoid);
    }
#endif
    if (pBasePhysAddr->MdxfAddr != 0U) {
        Addr = (DBG_PORT_DORC_BASE_ADDR + DBG_PORT_ORC_PC_OFFSET);
        dsp_osal_typecast(&pVoid, &Addr);
        dsp_osal_writel_relaxed(pBasePhysAddr->MdxfAddr, pVoid);
    }
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    if (pBasePhysAddr->Mdxf1Addr != 0) {
        Addr = (DBG_PORT_DORC1_BASE_ADDR + DBG_PORT_ORC_PC_OFFSET);
        dsp_osal_typecast(&pVoid, &Addr);
        dsp_osal_writel_relaxed((UINT32)pBasePhysAddr->Mdxf1Addr, pVoid);
    }
#endif
}

typedef struct {
    volatile uint32_t SyncCnt[DBG_PORT_SYNCNT_NUM];
} dsp_osal_orc_sync_cnt_t;
dsp_osal_orc_sync_cnt_t *pOrcSyncCnt = NULL;

static inline void dsp_osal_dbgport_map_sync_cnt(void)
{
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    uint64_t phys_addr;
#else
    uint32_t phys_addr;
#endif
    void __iomem *virt;

    if (pOrcSyncCnt == NULL) {
        phys_addr = DBG_PORT_SYNCNT_BASE_ADDR;
        virt = ioremap(phys_addr, sizeof(dsp_osal_orc_sync_cnt_t));
        if (virt == NULL) {
            dsp_osal_printU5("[ERR] dsp_osal_dbgport_map_sync_cnt : ioremap fail line[%d]", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            dsp_osal_typecast(&pOrcSyncCnt, &virt);
        }
    }
}

const volatile uint32_t *pOrcTimer = NULL;
void dsp_osal_map_orc_timer(void)
{
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    uint64_t phys_addr;
#else
    uint32_t phys_addr;
#endif
    void __iomem *virt;
    uint32_t size = 4U; //4byte

    if (pOrcTimer == NULL) {
        phys_addr = (DBG_PORT_CORC_BASE_ADDR+DBG_PORT_ORC_TIMER_OFFSET);
        virt = ioremap(phys_addr, size);
        if (virt == NULL) {
            dsp_osal_printU5("[ERR] dsp_osal_map_orc_timer : ioremap fail line[%d]", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            dsp_osal_typecast(&pOrcTimer, &virt);
        }
    }
}

void dsp_osal_orc_enable(UINT32 OrcMask)
{
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    uint64_t Addr;
#else
    uint32_t Addr;
#endif
    void *pVoid;

    /* Map SyncCnt buffer since Linux not allow ioremp inside ISR */
    dsp_osal_dbgport_map_sync_cnt();
    /* Map OrcTimer buffer since Linux not allow ioremp inside ISR */
    dsp_osal_map_orc_timer();

    /* Reset DORC/EORC first to prevent CORC trying to access them */
    /* EORC */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_ME0_IDX, 1U)) {
        Addr = (DBG_PORT_EORC_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
        dsp_osal_typecast(&pVoid, &Addr);
        dsp_osal_writel_relaxed(DBG_PORT_EORC_RESET_VALUE, pVoid);
    }
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    /* EORC1 */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_ME1_IDX, 1U)) {
        Addr = (DBG_PORT_EORC1_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
        dsp_osal_typecast(&pVoid, &Addr);
        dsp_osal_writel_relaxed(DBG_PORT_EORC_RESET_VALUE, pVoid);
    }
#endif

    /* DORC */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_MDXF0_IDX, 1U)) {
        Addr = (DBG_PORT_DORC_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
        dsp_osal_typecast(&pVoid, &Addr);
        dsp_osal_writel_relaxed(DBG_PORT_DORC_RESET_VALUE, pVoid);
    }
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    /* DORC1 */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_MDXF0_IDX, 1U)) {
        Addr = (DBG_PORT_DORC1_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
        dsp_osal_typecast(&pVoid, &Addr);
        dsp_osal_writel_relaxed(DBG_PORT_DORC_RESET_VALUE, pVoid);
    }
#endif

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    /* per ChenHan's suggest on 2021/6/1, orccode -> orcidsps -> orcvin */
    /* CORC */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_CODE_IDX, 1U)) {
        Addr = (DBG_PORT_CORC_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
        dsp_osal_typecast(&pVoid, &Addr);
        dsp_osal_writel_relaxed(DBG_PORT_CORC_RESET_VALUE, pVoid);
    }

    /* CORC3 */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_IDSP1_IDX, 1U)) {
        Addr = (DBG_PORT_CORC3_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
        dsp_osal_typecast(&pVoid, &Addr);
        dsp_osal_writel_relaxed(DBG_PORT_CORC_RESET_VALUE, pVoid);
    }

    /* CORC2 */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_IDSP0_IDX, 1U)) {
        Addr = (DBG_PORT_CORC2_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
        dsp_osal_typecast(&pVoid, &Addr);
        dsp_osal_writel_relaxed(DBG_PORT_CORC_RESET_VALUE, pVoid);
    }

    /* CORC1 */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_VIN_IDX, 1U)) {
        Addr = (DBG_PORT_CORC1_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
        dsp_osal_typecast(&pVoid, &Addr);
        dsp_osal_writel_relaxed(DBG_PORT_CORC_RESET_VALUE, pVoid);
    }
#else
    /* CORC */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_CODE_IDX, 1U)) {
        Addr = (DBG_PORT_CORC_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
        dsp_osal_typecast(&pVoid, &Addr);
        dsp_osal_writel_relaxed(DBG_PORT_CORC_RESET_VALUE, pVoid);
    }
#endif
}

void dsp_osal_orc_shutdown(void)
{
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    uint64_t Addr;
    void *pVoid;

    /* Map SyncCnt buffer since Linux not allow ioremp inside ISR */
    dsp_osal_dbgport_map_sync_cnt();
    /* Map OrcTimer buffer since Linux not allow ioremp inside ISR */
    dsp_osal_map_orc_timer();

    /* Reset DORC/EORC first to prevent CORC trying to access them */
    /* EORC */
    Addr = (DBG_PORT_EORC_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
    dsp_osal_typecast(&pVoid, &Addr);
    dsp_osal_writel_relaxed(DBG_PORT_EORC_SHUTDOWN_VALUE, pVoid);

    /* EORC1 */
    Addr = (DBG_PORT_EORC1_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
    dsp_osal_typecast(&pVoid, &Addr);
    dsp_osal_writel_relaxed(DBG_PORT_EORC_SHUTDOWN_VALUE, pVoid);

    /* DORC */
    Addr = (DBG_PORT_DORC_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
    dsp_osal_typecast(&pVoid, &Addr);
    dsp_osal_writel_relaxed(DBG_PORT_DORC_SHUTDOWN_VALUE, pVoid);

    /* DORC1 */
    Addr = (DBG_PORT_DORC1_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
    dsp_osal_typecast(&pVoid, &Addr);
    dsp_osal_writel_relaxed(DBG_PORT_DORC_SHUTDOWN_VALUE, pVoid);

    /* per ChenHan's suggest on 2021/6/1, orccode -> orcidsps -> orcvin */
    /* CORC */
    Addr = (DBG_PORT_CORC_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
    dsp_osal_typecast(&pVoid, &Addr);
    dsp_osal_writel_relaxed(DBG_PORT_CORC_SHUTDOWN_VALUE, pVoid);

    /* CORC3 */
    Addr = (DBG_PORT_CORC3_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
    dsp_osal_typecast(&pVoid, &Addr);
    dsp_osal_writel_relaxed(DBG_PORT_CORC_SHUTDOWN_VALUE, pVoid);

    /* CORC2 */
    Addr = (DBG_PORT_CORC2_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
    dsp_osal_typecast(&pVoid, &Addr);
    dsp_osal_writel_relaxed(DBG_PORT_CORC_SHUTDOWN_VALUE, pVoid);

    /* CORC1 */
    Addr = (DBG_PORT_CORC1_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
    dsp_osal_typecast(&pVoid, &Addr);
    dsp_osal_writel_relaxed(DBG_PORT_CORC_SHUTDOWN_VALUE, pVoid);
#elif defined (CONFIG_SOC_CV22) || defined (CONFIG_SOC_CV2) || defined (CONFIG_SOC_CV25) || defined (CONFIG_SOC_CV28) || defined (CONFIG_SOC_H32)
    uint64_t Addr;
    void *pVoid;

    /* Map SyncCnt buffer since Linux not allow ioremp inside ISR */
    dsp_osal_dbgport_map_sync_cnt();
    /* Map OrcTimer buffer since Linux not allow ioremp inside ISR */
    dsp_osal_map_orc_timer();

    /* Reset DORC/EORC first to prevent CORC trying to access them */
    /* EORC */
    Addr = (DBG_PORT_EORC_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
    dsp_osal_typecast(&pVoid, &Addr);
    dsp_osal_writel_relaxed(DBG_PORT_EORC_SHUTDOWN_VALUE, pVoid);

    /* DORC */
    Addr = (DBG_PORT_DORC_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
    dsp_osal_typecast(&pVoid, &Addr);
    dsp_osal_writel_relaxed(DBG_PORT_DORC_SHUTDOWN_VALUE, pVoid);

    /* per ChenHan's suggest on 2021/6/1, orccode -> orcidsps -> orcvin */
    /* CORC */
    Addr = (DBG_PORT_CORC_BASE_ADDR + DBG_PORT_ORC_RESET_OFFSET);
    dsp_osal_typecast(&pVoid, &Addr);
    dsp_osal_writel_relaxed(DBG_PORT_CORC_SHUTDOWN_VALUE, pVoid);
#else
    // TBD
#endif
}

void dsp_osal_dbgport_set_sync_cnt(uint32_t sync_cnt_id, uint32_t val)
{
    uint32_t reg_val;

    /* DspDriver will access SyncCnt frequently */
    /* Keep the SyncCnt virt-addr to prevent too much mmap-ops */
    dsp_osal_dbgport_map_sync_cnt();

    if (pOrcSyncCnt != NULL) {
        reg_val = ((val << 12) | val);
        pOrcSyncCnt->SyncCnt[sync_cnt_id] = reg_val;
    }
}

void dsp_osal_dbgport_set_smem_grp(void)
{
    void *pVoid;
    UINT32 SmemGrpAddr = (DBG_PORT_SMEM_GRP_BASE_ADDR + DBG_PORT_SMEM_GRP_OFFSET);
    UINT32 SmemGrpData = 0x83000002U; // b[31] for smem grouping

    dsp_osal_typecast(&pVoid, &SmemGrpAddr);
    dsp_osal_writel_relaxed(SmemGrpData, pVoid);
    dsp_osal_printU5("[DSP_INIT]SmemGrp", 0U, 0U, 0U, 0U, 0U);
}

uint32_t dsp_osal_get_orc_timer(void)
{
    dsp_osal_map_orc_timer();

    return *pOrcTimer;
}

uint32_t dsp_osal_get_dram_page(void)
{
    const UINT32 DramCAddr = (DBG_PORT_DRAM_PAGE_BASE_ADDR + DBG_PORT_DRAM_PAGE_OFFSET);
    void *pVoid;

    dsp_osal_typecast(&pVoid, &DramCAddr);
    return dsp_osal_readl_relaxed(pVoid);
}

void dsp_osal_get_cmd_req_timeout_cfg(uint32_t *pEnable, uint32_t *pTime)
{
    if ((pEnable == NULL) ||
        (pTime == NULL)) {
        dsp_osal_printU5("[ERR] dsp_osal_get_cmd_req_timeout_cfg : Null input", 0U, 0U, 0U, 0U, 0U);
    } else {
#define DSP_CMD_REQ_TIMEOUT_CHECK   (0U)
#define DSP_CMD_REQ_TIME            (8U) // sync counter timer in ms. ( only 1 ~ 4 ms allowed )
        /* Linux somehow can't get ISR in time, disable it first */
        *pEnable = DSP_CMD_REQ_TIMEOUT_CHECK;
        *pTime = DSP_CMD_REQ_TIME;

    }
}

static osal_buf_t ucode_bin_buf[UCODE_BIN_NUM] = {0};
uint32_t dsp_osal_alloc_ucode_addr(void)
{
    int32_t Rval = 0;
    struct device_node *sub_node = NULL;
    __be32 *reg;
    __be64 *reg64;
    int32_t len = 0;
    ULONG phys_addr = 0UL;
    uint32_t buf_size = 0U;
    pgprot_t prot;
    char *virt_buf = NULL;
    uint32_t i;
    uint32_t needed_dsp_bin_sz = 0;
    ULONG Base_start_virt_addr, start_virt_addr;
    uint32_t start_phys_addr;
    uint64_t mem_ofst = 0U;
    uint64_t used_size_offset = 0U, used_size = 0U;
    static const uint32_t ucode_bin_size[UCODE_BIN_NUM] = {
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        [UCODE_BIN_ORCODE]  = ORCODE_BIN_MAX_SIZE,
        [UCODE_BIN_ORCODE1] = ORCODE1_BIN_MAX_SIZE,
        [UCODE_BIN_ORCODE2] = ORCODE2_BIN_MAX_SIZE,
        [UCODE_BIN_ORCODE3] = ORCODE3_BIN_MAX_SIZE,
        [UCODE_BIN_ORCME]   = ORCME_BIN_MAX_SIZE,
        [UCODE_BIN_ORCME1]  = ORCME_BIN_MAX_SIZE,
        [UCODE_BIN_ORCMD]   = ORCMD_BIN_MAX_SIZE,
        [UCODE_BIN_ORCMD1]  = ORCMD_BIN_MAX_SIZE,
        [UCODE_BIN_DEFAULT] = DEFAULT_BIN_MAX_SIZE,
#else
        [UCODE_BIN_ORCODE]  = ORCODE_BIN_MAX_SIZE,
        [UCODE_BIN_ORCME]   = ORCME_BIN_MAX_SIZE,
        [UCODE_BIN_ORCMD]   = ORCMD_BIN_MAX_SIZE,
        [UCODE_BIN_DEFAULT] = DEFAULT_BIN_MAX_SIZE,
#endif
    };

    if (dsp_dev_node == NULL) {
        dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr : Null dsp_dev_node", 0U, 0U, 0U, 0U, 0U);
    } else {
        sub_node = of_parse_phandle(dsp_dev_node, "memory-region2", 0);
        if (sub_node != NULL) {
            reg = (__be32 *) of_get_property(sub_node, "reg", &len);
            if (reg && (len == (2 * sizeof(uint32_t)))) {
                phys_addr = (ULONG)be32_to_cpu(reg[0]);
                buf_size = be32_to_cpu(reg[1]);
            } else if (reg && (len == (4 * sizeof(uint32_t)))) {
                reg64 = (__be64 *)reg;
                phys_addr = (ULONG)be64_to_cpu(reg64[0]);
                buf_size = (uint32_t)be64_to_cpu(reg64[1]);
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr : wrong format", 0U, 0U, 0U, 0U, 0U);
            }
            prot = __pgprot(PROT_NORMAL_NC); // binary buffer shall be NonCache
            virt_buf = __ioremap(phys_addr, buf_size, prot);

            for (i = 0U; i<UCODE_BIN_NUM; i++) {
                needed_dsp_bin_sz += DSP_BUF_ADDR_ALIGN;
                needed_dsp_bin_sz += ucode_bin_size[i];
            }

            if (needed_dsp_bin_sz > buf_size) {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : total_dsp_bin_sz (0x%X) > buf_size (0x%X)", needed_dsp_bin_sz, buf_size, 0U, 0U, 0U);
            } else {

                /* update buf_pool */
                dsp_osal_typecast(&osal_buf_pool[DSP_BUF_POOL_BINARY].Base, &virt_buf);
                osal_buf_pool[DSP_BUF_POOL_BINARY].size = buf_size;
                osal_buf_pool[DSP_BUF_POOL_BINARY].UsedSize = 0U;
                osal_buf_pool[DSP_BUF_POOL_BINARY].is_cached = 0U;
                (void)dsp_osal_memcpy(&osal_buf_pool[DSP_BUF_POOL_BINARY].PhysBase, &phys_addr, sizeof(ULONG));
                dsp_osal_printU5("[dspdev] mempool[%d] Virt 0x%X%08X Phys 0x%X%08X",
                                  DSP_BUF_POOL_BINARY,
                                  dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_BINARY].Base),
                                  dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_BINARY].Base),
                                  dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_BINARY].PhysBase),
                                  dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_BINARY].PhysBase));
                dsp_osal_printU5("                     Sz %d Cached %d",
                                  osal_buf_pool[DSP_BUF_POOL_BINARY].size,
                                  osal_buf_pool[DSP_BUF_POOL_BINARY].is_cached, 0U, 0U, 0U);

                (void)dsp_osal_typecast(&Base_start_virt_addr, &virt_buf);
                start_virt_addr = ADDR_ALIGN_NUM(Base_start_virt_addr, DSP_BUF_ADDR_ALIGN);
                mem_ofst = (start_virt_addr - Base_start_virt_addr);
                used_size_offset += mem_ofst;
                start_phys_addr = phys_addr + mem_ofst;

                /* Allocate orcode */
                ucode_bin_buf[UCODE_BIN_ORCODE].Base = start_virt_addr;
                ucode_bin_buf[UCODE_BIN_ORCODE].size = ucode_bin_size[UCODE_BIN_ORCODE];
                ucode_bin_buf[UCODE_BIN_ORCODE].pool_id = DSP_BUF_POOL_BINARY;
                used_size += ucode_bin_size[UCODE_BIN_ORCODE];
                used_size = ALIGN_NUM64(used_size, DSP_BUF_ADDR_ALIGN);

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
                /* Allocate orcode1 */
                if ((used_size_offset + used_size) < buf_size) {
                    ucode_bin_buf[UCODE_BIN_ORCODE1].Base = start_virt_addr + used_size;
                    ucode_bin_buf[UCODE_BIN_ORCODE1].size = ucode_bin_size[UCODE_BIN_ORCODE1];
                    ucode_bin_buf[UCODE_BIN_ORCODE1].pool_id = DSP_BUF_POOL_BINARY;
                    used_size += ucode_bin_size[UCODE_BIN_ORCODE1];
                    used_size = ALIGN_NUM64(used_size, DSP_BUF_ADDR_ALIGN);
                } else {
                    dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0003;
                }

                /* Allocate orcode2 */
                if ((used_size_offset + used_size) < buf_size) {
                    ucode_bin_buf[UCODE_BIN_ORCODE2].Base = start_virt_addr + used_size;
                    ucode_bin_buf[UCODE_BIN_ORCODE2].size = ucode_bin_size[UCODE_BIN_ORCODE2];
                    ucode_bin_buf[UCODE_BIN_ORCODE2].pool_id = DSP_BUF_POOL_BINARY;
                    used_size += ucode_bin_size[UCODE_BIN_ORCODE2];
                    used_size = ALIGN_NUM64(used_size, DSP_BUF_ADDR_ALIGN);
                } else {
                    dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0003;
                }

                /* Allocate orcode1 */
                if ((used_size_offset + used_size) < buf_size) {
                    ucode_bin_buf[UCODE_BIN_ORCODE3].Base = start_virt_addr + used_size;
                    ucode_bin_buf[UCODE_BIN_ORCODE3].size = ucode_bin_size[UCODE_BIN_ORCODE3];
                    ucode_bin_buf[UCODE_BIN_ORCODE3].pool_id = DSP_BUF_POOL_BINARY;
                    used_size += ucode_bin_size[UCODE_BIN_ORCODE3];
                    used_size = ALIGN_NUM64(used_size, DSP_BUF_ADDR_ALIGN);
                } else {
                    dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0003;
                }
#endif

                /* Allocate orcme */
                if ((used_size_offset + used_size) < buf_size) {
                    ucode_bin_buf[UCODE_BIN_ORCME].Base = start_virt_addr + used_size;
                    ucode_bin_buf[UCODE_BIN_ORCME].size = ucode_bin_size[UCODE_BIN_ORCME];
                    ucode_bin_buf[UCODE_BIN_ORCME].pool_id = DSP_BUF_POOL_BINARY;
                    used_size += ucode_bin_size[UCODE_BIN_ORCME];
                    used_size = ALIGN_NUM64(used_size, DSP_BUF_ADDR_ALIGN);
                } else {
                    dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0003;
                }

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
                /* Allocate orcme */
                if ((used_size_offset + used_size) < buf_size) {
                    ucode_bin_buf[UCODE_BIN_ORCME1].Base = start_virt_addr + used_size;
                    ucode_bin_buf[UCODE_BIN_ORCME1].size = ucode_bin_size[UCODE_BIN_ORCME1];
                    ucode_bin_buf[UCODE_BIN_ORCME1].pool_id = DSP_BUF_POOL_BINARY;
                    used_size += ucode_bin_size[UCODE_BIN_ORCME1];
                    used_size = ALIGN_NUM64(used_size, DSP_BUF_ADDR_ALIGN);
                } else {
                    dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0003;
                }
#endif

                /* Allocate orcmd */
                if ((Rval == DSP_ERR_NONE) &&
                    (used_size_offset + used_size) < buf_size) {
                    ucode_bin_buf[UCODE_BIN_ORCMD].Base = start_virt_addr + used_size;
                    ucode_bin_buf[UCODE_BIN_ORCMD].size = ucode_bin_size[UCODE_BIN_ORCMD];
                    ucode_bin_buf[UCODE_BIN_ORCMD].pool_id = DSP_BUF_POOL_BINARY;
                    used_size += ucode_bin_size[UCODE_BIN_ORCMD];
                    used_size = ALIGN_NUM64(used_size, DSP_BUF_ADDR_ALIGN);
                } else {
                    dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0003;
                }

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
                /* Allocate orcmd1 */
                if ((Rval == DSP_ERR_NONE) &&
                    (used_size_offset + used_size) < buf_size) {
                    ucode_bin_buf[UCODE_BIN_ORCMD1].Base = start_virt_addr + used_size;
                    ucode_bin_buf[UCODE_BIN_ORCMD1].size = ucode_bin_size[UCODE_BIN_ORCMD1];
                    ucode_bin_buf[UCODE_BIN_ORCMD1].pool_id = DSP_BUF_POOL_BINARY;
                    used_size += ucode_bin_size[UCODE_BIN_ORCMD1];
                    used_size = ALIGN_NUM64(used_size, DSP_BUF_ADDR_ALIGN);
                } else {
                    dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0003;
                }
#endif

                /* Allocate default binary */
                if ((Rval == DSP_ERR_NONE) &&
                    (used_size_offset + used_size) < buf_size) {
                    ucode_bin_buf[UCODE_BIN_DEFAULT].Base = start_virt_addr + used_size;
                    ucode_bin_buf[UCODE_BIN_DEFAULT].size = ucode_bin_size[UCODE_BIN_DEFAULT];
                    ucode_bin_buf[UCODE_BIN_DEFAULT].pool_id = DSP_BUF_POOL_BINARY;
                } else {
                    dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0003;
                }
            }
        }
    }
    return Rval;
}

uint32_t dsp_osal_get_ucode_base_addr(osal_ucode_addr_t *pBase)
{
    uint32_t Rval = DSP_ERR_NONE;

    if (pBase == NULL) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_get_ucode_base_addr : Null address", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* Get dsp binary mem range */
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        if ((ucode_bin_buf[UCODE_BIN_ORCODE].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCODE1].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCODE2].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCODE3].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCME].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCME1].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCMD].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCMD1].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_DEFAULT].Base == 0UL))
#else
        if ((ucode_bin_buf[UCODE_BIN_ORCODE].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCME].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCMD].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_DEFAULT].Base == 0UL))
#endif
        {
            Rval = dsp_osal_alloc_ucode_addr();
        }

        if (Rval == DSP_ERR_NONE) {
            pBase->CodeAddr = ucode_bin_buf[UCODE_BIN_ORCODE].Base;
            pBase->MeAddr = ucode_bin_buf[UCODE_BIN_ORCME].Base;
            pBase->MdxfAddr = ucode_bin_buf[UCODE_BIN_ORCMD].Base;
            pBase->DefBinAddr = ucode_bin_buf[UCODE_BIN_DEFAULT].Base;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
            pBase->Code1Addr = ucode_bin_buf[UCODE_BIN_ORCODE1].Base;
            pBase->Code2Addr = ucode_bin_buf[UCODE_BIN_ORCODE2].Base;
            pBase->Code3Addr = ucode_bin_buf[UCODE_BIN_ORCODE3].Base;
            pBase->Me1Addr = ucode_bin_buf[UCODE_BIN_ORCME1].Base;
            pBase->Mdxf1Addr = ucode_bin_buf[UCODE_BIN_ORCMD1].Base;
#endif
        }
    }

    return Rval;
}

uint32_t dsp_osal_get_ucode_end_addr(osal_ucode_addr_t *pEnd)
{
    uint32_t Rval = DSP_ERR_NONE;

    if (pEnd == NULL) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_get_ucode_end_addr : Null address", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* Get dsp binary mem range */
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        if ((ucode_bin_buf[UCODE_BIN_ORCODE].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCODE1].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCODE2].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCODE3].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCME].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCME1].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCMD].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCMD1].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_DEFAULT].Base == 0UL))
#else
        if ((ucode_bin_buf[UCODE_BIN_ORCODE].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCME].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCMD].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_DEFAULT].Base == 0UL))
#endif
        {
            Rval = dsp_osal_alloc_ucode_addr();
        }

        if (Rval == DSP_ERR_NONE) {
            pEnd->CodeAddr = ucode_bin_buf[UCODE_BIN_ORCODE].Base + ucode_bin_buf[UCODE_BIN_ORCODE].size;
            pEnd->MeAddr = ucode_bin_buf[UCODE_BIN_ORCME].Base + ucode_bin_buf[UCODE_BIN_ORCME].size;
            pEnd->MdxfAddr = ucode_bin_buf[UCODE_BIN_ORCMD].Base + ucode_bin_buf[UCODE_BIN_ORCMD].size;
            pEnd->DefBinAddr = ucode_bin_buf[UCODE_BIN_DEFAULT].Base + ucode_bin_buf[UCODE_BIN_DEFAULT].size;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
            pEnd->Code1Addr = ucode_bin_buf[UCODE_BIN_ORCODE1].Base + ucode_bin_buf[UCODE_BIN_ORCODE1].size;
            pEnd->Code2Addr = ucode_bin_buf[UCODE_BIN_ORCODE2].Base + ucode_bin_buf[UCODE_BIN_ORCODE2].size;
            pEnd->Code3Addr = ucode_bin_buf[UCODE_BIN_ORCODE3].Base + ucode_bin_buf[UCODE_BIN_ORCODE3].size;
            pEnd->Me1Addr = ucode_bin_buf[UCODE_BIN_ORCME1].Base + ucode_bin_buf[UCODE_BIN_ORCME1].size;
            pEnd->Mdxf1Addr = ucode_bin_buf[UCODE_BIN_ORCMD1].Base + ucode_bin_buf[UCODE_BIN_ORCMD1].size;
#endif
        }
    }

    return Rval;
}

/* allocate dsp protocol buffer */
uint32_t dsp_osal_alloc_prot_addr(void)
{
    uint32_t Rval = DSP_ERR_NONE;
    struct device_node *sub_node = NULL;
    __be32 *reg;
    __be64 *reg64;
    int32_t len = 0;
    ULONG phys_addr = 0UL;
    uint32_t buf_size = 0U;
    pgprot_t prot;
    char *virt_buf = NULL;

    if (dsp_dev_node == NULL) {
        dsp_osal_printU5("[ERR] dsp_osal_alloc_prot_addr : Null dsp_dev_node", 0U, 0U, 0U, 0U, 0U);
    } else {
        sub_node = of_parse_phandle(dsp_dev_node, "memory-region", 0);
        if (sub_node != NULL) {
            reg = (__be32 *) of_get_property(sub_node, "reg", &len);
            if (reg && (len == (2 * sizeof(uint32_t)))) {
                phys_addr = (ULONG)be32_to_cpu(reg[0]);
                buf_size = be32_to_cpu(reg[1]);
            } else if (reg && (len == (4 * sizeof(uint32_t)))) {
                reg64 = (__be64 *)reg;
                phys_addr = (ULONG)be64_to_cpu(reg64[0]);
                buf_size = (uint32_t)be64_to_cpu(reg64[1]);
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_prot_addr : wrong format", 0U, 0U, 0U, 0U, 0U);
            }
            prot = __pgprot(PROT_NORMAL_NC); // protocol buffer shall be NonCache
            virt_buf = __ioremap(phys_addr, buf_size, prot);

            /* update buf_pool */
            dsp_osal_typecast(&osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base, &virt_buf);
            osal_buf_pool[DSP_BUF_POOL_PROTOCOL].size = buf_size;
            osal_buf_pool[DSP_BUF_POOL_PROTOCOL].UsedSize = 0U;
            osal_buf_pool[DSP_BUF_POOL_PROTOCOL].is_cached = 0U;
            (void)dsp_osal_memcpy(&osal_buf_pool[DSP_BUF_POOL_PROTOCOL].PhysBase, &phys_addr, sizeof(ULONG));
            dsp_osal_printU5("[dspdev] mempool[%d] Virt 0x%X%08X Phys 0x%X%08X",
                              DSP_BUF_POOL_PROTOCOL,
                              dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base),
                              dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base),
                              dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_PROTOCOL].PhysBase),
                              dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_PROTOCOL].PhysBase));
            dsp_osal_printU5("                     Sz %d Cached %d",
                              osal_buf_pool[DSP_BUF_POOL_PROTOCOL].size,
                              osal_buf_pool[DSP_BUF_POOL_PROTOCOL].is_cached, 0U, 0U, 0U);
        }
    }

    return Rval;
}

UINT32 dsp_osal_alloc_prot_buf(ULONG *pBase, UINT32 ReqSize, UINT32 Align)
{
    UINT32 Rval = DSP_ERR_NONE;
    ULONG Offset, CurBase;
    UINT32 NeededSize;

    if ((pBase == NULL) || (ReqSize == 0U)) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_get_prot_buf : Null address or Zero size", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* Get dsp protocol mem range */
        if (osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base == 0UL) {
            Rval = dsp_osal_alloc_prot_addr();
        }

        if (Rval == DSP_ERR_NONE) {
            CurBase = osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base + osal_buf_pool[DSP_BUF_POOL_PROTOCOL].UsedSize;
            Offset = ADDR_ALIGN_NUM(CurBase, Align) - CurBase;
            NeededSize = (UINT32)Offset + ReqSize;
            if ((UINT32)((CurBase + NeededSize) - osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base) > osal_buf_pool[DSP_BUF_POOL_PROTOCOL].size) {
                Rval = DSP_ERR_0003;
            } else {
                *pBase = CurBase + Offset;

                /* Update pool */
                osal_buf_pool[DSP_BUF_POOL_PROTOCOL].UsedSize += NeededSize;
            }
        }
    }

    return Rval;
}

/* allocate dsp statistic buffer */
uint32_t dsp_osal_alloc_stat_addr(void)
{
    uint32_t Rval = DSP_ERR_NONE;
    struct device_node *sub_node = NULL;
    __be32 *reg;
    __be64 *reg64;
    int32_t len = 0;
    ULONG phys_addr = 0UL;
    uint32_t buf_size = 0U;
    pgprot_t prot;
    char *virt_buf = NULL;

    if (dsp_dev_node == NULL) {
        dsp_osal_printU5("[ERR] dsp_osal_alloc_stat_addr : Null dsp_dev_node", 0U, 0U, 0U, 0U, 0U);
    } else {
        sub_node = of_parse_phandle(dsp_dev_node, "memory-region1", 0);
        if (sub_node != NULL) {
            reg = (__be32 *) of_get_property(sub_node, "reg", &len);
            if (reg && (len == (2 * sizeof(uint32_t)))) {
                phys_addr = (ULONG)be32_to_cpu(reg[0]);
                buf_size = be32_to_cpu(reg[1]);
            } else if (reg && (len == (4 * sizeof(uint32_t)))) {
                reg64 = (__be64 *)reg;
                phys_addr = (ULONG)be64_to_cpu(reg64[0]);
                buf_size = (uint32_t)be64_to_cpu(reg64[1]);
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_stat_addr : wrong format", 0U, 0U, 0U, 0U, 0U);
            }
            prot = __pgprot(PROT_NORMAL); // statistic buffer shall be Cache
            virt_buf = __ioremap(phys_addr, buf_size, prot);

            /* update buf_pool */
            dsp_osal_typecast(&osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base, &virt_buf);
            osal_buf_pool[DSP_BUF_POOL_STATISTIC].size = buf_size;
            osal_buf_pool[DSP_BUF_POOL_STATISTIC].UsedSize = 0U;
            osal_buf_pool[DSP_BUF_POOL_STATISTIC].is_cached = 1U;
            (void)dsp_osal_memcpy(&osal_buf_pool[DSP_BUF_POOL_STATISTIC].PhysBase, &phys_addr, sizeof(ULONG));
            dsp_osal_printU5("[dspdev] mempool[%d] Virt 0x%X%08X Phys 0x%X%08X",
                              DSP_BUF_POOL_STATISTIC,
                              dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base),
                              dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base),
                              dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_STATISTIC].PhysBase),
                              dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_STATISTIC].PhysBase));
            dsp_osal_printU5("                     Sz %d Cached %d",
                              osal_buf_pool[DSP_BUF_POOL_STATISTIC].size,
                              osal_buf_pool[DSP_BUF_POOL_STATISTIC].is_cached, 0U, 0U, 0U);
        }
    }

    return Rval;
}

UINT32 dsp_osal_get_stat_buf(ULONG *pBase, UINT32 *pSize)
{
    UINT32 Rval = DSP_ERR_NONE;

    if ((pBase == NULL) || (pSize == NULL)) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_get_stat_buf : Null address or size", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* Get dsp statistic mem range */
        if (osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base == 0UL) {
            Rval = dsp_osal_alloc_stat_addr();
        }

        if (Rval == DSP_ERR_NONE) {
            *pBase = osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base;
            *pSize = osal_buf_pool[DSP_BUF_POOL_STATISTIC].size;
        }
    }

    return Rval;
}

UINT32 dsp_osal_alloc_stat_buf(ULONG *pBase, UINT32 ReqSize, UINT32 Align)
{
    UINT32 Rval = DSP_ERR_NONE;
    ULONG Offset, CurBase;
    UINT32 NeededSize;

    if ((pBase == NULL) || (ReqSize == 0U)) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_alloc_stat_buf : Null address or Zero size", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* Get dsp statistic mem range */
        if (osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base == 0UL) {
            Rval = dsp_osal_alloc_stat_addr();
        }

        if (Rval == DSP_ERR_NONE) {
            CurBase = osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base + osal_buf_pool[DSP_BUF_POOL_STATISTIC].UsedSize;
            Offset = ADDR_ALIGN_NUM(CurBase, Align) - CurBase;
            NeededSize = (UINT32)Offset + ReqSize;
            if ((UINT32)((CurBase + NeededSize) - osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base) > osal_buf_pool[DSP_BUF_POOL_STATISTIC].size) {
                Rval = DSP_ERR_0003;
            } else {
                *pBase = CurBase + Offset;

                /* Update pool */
                osal_buf_pool[DSP_BUF_POOL_STATISTIC].UsedSize += NeededSize;
            }
        }
    }

    return Rval;
}

UINT32 dsp_osal_alloc_add_buf_addr(UINT32 Type, ULONG phys_addr, UINT32 size, UINT32 IsCached)
{
    UINT32 Rval = DSP_ERR_NONE;
    void *virt = NULL;

    if ((Type == DSP_BUF_POOL_WORK) ||
        (Type == DSP_BUF_POOL_DBG_LOG) ||
        (Type == DSP_BUF_POOL_DATA)) {
        /* un-map */
        if (osal_buf_pool[Type].Base != 0U) {
            dsp_osal_typecast(&virt, &osal_buf_pool[Type].Base);
            iounmap(virt);
        }

        /* map */
        virt = ioremap(phys_addr, size);
        if (virt == NULL) {
            dsp_osal_printU5("[ERR] dsp_osal_alloc_add_buf_addr : mmap_device_memory fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            dsp_osal_typecast(&osal_buf_pool[Type].Base, &virt);
            osal_buf_pool[Type].PhysBase = phys_addr;
            osal_buf_pool[Type].size = size;
            osal_buf_pool[Type].UsedSize = 0U;
            osal_buf_pool[Type].is_cached = IsCached;
            dsp_osal_printU5("[dspdev] mempool[%d] Virt 0x%X%08X Phys 0x%X%08X",
                              Type,
                              dsp_osal_get_u64msb((UINT64)osal_buf_pool[Type].Base),
                              dsp_osal_get_u64lsb((UINT64)osal_buf_pool[Type].Base),
                              dsp_osal_get_u64msb((UINT64)osal_buf_pool[Type].PhysBase),
                              dsp_osal_get_u64lsb((UINT64)osal_buf_pool[Type].PhysBase));
            dsp_osal_printU5("                     Sz %d Cached %d",
                              osal_buf_pool[Type].size,
                              osal_buf_pool[Type].is_cached, 0U, 0U, 0U);
        }
    } else {
        // DO NOTHING
    }

    return Rval;
}

#if defined (CONFIG_BUILD_MAL)
uint32_t dsp_osal_alloc_aux_addr(void)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT16 i;
    UINT32 MalId;
    AMBA_MAL_INFO_s MalInfo = {0};
    UINT64 Attr = 0ULL;
    void *vaddr = NULL;

    for (i = 0U; i < DSP_AUX_BUF_POOL_NUM; i++) {
        MalId = osal_aux_buf_mal_id_tbl[i];
        Rval = AmbaMAL_GetInfo(MalId, &MalInfo);
        if (Rval == DSP_ERR_NONE) {
            if (MalId != MalInfo.Id) {
                // DO NOTHING
            } else if (MalInfo.Type == AMBA_MAL_TYPE_INVALID) {
                // DO NOTHING
            } else {
                Attr = AMBA_MAL_ATTRI_CACHE;
                Rval = AmbaMAL_Map(MalId, MalInfo.PhysAddr, MalInfo.Size, Attr, &vaddr);
                if (Rval != DSP_ERR_NONE) {
                    printk("[ERR] dsp_osal_alloc_aux_addr : MAL_Map[0x%X] Phys[0x%X%08X] Sz[%d] Attr[0x%X]", Rval,
                                                                                                             dsp_osal_get_u64msb(MalInfo.PhysAddr),
                                                                                                             dsp_osal_get_u64lsb(MalInfo.PhysAddr),
                                                                                                             (UINT32)MalInfo.Size,
                                                                                                             (UINT32)Attr);
                } else {
                    /* update buf_pool */
                    dsp_osal_typecast(&osal_aux_buf_pool[i].Base, &vaddr);
                    osal_aux_buf_pool[i].size = MalInfo.Size;
                    osal_aux_buf_pool[i].UsedSize = 0U;
                    osal_aux_buf_pool[i].is_cached = 1U;
                    (void)dsp_osal_memcpy(&osal_aux_buf_pool[i].PhysBase, &MalInfo.PhysAddr, sizeof(ULONG));
                    printk("[dspdev] MAL aux mempool[%d] Virt 0x%X%08X Phys 0x%X%08X", i,
                                                                                   dsp_osal_get_u64msb(osal_aux_buf_pool[i].Base),
                                                                                   dsp_osal_get_u64lsb(osal_aux_buf_pool[i].Base),
                                                                                   dsp_osal_get_u64msb(osal_aux_buf_pool[i].PhysBase),
                                                                                   dsp_osal_get_u64lsb(osal_aux_buf_pool[i].PhysBase));
                    printk("                            Sz %u Cached %d", osal_aux_buf_pool[i].size,
                                                                          osal_aux_buf_pool[i].is_cached);

                }
            }
        }
    }
    return Rval;
}
#endif


/* allocate dsp data buffer */
#if defined (CONFIG_BUILD_MAL)
uint32_t dsp_osal_alloc_data_addr(void)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 MalId = AMBA_MAL_ID_DSP_DATA;
    AMBA_MAL_INFO_s MalInfo = {0};
    UINT64 Attr = 0ULL;
    void *vaddr = NULL;

    Rval = AmbaMAL_GetInfo(MalId, &MalInfo);
    if (Rval != DSP_ERR_NONE) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_alloc_data_addr : MAL_GetInfo[%d 0x%X]", MalId, Rval, 0U, 0U, 0U);
    } else {
        if (MalId != MalInfo.Id) {
            Rval = DSP_ERR_0003;
            dsp_osal_printU5("[ERR] dsp_osal_alloc_data_addr : Id error[%d %d]", MalId, MalInfo.Id, 0U, 0U, 0U);
        } else if (MalInfo.Type == AMBA_MAL_TYPE_INVALID) {
            Rval = DSP_ERR_0003;
            dsp_osal_printU5("[ERR] dsp_osal_alloc_data_addr : type error[%d]", MalInfo.Type, 0U, 0U, 0U, 0U);
        } else {
            // DO NOTHING
        }

        if (Rval == DSP_ERR_NONE) {
            Attr = AMBA_MAL_ATTRI_CACHE;
            Rval = AmbaMAL_Map(MalId, MalInfo.PhysAddr, MalInfo.Size, Attr, &vaddr);
            if (Rval != DSP_ERR_NONE) {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_data_addr : MAL_Map[0x%X] Phys[0x%X%08X] Sz[%d] Attr[0x%X]", Rval,
                                                                                                                    dsp_osal_get_u64msb(MalInfo.PhysAddr),
                                                                                                                    dsp_osal_get_u64lsb(MalInfo.PhysAddr),
                                                                                                                    MalInfo.Size,
                                                                                                                    Attr);
            } else {
                /* update buf_pool */
                dsp_osal_typecast(&osal_buf_pool[DSP_BUF_POOL_DATA].Base, &vaddr);
                osal_buf_pool[DSP_BUF_POOL_DATA].size = MalInfo.Size;
                osal_buf_pool[DSP_BUF_POOL_DATA].UsedSize = 0U;
                osal_buf_pool[DSP_BUF_POOL_DATA].is_cached = 1U;
                (void)dsp_osal_memcpy(&osal_buf_pool[DSP_BUF_POOL_DATA].PhysBase, &MalInfo.PhysAddr, sizeof(ULONG));
                dsp_osal_printU5("[dspdev] MAL mempool[%d] Virt 0x%X%08X Phys 0x%X%08X",
                                  DSP_BUF_POOL_DATA,
                                  dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_DATA].Base),
                                  dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_DATA].Base),
                                  dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_DATA].PhysBase),
                                  dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_DATA].PhysBase));
                dsp_osal_printU5("                         Sz %d Cached %d",
                                  osal_buf_pool[DSP_BUF_POOL_DATA].size,
                                  osal_buf_pool[DSP_BUF_POOL_DATA].is_cached, 0U, 0U, 0U);
            }
        }
    }

    return Rval;
}
#else
uint32_t dsp_osal_alloc_data_addr(void)
{
    uint32_t Rval = DSP_ERR_NONE;
    struct device_node *sub_node = NULL;
    __be32 *reg;
    __be64 *reg64;
    int32_t len = 0;
    ULONG phys_addr = 0UL;
    uint32_t buf_size = 0U;
    pgprot_t prot;
    char *virt_buf = NULL;

    if (dsp_dev_node == NULL) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_alloc_data_addr : Null dsp_dev_node", 0U, 0U, 0U, 0U, 0U);
    } else {
        sub_node = of_parse_phandle(dsp_dev_node, "memory-region3", 0);
        if (sub_node != NULL) {
            reg = (__be32 *) of_get_property(sub_node, "reg", &len);
            if (reg && (len == (2 * sizeof(uint32_t)))) {
                phys_addr = (ULONG)be32_to_cpu(reg[0]);
                buf_size = be32_to_cpu(reg[1]);
            } else if (reg && (len == (4 * sizeof(uint32_t)))) {
                reg64 = (__be64 *)reg;
                phys_addr = (ULONG)be64_to_cpu(reg64[0]);
                buf_size = (uint32_t)be64_to_cpu(reg64[1]);
            } else {
                Rval = DSP_ERR_0003;
                dsp_osal_printU5("[ERR] dsp_osal_alloc_data_addr : wrong format", 0U, 0U, 0U, 0U, 0U);
            }
            prot = __pgprot(PROT_NORMAL); // data buffer shall be Cache
            virt_buf = __ioremap(phys_addr, buf_size, prot);

            /* update buf_pool */
            dsp_osal_typecast(&osal_buf_pool[DSP_BUF_POOL_DATA].Base, &virt_buf);
            osal_buf_pool[DSP_BUF_POOL_DATA].size = buf_size;
            osal_buf_pool[DSP_BUF_POOL_DATA].UsedSize = 0U;
            osal_buf_pool[DSP_BUF_POOL_DATA].is_cached = 1U;
            (void)dsp_osal_memcpy(&osal_buf_pool[DSP_BUF_POOL_DATA].PhysBase, &phys_addr, sizeof(ULONG));
            dsp_osal_printU5("[dspdev] mempool[%d] Virt 0x%X%08X Phys 0x%X%08X",
                              DSP_BUF_POOL_DATA,
                              dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_DATA].Base),
                              dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_DATA].Base),
                              dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_DATA].PhysBase),
                              dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_DATA].PhysBase));
            dsp_osal_printU5("                     Sz %d Cached %d",
                              osal_buf_pool[DSP_BUF_POOL_DATA].size,
                              osal_buf_pool[DSP_BUF_POOL_DATA].is_cached, 0U, 0U, 0U);
        }
    }

    return Rval;
}
#endif

UINT32 dsp_osal_get_data_buf(ULONG *pBase, UINT32 *pSize, UINT32 *pIsCached)
{
    UINT32 Rval = DSP_ERR_NONE;

    if ((pBase == NULL) || (pSize == NULL) || (pIsCached == NULL)) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_get_data_buf : Null input", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* Get dsp data mem range */
        if (osal_buf_pool[DSP_BUF_POOL_DATA].Base == 0UL) {
            Rval = dsp_osal_alloc_data_addr();
        }

        if (Rval == DSP_ERR_NONE) {
            *pBase = osal_buf_pool[DSP_BUF_POOL_DATA].Base;
            *pSize = osal_buf_pool[DSP_BUF_POOL_DATA].size;
            *pIsCached = osal_buf_pool[DSP_BUF_POOL_DATA].is_cached;
        }
    }

    return Rval;
}

UINT32 dsp_osal_get_bin_buf(ULONG *pBase, UINT32 *pSize, UINT32 *pIsCached)
{
    UINT32 Rval = DSP_ERR_NONE;

    if ((pBase == NULL) || (pSize == NULL) || (pIsCached == NULL)) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_get_bin_buf : Null input", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* Get dsp log mem range */
        if (osal_buf_pool[DSP_BUF_POOL_BINARY].Base == 0UL) {
            Rval = dsp_osal_alloc_ucode_addr();
        }

        if (Rval == DSP_ERR_NONE) {
            *pBase = osal_buf_pool[DSP_BUF_POOL_BINARY].Base;
            *pSize = osal_buf_pool[DSP_BUF_POOL_BINARY].size;
            *pIsCached = osal_buf_pool[DSP_BUF_POOL_BINARY].is_cached;
        }
    }

    return Rval;
}

/******************************************************************************/
/*                                  DebugPort                                 */
/******************************************************************************/
#if defined (CONFIG_SOC_CV22)
#define DBG_PORT_IDSP_ADDR_EXT_BASE_ADDR    (0xED1C8000)
#define DBG_PORT_IDSP_VIN0_SECT_ID          (1)
#define DBG_PORT_IDSP_VIN1_SECT_ID          (10)
#define DBG_PORT_IDSP_VIN2_SECT_ID          (1)
#define DBG_PORT_IDSP_VIN3_SECT_ID          (1)
#define DBG_PORT_IDSP_VIN4_SECT_ID          (1)
#define DBG_PORT_IDSP_VIN5_SECT_ID          (1)
#define DBG_PORT_IDSP_VIN_ENB_BASE_ADDR     (0xED1C0000)
#elif defined (CONFIG_SOC_CV2)
#define DBG_PORT_IDSP_ADDR_EXT_BASE_ADDR    (0xED1C8000)
#define DBG_PORT_IDSP_VIN0_SECT_ID          (1)
#define DBG_PORT_IDSP_VIN1_SECT_ID          (10)
#define DBG_PORT_IDSP_VIN2_SECT_ID          (11)
#define DBG_PORT_IDSP_VIN3_SECT_ID          (12)
#define DBG_PORT_IDSP_VIN4_SECT_ID          (13)
#define DBG_PORT_IDSP_VIN5_SECT_ID          (14)
#define DBG_PORT_IDSP_VIN_ENB_BASE_ADDR     (0xED1C0000)
#elif defined (CONFIG_SOC_CV25)
#define DBG_PORT_IDSP_ADDR_EXT_BASE_ADDR    (0xED1C8000)
#define DBG_PORT_IDSP_VIN0_SECT_ID          (1)
#define DBG_PORT_IDSP_VIN1_SECT_ID          (10)
#define DBG_PORT_IDSP_VIN2_SECT_ID          (11)
#define DBG_PORT_IDSP_VIN3_SECT_ID          (1)
#define DBG_PORT_IDSP_VIN4_SECT_ID          (1)
#define DBG_PORT_IDSP_VIN5_SECT_ID          (1)
#define DBG_PORT_IDSP_VIN_ENB_BASE_ADDR     (0xED1C0000)
#elif defined (CONFIG_SOC_H32)
#define DBG_PORT_IDSP_ADDR_EXT_BASE_ADDR    (0xED1C8000)
#define DBG_PORT_IDSP_VIN0_SECT_ID          (1)
#define DBG_PORT_IDSP_VIN1_SECT_ID          (10)
#define DBG_PORT_IDSP_VIN2_SECT_ID          (11)
#define DBG_PORT_IDSP_VIN3_SECT_ID          (1)
#define DBG_PORT_IDSP_VIN4_SECT_ID          (1)
#define DBG_PORT_IDSP_VIN5_SECT_ID          (1)
#define DBG_PORT_IDSP_VIN_ENB_BASE_ADDR     (0xED1C0000)
#elif defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define DBG_PORT_IDSP_ADDR_EXT_BASE_ADDR    (0x20ED1E8000)
#define DBG_PORT_IDSP_VIN0_SECT_ID          (1)
#define DBG_PORT_IDSP_VIN1_SECT_ID          (2)
#define DBG_PORT_IDSP_VIN2_SECT_ID          (3)
#define DBG_PORT_IDSP_VIN3_SECT_ID          (4)
#define DBG_PORT_IDSP_VIN4_SECT_ID          (5)
#define DBG_PORT_IDSP_VIN5_SECT_ID          (6)
#define DBG_PORT_IDSP_VIN6_SECT_ID          (7)
#define DBG_PORT_IDSP_VIN7_SECT_ID          (8)
#define DBG_PORT_IDSP_VIN8_SECT_ID          (9)
#define DBG_PORT_IDSP_VIN9_SECT_ID          (10)
#define DBG_PORT_IDSP_VIN10_SECT_ID         (11)
#define DBG_PORT_IDSP_VIN11_SECT_ID         (12)
#define DBG_PORT_IDSP_VIN12_SECT_ID         (13)
#define DBG_PORT_IDSP_VIN13_SECT_ID         (14)
#define DBG_PORT_IDSP_VIN_ENB_BASE_ADDR     (0x20ED1E0000)
#else //CV2FS
#define DBG_PORT_IDSP_ADDR_EXT_BASE_ADDR    (0xED1C8000)
#define DBG_PORT_IDSP_VIN0_SECT_ID          (1)
#define DBG_PORT_IDSP_VIN1_SECT_ID          (10)
#define DBG_PORT_IDSP_VIN2_SECT_ID          (11)
#define DBG_PORT_IDSP_VIN3_SECT_ID          (12)
#define DBG_PORT_IDSP_VIN4_SECT_ID          (13)
#define DBG_PORT_IDSP_VIN5_SECT_ID          (19)
#define DBG_PORT_IDSP_VIN_ENB_BASE_ADDR     (0xED1C0000)
#endif

static ULONG dsp_idsp_base_virt_addr = 0;
static ULONG dsp_idsp_ctrl0_virt_addr = 0;

typedef struct {
    uint32_t sw_reset:1;
    uint32_t enable:1;
    uint32_t output_enable:1;
    uint32_t bits_per_pixel:3;
    uint32_t reserved:26;
} dsp_osal_idsp_ctrl0_t;

void dsp_osal_delay_cycles(UINT32 Delay)
{
    volatile UINT32 Tmp = Delay;

    while(Tmp > 0U) {
        Tmp--;
    }
}

void dsp_osal_dbgport_get_vin_enable_status(UINT32 VinId, UINT32 *pEnable)
{
    volatile UINT32 *pU32Val;
    volatile dsp_osal_idsp_ctrl0_t *pIdspCtrl0;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    uint64_t phys_addr;
#else
    uint32_t phys_addr;
#endif
    void __iomem *virt;
    uint32_t backup_addr;
    uint32_t addr;

    if (dsp_idsp_base_virt_addr == 0) {
        phys_addr = DBG_PORT_IDSP_ADDR_EXT_BASE_ADDR;
        virt = ioremap(phys_addr, sizeof(UINT32));
        if (virt == NULL) {
            dsp_osal_printU5("[ERR] dsp_osal_dbgport_get_vin_enable_status : mmap_device_memory fail line[%d]", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            dsp_osal_typecast(&dsp_idsp_base_virt_addr, &virt);
        }
    }

    if (dsp_idsp_ctrl0_virt_addr == 0) {
        phys_addr = DBG_PORT_IDSP_VIN_ENB_BASE_ADDR;
        virt = ioremap(phys_addr, sizeof(dsp_osal_idsp_ctrl0_t));
        if (virt == NULL) {
            dsp_osal_printU5("[ERR] dsp_osal_dbgport_get_vin_enable_status : mmap_device_memory fail line[%d]", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            dsp_osal_typecast(&dsp_idsp_ctrl0_virt_addr, &virt);
        }
    }

    dsp_osal_typecast(&pU32Val, &dsp_idsp_base_virt_addr);
    backup_addr = *pU32Val;

    /* switch idsp section */
    if (VinId == 1U/*AMBA_VIN_CHANNEL1*/) {
        addr = (uint32_t)DBG_PORT_IDSP_VIN1_SECT_ID << 12U;
    } else if (VinId == 2U/*AMBA_VIN_CHANNEL2*/) {
        addr = (uint32_t)DBG_PORT_IDSP_VIN2_SECT_ID << 12U;
    } else if (VinId == 3U/*AMBA_VIN_CHANNEL3*/) {
        addr = (uint32_t)DBG_PORT_IDSP_VIN3_SECT_ID << 12U;
    } else if (VinId == 4U/*AMBA_VIN_CHANNEL4*/) {
        addr = (uint32_t)DBG_PORT_IDSP_VIN4_SECT_ID << 12U;
    } else if (VinId == 5U/*AMBA_VIN_CHANNEL5*/) {
        addr = (uint32_t)DBG_PORT_IDSP_VIN5_SECT_ID << 12U;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    } else if (VinId == 6U/*AMBA_VIN_CHANNEL6*/) {
        addr = (uint32_t)DBG_PORT_IDSP_VIN6_SECT_ID << 12U;
    } else if (VinId == 7U/*AMBA_VIN_CHANNEL7*/) {
        addr = (uint32_t)DBG_PORT_IDSP_VIN7_SECT_ID << 12U;
    } else if (VinId == 8U/*AMBA_VIN_CHANNEL8*/) {
        addr = (uint32_t)DBG_PORT_IDSP_VIN8_SECT_ID << 12U;
    } else if (VinId == 9U/*AMBA_VIN_CHANNEL9*/) {
        addr = (uint32_t)DBG_PORT_IDSP_VIN9_SECT_ID << 12U;
    } else if (VinId == 10U/*AMBA_VIN_CHANNEL10*/) {
        addr = (uint32_t)DBG_PORT_IDSP_VIN10_SECT_ID << 12U;
    } else if (VinId == 11U/*AMBA_VIN_CHANNEL11*/) {
        addr = (uint32_t)DBG_PORT_IDSP_VIN11_SECT_ID << 12U;
    } else if (VinId == 12U/*AMBA_VIN_CHANNEL12*/) {
        addr = (uint32_t)DBG_PORT_IDSP_VIN12_SECT_ID << 12U;
    } else if (VinId == 13U/*AMBA_VIN_CHANNEL13*/) {
        addr = (uint32_t)DBG_PORT_IDSP_VIN13_SECT_ID << 12U;
#endif
    } else { /* default section to parse */
        addr = (uint32_t)DBG_PORT_IDSP_VIN0_SECT_ID << 12U;
    }
    *pU32Val = addr;
    dsp_osal_delay_cycles(1000);

    dsp_osal_typecast(&pIdspCtrl0, &dsp_idsp_ctrl0_virt_addr);
    *pEnable = pIdspCtrl0->enable;

    *pU32Val = backup_addr;
    dsp_osal_delay_cycles(1000);
}

#if defined (CONFIG_SOC_CV22)
#define DBG_PORT_RCT_CLK_CTRL_BASE_ADDR     (0xED08008C)
#define DBG_PORT_RCT_CLK_DSP_MASK           (0x3144U)
#define DBG_PORT_RCT_CLK_MCTF_MASK          (0x10000U)
#define DBG_PORT_RCT_CLK_CODEC_MASK         (0x10U)
#define DBG_PORT_RCT_CLK_HEVC_MASK          (0xC000U)
#elif defined (CONFIG_SOC_CV2)
#define DBG_PORT_RCT_CLK_CTRL_BASE_ADDR     (0xED08008C)
#define DBG_PORT_RCT_CLK_DSP_MASK           (0x3144U)
#define DBG_PORT_RCT_CLK_MCTF_MASK          (0x10000U)
#define DBG_PORT_RCT_CLK_CODEC_MASK         (0x10U)
#define DBG_PORT_RCT_CLK_HEVC_MASK          (0xC000U)
#elif defined (CONFIG_SOC_CV25)
#define DBG_PORT_RCT_CLK_CTRL_BASE_ADDR     (0xED08008C)
#define DBG_PORT_RCT_CLK_DSP_MASK           (0x3144U)
#define DBG_PORT_RCT_CLK_MCTF_MASK          (0x10000U)
#define DBG_PORT_RCT_CLK_CODEC_MASK         (0x10U)
#define DBG_PORT_RCT_CLK_HEVC_MASK          (0xC000U)
#elif defined (CONFIG_SOC_H32)
#define DBG_PORT_RCT_CLK_CTRL_BASE_ADDR     (0xED08008C)
#define DBG_PORT_RCT_CLK_DSP_MASK           (0x3144U)
#define DBG_PORT_RCT_CLK_MCTF_MASK          (0x10000U)
#define DBG_PORT_RCT_CLK_CODEC_MASK         (0x10U)
#define DBG_PORT_RCT_CLK_HEVC_MASK          (0xC000U)
#elif defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define DBG_PORT_RCT_CLK_CTRL_BASE_ADDR     (0x20ED08008C)
#define DBG_PORT_RCT_CLK_DSP_MASK           (0x4330101U)
#define DBG_PORT_RCT_CLK_MCTF_MASK          (0x0U)
#define DBG_PORT_RCT_CLK_CODEC_MASK         (0x3E3EU)
#define DBG_PORT_RCT_CLK_HEVC_MASK          (0xC0C0U)
#else //CV2FS
#define DBG_PORT_RCT_CLK_CTRL_BASE_ADDR     (0xED08008C)
#define DBG_PORT_RCT_CLK_DSP_MASK           (0x3144U)
#define DBG_PORT_RCT_CLK_MCTF_MASK          (0x10000U)
#define DBG_PORT_RCT_CLK_CODEC_MASK         (0x10U)
#define DBG_PORT_RCT_CLK_HEVC_MASK          (0xC000U)
#endif

static ULONG dsp_rtc_clk_ctrl_virt_addr = 0;

UINT32 dsp_osal_dbgport_get_clk_enable_status(UINT32 ClkId, UINT32 *pEnable)
{
    UINT32 Rval = DSP_ERR_NONE;
    volatile UINT32 *pU32Val;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    uint64_t phys_addr;
#else
    uint32_t phys_addr;
#endif
    void __iomem *virt;
    UINT32 FeatureMask;

    if (dsp_rtc_clk_ctrl_virt_addr == 0) {
        phys_addr = DBG_PORT_RCT_CLK_CTRL_BASE_ADDR;
        virt = ioremap(phys_addr, sizeof(UINT32));
        if (virt == NULL) {
            dsp_osal_printU5("[ERR] dsp_osal_dbgport_get_clk_enable_status : mmap_device_memory fail line[%d]", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            dsp_osal_typecast(&dsp_rtc_clk_ctrl_virt_addr, &virt);
        }
    }

    dsp_osal_typecast(&pU32Val, &dsp_rtc_clk_ctrl_virt_addr);

    switch ((UINT64)ClkId) {
    case AMBA_SYS_FEATURE_DSP:
        FeatureMask = DBG_PORT_RCT_CLK_DSP_MASK;
        break;
    case AMBA_SYS_FEATURE_MCTF:
        FeatureMask = DBG_PORT_RCT_CLK_MCTF_MASK;
        break;
    case AMBA_SYS_FEATURE_CODEC:
        FeatureMask = DBG_PORT_RCT_CLK_CODEC_MASK;
        break;
    case AMBA_SYS_FEATURE_HEVC:
        FeatureMask = DBG_PORT_RCT_CLK_HEVC_MASK;
        break;
    default:
        FeatureMask = 0U;
        break;
    }

    if ((*pU32Val & FeatureMask) == FeatureMask) {
        *pEnable = 1U;
    } else {
        *pEnable = 0U;
    }

    return Rval;
}

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
UINT32 dsp_osal_dbgport_get_reset_status_info(const UINT32 Idx, UINT32 *pAddr, UINT32 *pDefValue)
{
    UINT32 Rval = DSP_ERR_NONE;

    /*
        idsp        0   0xED1CFFC0  0x0
        Idsp-core   1   0xED1CFFC4  0x0
        smem        2   0xED05E00C  0xBEA8
        vdsp        3   0xED020308  0xBABEFACE
        Vout0-mixer 4   0xE0008214  0x7F
        Vout0-core  5   0xE000837C  0x7F
        Vout0-vo    6   0xE0008380  0xFF
        Vout0-byte  7   0xE0008384  0x1FF
        Vout1-mixer 4   0xE0008514  0x7F
        Vout1-core  5   0xE000867C  0x7F
        Vout1-vo    6   0xE0008680  0xFF
        Vout1-byte  7   0xE0008684  0x1FF
    */

    static const UINT32 DspResetStatusAddrTable[DSP_RESET_STATUS_NUM + DSP_RESET_STATUS_VOUT_MIXER] = {
            [DSP_RESET_STATUS_IDSP]                                     = 0xED1CFFC0U,
            [DSP_RESET_STATUS_IDSP_CORE]                                = 0xED1CFFC4U,
            [DSP_RESET_STATUS_SMEM]                                     = 0xED05E00CU,
            [DSP_RESET_STATUS_VDSP]                                     = 0xED020308U,
            [DSP_RESET_STATUS_VOUT_MIXER]                               = 0xE0008214U,
            [DSP_RESET_STATUS_VOUT_CORE]                                = 0xE000837CU,
            [DSP_RESET_STATUS_VOUT_VO]                                  = 0xE0008380U,
            [DSP_RESET_STATUS_VOUT_BYTE]                                = 0xE0008384U,
            [DSP_RESET_STATUS_VOUT_MIXER + DSP_RESET_STATUS_VOUT_MIXER] = 0xE0008514U,
            [DSP_RESET_STATUS_VOUT_CORE  + DSP_RESET_STATUS_VOUT_MIXER] = 0xE000867CU,
            [DSP_RESET_STATUS_VOUT_VO    + DSP_RESET_STATUS_VOUT_MIXER] = 0xE0008680U,
            [DSP_RESET_STATUS_VOUT_BYTE  + DSP_RESET_STATUS_VOUT_MIXER] = 0xE0008684U,
    };

    static const UINT32 DspResetStatusDefValTable[DSP_RESET_STATUS_NUM + DSP_RESET_STATUS_VOUT_MIXER] = {
            [DSP_RESET_STATUS_IDSP]                                     = 0x0U,
            [DSP_RESET_STATUS_IDSP_CORE]                                = 0x0U,
            [DSP_RESET_STATUS_SMEM]                                     = 0xBEA8U,
            [DSP_RESET_STATUS_VDSP]                                     = 0xBABEFACEU,
            [DSP_RESET_STATUS_VOUT_MIXER]                               = 0x7FU,
            [DSP_RESET_STATUS_VOUT_CORE]                                = 0x7FU,
            [DSP_RESET_STATUS_VOUT_VO]                                  = 0xFFU,
            [DSP_RESET_STATUS_VOUT_BYTE]                                = 0x1FFU,
            [DSP_RESET_STATUS_VOUT_MIXER + DSP_RESET_STATUS_VOUT_MIXER] = 0x7FU,
            [DSP_RESET_STATUS_VOUT_CORE  + DSP_RESET_STATUS_VOUT_MIXER] = 0x7FU,
            [DSP_RESET_STATUS_VOUT_VO    + DSP_RESET_STATUS_VOUT_MIXER] = 0xFFU,
            [DSP_RESET_STATUS_VOUT_BYTE  + DSP_RESET_STATUS_VOUT_MIXER] = 0x1FFU,
    };

    if (Idx < (DSP_RESET_STATUS_NUM + DSP_RESET_STATUS_VOUT_MIXER)) {
        *pAddr = DspResetStatusAddrTable[Idx];
        *pDefValue = DspResetStatusDefValTable[Idx];
    } else {
        Rval = DSP_ERR_0000;
    }
    return Rval;
}
#endif

/******************************************************************************/
/*                                  Event                                     */
/******************************************************************************/
static uint32_t dsp_evnt_data_q_init = 0;
osal_mq_t dsp_evnt_data_q;
#define DSP_EVENT_DATA_Q_DEPTH  (128U)

typedef struct {
    uint32_t MaxHdlrNum;
    uint32_t UsedHdlrNum;
} osal_evnt_cfg_t;
osal_evnt_cfg_t dsp_evnt_cfg[LL_EVENT_NUM_ALL];
static void *pDspEvntDataQ = NULL;

UINT32 dsp_osal_evnt_data_q_init(void)
{
    static char dsp_evnt_data_q_name[] = "DspEvntDataQ";
    uint32_t Rval = DSP_ERR_NONE;

    if (dsp_evnt_data_q_init == 0) {
        (void)dsp_osal_memset(dsp_evnt_cfg, 0, sizeof(osal_evnt_cfg_t)*LL_EVENT_NUM_ALL);
        pDspEvntDataQ = kzalloc(DSP_EVENT_DATA_Q_DEPTH*sizeof(dsp_evnt_data_t), GFP_KERNEL);
        if (pDspEvntDataQ == NULL) {
            dsp_osal_printU5("[ERR] dsp_osal_evnt_data_q_init : DspEvntDataQ buf create fail", 0U, 0U, 0U, 0U, 0U);
        } else {
            Rval = dsp_osal_mq_create(&dsp_evnt_data_q, dsp_evnt_data_q_name, sizeof(dsp_evnt_data_t), pDspEvntDataQ, DSP_EVENT_DATA_Q_DEPTH*sizeof(dsp_evnt_data_t));
            if (Rval != OK) {
                dsp_osal_printU5("[ERR] dsp_osal_evnt_data_q_init : DspEvntDataQ create fail [0x%X]", Rval, 0U, 0U, 0U, 0U);
            } else {
                dsp_evnt_data_q_init = 1;
            }
        }
    }

    return Rval;
}

UINT32 dsp_osal_evnt_hdlr_cfg(UINT32 EvntId, UINT32 MaxNum, AMBA_DSP_EVENT_HANDLER_f *pEvntHdlrs)
{
    UINT32 Rval = DSP_ERR_NONE;

    Rval = dsp_osal_evnt_data_q_init();
    if (Rval == DSP_ERR_NONE) {
        if (EvntId < AMBA_DSP_EVENT_NUM) {
(void)pEvntHdlrs;
            dsp_evnt_cfg[EvntId].MaxHdlrNum = MaxNum;
        }
    }

    return Rval;
}

UINT32 dsp_osal_evnt_hdlr_reg(UINT32 EvntId, UINT32 Reg, AMBA_DSP_EVENT_HANDLER_f EvntHdlr)
{
    UINT32 Rval = DSP_ERR_NONE;

    Rval = dsp_osal_evnt_data_q_init();
    if (Rval == DSP_ERR_NONE) {
        if (EvntId < AMBA_DSP_EVENT_NUM) {
(void)EvntHdlr;
            if (Reg == 0U) {
                if (dsp_evnt_cfg[EvntId].UsedHdlrNum > 0U) {
                    dsp_evnt_cfg[EvntId].UsedHdlrNum--;
                }
            } else {
                if (dsp_evnt_cfg[EvntId].UsedHdlrNum < dsp_evnt_cfg[EvntId].MaxHdlrNum) {
                    dsp_evnt_cfg[EvntId].UsedHdlrNum++;
                } else {
                    dsp_osal_printU5("[ERR] dsp_osal_evnt_hdlr_reg : Reg exceed [%d]", dsp_evnt_cfg[EvntId].MaxHdlrNum, 0U, 0U, 0U, 0U);
                }
            }
        }
    }

    return Rval;
}

UINT32 dsp_osal_evnt_hdlr_reset(UINT32 EvntId)
{
    UINT32 Rval = DSP_ERR_NONE;

    Rval = dsp_osal_evnt_data_q_init();
    if (Rval == DSP_ERR_NONE) {
        if (EvntId < AMBA_DSP_EVENT_NUM) {
            Rval = dsp_osal_memset(&dsp_evnt_cfg[EvntId], 0, sizeof(osal_evnt_cfg_t));
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
        }
    }

    return Rval;
}

UINT32 dsp_osal_exec_evnt_user_cb(UINT32 HdlrNum, const AMBA_DSP_EVENT_HANDLER_f *pHdlr, UINT32 EvntId, const void *pEvntInfo, const UINT16 EventBufIdx)
{
    UINT32 Rval = DSP_ERR_NONE;
    dsp_evnt_data_t DspEvntData = {0};
    const dsp_evnt_data_t *pDspEvntData;
    osal_mq_info_t mq_info = {0};
    dsp_stat_buf_t *pEvtBuf = &DspEvntData.EventData;
    void *pVoid;
    ULONG phys_addr;
    UINT16 i;
    ULONG ULAddr;

(void)HdlrNum;

    Rval = dsp_osal_evnt_data_q_init();

    if (Rval == DSP_ERR_NONE) {
        if (pHdlr != NULL) {
            dsp_osal_printU5("[ERR] dsp_osal_exec_evnt_user_cb : Null FuncAddr", 0U, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
        }
    }

    if (Rval == DSP_ERR_NONE) {
        if (dsp_evnt_cfg[EvntId].UsedHdlrNum > 0U) {
            DspEvntData.EventId = (UINT16)EvntId;
            DspEvntData.PoolEmpty = 0U;

            if (pEvntInfo != NULL) {
                /* V2P */
                dsp_osal_typecast(&ULAddr, &pEvntInfo);
                Rval = dsp_osal_virt2phys(ULAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvtBuf->Addr = phys_addr;
                pEvtBuf->BufIdx = EventBufIdx;
            }

            switch (EvntId) {
            case AMBA_DSP_EVENT_LV_RAW_RDY:
            {
                AMBA_DSP_RAW_DATA_RDY_s *pEvntRaw = NULL;
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
                AMBA_DSP_RAW_DATA_RDY_EXT_s *pEvntRawExt = NULL;
#elif defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
                AMBA_DSP_RAW_DATA_RDY_EXT2_s *pEvntRawExt = NULL;
#endif

                dsp_osal_typecast(&pEvntRaw, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pEvntRaw->RawBuffer.BaseAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntRaw->RawBuffer.BaseAddr = phys_addr;

                Rval = dsp_osal_virt2phys(pEvntRaw->AuxBuffer.BaseAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntRaw->AuxBuffer.BaseAddr = phys_addr;
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
                dsp_osal_typecast(&pEvntRawExt, &pEvntInfo);

                Rval = dsp_osal_virt2phys(pEvntRawExt->EmbedBuffer.BaseAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntRawExt->EmbedBuffer.BaseAddr = phys_addr;
#elif defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
                dsp_osal_typecast(&pEvntRawExt, &pEvntInfo);

                Rval = dsp_osal_virt2phys(pEvntRawExt->EmbedBuffer.BaseAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntRawExt->EmbedBuffer.BaseAddr = phys_addr;

                Rval = dsp_osal_virt2phys(pEvntRawExt->Aux2Buffer.BaseAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntRawExt->Aux2Buffer.BaseAddr = phys_addr;
#endif
            }
                break;
            case AMBA_DSP_EVENT_LV_CFA_AAA_RDY:
            case AMBA_DSP_EVENT_LV_PG_AAA_RDY:
            case AMBA_DSP_EVENT_LV_HIST_AAA_RDY:
            case AMBA_DSP_EVENT_STL_CFA_AAA_RDY:
                break;
            case AMBA_DSP_EVENT_LV_YUV_DATA_RDY:
            {
                AMBA_DSP_YUV_DATA_RDY_EXTEND_s *pEvntYuv = NULL;

                dsp_osal_typecast(&pEvntYuv, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pEvntYuv->Buffer.BaseAddrY, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntYuv->Buffer.BaseAddrY = phys_addr;

                Rval = dsp_osal_virt2phys(pEvntYuv->Buffer.BaseAddrUV, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntYuv->Buffer.BaseAddrUV = phys_addr;
            }
                break;
            case AMBA_DSP_EVENT_LV_PYRAMID_RDY:
            {
                AMBA_DSP_PYMD_DATA_RDY_s *pEvntPymd = NULL;

                dsp_osal_typecast(&pEvntPymd, &pEvntInfo);

                /* V2P */
                for (i = 0U; i<AMBA_DSP_MAX_HIER_NUM; i++) {
                    Rval = dsp_osal_virt2phys(pEvntPymd->YuvBuf[i].BaseAddrY, &phys_addr);
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    pEvntPymd->YuvBuf[i].BaseAddrY = phys_addr;

                    Rval = dsp_osal_virt2phys(pEvntPymd->YuvBuf[i].BaseAddrUV, &phys_addr);
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    pEvntPymd->YuvBuf[i].BaseAddrUV = phys_addr;
                }
            }
                break;
            case AMBA_DSP_EVENT_VOUT_DATA_RDY:
            {
                AMBA_DSP_VOUT_DATA_INFO_s *pEvntVout = NULL;

                dsp_osal_typecast(&pEvntVout, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pEvntVout->YuvBuf.BaseAddrY, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntVout->YuvBuf.BaseAddrY = phys_addr;

                Rval = dsp_osal_virt2phys(pEvntVout->YuvBuf.BaseAddrUV, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntVout->YuvBuf.BaseAddrUV = phys_addr;
            }
                break;
            case AMBA_DSP_EVENT_VIDEO_DATA_RDY:
            {
                AMBA_DSP_ENC_PIC_RDY_s *pEvntPic = NULL;

                dsp_osal_typecast(&pEvntPic, &pEvntInfo);

                /* V2P */
                if (pEvntPic->PicSize != AMBA_DSP_ENC_END_MARK) {
                    Rval = dsp_osal_virt2phys(pEvntPic->StartAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    pEvntPic->StartAddr = phys_addr;
                }

                Rval = dsp_osal_virt2phys(pEvntPic->InputYAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntPic->InputYAddr = phys_addr;

                Rval = dsp_osal_virt2phys(pEvntPic->InputUVAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntPic->InputUVAddr = phys_addr;
            }
                break;
            case AMBA_DSP_EVENT_VIDEO_ENC_START:
            case AMBA_DSP_EVENT_VIDEO_ENC_STOP:
                break;
            case AMBA_DSP_EVENT_STL_RAW_RDY:
            {
                AMBA_DSP_STL_RAW_DATA_INFO_s *pEvntStlRaw = NULL;

                dsp_osal_typecast(&pEvntStlRaw, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pEvntStlRaw->Buf.BaseAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntStlRaw->Buf.BaseAddr = phys_addr;

                Rval = dsp_osal_virt2phys(pEvntStlRaw->AuxBuf.BaseAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntStlRaw->AuxBuf.BaseAddr = phys_addr;

                Rval = dsp_osal_virt2phys(pEvntStlRaw->Aux2Buf.BaseAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntStlRaw->Aux2Buf.BaseAddr = phys_addr;
            }
                break;
            case AMBA_DSP_EVENT_STL_YUV_DATA_RDY:
            {
                AMBA_DSP_YUV_IMG_BUF_s *pEvntStlYuv = NULL;

                dsp_osal_typecast(&pEvntStlYuv, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pEvntStlYuv->BaseAddrY, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntStlYuv->BaseAddrY = phys_addr;

                Rval = dsp_osal_virt2phys(pEvntStlYuv->BaseAddrUV, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntStlYuv->BaseAddrUV = phys_addr;
            }
                break;
            case AMBA_DSP_EVENT_JPEG_DATA_RDY:
            {
                AMBA_DSP_ENC_PIC_RDY_s *pEvntJpg = NULL;

                dsp_osal_typecast(&pEvntJpg, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pEvntJpg->StartAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntJpg->StartAddr = phys_addr;
            }
                break;
            case AMBA_DSP_EVENT_VIDEO_PATH_STATUS:
                break;
            case AMBA_DSP_EVENT_VIDEO_DEC_STATUS:
            {
                AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s *pEvntVdoDec = NULL;

                dsp_osal_typecast(&pEvntVdoDec, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pEvntVdoDec->BitsNextReadAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntVdoDec->BitsNextReadAddr = phys_addr;
            }
                break;
            case AMBA_DSP_EVENT_VIDEO_DEC_YUV_DISP_REPORT:
            {
                AMBA_DSP_VIDEO_DEC_YUV_DISP_REPORT_s *pEvntVdoDecDisp = NULL;

                dsp_osal_typecast(&pEvntVdoDecDisp, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pEvntVdoDecDisp->YAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntVdoDecDisp->YAddr = phys_addr;

                Rval = dsp_osal_virt2phys(pEvntVdoDecDisp->UVAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntVdoDecDisp->UVAddr = phys_addr;
            }
                break;
            case AMBA_DSP_EVENT_VIDEO_DEC_PIC_INFO:
            {
                AMBA_DSP_EVENT_VIDDEC_PIC_EX_s *pEvntVdoDecPic = NULL;

                dsp_osal_typecast(&pEvntVdoDecPic, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pEvntVdoDecPic->YAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntVdoDecPic->YAddr = phys_addr;

                Rval = dsp_osal_virt2phys(pEvntVdoDecPic->UVAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntVdoDecPic->UVAddr = phys_addr;
            }
                break;
            case AMBA_DSP_EVENT_STILL_DEC_STATUS:
            {
                AMBA_DSP_STLDEC_STATUS_s *pEvntStlDec = NULL;

                dsp_osal_typecast(&pEvntStlDec, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pEvntStlDec->YAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntStlDec->YAddr = phys_addr;

                Rval = dsp_osal_virt2phys(pEvntStlDec->UVAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntStlDec->UVAddr = phys_addr;
            }
                break;
            case AMBA_DSP_EVENT_STILL_DEC_YUV_DISP_REPORT:
            {
                AMBA_DSP_STILL_DEC_YUV_DISP_REPORT_s *pStlDecDisp = NULL;

                dsp_osal_typecast(&pStlDecDisp, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pStlDecDisp->YAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pStlDecDisp->YAddr = phys_addr;

                Rval = dsp_osal_virt2phys(pStlDecDisp->UVAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pStlDecDisp->UVAddr = phys_addr;
            }
                break;
            case AMBA_DSP_EVENT_VIN_POST_CONFIG:
                break;
            case AMBA_DSP_EVENT_ERROR:
                // TBD
                break;
            case AMBA_DSP_EVENT_LV_SYNC_JOB_RDY:
                break;
            case AMBA_DSP_EVENT_VIDEO_MV_DATA_RDY:
            {
                AMBA_DSP_ENC_MV_RDY_s *pEvntEncMv = NULL;

                dsp_osal_typecast(&pEvntEncMv, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pEvntEncMv->MvBufAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntEncMv->MvBufAddr = phys_addr;
            }
                break;
            case AMBA_DSP_EVENT_LV_SIDEBAND_RDY:
            {
                AMBA_DSP_SIDEBAND_DATA_RDY_s *pEvntSideBand = NULL;

                dsp_osal_typecast(&pEvntSideBand, &pEvntInfo);

                /* V2P */
                for (i = 0U; i<AMBA_DSP_MAX_TOKEN_ARRAY; i++) {
                    Rval = dsp_osal_virt2phys(pEvntSideBand->Info[i].PrivateInfoAddr, &phys_addr);
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    pEvntSideBand->Info[i].PrivateInfoAddr = phys_addr;
                }
            }
                break;
            case AMBA_DSP_EVENT_LV_LNDT_RDY:
            {
                AMBA_DSP_LNDT_DATA_RDY_s *pEvntLndt = NULL;

                dsp_osal_typecast(&pEvntLndt, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pEvntLndt->YuvBuf.BaseAddrY, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntLndt->YuvBuf.BaseAddrY = phys_addr;

                Rval = dsp_osal_virt2phys(pEvntLndt->YuvBuf.BaseAddrUV, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntLndt->YuvBuf.BaseAddrUV = phys_addr;
            }
                break;
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
            case AMBA_DSP_EVENT_LV_MAIN_Y12_RDY:
            {
                AMBA_DSP_YUV_DATA_RDY_s *pEvntMainY12 = NULL;

                dsp_osal_typecast(&pEvntMainY12, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pEvntMainY12->Buffer.BaseAddrY, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntMainY12->Buffer.BaseAddrY = phys_addr;

                Rval = dsp_osal_virt2phys(pEvntMainY12->Buffer.BaseAddrUV, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntMainY12->Buffer.BaseAddrUV = phys_addr;
            }
                break;
#endif
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
            case AMBA_DSP_EVENT_LV_MAIN_IR_RDY:
            {
                AMBA_DSP_YUV_DATA_RDY_s *pEvntMainIr = NULL;

                dsp_osal_typecast(&pEvntMainIr, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pEvntMainIr->Buffer.BaseAddrY, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntMainIr->Buffer.BaseAddrY = phys_addr;

                Rval = dsp_osal_virt2phys(pEvntMainIr->Buffer.BaseAddrUV, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntMainIr->Buffer.BaseAddrUV = phys_addr;
            }
                break;
#endif
#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS) && !defined (CONFIG_SOC_CV5) && !defined (CONFIG_SOC_CV52)
            case AMBA_DSP_EVENT_LV_INT_MAIN_RDY:
            {
                AMBA_DSP_YUV_DATA_RDY_s *pEvntIntMain = NULL;

                dsp_osal_typecast(&pEvntIntMain, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pEvntIntMain->Buffer.BaseAddrY, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntIntMain->Buffer.BaseAddrY = phys_addr;

                Rval = dsp_osal_virt2phys(pEvntIntMain->Buffer.BaseAddrUV, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntIntMain->Buffer.BaseAddrUV = phys_addr;
            }
                break;
#endif
            case AMBA_DSP_EVENT_STL_RAW_POSTPROC_RDY:
            {
                AMBA_DSP_RAW_DATA_RDY_s *pEvntRaw = NULL;

                dsp_osal_typecast(&pEvntRaw, &pEvntInfo);

                /* V2P */
                Rval = dsp_osal_virt2phys(pEvntRaw->RawBuffer.BaseAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntRaw->RawBuffer.BaseAddr = phys_addr;

                Rval = dsp_osal_virt2phys(pEvntRaw->AuxBuffer.BaseAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntRaw->AuxBuffer.BaseAddr = phys_addr;
            }
                break;
            case AMBA_DSP_EVENT_STL_PG_AAA_RDY:
                break;
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
            case AMBA_DSP_EVENT_TESTFRAME_RDY:
                break;
#endif
            case AMBA_DSP_EVENT_CLK_UPDATE_READY:
                break;
            default:
                Rval = DSP_ERR_0003;
                break;
            }

            if (Rval == OK) {
                Rval = dsp_osal_mq_query(&dsp_evnt_data_q, &mq_info);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                if (mq_info.FreeNum > 0U) {
                    pDspEvntData = &DspEvntData;
                    dsp_osal_typecast(&pVoid, &pDspEvntData);
                    Rval = dsp_osal_mq_send(&dsp_evnt_data_q, pVoid, 0/*TimeOut*/);
                    if (Rval != OK) {
                        dsp_osal_printU5("[ERR] dsp_osal_exec_evnt_user_cb : mq_send fail [0x%X]", Rval, 0U, 0U, 0U, 0U);
                    }
                } else {
                    Rval = DSP_ERR_0005;
                }
            }
        } else {
            Rval = DSP_ERR_0000;
        }
    }

    return Rval;
}

UINT32 dsp_osal_fetch_evnt_data(const void *pEvntData)
{
    UINT32 Rval = DSP_ERR_NONE;
    osal_mq_info_t mq_info = {0};
    dsp_evnt_data_t *pDspEvntData;
    void *pVoid;

    Rval = dsp_osal_evnt_data_q_init();

    if (Rval == DSP_ERR_NONE) {
        if (pEvntData == NULL) {
            dsp_osal_printU5("[ERR] dsp_osal_fetch_evnt_data : Null Addr", 0U, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
        }
    }

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_mq_query(&dsp_evnt_data_q, &mq_info);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);

        if (mq_info.PendingNum > 0U) {
            dsp_osal_typecast(&pVoid, &pEvntData);
            Rval = dsp_osal_mq_recv(&dsp_evnt_data_q, pVoid, 0/*TimeOut*/);
            if (Rval != OK) {
                dsp_osal_printU5("[ERR] dsp_osal_fetch_evnt_data : mq_send fail [0x%X]", Rval, 0U, 0U, 0U, 0U);
            } else {
                dsp_osal_typecast(&pDspEvntData, &pEvntData);
                pDspEvntData->EvntDataValid = 1U;

                Rval = dsp_osal_memset(&mq_info, 0, sizeof(osal_mq_info_t));
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                Rval = dsp_osal_mq_query(&dsp_evnt_data_q, &mq_info);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                if (mq_info.PendingNum == 0U) {
                    pDspEvntData->PoolEmpty = 1U;
                }
                //dsp_osal_printU5("dsp_osal_fetch_evnt_data : mq_recv[%d] valid[%d] Empty[%d]", pDspEvntData->EventId, pDspEvntData->EvntDataValid, pDspEvntData->PoolEmpty, 0U, 0U);
            }
        } else {
            dsp_osal_typecast(&pDspEvntData, &pEvntData);
            pDspEvntData->EvntDataValid = 0U;
            pDspEvntData->PoolEmpty = 1U;
            //dsp_osal_printU5("dsp_osal_fetch_evnt_data : EmptyPool Valid[%d]", pDspEvntData->EvntDataValid, 0U, 0U, 0U, 0U);
        }
    }

    return Rval;
}

/******************************************************************************/
/*                                  AAA transfer                              */
/******************************************************************************/
UINT32 dsp_osal_aaa_transfer(const void *pEvntData)
{
    UINT32 Rval = DSP_ERR_NONE;
    const AMBA_DSP_EVENT_3A_TRANSFER_s *pAaaTransfer = NULL;

    dsp_osal_typecast(&pAaaTransfer, &pEvntData);
    if (pAaaTransfer->Type == DSP_3A_VIDEO) {
        Rval = TransferCFAAaaStatData(pEvntData);
    } else if (pAaaTransfer->Type == DSP_3A_VIDEO_PG) {
        Rval = TransferPGAaaStatData(pEvntData);
    } else if ((pAaaTransfer->Type == DSP_HIST_VIDEO) ||
               (pAaaTransfer->Type == DSP_HIST_VIDEO_RGBIR)) {
        Rval = TransCFAHistStatData(pEvntData);
    } else {
        dsp_osal_printU5("AaaTransferType[%u] is not supported", pAaaTransfer->Type, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

/******************************************************************************/
/*                                  Utilities                                 */
/******************************************************************************/
uint32_t dsp_osal_str_cmp(const char *pString0, const char *pString1)
{
    return (uint32_t)OSAL_StringCompare(pString0, pString1, OSAL_StringLength(pString0));
}

uint32_t dsp_osal_str_to_u32(const char *pString, uint32_t *pVal)
{
    return OSAL_StringToUInt32(pString, pVal);
}

uint32_t dsp_osal_u32_to_str(char *pString, uint32_t str_len, uint32_t val, uint32_t radix)
{
    return OSAL_UInt32ToStr(pString, str_len, val, radix);
}

uint32_t dsp_osal_str_print_u32(char *pBuffer, uint32_t BufferSize, const char *pFmtString, uint32_t Count, const uint32_t *pArgs)
{
    return OSAL_StringPrintUInt32(pBuffer, BufferSize, pFmtString, Count, pArgs);
}

uint32_t dsp_osal_str_append(char *pBuffer, uint32_t BufferSize, const char *pSource)
{
    uint32_t Rval = DSP_ERR_NONE;

    OSAL_StringAppend(pBuffer, BufferSize, pSource);

    return Rval;
}

uint32_t dsp_osal_str_append_u32(char *pBuffer, uint32_t BufferSize, uint32_t Value, uint32_t Radix)
{
    uint32_t Rval = DSP_ERR_NONE;

    OSAL_StringAppendUInt32(pBuffer, BufferSize, Value, Radix);

    return Rval;
}

uint32_t dsp_osal_str_copy(char *pBuffer, uint32_t BufferSize, const char *pSource)
{
    uint32_t Rval = DSP_ERR_NONE;

    OSAL_StringCopy(pBuffer, BufferSize, pSource);

    return Rval;
}

/******************************************************************************/
/*                                  Resource                                 */
/******************************************************************************/
uint32_t dsp_query_buf_info(uint32_t phys_Addr, uint32_t buf_size, uint32_t *is_cached)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t i;
    uint32_t pool_addr_start, pool_addr_end;
    uint32_t buf_addr_start, buf_addr_end;
    uint32_t exit_i_loop;

    exit_i_loop = 0U;
    for (i = 0U; ((i < DSP_BUF_POOL_NUM) && (exit_i_loop == 0U)); i++) {
        if ((osal_buf_pool[i].Base == 0UL) ||
            (osal_buf_pool[i].size == 0UL)) {
            continue;
        }
        pool_addr_start = osal_buf_pool[i].PhysBase;
        pool_addr_end = (pool_addr_start + osal_buf_pool[i].size) - 1U;
        buf_addr_start = phys_Addr;
        buf_addr_end = (buf_addr_start + buf_size) - 1U;

        if ((buf_addr_start >= pool_addr_start) &&
            (buf_addr_end <= pool_addr_end)) {
            *is_cached = osal_buf_pool[i].is_cached;
            exit_i_loop = 1U;
        }
    }

    return Rval;
}

#define DTS_DSP_VIN_NODE    "/dspdrv/dspvin"
#define DTS_DSP_VOUT_NODE   "/dspdrv/dspvout"

uint32_t dsp_osal_kernel_init(struct device_node *np)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t i;
    const char *dev_name = of_node_full_name(np);
#define NODE_NAME_LEN   (32U)
    char node_name[NODE_NAME_LEN];
    struct device_node *sub_node = NULL;

    if((0 != dsp_osal_str_cmp(dev_name, "/dspdrv")) &&
       (0 != dsp_osal_str_cmp(dev_name, "dspdrv"))) {
        dsp_osal_printS5("DSP [%s] is not supported!", dev_name, NULL, NULL, NULL, NULL);
    } else {
        dsp_dev_node = np;

#if 0
        if (osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base == 0UL) {
            Rval = dsp_osal_alloc_prot_addr();
        }
        if (osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base == 0UL) {
            Rval = dsp_osal_alloc_stat_addr();
        }
        if ((ucode_bin_buf[UCODE_BIN_ORCODE].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCME].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCMD].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_DEFAULT].Base == 0UL)) {
            Rval = dsp_osal_alloc_ucode_addr();
        }
        if (osal_buf_pool[DSP_BUF_POOL_WORK].Base == 0UL) {
            Rval = dsp_osal_alloc_work_addr();
        }
        if (osal_buf_pool[DSP_BUF_POOL_DATA].Base == 0UL) {
            Rval = dsp_osal_alloc_data_addr();
        }
        if (osal_buf_pool[DSP_BUF_POOL_DBG_LOG].Base == 0UL) {
            Rval = dsp_osal_alloc_log_addr();
        }
#endif

#if defined (CONFIG_BUILD_MAL)
        Rval = dsp_osal_alloc_aux_addr();
#endif

        /* Read Code IRQ */
        for (i = 0U; i < DSP_MAX_IRQ_CODE_NUM; i++) {
            osal_irq_id.DspCode[i] = irq_of_parse_and_map(np, i);
            //printk("Code[%d] Irq[%d] \n", i, osal_irq_id.DspCode[i]);
        }

        /* Read Vin IRQ */
        for (i = 0U; i < DSP_MAX_IRQ_VIN_NUM; i++) {
            (void)dsp_osal_str_copy(node_name, NODE_NAME_LEN, DTS_DSP_VIN_NODE);
            (void)dsp_osal_str_append_u32(node_name, NODE_NAME_LEN, i, 10U);
            sub_node = of_find_node_by_path(node_name);
            if (of_device_is_available(sub_node)) {
                osal_irq_id.VinSof[i] = irq_of_parse_and_map(sub_node, 0);
                osal_irq_id.VinEof[i] = irq_of_parse_and_map(sub_node, 1);
                //printk("Vin[%d] Irq[%d %d] \n", i, osal_irq_id.VinSof[i], osal_irq_id.VinEof[i]);
            }
        }

        /* Read Vout IRQ */
        for (i = 0U; i < DSP_MAX_IRQ_VOUT_NUM; i++) {
            (void)dsp_osal_str_copy(node_name, NODE_NAME_LEN, DTS_DSP_VOUT_NODE);
            (void)dsp_osal_str_append_u32(node_name, NODE_NAME_LEN, i, 10U);
            sub_node = of_find_node_by_path(node_name);
            if (of_device_is_available(sub_node)) {
                osal_irq_id.Vout[i] = irq_of_parse_and_map(sub_node, 0);
                //printk("Vout[%d] Irq[%d] \n", i, osal_irq_id.Vout[i]);
            }
        }
    }

    return Rval;
}

void dsp_osal_kernel_exit(void)
{
    //DO NOTHING
}

