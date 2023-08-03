/**
 *  @file AmbaBLD_UserCallBack.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details user callback API for BLD
 *
 */

// #include "AmbaBLD.h"
#include "AmbaBLD_UserCallBack.h"

/*-------------------------------------------------------------------*\
 *   user defined API region
\*-------------------------------------------------------------------*/

#include "AmbaRTSL_WDT.h"
#include "AmbaWrap.h"
#include "AmbaNVM_Partition.h"
#include "AmbaMisraFix.h"
#include "AmbaRTSL_UART.h"
#include "AmbaRTSL_PWC.h"
#include "AmbaIOUtility.h"

#if defined(CONFIG_ENABLE_NAND_BOOT) || defined(CONFIG_ENABLE_SPINAND_BOOT)
#include "AmbaRTSL_NAND.h"
extern AMBA_SYS_PARTITION_TABLE_s  AmbaRTSL_SysPartTable; //  = (AMBA_SYS_PARTITION_TABLE_s *)  (AMBA_CORTEX_A9_DRAM_RSVD_VIRT_BASE_ADDR + AMBA_RAM_NVM_SYS_PARTITION_TABLE_OFFSET);
extern AMBA_USER_PARTITION_TABLE_s AmbaRTSL_UserPartTable; // = (AMBA_USER_PARTITION_TABLE_s *) (AMBA_CORTEX_A9_DRAM_RSVD_VIRT_BASE_ADDR + AMBA_RAM_NVM_USER_PARTITION_TABLE_OFFSET);
AMBA_USER_PARTITION_TABLE_s*       pAmbaNvmTblUserPart = &AmbaRTSL_UserPartTable;
#define AmbaRTSL_NvmSetActivePtbNo AmbaRTSL_NandSetActivePtbNo
#define AmbaRTSL_NvmGetActivePtbNo AmbaRTSL_NandGetActivePtbNo
#define AmbaRTSL_NvmReadUserPTB    AmbaRTSL_NandReadUserPTB
#define AmbaRTSL_NvmWriteUserPTB   AmbaRTSL_NandWriteUserPTB
#elif defined(CONFIG_ENABLE_SPINOR_BOOT)
#include "AmbaRTSL_SpiNOR.h"
extern AMBA_SYS_PARTITION_TABLE_s  AmbaRTSL_NorSysPartTable;
extern AMBA_USER_PARTITION_TABLE_s AmbaRTSL_NorUserPartTable;
AMBA_USER_PARTITION_TABLE_s*       pAmbaNvmTblUserPart = &AmbaRTSL_NorUserPartTable;
#define AmbaRTSL_NvmSetActivePtbNo AmbaRTSL_NorSpiSetActivePtbNo
#define AmbaRTSL_NvmGetActivePtbNo AmbaRTSL_NorSpiGetActivePtbNo
#define AmbaRTSL_NvmReadUserPTB    AmbaRTSL_NorSpiReadUserPTB
#define AmbaRTSL_NvmWriteUserPTB   AmbaRTSL_NorSpiWriteUserPTB
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
#include "AmbaRTSL_SD.h"
extern AMBA_SYS_PARTITION_TABLE_s  AmbaRTSL_EmmcSysPartTable;
extern AMBA_USER_PARTITION_TABLE_s AmbaRTSL_EmmcUserPartTable;
AMBA_USER_PARTITION_TABLE_s*       pAmbaNvmTblUserPart = &AmbaRTSL_EmmcUserPartTable;
#define AmbaRTSL_NvmSetActivePtbNo AmbaRTSL_EmmcSetActivePtbNo
#define AmbaRTSL_NvmGetActivePtbNo AmbaRTSL_EmmcGetActivePtbNo
#define AmbaRTSL_NvmReadUserPTB    AmbaRTSL_EmmcReadUserPTB
#define AmbaRTSL_NvmWriteUserPTB   AmbaRTSL_EmmcWriteUserPTB
#else
#pragma message ("[AmbaBLD_UserCallBack.c] ERROR !! UNKNOWN BOOT DEVICE !!")
#endif

#if defined(CONFIG_SVC_ENABLE_WDT)
#define WDT_TIMEOUT        (5000U)
#define WDT_IRQ_PULSE_W    (500U)
#define WDT_MAGIC          (0xEEU)

#define WDT_SWITCH_ENA     (0U)
#define WDT_EXPIRE         (1U)

#define WDT_PRINT_BUF_SIZE (256U)
static void PrintUInt(const char *Fmt, const UINT32 Arg)
{
    static UINT8 BldPrintBuf[WDT_PRINT_BUF_SIZE] GNU_SECTION_NOZEROINIT;
    UINT32 ActualTxSize, UartTxSize, Index = 0U;
    char *pString;
    const UINT8 *pBuf = BldPrintBuf;
    const UINT32 *pArg = &Arg;

    AmbaMisra_TypeCast64(&pString, &pBuf);
    UartTxSize = IO_UtilityStringPrintUInt32(pString, WDT_PRINT_BUF_SIZE, Fmt, (UINT32)1U, pArg);
    while (UartTxSize > 0U) {
        if (AmbaRTSL_UartWrite(AMBA_UART_APB_CHANNEL0, UartTxSize, &BldPrintBuf[Index], &ActualTxSize) != UART_ERR_NONE) {
            break;
        } else {
            UartTxSize -= ActualTxSize;
            Index += ActualTxSize;
        }
    }
}

static void PrintStr(const char *pFmt)
{
    PrintUInt(pFmt, 0U);
}

static UINT32 GetReserve(UINT8 ID, UINT8* pRes)
{
    UINT32 Rval;
    UINT32 ActivePtb;

    Rval = AmbaRTSL_NvmGetActivePtbNo(&ActivePtb);
    if ((0U == Rval) && (pAmbaNvmTblUserPart != NULL) && (NULL != pRes)) {
        /* all WDT flag are saved in user PTB 0 table */
        Rval  = AmbaRTSL_NvmReadUserPTB(NULL, 0U);
        if (WDT_MAGIC == pAmbaNvmTblUserPart->Reserved[ID]) {
            *pRes = 1U;
        } else {
            *pRes = 0U;
        }
        /* read back the prigin user ptb table */
        Rval |= AmbaRTSL_NvmReadUserPTB(NULL, ActivePtb);

    } else {
        PrintStr("ERROR BLD UserCB GetReserve failed\r\n");
        *pRes = 0U;
    }

    return Rval;
}

UINT32 SetReserve(UINT32 ID, UINT32 Ena)
{
    UINT32 Rval;
    UINT32 ActivePtb;

    /* WDT enable flag was saved in User PTB 0 table, reserved[0], in UINT8 format. */
    Rval  = AmbaRTSL_NvmGetActivePtbNo(&ActivePtb);
    Rval |= AmbaRTSL_NvmReadUserPTB(NULL, 0U);
    if ((0U == Rval) && (pAmbaNvmTblUserPart != NULL)) {
        if (0U == Ena) {
            pAmbaNvmTblUserPart->Reserved[ID] = 0U;
        } else {
            pAmbaNvmTblUserPart->Reserved[ID] = WDT_MAGIC;
        }
        Rval = AmbaRTSL_NvmWriteUserPTB(NULL, 0U);
        if (0U != Rval) {
            PrintStr("SetReserve: AmbaRTSL_NvmWriteUserPTB failed");
        }
        Rval = AmbaRTSL_NvmReadUserPTB(NULL, ActivePtb);
        if (0U != Rval) {
            PrintStr("SetReserve: AmbaRTSL_NvmReadUserPTB 2nd failed");
        }
    } else {
        PrintStr("SetReserve: AmbaRTSL_NvmReadUserPTB 1st failed");
    }

    return Rval;
}

static void SwitchActivePtb(UINT32 ID)
{
    UINT32 NewPtb;
    if (ID == 0U) {
        NewPtb = 0U;
        (void) AmbaRTSL_NvmSetActivePtbNo(1U, 0U);
        (void) AmbaRTSL_NvmSetActivePtbNo(0U, 1U);
    } else {
        NewPtb = 1U;
        (void) AmbaRTSL_NvmSetActivePtbNo(0U, 0U);
        (void) AmbaRTSL_NvmSetActivePtbNo(1U, 1U);
    }

    /* reload user PTB */
    if (AmbaRTSL_NvmReadUserPTB(NULL, NewPtb) != 0U) {
        PrintStr("# Read User PTB Fail in WdtCheck\r\n");
    }
}

static void WdtCheck(void)
{
    UINT32 Rval = 0U;
    UINT8  flg_Switch = 0U;
    UINT8  flg_Expire = 0U;
    
    #if 0
    Rval |= GetReserve(WDT_EXPIRE, &flg_Expire);
    #else
    flg_Expire = AmbaRTSL_WdtGetTimeOutStatus();
    #endif

    Rval |= GetReserve(WDT_SWITCH_ENA, &flg_Switch);
    PrintUInt("\n\r# Watchdog enabled, switch flag %u, ", (UINT32)(0x1U & flg_Switch));
    PrintUInt("expire flag %u\n\r",                       (UINT32)(0x1U & flg_Expire));

    /* if WDT expired flag is activated,
       which means the previous boot was failed, try to reboot with other PTB */
    if ((0U != flg_Expire) && (0U == Rval)) {
        PrintStr("# Watchdog expire flag detected #\r\n");
        /* clear WDT flag */
        AmbaRTSL_WdtClearTimeOutStatus();

        if (0U != flg_Switch) {
            UINT32 CurrentPtb;
            UINT32 RestartPtb = 0U;

            Rval = AmbaRTSL_NvmGetActivePtbNo(&CurrentPtb);
            if (0U == Rval) {
                if (CurrentPtb == 0U) {
                    RestartPtb = 1U;
                } else if (CurrentPtb == 1U) {
                    RestartPtb = 0U;
                } else {
                    PrintUInt("ERROR : get invalid current active USER PTB %u\r\n#\r\n", CurrentPtb);
                }
                PrintUInt("# reset USER PTB to %u\r\n#\r\n", RestartPtb);
            } else {
                PrintStr("ERROR : failed to get current active PTB, reset USER PTB to 0\r\n#\r\n");
            }

            SwitchActivePtb(RestartPtb);
        }
    }
    
    SetReserve(WDT_EXPIRE, 1U);
}

static void WdtEnable(void)
{
    UINT32 Rval;

    /* start WDT */
    AmbaRTSL_Wdt_Init();
    Rval = AmbaRTSL_WdtStart(WDT_TIMEOUT, WDT_IRQ_PULSE_W);
    if (0U != Rval) {
        PrintStr("# AmbaRTSL_WdtStart failed\r\n#\r\n");
    } else {
        UINT32 act = AmbaRTSL_WdtGetTimeOutAction();
        PrintUInt("# Watchdog start success, expire action %u, ", act);
        PrintUInt("timeout after %u ms\r\n", WDT_TIMEOUT);

        /*  try to feed watchdog, if failed, stop it. */
        Rval = AmbaRTSL_WdtFeed();
        if (0U != Rval) {
            PrintStr("# AmbaRTSL_WdtFeed failed, try to stop WDT\r\n#\r\n");
            (void) AmbaRTSL_WdtStop();
        } else {
            PrintStr("# Watchdog feed  success\r\n#\r\n");
        }
    }
}
#endif

/*-------------------------------------------------------------------*\
 *   callback API
\*-------------------------------------------------------------------*/

static void BldUserCB_BefLoadFw(void)
{
    #if defined(CONFIG_SVC_ENABLE_WDT)
    WdtCheck();
    #endif
}


static void BldUserCB_AftLoadFw(void)
{
}

static void BldUserCB_BefEnterATF(void)
{
    #if defined(CONFIG_SVC_ENABLE_WDT)
    WdtEnable();
    #endif
}

AMBA_BLD_USER_CALLBACKS_s AmbaBldUserCallbacks = {
    .UserCbBefLoadFw   = BldUserCB_BefLoadFw,
    .UserCbAftLoadFw   = BldUserCB_AftLoadFw,
    .UserCbBefEnterATF = BldUserCB_BefEnterATF,
};
