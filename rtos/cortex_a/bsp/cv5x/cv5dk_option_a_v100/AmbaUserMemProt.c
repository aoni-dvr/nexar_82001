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
#define NUM_USER_DRAM_CLIENT        0x1U

extern UINT32 __ddr_resv_start;

/* Att Map */
static AMBA_DRAM_ATT_INFO_s UserAttInfo[NUM_USER_DRAM_ATT_REGION];

/* Client Info */
static AMBA_DRAM_ATT_CLIENT_INFO_s UserAttClientInfo[NUM_USER_DRAM_CLIENT];

static void CustomAddrTranslTable(void)
{
    extern UINT32 __ddr_resv_start;
    const UINT32 *pAddr;
    UINT32 Addr = 0U;

    /* memory region 0 */
    pAddr = &__ddr_resv_start;
    AmbaMisra_TypeCast(&Addr, &pAddr);
    UserAttInfo[0].VirtAddr = 0x0UL;
    UserAttInfo[0].PhysAddr = Addr;
    UserAttInfo[0].Size = AMBA_DRAM_ATT_MAP_SIZE;

    /* dram client 0 */
    UserAttClientInfo[0].ClientID = AMBA_DRAM_CLIENT_DMA0;
    UserAttClientInfo[0].VirtAddr = 0x0UL;
    UserAttClientInfo[0].Size = 0x100000000UL;
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
