#include "AmbaGPIO_Def.h"
#include <AmbaGPIO.h>
#include "AmbaUART_Def.h"
#include <AmbaPrint.h>
#include <AmbaUART.h>
#include <AmbaKAL.h>
#include <SvcTaskList.h>
//#include <AmbaDSP_ImageFilter.h>
//#include <AmbaSTU_StereoBarCalibInfo.h>
//#include <SvcWarp.h>
#include <AmbaSYS_Def.h>
#include <SvcBootDev.h>
#include <AmbaFS.h>
#include <AmbaSD_Priv.h>
#include <app_helper.h>
#include ".svc_autogen"
#include <platform.h>
#include <pmic.h>
#include <iCamSD.h>
#include <AmbaNAND_Def.h>
#include <AmbaNAND_FTL.h>

extern const AMBA_GPIO_DEFAULT_s GpioPinGrpConfig;
extern void AmbaUserConsole_Write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut);
extern int AmbaFwUpdaterMain(void);

static void SvcSysInitPreOS(void)
{
    AmbaGPIO_LoadDefaultRegVals(&GpioPinGrpConfig);
}

#define PRINT_BUF_SIZE          (0x100000U)
#define PRNLOG_BUF_SIZE         (0x100000U)
#define PRINT_STACK_SIZE        (16384U)
#define RX_BUFFER_SIZE          (1024U)
static void SvcSysInitPostOS(void)
{
    UINT32 Rval = 0;
    static UINT8  PrintBuffer[PRINT_BUF_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8  PrnLogBuffer[PRNLOG_BUF_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8  PrintTaskStack[PRINT_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 SvcUartRxBuf[RX_BUFFER_SIZE] GNU_SECTION_NOZEROINIT;
    AMBA_PRINT_CONFIG_s  PrintConfig = {0};

    AMBA_UART_CONFIG_s  UartConfig = {0};
    UartConfig.NumDataBits   = AMBA_UART_DATA_8_BIT;
    UartConfig.ParityBitMode = AMBA_UART_PARITY_NONE;
    UartConfig.NumStopBits   = AMBA_UART_STOP_1_BIT;
    Rval = AmbaUART_Config(AMBA_UART_APB_CHANNEL0, 115200U, &UartConfig);
    if (UART_ERR_NONE == Rval) {
        Rval = AmbaUART_HookDeferredRxBuf(AMBA_UART_APB_CHANNEL0, RX_BUFFER_SIZE, &(SvcUartRxBuf[0]));
        if (UART_ERR_NONE == Rval) {
            Rval = AmbaUART_ClearDeferredRxBuf(AMBA_UART_APB_CHANNEL0);
            if (UART_ERR_NONE != Rval) {
                debug_line("## Fail to clear rx buffer");
            }
        } else {
            debug_line("## Fail to hook rx buffer");
        }
    } else {
        debug_line("## Fail to uart config");
    }

    PrintConfig.TaskPriority    = SVC_PRINT_TASK_PRI;
    PrintConfig.SmpCoreSet      = SVC_PRINT_TASK_CPU_BITS;
    PrintConfig.PrintBufferSize = PRINT_BUF_SIZE;
    PrintConfig.pPrintBuffer    = PrintBuffer;
    PrintConfig.LogBufferSize   = PRNLOG_BUF_SIZE;
    PrintConfig.pLogBuffer      = PrnLogBuffer;
    PrintConfig.TaskStackSize   = PRINT_STACK_SIZE;
    PrintConfig.pTaskStack      = PrintTaskStack;
    PrintConfig.PutCharFunc     = AmbaUserConsole_Write;
    AmbaPrint_Init(&PrintConfig);

    Pmic_Init();
    AmbaFS_Init();
    /* init boot device */
    Rval = SvcBootDev_Init(NULL);
    if (SVC_OK != Rval) {
        debug_line("## boot_device isn't initialized");
    }

    if (NAND_ERR_NONE != AmbaNAND_FtlInit(AMBA_USER_PARTITION_FAT_DRIVE_A)) {
        debug_line("Drive A isn't enabled");
    }
    if (NAND_ERR_NONE != AmbaNAND_FtlInit(AMBA_USER_PARTITION_FAT_DRIVE_B)) {
        debug_line("Drive B isn't enabled");
    }
    iCamSD_Init();
    AmbaPrint_Flush();
    AmbaFwUpdaterMain();
}

static void SvcSysIdleFunc(void)
{
}

static void SvcExcept_Handler(UINT32 ExceptionID, ULONG * pSavedRegs)
{
    (void)ExceptionID;
    (void)(pSavedRegs);
}

AMBA_SYS_USER_CALLBACKS_s AmbaSysUserCallbacks = {
    .UserEntryBefOS       = SvcSysInitPreOS,
    .UserEntryAftOS       = SvcSysInitPostOS,
    .UserIdleCallback     = SvcSysIdleFunc,
    .UserExceptionHandler = SvcExcept_Handler
};

