/**
*  @file FileSinkHeader.c
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
*   @details The FileSink example with CvComm
*
*/

#include "FileSinkHeader.h"

#define ARM_LOG_FILE_SINK_HEADER     "FileSinkHeader"

#define SINK_HEADER_FILENAME         "OUTPUT_RAW"

typedef struct {
    UINT32 DataFormat[4];
    UINT32 Dimension[4];
    UINT32 NumOfByte;
} OUTPUT_DESCRIPTOR_s;

typedef struct {
    UINT32 FDIdx;
    UINT32 OutputIdx;
} FLEXIDAG_IDX_s;

typedef struct {
    UINT32 num_packed;
    UINT32 buffer_offset[13U];
    OUTPUT_DESCRIPTOR_s OutputDescriptor[13U];
    FLEXIDAG_IDX_s OutputIndex[13U];
} HEADER_INFO_s;


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FileSinkHeaderCallback
 *
 *  @Description:: Callback for saving data to file
 *
 *  @Input      ::
 *    Slot:        Flexidag id
 *    pEventData:  Pointer to data (memio_sink_send_out_t)
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK or NG
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FileSinkHeaderCallback(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Rval = ARM_OK;
    char FileName[MAX_SINK_FN_LEN];  // format = OUTPUT_RAW_XX_YY.out
    char FileIdxStr[4U];
    char IoNodeIdxStr[3U];
    UINT32 WriteSize = 0;
    char* pRawStartPos = NULL;
    HEADER_INFO_s* pHeaderInfo = NULL;

    if (Event == CALLBACK_EVENT_OUTPUT) {
        /* 1. Sanity check for parameters */
        if (pEventData == NULL) {
            ArmLog_ERR(ARM_LOG_FILE_SINK_HEADER, "## FileSinkCallback fail (pEventData is null)", 0U, 0U);
            Rval = ARM_NG;
        } else {
            if (pEventData->pOutput == NULL) {
                ArmLog_ERR(ARM_LOG_FILE_SINK_HEADER, "## FileSink_Register fail (pOutput is null)", 0U, 0U);
                Rval = ARM_NG;
            } else {
                Rval = ARM_OK;
            }
        }

        /* 2. Save file for CALLBACK_EVENT_OUTPUT */
        if ((Rval == ARM_OK) && (pEventData != NULL)) {
            pHeaderInfo = (HEADER_INFO_s *) pEventData->pExtOutput;

            for (UINT32 FDIdx = 0U; FDIdx < pEventData->pOutput->num_of_buf; FDIdx++) {
                FileIdxStr[0U] = '0' + ((FDIdx / 10U) % 10U);
                FileIdxStr[1U] = '0' + (FDIdx % 10U);
                FileIdxStr[2U] = '\0';
                ArmStdC_strcpy(FileName, MAX_SINK_FN_LEN, "");
                ArmStdC_strcat(FileName, MAX_SINK_FN_LEN, SINK_HEADER_FILENAME);
                ArmStdC_strcat(FileName, MAX_SINK_FN_LEN, "_");
                for (UINT32 OutIdx = 0U; OutIdx < pHeaderInfo[FDIdx].num_packed; OutIdx++) {
                    IoNodeIdxStr[0U] = '0' + ((OutIdx / 10U) % 10U);
                    IoNodeIdxStr[1U] = '0' + (OutIdx % 10U);
                    IoNodeIdxStr[2U] = '\0';
                    ArmStdC_strcat(FileName, MAX_SINK_FN_LEN, FileIdxStr);
                    ArmStdC_strcat(FileName, MAX_SINK_FN_LEN, "_");
                    ArmStdC_strcat(FileName, MAX_SINK_FN_LEN, IoNodeIdxStr);
                    ArmStdC_strcat(FileName, MAX_SINK_FN_LEN, ".out");

                    pRawStartPos = pEventData->pOutput->buf[FDIdx].pBuffer + pHeaderInfo[FDIdx].buffer_offset[OutIdx];

                    ArmLog_STR(ARM_LOG_FILE_SINK_HEADER, "save filename: %s", FileName, NULL);
                    Rval = ArmFIO_Save(pRawStartPos, pHeaderInfo[FDIdx].OutputDescriptor[OutIdx].NumOfByte, FileName, &WriteSize);
                    if (Rval == ARM_OK) {
                        if (WriteSize != pHeaderInfo[FDIdx].OutputDescriptor[OutIdx].NumOfByte) {
                            ArmLog_ERR(ARM_LOG_FILE_SINK_HEADER, "## FileSinkCallback fail (incorrect size (write=%u, out_data=%u))",
                                       WriteSize, pHeaderInfo[FDIdx].OutputDescriptor[OutIdx].NumOfByte);
                            Rval = ARM_NG;
                        }
                    }
                }
            }
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FileSinkHeader_Register
 *
 *  @Description:: Register FileSink callback to CvComm
 *
 *  @Input      ::
 *    SinkCfg:     The FileSink config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 FileSinkHeader_Register(const FILE_SINK_HEADER_CFG_s *pSinkCfg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if (pSinkCfg == NULL) {
        ArmLog_ERR(ARM_LOG_FILE_SINK_HEADER, "## FileSink_Register fail (pSinkCfg is null)", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Register callback */
    if ((Rval == ARM_OK) && (pSinkCfg != NULL)) {

        Rval = CtCvAlgoWrapper_RegCb(pSinkCfg->Slot, 0, FileSinkHeaderCallback);
        if (Rval != ARM_OK) {
            ArmLog_ERR(ARM_LOG_FILE_SINK_HEADER, "## CtCvAlgoWrapper_RegCb fail", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    return Rval;
}

