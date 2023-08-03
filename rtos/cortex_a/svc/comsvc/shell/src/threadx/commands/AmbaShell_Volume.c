/**
 *  @file AmbaShell_Volume.c
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Shell command for showing the storage volume information.
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaMisraFix.h>
#include <AmbaShell.h>
#include <AmbaFS.h>
#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"

static void SHELL_ShowVolumeInfo(char DriveLetter, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;
    AMBA_FS_VOL_INFO_t VolInfo;
    AMBA_FS_DRIVE_INFO_t DriveInfo;
    char VolLabel[VOLUME_LABEL_BUF_SIZE+1U];
    UINT64 SizeTotal, SizeUsed, SizeFree;

    // init VolName string buffer
    AmbaUtility_MemorySetChar(VolInfo.VolName, '\0', AMBA_FS_VOLUME_LABEL_BUF_SIZE);
    RetVal = AmbaFS_GetVolumeLabel(DriveLetter, &VolInfo);
    if (RetVal != 0U) {
        AmbaUtility_MemorySetChar(VolInfo.VolName, '\0', AMBA_FS_VOLUME_LABEL_BUF_SIZE);
    }

    RetVal = AmbaFS_GetDriveInfo(DriveLetter, &DriveInfo);
    if (RetVal != 0U) {
        PrintFunc("Cannot get drive information for drive ");
        AmbaShell_PrintChar(DriveLetter, PrintFunc);
        PrintFunc("\n");
    } else {
        AmbaUtility_MemorySetChar(VolLabel, '\0', (UINT32)sizeof(VolLabel));
        AmbaShell_StringCopyLength(VolLabel, VOLUME_LABEL_BUF_SIZE+1U, VolInfo.VolName, VOLUME_LABEL_BUF_SIZE);

        SizeTotal = (UINT64)DriveInfo.TotalClusters * DriveInfo.SectorsPerCluster * DriveInfo.BytesPerSector;
        SizeUsed =  ((UINT64)DriveInfo.TotalClusters - DriveInfo.EmptyClusters) * DriveInfo.SectorsPerCluster * DriveInfo.BytesPerSector;
        SizeFree =  (UINT64)DriveInfo.EmptyClusters * DriveInfo.SectorsPerCluster * DriveInfo.BytesPerSector;

        PrintFunc(" Volume in drive ");
        AmbaShell_PrintChar(DriveLetter, PrintFunc);
        PrintFunc(": is ");
        PrintFunc(VolLabel);
        PrintFunc(", FAT Type is ");
        PrintFunc(
            (DriveInfo.FormatType == (ULONG)PF_FMT_EXFAT) ? "exFAT" :
            (DriveInfo.FormatType == (ULONG)PF_FMT_FAT32) ? "FAT32" :
            (DriveInfo.FormatType == (ULONG)PF_FMT_FAT16) ? "FAT16" :
            (DriveInfo.FormatType == (ULONG)PF_FMT_FAT12) ? "FAT12" :
            (DriveInfo.FormatType == (ULONG)PF_FMT_FAT12) ? "FAT12" : "Unknow");
        PrintFunc("\n");

        PrintFunc(" Drive size: ");
        AmbaShell_PrintUInt64(SizeTotal, PrintFunc);
        PrintFunc(" total, ");
        AmbaShell_PrintUInt64(SizeUsed, PrintFunc);
        PrintFunc(" used, ");
        AmbaShell_PrintUInt64(SizeFree, PrintFunc);
        PrintFunc(" free\n");

        PrintFunc(" Number of clusters: ");
        AmbaShell_PrintUInt32(DriveInfo.TotalClusters, PrintFunc);
        PrintFunc(" total, ");
        AmbaShell_PrintUInt32(DriveInfo.EmptyClusters, PrintFunc);
        PrintFunc(" empty\n");

        PrintFunc(" Number of cluster groups: ");
        AmbaShell_PrintUInt32(DriveInfo.EmptyClusterGroups, PrintFunc);
        PrintFunc(" empty\n");

        PrintFunc(" Clusters Per Group: ");
        AmbaShell_PrintUInt32(DriveInfo.ClustersPerGroup, PrintFunc);
        PrintFunc(", Sectors Per Cluster: ");
        AmbaShell_PrintUInt32(DriveInfo.SectorsPerCluster, PrintFunc);
        PrintFunc(", Bytes Per Sector: ");
        AmbaShell_PrintUInt32(DriveInfo.BytesPerSector, PrintFunc);
        PrintFunc("\n");
    }

    return;
}

static void SHELL_SetVolumeInfo(char DriveLetter, const char *pVolLabel, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;

    RetVal = AmbaFS_SetVolumeLabel(DriveLetter, pVolLabel);
    if (RetVal != 0U) {
        PrintFunc("Failed to set volume name of drive ");
        AmbaShell_PrintChar(DriveLetter, PrintFunc);
        PrintFunc(": (ErrorCode: ");
        AmbaShell_PrintUInt32(AmbaFS_GetError(), PrintFunc);
        PrintFunc("\n");
    } else {
        PrintFunc("Volume name of drive ");
        AmbaShell_PrintChar(DriveLetter, PrintFunc);
        PrintFunc(": is changed.\n");
    }

    return;
}

static inline void SHELL_VolCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [drive:] [label]\n");
}

/**
 *  AmbaShell_CommandVersion - execute command volumn
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandVolumn(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    char DriveLetter;

    if (ArgCount < 2U) {
        SHELL_VolCmdUsage(pArgVector, PrintFunc);
    } else {
        DriveLetter = (char)pArgVector[1][0];

        if (ArgCount == 2U) {
            SHELL_ShowVolumeInfo(DriveLetter, PrintFunc);
        } else {
            SHELL_SetVolumeInfo(DriveLetter, pArgVector[2], PrintFunc);
        }
    }

    return;
}
