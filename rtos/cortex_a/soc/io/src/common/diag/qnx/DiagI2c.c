/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
 * Copyright 2020, Ambarella International LP
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <hw/i2c.h>
#include "diag.h"

#include <stdint.h>
#include <stdio.h>
#include "AmbaCortexA53.h"
#include "AmbaReg_I2C.h"

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaI2C.h"

typedef struct {
    UINT32 TxSize:          8;
    UINT32 RxSize:          8;
    UINT32 StopCond:        8;
    UINT32 RepeatStartCond: 8;
} DIAG_I2C_CTRL_s;

#define DIAG_I2C_TX_TRANSACTION     (4U)

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

AMBA_I2CS_REG_s *pAmbaI2C_SlaveReg;
#if defined(CONFIG_SOC_CV2)
#define AMBA_I2C_SLAVE_BASE      AMBA_CORTEX_A53_I2C_SLAVE_BASE_ADDR
#elif defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define AMBA_I2C_SLAVE_BASE      AMBA_CA53_I2C_SLAVE_BASE_ADDR
#else
#define AMBA_I2C_SLAVE_BASE      0
#endif
#define AMBA_I2C_SLAVE_SIZE      0x1000

static void IoDiag_I2csInit()
{
    uintptr_t virt_base;

    virt_base = mmap_device_io(AMBA_I2C_SLAVE_SIZE, AMBA_I2C_SLAVE_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        printf("mmap_device_io NG\n");
    } else {
        pAmbaI2C_SlaveReg = (AMBA_I2CS_REG_s *)virt_base;
    }
}

static void IoDiag_I2cCmdUsage()
{
    fprintf(stderr, "Usage: ");
    fprintf(stderr, "i2c <master_id> simpletest [start|stop] (<speed>)          - start/stop simple test\n");
    fprintf(stderr, "i2c <master_id> <speed> slave <data0> <data1> ...          - w/r test\n");
    fprintf(stderr, "   Ex: diag-ambarella i2c 0 1 slave 0x34 0x30 0x03 0x1 slave 0x35 1\n");
    fprintf(stderr, "   Ex: diag-ambarella i2c 2 0 slave 0x60 0 slave 0xa0 0 slave 0xa1 128\n");
}

static void IoDiag_I2cDumpData(UINT32 DataSize, const UINT8 *pDataBuf)
{
    UINT32 i;

    if (DataSize != 0U) {
        printf("---------------------------------------------------------\n");
        printf("Offset(h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
        for (i = 0; i < DataSize; i ++) {
            if ((i & 0xfU) == 0x0U) {
                printf("%08X  ", i);
                printf("%.2X ", pDataBuf[i]);
            } else {
                printf("%.2X ", pDataBuf[i]);
            }

            if ((i & 0xfU) == 0xfU) {
                printf("\n");
            }
        }

        if ((i & 0xfU) != 0x0U) {
            printf("\n");
        }
    }
}

/**
 *  IoDiag_I2cSimpleTest - I2C read after write test
 *  @param[in] I2cChan I2C master ID
 *  @param[in] ArgCount Argument count
 *  @param[in] pArgVector Argument vector
 *  @param[in] PrintFunc function to print messages on shell task
 *  @return error code
 */
static void IoDiag_I2cSimpleTest(UINT32 I2cChan)
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
        printf("[Diag][I2C]");
        printf("[Master_%u]", I2cChan);
        printf(" S %02x", TxCtrl.SlaveAddr);
        for (i = 0U; i < TxCtrl.DataSize; i ++) {
            printf(" %02x", (UINT32)TxCtrl.pDataBuf[i]);
        }
        printf(" Sr %02x", RxCtrl.SlaveAddr);
        printf(" (%u bytes) P\n", RxCtrl.DataSize);
    }

    if (RetVal != OK) {
        printf("[Diag][I2C]");
        printf("[Master_%u] Failure.\n", I2cChan);
        printf("(0x%08X)\n", RetVal);
    } else {
        printf("[Diag][I2C]");
        printf("[Master_%u] Success.\n", I2cChan);

        if (NumRxTransaction != 0U) {
            IoDiag_I2cDumpData(RxCtrl.DataSize, RxCtrl.pDataBuf);
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

static void * IoDiag_I2cSlaveTaskEntry(void * Arg)
{
    UINT32 i, j, ActualFlags;
    UINT32 NumRxData, NumTxData;
    UINT32 Loop = 1U;

    AmbaMisra_TouchUnused(Arg);

    while (1U == Loop) {
        printf("[Diag][I2C] Slave is ready. Wait for transaction starts..\n");
        (void)AmbaKAL_EventFlagGet(&DiagI2cEventFlag, 0xffffffffU, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        printf("[Diag][I2C] At least one transaction is done.\n");

        NumRxData = 0U;
        NumTxData = 0U;
        for (i = 0U; i < DiagI2cSlaveNumPrint; i ++) {
            NumRxData += DiagI2cSlaveCtrl[i].RxSize;
            NumTxData += DiagI2cSlaveCtrl[i].TxSize;
        }

        for (i = DiagI2cSlaveNumPrint; i < DiagI2cSlaveNumTransaction; i ++) {
            for (j = 0U; j < DiagI2cSlaveCtrl[i].RxSize; j ++) {
                printf("[Diag][I2C] Slave get data[%u]: 0x%02x\n", j, DiagI2cSlaveRxBuf[NumRxData + j]);
            }
            NumRxData += DiagI2cSlaveCtrl[i].RxSize;

            if (DiagI2cSlaveCtrl[i].RepeatStartCond != 0x0U) {
                printf("[Diag][I2C] Transaction ends with Repeated-START condition.\n");
            }

            for (j = 0U; j < DiagI2cSlaveCtrl[i].TxSize; j ++) {
                printf("[Diag][I2C] Slave put data[%u]: 0x%02x\n", j, DiagI2cSlaveTxBuf[NumTxData + j]);
            }
            NumTxData += DiagI2cSlaveCtrl[i].TxSize;

            if (DiagI2cSlaveCtrl[i].StopCond != 0x0U) {
                printf("[Diag][I2C] Transaction ends with STOP condition. (Totally Rx %u bytes, Tx %u bytes)\n", DiagI2cSlaveRxSize, DiagI2cSlaveTxSize);

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
 *  IoDiag_I2cListenModeTest - I2C listening mode test
 *  @param[in] I2cChan I2C slave ID
 *  @param[in] EnableFlag Enable flag
 *  @param[in] PrintFunc function to print messages on shell task
 *  @return error code
 */
static UINT32 IoDiag_I2cListenModeTest(UINT32 I2cChan, UINT32 SlaveAddr)
{
    static AMBA_KAL_TASK_t DiagI2cSlaveTask;
    static UINT8 DiagI2cSlaveTaskStack[0x1000];
    static char I2cEventFlagName[24] = "DiagI2cSlaveEventFlag";
    static char I2cTaskName[20] = "DiagI2cSlaveTask";
    UINT32 RetVal;

    if (SlaveAddr >= 0x80U) {
        RetVal = AmbaI2C_SlaveStop(I2cChan);
        printf("[Diag][I2C] Stop slave task.\n");

        (void)AmbaKAL_TaskTerminate(&DiagI2cSlaveTask);
        (void)AmbaKAL_TaskDelete(&DiagI2cSlaveTask);
        (void)AmbaKAL_EventFlagDelete(&DiagI2cEventFlag);
    } else {
        RetVal = AmbaKAL_TaskCreate(&DiagI2cSlaveTask, I2cTaskName, 60U, IoDiag_I2cSlaveTaskEntry, NULL, DiagI2cSlaveTaskStack, sizeof(DiagI2cSlaveTaskStack), AMBA_KAL_DONT_START);
        if (RetVal == KAL_ERR_NONE) {
            RetVal = AmbaI2C_SlaveStart(I2cChan, SlaveAddr << 1UL, IoDiag_I2cSlaveWrite, IoDiag_I2cSlaveRead, IoDiag_I2cSlaveBreak);
            printf("[Diag][I2C] Start slave task.\n");

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
        if (strncmp(pArgs[0], "slave", 5U) == 0) {
            SlaveAddr = strtol(pArgs[1], NULL, 0);

            if ((SlaveAddr & 0x1U) == 0x0U) {
                /* Tx transaction */
                if (NumTxTransaction < DIAG_I2C_TX_TRANSACTION) {
                    pTxCtrl[NumTxTransaction].SlaveAddr = SlaveAddr;
                    pTxCtrl[NumTxTransaction].pDataBuf = &pTxBuf[NumTxBytes];

                    for (i = 2U; i < NumArgs; i ++) {
                        if (strncmp(pArgs[i], "slave", 5U) == 0) {
                            break;
                        } else {
                            Data = strtol(pArgs[i], NULL, 0);
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
                    pRxCtrl->DataSize = strtol(pArgs[2], NULL, 0);
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
 *  @return error code
 */
static UINT32 IoDiag_I2cReadAfterWrite(UINT32 I2cChan, UINT32 ArgCount, char * const *pArgVector)
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
            printf("[Diag][I2C]");
            printf("[Master_%u]", I2cChan);
            printf(" S %02x", RxCtrl.SlaveAddr);
            printf(" (%u bytes) P\n", RxCtrl.DataSize);
        } else {
            RetVal = AmbaI2C_MasterReadAfterWrite(I2cChan, DiagI2cSpeed[I2cChan], NumTxTransaction, &TxCtrl[0], &RxCtrl, &ActualSize, AMBA_KAL_WAIT_FOREVER);

            /* The expected waveform on the bus */
            printf("[Diag][I2C]");
            printf("[Master_%u]", I2cChan);
            printf(" S %02x", TxCtrl[0].SlaveAddr);
            for (i = 0U; i < TxCtrl[0].DataSize; i ++) {
                printf(" %02x", (UINT32)TxCtrl[0].pDataBuf[i]);
            }
            for (j = 1U; j < NumTxTransaction; j ++) {
                printf(" Sr %02x", TxCtrl[j].SlaveAddr);
                for (i = 0U; i < TxCtrl[j].DataSize; i ++) {
                    printf(" %02x", (UINT32)TxCtrl[j].pDataBuf[i]);
                }
            }
            printf(" Sr %02x", RxCtrl.SlaveAddr);
            printf(" (%u bytes) P\n", RxCtrl.DataSize);
        }
    } else {
        if (NumTxTransaction != 0U) {   /* NumTxTransaction = 1U; */
            RetVal = AmbaI2C_MasterWrite(I2cChan, DiagI2cSpeed[I2cChan], &TxCtrl[0], &ActualSize, AMBA_KAL_WAIT_FOREVER);

            /* The expected waveform on the bus */
            printf("[Diag][I2C]");
            printf("[Master_%u]", I2cChan);
            printf(" S %02x", TxCtrl[0].SlaveAddr);
            for (i = 0U; i < TxCtrl[0].DataSize; i ++) {
                printf(" %02x", (UINT32)TxCtrl[0].pDataBuf[i]);
            }
            printf(" P\n");
        } else {
            printf("[Diag][I2C] ERROR: No transaction is exectuted.\n");
            RetVal = ~OK;
        }
    }

    if (RetVal != OK) {
        printf("[Diag][I2C]");
        printf("[Master_%u] Failure. ", I2cChan);
        printf("(0x%08X)\n", RetVal);
    } else {
        printf("[Diag][I2C]");
        printf("[Master_%u] Success.\n", I2cChan);

        if (NumRxTransaction != 0U) {
            IoDiag_I2cDumpData(RxCtrl.DataSize, RxCtrl.pDataBuf);
        }
    }

    return RetVal;
}

int DoI2CDiag(int argc, char *argv[])
{
    UINT32 I2cChan, SlaveAddr;
    UINT32 I2cSpeed = 1U;

    if (argc < 5) {
        IoDiag_I2cCmdUsage();
    } else {
        I2cChan = strtol(argv[2], NULL, 0);
        if (I2cChan >= AMBA_NUM_I2C_CHANNEL) {
            printf("[Diag][I2C] ERROR: Invalid i2c master id.\n");
        } else {
            if (argc >= 5) {
                if (strncmp(argv[3], "simpletest", 10U) == 0) {
                    if (strncmp(argv[4], "start", 5U) == 0) {
                        IoDiag_I2csInit();
                        if (argv[5] != NULL ) {
                            I2cSpeed = strtol(argv[5], NULL, 0);
                        }
                        DiagI2cSpeed[I2cChan] = I2cSpeed;
                        SlaveAddr = 0x08U;
                        (void)IoDiag_I2cListenModeTest(00, SlaveAddr);
                        (void)AmbaKAL_TaskSleep(100U);
                        (void)IoDiag_I2cSimpleTest(I2cChan);
                    } else if (strncmp(argv[4], "stop", 4U) == 0) {
                        (void)IoDiag_I2cListenModeTest(I2cChan, 0xffffffffU);
                    }
                } else {
                    I2cSpeed = strtol(argv[3], NULL, 0);
                    DiagI2cSpeed[I2cChan] = I2cSpeed;
                    (void)IoDiag_I2cReadAfterWrite(I2cChan, argc - 4U, &argv[4]);
                }
            }
        }
    }

    return 0;
}

