/**
 *  @file AmbaNAND_FTL.c
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
 *  @details NAND Flash Translation Layer (NFTL) APIs
 *
 */

#include <stdio.h>

#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"

#include "AmbaNAND.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaNAND_FTL.h"

//#define DEBUG_NFTL                  (1U)

#if defined(DEBUG_NFTL)
#define DBG_NFTL_PRN                printf
#else
#define DBG_NFTL_PRN(...)
#endif

#define AMBA_PART_CALIB_PATH        "/tmp/AmbaCalib/calib.bin"
#define AMBA_PART_PREF_PATH         "/tmp/AmbaPref/pref.bin"
#define AMBA_CALIB_MNAME            "AmbaCalib"
#define AMBA_PREF_MNAME             "AmbaPref"
#define AMBA_CALIB_UBIDEV           (2U)
#define AMBA_PREF_UBIDEV            (1U)

#define UBIVOL_MIN_KUINT            (10U)

static UINT32 CalcVolSize(UINT32 UbiDev, char *pMName)
{
    char    Cmd[128] = {0U};
    UINT32  KSize = 0U, Rval = 0U;
    FILE    *pFile = NULL;

    if (0 < sprintf(Cmd, "ubinfo -d %u -a | grep '%s'", UbiDev, pMName)) {
        pFile = popen(Cmd, "r");
        if (pFile != NULL) {
            if (fgets(Cmd, sizeof(Cmd), pFile) != NULL) {
                Rval = sprintf(Cmd, "ubinfo -d %u -a | grep 'Size'", UbiDev);
            } else {
                Rval = sprintf(Cmd, "ubinfo -d %u -a | grep 'available logical'", UbiDev);
            }

            if (pclose(pFile) == -1) {
                DBG_NFTL_PRN("[NFTL] CalcVolSize: fail to pclose \n");
            }
        }
    }

    if (0U < Rval) {
        strcat(Cmd, " | awk -F'(' {'print $2'} | awk -F' ' {'print $1'}");
        DBG_NFTL_PRN("[NFTL] CalcVolSize: (%s)\n", Cmd);

        pFile = popen(Cmd, "r");
        if (pFile != NULL) {
            if (fgets(Cmd, sizeof(Cmd), pFile) != NULL) {
                KSize = strtoul(Cmd, NULL, 10);
                KSize /= 2U;
                KSize = GetRoundDownValU32(KSize, (1024U * UBIVOL_MIN_KUINT));
            }

            if (pclose(pFile) == -1) {
                DBG_NFTL_PRN("[NFTL] CalcVolSize: fail to pclose \n");
            }
        } else {
            DBG_NFTL_PRN("[NFTL] CalcVolSize: fail to popen \n");
        }
    }

    DBG_NFTL_PRN("[NFTL] CalcVolSize: KSize = %u \n", KSize);
    return KSize;
}

static UINT32 MtdDevGrep(char *pKeyWord, char *pMtdDev, UINT32 MtdSize)
{
    char    Cmd[128] = {0U};
    UINT32  Rval = NAND_ERR_ARG;
    FILE    *pFile = NULL;

    pMtdDev[0] = '\0';
    if (0 < sprintf(Cmd, "cat /proc/mtd | grep '%s' | awk -F: {'print $1'} | tr -d '\\n'", pKeyWord)) {
        DBG_NFTL_PRN("[NFTL] MtdDevGrep: (%s)\n", Cmd);

        pFile = popen(Cmd, "r");
        if (pFile != NULL) {
            if (fgets(pMtdDev, MtdSize, pFile) != NULL) {
                Rval = NAND_ERR_NONE;
            } else {
                DBG_NFTL_PRN("[NFTL] MtdDevGrep: fail to fgets \n");
            }

            if (pclose(pFile) == -1) {
                DBG_NFTL_PRN("[NFTL] MtdDevGrep: fail to pclose \n");
            }
        }
    }

    return Rval;
}

static UINT32 UBIAttach(UINT32 UbiDev, char *pMName, char *pMtdStr)
{
    char    Cmd[128] = {0U};
    UINT32  Rval = NAND_ERR_ARG, IsNeedFormat = 0U, IsAttached = 0U;
    FILE    *pFile = NULL;

    if (0 < sprintf(Cmd, "ubinfo -a | grep '%s'", pMName)) {
        DBG_NFTL_PRN("[NFTL] UBIMount: (%s)\n", Cmd);

        pFile = popen(Cmd, "r");
        if (pFile != NULL) {
            if (fgets(Cmd, sizeof(Cmd), pFile) != NULL) {
                IsAttached = 1U;
            }

            if (pclose(pFile) == -1) {
                DBG_NFTL_PRN("[NFTL] UBIMount: fail to pclose \n");
            }
        }
    }

    if (IsAttached != 0U) {
        Rval = NAND_ERR_NONE;
    } else {
        if (0 < sprintf(Cmd, "ubiattach -d %u -p /dev/%s | grep 'error'", UbiDev, pMtdStr)) {
            DBG_NFTL_PRN("[NFTL] UBIAttach: (%s)\n", Cmd);

            pFile = popen(Cmd, "r");
            if (pFile != NULL) {
                if (fgets(Cmd, sizeof(Cmd), pFile) != NULL) {
                    IsNeedFormat = 1U;
                } else {
                    Rval = NAND_ERR_NONE;
                }

                if (pclose(pFile) == -1) {
                    DBG_NFTL_PRN("[NFTL] UBIMount: fail to pclose \n");
                }
            }

            if (0U < IsNeedFormat) {
                /* ubiformat */
                if (0 < sprintf(Cmd, "ubiformat /dev/%s", pMtdStr)) {
                    DBG_NFTL_PRN("[NFTL] UBIAttach: (%s)\n", Cmd);

                    if (system(Cmd) != -1) {
                        /* attach again */
                        if (0 < sprintf(Cmd, "ubiattach -d %u -p /dev/%s", UbiDev, pMtdStr)) {
                            DBG_NFTL_PRN("[NFTL] UBIAttach: (%s)\n", Cmd);

                            if (system(Cmd) != -1) {
                                Rval = NAND_ERR_NONE;
                            }
                        }
                    }
                }
            }

            if (Rval == NAND_ERR_NONE) {
                if (0 < sprintf(Cmd, "mkdir /tmp/%s", pMName)) {
                    DBG_NFTL_PRN("[NFTL] UBIAttach: (%s)\n", Cmd);

                    if (system(Cmd) == -1) {
                        Rval = NAND_ERR_ARG;
                        DBG_NFTL_PRN("[NFTL] UBIAttach: fail to mkdir \n");
                    }
                }
            } else {
                DBG_NFTL_PRN("[NFTL] UBIAttach: fail to ubiattach \n");
            }
        }
    }

    return Rval;
}

static UINT32 UBIMkVol(UINT32 UbiDev, char *pMName)
{
    char    Cmd[128] = {0U};
    UINT32  Rval = NAND_ERR_ARG;

    if (0 < sprintf(Cmd, "ubimkvol /dev/ubi%u --name=%s -m", UbiDev, pMName)) {
        DBG_NFTL_PRN("[NFTL] UBIMkVol: (%s)\n", Cmd);

        if (system(Cmd) != -1) {
            Rval = NAND_ERR_NONE;
        } else {
            DBG_NFTL_PRN("[NFTL] UBIMkVol: fail to mkvol \n");
        }
    }

    return Rval;
}

static UINT32 UBIMount(UINT32 UbiDev, char *pMName, char *pPath)
{
    char         Cmd[128] = {0U};
    UINT32       Rval = NAND_ERR_ARG, IsVolExist = 0U;
    FILE         *pFile = NULL;
    struct stat  FileStat;

    if (0 < sprintf(Cmd, "ubinfo -d %u -a | grep '%s'", UbiDev, pMName)) {
        DBG_NFTL_PRN("[NFTL] UBIMount: (%s)\n", Cmd);

        pFile = popen(Cmd, "r");
        if (pFile != NULL) {
            if (fgets(Cmd, sizeof(Cmd), pFile) != NULL) {
                IsVolExist = 1U;
            }

            if (pclose(pFile) == -1) {
                DBG_NFTL_PRN("[NFTL] UBIMount: fail to pclose \n");
            }
        }
    }

    if (0U < IsVolExist) {
        if (stat(pPath, &FileStat) == 0) {
            Rval = NAND_ERR_NONE;
        } else {
            if (0 < sprintf(Cmd, "mount -t ubifs ubi%u:%s /tmp/%s -o sync", UbiDev, pMName, pMName)) {
                DBG_NFTL_PRN("[NFTL] UBIMount: (%s)\n", Cmd);

                if (system(Cmd) != -1) {
                    Rval = NAND_ERR_NONE;
                } else {
                    DBG_NFTL_PRN("[NFTL] UBIMount: fail to mount \n");
                }
            }
        }
    } else {
        DBG_NFTL_PRN("[NFTL] UBIMount: volume isn't existing \n");
    }

    return Rval;
}

static UINT32 UBICreateFile(UINT32 UbiDev, char *pMName, char *pPath)
{
    char         Cmd[128] = {0U};
    UINT32       Rval = NAND_ERR_ARG, KSize;
    struct stat  FileStat;

    if (stat(pPath, &FileStat) != 0) {
        KSize = CalcVolSize(UbiDev, pMName);
        if (KSize != 0U) {
            if (access(pPath, F_OK) != -1) {
                Rval = NAND_ERR_NONE;
            } else {
                if (0 < sprintf(Cmd, "dd if=/dev/zero of=%s bs=%uK count=%u", pPath, UBIVOL_MIN_KUINT, KSize)) {
                    DBG_NFTL_PRN("[NFTL] UBICreateFile: (%s)\n", Cmd);

                    if (system(Cmd) != -1) {
                        Rval = NAND_ERR_NONE;
                    } else {
                        DBG_NFTL_PRN("[NFTL] UBICreateFile: fail to create file \n");
                    }
                }
            }
        }
    } else {
        /* file is existing */
        Rval = NAND_ERR_NONE;
    }

    return Rval;
}

/*
 *  @RoutineName:: AmbaNAND_FtlInit
 *
 *  @Description:: NAND FTL initializations
 *
 *  @Input      ::
 *      UserPartID: User Partition ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_FtlInit(UINT32 UserPartID)
{
#define NFTL_STATE_MTD_CHK      (0U)
#define NFTL_STATE_UBI_ATT      (1U)
#define NFTL_STATE_UBI_MNT      (2U)
#define NFTL_STATE_UBI_MKV      (3U)

    char    *pPath, *pMName, MtdStr[16] = {0U};
    UINT32  RetVal = NAND_ERR_ARG, UbiDev = 0U, State = NFTL_STATE_MTD_CHK;

    if (UserPartID == AMBA_USER_PARTITION_CALIBRATION_DATA) {
        pPath = AMBA_PART_CALIB_PATH;
        pMName = AMBA_CALIB_MNAME;
        UbiDev = AMBA_CALIB_UBIDEV;
        RetVal = MtdDevGrep("CALIB", MtdStr, 16);
    } else if (UserPartID == AMBA_USER_PARTITION_USER_SETTING) {
        pPath = AMBA_PART_PREF_PATH;
        pMName = AMBA_PREF_MNAME;
        UbiDev = AMBA_PREF_UBIDEV;
        RetVal = MtdDevGrep("USER_SETTING", MtdStr, 16);
    } else {
        /* do nothing */
        DBG_NFTL_PRN("[NFTL] AmbaNAND_FtlInit: wrong  UserPartID(%u) \n", UserPartID);
    }

    if (RetVal == NAND_ERR_NONE) {
        DBG_NFTL_PRN("[NFTL] AmbaNAND_FtlInit: %s \n", MtdStr);

        if (State == NFTL_STATE_MTD_CHK) {
            /* ubiattach */
            RetVal = UBIAttach(UbiDev, pMName, MtdStr);
            if (RetVal == NAND_ERR_NONE) {
                State = NFTL_STATE_UBI_ATT;
            }
        }

        if (State == NFTL_STATE_UBI_ATT) {
            /* mount */
            RetVal = UBIMount(UbiDev, pMName, pPath);
            if (RetVal == NAND_ERR_NONE) {
                State = NFTL_STATE_UBI_MNT;
            } else {
                State = NFTL_STATE_UBI_MKV;
            }
        }

        if (State == NFTL_STATE_UBI_MNT) {
            /* check bin file */
            RetVal = UBICreateFile(UbiDev, pMName, pPath);
        }

        if (State == NFTL_STATE_UBI_MKV) {
            /* mkdir ubifs volume */
            RetVal = UBIMkVol(UbiDev, pMName);
            if (RetVal == NAND_ERR_NONE) {
                RetVal = UBIMount(UbiDev, pMName, pPath);
                if (RetVal == NAND_ERR_NONE) {
                    RetVal = UBICreateFile(UbiDev, pMName, pPath);
                }
            }
        }
    } else {
        DBG_NFTL_PRN("[NFTL] AmbaNAND_FtlInit: partition isn't exist \n");
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_FtlSetInitMode
 *
 *  @Description::
 *
 *  @Input      ::
 *      UserPartID: User Partition ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_FtlSetInitMode(UINT32 UserPartID, UINT32 Mode)
{
    (void) UserPartID;
    (void) Mode;

    return NAND_ERR_NONE;
}

/*
 *  @RoutineName:: AmbaNVM_GetFtlInfo
 *
 *  @Description:: Get NAND FTL informatio of the partition
 *
 *  @Input      ::
 *      NvmID:      Memory type of the ROM partiton
 *      UserPartID: User Partition ID
 *      pFtlInfo:   pointer to the data buffer oof FTL information
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_GetFtlInfo(UINT32 UserPartID, AMBA_NVM_FTL_INFO_s *pFtlInfo)
{
    char    *pMName;
    UINT32  RetVal = NAND_ERR_NONE, UbiDev = 0U, KSize;

    if (AmbaWrap_memset(pFtlInfo, 0, sizeof(AMBA_NVM_FTL_INFO_s)) != 0U) {
        /* do nothing */
    }

    if (UserPartID == AMBA_USER_PARTITION_CALIBRATION_DATA) {
        pMName = AMBA_CALIB_MNAME;
        UbiDev = AMBA_CALIB_UBIDEV;
    } else if (UserPartID == AMBA_USER_PARTITION_USER_SETTING) {
        pMName = AMBA_PREF_MNAME;
        UbiDev = AMBA_PREF_UBIDEV;
    } else {
        RetVal = NAND_ERR_ARG;
        DBG_NFTL_PRN("[NFTL] AmbaNAND_GetFtlInfo: wrong  UserPartID(%u) \n", UserPartID);
    }

    if (RetVal == NAND_ERR_NONE) {
        KSize = CalcVolSize(UbiDev, pMName);
        KSize *= (1024U * UBIVOL_MIN_KUINT);

        pFtlInfo->Initialized       = 1U;
        pFtlInfo->TotalSizeInBytes  = KSize;
        pFtlInfo->SectorSizeInBytes = 512U;

        DBG_NFTL_PRN("[NFTL] AmbaNAND_GetFtlInfo: size = %u \n", KSize);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_ReadSector
 *
 *  @Description:: Read NAND sector data
 *
 *  @Input      ::
 *      UserPartID:     User Partition ID
 *      pSecConfig:     Sector config struture
 *      TimeOut:        Time out value
 *
 *  @Output     ::
 *      pDataBuf:    pointer to the data
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_ReadSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    LONG    Offset;
    UINT32  RetVal = NAND_ERR_ARG;
    FILE    *pFile = NULL;
    size_t  RdSize, Size;

    if (UserPartID == AMBA_USER_PARTITION_CALIBRATION_DATA) {
        pFile = fopen(AMBA_PART_CALIB_PATH, "r+");
    } else if (UserPartID == AMBA_USER_PARTITION_USER_SETTING) {
        pFile = fopen(AMBA_PART_PREF_PATH, "r+");
    } else {
        /* do nothing */
        DBG_NFTL_PRN("[NFTL] AmbaNAND_ReadSector: wrong  UserPartID(%u) \n", UserPartID);
    }

    if (pFile != NULL) {
        Offset = pSecConfig->StartSector * 512;
        RdSize = pSecConfig->NumSector * 512;

        if (fseek(pFile, Offset, SEEK_SET) == 0) {
            Size = fread(pSecConfig->pDataBuf, 1, RdSize, pFile);
            if (Size == RdSize) {
                RetVal = NAND_ERR_NONE;
            } else {
                DBG_NFTL_PRN("[NFTL] AmbaNAND_ReadSector: fread(%lu/%lu) \n", Size, RdSize);
            }
        } else {
            DBG_NFTL_PRN("[NFTL] AmbaNAND_ReadSector: fail to fseek \n");
        }

        if (fclose(pFile) != 0) {
            RetVal = NAND_ERR_ARG;
            DBG_NFTL_PRN("[NFTL] AmbaNAND_ReadSector: fail to fclose \n");
        }
    } else {
        DBG_NFTL_PRN("[NFTL] AmbaNAND_ReadSector: fail to fopen \n");
    }

    AmbaMisra_TouchUnused(&TimeOut);
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_WriteSector
 *
 *  @Description:: Write NAND Sector data
 *
 *  @Input      ::
 *      UserPartID:     User Partition ID
 *      pSecConfig:     Sector config struture
 *      TimeOut:        Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_WriteSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    char         *pPath, Str[128] = {0U};
    UINT32       RetVal = NAND_ERR_ARG;
    FILE         *pFile = NULL;
    size_t       PhSize, PwSize, PtSize, Size;
    struct stat  FileStat;

    if (UserPartID == AMBA_USER_PARTITION_CALIBRATION_DATA) {
        pPath = AMBA_PART_CALIB_PATH;
        if (stat(AMBA_PART_CALIB_PATH, &FileStat) == 0) {
            RetVal = NAND_ERR_NONE;
        } else {
            DBG_NFTL_PRN("[NFTL] AmbaNAND_WriteSector: fail to stat %s \n", AMBA_PART_CALIB_PATH);
        }
    } else if (UserPartID == AMBA_USER_PARTITION_USER_SETTING) {
        pPath = AMBA_PART_PREF_PATH;
        if (stat(AMBA_PART_PREF_PATH, &FileStat) == 0) {
            RetVal = NAND_ERR_NONE;
        } else {
            DBG_NFTL_PRN("[NFTL] AmbaNAND_WriteSector: fail to stat %s \n", AMBA_PART_PREF_PATH);
        }
    } else {
        /* do nothing */
        DBG_NFTL_PRN("[NFTL] AmbaNAND_WriteSector: wrong  UserPartID(%u) \n", UserPartID);
    }

    if (RetVal == NAND_ERR_NONE) {
        PhSize = pSecConfig->StartSector * 512;
        PwSize = pSecConfig->NumSector * 512;
        PtSize = FileStat.st_size - PhSize - PwSize;

        if (0 < sprintf(Str, "%s.part1", pPath)) {
            pFile = fopen(Str, "w+");
            if (pFile != NULL) {
                Size = fwrite(pSecConfig->pDataBuf, 1, PwSize, pFile);
                if (Size != PwSize) {
                    RetVal = NAND_ERR_ARG;
                    DBG_NFTL_PRN("[NFTL] AmbaNAND_WriteSector: fail to fwrite(%lu/%lu) \n", Size, PwSize);
                }

                if (fclose(pFile) != 0) {
                    RetVal = NAND_ERR_ARG;
                    DBG_NFTL_PRN("[NFTL] AmbaNAND_WriteSector: fail to fclose \n");
                }
            } else {
                DBG_NFTL_PRN("[NFTL] AmbaNAND_WriteSector: fail to fopen \n");
            }
        }

        if ((RetVal == NAND_ERR_NONE) && (PhSize != 0U)) {
            if (0 < sprintf(Str, "head -c %lu %s > %s.part0", PhSize, pPath, pPath)) {
                if (system(Str) == -1) {
                    RetVal = NAND_ERR_ARG;
                    DBG_NFTL_PRN("[NFTL] AmbaNAND_WriteSector: fail to head part0 \n");
                }
            }
        }

        if ((RetVal == NAND_ERR_NONE) && (PtSize != 0U)) {
            if (0 < sprintf(Str, "tail -c %lu %s > %s.part2", PtSize, pPath, pPath)) {
                if (system(Str) == -1) {
                    RetVal = NAND_ERR_ARG;
                    DBG_NFTL_PRN("[NFTL] AmbaNAND_WriteSector: fail to tail part2 \n");
                }
            }
        }

        if (RetVal == NAND_ERR_NONE) {
            if (0 < sprintf(Str, "cat %s.part? > %s", pPath, pPath)) {
                if (system(Str) == -1) {
                    RetVal = NAND_ERR_ARG;
                    DBG_NFTL_PRN("[NFTL] AmbaNAND_WriteSector: fail to merge \n");
                }
            }
        }

        if (0 < sprintf(Str, "rm -rf %s.part?", pPath)) {
            if (system(Str) == -1) {
                RetVal = NAND_ERR_ARG;
                DBG_NFTL_PRN("[NFTL] AmbaNAND_WriteSector: fail to remove \n");
            }
        }
    }

    AmbaMisra_TouchUnused(&TimeOut);
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_FTL_Format
 *
 *  @Description:: NAND format a partition
 *
 *  @Input      ::
 *      UserPartID: User Partition ID
 *      TimeOut:    Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_FTL_Format(UINT32 UserPartID, UINT32 TimeOut)
{
    (void)UserPartID;
    (void)TimeOut;

    return NAND_ERR_NONE;
}
