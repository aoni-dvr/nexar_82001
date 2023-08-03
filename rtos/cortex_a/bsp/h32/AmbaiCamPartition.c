/**
 *  @file AmbaiCamPartition.c
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
 *  @details User-defined System and User Partition
 *
 */

#include "AmbaTypes.h"
#include "AmbaNAND_Def.h"
#include "AmbaSpiNOR_Def.h"


/*-----------------------------------------------------------------------------------------------*\
 * System Partition configurations
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_PARTITION_CONFIG_s AmbaNAND_SysPartConfig[AMBA_NUM_SYS_PARTITION];
const AMBA_PARTITION_CONFIG_s AmbaNAND_SysPartConfig[AMBA_NUM_SYS_PARTITION] = {
    /* Bootstrap: the size is Ambarella chip dependent. It can not be modified. */
    [AMBA_SYS_PARTITION_BOOTSTRAP]              = {"BOOTSTRAP",       0x01, AMBA_NAND_BOOTSTRAP_CODE_SPACE_SIZE},
    [AMBA_SYS_PARTITION_BOOTLOADER]             = {"BOOTLOADER",      0x01, 256 * 1024},        /* Bootloader */
    [AMBA_SYS_PARTITION_ARM_TRUST_FW]           = {"ARM_TRUST_FW", 0x01, 128 * 1024},         /* ATF */

    [AMBA_SYS_PARTITION_FW_UPDATER]             = {"FW_UPDATER",   0x01, 2 * 1024 * 1024},    /* Firmware Updater */
};

/*-----------------------------------------------------------------------------------------------*\
 * User Partition configurations
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_PARTITION_CONFIG_s AmbaNAND_UserPartConfig[AMBA_NUM_USER_PARTITION];
const AMBA_PARTITION_CONFIG_s AmbaNAND_UserPartConfig[AMBA_NUM_USER_PARTITION] = {
    [AMBA_USER_PARTITION_SYS_SOFTWARE]          = {"SYS_SW",          0x29,  15 * 1024 * 1024},  /* System Software */
    [AMBA_USER_PARTITION_DSP_uCODE]             = {"SECURE",          0x21,  5 * 1024 * 1024},  /* DSP uCode (ROM Region) */
    [AMBA_USER_PARTITION_SYS_DATA]              = {"SYS_DATA",        0x21, 15 * 1024 * 1024},  /* System Data (ROM Region) */
#if defined(CONFIG_AMBALINK_BOOT_OS)
    [AMBA_USER_PARTITION_LINUX_KERNEL]          = {"LINUX_Kernel",    0x29, 15 * 1024 * 1024},  /* Linux Kernel */
    [AMBA_USER_PARTITION_LINUX_ROOT_FS]         = {"LINUX_RFS",       0x20, 25 * 1024 * 1024},  /* Linux Root File System */
    [AMBA_USER_PARTITION_LINUX_HIBERNATION_IMG] = {"LINUX_HIBER_IMG", 0x20,                0},  /* Linux Hibernation Image */
#endif
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    [AMBA_USER_PARTITION_SYS_SOFTWARE_B]        = {"SYS_SW_B",        0x29, 15 * 1024 * 1024},
    [AMBA_USER_PARTITION_DSP_uCODE_B]           = {"SECURE_B",        0x21, 5 * 1024 * 1024},   /* DSP uCode (ROM Region) */
    [AMBA_USER_PARTITION_SYS_DATA_B]            = {"SYS_DATA_B",      0x21, 15 * 1024 * 1024},  /* System Data (ROM Region) */
    [AMBA_USER_PARTITION_LINUX_KERNEL_B]        = {"LINUX_Kernel_B",  0x29, 15 * 1024 * 1024},  /* Linux Kernel */
    [AMBA_USER_PARTITION_LINUX_ROOT_FS_B]       = {"LINUX_RFS_B",     0x20, 25 * 1024 * 1024},  /* Linux Root File System */
#endif
    [AMBA_USER_PARTITION_XTB]                   = {"XTB",             0x21,       128 * 1024},  /* XTB */
    [AMBA_USER_PARTITION_CALIBRATION_DATA]      = {"CALIB",           0x10,  2 * 1024 * 1024},  /* Calibration Data */
    [AMBA_USER_PARTITION_USER_SETTING]          = {"USER_SETTING",    0x10,       128 * 1024},  /* User Settings */

    [AMBA_USER_PARTITION_VIDEO_REC_INDEX]       = {"", 0, 0},       /* Video Recording Index*/
    [AMBA_USER_PARTITION_FAT_DRIVE_A]           = {"DRIVE_A",       0x10, 10 * 1024 * 1024},       /* Internal Storage FAT Drive 'A': need to assign the size */
    [AMBA_USER_PARTITION_FAT_DRIVE_B]           = {"DRIVE_B",       0x10, 10 * 1024 * 1024},       /* Internal Storage FAT Drive 'B' */

#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
#if defined(CONFIG_ICAM_PROJECT_SHMOO)
    [AMBA_USER_PARTITION_RESERVED0]             = {"AdvShmoo",        0x10, 128 * 1024},         /* Advance Shmoo Data */
#else
    [AMBA_USER_PARTITION_RESERVED0]             = {"", 0, 0},       /* Reserved User Partition-0 */
#endif
    [AMBA_USER_PARTITION_RESERVED1]             = {"", 0, 0},       /* Reserved User Partition-1 */
    [AMBA_USER_PARTITION_RESERVED2]             = {"", 0, 0},       /* Reserved User Partition-2 */
    [AMBA_USER_PARTITION_RESERVED3]             = {"", 0, 0},       /* Reserved User Partition-3 */
    [AMBA_USER_PARTITION_RESERVED4]             = {"", 0, 0},       /* Reserved User Partition-4 */
#endif
};

#if 0
/*-----------------------------------------------------------------------------------------------*\
 * System Partition configurations
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_PARTITION_CONFIG_s AmbaNORSPI_SysPartConfig[AMBA_NUM_SYS_PARTITION];
const AMBA_PARTITION_CONFIG_s AmbaNORSPI_SysPartConfig[AMBA_NUM_SYS_PARTITION] = {
    /* Bootstrap: the size is Ambarella chip dependent. It can not be modified. */
    [AMBA_SYS_PARTITION_BOOTSTRAP]              = {"BOOTSTRAP",       0x01, AMBA_NAND_BOOTSTRAP_CODE_SPACE_SIZE},
    [AMBA_SYS_PARTITION_BOOTLOADER]             = {"BOOTLOADER",      0x01,       128 * 1024},   /* Bootloader */
    [AMBA_SYS_PARTITION_ARM_TRUST_FW]           = {"ARM_TRUST_FW", 0x01, 128 * 1024},         /* ATF */

    [AMBA_SYS_PARTITION_FW_UPDATER]             = {"", 0, 0},                                    /* Firmware Updater */
};

/*-----------------------------------------------------------------------------------------------*\
 * User Partition configurations
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_PARTITION_CONFIG_s AmbaNORSPI_UserPartConfig[AMBA_NUM_USER_PARTITION];
const AMBA_PARTITION_CONFIG_s AmbaNORSPI_UserPartConfig[AMBA_NUM_USER_PARTITION] = {
    [AMBA_USER_PARTITION_SYS_SOFTWARE]          = {"SYS_SW",          0x6d,  9 * 1024 * 1024},  /* System Software */
    [AMBA_USER_PARTITION_DSP_uCODE]             = {"SECURE",          0x6d,  4 * 1024 * 1024},  /* DSP uCode (ROM Region) */
    [AMBA_USER_PARTITION_SYS_DATA]              = {"SYS_DATA",        0x6d, 16 * 1024 * 1024},  /* System Data (ROM Region) */
#if defined(CONFIG_AMBALINK_BOOT_OS)
    [AMBA_USER_PARTITION_LINUX_KERNEL]          = {"LINUX_Kernel",    0x49,                0},  /* Linux Kernel */
    [AMBA_USER_PARTITION_LINUX_ROOT_FS]         = {"LINUX_RFS",       0x40,                0},  /* Linux Root File System */
    [AMBA_USER_PARTITION_LINUX_HIBERNATION_IMG] = {"LINUX_HIBER_IMG", 0x40,                0},  /* Linux Hibernation Image */
#endif
    [AMBA_USER_PARTITION_XTB]                   = {"XTB",             0x61,       128 * 1024},  /* XTB */
    [AMBA_USER_PARTITION_CALIBRATION_DATA]      = {"CALIB",           0x90,  2 * 1024 * 1024},  /* Calibration Data */
    [AMBA_USER_PARTITION_USER_SETTING]          = {"USER_SETTING",    0x90,       128 * 1024},  /* User Settings */

    /* Internal Storage FAT Drive 'A': need to assign the size */
    [AMBA_USER_PARTITION_FAT_DRIVE_A]           = {"DRIVE_A",         0x10, 0},
    /* Internal Storage FAT Drive 'B' */
    [AMBA_USER_PARTITION_FAT_DRIVE_B]           = {"DRIVE_B",         0x10, 0},
#if defined(CONFIG_ICAM_PROJECT_SHMOO)
    [AMBA_USER_PARTITION_RESERVED0]             = {"AdvShmoo",        0x10, 128 * 1024},         /* Advance Shmoo Data */
#else
    [AMBA_USER_PARTITION_RESERVED0]             = {"", 0, 0},       /* Reserved User Partition */
#endif
};
#endif
