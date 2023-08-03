/**
*  @file SvcUcBIST.h
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
*  @details svc ucode BIST functions
*
*/

#ifndef SVC_UCBIST_H
#define SVC_UCBIST_H

#define UC_BIST_STAGE_C2Y   (0U)
#define UC_BIST_STAGE_Y2Y   (1U)
#define UC_BIST_STAGE_ENC   (2U)
#define UC_BIST_STAGE_DEC   (3U)

void   SvcUcBIST_EvalMemSize(UINT32 *pNeedSize);

UINT32 SvcUcBIST_Init(ULONG BufAddr, UINT32 BufSize);
UINT32 SvcUcBIST_LoadData(void);

UINT32 SvcUcBIST_Config(UINT32 IntervalMs, ULONG EncBufAddr, UINT32 EncBufSize);
UINT32 SvcUcBIST_Ctrl(UINT32 StageId);
void   SvcUcBIST_CtrlReset(void);

void   SvcUcBIST_Dump(AMBA_SHELL_PRINT_f PrintFunc);

#endif  /* SVC_UCBIST_H */
