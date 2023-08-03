/**
 *  @file AmbaRTSL_CEHU.c
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
 *  @details CEHU Runtime Support Library APIs
 *
 */

#include <AmbaTypes.h>
#include <AmbaRTSL_CEHU.h>
#ifdef CONFIG_THREADX
#ifndef AMBA_RTSL_GIC_H
#include <AmbaRTSL_GIC.h>
#endif
#endif

#ifndef AMBA_CSL_CEHU_H
#include <AmbaCSL_CEHU.h>
#endif

static AMBA_CEHU_INT_CALLBACK_f int_callbacks[AMBA_NUM_RTSL_CEHU_CALLBACK] = {NULL, NULL, NULL};
static UINT32 int_callback_count = 0;

static void interrupt_dispatch(UINT32 InstanceID)
{
    UINT32 i;
    for (i = 0; i < int_callback_count; i++) {
        if (int_callbacks[i] != NULL) {
            int_callbacks[i](InstanceID);
        }
    }
}

static void interrupt_func_cehu0(UINT32 IntID, UINT32 UserArg)
{
    (void)UserArg;
    (void)IntID;
    interrupt_dispatch(0);
}

static void interrupt_func_cehu1(UINT32 IntID, UINT32 UserArg)
{
    (void)UserArg;
    (void)IntID;
    interrupt_dispatch(1);
}

#ifdef CONFIG_THREADX
// ThreadX to hook interrupt
static UINT32 interrupt_register(void)
{
    AMBA_INT_CONFIG_s interrupt_config;
    UINT32 uret;

    // config CEHU instance 0 interrupt
    interrupt_config.TriggerType = AMBA_INT_HIGH_LEVEL_TRIGGER;
    interrupt_config.IrqType = AMBA_INT_FIQ;
    interrupt_config.CpuTargets = 1U;
    uret = AmbaRTSL_GicIntConfig(AMBA_INT_SPI_ID032_CEHU0, &interrupt_config, interrupt_func_cehu0, 0U);
    if (uret == 0U) {
        // enable CEHU instance 0 interrupt
        uret = AmbaRTSL_GicIntEnable(AMBA_INT_SPI_ID032_CEHU0);
        if (uret != 0U) {
            uret = CEHU_ERR_INT_ENABLE;
        }
    } else {
        uret = CEHU_ERR_INT_CONFIG;
    }

    if (uret == 0U) {
        // config CEHU instance 1 interrupt
        interrupt_config.TriggerType = AMBA_INT_HIGH_LEVEL_TRIGGER;
        interrupt_config.IrqType = AMBA_INT_FIQ;
        interrupt_config.CpuTargets = 1U;
        uret = AmbaRTSL_GicIntConfig(AMBA_INT_SPI_ID033_CEHU1, &interrupt_config, interrupt_func_cehu1, 0U);
        if (uret == 0U) {
            // enable CEHU instance 1 interrupt
            uret = AmbaRTSL_GicIntEnable(AMBA_INT_SPI_ID033_CEHU1);
            if (uret != 0U) {
                uret = CEHU_ERR_INT_ENABLE;
            }
        } else {
            uret = CEHU_ERR_INT_CONFIG;
        }
    }

    return uret;
}

#else
// QNX or Linux to hook interrupt
static UINT32 interrupt_register(void)
{
    // TBD
    return 0;
}
#endif

/**
 * Register callback function for receiving CEHU interrupt event.
 * Note the callback function is in IRS context.
 * @param pCallBackFunc [IN] callback function for receiving CEHU interrupt event
 * @retval CEHU_ERR_REACH_MAX the number of registered callback function reaches maximum.
 * @retval 0 Success.
*/
UINT32 AmbaRTSL_CEHUIntCbkRegister(AMBA_CEHU_INT_CALLBACK_f pCallBackFunc)
{
    static UINT32 flag_init = 0;
    UINT32 uret = 0;

    if (flag_init == 0U) {
        uret = interrupt_register();
        flag_init = 1;
    }

    if (uret == 0U) {
        if (int_callback_count >= AMBA_NUM_RTSL_CEHU_CALLBACK) {
            uret = CEHU_ERR_REACH_MAX;
        } else {
            int_callbacks[int_callback_count] = pCallBackFunc;
            int_callback_count++;
        }
    }
    return uret;
}

/**
 * Disable CEHU interrupt.
 * @retval 0 Success.
*/
UINT32 AmbaRTSL_CEHUInterruptDisable(void)
{
#ifdef CONFIG_THREADX
    UINT32 uret = 0;
    UINT32 func_ret;
    func_ret = AmbaRTSL_GicIntDisable(AMBA_INT_SPI_ID032_CEHU0);
    if (func_ret != 0U) {
        uret = CEHU_ERR_INT_DISABLE;
    }
    func_ret = AmbaRTSL_GicIntDisable(AMBA_INT_SPI_ID033_CEHU1);
    if (func_ret != 0U) {
        uret = CEHU_ERR_INT_DISABLE;
    }
    return uret;
#else
    return 0;
#endif
}

/**
 * Enable CEHU interrupt.
 * @retval 0 Success.
*/
UINT32 AmbaRTSL_CEHUInterruptEnable(void)
{
#ifdef CONFIG_THREADX
    UINT32 uret = 0;
    UINT32 func_ret;
    func_ret = AmbaRTSL_GicIntEnable(AMBA_INT_SPI_ID032_CEHU0);
    if (func_ret != 0U) {
        uret = CEHU_ERR_INT_ENABLE;
    }
    func_ret = AmbaRTSL_GicIntEnable(AMBA_INT_SPI_ID033_CEHU1);
    if (func_ret != 0U) {
        uret = CEHU_ERR_INT_ENABLE;
    }
    return uret;
#else
    return 0;
#endif
}

/**
 * Enable mask for the CEHU Error ID. CEHU won't report the error.
 * @param InstanceID [IN] CEHU instance ID.
 * @param ErrorID    [IN] Error ID.
*/
UINT32 AmbaRTSL_CEHUMaskEnable(UINT32 InstanceID, UINT32 ErrorID)
{
    UINT32 uret = 0;
    if ((ErrorID < AMBA_NUM_CEHU_ERRORS) && (InstanceID < AMBA_NUM_CEHU_INSTANCES)) {
        UINT32 group = ErrorID / 32U;
        UINT32 idx   = ErrorID % 32U;
        UINT32 mask;
        UINT32 value_1 = 1U;

        mask = AmbaCSL_CEHUGetBitMask(InstanceID, group);
        mask = mask | (value_1 << idx);

        AmbaCSL_CEHUSetBitMask(InstanceID, group, mask);
    } else {
        uret = CEHU_ERR_PARAM;
    }
    return uret;
}

/**
 * Disable mask for the CEHU Error ID. CEHU will report the error.
 * @param InstanceID [IN] CEHU instance ID.
 * @param ErrorID    [IN] Error ID.
*/
UINT32 AmbaRTSL_CEHUMaskDisable(UINT32 InstanceID, UINT32 ErrorID)
{
    UINT32 uret = 0;
    if ((ErrorID < AMBA_NUM_CEHU_ERRORS) && (InstanceID < AMBA_NUM_CEHU_INSTANCES)) {
        UINT32 group = ErrorID / 32U;
        UINT32 idx   = ErrorID % 32U;
        UINT32 mask;
        UINT32 value_1 = 1U;

        mask = AmbaCSL_CEHUGetBitMask(InstanceID, group);
        mask = mask & (~(value_1 << idx));

        AmbaCSL_CEHUSetBitMask(InstanceID, group, mask);
    } else {
        uret = CEHU_ERR_PARAM;
    }
    return uret;
}

/**
 * Get the mask value of the CEHU Error ID.
 * @param InstanceID [IN] CEHU instance ID.
 * @param ErrorID    [IN] Error ID.
 * @param Value      [OUT] The mask value. 0: no masked. 1: masked
 * @retval CEHU_ERR_PARAM
 * @retval 0 Success
*/
UINT32 AmbaRTSL_CEHUMaskGet(UINT32 InstanceID, UINT32 ErrorID, UINT32 *Value)
{
    UINT32 uret = 0;
    if ((ErrorID < AMBA_NUM_CEHU_ERRORS) && (InstanceID < AMBA_NUM_CEHU_INSTANCES) && (Value != NULL)) {
        UINT32 group = ErrorID / 32U;
        UINT32 idx   = ErrorID % 32U;
        UINT32 mask;
        UINT32 value_1 = 1U;

        mask = AmbaCSL_CEHUGetBitMask(InstanceID, group);

        if ((mask & (value_1 << idx)) == 0U) {
            *Value = 0;
        } else {
            *Value = 1;
        }
    } else {
        uret = CEHU_ERR_PARAM;
    }
    return uret;
}

/**
 * Get the error value of the CEHU Error ID.
 * @param InstanceID [IN] CEHU instance ID.
 * @param ErrorID    [IN] Error ID.
 * @param Value      [OUT] The value of the CEHU Error ID. 0: No error. 1: Error occurred
*/
UINT32 AmbaRTSL_CEHUErrorGet(UINT32 InstanceID, UINT32 ErrorID, UINT32 *Value)
{
    UINT32 uret = 0;
    if ((ErrorID < AMBA_NUM_CEHU_ERRORS) && (InstanceID < AMBA_NUM_CEHU_INSTANCES) && (Value != NULL)) {
        UINT32 group = ErrorID / 32U;
        UINT32 idx   = ErrorID % 32U;
        UINT32 error_vector;
        UINT32 value_1 = 1U;

        error_vector = AmbaCSL_CEHUGetBitVector(InstanceID, group);

        if ((error_vector & (value_1 << idx)) == 0U) {
            *Value = 0;
        } else {
            *Value = 1;
        }
    } else {
        uret = CEHU_ERR_PARAM;
    }
    return uret;
}

/**
 * Clear the error value of the CEHU Error ID.
 * @param InstanceID [IN] CEHU instance ID.
 * @param ErrorID    [IN] Error ID.
 */
UINT32 AmbaRTSL_CEHUErrorClear(UINT32 InstanceID, UINT32 ErrorID)
{
    UINT32 uret = 0;
    if ((ErrorID < AMBA_NUM_CEHU_ERRORS) && (InstanceID < AMBA_NUM_CEHU_INSTANCES)) {
        UINT32 group_id = ErrorID / 32U;
        AmbaCSL_CEHUClearBitVector(InstanceID, group_id);
    } else {
        uret = CEHU_ERR_PARAM;
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
UINT32 AmbaRTSL_CEHUSafetyModeGet(UINT32 InstanceID, UINT32 ErrorID, UINT32 *Value)
{
    UINT32 uret = 0;
    if ((ErrorID < AMBA_NUM_CEHU_ERRORS) && (InstanceID < AMBA_NUM_CEHU_INSTANCES) && (Value != NULL)) {
        UINT32 group_id = ErrorID / 16U;
        UINT32 idx      = ErrorID % 16U;
        UINT32 shift    = idx * 2U;
        UINT32 mode_vector;
        UINT32 mask = 0x03U;

        mode_vector = AmbaCSL_CEHUGetSafetyMode(InstanceID, group_id);
        *Value = (mode_vector >> shift) & mask;

    } else {
        uret = CEHU_ERR_PARAM;
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
UINT32 AmbaRTSL_CEHUSafetyModeSet(UINT32 InstanceID, UINT32 ErrorID, UINT32 Value)
{
    UINT32 uret = 0;
    if ((ErrorID < AMBA_NUM_CEHU_ERRORS)  && (InstanceID < AMBA_NUM_CEHU_INSTANCES)) {
        UINT32 group_id = ErrorID / 16U;
        UINT32 idx      = ErrorID % 16U;
        UINT32 shift    = idx * 2U;
        UINT32 mode_vector;
        UINT32 mask = 0x03U;
        UINT32 value_to_set = Value & mask;


        mode_vector = AmbaCSL_CEHUGetSafetyMode(InstanceID, group_id);

        // clear bits first
        mode_vector = mode_vector & (~(mask << shift));

        // set bits
        mode_vector = mode_vector | ((value_to_set << shift));

        AmbaCSL_CEHUSetSafetyMode(InstanceID, group_id, mode_vector);

    } else {
        uret = CEHU_ERR_PARAM;
    }
    return uret;

}
