/**
 *  @file RefCmptPlayerImpl.h
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
 *  @details Header file of reference compatible player
 *
 */

#ifndef REF_CMPT_PLAYER_H
#define REF_CMPT_PLAYER_H

#include <AmbaTypes.h>

#include "RefPlayer.h"

extern UINT32 RefCmptPlayer_Init(UINT32 InitMode, UINT32 BitsBufAddr, UINT32 BitsBufSize);
extern UINT32 RefCmptPlayer_SetPipeNum(UINT8 PipeNum);
extern UINT32 RefCmptPlayer_Create(UINT8 FormatCount, const REF_PLAYER_DEMUXER_FILE_INFO_s FileInfo[AMBA_DEMUXER_MAX_FORMAT_PER_PIPE], UINT8 *PipeId, void **pHdlr);
extern UINT32 RefCmptPlayer_Delete(UINT8 PipeId);
extern UINT32 RefCmptPlayer_Start(UINT8 PipeId, UINT32 StartTime, UINT8 Direction, UINT32 Speed);
extern UINT32 RefCmptPlayer_Stop(UINT8 PipeId);
extern UINT32 RefCmptPlayer_Pause(UINT8 PipeId);
extern UINT32 RefCmptPlayer_Resume(UINT8 PipeId);
extern UINT32 RefCmptPlayer_Step(UINT8 PipeId);
extern UINT32 RefCmptPlayer_IsVideoRunning(void);
extern UINT32 RefCmptPlayer_SetGopConfig(UINT32 IdrInterval, UINT32 TimeScale, UINT32 TimePerFrame, UINT32 N, UINT32 M);
extern UINT32 RefCmptPlayer_DecDebugFlag(UINT8 Flag);

#endif
