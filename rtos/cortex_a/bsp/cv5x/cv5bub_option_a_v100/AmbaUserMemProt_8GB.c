/**
 *  @file AmbaUserDefaultAttMap.c
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
 *  @details Example of Dram Att Map, Dram Att Client, and Dram Att Setup Function
 *
 */

#include "AmbaMisraFix.h"
#include "AmbaMemProt.h"
#include "bsp.h"

/* A region of dram att defines a flat address mapping with the same access permission */
#define NUM_USER_DRAM_ATT_REGION    0x1U

/* A dram client could protect continuous addresses in ATT. It could be among multiple memory regions mapping. */
#define USE_DSP_CLIENT
#define USE_CV_CLIENT

#define USER_DRAM_CLIENT_ID_DMA     (0U)
#define USER_DRAM_CLIENT_ID_GDMA    (1U)
#define USER_DRAM_CLIENT_ID_SD      (2U)

#ifdef USE_DSP_CLIENT
#define USER_DRAM_CLIENT_ID_DSP_0   (USER_DRAM_CLIENT_ID_SD + 1U)
#define USER_DRAM_CLIENT_ID_DSP_1   (USER_DRAM_CLIENT_ID_SD + 2U)
#define USER_DRAM_CLIENT_ID_DSP_2   (USER_DRAM_CLIENT_ID_SD + 3U)
#define USER_DRAM_CLIENT_ID_DSP_3   (USER_DRAM_CLIENT_ID_SD + 4U)
#define USER_DRAM_CLIENT_ID_DSP_4   (USER_DRAM_CLIENT_ID_SD + 5U)
#define USER_DRAM_CLIENT_ID_DSP_5   (USER_DRAM_CLIENT_ID_SD + 6U)
#else
#define USER_DRAM_CLIENT_ID_DSP_0   (USER_DRAM_CLIENT_ID_SD)
#define USER_DRAM_CLIENT_ID_DSP_1   (USER_DRAM_CLIENT_ID_SD)
#define USER_DRAM_CLIENT_ID_DSP_2   (USER_DRAM_CLIENT_ID_SD)
#define USER_DRAM_CLIENT_ID_DSP_3   (USER_DRAM_CLIENT_ID_SD)
#define USER_DRAM_CLIENT_ID_DSP_4   (USER_DRAM_CLIENT_ID_SD)
#define USER_DRAM_CLIENT_ID_DSP_5   (USER_DRAM_CLIENT_ID_SD)
#endif

#ifdef USE_CV_CLIENT
#define USER_DRAM_CLIENT_ID_CV_0    (USER_DRAM_CLIENT_ID_DSP_5 + 1U)
#define USER_DRAM_CLIENT_ID_CV_1    (USER_DRAM_CLIENT_ID_DSP_5 + 2U)
#define USER_DRAM_CLIENT_ID_CV_2    (USER_DRAM_CLIENT_ID_DSP_5 + 3U)
#else
#define USER_DRAM_CLIENT_ID_CV_0    (USER_DRAM_CLIENT_ID_DSP_5)
#define USER_DRAM_CLIENT_ID_CV_1    (USER_DRAM_CLIENT_ID_DSP_5)
#define USER_DRAM_CLIENT_ID_CV_2    (USER_DRAM_CLIENT_ID_DSP_5)
#endif

#define NUM_USER_DRAM_CLIENT        (USER_DRAM_CLIENT_ID_CV_2 + 1U)

/* Att Map */
static AMBA_DRAM_ATT_INFO_s UserAttInfo[NUM_USER_DRAM_ATT_REGION];

/* Client Info */
static AMBA_DRAM_ATT_CLIENT_INFO_s UserAttClientInfo[NUM_USER_DRAM_CLIENT];

static void CustomAddrTranslTable(void)
{
    const UINT32 *pAddr;
    UINT32 Addr = 0U;

    /* memory region 0 */
    AmbaMisra_TypeCast32(&Addr, &pAddr);
    UserAttInfo[0].VirtAddr = 0x0UL;
    UserAttInfo[0].PhysAddr = 0x0UL;
    UserAttInfo[0].Size = 0x200000000UL;        /* Map 8GB by default */

    UserAttClientInfo[USER_DRAM_CLIENT_ID_GDMA].ClientID = AMBA_DRAM_CLIENT_GDMA;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_GDMA].VirtAddr = 0x100000000UL;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_GDMA].Size = 0x100000000UL;

    UserAttClientInfo[USER_DRAM_CLIENT_ID_DMA].ClientID = AMBA_DRAM_CLIENT_DMA1;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_DMA].VirtAddr = 0x100000000UL;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_DMA].Size = 0x100000000UL;

#ifdef USE_DSP_CLIENT
    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_0].ClientID = AMBA_DRAM_CLIENT_ORCME0;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_0].VirtAddr = 0x100000000UL;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_0].Size = 0x100000000UL;

    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_1].ClientID = AMBA_DRAM_CLIENT_ORCCODE0;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_1].VirtAddr = 0x100000000UL;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_1].Size = 0x100000000UL;

    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_2].ClientID = AMBA_DRAM_CLIENT_ORCME1;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_2].VirtAddr = 0x100000000UL;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_2].Size = 0x100000000UL;

    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_3].ClientID = AMBA_DRAM_CLIENT_ORCCODE1;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_3].VirtAddr = 0x100000000UL;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_3].Size = 0x100000000UL;

    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_4].ClientID = AMBA_DRAM_CLIENT_SMEM_WR;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_4].VirtAddr = 0x100000000UL;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_4].Size = 0x100000000UL;

    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_5].ClientID = AMBA_DRAM_CLIENT_SMEM_RD;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_5].VirtAddr = 0x100000000UL;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_DSP_5].Size = 0x100000000UL;
#endif

#ifdef USE_CV_CLIENT
    UserAttClientInfo[USER_DRAM_CLIENT_ID_CV_0].ClientID = AMBA_DRAM_CLIENT_ORCVP;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_CV_0].VirtAddr = 0x100000000UL;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_CV_0].Size = 0x100000000UL;

    UserAttClientInfo[USER_DRAM_CLIENT_ID_CV_1].ClientID = AMBA_DRAM_CLIENT_ORCL2;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_CV_1].VirtAddr = 0x100000000UL;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_CV_1].Size = 0x100000000UL;

    UserAttClientInfo[USER_DRAM_CLIENT_ID_CV_2].ClientID = AMBA_DRAM_CLIENT_VMEM0;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_CV_2].VirtAddr = 0x100000000UL;
    UserAttClientInfo[USER_DRAM_CLIENT_ID_CV_2].Size = 0x100000000UL;
#endif
}

/**
 *  AmbaMemProt_Config - Configure memory protection
 *  @param[in] EnableFlag enable memory protection or not
 */
void AmbaMemProt_Config(UINT32 EnableFlag)
{
    UINT32 i;

    if (EnableFlag != 0U) {
        /* Prepare customized address translation table for memory protection */
        CustomAddrTranslTable();

        /* Apply memory region definitions */
        (void)AmbaMemProt_Init(NUM_USER_DRAM_ATT_REGION, UserAttInfo);

        /* Enable memory protection for each dram client */
        for (i = 0U; i < NUM_USER_DRAM_CLIENT; i++) {
            (void)AmbaMemProt_Enable(UserAttClientInfo[i].ClientID, UserAttClientInfo[i].VirtAddr, UserAttClientInfo[i].Size);
        }
    }
}
