/**
 *  @file AmbaUserIdspMemProt.c
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

//#define CLIENT_KNOW_ALL_MEM

#define ATT_IDSP_START_VIRT_ADDR            (0x80000000U)

/* A region of dram att defines a flat address mapping with the same access permission */
#define ATT_REGN_ID_DSP_BINARY              (0x0U)  /* dsp binary */
#define ATT_REGN_ID_DSP_PRTCL_BUF           (0x1U)  /* DspBuf */
#define ATT_REGN_ID_DSP_DATA_BUF            (0x2U)  /* DspDbgLog+DspWork+DspData */
#define NUM_USER_DRAM_ATT_REGION            (0x3U)

/* A dram client could protect continuous addresses in ATT. It could be among multiple memory regions mapping. */
#define ATT_CLI_ID_ORCCODE                  (0x0U)
#define ATT_CLI_ID_ORCME                    (0x1U)
#define ATT_CLI_ID_SMEM                     (0x2U)
#define NUM_USER_DRAM_CLIENT                (0x3U)

extern void AmbaUserDramAttInit(void);

/* Att Map */
static AMBA_DRAM_ATT_INFO_s UserAttInfo[NUM_USER_DRAM_ATT_REGION];

/* Client Info */
static AMBA_DRAM_ATT_CLIENT_INFO_s UserAttClientInfo[NUM_USER_DRAM_CLIENT];

static void CustomAddrTranslTable(void)
{
    extern UINT32 __ucode_start, __ucode_end;
    extern UINT32 __dsp_cache_buf_start, __dsp_buf_end;
    extern UINT32 __dsp_data_start, __dsp_data_end;
    const UINT32 *pU32;
    UINT32 PhysStartAddr, PhysEndAddr;
    UINT32 StartVirtAddr, PoolSize = 0U;

    /* Config Region one by one */
    // Region0 : DspBinary
    StartVirtAddr = ATT_IDSP_START_VIRT_ADDR + PoolSize;
    pU32 = &__ucode_start;
    AmbaMisra_TypeCast32(&PhysStartAddr, &pU32);
    pU32 = &__ucode_end;
    AmbaMisra_TypeCast32(&PhysEndAddr, &pU32);
#ifdef CLIENT_KNOW_ALL_MEM
    UserAttInfo[ATT_REGN_ID_DSP_BINARY].VirtAddr = 0x0U;
    UserAttInfo[ATT_REGN_ID_DSP_BINARY].PhysAddr = 0x0U;
    UserAttInfo[ATT_REGN_ID_DSP_BINARY].Size = AMBA_DRAM_ATT_MAP_SIZE;
#else
    UserAttInfo[ATT_REGN_ID_DSP_BINARY].VirtAddr = StartVirtAddr;
    UserAttInfo[ATT_REGN_ID_DSP_BINARY].PhysAddr = PhysStartAddr;
    UserAttInfo[ATT_REGN_ID_DSP_BINARY].Size = ((UINT64)PhysEndAddr - (UINT64)PhysStartAddr);
#endif
    PoolSize += (UINT32)UserAttInfo[ATT_REGN_ID_DSP_BINARY].Size;

    // Region1 : DspProtocol buf
    StartVirtAddr = ATT_IDSP_START_VIRT_ADDR + PoolSize;
    pU32 = &__dsp_cache_buf_start;
    AmbaMisra_TypeCast32(&PhysStartAddr, &pU32);
    pU32 = &__dsp_buf_end;
    AmbaMisra_TypeCast32(&PhysEndAddr, &pU32);
#ifdef CLIENT_KNOW_ALL_MEM
    UserAttInfo[ATT_REGN_ID_DSP_PRTCL_BUF].VirtAddr = 0x0U;
    UserAttInfo[ATT_REGN_ID_DSP_PRTCL_BUF].PhysAddr = 0x0U;
    UserAttInfo[ATT_REGN_ID_DSP_PRTCL_BUF].Size = AMBA_DRAM_ATT_MAP_SIZE;
#else
    UserAttInfo[ATT_REGN_ID_DSP_PRTCL_BUF].VirtAddr = StartVirtAddr;
    UserAttInfo[ATT_REGN_ID_DSP_PRTCL_BUF].PhysAddr = PhysStartAddr;
    UserAttInfo[ATT_REGN_ID_DSP_PRTCL_BUF].Size = ((UINT64)PhysEndAddr - (UINT64)PhysStartAddr);
#endif
    PoolSize += (UINT32)UserAttInfo[ATT_REGN_ID_DSP_PRTCL_BUF].Size;

    // Region2 : DspDbgLog+DspWork+DspData
    StartVirtAddr = ATT_IDSP_START_VIRT_ADDR + PoolSize;
    pU32 = &__dsp_data_start;
    AmbaMisra_TypeCast32(&PhysStartAddr, &pU32);
    pU32 = &__dsp_data_end;
    AmbaMisra_TypeCast32(&PhysEndAddr, &pU32);
#ifdef CLIENT_KNOW_ALL_MEM
    UserAttInfo[ATT_REGN_ID_DSP_DATA_BUF].VirtAddr = 0x0U;
    UserAttInfo[ATT_REGN_ID_DSP_DATA_BUF].PhysAddr = 0x0U;
    UserAttInfo[ATT_REGN_ID_DSP_DATA_BUF].Size = AMBA_DRAM_ATT_MAP_SIZE;
#else
    UserAttInfo[ATT_REGN_ID_DSP_DATA_BUF].VirtAddr = StartVirtAddr;
    UserAttInfo[ATT_REGN_ID_DSP_DATA_BUF].PhysAddr = PhysStartAddr;
    UserAttInfo[ATT_REGN_ID_DSP_DATA_BUF].Size = ((UINT64)PhysEndAddr - (UINT64)PhysStartAddr);
#endif
    PoolSize += (UINT32)UserAttInfo[ATT_REGN_ID_DSP_DATA_BUF].Size;

    /* Config Client one by one */
    UserAttClientInfo[ATT_CLI_ID_ORCCODE].ClientID = AMBA_DRAM_CLIENT_ORCCODE;
#ifdef CLIENT_KNOW_ALL_MEM
    UserAttClientInfo[ATT_CLI_ID_ORCCODE].VirtAddr = 0x0U;
    UserAttClientInfo[ATT_CLI_ID_ORCCODE].Size = AMBA_DRAM_ATT_MAP_SIZE;
#else
    UserAttClientInfo[ATT_CLI_ID_ORCCODE].VirtAddr = ATT_IDSP_START_VIRT_ADDR;
    UserAttClientInfo[ATT_CLI_ID_ORCCODE].Size = PoolSize;
#endif

    UserAttClientInfo[ATT_CLI_ID_ORCME].ClientID = DRAM_CLIENT_ORCME;
#ifdef CLIENT_KNOW_ALL_MEM
    UserAttClientInfo[ATT_CLI_ID_ORCME].VirtAddr = 0x0U;
    UserAttClientInfo[ATT_CLI_ID_ORCME].Size = AMBA_DRAM_ATT_MAP_SIZE;
#else
    UserAttClientInfo[ATT_CLI_ID_ORCME].VirtAddr = ATT_IDSP_START_VIRT_ADDR;
    UserAttClientInfo[ATT_CLI_ID_ORCME].Size = PoolSize;
#endif

    UserAttClientInfo[ATT_CLI_ID_SMEM].ClientID = DRAM_CLIENT_SMEM;
#ifdef CLIENT_KNOW_ALL_MEM
    UserAttClientInfo[ATT_CLI_ID_SMEM].VirtAddr = 0x0U;
    UserAttClientInfo[ATT_CLI_ID_SMEM].Size = AMBA_DRAM_ATT_MAP_SIZE;
#else
    UserAttClientInfo[ATT_CLI_ID_SMEM].VirtAddr = ATT_IDSP_START_VIRT_ADDR;
    UserAttClientInfo[ATT_CLI_ID_SMEM].Size = PoolSize;
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
