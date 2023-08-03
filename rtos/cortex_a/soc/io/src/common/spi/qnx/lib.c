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

#include <AmbaWrap.h>
#include <AmbaKAL.h>
#include <AmbaCortexA53.h>
#include <fcntl.h>
#include "proto.h"
#include "hw/ambarella_spi.h"
#include "hw/ambarella_clk.h"
#include <AmbaRTSL_SPI.h>
#include <AmbaINT_Priv.h>

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define SPI0_BASE_ADDR          AMBA_CA53_SPI0_BASE_ADDR
#define SPI1_BASE_ADDR          AMBA_CA53_SPI1_BASE_ADDR
#define SPI2_BASE_ADDR          AMBA_CA53_SPI2_BASE_ADDR
#define SPI3_BASE_ADDR          AMBA_CA53_SPI3_BASE_ADDR
#define SPI4_BASE_ADDR          AMBA_CA53_SPI4_BASE_ADDR
#define SPI5_BASE_ADDR          AMBA_CA53_SPI5_BASE_ADDR
#define SPI_SLAVE_BASE_ADDR     AMBA_CA53_SPI_SLAVE_BASE_ADDR

#define INT_SPI_SSI_MASTER0     AMBA_INT_SPI_ID110_SSI_MASTER0
#define INT_SPI_SSI_MASTER1     AMBA_INT_SPI_ID111_SSI_MASTER1
#define INT_SPI_SSI_MASTER2     AMBA_INT_SPI_ID112_SSI_MASTER2
#define INT_SPI_SSI_MASTER3     AMBA_INT_SPI_ID113_SSI_MASTER3
#define INT_SPI_SSI_MASTER4     AMBA_INT_SPI_ID166_SSI_MASTER4
#define INT_SPI_SSI_MASTER5     AMBA_INT_SPI_ID167_SSI_MASTER5
#define INT_SPI_SSI_SLAVE       AMBA_INT_SPI_ID114_SSI_SLAVE
#else
#define SPI0_BASE_ADDR          AMBA_CORTEX_A53_SSI0_BASE_ADDR
#define SPI1_BASE_ADDR          AMBA_CORTEX_A53_SSI1_BASE_ADDR
#define SPI2_BASE_ADDR          AMBA_CORTEX_A53_SSI2_BASE_ADDR
#define SPI3_BASE_ADDR          AMBA_CORTEX_A53_SSI3_BASE_ADDR
#define SPI_SLAVE_BASE_ADDR     AMBA_CORTEX_A53_SSI_SLAVE_BASE_ADDR

#define INT_SPI_SSI_MASTER0     AMBA_INT_SPI_ID106_SSI_MASTER0
#define INT_SPI_SSI_MASTER1     AMBA_INT_SPI_ID107_SSI_MASTER1
#define INT_SPI_SSI_MASTER2     AMBA_INT_SPI_ID108_SSI_MASTER2
#define INT_SPI_SSI_MASTER3     AMBA_INT_SPI_ID109_SSI_MASTER3
#define INT_SPI_SSI_SLAVE       AMBA_INT_SPI_ID112_SSI_SLAVE
#endif

AMBA_SPI_REG_s *pAmbaSPI_MasterReg[AMBA_NUM_SPI_MASTER];
AMBA_SPI_REG_s *pAmbaSPI_SlaveReg[AMBA_NUM_SPI_SLAVE];
AMBA_RCT_REG_s *pAmbaRCT_Reg;

static amba_spi_t *dev_master;
static amba_spi_t *dev_slave;

int spi_dev_init(int chan, int slave)
{
    clk_freq_t clk_freq;
    uintptr_t base;
    int fd;
    int err;
    uintptr_t VirtualAddr;

    /* Map RCT register address */
    if ((VirtualAddr = (uintptr_t)mmap_device_memory( NULL, AMBA_RCT_MMAP_SIZE,
                       PROT_READ | PROT_WRITE | PROT_NOCACHE, 0,
                       AMBA_RCT_BASE_ADDR)) == MAP_FAILED) {
        fprintf(stderr, "RCT registers mapping failed!\n");
        return SPI_ERR_UNEXPECTED;
    } else {
        pAmbaRCT_Reg = (AMBA_RCT_REG_s *) VirtualAddr;
    }


    if (slave != AMBA_SPI_SLAVE_DEVICE) {
        /* Init SPI_Master */
        dev_master = calloc(1, sizeof(amba_spi_t));

        if (dev_master == NULL)
            return SPI_ERR_UNEXPECTED;

        /* Set configs */
        switch(chan) {
        case AMBA_SPI_MASTER0:
            dev_master->Channel = 0;
            dev_master->pbase = SPI0_BASE_ADDR;
            dev_master->irq_spi = INT_SPI_SSI_MASTER0;
            dev_master->CsPolarity = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW;
            dev_master->BaudRate = AMBA_SPI_BAUDRATE_DEFAULT;
            break;
        case AMBA_SPI_MASTER1:
            dev_master->Channel = 1;
            dev_master->pbase = SPI1_BASE_ADDR;
            dev_master->irq_spi = INT_SPI_SSI_MASTER1;
            dev_master->CsPolarity = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW;
            dev_master->BaudRate = AMBA_SPI_BAUDRATE_DEFAULT;
            break;
        case AMBA_SPI_MASTER2:
            dev_master->Channel = 2;
            dev_master->pbase = SPI2_BASE_ADDR;
            dev_master->irq_spi = INT_SPI_SSI_MASTER2;
            dev_master->CsPolarity = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW;
            dev_master->BaudRate = AMBA_SPI_BAUDRATE_DEFAULT;
            break;
        case AMBA_SPI_MASTER3:
            dev_master->Channel = 3;
            dev_master->pbase = SPI3_BASE_ADDR;
            dev_master->irq_spi = INT_SPI_SSI_MASTER3;
            dev_master->CsPolarity = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW;
            dev_master->BaudRate = AMBA_SPI_BAUDRATE_DEFAULT;
            break;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        case AMBA_SPI_MASTER4:
            dev_master->Channel = 4;
            dev_master->pbase = SPI4_BASE_ADDR;
            dev_master->irq_spi = INT_SPI_SSI_MASTER4;
            dev_master->CsPolarity = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW;
            dev_master->BaudRate = AMBA_SPI_BAUDRATE_DEFAULT;
            break;
        case AMBA_SPI_MASTER5:
            dev_master->Channel = 5;
            dev_master->pbase = SPI5_BASE_ADDR;
            dev_master->irq_spi = INT_SPI_SSI_MASTER5;
            dev_master->CsPolarity = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW;
            dev_master->BaudRate = AMBA_SPI_BAUDRATE_DEFAULT;
            break;
#endif
        default:
            return SPI_ERR_UNEXPECTED;
        }

        /* Map in SPI registers */
        if ((base = mmap_device_io(AMBA_SPI_REGLEN, dev_master->pbase)) == (uintptr_t) MAP_FAILED) {
            fprintf(stderr, "SPI registers mapping failed!\n");
            free(dev_master);
            return SPI_ERR_UNEXPECTED;
        }

        dev_master->vbase = base;
        pAmbaSPI_MasterReg[dev_master->Channel] = (AMBA_SPI_REG_s *)base;

        AmbaRTSL_SpiMasterStop(dev_master->Channel);

        /* Attach SPI interrupt */
        if (Amba_Attach_Intr(dev_master)) {
            munmap_device_io(dev_master->vbase, AMBA_SPI_REGLEN);
            return SPI_ERR_UNEXPECTED;
        }

        /* Open clock manager */
        fd = open("/dev/clock", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager. Use default clocks", __FUNCTION__);
        } else {
            clk_freq.freq = AMBA_SPI_CLOCK_VALUE;
            clk_freq.id = AMBA_CLK_SPIMASTER;

            /* Set SPI_Master control clock */
            err = devctl(fd, DCMD_CLOCK_SET_FREQ, &clk_freq, sizeof(clk_freq_t), NULL);
            if (err) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Clock SET devctl failed: %s", __FUNCTION__, strerror(errno));
            }

            /* Get SPI_Master control clock */
            err = devctl(fd, DCMD_CLOCK_GET_FREQ, &clk_freq, sizeof(clk_freq_t), NULL);
            if (err) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Clock GET devctl failed: %s", __FUNCTION__, strerror(errno));
            }

            if (clk_freq.freq == -1) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: /dev/clock: Invalid frequency (-1)", __FUNCTION__);
            } else {
                /* Set real frequency */
                dev_master->SourceClk = clk_freq.freq;
            }

            close(fd);
        }

    } else {
        /* Init SPI_Slave */
        dev_slave = calloc(1, sizeof(amba_spi_t));

        if (dev_slave == NULL)
            return SPI_ERR_UNEXPECTED;

        /* Set configs */
        dev_slave->Channel = 0;
        dev_slave->pbase = SPI_SLAVE_BASE_ADDR;
        dev_slave->irq_spi = INT_SPI_SSI_SLAVE;
        dev_slave->CsPolarity = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW;
        dev_slave->BaudRate = AMBA_SPI_BAUDRATE_DEFAULT;

        /* Map in SPI registers */
        if ((base = mmap_device_io(AMBA_SPI_REGLEN, dev_slave->pbase)) == (uintptr_t) MAP_FAILED) {
            fprintf(stderr, "SPI slave registers mapping failed!\n");
            free(dev_slave);
            return SPI_ERR_UNEXPECTED;
        }

        dev_slave->vbase = base;
        pAmbaSPI_SlaveReg[dev_slave->Channel] = (AMBA_SPI_REG_s *)base;

        /* Attach SPI interrupt */
        if (Amba_Attach_Intr(dev_slave)) {
            munmap_device_io(dev_slave->vbase, AMBA_SPI_REGLEN);
            return SPI_ERR_UNEXPECTED;
        }

        /* Open clock manager */
        fd = open("/dev/clock", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager. Use default clocks", __FUNCTION__);
        } else {
            clk_freq.freq = AMBA_SPI_CLOCK_VALUE;
            clk_freq.id = AMBA_CLK_SPISLAVE;

            /* Set SPI_Slave control clock */
            err = devctl(fd, DCMD_CLOCK_SET_FREQ, &clk_freq, sizeof(clk_freq_t), NULL);
            if (err) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Clock SET devctl failed: %s", __FUNCTION__, strerror(errno));
            }

            /* Get SPI_Slave control clock */
            err = devctl(fd, DCMD_CLOCK_GET_FREQ, &clk_freq, sizeof(clk_freq_t), NULL);
            if (err) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Clock GET devctl failed: %s", __FUNCTION__, strerror(errno));
            }

            if (clk_freq.freq == -1) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: /dev/clock: Invalid frequency (-1)", __FUNCTION__);
            } else {
                /* Set real frequency */
                dev_slave->SourceClk = clk_freq.freq;
            }

            close(fd);
        }
    }

    return SPI_ERR_NONE;
}

int spi_dev_dinit(void)
{
    /* Disable controller */
    AmbaRTSL_SpiMasterStop(dev_master->Channel);

    /* Unmap the register, detach the interrupt */
    InterruptDetach(dev_master->iid_spi);
    InterruptDetach(dev_slave->iid_spi);
    munmap_device_io(dev_master->vbase, AMBA_SPI_REGLEN);
    munmap_device_io(dev_slave->vbase, AMBA_SPI_REGLEN);

    ConnectDetach(dev_master->coid);
    ConnectDetach(dev_slave->coid);
    ChannelDestroy(dev_master->chid);
    ChannelDestroy(dev_slave->chid);

    free(dev_master);
    free(dev_slave);

    return 0;
}

/**
 *  spi_MasterTransferD8 - SPI Master write and read data operation
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
int spi_MasterTransferD8(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                         UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    int RetVal = SPI_ERR_NONE;

    (void)TimeOut;  /* Will calculate automatically by Amba_Wait */
    dev_master->Channel = MasterID;
    dev_master->BaudRate = pConfig->BaudRate;
    dev_master->CsPolarity = pConfig->CsPolarity;
    dev_master->ClkMode = pConfig->ClkMode;
    dev_master->FrameSize = pConfig->DataFrameSize;
    dev_master->FrameBitOrder = pConfig->FrameBitOrder;

    dev_master->status = 0;
    dev_master->dlen = ((dev_master->FrameSize & SPI_MODE_CHAR_LEN_MASK) + 7) >> 3;

    /* Estimate transfer time in us... The calculated dtime is only used for
     * the timeout, so it doesn't have to be that accurate.  At higher clock
     * rates, a calculated dtime of 0 would mess-up the timeout calculation, so
     * round up to 1 us
     */
    dev_master->dtime = dev_master->dlen * 1000 * 1000 / dev_master->BaudRate;
    if (dev_master->dtime == 0) {
        dev_master->dtime = 1;
    }
    dev_master->xlen = BufSize;

    AmbaRTSL_SpiMasterStop(MasterID);

    RetVal = AmbaRTSL_SpiMasterConfig(MasterID, SlaveMask, pConfig);

    if (RetVal == SPI_ERR_NONE) {

        RetVal = AmbaRTSL_SpiMasterTransferD8(MasterID, BufSize, pTxBuf, pRxBuf);

        if (RetVal == SPI_ERR_NONE) {
            RetVal = Amba_Wait(dev_master, dev_master->xlen);
            if (RetVal != 0) {
                RetVal = SPI_ERR_TMO;
            }

            AmbaRTSL_SpiMasterStop(MasterID);

            if (pActualSize != NULL) {
                RetVal = AmbaRTSL_SpiMasterGetInfo(MasterID, NULL, NULL, pActualSize);
            }
        }
    }

    return RetVal;
}

/**
 *  spi_MasterTransferD16 - SPI Master write and read data operation
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
int spi_MasterTransferD16(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                          UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    int RetVal = SPI_ERR_NONE;

    (void)TimeOut;  /* Will calculate automatically by Amba_Wait */
    dev_master->Channel = MasterID;
    dev_master->BaudRate = pConfig->BaudRate;
    dev_master->CsPolarity = pConfig->CsPolarity;
    dev_master->ClkMode = pConfig->ClkMode;
    dev_master->FrameSize = pConfig->DataFrameSize;
    dev_master->FrameBitOrder = pConfig->FrameBitOrder;

    dev_master->status = 0;
    dev_master->dlen = ((dev_master->FrameSize & SPI_MODE_CHAR_LEN_MASK) + 7) >> 3;

    /* Estimate transfer time in us... The calculated dtime is only used for
     * the timeout, so it doesn't have to be that accurate.  At higher clock
     * rates, a calculated dtime of 0 would mess-up the timeout calculation, so
     * round up to 1 us
     */
    dev_master->dtime = dev_master->dlen * 1000 * 1000 / dev_master->BaudRate;
    if (dev_master->dtime == 0) {
        dev_master->dtime = 1;
    }
    dev_master->xlen = BufSize;

    AmbaRTSL_SpiMasterStop(MasterID);

    RetVal = AmbaRTSL_SpiMasterConfig(MasterID, SlaveMask, pConfig);

    if (RetVal == SPI_ERR_NONE) {

        RetVal = AmbaRTSL_SpiMasterTransferD16(MasterID, BufSize, pTxBuf, pRxBuf);

        if (RetVal == SPI_ERR_NONE) {
            RetVal = Amba_Wait(dev_master, dev_master->xlen);
            if (RetVal != 0) {
                RetVal = SPI_ERR_TMO;
            }

            AmbaRTSL_SpiMasterStop(MasterID);

            if (pActualSize != NULL) {
                RetVal = AmbaRTSL_SpiMasterGetInfo(MasterID, NULL, NULL, pActualSize);
            }
        }
    }

    return RetVal;
}

/**
 *  spi_MasterGetInfo - The function returns all the information about the master.
 *  @param[in] MasterID Indicate a SPI master
 *  @param[out] pNumSlaves Number of slave support of the master
 *  @param[out] pStatus The current bus status
 *  @return error code
 */
int spi_MasterGetInfo(UINT32 MasterID, UINT32 *pNumSlaves, UINT32 *pStatus)
{
    int RetVal;

    RetVal = AmbaRTSL_SpiMasterGetInfo(MasterID, pNumSlaves, pStatus, NULL);

    return RetVal;
}

/**
 *  spi_SlaveTransferD8 - SPI Slave write and read data operation
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
int spi_SlaveTransferD8(UINT32 SlaveID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    int RetVal = SPI_ERR_NONE;

    (void)TimeOut;  /* Will calculate automatically by Amba_Wait */
    dev_slave->xlen = BufSize;

    RetVal = AmbaRTSL_SpiSlaveTransferD8(SlaveID, BufSize, pTxBuf, pRxBuf);
    if (RetVal == SPI_ERR_NONE) {
        RetVal = Amba_Wait(dev_slave, dev_slave->xlen);
        if (RetVal != 0) {
            RetVal = SPI_ERR_TMO;
        }

        AmbaRTSL_SpiSlaveStop(SlaveID);

        if (pActualSize != NULL) {
            RetVal = AmbaRTSL_SpiSlaveGetInfo(SlaveID, NULL, pActualSize);
        }
    }

    return RetVal;
}

/**
 *  spi_SlaveTransferD16 - SPI Slave write and read data operation
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] BufSize The size of TX and RX buffer if exist
 *  @param[in] pTxBuf TX buffer. If this pointer is NULL, the signal on SDO/MOSI pin will be ignored.
 *  @param[out] pRxBuf RX buffer. If this pointer is NULL, the signal on SDI/MISO pin will be ignored.
 *  @param[out] pActualSize The actual number of the TX/RX frames
 *  @param[in] TimeOut The timeout value in system tick (ms)
 *  @return error code
 */
int spi_SlaveTransferD16(UINT32 SlaveID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    int RetVal = SPI_ERR_NONE;

    (void)TimeOut;  /* Will calculate automatically by Amba_Wait */
    dev_slave->xlen = BufSize;

    RetVal = AmbaRTSL_SpiSlaveTransferD16(SlaveID, BufSize, pTxBuf, pRxBuf);
    if (RetVal == SPI_ERR_NONE) {
        RetVal = Amba_Wait(dev_slave, dev_slave->xlen);
        if (RetVal != 0) {
            RetVal = SPI_ERR_TMO;
        }

        AmbaRTSL_SpiSlaveStop(SlaveID);

        if (pActualSize != NULL) {
            RetVal = AmbaRTSL_SpiSlaveGetInfo(SlaveID, NULL, pActualSize);
        }
    }

    return RetVal;
}

/**
 *  spi_SlaveGetInfo - The function returns all the information about the slave.
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[out] pStatus The current bus status
 *  @return error code
 */
int spi_SlaveGetInfo(UINT32 SlaveID, UINT32 *pStatus)
{
    int RetVal = SPI_ERR_NONE;

    RetVal = AmbaRTSL_SpiSlaveGetInfo(SlaveID, pStatus, NULL);

    return RetVal;
}

/**
 *  spi_SlaveConfig - The function setup the configurations for slave.
 *  @param[in] SlaveID Indicate a SPI slave
 *  @param[in] pConfig Define the combinations of polarity and phases for transitions where data should be latched
 *  @return error code
 */
int spi_SlaveConfig(UINT32 SlaveID, const AMBA_SPI_CONFIG_s * pConfig)
{
    int RetVal = SPI_ERR_NONE;

    dev_slave->Channel = SlaveID;
    dev_slave->BaudRate = pConfig->BaudRate;
    dev_slave->CsPolarity = pConfig->CsPolarity;
    dev_slave->ClkMode = pConfig->ClkMode;
    dev_slave->FrameSize = pConfig->DataFrameSize;
    dev_slave->FrameBitOrder = pConfig->FrameBitOrder;

    dev_slave->status = 0;
    dev_slave->dlen = ((dev_slave->FrameSize & SPI_MODE_CHAR_LEN_MASK) + 7) >> 3;

    /* Estimate transfer time in us... The calculated dtime is only used for
     * the timeout, so it doesn't have to be that accurate.  At higher clock
     * rates, a calculated dtime of 0 would mess-up the timeout calculation, so
     * round up to 1 us
     */
    dev_slave->dtime = dev_slave->dlen * 1000 * 1000 / dev_slave->BaudRate;
    if (dev_slave->dtime == 0) {
        dev_slave->dtime = 1;
    }

    RetVal = AmbaRTSL_SpiSlaveConfig(SlaveID, pConfig);

    return RetVal;
}

