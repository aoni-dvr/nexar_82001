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
static void ISRVin3SOF(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin3SOF(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin3SOF(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin3SOF(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN3_SOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin3SOF dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin3Lp(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin3Lp(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin3Lp(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin3Lp(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN3_EOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin3Lp dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin4SOF(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin4SOF(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin4SOF(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin4SOF(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN4_SOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin4SOF dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin4Lp(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin4Lp(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin4Lp(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin4Lp(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN4_EOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin4Lp dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin5SOF(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin5SOF(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin5SOF(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin5SOF(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN5_SOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin5SOF dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }
#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin5Lp(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin5Lp(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin5Lp(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin5Lp(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN5_EOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin5Lp dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin6SOF(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin6SOF(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin6SOF(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin6SOF(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN6_SOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin6SOF dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }
#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin6Lp(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin6Lp(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin6Lp(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin6Lp(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN6_EOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin6Lp dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin7SOF(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin7SOF(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin7SOF(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin7SOF(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN7_SOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin7SOF dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }
#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin7Lp(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin7Lp(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin7Lp(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin7Lp(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN7_EOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin7Lp dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin8SOF(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin8SOF(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin8SOF(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin8SOF(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN8_SOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin8SOF dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }
#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin8Lp(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin8Lp(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin8Lp(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin8Lp(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN8_EOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin8Lp dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin9SOF(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin9SOF(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin9SOF(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin9SOF(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN9_SOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin9SOF dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }
#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin9Lp(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin9Lp(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin9Lp(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin9Lp(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN9_EOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin9Lp dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin10SOF(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin10SOF(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin10SOF(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin10SOF(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN10_SOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin10SOF dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }
#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin10Lp(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin10Lp(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin10Lp(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin10Lp(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN10_EOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin10Lp dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin11SOF(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin11SOF(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin11SOF(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin11SOF(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN11_SOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin11SOF dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }
#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin11Lp(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin11Lp(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin11Lp(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin11Lp(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN11_EOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin11Lp dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin12SOF(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin12SOF(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin12SOF(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin12SOF(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN12_SOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin12SOF dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }
#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin12Lp(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin12Lp(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin12Lp(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin12Lp(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN12_EOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin12Lp dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin13SOF(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin13SOF(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin13SOF(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin13SOF(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN13_SOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin13SOF dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
    }
#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#if defined(CONFIG_THREADX)
static void ISRVin13Lp(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVin13Lp(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVin13Lp(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVin13Lp(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VinIsrFlag, AMBA_DSP_VIN13_EOF);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVin13Lp dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
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

#if defined(CONFIG_THREADX)
static void ISRVout2(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t ISRVout2(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void ISRVout2(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *ISRVout2(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
AmbaMisra_TouchUnused(&IntID);
AmbaMisra_TouchUnused(&UserArg);
    Rval = dsp_osal_eventflag_set(&VoutIsrFlag, AMBA_DSP_VOUT2_INT);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "ISRVout2 dsp_osal_eventflag_set ret [0x%x]", Rval, 0U, 0U);
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

static inline UINT32 VinInterruptInitGrp0(void)
{
    UINT32 Rval;

    /* Vin0 SOF */
    Rval = InterruptInit_HookHdlr(0U, DSP_IRQ_TYPE_VIN_SOF, ISRVin0SOF);

    /* Vin0 EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(0U, DSP_IRQ_TYPE_VIN_EOF, ISRVin0Lp);
    }

    /* Vin0VC1(Vin1) SOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(1U, DSP_IRQ_TYPE_VIN_SOF, ISRVin1SOF);
    }

    /* Vin0VC1(Vin1) EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(1U, DSP_IRQ_TYPE_VIN_EOF, ISRVin1Lp);
    }

    /* Vin0VC2(Vin2) SOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(2U, DSP_IRQ_TYPE_VIN_SOF, ISRVin2SOF);
    }

    /* Vin0VC2(Vin2) EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(2U, DSP_IRQ_TYPE_VIN_EOF, ISRVin2Lp);
    }

    /* Vin0VC3(Vin3) SOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(3U, DSP_IRQ_TYPE_VIN_SOF, ISRVin3SOF);
    }

    /* Vin0VC3(Vin3) EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(3U, DSP_IRQ_TYPE_VIN_EOF, ISRVin3Lp);
    }

    return Rval;
}

static inline UINT32 VinInterruptInitGrp1(void)
{
    UINT32 Rval;

    /* Vin4 SOF */
    Rval = InterruptInit_HookHdlr(4U, DSP_IRQ_TYPE_VIN_SOF, ISRVin4SOF);

    /* Vin4 EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(4U, DSP_IRQ_TYPE_VIN_EOF, ISRVin4Lp);
    }

    /* Vin4VC1(Vin5) SOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(5U, DSP_IRQ_TYPE_VIN_SOF, ISRVin5SOF);
    }

    /* Vin4VC1(Vin5) EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(5U, DSP_IRQ_TYPE_VIN_EOF, ISRVin5Lp);
    }

    /* Vin4VC2(Vin6) SOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(6U, DSP_IRQ_TYPE_VIN_SOF, ISRVin6SOF);
    }

    /* Vin4VC2(Vin6) EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(6U, DSP_IRQ_TYPE_VIN_EOF, ISRVin6Lp);
    }

    /* Vin4VC3(Vin7) SOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(7U, DSP_IRQ_TYPE_VIN_SOF, ISRVin7SOF);
    }

    /* Vin4VC3(Vin7) EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(7U, DSP_IRQ_TYPE_VIN_EOF, ISRVin7Lp);
    }

    return Rval;
}

static inline UINT32 VinInterruptInitGrp2(void)
{
    UINT32 Rval;

    /* Vin8 SOF */
    Rval = InterruptInit_HookHdlr(8U, DSP_IRQ_TYPE_VIN_SOF, ISRVin8SOF);

    /* Vin8 EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(8U, DSP_IRQ_TYPE_VIN_EOF, ISRVin8Lp);
    }

    /* Vin8VC1(Vin9) SOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(9U, DSP_IRQ_TYPE_VIN_SOF, ISRVin9SOF);
    }

    /* Vin8VC1(Vin9) EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(9U, DSP_IRQ_TYPE_VIN_EOF, ISRVin9Lp);
    }

    /* Vin8VC2(Vin10) SOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(10U, DSP_IRQ_TYPE_VIN_SOF, ISRVin10SOF);
    }

    /* Vin8VC2(Vin10) EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(10U, DSP_IRQ_TYPE_VIN_EOF, ISRVin10Lp);
    }

    return Rval;

}

static inline UINT32 VinInterruptInitGrp3(void)
{
    UINT32 Rval;

    /* Vin11 SOF */
    Rval = InterruptInit_HookHdlr(11U, DSP_IRQ_TYPE_VIN_SOF, ISRVin11SOF);

    /* Vin11 EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(11U, DSP_IRQ_TYPE_VIN_EOF, ISRVin11Lp);
    }

    /* Vin11VC1(Vin12) SOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(12U, DSP_IRQ_TYPE_VIN_SOF, ISRVin12SOF);
    }

    /* Vin11VC1(Vin12) EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(12U, DSP_IRQ_TYPE_VIN_EOF, ISRVin12Lp);
    }

    /* Vin11VC2(Vin13) SOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(13U, DSP_IRQ_TYPE_VIN_SOF, ISRVin13SOF);
    }

    /* Vin11VC2(Vin13) EOF */
    if (Rval == OK) {
        Rval = InterruptInit_HookHdlr(13U, DSP_IRQ_TYPE_VIN_EOF, ISRVin13Lp);
    }

    return Rval;
}

static inline UINT32 VinInterruptInit(void)
{
    UINT32 Rval;

    /* Vin0~3 */
    Rval = VinInterruptInitGrp0();

    /* Vin4~7 */
    if (Rval == OK) {
        Rval = VinInterruptInitGrp1();
    }

    /* Vin8~10 */
    if (Rval == OK) {
        Rval = VinInterruptInitGrp2();
    }

    /* Vin11~13 */
    if (Rval == OK) {
        Rval = VinInterruptInitGrp3();
    }

    return Rval;
}

UINT32 LL_InterruptInit(void)
{
    static UINT8 LL_InterruptInitFlag = 0;
    static char VinFlag[] = "DspVinFlg";
    static char VoutFlag[] = "DspVoutFlg";
    UINT32 Rval = DSP_ERR_NONE;

    if (LL_InterruptInitFlag == 0U) {
        UINT32 ErrLine = 0U;

        /* ISR Flag */
        Rval = dsp_osal_eventflag_init(&VinIsrFlag, VinFlag);
        Rval |= dsp_osal_eventflag_init(&VoutIsrFlag, VoutFlag);
        if (Rval != DSP_ERR_NONE) {
            ErrLine = __LINE__;
            Rval = DSP_ERR_0003;
        } else {
            /* All Vin interrupt init*/
            Rval = VinInterruptInit();
            if (Rval != DSP_ERR_NONE) {
                ErrLine = __LINE__;
            } else {
                /* Vout0 */
                Rval = InterruptInit_HookHdlr(0U, DSP_IRQ_TYPE_VOUT, ISRVout0);
                if (Rval != DSP_ERR_NONE) {
                    ErrLine = __LINE__;
                }

                /* Vout1 */
                if (Rval == DSP_ERR_NONE){
                    Rval = InterruptInit_HookHdlr(1U, DSP_IRQ_TYPE_VOUT, ISRVout1);
                    if (Rval != OK) {
                        ErrLine = __LINE__;
                    }
                }

                /* Vout2 */
                if (Rval == DSP_ERR_NONE){
                    Rval = InterruptInit_HookHdlr(2U, DSP_IRQ_TYPE_VOUT, ISRVout2);
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

