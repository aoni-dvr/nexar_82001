/**
 *  @file AppPCIE_RegSeq.c
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
 *  @details register programming seeuence for PCIe diagnostic
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

extern void Impl_PcieInit(UINT32 mode, UINT32 gen, UINT32 lane, UINT32 use24mhz);
extern void Impl_LinkStart(UINT32 mode, UINT32 gen);

#define writel IO_UtilityRegWrite32
#define readl IO_UtilityRegRead32

void AppPCIE_ProgramRc(void)
{
#define DESC_ADDR  0x25474000
#define DESC_ADDR_NEXT  0x25474000 + 0x40

    Impl_PcieInit(PCIE_RC_MODE, PCIE_CTRL_GEN, PCIE_LANE_WIDTH, PCIE_CLK_SRC);
    Impl_LinkStart(PCIE_RC_MODE, PCIE_CTRL_GEN);

#if 1
    writel(UT_PCIE_CFG_BASE+ 0x4,0x7);    ;
    //writel(UT_PCIE_CFG_BASE+ 0x100300, 0x1299d);
    //writel(UT_PCIE_CFG_BASE+ 0x10, 0x10000004);
    writel(UT_PCIE_CFG_BASE+ 0x100300, 0x1298C);
    writel(UT_PCIE_CFG_BASE+ 0x10, 0x73000004);
    writel(UT_PCIE_CFG_BASE+ 0x000000c8,0x00002810);
    writel(UT_PCIE_CFG_BASE+ 0x000000e8,0x00000000);
    writel(UT_PCIE_CFG_BASE+ 0x000000d0,0x80420003);
    writel(UT_PCIE_CFG_BASE+ 0x002000d0,0x00420003);
    writel(UT_PCIE_CFG_BASE+ 0x00100208,0x8000000c);
    writel(UT_PCIE_CFG_BASE+ 0x00000118,0x000000a0);

    writel(UT_PCIE_CFG_BASE+0x400008, 0x80000a); // config TLP // ob pcie decscriptor register 0 - region 0
    writel(UT_PCIE_CFG_BASE+0x40000C, 0x0); // ob pcie desc register 1
    writel(UT_PCIE_CFG_BASE+0x400000, 0x400013); //ob axi - pcie region 0  ob address translation, register 0
    writel(UT_PCIE_CFG_BASE+0x400004, 0x0); //63:32 bits of address, regitser 1
    writel(UT_PCIE_CFG_BASE+0x400018, 0x13); //axi region 0 base address reg0
    writel(UT_PCIE_CFG_BASE+0x40001C, 0x0); // 63:32 bits reg1

    writel(UT_PCIE_CFG_BASE+0x400028, 0x800002); // // ob pcie decscriptor register 0 - region 1
    writel(UT_PCIE_CFG_BASE+0x40002C, 0x0); // ob pcie desc register 1 - region 1
    writel(UT_PCIE_CFG_BASE+0x400020, 0x100013);
    writel(UT_PCIE_CFG_BASE+0x400024, 0x0); //63:32 bits of address, regitser 1
    writel(UT_PCIE_CFG_BASE+0x400038, 0x100013);
    writel(UT_PCIE_CFG_BASE+0x40003C, 0x0); // 63:32 bits reg1

    writel(UT_PCIE_CFG_BASE+ 0x400800,0x2540400D);  // BAR0
    //writel(UT_PCIE_CFG_BASE+ 0x400800 ,0x1f);
    writel(UT_PCIE_CFG_BASE+ 0x400804,0);
    writel(UT_PCIE_CFG_BASE+ 0x400808,0x1f);
    writel(UT_PCIE_CFG_BASE+ 0x40080C,0);
    writel(UT_PCIE_CFG_BASE+ 0x400810,0x1f);
    writel(UT_PCIE_CFG_BASE+ 0x400814,0);
#if 1
    writel(UT_PCIE_CFG_BASE+ 0x400018,0x0);
    writel(UT_PCIE_CFG_BASE+ 0x40001c,0x0);
    writel(UT_PCIE_CFG_BASE+ 0x400018,0x13);
    writel(UT_PCIE_CFG_BASE+ 0x400018,0x13);
    writel(UT_PCIE_CFG_BASE+ 0x400038,0x100000);
    writel(UT_PCIE_CFG_BASE+ 0x40003c,0x0);
    writel(UT_PCIE_CFG_BASE+ 0x400038,0x100013);
    writel(UT_PCIE_CFG_BASE+ 0x400038,0x100013);

    writel(UT_PCIE_CFG_BASE+ 0x400058,0x200000);
    writel(UT_PCIE_CFG_BASE+ 0x40005c,0x0);
    writel(UT_PCIE_CFG_BASE+ 0x400058,0x200013);
    writel(UT_PCIE_CFG_BASE+ 0x400058,0x200013);
    writel(UT_PCIE_CFG_BASE+ 0x400078,0x300000);
    writel(UT_PCIE_CFG_BASE+ 0x40007c,0x0);
    writel(UT_PCIE_CFG_BASE+ 0x400078,0x300013);
    writel(UT_PCIE_CFG_BASE+ 0x400078,0x300013);
    writel(UT_PCIE_CFG_BASE+ 0x400098,0x400000);
    writel(UT_PCIE_CFG_BASE+ 0x40009c,0x0);
    writel(UT_PCIE_CFG_BASE+ 0x400098,0x400013);
    writel(UT_PCIE_CFG_BASE+ 0x400098,0x400013);
    writel(UT_PCIE_CFG_BASE+ 0x4000b8,0x500000);
    writel(UT_PCIE_CFG_BASE+ 0x4000bc,0x0);
    writel(UT_PCIE_CFG_BASE+ 0x4000b8,0x500013);
    writel(UT_PCIE_CFG_BASE+ 0x4000b8,0x500013);
    writel(UT_PCIE_CFG_BASE+ 0x4000d8,0x600000);
    writel(UT_PCIE_CFG_BASE+ 0x4000dc,0x0);
    writel(UT_PCIE_CFG_BASE+ 0x4000d8,0x600013);
    writel(UT_PCIE_CFG_BASE+ 0x4000d8,0x600013);
    writel(UT_PCIE_CFG_BASE+ 0x4000f8,0x700000);
    writel(UT_PCIE_CFG_BASE+ 0x4000fc,0x0);
    writel(UT_PCIE_CFG_BASE+ 0x4000f8,0x700013);
    writel(UT_PCIE_CFG_BASE+ 0x4000f8,0x700013);
#endif
#else
    (void)AmbaPCIE_Rp_Init(UT_PCIE_RP_BASE, RP_OB_REGION0_AXI);
#endif


#if 1
    writel(UT_PCIE_AXI_BASE +0x10, 0xFFFFFFFF);
    writel(UT_PCIE_AXI_BASE +0x14, 0xFFFFFFFF);
    AmbaPrint_PrintUInt5("[before setting ] BAR = 0x%x.%x ", readl(UT_PCIE_AXI_BASE +0x14), readl(UT_PCIE_AXI_BASE +0x10), 0U, 0U, 0U);
    //program ep model
    writel(UT_PCIE_AXI_BASE +0x10, 0x100000);
    writel(UT_PCIE_AXI_BASE +0x14, 0x0); // need to set it even the upper address is 0s.
    writel(UT_PCIE_AXI_BASE +0x4, 0x7);
#else
    (void)AmbaPCIE_Rp_MemAccessToEp(0x100000, 0x100000, RP_OB_REGION1_BITS); // 16KB

#endif
#if 0
    //PCIE DMA interrupt enable - channel 0
    writel(UT_PCIE_CFG_BASE+ 0x6000a4,0x1);  //enable done intr
    writel(UT_PCIE_CFG_BASE+ 0x6000a4,0x100);  //enable error intr

    writel(UT_PCIE_AXI_BASE+0x100000, 0xfabedd1e);
    writel(UT_PCIE_AXI_BASE+0x100004, 0xffffcccc);
    writel(UT_PCIE_AXI_BASE+0x100008, 0x30002000);
    writel(UT_PCIE_AXI_BASE+0x10000c, 0x4000ffff);
    writel(UT_PCIE_AXI_BASE+0x100010, 0xeeeedddd);
    writel(UT_PCIE_AXI_BASE+0x100014, 0x50006000);
    writel(UT_PCIE_AXI_BASE+0x100018, 0xccccdddd);
    writel(UT_PCIE_AXI_BASE+0x10001C, 0x66556655);
    writel(UT_PCIE_AXI_BASE+0x100020, 0xccddeeff);

    for (i = 0; i < 4096; i++) {
        writel(0x25410000 + i * 4, 0xccddeeff + i);
    }
    //AmbaPrint_PrintUInt5("[CRC32] local = 0x%x ", IO_UtilityCrc32((UINT8*)0x25410000, 4096), 0U, 0U, 0U, 0U);


    //program the descriptors
    //pcie inbound bulk transfer
    //1st desc:
    //axi addr
    writel(DESC_ADDR, 0x25410000);
    writel(DESC_ADDR + 0x4, 0x0);
    writel(DESC_ADDR + 0x8, 0x00000000); //writel(DESC_ADDR + 0x8, 0x00000010);
    //pcie addr
    writel(DESC_ADDR + 0xC, 0x100000);
    writel(DESC_ADDR + 0x10, 0x0);
    //tlp attr
    writel(DESC_ADDR + 0x14, 0x00000000);//writel(DESC_ADDR + 0x14, 0xf1832100);
    //ctrl tlp
    //writel(DESC_ADDR + 0x18, 0x01001000);
    writel(DESC_ADDR + 0x18, 0x01000020); //writel(DESC_ADDR + 0x18, 0x21001000);
    writel(DESC_ADDR + 0x1c, 0x0);
    //pointer to next desc
    writel(DESC_ADDR + 0x20, 0);//writel(DESC_ADDR + 0x20, DESC_ADDR_NEXT);
    writel(DESC_ADDR + 0x24, 0);
#if 0
    //2nd desc:
    writel(DESC_ADDR_NEXT, 0x25420000);
    writel(DESC_ADDR_NEXT + 0x4, 0x0);
    writel(DESC_ADDR_NEXT + 0x8, 0x00000010);
    //pcie addr
    writel(DESC_ADDR_NEXT + 0xc, 0x100080);
    writel(DESC_ADDR_NEXT + 0x10, 0x0);
    //tlp attr
    writel(DESC_ADDR_NEXT + 0x14, 0xf1832100);
    //ctrl tlp
    writel(DESC_ADDR_NEXT + 0x18, 0x01000020); //writel(DESC_ADDR_NEXT + 0x18, 0x01001000);
    writel(DESC_ADDR_NEXT + 0x1c, 0x0);
#endif
    //pcie dma channel 0
    writel(UT_PCIE_CFG_BASE+ 0x60000c,0x0);  //lower attr
    writel(UT_PCIE_CFG_BASE+ 0x600010,0x0);  //upper attr
    //READ @0x600000 data:0
    readl(UT_PCIE_CFG_BASE+0x600000); //must return 0!?
    writel(UT_PCIE_CFG_BASE+ 0x600004,DESC_ADDR);  //start pointer lower reg
    writel(UT_PCIE_CFG_BASE+ 0x600008,0x0);  //start pointer upper reg
    writel(UT_PCIE_CFG_BASE+ 0x600000,0x1);  //ctrl reg - bulk inbound desc - Go command
    //writel(UT_PCIE_CFG_BASE+ 0x600000 ,0x3); //ctrl reg - bulk outbound desc - Go command

    AmbaKAL_TaskSleep(1000);

    //AmbaPrint_PrintUInt5("[CRC32] external = 0x%x ", IO_UtilityCrc32((UINT8*)UT_PCIE_AXI_BASE+0x100000, 4096), 0U, 0U, 0U, 0U);
#endif
}


void AppPCIE_ProgramEp(void)
{

    Impl_PcieInit(PCIE_EP_MODE, PCIE_CTRL_GEN, PCIE_LANE_WIDTH, PCIE_CLK_SRC);

    // setFuncBarControlSetting
    // setFuncBarApertureSetting
    writel(UT_PCIE_CFG_BASE+ 0x000000c8,0x00002810);
    //writel(UT_PCIE_CFG_BASE+0x100240, 0x5058FE7);
    writel(UT_PCIE_CFG_BASE+0x100240, 0x5058F8D);

    //inbound desc
    writel(UT_PCIE_CFG_BASE+0x400840, 0x25400000);
    writel(UT_PCIE_CFG_BASE+0x400844, 0x0);

    Impl_LinkStart(PCIE_EP_MODE, PCIE_CTRL_GEN);

    while((readl(UT_PCIE_CFG_BASE+0x4) & 0x6) != 0x6) {
        AmbaKAL_TaskSleep(1);
    }
#if 1
    //outbound desc
    writel(UT_PCIE_CFG_BASE+0x400028, 0x2); // // ob pcie decscriptor register 0 - region 0
    writel(UT_PCIE_CFG_BASE+0x40002C, 0x0); // ob pcie desc register 1 - region 0
    writel(UT_PCIE_CFG_BASE+0x400020, 0x7300000D); //ob axi - pcie region 1 ob address translation, register 0
    writel(UT_PCIE_CFG_BASE+0x400024, 0x0); //63:32 bits of address, regitser 1
    writel(UT_PCIE_CFG_BASE+0x400038, 0x200000D); //axi region 1 base address reg0
    writel(UT_PCIE_CFG_BASE+0x40003C, 0x0); // 63:32 bits reg1
#endif
    AmbaPrint_PrintUInt5("[EP] bus enabled",0,0,0,0,0);

    writel(UT_PCIE_AXI_BASE+0x2000000, 0xfabedd1e);
    writel(UT_PCIE_AXI_BASE+0x2000004, 0xffffcccc);
    writel(UT_PCIE_AXI_BASE+0x2000008, 0x30002000);
    writel(UT_PCIE_AXI_BASE+0x200000c, 0x4000ffff);
    writel(UT_PCIE_AXI_BASE+0x2000010, 0xeeeedddd);
    writel(UT_PCIE_AXI_BASE+0x2000014, 0x50006000);
    writel(UT_PCIE_AXI_BASE+0x2000018, 0xccccdddd);
    writel(UT_PCIE_AXI_BASE+0x200001C, 0x66556655);
    writel(UT_PCIE_AXI_BASE+0x2000020, 0xccddeeff);

    //WriteQWord(0x25400000, (UINT64)0xABABABABEFEFEFEF);
    //WriteQWord(UT_PCIE_AXI_BASE+0x2000000, (UINT64)0xABABABABEFEFEFEF);
}
