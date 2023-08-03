/**
 *  @file AmbaNAND_Def.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for NAND Controller APIs
 *
 */

#ifndef AMBA_NAND_DEF_H
#define AMBA_NAND_DEF_H

#ifndef AMBA_NVM_PARTITION_H
#include "AmbaNVM_Partition.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#ifndef AMBAWRAP_H
#include "AmbaWrap.h"
#endif

#define NAND_ERR_0000                (NAND_ERR_BASE)   /* Invalid argument */
#define NAND_ERR_0001                (NAND_ERR_BASE | 0x1U)   /* No enough block in NAND flash for create partition */
#define NAND_ERR_0002                (NAND_ERR_BASE | 0x2U)   /* Too many bad blocks. */
#define NAND_ERR_0003                (NAND_ERR_BASE | 0x3U)   /* NAND IO fail */
#define NAND_ERR_0004                (NAND_ERR_BASE | 0x4U)   /* Mutex fail */
#define NAND_ERR_0005                (NAND_ERR_BASE | 0x5U)   /* ECC error */
#define NAND_ERR_0006                (NAND_ERR_BASE | 0x6U)   /* PTB error */
#define NAND_ERR_0007                (NAND_ERR_BASE | 0x7U)   /* Program error */
#define NAND_ERR_0008                (NAND_ERR_BASE | 0x8U)   /* Read error */

#define NAND_ERR_NONE                OK /* Success */
#define NAND_ERR_ARG                 NAND_ERR_0000
#define NAND_ERR_NOT_ENOUGH_SPACE    NAND_ERR_0001
#define NAND_ERR_BAD_BLOCK_OVER      NAND_ERR_0002
#define NAND_ERR_IO_FAIL             NAND_ERR_0003
#define NAND_ERR_OS_API_FAIL         NAND_ERR_0004
#define NAND_ERR_ECC                 NAND_ERR_0005
#define NAND_ERR_PTB                 NAND_ERR_0006
#define NAND_ERR_PROGRAM             NAND_ERR_0007
#define NAND_ERR_READ                NAND_ERR_0008

#if defined(CONFIG_SOC_CV2FS)
#define AMBA_NAND_BOOTSTRAP_CODE_SPACE_SIZE     (8192U * 4U)/* Bootstrap Code space Byte size for NAND, increase size for BIST test  */
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
  #if defined(CONFIG_BST_LARGE_SIZE)
#define AMBA_NAND_BOOTSTRAP_CODE_SPACE_SIZE     16384U      /* Bootstrap Code space Byte size for NAND */
  #else
#define AMBA_NAND_BOOTSTRAP_CODE_SPACE_SIZE     8192U       /* Bootstrap Code space Byte size for NAND */
  #endif
#else
#define AMBA_NAND_BOOTSTRAP_CODE_SPACE_SIZE     4096        /* Bootstrap Code space Byte size for NAND */
#endif
#define AMBA_NAND_1st_USER_PTB_SIZE     1U        /* The size of the User PTB  0 (block) */
#define AMBA_NAND_2nd_USER_PTB_SIZE     1U        /* The size of the User PTB  1 (block) */
#define AMBA_NAND_USER_PTB_SIZE    (AMBA_NAND_1st_USER_PTB_SIZE + AMBA_NAND_2nd_USER_PTB_SIZE)         /* The size of the User PTB (block) */


/*
 * Errcode for SPINAND flash
 */
#define SPINAND_ERR_0000                (SPINAND_ERR_BASE)   /* Invalid argument */
#define SPINAND_ERR_0001                (SPINAND_ERR_BASE | 0x1U)   /* No enough block in NAND flash for create partition */
#define SPINAND_ERR_0002                (SPINAND_ERR_BASE | 0x2U)   /* Too many bad blocks. */
#define SPINAND_ERR_0003                (SPINAND_ERR_BASE | 0x3U)   /* NAND IO fail */
#define SPINAND_ERR_0004                (SPINAND_ERR_BASE | 0x4U)   /* Mutex fail */
#define SPINAND_ERR_0005                (SPINAND_ERR_BASE | 0x5U)   /* ECC error */
#define SPINAND_ERR_0006                (SPINAND_ERR_BASE | 0x6U)   /* ECC error */

#define SPINAND_ERR_NONE                OK /* Success */
#define SPINAND_ERR_ARG                 NAND_ERR_0000
#define SPINAND_ERR_NOT_ENOUGH_SPACE    NAND_ERR_0001
#define SPINAND_ERR_BAD_BLOCK_OVER      NAND_ERR_0002
#define SPINAND_ERR_IO_FAIL             NAND_ERR_0003
#define SPINAND_ERR_OS_API_FAIL         NAND_ERR_0004
#define SPINAND_ERR_ECC                 NAND_ERR_0005
#define SPINAND_ERR_PTB                 NAND_ERR_0006
#define SPINAND_ERR_PROGRAM             NAND_ERR_0007
#define SPINAND_ERR_READ                NAND_ERR_0008

/*
 * Data structure for NAND flash memory device information
 */
#define AMBA_NAND_SIZE_64M_BITS  (0U)      /* 64M  bits, 2k page */
#define AMBA_NAND_SIZE_128M_BITS (1U)      /* 128M bits, 2k page */
#define AMBA_NAND_SIZE_256M_BITS (2U)      /* 256M bits, 2k page */
#define AMBA_NAND_SIZE_512M_BITS (3U)      /* 512M bits, 2k page */
#define AMBA_NAND_SIZE_1G_BITS   (4U)      /* 1G   bits, 4k/2k page */
#define AMBA_NAND_SIZE_2G_BITS   (5U)      /* 2G   bits, 4k/2k page */
#define AMBA_NAND_SIZE_4G_BITS   (6U)      /* 4G   bits, 4k/2k page */
#define AMBA_NAND_SIZE_8G_BITS   (7U)      /* 8G   bits, 4k/2k page */
#define AMBA_NAND_SIZE_16G_BITS  (8U)      /* 16G  bits, 4k/2k page */
#define AMBA_NAND_SIZE_32G_BITS  (9U)      /* 32G  bits, 4k/2k page */
#define AMBA_NAND_SIZE_64G_BITS  (10U)     /* 64G  bits, 4k/2k page */
#define AMBA_NAND_SIZE_128G_BITS (11U)     /* 128G bits, 4k/2k page */

#define AMBA_NAND_MAX_BYTE_SIZE     ((UINT32)1U << 30U)  /* maximum NAND device size in Byte can be supported */

typedef struct {
    char    DevName[64];            /* pointer to NAND Device name */

    UINT32  ChipSize;               /* the NAND Flash Memory Chip size */
    UINT32  MainByteSize;           /* Main area size in Byte */
    UINT32  SpareByteSize;          /* Spare area size in Byte */
    UINT32  BlockPageSize;          /* Block size in Page */
    UINT32  PlaneBlockSize;         /* Blocks per plane */
    UINT32  TotalPlanes;            /* Total number of planes */
    UINT32  ZoneBlockSize;          /* Blocks per zone */
    UINT32  TotalZones;             /* Total number of zones */

    UINT32  ColumnCycles;           /* Column access cycles */
    UINT32  PageCycles;             /* Page access cycles */
    UINT32  IdCycles;               /* ID read cycles */
    UINT32  PlaneAddrMask;          /* Mask of Plane Address */

    /** Chip Timing Parameters **/
    UINT8   tCLS;                   /* CLE Setup Time (ns) */
    UINT8   tCLH;                   /* CLE Hold Time (ns) */
    UINT8   tCS;                    /* CE setup Time (ns) */
    UINT8   tCH;                    /* CE Hold Time (ns) */
    UINT8   tWP;                    /* WE Pulse Width (ns) */
    UINT8   tALS;                   /* ALE Setup Time (ns) */
    UINT8   tALH;                   /* ALE Hold Time (ns) */
    UINT8   tDS;                    /* Data setup Time (ns) */
    UINT8   tDH;                    /* Data Hold Time (ns) */
    UINT8   tWH;                    /* WE High Hold Time (ns) */

    UINT8   tAR;                    /* ALE to RE Delay (ns) */
    UINT8   tCLR;                   /* CLE to REDelay (ns) */
    UINT8   tRR;                    /* Ready to RE Low (ns) */
    UINT8   tRP;                    /* RE Pulse Width (ns) */
    UINT8   tWB;                    /* WE High to Busy (ns) */
    UINT8   tREA;                   /* RE Access Time(tRDELAY) (ns) */
    UINT8   tRHZ;                   /* RE High to Output Hi-Z (ns) */
    UINT8   tREH;                   /* RE High Hold Time (ns) */
    UINT8   tIR;                    /* Output Hi-Z to RE Low (ns) */
    UINT8   tWHR;                   /* nWE High to nRE Low (ns) */

    UINT8   tRB;                    /* The same as tWB */
    UINT8   tCEH;                   /* tRHZ - tCHZ */
    UINT8   tWW;                    /* Write Protection time */

    UINT8   tCRL;                   /* CE Low to read pulse Low time, tCEA - tREA */
    UINT8   tADL;
    UINT8   tRHW;                   /* read pulse High to write pulse Low */

    UINT8   tRC;                    /* Read cycle time (ns), tRP + tREH >= tRC */

} AMBA_NAND_DEV_INFO_s;

#define AMBA_SPINAND_READCACHE_1LANE (0U) /* For Read with One I/O Ouput */
#define AMBA_SPINAND_READCACHE_2LANE (1U) /* For Read with Dual I/O Output */
#define AMBA_SPINAND_READCACHE_4LANE (2U) /* For Read with Qual I/O Output */
#define AMBA_SPINAND_READCACHE_DUAL  (3U) /* For Read with Qual I/O Output */
#define AMBA_SPINAND_READCACHE_QUAL  (4U) /* For Read with Qual I/O Output */

#define AMBA_SPINAND_PROGRAM_1LANE (0U)   /* For Program with One I/O Ouput*/
#define AMBA_SPINAND_PROGRAM_4LANE (1U)   /* For Program with Qual Output */

typedef struct {
    UINT8  ReadArray_AddrDummy;    /* The Dummy Cycles for Read Operator */
    UINT8  ReadCacheIndex;         /* The Index for ReadCache Cmd */
    UINT8  AddrDummy;              /* The Dummy Cycles for Read Operator */
    UINT8  DataDummy;              /* The Dummy Cycles for Read Operator */
    UINT32 CmdType;
} AMBA_SPINAND_READCMD_SET_s;

typedef struct {
    UINT8  LoadCmdIndex;               /* The index for ProgramLoad Array */
    UINT8  LoadAddrDummy;              /* The Dummy Cycles for Program Operator */
    UINT8  LoadDataDummy;              /* The Dummy Cycles for Program Operator */
    UINT32 LoadCmdType;
} AMBA_SPINAND_PROGRAM_SET_s;

#define AMBA_SPINAND_ERASE_ADDRBY_PAGE (0U)
#define AMBA_SPINAND_ERASE_ADDRBY_BYTE (1U)

#define AMBA_SPINAND_NOPLANE_SELECT    (0U)
#define AMBA_SPINAND_PLANE_SELECT_5BIT (1U)
#define AMBA_SPINAND_PLANE_SELECT_6BIT (2U)
#define AMBA_SPINAND_PLANE_SELECT_7BIT (3U)

#define AMBA_SPINAND_ENABLE_ONDEVICE_ECC (0U)
#define AMBA_SPINAND_DISABLE_ONDEVICE_ECC (1U)

typedef struct {
    char   DevName[64];            /* pointer to NAND Device name */

    UINT32  ChipSize;          /* the NAND Flash Memory Chip size */
    UINT32  MainByteSize;               /* Main area size in Byte */
    UINT32  SpareByteSize;              /* Spare area size in Byte */
    UINT32  BlockPageSize;              /* Block size in Page */
    UINT32  PlaneBlockSize;             /* Blocks per plane */
    UINT32  TotalPlanes;                /* Total number of planes */
    UINT32  ZoneBlockSize;              /* Blocks per zone */
    UINT32  TotalZones;                 /* Total number of zones */

    UINT8   ColumnCycles;               /* Column access cycles */
    UINT8   PageCycles;                 /* Page access cycles */
    UINT32  InternalECC;
    UINT32  EraseAddrType;
    UINT32  PlaneSelect;
    AMBA_SPINAND_READCMD_SET_s  Read;   /* The CMD set for Read Operator */
    AMBA_SPINAND_PROGRAM_SET_s  Program;/* The CMD set for Program Operator */

    /** Chip Timing Parameters **/
    UINT8    tCLQV;                     /* CLock Low to Read Data valid (ns) */
    UINT8    tCS;                       /* Command deselect time (ns) */
    UINT8    tCLL;                      /* Clock low time (ns)*/
    UINT8    tCLH;                      /* Clock High time (ns)*/
    UINT8    tSHC;                      /* CS Rising edge to SCK rising edge (ns) */
    UINT8    tCHS;                      /* SCK Rising edge to CS rising edge (ns), tCHSH */
    UINT8    tSLCH;                     /* CS Rising edge to SCK rising edge (ns) */

    UINT8    tCHSL;                     /* SCK Rising edge to CS falling edge (ns) */
    UINT8    tWPH;                      /* CS Rising Edge to WP falling edge (ns) */
    UINT8    tWPS;                      /* WP Rising edge to CS falling edge (ns) */
    UINT8    tHHQX;                     /* HOLD Rising edge to Read Data valid (ns) */
} AMBA_SPINAND_DEV_INFO_s;

#if 0 //SDK7
typedef struct {

    UINT32  MainByteSize;               /* Main area size in Byte */
    UINT32  SpareByteSize;              /* Spare area size in Byte */
    UINT32  BlockPageSize;              /* Block size in Page */
    UINT32  PlaneBlockSize;             /* Blocks per plane */
    UINT32  TotalPlanes;                /* Total number of planes */
    UINT32  ZoneBlockSize;              /* Blocks per zone */
    UINT32  TotalZones;                 /* Total number of zones */

    UINT32  PlaneAddrMask;              /* Mask of Plane Address */
} AMBA_NAND_COMMON_INFO_s;
#else
#define AMBA_NAND_COMMON_INFO_s  AMBA_NAND_DEV_INFO_s
#endif
/*
 * Data structure for NAND flash software configurations
 */
typedef struct {
    AMBA_NAND_DEV_INFO_s    *pNandDevInfo;      /* pointer to NAND flash device information */
    AMBA_PARTITION_CONFIG_s *pSysPartConfig;    /* pointer to System partition configurations */
    AMBA_PARTITION_CONFIG_s *pUserPartConfig;   /* pointer to User partition configurations */
    void (*AmbaNandPreAccessNotify)(void);      /* pointer to Call back function for User */
    void (*AmbaNandPostAccessNotify)(void);     /* pointer to Call back function for User */
} AMBA_NAND_CONFIG_s;

typedef struct {
    AMBA_SPINAND_DEV_INFO_s *pNandDevInfo;      /* pointer to SPINAND flash device information */
    AMBA_PARTITION_CONFIG_s *pSysPartConfig;    /* pointer to System partition configurations */
    AMBA_PARTITION_CONFIG_s *pUserPartConfig;   /* pointer to User partition configurations */
    void (*AmbaNandPreAccessNotify)(void);      /* pointer to Call back function for User */
    void (*AmbaNandPostAccessNotify)(void);     /* pointer to Call back function for User */
} AMBA_SPINAND_CONFIG_s;

#endif  /* AMBA_NAND_DEF_H */
