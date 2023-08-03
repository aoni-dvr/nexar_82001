/**
 *  @file SvcSafeTask.c
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
 *  @details svc img task
 *
 */

#include "AmbaTypes.h"

#include "AmbaVIN_Def.h"

#include "AmbaDSP_Capability.h"
#include "AmbaDSP_Liveview.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcResCfg.h"

#include "SvcSafe.h"

#include "SvcSafeTask.h"

#define SVC_LOG_SAFE_TASK "SAFE_TASK"

/**
 *  Svc safe task init
 *  @return error code
 */
UINT32 SvcSafeTask_Init(void)
{
    UINT32 RetVal = SVC_OK;

    SvcLog_DBG(SVC_LOG_SAFE_TASK, "@@ Init Begin", 0U, 0U);

    /* safe init */
    SvcSafe_Init();

    SvcLog_DBG(SVC_LOG_SAFE_TASK, "@@ Init End", 0U, 0U);

    return RetVal;
}

/**
 *  Svc safe task init ex
 *  @param[in] CoreInclusion core inclusion bits
 *  @return error code
 */
UINT32 SvcSafeTask_InitEx(UINT32 CoreInclusion)
{
    UINT32 RetVal = SVC_OK;

    SvcLog_DBG(SVC_LOG_SAFE_TASK, "@@ Init Ex (0x%08lx) Begin", CoreInclusion, 0U);

    /* safe init */
    SvcSafe_InitEx(CoreInclusion);

    SvcLog_DBG(SVC_LOG_SAFE_TASK, "@@ Init Ex (0x%08lx) End", CoreInclusion, 0U);

    return RetVal;
}

/**
 *  Svc safe task config
 *  @return error code
 */
UINT32 SvcSafeTask_Config(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 FuncRetVal;
    UINT32 i, j, k;

    UINT32 VinSrc = 0U;

    UINT32 VinNum = 0U;
    UINT32 VinIdx[AMBA_NUM_VIN_CHANNEL] = {0};

    UINT32 FovNum = 0U;
    UINT32 FovIdx[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};

    UINT32 SensorNum = 0U;
    UINT32 SensorIdx[SVC_NUM_VIN_SENSOR] = {0};

    UINT32 SensorId = 0U;
    UINT32 SerdesIdx = 0U;

    const SVC_RES_CFG_s *pResCfg;
    SVC_SAFE_CFG_s SafeCfg = {0};

    pResCfg = SvcResCfg_Get();

    /* vin/sensor/serdes/fov select bits */
    SafeCfg.Select.Vin = 0U;
    SafeCfg.Select.Fov = 0U;
    SafeCfg.Select.Td = 0U;
    FuncRetVal = SvcResCfg_GetVinIDs(VinIdx, &VinNum);
    if (FuncRetVal == SVC_OK) {
        for (i = 0U; i < VinNum; i++) {
            /* vin src get */
            FuncRetVal = SvcResCfg_GetVinSrc(VinIdx[i], &VinSrc);
            if (FuncRetVal == SVC_OK) {
                SafeCfg.VinSrc[VinIdx[i]] = VinSrc;
            }
            /* vin select bits */
            SafeCfg.Select.Vin |= (((UINT32) 1U) << (VinIdx[i] & 0x1FU));
            /* sensor/serdes/fov_vin select bits reset */
            SafeCfg.Select.Sensor[VinIdx[i]] = 0U;
            SafeCfg.Select.Serdes[VinIdx[i]] = 0U;
            SafeCfg.Select.FovInVin[VinIdx[i]] = 0U;
            /* sensor idx get */
            FuncRetVal = SvcResCfg_GetSensorIdxsInVinID(VinIdx[i], SensorIdx, &SensorNum);
            if (FuncRetVal == SVC_OK) {
                for (j = 0U; j < SensorNum; j++) {
                    /* sensor select bits */
                    SafeCfg.Select.Sensor[VinIdx[i]] |= (((UINT32) 1U) << (SensorIdx[j] & 0x1FU));
                    FuncRetVal = SvcResCfg_GetSensorIDOfSensorIdx(VinIdx[i], SensorIdx[j], &SensorId);
                    if (FuncRetVal == SVC_OK) {
                        /* sensor id */
                        SafeCfg.SensorId[VinIdx[i]][SensorIdx[j]] = SensorId;
                    }
                    /* serdes idx get */
                    FuncRetVal = SvcResCfg_GetSerdesIdxOfSensorIdx(VinIdx[i], SensorIdx[j], &SerdesIdx);
                    if (FuncRetVal == SVC_OK) {
                        /* serdes select bits */
                        SafeCfg.Select.Serdes[VinIdx[i]] |= (((UINT32) 1U) << (SerdesIdx & 0x1FU));
                    }
                    /* sensor fov idx get */
                    FuncRetVal = SvcResCfg_GetFovIdxsInSensorIdx(VinIdx[i], SensorIdx[j], FovIdx, &FovNum);
                    if (FuncRetVal == SVC_OK) {
                        /* fov select bits */
                        for (k = 0U; k < FovNum; k++) {
                            SafeCfg.Select.Fov |= (((UINT32) 1U) << (FovIdx[k] & 0x1FU));
                            SafeCfg.Select.FovInVin[VinIdx[i]] |= (((UINT32) 1U) << (FovIdx[k] & 0x1FU));
                        }
                    }
                    /* slow shutter id get */
                    SafeCfg.SlowShutterId[VinIdx[i]][SensorIdx[j]] = (pResCfg->SensorCfg[VinIdx[i]][SensorIdx[j]].SensorGroup >> 12U) & 0x3U;
#ifdef CONFIG_BUILD_IMGFRW_SMC
                    SafeCfg.SlowShutterId[VinIdx[i]][SensorIdx[j]] |= (((pResCfg->SensorCfg[VinIdx[i]][SensorIdx[j]].SensorGroup >> 30U) & 0x1U) << 2U);
#endif
                    SafeCfg.SlowShutterId[VinIdx[i]][SensorIdx[j]] |= ((pResCfg->SensorCfg[VinIdx[i]][SensorIdx[j]].SsAuxId & 0xFU) << 4U);
                }
            }
            /* time division id get */
            if (pResCfg->VinCfg[VinIdx[i]].SubChanCfg[0].TDNum > 1U) {
                SafeCfg.Select.Td |= (((UINT32) 1U) << (VinIdx[i] & 0x1FU));
            }
            /* boot latency get */
            SafeCfg.BootLatency[VinIdx[i]] = pResCfg->VinCfg[VinIdx[i]].TimeoutCfg.Boot;
        }
    } else {
        RetVal = SVC_NG;
    }

    if (VinNum != 0U) {
        /* vout select bits */
        SafeCfg.Select.Vout = 0U;
        for (i = 0U; i < pResCfg->DispNum; i++) {
            SafeCfg.Select.Vout |= (((UINT32) 1U) << (pResCfg->DispStrm[i].VoutID & 0x1FU));
        }

        SvcSafe_Config(&SafeCfg);
    }

    return RetVal;
}

/**
 *  Svc safe task stop
 *  @return error code
 */
UINT32 SvcSafeTask_Stop(void)
{
    UINT32 RetCode = SVC_OK;

    SvcSafe_Stop();

    return RetCode;
}

