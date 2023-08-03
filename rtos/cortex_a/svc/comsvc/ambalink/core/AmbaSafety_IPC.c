/**
 * @file AmbaSafety_IPC.c
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
 *  @details ipc safety
 */
/**
   Safety Measures
   1. register shadow
      *hardware shadow is not used because ipc is not a peripheral driver
      *software shadow (shared memory) is used to detect IPC error & A53 random failure
   2. safe state
      *R52 CEHU apps can call API to enter safe state
      *all API returns directly in safe state
      *safe state will disable irq
   3. software watchdog
      *rpmsg heartbeat: A53 write unique id as rpmsg heartbeat in shadow area (write 0 when rpmsg idle)
      *CPU heartbeat: A53 write system tick value in shadow area as A53 CPU heartbeat
      *R52 monitor health heartbeats, report to CEHU when hardware-halt
       random hardware fault ->
       transaction cannot finish / safety task not running ->
       heartbeat stopped ->
       report to CEHU ->
       user-defined error handler
   4. API control flow
      *all API will return directly if AmbaIPC_Init() is not called
   5. irq flooding detection
      *when more than 1000irqs/100ms, notify CEHU
   6. ipc protocol design
      *detect packet lost & packet delay: each message must be ACKed within timeout
      *detect packet corrupt: each message is CRC32-protected
   7. software lock-step calculation
      *when hardware fault cannot be detected in all other safety-measure mechanisms
       selected functions can be duplicated as software lock-step calculation
 */
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaLink.h"
#include "AmbaIPC_Lock.h"
#include "AmbaLinkPrivate.h"
#include "AmbaPrint.h"
#include "AmbaIOUtility.h"
#include "AmbaWrap.h"
#include "AmbaLink_core.h"
#include "AmbaSafety_IPC.h"

#define IPCW_Q_DEBUG        (0)

#define SPEC_DTTI           (100U)                               /**< Diagnostic Test Time Interval */
#if defined(CONFIG_CPU_CORTEX_R52)
#include "AmbaSYSErrorManager.h"
#define IPCW_IRQTH          (SPEC_DTTI * 10U)                    /**< irq flood threshold */
#define CPUW_CNT            (1U)                                 /**< R52 CPU watcher count down */
#define IPCW_CNT            (1U)                                 /**< R52 rpmsg watcher count down */
//#define CREATE_IPC_SAFETY_TASK
#else
#define CREATE_IPC_SAFETY_TASK
#define IPCW_TIMER          (SPEC_DTTI / 2U)                     /**< timer interval */
#define IPCW_CNT            (2U)                                 /**< A53 rpmsg watcher count down */
#endif

typedef struct {
    UINT32 HeartBeat[2];                                         /**< shadow health heartbeat */
    UINT32 SafeState[2];                                         /**< shadow safe state */
    UINT32 IrqCnt[2];                                            /**< irq status */
} AMBA_IPC_SHADOW_s;

typedef struct IPCW_Q_s {
#if IPCW_Q_DEBUG
    UINT64 idx;     /**< Index number in IPCW_Q pool */
#endif
    UINT32 hp;      /**< health points to count down for each timeout period. */
    UINT32 id;      /**< ID of msg */
    UINT32 ts;      /**< transaction start timestamp */
    struct IPCW_Q_s *next;
} IPCW_Q_t;

#define IPCW_QSIZE          ((UINT32)CONFIG_AMBALINK_G2_RPMSG_NUM * 3U / 2U)

static AMBA_KAL_MUTEX_t     IPCW_QMut;                           /**< queue lock */
static UINT32               IPCW_Inited;                         /**< ipc safety init state */
static UINT32              *pIPCIrqCnt[2];                       /**< irqs status */
static UINT32              *pIPCSafeState[2];                    /**< safe state */
static UINT32              *pIPCHeartBeat[2];                    /**< heartbeat */
static IPCW_Q_t            *pIPCW_Qidle;                         /**< queue idle */
static IPCW_Q_t            *pIPCW_Qhd;                           /**< queue head */
#if IPCW_Q_DEBUG
static UINT64               q_idled;
#endif

static void IPCW_Print(const char *func, const UINT32 line, const UINT16 ModuleID, const char *color, const char *msg)
{
    char Buf[22];

    (void)IO_UtilityUInt32ToStr(Buf, sizeof(Buf), line, 10U);
    AmbaPrint_ModulePrintStr5(ModuleID, "%s IPC %s(%s) %s %s", color, func, Buf, msg, ANSI_RESET);
}

/* Note: caller should get mutex */
static IPCW_Q_t *IPCW_Q_alloc(void)
{
    IPCW_Q_t *p = pIPCW_Qidle;

    if (p != NULL) {
        pIPCW_Qidle = p->next;
        p->next = NULL;
#if IPCW_Q_DEBUG
        q_idled--;
#endif
    } else {
        IPCW_Print(__func__, __LINE__, AMBALINK_MODULE_ID, ANSI_RED, "NULL pIPCW_Qidle");
    }

    return p;
}

/* Note: caller should get mutex */
static void IPCW_Q_free(IPCW_Q_t *p)
{
    if (p != NULL) {
        p->next = pIPCW_Qidle;
        pIPCW_Qidle = p;
#if IPCW_Q_DEBUG
        q_idled++;
#endif
    }
}

/* Idx 0: external, 1: (internal) notify R52 CEHU */
static UINT32 internal_AmbaSafety_IPCSetErrID(UINT32 Idx, UINT32 State)
{
    //AmbaSafety_IPCShadowLock();
    *pIPCSafeState[Idx] = State;
    //AmbaSafety_IPCShadowUnLock();
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID,ANSI_RED "IPCSafeState[%d] = %d" ANSI_RESET, Idx, State, 0U, 0U, 0U);

    return 0U;
}

/**
 * set current ipc Safe State
 * @param [in] State non-zero: enter safe state, zero: leave safe state
 * @return ipc error number
 */
UINT32 AmbaSafety_IPCSetSafeState(UINT32 State)
{
#if defined(CONFIG_CPU_CORTEX_R52)
    /*R52: notify A53 to enter safe state*/
    return internal_AmbaSafety_IPCSetErrID(0U, State);
#else
    /*A53: notify R52 CEHU
     *     A53 calls AmbaSafety_IPCSetSafeState() ->
     *     R52 safety task notify CEHU
     *     R52 CEHU APP calls AmbaSafety_IPCSetSafeState() ->
     *     A53 safety task disable irq
     */
    return internal_AmbaSafety_IPCSetErrID(1U, State);
#endif
}

/* Idx 0: external, 1: (internal) notify R52 CEHU */
static UINT32 internal_AmbaSafety_IPCGetSafeState(UINT32 Idx, UINT32 *pState)
{
    UINT32 Ret = 0U;

    //AmbaSafety_IPCShadowLock();
    if (pState != NULL) {
        *pState = *pIPCSafeState[Idx];
    } else {
        /* when pState is NULL, return State directly */
        Ret = *pIPCSafeState[Idx];
    }
    //AmbaSafety_IPCShadowUnLock();
    return Ret;
}

/**
 * get current ipc Safe State
 *
 * @param [in] pState 1: enter safe state, 0: leave safe state
 * @return when pState is NULL, return State directly
 */
UINT32 AmbaSafety_IPCGetSafeState(UINT32 *pState)
{
    return internal_AmbaSafety_IPCGetSafeState(0U, pState);
}


/**
 * set current ipc Health State
 *
 * @param [in] Idx 0 for RPMSG HeartBeat, 1 for A53 HeartBeat
 * @param [in] HS Health State(heartbeat)
 * @return ipc error number
 */
static UINT32 AmbaSafety_SetHeartBeat(UINT32 Idx, UINT32 HS)
{
    UINT32 Ret = 0U;

    //AmbaSafety_IPCShadowLock();
    *pIPCHeartBeat[Idx] = HS;
    //AmbaSafety_IPCShadowUnLock();

    return Ret;
}

/**
 * add monitor object to ipc watchdog timer
 *
 * @param [in] wid the watch id to identify your monitor object
 * @return ipc error number
 */
UINT32 AmbaSafety_IPCEnq(UINT32 wid)
{
    IPCW_Q_t *p;
    UINT32 Ret = 0U;

    /* code start */
    /* wid is unique for RPMSG(Idx 0) HeartBeat */
    (void)AmbaSafety_SetHeartBeat(0U, wid);
    (void)AmbaKAL_MutexTake(&IPCW_QMut, AMBA_KAL_WAIT_FOREVER);
    p = IPCW_Q_alloc();
    if (p == NULL) {
        IPCW_Print(__func__, __LINE__, AMBALINK_MODULE_ID, ANSI_RED, "NULL IPCW_Q_alloc");
        Ret = IPC_ERR_EINVAL;
    } else {
        (void)AmbaKAL_GetSysTickCount(&p->ts);
        p->hp = IPCW_CNT;
        p->id = wid;
        IpcDebug("IPC %s(%d) #%d hp %d", __func__, __LINE__, p->id, p->hp);
        if (pIPCW_Qhd != NULL) {
            p->next = pIPCW_Qhd;
        } else {
            p->next = NULL;
        }
        pIPCW_Qhd = p;
    }
    (void)AmbaKAL_MutexGive(&IPCW_QMut);

    return Ret;
}

/**
 * remove monitor object from ipc watchdog timer
 *
 * @param [in] wid the watch id to identify your monitor object
 * @return ipc error number
 */
UINT32 AmbaSafety_IPCDeq(UINT32 wid)
{
    IPCW_Q_t *cur;
    IPCW_Q_t *pre;
    UINT32 Ret = IPC_ERR_EINVAL;

    /* code start */
    /* set 0 for RPMSG(Idx 0) HeartBeat bus idle */
    (void)AmbaSafety_SetHeartBeat(0U, 0U);
    (void)AmbaKAL_MutexTake(&IPCW_QMut, AMBA_KAL_WAIT_FOREVER);
    cur = pIPCW_Qhd;
    pre = pIPCW_Qhd;
    if (cur == NULL) {
        /* re-entrant: this is normal */
        IpcDebug("IPC %s(%d) NULL pIPCW_Qhd: wid %d already removed", __func__, __LINE__, wid);
        Ret = 0U;
    } else {
        for (;;) {
            if ((cur == NULL) || (Ret == 0U)) {
                break;
            }
            if (cur->id == wid) {
                IpcDebug("IPC %s(%d) #%d hp %d", __func__, __LINE__, cur->id, cur->hp);
                if (cur == pIPCW_Qhd) {
                    pIPCW_Qhd = cur->next;
                } else {
                    pre->next = cur->next;
                }

                IPCW_Q_free(cur);
                // to break
                Ret = 0U;
                continue;
            } else {
                pre = cur;
                cur = cur->next;
            }
        }
    }
    (void)AmbaKAL_MutexGive(&IPCW_QMut);

    return Ret;
}

#if 0  /* do not need locking as long as A53_DTTI=R52_DTTI/2 */
/**
 * lock ipc register shadow
 *
 * @return void
 */
static void AmbaSafety_IPCShadowLock(void)
{
    if (0U != AmbaIPC_MutexTake(AMBA_IPC_MUTEX_RPMSG, 10U)) {
        IPCW_Print(__func__, __LINE__, AMBALINK_MODULE_ID, ANSI_RED, "AmbaIPC_MutexTake timeout");
    }
}

/**
 * lock ipc register shadow
 *
 * @return void
 */
static void AmbaSafety_IPCShadowUnLock(void)
{
    (void)AmbaIPC_MutexGive(AMBA_IPC_MUTEX_RPMSG);
    (void);
}
#endif

/**
 * irq callback function for safety check like flooding detection
 *
 * @return void
 */
UINT32 AmbaSafety_IPCIrqHdlr(void)
{
    const UINT32 irq = *pIPCIrqCnt[0] + 1U;
    *pIPCIrqCnt[0] = irq;
    return irq;
}

/*
 * check watch queue
 */
static void IPCW_Q_Check(void)
{
    IPCW_Q_t *cur;
    IPCW_Q_t *p;
#if IPCW_Q_DEBUG
    UINT32 SysTickCount;
#define CNT_PRINT_HIT   0x100
    static UINT64 cnt_print = 0;
#endif


    if (IPCW_Inited != 0U)
    {
        /* remove timed out items BEFORE count down */
        (void)AmbaKAL_MutexTake(&IPCW_QMut, AMBA_KAL_WAIT_FOREVER);

        /* count down and check */
        cur = pIPCW_Qhd;
        for (;;) {
            if (cur == NULL) {
                break;
            }
            if (cur->hp > 0u) {
                cur->hp--;
            }
#if IPCW_Q_DEBUG
            (void)AmbaKAL_GetSysTickCount(&SysTickCount);
            if ((cur->hp == 0U) &&
                (SysTickCount > (SPEC_DTTI + cur->ts))) {
#else
            if (cur->hp == 0U) {
#endif
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID,ANSI_RED "rpmsg id %d ts %d timed-out" ANSI_RESET, cur->id, cur->ts, 0U, 0U, 0U);
#if defined(CONFIG_CPU_CORTEX_R52)
                    (void)AmbaSEMGR_ModuleErrorReport(AMBALINK_MODULE_ID, 0U, 0U, IPC_ERR_ETIMEDOUT);
#else
                    /*A53: notify R52 CEHU */
                    (void)AmbaSafety_IPCSetSafeState(IPC_ERR_ETIMEDOUT);
#endif
                p = cur->next;
                if (0U != AmbaSafety_IPCDeq(cur->id)) {
                    IPCW_Print(__func__, __LINE__, AMBALINK_MODULE_ID, ANSI_RED, "AmbaSafety_IPCDeq failed");
                }
                cur = p;
            } else {
                cur = cur->next;
            }
        }
#if IPCW_Q_DEBUG
        cnt_print++;
        if ((cnt_print % CNT_PRINT_HIT) == 0u) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID,ANSI_RED "Current idled/free to q: %d" ANSI_RESET, q_idled, 0U, 0U, 0U, 0U);
        }
#endif // IPCW_Q_DEBUG
        (void)AmbaKAL_MutexGive(&IPCW_QMut);
    }
}

#if defined(CONFIG_CPU_CORTEX_R52)
/**
 * get current ipc Health State
 *
 * @param [in] Idx 0 for RPMSG HeartBeat, 1 for A53 HeartBeat
 * @return current unique health heartbeat
 */
static UINT32 AmbaSafety_GetHeartBeat(UINT32 Idx)
{
    UINT32 Ret;

    /* code start */
    //AmbaSafety_IPCShadowLock();
    Ret = *pIPCHeartBeat[Idx];
    //AmbaSafety_IPCShadowUnLock();

    return Ret;
}

static UINT32 internal_AmbaSafety_IPCAsilChecker(void)
{
    UINT32 beat, irq;
    static UINT32 LastBeat[2] = { 0U, 0U };
    static UINT32 hp[2] = { IPCW_CNT, CPUW_CNT };
    static UINT32 LastIrqCnt = 0U, LastR52State = 0U;
    UINT32 A53State, R52State;

    if (IPCW_Inited != 0U)
    {
        (void)internal_AmbaSafety_IPCGetSafeState(0U, &R52State);
        if (R52State != 0U) {
            /* enter safe state */
            if (LastR52State != R52State) {
                IpcDebug("IPC %s(%d) enter SafeState %d -> %d, disable irq", __func__, __LINE__, LastR52State, R52State);
                (void)AmbaIPC_INTDisable();
            }
        } else {
            /* did A53 reported error? */
            (void)internal_AmbaSafety_IPCGetSafeState(1U, &A53State);
            if (A53State != 0U){
                /* clear A53 err */
                (void)internal_AmbaSafety_IPCSetErrID(1U, 0U);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "A53 reported error: A53State %d", A53State, 0U, 0U, 0U, 0U);
                IPCW_Print(__func__, __LINE__, AMBALINK_MODULE_ID, ANSI_RED, "AmbaSEMGR_ModuleErrorReport");
                (void)AmbaSEMGR_ModuleErrorReport(AMBALINK_MODULE_ID, 0U, 0U, A53State);
            }
        }
        LastR52State = R52State;

        /* detect irq flooding */
        irq = *pIPCIrqCnt[0];
        if ((irq > LastIrqCnt) &&
            (irq > (LastIrqCnt + IPCW_IRQTH))) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "irq flooding LastIrqCnt %d -> irq %d", LastIrqCnt, irq, 0U, 0U, 0U);
            IPCW_Print(__func__, __LINE__, AMBALINK_MODULE_ID, ANSI_RED, "AmbaSEMGR_ModuleErrorReport");
            (void)AmbaSEMGR_ModuleErrorReport(AMBALINK_MODULE_ID, 0U, 0U, IPC_ERR_EINTR);
        }
        LastIrqCnt = irq;

        /* check watch queue */
        IPCW_Q_Check();

        /* are you dead? check rpmsg heartbeat */
        beat = AmbaSafety_GetHeartBeat(0U);
        if (0U == beat) {
            //IpcDebug("IPC %s(%d) bus idle", __func__, __LINE__);
            hp[0U] = IPCW_CNT;
        } else if (beat != LastBeat[0U]) {
            IpcDebug("IPC %s(%d) bus alive", __func__, __LINE__);
            hp[0U] = IPCW_CNT;
        } else {
            if (0U == hp[0U]) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "rpmsg heartbeat 0x%x not updating", LastBeat[0U], 0U, 0U, 0U, 0U);
                IPCW_Print(__func__, __LINE__, AMBALINK_MODULE_ID, ANSI_RED, "AmbaSEMGR_ModuleErrorReport");
                (void)AmbaSEMGR_ModuleErrorReport(AMBALINK_MODULE_ID, 0U, 0U, IPC_ERR_EREMOTE);
                hp[0U] = IPCW_CNT;
            } else {
                hp[0U]--;
                IpcDebug("IPC %s(%d) hp[0] %d", __func__, __LINE__, hp[0U]);
            }
        }
        LastBeat[0U] = beat;

        /* are you dead? check CA53 heartbeat */
        beat = AmbaSafety_GetHeartBeat(1U);
        if (0U == beat) {
            IpcDebug("IPC %s(%d) CA53 heartbeat not started yet", __func__, __LINE__);
            hp[1U] = CPUW_CNT;
        } else if (beat != LastBeat[1U]) {
            //IpcDebug("IPC %s(%d) A53 alive 0x%d", __func__, __LINE__, beat);
            hp[1U] = CPUW_CNT;
        } else {
            if (0U == hp[1U]) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "CA53 heartbeat 0x%x not updating", LastBeat[1U], 0U, 0U, 0U, 0U);
                IPCW_Print(__func__, __LINE__, AMBALINK_MODULE_ID, ANSI_RED, "AmbaSEMGR_ModuleErrorReport");
                (void)AmbaSEMGR_ModuleErrorReport(AMBALINK_MODULE_ID, 0U, 0U, IPC_ERR_EREMOTE);
                hp[1U] = CPUW_CNT;
            } else {
                hp[1U]--;
                IpcDebug("IPC %s(%d) hp[1] %d", __func__, __LINE__, hp[1U]);
            }
        }
        LastBeat[1U] = beat;
    }

    return 0U;
}

/**
 * safety check function in R52
 *
 * @param [in] ModuleID
 * @param [in] InstanceID
 * @param [in] InstanceID
 * @return zero: caller should not trigger CEHU error, non-zero: caller should trigger CEHU error
 */
UINT32 AmbaSafety_IPCAsilChecker(UINT32 *ModuleID, UINT32 *InstanceID, UINT32 *ApiID, UINT32 *ErrorID)
{
#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TouchUnused(ModuleID);
    AmbaMisra_TouchUnused(InstanceID);
    AmbaMisra_TouchUnused(ApiID);
    AmbaMisra_TouchUnused(ErrorID);
#else
    (void) ModuleID;
    (void) InstanceID;
    (void) ApiID;
    (void) ErrorID;
#endif

    return internal_AmbaSafety_IPCAsilChecker();
}
#else
static UINT32 AmbaSafety_IPCQmChecker(void)
{
    static UINT32 LastState = 0U;
    UINT32 State;

    if (IPCW_Inited != 0U)
    {
        /* enter safe state */
        (void)AmbaSafety_IPCGetSafeState(&State);
        if ((State != 0U) && (LastState != State)) {
            IpcDebug("IPC %s(%d) enter SafeState %d -> %d, disable irq", __func__, __LINE__, LastState, State);
            (void)AmbaIPC_INTDisable();
        }
        LastState = State;

        /* check watch queue */
        IPCW_Q_Check();
    }

    return 0U;
}
#endif  //#if defined(CONFIG_CPU_CORTEX_R52)

#if defined(CREATE_IPC_SAFETY_TASK)
static AMBA_KAL_SEMAPHORE_t IPCW_Sem;                            /**< watchdog task semaphore */

static void IPCTimerIsrFunc(UINT32 arg)
{
    (void) arg;
    if (IPCW_Inited == 1U) {
#if defined(CONFIG_CPU_CORTEX_R52)
#else
        /* system tick is unique for CA53(Idx 1) HeartBeat */
        UINT32 SysTickCount;
        (void)AmbaKAL_GetSysTickCount(&SysTickCount);
        (void)AmbaSafety_SetHeartBeat(1U, SysTickCount);
#endif
        (void) AmbaKAL_SemaphoreGive(&IPCW_Sem);
    }
}

static void *IPCSafetyTaskEntry(void *arg)
{
#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TouchUnused(arg);
#else
    (void)arg;
#endif

    IpcDebug("IPC %s(%d) start ", __func__, __LINE__);
    for(;;) {
        (void) AmbaKAL_SemaphoreTake(&IPCW_Sem, KAL_WAIT_FOREVER);
#if defined(CONFIG_CPU_CORTEX_R52)
        if (internal_AmbaSafety_IPCAsilChecker() != 0U) {
            IpcDebug("IPC %s(%d) fail in internal_AmbaSafety_IPCAsilChecker() ", __func__, __LINE__);
            break;
        }
#else
        if (AmbaSafety_IPCQmChecker() != 0U) {
            IpcDebug("IPC %s(%d) fail in AmbaSafety_IPCQmChecker()", __func__, __LINE__);
            break;
        }
#endif
        (void) AmbaKAL_TaskSleep(1);
    } //loop end

    return NULL;
}

static void IPCSafetyTaskCreate(void)
{
    AMBA_LINK_TASK_CTRL_s *pTask = &AmbaLinkTaskCtrl[LINK_TASK_SAFETY];
    UINT32 err;
    static UINT8 SafetyStack[0x4000] __attribute__((section(".bss.noinit")));
    static AMBA_KAL_TASK_t IpcTask = {
        .tx_thread_id = 0U,
    };

    if (IpcTask.tx_thread_id == 0U) {
        const UINT8 *pBuf;
        void *ptr;
        if (AmbaWrap_memset(&SafetyStack[0], 0, sizeof(SafetyStack))!= 0U) { }
        pBuf = &SafetyStack[0];
        AmbaMisra_TypeCast(&ptr, &pBuf);
        err = AmbaKAL_TaskCreate(&IpcTask,
                                 pTask->TaskName,
                                 pTask->Priority,
                                 IPCSafetyTaskEntry,
                                 NULL,
                                 ptr,
                                 (UINT32)sizeof(SafetyStack),
                                 0U);
        if (err != 0U) {
            AmbaPrint_PrintStr5("%s %s AmbaKAL_TaskCreate failed %s", ANSI_RED, __func__, ANSI_RESET, NULL, NULL);
        }

        // Using Core-0 (0x01)
        err = AmbaKAL_TaskSetSmpAffinity(&IpcTask, (pTask->CoreSel) & AMBA_KAL_CPU_CORE_MASK);
        if (err != 0U) {
            AmbaPrint_PrintStr5("%s %s AmbaKAL_TaskSetSmpAffinity failed %s", ANSI_RED, __func__, ANSI_RESET, NULL, NULL);
        }
        err = (UINT32)AmbaKAL_TaskResume(&IpcTask);
        if (err != 0U) {
            AmbaPrint_PrintStr5("%s %s AmbaKAL_TaskResume failed %s", ANSI_RED, __func__, ANSI_RESET, NULL, NULL);
        }
    }
}
#endif  //#if defined(CREATE_IPC_SAFETY_TASK)

/**
 * init ipc safety feature
 *
 * @return ipc error number
 */
UINT32 AmbaSafety_IPCInit(void)
{
    UINT32 i;
    UINT32 Ret = 0U;
    static IPCW_Q_t IPCW_Q[IPCW_QSIZE];
    UINT32 Base;
    INT32 tmpBase;
    const UINT32 *pBase;
    AMBA_IPC_SHADOW_s *pAmbaIPCShadow;

    /* arg check */
    if(IPCW_Inited != 0U) {
        IPCW_Print(__func__, __LINE__, AMBALINK_MODULE_ID, ANSI_RED, "Already init!");
        Ret = IPC_ERR_EINVAL;
    } else {
        /* code start */
#if defined(CREATE_IPC_SAFETY_TASK)
        static AMBA_KAL_TIMER_t IPCTimer = {0};
        static char IPCTimerName[16] = "IPCW";
        static char IPCSemName[16] = "IPCWSem";

        IPCSafetyTaskCreate();

        if (IPCW_Sem.tx_semaphore_id == 0U) {
            if (OK != AmbaKAL_SemaphoreCreate(&IPCW_Sem, IPCSemName, 0U)) {
                IPCW_Print(__func__, __LINE__, AMBALINK_MODULE_ID, ANSI_RED, "AmbaKAL_SemCreate failed!");
                Ret = IPC_ERR_OSERR;
            }
        }

        /* start WatchDog Timer */
        if (IPCTimer.tx_timer_id == 0U) {
            if (0U != AmbaKAL_TimerCreate(&IPCTimer, IPCTimerName, IPCTimerIsrFunc, 0U,
                IPCW_TIMER, IPCW_TIMER, 1U)) {
                IPCW_Print(__func__, __LINE__, AMBALINK_MODULE_ID, ANSI_RED, "AmbaKAL_TimerCreate failed");
                Ret = IPC_ERR_OSERR;
            }
        }
#endif
        if (IPCW_QMut.tx_mutex_id == 0U) {
            if (0U != AmbaKAL_MutexCreate(&IPCW_QMut, NULL)) {
                IPCW_Print(__func__, __LINE__, AMBALINK_MODULE_ID, ANSI_RED, "AmbaKAL_MutexCreate failed");
                Ret = IPC_ERR_OSERR;
            }
        }
        if (AmbaWrap_memset(IPCW_Q, 0, sizeof(IPCW_Q))!= 0U) { }
        for (i = 0U; i < (IPCW_QSIZE - 1U); i++) {
            IPCW_Q[i].next = &(IPCW_Q[i+1U]);
#if IPCW_Q_DEBUG
            IPCW_Q[i].idx = i;
#endif
        }
        IPCW_Q[i].next = NULL;
#if IPCW_Q_DEBUG
        IPCW_Q[i].idx = i;
        q_idled = IPCW_QSIZE;
#endif
        pIPCW_Qidle = IPCW_Q;

        tmpBase = SHADOWBASE_IPC;
        AmbaMisra_TypeCast(&Base, &tmpBase);
        /*pAmbaIPCShadow = (AMBA_IPC_SHADOW_s *)Base;*/
        pAmbaIPCShadow = NULL;
        if (AmbaWrap_memcpy(&pAmbaIPCShadow, &Base, sizeof(Base))!= 0U) { }

        for (i = 0U; i <= 1U; i++) {
            pBase = &pAmbaIPCShadow->HeartBeat[i];
            AmbaMisra_TypeCast(&pIPCHeartBeat[i], &pBase);

            pBase = &pAmbaIPCShadow->SafeState[i];
            AmbaMisra_TypeCast(&pIPCSafeState[i], &pBase);

            pBase = &pAmbaIPCShadow->IrqCnt[i];
            AmbaMisra_TypeCast(&pIPCIrqCnt[i], &pBase);
        }

        /* init shadow */
        if (AmbaWrap_memset(pAmbaIPCShadow, 0, sizeof(AMBA_IPC_SHADOW_s))!= 0U) { }

        if (Ret == 0U) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSafety_IPCInit shadow 0x%08x", SHADOWBASE_IPC, 0U, 0U, 0U, 0U);
            IPCW_Inited = 1U;
        }
    }

    return Ret;
}
