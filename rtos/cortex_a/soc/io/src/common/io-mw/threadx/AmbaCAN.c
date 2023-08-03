/**
 *  @file AmbaCAN.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details CAN bus control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaCAN.h"
#include "AmbaRTSL_CAN.h"
#include <AmbaMisraFix.h>

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define AMBA_CAN_TX_BUFFER_SIZE  3U          /* Reserve more than 1 Tx buffer for auto answering, the reset are Rx buffers */
#else
#define AMBA_CAN_TX_BUFFER_SIZE  2U          /* The number of message buffers are acted as Tx buffers, the rest are Rx buffers. */
#endif
#define AMBA_CAN_EVENT_MASK      0xffffU    /* Indicate the CAN job completion */

#define FLAG_CAN_API_FLOW_UNINIT    0x0000U
#define FLAG_CAN_API_FLOW_START     0x0001U

typedef struct {
    AMBA_KAL_EVENT_FLAG_t   EventFlag;      /* Event Flags */
    UINT32                  RxEventMask;
    UINT32                  TxEventMask;
    AMBA_KAL_MUTEX_t        RxMutex;
    AMBA_KAL_MUTEX_t        FdRxMutex;
    AMBA_KAL_MUTEX_t        TxMutex;
    AMBA_KAL_SEMAPHORE_t    RxSem;          /* Rx Counting Semaphore */
    AMBA_KAL_SEMAPHORE_t    FdRxSem;        /* Rx Counting Semaphore for CAN FD messages */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT8                   EnableAa;       /* Flag to record if user enable auto answer mode */
#endif
} AMBA_CAN_CTRL_s;

typedef struct {
    UINT32  EnableNum;
    UINT32  EnableMap[AMBA_NUM_CAN_CHANNEL];
} AMBA_CAN_STATE_s;

typedef struct {
    UINT32  CanCh;
    UINT32  MsgBufNo;
} AMBA_CAN_TX_COMPL_MSG_s;

static AMBA_CAN_CTRL_s AmbaCAN_Ctrl[AMBA_NUM_CAN_CHANNEL];  /* CAN Management Structure */
static AMBA_CAN_STATE_s AmbaCAN_State;  /* CAN Management Structure */
static UINT32           flag_can_api[AMBA_NUM_CAN_CHANNEL] = {FLAG_CAN_API_FLOW_UNINIT};

#define CAN_MQ_SIZE           4096
static AMBA_KAL_MSG_QUEUE_t   CanTxComplMq[AMBA_NUM_CAN_CHANNEL];
static UINT32                 TrackIdBuf[AMBA_NUM_CAN_CHANNEL][AMBA_CAN_TX_BUFFER_SIZE];

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
static UINT32           CanSafeState[AMBA_NUM_CAN_CHANNEL] = {0U};      /**< safe state */
#endif



static void CAN_RxISR(UINT32 CanCh, UINT32 MsgBufNo)
{
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl[CanCh];
    UINT32 Ret;
    (VOID) MsgBufNo;

    /* increase the counting semaphore */
    Ret = AmbaKAL_SemaphoreGive(&pCanCtrl->RxSem);
    if (Ret != KAL_ERR_NONE) {
        /* should never happen*/
    }
}

static void CAN_TxISR(UINT32 CanCh, UINT32 MsgBufNo)
{
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl[CanCh];
    UINT32 SetBit = (UINT32)(1UL << MsgBufNo);
    AMBA_CAN_TX_COMPL_MSG_s msg = {.CanCh = CanCh, .MsgBufNo = MsgBufNo};

    if (0U == AmbaKAL_MsgQueueSend(&CanTxComplMq[CanCh], &msg, AMBA_KAL_NO_WAIT)) {
        /* Do nothing */
    }

    /* Set CAN Done Event Flag */
    (VOID) AmbaKAL_EventFlagSet(&pCanCtrl->EventFlag, SetBit);
}

static void CAN_FdRxISR(UINT32 CanCh, UINT32 MsgBufNo)
{
    UINT32 Ret;
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl[CanCh];
    (VOID) MsgBufNo;

    /* increase the counting semaphore */
    Ret = AmbaKAL_SemaphoreGive(&(pCanCtrl->FdRxSem));
    if (Ret != KAL_ERR_NONE) {
        /* should never happen*/
    }
}

static UINT32 CAN_Init(UINT32 CanCh)
{
    static char CanEventFlagStr[]     = "CAN Event_Flag";
    static char CanTxMutexStr[]       = "CAN Tx Mutex";
    static char CanRxMutexStr[]       = "CAN Rx Mutex";
    static char CanFDRxMutexStr[]     = "CAN FD Rx Mutex";
    static char CanRxSemaphoreStr[]   = "CAN Rx Semaphore";
    static char CanFDRxSemaphoreStr[] = "CAN FD Rx Semaphore";
    static char CanTxComplMqStr[]     = "CAN Tx Compl Mq";
    static UINT8 CanTxComplMqBuf[AMBA_NUM_CAN_CHANNEL][CAN_MQ_SIZE];
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl[CanCh];
    UINT32 Ret;

    /* Create Event Flags */
    Ret = AmbaKAL_EventFlagCreate(&pCanCtrl->EventFlag, CanEventFlagStr);

    if (Ret == KAL_ERR_NONE) {
        /* Signal the Event Flags of All Available Message Buffers */
        Ret = AmbaKAL_EventFlagSet(&(pCanCtrl->EventFlag), AMBA_CAN_EVENT_MASK);
    }
    if (Ret == KAL_ERR_NONE) {
        /* Create Tx Mutex */
        Ret = AmbaKAL_MutexCreate(&pCanCtrl->TxMutex, CanTxMutexStr);
    }
    if (Ret == KAL_ERR_NONE) {
        /* Create Rx Mutex */
        Ret = AmbaKAL_MutexCreate(&pCanCtrl->RxMutex, CanRxMutexStr);
    }
    if (Ret == KAL_ERR_NONE) {
        /* Create FD Rx Mutex */
        Ret = AmbaKAL_MutexCreate(&pCanCtrl->FdRxMutex, CanFDRxMutexStr);
    }
    if (Ret == KAL_ERR_NONE) {
        /* Create Rx Semaphore */
        Ret = AmbaKAL_SemaphoreCreate(&pCanCtrl->RxSem, CanRxSemaphoreStr, 0);
    }
    if (Ret == KAL_ERR_NONE) {
        /* Create FD Rx Semaphore */
        Ret = AmbaKAL_SemaphoreCreate(&pCanCtrl->FdRxSem, CanFDRxSemaphoreStr, 0);
    }
    if (Ret == KAL_ERR_NONE) {
        /* Create Message Queue for Tx Complete */
        Ret = AmbaKAL_MsgQueueCreate(&CanTxComplMq[CanCh],
                                     CanTxComplMqStr,
                                     (UINT32)sizeof(AMBA_CAN_TX_COMPL_MSG_s),
                                     &CanTxComplMqBuf[CanCh],
                                     CAN_MQ_SIZE);
    }

    if (Ret == KAL_ERR_NONE) {
        Ret = AmbaRTSL_CanInit(CanCh);
    } else {
        Ret = CAN_ERR_ARG;
    }

    if (Ret == CAN_ERR_NONE) {
        /* Set Tx/Rx callback functions when completed transactions */
        /* Just set one time, since all controller share the same handler */
        if (AmbaCAN_State.EnableNum == 0U) {
            AmbaRTSL_CanSetRxIsrFunc(CAN_RxISR);
            AmbaRTSL_CanFdSetRxIsrFunc(CAN_FdRxISR);
            AmbaRTSL_CanSetTxIsrFunc(CAN_TxISR);
        }
    }

    return Ret;
}

static UINT32 CAN_EnableImpl(UINT32 CanCh, const AMBA_CAN_CONFIG_s *pCanConfig)
{
    UINT32 Ret;
    UINT32 i;
    UINT32 MsgBufType = 0;
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl[CanCh];

    if (AmbaWrap_memset(pCanCtrl, 0x0, sizeof(AMBA_CAN_CTRL_s)) != 0U) {
        /* Do nothing */
    }

    Ret = CAN_Init(CanCh);
    if (Ret == CAN_ERR_NONE) {
        AMBA_CAN_REG_s *pCanReg = pAmbaCAN_Reg[CanCh];

        /* Reset CAN */
        AmbaCSL_CanReset(pCanReg, 1);
        AmbaCSL_CanReset(pCanReg, 0);

        if (pCanConfig->OpMode == AMBA_CAN_OP_MODE_LISTEN) {
            MsgBufType = 0;
        } else {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            pCanCtrl->EnableAa = pCanConfig->EnableAa;
#endif
            for (i = 0; i < AMBA_CAN_TX_BUFFER_SIZE; i++) {
                UINT32 SetBit = (UINT32)(1UL << i);
                MsgBufType |= SetBit;
            }
        }

        /* CAN Configurations */
        (VOID)AmbaRTSL_CanConfig(CanCh, pCanConfig, MsgBufType);

        /* Config Tx/Rx event flags and Rx semaphore */
        pCanCtrl->RxEventMask = MsgBufType ^ AMBA_CAN_EVENT_MASK;
        pCanCtrl->TxEventMask = MsgBufType & AMBA_CAN_EVENT_MASK;

        /* Enable CANC */
        AmbaCSL_CanEnable(pCanReg);

        AmbaCAN_State.EnableNum++;
        AmbaCAN_State.EnableMap[CanCh] = 1;
        flag_can_api[CanCh] |= FLAG_CAN_API_FLOW_START;

        Ret = CAN_ERR_NONE;

    }
    return Ret;
}

static UINT32 CAN_PrmCheck(const AMBA_CAN_CONFIG_s *pCanConfig)
{
    UINT32 Ret;

    if ((pCanConfig->TimeQuanta.BRP >= 256U) ||
           (pCanConfig->TimeQuanta.SJW >= 16U) ||
           ((pCanConfig->TimeQuanta.PropSeg + pCanConfig->TimeQuanta.PhaseSeg1) >= 64U) ||
           (pCanConfig->TimeQuanta.PhaseSeg2 >= 16U)) {
        Ret = CAN_ERR_INVALID_TQ;
    } else {
        Ret = CAN_ERR_NONE;
    }

    return Ret;
}

static UINT32 CAN_FdPrmCheck(const AMBA_CAN_CONFIG_s *pCanConfig)
{
    UINT32 Ret;
    if (pCanConfig->pFdRxRingBuf == NULL) {
        Ret = CAN_ERR_ARG;
    } else if ((pCanConfig->FdTimeQuanta.BRP >= 256U) ||
        (pCanConfig->FdTimeQuanta.SJW >= 16U)  ||
        ((pCanConfig->FdTimeQuanta.PropSeg + pCanConfig->FdTimeQuanta.PhaseSeg1) >= 32U) ||
        (pCanConfig->FdTimeQuanta.PhaseSeg2 >= 16U)) {
        Ret = CAN_ERR_INVALID_TQ;
    } else if ((pCanConfig->FdSsp.TdcEnable > 1U) ||
               (pCanConfig->FdSsp.Mode > 1U) ||
               (pCanConfig->FdSsp.Offset >= 256U) ||
               (pCanConfig->FdSsp.Position >= 256U)) {
        Ret = CAN_ERR_ARG;
    } else {
        Ret = CAN_ERR_NONE;
    }

    return Ret;
}

static UINT32 CAN_FilterListCheck(const AMBA_CAN_CONFIG_s *pCanConfig)
{
    UINT32 Ret = CAN_ERR_NONE, fIdx, Used = 0;
    const AMBA_CAN_FILTER_s *pIdFilter;

    if (pCanConfig->NumIdFilter > (AMBA_NUM_CAN_MSG_BUF - AMBA_CAN_TX_BUFFER_SIZE)) {
        Ret = CAN_ERR_ARG;
    } else if (pCanConfig->pIdFilter == NULL) {
        Ret = CAN_ERR_ARG;
    } else {
        // scan and check filter list
        for (fIdx = 0; fIdx < pCanConfig->NumIdFilter; fIdx++) {
            // check input arguments
            pIdFilter = &pCanConfig->pIdFilter[fIdx];
            if (pIdFilter->IdFilter > 0x1FFFFFFFU) { // [28:0]
                Ret = CAN_ERR_ARG;
            } else if (pIdFilter->IdMask > 0x1FFFFFFFU) { // [28:0]
                Ret = CAN_ERR_ARG;
            } else {
                Used += 1U;

                if (Used > (AMBA_NUM_CAN_MSG_BUF - AMBA_CAN_TX_BUFFER_SIZE)) { // not enough rx buffer
                    Ret = CAN_ERR_ARG;
                } else {
                    Ret = CAN_ERR_NONE;
                }
            }

            if (Ret != CAN_ERR_NONE) {
                break;
            }
        }
    }
    return Ret;
}

/**
* This function is used to start a specific CAN controller with given configurations
* @param [in]  Channel ID
* @param [in]  CAN configuration
* @return ErrorCode
*/
UINT32 AmbaCAN_Enable(UINT32 CanCh, const AMBA_CAN_CONFIG_s *pCanConfig)
{
    UINT32 Ret;
    UINT32 PrmChk = CAN_ERR_ARG;
    UINT32 FdPrmChk = CAN_ERR_NONE, FilterListChk = CAN_ERR_NONE;

    if (pCanConfig != NULL) {

        if ((pCanConfig->pRxRingBuf != NULL) && (pCanConfig->MaxRxRingBufSize > 0U)) {
            PrmChk = CAN_PrmCheck(pCanConfig);
        }

        if (pCanConfig->FdMaxRxRingBufSize > 0U) { // optional feature
            FdPrmChk = CAN_FdPrmCheck(pCanConfig);
        }

        if (pCanConfig->NumIdFilter > 0U) { // optional feature
            FilterListChk = CAN_FilterListCheck(pCanConfig);
        }
    }

    if ((CanCh >= AMBA_NUM_CAN_CHANNEL) || (pCanConfig == NULL)) {
        Ret = CAN_ERR_ARG;
    } else if (pCanConfig->OpMode >= AMBA_NUM_CAN_OP_MODE) {
        Ret = CAN_ERR_ARG;
    } else if (PrmChk != CAN_ERR_NONE) {
        Ret = PrmChk;
    } else if (FdPrmChk != CAN_ERR_NONE) {
        Ret = FdPrmChk;
    } else if (FilterListChk != CAN_ERR_NONE) {
        Ret = FilterListChk;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    } else if (CanSafeState[CanCh] != 0U) {
        Ret = CAN_ERR_SAFE_STATE;
#endif
    } else {
        if ((flag_can_api[CanCh] & FLAG_CAN_API_FLOW_START) == 0U) {
            Ret = CAN_EnableImpl(CanCh, pCanConfig);
        } else {
            Ret = CAN_ERR_FLOW;
        }
    }

    return Ret;
}

/**
* This function is used to receive a CAN message
* @param [in]  Channel ID
* @param [in/out]  Pointer to receiving contents
* @param [in]  Receiving timeout in ms
* @return ErrorCode
*/
UINT32 AmbaCAN_Read(UINT32 CanCh, AMBA_CAN_MSG_s *pMessage, UINT32 Timeout)
{
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl[CanCh];
    UINT32 Ret;

    if ((CanCh >= AMBA_NUM_CAN_CHANNEL) || (pMessage == NULL)) {
        Ret = CAN_ERR_ARG;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    } else if (CanSafeState[CanCh] != 0U) {
        Ret = CAN_ERR_SAFE_STATE;
#endif
    } else {
        if ((flag_can_api[CanCh] & FLAG_CAN_API_FLOW_START) != 0U) {
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
        } else {
            Ret = CAN_ERR_FLOW;
        }
    }

    return Ret;
}

/**
* This function is used to receive a CAN FD message
* @param [in]  Channel ID
* @param [in/out]  Pointer to receiving contents
* @param [in]  Receiving timeout in ms
* @return ErrorCode
*/
UINT32 AmbaCAN_ReadFd(UINT32 CanCh, AMBA_CAN_FD_MSG_s *pFdMessage, UINT32 Timeout)
{
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl[CanCh];
    UINT32 Ret;

    if ((CanCh >= AMBA_NUM_CAN_CHANNEL) || (pFdMessage == NULL)) {
        Ret = CAN_ERR_ARG;
    } else if (AmbaRTSL_CanGetFdMode(CanCh) == 0U) {
        Ret = CAN_ERR_NOT_ENABLE_CANFD;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    } else if (CanSafeState[CanCh] != 0U) {
        Ret = CAN_ERR_SAFE_STATE;
#endif
    } else {
        if ((flag_can_api[CanCh] & FLAG_CAN_API_FLOW_START) != 0U) {
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
        } else {
            Ret = CAN_ERR_FLOW;
        }
    }

    return Ret;
}

static UINT32 CAN_TxBufAllocate(UINT32 CanCh, UINT32 Timeout, UINT32 *MsgBufNo)
{
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl[CanCh];
    UINT32 MsgBufMask;
    UINT32 CurFlags;
    UINT32 Ret;
    UINT32 i;

    MsgBufMask = pCanCtrl->TxEventMask;

    Ret = AmbaKAL_EventFlagGet(&pCanCtrl->EventFlag, MsgBufMask, 0U, 0U, &CurFlags, Timeout);
    if (Ret == KAL_ERR_NONE) {
        CurFlags &= MsgBufMask;
        for (i = 0; i < AMBA_NUM_CAN_MSG_BUF; i++) {
            if ((CurFlags & 0x1U) != 0U) {
                UINT32 ClearBit = (UINT32)(1UL << i);
                (VOID) AmbaKAL_EventFlagClear(&pCanCtrl->EventFlag, ClearBit);
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
* @param [in]  Channel ID
* @param [in]  Pointer to sending contents
* @param [in]  Sending timeout in ms
* @return ErrorCode
*/
UINT32 AmbaCAN_Write(UINT32 CanCh, const AMBA_CAN_MSG_s *pMessage, UINT32 Timeout)
{
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl[CanCh];
    UINT32 Ret, MsgBufNo;

    if ((CanCh >= AMBA_NUM_CAN_CHANNEL) || (pMessage == NULL)) {
        Ret = CAN_ERR_ARG;
    } else if (pMessage->DataLengthCode > AMBA_CAN_DATA_LENGTH_8) {
        Ret = CAN_ERR_ARG;
    } else if (pMessage->RemoteTxReq > 0x1U) { // 1 bit
        Ret = CAN_ERR_ARG;
    } else if (pMessage->Priority > 0x3FU) { // 6 bits
        Ret = CAN_ERR_ARG;
    } else if ((pMessage->Extension == 1U) && (pMessage->Id > 0x1FFFFFFFU)) { // 29 bits for extension id
        Ret = CAN_ERR_ARG;
    } else if ((pMessage->Extension == 0U) && (pMessage->Id > 0x7FFU)) { // 11 bits for non-extension id
        Ret = CAN_ERR_ARG;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    } else if (CanSafeState[CanCh] != 0U) {
        Ret = CAN_ERR_SAFE_STATE;
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    } else if ((pMessage->AutoAnswer != 0U) && (pCanCtrl->EnableAa == 0U) ) {
        Ret = CAN_ERR_ARG;
    } else if ((pMessage->AutoAnswer != 0U) && (pMessage->RemoteTxReq == 1U) ) {
        Ret = CAN_ERR_ARG;
#endif
    } else {
        if ((flag_can_api[CanCh] & FLAG_CAN_API_FLOW_START) != 0U) {
            /*-----------------------------------------------------------------------*\
             * Take the Mutex
            \*-----------------------------------------------------------------------*/
            if (AmbaKAL_MutexTake(&pCanCtrl->TxMutex, AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
                /* Get one avalable Tx message buffer */
                Ret = CAN_TxBufAllocate(CanCh, Timeout, &MsgBufNo);
                if (Ret == CAN_ERR_NONE) {
                    /* CAN Write */
                    TrackIdBuf[CanCh][MsgBufNo] = pMessage->TrackID;
                    Ret = AmbaRTSL_CanTxTransfer(CanCh, MsgBufNo, pMessage);
                    if ((Ret != CAN_ERR_NONE) && (Ret != CAN_ERR_MIX_FORMAT)) {
                        UINT32 GetBit = (UINT32)(1UL << MsgBufNo);
                        (VOID) AmbaKAL_EventFlagSet(&pCanCtrl->EventFlag, GetBit);
                    }
                }

                /*-----------------------------------------------------------------------*\
                 * Release the Mutex
                \*-----------------------------------------------------------------------*/
                (VOID) AmbaKAL_MutexGive(&pCanCtrl->TxMutex);

            } else {
                Ret = CAN_ERR_MUTEX;
            }
        } else {
            Ret = CAN_ERR_FLOW;
        }
    }

    return Ret;
}

/**
* This function is used to send a CAN FD message.
* @param [in]  Channel ID
* @param [in]  Pointer to sending contents
* @param [in]  Sending timeout in ms
* @return ErrorCode
*/
UINT32 AmbaCAN_WriteFd(UINT32 CanCh, const AMBA_CAN_FD_MSG_s *pFdMessage, UINT32 Timeout)
{
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl[CanCh];
    UINT32 Ret, MsgBufNo;

    if ((CanCh >= AMBA_NUM_CAN_CHANNEL) || (pFdMessage == NULL)) {
        Ret = CAN_ERR_ARG;
    } else if (AmbaRTSL_CanGetFdMode(CanCh) == 0U) {
        Ret = CAN_ERR_NOT_ENABLE_CANFD;
    } else if (pFdMessage->DataLengthCode >= AMBA_NUM_CAN_DATA_LENGTH) {
        Ret = CAN_ERR_ARG;
    } else if (pFdMessage->Priority > 0x3FU) { // 6 bits
        Ret = CAN_ERR_ARG;
    } else if ((pFdMessage->Extension == 1U) && (pFdMessage->Id > 0x1FFFFFFFU)) { // 29 bits for extension id
        Ret = CAN_ERR_ARG;
    } else if ((pFdMessage->Extension == 0U) && (pFdMessage->Id > 0x7FFU)) { // 11 bits for non-extension id
        Ret = CAN_ERR_ARG;
    } else if (pFdMessage->BitRateSwitch > 0x1U) { // 1 bit
        Ret = CAN_ERR_ARG;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    } else if (CanSafeState[CanCh] != 0U) {
        Ret = CAN_ERR_SAFE_STATE;
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    } else if ((pFdMessage->AutoAnswer != 0U) && (pCanCtrl->EnableAa == 0U) ) {
        Ret = CAN_ERR_ARG;
#endif
    } else {
        if ((flag_can_api[CanCh] & FLAG_CAN_API_FLOW_START) != 0U) {
            /*-----------------------------------------------------------------------*\
             * Take the Mutex
            \*-----------------------------------------------------------------------*/
            if (AmbaKAL_MutexTake(&(pCanCtrl->TxMutex), AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
                /* Get one avalable Tx message buffer */
                Ret = CAN_TxBufAllocate(CanCh, Timeout, &MsgBufNo);
                if (Ret == CAN_ERR_NONE) {
                    /* CAN Write */
                    TrackIdBuf[CanCh][MsgBufNo] = pFdMessage->TrackID;
                    Ret = AmbaRTSL_CanFdTxTransfer(CanCh, MsgBufNo, pFdMessage);
                    if (Ret != CAN_ERR_NONE) {
                        UINT32 GetBit = (UINT32)(1UL << MsgBufNo);
                        (VOID) AmbaKAL_EventFlagSet(&pCanCtrl->EventFlag, GetBit);
                    }
                }

                /*-----------------------------------------------------------------------*\
                 * Release the Mutex
                \*-----------------------------------------------------------------------*/
                (VOID) AmbaKAL_MutexGive(&pCanCtrl->TxMutex);
            } else {
                Ret = CAN_ERR_MUTEX;
            }
        } else {
            Ret = CAN_ERR_FLOW;
        }
    }

    return Ret;
}

UINT32 AmbaCAN_WaitForTxComplete(UINT32 CanCh, UINT32 *TrackId)
{
    UINT32 Ret;
    AMBA_CAN_TX_COMPL_MSG_s msg;

    if (CanCh >= AMBA_NUM_CAN_CHANNEL) {
        Ret = CAN_ERR_ARG;
    } else if (TrackId == NULL) {
        Ret = CAN_ERR_ARG;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    } else if (CanSafeState[CanCh] != 0U) {
        Ret = CAN_ERR_SAFE_STATE;
#endif
    } else {
        if ((flag_can_api[CanCh] & FLAG_CAN_API_FLOW_START) != 0U) {
            if (0U == AmbaKAL_MsgQueueReceive(&CanTxComplMq[CanCh], &msg, AMBA_KAL_WAIT_FOREVER)) {
                *TrackId = TrackIdBuf[msg.CanCh][msg.MsgBufNo];
                Ret = CAN_ERR_NONE;
            } else {
                Ret = CAN_ERR_MUTEX;
            }
        } else {
            Ret = CAN_ERR_FLOW;
        }
    }
    return Ret;
}

/**
* This function is used to stop a specific CAN controller.
* @param [in]  Channel ID
* @return ErrorCode
*/
UINT32 AmbaCAN_Disable(UINT32 CanCh)
{
    AMBA_CAN_CTRL_s *pCanCtrl = &AmbaCAN_Ctrl[CanCh];
    AMBA_CAN_REG_s *pCanReg = pAmbaCAN_Reg[CanCh];
    UINT32 Ret;

    if (CanCh >= AMBA_NUM_CAN_CHANNEL) {
        Ret = CAN_ERR_ARG;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    } else if (CanSafeState[CanCh] != 0U) {
        Ret = CAN_ERR_SAFE_STATE;
#endif
    } else {
        if ((flag_can_api[CanCh] & FLAG_CAN_API_FLOW_START) != 0U) {
            AmbaCAN_State.EnableNum--;
            AmbaCAN_State.EnableMap[CanCh] = 0;

            AmbaCSL_CanDisable(pCanReg);

            Ret = AmbaKAL_MsgQueueDelete(&CanTxComplMq[CanCh]);
            if (Ret != KAL_ERR_NONE) {
                /* should never happen*/
            }

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


            /* clear the pointer of Call back function when completed transactions */
            if (AmbaCAN_State.EnableNum == 0U) {
                AmbaRTSL_CanSetRxIsrFunc(NULL);
                AmbaRTSL_CanFdSetRxIsrFunc(NULL);
                AmbaRTSL_CanSetTxIsrFunc(NULL);
            }

            flag_can_api[CanCh] &= ~FLAG_CAN_API_FLOW_START;
        } else {
            Ret = CAN_ERR_FLOW;
        }
    }

    return Ret;
}

/**
* This function is used to acquire a specific CAN controller's configuration,
* which contains bit setting, filter number and filter info.
* @param [in]  Channel ID
* @param [in/out]  Pointer to the bit setting
* @param [in/out]  Pointer to the filter number
* @param [in/out]  Channel ID Pointer to the filter info
* @return ErrorCode
*/
UINT32 AmbaCAN_GetInfo(UINT32 CanCh, AMBA_CAN_BIT_INFO_s * pBitInfo, UINT32 * pNumFilter, AMBA_CAN_FILTER_s * pFilter)
{
    UINT32 Ret;

    if ((CanCh >= AMBA_NUM_CAN_CHANNEL) || (pBitInfo == NULL) || (pNumFilter == NULL) || (pFilter == NULL)) {
        Ret = CAN_ERR_ARG;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    } else if (CanSafeState[CanCh] != 0U) {
        Ret = CAN_ERR_SAFE_STATE;
#endif
    } else {
        if ((flag_can_api[CanCh] & FLAG_CAN_API_FLOW_START) != 0U) {
            Ret =  AmbaRTSL_CanGetInfo(CanCh, pBitInfo, pNumFilter, pFilter);
        } else {
            Ret = CAN_ERR_FLOW;
        }
    }

    return Ret;
}
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
/**
 * This function is used to set current CAN controller safe state.
 *
 * @param[in] Channel ID
 * @param[in] 1: enter safe state, 0: leave safe state
 * @return error code
 */
UINT32 AmbaSafety_CanSetSafeState(UINT32 CanCh, UINT32 State)
{
    UINT32 Ret = CAN_ERR_NONE;

    if (CanCh >= AMBA_NUM_CAN_CHANNEL) {
        Ret = CAN_ERR_ARG;
    } else {
        CanSafeState[CanCh] = State;
        // Do something
    }

    return Ret;
}

/**
 * This function is used to get current CAN controller safe state.
 *
 * @param[in] Channel ID
 * @param[in/out] Current safe state
 * @return error code
 */
UINT32 AmbaSafety_CanGetSafeState(UINT32 CanCh, UINT32 *pState)
{
    UINT32 Ret = CAN_ERR_NONE;

    if (CanCh >= AMBA_NUM_CAN_CHANNEL) {
        Ret = CAN_ERR_ARG;
    } else if (pState == NULL) {
        Ret = CAN_ERR_ARG;
    } else {
        *pState = CanSafeState[CanCh];
    }

    return Ret;
}
#endif
