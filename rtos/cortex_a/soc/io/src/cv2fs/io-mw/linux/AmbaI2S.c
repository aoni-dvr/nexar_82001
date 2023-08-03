/**
 *  @file AmbaI2S.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details I2S APIs
 *
 */

#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaI2S.h"
//#include "AmbaRTSL_I2S.h"

static AMBA_KAL_MUTEX_t AmbaI2sMutex[0];

/**
 *  AmbaI2S_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaI2S_DrvEntry(void)
{
    static char AmbaI2sMutexName[16] = "AmbaI2sMutex";
    UINT32 RetVal = I2S_ERR_NONE;
    UINT32 i;

    /* Create Mutex */
    for (i = 0U; i < AMBA_NUM_I2S_CHANNEL; i++) {
        if (AmbaKAL_MutexCreate(&AmbaI2sMutex[i], AmbaI2sMutexName) != KAL_ERR_NONE) {
            RetVal = I2S_ERR_MUTEX;
            break;
        }
    }

    return RetVal;
}

/**
 *  AmbaI2S_Config - Configure I2S
 *  @param[in] I2sPortIdx I2S port number
 *  @param[in] pCtrl I2S control information
 *  @return error code
 */
UINT32 AmbaI2S_Config(UINT32 I2sPortIdx, const AMBA_I2S_CTRL_s *pCtrl)
{
    UINT32 RetVal = I2S_ERR_NONE;
    (void) pCtrl;

    if (AmbaKAL_MutexTake(&AmbaI2sMutex[I2sPortIdx], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = I2S_ERR_MUTEX;
    } else {
        //RetVal = AmbaRTSL_I2sConfig(I2sPortIdx, pCtrl);
        (void)AmbaKAL_MutexGive(&AmbaI2sMutex[I2sPortIdx]);
    }

    return RetVal;
}

/**
 *  AmbaI2S_RxTrigger - I2S Rx enable / disable control
 *  @param[in] I2sPortIdx I2S port number
 *  @param[in] Cmd 0 - Disable I2S Rx, 1 - Enable I2S Rx
 *  @return error code
 */
UINT32 AmbaI2S_RxTrigger(UINT32 I2sPortIdx, UINT32 Cmd)
{
    UINT32 RetVal = I2S_ERR_NONE;
    (void) Cmd;

    if (AmbaKAL_MutexTake(&AmbaI2sMutex[I2sPortIdx], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = I2S_ERR_MUTEX;
    } else {
        //RetVal = AmbaRTSL_I2sRxTrigger(I2sPortIdx, Cmd);
        (void)AmbaKAL_MutexGive(&AmbaI2sMutex[I2sPortIdx]);
    }

    return RetVal;
}

/**
 *  AmbaI2S_TxTrigger - I2S Tx enable / disable control
 *  @param[in] I2sPortIdx I2S port number
 *  @param[in] Cmd 0 - Disable I2S Rx, 1 - Enable I2S Rx
 *  @return error code
 */
UINT32 AmbaI2S_TxTrigger(UINT32 I2sPortIdx, UINT32 Cmd)
{
    UINT32 RetVal = I2S_ERR_NONE;
    (void) Cmd;
    if (AmbaKAL_MutexTake(&AmbaI2sMutex[I2sPortIdx], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = I2S_ERR_MUTEX;
    } else {
        //RetVal = AmbaRTSL_I2sTxTrigger(I2sPortIdx, Cmd);
        if (Cmd == 0U) {
            //(void)AmbaRTSL_I2sTxWriteFifo(I2sPortIdx, 0U); /* Workaround to disable Tx when FIFO is empty */
            //(void)AmbaRTSL_I2sTxWriteFifo(I2sPortIdx, 0U);
            //(void)AmbaRTSL_I2sTxWriteFifo(I2sPortIdx, 0U);
            //(void)AmbaRTSL_I2sTxWriteFifo(I2sPortIdx, 0U);
        }
        (void)AmbaKAL_MutexGive(&AmbaI2sMutex[I2sPortIdx]);
    }

    return RetVal;
}

/**
 *  AmbaI2S_RxFifoReset - I2S Rx FIFO Reset
 *  @param[in] I2sPortIdx I2S port number
 *  @return error code
 */
UINT32 AmbaI2S_RxResetFifo(UINT32 I2sPortIdx)
{
    UINT32 RetVal = I2S_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaI2sMutex[I2sPortIdx], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = I2S_ERR_MUTEX;
    } else {
        //RetVal = AmbaRTSL_I2sRxResetFifo(I2sPortIdx);
        (void)AmbaKAL_MutexGive(&AmbaI2sMutex[I2sPortIdx]);
    }

    return RetVal;
}

/**
 *  AmbaI2S_TxFifoReset - I2S Tx FIFO Reset
 *  @param[in] I2sPortIdx I2S port number
 *  @return error code
 */
UINT32 AmbaI2S_TxResetFifo(UINT32 I2sPortIdx)
{
    UINT32 RetVal = I2S_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaI2sMutex[I2sPortIdx], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = I2S_ERR_MUTEX;
    } else {
        //RetVal = AmbaRTSL_I2sTxResetFifo(I2sPortIdx);
        (void)AmbaKAL_MutexGive(&AmbaI2sMutex[I2sPortIdx]);
    }

    return RetVal;
}
