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

#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "qspi_cmds.h"

/* ambarella */
#include "AmbaRTSL_SpiNOR.h"
#include "AmbaRTSL_SpiNOR_Ctrl.h"

/* Device Status Register Definitions */
#define DEVICE_SR_WIP    (1 << 0) /* Write in Progress (WIP) bit */
#define DEVICE_SR_WEL    (1 << 1) /* Write Enable Latch (WEL) bit */
#define DEVICE_SR_BP0    (1 << 2) /* Block Protect bit BP0 */
#define DEVICE_SR_BP1    (1 << 3) /* Block Protect bit BP1 */
#define DEVICE_SR_BP2    (1 << 4) /* Block Protect bit BP2 */
#define DEVICE_SR_E_ERR  (1 << 5) /* Erase Error Occurred */
#define DEVICE_SR_P_ERR  (1 << 5) /* Programming Error Occurred */
#define DEVICE_SR_RSVD   (3 << 5) /* bits 5 and 6 are reserved and always zero */
#define DEVICE_SR_SRWD   (1 << 7) /* Status Register Write Disable (SRWD) bit */

/* Device Flag Status Register Definitions */
#define DEVICE_FSR_PEC   (1 << 7) /* Program or erase controller is ready */

extern UINT32 AmbaSpiNOR_ReadStatus(UINT8 StatusNum, UINT8 *Status);
extern UINT32 AmbaNOR_SpiRead(UINT32 From, UINT32 ByteCount, UINT8 *pDataBuf, UINT32 TimeOut);
extern UINT32 AmbaSpiNOR_ReadFlagStatus(UINT8 *Status);
extern void AmbaSPINOR_Lock(void);
extern void AmbaSPINOR_UnLock(void);
extern UINT32 AmbaNOR_SpiWrite(UINT32 Offset, UINT32 ByteCount, const UINT8 *pDataBuf, UINT32 TimeOut);
extern UINT32 AmbaSpiNOR_WriteEnable(void);
extern UINT32 AmbaSpiNOR_SetExtAddr(void);
extern UINT32 AmbaSPINOR_ReadID(UINT8 *pId, UINT32 DataSize);
extern UINT32 AmbaSpiNOR_SoftReset(void);
extern UINT32 AmbaSpiNOR_EraseBlock(UINT32 Offset, UINT32 TimeOut);
extern UINT32 AmbaSPINOR_ReadSFDP(UINT8 *pBuf, UINT32 DataSize);

/* Read Status Register
 * Return -1 if failed to read
 * Return 0 if succeeded
 */
static int read_sr(const xzynq_qspi_t* qspi, uint32_t flags, uint8_t *sr)
{
    uint8_t cmd[4] = { FLASH_OPCODE_RDSR1, 0, 0, 0 };
    int     rc = 0;

    (void)qspi;
    /* ambarella */
    UINT8 Status = 0U;
    if (0 != AmbaSpiNOR_ReadStatus(1, &Status)) {
        rc = -1;
    }
    cmd[3] = Status;

    if (rc < 0) {
        fprintf(stderr, "norqspi: %s: Failed to read SR\n", __func__);
        return rc;
    }

    *sr = cmd[3];

    return rc;
}

/* Read Flag Status Register
 * Return -1 if failed to read
 * Return 0 if succeeded
 */
static int read_fsr(const xzynq_qspi_t* qspi, uint32_t flags, uint8_t *fsr)
{
    uint8_t cmd[4] = { FLASH_OPCODE_READ_FSR, 0, 0, 0 };
    int     rc = 0;

    (void)qspi;
    /* ambarella */
    UINT8 Status;
    if (0 != AmbaSpiNOR_ReadFlagStatus(&Status)) {
        rc = -1;
    } else {
        cmd[3] = Status;
    }

    *fsr = cmd[3];

    return rc;
}

/* Read Status Register on both LOW and UPPER devices
 * Return -1 if failed to read
 * Return 0 if not ready
 * Return 1 if all ready
 */
static int qspi_flash_sr_ready(const xzynq_qspi_t* qspi)
{
    int      ret;
    uint32_t flags = 0;
    uint8_t  sr_low = 0;

    /* ambarella */
    ret = read_sr(qspi, flags, &sr_low);

    if (ret < 0) return ret;
    if (sr_low & DEVICE_SR_WIP) return 0;

    return (1);
}

/* Read Flag Status Register on both LOW and UPPER devices
 * Return -1 if failed to read
 * Return 0 if not ready
 * Return 1 if all ready
 */
static int qspi_flash_fsr_ready(const xzynq_qspi_t* qspi)
{
    int      ret;
    uint32_t flags = 0;
    uint8_t  fsr_low = DEVICE_FSR_PEC;

    /* ambarella */
    ret = read_fsr(qspi, flags, &fsr_low);
    if (ret < 0) return ret;
    if ((fsr_low & DEVICE_FSR_PEC) == 0) return 0;

    return (1);
}

/* Must do a write enable immediately before a sector erase or program command */
static int write_enable(const xzynq_qspi_t* qspi)
{
    /* ambarella */
    int rc = 0;
    (void)qspi;

    if (0 != AmbaSpiNOR_WriteEnable()) {
        rc = -1;
    }

    if (-1 == rc) return rc;
    return EOK;
}

/* Check if the flash is busy
 * return 1 if device is busy
 * return 0 if device is not busy
 * return -1 for error
 */
int flash_busy(const xzynq_qspi_t* qspi)
{
    int sr, fsr;

    /* Check Status Register */
    sr = qspi_flash_sr_ready(qspi);
    if (sr < 0)  return sr;
    if (sr == 0) return 1;

    /* Check Flag Status Register. This is
     * necessary as indicated in the datasheet.
     */
    fsr = qspi_flash_fsr_ready(qspi);
    if (fsr < 0) return fsr;

    return (!fsr);
}

const xzynq_qspi_t* g_qspi;
/*
 * read identification
 * prereq: no write in progress
 */
int read_ident(const xzynq_qspi_t* qspi, uint8_t *ids, int flags)
{
    g_qspi = qspi;

    int      ret = 0;

    (void)flags;
    /* ambarella : Get the Maker ID and Device ID */
    if (SPINOR_ERR_NONE != AmbaSPINOR_ReadID(ids, IDENT_LEN)) {
        ret = -1;
    }

    if (ret < 0) return -1;

    return (EOK);
}

/*
 * release from power down
 * prereq: no write in progress
 */
int flash_reset(const xzynq_qspi_t* qspi)
{
    AmbaSpiNOR_SoftReset();
    enter_4byte_mode(qspi);

    return (EOK);
}

/*
 * erase sector at given offset
 * prereq: no write in progress
 */
int sector_erase(const xzynq_qspi_t* qspi, uint32_t offset)
{
    int     rc = 0;

    AmbaSPINOR_Lock();

    if (SPINOR_ERR_NONE != AmbaSpiNOR_EraseBlock(offset, 5000U)) {
        rc = -1;
    }
    AmbaSPINOR_UnLock();

    if (rc < 0) return rc;

    return EOK;
}

int enter_4byte_mode(const xzynq_qspi_t* qspi)
{
    int rc = 0;

    AmbaSPINOR_Lock();

    if (AmbaSpiNOR_SetExtAddr() != OK) {
        rc = -1;
    }

    AmbaSPINOR_UnLock();

    if (rc < 0) return rc;

    return EOK;
}


/*
 * program up to MAX_BURST-4 bytes at any given offset
 * prereq: no write in progress
 * returns number of bytes written
 */
int page_program(const xzynq_qspi_t* qspi, int offset, int len, uint8_t const* data)
{
    const int header_size = CMD_LEN + ADDR_LEN;
    int rc = 0;
    int dummy_write = 0;
    int nbytes = min(len, MAX_BURST - header_size);

    /* if writing all nbytes crosses a page boundary, then we reduce nbytes so that we write to the
     * end of the current page, but not beyond.
     */
    nbytes = min(nbytes, (offset & ~(qspi->page_size - 1)) + qspi->page_size - offset);

    AmbaSPINOR_Lock();

    if (0 != AmbaNOR_SpiWrite(offset, nbytes, data, 5000U)) {
        rc = -1;
    }

    AmbaSPINOR_UnLock();

    /* return with error if previous qspi_cmd_write failed */
    if (rc != EOK) return -1;

    return nbytes;
}

/*
 * read from open SPI flash from offset, up to MAX_BURST - header_size bytes
 * prereq: no write in progress
 * return len bytes read
 * return -1 if error
 */
int read_from(const xzynq_qspi_t* qspi, int offset, int len, uint8_t* buffer)
{
    const int header_size = CMD_LEN + ADDR_LEN + 1;
    const int max_read = MAX_BURST;
    uint8_t header[header_size];
    int rc = 0;
    int dummy_bytes = 0;

    (void)qspi;

    if (len == 0) return 0;

    len = min(len, max_read);

    AmbaSPINOR_Lock();

    if (0 != AmbaNOR_SpiRead(offset, len, buffer, 5000)) {
        rc = -1;
    }

    AmbaSPINOR_UnLock();
    if (-1 == rc) return rc;

    return len;
}

/*
 * read SFDP
 */
int read_sfdp(const xzynq_qspi_t* qspi, uint8_t *sfdp, int flags)
{
    int      ret = 0;

    AmbaSPINOR_Lock();

    if (SPINOR_ERR_NONE != AmbaSPINOR_ReadSFDP(sfdp, SFDP_LEN)) {
        ret = -1;
    }

    AmbaSPINOR_UnLock();
    if (ret < 0) {
        fprintf(stderr, "%s: Failed to read SFDP\n", __func__);
        return -1;
    }

    return (EOK);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION( "$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/hardware/flash/boards/qspi-xzynq/aarch64/le.zcu102/qspi_cmds.c $ $Rev: 864713 $" )
#endif
