/**
 *  @file AmbaMonListen.c
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
 *  @details Amba Monitor Listen
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaMonEvent.h"
#include "AmbaMonEvent_Internal.h"
#include "AmbaMonRing.h"
#include "AmbaMonWatchdog.h"
#include "AmbaMonMessage.h"
#include "AmbaMonMessage_Internal.h"
#include "AmbaMonVar.h"

#include "AmbaMonListen.h"
#include "AmbaMonListen_Internal.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

static AMBA_MON_LISTEN_s AmbaMonListen;

/**
 *  Amba monitor listen init
 *  @return error code
 */
UINT32 AmbaMonListen_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* mutex */
    FuncRetCode = AmbaKAL_MutexCreate(&(AmbaMonListen.Mutex), NULL);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    /* list header */
    AmbaMonListen.pIdList = NULL;

    return RetCode;
}

/**
 *  Amba monitor listen obj id list check
 *  @param[in] pListenId pointer to the listen obj id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListen_IdListCheck(const AMBA_MON_LISTEN_ID_s *pListenId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 FindFlag = 0U;

    const AMBA_MON_LISTEN_ID_s *pIdList;

    /* take msg list */
    FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonListen.Mutex), AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* get msg list */
        pIdList = AmbaMonListen.pIdList;
        /* null? */
        if (pIdList != NULL) {
            /* search by address */
            do {
                if (pIdList == pListenId) {
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

        /* give msg list */
        FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonListen.Mutex));
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
 *  Amba monitor listen create
 *  @param[in] pListenId pointer to the listen obj id
 *  @param[in] pName pointer to the listen obj string
 *  @param[in] pChunkInfo pointer to the listen chunk data information
 *  @return error code
 */
UINT32 AmbaMonListen_Create(AMBA_MON_LISTEN_ID_s *pListenId, const char *pName, const AMBA_MON_LISTEN_CHUNK_INFO_s *pChunkInfo)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_LISTEN_ID_s *pIdList;

    if ((pListenId != NULL) &&
        (pName != NULL) &&
        (pChunkInfo != NULL)) {
        /* list check */
        FuncRetCode = AmbaMonListen_IdListCheck(pListenId);
        if (FuncRetCode == OK_UL) {
            /* already in list, re-create? */
            RetCode = NG_UL;
        } else {
            /* not in list, force to reset */
            pListenId->Magic = 0U;
            /* message create */
            FuncRetCode = AmbaMonMessage_Create(&(pListenId->MsgId), pName, pChunkInfo);
            if (FuncRetCode != OK_UL) {
                /* msg fail */
                RetCode = NG_UL;
            } else {
                /* create mutex */
                FuncRetCode = AmbaKAL_MutexCreate(&(pListenId->Mutex), NULL);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* magic (valid), name */
                    pListenId->Magic = 0xCafeU;
                    pListenId->pName = pName;

                    /* port list init */
                    pListenId->pPortList = NULL;

                    /* take listen list */
                    FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonListen.Mutex), AMBA_KAL_WAIT_FOREVER);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* hook to listen list */
                        if (AmbaMonListen.pIdList == NULL) {
                            pListenId->Link.Up = NULL;
                            pListenId->Link.Down = NULL;
                            AmbaMonListen.pIdList = pListenId;
                        } else {
                            pIdList = AmbaMonListen.pIdList;
                            while (pIdList->Link.Down != NULL) {
                                pIdList = pIdList->Link.Down;
                            }
                            pListenId->Link.Up = pIdList;
                            pListenId->Link.Down = NULL;
                            pIdList->Link.Down = pListenId;
                        }

                        /* give listen list */
                        FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonListen.Mutex));
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                    }
                }
            }
        }
    } else {
        /* listen/name/chunk null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor listen reset
 *  @param[in] pListenId pointer to the listen obj id
 *  @return error code
 */
UINT32 AmbaMonListen_Reset(AMBA_MON_LISTEN_ID_s *pListenId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pListenId != NULL) &&
        (pListenId->Magic == 0xCafeU)) {
        /* take msg id */
        FuncRetCode = AmbaKAL_MutexTake(&(pListenId->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* message reset */
            FuncRetCode = AmbaMonMessage_Reset(&(pListenId->MsgId));
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }

            /* give msg id */
            FuncRetCode = AmbaKAL_MutexGive(&(pListenId->Mutex));
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
 *  Amba monitor listen delete
 *  @param[in] pListenId pointer to the listen obj id
 *  @return error code
 */
UINT32 AmbaMonListen_Delete(AMBA_MON_LISTEN_ID_s *pListenId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pListenId != NULL) &&
        (pListenId->Magic == 0xCafeU) &&
        (pListenId->pPortList == NULL)) {
        /* take listen list */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonListen.Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take listen id */
            FuncRetCode = AmbaKAL_MutexTake(&(pListenId->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                if (pListenId->pPortList == NULL) {
                    /* remove from listen list */
                    if (AmbaMonListen.pIdList != NULL) {
                        /* up link */
                        if (pListenId->Link.Up == NULL) {
                            /* root */
                            AmbaMonListen.pIdList= pListenId->Link.Down;
                        } else {
                            /* linker */
                            pListenId->Link.Up->Link.Down = pListenId->Link.Down;
                        }
                        /* down link */
                        if (pListenId->Link.Down != NULL) {
                            /* root/linker */
                            pListenId->Link.Down->Link.Up = pListenId->Link.Up;
                        }
                        /* magic (invaild), name */
                        pListenId->Magic = 0U;
                        pListenId->pName = NULL;
                    } else {
                        RetCode = NG_UL;
                    }
                } else {
                    RetCode = NG_UL;
                }

                if (RetCode == OK) {
                    /* message delete */
                    FuncRetCode = AmbaMonMessage_Delete(&(pListenId->MsgId));
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }

                    /* give listen id */
                    FuncRetCode = AmbaKAL_MutexGive(&(pListenId->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* mutex delete */
                    FuncRetCode = AmbaKAL_MutexDelete(&(pListenId->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                } else {
                    /* give listen id */
                    FuncRetCode = AmbaKAL_MutexGive(&(pListenId->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }

                /* give listen list */
                FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonListen.Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }
        }
    } else {
        /* listen null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor listen port list check
 *  @param[in] pPort pointer to the listen port
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListen_PortListCheck(const AMBA_MON_LISTEN_PORT_s *pPort)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 FindFlag = 0U;

    AMBA_MON_LISTEN_ID_s *pIdList;
    const AMBA_MON_LISTEN_PORT_s *pPortList;

    /* take listen obj */
    FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonListen.Mutex), AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        if (AmbaMonListen.pIdList != NULL) {
            /* get listen list */
            pIdList = AmbaMonListen.pIdList;
            do {
                /* take listen id */
                FuncRetCode = AmbaKAL_MutexTake(&(pIdList->Mutex), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == OK_UL) {
                    /* search by address */
                    pPortList = pIdList->pPortList;
                    if (pPortList != NULL) {
                        do {
                            if (pPortList == pPort) {
                                /* found */
                                FindFlag = 1U;
                            }
                            /* next listen port */
                            pPortList = pPortList->Link.Down;
                        } while ((pPortList != NULL) && (FindFlag == 0U));
                    }
                    /* give listen id */
                    FuncRetCode = AmbaKAL_MutexGive(&(pIdList->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }
                /* next listen id */
                pIdList = pIdList->Link.Down;
            } while ((pIdList != NULL) && (FindFlag == 0U));
        } else {
            /* list null */
            RetCode = NG_UL;
        }

        /* give listen obj */
        FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonListen.Mutex));
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
 *  Amba monitor listen open
 *  @param[in] pPort pointer to the listen port
 *  @param[in] pName pointer to the listen obj string
 *  @return error code
 */
UINT32 AmbaMonListen_Open(AMBA_MON_LISTEN_PORT_s *pPort, const char *pName)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_LISTEN_ID_s *pIdList;
    AMBA_MON_LISTEN_PORT_s *pPortList;

    if ((pPort != NULL) &&
        (pName != NULL)) {
        /* list check */
        FuncRetCode = AmbaMonListen_PortListCheck(pPort);
        if (FuncRetCode == OK_UL) {
            /* already in list, re-open? */
            RetCode = NG_UL;
        } else {
            /* not in list, force to reset */
            pPort->Magic = 0U;
            /* take msg list */
            FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonListen.Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                FuncRetCode = AmbaMonMessage_Open(&(pPort->MsgPort), pName, NULL);
                if (FuncRetCode != OK_UL) {
                    RetCode = NG_UL;
                }

                if (RetCode == OK_UL) {
                    if (AmbaMonListen.pIdList != NULL) {
                        /* search by name */
                        pIdList = AmbaMonListen.pIdList;
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
                                pPort->pListenId = pIdList;
                                pPort->Link.Up = NULL;
                                pPort->Link.Down = NULL;

                                /* take msg id */
                                FuncRetCode = AmbaKAL_MutexTake(&(pIdList->Mutex), AMBA_KAL_WAIT_FOREVER);
                                if (FuncRetCode == KAL_ERR_NONE) {
                                    /* hook port to msg */
                                    pPortList = pPort->pListenId->pPortList;
                                    if (pPortList == NULL) {
                                        /* root */
                                        pPort->pListenId->pPortList = pPort;
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
                                    RetCode = NG_UL;
                                }
                            } else {
                                RetCode = NG_UL;
                            }
                        } else {
                            RetCode = NG_UL;
                        }
                    } else {
                        RetCode = NG_UL;
                    }

                    if (RetCode == NG_UL) {
                        /* message close */
                        FuncRetCode = AmbaMonMessage_Close(&(pPort->MsgPort));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        /* magic resete */
                        pPort->Magic = 0U;
                    }
                }

                /* give msg list */
                FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonListen.Mutex));
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
 *  Amba monitor listen callback entry for message linking
 *  @param[in] pCbId pointer to the message callback obj id
 *  @param[in] pMsg pointer to the message data
 */
void AmbaMonListen_CbEntry(void *pCbId, void *pMsg)
{
    UINT32 FuncRetCode;

    AMBA_MON_MESSAGE_CB_s Cb;
    AMBA_MON_MESSAGE_MEM_s MemInfo;
    AMBA_MON_MESSAGE_HEADER_s ListenMsg;

    Cb.Ctx.pVoid = pCbId;

    MemInfo.Ctx.pVoid = pMsg;
    ListenMsg.Ctx.Link.Pointer = (UINT64) MemInfo.Ctx.Data;
    ListenMsg.Ctx.Link.Id = (UINT8) AMBA_MON_MSG_ID_LINK;

    if ((Cb.Ctx.pId != NULL) &&
        (Cb.Ctx.pId->pDesMsgPort != NULL) &&
        (Cb.Ctx.pId->pDesMsgPort->Magic == 0xCafeU)) {
        /* take listen port */
        FuncRetCode = AmbaKAL_MutexTake(Cb.Ctx.pId->pMutex, AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* Message/PutEvent send */
            const void *pMem;
            pMem = AmbaMonMessage_PutEx(Cb.Ctx.pId->pDesMsgPort, &ListenMsg, (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s), &(Cb.Ctx.pId->PutEvent));
            if (pMem == NULL) {
                /* */
            }
            /* give listen port */
            FuncRetCode = AmbaKAL_MutexGive(Cb.Ctx.pId->pMutex);
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    }
}

/**
 *  Amba monitor listen attach for message input
 *  @param[in] pPort pointer to the listen port
 *  @param[in] pSrcMsgPort pointer to source message port
 *  @param[in] pPutEventName pointer to the listen event string
 *  @param[in] pCbId pointer to the listen callback obj id
 *  @param[in] pFunc pointer tot he listen callback function
 *  @return error code
 */
UINT32 AmbaMonListen_Attach(AMBA_MON_LISTEN_PORT_s *pPort, AMBA_MON_MESSAGE_PORT_s *pSrcMsgPort, const char *pPutEventName, AMBA_MON_MESSAGE_CB_ID_s *pCbId, AMBA_MON_MESSAGE_CB_f pFunc)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pSrcMsgPort != NULL) &&
        (pSrcMsgPort->Magic == 0xCafeU) &&
        (pCbId != NULL) &&
        (pFunc != NULL)) {
        /* take listen port */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* cb id */
            pCbId->pMutex = &(pPort->Mutex);
            pCbId->pSrcMsgPort = pSrcMsgPort;
            pCbId->pDesMsgPort = &(pPort->MsgPort);
            pCbId->pPutEventName = pPutEventName;
            pCbId->pFunc = pFunc;
            pCbId->Link.Up = NULL;
            pCbId->Link.Down = NULL;

            /* event flag */
            if (pCbId->pPutEventName != NULL) {
                /* alloc */
                FuncRetCode = AmbaMonEvent_Alloc(&(pCbId->pDesMsgPort->EventPort), pCbId->pPutEventName, &(pCbId->PutEvent.Id), &(pCbId->PutEvent.Flag));
            } else {
                /* non-event */
                pCbId->PutEvent.Id = 0U;
                pCbId->PutEvent.Flag = 0ULL;
                FuncRetCode = OK_UL;
            }
            if (FuncRetCode == OK_UL) {
                /* register CB */
                FuncRetCode = AmbaMonMessage_PutCbRegister(pSrcMsgPort, pCbId, pFunc);
                if (FuncRetCode != OK_UL) {
                    /* cb fail */
                    FuncRetCode = AmbaMonEvent_Free(&(pCbId->pDesMsgPort->EventPort), pCbId->pPutEventName);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    RetCode = NG_UL;
                }
            } else {
                /* event alloc fail */
                RetCode = NG_UL;
            }

            /* give listen port */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* port/cb/func null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor listen remove
 *  @param[in] pPort pointer to the listem port
 *  @param[in] pCbId pointer to the mssage callback obj id
 *  @return error code
 */
UINT32 AmbaMonListen_Remove(AMBA_MON_LISTEN_PORT_s *pPort, AMBA_MON_MESSAGE_CB_ID_s *pCbId)
{
    UINT32 RetCode = OK;
    UINT32 FuncRetCode;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pCbId != NULL) &&
        (pCbId->Magic == 0xCafeU) &&
        (pCbId->pSrcMsgPort != NULL)) {
        /* take listen port */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* unregister CB */
            FuncRetCode = AmbaMonMessage_PutCbUnregister(pCbId->pSrcMsgPort, pCbId);
            if (FuncRetCode == OK_UL) {
                /* free event flag */
                if (pCbId->pPutEventName != NULL) {
                    FuncRetCode = AmbaMonEvent_Free(&(pCbId->pDesMsgPort->EventPort), pCbId->pPutEventName);
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                }
            } else {
                RetCode = NG_UL;
            }

            /* give listen port */
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
 *  Amba monitor listen close
 *  @param[in] pPort pointer to the listen port
 *  @return error code
 */
UINT32 AmbaMonListen_Close(AMBA_MON_LISTEN_PORT_s *pPort)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pPort->pListenId != NULL)) {
        /* take listen id */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->pListenId->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take listen port */
            FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                if (pPort->MsgPort.pCbList == NULL) {
                    if (pPort->pListenId->pPortList != NULL) {
                        /* up link */
                        if (pPort->Link.Up == NULL) {
                            /* root */
                            pPort->pListenId->pPortList = pPort->Link.Down;
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
                    /* msg close */
                    FuncRetCode = AmbaMonMessage_Close(&(pPort->MsgPort));
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                    /* give listen port */
                    FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* delete listen port mutex */
                    FuncRetCode = AmbaKAL_MutexDelete(&(pPort->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                } else {
                    /* give listen port */
                    FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }
            }

            /* give listen id */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->pListenId->Mutex));
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

