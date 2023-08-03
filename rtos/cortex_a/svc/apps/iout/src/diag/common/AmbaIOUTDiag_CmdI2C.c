/**
 *  @file AmbaIOUTDiag_CmdI2C.c
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
 *  @details I2C diagnostic command.
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"

#include "AmbaPrint.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"

#include "AmbaIOUTDiag.h"
#include "AmbaI2C.h"

#define DIAG_I2C_TX_TRANSACTION     (4U)

typedef struct {
    UINT32 TxSize:          8;
    UINT32 RxSize:          8;
    UINT32 StopCond:        8;
    UINT32 RepeatStartCond: 8;
} DIAG_I2C_CTRL_s;

static UINT32 DiagI2cSpeed[AMBA_NUM_I2C_CHANNEL];
static UINT8 DiagI2cDataBuf[2][128];

static UINT32 DiagI2cSlaveNumTransaction;
static UINT32 DiagI2cSlaveNumPrint;
static DIAG_I2C_CTRL_s DiagI2cSlaveCtrl[DIAG_I2C_TX_TRANSACTION + 1U];

static UINT8 DiagI2cSlaveTxBuf[128];
static UINT8 DiagI2cSlaveRxBuf[128];
static UINT32 DiagI2cSlaveTxSize;
static UINT32 DiagI2cSlaveRxSize;

static AMBA_KAL_EVENT_FLAG_t DiagI2cEventFlag;

static void IoDiag_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    PrintFunc(StrBuf);
}

/**
 *  IoDiag_I2cDumpData - Dump I2C data bytes
 *  @param[in] DataSize Data size in Byte
 *  @param[in] pDataBuf Pointer to the data buffer
 *  @param[in] PrintFunc function to print messages on shell task
 */
static void IoDiag_I2cDumpData(UINT32 DataSize, const UINT8 *pDataBuf, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i;

    if (DataSize != 0U) {
        PrintFunc("---------------------------------------------------------\n");
        PrintFunc("Offset(h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
        for (i = 0; i < DataSize; i ++) {
            if ((i & 0xfU) == 0x0U) {
                IoDiag_PrintFormattedInt("%08X  ", i, PrintFunc);
                IoDiag_PrintFormattedInt("%.2X ", pDataBuf[i], PrintFunc);
            } else {
                IoDiag_PrintFormattedInt("%.2X ", pDataBuf[i], PrintFunc);
            }

            if ((i & 0xfU) == 0xfU) {
                PrintFunc("\n");
            }
        }

        if ((i & 0xfU) != 0x0U) {
            PrintFunc("\n");
        }
    }
}

/**
 *  IoDiag_I2cParseCmd - Parse command for read after write test
 *  @param[in] ArgCount Argument count
 *  @param[in] pArgVector Argument vector
 */
static void IoDiag_I2cParseCmd(UINT32 *pNumTxTransaction, AMBA_I2C_TRANSACTION_s *pTxCtrl, UINT32 *pNumRxTransaction, AMBA_I2C_TRANSACTION_s *pRxCtrl, UINT32 ArgCount, char * const *pArgVector)
{
    UINT8 *pTxBuf = DiagI2cDataBuf[0], *pRxBuf = DiagI2cDataBuf[1];
    UINT32 SlaveAddr, NumTxTransaction = 0U, NumRxTransaction = 0U;
    UINT32 i, Data, NumTxBytes = 0U, NumArgs = ArgCount;
    char * const *pArgs = pArgVector;

    while (NumArgs >= 2U) {
        if (AmbaUtility_StringCompare(pArgs[0], "slave", 5U) == 0) {
            (void)AmbaUtility_StringToUInt32(pArgs[1], &SlaveAddr);
            if ((SlaveAddr & 0x1U) == 0x0U) {
                /* Tx transaction */
                if (NumTxTransaction < DIAG_I2C_TX_TRANSACTION) {
                    pTxCtrl[NumTxTransaction].SlaveAddr = SlaveAddr;
                    pTxCtrl[NumTxTransaction].pDataBuf = &pTxBuf[NumTxBytes];

                    for (i = 2U; i < NumArgs; i ++) {
                        if (AmbaUtility_StringCompare(pArgs[i], "slave", 5U) == 0) {
                            break;
                        } else {
                            (void)AmbaUtility_StringToUInt32(pArgs[i], &Data);
                            pTxBuf[NumTxBytes + i - 2U] = (UINT8)Data;
                        }
                    }
                    pTxCtrl[NumTxTransaction].DataSize = i - 2U;
                    NumTxBytes += (i - 2U);
                    NumTxTransaction++;

                    NumArgs -= i;
                    pArgs = &pArgs[i];
                } else {
                    /* The maximum number of tx transaction supported by this diag command. */
                    NumArgs -= 2U;
                    pArgs = &pArgs[2];
                }
            } else {
                /* Rx transaction */
                if (NumArgs >= 3U) {
                    pRxCtrl->SlaveAddr = SlaveAddr;
                    pRxCtrl->pDataBuf = &pRxBuf[0];
                    (void)AmbaUtility_StringToUInt32(pArgs[2], &(pRxCtrl->DataSize));
                    NumRxTransaction = 1U;
                }

                break;  /* At most one rx transaction is allowed. */
            }
        } else {
            /* Wrong command usage */
            NumArgs -= 1U;
            pArgs = &pArgs[1];
        }
    }

    *pNumTxTransaction = NumTxTransaction;
    *pNumRxTransaction = NumRxTransaction;
}

/**
 *  IoDiag_I2cReadAfterWrite - I2C read after write test
 *  @param[in] I2cChan I2C master ID
 *  @param[in] ArgCount Argument count
 *  @param[in] pArgVector Argument vector
 *  @param[in] PrintFunc function to print messages on shell task
 *  @return error code
 */
static UINT32 IoDiag_I2cReadAfterWrite(UINT32 I2cChan, UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_I2C_TRANSACTION_s TxCtrl[DIAG_I2C_TX_TRANSACTION], RxCtrl;
    UINT32 NumTxTransaction = 0U, NumRxTransaction = 0U;
    UINT32 i, j, ActualSize, RetVal = OK;

    for (i = 0U; i < DIAG_I2C_TX_TRANSACTION; i ++) {
        TxCtrl[i].SlaveAddr = 0U;
        TxCtrl[i].DataSize = 0U;
        TxCtrl[i].pDataBuf = NULL;
    }
    RxCtrl.SlaveAddr = 0U;
    RxCtrl.DataSize = 0U;
    RxCtrl.pDataBuf = NULL;

    IoDiag_I2cParseCmd(&NumTxTransaction, &TxCtrl[0], &NumRxTransaction, &RxCtrl, ArgCount, pArgVector);

    if (NumRxTransaction != 0U) {
        if (NumTxTransaction == 0U) {
            RetVal = AmbaI2C_MasterRead(I2cChan, DiagI2cSpeed[I2cChan], &RxCtrl, &ActualSize, AMBA_KAL_WAIT_FOREVER);

            /* The expected waveform on the bus */
            PrintFunc("[Diag][I2C]");
            IoDiag_PrintFormattedInt("[Master_%u]", I2cChan, PrintFunc);
            IoDiag_PrintFormattedInt(" S %02x", RxCtrl.SlaveAddr, PrintFunc);
            IoDiag_PrintFormattedInt(" (%u bytes) P\n", RxCtrl.DataSize, PrintFunc);
        } else {
            RetVal = AmbaI2C_MasterReadAfterWrite(I2cChan, DiagI2cSpeed[I2cChan], NumTxTransaction, &TxCtrl[0], &RxCtrl, &ActualSize, AMBA_KAL_WAIT_FOREVER);

            /* The expected waveform on the bus */
            PrintFunc("[Diag][I2C]");
            IoDiag_PrintFormattedInt("[Master_%u]", I2cChan, PrintFunc);
            IoDiag_PrintFormattedInt(" S %02x", TxCtrl[0].SlaveAddr, PrintFunc);
            for (i = 0U; i < TxCtrl[0].DataSize; i ++) {
                IoDiag_PrintFormattedInt(" %02x", (UINT32)TxCtrl[0].pDataBuf[i], PrintFunc);
            }
            for (j = 1U; j < NumTxTransaction; j ++) {
                IoDiag_PrintFormattedInt(" Sr %02x", TxCtrl[j].SlaveAddr, PrintFunc);
                for (i = 0U; i < TxCtrl[j].DataSize; i ++) {
                    IoDiag_PrintFormattedInt(" %02x", (UINT32)TxCtrl[j].pDataBuf[i], PrintFunc);
                }
            }
            IoDiag_PrintFormattedInt(" Sr %02x", RxCtrl.SlaveAddr, PrintFunc);
            IoDiag_PrintFormattedInt(" (%u bytes) P\n", RxCtrl.DataSize, PrintFunc);
        }
    } else {
        if (NumTxTransaction != 0U) {   /* NumTxTransaction = 1U; */
            RetVal = AmbaI2C_MasterWrite(I2cChan, DiagI2cSpeed[I2cChan], &TxCtrl[0], &ActualSize, AMBA_KAL_WAIT_FOREVER);

            /* The expected waveform on the bus */
            PrintFunc("[Diag][I2C]");
            IoDiag_PrintFormattedInt("[Master_%u]", I2cChan, PrintFunc);
            IoDiag_PrintFormattedInt(" S %02x", TxCtrl[0].SlaveAddr, PrintFunc);
            for (i = 0U; i < TxCtrl[0].DataSize; i ++) {
                IoDiag_PrintFormattedInt(" %02x", (UINT32)TxCtrl[0].pDataBuf[i], PrintFunc);
            }
            PrintFunc(" P\n");
        } else {
            PrintFunc("[Diag][I2C] ERROR: No transaction is exectuted.\n");
            RetVal = ~OK;
        }
    }

    if (RetVal != OK) {
        PrintFunc("[Diag][I2C]");
        IoDiag_PrintFormattedInt("[Master_%u] Failure. ", I2cChan, PrintFunc);
        IoDiag_PrintFormattedInt("(0x%08X)\n", RetVal, PrintFunc);
    } else {
        PrintFunc("[Diag][I2C]");
        IoDiag_PrintFormattedInt("[Master_%u] Success.\n", I2cChan, PrintFunc);

        if (NumRxTransaction != 0U) {
            IoDiag_I2cDumpData(RxCtrl.DataSize, RxCtrl.pDataBuf, PrintFunc);
        }
    }

    return RetVal;
}

static void * IoDiag_I2cSlaveTaskEntry(void * Arg)
{
    UINT32 i, j, ActualFlags;
    UINT32 NumRxData, NumTxData;
    UINT32 Loop = 1U;

    AmbaMisra_TouchUnused(Arg);

    while (1U == Loop) {
        AmbaPrint_PrintUInt5("[Diag][I2C] Slave is ready. Wait for transaction starts..", 0, 0, 0, 0, 0);
        AmbaPrint_Flush();
        (void)AmbaKAL_EventFlagGet(&DiagI2cEventFlag, 0xffffffffU, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        AmbaPrint_PrintUInt5("[Diag][I2C] At least one transaction is done. (0x%08x)..", ActualFlags, 0, 0, 0, 0);
        AmbaPrint_Flush();

        NumRxData = 0U;
        NumTxData = 0U;
        for (i = 0U; i < DiagI2cSlaveNumPrint; i ++) {
            NumRxData += DiagI2cSlaveCtrl[i].RxSize;
            NumTxData += DiagI2cSlaveCtrl[i].TxSize;
        }

        for (i = DiagI2cSlaveNumPrint; i < DiagI2cSlaveNumTransaction; i ++) {
            for (j = 0U; j < DiagI2cSlaveCtrl[i].RxSize; j ++) {
                AmbaPrint_PrintUInt5("[Diag][I2C] Slave get data[%u]: 0x%02x", j, DiagI2cSlaveRxBuf[NumRxData + j], 0, 0, 0);
                AmbaPrint_Flush();
            }
            NumRxData += DiagI2cSlaveCtrl[i].RxSize;

            if (DiagI2cSlaveCtrl[i].RepeatStartCond != 0x0U) {
                AmbaPrint_PrintUInt5("[Diag][I2C] Transaction ends with Repeated-START condition.", 0, 0, 0, 0, 0);
                AmbaPrint_Flush();
            }

            for (j = 0U; j < DiagI2cSlaveCtrl[i].TxSize; j ++) {
                AmbaPrint_PrintUInt5("[Diag][I2C] Slave put data[%u]: 0x%02x", j, DiagI2cSlaveTxBuf[NumTxData + j], 0, 0, 0);
                AmbaPrint_Flush();
            }
            NumTxData += DiagI2cSlaveCtrl[i].TxSize;

            if (DiagI2cSlaveCtrl[i].StopCond != 0x0U) {
                AmbaPrint_PrintUInt5("[Diag][I2C] Transaction ends with STOP condition. (Totally Rx %u bytes, Tx %u bytes)", DiagI2cSlaveRxSize, DiagI2cSlaveTxSize, 0, 0, 0);
                AmbaPrint_Flush();

                DiagI2cSlaveNumTransaction = 0U;
                DiagI2cSlaveRxSize = 0U;
                DiagI2cSlaveTxSize = 0U;
            }
        }

        DiagI2cSlaveNumPrint = DiagI2cSlaveNumTransaction;
    }

    return NULL;
}

/**
 *  IoDiag_I2cSimpleTest - I2C read after write test
 *  @param[in] I2cChan I2C master ID
 *  @param[in] ArgCount Argument count
 *  @param[in] pArgVector Argument vector
 *  @param[in] PrintFunc function to print messages on shell task
 *  @return error code
 */
static void IoDiag_I2cSimpleTest(UINT32 I2cChan, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_I2C_TRANSACTION_s TxCtrl, RxCtrl;
    UINT32 NumTxTransaction = 1U, NumRxTransaction = 1U;
    UINT32 i, ActualSize, RetVal = OK;

    /* TX */
    TxCtrl.SlaveAddr = 0x10U;
    TxCtrl.DataSize = 5U;
    TxCtrl.pDataBuf = DiagI2cDataBuf[0];
    TxCtrl.pDataBuf[0U] = 0x11U;
    TxCtrl.pDataBuf[1U] = 0x22U;
    TxCtrl.pDataBuf[2U] = 0x33U;
    TxCtrl.pDataBuf[3U] = 0x44U;
    TxCtrl.pDataBuf[4U] = 0x55U;

    /* RX */
    RxCtrl.SlaveAddr = 0x11U;
    RxCtrl.DataSize = 5U;
    RxCtrl.pDataBuf = DiagI2cDataBuf[1];

    if (NumRxTransaction != 0U) {
        RetVal = AmbaI2C_MasterReadAfterWrite(I2cChan, DiagI2cSpeed[I2cChan], NumTxTransaction, &TxCtrl, &RxCtrl, &ActualSize, AMBA_KAL_WAIT_FOREVER);

        /* The expected waveform on the bus */
        PrintFunc("[Diag][I2C]");
        IoDiag_PrintFormattedInt("[Master_%u]", I2cChan, PrintFunc);
        IoDiag_PrintFormattedInt(" S %02x", TxCtrl.SlaveAddr, PrintFunc);
        for (i = 0U; i < TxCtrl.DataSize; i ++) {
            IoDiag_PrintFormattedInt(" %02x", (UINT32)TxCtrl.pDataBuf[i], PrintFunc);
        }
        IoDiag_PrintFormattedInt(" Sr %02x", RxCtrl.SlaveAddr, PrintFunc);
        IoDiag_PrintFormattedInt(" (%u bytes) P\n", RxCtrl.DataSize, PrintFunc);
    }

    if (RetVal != OK) {
        PrintFunc("[Diag][I2C]");
        IoDiag_PrintFormattedInt("[Master_%u] Failure. ", I2cChan, PrintFunc);
        IoDiag_PrintFormattedInt("(0x%08X)\n", RetVal, PrintFunc);
    } else {
        PrintFunc("[Diag][I2C]");
        IoDiag_PrintFormattedInt("[Master_%u] Success.\n", I2cChan, PrintFunc);

        if (NumRxTransaction != 0U) {
            IoDiag_I2cDumpData(RxCtrl.DataSize, RxCtrl.pDataBuf, PrintFunc);
        }
    }
}

static void IoDiag_I2cSlaveWrite(UINT32 SlaveAddr, UINT32 MaxTxData, UINT32 * pNumTxData, UINT8 ** pTxData)
{
    if ((SlaveAddr < 0x100U) && (MaxTxData > 0U) && (pNumTxData != NULL) && (pTxData != NULL)) {
        if (DiagI2cSlaveTxSize < sizeof(DiagI2cSlaveTxBuf)) {
            DiagI2cSlaveTxBuf[DiagI2cSlaveTxSize] = (UINT8)(DiagI2cSlaveTxSize);

            *pTxData = &DiagI2cSlaveTxBuf[DiagI2cSlaveTxSize];
            *pNumTxData = 1U;
        } else {
            *pTxData = NULL;
            *pNumTxData = 0U;
        }
        DiagI2cSlaveTxSize ++;
    }
}

static void IoDiag_I2cSlaveRead(UINT32 SlaveAddr, UINT32 NumRxData, const UINT8 * pRxData, UINT32 * pStop)
{
    UINT32 i;

    if ((SlaveAddr < 0x100U) && (pRxData != NULL) && (DiagI2cSlaveRxSize < 128U)) {
        for (i = 0; i < NumRxData; i ++) {
            DiagI2cSlaveRxBuf[DiagI2cSlaveRxSize + i] = pRxData[i];
        }
        DiagI2cSlaveRxSize += NumRxData;
    }

    if (pStop != NULL) {
        if (DiagI2cSlaveRxSize < 128U) {
            *pStop = 0U;    /* ACK */
        } else {
            *pStop = 1U;    /* NACK */
        }
    }
}

static void IoDiag_I2cSlaveBreak(UINT32 CondType, UINT32 NumTxResidual)
{
    UINT32 i, NumRxData = 0U, NumTxData = 0U;

    if (DiagI2cSlaveNumTransaction > 0U) {
        for (i = 0U; i < DiagI2cSlaveNumTransaction; i ++) {
            NumRxData += DiagI2cSlaveCtrl[i].RxSize;
            NumTxData += DiagI2cSlaveCtrl[i].TxSize;
        }
    }

    DiagI2cSlaveCtrl[DiagI2cSlaveNumTransaction].TxSize = (UINT16)(DiagI2cSlaveTxSize - NumTxData);
    DiagI2cSlaveCtrl[DiagI2cSlaveNumTransaction].RxSize = (UINT16)(DiagI2cSlaveRxSize - NumRxData);

    if (CondType == 0U) {
        /* 0: STOP */
        DiagI2cSlaveCtrl[DiagI2cSlaveNumTransaction].StopCond = 1U;
        (void)AmbaKAL_EventFlagSet(&DiagI2cEventFlag, 0x80000000UL | NumTxResidual);
    } else {
        /* 1: Repeated-START */
        DiagI2cSlaveCtrl[DiagI2cSlaveNumTransaction].RepeatStartCond = 1U;
        (void)AmbaKAL_EventFlagSet(&DiagI2cEventFlag, 0x40000000UL | NumTxResidual);
    }

    DiagI2cSlaveNumTransaction ++;
}

/**
 *  IoDiag_I2cListenModeTest - I2C listening mode test
 *  @param[in] I2cChan I2C slave ID
 *  @param[in] EnableFlag Enable flag
 *  @param[in] PrintFunc function to print messages on shell task
 *  @return error code
 */
static UINT32 IoDiag_I2cListenModeTest(UINT32 I2cChan, UINT32 SlaveAddr, AMBA_SHELL_PRINT_f PrintFunc)
{
    static AMBA_KAL_TASK_t DiagI2cSlaveTask;
    static UINT8 DiagI2cSlaveTaskStack[0x1000];
    static char I2cEventFlagName[24] = "DiagI2cSlaveEventFlag";
    static char I2cTaskName[20] = "DiagI2cSlaveTask";
    UINT32 RetVal;

    if (SlaveAddr >= 0x80U) {
        RetVal = AmbaI2C_SlaveStop(I2cChan);
        PrintFunc("[Diag][I2C] Stop slave task.\n");

        (void)AmbaKAL_TaskTerminate(&DiagI2cSlaveTask);
        (void)AmbaKAL_TaskDelete(&DiagI2cSlaveTask);
        (void)AmbaKAL_EventFlagDelete(&DiagI2cEventFlag);
    } else {
        RetVal = AmbaKAL_TaskCreate(&DiagI2cSlaveTask, I2cTaskName, 60U, IoDiag_I2cSlaveTaskEntry, NULL, DiagI2cSlaveTaskStack, sizeof(DiagI2cSlaveTaskStack), AMBA_KAL_DONT_START);
        if (RetVal == KAL_ERR_NONE) {
            RetVal = AmbaI2C_SlaveStart(I2cChan, SlaveAddr << 1UL, IoDiag_I2cSlaveWrite, IoDiag_I2cSlaveRead, IoDiag_I2cSlaveBreak);
            PrintFunc("[Diag][I2C] Start slave task.\n");

            DiagI2cSlaveNumTransaction = 0U;
            DiagI2cSlaveNumPrint = 0U;
            DiagI2cSlaveTxSize = 0U;
            DiagI2cSlaveRxSize = 0U;

            AmbaMisra_TouchUnused(DiagI2cSlaveTxBuf);
            AmbaMisra_TouchUnused(DiagI2cSlaveRxBuf);

            (void)AmbaKAL_EventFlagCreate(&DiagI2cEventFlag, I2cEventFlagName);
            (void)AmbaKAL_EventFlagClear(&DiagI2cEventFlag, 0xffffffffU);

            (void)AmbaKAL_TaskSetSmpAffinity(&DiagI2cSlaveTask, 0x1U);
            (void)AmbaKAL_TaskResume(&DiagI2cSlaveTask);
            (void)AmbaKAL_TaskSleep(100U);  /* Make slave task be suspended on eventflags */
        }
    }

    return RetVal;
}

/**
 *  IoDiag_I2cShowInfo - Show I2C configuration info
 *  @param[in] I2cChan I2C master ID
 *  @param[in] PrintFunc function to print messages on shell task
 *  @return error code
 */
static void IoDiag_I2cShowInfo(UINT32 I2cChan, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (DiagI2cSpeed[I2cChan] == AMBA_I2C_SPEED_STANDARD) {
        PrintFunc("[Diag][I2C] Bus speed: Standard-mode (up to 100 kbit/s).\n");
    } else if (DiagI2cSpeed[I2cChan] == AMBA_I2C_SPEED_FAST) {
        PrintFunc("[Diag][I2C] Bus speed: Fast-mode (up to 400 kbit/s).\n");
    } else if (DiagI2cSpeed[I2cChan] == AMBA_I2C_SPEED_FAST_PLUS) {
        PrintFunc("[Diag][I2C] Bus speed: Fast-mode Plus (up to 1 Mbit/s).\n");
    } else if (DiagI2cSpeed[I2cChan] == AMBA_I2C_SPEED_HIGH) {
        PrintFunc("[Diag][I2C] Bus speed: High-speed mode (up to 3.4 Mbit/s).\n");
    } else {
        PrintFunc("[Diag][I2C] Bus speed: Unknown.\n");
    }
}

static void IoDiag_I2cCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <master_id> simpletest [start|stop]           - start/stop simple test\n");
}

/**
 *  I2C diagnostic command
 *  @param[in] ArgCount Argument count
 *  @param[in] pArgVector Argument vector
 *  @param[in] PrintFunc function to print messages on shell task
 */
void AmbaIOUTDiag_CmdI2C(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 I2cChan, SlaveAddr;

    if (ArgCount < 2U) {
        IoDiag_I2cCmdUsage(pArgVector, PrintFunc);
    } else {
        (void)AmbaUtility_StringToUInt32(pArgVector[1], &I2cChan);
        if (I2cChan >= AMBA_NUM_I2C_CHANNEL) {
            PrintFunc("[Diag][I2C] ERROR: Invalid i2c master id.\n");
        } else {
            if (ArgCount >= 4U) {
                if (AmbaUtility_StringCompare(pArgVector[2], "simpletest", 10U) == 0) {
                    if (AmbaUtility_StringCompare(pArgVector[3], "start", 5U) == 0) {
                        SlaveAddr = 0x08U;
                        (void)IoDiag_I2cListenModeTest(00, SlaveAddr, PrintFunc);
                        (void)AmbaKAL_TaskSleep(100U);
                        (void)IoDiag_I2cSimpleTest(I2cChan, PrintFunc);
                    } else if (AmbaUtility_StringCompare(pArgVector[3], "stop", 4U) == 0) {
                        (void)IoDiag_I2cListenModeTest(I2cChan, 0xffffffffU, PrintFunc);
                    } else {
                        PrintFunc("[Diag][I2C] ERROR: Unknown speed grade.\n");
                    }
                } else {
                    (void)IoDiag_I2cReadAfterWrite(I2cChan, ArgCount - 2U, &pArgVector[2], PrintFunc);
                }
            } else {
                IoDiag_I2cShowInfo(I2cChan, PrintFunc);
            }
        }
    }
}

