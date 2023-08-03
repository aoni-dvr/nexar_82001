/**
 * @file AmbaSafety_ENET.h
 * header for enet safety
 *
 * @ingroup enet
 *
 * Copyright (c) 2020 Ambarella International LP
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
 */

#ifndef AMBA_SAFETY_ENET_H
#define AMBA_SAFETY_ENET_H
#include "AmbaSafety.h"

extern UINT32 AmbaSafety_EnetInit(UINT32 Idx);
extern UINT32 AmbaSafety_EnetGetWId(UINT32 Idx, const char *name);
extern UINT32 AmbaSafety_EnetEnq(UINT32 Idx, UINT32 wid);
extern UINT32 AmbaSafety_EnetDeq(UINT32 Idx, UINT32 wid);
extern UINT32 AmbaSafety_EnetSetSafeState(UINT32 Idx, UINT32 State);
extern UINT32 AmbaSafety_EnetGetSafeState(UINT32 Idx, UINT32 *pState);
extern UINT32 AmbaSafety_EnetIrqHdlr(UINT32 Idx);
extern void AmbaSafety_EnetShadowLock(void);
extern void AmbaSafety_EnetShadowUnLock(void);
extern void AmbaSafety_EnetShadowWrite(volatile UINT32 *pAddr, UINT32 Value);
extern void *EnetSafetyTaskEntry(void *arg);

#endif /* AMBA_SAFETY_ENET_H */
