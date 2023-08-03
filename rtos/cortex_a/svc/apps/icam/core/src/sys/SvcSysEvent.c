/*
*  @file SvcSysEvent.c
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
*  @details  system event functions
*
*/

#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaDSP.h"
#include "AmbaDSP_Event.h"

#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcDSP.h"
#include "SvcSysEvent.h"

#define SVC_LOG_SEVT            "SEVT"

#define SVC_EVT_DSP_HDLR_MAX    16

typedef struct {
    UINT32                    Id;
    UINT32                    MaxHdlrs;
    AMBA_DSP_EVENT_HANDLER_f  *pEventHdlrs;
} SVC_DSP_EVENT_s;

typedef AMBA_DSP_EVENT_HANDLER_f DSP_EVENT_HND_t[SVC_EVT_DSP_HDLR_MAX];

static SVC_DSP_EVENT_s  g_DspEvents[AMBA_DSP_EVENT_NUM] GNU_SECTION_NOZEROINIT;

/**
* reset event
* @return none
*/
void SvcSysEvent_Reset(void)
{
    UINT32  i;

    for (i = 0U; i < AMBA_DSP_EVENT_NUM; i++) {
        if (AmbaDSP_EventHandlerCtrlReset((UINT16)i) != OK) {
            SvcLog_DBG(SVC_LOG_SEVT, "## fail to AmbaDSP_EventHandlerCtrlReset(%u)", i, 0U);
        }
    }
}

/**
* initialization of dsp event handler configuration
* @return none
*/
void SvcSysEvent_Init(void)
{
    static DSP_EVENT_HND_t  g_DspEventHnds[AMBA_DSP_EVENT_NUM] GNU_SECTION_NOZEROINIT;

    UINT32           i, Rval;
    SVC_DSP_EVENT_s  *pEvt;

    /* dsp event init */
    AmbaSvcWrap_MisraMemset(g_DspEvents, 0, sizeof(g_DspEvents));
    AmbaSvcWrap_MisraMemset(g_DspEventHnds, 0, sizeof(g_DspEventHnds));

    for (i = 0; i < AMBA_DSP_EVENT_NUM; i++) {
        pEvt = &g_DspEvents[i];

        pEvt->Id          = i;
        pEvt->MaxHdlrs    = SVC_EVT_DSP_HDLR_MAX;
        pEvt->pEventHdlrs = g_DspEventHnds[i];
        Rval = AmbaDSP_EventHandlerCtrlConfig((UINT16)pEvt->Id,
                                              (UINT16)pEvt->MaxHdlrs,
                                              pEvt->pEventHdlrs);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_SEVT, "## fail to config dsp event(%u), rval(%u)", i, Rval);
        }
    }
}

/**
* status dump of dsp event handler
* @return none
*/
void SvcSysEvent_DumpDSP(void)
{
    UINT32                 i;
    const SVC_DSP_EVENT_s  *pEvt;

    for (i = 0; i < AMBA_DSP_EVENT_NUM; i++) {
        pEvt = &g_DspEvents[i];

        SvcLog_DBG(SVC_LOG_SEVT, "Event ID/MaxHdlrs(%u/%u)", pEvt->Id, pEvt->MaxHdlrs);
    }
}
