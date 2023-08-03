/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
 * Copyright 2016, Freescale Semiconductor, Inc.
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

#ifndef DEVIO_H_
#define DEVIO_H_

#include <stdint.h>

#define AMBA_NAND_SPARE_SIZE_2X

#ifdef AMBA_NAND_SPARE_SIZE_2X
typedef struct {
    /* sub-sector 0 in page x */
    uint8_t       status;                 /**< Factory marking for bad block (0xff == GOOD) */
    uint8_t       status2;                /**< For 16 bit wide parts */
    uint16_t      nclusters;              /**< Number of clusters */
    uint8_t       lsn0;              /**< 4 bytes align since we have 8 byte of meta-data in each sub-sector */
    uint8_t       rsv0[14];               /**< ECC Signature */
    uint8_t       ecc0[13];               /**< ECC Signature */

    /* sub-sector 1 in page x */
    uint32_t      sequence;               /**< Sequence number */
    uint16_t      fid;                    /**< File id */
    uint8_t       rsv1[13];              /**< 2 bytes align since we have 8 byte of meta-data in each sub-sector */
    uint8_t       ecc1[13];               /**< ECC Signature */

    /* sub-sector 2 in page x */
    uint32_t      cluster;                /**< Cluster number */
    uint8_t       lsn1;
    uint8_t       rsv2[14];
    uint8_t       ecc2[13];
    //uint8_t       align2[4];              /**< 4 bytes align since we have 8 byte of meta-data in each sub-sector */
    //uint8_t     ecc2[42];               /**< ECC Signature */

    /* sub-sector 3 in page x */
    uint32_t      erasesig[2];            /**< The erase signature created by devio_eraseblk */
    uint8_t       rsv3[11];
    uint8_t       ecc4[13];
    //uint8_t     ecc3[42];               /**< ECC Signature */
    //uint8_t     unused[24];             /**< Unused unprotected area */
//------------------------------------------------------------------------------
} spare_t;
//} spare32_t; /* for spare 2x */
#else
typedef struct {
    /* sub-sector 0 in page x */
    uint8_t       status;                 /**< Factory marking for bad block (0xff == GOOD) */
    uint8_t       status2;                /**< For 16 bit wide parts */
    uint16_t      nclusters;              /**< Number of clusters */
    uint8_t       lsn0;              /**< 4 bytes align since we have 8 byte of meta-data in each sub-sector */
    uint8_t       rsv0;               /**< ECC Signature */
    uint8_t       ecc0[10];               /**< ECC Signature */

    /* sub-sector 1 in page x */
    uint32_t      sequence;               /**< Sequence number */
    uint16_t      fid;                    /**< File id */
    uint8_t       ecc1[10];               /**< ECC Signature */

    /* sub-sector 2 in page x */
    uint32_t      cluster;                /**< Cluster number */
    uint8_t       lsn1;
    uint8_t       rsv2;
    uint8_t       ecc2[10];

    /* sub-sector 3 in page x */
    uint32_t      erasesig[2];            /**< The erase signature created by devio_eraseblk */
    uint8_t       ecc4[8];
} spare_t;
//} spare16_t; /* for spare 1x */
#endif

#define ERASESIG1       0x756c7769
#define ERASESIG2       0x70626d66

// These timeouts are very generous.
#define MAX_RESET_USEC  600     // 600us
#define MAX_READ_USEC   50      //  50us
#define MAX_POST_USEC   2000    //   2ms
#define MAX_ERASE_USEC  10000   //  10ms

/** @}*/

#endif // DEVIO_H_
