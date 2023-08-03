/**
 *  @file AmbaSpiNOR_Def.h
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
 *  @details Definitions & Constants for NOR Controller APIs
 *
 */

#ifndef AMBA_NOR_SPI_DEF_H
#define AMBA_NOR_SPI_DEF_H

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#ifndef AMBA_NVM_PARTITION_H
#include "AmbaNVM_Partition.h"
#endif

#define SPINOR_ERR_0000                (SPINOR_ERR_BASE)          /* Invalid argument */
#define SPINOR_ERR_0001                (SPINOR_ERR_BASE | 0x1U)   /* No enough block in NOR flash for create partition */
#define SPINOR_ERR_0002                (SPINOR_ERR_BASE | 0x2U)   /* NOR IO fail */
#define SPINOR_ERR_0003                (SPINOR_ERR_BASE | 0x3U)   /* Mutex fail */
#define SPINOR_ERR_0004                (SPINOR_ERR_BASE | 0x4U)   /* NOR device status error after access. */

#define SPINOR_ERR_NONE                OK /* Success */
#define SPINOR_ERR_ARG                 SPINOR_ERR_0000
#define SPINOR_ERR_NOT_ENOUGH_SPACE    SPINOR_ERR_0001
#define SPINOR_ERR_IO_FAIL             SPINOR_ERR_0002
#define SPINOR_ERR_OS_API_FAIL         SPINOR_ERR_0003
#define SPINOR_ERR_NOT_READY           SPINOR_ERR_0004


#ifdef CONFIG_SOC_CV2FS
#define AMBA_NORSPI_BOOTSTRAP_CODE_SPACE_SIZE     (8192U * 4U)   /* Bootstrap Code space Byte size for SPINOR */
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
 #if defined(CONFIG_BST_LARGE_SIZE)
#define AMBA_NORSPI_BOOTSTRAP_CODE_SPACE_SIZE     (16384U)  /* Bootstrap Code space Byte size for SPINOR */
  #else
#define AMBA_NORSPI_BOOTSTRAP_CODE_SPACE_SIZE     (8192U)   /* Bootstrap Code space Byte size for SPINOR */
  #endif
#else
#define AMBA_NORSPI_BOOTSTRAP_CODE_SPACE_SIZE     (4096U)        /* Bootstrap Code space Byte size for SPINOR */
#endif

#define AMBA_NORSPI_BOOT_HEADER_SIZE (128U)

/*
 * NOR Address type
 */
#define AMBA_NORSPI_3BYTE_ADDR (3U)
#define AMBA_NORSPI_4BYTE_ADDR (4U)

/*
 * NOR
 */
#define AMBA_NORSPI_DATA_1_LANE  (0U)
#define AMBA_NORSPI_DATA_2_LANE  (1U)
#define AMBA_NORSPI_DATA_4_LANE  (2U)
#define AMBA_NORSPI_DATA_8_LANE  (3U)

/*
 * NOR
 */
#define AMBA_NORSPI_READ           (0U) /* For Read with One I/O Ouput*/
#define AMBA_NORSPI_FAST_READ      (1U) /* For Read with Higher Frequency */
#define AMBA_NORSPI_DOR_READ       (2U) /* For Read with Dual Output (Data Two I/O Ouput, not Address)*/
#define AMBA_NORSPI_QOR_READ       (3U) /* For Read with Quad Output (Data Four I/O Output, not Address)*/
#define AMBA_NORSPI_DORIO_READ     (4U) /* For Read with Dual I/O Output (Data & Address Two I/O Pin)*/
#define AMBA_NORSPI_QORIO_READ     (5U) /* For Read with Quad I/O Output (Data & Address Four I/O Pin)*/
#define AMBA_NORSPI_DDR_DORIO_READ (6U) /* For Read with Dual I/O Output and Dual Edge Access */
#define AMBA_NORSPI_DDR_QORIO_READ (7U) /* For Read with Quad I/O Output and Dual Edge Access */

#define AMBA_NORSPI_OCT_READ       (8U)

/*
 * NOR
 */
#define AMBA_NORSPI_WRITE       (0U)  /* For Write with One I/O Ouput*/
#define AMBA_NORSPI_DORIO_WRITE (1U)  /* For Write with Dual I/O Output (Data & Address Two I/O Pin)*/
#define AMBA_NORSPI_QORIO_WRITE (2U)  /* For Write with Quad I/O Output (Data & Address Four I/O Pin)*/
#define AMBA_NORSPI_DOR_WRITE   (3U)  /* For Write with Dual Output (Data Dual I/O Output, not Address)*/
#define AMBA_NORSPI_QOR_WRITE   (4U)  /* For Write with Quad Output (Data Four I/O Output, not Address)*/

#define AMBA_NORSPI_OCT_WRITE   (5U)

typedef struct {
    UINT8  CmdIndex;  /* The index for Read Flash Array */
    UINT8  DummyCycles; /* The Dummy Cycles for Read Operator */
    UINT32 CmdType;
    UINT8  DataLane;
} AMBA_NORSPI_READCMD_SET_s;

typedef struct {
    UINT8  CmdIndex;    /* The index for Write Flash Array */
    UINT8  DummyCycles; /* The Dummy Cycles for Write Operator */
    UINT32 CmdType;
    UINT8  DataLane;
} AMBA_NORSPI_PROGRAM_SET_s;

typedef struct {
    UINT8  EnableQuadMode;    /* The CmdIndex to enable Quad Mode */
    UINT8  ReadQuadMode;      /* The CmdIndex to read the status of Quad Mode */
    UINT8  ExitQuadMode;      /* The CmdIndex to exit Quad Mode */
    UINT8  QuadFlag;
    UINT8  ExitQuadFlag;
    UINT8  QuadCmdArgLen;
} AMBA_NORSPI_QUADCMD_SET_s;

/*
 * Data structure describing a SpiNOR device(s) configuration.
 */
typedef struct {
    char   DevName[64];       /* pointer to the Device name */
    UINT8  ManufactuerID;   /* Manufactuer ID */
    UINT8  DeviceID;        /* Device ID */
    UINT32 TotalByteSize;   /* Total Size of SPINOR Device in Byte */
    UINT32 PageSize;        /* Program Page size in Byte  */
    UINT32 EraseBlockSize;  /* Erase Block Size in Byte & Allocation Unit For Partition*/
    UINT32 EraseSectorSize; /* Erase Sector Size in Byte */
    UINT32 SpiFrequncy;     /* The Frequency for SPI Interface */
    UINT8  AddrByte;
    /** Instruction Command Set **/
    UINT8   LSBFirst;       /* 0: MSB First, 1: LSB First */
    AMBA_NORSPI_READCMD_SET_s  Read;     /* The CMD set for Read Operator */
    AMBA_NORSPI_PROGRAM_SET_s  Program;  /* The CMD set for Write Operator */
    UINT8  WriteReg;        /* The Index of Writ Register CMD of NORSPI Device */
    UINT8  ReadID;          /* The Index of Read Electronic Manufacturer Signature CMD */
    UINT8  ResetEnable;     /* Enable Reset CMD. If don't support, pls set 0 */
    UINT8  Reset;           /* The Index of Rset NORSPI Device CMD. */
    UINT8  ReadStatus0;     /* The Index of Read Status0 CMD. */
    UINT8  ReadStatus1;     /* The Index of Read Status1 CMD. */
    UINT8  ReadFlagStatus;  /* The Index of Read Flag Status CMD. */
    UINT8  ClearStatus;     /* The Index of Clear Status1 CMD. */
    UINT8  WriteEnable;     /* The Index for Write Enable. */
    UINT8  WriteDisable;    /* The Index for Write Disable. */
    UINT8  EraseBlk;        /* The Index for Erase Device By BlockSize. */
    UINT8  EraseChip;       /* The Index for Erase all NORSPI Device. */
    UINT8  EraseSector;     /* The Index for Erase NORSPI Device bySector Size. */
    UINT8  Enable4ByteMode; /* Command to Enable 4Byte Address Mode */
    AMBA_NORSPI_QUADCMD_SET_s Quad; /* The CMD set For Quad Mode */
    UINT8  EraseFailFlag;   /* The Flag for Erase Fail. */
    UINT8  ProgramFailFlag; /* The Flag for Erase Fail. */
} AMBA_NORSPI_DEV_INFO_s;

typedef struct {
    UINT8 FlowControl;        /* 1: Enable Flow Control 0: Disable Flow Control */
    UINT8 HoldTime;           /* Using dq_o[x] as hold to do flow control */
    UINT8 TxThresholdLevel;  /* Transmit FIFO Threshold Level */
    UINT8 RxThresholdLevel;  /* Receive FIFO Threshold Level */
    UINT8 ChipSelect;
    UINT8 RxSampleDelay;      /* Adjust RX Sampling phase by SPI Cycle shift */
} AMBA_SERIAL_SPI_CONFIG_s;

/*
 * Data structure for NOR SPI flash software configurations
 */
typedef struct {
    AMBA_NORSPI_DEV_INFO_s   *pNorSpiDevInfo;    /* pointer to NORSPI flash device information */
    AMBA_PARTITION_CONFIG_s  *pSysPartConfig;    /* pointer to System partition configurations */
    AMBA_PARTITION_CONFIG_s  *pUserPartConfig;   /* pointer to User partition configurations */
    AMBA_SERIAL_SPI_CONFIG_s *SpiSetting;
} AMBA_NOR_SPI_CONFIG_s;

#endif  /* AMBA_NOR_SPI_DEF_H */
