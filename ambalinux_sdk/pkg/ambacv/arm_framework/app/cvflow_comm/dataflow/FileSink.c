/**
*  @file FileSink.c
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

#include "FileSink.h"

#define ARM_LOG_FILE_SINK     "FileSink"

#define SINK_FILENAME         "DUMP"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FileSinkCallback
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
static UINT32 FileSinkCallback(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    static UINT32 FileIdx = 0U;
    UINT32 Rval = ARM_OK;
    char FileName[MAX_SINK_FN_LEN];  // format = DUMP_XX_OOO.out
    char FileIdxStr[4U];
    char IoNodeIdxStr[3U];
    UINT32 WriteSize = 0;

    if (Event == CALLBACK_EVENT_OUTPUT) {

        /* 1. Sanity check for parameters */
        if (pEventData == NULL) {
            ArmLog_ERR(ARM_LOG_FILE_SINK, "## FileSinkCallback fail (pEventData is null)", 0U, 0U);
            Rval = ARM_NG;
        } else {
            if (pEventData->pOutput == NULL) {
                ArmLog_ERR(ARM_LOG_FILE_SINK, "## FileSink_Register fail (pOutput is null)", 0U, 0U);
                Rval = ARM_NG;
            }
        }

        /* 2. Save file for CALLBACK_EVENT_OUTPUT */
        if ((Rval == ARM_OK) && (pEventData != NULL)) {
            ArmLog_DBG(ARM_LOG_FILE_SINK, "save %d files to IDX %.03d", pEventData->pOutput->num_of_buf, FileIdx);
            FileIdxStr[0U] = '0' + ((FileIdx / 100U) % 10U);
            FileIdxStr[1U] = '0' + ((FileIdx / 10U) % 10U);
            FileIdxStr[2U] = '0' + (FileIdx % 10U);
            FileIdxStr[3U] = '\0';

            for (UINT32 i = 0; i < pEventData->pOutput->num_of_buf ; i++) {
                ArmStdC_strcpy(FileName, MAX_SINK_FN_LEN, "");
                ArmStdC_strcat(FileName, MAX_SINK_FN_LEN, SINK_FILENAME);
                ArmStdC_strcat(FileName, MAX_SINK_FN_LEN, "_");
                IoNodeIdxStr[0U] = '0' + ((i / 10U) % 10U);
                IoNodeIdxStr[1U] = '0' + (i % 10U);
                IoNodeIdxStr[2U] = '\0';
                ArmStdC_strcat(FileName, MAX_SINK_FN_LEN, IoNodeIdxStr);
                ArmStdC_strcat(FileName, MAX_SINK_FN_LEN, "_");
                ArmStdC_strcat(FileName, MAX_SINK_FN_LEN, FileIdxStr);
                ArmStdC_strcat(FileName, MAX_SINK_FN_LEN, ".out");

                ArmLog_STR(ARM_LOG_FILE_SINK, "save filename: %s", FileName, NULL);
                Rval = ArmFIO_Save(pEventData->pOutput->buf[i].pBuffer, pEventData->pOutput->buf[i].buffer_size, FileName, &WriteSize);
                if (Rval == ARM_OK) {
                    if (WriteSize != pEventData->pOutput->buf[i].buffer_size) {
                        ArmLog_ERR(ARM_LOG_FILE_SINK, "## FileSinkCallback fail (incorrect size (write=%u, out_data=%u))",
                                   WriteSize, pEventData->pOutput->buf[i].buffer_size);
                        Rval = ARM_NG;
                    }
                }
            }

            FileIdx ++;
            if (FileIdx > 999U) {
                FileIdx = 0U;
            }

        }

    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FileSink_Register
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
UINT32 FileSink_Register(const FILE_SINK_CFG_s *pSinkCfg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if (pSinkCfg == NULL) {
        ArmLog_ERR(ARM_LOG_FILE_SINK, "## FileSink_Register fail (pSinkCfg is null)", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Register callback */
    if ((Rval == ARM_OK) && (pSinkCfg != NULL)) {

        Rval = CtCvAlgoWrapper_RegCb(pSinkCfg->Slot, 0, FileSinkCallback);
        if (Rval != ARM_OK) {
            ArmLog_ERR(ARM_LOG_FILE_SINK, "## CtCvAlgoWrapper_RegCb fail", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    return Rval;
}

