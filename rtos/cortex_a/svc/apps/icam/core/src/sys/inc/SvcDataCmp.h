/**
*  @file SvcDataCmp.h
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
*  @details svc data comparison
*
*/

#ifndef SVC_DATA_CMP_H
#define SVC_DATA_CMP_H

#define SVC_DATA_CMP_MAX_DATA_NUM  (CONFIG_ICAM_DATACMP_MAX_DATA_NUM)

typedef struct {
    UINT8     CmpNum;
    UINT8     DataNum;
    UINT32    TaskPriority;
    UINT32    TaskCpuBits;
} SVC_DATA_CMP_TASK_CREATE_s;

typedef struct {
    UINT32    DataSize;
    void      *DataAddr;
    ULONG     BufBase;
    UINT32    BufSize;
} SVC_DATA_CMP_DATA_s;

typedef struct {
    UINT8                  CmpId;   /* 0 ~ (CmpNum-1) */
    UINT8                  NumData;
    SVC_DATA_CMP_DATA_s    Data[SVC_DATA_CMP_MAX_DATA_NUM];
} SVC_DATA_CMP_SEND_s;

typedef struct {
    UINT64    ErrCount;
    UINT64    CmpCount;
} SVC_DATA_CMP_REPORT_s;

typedef struct {
#define DTCMP_HDLR_SIZE    (0x8000UL)
    UINT8    PrivData[DTCMP_HDLR_SIZE];
} SVC_DATA_CMP_HDLR_s;

UINT32 SvcDataCmp_Create(SVC_DATA_CMP_HDLR_s *pHdlr, const SVC_DATA_CMP_TASK_CREATE_s *pCreate);
UINT32 SvcDataCmp_Delete(SVC_DATA_CMP_HDLR_s *pHdlr);
UINT32 SvcDataCmp_DataSend(SVC_DATA_CMP_HDLR_s *pHdlr, const SVC_DATA_CMP_SEND_s *pSend);
UINT32 SvcDataCmp_ReportGet(SVC_DATA_CMP_HDLR_s *pHdlr, SVC_DATA_CMP_REPORT_s *pReport);

#endif  /* SVC_DATA_CMP_H */
