/**
 *  @file AmbaNFTL.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for NAND flash translation layer APIs.
 *
 */

#ifndef AMBA_NFTL_H
#define AMBA_NFTL_H

/*
 * #deftions for NFTL partitions config.
 */
#define NAND_TRL_TABLES                 16U
#define NAND_SS_BLK_PAGES               1U
#define NAND_IMARK_PAGES                1U
#define NAND_BBINFO_SIZE                (4U * 1024U)

#define STG1_ZONE_THRESHOLD             1000U
#define STG1_RSV_BLOCKS_PER_ZONET       24U
#define STG1_MIN_RSV_BLOCKS_PER_ZONE    24U
#define STG1_TRL_TABLES                 NAND_TRL_TABLES

#define STG2_ZONE_THRESHOLD             1000U
#define STG2_RSV_BLOCKS_PER_ZONET       24U
#define STG2_MIN_RSV_BLOCKS_PER_ZONE    24U
#define STG2_TRL_TABLES                 NAND_TRL_TABLES

#define IDX_ZONE_THRESHOLD              1000U
#define IDX_RSV_BLOCKS_PER_ZONET        24U
#define IDX_MIN_RSV_BLOCKS_PER_ZONE     24U
#define IDX_TRL_TABLES                  NAND_TRL_TABLES

#define PRF_ZONE_THRESHOLD              1000U
#define PRF_RSV_BLOCKS_PER_ZONET        24U
#define PRF_MIN_RSV_BLOCKS_PER_ZONE     5U
#define PRF_TRL_TABLES                  0U

#define CAL_ZONE_THRESHOLD              1000U
#define CAL_RSV_BLOCKS_PER_ZONET        24U
#define CAL_MIN_RSV_BLOCKS_PER_ZONE     5U
#define CAL_TRL_TABLES                  NAND_TRL_TABLES

#define NAND_MAIN_512            512U
#define NAND_MAIN_512_SHT        9U
#define NAND_MAIN_2K            2048U
#define NAND_MAIN_4K            4096U
#define NAND_SPARE_128          128U
#define NAND_SPARE_256          256U
#define NAND_PPB_64_SHT         6U       /* 64 pages per block */
#define NAND_PPB_64             64U
#define NAND_SPP_8              8U       /* 8 sectors per page */
#define NAND_SPP_4              4U       /* 4 sectors per page */
#define NAND_SPB_256            256U     /* 256 sectors per block */
#define NAND_SPB_512            512U     /* 512 sectors per block */
#define    NAND_SPB_32_SHT         5U       /* 32 sectors per block */
#define NAND_TYPE_NONE          0U
#define NAND_TYPE_2K            2U
#define NAND_TYPE_4K            4U

#define NAND_OP_READ            0U
#define NAND_OP_PROG            1U
#define NAND_SEC_SIZE            512U
#define NAND_SEC_SHT            9U

#define NAND_ECC_BIT_1          0x1U
#define NAND_ECC_BIT_6          0x6U
#define NAND_ECC_BIT_8          0x8U
#define NAND_SPP_4_SHT             2U
#define NAND_SPP_8_SHT          3U

/*-
 *  initial bad block        : page 0|1 is not 0xff.
 * Late developed bad block : Page 2|3 is not 0xff.
 *  */
#define INIT_BAD_BLOCK_PAGES            2U
#define LATE_BAD_BLOCK_PAGES            2U
#define BAD_BLOCK_PAGES                 (INIT_BAD_BLOCK_PAGES + LATE_BAD_BLOCK_PAGES)

/*-
 * NAND operations error code definitions.
 */
#define NAND_OP_DRIVER_ER               -1
#define NAND_OP_ERASE_ER                -2
#define NAND_OP_PROG_ER                 -3
#define NAND_OP_READ_ER                 -4
#define NAND_OP_WP_ER                   -5
#define NAND_OP_NOT_READY_ER            -6
#define NAND_OP_MARK_BBLK_ER            -7
#define NAND_OP_BCH_FAILED              -8
#define NAND_OP_BCH_CORRECTED           -9
#define NAND_OP_TBL_CACHE_ER           -10
#define NAND_OP_ARG_ER                 -11

/*-
 * NAND with page 512 byte spare area layout. It follows the Samsung's
 * NAND spare area definition and with some private definition field.
 */
typedef struct {
    UINT8   Lsn[3];     /* Logical sector number */
    UINT16  Rsv1;       /* Reserved */
    UINT8   BI;         /* Bad block information */
    UINT8   Ecc[3];     /* Ecc code for main area data */
    UINT16  LsnEcc;     /* Ecc code for lsn data */
    UINT32  UspBlk;     /* Used sectors pattern in a block */
    UINT8   Rsv2;       /* Reserved */
} __attribute__((packed)) NAND_SPARE_LAYOUT_SMALL_s;

/*
 * NAND with page 2K byte spare area layout. It follows the Samsung's
 * NAND spare area definition and with some private definition field.
 */
typedef struct {
    UINT8   BI;         /* Bad block information */
    UINT8   Rsv1;       /* Reserved */
    UINT8   Lsn[3];     /* Logical sector number */
    UINT8   Pub;        /* Page used bit */
    UINT16  Rsv2;       /* Reserved */
    UINT8   Ecc[3];     /* Ecc code for main area data */
    UINT16  LsnEcc;     /* Ecc code for lsn data */
    UINT8   Rsv3[3];    /* Used sectors bit pattern */
} __attribute__((packed)) NAND_SPARE_LAYOUT_LARGE_s;

typedef struct {

    UINT32   StartBlock;             /**< Start block of device partition */
    UINT32   StartTrlTbl;            /**< Starting block of nftl init tbl */
    UINT32   TrlBlocks;              /**< Blocks used for translation tbl */
    UINT32   PagesPerTchunk;         /**< Pages of tchunk */
    UINT32   PartStartBlk;           /**< Starting block of ftl partition */
    UINT32   FtlBlocks;              /**< Total blocks in ftl partition */
    UINT32   TotalZones;             /**< Total zones in ftl partition */
    UINT32   TotalBlocks;            /**< Total blocks in partition */
    UINT32   PBlksPerZone;           /**< Physical blocks per zone */
    UINT32   LBlksPerZone;           /**< Logical blocks per zone */
    UINT32   RBlksPerZone;           /**< Reserved blocks per zone */
    UINT32   SectorsPerBlock;
    UINT32   SectorsPerPage;

} __attribute__((packed)) AMBA_NFTL_PART_INFO_s;

/*
 * Port level sturcture
 */

//#define ENABLE_VERIFY_NFTL                      1

/* The max number that bad block handler can be called */
#define NFTL_BBM_HANDLER_MAX                    15U

/* The max number of zone */
#define NFTL_MAX_ZONE                           256U

/* Number of 32-bit word to store used sectors bit pattern */
#define MAX_BIT_PAT_32                          (NAND_SPB_256 / 32U)

/* Define for nftl error codes. */
#define NFTL_FATAL_ER                           -11
#define NFTL_GENERAL_ER                         -12

/* Minimun number of translation tables in trl_blocks */
#define NFTL_TRL_TABLES                         16U
#define NFTL_SS_BLK_PAGES                       1U
#define NFTL_IMARK_PAGES                        1U

/* NFTL initialize mode */
#define NFTL_MODE_NO_SAVE_TRL_TBL               0x0U
#define NFTL_MODE_SAVE_TRL_TBL                  0x1U
#define NFTL_MODE_HAVE_TABLE_CACHE              0x2U

/*
 * NFTL information
 */
#define NFTL_UNUSED_BLK                         0xffffU

#define NFTL_FROM_DEV                           0U
#define NFTL_FROM_BUF                           1U
#define NFTL_FROM_BBINFO                        2U
#define NFTL_FROM_BBINFO_ICHUNK                 3U

/*
 * Definition for NAND flash translation layer
 */
#define NFTL_PSECTS                             0U
#define NFTL_PSECTS_NEW_BLOCK                   1U
#define NFTL_BLOCKS                             2U
#define NFTL_BLOCKS_PSECTS                      3U
#define NFTL_BLOCKS_PSECTS_NEW_BLOCK            4U

#define NFTL_DEBUG_MBUF_SIZE                    (128U * 2048U)
#define NFTL_DEBUG_SBUF_SIZE                    (64U * 256U)
#define NFTL_SPARE_BUF_SIZE                     (64U * 256U)
#define NFTL_PAGE_BUF_SIZE                      4096U
#define NFTL_BB_INFO_SIZE                       NAND_BBINFO_SIZE
#define NFTL_CHUNK_BUF_SIZE                     (8U * 2048U)

/*
 * The max bad blocks could be in ichunk
 */
#define NFTL_ICHUNK_MAX_BB                      16U

/*
 * Definitions for NFTL init mark
 */
#define NFTL_IMARK_MAGIC                        0x27398067U
#define NFTL_IMARK_VALID                        0xa5U
#define NFTL_IMARK_INVALID                      0x0U
#define NFTL_IMARK_SRL                          0x11335577U
/*
 * Descriptor for the NFTL arguments.
 */
typedef struct {
    UINT32 SecNum;              /* Start sector address in a page */
    UINT32 Page;                /* Start page address in a block */
    UINT32 Lba;                 /* Start logical block address */
    UINT32 SecsStart;           /* Sectors in start page */
    UINT32 PagesStartBlk;       /* Pages in start block */
    UINT32 SecsEndBlk;          /* Last sectors in start block */
    UINT32 Blocks;              /* Blocks to be transferred */
    UINT32 PagesEnd;            /* Pages in last block */
    UINT32 SecsEndPage;         /* Sectors in last page */
} AMBA_NFTL_ARGS_s;

/*
 * Descriptor for the NFTL bad block management.
 */
typedef struct {
    AMBA_NFTL_ARGS_s    Arg[NFTL_BBM_HANDLER_MAX];  /* Backup for the original request argument */

    UINT8   *pBuf;          /* Backup the original buffer pointer */
    UINT32  Condition;      /* Current NFTL write condition */
    UINT32  Pba;            /* Backup the original PBA */
    UINT32  *pPat;          /* Backup the original bit patern pointer */
    UINT32  Count;          /* Number of bad blk handler be called in the same wirte condition */
    UINT32  TotalCnt;       /* Total number of bad blk handler be called */
    UINT32  FailWrCnt;      /* Number of failed write operation */
    UINT32  FailRdCnt;      /* Number of failed read operation */
    UINT32  FailErCnt;      /* Number of failed erase operation */
} AMBA_NFTL_BBM_s;

/*
 * Descriptor for the NFTL bad block managerment status.
 */
typedef struct {
    UINT32 NumBB;           /* Number of bad blocks */
    UINT32 *pBBInfo;        /* Bad block information */
    UINT32 TotalCnt;        /* Total number of bad blk handler be called */
    UINT32 FailWrCnt;       /* Number of failed write operation */
    UINT32 FailRdCnt;       /* Number of failed read operation */
    UINT32 FailErCnt;       /* Number of failed erase operation */
} AMBA_NFTL_BBM_STATUS_s;

/*
 * Descriptor for the NFTL object
 */
typedef struct {
    UINT32  Init;                   /*Indicate the trial initialization */
    UINT32  Mode;
    UINT32  ID;                     /* ID for instances */

    UINT8   *pMainBuf;              /* Pointer to page data aligned buffer */
    UINT8   *pSpareBuf;             /* Pointer to spare data aligned buffer */
    UINT8   *pSecBuf;               /* Pointer to spare data aligned buffer */
    UINT8   *pMainBufRaw;           /* Pointer to page data buffer */
    UINT8   *pSpareBufRaw;          /* Pointer to spare data buffer */
    UINT32  *pBBInfo;               /* Pointer to bad block information aligned buffer */
    UINT32  *pBBInfoRaw;            /* Pointer to bad block information buffer */

    UINT32  TotalBlks;              /* Total blocks in current partition, not include trl_blocks */
    UINT32  TotalSecs;              /* Total avaiable logical sectors */
    UINT32  TotalZones;             /* Total zones the partition */
    UINT32  PageType;               /* 2k page size or 512 byte page size */

    /* Required members to do translation */
    UINT32  LBlksPerZone;           /* Logical blocks per zone */
    UINT32  PBlksPerZone;           /* Physical blocks per zone */
    UINT32  StartBlkPart;           /* Start block of partition */
    UINT32  MaxZones;

    UINT32  NumBB;                  /* Number of bad blocks */
    UINT32  SBlk[NFTL_MAX_ZONE];    /* Start block in a zone */
    UINT16  *pTrlTable;             /* Pointer to translation table */
    UINT16  *pPhyTable;             /* pointer to physical to logical table */


    AMBA_NFTL_BBM_s BBM;            /* Bad block managerment object */

    UINT8   WrProtect;              /* Partition is write protected or not? */

    UINT8   *pMainDebug;            /* Pointer to page data aligned buffer for debugging */
    UINT8   *pSpareDebug;           /* Pointer to spare data aligned buffer for debugging */
    UINT8   *pMainDebugRaw;         /* Pointer to page data buffer */
    UINT8   *pSpareDebugRaw;        /* Pointer to spare data buffer */
} AMBA_NFTL_OBJ_s;

/*
 * Descriptor for the NFTL status
 */
typedef struct {
    UINT32 StartIChunkBlk;
    UINT32 IChunkBlks;
    UINT32 StartFtlBlk;
    UINT32 FtlBlocks;
    UINT16 LBlksPerZone;
    UINT16 PBlksPerZone;
    UINT32 NumBB;
    UINT32 TotalSecs;
    UINT32 TotalZones;
    UINT32 WrProtect;
} AMBA_NFTL_STATUS_s;

/*
 * Descriptor for the report of NFTL unused sectors
 */
typedef struct {
    UINT32 Block;
    UINT32 Page;
    UINT32 Sector;
} AMBA_NFTL_USPAT_RPT_s;

/*
 * Defined in AmbaNFTL.c (NAND flash translation layer MW)
 */
UINT32 AmbaNFTL_InitLock(UINT32 ID);
UINT32 AmbaNFTL_DeinitLock(UINT32 ID);
UINT32 AmbaNFTL_IsWrProtect(UINT32 ID);
UINT32 AmbaNFTL_IsInit(UINT32 ID);
UINT32 AmbaNFTL_Init(UINT32 ID, UINT32 Mode);
UINT32 AmbaNFTL_Deinit(UINT32 ID);
UINT32 AmbaNFTL_ErasePart(UINT32 ID);
UINT32 AmbaNFTL_Read(UINT32 ID, UINT8 *pBuf, UINT32 Sec, UINT32 Secs);
UINT32 AmbaNFTL_Write(UINT32 ID, UINT8 *pBuf, UINT32 Sec, UINT32 Secs);
UINT32 AmbaNFTL_EraseLogicalBlocks(UINT32 ID, UINT32 StartBlock, INT32 Blocks);
UINT32 AmbaNFTL_GetTotalSecs(UINT32 ID, UINT32 *pTotalSecs);
UINT32 AmbaNAND_InitNftlPart(UINT32 ID, UINT32 StartBlk, UINT32 NumBlk);

#endif  /* AMBA_NFTL_H */

