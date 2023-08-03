/**
 *  @file AmbaShell_IoStat.c
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
 *  @details Dump memory data to storage
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaMisraFix.h>
#include <AmbaShell.h>
#include "AmbaWrap.h"
#include <AmbaFS.h>
//#include "AmbaFS_Format.h"
#include <AmbaUtility.h>
#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"
#include "AmbaSD_Ctrl.h"

#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/statvfs.h>
#include <sys/dcmd_cam.h>
#include "hw/dcmd_sim_mmcsd.h"
#include "hw/dcmd_sim_sdmmc.h"

static void SHELL_IoStatCmdUsage(char *const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [show|enable] [sd0|sd1|sd2]: \n");
    PrintFunc("         [show]: show io stat\n");
    PrintFunc("         [enable]: enable io stat\n");
    PrintFunc("         [disable]: disable io stat\n");
    PrintFunc("         [sdx]: The sd channel\n");
    PrintFunc("\n");
}

/**
 * Get 2nd IO statistics data (for low level debug).
 */
static INT32 iostat_get(UINT64 *rb, UINT64 *wb, UINT32 *rt, UINT32 *wt,
                        UINT32 *rlvl, UINT32 *wlvl, const io_statistics_t *pIoStat)
{
    INT32 sret = 0;
    *rb = pIoStat->rd_bytes;
    *wb = pIoStat->wr_bytes;
    *rt = pIoStat->rd_time;
    *wt = pIoStat->wr_time;
    if (AmbaWrap_memcpy(rlvl, pIoStat->rlvl, PRF2_IOSTAT_MAX_LVL * sizeof(UINT32)) != 0U) {
        sret = 1;
    } else {
        if (AmbaWrap_memcpy(wlvl, pIoStat->wlvl, PRF2_IOSTAT_MAX_LVL * sizeof(UINT32)) != 0U) {
            sret = 1;
        }
    }

    return sret;
}

static void dump_iostat_diagram(const UINT32 *lvl, UINT32 bar_num, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i, j;
    UINT32 lvlcnt;
    UINT32 totalsecs = 0;
    UINT32 bar_num_x = bar_num;

    if (bar_num_x == 0U) {
        bar_num_x = 100U;    /* set default bar number */
    }

    for (i = 0; i < PRF2_IOSTAT_MAX_LVL; i++) {
        totalsecs += lvl[i];
    }

    if (totalsecs < bar_num_x) {
        PrintFunc("No diagram\n");
    } else {
        for (i = 0; i < PRF2_IOSTAT_MAX_LVL; i++) {
            lvlcnt = lvl[i] * bar_num_x / totalsecs;
            PrintFunc("[--");
            AmbaShell_PrintUInt32(1UL << i, PrintFunc);
            PrintFunc(" secs] ");
            for (j = 0; j < lvlcnt; j++) {
                PrintFunc("*");
            }
            PrintFunc(" [");
            AmbaShell_PrintUInt32(lvl[i], PrintFunc);
            PrintFunc("]\n");
        }
    }
}

/**
 *  AmbaShell_CommandSaveBinary - execute command save bin
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandIoStat(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT64 rb, wb;
    UINT32 rt, wt;
    UINT32 rl[PRF2_IOSTAT_MAX_LVL];
    UINT32 wl[PRF2_IOSTAT_MAX_LVL];
    UINT32 SdChanNo;
    INT32 fd;

    if (ArgCount < 2U) {
        SHELL_IoStatCmdUsage(pArgVector, PrintFunc);
    } else {
        io_statistics_t IoStat = {0};
        if ((AmbaUtility_StringCompare(pArgVector[ArgCount - 1U], "sd0", 3U) == 0)) {
            SdChanNo = AMBA_SD_CHANNEL0;
            fd = open("/dev/hd0", O_RDWR);
        } else if ((AmbaUtility_StringCompare(pArgVector[ArgCount - 1U], "sd1", 3U) == 0)) {
            SdChanNo = AMBA_SD_CHANNEL1;
            fd = open("/dev/hd1", O_RDWR);
        } else {
            SdChanNo = AMBA_SD_CHANNEL0;
            fd = open("/dev/hd0", O_RDWR);
        }

        IoStat.enable = 1U;

        if (0 == AmbaUtility_StringCompare(pArgVector[1], "enable", 6U)) {

            (void)devctl(fd, DCMD_SDMMC_IOSTAT_INFO, &IoStat, sizeof(io_statistics_t), NULL);
            (void)close(fd);

            PrintFunc("IoStat enable : SD");
            AmbaShell_PrintUInt32(SdChanNo, PrintFunc);
            PrintFunc("\n");

        } else if (0 == AmbaUtility_StringCompare(pArgVector[1], "show", 4U)) {
            (void)devctl(fd, DCMD_SDMMC_IOSTAT_INFO, &IoStat, sizeof(io_statistics_t), NULL);
            (void)close(fd);

            if (iostat_get(&rb, &wb, &rt, &wt, rl, wl, &IoStat) == 0) {

                PrintFunc("Read diagram:\n");
                PrintFunc("-----------------------------------\n");
                dump_iostat_diagram(rl, 0, PrintFunc);
                PrintFunc("-----------------------------------\n\n");

                PrintFunc("Write diagram:\n");
                PrintFunc("-----------------------------------\n");
                dump_iostat_diagram(wl, 0, PrintFunc);
                PrintFunc("-----------------------------------\n\n");

                PrintFunc("IO statistics:\n");
                PrintFunc("-----------------------------------\n");

                PrintFunc("bytes read: ");
                AmbaShell_PrintUInt64(rb, PrintFunc);
                PrintFunc("\n");

                PrintFunc("read time elapsed: ");
                AmbaShell_PrintUInt32(rt, PrintFunc);
                PrintFunc(" ms\n");

                PrintFunc("bytes written: ");
                AmbaShell_PrintUInt64(wb, PrintFunc);
                PrintFunc("\n");

                PrintFunc("write time elapsed: ");
                AmbaShell_PrintUInt32(wt, PrintFunc);
                PrintFunc(" ms\n");

                PrintFunc("Read throughput: ");
                AmbaShell_PrintUInt64((((8UL *rb) / rt) / 1000U), PrintFunc);
                PrintFunc(" Mbit/sec\n");

                PrintFunc("Write throughput: ");
                AmbaShell_PrintUInt64((((8UL *wb) / wt) / 1000U), PrintFunc);
                PrintFunc(" Mbit/sec\n");

                PrintFunc("-----------------------------------\n");
            } else {
                PrintFunc("failed to get statistics!\n");
            }
        } else {
            // pass misra-c checking
        }
    }
    return;
}

