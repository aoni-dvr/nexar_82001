/*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"

#include "AmbaWrap.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_GIC.h"

#include "AmbaRTSL_SD.h"
#include "AmbaCSL_SD.h"
#include "AmbaRTSL_GPIO.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaCSL_RCT.h"

#include "AmbaSD_Ctrl.h"

#include "AmbaCSL_PLL.h"

#include "AmbaMMU.h"

#define NG (-1)

static void SD_IntHandler(UINT32 SdChanNo);
static void SD_Chan0ISR(UINT32 IntID, UINT32 UserArg);
static void SD_Chan1ISR(UINT32 IntID, UINT32 UserArg);
static void SD_Chan2ISR(UINT32 IntID, UINT32 UserArg);


AMBA_RTSL_SD_CTRL_s AmbaRTSL_SdCtrl[SD_HOST_NUM] = {
    [AMBA_SD_CHANNEL0] = {
        .CmdConfig = {0},                   /* CMD Configurations */
        .Status   = 0,                      /* Status */
        .IntID    = AMBA_INT_SPI_ID104_SD,      /* Interrupt ID */
        .pISR     = SD_Chan0ISR,            /* pointer to the ISR */
        .Rca      = 0,
        .BusWidth = 1,

        .pSysPartConfig  = NULL,
        .pUserPartConfig = NULL,

        .BstPageCount        = 0,
        .SysPtblPageCount    = 0,
        .UserPtblPageCount   = 0,
        .VendorDataPageCount = 0,
    },

    [AMBA_SD_CHANNEL1] = {
        .CmdConfig = {0},                   /* CMD Configurations */
        .Status    = 0,                     /* Status */
        .IntID     = AMBA_INT_SPI_ID103_SDIO0,     /* Interrupt ID */
        .pISR      = SD_Chan1ISR,           /* pointer to the ISR */
        .Rca       = 0,
        .BusWidth  = 1,

        .pSysPartConfig  = NULL,
        .pUserPartConfig = NULL,

        .BstPageCount        = 0,
        .SysPtblPageCount    = 0,
        .UserPtblPageCount   = 0,
        .VendorDataPageCount = 0,
    },
    [AMBA_SD_CHANNEL2] = {
        .CmdConfig = {0},                   /* CMD Configurations */
        .Status    = 0,                     /* Status */
        .IntID     = AMBA_INT_SPI_ID96_SDIO1,     /* Interrupt ID */
        .pISR      = SD_Chan2ISR,           /* pointer to the ISR */
        .Rca       = 0,
        .BusWidth  = 1,

        .pSysPartConfig  = NULL,
        .pUserPartConfig = NULL,

        .BstPageCount        = 0,
        .SysPtblPageCount    = 0,
        .UserPtblPageCount   = 0,
        .VendorDataPageCount = 0,
    },
};

AMBA_SD_CTRL_s AmbaSD_Ctrl[AMBA_NUM_SD_CHANNEL] = {    /* SD Management Structure */
    [AMBA_SD_CHANNEL0] = {
        .SdType = AMBA_SD_NOT_PRESENT,  /* SD Card Type */
        .NumIoFunction = 0,             /* Number of I/O functions */

        .GetClock = AmbaRTSL_PllGetSd0Clk,
        .SetClock = AmbaRTSL_PllSetSd0Clk,
    },

    [AMBA_SD_CHANNEL1] = {
        .SdType = AMBA_SD_NOT_PRESENT,  /* SD Card Type */
        .NumIoFunction = 0,             /* Number of I/O functions */

        .GetClock = AmbaRTSL_PllGetSd1Clk,  /* pointer to the API to get SD clock */
        .SetClock = AmbaRTSL_PllSetSd1Clk,  /* pointer to the API to set SD clock */
    },
    [AMBA_SD_CHANNEL2] = {
        .SdType = AMBA_SD_NOT_PRESENT,  /* SD Card Type */
        .NumIoFunction = 0,             /* Number of I/O functions */

        .GetClock = AmbaRTSL_PllGetSd2Clk,  /* pointer to the API to get SD clock */
        .SetClock = AmbaRTSL_PllSetSd2Clk,  /* pointer to the API to set SD clock */
    },
};

#define AMBA_SD_DESC_COUNT              128U
#define AMBA_SD_DESC_MAX_TRAN_SIZE      (65536U)     /* 64U * 1024U = 64KB */
#define AMBA_SD_DESC_TOTAL_TRAN_SIZE    (AMBA_SD_DESC_COUNT * AMBA_SD_DESC_MAX_TRAN_SIZE)

static AMBA_SD_ADMA_DESCRIPTOR_s pAmbaSD_AdmaDesc[AMBA_NUM_SD_CHANNEL][AMBA_SD_DESC_COUNT] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

/* Call back functions when completed transactions */
void (*AmbaRTSL_SdIsrCallBack)(UINT32 SdChanNo, UINT32 SdIrqStatus) = NULL;

const UINT8 AmbaRTSL_SdFixedTuningBlkPattern[AMBA_SD_TUNING_BLOCK_BYTE_SIZE] = {
    0xff, 0x0f, 0xff, 0x00, 0xff, 0xcc, 0xc3, 0xcc, 0xc3, 0x3c, 0xcc, 0xff, 0xfe, 0xff, 0xfe, 0xef,
    0xff, 0xdf, 0xff, 0xdd, 0xff, 0xfb, 0xff, 0xfb, 0xbf, 0xff, 0x7f, 0xff, 0x77, 0xf7, 0xbd, 0xef,
    0Xff, 0xf0, 0xff, 0xf0, 0x0f, 0xfc, 0xcc, 0x3c, 0xcc, 0x33, 0xcc, 0xcf, 0xff, 0xef, 0xff, 0xee,
    0xff, 0xfd, 0xff, 0xfd, 0xdf, 0xff, 0xbf, 0xff, 0xbb, 0xff, 0xf7, 0xff, 0xf7, 0x7f, 0x7b, 0xde,
};

const UINT8 AmbaRTSL_SdEmm8bitTuningBlkPattern[AMBA_SD_TUNING_BLOCK_BYTE_SIZE * 2U] = {
    0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xcc, 0xcc, 0xcc, 0x33, 0xcc, 0xcc,
    0xcc, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xff, 0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xee, 0xee, 0xff,
    0Xff, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff, 0xbb,
    0xbb, 0xff, 0xff, 0xff, 0x77, 0xff, 0xff, 0xff, 0x77, 0x77, 0xff, 0x77, 0xbb, 0xdd, 0xee, 0xff,
    0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xcc, 0xcc, 0xcc, 0x33, 0xcc,
    0xcc, 0xcc, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xff, 0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xee, 0xee,
    0xff, 0xff, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff,
    0xbb, 0xbb, 0xff, 0xff, 0xff, 0x77, 0xff, 0xff, 0xff, 0x77, 0x77, 0xff, 0x77, 0xbb, 0xdd, 0xee,
};

UINT8 AmbaRTSL_SdCmd19TuningBlkPattern[AMBA_SD_TUNING_BLOCK_BYTE_SIZE]  GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
UINT8 AmbaRTSL_SdEmmcCmd21TuningBlkPattern[2U * AMBA_SD_TUNING_BLOCK_BYTE_SIZE]  GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
//void AmbaUserSD_DelayCtrl(UINT32 SdChanNo, AMBA_SD_INFO_CID_INFO_s *pCardID) GNU_WEAK_SYMBOL;

UINT8 AmbaRTSL_SdWorkBuf[AMBA_NUM_SD_CHANNEL * AMBA_SD_WORK_BUFFER_SIZE]
GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

#define SAVE_RESP_NUM       5
typedef struct {
    UINT32 OpCode;
    UINT32 Resp[4];
} CmdResp_s;

static CmdResp_s SdCmdResponce[AMBA_NUM_SD_CHANNEL][SAVE_RESP_NUM];
//static INT32 UserRTSLSDDelayCtrlEnable = 1;

/*
 *  @RoutineName:: AmbaRTSL_SdInit
 *
 *  @Description:: SD RTSL initializations
 *
 *  @Input      :: void
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_SdInit(void)
{
    AmbaCSL_PLLSetSd0ScalerClkSrc(0);
    AmbaCSL_PLLSetSd1ScalerClkSrc(0);
    AmbaCSL_PLLSetSd2ScalerClkSrc(0);

    (void) AmbaRTSL_PllSetSdPllOutClk(1600000000U);

    /* clear the pointer of Call back function when completed transactions */
    AmbaRTSL_SdIsrCallBack = NULL;

    return OK;
}

/*
 *  @RoutineName:: AmbaSD_UhsSupportChk
 *
 *  @Description:: Check status to support UHS mode
 *
 *  @Input      ::
 *      SdChanNo:    SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       UINT32 : Current SD event flags
 */
INT32 AmbaEMMC_HS200SupportChk(UINT32 SdChanNo)
{
#if defined(CONFIG_ENABLE_EMMC_HS200)
    AMBA_SD_INFO_REG_s *pCardRegs = &AmbaSD_Ctrl[SdChanNo].CardRegs;

    return (pCardRegs->ExtCsd.Properties.DeviceType & HIGH_SPEED_MMC_200MHZ_1_8V) ? (1) : (0);
#else
    AmbaMisra_TouchUnused(&SdChanNo);

    return 0;
#endif
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
static INT32 AmbaRTSL_EmmcSetupHS200Mode(UINT32 SdChanNo)
{
    INT32 Rval;
    AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    const AMBA_SD_INFO_REG_s *pCardRegs = &AmbaSD_Ctrl[SdChanNo].CardRegs;
    AMBA_SD_EMMC_CMD6_ARGUMENT_s ArgCmd6 = {0};

    /* HS200 only Support 4bit or 8 Bit */
    if ((pCardInfo->MemCardInfo.BusWidth == 4U) || (pCardInfo->MemCardInfo.BusWidth == 8U)) {

        /* Setup the argument of CMD6 for HS200 Timing */
        AmbaRTSL_GeteMMCSpeedMode(&ArgCmd6, &pCardRegs->ExtCsd);
        AmbaRTSL_GeteMMCDriveStrength(&ArgCmd6, &pCardRegs->ExtCsd);
        ArgCmd6.Access = ACCESS_WRITE_BYTE;
        ArgCmd6.Index = 0xb9;
        ArgCmd6.CmdSet = 0x0;
        Rval = AmbaRTSL_SdSendEmmcCMD6(SdChanNo, &ArgCmd6);
        if (Rval >= 0) {
            Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);

            if (Rval >= 0) {
                pCardInfo->MemCardInfo.DesiredClock =
                    ((ArgCmd6.Value & 0x0fU) == EMMC_HS200_MODE) ? AMBA_EMMC_HS200_MAX_FREQUENCY :
                    ((ArgCmd6.Value & 0x0fU) == EMMC_HS400_MODE) ? AMBA_EMMC_HS400_MAX_FREQUENCY :
                    AMBA_EMMC_SDR52_MAX_FREQUENCY;
            }
        }
    } else {
        Rval =  NG;
    }

    return Rval;
}

#if 0
/*
 *  @RoutineName:: AmbaRTSL_SdDelayCtrlEnable
 *
 *  @Description:: Enable/Disable  AmbaSD_SDDelayCtrl
 *
 *  @Input      ::
 *      Enable: 1-Enable 0-Disable
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *
  */
void AmbaRTSL_SdDelayCtrlEnable(INT32 Enable)
{
    UserRTSLSDDelayCtrlEnable = Enable;
}
#endif

static UINT32 AmbaRTSL_SdMmcCheckTuningBlock(UINT32 SdChanNo, const AMBA_SD_CARD_INFO_s *pCardInfo)
{
    UINT32 RetVal = 0U;
    INT32 Rval, CmpResult;

    if (OK != AmbaWrap_memset(AmbaRTSL_SdEmmcCmd21TuningBlkPattern, 0x0, 2U * AMBA_SD_TUNING_BLOCK_BYTE_SIZE)) {
        /* For MisraC check */
    }

    if (pCardInfo->MemCardInfo.BusWidth == 4U) {
        Rval = AmbaRTSL_SdSendEmmcCMD21(SdChanNo, AmbaRTSL_SdEmmcCmd21TuningBlkPattern, AMBA_SD_TUNING_BLOCK_BYTE_SIZE);
    } else {
        Rval = AmbaRTSL_SdSendEmmcCMD21(SdChanNo, AmbaRTSL_SdEmmcCmd21TuningBlkPattern, 2U * AMBA_SD_TUNING_BLOCK_BYTE_SIZE);
    }

    if (Rval == 0) {
        Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
    }

    if (Rval == 0) {

        if (pCardInfo->MemCardInfo.BusWidth == 4U) {
            if (AmbaWrap_memcmp(AmbaRTSL_SdFixedTuningBlkPattern, AmbaRTSL_SdEmmcCmd21TuningBlkPattern,
                                AMBA_SD_TUNING_BLOCK_BYTE_SIZE, &CmpResult) != 0U) {
                RetVal = AMBA_SD_ERR_CHECK_PATTERN;
            } else {
                if (CmpResult != 0) {
                    RetVal = AMBA_SD_ERR_CHECK_PATTERN;
                }
            }
        } else {
            if (AmbaWrap_memcmp(AmbaRTSL_SdEmm8bitTuningBlkPattern, AmbaRTSL_SdEmmcCmd21TuningBlkPattern,
                                2U * AMBA_SD_TUNING_BLOCK_BYTE_SIZE, &CmpResult) != 0U) {
                RetVal = AMBA_SD_ERR_CHECK_PATTERN;
            } else {
                if (CmpResult != 0) {
                    RetVal = AMBA_SD_ERR_CHECK_PATTERN;
                }
            }
        }
    } else {
        RetVal = AMBA_SD_ERR_CHECK_PATTERN;
    }
    return RetVal;
}

static UINT32 AmbaRTSL_SdMmcHS200CardInit(UINT32 SdChanNo)
{
    INT32 Rval, i;
    UINT32 RetVal = 0U;
    const AMBA_SD_CTRL_s*pSdCtrl   = &AmbaSD_Ctrl[SdChanNo];
    AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    AMBA_SD_INFO_REG_s  *pCardRegs = &AmbaSD_Ctrl[SdChanNo].CardRegs;
    AMBA_SD_INFO_EXT_CSD_REG_s ExtCsd GNU_ALIGNED_CACHESAFE;

    (void) (pSdCtrl);

    Rval = AmbaRTSL_EmmcSetupHS200Mode(SdChanNo);

    if (Rval == 0) {
        Rval = AmbaRTSL_SdSendEmmcCMD8(SdChanNo, &ExtCsd);
        if (Rval == 0) {
            Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
        }

        if (Rval == 0) {
            if (OK != AmbaWrap_memcpy(&pCardRegs->ExtCsd, &ExtCsd, sizeof(ExtCsd))) {
                /* For MisraC check */
            }

            if (pCardInfo->MemCardInfo.DesiredClock > pSdCtrl->SdConfig.SdSetting.MaxFrequency) {
                pCardInfo->MemCardInfo.DesiredClock = pSdCtrl->SdConfig.SdSetting.MaxFrequency;
            }

            (void) pSdCtrl->SetClock(pCardInfo->MemCardInfo.DesiredClock);
            pCardInfo->MemCardInfo.CurrentClock = pSdCtrl->GetClock();

            AmbaRTSL_SDSetRdLatencyCtrl(SdChanNo, AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.HsRdLatency);
        }

        if (Rval == 0) {
            for (i = 0 ; i < 3; i ++) {
                RetVal = AmbaRTSL_SdMmcCheckTuningBlock(SdChanNo, pCardInfo);

                if (RetVal != 0U) {
                    break;
                }
            }
        }
    }

    if (Rval != 0) {
        RetVal = AMBA_SD_ERR_TIMEOUT;
    }

    return RetVal;
}

static INT32 AmbaRTSL_SdACMD1_Polling(UINT32 SdChanNo)
{
    AMBA_SD_CMD1_ARGUMENT_s Cmd1Arg = {0};
    INT32 Rval;
    UINT32 OpCondition;
    AMBA_SD_CTRL_s      *pSdCtrl   = &AmbaSD_Ctrl[SdChanNo];
    AMBA_SD_INFO_REG_s  *pCardRegs = &AmbaSD_Ctrl[SdChanNo].CardRegs;
    UINT32 Temp;

    pSdCtrl->SdType = AMBA_SD_eMMC;

    Temp = 0x40ff8000;
    AmbaMisra_TypeCast32(&Cmd1Arg, &Temp);

    /* Repeate ACMD1 at least 1 sec or until the memory becomes ready  */
    for (INT32 PollCount = 0; PollCount < 1000; PollCount++) {
        Rval = AmbaRTSL_SdSendCMD1(SdChanNo, &Cmd1Arg, &OpCondition);

        if (Rval == 0) {
            Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, AMBA_SD_IRQ_CMD_DONE);
        }

        if (Rval == 0) {
            AmbaRTSL_SdGetResponse48Bits(SdChanNo, &OpCondition);
            AmbaMisra_TypeCast32(&pCardRegs->OpCondition, &OpCondition);
        }

        if ((pCardRegs->OpCondition.Busy == 1U) || (Rval != 0)) {
            break;
        }
    }

    return Rval;
}

static INT32 AmbaRTSL_SdMmcDSModeSendInitCmd(UINT32 SdChanNo)
{
    AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    AMBA_SD_INFO_REG_s  *pCardRegs = &AmbaSD_Ctrl[SdChanNo].CardRegs;
    AMBA_SD_RESPONSE_R6_s R6Response = {0};
    UINT32 Response;

    INT32 Rval = AmbaRTSL_SdACMD1_Polling(SdChanNo);

    if (Rval == 0) {
        pCardInfo->MemCardInfo.AddressMode = pCardRegs->OpCondition.Ccs;

        Rval = AmbaRTSL_SdSendCMD2(SdChanNo, &pCardRegs->CardID);
        if (Rval == 0) {
            Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, AMBA_SD_IRQ_CMD_DONE);
        }

        if (Rval == 0) {
            Rval = AmbaRTSL_SdSendCMD2Done(SdChanNo, &pCardRegs->CardID);
        }
    }

    //    if ((AmbaUserSD_DelayCtrl != NULL) && UserRTSLSDDelayCtrlEnable)
    //        AmbaUserSD_DelayCtrl(SdChanNo, (AMBA_SD_INFO_CID_INFO_u *) &pCardRegs->CardID);

    if (Rval == 0) {
        Rval = AmbaRTSL_SdSendCMD3(SdChanNo);
        if (Rval == 0) {
            Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, AMBA_SD_IRQ_CMD_DONE);
        }

        if (Rval == 0) {
            AmbaRTSL_SdGetResponse48Bits(SdChanNo, &Response);
            AmbaMisra_TypeCast32(&R6Response, &Response);
            pCardRegs->RelativeCardAddr = R6Response.PublishedRCA;
            AmbaRTSL_SdStoreCardRca(SdChanNo, pCardRegs->RelativeCardAddr);
        }
    }

    if (Rval == 0) {
        Rval = AmbaRTSL_SdSendCMD9(SdChanNo);
        if (Rval == 0) {
            Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, AMBA_SD_IRQ_CMD_DONE);
        }

        if (Rval == 0) {
            Rval = AmbaRTSL_SdSendCMD9Done(SdChanNo, &pCardRegs->CardSpecificData);
        }
    }

    if (Rval == 0) {
        Rval = AmbaRTSL_SdSendCMD7(SdChanNo, pCardRegs->RelativeCardAddr);
        if (Rval == 0) {
            Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
        }
    }
    return Rval;
}

static INT32 AmbaRTSL_SdMmcDSModeCardInit(UINT32 SdChanNo)
{
    const AMBA_SD_CTRL_s*pSdCtrl   = &AmbaSD_Ctrl[SdChanNo];
    AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    AMBA_SD_INFO_REG_s  *pCardRegs = &AmbaSD_Ctrl[SdChanNo].CardRegs;
    INT32 Rval;
    AMBA_SD_INFO_EXT_CSD_REG_s ExtCsd GNU_ALIGNED_CACHESAFE;

    (void) pSdCtrl;

    /* Send CMD1, CMD2, CMD3, CMD9, CMD7 */
    Rval = AmbaRTSL_SdMmcDSModeSendInitCmd(SdChanNo);

    if (Rval == 0) {
        Rval = AmbaRTSL_SdSendCMD16(SdChanNo, 512U);
        if (Rval == 0) {
            Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, AMBA_SD_IRQ_CMD_DONE);
        }
    }

    if (Rval == 0) {
        Rval = AmbaRTSL_SdSendEmmcCMD8(SdChanNo, &ExtCsd);
        if (Rval == 0) {
            Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
        }
        if (OK != AmbaWrap_memcpy(&pCardRegs->ExtCsd, &ExtCsd, sizeof(ExtCsd))) {
            /* For MisraC check */
        }
    }
#if defined(CONFIG_ENABLE_EMMC_HIGHSPEED_TIMING)

    if (Rval == 0) {
        AMBA_SD_EMMC_CMD6_ARGUMENT_s ArgCmd6 = {0};

        /* The card is able to operate at higher frequence */
        ArgCmd6.Access = ACCESS_WRITE_BYTE;
        ArgCmd6.Index  = 0xb9U;
        ArgCmd6.Value  = 0x1U;

        Rval = AmbaRTSL_SdSendEmmcCMD6(SdChanNo, &ArgCmd6);
        if (Rval == 0) {
            Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
        }
    }
#endif

    if (Rval == 0) {
        /* Crank up the clock to desired frequcnce */
        pCardInfo->MemCardInfo.DesiredClock = 50000000U;

        if (pCardInfo->MemCardInfo.DesiredClock > pSdCtrl->SdConfig.SdSetting.MaxFrequency) {
            pCardInfo->MemCardInfo.DesiredClock = pSdCtrl->SdConfig.SdSetting.MaxFrequency;
        }

        (void) pSdCtrl->SetClock(pCardInfo->MemCardInfo.DesiredClock);
        pCardInfo->MemCardInfo.CurrentClock = pSdCtrl->GetClock();
    }

    return Rval;
}
/*
 *  @RoutineName:: AmbaRTSL_SdMmcCardInit
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
static UINT32 AmbaRTSL_SdMmcCardInit(UINT32 SdChanNo)
{
    INT32 Rval, i = 0;
    UINT32 RetVal = 0U;
    AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    const AMBA_SD_INFO_REG_s  *pCardRegs = &AmbaSD_Ctrl[SdChanNo].CardRegs;

    AMBA_SD_EMMC_CMD6_ARGUMENT_s ArgCmd6;
    static UINT8 RTSL_SdCheckBuf[1024] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    if (OK != AmbaWrap_memset(&ArgCmd6, 0, sizeof(AMBA_SD_EMMC_CMD6_ARGUMENT_s))) {
        /* For MisraC check */
    }
    (void) (pCardRegs);

    Rval = AmbaRTSL_SdMmcDSModeCardInit(SdChanNo);

    if (Rval == 0) {
        ArgCmd6.Access = ACCESS_WRITE_BYTE;
        ArgCmd6.Index  = 0xb7U;

#if defined(CONFIG_EMMC_ACCESS_8BIT)
        ArgCmd6.Value  = 0x2U; /* 8bit mode */
#elif defined(CONFIG_EMMC_ACCESS_4BIT)
        ArgCmd6.Value  = 0x1U; /* 4bit mode */
#elif defined(CONFIG_EMMC_ACCESS_1BIT)
        ArgCmd6.Value  = 0x0U; /* 1bit mode */
#else
        ArgCmd6.Value  = 0x1U; /* 4bit mode */
#endif

        Rval = AmbaRTSL_SdSendEmmcCMD6(SdChanNo, &ArgCmd6);
        if (Rval == 0) {
            Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
        }
    }

    if (Rval == 0) {
        if (ArgCmd6.Value == 0U) {
            pCardInfo->MemCardInfo.BusWidth = 1U;
        } else {
            pCardInfo->MemCardInfo.BusWidth = (ArgCmd6.Value == 2U) ? 8U : 4U;
        }

        AmbaRTSL_SdSetHostBusWidth(SdChanNo, pCardInfo->MemCardInfo.BusWidth);

        /* See if the card is in write-protect Mode */
        pCardInfo->MemCardInfo.WriteProtect = 0;

        if(AmbaEMMC_HS200SupportChk(SdChanNo) != 0) {
            RetVal = AmbaRTSL_SdMmcHS200CardInit(SdChanNo);
        }
    }

    if ((Rval == 0) && (RetVal == 0U)) {

        /* Do bus test to make sure current clock is ok */
        for (i = 0; i < 3; i++) {
            Rval = AmbaRTSL_SdSendCMD18(SdChanNo, 0x4000U, (UINT32)((sizeof(RTSL_SdCheckBuf) / 512U) & 0xffffffffU), RTSL_SdCheckBuf);
            if (Rval == 0) {
                Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
            }
            if (Rval != 0) {
                break;
            }
        }

        if (Rval == 0) {
            /* See if the card is in write-protect Mode */
            pCardInfo->MemCardInfo.WriteProtect = 0;

            pCardInfo->MemCardInfo.TotalSectors =  pCardRegs->ExtCsd.Properties.SecCount;
        }
    }

    if (Rval != 0) {
        RetVal = AMBA_SD_ERR_TIMEOUT;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdCardInit
 *
 *  @Description:: SD RTSL initializations
 *
 *  @Input      :: void
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_SdCardInit(UINT32 SdChanNo)
{
    AMBA_SD_CTRL_s      *pSdCtrl   = &AmbaSD_Ctrl[SdChanNo];
    AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[SdChanNo].CardInfo;
    UINT32 RetVal = 0U;
    INT32 Rval;
    //UINT32 Delay = AmbaRTSL_PllGetNumCpuCycleUs();

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {

        if (OK != AmbaWrap_memset(&pSdCtrl->CardInfo, 0x0, sizeof(AMBA_SD_CARD_INFO_s))) {
            /* For MisraC check */
        }
        if (OK != AmbaWrap_memset(&pSdCtrl->CardRegs, 0x0, sizeof(AMBA_SD_INFO_REG_s))) {
            /* For MisraC check */
        }

        pSdCtrl->NumIoFunction = 0U;
        pSdCtrl->SdType = AMBA_SD_eMMC;

        /* power cycle */
        if (pSdCtrl->SdConfig.PowerCtrl != NULL) {
            pSdCtrl->SdConfig.PowerCtrl(SdChanNo, AMBA_SD_VDD_POWER_OFF);
            //AmbaDelayCycles(pSdCtrl->SdConfig.SdSetting.PowerCycleDelay * Delay);
            pSdCtrl->SdConfig.PowerCtrl(SdChanNo, AMBA_SD_VDD_3D30V);
            //AmbaDelayCycles(100U * Delay);  // wait for power stable
        }

        (void) pSdCtrl->SetClock(pSdCtrl->SdConfig.SdSetting.InitFrequency);
        pCardInfo->MemCardInfo.CurrentClock = pSdCtrl->GetClock();

        AmbaRTSL_SdClockEnable(SdChanNo, 1U);

        Rval = AmbaRTSL_SdSendCMD0(SdChanNo, 0x0U);
        if (Rval == 0) {
            Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, AMBA_SD_IRQ_CMD_DONE);
        }

        if (Rval == 0) {
            RetVal = AmbaRTSL_SdMmcCardInit(SdChanNo);
        }
    } else {
        RetVal = AMBA_SD_ERR_INVALID_ARG;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdConfig
 *
 *  @Description:: SD device driver configurations
 *
 *  @Input      ::
 *      SdChanNo:  SD Channel Number
 *      pSdConfig: pointer to the SD controller configurations
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_SdConfig(UINT32 SdChanNo, const AMBA_SD_CONFIG_s *pSdConfig)
{

    AMBA_SD_CTRL_s      *pSdCtrl   = &AmbaSD_Ctrl[SdChanNo];
    AMBA_INT_CONFIG_s IntConfig;
    UINT32 k, RetVal = 0U;
    const AMBA_RTSL_SD_CTRL_s *pSdRtslCtrl = (AMBA_RTSL_SD_CTRL_s *) & (AmbaRTSL_SdCtrl[SdChanNo]);
    AMBA_SD_REG_s *pSdReg = pAmbaSD_Reg[SdChanNo];  /* pointer to SD H/W Registers */

    if ((SdChanNo < AMBA_NUM_SD_CHANNEL) && (pSdConfig != NULL)) {

        if (SdChanNo == AMBA_SD_CHANNEL0) {
            pSdCtrl->GetClock = AmbaRTSL_PllGetSd0Clk;
            pSdCtrl->SetClock = AmbaRTSL_PllSetSd0Clk;
        } else {
            pSdCtrl->GetClock = AmbaRTSL_PllGetSd1Clk;
            pSdCtrl->SetClock = AmbaRTSL_PllSetSd1Clk;
        }

        (void) AmbaRTSL_SdSetDriveStrengthAll(SdChanNo, &pSdConfig->SdSetting);

        /* save SD configurations */
        if (OK != AmbaWrap_memcpy(&(AmbaSD_Ctrl[SdChanNo].SdConfig), pSdConfig, sizeof(AMBA_SD_CONFIG_s))) {
            /* For MisraC check */
        }

        /* SD isr and interput setup */
        IntConfig.TriggerType = INT_TRIG_HIGH_LEVEL;
        IntConfig.IrqType = INT_TYPE_FIQ;  /* Since Linux does not suuport GIC group1 interrupt handling, set it as FIQ for AmbaLink. */
        IntConfig.CpuTargets = 0x01;          /* Target cores */

        AmbaCSL_SdClearIrqStatus(pSdReg, 0xffffffffU);
        AmbaCSL_SdResetAll(SdChanNo, pSdReg);
        AmbaCSL_SdEnableIrq(pSdReg);

        k = pSdRtslCtrl->IntID;

        (void) AmbaRTSL_GicIntConfig(k, &IntConfig, pSdRtslCtrl->pISR, k);
        (void) AmbaRTSL_GicIntEnable(k);           /* Enable the Interrupt */

        SdCmdResponce[SdChanNo][0].OpCode   = AMBA_SD_CMD2_ALL_SEND_CID;
        SdCmdResponce[SdChanNo][1].OpCode   = AMBA_SD_CMD9_SEND_CSD;
        SdCmdResponce[SdChanNo][2].OpCode   = AMBA_SD_CMD13_SEND_STATUS;
        SdCmdResponce[SdChanNo][3].OpCode   = AMBA_SD_ACMD51_SEND_SCR;
        SdCmdResponce[SdChanNo][4].OpCode   = AMBA_SD_CMD8_SEND_EXT_CSD;

        if ((SdChanNo == AMBA_SD_CHANNEL0) && (pSdConfig->pSysPartConfig != NULL)) {
            UINT32 ByteCount;
            AmbaRTSL_SdCtrl[SdChanNo].pSysPartConfig  = pSdConfig->pSysPartConfig;
            AmbaRTSL_SdCtrl[SdChanNo].pUserPartConfig = pSdConfig->pUserPartConfig;

            /* Number of Pages for BST */
            ByteCount = pSdConfig->pSysPartConfig[AMBA_SYS_PARTITION_BOOTSTRAP].ByteCount;
            AmbaRTSL_SdCtrl[SdChanNo].BstPageCount = (UINT16) GetRoundUpValU32(ByteCount, 512U);

            /* Number of Pages for System Partition Table */
            AmbaRTSL_SdCtrl[SdChanNo].SysPtblPageCount = (UINT16) (GetRoundUpValU32((UINT32)(sizeof(AMBA_SYS_PARTITION_TABLE_s) & 0xffffffffU), 512U) & 0xffffU);

            /* Number of Pages for User Partition Table */
            AmbaRTSL_SdCtrl[SdChanNo].UserPtblPageCount = (UINT16) (GetRoundUpValU32((UINT32)(sizeof(AMBA_USER_PARTITION_TABLE_s) & 0xffffffffU), 512U) & 0xffffU);

            /* Number of Pages for Vendor Specific Data */
            ByteCount = pSdConfig->pUserPartConfig[AMBA_USER_PARTITION_PTB].ByteCount;
            AmbaRTSL_SdCtrl[SdChanNo].VendorDataPageCount = (UINT16) GetRoundUpValU32(ByteCount, 512U);
        }
    } else {
        RetVal = AMBA_SD_ERR_INVALID_ARG;
    }

    return RetVal;
}

static void SdSaveResponse(UINT32 SdChanNo, const UINT32 *pResp)
{
    INT32 i;
    UINT32 *pPtr;

    const AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);

    for (i = 0; i < SAVE_RESP_NUM; i++) {
        if (pCmdConfig->CmdIdx == SdCmdResponce[SdChanNo][i].OpCode) {
            pPtr = SdCmdResponce[SdChanNo][i].Resp;

            /* pPtr[0] = (pResp[2] << 24) | (pResp[3] >> 8); */
            pPtr[0] = pResp[0];
            break;
        }
    }
}

static void SdSaveResponse136Bits(UINT32 SdChanNo, const UINT32 *pResp)
{
    INT32 i;
    UINT32 *pPtr;

    const AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);

    for (i = 0; i < SAVE_RESP_NUM; i++) {
        if (pCmdConfig->CmdIdx == SdCmdResponce[SdChanNo][i].OpCode) {
            pPtr = SdCmdResponce[SdChanNo][i].Resp;

            if (pCmdConfig->ResponseType == AMBA_SD_RESPONSE_LENGTH_136_BIT) {
                if (OK != AmbaWrap_memcpy(pPtr, pResp, sizeof(UINT32) * 4U)) {
                    /* For MisraC check */
                }
            }
            break;
        }
    }
}

INT32 AmbaRTSL_SdGetResponse(UINT32 SdChanNo, UINT32 OpCode, UINT32 *pResp)
{
    INT32 i;
    INT32 ReVal = NG;

    for (i = 0; i < SAVE_RESP_NUM; i++) {
        if (OpCode == SdCmdResponce[SdChanNo][i].OpCode) {
            if (OK != AmbaWrap_memcpy(pResp, SdCmdResponce[SdChanNo][i].Resp, sizeof(UINT32) * 4U)) {
                /* For MisraC check */
            }
            ReVal = 0;
        }
    }
    return ReVal;
}

/*
 *  @RoutineName:: SD_IntHandler
 *
 *  @Description:: ISR for SD Controller
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */

/* disable NEON registers usage in ISR */
#pragma GCC push_options
#pragma GCC target("general-regs-only")

static void SD_IntHandler(UINT32 SdChanNo)
{
    AMBA_SD_REG_s *pSdReg = pAmbaSD_Reg[SdChanNo];     /* pointer to SD H/W Registers */
    UINT32 IrqStatus;

    IrqStatus = AmbaCSL_SdGetIrqStatus(pSdReg);    /* read  IRQ status */
    AmbaCSL_SdClearIrqStatus(pSdReg, IrqStatus);   /* clear IRQ status */

    /* Check for DMA interrupt */
    if ((IrqStatus & AMBA_SD_IRQ_DMA) != 0U) {
        UINT32 DmaAddr = AmbaCSL_SdGetSdmaSysMemAddr(pSdReg);
        AmbaCSL_SdSetSdmaSysMemAddr(pSdReg, (UINT32) DmaAddr);
    }

    if ((IrqStatus & AMBA_SD_IRQ_CARD) != 0U) {
        const AMBA_SD_CONFIG_s *pSdConfig = &AmbaSD_Ctrl[SdChanNo].SdConfig;
        if (pSdConfig->SdioCardIsr != NULL) {
            pSdConfig->SdioCardIsr();
        }
    }

    if (AmbaRTSL_SdIsrCallBack != NULL) {
        AmbaRTSL_SdIsrCallBack(SdChanNo, IrqStatus);
    }
}
#pragma GCC pop_options

/*
 *  @RoutineName:: SD_Chan0ISR
 *
 *  @Description:: SD Channel-0 ISR
 *
 *  @Input      ::
 *      IntID: Interrupt ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
static void SD_Chan0ISR(UINT32 IntID, UINT32 UserArg)
{
    (void) IntID;
    (void) UserArg;
    SD_IntHandler(AMBA_SD_CHANNEL0);
}

/*
 *  @RoutineName:: SD_Chan1ISR
 *
 *  @Description:: SD Channel-1 ISR
 *
 *  @Input      ::
 *      IntID: Interrupt ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
static void SD_Chan1ISR(UINT32 IntID, UINT32 UserArg)
{
    (void) IntID;
    (void) UserArg;
    SD_IntHandler(AMBA_SD_CHANNEL1);
}

/*
 *  @RoutineName:: SD_Chan1ISR
 *
 *  @Description:: SD Channel-2 ISR
 *
 *  @Input      ::
 *      IntID: Interrupt ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
static void SD_Chan2ISR(UINT32 IntID, UINT32 UserArg)
{
    (void) IntID;
    (void) UserArg;
    SD_IntHandler(AMBA_SD_CHANNEL2);
}

void AmbaRTSL_SdStoreCardRca(UINT32 SdChanNo, UINT16 Rca)
{
    AmbaRTSL_SdCtrl[SdChanNo].Rca = Rca;
}

/*
 *  @RoutineName:: AmbaRTSL_SdResetAll
 *
 *  @Description:: Reset SD controller and enable IRQ
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_SdResetAll(UINT32 SdChanNo)
{
    AMBA_SD_REG_s *pSdReg = pAmbaSD_Reg[SdChanNo];     /* pointer to SD H/W Registers */
    AmbaCSL_SdClearIrqStatus(pSdReg, 0xffffffffU);
    AmbaCSL_SdResetAll(SdChanNo, pSdReg);
    AmbaCSL_SdEnableIrq(pSdReg);
}

/*
 *  @RoutineName:: AmbaRTSL_SdioCardIrqEnable
 *
 *  @Description:: Enable/Disable SD controller IO card IRQ
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_SdioCardIrqEnable(UINT32 SdChanNo, UINT32 Enable)
{
    AMBA_SD_REG_s *pSdReg = pAmbaSD_Reg[SdChanNo];
    AmbaCSL_SdioEnableCardIrq(pSdReg, Enable);
}

UINT32 AmbaRTSL_SdGetDescTransferSize(void)
{
    return AMBA_SD_DESC_TOTAL_TRAN_SIZE;
}

UINT32 AmbaRTSL_SdGetWorkBufferSize(void)
{
    return AMBA_SD_WORK_BUFFER_SIZE;
}

/*
 *  @RoutineName:: AmbaRTSL_SdGetWriteProtectPin
 *
 *  @Description:: Get SD write protect pin status
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int :
 */
UINT32 AmbaRTSL_SdGetWriteProtectPin(UINT32 SdChanNo)
{
    const AMBA_SD_REG_s *pSdReg = pAmbaSD_Reg[SdChanNo];
    UINT32 CardState;
#ifdef CONFIG_ENABLE_EMMC_BOOT
    if (SdChanNo == AmbaRTSL_EmmcGetBootChannel()) {
        CardState = 0U;
    } else {
        CardState = AmbaCSL_SdGetWriteProtectPin(pSdReg);
    }
#else
    CardState = AmbaCSL_SdGetWriteProtectPin(pSdReg);
#endif
    return CardState;
}

/*
 *  @RoutineName:: AmbaRTSL_SdCardPresent
 *
 *  @Description:: Check if SD card is present or not in the slot
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : 0 - not Present; no-0 - Present
 */
UINT32 AmbaRTSL_SdCardPresent(UINT32 SdChanNo)
{
    const AMBA_SD_REG_s *pSdReg = pAmbaSD_Reg[SdChanNo];
    UINT32 CardState;

#ifdef CONFIG_ENABLE_EMMC_BOOT
    if (SdChanNo == AmbaRTSL_EmmcGetBootChannel()) {
        CardState = 1U;
    } else {
        CardState = AmbaCSL_SdGetCardDetectionPin(pSdReg);
    }
#else
    CardState = AmbaCSL_SdGetCardDetectionPin(pSdReg);
#endif
    return CardState;
}

UINT32 AmbaRTSL_SdGetLowDataPinStatus(UINT32 SdChanNo)
{
    const AMBA_SD_REG_s *pSdReg = pAmbaSD_Reg[SdChanNo];

    return AmbaCSL_SdGetDataLineStatus(pSdReg);
}

static UINT32 AmbaRTSL_SdParseIrqError(UINT32 IrqStatus)
{
    UINT32 RetVal;
    RetVal = ((IrqStatus & AMBA_SD_IRQ_CMD_TIME_OUT_ERROR) != 0U) ? AMBA_SD_ERR_TIMEOUT :
             ((IrqStatus & AMBA_SD_IRQ_CMD_CRC_ERROR) != 0U)      ? AMBA_SD_ERR_BADCRC  :
             ((IrqStatus & AMBA_SD_IRQ_CMD_INDEX_ERROR) != 0U)    ? AMBA_SD_ERR_INDEX   :
             ((IrqStatus & AMBA_SD_IRQ_CMD_END_BIT_ERROR) != 0U)  ? AMBA_SD_ERR_ENDBIT  :
             ((IrqStatus & AMBA_SD_IRQ_DATA_TIME_OUT_ERROR) != 0U)? AMBA_SD_ERR_TIMEOUT :
             ((IrqStatus & AMBA_SD_IRQ_DATA_CRC_ERROR) != 0U)     ? AMBA_SD_ERR_BADCRC  :
             ((IrqStatus & AMBA_SD_IRQ_DATA_END_BIT_ERROR) != 0U) ? AMBA_SD_ERR_ENDBIT  :
             ((IrqStatus & AMBA_SD_IRQ_ADMA_ERROR) != 0U)         ? AMBA_SD_ERR_ADMA    :
             ((IrqStatus & AMBA_SD_IRQ_AUTO_CMD12_ERROR) != 0U)   ? AMBA_SD_ERR_ACMD12  :
             AMBA_SD_ERR_INVALID;
    return RetVal;
}

INT32 AmbaRTSL_SdWaitCmdDone(UINT32 SdChanNo, UINT32 Flag)
{
    const AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    AMBA_SD_REG_s *pSdReg = pAmbaSD_Reg[SdChanNo];     /* pointer to SD H/W Registers */
    UINT32 IrqStatus;


    UINT32 StartTime, EndTime = 0U;
    UINT32 RetVal = 0U;
    INT32 Rval;
    UINT32 i;

    /* Wait for command to complete */

    AmbaCSL_RctTimer0Reset();   /* reset RCT Timer */
    AmbaCSL_RctTimer0Enable();  /* enable RCT Timer */
    StartTime = AmbaCSL_RctTimer0GetCounter();

    do {
        /* Read the interrupt registers */

        IrqStatus = AmbaCSL_SdGetIrqStatus(pSdReg);    /* read  IRQ status */

        if ((IrqStatus & AMBA_SD_IRQ_ERROR) != 0U) {
            break;
        }

        /* Check for DMA interrupt */
        if ((IrqStatus & AMBA_SD_IRQ_DMA) != 0U) {
            UINT32 DmaAddr = AmbaCSL_SdGetSdmaSysMemAddr(pSdReg);
            AmbaCSL_SdSetSdmaSysMemAddr(pSdReg, (UINT32) DmaAddr);
        }

        if ((IrqStatus & Flag) == Flag) {
            if (pCmdConfig->CmdIdx == AMBA_SD_CMD18_READ_MULTIPLE_BLOCK) {
                for (i = 0; i < 0xFFFFFFFFU; i++) {
                    if (pSdReg->BlkCtrl.BlkCount == 0U) {
                        break;
                    }
                }
            }
            RetVal = Flag;
        }
        EndTime = AmbaCSL_RctTimer0GetCounter();
    } while(((EndTime - StartTime) < 24000000U) && (RetVal != Flag));


    AmbaCSL_RctTimer0Reset();   /* reset RCT Timer */

    /* Clear interrupt */
    AmbaCSL_SdClearIrqStatus(pSdReg, 0xffffffffU);

    /* Error occured, abort current transaction */
    if ((IrqStatus & AMBA_SD_IRQ_ERROR) != 0U) {
        RetVal = AmbaRTSL_SdParseIrqError(IrqStatus);
        AmbaRTSL_SdResetAll(SdChanNo);
    } else if ((RetVal & Flag) != Flag) {
        RetVal = AMBA_SD_ERR_ISR_TIMEOUT;
    } else {
        /* Do not thing */
    }

    if (RetVal == Flag) {
        Rval = 0;
    } else {
        Rval = -1;
    }
    return Rval;
}

/*
 *  @RoutineName:: AmbaRTSL_SdGetResponse
 *
 *  @Description:: Get SD card command response.
 *
 *                 Type of Response   Meaning of Response      Response Field  Response Register
 *                 R1, R1b (normal)   Card Status              R[39:8]         REP[31:0]
 *                 R1b (Auto CMD12)   Card Status for ACMD12   R[39:8]         REP[127:96]
 *                 R3 (OCR Register)  OCR Register for memory  R[39:8]         REP[31:0]
 *                 R4 (OCR Register)  OCR Register for I/O     R[39:8]         REP[31:0]
 *                 R5, R5b            SDIO Response            R[39:8]         REP[31:0]
 *                 R6 (published RCA) New published RCA[31:16] R[39:8]         REP[31:0]
 *                 R2 (CID, CSD)      CID or CSD reg           R[127:8]        REP[119:0]
 *
 *  @Input      ::
 *                 pSdReg  - Pointer to SD host control register.
 *
 *  @Output     :: Resp - Point to SD card response buffer.
 *
 *  @Return     :: none
 *
 */
void AmbaRTSL_SdGetResponse48Bits(UINT32 SdChanNo, UINT32 *pResp)
{
    const AMBA_SD_REG_s *pSdReg = pAmbaSD_Reg[SdChanNo];
    const AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);

    if (pCmdConfig->ResponseType == AMBA_SD_RESPONSE_LENGTH_48_BIT_CHECK_BUSY) {
        (*pResp) = AmbaCSL_SdGetResponse(pSdReg, 3);
    } else {
        (*pResp) = AmbaCSL_SdGetResponse(pSdReg, 0);
    }

    SdSaveResponse(SdChanNo, (UINT32 *)pResp);
}

UINT32 AmbaRTSL_SdParseResponse(const AMBA_SD_RESPONSE_R2_s *pResponse, UINT32 StartBits, UINT32 BitsSize)
{
    const UINT32 *pResp = pResponse->CidCsdReg;

    UINT32 Offset = 3U - ((StartBits) / 32U);
    UINT32 Shift  = (StartBits) & 31U;
    UINT32 Res   = pResp[Offset] >> Shift;
    UINT32 Mask  = ((UINT32) 1U << (BitsSize & 0x1FU)) - 1U;
    UINT32 Temp;

    if ((BitsSize & 0x20U) != 0U) {
        Mask = 0xFFFFFFFFU;
    }

    Temp = BitsSize + Shift;
    if ((Temp >= 32U) && (Offset > 1U) && (Offset < 4U)) {
        Res |= pResp[Offset - 1U] << (32U - Shift);
    }

    return Res & Mask;
}

void AmbaRTSL_SdGetResponse136Bits(UINT32 SdChanNo, AMBA_SD_RESPONSE_R2_s *pResp)
{
    const AMBA_SD_REG_s *pSdReg = pAmbaSD_Reg[SdChanNo];
    const UINT32 *pTempResp;

    if (OK != AmbaWrap_memset(pResp, 0x0, sizeof(AMBA_SD_RESPONSE_R2_s))) {
        /* For MisraC check */
    }

    pResp->CidCsdReg[0] =   AmbaCSL_SdGetResponse(pSdReg, 3) << 8;
    pResp->CidCsdReg[0] |=  AmbaCSL_SdGetResponse(pSdReg, 2) >> 24;

    pResp->CidCsdReg[1] =   AmbaCSL_SdGetResponse(pSdReg, 2) << 8;
    pResp->CidCsdReg[1] |=  AmbaCSL_SdGetResponse(pSdReg, 1) >> 24;

    pResp->CidCsdReg[2] =   AmbaCSL_SdGetResponse(pSdReg, 1) << 8;
    pResp->CidCsdReg[2] |=  AmbaCSL_SdGetResponse(pSdReg, 0) >> 24;

    pResp->CidCsdReg[3] =   AmbaCSL_SdGetResponse(pSdReg, 0) << 8;

    AmbaMisra_TypeCast(&pTempResp, &pResp);
    SdSaveResponse136Bits(SdChanNo, pTempResp);
}

/*
 *  @RoutineName:: AmbaRTSL_SdClockEnable
 *
 *  @Description:: Send SD command
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *      Enable:   control flag: 1 - enabled; 0 - disabled
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
void AmbaRTSL_SdClockEnable(UINT32 SdChanNo, UINT32 Enable)
{
    AMBA_SD_REG_s *pSdReg = pAmbaSD_Reg[SdChanNo];

    AmbaCSL_SdClockEnable(pSdReg, Enable);
}

/*
 *  @RoutineName:: AmbaRTSL_SdSetHostBusWidth
 *
 *  @Description:: Set data transfer bus width
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *      Width:  Bus width of SD card
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_SdSetHostBusWidth(UINT32 SdChanNo, UINT32 BusWidth)
{
    AMBA_SD_REG_s *pSdReg = pAmbaSD_Reg[SdChanNo];

    AmbaRTSL_SdCtrl[SdChanNo].BusWidth = BusWidth;

    AmbaCSL_SdSetHostBusWidth(pSdReg, BusWidth);
}

/*
 *  @RoutineName:: AmbaRTSL_SdCheckVoltageSwitch
 *
 *  @Description:: Set data transfer bus width
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *      Width:  Bus width of SD card
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
INT32 AmbaRTSL_SdCheckVoltageSwitch(UINT32 SdChanNo, UINT32 Level)
{
    const AMBA_SD_REG_s *pSdReg = pAmbaSD_Reg[SdChanNo];
    UINT32 DataLevel;
    UINT32 CmdLevel;
    INT32 Rval = NG;

    if (Level != 0U) {
        DataLevel = 0xF;
        CmdLevel  = 1U;
    } else {
        DataLevel = 0U;
        CmdLevel  = 0U;
    }

    if ((AmbaCSL_SdVoltSwitchDatLineStat(pSdReg) == DataLevel)) {
        if ((AmbaCSL_SdVoltSwitchCmdLineStat(pSdReg)  == CmdLevel)) {
            Rval = 0;
        }
    }

    return Rval;
}

#if 0
/*
 *  @RoutineName:: AmbaRTSL_SdIrqStatusEnable
 *
 *  @Description:: Enable/Disable Irq Status
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *      Enable:   control flag: 1 - enabled; 0 - disabled
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
void AmbaRTSL_SdIrqStatusEnable(UINT32 SdChanNo, UINT32 Enable)
{
    UINT32 IrqEnable = AMBA_SD_IRQ_CMD_DONE            |
                       AMBA_SD_IRQ_TRANSFER_DONE       |
                       AMBA_SD_IRQ_BLK_GAP_EVENT       |
                       AMBA_SD_IRQ_DMA                 |
                       AMBA_SD_IRQ_CARD_INSERTION      |
                       AMBA_SD_IRQ_CARD_REMOVAL        |
                       AMBA_SD_IRQ_ERROR               |
                       AMBA_SD_IRQ_CMD_TIME_OUT_ERROR  |
                       AMBA_SD_IRQ_CMD_CRC_ERROR       |
                       AMBA_SD_IRQ_CMD_END_BIT_ERROR   |
                       AMBA_SD_IRQ_CMD_INDEX_ERROR     |
                       AMBA_SD_IRQ_DATA_TIME_OUT_ERROR |
                       AMBA_SD_IRQ_DATA_CRC_ERROR      |
                       AMBA_SD_IRQ_DATA_END_BIT_ERROR  |
                       AMBA_SD_IRQ_CURRENT_LIMIT_ERROR |
                       AMBA_SD_IRQ_AUTO_CMD12_ERROR    |
                       AMBA_SD_IRQ_ADMA_ERROR;
}
#endif

void AmbaRTSL_GeteMMCSpeedMode(AMBA_SD_EMMC_CMD6_ARGUMENT_s *pArgCmd6, const AMBA_SD_INFO_EXT_CSD_REG_s *pExtCsd)
{

    if ((pExtCsd->Properties.DeviceType & HIGH_SPEED_MMC_200MHZ_1_8V) != 0U) {
        pArgCmd6->Value = EMMC_HS200_MODE;
    } else {
        pArgCmd6->Value = HIGH_SPEED_MMC_52MHZ;
    }
}

void AmbaRTSL_GeteMMCDriveStrength(AMBA_SD_EMMC_CMD6_ARGUMENT_s *pArgCmd6, const AMBA_SD_INFO_EXT_CSD_REG_s *pExtCsd)
{
    UINT32 Val;
    UINT32 Tmp;

    if (OK != AmbaWrap_memcpy(&Val, pArgCmd6, sizeof(Val))) {
        /* MisraC */
    }

    /* It would use the dafult driving value first */
    if ((pExtCsd->Properties.DriverStrength & EMMC_DRV_STG_TYPE_0_SUPPORT) != 0U) {
        Tmp = ((UINT32)EMMC_DRV_STG_TYPE_0_MODE << (UINT32)8U);
        Val |= Tmp;
    } else if ((pExtCsd->Properties.DriverStrength & EMMC_DRV_STG_TYPE_1_SUPPORT) != 0U) {
        Tmp = ((UINT32)EMMC_DRV_STG_TYPE_1_MODE << (UINT32)8U);
        Val |= Tmp;
    } else if ((pExtCsd->Properties.DriverStrength & EMMC_DRV_STG_TYPE_4_SUPPORT) != 0U) {
        Tmp = ((UINT32)EMMC_DRV_STG_TYPE_1_MODE << (UINT32)8U);
        Val |= Tmp;
    } else if ((pExtCsd->Properties.DriverStrength & EMMC_DRV_STG_TYPE_2_SUPPORT) != 0U) {
        Tmp = ((UINT32)EMMC_DRV_STG_TYPE_2_MODE << (UINT32)8U);
        Val |= Tmp;
    } else {
        Tmp = ((UINT32)EMMC_DRV_STG_TYPE_3_MODE << (UINT32)8U);
        Val |= Tmp;
    }

    if (OK != AmbaWrap_memcpy(pArgCmd6, &Val, sizeof(AMBA_SD_EMMC_CMD6_ARGUMENT_s))) {
        /* MisraC */
    }
}

/*
 *  @RoutineName:: AmbaRTSL_SdCreateAdmaDescriptorTable
 *
 *  @Description:: Setup ADMA descriptor table
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
static INT32 AmbaRTSL_SdCreateAdmaDescriptorTable(UINT32 SdChanNo)
{
    UINT32 i;
    UINT32 NumDesc, Length, TempAddr;
    AMBA_SD_ADMA_DESCRIPTOR_s *pDesc = (AMBA_SD_ADMA_DESCRIPTOR_s *) & (pAmbaSD_AdmaDesc[SdChanNo][0]);
    const AMBA_SD_ADMA_DESCRIPTOR_s *pDescStartAddr = (AMBA_SD_ADMA_DESCRIPTOR_s *) & (pAmbaSD_AdmaDesc[SdChanNo][0]);
    const AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 Rval = 0;
#ifdef __aarch64__
    ULONG VirtAddr, PhysAddr;
    ULONG Tmp;
#else
    UINT32 VirtAddr, PhysAddr;
#endif

    Length = pCmdConfig->BlkSize * pCmdConfig->NumBlk;

    NumDesc = Length / AMBA_SD_DESC_MAX_TRAN_SIZE;
    if ((Length % AMBA_SD_DESC_MAX_TRAN_SIZE) != 0U) {
        NumDesc++;
    }

    if (NumDesc > AMBA_SD_DESC_COUNT) {
        Rval = NG;
    } else {

        /* Set up arrays */
        for (i = 0; i < NumDesc; i++) {
            if (OK != AmbaWrap_memset(pDesc, 0x0, sizeof(AMBA_SD_ADMA_DESCRIPTOR_s))) {
                /* For MisraC check */
            }

#ifdef __aarch64__
            AmbaMisra_TypeCast64(&VirtAddr, &pCmdConfig->pDataBuf);
            Tmp = AMBA_SD_DESC_MAX_TRAN_SIZE;
            Tmp = (ULONG)((ULONG)i * Tmp);
            //VirtAddr += (ULONG)((ULONG)i * (ULONG)AMBA_SD_DESC_MAX_TRAN_SIZE);
            VirtAddr = VirtAddr + Tmp;
            (void) AmbaMMU_Virt64ToPhys64(VirtAddr, &PhysAddr);
            pDesc->SysMemAddr = (UINT32)PhysAddr;
#else
            AmbaMisra_TypeCast32(&VirtAddr, &pCmdConfig->pDataBuf);
            VirtAddr += i * AMBA_SD_DESC_MAX_TRAN_SIZE;
            (void)  AmbaMMU_Virt32ToPhys32(VirtAddr, &PhysAddr);
            pDesc->SysMemAddr = (UINT32)PhysAddr;
#endif

            if (Length <= AMBA_SD_DESC_MAX_TRAN_SIZE) {
                /* set EOC */
                pDesc->Length = (UINT16) (Length);
                pDesc->AttributeEnd = 1;
            } else {
                pDesc->Length = 0;
            }

            pDesc->AttributeValid = 1;
            pDesc->AttributeWord  = 0;
            pDesc->AttributeAct   = AMBA_SD_ADMA_ATTR_TRANSFER;
            Length -= AMBA_SD_DESC_MAX_TRAN_SIZE;

            if (i < (NumDesc - 1U)) {
                pDesc++;
            }
        }

        AmbaMisra_TypeCast32(&TempAddr, &pDescStartAddr);
        (void)AmbaRTSL_CacheCleanData(TempAddr, NumDesc * (UINT32)(sizeof(AMBA_SD_ADMA_DESCRIPTOR_s) & 0xffffffffU));
    }

    return Rval;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCmd
 *
 *  @Description:: Send SD command
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCmd(UINT32 SdChanNo)
{
    AMBA_SD_REG_s        *pSdReg     = pAmbaSD_Reg[SdChanNo];
    const AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    AMBA_SD_TRANSFER_CTRL_REG_s TransferCtrl = {0};
    UINT32 TempTranCtrl;
    ULONG TempAddr;
    INT32 ReVal = 0;
    const AMBA_SD_ADMA_DESCRIPTOR_s *pAdma = &pAmbaSD_AdmaDesc[SdChanNo][0];
#ifdef __aarch64__
    ULONG PhysAddr;
    ULONG Addr = 0;
#else
    UINT32 PhysAddr;
    UINT32 Addr = 0;
#endif


    /* make sure Command Line is ready */
    if (AmbaCSL_SdWaitCmdLineReady(pSdReg) == 0U) {
        ReVal = NG;  /* something wrong ?! */
    } else {


        if ((pCmdConfig->NumBlk > 0U) && (pCmdConfig->pDataBuf != NULL)) {

            /* this command has data, to make sure Data Line is ready */
            if (AmbaCSL_SdWaitDataLineReady(pSdReg) == 0U) {
                ReVal = NG;  /* something wrong ?! */
            }

            /* 1. Create the Descriptor table for ADMA */
            if (ReVal == 0) {
                if (AmbaRTSL_SdCreateAdmaDescriptorTable(SdChanNo) != 0) {
                    ReVal = NG;
                }
            }

            if (ReVal == 0) {
                /* 2. Set the Descriptor address for ADMA: SD_adma_system_address[31:0] */
#ifdef __aarch64__
                AmbaMisra_TypeCast64(&Addr, &pAdma);
                (void) AmbaMMU_VirtToPhys(Addr, &PhysAddr);
                AmbaCSL_SdSetAdmaDescriptorAddr(pSdReg, (UINT32)PhysAddr);
#else
                AmbaMisra_TypeCast32(&Addr, &pAdma);
                (void) AmbaMMU_Virt32ToPhys32(Addr, &PhysAddr);
                AmbaCSL_SdSetAdmaDescriptorAddr(pSdReg, PhysAddr);
#endif

                /* 3. set block size (SD_block_size_and_count[15:0]) */
                AmbaCSL_SdSetBlkSize(pSdReg, pCmdConfig->BlkSize);

                /* 4. set block count (SD_block_size_and_count[31:16]) */
                AmbaCSL_SdSetBlkCount(pSdReg, (UINT16) pCmdConfig->NumBlk);

                AmbaCSL_SdSetDataTimeOutCounter(pSdReg, 0xE);

                AmbaCSL_SdAdmaEnable(pSdReg);

                AmbaCSL_SdSetHostBusWidth(pSdReg, AmbaRTSL_SdCtrl[SdChanNo].BusWidth);

                AmbaMisra_TypeCast32(&TempAddr, &pCmdConfig->pDataBuf);

                if (pCmdConfig->DataDirection == AMBA_SD_WRITE_DATA) {
                    (void)AmbaRTSL_CacheCleanData( TempAddr, pCmdConfig->BlkSize * pCmdConfig->NumBlk);
                } else {
                    (void)AmbaRTSL_CacheFlushData( TempAddr, pCmdConfig->BlkSize * pCmdConfig->NumBlk);
                }
            }
        }

        if (ReVal == 0) {
            /* 5. Set the Argument register value (SD_argument[31:0]) */
            AmbaCSL_SdSetCmdArgument(pSdReg, pCmdConfig->Argument); /* set command Argument */

            /* 6. Set the Transfer Mode register value (SD_transfer_mode_and_command[15:0]) */
            if ((pCmdConfig->NumBlk > 0U) && (pCmdConfig->pDataBuf != NULL)) {
                TransferCtrl.DataPresent = 1U;
                TransferCtrl.DmaEnable   = 1U;
                TransferCtrl.DataTransferDir = pCmdConfig->DataDirection;

                if (pCmdConfig->NumBlk > 1U) {
                    TransferCtrl.MultiBlkEnable  = 1U;
                    TransferCtrl.BlkCountEnable  = 1U;
                    /* SDIO do not use cmd12 */
                    if (pCmdConfig->CmdIdx != AMBA_SD_CMD53_IO_BLK_OPERATION) {
                        TransferCtrl.AutoCmd12Enable = 1U;
                    } else {
                        TransferCtrl.AutoCmd12Enable = 0U;
                    }
                }
            }

            /* 7. Set the Command register value (SD_transfer_mode_and_command[31:16]) */
            TransferCtrl.ResponseType = pCmdConfig->ResponseType;
            TransferCtrl.CmdIndex     = pCmdConfig->CmdIdx;

            if ((pCmdConfig->ResponseType != AMBA_SD_NO_RESPONSE)            &&
                (pCmdConfig->CmdIdx       != AMBA_SD_ACMD41_SD_SEND_OP_COND) &&
                (pCmdConfig->CmdIdx       != AMBA_SD_CMD5_IO_SEND_OP_COND)   &&
                (pCmdConfig->CmdIdx       != AMBA_SD_CMD1_SEND_OP_COND)) {
                /* All responses except for R3 (for ACMD41, CMD1) are protected by a CRC */
                TransferCtrl.CmdCrcCheckEnable = 1;

                if (pCmdConfig->ResponseType != AMBA_SD_RESPONSE_LENGTH_136_BIT) {
                    TransferCtrl.CmdIndexCheckEnable =  1U;
                }
            }

            AmbaMisra_TypeCast32(&TempTranCtrl, &TransferCtrl);
            AmbaCSL_SdSetTransferCtrl(pSdReg, TempTranCtrl);
        }
    }

    return ReVal;
}
#if 0
/*
 *  @RoutineName:: AmbaRTSL_SetDelayConfig
 *
 *  @Description:: Set SD Card Singal Delay Ctrl
 *
 *  @Input      ::
 *      SdChanNo:   SD Channel Number
 *      CardMaxFrequency: host max frequency
 *      RoundDelay: basic SD host delay setting
 *      DetailDelay:  full SD host delay setting
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SetDelayConfig(UINT32 SdChanNo, UINT32 DetailDelay)
{

    if(DetailDelay != 0) {
        AMBA_SD_DETAIL_DELAY_u SdDetailDelay;

        AmbaRTSL_SDPhyReset(SdChanNo);
        AmbaRTSL_SDPhySelReset(SdChanNo);
        AmbaRTSL_SDSetRdLatencyCtrl(SdChanNo, 0x0);

        SdDetailDelay.Data = DetailDelay;

        AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_DIN_CLK_POL, SdDetailDelay.Bits.DinClkPol);
        AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_RX_CLK_POL, SdDetailDelay.Bits.RXClkPol);
        AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_CLK_OUT_BYPASS, SdDetailDelay.Bits.ClkOutBypass);
        AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_DATA_CMD_BYPASS, SdDetailDelay.Bits.DataCmdBypass);
        AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_DLL_BYPASS, 0x0);
        AmbaRTSL_SDPhySetSharedBusCtrl(SdChanNo, AMBA_SD_PHY_SBC_COARSE_DELAY, SdDetailDelay.Bits.SbcCoreDelay);
        AmbaRTSL_SDPhySetSharedBusCtrl(SdChanNo, AMBA_SD_PHY_SBC_ENABLEDLL, 0x1);
        AmbaRTSL_SDPhySetSelect(SdChanNo, AMBA_SD_PHY_SEL0, SdDetailDelay.Bits.SelValue);
        AmbaRTSL_SDPhySetSelect(SdChanNo, AMBA_SD_PHY_SEL1, SdDetailDelay.Bits.SelValue);
        AmbaRTSL_SDPhySetSelect(SdChanNo, AMBA_SD_PHY_SEL2, SdDetailDelay.Bits.SelValue);
        AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_RESET, 0x1);
        AmbaDelayCycles(10000U);
        AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_RESET, 0x0);
        AmbaDelayCycles(10000U);
        AmbaRTSL_SDSetRdLatencyCtrl(SdChanNo, SdDetailDelay.Bits.RdLatency);

        return OK;
    }
    return NG;
}

INT32 AmbaRTSL_SetPhyDelay(UINT32 SdChanNo, UINT32 ClkBypass, UINT32 RxClkPol, UINT32 SbcCore, UINT32 SelValue, UINT32 DinClkPol, UINT32 CmdBypass)
{

    AmbaRTSL_SDPhyReset(SdChanNo);
    AmbaRTSL_SDPhySelReset(SdChanNo);
    AmbaRTSL_SDSetRdLatencyCtrl(SdChanNo, 0x0);

    AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_DIN_CLK_POL, DinClkPol);
    AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_RX_CLK_POL, RxClkPol);
    AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_CLK_OUT_BYPASS, ClkBypass);
    AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_DATA_CMD_BYPASS, CmdBypass);
    AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_DLL_BYPASS, 0x0);
    AmbaRTSL_SDPhySetSharedBusCtrl(SdChanNo, AMBA_SD_PHY_SBC_COARSE_DELAY, SbcCore);
    AmbaRTSL_SDPhySetSharedBusCtrl(SdChanNo, AMBA_SD_PHY_SBC_ENABLEDLL, 0x1);
    AmbaRTSL_SDPhySetSelect(SdChanNo, AMBA_SD_PHY_SEL0, SelValue);
    AmbaRTSL_SDPhySetSelect(SdChanNo, AMBA_SD_PHY_SEL1, SelValue);
    AmbaRTSL_SDPhySetSelect(SdChanNo, AMBA_SD_PHY_SEL2, SelValue);
    AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_RESET, 0x1U);
    AmbaDelayCycles(10000U);
    AmbaRTSL_SDPhySetConfig(SdChanNo, AMBA_SD_PHY_RESET, 0x0U);
    AmbaDelayCycles(10000U);
    AmbaRTSL_SDSetRdLatencyCtrl(SdChanNo, 1);

    return OK;
}
#endif
/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD0
 *
 *  @Description:: Send CMD0 ((bc): Resets all cards to idle state)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD0(UINT32 SdChanNo, UINT32 Cmd0Arg)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx = AMBA_SD_CMD0_GO_IDEL_STATE;
        pCmdConfig->ResponseType = AMBA_SD_NO_RESPONSE;
        pCmdConfig->Argument = Cmd0Arg;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD1
 *
 *  @Description:: Send CMD1 (bcr): Asks the card, in idlestate, to send its Operat-ing Conditions Register contents in the response on the CMD line
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD1(UINT32 SdChanNo, const AMBA_SD_CMD1_ARGUMENT_s * pCmd1Arg, UINT32 * pOcr)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    UINT32 TempArg;
    INT32 RetVal = NG;

    AmbaMisra_TouchUnused(pOcr);

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {

        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_CMD1_SEND_OP_COND;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        AmbaMisra_TypeCast32(&TempArg, pCmd1Arg);
        pCmdConfig->Argument     = 0x40ff8000;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD2
 *
 *  @Description:: Send CMD2 ((bcr): Asks any card to send the CID numbers on the CMD line)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD2(UINT32 SdChanNo, AMBA_SD_INFO_CID_REG_s * pCid)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    AmbaMisra_TouchUnused(pCid);

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_CMD2_ALL_SEND_CID;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_136_BIT;
        pCmdConfig->Argument     = 0x0;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }

    return RetVal;
}

INT32 AmbaRTSL_SdSendCMD2Done(UINT32 SdChanNo, AMBA_SD_INFO_CID_REG_s * pCid)
{
    AMBA_SD_RESPONSE_R2_s Response = {0};
    INT32 RetVal = -1;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        AmbaRTSL_SdGetResponse136Bits(SdChanNo, &Response);

        pCid->ManufacturerID     = (UINT8) AmbaRTSL_SdParseResponse(&Response, 120U,  8U);
        pCid->OemID              = (UINT16) AmbaRTSL_SdParseResponse(&Response, 104U, 16U);
        pCid->ProductName[0]     = (UINT8) AmbaRTSL_SdParseResponse(&Response,  96U,  8U);
        pCid->ProductName[1]     = (UINT8) AmbaRTSL_SdParseResponse(&Response,  88U,  8U);
        pCid->ProductName[2]     = (UINT8) AmbaRTSL_SdParseResponse(&Response,  80U,  8U);
        pCid->ProductName[3]     = (UINT8) AmbaRTSL_SdParseResponse(&Response,  72U,  8U);
        pCid->ProductName[4]     = (UINT8) AmbaRTSL_SdParseResponse(&Response,  64U,  8U);
        pCid->ProductRevision    = (UINT8) AmbaRTSL_SdParseResponse(&Response,  56U,  8U);
        pCid->ProductSerialNo    = AmbaRTSL_SdParseResponse(&Response,  24U, 32U);
        pCid->ManufacturingYear  = (UINT8) AmbaRTSL_SdParseResponse(&Response,  12U,  8U);
        pCid->ManufacturingMonth = (UINT8) AmbaRTSL_SdParseResponse(&Response,   8U,  4U);

        RetVal = 0;
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD3
 *
 *  @Description:: Send CMD3 ((bcr): Asks the card to publish a new relative address (RCA)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD3(UINT32 SdChanNo)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_CMD3_SEND_RELATIVE_ADDR;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->Argument     = 0x0;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD6
 *
 *  @Description:: Send CMD6 ((adtc): Checks switchable function (mode 0) and switch card function (mode 1))
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD6(UINT32 SdChanNo, const AMBA_SD_CMD6_ARGUMENT_s * pArgCmd6, AMBA_SD_CMD6_STATUS_s * pCmd6Status)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    UINT32 TempArg;
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx        = AMBA_SD_CMD6_SWITCH_FUNC;
        pCmdConfig->ResponseType  = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        AmbaMisra_TypeCast32(&TempArg, pArgCmd6);
        pCmdConfig->Argument      = TempArg;
        pCmdConfig->BlkSize       = 64;
        pCmdConfig->NumBlk        = 1;
        pCmdConfig->pDataBuf      = pCmd6Status;
        pCmdConfig->DataDirection = AMBA_SD_READ_DATA;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendEmmcCMD6
 *
 *  @Description:: Switches the mode of operation of the selected card or modifies the EXT_CSD registers (CMD6).
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendEmmcCMD6(UINT32 SdChanNo, const AMBA_SD_EMMC_CMD6_ARGUMENT_s * pArgCmd6)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;
    UINT32 Arg;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx        = AMBA_SD_CMD6_SWITCH_FUNC;
        AmbaMisra_TypeCast32(&Arg, pArgCmd6);
        pCmdConfig->Argument      = Arg;
        pCmdConfig->ResponseType  = AMBA_SD_RESPONSE_LENGTH_48_BIT_CHECK_BUSY;;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD7
 *
 *  @Description:: Send CMD7 ((ac): toggles a card between the stand-by and transfer states or programming and disconnect states)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD7(UINT32 SdChanNo, UINT16 Rca)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    AMBA_SD_RCA_ARGUMENT_s ArgRca;
    UINT32 Arg;
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        ArgRca.RelativeCardAddr = Rca;
        AmbaRTSL_SdCtrl[SdChanNo].Rca = Rca;

        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT_CHECK_BUSY;
        AmbaMisra_TypeCast32(&Arg, &ArgRca);
        pCmdConfig->Argument     = Arg;
        pCmdConfig->CmdIdx       = AMBA_SD_CMD7_SELECT_DESELECT_CARD;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD8
 *
 *  @Description:: Send CMD8 ((bcr): Sends SD Memory Card interface condition)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD8(UINT32 SdChanNo, const AMBA_SD_CMD8_ARGUMENT_s * pCmd8Arg)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    UINT32 TempArg;
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_CMD8_SEND_IF_COND;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        AmbaMisra_TypeCast32(&TempArg, pCmd8Arg);
        pCmdConfig->Argument     = TempArg;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendEmmcCMD8
 *
 *  @Description:: Send CMD8  ((adtc): Get the EXT_CSD)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *      pExtCsd:  pointer to the ext_csd buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendEmmcCMD8(UINT32 SdChanNo, AMBA_SD_INFO_EXT_CSD_REG_s * pExtCsd)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx = AMBA_SD_CMD8_SEND_EXT_CSD;

        pCmdConfig->ResponseType  = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->Argument      = 0x0;
        pCmdConfig->BlkSize       = (UINT32)(sizeof(AMBA_SD_INFO_EXT_CSD_REG_s) & 0xffffffffU);//512;
        pCmdConfig->NumBlk        = 1;
        pCmdConfig->pDataBuf      = (UINT8 *)pExtCsd;
        pCmdConfig->DataDirection = AMBA_SD_READ_DATA;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD9
 *
 *  @Description:: Send CMD9 ((ac): Addressed card sends its card-specific data (CSD) on the CMD line)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD9(UINT32 SdChanNo)
{
    AMBA_SD_RCA_ARGUMENT_s RCA = {0};
    UINT32 Arg;
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        RCA.RelativeCardAddr = AmbaRTSL_SdCtrl[SdChanNo].Rca;

        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_CMD9_SEND_CSD;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_136_BIT;
        AmbaMisra_TypeCast32(&Arg, &RCA);
        pCmdConfig->Argument     = Arg;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

INT32 AmbaRTSL_SdSendCMD9Done(UINT32 SdChanNo, AMBA_SD_INFO_CSD_REG_s * pCsd)
{
    AMBA_SD_RESPONSE_R2_s Cmd9Response;

    AmbaRTSL_SdGetResponse136Bits(SdChanNo, (AMBA_SD_RESPONSE_R2_s *)&Cmd9Response);

    pCsd->CsdStructure       = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response, 126U,  2U);
    pCsd->Taac               = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response, 112U,  8U);
    pCsd->Nsac               = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response, 104U,  8U);
    pCsd->TranSpeed          = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  96U,  8U);
    pCsd->CardCommandClass   = (UINT16) AmbaRTSL_SdParseResponse(&Cmd9Response,  84U, 12U);
    pCsd->ReadBlockLength    = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  80U,  4U);
    pCsd->ReadBlockPartial   = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  79U,  1U);
    pCsd->WriteBlockMisalign = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  78U,  1U);
    pCsd->ReadBlockMisalign  = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  77U,  1U);
    pCsd->DsrImplemented     = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  76U,  1U);

    if (pCsd->CsdStructure != 0U) {
        pCsd->CsdVersion.Version2.DeviceSize = AmbaRTSL_SdParseResponse(&Cmd9Response, 48U, 22U);
    } else {
        pCsd->CsdVersion.Version1.DeviceSize         = (UINT16) AmbaRTSL_SdParseResponse(&Cmd9Response,  62U, 12U);
        pCsd->CsdVersion.Version1.MaxReadCurrentMin  = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  59U, 3U);
        pCsd->CsdVersion.Version1.MaxReadCurrentMax  = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  56U, 3U);
        pCsd->CsdVersion.Version1.MaxWriteCurrentMin = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  53U, 3U);
        pCsd->CsdVersion.Version1.MaxWriteCurrentMax = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  50U, 3U);
        pCsd->CsdVersion.Version1.DeviceSizeMult     = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  47U, 3U);
    }

    pCsd->EraseBlockEnable   = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  46U,  1U);
    pCsd->SectorSize         = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  39U,  7U);

    pCsd->WriteGroupSize     = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  32U,  7U);
    pCsd->WirteGroupEnable   = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  31U,  1U);
    pCsd->WriteSpeedFactor   = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  26U,  3U);
    pCsd->WirteBlockLength   = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  22U,  4U);
    pCsd->WriteBlockPartial  = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  21U,  1U);
    pCsd->FileFormatGroup    = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  15U,  1U);
    pCsd->Copy               = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  14U,  1U);
    pCsd->PermWriteProtect   = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  13U,  1U);
    pCsd->TempWriteProtect   = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  12U,  1U);
    pCsd->FileFormat         = (UINT8) AmbaRTSL_SdParseResponse(&Cmd9Response,  10U,  2U);

    return 0;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD10
 *
 *  @Description:: Send CMD10 ((ac): Asks the selected card to send its card identification (CID)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD10(UINT32 SdChanNo)
{
    AMBA_SD_RCA_ARGUMENT_s RCA = {0};
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;
    UINT32 Arg;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        RCA.RelativeCardAddr = AmbaRTSL_SdCtrl[SdChanNo].Rca;

        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_CMD10_SEND_CID;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_136_BIT;
        AmbaMisra_TypeCast32(&Arg, &RCA);
        pCmdConfig->Argument     = Arg;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD11
 *
 *  @Description:: Send CMD11 ((acr): Switch to 1.8V bus signal level)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD11(UINT32 SdChanNo)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_CMD11_VOLTAGE_SWITCH;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->Argument     = 0x0;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}
/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD12
 *
 *  @Description:: Send CMD12 ((ac): Forces the card to stop transmission)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD12(UINT32 SdChanNo)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx = AMBA_SD_CMD12_STOP_TRANSMISSION;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->Argument = 0x0;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD13
 *
 *  @Description:: Send CMD13 ((ac): Addressed card sends its status register)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD13(UINT32 SdChanNo, UINT32 * pStatus)
{
    AMBA_SD_RCA_ARGUMENT_s RCA = {0};
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;
    UINT32 Arg;

    AmbaMisra_TouchUnused(pStatus);

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        RCA.RelativeCardAddr = AmbaRTSL_SdCtrl[SdChanNo].Rca;

        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_CMD13_SEND_STATUS;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        AmbaMisra_TypeCast32(&Arg, &RCA);
        pCmdConfig->Argument     = Arg;

        RetVal =  AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD16
 *
 *  @Description:: Send CMD16 ((ac): Set block length)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD16(UINT32 SdChanNo, UINT32 BlockLength)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_CMD16_SET_BLOCKLEN;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->Argument     = BlockLength;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD17
 *
 *  @Description:: Send CMD17  ((adtc): Read one block)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *      Sector:   start sector
 *      pBuf:     pointer to the data buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD17(UINT32 SdChanNo, UINT32 Sector, UINT8 * pBuf)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx = AMBA_SD_CMD17_READ_SINGLE_BLOCK;

        pCmdConfig->ResponseType  = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->Argument      = Sector;
        pCmdConfig->BlkSize       = 512;
        pCmdConfig->NumBlk        = 1;
        pCmdConfig->pDataBuf      = pBuf;
        pCmdConfig->DataDirection = AMBA_SD_READ_DATA;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return  RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD18
 *
 *  @Description:: Send CMD18 ((adtc): Read multiple blocks)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *      Sector:   start sector
 *      pBuf:     pointer to the data buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD18(UINT32 SdChanNo, UINT32 Sector, UINT32 Sectors, UINT8 * pBuf)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx        = AMBA_SD_CMD18_READ_MULTIPLE_BLOCK;
        pCmdConfig->ResponseType  = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->Argument      = Sector;
        pCmdConfig->BlkSize       = 512;
        pCmdConfig->NumBlk        = Sectors;
        pCmdConfig->pDataBuf      = pBuf;
        pCmdConfig->DataDirection = AMBA_SD_READ_DATA;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD19
 *
 *  @Description:: Send CMD19 ((adtc): 64 bytes tuning pattern is sent for SDR50 & SDR104)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD19(UINT32 SdChanNo, UINT8 * pBuf)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_CMD19_SEND_TUNING_BLOCK;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->Argument     = 0x0;
        pCmdConfig->BlkSize      = AMBA_SD_TUNING_BLOCK_BYTE_SIZE;
        pCmdConfig->NumBlk       = 1U;
        pCmdConfig->pDataBuf     = pBuf;
        pCmdConfig->DataDirection = AMBA_SD_READ_DATA;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD20
 *
 *  @Description:: Send CMD20 ((adtc): 64 bytes tuning pattern is sent for SDR50 & SDR104)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD20(UINT32 SdChanNo, UINT8 SpeedClassCtrl)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    AMBA_SD_CMD20_ARGUMENT_s Cmd20Arg = {0};
    INT32 RetVal = NG;

    AmbaMisra_TouchUnused(&SpeedClassCtrl);

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_CMD20_SPEED_CLASS_CONTROL;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->Argument     = 0x0;
        pCmdConfig->pDataBuf     = &Cmd20Arg;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendEmmcCMD21
 *
 *  @Description:: Send CMD21 ((adtc): 64 bytes or 128 Bytes tuning pattern is sent for HS200)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendEmmcCMD21(UINT32 SdChanNo, UINT8 * pBuf, UINT32 BlockLength)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_CMD21_SEND_TUNING_BLOCK;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->Argument     = 0x0;
        pCmdConfig->BlkSize = BlockLength;
        pCmdConfig->NumBlk       = 1;
        pCmdConfig->pDataBuf     = pBuf;
        pCmdConfig->DataDirection = AMBA_SD_READ_DATA;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD24
 *
 *  @Description:: Send CMD24 ((adtc): Write one block)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD24(UINT32 SdChanNo, UINT32 Sector, UINT8 * pBuf)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx        = AMBA_SD_CMD24_WRITE_SINGLE_BLOCK;
        pCmdConfig->ResponseType  = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->Argument      = Sector;
        pCmdConfig->BlkSize       = 512;
        pCmdConfig->NumBlk        = 1;
        pCmdConfig->pDataBuf      = pBuf;
        pCmdConfig->DataDirection = AMBA_SD_WRITE_DATA;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD25
 *
 *  @Description:: Send CMD25 ((adtc): Write multiple blocks
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD25(UINT32 SdChanNo, UINT32 Sector, UINT32 Sectors, UINT8 * pBuf)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx        = AMBA_SD_CMD25_WRITE_MULTIPLE_BLOCK;
        pCmdConfig->ResponseType  = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->Argument      = Sector;
        pCmdConfig->BlkSize       = 512;
        pCmdConfig->NumBlk        = Sectors;
        pCmdConfig->pDataBuf      = pBuf;
        pCmdConfig->DataDirection = AMBA_SD_WRITE_DATA;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD32
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
INT32 AmbaRTSL_SdSendCMD32(UINT32 SdChanNo, UINT32 StartSector)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_CMD32_ERASE_WR_BLOCK_START;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->Argument     = StartSector;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD33
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
INT32 AmbaRTSL_SdSendCMD33(UINT32 SdChanNo, UINT32 EndSector)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_CMD33_ERASE_WR_BLOCK_END;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->Argument     = EndSector;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD38
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
INT32 AmbaRTSL_SdSendCMD38(UINT32 SdChanNo)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_CMD38_ERASE;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT_CHECK_BUSY;
        pCmdConfig->Argument     = 0x0;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD55
 *
 *  @Description:: Send CMD55 ((ac): Indicates to the card that the next CMD is an application specific CMD)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD55(UINT32 SdChanNo)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    AMBA_SD_RCA_ARGUMENT_s Rca = {0};
    UINT32 Arg;
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        Rca.RelativeCardAddr = AmbaRTSL_SdCtrl[SdChanNo].Rca;

        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }

        pCmdConfig->CmdIdx       = AMBA_SD_CMD55_APP_CMD;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        AmbaMisra_TypeCast32(&Arg, &Rca);
        pCmdConfig->Argument     = Arg;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendACMD6
 *
 *  @Description:: Send ACMD6 ((ac): Defines the data bus width ('00'=1bit or '10'=4 bits bus) to be used for data transfer)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendACMD6(UINT32 SdChanNo, UINT8 BusWidth)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    AMBA_SD_ACMD6_ARGUMENT_s Acmd6Arg = {0};
    UINT32 Arg;
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        Acmd6Arg.BusWidth = BusWidth;

        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_ACMD6_SET_BUS_WIDTH;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        AmbaMisra_TypeCast32(&Arg, &Acmd6Arg);
        pCmdConfig->Argument     = Arg;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendACMD13
 *
 *  @Description:: Send ACMD13 ((adtc):  Sends the SD Status)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendACMD13(UINT32 SdChanNo, AMBA_SD_INFO_SSR_REG_s * pSSR)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx        = AMBA_SD_ACMD13_SD_STATUS;
        pCmdConfig->ResponseType  = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->BlkSize       = 64;
        pCmdConfig->NumBlk        = 1;
        pCmdConfig->pDataBuf      = pSSR;
        pCmdConfig->Argument      = 0x0;
        pCmdConfig->DataDirection = AMBA_SD_READ_DATA;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendACMD22
 *
 *  @Description:: Send ACMD22 ((adtc): Sends the number of the written (w/o errors) blocks)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendACMD22(UINT32 SdChanNo, UINT32 Blocks)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx = AMBA_SD_ACMD22_SEND_NUM_WR_BLOCKS;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->Argument = Blocks;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendACMD23
 *
 *  @Description:: Send ACMD23 ((adtc):Sets the number of write blocks to be pre-erased before writing)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendACMD23(UINT32 SdChanNo, UINT32 Blocks)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    AMBA_SD_ACMD23_ARGUMENT_s Acmd23Arg = {0};
    INT32 RetVal = NG;
    UINT32 Arg;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        Acmd23Arg.NumOfBlock = Blocks;

        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_ACMD23_SET_WR_BLK_ERASE_COUNT;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        AmbaMisra_TypeCast32(&Arg, &Acmd23Arg);
        pCmdConfig->Argument     = Arg;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendACMD41
 *
 *  @Description:: Send ACMD41 ((bcr): Sends host capacity support info (HCS) and asks the card to send OCR)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendACMD41(UINT32 SdChanNo, const AMBA_SD_ACMD41_ARGUMENT_s * pAcmd41Arg, UINT32 * pOcr)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    UINT32 TempArg;
    INT32 RetVal = NG;

    AmbaMisra_TouchUnused(pOcr);

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_ACMD41_SD_SEND_OP_COND;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        AmbaMisra_TypeCast32(&TempArg, pAcmd41Arg);
        pCmdConfig->Argument     = TempArg;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendACMD42
 *
 *  @Description:: Send ACMD42  ((ac):Connect[1]/Disconnect[0] the 50KOhm pull-up resistor on CD/DAT3 (pin 1) of the card)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendACMD42(UINT32 SdChanNo, UINT8 SetCardDetect)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    AMBA_SD_ACMD42_ARGUMENT_s Acmd42Arg = {0};
    UINT32 Arg;
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }

        if (SetCardDetect != 0U) {
            Acmd42Arg.SetCd = 1U;
        } else {
            Acmd42Arg.SetCd = 0U;
        }

        pCmdConfig->CmdIdx       = AMBA_SD_ACMD42_SET_CLR_CARD_DETECT;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        AmbaMisra_TypeCast32(&Arg, &Acmd42Arg);
        pCmdConfig->Argument     = Arg;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendACMD51
 *
 *  @Description:: Send ACMD51  ((ac):Connect[1]/Disconnect[0] the 50KOhm pull-up resistor on CD/DAT3 (pin 1) of the card)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendACMD51(UINT32 SdChanNo, AMBA_SD_INFO_SCR_REG_s * pScr)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_ACMD51_SEND_SCR;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->BlkSize      = 8U;
        pCmdConfig->NumBlk       = 1U;
        pCmdConfig->pDataBuf     = pScr;
        pCmdConfig->Argument     = 0x0;
        pCmdConfig->DataDirection = AMBA_SD_READ_DATA;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD5
 *
 *  @Description:: Send CMD5  ((SDIO): The function of CMD5 for SDIO cards is similar to the operation of ACMD41 for SD memory card)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD5(UINT32 SdChanNo, const AMBA_SDIO_CMD5_ARGUMENT_s * pSDIOcmd5Arg)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;
    UINT32 Arg;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx       = AMBA_SD_CMD5_IO_SEND_OP_COND;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        AmbaMisra_TypeCast32(&Arg, pSDIOcmd5Arg);
        pCmdConfig->Argument     = Arg;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD52
 *
 *  @Description:: Send CMD52  ((SDIO): I/O Reset, I/O about, or I/O Block Length, Bus_Width)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD52(UINT32 SdChanNo, const AMBA_SDIO_CMD52_ARGUMENT_s * pArgCmd52)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;
    UINT32 Arg;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx = AMBA_SD_CMD52_CCCR;
        pCmdConfig->ResponseType = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        AmbaMisra_TypeCast32(&Arg, pArgCmd52);
        pCmdConfig->Argument = Arg;

        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD53
 *
 *  @Description:: Send CMD53  ((SDIO): I/O block operations)
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdSendCMD53(UINT32 SdChanNo, const AMBA_SDIO_CMD53_ARGUMENT_s * pArgCmd53, UINT16 BlockSize, UINT8 * pDataBuf)
{
    AMBA_SD_CMD_CONFIG_s *pCmdConfig = &(AmbaRTSL_SdCtrl[SdChanNo].CmdConfig);
    INT32 RetVal = NG;
    UINT32 Arg;

    if (SdChanNo < AMBA_NUM_SD_CHANNEL) {
        if (OK != AmbaWrap_memset(pCmdConfig, 0, sizeof(AMBA_SD_CMD_CONFIG_s))) {
            /* For MisraC check */
        }
        pCmdConfig->CmdIdx        = AMBA_SD_CMD53_IO_BLK_OPERATION;
        pCmdConfig->ResponseType  = AMBA_SD_RESPONSE_LENGTH_48_BIT;
        pCmdConfig->BlkSize       = BlockSize;
        pCmdConfig->NumBlk        = (pArgCmd53->BlockMode != 0U) ? (UINT32) pArgCmd53->Count : 1U;
        pCmdConfig->pDataBuf      = pDataBuf;
        pCmdConfig->DataDirection = (pArgCmd53->ReadWriteFlag != 0U) ? AMBA_SD_WRITE_DATA : AMBA_SD_READ_DATA;
        AmbaMisra_TypeCast32(&Arg, pArgCmd53);
        pCmdConfig->Argument = Arg;
        RetVal = AmbaRTSL_SdSendCmd(SdChanNo);
    }
    return RetVal;
}

#if 0
/*
 *  @RoutineName:: AmbaRTSL_SdSendCMD5
 *
 *  @Description:: Send SDIO CMD5 ((bcr):
 *
 *  @Input      ::
 *      SdChanNo: SD Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_SdioSendCMD5(UINT32 SdChanNo)
{
    AmbaMisra_TouchUnused(&SdChanNo);

    return OK;
}
#endif

static inline void AmbaRTSL_SdHandleReadMisalign(UINT32 Misalign, UINT8 * pReadBuf, const UINT8 * pTmpBuf, UINT32 Sectors)
{
    if (Misalign != 0U) {
        (void)AmbaRTSL_CacheInvalDataPtr(pTmpBuf, Sectors * 512U);
        if (OK != AmbaWrap_memcpy(pReadBuf, pTmpBuf, (SIZE_t)((SIZE_t)Sectors * (SIZE_t)512U))) {
            /* For MisraC check */
        }
    }
}

INT32 AmbaRTSL_SdReadSector(UINT32 SdChanNo, const AMBA_NVM_SECTOR_CONFIG_s * pSecConfig)
{
    INT32 Rval = 0;
    UINT8 *pBuf    = (UINT8 *)pSecConfig->pDataBuf;
    UINT32 Sector  = pSecConfig->StartSector;
    UINT32 Sectors = pSecConfig->NumSector;
    UINT32 BufAddr, i;

    UINT32 Misalign;
    UINT32 MaxSectors = (AmbaRTSL_SdGetDescTransferSize() / 512U);

    AmbaMisra_TypeCast32(&BufAddr, &pBuf);
    Misalign   = (BufAddr & (UINT32)((CACHE_LINE_SIZE - 1U) & 0xffffffffU));

    if (Misalign != 0U) {
        MaxSectors = (AmbaRTSL_SdGetWorkBufferSize() / 512U);
        pBuf = &AmbaRTSL_SdWorkBuf[SdChanNo * AMBA_SD_WORK_BUFFER_SIZE];
    }

    if (Sectors == 1U) {
        Rval = AmbaRTSL_SdSendCMD17(SdChanNo, Sector, pBuf);
        if (Rval == 0) {
            Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, (AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE));
        }
        if (Rval == 0) {
            AmbaRTSL_SdHandleReadMisalign(Misalign, pSecConfig->pDataBuf, pBuf, 1U);
        }
    } else {
        UINT8 *pBufPtr;
        i = 0U;
        while(Sectors >= MaxSectors) {
            if (Misalign != 0U) {
                pBufPtr = &AmbaRTSL_SdWorkBuf[SdChanNo * AMBA_SD_WORK_BUFFER_SIZE];
            } else {
                pBufPtr = &pSecConfig->pDataBuf[i * MaxSectors * 512U];
            }
            Rval = AmbaRTSL_SdSendCMD18(SdChanNo, Sector, MaxSectors, pBufPtr);
            if (Rval == 0) {
                Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, (AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE));
            }

            if (Rval == 0) {
                AmbaRTSL_SdHandleReadMisalign(Misalign, &pSecConfig->pDataBuf[i * MaxSectors * 512U], pBufPtr, MaxSectors);
            } else {
                break;
            }

            i++;
            Sector += MaxSectors;
            Sectors -= MaxSectors;
        }

        if ((Sectors != 0U) && (Rval == 0)) {
            if (Misalign != 0U) {
                pBufPtr = &AmbaRTSL_SdWorkBuf[SdChanNo * AMBA_SD_WORK_BUFFER_SIZE];
            } else {
                pBufPtr = &pSecConfig->pDataBuf[i * MaxSectors * 512U];
            }
            if (Sectors == 1U) {
                Rval = AmbaRTSL_SdSendCMD17(SdChanNo, Sector, pBufPtr);
            } else {
                Rval = AmbaRTSL_SdSendCMD18(SdChanNo, Sector, Sectors, pBufPtr);
            }

            if (Rval == 0) {
                Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, (AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE));
            }

            if (Rval == 0) {
                AmbaRTSL_SdHandleReadMisalign(Misalign, &pSecConfig->pDataBuf[i * MaxSectors * 512U], pBufPtr, Sectors);
            }
        }
    }

    if (Misalign == 0U) {
        (void)AmbaRTSL_CacheInvalDataPtr(pBuf, pSecConfig->NumSector * 512U);
    }

    return Rval;
}

static inline void AmbaRTSL_SdHandleWriteMisalign(UINT32 Misalign, const UINT8 * pWriteBuf, UINT8 * pTmpBuf, UINT32 Sectors)
{
    if (Misalign != 0U) {
        UINT32 Rval = AmbaWrap_memcpy(pTmpBuf, pWriteBuf, (SIZE_t)((SIZE_t)Sectors * (SIZE_t)512U));
        if (Rval != OK) { /* For MisraC check */ }
    }
}

INT32 AmbaRTSL_SdWriteSector(UINT32 SdChanNo, const AMBA_NVM_SECTOR_CONFIG_s * pSecConfig)
{
    INT32 Rval = 0;
    UINT8 *pBuf    = (UINT8 *)pSecConfig->pDataBuf;
    UINT32 Sector  = pSecConfig->StartSector;
    UINT32 Sectors = pSecConfig->NumSector;

    UINT32 Misalign, i;
    UINT32 BufAddr;
    UINT32 MaxSectors = (AmbaRTSL_SdGetDescTransferSize() / 512U);  /* Number of Descriptor is pre-located */

    AmbaMisra_TypeCast32(&BufAddr, &pBuf);
    Misalign   = (BufAddr & (UINT32)((CACHE_LINE_SIZE - 1U) & 0xffffffffU));

    if (Misalign != 0U) {
        MaxSectors = (AmbaRTSL_SdGetWorkBufferSize() / 512U);
        pBuf = &AmbaRTSL_SdWorkBuf[SdChanNo * AMBA_SD_WORK_BUFFER_SIZE];
    }

    if (Sectors == 1U) {
        AmbaRTSL_SdHandleWriteMisalign(Misalign, pSecConfig->pDataBuf, pBuf, 1U);
        Rval = AmbaRTSL_SdSendCMD24(SdChanNo, Sector, pBuf);
        if (Rval == 0) {
            Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, (AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE));
        }
    } else {
        UINT8 *pBufPtr;
        /* Number of Descriptor is pre-located */
        //for (i = 0; Sectors > MaxSectors; i++, Sector += MaxSectors, Sectors -= MaxSectors) {
        i = 0U;
        while(Sectors > MaxSectors) {
            pBufPtr = &pSecConfig->pDataBuf[i * MaxSectors * 512U];
            AmbaRTSL_SdHandleWriteMisalign(Misalign, pBufPtr, pBuf, MaxSectors);
            Rval = AmbaRTSL_SdSendCMD25(SdChanNo, Sector, MaxSectors, (Misalign != 0U) ? pBuf : pBufPtr);
            if (Rval == 0) {
                Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, (AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE));
            }

            if (Rval != 0) {
                break;
            }
            i++;
            Sector += MaxSectors;
            Sectors -= MaxSectors;
        }

        if ((Sectors != 0U) && (Rval == 0)) {
            pBufPtr = &pSecConfig->pDataBuf[i * MaxSectors * 512U];
            AmbaRTSL_SdHandleWriteMisalign(Misalign, pBufPtr, pBuf, Sectors);
            if (Sectors == 1U) {
                Rval = AmbaRTSL_SdSendCMD24(SdChanNo, Sector, (Misalign != 0U) ? pBuf : pBufPtr);
            } else {
                Rval = AmbaRTSL_SdSendCMD25(SdChanNo, Sector, Sectors, (Misalign != 0U) ? pBuf : pBufPtr);
            }
            if (Rval == 0) {
                Rval = AmbaRTSL_SdWaitCmdDone(SdChanNo, (AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE));
            }
        }
    }

    return Rval;
}

INT32 AmbaRTSL_SdSetDriveStrengthAll(UINT32 SdChanNo, const AMBA_SD_SETTING_s * pSdSetting)
{
    INT32 RetVal = 0;
    UINT32 DataDriveStrength;
    AMBA_GPIO_INFO_s PinInfo;

    switch (SdChanNo) {
    case AMBA_SD_CHANNEL0:      /* for SD0 */
        /* Drive Strength for Clock */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_38_SD0_CLK, pSdSetting->ClockDrive);

        /* Drive Strength for Data */
        DataDriveStrength = pSdSetting->DataDrive;
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_46_SD0_DATA0, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_47_SD0_DATA1, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_48_SD0_DATA2, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_49_SD0_DATA3, DataDriveStrength);

        (void) AmbaRTSL_GpioGetPinInfo(GPIO_PIN_50_SD0_DATA4, &PinInfo);
        if(PinInfo.PinFunc == GPIO_FUNC_ALT2) {
            (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_50_SD0_DATA4, DataDriveStrength);
        }
        (void) AmbaRTSL_GpioGetPinInfo(GPIO_PIN_51_SD0_DATA5, &PinInfo);
        if(PinInfo.PinFunc == GPIO_FUNC_ALT2) {
            (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_51_SD0_DATA5, DataDriveStrength);
        }

        (void) AmbaRTSL_GpioGetPinInfo(GPIO_PIN_52_SD0_DATA6, &PinInfo);
        if(PinInfo.PinFunc == GPIO_FUNC_ALT2) {
            (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_52_SD0_DATA6, DataDriveStrength);
        }

        (void) AmbaRTSL_GpioGetPinInfo(GPIO_PIN_53_SD0_DATA7, &PinInfo);
        if(PinInfo.PinFunc == GPIO_FUNC_ALT2) {
            (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_53_SD0_DATA7, DataDriveStrength);
        }

        /* Drive Strength for CMD */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_39_SD0_CMD, pSdSetting->CMDDrive);

        /* Drive Strength for CD */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_40_SD0_CD,  pSdSetting->CDDrive);

        /* Drive Strength for WP */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_41_SD0_WP,  pSdSetting->WPDrive);
        break;

    case AMBA_SD_CHANNEL1:      /* for SD1 */
        /* Drive Strength for Clock */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_54_SD1_CLK, pSdSetting->ClockDrive);

        /* Drive Strength for Data */
        DataDriveStrength = pSdSetting->DataDrive;
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_56_SD1_DATA0, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_57_SD1_DATA1, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_58_SD1_DATA2, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_59_SD1_DATA3, DataDriveStrength);

        /* Drive Strength for CMD */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_55_SD1_CMD, pSdSetting->CMDDrive);

        /* Drive Strength for CD */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_60_SD1_CD,  pSdSetting->CDDrive);

        /* Drive Strength for WP */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_61_SD1_WP,  pSdSetting->WPDrive);
        break;

    case AMBA_SD_CHANNEL2:      /* for SD2 */
        /* Drive Strength for Clock */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_79_SD2_CLK, pSdSetting->ClockDrive);

        /* Drive Strength for Data */
        DataDriveStrength = pSdSetting->DataDrive;
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_75_SD2_DATA0, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_76_SD2_DATA1, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_77_SD2_DATA2, DataDriveStrength);
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_78_SD2_DATA3, DataDriveStrength);

        /* Drive Strength for CMD */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_80_SD2_CMD, pSdSetting->CMDDrive);

        /* Drive Strength for CD */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_81_SD2_CD,  pSdSetting->CDDrive);

        /* Drive Strength for WP */
        (void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_82_SD2_WP,  pSdSetting->WPDrive);
        break;

    default:
        RetVal = NG;
        break;
    }

    return RetVal;
}

