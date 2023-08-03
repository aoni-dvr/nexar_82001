/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
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

#ifndef _AMBA_SD_MMC_INCLUDED
#define _AMBA_SD_MMC_INCLUDED

#define AMBA_SD_DMA_ADDR_R        0x00

#define AMBA_SD_BLOCK_SZ_R        0x04
    #define AMBA_SD_BLK_SDMA_BOUNDARY_512K    ( 7 << 12 )

#define AMBA_SD_BLOCK_CNT_R       0x06

#define AMBA_SD_ARG_R             0x08

#define AMBA_SD_TRNS_MODE_R       0x0C
    #define AMBA_SD_TRNS_DMA         0x01
    #define AMBA_SD_TRNS_BLK_CNT_EN  0x02
    #define AMBA_SD_TRNS_ACMD12      0x04
    #define AMBA_SD_TRNS_READ        0x10
    #define AMBA_SD_TRNS_MULTI       0x20

#define AMBA_SD_CMD_R             0x0E
    #define AMBA_SD_CMD_RESP_NONE    0x00
    #define AMBA_SD_CMD_RESP_136     0x01
    #define AMBA_SD_CMD_RESP_48      0x02
    #define AMBA_SD_CMD_RESP_48_BUSY 0x03
    #define AMBA_SD_CMD_CRC          0x08
    #define AMBA_SD_CMD_INDEX        0x10
    #define AMBA_SD_CMD_DATA         0x20

#define AMBA_SD_RSP_0             0x10
#define AMBA_SD_RSP_1             0x14
#define AMBA_SD_RSP_2             0x18
#define AMBA_SD_RSP_3             0x1C

#define AMBA_SD_BUFF_R            0x20

#define AMBA_SD_PRES_STATE_R      0x24
    #define AMBA_SD_CMD_INHIBIT      0x00000001
    #define AMBA_SD_DATA_INHIBIT     0x00000002
    #define AMBA_SD_DATA_ACTIVE      0x00000004
    #define AMBA_SD_WRITE_ACTIVE     0x00000100
    #define AMBA_SD_READ_ACTIVE      0x00000200
    #define AMBA_SD_CARD_INS         0x00010000
    #define AMBA_SD_CARD_DB          0x00020000
    #define AMBA_SD_CARD_DPL         0x00040000
    #define AMBA_SD_CARD_WP          0x00080000

#define AMBA_SD_HOST_CTRL_R       0x28
    #define AMBA_SD_DATA_SD4         0x00000002
    #define AMBA_SD_HIGH_SPEED       0x00000004
    #define AMBA_SD_CD_TEST_INS      0x00000040
    #define AMBA_SD_CD_TEST          0x00000080
    #define AMBA_SD_HCTL_DMA_MSK     (3 << 3)    // DMA select mask
    #define AMBA_SD_HCTL_SDMA        (0 << 3)    // SDMA select
    #define AMBA_SD_HCTL_ADMA32      (2 << 3)    // 32 bit ADMA select
    #define AMBA_SD_HCTL_ADMA64      (3 << 3)    // 64 bit ADMA select

#define AMBA_SD_PWR_CTRL_R        0x29
    #define AMBA_SD_POWER_ON         0x01
    #define AMBA_SD_POWER_18         0x0A
    #define AMBA_SD_POWER_30         0x0C
    #define AMBA_SD_POWER_33         0x0E

#define AMBA_SD_BLCK_GAP_CTL_R    0x2A

#define AMBA_SD_WAKE_CTL_R        0x2B

#define AMBA_SD_CLK_CTL_R         0x2C
    #define AMBA_SD_DIV_SHIFT        8
    #define AMBA_SD_CLK_DIV( _c )       ( ( (_c) << 8 ) | ( ( (_c) >> 2 ) & 0xe0 ) )
    #define AMBA_SD_CLK_AMBA_SD_EN  0x0004  // SDCLK enable
    #define AMBA_SD_CLK_INT_STABLE   0x0002  // Internal Clock stable
    #define AMBA_SD_CLK_INT_EN       0x0001  // Internal Clock enable
    #define AMBA_SD_CLK_SDCLKFS_MSK  0xffe0  // SDCLK frequency mask

#define AMBA_SD_TIMEOUT_CTL_R     0x2E
    #define AMBA_SD_TIMEOUT_DTO_MAX  0xE     // Timeout = TCF x 2^27

#define AMBA_SD_SOFT_RST_R        0x2F
    #define AMBA_SD_RST_ALL          0x01
    #define AMBA_SD_RST_CMD          0x02
    #define AMBA_SD_RST_DATA         0x04

#define AMBA_SD_INT_STAT_R        0x30
#define AMBA_SD_INT_STAT_E        0x32
#define AMBA_SD_INT_ENA_R         0x34
#define AMBA_SD_SIG_ENA_R         0x38
    #define AMBA_SD_INT_CMD_CMPL     0x00000001
    #define AMBA_SD_INT_TRNS_CMPL    0x00000002
    #define AMBA_SD_INT_DMA          0x00000008
    #define AMBA_SD_INT_ERROR        0x00008000
    #define AMBA_SD_INT_ERR_CTIMEOUT 0x00010000
    #define AMBA_SD_INT_ERR_CCRC     0x00020000
    #define AMBA_SD_INT_ERR_CEB      0x00040000
    #define AMBA_SD_INT_ERR_IDX      0x00080000
    #define AMBA_SD_INT_ERR_DTIMEOUT 0x00100000
    #define AMBA_SD_INT_ERR_DCRC     0x00200000
    #define AMBA_SD_INT_ERR_DEB      0x00400000
    #define AMBA_SD_INT_ERR_CLMT     0x00800000
    #define AMBA_SD_INT_ERR_ACMD12   0x01000000
    #define AMBA_SD_INT_ERR_ADMA     0x02000000
    #define AMBA_SD_INT_ERR_TRESP    0x10000000

#define AMBA_SD_CAPABILITIES_R    0x40
#define AMBA_SD_CAPABILITIES_R2   0x44

#define AMBA_SD_ADMA_ES           0x54
#define AMBA_SD_ADMA_ADDRL        0x58
#define AMBA_SD_ADMA_ADDRH        0x5C


#endif /* #ifndef _AMBA_SD_MMC_INCLUDED */

