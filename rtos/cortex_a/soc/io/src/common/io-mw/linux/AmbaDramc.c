/**
 *  @file AmbaDramc.c
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
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDRAMC.h"
#ifndef AMBA_DRAMC_DEF_H
#include "AmbaDRAMC_Def.h"
#endif
#ifndef AMBA_REG_DRAMC_H
#include "AmbaReg_DRAMC.h"
#endif
#include <unistd.h>
#include <sys/types.h>

//#define DRAMC_IO_MW_PRINT
static AMBA_KAL_MUTEX_t AmbaDramcMutex;

#if defined(DRAMC_IO_MW_PRINT)
static void AmbaDRAMC_PrintStatis(AMBA_DRAMC_STATIS_s* StatisData)
{
    UINT32 index;

    printf("DRAMC stats:\n");
    for (index = 0U; index < 32U; index++) {
        printf("ClientRequestStatis[%u]: %u\n", index, StatisData->ClientRequestStatis[index]);
        printf("ClientBurstStatis[%u]: %u\n", index, StatisData->ClientBurstStatis[index]);
        printf("ClientMaskWriteStatis[%u]: %u\n", index, StatisData->ClientMaskWriteStatis[index]);
    }
}
#endif

/**
 *  AmbaDRAMC_DrvEntry - DRAMC driver initializations
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaDRAMC_DrvEntry(void)
{
    static char AmbaDramcMutexName[16] = "AmbaDramcMutex";
    UINT32 RetVal = DRAMC_ERR_NONE;

    /* Create Mutexes */
    if (AmbaKAL_MutexCreate(&AmbaDramcMutex, AmbaDramcMutexName) != KAL_ERR_NONE) {
        RetVal = DRAMC_ERR_UNEXPECTED;
    }

    return RetVal;
}

/**
 *  AmbaDRAMC_EnableStatis - Enable Dramc client statistics calculating
 *  @return error code
 */
UINT32 AmbaDRAMC_EnableStatis(void)
{
    UINT32 RetVal = DRAMC_ERR_NONE;
    char proc_file[] = "/proc/ambarella/dramc_stats";
    FILE* fp = NULL;

    if (AmbaKAL_MutexTake(&AmbaDramcMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        perror("Take mutex fail");
        RetVal = DRAMC_ERR_MUTEX;
    } else {
        fp = fopen(proc_file, "w");
        if(!fp) {
            perror("proc_file open fail");
            RetVal = DRAMC_ERR_IO;
        } else {
            fprintf(fp, "%d", 1);
            fclose(fp);
        }

        if (AmbaKAL_MutexGive(&AmbaDramcMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            perror("Give mutex fail");
            RetVal = DRAMC_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaDRAMC_GetStatisInfo - Get dram clients statistics info
 *  @param[out] StatisData data of dram client statistics
 *  @return error code
 */
UINT32 AmbaDRAMC_GetStatisInfo(AMBA_DRAMC_STATIS_s* StatisData)
{
    UINT32 RetVal = DRAMC_ERR_NONE;
    char proc_file[] = "/proc/ambarella/dramc_stats";
    int fd;
    FILE* fp;
    uint64_t* pVirtualAddr;
    int c = 0;

    if (StatisData == NULL) {
        perror("No vaild StatisData ptr");
        RetVal = DRAMC_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaDramcMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            perror("Take mutex fail");
            RetVal = DRAMC_ERR_MUTEX;
        } else {
            fp = fopen(proc_file, "r");
            if(!fp) {
                perror("proc_file open fail");
                RetVal = DRAMC_ERR_IO;
            } else {
                c = getc(fp);
                while (c != EOF)
                {
                    putchar(c);
                    c = getc(fp);
                }
                fclose(fp);

                fd = open("/dev/mem", O_RDONLY);
                if (fd < 0) {
                    perror("Unable to open /dev/mem");
                    RetVal = DRAMC_ERR_IO;
                } else {
                    pVirtualAddr = mmap(NULL, 0x1000, PROT_READ, MAP_SHARED, fd, 0x20e0030000);
                    if (pVirtualAddr == MAP_FAILED) {
                        perror("mmap failed");
                        RetVal = DRAMC_ERR_AT;
                    } else {
                        memcpy(StatisData, pVirtualAddr, sizeof(AMBA_DRAMC_STATIS_s));

#if defined(DRAMC_IO_MW_PRINT)
                        printf("DRAMC stats:\n");
                        AmbaDRAMC_PrintStatis(StatisData);
                        printf("DRAMC stats (scratchpad):\n");
                        AmbaDRAMC_PrintStatis((AMBA_DRAMC_STATIS_s*)pVirtualAddr);
#endif

                        if (munmap(pVirtualAddr, 0x1000) < 0) {
                            perror("munmap failed");
                            RetVal = DRAMC_ERR_AT;
                        }
                    }
                }
                close(fd);
            }

            if (AmbaKAL_MutexGive(&AmbaDramcMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                perror("Give mutex fail...");
                RetVal = DRAMC_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

static UINT32 AmbaDRAMC_TakeMutex(void)
{
    UINT32 RetVal = DRAMC_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaDramcMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        perror("Take mutex fail");
        RetVal = DRAMC_ERR_MUTEX;
    }

    return RetVal;
}

static UINT32 AmbaDRAMC_GiveMutex(void)
{
    UINT32 RetVal = DRAMC_ERR_NONE;

    if (AmbaKAL_MutexGive(&AmbaDramcMutex) != KAL_ERR_NONE) {
        /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
        perror("Give mutex fail");
        RetVal = DRAMC_ERR_UNEXPECTED;
    }

    return RetVal;
}

int AmbaDRAMC_OpenDevice(void)
{
    char dev_file[] = "/dev/dramc";
    int fd;

    fd = open(dev_file, O_RDWR);
    if(fd < 0) {
        perror("dev_file open fail");
    }

    return fd;
}

UINT32 AmbaDRAMC_Set(UINT32 Offset, UINT32 Val)
{
    UINT32 RetVal = DRAMC_ERR_NONE;
    int fd;

    RetVal = AmbaDRAMC_TakeMutex();
    if(RetVal == DRAMC_ERR_MUTEX)
        return RetVal;

    fd = AmbaDRAMC_OpenDevice();
    if(fd < 0)
        return DRAMC_ERR_UNEXPECTED;

    ioctl(fd, DRAMC_IOC_CHG_REG, &Offset);
    ioctl(fd, DRAMC_IOC_CHG_VAL, &Val);
    ioctl(fd, DRAMC_IOC_W_DRAMC);
    close(fd);

    RetVal = AmbaDRAMC_GiveMutex();
    if(RetVal == DRAMC_ERR_UNEXPECTED)
        return RetVal;

    return RetVal;
}

UINT32 AmbaDRAMC_GetHostNum(UINT32* pHostNum)
{
    int fd;
    unsigned int val;
    UINT32 RetVal = DRAMC_ERR_NONE;

    RetVal = AmbaDRAMC_TakeMutex();
    if(RetVal == DRAMC_ERR_MUTEX)
        return RetVal;

    fd = AmbaDRAMC_OpenDevice();
    if(fd < 0)
        return DRAMC_ERR_UNEXPECTED;

    if (pHostNum == NULL) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        val = offsetof(AMBA_DRAMC_REG_s, DramMode);
        ioctl(fd, DRAMC_IOC_CHG_REG, &val);
        ioctl(fd, DRAMC_IOC_R_DRAMC, &val);

        if (0x6 == (val & 0x6)) {
            *pHostNum = 2U;
        } else {
            *pHostNum = 1U;
        }
    }

    close(fd);

    RetVal = AmbaDRAMC_GiveMutex();
    if(RetVal == DRAMC_ERR_UNEXPECTED)
        return RetVal;

    return RetVal;
}

UINT32 AmbaDRAMC_GetHostStartId(UINT32* pHostStartId)
{
    int fd;
    unsigned int val;
    UINT32 RetVal = DRAMC_ERR_NONE;

    RetVal = AmbaDRAMC_TakeMutex();
    if(RetVal == DRAMC_ERR_MUTEX)
        return RetVal;

    fd = AmbaDRAMC_OpenDevice();
    if(fd < 0)
        return DRAMC_ERR_UNEXPECTED;

    if (pHostStartId == NULL) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        val = offsetof(AMBA_DRAMC_REG_s, DramMode);
        ioctl(fd, DRAMC_IOC_CHG_REG, &val);
        ioctl(fd, DRAMC_IOC_R_DRAMC, &val);

        if (0x0U != (val & 0x2U)) {
            *pHostStartId = 0U;
        } else if (0x0U != (val & 0x4U)) {
            *pHostStartId = 1U;
        } else {
            RetVal = DRAMC_ERR_ARG;
        }
    }

    close(fd);

    RetVal = AmbaDRAMC_GiveMutex();
    if(RetVal == DRAMC_ERR_UNEXPECTED)
        return RetVal;

    return RetVal;
}

