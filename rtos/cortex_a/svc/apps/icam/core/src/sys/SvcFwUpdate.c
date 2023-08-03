/**
 *  @file SvcFwUpdate.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you 
 * distribute this sample code (whether in source, object, or binary code form), it will be 
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details SD Controller APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaNAND_Def.h"
#include "AmbaFS.h"
#include "AmbaShell.h"

#include "SvcNvm.h"
#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcLog.h"
#include "SvcCmd.h"
#include "AmbaSvcWrap.h"
#include "SvcFwUpdate.h"
#if defined(CONFIG_SVC_ENABLE_WDT)
#include "SvcWdt.h"
#endif

typedef struct {
    UINT32  Crc32;                  /* CRC32 Checksum */
    UINT32  Version;                /* Version number */
    UINT32  Date;                   /* Date */
    UINT32  Length;                 /* Image length */
    UINT32  MemAddr;                /* Location to be loaded into memory */
    UINT32  Flag;                   /* Flag of partition    */
    UINT32  Magic;                  /* The magic number     */
    UINT32  Reserved[57];
} AMBA_FIRMWARE_IMAGE_HEADER_s;

typedef struct {
    char    ModelName[32];   /* model name */
    UINT32  PtbMagic;
    UINT32  CRC32;          /* CRC32 of entire Binary File: AmbaCamera.bin */

    struct {
        UINT32   Size;
        UINT32   Crc32;
    }  AmbaFwInfo[AMBA_NUM_USER_PARTITION];

    AMBA_PARTITION_CONFIG_s  SysPartitionInfo[AMBA_NUM_SYS_PARTITION];
    AMBA_PARTITION_CONFIG_s  UserPartitionInfo[AMBA_NUM_USER_PARTITION];
    AMBA_PLOAD_PARTITION_s   PloadInfo;
}  AMBA_FIRMWARE_HEADER_s;

#define UPDATE_ERR_BASE                0x900U
#define UPDATE_ERR_FILEIO_FAIL         (UPDATE_ERR_BASE | 0x2U)
#define UPDATE_ERR_PTB_ERROR           (UPDATE_ERR_BASE | 0x7U)

#define SVC_FW_UPDATE_FLG_INIT      (0x1U)
#define SVC_FW_UPDATE_FLG_DBG_MSG   (0x1000U)
#define SVC_FW_UPDATE_MAX_FILE_SIZE (60UL << 20U)

typedef UINT32 (*SVC_FW_UPDATE_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector, void *pData);
typedef void   (*SVC_FW_UPDATE_SHELL_USAGE_f)(void);
typedef struct {
    char                        CmdName[32];
    SVC_FW_UPDATE_SHELL_FUNC_f  pFunc;
    SVC_FW_UPDATE_SHELL_USAGE_f pUsage;
} SVC_FW_UPDATE_SHELL_FUNC_s;

typedef struct {
    ULONG  MemAddr;
    UINT32 MemSize;
} SVC_FW_UPDATE_CTRL_s;

static SVC_FW_UPDATE_CTRL_s SVC_FwUpdateCtrl GNU_SECTION_NOZEROINIT;
#define SVC_LOG_FWU "FWU"
#define LOG_FWU_OK( a, b, c) SvcLog_OK( SVC_LOG_FWU, a, b, c)
#define LOG_FWU_NG( a, b, c) SvcLog_NG( SVC_LOG_FWU, a, b, c)
#define LOG_FWU_API(a, b, c) SvcLog_DBG(SVC_LOG_FWU, a, b, c)
static void LOG_FWU_DBG(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
#ifdef SVC_FW_UPDATE_DBG_MSG_ON
    SvcLog_DBG(SVC_LOG_FWU, pStr, Arg1, Arg2);
#else
    AmbaMisra_TouchUnused(&pStr);
    AmbaMisra_TouchUnused(&Arg1);
    AmbaMisra_TouchUnused(&Arg2);
#endif
}

static AMBA_SYS_PARTITION_TABLE_s  SysPTB  = {0U};
static AMBA_USER_PARTITION_TABLE_s UserPTB = {0U};

#define PRN_FWU_NG  SVC_WRAP_PRINT
#define PRN_FWU_API SVC_WRAP_PRINT

static UINT32 Update_WriteImage(AMBA_FS_FILE *pFile, UINT8 *pImgBuf, UINT32 UserPtbNo);
/*------------------------------------------------------------------------------------------*\
 *   for NAND, SPI-NOR, SPI-NAND, Linux DTB is written in the "last valid block" in Linux
 *   kernel partition, which is related to bad block issue in NAND case,
 *
 *   for emmc, Linux DTB is written in the "last 128 sector" in Linux kernel partition,
 *
 *   for further info, please check "AmbaLink_Dtb.c"
\*------------------------------------------------------------------------------------------*/
#if defined(CONFIG_ENABLE_AMBALINK) && !defined(CONFIG_QNX)
#if defined(CONFIG_ENABLE_EMMC_BOOT) || defined(CONFIG_MUTI_BOOT_DEVICE)
#define LINUX_DTB_BLK_NUM  (128U)
#define LINUX_DTB_BLK_SIZE (512U)
#elif defined(CONFIG_ENABLE_NAND_BOOT) || defined(CONFIG_ENABLE_SPINAND_BOOT) || defined(CONFIG_ENABLE_SPINOR_BOOT)
#define LINUX_DTB_BLK_NUM  (1U)
#define LINUX_DTB_BLK_SIZE (0U) /* should only write 1 block */
#else
#pragma message ("[SvcFwUpdate.c] ERROR !! UNKNOWN BOOT DEVICE !!")
#endif
/*------------------------------------------------------------------------------------------*\
 * FwUpdater_WriteDtb : write Linux DTB in the last valid block of Linux kernel partition
\*------------------------------------------------------------------------------------------*/
static UINT32 FwUpdater_WriteDtb(UINT32 Size, UINT8 * pDtbBuf)
{
    AMBA_PARTITION_ENTRY_s PartEntry;
    UINT32 RetVal;
    AmbaSvcWrap_MisraMemset(&PartEntry, 0, sizeof(AMBA_PARTITION_ENTRY_s));

    AmbaPrint_PrintStr5("Program \"LINUX DTB\" to NVM", NULL, NULL, NULL, NULL, NULL);

    /* get Linux partition info */
    RetVal = SvcNvm_ReadPartitionInfo(1U, AMBA_USER_PARTITION_LINUX_KERNEL, &PartEntry);
    if (RetVal != OK) {
        LOG_FWU_NG("SvcNvm_ReadPartitionInfo Failed, return 0x%x", RetVal, 0U);

    } else {
        /* Backup Linux DTB data */
        if (PartEntry.BlkCount == 0U) {
            LOG_FWU_API("Linux BlkCount = 0", 0U, 0U);
            RetVal = OK;

        } else {
            UINT8  i;
            UINT32 DtbBlkAddr = PartEntry.StartBlkAddr + PartEntry.BlkCount - LINUX_DTB_BLK_NUM;
            ULONG  U32_pDtbBuf;
            UINT32 NvmID;

            /* copy DTB buffer address */
            RetVal = AmbaSvcWrap_GetNVMID(&NvmID, 1U, AMBA_USER_PARTITION_LINUX_KERNEL);
            AmbaMisra_TypeCast(&U32_pDtbBuf, &pDtbBuf);

            if (SVC_OK == RetVal) {
                for (; DtbBlkAddr > PartEntry.StartBlkAddr; DtbBlkAddr --) {
                    UINT32 add = DtbBlkAddr;
                    for (i = 0U; i < LINUX_DTB_BLK_NUM; i++) {

                        // AmbaPrint_PrintUInt5("AmbaNVM_WritePhyBlock: [%u] U32_pDtbBuf 0x%X, DtbBlkAddr %u, LINUX_DTB_BLK_SIZE %u", i, U32_pDtbBuf, DtbBlkAddr, LINUX_DTB_BLK_SIZE, 0U);
                        RetVal |= AmbaNVM_WritePhyBlock(NvmID, add, pDtbBuf, 1000U);

                        if (OK != RetVal) {
                            LOG_FWU_NG("AmbaNVM_WritePhyBlock write No.%u block Failed, return 0x%x", i, RetVal);
                            break;
                        } else {
                            U32_pDtbBuf += LINUX_DTB_BLK_SIZE;
                            add++;
                            AmbaMisra_TypeCast(&pDtbBuf, &U32_pDtbBuf);
                        }

                    }
                    if (OK == RetVal) {
                        AmbaPrint_PrintUInt5("Size: 0x%X, BlkNum %u, StartAddr 0x%X", Size, LINUX_DTB_BLK_NUM, DtbBlkAddr, 0U, 0U);
                        AmbaPrint_PrintStr5( "Done\n", NULL, NULL, NULL, NULL, NULL);
                        break;
                    }
                }
            }
        }
    }

    return RetVal;
}
#endif

void SvcFwUpdate_ShowPTB(void)
{
    UINT32 RetVal;
    UINT32 ActivePtb, i;
    const AMBA_PARTITION_ENTRY_s* pEntry;

    RetVal = SvcNvm_ReadSysPartitionTable(&SysPTB, 5000U);
    if (RetVal != OK) {
        LOG_FWU_NG("SvcNvm_ReadSysPartitionTable failed, return 0x%x", RetVal, 0U);
    } else {
        AmbaPrint_PrintUInt5("", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SYS  PTB    Version                0x%X", SysPTB.Version,                  0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("            PtbMagic               0x%X", SysPTB.PtbMagic,                 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("            BlkByteSize            %u",   SysPTB.BlkByteSize,              0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("            BlkAddrUserPTB         %u",   SysPTB.BlkAddrUserPTB,           0U, 0U, 0U, 0U);
     // AmbaPrint_PrintStr5( "            ModelName              %s",   (const char *)SysPTB.ModelName,  0U, 0U, 0U, 0U);
     // AmbaPrint_PrintStr5( "            SerialNo               %s",   (const char *)SysPTB.SerialNo ,  0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("            SignaturePrimaryBBT    0x%X", SysPTB.NAND_SignaturePrimaryBBT, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("            SignatureMirrorBBT     0x%X", SysPTB.NAND_SignatureMirrorBBT,  0U, 0U, 0U, 0U);
        for (i = 0; i < AMBA_NUM_SYS_PARTITION; i++) {
            pEntry = &SysPTB.Entry[i];
            AmbaPrint_PrintUInt5("", 0U, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("       [%2u] -- BlkCount            %u",   i, pEntry->BlkCount,      0U, 0U, 0U);
            if (0U != pEntry->BlkCount) {
                AmbaPrint_PrintStr5( "            -- PartitionName       %s", (const char *)pEntry->PartitionName, NULL, NULL, NULL, NULL);
                AmbaPrint_PrintUInt5("            -- Attribute           0x%X", pEntry->Attribute,     0U, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("            -- ByteCount           %u",   pEntry->ByteCount,     0U, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("            -- StartBlkAddr        %u",   pEntry->StartBlkAddr,  0U, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("            -- ActualByteSize      %u",   pEntry->ActualByteSize,0U, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("            -- RamLoadAddr         %u",   pEntry->RamLoadAddr,   0U, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("            -- ProgramStatus       0x%X", pEntry->ProgramStatus, 0U, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("            -- ImageCRC32          0x%X", pEntry->ImageCRC32,    0U, 0U, 0U, 0U);
            }
        }
    }
    AmbaPrint_PrintUInt5("-------------------------------------", 0U, 0U, 0U, 0U, 0U);

    for (ActivePtb = 0U; ActivePtb < 2U; ActivePtb++) {
        RetVal |= SvcNvm_ReadUserPartitionTable(&UserPTB, ActivePtb, 5000U);
        if (RetVal != OK) {
            LOG_FWU_NG("SvcNvm_ReadUserPartitionTable failed, return 0x%x", RetVal, 0U);
        } else {
            AmbaPrint_PrintUInt5("", 0U, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("User PTB[%u] Version                0x%X",  ActivePtb,     UserPTB.Version, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("            PTBActive              %u",     UserPTB.PTBActive,          0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("            PTBNumber              %u",     UserPTB.PTBNumber,          0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("            BldMagicCode           %u",     UserPTB.BldMagicCode,       0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("            BlkAddrPrimaryBBT      0x%X",   UserPTB.NAND_BlkAddrPrimaryBBT, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("            BlkAddrMirrorBBT       0x%X",   UserPTB.NAND_BlkAddrMirrorBBT , 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("            CRC32                  0x%X",   UserPTB.CRC32,              0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("            ExtraCRC32             0x%X",   UserPTB.ExtraCRC32,         0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("            PtbMagic               %u",     UserPTB.PtbMagic,           0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("            BlkAddrCurent/NextPTB  %u, %u", UserPTB.BlkAddrCurentPTB, UserPTB.BlkAddrNextPTB, 0U, 0U, 0U);

            for (i = 0; i < AMBA_NUM_USER_PARTITION; i++) {
                pEntry = &UserPTB.Entry[i];
                AmbaPrint_PrintUInt5("", 0U, 0U, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("       [%2u] -- BlkCount            %u",   i, pEntry->BlkCount,      0U, 0U, 0U);
                if (0U != pEntry->BlkCount) {
                    AmbaPrint_PrintStr5( "            -- PartitionName       %s", (const char *)pEntry->PartitionName, NULL, NULL, NULL, NULL);
                    AmbaPrint_PrintUInt5("            -- Attribute           0x%X", pEntry->Attribute,     0U, 0U, 0U, 0U);
                    AmbaPrint_PrintUInt5("            -- ByteCount           %u",   pEntry->ByteCount,     0U, 0U, 0U, 0U);
                    AmbaPrint_PrintUInt5("            -- StartBlkAddr        %u",   pEntry->StartBlkAddr,  0U, 0U, 0U, 0U);
                    AmbaPrint_PrintUInt5("            -- ActualByteSize      %u",   pEntry->ActualByteSize,0U, 0U, 0U, 0U);
                    AmbaPrint_PrintUInt5("            -- RamLoadAddr         %u",   pEntry->RamLoadAddr,   0U, 0U, 0U, 0U);
                    AmbaPrint_PrintUInt5("            -- ProgramStatus       0x%X", pEntry->ProgramStatus, 0U, 0U, 0U, 0U);
                    AmbaPrint_PrintUInt5("            -- ImageCRC32          0x%X", pEntry->ImageCRC32,    0U, 0U, 0U, 0U);
                }
            }
            AmbaPrint_PrintUInt5("-------------------------------------", 0U, 0U, 0U, 0U, 0U);
        }
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static UINT32 SvcFwUpdate_ShellShowPTB(UINT32 ArgCount, char * const *pArgVector, void *pData)
{
    if (pArgVector != NULL) {
        AmbaMisra_TouchUnused(pData);
        AmbaMisra_TouchUnused(&ArgCount);
    }
    SvcFwUpdate_ShowPTB();
    return OK;
}

static void SvcFwUpdate_ShellShowPTBU(void)
{
    PRN_FWU_API "  %sshow_ptb%s              : show user ptb info"
        SVC_PRN_ARG_S SVC_LOG_FWU
        SVC_PRN_ARG_CSTR ("\033""[38;2;255;255;128m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
        SVC_PRN_ARG_CSTR ("\033""[0m")                                    SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    PRN_FWU_API " " SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
}

static UINT32 Update_WriteImage(AMBA_FS_FILE * pFile, UINT8 * pImgBuf, UINT32 UserPtbNo)
{
    UINT8  *pWrBuf = pImgBuf;
    UINT32 i, RetVal = SVC_OK;
    AMBA_FIRMWARE_IMAGE_HEADER_s FwImgHeader;
    AMBA_PARTITION_ENTRY_s *pPartEntry;
    AMBA_FIRMWARE_HEADER_s FwHeader;
    UINT32 FwHeaderSize = (UINT32)sizeof(AMBA_FIRMWARE_HEADER_s);
    UINT32 FwImgOffset  = (UINT32)sizeof(AMBA_FIRMWARE_HEADER_s);
    UINT32 NumSucce = 0U;
    UINT32 BlkByteSize = 0U;

    AmbaSvcWrap_MisraMemset(&FwHeader, 0, sizeof(AMBA_FIRMWARE_HEADER_s));

    RetVal |= SvcNvm_ReadSysPartitionTable (&SysPTB,             5000U);
    RetVal |= SvcNvm_ReadUserPartitionTable(&UserPTB, UserPtbNo, 5000U);
    RetVal |= SvcNvm_ReadUserPartitionTable(NULL,     UserPtbNo, 5000U);
    if (SVC_OK == RetVal) {
        BlkByteSize = SysPTB.BlkByteSize;
    } else {
        LOG_FWU_NG("SvcNvm_Read Sys/User PartitionTable failed", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        for(i = 0; i < AMBA_NUM_USER_PARTITION; i++) {
            FwHeader.AmbaFwInfo[i].Size = 0;
            FwHeader.AmbaFwInfo[i].Crc32 = 0;
        }

        if (pFile != NULL) {
            RetVal = AmbaFS_FileSeek(pFile, (INT64) 0, AMBA_FS_SEEK_START);
            if (RetVal != SVC_OK) {
                AmbaPrint_PrintStr5("Cannot read fw image header!", NULL, NULL, NULL, NULL, NULL);
                RetVal = UPDATE_ERR_FILEIO_FAIL;
            }

            if (RetVal == SVC_OK) {
                RetVal = AmbaFS_FileRead(&FwHeader, 1, FwHeaderSize, pFile, &NumSucce);
                if ((FwHeaderSize != NumSucce) || (RetVal != SVC_OK)) {
                    AmbaPrint_PrintStr5("can't read firmware file!", NULL, NULL, NULL, NULL, NULL);
                    RetVal |= UPDATE_ERR_FILEIO_FAIL;
                }
            }
        } else {
            /* update from memory */
            AmbaSvcWrap_MisraMemcpy(&FwHeader, pImgBuf, FwHeaderSize);
        }
    }

    if (RetVal == SVC_OK) {
        /* Program to NAND and update PTB. */
        for (i = 0; i < AMBA_NUM_USER_PARTITION; i++) {
            #if defined(CONFIG_ENABLE_AMBALINK) && !defined(CONFIG_QNX)
            static UINT8 HasLinux = 0U;
            #endif
            if (RetVal != SVC_OK) {
                break;
            }
            if (FwHeader.AmbaFwInfo[i].Size == 0U) {
                continue;
            }

            pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (UserPTB.Entry[i]);

            /* Read partition header. */
            if (pFile != NULL) {
                if (AmbaFS_FileSeek(pFile, (INT64) FwImgOffset, AMBA_FS_SEEK_START) != SVC_OK) {
                    AmbaPrint_PrintStr5("Cannot read fw image header!", NULL, NULL, NULL, NULL, NULL);
                    RetVal = UPDATE_ERR_FILEIO_FAIL;
                    continue;
                }

                RetVal = AmbaFS_FileRead(&FwImgHeader, 1, (UINT32)sizeof(FwImgHeader), pFile, &NumSucce);
                if ((NumSucce != (UINT32)sizeof(FwImgHeader)) || (RetVal != SVC_OK)) {
                    AmbaPrint_PrintStr5("Cannot read fw image header!", NULL, NULL, NULL, NULL, NULL);
                    RetVal = UPDATE_ERR_FILEIO_FAIL;
                    continue;
                }

                if (FwImgHeader.Length > SVC_FwUpdateCtrl.MemSize) {
                    AmbaPrint_PrintStr5( "warning: Programming \"%s\" to NVM", (const char *) pPartEntry->PartitionName, NULL, NULL, NULL, NULL);
                    AmbaPrint_PrintUInt5("warning: partition [%u] Fw size %u > MemSize %u", i, FwImgHeader.Length, SVC_FwUpdateCtrl.MemSize, 0U, 0U);
                    RetVal = UPDATE_ERR_FILEIO_FAIL;
                    continue;
                }

                #ifdef CONFIG_LINUX
                {   /* reset buffer to align BlkByteSize */
                    UINT32 FwBlockCnt = FwImgHeader.Length / BlkByteSize;
                    if ((FwImgHeader.Length % BlkByteSize) > 0U) {
                        FwBlockCnt += 1U;
                    }
                    AmbaSvcWrap_MisraMemset(pImgBuf, 0, FwBlockCnt * BlkByteSize);
                }
                #endif
                RetVal = AmbaFS_FileRead(pImgBuf, 1, FwImgHeader.Length, pFile, &NumSucce);
                if ((NumSucce != FwImgHeader.Length) || (RetVal != SVC_OK)) {
                    AmbaPrint_PrintStr5("firmware image read fail", NULL, NULL, NULL, NULL, NULL);
                    RetVal = UPDATE_ERR_FILEIO_FAIL;
                    continue;
                }
                pWrBuf = pImgBuf;
            } else {
                /* update from memory */
                pWrBuf = &(pImgBuf[FwImgOffset]);
                AmbaSvcWrap_MisraMemcpy(&FwImgHeader, pWrBuf, sizeof(FwImgHeader));
                pWrBuf = &(pWrBuf[sizeof(FwImgHeader)]);
            }

            AmbaPrint_PrintStr5("Program \"%s\" to NVM", (const char *) pPartEntry->PartitionName, NULL, NULL, NULL, NULL);
            // AmbaPrint_PrintUInt5("[sys] Size: %u, MemAddr 0x%X", pPartEntry->ActualByteSize, pPartEntry->RamLoadAddr, 0U, 0U, 0U);
            // AmbaPrint_PrintUInt5("[new] Size: %u, MemAddr 0x%X", FwImgHeader.Length, FwImgHeader.MemAddr, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("[%u] Fw size: 0x%X, Partition size 0x%X (%u block)", i, FwImgHeader.Length, (pPartEntry->BlkCount *BlkByteSize), pPartEntry->BlkCount, 0U);

            if ((pPartEntry->BlkCount *BlkByteSize) >= FwImgHeader.Length) {
                UINT32 retry = 5U;

                #ifdef CONFIG_LINUX
                /* erase entire linux RFS partition, or data will not pass verification when linux booting. */
                if (AMBA_USER_PARTITION_LINUX_ROOT_FS == i) {
                    RetVal = SvcNvm_ErasePartition(1U, i, 5000U);
                    if (RetVal != SVC_OK) {
                        AmbaPrint_PrintUInt5("Erase linux rfs partition failed. ErrCode(0x%08X)", RetVal, 0U, 0U, 0U, 0U);
                    } else {
                        AmbaPrint_PrintUInt5("Erase linux rfs partition done", RetVal, 0U, 0U, 0U, 0U);
                    }
                    AmbaPrint_PrintUInt5("Write image start", 0U, 0U, 0U, 0U, 0U);
                }
                #endif

                do {
                    RetVal = SvcNvm_WritePartition(1U, i, FwImgHeader.Length, pWrBuf, 5000U);
                    if (RetVal != SVC_OK) {
                        LOG_FWU_NG("Write Image Failed : 0x%X, retry...... %u", RetVal, (6U - retry));
                    }
                    retry--;
                } while ((RetVal != SVC_OK) && (retry > 0U));
            } else {
                LOG_FWU_NG("ERROR: partition size %u < FwImgHeader.Length %u Byte\n", (pPartEntry->BlkCount *BlkByteSize), FwImgHeader.Length);
                /* Get offset of the next image. */
                FwImgOffset += FwHeader.AmbaFwInfo[i].Size;
                continue;
            }

            /* Update the PTB's entry */
            pPartEntry->ActualByteSize = FwImgHeader.Length;
            pPartEntry->RamLoadAddr    = FwImgHeader.MemAddr;
            pPartEntry->ImageCRC32     = FwImgHeader.Crc32;

            RetVal = SvcNvm_WriteUserPartitionTable(&UserPTB, UserPtbNo, 5000U);
            if (RetVal != SVC_OK) {
                AmbaPrint_PrintUInt5("SvcNvm_WriteUserPartitionTable return 0x%x", RetVal, 0, 0, 0, 0U);
                AmbaPrint_PrintStr5("Unable to update ptb %s", (const char *) pPartEntry->PartitionName, NULL, NULL, NULL, NULL);
                RetVal = UPDATE_ERR_PTB_ERROR;
            }

            if ((pPartEntry->Attribute & AMBA_PARTITION_ATTR_BACKUP) == 0x0U) {
                /* sync info to another user PTB */
                AMBA_USER_PARTITION_TABLE_s TmpUserPTB;
                UINT32 anotherPTB = 0U;

                AmbaSvcWrap_MisraMemset(&TmpUserPTB, 0, sizeof(AMBA_USER_PARTITION_TABLE_s));
                if (UserPTB.PTBNumber == 0U) {
                    anotherPTB = 1U;
                } else {
                    anotherPTB = 0U;
                }
                
                /* if the current user PTB is 0, get a copy of user PTB 1 */
                RetVal = SvcNvm_ReadUserPartitionTable(&TmpUserPTB, anotherPTB, 5000U);
                if (RetVal != SVC_OK) {
                    LOG_FWU_NG("SvcNvm_ReadUserPartitionTable failed before write, PTBNumber %u return 0x%x", UserPTB.PTBNumber, RetVal);
                } else {
                    AMBA_PARTITION_ENTRY_s* pTmpPartEntry = (AMBA_PARTITION_ENTRY_s *) &(TmpUserPTB.Entry[i]);
                    pTmpPartEntry->ActualByteSize = FwImgHeader.Length;
                    pTmpPartEntry->RamLoadAddr    = FwImgHeader.MemAddr;
                    pTmpPartEntry->ImageCRC32     = FwImgHeader.Crc32;
                    RetVal = SvcNvm_WriteUserPartitionTable(&TmpUserPTB, anotherPTB, 5000U);
                }

                if (RetVal != SVC_OK) {
                    LOG_FWU_NG("SvcNvm_WriteUserPartitionTable PTBNumber %u failed, return 0x%x", anotherPTB, RetVal);
                }
                if (RetVal != SVC_OK) {
                    LOG_FWU_NG("SvcNvm_ReadUserPartitionTable failed after write, PTBNumber %u return 0x%x", UserPTB.PTBNumber, RetVal);
                }
            }

            if (RetVal == SVC_OK) {
                AmbaPrint_PrintStr5("Done\n", NULL, NULL, NULL, NULL, NULL);
                #if defined(CONFIG_ENABLE_AMBALINK) && !defined(CONFIG_QNX)
                /* check if Linux kernel exist */
                if (AMBA_USER_PARTITION_LINUX_KERNEL == i) {
                    HasLinux = 1U;
                }

                /* update Linux DTB to the last sector of LINUX_KERNEL partition */
                if ((i == AMBA_USER_PARTITION_LINUX_ROOT_FS) && (0U != HasLinux)) {
                    if (pFile != NULL) {
                        FwImgOffset += FwHeader.AmbaFwInfo[i].Size;
                        /* after Linux rootfs, the next fw is Linux DTB */
                        if (AmbaFS_FileSeek(pFile, (INT64)FwImgOffset, AMBA_FS_SEEK_START) != SVC_OK) {
                            LOG_FWU_DBG("Warning: Cannot find linux dtb!", 0, 0);
                            RetVal = UPDATE_ERR_FILEIO_FAIL;
                        }

                        if (RetVal == SVC_OK) {
                            /* read Linux DTB header */
                            RetVal = AmbaFS_FileRead(&FwImgHeader, 1, (UINT32)sizeof(FwImgHeader), pFile, &NumSucce);
                            if ((NumSucce != (UINT32)sizeof(FwImgHeader)) || (RetVal != SVC_OK)) {
                                LOG_FWU_DBG("Warning: Cannot read linux dtb image header!", 0, 0);
                                continue;
                            }

                            /* read Linux DTB fw content */
                            RetVal = AmbaFS_FileRead(pImgBuf, 1, FwImgHeader.Length, pFile, &NumSucce);
                            if ((NumSucce != FwImgHeader.Length) || (RetVal != SVC_OK)) {
                                LOG_FWU_DBG("Warning: Linux dtb image read fail", 0, 0);
                                RetVal = UPDATE_ERR_FILEIO_FAIL;
                                continue;
                            }
                            pWrBuf = pImgBuf;
                        }
                    } else {
                        /* update from memory */
                        FwImgOffset += FwHeader.AmbaFwInfo[i].Size;
                        pWrBuf = &(pImgBuf[FwImgOffset]);
                        AmbaSvcWrap_MisraMemcpy(&FwImgHeader, pWrBuf, sizeof(FwImgHeader));
                        pWrBuf = &(pWrBuf[sizeof(FwImgHeader)]);
                    }

                    if (RetVal == SVC_OK) {
                        /* write Linux DTB into boot dev */
                        RetVal = FwUpdater_WriteDtb(FwImgHeader.Length, pWrBuf);
                        if (OK != RetVal) {
                            LOG_FWU_DBG("Warning: FwUpdater_WriteDtb fail", 0U, 0U);
                            RetVal = UPDATE_ERR_FILEIO_FAIL;
                        } else {
                            FwImgOffset += FwImgHeader.Length;
                            FwImgOffset += (UINT32)sizeof(FwImgHeader);
                            continue;
                        }
                    }
                }
                #endif
            }
            /* Get offset of the next image. */
            FwImgOffset += FwHeader.AmbaFwInfo[i].Size;
        }
    }

    return RetVal;
}

UINT32 SvcFwUpdate_GetActivePtb(UINT32 * pUserPtbNo)
{
    UINT32 i, RetVal = OK;
    UINT32 ActiveNo = 0U;

    for(i = 0; i < 2U; i++) {
        RetVal = SvcNvm_ReadUserPartitionTable(&UserPTB, i, 1000U);
        if (RetVal != OK) {
            LOG_FWU_NG("SvcNvm_ReadUserPartitionTable table i %u return 0x%x", i, RetVal);
        }
        if((RetVal == OK) && (UserPTB.PTBActive != 0U)) {
            ActiveNo = i;
            break;
        }
    }

    if (RetVal == OK) {
        *pUserPtbNo = ActiveNo;
    }

    return RetVal;
}

static UINT32 SvcFwUpdate_ShellGetPtb(UINT32 ArgCount, char * const *pArgVector, void *pData)
{
    UINT32 RetVal = OK;

    if (ArgCount < 1U) {
        RetVal = SVC_NG;
        LOG_FWU_NG("Failure to proc 'get_actptb' cmd - invalid arg count!", 0U, 0U);
        AmbaMisra_TouchUnused(pData);
    }

    if ((RetVal == OK) && (pArgVector != NULL)) {
        UINT32 ActivePtb = 0U;

        RetVal = SvcFwUpdate_GetActivePtb(&ActivePtb);

        if (RetVal == OK) {
            if (ActivePtb == 0U) {
                LOG_FWU_OK("Active PTB = 0\r\n", 0U, 0U);
            } else {
                LOG_FWU_OK("Active PTB = 1\r\n", 0U, 0U);
            }
        }
    }

    return RetVal;
}

static void SvcFwUpdate_ShellGetPtbU(void)
{
    PRN_FWU_API "  %sget_actptb%s            : get active PTB index"
        SVC_PRN_ARG_S SVC_LOG_FWU
        SVC_PRN_ARG_CSTR ("\033""[38;2;255;255;128m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
        SVC_PRN_ARG_CSTR ("\033""[0m")                                    SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    PRN_FWU_API " " SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
}

UINT32 SvcFwUpdate_SetActivePtb(UINT32 UserPtbNo, UINT32 Active)
{
    UINT32 RetVal = OK;

    if (0U != SvcNvm_ReadUserPartitionTable(&UserPTB, UserPtbNo, 1000U)) {
        LOG_FWU_NG("SvcNvm_ReadUserPartitionTable failed", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        UserPTB.PTBActive = Active;
        RetVal = SvcNvm_WriteUserPartitionTable(&UserPTB, UserPtbNo, 1000U);
        if (OK == RetVal) {
            LOG_FWU_OK("SvcFwUpdate_SetActivePtb %u %u", UserPtbNo, Active);
        } else {
            LOG_FWU_NG("SvcNvm_WriteUserPartitionTable %u failed, return 0x%x", UserPtbNo, RetVal);
        }
    }

    return RetVal;
}

static UINT32 SvcFwUpdate_ShellSetPtb(UINT32 ArgCount, char * const *pArgVector, void *pData)
{
    UINT32 RetVal = OK;

    if (ArgCount < 1U) {
        RetVal = SVC_NG;
        LOG_FWU_NG("Failure to proc 'set_actptb' cmd - invalid arg count!", 0U, 0U);
        AmbaMisra_TouchUnused(pData);
    }

    if ((RetVal == OK) && (pArgVector != NULL)) {
        UINT32 ActivePtb = 0U;

        RetVal = SvcWrap_strtoul(pArgVector[2U], &ActivePtb);    AmbaMisra_TouchUnused(&RetVal);
        if (ActivePtb == 0U) {
            RetVal = SvcFwUpdate_SetActivePtb( 0U, 1U);
            if (RetVal == OK) {
                RetVal = SvcFwUpdate_SetActivePtb( 1U, 0U);
            }
        } else {
            RetVal = SvcFwUpdate_SetActivePtb( 1U, 1U);
            if (RetVal == OK) {
                RetVal = SvcFwUpdate_SetActivePtb( 0U, 0U);
            }
        }
        #if defined(CONFIG_SVC_ENABLE_WDT)
        /* enable watch dog */
        RetVal |= SvcWdt_SetFlag(SVC_WDT_SWITCH_ENA, 1U);
        LOG_FWU_OK("Watchdog enabled.", 0U, 0U);

        if (ArgCount >= 4U) {
            UINT32 EnaWdt = 1U;
            RetVal = SvcWrap_strtoul(pArgVector[3U], &EnaWdt);   AmbaMisra_TouchUnused(&RetVal);
            if (0U == EnaWdt) {
                RetVal |= SvcWdt_SetFlag(SVC_WDT_SWITCH_ENA, 0U);
                LOG_FWU_OK("Watchdog disabled.", 0U, 0U);
            }
        }
        AmbaMisra_TouchUnused(&RetVal);
        #endif
    }

    return RetVal;
}

static void SvcFwUpdate_ShellSetPtbU(void)
{
    PRN_FWU_API "  %sset_actptb%s            : set active PTB index"
        SVC_PRN_ARG_S SVC_LOG_FWU
        SVC_PRN_ARG_CSTR ("\033""[38;2;255;255;128m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
        SVC_PRN_ARG_CSTR ("\033""[0m")                                    SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    PRN_FWU_API "  ------------------------------------------------------"    SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
    PRN_FWU_API "    [PTB_index]         : PTB index to be set (0 or 1)" SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
    PRN_FWU_API " " SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
}

#ifndef SVC_FW_UPDATE_DBG_MSG_ON
static UINT32 SVC_FwUpdateCtrlFlag = 0;
#else
static UINT32 SVC_FwUpdateCtrlFlag = SVC_FW_UPDATE_FLG_DBG_MSG;
#endif

static UINT32 SvcFwUpdate_WriteImage(AMBA_FS_FILE *pFile, UINT8 *pImgBuf, UINT32 UserPtbNo);

static void   SvcFwUpdate_ShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void   SvcFwUpdate_ShellUsage(void);
static UINT32 SvcFwUpdate_ShellMemQry(UINT32 ArgCount, char * const *pArgVector, void *pData);
static void   SvcFwUpdate_ShellMemQryU(void);
static UINT32 SvcFwUpdate_ShellCreate(UINT32 ArgCount, char * const *pArgVector, void *pData);
static void   SvcFwUpdate_ShellCreateU(void);
static UINT32 SvcFwUpdate_ShellUpdate(UINT32 ArgCount, char * const *pArgVector, void *pData);
static void   SvcFwUpdate_ShellUpdateU(void);
static UINT32 SvcFwUpdate_ShellUpdateMem(UINT32 ArgCount, char * const *pArgVector, void *pData);
static void   SvcFwUpdate_ShellUpdateMemU(void);
static UINT32 SvcFwUpdate_ShellShowPTB(UINT32 ArgCount, char * const *pArgVector, void *pData);
static void   SvcFwUpdate_ShellShowPTBU(void);
/*
UINT32 SvcFwUpdate_MemQry(UINT32 *pMemSize);
UINT32 SvcFwUpdate_Create(ULONG MemAddr, UINT32 MemSize);
UINT32 SvcFwUpdate_Update(const char *pFilePath);
void   SvcFwUpdate_CmdInstall(void);
*/

static SVC_FW_UPDATE_SHELL_FUNC_s SvcFwUpdateShellFunc[] = {
    { "mem_qry",    SvcFwUpdate_ShellMemQry,    SvcFwUpdate_ShellMemQryU },
    { "create",     SvcFwUpdate_ShellCreate,    SvcFwUpdate_ShellCreateU },
    { "update",     SvcFwUpdate_ShellUpdate,    SvcFwUpdate_ShellUpdateU },
    { "update_mem", SvcFwUpdate_ShellUpdateMem, SvcFwUpdate_ShellUpdateMemU },
    { "set_actptb", SvcFwUpdate_ShellSetPtb,    SvcFwUpdate_ShellSetPtbU },
    { "get_actptb", SvcFwUpdate_ShellGetPtb,    SvcFwUpdate_ShellGetPtbU },
    { "show_ptb",   SvcFwUpdate_ShellShowPTB,   SvcFwUpdate_ShellShowPTBU },
};

static UINT32 SvcFwUpdate_WriteImage(AMBA_FS_FILE *pFile, UINT8 *pImgBuf, UINT32 UserPtbNo)
{
    UINT32 RetVal = OK;

    if ((SVC_FwUpdateCtrlFlag & SVC_FW_UPDATE_FLG_INIT) == 0U) {
        AMBA_FIRMWARE_HEADER_s *pHeader;
        AMBA_FIRMWARE_IMAGE_HEADER_s *pImgHeader;

        AmbaMisra_TypeCast(&(pHeader), &(pImgBuf));
        if (pHeader != NULL) {
            AmbaMisra_TouchUnused(&(pHeader->CRC32));
            AmbaMisra_TouchUnused(&(pHeader->PtbMagic));
            AmbaMisra_TouchUnused(&(pHeader->PloadInfo));
        }
        AmbaMisra_TypeCast(&(pImgHeader), &(pImgBuf));
        if (pImgHeader != NULL) {
            AmbaMisra_TouchUnused(&(pImgHeader->Magic));
            AmbaMisra_TouchUnused(&(pImgHeader->Flag));
            AmbaMisra_TouchUnused(pImgHeader->Reserved);
        }

        RetVal = SVC_NG;
        LOG_FWU_NG("Failure to write image - create fw update module first!", 0U, 0U);
    } else {
        UINT32 ErrCode = Update_WriteImage(pFile, pImgBuf, UserPtbNo);
        if (ErrCode != 0U) {
            RetVal = SVC_NG;
            SvcFwUpdate_ShowPTB();
        }
    }

    return RetVal;
}

static void SvcFwUpdate_ShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = OK, PRetVal;

    if(ArgCount == 0xCafeU) {
        UINT32 SIdx;

        LOG_FWU_API("", 0U, 0U);
        PRN_FWU_API "==== %sShell Arguments Setting%s (%s%d%s) ===="
            SVC_PRN_ARG_S SVC_LOG_FWU
            SVC_PRN_ARG_CSTR ("\033""[38;2;255;255;128m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
            SVC_PRN_ARG_CSTR ("\033""[0m")                                    SVC_PRN_ARG_POST
            SVC_PRN_ARG_CSTR ("\033""[38;2;255;128;128m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
            SVC_PRN_ARG_UINT32 ArgCount                                       SVC_PRN_ARG_POST
            SVC_PRN_ARG_CSTR ("\033""[0m")                                    SVC_PRN_ARG_POST
            SVC_PRN_ARG_E

        for (SIdx = 0U; SIdx < ArgCount; SIdx ++) {
            PRN_FWU_API "  pArgVector[%s%d%s] : %s%s%s"
               SVC_PRN_ARG_S SVC_LOG_FWU
               SVC_PRN_ARG_CSTR   ("\033""[38;2;255;128;128m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
               SVC_PRN_ARG_UINT32 SIdx                                             SVC_PRN_ARG_POST
               SVC_PRN_ARG_CSTR   ("\033""[0m")                                    SVC_PRN_ARG_POST
               SVC_PRN_ARG_CSTR   ("\033""[38;2;0;255;128m""\033""[48;2;0;0;0m")   SVC_PRN_ARG_POST
               SVC_PRN_ARG_CSTR   pArgVector[SIdx]                                 SVC_PRN_ARG_POST
               SVC_PRN_ARG_CSTR   ("\033""[0m")                                    SVC_PRN_ARG_POST
               SVC_PRN_ARG_E
        }

        AmbaMisra_TouchUnused(&PrintFunc);
    }

    if ((ArgCount >= 1U) && (pArgVector != NULL)) {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(SvcFwUpdateShellFunc)) / (UINT32)(sizeof(SvcFwUpdateShellFunc[0]));

        RetVal = SVC_NG;

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if (SvcFwUpdateShellFunc[ShellIdx].pFunc != NULL) {
                if (0 == SvcWrap_strcmp(pArgVector[1U], SvcFwUpdateShellFunc[ShellIdx].CmdName)) {
                    PRetVal = (SvcFwUpdateShellFunc[ShellIdx].pFunc)(ArgCount, pArgVector, NULL); AmbaMisra_TouchUnused(&PRetVal);
                    RetVal = OK;
                    break;
                }
            }
        }
    }

    if (RetVal != OK) {
        SvcFwUpdate_ShellUsage();
    }
}

static void SvcFwUpdate_ShellUsage(void)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(SvcFwUpdateShellFunc)) / (UINT32)(sizeof(SvcFwUpdateShellFunc[0]));

    LOG_FWU_API("", 0U, 0U);
    PRN_FWU_API "====== %sFirmware Update Usage%s ======"
        SVC_PRN_ARG_S SVC_LOG_FWU
        SVC_PRN_ARG_CSTR ("\033""[38;2;255;125;38m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
        SVC_PRN_ARG_CSTR ("\033""[0m")                                   SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if (SvcFwUpdateShellFunc[ShellIdx].pFunc != NULL) {
            if (SvcFwUpdateShellFunc[ShellIdx].pUsage == NULL) {
                PRN_FWU_API "  %s"
                    SVC_PRN_ARG_S   SVC_LOG_FWU
                    SVC_PRN_ARG_CSTR SvcFwUpdateShellFunc[ShellIdx].CmdName SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
            } else {
                (SvcFwUpdateShellFunc[ShellIdx].pUsage)();
            }
        }
    }

    LOG_FWU_API("", 0U, 0U);
}

static UINT32 SvcFwUpdate_ShellMemQry(UINT32 ArgCount, char * const *pArgVector, void *pData)
{
    UINT32 RetVal = OK;

    if (ArgCount < 1U) {
        RetVal = SVC_NG;
        LOG_FWU_NG("Failure to proc 'mem_qry' cmd - invalid arg count!", 0U, 0U);
        AmbaMisra_TouchUnused(pData);
    }

    if ((RetVal == OK) && (pArgVector != NULL)) {
        RetVal = SvcFwUpdate_MemQry(NULL);
    }

    return RetVal;
}

static void SvcFwUpdate_ShellMemQryU(void)
{
    PRN_FWU_API "  %smem_qry%s               : query firmware update module memory size"
        SVC_PRN_ARG_S SVC_LOG_FWU
        SVC_PRN_ARG_CSTR ("\033""[38;2;255;255;128m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
        SVC_PRN_ARG_CSTR ("\033""[0m")                                    SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    PRN_FWU_API " " SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
}

static UINT32 SvcFwUpdate_ShellCreate(UINT32 ArgCount, char * const *pArgVector, void *pData)
{
    UINT32 RetVal = OK, PRetVal;

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        LOG_FWU_NG("Failure to proc 'create' cmd - invalid arg count!", 0U, 0U);
        AmbaMisra_TouchUnused(pData);
    }

    if ((RetVal == OK) && (pArgVector != NULL)) {
        UINT32 MemAddr = 0U;
        UINT32 MemSize = 0U;

        PRetVal = SvcWrap_strtoul(pArgVector[2U], &MemAddr);    AmbaMisra_TouchUnused(&PRetVal);
        PRetVal = SvcWrap_strtoul(pArgVector[3U], &MemSize);    AmbaMisra_TouchUnused(&PRetVal);

        RetVal = SvcFwUpdate_Create(MemAddr, MemSize);
    }

    return RetVal;
}

static void SvcFwUpdate_ShellCreateU(void)
{
    PRN_FWU_API "  %screate%s                : create firmware update module"
        SVC_PRN_ARG_S SVC_LOG_FWU
        SVC_PRN_ARG_CSTR ("\033""[38;2;255;255;128m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
        SVC_PRN_ARG_CSTR ("\033""[0m")                                    SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    PRN_FWU_API "  ------------------------------------------------------"    SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
    PRN_FWU_API "    [MemAddr]           : Configure the raw enc memory addr" SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
    PRN_FWU_API "    [MemSize]           : Configure the raw enc memory size" SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
    PRN_FWU_API " " SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
}

static UINT32 SvcFwUpdate_ShellUpdate(UINT32 ArgCount, char * const *pArgVector, void *pData)
{
    UINT32 RetVal = OK;

    if (ArgCount < 1U) {
        RetVal = SVC_NG;
        LOG_FWU_NG("Failure to proc 'update' cmd - invalid arg count!", 0U, 0U);
        AmbaMisra_TouchUnused(pData);
    }

    if ((RetVal == OK) && (pArgVector != NULL)) {
        char FilePath[64];
        UINT32 UserPtbNo = 0xFFFFFFFFU;

        AmbaSvcWrap_MisraMemset(FilePath, 0, sizeof(FilePath));
        if (ArgCount > 2U) {
            SvcWrap_strcpy(FilePath, sizeof(FilePath), pArgVector[2U]);
        } else {
            SvcWrap_strcpy(FilePath, sizeof(FilePath), SVC_FW_UPDATA_DEF_FILE_NAME);
        }
        if (ArgCount > 3U) {
            RetVal = SvcWrap_strtoul(pArgVector[3U], &UserPtbNo); AmbaMisra_TouchUnused(&RetVal);
            if (UserPtbNo != 0U) {
                UserPtbNo = 1;
            }
        } else {
            UserPtbNo = 0;
        }
        LOG_FWU_DBG("UserPtbNo %u", UserPtbNo, 0U);

        RetVal = SvcFwUpdate_Update(FilePath, UserPtbNo);
    }

    return RetVal;
}

static void SvcFwUpdate_ShellUpdateU(void)
{
    PRN_FWU_API "  %supdate%s                : update firmware by file"
        SVC_PRN_ARG_S SVC_LOG_FWU
        SVC_PRN_ARG_CSTR ("\033""[38;2;255;255;128m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
        SVC_PRN_ARG_CSTR ("\033""[0m")                                    SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    PRN_FWU_API "  ------------------------------------------------------"       SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
    PRN_FWU_API "    [FilePath]          : Configure the firmware file path"     SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
    PRN_FWU_API "                        : Default file path '%s'"
        SVC_PRN_ARG_S    SVC_LOG_FWU
        SVC_PRN_ARG_CSTR SVC_FW_UPDATA_DEF_FILE_NAME SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    PRN_FWU_API " " SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
}

static UINT32 SvcFwUpdate_ShellUpdateMem(UINT32 ArgCount, char * const *pArgVector, void *pData)
{
    UINT8   *pImgBuf;
    ULONG   MemAddr;
    UINT32  RetVal = OK, UserPtbNo;

    if (ArgCount < 1U) {
        RetVal = SVC_NG;
        LOG_FWU_NG("Failure to proc 'update' cmd - invalid arg count!", 0U, 0U);
        AmbaMisra_TouchUnused(pData);
    }

    if ((RetVal == OK) && (pArgVector != NULL)) {
        if (2U < ArgCount) {
            if (3U < ArgCount) {
                if (0 == SvcWrap_strcmp(pArgVector[2U], "go")) {
                    (void)SvcWrap_strtoul(pArgVector[3U], &UserPtbNo);

                    MemAddr = SVC_FwUpdateCtrl.MemAddr;
                    AmbaMisra_TypeCast(&pImgBuf, &MemAddr);
                    RetVal = SvcFwUpdate_WriteImage(NULL, pImgBuf, UserPtbNo);
                }
            } else {
                if (0 == SvcWrap_strcmp(pArgVector[2U], "info")) {
                    PRN_FWU_API "  image memory buffer address/size(0x%X/0x%X)"
                        SVC_PRN_ARG_S SVC_LOG_FWU
                        SVC_PRN_ARG_UINT32 SVC_FwUpdateCtrl.MemAddr SVC_PRN_ARG_POST
                        SVC_PRN_ARG_UINT32 SVC_FwUpdateCtrl.MemSize SVC_PRN_ARG_POST
                        SVC_PRN_ARG_E
                }
            }
        }
    }

    return RetVal;
}

static void SvcFwUpdate_ShellUpdateMemU(void)
{
    PRN_FWU_API "  %supdate_mem%s             : update firmware from memory"
        SVC_PRN_ARG_S SVC_LOG_FWU
        SVC_PRN_ARG_CSTR ("\033""[38;2;255;255;128m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
        SVC_PRN_ARG_CSTR ("\033""[0m")                                    SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    PRN_FWU_API "  ------------------------------------------------------" SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
    PRN_FWU_API "    info                : Show memory buffer address"     SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
    PRN_FWU_API "    go                  : Do firmware program from memory"SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
    PRN_FWU_API " " SVC_PRN_ARG_S SVC_LOG_FWU SVC_PRN_ARG_E
}

/**
* memory size query of firmware update module
* @param [out] pMemSize needed memory size
* @return 0-OK, 1-NG
*/
UINT32 SvcFwUpdate_MemQry(UINT32 *pMemSize)
{
    if (pMemSize != NULL) {
        *pMemSize = (UINT32)SVC_FW_UPDATE_MAX_FILE_SIZE;
    }
    LOG_FWU_DBG("Query the firmware update memory size 0x%x", (UINT32)SVC_FW_UPDATE_MAX_FILE_SIZE, 0U);

    return OK;
}

/**
* create of firmware update module
* @param [in] MemAddr base of memory
* @param [in] MemSize size of memory
* @return 0-OK, 1-NG
*/
UINT32 SvcFwUpdate_Create(ULONG MemAddr, UINT32 MemSize)
{
    UINT32 RetVal = OK;

    if (MemAddr == 0U) {
        RetVal = SVC_NG;
        LOG_FWU_NG("Failure to create fw update module - input memory address should not zero!", 0U, 0U);
    }

    if (MemSize == 0U) {
        RetVal = SVC_NG;
        LOG_FWU_NG("Failure to create fw update module - input memory size should not zero!", 0U, 0U);
    }

    if (RetVal == OK) {
        AmbaSvcWrap_MisraMemset(&SVC_FwUpdateCtrl, 0, sizeof(SVC_FwUpdateCtrl));
        SVC_FwUpdateCtrl.MemAddr = MemAddr;
        SVC_FwUpdateCtrl.MemSize = MemSize;

        SVC_FwUpdateCtrlFlag |= SVC_FW_UPDATE_FLG_INIT;
        LOG_FWU_OK("Successful to create fw update module!", 0U, 0U);
    }

    return RetVal;
}

/**
* do firmware update from file
* @param [in] pFilePath path of file
* @return 0-OK, 1-NG
*/
UINT32 SvcFwUpdate_Update(const char *pFilePath, UINT32 UserPtbNo)
{
    UINT32 RetVal = OK, PRetVal;

    if ((SVC_FwUpdateCtrlFlag & SVC_FW_UPDATE_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_FWU_NG("Failure to update firmware - create fw update module first!", 0U, 0U);
    }

    if (pFilePath == NULL) {
        RetVal = SVC_NG;
        LOG_FWU_NG("Failure to update firmware - firmware file path should not null!", 0U, 0U);
    }

    if (RetVal == OK) {
        AMBA_FS_FILE_INFO_s FileInfo;

        AmbaSvcWrap_MisraMemset(&FileInfo, 0, sizeof(FileInfo));
        if (0U != AmbaFS_GetFileInfo(pFilePath, &FileInfo)) {
            RetVal = SVC_NG;
            PRN_FWU_NG "Failure to update firmware - get file info fail %s"
                SVC_PRN_ARG_S SVC_LOG_FWU
                SVC_PRN_ARG_CSTR pFilePath SVC_PRN_ARG_POST
                SVC_PRN_ARG_E
        } else {
            if (FileInfo.Size == 0U) {
                RetVal = SVC_NG;
                LOG_FWU_NG("Failure to update firmware - file size should not zero!", 0U, 0U);
            } else {
                // if (FileInfo.Size > SVC_FwUpdateCtrl.MemSize) {
                //     RetVal = SVC_NG;
                    LOG_FWU_DBG("FileSize total %u, MemSize %u", (UINT32)FileInfo.Size, SVC_FwUpdateCtrl.MemSize);
                // }
            }
        }
    }

    if (RetVal == OK) {
        AMBA_FS_FILE *pFile = NULL;

        PRN_FWU_API "Open file to update firmware %s\n"
            SVC_PRN_ARG_S SVC_LOG_FWU
            SVC_PRN_ARG_CSTR pFilePath SVC_PRN_ARG_POST
            SVC_PRN_ARG_E

        if (0U != AmbaFS_FileOpen(pFilePath, "rb", &pFile)) {
            RetVal = SVC_NG;
            if ((SVC_FwUpdateCtrlFlag & SVC_FW_UPDATE_FLG_DBG_MSG) > 0U) {
                PRN_FWU_NG "Failure to update firmware - the firmware file does not exist - %s"
                    SVC_PRN_ARG_S   SVC_LOG_FWU
                    SVC_PRN_ARG_CSTR pFilePath SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
            }
        } else {
            UINT8 *pBuf = NULL;

            AmbaMisra_TypeCast(&(pBuf), &(SVC_FwUpdateCtrl.MemAddr));

            if (pBuf == NULL) {
                RetVal = SVC_NG;
                LOG_FWU_NG("Failure to update firmware - invalid buffer!", 0U, 0U);
            } else {
                UINT32 ErrCode;

                AmbaSvcWrap_MisraMemset(pBuf, 0, SVC_FwUpdateCtrl.MemSize);
                ErrCode = SvcFwUpdate_WriteImage(pFile, pBuf, UserPtbNo);
                if (ErrCode != 0U) {
                    RetVal = SVC_NG;
                    LOG_FWU_NG("Failure to update firmware - write image fail! ErrCode: 0x%x", ErrCode, 0U);
                }
            }

            PRetVal = AmbaFS_FileClose(pFile); AmbaMisra_TouchUnused(&PRetVal);
        }
    }

    return RetVal;
}

/**
* shell commands installation of firmware update module
* @return none
*/
void SvcFwUpdate_CmdInstall(void)
{
    AMBA_SHELL_COMMAND_s SvcCmdFwUpdate;
    UINT32  Rval;

    AmbaSvcWrap_MisraMemset(&SvcCmdFwUpdate, 0, sizeof(SvcCmdFwUpdate));

    SvcCmdFwUpdate.pName    = "svc_fwupdate";
    SvcCmdFwUpdate.MainFunc = SvcFwUpdate_ShellEntry;
    SvcCmdFwUpdate.pNext    = NULL;

    Rval = SvcCmd_CommandRegister(&SvcCmdFwUpdate);
    if (SHELL_ERR_SUCCESS != Rval) {
        LOG_FWU_NG("## fail to install svc shell fwupdate command", 0U, 0U);
    }
}
