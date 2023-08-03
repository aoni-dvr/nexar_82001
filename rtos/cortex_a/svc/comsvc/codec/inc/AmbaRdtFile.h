/**
*  @file AmbaRdtFile.h
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
*  @details Amba record destination - File
*
*/

#ifndef AMBA_RDT_FILE_H
#define AMBA_RDT_FILE_H

#define AMBA_RDTC_FILE_STATIS_GET    (0U)
#define AMBA_RDTC_FILE_OPEN          (1U)
#define AMBA_RDTC_FILE_CLOSE         (2U)
#define AMBA_RDTC_FILE_STOP          (3U)
#define AMBA_RDTC_FILE_BUF_STATUS    (4U)

UINT32 AmbaRdtFile_Create(AMBA_REC_DST_s *pFileRdt);
UINT32 AmbaRdtFile_Destroy(const AMBA_REC_DST_s *pFileRdt);
void   AmbaRdtFile_Control(const AMBA_REC_DST_s *pFileRdt, UINT32 CtrlType, void *pParam);

void   AmbaRdtFile_EvalMemSize(const AMBA_REC_EVAL_s *pEval, UINT32 *pSize);

#endif  /* AMBA_RDT_FILE_H */
