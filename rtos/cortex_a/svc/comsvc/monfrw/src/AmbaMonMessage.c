/**
 *  @file AmbaMonMessage.c
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
 *  @details  Amba Monitor Message
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaMonRing.h"
#include "AmbaMonRing_Internal.h"
#include "AmbaMonEvent.h"
#include "AmbaMonEvent_Internal.h"
#include "AmbaMonWatchdog.h"
#include "AmbaMonWatchdog_Internal.h"
#include "AmbaMonVar.h"

#include "AmbaMonMessage.h"
#include "AmbaMonMessage_Internal.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

static AMBA_MON_MESSAGE_s AmbaMonMessage;

/**
 *  Amba monitor message init
 *  @return error code
 */
UINT32 AmbaMonMessage_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* msg mutex */
    FuncRetCode = AmbaKAL_MutexCreate(&(AmbaMonMessage.Mutex), NULL);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    /* msg list header */
    AmbaMonMessage.pIdList = NULL;

    return RetCode;
}

/**
 *  Amba monitor message obj id list check
 *  @param[in] pMsgId pointer to the message obj id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonMessage_IdListCheck(const AMBA_MON_MESSAGE_ID_s *pMsgId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 FindFlag = 0U;

    const AMBA_MON_MESSAGE_ID_s *pIdList;

    /* take msg obj */
    FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonMessage.Mutex), AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* get msg list */
        pIdList = AmbaMonMessage.pIdList;
        /* null? */
        if (pIdList != NULL) {
            /* search by address */
            do {
                if (pIdList == pMsgId) {
                    /* found */
                    FindFlag = 1U;
                }
                /* next msg id */
                pIdList = pIdList->Link.Down;
            } while ((pIdList != NULL) && (FindFlag == 0U));
        } else {
            /* list null */
            RetCode = NG_UL;
        }

        /* give msg obj */
        FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonMessage.Mutex));
        if (FuncRetCode != KAL_ERR_NONE) {
            /* */
        }
    }

    /* not found? */
    if (FindFlag == 0U) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor message create
 *  @param[in] pMsgId pointer to the message obj id
 *  @param[in] pName pointer to the message obj string
 *  @param[in] pChunkInfo pointer to the message chunk data information
 *  @return error code
 */
UINT32 AmbaMonMessage_Create(AMBA_MON_MESSAGE_ID_s *pMsgId, const char *pName, const AMBA_MON_MESSAGE_CHUNK_INFO_s *pChunkInfo)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_MESSAGE_ID_s *pIdList;

    if ((pMsgId != NULL) &&
        (pName != NULL) &&
        (pChunkInfo != NULL)) {
        /* list check */
        FuncRetCode = AmbaMonMessage_IdListCheck(pMsgId);
        if (FuncRetCode == OK_UL) {
            /* already in list, re-create? */
            RetCode = NG_UL;
        } else {
            /* not in list, force reset */
            pMsgId->Magic = 0U;
            pMsgId->RingId.Magic = 0U;
            pMsgId->EventId.Magic = 0U;
            pMsgId->WatchdogId.Magic = 0U;
            /* create ring */
            FuncRetCode = AmbaMonRing_Create(&(pMsgId->RingId), pName, pChunkInfo);
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            } else {
                /* create event */
                FuncRetCode = AmbaMonEvent_Create(&(pMsgId->EventId), pName);
                if (FuncRetCode != OK_UL) {
                    /* ring delete */
                    FuncRetCode = AmbaMonRing_Delete(&(pMsgId->RingId));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    RetCode = NG_UL;
                } else {
                    /* create watchdog */
                    FuncRetCode = AmbaMonWatchdog_Create(&(pMsgId->WatchdogId), pName);
                    if (FuncRetCode != OK_UL) {
                        /* ring delete */
                        FuncRetCode = AmbaMonRing_Delete(&(pMsgId->RingId));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        /* event delete */
                        FuncRetCode = AmbaMonEvent_Delete(&(pMsgId->EventId));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        RetCode = NG_UL;
                    }
                }
            }

            if (RetCode == OK_UL) {
                /* create mutex */
                FuncRetCode = AmbaKAL_MutexCreate(&(pMsgId->Mutex), NULL);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* magic (valid), name */
                    pMsgId->Magic = 0xCafeU;
                    pMsgId->pName = pName;

                    /* port list init */
                    pMsgId->pPortList = NULL;

                    /* take msg obj */
                    FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonMessage.Mutex), AMBA_KAL_WAIT_FOREVER);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* hook to msg list */
                        if (AmbaMonMessage.pIdList == NULL) {
                            pMsgId->Link.Up = NULL;
                            pMsgId->Link.Down = NULL;
                            AmbaMonMessage.pIdList = pMsgId;
                        } else {
                            pIdList = AmbaMonMessage.pIdList;
                            while (pIdList->Link.Down != NULL) {
                                pIdList = pIdList->Link.Down;
                            }
                            pMsgId->Link.Up = pIdList;
                            pMsgId->Link.Down = NULL;
                            pIdList->Link.Down = pMsgId;
                        }

                        /* give msg obj */
                        FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonMessage.Mutex));
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                    } else {
                        /* mutex fail */
                        RetCode = NG_UL;
                    }
                } else {
                    /* mutex fail */
                    RetCode = NG_UL;
                }
            }

            if (RetCode == NG_UL) {
                /* watchdog delete */
                FuncRetCode = AmbaMonWatchdog_Delete(&(pMsgId->WatchdogId));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                /* event delete */
                FuncRetCode = AmbaMonEvent_Delete(&(pMsgId->EventId));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                /* ring delete */
                FuncRetCode = AmbaMonRing_Delete(&(pMsgId->RingId));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                /* magic reset */
                pMsgId->Magic = 0U;
            }
        }
    } else {
        /* msg/name/chunk null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor message reset
 *  @param[in] pMsgId pointer to the message obj id
 *  @return error code
 */
UINT32 AmbaMonMessage_Reset(AMBA_MON_MESSAGE_ID_s *pMsgId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pMsgId != NULL) &&
        (pMsgId->Magic == 0xCafeU)) {
        /* take msg id */
        FuncRetCode = AmbaKAL_MutexTake(&(pMsgId->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* ring reset */
            FuncRetCode = AmbaMonRing_Reset(&(pMsgId->RingId));
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }

            /* event reset */
            FuncRetCode = AmbaMonEvent_Reset(&(pMsgId->EventId));
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }

            /* watchdog reset */
            FuncRetCode = AmbaMonWatchdog_Reset(&(pMsgId->WatchdogId));
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }

            /* give msg id */
            FuncRetCode = AmbaKAL_MutexGive(&(pMsgId->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* msg null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor message delete
 *  @param[in] pMsgId pointer to the message obj id
 *  @return error code
 */
UINT32 AmbaMonMessage_Delete(AMBA_MON_MESSAGE_ID_s *pMsgId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pMsgId != NULL) &&
        (pMsgId->Magic == 0xCafeU) &&
        (pMsgId->pPortList == NULL)) {
        /* take msg obj */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonMessage.Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take msg id */
            FuncRetCode = AmbaKAL_MutexTake(&(pMsgId->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                if (pMsgId->pPortList == NULL) {
                    /* remove from msg list */
                    if (AmbaMonMessage.pIdList != NULL) {
                        /* up link */
                        if (pMsgId->Link.Up == NULL) {
                            /* root */
                            AmbaMonMessage.pIdList= pMsgId->Link.Down;
                        } else {
                            /* linker */
                            pMsgId->Link.Up->Link.Down = pMsgId->Link.Down;
                        }
                        /* down link */
                        if (pMsgId->Link.Down != NULL) {
                            /* root/linker */
                            pMsgId->Link.Down->Link.Up = pMsgId->Link.Up;
                        }
                        /* magic (invaild), name */
                        pMsgId->Magic = 0U;
                        pMsgId->pName = NULL;
                    } else {
                        RetCode = NG_UL;
                    }
                } else {
                    RetCode = NG_UL;
                }

                if (RetCode == OK_UL) {
                    /* ring delete */
                    FuncRetCode = AmbaMonRing_Delete(&(pMsgId->RingId));
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                    /* event delete */
                    FuncRetCode = AmbaMonEvent_Delete(&(pMsgId->EventId));
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                    /* watchdog delete */
                    FuncRetCode = AmbaMonWatchdog_Delete(&(pMsgId->WatchdogId));
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }

                    /* give msg id */
                    FuncRetCode = AmbaKAL_MutexGive(&(pMsgId->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* mutex delete */
                    FuncRetCode = AmbaKAL_MutexDelete(&(pMsgId->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                } else {
                    /* give msg id */
                    FuncRetCode = AmbaKAL_MutexGive(&(pMsgId->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }
            }

            /* give msg obj */
            FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonMessage.Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* msg null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor message port list check
 *  @param[in] pPort pointer to the message port
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonMessage_PortListCheck(const AMBA_MON_MESSAGE_PORT_s *pPort)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 FindFlag = 0U;

    AMBA_MON_MESSAGE_ID_s *pIdList;
    const AMBA_MON_MESSAGE_PORT_s *pPortList;

    /* take msg obj */
    FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonMessage.Mutex), AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        if (AmbaMonMessage.pIdList != NULL) {
            /* get msg list */
            pIdList = AmbaMonMessage.pIdList;
            do {
                /* take msg id */
                FuncRetCode = AmbaKAL_MutexTake(&(pIdList->Mutex), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == OK_UL) {
                    /* search msg port by address */
                    pPortList = pIdList->pPortList;
                    if (pPortList != NULL) {
                        do {
                            if (pPortList == pPort) {
                                /* found */
                                FindFlag = 1U;
                            }
                            /* next msg port */
                            pPortList = pPortList->Link.Down;
                        } while ((pPortList != NULL) && (FindFlag == 0U));
                    }
                    /* give msg id */
                    FuncRetCode = AmbaKAL_MutexGive(&(pIdList->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }
                /* next msg id */
                pIdList = pIdList->Link.Down;
            } while ((pIdList != NULL) && (FindFlag == 0U));
        } else {
            /* list null */
            RetCode = NG_UL;
        }

        /* give msg obj */
        FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonMessage.Mutex));
        if (FuncRetCode != KAL_ERR_NONE) {
            /* */
        }
    }

    /* not found? */
    if (FindFlag == 0U) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor message open
 *  @param[in] pPort pointer to the message port
 *  @param[in] pName pointer to the message obj string
 *  @param[in] pPutEventName pointer to the message event string
 *  @return error code
 */
UINT32 AmbaMonMessage_Open(AMBA_MON_MESSAGE_PORT_s *pPort, const char *pName, const char *pPutEventName)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_MESSAGE_ID_s *pIdList;
    AMBA_MON_MESSAGE_PORT_s *pPortList;

    if ((pPort != NULL) &&
        (pName != NULL)) {
        /* list check */
        FuncRetCode = AmbaMonMessage_PortListCheck(pPort);
        if (FuncRetCode == OK_UL) {
            /* already in list, re-open? */
            RetCode = NG_UL;
        } else {
            /* not in list, force to reset */
            pPort->Magic = 0U;
            pPort->RingPort.Magic = 0U;
            pPort->EventPort.Magic = 0U;
            pPort->WatchdogPort.Magic = 0U;
            /* take msg obj */
            FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonMessage.Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* ring port open */
                FuncRetCode = AmbaMonRing_Open(&(pPort->RingPort), pName);
                if (FuncRetCode != OK_UL) {
                    RetCode = NG_UL;
                } else {
                    /* event port open */
                    FuncRetCode = AmbaMonEvent_Open(&(pPort->EventPort), pName);
                    if (FuncRetCode != OK_UL) {
                        /* ring port close */
                        FuncRetCode = AmbaMonRing_Close(&(pPort->RingPort));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        RetCode = NG_UL;
                    } else {
                        /* watchdog port open */
                        FuncRetCode = AmbaMonWatchdog_Open(&(pPort->WatchdogPort), pName);
                        if (FuncRetCode != OK_UL) {
                            /* ring port close */
                            FuncRetCode = AmbaMonRing_Close(&(pPort->RingPort));
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                            /* event port close */
                            FuncRetCode = AmbaMonEvent_Close(&(pPort->EventPort));
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                            RetCode = NG_UL;
                        }
                    }
                }

                if (RetCode == OK_UL) {
                    if (pPutEventName != NULL) {
                        /* flag alloc */
                        FuncRetCode = AmbaMonEvent_Alloc(&(pPort->EventPort), pPutEventName, &(pPort->PutEvent.Id), &(pPort->PutEvent.Flag));
                        if (FuncRetCode == OK_UL) {
                            /* success */
                            pPort->pPutEventName = pPutEventName;
                        } else {
                            /* fail */
                            pPort->pPutEventName = NULL;
                        }
                    } else {
                        /* no need flag */
                        pPort->pPutEventName = NULL;
                        pPort->PutEvent.Id = 0xFFFFFFFFU;
                        pPort->PutEvent.Flag = 0ULL;
                    }

                    if (AmbaMonMessage.pIdList != NULL) {
                        /* search by name */
                        pIdList = AmbaMonMessage.pIdList;
                        do {
                            FuncRetCode = svar_strcmp(pIdList->pName, pName);
                            if (FuncRetCode == OK_UL) {
                                /* found */
                                break;
                            }
                            /* next */
                            pIdList = pIdList->Link.Down;
                        } while (pIdList != NULL);

                        if (pIdList != NULL) {
                            /* mutex create */
                            FuncRetCode = AmbaKAL_MutexCreate(&(pPort->Mutex), NULL);
                            if (FuncRetCode == KAL_ERR_NONE) {
                                /* port setting */
                                pPort->Magic = 0xCafeU;
                                pPort->pMsgId = pIdList;
                                pPort->pCbList = NULL;
                                pPort->Link.Up = NULL;
                                pPort->Link.Down = NULL;

                                /* port watchdog reset */
                                pPort->Watchdog.Period = 0U;
                                pPort->Watchdog.pUser = NULL;

                                /* take msg id */
                                FuncRetCode = AmbaKAL_MutexTake(&(pIdList->Mutex), AMBA_KAL_WAIT_FOREVER);
                                if (FuncRetCode == KAL_ERR_NONE) {
                                    /* hook port to msg */
                                    pPortList = pPort->pMsgId->pPortList;
                                    if (pPortList == NULL) {
                                        /* root */
                                        pPort->pMsgId->pPortList = pPort;
                                    } else {
                                        /* last */
                                        while (pPortList->Link.Down != NULL) {
                                            pPortList = pPortList->Link.Down;
                                        }
                                        pPort->Link.Up = pPortList;
                                        pPortList->Link.Down = pPort;
                                    }

                                    /* give msg id */
                                    FuncRetCode = AmbaKAL_MutexGive(&(pIdList->Mutex));
                                    if (FuncRetCode != KAL_ERR_NONE) {
                                        /* */
                                    }
                                } else {
                                    /* mutex fail */
                                    RetCode = NG_UL;
                                }
                            } else {
                                /* mutex fail */
                                RetCode = NG_UL;
                            }
                        } else {
                            RetCode = NG_UL;
                        }
                    } else {
                        RetCode = NG_UL;
                    }

                    if (RetCode == NG_UL) {
                        /* fail */
                        if (pPort->pPutEventName != NULL) {
                            /* free flag */
                            FuncRetCode = AmbaMonEvent_Free(&(pPort->EventPort), pPutEventName);
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                        }
                        /* watchdog close */
                        FuncRetCode = AmbaMonWatchdog_Close(&(pPort->WatchdogPort));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        /* event close */
                        FuncRetCode = AmbaMonEvent_Close(&(pPort->EventPort));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        /* ring close */
                        FuncRetCode = AmbaMonRing_Close(&(pPort->RingPort));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        /* magic reset */
                        pPort->Magic = 0U;
                    }
                }

                /* give msg obj */
                FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonMessage.Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }
        }
    } else {
        /* port/name null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor message callback list check
 *  @param[in] pPort pointer to the mssage port
 *  @param[in] pCbId pointer to the mssage callback obj id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonMessage_CbListCheck(AMBA_MON_MESSAGE_PORT_s *pPort, const AMBA_MON_MESSAGE_CB_ID_s *pCbId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 FindFlag = 0U;

    const AMBA_MON_MESSAGE_CB_ID_s *pCbList;

    /* take port obj */
    FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* get cb list */
        pCbList = pPort->pCbList;
        /* null? */
        if (pCbList != NULL) {
            /* search by address */
            do {
                if (pCbList == pCbId) {
                    /* found */
                    FindFlag = 1U;
                }
                /* next cb id */
                pCbList = pCbList->Link.Down;
            } while ((pCbList != NULL) && (FindFlag == 0U));
        } else {
            /* list null */
            RetCode = NG_UL;
        }

        /* give port obj */
        FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
        if (FuncRetCode != KAL_ERR_NONE) {
            /* */
        }
    }

    /* not found? */
    if (FindFlag == 0U) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor message put callback register
 *  @param[in] pPort pointer to the message port
 *  @param[in] pCbId pointer to the message callback obj id
 *  @param[in] pFunc pointer to the message callback function
 *  @return error code
 */
UINT32 AmbaMonMessage_PutCbRegister(AMBA_MON_MESSAGE_PORT_s *pPort, AMBA_MON_MESSAGE_CB_ID_s *pCbId, AMBA_MON_MESSAGE_CB_f pFunc)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_MESSAGE_CB_ID_s *pCbList;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pCbId != NULL)) {
        /* list check */
        FuncRetCode = AmbaMonMessage_CbListCheck(pPort, pCbId);
        if (FuncRetCode == OK_UL) {
            /* already in list, re-register? */
            RetCode = NG_UL;
        } else {
            /* not in list, force to reset */
            pCbId->Magic = 0U;
            /* take msg port */
            FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* attach cb func */
                pCbId->pFunc = pFunc;
                /* cb list link */
                pCbList = pPort->pCbList;
                if (pPort->pCbList == NULL) {
                    /* root */
                    pCbId->Link.Up = NULL;
                    pCbId->Link.Down = NULL;
                    pPort->pCbList = pCbId;
                } else {
                    /* last */
                    while (pCbList->Link.Down != NULL) {
                        pCbList = pCbList->Link.Down;
                    }
                    pCbId->Link.Up = pCbList;
                    pCbId->Link.Down = NULL;
                    pCbList->Link.Down = pCbId;
                }

                /* magic assign */
                pCbId->Magic = 0xCafeU;

                /* give msg port */
                FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }
        }
    } else {
        /* port/cb null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor message put callback unregister
 *  @param[in] pPort pointer to the message port
 *  @param[in] pCbId pointer to the message callback obj id
 *  @return error code
 */
UINT32 AmbaMonMessage_PutCbUnregister(AMBA_MON_MESSAGE_PORT_s *pPort, AMBA_MON_MESSAGE_CB_ID_s *pCbId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pCbId != NULL) &&
        (pCbId->Magic == 0xCafeU)) {
        /* take msg port */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* up link */
            if (pCbId->Link.Up == NULL) {
                /* root */
                pPort->pCbList = pCbId->Link.Down;
            } else {
                /* linker */
                pCbId->Link.Up->Link.Down = pCbId->Link.Down;
            }
            /* down link */
            if (pCbId->Link.Down != NULL) {
                /* root/linker */
                pCbId->Link.Down->Link.Up = pCbId->Link.Up;
            }

            /* magic clear */
            pCbId->Magic = 0U;

            /* give msg port */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* port/cb null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor message put
 *  @param[in] pPort pointer to the message port
 *  @param[in] pData pointer to the data
 *  @param[in] Size data size
 *  @return message address
 */
void *AmbaMonMessage_Put(AMBA_MON_MESSAGE_PORT_s *pPort, void *pData, UINT32 Size)
{
    UINT32 FuncRetCode;

    AMBA_MON_MESSAGE_MEM_s MemInfo;
    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;

    void *pMem = NULL;
    AMBA_MON_MESSAGE_CB_ID_s *pCbList;

    MemInfo.Ctx.pVoid = pData;
    pMsgHead = MemInfo.Ctx.pMsgHead;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pData != NULL)) {
        /* take msg port */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* assign event flag into header */
            pMsgHead->Ctx.Event.Flag = pPort->PutEvent.Flag;

            /* write to ring */
            pMem = AmbaMonRing_Write(&(pPort->RingPort), pData, Size);
            if (pMem != NULL) {
                /* callback */
                pCbList = pPort->pCbList;
                while (pCbList != NULL) {
                    if (pCbList->pFunc != NULL) {
                        pCbList->pFunc(pCbList, pMem);
                    }
                    pCbList = pCbList->Link.Down;
                }

                /* event notify */
                if (pPort->PutEvent.Flag > 0ULL) {
                    FuncRetCode = AmbaMonEvent_Put(&(pPort->EventPort), pPort->PutEvent.Flag);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }

                /* watchdog refresh */
                if (pPort->Watchdog.Period > 0U) {
                    MemInfo.Ctx.pMsgPort = pPort;
                    FuncRetCode = AmbaMonWatchdog_Refresh(&(pPort->WatchdogPort), pPort->Watchdog.Period, MemInfo.Ctx.pVoid);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
            }

            /* give msg port */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    }

    return pMem;
}

/**
 *  Amba monitor message put 2
 *  @param[in] pPort pointer to the message port
 *  @param[in] pHeader pointer to the header
 *  @param[in] HSize header size
 *  @param[in] pData pointer to the data
 *  @param[in] DSize data size
 *  @return message address
 */
void *AmbaMonMessage_Put2(AMBA_MON_MESSAGE_PORT_s *pPort, void *pHeader, UINT32 HSize, const void *pData, UINT32 DSize)
{
    UINT32 FuncRetCode;

    AMBA_MON_MESSAGE_MEM_s MemInfo;
    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;

    void *pMem = NULL;
    AMBA_MON_MESSAGE_CB_ID_s *pCbList;

    MemInfo.Ctx.pVoid = pHeader;
    pMsgHead = MemInfo.Ctx.pMsgHead;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pHeader != NULL) &&
        (pData != NULL)) {
        /* take msg port */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* assign event flag into header */
            pMsgHead->Ctx.Event.Flag = pPort->PutEvent.Flag;

            /* write to ring */
            pMem = AmbaMonRing_Write2(&(pPort->RingPort), pHeader, HSize, pData, DSize);
            if (pMem != NULL) {
                /* callback */
                pCbList = pPort->pCbList;
                while (pCbList != NULL) {
                    if (pCbList->pFunc != NULL) {
                        pCbList->pFunc(pCbList, pMem);
                    }
                    pCbList = pCbList->Link.Down;
                }

                /* event notify */
                if (pPort->PutEvent.Flag > 0ULL) {
                    FuncRetCode = AmbaMonEvent_Put(&(pPort->EventPort), pPort->PutEvent.Flag);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }

                /* watchdog refresh */
                if (pPort->Watchdog.Period > 0U) {
                    MemInfo.Ctx.pMsgPort = pPort;
                    FuncRetCode = AmbaMonWatchdog_Refresh(&(pPort->WatchdogPort), pPort->Watchdog.Period, MemInfo.Ctx.pVoid);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
            }

            /* give msg port */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    }

    return pMem;
}

/**
 *  Amba monitor message put (extend)
 *  @param[in] pPort pointer to the message port
 *  @param[in] pData pointer to the data
 *  @param[in] Size data size
 *  @param[in] pPutEvent pointer to the message event information
 *  @return message address
 */
void *AmbaMonMessage_PutEx(AMBA_MON_MESSAGE_PORT_s *pPort, void *pData, UINT32 Size, const AMBA_MON_MESSAGE_EVENT_s *pPutEvent)
{
    UINT32 FuncRetCode;

    AMBA_MON_MESSAGE_MEM_s MemInfo;
    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;

    void *pMem = NULL;
    AMBA_MON_MESSAGE_CB_ID_s *pCbList;

    MemInfo.Ctx.pVoid = pData;
    pMsgHead = MemInfo.Ctx.pMsgHead;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pData != NULL)) {
        /* take msg port */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* assign event flag into header */
            pMsgHead->Ctx.Event.Flag = 0ULL;
            if (pPutEvent != NULL) {
                pMsgHead->Ctx.Event.Flag = pPutEvent->Flag;
            }

            /* write to ring */
            pMem = AmbaMonRing_Write(&(pPort->RingPort), pData, Size);
            if (pMem != NULL) {
                /* callback */
                pCbList = pPort->pCbList;
                while (pCbList != NULL) {
                    if ((pCbList->pFunc != NULL) && (pCbList->pDesMsgPort != NULL)) {
                        pCbList->pFunc(pCbList, pMem);
                    }
                    pCbList = pCbList->Link.Down;
                }

                /* event notify */
                if ((pPutEvent != NULL) &&
                    (pPutEvent->Flag > 0ULL)) {
                    FuncRetCode = AmbaMonEvent_Put(&(pPort->EventPort), pPutEvent->Flag);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
            }

            /* give msg port */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    }

    return pMem;
}

/**
 *  Amba monitor message get
 *  @param[in] pPort pointer to the mssage port
 *  @param[in] pData pointer to the data pointer
 *  @return error code
 */
UINT32 AmbaMonMessage_Get(AMBA_MON_MESSAGE_PORT_s *pPort, void **pData)
{
    UINT32 RetCode = OK;
    UINT32 FuncRetCode;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pData != NULL)) {
        /* take msg port */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* read from ring */
            FuncRetCode = AmbaMonRing_Read(&(pPort->RingPort), pData);
            if (FuncRetCode != OK_UL) {
                *pData = NULL;
                RetCode = FuncRetCode;
            }

            /* give msg port */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* port/data null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor message read seek
 *  @param[in] pPort pointer to the message port
 *  @param[in] Offset position offset
 *  @param[in] Flag position flag
 *  @return error code
 */
UINT32 AmbaMonMessage_RSeek(AMBA_MON_MESSAGE_PORT_s *pPort, UINT32 Offset, UINT32 Flag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU)) {
        /* take msg port */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* read from ring */
            FuncRetCode = AmbaMonRing_RSeek(&(pPort->RingPort), Offset, Flag);
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }

            /* give msg port */
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
 *  Amba monitor message find
 *  @param[in] pPort pointer to the message port
 *  @param[in] pFlagName pointer to the event string
 *  @param[out] pId pointer to the event id
 *  @param[out] pFlag pointer to the event flag
 *  @return error code
 */
UINT32 AmbaMonMessage_Find(AMBA_MON_MESSAGE_PORT_s *pPort, const char *pFlagName, UINT32 *pId, UINT64 *pFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pFlagName != NULL)) {
        /* take msg port */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* find flag */
            FuncRetCode = AmbaMonEvent_Find(&(pPort->EventPort), pFlagName, pId, pFlag);
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }

            /* give msg port */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* port/flag null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor message wait
 *  @param[in] pPort pointer to the message port
 *  @param[in] Flags event flags
 *  @param[in] Option event flags operation
 *  @param[in] Timeout wait timeout
 *  @return actual event flags
 */
UINT64 AmbaMonMessage_Wait(AMBA_MON_MESSAGE_PORT_s *pPort, UINT64 Flags, UINT32 Option, UINT32 Timeout)
{
    UINT32 FuncRetCode;

    UINT64 RetFlags = 0ULL;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (Flags > 0ULL)) {
        /* take msg port */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* wait flags */
            FuncRetCode = AmbaMonEvent_Get(&(pPort->EventPort), Flags, &RetFlags, Option, Timeout);
            if (FuncRetCode != OK_UL) {
                RetFlags = 0ULL;
            }

            /* give msg port */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    }

    return RetFlags;
}

/**
 *  Amba monitor message timeout set (watchdog for periodic message)
 *  @param[in] pPort pointer to the message port
 *  @param[in] Period period time (ms)
 *  @param[in] pUser pointer to the user data
 *  @return error code
 */
UINT32 AmbaMonMessage_TimeoutSet(AMBA_MON_MESSAGE_PORT_s *pPort, UINT32 Period, void *pUser)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU)) {
        /* take msg port */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* watchdog info */
            pPort->Watchdog.Period = Period; /* 0: disable */
            pPort->Watchdog.pUser = pUser;
            /* give msg port */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        /* port null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor message timeout get
 *  @param[out] pPort poiner to the message port pointer
 *  @param[out] pPeriod pointer to the period time (ms)
 *  @param[out] pUser pointer to the user data pointer
 *  @return error code
 */
UINT32 AmbaMonMessage_TimeoutGet(AMBA_MON_MESSAGE_PORT_s **pPort, UINT32 *pPeriod, void **pUser)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_WATCHDOG_PORT_s *pTimeoutPort;
    UINT32 TimeoutPeriod;
    void *pTimeoutUser;

    FuncRetCode = AmbaMonWatchdog_TimeoutGet(&pTimeoutPort, &TimeoutPeriod, &pTimeoutUser);
    if (FuncRetCode == OK_UL) {
        AMBA_MON_MESSAGE_MEM_s MemInfo;
        MemInfo.Ctx.pVoid = pTimeoutUser;
        *pPort = MemInfo.Ctx.pMsgPort;
        *pPeriod = TimeoutPeriod;
        *pUser = MemInfo.Ctx.pMsgPort->Watchdog.pUser;
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor message timeout refresh
 *  @param[in] pPort pointer to the message port
 *  @param[in] Period period time (ms)
 *  @return error code
 */
UINT32 AmbaMonMessage_TimeoutRefresh(AMBA_MON_MESSAGE_PORT_s *pPort, UINT32 Period)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU)) {
        /* take msg port */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            AMBA_MON_MESSAGE_MEM_s MemInfo;
            MemInfo.Ctx.pMsgPort = pPort;
            /* watchdog refresh */
            FuncRetCode = AmbaMonWatchdog_Refresh(&(pPort->WatchdogPort), Period, MemInfo.Ctx.pVoid);
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }
            /* give msg port */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        } else {
            RetCode = NG_UL;
        }
    } else {
        /* port null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor message close
 *  @param[in] pPort pointer to the message port
 *  @return error code
 */
UINT32 AmbaMonMessage_Close(AMBA_MON_MESSAGE_PORT_s *pPort)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pPort->pMsgId != NULL)) {
        /* take msg obj */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->pMsgId->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take msg port */
            FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                if (pPort->pCbList == NULL) {
                    /* port list remove */
                    if (pPort->pMsgId->pPortList != NULL) {
                        /* up link */
                        if (pPort->Link.Up == NULL) {
                            /* root */
                            pPort->pMsgId->pPortList = pPort->Link.Down;
                        } else {
                            /* linker */
                            pPort->Link.Up->Link.Down = pPort->Link.Down;
                        }
                        /* down link */
                        if (pPort->Link.Down != NULL) {
                            /* root/linker */
                            pPort->Link.Down->Link.Up = pPort->Link.Up;
                        }
                        /* magic clr */
                        pPort->Magic = 0U;
                    } else {
                        RetCode = NG_UL;
                    }
                } else {
                    RetCode = NG_UL;
                }

                if (RetCode == OK_UL) {
                    /* ring close */
                    FuncRetCode = AmbaMonRing_Close(&(pPort->RingPort));
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                    /* event close */
                    FuncRetCode = AmbaMonEvent_Close(&(pPort->EventPort));
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                    /* watchdog close */
                    FuncRetCode = AmbaMonWatchdog_Close(&(pPort->WatchdogPort));
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                    /* give msg port */
                    FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* delete msg port mutex */
                    FuncRetCode = AmbaKAL_MutexDelete(&(pPort->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                } else {
                    /* give msg port */
                    FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }

                /* give msg obj */
                FuncRetCode = AmbaKAL_MutexGive(&(pPort->pMsgId->Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }
        }
    } else {
        /* port null or invlaid */
        RetCode = NG_UL;
    }

    return RetCode;
}

