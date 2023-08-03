/**
 *  @file AmbaCAN.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you 
 * distribute this sample code (whether in source, object, or binary code form), it will be 
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details CAN bus control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaCAN.h"
#include "AmbaRTSL_CAN.h"

#define AMBA_CAN_TX_BUFFER_SIZE  1U          /* The number of message buffers are acted as Tx buffers, the rest are Rx buffers. */
#define AMBA_CAN_EVENT_MASK      0xffffU    /* Indicate the CAN job completion */

typedef struct {
    AMBA_KAL_EVENT_FLAG_t   EventFlag;      /* Event Flags */
    UINT32                  RxEventMask;
    UINT32                  TxEventMask;
    AMBA_KAL_MUTEX_t        RxMutex;
    AMBA_KAL_MUTEX_t        FdRxMutex;
    AMBA_KAL_MUTEX_t        TxMutex;
    AMBA_KAL_SEMAPHORE_t    RxSem;          /* Rx Counting Semaphore */
    AMBA_KAL_SEMAPHORE_t    FdRxSem;        /* Rx Counting Semaphore for CAN FD messages */
} AMBA_CAN_CTRL_s;

static AMBA_CAN_CTRL_s AmbaCAN_Ctrl;  /* CAN Management Structure */

static void CAN_RxISR(UINT32 CanCh, UINT32 MsgBufNo)
{
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl;
    UINT32 Ret;

    (VOID) CanCh;
    (VOID) MsgBufNo;

    /* increase the counting semaphore */
    Ret = AmbaKAL_SemaphoreGive(&pCanCtrl->RxSem);
    if (Ret != KAL_ERR_NONE) {
        /* should never happen*/
    }
}

static void CAN_TxISR(UINT32 CanCh, UINT32 MsgBufNo)
{
    UINT32 SetBit = 1UL << MsgBufNo;

    (VOID) CanCh;

    /* Set CAN Done Event Flag */
    (VOID) AmbaKAL_EventFlagSet(&AmbaCAN_Ctrl.EventFlag, SetBit);
}

static void CAN_FdRxISR(UINT32 CanCh, UINT32 MsgBufNo)
{
    UINT32 Ret;
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl;

    (VOID) CanCh;
    (VOID) MsgBufNo;

    /* increase the counting semaphore */
    Ret = AmbaKAL_SemaphoreGive(&(pCanCtrl->FdRxSem));
    if (Ret != KAL_ERR_NONE) {
        /* should never happen*/
    }
}

static UINT32 CAN_Init(void)
{
    static char CanEventFlagStr[] = "CAN Event_Flag";
    static char CanTxMutexStr[] = "CAN Tx Mutex";
    static char CanRxMutexStr[] = "CAN Rx Mutex";
    static char CanFDRxMutexStr[] = "CAN FD Rx Mutex";
    static char CanRxSemaphoreStr[] = "CAN Rx Semaphore";
    static char CanFDRxSemaphoreStr[] = "CAN FD Rx Semaphore";
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl;
    UINT32 Ret;

    /* Create Event Flags */
    Ret = AmbaKAL_EventFlagCreate(&pCanCtrl->EventFlag, CanEventFlagStr);
    if (Ret == KAL_ERR_NONE) {
        /* Signal the Event Flags of All Available Message Buffers */
        Ret = AmbaKAL_EventFlagSet(&(pCanCtrl->EventFlag), AMBA_CAN_EVENT_MASK);
        if (Ret == KAL_ERR_NONE) {
            /* Create Tx/Rx Mutex */
            Ret = AmbaKAL_MutexCreate(&pCanCtrl->TxMutex, CanTxMutexStr);
            if (Ret == KAL_ERR_NONE) {
                Ret = AmbaKAL_MutexCreate(&pCanCtrl->RxMutex, CanRxMutexStr);
                if (Ret == KAL_ERR_NONE) {
                    Ret = AmbaKAL_MutexCreate(&pCanCtrl->FdRxMutex, CanFDRxMutexStr);
                    if (Ret == KAL_ERR_NONE) {
                        Ret = AmbaKAL_SemaphoreCreate(&pCanCtrl->RxSem, CanRxSemaphoreStr, 0);
                        if (Ret == KAL_ERR_NONE) {
                            Ret = AmbaKAL_SemaphoreCreate(&pCanCtrl->FdRxSem, CanFDRxSemaphoreStr, 0);
                            if (Ret == KAL_ERR_NONE) {
                                /* CAN RTSL initializations */
                                Ret = AmbaRTSL_CanInit();
                                if (Ret == CAN_ERR_NONE) {
                                    /* Set Tx/Rx callback functions when completed transactions */
                                    AmbaRTSL_CanSetRxIsrFunc(CAN_RxISR);
                                    AmbaRTSL_CanFdSetRxIsrFunc(CAN_FdRxISR);
                                    AmbaRTSL_CanSetTxIsrFunc(CAN_TxISR);
                                }
                            } else {
                                Ret = CAN_ERR_ARG;
                            }
                        } else {
                            Ret = CAN_ERR_ARG;
                        }
                    } else {
                        Ret = CAN_ERR_ARG;
                    }
                } else {
                    Ret = CAN_ERR_ARG;
                }
            } else {
                Ret = CAN_ERR_ARG;
            }
        } else {
            Ret = CAN_ERR_ARG;
        }
    } else {
        Ret = CAN_ERR_ARG;
    }

    return Ret;
}

/**
 * This function is used to start a specific CAN controller with given configurations
 * @param[in] CanCh Channel ID
 * @param[in] pCanConfig CAN configuration
 * @return 0-OK, other-NG
 */
UINT32 AmbaCAN_Enable(UINT32 CanCh, const AMBA_CAN_CONFIG_s *pCanConfig)
{
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl;
    UINT32 MsgBufType = 0;
    UINT32 i;
    UINT32 Ret;

    if ((CanCh >= AMBA_NUM_CAN_CHANNEL) || (pCanConfig == NULL)) {
        Ret = CAN_ERR_ARG;
    } else {
        Ret = CAN_Init();
        if (Ret == CAN_ERR_NONE) {
            AMBA_CAN_REG_s *pCanReg = pAmbaCAN_Reg[CanCh];

            /*-----------------------------------------------------------------------*\
             * Take the Mutex
            \*-----------------------------------------------------------------------*/
            if (AmbaKAL_MutexTake(&pCanCtrl->TxMutex, AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
                if (AmbaKAL_MutexTake(&(pCanCtrl->RxMutex), AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
                    /* Reset CAN */
                    AmbaCSL_CanReset(pCanReg, 1);
                    AmbaCSL_CanReset(pCanReg, 0);

                    if (pCanConfig->OpMode == AMBA_CAN_OP_MODE_LISTEN) {
                        MsgBufType = 0;
                    } else {
                        for (i = 0; i < AMBA_CAN_TX_BUFFER_SIZE; i++) {
                            UINT32 SetBit = 1UL << i;
                            MsgBufType |= SetBit;
                        }
                    }

                    /* CAN Configurations */
                    Ret = AmbaRTSL_CanConfig(CanCh, pCanConfig, MsgBufType);
                    if (Ret == CAN_ERR_NONE) {
                        /* Config Tx/Rx event flags and Rx semaphore */
                        pCanCtrl->RxEventMask = MsgBufType ^ AMBA_CAN_EVENT_MASK;
                        pCanCtrl->TxEventMask = MsgBufType & AMBA_CAN_EVENT_MASK;

                        /* Enable CANC */
                        AmbaCSL_CanEnable(pCanReg);

                        /*-----------------------------------------------------------------------*\
                         * Release the Mutex
                        \*-----------------------------------------------------------------------*/
                        Ret = AmbaKAL_MutexGive(&pCanCtrl->RxMutex);
                        if (Ret != KAL_ERR_NONE) {
                            /* should never happen*/
                        }
                        Ret = AmbaKAL_MutexGive(&pCanCtrl->TxMutex);
                        if (Ret != KAL_ERR_NONE) {
                            /* should never happen*/
                        }
                        Ret = CAN_ERR_NONE;
                    }
                } else {
                    Ret = AmbaKAL_MutexGive(&(pCanCtrl->TxMutex));
                    if (Ret != KAL_ERR_NONE) {
                        /* should never happen*/
                    }
                    Ret = CAN_ERR_MUTEX;
                }
            } else {
                Ret = CAN_ERR_MUTEX;
            }
        }
    }

    return Ret;
}

/**
 * This function is used to receive a CAN message
 * @param[in] CanCh Channel ID
 * @param[out] pMessage Pointer to receiving contents
 * @param[in] Timeout Receiving timeout in ms
 * @return 0-OK, other-NG
 */
UINT32 AmbaCAN_Read(UINT32 CanCh, AMBA_CAN_MSG_s *pMessage, UINT32 Timeout)
{
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl;
    UINT32 Ret;

    if ((CanCh >= AMBA_NUM_CAN_CHANNEL) || (pMessage == NULL)) {
        Ret = CAN_ERR_ARG;
    } else {
        /*-----------------------------------------------------------------------*\
         * Take the Mutex
        \*-----------------------------------------------------------------------*/
        if (AmbaKAL_MutexTake(&pCanCtrl->RxMutex, AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
            /* check the Counting Semaphore to see if there is any Data left in Rx Ring Buffer */
            if (AmbaKAL_SemaphoreTake(&pCanCtrl->RxSem, Timeout) == KAL_ERR_NONE) {
                /* CAN Read */
                Ret = AmbaRTSL_CanReadOneRxTuple(CanCh, pMessage);
            } else {
                Ret = CAN_ERR_TIMEOUT;
            }

            /*-----------------------------------------------------------------------*\
             * Release the Mutex
            \*-----------------------------------------------------------------------*/
            (VOID) AmbaKAL_MutexGive(&(pCanCtrl->RxMutex));

        } else {
            Ret = CAN_ERR_MUTEX;
        }
    }

    return Ret;
}

/**
 * This function is used to receive a CAN FD message
 * @param[in] CanCh Channel ID
 * @param[out] pFdMessage Pointer to receiving contents
 * @param[in] Timeout Receiving timeout in ms
 * @return 0-OK, other-NG
 */
UINT32 AmbaCAN_ReadFd(UINT32 CanCh, AMBA_CAN_FD_MSG_s *pFdMessage, UINT32 Timeout)
{
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl;
    UINT32 Ret;

    if ((CanCh >= AMBA_NUM_CAN_CHANNEL) || (pFdMessage == NULL)) {
        Ret = CAN_ERR_ARG;
    } else {
        /*-----------------------------------------------------------------------*\
         * Take the Mutex
        \*-----------------------------------------------------------------------*/
        if (AmbaKAL_MutexTake(&pCanCtrl->FdRxMutex, AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
            /* check the Counting Semaphore to see if there is any Data left in Rx Ring Buffer */
            if (AmbaKAL_SemaphoreTake(&pCanCtrl->FdRxSem, Timeout) == KAL_ERR_NONE) {
                /* CAN Read */
                Ret = AmbaRTSL_CanFdReadOneRxTuple(CanCh, pFdMessage);
            } else {
                Ret = CAN_ERR_TIMEOUT;
            }

            /*-----------------------------------------------------------------------*\
             * Release the Mutex
            \*-----------------------------------------------------------------------*/
            (VOID) AmbaKAL_MutexGive(&pCanCtrl->FdRxMutex);

        } else {
            Ret = CAN_ERR_MUTEX;
        }
    }

    return Ret;
}

static UINT32 CAN_MsgBufAllocate(UINT8 IsTxMsgBuf, UINT32 Timeout, UINT32 *MsgBufNo)
{
    UINT32 MsgBufMask;
    UINT32 CurFlags;
    UINT32 Ret;
    UINT32 i;

    MsgBufMask = (IsTxMsgBuf != 0U) ? AmbaCAN_Ctrl.TxEventMask : AmbaCAN_Ctrl.RxEventMask;

    Ret = AmbaKAL_EventFlagGet(&AmbaCAN_Ctrl.EventFlag, MsgBufMask, 0U, 0U, &CurFlags, Timeout);
    if (Ret == KAL_ERR_NONE) {
        CurFlags &= MsgBufMask;
        for (i = 0; i < AMBA_NUM_CAN_MSG_BUF; i++) {
            if ((CurFlags & 0x1U) != 0U) {
                UINT32 ClearBit = 1UL << i;
                (VOID) AmbaKAL_EventFlagClear(&AmbaCAN_Ctrl.EventFlag, ClearBit);
                *MsgBufNo = i;
                Ret = CAN_ERR_NONE;
                break;
            }
            CurFlags >>= 1U;
        }
        if (i == AMBA_NUM_CAN_MSG_BUF) {
            Ret = CAN_ERR_NO_TX_BUFFER;
        }
    } else {
        Ret = CAN_ERR_TIMEOUT;
    }

    return Ret;
}

/**
 * This function is used to send a CAN message.
 * @param[in] CanCh Channel ID
 * @param[in] pMessage Pointer to sending contents
 * @param[in] Timeout Sending timeout in ms
 * @return 0-OK, other-NG
 */
UINT32 AmbaCAN_Write(UINT32 CanCh, const AMBA_CAN_MSG_s *pMessage, UINT32 Timeout)
{
    UINT32 Ret, MsgBufNo;

    if ((CanCh >= AMBA_NUM_CAN_CHANNEL) || (pMessage == NULL)) {
        Ret = CAN_ERR_ARG;
    } else if (pMessage->DataLengthCode > AMBA_CAN_DATA_LENGTH_8) {
        Ret = CAN_ERR_ARG;
    } else {
        /*-----------------------------------------------------------------------*\
         * Take the Mutex
        \*-----------------------------------------------------------------------*/
        if (AmbaKAL_MutexTake(&AmbaCAN_Ctrl.TxMutex, AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
            /* Get one avalable Tx message buffer */
            Ret = CAN_MsgBufAllocate(1, Timeout, &MsgBufNo);
            if (Ret == CAN_ERR_NONE) {
                /* CAN Write */
                Ret = AmbaRTSL_CanTxTransfer(CanCh, MsgBufNo, pMessage);
                if (Ret != CAN_ERR_NONE) {
                    UINT32 GetBit = 1UL << MsgBufNo;
                    (VOID) AmbaKAL_EventFlagSet(&AmbaCAN_Ctrl.EventFlag, GetBit);
                }
            }

            /*-----------------------------------------------------------------------*\
             * Release the Mutex
            \*-----------------------------------------------------------------------*/
            (VOID) AmbaKAL_MutexGive(&AmbaCAN_Ctrl.TxMutex);

        } else {
            Ret = CAN_ERR_MUTEX;
        }
    }

    return Ret;
}

/**
 * This function is used to send a CAN FD message.
 * @param[in] CanCh Channel ID
 * @param[in] pFdMessage Pointer to sending contents
 * @param[in] Timeout Sending timeout in ms
 * @return 0-OK, other-NG
 */
UINT32 AmbaCAN_WriteFd(UINT32 CanCh, const AMBA_CAN_FD_MSG_s *pFdMessage, UINT32 Timeout)
{
    UINT32 Ret, MsgBufNo;

    if ((CanCh >= AMBA_NUM_CAN_CHANNEL) || (pFdMessage == NULL)) {
        Ret = CAN_ERR_ARG;
    } else if (pFdMessage->DataLengthCode >= AMBA_NUM_CAN_DATA_LENGTH) {
        Ret = CAN_ERR_ARG;
    } else {
        /*-----------------------------------------------------------------------*\
         * Take the Mutex
        \*-----------------------------------------------------------------------*/
        if (AmbaKAL_MutexTake(&(AmbaCAN_Ctrl.TxMutex), AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
            /* Get one avalable Tx message buffer */
            Ret = CAN_MsgBufAllocate(1, Timeout, &MsgBufNo);
            /* CAN Write */
            if (Ret == CAN_ERR_NONE) {
                Ret = AmbaRTSL_CanFdTxTransfer(CanCh, MsgBufNo, pFdMessage);
                if (Ret != CAN_ERR_NONE) {
                    UINT32 GetBit = 1UL << MsgBufNo;
                    (VOID) AmbaKAL_EventFlagSet(&AmbaCAN_Ctrl.EventFlag, GetBit);
                }
            }

            /*-----------------------------------------------------------------------*\
             * Release the Mutex
            \*-----------------------------------------------------------------------*/
            (VOID) AmbaKAL_MutexGive(&AmbaCAN_Ctrl.TxMutex);
        } else {
            Ret = CAN_ERR_MUTEX;
        }
    }

    return Ret;
}

/**
 * This function is used to stop a specific CAN controller.
 * @param[in] CanCh Channel ID
 * @return 0-OK, other-NG
 */
UINT32 AmbaCAN_Disable(UINT32 CanCh)
{
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl;
    AMBA_CAN_REG_s *pCanReg = pAmbaCAN_Reg[CanCh];
    UINT32 Ret;

    AmbaCSL_CanDisable(pCanReg);

    Ret = AmbaKAL_SemaphoreDelete(&pCanCtrl->RxSem);
    if (Ret != KAL_ERR_NONE) {
        /* should never happen*/
    }
    Ret = AmbaKAL_SemaphoreDelete(&pCanCtrl->FdRxSem);
    if (Ret != KAL_ERR_NONE) {
        /* should never happen*/
    }
    Ret = AmbaKAL_MutexDelete(&pCanCtrl->RxMutex);
    if (Ret != KAL_ERR_NONE) {
        /* should never happen*/
    }
    Ret = AmbaKAL_MutexDelete(&pCanCtrl->FdRxMutex);
    if (Ret != KAL_ERR_NONE) {
        /* should never happen*/
    }
    Ret = AmbaKAL_MutexDelete(&pCanCtrl->TxMutex);
    if (Ret != KAL_ERR_NONE) {
        /* should never happen*/
    }
    Ret = AmbaKAL_EventFlagDelete(&pCanCtrl->EventFlag);
    if (Ret != KAL_ERR_NONE) {
        /* should never happen*/
    }
    if (AmbaWrap_memset(pCanCtrl, 0x0, sizeof(AMBA_CAN_CTRL_s)) != 0U) {
        /* Do nothing */
    }

    return Ret;
}

/**
 * This function is used to acquire a specific CAN controller's configuration,
 * which contains bit setting, filter number and filter info.
 * @param[in] CanCh Channel ID
 * @param[out] pBitInfo Pointer to the bit setting
 * @param[out] pNumFilter Pointer to the filter number
 * @param[out] pFilter Channel ID Pointer to the filter info
 * @return 0-OK, other-NG
 */
UINT32 AmbaCAN_GetInfo(UINT32 CanCh, AMBA_CAN_BIT_INFO_s * pBitInfo, UINT32 * pNumFilter, AMBA_CAN_FILTER_s * pFilter)
{
    return AmbaRTSL_CanGetInfo(CanCh, pBitInfo, pNumFilter, pFilter);
}
