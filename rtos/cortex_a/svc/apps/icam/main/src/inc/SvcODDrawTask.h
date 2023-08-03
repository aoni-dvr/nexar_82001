/**
 *  @file SvcODDrawTask.h
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
 *  @details svc BSD task header
 *
 */

#ifndef SVC_OD_DRAW_TASK_H
#define SVC_OD_DRAW_TASK_H

#define SVC_FC_DRAW             (0U)
#define SVC_BSD_DRAW            (1U)
#define SVC_EMA_DRAW            (2U)
#define SVC_RMG_DRAW            (3U)
#define SVC_FC_3D_DRAW          (4U)
#define SVC_FC_PCPT_DRAW        (5U)

UINT32 SvcODDrawTask_Init(void);
UINT32 SvcODDrawTask_Config(UINT32 ColorMap);
UINT32 SvcODDrawTask_Start(void);
UINT32 SvcODDrawTask_Stop(void);
UINT32 SvcODDrawTask_GetStatus(UINT32 *pEnable);
void   SvcODDrawTask_CheckStart(UINT32 *pStarted);

UINT32 SvcODDrawTask_Ctrl(const char *pCmd, UINT32 Param);
UINT32 SvcODDrawTask_Enable(UINT32 Enable);
UINT32 SvcODDrawTask_Msg(UINT32 MsgCode, void *pInfo);
void   SvcODDrawTask_ResetFoV(void);

#endif /* SVC_ODDRAW_TASK_H */