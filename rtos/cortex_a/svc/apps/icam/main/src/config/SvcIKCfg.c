/**
 *  @file SvcIKCfg.c
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
 *  @details svc IK config
 *
 */

#include "AmbaDSP.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaSensor.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcVinSrc.h"
#include "SvcResCfg.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcIK.h"
#include "SvcIKCfg.h"

#include "SvcBufMap.h"

#define SVC_LOG_IK_CFG      "IK_CFG"

#define SVC_IK_CFG_NONE     (0U)
#define SVC_IK_CFG_NEED     (1U)
#define SVC_IK_CFG_DONE     (2U)

static AMBA_IK_CONTEXT_SETTING_s g_SvcIKSetting = {0};
static UINT32 g_SvcIKConfigure = SVC_IK_CFG_NONE;

static UINT32 IKCfg_Prepare(void);

/**
 * IK configuration initailization
 * @return ErrorCode
 */
UINT32 SvcIKCfg_Init(void)
{
    UINT32 Rval, Size;
    ULONG  Base;

    SvcLog_DBG(SVC_LOG_IK_CFG, "@@ IKInit begin", 0U, 0U);

    /* IK initialized setting */
    Rval = IKCfg_Prepare();
    if (Rval == SVC_OK) {
        Rval = SvcIK_Init(&g_SvcIKSetting);
    }
    if (Rval == SVC_OK) {
        /* re-init IK */
        Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_IK_WRK, &Base, &Size);

        if (SVC_OK == Rval) {
            Rval = SvcIK_InitArch(Base, Size);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_IK_CFG, "## fail to config IK", 0U, 0U);
            } else {
                /* One shot function */
                g_SvcIKConfigure = SVC_IK_CFG_DONE;
            }
        } else {
            SvcLog_NG(SVC_LOG_IK_CFG, "## fail to request IK buffer", 0U, 0U);
        }
    }

    SvcLog_DBG(SVC_LOG_IK_CFG, "@@ IKInit end", 0U, 0U);

    return Rval;
}

/**
 * IK re-configuration if necessary
 * @return ErrorCode
 */
UINT32 SvcIKCfg_Config(void)
{
    UINT32 Rval;

    SvcLog_DBG(SVC_LOG_IK_CFG, "@@ SvcIKCfg_Config start", 0U, 0U);

    /* IK initialized setting, might be overrided when liveview configured */
    Rval = IKCfg_Prepare();

    SvcLog_DBG(SVC_LOG_IK_CFG, "@@ SvcIKCfg_Config end", 0U, 0U);

    return Rval;
}

/**
 * IK context number getting function
 * @param [out] pCtxNum pointer to IK context number
 * @return ErrorCode
 */
UINT32 SvcIKCfg_GetTotalCtxNum(UINT32 *pCtxNum)
{
    *pCtxNum = g_SvcIKSetting.ContextNumber;
    return SVC_OK;
}

/**
 * get IK context setting
 * @param [out] ppSetting pointer to &g_SvcIKSetting
 * @return ErrorCode
 */
UINT32 SvcIKCfg_GetSetting(AMBA_IK_CONTEXT_SETTING_s **ppSetting)
{
    UINT32 Addr;
    AMBA_IK_CONTEXT_SETTING_s *pSetting;
    *ppSetting = &g_SvcIKSetting;
    pSetting = &g_SvcIKSetting;
    AmbaMisra_TypeCast(&Addr, &pSetting);
    SvcLog_OK(SVC_LOG_IK_CFG, "SvcIKCfg_GetSetting 0x%x", Addr, 0U);
    AmbaMisra_TouchUnused(pSetting);
    return SVC_OK;
}

static UINT32 IKCfg_Prepare(void)
{
    static AMBA_IK_ABILITY_s g_SvcIKCtxAbi[MAX_CONTEXT_NUM];

    UINT32 FovIdx[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum;
    UINT32 FovSrc;
    UINT32 Configure;
    UINT32 i;
    UINT32 Rval;
    UINT32 MaxIkCtxNum, MaxIkCfgNum;

    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

#ifdef CONFIG_ICAM_IK_MAX_CTX_NUM
    MaxIkCtxNum = CONFIG_ICAM_IK_MAX_CTX_NUM;
    AmbaMisra_TouchUnused(&MaxIkCtxNum);
    if (MaxIkCtxNum > MAX_CONTEXT_NUM) {
        MaxIkCtxNum = MAX_CONTEXT_NUM;
    }
    AmbaMisra_TouchUnused(&MaxIkCtxNum);
#else
    MaxIkCtxNum = MAX_CONTEXT_NUM;
#endif

#ifdef CONFIG_ICAM_IK_MAX_CFG_NUM
        MaxIkCfgNum = CONFIG_ICAM_IK_MAX_CFG_NUM;
        AmbaMisra_TouchUnused(&MaxIkCfgNum);
#if defined(MAX_CONFIG_NUM)
        if (MaxIkCfgNum > MAX_CONFIG_NUM) {
            MaxIkCfgNum = MAX_CONFIG_NUM;
        }
#elif defined(MAX_CR_RING_NUM)
        if (MaxIkCfgNum > MAX_CR_RING_NUM) {
            MaxIkCfgNum = MAX_CR_RING_NUM;
        }
#endif
#else
#if defined(MAX_CONFIG_NUM)
        MaxIkCfgNum = MAX_CONFIG_NUM;
#elif defined(MAX_CR_RING_NUM)
        MaxIkCfgNum = MAX_CR_RING_NUM;
#endif
#endif

    Rval = SvcResCfg_GetFovIdxs(FovIdx, &FovNum);

    if (Rval == SVC_OK) {
        if (g_SvcIKSetting.ContextNumber == 0U) {
            Configure = SVC_IK_CFG_NEED;
        } else {
            if (FovNum != g_SvcIKSetting.ContextNumber) {
                Configure = SVC_IK_CFG_NEED;
            } else {
                Configure = SVC_IK_CFG_NONE;
            }
        }
        if (Configure == SVC_IK_CFG_NEED) {
            AmbaSvcWrap_MisraMemset(&g_SvcIKCtxAbi, 0, sizeof(g_SvcIKCtxAbi));
            /* Configure video context */
            for (i = 0U; i < MaxIkCtxNum; i++) {
                if (i < AMBA_DSP_MAX_VIEWZONE_NUM) {
                    g_SvcIKCtxAbi[i].Pipe = AMBA_IK_PIPE_VIDEO;
                    g_SvcIKCtxAbi[i].StillPipe = AMBA_IK_STILL_MAX;
                    if (i < FovNum) {
                        FovSrc = 255U;
                        Rval = SvcResCfg_GetFovSrc(i, &FovSrc);

                        if (Rval == SVC_OK) {
                            if ((FovSrc == SVC_VIN_SRC_SENSOR) || (FovSrc == SVC_VIN_SRC_MEM)) {
                                /* The FOV only generate RAW, not go through whole R2Y pipeline */
                                if (pResCfg->FovCfg[i].PipeCfg.PipeMode == SVC_VDOPIPE_MIPIRAW) {
#if defined(AMBA_IK_VIDEO_Y2Y_MIPI)
                                    g_SvcIKCtxAbi[i].VideoPipe = AMBA_IK_VIDEO_Y2Y_MIPI;
#endif
                                } else {
                                    g_SvcIKCtxAbi[i].VideoPipe = AMBA_IK_VIDEO_LINEAR;
                                }
#if defined(CONFIG_ICAM_PIPE_PIPVIN_YUV_SUPPORTED)
                            } else if (FovSrc == SVC_VIN_SRC_YUV) {
                                UINT32 VinId;
                                Rval = SvcResCfg_GetVinIDOfFovIdx(i, &VinId);
                                if (Rval == SVC_OK) {
                                    if (0U == VinId) {
#if defined(AMBA_IK_VIDEO_Y2Y_MIPI)
                                        if (pResCfg->FovCfg[i].PipeCfg.PipeMode == SVC_VDOPIPE_MIPIYUV) {
                                            g_SvcIKCtxAbi[i].VideoPipe = AMBA_IK_VIDEO_Y2Y_MIPI;
                                        } else {
                                            g_SvcIKCtxAbi[i].VideoPipe = AMBA_IK_VIDEO_Y2Y;
                                        }
#else
                                      g_SvcIKCtxAbi[i].VideoPipe = AMBA_IK_VIDEO_Y2Y;
#endif
                                    } else {
#if defined(AMBA_IK_VIDEO_Y2Y_MIPI)
                                        g_SvcIKCtxAbi[i].VideoPipe = AMBA_IK_VIDEO_Y2Y_MIPI;
#endif
                                    }
                                }
#endif
                            } else {
                                g_SvcIKCtxAbi[i].VideoPipe = AMBA_IK_VIDEO_Y2Y;
                            }
                        }
                    } else {
                        g_SvcIKCtxAbi[i].VideoPipe = AMBA_IK_VIDEO_LINEAR;
                    }

                    g_SvcIKSetting.ConfigSetting[i].ConfigNumber = MaxIkCfgNum;
                    g_SvcIKSetting.ConfigSetting[i].pAbility     = &(g_SvcIKCtxAbi[i]);
                }
            }

#ifdef CONFIG_ICAM_STLCAP_USED
            if (pResCfg->StillCfg.EnableStill == 1U) {

                if (MaxIkCtxNum < (FovNum + 1U)) {
                    SvcLog_NG(SVC_LOG_IK_CFG, "MaxIkCtxNum %u not enought for PIV (FovNum %u)", MaxIkCtxNum, FovNum);
                }

#ifndef CONFIG_SOC_CV2FS
                if (pResCfg->StillCfg.EnableHISO == 1U) {
                    if (MaxIkCtxNum >= (FovNum + 2U)) {
                        /* user should reserve one ik ctx for still HISO */
                        i = MaxIkCtxNum - 1U;
                        g_SvcIKCtxAbi[i].Pipe      = AMBA_IK_PIPE_STILL;
                        g_SvcIKCtxAbi[i].StillPipe = AMBA_IK_STILL_HISO;
                    } else {
                        SvcLog_NG(SVC_LOG_IK_CFG, "MaxIkCtxNum %u not enought for still hiso (FovNum %u)", MaxIkCtxNum, FovNum);
                    }
                }
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                if (pResCfg->StillCfg.EnableStillRaw == 1U) {
                    if (MaxIkCtxNum >= (FovNum + 2U)) {
                        /* user should reserve one ik ctx for still LISO r2y with CE */
                        i = MaxIkCtxNum - 2U;
                        g_SvcIKCtxAbi[i].Pipe      = AMBA_IK_PIPE_STILL;
                        g_SvcIKCtxAbi[i].StillPipe = AMBA_IK_STILL_LISO;
                    } else {
                        SvcLog_NG(SVC_LOG_IK_CFG, "MaxIkCtxNum %u not enought for still liso with CE (FovNum %u)", MaxIkCtxNum, FovNum);
                    }
                    if (MaxIkCtxNum >= (FovNum + 3U)) {
                        /* user should reserve one ik ctx for still LISO r2y without CE*/
                        i = MaxIkCtxNum - 3U;
                        g_SvcIKCtxAbi[i].Pipe      = AMBA_IK_PIPE_VIDEO;
                        g_SvcIKCtxAbi[i].StillPipe = AMBA_IK_VIDEO_LINEAR;
                    } else {
                        SvcLog_NG(SVC_LOG_IK_CFG, "MaxIkCtxNum %u not enought for still liso without CE (FovNum %u)", MaxIkCtxNum, FovNum);
                    }
                }
#endif
            }
#endif

            g_SvcIKSetting.ContextNumber = MaxIkCtxNum;

            SvcLog_OK(SVC_LOG_IK_CFG, "## g_SvcIKSetting.ContextNumber %u", g_SvcIKSetting.ContextNumber, 0U);
        }
        g_SvcIKConfigure = Configure;

        Rval = SVC_OK;
    } else {
        Rval = SVC_NG;
    }

    return Rval;
}
