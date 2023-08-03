/**
 *  @file AmbaFS_Format.h
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
 *  @details Ambarella file system abstraction layer.
 */
#ifndef AMBA_FS_FORMAT_H
#define AMBA_FS_FORMAT_H

#ifndef     DRV_SUCCEED
#define     DRV_SUCCEED     0
#endif

#ifndef     DRV_FAILED
#define     DRV_FAILED      (-1)
#endif

#define PRF2_FORMAT_LAYER_SELECTABLE    1

#if (PRF2_FORMAT_LAYER_SELECTABLE)
#define PRF2_MEDIA_ATTR PDM_MEDIA_ATTR_NOT_EXE_LOGICAL_FORMAT
#else
#define PRF2_MEDIA_ATTR (0U)
#endif

#define SEC_SIZE            (512U) /* size of sector in byte */
#define SEC_SHIFT           (9U)   /* used to caculate sectors */
#define ROOT_DIR_ENTRY      (256U) /* number of root directory entries */
#define NFAT                (2U)   /* number of fat tables */
#define FAT32_BS_SIZE       (32U)  /* number of boot sectors for fat32 */
#define FAT16_BS_SIZE       (1U)   /* number of boot sector of fat16 */

#define RSV_MBR_CLUST       (1U)   /* reserved cluster for MBR */

#define NAND_NHEAD              (0x10U)
#define NAND_SPT                (0x3fU)

/* File System Description */
#define FS_FAT_CONTAIN_ERROR    (-3)    /**< File system contain currupt */
#define FS_FAT_ERROR            (-2)    /**< can't get file system info */
#define FS_FAT_NO_FORMAT        (-1)    /**< initial value of format field */
#define FS_FAT_UNKNOWN          (0x0U)
#define FS_FAT12                (0x01U)  /**< FAT12            */
#define FS_FAT16S               (0x04U)  /**< FAT16 < 32MB     */
#define FS_FAT16_EXT_DOS        (0x05U)  /**< Extended DOS partition (0~2GB) */
#define FS_FAT16                (0x06U)  /**< FAT16 >= 32MB    */
#define FS_FAT32                (0x0bU)  /**< FAT32            */
#define FS_FAT32L               (0x0cU)  /**< FAT32 require extend int13 support(LBA) */
#define FS_EXFAT                (0x07U)  /**< EX-FAT           */
#define FS_FAT16L               (0x0eU)  /**< FAT16 require extend int13 support(LBA) */
#define FS_FAT16L_EXT_DOS       (0x0fU)  /**< FAT16 require extend int13 support(LBA) */

#define FAT12_TYPE              (12U)
#define FAT16_TYPE              (16U)
#define FAT32_TYPE              (32U)
#define EXFAT_TYPE              (64U)

/**
 * PapRtition entry description.
 */
typedef struct  {
    UINT8   BootType;           /**< boot type 0x80: boot, 0x00: not boot */
    UINT8   StartHead;          /**< stapRt Head                          */
    UINT8   StartSector;        /**< stapRt Sector                        */
    UINT8   StartCylinder;      /**< stapRt Cylinder                      */
    UINT8   FatType;            /**< FAT TYPE                             */
    UINT8   EndHead;            /**< end Head                             */
    UINT8   EndSector;          /**< end Sector                           */
    UINT8   EndCylinder;        /**< end Cylinder                         */
    UINT32  LogicStartSector;   /**< logical start Sector                 */
    UINT32  AllSector;          /**< ALL Sectors                          */
} GNU_MIN_PADDING AMBA_FS_PART_ENTRY;

/**
 * PapRtition table for hard disk-like file system.
 */
typedef struct {
    UINT8       BootProgram[446];   /**< consistency check routine    */
    AMBA_FS_PART_ENTRY  PartTable0;    /**< partition table entry        */
    AMBA_FS_PART_ENTRY  PartTable1;    /**< papRtition table entry        */
    AMBA_FS_PART_ENTRY  PartTable2;    /**< papRtition table entry        */
    AMBA_FS_PART_ENTRY  PartTable3;    /**< papRtition table entry        */
    UINT16      Signature;  /**< system byte 0x55, 0xaa       */
} GNU_MIN_PADDING AMBA_FS_FAT_MBR;

/**
 * Boot Sector configuration (EX-FAT).
 */
typedef struct {
    UINT8   Jmp;        /**< 0:Jump command                       */
    UINT8   Adr;        /**< 1:IPL address                        */
    UINT8   Nop;        /**< 2:NOP(0x90)                          */
    UINT8   Maker[8];   /**< 3-A:name                             */
    UINT8   Mbz[53];    /**< B-3F:MustBeZero              */
    /* BPB */
    UINT64  Pto;        /**< 40-47:PapRtition Offset          */
    UINT64  Vol;        /**< 48-4F:Volume Length          */
    UINT32  Fto;        /**< 50-53:Fat Offset             */
    UINT32  Ftl;        /**< 54-57:Fat Length             */
    UINT32  Cho;        /**< 58-5B:Cluster Heap Offset        */
    UINT32  Cnt;        /**< 5C-5F:Cluster Count          */
    UINT32  Fcr;        /**< 60-63:FirstCluster Of Root Directory */
    UINT32  Vsn;        /**< 64-67:Volume Seria Number(UINT32)    */
    UINT16  Fsr;        /**< 68-69:File System Revision       */
    UINT16  Vfg;        /**< 6A-6B:Volume Flags           */
    UINT8   Bps;        /**< 6C:Bytes Per Sector Shift        */
    UINT8   Spc;        /**< 6D:Sector Per Cluster Shift      */
    UINT8   Fts;        /**< 6E:Number Of Fats            */
    UINT8   Dsl;        /**< 6F:Drive Select              */
    UINT8   Piu;        /**< 70:Percent In Use            */
    /* end */
    UINT8   Rsv1[7];    /**< 71-77:reserved(all 00h)          */
    UINT8   Bcode[390]; /**< 78-1FD:Boot Code                     */
    UINT16  Mrk;        /**< 1FE-1FF:extension format mark(55,AA) */
} GNU_MIN_PADDING AMBA_FS_EXFAT_BS;

/**
 * Generic Parameters Template (EX-FAT).
 */
typedef struct {
    UINT64  Guid;       /**< Parameter GuId                       */
    UINT8   Custom[32]; /**< Custom Defined                       */
} GNU_MIN_PADDING AMBA_FS_EXFAT_PARAM_TMP;

/**
 * Flash Parameters (EX-FAT).
 */
typedef struct {
    UINT8   Guid[16];   /**< Parameter GuId                       */
    UINT32  Ebs;        /**< Erase Block Size                     */
    UINT32  Pgs;        /**< Page Size                            */
    UINT32  Sps;        /**< Spare Sectors                        */
    UINT32  Rat;        /**< Random Access Time                   */
    UINT32  Pgt;        /**< Programming Time                     */
    UINT32  Rcy;        /**< Read Cycle                           */
    UINT32  Wcy;        /**< Write Cycle                          */
    UINT32  Rsv;        /**< Reserved                             */
} GNU_MIN_PADDING AMBA_FS_EXFAT_FLASH_PARAM;

/**
 * OEM Parameters (EX-FAT).
 */
typedef struct {
    AMBA_FS_EXFAT_PARAM_TMP para[9];
    UINT8                   Rsv[32];
} GNU_MIN_PADDING AMBA_FS_EXFAT_PARAM_OEM;

/**
 * Generic DirectoryEntry Template (EX-FAT).
 */
typedef struct {
    UINT8   Type;
    UINT8   Custom[19];
    UINT32  FirstCluster;
    UINT64  Length;
} GNU_MIN_PADDING AMBA_FS_EXFAT_ENTRY;

/**
 * Allocation Bitmap DirectoryEntry (EX-FAT).
 */
typedef struct {
#define BMP_ENTRY_TYPE  0x81
    UINT8   Type;
    UINT8   BmpFlag;
    UINT8   Rsv[18];
    UINT32  FirstCluster;
    UINT64  Length;
} GNU_MIN_PADDING AMBA_FS_EXFAT_BITMAP_ENTRY;

/**
 * Up-case Table DirectoryEntry (EX-FAT).
 */
typedef struct {
#define UPTBL_ENTRY_TYPE    (0x82U)
    UINT8   Type;
    UINT8   Rsv1[3];
    UINT32  ChkSum;
    UINT8   Rsv2[12];
    UINT32  FirstCluster;
    UINT64  Length;
} GNU_MIN_PADDING AMBA_FS_EXFAT_UPTBL_ENTRY;

/**
 * Boot Sector configuration (FAT32).
 */
typedef struct {
    UINT8   Jmp;        /**< 0:Jump command                       */
    UINT8   Adr;        /**< 1:IPL address                        */
    UINT8   Nop;        /**< 2:NOP(0x90)                          */
    UINT8   Maker[8];   /**< 3-A:name                             */
    /* BPB */
    UINT16  Bps;        /**< B-C:byte count of 1Sector            */
    UINT8   Spc;        /**< D:Sector count of 1cluster           */
    UINT16  Fno;        /**< E-F:stapRt recode number of FAT       */
    UINT8   Fts;        /**< 10:FAT number                        */
    UINT16  Ent;        /**< 11-12:entry count of root directory  */
    UINT16  All;        /**< 13-14:all count of desk              */
    UINT8   Med;        /**< 15:media descriptor                  */
    UINT16  Spf;        /**< 16-17:Sector count of 1FAT           */
    UINT16  Spt;        /**< 18-19:Sector count of 1track         */
    UINT16  Hed;        /**< 1A-1B:head count                     */
    UINT32  Hid;        /**< 1C-1F:hIdden Sector count            */
    UINT32  Lal;        /**< 20-23:all Sector count of volume desk*/
    /* for FAT32 */
    UINT32  Spf32;      /**< 24-27:recode count of 1FAT           */
    UINT16  Act32;      /**< 28-29:activeFAT                      */
    UINT16  Ver32;      /**< 2A-2B:file system version            */
    UINT32  Ent32;      /**< 2C-2F:head cluster of root directory */
    UINT16  Fsi32;      /**< 30-31:recode number of FS info       */
    UINT16  Bck32;      /**< 32-33:boot recode backup recode      */
    UINT8   Rsv32[12];  /*<* 34-3F:reserved                       */

    /* end */
    UINT8   Drvno;      /**< 40:physical drive number             */
    UINT8   Rsv0;       /**< 41:reserved                          */
    UINT8   Exb;        /**< 42:extension boot recode
                 *      discrimination code(29H) */
    UINT32  Vno;        /**< 43-46:volume serial number           */
    UINT8   Vnm[11];    /**< 47-51:volume label                   */
    UINT8   Typ[8];     /**< 52-59:file system type               */
    UINT8   Rsv1[420];  /**< 5A-1FD:reserved                      */
    UINT16  Mrk;        /**< 1FE-1FF:extension format mark(55,AA) */
} GNU_MIN_PADDING AMBA_FS_FAT_BS32;

/**
 * Boot Sector configuration (FAT16).
 */
typedef struct {
    UINT8   Jmp;        /**< 0:Jump command                       */
    UINT8   Adr;        /**< 1:IPL address                        */
    UINT8   Nop;        /**< 2:NOP(0x90)                          */
    UINT8   Maker[8];   /**< 3-A:Maker                            */
    /* BPB */
    UINT16  Bps;        /**< B-C:byte count of 1Sector            */
    UINT8   Spc;        /**< D:Sector count of 1cluster           */
    UINT16  Fno;        /**< E-F:stapRt record number of FAT       */
    UINT8   Fts;        /**< 10:FAT count                         */
    UINT16  Ent;        /**< 11-12:entry count of root directory  */
    UINT16  All;        /**< 13-14:all Sector number of disk      */
    UINT8   Med;        /**< 15:media discriptor                  */
    UINT16  Spf;        /**< 16-17:Sector count of 1FAT           */
    UINT16  Spt;        /**< 18-19:Sector count of 1track         */
    UINT16  Hed;        /**< 1A-1B:head count                     */
    UINT32  Hid;        /**< 1C-1F:hIdden Sector count            */
    UINT32  Lal;        /**< 20-23:all Sector count of volume desk*/
    /* end */
    UINT8   Drvno;      /**< 24:physical drive number             */
    UINT8   Rsv0;       /**< 25:reserved                          */
    UINT8   Exb;        /**< 26:extension boot record
                                discrimination code(29H)          */
    UINT32  Vno;        /**< 27-2A:volume serial number           */
    UINT8   Vnm[11];    /**< 2B-35:volume label                   */
    UINT8   Typ[8];     /**< 36-3D:file system type               */
    UINT8   Rsv1[448];  /**< 3E-1FD:reserved                      */
    UINT16  Mrk;        /**< 1FE-1FF:extension format mark(55,AA) */
} GNU_MIN_PADDING AMBA_FS_FAT_BS16;

/**
 * Used in creating data to be written to storage devices during formatting.
 */
typedef struct {
    UINT32 StartLba;        /**< Start of logical block address */
    UINT32 StartLbaCustom;  /* The customized StartLba value */
    UINT32 EndLba;
    UINT32 BsSizeCustom;    /** The customized boot sector size **/
    UINT32 TotalSector;     /**< Total number of sectors */
    UINT16 NumOfHead;       /**< Number of heads */
    UINT16 Spt;             /**< Sectors per track */
    UINT16 NumCyls;         /**< number of cylinders. */
    UINT16 Spc;             /**< Sectors per cluster */
    UINT32 Spf;             /**< Sectors per FAT */
    UINT16 RdeCustom;       /**< The customized Number of Root-directory Entries */
    UINT16 FatType;         /**< The valid values are: 12, 16, 32 */
    UINT8  FatId;           /**< FAT12/FAT16(S)/FAT32 */
    UINT16 Drive;           /**< Flash, xD, CF or SD Drive */
} AMBA_FS_DRIVE_INFO;

/**
 * File system information sector.
 */
typedef struct {
    UINT32 Title;
    UINT8  Rsv1[480];
    UINT32 Signature;
    UINT32 FsiEmpClst;      /**< Number of empty clster */
    UINT32 FsiNextClsNo;    /**< Last access cluster */
    UINT32 Rsv2[3];
    UINT16 Rsv3;
    UINT16 MagicNum;       /**< 0xaa55 */
} AMBA_FS_FSI_INFO;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaFS_PrFile2_SD.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_PrFile2SdMount(UINT32 SdChanNo);
UINT32 AmbaFS_PrFile2SdUnmount(UINT32 SdChanNo);
UINT32 AmbaFS_SdInit(PF_DRV_TBL *pDrive);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaFS_PrFile2_NAND.c
\*-----------------------------------------------------------------------------------------------*/

void AmbaFS_PrFile2Nand0Mount(void);
void AmbaFS_PrFile2Nand0Unmount(void);
void AmbaFS_PrFile2Nand1Mount(void);
void AmbaFS_PrFile2Nand1Unmount(void);
UINT32 AmbaFS_NandInit(PF_DRV_TBL * pDrive);

/**
 * Function proto-type for 'Write Sector Function'.
 */
typedef INT32 (*AMBA_FS_WR_SEC_FUNC)(UINT8* pBuf, UINT32 Sector, UINT32 Sectors);

/**
 * Function proto-type for 'Read Sector Function'.
 */
typedef INT32 (*AMBA_FS_RD_SEC_FUNC)(UINT8* pBuf, UINT32 Sector, UINT32 Sectors);

/**
 * Fill in pBuffer to be written to the master boot record (MBR) sector.
 *
 * @param pDinfo - Drive info
 * @param pBuf - Output pBuffer
 */
void AmbaFS_CreateMbr(AMBA_FS_DRIVE_INFO *pDrvInfo, UINT8 *pBuf);

/**
 * Fill in Buffer to be written to the boot sector.
 *
 * @param pDinfo - Drive info
 * @param pBuf - Output pBuffer
 */
void AmbaFS_CreateBs(AMBA_FS_DRIVE_INFO *pDrvInfo, UINT8 *pBuf);

/**
 * Fill in Buffer to be written to the file system information (FSI) sector.
 *
 * @param pDinfo - Drive info
 * @param bs32 - Boot sector data
 */
void AmbaFS_CreateFsi(AMBA_FS_DRIVE_INFO *pDinfo, AMBA_FS_FAT_BS32 *bs32, UINT8 *pBuf);

/**
 * create FAT table and write to the FAT area on device.
 *
 * @param pDinfo - Drive info
 * @param bs - Boot sector data
 * @param wsf - Write sector function
 * @returns - 1 if successful, -1 if fail
 */
INT32 AmbaFS_CreateFat(AMBA_FS_DRIVE_INFO *pDrvInfo, UINT8 *pBs, AMBA_FS_WR_SEC_FUNC WrSecFunc);

/**
 * Write the boot area on Drive.
 *
 * @param pDinfo - Drive info
 * @param wsf - Write sector function
 * @returns - 1 if successful, -1 if fail
 */
INT32 AmbaFS_CreateBootArea(AMBA_FS_DRIVE_INFO *pDrvInfo, AMBA_FS_WR_SEC_FUNC WrSecFunc);

/**
 * Write the boot area on Drive.
 *
 * @param bs - pointer to boot sector
 * @param wsf - Write sector function
 * @param rsf - Read sector function
 * @returns - 1 if successful, -1 if fail
 */
INT32 AmbaFS_CreateExfatEntry(AMBA_FS_EXFAT_BS *pBs,
                              AMBA_FS_WR_SEC_FUNC WrSecFunc, AMBA_FS_RD_SEC_FUNC RdSecFunc,
                              UINT8 *pExtraBuf, UINT32 ExtraSize);

/**
 * Utility function for computing the number of sectors per FAT.
 *
 * @param FatType - The valid values are: 12, 16, 32
 * @param tsector - Total number of sectors
 * @param nbs - Number of boot sectors
 * @param nrs - Number of reserved sectors
 * @param spc - Sectors per track
 * @returns - Number of sectors per FAT
 */
UINT32 AmbaFS_GetSectorsPerFat(UINT8 FatType, UINT32 TotalSector, UINT8 Nbs, UINT8 Nrs, UINT8 Spc);

/**
 * Utility function for computing the number of sectors per FAT (used
 * exclusively by NAND flash devices).
 *
 * @param FatType - The valid values are: 12, 16, 32
 * @param tclust - Total number of clusters
 * @param spc - Sectors per track
 * @returns - Number of sectors per FAT
 */
UINT32 AmbaFS_GetFlashSectorsPerFat(UINT16 FatType, UINT32 TotalClust, UINT16 Spc);

INT32 AmbaFS_ParseFormatParam(UINT32 DevSize, AMBA_FS_DRIVE_INFO *pDrvInfo);

void AmbaFS_ParseSdaFormatParam(UINT32 SecCnt, AMBA_FS_DRIVE_INFO *pDrvInfo);

/**
 * FAT type determination.
 *
 * @param pbs - Boot sector data
 * @returns - FAT type
 */
UINT32 AmbaFS_FatTypeDetermination(AMBA_FS_FAT_BS32 * pBs);

typedef UINT32 (*AMBA_FS_EXT_DRIVE_f)(void *pDrive);

/**
 * Utility function for register external drives like USB and PCIE.
 *
 * @param ExtDrvFunc - external drive function
 * @retval 0 Success
 * @retval 1 Fail due to maximum number of registered drivers reached
 */
UINT32 AmbaFS_RegisterExtDrive(AMBA_FS_EXT_DRIVE_f ExtDrvFunc);


#endif  /* AMBA_FS_FORMAT_H */
