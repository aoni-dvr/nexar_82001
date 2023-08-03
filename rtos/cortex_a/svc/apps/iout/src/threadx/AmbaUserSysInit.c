/**
 *  @file AmbaUserSysInit.c
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
 *  @details Example for the implementation of system initialization
 *
 */

#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaKAL.h"
#include "AmbaSYS.h"
#include "AmbaFS.h"

#include "AmbaUtility.h"
#include "AmbaPrint.h"
#include "AmbaShell.h"
#include "AmbaShell_Commands.h"
//#include "AmbaRNG.h"

#include "AmbaIOUTDiag.h"
//#include "AmbaRTSL_PLL.h"
#include "AmbaGPIO.h"
#include "AmbaUART.h"
//#include "AmbaSPI.h"
//#include "AmbaDMA.h"
#include "AmbaSD.h"
#include "AmbaNAND.h"
#include "AmbaSPINAND.h"
#include "AmbaSPINOR.h"
#include "AmbaCache.h"
#include "AmbaDMA.h"
#include <AmbaSYS_Ctrl.h>
#include <AmbaMisraFix.h>

#if defined(CONFIG_ENABLE_AMBALINK)
#if defined(CONFIG_AMBALINK_BOOT_OS)
#include "AmbaLink.h"
#else
#include "AmbaIPC.h"
#endif
#endif

#include <AmbaTimerInfo.h>
#include <AmbaMemMap.h>
#include <AmbaCSL_RCT.h>

#ifdef  CONFIG_XEN_SUPPORT
#include <AmbaXen.h>
#define amba_xen_print(...)
#endif
#include "AmbaFusa.h"

#ifndef AMBA_UART_A53_CONSOLE
#define AMBA_UART_A53_CONSOLE       AMBA_UART_APB_CHANNEL0
#endif // TODO: set in all chip

#define TASK_PRI_IOUT_SYSINIT       (AMBA_KAL_TASK_LOWEST_PRIORITY - 5U)
#define TASK_PRI_IOUT_SD_MONITOR    (32U)
#define TASK_PRI_IOUT_SHELL_CLI     (AMBA_KAL_TASK_LOWEST_PRIORITY - 5U)
#define TASK_PRI_IOUT_SHELL_EXEC    (AMBA_KAL_TASK_LOWEST_PRIORITY - 6U)
#define TASK_PRI_IOUT_PRINT         (AMBA_KAL_TASK_LOWEST_PRIORITY)

#define PRINT_BUFFER_SIZE       (1024U*1024U)
#define PRINT_LOG_BUFFER_SIZE   (1024U*1024U)

#define EXEC_BUFFER_SIZE 0x40000U

#define IOUT_SVC_TIME_PREOS_CONFIG (0U)
#define AMBA_SYS_BOOT_CLK_MHz      (24U)

#ifdef CONFIG_ENABLE_AMBALINK
extern void AmbaShell_CommandAmbaLink(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc);
#endif
#ifdef CONFIG_BUILD_SSP_ENET
extern void AmbaShell_CommandEnet(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc);
#endif
//extern void AmbaMemProt_Config(UINT32 EnableFlag);

static IOUT_MEMORY_INFO_s noncache_mem_info[IOUT_MEMID_NUM] = {
    {
        IOUT_MEMID_PCIE, // ID
        64,              // AlignSize
        0x200000U,       // Size
        NULL
    },

    {
        IOUT_MEMID_USB, // ID
        64,             // AlignSize
        0x200000U,      // size = 2MB
        NULL
    },
};

static void get_pre_os_boot_time(void)
{
    ULONG              VirtAddr = (AMBA_DRAM_RESERVED_VIRT_ADDR + AMBA_RAM_APPLICATION_SPECIFIC_OFFSET);
    AMBA_TIMER_INFO_s  *pTimerInfo;

    AmbaMisra_TypeCast(&pTimerInfo, &VirtAddr);

    AmbaCSL_RctTimer1Freeze();
    pTimerInfo->A53_APP_Time[IOUT_SVC_TIME_PREOS_CONFIG] = AmbaCSL_RctTimer1GetCounter();
    AmbaCSL_RctTimer1Enable();
}

static void noncache_mem_init(void)
{
    UINT32 i;
    extern UINT32 __non_cache_heap_start;
    UINT8 *p_noncahce_ptr = (UINT8 *)&__non_cache_heap_start;
    UINT64 offset = 0;

    for (i = 0; i < IOUT_MEMID_NUM; i++) {
        IOUT_MEMORY_INFO_s *info = &noncache_mem_info[i];

        // process align
        if ((offset % info->AlignSize) != 0U) {
            offset = ((offset / info->AlignSize) + 1U) * info->AlignSize;
        }

        // check size
        info->Ptr = &p_noncahce_ptr[offset];
        offset += info->Size;

        //AmbaPrint_PrintUInt5("Noncache Mmeory[%d] ptr = %X, size %d", i, (UINT32)(UINT64)info->Ptr, (UINT32)info->Size, 0, 0);
    }
}

IOUT_MEMORY_INFO_s *AmbaIOUT_NoncacheMemInfoGet(UINT32 ID)
{
    IOUT_MEMORY_INFO_s *info = NULL;

    if (ID < IOUT_MEMID_NUM) {
        info = &noncache_mem_info[ID];
    }
    return info;
}


static UINT32 flag_alt_console = 0;
static UINT32 alt_console_ch = AMBA_UART_AHB_CHANNEL0;

static void console_write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut)
{
    if (flag_alt_console == 0U) {
        extern void AmbaUserConsole_Write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut);
        AmbaUserConsole_Write(StringSize, StringBuf, TimeOut);
    } else {
        UINT32 SentSize;
        UINT32 TxSize = StringSize;
        const UINT8 *pTxBuf;

        AmbaMisra_TypeCast32(&pTxBuf, &StringBuf);
        while (AmbaUART_Write(alt_console_ch, 0U, StringSize, pTxBuf, &SentSize, TimeOut) == UART_ERR_NONE) {
            TxSize -= SentSize;
            if (TxSize == 0U) {
                break;
            }
        }
    }
}

static UINT32 shell_read(UINT32 StringSize, char *StringBuf, UINT32 TimeOut)
{
    UINT32 uret;

    if (flag_alt_console == 0U) {
        extern UINT32 AmbaUserShell_Read(UINT32 StringSize, char *StringBuf, UINT32 TimeOut);
        uret = AmbaUserShell_Read(StringSize, StringBuf, TimeOut);
    } else {
        UINT32 RxSize;
        UINT8 *pRxBuf;

        (void)StringBuf;
        AmbaMisra_TypeCast32(&pRxBuf, &StringBuf);
        (void)AmbaUART_Read(alt_console_ch, 0U, StringSize, pRxBuf, &RxSize, TimeOut);
        uret = RxSize;
    }
    return uret;
}

static UINT32 shell_write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut)
{
    UINT32 uret;

    if (flag_alt_console == 0U) {
        extern UINT32 AmbaUserShell_Write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut);
        uret = AmbaUserShell_Write(StringSize, StringBuf, TimeOut);
    } else {
        UINT32 SentSize;
        const UINT8 *pTxBuf;

        AmbaMisra_TypeCast32(&pTxBuf, &StringBuf);
        (void)AmbaUART_Write(alt_console_ch, 0U, StringSize, pTxBuf, &SentSize, TimeOut);

        uret =  SentSize;
    }
    return uret;
}

void AmbaIOUT_ConsoleSwitch(UINT32 ConsoleIdx)
{
    flag_alt_console = ConsoleIdx;

#if defined(CONFIG_BSP_H32BUB_OPTION_A_V300) && defined(CONFIG_SVC_APPS_IOUT)
    if (flag_alt_console != 0U) {
#if 1
        static UINT8 AmbaSysInitUartRxRingBuf[4096U] __attribute__((section(".bss.noinit")));
        AMBA_UART_CONFIG_s UartConfig = {0};

        UartConfig.NumDataBits = AMBA_UART_DATA_8_BIT;              /* number of data bits */
        UartConfig.ParityBitMode = AMBA_UART_PARITY_NONE;           /* parity */
        UartConfig.NumStopBits = AMBA_UART_STOP_1_BIT;              /* number of stop bits */

        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_69_UART0_RXD);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_70_UART0_TXD);
        if (AmbaUART_Config(alt_console_ch, 115200U, &UartConfig) != 0U) {
            // action TBD
        }
        if (AmbaUART_HookDeferredRxBuf(alt_console_ch, 4096U, &(AmbaSysInitUartRxRingBuf[0])) != 0U) {
            // action TDB
        }

        if (AmbaUART_ClearDeferredRxBuf(alt_console_ch) != 0U) {
            // action TDB
        }
#endif
    }
#endif
}

static void AmbaSysInitConsole(void)
{
    static UINT8 AmbaSysInitPrintBuffer[PRINT_BUFFER_SIZE] __attribute__((section(".bss.noinit")));
    static UINT8 AmbaSysInitPrintLogBuffer[PRINT_LOG_BUFFER_SIZE] __attribute__((section(".bss.noinit")));
    static UINT8 AmbaSysInitPrintTaskStack[4096U] __attribute__((section(".bss.noinit")));
    static UINT8 AmbaSysInitUartRxRingBuf[4096U] __attribute__((section(".bss.noinit")));

    AMBA_PRINT_CONFIG_s PrintConfig = {
        .TaskPriority = TASK_PRI_IOUT_PRINT,
        .SmpCoreSet = 0x2U,
        .PrintBufferSize = PRINT_BUFFER_SIZE,
        .LogBufferSize = PRINT_LOG_BUFFER_SIZE,
        .pPrintBuffer = AmbaSysInitPrintBuffer,
        .pLogBuffer = AmbaSysInitPrintLogBuffer,
        .TaskStackSize = 4096U,
        .pTaskStack = AmbaSysInitPrintTaskStack,
        .PutCharFunc = console_write,
    };

    AMBA_UART_CONFIG_s UartConfig = {0};

    UartConfig.NumDataBits = AMBA_UART_DATA_8_BIT;              /* number of data bits */
    UartConfig.ParityBitMode = AMBA_UART_PARITY_NONE;           /* parity */
    UartConfig.NumStopBits = AMBA_UART_STOP_1_BIT;              /* number of stop bits */

    if (AmbaUART_Config(AMBA_UART_A53_CONSOLE, 115200U, &UartConfig) != 0U) {
        // action TBD
    }

    if (AmbaUART_HookDeferredRxBuf(AMBA_UART_A53_CONSOLE, 4096U, &(AmbaSysInitUartRxRingBuf[0])) != 0U) {
        // action TDB
    }

    if (AmbaUART_ClearDeferredRxBuf(AMBA_UART_A53_CONSOLE) != 0U) {
        // action TDB
    }

    if (AmbaPrint_Init(&PrintConfig) != 0U) {
        // action TDB
    }

    AmbaPrint_PrintUInt5("Print Service Ready!", 0U, 0U, 0U, 0U, 0U);
}

static void AmbaSysInitShell(void)
{

    static UINT8 AmbaSysInitCliTaskStack[4096U] __attribute__((section(".bss.noinit")));
    static UINT8 AmbaSysInitExecTaskStack[EXEC_BUFFER_SIZE] __attribute__((section(".bss.noinit")));

    static AMBA_SHELL_COMMAND_s Commands[] = {
        //    {.pName = "aucodec",  .MainFunc = AmbaShell_CommandAuCodec, .pNext = NULL},
        {.pName = "cat",      .MainFunc = AmbaShell_CommandCat, .pNext = NULL},
        {.pName = "cd",       .MainFunc = AmbaShell_CommandChangeDir, .pNext = NULL},
        {.pName = "cp",       .MainFunc = AmbaShell_CommandCopy, .pNext = NULL},
        {.pName = "date",     .MainFunc = AmbaShell_CommandDate, .pNext = NULL},
        //    {.pName = "dbgsym",   .MainFunc = AmbaShell_CommandDbgSym, .pNext = NULL},
        {.pName = "diag",     .MainFunc = AmbaIOUTDiag_CmdDiag, .pNext = NULL},
        {.pName = "format",   .MainFunc = AmbaShell_CommandFormat, .pNext = NULL},
        {.pName = "help",     .MainFunc = AmbaShell_CommandHelp, .pNext = NULL},
        {.pName = "ls",       .MainFunc = AmbaShell_CommandListDir, .pNext = NULL},
        //    {.pName = "memtest",  .MainFunc = AmbaShell_CommandMemTest, .pNext = NULL},
        {.pName = "mkdir",    .MainFunc = AmbaShell_CommandMakeDir, .pNext = NULL},
        {.pName = "mv",       .MainFunc = AmbaShell_CommandMove, .pNext = NULL},
        {.pName = "ps",       .MainFunc = AmbaShell_CommandProcessStatus, .pNext = NULL},
        {.pName = "randvrfy", .MainFunc = AmbaShell_CommandRandvrfy, .pNext = NULL},
        {.pName = "readl",    .MainFunc = AmbaShell_CommandReadLong, .pNext = NULL},
        {.pName = "reboot",   .MainFunc = AmbaShell_CommandReboot, .pNext = NULL},
        {.pName = "rm",       .MainFunc = AmbaShell_CommandRemove, .pNext = NULL},
        {.pName = "sdshmoo",  .MainFunc = AmbaShell_SdShmoo, .pNext = NULL},
        {.pName = "sleep",    .MainFunc = AmbaShell_CommandSleep, .pNext = NULL},
        {.pName = "sysinfo",  .MainFunc = AmbaShell_CommandSysInfo, .pNext = NULL},
        {.pName = "thruput",  .MainFunc = AmbaShell_CommandFsThruput, .pNext = NULL},
        {.pName = "ver",      .MainFunc = AmbaShell_CommandVersion, .pNext = NULL},
        {.pName = "vol",      .MainFunc = AmbaShell_CommandVolumn, .pNext = NULL},
        {.pName = "writel",   .MainFunc = AmbaShell_CommandWriteLong, .pNext = NULL},
        {.pName = "atf",      .MainFunc = AmbaShell_CommandAtf,       .pNext = NULL },
        {.pName = "iostat",   .MainFunc = AmbaShell_CommandIoStat,    .pNext = NULL},
        {.pName = "sysinfo",  .MainFunc = AmbaShell_CommandSysInfo,   .pNext = NULL},
#ifdef CONFIG_ENABLE_AMBALINK
        {.pName = "ambalink", .MainFunc = AmbaShell_CommandAmbaLink, .pNext = NULL},
#endif
#ifdef CONFIG_BUILD_SSP_ENET
        {.pName = "enet", .MainFunc = AmbaShell_CommandEnet, .pNext = NULL},
#endif
#if defined(CONFIG_SOC_CV2FS)
        {.pName = "fusa", .MainFunc = AmbaShell_CommandFuSa, .pNext = NULL},
#endif
    };

    UINT32 BASIC_COMMAND_NUMBER = sizeof(Commands)/sizeof(Commands[0]);

    UINT32 i;

    AMBA_SHELL_CONFIG_s ShellConfig = {
        .CliTaskPriority = TASK_PRI_IOUT_SHELL_CLI,
        .CliTaskSmpCoreSet = 1U,
        .pCliTaskStack = AmbaSysInitCliTaskStack,
        .CliTaskStackSize = 4096U,
        .ExecTaskPriority = TASK_PRI_IOUT_SHELL_EXEC,
        .ExecTaskSmpCoreSet = 2U,
        .pExecTaskStack = AmbaSysInitExecTaskStack,
        .ExecTaskStackSize = EXEC_BUFFER_SIZE,
        .GetCharFunc = shell_read,
        .PutCharFunc = shell_write,
    };

    if (AmbaShell_Init(&ShellConfig) != 0U) {
        AmbaPrint_PrintUInt5("Shell Service Init failed!", 0U, 0U, 0U, 0U, 0U);
    } else {
        for (i = 0; i < BASIC_COMMAND_NUMBER; i++) {
            if (AmbaShell_CommandRegister(&Commands[i]) != 0U) {
                break;
            }
        }
        AmbaPrint_PrintUInt5("Shell Service Ready!", 0U, 0U, 0U, 0U, 0U);
    }

    /* do rng init because we used __wrap_rand() to replace stdlib rand() */
    /*if (AmbaRNG_Init() != RNG_ERR_NONE) {
        AmbaPrint_PrintUInt5("Failed to start RNG!", 0U, 0U, 0U, 0U, 0U);
    }*/

#if defined(CONFIG_ENABLE_AMBALINK)
    (void) AmbaPrint_ModuleSetAllowList(AMBALINK_MODULE_ID, 1U);

    //ambalink init
    (void) AmbaIPC_Init();
    AmbaPrint_PrintStr5("AmbaIPC_Init() done", NULL, NULL, NULL, NULL, NULL);
#if defined(CONFIG_AMBALINK_BOOT_OS)
    AmbaLink_OS();
#if defined(CONFIG_OPENAMP)
    (void) AmbaLink_RpmsgInit(RPMSG_DEV_OAMP);
#elif !defined(CONFIG_AMBALINK_RPMSG_G2)
    (void) AmbaLink_RpmsgInit(RPMSG_DEV_AMBA);
#endif
#endif

    //ambalink defapp
#ifdef CONFIG_AMBALINK_RPMSG_G2
    {
        extern INT32 AmbaIPC_G2Echo(void);
        (void)AmbaIPC_G2Echo();
        AmbaPrint_PrintStr5("AmbaIPC_G2Echo() done", NULL, NULL, NULL, NULL, NULL);
    }
#endif
#if defined(CONFIG_AMBALINK_BOOT_OS)
    {
        extern void AmbaLink_StartBasicRpmsgApp(void);
        AmbaLink_StartBasicRpmsgApp();
        AmbaPrint_PrintStr5("AmbaLink_StartBasicRpmsgApp() done", NULL, NULL, NULL, NULL, NULL);
    }
#endif
#endif /*#if defined(CONFIG_ENABLE_AMBALINK)*/
}

void AmbaSysBootInfo(void)
{
    ULONG VirtAddr = (AMBA_DRAM_RESERVED_VIRT_ADDR + AMBA_RAM_APPLICATION_SPECIFIC_OFFSET);
    AMBA_TIMER_INFO_s *TimerInfo;
    UINT32 LoadTimeDiff, BootTime;

    AmbaMisra_TypeCast(&TimerInfo, &VirtAddr);

    // OS load time
    LoadTimeDiff = (TimerInfo->A53_BLD_Time[A53_BLD_LOAD_SYS_DONE] - TimerInfo->A53_BLD_Time[A53_BLD_LOAD_SYS_START]) / AMBA_SYS_BOOT_CLK_MHz; // us
    AmbaPrint_PrintUInt5("OS Load Time : %u.%03u ms", (LoadTimeDiff/1000U), (LoadTimeDiff%1000U), 0U, 0U, 0U);

    // OS boot time
    BootTime = TimerInfo->A53_APP_Time[IOUT_SVC_TIME_PREOS_CONFIG] / AMBA_SYS_BOOT_CLK_MHz; // us
    AmbaPrint_PrintUInt5("OS Boot Time : %u.%03u ms", (BootTime/1000U), (BootTime%1000U), 0U, 0U, 0U);
}

#if 0
static void AmbaShowAllClk(void)
{
    AmbaPrint_PrintUInt5("-----------------------------------", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_adc           = %10u Hz", AmbaRTSL_PllGetAdcClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_ahb           = %10u Hz", AmbaRTSL_PllGetAhbClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_apb           = %10u Hz", AmbaRTSL_PllGetApbClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_axi           = %10u Hz", AmbaRTSL_PllGetAxiClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_au            = %10u Hz", AmbaRTSL_PllGetAudioClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_core          = %10u Hz", AmbaRTSL_PllGetCoreClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_cortex        = %10u Hz", AmbaRTSL_PllGetCortexClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_dbg           = %10u Hz", AmbaRTSL_PllGetDbgClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_debounce      = %10u Hz", AmbaRTSL_PllGetDebounceClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_enet_clk_rmii = %10u Hz", AmbaRTSL_PllGetEthernetClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_gpio_debounce = %10u Hz", AmbaRTSL_PllGetGpioDebounceClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_idsp          = %10u Hz", AmbaRTSL_PllGetIdspClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_ir            = %10u Hz", AmbaRTSL_PllGetIrClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_motor         = %10u Hz", AmbaRTSL_PllGetMotorClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_nand          = %10u Hz", AmbaRTSL_PllGetNandClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_pwm           = %10u Hz", AmbaRTSL_PllGetPwmClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_sd48          = %10u Hz", AmbaRTSL_PllGetSd0Clk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_sdxc          = %10u Hz", AmbaRTSL_PllGetSd1Clk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_so            = %10u Hz", AmbaRTSL_PllGetSensor0Clk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_so2           = %10u Hz", AmbaRTSL_PllGetSensor1Clk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_so_vin        = %10u Hz", AmbaRTSL_PllGetVin0Clk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_so_pip        = %10u Hz", AmbaRTSL_PllGetVin1Clk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_ssi           = %10u Hz", AmbaRTSL_PllGetSpiMasterClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_ssi2          = %10u Hz", AmbaRTSL_PllGetSpiSlaveClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_ssi3          = %10u Hz", AmbaRTSL_PllGetSpiNorClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_uart_apb      = %10u Hz", AmbaRTSL_PllGetUartApbClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_uart0         = %10u Hz", AmbaRTSL_PllGetUart0Clk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_uart1         = %10u Hz", AmbaRTSL_PllGetUart1Clk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_uart2         = %10u Hz", AmbaRTSL_PllGetUart2Clk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_uart3         = %10u Hz", AmbaRTSL_PllGetUart3Clk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_usb_phy       = %10u Hz", AmbaRTSL_PllGetUsbPhyClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("gclk_vision        = %10u Hz", AmbaRTSL_PllGetVisionClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("pclk_vout_a        = %10u Hz", AmbaRTSL_PllGetVoutLcdClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("pclk_vout_b        = %10u Hz", AmbaRTSL_PllGetVoutTvClk(), 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("-----------------------------------", 0U, 0U, 0U, 0U, 0U);
}
#endif

#if defined(CONFIG_ENABLE_SPINOR_BOOT)
static void AmbaSysInitSpinor(void)
{
    void AmbaRTSL_AllocateNorSpiDMAChan(UINT32, UINT32);
    UINT32 DmaTxChan, DmaRxChan;
    extern AMBA_NORSPI_DEV_INFO_s AmbaNORSPI_DevInfo;
    extern AMBA_PARTITION_CONFIG_s AmbaNORSPI_SysPartConfig[];
    extern AMBA_PARTITION_CONFIG_s AmbaNORSPI_UserPartConfig[];
    extern AMBA_SERIAL_SPI_CONFIG_s AmbaNOR_SPISetting;

    AMBA_NOR_SPI_CONFIG_s NorSpiConfig;

    (void)AmbaWrap_memset(&NorSpiConfig, 0, sizeof(AMBA_NOR_SPI_CONFIG_s));
    NorSpiConfig.pNorSpiDevInfo  = &AmbaNORSPI_DevInfo;
    NorSpiConfig.pSysPartConfig  = AmbaNORSPI_SysPartConfig;
    NorSpiConfig.pUserPartConfig = AmbaNORSPI_UserPartConfig;
    NorSpiConfig.SpiSetting      = &AmbaNOR_SPISetting;

    AmbaDMA_ChannelAllocate(AMBA_DMA_CHANNEL_NOR_SPI_TX, &DmaTxChan);
    AmbaDMA_ChannelAllocate(AMBA_DMA_CHANNEL_NOR_SPI_RX, &DmaRxChan);

    AmbaRTSL_AllocateNorSpiDMAChan(DmaRxChan, DmaTxChan);

    if (AmbaSpiNOR_Config(&NorSpiConfig) != OK) {
        //AmbaPrint_PrintUInt5("\r\nWARNING: Incorrect SpiNOR device info!!\r\n", 0, 0, 0, 0, 0);
    }
}
#endif
#if defined(CONFIG_ENABLE_NAND_BOOT)
static void AmbaSysInitNand(void)
{
    extern AMBA_NAND_DEV_INFO_s AmbaNAND_DevInfo;
    extern AMBA_PARTITION_CONFIG_s AmbaNAND_SysPartConfig[AMBA_NUM_SYS_PARTITION];
    extern AMBA_PARTITION_CONFIG_s AmbaNAND_UserPartConfig[AMBA_NUM_USER_PARTITION];
    extern UINT32 AmbaNAND_FtlInit(UINT32 UserPartID);

    AMBA_NAND_CONFIG_s NandConfig = {
        .pNandDevInfo    =  &AmbaNAND_DevInfo,
        .pSysPartConfig  =  &(AmbaNAND_SysPartConfig[0]),   /* pointer to System partition configurations */
        .pUserPartConfig =  &(AmbaNAND_UserPartConfig[0]),  /* pointer to partition configurations */
    };

    /* NAND software configurations */
    if (AmbaNAND_Config(&NandConfig) != OK) {
        // action TBD
    }
    if (AmbaNAND_FtlInit(AMBA_USER_PARTITION_FAT_DRIVE_A) != 0U) {
        // action TBD
    }
    if (AmbaNAND_FtlInit(AMBA_USER_PARTITION_FAT_DRIVE_B) != 0U) {
        // action TBD
    }

    AmbaPrint_PrintUInt5("SpiNAND Service Ready!", 0U, 0U, 0U, 0U, 0U);
}
#endif
#if defined(CONFIG_ENABLE_SPINAND_BOOT)
static void AmbaSysInitNand(void)
{
    extern AMBA_SPINAND_DEV_INFO_s AmbaSpiNAND_DevInfo;
    extern AMBA_PARTITION_CONFIG_s AmbaNAND_SysPartConfig[AMBA_NUM_SYS_PARTITION];
    extern AMBA_PARTITION_CONFIG_s AmbaNAND_UserPartConfig[AMBA_NUM_USER_PARTITION];
    extern UINT32 AmbaNAND_FtlInit(UINT32 UserPartID);

    AMBA_SPINAND_CONFIG_s SpiNandConfig = {
        .pNandDevInfo    =  &AmbaSpiNAND_DevInfo,
        .pSysPartConfig  =  &(AmbaNAND_SysPartConfig[0]),   /* pointer to System partition configurations */
        .pUserPartConfig =  &(AmbaNAND_UserPartConfig[0]),  /* pointer to partition configurations */
    };

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    // GPIO ALT function should be configured by BSP.
    // Doing this is just for debug purpose.
    AmbaGPIO_SetFuncAlt(GPIO_PIN_51_NAND_SPI_CLK);
    AmbaGPIO_SetFuncAlt(GPIO_PIN_53_NAND_SPI_DATA0);
    AmbaGPIO_SetFuncAlt(GPIO_PIN_54_NAND_SPI_DATA1);
    AmbaGPIO_SetFuncAlt(GPIO_PIN_55_NAND_SPI_DATA2);
    AmbaGPIO_SetFuncAlt(GPIO_PIN_56_NAND_SPI_DATA3);
    AmbaGPIO_SetFuncAlt(GPIO_PIN_52_NAND_SPI_CS);
#endif

    /* NAND software configurations */
    if (AmbaSpiNAND_Config(&SpiNandConfig) != OK) {
        // action TBD
    }
    if (AmbaNAND_FtlInit(AMBA_USER_PARTITION_FAT_DRIVE_A) != 0U) {
        // action TBD
    }
    if (AmbaNAND_FtlInit(AMBA_USER_PARTITION_FAT_DRIVE_B) != 0U) {
        // action TBD
    }

    AmbaPrint_PrintUInt5("SpiNAND Service Ready!", 0U, 0U, 0U, 0U, 0U);
}
#endif

static UINT32 SD_SystemTaskCreateCb(SD_TASK_ENTRY_f TaskEntry, UINT32 Arg, AMBA_KAL_TASK_t **pTask)
{
    UINT32 RetVal;

    /*---------------------------------------------------------------------------*\
     * Create the task
    \*---------------------------------------------------------------------------*/
    static UINT8 SD_CardMonitorTaskStack[3 * 1024];
    static char card_monitor_task_name[]    = "AmbaSD_CardMonitorTask";
    static AMBA_KAL_TASK_t _AmbaSD_CardMonitorTask;

    RetVal = AmbaKAL_TaskCreate(&_AmbaSD_CardMonitorTask,                    /* pTask */
                                card_monitor_task_name,                      /* pTaskName */
                                TASK_PRI_IOUT_SD_MONITOR,                    /* Priority */
                                TaskEntry,                                   /* void (*EntryFunction)(UINT32) */
                                &Arg,                                        /* EntryArg */
                                SD_CardMonitorTaskStack,                     /* pStackBase */
                                sizeof(SD_CardMonitorTaskStack),             /* StackByteSize */
                                0x1);
    *pTask = &_AmbaSD_CardMonitorTask;

    return RetVal;
}

static void AmbaSysInitSD(void)
{
    extern void AmbaUserSD_PowerCtrl(UINT32 SdChanNo, UINT32 VddPower);
    extern void AmbaUserSD_PhyCtrl(UINT32 SdChanNo, AMBA_SD_INFO_CID_INFO_s *pCardID)  GNU_WEAK_SYMBOL;
    AMBA_SD_CONFIG_s SdConfig;
#if defined(CONFIG_ENABLE_EMMC_BOOT)
    extern const AMBA_PARTITION_CONFIG_s AmbaNAND_SysPartConfig[AMBA_NUM_SYS_PARTITION];
    extern const AMBA_PARTITION_CONFIG_s AmbaNAND_UserPartConfig[AMBA_NUM_USER_PARTITION];
#endif

    if (AmbaWrap_memset(&SdConfig, 0, sizeof(AMBA_SD_CONFIG_s)) != 0U) {
        // action TBD
    }
    SdConfig.PowerCtrl               = AmbaUserSD_PowerCtrl;
    SdConfig.PhyCtrl                 = AmbaUserSD_PhyCtrl;
    SdConfig.SdSetting.ClockDrive    = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.DataDrive     = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.CMDDrive      = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.WPDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.CDDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdConfig.SdSetting.HsRdLatency   = 0U;
    SdConfig.SdSetting.InitFrequency = 300000;
    SdConfig.SdSetting.MaxFrequency  = 100000000;
#if defined(CONFIG_SOC_CV2FS)
    SdConfig.SdSetting.DetailDelay   = 0x4401U;
#if defined(CONFIG_ENABLE_EMMC_BOOT)
    SdConfig.SdSetting.MaxFrequency  = 10000000U;
#else
    SdConfig.SdSetting.MaxFrequency  = 80000000U;
#endif
#elif defined(CONFIG_SOC_CV28)
    SdConfig.SdSetting.DetailDelay   = 0x0U;
#if defined(CONFIG_BSP_CV28BUB_OPTION_A_V100)
    SdConfig.SdSetting.MaxFrequency  = 40000000U;
#elif defined(CONFIG_BSP_CV28DK_8LAYER_A_V100)
    SdConfig.SdSetting.MaxFrequency  = 50000000U;
#else
    SdConfig.SdSetting.MaxFrequency  = 100000000U;
#endif
    SdConfig.SdSetting.DetailDelay   = 0x6541U;
    AmbaPrint_PrintUInt5("[CV28] SD DetailDelay[%d] MaxFrequency[%d]", SdConfig.SdSetting.DetailDelay, SdConfig.SdSetting.MaxFrequency, 0U, 0U, 0U);
#else
    SdConfig.SdSetting.DetailDelay   = 0x6541U;
    SdConfig.SdSetting.MaxFrequency  = 100000000U;
#endif

    SdConfig.SystemTaskCreateCb = SD_SystemTaskCreateCb;

    SdConfig.SmpCoreSet = 0x1;
    SdConfig.Priority   = 32;

#if defined(CONFIG_ENABLE_EMMC_BOOT)
    SdConfig.pSysPartConfig          = (AMBA_PARTITION_CONFIG_s *)AmbaNAND_SysPartConfig;
    SdConfig.pUserPartConfig         = (AMBA_PARTITION_CONFIG_s *)AmbaNAND_UserPartConfig;
#endif

#if defined (CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined (CONFIG_BSP_CV28DK_8LAYER_A_V100) || defined(CONFIG_BSP_CV2XFSDK_OPTION_B) || defined(CONFIG_BSP_H32DK_V100)
    (void)AmbaSD_Config(AMBA_SD_CHANNEL1, &SdConfig);
#else
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    SdConfig.SdSetting.MaxFrequency  = 200000000U;
#ifdef CONFIG_EMMC_MAX_CLOCK
    SdConfig.SdSetting.MaxFrequency  = CONFIG_EMMC_MAX_CLOCK;
#endif
    (void)AmbaSD_Config(AMBA_SD_CHANNEL0, &SdConfig);
    SdConfig.SdSetting.MaxFrequency  = 200000000U;
    (void)AmbaSD_Config(AMBA_SD_CHANNEL1, &SdConfig);

    SdConfig.SdSetting.MaxFrequency  = 100000000U;
    (void)AmbaSD_Config(AMBA_SD_CHANNEL2, &SdConfig);
#else
    (void)AmbaSD_Config(AMBA_SD_CHANNEL0, &SdConfig);
#if defined(CONFIG_BSP_CV28DK_OPTION_B_V100)
    (void)AmbaSD_Config(AMBA_SD_CHANNEL1, &SdConfig);
#endif
#endif
#endif

    AmbaPrint_PrintUInt5("SD Service Ready!", 0U, 0U, 0U, 0U, 0U);
}

static void AmbaSysInitFio(void)
{
#ifdef CONFIG_BOOT_SEQ_LINUX
#define AMBALINK_MODULE_ID               ((UINT16)(AMBALINK_ERR_BASE >> 16U))
    extern void AmbaLink_V2Init(void);
    (void) AmbaPrint_ModuleSetAllowList(AMBALINK_MODULE_ID, 1U);
    AmbaLink_V2Init();
#endif
#if defined(CONFIG_ENABLE_SPINOR_BOOT)
    AmbaSysInitSpinor();
#endif
#if defined(CONFIG_ENABLE_NAND_BOOT) || defined(CONFIG_ENABLE_SPINAND_BOOT)
    AmbaSysInitNand();
#endif
    AmbaSysInitSD();
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSysInitTaskEntry
 *
 *  @Description:: User defined System Initializations (before OS running)
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void *AmbaSysInitTaskEntry(void * EntryArg)
{
    AmbaMisra_TouchUnused(EntryArg);
    AmbaDiag_IntEnableProfiler();
    AmbaSysInitConsole();
    noncache_mem_init();
#ifdef CONFIG_XEN_SUPPORT
    if (AmbaXen_is_guest()) {
        amba_xen_print("FIXME: Skipping FS in AmbaSysInitTaskEntry()\n");
    } else {
#else
    {
#endif
        if (AmbaFS_Init() != 0U)
        {
            // action TBD
        }
        AmbaSysInitFio();
    }

    AmbaSysBootInfo();
    AmbaSysInitShell();

#if 0
    AmbaShowAllClk();
#endif
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    AmbaCortexA53FusaInit(FUSA_RUN_EXCEPTION |
                          FUSA_RUN_ISA       |
                          FUSA_RUN_CREG      |
                          FUSA_RUN_BP,
                          99);
#endif
    return NULL;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUserSysInitPreOS
 *
 *  @Description:: User defined System Initializations (before OS running)
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AmbaUserSysInitPreOS(void)
{
    extern const AMBA_GPIO_DEFAULT_s GpioPinGrpConfig;
    /* GPIO Initializations before OS running */
    if (AmbaGPIO_LoadDefaultRegVals(&GpioPinGrpConfig) != 0U) {
        // action TBD
    };

    get_pre_os_boot_time();

    //AmbaMemProt_Config(1U);
}
#ifdef CONFIG_KAL_THREADX_EVENT_TRACE
UCHAR my_trace_buffer[64000];
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUserSysInitPostOS
 *
 *  @Description:: User defined System Initializations (after OS running)
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AmbaUserSysInitPostOS(void)
{
    static AMBA_KAL_TASK_t AmbaSysInitTask;
    static UINT8 AmbaSysInitTaskStack[0x8000] __attribute__((section(".bss.noinit")));
    static char sys_init_task_name[] = "AmbaSysInit";

#ifdef CONFIG_KAL_THREADX_EVENT_TRACE
    extern UINT  _tx_trace_enable(VOID *trace_buffer_start, ULONG trace_buffer_size, ULONG registry_entries);
    (void)_tx_trace_enable(&my_trace_buffer, 64000, 30);
#endif

    if (AmbaKAL_TaskCreate(&AmbaSysInitTask,                        /* pTask */
                           sys_init_task_name,                      /* pTaskName */
                           TASK_PRI_IOUT_SYSINIT,                   /* Priority */
                           AmbaSysInitTaskEntry,                    /* void (*EntryFunction)(UINT32) */
                           NULL,                                    /* EntryArg */
                           AmbaSysInitTaskStack,                    /* pStackBase */
                           sizeof(AmbaSysInitTaskStack),            /* StackByteSize */
                           0U) != 0U)                               /* AutoStart */

    {
        /* should never happen ! */
    } else {
        if (AmbaKAL_TaskSetSmpAffinity(&AmbaSysInitTask, 0x1U) != 0U) {
            // action TBD
        }
        if (AmbaKAL_TaskResume(&AmbaSysInitTask) != 0U) {
            // action TBD
        }

        /* hook stack overflow handler */
        if (AmbaKAL_HookStkErrHandler(AmbaIOUT_StackOverflow) != 0U) {
            // action TBD
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUserIdleFunction
 *
 *  @Description:: User defined callback function during each idle cycle.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AmbaUserIdleFunction(void)
{
}

AMBA_SYS_USER_CALLBACKS_s  AmbaSysUserCallbacks = {
    .UserEntryBefOS = AmbaUserSysInitPreOS,
    .UserEntryAftOS = AmbaUserSysInitPostOS,
    .UserIdleCallback = AmbaUserIdleFunction,
    .UserExceptionHandler = AmbaIOUT_Backtrace
};

