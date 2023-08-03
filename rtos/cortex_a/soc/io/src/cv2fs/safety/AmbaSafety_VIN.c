/**
 *  @file AmbaSafety_VIN.c
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
 *  @details VIN Safety driver APIs
 *
 */
/**
   Safety Measures
   1. register shadow
      *shadow area is auto-synced while VIN main data is configured by
       (a) AmbaVIN_* in AmbaVIN.c (b) DebugBusConfigVin* in AmbaCSL_VIN.c
      *shadow area is auto-synced while MIPI PHY is configured by VIN driver
      *shadow area is auto-locked by IPC mutex
      *R52 compare shadow area with hardware register (only chosen significant registers were compared, not entire VIN hardware registers)
   2. safe state
      *R52 CEHU apps can call API to enter safe state
      *all main configuration related APIs return directly in safe state
   3. API control flow
      *all APIs will return directly if AmbaVIN_DrvEntry() is not called or not successfully executed
   4. software lock-step calculation
      *when hardware fault cannot be detected in all other safety-measure mechanisms
       selected functions can be duplicated as software lock-step calculation
 */

#include "AmbaIOUtility.h"
#include "AmbaKAL.h"
#include "AmbaVIN.h"
#include "AmbaSafety.h"
#include "AmbaSafety_VIN.h"
#include "AmbaIPC.h"
#if defined(CONFIG_CPU_CORTEX_R52)
#include "AmbaWrap.h"
#endif
#include "AmbaMisraFix.h"
#include "AmbaCSL_DebugPort.h"
#include "AmbaReg_RCT.h"

/*
 *   Layout of shadow memory for VIN
 *  +-------------------------------+  0x000
 *  |    vin0 main config           |
 *  +-------------------------------+  0x100
 *  |    vin1 main config           |
 *  +-------------------------------+  0x200
 *  |    vin2 main config           |
 *  +-------------------------------+  0x300
 *  |    vin3 main config           |
 *  +-------------------------------+  0x400
 *  |    vin4 main config           |
 *  +-------------------------------+  0x500
 *  |    vin8 main config           |
 *  +-------------------------------+  0x600
 *  |     MIPI PHY configuration    |
 *  +-------------------------------+  0x700
 */

#define VIN_SHADOW_BASE             (SHADOWBASE_VIN)                          /**< base address for vin shadow */
#define VIN_SHADOW_MAIN_REG_NUM     (46U)                                     /**< main reg to watch */
#define VIN_SHADOW_PADDING_NUM      (15U)                                     /**< padding data */

#define VIN_SHADOW_MAIN_RANGE       ((VIN_SHADOW_MAIN_REG_NUM - 1U) * 4U)     /**< max main reg offset */

#define VIN_SHADOW_RESET_VAL        (0xBAADF00DU)                             /**< reset value to indicate config data is not ready */

#define VIN_SHADOW_PHY_BASE         (SHADOWBASE_VIN + 0x600)
#define VIN_SHADOW_MIPI_PHY_REG_NUM (11U)                                     /**< mipi phy reg to watch */
#define VIN_SHADOW_PHY_PADDING_NUM  (53U)                                     /**< padding data */

typedef struct {
    UINT32 MainConfig[VIN_SHADOW_MAIN_REG_NUM];
    UINT32 Padding[VIN_SHADOW_PADDING_NUM];
    UINT32 SafeState;
    UINT32 R52Inited;
    UINT32 A53Inited;
} AMBA_VIN_CFG_SHADOW_s; /* (64 * 4) bytes for each VIN */

typedef struct {
    UINT32 DphyCtrl[VIN_SHADOW_MIPI_PHY_REG_NUM];
    UINT32 Padding[VIN_SHADOW_PHY_PADDING_NUM];
} AMBA_VIN_PHY_SHADOW_s; /* (64 * 4) bytes, shared by all VINs */

static UINT32                       VIN_Inited[AMBA_NUM_VIN_CHANNEL];         /**< VIN safety init state */
static UINT32                      *pVinSafeState[AMBA_NUM_VIN_CHANNEL];      /**< safe state */
static AMBA_VIN_CFG_SHADOW_s       *pVinCfgShadow[AMBA_NUM_VIN_CHANNEL];      /**< register shadow (including state indicators) for VIN main config */

static AMBA_VIN_PHY_SHADOW_s       *pVinPhyShadow;                            /**< register shadow for MIPI PHY registers */

extern void AmbaPrint_ModulePrintUInt5(UINT16 ModuleID, const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
extern void AmbaPrint_ModulePrintStr5(UINT16 ModuleID, const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);

static void VinW_Print(const char *func, const UINT32 line, const UINT16 ModuleID, const char *msg)
{
    char Buf[22];

    if (IO_UtilityUInt32ToStr(Buf, (UINT32)sizeof(Buf), line, 10U) != 0U) {
        AmbaPrint_ModulePrintStr5(ModuleID, "VIN %s(%s) %s", func, Buf, msg, NULL, NULL);
    }
}

/**
 * AmbaSafety_VinSetSafeState - Set current VIN Safe State
 *
 * @param[in] VinID Indicate VIN channel
 * @param[in] State 1: enter safe state, 0: leave safe state
 * @return error code
 */
UINT32 AmbaSafety_VinSetSafeState(UINT32 VinID, UINT32 State)
{
    UINT32 Ret = 0U;

    /* arg check */
    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        VinW_Print(__func__, __LINE__, VIN_MODULE_ID, "VinID over range!");
        Ret = VIN_ERR_ARG;
    } else {
        /* code start */
        (void)State;
        AmbaSafety_VinShadowLock();
        *pVinSafeState[VinID] = 1U;
        AmbaSafety_VinShadowUnLock();
        AmbaPrint_ModulePrintUInt5(VIN_MODULE_ID, "VinSafeState[%d] = 1", VinID, 0U, 0U, 0U, 0U);
    }

    return Ret;
}

/**
 * AmbaSafety_VinGetSafeState - Get current VIN Safe State
 *
 * @param[in] VinID Indicate VIN channel
 * @param[in] pState 1: enter safe state, 0: leave safe state
 * @return when pState is NULL, return State directly
 */
UINT32 AmbaSafety_VinGetSafeState(UINT32 VinID, UINT32 *pState)
{
    UINT32 Ret = 0U;

    /* arg check */
    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        VinW_Print(__func__, __LINE__, VIN_MODULE_ID, "VinID over range!");
        Ret = VIN_ERR_ARG;
    } else {
        /* code start */
        AmbaSafety_VinShadowLock();
        if (pState != NULL) {
            *pState = *pVinSafeState[VinID];
        } else {
            /* when pState is NULL, return State directly */
            Ret = *pVinSafeState[VinID];
        }
        AmbaSafety_VinShadowUnLock();
    }

    return Ret;
}

/**
 *  AmbaSafety_VinShadowLock - Lock VIN register shadow
 *
 *  @return void
 */
void AmbaSafety_VinShadowLock(void)
{
    if (0U != AmbaIPC_MutexTake(AMBA_IPC_MUTEX_VIN, 100U)) {
        VinW_Print(__func__, __LINE__, VIN_MODULE_ID, "AmbaIPC_MutexTake timeout");
    }
}

/**
 *  AmbaSafety_VinShadowUnLock - Unlock VIN register shadow
 *
 *  @return void
 */
void AmbaSafety_VinShadowUnLock(void)
{
    (void)AmbaIPC_MutexGive(AMBA_IPC_MUTEX_VIN);
}

/**
 *  AmbaSafety_VinShadowWrite - Write VIN register shadow
 *
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pAddr Shadow VIN register shadow offset
 *  @param[in] Value Shadow VIN register shadow value
 *  @return void
 */
void AmbaSafety_VinShadowWrite(UINT32 VinID, volatile UINT32 *pAddr, UINT32 Value)
{
    UINT32 Offset;
    UINT8 *addr = NULL;
    UINT32 *pShadow;
    const UINT32 *pBase;

    if (VinID < AMBA_NUM_VIN_CHANNEL) {
        (void)pAddr;
        AmbaMisra_TypeCast(&Offset, &pAddr);

        if ((Offset >= AMBA_DBG_PORT_IDSP_VIN_MAIN_CONFIG_BASE_ADDR) &&
            (Offset <= (AMBA_DBG_PORT_IDSP_VIN_MAIN_CONFIG_BASE_ADDR + VIN_SHADOW_MAIN_RANGE))) {

            Offset -= AMBA_DBG_PORT_IDSP_VIN_MAIN_CONFIG_BASE_ADDR;
            pBase = &pVinCfgShadow[VinID]->MainConfig[0];
            AmbaMisra_TypeCast(&addr, &pBase);
        } else {
            /* skip register clone */
        }

        if (addr != NULL) {
            addr = &addr[Offset];
            AmbaMisra_TypeCast(&pShadow, &addr);

            *(pShadow) = Value;
        }
    }
}

/**
 *  AmbaSafety_VinShadowMainCfg - Write VIN Main configuration to shadow memory
 *
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pBuf Pointer to Main configuration buffer
 *  @return void
 */
void AmbaSafety_VinShadowMainCfg(UINT32 VinID, const void *pBuf)
{
    UINT32 i;
    const UINT16 *pVinCfg;

    if (pBuf != NULL) {
        AmbaMisra_TypeCast(&pVinCfg, &pBuf);

        AmbaPrint_ModulePrintUInt5(VIN_MODULE_ID, "[shadow main cfg]", 0U, 0U, 0U, 0U, 0U);
        for (i = 0U ; i < VIN_SHADOW_MAIN_REG_NUM; i++) {
            pVinCfgShadow[VinID]->MainConfig[i] = (UINT32)pVinCfg[i];
            AmbaPrint_ModulePrintUInt5(VIN_MODULE_ID, " %d: 0x%x", i, pVinCfgShadow[VinID]->MainConfig[i], 0U, 0U, 0U);
        }
    }
}

/**
 *  AmbaSafety_VinShadowPhyCfg - Write PHY configuration to shadow memory
 *
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pBuf Pointer to VIN PHY configuration buffer
 *  @return void
 */
void AmbaSafety_VinShadowPhyCfg(UINT32 VinID, const void *pBuf)
{
    UINT32 i;
    const UINT32 *pPhyReg;

    AmbaMisra_TouchUnused(&VinID);

    if (pBuf != NULL) {
        AmbaMisra_TypeCast(&pPhyReg, &pBuf);

        for (i = 0U ; i < VIN_SHADOW_MIPI_PHY_REG_NUM; i++) {
            pVinPhyShadow->DphyCtrl[i] = (UINT32)pPhyReg[i + 10U];
        }
    }
}

/**
 *  AmbaSafety_VinShadowMainRst - Reset VIN Main configuration indicator in shadow memory
 *
 *  @param[in] VinID Indicate VIN channel
 *  @return void
 */
void AmbaSafety_VinShadowMainRst(UINT32 VinID)
{
    AmbaPrint_ModulePrintUInt5(VIN_MODULE_ID, "[shadow main rst]", 0U, 0U, 0U, 0U, 0U);
    pVinCfgShadow[VinID]->MainConfig[0] = (UINT32)VIN_SHADOW_RESET_VAL;
    AmbaPrint_ModulePrintUInt5(VIN_MODULE_ID, " %d: 0x%x", 0U, pVinCfgShadow[VinID]->MainConfig[0], 0U, 0U, 0U);
}

static UINT32 VIN_IdspSelectSection(UINT32 VinID)
{
    UINT32 Ret = VIN_ERR_NONE;

    if (VinID == AMBA_VIN_CHANNEL0) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN0, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL1) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN1, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL2) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN2, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL3) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN3, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL4) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN4, 0U);
    } else if (VinID == AMBA_VIN_CHANNEL8) {
        AmbaCSL_IdspSetAddrExtension(AMBA_DBG_PORT_IDSP_VIN8, 0U);
    } else {
        Ret = VIN_ERR_ARG;
    }

    if (Ret == VIN_ERR_NONE) {
        (void)AmbaKAL_TaskSleep(1U);
    }

    return Ret;
}

/**
 *  AmbaSafety_VinChecker - VIN safety checker for R52
 *
 * @param [out] pModuleID
 * @param [out] pInstanceID
 * @param [out] pApiID
 * @param [out] pErrorID
 * @return zero: caller should not trigger CEHU error, non-zero: caller should trigger CEHU error
 */
UINT32 AmbaSafety_VinChecker(UINT32 *pModuleID, UINT32 *pInstanceID, UINT32 *pApiID, UINT32 *pErrorID)
{
    UINT32 i, VinID, State, r1, r2;
    const volatile UINT32 *reg, *shadow;
    UINT32 DoErrorReport = 0U, InstanceID = 0U, DoPhyCheck = 1U;

    /*
     * Shadow Compare
     */
    for (VinID = 0U; VinID < AMBA_NUM_VIN_CHANNEL; VinID++) {
        if ((VIN_Inited[VinID] != 0U) && (pVinCfgShadow[VinID]->A53Inited != 0U) &&
            (pVinCfgShadow[VinID]->MainConfig[0] != VIN_SHADOW_RESET_VAL)) {
            (void)AmbaSafety_VinGetSafeState(VinID, &State);
            if (State == 0U) {
                if (AmbaIPC_MutexTake(AMBA_IPC_MUTEX_VIN, 100U) == 0U) {
                    /* select Idsp section */
                    (void)VIN_IdspSelectSection(VinID);

                    if (pAmbaIDSP_VinMainReg->Ctrl0.Enable == 0U) {
                        (void)AmbaIPC_MutexGive(AMBA_IPC_MUTEX_VIN);
                        continue;
                    }

                    /* compare main config */
                    AmbaMisra_TypeCast(&reg, &pAmbaIDSP_VinMainReg);
                    shadow = &pVinCfgShadow[VinID]->MainConfig[0];
                    for (i = 0U; i < VIN_SHADOW_MAIN_REG_NUM; i++) {
                        r1 = reg[i];
                        r2 = shadow[i];
                        if (r1 != r2) {
                            AmbaPrint_ModulePrintUInt5(VIN_MODULE_ID, "[VinID:%d][main] %04x: reg(0x%x) != shadow(0x%x)", VinID, i * 4U, r1, r2, 0U);
                            DoErrorReport = 2U;
                        }
                    }

                    /* compare MIPI PHY registers Ctrl0~Ctrl10 */
                    if (DoPhyCheck == 1U) {
                        AmbaMisra_TypeCast(&reg, &pAmbaMIPI_Reg);
                        shadow = &pVinPhyShadow->DphyCtrl[0];
                        for (i = 0U; i < VIN_SHADOW_MIPI_PHY_REG_NUM; i++) {
                            r1 = reg[i + 10U];
                            r2 = shadow[i];
                            if (r1 != r2) {
                                AmbaPrint_ModulePrintUInt5(VIN_MODULE_ID, "[phy] 0x%04x: reg(0x%x) != shadow(0x%x)", (i + 10U) * 4U, r1, r2, 0U, 0U);
                                DoErrorReport = 4U;
                            }
                        }
                        DoPhyCheck = 0U;
                    }

                    (void)AmbaIPC_MutexGive(AMBA_IPC_MUTEX_VIN);
                } else {
                    VinW_Print(__func__, __LINE__, VIN_MODULE_ID, "AmbaIPC_MutexTake timeout");
                    DoErrorReport = 8U;
                }
            }
        }
    }

    if (DoErrorReport != 0U) {
        *pModuleID = VIN_MODULE_ID;
        *pInstanceID = InstanceID;
        *pApiID = 0U;
        *pErrorID = DoErrorReport;
    }

    return DoErrorReport;
}

/**
 *  AmbaSafety_VinInit - Init VIN safety feature
 *  @param[in] VinID Indicate VIN channel
 *  @return error code
 */
UINT32 AmbaSafety_VinInit(UINT32 VinID)
{
    UINT32 Ret = VIN_ERR_NONE;
    ULONG Base = VIN_SHADOW_BASE;
    const UINT32 *pBase;

    /* arg check */
    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        Ret = VIN_ERR_ARG;
    } else if (VIN_Inited[VinID] != 0U) {
        Ret = VIN_ERR_ARG;
    } else {
        /* code start */
        Base += sizeof(AMBA_VIN_CFG_SHADOW_s) * VinID;
        AmbaMisra_TypeCast(&pVinCfgShadow[VinID], &Base);

        pBase = &pVinCfgShadow[VinID]->SafeState;
        AmbaMisra_TypeCast(&pVinSafeState[VinID], &pBase);

        if (pVinPhyShadow == NULL) {
            Base = VIN_SHADOW_PHY_BASE;
            AmbaMisra_TypeCast(&pVinPhyShadow, &Base);
        }

#if defined(CONFIG_CPU_CORTEX_R52)
        pVinCfgShadow[VinID]->R52Inited = 1U;
        pVinCfgShadow[VinID]->SafeState = 0U;
        pVinCfgShadow[VinID]->MainConfig[0] = VIN_SHADOW_RESET_VAL;
        if (AmbaWrap_memset(&pVinCfgShadow[VinID]->Padding[0], 0xff, VIN_SHADOW_PADDING_NUM * 4U) != ERR_NONE) {
            Ret = VIN_ERR_UNEXPECTED;
        } else {
            if (AmbaWrap_memset(&pVinPhyShadow->Padding[0], 0xff, VIN_SHADOW_PHY_PADDING_NUM * 4U) != ERR_NONE) {
                Ret = VIN_ERR_UNEXPECTED;
            }
        }
#else
        pVinCfgShadow[VinID]->A53Inited = 1U;
        (void)pVinCfgShadow[VinID]->R52Inited;
        (void)pVinCfgShadow[VinID]->Padding;
        (void)pVinPhyShadow->Padding;
#endif
        VIN_Inited[VinID] = 1U;
    }

    return Ret;
}
