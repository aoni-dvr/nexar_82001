/**
 *  @file AmbaHDMI_CEC.c
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
 *  @details HDMI Consumer Electronics Control (CEC) APIs
 *
 */


#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaHDMI.h"
#include "AmbaHDMI_Ctrl.h"
#include "AmbaRTSL_HDMI.h"

static AMBA_KAL_MUTEX_t AmbaHdmiCecMutex;
static AMBA_KAL_EVENT_FLAG_t AmbaHdmiCecEventFlag;

#if 0
/**
 *  HDMI_CecIntHandler - ISR on CEC events
 *  @param[in] IntReason Interrupt Reason
 */
static void HDMI_CecIntHandler(UINT32 IntReason)
{
    (void)AmbaKAL_EventFlagSet(&AmbaHdmiCecEventFlag, IntReason);
}
#endif

/**
 *  AmbaHDMI_CecDrvEntry - CEC device driver initializations
 *  @return error code
 */
UINT32 AmbaHDMI_CecDrvEntry(void)
{
    static char AmbaHdmiCecEventFlagName[24] = "AmbaHdmiCecEventFlags";
    static char AmbaHdmiCecMutexName[20] = "AmbaHdmiCecMutex";
    UINT32 RetVal = HDMI_ERR_NONE;

    /* Create EventFlags */
    if (AmbaKAL_EventFlagCreate(&AmbaHdmiCecEventFlag, AmbaHdmiCecEventFlagName) != KAL_ERR_NONE) {
        RetVal = HDMI_ERR_UNEXPECTED;
    }

    /* Create Mutex */
    if (AmbaKAL_MutexCreate(&AmbaHdmiCecMutex, AmbaHdmiCecMutexName) != KAL_ERR_NONE) {
        RetVal = HDMI_ERR_UNEXPECTED;
    }

    return RetVal;
}

/**
 *  AmbaHDMI_CecEnable - Enable CEC module
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] LogicalAddr Logical address of the hdmi port
 *  @return error code
 */
UINT32 AmbaHDMI_CecEnable(UINT32 HdmiPort, UINT32 LogicalAddr)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiCecMutex;
    UINT32 RetVal = HDMI_ERR_NONE;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            AmbaRTSL_HdmiCecEnable();
            AmbaRTSL_HdmiCecSetLogicalAddr(LogicalAddr);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaHDMI_CecDisable - Disable CEC module
 *  @param[in] HdmiPort HDMI port ID
 *  @return error code
 */
UINT32 AmbaHDMI_CecDisable(UINT32 HdmiPort)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiCecMutex;
    UINT32 RetVal = HDMI_ERR_NONE;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            AmbaRTSL_HdmiCecDisable();

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaHDMI_CecWrite - Transmit one CEC message
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] pMsg CEC message
 *  @param[in] MsgSize Size of the CEC message (in Bytes)
 *  @return error code
 */
UINT32 AmbaHDMI_CecWrite(UINT32 HdmiPort, const UINT8 *pMsg, UINT32 MsgSize)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiCecMutex;
    UINT32 ActualFlags = 0U, DesiredFlags;
    UINT32 RetVal = HDMI_ERR_NONE;

    if (HdmiPort >= NUM_HDMI_TX_PORT) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            DesiredFlags = (HDMI_CEC_TX_OK | HDMI_CEC_TX_NG);
            (void)AmbaKAL_EventFlagClear(&AmbaHdmiCecEventFlag, DesiredFlags);

            /* Transmit Frame */
            while (AmbaRTSL_HdmiCecTransmit(pMsg, MsgSize) != OK) {
                /* Controller is busy. Wait 1 data bit period (2.4 ms) and then try again */
                (void)AmbaKAL_TaskSleep(3U);
            }

            /* Check TX status */
            if (AmbaKAL_EventFlagGet(&AmbaHdmiCecEventFlag, DesiredFlags, KAL_FLAGS_WAIT_ANY, KAL_FLAGS_CLEAR_AUTO, &ActualFlags, KAL_WAIT_FOREVER) != OK) {
                RetVal = HDMI_ERR_UNEXPECTED;
            } else {
                if ((ActualFlags & HDMI_CEC_TX_OK) == 0U) {
                    RetVal = HDMI_ERR_CEC;
                }
            }

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaHDMI_CecRead - Receive one CEC message
 *  @param[in] HdmiPort HDMI port ID
 *  @param[in] pMsg CEC message
 *  @param[out] pMsgSize Size of the CEC message (in Bytes)
 *  @return error code
 */
UINT32 AmbaHDMI_CecRead(UINT32 HdmiPort, UINT8 *pMsg, UINT32 *pMsgSize)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaHdmiCecMutex;
    UINT32 ActualFlags = 0U, DesiredFlags;
    UINT32 RetVal = HDMI_ERR_NONE;

    if ((HdmiPort >= NUM_HDMI_TX_PORT) || (pMsg == NULL) || (pMsgSize == NULL)) {
        RetVal = HDMI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = HDMI_ERR_MUTEX;
        } else {
            DesiredFlags = HDMI_CEC_RX_OK;
            (void)AmbaKAL_EventFlagClear(&AmbaHdmiCecEventFlag, DesiredFlags);

            /* Receive Frame */
            AmbaRTSL_HdmiCecReceive(pMsg, pMsgSize);

            /* Check RX status */
            if (AmbaKAL_EventFlagGet(&AmbaHdmiCecEventFlag, DesiredFlags, KAL_FLAGS_WAIT_ANY, KAL_FLAGS_CLEAR_AUTO, &ActualFlags, KAL_WAIT_FOREVER) != OK) {
                RetVal = HDMI_ERR_UNEXPECTED;
            } else {
                if ((ActualFlags & HDMI_CEC_RX_OK) == 0U) {
                    RetVal = HDMI_ERR_CEC;
                }
            }

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = HDMI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

