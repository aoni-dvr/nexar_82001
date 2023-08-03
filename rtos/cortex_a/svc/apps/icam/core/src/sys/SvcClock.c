/**
*  @file SvcClock.c
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
*  @details svc clock functions
*
*/

#include ".svc_autogen"

#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaSYS.h"
#include "AmbaDef.h"
#include "AmbaWDT.h"
#include "AmbaPrint.h"
#include "AmbaDSP.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcDSP.h"
#include "SvcClock.h"
#include "SvcSafeStateMgr.h"

#define SVC_CLK_FEA_ENABLE

/* cv2x:    0-AQ, 1-AX                      */
/* cv2xfs:  0-CV2FS, 1-CV22FS, 2-CV22FS15   */
#define SVC_CLK_LINE_NUM    (5U)

#define SVC_LOG_CLK     "CLK"

typedef struct {
    UINT32  Core;
    UINT32  Idsp;
    UINT32  Vecp;
    UINT32  Spu;
    UINT32  IdspV;
} SVC_CLK_LINE_s;

typedef struct {
    UINT32 (*pfnSetClkFreq)(UINT32 ClkID, UINT32 DesiredFreq, UINT32 *pActualFreq);
    UINT32 (*pfnGetClkFreq)(UINT32 ClkID, UINT32 *pFreq);
    UINT32 (*pfnEnableFeature)(UINT32 SysFeature);
    UINT32 (*pfnDisableFeature)(UINT32 SysFeature);
} SVC_CLK_FUNC_s;

static AMBA_KAL_MUTEX_t CodecMutex;
static UINT32          g_ClkFeatureBits GNU_SECTION_NOZEROINIT;

static SVC_CLK_FUNC_s  g_ClkFuncs = {
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
    .pfnSetClkFreq     = SvcSafeStateMgr_SetClkFreq,
    .pfnGetClkFreq     = SvcSafeStateMgr_GetClkFreq,
    .pfnEnableFeature  = SvcSafeStateMgr_EnableFeature,
    .pfnDisableFeature = SvcSafeStateMgr_DisableFeature,
#else
    .pfnSetClkFreq     = AmbaSYS_SetClkFreq,
    .pfnGetClkFreq     = AmbaSYS_GetClkFreq,
    .pfnEnableFeature  = AmbaSYS_EnableFeature,
    .pfnDisableFeature = AmbaSYS_DisableFeature,
#endif
};

static void ClockSet(SVC_CLK_LINE_s *pClkLine)
{
    UINT32  Rval;
    UINT32  Freq;

    Rval = g_ClkFuncs.pfnSetClkFreq(AMBA_SYS_CLK_CORE, pClkLine->Core, &Freq);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_CLK, "## fail to set core clock", 0U, 0U);
    }

    Rval = g_ClkFuncs.pfnSetClkFreq(AMBA_SYS_CLK_IDSP, pClkLine->Idsp, &Freq);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_CLK, "## fail to set idsp clock", 0U, 0U);
    }

#if defined(AMBA_SYS_CLK_VISION)
    Rval = g_ClkFuncs.pfnSetClkFreq(AMBA_SYS_CLK_VISION, pClkLine->Vecp, &Freq);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_CLK, "## fail to set vecp clock", 0U, 0U);
    }
#endif

#if defined(AMBA_SYS_CLK_FEX)
    Rval = g_ClkFuncs.pfnSetClkFreq(AMBA_SYS_CLK_FEX, pClkLine->Spu, &Freq);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_CLK, "## fail to set spu clock", 0U, 0U);
    }
#endif

#if defined(AMBA_SYS_CLK_IDSPV)
    Rval = g_ClkFuncs.pfnSetClkFreq(AMBA_SYS_CLK_IDSPV, pClkLine->IdspV, &Freq);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_CLK, "## fail to set IdspV clock", 0U, 0U);
    }
#endif

    AmbaMisra_TouchUnused(pClkLine);
}


/**
* initialization of system clocks
* @param [in] ClkLine clock line, 0-line0, 1-line1
* @return none
*/
void SvcClock_Init(UINT32 ClkLine)
{
    static SVC_CLK_LINE_s  g_SvcClkLines[SVC_CLK_LINE_NUM] GNU_SECTION_NOZEROINIT;

    UINT32  Line;
    UINT32  Err;

    if (AmbaWrap_memset(g_SvcClkLines, 0, sizeof(g_SvcClkLines)) != SVC_OK) {
        SvcLog_NG(SVC_LOG_CLK, "## fail to memset", 0U, 0U);
    }

    Err = AmbaKAL_MutexCreate(&CodecMutex, NULL);
    if (Err != KAL_ERR_NONE) {
        SvcLog_NG(SVC_LOG_CLK, "AmbaKAL_MutexCreate failed %u", Err, 0U);
    }

    /* line0 */
    #if defined(SVC_CLK_L0_CORE)
    Line = 0U;
    g_SvcClkLines[Line].Core  = SVC_CLK_L0_CORE;
    g_SvcClkLines[Line].Idsp  = SVC_CLK_L0_IDSP;
    #if defined(SVC_CLK_L0_VPU)
    g_SvcClkLines[Line].Vecp  = SVC_CLK_L0_VPU;
    #endif
    #if defined(SVC_CLK_L0_SPU)
    g_SvcClkLines[Line].Spu   = SVC_CLK_L0_SPU;
    #endif
    #if defined(SVC_CLK_L0_IDSPV)
    g_SvcClkLines[Line].IdspV = SVC_CLK_L0_IDSPV;
    #endif
    #endif

    /* line1 */
    #if defined(SVC_CLK_L1_CORE)
    Line = 1U;
    g_SvcClkLines[Line].Core  = SVC_CLK_L1_CORE;
    g_SvcClkLines[Line].Idsp  = SVC_CLK_L1_IDSP;
    #if defined(SVC_CLK_L1_VPU)
    g_SvcClkLines[Line].Vecp  = SVC_CLK_L1_VPU;
    #endif
    #if defined(SVC_CLK_L1_SPU)
    g_SvcClkLines[Line].Spu   = SVC_CLK_L1_SPU;
    #endif
    #if defined(SVC_CLK_L1_IDSPV)
    g_SvcClkLines[Line].IdspV = SVC_CLK_L1_IDSPV;
    #endif
    #endif

    /* line2 */
    #if defined(SVC_CLK_L2_CORE)
    Line = 2U;
    g_SvcClkLines[Line].Core  = SVC_CLK_L2_CORE;
    g_SvcClkLines[Line].Idsp  = SVC_CLK_L2_IDSP;
    #if defined(SVC_CLK_L2_VPU)
    g_SvcClkLines[Line].Vecp  = SVC_CLK_L2_VPU;
    #endif
    #if defined(SVC_CLK_L2_SPU)
    g_SvcClkLines[Line].Spu   = SVC_CLK_L2_SPU;
    #endif
    #if defined(SVC_CLK_L2_IDSPV)
    g_SvcClkLines[Line].IdspV = SVC_CLK_L2_IDSPV;
    #endif
    #endif

    /* line3 */
    #if defined(SVC_CLK_L3_CORE)
    Line = 3U;
    g_SvcClkLines[Line].Core  = SVC_CLK_L3_CORE;
    g_SvcClkLines[Line].Idsp  = SVC_CLK_L3_IDSP;
    #if defined(SVC_CLK_L3_VPU)
    g_SvcClkLines[Line].Vecp  = SVC_CLK_L3_VPU;
    #endif
    #if defined(SVC_CLK_L3_SPU)
    g_SvcClkLines[Line].Spu   = SVC_CLK_L3_SPU;
    #endif
    #if defined(SVC_CLK_L3_IDSPV)
    g_SvcClkLines[Line].IdspV = SVC_CLK_L3_IDSPV;
    #endif
    #endif

    /* line4 */
    #if defined(SVC_CLK_L4_CORE)
    Line = 4U;
    g_SvcClkLines[Line].Core  = SVC_CLK_L4_CORE;
    g_SvcClkLines[Line].Idsp  = SVC_CLK_L4_IDSP;
    #if defined(SVC_CLK_L4_VPU)
    g_SvcClkLines[Line].Vecp  = SVC_CLK_L4_VPU;
    #endif
    #if defined(SVC_CLK_L4_SPU)
    g_SvcClkLines[Line].Spu   = SVC_CLK_L4_SPU;
    #endif
    #if defined(SVC_CLK_L4_IDSPV)
    g_SvcClkLines[Line].IdspV = SVC_CLK_L4_IDSPV;
    #endif
    #endif

    /* set init clock */
    if (SvcDSP_IsCleanBoot() == SVCDSP_OP_CLEANBOOT) {
        ClockSet(&(g_SvcClkLines[ClkLine]));
    }

    /* clock feature init */
    g_ClkFeatureBits = 0U;
#if defined(SVCAG_SOC_CLK_FEA_CTRL)
#if defined(SVC_CLK_FEA_ENABLE)
    SvcClock_FeatureCtrl(1U, SVC_CLK_FEA_BIT_DSP | SVC_CLK_FEA_BIT_CNN);
#else
    (void)AmbaSYS_EnableFeature(AMBA_SYS_FEATURE_ALL);
#endif
#endif
}

/**
* feature control of clocks
* @param [in] Ctrl feature control, 0-off, 1-on
* @param [in] FeatureBits bits of feature
* @return none
*/
void SvcClock_FeatureCtrl(UINT32 Ctrl, UINT32 FeatureBits)
{
#if defined(SVCAG_SOC_CLK_FEA_CTRL) && defined(SVC_CLK_FEA_ENABLE)

    #define SVC_FEATURE_NULL        (0xFFFFFFFFU)
    static UINT32 CodecCtrlCount = 0U;
    UINT32  Err;
    UINT32  i, Bit, Fid;
    UINT32  FeatureID[SVC_CLK_FEATURE_NUM];

    AmbaSvcWrap_MisraMemset(FeatureID, 0xFF, sizeof(FeatureID));

    i = 0U;
    FeatureID[i] = AMBA_SYS_FEATURE_ALL;    i++;
    FeatureID[i] = AMBA_SYS_FEATURE_DSP;    i++;
    FeatureID[i] = AMBA_SYS_FEATURE_MCTF;   i++;
    FeatureID[i] = AMBA_SYS_FEATURE_CODEC;
#if defined(AMBA_SYS_FEATURE_HEVC)
    i++;    /* Rule 2.2 */
    FeatureID[i] = AMBA_SYS_FEATURE_HEVC;
#endif
#if defined(AMBA_SYS_FEATURE_CNN)
    i++;    /* Rule 2.2 */
    FeatureID[i] = AMBA_SYS_FEATURE_CNN;
#endif
#if defined(AMBA_SYS_FEATURE_FEX)
    i++;    /* Rule 2.2 */
    FeatureID[i] = AMBA_SYS_FEATURE_FEX;
#endif
#if defined(AMBA_SYS_FEATURE_FMA)
    i++;    /* Rule 2.2 */
    FeatureID[i] = AMBA_SYS_FEATURE_FMA;
#endif

    for (i = 0U; i < SVC_CLK_FEATURE_NUM; i++) {
        Fid = FeatureID[i];
        if (Fid == SVC_FEATURE_NULL) {
            /*SvcLog_DBG(SVC_LOG_CLK, "## clk_feature 0x%X isn't supported", Fid, 0U);*/
            continue;
        }
        Bit = ((UINT32)0x01U << i);

        if ((CodecCtrlCount == 0xFFFFFFFFU) && (Bit == SVC_CLK_FEA_BIT_CODEC)) {
            SvcLog_NG(SVC_LOG_CLK, "## Overflow clk_feature on 0x%X", Fid, 0U);
            continue;
        }
        if (0U < CheckBits(FeatureBits, Bit)) {
            if (0U < Ctrl) {
                if (Bit == SVC_CLK_FEA_BIT_CODEC) {
                    Err = AmbaKAL_MutexTake(&CodecMutex, 1000U);
                    if (Err != KAL_ERR_NONE) {
                        SvcLog_NG(SVC_LOG_CLK, "Ctrl1 CodecMutex Take Mutex fail %u", Err, 0U);
                    } else {
                        CodecCtrlCount++;
                        Err = AmbaKAL_MutexGive(&CodecMutex);
                        if (Err != KAL_ERR_NONE) {
                            SvcLog_NG(SVC_LOG_CLK, "Ctrl1 CodecMutex Give Mutex failed %u", Err, 0U);
                        }
                    }
                }
                if (CheckBits(g_ClkFeatureBits, Bit) == 0U) {
                    g_ClkFeatureBits = SetBits(g_ClkFeatureBits, Bit);
                    if (g_ClkFuncs.pfnEnableFeature(Fid) == SVC_OK) {
                        SvcLog_DBG(SVC_LOG_CLK, "## on clk_feature 0x%X", Fid, 0U);
                    } else {
                        SvcLog_NG(SVC_LOG_CLK, "## fail to on clk_feature 0x%X", Fid, 0U);
                    }
                }
            } else {
                if (0U < CheckBits(g_ClkFeatureBits, Bit)) {
                    if (Bit == SVC_CLK_FEA_BIT_CODEC) {
                        Err = AmbaKAL_MutexTake(&CodecMutex, 1000U);
                        if (Err != KAL_ERR_NONE) {
                            SvcLog_NG(SVC_LOG_CLK, "Ctrl0 CodecMutex Take Mutex fail %u", Err, 0U);
                        } else {

#if defined(CONFIG_ICAM_BIST_UCODE)
                            /* Don't turn off codec clocks if bist is on because encoder bist is run at background */
                            /* Do nothing */
#else
                            CodecCtrlCount--;
#endif

                            Err = AmbaKAL_MutexGive(&CodecMutex);
                            if (Err != KAL_ERR_NONE) {
                                SvcLog_NG(SVC_LOG_CLK, "Ctrl0 CodecMutex Give Mutex failed %u", Err, 0U);
                            }
                        }
                        if (CodecCtrlCount != 0U) {
                            SvcLog_DBG(SVC_LOG_CLK, "## keep clk_feature on 0x%X", Fid, 0U);
                            continue;
                        }
                    }
                    g_ClkFeatureBits = ClearBits(g_ClkFeatureBits, Bit);
                    if (g_ClkFuncs.pfnDisableFeature(Fid) == SVC_OK) {
                        SvcLog_DBG(SVC_LOG_CLK, "## off clk_feature 0x%X", Fid, 0U);
                    } else {
                        SvcLog_NG(SVC_LOG_CLK, "## fail to off clk_feature 0x%X", Fid, 0U);
                    }
                }
            }
        }
    }
#else
    AmbaMisra_TouchUnused(&Ctrl);
    AmbaMisra_TouchUnused(&FeatureBits);
#endif
}

/**
* control of clocks
* @param [in] pClkCfg clocks setting
* @return none
*/
void SvcClock_Ctrl(SVC_CLK_CFG_s *pClkCfg)
{
    UINT32  Freq;

    if (g_ClkFuncs.pfnGetClkFreq(AMBA_SYS_CLK_CORE, &Freq) == OK) {
        if ((pClkCfg->CoreClk != 0U) && (pClkCfg->CoreClk != Freq)) {
            if (g_ClkFuncs.pfnSetClkFreq(AMBA_SYS_CLK_CORE, pClkCfg->CoreClk, &Freq) == OK) {
                SvcLog_DBG(SVC_LOG_CLK, "#### update core clock to %u(%u) ", pClkCfg->CoreClk, Freq);
            } else {
                SvcLog_NG(SVC_LOG_CLK, "#### fail to set core clock", 0U, 0U);
            }
        }
    }

    if (g_ClkFuncs.pfnGetClkFreq(AMBA_SYS_CLK_IDSP, &Freq) == OK) {
        if ((pClkCfg->IdspClk != 0U) && (pClkCfg->IdspClk != Freq)) {
            if (g_ClkFuncs.pfnSetClkFreq(AMBA_SYS_CLK_IDSP, pClkCfg->IdspClk, &Freq) == OK) {
                SvcLog_DBG(SVC_LOG_CLK, "#### update idsp clock to %u(%u) ", pClkCfg->IdspClk, Freq);
            } else {
                SvcLog_NG(SVC_LOG_CLK, "#### fail to set idsp clock", 0U, 0U);
            }

        #if defined(AMBA_SYS_CLK_IDSPV)
            if (g_ClkFuncs.pfnSetClkFreq(AMBA_SYS_CLK_IDSPV, pClkCfg->IdspClk, &Freq) == OK) {
                SvcLog_DBG(SVC_LOG_CLK, "#### update idspv clock to %u(%u) ", pClkCfg->IdspClk, Freq);
            } else {
                SvcLog_NG(SVC_LOG_CLK, "#### fail to set idspv clock", 0U, 0U);
            }
        #endif
        }
    }

#if defined(AMBA_SYS_CLK_VISION)
    if (g_ClkFuncs.pfnGetClkFreq(AMBA_SYS_CLK_VISION, &Freq) == OK) {
        if ((pClkCfg->VpuClk != 0U) && (pClkCfg->VpuClk != Freq)) {
            if (g_ClkFuncs.pfnSetClkFreq(AMBA_SYS_CLK_VISION, pClkCfg->VpuClk, &Freq) == OK) {
                SvcLog_DBG(SVC_LOG_CLK, "#### update vpu clock to %u(%u) ", pClkCfg->VpuClk, Freq);
            } else {
                SvcLog_NG(SVC_LOG_CLK, "#### fail to set vpu clock", 0U, 0U);
            }
        }
    }
#endif

    AmbaMisra_TouchUnused(pClkCfg);
}

/**
* status dump of clocks
* @return none
*/
void SvcClock_Dump(void)
{
    UINT32  Freq;

    AmbaPrint_PrintUInt5("[Clocks]", 0U, 0U, 0U, 0U, 0U);

    if (AmbaSYS_GetClkFreq(AMBA_SYS_CLK_CORTEX, &Freq) == OK) {
        AmbaPrint_PrintUInt5(" cortex      : %u", Freq, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5(" cortex      : n/a", 0U, 0U, 0U, 0U, 0U);
    }

    if (AmbaSYS_GetClkFreq(AMBA_SYS_CLK_DRAM, &Freq) == OK) {
        AmbaPrint_PrintUInt5(" dram        : %u", Freq, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5(" dram        : n/a", 0U, 0U, 0U, 0U, 0U);
    }

    if (g_ClkFuncs.pfnGetClkFreq(AMBA_SYS_CLK_CORE, &Freq) == OK) {
        AmbaPrint_PrintUInt5(" core        : %u", Freq, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5(" core        : n/a", 0U, 0U, 0U, 0U, 0U);
    }

    if (g_ClkFuncs.pfnGetClkFreq(AMBA_SYS_CLK_IDSP, &Freq) == OK) {
        AmbaPrint_PrintUInt5(" idsp        : %u", Freq, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5(" idsp        : n/a", 0U, 0U, 0U, 0U, 0U);
    }

#if defined(AMBA_SYS_CLK_IDSPV)
    if (g_ClkFuncs.pfnGetClkFreq(AMBA_SYS_CLK_IDSPV, &Freq) == OK) {
        AmbaPrint_PrintUInt5(" idspv       : %u", Freq, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5(" idspv       : n/a", 0U, 0U, 0U, 0U, 0U);
    }
#endif

#if defined(AMBA_SYS_CLK_VISION)
    if (g_ClkFuncs.pfnGetClkFreq(AMBA_SYS_CLK_VISION, &Freq) == OK) {
        AmbaPrint_PrintUInt5(" vision      : %u", Freq, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5(" vision      : n/a", 0U, 0U, 0U, 0U, 0U);
    }
#endif

#if defined(AMBA_SYS_CLK_FEX)
    if (g_ClkFuncs.pfnGetClkFreq(AMBA_SYS_CLK_FEX, &Freq) == OK) {
        AmbaPrint_PrintUInt5(" spu         : %u", Freq, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5(" spu         : n/a", 0U, 0U, 0U, 0U, 0U);
    }
#endif

#if defined(AMBA_SYS_CLK_SD)
    if (AmbaSYS_GetClkFreq(AMBA_SYS_CLK_SD, &Freq) == OK) {
        AmbaPrint_PrintUInt5(" sd          : %u", Freq, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5(" sd          : n/a", 0U, 0U, 0U, 0U, 0U);
    }
#endif

    if (AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, &Freq) == OK) {
        AmbaPrint_PrintUInt5(" gclk_au     : %u", Freq, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5(" gclk_au     : n/a", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaPrint_PrintUInt5("", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5(" feature_bits: 0x%08X", g_ClkFeatureBits, 0U, 0U, 0U, 0U);
}
