/**
*  @file SvcSysStat.h
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
*  @details svc system status
*
*/

#ifndef SVC_SYS_STAT_H
#define SVC_SYS_STAT_H

#define SVC_SYS_STAT_CALLBACK_MAX   (16U)

typedef void (*SVC_SYS_STAT_f)(UINT32 StatIdx, void *pInfo);
typedef UINT32 SVC_SYS_STAT_ID;
typedef struct {
    void           *pStatus;
    UINT32         StatusSize;
    SVC_SYS_STAT_f Callback[SVC_SYS_STAT_CALLBACK_MAX];
    UINT8          CallbackNum;
} SVC_SYS_STAT_CALLBACK_s;

UINT32 SvcSysStat_Init(UINT32 StatNum);
UINT32 SvcSysStat_Config(ULONG CallbackMemBase, UINT32 BufSize);
UINT32 SvcSysStat_Hook(UINT32 StatIdx, void *pStatus, UINT32 StatusSize);
UINT32 SvcSysStat_Register(UINT32 StatIdx, SVC_SYS_STAT_f Callback, SVC_SYS_STAT_ID *pCtrlID);
UINT32 SvcSysStat_Unregister(UINT32 StatIdx, SVC_SYS_STAT_ID CtrlID);
UINT32 SvcSysStat_Issue(UINT32 StatIdx, void *pInfo);
UINT32 SvcSysStat_Get(UINT32 StatIdx, void *pInfo);

UINT32 SvcSysStat_QueryBufSize(UINT32 *pBufferSize);

#endif  /* SVC_SYS_STAT_H */
