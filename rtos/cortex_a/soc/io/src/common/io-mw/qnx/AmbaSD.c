/**
 *  @file AmbaSD.c
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
 *  @details SD Controller APIs
 *
 */

#include <AmbaTypes.h>
#include "AmbaDef.h"
#include "AmbaWrap.h"

#include "AmbaSD.h"
#include "AmbaSD_Ctrl.h"

#include "AmbaRTSL_SD.h"

#include "AmbaSYS.h"
#include "AmbaMisraFix.h"

#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/statvfs.h>
#include <sys/dcmd_cam.h>
#include "hw/dcmd_sim_mmcsd.h"
#include "hw/dcmd_sim_sdmmc.h"
#include <string.h>

UINT32 AmbaSD_CardInit(UINT32 SdChanNo)
{
    UINT32 status = OK; // OK is defined as unsigned int
    int fd;
    SDMMC_DRVR_STATE State;

    if (SdChanNo == AMBA_SD_CHANNEL0) {
        fd = open("/dev/hd0", O_RDWR);
    } else if (SdChanNo == AMBA_SD_CHANNEL1) {
        fd = open("/dev/hd1", O_RDWR);
    } else {
        fd = -1;
    }

    if (fd < 0) {
        status = 0x00340012U;//AMBA_SD_ERR_NO_CARD;
    } else {
        devctl(fd, DCMD_SDMMC_RESET, &State, sizeof(SDMMC_DRVR_STATE), NULL);

        close(fd);
    }
    return status;
}

UINT32 AmbaSD_ReadSector(UINT32 SdChanNo, const AMBA_NVM_SECTOR_CONFIG_s * pSecConfig)
{
    UINT32 RetVal = NVM_ERR_NONE;

    int fd;

    if (SdChanNo == AMBA_SD_CHANNEL0) {
        fd = open("/dev/hd0", O_RDWR);
    } else if (SdChanNo == AMBA_SD_CHANNEL1) {
        fd = open("/dev/hd1", O_RDWR);
    } else {
        fd = -1;
    }

    if (fd < 0) {
        RetVal = NVM_ERR_ARG;
    } else {
        int Ret = readblock(fd, 512, pSecConfig->StartSector, pSecConfig->NumSector, pSecConfig->pDataBuf);
        if (Ret == -1) {
            RetVal = NVM_ERR_eMMC_READ;
        }
        close(fd);
    }

    return RetVal;
}

UINT32 AmbaSD_WriteSector(UINT32 SdChanNo, const AMBA_NVM_SECTOR_CONFIG_s * pSecConfig)
{
    UINT32 RetVal = NVM_ERR_NONE;

    int fd;

    if (SdChanNo == AMBA_SD_CHANNEL0) {
        fd = open("/dev/hd0", O_RDWR | O_SYNC);
    } else if (SdChanNo == AMBA_SD_CHANNEL1) {
        fd = open("/dev/hd1", O_RDWR | O_SYNC);
    } else {
        fd = -1;
    }

    if (fd < 0) {
        RetVal = NVM_ERR_ARG;
    } else {
        int Ret = writeblock(fd, 512, pSecConfig->StartSector, pSecConfig->NumSector, pSecConfig->pDataBuf);
        if (Ret == -1) {
            RetVal = NVM_ERR_eMMC_WRITE;
        }
        close(fd);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSD_GetCardStatus
 *
 *  @Description:: Get Card Status
 *
 *  @Input      ::
 *      SdChanNo:    SD Channel Number
 *      pCardStatus: pointer to Card Status buffer
 *      TimeOut:     Time out value
 *
 *  @Output     ::
 *      pCardStatus: pointer to Card Status
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaSD_GetCardStatus(UINT32 SdChanNo, AMBA_SD_CARD_STATUS_s *pCardStatus)
{
    UINT32 status = OK; // OK is defined as unsigned int
    int fd;
    SDMMC_DEVICE_INFO Info;

    if (SdChanNo == AMBA_SD_CHANNEL0) {
        fd = open("/dev/hd0", O_RDWR);
    } else if (SdChanNo == AMBA_SD_CHANNEL1) {
        fd = open("/dev/hd1", O_RDWR);
    } else {
        fd = -1;
    }

    (void)memset(pCardStatus, 0, sizeof(AMBA_SD_CARD_STATUS_s));

    if (fd < 0) {
        status = 0x00340012U;//AMBA_SD_ERR_NO_CARD;
    } else {
        devctl(fd, DCMD_SDMMC_DEVICE_INFO, &Info, sizeof(SDMMC_DEVICE_INFO), NULL);

        close(fd);

        if ((Info.dtype == DEV_TYPE_MMC) || (Info.dtype == DEV_TYPE_SD)) {
            pCardStatus->CardIsInit = 1U;
            pCardStatus->CardSize   = (UINT64) Info.sectors * (UINT64)Info.sector_size;
        } else {
            pCardStatus->CardIsInit = 0;
            pCardStatus->CardSize   = 0;
        }

        AmbaSYS_GetIoClkFreq(AMBA_CLK_SD0 + SdChanNo, &pCardStatus->CardSpeed);
        pCardStatus->NumIoFunction  = 0;
        pCardStatus->SdType         = Info.dtype;
        pCardStatus->DataAfterErase = 1U;
        pCardStatus->WriteProtect   = ((Info.flags == DEV_FLAG_WP) ? 1U : 0U);

        status = 0;
    }

    return status;
}

UINT32 AmbaSD_DevctlSetDelay(UINT32 SdChanNo, UINT32 DelayValue)
{
    UINT32 status = OK; // OK is defined as unsigned int
    int fd;
    AMBA_SD_SETTING_s SdSetting;

    SdSetting.DetailDelay = DelayValue;

    if (SdChanNo == AMBA_SD_CHANNEL0) {
        fd = open("/dev/hd0", O_RDWR);
    } else if (SdChanNo == AMBA_SD_CHANNEL1) {
        fd = open("/dev/hd1", O_RDWR);
    } else {
        fd = -1;
    }

    devctl(fd, DCMD_SDMMC_SET_CONFIG, &SdSetting, sizeof(AMBA_SD_SETTING_s), NULL);
    close(fd);

    return status;
}

UINT32 AmbaSD_SendEmmcCMD6(UINT32 SdChanNo, const AMBA_SD_EMMC_CMD6_ARGUMENT_s *pArgCmd6)
{
    UINT32 RetVal = OK; // OK is defined as unsigned int
    INT32 fd;
    SDMMC_CARD_REGISTER Cmd6Reg[2];
    UINT8 *dptr;

    if (SdChanNo == AMBA_SD_CHANNEL0) {
        fd = open("/dev/hd0", O_RDWR);
    } else if (SdChanNo == AMBA_SD_CHANNEL1) {
        fd = open("/dev/hd1", O_RDWR);
#ifdef AMBA_SD_CHANNEL2
    } else if (SdChanNo == AMBA_SD_CHANNEL2) {
        fd = open("/dev/hd2", O_RDWR);
#endif
    } else {
        fd = -1;
    }

    Cmd6Reg[0].action = SDMMC_CR_ACTION_WRITE;
    Cmd6Reg[0].address = pArgCmd6->Index;
    Cmd6Reg[0].length = 1U;
    Cmd6Reg[0].type = SDMMC_REG_TYPE_EXT_CSD;

    dptr = (uint8_t *)&Cmd6Reg[1];

    *dptr = pArgCmd6->Value;

    devctl(fd, DCMD_SDMMC_CARD_REGISTER, Cmd6Reg, sizeof(SDMMC_CARD_REGISTER) * 2, NULL);
    close(fd);

    return RetVal;
}

