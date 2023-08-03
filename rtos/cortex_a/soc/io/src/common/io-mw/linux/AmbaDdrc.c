/**
 *  @file AmbaDdrc.c
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
#include "AmbaDDRC.h"
#ifndef AMBA_DDRC_DEF_H
#include "AmbaDDRC_Def.h"
#endif
#ifndef AMBA_REG_DDRC_H
#include "AmbaReg_DDRC.h"
#endif
#ifndef AMBA_CSL_DDRC_H
#include "AmbaCSL_DDRC.h"
#endif
#ifndef AMBA_RTSL_DDRC_H
#include "AmbaRTSL_DDRC.h"
#endif
#include <unistd.h>
#include <sys/types.h>

static AMBA_KAL_MUTEX_t AmbaDdrcMutex;

static UINT32 AmbaDDRC_TakeMutex(void)
{
    UINT32 RetVal = DDRC_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaDdrcMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        perror("Take mutex fail");
        RetVal = DDRC_ERR_MUTEX;
    }

    return RetVal;
}

static UINT32 AmbaDDRC_GiveMutex(void)
{
    UINT32 RetVal = DDRC_ERR_NONE;

    if (AmbaKAL_MutexGive(&AmbaDdrcMutex) != KAL_ERR_NONE) {
        /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
        perror("Give mutex fail");
        RetVal = DDRC_ERR_UNEXPECTED;
    }

    return RetVal;
}

/**
 *  AmbaDDRC_DrvEntry - DDRC driver initializations
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaDDRC_DrvEntry(void)
{
    static char AmbaDdrcMutexName[16] = "AmbaDdrcMutex";
    UINT32 RetVal = DDRC_ERR_NONE;

    /* Create Mutexes */
    if (AmbaKAL_MutexCreate(&AmbaDdrcMutex, AmbaDdrcMutexName) != KAL_ERR_NONE) {
        RetVal = DDRC_ERR_UNEXPECTED;
    }

    return RetVal;
}

int AmbaDDRC_OpenDevice(UINT32 DdrcId)
{
    char dev_file[] = "/dev/ddrcXX";
    int fd;

    snprintf(dev_file, sizeof(dev_file), "/dev/ddrc%d", DdrcId);
    fd = open(dev_file, O_RDWR);
    if(fd < 0) {
        perror("dev_file open fail");
    }

    return fd;
}

UINT32 AmbaDDRC_Get(UINT32 DdrcId, UINT32 Offset)
{
    UINT32 RetVal = DDRC_ERR_NONE;
    UINT32 Val;
    int fd;

    RetVal = AmbaDDRC_TakeMutex();
    if(RetVal == DDRC_ERR_MUTEX)
        return RetVal;

    fd = AmbaDDRC_OpenDevice(DdrcId);
    if(fd < 0)
        return DDRC_ERR_UNEXPECTED;

    ioctl(fd, DDRC_IOC_CHG_REG, &Offset);
    Val = Offset;
    ioctl(fd, DDRC_IOC_R_DDRC, &Val);
    close(fd);

    RetVal = AmbaDDRC_GiveMutex();
    if(RetVal == DDRC_ERR_UNEXPECTED)
        return RetVal;

    return (RetVal == DDRC_ERR_NONE) ? Val : RetVal;

}

UINT32 AmbaDDRC_Set(UINT32 DdrcId, UINT32 Offset, UINT32 Val)
{
    UINT32 RetVal = DDRC_ERR_NONE;
    int fd;

    RetVal = AmbaDDRC_TakeMutex();
    if(RetVal == DDRC_ERR_MUTEX)
        return RetVal;

    fd = AmbaDDRC_OpenDevice(DdrcId);
    if(fd < 0)
        return DDRC_ERR_UNEXPECTED;

    ioctl(fd, DDRC_IOC_CHG_REG, &Offset);
    ioctl(fd, DDRC_IOC_CHG_VAL, &Val);
    ioctl(fd, DDRC_IOC_W_DDRC);
    close(fd);

    RetVal = AmbaDDRC_GiveMutex();
    if(RetVal == DDRC_ERR_UNEXPECTED)
        return RetVal;

    return RetVal;
}

UINT32 AmbaDDRC_GetDramTypeFromVal(UINT32 Config0)
{
    AMBA_DDRC_CONFIG0_REG_s* temp = (AMBA_DDRC_CONFIG0_REG_s*)&Config0;
    return (UINT32)(temp->DramType);
}

UINT32 AmbaDDRC_GetDramSizeFromVal(UINT32 Config0)
{
    AMBA_DDRC_CONFIG0_REG_s* temp = (AMBA_DDRC_CONFIG0_REG_s*)&Config0;
    return (UINT32)(temp->DramSize);
}

UINT32 AmbaDDRC_GetDuelDieEnFromVal(UINT32 Config0)
{
    AMBA_DDRC_CONFIG0_REG_s* temp = (AMBA_DDRC_CONFIG0_REG_s*)&Config0;
    return (UINT32)(temp->DuelDieEn);
}

extern const char DdrDramType[4][8];
extern const char DdrDramSize[10][8];
UINT32 AmbaDDRC_GetDramType(UINT32 DdrcId)
{
    UINT32 RetVal = DDRC_ERR_NONE;
    int fd;
    unsigned int temp = 0;

    RetVal = AmbaDDRC_TakeMutex();
    if(RetVal == DDRC_ERR_MUTEX)
        return RetVal;

    fd = AmbaDDRC_OpenDevice(DdrcId);
    if(fd < 0)
        return DDRC_ERR_UNEXPECTED;

    temp = offsetof(AMBA_DDRC_REG_s, Config0);
    ioctl(fd, DDRC_IOC_CHG_REG, &temp);
    ioctl(fd, DDRC_IOC_R_DDRC, &temp);
    temp = AmbaDDRC_GetDramTypeFromVal(temp);
    close(fd);

    RetVal = AmbaDDRC_GiveMutex();
    if(RetVal == DDRC_ERR_UNEXPECTED)
        return RetVal;

    return (RetVal == DDRC_ERR_NONE) ? temp : RetVal;
}

UINT32 AmbaDDRC_GetDramSize(UINT32 DdrcId)
{
    UINT32 RetVal = DDRC_ERR_NONE;
    int fd;
    unsigned int temp = 0;

    RetVal = AmbaDDRC_TakeMutex();
    if(RetVal == DDRC_ERR_MUTEX)
        return RetVal;

    fd = AmbaDDRC_OpenDevice(DdrcId);
    if(fd < 0)
        return DDRC_ERR_UNEXPECTED;

    temp = offsetof(AMBA_DDRC_REG_s, Config0);
    ioctl(fd, DDRC_IOC_CHG_REG, &temp);
    ioctl(fd, DDRC_IOC_R_DDRC, &temp);
    temp = AmbaDDRC_GetDramSizeFromVal(temp);
    close(fd);

    RetVal = AmbaDDRC_GiveMutex();
    if(RetVal == DDRC_ERR_UNEXPECTED)
        return RetVal;

    return (RetVal == DDRC_ERR_NONE) ? temp : RetVal;
}

const char* AmbaDDRC_GetDramTypeStr(UINT32 DdrcId)
{
    return DdrDramType[AmbaDDRC_GetDramType(DdrcId)];
}

const char* AmbaDDRC_GetDramSizeStr(UINT32 DdrcId)
{
    return DdrDramSize[AmbaDDRC_GetDramSize(DdrcId)];
}

UINT32 AmbaDDRC_GetModeReg(UINT32 DdrcId)
{
    UINT32 RetVal = DDRC_ERR_NONE;
    int fd;
    unsigned int temp = 0;

    RetVal = AmbaDDRC_TakeMutex();
    if(RetVal == DDRC_ERR_MUTEX)
        return RetVal;

    fd = AmbaDDRC_OpenDevice(DdrcId);
    if(fd < 0)
        return DDRC_ERR_UNEXPECTED;

    temp = offsetof(AMBA_DDRC_REG_s, ModeReg);
    ioctl(fd, DDRC_IOC_CHG_REG, &temp);
    ioctl(fd, DDRC_IOC_R_DDRC, &temp);
    close(fd);

    RetVal = AmbaDDRC_GiveMutex();
    if(RetVal == DDRC_ERR_UNEXPECTED)
        return RetVal;

    return (RetVal == DDRC_ERR_NONE) ? temp : RetVal;
}

UINT32 AmbaDDRC_SetModeReg(UINT32 DdrcId, UINT32 Val)
{
    UINT32 RetVal = DDRC_ERR_NONE;
    int fd;
    unsigned int temp = 0;

    RetVal = AmbaDDRC_TakeMutex();
    if(RetVal == DDRC_ERR_MUTEX)
        return RetVal;

    fd = AmbaDDRC_OpenDevice(DdrcId);
    if(fd < 0)
        return DDRC_ERR_UNEXPECTED;

    temp = offsetof(AMBA_DDRC_REG_s, ModeReg);
    ioctl(fd, DDRC_IOC_CHG_REG, &temp);
    ioctl(fd, DDRC_IOC_CHG_VAL, &Val);
    ioctl(fd, DDRC_IOC_W_DDRC);
    close(fd);

    RetVal = AmbaDDRC_GiveMutex();
    if(RetVal == DDRC_ERR_UNEXPECTED)
        return RetVal;

    return RetVal;
}

UINT32 AmbaDDRC_GetByteDly(UINT32 DdrcId, UINT32 ByteNum, UINT32 Index)
{
    if (ByteNum == 0U) {
        return AmbaDDRC_Get(DdrcId, offsetof(AMBA_DDRC_REG_s, Byte0Die0Dly[Index]));
    } else if (ByteNum == 1U) {
        return AmbaDDRC_Get(DdrcId, offsetof(AMBA_DDRC_REG_s, Byte1Die0Dly[Index]));
    } else if (ByteNum == 2U) {
        return AmbaDDRC_Get(DdrcId, offsetof(AMBA_DDRC_REG_s, Byte2Die0Dly[Index]));
    } else if (ByteNum == 3U) {
        return AmbaDDRC_Get(DdrcId, offsetof(AMBA_DDRC_REG_s, Byte3Die0Dly[Index]));
    }
    return 0;
}

UINT32 AmbaDDRC_GetDqVref(UINT32 DdrcId, UINT32 ByteNum)
{
    if (ByteNum == 0U) {
        return (AmbaDDRC_Get(DdrcId, offsetof(AMBA_DDRC_REG_s, RdVref0)) >> DQ_VREF_POS1) & DQ_VREF_MASK;
    } else if (ByteNum == 1U) {
        return (AmbaDDRC_Get(DdrcId, offsetof(AMBA_DDRC_REG_s, RdVref0)) >> DQ_VREF_POS2) & DQ_VREF_MASK;
    } else if (ByteNum == 2U) {
        return (AmbaDDRC_Get(DdrcId, offsetof(AMBA_DDRC_REG_s, RdVref1)) >> DQ_VREF_POS1) & DQ_VREF_MASK;
    } else if (ByteNum == 3U) {
        return (AmbaDDRC_Get(DdrcId, offsetof(AMBA_DDRC_REG_s, RdVref1)) >> DQ_VREF_POS2) & DQ_VREF_MASK;
    }
    return 0;
}

