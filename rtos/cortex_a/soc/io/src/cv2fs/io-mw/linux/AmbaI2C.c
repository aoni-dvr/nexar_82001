/**
 *  @file AmbaI2C.c
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
 *  @details I2C APIs
 *
 */
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include "Generic.h"

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaI2C.h"

static AMBA_KAL_MUTEX_t AmbaI2cMasterMutex[AMBA_NUM_I2C_CHANNEL];
static char *pDeviceName[AMBA_NUM_I2C_CHANNEL] = {
    "/dev/i2c-0",
    "/dev/i2c-1",
    "/dev/i2c-2",
    "/dev/i2c-3"
};

static UINT32 I2cWrite(UINT32 MasterID, UINT32 BusSpeed, const AMBA_I2C_TRANSACTION_s *pTransaction)
{
    int fd;
    int rval;
    UINT32 RetVal = I2C_ERR_NONE;
    unsigned Speed = 0;
    struct i2c_msg msgs;
    struct i2c_rdwr_ioctl_data rdwr;

    fd = open(pDeviceName[MasterID], O_RDWR);
    if (fd == -1) {
        fprintf(stderr,"%s %d, open device %s failed\n",__FUNCTION__, __LINE__,pDeviceName[MasterID]);
        RetVal = I2C_ERR_UNEXPECTED;
    } else {
        switch (BusSpeed) {
        case I2C_SPEED_STANDARD:
            Speed = 100000U;
            break;
        case I2C_SPEED_FAST:
            Speed = 400000U;
            break;
        case I2C_SPEED_FAST_PLUS:
            Speed = 1000000U;
            break;
        case I2C_SPEED_HIGH:
            Speed = 3400000;
            break;
        default:
            Speed = 0U;
            break;
        }
        (void) Speed;

        msgs.addr = (pTransaction->SlaveAddr >> 1U);
        msgs.flags = 0;
        msgs.len = pTransaction->DataSize;
        msgs.buf = pTransaction->pDataBuf;
        rdwr.msgs = &msgs;
        rdwr.nmsgs = 1U;
        rval = ioctl(fd, I2C_RDWR, &rdwr);
        if (rval < 0) {
            printf("%s %d, ioctl I2C_RDWR error\n",__FUNCTION__, __LINE__);
            RetVal = I2C_ERR_UNEXPECTED;
        } else if ((UINT32)rval < rdwr.nmsgs) {
            printf("%s %d, only %d/%d messages were sent\n",__FUNCTION__, __LINE__, rval, rdwr.nmsgs);
            RetVal = I2C_ERR_UNEXPECTED;
        }

        close(fd);
    }

    return RetVal;
}

static UINT32 I2cRead(UINT32 MasterID, UINT32 BusSpeed, const AMBA_I2C_TRANSACTION_s* pTransaction)
{
    int fd;
    int rval;
    UINT32 RetVal = I2C_ERR_NONE;
    unsigned Speed = 0;
    struct i2c_msg msgs;
    struct i2c_rdwr_ioctl_data rdwr;

    fd = open(pDeviceName[MasterID], O_RDWR);
    if (fd == -1) {
        RetVal = I2C_ERR_UNEXPECTED;
    } else {
        switch (BusSpeed) {
        case I2C_SPEED_STANDARD:
            Speed = 100000U;
            break;
        case I2C_SPEED_FAST:
            Speed = 400000U;
            break;
        case I2C_SPEED_FAST_PLUS:
            Speed = 1000000U;
            break;
        case I2C_SPEED_HIGH:
            Speed = 3400000;
            break;
        default:
            Speed = 0U;
            break;
        }
        (void) Speed;

        msgs.addr = (pTransaction->SlaveAddr >> 1U);
        msgs.flags = I2C_M_RD;
        msgs.len = pTransaction->DataSize;
        msgs.buf = pTransaction->pDataBuf;
        rdwr.msgs = &msgs;
        rdwr.nmsgs = 1U;
        rval = ioctl(fd, I2C_RDWR, &rdwr);
        if (rval < 0) {
            printf("%s %d, ioctl I2C_RDWR error\n",__FUNCTION__, __LINE__);
            RetVal = I2C_ERR_UNEXPECTED;
        } else if ((UINT32)rval < rdwr.nmsgs) {
            printf("%s %d, only %d/%d messages were sent\n",__FUNCTION__, __LINE__, rval, rdwr.nmsgs);
            RetVal = I2C_ERR_UNEXPECTED;
        }

        close(fd);
    }

    return RetVal;
}

static UINT32 I2cReadAfterWrite(UINT32 MasterID, UINT32 BusSpeed, const AMBA_I2C_TRANSACTION_s* pTxTransaction, const AMBA_I2C_TRANSACTION_s* pRxTransaction)
{
    int fd;
    int rval;
    UINT32 RetVal = I2C_ERR_NONE;
    unsigned Speed = 0;
    struct i2c_msg msgs[2U];
    struct i2c_rdwr_ioctl_data rdwr;

    fd = open(pDeviceName[MasterID], O_RDWR);
    if (fd == -1) {
        RetVal = I2C_ERR_UNEXPECTED;
    } else {
        switch (BusSpeed) {
        case I2C_SPEED_STANDARD:
            Speed = 100000U;
            break;
        case I2C_SPEED_FAST:
            Speed = 400000U;
            break;
        case I2C_SPEED_FAST_PLUS:
            Speed = 1000000U;
            break;
        case I2C_SPEED_HIGH:
            Speed = 3400000;
            break;
        default:
            Speed = 0U;
            break;
        }
        (void) Speed;

        msgs[0U].addr = (pTxTransaction->SlaveAddr >> 1U);
        msgs[0U].flags = 0;
        msgs[0U].len = pTxTransaction->DataSize;
        msgs[0U].buf = pTxTransaction->pDataBuf;
        msgs[1U].addr = (pTxTransaction->SlaveAddr >> 1U);
        msgs[1U].flags = I2C_M_RD;
        msgs[1U].len = pRxTransaction->DataSize;
        msgs[1U].buf = pRxTransaction->pDataBuf;
        rdwr.msgs = msgs;
        rdwr.nmsgs = 2U;
        rval = ioctl(fd, I2C_RDWR, &rdwr);
        if (rval < 0) {
            printf("%s %d, ioctl I2C_RDWR error\n",__FUNCTION__, __LINE__);
            RetVal = I2C_ERR_UNEXPECTED;
        } else if ((UINT32)rval < rdwr.nmsgs) {
            printf("%s %d, only %d/%d messages were sent\n",__FUNCTION__, __LINE__, rval, rdwr.nmsgs);
            RetVal = I2C_ERR_UNEXPECTED;
        }

        close(fd);
    }

    return RetVal;
}

/**
 *  AmbaI2C_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaI2C_DrvEntry(void)
{
    static char AmbaI2cMasterMutexName[20] = "AmbaI2cMasterMutex";

    UINT32 i, RetVal = I2C_ERR_NONE;

    /* Create Mutex */
    for (i = 0U; i < AMBA_NUM_I2C_CHANNEL; i++) {
        if (AmbaKAL_MutexCreate(&AmbaI2cMasterMutex[i], AmbaI2cMasterMutexName) != KAL_ERR_NONE) {
            RetVal = I2C_ERR_UNEXPECTED;
            break;
        }
    }

    return RetVal;
}

/**
 *  AmbaI2C_MasterWrite - I2C Master write data operation
 *  @param[in] MasterID Indicate an I2C master
 *  @param[in] BusSpeed Bus speed mode with preset timing parameters
 *  @param[in] pTxTransaction Pointer to TX transaction
 *  @param[out] pActualTxSize The actual number of transmitted data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaI2C_MasterWrite(UINT32 MasterID, UINT32 BusSpeed, const AMBA_I2C_TRANSACTION_s *pTxTransaction, UINT32 *pActualTxSize, UINT32 TimeOut)
{
    UINT32 RetVal = I2C_ERR_NONE;

    if ((MasterID >= AMBA_NUM_I2C_CHANNEL) ||
        (pTxTransaction == NULL) || (pTxTransaction->DataSize == 0U) || (pTxTransaction->pDataBuf == NULL)) {
        RetVal = I2C_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaI2cMasterMutex[MasterID], TimeOut) != KAL_ERR_NONE) {
            RetVal = I2C_ERR_MUTEX;
        } else {
            RetVal = I2cWrite(MasterID, BusSpeed, pTxTransaction);

            if ((pActualTxSize != NULL) && (RetVal == I2C_ERR_NONE)) {
                *pActualTxSize = pTxTransaction->DataSize;
            }

            if (AmbaKAL_MutexGive(&AmbaI2cMasterMutex[MasterID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = I2C_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaI2C_MasterRead - I2C Master read data operation
 *  @param[in] MasterID Indicate an I2C master
 *  @param[in] BusSpeed Bus speed mode with preset timing parameters
 *  @param[in] pRxransaction Pointer to RX transaction
 *  @param[out] pActualRxSize The actual number of recieved data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaI2C_MasterRead(UINT32 MasterID, UINT32 BusSpeed, const AMBA_I2C_TRANSACTION_s *pRxTransaction, UINT32 *pActualRxSize, UINT32 TimeOut)
{
    UINT32 RetVal = I2C_ERR_NONE;

    if ((MasterID >= AMBA_NUM_I2C_CHANNEL) ||
        (pRxTransaction == NULL) || (pRxTransaction->DataSize == 0U) || (pRxTransaction->pDataBuf == NULL)) {
        RetVal = I2C_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaI2cMasterMutex[MasterID], TimeOut) != KAL_ERR_NONE) {
            RetVal = I2C_ERR_MUTEX;
        } else {
            RetVal = I2cRead(MasterID, BusSpeed, pRxTransaction);

            if ((pActualRxSize != NULL) && (RetVal == I2C_ERR_NONE)) {
                *pActualRxSize = pRxTransaction->DataSize;
            }

            if (AmbaKAL_MutexGive(&AmbaI2cMasterMutex[MasterID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = I2C_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaI2C_MasterReadAfterWrite - I2C Master write and then read data operation
 *  @param[in] MasterID Indicate an I2C master
 *  @param[in] BusSpeed Bus speed mode with preset timing parameters
 *  @param[in] NumTxTransaction Number of TX transactions
 *  @param[in] pTxTransaction Pointer to TX transaction
 *  @param[in] pRxransaction Pointer to RX transaction
 *  @param[out] pActualSize The actual number of rx/tx data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaI2C_MasterReadAfterWrite(UINT32 MasterID, UINT32 BusSpeed, UINT32 NumTxTransaction, const AMBA_I2C_TRANSACTION_s * pTxTransaction, const AMBA_I2C_TRANSACTION_s * pRxTransaction, UINT32 * pActualSize, UINT32 TimeOut)
{
    UINT32 RetVal = I2C_ERR_NONE;

    if ((MasterID >= AMBA_NUM_I2C_CHANNEL) || (NumTxTransaction == 0U) ||
        (pTxTransaction == NULL) || (pTxTransaction->DataSize == 0U) || (pTxTransaction->pDataBuf == NULL) ||
        (pRxTransaction == NULL) || (pRxTransaction->DataSize == 0U) || (pRxTransaction->pDataBuf == NULL)) {
        RetVal = I2C_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaI2cMasterMutex[MasterID], TimeOut) != KAL_ERR_NONE) {
            RetVal = I2C_ERR_MUTEX;
        } else {
            RetVal = I2cReadAfterWrite(MasterID, BusSpeed, pTxTransaction, pRxTransaction);

            if ((pActualSize != NULL) && (RetVal == I2C_ERR_NONE)) {
                *pActualSize = pRxTransaction->DataSize;
            }

            if (AmbaKAL_MutexGive(&AmbaI2cMasterMutex[MasterID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = I2C_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaI2C_MasterReadVarLength - I2C Master read Variable Length of data operation
 *  @param[in] MasterID Indicate an I2C master
 *  @param[in] BusSpeed Bus speed mode with preset timing parameters
 *  @param[in] VarLenInfo Number of bytes used to indicate the variable length information
 *  @param[in] pRxransaction Pointer to RX transaction
 *  @param[out] pActualSize The actual number of rx data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaI2C_MasterReadVarLength(UINT32 MasterID, UINT32 BusSpeed, UINT32 VarLenInfo, const AMBA_I2C_TRANSACTION_s *pRxTransaction, UINT32 *pActualRxSize, UINT32 TimeOut)
{
    UINT32 RetVal = I2C_ERR_NONE;

    (void)MasterID;
    (void)BusSpeed;
    (void)pRxTransaction;
    (void)VarLenInfo;
    (void)pActualRxSize;
    (void)TimeOut;

    return RetVal;
}

/**
 *  AmbaI2C_MasterGetInfo - I2C Master get actual transimission bitrate
 *  @param[in] MasterID Indicate an I2C master
 *  @param[in] BusSpeed Bus speed mode with preset timing parameters
 *  @param[out] pActualBitRate The actual transmission bitrate
 *  @return error code
 */
UINT32 AmbaI2C_MasterGetInfo(UINT32 MasterID, UINT32 BusSpeed, UINT32 * pActualBitRate)
{
    UINT32 RetVal = I2C_ERR_NONE;

    (void)MasterID;
    (void)BusSpeed;
    (void)pActualBitRate;

    return RetVal;
}

/**
 *  AmbaI2C_SlaveStart - Enable an I2C slave
 *  @param[in] SlaveID Indicate an I2C slave
 *  @param[in] WriteIntFunc ISR when slave detects a slave address for master read
 *  @param[in] ReadIntFunc ISR when slave detects a slave address for master write
 *  @param[in] BreakIntFunc ISR when either STOP or repeated-START condition is detected
 *  @return error code
 */
UINT32 AmbaI2C_SlaveStart(UINT32 SlaveID, UINT32 SlaveAddr, AMBA_I2C_SLAVE_WRITE_ISR_f WriteIntFunc, AMBA_I2C_SLAVE_READ_ISR_f ReadIntFunc, AMBA_I2C_SLAVE_BREAK_ISR_f BreakIntFunc)
{
    UINT32 RetVal = I2C_ERR_NONE;

    (void)SlaveID;
    (void)SlaveAddr;
    (void)WriteIntFunc;
    (void)ReadIntFunc;
    (void)BreakIntFunc;

    return RetVal;
}

/**
 *  AmbaI2C_SlaveStop - Disable an I2C slave
 *  @param[in] SlaveID Indicate an I2C slave
 *  @return error code
 */
UINT32 AmbaI2C_SlaveStop(UINT32 SlaveID)
{
    UINT32 RetVal = I2C_ERR_NONE;

    (void)SlaveID;

    return RetVal;
}

