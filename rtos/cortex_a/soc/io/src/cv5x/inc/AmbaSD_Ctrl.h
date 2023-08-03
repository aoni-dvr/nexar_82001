/**
 *  @file AmbaSD_Ctrl.h
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
 *  @details Definitions & Constants for SD Controller APIs (Internal Use)
 *
 */

#ifndef AMBA_SD_CTRL_H
#define AMBA_SD_CTRL_H

#ifndef AMBA_SD_STD_H
#include "AmbaSD_STD.h"
#endif

#ifndef AMBA_SD_DEF_H
#include "AmbaSD_Def.h"
#endif


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
 * Definitions for SD Memory Card Information
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef struct {
    UINT32  DeviceSize;                 /* [69:48]: Device size */
    UINT16  CardCommandClass;           /* [95:84]: Card command classes */

    UINT8   Taac;                       /* [119:112]: Data read access-time */
    UINT8   Nsac;                       /* [111:104]: Data read access-time in CLK cycles (NSAC*100) */
    UINT8   TranSpeed;                  /* [103:96]: Max. data transfer rate */
    UINT8   ReadBlockLength;            /* [83:80]: Max. read data block length */
    UINT8   ReadBlockPartial;           /* [79]: Partial blocks for read allowed */
    UINT8   WriteBlockMisalign;         /* [78]: Write block misalignment */
    UINT8   ReadBlockMisalign;          /* [77]: Read block misalignment */
    UINT8   DsrImplemented;             /* [76]: DSR implemented */
    UINT8   EraseBlockEnable;           /* [46]: Erase single block enable */
    UINT8   SectorSize;                 /* [45:39]: Erase sector size */
    UINT8   WriteGroupSize;             /* [38:32]: Write protect group size */
    UINT8   WirteGroupEnable;           /* [31]: Write protect group enable */
    UINT8   WriteSpeedFactor;           /* [28:26]: Write speed factor */
    UINT8   WirteBlockLength;           /* [25:22]: Max. write data block length */
    UINT8   WriteBlockPartial;          /* [21]: partial blocks for write allowed */
    UINT8   FileFormatGroup;            /* [15]: File format group */
    UINT8   Copy;                       /* [14]: Copy flag */
    UINT8   PermWriteProtect;           /* [13]: Permanent write protection */
    UINT8   TempWriteProtect;           /* [12]: Temporary write protection */
    UINT8   FileFormat;                 /* [11:10]: file format */

    UINT8   BusWidth;
    UINT8   UhsSupport;                 /* 1 - UHS Support */
    UINT8   AddressMode;                /* Address mode */
    UINT8   DataStateAfterErase;        /* Data State after erase */
    UINT8   CardIsInit;                 /* Card init status */
    UINT8   WriteProtect;               /* Write Protect */
    UINT8   AccessMode;                 /*  SD access mode */
    UINT32  CurrentClock;               /* Actual clock freqency */
    UINT32  DesiredClock;               /* Desired clock freqency */
    UINT32  TotalSectors;               /* Card capacity */
    UINT32  SDSupport4Bit;              /* Uhs SD card can suppprt 4 bit bus width mode */
} AMBA_SD_MEM_CARD_INFO_s;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
 * Definitions for SD IO Card Information
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef struct {
    AMBA_SDIO_RESPONSE_R4_s Cmd5_R4;                    /* R4 (IO_SEND_OP_COND Response) */
    UINT32  MaxBlkSize[AMBA_MAX_NUM_SDIO_FUNCTION];     /* maximum block size in Byte: 1 - 2048 Bytes */
    UINT32  CurBlkSize[AMBA_MAX_NUM_SDIO_FUNCTION];     /* current block size in Byte: 1 - 2048 Bytes */
    AMBA_SD_INFO_OCR_REG_s      OpCondition;            /* OCR (Operation conditions, Width = 32 bits) */
    AMBA_SD_INFO_CID_REG_s      CardID;                 /* CID (Card identification number, Width = 128 bits) */
    AMBA_SD_INFO_CSD_REG_s      CardSpecificData;       /* CSD (Card Specific Data, Width = 128 bits) */
    AMBA_SD_IO_INFO_CAP_REG_s   CardCapability;         /* CCCR(Card Command Control Data, Width = 8 bits) */

    UINT8   BusWidth;
    UINT32  CurrentClock;               /* Actual clock freqency */
    UINT32  DesiredClock;               /* Desired clock freqency */
    UINT8   CardIsInit;                 /* Card init status */
} AMBA_SD_IO_CARD_INFO_s;

typedef struct {
    AMBA_SD_MEM_CARD_INFO_s  MemCardInfo;
    AMBA_SD_IO_CARD_INFO_s   IoCardInfo;
} AMBA_SD_CARD_INFO_s;

typedef struct {
    AMBA_SD_CONFIG_s    SdConfig;                   /* SD Configurations */

    UINT32              SdType;                     /* SD Card Type */
    UINT8               NumIoFunction;              /* Number of I/O functions */
    UINT32              LastRoundDelay;             /* Last SD Card round delay value */

    AMBA_SD_CARD_INFO_s CardInfo;                   /* Memory Card Information */
    AMBA_SD_INFO_REG_s  CardRegs;                   /* SD Info Register Value */

    UINT32              (*GetClock)(void);          /* pointer to the API to get SD clock */
    UINT32                 (*SetClock)(UINT32 Frequency);        /* pointer to the API to set SD clock */
} AMBA_SD_CTRL_s;

#define PRF2_IOSTAT_MAX_LVL    (16U)

/**
 * For IO statistics
 */
typedef struct {
    UINT32  enable;
    UINT64  rd_bytes;
    UINT64  wr_bytes;
    UINT32  rd_time;        /**< in ms */
    UINT32  wr_time;        /**< in ms */
    /* For read/write sectors distribution diagram */
    UINT32  rlvl[PRF2_IOSTAT_MAX_LVL];      /**< for read level */
    UINT32  wlvl[PRF2_IOSTAT_MAX_LVL];      /**< for write level */
} io_statistics_t;

/*
 * Definitions of global SD event flags
 */
#define AMBA_SD_EVENT_CARD_DETECT_MASK  (0xffU)    /* Byte-0: for Card Detect event flags */
#define AMBA_SD_EVENT_CARD_DETECT        (0x1U)    /* the flag of SD Card Detect */

/*
 * Defined in AmbaSD.c
 */
extern AMBA_SD_CTRL_s AmbaSD_Ctrl[AMBA_NUM_SD_CHANNEL];               /* SD Management Structure */
extern io_statistics_t iostat[AMBA_NUM_SD_CHANNEL];

UINT32 AmbaSD_IsWriteProtect(UINT32 SdChanNo);

UINT32 AmbaSD_Lock(UINT32 SdChanNo);
UINT32 AmbaSD_Unlock(UINT32 SdChanNo);

UINT32 AmbaSD_SetEventFlag(UINT32 SdEventFlag);
UINT32 AmbaSD_ClearEventFlag(UINT32 SdEventFlag);

UINT32 AmbaSD_ClearCtrlFlag(UINT32 SdChanNo);

UINT32 AmbaSD_WaitEventFlag(UINT32 SdEventFlag, UINT32 AnyOrAll, UINT32 AutoClear, UINT32 Timeout);


void AmbaSD_MemCardInit(UINT32 SdChanNo);


UINT32 AmbaSD_SendCMD0(UINT32 SdChanNo);
UINT32 AmbaSD_SendCMD1(UINT32 SdChanNo, const AMBA_SD_CMD1_ARGUMENT_s * pCmd1, UINT32 * pOcr);
UINT32 AmbaSD_SendCMD2(UINT32 SdChanNo, AMBA_SD_INFO_CID_REG_s * pCid);
UINT32 AmbaSD_SendCMD3(UINT32 SdChanNo, UINT16 *pRCA);
UINT32 AmbaSD_SendCMD5(UINT32 SdChanNo, const AMBA_SDIO_CMD5_ARGUMENT_s *pSdioCmd5Arg, UINT32 *pOcr);
UINT32 AmbaSD_SendCMD6(UINT32 SdChanNo, const AMBA_SD_CMD6_ARGUMENT_s *pArgCmd6, AMBA_SD_CMD6_STATUS_s *pCmd6Status);
UINT32 AmbaSD_SendCMD7(UINT32 SdChanNo, UINT16 RCA);
UINT32 AmbaSD_SendCMD8(UINT32 SdChanNo);
UINT32 AmbaSD_SendCMD9(UINT32 SdChanNo, AMBA_SD_INFO_CSD_REG_s *pCsd);
UINT32 AmbaSD_SendCMD11(UINT32 SdChanNo);
UINT32 AmbaSD_SendCMD12(UINT32 SdChanNo);
UINT32 AmbaSD_SendCMD16(UINT32 SdChanNo, UINT32 BlockLength);
UINT32 AmbaSD_SendCMD17(UINT32 SdChanNo, UINT32 Sector, UINT8 *pBuf);
UINT32 AmbaSD_SendCMD18(UINT32 SdChanNo, UINT32 Sector, UINT32 Sectors, UINT8 *pBuf);
UINT32 AmbaSD_SendCMD19(UINT32 SdChanNo);
UINT32 AmbaSD_SendCMD20(UINT32 SdChanNo, UINT8 SpeedClassCtrl);
UINT32 AmbaSD_SendCMD24(UINT32 SdChanNo, UINT32 Sector, UINT8 *pBuf);
UINT32 AmbaSD_SendCMD25(UINT32 SdChanNo, UINT32 Sector, UINT32 Sectors, UINT8 *pBuf);
UINT32 AmbaSD_SendCMD32(UINT32 SdChanNo, UINT32 StartSector);
UINT32 AmbaSD_SendCMD33(UINT32 SdChanNo, UINT32 EndSector);
UINT32 AmbaSD_SendCMD38(UINT32 SdChanNo);
UINT32 AmbaSD_SendCMD52(UINT32 SdChanNo, const AMBA_SDIO_CMD52_ARGUMENT_s* pArgCmd52, UINT8* pDataBuf);
UINT32 AmbaSD_SendCMD53(UINT32 SdChanNo, const AMBA_SDIO_CMD53_ARGUMENT_s* pArgCmd53, UINT16 BlockSize, UINT8* pDataBuf);


UINT32 AmbaSD_SendCMD55(UINT32 SdChanNo);
UINT32 AmbaSD_SendACMD6(UINT32 SdChanNo, UINT8 BusWidth);
UINT32 AmbaSD_SendACMD41(UINT32 SdChanNo, const AMBA_SD_ACMD41_ARGUMENT_s *pAcmd41, UINT32 *pOcr);
UINT32 AmbaSD_SendACMD42(UINT32 SdChanNo, UINT8 SetCardDetect);
UINT32 AmbaSD_SendACMD51(UINT32 SdChanNo, AMBA_SD_INFO_SCR_REG_s *pScr);

UINT32 AmbaSD_SendEmmcCMD6(UINT32 SdChanNo, const AMBA_SD_EMMC_CMD6_ARGUMENT_s * pArgCmd6);
UINT32 AmbaSD_SendEmmcCMD8(UINT32 SdChanNo, AMBA_SD_INFO_EXT_CSD_REG_s * pExtCsd);
UINT32 AmbaSD_SendEmmcCMD21(UINT32 SdChanNo);

/* For HW Auto-Tuning */
UINT32 AmbaSD_ExecuteTuning(UINT32 SdChanNo);
/*
 * Defined in AmbaSD.c
 */
UINT32 AmbaSD_IsCardPresent(UINT32 SdChanNo);
UINT32 AmbaSD_GetTotalSectors(UINT32 SdChanNo);
UINT32 AmbaSD_EraseSector(UINT32 SdChanNo, UINT32 Sector, UINT32 Sectors);
UINT32 AmbaSD_ReadSector(UINT32 SdChanNo, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig);
UINT32 AmbaSD_WriteSector(UINT32 SdChanNo, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig);
UINT32 AmbaSD_UhsSupportChk(UINT32 SdChanNo);
void AmbaSD_SDDelayCtrlEnable(INT32 Enable);
void AmbaSD_SetHsRdLatency(UINT32 SdChanNo, UINT8 HsRdLatency);
UINT8 AmbaSD_GetHsRdLatency(UINT32 SdChanNo);

/* SD0 */
INT32 AmbaSD0_ReadSector(UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
INT32 AmbaSD0_WriteSector(UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
INT32 AmbaSD0_EraseSector(UINT32 Sector, UINT32 Sectors);

/* SD1 */
INT32 AmbaSD1_ReadSector(UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
INT32 AmbaSD1_WriteSector(UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
INT32 AmbaSD1_EraseSector(UINT32 Sector, UINT32 Sectors);

/* SD2 */
INT32 AmbaSD2_ReadSector(UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
INT32 AmbaSD2_WriteSector(UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
INT32 AmbaSD2_EraseSector(UINT32 Sector, UINT32 Sectors);


INT32 AmbaSD_GetResponse(UINT32 SdChanNo, UINT32 OpCode, UINT32 * pResp);

/*
 * Defined in AmbaSDIO.c
 */
void AmbaSD_IoCardInit(UINT32 SdChanNo);

/*
 * Defined in AmbaSD_Card.c
 */
UINT32 AmbaSD_CardInit(UINT32 SdChanNo);
INT32 AmbaSD_CardMonitorTaskCreate(UINT32 Priority);
INT32 AmbaSD_CardMonitorTaskStart(UINT32 Priority, UINT32 SmpCoreSet);
void AmbaSD_SetSDMaxClock(UINT32 SdChanNo, UINT32 SdMaxFrequency);
#ifdef CONFIG_QNX
void SD_CardMonitorTaskEntry(UINT32 EntryArg);
#else
void *SD_CardMonitorTaskEntry(void *EntryArg);
#endif
/*
 * Defined in AmbaSD_DelayCtrl.c
 */
UINT32 AmbaSD_SetDriveStrength(UINT32 SdChanNo, const AMBA_SD_SETTING_s * pSdSetting);
void AmbaSD_DelayCtrlSet(UINT32 SdChanNo, UINT32 ClkBypass, UINT32 RxClkPol, UINT32 SbcCore, UINT32 SelValue, UINT32 DinClkPol, UINT32 CmdBypass);
UINT32 AmbaSD_DelayCtrlGet(UINT32 SdChanNo);
void AmbaSD_DelayCtrlReset(UINT32 SdChanNo);

/*
 * Defined in AmbaEMMC_Partition.c
 */
UINT32 AmbaEMMC_InitPtbBbt(UINT32 TimeOut);
UINT32 AmbaEMMC_CreatePTB(void);
UINT32 AmbaEMMC_CreateExtUserPTB(const AMBA_PARTITION_CONFIG_s *pUserPartConfig, AMBA_USER_PARTITION_TABLE_s * pUserPartTable, UINT32 StartBlk);
UINT32 AmbaEMMC_LoadNvmRomFileTable(void);


UINT32 AmbaEMMC_GetRomFileSize(UINT32 RegionID, const char *pFileName, UINT32 *pFileSize);
UINT32 AmbaEMMC_GetRomFileInfo(UINT32 RegionID, UINT32 Index, AMBA_NVM_ROM_FILE_INFO_s *pRomFileInfo);
UINT32 AmbaEMMC_ReadRomFile(UINT32 RegionID, const char *pFileName, UINT32 StartPos,
                            UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut);

UINT32 AmbaEMMC_GetFtlInfo(UINT32 UserPartID, AMBA_NVM_FTL_INFO_s * pFtlInfo);
UINT32 AmbaEMMC_ReadSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut);
UINT32 AmbaEMMC_WriteSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut);
UINT32 AmbaEMMC_ReadUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut);
UINT32 AmbaEMMC_WriteUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut);

UINT32 AmbaEMMC_ReadPartitionInfo(UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo);
UINT32 AmbaEMMC_ReadPartition(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf, UINT32 TimeOut);
UINT32 AmbaEMMC_WritePartition(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf, UINT32 TimeOut);
UINT32 AmbaEMMC_InvalidatePartition(UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut);
UINT32 AmbaEMMC_ErasePartition(UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut);

UINT32 AmbaEMMC_ReadSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s * pSecConfig, UINT32 TimeOut);
UINT32 AmbaEMMC_WriteSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s * pSecConfig, UINT32 TimeOut);

UINT32 AmbaEMMC_IsBldMagicCodeSet(void);
UINT32 AmbaEMMC_SetBldMagicCode(UINT32 TimeOut);
UINT32 AmbaEMMC_EraseBldMagicCode(UINT32 TimeOut);
UINT32 AmbaEMMC_ReadBldMagicCode(void);
void AmbaEMMC_SetWritePtbFlag(UINT32 Flag);

UINT32 AmbaEMMC_SetActivePtbNo(UINT32 PtbNo, UINT32 Active);
UINT32 AmbaEMMC_GetActivePtbNo(UINT32 * pPTBActive);

UINT32 AmbaEMMC_WritePhyBlock(UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut);
UINT32 AmbaEMMC_ReadPhyBlock(UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut);

#endif /* _AMBA_SD_CTRL_H_ */
