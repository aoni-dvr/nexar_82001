/**
 *  @file AmbaIOUTDiag_CmdUART.c
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
 *  @details UART diagnostic command
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaShell.h"
#include "AmbaUtility.h"

#include "AmbaIOUTDiag.h"
#include "AmbaUART.h"

//#define UART_DMA_TEST_WITH_FILE_TRANSFER 1

#if defined(UART_DMA_TEST_WITH_FILE_TRANSFER)
#include <AmbaMisraFix.h>
#include "AmbaWrap.h"
#include "AmbaFS.h"

typedef struct {
    AMBA_KAL_EVENT_FLAG_t   EventFlag;              /* Event Flags */
    AMBA_KAL_MUTEX_t        Mutex;                  /* Mutex */
    UINT32 ReadPtr;
    UINT32 WritePtr;
    UINT32 ReceiveChan;
    UINT32 SendChan;
} DIAG_UART_CTRL_s;

#define UART_DATA_READY_EVENT (0x00000001U)
#define UART_DATA_END_EVENT (0x00000002U)
#define SIZE_UART_BUFFER (81920U)
static DIAG_UART_CTRL_s DiagUartCtrl GNU_SECTION_NOZEROINIT;

/* UART receive task */
static AMBA_KAL_TASK_t gUartReceiveTask;
static UINT8 gUartReceiveTaskStack[1024 * 1024 * 4]  GNU_SECTION_NOZEROINIT;

/* UART save task */
static AMBA_KAL_TASK_t gUartFetchTask;
static UINT8 gUartFetchTaskStack[1024 * 1024 * 4]  GNU_SECTION_NOZEROINIT;
static AMBA_FS_FILE *gpUartFetchSaveFile = NULL;

/* UART send task test */
static AMBA_KAL_TASK_t gUartSendTask;
static UINT8 gUartSendTaskStack[1024 * 1024 * 4]  GNU_SECTION_NOZEROINIT;
static AMBA_FS_FILE *gpUartSendFile = NULL;

char gUartStringBuf[SIZE_UART_BUFFER] __attribute__((section(".bss.noinit"))) GNU_ALIGNED_CACHESAFE;
UINT8 gDmaReadStringBuf[512] __attribute__((section(".bss.noinit"))) GNU_ALIGNED_CACHESAFE;

#else
#define SIZE_UART_BUFFER 512U
#endif

static void IoDiag_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    PrintFunc(StrBuf);
}

static void IoDiag_PrintFormattedStr(const char *pFmtString, const char *Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    const char *ArgsStr[2];

    ArgsStr[0] = Value;
    (void)AmbaUtility_StringPrintStr(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgsStr);
    PrintFunc(StrBuf);
}

static UINT32 IoDiag_UartRead(UINT32 UartChan, UINT32 StringSize, char *StringBuf, UINT32 TimeOut)
{
    UINT32 RxSize;

    (void)AmbaUART_Read(UartChan, 0U, StringSize, (UINT8 *)StringBuf, &RxSize, TimeOut);

    return RxSize;
}

static UINT32 IoDiag_UartDmaRead(UINT32 UartChan, UINT32 StringSize, char *StringBuf, UINT32 TimeOut)
{
    UINT32 RxSize;

    (void)AmbaUART_DmaRead(UartChan, 0U, StringSize, (UINT8 *)StringBuf, &RxSize, TimeOut);

    return RxSize;
}

static UINT32 IoDiag_UartWrite(UINT32 UartChan, UINT32 StringSize, const UINT8 *StringBuf, UINT32 TimeOut)
{
    UINT32 SentSize;

    (void)AmbaUART_Write(UartChan, 0U, StringSize, StringBuf, &SentSize, TimeOut);

    return SentSize;
}

static UINT32 IoDiag_UartDmaWrite(UINT32 UartChan, UINT32 StringSize, const UINT8 *StringBuf, UINT32 TimeOut)
{
    UINT32 SentSize;

    (void)AmbaUART_DmaWrite(UartChan, 0U, StringSize, StringBuf, &SentSize, TimeOut);

    return SentSize;
}

#if defined(UART_DMA_TEST_WITH_FILE_TRANSFER)
static void* IoDiag_UartReceiveTask(void* EntryArg)
{
    AMBA_SHELL_PRINT_f PrintFunc;
    UINT32 RxSize = 0;
    (void)EntryArg;
    AmbaMisra_TypeCast(&PrintFunc, &EntryArg);
    while (1) {
        RxSize = IoDiag_UartDmaRead(DiagUartCtrl.ReceiveChan, sizeof(gDmaReadStringBuf), (char *)gDmaReadStringBuf, 8000);
        if (RxSize) {
            AmbaWrap_memcpy(&gUartStringBuf[DiagUartCtrl.WritePtr], gDmaReadStringBuf, RxSize);
            AmbaKAL_MutexTake(&(DiagUartCtrl.Mutex), AMBA_KAL_WAIT_FOREVER);
            DiagUartCtrl.WritePtr = (DiagUartCtrl.WritePtr + RxSize) % SIZE_UART_BUFFER;
            AmbaKAL_MutexGive(&(DiagUartCtrl.Mutex));
            AmbaKAL_EventFlagSet(&(DiagUartCtrl.EventFlag), UART_DATA_READY_EVENT);
            if (RxSize != sizeof(gDmaReadStringBuf)) {
                AmbaKAL_EventFlagSet(&(DiagUartCtrl.EventFlag), UART_DATA_END_EVENT);
            }
        }
    }
    return NULL;
}

static void* IoDiag_UartFetchTask(void* EntryArg)
{
    AMBA_SHELL_PRINT_f PrintFunc;
    UINT32 ActualFlags;
    UINT32 DataSize;
    UINT32 StartReadPtr;
    UINT32 RetVal = 0;
    UINT32 WrittenSize = 0;
    (void)EntryArg;
    AmbaMisra_TypeCast(&PrintFunc, &EntryArg);
    while (1) {
        AmbaKAL_EventFlagGet(&(DiagUartCtrl.EventFlag), UART_DATA_READY_EVENT | UART_DATA_END_EVENT, KAL_FLAGS_WAIT_ANY, KAL_FLAGS_CLEAR_AUTO, &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        AmbaKAL_MutexTake(&(DiagUartCtrl.Mutex), AMBA_KAL_WAIT_FOREVER);
        if (DiagUartCtrl.ReadPtr < DiagUartCtrl.WritePtr) {
            DataSize = DiagUartCtrl.WritePtr - DiagUartCtrl.ReadPtr;
        } else if (DiagUartCtrl.ReadPtr > DiagUartCtrl.WritePtr) {
            DataSize = SIZE_UART_BUFFER - DiagUartCtrl.ReadPtr + DiagUartCtrl.WritePtr;
        } else {
            PrintFunc("IoDiag_UartFetchTask DataSize=0\n");
            DataSize = 0;
        }
        StartReadPtr = DiagUartCtrl.ReadPtr;
        DiagUartCtrl.ReadPtr = (DiagUartCtrl.ReadPtr + DataSize) % SIZE_UART_BUFFER;
        AmbaKAL_MutexGive(&(DiagUartCtrl.Mutex));
        if (gpUartFetchSaveFile != NULL) {
            RetVal = AmbaFS_FileWrite(&gUartStringBuf[StartReadPtr], 1, DataSize, gpUartFetchSaveFile, &WrittenSize);
        }
        if (ActualFlags & UART_DATA_END_EVENT) {
            if (gpUartFetchSaveFile != NULL) {
                RetVal = AmbaFS_FileClose(gpUartFetchSaveFile);
                if (RetVal != OK) {
                    IoDiag_PrintFormattedInt("UartFetchSaveFile Close Fail(%d)!\n", RetVal, PrintFunc);
                } else {
                    IoDiag_PrintFormattedInt("UartFetchSaveFile Close Success(%d)!\n", RetVal, PrintFunc);
                    gpUartFetchSaveFile = NULL;
                }
            }
        }
    }

    return NULL;
}


static UINT8 gDmaUartSendBuf[512] __attribute__((section(".bss.noinit"))) GNU_ALIGNED_CACHESAFE;

static void* IoDiag_UartSendTask(void* EntryArg)
{
    AMBA_SHELL_PRINT_f PrintFunc;
    UINT32 RetVal = 0;
    UINT32 DataSize = 0;
    (void)EntryArg;
    AmbaMisra_TypeCast(&PrintFunc, &EntryArg);
    while (1) {
        do {
            if (gpUartSendFile != NULL) {
                RetVal = AmbaFS_FileRead(gDmaUartSendBuf, 1, sizeof(gDmaUartSendBuf), gpUartSendFile, &DataSize);
            }
            if ((DataSize > 0U) && (RetVal == 0U)) {
                IoDiag_UartDmaWrite(DiagUartCtrl.SendChan, DataSize, (UINT8 *)gDmaUartSendBuf, AMBA_KAL_WAIT_FOREVER);
            }
            AmbaKAL_TaskSleep(10);
        } while (DataSize > 0U);
        if ((AmbaFS_FileEof(gpUartSendFile) == 0U) || (RetVal != 0U)) {
            PrintFunc("IoDiag_UartSendTask Failed to Send \r\n ");
        }
        if (gpUartSendFile != NULL) {
            RetVal = AmbaFS_FileClose(gpUartSendFile);
            if (0U != RetVal) {
                PrintFunc("Failed to close UartSendFile.\n");
            } else {

                PrintFunc("Success to close UartSendFile.\n");
                gpUartSendFile = NULL;
            }
        }
        AmbaKAL_TaskSleep(1000);
    }

    return NULL;
}
#endif
static void IoDiag_UartCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <Channel> cfg <BaudRate> <NumDataBits> <ParityBitMode> <NumStopBits>\n");
    PrintFunc("            <BaudRate>: The baud rate of UART\n");
    PrintFunc("            < NumDataBits > = DATA_5_BIT  | DATA_6_BIT   | DATA_7_BIT |   DATA_8_BIT\n");
    PrintFunc("            <ParityBitMode> = PARITY_NONE | PARITY_EVEN  | PARITY_ODD\n");
    PrintFunc("            < NumStopBits > = STOP_1_BIT  | STOP_1D5_BIT | STOP_2_BIT\n");
    PrintFunc("            < NumStopBits > = STOP_1_BIT  | STOP_1D5_BIT | STOP_2_BIT\n");
    PrintFunc("       Example: uart 0 cfg 115200 DATA_8_BIT PARITY_NONE STOP_1_BIT\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <Channel> write <data>\n");
    PrintFunc("       Example: uart 0 write abcde12345\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <Channel> read <count> \n");
    PrintFunc("       Example: uart 1 read 10\n");
#if defined(UART_DMA_TEST_WITH_FILE_TRANSFER)
    PrintFunc(pArgVector[0]);
    PrintFunc(" <Channel> dmatest <savefile> <sendfile> \n");
    PrintFunc("       Example: uart 1 dmatest C:\\savefile.bin C:\\sendfile.bin \n");
#endif
}

/**
 *  UART diagnostic command
 *  @param[in] ArgCount Argument count
 *  @param[in] pArgVector Argument vector
 *  @param[in] PrintFunc function to print messages on shell task
 */
void AmbaIOUTDiag_CmdUART(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    static UINT8 AmbaDiagInitUartRxRingBuf[AMBA_NUM_UART_CHANNEL][SIZE_UART_BUFFER] __attribute__((section(".bss.noinit")));
    UINT32 UartChan;
    UINT32 StringSize, RxSize, SentSize;
    char StringBuf[SIZE_UART_BUFFER];
    static AMBA_UART_CONFIG_s UartConfig = {AMBA_UART_DATA_8_BIT, AMBA_UART_PARITY_NONE, AMBA_UART_STOP_1_BIT};
    static UINT32 BaudRate = 115200U;

    if (ArgCount < 2U) {
        IoDiag_UartCmdUsage(pArgVector, PrintFunc);
    } else {
        (void)AmbaUtility_StringToUInt32(pArgVector[1], &UartChan);
        if (UartChan >= AMBA_NUM_UART_CHANNEL) {
            PrintFunc("[Diag][UART] ERROR: Invalid UART id.\n");
        } else {
            (void)AmbaUART_Config(UartChan, BaudRate, &UartConfig);
            (void)AmbaUART_HookDeferredRxBuf(UartChan, SIZE_UART_BUFFER, &(AmbaDiagInitUartRxRingBuf[UartChan][0]));
            (void)AmbaUART_ClearDeferredRxBuf(UartChan);
        }

        if (ArgCount >= 3U) {
            if (AmbaUtility_StringCompare(pArgVector[2], "cfg", 3U) == 0) {
                (void)AmbaUtility_StringToUInt32(pArgVector[3], &BaudRate);

                if (ArgCount >= 7U) {
                    if (AmbaUtility_StringCompare(pArgVector[4], "DATA_5_BIT", 10U) == 0) {
                        UartConfig.NumDataBits = AMBA_UART_DATA_5_BIT;
                    } else if (AmbaUtility_StringCompare(pArgVector[4], "DATA_6_BIT", 10U) == 0) {
                        UartConfig.NumDataBits = AMBA_UART_DATA_6_BIT;
                    } else if (AmbaUtility_StringCompare(pArgVector[4], "DATA_7_BIT", 10U) == 0) {
                        UartConfig.NumDataBits = AMBA_UART_DATA_7_BIT;
                    } else if (AmbaUtility_StringCompare(pArgVector[4], "DATA_8_BIT", 10U) == 0) {
                        UartConfig.NumDataBits = AMBA_UART_DATA_8_BIT;
                    } else {
                        PrintFunc("[Diag][UART] ERROR: Invalid UART NumDataBits.\n");
                    }

                    if (AmbaUtility_StringCompare(pArgVector[5], "PARITY_NONE", 11U) == 0) {
                        UartConfig.ParityBitMode = AMBA_UART_PARITY_NONE;
                    } else if (AmbaUtility_StringCompare(pArgVector[5], "PARITY_EVEN", 11U) == 0) {
                        UartConfig.ParityBitMode = AMBA_UART_PARITY_EVEN;
                    } else if (AmbaUtility_StringCompare(pArgVector[5], "PARITY_ODD", 10U) == 0) {
                        UartConfig.ParityBitMode = AMBA_UART_PARITY_ODD;
                    } else {
                        PrintFunc("[Diag][UART] ERROR: Invalid UART ParityBitMode.\n");
                    }

                    if (AmbaUtility_StringCompare(pArgVector[6], "STOP_1_BIT", 10U) == 0) {
                        UartConfig.NumStopBits = AMBA_UART_STOP_1_BIT;
                    } else if (AmbaUtility_StringCompare(pArgVector[6], "STOP_1D5_BIT", 12U) == 0) {
                        UartConfig.NumStopBits = AMBA_UART_STOP_1D5_BIT;
                    } else if (AmbaUtility_StringCompare(pArgVector[6], "STOP_2_BIT", 10U) == 0) {
                        UartConfig.NumStopBits = AMBA_UART_STOP_2_BIT;
                    } else {
                        PrintFunc("[Diag][UART] ERROR: Invalid UART NumStopBits.\n");
                    }

                    (void)AmbaUART_Config(UartChan, BaudRate, &UartConfig);

                    (void)AmbaUART_HookDeferredRxBuf(UartChan, SIZE_UART_BUFFER, &(AmbaDiagInitUartRxRingBuf[UartChan][0]));
                    (void)AmbaUART_ClearDeferredRxBuf(UartChan);
                } else {
                    PrintFunc("[Diag][UART] ERROR: Invalid UART Config.\n");
                }
            } else if (AmbaUtility_StringCompare(pArgVector[2], "write", 5U) == 0) {
                if (ArgCount >= 4U) {
                    StringSize = AmbaUtility_StringLength(pArgVector[3]);
                    AmbaUtility_StringCopy(&StringBuf[0], StringSize + 1U, pArgVector[3]);

                    SentSize = IoDiag_UartWrite(UartChan, StringSize + 1U, (UINT8 *)StringBuf, AMBA_KAL_WAIT_FOREVER);

                    PrintFunc("[Diag][UART]");
                    IoDiag_PrintFormattedInt("UART%d: ", UartChan, PrintFunc);
                    IoDiag_PrintFormattedInt("TxSize = %d, ", SentSize, PrintFunc);
                    IoDiag_PrintFormattedStr("TxData = %s\n", StringBuf, PrintFunc);
                } else {
                    PrintFunc("[Diag][UART] ERROR: ");
                    IoDiag_PrintFormattedInt("UART%d write failed", UartChan, PrintFunc);
                }
            } else if (AmbaUtility_StringCompare(pArgVector[2], "read", 4U) == 0) {
                if (ArgCount >= 4U) {
                    (void)AmbaUtility_StringToUInt32(pArgVector[3], &StringSize);

                    RxSize = IoDiag_UartRead(UartChan, StringSize, StringBuf, AMBA_KAL_WAIT_FOREVER);

                    PrintFunc("[Diag][UART]");
                    IoDiag_PrintFormattedInt("UART%d: ", UartChan, PrintFunc);
                    IoDiag_PrintFormattedInt("RxSize = %d, ", RxSize, PrintFunc);
                    IoDiag_PrintFormattedStr("RxData = %s\n", StringBuf, PrintFunc);
                } else {
                    PrintFunc("[Diag][UART] ERROR: ");
                    IoDiag_PrintFormattedInt("UART%d read failed", UartChan, PrintFunc);
                }
            } else if (AmbaUtility_StringCompare(pArgVector[2], "dmawrite", 7U) == 0) {
                if (ArgCount >= 4U) {
                    StringSize = AmbaUtility_StringLength(pArgVector[3]);
                    AmbaUtility_StringCopy(&StringBuf[0], StringSize + 1U, pArgVector[3]);

                    SentSize = IoDiag_UartDmaWrite(UartChan, StringSize + 1U, (UINT8 *)StringBuf, AMBA_KAL_WAIT_FOREVER);

                    PrintFunc("[Diag][UART]");
                    IoDiag_PrintFormattedInt("UART%d: ", UartChan, PrintFunc);
                    IoDiag_PrintFormattedInt("TxSize = %d, ", SentSize, PrintFunc);
                    IoDiag_PrintFormattedStr("TxData = %s\n", StringBuf, PrintFunc);
                } else {
                    PrintFunc("[Diag][UART] ERROR: ");
                    IoDiag_PrintFormattedInt("UART%d DMA write failed", UartChan, PrintFunc);
                }
            } else if (AmbaUtility_StringCompare(pArgVector[2], "dmaread", 7U) == 0) {
                if (ArgCount >= 4U) {
                    (void)AmbaUtility_StringToUInt32(pArgVector[3], &StringSize);

                    RxSize = IoDiag_UartDmaRead(UartChan, StringSize, StringBuf, AMBA_KAL_WAIT_FOREVER);

                    PrintFunc("[Diag][UART]");
                    IoDiag_PrintFormattedInt("UART%d: ", UartChan, PrintFunc);
                    IoDiag_PrintFormattedInt("RxSize = %d, ", RxSize, PrintFunc);
                    IoDiag_PrintFormattedStr("RxData = %s\n", StringBuf, PrintFunc);
                } else {
                    PrintFunc("[Diag][UART] ERROR: ");
                    IoDiag_PrintFormattedInt("UART%d DMA read failed", UartChan, PrintFunc);
                }
#if defined(UART_DMA_TEST_WITH_FILE_TRANSFER)
            } else if (AmbaUtility_StringCompare(pArgVector[2], "dmatest", 7U) == 0) {
                UINT32  Rval = 0;
                AMBA_KAL_TASK_INFO_s taskInfo;
                Rval = AmbaKAL_TaskQuery(&gUartReceiveTask, &taskInfo);
                DiagUartCtrl.ReceiveChan = UartChan;
                DiagUartCtrl.SendChan = UartChan;
                if (Rval != 0U) {
                    if (AmbaKAL_EventFlagCreate(&(DiagUartCtrl.EventFlag), NULL) != OK) {
                        PrintFunc("DiagUartCtrl EventFlag create failed!.\n");
                    }
                    if (AmbaKAL_MutexCreate(&(DiagUartCtrl.Mutex), "DiagUartCtrl_Mutex") != OK) {
                        PrintFunc("DiagUartCtrl  Mutex create failed!.\n");
                    }
                    DiagUartCtrl.ReadPtr = DiagUartCtrl.WritePtr = 0;
                    Rval = AmbaKAL_TaskCreate(&gUartReceiveTask, "UartReceiveTask", 45, IoDiag_UartReceiveTask, PrintFunc, \
                                              gUartReceiveTaskStack, sizeof(gUartReceiveTaskStack), AMBA_KAL_AUTO_START);
                    if (Rval != 0) {
                        PrintFunc("IoDiag_UartReceiveTask create failed!.\n");
                    } else {
                        PrintFunc("IoDiag_UartReceiveTask create done!.\n");
                    }

                    Rval = AmbaKAL_TaskCreate(&gUartFetchTask, "UartFetchTask", 48, IoDiag_UartFetchTask, PrintFunc, \
                                              gUartFetchTaskStack, sizeof(gUartFetchTaskStack), AMBA_KAL_AUTO_START);
                    if (Rval != 0) {
                        PrintFunc("UartFetchTask create failed!.\n");
                    } else {
                        PrintFunc("UartFetchTask create done!.\n");
                    }

                    /* Save the receive data as file */
                    if (ArgCount > 3) {
                        Rval = AmbaFS_FileOpen(pArgVector[3], "wb", &gpUartFetchSaveFile);

                        if (Rval != 0) {
                            PrintFunc("AmbaFS_FileOpen UartFetchSaveFile failed!.\n");
                        } else {
                            PrintFunc("AmbaFS_FileOpen UartFetchSaveFile success!.\n");
                        }
                    }

                    /* Send the data from file */
                    if (ArgCount > 4) {
                        Rval = AmbaFS_FileOpen(pArgVector[4], "r", &gpUartSendFile);
                        if (Rval != 0) {
                            PrintFunc("AmbaFS_FileOpen UartSendFile failed!.\n");
                        } else {
                            PrintFunc("AmbaFS_FileOpen UartSendFile success!.\n");
                        }
                        Rval = AmbaKAL_TaskCreate(&gUartSendTask, "UartSendTask", 29, IoDiag_UartSendTask, PrintFunc, \
                                                  gUartSendTaskStack, sizeof(gUartSendTaskStack), AMBA_KAL_AUTO_START);
                    }
                }
#endif
            } else {
                IoDiag_UartCmdUsage(pArgVector, PrintFunc);
            }
        }
    }
}

