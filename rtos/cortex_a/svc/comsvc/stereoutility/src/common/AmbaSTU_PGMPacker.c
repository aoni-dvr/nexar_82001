/**
 *  @file AmbaSTU_PGMPacker.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details stereo utility
 *
 */
#include "AmbaSTU_StringConverter.h"
#include "AmbaSTU_PGMPacker.h"
#include "AmbaWrap.h"
#include "AmbaSTU_ErrNo.h"
#include "AmbaCalib_Def.h"
#include "AmbaPrint.h"
#define PGM_LINE_FEED (0xA) // '\n'
#define PGM_SPACE (0x20) // ' '
#define PGM_DOT (0x2E) // '.'
static void STU_PGMPackString(UINT8 *pPGMDataBuf, UINT32 Sof, const UINT8 *pStr, UINT32 StrLen, UINT32 *pEof)
{
    UINT32 Rval;
    Rval = AmbaWrap_memcpy(&pPGMDataBuf[Sof], pStr, StrLen);
    if (Rval != STU_OK) {
        AmbaPrint_PrintStr5("%s() call AmbaWrap_memcpy fail\n", __func__, NULL, NULL, NULL, NULL);
    }
    *pEof = Sof + StrLen;
}

void STU_PGMGen16BitHeader(UINT8 *pPGMDataBuf, UINT32 Sof, UINT32 *pEof)
{
    const UINT8 Bit16Header[PGM_BIT16_HEADER_SIZE] = PGM_BIT16_HEADER;
    STU_PGMPackString(pPGMDataBuf, Sof, Bit16Header, sizeof(Bit16Header) - 1U, pEof);
}

UINT32 STU_PGMGenBaselineHeader(UINT8 *pPGMDataBuf, UINT32 Sof, DOUBLE Baseline, UINT32 *pEof)
{
    UINT32 Rval = STU_OK;
    const UINT8 BaselineHeader[PGM_BASELIBE_HEADER_SIZE] = PGM_BASELINE_HEADER;
    UINT8 Num2StrBuf[20];
    UINT32 Len;
    UINT32 Eof;
    STU_PGMPackString(pPGMDataBuf, Sof, BaselineHeader, sizeof(BaselineHeader) - 1U, &Eof);
    if (0U != AmbaWrap_memset(Num2StrBuf, 0x0, sizeof(Num2StrBuf))) {
        Rval = STU_ERROR_GENERAL;
    }
    if (0U !=  STU_DBtoStr(Num2StrBuf, sizeof(Num2StrBuf), Baseline, &Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    if (0U != AmbaWrap_memcpy(&pPGMDataBuf[Eof], Num2StrBuf, Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    Eof += Len;
    pPGMDataBuf[Eof] = PGM_LINE_FEED;
    Eof++;
    *pEof = Eof;
    return Rval;
}

UINT32 STU_PGMGenScaledFocalLength(UINT8 *pPGMDataBuf, UINT32 Sof, DOUBLE ScaledFocalLengthRight, DOUBLE ScaledFocalLengthLeft, UINT32 *pEof)
{
    UINT32 Rval = STU_OK;
    const UINT8 ScaledFocalLength[PGM_SCALED_FOCAL_LENGTH_SIZE] = PGM_SCALED_FOCAL_LENGTH;
    UINT8 Num2StrBuf[10];
    UINT32 Len;
    UINT32 Eof;
    UINT32 SFLRightInt, SFLLeftInt;
    SFLRightInt = (UINT32)ScaledFocalLengthRight;
    SFLLeftInt = (UINT32)ScaledFocalLengthLeft;
    STU_PGMPackString(pPGMDataBuf, Sof, ScaledFocalLength, sizeof(ScaledFocalLength) - 1U, &Eof);
    if (0U != STU_U32toStr(Num2StrBuf, sizeof(Num2StrBuf), SFLRightInt, &Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    if (0U != AmbaWrap_memcpy(&pPGMDataBuf[Eof], Num2StrBuf, Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    Eof += Len;
    pPGMDataBuf[Eof] = PGM_SPACE;
    Eof++;

    if (0U != STU_U32toStr(Num2StrBuf, sizeof(Num2StrBuf), SFLLeftInt, &Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    if (0U != AmbaWrap_memcpy(&pPGMDataBuf[Eof], Num2StrBuf, Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    Eof += Len;

    pPGMDataBuf[Eof] = PGM_LINE_FEED;
    Eof++;
    *pEof = Eof;
    return Rval;
}

UINT32 STU_PGMGenResolutionHeader(UINT8 *pPGMDataBuf, UINT32 Sof, UINT32 Width, UINT32 Height, UINT32 Depth, UINT32 *pEof)
{

    UINT32 Rval = STU_OK;
    UINT32 Eof = Sof;
    UINT8 Num2StrBuf[10];
    UINT32 Len;
    UINT32 MaxVal;
    if (0U != STU_U32toStr(Num2StrBuf, sizeof(Num2StrBuf), Width, &Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    if (0U != AmbaWrap_memcpy(&pPGMDataBuf[Eof], Num2StrBuf, Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    Eof += Len;
    pPGMDataBuf[Eof] = PGM_SPACE;
    Eof++;
    if (0U != STU_U32toStr(Num2StrBuf, sizeof(Num2StrBuf), Height, &Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    if (0U != AmbaWrap_memcpy(&pPGMDataBuf[Eof], Num2StrBuf, Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    Eof += Len;
    pPGMDataBuf[Eof] = PGM_SPACE;
    Eof++;
    MaxVal = ((UINT32)1U << Depth) - (UINT32)1U;
    if (0U != STU_U32toStr(Num2StrBuf, sizeof(Num2StrBuf), MaxVal, &Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    if (0U != AmbaWrap_memcpy(&pPGMDataBuf[Eof], Num2StrBuf, Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    Eof += Len;
    pPGMDataBuf[Eof] = PGM_LINE_FEED;
    Eof++;
    *pEof = Eof;
    return Rval;
}

UINT32 STU_PGMGenDsiFmtHeader(UINT8 *pPGMDataBuf, UINT32 Sof, UINT32 IntegerBitNum, UINT32 DecimalBitNum, UINT32 *pEof)
{
    UINT32 Rval = STU_OK;
    static const UINT8 DsiFmtHeader[PGM_DSI_FMT_HEADER_SIZE] = PGM_DSI_FMT_HEADER;
    UINT8 Num2StrBuf[10];
    UINT32 Len;
    UINT32 Eof;
    STU_PGMPackString(pPGMDataBuf, Sof, DsiFmtHeader, sizeof(DsiFmtHeader) - 1U, &Eof);
    if (0U != STU_U32toStr(Num2StrBuf, sizeof(Num2StrBuf), IntegerBitNum, &Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    if (0U != AmbaWrap_memcpy(&pPGMDataBuf[Eof], Num2StrBuf, Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    Eof += Len;
    pPGMDataBuf[Eof] = PGM_DOT;
    Eof++;
    if (0U != STU_U32toStr(Num2StrBuf, sizeof(Num2StrBuf), DecimalBitNum, &Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    if (0U != AmbaWrap_memcpy(&pPGMDataBuf[Eof], Num2StrBuf, Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    Eof += Len;
    pPGMDataBuf[Eof] = PGM_LINE_FEED;
    Eof++;
    *pEof = Eof;
    return Rval;
}

UINT32 STU_PGMGenOpticalCenter(UINT8 *pPGMDataBuf, UINT32 Sof, const AMBA_CAL_POINT_DB_2D_s *pOpticalCenter, UINT32 *pEof)
{
    UINT32 Rval = STU_OK;
    static const UINT8 OpticalCenterHeader[PGM_OPTICAL_CENTER_HEADER_SIZE] = PGM_OPTICAL_CENTER_HEADER;
    UINT8 Num2StrBuf[20];
    UINT32 Len;
    UINT32 Eof;
    STU_PGMPackString(pPGMDataBuf, Sof, OpticalCenterHeader, sizeof(OpticalCenterHeader) - 1U, &Eof);
    if ( 0U != STU_DBtoStr(Num2StrBuf, sizeof(Num2StrBuf), pOpticalCenter->X, &Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    if ( 0U != AmbaWrap_memcpy(&pPGMDataBuf[Eof], Num2StrBuf, Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    Eof += Len;
    pPGMDataBuf[Eof] = PGM_SPACE;
    Eof++;
    if ( 0U != STU_DBtoStr(Num2StrBuf, sizeof(Num2StrBuf), pOpticalCenter->Y, &Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    if ( 0U != AmbaWrap_memcpy(&pPGMDataBuf[Eof], Num2StrBuf, Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    Eof += Len;
    pPGMDataBuf[Eof] = PGM_LINE_FEED;
    Eof++;
    *pEof = Eof;
    return Rval;
}

UINT32 STU_PGMGenProjectionModel(UINT8 *pPGMDataBuf, UINT32 Sof, AMBA_CAL_EM_PROJECTION_MODEL_e ProjectionModel, UINT32 *pEof)
{
    UINT32 Rval = STU_OK;
    static const UINT8 ProjectionModelHeader[PGM_PROJECTION_MODEL_HEADER_SIZE] = PGM_PROJECTION_MODEL_HEADER;
    static const UINT8 Pinhole[] = "PINHOLE";
    static const UINT8 Spherical[] = "SPHERICAL";
    UINT32 Eof;
    UINT32 SofMisra = Sof;
    STU_PGMPackString(pPGMDataBuf, SofMisra, ProjectionModelHeader, sizeof(ProjectionModelHeader) - 1U, &Eof);
    SofMisra = Eof;
    if (ProjectionModel == AMBA_CAL_EM_MODEL_PINHOLE) {
        STU_PGMPackString(pPGMDataBuf, SofMisra, Pinhole, sizeof(Pinhole) - 1U, &Eof);
    } else if (ProjectionModel == AMBA_CAL_EM_MODEL_SPHERICAL) {
        STU_PGMPackString(pPGMDataBuf, SofMisra, Spherical, sizeof(Spherical) - 1U, &Eof);
    } else {
        Rval = STU_ERROR_GENERAL;
    }
    // SofMisra = Eof;
    pPGMDataBuf[Eof] = PGM_LINE_FEED;
    Eof++;
    *pEof = Eof;
    return Rval;
}

UINT32 STU_PGMGenScale(UINT8 *pPGMDataBuf, UINT32 Sof, UINT32 Scale, UINT32 *pEof)
{
    UINT32 Rval = STU_OK;
    static const UINT8 ScaleHeader[PGM_SCALE_HEADER_SIZE] = PGM_SCALE_HEADER;
    UINT32 Eof;
    UINT8 Num2StrBuf[20];
    UINT32 Len;
    STU_PGMPackString(pPGMDataBuf, Sof, ScaleHeader, sizeof(ScaleHeader) - 1U, &Eof);
    if (0U != STU_U32toStr(Num2StrBuf, sizeof(Num2StrBuf), Scale, &Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    if (0U != AmbaWrap_memcpy(&pPGMDataBuf[Eof], Num2StrBuf, Len)) {
        Rval = STU_ERROR_GENERAL;
    }
    Eof += Len;
    pPGMDataBuf[Eof] = PGM_LINE_FEED;
    Eof++;
    *pEof = Eof;
    return Rval;
}

static inline UINT16 STU_PGMReverseBytes(UINT16 Value)
{
    return (((Value & 0x00FFU) << 8U) | ((Value & 0xFF00U) >> 8U));
}

UINT32 STU_PGMReverseDsi(UINT8 *pPGMDataBuf, UINT32 Sof, const UINT16 *pDsiTbl, UINT32 DsiTblSize, UINT32 *pEof)
{
    UINT32 Rval = STU_OK;
    UINT32 i;
    UINT16 ReversedData;
    UINT32 Eof = Sof;
    if ((pPGMDataBuf == NULL) || (pDsiTbl == NULL) || (pEof == NULL)) {
        Rval = STU_ERROR_GENERAL;
    } else {
        for (i = 0U; i < (DsiTblSize / 2U); i++) {
            ReversedData = STU_PGMReverseBytes(pDsiTbl[i]);
            if (0U != AmbaWrap_memcpy(&pPGMDataBuf[Eof], &ReversedData, sizeof(UINT16))) {
                Rval = STU_ERROR_GENERAL;
            }
            Eof += sizeof(UINT16);
        }
        *pEof = Eof;
    }
    return Rval;
}

