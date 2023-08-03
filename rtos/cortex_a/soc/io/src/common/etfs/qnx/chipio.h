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

#ifndef CHIPIO_H_
#define CHIPIO_H_

#include <stdbool.h>
#include <stdio.h>
#include <sys/mman.h>

#include <arm/imx/imx_apbh.h>
#include <arm/imx/imx_gpmi.h>

/**
 * @file       imx-micron/chipio.h
 * @addtogroup etfs_chipio Chip interface
 * @{
 */

/** @name Internal NFC error codes */
#define NAND_EOK                0x55AA
#define NAND_EIO                0x3
#define NAND_DMA_EIO            0x33
/** @} */

/** @name Internal DMA flags - intended for run_dma method */
#define NAND_DMA_GPMI_TRANS     0x1
#define NAND_DMA_BCH_TRANS      0x2
/** @} */

/** @name Address size description */
#define NAND_COLUMN_ADDRESS_CYCLES      2
#define NAND_ROW_ADDRESS_CYCLES         3
#define NAND_ADDRESS_CYCLES (NAND_COLUMN_ADDRESS_CYCLES + NAND_ROW_ADDRESS_CYCLES)
/** @} */

/** @name Row and Column manipulation macros */
#define NAND_ADDR_COL1(addr)    ((addr) & 0xff)                 // Column 1
#define NAND_ADDR_COL2(addr)    (((addr) & 0x1f00) >> 8)        // Column 2
#define NAND_ADDR_ROW1(page)    ((page) & 0xff)                 // Row 1
#define NAND_ADDR_ROW2(page)    (((page) & 0xff00) >> 8)        // Row 2
#define NAND_ADDR_ROW3(page)    (((page) & 0x70000) >> 16)      // Row 3
/** @} */

/** @name Reset command description */
#define NANDCMD_RESET                   0xFF
#define NANDCMD_RESET_SIZE              1
/** @} */

/** @name Status command description */
#define NANDCMD_READ_STATUS_SIZE        1
#define NANDCMD_READ_STATUS             0x70
#define NANDCMD_READ_STATUS_ENHANCED    0x78
/** @} */

/** @name Read Id command description - read device followed by one address cycle to specify read ID type */
#define NANDCMD_READ_ID                 0x90
#define NANDCMD_READ_ID_TYPE            0x00
#define NANDCMD_READ_ID_SIZE            2       //!< Number of commands sent for a NAND Device Read ID.
#define NANDCMD_READ_ID_RESULT_SIZE     5       //!< Size in bytes of a Read ID command result.
/** @} */

/**  @name Block erase command description */
#define NANDCMD_BLOCK_ERASE             0x60
#define NANDCMD_BLOCK_ERASE_CONFIRM     0xD0
/** @} */

/** @name Read command description */
#define NANDCMD_READ                    0x00
#define NANDCMD_READ_CONFIRM            0x30
/** @} */

/** @name Program command description */
#define NANDCMD_PAGE_PROGRAM            0x80
#define NANDCMD_PAGE_PROGRAM_CONFIRM    0x10
#define NANDCMD_PAGE_CACHE_PROG_CFRM    0x15
/** @} */

/** Define the APBH DMA structure without GPMI transfers. */
typedef struct _apbh_dma_t {
    struct _apbh_dma_t*   nxt;
    imx_apbh_chn_cmd_t     cmd;
    void*                 bar;
} apbh_dma_t;

/** Define the APBH DMA structure with 1 GPMI Parameter word writes. */
typedef struct _apbh_dma_gpmi1_t {
    struct _apbh_dma_gpmi1_t*   nxt;
    imx_apbh_chn_cmd_t           cmd;
    void*                       bar;
    imx_gpmi_ctrl0_t       gpmi_ctrl0;
} apbh_dma_gpmi1_t;

/** Define the APBH DMA structure with 3 GPMI Parameter word writes. */
typedef struct _apbh_dma_gpmi3_t {
    struct _apbh_dma_gpmi1_t*     nxt;
    imx_apbh_chn_cmd_t             cmd;
    void*                         bar;
    imx_gpmi_ctrl0_t       gpmi_ctrl0;
    imx_gpmi_compare_t     gpmi_compare;
    imx_gpmi_eccctrl_t     gpmi_eccctrl;
} apbh_dma_gpmi3_t;

/** Define the APBH DMA structure with 5 GPMI Parameter word writes. */
typedef struct _apbh_dma_gpmi5_t {
    struct _apbh_dma_gpmi1_t*     nxt;
    imx_apbh_chn_cmd_t             cmd;
    void*                         bar;
    imx_gpmi_ctrl0_t       gpmi_ctrl0;
    imx_gpmi_compare_t     gpmi_compare;
    imx_gpmi_eccctrl_t     gpmi_eccctrl;
    imx_gpmi_ecccount_t    gpmi_ecccount;
    imx_gpmi_payload_t     gpmi_payload;
} apbh_dma_gpmi5_t;

/** DMA chain structure for device erase block. */
typedef struct _dma_blk_erase_t {
    // descriptor sequence
    apbh_dma_gpmi3_t tx_cle1_row_dma;
    apbh_dma_gpmi1_t tx_cle2_dma;
    apbh_dma_gpmi1_t wait_dma;
    apbh_dma_gpmi1_t sense;

    // terminator functions
    apbh_dma_t success;
    apbh_dma_t failure;

    uint8_t address_and_cmd_buf[1 + NAND_ROW_ADDRESS_CYCLES];
    uint8_t erase_blk_confirm_cmd;
    //
    void *phy_addr;
} dma_blk_erase_t;

/** DMA chain structure for NAND Program. */
typedef struct _dma_programEcc_t {
    // descriptor sequence
    apbh_dma_gpmi3_t tx_cle1_addr_dma;
    apbh_dma_gpmi1_t wait1;
    apbh_dma_gpmi1_t sense1;
    apbh_dma_gpmi5_t tx_data_dma;
    apbh_dma_gpmi3_t tx_cle2_dma;
    apbh_dma_gpmi1_t wait_dma;
    apbh_dma_t sense;

    // terminator functions
    apbh_dma_t success;
    apbh_dma_t failure;

    // buffer for cmd and address
    uint8_t address_and_cmd_buf[NAND_ADDRESS_CYCLES + 1];
    //buffer for write confirm command
    uint8_t wr_confirm_command;
    //
    void *phy_addr;
} dma_programEcc_t;

/** DMA chain structure for NAND Program. */
typedef struct _dma_programRaw_t {
    // descriptor sequence
    apbh_dma_gpmi3_t tx_cle1_addr_dma;
    apbh_dma_gpmi3_t tx_data_dma;
    apbh_dma_gpmi3_t tx_cle2_dma;
    apbh_dma_gpmi1_t wait_dma;
    apbh_dma_t sense;

    // terminator functions
    apbh_dma_t success;
    apbh_dma_t failure;

    // buffer for cmd and address
    uint8_t address_and_cmd_buf[NAND_ADDRESS_CYCLES + 1];
    //buffer for write confirm command
    uint8_t wr_confirm_command;
    //
    void *phy_addr;
} dma_programRaw_t;

/** DMA chain structure for Raw Read Page */
typedef struct _dma_readEcc_device_t {
    // descriptor sequence
    apbh_dma_gpmi3_t cmd_addr;
#ifndef NAND_DEVICE_NO_READ_CMD_CONFIRM
    apbh_dma_gpmi1_t tx_cle2_dma;
#endif
    apbh_dma_gpmi1_t wait1;
    apbh_dma_gpmi1_t sense1;
    apbh_dma_gpmi5_t receive_data;
    apbh_dma_gpmi3_t wait2;
    apbh_dma_gpmi1_t sense2;

    // terminator functions
    apbh_dma_t success;
    apbh_dma_t failure;

    // read page buffer
    uint8_t read_confirm_command;
    uint8_t address_and_command_buf[NAND_ADDRESS_CYCLES + 1];
    //
    void *phy_addr;
} dma_readEcc_device_t;

/** DMA chain structure for Raw Read Page */
typedef struct _dma_readRaw_device_t {
    // descriptor sequence
    apbh_dma_gpmi3_t cmd_addr;
#ifndef NAND_DEVICE_NO_READ_CMD_CONFIRM
    apbh_dma_gpmi1_t tx_cle2_dma;
#endif
    apbh_dma_gpmi1_t wait1;
    apbh_dma_gpmi1_t sense1;
    apbh_dma_gpmi1_t receive_data;
    apbh_dma_gpmi1_t wait2;
    apbh_dma_gpmi1_t sense2;

    // terminator functions
    apbh_dma_t success;
    apbh_dma_t failure;

    // read page buffer
    uint8_t read_confirm_command;
    uint8_t address_and_command_buf[NAND_ADDRESS_CYCLES + 1];
} dma_readRaw_device_t;

/** DMA chain structure for Read Status. */
typedef struct _NAND_dma_read_status_device_t {
    // descriptor sequence
    apbh_dma_gpmi1_t wait1;
    apbh_dma_gpmi1_t sense1;
    apbh_dma_gpmi3_t read_status_cmd;
    apbh_dma_gpmi1_t receive_data;

    // terminator functions
    apbh_dma_t success;
    apbh_dma_t failure;

    // read status command descriptor
    uint8_t status_command;
    uint8_t status2_and_addr_command_buf[NAND_ROW_ADDRESS_CYCLES + 1];
} dma_read_status_t;

/** DMA chain structure for Reset Device */
typedef struct _dma_reset_device_t {
    // descriptor sequence
    apbh_dma_gpmi1_t wait1;
    apbh_dma_gpmi1_t sense1;
    apbh_dma_gpmi3_t reset_cmd;
    apbh_dma_gpmi1_t wait2;
    apbh_dma_gpmi1_t sense2;

    // terminator functions
    apbh_dma_t success;
    apbh_dma_t failure;

    // reset command descriptor
    uint8_t reset_command;
} dma_reset_device_t;

/** DMA chain structure for Read ID */
typedef struct _dma_read_id_device_t {
    // descriptor sequence
    apbh_dma_gpmi1_t wait1;
    apbh_dma_gpmi1_t sense1;
    apbh_dma_gpmi3_t read_id_cmd;
    apbh_dma_gpmi1_t receive_data;

    // terminator functions
    apbh_dma_t success;
    apbh_dma_t failure;

    // buffer for read id command
    uint8_t read_id_command_buf[NANDCMD_READ_ID_SIZE];
} dma_read_id_device_t;

/** Low level driver structure */
typedef struct _chipio_t {
    //
    uint8_t               addrcycles;
    uint8_t               numplanes;
    uint32_t              pages2blk;
    //
    uintptr_t             apbh_dma_reg_base;
    uintptr_t             gpmi_pio_reg_base;
    uintptr_t             bch_ecc_reg_base;
    //
    uint8_t               *v_data_fs_buf; /**< This is intended for ECC engine. Buffer contains only data and fs meta */
    void                  *p_data_fs_buf;
    uint8_t               *v_data_page_buf; /**< This is intended for RAW writes. Buffer contains data, ecc and fs meta */
    void                  *p_data_page_buf;
    //
    dma_readEcc_device_t  *dma_readEcc_device;
    dma_programEcc_t      *dma_ecc_wr_desc;
    dma_blk_erase_t       *dma_blk_erase_desc;
    dma_programRaw_t      *dma_raw_wr_desc;
    //
    int                   apbhirq;
    int                   apbhiid;
    struct sigevent       apbhevent;
    pthread_mutex_t       apbhmutex;
    pthread_cond_t        apbhcond;
    int                   apbhirq_expected;
    //
    int                   bchirq;
    int                   bchiid;
    struct sigevent       bchevent;
    pthread_mutex_t       bchmutex;
    pthread_cond_t        bchcond;
    int                   bchirq_expected;
} chipio;

void create_blockErase_descriptor(dma_blk_erase_t  *superStruct, unsigned page);

void create_writeRaw_descriptor(dma_programRaw_t *superStruct, unsigned page, void *write_data, uint32_t data_size);

void create_writeEcc_descriptor(dma_programEcc_t *superStruct, unsigned page, void *write_data, uint32_t data_size);

void create_readEcc_descriptor(dma_readEcc_device_t *superStruct, unsigned page, void *ret_data);

void create_readRaw_descriptor(dma_readRaw_device_t *superStruct, uint32_t data_size, unsigned page, void *ret_data);

void create_readStatus_descriptor(dma_read_status_t *superStruct, uint8_t *ret_data);

void create_readStatus2_descriptor(dma_read_status_t *superStruct, uint8_t *memory_status, unsigned page);

void create_reset_descriptor(dma_reset_device_t *superStruct);

void create_readId_descriptor(dma_read_id_device_t *superStruct, uint8_t *ret_data);

void device_to_nfc(uint8_t *nfc_data, uint8_t *device_data);

void nfc_to_device(uint8_t *device_data, uint8_t *nfc_data, uint8_t ecc_size);

int run_dma(apbh_dma_t *dma, chipio *chipio, uint8_t wait_flag, uint32_t phy_addr);

bool is_dma_active(chipio *chipio);

int nand_init(chipio *cio);

int nand_wait_busy(chipio *cio, uint32_t time_out, uint8_t chip_select);

/**
 * Convert a virtual address to a physical address.
 *
 * @param virt_addr Virtual address.
 */
static inline void * virtual_to_physical_addr(const void * virt_addr)
{
    off64_t offset = 0x0;

    if (mem_offset64(virt_addr, NOFD, 1, &offset, 0) == -1) {
        printf("error in address conversion /n");
        return (void *)-1;
    }
    return (void *)(uint32_t)offset;
}

/** @}*/

#endif /* CHIPIO_H_ */
