/**
 *  @file AmbaSD_Card.c
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
 *  @details SD Card Control APIs
 *
 */

#include "AmbaWrap.h"

#include "AmbaSD.h"
#include "AmbaSD_STD.h"
#include "AmbaSD_Ctrl.h"

//#include "AmbaRTSL_SD.h"
//#include "AmbaPrint.h"

/**
 *  AmbaSD_CardInit - SD Card Initialization
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_CardInit(UINT32 SdChanNo)
{
    UINT8 IsInit;
    AMBA_SD_CTRL_s      *pSdCtrl   = &AmbaSD_Ctrl[SdChanNo];
    AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    UINT32 Status = 0;
    AMBA_SDIO_CMD5_ARGUMENT_s SDIO_Cmd5Arg = {0};

    if (SdChanNo >= AMBA_NUM_SD_CHANNEL) {
        Status = 1;  /* wrong parameter */
    } else {
        (void)AmbaWrap_memset(&pSdCtrl->CardInfo, 0x0, sizeof(AMBA_SD_CARD_INFO_s));
        (void)AmbaWrap_memset(&pSdCtrl->CardRegs, 0x0, sizeof(AMBA_SD_INFO_REG_s));

        pSdCtrl->NumIoFunction = 0;
        pSdCtrl->SdType = AMBA_SD_MEMORY;

        /* power cycle */
        if (pSdCtrl->SdConfig.PowerCtrl != NULL) {
            pSdCtrl->SdConfig.PowerCtrl(SdChanNo, AMBA_SD_VDD_POWER_OFF);
            pSdCtrl->SdConfig.PowerCtrl(SdChanNo, AMBA_SD_VDD_3D30V);
        }

        (void)pSdCtrl->SetClock(pSdCtrl->SdConfig.SdSetting.InitFrequency);
        pCardInfo->MemCardInfo.CurrentClock = pSdCtrl->GetClock();
        AmbaSD_DelayCtrlReset(SdChanNo);

        (void)AmbaSD_ClearCtrlFlag(SdChanNo);
        Status = AmbaSDIO_Reset(SdChanNo);

        if (Status == OK) {
            Status = AmbaSD_SendCMD0(SdChanNo);
        }

        if (Status == OK) {

            (void) AmbaSD_SendCMD5(SdChanNo, &SDIO_Cmd5Arg, NULL);

            if (pSdCtrl->NumIoFunction != 0U) {
                AmbaSD_IoCardInit(SdChanNo);   /* SDIO Memory Card initialization */
                IsInit = pCardInfo->IoCardInfo.CardIsInit;
            } else {
                AmbaSD_MemCardInit(SdChanNo);   /* SD Memory Card initialization */
                IsInit = pCardInfo->MemCardInfo.CardIsInit;
            }

            if (IsInit == 1U) {
                Status = OK;
            } else {
                Status = 3;
            }
        }
    }

    return Status;
}

/**
 *  SD_CardMonitorTaskEntry - SD Card Monitoring Task entry
 *  @param[in] EntryArg Task entry Argument
 */
void *SD_CardMonitorTaskEntry(void *EntryArg)
{
    UINT32 Loop = 1U;
    UINT32 i, Rval, ActualFlags, CardPresent;
    AMBA_SD_CTRL_s *pSdCtrl;
    extern UINT32 AmbaFS_PrFile2SdMount(UINT32 SdChanNo);
    extern UINT32 AmbaFS_PrFile2SdUnmount(UINT32 SdChanNo);
    static UINT32 ExitTask = 0;

    (void)EntryArg;

    while (1U == Loop) { /* loop forever */
        ActualFlags = AmbaSD_WaitEventFlag(AMBA_SD_EVENT_CARD_DETECT_MASK,
                                           0, 1, 1);

        /* To eliminate temporary debounce. */

        for (i = AMBA_SD_CHANNEL0; i < AMBA_NUM_SD_CHANNEL; i++) {
            pSdCtrl = &(AmbaSD_Ctrl[i]);

            if ((ActualFlags & (UINT32)((UINT32)1 << i)) == 0U) {
                continue;
            }

            CardPresent = AmbaSD_IsCardPresent(i);
            if (CardPresent != 0U) {
                if (pSdCtrl->SdType == AMBA_SD_NOT_PRESENT) {
#if 0
                    UINT32 StartMaxFrequency[AMBA_NUM_SD_CHANNEL] = {0};
                    StartMaxFrequency[i] = pSdCtrl->SdConfig.SdSetting.MaxFrequency;
#endif
                    for (UINT32 InitCnt = 0; InitCnt < 3U; InitCnt++) {
                        Rval = AmbaSD_CardInit(i);
                        if (Rval == OK) {
                            break;
                        }
                        //AmbaPrint_ModulePrintUInt5(AMBA_SD_PRINT_MODULE_ID, "SD init Cnt : %u", InitCnt, 0U, 0U, 0U, 0U);
                    }
                    if (Rval == OK) {
                        //AmbaPrint_ModulePrintUInt5(AMBA_SD_PRINT_MODULE_ID, "SD(%u) init OK", i, 0U, 0U, 0U, 0U);
                        (void)AmbaFS_PrFile2SdMount(i);
                    } else {
                        //AmbaPrint_ModulePrintUInt5(AMBA_SD_PRINT_MODULE_ID, "SD(%u) init Fail", i, 0U, 0U, 0U, 0U);
                    }

                    /* TODO : set the flag of SD-# Card Status Changed */
                }
            } else if (pSdCtrl->SdType != AMBA_SD_NOT_PRESENT) {
                /* card removed */
                pSdCtrl->SdType = AMBA_SD_NOT_PRESENT;
                (void)AmbaFS_PrFile2SdUnmount(i);
                if (pSdCtrl->SdConfig.PowerCtrl != NULL) {
                    pSdCtrl->SdConfig.PowerCtrl(i, AMBA_SD_VDD_POWER_OFF);
                }

                (void)AmbaSD_SetSdSetting(i, &pSdCtrl->SdConfig.SdSetting);
                //AmbaPrint_ModulePrintUInt5(AMBA_SD_PRINT_MODULE_ID, "SD(%u) Removed!", i, 0U, 0U, 0U, 0U);

                /* TODO: set the flag of SD-# Card Status Changed */
            } else {
                /* Do noting to pass misra-c */
            }
        }
        if (ExitTask == 1U) {
            break; /* for misra-c  */
        }
    }

    return NULL;
}

/**
 *  AmbaSD_GetCardStatus - Get Card Status
 *  @param[in] SdChanNo SD Channel Number
 *  @param[out] pCardStatus pointer to Card Status buffer
 *  @return error code
 */
UINT32 AmbaSD_GetCardStatus(UINT32 SdChanNo, AMBA_SD_CARD_STATUS_s *pCardStatus)
{
    const AMBA_SD_CTRL_s      *pSdCtrl   = &AmbaSD_Ctrl[SdChanNo];
    const AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    const AMBA_SD_INFO_REG_s  *pCardRegs = &AmbaSD_Ctrl[SdChanNo].CardRegs;
    UINT32 status = OK; // OK is defined as unsigned int

    (void)AmbaWrap_memset(pCardStatus, 0, sizeof(AMBA_SD_CARD_STATUS_s));

    if (AmbaSD_IsCardPresent(SdChanNo) == 0U) {
        status = 6;
    } else {
        if (pSdCtrl->SdType == AMBA_SD_IO_ONLY) {
            pCardStatus->CardIsInit = pCardInfo->IoCardInfo.CardIsInit;
            pCardStatus->CardSize   = 0;
        } else {
            pCardStatus->CardIsInit = pCardInfo->MemCardInfo.CardIsInit;
            pCardStatus->CardSize   = (UINT64) pCardInfo->MemCardInfo.TotalSectors * (UINT64)512;
        }

        pCardStatus->CardSpeed      = pSdCtrl->GetClock();
        pCardStatus->NumIoFunction  = pSdCtrl->NumIoFunction;
        pCardStatus->SdType         = pSdCtrl->SdType;
        pCardStatus->DataAfterErase = ((pCardRegs->SdConfig.DataStatAfterErase == 1U) ? 1U : 0U);
        pCardStatus->WriteProtect   = 0;

        status = 0;
    }

    return status;
}
