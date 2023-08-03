/**
 * @file AmbaLink_Dtb.c
 *
 * Copyright (c) 2020 Ambarella International LP
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
 * @details ATAG related structure and function are implemented here.
 *
 */
#include "AmbaUtility.h"
#include "AmbaIntrinsics.h"
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaINT.h"
#include "AmbaLink.h"
#include "AmbaLinkPrivate.h"
#include "AmbaPrint.h"
#include "AmbaCache.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaNAND.h"
#include "AmbaNAND_OP.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaSD.h"
#include "AmbaSD_STD.h"
#include "AmbaSD_Ctrl.h"
#include "AmbaSpiNOR_Ctrl.h"
#include "AmbaRTC.h"
#include "libfdt.h"
#include "AmbaSYS.h"
#include "AmbaMMU.h"

#define COMMAND_LINE_SIZE   1024

static UINT8 CommandLineBuf[COMMAND_LINE_SIZE];
extern AMBA_LINK_CTRL_s AmbaLinkCtrl;
extern void *__linux_start, *__linux_end;
extern void *__ddr_end;
extern void *__ddr_region0_ro_start;
extern int AmbaLink_ReadPartitionInfo(int, UINT32, AMBA_PARTITION_ENTRY_s *);

static int SetupDTB(UINT8 *pFdt)
{
    int Rval = 0;
    UINT32 Len;
    UINT32 Block, Pages, PhysAddr;
    UINT8 *pBuf = NULL;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;
    AMBA_PARTITION_ENTRY_s LnxPartInfo;
    LinuxMemoryInfo MemInfo;
    UINT32 BootMode;
    AMBA_NORSPI_DEV_INFO_s *pNorDevInfo;
    AmbaSYS_GetBootMode(&BootMode);

    Len = AmbaLinkCtrl.AmbarellaParamsSize;

    pBuf = &(CommandLineBuf[0]);
    AmbaLink_ReadPartitionInfo(1, AMBA_USER_PARTITION_LINUX_KERNEL, &LnxPartInfo);

    if (BootMode == AMBA_SYS_BOOT_FROM_NAND) {
        UINT32 PageAddr;
        AMBA_NAND_COMMON_INFO_s *pAmbaNAND_DevInfo = AmbaNAND_GetCommonInfo();

        if (Len % pAmbaNAND_DevInfo->MainByteSize) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "pAmbaNAND_DevInfo->MainByteSize 0x%x", pAmbaNAND_DevInfo->MainByteSize, 0U, 0U, 0U, 0U);
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: memory for dt is not aligned to nand page size!", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            Rval = -1;
            goto Done;
        }

        Pages = Len / pAmbaNAND_DevInfo->MainByteSize;

        /* DTB is programmed to the last good block of Linux partition. */
        Block = LnxPartInfo.StartBlkAddr + LnxPartInfo.BlkCount - 1;

        Block = AmbaNAND_FindGoodBlkBackward(Block);

        PageAddr = (Block * pAmbaNAND_DevInfo->BlockPageSize);
        Rval = AmbaNAND_Read(PageAddr, Pages, pFdt, NULL, 5000);
        if (Rval < 0) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNAND_Read failed. <Block: %d>", Block, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            goto Done;
        }
    }
    else if (BootMode == AMBA_SYS_BOOT_FROM_SPI_NAND) {
        UINT32 PageAddr;
        AMBA_NAND_COMMON_INFO_s *pAmbaNAND_DevInfo = AmbaNAND_GetCommonInfo();
        if (Len % pAmbaNAND_DevInfo->MainByteSize) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: memory for dt is not aligned to nand page size!", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            Rval = -1;
            goto Done;
        }

        Pages = Len / pAmbaNAND_DevInfo->MainByteSize;

        /* DTB is programmed to the last good block of Linux partition. */
        Block = LnxPartInfo.StartBlkAddr + LnxPartInfo.BlkCount - 1;

        Block = AmbaNAND_FindGoodBlkBackward(Block);

        PageAddr = (Block * pAmbaNAND_DevInfo->BlockPageSize);
        Rval = AmbaNandOp_Read(PageAddr, Pages, pFdt, NULL, 5000);
        if (Rval < 0) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNAND_Read failed. <Block: %d>", Block, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            goto Done;
        }
    }
    else if (BootMode == AMBA_SYS_BOOT_FROM_EMMC) {
        Pages = Len / 512;

        /* DTB is programmed to the last 128 sectors of Linux partition. */
        Block = LnxPartInfo.StartBlkAddr + LnxPartInfo.BlkCount - 128;

        SecConfig.NumSector   = 128;
        SecConfig.StartSector = Block;
        SecConfig.pDataBuf    = (void *)pFdt;
        Rval = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
        if (Rval < 0) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaEMMC_MediaPartRead failed. <Sector: %d>", Block, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            goto Done;
        }
    }
    else if (BootMode == AMBA_SYS_BOOT_FROM_SPI_NOR) {
        #if defined(CONFIG_MUTI_BOOT_DEVICE)
        if(((LnxPartInfo.Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) == AMBA_NVM_SPI_NOR) {
            AmbaPrint_PrintStr5("%s: dual boot, read from spinor", __func__, NULL, NULL, NULL, NULL);
            pNorDevInfo = AmbaSpiNOR_GetDevInfo();

            /* DTB is programmed to the last block of Linux partition. */
            Block  = LnxPartInfo.StartBlkAddr + LnxPartInfo.BlkCount - 1;
            Block *= pNorDevInfo->EraseBlockSize;

            Rval = AmbaSpiNOR_Readbyte(Block, pNorDevInfo->EraseBlockSize, pFdt, 5000);
            if (Rval < 0) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSpiNOR_Readbyte failed. <Block: %d>", Block, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
                goto Done;
            }
                
        } else if(((LnxPartInfo.Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) == AMBA_NVM_eMMC) {
            AmbaPrint_PrintStr5("%s: dual boot, read from emmc", __func__, NULL, NULL, NULL, NULL);
            Pages = Len / 512;

            /* DTB is programmed to the last 128 sectors of Linux partition. */
            Block = LnxPartInfo.StartBlkAddr + LnxPartInfo.BlkCount - 128;

            SecConfig.NumSector   = 128;
            SecConfig.StartSector = Block;
            SecConfig.pDataBuf    = (void *)pFdt;
            Rval = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
            if (Rval < 0) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaEMMC_MediaPartRead failed. <Sector: %d>", Block, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
                goto Done;
            }

        } else {
            AmbaPrint_PrintUInt5("# Kernel image load failed: Storage does not support, attr 0x%X\r\n#\r\n", LnxPartInfo.Attribute, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }

        #else
        pNorDevInfo = AmbaSpiNOR_GetDevInfo();

        /* DTB is programmed to the last block of Linux partition. */
        Block  = LnxPartInfo.StartBlkAddr + LnxPartInfo.BlkCount - 1;
        Block *= pNorDevInfo->EraseBlockSize;

        Rval = AmbaSpiNOR_Readbyte(Block, pNorDevInfo->EraseBlockSize, pFdt, 5000);
        if (Rval < 0) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSpiNOR_Readbyte failed. <Block: %d>", Block, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            goto Done;
        }
        #endif
    } else {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "unsupported boottype (%d)!", BootMode, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        Rval = -1;
        goto Done;
    }

    if (fdt_magic(pFdt) != FDT_MAGIC) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: FDT is invalid! FDT_MAGIC check failed", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
        goto Done;
    }

    if (fdt_version(pFdt) < 17) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: FDT is invalid! fdt_version < 17", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
        goto Done;
    }

    if (BootMode == AMBA_SYS_BOOT_FROM_NAND) {
        /* Update the NAND partition info. */
        Rval = fdtput_nand((void *) pFdt);
        if (Rval < 0) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: fdtput_nand failed. ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            goto Done;
        }
    }
    else if (BootMode == AMBA_SYS_BOOT_FROM_SPI_NAND) {
        /* Update the NAND partition info. */
        Rval = fdtput_nand((void *) pFdt);
        if (Rval < 0) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: fdtput_nand failed. ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            goto Done;
        }
    }

    else if (BootMode == AMBA_SYS_BOOT_FROM_EMMC) {
        /* Update the eMMC partition info. */
        Rval = fdtput_emmc((void *) pFdt);
        if (Rval < 0) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: fdtput_emmc failed. ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            goto Done;
        }

    } else if (BootMode == AMBA_SYS_BOOT_FROM_SPI_NOR) {
        #if defined(CONFIG_MUTI_BOOT_DEVICE)
        if(((LnxPartInfo.Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) == AMBA_NVM_SPI_NOR) {
            /* Update the SPI NOR partition info. */
            Rval = fdtput_spinor((void *) pFdt);
            if (Rval < 0) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: [dual spinor] fdtput_spinor failed. ", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_Flush();
                goto Done;
            }
            
        } else if(((LnxPartInfo.Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) == AMBA_NVM_eMMC) {
            Rval = fdtput_emmc((void *) pFdt);
            if (Rval < 0) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: [dual emmc] fdtput_emmc failed. ", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_Flush();
                goto Done;
            }

        } else {
            AmbaPrint_PrintUInt5("# fdtput_emmc failed: Storage does not support, attr 0x%X\r\n#\r\n", LnxPartInfo.Attribute, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
        #else
        /* Update the SPI NOR partition info. */
        Rval = fdtput_spinor((void *) pFdt);
        if (Rval < 0) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: fdtput_spinor failed. ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            goto Done;
        }
        #endif
    } else {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "unsupported boottype (%d)!", BootMode, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        Rval = -1;
        goto Done;
    }

    /* Update the kernel command line if needed. */
    memset(pBuf, 0x0, COMMAND_LINE_SIZE);
    Len = AmbaUtility_StringLength(AmbaLinkCtrl.pKernelCmdLine);
    AmbaUtility_StringCopy(pBuf, Len+1, AmbaLinkCtrl.pKernelCmdLine);
    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "AmbaLink: cmdline: %s", pBuf, NULL, NULL, NULL, NULL);
    //AmbaPrint_Flush();

    Rval = fdtput_cmdline((void *) pFdt, (const char *) pBuf);
    if (Rval < 0) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: fdtput_cmdline failed. ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        goto Done;
    }

    /* Always send RTOS memory info as PPM2. */
    Len = (UINT32)((ULONG)(&__linux_start) - (ULONG)(&__ddr_region0_ro_start));

    MemInfo.KernelBase  = (UINT64) AmbaLinkCtrl.AmbarellaPPMSize;
    MemInfo.KernelSize  = (UINT64) AmbaLinkCtrl.AmbaLinkMemSize;
    MemInfo.ShmBase     = (UINT64) AmbaLinkCtrl.AmbaLinkSharedMemAddr;
    MemInfo.ShmSize     = (UINT64) (AmbaLinkCtrl.AmbarellaPPMSize -
                                    AmbaLinkCtrl.AmbaLinkSharedMemAddr);
    AmbaMMU_VirtToPhys((ULONG) &__ddr_region0_ro_start, (ULONG *)&PhysAddr);
    MemInfo.Ppm2Base    = (UINT64) ((ULONG) PhysAddr);
    MemInfo.Ppm2Size    = (UINT64) Len;

    Rval = fdtput_memory((void *) pFdt, &MemInfo);
    if (Rval < 0) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: fdtput_memory failed. ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        goto Done;
    }

    Rval = fdtput_cvshm((void *) pFdt);
    if (Rval < 0) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: fdtput_cvshm failed. ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        goto Done;
    }

    AmbaCache_DataClean((ULONG) pFdt, 0x10000);

Done:
    return Rval;
}

void AmbaLink_DtbInit(void)
{
    UINT32 dtb_data = 0;

    AmbaMMU_PhysToVirt((ULONG)AmbaLinkCtrl.AmbarellaParamsPhys, (ULONG *)&dtb_data);

    if (dtb_data == 0x0) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: dtb_data == 0x0 ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return;
    }

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaLink: dtb    = 0x%08x", dtb_data, 0U, 0U, 0U, 0U);
    //AmbaPrint_Flush();
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaLink: mem    = 0x%08x, 0x%08x", AmbaLinkCtrl.AmbarellaParamsPhys, AmbaLinkCtrl.AmbarellaParamsSize, 0U, 0U, 0U);
    //AmbaPrint_Flush();

    SetupDTB((UINT8*)(ULONG) dtb_data);
}
