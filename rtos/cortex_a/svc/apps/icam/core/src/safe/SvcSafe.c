/**
 *  @file SvcSafe.c
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
 *  @details Svc Image
 *
 */

#include "AmbaTypes.h"
#include "AmbaVIN_Def.h"

#include "AmbaDSP_Capability.h"

#include "SvcVinSrc.h"

#include "SvcSafe.h"
#include "AmbaMonMain.h"

static SVC_SAFE_CFG_s SvcSafeCfg;

/**
 *  Svc safe init
 */
void SvcSafe_Init(void)
{
    /* safety sys init */
    AmbaMonMain_SysInit();
}

/**
 *  Svc safe init ex
 *  @param[in] CoreInclusion core inclusion bits
 */
void SvcSafe_InitEx(UINT32 CoreInclusion)
{
    /* safety sys init ex */
    AmbaMonMain_SysInitEx(CoreInclusion);
}

/**
 *  Svc safe config
 *  @param[in] pSafeCfg pointer to the safe configuration
 */
void SvcSafe_Config(const SVC_SAFE_CFG_s *pSafeCfg)
{
    SVC_SAFE_CMD_MSG_s CmdMsg;
    UINT32 i, j;

    UINT8 SsId;

    for (i = 0U; i < AMBA_NUM_VIN_CHANNEL; i++) {
        if ((pSafeCfg->Select.Vin & (((UINT32) 1U) << i)) > 0U) {
            /* vin src id */
            CmdMsg.Ctx.Data = 0ULL;
            if (pSafeCfg->VinSrc[i] == SVC_VIN_SRC_SENSOR) {
                CmdMsg.Ctx.VinSrc.Id = SVC_SAFE_VIN_SRC_SENSOR;
            } else if (pSafeCfg->VinSrc[i] == SVC_VIN_SRC_YUV) {
                CmdMsg.Ctx.VinSrc.Id = SVC_SAFE_VIN_SRC_YUV;
            } else {
                CmdMsg.Ctx.VinSrc.Id = SVC_SAFE_VIN_SRC_ALT;
            }
            CmdMsg.Ctx.VinSrc.VinIdx = (UINT8) i;
            CmdMsg.Ctx.VinSrc.Cmd = (UINT8) SVC_SAFE_CMD_VIN_SRC;
            AmbaMonMain_Config(CmdMsg.Ctx.Data);
            /* sensor/serdes/fov_vin select id (bits) */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.Vin.FovId = (UINT16) (pSafeCfg->Select.FovInVin[i] & 0xFFFFU);
            CmdMsg.Ctx.Vin.SensorId = (UINT8) (pSafeCfg->Select.Sensor[i] & 0xFFU);
            CmdMsg.Ctx.Vin.SerdesId = (UINT8) (pSafeCfg->Select.Serdes[i] & 0xFFU);
            CmdMsg.Ctx.Vin.VinIdx = (UINT8) i;
            CmdMsg.Ctx.Vin.Cmd = (UINT8) SVC_SAFE_CMD_VIN;
            AmbaMonMain_Config(CmdMsg.Ctx.Data);
            /* sensor id */
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                if ((pSafeCfg->Select.Sensor[i] & (((UINT32) 1U) << j)) > 0U) {
                    CmdMsg.Ctx.Data = 0ULL;
                    CmdMsg.Ctx.Sensor.Id = pSafeCfg->SensorId[i][j];
                    CmdMsg.Ctx.Sensor.SensorIdx = (UINT8) j;
                    CmdMsg.Ctx.Sensor.VinIdx = (UINT8) i;
                    CmdMsg.Ctx.Sensor.Cmd = (UINT8) SVC_SAFE_CMD_SENSOR;
                    AmbaMonMain_Config(CmdMsg.Ctx.Data);
                }
            }
            /* slow shutter id */
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                if ((pSafeCfg->Select.Sensor[i] & (((UINT32) 1U) << j)) > 0U) {
                    CmdMsg.Ctx.Data = 0ULL;
                    SsId = (UINT8) (pSafeCfg->SlowShutterId[i][j] & 0xFFU);
                    CmdMsg.Ctx.SlowShutter.Id = SsId;
                    CmdMsg.Ctx.SlowShutter.VinIdx = (UINT8) i;
                    CmdMsg.Ctx.SlowShutter.Cmd = (UINT8) SVC_SAFE_CMD_SLOW_SHUTTER;
                    AmbaMonMain_Config(CmdMsg.Ctx.Data);
                    break;
                }
            }
            /* boot latency */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.BootLatency.Timetick = pSafeCfg->BootLatency[i];
            CmdMsg.Ctx.BootLatency.VinIdx = (UINT8) i;
            CmdMsg.Ctx.BootLatency.Cmd = (UINT8) SVC_SAFE_CMD_BOOT_LATENCY;
            AmbaMonMain_Config(CmdMsg.Ctx.Data);
        }
    }

    /* vin/fov/vout id (bits) */
    CmdMsg.Ctx.Data = 0ULL;
    CmdMsg.Ctx.Liv.VinId = (UINT16) (pSafeCfg->Select.Vin & 0xFFFFU);
    CmdMsg.Ctx.Liv.FovId = (UINT16) (pSafeCfg->Select.Fov & 0xFFFFU);
    CmdMsg.Ctx.Liv.VoutId = (UINT8) (pSafeCfg->Select.Vout & 0xFFU);
    CmdMsg.Ctx.Liv.TdId = (UINT16) (pSafeCfg->Select.Td & 0xFFFFU);
    CmdMsg.Ctx.Liv.Cmd = (UINT8) SVC_SAFE_CMD_START;
    AmbaMonMain_Config(CmdMsg.Ctx.Data);

    /* cfg update */
    SvcSafeCfg = *pSafeCfg;
}

/**
 *  Svc safe stop
 */
void SvcSafe_Stop(void)
{
    SVC_SAFE_CMD_MSG_s CmdMsg;

    CmdMsg.Ctx.Data = 0ULL;
    CmdMsg.Ctx.Liv.VinId = (UINT16) (SvcSafeCfg.Select.Vin & 0xFFFFU);
    CmdMsg.Ctx.Liv.FovId = (UINT16) (SvcSafeCfg.Select.Fov & 0xFFFFU);
    CmdMsg.Ctx.Liv.VoutId = (UINT8) (SvcSafeCfg.Select.Vout & 0xFFU);
    CmdMsg.Ctx.Liv.TdId = (UINT16) (SvcSafeCfg.Select.Td & 0xFFFFU);
    CmdMsg.Ctx.Liv.Cmd = (UINT8) SVC_SAFE_CMD_STOP;
    AmbaMonMain_Config(CmdMsg.Ctx.Data);
}

