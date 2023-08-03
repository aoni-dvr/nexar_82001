/**
 *  @file AmbaShell_SdShmoo.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details SPI Test APIs
 *
 */
#include <AmbaTypes.h>
#include "AmbaFS.h"
#include "AmbaWrap.h"

#include <AmbaKAL.h>
#include <AmbaShell.h>
#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"
#include "AmbaFS.h"

#include "AmbaSD.h"
#include "AmbaSD_Ctrl.h"

#include "AmbaRTSL_SD.h"

#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
/*
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/statvfs.h>
#include <sys/dcmd_cam.h>
#include "hw/dcmd_sim_mmcsd.h"
#include "hw/dcmd_sim_sdmmc.h"
#include <string.h>
*/
#define ShmooNG 500U
#define HUGE_BUFFER_SIZE    (0x4000U)
#define DATA_VERIFY_COUNT (1U)

extern int AmbaSD_DelayCtrlInit(void);
extern UINT32 AmbaSD_DevctlSetDelay(UINT32 SdChanNo, UINT32 DelayValue);

static void SHELL_SHMOOPrintUInt32(AMBA_SHELL_PRINT_f PrintFunc,
                                   const char *pFmtString,
                                   UINT32 Count,
                                   UINT32 Arg1,
                                   UINT32 Arg2,
                                   UINT32 Arg3,
                                   UINT32 Arg4,
                                   UINT32 Arg5)
{
    char StrBuf[256];
    UINT32 Arg[5];

    Arg[0] = Arg1;
    Arg[1] = Arg2;
    Arg[2] = Arg3;
    Arg[3] = Arg4;
    Arg[4] = Arg5;

    (void) AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, Count, Arg);
    PrintFunc(StrBuf);
}

static UINT32 Shell_SdShmooDelayCtrlCriteria(UINT32 SdChanNo, UINT8 Format, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = 0U;
    INT32 Result;
    static UINT8 pWriteBuf[HUGE_BUFFER_SIZE] __attribute__((aligned(AMBA_CACHE_LINE_SIZE)))  __attribute__((section(".bss.noinit")));
    static UINT8 pReadBuf[HUGE_BUFFER_SIZE] __attribute__((aligned(AMBA_CACHE_LINE_SIZE)))  __attribute__((section(".bss.noinit")));
    AMBA_SD_CARD_STATUS_s CardStatus;

    RetVal = AmbaSD_CardInit(SdChanNo);

    if (RetVal == 0U) {
        RetVal = AmbaSD_GetCardStatus(SdChanNo, &CardStatus);
    }

    if (RetVal == 0U) {
        if (CardStatus.CardSpeed <= 48000000U) {
            SHELL_SHMOOPrintUInt32(PrintFunc, "[SdShmoo] card speed : %u Hz\n\r", 1U, CardStatus.CardSpeed, 0U, 0U, 0U, 0U);
            RetVal = ShmooNG;
        }
    }

    for (UINT32 i = 0; i < HUGE_BUFFER_SIZE; i ++) {
        pWriteBuf[i] = rand() % 0xff;
    }

    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    for (UINT32 i = 0; i < DATA_VERIFY_COUNT; i ++) {
        if (RetVal == 0U) {
            SecConfig.NumSector = HUGE_BUFFER_SIZE / 512;
            SecConfig.pDataBuf = pWriteBuf;
            SecConfig.StartSector = i;

            RetVal = AmbaSD_WriteSector(0, &SecConfig);
        }

        if (RetVal == 0U) {
            SecConfig.NumSector = HUGE_BUFFER_SIZE / 512;
            SecConfig.pDataBuf = pReadBuf;
            SecConfig.StartSector = i;

            RetVal = AmbaSD_ReadSector(0, &SecConfig);
        }

        if (RetVal == 0U) {
            (void) AmbaWrap_memcmp(pWriteBuf, pReadBuf, HUGE_BUFFER_SIZE, &Result);
            if (0 != Result) {
                SHELL_SHMOOPrintUInt32(PrintFunc, "[SdShmoo] compare fail %d\n\n\r", 1U, Result, 0U, 0U, 0U, 0U);
                RetVal = ShmooNG;
            }
        }

        if (RetVal != 0U) {
            RetVal = ShmooNG;
            break;
        }
    }
    return RetVal;
}

static UINT32 DetailDelayRead(UINT32 SdChanNo)
{
    AMBA_SD_DETAIL_DELAY_s DetailDelay = {0};
    UINT32 DelayValue;

    DetailDelay.RdLatency = (UINT8) (AmbaRTSL_SDGetRdLatencyCtrl(SdChanNo) & 0x3U);
    DetailDelay.RXClkPol = AmbaRTSL_SDPhyGetConfig(SdChanNo, AMBA_SD_PHY_RX_CLK_POL);
    DetailDelay.ClkOutBypass = AmbaRTSL_SDPhyGetConfig(SdChanNo, AMBA_SD_PHY_CLK_OUT_BYPASS);
    DetailDelay.DataCmdBypass = AmbaRTSL_SDPhyGetConfig(SdChanNo, AMBA_SD_PHY_DATA_CMD_BYPASS);
    DetailDelay.SelValue = AmbaRTSL_SDPhyGetSelect(SdChanNo, AMBA_SD_PHY_SEL0);
    DetailDelay.SbcCoreDelay = (UINT8) AmbaRTSL_SDPhyGetSharedBusCtrl(SdChanNo, AMBA_SD_PHY_SBC_COARSE_DELAY);

    AmbaMisra_TypeCast32(&DelayValue, &DetailDelay);

    return DelayValue;
}
/*
UINT32 SdShmoo_SetDetailDelay(UINT32 SdChanNo, UINT32 DelayValue)
{
    UINT32 status = OK; // OK is defined as unsigned int
    int fd;
    AMBA_SD_SETTING_s SdSetting;

    SdSetting.DetailDelay = DelayValue;

    if (SdChanNo == AMBA_SD_CHANNEL0) {
        fd = open("/dev/hd0", O_RDWR);
    } else if (SdChanNo == AMBA_SD_CHANNEL1) {
        fd = open("/dev/hd1", O_RDWR);
    } else {
        fd = -1;
    }

    devctl(fd, DCMD_SDMMC_SET_CONFIG, &SdSetting, sizeof(AMBA_SD_SETTING_s), NULL);
    close(fd);

    return status;
}
*/
static void SdShmooRegTest(UINT32 SdChanNo, AMBA_SHELL_PRINT_f PrintFunc, UINT8 ClkBypass, UINT8 RxClkPol, UINT8 DinClkPol, UINT8 CmdBypass, UINT8 Format)
{
    UINT8 SbcCore, SelValue;
    UINT32 RetVal;
    INT32 ShmooRegResult[4][64] = {0};
    AMBA_SD_CARD_STATUS_s CardStatus;
    AMBA_SD_DETAIL_DELAY_s DetailDelay = {0};
    UINT32 DelayValue;

    for(SbcCore = 0; SbcCore < 4U; SbcCore++) {
        for(SelValue = 32U; SelValue < 64U; SelValue++) {
            DetailDelay.RdLatency = 1U;
            DetailDelay.RXClkPol = RxClkPol;
            DetailDelay.ClkOutBypass = ClkBypass;
            DetailDelay.DataCmdBypass = CmdBypass;
            DetailDelay.SelValue = SelValue;
            DetailDelay.SbcCoreDelay = SbcCore;
            AmbaMisra_TypeCast32(&DelayValue, &DetailDelay);
            //AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.DetailDelay = DelayValue;
            //AmbaSD_SetDetailDelay(DelayValue);
            AmbaSD_DevctlSetDelay(SdChanNo, DelayValue);
            DelayValue = 0U;

            RetVal = Shell_SdShmooDelayCtrlCriteria(SdChanNo, Format, PrintFunc);
            (void) AmbaSD_GetCardStatus(SdChanNo, &CardStatus);

            if(RetVal != 0U) {
                DelayValue = DetailDelayRead(SdChanNo);
                SHELL_SHMOOPrintUInt32(PrintFunc, "[failed], sc : %u, selvalue : %u , DelayValue : 0x%x\n", 3U, SbcCore, SelValue, DelayValue, 0, 0);
                ShmooRegResult[SbcCore][SelValue] = -1;
            } else {
                DelayValue = DetailDelayRead(SdChanNo);
                SHELL_SHMOOPrintUInt32(PrintFunc, "[pass], sc : %u, selvalue : %u, DelayValue : 0x%x, CardSpeed : %u\n", 4U, SbcCore, SelValue, DelayValue, CardStatus.CardSpeed, 0);
                ShmooRegResult[SbcCore][SelValue] = 1;
            }
        }
    }
    UINT32 Val = AmbaSD_GetHsRdLatency(0);
    SHELL_SHMOOPrintUInt32(PrintFunc, "---------[Pass Value] HsRdLatency : %u ClkBypass :%d RxClkPol:%d DinClkPol : %d----------\r\n", 4U, Val, ClkBypass, RxClkPol, DinClkPol, 0);
    for(SbcCore = 0U; SbcCore < 4U; SbcCore++) {
        SHELL_SHMOOPrintUInt32(PrintFunc, "[sc %u]\r\n", 1U, SbcCore, 0, 0, 0, 0);
        for(SelValue = 63U; SelValue > 31U; SelValue--) {
            if(ShmooRegResult[SbcCore][SelValue] == 1) {
                SHELL_SHMOOPrintUInt32(PrintFunc, "%2u(-%2u)  ", 2U, ((UINT32) SelValue), (((UINT32) SelValue) - 32U), 0, 0, 0);
            } else {
                PrintFunc("   X     ");
            }

            if((SelValue % 16U) == 0U) {
                PrintFunc("\r\n");
            }
        }
        PrintFunc("\r\n");
    }

}
#if 0
static void SdShmooRegSet(UINT32 SdChanNo, AMBA_SHELL_PRINT_f PrintFunc, UINT32 DealyValue)
{
    UINT32 RetVal;
    AMBA_SD_CARD_STATUS_s CardStatus;

    (void)AmbaWrap_memset(&CardStatus, 0, sizeof(AMBA_SD_CARD_STATUS_s));

    AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.DetailDelay = DealyValue;

    RetVal = AmbaFS_UnMount(('C' + SdChanNo));

    if (RetVal == 0U) {
        RetVal = AmbaSD_CardInit(SdChanNo);
    }

    if (RetVal == 0U) {
        RetVal = AmbaSD_GetCardStatus(SdChanNo, &CardStatus);
    }

    if (RetVal == 0U) {
        SHELL_SHMOOPrintUInt32(PrintFunc, "[SdShmoo] card speed : %u Hz\n\r", 1U, CardStatus.CardSpeed, 0U, 0U, 0U, 0U);
    }

    if (RetVal == 0U) {
        RetVal = AmbaFS_Mount(('C' + SdChanNo));
    }

    if (RetVal == 0U) {
        PrintFunc("SdShmooRegSet OK\r\n");
    } else {
        PrintFunc("SdShmooRegSet Fail\r\n");
    }
}
#endif
static void usage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" reg_test [sd0 | sd1] \n");

    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" reg_set [DelayValue] [sd0 | sd1]\n");

    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" maxclk_set [SdMaxClock] [sd0 | sd1]\n");

    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" show\n");

    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" reg_scan 0 0 [sd0 | sd1]\n");

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Test_SdShmoo
 *
 *  @Description:: The main function of sd shmoo test
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: OK(0) ERR(non-zero)
\*-----------------------------------------------------------------------------------------------*/
void AmbaShell_SdShmoo(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 SdChanNo;
    UINT32 DelayValue, MaxClkValue;
    UINT32 ClkBypassValue, RxClkPolValue;

    if (ArgCount >= 2U) {

        if ((AmbaUtility_StringCompare(pArgVector[ArgCount - 1U], "sd0", 3U) == 0)) {
            SdChanNo = AMBA_SD_CHANNEL0;
        } else if ((AmbaUtility_StringCompare(pArgVector[ArgCount - 1U], "sd1", 3U) == 0)) {
            SdChanNo = AMBA_SD_CHANNEL1;
        } else {
            SdChanNo = AMBA_SD_CHANNEL0;
        }

        AmbaSD_DelayCtrlInit();

        //if (AmbaSD_IsCardPresent(SdChanNo) != 0U) {
        if (AmbaUtility_StringCompare(pArgVector[1], "reg_test", 8U) == 0) {
            if (ArgCount == 2U) {
                SdShmooRegTest(SdChanNo, PrintFunc, 0U, 0U, 0U, 0U, 1U);
            } else if (ArgCount == 3U) {
                if ((AmbaUtility_StringCompare(pArgVector[2], "sd0", 3U) == 0) || (AmbaUtility_StringCompare(pArgVector[2], "sd1", 3U) == 0)) {
                    SdShmooRegTest(SdChanNo, PrintFunc, 0U, 0U, 0U, 0U, 1U);
                } else {
                    PrintFunc("AmbaShell_SdShmoo command fail\r\n");
                }
#if 0
            } else if (ArgCount == 4U) {
                (void)AmbaUtility_StringToUInt32(pArgVector[2], &ClkBypassValue);
                (void)AmbaUtility_StringToUInt32(pArgVector[3], &RxClkPolValue);
                SdShmooRegTest(SdChanNo, PrintFunc, (UINT8) ClkBypassValue, (UINT8) RxClkPolValue, 0U, 0U, 1U);
            } else if (ArgCount == 5U) {
                if ((AmbaUtility_StringCompare(pArgVector[4], "sd0", 3U) == 0) || (AmbaUtility_StringCompare(pArgVector[4], "sd1", 3U) == 0)) {
                    (void)AmbaUtility_StringToUInt32(pArgVector[2], &ClkBypassValue);
                    (void)AmbaUtility_StringToUInt32(pArgVector[3], &RxClkPolValue);
                    SdShmooRegTest(SdChanNo, PrintFunc, (UINT8) ClkBypassValue, (UINT8) RxClkPolValue, 0U, 0U, 1U);
                } else {
                    PrintFunc("AmbaShell_SdShmoo command fail\r\n");
                }
            } else {
                PrintFunc("AmbaShell_SdShmoo command fail\r\n");
#endif
            }
#if 0
        } else if (AmbaUtility_StringCompare(pArgVector[1], "reg_set", 7U) == 0) {
            if (ArgCount == 3U) {
                (void)AmbaUtility_StringToUInt32(pArgVector[2], &DelayValue);
                SdShmooRegSet(SdChanNo, PrintFunc, DelayValue);
            } else {
                PrintFunc("AmbaShell_SdShmoo command fail\r\n");
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "maxclk_set", 10U) == 0) {
            (void)AmbaUtility_StringToUInt32(pArgVector[2], &MaxClkValue);
            AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.MaxFrequency = MaxClkValue;
        } else if (AmbaUtility_StringCompare(pArgVector[1], "reg_scan", 8U) == 0) {
            if (ArgCount == 5U) {
                if ((AmbaUtility_StringCompare(pArgVector[4], "sd0", 3U) == 0) || (AmbaUtility_StringCompare(pArgVector[4], "sd1", 3U) == 0)) {
                    (void)AmbaUtility_StringToUInt32(pArgVector[2], &ClkBypassValue);
                    (void)AmbaUtility_StringToUInt32(pArgVector[3], &RxClkPolValue);
                    SdShmooRegTest(SdChanNo, PrintFunc, (UINT8) ClkBypassValue, (UINT8) RxClkPolValue, 0U, 0U, 0U);
                } else {
                    PrintFunc("AmbaShell_SdShmoo command fail\r\n");
                }
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "show", 4U) == 0) {
            (void)AmbaUtility_StringToUInt32(pArgVector[2], &MaxClkValue);
            SHELL_SHMOOPrintUInt32(PrintFunc, "Card speed : %u Hz\n\r", 1U, AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.MaxFrequency, 0U, 0U, 0U, 0U);
            SHELL_SHMOOPrintUInt32(PrintFunc, "Phy delay : 0x%x\n\r", 1U, DetailDelayRead(SdChanNo), 0U, 0U, 0U, 0U);
            SHELL_SHMOOPrintUInt32(PrintFunc, "HsRdLatency : 0x%x\n\r", 1U, AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.HsRdLatency, 0U, 0U, 0U, 0U);
            SHELL_SHMOOPrintUInt32(PrintFunc, "DataDrive : 0x%x\n\r", 1U, AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.DataDrive, 0U, 0U, 0U, 0U);
            SHELL_SHMOOPrintUInt32(PrintFunc, "Card MID : 0x%x\n\r", 1U, AmbaSD_Ctrl[SdChanNo].CardRegs.CardID.ManufacturerID, 0U, 0U, 0U, 0U);
            SHELL_SHMOOPrintUInt32(PrintFunc, "Card OID : 0x%x\n\r", 1U, AmbaSD_Ctrl[SdChanNo].CardRegs.CardID.OemID, 0U, 0U, 0U, 0U);
#endif
        } else {
            PrintFunc("AmbaShell_SdShmoo command fail\r\n");
        }


        //}
    } else {
        usage(pArgVector, PrintFunc);
    }
}
