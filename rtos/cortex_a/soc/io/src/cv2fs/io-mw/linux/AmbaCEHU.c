/**
 *  @file AmbaCHEU.c
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
 *  @details CEHU Control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
//#include "AmbaCEHU.h"

static AMBA_KAL_MUTEX_t AmbaCehuMutex;
typedef void (*AMBA_CEHU_INT_CALLBACK_f)(UINT32 InstanceID);
/**
 *  Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaCEHU_DrvEntry(void)
{
    static char AmbaCehuMutexName[14] = "AmbaCehuMutex";
    UINT32 uret = 1;

    if (AmbaKAL_MutexCreate(&AmbaCehuMutex, AmbaCehuMutexName) != KAL_ERR_NONE) {
        uret = 3;
    }

    return uret;
}

/**
 * Enable mask for the CEHU Error ID. CEHU won't report the error.
 * @param InstanceID [IN] CEHU instance ID.
 * @param ErrorID    [IN] Error ID.
*/
UINT32 AmbaCEHU_MaskEnable(UINT32 InstanceID, UINT32 ErrorID)
{
    UINT32 uret = 0;
    (void) InstanceID;
    (void) ErrorID;

    if (AmbaKAL_MutexTake(&AmbaCehuMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        uret = 1;
    } else {
        //uret = AmbaRTSL_CEHUMaskEnable(InstanceID, ErrorID);
        if (AmbaKAL_MutexGive(&AmbaCehuMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            uret = 3;
        }
    }
    return uret;
}

/**
 * Disable mask for the CEHU Error ID. CEHU will report the error.
 * @param InstanceID [IN] CEHU instance ID.
 * @param ErrorID    [IN] Error ID.
*/
UINT32 AmbaCEHU_MaskDisable(UINT32 InstanceID, UINT32 ErrorID)
{
    UINT32 uret = 0;
    (void) InstanceID;
    (void) ErrorID;

    if (AmbaKAL_MutexTake(&AmbaCehuMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        uret = 1;
    } else {
        //uret = AmbaRTSL_CEHUMaskDisable(InstanceID, ErrorID);
        if (AmbaKAL_MutexGive(&AmbaCehuMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            uret = 3;
        }
    }
    return uret;
}

/**
 * Get the mask value of the CEHU Error ID.
 * @param InstanceID [IN] CEHU instance ID.
 * @param ErrorID    [IN] Error ID.
 * @param Value      [OUT] The mask value. 0: no masked. 1: masked
*/
UINT32 AmbaCEHU_MaskGet(UINT32 InstanceID, UINT32 ErrorID, UINT32 *Value)
{
    UINT32 uret = 0;
    (void) InstanceID;
    (void) ErrorID;
    (void) Value;

    if (AmbaKAL_MutexTake(&AmbaCehuMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        uret = 1;
    } else {
        //uret = AmbaRTSL_CEHUMaskGet(InstanceID, ErrorID, Value);
        if (AmbaKAL_MutexGive(&AmbaCehuMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            uret = 3;
        }
    }
    return uret;
}

/**
 * Get the error value of the CEHU Error ID.
 * @param InstanceID [IN] CEHU instance ID.
 * @param ErrorID    [IN] Error ID.
 * @param Value      [OUT] The value of the CEHU Error ID. 0: No error. 1: Error occurred
*/
UINT32 AmbaCEHU_ErrorGet(UINT32 InstanceID, UINT32 ErrorID, UINT32 *Value)
{
    UINT32 uret = 0;
    (void) InstanceID;
    (void) ErrorID;
    (void) Value;

    if (AmbaKAL_MutexTake(&AmbaCehuMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        uret = 1;
    } else {
        //uret = AmbaRTSL_CEHUErrorGet(InstanceID, ErrorID, Value);
        if (AmbaKAL_MutexGive(&AmbaCehuMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            uret = 3;
        }
    }
    return uret;
}

/**
 * Clear the error value of the CEHU Error ID.
 * @param InstanceID [IN] CEHU instance ID.
 * @param ErrorID    [IN] Error ID.
 */
UINT32 AmbaCEHU_ErrorClear(UINT32 InstanceID, UINT32 ErrorID)
{
    UINT32 uret = 0;
    (void) InstanceID;
    (void) ErrorID;

    if (AmbaKAL_MutexTake(&AmbaCehuMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        uret = 1;
    } else {
        //uret = AmbaRTSL_CEHUErrorClear(InstanceID, ErrorID);
        if (AmbaKAL_MutexGive(&AmbaCehuMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            uret = 3;
        }
    }
    return uret;
}

/**
 * Get the safety mode of the CEHU Error ID.
 * @param InstanceID [IN] CEHU instance ID.
 * @param ErrorID    [IN] Error ID.
 * @param Value      [OUT] Safety mode.<br>
 *                          - bit[0]: Error will be routed to GIC if its value is 1.
 *                          - bit[1]: Error will be routed to safety pin if its value is 1.
 *                          - bit[31:2]: Unused and reserved.
 */
UINT32 AmbaCEHU_SafetyModeGet(UINT32 InstanceID, UINT32 ErrorID, UINT32 *Value)
{
    UINT32 uret = 0;
    (void) InstanceID;
    (void) ErrorID;
    (void) Value;

    if (AmbaKAL_MutexTake(&AmbaCehuMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        uret = 1;
    } else {
        //uret = AmbaRTSL_CEHUSafetyModeGet(InstanceID, ErrorID, Value);
        if (AmbaKAL_MutexGive(&AmbaCehuMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            uret = 3;
        }
    }
    return uret;
}

/**
 * Set the safety mode of the CEHU Error ID.
 * @param InstanceID [IN] CEHU instance ID.
 * @param ErrorID    [IN] Error ID.
 * @param Value      [IN] Safety mode.<br>
 *                          - bit 0: Error will be routed to GIC if its value is 1.
 *                          - bit 1: Error will be routed to safety pin if its value is 1.
 *                          - bit[31:2]: Unused and reserved.
 */
UINT32 AmbaCEHU_SafetyModeSet(UINT32 InstanceID, UINT32 ErrorID, UINT32 Value)
{
    UINT32 uret = 0;
    (void) InstanceID;
    (void) ErrorID;
    (void) Value;

    if (AmbaKAL_MutexTake(&AmbaCehuMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        uret = 1;
    } else {
        //uret = AmbaRTSL_CEHUSafetyModeSet(InstanceID, ErrorID, Value);
        if (AmbaKAL_MutexGive(&AmbaCehuMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            uret = 3;
        }
    }

    return uret;
}

/**
 * Disable CEHU interrupt.
*/
UINT32 AmbaCEHU_InterruptDisable(void)
{

    UINT32 uret = 0;

    if (AmbaKAL_MutexTake(&AmbaCehuMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        uret = 1;
    } else {
        //uret = AmbaRTSL_CEHUInterruptDisable();
        if (AmbaKAL_MutexGive(&AmbaCehuMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            uret = 3;
        }
    }

    return uret;
}

/**
 * Enable CEHU interrupt.
*/
UINT32 AmbaCEHU_InterruptEnable(void)
{
    UINT32 uret = 0;

    if (AmbaKAL_MutexTake(&AmbaCehuMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        uret = 1;
    } else {
        //uret = AmbaRTSL_CEHUInterruptEnable();
        if (AmbaKAL_MutexGive(&AmbaCehuMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            uret = 3;
        }
    }

    return uret;
}

/**
 * Register callback function for receiving CEHU interrupt event.
 * Note the callback function is in IRS context.
 * @param pCallBackFunc [IN] callback function for receiving CEHU interrupt event
*/
UINT32 AmbaCEHU_IntCallbackRegister(AMBA_CEHU_INT_CALLBACK_f pCallBackFunc)
{

    UINT32 uret = 0;
    (void) pCallBackFunc;

    if (AmbaKAL_MutexTake(&AmbaCehuMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        uret = 1;
    } else {
        //uret = AmbaRTSL_CEHUIntCbkRegister(pCallBackFunc);
        if (AmbaKAL_MutexGive(&AmbaCehuMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            uret = 3;
        }
    }

    return uret;
}
