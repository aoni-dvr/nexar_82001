/**
*  @file ArmHeaderParser.h
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

#include "SvcCvAlgo.h"
#include "CvCommFlexi.h"


#ifndef ARM_HEADER_PARSER_H
#define ARM_HEADER_PARSER_H

/* byte size of fields in header */
#define SIZE_OF_HEADER                     (64U)
#define SIZE_OF_MAGIC_NUMBER               (4U)
#define SIZE_OF_NUMBER_OF_PACKED           (4U)
#define SIZE_OF_BUF_OFFSET                 (52U)
#define SIZE_OF_VERSION                    (4U)
#define SIZE_OF_HEADER_PADDING             (0U)
#define SIZE_OF_OUTPUT_PADDING             (4U)
#define SIZE_OF_OUTPUT_DESCRIPTOR_PADDING  (24U)
#define SIZE_OF_DATA_FORMAT                (16U)
#define SIZE_OF_DIMENSION                  (16U)
#define SIZE_OF_NUMBER_OF_BYTE             (4U)

/* magic number */
#define MAGIC_NUMBER                       (0xACEFACED)

/* number of dimensions */
#define NUM_OF_DIMENSION                   (4U)

/* number of data format */
#define NUM_OF_DATA_FORMAT                 (4U)

/* current module version */
#define MODULE_VERSION                     (0x00010D24)

UINT32 ArmHeaderParser_GetDim(AMBA_CV_FLEXIDAG_IO_s* pFDOut, UINT8 FDIdx, UINT8 OutputIdx, UINT32* pDim);
UINT32 ArmHeaderParser_GetDataFormat(AMBA_CV_FLEXIDAG_IO_s* pFDOut, UINT8 FDIdx, UINT8 OutputIdx, UINT32* pDataFormat);
UINT32 ArmHeaderParser_GetByteNum(AMBA_CV_FLEXIDAG_IO_s* pFDOut, UINT8 FDIdx, UINT8 OutputIdx, UINT32* pByteNum);
UINT32 ArmHeaderParser_GetPackedOutputNum(AMBA_CV_FLEXIDAG_IO_s* pFDOut, UINT8 FDIdx, UINT32* pPackedOutputNum);
UINT32 ArmHeaderParser_GetOutputPayload(AMBA_CV_FLEXIDAG_IO_s* pFDOut, UINT8 FDIdx, UINT8 OutputIdx, char* pOutputPayload);

#endif  /* ARM_HEADER_PARSER_H */
