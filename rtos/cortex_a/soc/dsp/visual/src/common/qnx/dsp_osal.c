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

#include <stdint.h>
#include <devctl.h>
#include "dsp_osal.h"
#include "AmbaDSP_Def.h"
#include "AmbaDSP_Priv.h"
#include "ambadsp_ioctl.h"
#include "AmbaDSP_EventCtrl.h"

#if defined(USE_AMBA_KAL) || defined(USE_AMBA_KAL_EVNT)
#include "AmbaKAL.h"
#endif

#if defined(USE_AMBA_PRINT) || defined(USE_AMBA_PRINT_MODULE)
#include "AmbaPrint.h"
#endif

#ifdef USE_AMBA_UTILITY
#include "AmbaUtility.h"
#endif

#if defined (CONFIG_DEVICE_TREE_SUPPORT)
#include "AmbaFDT.h"  // for misra depress
#endif

uint8_t DspIrqStop = 0;
typedef struct {
    uint32_t DspCode[DSP_MAX_IRQ_CODE_NUM];
    uint32_t VinSof[DSP_MAX_IRQ_VIN_NUM];
    uint32_t VinEof[DSP_MAX_IRQ_VIN_NUM];
    uint32_t Vout[DSP_MAX_IRQ_VOUT_NUM];
} osal_irq_id_t;

#if defined (CONFIG_DEVICE_TREE_SUPPORT)
static ULONG dts_virt_addr = 0UL;
#endif

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
#ifdef USE_AMBA_PRINT
    OSAL_PrintFlush();
#endif
    //DO NOTHING
}

void dsp_osal_print_stop_and_flush(void)
{
#ifdef USE_AMBA_PRINT
    OSAL_PrintStopAndFlush();
#endif
    //DO NOTHING
}

void dsp_osal_printU5(const char *fmt,
                      uint32_t argv0,
                      uint32_t argv1,
                      uint32_t argv2,
                      uint32_t argv3,
                      uint32_t argv4)
{
#ifdef USE_AMBA_PRINT
    OSAL_PrintUInt5(fmt, argv0, argv1, argv2, argv3, argv4);
#else
    static const char *pCpuCoreName[4] = {"CA53_0", "CA53_1", "CA53_2", "CA53_3"};
    uint32_t systick = 0;
    (void)dsp_osal_get_sys_tick(&systick);

    printf("[%08d][%s]", systick, pCpuCoreName[SchedGetCpuNum()]);
    printf(fmt, argv0, argv1, argv2, argv3, argv4);
    printf("\n");
#endif
}

static inline void dsp_osal_printI5(const char *fmt,
                      int32_t argv0,
                      int32_t argv1,
                      int32_t argv2,
                      int32_t argv3,
                      int32_t argv4)
{
#ifdef USE_AMBA_PRINT
    OSAL_PrintInt5(fmt, argv0, argv1, argv2, argv3, argv4);
#else
    static const char *pCpuCoreName[4] = {"CA53_0", "CA53_1", "CA53_2", "CA53_3"};
    uint32_t systick = 0;
    (void)dsp_osal_get_sys_tick(&systick);

    printf("[%08d][%s]", systick, pCpuCoreName[SchedGetCpuNum()]);
    printf(fmt, argv0, argv1, argv2, argv3, argv4);
    printf("\n");
#endif
}

static inline void dsp_osal_printS5(const char *fmt,
                      const char *argv0,
                      const char *argv1,
                      const char *argv2,
                      const char *argv3,
                      const char *argv4)
{
#ifdef USE_AMBA_PRINT
    OSAL_PrintStr5(fmt, argv0, argv1, argv2, argv3, argv4);
#else
    static const char *pCpuCoreName[4] = {"CA53_0", "CA53_1", "CA53_2", "CA53_3"};
    uint32_t systick = 0;
    (void)dsp_osal_get_sys_tick(&systick);

    printf("[%08d][%s]", systick, pCpuCoreName[SchedGetCpuNum()]);
    printf(fmt, argv0, argv1, argv2, argv3, argv4);
    printf("\n");
#endif
}

void dsp_osal_module_printU5(uint32_t module_id, const char *fmt,
                             uint32_t argv0, uint32_t argv1,
                             uint32_t argv2, uint32_t argv3,
                             uint32_t argv4)
{
#ifdef USE_AMBA_PRINT_MODULE
    OSAL_ModulePrintUInt5((uint16_t)module_id, fmt, argv0, argv1, argv2, argv3, argv4);
#else
    static const char *pCpuCoreName[4] = {"CA53_0", "CA53_1", "CA53_2", "CA53_3"};
    uint32_t systick = 0;
    (void)dsp_osal_get_sys_tick(&systick);
(void)module_id;

    printf("[%08d][%s]", systick, pCpuCoreName[SchedGetCpuNum()]);
    printf(fmt, argv0, argv1, argv2, argv3, argv4);
    printf("\n");
#endif
}

void dsp_osal_module_printS5(uint32_t module_id, const char *fmt,
                             const char *argv0, const char *argv1,
                             const char *argv2, const char *argv3,
                             const char *argv4)
{
#ifdef USE_AMBA_PRINT_MODULE
    OSAL_ModulePrintStr5((uint16_t)module_id, fmt, argv0, argv1, argv2, argv3, argv4);
#else
    static const char *pCpuCoreName[4] = {"CA53_0", "CA53_1", "CA53_2", "CA53_3"};
    uint32_t systick = 0;
    (void)dsp_osal_get_sys_tick(&systick);
(void)module_id;

    printf("[%08d][%s]", systick, pCpuCoreName[SchedGetCpuNum()]);
    printf(fmt, argv0, argv1, argv2, argv3, argv4);
    printf("\n");
#endif
}

#if 0
void dsp_osal_module_printI5(uint32_t module_id, const char *fmt,
                             int32_t argv0, int32_t argv1,
                             int32_t argv2, int32_t argv3,
                             int32_t argv4)
{
#ifdef USE_AMBA_PRINT_MODULE
    OSAL_ModulePrintInt5((uint16_t)module_id, fmt, argv0, argv1, argv2, argv3, argv4);
#else
    static const char *pCpuCoreName[4] = {"CA53_0", "CA53_1", "CA53_2", "CA53_3"};
    uint32_t systick = 0;
    (void)dsp_osal_get_sys_tick(&systick);
(void)module_id;

    printf("[%08d][%s]", systick, pCpuCoreName[SchedGetCpuNum()]);
    printf(fmt, argv0, argv1, argv2, argv3, argv4);
    printf("\n");
#endif
}
#endif

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
#if 0
uint32_t dsp_osal_sem_init(osal_sem_t *sem, char *pName, int32_t pshared, uint32_t value)
{
#ifdef USE_AMBA_KAL
(void)pshared;
    return OSAL_SemaphoreCreate(sem, pName, value);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;

(void)pshared;
    if (sem == NULL) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_sem_init : sem == NULL Rval = 0x%x", Rval, 0U, 0U, 0U, 0U);
    } else {
        retcode = sem_init(sem, 1, value);
        if (retcode < 0) {
            Rval = DSP_ERR_0003;
            dsp_osal_printI5("[ERR] dsp_osal_sem_init : fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
        }
    }
    return Rval;
#endif
}

uint32_t dsp_osal_sem_deinit(osal_sem_t *sem)
{
#ifdef USE_AMBA_KAL
    return OSAL_SemaphoreDelete(sem);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;

    if (sem == NULL) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_sem_deinit : sem == NULL Rval = 0x%x", Rval, 0U, 0U, 0U, 0U);
    } else {
        retcode = sem_destroy(sem);
        if (retcode < 0) {
            Rval = DSP_ERR_0003;
            dsp_osal_printI5("[ERR] dsp_osal_sem_deinit : fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
        }
    }
    return Rval;
#endif
}

uint32_t dsp_osal_sem_post(osal_sem_t *sem)
{
#ifdef USE_AMBA_KAL
    return OSAL_SemaphoreGive(sem);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;

    if (sem == NULL) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_sem_post : sem == NULL Rval = 0x%x", Rval, 0U, 0U, 0U, 0U);
    } else {
        retcode = sem_post(sem);
        if (retcode == -1) {
            Rval = DSP_ERR_0003;
            dsp_osal_printI5("[ERR] dsp_osal_sem_post : fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
        }
    }
    return Rval;
#endif
}

uint32_t dsp_osal_sem_wait(osal_sem_t *sem, uint32_t timeout)
{
#ifdef USE_AMBA_KAL
    return OSAL_SemaphoreTake(sem, timeout);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;

(void)timeout;
    if (sem == NULL) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_sem_wait : sem == NULL Rval = 0x%x", Rval, 0U, 0U, 0U, 0U);
    } else {
        retcode = sem_wait(sem);
        if (retcode == -1) {
            Rval = DSP_ERR_0003;
            dsp_osal_printI5("[ERR] dsp_osal_sem_wait : fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
        }
    }
    return Rval;
#endif
}
#endif

static void dsp_osal_set_irq_stop(int32_t val)
{
    if (val > 0) {
        DspIrqStop = 1;
    } else {
        DspIrqStop = 0;
    }
    return;
}

uint32_t dsp_osal_mutex_init(osal_mutex_t *mutex, char *pName)
{
#ifdef USE_AMBA_KAL
    return OSAL_MutexCreate(mutex, pName);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;
    pthread_mutexattr_t attr;

(void)pName;
    // Invoke by Ambadsp_dev main
    // Using internal print to avoid AmbaPrint init has not been completed
    if (mutex == NULL) {
        Rval = DSP_ERR_0003;
        printf("[ERR] dsp_osal_mutex_init : mutex == NULL Rval = 0x%x\n", Rval);
    } else {
        retcode = pthread_mutex_init(mutex, &attr);
        if (retcode < 0) {
            Rval = DSP_ERR_0003;
            printf("[ERR] dsp_osal_mutex_init : pthread_mutex_init fail retcode = %d errno %d\n", retcode, errno);
        } else {
            retcode = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
            if (retcode < 0) {
                Rval = DSP_ERR_0003;
                printf("[ERR] dsp_osal_mutex_init : pthread_mutexattr_setpshared fail retcode = %d errno %d\n", retcode, errno);
            }
        }
    }
    return Rval;
#endif
}

#if 0
uint32_t dsp_osal_mutex_deinit(osal_mutex_t *mutex)
{
#ifdef USE_AMBA_KAL
    return OSAL_MutexDelete(mutex);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;

    if (mutex == NULL) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_mutex_deinit : mutex == NULL Rval = 0x%x", Rval, 0U, 0U, 0U, 0U);
    } else {
        retcode = pthread_mutex_destroy(mutex);
        if (retcode < 0) {
            Rval = DSP_ERR_0003;
            dsp_osal_printI5("[ERR] dsp_osal_mutex_deinit : fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
        }
    }
    return Rval;
#endif
}
#endif

uint32_t dsp_osal_mutex_lock(osal_mutex_t *mutex, uint32_t timeout)
{
#ifdef USE_AMBA_KAL
    return OSAL_MutexTake(mutex, timeout);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;
    struct timespec tm;
    uint64_t time;

    if (mutex == NULL) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_mutex_lock : mutex == NULL Rval = 0x%x", Rval, 0U, 0U, 0U, 0U);
    } else {
        retcode = clock_gettime(CLOCK_MONOTONIC, &tm);
        if (retcode < 0) {
            Rval = DSP_ERR_0003;
            dsp_osal_printI5("[ERR] dsp_osal_mutex_lock : clock_gettime fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
        } else {
            time = timespec2nsec(&tm);
            time += ((uint64_t)timeout*1000000U);
            nsec2timespec(&tm, time);
            retcode = pthread_mutex_timedlock_monotonic(mutex, &tm);
            if (retcode < 0) {
                Rval = DSP_ERR_0003;
                dsp_osal_printI5("[ERR] dsp_osal_mutex_lock : fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
            }
        }
    }
    return Rval;
#endif
}

uint32_t dsp_osal_mutex_unlock(osal_mutex_t *mutex)
{
#ifdef USE_AMBA_KAL
    return OSAL_MutexGive(mutex);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;

    if (mutex == NULL) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_mutex_unlock : mutex == NULL Rval = 0x%x", Rval, 0U, 0U, 0U, 0U);
    } else {
        retcode = pthread_mutex_unlock(mutex);
        if (retcode < 0) {
            Rval = DSP_ERR_0003;
            dsp_osal_printI5("[ERR] dsp_osal_mutex_unlock : fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
        }
    }
    return Rval;
#endif
}

uint32_t dsp_osal_eventflag_init(osal_even_t *event, const char *pName)
{
#ifdef USE_AMBA_KAL_EVNT
    return OSAL_EventFlagCreate(event, pName);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;

(void)pName;
    if (event == NULL) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_eventflag_init : event == NULL Rval = 0x%x", Rval, 0U, 0U, 0U, 0U);
    } else {
        atomic_set(&event->flag, 0);
        retcode = sem_init(&event->wait_queue, 1, 0);
        if (retcode == -1) {
            Rval = DSP_ERR_0003;
            dsp_osal_printI5("[ERR] dsp_osal_eventflag_init : fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
        }
    }
    return Rval;
#endif
}

uint32_t dsp_osal_eventflag_set(osal_even_t *event, uint32_t flag)
{
#ifdef USE_AMBA_KAL_EVNT
    return OSAL_EventFlagSet(event, flag);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;

    if (event == NULL) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_eventflag_set : event == NULL Rval = 0x%x", Rval, 0U, 0U, 0U, 0U);
    } else {
        atomic_set(&event->flag, flag);
        retcode = sem_post(&event->wait_queue);
        if (retcode == -1) {
            Rval = DSP_ERR_0003;
            dsp_osal_printI5("[ERR] dsp_osal_eventflag_set : fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
        }
    }
    return Rval;
#endif
}

uint32_t dsp_osal_eventflag_get(osal_even_t *event, uint32_t reqflag, uint32_t all, uint32_t clear, uint32_t *actflag, uint32_t timeout)
{
#ifdef USE_AMBA_KAL_EVNT
    return OSAL_EventFlagGet(event, reqflag, all, clear, actflag, timeout);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;
    struct timespec tm;
    uint64_t nesc_time;
    uint32_t whole_one = 1U;

    if ((event == NULL) || (actflag == NULL)) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_eventflag_get : event == NULL or actflag == NULL Rval = 0x%x", Rval, 0U, 0U, 0U, 0U);
    } else {
        retcode = clock_gettime(CLOCK_MONOTONIC, &tm);
        if (retcode < 0) {
            Rval = DSP_ERR_0003;
            dsp_osal_printI5("[ERR] dsp_osal_eventflag_get : clock_gettime fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
        }

        if (retcode == 0) {
            nesc_time = timespec2nsec(&tm);
            nesc_time += ((uint64_t)timeout*1000000UL);
            nsec2timespec(&tm, nesc_time);

            do {
                Rval = DSP_ERR_NONE;
                *actflag = event->flag;
                if (all > 0U) {
                    if ((*actflag & reqflag) == reqflag) {
                        /* all flag appeared */
                        if (clear > 0U) {
                            atomic_clr(&event->flag, reqflag);
                        }
                        whole_one = 0U;
                    } else {
                        if (timeout == 0U/*NO_WAIT*/) {
                            whole_one = 0U;
                            Rval = KAL_ERR_TIMEOUT;
                            *actflag = event->flag;
                        }
                        /* some flag are not appeared yet, keep wait */
                    }
                } else {
                    if ((*actflag & reqflag) > 0U) {
                        /* any flag appeared */
                        if (clear > 0U) {
                            atomic_clr(&event->flag, (*actflag & reqflag));
                        }
                        whole_one = 0U;
                    } else {
                        if (timeout == 0U/*NO_WAIT*/) {
                            whole_one = 0U;
                            Rval = KAL_ERR_TIMEOUT;
                            *actflag = event->flag;
                        }
                        /* some flags are not appeared yet, keep wait */
                    }
                }

                if (whole_one == 1U) {
                    retcode = sem_timedwait_monotonic(&event->wait_queue, &tm);
                    if (retcode == -1) {
                        if (errno != ETIMEDOUT) {
                            whole_one = 0;
                            Rval = DSP_ERR_0003;
                            dsp_osal_printI5("[ERR] dsp_osal_eventflag_get : fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
                        } else {
                            whole_one = 0U;
                            Rval = KAL_ERR_TIMEOUT;
                            *actflag = event->flag;
                        }
                    }
                }
            } while (whole_one == 1U);
        }
    }
    return Rval;
#endif
}

uint32_t dsp_osal_eventflag_clear(osal_even_t *event, uint32_t flag)
{
#ifdef USE_AMBA_KAL_EVNT
    return OSAL_EventFlagClear(event, flag);
#else
    uint32_t Rval = DSP_ERR_NONE;

    if (event == NULL) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_eventflag_clear : event == NULL or actflag == NULL Rval = 0x%x", Rval, 0U, 0U, 0U, 0U);
    } else {
        atomic_clr(&event->flag, flag);
    }
    return Rval;
#endif
}

uint32_t dsp_osal_spinlock(osal_spinlock_t *pSpinlock)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;
    int32_t aligFaultHandle = 0;

    if (pSpinlock == NULL) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_spinlock : spinlock == NULL Rval = 0x%x", Rval, 0U, 0U, 0U, 0U);
    } else {
        Rval = dsp_osal_memset(pSpinlock, 0, sizeof(osal_spinlock_t));
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        retcode = ThreadCtl(_NTO_TCTL_IO, &aligFaultHandle);
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_spinlock : ThreadCtl fail errno = %d", errno, 0, 0, 0, 0);
        }
        InterruptLock(pSpinlock);
    }
    return Rval;
}

uint32_t dsp_osal_spinunlock(osal_spinlock_t *pSpinlock)
{
    uint32_t Rval = DSP_ERR_NONE;

    if (pSpinlock == NULL) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_spinunlock : spinlock == NULL Rval = 0x%x", Rval, 0U, 0U, 0U, 0U);
    } else {
        InterruptUnlock(pSpinlock);
    }
    return Rval;
}

uint32_t dsp_osal_cmdlock(const osal_mutex_t *mutex, osal_spinlock_t *pSpinlock)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t retcode = 0U;

    retcode = dsp_osal_mutex_lock(mutex, DSP_CMD_LOCK_TIMEOUT);
    if (retcode != DSP_ERR_NONE) {
        dsp_osal_printU5("[ERR] dsp_osal_cmdlock : mtx lock fail[0x%X]", retcode, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
        retcode = dsp_osal_spinlock(pSpinlock);
        if (retcode != DSP_ERR_NONE) {
            dsp_osal_printU5("[ERR] dsp_osal_cmdlock : spinlock lock fail[0x%X]", retcode, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
        }
    }
    return Rval;
}

uint32_t dsp_osal_cmdunlock(const osal_mutex_t *mutex, osal_spinlock_t *pSpinlock)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t retcode = 0U;

    retcode = dsp_osal_spinunlock(pSpinlock);
    if (retcode != DSP_ERR_NONE) {
        dsp_osal_printU5("[ERR] dsp_osal_cmdunlock : spinunlock unlock fail[0x%X]", retcode, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
        retcode = dsp_osal_mutex_unlock(mutex);
        if (retcode != DSP_ERR_NONE) {
            dsp_osal_printU5("[ERR] dsp_osal_cmdunlock : mtx unlock fail[0x%X]", retcode, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
        }
    }
    return Rval;
}

#if 0
uint32_t dsp_osal_thread_create(osal_thread_t *thread, char *name, uint32_t priority,
                                osal_thread_entry_f entry, const void *arg,
                                void *stack_buff, uint32_t stack_size, uint32_t auto_start)
{
#ifdef USE_AMBA_KAL
    return OSAL_TaskCreate(thread, name, priority, entry, arg, stack_buff, stack_size, auto_start);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    pthread_attr_t attr;
    int32_t suspend_state;

    pthread_attr_init(&attr);
    if ((stack_buff != NULL) && (stack_size > 0)) {
        retcode = pthread_attr_setstack(&attr, stack_buff, stack_size);
        if (retcode != 0) {
            dsp_osal_printI5("[ERR] dsp_osal_thread_create : setstack fail retcode = %d", retcode, 0, 0, 0, 0);
            Rval = DSP_ERR_0003;
        }
    }

    if (retcode == 0) {
        suspend_state = (auto_start > 0)? PTHREAD_CREATE_NOT_SUSPENDED: PTHREAD_CREATE_SUSPENDED;
        retcode = pthread_attr_setsuspendstate_np(&attr, suspend_state);
        if (retcode != 0) {
            dsp_osal_printI5("[ERR] dsp_osal_thread_create : setsuspend fail retcode = %d", retcode, 0, 0, 0, 0);
            Rval = DSP_ERR_0003;
        }
    }

    if (retcode == 0) {
        retcode = pthread_create(thread, &attr, entry, &arg);
        if (retcode != 0) {
            dsp_osal_printI5("[ERR] dsp_osal_thread_create : create fail retcode = %d", retcode, 0, 0, 0, 0);
            Rval = DSP_ERR_0003;
        }
    }

    if (retcode == 0) {
        retcode = pthread_setname_np(*thread, (const char *)name);
        if (retcode != 0) {
            dsp_osal_printI5("[ERR] dsp_osal_thread_create : setname fail retcode = %d", retcode, 0, 0, 0, 0);
            Rval = DSP_ERR_0003;
        }
    }

    if (retcode == 0) {
        retcode = pthread_setschedprio(*thread, priority);
        if (retcode != 0) {
            dsp_osal_printI5("[ERR] dsp_osal_thread_create : setschedprio fail retcode = %d", retcode, 0, 0, 0, 0);
            Rval = DSP_ERR_0003;
        }
    }

    return Rval;
#endif
}

uint32_t dsp_osal_thread_delete(osal_thread_t *thread)
{
#ifdef USE_AMBA_KAL
    return OSAL_TaskDelete(thread);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;

    if (thread == NULL) {
        dsp_osal_printU5("[ERR] dsp_osal_thread_delete : Null address", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0003;
    } else {
        if (thread != NULL) {
            retcode = pthread_join(*thread, NULL);
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_thread_delete : fail retcode = %d", retcode, 0, 0, 0, 0);
                Rval = DSP_ERR_0003;
            } else {
                thread = NULL;
            }
        }
    }
    return Rval;
#endif
}

uint32_t dsp_osal_thread_should_stop(osal_thread_t *thread)
{
#ifdef USE_AMBA_KAL
    return OSAL_TaskSuspend(thread);
#else
(void)thread;
    return 0U;
#endif
}

uint32_t dsp_osal_thread_set_affinity(osal_thread_t *thread, uint32_t affinity)
{
#ifdef USE_AMBA_KAL
    return OSAL_TaskSetSmpAffinity(thread, affinity);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    pid_t pid = 0;
    uint32_t affinity_core_zero = 0x1;
    uint32_t new_affinity;

    if (thread == NULL) {
        dsp_osal_printU5("[ERR] dsp_osal_thread_set_affinity : Null addr", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0003;
    } else {
        new_affinity = (affinity > 0)? affinity: affinity_core_zero;

        /* Get process id */
        pid = getpid();
        procmgr_ability(pid, PROCMGR_AID_XTHREAD_THREADCTL, PROCMGR_AID_EOL);
        /* pid must be 0 based on QNX document */
        retcode = ThreadCtlExt(0/*pid*/, *thread, _NTO_TCTL_RUNMASK, (void *)new_affinity);
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_thread_set_affinity : fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
            Rval = DSP_ERR_0003;
        }
    }

    return Rval;
#endif
}

uint32_t dsp_osal_thread_resume(osal_thread_t *thread)
{
#ifdef USE_AMBA_KAL
    return OSAL_TaskResume(thread);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;

    if (thread == NULL) {
        dsp_osal_printU5("[ERR] dsp_osal_thread_resume : Null addr", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0003;
    } else {
        /* pid must be 0 based on QNX document */
        retcode = ThreadCtlExt(0/*pid*/, *thread, _NTO_TCTL_THREADS_CONT, 0);
        if (retcode != 0) {
            dsp_osal_printU5("[ERR] dsp_osal_thread_resume : fail retcode = %d", retcode, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        }
    }

    return Rval;
#endif
}
#endif

static inline uint32_t dsp_osal_mq_create(osal_mq_t *mq, char *name, uint32_t msg_size, void *msg_q_buf, uint32_t msg_q_buf_size)
{
#ifdef USE_AMBA_KAL
    return OSAL_MsgQueueCreate(mq, name, msg_size, msg_q_buf, msg_q_buf_size);
#else
    struct mq_attr attrs;
    uint32_t Rval = DSP_ERR_NONE;

(void)msg_q_buf;
    Rval = dsp_osal_memset(&attrs, 0, sizeof(attrs));
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    attrs.mq_maxmsg = msg_q_buf_size/msg_size;
    attrs.mq_msgsize = msg_size;

#if 0
    /* do not unlink so when pid1 terminated, pid2 still gets it */
    (void)mq_unlink(pMsgQueueName);
#endif
    /* mq_open returns a message queue descriptor */
    *mq = mq_open(name, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG, &attrs);
    /* call mq_setattr() later to change mq_attr */

    if (*mq == -1) {
        dsp_osal_printI5("[ERR] dsp_osal_mq_create : fail errno %d", errno, 0, 0, 0, 0);
        Rval = DSP_ERR_0003;
    }

    return Rval;
#endif
}

static inline uint32_t dsp_osal_mq_send(osal_mq_t *mq, void *msg, uint32_t time_out)
{
#ifdef USE_AMBA_KAL
    return OSAL_MsgQueueSend(mq, msg, time_out);
#else
    struct timespec abs_timeout;
    struct mq_attr attr;
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t msg_prio = 0;
    int32_t retcode;

    if (mq == NULL) {
        dsp_osal_printU5("[ERR] dsp_osal_mq_send : Null Addr", 0U, 0U, 0U, 0U, 0U);
    } else {
        retcode = mq_getattr(*mq, &attr);
        if (retcode != -1) {
            clock_gettime(CLOCK_MONOTONIC, &abs_timeout);
            abs_timeout.tv_sec += time_out/1000;
            abs_timeout.tv_nsec += (time_out%1000)*1000*1000;

            /* hard code msg_prio to 0 */
            retcode = mq_timedsend_monotonic(*mq, msg, attr.mq_msgsize, msg_prio, &abs_timeout);
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_mq_send : fail errno %d", errno, 0, 0, 0, 0);
                Rval = DSP_ERR_0003;
            }
        } else {
            dsp_osal_printI5("[ERR] dsp_osal_mq_send : mq_getattr fail errno %d", errno, 0, 0, 0, 0);
            Rval = DSP_ERR_0003;
        }
    }

    return Rval;
#endif
}

static inline uint32_t dsp_osal_mq_recv(osal_mq_t *mq, void *msg, uint32_t time_out)
{
#ifdef USE_AMBA_KAL
    return OSAL_MsgQueueReceive(mq, msg, time_out);
#else
    struct timespec abs_timeout;
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;
    struct mq_attr attr;
    uint32_t msec;

    if (mq == NULL) {
        dsp_osal_printU5("[ERR] dsp_osal_mq_recv : Null Addr", 0U, 0U, 0U, 0U, 0U);
    } else {
        retcode = mq_getattr(*mq, &attr);
        if (retcode != -1) {
            clock_gettime(CLOCK_MONOTONIC, &abs_timeout);
            msec = time_out%1000;

            abs_timeout.tv_nsec += (msec*1000)*1000;
            if (abs_timeout.tv_nsec >= 1000000000) {
                abs_timeout.tv_nsec -= 1000000000;
                abs_timeout.tv_sec += 1;
                abs_timeout.tv_sec += time_out/1000;
            } else {
                abs_timeout.tv_sec += time_out/1000;
            }

            /* mq_timedreceive returns number of bytes in the received message */
            retcode = mq_timedreceive_monotonic(*mq, msg, attr.mq_msgsize, NULL/*MsgPrio*/, &abs_timeout);
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_mq_recv : mq_timedreceive_monotonic fail errno %d", errno, 0, 0, 0, 0);
                Rval = DSP_ERR_0003;
            }
        } else {
            dsp_osal_printI5("[ERR] dsp_osal_mq_recv : mq_getattr fail errno %d", errno, 0, 0, 0, 0);
            Rval = DSP_ERR_0003;
        }
    }

    return Rval;
#endif
}

static inline uint32_t dsp_osal_mq_query(osal_mq_t *mq, osal_mq_info_t *mq_info)
{
    uint32_t Rval = DSP_ERR_NONE;
#ifdef USE_AMBA_KAL
    AMBA_KAL_MSG_QUEUE_INFO_s MsgQInfo = {0};

    Rval = OSAL_MsgQueueQuery(mq, &MsgQInfo);
    mq_info->PendingNum = MsgQInfo.NumEnqueued;
    mq_info->FreeNum = MsgQInfo.NumAvailable;

    return Rval;
#else
    struct mq_attr mq_attribute;
    int32_t retcode;

    retcode = mq_getattr(*mq, &mq_attribute);
    if (retcode == -1) {
        dsp_osal_printI5("[ERR] dsp_osal_mq_query : mq_getattr fail errno %d", errno, 0, 0, 0, 0);
        Rval = DSP_ERR_0003;
    } else {
        mq_info->PendingNum = mq_attribute.mq_curmsgs;
        mq_info->FreeNum = mq_attribute.mq_maxmsg - mq_attribute.mq_curmsgs;
    }

    return Rval;
#endif
}

uint32_t dsp_osal_sleep(uint32_t msec)
{
#ifdef USE_AMBA_KAL
    return OSAL_TaskSleep(msec);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;
    uint32_t sec, micro_sec;

    sec = msec/1000U;
    micro_sec = msec%1000;
    micro_sec *= 1000;

    retcode = sleep(sec);
    if (retcode == -1) {
        Rval = DSP_ERR_0003;
        dsp_osal_printI5("[ERR] dsp_osal_sleep : sleep fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
    } else {
        if (micro_sec > 0U) {
            retcode = usleep(msec*1000U);
            if (retcode == -1) {
                Rval = DSP_ERR_0003;
                dsp_osal_printI5("[ERR] dsp_osal_sleep : usleep fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
            }
        }
    }

    return Rval;
#endif
}

uint32_t dsp_osal_get_sys_tick(uint32_t *msec)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    struct timespec tp;

    retcode = clock_gettime(CLOCK_MONOTONIC, &tp);
    if (retcode == 0) {
        *msec = (uint32_t)(((uint64_t)tp.tv_sec * 1000UL) + ((uint64_t)tp.tv_nsec / 1000000UL));
    } else {
        Rval = DSP_ERR_0003;
        *msec = 0;
    }

    return Rval;
}

#if 0
uint32_t dsp_osal_cache_size_align(uint32_t size)
{
    return ((size) + AMBA_CACHE_LINE_SIZE - 1U) & ~(AMBA_CACHE_LINE_SIZE - 1U);
}

uint32_t dsp_osal_cache_addr_align(uint32_t addr)
{
    return addr & ~(AMBA_CACHE_LINE_SIZE - 1U);
}
#endif

static struct cache_ctrl dsp_cache_info = {0};
uint32_t dsp_osal_cache_clean(const void *ptr, uint32_t size)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;
    off_t offset;
    size_t contig_len;

    if (size != 0U) {
        if (ptr == NULL) {
            Rval = DSP_ERR_0003;
            dsp_osal_printU5("[ERR] dsp_osal_cache_clean : fail : ptr == NULL ", 0U, 0U, 0U, 0U, 0U);
        } else {
            contig_len = 0U;
            retcode = mem_offset(ptr, NOFD, 1, &offset, &contig_len);
            if (retcode < 0) {
                Rval = DSP_ERR_0003;
                dsp_osal_printI5("[ERR] dsp_osal_cache_clean : mem_offset fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
            } else {
                CACHE_FLUSH(&dsp_cache_info, ptr, (uint64_t)offset, size);
            }
        }
    }

    return Rval;
}

uint32_t dsp_osal_cache_invalidate(const void *ptr, uint32_t size)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;
    off_t offset;
    size_t contig_len;

    if (size != 0U) {
        if (ptr == NULL) {
            Rval = DSP_ERR_0003;
            dsp_osal_printU5("[ERR] dsp_osal_cache_invalidate : fail : ptr == NULL ", 0U, 0U, 0U, 0U, 0U);
        } else {
            contig_len = 0U;
            retcode = mem_offset(ptr, NOFD, 1, &offset, &contig_len);
            if (retcode < 0) {
                Rval = DSP_ERR_0003;
                dsp_osal_printI5("[ERR] dsp_osal_cache_invalidate : mem_offset fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
            } else {
                CACHE_INVAL(&dsp_cache_info, ptr, (uint64_t)offset, size);
            }
        }
    }

    return Rval;
}

#if 0
static inline uint32_t dsp_osal_is_cached_addr(const ULONG Virt, uint32_t *pCached)
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
#endif

uint32_t dsp_osal_virt2phys(const ULONG Virt, ULONG *pPhys)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t i, exit_i_loop;
    ULONG ul_pool_end_addr, offset;

    if (pPhys == NULL) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_virt2phys : Null addr", 0U, 0U, 0U, 0U, 0U);
    } else if (Virt == 0UL) {
        *pPhys = 0UL;
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

        if (exit_i_loop == 0U) {
            dsp_osal_printU5("[ERR] dsp_osal_virt2phys : fail for 0x%X%X", dsp_osal_get_u64msb((UINT64)Virt), dsp_osal_get_u64lsb((UINT64)Virt), 0U, 0U, 0U);
            *pPhys = (uint32_t)Virt;
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

#if 0
uint32_t dsp_osal_copy_to_user(void *to, const void *from, uint32_t n)
{
    uint32_t Rval = DSP_ERR_NONE;

    if ((to == NULL) || (from == NULL)) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_copy_to_user : fail Rval = 0x%x", Rval, 0U, 0U, 0U, 0U);
    } else {
        Rval = dsp_osal_memcpy(to, from, n);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
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
        Rval = dsp_osal_memcpy(to, from, n);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
    }
    return retcode;
}
#endif

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define PHYS_ADDR_MASK  (0xFFFFFFFFFFFFFFFFU)
#else
#define PHYS_ADDR_MASK  (0xFFFFFFFFU)
#endif

#define USE_OSAL_READL
#define USE_MEM_BLK
#define MMAP_MEM_BLK_SIZE   (128U)
typedef struct {
    uint8_t  Data[MMAP_MEM_BLK_SIZE];
} dsp_osal_mem_access_blk_t;

#ifdef USE_OSAL_READL
/* read from physical address */
/* From experiemnt QNX need address to be 128 alignment for mmap_device_memory */
/* hence it's not easy to use read/write physical address directly */
uint32_t dsp_osal_readl_relaxed(const void* reg)
{
#ifdef USE_MEM_BLK
    const volatile uint32_t *ptr;
    const dsp_osal_mem_access_blk_t *MemAccessBlk;
    const uint8_t *pU8;
    uint32_t value = 0U;
    int32_t retcode;
    uint64_t phys_addr;
    uint32_t offset;
    uint64_t len;
    void *virt_addr;
    uint32_t flag;

    if (reg == NULL) {
        dsp_osal_printU5("[ERR] dsp_osal_readl_relaxed : fail reg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        dsp_osal_typecast(&phys_addr, &reg);
        phys_addr = phys_addr & PHYS_ADDR_MASK;
        offset = (uint32_t)(phys_addr % MMAP_MEM_BLK_SIZE);
        len = sizeof(dsp_osal_mem_access_blk_t);
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_NOCACHE;

        virt_addr = mmap_device_memory(NULL, len, (int32_t)flag, 0, (phys_addr - offset));
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_readl_relaxed : mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
        } else {
            dsp_osal_typecast(&MemAccessBlk, &virt_addr);
            pU8 = &MemAccessBlk->Data[offset];
            dsp_osal_typecast(&ptr, &pU8);

            value = *ptr;

            retcode = munmap_device_memory(virt_addr, len);
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_readl_relaxed : munmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
            }
        }
    }
#else
    const volatile uint32_t *ptr;
    uint32_t value = 0U;
    int32_t retcode;
    uint64_t phys_addr;
    void *virt_addr;
    uint32_t flag;

    dsp_osal_printU5("Warning!  Address need 128 alignment", 0U, 0U, 0U, 0U, 0U);

    if (reg == NULL) {
        dsp_osal_printU5("[ERR] dsp_osal_readl_relaxed : fail reg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        dsp_osal_typecast(&phys_addr, &reg);
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;

        virt_addr = mmap_device_memory(NULL, sizeof(uint32_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_readl_relaxed : mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
        } else {
            dsp_osal_typecast(&ptr, &virt_addr);
            value = *ptr;

            retcode = munmap_device_memory(virt_addr, sizeof(uint32_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_readl_relaxed : munmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
            }
        }
    }
#endif
    return value;
}
#endif

//#define USE_OSAL_WRITEL
/* write to physical address */
/* From experiemnt QNX need address to be 128 alignment for mmap_device_memory */
/* hence it's not easy to use read/write physical address directly */
void dsp_osal_writel_relaxed(uint32_t val, const void *reg)
{
#ifdef USE_MEM_BLK
    uint32_t *ptr;
    const dsp_osal_mem_access_blk_t *MemAccessBlk;
    const uint8_t *pU8;
    uint64_t phys_addr;
    uint32_t offset;
    uint32_t len;
    void *virt_addr;
    int32_t retcode;
    uint32_t flag;

    if (reg == NULL) {
        dsp_osal_printU5("[ERR] dsp_osal_writel_relaxed : fail reg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        dsp_osal_typecast(&phys_addr, &reg);
        phys_addr = phys_addr & PHYS_ADDR_MASK;
        offset = (uint32_t)(phys_addr % MMAP_MEM_BLK_SIZE);
        len = (uint32_t)(sizeof(dsp_osal_mem_access_blk_t));
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;

        virt_addr = mmap_device_memory(NULL, len, (int32_t)flag, 0, (phys_addr - offset));
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_writel_relaxed : mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
        } else {
            dsp_osal_typecast(&MemAccessBlk, &virt_addr);
            pU8 = &MemAccessBlk->Data[offset];
            dsp_osal_typecast(&ptr, &pU8);

            *(volatile uint32_t*)ptr = val;

            retcode = munmap_device_memory(virt_addr, len);
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_writel_relaxed : munmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
            }
        }
    }
#else
    uint32_t *ptr;
    uint64_t phys_addr;
    void *virt_addr;
    int32_t retcode;
    uint32_t flag;

    dsp_osal_printU5("Warning!  Address need 128 alignment", 0U, 0U, 0U, 0U, 0U);

    if (reg == NULL) {
        dsp_osal_printU5("[ERR] dsp_osal_writel_relaxed : fail reg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        dsp_osal_typecast(&phys_addr, &reg);
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;

        virt_addr = mmap_device_memory(NULL, sizeof(uint32_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_writel_relaxed : mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
        } else {
            dsp_osal_typecast(&ptr, &virt_addr);
            *(volatile uint32_t*)ptr = val;

            retcode = munmap_device_memory(virt_addr, sizeof(uint32_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_writel_relaxed : munmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
            }
        }
    }
#endif
}

#if 0
void *dsp_osal_ioremap(uint32_t pa, uint32_t size)
{
    void *ptr = NULL;
    _Uintptrt io;

    if ((pa == 0U) || (size == 0U)) {
        dsp_osal_printU5("[ERR] dsp_osal_ioremap : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        io = mmap_device_io(size, pa);
        dsp_osal_typecast(&ptr, &io);
    }
    return ptr;
}

void dsp_osal_iounmap(void* va, uint32_t size)
{
    int32_t retcode;
    _Uintptrt io;

    dsp_osal_typecast(&io, &va);
    retcode = munmap_device_io(io, size);
    if (retcode == -1) {
        dsp_osal_printI5("[ERR] dsp_osal_iounmap : fail errno = %d", errno, 0, 0, 0, 0);
    }
}
#endif

#ifndef USE_ISR_EVENT_ATTACH
uint32_t dsp_osal_irq_config(uint32_t irq_id, const osal_irq_t *irq_cfg, osal_isr_entry_f hdlr, uint32_t arg)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;
(void)irq_cfg;

    retcode = InterruptAttach(irq_id, hdlr, NULL, 0, _NTO_INTR_FLAGS_TRK_MSK | _NTO_INTR_FLAGS_NO_UNMASK);
    if (retcode == -1) {
        dsp_osal_printI5("[ERR] dsp_osal_irq_config : fail errno = %d", errno, 0, 0, 0, 0);
        Rval = DSP_ERR_0003;
    }

    return Rval;
}

uint32_t dsp_osal_irq_enable(uint32_t irq_id)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;

    retcode = ThreadCtl(_NTO_TCTL_IO, 0);
    if (retcode == -1) {
        dsp_osal_printI5("[ERR] dsp_osal_irq_enable : ThreadCtl fail errno = %d", errno, 0, 0, 0, 0);
        Rval = DSP_ERR_0003;
    }

    if (retcode != -1) {
        retcode = InterruptUnmask(irq_id, -1);
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_irq_enable : fail errno = %d", errno, 0, 0, 0, 0);
            Rval = DSP_ERR_0003;
        }
    }

    return rval;
}

#if 0
uint32_t dsp_osal_irq_disable(uint32_t irq_id)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;

    retcode = InterruptMask(irq_id, -1);
    if (retcode == -1) {
        dsp_osal_printI5("[ERR] dsp_osal_irq_disable : fail errno = %d", errno, 0, 0, 0, 0);
        Rval = DSP_ERR_0003;
    }

    return Rval;
}
#endif

#else
#define IRQ_ID_INVALID  (0xFFFFFFFFU)
typedef struct {
    uint32_t            irq_id;
    uint32_t            irq_attache_id;
    osal_isr_entry_f    relay_hdlr;
} osal_irq_relay_usage_t;

#define MAX_VIN_ISR_NUM     (AMBA_DSP_MAX_VIN_NUM*2U)
#define MAX_VOUT_ISR_NUM    (AMBA_DSP_MAX_VOUT_NUM*2U)
#define MAX_VDSP_ISR_NUM    (4U) //vdsp0/1/2/3
#define MAX_DSPDRV_ISR_NUM  (MAX_VIN_ISR_NUM+MAX_VOUT_ISR_NUM+MAX_VDSP_ISR_NUM)

//#define USE_LOWEST_NUM_HIGHEST_PRIORITY
#define USE_HIGHEST_NUM_HIGHEST_PRIORITY
#define HIHGEST_PRIORITY            (255U)
#define DSP_ISR_TASK_PRIORITY       (3U)
static void *dsp_irq_thread(void *data)
{
    struct sigevent event;
    osal_irq_relay_usage_t *irq_data;
    uint32_t Rval;
    int32_t retcode;
    pthread_t thread_id;
    uint32_t priority;
    //struct sched_param sch_param;
    //int32_t policy;
    uint32_t DoWhile = 1U;

    thread_id = pthread_self();
#ifdef USE_LOWEST_NUM_HIGHEST_PRIORITY
    priority = DSP_ISR_TASK_PRIORITY;
#else
    priority = HIHGEST_PRIORITY - DSP_ISR_TASK_PRIORITY;
#endif

    retcode = pthread_setschedprio(thread_id, (int32_t)priority);
    if (retcode != 0) {
        dsp_osal_printI5("[ERR] dsp_osal_irq_config : schedprio fail retcode = %d", retcode, 0, 0, 0, 0);
    }

    retcode = ThreadCtlExt(0/*pid*/, 0/*Self*/, _NTO_TCTL_LOW_LATENCY, NULL);
    if (retcode != 0) {
        dsp_osal_printI5("[ERR] dsp_osal_irq_config : ThreadCtlExt fail retcode = %d", retcode, 0, 0, 0, 0);
    }

    //pthread_getschedparam(thread_id, &policy, &sch_param);
    //dsp_osal_printU5("IRQ [%d] priority[%d] current priority[%d] Policy[%d]", irq_usage[idx].irq_id, sch_param.sched_priority, sch_param.sched_curpriority, policy, 0U);
    //dsp_osal_printU5("IRQ thread[%d] Start [%d]", thread_id, idx, 0U, 0U, 0U);

    /* fill in "event" structure */
    Rval = dsp_osal_memset(&event, 0, sizeof(event));
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    //event.sigev_notify = SIGEV_INTR; //SIGEV_INTR_INIT will do this
    /* Enable the INTERRUPTEVENT ability */

    retcode = procmgr_ability(0/*Self*/, (uint32_t)PROCMGR_ADN_ROOT|(uint32_t)PROCMGR_AOP_ALLOW|(uint32_t)PROCMGR_AID_INTERRUPTEVENT, PROCMGR_AID_EOL);
    if (retcode != EOK) {
        dsp_osal_printI5("[ERR] dsp_osal_irq_config : procmgr_ability fail retcode = %d", retcode, 0, 0, 0, 0);
    }

    SIGEV_INTR_INIT(&event);
    dsp_osal_typecast(&irq_data, &data);
    retcode = InterruptAttachEvent((int32_t)irq_data->irq_id, &event, _NTO_INTR_FLAGS_TRK_MSK);
    if (retcode == -1) {
        dsp_osal_printI5("[ERR] dsp_irq_thread : InterruptAttachEvent fail errno = %d", errno, 0, 0, 0, 0);
    } else {
        irq_data->irq_attache_id = (uint32_t)retcode;

        /*... insert your code here ... */
        while (DoWhile == 1U) {
            (void)InterruptWait(0/* 0 based on QNX doc */, NULL/* Null based on QNX doc */);
            if (irq_data->relay_hdlr != NULL) {
                irq_data->relay_hdlr(irq_data->irq_id, NULL);
            }
            (void)InterruptUnmask((int32_t)irq_data->irq_id, (int32_t)irq_data->irq_attache_id);
            if (DspIrqStop == 1U) {
                DoWhile = 0;
            }
        }
    }

    return NULL;
}

uint32_t dsp_osal_irq_config(uint32_t irq_id, const osal_irq_t *irq_cfg, osal_isr_entry_f hdlr, uint32_t arg)
{
#define IRQ_RELAY_HDLR_SEARCH_OK    (1U)
#define IRQ_RELAY_HDLR_SEARCH_EXIST (2U)
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;
    static uint32_t irq_usage_init = 0U;
    static osal_irq_relay_usage_t irq_usage[MAX_DSPDRV_ISR_NUM];
    uint32_t i, exit_i_loop, idx;

(void)irq_cfg;
(void)arg;
    if (irq_usage_init == 0U) {
        for (i = 0U; i < MAX_DSPDRV_ISR_NUM; i++) {
            irq_usage[i].irq_id = IRQ_ID_INVALID;
            irq_usage[i].irq_attache_id = IRQ_ID_INVALID;
            irq_usage[i].relay_hdlr = NULL;
        }
        irq_usage_init = 1;
    }

    if (hdlr == NULL) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_irq_enable : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* Search */
        exit_i_loop = 0U;
        for (i = 0U; i < MAX_DSPDRV_ISR_NUM; i++) {
            if (irq_usage[i].irq_id == IRQ_ID_INVALID) {
                irq_usage[i].irq_id = irq_id;
                irq_usage[i].relay_hdlr = hdlr;
                exit_i_loop = IRQ_RELAY_HDLR_SEARCH_OK;
                idx = i;
            } else if (irq_usage[i].irq_id == irq_id) {
                // already config, only update hdlr
                irq_usage[i].relay_hdlr = hdlr;
                exit_i_loop = IRQ_RELAY_HDLR_SEARCH_EXIST;
            } else {
                // DO NOTHING
            }
            if (exit_i_loop != 0U) {
                break;
            }
        }

        if (exit_i_loop == IRQ_RELAY_HDLR_SEARCH_OK) {
            /* Start the thread that will handle interrupt events. */
            retcode = pthread_create(NULL, NULL, dsp_irq_thread, &irq_usage[idx]);
            if (retcode != EOK) {
                dsp_osal_printI5("[ERR] dsp_osal_irq_config : pthread create fail retcode = %d", retcode, 0, 0, 0, 0);
                Rval = DSP_ERR_0003;
            }
        }
    }

    return Rval;
}

uint32_t dsp_osal_irq_enable(uint32_t irq_id)
{
    uint32_t Rval = DSP_ERR_NONE;
(void)irq_id;
    // DO NOTHING

    return Rval;
}

#if 0
uint32_t dsp_osal_irq_disable(uint32_t irq_id)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;
    uint32_t i, exit_i_loop, usage_id = 0U;

    /* Search */
    exit_i_loop = 0U;
    for (i = 0; i < MAX_DSPDRV_ISR_NUM; i++) {
        if (irq_usage[i].irq_id == irq_id) {
            exit_i_loop = 1U;
            usage_id = i;
            break;
        }
    }

    if (exit_i_loop == 1U) {
        retcode = InterruptDetach((int32_t)irq_usage[usage_id].irq_attache_id);
        if (retcode < 0) {
            Rval = DSP_ERR_0003;
            dsp_osal_printI5("[ERR] dsp_osal_irq_disable() : fail retcode = %d errno %d", retcode, errno, 0, 0, 0);
        } else {
            irq_usage[usage_id].irq_id = IRQ_ID_INVALID;
            irq_usage[usage_id].irq_attache_id = IRQ_ID_INVALID;
            irq_usage[usage_id].relay_hdlr = NULL;
        }
    }

    return Rval;
}
#endif
#endif

#if !defined (CONFIG_DEVICE_TREE_SUPPORT)
#if defined (CONFIG_SOC_CV22) || defined (CONFIG_SOC_CV2) || defined (CONFIG_SOC_CV25) || defined (CONFIG_SOC_H32) || defined (CONFIG_SOC_CV28)
#ifdef AMBA_INT_SPI_ID173_CODE_VDSP0_IRQ
#define IRQ_VDSP0   (AMBA_INT_SPI_ID173_CODE_VDSP0_IRQ)
#else
#define IRQ_VDSP0   (173)
#endif
#ifdef AMBA_INT_SPI_ID174_CODE_VDSP1_IRQ
#define IRQ_VDSP1   (AMBA_INT_SPI_ID174_CODE_VDSP1_IRQ)
#else
#define IRQ_VDSP1   (174)
#endif
#ifdef AMBA_INT_SPI_ID175_CODE_VDSP2_IRQ
#define IRQ_VDSP2   (AMBA_INT_SPI_ID175_CODE_VDSP2_IRQ)
#else
#define IRQ_VDSP2   (175)
#endif
#ifdef AMBA_INT_SPI_ID176_CODE_VDSP3_IRQ
#define IRQ_VDSP3   (AMBA_INT_SPI_ID176_CODE_VDSP3_IRQ)
#else
#define IRQ_VDSP3   (176)
#endif

#if defined (CONFIG_SOC_CV22)
#ifdef AMBA_INT_SPI_ID143_VIN_SOF
#define IRQ_VIN0_SOF    (AMBA_INT_SPI_ID143_VIN_SOF)
#else
#define IRQ_VIN0_SOF    (143)
#endif
#ifdef AMBA_INT_SPI_ID144_VIN_LAST_PIXEL
#define IRQ_VIN0_EOF    (AMBA_INT_SPI_ID144_VIN_LAST_PIXEL)
#else
#define IRQ_VIN0_EOF    (144)
#endif
#ifdef AMBA_INT_SPI_ID168_PIP_SOF
#define IRQ_VIN1_SOF    (AMBA_INT_SPI_ID168_PIP_SOF)
#else
#define IRQ_VIN1_SOF    (168)
#endif
#ifdef AMBA_INT_SPI_ID169_PIP_LAST_PIXEL
#define IRQ_VIN1_EOF    (AMBA_INT_SPI_ID169_PIP_LAST_PIXEL)
#else
#define IRQ_VIN1_EOF    (169)
#endif
#ifdef AMBA_INT_SPI_ID121_VOUT_A_INT
#define IRQ_VOUT0    (AMBA_INT_SPI_ID121_VOUT_A_INT)
#else
#define IRQ_VOUT0    (121)
#endif
#ifdef AMBA_INT_SPI_ID120_VOUT_B_INT
#define IRQ_VOUT1    (AMBA_INT_SPI_ID120_VOUT_B_INT)
#else
#define IRQ_VOUT1    (120)
#endif

#elif defined (CONFIG_SOC_CV2)
#ifdef AMBA_INT_SPI_ID143_VIN_SOF
#define IRQ_VIN0_SOF    (AMBA_INT_SPI_ID143_VIN_SOF)
#else
#define IRQ_VIN0_SOF    (143)
#endif
#ifdef AMBA_INT_SPI_ID144_VIN_LAST_PIXEL
#define IRQ_VIN0_EOF    (AMBA_INT_SPI_ID144_VIN_LAST_PIXEL)
#else
#define IRQ_VIN0_EOF    (144)
#endif
#ifdef AMBA_INT_SPI_ID168_PIP_SOF
#define IRQ_VIN1_SOF    (AMBA_INT_SPI_ID168_PIP_SOF)
#else
#define IRQ_VIN1_SOF    (168)
#endif
#ifdef AMBA_INT_SPI_ID169_PIP_LAST_PIXEL
#define IRQ_VIN1_EOF    (AMBA_INT_SPI_ID169_PIP_LAST_PIXEL)
#else
#define IRQ_VIN1_EOF    (169)
#endif
#ifdef AMBA_INT_SPI_ID163_PIP2_SOF
#define IRQ_VIN2_SOF    (AMBA_INT_SPI_ID163_PIP2_SOF)
#else
#define IRQ_VIN2_SOF    (163)
#endif
#ifdef AMBA_INT_SPI_ID164_PIP2_LAST_PIXEL
#define IRQ_VIN2_EOF    (AMBA_INT_SPI_ID164_PIP2_LAST_PIXEL)
#else
#define IRQ_VIN2_EOF    (164)
#endif
#ifdef AMBA_INT_SPI_ID158_PIP3_SOF
#define IRQ_VIN3_SOF    (AMBA_INT_SPI_ID158_PIP3_SOF)
#else
#define IRQ_VIN3_SOF    (158)
#endif
#ifdef AMBA_INT_SPI_ID159_PIP3_LAST_PIXEL
#define IRQ_VIN3_EOF    (AMBA_INT_SPI_ID159_PIP3_LAST_PIXEL)
#else
#define IRQ_VIN3_EOF    (159)
#endif
#ifdef AMBA_INT_SPI_ID153_PIP4_SOF
#define IRQ_VIN4_SOF    (AMBA_INT_SPI_ID153_PIP4_SOF)
#else
#define IRQ_VIN4_SOF    (153)
#endif
#ifdef AMBA_INT_SPI_ID154_PIP4_LAST_PIXEL
#define IRQ_VIN4_EOF    (AMBA_INT_SPI_ID154_PIP4_LAST_PIXEL)
#else
#define IRQ_VIN4_EOF    (154)
#endif
#ifdef AMBA_INT_SPI_ID148_PIP5_SOF
#define IRQ_VIN5_SOF    (AMBA_INT_SPI_ID148_PIP5_SOF)
#else
#define IRQ_VIN5_SOF    (148)
#endif
#ifdef AMBA_INT_SPI_ID149_PIP5_LAST_PIXEL
#define IRQ_VIN5_EOF    (AMBA_INT_SPI_ID149_PIP5_LAST_PIXEL)
#else
#define IRQ_VIN5_EOF    (149)
#endif
#ifdef AMBA_INT_SPI_ID121_VOUT_A_INT
#define IRQ_VOUT0    (AMBA_INT_SPI_ID121_VOUT_A_INT)
#else
#define IRQ_VOUT0    (121)
#endif
#ifdef AMBA_INT_SPI_ID120_VOUT_B_INT
#define IRQ_VOUT1    (AMBA_INT_SPI_ID120_VOUT_B_INT)
#else
#define IRQ_VOUT1    (120)
#endif

#elif defined (CONFIG_SOC_CV25)
#ifdef AMBA_INT_SPI_ID143_VIN_SOF
#define IRQ_VIN0_SOF    (AMBA_INT_SPI_ID143_VIN_SOF)
#else
#define IRQ_VIN0_SOF    (143)
#endif
#ifdef AMBA_INT_SPI_ID144_VIN_LAST_PIXEL
#define IRQ_VIN0_EOF    (AMBA_INT_SPI_ID144_VIN_LAST_PIXEL)
#else
#define IRQ_VIN0_EOF    (144)
#endif
#ifdef AMBA_INT_SPI_ID168_PIP_SOF
#define IRQ_VIN1_SOF    (AMBA_INT_SPI_ID168_PIP_SOF)
#else
#define IRQ_VIN1_SOF    (168)
#endif
#ifdef AMBA_INT_SPI_ID169_PIP_LAST_PIXEL
#define IRQ_VIN1_EOF    (AMBA_INT_SPI_ID169_PIP_LAST_PIXEL)
#else
#define IRQ_VIN1_EOF    (169)
#endif
#ifdef AMBA_INT_SPI_ID147_PIP2_SOF
#define IRQ_VIN2_SOF    (AMBA_INT_SPI_ID147_PIP2_SOF)
#else
#define IRQ_VIN2_SOF    (147)
#endif
#ifdef AMBA_INT_SPI_ID148_PIP2_LAST_PIXEL
#define IRQ_VIN2_EOF    (AMBA_INT_SPI_ID148_PIP2_LAST_PIXEL)
#else
#define IRQ_VIN2_EOF    (148)
#endif
#ifdef AMBA_INT_SPI_ID121_VOUT_A_INT
#define IRQ_VOUT0    (AMBA_INT_SPI_ID121_VOUT_A_INT)
#else
#define IRQ_VOUT0    (121)
#endif
#ifdef AMBA_INT_SPI_ID120_VOUT_B_INT
#define IRQ_VOUT1    (AMBA_INT_SPI_ID120_VOUT_B_INT)
#else
#define IRQ_VOUT1    (120)
#endif

#elif defined (CONFIG_SOC_H32)
#ifdef AMBA_INT_SPI_ID143_VIN_SOF
#define IRQ_VIN0_SOF    (AMBA_INT_SPI_ID143_VIN_SOF)
#else
#define IRQ_VIN0_SOF    (143)
#endif
#ifdef AMBA_INT_SPI_ID144_VIN_LAST_PIXEL
#define IRQ_VIN0_EOF    (AMBA_INT_SPI_ID144_VIN_LAST_PIXEL)
#else
#define IRQ_VIN0_EOF    (144)
#endif
#ifdef AMBA_INT_SPI_ID168_PIP_SOF
#define IRQ_VIN1_SOF    (AMBA_INT_SPI_ID168_PIP_SOF)
#else
#define IRQ_VIN1_SOF    (168)
#endif
#ifdef AMBA_INT_SPI_ID169_PIP_LAST_PIXEL
#define IRQ_VIN1_EOF    (AMBA_INT_SPI_ID169_PIP_LAST_PIXEL)
#else
#define IRQ_VIN1_EOF    (169)
#endif
#ifdef AMBA_INT_SPI_ID147_PIP2_SOF
#define IRQ_VIN2_SOF    (AMBA_INT_SPI_ID147_PIP2_SOF)
#else
#define IRQ_VIN2_SOF    (147)
#endif
#ifdef AMBA_INT_SPI_ID148_PIP2_LAST_PIXEL
#define IRQ_VIN2_EOF    (AMBA_INT_SPI_ID148_PIP2_LAST_PIXEL)
#else
#define IRQ_VIN2_EOF    (148)
#endif
#ifdef AMBA_INT_SPI_ID121_VOUT_A_INT
#define IRQ_VOUT0    (AMBA_INT_SPI_ID121_VOUT_A_INT)
#else
#define IRQ_VOUT0    (121)
#endif
#ifdef AMBA_INT_SPI_ID120_VOUT_B_INT
#define IRQ_VOUT1    (AMBA_INT_SPI_ID120_VOUT_B_INT)
#else
#define IRQ_VOUT1    (120)
#endif

#else /* CV28 */
#ifdef AMBA_INT_SPI_ID143_VIN_SOF
#define IRQ_VIN0_SOF    (AMBA_INT_SPI_ID143_VIN_SOF)
#else
#define IRQ_VIN0_SOF    (143)
#endif
#ifdef AMBA_INT_SPI_ID144_VIN_LAST_PIXEL
#define IRQ_VIN0_EOF    (AMBA_INT_SPI_ID144_VIN_LAST_PIXEL)
#else
#define IRQ_VIN0_EOF    (144)
#endif
#ifdef AMBA_INT_SPI_ID168_PIP_SOF
#define IRQ_VIN1_SOF    (AMBA_INT_SPI_ID168_PIP_SOF)
#else
#define IRQ_VIN1_SOF    (168)
#endif
#ifdef AMBA_INT_SPI_ID169_PIP_LAST_PIXEL
#define IRQ_VIN1_EOF    (AMBA_INT_SPI_ID169_PIP_LAST_PIXEL)
#else
#define IRQ_VIN1_EOF    (169)
#endif
#ifdef AMBA_INT_SPI_ID147_PIP2_SOF
#define IRQ_VIN2_SOF    (AMBA_INT_SPI_ID147_PIP2_SOF)
#else
#define IRQ_VIN2_SOF    (147)
#endif
#ifdef AMBA_INT_SPI_ID148_PIP2_LAST_PIXEL
#define IRQ_VIN2_EOF    (AMBA_INT_SPI_ID148_PIP2_LAST_PIXEL)
#else
#define IRQ_VIN2_EOF    (148)
#endif
#ifdef AMBA_INT_SPI_ID121_VOUT_A_INT
#define IRQ_VOUT0    (AMBA_INT_SPI_ID121_VOUT_A_INT)
#else
#define IRQ_VOUT0    (121)
#endif
#endif

#elif defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
#ifdef AMBA_INT_SPI_ID177_CODE_VDSP0_IRQ
#define IRQ_VDSP0   (AMBA_INT_SPI_ID177_CODE_VDSP0_IRQ)
#else
#define IRQ_VDSP0   (177)
#endif
#ifdef AMBA_INT_SPI_ID178_CODE_VDSP1_IRQ
#define IRQ_VDSP1   (AMBA_INT_SPI_ID178_CODE_VDSP1_IRQ)
#else
#define IRQ_VDSP1   (178)
#endif
#ifdef AMBA_INT_SPI_ID179_CODE_VDSP2_IRQ
#define IRQ_VDSP2   (AMBA_INT_SPI_ID179_CODE_VDSP2_IRQ)
#else
#define IRQ_VDSP2   (179)
#endif
#ifdef AMBA_INT_SPI_ID180_CODE_VDSP3_IRQ
#define IRQ_VDSP3   (AMBA_INT_SPI_ID180_CODE_VDSP3_IRQ)
#else
#define IRQ_VDSP3   (180)
#endif

#ifdef AMBA_INT_SPI_ID139_VIN0_SOF
#define IRQ_VIN0_SOF    (AMBA_INT_SPI_ID139_VIN0_SOF)
#else
#define IRQ_VIN0_SOF    (139)
#endif
#ifdef AMBA_INT_SPI_ID141_VIN0_LAST_PIXEL
#define IRQ_VIN0_EOF    (AMBA_INT_SPI_ID141_VIN0_LAST_PIXEL)
#else
#define IRQ_VIN0_EOF    (141)
#endif
#ifdef AMBA_INT_SPI_ID144_VIN1_SOF
#define IRQ_VIN1_SOF    (AMBA_INT_SPI_ID144_VIN1_SOF)
#else
#define IRQ_VIN1_SOF    (144)
#endif
#ifdef AMBA_INT_SPI_ID146_VIN1_LAST_PIXEL
#define IRQ_VIN1_EOF    (AMBA_INT_SPI_ID146_VIN1_LAST_PIXEL)
#else
#define IRQ_VIN1_EOF    (146)
#endif
#ifdef AMBA_INT_SPI_ID149_VIN2_SOF
#define IRQ_VIN2_SOF    (AMBA_INT_SPI_ID149_VIN2_SOF)
#else
#define IRQ_VIN2_SOF    (149)
#endif
#ifdef AMBA_INT_SPI_ID150_VIN2_LAST_PIXEL
#define IRQ_VIN2_EOF    (AMBA_INT_SPI_ID150_VIN2_LAST_PIXEL)
#else
#define IRQ_VIN2_EOF    (150)
#endif
#ifdef AMBA_INT_SPI_ID153_VIN3_SOF
#define IRQ_VIN3_SOF    (AMBA_INT_SPI_ID153_VIN3_SOF)
#else
#define IRQ_VIN3_SOF    (153)
#endif
#ifdef AMBA_INT_SPI_ID155_VIN3_LAST_PIXEL
#define IRQ_VIN3_EOF    (AMBA_INT_SPI_ID155_VIN3_LAST_PIXEL)
#else
#define IRQ_VIN3_EOF    (155)
#endif
#ifdef AMBA_INT_SPI_ID158_VIN4_SOF
#define IRQ_VIN4_SOF    (AMBA_INT_SPI_ID158_VIN4_SOF)
#else
#define IRQ_VIN4_SOF    (158)
#endif
#ifdef AMBA_INT_SPI_ID160_VIN4_LAST_PIXEL
#define IRQ_VIN4_EOF    (AMBA_INT_SPI_ID160_VIN4_LAST_PIXEL)
#else
#define IRQ_VIN4_EOF    (160)
#endif
#ifdef AMBA_INT_SPI_ID174_VIN8_SOF
#define IRQ_VIN5_SOF    (AMBA_INT_SPI_ID174_VIN8_SOF)
#else
#define IRQ_VIN5_SOF    (174)
#endif
#ifdef AMBA_INT_SPI_ID175_VIN8_LAST_PIXEL
#define IRQ_VIN5_EOF    (AMBA_INT_SPI_ID175_VIN8_LAST_PIXEL)
#else
#define IRQ_VIN5_EOF    (175)
#endif
#ifdef AMBA_INT_SPI_ID120_VOUT_A_INT
#define IRQ_VOUT0    (AMBA_INT_SPI_ID120_VOUT_A_INT)
#else
#define IRQ_VOUT0    (120)
#endif
#ifdef AMBA_INT_SPI_ID119_VOUT_B_INT
#define IRQ_VOUT1    (AMBA_INT_SPI_ID119_VOUT_B_INT)
#else
#define IRQ_VOUT1    (119)
#endif

#else /*CV5*/
#ifdef AMBA_INT_SPI_ID178_CODE_VDSP1_IRQ0
#define IRQ_VDSP0   (AMBA_INT_SPI_ID178_CODE_VDSP1_IRQ0)
#else
#define IRQ_VDSP0   (178)
#endif
#ifdef AMBA_INT_SPI_ID179_CODE_VDSP1_IRQ1
#define IRQ_VDSP1   (AMBA_INT_SPI_ID179_CODE_VDSP1_IRQ1)
#else
#define IRQ_VDSP1   (179)
#endif
#ifdef AMBA_INT_SPI_ID180_CODE_VDSP1_IRQ2
#define IRQ_VDSP2   (AMBA_INT_SPI_ID180_CODE_VDSP1_IRQ2)
#else
#define IRQ_VDSP2   (180)
#endif
#ifdef AMBA_INT_SPI_ID182_CODE_VDSP2_IRQ0
#define IRQ_VDSP3   (AMBA_INT_SPI_ID182_CODE_VDSP2_IRQ0)
#else
#define IRQ_VDSP3   (182)
#endif
#ifdef AMBA_INT_SPI_ID174_CODE_VDSP0_IRQ0
#define IRQ_VDSP4   (AMBA_INT_SPI_ID174_CODE_VDSP0_IRQ0)
#else
#define IRQ_VDSP4   (174)
#endif

#ifdef AMBA_INT_SPI_ID144_VIN_SOF
#define IRQ_VIN0_SOF    (AMBA_INT_SPI_ID144_VIN_SOF)
#else
#define IRQ_VIN0_SOF    (144)
#endif
#ifdef AMBA_INT_SPI_ID146_VIN_LAST_PIXEL
#define IRQ_VIN0_EOF    (AMBA_INT_SPI_ID146_VIN_LAST_PIXEL)
#else
#define IRQ_VIN0_EOF    (146)
#endif
#ifdef AMBA_INT_SPI_ID149_VIN2_SOF
#define IRQ_VIN1_SOF    (AMBA_INT_SPI_ID149_VIN2_SOF)
#else
#define IRQ_VIN1_SOF    (149)
#endif
#ifdef AMBA_INT_SPI_ID150_VIN2_LAST_PIXEL
#define IRQ_VIN1_EOF    (AMBA_INT_SPI_ID150_VIN2_LAST_PIXEL)
#else
#define IRQ_VIN1_EOF    (150)
#endif
#ifdef AMBA_INT_SPI_ID153_VIN3_SOF
#define IRQ_VIN2_SOF    (AMBA_INT_SPI_ID153_VIN3_SOF)
#else
#define IRQ_VIN2_SOF    (153)
#endif
#ifdef AMBA_INT_SPI_ID154_VIN3_LAST_PIXEL
#define IRQ_VIN2_EOF    (AMBA_INT_SPI_ID154_VIN3_LAST_PIXEL)
#else
#define IRQ_VIN2_EOF    (154)
#endif
#ifdef AMBA_INT_SPI_ID157_VIN4_SOF
#define IRQ_VIN3_SOF    (AMBA_INT_SPI_ID157_VIN4_SOF)
#else
#define IRQ_VIN3_SOF    (157)
#endif
#ifdef AMBA_INT_SPI_ID158_VIN4_LAST_PIXEL
#define IRQ_VIN3_EOF    (AMBA_INT_SPI_ID158_VIN4_LAST_PIXEL)
#else
#define IRQ_VIN3_EOF    (158)
#endif
#ifdef AMBA_INT_SPI_ID139_VIN5_SOF
#define IRQ_VIN4_SOF    (AMBA_INT_SPI_ID139_VIN5_SOF)
#else
#define IRQ_VIN4_SOF    (139)
#endif
#ifdef AMBA_INT_SPI_ID141_VIN5_LAST_PIXEL
#define IRQ_VIN4_EOF    (AMBA_INT_SPI_ID141_VIN5_LAST_PIXEL)
#else
#define IRQ_VIN4_EOF    (141)
#endif
#ifdef AMBA_INT_SPI_ID163_VIN6_SOF
#define IRQ_VIN5_SOF    (AMBA_INT_SPI_ID163_VIN6_SOF)
#else
#define IRQ_VIN5_SOF    (163)
#endif
#ifdef AMBA_INT_SPI_ID164_VIN6_LAST_PIXEL
#define IRQ_VIN5_EOF    (AMBA_INT_SPI_ID164_VIN6_LAST_PIXEL)
#else
#define IRQ_VIN5_EOF    (164)
#endif
#ifdef AMBA_INT_SPI_ID167_VIN7_SOF
#define IRQ_VIN6_SOF    (AMBA_INT_SPI_ID167_VIN7_SOF)
#else
#define IRQ_VIN6_SOF    (167)
#endif
#ifdef AMBA_INT_SPI_ID168_VIN7_LAST_PIXEL
#define IRQ_VIN6_EOF    (AMBA_INT_SPI_ID168_VIN7_LAST_PIXEL)
#else
#define IRQ_VIN6_EOF    (168)
#endif
#ifdef AMBA_INT_SPI_ID171_VIN8_SOF
#define IRQ_VIN7_SOF    (AMBA_INT_SPI_ID171_VIN8_SOF)
#else
#define IRQ_VIN7_SOF    (171)
#endif
#ifdef AMBA_INT_SPI_ID172_VIN8_LAST_PIXEL
#define IRQ_VIN7_EOF    (AMBA_INT_SPI_ID172_VIN8_LAST_PIXEL)
#else
#define IRQ_VIN7_EOF    (172)
#endif
#ifdef AMBA_INT_SPI_ID209_VIN9_SOF
#define IRQ_VIN8_SOF    (AMBA_INT_SPI_ID209_VIN9_SOF)
#else
#define IRQ_VIN8_SOF    (209)
#endif
#ifdef AMBA_INT_SPI_ID210_VIN9_LAST_PIXEL
#define IRQ_VIN8_EOF    (AMBA_INT_SPI_ID210_VIN9_LAST_PIXEL)
#else
#define IRQ_VIN8_EOF    (210)
#endif
#ifdef AMBA_INT_SPI_ID213_VIN10_SOF
#define IRQ_VIN9_SOF    (AMBA_INT_SPI_ID213_VIN10_SOF)
#else
#define IRQ_VIN9_SOF    (213)
#endif
#ifdef AMBA_INT_SPI_ID214_VIN10_LAST_PIXEL
#define IRQ_VIN9_EOF    (AMBA_INT_SPI_ID214_VIN10_LAST_PIXEL)
#else
#define IRQ_VIN9_EOF    (214)
#endif
#ifdef AMBA_INT_SPI_ID217_VIN11_SOF
#define IRQ_VIN10_SOF    (AMBA_INT_SPI_ID217_VIN11_SOF)
#else
#define IRQ_VIN10_SOF    (217)
#endif
#ifdef AMBA_INT_SPI_ID218_VIN11_LAST_PIXEL
#define IRQ_VIN10_EOF    (AMBA_INT_SPI_ID218_VIN11_LAST_PIXEL)
#else
#define IRQ_VIN10_EOF    (218)
#endif
#ifdef AMBA_INT_SPI_ID221_VIN12_SOF
#define IRQ_VIN11_SOF    (AMBA_INT_SPI_ID221_VIN12_SOF)
#else
#define IRQ_VIN11_SOF    (221)
#endif
#ifdef AMBA_INT_SPI_ID222_VIN12_LAST_PIXEL
#define IRQ_VIN11_EOF    (AMBA_INT_SPI_ID222_VIN12_LAST_PIXEL)
#else
#define IRQ_VIN11_EOF    (222)
#endif
#ifdef AMBA_INT_SPI_ID225_VIN13_SOF
#define IRQ_VIN12_SOF    (AMBA_INT_SPI_ID225_VIN13_SOF)
#else
#define IRQ_VIN12_SOF    (225)
#endif
#ifdef AMBA_INT_SPI_ID226_VIN13_LAST_PIXEL
#define IRQ_VIN12_EOF    (AMBA_INT_SPI_ID226_VIN13_LAST_PIXEL)
#else
#define IRQ_VIN12_EOF    (226)
#endif
#ifdef AMBA_INT_SPI_ID229_VIN14_SOF
#define IRQ_VIN13_SOF    (AMBA_INT_SPI_ID229_VIN14_SOF)
#else
#define IRQ_VIN13_SOF    (229)
#endif
#ifdef AMBA_INT_SPI_ID230_VIN14_LAST_PIXEL
#define IRQ_VIN13_EOF    (AMBA_INT_SPI_ID230_VIN14_LAST_PIXEL)
#else
#define IRQ_VIN13_EOF    (230)
#endif
#ifdef AMBA_INT_SPI_ID120_VOUT_A_INT
#define IRQ_VOUT0    (AMBA_INT_SPI_ID120_VOUT_A_INT)
#else
#define IRQ_VOUT0    (120)
#endif
#ifdef AMBA_INT_SPI_ID119_VOUT_B_INT
#define IRQ_VOUT1    (AMBA_INT_SPI_ID119_VOUT_B_INT)
#else
#define IRQ_VOUT1    (119)
#endif
#ifdef AMBA_INT_SPI_ID241_VOUT_C_INT
#define IRQ_VOUT2    (AMBA_INT_SPI_ID241_VOUT_C_INT)
#else
#define IRQ_VOUT2    (241)
#endif

#endif
#else
#define AMBA_INT_OFFSET     (32U)
static osal_irq_id_t osal_irq_id = {0};
#endif

#if defined (CONFIG_DEVICE_TREE_SUPPORT)
#define DTS_DSP_NODE        "/dspdrv"
#define DTS_DSP_VIN_NODE    "/dspdrv/dspvin"
#define DTS_DSP_VOUT_NODE   "/dspdrv/dspvout"

static inline uint32_t dsp_osal_irq_code_id_init(const void *pfdt)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t fdt_ret = 0;
    int32_t offset = 0;
    const struct fdt_property *prop = NULL;
    int32_t len = 0;
    const char *chr1;
    const UINT32 *ptr1;
    uint32_t i, idx;

    fdt_ret = AmbaFDT_PathOffset(pfdt, DTS_DSP_NODE);
    if (fdt_ret < 0) {
        dsp_osal_printU5("[ERR] dsp_osal_irq_code_id_init : AmbaFDT_PathOffset[DSP_NODE] fail[0x%X]", (uint32_t)fdt_ret, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0003;
    } else {
        offset = fdt_ret;
        prop = AmbaFDT_GetProperty(pfdt, offset, "interrupts", &len);
        if ((prop != NULL) && (len > 4)) {
            chr1 = (const char *)&prop->data[0];
            dsp_osal_typecast(&ptr1, &chr1);
            // interrupts = <0 145 0x1 0 146 0x1 0 147 0x1 0 148 0x1>
            for (i = 0U; i < DSP_MAX_IRQ_CODE_NUM; i++) {
                idx = (3U*i) + 1U;
                osal_irq_id.DspCode[i] = AmbaFDT_Fdt32ToCpu(ptr1[idx]);
                osal_irq_id.DspCode[i] += AMBA_INT_OFFSET;
            }
            //dsp_osal_printU5("dsp_osal_irq_code_id_init : Code[%d %d %d %d]", osal_irq_id.DspCode[0U], osal_irq_id.DspCode[1U], osal_irq_id.DspCode[2U], osal_irq_id.DspCode[3U], 0U);
        } else {
            dsp_osal_printU5("[ERR] dsp_osal_irq_code_id_init : AmbaFDT_GetProperty[DSP_NODE] fail", 0U, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        }
    }

    return Rval;
}

static inline uint32_t dsp_osal_irq_vin_id_init(const void *pfdt)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t fdt_ret = 0;
    int32_t offset = 0;
    const struct fdt_property *prop = NULL;
    int32_t len = 0;
    const char *chr1;
    const UINT32 *ptr1;
    uint32_t i;
#define NODE_NAME_LEN   (32U)
    char node_name[NODE_NAME_LEN];

    for (i = 0U; i < DSP_MAX_IRQ_VIN_NUM; i++) {
        Rval = dsp_osal_str_copy(node_name, NODE_NAME_LEN, DTS_DSP_VIN_NODE);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        Rval = dsp_osal_str_append_u32(node_name, NODE_NAME_LEN, i, 10U);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        fdt_ret = AmbaFDT_PathOffset(pfdt, node_name);
        if (fdt_ret < 0) {
            dsp_osal_printU5("[ERR] dsp_osal_irq_vin_id_init : AmbaFDT_PathOffset[DSP_VIN_NODE %d] fail[%d]", i, (uint32_t)fdt_ret, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        } else {
            offset = fdt_ret;
            prop = AmbaFDT_GetProperty(pfdt, offset, "interrupts", &len);
            if ((prop != NULL) && (len > 4)) {
                chr1 = (const char *)&prop->data[0];
                dsp_osal_typecast(&ptr1, &chr1);
                // interrupts = <0 107 0x1 0 109 0x1>
                osal_irq_id.VinSof[i] = AmbaFDT_Fdt32ToCpu(ptr1[1U]);
                osal_irq_id.VinSof[i] += AMBA_INT_OFFSET;
                osal_irq_id.VinEof[i] = AmbaFDT_Fdt32ToCpu(ptr1[4U]);
                osal_irq_id.VinEof[i] += AMBA_INT_OFFSET;
                //dsp_osal_printU5("dsp_osal_irq_vin_id_init : Vin%d[%d %d]", i, osal_irq_id.VinSof[i], osal_irq_id.VinEof[i], 0U, 0U);
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_irq_vin_id_init : AmbaFDT_GetProperty[DSP_VIN_NODE %d] fail", i, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
        }

        if (Rval != DSP_ERR_NONE) {
            break;
        }
    }

    return Rval;
}

static inline uint32_t dsp_osal_irq_vout_id_init(const void *pfdt)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t fdt_ret = 0;
    int32_t offset = 0;
    const struct fdt_property *prop = NULL;
    int32_t len = 0;
    const char *chr1;
    const UINT32 *ptr1;
    uint32_t i;
#define NODE_NAME_LEN   (32U)
    char node_name[NODE_NAME_LEN];

    for (i = 0U; i < DSP_MAX_IRQ_VOUT_NUM; i++) {
        Rval = dsp_osal_str_copy(node_name, NODE_NAME_LEN, DTS_DSP_VOUT_NODE);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        Rval = dsp_osal_str_append_u32(node_name, NODE_NAME_LEN, i, 10U);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        fdt_ret = AmbaFDT_PathOffset(pfdt, node_name);
        if (fdt_ret < 0) {
            dsp_osal_printU5("[ERR] dsp_osal_irq_vout_id_init : AmbaFDT_PathOffset[DSP_VOUT_NODE %d] fail[%d]", i, (uint32_t)fdt_ret, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        } else {
            offset = fdt_ret;
            prop = AmbaFDT_GetProperty(pfdt, offset, "interrupts", &len);
            if ((prop != NULL) && (len > 4)) {
                chr1 = (const char *)&prop->data[0];
                dsp_osal_typecast(&ptr1, &chr1);
                // interrupts = <0 88 0x1>
                osal_irq_id.Vout[i] = AmbaFDT_Fdt32ToCpu(ptr1[1U]);
                osal_irq_id.Vout[i] += AMBA_INT_OFFSET;
                //dsp_osal_printU5("dsp_osal_irq_vout_id_init : Vout%d[%d]", i, osal_irq_id.Vout[i], 0U, 0U, 0U);
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_irq_vout_id_init : AmbaFDT_GetProperty[DSP_VOUT_NODE %d] fail", i, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
        }

        if (Rval != DSP_ERR_NONE) {
            break;
        }
    }

    return Rval;
}

static inline uint32_t dsp_osal_dts_init(void)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t dtb_phys_addr = 0U;
    void *virt_addr;

    if (dts_virt_addr == 0UL) {
        dtb_phys_addr = (uint32_t)CONFIG_DTB_LOADADDR;
        virt_addr = mmap_device_memory(NULL, CONFIG_DTB_SIZE, PROT_READ, 0, dtb_phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_dts_init : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            Rval = DSP_ERR_0003;
        } else {
            dsp_osal_typecast(&dts_virt_addr, &virt_addr);
        }
        //dsp_osal_printU5("dsp_osal_dts_init : dtb virt_addr 0x%X phys_addr 0x%X Sz %d", dts_virt_addr, dtb_phys_addr, CONFIG_DTB_SIZE, 0U, 0U);
    }

    return Rval;
}

static inline uint32_t dsp_osal_irq_id_init(void)
{
static uint32_t osal_irq_id_inited = 0U;
    uint32_t Rval = DSP_ERR_NONE;
    int32_t fdt_ret = 0;
    const void *pfdt = NULL;

    if (osal_irq_id_inited == 0U) {
#if defined (CONFIG_DTB_LOADADDR)
        Rval = dsp_osal_dts_init();
        if (Rval == DSP_ERR_NONE) {
            dsp_osal_typecast(&pfdt, &dts_virt_addr);
            fdt_ret = AmbaFDT_CheckHeader(pfdt);
            if (fdt_ret != 0) {
                dsp_osal_printU5("[ERR] dsp_osal_irq_id_init : AmbaFDT_CheckHeader fail[0x%X]", (uint32_t)fdt_ret, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            } else {
                /* Get CODE interrupt */
                Rval = dsp_osal_irq_code_id_init(pfdt);

                /* Get VIN interrupt */
                if (Rval == DSP_ERR_NONE) {
                    Rval = dsp_osal_irq_vin_id_init(pfdt);
                }

                /* Get VOUT interrupt */
                if (Rval == DSP_ERR_NONE) {
                    Rval = dsp_osal_irq_vout_id_init(pfdt);
                }
            }
        }

        if (Rval == DSP_ERR_NONE) {
            osal_irq_id_inited = 1U;
        }
#else
        dsp_osal_printU5("[ERR] dsp_osal_irq_id_init : Null dtb address", 0U, 0U, 0U, 0U, 0U);
#endif
    }

    return Rval;
}
#endif

uint32_t dsp_osal_irq_id_query(uint32_t type, uint32_t idx, uint32_t *irq_id)
{
    uint32_t Rval = DSP_ERR_NONE;

#if defined (CONFIG_DEVICE_TREE_SUPPORT)
    Rval = dsp_osal_irq_id_init();
#else
#if defined (CONFIG_SOC_CV22)
const osal_irq_id_t osal_irq_id = {
    .DspCode = {
        [0U] = IRQ_VDSP0,
        [1U] = IRQ_VDSP1,
        [2U] = IRQ_VDSP2,
        [3U] = IRQ_VDSP3,
    },
    .VinSof = {
        [0U] = IRQ_VIN0_SOF,
        [1U] = IRQ_VIN1_SOF,
    },
    .VinEof = {
        [0U] = IRQ_VIN0_EOF,
        [1U] = IRQ_VIN1_EOF,
    },
    .Vout = {
        [0U] = IRQ_VOUT0,
        [1U] = IRQ_VOUT1,
    },
};
#elif defined (CONFIG_SOC_CV2)
const osal_irq_id_t osal_irq_id = {
    .DspCode = {
        [0U] = IRQ_VDSP0,
        [1U] = IRQ_VDSP1,
        [2U] = IRQ_VDSP2,
        [3U] = IRQ_VDSP3,
    },
    .VinSof = {
        [0U] = IRQ_VIN0_SOF,
        [1U] = IRQ_VIN1_SOF,
        [2U] = IRQ_VIN2_SOF,
        [3U] = IRQ_VIN3_SOF,
        [4U] = IRQ_VIN4_SOF,
        [5U] = IRQ_VIN5_SOF,
    },
    .VinEof = {
        [0U] = IRQ_VIN0_EOF,
        [1U] = IRQ_VIN1_EOF,
        [2U] = IRQ_VIN2_EOF,
        [3U] = IRQ_VIN3_EOF,
        [4U] = IRQ_VIN4_EOF,
        [5U] = IRQ_VIN5_EOF,
    },
    .Vout = {
        [0U] = IRQ_VOUT0,
        [1U] = IRQ_VOUT1,
    },
};
#elif defined (CONFIG_SOC_CV25)
const osal_irq_id_t osal_irq_id = {
    .DspCode = {
        [0U] = IRQ_VDSP0,
        [1U] = IRQ_VDSP1,
        [2U] = IRQ_VDSP2,
        [3U] = IRQ_VDSP3,
    },
    .VinSof = {
        [0U] = IRQ_VIN0_SOF,
        [1U] = IRQ_VIN1_SOF,
        [2U] = IRQ_VIN2_SOF,
    },
    .VinEof = {
        [0U] = IRQ_VIN0_EOF,
        [1U] = IRQ_VIN1_EOF,
        [2U] = IRQ_VIN2_EOF,
    },
    .Vout = {
        [0U] = IRQ_VOUT0,
        [1U] = IRQ_VOUT1,
    },
};
#elif defined (CONFIG_SOC_H32)
const osal_irq_id_t osal_irq_id = {
    .DspCode = {
        [0U] = IRQ_VDSP0,
        [1U] = IRQ_VDSP1,
        [2U] = IRQ_VDSP2,
        [3U] = IRQ_VDSP3,
    },
    .VinSof = {
        [0U] = IRQ_VIN0_SOF,
        [1U] = IRQ_VIN1_SOF,
        [2U] = IRQ_VIN2_SOF,
    },
    .VinEof = {
        [0U] = IRQ_VIN0_EOF,
        [1U] = IRQ_VIN1_EOF,
        [2U] = IRQ_VIN2_EOF,
    },
    .Vout = {
        [0U] = IRQ_VOUT0,
        [1U] = IRQ_VOUT1,
    },
};
#elif defined (CONFIG_SOC_CV28)
const osal_irq_id_t osal_irq_id = {
    .DspCode = {
        [0U] = IRQ_VDSP0,
        [1U] = IRQ_VDSP1,
        [2U] = IRQ_VDSP2,
        [3U] = IRQ_VDSP3,
    },
    .VinSof = {
        [0U] = IRQ_VIN0_SOF,
        [1U] = IRQ_VIN1_SOF,
        [2U] = IRQ_VIN2_SOF,
    },
    .VinEof = {
        [0U] = IRQ_VIN0_EOF,
        [1U] = IRQ_VIN1_EOF,
        [2U] = IRQ_VIN2_EOF,
    },
    .Vout = {
        [0U] = IRQ_VOUT0,
    },
};
#elif defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
const osal_irq_id_t osal_irq_id = {
    .DspCode = {
        [0U] = IRQ_VDSP0,
        [1U] = IRQ_VDSP1,
        [2U] = IRQ_VDSP2,
        [3U] = IRQ_VDSP3,
    },
    .VinSof = {
        [0U] = IRQ_VIN0_SOF,
        [1U] = IRQ_VIN1_SOF,
        [2U] = IRQ_VIN2_SOF,
        [3U] = IRQ_VIN3_SOF,
        [4U] = IRQ_VIN4_SOF,
        [5U] = IRQ_VIN5_SOF,
    },
    .VinEof = {
        [0U] = IRQ_VIN0_EOF,
        [1U] = IRQ_VIN1_EOF,
        [2U] = IRQ_VIN2_EOF,
        [3U] = IRQ_VIN3_EOF,
        [4U] = IRQ_VIN4_EOF,
        [5U] = IRQ_VIN5_EOF,
    },
    .Vout = {
        [0U] = IRQ_VOUT0,
        [1U] = IRQ_VOUT1,
    },
};
#else /* CV5 */
const osal_irq_id_t osal_irq_id = {
    .DspCode = {
        [0U] = IRQ_VDSP0,
        [1U] = IRQ_VDSP1,
        [2U] = IRQ_VDSP2,
        [3U] = IRQ_VDSP3,
        [4U] = IRQ_VDSP4,
    },
    .VinSof = {
        [0U]  = IRQ_VIN0_SOF,
        [1U]  = IRQ_VIN1_SOF,
        [2U]  = IRQ_VIN2_SOF,
        [3U]  = IRQ_VIN3_SOF,
        [4U]  = IRQ_VIN4_SOF,
        [5U]  = IRQ_VIN5_SOF,
        [6U]  = IRQ_VIN6_SOF,
        [7U]  = IRQ_VIN7_SOF,
        [8U]  = IRQ_VIN8_SOF,
        [9U]  = IRQ_VIN9_SOF,
        [10U] = IRQ_VIN10_SOF,
        [11U] = IRQ_VIN11_SOF,
        [12U] = IRQ_VIN12_SOF,
        [13U] = IRQ_VIN13_SOF,
    },
    .VinEof = {
        [0U]  = IRQ_VIN0_EOF,
        [1U]  = IRQ_VIN1_EOF,
        [2U]  = IRQ_VIN2_EOF,
        [3U]  = IRQ_VIN3_EOF,
        [4U]  = IRQ_VIN4_EOF,
        [5U]  = IRQ_VIN5_EOF,
        [6U]  = IRQ_VIN6_EOF,
        [7U]  = IRQ_VIN7_EOF,
        [8U]  = IRQ_VIN8_EOF,
        [9U]  = IRQ_VIN9_EOF,
        [10U] = IRQ_VIN10_EOF,
        [11U] = IRQ_VIN11_EOF,
        [12U] = IRQ_VIN12_EOF,
        [13U] = IRQ_VIN13_EOF,
    },
    .Vout = {
        [0U] = IRQ_VOUT0,
        [1U] = IRQ_VOUT1,
        [2U] = IRQ_VOUT2,
    },
};
#endif
#endif

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

#if 0
uint32_t dsp_osal_cma_alloc(const void *arg, uint64_t owner)
{
(void)arg;
(void)owner;
    dsp_osal_printU5("[ERR] dsp_osal_cma_alloc : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003;
}

uint32_t dsp_osal_cma_free(const void *arg)
{
(void)arg;
    dsp_osal_printU5("[ERR] dsp_osal_cma_free : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003;
}

uint32_t dsp_osal_cma_sync_cache(const void *arg)
{
(void) arg;
    dsp_osal_printU5("[ERR] dsp_osal_cma_sync_cache : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003;
}

uint32_t dsp_osal_cma_get_usage(const void *arg)
{
(void)arg;
    dsp_osal_printU5("[ERR] dsp_osal_cma_get_usage : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003;
}

uint32_t dsp_osal_cma_p2v(uint32_t phy, const uint32_t *pCacheFlag, const ULONG *pAddr)
{
(void)phy;
(void)pCacheFlag;
(void)pAddr;
    dsp_osal_printU5("[ERR] dsp_osal_cma_p2v : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003;
}

uint32_t dsp_osal_cma_v2p(ULONG virt, const uint32_t *pCacheFlag)
{
(void)virt;
(void)pCacheFlag;
    dsp_osal_printU5("[ERR] dsp_osal_cma_v2p : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003;
}
#endif

/******************************************************************************/
/*                                  DSP                                       */
/******************************************************************************/
#if defined (CONFIG_SOC_CV22)
#define DBG_PORT_CORC_BASE_ADDR         (0xED030000U)
#define DBG_PORT_EORC_BASE_ADDR         (0xED020000U)
#define DBG_PORT_DORC_BASE_ADDR         (0xED0C0000U)
#define DBG_PORT_SYNCNT_BASE_ADDR       (0xED05F000U)
#elif defined (CONFIG_SOC_CV2)
#define DBG_PORT_CORC_BASE_ADDR         (0xED030000U)
#define DBG_PORT_EORC_BASE_ADDR         (0xED020000U)
#define DBG_PORT_DORC_BASE_ADDR         (0xED0C0000U)
#define DBG_PORT_SYNCNT_BASE_ADDR       (0xED05F000U)
#elif defined (CONFIG_SOC_CV25)
#define DBG_PORT_CORC_BASE_ADDR         (0xED030000U)
#define DBG_PORT_EORC_BASE_ADDR         (0xED020000U)
#define DBG_PORT_DORC_BASE_ADDR         (0xED0C0000U)
#define DBG_PORT_SYNCNT_BASE_ADDR       (0xED05F000U)
#elif defined (CONFIG_SOC_H32)
#define DBG_PORT_CORC_BASE_ADDR         (0xED030000U)
#define DBG_PORT_EORC_BASE_ADDR         (0xED020000U)
#define DBG_PORT_DORC_BASE_ADDR         (0xED0C0000U)
#define DBG_PORT_SYNCNT_BASE_ADDR       (0xED05F000U)
#elif defined (CONFIG_SOC_CV28)
#define DBG_PORT_CORC_BASE_ADDR         (0xED030000U)
#define DBG_PORT_EORC_BASE_ADDR         (0xED020000U)
#define DBG_PORT_DORC_BASE_ADDR         (0xED0C0000U)
#define DBG_PORT_SYNCNT_BASE_ADDR       (0xED05F000U)
#elif defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define DBG_PORT_CORC_BASE_ADDR         (0xED140000)
#define DBG_PORT_CORC1_BASE_ADDR        (0xED150000)
#define DBG_PORT_CORC2_BASE_ADDR        (0xED160000)
#define DBG_PORT_CORC3_BASE_ADDR        (0xED170000)
#define DBG_PORT_EORC_BASE_ADDR         (0xED020000)
#define DBG_PORT_EORC1_BASE_ADDR        (0xED320000)
#define DBG_PORT_DORC_BASE_ADDR         (0xED030000)
#define DBG_PORT_DORC1_BASE_ADDR        (0xED330000)
#define DBG_PORT_SYNCNT_BASE_ADDR       (0xED05F000)
#else //CV2FS
#define DBG_PORT_CORC_BASE_ADDR         (0xED160000U)
#define DBG_PORT_EORC_BASE_ADDR         (0xED020000U)
#define DBG_PORT_DORC_BASE_ADDR         (0xED030000U)
#define DBG_PORT_SYNCNT_BASE_ADDR       (0xED05F000U)
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
#define DBG_PORT_SMEM_GRP_BASE_ADDR     (0xED05E000U)
#define DBG_PORT_SMEM_GRP_OFFSET        (0x4)
#define DBG_PORT_DRAM_PAGE_BASE_ADDR    (0xDFFE0800U)
#define DBG_PORT_DRAM_PAGE_OFFSET       (0x4)
#define DBG_PORT_SMEM_INIT_BASE_ADDR    (0xED05E000U)
#define DBG_PORT_SMEM_INIT_OFFSET       (16)

typedef struct {
    uint8_t  Data[DBG_PORT_SMEM_INIT_OFFSET];
    uint32_t Value;
} dsp_osal_smem_init_t;

/* Init SMEM via Sydney Reader */
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
void dsp_osal_safety_init(void)
{
#ifndef USE_OSAL_WRITEL
    volatile dsp_osal_smem_init_t *pSmemInit;
    uint64_t phys_addr;
    void *virt_addr;
    int32_t retcode;
    uint32_t flag;
#else
    void *pVoid;
    UINT32 SmemInitAddr = 0xED05E010U;
#endif
    UINT32 SmemInitData = 0x808080U;

#ifndef USE_OSAL_WRITEL
    phys_addr = DBG_PORT_SMEM_INIT_BASE_ADDR;
    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
    virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_smem_init_t), (int32_t)flag, 0, phys_addr);
    if (virt_addr == MAP_FAILED) {
        dsp_osal_printI5("[ERR] dsp_osal_safety_init : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
    } else {
        dsp_osal_typecast(&pSmemInit, &virt_addr);
        pSmemInit->Value = SmemInitData;
        dsp_osal_printU5("[DSP_INIT]SmemInit", 0U, 0U, 0U, 0U, 0U);

        retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_smem_init_t));
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_safety_init : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        }
    }
#else
    dsp_osal_typecast(&pVoid, &SmemInitAddr);
    dsp_osal_writel_relaxed(SmemInitData, pVoid);
    dsp_osal_printU5("[DSP_INIT]SmemInit", 0U, 0U, 0U, 0U, 0U);
#endif
}
#endif

typedef struct {
    uint8_t  Data[DBG_PORT_ORC_PC_OFFSET];
    uint32_t PcAddr;
} dsp_osal_orc_pc_t;

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
void dsp_osal_orc_set_pc(const osal_ucode_addr_t *pBasePhysAddr, const uint8_t IdspBinShareMode)
#else
void dsp_osal_orc_set_pc(const osal_ucode_addr_t *pBasePhysAddr)
#endif
{
#ifndef USE_OSAL_WRITEL
    volatile dsp_osal_orc_pc_t *pOrcPc;
    uint64_t phys_addr;
    void *virt_addr;
    int32_t retcode;
    uint32_t flag;
#else
    uint32_t U32Val;
    uint32_t *pU32;
#endif

    if (pBasePhysAddr->CodeAddr != 0U) {
#ifndef USE_OSAL_WRITEL
        phys_addr = DBG_PORT_CORC_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_pc_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_set_pc : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcPc, &virt_addr);
            pOrcPc->PcAddr = (UINT32)pBasePhysAddr->CodeAddr;

            retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_pc_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_orc_set_pc : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            }
        }
#else
        U32Val = (DBG_PORT_CORC_BASE_ADDR+DBG_PORT_ORC_PC_OFFSET);
        pU32 = &U32Val;
        dsp_osal_writel_relaxed((UINT32)pBasePhysAddr->CodeAddr, pU32);
#endif
    }
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    if (pBasePhysAddr->Code1Addr != 0U) {
#ifndef USE_OSAL_WRITEL
        phys_addr = DBG_PORT_CORC1_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_pc_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_set_pc : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcPc, &virt_addr);
            pOrcPc->PcAddr = (UINT32)pBasePhysAddr->Code1Addr;

            retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_pc_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_orc_set_pc : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            }
        }
#else
        U32Val = (DBG_PORT_CORC1_BASE_ADDR+DBG_PORT_ORC_PC_OFFSET);
        pU32 = &U32Val;
        dsp_osal_writel_relaxed((UINT32)pBasePhysAddr->Code1Addr, pU32);
#endif
    }

    if (pBasePhysAddr->Code2Addr != 0U) {
#ifndef USE_OSAL_WRITEL
        phys_addr = DBG_PORT_CORC2_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_pc_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_set_pc : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcPc, &virt_addr);
            pOrcPc->PcAddr = (UINT32)pBasePhysAddr->Code2Addr;

            retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_pc_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_orc_set_pc : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            }
        }
#else
        U32Val = (DBG_PORT_CORC2_BASE_ADDR+DBG_PORT_ORC_PC_OFFSET);
        pU32 = &U32Val;
        dsp_osal_writel_relaxed((UINT32)pBasePhysAddr->Code2Addr, pU32);
#endif
    }

    if (pBasePhysAddr->Code3Addr != 0U) {
#ifndef USE_OSAL_WRITEL
        phys_addr = DBG_PORT_CORC3_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_pc_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_set_pc : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcPc, &virt_addr);
            pOrcPc->PcAddr = (UINT32)pBasePhysAddr->Code3Addr;
            if (IdspBinShareMode == 1U) {
                pOrcPc->PcAddr = (UINT32)pBasePhysAddr->Code2Addr;
            } else {
                pOrcPc->PcAddr = (UINT32)pBasePhysAddr->Code3Addr;
            }

            retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_pc_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_orc_set_pc : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            }
        }
#else
        U32Val = (DBG_PORT_CORC3_BASE_ADDR+DBG_PORT_ORC_PC_OFFSET);
        pU32 = &U32Val;
        if (IdspBinShareMode == 1U) {
            dsp_osal_writel_relaxed((UINT32)pBasePhysAddr->Code2Addr, pU32);
        } else {
            dsp_osal_writel_relaxed((UINT32)pBasePhysAddr->Code3Addr, pU32);
        }
#endif
    }

#endif

    if (pBasePhysAddr->MeAddr != 0U) {
#ifndef USE_OSAL_WRITEL
        phys_addr = DBG_PORT_EORC_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_pc_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_set_pc : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcPc, &virt_addr);
            pOrcPc->PcAddr = (UINT32)pBasePhysAddr->MeAddr;

            retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_pc_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_orc_set_pc : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            }
        }
#else
        U32Val = (DBG_PORT_EORC_BASE_ADDR+DBG_PORT_ORC_PC_OFFSET);
        pU32 = &U32Val;
        dsp_osal_writel_relaxed((UINT32)pBasePhysAddr->MeAddr, pU32);
#endif
    }
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    if (pBasePhysAddr->Me1Addr != 0U) {
#ifndef USE_OSAL_WRITEL
        phys_addr = DBG_PORT_EORC1_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_pc_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_set_pc : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcPc, &virt_addr);
            pOrcPc->PcAddr = (UINT32)pBasePhysAddr->Me1Addr;

            retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_pc_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_orc_set_pc : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            }
        }
#else
        U32Val = (DBG_PORT_EORC1_BASE_ADDR+DBG_PORT_ORC_PC_OFFSET);
        pU32 = &U32Val;
        dsp_osal_writel_relaxed((UINT32)pBasePhysAddr->Me1Addr, pU32);
#endif
    }
#endif

    if (pBasePhysAddr->MdxfAddr != 0U) {
#ifndef USE_OSAL_WRITEL
        phys_addr = DBG_PORT_DORC_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_pc_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_set_pc : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcPc, &virt_addr);
            pOrcPc->PcAddr = (UINT32)pBasePhysAddr->MdxfAddr;

            retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_pc_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_orc_set_pc : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            }
        }
#else
        U32Val = (DBG_PORT_DORC_BASE_ADDR+DBG_PORT_ORC_PC_OFFSET);
        pU32 = &U32Val;
        dsp_osal_writel_relaxed((UINT32)pBasePhysAddr->MdxfAddr, pU32);
#endif
    }
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    if (pBasePhysAddr->Mdxf1Addr != 0U) {
#ifndef USE_OSAL_WRITEL
        phys_addr = DBG_PORT_DORC1_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_pc_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_set_pc : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcPc, &virt_addr);
            pOrcPc->PcAddr = (UINT32)pBasePhysAddr->Mdxf1Addr;

            retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_pc_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_orc_set_pc : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            }
        }
#else
        U32Val = (DBG_PORT_DORC1_BASE_ADDR+DBG_PORT_ORC_PC_OFFSET);
        pU32 = &U32Val;
        dsp_osal_writel_relaxed((UINT32)pBasePhysAddr->Mdxf1Addr, pU32);
#endif
    }
#endif
}

typedef struct {
    uint8_t  Data[DBG_PORT_ORC_RESET_OFFSET];
    uint32_t Reset;
} dsp_osal_orc_reset_t;

void dsp_osal_orc_enable(UINT32 OrcMask)
{
#ifndef USE_OSAL_WRITEL
    volatile dsp_osal_orc_reset_t *pOrcReset;
    uint64_t phys_addr;
    void *virt_addr;
    int32_t retcode;
    uint32_t flag;
#else
    uint32_t U32Val;
    uint32_t *pU32;
#endif

    /* EORC */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_ME0_IDX, 1U)) {
#ifndef USE_OSAL_WRITEL
        phys_addr = DBG_PORT_EORC_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_enable : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcReset, &virt_addr);
            pOrcReset->Reset = DBG_PORT_EORC_RESET_VALUE;

            retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_orc_enable : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            }
        }
#else
        U32Val = (DBG_PORT_EORC_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
        pU32 = &U32Val;
        dsp_osal_writel_relaxed(DBG_PORT_EORC_RESET_VALUE, pU32);
#endif
    }
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    /* EORC1 */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_ME1_IDX, 1U)) {
#ifndef USE_OSAL_WRITEL
        phys_addr = DBG_PORT_EORC1_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_enable : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcReset, &virt_addr);
            pOrcReset->Reset = DBG_PORT_EORC_RESET_VALUE;

            retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_orc_enable : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            }
        }
#else
        U32Val = (DBG_PORT_EORC1_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
        pU32 = &U32Val;
        dsp_osal_writel_relaxed(DBG_PORT_EORC_RESET_VALUE, pU32);
#endif
    }
#endif

    /* DORC */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_MDXF0_IDX, 1U)) {
#ifndef USE_OSAL_WRITEL
        phys_addr = DBG_PORT_DORC_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_enable : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcReset, &virt_addr);
            pOrcReset->Reset = DBG_PORT_DORC_RESET_VALUE;

            retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_orc_enable : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            }
        }
#else
        U32Val = (DBG_PORT_DORC_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
        pU32 = &U32Val;
        dsp_osal_writel_relaxed(DBG_PORT_DORC_RESET_VALUE, pU32);
#endif
    }

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    /* DORC1 */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_MDXF1_IDX, 1U)) {
#ifndef USE_OSAL_WRITEL
        phys_addr = DBG_PORT_DORC1_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_enable : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcReset, &virt_addr);
            pOrcReset->Reset = DBG_PORT_DORC_RESET_VALUE;

            retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_orc_enable : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            }
        }
#else
        U32Val = (DBG_PORT_DORC1_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
        pU32 = &U32Val;
        dsp_osal_writel_relaxed(DBG_PORT_DORC_RESET_VALUE, pU32);
#endif
    }
#endif

    /* CORC */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_CODE_IDX, 1U)) {
#ifndef USE_OSAL_WRITEL
        phys_addr = DBG_PORT_CORC_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_enable : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcReset, &virt_addr);
            pOrcReset->Reset = DBG_PORT_CORC_RESET_VALUE;

            retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_orc_enable : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            }
        }
#else
        U32Val = (DBG_PORT_CORC_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
        pU32 = &U32Val;
        dsp_osal_writel_relaxed(DBG_PORT_CORC_RESET_VALUE, pU32);
#endif
    }

    /* per ChenHan's suggest on 2021/6/1, orccode -> orcidsps -> orcvin */
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    /* CORC3 */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_IDSP1_IDX, 1U)) {
#ifndef USE_OSAL_WRITEL
        phys_addr = DBG_PORT_CORC3_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_enable : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcReset, &virt_addr);
            pOrcReset->Reset = DBG_PORT_CORC_RESET_VALUE;

            retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_orc_enable : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            }
        }
#else
        U32Val = (DBG_PORT_CORC3_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
        pU32 = &U32Val;
        dsp_osal_writel_relaxed(DBG_PORT_CORC_RESET_VALUE, pU32);
#endif
    }

    /* CORC2 */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_IDSP0_IDX, 1U)) {
#ifndef USE_OSAL_WRITEL
        phys_addr = DBG_PORT_CORC2_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_enable : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcReset, &virt_addr);
            pOrcReset->Reset = DBG_PORT_CORC_RESET_VALUE;

            retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_orc_enable : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            }
        }
#else
        U32Val = (DBG_PORT_CORC2_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
        pU32 = &U32Val;
        dsp_osal_writel_relaxed(DBG_PORT_CORC_RESET_VALUE, pU32);
#endif
    }

    /* CORC1 */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_VIN_IDX, 1U)) {
#ifndef USE_OSAL_WRITEL
        phys_addr = DBG_PORT_CORC1_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_enable : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcReset, &virt_addr);
            pOrcReset->Reset = DBG_PORT_CORC_RESET_VALUE;

            retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
            if (retcode == -1) {
                dsp_osal_printI5("[ERR] dsp_osal_orc_enable : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
            }
        }
#else
        U32Val = (DBG_PORT_CORC1_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
        pU32 = &U32Val;
        dsp_osal_writel_relaxed(DBG_PORT_CORC_RESET_VALUE, pU32);
#endif
    }
#endif

}

void dsp_osal_orc_shutdown(void)
{
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#ifndef USE_OSAL_WRITEL
    volatile dsp_osal_orc_reset_t *pOrcReset;
    uint64_t phys_addr;
    void *virt_addr;
    int32_t retcode;
    uint32_t flag;
#else
    uint32_t U32Val;
    uint32_t *pU32;
#endif

    /* Reset DORC/EORC first to prevent CORC trying to access them */

    /* EORC */
#ifndef USE_OSAL_WRITEL
    phys_addr = DBG_PORT_EORC_BASE_ADDR;
    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
    virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
    if (virt_addr == MAP_FAILED) {
        dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
    } else {
        dsp_osal_typecast(&pOrcReset, &virt_addr);
        pOrcReset->Reset = DBG_PORT_EORC_SHUTDOWN_VALUE;

        retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        }
    }
#else
    U32Val = (DBG_PORT_EORC_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
    pU32 = &U32Val;
    dsp_osal_writel_relaxed(DBG_PORT_EORC_SHUTDOWN_VALUE, pU32);
#endif


    /* EORC1 */
#ifndef USE_OSAL_WRITEL
    phys_addr = DBG_PORT_EORC1_BASE_ADDR;
    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
    virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
    if (virt_addr == MAP_FAILED) {
        dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
    } else {
        dsp_osal_typecast(&pOrcReset, &virt_addr);
        pOrcReset->Reset = DBG_PORT_EORC_SHUTDOWN_VALUE;

        retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        }
    }
#else
    U32Val = (DBG_PORT_EORC1_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
    pU32 = &U32Val;
    dsp_osal_writel_relaxed(DBG_PORT_EORC_SHUTDOWN_VALUE, pU32);
#endif

    /* DORC */
#ifndef USE_OSAL_WRITEL
    phys_addr = DBG_PORT_DORC_BASE_ADDR;
    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
    virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
    if (virt_addr == MAP_FAILED) {
        dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
    } else {
        dsp_osal_typecast(&pOrcReset, &virt_addr);
        pOrcReset->Reset = DBG_PORT_DORC_SHUTDOWN_VALUE;

        retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        }
    }
#else
    U32Val = (DBG_PORT_DORC_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
    pU32 = &U32Val;
    dsp_osal_writel_relaxed(DBG_PORT_DORC_SHUTDOWN_VALUE, pU32);
#endif

    /* DORC1 */
#ifndef USE_OSAL_WRITEL
    phys_addr = DBG_PORT_DORC1_BASE_ADDR;
    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
    virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
    if (virt_addr == MAP_FAILED) {
        dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
    } else {
        dsp_osal_typecast(&pOrcReset, &virt_addr);
        pOrcReset->Reset = DBG_PORT_DORC_SHUTDOWN_VALUE;

        retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        }
    }
#else
    U32Val = (DBG_PORT_DORC1_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
    pU32 = &U32Val;
    dsp_osal_writel_relaxed(DBG_PORT_DORC_SHUTDOWN_VALUE, pU32);
#endif

    /* CORC */
#ifndef USE_OSAL_WRITEL
    phys_addr = DBG_PORT_CORC_BASE_ADDR;
    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
    virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
    if (virt_addr == MAP_FAILED) {
        dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
    } else {
        dsp_osal_typecast(&pOrcReset, &virt_addr);
        pOrcReset->Reset = DBG_PORT_CORC_SHUTDOWN_VALUE;

        retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        }
    }
#else
    U32Val = (DBG_PORT_CORC_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
    pU32 = &U32Val;
    dsp_osal_writel_relaxed(DBG_PORT_CORC_SHUTDOWN_VALUE, pU32);
#endif

    /* CORC3 */
#ifndef USE_OSAL_WRITEL
    phys_addr = DBG_PORT_CORC3_BASE_ADDR;
    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
    virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
    if (virt_addr == MAP_FAILED) {
        dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
    } else {
        dsp_osal_typecast(&pOrcReset, &virt_addr);
        pOrcReset->Reset = DBG_PORT_CORC_SHUTDOWN_VALUE;

        retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        }
    }
#else
    U32Val = (DBG_PORT_CORC3_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
    pU32 = &U32Val;
    dsp_osal_writel_relaxed(DBG_PORT_CORC_SHUTDOWN_VALUE, pU32);
#endif

    /* CORC2 */
#ifndef USE_OSAL_WRITEL
    phys_addr = DBG_PORT_CORC2_BASE_ADDR;
    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
    virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
    if (virt_addr == MAP_FAILED) {
        dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
    } else {
        dsp_osal_typecast(&pOrcReset, &virt_addr);
        pOrcReset->Reset = DBG_PORT_CORC_SHUTDOWN_VALUE;

        retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        }
    }
#else
    U32Val = (DBG_PORT_CORC2_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
    pU32 = &U32Val;
    dsp_osal_writel_relaxed(DBG_PORT_CORC_SHUTDOWN_VALUE, pU32);
#endif

    /* CORC1 */
#ifndef USE_OSAL_WRITEL
    phys_addr = DBG_PORT_CORC1_BASE_ADDR;
    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
    virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
    if (virt_addr == MAP_FAILED) {
        dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
    } else {
        dsp_osal_typecast(&pOrcReset, &virt_addr);
        pOrcReset->Reset = DBG_PORT_CORC_SHUTDOWN_VALUE;

        retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        }
    }
#else
    U32Val = (DBG_PORT_CORC1_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
    pU32 = &U32Val;
    dsp_osal_writel_relaxed(DBG_PORT_CORC_SHUTDOWN_VALUE, pU32);
#endif

#elif defined (CONFIG_SOC_CV22) || defined (CONFIG_SOC_CV2) || defined (CONFIG_SOC_CV25) || defined (CONFIG_SOC_CV28) || defined (CONFIG_SOC_H32)
#ifndef USE_OSAL_WRITEL
    volatile dsp_osal_orc_reset_t *pOrcReset;
    uint64_t phys_addr;
    void *virt_addr;
    int32_t retcode;
    uint32_t flag;
#else
    uint32_t U32Val;
    uint32_t *pU32;
#endif

    /* Reset DORC/EORC first to prevent CORC trying to access them */

    /* EORC */
#ifndef USE_OSAL_WRITEL
    phys_addr = DBG_PORT_EORC_BASE_ADDR;
    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
    virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
    if (virt_addr == MAP_FAILED) {
        dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
    } else {
        dsp_osal_typecast(&pOrcReset, &virt_addr);
        pOrcReset->Reset = DBG_PORT_EORC_SHUTDOWN_VALUE;

        retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        }
    }
#else
    U32Val = (DBG_PORT_EORC_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
    pU32 = &U32Val;
    dsp_osal_writel_relaxed(DBG_PORT_EORC_SHUTDOWN_VALUE, pU32);
#endif

    /* DORC */
#ifndef USE_OSAL_WRITEL
    phys_addr = DBG_PORT_DORC_BASE_ADDR;
    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
    virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
    if (virt_addr == MAP_FAILED) {
        dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
    } else {
        dsp_osal_typecast(&pOrcReset, &virt_addr);
        pOrcReset->Reset = DBG_PORT_DORC_SHUTDOWN_VALUE;

        retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        }
    }
#else
    U32Val = (DBG_PORT_DORC_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
    pU32 = &U32Val;
    dsp_osal_writel_relaxed(DBG_PORT_DORC_SHUTDOWN_VALUE, pU32);
#endif

    /* CORC */
#ifndef USE_OSAL_WRITEL
    phys_addr = DBG_PORT_CORC_BASE_ADDR;
    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
    virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_reset_t), (int32_t)flag, 0, phys_addr);
    if (virt_addr == MAP_FAILED) {
        dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
    } else {
        dsp_osal_typecast(&pOrcReset, &virt_addr);
        pOrcReset->Reset = DBG_PORT_CORC_SHUTDOWN_VALUE;

        retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_reset_t));
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_orc_shutdown : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        }
    }
#else
    U32Val = (DBG_PORT_CORC_BASE_ADDR+DBG_PORT_ORC_RESET_OFFSET);
    pU32 = &U32Val;
    dsp_osal_writel_relaxed(DBG_PORT_CORC_SHUTDOWN_VALUE, pU32);
#endif

#else
    // TBD
#endif
}

typedef struct {
    volatile uint32_t SyncCnt[DBG_PORT_SYNCNT_NUM];
} dsp_osal_orc_sync_cnt_t;

void dsp_osal_dbgport_set_sync_cnt(uint32_t sync_cnt_id, uint32_t val)
{
    static dsp_osal_orc_sync_cnt_t *pOrcSyncCnt = NULL;
    uint64_t phys_addr;
    void *virt_addr;
    uint32_t reg_val;
    uint32_t flag;

    reg_val = ((val << 12) | val);
    /* DspDriver will access SyncCnt frequently */
    /* Keep the SyncCnt virt-addr to prevent too much mmap-ops */
    if (pOrcSyncCnt == NULL) {
        phys_addr = DBG_PORT_SYNCNT_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_sync_cnt_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_dbgport_set_sync_cnt : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&pOrcSyncCnt, &virt_addr);
        }
    }

    if (pOrcSyncCnt != NULL) {
        pOrcSyncCnt->SyncCnt[sync_cnt_id] = reg_val;
    }
}

typedef struct {
    uint8_t  Data[DBG_PORT_SMEM_GRP_OFFSET];
    uint32_t SmemGrp;
} dsp_osal_orc_smem_grp_t;

void dsp_osal_dbgport_set_smem_grp(void)
{
#ifndef USE_OSAL_WRITEL
    volatile dsp_osal_orc_smem_grp_t *pOrcSmemGrp;
    uint64_t phys_addr;
    void *virt_addr;
    int32_t retcode;
    uint32_t flag;
#else
    void *pVoid;
    UINT32 SmemGrpAddr = 0xED05E004U;
#endif
    UINT32 SmemGrpData = 0x83000002U; // b[31] for smem grouping

#ifndef USE_OSAL_WRITEL
    phys_addr = DBG_PORT_SMEM_GRP_BASE_ADDR;
    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
    virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_smem_grp_t), (int32_t)flag, 0, phys_addr);
    if (virt_addr == MAP_FAILED) {
        dsp_osal_printI5("[ERR] dsp_osal_dbgport_set_smem_grp : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
    } else {
        dsp_osal_typecast(&pOrcSmemGrp, &virt_addr);
        pOrcSmemGrp->SmemGrp = SmemGrpData;

        retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_smem_grp_t));
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_dbgport_set_smem_grp : munmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        }
    }
#else
    dsp_osal_typecast(&pVoid, &SmemGrpAddr);
    dsp_osal_writel_relaxed(SmemGrpData, pVoid);
    dsp_osal_printU5("[DSP_INIT]SmemGrp", 0U, 0U, 0U, 0U, 0U);
#endif
}

typedef struct {
    uint8_t  Data[DBG_PORT_ORC_TIMER_OFFSET];
    uint32_t OrcTimer;
} dsp_osal_orc_timer_t;

uint32_t dsp_osal_get_orc_timer(void)
{
#ifndef USE_OSAL_READL
    const volatile dsp_osal_orc_timer_t *pOrcTimer;
    uint32_t value = 0U;
    uint64_t phys_addr;
    void *virt_addr;
    int32_t retcode;
    uint32_t flag;

    phys_addr = DBG_PORT_CORC_BASE_ADDR;
    flag = (uint32_t)PROT_READ | (uint32_t)PROT_NOCACHE;
    virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_orc_timer_t), (int32_t)flag, 0, phys_addr);
    if (virt_addr == MAP_FAILED) {
        dsp_osal_printI5("[ERR] dsp_osal_get_orc_timer : mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
    } else {
        dsp_osal_typecast(&pOrcTimer, &virt_addr);
        value = pOrcTimer->OrcTimer;

        retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_orc_timer_t));
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_get_orc_timer : munmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
        }
    }

    return value;
#else
    uint32_t U32Addr;
    void *pVoid;
    U32Addr = (DBG_PORT_CORC_BASE_ADDR+DBG_PORT_ORC_TIMER_OFFSET);
    (void)dsp_osal_memcpy(&pVoid, &U32Addr, sizeof(UINT32));

    return dsp_osal_readl_relaxed(pVoid);
#endif
}

typedef struct {
    uint8_t  Data[DBG_PORT_DRAM_PAGE_OFFSET];
    uint32_t PageSize;
} dsp_osal_dram_page_t;

uint32_t dsp_osal_get_dram_page(void)
{
#ifndef USE_OSAL_READL
    const volatile dsp_osal_dram_page_t *pDramPage;
    uint32_t value = 0U;
    uint64_t phys_addr;
    void *virt_addr;
    int32_t retcode;
    uint32_t flag;

    phys_addr = DBG_PORT_DRAM_PAGE_BASE_ADDR;
    flag = (uint32_t)PROT_READ | (uint32_t)PROT_NOCACHE;
    virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_dram_page_t), (int32_t)flag, 0, phys_addr);
    if (virt_addr == MAP_FAILED) {
        dsp_osal_printI5("[ERR] dsp_osal_get_dram_page : mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
    } else {
        dsp_osal_typecast(&pDramPage, &virt_addr);
        value = pDramPage->PageSize;

        retcode = munmap_device_memory(virt_addr, sizeof(dsp_osal_dram_page_t));
        if (retcode == -1) {
            dsp_osal_printI5("[ERR] dsp_osal_get_dram_page : munmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
        }
    }

    return value;
#else
    const UINT32 DramCAddr = 0xDFFE0804U;
    void *pVoid;

    (void)dsp_osal_memcpy(&pVoid, &DramCAddr, sizeof(UINT32));
    return dsp_osal_readl_relaxed(pVoid);
#endif
}

void dsp_osal_get_cmd_req_timeout_cfg(uint32_t *pEnable, uint32_t *pTime)
{
    if ((pEnable == NULL) ||
        (pTime == NULL)) {
        dsp_osal_printU5("[ERR] dsp_osal_get_cmd_req_timeout_cfg : Null input", 0U, 0U, 0U, 0U, 0U);
    } else {
#define DSP_CMD_REQ_TIMEOUT_CHECK   (0U) // cmd-req-msg has timeout information, disable assertion
#define DSP_CMD_REQ_TIME            (4U) // sync counter timer in ms. ( only 1 ~ 4 ms allowed )
        *pEnable = DSP_CMD_REQ_TIMEOUT_CHECK;
        *pTime = DSP_CMD_REQ_TIME;

    }
}

static osal_buf_t ucode_bin_buf[UCODE_BIN_NUM] = {0};
static inline uint32_t dsp_osal_alloc_ucode_addr(void)
{
    static int32_t alloc_fd_dsp_bin = 0;
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    struct posix_typed_mem_info info;
    uint32_t needed_dsp_bin_sz = 0;
    void *virt_addr;
    off_t offset;
    ULONG Base_start_virt_addr, start_virt_addr;
    uint32_t start_phys_addr;
    uint64_t used_size_offset = 0, used_size = 0;
    uint32_t i;
    uint32_t flag;
    size_t contig_len = 0U;
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

    /* From experience, first time use ALLOC_CONTIG, following use MAP_ALLOCATABLE */
    alloc_fd_dsp_bin = posix_typed_mem_open("/ram/dsp_bin_buf", O_RDWR, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
    if (alloc_fd_dsp_bin < 0) {
        Rval = DSP_ERR_0000;
        dsp_osal_printI5("[ERR] dsp_osal_alloc_ucode_addr() : posix_typed_mem_open fail(/ram/dsp_bin_buf) fd = %d errno = %d", alloc_fd_dsp_bin, errno, 0, 0, 0);
    } else {
        retcode = posix_typed_mem_get_info(alloc_fd_dsp_bin, &info);
        if (retcode < 0) {
            Rval = DSP_ERR_0003;
            dsp_osal_printI5("[ERR] dsp_osal_alloc_ucode_addr() : posix_typed_mem_get_info fail ret = %d errno = %d", retcode, errno, 0, 0, 0);
        } else {
            for (i = 0U; i<UCODE_BIN_NUM; i++) {
                needed_dsp_bin_sz += DSP_BUF_ADDR_ALIGN;
                needed_dsp_bin_sz += ucode_bin_size[i];
            }

            if (needed_dsp_bin_sz > info.posix_tmi_length) {
                Rval = DSP_ERR_0003;
                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : total_dsp_bin_sz (0x%X) > info.posix_tmi_length (0x%X)", needed_dsp_bin_sz, (uint32_t)info.posix_tmi_length, 0U, 0U, 0U);
            } else {
                flag = (uint32_t)PROT_NOCACHE | (uint32_t)PROT_READ | (uint32_t)PROT_WRITE;
                virt_addr = mmap(NULL, info.posix_tmi_length, (int32_t)flag, MAP_SHARED, alloc_fd_dsp_bin, 0);
                if (virt_addr == MAP_FAILED) {
                    Rval = DSP_ERR_0003;
                    dsp_osal_printI5("[ERR] dsp_osal_alloc_ucode_addr() : mmap fail errno = %d", errno, 0, 0, 0, 0);
                } else {
                    dsp_osal_typecast(&Base_start_virt_addr, &virt_addr);
                    contig_len = 0U;
                    retcode = mem_offset(virt_addr, NOFD, 1, &offset, &contig_len);
                    start_virt_addr = ADDR_ALIGN_NUM(Base_start_virt_addr, DSP_BUF_ADDR_ALIGN);
                    if (retcode < 0) {
                        Rval = DSP_ERR_0003;
                        dsp_osal_printI5("[ERR] dsp_osal_alloc_ucode_addr() : mem_offset fail ret = %d errno = %d", retcode, errno, 0, 0, 0);
                    } else {
                        /* update buf_pool */
                        dsp_osal_typecast(&osal_buf_pool[DSP_BUF_POOL_BINARY].Base, &virt_addr);
                        osal_buf_pool[DSP_BUF_POOL_BINARY].size = (uint32_t)info.posix_tmi_length;
                        osal_buf_pool[DSP_BUF_POOL_BINARY].UsedSize = 0U;
                        osal_buf_pool[DSP_BUF_POOL_BINARY].is_cached = 0U;
                        Rval = dsp_osal_memcpy(&osal_buf_pool[DSP_BUF_POOL_BINARY].PhysBase, &offset, sizeof(uint32_t));
                        dsp_osal_print_err_line(Rval, __func__, __LINE__);
                        dsp_osal_printU5("[dspdev] mempool[%d] Virt 0x%X%X Phys 0x%X Sz %d",
                                          DSP_BUF_POOL_BINARY,
                                          dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_BINARY].Base),
                                          dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_BINARY].Base),
                                          osal_buf_pool[DSP_BUF_POOL_BINARY].PhysBase,
                                          osal_buf_pool[DSP_BUF_POOL_BINARY].size);
                    }

                    if (retcode == 0) {
                        dsp_osal_typecast(&virt_addr, &start_virt_addr);
                        contig_len = 0U;
                        retcode = mem_offset(virt_addr, NOFD, 1, &offset, &contig_len);
                        if (retcode < 0) {
                            Rval = DSP_ERR_0003;
                            dsp_osal_printI5("[ERR] dsp_osal_alloc_ucode_addr() : mem_offset fail ret = %d errno = %d", retcode, errno, 0, 0, 0);
                        } else {
                            (void)dsp_osal_memcpy(&start_phys_addr, &offset, sizeof(uint32_t));
                            if (start_virt_addr > Base_start_virt_addr) {
                                used_size_offset += (start_virt_addr - Base_start_virt_addr);
                            }

                            /* Allocate orcode */
                            ucode_bin_buf[UCODE_BIN_ORCODE].Base = start_virt_addr;
                            ucode_bin_buf[UCODE_BIN_ORCODE].size = ucode_bin_size[UCODE_BIN_ORCODE];
                            ucode_bin_buf[UCODE_BIN_ORCODE].pool_id = DSP_BUF_POOL_BINARY;
                            used_size += ucode_bin_size[UCODE_BIN_ORCODE];
                            used_size = ALIGN_NUM64(used_size, DSP_BUF_ADDR_ALIGN);

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
                            /* Allocate orcode1 */
                            if ((used_size_offset + used_size) < info.posix_tmi_length) {
                                ucode_bin_buf[UCODE_BIN_ORCODE1].Base = start_virt_addr + used_size;
                                ucode_bin_buf[UCODE_BIN_ORCODE1].size = ucode_bin_size[UCODE_BIN_ORCODE1];
                                ucode_bin_buf[UCODE_BIN_ORCODE1].pool_id = DSP_BUF_POOL_BINARY;
                                used_size += ucode_bin_size[UCODE_BIN_ORCODE1];
                                used_size = ALIGN_NUM64(used_size, DSP_BUF_ADDR_ALIGN);
                            } else {
                                Rval = DSP_ERR_0003;
                                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                            }

                            /* Allocate orcode2 */
                            if ((used_size_offset + used_size) < info.posix_tmi_length) {
                                ucode_bin_buf[UCODE_BIN_ORCODE2].Base = start_virt_addr + used_size;
                                ucode_bin_buf[UCODE_BIN_ORCODE2].size = ucode_bin_size[UCODE_BIN_ORCODE2];
                                ucode_bin_buf[UCODE_BIN_ORCODE2].pool_id = DSP_BUF_POOL_BINARY;
                                used_size += ucode_bin_size[UCODE_BIN_ORCODE2];
                                used_size = ALIGN_NUM64(used_size, DSP_BUF_ADDR_ALIGN);
                            } else {
                                Rval = DSP_ERR_0003;
                                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                            }

                            /* Allocate orcode3 */
                            if ((used_size_offset + used_size) < info.posix_tmi_length) {
                                ucode_bin_buf[UCODE_BIN_ORCODE3].Base = start_virt_addr + used_size;
                                ucode_bin_buf[UCODE_BIN_ORCODE3].size = ucode_bin_size[UCODE_BIN_ORCODE3];
                                ucode_bin_buf[UCODE_BIN_ORCODE3].pool_id = DSP_BUF_POOL_BINARY;
                                used_size += ucode_bin_size[UCODE_BIN_ORCODE3];
                                used_size = ALIGN_NUM64(used_size, DSP_BUF_ADDR_ALIGN);
                            } else {
                                Rval = DSP_ERR_0003;
                                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                            }
#endif

                            /* Allocate orcme */
                            if ((used_size_offset + used_size) < info.posix_tmi_length) {
                                ucode_bin_buf[UCODE_BIN_ORCME].Base = start_virt_addr + used_size;
                                ucode_bin_buf[UCODE_BIN_ORCME].size = ORCME_BIN_MAX_SIZE;
                                ucode_bin_buf[UCODE_BIN_ORCME].pool_id = DSP_BUF_POOL_BINARY;
                                used_size += ORCME_BIN_MAX_SIZE;
                                used_size = ALIGN_NUM64(used_size, DSP_BUF_ADDR_ALIGN);
                            } else {
                                Rval = DSP_ERR_0003;
                                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                            }

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
                            /* Allocate orcme1 */
                            if ((used_size_offset + used_size) < info.posix_tmi_length) {
                                ucode_bin_buf[UCODE_BIN_ORCME1].Base = start_virt_addr + used_size;
                                ucode_bin_buf[UCODE_BIN_ORCME1].size = ucode_bin_size[UCODE_BIN_ORCME1];
                                ucode_bin_buf[UCODE_BIN_ORCME1].pool_id = DSP_BUF_POOL_BINARY;
                                used_size += ucode_bin_size[UCODE_BIN_ORCME1];
                                used_size = ALIGN_NUM64(used_size, DSP_BUF_ADDR_ALIGN);
                            } else {
                                Rval = DSP_ERR_0003;
                                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                            }
#endif

                            /* Allocate orcmd */
                            if ((Rval == DSP_ERR_NONE) &&
                                ((used_size_offset + used_size) < info.posix_tmi_length)) {
                                ucode_bin_buf[UCODE_BIN_ORCMD].Base = start_virt_addr + used_size;
                                ucode_bin_buf[UCODE_BIN_ORCMD].size = ORCMD_BIN_MAX_SIZE;
                                ucode_bin_buf[UCODE_BIN_ORCMD].pool_id = DSP_BUF_POOL_BINARY;
                                used_size += ORCMD_BIN_MAX_SIZE;
                                used_size = ALIGN_NUM64(used_size, DSP_BUF_ADDR_ALIGN);
                            } else {
                                Rval = DSP_ERR_0003;
                                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                            }

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
                            /* Allocate orcmd1 */
                            if ((used_size_offset + used_size) < info.posix_tmi_length) {
                                ucode_bin_buf[UCODE_BIN_ORCMD1].Base = start_virt_addr + used_size;
                                ucode_bin_buf[UCODE_BIN_ORCMD1].size = ucode_bin_size[UCODE_BIN_ORCMD1];
                                ucode_bin_buf[UCODE_BIN_ORCMD1].pool_id = DSP_BUF_POOL_BINARY;
                                used_size += ucode_bin_size[UCODE_BIN_ORCMD1];
                                used_size = ALIGN_NUM64(used_size, DSP_BUF_ADDR_ALIGN);
                            } else {
                                Rval = DSP_ERR_0003;
                                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                            }
#endif

                            /* Allocate default binary */
                            if ((Rval == DSP_ERR_NONE) &&
                                ((used_size_offset + used_size) < info.posix_tmi_length)) {
                                ucode_bin_buf[UCODE_BIN_DEFAULT].Base = start_virt_addr + used_size;
                                ucode_bin_buf[UCODE_BIN_DEFAULT].size = ucode_bin_size[UCODE_BIN_DEFAULT];
                                ucode_bin_buf[UCODE_BIN_DEFAULT].pool_id = DSP_BUF_POOL_BINARY;
                            } else {
                                Rval = DSP_ERR_0003;
                                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                            }
                        }
                    }
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
static inline uint32_t dsp_osal_alloc_prot_addr(void)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    static int32_t alloc_fd_dsp_prot = 0;
    struct posix_typed_mem_info info;
    void *virt_addr;
    off_t offset;
    uint32_t flag;
    size_t contig_len;

    /* From experience, first time use ALLOC_CONTIG, following use MAP_ALLOCATABLE */
    alloc_fd_dsp_prot = posix_typed_mem_open("/ram/dsp_prot_buf", O_RDWR, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
    if (alloc_fd_dsp_prot < 0) {
        Rval = DSP_ERR_0003;
        dsp_osal_printI5("[ERR] dsp_osal_alloc_prot_addr() : posix_typed_mem_open fail(/ram/dsp_prot_buf) fd = %d errno = %d", alloc_fd_dsp_prot, errno, 0, 0, 0);
    } else {
        retcode = posix_typed_mem_get_info(alloc_fd_dsp_prot, &info);
        if (retcode < 0) {
            Rval = DSP_ERR_0003;
            dsp_osal_printI5("[ERR] dsp_osal_alloc_prot_addr() : posix_typed_mem_get_info fail ret = %d errno = %d", retcode, errno, 0, 0, 0);
        } else {
            flag = (uint32_t)PROT_NOCACHE | (uint32_t)PROT_READ | (uint32_t)PROT_WRITE;
            virt_addr = mmap(NULL, info.posix_tmi_length, (int32_t)flag, MAP_SHARED, alloc_fd_dsp_prot, 0);
            if (virt_addr == MAP_FAILED) {
                Rval = DSP_ERR_0003;
                dsp_osal_printI5("[ERR] dsp_osal_alloc_prot_addr() : mmap fail errno = %d", errno, 0, 0, 0, 0);
            } else {
                contig_len = 0U;
                retcode = mem_offset(virt_addr, NOFD, 1, &offset, &contig_len);
                if (retcode < 0) {
                    Rval = DSP_ERR_0003;
                    dsp_osal_printI5("[ERR] dsp_osal_alloc_prot_addr() : mem_offset fail ret = %d errno = %d", retcode, errno, 0, 0, 0);
                } else {
                    /* update buf_pool */
                    dsp_osal_typecast(&osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base, &virt_addr);
                    osal_buf_pool[DSP_BUF_POOL_PROTOCOL].size = (uint32_t)info.posix_tmi_length;
                    osal_buf_pool[DSP_BUF_POOL_PROTOCOL].UsedSize = 0U;
                    osal_buf_pool[DSP_BUF_POOL_PROTOCOL].is_cached = 0U;
                    Rval = dsp_osal_memcpy(&osal_buf_pool[DSP_BUF_POOL_PROTOCOL].PhysBase, &offset, sizeof(uint32_t));
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    dsp_osal_printU5("[dspdev] mempool[%d] Virt 0x%X Phys 0x%X Sz %d Cached %d",
                                      DSP_BUF_POOL_PROTOCOL,
                                      (uint32_t)osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base,
                                      osal_buf_pool[DSP_BUF_POOL_PROTOCOL].PhysBase,
                                      osal_buf_pool[DSP_BUF_POOL_PROTOCOL].size,
                                      osal_buf_pool[DSP_BUF_POOL_PROTOCOL].is_cached);
                }
            }
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
static inline uint32_t dsp_osal_alloc_stat_addr(void)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    static int32_t alloc_fd_dsp_stat = 0;
    struct posix_typed_mem_info info;
    void *virt_addr;
    off_t offset;
    uint32_t flag;
    size_t contig_len;

    /* From experience, first time use ALLOC_CONTIG, following use MAP_ALLOCATABLE */
    alloc_fd_dsp_stat = posix_typed_mem_open("/ram/dsp_prot_cache_buf", O_RDWR, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
    if (alloc_fd_dsp_stat < 0) {
        Rval = DSP_ERR_0003;
        dsp_osal_printI5("[ERR] dsp_osal_alloc_stat_addr() : posix_typed_mem_open fail(/ram/dsp_prot_cache_buf) fd = %d errno = %d", alloc_fd_dsp_stat, errno, 0, 0, 0);
    } else {
        retcode = posix_typed_mem_get_info(alloc_fd_dsp_stat, &info);
        if (retcode < 0) {
            Rval = DSP_ERR_0003;
            dsp_osal_printI5("[ERR] dsp_osal_alloc_stat_addr() : posix_typed_mem_get_info fail ret = %d errno = %d", retcode, errno, 0, 0, 0);
        } else {
            flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE;
            virt_addr = mmap(NULL, info.posix_tmi_length, (int32_t)flag, MAP_SHARED, alloc_fd_dsp_stat, 0);
            if (virt_addr == MAP_FAILED) {
                Rval = DSP_ERR_0003;
                dsp_osal_printI5("[ERR] dsp_osal_alloc_stat_addr() : mmap fail errno = %d", errno, 0, 0, 0, 0);
            } else {
                contig_len = 0U;
                retcode = mem_offset(virt_addr, NOFD, 1, &offset, &contig_len);
                if (retcode < 0) {
                    Rval = DSP_ERR_0003;
                    dsp_osal_printI5("[ERR] dsp_osal_alloc_stat_addr() : mem_offset fail ret = %d errno = %d", retcode, errno, 0, 0, 0);
                } else {
                    /* update buf_pool */
                    dsp_osal_typecast(&osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base, &virt_addr);
                    osal_buf_pool[DSP_BUF_POOL_STATISTIC].size = (uint32_t)info.posix_tmi_length;
                    osal_buf_pool[DSP_BUF_POOL_STATISTIC].UsedSize = 0U;
                    osal_buf_pool[DSP_BUF_POOL_STATISTIC].is_cached = 1U;
                    Rval = dsp_osal_memcpy(&osal_buf_pool[DSP_BUF_POOL_STATISTIC].PhysBase, &offset, sizeof(uint32_t));
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    dsp_osal_printU5("[dspdev] mempool[%d] Virt 0x%X Phys 0x%X Sz %d Cached %d",
                                      DSP_BUF_POOL_STATISTIC,
                                      (uint32_t)osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base,
                                      osal_buf_pool[DSP_BUF_POOL_STATISTIC].PhysBase,
                                      osal_buf_pool[DSP_BUF_POOL_STATISTIC].size,
                                      osal_buf_pool[DSP_BUF_POOL_STATISTIC].is_cached);
                }
            }
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

UINT32 dsp_osal_alloc_add_buf_addr(UINT32 Type, UINT32 phys_addr, UINT32 size, UINT32 IsCached)
{
    UINT32 Rval = DSP_ERR_NONE;
    int32_t retcode;
    void *virt_addr;
    uint32_t flag;

    if ((Type == DSP_BUF_POOL_WORK) ||
        (Type == DSP_BUF_POOL_DBG_LOG) ||
        (Type == DSP_BUF_POOL_DATA)) {
        /* un-map */
        if (osal_buf_pool[Type].Base != 0U) {
            dsp_osal_typecast(&virt_addr, &osal_buf_pool[Type].Base);
            retcode = munmap_device_memory(virt_addr, osal_buf_pool[Type].size);
            if (retcode == -1) {
                Rval = DSP_ERR_0003;
                dsp_osal_printI5("[ERR] dsp_osal_alloc_add_buf_addr : munmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
            }
        }

        /* map */
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE;
        if (IsCached == 0U) {
            flag |= (uint32_t)PROT_NOCACHE;
        }
        virt_addr = mmap_device_memory(NULL, size, (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            Rval = DSP_ERR_0003;
            dsp_osal_printI5("[ERR] dsp_osal_alloc_add_buf_addr : mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
        } else {
            dsp_osal_typecast(&osal_buf_pool[Type].Base, &virt_addr);
            osal_buf_pool[Type].PhysBase = phys_addr;
            osal_buf_pool[Type].size = size;
            osal_buf_pool[Type].UsedSize = 0U;
            osal_buf_pool[Type].is_cached = IsCached;
            dsp_osal_printU5("[dspdev] mempool[%d] Virt 0x%X Phys 0x%X Sz %d Cached %d",
                              Type,
                              (UINT32)osal_buf_pool[Type].Base,
                              osal_buf_pool[Type].PhysBase,
                              osal_buf_pool[Type].size,
                              osal_buf_pool[Type].is_cached);
        }
    } else {
        // DO NOTHING
    }

    return Rval;
}

#if defined (CONFIG_DEVICE_TREE_SUPPORT)
/* allocate dsp data buffer */
static inline uint32_t dsp_osal_alloc_data_addr(void)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    static int32_t alloc_fd_dsp_data = 0;
    struct posix_typed_mem_info info;
    void *virt_addr;
    off_t offset;
    uint32_t flag;
    size_t contig_len;

    /* From experience, first time use ALLOC_CONTIG, following use MAP_ALLOCATABLE */
    alloc_fd_dsp_data = posix_typed_mem_open("/ram/dsp_data_buf", O_RDWR, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
    if (alloc_fd_dsp_data < 0) {
        Rval = DSP_ERR_0003;
        dsp_osal_printI5("[ERR] dsp_osal_alloc_data_addr() : posix_typed_mem_open fail(/ram/dsp_prot_cache_buf) fd = %d errno = %d", alloc_fd_dsp_data, errno, 0, 0, 0);
    } else {
        retcode = posix_typed_mem_get_info(alloc_fd_dsp_data, &info);
        if (retcode < 0) {
            Rval = DSP_ERR_0003;
            dsp_osal_printI5("[ERR] dsp_osal_alloc_data_addr() : posix_typed_mem_get_info fail ret = %d errno = %d", retcode, errno, 0, 0, 0);
        } else {
            flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE;
            virt_addr = mmap(NULL, info.posix_tmi_length, (int32_t)flag, MAP_SHARED, alloc_fd_dsp_data, 0);
            if (virt_addr == MAP_FAILED) {
                Rval = DSP_ERR_0003;
                dsp_osal_printI5("[ERR] dsp_osal_alloc_data_addr() : mmap fail errno = %d", errno, 0, 0, 0, 0);
            } else {
                contig_len = 0U;
                retcode = mem_offset(virt_addr, NOFD, 1, &offset, &contig_len);
                if (retcode < 0) {
                    Rval = DSP_ERR_0003;
                    dsp_osal_printI5("[ERR] dsp_osal_alloc_data_addr() : mem_offset fail ret = %d errno = %d", retcode, errno, 0, 0, 0);
                } else {
                    /* update buf_pool */
                    dsp_osal_typecast(&osal_buf_pool[DSP_BUF_POOL_DATA].Base, &virt_addr);
                    osal_buf_pool[DSP_BUF_POOL_DATA].size = (uint32_t)info.posix_tmi_length;
                    osal_buf_pool[DSP_BUF_POOL_DATA].UsedSize = 0U;
                    osal_buf_pool[DSP_BUF_POOL_DATA].is_cached = 1U;
                    Rval = dsp_osal_memcpy(&osal_buf_pool[DSP_BUF_POOL_DATA].PhysBase, &offset, sizeof(uint32_t));
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    dsp_osal_printU5("[dspdev] FDT mempool[%d] Virt 0x%X Phys 0x%X Sz %d Cached %d",
                                      DSP_BUF_POOL_DATA,
                                      (uint32_t)osal_buf_pool[DSP_BUF_POOL_DATA].Base,
                                      osal_buf_pool[DSP_BUF_POOL_DATA].PhysBase,
                                      osal_buf_pool[DSP_BUF_POOL_DATA].size,
                                      osal_buf_pool[DSP_BUF_POOL_DATA].is_cached);
                }
            }
        }
    }

    return Rval;
}
#else
/* allocate dsp data buffer */
uint32_t dsp_osal_alloc_data_addr(void)
{
    UINT32 Rval = DSP_ERR_0008;

    dsp_osal_printU5("[ERR] dsp_osal_alloc_data_addr : Shall from App", 0U, 0U, 0U, 0U, 0U);

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
#define DBG_PORT_IDSP_ADDR_EXT_BASE_ADDR    (0xED1C8000U)
#define DBG_PORT_IDSP_VIN0_SECT_ID          (1U)
#define DBG_PORT_IDSP_VIN1_SECT_ID          (10U)
#define DBG_PORT_IDSP_VIN2_SECT_ID          (1U)
#define DBG_PORT_IDSP_VIN3_SECT_ID          (1U)
#define DBG_PORT_IDSP_VIN4_SECT_ID          (1U)
#define DBG_PORT_IDSP_VIN5_SECT_ID          (1U)
#define DBG_PORT_IDSP_VIN_ENB_BASE_ADDR     (0xED1C0000U)
#elif defined (CONFIG_SOC_CV2)
#define DBG_PORT_IDSP_ADDR_EXT_BASE_ADDR    (0xED1C8000U)
#define DBG_PORT_IDSP_VIN0_SECT_ID          (1U)
#define DBG_PORT_IDSP_VIN1_SECT_ID          (10U)
#define DBG_PORT_IDSP_VIN2_SECT_ID          (11U)
#define DBG_PORT_IDSP_VIN3_SECT_ID          (12U)
#define DBG_PORT_IDSP_VIN4_SECT_ID          (13U)
#define DBG_PORT_IDSP_VIN5_SECT_ID          (14U)
#define DBG_PORT_IDSP_VIN_ENB_BASE_ADDR     (0xED1C0000U)
#elif defined (CONFIG_SOC_CV25)
#define DBG_PORT_IDSP_ADDR_EXT_BASE_ADDR    (0xED1C8000U)
#define DBG_PORT_IDSP_VIN0_SECT_ID          (1U)
#define DBG_PORT_IDSP_VIN1_SECT_ID          (10U)
#define DBG_PORT_IDSP_VIN2_SECT_ID          (11U)
#define DBG_PORT_IDSP_VIN3_SECT_ID          (1U)
#define DBG_PORT_IDSP_VIN4_SECT_ID          (1U)
#define DBG_PORT_IDSP_VIN5_SECT_ID          (1U)
#define DBG_PORT_IDSP_VIN_ENB_BASE_ADDR     (0xED1C0000U)
#elif defined (CONFIG_SOC_H32)
#define DBG_PORT_IDSP_ADDR_EXT_BASE_ADDR    (0xED1C8000U)
#define DBG_PORT_IDSP_VIN0_SECT_ID          (1U)
#define DBG_PORT_IDSP_VIN1_SECT_ID          (10U)
#define DBG_PORT_IDSP_VIN2_SECT_ID          (11U)
#define DBG_PORT_IDSP_VIN3_SECT_ID          (1U)
#define DBG_PORT_IDSP_VIN4_SECT_ID          (1U)
#define DBG_PORT_IDSP_VIN5_SECT_ID          (1U)
#define DBG_PORT_IDSP_VIN_ENB_BASE_ADDR     (0xED1C0000U)
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
#define DBG_PORT_IDSP_ADDR_EXT_BASE_ADDR    (0xED1C8000U)
#define DBG_PORT_IDSP_VIN0_SECT_ID          (1U)
#define DBG_PORT_IDSP_VIN1_SECT_ID          (10U)
#define DBG_PORT_IDSP_VIN2_SECT_ID          (11U)
#define DBG_PORT_IDSP_VIN3_SECT_ID          (12U)
#define DBG_PORT_IDSP_VIN4_SECT_ID          (13U)
#define DBG_PORT_IDSP_VIN5_SECT_ID          (19U)
#define DBG_PORT_IDSP_VIN_ENB_BASE_ADDR     (0xED1C0000U)
#endif

typedef struct {
    uint32_t sw_reset:1;
    uint32_t enable:1;
    uint32_t output_enable:1;
    uint32_t bits_per_pixel:3;
    uint32_t reserved:26;
} dsp_osal_idsp_ctrl0_t;

static inline void dsp_osal_delay_cycles(UINT32 Delay)
{
    volatile UINT32 Tmp = Delay;

    while(Tmp > 0U) {
        Tmp--;
    }
}

void dsp_osal_dbgport_get_vin_enable_status(UINT32 VinId, UINT32 *pEnable)
{
#if 0 //ThreadX
    //#include "AmbaCSL_VIN.h"
    extern UINT32 AmbaCSL_VinIsEnabled(UINT32 VinID);

    *pEnable = AmbaCSL_VinIsEnabled(VinId);
#else
    static ULONG dsp_idsp_ctrl0_virt_addr = 0;
    static ULONG dsp_idsp_base_virt_addr = 0;
    volatile UINT32 *pU32Val;
    volatile dsp_osal_idsp_ctrl0_t *pIdspCtrl0;
    uint64_t phys_addr;
    void *virt_addr;
    uint32_t backup_addr;
    uint32_t addr;
    uint32_t flag;

    if (dsp_idsp_base_virt_addr == 0U) {
        phys_addr = DBG_PORT_IDSP_ADDR_EXT_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(UINT32), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_dbgport_get_vin_enable_status : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&dsp_idsp_base_virt_addr, &virt_addr);
        }
    }

    if (dsp_idsp_ctrl0_virt_addr == 0U) {
        phys_addr = DBG_PORT_IDSP_VIN_ENB_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(dsp_osal_idsp_ctrl0_t), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_dbgport_get_vin_enable_status : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&dsp_idsp_ctrl0_virt_addr, &virt_addr);
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
#endif
}

#if defined (CONFIG_SOC_CV22)
#define DBG_PORT_RCT_CLK_CTRL_BASE_ADDR     (0xED08008CU)
#define DBG_PORT_RCT_CLK_DSP_MASK           (0x3144U)
#define DBG_PORT_RCT_CLK_MCTF_MASK          (0x10000U)
#define DBG_PORT_RCT_CLK_CODEC_MASK         (0x10U)
#define DBG_PORT_RCT_CLK_HEVC_MASK          (0xC000U)
#elif defined (CONFIG_SOC_CV2)
#define DBG_PORT_RCT_CLK_CTRL_BASE_ADDR     (0xED08008CU)
#define DBG_PORT_RCT_CLK_DSP_MASK           (0x3144U)
#define DBG_PORT_RCT_CLK_MCTF_MASK          (0x10000U)
#define DBG_PORT_RCT_CLK_CODEC_MASK         (0x10U)
#define DBG_PORT_RCT_CLK_HEVC_MASK          (0xC000U)
#elif defined (CONFIG_SOC_CV25)
#define DBG_PORT_RCT_CLK_CTRL_BASE_ADDR     (0xED08008CU)
#define DBG_PORT_RCT_CLK_DSP_MASK           (0x3144U)
#define DBG_PORT_RCT_CLK_MCTF_MASK          (0x10000U)
#define DBG_PORT_RCT_CLK_CODEC_MASK         (0x10U)
#define DBG_PORT_RCT_CLK_HEVC_MASK          (0xC000U)
#elif defined (CONFIG_SOC_H32)
#define DBG_PORT_RCT_CLK_CTRL_BASE_ADDR     (0xED08008CU)
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
#define DBG_PORT_RCT_CLK_CTRL_BASE_ADDR     (0xED08008CU)
#define DBG_PORT_RCT_CLK_DSP_MASK           (0x3144U)
#define DBG_PORT_RCT_CLK_MCTF_MASK          (0x10000U)
#define DBG_PORT_RCT_CLK_CODEC_MASK         (0x10U)
#define DBG_PORT_RCT_CLK_HEVC_MASK          (0xC000U)
#endif


UINT32 dsp_osal_dbgport_get_clk_enable_status(UINT32 ClkId, UINT32 *pEnable)
{
    UINT32 Rval = DSP_ERR_NONE;
    static ULONG dsp_rtc_clk_ctrl_virt_addr = 0;
    volatile UINT32 *pU32Val;
    uint64_t phys_addr;
    void *virt_addr;
    UINT32 FeatureMask;
    uint32_t flag;

    if (dsp_rtc_clk_ctrl_virt_addr == 0U) {
        phys_addr = DBG_PORT_RCT_CLK_CTRL_BASE_ADDR;
        flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
        virt_addr = mmap_device_memory(NULL, sizeof(UINT32), (int32_t)flag, 0, phys_addr);
        if (virt_addr == MAP_FAILED) {
            dsp_osal_printI5("[ERR] dsp_osal_dbgport_get_clk_enable_status : mmap_device_memory fail errno = %d line[%d]", errno, __LINE__, 0, 0, 0);
        } else {
            dsp_osal_typecast(&dsp_rtc_clk_ctrl_virt_addr, &virt_addr);
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
static osal_mq_t dsp_evnt_data_q;
#define DSP_EVENT_DATA_Q_DEPTH  (128U)

typedef struct {
    uint32_t MaxHdlrNum;
    uint32_t UsedHdlrNum;
} osal_evnt_cfg_t;
static osal_evnt_cfg_t dsp_evnt_cfg[LL_EVENT_NUM_ALL];

static inline UINT32 dsp_osal_evnt_data_q_init(void)
{
    static uint32_t dsp_evnt_data_q_init = 0U;
    static char dsp_evnt_data_q_name[] = "DspEvntDataQ";
    uint32_t Rval = DSP_ERR_NONE;

    if (dsp_evnt_data_q_init == 0U) {
        Rval = dsp_osal_memset(dsp_evnt_cfg, 0, ((uint32_t)sizeof(osal_evnt_cfg_t))*LL_EVENT_NUM_ALL);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        Rval = dsp_osal_mq_create(&dsp_evnt_data_q, dsp_evnt_data_q_name,
            (uint32_t)sizeof(dsp_evnt_data_t), NULL, DSP_EVENT_DATA_Q_DEPTH*((uint32_t)sizeof(dsp_evnt_data_t)));
        if (Rval != OK) {
            dsp_osal_printU5("[ERR] dsp_osal_evnt_data_q_init : DspEvntDataQ create fail [0x%X]", Rval, 0U, 0U, 0U, 0U);
        } else {
            dsp_evnt_data_q_init = 1U;
        }
    }

    return Rval;
}

UINT32 dsp_osal_evnt_hdlr_cfg(UINT32 EvntId, UINT32 MaxNum, const AMBA_DSP_EVENT_HANDLER_f *pEvntHdlrs)
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

static UINT32 dsp_osal_event_lv_raw_rdy(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_RAW_DATA_RDY_s *pEvntRaw = NULL;
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
    AMBA_DSP_RAW_DATA_RDY_EXT_s *pEvntRawExt = NULL;
#elif defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
    AMBA_DSP_RAW_DATA_RDY_EXT2_s *pEvntRawExt = NULL;
#endif

    dsp_osal_typecast(&pEvntRaw, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pEvntRaw->RawBuffer.BaseAddr, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pEvntRaw->RawBuffer.BaseAddr = phys_addr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pEvntRaw->AuxBuffer.BaseAddr, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntRaw->AuxBuffer.BaseAddr = phys_addr;
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
        if (Rval == DSP_ERR_NONE) {
            dsp_osal_typecast(&pEvntRawExt, &pEvntInfo);

            Rval = dsp_osal_virt2phys(pEvntRawExt->EmbedBuffer.BaseAddr, &phys_addr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pEvntRawExt->EmbedBuffer.BaseAddr = phys_addr;
        }
#elif defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
        if (Rval == DSP_ERR_NONE) {
            dsp_osal_typecast(&pEvntRawExt, &pEvntInfo);

            Rval = dsp_osal_virt2phys(pEvntRawExt->EmbedBuffer.BaseAddr, &phys_addr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pEvntRawExt->EmbedBuffer.BaseAddr = phys_addr;

            if (Rval == DSP_ERR_NONE) {
                Rval = dsp_osal_virt2phys(pEvntRawExt->Aux2Buffer.BaseAddr, &phys_addr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                pEvntRawExt->Aux2Buffer.BaseAddr = phys_addr;
            }
        }
#endif
    }

    return Rval;
}

static UINT32 dsp_osal_event_lv_yuv_data_rdy(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_YUV_DATA_RDY_EXTEND_s *pEvntYuv = NULL;

    dsp_osal_typecast(&pEvntYuv, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pEvntYuv->Buffer.BaseAddrY, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pEvntYuv->Buffer.BaseAddrY = phys_addr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pEvntYuv->Buffer.BaseAddrUV, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntYuv->Buffer.BaseAddrUV = phys_addr;
    }

    return Rval;
}

static UINT32 dsp_osal_event_lv_pyramid_rdy(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_PYMD_DATA_RDY_s *pEvntPymd = NULL;
    UINT16 i;

    dsp_osal_typecast(&pEvntPymd, &pEvntInfo);

    /* V2P */
    for (i = 0U; i<AMBA_DSP_MAX_HIER_NUM; i++) {
        Rval = dsp_osal_virt2phys(pEvntPymd->YuvBuf[i].BaseAddrY, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntPymd->YuvBuf[i].BaseAddrY = phys_addr;

        if (Rval == DSP_ERR_NONE) {
            Rval = dsp_osal_virt2phys(pEvntPymd->YuvBuf[i].BaseAddrUV, &phys_addr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pEvntPymd->YuvBuf[i].BaseAddrUV = phys_addr;
        } else {
            break;
        }
    }

    return Rval;
}

static UINT32 dsp_osal_event_vout_data_rdy(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_VOUT_DATA_INFO_s *pEvntVout = NULL;

    dsp_osal_typecast(&pEvntVout, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pEvntVout->YuvBuf.BaseAddrY, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pEvntVout->YuvBuf.BaseAddrY = phys_addr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pEvntVout->YuvBuf.BaseAddrUV, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntVout->YuvBuf.BaseAddrUV = phys_addr;
    }

    return Rval;
}

static UINT32 dsp_osal_event_video_data_rdy(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval = DSP_ERR_NONE;
    AMBA_DSP_ENC_PIC_RDY_s *pEvntPic = NULL;

    dsp_osal_typecast(&pEvntPic, &pEvntInfo);

    /* V2P */
    if (pEvntPic->PicSize != AMBA_DSP_ENC_END_MARK) {
        Rval = dsp_osal_virt2phys(pEvntPic->StartAddr, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntPic->StartAddr = phys_addr;
    }

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pEvntPic->InputYAddr, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntPic->InputYAddr = phys_addr;
    }

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pEvntPic->InputUVAddr, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntPic->InputUVAddr = phys_addr;
    }

    return Rval;
}

static UINT32 dsp_osal_event_stl_raw_rdy(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_STL_RAW_DATA_INFO_s *pEvntStlRaw = NULL;

    dsp_osal_typecast(&pEvntStlRaw, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pEvntStlRaw->Buf.BaseAddr, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pEvntStlRaw->Buf.BaseAddr = phys_addr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pEvntStlRaw->AuxBuf.BaseAddr, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntStlRaw->AuxBuf.BaseAddr = phys_addr;
    }

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pEvntStlRaw->Aux2Buf.BaseAddr, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntStlRaw->Aux2Buf.BaseAddr = phys_addr;
    }

    return Rval;
}

static UINT32 dsp_osal_event_stl_yuv_data_rdy(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_YUV_IMG_BUF_s *pEvntStlYuv = NULL;

    dsp_osal_typecast(&pEvntStlYuv, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pEvntStlYuv->BaseAddrY, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pEvntStlYuv->BaseAddrY = phys_addr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pEvntStlYuv->BaseAddrUV, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntStlYuv->BaseAddrUV = phys_addr;
    }

    return Rval;
}

static UINT32 dsp_osal_event_jpeg_data_rdy(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_ENC_PIC_RDY_s *pEvntJpg = NULL;

    dsp_osal_typecast(&pEvntJpg, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pEvntJpg->StartAddr, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pEvntJpg->StartAddr = phys_addr;

    return Rval;
}

static UINT32 dsp_osal_event_video_dec_status(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s *pEvntVdoDec = NULL;

    dsp_osal_typecast(&pEvntVdoDec, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pEvntVdoDec->BitsNextReadAddr, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pEvntVdoDec->BitsNextReadAddr = phys_addr;

    return Rval;
}

static UINT32 dsp_osal_event_video_dec_yuv_disp_report(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_VIDEO_DEC_YUV_DISP_REPORT_s *pEvntVdoDecDisp = NULL;

    dsp_osal_typecast(&pEvntVdoDecDisp, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pEvntVdoDecDisp->YAddr, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pEvntVdoDecDisp->YAddr = phys_addr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pEvntVdoDecDisp->UVAddr, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntVdoDecDisp->UVAddr = phys_addr;
    }

    return Rval;
}

static UINT32 dsp_osal_event_video_dec_pic_info(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_EVENT_VIDDEC_PIC_EX_s *pEvntVdoDecPic = NULL;

    dsp_osal_typecast(&pEvntVdoDecPic, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pEvntVdoDecPic->YAddr, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pEvntVdoDecPic->YAddr = phys_addr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pEvntVdoDecPic->UVAddr, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntVdoDecPic->UVAddr = phys_addr;
    }

    return Rval;
}

static UINT32 dsp_osal_event_still_dec_status(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_STLDEC_STATUS_s *pEvntStlDec = NULL;

    dsp_osal_typecast(&pEvntStlDec, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pEvntStlDec->YAddr, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pEvntStlDec->YAddr = phys_addr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pEvntStlDec->UVAddr, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntStlDec->UVAddr = phys_addr;
    }

    return Rval;
}

static UINT32 dsp_osal_event_still_dec_yuv_disp_report(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_STILL_DEC_YUV_DISP_REPORT_s *pStlDecDisp = NULL;

    dsp_osal_typecast(&pStlDecDisp, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pStlDecDisp->YAddr, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pStlDecDisp->YAddr = phys_addr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pStlDecDisp->UVAddr, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pStlDecDisp->UVAddr = phys_addr;
    }

    return Rval;
}

static UINT32 dsp_osal_event_video_mv_data_rdy(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_ENC_MV_RDY_s *pEvntEncMv = NULL;

    dsp_osal_typecast(&pEvntEncMv, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pEvntEncMv->MvBufAddr, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pEvntEncMv->MvBufAddr = phys_addr;

    return Rval;
}

static UINT32 dsp_osal_event_lv_sideband_rdy(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_SIDEBAND_DATA_RDY_s *pEvntSideBand = NULL;
    UINT16 i;

    dsp_osal_typecast(&pEvntSideBand, &pEvntInfo);

    /* V2P */
    for (i = 0U; i<AMBA_DSP_MAX_TOKEN_ARRAY; i++) {
        Rval = dsp_osal_virt2phys(pEvntSideBand->Info[i].PrivateInfoAddr, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntSideBand->Info[i].PrivateInfoAddr = phys_addr;
        if (Rval != DSP_ERR_NONE) {
            break;
        }
    }

    return Rval;
}

static UINT32 dsp_osal_event_lv_lndt_rdy(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_LNDT_DATA_RDY_s *pEvntLndt = NULL;

    dsp_osal_typecast(&pEvntLndt, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pEvntLndt->YuvBuf.BaseAddrY, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pEvntLndt->YuvBuf.BaseAddrY = phys_addr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pEvntLndt->YuvBuf.BaseAddrUV, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntLndt->YuvBuf.BaseAddrUV = phys_addr;
    }

    return Rval;
}

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
static UINT32 dsp_osal_event_lv_main_y12_rdy(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_YUV_DATA_RDY_s *pEvntMainY12 = NULL;

    dsp_osal_typecast(&pEvntMainY12, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pEvntMainY12->Buffer.BaseAddrY, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pEvntMainY12->Buffer.BaseAddrY = phys_addr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pEvntMainY12->Buffer.BaseAddrUV, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntMainY12->Buffer.BaseAddrUV = phys_addr;
    }

    return Rval;
}
#endif

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
static UINT32 dsp_osal_event_lv_main_ir_rdy(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_YUV_DATA_RDY_s *pEvntMainIr = NULL;

    dsp_osal_typecast(&pEvntMainIr, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pEvntMainIr->Buffer.BaseAddrY, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pEvntMainIr->Buffer.BaseAddrY = phys_addr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pEvntMainIr->Buffer.BaseAddrUV, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntMainIr->Buffer.BaseAddrUV = phys_addr;
    }

    return Rval;
}
#endif

#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS) && !defined (CONFIG_SOC_CV5) && !defined (CONFIG_SOC_CV52) && !defined (CONFIG_SOC_CV7)
static UINT32 dsp_osal_event_lv_int_main_rdy(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_YUV_DATA_RDY_s *pEvntIntMain = NULL;

    dsp_osal_typecast(&pEvntIntMain, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pEvntIntMain->Buffer.BaseAddrY, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pEvntIntMain->Buffer.BaseAddrY = phys_addr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pEvntIntMain->Buffer.BaseAddrUV, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntIntMain->Buffer.BaseAddrUV = phys_addr;
    }

    return Rval;
}
#endif

static UINT32 dsp_osal_event_stl_raw_postproc_rdy(const void *pEvntInfo, ULONG phys_addr)
{
    UINT32 Rval;
    AMBA_DSP_RAW_DATA_RDY_s *pEvntRaw = NULL;

    dsp_osal_typecast(&pEvntRaw, &pEvntInfo);

    /* V2P */
    Rval = dsp_osal_virt2phys(pEvntRaw->RawBuffer.BaseAddr, &phys_addr);
    dsp_osal_print_err_line(Rval, __func__, __LINE__);
    pEvntRaw->RawBuffer.BaseAddr = phys_addr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_virt2phys(pEvntRaw->AuxBuffer.BaseAddr, &phys_addr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pEvntRaw->AuxBuffer.BaseAddr = phys_addr;
    }

    return Rval;
}

static UINT32 dsp_osal_event_do_nothing(const void *pEvntInfo, ULONG phys_addr)
{
(void)pEvntInfo;
(void)phys_addr;
    return DSP_ERR_NONE;
}

typedef UINT32 (*dsp_osal_event_user_cb_array)(const void *pEvntInfo, ULONG phys_addr);

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

    static dsp_osal_event_user_cb_array dsp_osal_event_cb[AMBA_DSP_EVENT_NUM] = {
            dsp_osal_event_lv_raw_rdy,                  /* AMBA_DSP_EVENT_LV_RAW_RDY */
            dsp_osal_event_do_nothing,                  /* AMBA_DSP_EVENT_LV_CFA_AAA_RDY */
            dsp_osal_event_do_nothing,                  /* AMBA_DSP_EVENT_LV_PG_AAA_RDY */
            dsp_osal_event_do_nothing,                  /* AMBA_DSP_EVENT_LV_HIST_AAA_RDY */
            dsp_osal_event_lv_yuv_data_rdy,             /* AMBA_DSP_EVENT_LV_YUV_DATA_RDY */
            dsp_osal_event_lv_pyramid_rdy,              /* AMBA_DSP_EVENT_LV_PYRAMID_RDY */
            dsp_osal_event_vout_data_rdy,               /* AMBA_DSP_EVENT_VOUT_DATA_RDY */
            dsp_osal_event_video_data_rdy,              /* AMBA_DSP_EVENT_VIDEO_DATA_RDY */
            dsp_osal_event_do_nothing,                  /* AMBA_DSP_EVENT_VIDEO_ENC_START */
            dsp_osal_event_do_nothing,                  /* AMBA_DSP_EVENT_VIDEO_ENC_STOP */
            dsp_osal_event_stl_raw_rdy,                 /* AMBA_DSP_EVENT_STL_RAW_RDY */
            dsp_osal_event_do_nothing,                  /* AMBA_DSP_EVENT_STL_CFA_AAA_RDY */
            dsp_osal_event_do_nothing,                  /* AMBA_DSP_EVENT_STL_PG_AAA_RDY */
            dsp_osal_event_stl_raw_postproc_rdy,        /* AMBA_DSP_EVENT_STL_RAW_POSTPROC_RDY */
            dsp_osal_event_stl_yuv_data_rdy,            /* AMBA_DSP_EVENT_STL_YUV_DATA_RDY */
            dsp_osal_event_jpeg_data_rdy,               /* AMBA_DSP_EVENT_JPEG_DATA_RDY */
            dsp_osal_event_do_nothing,                  /* AMBA_DSP_EVENT_VIDEO_PATH_STATUS */
            dsp_osal_event_video_dec_status,            /* AMBA_DSP_EVENT_VIDEO_DEC_STATUS */
            dsp_osal_event_video_dec_yuv_disp_report,   /* AMBA_DSP_EVENT_VIDEO_DEC_YUV_DISP_REPORT */
            dsp_osal_event_video_dec_pic_info,          /* AMBA_DSP_EVENT_VIDEO_DEC_PIC_INFO */
            dsp_osal_event_still_dec_status,            /* AMBA_DSP_EVENT_STILL_DEC_STATUS */
            dsp_osal_event_still_dec_yuv_disp_report,   /* AMBA_DSP_EVENT_STILL_DEC_YUV_DISP_REPORT */
            dsp_osal_event_do_nothing,                  /* AMBA_DSP_EVENT_VIN_POST_CONFIG */
            dsp_osal_event_do_nothing,                  /* AMBA_DSP_EVENT_ERROR */
            dsp_osal_event_do_nothing,                  /* AMBA_DSP_EVENT_LV_SYNC_JOB_RDY */
            dsp_osal_event_video_mv_data_rdy,           /* AMBA_DSP_EVENT_VIDEO_MV_DATA_RDY */
            dsp_osal_event_lv_sideband_rdy,             /* AMBA_DSP_EVENT_LV_SIDEBAND_RDY */
            dsp_osal_event_lv_lndt_rdy,                 /* AMBA_DSP_EVENT_LV_LNDT_RDY */
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
            dsp_osal_event_lv_main_y12_rdy,             /* AMBA_DSP_EVENT_LV_MAIN_Y12_RDY */
            dsp_osal_event_do_nothing,                  /* AMBA_DSP_EVENT_TESTFRAME_RDY */
#elif defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
            dsp_osal_event_lv_main_y12_rdy,             /* AMBA_DSP_EVENT_LV_MAIN_Y12_RDY */
            dsp_osal_event_lv_main_ir_rdy,              /* AMBA_DSP_EVENT_LV_MAIN_IR_RDY */
#else
            dsp_osal_event_lv_int_main_rdy,             /* AMBA_DSP_EVENT_LV_INT_MAIN_RDY */
#endif
            dsp_osal_event_do_nothing                   /* AMBA_DSP_EVENT_CLK_UPDATE_READY */
    };

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
            } else {
                return DSP_ERR_0000;
            }

            if (EvntId < AMBA_DSP_EVENT_NUM) {
                Rval = dsp_osal_event_cb[EvntId](pEvntInfo, phys_addr);
            } else {
                Rval = DSP_ERR_0003;
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
#if 0
uint32_t dsp_osal_str_to_u32(const char *pString, uint32_t *pVal)
{
#ifdef USE_AMBA_UTILITY
    return OSAL_StringToUInt32(pString, pVal);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t int32_val;

    if ((pString == NULL) || (pVal == NULL)) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_str_to_u32 : fail Rval = 0x%X", Rval, 0U, 0U, 0U, 0U);
    } else {
        int32_val = strtol(pString, NULL, 0);
        memcpy(pVal, &int32_val, sizeof(uint32_t));
    }
    return Rval;
#endif
}
#endif

uint32_t dsp_osal_u32_to_str(char *pString, uint32_t str_len, uint32_t val, uint32_t radix)
{
#ifdef USE_AMBA_UTILITY
        return OSAL_UInt32ToStr(pString, str_len, val, radix);
#else
    uint32_t Rval = DSP_ERR_NONE;
    int32_t int32_val;

(void)str_len;
    if (pString == NULL) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_u32_to_str : fail retcode = 0x%X", retcode, 0U, 0U, 0U, 0U);
    } else {
        memcpy(&int32_val, &val, sizeof(uint32_t));
        (void)itoa(int32_val, pString, radix);
    }
    return Rval;
#endif
}

uint32_t dsp_osal_str_print_u32(char *pBuffer, uint32_t BufferSize, const char *pFmtString, uint32_t Count, const uint32_t *pArgs)
{
#ifdef USE_AMBA_UTILITY
    return OSAL_StringPrintUInt32(pBuffer, BufferSize, pFmtString, Count, pArgs);
#else
    uint32_t Rval = 0;

(void)BufferSize;
    if ((pBuffer == NULL) || (Count == 0)) {
        Rval = 0;
        dsp_osal_printU5("[ERR] dsp_osal_str_print_u32 : fail", 0U, 0U, 0U, 0U, 0U);
    } else if (Count > 5) {
        Rval = 0;
        dsp_osal_printU5("[ERR] dsp_osal_str_print_u32 : count shall less than 5", 0U, 0U, 0U, 0U, 0U);
    } else {
        if (Count == 1U) {
            (void)sprintf(pBuffer, pFmtString, pArgs[0U]);
        } else if (Count == 2U) {
            (void)sprintf(pBuffer, pFmtString, pArgs[0U], pArgs[1U]);
        } else if (Count == 3) {
            (void)sprintf(pBuffer, pFmtString, pArgs[0U], pArgs[1U], pArgs[2U]);
        } else if (Count == 4U) {
            (void)sprintf(pBuffer, pFmtString, pArgs[0U], pArgs[1U], pArgs[2U], pArgs[3U]);
        } else {
            (void)sprintf(pBuffer, pFmtString, pArgs[0U], pArgs[1U], pArgs[2U], pArgs[3U], pArgs[4U]);
        }
        Rval = strlen(pBuffer);
    }
    return Rval;
#endif
}

uint32_t dsp_osal_str_append(char *pBuffer, uint32_t BufferSize, const char *pSource)
{
#ifdef USE_AMBA_UTILITY
    uint32_t Rval = DSP_ERR_NONE;

    OSAL_StringAppend(pBuffer, BufferSize, pSource);

    return Rval;
#else
    uint32_t Rval = DSP_ERR_NONE;

(void)BufferSize;
    if ((pBuffer == NULL) || (pSource == NULL)) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_str_append : fail Rval = 0x%X", Rval, 0U, 0U, 0U, 0U);
    } else {
        (void)strcat(pBuffer, pSource);
    }
    return Rval;
#endif
}

uint32_t dsp_osal_str_append_u32(char *pBuffer, uint32_t BufferSize, uint32_t Value, uint32_t Radix)
{
#ifdef USE_AMBA_UTILITY
    uint32_t Rval = DSP_ERR_NONE;

    OSAL_StringAppendUInt32(pBuffer, BufferSize, Value, Radix);

    return Rval;
#else
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t str_len = 0U;
#define WORK_BUF_STRING_LEN (128)
    char work_buf[WORK_BUF_STRING_LEN];

(void)BufferSize;
    if (pBuffer == NULL) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_str_append_u32 : fail Rval = 0x%X", Rval, 0U, 0U, 0U, 0U);
    } else {
        str_len = dsp_osal_u32_to_str(work_buf, WORK_BUF_STRING_LEN, Value, Radix);
        if (str_len > 0U) {
            (void)strcat(pBuffer, work_buf);
        }
    }
    return Rval;
#endif
}

uint32_t dsp_osal_str_copy(char *pBuffer, uint32_t BufferSize, const char *pSource)
{
#ifdef USE_AMBA_UTILITY
    uint32_t Rval = DSP_ERR_NONE;

    OSAL_StringCopy(pBuffer, BufferSize, pSource);

    return Rval;
#else
    uint32_t Rval = DSP_ERR_NONE;

(void)BufferSize;
    if ((pBuffer == NULL) || (pSource == NULL)) {
        Rval = DSP_ERR_0003;
        dsp_osal_printU5("[ERR] dsp_osal_str_append : fail Rval = 0x%X", Rval, 0U, 0U, 0U, 0U);
    } else {
        (void)strcpy(pBuffer, pSource);
    }
    return Rval;
#endif
}

/******************************************************************************/
/*                                  Resource                                 */
/******************************************************************************/
uint32_t dsp_osal_kernel_init(void)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode;

    retcode = cache_init(0, &dsp_cache_info, NULL);
    if (retcode < 0) {
        Rval = DSP_ERR_0003;
        // Invoke by Ambadsp_dev main
        // Using internal print to avoid AmbaPrint init has not been completed
        (void)printf("[ERR] dsp_osal_kernel_init : cache_init fail retcode = %d errno %d\n", retcode, errno);
    }

    HL_CreateCtxInitMtx();

    return Rval;
}

#if 0
void dsp_osal_kernel_exit(void)
{

}
#endif

