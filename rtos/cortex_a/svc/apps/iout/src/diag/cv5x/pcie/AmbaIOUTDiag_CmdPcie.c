/**
 *  @file AmbaIOUTDiag_CmdPcie.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details PCIe diagnostic Command
 */

#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif
#include <AmbaUtility.h>
#include <AmbaShell.h>
#include <AmbaIOUTDiag.h>
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AppPCIE.h"

static void AppPcie_Print(const char *pFmt)
{
    AmbaPrint_PrintUInt5(pFmt, 0, 0, 0, 0, 0);
    AmbaPrint_Flush();
}


#define UT_PCIE_NG            -1
#define UT_PCIE_NO_PROCESS    -2


static void PCIE_TestUsage(void)
{
    AppPcie_Print("PCIE Root Complex Test Command:");
    AppPcie_Print("  rc [item] : ");
    AppPcie_Print("        init       : setup driver and create data link");
    AppPcie_Print("        odma       : write data through DMA");
    AppPcie_Print("        idma       : read data through DMA");
    AppPcie_Print("        compl      : enter compliance mode for signal integrity test.");
    AppPcie_Print("        test       : execute programming sequence (used for simulation/debug)");
    AppPcie_Print("PCIE Endpoint Test Command:");
    AppPcie_Print("  ep [item] : ");
    AppPcie_Print("        init       : setup driver and create data link");
    AppPcie_Print("        odma       : write data through DMA");
    AppPcie_Print("        idma       : read data through DMA");
    AppPcie_Print("        test       : execute programming sequence (used for simulation/debug)");
}

static void AppPcie_TestCmd(UINT32 Argc, char * const * Argv)
{
    INT32 nret = 0;

    if (Argc >= 2U) {
        if (AmbaUtility_StringCompare(Argv[1], "rc", 2) == 0) {

            if (AmbaUtility_StringCompare(Argv[2], "init", 4) == 0) {
                AppPcie_RcInit();

            } else if (AmbaUtility_StringCompare(Argv[2], "odma", 4) == 0) {
                AppPcie_RcDmaWrite();

            } else if (AmbaUtility_StringCompare(Argv[2], "idma", 4) == 0) {
                AppPcie_RcDmaRead();

            } else if (AmbaUtility_StringCompare(Argv[2], "compl", 5) == 0) {
                extern void Impl_PcieInit(UINT32 mode, UINT32 gen, UINT32 lane, UINT32 use24mhz);
                extern void Impl_LinkStart(UINT32 mode, UINT32 gen);
                UINT32 value = 0;
                if (AmbaUtility_StringCompare(Argv[3], "gen1", 4) == 0) {
                    value = 0x0011; // enter compliance mode 2.5 GT/s
                    AppPcie_Print("[RC] select gen1 compliance mode");
                } else if (AmbaUtility_StringCompare(Argv[3], "gen2", 4) == 0) {
                    value = 0x0012; // enter compliance mode 5.0 GT/s
                    AppPcie_Print("[RC] select gen2 compliance mode");
                } else if (AmbaUtility_StringCompare(Argv[3], "gen3", 4) == 0) {
                    //value = 0x4413; // enter compliance mode 8.0 GT/s P4
                    value = 0x0013; // enter compliance mode 8.0 GT/s
                    AppPcie_Print("[RC] select gen3 compliance mode");
                } else {
                    AppPcie_Print("[RC][ERROR] Unknown compliance mode");
                }
                Impl_PcieInit(PCIE_RC_MODE, PCIE_CTRL_GEN, PCIE_LANE_WIDTH, PCIE_CLK_SRC);
                IO_UtilityRegWrite32(UT_PCIE_CFG_BASE+ 0xf0U,value);
                Impl_LinkStart(PCIE_RC_MODE, PCIE_CTRL_GEN);
                // readl 0x204000000f0
                // 0x10013 compliance mode
                // 0x10493 transmit margin
                // 0x1X493 preset
            } else if (AmbaUtility_StringCompare(Argv[2], "test", 4) == 0) {
                AppPCIE_ProgramRc();
            } else {
                nret = UT_PCIE_NO_PROCESS;
            }
        } else if (AmbaUtility_StringCompare(Argv[1], "ep", 2) == 0) {

            if (AmbaUtility_StringCompare(Argv[2], "init", 4) == 0) {
                AppPcie_EpInit();

            } else if (AmbaUtility_StringCompare(Argv[2], "odma", 4) == 0) {
                AppPcie_EpDmaWrite();

            } else if (AmbaUtility_StringCompare(Argv[2], "idma", 4) == 0) {
                AppPcie_EpDmaRead();

            } else if (AmbaUtility_StringCompare(Argv[2], "test", 4) == 0) {
                AppPCIE_ProgramEp();

            } else {
                nret = UT_PCIE_NO_PROCESS;
            }
        } else {
            nret = UT_PCIE_NO_PROCESS;
        }
    } else {
        nret = UT_PCIE_NG;
    }

    if (nret != 0) {
        PCIE_TestUsage();
    }

    return;
}

void AmbaIOUTDiag_CmdPcie(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    (VOID)PrintFunc;
    AppPcie_TestCmd(ArgCount, pArgVector);
}
