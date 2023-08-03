/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
 * Copyright 2020, Ambarella International LP
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include <stdio.h>
#include <string.h>
#include "diag.h"

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"

#include "AmbaKAL.h"
#include "AmbaWrap.h"

#include "AmbaDMA.h"
#include "AmbaCache.h"

#define DIAG_DMA_AMOUNT     (0x10000000U)
#define DIAG_NUM_DMA_DATA   (1024)
#define DIAG_DMA_GAURDBAND  (1024U)
#define DIAG_NUM_DESC       (256U)

void get_diag_work_buf(ULONG *pAddr, UINT32 *pSize)
{
    int32_t Rval = 0;
    struct posix_typed_mem_info info;
    void *virt_addr;
    int MemFd;

    MemFd = posix_typed_mem_open("/ram/diag_work", O_RDWR, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
    if (MemFd < 0) {
        printf("[%s] ut_get_dsp_work_buf() : posix_typed_mem_open fail(/ram/diag_work) fd = %d", __FUNCTION__, MemFd);
    } else {
        Rval = posix_typed_mem_get_info(MemFd, &info);
        if (Rval < 0) {
            printf("[%s] ut_get_dsp_work_buf() : posix_typed_mem_get_info fail Rval = %d", __FUNCTION__, Rval);
        } else {
            virt_addr = mmap(NULL, info.posix_tmi_length, PROT_READ | PROT_WRITE, MAP_SHARED, MemFd, 0);
            if (virt_addr == MAP_FAILED) {
                printf("[%s] ut_get_dsp_work_buf() : mmap fail", __FUNCTION__);
            } else {
                (void)AmbaMisra_TypeCast(pAddr, &virt_addr);
                *pSize = info.posix_tmi_length;
            }
        }
    }
}

typedef struct {
    ULONG       Base;       /* Virtual Address */
    uint32_t    PhysBase;   /* Physical Address, u32 only */
    uint32_t    size;
    uint32_t    UsedSize;
    uint32_t    is_cached;
} audio_buf_pool_t;

ULONG ADDR_ALIGN_NUM(ULONG Addr, ULONG Num)
{
    return (Addr + (Num - (Addr & (Num - 1U))));
}

static audio_buf_pool_t audio_buf_pool = {0};
static int32_t alloc_fd_audio_dma = 0;
/* allocate audio dma descriptor and work buffer */
uint32_t dma_alloc_addr(void)
{
    int32_t Rval = 0;
    struct posix_typed_mem_info info;
    void *virt_addr;
    off_t offset;

    /* From experience, first time use ALLOC_CONTIG, following use MAP_ALLOCATABLE */
    alloc_fd_audio_dma = posix_typed_mem_open("/ram/diag_work", O_RDWR, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
    if (alloc_fd_audio_dma < 0) {
        printf("[ERR] dma_alloc_addr() : posix_typed_mem_open fail(/ram/diag_work) fd = %d errno = %d\n", alloc_fd_audio_dma, errno);
    } else {
        Rval = posix_typed_mem_get_info(alloc_fd_audio_dma, &info);
        if (Rval < 0) {
            printf("[ERR] dma_alloc_addr() : posix_typed_mem_get_info fail ret = %d errno = %d\n", Rval, errno);
        } else {
            virt_addr = mmap(NULL, info.posix_tmi_length, PROT_NOCACHE | PROT_READ | PROT_WRITE, MAP_SHARED, alloc_fd_audio_dma, 0);
            if (virt_addr == MAP_FAILED) {
                printf("[ERR] dma_alloc_addr() : mmap fail errno = %d\n", errno);
            } else {
                Rval = mem_offset(virt_addr, NOFD, 1, &offset, 0);
                if (Rval < 0) {
                    printf("[ERR] dma_alloc_addr() : mem_offset fail ret = %d errno = %d\n", Rval, errno);
                } else {
                    /* update buf_pool */
                    memcpy(&audio_buf_pool.Base, &virt_addr, sizeof(void *));
                    audio_buf_pool.size = (uint32_t)info.posix_tmi_length;
                    audio_buf_pool.UsedSize = 0U;
                    audio_buf_pool.is_cached = 0U;
                    memcpy(&audio_buf_pool.PhysBase, &offset, sizeof(uint32_t));
                    printf("[audio_dma] mempool Virt %lx, Phys 0x%X, Sz %d Cached %d\n",
                           audio_buf_pool.Base,
                           audio_buf_pool.PhysBase,
                           audio_buf_pool.size,
                           audio_buf_pool.is_cached);
                }
            }
        }
    }

    return Rval;
}

uint32_t dma_alloc_buf(ULONG *pBase, uint32_t ReqSize, uint32_t Align)
{
    uint32_t Rval = 0;
    uint32_t NeededSize;
    uint32_t Offset;
    ULONG CurBase;

    if ((pBase == NULL) || (ReqSize == 0U)) {
        Rval = 0xffffffff;
        printf("[ERR] dma_alloc_buf : Null address or Zero size\n");
    } else {
        /* Get dsp protocol mem range */
        if (audio_buf_pool.Base == 0UL) {
            Rval = dma_alloc_addr();
        }

        if (Rval == 0) {
            CurBase = audio_buf_pool.Base + (ULONG)audio_buf_pool.UsedSize;
            Offset = (uint32_t)(ADDR_ALIGN_NUM(CurBase, (ULONG)Align) - CurBase);
            NeededSize = Offset + ReqSize;
            if (((CurBase + (ULONG)NeededSize) - audio_buf_pool.Base) > (ULONG)audio_buf_pool.size) {
                printf("[ERR] dma_alloc_buf : Over audio dma pool size, %d > %d\n", (ReqSize), audio_buf_pool.size);
                Rval = 0xffffffff;
            } else {
                *pBase = CurBase + Offset;

                /* Update pool */
                audio_buf_pool.UsedSize += NeededSize;
            }
        }
    }

    return Rval;
}

static void Diag_PrintFormattedInt(const char *pFmtString, UINT32 Value)
{
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    printf(pFmtString, ArgUINT32[0]);
}

static void Diag_DmaCmdUsage()
{
    printf("Usage: dma unittest                               - Sanity testing by dram-to-dram transfer\n");
    printf("       dma dsunittest                             - Sanity testing by dram-to-dram transfer in descriptor mode\n");
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    printf("       dma stat <reset>                           - Get/reset dma interruput statistics\n");
#endif
}

static UINT32 Diag_DmaDram2Dram(UINT32 DmaChan, const UINT8 *pSrc, const UINT8 *pDst, UINT32 Len)
{
    AMBA_DMA_DESC_s DmaDesc;
    UINT32 DmaStatus;
    ULONG SrcAddr, DstAddr;

    AmbaMisra_TypeCast(&DmaDesc.pSrcAddr, &pSrc);
    AmbaMisra_TypeCast(&DmaDesc.pDstAddr, &pDst);
    DmaDesc.pNextDesc = NULL;
    DmaDesc.pStatus = NULL;
    DmaDesc.DataSize = Len;
    DmaDesc.Ctrl.StopOnError = 1U;
    DmaDesc.Ctrl.IrqOnError = 1U;
    DmaDesc.Ctrl.IrqOnDone = 1U;
    DmaDesc.Ctrl.Reserved0 = 0U;
    DmaDesc.Ctrl.BusBlockSize = AMBA_DMA_BUS_BLOCK_32BYTE;
    DmaDesc.Ctrl.BusDataSize = AMBA_DMA_BUS_DATA_4BYTE;
    DmaDesc.Ctrl.NoBusAddrInc = 1U;
    DmaDesc.Ctrl.ReadMem = 1U;
    DmaDesc.Ctrl.WriteMem = 1U;
    DmaDesc.Ctrl.EndOfChain = 1U;
    DmaDesc.Ctrl.Reserved1 = 0U;

    /* Push CACHE data to DRAM */
    AmbaMisra_TypeCast(&SrcAddr, &pSrc);
    AmbaMisra_TypeCast(&DstAddr, &pDst);
    (void)AmbaCache_DataFlush(SrcAddr, Len);
    (void)AmbaCache_DataInvalidate(DstAddr, Len);

    DmaStatus = AmbaDMA_Transfer(DmaChan, &DmaDesc);
    (void)AmbaDMA_Wait(DmaChan, 5000U);
    (void)AmbaCache_DataInvalidate(DstAddr, Len);

    return DmaStatus;
}

static void Diag_DmaUnitTest()
{
    UINT8 *pSrc, *pDst;
    ULONG SrcAddr, DstAddr, DataSize = DIAG_NUM_DMA_DATA / 2U;
    UINT32 i, j, RetVal = 0;

    get_diag_work_buf(&SrcAddr, &i);
    DstAddr = SrcAddr + (DIAG_NUM_DMA_DATA / 2U);
    AmbaMisra_TypeCast(&pSrc, &SrcAddr);
    AmbaMisra_TypeCast(&pDst, &DstAddr);

    pSrc[0] = 0x55U;
    for (j = 1U; j < DataSize; j ++) {
        pSrc[j] = ~pSrc[j - 1U];
    }
    (void)AmbaCache_DataFlush(SrcAddr, DataSize);

    for (i = 0; i < AMBA_NUM_DMA_CHANNEL; i ++) {
        printf("[Diag][DMA] DMA channel ");
        Diag_PrintFormattedInt("%u,", i);

        for (j = 0; j < DataSize; j ++) {
            pDst[j] = 0x00U;
        }
        (void)AmbaCache_DataFlush(DstAddr, DataSize);

        RetVal = Diag_DmaDram2Dram(i, pSrc, pDst, DataSize);
        if (RetVal != 0U) {
            printf(" DRAM-to-DRAM transfer failed.\n");
        } else {
            printf(" DRAM-to-DRAM transfer done.");
            for (j = 0; j < DataSize; j ++) {
                if (pSrc[j] != pDst[j]) {
                    break;
                }
            }

            if (j == DataSize) {
                printf(" Data verification result: OK\n");
            } else {
                printf(" Data verification result: NG\n");
            }
        }
    }
}

static void Diag_DmaDescriptorUnitTest()
{
    UINT8 *pSrc[DIAG_NUM_DESC], *pDst[DIAG_NUM_DESC];
    UINT32 DataSize = DIAG_NUM_DMA_DATA / 2U;
    UINT32 i, j, k, RetVal = 0U;
    ULONG SrcAddr, DstAddr, SrcAddrStart;
    UINT32 Size = DIAG_NUM_DMA_DATA * DIAG_NUM_DESC;
    ULONG  ULAddr;
    AMBA_DMA_DESC_s *pDmaDesc, *pDmaDescStart;
    UINT32 *pReport, *pReportStart;

    RetVal = dma_alloc_buf(&ULAddr, sizeof(AMBA_DMA_DESC_s) * DIAG_NUM_DESC, 8);
    AmbaMisra_TypeCast(&pDmaDescStart, &ULAddr);
    memset(pDmaDescStart, 0, sizeof(AMBA_DMA_DESC_s) * DIAG_NUM_DESC);
    pDmaDesc = pDmaDescStart;

    RetVal = dma_alloc_buf(&ULAddr, sizeof(uint32_t) * DIAG_NUM_DESC, 4);
    AmbaMisra_TypeCast(&pReportStart, &ULAddr);
    memset(pReportStart, 0, sizeof(uint32_t) * DIAG_NUM_DESC);
    pReport = pReportStart;

    for (i = 0U; i < DIAG_NUM_DESC; i ++) {
        /* Initialize pSrc */
        if (i == 0) {
            dma_alloc_buf(&SrcAddrStart, Size, 8);
            //get_diag_work_buf(&SrcAddrStart, &Size);
        }
        SrcAddr = SrcAddrStart + (DIAG_NUM_DMA_DATA * i);
        DstAddr = SrcAddr + (DIAG_NUM_DMA_DATA / 2U);
        AmbaMisra_TypeCast(&pSrc[i], &SrcAddr);
        AmbaMisra_TypeCast(&pDst[i], &DstAddr);

        pSrc[i][0U] = 0x55U;
        for (j = 1U; j < DataSize; j ++) {
            pSrc[i][j] = ~pSrc[i][j - 1U];
        }
    }

    for (i = 0U; i < 1; i ++) {
        /* Clean pDst */
        for (j = 0U; j < DIAG_NUM_DESC; j ++) {
            for (k = 0U; k < DataSize; k ++) {
                pDst[j][k] = 0x00U;
            }
        }

        /* Initialize DMA descriptor */
        for (j = 0U; j < DIAG_NUM_DESC; j ++) {
            AmbaMisra_TypeCast(&pDmaDesc->pSrcAddr, &pSrc[j]);
            AmbaMisra_TypeCast(&pDmaDesc->pDstAddr, &pDst[j]);
            if (j == (DIAG_NUM_DESC - 1U)) {
                pDmaDesc->pNextDesc = pDmaDescStart;
            } else {
                pDmaDesc->pNextDesc = &pDmaDesc[1];
            }
            pDmaDesc->pStatus = pReport;
            pDmaDesc->DataSize = DataSize;
            pDmaDesc->Ctrl.StopOnError = 1U;
            pDmaDesc->Ctrl.IrqOnError = 1U;
            pDmaDesc->Ctrl.IrqOnDone = 1U;
            pDmaDesc->Ctrl.Reserved0 = 0U;
            pDmaDesc->Ctrl.BusBlockSize = AMBA_DMA_BUS_BLOCK_32BYTE;
            pDmaDesc->Ctrl.BusDataSize = AMBA_DMA_BUS_DATA_4BYTE;
            pDmaDesc->Ctrl.NoBusAddrInc = 1U;
            pDmaDesc->Ctrl.ReadMem = 1U;
            pDmaDesc->Ctrl.WriteMem = 1U;
            if (j == (DIAG_NUM_DESC - 1U)) {
                pDmaDesc->Ctrl.EndOfChain = 1U;
            } else {
                pDmaDesc->Ctrl.EndOfChain = 0U;
            }
            pDmaDesc->Ctrl.Reserved1 = 0U;

            /* Push CACHE data to DRAM */
            AmbaMisra_TypeCast(&SrcAddr, &pSrc[j]);
            AmbaMisra_TypeCast(&DstAddr, &pDst[j]);
            (void)AmbaCache_DataFlush(SrcAddr, DataSize);
            (void)AmbaCache_DataInvalidate(DstAddr, DataSize);
            pDmaDesc++;
            pReport++;
        }

        (void)AmbaDMA_Transfer(0, pDmaDescStart);
        (void)AmbaDMA_Wait(i, 1000);

        for (j = 0U; j < DIAG_NUM_DESC; j ++) {
            AmbaMisra_TypeCast(&DstAddr, &pDst[j]);
            (void)AmbaCache_DataInvalidate(DstAddr, DataSize);
        }

        if (RetVal != 0U) {
            printf("[Diag][DMA] DMA channel ");
            Diag_PrintFormattedInt("%2u,", i);
            printf(" DRAM-to-DRAM transfer failed.\n");
        } else {
            /*  Compare pSrc with pDst */
            for (j = 0U; j < DIAG_NUM_DESC; j ++) {
                for (k = 0U; k < DataSize; k ++) {
                    if (pSrc[j][k] != pDst[j][k]) {
                        break;
                    }
                }

                printf("[Diag][DMA] DMA channel ");
                Diag_PrintFormattedInt("%2u,", i);
                printf(" Dram Part ");
                Diag_PrintFormattedInt("%2u,", j);
                printf(" DRAM-to-DRAM transfer done in descriptor mode.");

                if (k == DataSize) {
                    printf(" Data verification result: OK\n");
                } else {
                    printf(" Data verification result: NG\n");
                }
            }
        }
    }
}

static void Diag_DmaTransfer(UINT32 ArgCount, char * const * pArgVector)
{
    const UINT8 *pSrc = NULL, *pDst = NULL;
    UINT32 DmaChan, SrcAddr, DstAddr;
    UINT32 i, DataSize, RetVal;
    char **ptr = NULL;

    if (ArgCount >= 4U) {
        DmaChan = strtoul(pArgVector[0], ptr, 0);
        SrcAddr = strtoul(pArgVector[1], ptr, 0);
        DstAddr = strtoul(pArgVector[2], ptr, 0);
        DataSize = strtoul(pArgVector[3], ptr, 0);

        if (DmaChan >= AMBA_NUM_DMA_CHANNEL) {
            printf("[Diag][DMA] ERROR: Invalid dma channel id.\n");
        } else {
            AmbaMisra_TypeCast(&pSrc, &SrcAddr);
            AmbaMisra_TypeCast(&pDst, &DstAddr);

            RetVal = Diag_DmaDram2Dram(DmaChan, pSrc, pDst, DataSize);
            if (RetVal != 0U) {
                printf("[Diag][DMA] DRAM-to-DRAM transfer failed.\n");
            } else {
                printf("[Diag][DMA] DRAM-to-DRAM transfer done.\n");
                for (i = 0; i < DataSize; i ++) {
                    if (pSrc[i] != pDst[i]) {
                        break;
                    }
                }

                if (i == DataSize) {
                    printf("[Diag][DMA] Data verification result: OK\n");
                } else {
                    printf("[Diag][DMA] Data verification result: NG\n");
                }
            }
        }
    }
}

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
static void Diag_DmaIntStat()
{
    UINT32 i;

    printf("[Diag][DMA] Interrupt count statistics:\n");
    printf("[Diag][DMA] [Group0]:\n");

    for (i = 0U; i < AMBA_NUM_DMA_CHANNEL; i++) {
        if (i == 8U) {
            printf("[Diag][DMA] [Group1]:\n");
        }
        Diag_PrintFormattedInt("[Diag][DMA] channel[%d]: \t", i);
        Diag_PrintFormattedInt("%d\n", AmbaDMA_GetIntCount(i));
    }
}

static void Diag_DmaIntStatReset()
{
    UINT32 i;

    for (i = 0U; i < AMBA_NUM_DMA_CHANNEL; i++) {
        AmbaDMA_ResetIntCount(i);
    }

    printf("[Diag][DMA] Interrupt count statistics are reseted\n");
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaDiag_DMA
 *
 *  @Description:: DMA diagnostic command
 *
 *  @Input      ::
 *      ArgCount:   Argument count
 *      pArgVector: Argument vector
 *      printf:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
int DoDmaDiag(int argc, char *argv[])
{
    if (argc < 3) {
        Diag_DmaCmdUsage();
    } else {
        if (strcmp(argv[2], "unittest") == 0) {
            Diag_DmaUnitTest();
        } else if (strcmp(argv[2], "dsunittest") == 0) {
            Diag_DmaDescriptorUnitTest();
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        } else if (strcmp(argv[2], "stat") == 0) {
            if (argv[3] != NULL) {
                if (strcmp(argv[3], "reset") == 0) {
                    Diag_DmaIntStatReset();
                } else {
                    Diag_DmaCmdUsage();
                }
            } else {
                Diag_DmaIntStat();
            }
#endif
        } else {
            Diag_DmaTransfer(argc - 1U, &argv[2]);
        }
    }

    return 0;
}
