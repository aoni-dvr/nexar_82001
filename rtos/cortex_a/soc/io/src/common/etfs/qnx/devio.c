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

#include <arm/inout.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <gulliver.h>
#include <sys/slog.h>
#include <sys/neutrino.h>
#include <fs/etfs.h>
#include <sys/mman.h>

#include "devio.h"
#include "AmbaTypes.h"
#include "AmbaNAND.h"
#include "AmbaNAND_OP.h"

/**
 * @file       devio.c
 * @addtogroup etfs
 * @{
 */

static UINT8 *pETFS_PageBuf = NULL;

static UINT32 BlkOffset = 0;

int main(int argc, char *argv[])
{
#if defined (CONFIG_ENABLE_NAND_BOOT) || defined (CONFIG_ENABLE_SPINAND_BOOT)
    return(etfs_main(argc, argv));
#else
    return EOK;
#endif
}
#if 0
void AmbaETFS_SetTrp(struct etfs_devio *dev, unsigned cluster, void *pSpare, struct etfs_trans *trp)
{
    spare_t *spare = ;

    if (spare->status != 0xFF || spare->status2 != 0xFF) {
        dev->log(_SLOG_ERROR, "devio_readcluster: readtrans BADBLK on cluster %d", cluster);
        trp->tacode = ETFS_TRANS_BADBLK;
    }
    /* Build transaction data from data in the spare area */
    trp->sequence  = ENDIAN_LE32(spare->sequence);
    trp->fid       = ENDIAN_LE16(spare->fid);
    trp->cluster   = ENDIAN_LE32(spare->cluster);
    trp->nclusters = spare->nclusters
}
#endif
/**
 * Process device specific options (if any).
 * This is always called before any access to the part.
 * It is called by the -D option to the file system. If no -D option is given,
 * this function will still be called with "" for optstr.
 *
 * @param dev    ETFS handle.
 * @param optstr Driver string options.
 *
 * @retval EOK D options processed successfully.
 * @retval -1 error in peripheral memory space allocation.
 * @retval EINVAL Invalid -D sub-option.
 */
int devio_options(struct etfs_devio *dev, char *optstr)
{
    return(EOK);
}

/**
 * Initialize the part and stuff physical parameters for the part.
 *
 * @param dev ETFS handle.
 *
 * @return EOK always.
 */
int devio_init(struct etfs_devio *dev)
{
#if defined (CONFIG_ENABLE_NAND_BOOT)
    extern AMBA_NAND_DEV_INFO_s AmbaNAND_DevInfo;
    Nand_Init();
#elif defined (CONFIG_ENABLE_SPINAND_BOOT)
    extern AMBA_SPINAND_DEV_INFO_s AmbaNAND_DevInfo;
    spinand_init();
#endif

    extern AMBA_USER_PARTITION_TABLE_s *pAmbaNandTblUserPart;

    BlkOffset =    pAmbaNandTblUserPart->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A].StartBlkAddr;
    dev->numblks = pAmbaNandTblUserPart->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A].BlkCount;

    /* These must be initialized here. All other numbers are built from them. */
    sprintf(dev->id, "%s", AmbaNAND_DevInfo.DevName);

    dev->memtype       = ETFS_MEMTYPE_NAND;
    /* We glue to physical pages at the driver and report back their combined size */
    dev->clustersize   = AmbaNAND_DevInfo.MainByteSize;
    dev->sparesize     = AmbaNAND_DevInfo.SpareByteSize;
    dev->clusters2blk  = AmbaNAND_DevInfo.BlockPageSize;
    dev->blksize       = (dev->clustersize + dev->sparesize) * dev->clusters2blk;

    {
        ULONG Addr;

        get_fio_work_buf(&Addr, dev->clustersize + dev->sparesize);
        AmbaMisra_TypeCast(&pETFS_PageBuf, &Addr);
    }

    return(EOK);
}

/**
 * Read a cluster of data.
 * Verify crc for both the spare area and the entire page (data + spare).
 * The passed buffer "buf" is larger than the cluster size. It can hold
 * PAGESIZE bytes. This is for convenience when reading data from the
 * device and calculating the crc. The work area after clustersize
 * bytes is ignored by the caller.
 *
 * @param dev     ETFS handle.
 * @param cluster Cluster (page) to read.
 * @param buf     Buffer to fill.
 * @param trp     Transaction pointer.
 *
 * @return Execution status (ETFS_TRANS_XXX).
 */
int devio_readcluster(struct etfs_devio *dev, unsigned cluster, uint8_t *buf, struct etfs_trans *trp)
{

    spare_t *spare = (spare_t*)&pETFS_PageBuf[dev->clustersize];
    uint32_t status;
    UINT32 page = cluster + (BlkOffset * dev->clusters2blk);

    status = AmbaNandOp_Read(page, 1U, pETFS_PageBuf, spare, 5000);
    if (status != 0) {
        fprintf(stderr, "%s %d fail\n", __func__, __LINE__);
    }

    memcpy(buf, pETFS_PageBuf, dev->clustersize + dev->sparesize);

    if (spare->status != 0xFF || spare->status2 != 0xFF) {
        dev->log(_SLOG_ERROR, "devio_readcluster: readtrans BADBLK on cluster %d", cluster);
        trp->tacode = ETFS_TRANS_BADBLK;
    }
    /* Build transaction data from data in the spare area */
    trp->sequence  = ENDIAN_LE32(spare->sequence);
    trp->fid       = ENDIAN_LE16(spare->fid);
    trp->cluster   = ENDIAN_LE32(spare->cluster);
    trp->nclusters = spare->nclusters;

    if ((trp->sequence == 0xFFFFFFFFUL) && (trp->fid == 0xFFFFU) &&
        (trp->cluster == 0xFFFFFFFFUL)  && (trp->nclusters == 0xFFFFU)) {
        if (spare->erasesig[0] == ERASESIG1 && spare->erasesig[1] == ERASESIG2) {
            trp->tacode = ETFS_TRANS_ERASED;
        } else {
            trp->tacode = ETFS_TRANS_FOXES;
        }
    } else {
        trp->tacode = ETFS_TRANS_OK;
    }

    trp->dacode = ETFS_TRANS_OK;
    return(trp->tacode);
}

/**
 * Read the spare area of a page (not the data) to return transaction information.
 * This called is used heavily on startup to process the transactions. It is
 * a cheaper call than devio_readcluster() since it reads less data and has a smaller
 * checksum to calculate.
 *
 * @param dev     ETFS handle.
 * @param cluster Cluster (page) to read.
 * @param trp     Transaction pointer.
 *
 * @return Exectution status (ETFS_TRANS_XXX).
 */
int devio_readtrans(struct etfs_devio *dev, unsigned cluster, struct etfs_trans *trp)
{
    spare_t *spare_p = (spare_t*)&pETFS_PageBuf[dev->clustersize];
    UINT32 page = cluster + (BlkOffset * dev->clusters2blk);

    uint32_t status;

    status = AmbaNandOp_Read(page, 1U, pETFS_PageBuf, spare_p, 5000U);
    if (status != 0) {
        fprintf(stderr, "%s %d fail\n", __func__, __LINE__);
    }

    if (trp) {
        trp->sequence   = ENDIAN_LE32(spare_p->sequence);
        trp->fid        = ENDIAN_LE16(spare_p->fid);
        trp->cluster    = ENDIAN_LE32(spare_p->cluster);
        trp->nclusters  = spare_p->nclusters;

        /* Check for erased block */
        if ((trp->sequence == 0xFFFFFFFFUL) && (trp->fid == 0xFFFFU) &&
            (trp->cluster == 0xFFFFFFFFUL) && (trp->nclusters == 0xFFFFU)) {
            /* Check for bad block (Refer Spare Area Buffer 8-bit organization) */
            if ((spare_p->status != 0xff) || (spare_p->status2 != 0xff)) {
                //dev->log(_SLOG_ERROR, "readtrans BADBLK on cluster %d. Status = 0x%x, Status 2 = 0x%x.", cluster, spare.status, spare.status2);
                return (ETFS_TRANS_BADBLK);
            }

            if ((spare_p->erasesig[0] == ERASESIG1) && (spare_p->erasesig[1] == ERASESIG2)) {
                trp->tacode = ETFS_TRANS_ERASED;
                return (ETFS_TRANS_ERASED);
            } else {
                trp->tacode = ETFS_TRANS_FOXES;
                return (ETFS_TRANS_FOXES);
            }
        }
        trp->tacode = ETFS_TRANS_OK;
        if (status != 0) {
            fprintf(stderr, "%s %d data error\n", __func__, __LINE__);
            trp->tacode = ETFS_TRANS_DATAERR;
        }
    }

    return ETFS_TRANS_OK;
}

/**
 * Post a cluster of data.
 * Set crc for both the spare area and the entire page (data + spare).
 * The passed buffer "buf" is larger than the cluster size. It can hold
 * PAGESIZE bytes. This is for convenience writing data to the device and
 * calculating the crc. The work area after clustersize bytes is ignored
 * by the caller.
 *
 * @param dev     Device structure.
 * @param cluster Current cluster (page).
 * @param buf     Data buffer.
 * @param trp     Transaction pointer.
 *
 * @return Execution status (ETFS_TRANS_XXX).
 */
int devio_postcluster(struct etfs_devio *dev, unsigned cluster, uint8_t *buf, struct etfs_trans *trp)
{
    spare_t *spare;
    uint32_t status = 0;
    UINT32 page = cluster + (BlkOffset * dev->clusters2blk);

    memcpy(pETFS_PageBuf, buf, dev->clustersize + dev->sparesize);

    // Build spare area
    spare = (spare_t *) &pETFS_PageBuf[dev->clustersize];

    if (trp) {
        spare->sequence = ENDIAN_LE32(trp->sequence);
        spare->fid = ENDIAN_LE16((uint16_t) trp->fid);
        spare->cluster = ENDIAN_LE32(trp->cluster);
        spare->nclusters = trp->nclusters;
        spare->erasesig[0] = 0;
        spare->erasesig[1] = 0;

        spare->status = 0xffU;
        spare->status2 = 0xffU;
    } else {
        // called from eraseblk
        spare->erasesig[0] = ERASESIG1;
        spare->erasesig[1] = ERASESIG2;
    }

    status = AmbaNandOp_Program(page, 1U, pETFS_PageBuf, (unsigned char *)spare, 5000U);
    if (status != 0) {
        fprintf(stderr, "%s %d fail\n", __func__, __LINE__);
    }

    return(ETFS_TRANS_OK);
}

/**
 * Erase a block.
 *
 * @param dev ETFS handle.
 * @param blk Block to erase (blk is 0,1,2,...).
 *
 * @return Execution status (ETFS_TRANS_XXX).
 */
int devio_eraseblk(struct etfs_devio *dev, unsigned blk)
{
    uint32_t status;
    uint32_t page_size = dev->clustersize + dev->sparesize;
    uint32_t page = blk * dev->clusters2blk;

    status = AmbaNandOp_EraseBlock(blk + BlkOffset, 5000U);
    if (status != 0) {
        fprintf(stderr, "%s %d fail\n", __func__, __LINE__);
        dev->log(_SLOG_CRITICAL, "Error in block erase.");
    }

    /*
     * Write the erase signature. We only write non FFs in the erase signature bytes
     * of the spare area and put FFs everywhere else. Also disables ECC for this writes.
     */
#if 0
    uint8_t *buf = pETFS_PageBuf;
    memset(buf, 0xff, page_size);
    status = devio_postcluster(dev, page, buf, NULL);
#endif
    return(status);
}

/**
 * Called to allow the driver to flush any cached data that
 * has not be written to the device. The NAND class driver does
 * not need it.
 *
 * @param dev ETFS handle.
 *
 * @return -1 always
 */
int devio_sync(struct etfs_devio *dev)
{
    (void)dev;
    return -1;
}

/** @}*/
