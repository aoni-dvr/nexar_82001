/**
 *  @file AmbaRTSL_NAND_Ctrl.h
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
 *  @details Definitions & Constants for NAND Controller RTSL APIs
 *
 */

#ifndef AMBA_RTSL_NAND_CTRL_H
#define AMBA_RTSL_NAND_CTRL_H

#ifndef AMBA_NAND_DEF_H
#include "AmbaNAND_Def.h"
#endif

#ifndef AMBA_NAND_BBM_DEF_H
#include "AmbaNAND_BBM_Def.h"
#endif

/*
 * Macro Definitions
 */
#define AmbaRTSL_NandSendResetCmd              AmbaCSL_NandSendResetCmd
#define AmbaRTSL_NandSendReadStatusCmd         AmbaCSL_NandSendReadStatusCmd
#define AmbaRTSL_NandSendReadIdCmd             AmbaCSL_NandSendReadIdCmd
#define AmbaRTSL_NandGetReadIdResponse         AmbaCSL_NandGetReadIdResponse
#define AmbaRTSL_NandGetCmdResponse            AmbaCSL_NandGetCmdResponse

#define AmbaRTSL_SpiNandWriteEnableCmd         AmbaCSL_SpiNandSendWriteEnable
#define AmbaRTSL_SpiNandSendWriteDisableCmd    AmbaCSL_SpiNandSendWriteDisable
#define AmbaRTSL_SpiNandSendResetCmd           AmbaCSL_SpiNandSendResetCmd

#define AmbaRTSL_NandDevInfo                   (AmbaRTSL_NandCtrl.DevInfo.pNandDevInfo)
#define AmbaRTSL_NandCommonInfo                (&(AmbaRTSL_NandCtrl.NandInfo))
#define AmbaRTSL_NandSignaturePrimaryBBT       (pAmbaNandTblSysPart->NAND_SignaturePrimaryBBT)
#define AmbaRTSL_NandSignatureMirrorBBT        (pAmbaNandTblSysPart->NAND_SignatureMirrorBBT)
#define AmbaRTSL_NandBlkAddrPrimaryBBT         (pAmbaNandTblUserPart->NAND_BlkAddrPrimaryBBT)
#define AmbaRTSL_NandBlkAddrMirrorBBT          (pAmbaNandTblUserPart->NAND_BlkAddrMirrorBBT)

#define AmbaRTSL_SpiNandDevInfo                (AmbaRTSL_NandCtrl.DevInfo.pSpiNandDevInfo)
/*
 * RTSL NAND Management Structure
 */

#define AMBA_NAND_STATUS_LAST_CMD_FAIL       (0x01U)
#define AMBA_NAND_STATUS_PRIOR_LAST_CMD_FAIL (0x02U)
#define AMBA_NAND_STATUS_RESERVED            (0x04U)
#define AMBA_NAND_STATUS_NE_NAND_RESERVED    (0x08U)
#define AMBA_NAND_STATUS_VENDER              (0x10U)
#define AMBA_NAND_STATUS_NO_ARRAY_OP         (0x20U)
#define AMBA_NAND_STATUS_READY               (0x40U)
#define AMBA_NAND_STATUS_NO_WP               (0x80U)

typedef union {
    UINT32  Data;
    struct {
        UINT32   LastCmdFailed:      1;  /* [0]: 1 - the last command failed */

        UINT32   PrioLastCmdFailed:  1;  /* [1]: 1 - the command issued prior to the last command failed */
        UINT32   Reserved:           1;  /* [2]: Reserved */
        UINT32   CSP:                1;  /* [3]: Reserved for non-EZ NAND read operations */
        UINT32   VendorSpecific:     1;  /* [4]: Vendor Specific */
        UINT32   ArrayOpReady:       1;  /* [5]: 1 - there is no array operation in progress */

        UINT32   Ready:              1;  /* [6]: 1 - ready for accept another command */
        UINT32   NotWriteProtect:    1;  /* [7]: 1 - the device is not write protected */
    } Bits[4];
} AMBA_NAND_STATUS_u;

typedef struct {

    union {
        AMBA_NAND_DEV_INFO_s    *pNandDevInfo;      /* Pointer to external NAND device information */
        AMBA_SPINAND_DEV_INFO_s *pSpiNandDevInfo;
    } DevInfo;

    AMBA_NAND_COMMON_INFO_s NandInfo;

    AMBA_PARTITION_CONFIG_s *pSysPartConfig;    /* pointer to System partition configurations */
    AMBA_PARTITION_CONFIG_s *pUserPartConfig;   /* pointer to User Partition Configurations */

    AMBA_NAND_STATUS_u  Status[4];              /* NAND command Response */

    UINT32  BlkByteSize;                        /* Block size in Byte */
    UINT32  TotalNumBlk;                        /* Total number of blocks */
    UINT32  BstPageCount;                       /* Number of Pages for BST */
    UINT32  SysPtblPageCount;                   /* Number of Pages for System Partition Table */
    UINT32  UserPtblPageCount;                  /* Number of Pages for User Partition Table */
    UINT32  VendorDataPageCount;                /* Number of Pages for Vendor Specific Data */
    UINT32  NumEccBit;

    UINT32  BbtPageCount;                       /* Number of Pages for BBT */
    UINT8   BadBlkMarkOffset;                   /* the offset of Bad Block Mark */
    UINT8   BbtSignatureOffset;                 /* the offset of BBT Signature */
    UINT8   BbtVersionOffset;                   /* the offset of BBT Version */
    UINT8   BbtVersion;                         /* BBT Version */

    UINT32  NandCtrlRegVal;
} AMBA_RTSL_NAND_CTRL_s;

extern AMBA_SYS_PARTITION_TABLE_s  AmbaRTSL_SysPartTable GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
extern AMBA_USER_PARTITION_TABLE_s AmbaRTSL_UserPartTable GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
extern UINT32 AmbaRTSL_NandBadBlkTable[AMBA_NAND_MAX_BBT_BYTE_SIZE]                     GNU_ALIGNED_CACHESAFE;

/*
 * Defined in AmbaRTSL_NAND.c
 */
extern AMBA_RTSL_NAND_CTRL_s AmbaRTSL_NandCtrl;

void AmbaRTSL_NandReset(void);
//UINT32 AmbaRTSL_NandReadStatus(AMBA_NAND_STATUS_u *pStatus);
UINT32 AmbaRTSL_NandCopyback(UINT32 SrcPageAddr, UINT32 DestPageAddr);
UINT32 AmbaRTSL_NandEraseBlock(UINT32 BlkAddr);

UINT32 AmbaRTSL_NandReadStart(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf);
UINT32 AmbaRTSL_NandRead(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf);
void AmbaRTSL_NandProgramStart(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf);
UINT32 AmbaRTSL_NandProgram(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf);

UINT32 AmbaRTSL_NandCheckDeviceStatus(UINT32 NumPage);

/*
 * Defined in AmbaRTSL_SPINAND.c
 */
UINT32 AmbaRTSL_SpiNandRead(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf);
UINT32 AmbaRTSL_SpiNandReadStart(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf);
UINT32 AmbaRTSL_SpiNandProgram(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf);
UINT32 AmbaRTSL_SpiNandProgramStart(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf);
UINT32 AmbaRTSL_SpiNandEraseBlock(UINT32 BlkAddr);
void AmbaRTSL_SpiNandEraseBlockStart(UINT32 BlkAddr);

void AmbaRTSL_SpiNandSetFeature(UINT8 FeatureAddr, UINT8 Value);
void AmbaRTSL_SpiNandGetFeature(UINT8 FeatureAddr, UINT8 *pStatus);
UINT32 AmbaRTSL_SpiNandReadID(UINT8 *pDeviceID);
UINT32 AmbaRTSL_SpiNandReset(void);
UINT32 AmbaRTSL_SpiNandCheckDeviceStatus(UINT32 NumPage, UINT8 ErrorPattern);

/*
 * Defined in AmbaRTSL_NAND_Partition.c
 */
extern AMBA_SYS_PARTITION_TABLE_s  *pAmbaNandTblSysPart;
extern AMBA_USER_PARTITION_TABLE_s *pAmbaNandTblUserPart;

#endif /* AMBA_RTSL_NAND_CTRL_H */
