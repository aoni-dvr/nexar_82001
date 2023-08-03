/**
 *  @file AmbaIOUTDiag_CmdDMA.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details DMA diagnostic Command
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"

#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"

#include "AmbaIOUTDiag.h"
#include "AmbaDMA.h"
#include "AmbaCache.h"

#define DIAG_DMA_AMOUNT     (0x10000000U)
#define DIAG_NUM_DMA_DATA   (0x100000U)
#define DIAG_DMA_GAURDBAND  (1024U)
#define DIAG_NUM_DESC       (5U)

static UINT8 IoDiagDmaBuf[DIAG_NUM_DESC][DIAG_NUM_DMA_DATA + DIAG_DMA_GAURDBAND] __attribute__ ((aligned (AMBA_CACHE_LINE_SIZE)));

static void Diag_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    PrintFunc(StrBuf);
}

static void Diag_DmaCmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: dma unittest                               - Sanity testing by dram-to-dram transfer\n");
    PrintFunc("       dma dsunittest                             - Sanity testing by dram-to-dram transfer in descriptor mode\n");
    PrintFunc("       dma thruput                                - Determine DMA thruput and compare to memcpy\n");
    PrintFunc("       dma dsthruput                              - Determine DMA thruput and compare to memcpy in descriptor mode\n");
    PrintFunc("       dma <channel> <src_addr> <dst_addr> <size> - Transfer continous bytes from <src_addr> to <dst_addr>\n");
}

static UINT32 Diag_DmaDram2Dram(UINT32 DmaChan, const UINT8 *pSrc, const UINT8 *pDst, UINT32 Len)
{
    AMBA_DMA_DESC_s DmaDesc;
    UINT32 DmaStatus;
    UINT32 SrcAddr, DstAddr;

    AmbaMisra_TypeCast32(&DmaDesc.pSrcAddr, &pSrc);
    AmbaMisra_TypeCast32(&DmaDesc.pDstAddr, &pDst);
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
    AmbaMisra_TypeCast32(&SrcAddr, &pSrc);
    AmbaMisra_TypeCast32(&DstAddr, &pDst);
    (void)AmbaCache_DataFlush(SrcAddr, Len);
    (void)AmbaCache_DataInvalidate(DstAddr, Len);

    DmaStatus = AmbaDMA_Transfer(DmaChan, &DmaDesc);
    (void)AmbaDMA_Wait(DmaChan, 5000U);
    (void)AmbaCache_DataInvalidate(DstAddr, Len);

    return DmaStatus;
}

static void Diag_DmaUnitTest(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT8 *pSrc = &IoDiagDmaBuf[0U][0U], *pDst = &IoDiagDmaBuf[0U][DIAG_NUM_DMA_DATA / 2U];
    ULONG SrcAddr, DstAddr;
    UINT32 DataSize = DIAG_NUM_DMA_DATA / 2U;
    UINT32 i, j, RetVal = 0;

    AmbaMisra_TypeCast(&SrcAddr, &pSrc);
    AmbaMisra_TypeCast(&DstAddr, &pDst);

    pSrc[0] = 0x55U;
    for (j = 1U; j < DataSize; j ++) {
        pSrc[j] = ~pSrc[j - 1U];
    }
    (void)AmbaCache_DataFlush(SrcAddr, DataSize);

    for (i = 0; i < AMBA_NUM_DMA_CHANNEL; i ++) {
        PrintFunc("[Diag][DMA] DMA channel ");
        Diag_PrintFormattedInt("%u,", i, PrintFunc);

        for (j = 0; j < DataSize; j ++) {
            pDst[j] = 0x00U;
        }
        (void)AmbaCache_DataFlush(DstAddr, DataSize);

        RetVal = Diag_DmaDram2Dram(i, pSrc, pDst, DataSize);
        if (RetVal != 0U) {
            PrintFunc(" DRAM-to-DRAM transfer failed.\n");
        } else {
            PrintFunc(" DRAM-to-DRAM transfer done.");
            for (j = 0; j < DataSize; j ++) {
                if (pSrc[j] != pDst[j]) {
                    break;
                }
            }

            if (j == DataSize) {
                PrintFunc(" Data verification result: OK\n");
            } else {
                PrintFunc(" Data verification result: NG\n");
            }
        }
    }
}

static void Diag_DmaDescriptorUnitTest(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT8 *pSrc[DIAG_NUM_DESC], *pDst[DIAG_NUM_DESC];
    UINT32 DataSize = DIAG_NUM_DMA_DATA / 2U;
    UINT32 i, j, k, RetVal = 0U;
    AMBA_DMA_DESC_s DmaDesc[DIAG_NUM_DESC];
    AMBA_DMA_DESC_s *pDmaDesc;
    UINT32 Status[DIAG_NUM_DESC];
    ULONG SrcAddr, DstAddr;

    for (i = 0U; i < DIAG_NUM_DESC; i ++) {
        /* Initialize pSrc */
        pSrc[i] = &IoDiagDmaBuf[i][0U];
        pDst[i] = &IoDiagDmaBuf[i][DIAG_NUM_DMA_DATA / 2U];

        pSrc[i][0U] = 0x55U;
        for (j = 1U; j < DataSize; j ++) {
            pSrc[i][j] = ~pSrc[i][j - 1U];
        }
    }

    for (i = 0U; i < AMBA_NUM_DMA_CHANNEL; i ++) {
        /* Clean pDst */
        for (j = 0U; j < DIAG_NUM_DESC; j ++) {
            for (k = 0U; k < DataSize; k ++) {
                pDst[j][k] = 0x00U;
            }
        }

        /* Initialize DMA descriptor */
        for (j = 0U; j < DIAG_NUM_DESC; j ++) {
            pDmaDesc = &DmaDesc[j];
            AmbaMisra_TypeCast(&pDmaDesc->pSrcAddr, &pSrc[j]);
            AmbaMisra_TypeCast(&pDmaDesc->pDstAddr, &pDst[j]);
            if (j == (DIAG_NUM_DESC - 1U)) {
                pDmaDesc->pNextDesc = NULL;
            } else {
                pDmaDesc->pNextDesc = &DmaDesc[j + 1U];
            }
            pDmaDesc->pStatus = &Status[j];
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
        }

        (void)AmbaDMA_Transfer(i, &DmaDesc[0]);
        (void)AmbaDMA_Wait(i, AMBA_KAL_WAIT_FOREVER);

        for (j = 0U; j < DIAG_NUM_DESC; j ++) {
            AmbaMisra_TypeCast(&DstAddr, &pDst[j]);
            (void)AmbaCache_DataInvalidate(DstAddr, DataSize);
        }

        if (RetVal != 0U) {
            PrintFunc("[Diag][DMA] DMA channel ");
            Diag_PrintFormattedInt("%2u,", i, PrintFunc);
            PrintFunc(" DRAM-to-DRAM transfer failed.\n");
        } else {
            /*  Compare pSrc with pDst */
            for (j = 0U; j < DIAG_NUM_DESC; j ++) {
                for (k = 0U; k < DataSize; k ++) {
                    if (pSrc[j][k] != pDst[j][k]) {
                        break;
                    }
                }

                PrintFunc("[Diag][DMA] DMA channel ");
                Diag_PrintFormattedInt("%2u,", i, PrintFunc);
                PrintFunc(" Dram Part ");
                Diag_PrintFormattedInt("%2u,", j, PrintFunc);
                PrintFunc(" DRAM-to-DRAM transfer done in descriptor mode.");

                if (k == DataSize) {
                    PrintFunc(" Data verification result: OK\n");
                } else {
                    PrintFunc(" Data verification result: NG\n");
                }
            }
        }
    }
}

static void Diag_DmaThruput(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT8 *pSrc = &IoDiagDmaBuf[0U][0U], *pDst = &IoDiagDmaBuf[0U][DIAG_DMA_GAURDBAND];
    UINT32 SrcAddr, DstAddr;
    UINT32 i, j, DmaChan, RetVal;

    UINT32 TestSize, TestCount;
    UINT32 StartTime, EndTime, DmaTimeDiff, CpTimeDiff;
    UINT64 ActualSize, DmaRate, CpRate;

    AmbaMisra_TypeCast32(&SrcAddr, &pSrc);
    AmbaMisra_TypeCast32(&DstAddr, &pDst);

    pSrc[0] = 0x55U;
    pDst[0] = 0x00U;
    for (i = 1U; i < DIAG_NUM_DMA_DATA; i ++) {
        pSrc[i] = ~pSrc[i - 1U];
        pDst[i] = 0x00U;
    }
    (void)AmbaCache_DataFlush(SrcAddr, DIAG_NUM_DMA_DATA);
    (void)AmbaCache_DataFlush(DstAddr, DIAG_NUM_DMA_DATA);

    /* check if the specified dma channel available */
    RetVal = AmbaDMA_ChannelAllocate(AMBA_DMA_CHANNEL_MEM_TRX, &DmaChan);

    if (DmaChan != 0xffffffffU) {
        PrintFunc("[Diag][DMA] Use DMA channel ");
        Diag_PrintFormattedInt("%u.\n", DmaChan, PrintFunc);

        RetVal = Diag_DmaDram2Dram(DmaChan, pSrc, pDst, DIAG_NUM_DMA_DATA);
        if (RetVal == OK) {
            for (i = 0; i < DIAG_NUM_DMA_DATA; i ++) {
                if (pSrc[i] != pDst[i]) {
                    break;
                }
            }

            if (i != DIAG_NUM_DMA_DATA) {
                RetVal = ~OK;
            }
        }

        if (RetVal != OK) {
            PrintFunc("[Diag][DMA] DRAM-to-DRAM transfer failed.\n");
        } else {
            PrintFunc("[Diag][DMA] Start to estimate throughput.\n");

            for (TestSize = 2048U; TestSize <= DIAG_NUM_DMA_DATA; TestSize = TestSize << 1U) {
                TestCount = DIAG_DMA_AMOUNT / TestSize;

                PrintFunc("[Diag][DMA] Block Size = ");
                Diag_PrintFormattedInt("%u bytes\n", TestSize, PrintFunc);
                PrintFunc("[Diag][DMA] Number of Blocks = ");
                Diag_PrintFormattedInt("%u blocks\n", TestCount, PrintFunc);

                /* DMA transfer */
                (void)AmbaKAL_GetSysTickCount(&StartTime);
                ActualSize = 0;
                for (j = 0; j < TestCount; j++) {
                    RetVal = Diag_DmaDram2Dram(DmaChan, pSrc, pDst, TestSize);
                    if (RetVal == OK) {
                        ActualSize += (UINT64)TestSize;
                    }
                }
                (void)AmbaKAL_GetSysTickCount(&EndTime);

                if (StartTime < EndTime) {
                    DmaTimeDiff = EndTime - StartTime;
                } else {
                    DmaTimeDiff = ((0xffffffffUL - StartTime) + EndTime + 1U);
                }
                DmaRate = (ActualSize >> 20U);
                DmaRate = (DmaRate * 1000UL) / DmaTimeDiff;

                PrintFunc("[Diag][DMA] Elapsed time to DMA all blocks    : ");
                Diag_PrintFormattedInt("%u mSec\n", DmaTimeDiff, PrintFunc);
                PrintFunc("[Diag][DMA] Average DMA throughput            : ");
                Diag_PrintFormattedInt("%u MBs/Sec\n", (UINT32)DmaRate, PrintFunc);

                /* memcpy */
                (void)AmbaKAL_GetSysTickCount(&StartTime);
                ActualSize = 0;
                for (j = 0 ; j < TestCount; j++) {
                    (void)AmbaWrap_memcpy(pDst, pSrc, TestSize);
                    ActualSize += TestSize;
                }
                (void)AmbaKAL_GetSysTickCount(&EndTime);

                if (StartTime < EndTime) {
                    CpTimeDiff = EndTime - StartTime;
                } else {
                    CpTimeDiff = ((0xffffffffUL - StartTime) + EndTime + 1U);
                }

                CpRate = (ActualSize >> 20U);
                CpRate = (CpRate * 1000UL) / CpTimeDiff;

                PrintFunc("[Diag][DMA] Elapsed time to memcpy all blocks : ");
                Diag_PrintFormattedInt("%u mSec\n", CpTimeDiff, PrintFunc);
                PrintFunc("[Diag][DMA] Average memcpy throughput         : ");
                Diag_PrintFormattedInt("%u MBs/Sec\n", (UINT32)CpRate, PrintFunc);
            }
        }

        (void)AmbaDMA_ChannelRelease(DmaChan);
    }
}

static void Diag_DmaDescriptorThruput(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT8 *pSrc[DIAG_NUM_DESC], *pDst[DIAG_NUM_DESC];
    UINT32 DataSize = DIAG_NUM_DMA_DATA / 2U;
    UINT32 i, j, k;
    UINT32 StartTime, EndTime, DmaTimeDiff, CpTimeDiff;
    UINT64 ActualSize, DmaRate, CpRate;
    AMBA_DMA_DESC_s DmaDesc[DIAG_NUM_DESC];
    AMBA_DMA_DESC_s *pDmaDesc;
    UINT32 Status[DIAG_NUM_DESC];
    UINT32 SrcAddr, DstAddr;

    PrintFunc("[Diag][DMA] Start to estimate throughput in descriptor mode.\n");
    PrintFunc("[Diag][DMA] Block Size = ");
    Diag_PrintFormattedInt("%u bytes\n", DataSize, PrintFunc);
    PrintFunc("[Diag][DMA] Number of Blocks = ");
    Diag_PrintFormattedInt("%u blocks\n", DIAG_NUM_DESC, PrintFunc);

    for (i = 0U; i < DIAG_NUM_DESC; i ++) {
        /* Initialize pSrc */
        pSrc[i] = &IoDiagDmaBuf[i][0U];
        pDst[i] = &IoDiagDmaBuf[i][DIAG_NUM_DMA_DATA / 2U];

        pSrc[i][0U] = 0x55U;
        for (j = 1U; j < DataSize; j ++) {
            pSrc[i][j] = ~pSrc[i][j - 1U];
        }
    }

    for (i = 0U; i < AMBA_NUM_DMA_CHANNEL; i ++) {
        PrintFunc("[Diag][DMA]----------------------------------------\n");
        PrintFunc("[Diag][DMA] DMA channel ");
        Diag_PrintFormattedInt("%u\n", i, PrintFunc);

        /* Clean pDst */
        for (j = 0U; j < DIAG_NUM_DESC; j ++) {
            for (k = 0U; k < DataSize; k ++) {
                pDst[j][k] = 0x00U;
            }
        }

        /* DMA */
        ActualSize = 0U;
        (void)AmbaKAL_GetSysTickCount(&StartTime);

        /* Initialize DMA descriptor */
        for (j = 0U; j < DIAG_NUM_DESC; j ++) {
            pDmaDesc = &DmaDesc[j];
            AmbaMisra_TypeCast32(&pDmaDesc->pSrcAddr, &pSrc[j]);
            AmbaMisra_TypeCast32(&pDmaDesc->pDstAddr, &pDst[j]);
            if (j == (DIAG_NUM_DESC - 1U)) {
                pDmaDesc->pNextDesc = NULL;
            } else {
                pDmaDesc->pNextDesc = &DmaDesc[j + 1U];
            }
            pDmaDesc->pStatus = &Status[j];
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
            AmbaMisra_TypeCast32(&SrcAddr, &pSrc[j]);
            AmbaMisra_TypeCast32(&DstAddr, &pDst[j]);
            (void)AmbaCache_DataFlush(SrcAddr, DataSize);
            (void)AmbaCache_DataInvalidate(DstAddr, DataSize);
        }

        (void)AmbaDMA_Transfer(i, &DmaDesc[0]);
        (void)AmbaDMA_Wait(i, AMBA_KAL_WAIT_FOREVER);

        for (j = 0U; j < DIAG_NUM_DESC; j ++) {
            AmbaMisra_TypeCast32(&DstAddr, &pDst[j]);
            (void)AmbaCache_DataInvalidate(DstAddr, DataSize);
        }

        ActualSize += ((UINT64)DataSize * DIAG_NUM_DESC);

        (void)AmbaKAL_GetSysTickCount(&EndTime);

        (void)AmbaDMA_ChannelRelease(i);

        if (StartTime < EndTime) {
            DmaTimeDiff = EndTime - StartTime;
        } else {
            DmaTimeDiff = ((0xffffffffUL - StartTime) + EndTime + 1U);
        }
        DmaRate = (ActualSize >> 20U);
        DmaRate = (DmaRate * 1000UL) / DmaTimeDiff;

        PrintFunc("[Diag][DMA] Elapsed time to DMA       : ");
        Diag_PrintFormattedInt("%u mSec\n", DmaTimeDiff, PrintFunc);
        PrintFunc("[Diag][DMA] Average DMA throughput    : ");
        Diag_PrintFormattedInt("%u MBs/Sec\n", (UINT32)DmaRate, PrintFunc);

        /* memcpy */
        (void)AmbaKAL_GetSysTickCount(&StartTime);
        ActualSize = 0U;
        for (j = 0U; j < DIAG_NUM_DESC; j ++) {
            (void)AmbaWrap_memcpy(pDst[j], pSrc[j], DataSize);
            ActualSize += (UINT64)DataSize;
        }
        (void)AmbaKAL_GetSysTickCount(&EndTime);

        if (StartTime < EndTime) {
            CpTimeDiff = EndTime - StartTime;
        } else {
            CpTimeDiff = ((0xffffffffUL - StartTime) + EndTime + 1U);
        }

        CpRate = (ActualSize >> 20U);
        CpRate = (CpRate * 1000UL) / CpTimeDiff;

        PrintFunc("[Diag][DMA] Elapsed time to memcpy    : ");
        Diag_PrintFormattedInt("%u mSec\n", CpTimeDiff, PrintFunc);
        PrintFunc("[Diag][DMA] Average memcpy throughput : ");
        Diag_PrintFormattedInt("%u MBs/Sec\n", (UINT32)CpRate, PrintFunc);
        PrintFunc("[Diag][DMA]\n");
    }

    PrintFunc("[Diag][DMA]----------------------------------------\n");
}

static void Diag_DmaTransfer(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    const UINT8 *pSrc = NULL, *pDst = NULL;
    UINT32 DmaChan, SrcAddr, DstAddr;
    UINT32 i, DataSize, RetVal;

    if (ArgCount >= 4U) {
        (void)AmbaUtility_StringToUInt32(pArgVector[0], &DmaChan);
        (void)AmbaUtility_StringToUInt32(pArgVector[1], &SrcAddr);
        (void)AmbaUtility_StringToUInt32(pArgVector[2], &DstAddr);
        (void)AmbaUtility_StringToUInt32(pArgVector[3], &DataSize);

        if (DmaChan >= AMBA_NUM_DMA_CHANNEL) {
            PrintFunc("[Diag][DMA] ERROR: Invalid dma channel id.\n");
        } else {
            AmbaMisra_TypeCast32(&pSrc, &SrcAddr);
            AmbaMisra_TypeCast32(&pDst, &DstAddr);

            RetVal = Diag_DmaDram2Dram(DmaChan, pSrc, pDst, DataSize);
            if (RetVal != 0U) {
                PrintFunc("[Diag][DMA] DRAM-to-DRAM transfer failed.\n");
            } else {
                PrintFunc("[Diag][DMA] DRAM-to-DRAM transfer done.\n");
                for (i = 0; i < DataSize; i ++) {
                    if (pSrc[i] != pDst[i]) {
                        break;
                    }
                }

                if (i == DataSize) {
                    PrintFunc("[Diag][DMA] Data verification result: OK\n");
                } else {
                    PrintFunc("[Diag][DMA] Data verification result: NG\n");
                }
            }
        }
    }
}

static void Diag_DmaIntStat(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i;

    PrintFunc("[Diag][DMA] Interrupt count statistics:\n");
    PrintFunc("[Diag][DMA] [Group0]:\n");

    for (i = 0U; i < AMBA_NUM_DMA_CHANNEL; i++) {
        if (i == 8U) {
            PrintFunc("[Diag][DMA] [Group1]:\n");
        }
        Diag_PrintFormattedInt("[Diag][DMA] channel[%d]: \t", i, PrintFunc);
        Diag_PrintFormattedInt("%d\n", AmbaDMA_GetIntCount(i), PrintFunc);
    }
}

static void Diag_DmaIntStatReset(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i;

    for (i = 0U; i < AMBA_NUM_DMA_CHANNEL; i++) {
        AmbaDMA_ResetIntCount(i);
    }

    PrintFunc("[Diag][DMA] Interrupt count statistics are reseted\n");
}

/**
 *  DMA diagnostic command
 *  @param[in] ArgCount Argument count
 *  @param[in] pArgVector Argument vector
 *  @param[in] PrintFunc function to print messages on shell task
 *  @return none
 */
void AmbaIOUTDiag_CmdDMA(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount < 2U) {
        Diag_DmaCmdUsage(PrintFunc);
    } else {
        if (AmbaUtility_StringCompare(pArgVector[1], "unittest", 8U) == 0) {
            Diag_DmaUnitTest(PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[1], "dsunittest", 10U) == 0) {
            Diag_DmaDescriptorUnitTest(PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[1], "thruput", 7U) == 0) {
            Diag_DmaThruput(PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[1], "dsthruput", 9U) == 0) {
            Diag_DmaDescriptorThruput(PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[1], "stat", 4U) == 0) {
            if (AmbaUtility_StringCompare(pArgVector[2], "reset", 5U) == 0) {
                Diag_DmaIntStatReset(PrintFunc);
            } else {
                Diag_DmaIntStat(PrintFunc);
            }
        } else {
            Diag_DmaTransfer(ArgCount - 1U, &pArgVector[1], PrintFunc);
        }
    }
}
