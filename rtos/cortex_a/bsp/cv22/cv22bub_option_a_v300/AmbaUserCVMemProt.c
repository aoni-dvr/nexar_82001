/**
 *  @file AmbaUserCVMemProt.c
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

//#define CLIENT_KNOW_ALL_MEM

#define ATT_CV_START_VIRT_ADDR            (0x80000000U)

/* A region of dram att defines a flat address mapping with the same access permission */
#define ATT_REGN_ID_CV_MEM                  (0x0U)  /* cv memory */
#define NUM_USER_DRAM_ATT_REGION            (0x1U)

/* A dram client could protect continuous addresses in ATT. It could be among multiple memory regions mapping. */
#define ATT_CLI_ID_ORCVP                    (0x0U)
#define ATT_CLI_ID_ORCL2                    (0x1U)
#define ATT_CLI_ID_VMEM                     (0x2U)
#define NUM_USER_DRAM_CLIENT                (0x3U)

extern void AmbaUserDramAttInit(void);

/* Att Map */
static AMBA_DRAM_ATT_INFO_s UserAttInfo[NUM_USER_DRAM_ATT_REGION];

/* Client Info */
static AMBA_DRAM_ATT_CLIENT_INFO_s UserAttClientInfo[NUM_USER_DRAM_CLIENT];

static void CustomAddrTranslTable(void)
{
    extern UINT32 __cv_start, __cv_end;
    const UINT32 *pU32;
    UINT32 PhysStartAddr, PhysEndAddr;
    UINT32 StartVirtAddr, PoolSize = 0U;

    /* Config Region one by one */
    // Region0 : CV
    StartVirtAddr = ATT_CV_START_VIRT_ADDR + PoolSize;
    pU32 = &__cv_start;
    AmbaMisra_TypeCast32(&PhysStartAddr, &pU32);
    pU32 = &__cv_end;
    AmbaMisra_TypeCast32(&PhysEndAddr, &pU32);
#ifdef CLIENT_KNOW_ALL_MEM
    UserAttInfo[ATT_REGN_ID_CV_MEM].VirtAddr = 0x0U;
    UserAttInfo[ATT_REGN_ID_CV_MEM].PhysAddr = 0x0U;
    UserAttInfo[ATT_REGN_ID_CV_MEM].Size = AMBA_DRAM_ATT_MAP_SIZE;
#else
    UserAttInfo[ATT_REGN_ID_CV_MEM].VirtAddr = StartVirtAddr;
    UserAttInfo[ATT_REGN_ID_CV_MEM].PhysAddr = PhysStartAddr;
    UserAttInfo[ATT_REGN_ID_CV_MEM].Size = ((UINT64)PhysEndAddr - (UINT64)PhysStartAddr);
#endif
    PoolSize += (UINT32)UserAttInfo[ATT_REGN_ID_CV_MEM].Size;

    /* Config Client one by one */
    UserAttClientInfo[ATT_CLI_ID_ORCVP].ClientID = AMBA_DRAM_CLIENT_ORCVP;
#ifdef CLIENT_KNOW_ALL_MEM
    UserAttClientInfo[ATT_CLI_ID_ORCVP].VirtAddr = 0x0U;
    UserAttClientInfo[ATT_CLI_ID_ORCVP].Size = AMBA_DRAM_ATT_MAP_SIZE;
#else
    UserAttClientInfo[ATT_CLI_ID_ORCVP].VirtAddr = ATT_CV_START_VIRT_ADDR;
    UserAttClientInfo[ATT_CLI_ID_ORCVP].Size = PoolSize;
#endif

    UserAttClientInfo[ATT_CLI_ID_ORCL2].ClientID = DRAM_CLIENT_ORCL2;
#ifdef CLIENT_KNOW_ALL_MEM
    UserAttClientInfo[ATT_CLI_ID_ORCL2].VirtAddr = 0x0U;
    UserAttClientInfo[ATT_CLI_ID_ORCL2].Size = AMBA_DRAM_ATT_MAP_SIZE;
#else
    UserAttClientInfo[ATT_CLI_ID_ORCL2].VirtAddr = ATT_CV_START_VIRT_ADDR;
    UserAttClientInfo[ATT_CLI_ID_ORCL2].Size = PoolSize;
#endif

    UserAttClientInfo[ATT_CLI_ID_VMEM].ClientID = DRAM_CLIENT_VMEM;
#ifdef CLIENT_KNOW_ALL_MEM
    UserAttClientInfo[ATT_CLI_ID_VMEM].VirtAddr = 0x0U;
    UserAttClientInfo[ATT_CLI_ID_VMEM].Size = AMBA_DRAM_ATT_MAP_SIZE;
#else
    UserAttClientInfo[ATT_CLI_ID_VMEM].VirtAddr = ATT_CV_START_VIRT_ADDR;
    UserAttClientInfo[ATT_CLI_ID_VMEM].Size = PoolSize;
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
