/**
 *  @file AmbaSD_Card.c
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
 *  @details SD Card Control APIs
 *
 */

#include "AmbaWrap.h"

#include "AmbaSD.h"
#include "AmbaSD_STD.h"
#include "AmbaSD_Ctrl.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#include "AmbaRTSL_SD.h"
//#include "AmbaPrint.h"

/**
 *  AmbaSD_CardInit - SD Card Initialization
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_CardInit(UINT32 SdChanNo)
{
    (void)SdChanNo;
    if (system("echo modprobe ambarella_sd") < 0) { }
    if (system("echo modprobe mmc_block") < 0) { }

    return 0U;
}

static int sd_popen(char *cmd, char *str, int strsize)
{
    FILE *ifp;
    char *tokp;
    char buf[64]  = "\0";

    if ((ifp = popen(cmd, "r")) == NULL) {
        return 1;
    }
    if (fgets(buf, sizeof(buf), ifp) == NULL) {
        pclose(ifp);
        return 1;
    }
    if ((tokp = strtok(buf, "\t \n")) != NULL) {
        memset(str, '\0', strsize);
        snprintf(str, strsize, "%s", tokp);
    } else {
        pclose(ifp);
        return 1;
    }

    pclose(ifp);

    return 0;
}

/**
 *  AmbaSD_GetCardStatus - Get Card Status
 *  @param[in] SdChanNo SD Channel Number
 *  @param[out] pCardStatus pointer to Card Status buffer
 *  @return error code
 */
UINT32 AmbaSD_GetCardStatus(UINT32 SdChanNo, AMBA_SD_CARD_STATUS_s *pCardStatus)
{
    UINT32 status = OK; // OK is defined as unsigned int
    char cmd[64];
    char buf[64];
    int ret;

    (void)AmbaWrap_memset(pCardStatus, 0, sizeof(AMBA_SD_CARD_STATUS_s));

    snprintf(cmd, sizeof(cmd), "cat /sys/block/mmcblk%d/size 2>/dev/null", SdChanNo);
    ret = sd_popen(cmd, buf, sizeof(buf));
    if (ret != 0) {
        status = 6;
    } else {
        pCardStatus->CardIsInit = 1;
        pCardStatus->CardSize = atol(buf) * 512;

        if (SdChanNo == 0U) {
            snprintf(cmd, sizeof(cmd), "cat /proc/ambarella/clock|grep \"gclk_sd:\"|awk '{print $2}'");
        } else {
            snprintf(cmd, sizeof(cmd), "cat /proc/ambarella/clock|grep \"gclk_sdxc:\"|awk '{print $2}'");
        }
        ret = sd_popen(cmd, buf, sizeof(buf));
        if (ret != 0) {
            pCardStatus->CardSpeed = 0;
        } else {
            pCardStatus->CardSpeed = atol(buf);
        }
        pCardStatus->NumIoFunction  = 0;
        /*DEV_TYPE_SD*/
        pCardStatus->SdType         = 2;
        pCardStatus->DataAfterErase = 1;
        pCardStatus->WriteProtect   = 0;

        status = 0;
    }

    return status;
}
