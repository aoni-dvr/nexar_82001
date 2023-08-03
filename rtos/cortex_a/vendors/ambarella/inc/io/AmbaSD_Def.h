/**
 *  @file AmbaSD_Def.h
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
 *  @details Definitions & Constants for SD Controller APIs
 *
 */

#ifndef AMBA_SD_DEF_H
#define AMBA_SD_DEF_H

#ifndef AMBA_SD_PRIV_H
#include "AmbaSD_Priv.h"
#endif

#ifndef AMBA_NVM_PARTITION_H
#include "AmbaNVM_Partition.h"
#endif

#if (!defined(AMBA_FWPROG) && defined(CONFIG_THREADX))
#ifndef AMBA_KAL_H
#include "AmbaKAL.h"
#endif
#endif

#ifdef CONFIG_SOC_CV2FS
#define AMBA_EMMC_BOOTSTRAP_CODE_SPACE_SIZE     (8192U * 4U)/* Bootstrap Code space Byte size for NAND, increase size for BIST test  */
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
  #if defined(CONFIG_BST_LARGE_SIZE)
#define AMBA_EMMC_BOOTSTRAP_CODE_SPACE_SIZE     16384U      /* Bootstrap Code space Byte size for EMMC */
  #else
#define AMBA_EMMC_BOOTSTRAP_CODE_SPACE_SIZE     8192U       /* Bootstrap Code space Byte size for EMMC */
  #endif
#else
#define AMBA_EMMC_BOOTSTRAP_CODE_SPACE_SIZE     4096        /* Bootstrap Code space Byte size for EMMC */
#endif

#define AMBA_SD_NOT_PRESENT (0U)            /* the Card is not present */
#define AMBA_SD_MEMORY      (1U)            /* SD Memory card */
#define AMBA_SD_IO_ONLY     (2U)            /* SDIO only card */
#define AMBA_SD_COMBO       (3U)            /* Combo (SDIO with SD Memory integrated) card */
#define AMBA_SD_eMMC        (4U)            /* eMMC */

#define AMBA_SD_VDD_POWER_OFF (0U)
#define AMBA_SD_VDD_1D80V     (1U)
#define AMBA_SD_VDD_3D30V     (2U)
#define AMBA_SD_VDD_3D33V     (3U)
#define AMBA_SD_VDD_3D12V     (4U)

#define AMBA_SD_DRIVE_STRENGTH_2MA  (0U)
#define AMBA_SD_DRIVE_STRENGTH_8MA  (2U)
#define AMBA_SD_DRIVE_STRENGTH_4MA  (1U)
#define AMBA_SD_DRIVE_STRENGTH_12MA (3U)

typedef struct {
    UINT32  ClockDrive;
    UINT32  DataDrive;
    UINT32  CMDDrive;
    UINT32  CDDrive;
    UINT32  WPDrive;

    UINT32  DetailDelay;
    UINT8   HsRdLatency;                /* The rdlatency value when sd switch to hs*/

    UINT32  InitFrequency;
    UINT32  MaxFrequency;
} AMBA_SD_SETTING_s;

typedef struct {
    UINT32  NotUsed:             1; /* [0]: Not used, always 1 */
    UINT32  Crc7Checksum:        7; /* [7:1]: CRC (CRC7 checksum) */
    UINT8   ManufacturingMonth;     /* [11:8]: MDT-Month(Manufacturing date) */
    UINT8   ManufacturingYear;      /* [19:12]: MDT-Year (Manufacturing year, 0 = 2000) */
    UINT32  Reserved0:           4; /* [23:20]: Reserved */
    UINT32  ProductSerialNo;        /* [55:24]: PSN (Product serial number) */
    UINT8   ProductRevision;        /* [63:56]: PRV (Product revision) */
    UINT8   ProductName[5];         /* [103:64]: PNM (Product name) */
    UINT16  OemID;                  /* [119:104]: OID (OEM/Application ID) */
    UINT8   ManufacturerID;         /* [127:120]: MID (Manufacturer ID) */
} AMBA_SD_INFO_CID_INFO_s;

typedef void *(*SD_TASK_ENTRY_f)(void *EntryArg);

#if (!defined(AMBA_FWPROG) && defined(CONFIG_THREADX))
typedef UINT32 (*SD_SYSTEM_TASK_CREATE_CB_f)(SD_TASK_ENTRY_f TaskEntry, UINT32 Arg, AMBA_KAL_TASK_t **pTask);
#else
typedef UINT32 (*SD_SYSTEM_TASK_CREATE_CB_f)(SD_TASK_ENTRY_f TaskEntry, UINT32 Arg, void **pTask);
#endif

typedef void (*SD_POWER_CTRL_CB_f)(UINT32 SdChanNo, UINT32 VddPower);
typedef void (*SD_PHY_CTRL_CB_f)(UINT32 SdChanNo, AMBA_SD_INFO_CID_INFO_s *pCardID);
typedef void (*SDIO_CARD_ISR_CB_f)(void);

typedef struct {
    SD_POWER_CTRL_CB_f PowerCtrl;
    SD_PHY_CTRL_CB_f PhyCtrl;
    SDIO_CARD_ISR_CB_f SdioCardIsr;

    AMBA_SD_SETTING_s  SdSetting;

    UINT32  SmpCoreSet;                 /* A given set of CPUs on the SMP system allowed for SD card task */
    UINT32  Priority;                   /* SD init task priority */
    SD_SYSTEM_TASK_CREATE_CB_f  SystemTaskCreateCb;     /* [Input] System task create callback */

    AMBA_PARTITION_CONFIG_s *pSysPartConfig;    /* pointer to System partition configurations */
    AMBA_PARTITION_CONFIG_s *pUserPartConfig;   /* pointer to User partition configurations */
} AMBA_SD_CONFIG_s;

typedef struct {
    UINT32  SdType;             /* SD Card Type */
    UINT8   CardIsInit;
    UINT8   NumIoFunction;              /* Number of I/O functions */
    UINT8   DataAfterErase;
    UINT8   WriteProtect;
    UINT32  CardSpeed;                  /* Card speed */
    UINT64  CardSize;                   /* Card Size in Byte */
} AMBA_SD_CARD_STATUS_s;

typedef struct {
    UINT8   FunctionNo;                 /* Function Number: 0 - 7. 0 = Common I/O Area (CIA) */
    UINT8   IncAddrFlag;                /* 0 - Multi byte R/W to a fixed address; 1 -  Multi byte R/W to incrementing address */
    UINT8   ByteMode;                   /* 1 - Force to use byte mode to transfer */
    UINT32  RegAddr;                    /* start Register Address for R/W */
    UINT32  DataSize;                   /* Data size in Byte for Read/Write */
    UINT32  BlkSize;                    /* Block size in Byte */
    UINT8   *pDataBuf;                  /* pointer to the Data Buffer for Read/Write */
} AMBA_SDIO_RW_CONFIG_s;

#endif  /* AMBA_SD_DEF_H */
