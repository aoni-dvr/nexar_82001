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

#include "f3s_qspi.h"

/*
 * This is the ident callout for SPI serial NOR flash.
 */
int32_t f3s_qspi_ident(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags,
                       uint32_t offset)
{
    xzynq_qspi_t *qspi = (xzynq_qspi_t *)access->socket.socket_handle;
    uint8_t     ids_low[IDENT_LEN] = { 0 };     /* Device id data */
    uint8_t     ids_up[IDENT_LEN] = { 0 };
    uint16_t    did_low = 0;                    /* Device id */
    uint16_t    did_up = 0;
    uint8_t     sfdp[SFDP_LEN] = { 0 };         /* SFDP data */

    // Query flash chip,
    // For now, we only support the following two configurations
    //  1) one chip connected to either CS_LOW/DB_LOW, or CS_UP/DB_UP
    //  2) two chips stripe mode

    qspi->flags &= ~XZYNQ_FLAG_CS_MASK;

    /* Read ident of lower chip */
    if (read_ident(qspi, ids_low, TRANSFER_FLAG_LOW_DB | TRANSFER_FLAG_CS(TRANSFER_FLAG_CS_LOW)) < 0) {
        return (ENOENT);
    }
    if (ids_low[IDENT_MID] != 0 && ids_low[IDENT_MID] != 0xff) {
        did_low = ids_low[IDENT_MEMTYPE] << 8 | ids_low[IDENT_MEMCAP];
        qspi->flags |= XZYNQ_FLAG_CS_LOWER;
        slogf(1000, 1, "(devf  t%d::%s:%d) DID0: 0x%x MID0: 0x%x", pthread_self(), __func__, __LINE__, did_low, ids_low[IDENT_MID]);
    }

    /* Read ident of upper chip */
    if (read_ident(qspi, ids_up, TRANSFER_FLAG_UP_DB | TRANSFER_FLAG_CS(TRANSFER_FLAG_CS_UP)) < 0) {
        return (ENOENT);
    }
    if (ids_up[IDENT_MID] != 0 && ids_up[IDENT_MID] != 0xff) {
        did_up = ids_up[IDENT_MEMTYPE] << 8 | ids_up[IDENT_MEMCAP];
        slogf(1000, 1, "(devf  t%d::%s:%d) DID1: 0x%x MID1: 0x%x", pthread_self(), __func__, __LINE__, did_up, ids_up[IDENT_MID]);
        qspi->flags |= XZYNQ_FLAG_CS_UPPER;
    }
#if 0
    /* According to Zynq UltraScale+ MPSoC TRM UG1085 (v1.3)
     * Table 24-18: Quad-SPI Command List for Dual Quad-SPI
     * Parallel Mode, the two chips must be identical when
     * when using the parallel flash mode.
     */
    if ((qspi->flags & XZYNQ_FLAG_CS_MASK) == (XZYNQ_FLAG_CS_UPPER | XZYNQ_FLAG_CS_LOWER)) {
        if (ids_low[IDENT_MID] != ids_up[IDENT_MID] || did_low != did_up) {
            slogf(1000, 1, "(devf  t%d::%s:%d) Two flash chips are not identical!", pthread_self(), __func__, __LINE__);
            return (ENOENT);
        }
        qspi->flags |= XZYNQ_FLAG_CS_STRIPE;
    }
#endif
    int xflags = (qspi->flags & XZYNQ_FLAG_CS_UPPER) ?
                 TRANSFER_FLAG_UP_DB | TRANSFER_FLAG_CS(TRANSFER_FLAG_CS_UP) :
                 TRANSFER_FLAG_LOW_DB | TRANSFER_FLAG_CS(TRANSFER_FLAG_CS_LOW);
    /* Read and parse SFDP data */
    if (read_sfdp(qspi, sfdp, xflags) == EOK) {
        if (sfdp[0] == 'S' && sfdp[1] == 'F' && sfdp[2] == 'D' && sfdp[3] == 'P') {
            // Byte 6, number of parameter tables, 0 based
            int num_p = (sfdp[6] + 1);

            // Parameter table header starts from byte 8
            uint8_t *header = &sfdp[0x08];

            int n;
            uint8_t *p;
            for (n = 0; n < num_p; n++) {
                p = &header[n * 8];
                if (p[0] == 0 && p[3] > 4) {    // Byte 0, header type, "0" JEDEC header
                    int     offset = ((uint32_t)p[6] << 16) | ((uint32_t)p[5] << 8) | p[4]; // Table offset

                    // Get table address
                    uint8_t *table = &sfdp[offset];

                    // Chip density (bits)
                    uint32_t    density = ((uint32_t)table[7] << 24) | ((uint32_t)table[6] << 16) | ((uint32_t)table[5] << 8) | table[4];
                    int chip_size = ((density + 1) >> 3);

                    // write buffer size
                    int buffer_size = (uint32_t)1 << (table[0x28] >> 4);

                    // Default sector size: 64KB
                    int unit_size = access->socket.unit_size ? access->socket.unit_size : 64 * 1024;

                    int et;
                    for (et = 0x22; et > 0x1C; et -= 2) {
                        if (((uint32_t)1 << table[et]) == unit_size) break;
                    }

                    dbase->geo_vect[0].unit_pow2 = table[et];
                    unit_size = 1 << table[et];

                    access->socket.unit_size = unit_size;
                    access->socket.array_size = access->socket.window_size = chip_size;

                    dbase->geo_vect[0].unit_num = chip_size / unit_size;

                    if (qspi->flags & XZYNQ_FLAG_CS_LOWER) {
                        dbase->jedec_hi    = ids_low[IDENT_MID];
                        dbase->jedec_lo    = did_low;
                    } else {
                        dbase->jedec_hi    = ids_up[IDENT_MID];
                        dbase->jedec_lo    = did_up;
                    }
                    dbase->struct_size = sizeof(*dbase);
                    dbase->name        = "SFDP";
                    dbase->buffer_size = qspi->page_size = buffer_size;
                    dbase->geo_num     = 1;
                    dbase->flags       = 0;

                    slogf(1000, 1, "(devf  t%d::%s:%d) Page size: %d", pthread_self(), __func__,__LINE__, dbase->buffer_size);
                    slogf(1000, 1, "(devf  t%d::%s:%d) Total chip size: 0x%x", pthread_self(), __func__,__LINE__, access->socket.window_size);
                }
            }
        } else {
            fprintf(stderr, "\n(devf  t%d::%s:%d) Invalid SFDP\n", pthread_self(), __func__, __LINE__);
            return (EIO);
        }
    } else {
        fprintf(stderr, "\n(devf  t%d::%s:%d) Read ID failed\n", pthread_self(), __func__, __LINE__);
        return (EIO);
    }

    return (EOK);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/hardware/flash/boards/qspi-xzynq/f3s_qspi_ident.c $ $Rev: 859471 $");
#endif
