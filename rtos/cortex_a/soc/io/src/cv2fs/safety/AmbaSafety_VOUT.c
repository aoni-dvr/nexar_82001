/**
 *  @file AmbaSafety_VOUT.c
 *
 *  @copyright Copyright (c) 2020 Ambarella International LP
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
 *  @details VOUT Safety driver APIs
 *
 */
/**
   Safety Measures
   1. register shadow
      *shadow area is auto-synced while VOUT display and dsi phy config are configured by
       (a) AmbaVOUT_* in AmbaVOUT.c (b) AmbaRTSL_Vout* in AmbaRTSL_VOUT.c
      *shadow area is auto-locked by IPC mutex
      *R52 compare shadow area with hardware register (only chosen significant registers were compared, not entire VOUT hardware registers)
   2. safe state
      *R52 CEHU apps can call API to enter safe state
      *all display/dsi phy configuration related APIs return directly in safe state
   3. API control flow
      *all APIs will return directly if AmbaVOUT_DrvEntry() is not called or not successfully executed
   4. software lock-step calculation
      *when hardware fault cannot be detected in all other safety-measure mechanisms
       selected functions can be duplicated as software lock-step calculation
 */

#include "AmbaIOUtility.h"
#include "AmbaKAL.h"
#include "AmbaVOUT.h"
#include "AmbaVOUT_Ctrl.h"
#include "AmbaSafety.h"
#include "AmbaSafety_VOUT.h"
#include "AmbaIPC.h"
#include "AmbaMisraFix.h"
#include "AmbaCSL_VOUT.h"

#define VOUT_SHADOW_BASE             (SHADOWBASE_VOUT)                         /**< base address for vout shadow */
#define VOUT_SHADOW_DISP_REG_NUM     (15U)                                     /**< disp reg to watch */
#define VOUT_SHADOW_DSI_PHY_REG_NUM  (5U)                                      /**< dsi phy reg to watch */

#define VOUT_SHADOW_RESET_VAL        (0xBAADF00DU)                             /**< reset value to indicate config data is not ready */

typedef struct {
    UINT32 DispConfig[VOUT_SHADOW_DISP_REG_NUM];
    UINT32 DsiPhyConfig[VOUT_SHADOW_DSI_PHY_REG_NUM];
    UINT32 SafeState;
    UINT32 R52Inited;
    UINT32 A53Inited;
} AMBA_VOUT_SHADOW_s;

static UINT32                      VOUT_Inited[AMBA_NUM_VOUT_INDEX];            /**< VOUT safety init state */
static UINT32                      *pVoutSafeState[AMBA_NUM_VOUT_INDEX];        /**< safe state */
static AMBA_VOUT_SHADOW_s          *pVoutShadow[AMBA_NUM_VOUT_INDEX];           /**< register shadow (including state indicators) */

/**
 * AmbaSafety_VoutSetSafeState - Set current VOUT Safe State
 *
 * @param[in] VoutID Indicate VOUT channel
 * @param[in] State 1: enter safe state, 0: leave safe state
 * @return error code
 */
UINT32 AmbaSafety_VoutSetSafeState(UINT32 VoutID, UINT32 State)
{
    UINT32 Ret = 0U;

    /* arg check */
    if (VoutID >= AMBA_NUM_VOUT_INDEX) {
        Ret = VOUT_ERR_ARG;
    } else {
        /* code start */
        (void)State;
        AmbaSafety_VoutShadowLock();
        *pVoutSafeState[VoutID] = 1U;
        AmbaSafety_VoutShadowUnLock();
    }

    return Ret;
}

/**
 * AmbaSafety_VoutGetSafeState - Get current VOUT Safe State
 *
 * @param[in] VoutID Indicate VOUT channel
 * @param[in] pState 1: enter safe state, 0: leave safe state
 * @return when pState is NULL, return State directly
 */
UINT32 AmbaSafety_VoutGetSafeState(UINT32 VoutID, UINT32 *pState)
{
    UINT32 Ret = 0U;

    /* arg check */
    if (VoutID >= AMBA_NUM_VOUT_INDEX) {
        Ret = VOUT_ERR_ARG;
    } else {
        /* code start */
        AmbaSafety_VoutShadowLock();
        if (pState != NULL) {
            *pState = *pVoutSafeState[VoutID];
        } else {
            /* when pState is NULL, return State directly */
            Ret = *pVoutSafeState[VoutID];
        }
        AmbaSafety_VoutShadowUnLock();
    }

    return Ret;
}

/**
 *  AmbaSafety_VoutShadowLock - Lock VOUT register shadow
 *
 *  @return void
 */
void AmbaSafety_VoutShadowLock(void)
{
    if (0U != AmbaIPC_MutexTake(AMBA_IPC_MUTEX_VOUT, 100U)) {
        // Should not happen
    }
}

/**
 *  AmbaSafety_VoutShadowUnLock - Unlock VOUT register shadow
 *
 *  @return void
 */
void AmbaSafety_VoutShadowUnLock(void)
{
    (void)AmbaIPC_MutexGive(AMBA_IPC_MUTEX_VOUT);
}

/**
 *  AmbaSafety_VoutShadowDispCfg - Write VOUT disp configuration to shadow memory
 *
 *  @param[in] VoutID Indicate VOUT channel
 *  @param[in] pBuf Pointer to Main configuration buffer
 *  @return void
 */
void AmbaSafety_VoutShadowDispCfg(UINT32 VoutID, const void *pBuf)
{
    UINT32 i;
    const UINT32 *pVoutCfg;

    if (pBuf != NULL) {
        AmbaMisra_TypeCast(&pVoutCfg, &pBuf);

        for (i = 0U ; i < VOUT_SHADOW_DISP_REG_NUM; i++) {
            pVoutShadow[VoutID]->DispConfig[i] = (UINT32) pVoutCfg[i];
        }
    }
}

/**
 *  AmbaSafety_VoutShadowDispRst - Reset VOUT disp configuration indicator in shadow memory
 *
 *  @param[in] pBuf Pointer to Main configuration buffer
 *  @return void
 */
void AmbaSafety_VoutShadowDispRst(UINT32 VoutID)
{
    pVoutShadow[VoutID]->DispConfig[0] = (UINT32) VOUT_SHADOW_RESET_VAL;
}


/**
 *  AmbaSafety_VoutShadowDsiPhyCfg - Write VOUT DSI PHY configuration to shadow memory
 *
 *  @param[in] VoutID Indicate VOUT channel
 *  @param[in] pBuf Pointer to Main configuration buffer
 *  @return void
 */
void AmbaSafety_VoutShadowDsiPhyCfg(UINT32 VoutID, const void *pBuf)
{
    UINT32 i;
    const UINT32 *pDsiPhyCfg;

    if (pBuf != NULL) {
        AmbaMisra_TypeCast(&pDsiPhyCfg, &pBuf);

        for (i = 0U ; i < VOUT_SHADOW_DSI_PHY_REG_NUM; i++) {
            pVoutShadow[VoutID]->DsiPhyConfig[i] = (UINT32) pDsiPhyCfg[i];
        }
    }
}

/**
 *  AmbaSafety_VoutShadowDsiPhyRst - Reset VOUT DSI PHY configuration indicator in shadow memory
 *
 *  @param[in] pBuf Pointer to Main configuration buffer
 *  @return void
 */
void AmbaSafety_VoutShadowDsiPhyRst(UINT32 VoutID)
{
    pVoutShadow[VoutID]->DsiPhyConfig[0] = (UINT32) VOUT_SHADOW_RESET_VAL;
}

static UINT32 Safety_VoutCompareDispConfig(UINT32 VoutID)
{
    UINT32 i, RetVal = 0U;
    const volatile UINT32 *reg;
    const UINT32 *shadow;

    if (VoutID == 0U) {
        AmbaMisra_TypeCast(&reg, &pAmbaVoutDisplay0_Reg);
    } else {
        AmbaMisra_TypeCast(&reg, &pAmbaVoutDisplay1_Reg);
    }
    shadow = &pVoutShadow[VoutID]->DispConfig[0];
    for (i = 0U; i < VOUT_SHADOW_DISP_REG_NUM; i++) {

        /* Skip list */
        if (i == 1U) {
            continue;
        }

        if (reg[i] != shadow[i]) {
            RetVal = 1U;
        }
    }

    return RetVal;
}

static UINT32 Safety_VoutCompareDsiPhyConfig(UINT32 VoutID)
{
    UINT32 i, RetVal = 0U;
    const volatile UINT32 *reg, *reg2;
    const UINT32 *shadow;

    if (VoutID == 0U) {
        reg2 = &pAmbaRCT_Reg->MipiDsi0Ctrl0;
        AmbaMisra_TypeCast(&reg, &reg2);
    } else {
        reg2 = &pAmbaRCT_Reg->MipiDsi1Ctrl0;
        AmbaMisra_TypeCast(&reg, &reg2);
    }

    shadow = &pVoutShadow[VoutID]->DsiPhyConfig[0];
    for (i = 0U; i < VOUT_SHADOW_DSI_PHY_REG_NUM; i++) {
        if (reg[i] != shadow[i]) {
            RetVal = 2U;
        }
    }

    return RetVal;
}

/**
 *  AmbaSafety_VoutChecker - VOUT safety checker for R52
 *
 * @param [out] pModuleID
 * @param [out] pInstanceID
 * @param [out] pApiID
 * @param [out] pErrorID
 * @return zero: caller should not trigger CEHU error, non-zero: caller should trigger CEHU error
 */
UINT32 AmbaSafety_VoutChecker(UINT32 *pModuleID, UINT32 *pInstanceID, UINT32 *pApiID, UINT32 *pErrorID)
{
    UINT32 VoutID, State;
    UINT32 DoErrorReport = 0U, InstanceID = 0U;

    /*
     * Shadow Compare
     */
    for (VoutID = 0U; VoutID < AMBA_NUM_VOUT_INDEX; VoutID++) {
        if ((VOUT_Inited[VoutID] != 0U) && (pVoutShadow[VoutID]->A53Inited != 0U) &&
            (pVoutShadow[VoutID]->DispConfig[0] != VOUT_SHADOW_RESET_VAL)) {
            (void)AmbaSafety_VoutGetSafeState(VoutID, &State);
            if (State == 0U) {
                if (AmbaIPC_MutexTake(AMBA_IPC_MUTEX_VOUT, 100U) == 0U) {

                    if (VoutID == 0U) {
                        if ((pAmbaVoutDisplay0_Reg->OutputEnable == 0U)) {
                            (void)AmbaIPC_MutexGive(AMBA_IPC_MUTEX_VOUT);
                            continue;
                        }

                    } else if (VoutID == 1U) {
                        if (pAmbaVoutDisplay1_Reg->OutputEnable == 0U) {
                            (void)AmbaIPC_MutexGive(AMBA_IPC_MUTEX_VOUT);
                            continue;
                        }
                    } else {
                        /* Do nothing */
                    }

                    /* compare disp config */
                    if (Safety_VoutCompareDispConfig(VoutID) != 0U) {
                        DoErrorReport = 1U;
                    }

                    /* compare dsi phy config */
                    if (Safety_VoutCompareDsiPhyConfig(VoutID) != 0U) {
                        DoErrorReport = 2U;
                    }

                    (void)AmbaIPC_MutexGive(AMBA_IPC_MUTEX_VOUT);
                } else {
                    DoErrorReport = 4U;
                }
            }
        }
    }

    if (DoErrorReport != 0U) {
        *pModuleID = VOUT_MODULE_ID;
        *pInstanceID = InstanceID;
        *pApiID = 0U;
        *pErrorID = DoErrorReport;
    }

    return DoErrorReport;
}

/**
 *  AmbaSafety_VoutInit - Init VOUT safety feature
 *  @param[in] VoutID Indicate VOUT channel
 *  @return error code
 */
UINT32 AmbaSafety_VoutInit(UINT32 VoutID)
{
    UINT32 Ret = VOUT_ERR_NONE;
    ULONG Base = VOUT_SHADOW_BASE;
    const UINT32 *pBase;

    /* arg check */
    if (VoutID >= AMBA_NUM_VOUT_INDEX) {
        Ret = VOUT_ERR_ARG;
    } else {
        /* code start */
        Base += sizeof(AMBA_VOUT_SHADOW_s) * VoutID;
        AmbaMisra_TypeCast(&pVoutShadow[VoutID], &Base);

        pBase = &pVoutShadow[VoutID]->SafeState;
        AmbaMisra_TypeCast(&pVoutSafeState[VoutID], &pBase);

#if defined(CONFIG_CPU_CORTEX_R52)
        pVoutShadow[VoutID]->R52Inited = 1U;
        pVoutShadow[VoutID]->SafeState = 0U;
        pVoutShadow[VoutID]->DispConfig[0] = VOUT_SHADOW_RESET_VAL;
#else
        pVoutShadow[VoutID]->A53Inited = 1U;
        (void)pVoutShadow[VoutID]->R52Inited;
#endif
        VOUT_Inited[VoutID] = 1U;
    }

    return Ret;
}

/**
 *  AmbaSafety_VoutDeInit - De-init VOUT safety feature
 *  @param[in] VoutID Indicate VOUT channel
 *  @return error code
 */
UINT32 AmbaSafety_VoutDeInit(UINT32 VoutID)
{
    UINT32 Ret = VOUT_ERR_NONE;

    /* arg check */
    if (VoutID >= AMBA_NUM_VOUT_INDEX) {
        Ret = VOUT_ERR_ARG;
    } else if (VOUT_Inited[VoutID] == 0U) {
        Ret = VOUT_ERR_ARG; /* Already deinit */
    } else {
#if defined(CONFIG_CPU_CORTEX_R52)
        pVoutShadow[VoutID]->R52Inited = 0U;
#else
        pVoutShadow[VoutID]->A53Inited = 0U;
        (void)pVoutShadow[VoutID]->R52Inited;
#endif
        VOUT_Inited[VoutID] = 0U;
    }

    return Ret;
}

