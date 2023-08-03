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

#include "hw/ambarella_spi.h"
#include <AmbaRTSL_SPI.h>
#include <AmbaINT_Priv.h>
#include <sys/neutrino.h>
#include <string.h>

static intrspin_t spi_lock_master;
static intrspin_t spi_lock_slave;

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define INT_SPI_SSI_SLAVE AMBA_INT_SPI_ID114_SSI_SLAVE
#else
#define INT_SPI_SSI_SLAVE AMBA_INT_SPI_ID112_SSI_SLAVE
#endif

/*
 * We use the same buffer for transmit and receive
 * For exchange, that's exactly what we wanted
 * For Read, it doesn't matter what we write to SPI, so we are OK.
 * For transmit, the receive data is put at the buffer we just transmitted, we are still OK.
 */

static const struct sigevent *spi_intr_master(void *area, int id)
{
    struct sigevent *ret = NULL;
    amba_spi_t *dev = area;

    InterruptLock(&spi_lock_master);

    SPI_MasterISR(id, dev->Channel);
    ret = (&dev->spievent);

    InterruptUnlock(&spi_lock_master);

    return ret;
}

static const struct sigevent *spi_intr_slave(void *area, int id)
{
    struct sigevent *ret = NULL;
    amba_spi_t *dev = area;

    InterruptLock(&spi_lock_slave);

    SPI_SlaveISR(id, dev->Channel);
    ret = (&dev->spievent);

    InterruptUnlock(&spi_lock_slave);

    return ret;
}

int Amba_Attach_Intr(amba_spi_t *dev)
{
    if ((dev->chid = ChannelCreate(_NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK))
        == -1)
        return -1;

    if ((dev->coid = ConnectAttach(0, 0, dev->chid, _NTO_SIDE_CHANNEL, 0))
        == -1)
        goto fail0;

    dev->spievent.sigev_notify = SIGEV_PULSE;
    dev->spievent.sigev_coid = dev->coid;
    dev->spievent.sigev_code = AMBA_SPI_EVENT;
    dev->spievent.sigev_priority = AMBA_SPI_PRIORITY;

    /* required for InterruptLock/Unlock abilities */
    ThreadCtl(_NTO_TCTL_IO, 0);
    memset(&spi_lock_master, 0, sizeof(spi_lock_master));
    memset(&spi_lock_slave, 0, sizeof(spi_lock_slave));

    /* Attach SPI interrupt */
    if (dev->irq_spi == INT_SPI_SSI_SLAVE) {
        dev->iid_spi = InterruptAttach(dev->irq_spi, spi_intr_slave, dev,
                                       sizeof(amba_spi_t), _NTO_INTR_FLAGS_TRK_MSK);
    } else {
        dev->iid_spi = InterruptAttach(dev->irq_spi, spi_intr_master, dev,
                                       sizeof(amba_spi_t), _NTO_INTR_FLAGS_TRK_MSK);
    }

    if (dev->iid_spi != -1) {
        return 0;
    }

    ConnectDetach(dev->coid);
fail0:
    ChannelDestroy(dev->chid);

    return -1;
}

