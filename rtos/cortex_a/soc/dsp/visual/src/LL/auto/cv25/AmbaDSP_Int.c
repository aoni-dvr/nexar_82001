/**
*  @file AmbaDSP_Int.c
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
 *
*  @details SSP Interrupt Module
*
*/

#include "AmbaDSP.h"
#include "AmbaDSP_Int.h"
#include "AmbaDSP_Log.h"
#include "dsp_osal.h"

/* EventFlag */
static osal_even_t VinIsrFlag;
static osal_even_t VoutIsrFlag;

// disable NEON registers usage in ISR
#pragma GCC push_options
#pragma GCC target("general-regs-only")

#if defined(CONFIG_THREADX)
static void ISRVin0SOF(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin0SOF(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin0SOF(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin0SOF(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN0_SOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin0SOF dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin0Lp(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin0Lp(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin0Lp(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin0Lp(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN0_EOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin0Lp dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin1SOF(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin1SOF(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin1SOF(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin1SOF(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN1_SOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin1SOF dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin1Lp(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin1Lp(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin1Lp(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin1Lp(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN1_EOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin1Lp dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin2SOF(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin2SOF(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin2SOF(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin2SOF(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN2_SOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin2SOF dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin2Lp(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin2Lp(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin2Lp(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin2Lp(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN2_EOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin2Lp dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVout0(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVout0(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVout0(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVout0(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VoutIsrFlag, AMBA_DSP_VOUT0_INT);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVout0 dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVout1(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVout1(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVout1(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVout1(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VoutIsrFlag, AMBA_DSP_VOUT1_INT);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVout1 dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#pragma GCC pop_options

UINT32 LL_WaitVinInterrupt(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut)
{
    UINT32 Rval;

    Rval = dsp_osal_eventflag_get(&VinIsrFlag, Flag, 0U/*AnyOrAll*/, 1U/*AutoClear*/, ActualFlag, TimeOut);

    return Rval;
}

UINT32 LL_WaitVoutInterrupt(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut)
{
    UINT32 Rval;

    Rval = dsp_osal_eventflag_get(&VoutIsrFlag, Flag, 0U/*AnyOrAll*/, 1U/*AutoClear*/, ActualFlag, TimeOut);

    return Rval;
}

static inline UINT32 InterruptInit_HookHdlr(UINT32 Id, UINT8 IrqType, osal_isr_entry_f Hldr)
{
    UINT32 Rval = DSP_ERR_NONE;
    osal_irq_t IntCfg;
    UINT32 IrqId = 0U;
    UINT32 ErrLine = 0U;

    Rval = dsp_osal_memset(&IntCfg, 0, sizeof(osal_irq_t));
    if (Rval != OK) {
        ErrLine = __LINE__;
    }
    IntCfg.trigger_type = IRQ_TRIG_RISE_EDGE;
    IntCfg.irq_type = IRQ_TYPE_IRQ;
    IntCfg.cpu_targets = INT_TARGET_CORE;
    Rval = dsp_osal_irq_id_query(IrqType, Id, &IrqId);
    if (Rval != OK) {
        ErrLine = __LINE__;
    } else {
        Rval = dsp_osal_irq_config(IrqId ,&IntCfg, Hldr, 0U);
        if (Rval != OK) {
            ErrLine = __LINE__;
        } else {
            Rval = dsp_osal_irq_enable(IrqId);
            if (Rval != OK) {
                ErrLine = __LINE__;
            }
        }
    }

    if (ErrLine != 0U) {
        AmbaLL_LogUInt5("[ERR] InterruptInit_HookHdlr Type %d Id %d return 0x%x @ %d", IrqType, Id, Rval, ErrLine, 0U);
    }

    return Rval;
}

static inline UINT32 VinInterruptInit(void)
{
    UINT32 Rval = DSP_ERR_NONE;

    /* Vin0 SOF */
    Rval = InterruptInit_HookHdlr(0U, DSP_IRQ_TYPE_VIN_SOF, ISRVin0SOF);

    /* Vin0 EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(0U, DSP_IRQ_TYPE_VIN_EOF, ISRVin0Lp);
    }

    /* Vin1 SOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(1U, DSP_IRQ_TYPE_VIN_SOF, ISRVin1SOF);
    }

    /* Vin1 EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(1U, DSP_IRQ_TYPE_VIN_EOF, ISRVin1Lp);
    }

    /* Vin2 SOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(2U, DSP_IRQ_TYPE_VIN_SOF, ISRVin2SOF);
    }

    /* Vin2 EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(2U, DSP_IRQ_TYPE_VIN_EOF, ISRVin2Lp);
    }


    return Rval;
}

UINT32 LL_InterruptInit(void)
{
    static UINT8 LL_InterruptInitFlag = 0;
    static char VinFlag[] = "DspVinFlg";
    static char VoutFlag[] = "DspVoutFlg";
    UINT32 Rval = OK;

    if (LL_InterruptInitFlag == 0U) {
        UINT32 ErrLine = 0U;

        /* ISR Flag */       
        Rval = dsp_osal_eventflag_init(&VinIsrFlag, VinFlag);
        Rval |= dsp_osal_eventflag_init(&VoutIsrFlag, VoutFlag);
        if (Rval != OK) {
            ErrLine = __LINE__;
            Rval = DSP_ERR_0003;
        } else {
            /* All Vin interrupt init*/
            Rval = VinInterruptInit();
            if (Rval != OK) {
                ErrLine = __LINE__;
            } else {
                /* Vout0 */
                Rval = InterruptInit_HookHdlr(0U, DSP_IRQ_TYPE_VOUT, ISRVout0);
                if (Rval != OK) {
                    ErrLine = __LINE__;
                } else {
                    /* Vout1 */
                    Rval = InterruptInit_HookHdlr(1U, DSP_IRQ_TYPE_VOUT, ISRVout1);
                    if (Rval != OK) {
                        ErrLine = __LINE__;
                    }
                }
            }
        }

        if (Rval == OK) {
            AmbaLL_Log((UINT32)AMBALLLOG_TYPE_INIT, "AmbaLL_IntInit", 0, 0, 0);
            LL_InterruptInitFlag = 1;
        } else {
            AmbaLL_LogUInt5("[ERR] LL_InterruptInit return 0x%x @ %d", Rval, ErrLine, 0U, 0U, 0U);
        }
    }

    return Rval;
}

