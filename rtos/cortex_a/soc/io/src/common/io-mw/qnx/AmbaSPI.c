/*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include <fcntl.h>
#include "hw/ambarella_spi.h"


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
            RetVal = SPI_ERR_UNEXPECTED;
            break;
        }
    }

    for (i = 0U; i < AMBA_NUM_SPI_SLAVE; i++) {
        if (AmbaKAL_MutexCreate(&AmbaSpiSlvMutex[i], AmbaSpiSlvMutexName) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_UNEXPECTED;
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
    UINT32 RetVal = SPI_ERR_NONE;
    int fd, err;
    mst_xfer_d8_msg_t MstXferD8Msg;
    unsigned int BufIdx = 0U;


    if ((MasterID >= AMBA_NUM_SPI_MASTER) || (BufSize == 0U) || (BufSize > AMBA_SPI_MASTER_FIFO_COUNT) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {

        if (AmbaKAL_MutexTake(&AmbaSpiMstMutex[MasterID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else {

            switch (MasterID) {
            case AMBA_SPI_MASTER0:
                fd = open("/dev/spi0", O_RDWR);
                break;
            case AMBA_SPI_MASTER1:
                fd = open("/dev/spi1", O_RDWR);
                break;
            case AMBA_SPI_MASTER2:
                fd = open("/dev/spi2", O_RDWR);
                break;
            case AMBA_SPI_MASTER3:
                fd = open("/dev/spi3", O_RDWR);
                break;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
            case AMBA_SPI_MASTER4:
                fd = open("/dev/spi4", O_RDWR);
                break;
            case AMBA_SPI_MASTER5:
                fd = open("/dev/spi5", O_RDWR);
                break;
#endif
            }

            if (fd == -1) {
                fprintf(stderr, "%s %d, open device failed\n", __FUNCTION__, __LINE__);
                RetVal = SPI_ERR_OPEN_FILE;
            } else {

                MstXferD8Msg.In.SlaveMask = SlaveMask;
                memcpy(&MstXferD8Msg.In.Config, pConfig, sizeof(AMBA_SPI_CONFIG_s));
                MstXferD8Msg.In.BufSize = BufSize;
                MstXferD8Msg.In.TimeOut = TimeOut;

                if (pTxBuf != NULL) {
                    for (BufIdx = 0U; BufIdx < BufSize; BufIdx++) {
                        MstXferD8Msg.In.TxBuf[BufIdx] = pTxBuf[BufIdx];
                    }
                }

                err = devctl(fd, DCMD_SPI_MST_XFER_D8, &MstXferD8Msg, sizeof(mst_xfer_d8_msg_t), NULL);
                if (err) {
                    RetVal = SPI_ERR_DEV_CTL;
                }

                if (pRxBuf != NULL) {
                    for (BufIdx = 0U; BufIdx < BufSize; BufIdx++) {
                        pRxBuf[BufIdx] = MstXferD8Msg.Out.RxBuf[BufIdx];
                    }
                }

                if (pActualSize != NULL) {
                    *pActualSize = MstXferD8Msg.Out.ActualSize;
                }

                close(fd);
            }

            if (AmbaKAL_MutexGive(&AmbaSpiMstMutex[MasterID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SPI_ERR_UNEXPECTED;
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
 *  @param[in] TimeOut: The timeout value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSPI_MasterTransferD16(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                                 UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    UINT32 RetVal = SPI_ERR_NONE;
    int fd, err;
    mst_xfer_d16_msg_t MstXferD16Msg;
    unsigned int BufIdx = 0U;


    if ((MasterID >= AMBA_NUM_SPI_MASTER) || (BufSize == 0U) || (BufSize > AMBA_SPI_MASTER_FIFO_COUNT) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {

        if (AmbaKAL_MutexTake(&AmbaSpiMstMutex[MasterID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else {

            switch (MasterID) {
            case AMBA_SPI_MASTER0:
                fd = open("/dev/spi0", O_RDWR);
                break;
            case AMBA_SPI_MASTER1:
                fd = open("/dev/spi1", O_RDWR);
                break;
            case AMBA_SPI_MASTER2:
                fd = open("/dev/spi2", O_RDWR);
                break;
            case AMBA_SPI_MASTER3:
                fd = open("/dev/spi3", O_RDWR);
                break;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
            case AMBA_SPI_MASTER4:
                fd = open("/dev/spi4", O_RDWR);
                break;
            case AMBA_SPI_MASTER5:
                fd = open("/dev/spi5", O_RDWR);
                break;
#endif
            }

            if (fd == -1) {
                fprintf(stderr, "%s %d, open device failed\n", __FUNCTION__, __LINE__);
                RetVal = SPI_ERR_OPEN_FILE;
            } else {

                MstXferD16Msg.In.SlaveMask = SlaveMask;
                memcpy(&MstXferD16Msg.In.Config, pConfig, sizeof(AMBA_SPI_CONFIG_s));
                MstXferD16Msg.In.BufSize = BufSize;
                MstXferD16Msg.In.TimeOut = TimeOut;

                if (pTxBuf != NULL) {
                    for (BufIdx = 0U; BufIdx < BufSize; BufIdx++) {
                        MstXferD16Msg.In.TxBuf[BufIdx] = pTxBuf[BufIdx];
                    }
                }

                err = devctl(fd, DCMD_SPI_MST_XFER_D16, &MstXferD16Msg, sizeof(mst_xfer_d16_msg_t), NULL);
                if (err) {
                    RetVal = SPI_ERR_DEV_CTL;
                }

                if (pRxBuf != NULL) {
                    for (BufIdx = 0U; BufIdx < BufSize; BufIdx++) {
                        pRxBuf[BufIdx] = MstXferD16Msg.Out.RxBuf[BufIdx];
                    }
                }

                if (pActualSize != NULL) {
                    *pActualSize = MstXferD16Msg.Out.ActualSize;
                }

                close(fd);
            }

            if (AmbaKAL_MutexGive(&AmbaSpiMstMutex[MasterID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SPI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
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
    int fd, err;
    mst_info_get_msg_t MstInfoGetMsg;

    if ((MasterID >= AMBA_NUM_SPI_MASTER) || (pNumSlaves == NULL) || (pStatus == NULL)) {
        RetVal = SPI_ERR_ARG;
    } else {

        switch (MasterID) {
        case AMBA_SPI_MASTER0:
            fd = open("/dev/spi0", O_RDWR);
            break;
        case AMBA_SPI_MASTER1:
            fd = open("/dev/spi1", O_RDWR);
            break;
        case AMBA_SPI_MASTER2:
            fd = open("/dev/spi2", O_RDWR);
            break;
        case AMBA_SPI_MASTER3:
            fd = open("/dev/spi3", O_RDWR);
            break;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        case AMBA_SPI_MASTER4:
            fd = open("/dev/spi4", O_RDWR);
            break;
        case AMBA_SPI_MASTER5:
            fd = open("/dev/spi5", O_RDWR);
            break;
#endif
        }

        if (fd == -1) {
            fprintf(stderr, "%s %d, open device failed\n", __FUNCTION__, __LINE__);
            RetVal = SPI_ERR_OPEN_FILE;
        } else {

            err = devctl(fd, DCMD_SPI_MST_INFO_GET, &MstInfoGetMsg, sizeof(mst_info_get_msg_t), NULL);
            if (err) {
                RetVal = SPI_ERR_DEV_CTL;
            }

            *pNumSlaves = MstInfoGetMsg.NumSlaves;
            *pStatus = MstInfoGetMsg.Status;

            close(fd);
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
    int fd, err;
    slv_xfer_d8_msg_t SlvXferD8Msg;
    unsigned int BufIdx = 0U;


    if ((SlaveID >= AMBA_NUM_SPI_MASTER) || (BufSize == 0U) || (BufSize > AMBA_SPI_MASTER_FIFO_COUNT) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {

        if (AmbaKAL_MutexTake(&AmbaSpiSlvMutex[SlaveID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else {

            fd = open("/dev/spislave", O_RDWR);
            if (fd == -1) {
                fprintf(stderr, "%s %d, open device failed\n", __FUNCTION__, __LINE__);
                RetVal = SPI_ERR_OPEN_FILE;
            } else {

                SlvXferD8Msg.In.BufSize = BufSize;
                SlvXferD8Msg.In.TimeOut = TimeOut;

                if (pTxBuf != NULL) {
                    for (BufIdx = 0U; BufIdx < BufSize; BufIdx++) {
                        SlvXferD8Msg.In.TxBuf[BufIdx] = pTxBuf[BufIdx];
                    }
                }

                err = devctl(fd, DCMD_SPI_SLV_XFER_D8, &SlvXferD8Msg, sizeof(slv_xfer_d8_msg_t), NULL);
                if (err) {
                    RetVal = SPI_ERR_DEV_CTL;
                }

                if (pRxBuf != NULL) {
                    for (BufIdx = 0U; BufIdx < BufSize; BufIdx++) {
                        pRxBuf[BufIdx] = SlvXferD8Msg.Out.RxBuf[BufIdx];
                    }
                }

                if (pActualSize != NULL) {
                    *pActualSize = SlvXferD8Msg.Out.ActualSize;
                }

                close(fd);
            }

            if (AmbaKAL_MutexGive(&AmbaSpiSlvMutex[SlaveID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SPI_ERR_UNEXPECTED;
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
    UINT32 RetVal = SPI_ERR_NONE;
    int fd, err;
    slv_xfer_d16_msg_t SlvXferD16Msg;
    unsigned int BufIdx = 0U;


    if ((SlaveID >= AMBA_NUM_SPI_MASTER) || (BufSize == 0U) || (BufSize > AMBA_SPI_MASTER_FIFO_COUNT) || ((pTxBuf == NULL) && (pRxBuf == NULL))) {
        RetVal = SPI_ERR_ARG;
    } else {

        if (AmbaKAL_MutexTake(&AmbaSpiSlvMutex[SlaveID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else {

            fd = open("/dev/spislave", O_RDWR);
            if (fd == -1) {
                fprintf(stderr, "%s %d, open device failed\n", __FUNCTION__, __LINE__);
                RetVal = SPI_ERR_OPEN_FILE;
            } else {

                SlvXferD16Msg.In.BufSize = BufSize;
                SlvXferD16Msg.In.TimeOut = TimeOut;

                if (pTxBuf != NULL) {
                    for (BufIdx = 0U; BufIdx < BufSize; BufIdx++) {
                        SlvXferD16Msg.In.TxBuf[BufIdx] = pTxBuf[BufIdx];
                    }
                }

                err = devctl(fd, DCMD_SPI_SLV_XFER_D16, &SlvXferD16Msg, sizeof(slv_xfer_d16_msg_t), NULL);
                if (err) {
                    RetVal = SPI_ERR_DEV_CTL;
                }

                if (pRxBuf != NULL) {
                    for (BufIdx = 0U; BufIdx < BufSize; BufIdx++) {
                        pRxBuf[BufIdx] = SlvXferD16Msg.Out.RxBuf[BufIdx];
                    }
                }

                if (pActualSize != NULL) {
                    *pActualSize = SlvXferD16Msg.Out.ActualSize;
                }

                close(fd);
            }

            if (AmbaKAL_MutexGive(&AmbaSpiSlvMutex[SlaveID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SPI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaSPI_SlaveGetInfo - The function returns all the information about the slave.
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[out] pStatus The current bus status
 *  @return error code
 */
UINT32 AmbaSPI_SlaveGetInfo(UINT32 SlaveID, UINT32 *pStatus)
{
    UINT32 RetVal = SPI_ERR_NONE;
    int fd, err;
    slv_info_get_msg_t SlvInfoGetMsg;

    if ((SlaveID >= AMBA_NUM_SPI_MASTER) || (pStatus == NULL)) {
        RetVal = SPI_ERR_ARG;
    } else {

        fd = open("/dev/spislave", O_RDWR);
        if (fd == -1) {
            fprintf(stderr, "%s %d, open device failed\n", __FUNCTION__, __LINE__);
            RetVal = SPI_ERR_OPEN_FILE;
        } else {

            err = devctl(fd, DCMD_SPI_SLV_INFO_GET, &SlvInfoGetMsg, sizeof(slv_info_get_msg_t), NULL);
            if (err) {
                RetVal = SPI_ERR_DEV_CTL;
            }

            *pStatus = SlvInfoGetMsg.Status;

            close(fd);
        }
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
    int fd, err;
    slv_cfg_msg_t SlvCfgMsg;

    if ((SlaveID >= AMBA_NUM_SPI_SLAVE) || (pConfig == NULL)) {
        RetVal = SPI_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaSpiSlvMutex[SlaveID], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SPI_ERR_MUTEX;
        } else {

            fd = open("/dev/spislave", O_RDWR);
            if (fd == -1) {
                fprintf(stderr, "%s %d, open device failed\n", __FUNCTION__, __LINE__);
                RetVal = SPI_ERR_OPEN_FILE;
            } else {

                memcpy(&(SlvCfgMsg.Config), pConfig, sizeof(AMBA_SPI_CONFIG_s));

                err = devctl(fd, DCMD_SPI_SLV_CFG, &SlvCfgMsg, sizeof(slv_cfg_msg_t), NULL);
                if (err) {
                    RetVal = SPI_ERR_DEV_CTL;
                }

                close(fd);
            }

            if (AmbaKAL_MutexGive(&AmbaSpiSlvMutex[SlaveID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SPI_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

