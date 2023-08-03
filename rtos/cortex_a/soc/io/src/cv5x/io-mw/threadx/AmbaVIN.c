/**
 *  @file AmbaVIN.c
 *
 *  @copyright Copyright (c) 2021 Ambarella International LP
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
 *  @details Video Input APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaVIN.h"
#include "AmbaVIN_Ctrl.h"
#include "AmbaRTSL_VIN.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaCSL_Scratchpad.h"
#include "AmbaCSL_VIN.h"
#if !defined(CONFIG_LINUX)
#include "AmbaCSL_DebugPort.h"
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include "AmbaIOUtility.h"
#endif
#include "AmbaCortexA76.h"
#endif

#define MIPI_RX_CLK_ACTIVE_HS_POLLING_TIMEOUT 500U

AMBA_KAL_MUTEX_t AmbaVinMutex;  /* Mutex */

static void AmbaVIN_CalculateMphyConfig(UINT64 BitRate, AMBA_VIN_MIPI_TIMING_PARAM_s *pVinMipiTiming);
static void AmbaVIN_GetDcphyConfig(UINT64 DataRate, UINT32 PhyMode, AMBA_VIN_DCPHY_PARAM_s *pVinDcphyParam);

static UINT32 IsSuccessor(UINT32 VinID)
{
    ULONG BufAddr;
    const AMBA_CSL_VIN_CONFIG_DATA_s *pVinConfigAddr;
    UINT32 RetVal = 0U;

    if (VinID == 0xFFFFFFFFU) {
        /* To indicate that AmbaRTSL_VinInit() has been called. */
        if (AmbaCSL_VinGetRefClkSel() == 0U) {
            RetVal = 1U;
        }
    } else {
        (void)AmbaCSL_VinGetMainCfgBufAddr(VinID, &BufAddr);
        AmbaMisra_TypeCast(&pVinConfigAddr, &BufAddr);

        /* To indicate that VIN HW has been enabled but the Enable bit in the config buffer is 0 */
        if ((AmbaCSL_VinIsEnabled(VinID) == 1U) &&
            (pVinConfigAddr->Ctrl.Enable == 0U)) {
            RetVal = 1U;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaVIN_DrvEntry(void)
{
    UINT32 RetVal = VIN_ERR_NONE;
#if !defined(CONFIG_LINUX)
    ULONG BaseAddr;
#endif

    /* Create Mutex */
    if (AmbaKAL_MutexCreate(&AmbaVinMutex, NULL) != OK) {
        RetVal = VIN_ERR_MUTEX;    /* should never happen */
    } else {
        /* get base address */
#if !defined(CONFIG_LINUX)
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
        BaseAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,mipi-phy", "reg", 0U);
        if (BaseAddr == 0ULL) {
            RetVal = VIN_ERR_UNEXPECTED;
        } else {
            BaseAddr |= AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR;
            AmbaMisra_TypeCast(&pAmbaMIPI_Reg, &BaseAddr);
        }

        BaseAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,idsp-controller", "reg", 0U);
        if (BaseAddr == 0ULL) {
            RetVal = VIN_ERR_UNEXPECTED;
        } else {
            BaseAddr |= AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR;
            AmbaMisra_TypeCast(&pAmbaIDSP_CtrlReg, &BaseAddr);
        }

        BaseAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,idsp-vin-main", "reg", 0U);
        if (BaseAddr == 0ULL) {
            RetVal = VIN_ERR_UNEXPECTED;
        } else {
            BaseAddr |= AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR;
            AmbaMisra_TypeCast(&pAmbaIDSP_VinMainReg, &BaseAddr);
        }

        BaseAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,idsp-vin-global", "reg", 0U);
        if (BaseAddr == 0ULL) {
            RetVal = VIN_ERR_UNEXPECTED;
        } else {
            BaseAddr |= AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR;
            AmbaMisra_TypeCast(&pAmbaIDSP_VinGlobalReg, &BaseAddr);
        }

        BaseAddr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,idsp-vin-msync", "reg", 0U);
        if (BaseAddr == 0ULL) {
            RetVal = VIN_ERR_UNEXPECTED;
        } else {
            BaseAddr |= AMBA_CORTEX_A76_AHB_VIRT_BASE_ADDR;
            AmbaMisra_TypeCast(&pAmbaIDSP_VinMasterSyncReg, &BaseAddr);
        }

        BaseAddr = IO_UtilityFDTPropertyU64Quick(0, "ambarella,mipi-cphy0", "reg", 0U);
        if (BaseAddr == 0ULL) {
            RetVal = VIN_ERR_UNEXPECTED;
        } else {
            AmbaMisra_TypeCast(&pAmbaDCPHY_Regs[0], &BaseAddr);
        }

        BaseAddr = IO_UtilityFDTPropertyU64Quick(0, "ambarella,mipi-cphy1", "reg", 0U);
        if (BaseAddr == 0ULL) {
            RetVal = VIN_ERR_UNEXPECTED;
        } else {
            AmbaMisra_TypeCast(&pAmbaDCPHY_Regs[1], &BaseAddr);
        }
#else
        BaseAddr = AMBA_DBG_PORT_MIPI_BASE_ADDR;
        AmbaMisra_TypeCast(&pAmbaMIPI_Reg, &BaseAddr);

        BaseAddr = AMBA_DBG_PORT_IDSP_CONTROLLER_BASE_ADDR;
        AmbaMisra_TypeCast(&pAmbaIDSP_CtrlReg, &BaseAddr);

        BaseAddr = AMBA_DBG_PORT_IDSP_VIN_MAIN_CONFIG_BASE_ADDR;
        AmbaMisra_TypeCast(&pAmbaIDSP_VinMainReg, &BaseAddr);

        BaseAddr = AMBA_DBG_PORT_IDSP_VIN_GLOBAL_CONFIG_BASE_ADDR;
        AmbaMisra_TypeCast(&pAmbaIDSP_VinGlobalReg, &BaseAddr);

        BaseAddr = AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_CONFIG_BASE_ADDR;
        AmbaMisra_TypeCast(&pAmbaIDSP_VinMasterSyncReg, &BaseAddr);

        BaseAddr = AMBA_CORTEX_A76_CPHY0_BASE_ADDR;
        AmbaMisra_TypeCast(&pAmbaDCPHY_Regs[0], &BaseAddr);

        BaseAddr = AMBA_CORTEX_A76_CPHY1_BASE_ADDR;
        AmbaMisra_TypeCast(&pAmbaDCPHY_Regs[1], &BaseAddr);
#endif
#endif
        if (RetVal == VIN_ERR_NONE) {
            if (IsSuccessor(0xFFFFFFFFU) == 0U) {
                AmbaRTSL_VinInit();
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_SlvsReset - Reset VIN module and reset VIN pad to SLVS mode
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pPadConfig Pointer to VIN pad configuration
 *  @return error code
 */
UINT32 AmbaVIN_SlvsReset(UINT32 VinID, const AMBA_VIN_SLVS_PAD_CONFIG_s *pPadConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pPadConfig == NULL) ||
        (pPadConfig->EnabledPin == 0U)) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {
            if (IsSuccessor(VinID) == 0U) {
                /* pre-configure pad mode */
                (void)AmbaRTSL_VinSetLvdsPadMode(VinID, AMBA_VIN_PAD_MODE_SLVS, pPadConfig->EnabledPin);

                /* Reset VIN module */
                (void)AmbaRTSL_VinReset(VinID, 1U);
                (void)AmbaRTSL_VinReset(VinID, 0U);
            }

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_MipiReset - Reset VIN module and reset VIN pad to MIPI mode
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pPadConfig Pointer to VIN pad configuration
 *  @return error code
 */
UINT32 AmbaVIN_MipiReset(UINT32 VinID, const AMBA_VIN_MIPI_PAD_CONFIG_s *pPadConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_VIN_MIPI_TIMING_PARAM_s VinMipiTiming;
    AMBA_VIN_DCPHY_PARAM_s VinDcphyParam;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pPadConfig == NULL) ||
        ((pPadConfig->ClkMode != AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS) &&
         (pPadConfig->ClkMode != AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS)) ||
        (pPadConfig->DateRate == 0U) || (pPadConfig->EnabledPin == 0U)) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {
            if (IsSuccessor(VinID) == 0U) {
                if (VinID <= AMBA_VIN_CHANNEL7) { /* DCPHY */
                    /* Set resetn to 0 */
                    if ((VinID == AMBA_VIN_CHANNEL0) || (VinID == AMBA_VIN_CHANNEL1) ||
                        (VinID == AMBA_VIN_CHANNEL2) || (VinID == AMBA_VIN_CHANNEL3)) {
                        AmbaCSL_VinSetDcphyRstn0(AMBA_VIN_DCPHY_MODE_DPHY, 0);
                    } else {
                        AmbaCSL_VinSetDcphyRstn1(AMBA_VIN_DCPHY_MODE_DPHY, 0);
                    }

                    /* configure MIPI Dcphy */
                    AmbaVIN_GetDcphyConfig(pPadConfig->DateRate, AMBA_VIN_DCPHY_MODE_DPHY, &VinDcphyParam);
                    RetVal = AmbaRTSL_VinConfigDcphy(VinID, pPadConfig->EnabledPin, &VinDcphyParam);

                    /* Set resetn to 1 */
                    if ((VinID == AMBA_VIN_CHANNEL0) || (VinID == AMBA_VIN_CHANNEL1) ||
                        (VinID == AMBA_VIN_CHANNEL2) || (VinID == AMBA_VIN_CHANNEL3)) {
                        AmbaCSL_VinSetDcphyRstn0(AMBA_VIN_DCPHY_MODE_DPHY, 1);
                    } else {
                        AmbaCSL_VinSetDcphyRstn1(AMBA_VIN_DCPHY_MODE_DPHY, 1);
                    }

                    // writel(RCT_DEBUG_ADDRESS+VIN_CLK_SEL_REG, vin_clk_sel_reg);

                    /* Reset VIN module */
                    (void)AmbaRTSL_VinReset(VinID, 1U);
                    (void)AmbaRTSL_VinReset(VinID, 0U);

                } else {    /* Legacy DPHY */
                    /* configure MIPI phy */
                    AmbaVIN_CalculateMphyConfig(pPadConfig->DateRate, &VinMipiTiming);
                    RetVal = AmbaRTSL_VinConfigMipiPhy(VinID, &VinMipiTiming, pPadConfig->ClkMode, pPadConfig->EnabledPin);

                    /* pre-configure pad mode */
                    (void)AmbaRTSL_VinSetLvdsPadMode(VinID, AMBA_VIN_PAD_MODE_MIPI, pPadConfig->EnabledPin);

                    /* Reset VIN module */
                    (void)AmbaRTSL_VinReset(VinID, 1U);
                    (void)AmbaRTSL_VinReset(VinID, 0U);
                }
#ifdef ENABLE_MIPI_DESKEW_CAL
                /* set MipiSkewCalEn for over 1.5G bps/lane from Dphy spec*/
                if (pPadConfig->DateRate > 1500000000U) {
                    (void)AmbaCSL_VinSetMipiSkewCalEn(1U);
                }
#endif
            }

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_CphyMipiReset - Reset VIN module and reset VIN pad to MIPI mode
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pPadConfig Pointer to VIN pad configuration
 *  @return error code
 */
UINT32 AmbaVIN_MipiCphyReset(UINT32 VinID, const AMBA_VIN_MIPI_CPHY_PAD_CONFIG_s *pPadConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_VIN_DCPHY_PARAM_s VinDcphyParam;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pPadConfig == NULL) ||
        (pPadConfig->DateRate == 0U) || (pPadConfig->EnabledPin == 0U)) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {
            if (IsSuccessor(VinID) == 0U) {
                /* Set resetn to 0 */
                if ((VinID == AMBA_VIN_CHANNEL0) || (VinID == AMBA_VIN_CHANNEL1) ||
                    (VinID == AMBA_VIN_CHANNEL2) || (VinID == AMBA_VIN_CHANNEL3)) {
                    AmbaCSL_VinSetDcphyRstn0(AMBA_VIN_DCPHY_MODE_CPHY, 0);
                } else {
                    AmbaCSL_VinSetDcphyRstn1(AMBA_VIN_DCPHY_MODE_CPHY, 0);
                }

                /* configure MIPI Dcphy */
                AmbaVIN_GetDcphyConfig(pPadConfig->DateRate, AMBA_VIN_DCPHY_MODE_CPHY, &VinDcphyParam);
                RetVal = AmbaRTSL_VinConfigDcphy(VinID, pPadConfig->EnabledPin, &VinDcphyParam);

                /* Set resetn to 1 */
                if ((VinID == AMBA_VIN_CHANNEL0) || (VinID == AMBA_VIN_CHANNEL1) ||
                    (VinID == AMBA_VIN_CHANNEL2) || (VinID == AMBA_VIN_CHANNEL3)) {
                    AmbaCSL_VinSetDcphyRstn0(AMBA_VIN_DCPHY_MODE_CPHY, 1);
                } else {
                    AmbaCSL_VinSetDcphyRstn1(AMBA_VIN_DCPHY_MODE_CPHY, 1);
                }

                // writel(RCT_DEBUG_ADDRESS+VIN_CLK_SEL_REG, vin_clk_sel_reg);

                /* Reset VIN module */
                (void)AmbaRTSL_VinReset(VinID, 1U);
                (void)AmbaRTSL_VinReset(VinID, 0U);
            }

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

static UINT32 WaitSlveEcPhyRxState(const AMBA_VIN_SLVSEC_PAD_CONFIG_s *pPadConfig, UINT32 WaitState)
{
    UINT32 i, RetVal = VIN_ERR_NONE;
    UINT32 LaneStateFlags = 0U;
    UINT32 CountDown = 100U;

    while (LaneStateFlags != pPadConfig->EnabledPin) {
        if (CountDown-- == 0U) {
            RetVal = VIN_ERR_UNEXPECTED;
            break;
        }

        for (i = 0U; i < 12U; i++) {
            if ((pPadConfig->EnabledPin & ((UINT32)1U << i)) != 0U) {
                if (AmbaCSL_VinGetSlvsEcPhyRxState(i) == WaitState) {
                    LaneStateFlags |= (UINT32)1U << i;
                }
            }
        }

        (void)AmbaKAL_TaskSleep(1U);
    }

#if 0 /* for debugging */
    {
        extern void AmbaPrint_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);

        AmbaPrint_PrintUInt5("=> WaitState: 0x%x, CountDown: %u, LaneStateFlags: 0x%03x", WaitState, CountDown, LaneStateFlags, 0U, 0U);
    }
#endif

    return RetVal;
}

/**
 *  AmbaVIN_SlvsEcReset - Reset VIN module and reset VIN pad to SLVS-EC mode
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pPadConfig Pointer to VIN pad configuration
 *  @return error code
 */
UINT32 AmbaVIN_SlvsEcReset(UINT32 VinID, const AMBA_VIN_SLVSEC_PAD_CONFIG_s *pPadConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 i = 0U, MaxRetry = 10U;

    //if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pPadConfig == NULL) ||
    if ((VinID != AMBA_VIN_CHANNEL0) || (pPadConfig == NULL) ||
        (pPadConfig->EnabledPin == 0U) || (pPadConfig->DataRate == 0ULL)) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {
            if (IsSuccessor(VinID) == 0U) {
                /* pre-configure PHY Controller and SerDes_RX */
                do {
                    (void)AmbaRTSL_VinConfigSlvsEcPhy(VinID, pPadConfig, 0U);
                    RetVal = WaitSlveEcPhyRxState(pPadConfig, 0xcU);

                    i++;
                } while ((i < MaxRetry) && (RetVal != OK));

#if 0 /* for debugging */
                {
                    extern void AmbaPrint_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);

                    AmbaPrint_PrintUInt5("=> WaitState:0x%x, Retry count: %d", 0xC, i, 0U, 0U, 0U);
                }
#endif
                (void)AmbaRTSL_VinConfigSlvsEcPhy(VinID, pPadConfig, 1U);

                /* Reset VIN module */
                (void)AmbaRTSL_VinReset(VinID, 1U);
                (void)AmbaRTSL_VinReset(VinID, 0U);
            }

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_SlvsEcCalib - Perform SLVS-EC PHY calibration
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pPadConfig Pointer to VIN pad configuration
 *  @return error code
 */
UINT32 AmbaVIN_SlvsEcCalib(UINT32 VinID, const AMBA_VIN_SLVSEC_PAD_CONFIG_s *pPadConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    //if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pPadConfig == NULL) ||
    if ((VinID != AMBA_VIN_CHANNEL0) || (pPadConfig == NULL) ||
        (pPadConfig->EnabledPin == 0U) || (pPadConfig->DataRate == 0ULL)) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {
            if (IsSuccessor(VinID) == 0U) {
                (void)AmbaRTSL_VinConfigSlvsEcPhy(VinID, pPadConfig, 2U);
                RetVal = WaitSlveEcPhyRxState(pPadConfig, 0xaU);
                (void)AmbaRTSL_VinConfigSlvsEcPhy(VinID, pPadConfig, 3U);
            }

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_DvpReset - Reset VIN module and reset VIN pad to Parallel mode
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pPadConfig Pointer to VIN pad configuration
 *  @return error code
 */
UINT32 AmbaVIN_DvpReset(UINT32 VinID, const AMBA_VIN_DVP_PAD_CONFIG_s *pPadConfig)
{
    (void) VinID;
    (void) pPadConfig;

    return VIN_ERR_INVALID_API;
}

/**
 *  AmbaVIN_SlvsConfig - Configure VIN to receive data via SLVS or Sub-LVDS interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinSlvsConfig Pointer to SLVS configuration
 *  @return error code
 */
UINT32 AmbaVIN_SlvsConfig(UINT32 VinID, const AMBA_VIN_SLVS_CONFIG_s *pVinSlvsConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pVinSlvsConfig == NULL)) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {
            if (IsSuccessor(VinID) == 0U) {
                /* Reset SLVS PHY when sensor bit clock is present, or PHY may not lock the clock */
                (void)AmbaRTSL_VinResetSlvsPhy(VinID);
            }

            RetVal = AmbaRTSL_VinSlvsConfig(VinID, pVinSlvsConfig);

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_MipiConfig - Configure VIN to receive data via MIPI interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinMipiConfig Pointer to MIPI configuration
 *  @return error code
 */
UINT32 AmbaVIN_MipiConfig(UINT32 VinID, const AMBA_VIN_MIPI_CONFIG_s *pVinMipiConfig)
{
    UINT32 RetVal = VIN_ERR_NONE, IsTimeOut = 0U;
    UINT32 Count = 0U;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pVinMipiConfig == NULL)) {
        RetVal = VIN_ERR_ARG;
    } else {
        if (VinID <= AMBA_VIN_CHANNEL7) { /* DCPHY */
            /* do nothing */
        } else {    /* Legacy DPHY */
            /* Reset MIPI logic inside VIN after receiving input MIPI clock from the PHY */
            if ((VinID == AMBA_VIN_CHANNEL11) ||
                (VinID == AMBA_VIN_CHANNEL12) ||
                (VinID == AMBA_VIN_CHANNEL13)) { // VIN11/12/13
                while (AmbaCSL_VinGetMipiRxClkActHs1() == 0U) {
                    if (Count++ >= MIPI_RX_CLK_ACTIVE_HS_POLLING_TIMEOUT) {
                        IsTimeOut = 1U;
                        break;
                    }
                    (void)AmbaKAL_TaskSleep(1);
                }
            } else { // VIN8/9/10
                while (AmbaCSL_VinGetMipiRxClkActHs0() == 0U) {
                    if (Count++ >= MIPI_RX_CLK_ACTIVE_HS_POLLING_TIMEOUT) {
                        IsTimeOut = 1U;
                        break;
                    }
                    (void)AmbaKAL_TaskSleep(1);
                }
            }
            /*
             * Take the Mutex
             */
            if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {
                if (IsSuccessor(VinID) == 0U) {
                    (void)AmbaRTSL_VinResetMipiLogic(VinID);
                }

                /*
                 * Release the Mutex
                 */
                if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                    /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                    RetVal = VIN_ERR_UNEXPECTED;
                }
            }
        }

        if (RetVal == VIN_ERR_NONE) {
            /*
             * Take the Mutex
             */
            if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

                RetVal = AmbaRTSL_VinMipiConfig(VinID, pVinMipiConfig);

                /*
                 * Release the Mutex
                 */
                if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                    /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                    RetVal = VIN_ERR_UNEXPECTED;
                }

            } else {
                RetVal = VIN_ERR_MUTEX;
            }
        }
    }

    return (IsTimeOut == 0U) ? RetVal : VIN_ERR_TIMEOUT;
}

/**
 *  AmbaVIN_MipiCphyConfig - Configure VIN to receive data via MIPI interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinCphyMipiConfig Pointer to MIPI configuration
 *  @return error code
 */
UINT32 AmbaVIN_MipiCphyConfig(UINT32 VinID, const AMBA_VIN_MIPI_CONFIG_s *pVinMipiCphyConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pVinMipiCphyConfig == NULL)) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinMipiCphyConfig(VinID, pVinMipiCphyConfig);

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_SlvsEcConfig - Configure VIN to receive data via SLVS-EC interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinSlvsEcConfig Pointer to SLVS-EC configuration
 *  @return error code
 */
UINT32 AmbaVIN_SlvsEcConfig(UINT32 VinID, const AMBA_VIN_SLVSEC_CONFIG_s *pVinSlvsEcConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pVinSlvsEcConfig == NULL)) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinSlvsEcConfig(VinID, pVinSlvsEcConfig);

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_DvpConfig - Configure VIN to receive data via digital video port (DVP) parallel interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinDvpConfig Pointer to DVP configuration
 *  @return error code
 */
UINT32 AmbaVIN_DvpConfig(UINT32 VinID, const AMBA_VIN_DVP_CONFIG_s *pVinDvpConfig)
{
    (void) VinID;
    (void) pVinDvpConfig;

    return VIN_ERR_INVALID_API;
}

/**
 *  AmbaVIN_MasterSyncEnable - Enable master H/V Sync generator
 *  @param[in] MSyncID Indicate Master sync pin ID
 *  @param[in] pMSyncConfig Pointer to master H/V Sync configuration
 *  @return error code
 */
UINT32 AmbaVIN_MasterSyncEnable(UINT32 MSyncID, const AMBA_VIN_MASTER_SYNC_CONFIG_s *pMSyncConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 RefClk;
    AMBA_VIN_MASTER_SYNC_CONFIG_s MSyncConfig;

    if ((MSyncID >= AMBA_NUM_VIN_MSYNC) || (pMSyncConfig == NULL)) {
        RetVal = VIN_ERR_ARG;
    } else {
        RefClk = pMSyncConfig->RefClk;
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {
            if (MSyncID == AMBA_VIN_MSYNC0) {
#ifdef AMBA_PLL_FOURFOLD_GCLK_SO_VIN
                if (RefClk != (AmbaRTSL_PllGetVin0Clk() / 4U)) {

#else
                if (RefClk != AmbaRTSL_PllGetVin0Clk()) {
#endif
                    /* The reference clock is from SENSOR2 PLL */
                    RetVal = AmbaRTSL_PllSetSensor1Clk(RefClk);
                }
            } else { /* (MSyncID == AMBA_VIN_MSYNC1) */
#ifdef AMBA_PLL_FOURFOLD_GCLK_SO_VIN
                if (RefClk != (AmbaRTSL_PllGetVin4Clk() / 4U)) {

#else
                if (RefClk != AmbaRTSL_PllGetVin4Clk()) {
#endif
                    /* The reference clock is from SENSOR PLL */
                    RetVal = AmbaRTSL_PllSetSensor0Clk(RefClk);
                }
            }

            if (RetVal == PLL_ERR_NONE) {
                if (AmbaWrap_memcpy(&MSyncConfig, pMSyncConfig, sizeof(MSyncConfig)) == ERR_NONE) {
#ifdef AMBA_PLL_FOURFOLD_GCLK_SO_VIN
                    /* Since gclk_so_vin is fourfold,
                       all the parameters with unit=cycle should be fourfold. */
                    MSyncConfig.HSync.Period = MSyncConfig.HSync.Period * 4U;
                    MSyncConfig.HSync.PulseWidth = MSyncConfig.HSync.PulseWidth * 4U;
                    MSyncConfig.VSync.PulseWidth =  MSyncConfig.VSync.PulseWidth * 4U;
                    MSyncConfig.HSyncDelayCycles = MSyncConfig.HSyncDelayCycles * 4U;
                    MSyncConfig.VSyncDelayCycles = MSyncConfig.VSyncDelayCycles * 4U;
#endif
                    RetVal = AmbaRTSL_VinMasterSyncConfig(MSyncID, &MSyncConfig);
                } else {
                    RetVal = VIN_ERR_UNEXPECTED;
                }
            }

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_MasterSyncDisable - Disable master H/V Sync generator
 *  @param[in] MSyncID Indicate Master sync pin ID
 *  @return error code
 */
UINT32 AmbaVIN_MasterSyncDisable(UINT32 MSyncID)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (MSyncID >= AMBA_NUM_VIN_MSYNC) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinResetHvSyncOutput(MSyncID);

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_SensorClkEnable - Enable clock as the sensor clock source
 *  @param[in] ClkID Indicate clock channel
 *  @param[in] Frequency Clock frequency
 *  @return error code
 */
UINT32 AmbaVIN_SensorClkEnable(UINT32 ClkID, UINT32 Frequency)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (ClkID >= AMBA_NUM_VIN_SENSOR_CLOCK) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {
            /* Enable sequence: (1) Set desired clock frequency. (2) Set clock configuration. */
            if (ClkID == AMBA_VIN_SENSOR_CLOCK0) {
                RetVal = AmbaRTSL_PllSetSensor0Clk(Frequency);
                if (RetVal == PLL_ERR_NONE) {
                    (void)AmbaRTSL_PllSetSensor0ClkDir(AMBA_PLL_SENSOR_REF_CLK_OUTPUT);
                }
            } else {
                RetVal = AmbaRTSL_PllSetSensor1Clk(Frequency);
            }

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_SensorClkDisable - Disable the clock
 *  @param[in] ClkID Indicate clock channel
 *  @return error code
 */
UINT32 AmbaVIN_SensorClkDisable(UINT32 ClkID)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (ClkID >= AMBA_NUM_VIN_SENSOR_CLOCK) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {
            /* Disable sequence: (1) Set clock configuration. (2) Set clock frequency=0 to power down the PLL. */
            if (ClkID == AMBA_VIN_SENSOR_CLOCK0) {
                (void) AmbaRTSL_PllSetSensor0ClkDir(AMBA_PLL_SENSOR_REF_CLK_INPUT);
                RetVal = AmbaRTSL_PllSetSensor0Clk(0U);
            } else { /* (ClkID == AMBA_VIN_SENSOR_CLOCK1) */
                RetVal = AmbaRTSL_PllSetSensor1Clk(0U);
            }

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_SensorClkSetDrvStrength - Adjust the driving strength of sensor clock
 *  @param[in] Value Indicate the strength
 *  @return error code
 */
UINT32 AmbaVIN_SensorClkSetDrvStrength(UINT32 Value)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (Value > AMBA_VIN_DRIVE_STRENGTH_12MA) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinSetSensorClkDrvStr(Value);

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_SetLvdsTermination - Adjust the impendance of LVDS pad
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] Value Indicate the impendance
 *  @return error code
 */
UINT32 AmbaVIN_SetLvdsTermination(UINT32 VinID, UINT32 Value)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) ||
        (Value > AMBA_VIN_TERMINATION_VALUE_MAX) ||
        (Value < AMBA_VIN_TERMINATION_VALUE_MIN)) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinSetLvdsTermination(VinID, Value);
            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_DataLaneRemap - Remap data lane. It's used when sensor output data lanes are not connected to Ambarella chip in order
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pLaneRemapConfig Pointer to lane remapping configuration
 *  @return error code
 */
UINT32 AmbaVIN_DataLaneRemap(UINT32 VinID, const AMBA_VIN_LANE_REMAP_CONFIG_s *pLaneRemapConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pLaneRemapConfig == NULL)) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinDataLaneRemap(VinID, pLaneRemapConfig);

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_MipiVirtChanConfig - It's used to configure MIPI virtual channel receiving
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pMipiVirtChanConfig Pointer to MIPI virtual channel receiving configuration
 *  @return error code
 */
UINT32 AmbaVIN_MipiVirtChanConfig(UINT32 VinID, const AMBA_VIN_MIPI_VC_CONFIG_s *pMipiVirtChanConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pMipiVirtChanConfig == NULL)) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinMipiVirtChanConfig(VinID, pMipiVirtChanConfig);

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_MipiEmbDataConfig - Configure MIPI embedded data capturing
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pMipiEmbDataConfig Pointer to MIPI embedded data configuration
 *  @return error code
 */
UINT32 AmbaVIN_MipiEmbDataConfig(UINT32 VinID, const AMBA_VIN_MIPI_EMB_DATA_CONFIG_s *pMipiEmbDataConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pMipiEmbDataConfig == NULL)) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            RetVal = AmbaRTSL_VinMipiEmbDataConfig(VinID, pMipiEmbDataConfig);

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  BoundaryCheck - Check the boundary for mipi-phy parameters
 *  @param[in] Width bus width
 *  @param[in] Count count number
 *  @return calculated count number
 */
static UINT8 BoundaryCheck(UINT32 Width, DOUBLE Count)
{
    DOUBLE CalCount = 0.0;
    DOUBLE CountMax = 0.0;

    if (AmbaWrap_pow(2.0, (DOUBLE) Width, &CountMax) == ERR_NONE) {
        if (Count > (CountMax - 1.0)) {
            CalCount = (CountMax - 1.0);
        } else if (Count == 0.0) {
            CalCount = 0.0;
        } else {
            CalCount = Count - 1.0;
        }
    }

    return (UINT8)CalCount;
}

/**
 *  AmbaVIN_CalculateMphyConfig - Calculate mipi-phy parameters
 *  @param[in] BitRate MIPI data rate (DDR)
 *  @param[out] pVinMipiTiming Pointer to mipi-phy parameters
 */
static void AmbaVIN_CalculateMphyConfig(UINT64 BitRate, AMBA_VIN_MIPI_TIMING_PARAM_s *pVinMipiTiming)
{
    UINT32 RetVal;
    DOUBLE MipiBitRate;
    DOUBLE RxRefClk;
    DOUBLE TxByteClkHS, UI;
    DOUBLE ClkPrepare, ClkPrepareSec;
    DOUBLE HsSettle;
    DOUBLE HsPrepareSec;
    DOUBLE HsZeroMinSec;
    DOUBLE HsTermMax, HsTermSecMax;
    DOUBLE ClkZeroMinSec;
    DOUBLE ClkMissMax;
    DOUBLE ClkSettle;
    DOUBLE ClkTermMax;
    DOUBLE RxDDRClkHS;
    DOUBLE InitRxMin;

    UINT8 HsSettleTime;
    UINT8 HsTermTime;
    UINT8 ClkSettleTime;
    UINT8 ClkTermTime;
    UINT8 ClkMissTime;
    UINT8 RxInitTime;

    DOUBLE WorkDouble;

    RetVal = AmbaWrap_ceil(((DOUBLE)BitRate / 1000000.0), &MipiBitRate); /* in MHz */
    RetVal |= AmbaWrap_ceil((((DOUBLE)AmbaRTSL_PllGetIdspvClk() / 2.0) / 1000000.0), &RxRefClk); /* in MHz */

    if (RetVal == ERR_NONE) {
        /********************************************************
        * TX (TxByteClkHS)
        ********************************************************/
        TxByteClkHS = MipiBitRate * 1e6 / 8.0; // in Hz
        UI = 1.0 / (MipiBitRate * 1e6);        // in seccond

        ClkPrepareSec = (38e-9 + 95e-9) / 2.0;
        ClkPrepare = ClkPrepareSec * TxByteClkHS;
        if (AmbaWrap_floor((ClkPrepare + 0.5), &ClkPrepare) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }

        // ClkZero
        ClkZeroMinSec = 300e-9 - (ClkPrepare / TxByteClkHS);

        // HsPrepare
        HsPrepareSec = ((40e-9 + (4.0 * UI)) + (85e-9 + (6.0 * UI))) / 2.0;

        // HsZero
        HsZeroMinSec = 145e-9 + (10.0 * UI) - HsPrepareSec;

        /********************************************************
        * RX
        * (RxRefClk for clock lane)
        * (RxDDRClkHS for data lane)
        *******************************************************/
        RxRefClk = RxRefClk * 1e6;
        RxDDRClkHS = MipiBitRate * 1e6 / 4.0; // in Hz

        // InitRx
        InitRxMin = 100e-6 * RxRefClk / 1024.0;
        if (AmbaWrap_floor((InitRxMin + 0.5), &InitRxMin) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }

        RxInitTime = BoundaryCheck(7, InitRxMin);

        // ClkMiss
        ClkMissMax = 60e-9 * RxRefClk;
        if (AmbaWrap_floor((ClkMissMax + 0.5), &ClkMissMax) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }
        ClkMissTime = BoundaryCheck(5, ClkMissMax);
        /* There's a ratio requirement: RxDDRClkHS > 2.5 RxRefClk
         * 2.5 is a number with margin. When this ratio not meet, it's quit easy
         * for mipi_dphy to think sensor clock is missing (not toggling).
         * Per VLSI's information on 2019/9/12, we can increase the miss_ctrl as
         * large as we can to allow faster IDSP_clk. It just increased the time for PHY
         * to be aware of real clock disappeared, which is no effect currently. */
        WorkDouble = (DOUBLE)RxDDRClkHS / 2.5;
        WorkDouble = WorkDouble / (DOUBLE)RxRefClk;
        if (WorkDouble > 1.0) {
            ClkMissTime = BoundaryCheck(5, ClkMissMax);
        } else {
            ClkMissTime = 0x1f;
        }

        // ClkTerm
        ClkTermMax = 38e-9 * RxRefClk;
        if (AmbaWrap_floor((ClkTermMax + 0.5), &ClkTermMax) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }
        ClkTermTime = BoundaryCheck(5, ClkTermMax);

        // ClkSettle
        ClkSettle = (ClkPrepareSec + ClkZeroMinSec) * RxRefClk;
        ClkSettleTime = BoundaryCheck(6, ClkSettle);

        // HsTerm
        HsTermSecMax = 35e-9 + (4.0 * UI);
        HsTermMax = HsTermSecMax * RxDDRClkHS;
        if (AmbaWrap_floor(HsTermMax, &HsTermMax) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }
        HsTermTime = BoundaryCheck(5, HsTermMax);

        // HsSettle
        HsSettle = ((HsPrepareSec - HsTermSecMax) + HsZeroMinSec) * RxDDRClkHS;
        if (AmbaWrap_floor(HsSettle, &HsSettle) != ERR_NONE) {
            RetVal |= VIN_ERR_UNEXPECTED;
        }
        HsSettleTime = BoundaryCheck(6, HsSettle);

        if ((pVinMipiTiming != NULL) && (RetVal == ERR_NONE)) {
            pVinMipiTiming->HsSettleTime  = HsSettleTime;
            pVinMipiTiming->HsTermTime    = HsTermTime;
            pVinMipiTiming->ClkSettleTime = ClkSettleTime;
            pVinMipiTiming->ClkTermTime   = ClkTermTime;
            pVinMipiTiming->ClkMissTime   = ClkMissTime;
            pVinMipiTiming->RxInitTime    = RxInitTime;
        }
    }
}


static void AmbaVIN_GetDcphyConfig(UINT64 DataRate, UINT32 PhyMode, AMBA_VIN_DCPHY_PARAM_s *pVinDcphyParam)
{
    UINT64 Idx;

    if (PhyMode == AMBA_VIN_DCPHY_MODE_CPHY) {
        Idx = DataRate / 16U * 7U / 10000000U;   /* idx: 10M sps */

        if (AmbaWrap_memcpy(&pVinDcphyParam->TimingParam, &CphyTimingParam[Idx], sizeof(AMBA_VIN_DCPHY_TIMING_s)) == ERR_NONE) {
            pVinDcphyParam->IMuxSel          = 0x2;
            pVinDcphyParam->TErrSotSync      = 0x34;
            pVinDcphyParam->HsRxBiasConCsd   = 0x3A;
            pVinDcphyParam->HsRxBiasConSc    = 0x1D;
            pVinDcphyParam->RxTermSwCsd      = 0x5;
            pVinDcphyParam->RxTermSwSc       = 0x4;     /* unused */
            pVinDcphyParam->SelDly           = 0x2;
            pVinDcphyParam->DlCompCtrl       = 0x6;
            pVinDcphyParam->ExtDlMuxEn       = 1;
            pVinDcphyParam->HsRxCntRout      = 3;
            pVinDcphyParam->RxNcCoef         = 0x1f;
            pVinDcphyParam->UiMaskCtrl       = 3;
            pVinDcphyParam->CrcPostGatingCnt = 0x15;
            pVinDcphyParam->CrcAvgSel        = 1;
            pVinDcphyParam->CrcCodeTol       = 3;
            pVinDcphyParam->CrcCodeTune      = 1;
            pVinDcphyParam->ClkLaneEnable    = 0;
        }
    } else {  /* dphy */
        Idx = DataRate / 10000000U;   /* idx: 10M sps */

        if (AmbaWrap_memcpy(&pVinDcphyParam->TimingParam, &DphyTimingParam[Idx], sizeof(AMBA_VIN_DCPHY_TIMING_s)) == ERR_NONE) {
            pVinDcphyParam->IMuxSel          = 0x0;
            pVinDcphyParam->TErrSotSync      = 0x3;
            pVinDcphyParam->HsRxBiasConCsd   = 0x2D;
            pVinDcphyParam->HsRxBiasConSc    = 0x1D;
            pVinDcphyParam->RxTermSwCsd      = 0x2;
            pVinDcphyParam->RxTermSwSc       = 0x2;
            pVinDcphyParam->SelDly           = 0x0;
            pVinDcphyParam->DlCompCtrl       = 0;
            pVinDcphyParam->ExtDlMuxEn       = 0;
            pVinDcphyParam->HsRxCntRout      = 1;
            pVinDcphyParam->RxNcCoef         = 0x0;
            pVinDcphyParam->UiMaskCtrl       = 0;
            pVinDcphyParam->CrcPostGatingCnt = 0;
            pVinDcphyParam->CrcAvgSel        = 0;
            pVinDcphyParam->CrcCodeTol       = 0;
            pVinDcphyParam->CrcCodeTune      = 0;
            pVinDcphyParam->ClkLaneEnable    = 1;
        }
    }
}

/**
 *  AmbaVIN_DelayedVSyncEnable - Generate Delayed HSYNC/VSYNC output control (Only Delay Period can be changed after first time config)
 *  @param[in] pDelayedVSyncConfig Pointer to DelayedVSync configuration
 *  @return error code
 */
UINT32 AmbaVIN_DelayedVSyncEnable(const AMBA_VIN_DELAYED_VSYNC_CONFIG_s *pDelayedVSyncConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 RefClk;
    UINT32 VsdelaySrc;

    if (pDelayedVSyncConfig == NULL) {
        RetVal = VIN_ERR_ARG;
    } else {
        RefClk = pDelayedVSyncConfig->FineAdjust.RefClk;
        VsdelaySrc = pDelayedVSyncConfig->DelayedHVsyncSource;

        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {
            /* If fine adjust is used, then config clock is necessary */
            if (RefClk != 0U) {
                if ((VsdelaySrc == AMBA_VIN_VSDLY_SRC_HV_MSYNC0)
                    || (VsdelaySrc == AMBA_VIN_VSDLY_SRC_H_MSYNC0_V_EXT)
                    || (VsdelaySrc == AMBA_VIN_VSDLY_SRC_H_EXT_V_MSYNC0)) {
                    /* Config sensor clock0 */
                    if (RefClk != AmbaRTSL_PllGetVin0Clk()) {
                        RetVal |= AmbaRTSL_PllSetSensor1Clk(RefClk);
                    }

                } else if ((VsdelaySrc == AMBA_VIN_VSDLY_SRC_HV_MSYNC1)
                           || (VsdelaySrc == AMBA_VIN_VSDLY_SRC_H_MSYNC1_V_EXT)
                           || (VsdelaySrc == AMBA_VIN_VSDLY_SRC_H_EXT_V_MSYNC1)) {
                    /* Config sensor clock1 */
                    if (RefClk != AmbaRTSL_PllGetVin4Clk()) {
                        RetVal |= AmbaRTSL_PllSetSensor0Clk(RefClk);
                    }

                } else {
                    /* No need to config sensor clock */
                    RetVal = VIN_ERR_ARG;
                }
            }

            RetVal |= AmbaRTSL_VinDelayedVSyncEnable(pDelayedVSyncConfig);

            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_DelayedVSyncDisable - Disable Delayed HSYNC/VSYNC output (Previous state and configurations will be kept)
 *  @return error code
 */
UINT32 AmbaVIN_DelayedVSyncDisable(void)
{
    UINT32 RetVal = VIN_ERR_NONE;

    /*
     * Take the Mutex
     */
    if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

        RetVal = AmbaRTSL_VinDelayedVSyncDisable();

        /*
         * Release the Mutex
         */
        if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = VIN_ERR_UNEXPECTED;
        }

    } else {
        RetVal = VIN_ERR_MUTEX;
    }

    return RetVal;
}

/**
 *  AmbaVIN_GetMainCfgBufInfo - Get VIN main buffer address
 *  @param[in] VinID Indicate VIN channel
 *  @param[out] pAddr Pointer to receive the VIN main buffer address
 *  @param[out] pSize Pointer to receive the VIN main buffer size
 *  @return error code
 */
UINT32 AmbaVIN_GetMainCfgBufInfo(UINT32 VinID, ULONG *pAddr, UINT32 *pSize)
{
    UINT32 RetVal;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pAddr == NULL) || (pSize == NULL)) {
        RetVal = VIN_ERR_ARG;
    } else {
        RetVal = AmbaCSL_VinGetMainCfgBufAddr(VinID, pAddr);

        if (RetVal == VIN_ERR_NONE) {
            *pSize = AMBA_VIN_MAIN_BUF_SIZE;
        } else {
            *pSize = 0U;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_CaptureConfig - Configure Vin capture window
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pCaptureWindow Pointer to VIN capture window
 *  @return error code
 */
UINT32 AmbaVIN_CaptureConfig(UINT32 VinID, const AMBA_VIN_WINDOW_s *pCaptureWindow)
{
    UINT32 RetVal = VIN_ERR_NONE;
    const AMBA_CSL_VIN_WINDOW_s *pCslCaptureWindow = NULL;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pCaptureWindow == NULL)) {
        RetVal = VIN_ERR_ARG;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {
            if (AmbaWrap_memcpy(&pCslCaptureWindow, &pCaptureWindow, sizeof(pCslCaptureWindow)) == ERR_NONE) {
                RetVal = AmbaCSL_VinCaptureConfig(VinID, pCslCaptureWindow);
            } else {
                RetVal = VIN_ERR_UNEXPECTED;
            }
            /*
             * Release the Mutex
             */
            if (AmbaKAL_MutexGive(&AmbaVinMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = VIN_ERR_UNEXPECTED;
            }

        } else {
            RetVal = VIN_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_GetInfo - Get Vin info
 *  @param[in] VinID Indicate VIN channel
 *  @param[out] pInfo Pointer to VIN info
 *  @return error code
 */
UINT32 AmbaVIN_GetInfo(UINT32 VinID, AMBA_VIN_INFO_s *pInfo)
{
    UINT32 RetVal = VIN_ERR_NONE;
    AMBA_CSL_VIN_INFO_s CslVinInfo;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pInfo == NULL)) {
        RetVal = VIN_ERR_ARG;
    } else {
        RetVal = AmbaCSL_VinGetInfo(VinID, &CslVinInfo);

        if (RetVal == VIN_ERR_NONE) {
            pInfo->BayerPattern  = CslVinInfo.BayerPattern;
            pInfo->ColorSpace    = CslVinInfo.ColorSpace;
            pInfo->FrameRate     = CslVinInfo.FrameRate;
            pInfo->NumDataBits   = CslVinInfo.NumDataBits;
            pInfo->NumSkipFrame  = CslVinInfo.NumSkipFrame;
            pInfo->YuvOrder      = CslVinInfo.YuvOrder;
        }
    }

    return RetVal;
}

/**
 *  AmbaVIN_IsEnabled - Get Vin enable status
 *  @param[in] VinID Indicate VIN channel
 *  @return vin enable status
 */
UINT32 AmbaVIN_IsEnabled(UINT32 VinID)
{
    UINT32 Enable = 0U;

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        Enable = 0U;
    } else {
        /*
         * Take the Mutex
         */
        if (AmbaKAL_MutexTake(&AmbaVinMutex, 100) == OK) {

            Enable = AmbaCSL_VinIsEnabled(VinID);
            /*
             * Release the Mutex
             */
            (void)AmbaKAL_MutexGive(&AmbaVinMutex);
        }
    }

    return Enable;
}
