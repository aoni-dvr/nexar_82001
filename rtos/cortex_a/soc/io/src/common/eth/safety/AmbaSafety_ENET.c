/**
 * @file AmbaSafety_ENET.c
 *  Enet safety driver
 *
 * @defgroup enet title
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
 */
/**
   Safety Measures
   1. register shadow
      *shadow area is auto-synced in AmbaCSL_Enetxxx write
      *shadow area is auto-locked by IPC mutex
      *R52 compare shadow area with hardware register (only chosen significant registers were compared, not entire 8k)
   2. safe state
      *R52 CEHU apps can call API to enter safe state
      *all API returns directly in safe state
      *safe state will stop dma, disable irq
   3. software watchdog
      *A53 write unique health heartbeat in shadow area (write 0 when bus idle)
      *R52 monitor health heartbeat, report to CEHU when hardware-halt
       random hardware fault ->
       transaction cannot finish ->
       heartbeat stopped ->
       report to CEHU ->
       user-defined error handler
   4. API control flow
      *all API will return directly if AmbaEnet_Init() is not called
   5. irq flooding detection
      *when more than 1000irqs/100ms, notify CEHU
   6. (optional)(extra-safety in enet)ipc: A53 will monitor each API, send IPC to R52 when timeout
   7. software lock-step calculation
      *when hardware fault cannot be detected in all other safety-measure mechanisms
       selected functions can be duplicated as software lock-step calculation
 */
#include "AmbaIOUtility.h"
#include "AmbaKAL.h"
#include "AmbaENET.h"
#include "AmbaSafety_ENET.h"
#include "AmbaIPC.h"
#include "AmbaIPC_Lock.h"
#if defined(CONFIG_CPU_CORTEX_R52)
#include "AmbaReg_ENET.h"
#include "AmbaSYSErrorManager.h"
#if 0
void IpcDebug(const char *fmt, ...);
#define EnetDebug IpcDebug
#else
#define EnetDebug(...)
#endif
#define ENETW_IRQTH         (ENETW_DTTI * 10U)                   /**< irq flood threshold */
static AMBA_ENET_REG_s *pAmbaCSL_EnetReg[ENET_INSTANCES];
#else
#include "AmbaRTSL_ENET.h"
#endif

typedef struct ENETW_Q_s {
    UINT32 hp;      /**< health points to count down for each timeout period. */
    UINT32 id;      /**< ID of msg */
    struct ENETW_Q_s *next;
} ENETW_Q_t;

extern void AmbaPrint_ModulePrintUInt5(UINT16 ModuleID, const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
extern void AmbaPrint_ModulePrintStr5(UINT16 ModuleID, const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);

#if defined(ETHDEBUG)
#define ENETW_TO            (8000U)                              /**< watchdog time out (ms) */
#define ENETW_DTTI          (1000U)                              /**< Diagnostic Test Time Interval (ms) */
#else
#define ENETW_TO            (200U)                               /**< watchdog time out (ms) */
#define ENETW_DTTI          (100U)                               /**< Diagnostic Test Time Interval (ms) */
#endif
#define ENETW_PREFIX        "ENET"                               /**< IPC message */
#define ENETW_QSIZE         (128U)                               /**< queue size */
#define ENETW_CNT           (ENETW_TO / ENETW_DTTI)              /**< count down for each timeout period. */
#define ENETW_SHADOW_RANGE  (0x0044U)                            /**< reg shadow range to watch */
static AMBA_KAL_SEMAPHORE_t EnetW_Sem[ENET_INSTANCES];           /**< watchdog task semaphore */
static AMBA_KAL_MUTEX_t     EnetW_IdMut[ENET_INSTANCES];         /**< watch index lock */
static AMBA_KAL_MUTEX_t     EnetW_QMut[ENET_INSTANCES];          /**< queue lock */
static UINT32               EnetW_Inited[ENET_INSTANCES];        /**< enet safety init state */
static UINT32              *pEnetIrqCnt[ENET_INSTANCES];         /**< irqs status */
static UINT32              *pEnetSafeState[ENET_INSTANCES];      /**< safe state */
static UINT32              *pEnetHeartBeat[ENET_INSTANCES];      /**< heartbeat */
static ENETW_Q_t           *pEnetW_Qidle[ENET_INSTANCES];        /**< queue idle */
static ENETW_Q_t           *pEnetW_Qhd[ENET_INSTANCES];          /**< queue head */
static ENETW_Q_t           *pEnetW_Qtl[ENET_INSTANCES];          /**< queue tail */
static AMBA_ENET_REG_s     *pAmbaCSL_EnetShadow[ENET_INSTANCES]; /**< register shadow */
static AMBA_IPC_HANDLE      EnetIPC_Handle;                      /**< IPC channel */

static void EnetW_Print(const char *func, const UINT32 line, const UINT16 ModuleID, const char *color, const char *msg)
{
    char Buf[22];

    (void)IO_UtilityUInt32ToStr(Buf, sizeof(Buf), line, 10U);
    AmbaPrint_ModulePrintStr5(ModuleID, "%s ENET %s(%s) %s %s", color, func, Buf, msg, ANSI_RESET);
}

static INT32 EnetW_MsgHandler(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    UINT32 Idx;
    const char * str;
    UINT32 len = IO_UtilityStringLength(ENETW_PREFIX);
#if defined(AMBA_MISRA_FIX_H)
    AmbaMisra_TypeCast(&str, &pMsgCtrl->pMsgData);
#else
    str = (char *)pMsgCtrl->pMsgData;
#endif
    (void)IpcHandle;
    /* [MISRA Rule 8.13] could be declared as pointing to const */
    (void)pMsgCtrl;
    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s [%s]", __func__, str, NULL, NULL, NULL);
    if (0 == IO_UtilityStringCompare(str, ENETW_PREFIX, len)) {
        if (0U == IO_UtilityStringToUInt32(&str[len], &Idx)) {
#if defined(CONFIG_CPU_CORTEX_R52)
            EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "AmbaSEMGR_ModuleErrorReport");
            /*Execute a user-defined error handler*/
            AmbaSEMGR_ModuleErrorReport(ETH_MODULE_ID, Idx, 0U, 0U);
            /*Trigger a system reset if returning from the user-defined error handler (optional)*/
#endif
        }
    }

    return 0;
}

static void EnetW_CreateIPC(void)
{
    EnetIPC_Handle = AmbaIPC_Alloc(RPMSG_DEV_AMBA, "enet", EnetW_MsgHandler);

    if (EnetIPC_Handle == NULL) {
        EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "AmbaIPC_Alloc failed");
    } else {
        (void)AmbaIPC_RegisterChannel(EnetIPC_Handle, NULL);
    }
}

/* Note: caller should get mutex */
static ENETW_Q_t *EnetW_Q_alloc(UINT32 Idx)
{
    ENETW_Q_t *p = pEnetW_Qidle[Idx];

    if (p != NULL) {
        pEnetW_Qidle[Idx] = p->next;
        p->next = NULL;
    } else {
        EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "NULL pEnetW_Qidle");
    }

    return p;
}

/* Note: caller should get mutex */
static void EnetW_Q_free(UINT32 Idx, ENETW_Q_t *p)
{
    if (p != NULL) {
        p->next = pEnetW_Qidle[Idx];
        pEnetW_Qidle[Idx] = p;
    }
}

/* ISR */
static void EnetTimerIsrFunc(UINT32 Idx)
{
    if (EnetW_Inited[Idx] != 0U) {
        (void) AmbaKAL_SemaphoreGive(&EnetW_Sem[Idx]);
    }
}

/**
 * @ingroup enet
 * set current ENET Safe State
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] State 1: enter safe state, 0: leave safe state
 * @return enet error number
 */
UINT32 AmbaSafety_EnetSetSafeState(UINT32 Idx, UINT32 State)
{
    UINT32 Ret = 0U;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else {
        /* code start */
#if defined(CONFIG_CPU_CORTEX_R52)
        /* (optional) notify A53 */
        char msg[32];
        IO_UtilityStringCopy(msg, sizeof(msg), ENETW_PREFIX);
        IO_UtilityStringAppendUInt32(msg, sizeof(msg), Idx, 10U);
        IO_UtilityStringAppend(msg, sizeof(msg), (const char *)"SetSafeState");
        (void)AmbaIPC_Send(EnetIPC_Handle, msg, (INT32)IO_UtilityStringLength(msg) + 1);
#endif
        (void)State;
        AmbaSafety_EnetShadowLock();
        *pEnetSafeState[Idx] = 1U;
        AmbaSafety_EnetShadowUnLock();
        AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "EnetSafeState[%d] = 1", Idx, 0U, 0U, 0U, 0U);
    }

    return Ret;
}

/**
 * @ingroup enet
 * get current ENET Safe State
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] pState 1: enter safe state, 0: leave safe state
 * @return when pState is NULL, return State directly
 */
UINT32 AmbaSafety_EnetGetSafeState(UINT32 Idx, UINT32 *pState)
{
    UINT32 Ret = 0U;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else {
        /* code start */
        AmbaSafety_EnetShadowLock();
        if (pState != NULL) {
            *pState = *pEnetSafeState[Idx];
        } else {
            /* when pState is NULL, return State directly */
            Ret = *pEnetSafeState[Idx];
        }
        AmbaSafety_EnetShadowUnLock();
    }

    return Ret;
}


/**
 * @ingroup enet
 * set current ENET Health State
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] TS timestamp or unique health heartbeat value
 * @return enet error number
 */
static UINT32 AmbaSafety_SetHeartBeat(UINT32 Idx, UINT32 HS)
{
    UINT32 Ret = 0U;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else {
        /* code start */
        AmbaSafety_EnetShadowLock();
        *pEnetHeartBeat[Idx] = HS;
        AmbaSafety_EnetShadowUnLock();
    }

    return Ret;
}

/**
 * @ingroup enet
 * get watch id to identify your monitor object
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @return non-zero watch id
 */
UINT32 AmbaSafety_EnetGetWId(UINT32 Idx, const char *name)
{
    static UINT32 EnetW_Id[ENET_INSTANCES] = {0};
    UINT32 wid;

    (void) AmbaKAL_MutexTake(&EnetW_IdMut[Idx], KAL_WAIT_FOREVER);
    if (EnetW_Id[Idx] == 0U) {
        EnetW_Id[Idx]++;
    }
    wid = EnetW_Id[Idx];
    EnetW_Id[Idx]++;
    (void) AmbaKAL_MutexGive(&EnetW_IdMut[Idx]);
    (void) name;
    EnetDebug("ENET %s(%d) %s #%d ", __func__, __LINE__, name, wid);

    return wid;
}

/**
 * @ingroup enet
 * add monitor object to ethernet watchdog timer
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] wid the watch id to identify your monitor object
 * @return enet error number
 */
UINT32 AmbaSafety_EnetEnq(UINT32 Idx, UINT32 wid)
{
    ENETW_Q_t *p;
    UINT32 Ret = 0U;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else {
        /* code start */
        (void)AmbaSafety_SetHeartBeat(Idx, pAmbaCSL_EnetReg[Idx]->MacR450_TSS);
        (void)AmbaKAL_MutexTake(&EnetW_QMut[Idx], AMBA_KAL_WAIT_FOREVER);
        p = EnetW_Q_alloc(Idx);
        if (p == NULL) {
            EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "NULL EnetW_Q_alloc");
            Ret = ETH_ERR_EINVAL;
        } else {
            p->hp = ENETW_CNT;
            p->id = wid;
            EnetDebug("ENET %s(%d) #%d hp %d", __func__, __LINE__, p->id, p->hp);
            if (pEnetW_Qtl[Idx] != NULL) {
                pEnetW_Qtl[Idx]->next = p;
                p->next = NULL;
                pEnetW_Qtl[Idx] = p;
            } else {
                pEnetW_Qhd[Idx] = p;
                pEnetW_Qtl[Idx] = p;
                p->next = NULL;
            }
        }
        (void)AmbaKAL_MutexGive(&EnetW_QMut[Idx]);
    }

    return Ret;
}


/**
 * @ingroup enet
 * remove monitor object from ethernet watchdog timer
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] wid the watch id to identify your monitor object
 * @return enet error number
 */
UINT32 AmbaSafety_EnetDeq(UINT32 Idx, UINT32 wid)
{
    ENETW_Q_t *cur;
    ENETW_Q_t *pre;
    UINT32 Ret = ETH_ERR_EINVAL;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else {
        /* code start */
        /* set 0 for bus idle */
        (void)AmbaSafety_SetHeartBeat(Idx, 0U);
        (void)AmbaKAL_MutexTake(&EnetW_QMut[Idx], AMBA_KAL_WAIT_FOREVER);
        cur = pEnetW_Qhd[Idx];
        pre = pEnetW_Qhd[Idx];
        if (cur == NULL) {
            EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "NULL pEnetW_Qhd");
        } else {
            for (;;) {
                if ((cur == NULL) || (Ret == 0U)) {
                    break;
                }
                if (cur->id == wid) {
                    EnetDebug("ENET %s(%d) #%d hp %d", __func__, __LINE__, cur->id, cur->hp);
                    if (cur == pEnetW_Qhd[Idx]) {
                        pEnetW_Qhd[Idx] = cur->next;
                    }
                    pre->next = cur->next;
                    if (cur == pEnetW_Qtl[Idx]) {
                        if (pre != cur) {
                            pEnetW_Qtl[Idx] = pre;
                        } else {
                            pEnetW_Qtl[Idx] = NULL;
                        }
                    }
                    EnetW_Q_free(Idx, cur);
                    Ret = 0U;
                    continue;
                } else {
                    pre = cur;
                    cur = cur->next;
                }
            }
        }
        (void)AmbaKAL_MutexGive(&EnetW_QMut[Idx]);
    }

    return Ret;
}

/**
 * @ingroup enet
 * lock ethernet register shadow
 *
 * @return void
 */
void AmbaSafety_EnetShadowLock(void)
{
    if (0U != AmbaIPC_MutexTake(AMBA_IPC_MUTEX_ENET, ENETW_DTTI)) {
        EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "AmbaIPC_MutexTake timeout");
    }
}

/**
 * @ingroup enet
 * lock ethernet register shadow
 *
 * @return void
 */
void AmbaSafety_EnetShadowUnLock(void)
{
    (void)AmbaIPC_MutexGive(AMBA_IPC_MUTEX_ENET);
}

/**
 * @ingroup enet
 * write ethernet register shadow
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] Shadow ENET register shadow offset
 * @param [in] Shadow ENET register shadow value
 * @return void
 */
void AmbaSafety_EnetShadowWrite(volatile UINT32 *pAddr, UINT32 Value)
{
    UINT32 Idx = ENET_INSTANCES, Offset;

    (void)pAddr;
    /*Offset = (UINT32)pAddr;*/
    (void)AmbaWrap_memcpy(&Offset, &pAddr, sizeof(Offset));
    if ((Offset >= EnetReg[0]) && (Offset <= (EnetReg[0] + ENETW_SHADOW_RANGE))) {
        Idx = 0U;
        Offset -= EnetReg[0];
    }
#if (ENET_INSTANCES >= 2U)
    else if ((Offset >= EnetReg[1]) && (Offset <= (EnetReg[1] + ENETW_SHADOW_RANGE))) {
        Idx = 1U;
        Offset -= EnetReg[1];
    } else {
        /* write to descriptor, skip register clone */
        Idx = ENET_INSTANCES;
    }
#endif

    if (Idx < ENET_INSTANCES) {
        UINT8 *addr;
        UINT32 *pShadow;

        AmbaMisra_TypeCast(&addr, &pAmbaCSL_EnetShadow[Idx]);
        addr = &addr[Offset];
        AmbaMisra_TypeCast(&pShadow, &addr);

        *(pShadow) = Value;
        EnetDebug("ENET %s(%d) %p 0x%08x", __func__, __LINE__, pShadow, Value);
    }
}

/**
 * @ingroup enet
 * irq callback function for safety check like flooding detection
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @return void
 */
UINT32 AmbaSafety_EnetIrqHdlr(UINT32 Idx)
{
    const UINT32 irq = *pEnetIrqCnt[Idx] + 1U;
    *pEnetIrqCnt[Idx] = irq;
    return irq;
}

#if defined(CONFIG_CPU_CORTEX_R52)
/**
 * @ingroup enet
 * get current ENET Health State
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @return current unique health heartbeat
 */
static UINT32 AmbaSafety_GetHeartBeat(UINT32 Idx)
{
    UINT32 Ret = 0U;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else {
        /* code start */
        AmbaSafety_EnetShadowLock();
        Ret = *pEnetHeartBeat[Idx];
        AmbaSafety_EnetShadowUnLock();
    }

    return Ret;
}

static void AmbaSafety_EnetR52Check(UINT32 Idx)
{
    const AMBA_ENET_REG_s *reg = pAmbaCSL_EnetReg[Idx];
    const AMBA_ENET_REG_s *shadow = pAmbaCSL_EnetShadow[Idx];
    UINT32 DoErrorReport = 0U;
    UINT32 r1, r2, State;
    static UINT32 LastBeat[2] = { 0U, 0U };
    static UINT32 hp[2] = { ENETW_CNT, ENETW_CNT };
    static UINT32 LastIrqCnt[2] = { 0U, 0U };

    /* check IPC channel */
    if (EnetIPC_Handle == NULL) {
        EnetW_CreateIPC();
    }

    /* detect irq flooding */
    r1 = *pEnetIrqCnt[Idx];
    if ((r1 > LastIrqCnt[Idx]) &&
        (r1 > (LastIrqCnt[Idx] + ENETW_IRQTH))) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "irq flooding LastIrqCnt[%d] %d -> irq %d", Idx, LastIrqCnt[Idx], r1, 0U, 0U);
        DoErrorReport += 1U;
    }
    LastIrqCnt[Idx] = r1;

    /* are you dead? check heartbeat */
    r1 = AmbaSafety_GetHeartBeat(Idx);
    if (0U == r1) {
        //EnetDebug("ENET %s(%d) bus idle", __func__, __LINE__);
        hp[Idx] = ENETW_CNT;
    } else if (r1 != LastBeat[Idx]) {
        EnetDebug("ENET %s(%d) bus alive", __func__, __LINE__);
        hp[Idx] = ENETW_CNT;
    } else {
        if (0U == hp[Idx]) {
            AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "enet heartbeat[%d] 0x%x not updating for %d ms ", Idx, LastBeat[Idx], ENETW_TO, 0U, 0U);
            DoErrorReport += 2U;
            hp[Idx] = ENETW_CNT;
        } else {
            hp[Idx]--;
            EnetDebug("ENET %s(%d) hp[%d] %d", __func__, __LINE__, Idx, hp[Idx]);
        }
    }
    LastBeat[Idx] = r1;

    /*
     * Shadow Compare
     */
    (void)AmbaSafety_EnetGetSafeState(Idx, &State);
    if (State == 0U) {
        if (AmbaIPC_MutexTake(AMBA_IPC_MUTEX_ENET, ENETW_DTTI) == 0U) {
            /* lock for ifup & ifdown */
            r1 = (UINT32)reg->MacR0.Data;
            r2 = (UINT32)shadow->MacR0.Data;
            if (r1 != r2) {
                AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "MacR0 reg 0x%x != shadow 0x%x", reg->MacR0.Data, shadow->MacR0.Data, 0U, 0U, 0U);
                DoErrorReport += 4U;
            }

            r1 = (UINT32)reg->MacR1.Data;
            r2 = (UINT32)shadow->MacR1.Data;
            if (r1 != r2) {
                AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID,"MacR1 reg 0x%x != shadow 0x%x", reg->MacR1.Data, shadow->MacR1.Data, 0U, 0U, 0U);
                DoErrorReport += 4U;
            }

            /* lock for multicast filter */
            r1 = (UINT32)reg->MacR2_HTH;
            r2 = (UINT32)shadow->MacR2_HTH;
            if (r1 != r2) {
                AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID,"MacR2_HTH reg 0x%x != shadow 0x%x", reg->MacR2_HTH, shadow->MacR2_HTH, 0U, 0U, 0U);
                DoErrorReport += 4U;
            }

            /* lock for multicast filter */
            r1 = (UINT32)reg->MacR3_HTL;
            r2 = (UINT32)shadow->MacR3_HTL;
            if (r1 != r2) {
                AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID,"MacR3_HTL reg 0x%x != shadow 0x%x", reg->MacR3_HTL, shadow->MacR3_HTL, 0U, 0U, 0U);
                DoErrorReport += 4U;
            }

            r1 = (UINT32)reg->MacR6.Data;
            r2 = (UINT32)shadow->MacR6.Data;
            if (r1 != r2) {
                AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID,"MacR6 reg 0x%x != shadow 0x%x", reg->MacR6.Data, shadow->MacR6.Data, 0U, 0U, 0U);
                DoErrorReport += 4U;
            }

            r1 = (UINT32)reg->MacR8_VER;
            r2 = (UINT32)shadow->MacR8_VER;
            if (r1 != r2) {
                AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID,"MacR8_VER reg 0x%x != shadow 0x%x", reg->MacR8_VER, shadow->MacR8_VER, 0U, 0U, 0U);
                DoErrorReport += 4U;
            }

            /* skip readonly b[16:31] */
            r1 = (UINT32)(reg->MacR16_ADDRHI & 0xffffU);
            r2 = (UINT32)(shadow->MacR16_ADDRHI & 0xffffU);
            if (r1 != r2) {
                AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID,"MacR16_ADDRHI reg 0x%x != shadow 0x%x", reg->MacR16_ADDRHI, shadow->MacR16_ADDRHI, 0U, 0U, 0U);
                DoErrorReport += 4U;
            }

            r1 = (UINT32)reg->MacR17_ADDRLO;
            r2 = (UINT32)shadow->MacR17_ADDRLO;
            if (r1 != r2) {
                AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID,"MacR17_ADDRLO reg 0x%x != shadow 0x%x", reg->MacR17_ADDRLO, shadow->MacR17_ADDRLO, 0U, 0U, 0U);
                DoErrorReport += 4U;
            }

            (void)AmbaIPC_MutexGive(AMBA_IPC_MUTEX_ENET);
        } else {
            EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "AmbaIPC_MutexTake timeout");
            DoErrorReport += 8U;
        }
    }

    if (DoErrorReport != 0U) {
        EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "AmbaSEMGR_ModuleErrorReport");
        AmbaSEMGR_ModuleErrorReport(ETH_MODULE_ID, Idx, DoErrorReport, 0U);
        /*Trigger a system reset if returning from the user-defined error handler (optional)*/
    }
}
#else
static void AmbaSafety_EnetA53Check(UINT32 Idx)
{
    char msg[32];
    ENETW_Q_t *cur;
    ENETW_Q_t *p;
    UINT32 State;

    /* check IPC channel */
    if (EnetIPC_Handle == NULL) {
        EnetW_CreateIPC();
    }

    /* enter safe state */
    (void)AmbaSafety_EnetGetSafeState(Idx, &State);
    if (State != 0U) {
        if (pAmbaCSL_EnetReg[Idx]->MacR0.Bits.TE == 1U) {
            (void)AmbaEnet_IfDown(Idx);
            AmbaRTSL_EnetINTDisable(Idx);
        }
    }

    /*
     * check watch queue
     */
    /* count down */
    (void)AmbaKAL_MutexTake(&EnetW_QMut[Idx], AMBA_KAL_WAIT_FOREVER);

    cur = pEnetW_Qhd[Idx];
    for (;;) {
        if (cur == NULL) {
            break;
        }
        if (cur->hp > 0u) {
            cur->hp--;
        }
        cur = cur->next;
    }
    cur = pEnetW_Qhd[Idx];
    (void)AmbaKAL_MutexGive(&EnetW_QMut[Idx]);

    /* remove timed out items */
    for (;;) {
        if (cur == NULL) {
            break;
        }
        if (cur->hp == 0U) {
            /* notify R52 CEHU */
            EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "notify R52 CEHU");
            IO_UtilityStringCopy(msg, sizeof(msg), ENETW_PREFIX);
            IO_UtilityStringAppendUInt32(msg, sizeof(msg), Idx, 10U);
            IO_UtilityStringAppend(msg, sizeof(msg), (const char *)"SetSafeState");
            (void)AmbaIPC_Send(EnetIPC_Handle, msg, (INT32)IO_UtilityStringLength(msg) + 1);
            p = cur->next;
            if (0U != AmbaSafety_EnetDeq(Idx, cur->id)) {
                EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "AmbaSafety_EnetDeq failed");
            }
            cur = p;
        } else {
            cur = cur->next;
        }
    }
}
#endif

/**
 * @ingroup enet
 * ethernet safety task entry
 *
 * @param [in] arg ENET controller index on AMBA SoC, value start from 0
 * @return void
 */
void *EnetSafetyTaskEntry(void *arg)
{
    UINT32 Idx;
    UINT32 loop = 1U;

    /*Idx = (UINT32)arg;*/
    (void)AmbaWrap_memcpy(&Idx, &arg, sizeof(Idx));

    while (EnetW_Inited[Idx] == 0U) {
        (void) AmbaKAL_TaskSleep(1);
    }

    EnetDebug("ENET %s(%d) start ", __func__, __LINE__);
    while (loop == 1U) {
        (void) AmbaKAL_SemaphoreTake(&EnetW_Sem[Idx], KAL_WAIT_FOREVER);
        if (pAmbaCSL_EnetShadow[Idx] != NULL) {
#if defined(CONFIG_CPU_CORTEX_R52)
            AmbaSafety_EnetR52Check(Idx);
#else
            AmbaSafety_EnetA53Check(Idx) ;
#endif
        }
        (void) AmbaKAL_TaskSleep(1);
    }  //loop end

    return NULL;
}

/**
 * @ingroup enet
 * init ethernet safety feature
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @return enet error number
 */
UINT32 AmbaSafety_EnetInit(UINT32 Idx)
{
    UINT32 i;
    UINT32 Ret = 0U;
    static AMBA_KAL_TIMER_t EnetTimer[ENET_INSTANCES] = {0};
    static ENETW_Q_t EnetW_Q[ENETW_QSIZE];
    static char EnetTimerName[2][16] = { "EnetW0", "EnetW1" };
    static char EnetSemName[2][16] = { "EnetWSem0", "EnetWSem1" };
    UINT32 Base = 0U;
    volatile const UINT32 *pBase;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else if(EnetW_Inited[Idx] != 0U) {
        EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Already init!");
        Ret = ETH_ERR_EINVAL;
    } else {
        /* code start */
        if (EnetW_QMut[Idx].tx_mutex_id == 0U) {
            if (0U != AmbaKAL_MutexCreate(&EnetW_QMut[Idx], NULL)) {
                EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "AmbaKAL_MutexCreate failed");
                Ret = ETH_ERR_OSERR;
            }
        }
        if (EnetW_IdMut[Idx].tx_mutex_id == 0U) {
            if (0U != AmbaKAL_MutexCreate(&EnetW_IdMut[Idx], NULL)) {
                EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "AmbaKAL_MutexCreate failed");
                Ret = ETH_ERR_OSERR;
            }
        }
        if (EnetW_Sem[Idx].tx_semaphore_id == 0U) {
            if (OK != AmbaKAL_SemaphoreCreate(&EnetW_Sem[Idx], EnetSemName[Idx], 0U)) {
                EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "AmbaKAL_SemCreate failed!");
                Ret = ETH_ERR_OSERR;
            }
        }
        (void)AmbaWrap_memset(EnetW_Q, 0, sizeof(EnetW_Q));
        for (i = 0U; i < (ENETW_QSIZE - 1U); i++) {
            EnetW_Q[i].next = &(EnetW_Q[i+1U]);
        }
        EnetW_Q[i].next = NULL;
        pEnetW_Qidle[Idx] = EnetW_Q;

#if defined(CONFIG_CPU_CORTEX_R52)
        if (Idx == 0U) {
            Base = EnetReg[Idx];
        } else {
            if (Idx == 1U) {
                Base = EnetReg[Idx];
            }
        }
        /*pAmbaCSL_EnetReg[Idx] = (AMBA_ENET_REG_s *)Base;*/
        pAmbaCSL_EnetReg[Idx] = NULL;
        (void)AmbaWrap_memcpy(&pAmbaCSL_EnetReg[Idx], &Base, sizeof(Base));
#endif
        if (Idx == 0U) {
            Base = (UINT32)SHADOWBASE_ENET0;
        } else {
            if (Idx == 1U) {
                Base = (UINT32)SHADOWBASE_ENET1;
            }
        }
        /*pAmbaCSL_EnetShadow[Idx] = (AMBA_ENET_REG_s *)Base;*/
        pAmbaCSL_EnetShadow[Idx] = NULL;
        (void)AmbaWrap_memcpy(&pAmbaCSL_EnetShadow[Idx], &Base, sizeof(Base));
        AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "ENET register shadow 0x%08x", Base, 0U, 0U, 0U, 0U);

        /* 0x00E4 Reserved57_58[2] is used as heartbeat */
        pBase = &pAmbaCSL_EnetShadow[Idx]->Reserved57_58[Idx];
        AmbaMisra_TypeCast(&pEnetHeartBeat[Idx], &pBase);

        /* 0x00EC Reserved59_60[2] is used as safe state */
        pBase = &pAmbaCSL_EnetShadow[Idx]->Reserved59_60[Idx];
        AmbaMisra_TypeCast(&pEnetSafeState[Idx], &pBase);

        /* 0x00F4 Reserved61_62[2] is used as safe state */
        pBase = &pAmbaCSL_EnetShadow[Idx]->Reserved61_62[Idx];
        AmbaMisra_TypeCast(&pEnetIrqCnt[Idx], &pBase);

        /* do initial shadow clone */
        (void)AmbaWrap_memcpy(pAmbaCSL_EnetShadow[Idx], pAmbaCSL_EnetReg[Idx], sizeof(AMBA_ENET_REG_s));

        /* start WatchDog Timer */
        if (EnetTimer[Idx].tx_timer_id == 0U) {
            if (0U != AmbaKAL_TimerCreate(&EnetTimer[Idx], EnetTimerName[Idx], EnetTimerIsrFunc, Idx,
                                          ENETW_DTTI, ENETW_DTTI, 1U)) {
                EnetW_Print(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "AmbaKAL_TimerCreate failed");
                Ret = ETH_ERR_OSERR;
            }
        }
        if (Ret == 0U) {
            EnetW_Inited[Idx] = 1U;
        }
    }

    return Ret;
}
