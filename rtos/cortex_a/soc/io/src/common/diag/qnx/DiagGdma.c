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

#include "AmbaCache.h"
#include "AmbaGDMA.h"

#define DIAG_GDMA_TIMEOUT       (3000U)
#define DIAG_GDMA_STR_BUF_SIZE  (250U)
#define DIAG_GDMA_IMG_BUF_SIZE  (1024U * 1024U)

typedef UINT32 AMBA_GDMA_32BIT_DATA;

typedef struct {
    UINT8 Blue;
    UINT8 Green;
    UINT8 Red;
    UINT8 Alpha;
} AMBA_DIAG_GDAM_ARGB_s;

typedef struct {
    UINT32  Alpha;
    UINT32  TansparentColor;
    UINT8   ShowDbgMsg;
    UINT32  TestWidth;
    UINT32  TestHeight;
    UINT32  TestRowStride;
} AMBA_DIAG_GDMA_CTRL_s;

static const UINT8 DiagGdmaPixelWidth[AMBA_NUM_GDMA_PIXEL_FORMAT] = {
    [AMBA_GDMA_8_BIT]       = 8U,
    [AMBA_GDMA_4_BIT]       = 4U,
    [AMBA_GDMA_16_BIT_1555] = 16U,
    [AMBA_GDMA_16_BIT_565]  = 16U,
    [AMBA_GDMA_24_BIT]      = 24U,
    [AMBA_GDMA_32_BIT]      = 32U,
};

static AMBA_DIAG_GDMA_CTRL_s AmbaDiagGdmaCtrl = {
    .Alpha              = 128U,
    .TansparentColor    = 0xff02ffffUL,
    .ShowDbgMsg         = 1U,
    .TestWidth          = 320U,
    .TestHeight         = 240U,
    .TestRowStride      = 320U,
};

void get_work_buf(ULONG *pAddr, UINT32 *pSize)
{
    int32_t Rval = 0;
    struct posix_typed_mem_info info;
    static void *virt_addr = NULL;
    int MemFd = 0;

    if (virt_addr == NULL) {
        MemFd = posix_typed_mem_open("/ram/diag_work", O_RDWR, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
        if (MemFd < 0) {
            printf("[%s] ut_get_dsp_work_buf() : posix_typed_mem_open fail(/ram/diag_work) fd = %d",__FUNCTION__, MemFd);
        } else {
            Rval = posix_typed_mem_get_info(MemFd, &info);
            if (Rval < 0) {
                printf("[%s] ut_get_dsp_work_buf() : posix_typed_mem_get_info fail Rval = %d",__FUNCTION__, Rval);
            } else {
                virt_addr = mmap(NULL, info.posix_tmi_length, PROT_READ | PROT_WRITE, MAP_SHARED, MemFd, 0);
                if (virt_addr == MAP_FAILED) {
                    printf("[%s] ut_get_dsp_work_buf() : mmap fail",__FUNCTION__);
                } else {
                    (void)AmbaMisra_TypeCast(pAddr, &virt_addr);
                    *pSize = info.posix_tmi_length;
                }
            }
        }
    } else {
        (void)AmbaMisra_TypeCast(pAddr, &virt_addr);
        *pSize = 4096*1024;
    }

}

static void Diag_GdmaGen32bppSrcImg(UINT32 NumPixels, AMBA_GDMA_32BIT_DATA *pPixel)
{
    UINT32 i, R, G, B, PixelVal;

    if (pPixel != NULL) {
        PixelVal = *pPixel;
        R = (PixelVal >> 16UL) & 0xFFUL;
        //        G = (PixelVal >> 8UL) & 0xFFUL;
        B = (PixelVal) & 0xFFUL;

        if ((R + B) == 0x100UL) {
            R = (R + 1UL) & 0xFFUL;
        } else {
            R = 0x00UL;
        }
        B = (0x100UL - R) & 0xFFUL;
        G = (R ^ B) & 0xFFUL;

        PixelVal = 0xFF000000UL;
        PixelVal |= (R << 16UL);
        PixelVal |= (G << 8UL);
        PixelVal |= (B);

        for (i = 0; i < NumPixels; i++) {
            *pPixel = PixelVal;
            pPixel++;
        }
    }
}

static void Diag_GdmaGen32bppDstImg(UINT32 NumPixels, AMBA_GDMA_32BIT_DATA *pPixel)
{
    UINT32 i, PixelVal = 0x20000000;    /* (A,R,G,B) = (0x20, 0x00, 0x00, 0x00) */

    if (pPixel != NULL) {
        for (i = 0; i < NumPixels; i++) {
            *pPixel = PixelVal;
            pPixel++;
        }
    }
}

static ULONG Diag_GdmaGetAddr(const void *pPtr)
{
    ULONG Value = 0;

    AmbaMisra_TypeCast(&Value, &pPtr);

    return Value;
}

static void Diag_GdmaPrintBufferInfo(const AMBA_GDMA_32BIT_DATA *pSrc, const AMBA_GDMA_32BIT_DATA *pDst, const AMBA_GDMA_32BIT_DATA *pGolden)
{
    UINT32 ArgList[2];

    ArgList[0] = Diag_GdmaGetAddr(pSrc);
    printf("[Diag][GDMA] Source Image Base:      0x%X\n", ArgList[0]);

    ArgList[0] = Diag_GdmaGetAddr(pDst);
    printf("[Diag][GDMA] Destination Image Base: 0x%X\n", ArgList[0]);

    if (pGolden != NULL) {
        ArgList[0] = Diag_GdmaGetAddr(pGolden);
        printf("[Diag][GDMA] Golden Image Base:      0x%X\n", ArgList[0]);
    }
}

static void Diag_GdmaVerifyData(const AMBA_GDMA_32BIT_DATA *pData1, const AMBA_GDMA_32BIT_DATA *pData2,
                                UINT32 DataSize, UINT32 PixelFormat)
{
    UINT32 i, Args[5];
    INT32  CmpResult;

    CmpResult = memcmp(pData1, pData2, DataSize * DiagGdmaPixelWidth[PixelFormat] / 8U);
    if (CmpResult != 0) {
        printf("[Diag][GDMA] Error: Data mismatch!!!\n");
        /* Show error log */
        if (AmbaDiagGdmaCtrl.ShowDbgMsg != 0U) {
            /* Print Error message */
            for (i = 0; i < DataSize; i++) {
                if (pData1[i] != pData2[i]) {
                    Args[0] = i;
                    Args[1] = Diag_GdmaGetAddr(&pData1[i]);
                    Args[2] = pData1[i];
                    Args[3] = Diag_GdmaGetAddr(&pData2[i]);
                    Args[4] = pData2[i];
                    printf("[Diag][GDMA] Data %uth, [0x%x]=%08x, [0x%x]=%08x\n", Args[0], Args[1], Args[2], Args[3], Args[4]);
                }
            }
        }
    } else {
        printf("[Diag][GDMA] GDMA function done with no errors\n");
    }
}


static void Diag_GdmaLinearCopy(UINT32 NumPixels)
{
    AMBA_GDMA_LINEAR_s LinearBlit;
    AMBA_GDMA_32BIT_DATA *pGdmaSrc, *pGdmaDst;
    ULONG GdmaSrcAddr, GdmaDstAddr;
    UINT32 Offset = 0;
    UINT32 Remainder = 0;
    UINT32 uRet;

    if (NumPixels > DIAG_GDMA_IMG_BUF_SIZE) {
        printf("[Diag][GDMA] ERROR: Data size is too large.\n");
    } else {
        get_work_buf(&GdmaSrcAddr, &Offset);
        GdmaDstAddr = GdmaSrcAddr + DIAG_GDMA_IMG_BUF_SIZE;
        AmbaMisra_TypeCast(&pGdmaSrc, &GdmaSrcAddr);
        AmbaMisra_TypeCast(&pGdmaDst, &GdmaDstAddr);

        LinearBlit.PixelFormat = AMBA_GDMA_32_BIT;
        LinearBlit.NumPixels = NumPixels;
        LinearBlit.pDstImg = pGdmaDst;
        LinearBlit.pSrcImg = pGdmaSrc;

        if (AmbaDiagGdmaCtrl.ShowDbgMsg != 0U) {
            Diag_GdmaPrintBufferInfo(pGdmaSrc, pGdmaDst, NULL);
        }

        (void)memset(pGdmaSrc, 0x11, NumPixels * DiagGdmaPixelWidth[LinearBlit.PixelFormat] / 8U);
        (void)memset(pGdmaDst, 0x22, NumPixels * DiagGdmaPixelWidth[LinearBlit.PixelFormat] / 8U);

        Diag_GdmaGen32bppSrcImg(NumPixels, pGdmaSrc);
        Diag_GdmaGen32bppDstImg(NumPixels, pGdmaDst);

        (void)AmbaCache_DataClean(GdmaSrcAddr, NumPixels * DiagGdmaPixelWidth[LinearBlit.PixelFormat] / 8U);
        (void)AmbaCache_DataInvalidate(GdmaDstAddr, NumPixels * DiagGdmaPixelWidth[LinearBlit.PixelFormat] / 8U);

        /* Start GDMA function */
        Remainder = NumPixels;
        Offset = 0;
        while (Remainder > 0U) {
            LinearBlit.pSrcImg = &pGdmaSrc[Offset];
            LinearBlit.pDstImg = &pGdmaDst[Offset];

            if (Remainder > AMBA_GDMA_MAX_WIDTH) {
                LinearBlit.NumPixels = AMBA_GDMA_MAX_WIDTH;
                uRet = AmbaGDMA_LinearCopy(&LinearBlit, NULL, 0U, DIAG_GDMA_TIMEOUT);
            } else {
                LinearBlit.NumPixels = Remainder;
                uRet = AmbaGDMA_LinearCopy(&LinearBlit, NULL, 0U, DIAG_GDMA_TIMEOUT);
            }
            if (uRet != 0U) {
                printf("[Diag][GDMA] ERROR: 0x%X\n", uRet);
                break;
            }
            Offset += AMBA_GDMA_MAX_WIDTH;
            Remainder -= LinearBlit.NumPixels;
        }

        if (AmbaGDMA_WaitAllCompletion(DIAG_GDMA_TIMEOUT) != 0U) {
            printf("[Diag][GDMA] ERROR: Failed to wait for completion.\n");
        }

        (void)AmbaCache_DataInvalidate(GdmaDstAddr, NumPixels * DiagGdmaPixelWidth[LinearBlit.PixelFormat] / 8U);

        /* Verify data */
        Diag_GdmaVerifyData(pGdmaSrc, pGdmaDst, NumPixels, LinearBlit.PixelFormat);
    }
}

static void Diag_GdmaBlockCopy(UINT32 Width, UINT32 Height, UINT32 RowStride)
{
    AMBA_GDMA_BLOCK_s BlockBlit;
    AMBA_GDMA_32BIT_DATA *pGdmaSrc, *pGdmaDst, *pGolden;
    ULONG GdmaSrcAddr, GdmaDstAddr, GdmaGoldAddr;
    UINT32 i, j;
    UINT32 DataSize;
    UINT32 uRet;

    DataSize = RowStride * Height;

    if (DataSize > DIAG_GDMA_IMG_BUF_SIZE) {
        printf("[Diag][GDMA] ERROR: Data size is too large.\n");
    } else {
        get_work_buf(&GdmaSrcAddr, &i);
        GdmaDstAddr = GdmaSrcAddr + DIAG_GDMA_IMG_BUF_SIZE;
        GdmaGoldAddr = GdmaDstAddr + DIAG_GDMA_IMG_BUF_SIZE;
        AmbaMisra_TypeCast(&pGdmaSrc, &GdmaSrcAddr);
        AmbaMisra_TypeCast(&pGdmaDst, &GdmaDstAddr);
        AmbaMisra_TypeCast(&pGolden, &GdmaGoldAddr);

        BlockBlit.PixelFormat = AMBA_GDMA_32_BIT;
        BlockBlit.BltWidth = Width;
        BlockBlit.BltHeight = Height;
        BlockBlit.DstRowStride = RowStride;
        BlockBlit.SrcRowStride = RowStride;
        BlockBlit.pDstImg = pGdmaDst;
        BlockBlit.pSrcImg = pGdmaSrc;

        if (AmbaDiagGdmaCtrl.ShowDbgMsg != 0U) {
            Diag_GdmaPrintBufferInfo(pGdmaSrc, pGdmaDst, pGolden);
        }

        (void)memset(pGdmaSrc, 0x11, DataSize * DiagGdmaPixelWidth[BlockBlit.PixelFormat] / 8U);
        (void)memset(pGdmaDst, 0x22, DataSize * DiagGdmaPixelWidth[BlockBlit.PixelFormat] / 8U);

        Diag_GdmaGen32bppSrcImg(DataSize, pGdmaSrc);
        Diag_GdmaGen32bppDstImg(DataSize, pGdmaDst);

        /* Generated Golden Data */
        for (i = 0; i < DataSize; i++) {
            pGolden[i] = pGdmaDst[i];
        }

        for (i = 0; i < BlockBlit.BltHeight ; i++) {
            for (j = 0; j < BlockBlit.BltWidth; j++) {
                pGolden[ (i * BlockBlit.DstRowStride / 4UL) + j] = pGdmaSrc[(i * BlockBlit.SrcRowStride / 4UL) + j];
            }
        }

        /* Start GDMA function */
        (void)AmbaCache_DataClean(GdmaSrcAddr, DataSize * DiagGdmaPixelWidth[BlockBlit.PixelFormat] / 8U);
        (void)AmbaCache_DataClean(GdmaDstAddr, DataSize * DiagGdmaPixelWidth[BlockBlit.PixelFormat] / 8U);
        (void)AmbaCache_DataInvalidate(GdmaDstAddr, DataSize * DiagGdmaPixelWidth[BlockBlit.PixelFormat] / 8U);

        uRet = AmbaGDMA_BlockCopy(&BlockBlit, NULL, 0U, DIAG_GDMA_TIMEOUT);
        if (uRet != 0U) {
            printf("AmbaGDMA_BlockCopy() return 0x%X", uRet);
        }

        if (AmbaGDMA_WaitAllCompletion(DIAG_GDMA_TIMEOUT) != 0U) {
            printf("[Diag][GDMA] ERROR: Failed to wait for completion.\n");
        }

        /* Verify data */
        Diag_GdmaVerifyData(pGdmaDst, pGolden, DataSize, BlockBlit.PixelFormat);

    }

    return;
}

static void Diag_GdmaColorKeying(UINT32 Width, UINT32 Height, UINT32 RowStride)
{
    AMBA_GDMA_BLOCK_s GdmaComposite;
    AMBA_GDMA_32BIT_DATA *pGdmaSrc, *pGdmaDst, *pGolden;
    ULONG GdmaSrcAddr, GdmaDstAddr, GdmaGoldAddr;
    UINT32 i, j;
    UINT32 DataSize;
    UINT32 uRet;

    DataSize = RowStride * Height;

    if (DataSize > DIAG_GDMA_IMG_BUF_SIZE) {
        printf("[Diag][GDMA] ERROR: Data size is too large.\n");
    } else {
        get_work_buf(&GdmaSrcAddr, &i);
        GdmaDstAddr = GdmaSrcAddr + DIAG_GDMA_IMG_BUF_SIZE;
        GdmaGoldAddr = GdmaDstAddr + DIAG_GDMA_IMG_BUF_SIZE;
        AmbaMisra_TypeCast(&pGdmaSrc, &GdmaSrcAddr);
        AmbaMisra_TypeCast(&pGdmaDst, &GdmaDstAddr);
        AmbaMisra_TypeCast(&pGolden, &GdmaGoldAddr);

        GdmaComposite.PixelFormat = AMBA_GDMA_32_BIT;
        GdmaComposite.BltWidth = Width;
        GdmaComposite.BltHeight = Height;
        GdmaComposite.DstRowStride = RowStride;
        GdmaComposite.SrcRowStride = RowStride;
        GdmaComposite.pDstImg = pGdmaDst;
        GdmaComposite.pSrcImg = pGdmaSrc;

        if (AmbaDiagGdmaCtrl.ShowDbgMsg != 0U) {
            Diag_GdmaPrintBufferInfo(pGdmaSrc, pGdmaDst, pGolden);
        }

        (void)memset(pGdmaSrc, 0, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);
        (void)memset(pGdmaDst, 0, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);

        Diag_GdmaGen32bppSrcImg(DataSize, pGdmaSrc);
        Diag_GdmaGen32bppDstImg(DataSize, pGdmaDst);

        /* Generated Golden Data */
        for (i = 0; i < DataSize; i++) {
            pGolden[i] = pGdmaDst[i];
        }

        for (i = 0; i < GdmaComposite.BltHeight ; i++) {
            for (j = 0; j < GdmaComposite.BltWidth; j++) {
                if (pGdmaSrc[(i * GdmaComposite.SrcRowStride / 4UL) + j] != AmbaDiagGdmaCtrl.TansparentColor) {
                    pGolden[(i * GdmaComposite.DstRowStride / 4UL) + j] = pGdmaSrc[(i * GdmaComposite.SrcRowStride / 4UL) + j];
                }
            }
        }

        /* Start GDMA function */
        (void)AmbaCache_DataClean(GdmaSrcAddr, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);
        (void)AmbaCache_DataClean(GdmaDstAddr, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);
        (void)AmbaCache_DataInvalidate(GdmaDstAddr, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);

        uRet = AmbaGDMA_ColorKeying(&GdmaComposite, AmbaDiagGdmaCtrl.TansparentColor, NULL, 0U, DIAG_GDMA_TIMEOUT);
        if (uRet != 0U) {
            printf("AmbaGDMA_ColorKeying() return 0x%X", uRet);
        }

        if (AmbaGDMA_WaitAllCompletion(DIAG_GDMA_TIMEOUT) != 0U) {
            printf("[Diag][GDMA] ERROR: Failed to wait for completion.\n");
        }

        /* Verify data */
        Diag_GdmaVerifyData(pGdmaDst, pGolden, DataSize, GdmaComposite.PixelFormat);

    }

    return;
}

static UINT32 Diag_GdmaBlendColor(UINT32 Color0, UINT32 Percent0, UINT32 Color1, UINT32 Percent1)
{
    return (((Color0 * Percent0) + (Color1 * Percent1) + 0x80UL) >> 8UL);
}

static UINT32 Diag_GdmaAlphaBlendPixelMode0(AMBA_GDMA_32BIT_DATA SrcPixel, AMBA_GDMA_32BIT_DATA DstPixel)
{
    AMBA_DIAG_GDAM_ARGB_s SrcPel, DstPel;
    UINT32 Alpha, Red, Green, Blue;
    UINT32 Color, SrcPercent, DstPercent;

    AmbaMisra_TypeCast32(&SrcPel, &SrcPixel);
    AmbaMisra_TypeCast32(&DstPel, &DstPixel);

    if (SrcPel.Alpha == 0U) {
        SrcPercent = 0UL;
        DstPercent = 0x100UL;
    } else {
        SrcPercent = (UINT32)SrcPel.Alpha + 1UL;
        DstPercent = 0x0FFUL - SrcPel.Alpha;
    }

    Alpha = Diag_GdmaBlendColor((UINT32)SrcPel.Alpha, SrcPercent, (UINT32)DstPel.Alpha, DstPercent);
    Red   = Diag_GdmaBlendColor((UINT32)SrcPel.Red,   SrcPercent, (UINT32)DstPel.Red,   DstPercent);
    Green = Diag_GdmaBlendColor((UINT32)SrcPel.Green, SrcPercent, (UINT32)DstPel.Green, DstPercent);
    Blue  = Diag_GdmaBlendColor((UINT32)SrcPel.Blue,  SrcPercent, (UINT32)DstPel.Blue,  DstPercent);

    Color = (Alpha << 24UL) | (Red << 16UL) | (Green << 8UL) | Blue;
    return Color;
}

static UINT32 Diag_GdmaAlphaBlendPixelMode1(AMBA_GDMA_32BIT_DATA SrcPixel, AMBA_GDMA_32BIT_DATA DstPixel)
{
    AMBA_DIAG_GDAM_ARGB_s SrcPel, DstPel;
    UINT32 Alpha, Red, Green, Blue;
    UINT32 Color, SrcPercent, DstPercent;

    AmbaMisra_TypeCast32(&SrcPel, &SrcPixel);
    AmbaMisra_TypeCast32(&DstPel, &DstPixel);

    SrcPercent = 0x100UL;       /* Source image is already multiplied alpha value */
    if (SrcPel.Alpha == 0U) {
        DstPercent = 0x100UL;
    } else {
        DstPercent = 0x0FFUL - (UINT32)SrcPel.Alpha;
    }

    Alpha = Diag_GdmaBlendColor((UINT32)SrcPel.Alpha, SrcPercent, (UINT32)DstPel.Alpha, DstPercent);
    Red   = Diag_GdmaBlendColor((UINT32)SrcPel.Red,   SrcPercent, (UINT32)DstPel.Red,   DstPercent);
    Green = Diag_GdmaBlendColor((UINT32)SrcPel.Green, SrcPercent, (UINT32)DstPel.Green, DstPercent);
    Blue  = Diag_GdmaBlendColor((UINT32)SrcPel.Blue,  SrcPercent, (UINT32)DstPel.Blue,  DstPercent);

    Color = (Alpha << 24UL) | (Red << 16UL) | (Green << 8UL) | Blue;
    return Color;
}

static void Diag_GdmaAlphaBlendMode0(UINT32 Width, UINT32 Height, UINT32 RowStride)
{
    AMBA_GDMA_BLOCK_s GdmaComposite;
    AMBA_GDMA_32BIT_DATA *pGdmaSrc, *pGdmaDst, *pGolden;
    ULONG GdmaSrcAddr, GdmaDstAddr, GdmaGoldAddr;
    UINT32 Index1, Index2;
    UINT32 i, j;
    UINT32 DataSize;
    UINT32 uRet;

    DataSize = RowStride * Height;

    if (DataSize > DIAG_GDMA_IMG_BUF_SIZE) {
        printf("[Diag][GDMA] ERROR: Data size is too large.\n");
    } else {
        get_work_buf(&GdmaSrcAddr, &i);
        GdmaDstAddr = GdmaSrcAddr + DIAG_GDMA_IMG_BUF_SIZE;
        GdmaGoldAddr = GdmaDstAddr + DIAG_GDMA_IMG_BUF_SIZE;
        AmbaMisra_TypeCast(&pGdmaSrc, &GdmaSrcAddr);
        AmbaMisra_TypeCast(&pGdmaDst, &GdmaDstAddr);
        AmbaMisra_TypeCast(&pGolden, &GdmaGoldAddr);

        GdmaComposite.PixelFormat = AMBA_GDMA_32_BIT;
        GdmaComposite.BltWidth = (UINT16)Width;
        GdmaComposite.BltHeight = (UINT16)Height;
        GdmaComposite.DstRowStride = (UINT16)RowStride;
        GdmaComposite.SrcRowStride = (UINT16)RowStride;
        GdmaComposite.pDstImg = pGdmaDst;
        GdmaComposite.pSrcImg = pGdmaSrc;

        if (AmbaDiagGdmaCtrl.ShowDbgMsg != 0U) {
            Diag_GdmaPrintBufferInfo(pGdmaSrc, pGdmaDst, pGolden);
        }

        (void)memset(pGdmaSrc, 0, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);
        (void)memset(pGdmaDst, 0, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);

        Diag_GdmaGen32bppSrcImg(DataSize, pGdmaSrc);
        Diag_GdmaGen32bppDstImg(DataSize, pGdmaDst);

        /* Generated Golden Data */
        for (i = 0; i < DataSize; i++) {
            pGolden[i] = pGdmaDst[i];
        }

        for (i = 0; i < GdmaComposite.BltHeight; i++) {
            for (j = 0; j < GdmaComposite.BltWidth; j++) {
                Index1 = ((i * GdmaComposite.DstRowStride) / 4U) + j;
                Index2 = ((i * GdmaComposite.SrcRowStride) / 4U) + j;
                pGolden[Index1] = Diag_GdmaAlphaBlendPixelMode0(pGdmaSrc[Index2], pGdmaDst[Index1]);
            }
        }

        /* Start GDMA function */
        (void)AmbaCache_DataClean(GdmaSrcAddr, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);
        (void)AmbaCache_DataClean(GdmaDstAddr, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);
        (void)AmbaCache_DataInvalidate(GdmaDstAddr, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);

        uRet = AmbaGDMA_AlphaBlending(&GdmaComposite, AmbaDiagGdmaCtrl.Alpha, 0U, NULL, 0U, DIAG_GDMA_TIMEOUT);
        if (uRet != 0U) {
            printf("AmbaGDMA_AlphaBlending() return 0x%X", uRet);
        }

        if (AmbaGDMA_WaitAllCompletion(DIAG_GDMA_TIMEOUT) != 0U) {
            printf("[Diag][GDMA] ERROR: Failed to wait for completion.\n");
        }

        /* Verify data */
        Diag_GdmaVerifyData(pGdmaDst, pGolden, DataSize, GdmaComposite.PixelFormat);
    }
}

static void Diag_GdmaAlphaBlendMode1(UINT32 Width, UINT32 Height, UINT32 RowStride)
{
    AMBA_GDMA_BLOCK_s GdmaComposite;
    AMBA_GDMA_32BIT_DATA *pGdmaSrc, *pGdmaDst, *pGolden;
    ULONG GdmaSrcAddr, GdmaDstAddr, GdmaGoldAddr;
    UINT32 i, j;
    UINT32 Index1, Index2;
    UINT32 DataSize;
    UINT32 uRet;

    DataSize = RowStride * Height;

    if (DataSize > DIAG_GDMA_IMG_BUF_SIZE) {
        printf("[Diag][GDMA] ERROR: Data size is too large.\n");
    } else {
        get_work_buf(&GdmaSrcAddr, &i);
        GdmaDstAddr = GdmaSrcAddr + DIAG_GDMA_IMG_BUF_SIZE;
        GdmaGoldAddr = GdmaDstAddr + DIAG_GDMA_IMG_BUF_SIZE;
        AmbaMisra_TypeCast(&pGdmaSrc, &GdmaSrcAddr);
        AmbaMisra_TypeCast(&pGdmaDst, &GdmaDstAddr);
        AmbaMisra_TypeCast(&pGolden, &GdmaGoldAddr);

        GdmaComposite.PixelFormat = AMBA_GDMA_32_BIT;
        GdmaComposite.BltWidth = (UINT16)Width;
        GdmaComposite.BltHeight = (UINT16)Height;
        GdmaComposite.DstRowStride = (UINT16)RowStride;
        GdmaComposite.SrcRowStride = (UINT16)RowStride;
        GdmaComposite.pDstImg = pGdmaDst;
        GdmaComposite.pSrcImg = pGdmaSrc;

        if (AmbaDiagGdmaCtrl.ShowDbgMsg != 0U) {
            Diag_GdmaPrintBufferInfo(pGdmaSrc, pGdmaDst, pGolden);
        }

        (void)memset(pGdmaSrc, 0, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);
        (void)memset(pGdmaDst, 0, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);

        Diag_GdmaGen32bppSrcImg(DataSize, pGdmaSrc);
        Diag_GdmaGen32bppDstImg(DataSize, pGdmaDst);

        /* Generated Golden Data */
        for (i = 0; i < DataSize; i++) {
            pGolden[i] = pGdmaDst[i];
        }

        for (i = 0; i < GdmaComposite.BltHeight ; i++) {
            for (j = 0; j < GdmaComposite.BltWidth; j++) {
                Index1 = (i * GdmaComposite.DstRowStride / 4UL) + j;
                Index2 = (i * GdmaComposite.SrcRowStride / 4UL) + j;
                pGolden[Index1] = Diag_GdmaAlphaBlendPixelMode1(pGdmaSrc[Index2], pGdmaDst[Index1]);
            }
        }

        /* Start GDMA function */
        (void)AmbaCache_DataClean(GdmaSrcAddr, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);
        (void)AmbaCache_DataClean(GdmaDstAddr, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);
        (void)AmbaCache_DataInvalidate(GdmaDstAddr, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);

        uRet = AmbaGDMA_AlphaBlending(&GdmaComposite, AmbaDiagGdmaCtrl.Alpha, 1U, NULL, 0U, DIAG_GDMA_TIMEOUT);
        if (uRet != 0U) {
            printf("AmbaGDMA_AlphaBlending() return 0x%X", uRet);
        }

        if (AmbaGDMA_WaitAllCompletion(DIAG_GDMA_TIMEOUT) != 0U) {
            printf("[Diag][GDMA] ERROR: Failed to wait for completion.\n");
        }

        /* Verify data */
        Diag_GdmaVerifyData(pGdmaDst, pGolden, DataSize, GdmaComposite.PixelFormat);
    }
}

static void Diag_GdmaUnitTest()
{
    const AMBA_DIAG_GDMA_CTRL_s *pGdmaCtrl = &AmbaDiagGdmaCtrl;

    printf("\n");
    printf("[Diag][GDMA] ==================================================\n");
    printf("[Diag][GDMA] 1. Test linear copy:\n");
    Diag_GdmaLinearCopy(pGdmaCtrl->TestWidth);
    printf("[Diag][GDMA] ==================================================\n");
    printf("[Diag][GDMA] 2. Test block copy:\n");
    Diag_GdmaBlockCopy(pGdmaCtrl->TestWidth, pGdmaCtrl->TestHeight, pGdmaCtrl->TestRowStride);
    printf("[Diag][GDMA] ==================================================\n");
    printf("[Diag][GDMA] 3. Test color keying:\n");
    Diag_GdmaColorKeying(pGdmaCtrl->TestWidth, pGdmaCtrl->TestHeight, pGdmaCtrl->TestRowStride);
    printf("[Diag][GDMA] ==================================================\n");
    printf("[Diag][GDMA] 4. Test block composite with alpha premultiplied:\n");
    Diag_GdmaAlphaBlendMode1(pGdmaCtrl->TestWidth, pGdmaCtrl->TestHeight, pGdmaCtrl->TestRowStride);
    printf("[Diag][GDMA] ==================================================\n");
    printf("[Diag][GDMA] 5. Test block composite with alpha non-premultiplied:\n");
    Diag_GdmaAlphaBlendMode0(pGdmaCtrl->TestWidth, pGdmaCtrl->TestHeight, pGdmaCtrl->TestRowStride);
    printf("[Diag][GDMA] ==================================================\n");
}

#if 0
static void Diag_GdmaMeasureThruput(UINT32 TimeInterval, UINT32 NumBytes, const AMBA_GDMA_LINEAR_s *pLinearCopy)
{
    UINT32 RetVal, TimeElapsed, TimeStart = 0, TimeEnd = 0;
    UINT64 DataRate, NumTrans = 0;

    (void)AmbaKAL_GetSysTickCount(&TimeStart);
    do {
        RetVal = AmbaGDMA_LinearCopy(pLinearCopy, NULL, 0U, DIAG_GDMA_TIMEOUT);
        if (RetVal != OK) {
            printf("[Diag][GDMA] Unable to do linear copy.\n");
            break;
        }

        (void)AmbaKAL_GetSysTickCount(&TimeEnd);
        NumTrans += (UINT64)NumBytes;
    } while ((TimeEnd - TimeStart) >= (TimeInterval * 1000U));

    if (RetVal == OK) {
        RetVal = AmbaGDMA_WaitAllCompletion(DIAG_GDMA_TIMEOUT);
        if (RetVal != OK) {
            printf("[Diag][GDMA] Unable to wait for completion.\n");
        } else {
            /* Get the actual time_diff after all the transfers done */
            (void)AmbaKAL_GetSysTickCount(&TimeEnd);
            if (TimeStart < TimeEnd) {
                TimeElapsed = TimeEnd - TimeStart;
            } else {
                TimeElapsed = ((0xFFFFFFFFUL - TimeStart) + TimeEnd + 1U);
            }

            printf("[Diag][GDMA] GDMA transaction length is %d", NumBytes);
            printf(" bytes. There are %d", (UINT32)NumTrans);
            printf(" bytes transferred within %d", TimeElapsed);
            printf(" milliseconds.\n");

            DataRate = (NumTrans * 1000ULL) / (UINT64)TimeElapsed;  /* Get Bytes/s */
            DataRate >>= 10ULL;                                     /* Get KBytes/s */

            printf("[Diag][GDMA] Estimated transfer rate is %d", DataRate);
            printf(" KB/s.\n");
        }
    }
}
#endif

static void Diag_GdmaCmdUsage(void)
{
    printf("Usage: gdma [test|unittest|thruput|debug|isr] <...>\n");
    printf("       gdma unittest        : Sanity testing\n");
    printf("       gdma test <test_id> : Single function test\n");
    printf("         <test_id>:\n");
    printf("           1: Test Gdma LinearTransfer\n");
    printf("           2: Test Gdma BlockTransfer\n");
    printf("           3: Test Gdma ColorKeying\n");
    printf("           4: Test Gdma BlockCompositeWithAlphaPreMultiplied\n");
    printf("           5: Test Gdma BlockCompositeWithAlphaNonPreMultiplied\n");
    printf("       gdma debug [on|off] : Turn on/off debug messages\n");
    printf(" Ex:   gdma test [1] + <size>\n");
    printf("       gdma test [2|3|4|5] + <width> <height> <pitch>\n");
    printf("       gdma thruput + <seconds>\n");
    printf("       gdma unittest\n");
}

int DoGdmaDiag(int argc, char *argv[])
{
    UINT32 Width = 0, Height = 0, RowStride = 0;
    UINT32 TestID;
    char **ptr = NULL;

    if (argc < 3) {
        Diag_GdmaCmdUsage();
    } else {
        if (strcmp("unittest", argv[2]) == 0) {
            Diag_GdmaUnitTest();
        } else if (strcmp("test", argv[2]) == 0) {
            if (argc < 5) {
                Diag_GdmaCmdUsage();
            } else {
                TestID = strtoul(argv[3], ptr, 0);
                Width = strtoul(argv[4], ptr, 0);

                if (TestID == 1U) {
                    Diag_GdmaLinearCopy(Width);
                } else if (TestID < 6U) {
                    if (argc < 7) {
                        Diag_GdmaCmdUsage();
                    } else {
                        Height = strtoul(argv[5], ptr, 0);
                        RowStride = strtoul(argv[6], ptr, 0);

                        if (TestID == 2U) {
                            Diag_GdmaBlockCopy(Width, Height, RowStride);
                        } else if (TestID == 3U) {
                            Diag_GdmaColorKeying(Width, Height, RowStride);
                        } else if (TestID == 4U) {
                            Diag_GdmaAlphaBlendMode1(Width, Height, RowStride);
                        } else if (TestID == 5U) {
                            Diag_GdmaAlphaBlendMode0(Width, Height, RowStride);
                        } else {
                            // pass vcast check
                        }
                    }
                } else {
                    Diag_GdmaCmdUsage();
                }
            }
        } else {
            Diag_GdmaCmdUsage();
        }
    }

    return 0;
}

