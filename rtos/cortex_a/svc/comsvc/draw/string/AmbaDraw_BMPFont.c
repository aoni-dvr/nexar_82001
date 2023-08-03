/**
 * @file AmbaDraw_BMPFont.c
 *
 * Copyright (c) 2019 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <AmbaMisraFix.h>
#include "AmbaDraw_Common.h"
#include "string/AmbaDraw_BMPFont.h"

/*************************************************************************
 * BMP font definitions
 ************************************************************************/

/*************************************************************************
 * BMP font enum
 ************************************************************************/

/*************************************************************************
 * BMP font struct
 ************************************************************************/

/*************************************************************************
 * BMP font variables
 ************************************************************************/
static AMBADRAW_FONT_s G_FontConfig;
static AMBA_DRAW_BMPFONT_BIN_HEADER_s *G_FontHeader = NULL;
static AMBA_DRAW_BMPFONT_BIN_LANG_s *G_FontTable = NULL;
static AMBA_DRAW_BMPFONT_BIN_FONT_s *G_PageTable = NULL;

/*************************************************************************
 * BMP font APIs
 ************************************************************************/
static UINT32 AmbaDrawBmpFont_ValidCheck(void)
{
    UINT32 Rval;
    if (G_FontConfig.Size == 0U) {
        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawBmpFont_ValidCheck: [ERROR] Invalid Size %u", G_FontConfig.Size, 0U, 0U, 0U, 0U);
        Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    } else if (G_FontConfig.Buffer == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Buffer is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    } else {
        Rval = DRAW_OK;
    }

    return Rval;
}

static UINT32 AmbaDrawBmpFont_Load(UINT8 *FontBuffer, UINT32 FontBufferSize, AMBA_DRAW_BMPFONT_BIN_FONT_INFO_s *FontPageInfo, UINT32 MaxFontPageNum)
{
    UINT32 Rval = DRAW_OK;

    if (FontBuffer == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] FontBuffer is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (FontBufferSize == 0U) {
        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawBmpFont_Load: [ERROR] Invalid FontBufferSize %u", FontBufferSize, 0U, 0U, 0U, 0U);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (FontPageInfo == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] FontPageInfo is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (MaxFontPageNum == 0U) {
        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawBmpFont_Load: [ERROR] Invalid MaxFontPageNum %u", MaxFontPageNum, 0U, 0U, 0U, 0U);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        G_FontConfig.Buffer = FontBuffer;
        G_FontConfig.Size   = FontBufferSize;
        G_FontConfig.FontPageInfo = FontPageInfo;
        G_FontConfig.MaxFontPageNum = MaxFontPageNum;
    }

    return Rval;
}

static UINT32 AmbaDrawBmpFont_InitFromROMFS(void)
{
    UINT32 Rval;

    /* AmbaDrawBmpFont_Load should be called first, and then AmbaDrawBmpFont_InitFromROMFS. */
    Rval = AmbaDrawBmpFont_ValidCheck();
    if (Rval != DRAW_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawBmpFont_ValidCheck failed", __func__, NULL, NULL, NULL, NULL);
    }

    if (Rval == DRAW_OK) {
        const UINT8 *FontTableAddr;
        const UINT8 *PageTableAddr;

        /* Parse Font bin file */
        // TODO: Check G_FontConfig.Size not overflow
        AmbaMisra_TypeCast(&G_FontHeader, &G_FontConfig.Buffer);
        FontTableAddr = &G_FontConfig.Buffer[G_FontHeader->HeaderSize];
        AmbaMisra_TypeCast(&G_FontTable, &FontTableAddr);
        PageTableAddr = &FontTableAddr[(UINT32)G_FontHeader->SizeOfLang * (UINT32)G_FontHeader->LangNum];
        AmbaMisra_TypeCast(&G_PageTable, &PageTableAddr);

        if ((G_FontHeader->Pages > 0U) && (G_FontHeader->Pages <= G_FontConfig.MaxFontPageNum)) {
            /* Process on page info */
            //AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawBmpFont_InitFromROMFS: Font total page %u", (UINT32)G_FontHeader->Pages, 0U, 0U, 0U, 0U);
            for (UINT16 PageIdx = 0; PageIdx < G_FontHeader->Pages; PageIdx++) {
                //AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawBmpFont_InitFromROMFS: Page %u Ascent %u H %u FirstChar %u LastChar %u", (UINT32)PageIdx, (UINT32)G_PageTable[PageIdx].Ascent, (UINT32)G_PageTable[PageIdx].Height, (UINT32)G_PageTable[PageIdx].FirstChar, (UINT32)G_PageTable[PageIdx].LastChar);
                if (G_PageTable[PageIdx].Ascent == 0U) {
                    const UINT8 *OffsetAddr;
                    UINT16 OffsetSize;
                    const UINT16 *DataAddr;
                    AMBA_DRAW_BMPFONT_BIN_FONT_s *CurPage = &G_PageTable[PageIdx];
                    AMBA_DRAW_BMPFONT_BIN_FONT_INFO_s *CurPageInfo = &G_FontConfig.FontPageInfo[PageIdx];
                    // start to load data

                    //CurPage->Offset = (UINT16 *) ((LONG) G_FontConfig.Buffer + (LONG) CurPage->Offset); // Char width for variable-width font
                    OffsetAddr = &G_FontConfig.Buffer[CurPage->Offset];
                    AmbaMisra_TypeCast(&CurPageInfo->Offset, &OffsetAddr);

                    /* Assign data_table pointer */
                    OffsetSize = CurPage->LastChar - CurPage->FirstChar + 2U;

                    //CurPage->Data = (UINT8 *) (CurPage->Offset + OffsetSize);
                    DataAddr = &CurPageInfo->Offset[OffsetSize];
                    AmbaMisra_TypeCast(&CurPageInfo->Data, &DataAddr);

                    /* Page is loaded */
                    CurPage->Ascent = CurPage->Height;
                }
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawBmpFont_InitFromROMFS: [ERROR] Invalid Pages %u", G_FontHeader->Pages, 0U, 0U, 0U, 0U);
            Rval = DRAW_ERROR_PARAM_STRUCTURE;
        }
    }

    return Rval;
}

static inline UINT32 AmbaDrawBmpFont_FindCharPage(UINT32 FontId, char Ch, UINT16 *PageIdx)
{
    UINT32 Rval = DRAW_ERROR_GENERAL_ERROR;
    const UINT8 ChU8 = (UINT8)Ch;
    const UINT16 ChU16 = (UINT16)ChU8;
    //AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawBmpFont_FindCharPage: Lang[%u] H %u First \'%c\' Last \'%c\'", FontId, (UINT32)G_FontTable[FontId].Height, (UINT32)G_FontTable[FontId].First, (UINT32)G_FontTable[FontId].Last, 0U);

    if ((ChU16 >= G_FontTable[FontId].First) && (ChU16 <= G_FontTable[FontId].Last)) {
        /* Find Ch in FontPage */
        for (UINT16 PageCount = G_FontTable[FontId].Start; PageCount <= G_FontTable[FontId].End; PageCount++) {
            if (ChU16 <= G_PageTable[PageCount].LastChar) {
                *PageIdx = PageCount;
                Rval = DRAW_OK;
                break;
            }
        }
    }

    if (Rval != DRAW_OK) {
        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawBmpFont_FindCharPage: [ERROR] FontPage not found. Char %c FontId %u", (UINT32)ChU16, FontId, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 AmbaDrawBmpFont_PutChar(const AMBA_DRAW_RENDER_s *Render, UINT16 PageIdx,
                                      INT32 X, INT32 Y, char Ch, const AMBADRAW_FONT_DRAW_STRING_CONFIG_s *DrawConfig)
{
    const UINT8 *BaseAddr;
    UINT8  CharU8 = (UINT8)Ch;
    UINT32 CharU32 = (UINT32)CharU8;
    INT32 CodeOffset;
    INT32 BitOffset; // bit offset (char width in pixel)
    UINT32 CharWidth;
    INT32 ByteOffset;
    const AMBA_DRAW_BMPFONT_BIN_FONT_s *FontPage = &G_PageTable[PageIdx];
    const AMBA_DRAW_BMPFONT_BIN_FONT_INFO_s *FontPageInfo = &G_FontConfig.FontPageInfo[PageIdx];

    //AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawBmpFont_PutChar: \'%c\' H %u Line %u First \'%c\' Last \'%c\'", CharU32, (UINT32)FontPage->Height, (UINT32)FontPage->LineBytes, (UINT32)FontPage->FirstChar, (UINT32)FontPage->LastChar);
    /*
     * Ch should be in FontPage after the check in AmbaDrawBmpFont_FindCharPage().
     * So no need to check CharU32. CodeOffset should be > 0.
     */
    CodeOffset = (INT32)CharU32 - (INT32)FontPage->FirstChar;
    BitOffset = (INT32)(FontPageInfo->Offset[CodeOffset]);      // bit offset (char width in pixel)
    CharWidth = (UINT32)(FontPageInfo->Offset[CodeOffset + 1]) - (UINT32)BitOffset;
    ByteOffset = (INT32)(FontPageInfo->Offset[CodeOffset]) / 4; // byte offset, bmp font take 2 bits per pixel
    BaseAddr = &FontPageInfo->Data[ByteOffset];

    for (INT32 ScanY = Y; ScanY < (Y + (INT32)FontPage->Height); ScanY++) {
        UINT8 Data;
        INT32 Shift = 6; /* Reset to 6 */
        UINT32 TShift = (UINT32)BitOffset & 0x03U;
        const UINT8 *GetAddr = BaseAddr;

        Shift -= ((INT32)TShift * 2);
        Data = *GetAddr;
        GetAddr++;

        for (INT32 ScanX = X; ScanX < (X + (INT32)CharWidth); ScanX++) {
            if (Shift < 0) {
                Shift = 6;
                Data = *GetAddr;
                GetAddr++;
            }
            /*
             * There are 3 possible values in Font Bin file
             * 0: Transparent color
             * 1: Fore color
             * 2. Back color
             */
            //tmp[bout] = (data >> sh) & 0x03;
            switch (((UINT32)Data >> (UINT32)Shift) & 0x03U) {
            case 0:
                break;
            case 1:
                Render->PlotPixelAt_f(Render, ScanX, ScanY, &DrawConfig->ColorFore);
                break;
            case 2:
                Render->PlotPixelAt_f(Render, ScanX, ScanY, &DrawConfig->ColorBack);
                break;
            default:
                /* Do nothing */
                break;
            }
            Shift -= 2;
        }
        /* BaseAddr += FontPage->LineBytes; */
        BaseAddr = &BaseAddr[FontPage->LineBytes];
    }

    return CharWidth;
}

static UINT32 AmbaDrawBmpFont_DrawString(const AMBA_DRAW_RENDER_s *Render, const AMBADRAW_FONT_DRAW_STRING_CONFIG_s *DrawConfig)
{
    UINT32 Rval = DRAW_OK;

    if (Render == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Render is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (DrawConfig == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] DrawConfig is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (DrawConfig->String == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] String is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (DrawConfig->FontId >= G_FontHeader->LangNum) {
        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawBmpFont_DrawString: [ERROR] Invalid FontId %u (Should be less than %u)", DrawConfig->FontId, (UINT32)G_FontHeader->LangNum, 0U, 0U, 0U);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        Rval = AmbaDrawBmpFont_ValidCheck();
        if (Rval == DRAW_OK) {
            const char *Ch = DrawConfig->String;
            INT32 X = DrawConfig->StringX;
            while (*Ch != '\0') { /* Not end of String */
                UINT16 PageIdx = 0U;
                Rval = AmbaDrawBmpFont_FindCharPage(DrawConfig->FontId, (*Ch), &PageIdx);
                if (Rval == DRAW_OK) {
                    // put char on buffer
                    X += (INT32)AmbaDrawBmpFont_PutChar(Render, PageIdx, X, DrawConfig->StringY, (*Ch), DrawConfig);
                } else {
                    break;
                }
                /* Next char */
                Ch++;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDrawBmpFont_GetStringWidth(UINT32 FontId, const char *String, UINT32 *Width)
{
    UINT32 Rval;

    Rval = AmbaDrawBmpFont_ValidCheck();
    if (Rval != DRAW_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawBmpFont_ValidCheck failed", __func__, NULL, NULL, NULL, NULL);
    } else if (String == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] String is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (Width == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Width is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (FontId >= G_FontHeader->LangNum) {
        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawBmpFont_GetStringWidth: [ERROR] Invalid FontId %u (Should be less than %u)", FontId, (UINT32)G_FontHeader->LangNum, 0U, 0U, 0U);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        const char *Ch = String;
        UINT32 StringWidth = 0U;
        while (*Ch != '\0') { /* Not end of String */
            UINT16 PageIdx = 0U;
            Rval = AmbaDrawBmpFont_FindCharPage(FontId, (*Ch), &PageIdx);
            if (Rval == DRAW_OK) {
                const AMBA_DRAW_BMPFONT_BIN_FONT_s *FontPage = &G_PageTable[PageIdx];
                const AMBA_DRAW_BMPFONT_BIN_FONT_INFO_s *FontPageInfo = &G_FontConfig.FontPageInfo[PageIdx];
                INT32 CodeOffset = (INT32)((UINT8)*Ch) - (INT32)(FontPage->FirstChar);
                INT32 BitOffset = (INT32)(FontPageInfo->Offset[CodeOffset]); // Bit offset (char width in pixel)
                StringWidth += ((UINT32)(FontPageInfo->Offset[CodeOffset + 1]) - (UINT32)BitOffset);
            } else {
                break;
            }
            /* Next char */
            Ch++;
        }
        *Width = StringWidth;
    }

    return Rval;
}

static UINT32 AmbaDrawBmpFont_GetStringHeight(UINT32 FontId, const char *String, UINT32 *Height)
{
    UINT32 Rval;

    Rval = AmbaDrawBmpFont_ValidCheck();
    if (Rval != DRAW_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawBmpFont_ValidCheck failed", __func__, NULL, NULL, NULL, NULL);
    } else if (String == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] String is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (Height == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Height is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (FontId >= G_FontHeader->LangNum) {
        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawBmpFont_GetStringHeight: [ERROR] Invalid FontId %u (Should be less than %u)", FontId, (UINT32)G_FontHeader->LangNum, 0U, 0U, 0U);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        *Height = G_FontTable[FontId].Height;
    }

    return Rval;
}

/**
 *  @brief Get Font functions
 *
 *  To Get Font functions
 *
 *  @param [out] Func               Functions of BMP font
 *
 *  @return DRAW_OK
 */
UINT32 AmbaDrawBMPFont_GetFunc(AMBADRAW_FONT_FUNCTION_s *Func)
{
    UINT32 Rval;
    G_FontConfig.Func.Init_f = AmbaDrawBmpFont_InitFromROMFS;
    G_FontConfig.Func.Load_f = AmbaDrawBmpFont_Load;
    G_FontConfig.Func.Draw_f = AmbaDrawBmpFont_DrawString;
    G_FontConfig.Func.GetStrWidth_f = AmbaDrawBmpFont_GetStringWidth;
    G_FontConfig.Func.GetStrHeight_f = AmbaDrawBmpFont_GetStringHeight;
    Rval = DRAW_WRAP2D(AmbaWrap_memcpy(Func, &G_FontConfig.Func, sizeof(AMBADRAW_FONT_FUNCTION_s)));
    if (Rval != DRAW_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memcpy fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Check font
 *
 *  @param [in] FontCfg             Font config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawBMPFont_IsValidFont(const AMBADRAW_FONT_s *FontCfg)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (FontCfg != NULL) {
        if (FontCfg->Func.GetStrWidth_f != NULL) {
            if (FontCfg->Func.GetStrHeight_f != NULL) {
                Rval = DRAW_OK;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] GetStrHeight_f is NULL", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] GetStrWidth_f is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Font is NULL", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

