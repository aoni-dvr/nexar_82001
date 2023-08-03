/**
 *  @file AmbaSD.c
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
 *  @details SD Controller APIs
 *
 */

#include "AmbaDef.h"
#include "AmbaWrap.h"

#include "AmbaDrvEntry.h"
#include "AmbaSD.h"
#include "AmbaSD_Ctrl.h"

#include "AmbaRTSL_SD.h"

#include "AmbaCache.h"
#ifndef AMBA_KAL_H
#include "AmbaKAL.h"
#endif

#include "AmbaMisraFix.h"
//#include "AmbaPrint.h"


#define SdCmdRetry 0x3

#define SD_CMD_TIMEOUT (1000U * AMBA_KAL_SYS_TICK_MS)
#define SD_DATA_TIMEOUT (5000U * AMBA_KAL_SYS_TICK_MS)

#define CLK_SWITCH_DELAY_MS         10U
#define CMDLINE_SWITCH_DELAY_MS     5U

static AMBA_KAL_MUTEX_t SD_WorkBufMutex[AMBA_NUM_SD_CHANNEL] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_EVENT_FLAG_t SD_EventFlag GNU_SECTION_NOZEROINIT;  /* SD global Event Flags */

typedef struct {
    AMBA_KAL_EVENT_FLAG_t   EventFlag;              /* Event Flags */
    AMBA_KAL_MUTEX_t        Mutex;                  /* Mutex */
} AMBA_SD_EVENT_CTRL_s;

static AMBA_SD_EVENT_CTRL_s AmbaSD_EventCtrl[AMBA_NUM_SD_CHANNEL] GNU_SECTION_NOZEROINIT;

static void SD_Isr(UINT32 SdChanNo, UINT32 SdIrqStatus);

UINT32 AmbaSD_WaitIrqStatus(UINT32 SdChanNo, UINT32 SdEventFlag, UINT32 AnyOrAll, UINT32 AutoClear, UINT32 Timeout);
UINT32 AmbaSD_ResetAllEventFlags(void);
#if defined(CONFIG_ENABLE_EMMC_BOOT) || defined(CONFIG_MUTI_BOOT_DEVICE)
extern UINT32 AmbaFS_PrFile2SdMount(UINT32 SdChanNo);
#endif
void GNU_WEAK_SYMBOL AmbaUserSD_DelayCtrl(UINT32 SdChanNo, AMBA_SD_INFO_CID_INFO_s *pCardID);

/*
 * Default implementations of functions (can be replaced by strong ones at link-time)
 */
void GNU_WEAK_SYMBOL AmbaUserSD_DelayCtrl(UINT32 SdChanNo, AMBA_SD_INFO_CID_INFO_s *pCardID)
{
    AmbaMisra_TouchUnused(pCardID);
    AmbaMisra_TouchUnused(&SdChanNo);
}

io_statistics_t iostat[AMBA_NUM_SD_CHANNEL] = {0};

static inline void prf2_iostat_diagram(UINT32 *lvl, UINT32 sectors)
{
    UINT32 EndStat = 0;

    for (UINT32 i = 0; i < PRF2_IOSTAT_MAX_LVL; i++) {
        UINT32 Num = ((UINT32)1U << i);
        if (sectors <= Num) {
            lvl[i]++;
            EndStat = 1U;
            break;
        }
    }

    if (EndStat == 0U) {
        lvl[PRF2_IOSTAT_MAX_LVL - 1U]++;
    }
}

#if 0
static INT32 UserSDDelayCtrlEnable = 1;

/**
 *  AmbaSD_SDDelayCtrlEnable - Enable/Disable  AmbaSD_SDDelayCtrl
 *  @param[in] Enable: 1-Enable 0-Disable
 */
void AmbaSD_SDDelayCtrlEnable(INT32 Enable)
{
    UserSDDelayCtrlEnable = Enable;
}
#endif

/**
 *  AmbaSD_Init - SD device driver initializations
 *  @return error code
 */
UINT32 AmbaSD_DrvEntry(void)
{
    AMBA_SD_CTRL_s *pSdCtrl = &(AmbaSD_Ctrl[0]);
    UINT32 RetVal = OK;
    UINT32 i;

    /* create the SD global Event Flags */
    if (AmbaKAL_EventFlagCreate(&SD_EventFlag, NULL) != OK) {
        RetVal = AMBA_SD_ERR_OTHER;  /* should never happen ! */
    } else {
        for (i = 0; i < AMBA_NUM_SD_CHANNEL; i++) {
            pSdCtrl->SdType = AMBA_SD_NOT_PRESENT;
            if (0U != AmbaWrap_memset(&pSdCtrl->CardInfo, 0x0, sizeof(AMBA_SD_CARD_INFO_s))) { /* Misrac */ };
            if (0U != AmbaWrap_memset(&pSdCtrl->CardRegs, 0x0, sizeof(AMBA_SD_INFO_REG_s))) { /* Misrac */ };

            /* Create Event Flag */
            if (AmbaKAL_EventFlagCreate(&(AmbaSD_EventCtrl[i].EventFlag), NULL) != OK) {
                RetVal = AMBA_SD_ERR_OTHER;  /* should never happen ! */
            }

            /* Create Mutex */
            if (RetVal == OK) {
                if (AmbaKAL_MutexCreate(&(AmbaSD_EventCtrl[i].Mutex), NULL) != OK) {
                    RetVal = AMBA_SD_ERR_OTHER;  /* should never happen ! */
                }
            }

            if (RetVal == OK) {
                if (AmbaKAL_MutexCreate(&SD_WorkBufMutex[i], NULL) != OK) {
                    RetVal = AMBA_SD_ERR_OTHER;  /* should never happen ! */
                }
            }

            if(RetVal != OK) {
                break;
            } else {
                pSdCtrl++;
            }
        }

        if (RetVal == OK) {
            RetVal = AmbaRTSL_SdInit();              /* SD RTSL initializations */

            if (RetVal == OK) {
                AmbaRTSL_SdIsrCallBack = SD_Isr; /* Call back function when completed transactions */
            }
        }
    }

    if (RetVal == OK) {
        RetVal = OK;
    } else {
        RetVal = AMBA_SD_ERR_OTHER;
    }

    return RetVal;
}

/**
 *  AmbaSD_SetSdSetting - Set SD Card Singal setting
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pSdSetting Pointer to the sd card signal setting
 *  @return error code
 */
UINT32 AmbaSD_SetSdSetting(UINT32 SdChanNo, const AMBA_SD_SETTING_s * pSdSetting)
{
    UINT32 RetVal = OK;

    if ((SdChanNo >= AMBA_NUM_SD_CHANNEL) || (pSdSetting == NULL)) {
        RetVal = AMBA_SD_ERR_INVALID_ARG;  /* wrong parameters */
    } else {
        if(pSdSetting->HsRdLatency != 0U) {
            AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.HsRdLatency = pSdSetting->HsRdLatency;
        }

        if(pSdSetting->InitFrequency != 0U) {
            AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.InitFrequency = pSdSetting->InitFrequency;
        }

        if(pSdSetting->MaxFrequency != 0U) {
            AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.MaxFrequency = pSdSetting->MaxFrequency;
        }

        if(pSdSetting->DetailDelay != 0U) {
            AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.DetailDelay = pSdSetting->DetailDelay;
        }

        (void) AmbaRTSL_SdSetDriveStrengthAll(SdChanNo, pSdSetting);
        if (0U != AmbaWrap_memcpy(&(AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting), pSdSetting, sizeof(AMBA_SD_SETTING_s))) { /* Misrac */ };
    }
    return RetVal;
}

/**
 *  AmbaSD_GetCardConfig - Get SD Card Singal Delay Register
 *  @param[in] SdChanNo SD Channel Number
 *  @param[out] pSdSetting Pointer to the sd card signal setting
 *  @return error code
 */
UINT32 AmbaSD_GetSdSetting(UINT32 SdChanNo, AMBA_SD_SETTING_s * pSdSetting)
{
    UINT32 RetVal = OK ;

    if ((SdChanNo >= AMBA_NUM_SD_CHANNEL) || (pSdSetting == NULL)) {
        RetVal = AMBA_SD_ERR_INVALID_ARG;  /* wrong parameters */
    } else {
        if(AmbaRTSL_SdCtrl[SdChanNo].SupportSdPhy != 0U) {
            pSdSetting->InitFrequency = AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.InitFrequency;
            pSdSetting->MaxFrequency = AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.MaxFrequency;
            pSdSetting->DetailDelay = AmbaSD_DelayCtrlGet(SdChanNo);
            pSdSetting->HsRdLatency = AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.HsRdLatency;
            if (0U != AmbaWrap_memcpy(pSdSetting, &(AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting), sizeof(AMBA_SD_SETTING_s))) { /* Misrac */ };
        }
    }
    return RetVal;
}

/**
 *  AmbaSD_SetHsRdLatency - Set SD Card high speed rdlatency
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] HsRdLatency high speed rdlatency
 *  @return error code
 */
void AmbaSD_SetHsRdLatency(UINT32 SdChanNo, UINT8 HsRdLatency)
{
    AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.HsRdLatency = HsRdLatency;
}

/**
 *  AmbaSD_GetHsRdLatency - Set SD Card high speed rdlatency
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT8 AmbaSD_GetHsRdLatency(UINT32 SdChanNo)
{
    return AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.HsRdLatency;
}

/**
 *  AmbaSD_Config - SD device driver configurations
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pSdConfig pointer to the SD controller configurations
 *  @return error code
 */
UINT32 AmbaSD_Config(UINT32 SdChanNo, const AMBA_SD_CONFIG_s *pSdConfig)
{
    UINT32 RetVal = OK;
    UINT32 SdEvent;
    static UINT32 TaskCreate = 0U;

    if ((SdChanNo >= AMBA_NUM_SD_CHANNEL) || (pSdConfig == NULL)) {
        RetVal = AMBA_SD_ERR_INVALID_ARG;  /* wrong parameters */
    } else {
        RetVal = AmbaRTSL_SdConfig(SdChanNo, pSdConfig);

        if (RetVal == OK) {
            if (pSdConfig->SystemTaskCreateCb != NULL) {
                if(TaskCreate == 0U) {
#ifdef CONFIG_ARM32
                    AMBA_KAL_TASK_t *SDMonitorTask;
                    RetVal = pSdConfig->SystemTaskCreateCb(SD_CardMonitorTaskEntry, 0x0, &SDMonitorTask);
#endif
                    if (RetVal == OK) {
                        TaskCreate = 1U;
                    }
                }
            } else {
                RetVal = AMBA_SD_ERR_INVALID_ARG;
            }
        } else {
            RetVal = AMBA_SD_ERR_OTHER;
        }

        if (RetVal != OK) {
            RetVal = AMBA_SD_ERR_OTHER;
        }

        if (RetVal == OK) {

            RetVal = AmbaSD_SetSdSetting(SdChanNo, &pSdConfig->SdSetting);

            if (RetVal == OK) {
                if (pSdConfig->SdSetting.HsRdLatency != 0U) {
                    AmbaSD_SetHsRdLatency(SdChanNo, pSdConfig->SdSetting.HsRdLatency);
                }
            }

            /* set the Event Flag */
            if (RetVal == OK) {
#ifdef CONFIG_ENABLE_EMMC_BOOT
                if (SdChanNo == AMBA_SD_CHANNEL0) {
                    RetVal = AmbaSD_CardInit(SdChanNo);

                    if (RetVal == OK) {
                        RetVal = AmbaEMMC_InitPtbBbt(1000);
                    }

                    if (RetVal == OK) {
                        RetVal = AmbaEMMC_LoadNvmRomFileTable();
                    }

                    if (RetVal == OK) {
                        (void) AmbaFS_PrFile2SdMount(SdChanNo);
                    }

                } else {
                    if (AmbaRTSL_SdCardPresent(SdChanNo) != 0U) {
                        SdEvent = ((UINT32) AMBA_SD_EVENT_CARD_DETECT << SdChanNo);
                        RetVal = AmbaSD_SetEventFlag(SdEvent); /* EVENT Flag for next SD Channel */
                    }
                }
#elif defined(CONFIG_MUTI_BOOT_DEVICE)
                if (SdChanNo == AMBA_SD_CHANNEL0) {
                    UINT32 PTBActive = 0U;

                    RetVal = AmbaSD_CardInit(SdChanNo);

                    pAmbaRTSL_EmmcSysPartTable  = &AmbaRTSL_EmmcSysPartTable;
                    pAmbaRTSL_EmmcUserPartTable  = &AmbaRTSL_EmmcUserPartTable;

                    if (RetVal == OK) {
                        RetVal = AmbaEMMC_GetActivePtbNo(&PTBActive);
                    }

                    if (RetVal == OK) {
                        RetVal = AmbaEMMC_ReadUserPTB( NULL, PTBActive, 1000U);
                    }

                    if (RetVal == OK) {
                        if ((pSdConfig->pUserPartConfig[AMBA_USER_PARTITION_DSP_uCODE].Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) == 0x40U) {
                            RetVal = AmbaEMMC_LoadNvmRomFileTable();
                        }
                    }

                    if (RetVal == OK) {
                        (void) AmbaFS_PrFile2SdMount(SdChanNo);
                    }

                } else {
                    if (AmbaRTSL_SdCardPresent(SdChanNo) != 0U) {
                        SdEvent = ((UINT32) AMBA_SD_EVENT_CARD_DETECT << SdChanNo);
                        RetVal = AmbaSD_SetEventFlag(SdEvent); /* EVENT Flag for next SD Channel */
                    }
                }
#else
                if (AmbaRTSL_SdCardPresent(SdChanNo) != 0U) {
                    SdEvent = ((UINT32) AMBA_SD_EVENT_CARD_DETECT << SdChanNo);
                    RetVal = AmbaSD_SetEventFlag(SdEvent); /* EVENT Flag for next SD Channel */
                }
#endif
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaSD_SetEventFlag - Set SD event flags
 *  @param[in] SdEventFlag SD event flags
 *  @return error code
 */
UINT32 AmbaSD_SetEventFlag(UINT32 SdEventFlag)
{
    return AmbaKAL_EventFlagSet(&SD_EventFlag, SdEventFlag);
}

/**
 *  AmbaSD_ClearEventFlag - Clear SD event flags
 *  @param[in] SdEventFlag SD event flags
 *  @return error code
 */
UINT32 AmbaSD_ClearEventFlag(UINT32 SdEventFlag)
{
    return AmbaKAL_EventFlagClear(&SD_EventFlag, SdEventFlag);
}

/**
 *  AmbaSD_WaitEventFlag - Wait for SD event flags
 *  @param[in] SdEventFlag SD event flags
 *  @param[in] AnyOrAll
 *  @param[in] AutoClear
 *  @return Current SD event flags
 */
UINT32 AmbaSD_WaitEventFlag(UINT32 SdEventFlag, UINT32 AnyOrAll, UINT32 AutoClear, UINT32 Timeout)
{
    UINT32 ActualFlags;

    if (AmbaKAL_EventFlagGet(&SD_EventFlag, SdEventFlag, AnyOrAll, AutoClear,
                             &ActualFlags, Timeout) == OK) {
        ActualFlags &= SdEventFlag;
    } else {
        ActualFlags = 0;
    }

    return ActualFlags;
}

/**
 *  AmbaSD_ClearCtrlFlag - Clear SD Ctrl flags
 *  @param[in] SdChanNo
 *  @return error code
 */
UINT32 AmbaSD_ClearCtrlFlag(UINT32 SdChanNo)
{
    AMBA_KAL_EVENT_FLAG_t *pSdFlag = &(AmbaSD_EventCtrl[SdChanNo].EventFlag);
    AMBA_KAL_EVENT_FLAG_INFO_s EventFlagInfoCurFlag;
    UINT32 RetVal = OK;

    (void) AmbaKAL_EventFlagQuery(pSdFlag, &EventFlagInfoCurFlag);

    if (EventFlagInfoCurFlag.CurrentFlags != 0U) {
        RetVal = AmbaKAL_EventFlagClear(pSdFlag, EventFlagInfoCurFlag.CurrentFlags);
    }

    return RetVal;
}

/**
 *  SdCheckIrqStatus - Check SD status after command done
 *  @param[in] Status
 *  @param[in] Flag SD event flags
 *  @return error code
 */
static inline UINT32 SdCheckIrqStatus(UINT32 Status, UINT32 Flag)
{
    UINT32 RetVal = Status;

    if (((Status & AMBA_SD_IRQ_ERROR) != 0U)) {
        if ((Status & AMBA_SD_IRQ_CMD_TIME_OUT_ERROR) != 0U) {
            RetVal = AMBA_SD_ERR_TIMEOUT;
        } else if ((Status & AMBA_SD_IRQ_CMD_CRC_ERROR) != 0U) {
            RetVal = AMBA_SD_ERR_BADCRC;
        } else if ((Status & AMBA_SD_IRQ_CMD_INDEX_ERROR) != 0U) {
            RetVal = AMBA_SD_ERR_INDEX;
        } else if ((Status & AMBA_SD_IRQ_CMD_END_BIT_ERROR) != 0U) {
            RetVal = AMBA_SD_ERR_ENDBIT;
        } else if ((Status & AMBA_SD_IRQ_DATA_TIME_OUT_ERROR) != 0U) {
            RetVal = AMBA_SD_ERR_TIMEOUT;
        } else if ((Status & AMBA_SD_IRQ_DATA_CRC_ERROR) != 0U) {
            RetVal = AMBA_SD_ERR_BADCRC;
        } else if ((Status & AMBA_SD_IRQ_DATA_END_BIT_ERROR) != 0U) {
            RetVal = AMBA_SD_ERR_ENDBIT;
        } else if ((Status & AMBA_SD_IRQ_ADMA_ERROR) != 0U) {
            RetVal = AMBA_SD_ERR_ADMA;
        } else if ((Status & AMBA_SD_IRQ_AUTO_CMD12_ERROR) != 0U) {
            RetVal = AMBA_SD_ERR_ACMD12;
        } else {
            RetVal = AMBA_SD_ERR_INVALID;
        }
    } else if ((Status & Flag) != Flag) {
        RetVal = AMBA_SD_ERR_ISR_TIMEOUT;
    } else {
        RetVal = Status;
    }

    if ((Status & AMBA_SD_IRQ_CARD_REMOVAL) != 0U) {
        RetVal = AMBA_SD_ERR_NO_CARD;
    }

    /* SD host controller generate unexcepted card insertion status during first command after controller reset in eMMC boot mode. */
    if ((Status & AMBA_SD_IRQ_CARD_INSERTION) != 0U) {
        RetVal = ClearBits(RetVal, AMBA_SD_IRQ_CARD_INSERTION);
    }

    return RetVal;
}

/**
 *  AmbaSD_ResetAllEventFlags - Reset All SD event flags
 *  @return error code
 */
UINT32 AmbaSD_ResetAllEventFlags(void)
{
    return AmbaKAL_EventFlagClear(&SD_EventFlag, 0xffffffffU);
}

/**
 *  AmbaSD_Lock - Lock SD mutex and enable SD clock
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_Lock(UINT32 SdChanNo)
{
    UINT32 RetVal = OK;
    UINT32 CardPresent;

    CardPresent = AmbaRTSL_SdCardPresent(SdChanNo);
    if (CardPresent != 0U) {
#if defined(CONFIG_ARM32) && defined(CONFIG_ENABLE_AMBALINK)
        {
            extern UINT32 AmbaIPC_SDLock(UINT32 SdChanNo);
            (void) AmbaIPC_SDLock(SdChanNo);
        }
#endif
        (void) AmbaKAL_MutexTake(&AmbaSD_EventCtrl[SdChanNo].Mutex, KAL_WAIT_FOREVER);
        AmbaRTSL_SdClockEnable(SdChanNo, 1U);
        RetVal = OK;
    } else {
        RetVal = AMBA_SD_ERR_NO_CARD;
    }

    return RetVal;
}

/**
 *  AmbaSD_Unlock - Unlock SD mutex and enable SD clock
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_Unlock(UINT32 SdChanNo)
{
    const AMBA_SD_CTRL_s *pSdCtrl = &AmbaSD_Ctrl[SdChanNo];

    if (pSdCtrl->SdType != AMBA_SD_IO_ONLY) {
        AmbaRTSL_SdClockEnable(SdChanNo, 0U);
    }
    (void) AmbaKAL_MutexGive(&AmbaSD_EventCtrl[SdChanNo].Mutex);
#if defined(CONFIG_ARM32) && defined(CONFIG_ENABLE_AMBALINK)
    {
        extern UINT32 AmbaIPC_SDUnlock(UINT32 SdChanNo);
        (void) AmbaIPC_SDUnlock(SdChanNo);
    }
#endif

    return OK;
}

/**
 *  AmbaSD_IsCardPresent - Check if SD card is present or not in the slot
 *  @param[in] SdChanNo SD Channel Number
 *  @return 0 - not Present; no-0 - Present
 */
UINT32 AmbaSD_IsCardPresent(UINT32 SdChanNo)
{
    UINT32 CardStatus;

    if (AmbaRTSL_SdCardPresent(SdChanNo) == 0U) {
        CardStatus = 0U;
    } else {
        CardStatus = 1U;
    }

    return CardStatus;
}

/**
 *  AmbaSD_IsWriteProtect - Check if SD card is write protect or not
 *  @param[in] SdChanNo SD Channel Number
 *  @return 1 - write protect; 0 - no
 */
UINT32 AmbaSD_IsWriteProtect(UINT32 SdChanNo)
{
    UINT32 RetVal;

    if (AmbaRTSL_SdGetWriteProtectPin(SdChanNo) != 0U) {
        RetVal = 1U;
    } else {
        RetVal = 0U;
    }

    return RetVal;
}

/**
 *  SD_Isr - SD ISR for Reading/Writing data
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] SdIrqStatues SD Interrupt status
 */
/* disable NEON registers usage in ISR */
#pragma GCC push_options
#pragma GCC target("general-regs-only")

static void SD_Isr(UINT32 SdChanNo, UINT32 SdIrqStatus)
{
    UINT32 SdIrqFlag = SdIrqStatus;

    /* Set Cmd & Xfer done to end request immediately, when error occur. */
    if ((SdIrqStatus & (AMBA_SD_IRQ_ERROR | AMBA_SD_IRQ_CARD_REMOVAL)) != 0U) {
        SdIrqFlag |= (AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
        AmbaRTSL_SdResetAll(SdChanNo);
    }

    (void) AmbaKAL_EventFlagSet(&(AmbaSD_EventCtrl[SdChanNo].EventFlag), SdIrqFlag);

    if ((SdIrqStatus & (AMBA_SD_IRQ_CARD_INSERTION | AMBA_SD_IRQ_CARD_REMOVAL)) != 0U) {
        /* the flag of SD Card Detect */
        (void) AmbaSD_SetEventFlag((UINT32) AMBA_SD_EVENT_CARD_DETECT << SdChanNo);
    }
}
#pragma GCC pop_options

/**
 *  AmbaSD_UhsSupportChk - Check status to support UHS mode
 *  @param[in] SdChanNo SD Channel Number
 *  @return Current SD event flags
 */
UINT32 AmbaSD_UhsSupportChk(UINT32 SdChanNo)
{
    const AMBA_SD_CTRL_s *pSdCtrl = &AmbaSD_Ctrl[SdChanNo];
    UINT32 UhsSupport;

    UhsSupport = pSdCtrl->CardInfo.MemCardInfo.UhsSupport;

    return UhsSupport;
}

/**
 *  AmbaSD_GetSpeedMode -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[out] pArgCmd6
 *  @param[in] pCmd6Status
 */
static inline void AmbaSD_GetSpeedMode(UINT32 SdChanNo, AMBA_SD_CMD6_ARGUMENT_s * pArgCmd6, const AMBA_SD_CMD6_STATUS_s * pCmd6Status)
{
    const AMBA_SD_CTRL_s *pSdCtrl = &AmbaSD_Ctrl[SdChanNo];
    UINT32 SDR104_Support, DDR50_Support, SDR50_Support, SDR25_Support;

    SDR104_Support = (((UINT32) pCmd6Status->SupportGroup1) & AMBA_SD_UHS_SDR104_SUPPORT);
    DDR50_Support = (((UINT32) pCmd6Status->SupportGroup1) & AMBA_SD_UHS_DDR50_SUPPORT);
    SDR50_Support = (((UINT32) pCmd6Status->SupportGroup1) & AMBA_SD_UHS_SDR50_SUPPORT);
    SDR25_Support = (((UINT32) pCmd6Status->SupportGroup1) & AMBA_SD_UHS_SDR25_SUPPORT);

    // For misrac check
    (void) DDR50_Support;

    if ((pSdCtrl->SdConfig.SdSetting.MaxFrequency > AMBA_SD_SDR50_MAX_FREQUENCY) && (SDR104_Support != 0U)) {
        pArgCmd6->AccessMode = AMBA_SD_UHS_SDR104_MODE;
    } else if ((pSdCtrl->SdConfig.SdSetting.MaxFrequency > AMBA_SD_SDR25_MAX_FREQUENCY) && (SDR50_Support != 0U)) {
        pArgCmd6->AccessMode = AMBA_SD_UHS_SDR50_MODE;
    } else if ((pSdCtrl->SdConfig.SdSetting.MaxFrequency > AMBA_SD_SDR12_MAX_FREQUENCY) && (SDR25_Support != 0U)) {
        pArgCmd6->AccessMode = AMBA_SD_UHS_SDR25_MODE;
    } else if (SDR25_Support != 0U) {
        pArgCmd6->AccessMode = AMBA_SD_UHS_SDR12_MODE;
    } else {
        pArgCmd6->AccessMode = AMBA_SD_UHS_SDR25_MODE;
    }

}

/**
 *  AmbaSD_GetCurrentLimit -
 *  @param[out] pArgCmd6
 *  @param[in] pCmd6Status
 */
static inline void AmbaSD_GetCurrentLimit(AMBA_SD_CMD6_ARGUMENT_s * pArgCmd6, const AMBA_SD_CMD6_STATUS_s * pCmd6Status)
{
    UINT32 CUR_800MA_Support, CUR_600MA_Support, CUR_400MA_Support;

    CUR_800MA_Support = (((UINT32) pCmd6Status->SupportGroup4) & AMBA_SD_CURRENT_800MA_SUPPORT);
    CUR_600MA_Support = (((UINT32) pCmd6Status->SupportGroup4) & AMBA_SD_CURRENT_600MA_SUPPORT);
    CUR_400MA_Support = (((UINT32) pCmd6Status->SupportGroup4) & AMBA_SD_CURRENT_400MA_SUPPORT);

    if ( CUR_800MA_Support != 0U) {
        pArgCmd6->CurrentLimit = AMBA_SD_CURRENT_800MA;
    } else if (CUR_600MA_Support != 0U) {
        pArgCmd6->CurrentLimit = AMBA_SD_CURRENT_600MA;
    } else if (CUR_400MA_Support != 0U) {
        pArgCmd6->CurrentLimit = AMBA_SD_CURRENT_400MA;
    } else {
        pArgCmd6->CurrentLimit = AMBA_SD_CURRENT_200MA;
    }
}

/**
 *  AmbaSD_GetDriveStrength -
 *  @param[out] pArgCmd6
 *  @param[in] pCmd6Status
 */
static inline void AmbaSD_GetDriveStrength(AMBA_SD_CMD6_ARGUMENT_s * pArgCmd6, AMBA_SD_CMD6_STATUS_s * pCmd6Status)
{
    AmbaMisra_TouchUnused(pCmd6Status);
    pArgCmd6->DrivingStrength = AMBA_SD_DRIVE_STG_TYPE_B;
}

static void AmbaSD_GetUhsPram(UINT32 SdChanNo, AMBA_SD_CMD6_ARGUMENT_s * pArgCmd6, AMBA_SD_CMD6_STATUS_s * pCmd6Status)
{
    AmbaSD_GetSpeedMode(SdChanNo, pArgCmd6, pCmd6Status);
    AmbaSD_GetCurrentLimit(pArgCmd6, pCmd6Status);
    AmbaSD_GetDriveStrength(pArgCmd6, pCmd6Status);
}

static UINT32 AmbaSD_ModeChk(const AMBA_SD_CMD6_ARGUMENT_s * pArgCmd6, const AMBA_SD_CMD6_STATUS_s * pCmd6Status)
{
    UINT32 RetVal = OK;

    if ((pCmd6Status->MaxCurrent == 0U)                                                                             ||
        ((pArgCmd6->AccessMode      != 0xFU) && (pCmd6Status->StatusGroup1 != pArgCmd6->AccessMode))      ||
        ((pArgCmd6->DrivingStrength != 0xFU) && (pCmd6Status->StatusGroup3 != pArgCmd6->DrivingStrength)) ||
        ((pArgCmd6->CurrentLimit    != 0xFU) && (pCmd6Status->StatusGroup4 != pArgCmd6->CurrentLimit))) {
        RetVal = AMBA_SD_ERR_OTHER;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SetupUhsMode
 *
 *  @Description:: Setup UHS mode
 *
 *  @Input      ::
 *      SdChanNo:    SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       UINT32 : Current SD event flags
 */
static UINT32 AmbaSD_SetupUhsMode(UINT32 SdChanNo)
{
    UINT32 RetVal, TempArg;
    AMBA_SD_CMD6_ARGUMENT_s ArgCmd6;
    AMBA_SD_CMD6_STATUS_s Cmd6Status GNU_ALIGNED_CACHESAFE;
    AMBA_SD_CTRL_s *pSdCtrl = &AmbaSD_Ctrl[SdChanNo];

    TempArg = 0xFU;
    AmbaMisra_TypeCast32(&ArgCmd6, &TempArg);

    if (0U != AmbaWrap_memset(&Cmd6Status, 0, sizeof(AMBA_SD_CMD6_STATUS_s))) { /* Misrac */ };

    /* Set func to 0xf to keep current function */
    ArgCmd6.Mode            = AMBA_SD_MODE_CHECK_FUNC;
    ArgCmd6.CurrentLimit    = AMBA_SD_CURRENT_FUNC;
    ArgCmd6.DrivingStrength = AMBA_SD_CURRENT_FUNC;
    ArgCmd6.AccessMode      = AMBA_SD_CURRENT_FUNC;
    RetVal = AmbaSD_SendCMD6(SdChanNo, &ArgCmd6, &Cmd6Status);

    if (RetVal == OK) {
        AmbaSD_GetUhsPram(SdChanNo, &ArgCmd6, &Cmd6Status);

        RetVal = AmbaSD_SendCMD6(SdChanNo, &ArgCmd6, &Cmd6Status);
        if (RetVal == OK) {
            RetVal = AmbaSD_ModeChk(&ArgCmd6, &Cmd6Status);
        }
    }

    if (RetVal == OK) {
        /* Function Group3 Driver Strength */
        ArgCmd6.CurrentLimit = AMBA_SD_CURRENT_FUNC;
        ArgCmd6.AccessMode   = AMBA_SD_CURRENT_FUNC;
        ArgCmd6.Mode         = AMBA_SD_MODE_SWITCH_FUNC;
        AmbaSD_GetDriveStrength(&ArgCmd6, &Cmd6Status);

        RetVal = AmbaSD_SendCMD6(SdChanNo, &ArgCmd6, &Cmd6Status);
        if (RetVal == OK) {
            RetVal = AmbaSD_ModeChk(&ArgCmd6, &Cmd6Status);
        }
    }

    if (RetVal == OK) {
        /* Function Group1,4, Speed mode Current Limit */
        AmbaSD_GetUhsPram(SdChanNo, &ArgCmd6, &Cmd6Status);
        ArgCmd6.DrivingStrength = AMBA_SD_CURRENT_FUNC;
        ArgCmd6.Mode            = AMBA_SD_MODE_SWITCH_FUNC;

        RetVal = AmbaSD_SendCMD6(SdChanNo, &ArgCmd6, &Cmd6Status);
        if (RetVal == OK) {
            RetVal = AmbaSD_ModeChk(&ArgCmd6, &Cmd6Status);
        }
    }

    if (RetVal == OK) {
        pSdCtrl->CardInfo.MemCardInfo.AccessMode = ArgCmd6.AccessMode;

        if (ArgCmd6.AccessMode == AMBA_SD_UHS_SDR104_MODE) {
            pSdCtrl->CardInfo.MemCardInfo.DesiredClock = AMBA_SD_SDR104_MAX_FREQUENCY;
        } else if (ArgCmd6.AccessMode == AMBA_SD_UHS_DDR50_MODE) {
            pSdCtrl->CardInfo.MemCardInfo.DesiredClock = AMBA_SD_DDR50_MAX_FREQUENCY;
        } else if (ArgCmd6.AccessMode == AMBA_SD_UHS_SDR50_MODE) {
            pSdCtrl->CardInfo.MemCardInfo.DesiredClock = AMBA_SD_SDR50_MAX_FREQUENCY;
        } else if (ArgCmd6.AccessMode == AMBA_SD_UHS_SDR25_MODE) {
            pSdCtrl->CardInfo.MemCardInfo.DesiredClock = AMBA_SD_SDR25_MAX_FREQUENCY;
        } else {
            pSdCtrl->CardInfo.MemCardInfo.DesiredClock = AMBA_SD_SDR12_MAX_FREQUENCY;
        }
    } else {
        RetVal = AMBA_SD_ERR_OTHER;
    }

    return RetVal;
}

static void SdGeteMMCHS200Pram(AMBA_SD_EMMC_CMD6_ARGUMENT_s *pArgCmd6, const AMBA_SD_INFO_EXT_CSD_REG_s *pExtCsd)
{
    AmbaRTSL_GeteMMCSpeedMode(pArgCmd6, pExtCsd);
    AmbaRTSL_GeteMMCDriveStrength(pArgCmd6, pExtCsd);
}

/*
 *  @RoutineName:: AmbaSD_SetupHS200Mode
 *
 *  @Description:: Setup HS200 mode
 *
 *  @Input      ::
 *      SdChanNo:    SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       UINT32 : Current SD event flags
 */
static UINT32 AmbaSD_SetupHS200Mode(UINT32 SdChanNo)
{
    UINT32 RetVal;
    AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    const AMBA_SD_INFO_REG_s *pCardRegs = &AmbaSD_Ctrl[SdChanNo].CardRegs;
    AMBA_SD_EMMC_CMD6_ARGUMENT_s ArgCmd6 = {0};

    /* HS200 only Support 4bit or 8 Bit */
    if ((pCardInfo->MemCardInfo.BusWidth == 4U) || (pCardInfo->MemCardInfo.BusWidth == 8U)) {

        /* Setup the argument of CMD6 for HS200 Timing */
        SdGeteMMCHS200Pram(&ArgCmd6, &pCardRegs->ExtCsd);
        ArgCmd6.Access = ACCESS_WRITE_BYTE;
        ArgCmd6.Index = 0xb9U;
        ArgCmd6.CmdSet = 0x2U;
        RetVal = AmbaSD_SendEmmcCMD6(SdChanNo, &ArgCmd6);


        if (RetVal == OK) {
            pCardInfo->MemCardInfo.DesiredClock =
                ((ArgCmd6.Value & 0x0fU) == EMMC_HS200_MODE) ? AMBA_EMMC_HS200_MAX_FREQUENCY :
                ((ArgCmd6.Value & 0x0fU) == EMMC_HS400_MODE) ? AMBA_EMMC_HS400_MAX_FREQUENCY :
                AMBA_EMMC_SDR52_MAX_FREQUENCY;
        }
    } else {
        RetVal = AMBA_SD_ERR_INVALID_ARG;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_WaitIrqStatus
 *
 *  @Description:: Wait for SD interrupt status
 *
 *  @Input      ::
 *      SdChanNo:    SD Channel Number
 *      SdEventFlag: SD event flags
 *      Options:     Options for this operation (AND, OR, CLEAR, etc.)
 *      Timeout:     Wait option (timer-ticks)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       UINT32 : Current SD event flags
 */
UINT32 AmbaSD_WaitIrqStatus(UINT32 SdChanNo, UINT32 SdEventFlag, UINT32 AnyOrAll, UINT32 AutoClear, UINT32 Timeout)
{
    UINT32 ActualFlags = 0, MaskFlags, GetAll;
    AMBA_KAL_EVENT_FLAG_t *pSdFlag = &(AmbaSD_EventCtrl[SdChanNo].EventFlag);

    GetAll = (AnyOrAll & 0x1U);

    if (AmbaKAL_EventFlagGet(pSdFlag, SdEventFlag, GetAll, AutoClear, &ActualFlags, Timeout) != OK) {
        //AmbaPrint_ModulePrintUInt5(AMBA_SD_PRINT_MODULE_ID, "[AmbaSD_WaitIrqStatus]CMD:%u timeout, flag= 0x%x", AmbaRTSL_SdCtrl[SdChanNo].CmdConfig.CmdIdx, ActualFlags, 0U, 0U, 0U);
    }
    if ((ActualFlags & AMBA_SD_IRQ_ERROR) != 0U) {
        //AmbaPrint_ModulePrintUInt5(AMBA_SD_PRINT_MODULE_ID, "[AmbaSD_WaitIrqStatus]CMD:%u , Acflag= 0x%x", AmbaRTSL_SdCtrl[SdChanNo].CmdConfig.CmdIdx, ActualFlags, 0U, 0U, 0U);
    }

    MaskFlags = ActualFlags & (~AMBA_SD_IRQ_CARD_REMOVAL);

    (void) AmbaKAL_EventFlagClear(pSdFlag, MaskFlags);

    return ActualFlags;
}

/*
 *  @RoutineName:: Mmc4DetermineBusWidth
 *
 *  @Description:: Determine MMC 4.0 bus width.
 *
 *  @Input      ::
 *      SdChanNo:    SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       UINT32 : Current SD event flags
 */
static UINT32 Mmc4DetermineBusWidth(UINT32 SdChanNo)
{
    UINT32 RetVal = OK;

    AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    AMBA_SD_EMMC_CMD6_ARGUMENT_s ArgCmd6 = {0};

    ArgCmd6.Access = ACCESS_WRITE_BYTE;
    ArgCmd6.Index  = 0xb7U;

#if defined(CONFIG_EMMC_ACCESS_8BIT)
    ArgCmd6.Value  = 0x2U; /* 8bit mode */
#elif defined(CONFIG_EMMC_ACCESS_4BIT)
    ArgCmd6.Value  = 0x1U; /* 4bit mode */
#elif defined(CONFIG_EMMC_ACCESS_1BIT)
    ArgCmd6.Value  = 0x0U; /* 1bit mode */
#else
    /* Use 8bit mode by default. */
    ArgCmd6.Value  = 0x2U; /* 8bit mode */
#endif

    if (ArgCmd6.Value == 0U) {
        pCardInfo->MemCardInfo.BusWidth = 1U;
    } else {
        RetVal = AmbaSD_SendEmmcCMD6(SdChanNo, &ArgCmd6);

        if (RetVal != OK) {
            pCardInfo->MemCardInfo.BusWidth = 1U;
        } else {
            pCardInfo->MemCardInfo.BusWidth = (ArgCmd6.Value == 2U) ? 8U : 4U;
        }

        AmbaRTSL_SdSetHostBusWidth(SdChanNo, pCardInfo->MemCardInfo.BusWidth);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SelfCheck
 *
 *  @Description:: Self check to see if accessing is ok.
 *
 *  @Input      ::
 *      SdChanNo:    SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       UINT32 : Current SD event flags
 */
static UINT32 AmbaSD_SelfCheck(UINT32 SdChanNo)
{
    static UINT8 SD_CheckBuf[1024] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    UINT32 RetVal;

    RetVal = AmbaSD_SendCMD17(SdChanNo, 0, SD_CheckBuf);
    if (RetVal == OK) {
        for (INT32 i = 0; i < 3; i++) {
            RetVal = AmbaSD_SendCMD18(SdChanNo, 0x4000U, (sizeof(SD_CheckBuf) / 512U), SD_CheckBuf);
            if (RetVal != OK) {
                break;
            }
        }
    }

    return RetVal;
}

static UINT32 AmbaSD_MncInit_HighSpeed(UINT32 SdChanNo)
{
    UINT32 RetVal;
    const AMBA_SD_CTRL_s      *pSdCtrl   = &AmbaSD_Ctrl[SdChanNo];
    AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    AMBA_SD_INFO_REG_s  *pCardRegs = &AmbaSD_Ctrl[SdChanNo].CardRegs;

    if(AmbaEMMC_HS200SupportChk(SdChanNo) != 0) {
        RetVal = Mmc4DetermineBusWidth(SdChanNo);
        if (RetVal == OK) {
            RetVal = AmbaSD_SetupHS200Mode(SdChanNo);
        }

        if (RetVal == OK) {
            if (pCardInfo->MemCardInfo.DesiredClock > pSdCtrl->SdConfig.SdSetting.MaxFrequency) {
                pCardInfo->MemCardInfo.DesiredClock = pSdCtrl->SdConfig.SdSetting.MaxFrequency;
            }

            (void) pSdCtrl->SetClock(pCardInfo->MemCardInfo.DesiredClock);
            pCardInfo->MemCardInfo.CurrentClock = pSdCtrl->GetClock();

            AmbaRTSL_SDSetRdLatencyCtrl(SdChanNo, AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.HsRdLatency);
            for (UINT32 i = 0 ; i < 3U; i ++) {
                RetVal = AmbaSD_SendEmmcCMD21(SdChanNo);

                if (RetVal != OK) {
                    break;
                }
            }
        }

        if (RetVal == OK) {
            /* Update the info about HStiming on ExtCsd */
            RetVal = AmbaSD_SendEmmcCMD8(SdChanNo, &pCardRegs->ExtCsd);
        }

    } else {
#if defined(CONFIG_ENABLE_EMMC_HIGHSPEED_TIMING)
        AMBA_SD_EMMC_CMD6_ARGUMENT_s ArgCmd6 = {0};

        /* The card is able to operate at higher frequence */
        ArgCmd6.Access = ACCESS_WRITE_BYTE;
        ArgCmd6.Index  = 0xb9;
        ArgCmd6.Value  = 0x1;

        RetVal = AmbaSD_SendEmmcCMD6(SdChanNo, &ArgCmd6);

        if (RetVal == OK) {
#endif
            /* Crank up the clock to desired frequcnce */
            pCardInfo->MemCardInfo.DesiredClock = 50000000U;

            if (pCardInfo->MemCardInfo.DesiredClock > pSdCtrl->SdConfig.SdSetting.MaxFrequency) {
                pCardInfo->MemCardInfo.DesiredClock = pSdCtrl->SdConfig.SdSetting.MaxFrequency;
            }

            (void) pSdCtrl->SetClock(pCardInfo->MemCardInfo.DesiredClock);
            pCardInfo->MemCardInfo.CurrentClock = pSdCtrl->GetClock();

            RetVal = Mmc4DetermineBusWidth(SdChanNo);
#if defined(CONFIG_ENABLE_EMMC_HIGHSPEED_TIMING)
        }
#endif
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_MmcCardInit
 *
 *  @Description:: Setup MMC Card
 *
 *  @Input      ::
 *      SdChanNo:    SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       UINT32 : Current SD event flags
 */
static UINT32 AmbaSD_MmcCardInit(UINT32 SdChanNo)
{
    UINT32 RetVal;
    AMBA_SD_CTRL_s      *pSdCtrl   = &AmbaSD_Ctrl[SdChanNo];
    AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    AMBA_SD_INFO_REG_s  *pCardRegs = &AmbaSD_Ctrl[SdChanNo].CardRegs;

    AMBA_SD_CMD1_ARGUMENT_s Cmd1Arg = {0};
    UINT32 Arg, Response;

    pSdCtrl->SdType = AMBA_SD_eMMC;

    (void) AmbaKAL_TaskSleep(1U);

    RetVal = AmbaSD_SendCMD0(SdChanNo);

    if (RetVal == OK) {
        Arg = 0x40ff8000U;
        AmbaMisra_TypeCast32(&Cmd1Arg, &Arg);

        /* Repeate ACMD41 at least 1 sec or until the memory becomes ready  */
        for (UINT32 PollCount = 0U; PollCount < 1000U; PollCount++) {
            RetVal = AmbaSD_SendCMD1(SdChanNo, &Cmd1Arg, &Response);
            AmbaMisra_TypeCast32(&pCardRegs->OpCondition, &Response);

            if ((pCardRegs->OpCondition.Busy == 1U) || (RetVal != OK)) {
                break;
            }
            (void)AmbaKAL_TaskSleep(1U);
        }
    }

    if (RetVal == OK) {
        pCardInfo->MemCardInfo.AddressMode = pCardRegs->OpCondition.Ccs;

        RetVal = AmbaSD_SendCMD2(SdChanNo, &pCardRegs->CardID);
    }

    if (RetVal == OK) {
        RetVal = AmbaSD_SendCMD3(SdChanNo, &pCardRegs->RelativeCardAddr);
    }

    if (RetVal == OK) {
        RetVal = AmbaSD_SendCMD9(SdChanNo, &pCardRegs->CardSpecificData);
    }

    if (RetVal == OK) {
        RetVal = AmbaSD_SendCMD7(SdChanNo, pCardRegs->RelativeCardAddr);
    }

    if (RetVal == OK) {
        RetVal = AmbaSD_SendCMD16(SdChanNo, 512);
    }

    if (RetVal == OK) {
        RetVal = AmbaSD_SendEmmcCMD8(SdChanNo, &pCardRegs->ExtCsd);
    }

    if (RetVal == OK) {
        RetVal = AmbaSD_MncInit_HighSpeed(SdChanNo);
    }

    if (RetVal == OK) {
        /* Do bus test to make sure current clock is ok */
        RetVal = AmbaSD_SelfCheck(SdChanNo);
    }

    /* See if the card is in write-protect Mode */
    pCardInfo->MemCardInfo.WriteProtect = 0;

    pCardInfo->MemCardInfo.TotalSectors =  pCardRegs->ExtCsd.Properties.SecCount;

    return RetVal;
}

static UINT32 SdSetupHighSpeedMode(UINT32 SdChanNo)
{
    UINT32 RetVal, Temp;
    AMBA_SD_CTRL_s *pSdCtrl = &AmbaSD_Ctrl[SdChanNo];
    AMBA_SD_CMD6_ARGUMENT_s ArgCmd6;
    AMBA_SD_CMD6_STATUS_s Cmd6Status;

    Temp = 0xFFFFFFFFU;
    AmbaMisra_TypeCast32(&ArgCmd6, &Temp);

    if (0U != AmbaWrap_memset(&Cmd6Status, 0, sizeof(AMBA_SD_CMD6_STATUS_s))) { /* Misrac */ };

    /* Set func to 0xf to keep current function */
    ArgCmd6.AccessMode   = AMBA_SD_ACCESS_HIGH_SPEED;
    ArgCmd6.Mode         = AMBA_SD_MODE_CHECK_FUNC;

    RetVal = AmbaSD_SendCMD6(SdChanNo, &ArgCmd6, &Cmd6Status);

    if (RetVal == OK) {

        /* The card is able to operate at higher frequence */
        if ((Cmd6Status.MaxCurrent != 0U) &&
            (Cmd6Status.StatusGroup1 != AMBA_SD_CURRENT_FUNC)) {
            AMBA_SD_CARD_INFO_s *pCardInfo = &pSdCtrl->CardInfo;

            ArgCmd6.AccessMode = AMBA_SD_ACCESS_HIGH_SPEED;
            ArgCmd6.Mode       = AMBA_SD_MODE_SWITCH_FUNC;
            RetVal = AmbaSD_SendCMD6(SdChanNo, &ArgCmd6, &Cmd6Status);

            if (RetVal == OK) {
                if ((Cmd6Status.SupportGroup1 & AMBA_SD_ACCESS_HIGH_SPEED) != 0U) {
                    pCardInfo->MemCardInfo.DesiredClock = 50000000U;
                } else {
                    pCardInfo->MemCardInfo.DesiredClock = 25000000U;
                }
            }
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: UINT32 AmbaSD_CardInit_HighSpeed
 *
 *  @Description:: Setup SD Card
 *
 *  @Input      ::
 *      SdChanNo:    SD Channel Number
 *      UhsSupport:    SD support UHS mode or not
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       UINT32 : Current SD event flags
 */
static UINT32 AmbaSD_CardInit_HighSpeed(UINT32 SdChanNo, UINT32 UhsSupport)
{
    UINT32 RetVal = OK;
    const AMBA_SD_CTRL_s      *pSdCtrl   = &AmbaSD_Ctrl[SdChanNo];
    AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    AMBA_SD_INFO_REG_s  *pCardRegs = &AmbaSD_Ctrl[SdChanNo].CardRegs;
    AMBA_SD_INFO_CID_INFO_s CardID;
    UINT64 Tsize;
    UINT16 Temp;
    UINT8  ShiftVal;

    if (UhsSupport != 0U) {
        RetVal = AmbaSD_SetupUhsMode(SdChanNo);
        //if (RetVal != OK) {
        //AmbaPrint_ModulePrintUInt5(AMBA_SD_PRINT_MODULE_ID, "SDSetupUhsMode failed(%d)", RetVal, 0U, 0U, 0U, 0U);
        //}
    } else {
        /* Set clock for Normal High Speed mode */
        pCardInfo->MemCardInfo.DesiredClock = 25000000U;
        (void) pSdCtrl->SetClock(pCardInfo->MemCardInfo.DesiredClock);
        pCardInfo->MemCardInfo.CurrentClock = pSdCtrl->GetClock();
        RetVal = SdSetupHighSpeedMode(SdChanNo);
    }

    if (RetVal == OK) {
        /* Crank up the clock to desired frequcnce */
        if (pCardInfo->MemCardInfo.DesiredClock > pSdCtrl->SdConfig.SdSetting.MaxFrequency) {
            pCardInfo->MemCardInfo.DesiredClock = pSdCtrl->SdConfig.SdSetting.MaxFrequency;
        }

        if (AmbaSD_Ctrl[SdChanNo].SdConfig.PhyCtrl != NULL) {
            if (OK != AmbaWrap_memcpy( &CardID, &pCardRegs->CardID, sizeof(CardID))) { /* Misrac */ };
            AmbaSD_Ctrl[SdChanNo].SdConfig.PhyCtrl(SdChanNo, &CardID);

            if (pCardInfo->MemCardInfo.DesiredClock > pSdCtrl->SdConfig.SdSetting.MaxFrequency) {
                pCardInfo->MemCardInfo.DesiredClock = pSdCtrl->SdConfig.SdSetting.MaxFrequency;
            }
        }

        (void) pSdCtrl->SetClock(pCardInfo->MemCardInfo.DesiredClock);
        pCardInfo->MemCardInfo.CurrentClock = pSdCtrl->GetClock();

        if (UhsSupport != 0U) {
            AMBA_SD_DETAIL_DELAY_s DetailDelay = {0};

            if (AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.DetailDelay != 0U) {
                AmbaMisra_TypeCast32(&DetailDelay, &AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.DetailDelay);
                AmbaSD_DelayCtrlSet(SdChanNo, DetailDelay.ClkOutBypass, DetailDelay.RXClkPol, DetailDelay.SbcCoreDelay, DetailDelay.SelValue,
                                    DetailDelay.DinClkPol, DetailDelay.DataCmdBypass);
            }

            if(AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.HsRdLatency != 0U) {
                AmbaRTSL_SDSetRdLatencyCtrl(SdChanNo, AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.HsRdLatency);
            }

            if ((AmbaSD_Ctrl[SdChanNo].CardInfo.MemCardInfo.AccessMode == AMBA_SD_UHS_SDR104_MODE) ||
                (AmbaSD_Ctrl[SdChanNo].CardInfo.MemCardInfo.AccessMode == AMBA_SD_UHS_SDR50_MODE)) {
                RetVal = AmbaSD_SendCMD19(SdChanNo);
            }
        }

        if (RetVal == OK) {
            RetVal = AmbaSD_SelfCheck(SdChanNo);
        }
    }

    if (RetVal == OK) {
        RetVal = AmbaSD_SendACMD51(SdChanNo, &pCardRegs->SdConfig);
    }

    if (RetVal == OK) {
        if (pCardInfo->MemCardInfo.AddressMode != 0U) {
            pCardInfo->MemCardInfo.TotalSectors = (pCardRegs->CardSpecificData.CsdVersion.Version2.DeviceSize + 1U) * 1024U;
        } else {
            Temp = (1U + pCardRegs->CardSpecificData.CsdVersion.Version1.DeviceSize);
            ShiftVal = (pCardRegs->CardSpecificData.CsdVersion.Version1.DeviceSizeMult + 2U);

            Tsize = ((UINT64) Temp << ShiftVal);

            Tsize *= (((UINT64) 1U) << pCardRegs->CardSpecificData.ReadBlockLength);
            pCardInfo->MemCardInfo.TotalSectors = (UINT32) (Tsize / 512U);
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SdCardInit
 *
 *  @Description:: Setup SD Card
 *
 *  @Input      ::
 *      SdChanNo:    SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       UINT32 : Current SD event flags
 */
static UINT32 AmbaSD_SdCardInit(UINT32 SdChanNo)
{
    UINT32 RetVal = OK;
    AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    AMBA_SD_INFO_REG_s  *pCardRegs = &AmbaSD_Ctrl[SdChanNo].CardRegs;

    AMBA_SD_ACMD41_ARGUMENT_s Acmd41Arg = {0};
    UINT32 UhsSupport = 0, OpCondition;
    INT32 PollCount;


    AmbaMisra_TypeCast32(&OpCondition, &pCardRegs->OpCondition);
    Acmd41Arg.Vdd  = OpCondition;
    Acmd41Arg.Hcs  = 1U;
    Acmd41Arg.Xpc  = 1U;
    Acmd41Arg.S18r = 1U;

    /* Repeate ACMD41 at least 1 sec or until the memory becomes ready  */
    for (PollCount = 0; PollCount < 1000; PollCount++) {
        RetVal = AmbaSD_SendACMD41(SdChanNo, &Acmd41Arg, &OpCondition);
        AmbaMisra_TypeCast32(&pCardRegs->OpCondition, &OpCondition);

        if ((pCardRegs->OpCondition.Busy == 1U) || (RetVal != OK)) {
            break;
        }
        (void) AmbaKAL_TaskSleep(1U);
    }

    if (PollCount >= 1000) {
        RetVal = AMBA_SD_ERR_UNEXPECT;
    }

    if (RetVal == OK) {
        pCardInfo->MemCardInfo.UhsSupport  = pCardRegs->OpCondition.S18a;
        pCardInfo->MemCardInfo.AddressMode = pCardRegs->OpCondition.Ccs;

        UhsSupport =  AmbaSD_UhsSupportChk(SdChanNo);


        if (UhsSupport != 0U) {
            /* Voltage switch to 1.8V */
            RetVal = AmbaSD_SendCMD11(SdChanNo);
        }

        if (RetVal == OK) {
            RetVal = AmbaSD_SendCMD2(SdChanNo, &pCardRegs->CardID);
        }

        if (RetVal == OK) {
            RetVal = AmbaSD_SendCMD3(SdChanNo, &pCardRegs->RelativeCardAddr);
        }

        if (RetVal == OK) {
            RetVal = AmbaSD_SendCMD9(SdChanNo, &pCardRegs->CardSpecificData);
        }

        if (RetVal == OK) {
            RetVal = AmbaSD_SendCMD7(SdChanNo, pCardRegs->RelativeCardAddr);
        }

        if (RetVal == OK) {
            /* Clear card detect */
            RetVal = AmbaSD_SendACMD42(SdChanNo, 0);
        }

        if (RetVal == OK) {
            /* Set the card to 4 bit data Mode */
            RetVal = AmbaSD_SendACMD6(SdChanNo, AMBA_SD_ACMD6_BUS_WIDTH_4_BIT);

            if (RetVal == OK) {
                pCardInfo->MemCardInfo.BusWidth = 4U;
            } else {
                pCardInfo->MemCardInfo.BusWidth = 1U;
            }

            AmbaRTSL_SdSetHostBusWidth(SdChanNo, pCardInfo->MemCardInfo.BusWidth);

            if (RetVal == OK) {
                RetVal = AmbaSD_SendCMD16(SdChanNo, 512);
            }
        }

        if (RetVal == OK) {
            RetVal = AmbaSD_CardInit_HighSpeed( SdChanNo, UhsSupport);
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_MemCardInit
 *
 *  @Description:: SD Memory Card Initialization
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : SUCCESS(0)/NG(-1)
 */
void AmbaSD_MemCardInit(UINT32 SdChanNo)
{
    AMBA_SD_ACMD41_ARGUMENT_s Acmd41Arg = {0};
    UINT32 OpCondition;
    UINT32 RetVal;

    AMBA_SD_CTRL_s *pSdCtrl   = &AmbaSD_Ctrl[SdChanNo];

    AmbaRTSL_SdStoreCardRca(SdChanNo, 0x0);
    AmbaRTSL_SdSetHostBusWidth(SdChanNo, 1U);
#if defined(CONFIG_ENABLE_EMMC_BOOT)
    if (SdChanNo == AMBA_SD_CHANNEL0) {
        RetVal = AmbaSD_MmcCardInit(SdChanNo);
    } else {
#elif defined(CONFIG_MUTI_BOOT_DEVICE)
    if (SdChanNo == AMBA_SD_CHANNEL0) {
        RetVal = AmbaSD_MmcCardInit(SdChanNo);
    } else {
#endif
        RetVal = AmbaSD_SendCMD0(SdChanNo);

        if (RetVal == OK) {
            RetVal = AmbaSD_SendCMD8(SdChanNo);

            if (RetVal != OK) {
                /* Reset again if card does not support SD ver2.00 */
                (void) AmbaSD_SendCMD0(SdChanNo);
            }

            RetVal = AmbaSD_SendACMD41(SdChanNo, &Acmd41Arg, &OpCondition);
            AmbaMisra_TypeCast32(&pSdCtrl->CardRegs.OpCondition, &OpCondition);


            if (RetVal == OK) {
                RetVal = AmbaSD_SdCardInit(SdChanNo);
            } else {
                RetVal = AmbaSD_MmcCardInit(SdChanNo);
            }
        }
#if defined(CONFIG_ENABLE_EMMC_BOOT)
    }
#elif defined(CONFIG_MUTI_BOOT_DEVICE)
    }
#endif

    if (RetVal != OK) {
        if (pSdCtrl->SdConfig.PowerCtrl != NULL) {
            pSdCtrl->SdConfig.PowerCtrl(SdChanNo, AMBA_SD_VDD_POWER_OFF);
        }
        AmbaRTSL_SdClockEnable(SdChanNo, 0);
        pSdCtrl->CardInfo.MemCardInfo.CardIsInit = 0U;
    } else {
        pSdCtrl->CardInfo.MemCardInfo.CardIsInit = 1U;
    }
}

static UINT32 AmbaSD_SetupSdioCard(UINT32 SdChanNo)
{
    UINT32 RetVal;
    UINT8 RegData = 0U;
    const AMBA_SD_CTRL_s            *pSdCtrl   = &AmbaSD_Ctrl[SdChanNo];
    AMBA_SD_CARD_INFO_s       *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    AMBA_SD_IO_INFO_CAP_REG_s *pCapReg   = &AmbaSD_Ctrl[SdChanNo].CardInfo.IoCardInfo.CardCapability;

    RetVal = AmbaSDIO_ReadByte(SdChanNo, 0, AMBA_SD_CARD_CAPABILITY_REG, (UINT8 *)pCapReg);

    if (RetVal == OK) {
        if ((pCapReg->LowSpeedCard == 0U) || (pCapReg->LowSpeedSupport4Bit != 0U)) {
            RetVal = AmbaSDIO_ReadByte(SdChanNo, 0, AMBA_SD_BUS_INTERFACE_CONTROL_REG, &RegData);

            if (RetVal == OK) {
                /* clear card detect, set bus width */
                RegData = (UINT8) (CD_DISABLE | SDIO_BUS_WIDTH_4_BIT);

                RetVal = AmbaSDIO_WriteByte(SdChanNo, 0, AMBA_SD_BUS_INTERFACE_CONTROL_REG, &RegData);

                if (RetVal == OK) {
                    pCardInfo->IoCardInfo.BusWidth = 4U;
                    RetVal = OK;
                } else {
                    pCardInfo->IoCardInfo.BusWidth = 1U;
                    RetVal = OK;
                }
            }
        } else {
            pCardInfo->IoCardInfo.BusWidth = 1U;
        }

        if (RetVal == OK) {
            AmbaRTSL_SdSetHostBusWidth(SdChanNo, pCardInfo->IoCardInfo.BusWidth);

            /* Set clock for High Speed mode */
            RetVal = AmbaSDIO_ReadByte(SdChanNo, 0, AMBA_SD_HIGH_SPEED_REG, &RegData);
        }

        if (RetVal == OK) {
            RegData = (UINT8) (RegData | SDIO_ENABLE_HIGH_SPEED);

            /* The card is able to operate at higher frequence */
            if (AmbaSDIO_WriteByte(SdChanNo, 0, AMBA_SD_HIGH_SPEED_REG, &RegData) == OK) {
                pCardInfo->IoCardInfo.DesiredClock = 50000000U;
            } else {
                pCardInfo->IoCardInfo.DesiredClock = 25000000U;
            }

            /* Crank up the clock to desired frequcnce */
            if (pCardInfo->IoCardInfo.DesiredClock > pSdCtrl->SdConfig.SdSetting.MaxFrequency) {
                pCardInfo->IoCardInfo.DesiredClock = pSdCtrl->SdConfig.SdSetting.MaxFrequency;
            }

            (void) pSdCtrl->SetClock(pCardInfo->IoCardInfo.DesiredClock);
            pCardInfo->IoCardInfo.CurrentClock = pSdCtrl->GetClock();
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_IoCardInit
 *
 *  @Description:: Setup MMC Card
 *
 *  @Input      ::
 *      SdChanNo:    SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       UINT32 : Current SD event flags
 */
void AmbaSD_IoCardInit(UINT32 SdChanNo)
{
    UINT32 RetVal;
    AMBA_SD_CTRL_s      *pSdCtrl    = &AmbaSD_Ctrl[SdChanNo];
    AMBA_SD_CARD_INFO_s *pCardInfo  = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    AMBA_SD_INFO_REG_s  *pCardRegs  = &AmbaSD_Ctrl[SdChanNo].CardRegs;

    AMBA_SDIO_CMD5_ARGUMENT_s   SdioCmd5Arg = {0};
    UINT32 PollCount;
    UINT32 Temp;

    pSdCtrl->SdType = AMBA_SD_IO_ONLY;

    RetVal = AmbaSD_SendCMD5(SdChanNo, &SdioCmd5Arg, &Temp);
    AmbaMisra_TypeCast32(&pSdCtrl->CardRegs.OpCondition, &Temp);

    if (RetVal == AMBA_SD_ERR_NONE) {

        AmbaMisra_TypeCast32(&SdioCmd5Arg, &pCardRegs->OpCondition);
        /* Repeate CMD5 at least 1 sec or until the memory becomes ready  */
        for (PollCount = 0U; PollCount < 1000U; PollCount++) {
            RetVal = AmbaSD_SendCMD5(SdChanNo, &SdioCmd5Arg, &Temp);
            AmbaMisra_TypeCast32(&pSdCtrl->CardRegs.OpCondition, &Temp);

            if (pCardRegs->OpCondition.Busy == 1U) {
                break;
            }

            (void) AmbaKAL_TaskSleep(1U);
        }

        if (RetVal == AMBA_SD_ERR_NONE) {
            RetVal = AmbaSD_SendCMD3(SdChanNo, &pCardRegs->RelativeCardAddr);
        }

        /* Crank up the clock to 25MHz */
        if (RetVal == AMBA_SD_ERR_NONE) {
            pCardInfo->IoCardInfo.DesiredClock = 25000000;
            (void) pSdCtrl->SetClock(pCardInfo->IoCardInfo.DesiredClock);
            pCardInfo->IoCardInfo.CurrentClock = pSdCtrl->GetClock();

            RetVal = AmbaSD_SendCMD7(SdChanNo, pCardRegs->RelativeCardAddr);
        }

        if (RetVal == AMBA_SD_ERR_NONE) {
            RetVal = AmbaSD_SetupSdioCard(SdChanNo);
        }
    }

    if (RetVal != 0U) {
        /* Turn off power and clock */
        pSdCtrl->SdConfig.PowerCtrl(SdChanNo, AMBA_SD_VDD_POWER_OFF);
        AmbaRTSL_SdClockEnable(SdChanNo, 0);
        pSdCtrl->CardInfo.IoCardInfo.CardIsInit = 0U;
    } else {
        pSdCtrl->CardInfo.IoCardInfo.CardIsInit = 1U;
    }

    return;

}

/*
 *  @RoutineName:: AmbaSD_SendCMD0
 *
 *  @Description:: Send CMD0 ((bc): Resets all cards to idle state)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : SUCCESS(0)/NG(-1)
 */
UINT32 AmbaSD_SendCMD0(UINT32 SdChanNo)
{
    UINT32 RetVal;
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        (void) AmbaRTSL_SdSendCMD0(SdChanNo, 0x0);

        RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
        RetVal = SdCheckIrqStatus(RetVal, Flag);

        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SendCMD1
 *
 *  @Description:: Send CMD1 ((bcr): Asks the card, in idlestate, to send its Operat-ing Conditions Register contents in the response on the CMD line
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : SUCCESS(0)/NG(-1)
 */
UINT32 AmbaSD_SendCMD1(UINT32 SdChanNo, const AMBA_SD_CMD1_ARGUMENT_s * pCmd1, UINT32 * pOcr)
{
    UINT32 RetVal;
    INT32 i;
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        for (i = 0; i < SdCmdRetry; i++) {

            (void) AmbaRTSL_SdSendCMD1(SdChanNo, pCmd1, pOcr);
            RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
            RetVal = SdCheckIrqStatus(RetVal, Flag);

            AmbaRTSL_SdGetResponse48Bits(SdChanNo, pOcr);

            if ((RetVal == Flag) || (RetVal == AMBA_SD_ERR_NO_CARD)) {
                break;
            }
        }
        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SendCMD2
 *
 *  @Description:: Send CMD2 ((bcr): Asks any card to send the CID numbers on the CMD line)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : SUCCESS(0)/NG(-1)
 */
UINT32 AmbaSD_SendCMD2(UINT32 SdChanNo, AMBA_SD_INFO_CID_REG_s * pCid)
{
    UINT32 RetVal;
    AMBA_SD_RESPONSE_R2_s Response;
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        (void) AmbaRTSL_SdSendCMD2(SdChanNo, pCid);

        RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
        RetVal = SdCheckIrqStatus(RetVal, Flag);

        AmbaRTSL_SdGetResponse136Bits(SdChanNo, &Response);

        pCid->ManufacturerID     = ((UINT8) AmbaRTSL_SdParseResponse(&Response, 120U,  8U));
        pCid->OemID              = ((UINT16) AmbaRTSL_SdParseResponse(&Response, 104U, 16U));
        pCid->ProductName[0]     = ((UINT8) AmbaRTSL_SdParseResponse(&Response,  96U,  8U));
        pCid->ProductName[1]     = ((UINT8) AmbaRTSL_SdParseResponse(&Response,  88U,  8U));
        pCid->ProductName[2]     = ((UINT8) AmbaRTSL_SdParseResponse(&Response,  80U,  8U));
        pCid->ProductName[3]     = ((UINT8) AmbaRTSL_SdParseResponse(&Response,  72U,  8U));
        pCid->ProductName[4]     = ((UINT8) AmbaRTSL_SdParseResponse(&Response,  64U,  8U));
        pCid->ProductRevision    = ((UINT8) AmbaRTSL_SdParseResponse(&Response,  56U,  8U));
        pCid->ProductSerialNo    = AmbaRTSL_SdParseResponse(&Response,  24U, 32U);
        pCid->ManufacturingYear  = ((UINT8) AmbaRTSL_SdParseResponse(&Response,  12U,  8U));
        pCid->ManufacturingMonth = ((UINT8) AmbaRTSL_SdParseResponse(&Response,   8U,  4U));

        /*
         * Release the Mutex
         */

        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SendCMD3
 *
 *  @Description:: Send CMD3 ((bcr): Asks the card to publish a new relative address (RCA)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : SUCCESS(0)/NG(-1)
 */
UINT32 AmbaSD_SendCMD3(UINT32 SdChanNo, UINT16 * pRCA)
{
    AMBA_SD_RESPONSE_R6_s Cmd3Response;
    UINT32 RetVal;
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE;

    if (0U != AmbaWrap_memset(&Cmd3Response, 0, sizeof(AMBA_SD_RESPONSE_R6_s))) { /* Misrac */ };

    /*
       * Take the Mutex
       */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {
        const AMBA_SD_RESPONSE_R6_s *pRespPtr = &Cmd3Response;
        UINT32 *pPtr = NULL;
        AmbaMisra_TypeCast(&pPtr, &pRespPtr);

        (void) AmbaRTSL_SdSendCMD3(SdChanNo);
        RetVal = AmbaSD_WaitIrqStatus(SdChanNo, AMBA_SD_IRQ_CMD_DONE, 1U, 1U, SD_CMD_TIMEOUT);
        RetVal = SdCheckIrqStatus(RetVal, AMBA_SD_IRQ_CMD_DONE);

        AmbaRTSL_SdGetResponse48Bits(SdChanNo, pPtr);

        (*pRCA) = Cmd3Response.PublishedRCA;
        AmbaRTSL_SdStoreCardRca(SdChanNo, (*pRCA));
        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SendCMD5
 *
 *  @Description:: Send CMD5 ((SDIO): The function of CMD5 for SDIO cards is similar to the operation of ACMD41 for SD memory card)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaSD_SendCMD5(UINT32 SdChanNo, const AMBA_SDIO_CMD5_ARGUMENT_s *pSdioCmd5Arg, UINT32 *pOcr)
{
    UINT32 RetVal = 0U;
    AMBA_SDIO_RESPONSE_R4_s SdioR4 = {0};
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE;
    UINT32 Temp;

    AMBA_SD_CTRL_s      *pSdCtrl   = &AmbaSD_Ctrl[SdChanNo];
    /*
     * Take the Mutex
     */
    if (AmbaSD_Lock(SdChanNo) == OK) {

        (void) AmbaRTSL_SdSendCMD5(SdChanNo, pSdioCmd5Arg);
        RetVal = AmbaSD_WaitIrqStatus(SdChanNo, AMBA_SD_IRQ_CMD_DONE, 1U, 1U, SD_CMD_TIMEOUT);
        RetVal = SdCheckIrqStatus(RetVal, Flag);

        /* Get response */
        AmbaRTSL_SdGetResponse48Bits(SdChanNo, &Temp);
        AmbaMisra_TypeCast32(&SdioR4, &Temp);

        pSdCtrl->NumIoFunction = SdioR4.NumIoFunction;

        if(pOcr != NULL) {
            AmbaMisra_TypeCast32(pOcr, &SdioR4);
        }
        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/**
 *  _SWAPWORD -
 *  @param[in] data
 *  @return
 */
static UINT16 SWAPWORD(UINT16 data)
{
    return (UINT16) ((data << 8U) | (data >> 8U));
}

/**
 *  GetCMD6Data -
 *  @param[out] pCmd6Status
 */
static void GetCMD6Data(AMBA_SD_CMD6_STATUS_s * pCmd6Status)
{
    UINT8 Tmp;

    Tmp = pCmd6Status->StatusGroup6;
    pCmd6Status->StatusGroup6 = pCmd6Status->StatusGroup5;
    pCmd6Status->StatusGroup5 = Tmp;

    Tmp = pCmd6Status->StatusGroup4;
    pCmd6Status->StatusGroup4 = pCmd6Status->StatusGroup3;
    pCmd6Status->StatusGroup3 = Tmp;

    Tmp = pCmd6Status->StatusGroup2;
    pCmd6Status->StatusGroup2 = pCmd6Status->StatusGroup1;
    pCmd6Status->StatusGroup1 = Tmp;

    pCmd6Status->MaxCurrent    = SWAPWORD((UINT16)(pCmd6Status->MaxCurrent    & 0xffffU));
    pCmd6Status->SupportGroup6 = SWAPWORD((UINT16)(pCmd6Status->SupportGroup6 & 0xffffU));
    pCmd6Status->SupportGroup5 = SWAPWORD((UINT16)(pCmd6Status->SupportGroup5 & 0xffffU));
    pCmd6Status->SupportGroup4 = SWAPWORD((UINT16)(pCmd6Status->SupportGroup4 & 0xffffU));
    pCmd6Status->SupportGroup3 = SWAPWORD((UINT16)(pCmd6Status->SupportGroup3 & 0xffffU));
    pCmd6Status->SupportGroup2 = SWAPWORD((UINT16)(pCmd6Status->SupportGroup2 & 0xffffU));
    pCmd6Status->SupportGroup1 = SWAPWORD((UINT16)(pCmd6Status->SupportGroup1 & 0xffffU));

    pCmd6Status->BusyGroup6    = SWAPWORD((UINT16)(pCmd6Status->BusyGroup6 & 0xffffU));
    pCmd6Status->BusyGroup5    = SWAPWORD((UINT16)(pCmd6Status->BusyGroup5 & 0xffffU));
    pCmd6Status->BusyGroup4    = SWAPWORD((UINT16)(pCmd6Status->BusyGroup4 & 0xffffU));
    pCmd6Status->BusyGroup3    = SWAPWORD((UINT16)(pCmd6Status->BusyGroup3 & 0xffffU));
    pCmd6Status->BusyGroup2    = SWAPWORD((UINT16)(pCmd6Status->BusyGroup2 & 0xffffU));
    pCmd6Status->BusyGroup1    = SWAPWORD((UINT16)(pCmd6Status->BusyGroup1 & 0xffffU));
}

/**
 *  AmbaSD_SendCMD6 - Send CMD6 ((adtc): Checks switchable function (mode 0) and switch card function (mode 1))
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pArgCmd6
 *  @param[out] pCmd6Status
 *  return error code
 */
UINT32 AmbaSD_SendCMD6(UINT32 SdChanNo, const AMBA_SD_CMD6_ARGUMENT_s * pArgCmd6, AMBA_SD_CMD6_STATUS_s * pCmd6Status)
{
    AMBA_SD_INFO_CSR_REG_s CardStatus;
    UINT32 Flag = (AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
    UINT32 RetVal;
    UINT32 Temp;
    INT32 i;

    if (0U != AmbaWrap_memset(&CardStatus, 0, sizeof(AMBA_SD_INFO_CSR_REG_s))) { /* Misrac */ };
    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        for (i = 0; i < SdCmdRetry; i++) {
            (void) AmbaRTSL_SdSendCMD6(SdChanNo, pArgCmd6, pCmd6Status);
            RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
            RetVal = SdCheckIrqStatus(RetVal, Flag);
            if ((RetVal == Flag) || (RetVal == AMBA_SD_ERR_NO_CARD)) {
                break;
            }
        }

        AmbaRTSL_SdGetResponse48Bits(SdChanNo, &Temp);
        AmbaMisra_TypeCast32(&CardStatus, &Temp);

        GetCMD6Data(pCmd6Status);
        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/**
 *  AmbaSD_SendEmmcCMD6 - Switches the mode of operation of the selected card or modifies the EXT_CSD registers (CMD6).
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pArgCmd6
 *  @return error code
 */
UINT32 AmbaSD_SendEmmcCMD6(UINT32 SdChanNo, const AMBA_SD_EMMC_CMD6_ARGUMENT_s *pArgCmd6)
{
    AMBA_SD_INFO_CSR_REG_s CardStatus = {0};
    UINT32 Temp;
    UINT32 Flag = (AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
    UINT32 RetVal;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        (void) AmbaRTSL_SdSendEmmcCMD6(SdChanNo, pArgCmd6);
        RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
        RetVal = SdCheckIrqStatus(RetVal, Flag);

        AmbaRTSL_SdGetResponse48Bits(SdChanNo, &Temp);
        AmbaMisra_TypeCast32(&CardStatus, &Temp);

        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/**
 *  AmbaSD_SendCMD7 - Send CMD7 ((ac): Toggles a card between the stand-by and transfer states or programming and disconnect states)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] RCA
 *  @return error code
 */
UINT32 AmbaSD_SendCMD7(UINT32 SdChanNo, UINT16 RCA)
{
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE;
    UINT32 RetVal = OK;

    /*
     * Take the Mutex
     */
    if (AmbaSD_Lock(SdChanNo) == OK) {

        (void) AmbaRTSL_SdSendCMD7(SdChanNo, RCA);
        RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
        RetVal = SdCheckIrqStatus(RetVal, Flag);
        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/**
 *  AmbaSD_SendCMD8 - Send CMD8 ((bcr): Sends SD Memory Card interface condition)
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_SendCMD8(UINT32 SdChanNo)
{
    AMBA_SD_RESPONSE_R7_s Cmd8Response;
    AMBA_SD_CMD8_ARGUMENT_s Cmd8Arg;
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE;
    UINT32 RetVal;
    UINT32 Temp;

    if (0U != AmbaWrap_memset(&Cmd8Response, 0, sizeof(AMBA_SD_RESPONSE_R7_s))) { /* Misrac */ };
    if (0U != AmbaWrap_memset(&Cmd8Arg, 0, sizeof(AMBA_SD_CMD8_ARGUMENT_s))) { /* Misrac */ };
    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        Cmd8Arg.CheckPattern  = SD_CHECK_PATTERN;
        Cmd8Arg.SupplyVoltage = SD_HIGH_VOLTAGE;

        (void) AmbaRTSL_SdSendCMD8(SdChanNo, &Cmd8Arg);
        RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
        RetVal = SdCheckIrqStatus(RetVal, Flag);

        AmbaRTSL_SdGetResponse48Bits(SdChanNo, &Temp);
        AmbaMisra_TypeCast32(&Cmd8Response, &Temp);

        if (Cmd8Response.EchoBack != Cmd8Arg.CheckPattern) {
            RetVal = AMBA_SD_ERR_UNEXPECT;
        }
        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/**
 *  AmbaSD_SendMmcCMD8 - Send CMD6 ((adtc): Checks switchable function (mode 0) and switch card function (mode 1))
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pExtCsd
 *  @return error code
 */
UINT32 AmbaSD_SendEmmcCMD8(UINT32 SdChanNo, AMBA_SD_INFO_EXT_CSD_REG_s *pExtCsd)
{
    AMBA_SD_INFO_CSR_REG_s CardStatus = {0};
    UINT32 Flag = (AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
    UINT32 RetVal;
    UINT32 Temp;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        for (INT32 i = 0; i < SdCmdRetry; i++) {
            (void) AmbaRTSL_SdSendEmmcCMD8(SdChanNo, pExtCsd);
            RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
            RetVal = SdCheckIrqStatus(RetVal, Flag);
            if ((RetVal == Flag) || (RetVal == AMBA_SD_ERR_NO_CARD)) {
                break;
            }
        }

        AmbaRTSL_SdGetResponse48Bits(SdChanNo, &Temp);
        AmbaMisra_TypeCast32(&CardStatus, &Temp);

        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/**
 *  AmbaSD_SendCMD9 - Send CMD9 ((ac): Addressed card sends its card-specific data (CSD) on the CMD line)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[out] pCsd
 *  @return error code
 */
UINT32 AmbaSD_SendCMD9(UINT32 SdChanNo, AMBA_SD_INFO_CSD_REG_s *pCsd)
{
    AMBA_SD_RESPONSE_R2_s Cmd9Response;
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE;
    UINT32 RetVal;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        (void) AmbaRTSL_SdSendCMD9(SdChanNo);
        RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
        RetVal = SdCheckIrqStatus(RetVal, Flag);

        AmbaRTSL_SdGetResponse136Bits(SdChanNo, (AMBA_SD_RESPONSE_R2_s *)&Cmd9Response);

        pCsd->CsdStructure       = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response, 126U,  2U));
        pCsd->Taac               = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response, 112U,  8U));
        pCsd->Nsac               = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response, 104U,  8U));
        pCsd->TranSpeed          = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  96U,  8U));
        pCsd->CardCommandClass   = ((UINT16) AmbaRTSL_SdParseResponse(&Cmd9Response,  84U, 12U));
        pCsd->ReadBlockLength    = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  80U,  4U));
        pCsd->ReadBlockPartial   = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  79U,  1U));
        pCsd->WriteBlockMisalign = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  78U,  1U));
        pCsd->ReadBlockMisalign  = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  77U,  1U));
        pCsd->DsrImplemented     = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  76U,  1U));

        if (pCsd->CsdStructure != 0U) {
            pCsd->CsdVersion.Version2.DeviceSize = AmbaRTSL_SdParseResponse(&Cmd9Response, 48U, 22U);
        } else {
            pCsd->CsdVersion.Version1.DeviceSize         = ((UINT16) AmbaRTSL_SdParseResponse(&Cmd9Response,  62U, 12U));
            pCsd->CsdVersion.Version1.MaxReadCurrentMin  = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  59U, 3U));
            pCsd->CsdVersion.Version1.MaxReadCurrentMax  = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  56U, 3U));
            pCsd->CsdVersion.Version1.MaxWriteCurrentMin = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  53U, 3U));
            pCsd->CsdVersion.Version1.MaxWriteCurrentMax = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  50U, 3U));
            pCsd->CsdVersion.Version1.DeviceSizeMult     = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  47U, 3U));
        }

        pCsd->EraseBlockEnable   = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  46U,  1U));
        pCsd->SectorSize         = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  39U,  7U));

        pCsd->WriteGroupSize     = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  32U,  7U));
        pCsd->WirteGroupEnable   = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  31U,  1U));
        pCsd->WriteSpeedFactor   = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  26U,  3U));
        pCsd->WirteBlockLength   = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  22U,  4U));
        pCsd->WriteBlockPartial  = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  21U,  1U));
        pCsd->FileFormatGroup    = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  15U,  1U));
        pCsd->Copy               = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  14U,  1U));
        pCsd->PermWriteProtect   = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  13U,  1U));
        pCsd->TempWriteProtect   = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  12U,  1U));
        pCsd->FileFormat         = ((UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  10U,  2U));

        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/**
 *  AmbaSD_SendCMD11 - Send CMD11 ((acr): Switch to 1.8V bus signal level)
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_SendCMD11(UINT32 SdChanNo)
{
    const AMBA_SD_CTRL_s *pSdCtrl = &AmbaSD_Ctrl[SdChanNo];
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE;
    UINT32 RetVal;
    INT32 Retry = 0;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        (void) AmbaRTSL_SdSendCMD11(SdChanNo);
        RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1, 1, SD_CMD_TIMEOUT);
        RetVal = SdCheckIrqStatus(RetVal, Flag);

        if (RetVal == Flag) {
            RetVal = OK;
        }

        if (RetVal == OK) {
            /* check DAT line switch to level low */
            while(AmbaRTSL_SdGetLowDataPinStatus(SdChanNo) != 0U) {
                (void) AmbaKAL_TaskSleep(1U);
                Retry ++;
                if (Retry > 3) {
                    (void) AmbaSD_Unlock(SdChanNo);
                    //AmbaPrint_ModulePrintStr5(AMBA_SD_PRINT_MODULE_ID, "CMD 11 Data Pin without pull low", NULL, NULL, NULL, NULL, NULL);
                    RetVal = AMBA_SD_ERR_UNEXPECT;
                    break;
                }
            }

            if (RetVal == OK) {
                AmbaRTSL_SdClockEnable(SdChanNo, 0U);

                if (pSdCtrl->SdConfig.PowerCtrl != NULL) {
                    pSdCtrl->SdConfig.PowerCtrl(SdChanNo, AMBA_SD_VDD_1D80V);
                }

                /* wait 5ms for switching voltage */
                (void) AmbaKAL_TaskSleep(CLK_SWITCH_DELAY_MS);

                (void) pSdCtrl->SetClock(25000000U);

                /* output 1.8v SDCLK */
                AmbaRTSL_SdClockEnable(SdChanNo, 1U);
                /* wait 1 ms from start providing SDCLK */
                (void)AmbaKAL_TaskSleep(CMDLINE_SWITCH_DELAY_MS);

                /* check DAT line switch to level high */
                Retry = 0;
                while(AmbaRTSL_SdGetLowDataPinStatus(SdChanNo) != 0xfU) {
                    (void) AmbaKAL_TaskSleep(1U);
                    Retry ++;
                    if (Retry > 3) {
                        (void) AmbaSD_Unlock(SdChanNo);
                        //AmbaPrint_ModulePrintStr5(AMBA_SD_PRINT_MODULE_ID, "CMD 11 Data Pin without pull high", NULL, NULL, NULL, NULL, NULL);
                        RetVal = AMBA_SD_ERR_UNEXPECT;
                        break;
                    }
                }
                if (RetVal == OK) {
                    (void) pSdCtrl->SetClock(pSdCtrl->SdConfig.SdSetting.InitFrequency);
                }
            }
            /*
             * Release the Mutex
             */
            (void) AmbaSD_Unlock(SdChanNo);
        }
    }

    return RetVal;
}

/**
 *  AmbaSD_SendCMD12 - Send CMD12 ((ac): Forces the card to stop transmission)
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_SendCMD12(UINT32 SdChanNo)
{
    UINT32 RetVal;
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        (void) AmbaRTSL_SdSendCMD12(SdChanNo);
        RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
        RetVal = SdCheckIrqStatus(RetVal, AMBA_SD_IRQ_CMD_DONE);

        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/**
 *  AmbaSD_SendCMD16 - Send CMD16 ((ac): Set block length)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] BlockLength
 *  @return error code
 */
UINT32 AmbaSD_SendCMD16(UINT32 SdChanNo, UINT32 BlockLength)
{
    UINT32 RetVal;
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        (void) AmbaRTSL_SdSendCMD16(SdChanNo, BlockLength);
        RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
        if ((RetVal & AMBA_SD_IRQ_ERROR) != 0U) {
            RetVal = AMBA_SD_ERR_INVALID;
        }
        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/**
 *  AmbaSD_SendCMD17 - Send CMD17 ((adtc): Read one block)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Sector start sector
 *  @param[in] pBuf pointer to the data buffer
 *  @return error code
 */
UINT32 AmbaSD_SendCMD17(UINT32 SdChanNo, UINT32 Sector, UINT8 * pBuf)
{
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE;
    const AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    UINT32 RetVal;
    UINT32 SectorNo = Sector;
    INT32 i;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        if (pCardInfo->MemCardInfo.AddressMode == 0U) {
            SectorNo = Sector * 512U;
        }

        for (i = 0; i < SdCmdRetry; i++) {
            (void) AmbaRTSL_SdSendCMD17(SdChanNo, SectorNo, pBuf);
            RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_DATA_TIMEOUT);
            RetVal = SdCheckIrqStatus(RetVal, Flag);
            if ((RetVal == Flag) || (RetVal == AMBA_SD_ERR_NO_CARD)) {
                break;
            }
        }

        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/**
 *  AmbaSD_SendCMD18 - Send CMD18 ((adtc): Read multiple blocks)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Sector start sector
 *  @param[in] Sectors
 *  @param[in] pBuf pointer to the data buffer
 *  @return error code
 */
UINT32 AmbaSD_SendCMD18(UINT32 SdChanNo, UINT32 Sector, UINT32 Sectors, UINT8 * pBuf)
{
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE;
    UINT32 RetVal;
    const AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    UINT32 SectorNo = Sector;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        if (pCardInfo->MemCardInfo.AddressMode == 0U) {
            SectorNo = Sector * 512U;
        }

        for (INT32 i = 0; i < SdCmdRetry; i++) {
            (void) AmbaRTSL_SdSendCMD18(SdChanNo, SectorNo, Sectors, pBuf);
            RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1, 1, SD_DATA_TIMEOUT);
            RetVal = SdCheckIrqStatus(RetVal, Flag);
            if ((RetVal == Flag) || (RetVal == AMBA_SD_ERR_NO_CARD)) {
                break;
            }
        }

        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/**
 *  AmbaSD_SendCMD19 - Send CMD19 ((adtc): 64 bytes tuning pattern is sent for SDR50 & SDR104)
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_SendCMD19(UINT32 SdChanNo)
{
    UINT32 Flag = (AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
    UINT32 RetVal;
    INT32 CmpResult;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        if (0U != AmbaWrap_memset(AmbaRTSL_SdCmd19TuningBlkPattern, 0x0, AMBA_SD_TUNING_BLOCK_BYTE_SIZE)) { /* Misrac */ };

        (void) AmbaRTSL_SdSendCMD19(SdChanNo, AmbaRTSL_SdCmd19TuningBlkPattern);
        RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
        RetVal = SdCheckIrqStatus(RetVal, Flag);

        if (RetVal == Flag) {
            if (0U != AmbaWrap_memcmp(AmbaRTSL_SdFixedTuningBlkPattern, AmbaRTSL_SdCmd19TuningBlkPattern, AMBA_SD_TUNING_BLOCK_BYTE_SIZE, &CmpResult)) { /* Misrac */ };

            if (CmpResult != 0) {
                RetVal = AMBA_SD_ERR_CHECK_PATTERN;
            } else {
                RetVal = OK;
            }
        }

        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SendEmmcCMD21
 *
 *  @Description:: Send CMD21 ((adtc): 64 bytes or 128Bytes tuning pattern is sent for HS200)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaSD_SendEmmcCMD21(UINT32 SdChanNo)
{
    UINT32 Flag = (AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
    const AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    INT32 RVal = 0;
    UINT32 RetVal = OK;
    INT32  CmpResult;

    /*
     * Take the Mutex
     */
    if (AmbaSD_Lock(SdChanNo) == OK) {

        if (0U != AmbaWrap_memset(AmbaRTSL_SdEmmcCmd21TuningBlkPattern, 0x0, 2U * AMBA_SD_TUNING_BLOCK_BYTE_SIZE)) { /* Misrac */ };

        for (INT32 i = 0; i < SdCmdRetry; i++) {
            (void) AmbaKAL_EventFlagClear(&AmbaSD_EventCtrl[SdChanNo].EventFlag, 0xffffffffU);
            if (pCardInfo->MemCardInfo.BusWidth == 4U) {
                RVal = AmbaRTSL_SdSendEmmcCMD21(SdChanNo, AmbaRTSL_SdEmmcCmd21TuningBlkPattern, AMBA_SD_TUNING_BLOCK_BYTE_SIZE);
            } else {
                RVal = AmbaRTSL_SdSendEmmcCMD21(SdChanNo, AmbaRTSL_SdEmmcCmd21TuningBlkPattern, 2U * AMBA_SD_TUNING_BLOCK_BYTE_SIZE);
            }

            if (RVal == 0) {
                RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
            } else {
                RetVal = AMBA_SD_ERR_OTHER;
            }

            if (RetVal == Flag) {
                RetVal = SdCheckIrqStatus(RetVal, Flag);

                if ((RetVal & Flag) != Flag) {
                    continue;
                } else {
                    RetVal = OK;
                }
            }

            if (RetVal == OK) {
                if (pCardInfo->MemCardInfo.BusWidth == 4U) {
                    if (0U != AmbaWrap_memcmp(AmbaRTSL_SdFixedTuningBlkPattern,
                                              AmbaRTSL_SdEmmcCmd21TuningBlkPattern,
                                              AMBA_SD_TUNING_BLOCK_BYTE_SIZE, &CmpResult)) { /* Misrac */ };
                    if (CmpResult != 0) {
                        RetVal = AMBA_SD_ERR_CHECK_PATTERN;
                    }
                } else {
                    if (0U != AmbaWrap_memcmp(AmbaRTSL_SdEmm8bitTuningBlkPattern,
                                              AmbaRTSL_SdEmmcCmd21TuningBlkPattern,
                                              2U * AMBA_SD_TUNING_BLOCK_BYTE_SIZE, &CmpResult)) { /* Misrac */ };

                    if (CmpResult != 0) {
                        RetVal = AMBA_SD_ERR_CHECK_PATTERN;
                    }
                }

                if (RetVal != OK) {
                    break;
                }
            }
        }

        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SendCMD24
 *
 *  @Description:: Send CMD24 ((adtc): Write one block)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : SUCCESS(0)/NG(-1)
 */
UINT32 AmbaSD_SendCMD24(UINT32 SdChanNo, UINT32 Sector, UINT8 * pBuf)
{
    UINT32 Flag = (AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
    const AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    UINT32 RetVal;
    INT32 i;
    UINT32 SectorNo = Sector;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        if (pCardInfo->MemCardInfo.AddressMode == 0U) {
            SectorNo = Sector * 512U;
        }

        for (i = 0; i < SdCmdRetry; i++) {
            (void) AmbaRTSL_SdSendCMD24(SdChanNo, SectorNo, pBuf);
            RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_DATA_TIMEOUT);
            RetVal = SdCheckIrqStatus(RetVal, Flag);
            if ((RetVal == Flag) || (RetVal == AMBA_SD_ERR_NO_CARD)) {
                break;
            }
        }

        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SendCMD25
 *
 *  @Description:: Send CMD25 ((adtc): Write multiple blocks
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : SUCCESS(0)/NG(-1)
 */
UINT32 AmbaSD_SendCMD25(UINT32 SdChanNo, UINT32 Sector, UINT32 Sectors, UINT8 * pBuf)
{
    UINT32 Flag = (AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
    const AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    UINT32 RetVal;
    INT32 i;
    UINT32 SectorNo = Sector;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        if (pCardInfo->MemCardInfo.AddressMode == 0U) {
            SectorNo = Sector * 512U;
        }

        for (i = 0; i < SdCmdRetry; i++) {
            (void) AmbaRTSL_SdSendCMD25(SdChanNo, SectorNo, Sectors, pBuf);
            RetVal |= AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_DATA_TIMEOUT);
            RetVal = SdCheckIrqStatus(RetVal, Flag);
            if ((RetVal == Flag) || (RetVal == AMBA_SD_ERR_NO_CARD)) {
                break;
            }
        }

        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SendCMD32
 *
 *  @Description:: Send CMD32 (ac): Sets the address of the first write block to be erased
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaSD_SendCMD32(UINT32 SdChanNo, UINT32 StartSector)
{
    UINT32 Flag = (AMBA_SD_IRQ_CMD_DONE);
    UINT32 RetVal;
    AMBA_SD_RESPONSE_R1_s Response;
    UINT32 Temp;
    AMBA_SD_INFO_REG_s *pCardRegs = &AmbaSD_Ctrl[SdChanNo].CardRegs;

    Response.CardStatus = 0U;
    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        (void) AmbaRTSL_SdSendCMD32(SdChanNo, StartSector);
        RetVal = AmbaSD_WaitIrqStatus(SdChanNo, AMBA_SD_IRQ_CMD_DONE, 1U, 1U, SD_CMD_TIMEOUT);
        RetVal = SdCheckIrqStatus(RetVal, AMBA_SD_IRQ_CMD_DONE);

        AmbaRTSL_SdGetResponse48Bits(SdChanNo, &Temp);
        AmbaMisra_TypeCast32(&Response, &Temp);

        AmbaMisra_TypeCast32(&pCardRegs->CardStatus, &Response.CardStatus);
        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
        if (RetVal == Flag) {
            RetVal = OK;
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SendCMD33
 *
 *  @Description:: Send CMD33 (ac): Sets the address of the last write block of the continuous range to be erased
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaSD_SendCMD33(UINT32 SdChanNo, UINT32 EndSector)
{
    UINT32 Flag = (AMBA_SD_IRQ_CMD_DONE);
    UINT32 RetVal;
    AMBA_SD_RESPONSE_R1_s Response;
    AMBA_SD_INFO_REG_s *pCardRegs = &AmbaSD_Ctrl[SdChanNo].CardRegs;

    Response.CardStatus = 0U;
    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        (void) AmbaRTSL_SdSendCMD33(SdChanNo, EndSector);
        RetVal = AmbaSD_WaitIrqStatus(SdChanNo, AMBA_SD_IRQ_CMD_DONE, 1U, 1U, SD_CMD_TIMEOUT);
        RetVal = SdCheckIrqStatus(RetVal, AMBA_SD_IRQ_CMD_DONE);

        AmbaRTSL_SdGetResponse48Bits(SdChanNo, &Response.CardStatus);

        AmbaMisra_TypeCast32(&pCardRegs->CardStatus, &Response.CardStatus);
        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
        if (RetVal == Flag) {
            RetVal = OK;
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SendCMD38
 *
 *  @Description:: Send CMD38 (ac): (ac): Erases all previously selected write blocks
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaSD_SendCMD38(UINT32 SdChanNo)
{
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE;
    UINT32 RetVal;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        (void) AmbaRTSL_SdSendCMD38(SdChanNo);
        RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, (10000U * AMBA_KAL_SYS_TICK_MS));
        RetVal = SdCheckIrqStatus(RetVal, Flag);

        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
        if (RetVal == Flag) {
            RetVal = OK;
        }
    }

    return RetVal;
}

static INT32 AmbaSD_CheckR5Status(UINT32 R5CardStatus)
{
    INT32 ErrCode;
    if ((R5CardStatus & 0xcbU) == 0U) {
        ErrCode = 0;
    } else if (0x0U != (R5CardStatus & R5_OUT_OF_RANGE)) {
        ErrCode = -1;
    } else if (0x0U != (R5CardStatus & R5_FUNC_NUM_ERROR)) {
        ErrCode = -1;
    } else if (0x0U != (R5CardStatus & R5_ERROR))  {
        ErrCode = -1;
    } else if (0x0U != (R5CardStatus & R5_ILLEGAL_COMMAND)) {
        ErrCode = -1;
    } else if (0x0U != (R5CardStatus & R5_COM_CRC_ERROR)) {
        ErrCode = -1;
    } else {
        ErrCode = 0;
    }
    return ErrCode;
}

/*
 *  @RoutineName:: AmbaSD_SendCMD52
 *
 *  @Description:: Send CMD52 ((SDIO): I/O Reset, I/O about, or I/O Block Length, Bus_Width)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaSD_SendCMD52(UINT32 SdChanNo, const AMBA_SDIO_CMD52_ARGUMENT_s * pArgCmd52, UINT8 * pDataBuf)
{
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE;
    UINT32 RetVal;
    INT32 i;

    UINT32 SdioR5, SdioR5Flag;
    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        for (i = 0; i < SdCmdRetry; i++) {
            (void) AmbaRTSL_SdSendCMD52(SdChanNo, pArgCmd52);
            RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);

            RetVal = SdCheckIrqStatus(RetVal, Flag);
            RetVal = ClearBits(RetVal, AMBA_SD_IRQ_CARD);
            if (RetVal == Flag) {
                break;
            }
        }

        AmbaRTSL_SdGetResponse48Bits(SdChanNo, &SdioR5);
        SdioR5Flag = ((SdioR5 & 0xFF00U) >> 8U);
        if(AmbaSD_CheckR5Status(SdioR5Flag) != 0) {
            RetVal = AMBA_SD_ERR_UNEXPECT;
        } else {
            RetVal = OK;
        }

        if (0U != AmbaWrap_memcpy(pDataBuf, &SdioR5, sizeof(UINT8))) { /* Misrac */ };

        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SendCMD53
 *
 *  @Description:: Send CMD53 ((SDIO): I/O block operations)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaSD_SendCMD53(UINT32 SdChanNo, const AMBA_SDIO_CMD53_ARGUMENT_s * pArgCmd53, UINT16 BlockSize, UINT8 * pDataBuf)
{
    UINT32 Flag = (AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
    UINT32 RetVal;
    INT32 i;
    UINT32 Cmd53Response, SdioR5Flag;;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        for (i = 0; i < SdCmdRetry; i++) {
            (void) AmbaRTSL_SdSendCMD53(SdChanNo, pArgCmd53, BlockSize, pDataBuf);
            RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
            /* clear AMBA_SD_IRQ_CARD bit before Flag compare */
            RetVal &= (~AMBA_SD_IRQ_CARD);
            RetVal = SdCheckIrqStatus(RetVal, Flag);
            if (RetVal == Flag) {
                break;
            }

            /* Handle WiFi command 53 read Bad CRC */
            if ((RetVal == AMBA_SD_ERR_BADCRC) &&
                (pArgCmd53->ReadWriteFlag == 0U)) {
                /* wait else SDIO may hang */
                (void) AmbaKAL_TaskSleep(1);
                /* re-enable clk else next retry will timeout */
                AmbaRTSL_SdClockEnable(SdChanNo, 0U);
                AmbaRTSL_SdClockEnable(SdChanNo, 1U);
            }
        }

        if (RetVal == Flag) {
            AmbaRTSL_SdGetResponse48Bits(SdChanNo, &Cmd53Response);
            SdioR5Flag = ((Cmd53Response & 0xFF00U) >> 8U);
            if(AmbaSD_CheckR5Status(SdioR5Flag) < 0) {
                RetVal = AMBA_SD_ERR_UNEXPECT;
            } else {
                RetVal = OK;
            }
        }

        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SendCMD55
 *
 *  @Description:: Send CMD55 ((ac): Indicates to the card that the next CMD is an application specific CMD)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : SUCCESS(0)/NG(-1)
 */
UINT32 AmbaSD_SendCMD55(UINT32 SdChanNo)
{
    UINT32 RetVal;
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE;


    (void)AmbaRTSL_SdSendCMD55(SdChanNo);
    RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
    RetVal = SdCheckIrqStatus(RetVal, Flag);

    if (RetVal == AMBA_SD_IRQ_CMD_DONE) {
        RetVal = OK;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SendACMD6
 *
 *  @Description:: Send ACMD6 ((ac): Defines the data bus width ('00'=1bit or '10'=4 bits bus) to be used for data transfer)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : SUCCESS(0)/NG(-1)
 */
UINT32 AmbaSD_SendACMD6(UINT32 SdChanNo, UINT8 BusWidth)
{
    UINT32 RetVal;
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        RetVal = AmbaSD_SendCMD55(SdChanNo);

        if (RetVal == OK) {
            if (0 == AmbaRTSL_SdSendACMD6(SdChanNo, BusWidth)) {
                RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
                if ((RetVal & AMBA_SD_IRQ_ERROR) != 0U) {
                    RetVal = AMBA_SD_ERR_INVALID;
                }
            }
        }
        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SendACMD41
 *
 *  @Description:: Send ACMD41 ((bcr): Sends host capacity support info (HCS) and asks the card to send OCR)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : SUCCESS(0)/NG(-1)
 */
UINT32 AmbaSD_SendACMD41(UINT32 SdChanNo, const AMBA_SD_ACMD41_ARGUMENT_s * pAcmd41, UINT32 * pOcr)
{
    UINT32 RetVal;
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE;
    INT32 i;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        for (i = 0; i < SdCmdRetry; i++) {

            RetVal = AmbaSD_SendCMD55(SdChanNo);
            if (RetVal == OK) {
                (void) AmbaRTSL_SdSendACMD41(SdChanNo, pAcmd41, pOcr);
                RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
                RetVal = SdCheckIrqStatus(RetVal, Flag);

                AmbaRTSL_SdGetResponse48Bits(SdChanNo, pOcr);

                if ((RetVal == Flag) || (RetVal == AMBA_SD_ERR_NO_CARD)) {
                    break;
                }
            }
        }
        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SendACMD42
 *
 *  @Description:: Send ACMD42 ((ac):Connect[1]/Disconnect[0] the 50KOhm pull-up resistor on CD/DAT3 (pin 1) of the card)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : SUCCESS(0)/NG(-1)
 */
UINT32 AmbaSD_SendACMD42(UINT32 SdChanNo, UINT8 SetCardDetect)
{
    UINT32 RetVal;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        RetVal = AmbaSD_SendCMD55(SdChanNo);
        if (RetVal == OK) {
            (void) AmbaRTSL_SdSendACMD42(SdChanNo, SetCardDetect);
            RetVal = AmbaSD_WaitIrqStatus(SdChanNo, AMBA_SD_IRQ_CMD_DONE, 1U, 1U, SD_CMD_TIMEOUT);
            if ((RetVal & AMBA_SD_IRQ_ERROR) != 0U) {
                RetVal = AMBA_SD_ERR_INVALID;
            } else if (RetVal == AMBA_SD_IRQ_CMD_DONE) {
                RetVal = OK;
            } else {
                /* For MisraC checking */
            }
        }
        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_SendACMD51
 *
 *  @Description:: Send ACMD51 ((adtc): Reads the SD Configuration Register (SCR))
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaSD_SendACMD51(UINT32 SdChanNo, AMBA_SD_INFO_SCR_REG_s * pScr)
{
    UINT32 RetVal;
    UINT32 Flag = AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE;

    /*
     * Take the Mutex
     */
    RetVal = AmbaSD_Lock(SdChanNo);

    if (RetVal == OK) {

        (void) AmbaSD_SendCMD55(SdChanNo);

        (void) AmbaRTSL_SdSendACMD51(SdChanNo, pScr);
        RetVal = AmbaSD_WaitIrqStatus(SdChanNo, Flag, 1U, 1U, SD_CMD_TIMEOUT);
        RetVal = SdCheckIrqStatus(RetVal, Flag);
        if (RetVal == Flag) {
            UINT8 RawSCR[8];
            if (0U != AmbaWrap_memcpy(RawSCR, pScr, sizeof(RawSCR))) { /* Misrac */ };
            pScr->ScrStructure       = (UINT8)(RawSCR[0] >> 4U);
            pScr->SdSpec             = (UINT8)(RawSCR[0] & 0xfU);
            pScr->DataStatAfterErase = (UINT8)(RawSCR[1] >> 7U);
            pScr->SdSecurity         = (UINT8)((RawSCR[1] >> 4U) & 0x7U);
            pScr->SdBusWidths        = (UINT8)(RawSCR[1] & 0xfU);
            pScr->SdSpec3            = (UINT8)(RawSCR[2] >> 7U);
            pScr->ExSecurity         = (UINT8)((RawSCR[2] >> 3U) & 0xfU);
            pScr->SdSpec4            = (UINT8)((RawSCR[2] >> 2U) & 0x1U);
            pScr->CmdSupport         = (UINT8)(RawSCR[4] & 0xfU);
        }

        /*
         * Release the Mutex
         */
        (void) AmbaSD_Unlock(SdChanNo);
    }

    if (RetVal == Flag) {
        RetVal = OK;
    }

    return RetVal;
}

INT32 AmbaSD_GetResponse(UINT32 SdChanNo, UINT32 OpCode, UINT32 * pResp)
{
    return AmbaRTSL_SdGetResponse(SdChanNo, OpCode, pResp);
}

//-----------------------------------------------------------------------------------------------------
static inline void HandleReadMisalign(UINT32 Misalign, UINT8 * pReadBuf, const UINT8 * pTmpBuf, UINT32 Sectors, UINT32 BlkSize)
{
    UINT32 Addr;
    if (Misalign != 0U) {
        if (0U != AmbaWrap_memcpy(&Addr, &pTmpBuf, sizeof(UINT32))) { /* Misrac */ };
        (void)AmbaCache_DataInvalidate(Addr, Sectors * BlkSize);
        if (0U != AmbaWrap_memcpy(pReadBuf, pTmpBuf, Sectors * BlkSize)) { /* Misrac */ };
    }
}

UINT32 AmbaSD_ReadSector(UINT32 SdChanNo, const AMBA_NVM_SECTOR_CONFIG_s * pSecConfig)
{
    UINT32 i;
    UINT32 RetVal = AMBA_SD_ERR_INVALID_ARG;
    UINT8 *pBuf    = (UINT8 *)pSecConfig->pDataBuf;
    UINT32 Sector  = pSecConfig->StartSector;
    UINT32 Sectors = pSecConfig->NumSector;
    UINT32 Addr;
    UINT32 Misalign;
    ULONG BufAddr;
    UINT32 GoDone = 0U;
    UINT32 MaxSectors = AmbaRTSL_SdGetDescTransferSize() / 512U;
    const AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;

    UINT32 StartTime = 0, EndTime = 0, TimeDiff;

    (void)AmbaKAL_GetSysTickCount(&StartTime);

    AmbaMisra_TypeCast32(&BufAddr, &pBuf);
    Misalign   = BufAddr & (CACHE_LINE_SIZE - 1U);

    if (AmbaSD_IsCardPresent(SdChanNo) != 0U)    {

        if (pCardInfo->MemCardInfo.CardIsInit != 0U)   {

            if (Misalign != 0U) {
                (void) AmbaKAL_MutexTake(&SD_WorkBufMutex[SdChanNo], KAL_WAIT_FOREVER);
                MaxSectors = AmbaRTSL_SdGetWorkBufferSize() / 512U;
                pBuf = &AmbaRTSL_SdWorkBuf[SdChanNo * AMBA_SD_WORK_BUFFER_SIZE];
            }

            if (Sectors == 1U) {
                RetVal = AmbaSD_SendCMD17(SdChanNo, Sector, pBuf);
                HandleReadMisalign(Misalign, pSecConfig->pDataBuf, pBuf, 1U, 512U);
            } else {
                UINT8 *pBufPtr;
                i = 0U;
                while(Sectors >= MaxSectors) {
                    if (Misalign != 0U) {
                        pBufPtr = &AmbaRTSL_SdWorkBuf[SdChanNo * AMBA_SD_WORK_BUFFER_SIZE];
                    } else {
                        pBufPtr = &pSecConfig->pDataBuf[i * MaxSectors * 512U];
                    }

                    RetVal = AmbaSD_SendCMD18(SdChanNo, Sector, MaxSectors, pBufPtr);
                    if (RetVal != OK) {
                        GoDone = 1U;
                        break;
                    }

                    HandleReadMisalign(Misalign, &pSecConfig->pDataBuf[i * MaxSectors * 512U], pBufPtr, MaxSectors, 512U);
                    Sector += MaxSectors;
                    Sectors -= MaxSectors;
                    i ++;
                }

                if ((Sectors != 0U) && (GoDone == 0U)) {
                    if (Misalign != 0U) {
                        pBufPtr = &AmbaRTSL_SdWorkBuf[SdChanNo * AMBA_SD_WORK_BUFFER_SIZE];
                    } else {
                        pBufPtr = &pSecConfig->pDataBuf[i * MaxSectors * 512U];
                    }
                    if (Sectors == 1U) {
                        RetVal = AmbaSD_SendCMD17(SdChanNo, Sector, pBufPtr);
                    } else {
                        RetVal = AmbaSD_SendCMD18(SdChanNo, Sector, Sectors, pBufPtr);
                    }
                    HandleReadMisalign(Misalign, &pSecConfig->pDataBuf[i * MaxSectors * 512U], pBufPtr, Sectors, 512U);
                }
            }

            if (Misalign != 0U) {
                (void) AmbaKAL_MutexGive(&SD_WorkBufMutex[SdChanNo]);
            } else {
                if (0U != AmbaWrap_memcpy(&Addr, &pBuf, sizeof(Addr))) { /* Misrac */ };
                (void) AmbaCache_DataInvalidate(Addr, pSecConfig->NumSector * 512U);
            }
        }
    }

    if (iostat[SdChanNo].enable != 0U) {
        (void)AmbaKAL_GetSysTickCount(&EndTime);
        if (StartTime < EndTime) {
            TimeDiff = EndTime - StartTime;
        } else {
            TimeDiff = ((0xffffffffU - StartTime) + EndTime + 1U);
        }

        iostat[SdChanNo].rd_time  += TimeDiff;
        iostat[SdChanNo].rd_bytes += (UINT64)pSecConfig->NumSector * 512U;
        prf2_iostat_diagram(iostat[SdChanNo].rlvl, pSecConfig->NumSector);
    }

    return RetVal;
}

UINT32 AmbaSD_GetTotalSectors(UINT32 SdChanNo)
{
    UINT32 TotalSectors;
#ifdef CONFIG_ENABLE_EMMC_BOOT
    if (SdChanNo == AMBA_SD_CHANNEL0) {
        const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_EmmcUserPartTable ->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A]);
        TotalSectors = pPartEntry->BlkCount;
    } else {
        TotalSectors = AmbaSD_Ctrl[SdChanNo].CardInfo.MemCardInfo.TotalSectors;
    }
#else
    TotalSectors = AmbaSD_Ctrl[SdChanNo].CardInfo.MemCardInfo.TotalSectors;
#endif
    return TotalSectors;
}

UINT32 AmbaSD_EraseSector(UINT32 SdChanNo, UINT32 Sector, UINT32 Sectors)
{
    UINT32 RetVal = AMBA_SD_ERR_INVALID_ARG;
    UINT32 EndSector = Sector + Sectors;
    UINT32 StartSector = Sector;
    const AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;

    if (AmbaSD_IsCardPresent(SdChanNo) != 0U)    {

        if (pCardInfo->MemCardInfo.AddressMode == 0U) {
            StartSector = Sector * 512U;
        }

        if (pCardInfo->MemCardInfo.AddressMode == 0U) {
            EndSector = EndSector * 512U;
        }

        if (AmbaSD_Ctrl[SdChanNo].SdType == AMBA_SD_MEMORY) {
            RetVal = AmbaSD_SendCMD32(SdChanNo, StartSector);
            if (RetVal == OK) {
                RetVal = AmbaSD_SendCMD33(SdChanNo, EndSector);
            }
        }
        if (RetVal == OK) {
            RetVal = AmbaSD_SendCMD38(SdChanNo);
        }
    } else {
        RetVal = AMBA_SD_ERR_NO_CARD;
    }

    return RetVal;
}

static inline void HandleWriteMisalign(UINT32 Misalign, const UINT8 * pWriteBuf, UINT8 * pTmpBuf, UINT32 Sectors, UINT32 BlkSize)
{
    if (Misalign != 0U) {
        if (0U != AmbaWrap_memcpy(pTmpBuf, pWriteBuf, Sectors * BlkSize)) { /* Misrac */ };
    }
}

UINT32 AmbaSD_WriteSector(UINT32 SdChanNo, const AMBA_NVM_SECTOR_CONFIG_s * pSecConfig)
{
    UINT32 i;
    UINT32 RetVal = AMBA_SD_ERR_INVALID_ARG;
    UINT8 *pBuf    = (UINT8 *)pSecConfig->pDataBuf;
    UINT32 Sector  = pSecConfig->StartSector;
    UINT32 Sectors = pSecConfig->NumSector;

    UINT32 Misalign, GoDone = 0U;
    ULONG BufAddr;
    UINT32 MaxSectors = AmbaRTSL_SdGetDescTransferSize() / 512U;  /* Number of Descriptor is pre-located */

    const AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;

    UINT32 StartTime = 0, EndTime = 0, TimeDiff;

    (void) AmbaKAL_GetSysTickCount(&StartTime);

    AmbaMisra_TypeCast32(&BufAddr, &pBuf);
    Misalign   = BufAddr & (CACHE_LINE_SIZE - 1U);

    if (AmbaSD_IsCardPresent(SdChanNo) != 0U)    {

        if (pCardInfo->MemCardInfo.CardIsInit != 0U)   {

            if (Misalign != 0U) {
                (void) AmbaKAL_MutexTake(&SD_WorkBufMutex[SdChanNo], KAL_WAIT_FOREVER);
                MaxSectors = AmbaRTSL_SdGetWorkBufferSize() / 512U;
                pBuf = &AmbaRTSL_SdWorkBuf[SdChanNo * AMBA_SD_WORK_BUFFER_SIZE];
            }

            if (Sectors == 1U) {
                HandleWriteMisalign(Misalign, pSecConfig->pDataBuf, pBuf, 1U, 512U);
                RetVal = AmbaSD_SendCMD24(SdChanNo, Sector, pBuf);
            } else {
                UINT8 *pBufPtr;
                /* Number of Descriptor is pre-located */
                i = 0U;
                while(Sectors > MaxSectors) {
                    pBufPtr = &pSecConfig->pDataBuf[i * MaxSectors * 512U];
                    HandleWriteMisalign(Misalign, pBufPtr, pBuf, MaxSectors, 512U);
                    if (Misalign != 0U) {
                        RetVal = AmbaSD_SendCMD25(SdChanNo, Sector, MaxSectors, pBuf);
                    } else {
                        RetVal = AmbaSD_SendCMD25(SdChanNo, Sector, MaxSectors, pBufPtr);
                    }
                    if (RetVal != OK) {
                        GoDone = 1U;
                        break;
                    }
                    i++;
                    Sector += MaxSectors;
                    Sectors -= MaxSectors;
                }
                if ((Sectors != 0U) && (GoDone == 0U)) {
                    pBufPtr = &pSecConfig->pDataBuf[i * MaxSectors * 512U];
                    HandleWriteMisalign(Misalign, pBufPtr, pBuf, Sectors, 512U);
                    if (Sectors == 1U) {
                        if (Misalign != 0U) {
                            RetVal = AmbaSD_SendCMD24(SdChanNo, Sector, pBuf);
                        } else {
                            RetVal = AmbaSD_SendCMD24(SdChanNo, Sector, pBufPtr);
                        }
                    } else {
                        if (Misalign != 0U) {
                            RetVal = AmbaSD_SendCMD25(SdChanNo, Sector, Sectors, pBuf);
                        } else {
                            RetVal = AmbaSD_SendCMD25(SdChanNo, Sector, Sectors, pBufPtr);
                        }
                    }
                }
            }

            if (Misalign != 0U) {
                (void) AmbaKAL_MutexGive(&SD_WorkBufMutex[SdChanNo]);
            }
        }
    }

    if (iostat[SdChanNo].enable != 0U) {
        (void)AmbaKAL_GetSysTickCount(&EndTime);
        if (StartTime < EndTime) {
            TimeDiff = EndTime - StartTime;
        } else {
            TimeDiff = ((0xffffffffU - StartTime) + EndTime + 1U);
        }

        iostat[SdChanNo].wr_time  += TimeDiff;
        iostat[SdChanNo].wr_bytes += (UINT64)pSecConfig->NumSector * 512U;
        prf2_iostat_diagram(iostat[SdChanNo].wlvl, pSecConfig->NumSector);
    }

    return RetVal;
}

INT32 AmbaSD0_EraseSector(UINT32 Sector, UINT32 Sectors)
{
    UINT32 RetVal;
    INT32 Rval;
#ifdef CONFIG_ENABLE_EMMC_BOOT
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_EmmcUserPartTable ->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A]);
    if (AmbaRTSL_EmmcGetBootChannel() == AMBA_SD_CHANNEL0) {
        Sector += pPartEntry->StartBlkAddr;
    }
#else
#ifdef CONFIG_MUTI_BOOT_DEVICE
    extern AMBA_USER_PARTITION_TABLE_s *pAmbaRTSL_NorSpiUserPartTable;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_NorSpiUserPartTable->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A]);
    if (AmbaRTSL_EmmcGetBootChannel() == AMBA_SD_CHANNEL0) {
        Sector += pPartEntry->StartBlkAddr;
    }
#endif
#endif

    RetVal = AmbaSD_EraseSector(AMBA_SD_CHANNEL0, Sector, Sectors);

    if(RetVal == OK) {
        Rval = 0;
    } else {
        Rval = -1;
    }

    return Rval;
}

INT32 AmbaSD0_ReadSector(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors)
{
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;
    UINT32 RetVal;
    INT32 Rval;
#ifdef CONFIG_ENABLE_EMMC_BOOT
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_EmmcUserPartTable ->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A]);
    if (AmbaRTSL_EmmcGetBootChannel() == AMBA_SD_CHANNEL0) {
        Sector += pPartEntry->StartBlkAddr;
    }
#else
#ifdef CONFIG_MUTI_BOOT_DEVICE
    extern AMBA_USER_PARTITION_TABLE_s *pAmbaRTSL_NorSpiUserPartTable;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_NorSpiUserPartTable->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A]);
    if (AmbaRTSL_EmmcGetBootChannel() == AMBA_SD_CHANNEL0) {
        Sector += pPartEntry->StartBlkAddr;
    }
#endif
#endif
    SecConfig.NumSector   = Sectors;
    SecConfig.StartSector = Sector;
    SecConfig.pDataBuf    = pBuf;

    RetVal = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);

    if(RetVal == OK) {
        Rval = 0;
    } else {
        Rval = -1;
    }

    return Rval;
}

INT32 AmbaSD0_WriteSector(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors)
{
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;
    UINT32 RetVal;
    INT32 Rval;
#ifdef CONFIG_ENABLE_EMMC_BOOT
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_EmmcUserPartTable ->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A]);
    if (AmbaRTSL_EmmcGetBootChannel() == AMBA_SD_CHANNEL0) {
        Sector += pPartEntry->StartBlkAddr;
    }
#else
#ifdef CONFIG_MUTI_BOOT_DEVICE
    extern AMBA_USER_PARTITION_TABLE_s *pAmbaRTSL_NorSpiUserPartTable;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_NorSpiUserPartTable->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A]);
    if (AmbaRTSL_EmmcGetBootChannel() == AMBA_SD_CHANNEL0) {
        Sector += pPartEntry->StartBlkAddr;
    }
#endif
#endif
    SecConfig.NumSector   = Sectors;
    SecConfig.StartSector = Sector;
    SecConfig.pDataBuf    = pBuf;
    RetVal = AmbaSD_WriteSector(AMBA_SD_CHANNEL0, &SecConfig);

    if(RetVal == OK) {
        Rval = 0;
    } else {
        Rval = -1;
    }
    return Rval;
}

INT32 AmbaSD1_EraseSector(UINT32 Sector, UINT32 Sectors)
{
    UINT32 RetVal;
    INT32 Rval;

    RetVal = AmbaSD_EraseSector(AMBA_SD_CHANNEL1, Sector, Sectors);

    if(RetVal == OK) {
        Rval = 0;
    } else {
        Rval = -1;
    }
    return Rval;
}

INT32 AmbaSD1_ReadSector(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors)
{
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;
    UINT32 RetVal;
    INT32 Rval;

    SecConfig.NumSector   = Sectors;
    SecConfig.StartSector = Sector;
    SecConfig.pDataBuf    = pBuf;

    RetVal = AmbaSD_ReadSector(AMBA_SD_CHANNEL1, &SecConfig);

    if(RetVal == OK) {
        Rval = 0;
    } else {
        Rval = -1;
    }
    return Rval;
}

INT32 AmbaSD1_WriteSector(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors)
{
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;
    UINT32 RetVal;
    INT32 Rval;

    SecConfig.NumSector   = Sectors;
    SecConfig.StartSector = Sector;
    SecConfig.pDataBuf    = pBuf;
    RetVal = AmbaSD_WriteSector(AMBA_SD_CHANNEL1, &SecConfig);

    if(RetVal == OK) {
        Rval = 0;
    } else {
        Rval = -1;
    }
    return Rval;
}

UINT32 AmbaSDIO_Reset(UINT32 SdChanNo)
{
    UINT8 Data;
    UINT32 RetVal;

    if (SdChanNo >= AMBA_NUM_SD_CHANNEL) {
        RetVal = AMBA_SD_ERR_INVALID_ARG;  /* wrong parameters */
    } else {
        RetVal = AmbaSDIO_ReadByte(SdChanNo, 0, AMBA_SD_IO_ABORT_REG, &Data);

        if (RetVal != OK) {
            Data = 8U;
        } else {
            Data = (Data | 0x8U);
        }

        RetVal = AmbaSDIO_WriteByte(SdChanNo, 0, AMBA_SD_IO_ABORT_REG, &Data);
    }
    return RetVal;
}

/**
 *  AmbaSDIO_CardIrqEnable -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Enable
 *  @return error code
 */
UINT32 AmbaSDIO_CardIrqEnable(UINT32 SdChanNo, UINT32 Enable)
{
    UINT32 RetVal = OK;

    if (SdChanNo >= AMBA_NUM_SD_CHANNEL) {
        RetVal = AMBA_SD_ERR_INVALID_ARG;  /* wrong parameters */
    } else {
        AmbaRTSL_SdioCardIrqEnable(SdChanNo, Enable);
    }
    return RetVal;
}

/**
 *  AmbaSDIO_ReadByte -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] FunctionNo
 *  @param[in] RegAddr
 *  @param[out] pRxDataBuf
 *  @return error code
 */
UINT32 AmbaSDIO_ReadByte(UINT32 SdChanNo, UINT8 FunctionNo, UINT32 RegAddr, UINT8 * pRxDataBuf)
{
    UINT32 RetVal;
    AMBA_SDIO_CMD52_ARGUMENT_s ArgCmd52 = {0U};
    const AMBA_SD_CTRL_s       *pSdCtrl = &AmbaSD_Ctrl[SdChanNo];

    if ((SdChanNo >= AMBA_NUM_SD_CHANNEL) ||
        (pRxDataBuf == NULL) ||
        (FunctionNo > pSdCtrl->NumIoFunction)) {
        RetVal = AMBA_SD_ERR_INVALID_ARG;  /* wrong parameters */
    } else {
        ArgCmd52.WriteData          = 0U;
        ArgCmd52.ReadWriteFlag      = 0U;
        ArgCmd52.ReadAfterWriteFlag = 0U;

        ArgCmd52.FunctionNo = FunctionNo;
        ArgCmd52.RegAddr    = RegAddr;

        RetVal = AmbaSD_SendCMD52(SdChanNo, &ArgCmd52, pRxDataBuf);
    }

    return RetVal;
}

/**
 *  AmbaSDIO_WriteByte -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] FunctionNo
 *  @param[in] RegAddr
 *  @param[in] pTxDataBuf
 *  @return error code
 */
UINT32 AmbaSDIO_WriteByte(UINT32 SdChanNo, UINT8 FunctionNo, UINT32 RegAddr, UINT8 * pTxDataBuf)
{
    UINT32   RetVal;
    AMBA_SDIO_CMD52_ARGUMENT_s ArgCmd52 = {0};
    const AMBA_SD_CTRL_s       *pSdCtrl = &AmbaSD_Ctrl[SdChanNo];

    if ((SdChanNo >= AMBA_NUM_SD_CHANNEL) ||
        (pTxDataBuf == NULL) ||
        (FunctionNo > pSdCtrl->NumIoFunction)) {
        RetVal = AMBA_SD_ERR_INVALID_ARG;  /* wrong parameters */
    } else {
        ArgCmd52.WriteData          = (*pTxDataBuf);
        ArgCmd52.FunctionNo         = FunctionNo;
        ArgCmd52.RegAddr            = RegAddr;
        ArgCmd52.ReadAfterWriteFlag = 0U;
        ArgCmd52.ReadWriteFlag      = 1U;

        RetVal = AmbaSD_SendCMD52(SdChanNo, &ArgCmd52, pTxDataBuf);
    }

    return RetVal;
}

/**
 *  AmbaSDIO_ReadAfterWrite -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] FunctionNo
 *  @param[in] RegAddr
 *  @param[in] pTxDataBuf
 *  @return error code
 */
UINT32 AmbaSDIO_ReadAfterWrite(UINT32 SdChanNo, UINT8 FunctionNo, UINT32 RegAddr, const UINT8 * pTxDataBuf)
{
    UINT32   RetVal;
    UINT8 ReadData = 0;
    AMBA_SDIO_CMD52_ARGUMENT_s ArgCmd52 = {0};
    const AMBA_SD_CTRL_s       *pSdCtrl = &AmbaSD_Ctrl[SdChanNo];

    if ((SdChanNo >= AMBA_NUM_SD_CHANNEL) ||
        (pTxDataBuf == NULL) ||
        (FunctionNo > pSdCtrl->NumIoFunction)) {
        RetVal = AMBA_SD_ERR_INVALID_ARG;  /* wrong parameters */
    } else {
        ArgCmd52.WriteData          = (*pTxDataBuf);
        ArgCmd52.FunctionNo         = FunctionNo;
        ArgCmd52.RegAddr            = RegAddr;
        ArgCmd52.ReadAfterWriteFlag = 1U;
        ArgCmd52.ReadWriteFlag      = 1U;

        RetVal = AmbaSD_SendCMD52(SdChanNo, &ArgCmd52, &ReadData);
        if (ReadData != (*pTxDataBuf)) {
            RetVal = AMBA_SD_ERR_UNEXPECT;
        }
    }
    return RetVal;
}

/**
 *  FN_BLOCK_SIZE_0_REG -
 *  @param[in] x
 *  @return
 */
static UINT32 FN_BLOCK_SIZE_0_REG(UINT32 x)
{
    return ((0x100U * (x)) + 0x10U);
}

/**
 *  FN_BLOCK_SIZE_1_REG -
 *  @param[in] x
 *  @return
 */
static UINT32 FN_BLOCK_SIZE_1_REG(UINT32 x)
{
    return ((0x100U * (x)) + 0x11U);
}

/**
 *  AmbaSDIO_SetBlkSize -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] FunctionNo
 *  @param[in] BlkSize
 *  @return error code
 */
UINT32 AmbaSDIO_SetBlkSize(UINT32 SdChanNo, UINT8 FunctionNo, UINT32 BlkSize)
{
    UINT8 Data = (UINT8) (BlkSize & 0xFFU);
    UINT32 RetVal;
    const AMBA_SD_CTRL_s *pSdCtrl = &AmbaSD_Ctrl[SdChanNo];

    if ((SdChanNo >= AMBA_NUM_SD_CHANNEL) ||
        (FunctionNo > pSdCtrl->NumIoFunction) ||
        (BlkSize > 0xFFU)) {
        RetVal = AMBA_SD_ERR_INVALID_ARG;  /* wrong parameters */
    } else {
        RetVal = AmbaSDIO_ReadAfterWrite(SdChanNo, 0, FN_BLOCK_SIZE_0_REG((UINT32) FunctionNo), (const UINT8 *) &Data);

        if (RetVal == OK) {
            Data = (UINT8) ((BlkSize >> 8U) & 0xFFU);
            RetVal = AmbaSDIO_ReadAfterWrite(SdChanNo, 0, FN_BLOCK_SIZE_1_REG((UINT32) FunctionNo), &Data);
        }
    }
    return RetVal;
}

static UINT32 AmbaSDIO_CheckBlkSize(UINT32 SdChanNo, const AMBA_SDIO_RW_CONFIG_s * pConfig, AMBA_SD_IO_CARD_INFO_s *pIoCardInfo)
{
    UINT32 RetVal = AMBA_SD_ERR_NONE;

    if (pIoCardInfo->CurBlkSize[pConfig->FunctionNo] != pConfig->BlkSize) {
        RetVal = AmbaSDIO_SetBlkSize(SdChanNo, pConfig->FunctionNo, pConfig->BlkSize);
        if (RetVal == AMBA_SD_ERR_NONE) {
            pIoCardInfo->CurBlkSize[pConfig->FunctionNo] = pConfig->BlkSize;
        }
    }

    return RetVal;
}

/**
 *  AmbaSDIO_Read -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pConfig
 *  @return error code
 */
UINT32 AmbaSDIO_Read(UINT32 SdChanNo, const AMBA_SDIO_RW_CONFIG_s * pConfig)
{
    UINT32 i = 0U;
    UINT32 RetVal = AMBA_SD_ERR_NONE;
    AMBA_SDIO_CMD53_ARGUMENT_s ArgCmd53 = {0};
    const AMBA_SD_CTRL_s         *pSdCtrl     = &AmbaSD_Ctrl[SdChanNo];
    AMBA_SD_IO_CARD_INFO_s *pIoCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo.IoCardInfo;
    UINT8 *pBufPtr;
    UINT8 *pBuf     = pConfig->pDataBuf;
    UINT32 Sector   = pConfig->RegAddr;
    UINT32 BlkCount = pConfig->DataSize;
    UINT32 Misalign;
    ULONG  BufAddr;
    UINT32 MaxSectors = AmbaRTSL_SdGetDescTransferSize() / pConfig->BlkSize;

    AmbaMisra_TypeCast(&BufAddr, &pBuf);
    Misalign   = BufAddr & (CACHE_LINE_SIZE - 1U);

    if (pConfig->FunctionNo <= pSdCtrl->NumIoFunction) {

        RetVal = AmbaSDIO_CheckBlkSize(SdChanNo, pConfig, pIoCardInfo);

        if (RetVal == AMBA_SD_ERR_NONE) {
            if (Misalign != 0U) {
                (void) AmbaKAL_MutexTake(&SD_WorkBufMutex[SdChanNo], KAL_WAIT_FOREVER);
                MaxSectors = AmbaRTSL_SdGetWorkBufferSize() / pConfig->BlkSize;
                pBuf = &AmbaRTSL_SdWorkBuf[SdChanNo * AMBA_SD_WORK_BUFFER_SIZE];
            }

            if ((pIoCardInfo->CardCapability.SupportMultiBlk != 0U) && (pConfig->ByteMode == 0U)) {
                ArgCmd53.BlockMode = 1U;
            } else {
                ArgCmd53.BlockMode = 0;
            }

            ArgCmd53.Count         = (UINT16) pConfig->DataSize;
            ArgCmd53.FunctionNo    = pConfig->FunctionNo;
            ArgCmd53.RegAddr       = pConfig->RegAddr;
            ArgCmd53.OpCode        = pConfig->IncAddrFlag;
            ArgCmd53.ReadWriteFlag = 0;

            if (ArgCmd53.BlockMode == 0U) {
                /* Byte mode */
                RetVal = AmbaSD_SendCMD53(SdChanNo, &ArgCmd53, (UINT16) pConfig->DataSize, pBuf);
                if (RetVal == AMBA_SD_ERR_NONE) {
                    HandleReadMisalign(Misalign, pConfig->pDataBuf, pBuf, 1U, pConfig->DataSize);
                }
            }
        }

        if (RetVal == AMBA_SD_ERR_NONE) {
            while(BlkCount >= MaxSectors) {
                if (Misalign != 0U) {
                    pBufPtr = &AmbaRTSL_SdWorkBuf[SdChanNo * AMBA_SD_WORK_BUFFER_SIZE];
                } else {
                    pBufPtr = &pConfig->pDataBuf[i * MaxSectors *  pConfig->BlkSize];
                }

                ArgCmd53.Count   = (UINT16) BlkCount;
                ArgCmd53.RegAddr = Sector;
                RetVal = AmbaSD_SendCMD53(SdChanNo, &ArgCmd53, (UINT16) pConfig->BlkSize, pBufPtr);
                if (RetVal != AMBA_SD_ERR_NONE) {
                    break;
                }

                HandleReadMisalign(Misalign, &pConfig->pDataBuf[i * MaxSectors * pConfig->BlkSize], pBufPtr, MaxSectors, pConfig->BlkSize);
                Sector += MaxSectors;
                BlkCount -= MaxSectors;
                i++;
            }
        }

        if (RetVal == AMBA_SD_ERR_NONE) {
            if (BlkCount != 0U) {
                if (Misalign != 0U) {
                    pBufPtr = &AmbaRTSL_SdWorkBuf[SdChanNo * AMBA_SD_WORK_BUFFER_SIZE];
                } else {
                    pBufPtr = &pConfig->pDataBuf[i * MaxSectors *  pConfig->BlkSize];
                }

                ArgCmd53.Count   = (UINT16) BlkCount;
                ArgCmd53.RegAddr = Sector;
                RetVal = AmbaSD_SendCMD53(SdChanNo, &ArgCmd53, (UINT16) pConfig->BlkSize, pBufPtr);
                HandleReadMisalign(Misalign, &pConfig->pDataBuf[i * MaxSectors * pConfig->BlkSize], pBufPtr, BlkCount, pConfig->BlkSize);
            }
        }

        if (Misalign != 0U) {
            (void) AmbaKAL_MutexGive(&SD_WorkBufMutex[SdChanNo]);
        } else {
            (void) AmbaCache_DataInvalidate(BufAddr, pConfig->DataSize * pConfig->BlkSize);
        }
    } else {
        RetVal = AMBA_SD_ERR_INVALID_ARG;
    }

    return RetVal;
}

/**
 *  AAmbaSDIO_Write -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pConfig
 *  @return error code
 */
UINT32 AmbaSDIO_Write(UINT32 SdChanNo, const AMBA_SDIO_RW_CONFIG_s * pConfig)
{
    UINT32 i = 0U;
    UINT32 RetVal = AMBA_SD_ERR_NONE;
    AMBA_SDIO_CMD53_ARGUMENT_s ArgCmd53 = {0};
    const AMBA_SD_CTRL_s         *pSdCtrl     = &AmbaSD_Ctrl[SdChanNo];
    AMBA_SD_IO_CARD_INFO_s *pIoCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo.IoCardInfo;
    UINT8 *pBufPtr;
    UINT8 *pBuf    = pConfig->pDataBuf;
    UINT32 Sectors = pConfig->DataSize;
    UINT32 Misalign;
    ULONG BufAddr;
    UINT32 MaxSectors = AmbaRTSL_SdGetDescTransferSize() / pConfig->BlkSize;

    AmbaMisra_TypeCast(&BufAddr, &pBuf);
    Misalign   = BufAddr & (CACHE_LINE_SIZE - 1U);

    if (pConfig->FunctionNo <= pSdCtrl->NumIoFunction) {

        RetVal = AmbaSDIO_CheckBlkSize(SdChanNo, pConfig, pIoCardInfo);

        if (RetVal == AMBA_SD_ERR_NONE) {
            if (Misalign != 0U) {
                (void) AmbaKAL_MutexTake(&SD_WorkBufMutex[SdChanNo], KAL_WAIT_FOREVER);
                MaxSectors = AmbaRTSL_SdGetWorkBufferSize() / pConfig->BlkSize;
                pBuf = &AmbaRTSL_SdWorkBuf[SdChanNo * AMBA_SD_WORK_BUFFER_SIZE];
            }

            if ((pIoCardInfo->CardCapability.SupportMultiBlk != 0U) && (pConfig->ByteMode == 0U)) {
                ArgCmd53.BlockMode = 1U;
            } else {
                ArgCmd53.BlockMode = 0;
            }

            ArgCmd53.Count         = (UINT16) pConfig->DataSize;
            ArgCmd53.FunctionNo    = pConfig->FunctionNo;
            ArgCmd53.RegAddr       = pConfig->RegAddr;
            ArgCmd53.OpCode        = pConfig->IncAddrFlag;
            ArgCmd53.ReadWriteFlag = 1U;

            if (ArgCmd53.BlockMode == 0U) {
                /* Byte mode */
                HandleWriteMisalign(Misalign, pConfig->pDataBuf, pBuf, 1, pConfig->DataSize);
                RetVal = AmbaSD_SendCMD53(SdChanNo, &ArgCmd53, (UINT16) pConfig->DataSize, pBuf);
            }
        }

        if (RetVal == AMBA_SD_ERR_NONE) {
            while(Sectors >= MaxSectors) {
                pBufPtr = &pConfig->pDataBuf[i * MaxSectors * pConfig->BlkSize];
                HandleWriteMisalign(Misalign, pBufPtr, pBuf, MaxSectors, pConfig->BlkSize);

                ArgCmd53.Count = (UINT16) MaxSectors;
                if (Misalign != 0U) {
                    RetVal = AmbaSD_SendCMD53(SdChanNo, &ArgCmd53, (UINT16) pConfig->BlkSize, pBuf);
                } else {
                    RetVal = AmbaSD_SendCMD53(SdChanNo, &ArgCmd53, (UINT16) pConfig->BlkSize, pBufPtr);
                }

                if (RetVal != AMBA_SD_ERR_NONE) {
                    break;
                }
                i++;
                Sectors -= MaxSectors;
            }
        }

        if (RetVal == AMBA_SD_ERR_NONE) {
            if (Sectors != 0U) {
                pBufPtr = &pConfig->pDataBuf[i * MaxSectors * pConfig->BlkSize];
                HandleWriteMisalign(Misalign, pBufPtr, pBuf, Sectors, pConfig->BlkSize);
                ArgCmd53.Count = (UINT16) Sectors;
                if (Misalign != 0U) {
                    RetVal = AmbaSD_SendCMD53(SdChanNo, &ArgCmd53, (UINT16) pConfig->BlkSize, pBuf);
                } else {
                    RetVal = AmbaSD_SendCMD53(SdChanNo, &ArgCmd53, (UINT16) pConfig->BlkSize, pBufPtr);
                }
            }
        }

        if (Misalign != 0U) {
            (void) AmbaKAL_MutexGive(&SD_WorkBufMutex[SdChanNo]);
        }

    } else {
        RetVal = AMBA_SD_ERR_INVALID_ARG;
    }

    return RetVal;
}

