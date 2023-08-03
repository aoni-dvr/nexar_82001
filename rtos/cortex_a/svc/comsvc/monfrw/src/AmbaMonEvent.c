/**
 *  @file AmbaMonEvent.c
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
 *  @details Amba Monitor Event
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaMonVar.h"
#include "AmbaMonEvent.h"
#include "AmbaMonEvent_Internal.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

static AMBA_MON_EVENT_s AmbaMonEvent;

/**
 *  Amba monitor event init
 *  @return error code
 */
UINT32 AmbaMonEvent_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* mutex */
    FuncRetCode = AmbaKAL_MutexCreate(&(AmbaMonEvent.Mutex), NULL);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    /* list header */
    AmbaMonEvent.pIdList = NULL;

    return RetCode;
}

/**
 *  @private
 *  Amba monitor event obj create
 *  @param[in] pEventId pointer to the event obj id
 *  @param[in] pName pointer to the event obj string
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonEvent_Create(AMBA_MON_EVENT_ID_s *pEventId, const char *pName)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_EVENT_ID_s *pIdList;

    if ((pEventId != NULL) &&
        (pEventId->Magic != 0xCafeU) &&
        (pName != NULL)) {
        /* create mutex */
        FuncRetCode = AmbaKAL_MutexCreate(&(pEventId->Mutex), NULL);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* magic (valid), name */
            pEventId->Magic = 0xCafeU;
            pEventId->pName = pName;

            /* flags reset */
            pEventId->Flags.BitAlloc = 0ULL;
            for (UINT32 i = 0U; i < 64U; i++) {
                pEventId->Flags.BitCnt[i] = 0U;
                pEventId->Flags.pNameTable[i] = NULL;
            }

            /* port list init */
            pEventId->pPortList = NULL;

            /* take event list */
            FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonEvent.Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* hook to event list */
                if (AmbaMonEvent.pIdList == NULL) {
                    /* root */
                    pEventId->Link.Up = NULL;
                    pEventId->Link.Down = NULL;
                    AmbaMonEvent.pIdList = pEventId;
                } else {
                    /* last */
                    pIdList = AmbaMonEvent.pIdList;
                    while (pIdList->Link.Down != NULL) {
                        pIdList = pIdList->Link.Down;
                    }
                    pEventId->Link.Up = pIdList;
                    pEventId->Link.Down = NULL;
                    pIdList->Link.Down = pEventId;
                }

                /* give event list */
                FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonEvent.Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }
        }
    } else {
        /* port/name null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor event obj reset
 *  @param[in] pEventId pointer to the event obj id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonEvent_Reset(AMBA_MON_EVENT_ID_s *pEventId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_EVENT_PORT_s *pPortList;

    if ((pEventId != NULL) &&
        (pEventId->Magic == 0xCafeU)) {
        /* take event id */
        FuncRetCode = AmbaKAL_MutexTake(&(pEventId->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            pPortList = pEventId->pPortList;
            while (pPortList != NULL) {
                if (pPortList->Magic == 0xCafeU) {
                    /* take port */
                    FuncRetCode = AmbaKAL_MutexTake(&(pPortList->Mutex), AMBA_KAL_NO_WAIT);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* accumulated flags clear */
                        pPortList->Flags.Acc = 0ULL;
                        /* give port */
                        FuncRetCode = AmbaKAL_MutexGive(&(pPortList->Mutex));
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                    } else {
                        /* force to clear */
                        pPortList->Flags.Acc = 0ULL;
                    }
                }
                pPortList = pPortList->Link.Down;
            }

            /* give event id */
            FuncRetCode = AmbaKAL_MutexGive(&(pEventId->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* event null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor event obj delete
 *  @param[in] pEventId pointer to the event obj id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonEvent_Delete(AMBA_MON_EVENT_ID_s *pEventId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pEventId != NULL) &&
        (pEventId->Magic == 0xCafeU) &&
        (pEventId->pPortList == NULL)) {
        /* take event list */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonEvent.Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take event id */
            FuncRetCode = AmbaKAL_MutexTake(&(pEventId->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* remove from event list */
                if(AmbaMonEvent.pIdList != NULL) {
                    /* up link */
                    if (pEventId->Link.Up == NULL) {
                        /* root */
                        AmbaMonEvent.pIdList = pEventId->Link.Down;
                    } else {
                        /* linker */
                        pEventId->Link.Up->Link.Down = pEventId->Link.Down;
                    }
                    /* down link */
                    if (pEventId->Link.Down != NULL) {
                        /* root/linker */
                        pEventId->Link.Down->Link.Up = pEventId->Link.Up;
                    }
                    /* magic (invalid), name */
                    pEventId->Magic = 0U;
                    pEventId->pName = NULL;

                    /* give event id */
                    FuncRetCode = AmbaKAL_MutexGive(&(pEventId->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* event id mutex delete */
                    FuncRetCode = AmbaKAL_MutexDelete(&(pEventId->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                } else {
                    RetCode = NG_UL;
                    /* give event id */
                    FuncRetCode = AmbaKAL_MutexGive(&(pEventId->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }
            }

            /* give event list */
            FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonEvent.Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* event null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor event open
 *  @param[in] pPort pointer to the event port
 *  @param[in] pName pointer to the event obj string
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonEvent_Open(AMBA_MON_EVENT_PORT_s *pPort, const char *pName)
{
    UINT32 RetCode = OK_UL;
    UINT32 ChkCode = OK_UL, FuncRetCode;

    AMBA_MON_EVENT_ID_s *pIdList;
    AMBA_MON_EVENT_PORT_s *pPortList;

    if ((pPort != NULL) &&
        (pPort->Magic != 0xCafeU) &&
        (pName != NULL)) {
        /* take event list */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonEvent.Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* event id find */
            if (AmbaMonEvent.pIdList != NULL) {
                /* search by name */
                pIdList = AmbaMonEvent.pIdList;
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
                    if (FuncRetCode != KAL_ERR_NONE) {
                        ChkCode = NG_UL;
                    }
                    /* port sem req */
                    if (ChkCode == OK_UL) {
                        FuncRetCode = AmbaKAL_SemaphoreCreate(&(pPort->Sem.Req), NULL, 0U);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            FuncRetCode = AmbaKAL_MutexDelete(&(pPort->Mutex));
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
                            ChkCode = NG_UL;
                        }
                    }
                    /* port sem ack */
                    if (ChkCode == OK_UL) {
                        FuncRetCode = AmbaKAL_SemaphoreCreate(&(pPort->Sem.Ack), NULL, 0U);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            FuncRetCode = AmbaKAL_MutexDelete(&(pPort->Mutex));
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
                            FuncRetCode = AmbaKAL_SemaphoreDelete(&(pPort->Sem.Req));
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
                            ChkCode = NG_UL;
                        }
                    }

                    if (ChkCode == OK_UL) {
                        /* port settings */
                        pPort->Magic = 0xCafeU;
                        pPort->pEventId = pIdList;
                        pPort->Link.Up = NULL;
                        pPort->Link.Down = NULL;

                        /* port flag */
                        pPort->Flags.Req = 0ULL;
                        pPort->Flags.Acc = 0ULL;
                        pPort->Flags.Ret = 0ULL;

                        /* port op, and/or */
                        pPort->Op.Bits.AndOrClr = MON_EVENT_FLAG_OR_CLR;

                        /* take event id */
                        FuncRetCode = AmbaKAL_MutexTake(&(pIdList->Mutex), AMBA_KAL_WAIT_FOREVER);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* hook port to event */
                            pPortList = pPort->pEventId->pPortList;
                            if (pPortList == NULL) {
                                /* root */
                                pPort->pEventId->pPortList = pPort;
                            } else {
                                /* last */
                                while (pPortList->Link.Down != NULL) {
                                    pPortList = pPortList->Link.Down;
                                }
                                pPort->Link.Up = pPortList;
                                pPortList->Link.Down = pPort;
                            }

                            /* give event id */
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

            /* give event list */
            FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonEvent.Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* port/name null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor event allocation
 *  @param[in] pPort pointer to the event port
 *  @param[in] pEventName pointer to the event string
 *  @param[out] pId pointer to the event id
 *  @param[out] pFlag pointer to the event flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonEvent_Alloc(AMBA_MON_EVENT_PORT_s *pPort, const char *pEventName, UINT32 *pId, UINT64 *pFlag)
{
    UINT32 i, k;
    UINT32 RetCode = OK_UL;
    UINT64 RetFlag = 0ULL;
    UINT32 FuncRetCode;

    AMBA_MON_EVENT_FLAG_s *pFlags;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pPort->pEventId != NULL) &&
        (pEventName != NULL)) {
        /* take event id */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->pEventId->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take event port */
            FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                pFlags = &(pPort->pEventId->Flags);
                /* search flag in event id */
                for (k = 0U; k < 64U; k++) {
                    /* exist check */
                    RetFlag = 1ULL << k;
                    if ((pFlags->BitAlloc & RetFlag) > 0ULL) {
                        if (pFlags->pNameTable[k] != NULL) {
                            FuncRetCode = svar_strcmp(pEventName, pFlags->pNameTable[k]);
                            if (FuncRetCode == OK_UL) {
                                /* exist */
                                *pId = k;
                                *pFlag = RetFlag;
                                pFlags->BitCnt[k]++;
                                break;
                            }
                        }
                    }
                }

                if (k == 64U) {
                    /* not exist */
                    for (i = 0U; i < 64U; i++) {
                        RetFlag = 1ULL << i;
                        if ((pFlags->BitAlloc & RetFlag) == 0ULL) {
                            /* alloc */
                            pFlags->BitAlloc |= RetFlag;
                            pFlags->pNameTable[i] = pEventName;
                            *pId = i;
                            *pFlag = RetFlag;
                            pFlags->BitCnt[i] = 1U;
                            break;
                        }
                    }

                    if (i == 64U) {
                        /* alloc fail */
                        *pId = 0xFFFFFFFFU;
                        *pFlag = 0ULL;
                        RetCode = NG_UL;
                    }
                }

                /* give event port */
                FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }
            /* give event id */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->pEventId->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* port/event null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor event find
 *  @param[in] pPort pointer to the event port
 *  @param[in] pEventName pointer to the event string
 *  @param[out] pId pointer to the event id
 *  @param[out] pFlag pointer to the event flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonEvent_Find(AMBA_MON_EVENT_PORT_s *pPort, const char *pEventName, UINT32 *pId, UINT64 *pFlag)
{
    UINT32 i;
    UINT32 RetCode = OK_UL;
    UINT64 RetFlag = 0ULL;
    UINT32 FuncRetCode;

    const AMBA_MON_EVENT_FLAG_s *pFlags;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pPort->pEventId != NULL) &&
        (pEventName != NULL)) {
        /* take event id */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->pEventId->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take event port */
            FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                pFlags = &(pPort->pEventId->Flags);
                /* event flag find */
                for (i = 0U; i < 64U; i++) {
                    RetFlag = 1ULL << i;
                    if ((pFlags->BitAlloc & RetFlag) > 0ULL) {
                        if (pFlags->pNameTable[i] != NULL) {
                            FuncRetCode = svar_strcmp(pEventName, pFlags->pNameTable[i]);
                            if (FuncRetCode == OK_UL) {
                                /* found */
                                *pId = i;
                                *pFlag = RetFlag;
                                break;
                            }
                        }
                    }
                }

                if (i == 64U) {
                    *pId = 0xFFFFFFFFU;
                    *pFlag = 0ULL;
                    RetCode = NG_UL;
                }

                /* give event port */
                FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }

            /* give event id */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->pEventId->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* port/even null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor event free
 *  @param[in] pPort pointer to the event port
 *  @param[in] pEventName pointer to the event string
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonEvent_Free(AMBA_MON_EVENT_PORT_s *pPort, const char *pEventName)
{
    UINT32 i;
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT64 RetFlag;

    AMBA_MON_EVENT_FLAG_s *pFlags;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pPort->pEventId != NULL) &&
        (pEventName != NULL)) {
        /* take event id */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->pEventId->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take event port */
            FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* get flags info */
                pFlags = &(pPort->pEventId->Flags);
                /* find flag */
                for (i = 0U; i < 64U; i++) {
                    RetFlag = 1ULL << i;
                    if ((pFlags->BitAlloc & RetFlag) > 0ULL) {
                        if (pFlags->pNameTable[i] != NULL) {
                            FuncRetCode = svar_strcmp(pEventName, pFlags->pNameTable[i]);
                            if (FuncRetCode == OK_UL) {
                                /* exist */
                                if (pFlags->BitCnt[i] > 0U) {
                                    pFlags->BitCnt[i]--;
                                }
                                /* flag clr if cnt is zero */
                                if (pFlags->BitCnt[i] == 0U) {
                                    pFlags->BitAlloc &= ~RetFlag;
                                    pFlags->pNameTable[i] = NULL;
                                }
                                break;
                            }
                        }
                    }
                }

                if (i == 64U) {
                    /* non-found */
                    RetCode = NG_UL;
                }

                /* give event port */
                FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }

            /* give event id */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->pEventId->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* port/event null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor event put
 *  @param[in] pPort pointer to the event port
 *  @param[in] Flags event flags
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonEvent_Put(AMBA_MON_EVENT_PORT_s *pPort, UINT64 Flags)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT64 ActFlags;

    AMBA_MON_EVENT_PORT_s *pPortList;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pPort->pEventId != NULL) &&
        (Flags > 0ULL)) {
        /* take event id */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->pEventId->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take event port */
            FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* port list check */
                pPortList = pPort->pEventId->pPortList;
                while (pPortList != NULL) {
                    /* accumulate flags */
                    pPortList->Flags.Acc |= Flags;
                    /* req check */
                    FuncRetCode = AmbaKAL_SemaphoreTake(&(pPortList->Sem.Req), AMBA_KAL_NO_WAIT);
                    /* if req */
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* act flags */
                        ActFlags = pPortList->Flags.Req & pPortList->Flags.Acc;
                        if (ActFlags > 0ULL) {
                            if (((INT32) pPortList->Op.Bits.AndOrClr) == MON_EVENT_FLAG_OR_CLR) {
                                /* or clear*/
                                pPortList->Flags.Ret = ActFlags;
                                pPortList->Flags.Acc = pPortList->Flags.Acc & ~(ActFlags);
                                FuncRetCode = AmbaKAL_SemaphoreGive(&(pPortList->Sem.Ack));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            } else {
                               /* and clear */
                               if (pPortList->Flags.Req == ActFlags) {
                                   pPortList->Flags.Ret = ActFlags;
                                   pPortList->Flags.Acc = pPortList->Flags.Acc & ~(ActFlags);
                                   FuncRetCode = AmbaKAL_SemaphoreGive(&(pPortList->Sem.Ack));
                                   if (FuncRetCode != KAL_ERR_NONE) {
                                       /* */
                                   }
                               } else {
                                   /* wait next */
                                   FuncRetCode = AmbaKAL_SemaphoreGive(&(pPortList->Sem.Req));
                                   if (FuncRetCode != KAL_ERR_NONE) {
                                       /* */
                                   }
                               }
                            }
                        } else {
                            /* wait next */
                            FuncRetCode = AmbaKAL_SemaphoreGive(&(pPortList->Sem.Req));
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
                        }
                    }
                    /* next port */
                    pPortList = pPortList->Link.Down;
                }

                /* give event port */
                FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }

            /* give event id */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->pEventId->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* port/event null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor event get
 *  @param[in] pPort pointer to the event port
 *  @param[in] ReqFlags required flags
 *  @param[out] pActualFlags actual flags
 *  @param[in] Option event flags operation
 *  @param[in] Timeout wait timeout
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonEvent_Get(AMBA_MON_EVENT_PORT_s *pPort, UINT64 ReqFlags, UINT64 *pActualFlags, UINT32 Option, UINT32 Timeout)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_EVENT_OP_s EventOp;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU)) {
        /* take event port */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* option */
            EventOp.Ctx.Data = Option & 0x1U;
            pPort->Op.Bits.AndOrClr = EventOp.Ctx.Bits.AndOrClr;
            /* flags */
            pPort->Flags.Req = ReqFlags;
            /* clear ack */
            while (FuncRetCode == OK_UL) {
                FuncRetCode = AmbaKAL_SemaphoreTake(&(pPort->Sem.Ack), AMBA_KAL_NO_WAIT);
            }
            /* req */
            FuncRetCode = AmbaKAL_SemaphoreGive(&(pPort->Sem.Req));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
            /* wait */
            FuncRetCode = AmbaKAL_SemaphoreTake(&(pPort->Sem.Ack), Timeout);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* ack */
                *pActualFlags = pPort->Flags.Ret;
            } else {
                /* timeout */
                *pActualFlags = 0ULL;
                RetCode = NG_UL;
            }

            /* give event port */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
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
 *  Amba monitor event clear
 *  @param[in] pPort pointer to the event port
 *  @param[in] Flags event flags
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonEvent_Clr(AMBA_MON_EVENT_PORT_s *pPort, UINT64 Flags)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU)) {
        /* take event port */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* flags */
            pPort->Flags.Acc = pPort->Flags.Acc & ~(Flags);
            /* give event port */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
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
 *  Amba monitor event close
 *  @param[in] pPort pointer to the event port
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonEvent_Close(AMBA_MON_EVENT_PORT_s *pPort)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pPort->pEventId != NULL)) {
        /* take event id */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->pEventId->Mutex), AMBA_KAL_WAIT_FOREVER);
        if(FuncRetCode == KAL_ERR_NONE) {
            /* take event port */
            FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* remove from port list  */
                if (pPort->pEventId->pPortList != NULL) {
                    /* up link check */
                    if (pPort->Link.Up == NULL) {
                        /* root */
                        pPort->pEventId->pPortList = pPort->Link.Down;
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
                    pPort->Magic = 0U;

                    /* delete semaphore */
                    FuncRetCode = AmbaKAL_SemaphoreDelete(&(pPort->Sem.Req));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    FuncRetCode = AmbaKAL_SemaphoreDelete(&(pPort->Sem.Ack));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }

                    /* give event port */
                    FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* delete event port mutex */
                    FuncRetCode = AmbaKAL_MutexDelete(&(pPort->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                } else {
                    RetCode = NG_UL;
                    /* give event port */
                    FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }
            }

            /* give event id */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->pEventId->Mutex));
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

