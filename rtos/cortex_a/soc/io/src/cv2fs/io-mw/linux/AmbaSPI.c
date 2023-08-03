/**
 *  @file AmbaSPI.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details SPI APIs
 *
 */

#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <linux/spi/spi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaSPI.h"
#include "AmbaDMA.h"

static AMBA_KAL_MUTEX_t AmbaSpiMstMutex[AMBA_NUM_SPI_MASTER];
static AMBA_KAL_MUTEX_t AmbaSpiSlvMutex[AMBA_NUM_SPI_SLAVE];

/**
 *  AmbaSPI_DrvEntry - SPI device driver initializations
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaSPI_DrvEntry(void)
{
    static char AmbaSpiMstMutexName[20] = "AmbaSpiMstMutex";
    static char AmbaSpiSlvMutexName[20] = "AmbaSpiSlvMutex";
    UINT32 i, RetVal = SPI_ERR_NONE;

    for (i = 0U; i < AMBA_NUM_SPI_MASTER; i++) {
        if (AmbaKAL_MutexCreate(&AmbaSpiMstMutex[i], AmbaSpiMstMutexName) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
            break;
        }
    }

    for (i = 0U; i < AMBA_NUM_SPI_SLAVE; i++) {
        if (AmbaKAL_MutexCreate(&AmbaSpiSlvMutex[i], AmbaSpiSlvMutexName) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        }
    }

    return RetVal;
}

/**
 *  AmbaSPI_MasterTransferD8 - SPI Master write and read data operation
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] SlaveMask Indicate active slave devices of the master
 *  @param[in] pConfig Define the combinations of polarity and phases for transitions where data should be latched.
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut: The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_MasterTransferD8(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                                UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    (void)MasterID;
    (void)SlaveMask;
    (void)pConfig;
    (void)BufSize;
    (void)pTxBuf;
    (void)pRxBuf;
    (void)pActualSize;
    (void)TimeOut;

    printf("%s not implemented!\n", __func__);

    return SPI_ERR_NONE;
}

/**
 *  AmbaSPI_MasterTransferD16 - SPI Master write and read data operation
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] SlaveMask Indicate active slave devices of the master
 *  @param[in] pConfig Define the combinations of polarity and phases for transitions where data should be latched.
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut: The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_MasterTransferD16(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                                 UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    (void)MasterID;
    (void)SlaveMask;
    (void)pConfig;
    (void)BufSize;
    (void)pTxBuf;
    (void)pRxBuf;
    (void)pActualSize;
    (void)TimeOut;

    printf("%s not implemented!\n", __func__);

    return SPI_ERR_NONE;
}

/**
 *  AmbaSPI_MasterDmaTransferD8 - SPI Master write and read data operation with Dma
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] SlaveMask Indicate active slave devices of the master
 *  @param[in] pConfig Define the combinations of polarity and phases for transitions where data should be latched.
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_MasterDmaTransferD8(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                                   UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    (void)MasterID;
    (void)SlaveMask;
    (void)pConfig;
    (void)BufSize;
    (void)pTxBuf;
    (void)pRxBuf;
    (void)pActualSize;
    (void)TimeOut;

    printf("%s not implemented!\n", __func__);

    return SPI_ERR_NONE;
}

/**
 *  AmbaSPI_MasterDmaTransferD16 - SPI Master write and read data operation with Dma
 *  @param[in] MasterID Indicate a SPI master
 *  @param[in] SlaveMask Indicate active slave devices of the master
 *  @param[in] pConfig Define the combinations of polarity and phases for transitions where data should be latched.
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_MasterDmaTransferD16(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                                    UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    (void)MasterID;
    (void)SlaveMask;
    (void)pConfig;
    (void)BufSize;
    (void)pTxBuf;
    (void)pRxBuf;
    (void)pActualSize;
    (void)TimeOut;

    printf("%s not implemented!\n", __func__);

    return SPI_ERR_NONE;
}

/**
 *  AmbaSPI_MasterGetInfo - The function returns all the information about the master.
 *  @param[in] MasterID Indicate a SPI master
 *  @param[out] pNumSlaves Number of slave support of the master
 *  @param[out] pStatus The current bus status
 *  @return error code
 */
UINT32 AmbaSPI_MasterGetInfo(UINT32 MasterID, UINT32 *pNumSlaves, UINT32 *pStatus)
{
    (void)MasterID;
    (void)pNumSlaves;
    (void)pStatus;

    printf("%s not implemented!\n", __func__);

    return SPI_ERR_NONE;
}

/**
 *  AmbaSPI_SlaveConfig - The function setup the configurations for slave.
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] pConfig Define the combinations of polarity and phases for transitions where data should be latched
 *  @return error code
 */
UINT32 AmbaSPI_SlaveConfig(UINT32 SlaveID, const AMBA_SPI_CONFIG_s * pConfig)
{
    (void)SlaveID;
    (void)pConfig;

    printf("%s not implemented!\n", __func__);

    return SPI_ERR_NONE;
}

/**
 *  AmbaSPI_SlaveTransferD8 - SPI Slave write and read data operation
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_SlaveTransferD8(UINT32 SlaveID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    (void)SlaveID;
    (void)BufSize;
    (void)pTxBuf;
    (void)pRxBuf;
    (void)pActualSize;
    (void)TimeOut;

    printf("%s not implemented!\n", __func__);

    return SPI_ERR_NONE;
}

/**
 *  AmbaSPI_SlaveTransferD16 - SPI Slave write and read data operation
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_SlaveTransferD16(UINT32 SlaveID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    (void)SlaveID;
    (void)BufSize;
    (void)pTxBuf;
    (void)pRxBuf;
    (void)pActualSize;
    (void)TimeOut;

    printf("%s not implemented!\n", __func__);

    return SPI_ERR_NONE;
}

/**
 *  AmbaSPI_SlaveDmaTransferD8 - SPI Slave write and read data operation with Dma
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_SlaveDmaTransferD8(UINT32 SlaveID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    (void)SlaveID;
    (void)BufSize;
    (void)pTxBuf;
    (void)pRxBuf;
    (void)pActualSize;
    (void)TimeOut;

    printf("%s not implemented!\n", __func__);

    return SPI_ERR_NONE;
}

/**
 *  AmbaSPI_SlaveDmaTransferD16 - SPI Slave write and read data operation with Dma
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_SlaveDmaTransferD16(UINT32 SlaveID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    (void)SlaveID;
    (void)BufSize;
    (void)pTxBuf;
    (void)pRxBuf;
    (void)pActualSize;
    (void)TimeOut;

    printf("%s not implemented!\n", __func__);

    return SPI_ERR_NONE;
}

/**
 *  AmbaSPI_SlaveGetInfo - The function returns all the information about the slave.
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[out] pStatus The current bus status
 *  @return error code
 */
UINT32 AmbaSPI_SlaveGetInfo(UINT32 SlaveID, UINT32 *pStatus)
{
    (void)SlaveID;
    (void)pStatus;

    printf("%s not implemented!\n", __func__);

    return SPI_ERR_NONE;
}

