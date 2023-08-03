#include "AmbaRTSL_GPIO.h"
#include "AmbaRTSL_UART.h"
//#include "SvcNvm.h"
#include "AmbaNVM_Partition.h"
#include <AmbaSD_Priv.h>
#include <AmbaSD_Ctrl.h>
#include <app_helper.h>
//#include <SvcStorage.h>
#include ".svc_autogen"
#include <platform.h>
#include <pmic.h>
#include <custom.h>
#include "AmbaFirmwareUpdater.h"
#if !defined(CONFIG_ENABLE_EMMC_BOOT)
#include "AmbaRTSL_NAND_Ctrl.h"
#include "AmbaNAND_Ctrl.h"
#endif
#include <bsp.h>
#include <user_setting/user_setting.h>

#define LINE_BUFFER_LENGTH   (4096U)
static char _AmbaPrint_LineBuf[LINE_BUFFER_LENGTH] GNU_SECTION_NOZEROINIT; /* Temporary buffer */

void AmbaPrint_PrintStr5Fwprog(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{
    const char *Arg[5];
    UINT32  RetVal, ActualTxSize, UartTxSize, Index = 0;

    Arg[0] = pArg1;
    Arg[1] = pArg2;
    Arg[2] = pArg3;
    Arg[3] = pArg4;
    Arg[4] = pArg5;
    UartTxSize = AmbaUtility_StringPrintStr(_AmbaPrint_LineBuf, LINE_BUFFER_LENGTH, pFmt, 5, Arg);
    while ((RetVal = AmbaRTSL_UartWrite(AMBA_UART_APB_CHANNEL0, UartTxSize, (const UINT8 *)&_AmbaPrint_LineBuf[Index], &ActualTxSize)) == UART_ERR_NONE) {
        UartTxSize -= ActualTxSize;
        Index += ActualTxSize;
        if (UartTxSize == 0) {
            break;
        }
    }
}

void AmbaPrint_PrintUInt5Fwprog(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    UINT32 Arg[5];
    UINT32  RetVal, ActualTxSize, UartTxSize, Index = 0;

    Arg[0] = Arg1;
    Arg[1] = Arg2;
    Arg[2] = Arg3;
    Arg[3] = Arg4;
    Arg[4] = Arg5;
    UartTxSize = AmbaUtility_StringPrintUInt32(_AmbaPrint_LineBuf, LINE_BUFFER_LENGTH, pFmt, 5, Arg);
    while ((RetVal = AmbaRTSL_UartWrite(AMBA_UART_APB_CHANNEL0, UartTxSize, (const UINT8 *)&_AmbaPrint_LineBuf[Index], &ActualTxSize)) == UART_ERR_NONE) {
        UartTxSize -= ActualTxSize;
        Index += ActualTxSize;
        if (UartTxSize == 0) {
            break;
        }
    }
}

/* Partition table is pre-load into memory by BST */
static unsigned char fw_buf[2 * 0x100000] = {0};

static int update_boot_firmware(void *pFile, unsigned int offset, unsigned int *app_offset)
{
    UINT32 i = 0, BurnPart = 0;
    AMBA_FIRMWARE_IMAGE_HEADER_s FwImgHeader;
    AMBA_PARTITION_ENTRY_s *pPartEntry;
    AMBA_FIRMWARE_HEADER_s FwHeader;
    UINT32 FwHeaderSize = sizeof(AMBA_FIRMWARE_HEADER_s);
    UINT32 FwImgOffset = offset + 4 + sizeof(AMBA_FIRMWARE_HEADER_s);
    UINT32 NvmID, NumSucce = 0U;
#if defined(CONFIG_ENABLE_EMMC_BOOT)
    extern AMBA_USER_PARTITION_TABLE_s *_pAmbaRTSL_EmmcUserPartTable;
    AMBA_USER_PARTITION_TABLE_s *_pAmbaRTSL_UserPartTable = _pAmbaRTSL_EmmcUserPartTable;
    UINT32 BlockSize = 512U;
#else
    extern AMBA_SYS_PARTITION_TABLE_s *pAmbaNandTblSysPart;
    AMBA_SYS_PARTITION_TABLE_s *_pAmbaRTSL_SysPartTable = pAmbaNandTblSysPart;
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 BlockSize = pNandDevInfo->MainByteSize * pNandDevInfo->BlockPageSize;
#endif
    UINT32 BldSize = 0;

    for (i = 0; i < AMBA_NUM_USER_PARTITION; i++) {
        FwHeader.AmbaFwInfo[i].Size = 0;
        FwHeader.AmbaFwInfo[i].Crc32 = 0;
    }

    NvmID = NVM_TYPE;

    if (fseek(pFile, offset, AMBA_FS_SEEK_START) != OK) {
        AmbaPrint_PrintStr5Fwprog("Cannot seek fw image header!\r\n", NULL, NULL, NULL, NULL, NULL);
        return -1;
    }

    NumSucce = fread(&BldSize, 1, 4, pFile);
    if (NumSucce != 4) {
        AmbaPrint_PrintStr5Fwprog("can't read firmware file!\r\n", NULL, NULL, NULL, NULL, NULL);
        return -1;
    }
    if (app_offset != NULL) {
        *app_offset = (BldSize + offset + 4);
    }

    if (fseek(pFile, offset + 4, AMBA_FS_SEEK_START) != OK) {
        AmbaPrint_PrintStr5Fwprog("Cannot seek fw image header!\r\n", NULL, NULL, NULL, NULL, NULL);
        return -1;
    }

    NumSucce = fread(&FwHeader, 1, FwHeaderSize, pFile);
    if (NumSucce != FwHeaderSize) {
        AmbaPrint_PrintStr5Fwprog("can't read firmware file!\r\n", NULL, NULL, NULL, NULL, NULL);
        return -1;
    }

    if (memcmp(FwHeader.ModelName, PROJECT_MODEL, strlen(PROJECT_MODEL)) != 0) {
        AmbaPrint_PrintStr5Fwprog("invalid boot model, %s, shoule be %s!\r\n", (char *)FwHeader.ModelName, PROJECT_MODEL, NULL, NULL, NULL);
        return -1;
    }

    AmbaNVM_ReadSysPartitionTable(NvmID, NULL, 5000U);
    for (i = AMBA_SYS_PARTITION_BOOTSTRAP; i <= AMBA_SYS_PARTITION_FW_UPDATER; i++) {
        pPartEntry = (AMBA_PARTITION_ENTRY_s *)&(_pAmbaRTSL_SysPartTable->Entry[i]);
        if (FwHeader.AmbaFwInfo[i].Size == 0U) {
            continue;
        }
        BurnPart = i;
        /* Read partition header. */
        if (fseek(pFile, FwImgOffset, AMBA_FS_SEEK_START) != OK) {
            AmbaPrint_PrintStr5Fwprog("Cannot read fw image header!\r\n", NULL, NULL, NULL, NULL, NULL);
            continue;
        }
        NumSucce = fread(&FwImgHeader, 1, sizeof(FwImgHeader), pFile);
        if (NumSucce != sizeof(FwImgHeader)) {
            AmbaPrint_PrintStr5Fwprog("Cannot read fw image header!\r\n", NULL, NULL, NULL, NULL, NULL);
            continue;
        }
        FwImgOffset += FwHeader.AmbaFwInfo[i].Size;
        AmbaPrint_PrintStr5Fwprog("\r\n%s found in firmware!\r\n", (char *)pPartEntry->PartitionName, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5Fwprog("\r    Crc32  :    0x%08x\r\n",      FwImgHeader.Crc32, 0, 0, 0, 0);
        AmbaPrint_PrintUInt5Fwprog("\r    Version:    %d.%d\r\n",       FwImgHeader.Version >> 16U, (FwImgHeader.Version & 0xffffU), 0, 0, 0);
        AmbaPrint_PrintUInt5Fwprog("\r    VerDate:    %d/%d/%d\r\n",    FwImgHeader.Date >> 16U, ((FwImgHeader.Date >> 8U) & 0xffU), (FwImgHeader.Date & 0xffU), 0, 0);
        AmbaPrint_PrintUInt5Fwprog("\r    ImgLen :    %d\r\n",          FwImgHeader.Length, 0, 0, 0, 0);
        AmbaPrint_PrintUInt5Fwprog("\r    MemAddr:    0x%08x\r\n",      FwImgHeader.MemAddr, 0, 0, 0, 0);
        AmbaPrint_PrintUInt5Fwprog("\r    Magic  :    0x%08x\r\n",      FwImgHeader.Magic, 0, 0, 0, 0);
        AmbaPrint_PrintUInt5Fwprog("\r    Attr   :    0x%08x\r\n",      pPartEntry->Attribute, 0, 0, 0, 0);

        AmbaPrint_PrintUInt5Fwprog("\rProgram image to Flash(0x%X)\r\n", pPartEntry->StartBlkAddr, 0, 0, 0, 0);
        {
            int k = 0, times = 0;
            UINT32 StartSector = 0, BadSectors = 0;
    
            times = FwImgHeader.Length / sizeof(fw_buf);
            if ((FwImgHeader.Length % sizeof(fw_buf)) != 0) {
                times += 1;
            }
            for (k = 0; k < times; k++) {
                AMBA_NVM_SECTOR_CONFIG_s SecConfig;
                UINT32 one_time_size = ((FwImgHeader.Length - k * sizeof(fw_buf)) >= sizeof(fw_buf)) ? sizeof(fw_buf) : (FwImgHeader.Length % sizeof(fw_buf));
                NumSucce = fread(fw_buf, 1, one_time_size, pFile);
                if (NumSucce != one_time_size) {
                    AmbaPrint_PrintStr5Fwprog("firmware image read fail\r\n", NULL, NULL, NULL, NULL, NULL);
                    return -1;
                }
                SecConfig.NumSector   = (one_time_size + BlockSize - 1U) / BlockSize;
                SecConfig.pDataBuf    = (UINT8 *)fw_buf;
                SecConfig.StartSector = StartSector;
                if (AmbaNVM_WriteRawSector(NvmID, 0, BurnPart, &SecConfig, &BadSectors, 5000) != OK) {
                    AmbaPrint_PrintStr5Fwprog("AmbaNVM_WriteSector Failed\r\n", NULL, NULL, NULL, NULL, NULL);
                    return -1;
                }
                StartSector += (SecConfig.NumSector + BadSectors);
                AmbaPrint_PrintUInt5Fwprog("\r\x1b[A\x1b[32C\x1b[0K%3u%%\r\n", (k + 1) * 100 / times, 0, 0, 0, 0);
            }
            AmbaPrint_PrintStr5Fwprog("\r%s program ok\r\n\r\n", (char *)pPartEntry->PartitionName, NULL, NULL, NULL, NULL);
        }
        /* Update the PTB's entry */
        pPartEntry->ActualByteSize  = FwImgHeader.Length;
        pPartEntry->RamLoadAddr     = FwImgHeader.MemAddr;
        pPartEntry->ImageCRC32      = FwImgHeader.Crc32;
        pPartEntry->ProgramStatus   = 0;
        pPartEntry->Attribute       = FwHeader.SysPartitionInfo[i].Attribute;
        if (AmbaNVM_WriteSysPartitionTable(NvmID, NULL, 5000U) != OK) {
            AmbaPrint_PrintStr5Fwprog("Unable to sys ptb %s\r\n", (const char *)pPartEntry->PartitionName, NULL, NULL, NULL, NULL);
            return -1;
        }
    }

    return 0;
}

static int update_app_firmware(void *pFile, unsigned int offset, int is_a)
{
    UINT32 i = 0, BurnPart = 0;
    AMBA_FIRMWARE_IMAGE_HEADER_s FwImgHeader;
    AMBA_PARTITION_ENTRY_s *pPartEntry;
    AMBA_FIRMWARE_HEADER_s FwHeader;
    UINT32 FwHeaderSize = sizeof(AMBA_FIRMWARE_HEADER_s);
    UINT32 FwImgOffset = offset + sizeof(AMBA_FIRMWARE_HEADER_s);
    UINT32 NvmID, NumSucce = 0U;
    int UserPtbNo = 0;
#if defined(CONFIG_ENABLE_EMMC_BOOT)
    extern AMBA_USER_PARTITION_TABLE_s *_pAmbaRTSL_EmmcUserPartTable;
    AMBA_USER_PARTITION_TABLE_s *_pAmbaRTSL_UserPartTable = _pAmbaRTSL_EmmcUserPartTable;
    UINT32 BlockSize = 512U;
#else
    extern AMBA_USER_PARTITION_TABLE_s *pAmbaNandTblUserPart;
    AMBA_USER_PARTITION_TABLE_s *_pAmbaRTSL_UserPartTable = pAmbaNandTblUserPart;
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 BlockSize = pNandDevInfo->MainByteSize * pNandDevInfo->BlockPageSize;
#endif

    for (i = 0; i < AMBA_NUM_USER_PARTITION; i++) {
        FwHeader.AmbaFwInfo[i].Size = 0;
        FwHeader.AmbaFwInfo[i].Crc32 = 0;
    }

    NvmID = NVM_TYPE;

    if (fseek(pFile, offset, AMBA_FS_SEEK_START) != OK) {
        AmbaPrint_PrintStr5Fwprog("Cannot seek fw image header!\r\n", NULL, NULL, NULL, NULL, NULL);
        return -1;
    }

    NumSucce = fread(&FwHeader, 1, FwHeaderSize, pFile);
    if (NumSucce != FwHeaderSize) {
        AmbaPrint_PrintStr5Fwprog("can't read firmware file!\r\n", NULL, NULL, NULL, NULL, NULL);
        return -1;
    }

    if (memcmp(FwHeader.ModelName, PROJECT_MODEL, strlen(PROJECT_MODEL)) != 0) {
        AmbaPrint_PrintStr5Fwprog("invalid app model, %s, shoule be %s!\r\n", (char *)FwHeader.ModelName, PROJECT_MODEL, NULL, NULL, NULL);
        return -1;
    }

    AmbaNVM_ReadUserPartitionTable(NvmID, NULL, UserPtbNo, 5000U);
    for (i = AMBA_USER_PARTITION_SYS_SOFTWARE; i <= AMBA_USER_PARTITION_LINUX_ROOT_FS; i++) {
        pPartEntry = (AMBA_PARTITION_ENTRY_s *)&(_pAmbaRTSL_UserPartTable->Entry[i]);
        if (FwHeader.AmbaFwInfo[i].Size == 0U) {
            AmbaPrint_PrintStr5Fwprog("%s == 0 !\r\n", (char *)pPartEntry->PartitionName, NULL, NULL, NULL, NULL);
            continue;
        }
        BurnPart = i;
        /* Read partition header. */
        if (fseek(pFile, FwImgOffset, AMBA_FS_SEEK_START) != OK) {
            AmbaPrint_PrintStr5Fwprog("Cannot read fw image header!\r\n", NULL, NULL, NULL, NULL, NULL);
            continue;
        }
        NumSucce = fread(&FwImgHeader, 1, sizeof(FwImgHeader), pFile);
        if (NumSucce != sizeof(FwImgHeader)) {
            AmbaPrint_PrintStr5Fwprog("Cannot read fw image header!\r\n", NULL, NULL, NULL, NULL, NULL);
            continue;
        }
        /* Get offset of the next image. */
        FwImgOffset += FwHeader.AmbaFwInfo[i].Size;
        if (i >= AMBA_USER_PARTITION_SYS_SOFTWARE && i <= AMBA_USER_PARTITION_SYS_DATA) {
            if (is_a == 0) {
                BurnPart = i + (AMBA_USER_PARTITION_SYS_SOFTWARE_B - AMBA_USER_PARTITION_SYS_SOFTWARE);
            }
            pPartEntry = (AMBA_PARTITION_ENTRY_s *)&(_pAmbaRTSL_UserPartTable->Entry[BurnPart]);
        } else if (i >= AMBA_USER_PARTITION_LINUX_KERNEL && i <= AMBA_USER_PARTITION_LINUX_ROOT_FS) {
            if (is_a == 0) {
                BurnPart = i + (AMBA_USER_PARTITION_LINUX_KERNEL_B - AMBA_USER_PARTITION_LINUX_KERNEL);
            }
            pPartEntry = (AMBA_PARTITION_ENTRY_s *)&(_pAmbaRTSL_UserPartTable->Entry[BurnPart]);
        }
        AmbaPrint_PrintStr5Fwprog("\r\n%s found in firmware!\r\n", (char *)pPartEntry->PartitionName, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5Fwprog("\r    Crc32  :    0x%08x\r\n",      FwImgHeader.Crc32, 0, 0, 0, 0);
        AmbaPrint_PrintUInt5Fwprog("\r    Version:    %d.%d\r\n",       FwImgHeader.Version >> 16U, (FwImgHeader.Version & 0xffffU), 0, 0, 0);
        AmbaPrint_PrintUInt5Fwprog("\r    VerDate:    %d/%d/%d\r\n",    FwImgHeader.Date >> 16U, ((FwImgHeader.Date >> 8U) & 0xffU), (FwImgHeader.Date & 0xffU), 0, 0);
        AmbaPrint_PrintUInt5Fwprog("\r    ImgLen :    %d\r\n",          FwImgHeader.Length, 0, 0, 0, 0);
        AmbaPrint_PrintUInt5Fwprog("\r    MemAddr:    0x%08x\r\n",      FwImgHeader.MemAddr, 0, 0, 0, 0);
        AmbaPrint_PrintUInt5Fwprog("\r    Magic  :    0x%08x\r\n",      FwImgHeader.Magic, 0, 0, 0, 0);
        AmbaPrint_PrintUInt5Fwprog("\r    Attr   :    0x%08x\r\n",      pPartEntry->Attribute, 0, 0, 0, 0);

        AmbaPrint_PrintUInt5Fwprog("\rProgram image to Flash(0x%X)\r\n", pPartEntry->StartBlkAddr, 0, 0, 0, 0);

        {
            int k = 0, times = 0;
            UINT32 StartSector = 0, BadSectors = 0;

            times = FwImgHeader.Length / sizeof(fw_buf);
            if ((FwImgHeader.Length % sizeof(fw_buf)) != 0) {
                times += 1;
            }
            for (k = 0; k < times; k++) {
                AMBA_NVM_SECTOR_CONFIG_s SecConfig;
                UINT32 one_time_size = ((FwImgHeader.Length - k * sizeof(fw_buf)) >= sizeof(fw_buf)) ? sizeof(fw_buf) : (FwImgHeader.Length % sizeof(fw_buf));
                NumSucce = fread(fw_buf, 1, one_time_size, pFile);
                if (NumSucce != one_time_size) {
                    AmbaPrint_PrintStr5Fwprog("firmware image read fail\r\n", NULL, NULL, NULL, NULL, NULL);
                    return -1;
                }
                SecConfig.NumSector   = (one_time_size + BlockSize - 1U) / BlockSize;
                SecConfig.pDataBuf    = (UINT8 *)fw_buf;
                SecConfig.StartSector = StartSector;
                if (AmbaNVM_WriteRawSector(NvmID, 1, BurnPart, &SecConfig, &BadSectors, 5000) != OK) {
                    AmbaPrint_PrintStr5Fwprog("AmbaNVM_WriteSector Failed\r\n", NULL, NULL, NULL, NULL, NULL);
                    return -1;
                }
                StartSector += (SecConfig.NumSector + BadSectors);
                AmbaPrint_PrintUInt5Fwprog("\r\x1b[A\x1b[32C\x1b[0K%3u%%\r\n", (k + 1) * 100 / times, 0, 0, 0, 0);
            }
            AmbaPrint_PrintStr5Fwprog("\r%s program ok\r\n\r\n", (char *)pPartEntry->PartitionName, NULL, NULL, NULL, NULL);
        }
        /* Update the PTB's entry */
        pPartEntry->ActualByteSize  = FwImgHeader.Length;
        pPartEntry->RamLoadAddr     = FwImgHeader.MemAddr;
        pPartEntry->ImageCRC32      = FwImgHeader.Crc32;
        pPartEntry->ProgramStatus   = 0;
        pPartEntry->Attribute       = FwHeader.UserPartitionInfo[i].Attribute;
        if (AmbaNVM_WriteUserPartitionTable(NvmID, NULL, UserPtbNo, 5000U) != OK) {
            AmbaPrint_PrintStr5Fwprog("Unable to update ptb %s\r\n", (const char *)pPartEntry->PartitionName, NULL, NULL, NULL, NULL);
            return -1;
        }

        if (i == AMBA_USER_PARTITION_LINUX_KERNEL) {
            UINT32 Size = 0, j = 0;
            UINT32 DtbImageOffset = 0;
            //get rfs image len
            if (fseek(pFile, FwImgOffset, AMBA_FS_SEEK_START) != OK) {
                AmbaPrint_PrintStr5Fwprog("Cannot read rfs image header!\r\n", NULL, NULL, NULL, NULL, NULL);
                return -1;
            }
            NumSucce = fread(&FwImgHeader, 1, sizeof(FwImgHeader), pFile);
            if (NumSucce != sizeof(FwImgHeader)) {
                AmbaPrint_PrintStr5Fwprog("Cannot read rfs image header!\r\n", NULL, NULL, NULL, NULL, NULL);
                return -1;
            }
            //get dtb image len
            DtbImageOffset = FwImgOffset + FwImgHeader.Length + sizeof(FwImgHeader);
            if (fseek(pFile, DtbImageOffset, AMBA_FS_SEEK_START) != OK) {
                AmbaPrint_PrintStr5Fwprog("Cannot seek dtb image header!\r\n", NULL, NULL, NULL, NULL, NULL);
                return -1;
            }
            NumSucce = fread(&FwImgHeader, 1, sizeof(FwImgHeader), pFile);
            if (NumSucce != sizeof(FwImgHeader)) {
                AmbaPrint_PrintStr5Fwprog("Cannot read dtb image header!\r\n", NULL, NULL, NULL, NULL, NULL);
                return -1;
            }
            j = pPartEntry->ByteCount / pPartEntry->BlkCount;
            Size = FwImgHeader.Length / j;
            Size += (FwImgHeader.Length % j) ? 1 : 0;
#if defined(CONFIG_ENABLE_EMMC_BOOT)
            Size += 128;
#else
            Size += 1;
#endif
            if (Size > pPartEntry->BlkCount) {
                AmbaPrint_PrintStr5Fwprog("Please reserve 1 block for Linux DTB! %s\r\n", (char *)pPartEntry->PartitionName, NULL, NULL, NULL, NULL);
                return -1;
            }
            NumSucce = fread(fw_buf, 1, FwImgHeader.Length, pFile);
            if (NumSucce != FwImgHeader.Length) {
                AmbaPrint_PrintStr5Fwprog("dtb image read fail!\r\n", NULL, NULL, NULL, NULL, NULL);
                return -1;
            }
            if ((fdt_magic(fw_buf) == FDT_MAGIC) && (fdt_version(fw_buf) >= 17)) {
                AMBA_NVM_SECTOR_CONFIG_s SecConfig;
                AmbaPrint_PrintStr5Fwprog("\r\nDTB found in firmware!\r\n", NULL, NULL, NULL, NULL, NULL);
                AmbaPrint_PrintStr5Fwprog("\rProgram DTB to the last block\r\n", NULL, NULL, NULL, NULL, NULL);
                SecConfig.NumSector   = (FwImgHeader.Length + BlockSize - 1U) / BlockSize;
                SecConfig.pDataBuf    = (UINT8 *)fw_buf;
#if defined(CONFIG_ENABLE_EMMC_BOOT)
                SecConfig.StartSector = pPartEntry->BlkCount - 128;
#else
                SecConfig.StartSector = pPartEntry->BlkCount - 1;
#endif
                if (AmbaNVM_WriteRawSector(NvmID, 1, BurnPart, &SecConfig, NULL, 5000) != OK) {
                    AmbaPrint_PrintStr5Fwprog("FwUpdater_WriteDtb Failed!\r\n", NULL, NULL, NULL, NULL, NULL);
                    return -1;
                }
            } else {
                AmbaPrint_PrintStr5Fwprog("\r\nDTB is not found in firmware!"
                                          "\r\nLinux can't be booted!\r\n\r\n", NULL, NULL, NULL, NULL, NULL);
                return -1;
            }
        }
    }

    return 0;
}

static int app_fwupdater_write_image(void *pFile, int is_full)
{
    if (is_full) {
        unsigned int app_offset = 0;
        if (update_boot_firmware(pFile, 4, &app_offset) < 0) {
            return -1;
        }
        //app_offset
        return update_app_firmware(pFile, app_offset, 1);
    }
    return update_app_firmware(pFile, 0, 0);
}

static int app_fwupdater_update(const char *path)
{
    void *pFile = NULL;
    int rval = 0;
    AMBA_PARTITION_ENTRY_s *pPartEntry = NULL;
#if defined(CONFIG_ENABLE_EMMC_BOOT)
    extern AMBA_USER_PARTITION_TABLE_s *_pAmbaRTSL_EmmcUserPartTable;
    AMBA_USER_PARTITION_TABLE_s *_pAmbaRTSL_UserPartTable = _pAmbaRTSL_EmmcUserPartTable;
#else
    extern AMBA_USER_PARTITION_TABLE_s *pAmbaNandTblUserPart;
    AMBA_USER_PARTITION_TABLE_s *_pAmbaRTSL_UserPartTable = pAmbaNandTblUserPart;
#endif
    unsigned char header[4] = {0};
    int is_full = 0;

    if (path == NULL) {
        AmbaPrint_PrintStr5Fwprog("invalid fw path\r\n", NULL, NULL, NULL, NULL, NULL);
        return -1;
    }

    pFile = fopen(path, "rb");
    if (pFile == NULL) {
        AmbaPrint_PrintStr5Fwprog("open %s failed\r\n", path, NULL, NULL, NULL, NULL);
        return -1;
    }

    if (fread(header, 1, sizeof(header), pFile) != sizeof(header)) {
        AmbaPrint_PrintStr5Fwprog("can't read header!\r\n", NULL, NULL, NULL, NULL, NULL);        
        fclose(pFile);
        return -1;
    }

    if (memcmp(header, "FULL", 4) == 0) {
        is_full = 1;
    }

    if (is_full == 0) {
        //set A to active first, to avoid update fail and watchdog reboot
        AmbaPrint_PrintStr5Fwprog("Set FW A To Active First\r\n", 0, 0, 0, 0, 0);
        pPartEntry = (AMBA_PARTITION_ENTRY_s *)&(_pAmbaRTSL_UserPartTable->Entry[AMBA_USER_PARTITION_SYS_SOFTWARE]);
        pPartEntry->Attribute |= FW_PARTITION_ACTIVE_FLAG;
        pPartEntry = (AMBA_PARTITION_ENTRY_s *)&(_pAmbaRTSL_UserPartTable->Entry[AMBA_USER_PARTITION_SYS_SOFTWARE_B]);
        pPartEntry->Attribute &= ~FW_PARTITION_ACTIVE_FLAG;
        _pAmbaRTSL_UserPartTable->ErrorCode = 0U;
        if (AmbaNVM_WriteUserPartitionTable(NVM_TYPE, NULL, 0, 5000U) != OK) {
            AmbaPrint_PrintStr5Fwprog("Unable to update ptb\r\n", 0, 0, 0, 0, 0);
            return -1;
        }
    }

    AmbaPrint_PrintStr5Fwprog("open %s ok\r\n", path, NULL, NULL, NULL, NULL);
    rval = app_fwupdater_write_image(pFile, is_full);
    fclose(pFile);

    if (is_full) {
        if (rval == 0) {
            pPartEntry = (AMBA_PARTITION_ENTRY_s *)&(_pAmbaRTSL_UserPartTable->Entry[AMBA_USER_PARTITION_SYS_SOFTWARE]);
            pPartEntry->Attribute |= FW_PARTITION_ACTIVE_FLAG;
            pPartEntry = (AMBA_PARTITION_ENTRY_s *)&(_pAmbaRTSL_UserPartTable->Entry[AMBA_USER_PARTITION_SYS_SOFTWARE_B]);
            pPartEntry->Attribute &= ~FW_PARTITION_ACTIVE_FLAG;
            AmbaPrint_PrintStr5Fwprog("FW A update ok, Set FW A To Active\r\n", 0, 0, 0, 0, 0);
        } else {
            pPartEntry = (AMBA_PARTITION_ENTRY_s *)&(_pAmbaRTSL_UserPartTable->Entry[AMBA_USER_PARTITION_SYS_SOFTWARE]);
            pPartEntry->Attribute &= ~FW_PARTITION_ACTIVE_FLAG;
            pPartEntry = (AMBA_PARTITION_ENTRY_s *)&(_pAmbaRTSL_UserPartTable->Entry[AMBA_USER_PARTITION_SYS_SOFTWARE_B]);
            pPartEntry->Attribute |= FW_PARTITION_ACTIVE_FLAG;
            AmbaPrint_PrintStr5Fwprog("FW A update fail, Set FW B To Active\r\n", 0, 0, 0, 0, 0);
        }
    } else {
        if (rval == 0) {
            pPartEntry = (AMBA_PARTITION_ENTRY_s *)&(_pAmbaRTSL_UserPartTable->Entry[AMBA_USER_PARTITION_SYS_SOFTWARE]);
            pPartEntry->Attribute &= ~FW_PARTITION_ACTIVE_FLAG;
            pPartEntry = (AMBA_PARTITION_ENTRY_s *)&(_pAmbaRTSL_UserPartTable->Entry[AMBA_USER_PARTITION_SYS_SOFTWARE_B]);
            pPartEntry->Attribute |= FW_PARTITION_ACTIVE_FLAG;
            AmbaPrint_PrintStr5Fwprog("FW B update ok, Set FW B To Active\r\n", 0, 0, 0, 0, 0);
        } else {
            pPartEntry = (AMBA_PARTITION_ENTRY_s *)&(_pAmbaRTSL_UserPartTable->Entry[AMBA_USER_PARTITION_SYS_SOFTWARE]);
            pPartEntry->Attribute |= FW_PARTITION_ACTIVE_FLAG;
            pPartEntry = (AMBA_PARTITION_ENTRY_s *)&(_pAmbaRTSL_UserPartTable->Entry[AMBA_USER_PARTITION_SYS_SOFTWARE_B]);
            pPartEntry->Attribute &= ~FW_PARTITION_ACTIVE_FLAG;
            AmbaPrint_PrintStr5Fwprog("FW B update fail, Set FW A To Active\r\n", 0, 0, 0, 0, 0);
        }
    }
    _pAmbaRTSL_UserPartTable->ErrorCode = 0U;
    if (AmbaNVM_WriteUserPartitionTable(NVM_TYPE, NULL, 0, 5000U) != OK) {
        AmbaPrint_PrintStr5Fwprog("Unable to update ptb\r\n", 0, 0, 0, 0, 0);
        return -1;
    }

    return rval;
}

static int fwupdater_do_hard_reset(void)
{
#define FILE_MAX_LINE_NUM (64)
#define FILE_MAX_LINE_SIZE (128)
    char lines[FILE_MAX_LINE_NUM][FILE_MAX_LINE_SIZE] = {0};
    int line = 0;
    FILE *pFile = NULL;

    pFile = fopen(USER_SETTING_DEVINFO_PATH, "r");
    if (pFile) {
        while(line < FILE_MAX_LINE_NUM) {
            memset(lines[line], 0, FILE_MAX_LINE_SIZE);
            if (readline(pFile, lines[line]) < 0) {
                break;
            }
            line += 1;
        }
        fclose(pFile);
    }
    debug_line("clean %s", USER_SETTING_PREF_DIR);
    clean_dir(USER_SETTING_PREF_DIR);
    if (line > 0) {
        pFile = fopen(USER_SETTING_DEVINFO_PATH, "w");
        if (pFile) {
            int index = 0;
            while (index < line) {
                if (strncmp(lines[index], "first_access=", strlen("first_access=")) == 0) {
                    memset(lines[index], 0, FILE_MAX_LINE_SIZE);
                    snprintf(lines[index], FILE_MAX_LINE_SIZE - 1, "%s", "first_access=1");
                }
                fwrite(lines[index], strlen(lines[index]), 1, pFile);
                fwrite("\n", 1, 1, pFile);
                index += 1;
            }
            fflush(pFile);
            fclose(pFile);
        }
    }

    return 0;
}

static void fwupdater_wait_sd_ready(void)
{
    int timeout = 100;

    AmbaPrint_PrintStr5Fwprog("checking "SD_SLOT" disk...\r\n", NULL, NULL, NULL, NULL, NULL);
    while (1) {
        if (AmbaSD_IsCardPresent(SD_CHANNEL) == 0) {
            AmbaPrint_PrintStr5Fwprog("no sd card detected\r\n", NULL, NULL, NULL, NULL, NULL);
            break;
        }
        if (AmbaFS_ChangeDir(SD_SLOT":\\") == AMBA_FS_ERR_NONE) {
            AmbaPrint_PrintStr5Fwprog("sd card is ready\r\n", NULL, NULL, NULL, NULL, NULL);
            break;
        }
        timeout--;
        if (timeout < 0) {
            AmbaPrint_PrintStr5Fwprog("can not detect sd card, ignore\r\n", NULL, NULL, NULL, NULL, NULL);
            break;
        }
        msleep(50);
    }
}

static void led_stage(int stage)
{
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    int i = 0;

    if (stage == 0) {
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x00, 0x55);//reset led
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x03, 0x15);//disable charge led
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x01, 0x01);//turn on led
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x30, 0x07);//enable 3 leds
        //Purple Continuous breathing
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x4a, 0x04);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x31, 0x11);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x32, 0x11);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x33, 0x11);
        //set color
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x34, 0xff);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x35, 0x00);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x36, 0xff);
    } else if (stage == 1) {
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x31, 0x01);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x32, 0x01);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x33, 0x01);
        //set color
        for (i = 0; i < 2; i++) {
            i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x34, 0xff);
            i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x35, 0x00);
            i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x36, 0xff);
            msleep(1000);
            i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x34, 0x00);
            i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x35, 0x00);
            i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x36, 0x00);
            msleep(1000);
        }
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x00, 0x55);//reset led
    } else if (stage == 2) {
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x31, 0x01);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x32, 0x01);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x33, 0x01);
        for (i = 0; i < 15; i++) {
            i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x34, 0xff);
            i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x35, 0x00);
            i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x36, 0xff);
            msleep(100);
            i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x34, 0x00);
            i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x35, 0x00);
            i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x36, 0x00);
            msleep(100);
        }
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x00, 0x55);//reset led
    }
#endif
}

struct _fw_path_s_ {
    char path[32];
} fw_paths[] = {
    {"x:\\"SD_FWUPDATE_NAME},
};
typedef struct _fw_path_s_ fw_path_s;
static int is_sd_update = 0;
static void *fwupdater_task_entry(void *Argv)
{
    struct stat fstat;
    unsigned int is_update_success = 0;
    boot_reason_e BootReason = 0;
    unsigned int i = 0;

    BootReason = Pmic_GetBootReason();
    if (BootReason == BOOT_REASON_HARD_RESET_TRIGGER) {
        debug_line("hard reset");
        fwupdater_do_hard_reset();
        Pmic_SetSramRegister(0x01);
    } else if (BootReason == BOOT_REASON_FACTORY_RESET_TRIGGER) {
        debug_line("factory reset");
        debug_line("format a");
        AmbaFS_Format('a');
        if (AmbaSD_IsCardPresent(SD_CHANNEL)) {
            fwupdater_wait_sd_ready();
            debug_line("format c");
            AmbaFS_Format(SD_SLOT[0]);
        }
        debug_line("set boot from part A");
        AmbaNAND_SwitchBoot(1);
        Pmic_SetSramRegister(0x02);
        Pmic_NormalSoftReset();
        return NULL;
    }
    led_stage(0);
    fwupdater_wait_sd_ready();
    AmbaPrint_PrintStr5Fwprog("FirmwareUpdater begin\r\n", NULL, NULL, NULL, NULL, NULL);
    for (i = 0; i < sizeof(fw_paths) / sizeof(fw_path_s); i++) {
        fw_paths[i].path[0] = SD_SLOT[0];
        if (0U == stat(fw_paths[i].path, &fstat)) {
            AmbaPrint_PrintStr5Fwprog("found %s\r\n", fw_paths[i].path, NULL, NULL, NULL, NULL);
            if (fstat.size > 0) {
                char new_path[128] = {0};

                is_sd_update = 1;
                memset(new_path, 0, sizeof(new_path));
                snprintf(new_path, sizeof(new_path) - 1, "%s.bak", fw_paths[i].path);
                AmbaFS_Rename(fw_paths[i].path, new_path);
                if (app_fwupdater_update(new_path) < 0) {
                    AmbaPrint_PrintStr5Fwprog("%s: fwupdate failed\r\n", fw_paths[i].path, NULL, NULL, NULL, NULL);
                } else {
                    is_update_success = 1;
                }
                remove(new_path);
            } else {
                AmbaPrint_PrintStr5Fwprog("%s: file is empty\r\n", fw_paths[i].path, NULL, NULL, NULL, NULL);
                remove(fw_paths[i].path);
            }
        }
    }

    led_stage(is_update_success ? 1 : 2);
    AmbaPrint_PrintStr5Fwprog("FirmwareUpdater finish: %s\r\n", is_update_success ? "success": "fail", NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    msleep(200);
    Pmic_NormalSoftReset();

    return NULL;
}

#if 0
static int led_onoff(int on)
{
    if (on) {
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x34, 10);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x35, 0);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x36, 0);
    } else {
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x34, 0);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x35, 0);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x36, 0);
    }

    return 0;
}
#endif

#if 1
static void *monitor_task_entry(void *Argv)
{
#if 0
    int cnt = 0, flag = 0;
    i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x00, 0x55);//reset led
    i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x01, 0x01);//turn on led
    i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x30, 0x07);//enable 3 leds
    //set brightness
    i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x31, 1);
    i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x32, 1);
    i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x33, 1);
#endif
    while (1) {
#if 0
        cnt += 1;
        if ((cnt % 2) == 0) {
            cnt = 0;
            flag = !flag;
            led_onoff(flag);
        }
        if (Pmic_CheckUsbConnected() == 0) {
            AmbaPrint_PrintStr5Fwprog("detect usb removed. power off\r\n", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            Pmic_DoPowerOff(0);
        }
        if (is_sd_update && AmbaSD_IsCardPresent(SD_CHANNEL) == 0) {
            AmbaPrint_PrintStr5Fwprog("detect sd card removed. power off\r\n", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            Pmic_DoPowerOff(0);
        }
        msleep(500);
#else
        //Pmic_Read(0x00, NULL);
        sleep(1);
#endif
    }

    return NULL;
}
#endif

static UINT8 FwUpdaterStack[1024 * 1024];
static AMBA_KAL_TASK_t FwUpdaterTask;
#if 1
static UINT8 MonitorStack[8 * 1024];
static AMBA_KAL_TASK_t MonitorTask;
#endif
int AmbaFwUpdaterMain(void)
{
    UINT32 ret = 0;

#if 1
    /* Create monitor task */
    ret = AmbaKAL_TaskCreate(&MonitorTask,
                              "MonitorTask",
                              55,
                              monitor_task_entry,
                              (unsigned int)0x0,
                              MonitorStack,
                              sizeof(MonitorStack),
                              AMBA_KAL_AUTO_START);
    if (ret != 0) {
        AmbaPrint_PrintStr5Fwprog("MonitorTask create fail\r\n", NULL, NULL, NULL, NULL, NULL);
        return -1;
    }
#endif
    /* Create fwupdate task */
    ret = AmbaKAL_TaskCreate(&FwUpdaterTask,
                              "FwUpdaterTask",
                              55,
                              fwupdater_task_entry,
                              (unsigned int)0x0,
                              FwUpdaterStack,
                              sizeof(FwUpdaterStack),
                              AMBA_KAL_AUTO_START);
    if (ret != 0) {
        AmbaPrint_PrintStr5Fwprog("FwUpdaterTask create fail\r\n", NULL, NULL, NULL, NULL, NULL);
        return -1;
    }
    return 0;
}

