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

static UINT32 DiagI2cSpeed[AMBA_NUM_I2C_CHANNEL];
static UINT8 DiagI2cDataBuf[4][128];
#define DATA_BUF_IDX_M_W (0U)
#define DATA_BUF_IDX_M_R (1U)
#define DATA_BUF_IDX_S_W (2U)
#define DATA_BUF_IDX_S_R (3U)

#define TEST_SLAVE_ADDR (0x10U)
#define SLAVE_DEV_NAME "slave-24c02"

AMBA_I2C_TRANSACTION_s gSlaveTxCtrl;

static INT8 IoDiag_ParseDevNum(void)
{
    FILE *ifp_cat;
    FILE *ifp_number;
    char buf_cat[48]  = "\0";
    char buf_number[8]  = "\0";
    const char needle[] = "i2cs";
    char *needleRet;
    INT8 dev_num = -1;

    if ((ifp_cat = popen("ls -r /sys/bus/i2c/devices/i2c-*/name | xargs cat", "r")) != NULL) {
        ifp_number = popen("ls -r /sys/bus/i2c/devices/i2c-*/name | cut -d / -f 6 | cut -d - -f 2", "r");
        while (fgets(buf_cat, sizeof(buf_cat), ifp_cat) != NULL) {
            needleRet = strstr(buf_cat, needle);
            if ((fgets(buf_number, sizeof(buf_number), ifp_number) != NULL) && (needleRet != NULL)) {
                dev_num = atoi(buf_number);
                break;
            }
        }
    }
    return dev_num;
}

static UINT32 IoDiag_NewDevice(UINT32 SlaveAddr)
{
    UINT32 RetVal = I2C_ERR_NONE;
    char device[48];
    char cmd[96];
    INT8 dev_num;

    /* Check and parse i2c slave number */
    dev_num = IoDiag_ParseDevNum();

    /* Check and create i2c slave device */
    if (dev_num < 0) {
        fprintf(stderr,"%s %d, parse i2c slave number failed\n", __FUNCTION__, __LINE__);
        RetVal = I2C_ERR_UNEXPECTED;
    } else {
        sprintf(device, "/sys/bus/i2c/devices/i2c-%d/%d-10%02x", dev_num, dev_num, (SlaveAddr>>1));
        if (access(device, F_OK) != 0) {
            sprintf(cmd, "echo %s 0x10%02x > /sys/bus/i2c/devices/i2c-%d/new_device 2>/dev/null", SLAVE_DEV_NAME, (SlaveAddr>>1), dev_num);
            if (system(cmd) < 0) { }
        }
    }
    return RetVal;
}

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
    UINT8 *pTxBuf = DiagI2cDataBuf[DATA_BUF_IDX_M_W], *pRxBuf = DiagI2cDataBuf[DATA_BUF_IDX_M_R];
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

/**
 *  IoDiag_I2cPrepareSimpleTest - prepare for I2C simple read after write test
 *  @param[in] I2cChan I2C master ID
 *  @param[in] PrintFunc function to print messages on shell task
 *  @return error code
 *  NOTE: to match result as Thread MW API, write 0x0~0x6 behind 5 bytes before master read in simple test
 */
static void IoDiag_I2cPrepareSimpleTest(UINT32 I2cChan, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 ActualSize, RetVal = OK;

    /* TX */
    gSlaveTxCtrl.SlaveAddr = TEST_SLAVE_ADDR;
    gSlaveTxCtrl.DataSize = 8U;
    gSlaveTxCtrl.pDataBuf = DiagI2cDataBuf[DATA_BUF_IDX_S_W];
    gSlaveTxCtrl.pDataBuf[0U] = 0x05U; //[0] is reg, reserve 5 bytes for master write
    gSlaveTxCtrl.pDataBuf[1U] = 0x00U;
    gSlaveTxCtrl.pDataBuf[2U] = 0x01U;
    gSlaveTxCtrl.pDataBuf[3U] = 0x02U;
    gSlaveTxCtrl.pDataBuf[4U] = 0x03U;
    gSlaveTxCtrl.pDataBuf[5U] = 0x04U;
    gSlaveTxCtrl.pDataBuf[6U] = 0x05U;
    gSlaveTxCtrl.pDataBuf[7U] = 0x06U;

    RetVal = AmbaI2C_MasterWrite(I2cChan, DiagI2cSpeed[I2cChan], &gSlaveTxCtrl, &ActualSize, AMBA_KAL_WAIT_FOREVER);
    if (RetVal != OK) {
        IoDiag_PrintFormattedInt("Prepare Simple Test Fail (RetVal=0x%08X)\n", RetVal, PrintFunc);
    }
}

/**
 *  IoDiag_I2cSlaveLog - print
 *  @param[in] I2cChan I2C master ID
 *  @return error code
 */
static void IoDiag_I2cSlaveLog(UINT32 I2cChan, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i;
    AMBA_I2C_TRANSACTION_s SlaveRxCtrl;
    AMBA_I2C_TRANSACTION_s TxCtrl;
    UINT32 ActualSize;
    SlaveRxCtrl.SlaveAddr = TEST_SLAVE_ADDR | 0x1U;
    SlaveRxCtrl.DataSize = 5U;
    SlaveRxCtrl.pDataBuf = DiagI2cDataBuf[DATA_BUF_IDX_S_R];

    PrintFunc("[Diag][I2C] Slave is ready. Wait for transaction starts..\n");
    PrintFunc("[Diag][I2C] At least one transaction is done.\n");

    TxCtrl.SlaveAddr = TEST_SLAVE_ADDR;
    TxCtrl.DataSize = 1U;
    TxCtrl.pDataBuf = DiagI2cDataBuf[DATA_BUF_IDX_M_W];
    TxCtrl.pDataBuf[0U] = 0x00U; //[0] is reg

    AmbaI2C_MasterWrite(I2cChan, DiagI2cSpeed[I2cChan], &TxCtrl, &ActualSize, AMBA_KAL_WAIT_FOREVER); // Reset to reg=0
    AmbaI2C_MasterRead(I2cChan, DiagI2cSpeed[I2cChan], &SlaveRxCtrl, &SlaveRxCtrl.DataSize, AMBA_KAL_WAIT_FOREVER);
    for (i = 0U; i < SlaveRxCtrl.DataSize; i ++) {
        IoDiag_PrintFormattedInt("[Diag][I2C] Slave get data[%u]: ", i, PrintFunc);
        IoDiag_PrintFormattedInt("0x%02x\n", SlaveRxCtrl.pDataBuf[i], PrintFunc);
    }

    PrintFunc("[Diag][I2C] Transaction ends with Repeated-START condition.\n");

    for (i = 0U; i < gSlaveTxCtrl.DataSize; i ++) {
        IoDiag_PrintFormattedInt("[Diag][I2C] Slave put data[%u]: ", i, PrintFunc);
        IoDiag_PrintFormattedInt("0x%02x\n", gSlaveTxCtrl.pDataBuf[i], PrintFunc);
    }

    IoDiag_PrintFormattedInt("[Diag][I2C] Transaction ends with STOP condition. (Totally Rx %u bytes, ", SlaveRxCtrl.DataSize, PrintFunc);
    IoDiag_PrintFormattedInt("Tx %u bytes)\n", gSlaveTxCtrl.DataSize, PrintFunc);
}

/**
 *  IoDiag_I2cSimpleTest - I2C read after write test
 *  @param[in] I2cChan I2C master ID
 *  @param[in] PrintFunc function to print messages on shell task
 *  @return error code
 */
static void IoDiag_I2cSimpleTest(UINT32 I2cChan, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_I2C_TRANSACTION_s TxCtrl, RxCtrl;
    UINT32 NumTxTransaction = 1U, NumRxTransaction = 1U;
    UINT32 i, ActualSize, RetVal = OK;

    /* TX */
    TxCtrl.SlaveAddr = TEST_SLAVE_ADDR;
    TxCtrl.DataSize = 6U;
    TxCtrl.pDataBuf = DiagI2cDataBuf[DATA_BUF_IDX_M_W];
    TxCtrl.pDataBuf[0U] = 0x00U; //[0] is reg
    TxCtrl.pDataBuf[1U] = 0x11U;
    TxCtrl.pDataBuf[2U] = 0x22U;
    TxCtrl.pDataBuf[3U] = 0x33U;
    TxCtrl.pDataBuf[4U] = 0x44U;
    TxCtrl.pDataBuf[5U] = 0x55U;

    /* RX */
    RxCtrl.SlaveAddr = TEST_SLAVE_ADDR | 0x1U;
    RxCtrl.DataSize = 5U;
    RxCtrl.pDataBuf = DiagI2cDataBuf[DATA_BUF_IDX_M_R];

    if (NumRxTransaction != 0U) {
        RetVal = AmbaI2C_MasterReadAfterWrite(I2cChan, DiagI2cSpeed[I2cChan], NumTxTransaction, &TxCtrl, &RxCtrl, &ActualSize, AMBA_KAL_WAIT_FOREVER);

        /* Print Slave Log */
        IoDiag_I2cSlaveLog(I2cChan, PrintFunc);

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
    UINT32 I2cChan;

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
                        (void)IoDiag_NewDevice(TEST_SLAVE_ADDR);
                        (void)IoDiag_I2cPrepareSimpleTest(I2cChan, PrintFunc);
                        (void)IoDiag_I2cSimpleTest(I2cChan, PrintFunc);
                    } else if (AmbaUtility_StringCompare(pArgVector[3], "stop", 4U) == 0) {
                        // No need to implement in Linux
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

