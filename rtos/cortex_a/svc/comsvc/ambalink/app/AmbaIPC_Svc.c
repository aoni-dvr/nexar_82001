/**
 * @file AmbaIPC_Svc.c
 *
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @details The RPC server module of Ambarella Network Support
 *
 */
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaLink.h"
#include "AmbaLinkInternal.h"
#include "AmbaLinkPrivate.h"
#include "AmbaCSL_TMR.h"
#include "libwrapc.h"
#include "AmbaIPC_List.h"
#include "AmbaIPC_Rpc.h"
#include "AmbaShell.h"

#define SVC_THREAD_NUM          2
#define FLAG_DATA_READY         1
#define FLAG_DATA_CONSUMED      2
#define FLAG_THREAD_CANCEL      4
#define FLAG_THREAD_FINISH      8

/* Flags for threadpool */
#define TFLAG_QUEUE_EMPTY       1
#define TFLAG_QUEUE_FULL        2

typedef struct _AMBA_IPC_SVC_HANDLE_s_ {
    int               prog;
    int               vers;
    AMBA_IPC_PORT_s   port;
    AMBA_IPC_LIST_s   list;
    char *            name;

    AMBA_KAL_TASK_t   thread;

    AMBA_IPC_XPRT_s   xprt_pool[AMBA_IPC_CLIENT_NR_MAX];
    AMBA_IPC_PKT_s    *recv_pkt;
    int               recv_len;
    int               new_thread;
    AMBA_IPC_PROG_INFO_s    prog_info;
    AMBA_IPC_PKT_s          pkt;  //must be the last  one
} AMBA_IPC_SVC_HANDLE_s;

typedef struct _AMBA_THREADPOOL_TASK_s_ {
    void     *(*function)(void *);
    UINT32   arg;
} AMBA_THREADPOOL_TASK_s;

typedef struct _AMBA_THREADPOOL_s_ {
    AMBA_KAL_MUTEX_t            lock;
    AMBA_KAL_TASK_t             *threads;
    AMBA_THREADPOOL_TASK_s      *queue;
    AMBA_KAL_EVENT_FLAG_t       flag;
    int                         thread_count;
    int                         queue_size;
    volatile int                queue_count;
    volatile int                head;
    volatile int                tail;
    char                        thread_name[MAX_TASK_NAME];
    UINT8                       *stack;
} AMBA_THREADPOOL_s;

static LIST_HEAD(head);
static AMBA_THREADPOOL_s *pool;
extern  unsigned int crc_result[]                __attribute__((weak));
extern  unsigned int priv_crc_result[]           __attribute__((weak));
extern  int rpc_prog_id[]                        __attribute__((weak));
extern  int priv_rpc_prog_id[]                   __attribute__((weak));
extern  int rpc_num                              __attribute__((weak));
extern  int priv_rpc_num                         __attribute__((weak));
extern  AMBA_LINK_CTRL_s AmbaLinkCtrl            __attribute__((weak));
#ifdef RPC_DEBUG
extern void AmbaIPC_RecordStats(AMBA_IPC_XPRT_s *xprt, AMBA_IPC_COMMUICATION_MODE_e mode);
#endif

/*
* find a service and return its handle
*/
static AMBA_IPC_SVC_HANDLE_s *find_svc(int prog, int vers)
{
    AMBA_IPC_SVC_HANDLE_s *handle;

    list_for_each_entry(handle, &head, AMBA_IPC_SVC_HANDLE_s, list)
    if (handle->prog == prog && handle->vers == vers)
        return handle;

    return NULL;
}

/*
* find CRC value for the service
*/
static unsigned int find_crc(int prog)
{
    int i = 0;

    if ((&rpc_num != NULL) && (rpc_num) &&
            (rpc_prog_id != NULL) && (crc_result != NULL)) {
        for (i = 0; i < rpc_num; i++) {
            if (rpc_prog_id[i] == prog) {   //look for the index of crc_result
                return crc_result[i];
            }
        }
    }

    if ((&priv_rpc_num != NULL) && (priv_rpc_num) &&
            (priv_rpc_prog_id != NULL) && (priv_crc_result != NULL)) {
        for (i = 0; i < priv_rpc_num; i++) {
            if (priv_rpc_prog_id[i] == prog) {   //look for the index of crc_result
                return priv_crc_result[i];
            }
        }
    }

    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "ERROR!! Program id is not found!!\n", NULL, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s Please use the predefined program id in the header file\n", __func__, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    return 0;
}

static int AmbaIPC_SvcSendReply(AMBA_IPC_MSG_s *msg,
                         AMBA_IPC_XPRT_s *xprt,
                         AMBA_IPC_SVC_RESULT_s *result)
{
    AMBA_IPC_SVC_HANDLE_s *svc = (AMBA_IPC_SVC_HANDLE_s*)(ULONG)xprt->private;
#ifdef RPC_DEBUG
    unsigned int cur_time = AmbaCSL_TmrGetCurrentVal(PROFILE_TIMER);
#endif
    LOCK_PORT(svc->port);
    if (msg) {
        msg->u.reply.status = result->Status;
        msg->type = AMBA_IPC_MSG_REPLY;
    }
    if (result->Mode == AMBA_IPC_SYNCHRONOUS) {
        AMBA_IPC_PKT_s *pkt = &svc->pkt;
        UINT64 *parameters = (UINT64 *)((char *)&pkt->msg + sizeof(AMBA_IPC_MSG_s));
        pkt->xprt = *xprt;
#ifdef RPC_DEBUG
        pkt->xprt.tx_rpc_send_start = cur_time;
#endif
        memcpy(parameters, result->pResult, result->Length);
        xprt_send_pkt(&svc->port, pkt, result->Length);
    }
    xprt->private = 0;
    UNLOCK_PORT(svc->port);

    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: svc_thread_func
 *
 *  @Description:: thread that handles incoming SVC requests from clients
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void *svc_thread_func(void *arg)
{
    AMBA_IPC_SVC_HANDLE_s *svc = (AMBA_IPC_SVC_HANDLE_s*)arg;
    UINT32 actFlags;
    AMBA_IPC_MSG_s *msg;
    AMBA_IPC_SVC_RESULT_s ret[1];
    int errno = 0, status;

    memset(ret, 0, sizeof(ret));
    ret->pResult = AmbaLink_Malloc(MEM_POOL_RPC_BUF, AmbaLinkCtrl.RpcPktSize);

    do {
        status = AmbaKAL_EventFlagGet(
                     &svc->port.flag,
                     FLAG_DATA_READY | FLAG_THREAD_CANCEL,
                     0U, 1U,
                     &actFlags,
                     AMBA_KAL_WAIT_FOREVER);
        if ( status != TX_SUCCESS ) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "SVC_THREAD_FUN Event Flag Take Error: status is 0x%x", status, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            break;
        }
        if (actFlags & FLAG_THREAD_CANCEL) {
            errno = -1;
            break;
        }
        LOCK_PORT(svc->port);
        memcpy(&svc->pkt, svc->recv_pkt, svc->recv_len + AMBA_IPC_HDRLEN);
        status = AmbaKAL_EventFlagSet(&svc->port.flag, FLAG_DATA_CONSUMED);
        if ( status != TX_SUCCESS ) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "SVC_THREAD_FUN Event Flag Give Error: status is 0x%x", status, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
        if (svc->prog_info.ProcNum > 0) {
            int i;
            AMBA_IPC_XPRT_s *xprt = NULL;
            for (i = 0; i < AMBA_IPC_CLIENT_NR_MAX; i++) {
                if (!svc->xprt_pool[i].private) {
                    xprt = &svc->xprt_pool[i];
                    *xprt = svc->pkt.xprt;
                    xprt->private = (UINT64)(ULONG) svc;
                    break;
                }
            }
            if (xprt) {
#ifdef RPC_DEBUG
                svc->pkt.xprt.tx_rpc_recv_end = AmbaCSL_TmrGetCurrentVal(PROFILE_TIMER);
                AmbaIPC_RecordStats(&svc->pkt.xprt, svc->prog_info.pProcInfo[svc->pkt.msg.u.call.proc - 1].Mode);
#endif
                msg = &svc->pkt.msg;
                if ( msg->u.call.proc <= svc->prog_info.ProcNum && msg->u.call.proc > 0 ) {
                    UINT64 *parameters = (UINT64 *)((char *)msg + sizeof(AMBA_IPC_MSG_s));
                    memset(ret->pResult, 0, AmbaLinkCtrl.RpcPktSize);
                    /* call the corresponding procedure */
                    svc->prog_info.pProcInfo[msg->u.call.proc - 1].Proc(parameters, ret);
                    AmbaIPC_SvcSendReply(msg, xprt, ret);
                } else { /* can't find the procedure*/
                    ret->Status = AMBA_IPC_REPLY_PARA_INVALID;
                    ret->Mode = AMBA_IPC_SYNCHRONOUS;
                    AmbaIPC_SvcSendReply(msg, xprt, ret);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "ERROR!! reached AIPC_CLIENT_NR_MAX!!\n", NULL, NULL, NULL, NULL, NULL);
                AmbaPrint_Flush();
            }
        }
        UNLOCK_PORT(svc->port);
    } while (svc->new_thread == 1);

    AmbaLink_Free(MEM_POOL_RPC_BUF, ret->pResult);

    if (errno == -1) { // svc is unregistered, set the thread finish flag.
        status = AmbaKAL_EventFlagSet(&svc->port.flag, FLAG_THREAD_FINISH);
        if ( status != TX_SUCCESS ) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Set Thread Finish Flag Error: status is 0x%x", status, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

    return NULL;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  threadpool_addtask
 *
 *  @Description::  add the task in the task queue
 *
 *  @Input      ::
 *      pool : thread pool
 *      function : svc_thread_func of the svc task
 *      arg : svc task
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
static void threadpool_addtask(AMBA_THREADPOOL_s *pool, void *(*function)(void*), UINT32 arg)
{
    int status;
    UINT32 actFlags;
    status = AmbaKAL_MutexTake(&pool->lock, AMBA_KAL_WAIT_FOREVER);
    if ( status != TX_SUCCESS ) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Add Task Lock Mutex Error: status is 0x%x", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    while ( (pool->queue_count + 1) > pool->queue_size ) {  // check whether the queue is full.
        status = AmbaKAL_MutexGive(&pool->lock);  // let other threads to pick up the task.
        if ( status != TX_SUCCESS ) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "FULL!!! Add Task Lock Mutex Error: status is 0x%x", status, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
        status = AmbaKAL_EventFlagGet(
                     &pool->flag, TFLAG_QUEUE_FULL, 1U, 1U,
                     &actFlags, AMBA_KAL_WAIT_FOREVER);
        if ( status != TX_SUCCESS ) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Queue Full Error: status is 0x%x", status, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
        status = AmbaKAL_MutexTake(&pool->lock, AMBA_KAL_WAIT_FOREVER);
        if ( status != TX_SUCCESS ) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "FULL!!! Add Task Unlock Mutex Error: status is 0x%x", status, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

    /* Add task to the circular queue */
    pool->queue[pool->tail].function = function;
    pool->queue[pool->tail].arg = arg;
    pool->tail = (pool->tail + 1) % pool->queue_size;
    pool->queue_count++;
    status = AmbaKAL_EventFlagSet(&pool->flag, TFLAG_QUEUE_EMPTY);
    if ( status != TX_SUCCESS ) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Queue Empty: Event Flag Give Error: status is 0x%x", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    status = AmbaKAL_MutexGive(&pool->lock);
    if ( status != TX_SUCCESS ) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Add Task Ulock Mutex Error: status is 0x%x", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  svc_recv
 *
 *  @Description::  callback to recv request
 *
 *  @Input      ::
 *      context :
 *      pkt :
 *      len :
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int svc_recv(void *context, AMBA_IPC_PKT_s *pkt, int len)
{
    AMBA_IPC_SVC_HANDLE_s *svc = (AMBA_IPC_SVC_HANDLE_s *)context;
    UINT32 actFlags;
    int status;

    svc->recv_pkt = pkt;
    svc->recv_len = len;
    status = AmbaKAL_EventFlagSet(&svc->port.flag, FLAG_DATA_READY);
    if ( status != TX_SUCCESS ) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "SVC_RECV Event Flag Give Error: status is 0x%x", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    if ( svc->new_thread == 0 ) {
        threadpool_addtask(pool, &svc_thread_func, (UINT32)(ULONG) svc);
    }
    status = AmbaKAL_EventFlagGet(&svc->port.flag, FLAG_DATA_CONSUMED,
                                   1U, 1U, &actFlags, AMBA_KAL_WAIT_FOREVER);
    //don't use NULL for actual_flags_ptr
    if ( status != TX_SUCCESS ) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "SVC_RECV Event Flag Take Error: status is 0x%x", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    return 0;
}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  threadpool_func
 *
 *  @Description::  thread of the thread pool will execute this function
 *
 *  @Input      ::
 *      arg : threadpool
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void *threadpool_func(void *arg)
{
    AMBA_THREADPOOL_s *pool = (AMBA_THREADPOOL_s *)arg;
    AMBA_IPC_SVC_HANDLE_s *svc;
    AMBA_THREADPOOL_TASK_s task;
    int errno;
    UINT32 actFlags;
    while (1) {
        errno = AmbaKAL_MutexTake(&pool->lock, AMBA_KAL_WAIT_FOREVER);
        if ( errno != TX_SUCCESS ) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "ThreadPoolFunc Mutex Take Error: status is 0x%x", errno, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
        while (pool->queue_count == 0) {
            AmbaKAL_MutexGive(&pool->lock); /* let other threads can add the task in the queue */
            errno = AmbaKAL_EventFlagGet(
                        &pool->flag, TFLAG_QUEUE_EMPTY, 1U, 1U,
                        &actFlags, AMBA_KAL_WAIT_FOREVER);
            if ( errno != TX_SUCCESS ) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Queue Empty: Flag Take Error: status is 0x%x", errno, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
            AmbaKAL_MutexTake(&pool->lock, AMBA_KAL_WAIT_FOREVER); /* take the lock again */
        }

        /* Choose a task from the head of the queue */
        svc = (AMBA_IPC_SVC_HANDLE_s *)(ULONG)pool->queue[pool->head].arg;
        while ( svc == NULL ) { // Skip the unregistered svc
            pool->queue_count--;
            pool->head = (pool->head + 1) % pool->queue_size;
            svc = (AMBA_IPC_SVC_HANDLE_s *)(ULONG)pool->queue[pool->head].arg;
        }
        task.function = pool->queue[pool->head].function;
        task.arg = pool->queue[pool->head].arg;
        pool->queue_count--;
        pool->head = (pool->head + 1) % pool->queue_size;
        errno = AmbaKAL_EventFlagSet(&pool->flag, TFLAG_QUEUE_FULL);
        if ( errno != TX_SUCCESS ) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Queue Full: Flag Give Error: status is 0x%x", errno, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
        errno = AmbaKAL_MutexGive(&pool->lock);
        if ( errno != TX_SUCCESS ) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "ThreadPoolFunc Mutex Give Error: status is 0x%x", errno, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
        AmbaKAL_TaskSleep(1);      // yield the cpu
        /* Execute the task */
        task.function((void *)(ULONG)task.arg);
        AmbaKAL_TaskSleep(1);      // yield the cpu
    }

    return NULL;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  threadpool_free
 *
 *  @Description::  free the resouce for thread pool
 *
 *  @Input      ::
 *      pool :
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
static void threadpool_free(AMBA_THREADPOOL_s *pool)
{
    if (pool->threads) {
        AmbaLink_Free(MEM_POOL_TPOOL_THREAD_BUF, pool->threads);
    }
    if (pool->queue) {
        AmbaLink_Free(MEM_POOL_TPOOL_QUEUE_BUF, pool->queue);
    }
    if (pool->stack) {
        AmbaLink_Free(MEM_POOL_TPOOL_STACK_BUF, pool->stack);
    }
    AmbaKAL_MutexDelete(&pool->lock);
    AmbaKAL_EventFlagDelete(&pool->flag);
    AmbaLink_Free(MEM_POOL_TPOOL_BUF, pool);
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  threadpool_create
 *
 *  @Description::  creat a thread pool to let the fixed threads to serve the incoming services.
 *
 *  @Input      ::
 *      thread_num : how many threads you want to create in the thread pool
 *      queue_size : the task queue size
 *      priority :
 *      stack_size :
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
static void threadpool_create(int thread_num, int queue_size, UINT32 priority, UINT32 stack_size)
{
    int i;
    UINT32 errno = 0;
    AMBA_LINK_TASK_CTRL_s *pTask;

    pTask = &AmbaLinkTaskCtrl[LINK_TASK_SVC];

    pool = AmbaLink_Malloc(MEM_POOL_TPOOL_BUF, sizeof(AMBA_THREADPOOL_s));

    /* Initialize */
    pool->thread_count = thread_num;
    pool->queue_size = queue_size;
    pool->queue_count = 0;
    pool->head = pool->tail = 0;
    pool->threads = AmbaLink_Malloc(MEM_POOL_TPOOL_THREAD_BUF, thread_num * sizeof(AMBA_KAL_TASK_t));
    AmbaKAL_MutexCreate(&pool->lock,"ThreadPool");
    AmbaKAL_EventFlagCreate(&pool->flag,"ThreadPoolFlag");
    pool->queue = AmbaLink_Malloc(MEM_POOL_TPOOL_QUEUE_BUF, queue_size * sizeof(AMBA_THREADPOOL_TASK_s));

    /* Create worker threads */
    for (i = 0; i < thread_num; i++) {
        strcpy(pool->thread_name, "LINK_RPC_SVC_THREAD ");
        AmbaUtility_StringAppendUInt32(pool->thread_name, sizeof(pool->thread_name), i, 10U);
        pool->stack = AmbaLink_Malloc(MEM_POOL_TPOOL_STACK_BUF, stack_size);
        errno = AmbaKAL_TaskCreate(&pool->threads[i],
                                   pool->thread_name, (priority + i),
                                   &threadpool_func, (void *) pool,
                                   (void *) pool->stack, stack_size, 0U);
        if (errno != TX_SUCCESS) {
            goto err;
        }

        errno = AmbaKAL_TaskSetSmpAffinity(&pool->threads[i],(pTask->CoreSel) & AMBA_KAL_CPU_CORE_MASK);
        if (errno) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_TaskSetSmpAffinity is failed! (0x%x)", errno, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }

        errno = AmbaKAL_TaskResume(&pool->threads[i]);
        if (errno) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_TaskResume is failed! (%d)", errno, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

    return;

err:
    if (pool) {
        threadpool_free(pool);
    }
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: threadpool_clear
 *
 *  @Description:: clear the client requests for the svc in the threadpool
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
static void threadpool_clear(AMBA_IPC_SVC_HANDLE_s *svc)
{
    int i;
    AMBA_IPC_SVC_HANDLE_s *svc_pool;
    AmbaKAL_MutexTake(&pool->lock, AMBA_KAL_WAIT_FOREVER);
    /* search any client request for this svc still in the task queue */
    for (i = pool->head; i < pool->queue_count; i++) {
        svc_pool = (AMBA_IPC_SVC_HANDLE_s *)(ULONG) pool->queue[i].arg;
        if (svc_pool == svc) {
            pool->queue[i].arg = 0;  //clear the svc
        }
    }
    AmbaKAL_MutexGive(&pool->lock);
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_ListSvc
 *
 *  @Description:: list all registered service
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaIPC_ListSvc(AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_IPC_SVC_HANDLE_s *svc;

    PrintFunc("   program version port  name\n");
    list_for_each_entry(svc, &head, AMBA_IPC_SVC_HANDLE_s, list) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, " %7d %6d  %5d   ",svc->prog, svc->vers, svc->port.id, 0U, 0U);
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s\n", svc->name, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_SvcRegister
 *
 *  @Description::  register a service with binder and create a new thread to handle incoming calls from client
 *
 *  @Input      ::
 *      prog :
 *      vers :
 *      dispatch:
 *      name:
 *      priority:
 *      stack:
 *      stack_size:
 *      info: IPC program information
 *      new_thread: 1 means creating a new thread to run svc.
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_SvcRegister(INT32 prog,
                        INT32 vers,
                        char * const name,
                        UINT32 priority,
                        void *stack, UINT32 stack_size,
                        AMBA_IPC_PROG_INFO_s *info,
                        INT32 new_thread)
{
    AMBA_IPC_SVC_HANDLE_s  *svc = find_svc(prog, vers);

    if (svc) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Prog %d has been resgistered already\n", prog, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        return 1;
    }

    svc = AmbaLink_Malloc(MEM_POOL_SVC_BUF, sizeof(AMBA_IPC_SVC_HANDLE_s) + AmbaLinkCtrl.RpcPktSize);
    memset(svc, 0, sizeof(AMBA_IPC_SVC_HANDLE_s));
    svc->prog = prog;
    svc->vers = vers;
    svc->name = name;
    svc->new_thread = new_thread;   // use a new thread to run this svc.
    svc->prog_info.ProcNum = info->ProcNum;
    svc->prog_info.pProcInfo = AmbaLink_Malloc(MEM_POOL_PROC_INFO_BUF, info->ProcNum * sizeof(AMBA_IPC_PROC_s));
    // record the procedure information
    memcpy(svc->prog_info.pProcInfo, info->pProcInfo, info->ProcNum * sizeof(AMBA_IPC_PROC_s));
    xprt_init_port(&svc->port, (void *) svc, &svc_recv);

    // insert this svc to the list
    AmbaIPC_ListAdd(&svc->list, &head);

    if (new_thread) {   // create a new thread.
        INT32 rval;
        rval = AmbaKAL_TaskCreate(&svc->thread, name, priority,
                           &svc_thread_func, (void *) svc,
                           (void *) stack, stack_size, 0U);
        if (rval != OK) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: task create fail!!!", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            return -1;
        } else {
            //rval = AmbaKAL_TaskSetSmpAffinity(&svc->thread, 0x1U << CONFIG_BOOT_CORE_SRTOS);
            rval = AmbaKAL_TaskSetSmpAffinity(&svc->thread, 0x2U);
            if (rval != OK) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Failed to Specify core task %s", __func__, name, NULL, NULL, NULL);
                AmbaPrint_Flush();
            } else {
                rval = AmbaKAL_TaskResume(&svc->thread);
                if (rval != OK) {
                    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "%d", __LINE__, 0U, 0U, 0U, 0U);
                    AmbaPrint_Flush();
                }
            }
        }
    }
    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_SvcUnregister
 *
 *  @Description::  unregister a service with binder and terminate the corresponding svc thread
 *
 *  @Input      ::
 *      prog :
 *      vers :
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_SvcUnregister(INT32 prog, INT32 vers)
{
    AMBA_IPC_SVC_HANDLE_s *svc = find_svc(prog, vers);
    UINT32 actFlags;
    AMBA_KAL_TASK_INFO_s task_info;
    int status;

    if (!svc)
        return 0;

    // wait for the svc thread to finsh
    AmbaKAL_EventFlagSet(&svc->port.flag, FLAG_THREAD_CANCEL);
    if (!svc->new_thread) {
        /* clear all the client requests for this svc in the pool,
        so set flag FLAG_THREAD_FINISH itself */
        threadpool_clear(svc);
    }

    AmbaKAL_EventFlagGet(&svc->port.flag, FLAG_THREAD_FINISH,
                          1U, 1U, &actFlags, AMBA_KAL_WAIT_FOREVER);

    if ( svc->new_thread ) {
        AmbaKAL_TaskQuery(&svc->thread, &task_info);
        if ((task_info.TaskState != TX_COMPLETED) && (task_info.TaskState != TX_TERMINATED)) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", svc->thread.tx_thread_name, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            status = AmbaKAL_TaskTerminate(&svc->thread);
            if ( status != TX_SUCCESS ) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Terminate SVC Task Error: status is %d", status, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
        }
        status = AmbaKAL_TaskDelete(&svc->thread);
        if ( status != TX_SUCCESS ) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Kill SVC Task Error: status is %d", status, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
    }
    AmbaIPC_ListDel(&svc->list);
    xprt_exit_port(&svc->port);
    AmbaLink_Free(MEM_POOL_PROC_INFO_BUF, svc->prog_info.pProcInfo);
    AmbaLink_Free(MEM_POOL_SVC_BUF, svc);

    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::  binder_recv
 *
 *  @Description::  sepcial callback fucntion for binder port
 *
 *  @Input      ::
 *      context :
 *      pkt :
 *      len :
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*----------------------------------------------------------------------------------------------*/
static int binder_recv(void *context, AMBA_IPC_PKT_s *pkt, int len)
{
    AMBA_IPC_PORT_s *port = (AMBA_IPC_PORT_s*)context;
    AMBA_IPC_SVC_HANDLE_s *svc;
    AMBA_IPC_COMMUICATION_MODE_e *mode;
    int i,size;
    UINT64 *parameters = (UINT64 *)((char *)&pkt->msg + sizeof(AMBA_IPC_MSG_s));

    switch (pkt->msg.u.call.proc) {
    case AMBA_IPC_BINDER_BIND:
        pkt->msg.type = AMBA_IPC_MSG_REPLY;
        svc = find_svc(pkt->msg.u.call.prog,  pkt->msg.u.call.vers);
        if (svc) {
            pkt->msg.u.reply.status = AMBA_IPC_REPLY_SUCCESS;
            parameters[0] = svc->port.id;
            parameters[1] = find_crc(svc->prog);
            parameters[2] = svc->prog_info.ProcNum;
            size = svc->prog_info.ProcNum * sizeof(AMBA_IPC_COMMUICATION_MODE_e);
            mode = (AMBA_IPC_COMMUICATION_MODE_e *)AmbaLink_Malloc(MEM_POOL_COMM_BUF, size);
            // we only need to send the communication mode of procedures.
            for (i = 0; i < svc->prog_info.ProcNum; i++) {
                mode[i] = svc->prog_info.pProcInfo[i].Mode;
            }
            memcpy(&parameters[3], mode, svc->prog_info.ProcNum * sizeof(AMBA_IPC_COMMUICATION_MODE_e));
            xprt_send_pkt(port, pkt, RPC_PARAM_SIZE*3 + svc->prog_info.ProcNum * sizeof(AMBA_IPC_COMMUICATION_MODE_e));
            AmbaLink_Free(MEM_POOL_COMM_BUF, mode);
        } else {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "info: can't find rpc server, re-bind next time.\n", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            pkt->msg.u.reply.status = AMBA_IPC_REPLY_PROG_UNAVAIL;
            xprt_send_pkt(port, pkt, 0);
        }
        break;
    case AMBA_IPC_BINDER_LIST:
        pkt->msg.type = AMBA_IPC_MSG_REPLY;
        pkt->msg.u.reply.status  = AMBA_IPC_REPLY_SUCCESS;
        list_for_each_entry(svc, &head, AMBA_IPC_SVC_HANDLE_s, list) {
            int len = strlen(svc->name) + 1;
            parameters[0] = svc->prog;
            parameters[1] = svc->vers;
            parameters[2] = svc->port.id;
            memcpy(&parameters[3], svc->name, len);
            xprt_send_pkt(port, pkt, RPC_PARAM_SIZE*3 + len);
        }
        memset(parameters, 0, RPC_PARAM_SIZE*3);
        xprt_send_pkt(port, pkt, RPC_PARAM_SIZE*3);
        break;
    case AMBA_IPC_BINDER_REBIND:
        svc = find_svc(pkt->msg.u.call.prog,  pkt->msg.u.call.vers);
        if(svc) {
            // automatically relay the pkt to svc
            pkt->xprt.server_port = svc->port.id;
            pkt->msg.u.call.proc = pkt->xprt.private; /* restore the original proc */
            xprt_send_pkt(port, pkt, len);

            // update the information
            pkt->xprt.server_port = pkt->xprt.client_ctrl_port; /* dst is client controller */
            pkt->xprt.server_addr = AMBA_IPC_HOST_LINUX;
            pkt->xprt.client_addr = AMBA_IPC_HOST_THREADX;
            pkt->msg.u.call.proc = AMBA_IPC_CLNT_REBIND;
            parameters[0] = svc->port.id;  /* update the svc port */
            xprt_send_pkt(port, pkt, RPC_PARAM_SIZE);
        } else {
            // really can't find this svc
            if(pkt->xprt.mode == AMBA_IPC_SYNCHRONOUS) {
                /* it needs to send reply when clnt call is sync. */
                pkt->msg.type = AMBA_IPC_MSG_REPLY;
                pkt->msg.u.reply.status = AMBA_IPC_REPLY_PROG_UNAVAIL;
                xprt_send_pkt(port, pkt, RPC_PARAM_SIZE);
            } else {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "warning: can't find the rpc port.", NULL, NULL, NULL, NULL, NULL);
                AmbaPrint_Flush();
            }
        }
        break;
    default:
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "\tunknown case: %d\n", pkt->msg.u.call.proc, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        break;
    }
    return 0;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_InitBinder
 *
 *  @Description:: Init binder
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaIPC_InitBinder(void)
{
    static AMBA_IPC_PORT_s   binder_port;
    AMBA_LINK_TASK_CTRL_s *pTask;

    pTask = &AmbaLinkTaskCtrl[LINK_TASK_SVC];

    /* create the thread pool to serve the svcs */
    threadpool_create(SVC_THREAD_NUM, 100, pTask->Priority, pTask->StackSize);

    /* setup the binding port */
    binder_port.id = AMBA_IPC_BINDING_PORT;
    xprt_init_port(&binder_port, (void *) &binder_port, binder_recv);
}

