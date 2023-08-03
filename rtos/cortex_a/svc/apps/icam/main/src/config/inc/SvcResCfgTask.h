/**
 *  @file SvcResCfgTask.h
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
 *  @details svc res config task
 *
 */

#ifndef SVC_RES_CFG_TASK_H
#define SVC_RES_CFG_TASK_H

#define SVC_NORMAL                  (0x00000000U)
#define SVC_EMR_BSD                 (0x00000001U)
#define SVC_EMR_STITCH              (0x00000002U)
#define SVC_USER_FLAG_OD_DRAW_OFF   (0x00000004U)
#define SVC_EMR_POSITION            (0x00000008U)
#define SVC_EMR_RMG                 (0x00000010U)

UINT32 SvcResCfgTask_Init(void);
UINT32 SvcResCfgTask_Config(UINT32 FormatId);
UINT32 SvcResCfgTask_Switch(UINT32 FormatId);
void   SvcResCfgTask_Dump(void);
void   SvcResCfgTask_GetCfgArr(const SVC_RES_CFG_s **pResCfgArr, UINT32 *pResCfgNum);
UINT32 SvcResCfgTask_GetPbkModeId(UINT32 *pFormatId);

#endif /* SVC_RES_CFG_TASK_H */