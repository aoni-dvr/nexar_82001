/**
 *  @file SvcCvFlowUtil.h
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
 *  @details Header of SvcCvFlow utility
 *
 */

#ifndef SVC_CV_FLOW_UTIL_H
#define SVC_CV_FLOW_UTIL_H

typedef struct {
    ULONG       MemBase;            /* Memory for FlexiDAG binary buffer and working buffer */
    UINT32      MemSize;
} SVC_CV_FLOW_UTIL_CFG_s;

typedef struct {
    UINT8       *pAddr;
    UINT32      Size;               /* Buffer size */
    UINT32      DataSize;           /* Valid data size */
} SVC_CV_FLOW_BUF_INFO_s;

UINT32 SvcCvFlow_UtilInit(SVC_CV_FLOW_UTIL_CFG_s *pCfg);
UINT32 SvcCvFlow_UtilDeInit(void);
UINT32 SvcCvFlow_AttachFlexiBin(const char *pFileName, UINT32 StorageType, SVC_CV_FLOW_BUF_INFO_s *pBufInfo);
UINT32 SvcCvFlow_AddVisOrcBin(const char *pFileName);
UINT32 SvcCvFlow_AddCVTable(const char *pFileName);

UINT32 SvcCvFlow_GetFileSize(const char *pFileName, UINT32 StorageType, UINT32 *pSize);
UINT32 SvcCvFlow_LoadFile(const char *pFileName, UINT32 StorageType, SVC_CV_FLOW_BUF_INFO_s *pBufInfo);

UINT32 SvcCvFlow_LoadBinFile(const char *pFileName, UINT32 StorageType, SVC_CV_FLOW_BUF_INFO_s *pBufInfo);

UINT32 SvcCvFlow_AllocWorkBuf(UINT32 ReqSize, SVC_CV_FLOW_BUF_INFO_s *pBufInfo);
void   SvcCvFlow_DumpBufUsage(void);

UINT32 SvcCvFlow_GetChipID(void);
void   SvcCvFlow_PrintULong(const char *pFormat, ULONG Arg1, ULONG Arg2, ULONG Arg3, ULONG Arg4, ULONG Arg5);

#endif /* SVC_CV_FLOW_UTIL_H */