/**
*  @file ArmHeaderParser.c
*
* Copyright (c) [2020] Ambarella International LP
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
*   @details The Flexidag output header parser functions
*
*/
#include "ArmLog.h"
#include "ArmErrCode.h"
#include "ArmStdC.h"
#include "ArmHeaderParser.h"

#define ARM_LOG_HEADER_PARSER  "ArmUtil_HeaderParser"


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: cast_4char_to_u32
 *
 *  @Description:: cast 4 consecutive chars in array to an UINT32 number
 *
 *  @Input      ::
 *    x: Pointer to the first char
 *
 *  @Output    :: None
 *
 *  @Return    ::
       UINT32: u32
\*----------------------------------------------------------------------------------------------*/
static UINT32 cast_4char_to_u32(char* x)
{
    UINT32 u32;

    u32 = x[0U] | (x[1U] << 8U) | (x[2U] << 16U) | (x[3U] << 24U);

    return u32;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: is_magic_number
 *
 *  @Description:: Check if magic number exist or not
 *
 *  @Input      ::
 *    pFDOut: Pointer to Flexidag output structure
 *
 *  @Output    :: None
 *
 *  @Return    ::
       UINT32: ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*----------------------------------------------------------------------------------------------*/
static UINT32 is_magic_number(AMBA_CV_FLEXIDAG_IO_s* pFDOut)
{
    UINT32 RetVal = ARM_OK;
    UINT32 MagicNum;
    UINT32 Position;

    Position = 0U;
    MagicNum = cast_4char_to_u32(&pFDOut->buf[0U].pBuffer[Position]);

    if (MagicNum == MAGIC_NUMBER) {
        RetVal = ARM_OK;
    } else {
        RetVal = ARM_NG;
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: get_packed_number
 *
 *  @Description:: Get the number of packed output in a specific Flexidag output
 *
 *  @Input      ::
 *    pFDOut: Pointer to Flexidag output structure
 *    FDIdx: Index of Flexidag output
 *
 *  @Output    :: None
 *
 *  @Return    ::
       UINT32: PackedNum
\*----------------------------------------------------------------------------------------------*/
static UINT32 get_packed_number(AMBA_CV_FLEXIDAG_IO_s* pFDOut, UINT8 FDIdx)
{
    UINT32 PackedNum;
    UINT32 Position;

    Position = SIZE_OF_MAGIC_NUMBER;

    PackedNum = cast_4char_to_u32(&pFDOut->buf[FDIdx].pBuffer[Position]);

    return PackedNum;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: get_buf_offset
 *
 *  @Description:: Get the buffer offset of a specific packed output in a Flexidag output
 *
 *  @Input      ::
 *    pFDOut: Pointer to Flexidag output structure
 *    FDIdx: Index of Flexidag output
 *    OutputIdx: Index of output packed in the Flexidag output
 *
 *  @Output    :: None
 *
 *  @Return    ::
       UINT32: BufOffset
\*----------------------------------------------------------------------------------------------*/
static UINT32 get_buf_offset(AMBA_CV_FLEXIDAG_IO_s* pFDOut, UINT8 FDIdx, UINT8 OutputIDx)
{
    UINT32 BufOffset;
    UINT32 Position;

    Position = SIZE_OF_MAGIC_NUMBER + SIZE_OF_NUMBER_OF_PACKED + OutputIDx * 4U;

    BufOffset = cast_4char_to_u32(&pFDOut->buf[FDIdx].pBuffer[Position]);

    return BufOffset;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: get_byte_number
 *
 *  @Description:: Get the number of byte of a specific output payload
 *
 *  @Input      ::
 *    pFDOut: Pointer to Flexidag output structure
 *    FDIdx: Index of Flexidag output
 *
 *  @Output    :: None
 *
 *  @Return    ::
       UINT32: PackedNum
\*----------------------------------------------------------------------------------------------*/
static UINT32 get_byte_number(AMBA_CV_FLEXIDAG_IO_s* pFDOut, UINT8 FDIdx, UINT8 OutputIDx)
{
    UINT32 ByteNum;
    UINT32 Position;

    Position = get_buf_offset(pFDOut, FDIdx, OutputIDx) - SIZE_OF_NUMBER_OF_BYTE;

    ByteNum = cast_4char_to_u32(&pFDOut->buf[FDIdx].pBuffer[Position]);

    return ByteNum;
}


static UINT32 get_header_version(AMBA_CV_FLEXIDAG_IO_s* pFDOut)
{
    UINT32 HeaderVer;
    UINT32 Position;

    Position = SIZE_OF_MAGIC_NUMBER + SIZE_OF_NUMBER_OF_PACKED + SIZE_OF_BUF_OFFSET;

    HeaderVer = cast_4char_to_u32(&pFDOut->buf[0U].pBuffer[Position]);

    return HeaderVer;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: sanity_check
 *
 *  @Description:: sanit check for ArmHeaderParser APIs.
 *
 *  @Input      ::
 *    pFDOut: Pointer to Flexidag output structure
 *    FDIdx: Index of Flexidag output
 *    OutputIdx: Index of output packed in the Flexidag output
 *
 *  @Output    :: None
 *
 *  @Return    ::
       UINT32: ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*----------------------------------------------------------------------------------------------*/
static UINT32 sanity_check(AMBA_CV_FLEXIDAG_IO_s* pFDOut, UINT8 FDIdx, UINT8 OutputIdx)
{
    UINT32 RetVal = ARM_OK;
    UINT32 ModuleVer = MODULE_VERSION;

    if (pFDOut == NULL) {
        ArmLog_ERR(ARM_LOG_HEADER_PARSER, "## sanity check error: pFDOut is NULL.", 0U, 0U);
        RetVal = ARM_NG;
    }

    if (is_magic_number(pFDOut) == ARM_NG && RetVal == ARM_OK) {
        ArmLog_ERR(ARM_LOG_HEADER_PARSER, "## sanity check error: pFDout doesn't have magic number, so doesn't have header", 0U, 0U);
        RetVal = ARM_NG;
    }

    if (get_header_version(pFDOut) != ModuleVer && RetVal == ARM_OK) {
        ArmLog_ERR(ARM_LOG_HEADER_PARSER, "## sanity check error: Flexidag header version isn't equal to ArmHeaderParser module version", 0U, 0U);
    }

    if (FDIdx >= pFDOut->num_of_buf && RetVal == ARM_OK) {
        ArmLog_ERR(ARM_LOG_HEADER_PARSER, "## sanity check error: FDIdx is out-of-range.", 0U, 0U);
        RetVal = ARM_NG;
    }

    if (OutputIdx >= get_packed_number(pFDOut, FDIdx) && RetVal == ARM_OK) {
        ArmLog_ERR(ARM_LOG_HEADER_PARSER, "## sanity check error: OutputIdx is out-of-range.", 0U, 0U);
        RetVal = ARM_NG;
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmHeaderParser_GetDim
 *
 *  @Description:: Get output dimension
 *
 *  @Input       ::
 *    pFDOut: Pointer to Flexidag output structure
 *    FDIdx: Index of Flexidag output
 *    OutputIdx: Index of output packed in the Flexidag output
 *
 *  @Output     ::
 *    pDim: Pointer to 4 dimension values
 *
 *  @Return     ::
 *    UINT32: ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmHeaderParser_GetDim(AMBA_CV_FLEXIDAG_IO_s* pFDOut, UINT8 FDIdx, UINT8 OutputIdx, UINT32* pDim)
{
    UINT32 RetVal = ARM_OK, i;
    UINT32 Offset, Position;

    /* sanity check */
    if (sanity_check(pFDOut, FDIdx, OutputIdx) == ARM_NG) {
        RetVal = ARM_NG;
    }

    if (RetVal == ARM_OK) {
        /* get offset of this packed output */
        Offset = get_buf_offset(pFDOut, FDIdx, OutputIdx);

        /* get position of dimension in output descriptor */
        Position = Offset - SIZE_OF_NUMBER_OF_BYTE - SIZE_OF_DIMENSION;

        /* get dimension */
        for (i = 0; i < NUM_OF_DIMENSION; i++) {
            pDim[i] = cast_4char_to_u32(&pFDOut->buf[FDIdx].pBuffer[Position + i * 4U]);
        }
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmHeaderParser_GetDataFormat
 *
 *  @Description:: Get output dimension
 *
 *  @Input       ::
 *    pFDOut: Pointer to Flexidag output structure
 *    FDIdx: Index of Flexidag output
 *    OutputIdx: Index of output packed in the Flexidag output
 *
 *  @Output     ::
 *    pDataFormat: Pointer to 4 data format values
 *
 *  @Return     ::
 *    UINT32: ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmHeaderParser_GetDataFormat(AMBA_CV_FLEXIDAG_IO_s* pFDOut, UINT8 FDIdx, UINT8 OutputIdx, UINT32* pDataFormat)
{
    UINT32 RetVal = ARM_OK, i;
    UINT32 Offset, Position;

    /* sanity check */
    if (sanity_check(pFDOut, FDIdx, OutputIdx) == ARM_NG) {
        RetVal = ARM_NG;
    }

    if (RetVal == ARM_OK) {
        /* get offset of this packed output */
        Offset = get_buf_offset(pFDOut, FDIdx, OutputIdx);

        /* get position of data format in output descriptor */
        Position = Offset - SIZE_OF_NUMBER_OF_BYTE - SIZE_OF_DIMENSION - SIZE_OF_DATA_FORMAT;

        /* get data format */
        for (i = 0; i < NUM_OF_DATA_FORMAT; i++) {
            pDataFormat[i] = cast_4char_to_u32(&pFDOut->buf[FDIdx].pBuffer[Position + i * 4U]);
        }
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmHeaderParser_GetByteNum
 *
 *  @Description:: Get output byte number
 *
 *  @Input       ::
 *    pFDOut: Pointer to Flexidag output structure
 *    FDIdx: Index of Flexidag output
 *    OutputIdx: Index of output packed in the Flexidag output
 *
 *  @Output     ::
 *    pByteNum: Pointer to byte number
 *
 *  @Return     ::
 *    UINT32: ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmHeaderParser_GetByteNum(AMBA_CV_FLEXIDAG_IO_s* pFDOut, UINT8 FDIdx, UINT8 OutputIdx, UINT32* pByteNum)
{
    UINT32 RetVal = ARM_OK;

    /* sanity check */
    if (sanity_check(pFDOut, FDIdx, OutputIdx) == ARM_NG) {
        RetVal = ARM_NG;
    }

    if (RetVal == ARM_OK) {
        /* get byte number */
        *pByteNum = get_byte_number(pFDOut, FDIdx, OutputIdx);
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmHeaderParser_GetPackedOutputNum
 *
 *  @Description:: Get total packed output number in a flexidag output
 *
 *  @Input       ::
 *    pFDOut: Pointer to Flexidag output structure
 *    FDIdx: Index of Flexidag output
 *    OutputIdx: Index of output packed in the Flexidag output
 *
 *  @Output     ::
 *    pPackedOutputNum: Pointer to packed output number
 *
 *  @Return     ::
 *    UINT32: ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmHeaderParser_GetPackedOutputNum(AMBA_CV_FLEXIDAG_IO_s* pFDOut, UINT8 FDIdx, UINT32* pPackedOutputNum)
{
    UINT32 RetVal = ARM_OK;

    /* sanity check */
    if (sanity_check(pFDOut, FDIdx, 0U) == ARM_NG) {
        RetVal = ARM_NG;
    }

    if (RetVal == ARM_OK) {
        /* get byte number */
        *pPackedOutputNum = get_packed_number(pFDOut, FDIdx);
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmHeaderParser_GetPackedNum
 *
 *  @Description:: Get the packed outputs number in an Flexidag output
 *
 *  @Input       ::
 *    pFDOut: Pointer to Flexidag output structure
 *    FDIdx: Index of Flexidag output
 *    OutputIdx: Index of output packed in the Flexidag output
 *
 *  @Output     ::
 *    pOutputPayload: Pointer to output payload
 *
 *  @Return     ::
 *    UINT32: ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmHeaderParser_GetOutputPayload(AMBA_CV_FLEXIDAG_IO_s* pFDOut, UINT8 FDIdx, UINT8 OutputIdx, char* pOutputPayload)
{
    UINT32 RetVal = ARM_OK, i;
    UINT32 Offset, ByteNum;

    /* sanity check */
    if (sanity_check(pFDOut, FDIdx, OutputIdx) == ARM_NG) {
        RetVal = ARM_NG;
    }

    if (RetVal == ARM_OK) {
        /* get offset of this packed output */
        Offset = get_buf_offset(pFDOut, FDIdx, OutputIdx);

        /* get byte number of this packed output */
        ByteNum = get_byte_number(pFDOut, FDIdx, OutputIdx);

        /* get this packed output payload */
        for (i = 0; i < ByteNum; i++) {
            pOutputPayload[i] = pFDOut->buf[FDIdx].pBuffer[Offset + i];
        }
    }

    return RetVal;
}
