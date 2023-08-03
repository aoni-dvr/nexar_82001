/*
 *  @file AmbaRTSL_SpiNOR_Ctrl.h
 *
 *  @Copyright      :: Copyright (C) 2019 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for NAND Controller RTSL APIs
 */

#ifndef AMBA_RTSL_NORSPI_CTRL_H
#define AMBA_RTSL_NORSPI_CTRL_H

#ifndef AMBA_NOR_SPI_DEF_H
#include "AmbaSpiNOR_Def.h"
#endif

/*
 * Macro Definitions
 */
#define AmbaRTSL_NorSpiDevInfo                 (AmbaRTSL_NorSpiCtrl.pNorDevInfo)

/*
 * NOR cmd type definitions.
 */
#define NOR_SPI_SEND_CMD      (0x1U)
#define NOR_SPI_READ_REG      (0x2U)
#define NOR_SPI_WRITE_DATA    (0x3U)
#define NOR_SPI_READ_DATA     (0x4U)
#define NOR_SPI_READ_DATA_DTR (0x5U)
#define NOR_SPI_READ_DATA_IO  (0x6U)
#define NOR_SPI_WRITE_REG     (0x7U)
#define NOR_SPI_WRITE_DATA_IO (0x8U)

#define NOR_SPI_OCT_READ_DATA  (0xAU)
#define NOR_SPI_OCT_WRITE_DATA (0xBU)

/*
 * the buffer size must align to 32 and smaller than the max size of DMA
 */
#define AMBA_SPINOR_DMA_BUF_SIZE    (128U * 1024U)
#define SPINOR_RESET_TIME           (250U)

/*
 * Data structure for SpiNOR flash memory device information
 */
typedef union {
    UINT16  Data;

    struct {
        UINT32   WriteInProgress:      1;  /* [0]: 1 - Device busy */
        UINT32   WriteEnableLatch:     1;  /* [1]: 1 - Device Accept Write Command */
        UINT32   BP0Protect:           1;  /* [2]: 1 - Protected selected range of block from Erase or Program */
        UINT32   BP1Protect:           1;  /* [3]: Protected selected range of block from Erase or Program */
        UINT32   BP2Protect:           1;  /* [4]: Protected selected range of block from Erase or Program */
        UINT32   ProgramError:         1;  /* [5]: 1 - Error ocurred of Program */
        UINT32   EraseError:           1;  /* [6]: 1 - Error ocurred of Program */
        UINT32   SRWD:                 1;  /* [7]: 1 - the register of Status and Configuration Write Disable */
    } Bits[2];
} AMBA_NORSPI_STATUS_u;

/*
 * NOR device Status error code definitions.
 */
#define NOR_STATUS_BLOCK_PROTECT2       ((UINT8)1U << 4U)
#define NOR_STATUS_BLOCK_PROTECT1       ((UINT8)1U << 3U)
#define NOR_STATUS_BLOCK_PROTECT0       ((UINT8)1U << 2U)
#define NOR_STATUS_WRTIE_ENABLE         ((UINT8)1U << 1U)
#define NOR_STATUS_WRTIE_IN_PROCESS     (1U)

/*
 * RTSL NOR_SPI Management Structure
 */
typedef struct {
    AMBA_NORSPI_DEV_INFO_s   *pNorDevInfo;     /* Pointer to external NAND device information */
    AMBA_PARTITION_CONFIG_s  *pSysPartConfig;  /* pointer to System partition configurations */
    AMBA_PARTITION_CONFIG_s  *pUserPartConfig; /* pointer to User Partition Configurations */
    AMBA_SERIAL_SPI_CONFIG_s *pSpiSetting;

    AMBA_NORSPI_STATUS_u  Status[2];           /* NAND command Response */
    UINT8  ManufactuerID;                      /* Manufactuer ID */
    UINT8  DeviceID;                           /* Device ID */
    UINT8  *pBuf;
    UINT32 TotalByteSize;                      /* Block size in Byte */
    UINT32 ProgramPageSize;                    /* Total number of blocks */
    UINT32 PartitionAllocateSize;              /* Allocation Size Unit for Sysyem & User Partition */
    UINT32 BstPageCount;                       /* Number of Pages for BST */
    UINT32 SysPtblPageCount;                   /* Number of Pages for System Partition Table */
    UINT32 UserPtblPageCount;                  /* Number of Pages for User Partition Table */
    UINT32 VendorDataPageCount;                /* Number of Pages for Vendor Specific Data */
    UINT32 SpiFrequncy;

    UINT32 NorSpiCtrlRegVal;
    UINT32 NorRxDMAChannel;
    UINT32 NorTxDMAChannel;
    UINT8  BusMode;
} AMBA_RTSL_NOR_SPI_CTRL_s;

/*
 * Defined in AmbaRTSL_NAND.c
 */
extern AMBA_RTSL_NOR_SPI_CTRL_s AmbaRTSL_NorSpiCtrl;
extern AMBA_SYS_PARTITION_TABLE_s  AmbaRTSL_NorSysPartTable GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
extern AMBA_USER_PARTITION_TABLE_s AmbaRTSL_NorUserPartTable GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

/*
 * Defined in AmbaRTSL_NORSPI_Partition.c
 */
extern AMBA_SYS_PARTITION_TABLE_s  *pAmbaRTSL_NorSpiSysPartTable;
extern AMBA_USER_PARTITION_TABLE_s *pAmbaRTSL_NorSpiUserPartTable;

#endif /* _AMBA_RTSL_NOR_SPI_CTRL_H_ */
