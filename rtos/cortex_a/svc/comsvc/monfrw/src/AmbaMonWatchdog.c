/**
 *  @file AmbaMonWatchdog.c
 *
 *  Copyright (c) [2020] Ambarella International LP
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
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details Amba Monitor Watchdog
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaMonVar.h"

#include "AmbaMonWatchdog.h"
#include "AmbaMonWatchdog_Internal.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

static AMBA_MON_WATCHDOG_s AmbaMonWatchdog;
static AMBA_MON_WATCHDOG_PORT_s EndlessPort;

/**
 *  Amba monitor watchdog init
 *  @return error code
 */
UINT32 AmbaMonWatchdog_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* mutex */
    FuncRetCode = AmbaKAL_MutexCreate(&(AmbaMonWatchdog.Mutex), NULL);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    /* semaphore */
    FuncRetCode = AmbaKAL_SemaphoreCreate(&(AmbaMonWatchdog.Sem), NULL, 0);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    /* id list header */
    AmbaMonWatchdog.pIdList = NULL;

    /* create endless port */
    FuncRetCode = AmbaKAL_MutexCreate(&(EndlessPort.Mutex), NULL);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* open endless prot */
        EndlessPort.Magic = 0xCafeU;
        EndlessPort.Period = 0xFFFFFFFFU;
        EndlessPort.Time2Wait = 0xFFFFFFFFU;
        EndlessPort.pUser = NULL;
        EndlessPort.pWdogId = NULL;
        EndlessPort.WaitLink.Up = NULL;
        EndlessPort.WaitLink.Down = NULL;
        EndlessPort.Link.Up = NULL;
        EndlessPort.Link.Down = NULL;
        /* hook to wait list */
        AmbaMonWatchdog.pWaitList = &EndlessPort;
        /* hook to wait tail */
        AmbaMonWatchdog.pWaitTail = &EndlessPort;
    } else {
        /* null wait list */
        AmbaMonWatchdog.pWaitList = NULL;
        /* null wait tail */
        AmbaMonWatchdog.pWaitTail = NULL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor watchdog create
 *  @param[in] pWdogId pointer to the watchdog obj id
 *  @param[in] pName pointer to the watchdog obj string
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonWatchdog_Create(AMBA_MON_WATCHDOG_ID_s *pWdogId, const char *pName)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_WATCHDOG_ID_s *pIdList;

    if ((pWdogId != NULL) &&
        (pWdogId->Magic != 0xCafeU) &&
        (pName != NULL)) {
        /* create mutex */
        FuncRetCode = AmbaKAL_MutexCreate(&(pWdogId->Mutex), NULL);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* magic (valid), name */
            pWdogId->Magic = 0xCafeU;
            pWdogId->pName = pName;

            /* port list init */
            pWdogId->pPortList = NULL;

            /* take watchdog list */
            FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonWatchdog.Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* hook to watchdog list */
                if (AmbaMonWatchdog.pIdList == NULL) {
                    /* root */
                    pWdogId->Link.Up = NULL;
                    pWdogId->Link.Down = NULL;
                    AmbaMonWatchdog.pIdList = pWdogId;
                } else {
                    /* last */
                    pIdList = AmbaMonWatchdog.pIdList;
                    while (pIdList->Link.Down != NULL) {
                        pIdList = pIdList->Link.Down;
                    }
                    pWdogId->Link.Up = pIdList;
                    pWdogId->Link.Down = NULL;
                    pIdList->Link.Down = pWdogId;
                }

                /* give watchdog list */
                FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonWatchdog.Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }
        }
    } else {
        /* wdog null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor watchdog reset
 *  @param[in] pWdogId pointer to the watchdog obj id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonWatchdog_Reset(AMBA_MON_WATCHDOG_ID_s *pWdogId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_WATCHDOG_PORT_s *pPortList;

    const AMBA_MON_WATCHDOG_PORT_s *pWaitList;
    const AMBA_MON_WATCHDOG_PORT_s *pWaitTail;

    if ((pWdogId != NULL) &&
        (pWdogId->Magic == 0xCafeU)) {
        /* take watchdog list */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonWatchdog.Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take watchdog id */
            FuncRetCode = AmbaKAL_MutexTake(&(pWdogId->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                pPortList = pWdogId->pPortList;
                while (pPortList != NULL) {
                    if (pPortList->Magic == 0xCafeU) {
                        FuncRetCode = AmbaKAL_MutexTake(&(pPortList->Mutex), AMBA_KAL_WAIT_FOREVER);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* get wait list */
                            pWaitList = AmbaMonWatchdog.pWaitList;
                            /* root? */
                            if (pWaitList == pPortList) {
                                /* stop sem timeout timer, enable refresh */
                                FuncRetCode = AmbaKAL_SemaphoreGive(&(AmbaMonWatchdog.Sem));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            }
                            /* remove from wait list */
                            pWaitList = AmbaMonWatchdog.pWaitList;
                            if (pWaitList != NULL) {
                                /* up link check */
                                if (pPortList->WaitLink.Up == NULL) {
                                    /* root? */
                                    if (pWaitList == pPortList) {
                                        /* root */
                                        AmbaMonWatchdog.pWaitList = pPortList->WaitLink.Down;
                                    }
                                } else {
                                    /* linker */
                                    pPortList->WaitLink.Up->WaitLink.Down = pPortList->WaitLink.Down;
                                }
                                /* down link check */
                                if (pPortList->WaitLink.Down != NULL) {
                                    /* root and linker */
                                    pPortList->WaitLink.Down->WaitLink.Up = pPortList->WaitLink.Up;
                                }
                            }
                            /* tail check for wait list */
                            pWaitTail = AmbaMonWatchdog.pWaitTail;
                            if (pWaitTail != NULL) {
                                if (pWaitTail == pPortList) {
                                    AmbaMonWatchdog.pWaitTail = pPortList->WaitLink.Up;
                                }
                            }
                            /* port settings clear */
                            pPortList->Period = 0U;
                            pPortList->Time2Wait = 0U;
                        }

                        /* give watchdog port */
                        FuncRetCode = AmbaKAL_MutexGive(&(pPortList->Mutex));
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                    }
                    pPortList = pPortList->Link.Down;
                }

                /* give watchdog id */
                FuncRetCode = AmbaKAL_MutexGive(&(pWdogId->Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }

            /* give watchdog list */
            FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonWatchdog.Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* wdog null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor watchdog delete
 *  @param[in] pWdogId pointer to the watchdog obj id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonWatchdog_Delete(AMBA_MON_WATCHDOG_ID_s *pWdogId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pWdogId != NULL) &&
        (pWdogId->Magic == 0xCafeU) &&
        (pWdogId->pPortList == NULL)) {
        /* take watchdog list */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonWatchdog.Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take watchdog id */
            FuncRetCode = AmbaKAL_MutexTake(&(pWdogId->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* remove from watchdog list */
                if(AmbaMonWatchdog.pIdList != NULL) {
                    /* up link */
                    if (pWdogId->Link.Up == NULL) {
                        /* root */
                        AmbaMonWatchdog.pIdList = pWdogId->Link.Down;
                    } else {
                        /* linker */
                        pWdogId->Link.Up->Link.Down = pWdogId->Link.Down;
                    }
                    /* down link */
                    if (pWdogId->Link.Down != NULL) {
                        /* root/linker */
                        pWdogId->Link.Down->Link.Up = pWdogId->Link.Up;
                    }
                    /* magic (invalid), name */
                    pWdogId->Magic = 0U;
                    pWdogId->pName = NULL;

                    /* give watchdog id */
                    FuncRetCode = AmbaKAL_MutexGive(&(pWdogId->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* watchdog id mutex delete */
                    FuncRetCode = AmbaKAL_MutexDelete(&(pWdogId->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                } else {
                    RetCode = NG_UL;
                    /* give watchdog id */
                    FuncRetCode = AmbaKAL_MutexGive(&(pWdogId->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }
            }

            /* give watchdog list */
            FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonWatchdog.Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* wdog null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor watchdog open
 *  @param[in] pPort pointer to the watchdog port
 *  @param[in] pName pointer to the watchdog obj string
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonWatchdog_Open(AMBA_MON_WATCHDOG_PORT_s *pPort, const char *pName)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_WATCHDOG_ID_s *pIdList;
    AMBA_MON_WATCHDOG_PORT_s *pPortList;

    if ((pPort != NULL) &&
        (pPort->Magic != 0xCafeU) &&
        (pName != NULL)) {
        /* take watchdog list */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonWatchdog.Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* watchdog id find */
            if (AmbaMonWatchdog.pIdList != NULL) {
                /* search by name */
                pIdList = AmbaMonWatchdog.pIdList;
                do {
                    FuncRetCode = svar_strcmp(pName, pIdList->pName);
                    if (FuncRetCode == OK_UL) {
                        /* found */
                        break;
                    }
                    /* next */
                    pIdList = pIdList->Link.Down;
                } while (pIdList != NULL);

                if (pIdList != NULL) {
                    /* port mutex */
                    FuncRetCode = AmbaKAL_MutexCreate(&(pPort->Mutex), NULL);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* port settings */
                        pPort->Magic = 0xCafeU;
                        pPort->pWdogId = pIdList;
                        pPort->Link.Up = NULL;
                        pPort->Link.Down = NULL;

                        /* watchdog period & time2wait */
                        pPort->Period = 0U;
                        pPort->Time2Wait = 0U;

                        /* watchdog wait link */
                        pPort->WaitLink.Up = NULL;
                        pPort->WaitLink.Down = NULL;

                        /* take watchdog id */
                        FuncRetCode = AmbaKAL_MutexTake(&(pIdList->Mutex), AMBA_KAL_WAIT_FOREVER);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* hook port to watchdog */
                            pPortList = pPort->pWdogId->pPortList;
                            if (pPortList == NULL) {
                                /* root */
                                pPort->pWdogId->pPortList = pPort;
                            } else {
                                /* last */
                                while (pPortList->Link.Down != NULL) {
                                    pPortList = pPortList->Link.Down;
                                }
                                pPort->Link.Up = pPortList;
                                pPortList->Link.Down = pPort;
                            }

                            /* give watchdog id */
                            FuncRetCode = AmbaKAL_MutexGive(&(pIdList->Mutex));
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
                        }
                    }
                } else {
                    RetCode = NG_UL;
                }
            } else {
                RetCode = NG_UL;
            }

            /* give watchdog list */
            FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonWatchdog.Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* wdog null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor watchdog refresh
 *  @param[in] pPort pointer to the watchdog port
 *  @param[in] Period period time (ms)
 *  @param[in] pUser Pointer to the user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonWatchdog_Refresh(AMBA_MON_WATCHDOG_PORT_s *pPort, UINT32 Period, void *pUser)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 WatchdogStop = 0U;

    AMBA_MON_WATCHDOG_PORT_s *pWaitList;
    AMBA_MON_WATCHDOG_PORT_s *pWaitTail;

    UINT32 Timetick;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pPort->pWdogId != NULL)) {
        /* take watchdog list */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonWatchdog.Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take watchdog id */
            FuncRetCode = AmbaKAL_MutexTake(&(pPort->pWdogId->Mutex), AMBA_KAL_WAIT_FOREVER);
            if(FuncRetCode == KAL_ERR_NONE) {
                /* take watchdog port */
                FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* get wait list */
                    pWaitList = AmbaMonWatchdog.pWaitList;
                    /* root? */
                    if (pWaitList == pPort) {
                        /* give sem to refresh sem timer */
                        FuncRetCode = AmbaKAL_SemaphoreGive(&(AmbaMonWatchdog.Sem));
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                        WatchdogStop = 1U;
                    }

                    /* remove from wait list */
                    if (pWaitList != NULL) {
                        /* up link check */
                        if (pPort->WaitLink.Up == NULL) {
                            /* root? */
                            if (pWaitList == pPort) {
                                /* root */
                                AmbaMonWatchdog.pWaitList = pPort->WaitLink.Down;
                            }
                        } else {
                            /* linker */
                            pPort->WaitLink.Up->WaitLink.Down = pPort->WaitLink.Down;
                        }
                        /* down link check */
                        if (pPort->WaitLink.Down != NULL) {
                            /* root and linker */
                            pPort->WaitLink.Down->WaitLink.Up = pPort->WaitLink.Up;
                        }
                    }

                    /* get wait tail */
                    pWaitTail = AmbaMonWatchdog.pWaitTail;
                    if (pWaitTail != NULL) {
                        /* tail? */
                        if (pWaitTail == pPort) {
                            AmbaMonWatchdog.pWaitTail = pPort->WaitLink.Up;
                        }
                    }

                    /* refresh port and re-hook to wait list */
                    if (Period != 0U) {
                        /* get current sys timetick */
                        FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* calculate time to wait */
                            pPort->Period = Period;
                            pPort->Time2Wait = Timetick + Period;
                            /* find the position */
                            pWaitTail = AmbaMonWatchdog.pWaitTail;
                            while (pWaitTail != NULL) {
                                if (pPort->Time2Wait >= pWaitTail->Time2Wait) {
                                    break;
                                }
                                pWaitTail = pWaitTail->WaitLink.Up;
                            }
                            /* insert port to wait list */
                            if (pWaitTail == NULL) {
                                /* root */
                                pWaitList = AmbaMonWatchdog.pWaitList;
                                /* refine link */
                                if (pWaitList == NULL) {
                                    pPort->WaitLink.Up = NULL;
                                    pPort->WaitLink.Down = NULL;
                                    /* also last */
                                    AmbaMonWatchdog.pWaitTail = pPort;
                                } else {
                                    pPort->WaitLink.Down = pWaitList;
                                    pWaitList->WaitLink.Up = pPort;
                                    pPort->WaitLink.Up = NULL;
                                }
                                /* attach root */
                                AmbaMonWatchdog.pWaitList = pPort;
                                /* give sem to refresh sem timer */
                                if (WatchdogStop == 0U) {
                                    FuncRetCode = AmbaKAL_SemaphoreGive(&(AmbaMonWatchdog.Sem));
                                    if (FuncRetCode != KAL_ERR_NONE) {
                                        /* */
                                    }
                                }
                            } else {
                                /* insert */
                                pPort->WaitLink.Down = pWaitTail->WaitLink.Down;
                                if (pWaitTail->WaitLink.Down != NULL) {
                                    pWaitTail->WaitLink.Down->WaitLink.Up = pPort;
                                }
                                pWaitTail->WaitLink.Down = pPort;
                                pPort->WaitLink.Up = pWaitTail;
                                /* last? */
                                if(AmbaMonWatchdog.pWaitTail == pWaitTail) {
                                    AmbaMonWatchdog.pWaitTail = pPort;
                                }
                            }
                        } else {
                            /* time tick fail */
                            RetCode = NG_UL;
                            /* disable */
                            pPort->Period = 0U;
                            pPort->Time2Wait = 0U;
                            pPort->WaitLink.Up = NULL;
                            pPort->WaitLink.Down = NULL;
                        }
                    } else {
                        /* disable */
                        pPort->Period = 0U;
                        pPort->Time2Wait = 0U;
                        pPort->WaitLink.Up = NULL;
                        pPort->WaitLink.Down = NULL;
                    }

                    /* attach pUser */
                    pPort->pUser = pUser;

                    /* give watchdog port */
                    FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }

                /* give watchdog id */
                FuncRetCode = AmbaKAL_MutexGive(&(pPort->pWdogId->Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }

            /* give watchdog list */
            FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonWatchdog.Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* port null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor watchdog close
 *  @param[in] pPort pointer to the watchdog port
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonWatchdog_Close(AMBA_MON_WATCHDOG_PORT_s *pPort)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    const AMBA_MON_WATCHDOG_PORT_s *pWaitList;
    const AMBA_MON_WATCHDOG_PORT_s *pWaitTail;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pPort->pWdogId != NULL)) {
        /* take watchdog list */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonWatchdog.Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take watchdog id */
            FuncRetCode = AmbaKAL_MutexTake(&(pPort->pWdogId->Mutex), AMBA_KAL_WAIT_FOREVER);
            if(FuncRetCode == KAL_ERR_NONE) {
                /* take watchdog port */
                FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* get wait list */
                    pWaitList = AmbaMonWatchdog.pWaitList;
                    /* root? */
                    if (pWaitList == pPort) {
                        /* stop sem timeout timer, enable refresh */
                        FuncRetCode = AmbaKAL_SemaphoreGive(&(AmbaMonWatchdog.Sem));
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                    }

                    /* remove from wait list */
                    if (pWaitList != NULL) {
                        /* up link check */
                        if (pPort->WaitLink.Up == NULL) {
                            /* root? */
                            if (pWaitList == pPort) {
                                /* root */
                                AmbaMonWatchdog.pWaitList = pPort->WaitLink.Down;
                            }
                        } else {
                            /* linker */
                            pPort->WaitLink.Up->WaitLink.Down = pPort->WaitLink.Down;
                        }
                        /* down link check */
                        if (pPort->WaitLink.Down != NULL) {
                            /* root and linker */
                            pPort->WaitLink.Down->WaitLink.Up = pPort->WaitLink.Up;
                        }
                    }

                    /* get wait tail */
                    pWaitTail = AmbaMonWatchdog.pWaitTail;
                    if (pWaitTail != NULL) {
                        /* tail? */
                        if (pWaitTail == pPort) {
                            AmbaMonWatchdog.pWaitTail = pPort->WaitLink.Up;
                        }
                    }

                    /* remove from port list */
                    if (pPort->pWdogId->pPortList != NULL) {
                        /* up link check */
                        if (pPort->Link.Up == NULL) {
                            /* root */
                            pPort->pWdogId->pPortList = pPort->Link.Down;
                        } else {
                            /* linker */
                            pPort->Link.Up->Link.Down = pPort->Link.Down;
                        }
                        /* down link check */
                        if (pPort->Link.Down != NULL) {
                            /* root and linker */
                            pPort->Link.Down->Link.Up =  pPort->Link.Up;
                        }
                        /* clear magic */
                        pPort->Magic = 0;

                        /* give watchdog port */
                        FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                        /* delete watchdog port mutex */
                        FuncRetCode = AmbaKAL_MutexDelete(&(pPort->Mutex));
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                    } else {
                        RetCode = NG_UL;
                        /* give watchdog port */
                        FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                    }
                }

                /* give watchdog id */
                FuncRetCode = AmbaKAL_MutexGive(&(pPort->pWdogId->Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }

            /* give watchdog list */
            FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonWatchdog.Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* port null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor watchdog timeout get
 *  @param[out] pTimeoutPort pointer to the watchdog port pointer
 *  @param[out] pTimeoutPeriod pointer to the timeout period (ms)
 *  @param[out] pTimeoutUser pointer to the user data
 *  @return error code
 */
UINT32 AmbaMonWatchdog_TimeoutGet(AMBA_MON_WATCHDOG_PORT_s **pTimeoutPort, UINT32 *pTimeoutPeriod, void **pTimeoutUser)
{
    UINT32 RetCode = NG_UL;
    UINT32 FuncRetCode;

    UINT32 Period = 0U;
    UINT32 Time2Wait = 0U;
    UINT32 Timetick;

    AMBA_MON_WATCHDOG_PORT_s *pPort = NULL;
    void *pUser = NULL;

    /* take watchdog list */
    FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonWatchdog.Mutex), AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* semaphore clear */
        do {
            FuncRetCode = AmbaKAL_SemaphoreTake(&(AmbaMonWatchdog.Sem), AMBA_KAL_NO_WAIT);
        } while (FuncRetCode == KAL_ERR_NONE);

        /* get root from wait list */
        pPort = AmbaMonWatchdog.pWaitList;
        if ((pPort != NULL) && (pPort->Magic == 0xCafeU)) {
            Period = pPort->Period;
            Time2Wait = pPort->Time2Wait;
            pUser = pPort->pUser;
        } else {
            Period = 0U;
            Time2Wait = 0U;
            pUser = NULL;
        }

        /* give watchdog list */
        FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonWatchdog.Mutex));
        if (FuncRetCode != KAL_ERR_NONE) {
            /* */
        }
    }

    if (Period > 0U) {
        if (Time2Wait == 0xFFFFFFFFU) {
            /* endless wait */
            FuncRetCode = AmbaKAL_SemaphoreTake(&(AmbaMonWatchdog.Sem), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                /* timeout? (never enter) */
                RetCode = OK_UL;
            }
        } else {
            /* get current timetick */
            FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* sem timeout wait */
                if (Time2Wait > Timetick) {
                    /* wait for refresh */
                    FuncRetCode = AmbaKAL_SemaphoreTake(&(AmbaMonWatchdog.Sem), (Time2Wait - Timetick));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* timeout */
                        RetCode = OK_UL;
                    }
                } else {
                    /* immediately timeout */
                    RetCode = OK_UL;
                }
            } else {
                /* immediately timeout */
                RetCode = OK_UL;
            }
        }
    }

    if (pPort == &EndlessPort) {
        /* do nothing for endless port */
        *pTimeoutPort = NULL;
        *pTimeoutPeriod = 0xFFFFFFFFU;
        *pTimeoutUser = NULL;
    } else {
        /* timeout or refresh port */
        *pTimeoutPort = pPort;
        *pTimeoutPeriod = Period;
        *pTimeoutUser = pUser;
    }

    return RetCode;
}

/**
 *  Amba monitor watchdog timeout restart
 *  @return error code
 */
UINT32 AmbaMonWatchdog_TimeoutRestart(void)
{
    UINT32 RetCode = NG_UL;
    UINT32 FuncRetCode;

    /* take watchdog list */
    FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonWatchdog.Mutex), AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* stop sem timeout timer */
        FuncRetCode = AmbaKAL_SemaphoreGive(&(AmbaMonWatchdog.Sem));
        if (FuncRetCode != KAL_ERR_NONE) {
            /* */
        }
        /* give watchdog list */
        FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonWatchdog.Mutex));
        if (FuncRetCode != KAL_ERR_NONE) {
            /* */
        }
        /* return code */
        RetCode = OK_UL;
    }

    return RetCode;
}
