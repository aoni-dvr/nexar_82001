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
#include <linux/spi/spidev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaSPI.h"

#define NUM_SPI_DEVICE (3)

typedef struct {
    UINT32 Mode;
    UINT32 Speed;
    UINT8  Bits;
    UINT32 MaxNumSlaveSelect;
} AMBA_SPI_CTRL_s;

static AMBA_SPI_CTRL_s AmbaSpiMasterCtrl[AMBA_NUM_SPI_MASTER] = {
    [AMBA_SPI_MASTER0] = {
        .Mode  = 0U,
        .Speed = 0U,
        .Bits  = 8U,
        .MaxNumSlaveSelect = 8U,
    },

    [AMBA_SPI_MASTER1] = {
        .Mode  = 0U,
        .Speed = 0U,
        .Bits  = 8U,
        .MaxNumSlaveSelect = 4U,
    },

    [AMBA_SPI_MASTER2] = {
        .Mode  = 0U,
        .Speed = 0U,
        .Bits  = 8U,
        .MaxNumSlaveSelect = 1U,
    },

    [AMBA_SPI_MASTER3] = {
        .Mode  = 0U,
        .Speed = 0U,
        .Bits  = 8U,
        .MaxNumSlaveSelect = 1U,
    },
};

static AMBA_SPI_CTRL_s AmbaSpiSlaveCtrl[AMBA_NUM_SPI_SLAVE] = {
    [AMBA_SPI_SLAVE] = {
        .Mode  = 0U,
        .Speed = 0U,
        .Bits  = 8U,
        .MaxNumSlaveSelect = 0U,            /* maximum Number of Slave Selects */
    },
};

static AMBA_KAL_MUTEX_t AmbaSpiMstMutex[AMBA_NUM_SPI_MASTER];
static AMBA_KAL_MUTEX_t AmbaSpiSlvMutex[AMBA_NUM_SPI_SLAVE];

static char *pMstDeviceName[AMBA_NUM_SPI_MASTER][NUM_SPI_DEVICE] = {
    {"/dev/spidev0.0", "/dev/spidev0.1", "/dev/spidev0.2"},
    {"/dev/spidev1.0", "/dev/spidev1.1", "/dev/spidev1.2"},
    {"/dev/spidev2.0", "/dev/spidev2.1", "/dev/spidev2.2"},
    {"/dev/spidev3.0", "/dev/spidev3.1", "/dev/spidev3.2"},
};
static char *pSlvDeviceName[AMBA_NUM_SPI_SLAVE] = {
    "/dev/slavespi"
};

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

    if (system("modprobe spidev 2>/dev/null") < 0) { }
    if (system("modprobe spi-ambarella 2>/dev/null") < 0) { }
    if (system("modprobe spi-slave-ambarella 2>/dev/null") < 0) { }

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
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_MasterTransferD8(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                                UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    UINT32 RetVal = SPI_ERR_NONE;
    int ret = 0;
    int fd;
    struct spi_ioc_transfer tr = {0};
    UINT8 SlaveNum;

    /* parameters pActualSize and TimeOut are not supported in Linux SDK */
    (void) pActualSize;
    (void) TimeOut;

    for (SlaveNum = 0U; SlaveNum < NUM_SPI_DEVICE; SlaveNum++) {
        if (((1U << SlaveNum) & SlaveMask) != 0U) {
            break;
        }
    }

    if ((MasterID >= AMBA_NUM_SPI_MASTER) || (pConfig == NULL) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL)) || (SlaveNum >= NUM_SPI_DEVICE)) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaSpiMstMutex[MasterID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else {
            fd = open(pMstDeviceName[MasterID][SlaveNum], O_RDWR);
            if (fd == -1) {
                fprintf(stderr, "%s %d, open device %s failed\n", __FUNCTION__, __LINE__, pMstDeviceName[MasterID][SlaveNum]);
                RetVal = SPI_ERR_OPEN_FILE;
            } else {
                /* Config SPI mode */
                switch(pConfig->ClkMode) {
                case AMBA_SPI_CPOL_LOW_CPHA_LOW:
                    AmbaSpiMasterCtrl[MasterID].Mode = SPI_MODE_0;
                    break;
                case AMBA_SPI_CPOL_LOW_CPHA_HIGH:
                    AmbaSpiMasterCtrl[MasterID].Mode = SPI_MODE_1;
                    break;
                case AMBA_SPI_CPOL_HIGH_CPHA_LOW:
                    AmbaSpiMasterCtrl[MasterID].Mode = SPI_MODE_2;
                    break;
                case AMBA_SPI_CPOL_HIGH_CPHA_HIGH:
                default:
                    AmbaSpiMasterCtrl[MasterID].Mode = SPI_MODE_3;
                    break;
                };

                if(pConfig->CsPolarity == AMBA_SPI_CHIP_SELECT_ACTIVE_HIGH) {
                    AmbaSpiMasterCtrl[MasterID].Mode |= SPI_CS_HIGH;
                }

                if(pConfig->FrameBitOrder == AMBA_SPI_TRANSFER_LSB_FIRST) {
                    AmbaSpiMasterCtrl[MasterID].Mode |= SPI_LSB_FIRST;
                }

                ret = ioctl(fd, SPI_IOC_WR_MODE, &AmbaSpiMasterCtrl[MasterID].Mode);
                if (ret == -1) {
                    printf("Set SPI mode fail: %d\n", ret);
                    RetVal = SPI_ERR_DEV_CTL;
                }

                /* Config SPI bits per word */
                AmbaSpiMasterCtrl[MasterID].Bits = (UINT8)pConfig->DataFrameSize;
                ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &AmbaSpiMasterCtrl[MasterID].Bits);
                if (ret == -1) {
                    printf("Set SPI bits per word fail: %d\n", ret);
                    RetVal = SPI_ERR_DEV_CTL;
                }

                /* Config SPI speed */
                AmbaSpiMasterCtrl[MasterID].Speed = pConfig->BaudRate;
                ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &AmbaSpiMasterCtrl[MasterID].Speed);
                if (ret == -1) {
                    printf("Set SPI speed fail: %d\n", ret);
                    RetVal = SPI_ERR_DEV_CTL;
                }

                /* SPI transfer */
                tr.tx_buf = (uintptr_t)pTxBuf;
                tr.rx_buf = (uintptr_t)pRxBuf;
                tr.len = BufSize;
                tr.speed_hz = AmbaSpiMasterCtrl[MasterID].Speed;
                tr.bits_per_word = AmbaSpiMasterCtrl[MasterID].Bits;

                ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
                if (ret == -1) {
                    printf("SPI transfer fail: %d\n", ret);
                    RetVal = SPI_ERR_DEV_CTL;
                }

                close(fd);
            }

            if (AmbaKAL_MutexGive(&AmbaSpiMstMutex[MasterID]) != KAL_ERR_NONE) {
                RetVal = SPI_ERR_MUTEX;
            }
        }
    }

    return RetVal;
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
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_MasterTransferD16(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                                 UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    UINT32 RetVal = SPI_ERR_NONE;
    int ret = 0;
    int fd;
    struct spi_ioc_transfer tr = {0};
    UINT8 SlaveNum = 0U;

    /* parameters pActualSize and TimeOut are not supported in Linux SDK */
    (void) pActualSize;
    (void) TimeOut;

    for (SlaveNum = 0U; SlaveNum < NUM_SPI_DEVICE; SlaveNum++) {
        if (((1U << SlaveNum) & SlaveMask) != 0U) {
            break;
        }
    }

    if ((MasterID >= AMBA_NUM_SPI_MASTER) || (pConfig == NULL) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL)) || (SlaveNum >= NUM_SPI_DEVICE)) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaSpiMstMutex[MasterID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else {
            fd = open(pMstDeviceName[MasterID][SlaveNum], O_RDWR);
            if (fd == -1) {
                fprintf(stderr, "%s %d, open device %s failed\n", __FUNCTION__, __LINE__, pMstDeviceName[MasterID][SlaveNum]);
                RetVal = SPI_ERR_OPEN_FILE;
            } else {
                /* Config SPI mode */
                switch(pConfig->ClkMode) {
                case AMBA_SPI_CPOL_LOW_CPHA_LOW:
                    AmbaSpiMasterCtrl[MasterID].Mode = SPI_MODE_0;
                    break;
                case AMBA_SPI_CPOL_LOW_CPHA_HIGH:
                    AmbaSpiMasterCtrl[MasterID].Mode = SPI_MODE_1;
                    break;
                case AMBA_SPI_CPOL_HIGH_CPHA_LOW:
                    AmbaSpiMasterCtrl[MasterID].Mode = SPI_MODE_2;
                    break;
                case AMBA_SPI_CPOL_HIGH_CPHA_HIGH:
                default:
                    AmbaSpiMasterCtrl[MasterID].Mode = SPI_MODE_3;
                    break;
                };

                if(pConfig->CsPolarity == AMBA_SPI_CHIP_SELECT_ACTIVE_HIGH) {
                    AmbaSpiMasterCtrl[MasterID].Mode |= SPI_CS_HIGH;
                }

                if(pConfig->FrameBitOrder == AMBA_SPI_TRANSFER_LSB_FIRST) {
                    AmbaSpiMasterCtrl[MasterID].Mode |= SPI_LSB_FIRST;
                }

                ret = ioctl(fd, SPI_IOC_WR_MODE, &AmbaSpiMasterCtrl[MasterID].Mode);
                if (ret == -1) {
                    printf("Set SPI mode fail: %d\n", ret);
                    RetVal = SPI_ERR_DEV_CTL;
                }

                /* Config SPI bits per word */
                AmbaSpiMasterCtrl[MasterID].Bits = (UINT8)pConfig->DataFrameSize;
                ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &AmbaSpiMasterCtrl[MasterID].Bits);
                if (ret == -1) {
                    printf("Set SPI bits per word fail: %d\n", ret);
                    RetVal = SPI_ERR_DEV_CTL;
                }

                /* Config SPI speed */
                AmbaSpiMasterCtrl[MasterID].Speed = pConfig->BaudRate;
                ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &AmbaSpiMasterCtrl[MasterID].Speed);
                if (ret == -1) {
                    printf("Set SPI speed fail: %d\n", ret);
                    RetVal = SPI_ERR_DEV_CTL;
                }

                /* SPI transfer */
                tr.tx_buf = (uintptr_t)pTxBuf;
                tr.rx_buf = (uintptr_t)pRxBuf;
                tr.len = BufSize * 2U;   /* unit of u16 */
                tr.speed_hz = AmbaSpiMasterCtrl[MasterID].Speed;
                tr.bits_per_word = AmbaSpiMasterCtrl[MasterID].Bits;
                ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
                if (ret == -1) {
                    printf("SPI transfer fail: %d\n", ret);
                    RetVal = SPI_ERR_DEV_CTL;
                }

                close(fd);
            }

            if (AmbaKAL_MutexGive(&AmbaSpiMstMutex[MasterID]) != KAL_ERR_NONE) {
                RetVal = SPI_ERR_MUTEX;
            }
        }
    }

    return RetVal;
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
    UINT32 RetVal = SPI_ERR_NONE;

    /* parameter pStatus is not supported in Linux SDK */
    (void)pStatus;

    if ((MasterID >= AMBA_NUM_SPI_MASTER) || (pNumSlaves == NULL)) {
        RetVal = SPI_ERR_ARG;
    } else {
        *pNumSlaves = AmbaSpiMasterCtrl[MasterID].MaxNumSlaveSelect;
    }

    return RetVal;
}

/**
 *  AmbaSPI_SlaveConfig - The function setup the configurations for slave.
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] pConfig Define the combinations of polarity and phases for transitions where data should be latched
 *  @return error code
 */
UINT32 AmbaSPI_SlaveConfig(UINT32 SlaveID, const AMBA_SPI_CONFIG_s * pConfig)
{
    UINT32 RetVal = SPI_ERR_NONE;

    if ((SlaveID >= AMBA_NUM_SPI_SLAVE) || (pConfig == NULL)) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaSpiSlvMutex[SlaveID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else {
            /* Config SPI mode */
            switch(pConfig->ClkMode) {
            case AMBA_SPI_CPOL_LOW_CPHA_LOW:
                AmbaSpiSlaveCtrl[SlaveID].Mode = SPI_MODE_0;
                fprintf(stderr, "%s(%d): mode0 is not supported\n", __func__, __LINE__);
                break;
            case AMBA_SPI_CPOL_LOW_CPHA_HIGH:
                AmbaSpiSlaveCtrl[SlaveID].Mode = SPI_MODE_1;
                break;
            case AMBA_SPI_CPOL_HIGH_CPHA_LOW:
                AmbaSpiSlaveCtrl[SlaveID].Mode = SPI_MODE_2;
                fprintf(stderr, "%s(%d): mode2 is not supported\n", __func__, __LINE__);
                break;
            case AMBA_SPI_CPOL_HIGH_CPHA_HIGH:
            default:
                AmbaSpiSlaveCtrl[SlaveID].Mode = SPI_MODE_3;
                break;
            };

            if(pConfig->CsPolarity == AMBA_SPI_CHIP_SELECT_ACTIVE_HIGH) {
                AmbaSpiSlaveCtrl[SlaveID].Mode |= SPI_CS_HIGH;
            }

            if(pConfig->FrameBitOrder == AMBA_SPI_TRANSFER_LSB_FIRST) {
                AmbaSpiSlaveCtrl[SlaveID].Mode |= SPI_LSB_FIRST;
            }

            /* Config SPI bits per word */
            AmbaSpiSlaveCtrl[SlaveID].Bits = (UINT8)pConfig->DataFrameSize;

            /* Config SPI speed */
            AmbaSpiSlaveCtrl[SlaveID].Speed = pConfig->BaudRate;

            if (AmbaKAL_MutexGive(&AmbaSpiSlvMutex[SlaveID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SPI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
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
    UINT32 RetVal = SPI_ERR_NONE;
    struct spi_ioc_transfer tr = {0}; /*linux/include/uapi/linux/spi/spidev.h*/
    int ret = 0;
    int fd;

    /* pActualSize not supported in Linux SDK */
    if (pActualSize != NULL) {
        *pActualSize = BufSize;
    }

    if ((SlaveID >= AMBA_NUM_SPI_SLAVE) || (BufSize == 0U) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaSpiSlvMutex[SlaveID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else {
            fd = open(pSlvDeviceName[SlaveID], O_RDWR);
            if (fd == -1) {
                fprintf(stderr, "%s %d, open device %s failed\n", __FUNCTION__, __LINE__, pSlvDeviceName[SlaveID]);
                RetVal = SPI_ERR_OPEN_FILE;
            } else {
                /* Config SPI mode */
                ret = ioctl(fd, SPI_IOC_WR_MODE, &AmbaSpiSlaveCtrl[SlaveID].Mode);
                if (ret == -1) {
                    printf("Set SPI mode fail: %d\n", ret);
                    RetVal = SPI_ERR_DEV_CTL;
                }

                /* Config SPI bits per word */
                ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &AmbaSpiSlaveCtrl[SlaveID].Bits);
                if (ret == -1) {
                    printf("Set SPI bits per word fail: %d\n", ret);
                    RetVal = SPI_ERR_DEV_CTL;
                }

                /* Config SPI speed */
                ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &AmbaSpiSlaveCtrl[SlaveID].Speed);
                if (ret == -1) {
                    printf("Set SPI speed fail: %d\n", ret);
                    RetVal = SPI_ERR_DEV_CTL;
                }

                /* SPI transfer */
                tr.tx_buf = (uintptr_t)pTxBuf;
                tr.rx_buf = (uintptr_t)pRxBuf;
                tr.len = BufSize;
                tr.speed_hz = AmbaSpiSlaveCtrl[SlaveID].Speed;
                tr.bits_per_word = AmbaSpiSlaveCtrl[SlaveID].Bits;
                tr.delay_usecs = TimeOut;  /*return after delay_usecs milliseconds even though the SPI transfer not done*/

                ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
                if (ret == -1) {
                    printf("SPI transfer fail: %d\n", ret);
                    RetVal = SPI_ERR_DEV_CTL;
                }

                close(fd);
            }

            if (AmbaKAL_MutexGive(&AmbaSpiSlvMutex[SlaveID]) != KAL_ERR_NONE) {
                RetVal = SPI_ERR_MUTEX;
            }
        }
    }

    return RetVal;
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
    UINT32 RetVal;

    RetVal = AmbaSPI_SlaveTransferD8(SlaveID, BufSize*2, (const UINT8 *)pTxBuf, (UINT8 *)pRxBuf, pActualSize, TimeOut);

    return RetVal;
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


