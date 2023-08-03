/**
 *  @file AmbaDSP_Event.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Header of DSP Driver Event API
 *
 */

#ifndef AMBA_DSP_EVENT_H
#define AMBA_DSP_EVENT_H

#if defined(CONFIG_THREADX)
#include "AmbaKAL.h"
#endif
#include "AmbaTypes.h"
#include "AmbaDSP_Event_Def.h"

typedef UINT32 (*AMBA_DSP_EVENT_HANDLER_f)(const void *pEventData);

UINT32 AmbaDSP_EventHandlerCtrlConfig(UINT16 EventID, UINT16 MaxNumHandlers, AMBA_DSP_EVENT_HANDLER_f *pEventHandlers);

UINT32 AmbaDSP_EventHandlerCtrlReset(UINT16 EventID);

UINT32 AmbaDSP_EventHandlerRegister(UINT16 EventID, AMBA_DSP_EVENT_HANDLER_f EventHandler);

UINT32 AmbaDSP_EventHandlerUnRegister(UINT16 EventID, AMBA_DSP_EVENT_HANDLER_f EventHandler);

#endif  /* AMBA_DSP_EVENT_H */
