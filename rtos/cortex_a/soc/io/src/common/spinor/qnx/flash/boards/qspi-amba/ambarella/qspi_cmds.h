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

#ifndef QSPI_CMDS_H_
#define QSPI_CMDS_H_

#include <stdint.h>
#include "variant.h"

/* Test defines */
#define KILO(__n) ((__n) << 10)

/* Flash commands */
#define    FLASH_OPCODE_WRSR              0x01 /* Write status register */
#define    FLASH_OPCODE_PP                0x12 /* Page program */
#define    FLASH_OPCODE_NORM_READ         0x03 /* Normal read data bytes */
#define    FLASH_OPCODE_WRDS              0x04 /* Write disable */
#define    FLASH_OPCODE_RDSR1             0x05 /* Read status register 1 */
#define    FLASH_OPCODE_WREN              0x06 /* Write enable */
#define    FLASH_OPCODE_FAST_READ         0x0B /* Fast read data bytes */
#define    FLASH_OPCODE_BE_4K             0x21 /* Erase 4KiB block */
#define    FLASH_OPCODE_RDSR2             0x35 /* Read status register 2 */
#define    FLASH_OPCODE_DUAL_READ         0x3C /* Dual read data bytes */
#define    FLASH_OPCODE_BE_32K            0x52 /* Erase 32KiB block */
#define    FLASH_OPCODE_QUAD_READ         0x6B /* Quad read data bytes */
#define    FLASH_OPCODE_QUAD_4BYTE_READ   0x6C /* 4-Byte Quad read data bytes */
#define    FLASH_OPCODE_READ_FSR          0x70 /* Read flag status register */
#define    FLASH_OPCODE_ERASE_SUS         0x75 /* Erase suspend */
#define    FLASH_OPCODE_ERASE_RES         0x7A /* Erase resume */
#define    FLASH_OPCODE_RDID              0x9F /* Read JEDEC ID */
#define    FLASH_OPCODE_RSFDP             0x5A /* Read Serial Flash Discovery Parameter */
#define    FLASH_OPCODE_BE                0xC7 /* Erase whole flash block */
#define    FLASH_OPCODE_SE                0xD8 /* Sector erase (usually 64KB)*/
#define    FLASH_OPCODE_4BYTE_SE          0xDC /* 4-Byte Sector erase (usually 64KB)*/
#define    FLASH_OPCODE_QUAD_WRITE        0x32 /* QUAD input fast program */
#define    FLASH_OPCODE_QUAD_4BYTE_WRITE  0x34 /* 4-Byte QUAD input fast program */
#define    FLASH_OPCODE_4BYTE_MODE        0xB7 /* Enter in 4-byte mode */
#define    FLASH_OPCODE_RELEASE_PD        0xAB /* Release from power-down */
#define    FLASH_OPCODE_RSTEN             0x66 /* Reset enable */
#define    FLASH_OPCODE_RST               0x99 /* Reset */

int read_ident(const xzynq_qspi_t* qspi, uint8_t *ids, int flags);
int read_sfdp(const xzynq_qspi_t* qspi, uint8_t *sfdp, int flags);
int sector_erase(const xzynq_qspi_t* qspi, uint32_t offset);
int enter_4byte_mode(const xzynq_qspi_t* qspi);
int page_program(const xzynq_qspi_t* qspi, int offset, int len, uint8_t const* data);
int read_from(const xzynq_qspi_t* qspi, int offset, int len, uint8_t* buffer);
int flash_reset(const xzynq_qspi_t* qspi);
int flash_busy(const xzynq_qspi_t* qspi);

#endif /* QSPI_CMDS_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION( "$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/hardware/flash/boards/qspi-xzynq/aarch64/le.zcu102/qspi_cmds.h $ $Rev: 859471 $" )
#endif
