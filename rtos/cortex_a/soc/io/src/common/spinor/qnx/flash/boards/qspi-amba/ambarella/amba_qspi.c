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

#include "variant.h"
#include "amba_gqspi.h"
#include "qspi_cmds.h"

/* open() ... */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <gulliver.h>
//#include <hw/xzynq_clk.h>

#include "AmbaSPINOR.h"
#include "AmbaCSL_SpiNOR.h"

static uint64_t vtophys(void* addr)
{
    off64_t buf;

    if (mem_offset64(addr, NOFD, 1, &buf,  0) < 0) return 0;
    return buf;
}

/* Function: allocate memory for passed QSPI buffer and fill required fields
 * Parameters:
 *                 buf - pointer to buffer descriptor with filled field 'len'
 * Return:
 *             0 for success
 */
static int qspi_buf_alloc(qspi_buf* buf)
{
    buf->offset = mmap(NULL, buf->len, PROT_READ | PROT_WRITE,
                       MAP_ANON | MAP_PHYS | MAP_PRIVATE, NOFD, 0);

    if (buf->offset == MAP_FAILED) return 1;

    buf->phys_base = vtophys(buf->offset);
    return 0;
}

static int qspi_buf_free(qspi_buf* buf)
{
    munmap(buf->offset, buf->len);
    return 0;
}
#if 0
static void xzynq_qspi_dma_flush(xzynq_qspi_t* dev)
{
    int dma_unreaded_size = (in32(dev->vbase + XZYNQ_GQSPIDMA_STS_OFFSET) >> 3) & 0xFF;
    out32(dev->vbase + XZYNQ_GQSPI_CR_OFFSET, in32(dev->vbase + XZYNQ_GQSPI_CR_OFFSET) & ~XZYNQ_GQSPI_CR_MODE_MASK);
    out32(dev->vbase + XZYNQ_GQSPIDMA_SIZE_OFFSET, dma_unreaded_size);
    delay(5);
    out32(dev->vbase + XZYNQ_GQSPIDMA_ISR_OFFSET,in32(dev->vbase + XZYNQ_GQSPIDMA_ISR_OFFSET));
}
#endif
xzynq_qspi_t* xzynq_qspi_open(void)
{
    static xzynq_qspi_t *dev;

    if ((dev = calloc(1, sizeof(xzynq_qspi_t))) == NULL) {
        fprintf(stderr, "norqspi: Could not allocate xzynq_qspi_t memory\n");
        return NULL;
    }

    if ((dev->clk_dev = open("/dev/clock",O_RDWR)) < 0) {
        fprintf(stderr,"norqspi: Cannot access to device /dev/clock. Use default clocks");
        dev->clk_dev = -1;
    }

    dev->pbase = XZYNQ_GQSPI_REG_ADDRESS;
    dev->irq = XZYNQ_GQSPI_IRQ;
    //dev->board_clock = xzynq_read_board_clock(dev);
    dev->board_clock = 60000000;
    dev->speed = XZYNQ_GQSPI_DEFAULT_SPEED;
    dev->treshold = XZYNQ_GQSPI_DEFAULT_TRESHOLD;
    dev->mode = XZYNQ_GQSPI_IO_MODE;
#if 0
    dev->vbase = mmap_device_io(XZYNQ_GQSPI_REG_SIZE, dev->pbase);

    if (dev->vbase == (uintptr_t)MAP_FAILED) goto fail0;

    extern AMBA_NOR_REG_s *pAmbaNOR_Reg;

    pAmbaNOR_Reg = (AMBA_NOR_REG_s *) dev->vbase;
#endif
    extern const AMBA_NORSPI_DEV_INFO_s AmbaNORSPI_DevInfo;
    extern const AMBA_SERIAL_SPI_CONFIG_s AmbaNOR_SPISetting;
    extern const AMBA_PARTITION_CONFIG_s AmbaNORSPI_SysPartConfig[AMBA_NUM_SYS_PARTITION];
    extern const AMBA_PARTITION_CONFIG_s AmbaNORSPI_UserPartConfig[AMBA_NUM_USER_PARTITION];

    AMBA_NOR_SPI_CONFIG_s NorSpiConfig = {0};

    NorSpiConfig.pNorSpiDevInfo  = (AMBA_NORSPI_DEV_INFO_s *)&AmbaNORSPI_DevInfo;
    NorSpiConfig.SpiSetting      = (AMBA_SERIAL_SPI_CONFIG_s *)&AmbaNOR_SPISetting;

    NorSpiConfig.pSysPartConfig  = (AMBA_PARTITION_CONFIG_s *)AmbaNORSPI_SysPartConfig;
    NorSpiConfig.pUserPartConfig = (AMBA_PARTITION_CONFIG_s *)AmbaNORSPI_UserPartConfig;
#if 0
    AmbaRTSL_NorInit();
    AmbaRTSL_NorSpiConfig(&NorSpiConfig);

    flash_reset(dev);
    AmbaRTSL_NorSpiInitPtbBbt();
#else
    AmbaSpiNOR_Config(&NorSpiConfig);
#endif
    cache_init(CACHE_INIT_FLAG_IGNORE_SCAN, &dev->cinfo, NULL);
#if 0
    dev->dma_buf.len = XZYNQ_GQSPI_MAX_DMA_SIZE;
    if (qspi_buf_alloc(&(dev->dma_buf))) {
        fprintf(stderr, "norqspi: Could not allocate memory for DMA buffer(%s)\n", strerror(errno));
        goto fail1;
    }

    /* Attach interrupt */
    if ((dev->chid = ChannelCreate(_NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK)) == -1) goto fail2;

    if ((dev->coid = ConnectAttach(0, 0, dev->chid, _NTO_SIDE_CHANNEL, 0)) == -1) goto fail3;

    SIGEV_PULSE_INIT(&dev->qspievent,dev->coid, XZYNQ_GQSPI_PRIORITY, XZYNQ_GQSPI_EVENT, 0);

    dev->iid = InterruptAttachEvent(dev->irq, &dev->qspievent, _NTO_INTR_FLAGS_TRK_MSK);

    if (dev->iid == -1) goto fail4;

    /* Enable QSPI */
    out32(dev->vbase + XZYNQ_GQSPI_ER_OFFSET, XZYNQ_GQSPI_ER_ENABLE_MASK);

    xzynq_qspi_dma_flush(dev);
#endif
//    slogf(1000, 1, "(devf  t%d::%s:%d) irq=%d", pthread_self(), __func__, __LINE__, dev->irq);

    return dev;

fail4:
    ConnectDetach(dev->coid);
fail3:
    ChannelDestroy(dev->chid);
fail2:
    qspi_buf_free(&(dev->dma_buf));
fail1:
    munmap_device_io(dev->vbase, XZYNQ_GQSPI_REG_SIZE);
fail0:
    free(dev);

    return NULL;
}

int xzynq_qspi_close(void* fd)
{
    xzynq_qspi_t *dev = (xzynq_qspi_t *)fd;

    InterruptDetach(dev->iid);
    ConnectDetach(dev->coid);
    ChannelDestroy(dev->chid);
    qspi_buf_free(&(dev->dma_buf));
    munmap_device_io(dev->vbase, XZYNQ_GQSPI_REG_SIZE);
    if (dev->clk_dev) close(dev->clk_dev);
    free(dev);

    return EOK;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/hardware/flash/boards/qspi-xzynq/aarch64/le.zcu102/xzynq_qspi.c $ $Rev: 864741 $");
#endif
