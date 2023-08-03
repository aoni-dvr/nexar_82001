/**
 *  @file AmbaIOUTDiag_CmdGDMA.c
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
 *  @details GDMA diagnostic command.
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaShell.h"
#include "AmbaShell_Utility.h"

#include "AmbaIOUTDiag.h"
#include "AmbaCache.h"
#include "AmbaGDMA.h"
#include "AmbaPIO.h"

#define DIAG_GDMA_TIMEOUT       (3000U)
#define DIAG_GDMA_STR_BUF_SIZE  (250U)
#define DIAG_GDMA_IMG_BUF_SIZE  (512U * 1024U)

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

static AMBA_PIO_s PioSrc, PioDst;
static UINT32 GdmaGoldenBuffer[DIAG_GDMA_IMG_BUF_SIZE] __attribute__ ((aligned (AMBA_CACHE_LINE_SIZE)));
static char DiagGdmaStrBuf[DIAG_GDMA_STR_BUF_SIZE];

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

static UINT32 Diag_GdmaGetAddr(const void *pPtr)
{
    UINT32 Value = 0;

    AmbaMisra_TypeCast32(&Value, &pPtr);

    return Value;
}

static void Diag_GdmaPrintBufferInfo(AMBA_SHELL_PRINT_f PrintFunc, const AMBA_GDMA_32BIT_DATA *pSrc, const AMBA_GDMA_32BIT_DATA *pDst, const AMBA_GDMA_32BIT_DATA *pGolden)
{
    UINT32 ArgList[2];

    ArgList[0] = Diag_GdmaGetAddr(pSrc);
    (void)AmbaUtility_StringPrintUInt32(DiagGdmaStrBuf, DIAG_GDMA_STR_BUF_SIZE, "[Diag][GDMA] Source Image Base:      0x%X\n", 1U, ArgList);
    PrintFunc(DiagGdmaStrBuf);

    ArgList[0] = Diag_GdmaGetAddr(pDst);
    (void)AmbaUtility_StringPrintUInt32(DiagGdmaStrBuf, DIAG_GDMA_STR_BUF_SIZE, "[Diag][GDMA] Destination Image Base: 0x%X\n", 1U, ArgList);
    PrintFunc(DiagGdmaStrBuf);

    if (pGolden != NULL) {
        ArgList[0] = Diag_GdmaGetAddr(pGolden);
        (void)AmbaUtility_StringPrintUInt32(DiagGdmaStrBuf, DIAG_GDMA_STR_BUF_SIZE, "[Diag][GDMA] Golden Image Base:      0x%X\n", 1U, ArgList);
        PrintFunc(DiagGdmaStrBuf);
    }
}

static void Diag_GdmaVerifyData(AMBA_SHELL_PRINT_f PrintFunc, const AMBA_GDMA_32BIT_DATA *pData1, const AMBA_GDMA_32BIT_DATA *pData2,
                                UINT32 DataSize, UINT32 PixelFormat)
{
    UINT32 i, Args[5];
    INT32  CmpResult;

    (void)AmbaWrap_memcmp(pData1, pData2, DataSize * DiagGdmaPixelWidth[PixelFormat] / 8U, &CmpResult);
    if (CmpResult != 0) {
        PrintFunc("[Diag][GDMA] Error: Data mismatch!!!\n");
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
                    (void)AmbaUtility_StringPrintUInt32(DiagGdmaStrBuf, DIAG_GDMA_STR_BUF_SIZE, "[Diag][GDMA] Data %uth, [0x%x]=%08x, [0x%x]=%08x\n", 5U, Args);
                    PrintFunc(DiagGdmaStrBuf);
                }
            }
        }
    } else {
        PrintFunc("[Diag][GDMA] GDMA function done with no errors\n");
    }
}


static void Diag_GdmaLinearCopy(AMBA_SHELL_PRINT_f PrintFunc, UINT32 NumPixels)
{
    AMBA_GDMA_LINEAR_s LinearBlit;
    AMBA_GDMA_32BIT_DATA *pGdmaSrcVir, *pGdmaDstVir, *pGdmaSrcPhy, *pGdmaDstPhy;
    UINT32 Offset = 0;
    UINT32 Remainder = 0;
    UINT32 uRet;
    UINT32 args[2];

    if (NumPixels > DIAG_GDMA_IMG_BUF_SIZE) {
        PrintFunc("[Diag][GDMA] ERROR: Data size is too large.\n");
    } else {
        AmbaMisra_TypeCast(&pGdmaSrcVir, &PioSrc.VirAddr);
        AmbaMisra_TypeCast(&pGdmaDstVir, &PioDst.VirAddr);
        AmbaMisra_TypeCast(&pGdmaSrcPhy, &PioSrc.PhyAddr);
        AmbaMisra_TypeCast(&pGdmaDstPhy, &PioDst.PhyAddr);

        LinearBlit.PixelFormat = AMBA_GDMA_32_BIT;
        LinearBlit.NumPixels = NumPixels;

        if (AmbaDiagGdmaCtrl.ShowDbgMsg != 0U) {
            Diag_GdmaPrintBufferInfo(PrintFunc, pGdmaSrcVir, pGdmaDstVir, NULL);
        }

        (void)AmbaWrap_memset(pGdmaSrcVir, 0x11, NumPixels * DiagGdmaPixelWidth[LinearBlit.PixelFormat] / 8U);
        (void)AmbaWrap_memset(pGdmaDstVir, 0x22, NumPixels * DiagGdmaPixelWidth[LinearBlit.PixelFormat] / 8U);

        Diag_GdmaGen32bppSrcImg(NumPixels, pGdmaSrcVir);
        Diag_GdmaGen32bppDstImg(NumPixels, pGdmaDstVir);

        (void)AmbaPIO_CacheClean(&PioSrc);
        (void)AmbaPIO_CacheInvalid(&PioDst);

        /* Start GDMA function */
        Remainder = NumPixels;
        while (Remainder > 0U) {
            LinearBlit.pSrcImg = &pGdmaSrcPhy[Offset];
            LinearBlit.pDstImg = &pGdmaDstPhy[Offset];

            if (Remainder > AMBA_GDMA_MAX_WIDTH) {
                LinearBlit.NumPixels = AMBA_GDMA_MAX_WIDTH;
                uRet = AmbaGDMA_LinearCopy(&LinearBlit, NULL, 0U, DIAG_GDMA_TIMEOUT);
            } else {
                LinearBlit.NumPixels = Remainder;
                uRet = AmbaGDMA_LinearCopy(&LinearBlit, NULL, 0U, DIAG_GDMA_TIMEOUT);
            }
            if (uRet != 0U) {
                args[0] = uRet;
                (void)AmbaUtility_StringPrintUInt32(DiagGdmaStrBuf, DIAG_GDMA_STR_BUF_SIZE, "[Diag][GDMA] ERROR: 0x%X\n", 1, args);
                PrintFunc(DiagGdmaStrBuf);
                break;
            }
            Offset += AMBA_GDMA_MAX_WIDTH;
            Remainder -= LinearBlit.NumPixels;
        }

        if (AmbaGDMA_WaitAllCompletion(DIAG_GDMA_TIMEOUT) != 0U) {
            PrintFunc("[Diag][GDMA] ERROR: Failed to wait for completion.\n");
        }

        (void)AmbaPIO_CacheInvalid(&PioDst);

        /* Verify data */
        Diag_GdmaVerifyData(PrintFunc, pGdmaSrcVir, pGdmaDstVir, NumPixels, LinearBlit.PixelFormat);
    }
}

static void Diag_GdmaBlockCopy(AMBA_SHELL_PRINT_f PrintFunc, UINT32 Width, UINT32 Height, UINT32 RowStride)
{
    AMBA_GDMA_BLOCK_s BlockBlit;
    AMBA_GDMA_32BIT_DATA *pGdmaSrcVir, *pGdmaDstVir, *pGdmaSrcPhy, *pGdmaDstPhy, *pGolden;
    UINT32 i, j;
    UINT32 DataSize;
    UINT32 uRet;
    UINT32 args[2];

    DataSize = RowStride * Height;

    if (DataSize > DIAG_GDMA_IMG_BUF_SIZE) {
        PrintFunc("[Diag][GDMA] ERROR: Data size is too large.\n");
    } else {
        AmbaMisra_TypeCast(&pGdmaSrcVir, &PioSrc.VirAddr);
        AmbaMisra_TypeCast(&pGdmaDstVir, &PioDst.VirAddr);
        AmbaMisra_TypeCast(&pGdmaSrcPhy, &PioSrc.PhyAddr);
        AmbaMisra_TypeCast(&pGdmaDstPhy, &PioDst.PhyAddr);

        pGolden  = GdmaGoldenBuffer;

        BlockBlit.PixelFormat = AMBA_GDMA_32_BIT;
        BlockBlit.BltWidth = Width;
        BlockBlit.BltHeight = Height;
        BlockBlit.DstRowStride = RowStride;
        BlockBlit.SrcRowStride = RowStride;
        BlockBlit.pDstImg = pGdmaDstPhy;
        BlockBlit.pSrcImg = pGdmaSrcPhy;

        if (AmbaDiagGdmaCtrl.ShowDbgMsg != 0U) {
            Diag_GdmaPrintBufferInfo(PrintFunc, pGdmaSrcVir, pGdmaDstVir, pGolden);
        }

        (void)AmbaWrap_memset(pGdmaSrcVir, 0x11, DataSize * DiagGdmaPixelWidth[BlockBlit.PixelFormat] / 8U);
        (void)AmbaWrap_memset(pGdmaDstVir, 0x22, DataSize * DiagGdmaPixelWidth[BlockBlit.PixelFormat] / 8U);

        Diag_GdmaGen32bppSrcImg(DataSize, pGdmaSrcVir);
        Diag_GdmaGen32bppDstImg(DataSize, pGdmaDstVir);

        /* Generated Golden Data */
        for (i = 0; i < DataSize; i++) {
            pGolden[i] = pGdmaDstVir[i];
        }

        for (i = 0; i < BlockBlit.BltHeight ; i++) {
            for (j = 0; j < BlockBlit.BltWidth; j++) {
                pGolden[ (i * BlockBlit.DstRowStride / 4UL) + j] = pGdmaSrcVir[(i * BlockBlit.SrcRowStride / 4UL) + j];
            }
        }

        /* Start GDMA function */
        (void)AmbaPIO_CacheClean(&PioSrc);
        (void)AmbaPIO_CacheClean(&PioDst);
        (void)AmbaPIO_CacheInvalid(&PioDst);

        uRet = AmbaGDMA_BlockCopy(&BlockBlit, NULL, 0U, DIAG_GDMA_TIMEOUT);
        if (uRet != 0U) {
            args[0] = uRet;
            (void)AmbaUtility_StringPrintUInt32(DiagGdmaStrBuf, DIAG_GDMA_STR_BUF_SIZE, "AmbaGDMA_BlockCopy() return 0x%X", 1, args);
            PrintFunc(DiagGdmaStrBuf);
        }

        if (AmbaGDMA_WaitAllCompletion(DIAG_GDMA_TIMEOUT) != 0U) {
            PrintFunc("[Diag][GDMA] ERROR: Failed to wait for completion.\n");
        }

        /* Verify data */
        Diag_GdmaVerifyData(PrintFunc, pGdmaDstVir, pGolden, DataSize, BlockBlit.PixelFormat);

    }

    return;
}

static void Diag_GdmaColorKeying(AMBA_SHELL_PRINT_f PrintFunc, UINT32 Width, UINT32 Height, UINT32 RowStride)
{
    AMBA_GDMA_BLOCK_s GdmaComposite;
    AMBA_GDMA_32BIT_DATA *pGdmaSrcVir, *pGdmaDstVir, *pGdmaSrcPhy, *pGdmaDstPhy, *pGolden;
    UINT32 i, j;
    UINT32 DataSize;
    UINT32 uRet;
    UINT32 args[2];

    DataSize = RowStride * Height;

    if (DataSize > DIAG_GDMA_IMG_BUF_SIZE) {
        PrintFunc("[Diag][GDMA] ERROR: Data size is too large.\n");
    } else {
        AmbaMisra_TypeCast(&pGdmaSrcVir, &PioSrc.VirAddr);
        AmbaMisra_TypeCast(&pGdmaDstVir, &PioDst.VirAddr);
        AmbaMisra_TypeCast(&pGdmaSrcPhy, &PioSrc.PhyAddr);
        AmbaMisra_TypeCast(&pGdmaDstPhy, &PioDst.PhyAddr);

        pGolden  = GdmaGoldenBuffer;

        GdmaComposite.PixelFormat = AMBA_GDMA_32_BIT;
        GdmaComposite.BltWidth = Width;
        GdmaComposite.BltHeight = Height;
        GdmaComposite.DstRowStride = RowStride;
        GdmaComposite.SrcRowStride = RowStride;
        GdmaComposite.pDstImg = pGdmaDstPhy;
        GdmaComposite.pSrcImg = pGdmaSrcPhy;

        if (AmbaDiagGdmaCtrl.ShowDbgMsg != 0U) {
            Diag_GdmaPrintBufferInfo(PrintFunc, pGdmaSrcVir, pGdmaDstVir, pGolden);
        }

        (void)AmbaWrap_memset(pGdmaSrcVir, 0, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);
        (void)AmbaWrap_memset(pGdmaDstVir, 0, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);

        Diag_GdmaGen32bppSrcImg(DataSize, pGdmaSrcVir);
        Diag_GdmaGen32bppDstImg(DataSize, pGdmaDstVir);

        /* Generated Golden Data */
        for (i = 0; i < DataSize; i++) {
            pGolden[i] = pGdmaDstVir[i];
        }

        for (i = 0; i < GdmaComposite.BltHeight ; i++) {
            for (j = 0; j < GdmaComposite.BltWidth; j++) {
                if (pGdmaSrcVir[(i * GdmaComposite.SrcRowStride / 4UL) + j] != AmbaDiagGdmaCtrl.TansparentColor) {
                    pGolden[(i * GdmaComposite.DstRowStride / 4UL) + j] = pGdmaSrcVir[(i * GdmaComposite.SrcRowStride / 4UL) + j];
                }
            }
        }

        /* Start GDMA function */
        (void)AmbaPIO_CacheClean(&PioSrc);
        (void)AmbaPIO_CacheClean(&PioDst);
        (void)AmbaPIO_CacheInvalid(&PioDst);

        uRet = AmbaGDMA_ColorKeying(&GdmaComposite, AmbaDiagGdmaCtrl.TansparentColor, NULL, 0U, DIAG_GDMA_TIMEOUT);
        if (uRet != 0U) {
            args[0] = uRet;
            (void)AmbaUtility_StringPrintUInt32(DiagGdmaStrBuf, DIAG_GDMA_STR_BUF_SIZE, "AmbaGDMA_ColorKeying() return 0x%X", 1, args);
            PrintFunc(DiagGdmaStrBuf);
        }

        if (AmbaGDMA_WaitAllCompletion(DIAG_GDMA_TIMEOUT) != 0U) {
            PrintFunc("[Diag][GDMA] ERROR: Failed to wait for completion.\n");
        }

        /* Verify data */
        Diag_GdmaVerifyData(PrintFunc, pGdmaDstVir, pGolden, DataSize, GdmaComposite.PixelFormat);

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

static void Diag_GdmaAlphaBlendMode0(AMBA_SHELL_PRINT_f PrintFunc, UINT32 Width, UINT32 Height, UINT32 RowStride)
{
    AMBA_GDMA_BLOCK_s GdmaComposite;
    AMBA_GDMA_32BIT_DATA *pGdmaSrcVir, *pGdmaDstVir, *pGdmaSrcPhy, *pGdmaDstPhy, *pGolden;
    UINT32 Index1, Index2;
    UINT32 i, j;
    UINT32 DataSize;
    UINT32 uRet;
    UINT32 args[2];

    DataSize = RowStride * Height;

    if (DataSize > DIAG_GDMA_IMG_BUF_SIZE) {
        PrintFunc("[Diag][GDMA] ERROR: Data size is too large.\n");
    } else {
        AmbaMisra_TypeCast(&pGdmaSrcVir, &PioSrc.VirAddr);
        AmbaMisra_TypeCast(&pGdmaDstVir, &PioDst.VirAddr);
        AmbaMisra_TypeCast(&pGdmaSrcPhy, &PioSrc.PhyAddr);
        AmbaMisra_TypeCast(&pGdmaDstPhy, &PioDst.PhyAddr);

        pGolden  = GdmaGoldenBuffer;

        GdmaComposite.PixelFormat = AMBA_GDMA_32_BIT;
        GdmaComposite.BltWidth = (UINT16)Width;
        GdmaComposite.BltHeight = (UINT16)Height;
        GdmaComposite.DstRowStride = (UINT16)RowStride;
        GdmaComposite.SrcRowStride = (UINT16)RowStride;
        GdmaComposite.pDstImg = pGdmaDstPhy;
        GdmaComposite.pSrcImg = pGdmaSrcPhy;

        if (AmbaDiagGdmaCtrl.ShowDbgMsg != 0U) {
            Diag_GdmaPrintBufferInfo(PrintFunc, pGdmaSrcVir, pGdmaDstVir, pGolden);
        }

        (void)AmbaWrap_memset(pGdmaSrcVir, 0, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);
        (void)AmbaWrap_memset(pGdmaDstVir, 0, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);

        Diag_GdmaGen32bppSrcImg(DataSize, pGdmaSrcVir);
        Diag_GdmaGen32bppDstImg(DataSize, pGdmaDstVir);

        /* Generated Golden Data */
        for (i = 0; i < DataSize; i++) {
            pGolden[i] = pGdmaDstVir[i];
        }

        for (i = 0; i < GdmaComposite.BltHeight; i++) {
            for (j = 0; j < GdmaComposite.BltWidth; j++) {
                Index1 = ((i * GdmaComposite.DstRowStride) / 4U) + j;
                Index2 = ((i * GdmaComposite.SrcRowStride) / 4U) + j;
                pGolden[Index1] = Diag_GdmaAlphaBlendPixelMode0(pGdmaSrcVir[Index2], pGdmaDstVir[Index1]);
            }
        }

        /* Start GDMA function */
        (void)AmbaPIO_CacheClean(&PioSrc);
        (void)AmbaPIO_CacheClean(&PioDst);
        (void)AmbaPIO_CacheInvalid(&PioDst);

        uRet = AmbaGDMA_AlphaBlending(&GdmaComposite, AmbaDiagGdmaCtrl.Alpha, 0U, NULL, 0U, DIAG_GDMA_TIMEOUT);
        if (uRet != 0U) {
            args[0] = uRet;
            (void)AmbaUtility_StringPrintUInt32(DiagGdmaStrBuf, DIAG_GDMA_STR_BUF_SIZE, "AmbaGDMA_AlphaBlending() return 0x%X", 1, args);
            PrintFunc(DiagGdmaStrBuf);
        }

        if (AmbaGDMA_WaitAllCompletion(DIAG_GDMA_TIMEOUT) != 0U) {
            PrintFunc("[Diag][GDMA] ERROR: Failed to wait for completion.\n");
        }

        /* Verify data */
        Diag_GdmaVerifyData(PrintFunc, pGdmaDstVir, pGolden, DataSize, GdmaComposite.PixelFormat);
    }
}

static void Diag_GdmaAlphaBlendMode1(AMBA_SHELL_PRINT_f PrintFunc, UINT32 Width, UINT32 Height, UINT32 RowStride)
{
    AMBA_GDMA_BLOCK_s GdmaComposite;
    AMBA_GDMA_32BIT_DATA *pGdmaSrcVir, *pGdmaDstVir, *pGdmaSrcPhy, *pGdmaDstPhy, *pGolden;
    UINT32 i, j;
    UINT32 Index1, Index2;
    UINT32 DataSize;
    UINT32 uRet;
    UINT32 args[2];

    DataSize = RowStride * Height;

    if (DataSize > DIAG_GDMA_IMG_BUF_SIZE) {
        PrintFunc("[Diag][GDMA] ERROR: Data size is too large.\n");
    } else {
        AmbaMisra_TypeCast(&pGdmaSrcVir, &PioSrc.VirAddr);
        AmbaMisra_TypeCast(&pGdmaDstVir, &PioDst.VirAddr);
        AmbaMisra_TypeCast(&pGdmaSrcPhy, &PioSrc.PhyAddr);
        AmbaMisra_TypeCast(&pGdmaDstPhy, &PioDst.PhyAddr);

        pGolden  = GdmaGoldenBuffer;

        GdmaComposite.PixelFormat = AMBA_GDMA_32_BIT;
        GdmaComposite.BltWidth = (UINT16)Width;
        GdmaComposite.BltHeight = (UINT16)Height;
        GdmaComposite.DstRowStride = (UINT16)RowStride;
        GdmaComposite.SrcRowStride = (UINT16)RowStride;
        GdmaComposite.pDstImg = pGdmaDstPhy;
        GdmaComposite.pSrcImg = pGdmaSrcPhy;

        if (AmbaDiagGdmaCtrl.ShowDbgMsg != 0U) {
            Diag_GdmaPrintBufferInfo(PrintFunc, pGdmaSrcVir, pGdmaDstVir, pGolden);
        }

        (void)AmbaWrap_memset(pGdmaSrcVir, 0, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);
        (void)AmbaWrap_memset(pGdmaDstVir, 0, DataSize * DiagGdmaPixelWidth[GdmaComposite.PixelFormat] / 8U);

        Diag_GdmaGen32bppSrcImg(DataSize, pGdmaSrcVir);
        Diag_GdmaGen32bppDstImg(DataSize, pGdmaDstVir);

        /* Generated Golden Data */
        for (i = 0; i < DataSize; i++) {
            pGolden[i] = pGdmaDstVir[i];
        }

        for (i = 0; i < GdmaComposite.BltHeight ; i++) {
            for (j = 0; j < GdmaComposite.BltWidth; j++) {
                Index1 = (i * GdmaComposite.DstRowStride / 4UL) + j;
                Index2 = (i * GdmaComposite.SrcRowStride / 4UL) + j;
                pGolden[Index1] = Diag_GdmaAlphaBlendPixelMode1(pGdmaSrcVir[Index2], pGdmaDstVir[Index1]);
            }
        }

        /* Start GDMA function */
        (void)AmbaPIO_CacheClean(&PioSrc);
        (void)AmbaPIO_CacheClean(&PioDst);
        (void)AmbaPIO_CacheInvalid(&PioDst);

        uRet = AmbaGDMA_AlphaBlending(&GdmaComposite, AmbaDiagGdmaCtrl.Alpha, 1U, NULL, 0U, DIAG_GDMA_TIMEOUT);
        if (uRet != 0U) {
            args[0] = uRet;
            (void)AmbaUtility_StringPrintUInt32(DiagGdmaStrBuf, DIAG_GDMA_STR_BUF_SIZE, "AmbaGDMA_AlphaBlending() return 0x%X", 1, args);
            PrintFunc(DiagGdmaStrBuf);
        }

        if (AmbaGDMA_WaitAllCompletion(DIAG_GDMA_TIMEOUT) != 0U) {
            PrintFunc("[Diag][GDMA] ERROR: Failed to wait for completion.\n");
        }

        /* Verify data */
        Diag_GdmaVerifyData(PrintFunc, pGdmaDstVir, pGolden, DataSize, GdmaComposite.PixelFormat);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Diag_GdmaUnitTest
 *
 *  @Description:: Test all GDMA functions
 *
 *  @Input      ::
 *      PrintFunc:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void Diag_GdmaUnitTest(AMBA_SHELL_PRINT_f PrintFunc)
{
    const AMBA_DIAG_GDMA_CTRL_s *pGdmaCtrl = &AmbaDiagGdmaCtrl;

    PrintFunc("\n");
    PrintFunc("[Diag][GDMA] ==================================================\n");
    PrintFunc("[Diag][GDMA] 1. Test linear copy:\n");
    Diag_GdmaLinearCopy(PrintFunc, pGdmaCtrl->TestWidth);
    PrintFunc("[Diag][GDMA] ==================================================\n");
    PrintFunc("[Diag][GDMA] 2. Test block copy:\n");
    Diag_GdmaBlockCopy(PrintFunc, pGdmaCtrl->TestWidth, pGdmaCtrl->TestHeight, pGdmaCtrl->TestRowStride);
    PrintFunc("[Diag][GDMA] ==================================================\n");
    PrintFunc("[Diag][GDMA] 3. Test color keying:\n");
    Diag_GdmaColorKeying(PrintFunc, pGdmaCtrl->TestWidth, pGdmaCtrl->TestHeight, pGdmaCtrl->TestRowStride);
    PrintFunc("[Diag][GDMA] ==================================================\n");
    PrintFunc("[Diag][GDMA] 4. Test block composite with alpha premultiplied:\n");
    Diag_GdmaAlphaBlendMode1(PrintFunc, pGdmaCtrl->TestWidth, pGdmaCtrl->TestHeight, pGdmaCtrl->TestRowStride);
    PrintFunc("[Diag][GDMA] ==================================================\n");
    PrintFunc("[Diag][GDMA] 5. Test block composite with alpha non-premultiplied:\n");
    Diag_GdmaAlphaBlendMode0(PrintFunc, pGdmaCtrl->TestWidth, pGdmaCtrl->TestHeight, pGdmaCtrl->TestRowStride);
    PrintFunc("[Diag][GDMA] ==================================================\n");
}

static void Diag_GdmaMeasureThruput(UINT32 TimeInterval, UINT32 NumBytes, const AMBA_GDMA_LINEAR_s *pLinearCopy, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal, TimeElapsed, TimeStart = 0, TimeEnd = 0;
    UINT64 DataRate, NumTrans = 0;

    (void)AmbaKAL_GetSysTickCount(&TimeStart);
    do {
        RetVal = AmbaGDMA_LinearCopy(pLinearCopy, NULL, 0U, DIAG_GDMA_TIMEOUT);
        if (RetVal != OK) {
            PrintFunc("[Diag][GDMA] Unable to do linear copy.\n");
            break;
        }

        (void)AmbaKAL_GetSysTickCount(&TimeEnd);
        NumTrans += (UINT64)NumBytes;
    } while ((TimeEnd - TimeStart) >= (TimeInterval * 1000U));

    if (RetVal == OK) {
        RetVal = AmbaGDMA_WaitAllCompletion(DIAG_GDMA_TIMEOUT);
        if (RetVal != OK) {
            PrintFunc("[Diag][GDMA] Unable to wait for completion.\n");
        } else {
            /* Get the actual time_diff after all the transfers done */
            (void)AmbaKAL_GetSysTickCount(&TimeEnd);
            if (TimeStart < TimeEnd) {
                TimeElapsed = TimeEnd - TimeStart;
            } else {
                TimeElapsed = ((0xFFFFFFFFUL - TimeStart) + TimeEnd + 1U);
            }

            PrintFunc("[Diag][GDMA] GDMA transaction length is ");
            (void)AmbaUtility_UInt32ToStr(DiagGdmaStrBuf, UTIL_MAX_FLOAT_STR_LEN, NumBytes, 10U);
            PrintFunc(DiagGdmaStrBuf);
            PrintFunc(" bytes. There are ");
            (void)AmbaUtility_UInt64ToStr(DiagGdmaStrBuf, UTIL_MAX_FLOAT_STR_LEN, NumTrans, 16U);
            PrintFunc(DiagGdmaStrBuf);
            PrintFunc(" bytes transferred within ");
            (void)AmbaUtility_UInt32ToStr(DiagGdmaStrBuf, UTIL_MAX_FLOAT_STR_LEN, TimeElapsed, 10U);
            PrintFunc(DiagGdmaStrBuf);
            PrintFunc(" milliseconds.\n");

            DataRate = (NumTrans * 1000ULL) / (UINT64)TimeElapsed;  /* Get Bytes/s */
            DataRate >>= 10ULL;                                     /* Get KBytes/s */

            PrintFunc("[Diag][GDMA] Estimated transfer rate is ");
            (void)AmbaUtility_UInt64ToStr(DiagGdmaStrBuf, UTIL_MAX_FLOAT_STR_LEN, DataRate, 10U);
            PrintFunc(DiagGdmaStrBuf);
            PrintFunc(" KB/s.\n");
        }
    }
}

static void Diag_GdmaThruput(UINT32 TimeInterval, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_GDMA_LINEAR_s LinearCopy;
    AMBA_GDMA_32BIT_DATA *pGdmaSrcVir, *pGdmaDstVir, *pGdmaSrcPhy, *pGdmaDstPhy;
    UINT32 NumBytes;

    AmbaMisra_TypeCast(&pGdmaSrcVir, &PioSrc.VirAddr);
    AmbaMisra_TypeCast(&pGdmaDstVir, &PioDst.VirAddr);
    AmbaMisra_TypeCast(&pGdmaSrcPhy, &PioSrc.PhyAddr);
    AmbaMisra_TypeCast(&pGdmaDstPhy, &PioDst.PhyAddr);

    PrintFunc("[Diag][GDMA] Start to measure throughput\n");

    LinearCopy.PixelFormat = AMBA_GDMA_32_BIT;
    LinearCopy.NumPixels = AMBA_GDMA_MAX_WIDTH;
    LinearCopy.pSrcImg = &pGdmaSrcPhy[0U];
    LinearCopy.pDstImg = &pGdmaDstPhy[0U];

    NumBytes = LinearCopy.NumPixels * DiagGdmaPixelWidth[LinearCopy.PixelFormat] / 8U;

    (void)AmbaWrap_memset(pGdmaSrcVir, 0x11, NumBytes);
    (void)AmbaWrap_memset(pGdmaDstVir, 0x22, NumBytes);
    Diag_GdmaGen32bppSrcImg(LinearCopy.NumPixels, pGdmaSrcVir);
    Diag_GdmaGen32bppDstImg(LinearCopy.NumPixels, pGdmaDstVir);

    (void)AmbaPIO_CacheClean(&PioSrc);
    (void)AmbaPIO_CacheInvalid(&PioDst);

    Diag_GdmaMeasureThruput(TimeInterval, NumBytes, &LinearCopy, PrintFunc);
}

static void Diag_GdmaCmdUsage(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    (void)ArgCount;
    (void)pArgVector;

    PrintFunc("Usage: gdma [test|unittest|thruput|debug|isr] <...>\n");
    PrintFunc("       gdma thruput         : Performance evaluation\n");
    PrintFunc("       gdma unittest        : Sanity testing\n");
    PrintFunc("       gdma test <test_id> : Single function test\n");
    PrintFunc("         <test_id>:\n");
    PrintFunc("           1: Test Gdma LinearTransfer\n");
    PrintFunc("           2: Test Gdma BlockTransfer\n");
    PrintFunc("           3: Test Gdma ColorKeying\n");
    PrintFunc("           4: Test Gdma BlockCompositeWithAlphaPreMultiplied\n");
    PrintFunc("           5: Test Gdma BlockCompositeWithAlphaNonPreMultiplied\n");
    PrintFunc("       gdma debug [on|off] : Turn on/off debug messages\n");
    PrintFunc(" Ex:   gdma test [1] + <size>\n");
    PrintFunc("       gdma test [2|3|4|5] + <width> <height> <pitch>\n");
    PrintFunc("       gdma thruput + <seconds>\n");
    PrintFunc("       gdma unittest\n");
}

/**
 *  GDMA diagnostic command
 *  @param[in] ArgCount Argument count
 *  @param[in] pArgVector Argument vector
 *  @param[in] PrintFunc function to print messages on shell task
 *  @return none
 */
void AmbaIOUTDiag_CmdGDMA(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Width = 0, Height = 0, RowStride = 0;
    UINT32 Seconds = 10U;
    UINT32 TestID;
    UINT32 uRet;

    if (ArgCount < 2U) {
        Diag_GdmaCmdUsage(ArgCount, pArgVector, PrintFunc);
    } else {
        if ((0U == PioSrc.VirAddr) || (0U == PioSrc.PhyAddr) || (0U == PioDst.VirAddr) || (0U == PioDst.PhyAddr)) {
            uRet = AmbaPIO_MemAlloc(DIAG_GDMA_IMG_BUF_SIZE, 1U, &PioSrc);
            if (uRet != 0U) {
                PrintFunc("[Diag][GDMA] ERROR: AmbaPIO_MemAlloc failure.\n");
            }
            uRet = AmbaPIO_MemAlloc(DIAG_GDMA_IMG_BUF_SIZE, 1U, &PioDst);
            if (uRet != 0U) {
                PrintFunc("[Diag][GDMA] ERROR: AmbaPIO_MemAlloc failure.\n");
            }
        }

        if (AmbaUtility_StringCompare(pArgVector[1], "unittest", 8) == 0) {
            Diag_GdmaUnitTest(PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[1], "test", 4) == 0) {
            if (ArgCount < 4U) {
                Diag_GdmaCmdUsage(ArgCount, pArgVector, PrintFunc);
            } else {
                (void)AmbaUtility_StringToUInt32(pArgVector[2], &TestID);
                (void)AmbaUtility_StringToUInt32(pArgVector[3], &Width);

                if (TestID == 1U) {
                    Diag_GdmaLinearCopy(PrintFunc, Width);
                } else if (TestID < 6U) {
                    if (ArgCount < 6U) {
                        Diag_GdmaCmdUsage(ArgCount, pArgVector, PrintFunc);
                    } else {

                        (void)AmbaUtility_StringToUInt32(pArgVector[4], &Height);
                        (void)AmbaUtility_StringToUInt32(pArgVector[5], &RowStride);

                        if (TestID == 2U) {
                            Diag_GdmaBlockCopy(PrintFunc, Width, Height, RowStride);
                        } else if (TestID == 3U) {
                            Diag_GdmaColorKeying(PrintFunc, Width, Height, RowStride);
                        } else if (TestID == 4U) {
                            Diag_GdmaAlphaBlendMode1(PrintFunc, Width, Height, RowStride);
                        } else if (TestID == 5U) {
                            Diag_GdmaAlphaBlendMode0(PrintFunc, Width, Height, RowStride);
                        } else {
                            // pass vcast check
                        }
                    }
                } else {
                    Diag_GdmaCmdUsage(ArgCount, pArgVector, PrintFunc);
                }
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "debug", 5) == 0) {
            if (AmbaUtility_StringCompare(pArgVector[2], "on", 2) == 0) {
                AmbaDiagGdmaCtrl.ShowDbgMsg = 1;
                PrintFunc("[Diag][GDMA] debug message on\n");
            } else if (AmbaUtility_StringCompare(pArgVector[2], "off", 3) == 0) {
                AmbaDiagGdmaCtrl.ShowDbgMsg = 0;
                PrintFunc("[Diag][GDMA] debug message off\n");
            } else {
                // pass vcast check
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "thruput", 7) == 0) {
            (void)AmbaUtility_StringToUInt32(pArgVector[2], &Seconds);
            Diag_GdmaThruput(Seconds, PrintFunc);
        } else {
            Diag_GdmaCmdUsage(ArgCount, pArgVector, PrintFunc);
        }
    }
}

