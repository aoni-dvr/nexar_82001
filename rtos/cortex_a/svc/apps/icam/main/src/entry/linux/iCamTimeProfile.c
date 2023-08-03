/**
*  @file iCamTimeProfile.c
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
*  @details icam time profile functions
*
*/

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "AmbaTypes.h"
#include "AmbaMemMap.h"
#include "AmbaMisraFix.h"

#include "SvcLog.h"
#include "SvcTiming.h"
#include "AmbaTimerInfo.h"
#include "iCamTimeProfile.h"

#define SVC_LOG_TMPR    "TMPR"

static AMBA_TIMER_INFO_s  *g_pTimerInfo;

/**
 *  Initialization of time profile
 *  @return none
 */
void iCamTimeProfile_Init(void)
{
    INT32   Fd;
    UINT32  MapSize;
    ULONG   PhyAddr = (AMBA_DRAM_RESERVED_PHYS_ADDR + AMBA_RAM_APPLICATION_SPECIFIC_OFFSET);
    ULONG   PageOff = PhyAddr & ~(sysconf(_SC_PAGE_SIZE) - 1U);
    ULONG   VirAddr;
    void    *pPageOff;

    MapSize = sizeof(AMBA_TIMER_INFO_s) + (PhyAddr - PageOff);

    if ((Fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        SvcLog_NG(SVC_LOG_TMPR, "fail to open /dev/mem", 0U, 0U);
    } else {
        if ((pPageOff = mmap(NULL, MapSize,
                             PROT_READ | PROT_WRITE,
                             MAP_SHARED,
                             Fd,
                             PageOff)) == MAP_FAILED) {
            SvcLog_NG(SVC_LOG_TMPR, "fail to mmap", 0U, 0U);
        } else {
            AmbaMisra_TypeCast(&VirAddr, &pPageOff);
            VirAddr += (PhyAddr - PageOff);
            AmbaMisra_TypeCast(&g_pTimerInfo, &VirAddr);

            //SvcLog_DBG(SVC_LOG_TMPR, "pPageOff/g_pTimerInfo(%p/%p)"
            //                       , (ULONG)pPageOff, (ULONG)g_pTimerInfo);

            iCamTimeProfile_Set(SVC_TIME_POSTOS_CONFIG);
        }

        close(Fd);
    }
}

/**
 *  Configuration of time profile
 *  @param[in] ProfileId profile id
 *  @return none
 */
void iCamTimeProfile_Set(UINT32 ProfileId)
{
    char    Result[32];
    UINT32  Time;
    FILE    *pFile = NULL;

    pFile = popen("cat /sys/module/ambarella_rct_profile/parameters/rct_timer1", "r");
    if (pFile != NULL) {
        if (fgets(Result, sizeof(Result), pFile) != NULL) {
            Time = strtoul(Result, NULL, 10);
            /* driver will divide 24, we roll back it */
            Time *= 24U;
            g_pTimerInfo->A53_APP_Time[ProfileId] = Time;

            //SvcLog_DBG(SVC_LOG_TMPR, "## ProfileId/Time(%u/%u)", ProfileId, Time);
        }

        if (pclose(pFile) == -1) {
             SvcLog_NG(SVC_LOG_TMPR, "fail to pclose", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_TMPR, "fail to popen", 0U, 0U);
    }
}

/**
 *  Get time profile information
 *  @param[in] ppTimeInfo pointer to timer information
 *  @return none
 */
void iCamTimeProfile_Get(void **ppTimeInfo)
{
    *ppTimeInfo = g_pTimerInfo;
}
