/**
 *  @file dsp_wrapper_osal.h
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
 *  @details Definitions & Constants for DSP Wrapper OS abstract layer
 *
 */

#ifndef DSP_WRAPPER_OSAL_H
#define DSP_WRAPPER_OSAL_H

#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <atomic.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <sys/cache.h>
#include <sys/procmgr.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <devctl.h>
#include "AmbaTypes.h"
#include "AmbaDSP.h"
#include "AmbaDSP_Event.h"

#define DSP_WRAP_EVENT_GET_DATA_NO_THREAD

/******************************************************************************/
/*                                  print                                     */
/******************************************************************************/
//void dsp_wrapper_osal_print_flush(void);
//void dsp_wrapper_osal_print_stop_and_flush(void);
void dsp_wrapper_osal_printS5(const char *fmt, const char *argv0,
                              const char *argv1, const char *argv2,
                              const char *argv3, const char *argv4);
void dsp_wrapper_osal_printU5(const char *fmt, uint32_t argv0,
                              uint32_t argv1, uint32_t argv2,
                              uint32_t argv3, uint32_t argv4);
void dsp_wrapper_osal_printI5(const char *fmt, int32_t argv0,
                              int32_t argv1, int32_t argv2,
                              int32_t argv3, int32_t argv4);


//void dsp_wrapper_osal_module_printS5(uint32_t module_id, const char *fmt,
//                                     const char *argv0, const char *argv1,
//                                     const char *argv2, const char *argv3,
//                                     const char *argv4);

//void dsp_wrapper_osal_module_printU5(uint32_t module_id, const char *fmt,
//                                     uint32_t argv0, uint32_t argv1,
//                                     uint32_t argv2, uint32_t argv3,
//                                     uint32_t argv4);

void dsp_wrapper_osal_print_err(uint32_t rval, const char *fn, uint32_t line_num);

/******************************************************************************/
/*                                  OS relative                               */
/******************************************************************************/
typedef void *(*wrap_osal_thread_entry_f) (void *arg);

typedef pthread_mutex_t         wrap_osal_mutex_t;
typedef pthread_t               wrap_osal_thread_t;
typedef struct {
    volatile uint32_t           flag;
    pthread_mutex_t             mutex;
    pthread_cond_t              cond;
} wrap_osal_even_t;

uint32_t dsp_wrapper_osal_mutex_init(wrap_osal_mutex_t *mutex, char *pName);
//uint32_t dsp_wrapper_osal_mutex_deinit(wrap_osal_mutex_t *mutex);
uint32_t dsp_wrapper_osal_mutex_lock(wrap_osal_mutex_t *mutex, uint32_t timeout);
uint32_t dsp_wrapper_osal_mutex_unlock(wrap_osal_mutex_t *mutex);
#ifndef DSP_WRAP_EVENT_GET_DATA_NO_THREAD
uint32_t dsp_wrapper_osal_eventflag_init(wrap_osal_even_t *event, char *pName);
uint32_t dsp_wrapper_osal_eventflag_set(wrap_osal_even_t *event, uint32_t flag);
uint32_t dsp_wrapper_osal_eventflag_get(wrap_osal_even_t *event, uint32_t reqflag, uint32_t all, uint32_t clear, uint32_t *actflag, uint32_t timeout);
uint32_t dsp_wrapper_osal_eventflag_clear(wrap_osal_even_t *event, uint32_t flag);
uint32_t dsp_wrapper_osal_thread_create(wrap_osal_thread_t *thread, char *name, uint32_t priority,
                                        wrap_osal_thread_entry_f entry, const void *arg,
                                        void *stack_buff, uint32_t stack_size, uint32_t auto_start);
#endif

//uint32_t dsp_wrapper_osal_thread_delete(wrap_osal_thread_t *thread);
//uint32_t dsp_wrapper_osal_thread_should_stop(const wrap_osal_thread_t *thread);
#ifndef DSP_WRAP_EVENT_GET_DATA_NO_THREAD
uint32_t dsp_wrapper_osal_thread_set_affinity(const wrap_osal_thread_t *thread, uint32_t affinity);
uint32_t dsp_wrapper_osal_thread_resume(wrap_osal_thread_t *thread);
uint32_t dsp_wrapper_osal_sleep(uint32_t msec);
#endif

/******************************************************************************/
/*                                  Memory                                    */
/******************************************************************************/
uint32_t dsp_wrapper_osal_virt2phys(const ULONG Virt, ULONG *pPhys);
uint32_t dsp_wrapper_osal_phys2virt(const ULONG Phys, ULONG *pVirt);
#define DSP_WRAP_POOL_WORK      (0U)
#define DSP_WRAP_POOL_DATA      (1U)
#define DSP_WRAP_POOL_STAT      (2U)
#define DSP_WRAP_POOL_BIN_ORC   (3U)
#define DSP_WRAP_POOL_BIN_ME    (4U)
#define DSP_WRAP_POOL_BIN_MDXF  (5U)
#define DSP_WRAP_POOL_BIN_DEFT  (6U)
#define DSP_WRAP_POOL_EVT       (7U)
#define NUM_DSP_WRAP_POOL       (8U)

uint32_t dsp_wrapper_osal_add_buf_pool(UINT32 Type, ULONG VirtAddr, ULONG PhysAddr, UINT32 Size, UINT32 IsCached);
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
uint32_t dsp_wrapper_osal_get_buf_pool_Info(UINT32 Type, ULONG *pVirtAddr, ULONG *pPhysAddr, UINT32 *pSize, UINT32 *pIsCached);
#endif

/******************************************************************************/
/*                                  Utilities                                 */
/******************************************************************************/
static inline void dsp_wrapper_osal_typecast(void *pNewType, const void *pOldType)
{
    if (pNewType == NULL) {
        dsp_wrapper_osal_printU5("[ERR] dsp_wrapper_osal_typecast : pNewType == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if (pOldType == NULL) {
        dsp_wrapper_osal_printU5("[ERR] dsp_wrapper_osal_typecast : pOldType == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        size_t num = sizeof(void *);
        (void)memcpy(pNewType, pOldType, num);
    }
}

static inline uint32_t dsp_wrapper_osal_memcpy(void *pDst, const void *pSrc, size_t num)
{
    (void)memcpy(pDst, pSrc, num);
    return 0U;
}

static inline uint32_t dsp_wrapper_osal_memset(void *ptr, int32_t v, size_t n)
{
    (void)memset(ptr, v, n);
    return 0U;
}

//uint32_t dsp_wrapper_osal_str_to_u32(const char *pString, uint32_t *pVal);
//uint32_t dsp_wrapper_osal_u32_to_str(char *pString, uint32_t str_len, uint32_t val, uint32_t radix);
//uint32_t dsp_wrapper_osal_str_print_u32(char *pBuffer, uint32_t BufferSize, const char *pFmtString, uint32_t Count, const uint32_t *pArgs);
//uint32_t dsp_wrapper_osal_str_append(char *pBuffer, uint32_t BufferSize, const char *pSource);
//uint32_t dsp_wrapper_osal_str_append_u32(char *pBuffer, uint32_t BufferSize, uint32_t Value, uint32_t Radix);
//uint32_t dsp_wrapper_osal_str_copy(char *pBuffer, uint32_t BufferSize, const char *pSource);

/******************************************************************************/
/*                                  Event                                 */
/******************************************************************************/
typedef struct {
    UINT32                       MaxNumHandlers;     /* maximum number of Handlers */
    AMBA_DSP_EVENT_HANDLER_f    *pEventHandlers;    /* pointer to the Event Handlers */
} DSP_WRAP_EVENT_HANDLER_s;

typedef struct {
    wrap_osal_mutex_t           Mtx;
    DSP_WRAP_EVENT_HANDLER_s    Main;
} DSP_WRAP_EVENT_HANDLER_CTRL_s;

uint32_t DspWrapEventHandlerConfig(uint16_t EventID, uint32_t MaxNumHandlers, AMBA_DSP_EVENT_HANDLER_f *pEventHandlers);
uint32_t DspWrapRegisterEventHandler(uint16_t EventID, AMBA_DSP_EVENT_HANDLER_f EventHandler);
uint32_t DspWrapUnRegisterEventHandler(uint16_t EventID, AMBA_DSP_EVENT_HANDLER_f EventHandler);
uint32_t DspWrapEventHandlerReset(uint16_t EventID);
uint32_t DspWrapGiveEvent(uint16_t EventID, const void *pEventInfo);

#endif  //DSP_WRAPPER_OSAL_H
