/**
*  @file ArmHeader.c
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
#include "ArmHeader.h"

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
 *  @RoutineName:: sanity_check
 *
 *  @Description:: sanit check for ArmHeader APIs.
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

    if (pFDOut == NULL) {
        ArmLog_ERR(ARM_LOG_HEADER_PARSER, "## sanity check error: pFDOut is NULL.", 0U, 0U);
        RetVal = ARM_NG;
    }

    if (is_magic_number(pFDOut) == ARM_NG && RetVal == ARM_OK) {
        ArmLog_ERR(ARM_LOG_HEADER_PARSER, "## sanity check error: pFDout doesn't have magic number, so doesn't have header", 0U, 0U);
        RetVal = ARM_NG;
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
 *  @RoutineName:: ArmHeader_GetFdgHeader
 *
 *  @Description:: Get output flexidag header struct
 *
 *  @Input       ::
 *    pFDOut: Pointer to Flexidag output struct
 *    FDIdx: Index of Flexidag output
 *
 *  @Output     ::
 *    pFdgHeader: Pointer to Flexidag output header
 *
 *  @Return     ::
 *    UINT32: ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmHeader_GetFdgHeader(AMBA_CV_FLEXIDAG_IO_s* pFDOut, UINT32 FDIdx, cvflow_port_header_t* pFdgHeader)
{
    UINT32 RetVal = ARM_OK;

    /* sanity check */
    if (sanity_check(pFDOut, FDIdx, 0U) == ARM_NG) {
        RetVal = ARM_NG;
    }

    if (RetVal == ARM_OK) {
       //*pFdgHeader = (cvflow_port_header_t)pFDOut->buf[FDIdx].pBuffer[0U];
       ArmStdC_memcpy(pFdgHeader, pFDOut->buf[FDIdx].pBuffer, sizeof(cvflow_port_header_t));
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmHeader_GetFdgDesc
 *
 *  @Description:: Get output flexidag descriptor struct
 *
 *  @Input       ::
 *    pFDOut: Pointer to Flexidag output struct
 *    FDIdx: Index of Flexidag output
 *    OutputIdx: Index of output packed in the Flexidag output
 *
 *  @Output     ::
 *    pFdgDesc: Pointer to Flexidag output desctiptor
 *
 *  @Return     ::
 *    UINT32: ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmHeader_GetFdgDesc(AMBA_CV_FLEXIDAG_IO_s* pFDOut, UINT32 FDIdx, UINT32 OutputIdx, cvflow_buffer_desc_t* pFdgDesc)
{
    UINT32 RetVal = ARM_OK;
    UINT32 Offset;
    cvflow_port_header_t Header = {0U};

    /* sanity check */
    if (sanity_check(pFDOut, FDIdx, OutputIdx) == ARM_NG) {
        RetVal = ARM_NG;
    }

    if (RetVal == ARM_OK) {
        //Header = (cvflow_port_header_t)pFDOut->buf[FDIdx].pBuffer[0U];
        ArmStdC_memcpy(&Header, pFDOut->buf[FDIdx].pBuffer, sizeof(cvflow_port_header_t));
        Offset = Header.buffer_offset[OutputIdx];
        //*pFdgDesc = (cvflow_buffer_desc_t)pFDOut->buf[FDIdx].pBuffer[Offset - sizeof(cvflow_buffer_desc_t)];
        ArmStdC_memcpy(pFdgDesc, (pFDOut->buf[FDIdx].pBuffer + Offset - sizeof(cvflow_buffer_desc_t)), sizeof(cvflow_buffer_desc_t));
    }

    return RetVal;
}
