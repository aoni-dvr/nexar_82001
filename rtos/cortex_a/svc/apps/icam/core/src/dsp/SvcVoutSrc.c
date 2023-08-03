/**
 *  @file SvcVoutSrc.c
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
 *  @details svc vout src task
 *
 */

#include "AmbaHDMI.h"
#include "AmbaHDMI_Def.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaFPD.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
#include "AmbaVOUT_Priv.h"
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"
#endif

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcTask.h"
#include "SvcTiming.h"
#include "SvcDSP.h"
#include "SvcVoutSrc.h"

#define SVC_LOG_VOUT_SRC            "VOUT_SRC"
#define VOUT_SRC_TASK_STACK_SIZE    (0x5000U)

static SVC_VOUT_SRC_INIT_s SvcVoutSrcInit GNU_SECTION_NOZEROINIT;
static SVC_TASK_CTRL_s VoutSrcTaskCtrl GNU_SECTION_NOZEROINIT;

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
static AMBA_KAL_EVENT_FLAG_t VoutSrcFlag;
#endif

static void* VoutSrcTaskEntry(void* EntryArg)
{
    UINT32       i, Rval;
    UINT32       VoutID, EnableFpd;
    const ULONG  *pArg;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_VOUT_CONFIG_START, "Vout config START");
    #endif

    EnableFpd = (UINT32) (*pArg);

    SvcLog_DBG(SVC_LOG_VOUT_SRC, "@@ vout init begin", 0U, 0U);

    if (0U < SvcDSP_IsCleanBoot()) {
        /* Force to reset Fpd via GPIO, defined in bsp.c */
        extern void AmbaUserGPIO_FpdCtrl(UINT32 FpdFlag);

        AmbaUserGPIO_FpdCtrl(1U);
        Rval = AmbaKAL_TaskSleep(5U);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_VOUT_SRC, "AmbaKAL_TaskSleep failed 0x%x", Rval, 0U);
        }
    }

    /* hook and init */
    for (i = 0U; i < SvcVoutSrcInit.NumInit; i++) {
        VoutID = SvcVoutSrcInit.Dev[i].VoutID;

        if (EnableFpd == 1U) {
            Rval = AmbaFPD_Hook(VoutID, SvcVoutSrcInit.Dev[i].pFpdObj);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_VOUT_SRC, "FPD_Hook(%d) failed 0x%x", VoutID, Rval);
            }
        }

        if (0U < SvcDSP_IsCleanBoot()) {
            SvcLog_OK(SVC_LOG_VOUT_SRC, "FPD_Config(%d) start", VoutID, 0);
            Rval = AmbaFPD_Config(VoutID, (UINT8) SvcVoutSrcInit.Dev[i].DevMode);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_VOUT_SRC, "FPD_Config(%d) failed 0x%x", VoutID, Rval);
            } else {
                SvcLog_OK(SVC_LOG_VOUT_SRC, "FPD_Config(%d) end", VoutID, 0);
            }

            if (EnableFpd == 1U) {
                Rval = AmbaFPD_Enable(VoutID);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_VOUT_SRC, "FPD_Enable(%d) failed 0x%x", VoutID, Rval);
                }
            }
        #if defined(CONFIG_FWPROG_R52SYS_ENABLE) && !defined(CONFIG_QNX)
            if (SVC_OK == Rval) {
                Rval = AmbaVout_SafetyStart(VoutID);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_VOUT_SRC, "## AmbaVout_SafetyStart(%d) failed 0x%x", VoutID, Rval);
                } else {
                    SvcLog_OK(SVC_LOG_VOUT_SRC, "## AmbaVout_SafetyStart(%d)", VoutID, 0);
                }
            }
        #endif
        }
    }

    if (SvcVoutSrcInit.pInitDoneCB != NULL) {
        SvcVoutSrcInit.pInitDoneCB();
    }

    SvcLog_DBG(SVC_LOG_VOUT_SRC, "@@ vout init done", 0U, 0U);
    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_VOUT_CONFIG_DONE, "Vout config DONE");
    #endif

    return NULL;
}

/**
* initialization of vout source task
* @param [in] pInit info block of vout source
* @param [in] EnableFpd control of display device
* @param [in] Priority task priority
* @param [in] CpuBits task cpu bits
* @return none
*/
void SvcVoutSrc_Init(const SVC_VOUT_SRC_INIT_s *pInit, UINT32 EnableFpd, UINT32 Priority, UINT32 CpuBits)
{
    static UINT8 VoutSrcTaskStack[VOUT_SRC_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    UINT32  Rval;

    SvcLog_DBG(SVC_LOG_VOUT_SRC, "@@ vout task create begin", 0U, 0U);

    Rval = AmbaWrap_memcpy(&SvcVoutSrcInit, pInit, sizeof(SVC_VOUT_SRC_INIT_s));

    /* trigger task to init vout source */
    if (SVC_OK == Rval) {
        VoutSrcTaskCtrl.Priority   = Priority;
        VoutSrcTaskCtrl.EntryFunc  = VoutSrcTaskEntry;
        VoutSrcTaskCtrl.pStackBase = VoutSrcTaskStack;
        VoutSrcTaskCtrl.StackSize  = VOUT_SRC_TASK_STACK_SIZE;
        VoutSrcTaskCtrl.CpuBits    = CpuBits;
        VoutSrcTaskCtrl.EntryArg   = EnableFpd;

        Rval = SvcTask_Create("SvcVoutSrcTask", &VoutSrcTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_VOUT_SRC, "vout_src_task isn't created", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_VOUT_SRC, "vout_src_task isn't created", 0U, 0U);
    }
}

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
static UINT32 VoutDataReady(const void *pEventInfo)
{
    const AMBA_DSP_VOUT_DATA_INFO_s *pInfo;
    UINT32 Rval;

    AmbaMisra_TypeCast(&pInfo, &pEventInfo);
    if (pInfo->Status == 1U) {
        Rval = AmbaKAL_EventFlagSet(&VoutSrcFlag, (UINT32)1U << (UINT32)pInfo->VoutIdx);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_VOUT_SRC, "AmbaKAL_EventFlagSet VoutId %u failed! return 0x%x", pInfo->VoutIdx, Rval);
        }
    } else {
        Rval = OK;
    }
    return Rval;
}
#endif

/**
* de-initialization of vout source task
* @param [in] pInit info block of vout source
* @param [in] DisableFpd control of display device
* @return none
*/
void SvcVoutSrc_DeInit(const SVC_VOUT_SRC_INIT_s *pInit, UINT32 DisableFpd)
{
    UINT32                i, Rval, VoutID;
    const SVC_VOUT_DEV_s  *pDev;

    Rval = SvcTask_Destroy(&VoutSrcTaskCtrl);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_VOUT_SRC, "## fail to destroy vout src task (%u)", Rval, 0U);
    }

    if (DisableFpd > 0U) {

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        UINT32 ActualFlag = 0;
        static char  FlgName[] = "VoutSrcFlag";

        Rval = AmbaKAL_EventFlagCreate(&VoutSrcFlag, FlgName);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_VOUT_SRC, "Event flag create failed with 0x%x", Rval, 0U);
        }
        Rval = AmbaKAL_EventFlagClear(&VoutSrcFlag, 0xFFFFFFFFU);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_VOUT_SRC, "Event flag clear failed with 0x%x", Rval, 0U);
        }
        Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VOUT_DATA_RDY, VoutDataReady);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_VOUT_SRC, "AmbaDSP_EventHandlerRegister fail, return 0x%x", Rval, 0);
        }
#endif

        for (i = 0; i < pInit->NumInit; i++) {
            pDev = &(SvcVoutSrcInit.Dev[i]);

            VoutID = pDev->VoutID;

#if defined(CONFIG_FWPROG_R52SYS_ENABLE) && !defined(CONFIG_QNX)
            Rval = AmbaVout_SafetyStop(VoutID);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_VOUT_SRC, "## AmbaVout_SafetyStop(%d) failed 0x%x", VoutID, Rval);
            } else {
                SvcLog_OK(SVC_LOG_VOUT_SRC, "## AmbaVout_SafetyStop(%d)", VoutID, 0);
            }
#endif
            /* skip 'vout reset' if cvbs */
            if (0U < SvcDSP_IsCleanBoot()) {
                if (SvcWrap_strcmp(pDev->pFpdObj->pName, "CVBS") != 0) {
                    Rval = AmbaDSP_VoutReset((UINT8) VoutID);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_VOUT_SRC, "## VoutReset(%d) failed 0x%x", VoutID, Rval);
                    } else {
                        SvcLog_OK(SVC_LOG_VOUT_SRC, "## VoutReset(%d)", VoutID, 0U);
                    }
                }
            }

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            Rval = AmbaKAL_EventFlagGet(&VoutSrcFlag, ((UINT32)1U << VoutID), AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlag, 1000);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_VOUT_SRC, "Event flag get failed with 0x%x", Rval, 0U);
            }
#endif

            SvcLog_DBG(SVC_LOG_VOUT_SRC, "@@ FPD_Disable(%u) start", VoutID, 0U);
            Rval = AmbaFPD_Disable(VoutID);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_VOUT_SRC, "## fail to vin src disable (%u)", Rval, 0U);
            } else {
                SvcLog_DBG(SVC_LOG_VOUT_SRC, "@@ FPD_Disable(%u) end", VoutID, 0U);
            }
        }

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        Rval = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_VOUT_DATA_RDY, VoutDataReady);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_VOUT_SRC, "AmbaDSP_EventHandlerUnRegister fail, return 0x%x", Rval, 0);
        }
        Rval = AmbaKAL_EventFlagDelete(&VoutSrcFlag);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_VOUT_SRC, "Flag delete failed! return 0x%x", Rval, 0U);
        }
#endif

        if (pInit->NumInit > 1U) {
            /* need to wait vout reset done for format_id switch */
            Rval = AmbaKAL_TaskSleep(15U);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_VOUT_SRC, "## fail to AmbaKAL_TaskSleep (%u)", Rval, 0U);
            }
        }
    }
}

/**
* control of vout source
* @param [in] Enable 0 or 1
* @param [in] IsModeChg 0(no delay) 1(backlight delay)
* @return none
*/
void SvcVoutSrc_Ctrl(UINT32 Enable, UINT32 IsModeChg)
{
    UINT32  Rval;
    UINT32  i, VoutID;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(&IsModeChg);
    AmbaMisra_TouchUnused(&Enable);

    if (IsModeChg == 0U) {
        /* nothing */
    }

    for (i = 0U; i < SvcVoutSrcInit.NumInit; i++) {
        VoutID = SvcVoutSrcInit.Dev[i].VoutID;
        Rval = AmbaFPD_SetBacklight(VoutID, Enable);
        if (Rval != OK) {
            SvcLog_DBG(SVC_LOG_VOUT_SRC, "## fail to enable blacklight(%u), rval(%u)", VoutID, Rval);
        }
    }
}
